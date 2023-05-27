/////////////////////////////////////////////////////////////////
// BH_CycleState_MegaDa.cpp : Cycle Operation State of the CBondHead class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wed, Nov 6, 2019
//	Revision:	1.00
//
//	By:			Matthew Law
//				AAA Software Group 
//
//	Copyright @ ASM Assembly Automation Ltd., 2019.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "CycleState.h"
#include "math.h"
#include "BondPr.h"
#include "Mmsystem.h"
#include "WT_SubRegion.h"
#include "BH_AirFlowCompensation.h"
#include "WaferTable.h"
#include "WaferPr.h"
#include "BinTable.h"
#include "PRFailureCaseLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CBondHead::RunOperation_MegaDa()
{
	CString szMsg, szTemp;
	LONG	lTemp = 0, lDelay = 0, lSPCResult = 0;
	LONG	lTotal = 0;
	LONG	lEjectorLvl			= 0;
	LONG	lBinEjectorLvl		= 0;
	LONG	lEjectorThetaLvl	= 0;
	LONG	lBinEjectorThetaLvl	= 0;
	LONG	lTime_Ej	= m_lTime_Ej;
	LONG	lDnTime_Ej	= m_lDnTime_Ej;	
	BOOL	bCriticalError=FALSE;
	BOOL	bWaferEnd, bColletFull, bLFDie;
	BOOL	bRegionEnd = FALSE;
	double	dCurrentTime = 0;
	static  LONG	lArmPicBondDelay = 0;
	static	BOOL	s_bUpdateNVRAM = TRUE;
	static	BOOL	s_bDownToPick = TRUE;
	static	DOUBLE	s_dbStartZ2EjUp = 0;
	static	DOUBLE	s_dbStartZ2PK = 0, s_dbStartT2PB = 0, s_dbSettling = 100;
	static	DOUBLE  s_dArmPickUpStart = 0, s_dBondZDownStart = 0;
	static	DOUBLE	s_dbStartEjTMove = 0, s_dbStartBinEjTMove = 0;
	static	DOUBLE	s_dbStartEjUp = 0;
	static  DOUBLE  s_dEjectorCapStartTime = 0;
	static  DOUBLE  s_dTotalTime_Ej = 0;
	static  DOUBLE  s_dBHEJSyncTime = 0;
	LONG lMotionTime_BT = 0;
	LONG lTMoveDelay = 0, lTMaxUnblockTime = 0, lTMinPrUnblockTime = 0;
	DOUBLE dWTIndexStart = 0, dWTIndexDuration = 0;
	LONG lEjVacOffDelay = 0;
	LONG lWTIndexDuration	= 0;
	LONG lEjCompleteTime	= 0;				
	LONG lEjDnDuration		= 0;				
	DOUBLE dEjDuration		= 0;				
	LONG lWTLongJumpExDelay = 0;
	LONG lWTLongJumpMotionTime = 0;
	BOOL bIsNeedRestartUpdate = FALSE;
	BOOL bPostBondNextCycleStop = FALSE;		//Cree HuiZhou
	BOOL bWTNeedRotate = FALSE;
	INT nResult = gnOK;
	BOOL bMS60NGPickNoLFCycle = FALSE;			
	BOOL bUpLookFailAtMD	= FALSE;
	LONG lLastPickRow		= 0;
	LONG lLastPickCol		= 0;
	BOOL bStatus			= TRUE;
	LONG lEnableSyncMotionViaEjCmdExtraTime = 0; //if enable SyncMotionViaEjCmd, it need more than 2 ms for ej up move
	BOOL bBHMarkStop = FALSE;
	LONG EmptyCounter, EmptyCounter2, AccEmptyCounter, AccEmptyCounter2, AccEmptyCounterTotal, GenMaxEmptyAllow, GenMaxAccEmptyAllow;
	BOOL bEjUseContactLvl = FALSE;
	BOOL bBprUseHWTriggerMode = FALSE;
	LONG lSynUpTime_Ej = 0;

	static BOOL bSwitchPR			= FALSE;
	BOOL bEnableWTIndexTest = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableWTIndexTest"];
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		DisplaySequence("BH - STOP");
		State(STOPPING_Q);
		return ;
	}

	m_bStep = FALSE;

	//Support of default HeadPick/HeadBond delay = -1
	LONG lHeadPickDelay = m_lHeadPickDelay;
	if (m_lHeadPickDelay <= -1)
	{
		lHeadPickDelay = 1;
	}
	
	LONG lHeadBondDelay = m_lHeadBondDelay;
	if (m_lHeadBondDelay <= -1)
	{
		lHeadBondDelay = 1;
	}
	
	if (m_lUpTime_Ej_Theta > m_lUpTime_BinEj_Theta)
	{
		lHeadBondDelay += abs(m_lUpTime_Ej_Theta - m_lUpTime_BinEj_Theta);
	}
	else
	{
		lHeadPickDelay += abs(m_lUpTime_BinEj_Theta - m_lUpTime_Ej_Theta);
	}

	LONG lArmPickDelay = m_lArmPickDelay;
	if (m_lArmPickDelay <= -1)
	{
		lArmPickDelay = 1;
	}

	LONG lArmBondDelay = m_lArmBondDelay;
	if (m_lArmBondDelay <= -1)
	{
		lArmBondDelay = 1;
	}

	LONG lEjectorUpDelay	= m_lEjectorUpDelay;
	LONG lEjectorDownDelay	= m_lEjectorDownDelay;

	LONG lExtraPrGrabDelay = (*m_psmfSRam)["WaferPr"]["ExtraGrabDelay"];

	LONG lAutobondCurrTime = 0;
	LONG lWTStop		= (*m_psmfSRam)["WaferTable"]["AtSafePos"];
	LONG lBTStop		= (*m_psmfSRam)["BinTable"]["AtSafePos"];


	BOOL bEMOStop = FALSE;
	if (CMS896AStn::m_bEMOChecking)
	{
		m_lEMOCheckCounter++;
		if (m_lEMOCheckCounter > 20)
		{
			m_lEMOCheckCounter = 0;
			bEMOStop = IsEMOTriggered();
			if (bEMOStop)
			{
				bCriticalError = TRUE;
				//(*m_psmfSRam)["MS899"]["CriticalError"] = bCriticalError;
				SetMotionCE(TRUE, "EMO Check fail");
			}
		}
	}

	if (IsMS60() && CMS896AStn::m_bDBHThermostat)
	{
		m_lMS60TempCheckCounter++;

		if (m_lMS60TempCheckCounter > MS60_BH_THERMALCHECK_CYCLE)
		{
			m_lMS60TempCheckCounter = 0;
			if (IsMS60TempertureOverLoaded(TRUE, TRUE))		
			{
				SetBondHeadFan(TRUE);

				HmiMessage_Red_Yellow("BonHead Temperature is overheat!  Machine is aborted!");
				bCriticalError = TRUE;
				//(*m_psmfSRam)["MS899"]["CriticalError"] = bCriticalError;
				SetMotionCE(TRUE, "MS60 Temperature is over-heat");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
		}
	}

	try
	{
		if (IsMotionCE() == TRUE)
		{
			DisplaySequence("BH - critical error ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case WAIT_WT_Q:
			DisplaySequence("BH - WAIT_WT_Q");

			bWaferEnd	= IsWaferEnded();

			if (IsAutoRescanWafer() && (GetWftSampleState()==1 || GetWftCleanEjPinState()==1) && m_bFirstCycle)	// wait wt, first cycle
			{
				CString szMsg;
				if (GetWftSampleState()==1)
				{
					m_lReadyToSampleOnWft = 2;	//	wait wt, first cycle
					szMsg = "BH - DBH at wait wt first cycle, to sample from 1 to 2";
				}
				else
				{
					m_lReadyToCleanEjPinOnWft = 2;	//	wait wt, first cycle
					szMsg = "BH - DBH at wait wt first cycle, to Clean Ej Pin from 1 to 2";
				}
				
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				pUtl->PrescanMoveLog(szMsg);
				DisplaySequence(szMsg);
			}

			// Check whether the collet & ejector count is larger than the user-defined maximum
			if (lWTStop)
			{
				if (m_bStop)
				{
					m_bDBHStop = TRUE;
					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					DisplaySequence("BH - Wait WT lWTstop ==> T Prepick to pick");
				}
				else
				{
					DisplaySequence("BH - WT stop ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			else if (IsMaxColletEjector() && !m_bEjPinNeedReplacementAtUnloadWafer)
			{
				m_bStop = TRUE;
				m_bDBHStop = TRUE;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait WT reach life limit ==> T Prepick to pick");
			}
			else if (WaitWTReady() || bWaferEnd || IsSortTo2ndPart() || IsOnlyRegionEnd())	//add WaferEnd checking
			{	
				//TakeTime(LC4);	
				SetWTReady(FALSE, "WAIT_WT_Q");
				SetPRStart(FALSE);
				SetPRLatched(FALSE);
				SetEjectorReady(FALSE, "1");	//	Wait WT Q; WT moved or wafer/region end
				SetDieBondedForWT(FALSE);
				SetConfirmSrch(FALSE);
				SetWTStartMove(FALSE);				//Clear it once again as die-ready means WT not yet indexed for sure!
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait WT ready ==> T Prepick to pick");
			}
			else if (m_bStop)	// && 	
			{
				DisplaySequence("BH - Wait WT ==> HK");
				m_qSubOperation = HOUSE_KEEPING_Q;	
			}
			else
			{
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

				if (s_bUpdateNVRAM == TRUE)
				{
					SetBackupNVRam(TRUE);
					s_bUpdateNVRAM = FALSE;
				}
			}
			break;


		case MOVE_T_PREPICK_TO_PICK_Q:
			DisplaySequence("BH - MOVE_T_PREPICK_TO_PICK_Q");
			if (IsAutoSampleSort())	// rescan block camera
			{
				if (IsAutoRescanWafer())
				{
					if (m_bStop)
					{
						DisplaySequence("BH - |Wait rescanning| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					szMsg = "BH - DBH at T prepick to pick, rescanning";
					DisplaySequence(szMsg);
					break;
				}

				if (GetWftSampleState() > 1 && m_bFirstCycle == FALSE)
				{
					if (m_bStop)
					{
						DisplaySequence("BH - |Wait sampling| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					szMsg = "BH - DBH at T prepick to pick, sampling waiting";
					DisplaySequence(szMsg);
					break;
				}
			}

			if (GetWftSampleState() == 1)
			{
				CString szMsg;
				m_lReadyToSampleOnWft = 2;
				szMsg = "BH - DBH at T prepick to pick, to sample from 1 to 2";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				DisplaySequence(szMsg);
			}
			else if (GetWftCleanEjPinState() == 1)
			{
				CString szMsg;
				m_lReadyToCleanEjPinOnWft = 2;
				szMsg = "BH - DBH at T prepick to pick, to Clean Ej Pin from 1 to 2";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				DisplaySequence(szMsg);
			}

			if (IsOnlyRegionEnd())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				pUtl->RegionOrderLog("T Prepick to pick q, region end, to house keeping");
			}
			else if (AllowMove() == TRUE)
			{
				bWaferEnd		= IsWaferEnded();
				LONG lRefDieCheck	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];

				//** Wait for BT-Start to avoid extra head-PICK delay at PICK side **//
				if (m_bStop)
				{
					szMsg.Format("BH - T Prepick to pick bstop |firstcycle=%d or changegrade=%d| ==> HK",
						m_bFirstCycle, m_bChangeGrade);
					DisplaySequence(szMsg);
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}

				if (!WaitBTStartMove(200))
				{
					if (m_bStop)
					{
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}

					DisplaySequence("BH - MS90 T Prepick to pick wait BT start move");
					break;
				}	//	for MS90 only
	
				//andrewng //2020-0727
				if ( (m_lFloatBlowDelay > 0) && (m_lFloatBlowDelay > CMS896AStn::m_lABDummyCounter) )
				{
					if ( WaitBTStartMove() )
					{
						m_qSubOperation = ILC_RESTART_UPDATE_Q;
						DisplaySequence("BH - T Prepick to pick ==> ILC Restart update q");
					}
					break;
				}

				if (bWaferEnd && lRefDieCheck == 1)
				{
					DisplaySequence("BH - |Wafer End, at prepick, stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}

				CheckCoverOpenInAuto("at BH PrePick state");	

				//m_bComplete_T = FALSE;
				//m_bMoveFromPrePick = TRUE;		//Sanan	//Enable Head-PrePick delay for Sanan MS100+
				//m_lTime_T = m_lPrePickTime_T;
				//s_dbStartT2PB = GetTime();
				//(*m_psmfSRam)["BondHead"]["AtPrePick"] = 0;
				//(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
				//m_dWaitBTStartTime	= GetTime();
	
				m_qSubOperation		= EJ_THETA_UP_Q;//PICK_Z_DOWN_Q;
				DisplaySequence("BH - T Prepick to Pick ==> Pick Z Down");
			}
			break;

		case ILC_RESTART_UPDATE_Q:		//andrewng //2020-0727
			if (m_bStop)	// && 	
			{
				DisplaySequence("BH - ILC ==> HouseKeeping");
				m_qSubOperation = HOUSE_KEEPING_Q;	
			}
			else if (CMS896AStn::m_lABDummyCounter < m_lFloatBlowDelay)
			{
				if (CMS896AStn::m_lABDummyCounter == 0)
				{
					Sleep(100);
					MoveWaferTableToDummyPos(TRUE);
					MoveBinTableToDummyPos(TRUE);
					Sleep(100);
				}

				lEjectorThetaLvl	= m_lUpLevel_EJ_Theta;
				lBinEjectorThetaLvl	= m_lUpLevel_Bin_EJ_Theta;

				EjT_MoveTo(lEjectorThetaLvl,		SFM_NOWAIT);
				BinEjT_MoveTo(lBinEjectorThetaLvl,	SFM_NOWAIT);			
				Sleep(m_lHeadPickDelay);
				EjT_Sync();
				BinEjT_Sync();

				lEjectorLvl			= m_lEjectLevel_Ej;
				lBinEjectorLvl		= m_lUpLevel_Bin_EJ_Pin;
		
				if ( !m_bEnableSyncMotionViaEjCmd && 
					 (m_lContactLevel_Ej > m_lStandbyLevel_Ej) && (m_lContactLevel_Ej <= m_lEjectLevel_Ej) )
				{
					lEjectorLvl			= m_lContactLevel_Ej;
					bEjUseContactLvl	= TRUE;
				}

				Ej_MoveTo(lEjectorLvl,				SFM_NOWAIT);
				BinEj_MoveTo(lBinEjectorLvl,		SFM_NOWAIT);

				Ej_Sync();
				BinEj_Sync();

				if ((m_lSyncZOffset > 0 || bEjUseContactLvl) && (m_lSyncZOffset <= 1000))
				{
					if ((lEjectorUpDelay > 0) && (lEjectorUpDelay <= 1000))
					{
						Sleep(lEjectorUpDelay);
					}

					LONG lEjSyncOffsetZ		= m_lSyncZOffset;
					LONG lBinEjSyncZOffsetZ = m_lSyncZOffset;
					if (bEjUseContactLvl)
					{
						lEjSyncOffsetZ		= m_lEjectLevel_Ej - m_lContactLevel_Ej;
						if ( m_lSyncZOffset < (m_lEjectLevel_Ej - m_lContactLevel_Ej) )
						{
							lBinEjSyncZOffsetZ	= m_lEjectLevel_Ej - m_lContactLevel_Ej - m_lSyncZOffset;
						}
						else
						{
							lBinEjSyncZOffsetZ	= m_lEjectLevel_Ej - m_lContactLevel_Ej;
						}
					}

					//Ej_SelectObwProfile(m_lSyncZOffset);
					//BinEj_SelectObwProfile(m_lSyncZOffset);
					Ej_SelectControl(lEjSyncOffsetZ,		TRUE);
					BinEj_SelectControl(lBinEjSyncZOffsetZ, TRUE);

					TakeTime(LC1);
					Ej_Move(lEjSyncOffsetZ,			SFM_NOWAIT);
					BinEj_Move(lBinEjSyncZOffsetZ,	SFM_NOWAIT);
							
					Ej_Sync();
					BinEj_Sync();

					Ej_SelectControl(lEjSyncOffsetZ,		FALSE);
					BinEj_SelectControl(lBinEjSyncZOffsetZ, FALSE);
				}

				Sleep(m_lPickDelay);

				Ej_MoveTo(m_lStandbyLevel_Ej,			SFM_NOWAIT);						
				BinEj_MoveTo(m_lStandby_Bin_EJ_Pin,		SFM_NOWAIT);
				Sleep(m_lArmPickDelay);
				Ej_Sync();
				BinEj_Sync();

				EjT_MoveTo(m_lStandby_EJ_Theta,			SFM_NOWAIT);
				BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta,	SFM_NOWAIT);

				EjT_Sync();
				BinEjT_Sync();

				Sleep(20);

				CMS896AStn::m_lABDummyCounter++;

				if (CMS896AStn::m_lABDummyCounter >= m_lFloatBlowDelay)
				{
					Sleep(100);
					BOOL bStatus1 = MoveWaferTableToDummyPos(FALSE);
					BOOL bStatus2 = MoveBinTableToDummyPos(FALSE);
					Sleep(100);

					if (!bStatus1 || !bStatus2)
					{
						HmiMessage_Red_Yellow("Table Error in DUMMY pos to BACKUP pos");
						m_qSubOperation = HOUSE_KEEPING_Q;
						DisplaySequence("BH - ILC ==> ERR to HouseKeeping");
						break;
					}
				}
			}
			else
			{
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - ILC ==> T Prepick to pick");
			}
			break;


		case WAIT_WT_READY_Q:	// must after die picked
			DisplaySequence("Mega Da BH - WAIT_WT_READY_Q");
			// Check whether Z is completed
			
			bWaferEnd	= IsWaferEnded();
			bRegionEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"];	// xuzhijin_region_end 

			if (m_bStop)
			{
				TakeTime(BD);
				SetEjectorReady(FALSE, "3");
				SetDieBondedForWT(FALSE);
				m_dWaitBTStartTime = GetTime();			
				m_qSubOperation = HOUSE_KEEPING_Q;	
				DisplaySequence("BH - Wait WT ready stop ==> HOUSE_KEEPING_Q");
				break;
			}
			else if (WaitWTReady() || bWaferEnd || bRegionEnd || IsOnlyRegionEnd())
			{	
				TakeTime(BD);
				SetConfirmSrch(FALSE);		
				SetWTReady(FALSE, "WAIT_WT_READY_Q");
				//SetEjectorReady(FALSE, "4");	//	Wait WT ready q, WT moved or wafer/region end
				SetDieBondedForWT(FALSE);
				SetWTStartMove(FALSE);						//Clear it once again as die-ready means WT not yet indexed for sure!
				m_dWaitBTStartTime = GetTime();		
				m_qSubOperation = WAIT_BT_READY_Q;	
				DisplaySequence("BH - wait WT ready ok ==> wait BT ready");
			}
			else if (WaitBadDieForT())
			{
				SetBadDieForT(FALSE); 
	//	V450X16	SetEjectorReady(FALSE, "5");
				OpBadDie(TRUE);			
	//	V450X16	SetEjectorReady(TRUE, "5");
				StartTime(GetRecordNumber());	// Restart the time
				OpUpdateDieIndex();				// Update the die index
				m_qSubOperation = WAIT_WT_Q;
				DisplaySequence("BH - Bad die to Wait WT");
				s_bUpdateNVRAM = TRUE;
			}
			else if (WaitConfirmSrch())
			{
				SetConfirmSrch(FALSE);
				//	V450X16	SetEjectorReady(FALSE, "6");	
				OpBadDie(TRUE);					// Treat as bad die case	//BH-T to PRE-PICK
				//	V450X16	SetEjectorReady(TRUE, "6");
				OpUpdateDieIndex();				// Update the die index
				SetBhToPrePick(TRUE);			// Tell WPR that BH is now on PRE-PICK & go for CONFIRM-SEARCH
				m_qSubOperation = WAIT_WT_Q;
				s_bUpdateNVRAM = TRUE;
				DisplaySequence("BH - Confirm search to Wait WT");
			}
			break;

		case WAIT_BT_READY_Q:
			DisplaySequence("BH - WAIT_BT_READY_Q");
			bWaferEnd		= IsWaferEnded();	

			if (lBTStop)
			{
				DisplaySequence("BH - |BT stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (WaitBTStartMoveWithoutDelay(200))
			{
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				SetBTStartMoveWithoutDelay(FALSE);
				DisplaySequence("BH - SetBTStartMoveWithoutDelay ==> FALSE");
				m_qSubOperation = EJ_THETA_UP_Q;//PICK_Z_DOWN_Q;
			}
			else if (IsBTChngGrd() || IsOnlyRegionEnd())
			{
				//Normal CHANGE-GRADE triggering position in MS100 sequence
				Sleep(100);	

				CheckCoverOpenInAuto("MS100 change grade");
				if (!(IsBTChngGrd() && pUtl->GetPrescanRegionMode()))
				{
					(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				}
				//T_MoveTo(m_lPrePickPos_T);

				szMsg = "BH - Wait BT ready ==> T Prepick to pick ";

				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;

				if (pUtl->GetPrescanRegionMode())	// xu_semitek
				{
					m_lBondDieCount++;
					SetBTReady(FALSE); //2018.3.28 
					SetDieBonded(TRUE);
					m_qSubOperation =  WAIT_BT_CHANGE_GRADE;
				}

				CMSLogFileUtility::Instance()->BL_LogStatus("BH: change grade to prepick");
				DisplaySequence(szMsg);
				
				if (IsBTChngGrd())	// HuaMao green light idle, reset ejector ready state
				{
					SetEjectorReady(FALSE, "8");
				}
				break;
			}
			else if (WaitBTReSrchDie())		
			{
				SetBTReSrchDie(FALSE);
				SetWTReady(TRUE, "SetWTReady TRUE WaitBTReSrchDie");
				//T_Sync();			//Must wait T complete for ILC
				Sleep(20);	

				SetErrorMessage("BH: BPR ReSrchDie triggers BH to PrePICK");
				CheckCoverOpenInAuto("MS100 ReSrchDie");
				//T_MoveTo(m_lPrePickPos_T);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = WAIT_WT_Q;
				break;
			}
			else	
			{
				//Emergency STOP loop to avoid sw hangup here !! 
				if (m_bStop)
				{
					if (!WaitBTStartMove(200))		
					{
						SetErrorMessage("BH: abnormal STOP encountered in WAIT_BT_READY state -> STOP");
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
				}
			}
			break;

		case EJ_THETA_UP_Q://PICK_Z_DOWN_Q:
			DisplaySequence("BH - EJ_THETA_UP_Q");
			//DisplaySequence("Mega Da BH - PICK_Z_DOWN_Q");
			m_lSPCResult = (*m_psmfSRam)["BondPr"]["PostBondResult"];
			if (m_bDisableBT)
			{
				m_lSPCResult = 1;
			}
			if (m_lSPCResult != 1)
			{
				//CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
				//if (!pWaferPr->WaferPrLogFailureCaseImage())
				//{
				//	DisplaySequence("BH - EJ_THETA_UP_Q WaferPrLogFailureCaseImage() ==> house keeping");
				//	m_qSubOperation = HOUSE_KEEPING_Q;
				//	break;
				//}

				if (!CheckBPRErrorNeedAlarm(0))
				{
					m_lSPCResult = 1;
					(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
				}
			}

			//if ((lSPCResult != 1) && (pAppMod->GetCustomerName().Find(CTM_SANAN) != -1))
			if (m_lSPCResult != 1)
			{
				OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "1");	//Sanan & Semitek
			}
			if (m_lSPCResult != 1)	
			{
				m_lPrevDieSPCResult = m_lSPCResult;
				OpSetMS100AlarmCode(6);			//BPR PostBond-fail
			}

			s_bDownToPick = FALSE;
			if (AllowMove() == TRUE)
			{
				//Check machine pressure
				if (IsLowPressure() == TRUE)
				{
					SetStatusMessage("Machine low pressure");
					SetErrorMessage("Machine low pressure");
					OpSetMS100AlarmCode(1);			//LowPressure
				}

				//Check machine pressure
				if (IsLowVacuumFlow())
				{
					SetStatusMessage("Machine low vacuum flow in auto");
					SetErrorMessage("Machine low vacuum flow in auto");
					OpSetMS100AlarmCode(16);			//LowPressure
				}

				// Check whether it is suitable time for Collet Jam check and Move Z
				//LONG lPickDelay = 0, lBondDelay = 0;
				//LONG lTMvTm = (LONG)(GetTime() - s_dbStartT2PB);

				//m_bMoveFromPrePick = FALSE;	
				m_bPickDownFirst = TRUE;
				m_nBHZ1BHZ2Delay = 1;

				//MegaDa No BHT
				m_bComplete_T = TRUE;
				//andrewng //2020-0617
				EjT_Sync();
				BinEjT_Sync();

				if (bEnableWTIndexTest)
				{
					//Do not move Ejector Pin and Theta
					lEjectorThetaLvl	= m_lStandby_EJ_Theta;
					lBinEjectorThetaLvl	= m_lStandby_Bin_EJ_Theta;
				}
				else
				{
					lEjectorThetaLvl	= m_lUpLevel_EJ_Theta;
					lBinEjectorThetaLvl	= m_lUpLevel_Bin_EJ_Theta;
				}

				szMsg.Format("BH - EJ T up final to %d", m_lUpLevel_EJ_Theta);
				DisplaySequence(szMsg);
				TakeTime(Z7);	//EJT3		//andrewng //2020-0507
				EjT_MoveTo(lEjectorThetaLvl, SFM_NOWAIT);
				s_dbStartEjTMove = GetTime();
				
				szMsg.Format("BH - BinEJ T up final to %d", m_lUpLevel_Bin_EJ_Theta);
				DisplaySequence(szMsg);
				TakeTime(T3);	//BJT3		//andrewng //2020-0507
				BinEjT_MoveTo(lBinEjectorThetaLvl, SFM_NOWAIT);
				s_dbStartBinEjTMove = GetTime();
					
				m_bComplete_Ej		= FALSE;
				m_bComplete_BinEj	= FALSE;

				OpUpdateMS100EjectorCounter(2);	
				
				// check machine cover state
				if (IsCoverOpen() == TRUE)
				{
					SetStatusMessage("Machine Cover Open");
					SetErrorMessage("Machine Cover Open");
					OpSetMS100AlarmCode(2);			//CoverOpen
				}
				
				//20171025 Leo Protection of hit hand
				CheckCoverOpenInAuto("During Bonding Cycle");

				if (!OpCheckValidAccessMode(TRUE))			//SanAn
				{
					SetErrorMessage("OpCheckValidAccessMode fails");	
					m_bDBHStop = TRUE;
				}

				if (!OpCheckStartLotCount())
				{
					//SetStatusMessage("OpCheckStartLotCount fails");
					SetErrorMessage("OpCheckStartLotCount fails");		
					m_bDBHStop = TRUE;
				}

				if (IsMS60() == TRUE)		//Check BH Fan sensor
				{
					if (m_lCheckFanCount >= 50)	// 0.06x50 = 3sec
					{
						if (OpCheckMS60BHFanOK() != TRUE)	
						{
							OpSetMS100AlarmCode(8);	
						}
						m_lCheckFanCount = 0;
					}
					else
					{
						m_lCheckFanCount++;
					}
				}

				if (m_bPickDownFirst)	//	pick side down firstly
				{
					TakeTime(CJ);		// Take Time

					BOOL bIsColletJam = FALSE;
					BOOL bReachedColletJamCount = FALSE;

					BOOL bPickDie = TRUE;
					LONG eAction = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"];
					if ((WAF_CDieSelectionAlgorithm::WAF_EDieAction)eAction != WAF_CDieSelectionAlgorithm::PICK)
					{
						bPickDie = FALSE;
					}
					LONG lNextPickRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
					LONG lNextPickCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
					lLastPickRow	= m_ulAtPickDieRow;
					lLastPickCol	= m_ulAtPickDieCol;

					if (OpCheckAlarmCodeOKToPick())
					{
						s_bDownToPick = TRUE;
						m_dBHZPickStartTime = GetTime();	//shiraishi03

						if (!bPickDie)
						{
							s_bDownToPick = FALSE;	//Do not move ejector up if eAction != WAF_CDieSelectionAlgorithm::PICK
						}
					}

					s_dbStartZ2PK = GetTime();	//	pick down first
				}

				m_qSubOperation = BOND_Z_WAIT_BT_READY_Q;	// -> normal flow
			}
			break;

		case BOND_Z_WAIT_BT_READY_Q:
			DisplaySequence("BH - BOND_Z_WAIT_BT_READY_Q");
			/*
			if (!WaitBTStartMoveWithoutDelay(0) && !m_bFirstCycle && m_bPickDownFirst)
			{
				//====================================================================================
				//  Wait for BT table move event
				//====================================================================================
				if  (WaitBTStartMove(200))
				{
					TakeTime(RD);
					DisplaySequence("BH - SetBTStartMove2 ==> FALSE");
					SetBTStartMove(FALSE);
					m_qSubOperation = PICK_Z2_DOWN_Q;	// -> normal flow
				}
				break;
				//====================================================================================
			}*/
			m_qSubOperation = PICK_Z2_DOWN_Q;	// -> normal flow
			break;

			 //go to PICK_Z2_DOWN_Q
		case PICK_Z2_DOWN_Q:
			DisplaySequence("BH - PICK_Z2_DOWN_Q");

			//Check CP2 completion finish
			if (s_bDownToPick && !m_bStop && !m_bIsWaferEnded)	
			{
				if (!WaitCompDone())
				{
					break;
				}
				SetCompDone(FALSE);
			}

			TakeTime(MD);
			s_dBondZDownStart = GetTime();
			m_qSubOperation = EJ_UP_Q;	
			break;

		case EJ_UP_Q:
			DisplaySequence("BH - EJ_UP_Q");
			if (AllowMove() == TRUE)
			{
				bWaferEnd = IsWaferEnded();
				BOOL bOnlyRegionEnd	= IsOnlyRegionEnd(); // xuzhijin_region_end
				//OpSyncEjectorTableXY();

				lEjectorLvl			= m_lEjectLevel_Ej;
				//andrewng //2020-0608
				if ( !m_bEnableSyncMotionViaEjCmd && 
					 (m_lContactLevel_Ej > m_lStandbyLevel_Ej) && (m_lContactLevel_Ej <= m_lEjectLevel_Ej) )
				{
					lEjectorLvl			= m_lContactLevel_Ej;
					bEjUseContactLvl	= TRUE;
				}

				lBinEjectorLvl		= m_lUpLevel_Bin_EJ_Pin;
				//lBinEjectorLvl	= m_lUpLevel_Bin_EJ_Cap;		//andrewng //2020-0601
				
				LONG lCompen_Ej = OpCalCompen_Ej();	// Calculate the compensation for wear out of ejector pin

				//Because BHZ not yet motion completed, so need to wait for delay to move the ejector up
				//WaitForDelayToEjectorUpMove(lTime_Ej, lEjectorUpDelay, s_dbStartZ2PK);

				BOOL bEjtToUp = FALSE;
				//Fixed EJ Up delay bug when Sync-Pick fcn is used
				if (s_bDownToPick && !m_bDisableBH)		
				{
					bEjtToUp = TRUE;
				}
				s_dBHEJSyncTime = 0;
						
				s_dbStartZ2EjUp = GetTime();
				s_dTotalTime_Ej = lTime_Ej;

				if (bEjtToUp)		
				{
					szMsg.Format("BH - EJ Cap up final to %d", m_lUpLevel_EJ_Cap);
					//DisplaySequence(szMsg);
					EjCap_MoveTo(m_lUpLevel_EJ_Cap, SFM_NOWAIT);

					szMsg.Format("BH - BinEJ Cap up final to %d", m_lUpLevel_Bin_EJ_Cap);
					//DisplaySequence(szMsg);
					BinEjCap_MoveTo(m_lUpLevel_Bin_EJ_Cap, SFM_NOWAIT);

					/*
					if (m_lHeadPickDelay > m_lTime_Ej)			//andrewng //2020-0608
					{
						//Let Theta move finish then move pin
						//WaitForDelay(m_lUpTime_Ej_Theta, s_dbStartEjTMove);
						Ej_Sync();
						m_bComplete_Ej = TRUE;
						TakeTime(Z8);	//EJT4					//andrewng //2020-0507
					}
					if (m_lHeadBondDelay > m_lTime_BinEj)		//andrewng //2020-0608
					{
						//Let Theta move finish then move pin
						//WaitForDelay(m_lUpTime_BinEj_Theta, s_dbStartBinEjTMove);
						BinEj_Sync();
						m_bComplete_BinEj = TRUE;
						TakeTime(T4);	//BJT4					//andrewng //2020-0507
					}
					*/

					LONG lWTIndexTime = (*m_psmfSRam)["WaferTable"]["IndexTime"];
					LONG lWTIndexStartTime = (*m_psmfSRam)["WaferTable"]["IndexStart"];

					if (m_lHeadBondDelay > m_lHeadPickDelay)
					{
/*
						LONG lWaitTime = 0;
						LONG lEjTDuration = GetTime() - s_dbStartEjTMove;

						lWaitTime = m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
						WaitForDelay(lWaitTime, s_dbStartEjTMove);

						TakeTime(EJ3);
						Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
						m_dEjDnStartTime = GetTime();

						szMsg.Format("BH - EJ up final to %ld, WAIT Time = %ld ms", lEjectorLvl + lCompen_Ej, lWaitTime);
						DisplaySequence(szMsg);

						/*if (lWTIndexTime > m_lUpTime_Ej_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadPickDelay - m_lTime_Ej;	//andrewng
							szMsg.Format("BH - lWaitTime = %d, lWTIndexTime = %d, m_lHeadPickDelay = %d, m_lTime_Ej = %d", 
								lWaitTime, lWTIndexTime, m_lHeadPickDelay, m_lTime_Ej);
						}
						else
						{
							lWaitTime =	m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
							szMsg.Format("BH - lWaitTime = %d, m_lUpTime_Ej_Theta = %d, m_lHeadPickDelay = %d, m_lTime_Ej = %d", 
								lWaitTime, m_lUpTime_Ej_Theta, m_lHeadPickDelay, m_lTime_Ej);
						}
						DisplaySequence(szMsg);

						if (m_lEjectLevel_Ej != m_lStandbyLevel_Ej)
						{	
							WaitForDelay(lWaitTime, s_dbStartEjTMove);
						}
						else
						{
							DisplaySequence("BH - m_lEjectLevel_Ej == m_lStandbyLevel_Ej, No Wait Time");
						}

						szMsg.Format("BH - EJ up final to %d", lEjectorLvl + lCompen_Ej);
						DisplaySequence(szMsg);
						*/

						//lWaitTime = m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
						//WaitForDelay(lWaitTime, s_dbStartBinEjTMove);
						/*
						if (lWTIndexTime > m_lUpTime_BinEj_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadBondDelay - m_lTime_BinEj;	//andrewng
							szMsg.Format("BH - lWaitTime = %d, lWTIndexTime = %d, m_lHeadBondDelay = %d, m_lTime_BinEj = %d", 
								lWaitTime, lWTIndexTime, m_lHeadBondDelay, m_lTime_BinEj);
						}
						else
						{
							lWaitTime =	m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
							szMsg.Format("BH - lWaitTime = %d, m_lUpTime_BinEj_Theta = %d, m_lHeadBondDelay = %d, m_lTime_BinEj = %d", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lHeadBondDelay, m_lTime_BinEj);
						}
						DisplaySequence(szMsg);

						//Need to keep to make sure WT and BHT arrive, even m_lEjectLevel_Ej == m_lStandbyLevel_Ej, No Wait Time
						WaitForDelay(lWaitTime, s_dbStartBinEjTMove);
						
						szMsg.Format("BH - Bin EJ up final to %d", m_lUpLevel_Bin_EJ_Pin);
						DisplaySequence(szMsg);
						*/

						LONG lWaitTime = 0;
						if (lWTIndexTime > m_lUpTime_Ej_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadPickDelay - m_lTime_Ej;	//andrewng
							szMsg.Format("BH - lWaitTime = %d, lWTIndexTime = %d, m_lHeadPickDelay = %d, m_lTime_Ej = %d", 
								lWaitTime, lWTIndexTime, m_lHeadPickDelay, m_lTime_Ej);
						}
						else
						{
							lWaitTime =	m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
							szMsg.Format("BH - lWaitTime = %d, m_lUpTime_Ej_Theta = %d, m_lHeadPickDelay = %d, m_lTime_Ej = %d", 
								lWaitTime, m_lUpTime_Ej_Theta, m_lHeadPickDelay, m_lTime_Ej);
						}
						DisplaySequence(szMsg);

						if (m_lEjectLevel_Ej != m_lStandbyLevel_Ej)
						{	
							WaitForDelay(lWaitTime, s_dbStartEjTMove);
						}
						else
						{
							DisplaySequence("BH - m_lEjectLevel_Ej == m_lStandbyLevel_Ej, No Wait Time");
						}

						szMsg.Format("BH - EJ up final to %d", lEjectorLvl + lCompen_Ej);
						DisplaySequence(szMsg);
						TakeTime(EJ3);
						Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
						m_dEjDnStartTime = GetTime();

						lWaitTime = 0;
						if (lWTIndexTime > m_lUpTime_BinEj_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadBondDelay - m_lTime_BinEj;	//andrewng
							szMsg.Format("BH - lWaitTime = %d, lWTIndexTime = %d, m_lHeadBondDelay = %d, m_lTime_BinEj = %d", 
								lWaitTime, lWTIndexTime, m_lHeadBondDelay, m_lTime_BinEj);
						}
						else
						{
							lWaitTime =	m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
							szMsg.Format("BH - lWaitTime = %d, m_lUpTime_BinEj_Theta = %d, m_lHeadBondDelay = %d, m_lTime_BinEj = %d", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lHeadBondDelay, m_lTime_BinEj);
						}
						DisplaySequence(szMsg);

						//Need to keep to make sure WT and BHT arrive, even m_lEjectLevel_Ej == m_lStandbyLevel_Ej, No Wait Time
						WaitForDelay(lWaitTime, s_dbStartBinEjTMove);
						
						szMsg.Format("BH - Bin EJ up final to %d", m_lUpLevel_Bin_EJ_Pin);
						DisplaySequence(szMsg);
						TakeTime(Z3);
						BinEj_MoveTo(lBinEjectorLvl, SFM_NOWAIT);
						//BinEjCap_MoveTo(lBinEjectorLvl, SFM_NOWAIT);		//andrewng //2020-0601
						m_dBinEjDnStartTime = GetTime();

						/*
						szMsg.Format("BH - BIN EJ up final to %ld, WAIT Time = %ld ms", lBinEjectorLvl, lWaitTime);
						DisplaySequence(szMsg);

						m_dBinEjDnStartTime = GetTime();
						m_bComplete_Ej		= FALSE;
						m_bComplete_BinEj	= FALSE;
						*/

					}
					else
					{
						LONG lWaitTime = 0;
						
						//** First, calculate and wait for HEAD-BOND delay 

						if (lWTIndexTime > m_lUpTime_BinEj_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadBondDelay - m_lTime_BinEj;	//andrewng
							szMsg.Format("BH - EJ_UP_Q HeadBOND: lWaitTime = %ld, lWTIndexTime = %ld (%ld), m_lHeadBondDelay = %ld, m_lTime_BinEj = %ld", 
								lWaitTime, lWTIndexTime, m_lWTSettlingDelay, m_lHeadBondDelay, m_lTime_BinEj);
						}
						else
						{
							lWaitTime =	m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
							szMsg.Format("BH - EJ_UP_Q HeadBOND: lWaitTime = %d, m_lUpTime_BinEj_Theta = %d, m_lHeadBondDelay = %d, m_lTime_BinEj = %d", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lHeadBondDelay, m_lTime_BinEj);
						}
						DisplaySequence(szMsg);

						if (m_lUpLevel_Bin_EJ_Pin != m_lStandby_Bin_EJ_Pin)
						{
							WaitForDelay(lWaitTime, s_dbStartBinEjTMove);
						}

						szMsg.Format("BH - Bin EJ up final to %ld (%ld)", lBinEjectorLvl, m_lUpLevel_Bin_EJ_Pin);
						DisplaySequence(szMsg);

						TakeTime(Z3);
						BinEj_MoveTo(lBinEjectorLvl, SFM_NOWAIT);
						m_dBinEjDnStartTime = GetTime();
	
						//** Second, calculate and wait for HEAD-PICK delay 

						lWaitTime = 0;
						if (lWTIndexTime > m_lUpTime_Ej_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadPickDelay - m_lTime_Ej;	//andrewng
							szMsg.Format("BH - EJ_UP_Q HeadPICK: lWaitTime = %ld, lWTIndexTime = %ld (%ld), m_lHeadPickDelay = %ld, m_lTime_Ej = %ld", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lWTSettlingDelay, m_lHeadPickDelay, m_lTime_Ej);
						}
						else
						{
							lWaitTime =	m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
							szMsg.Format("BH - EJ_UP_Q HeadPICK: lWaitTime = %d, m_lUpTime_BinEj_Theta = %d, m_lHeadPickDelay = %d, m_lTime_Ej = %d", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lHeadPickDelay, m_lTime_Ej);
						}
						DisplaySequence(szMsg);

						//Need to keep to make sure WT and BHT arrive, even m_lUpLevel_Bin_EJ_Pin == m_lStandby_Bin_EJ_Pin, No Wait Time
						WaitForDelay(lWaitTime, s_dbStartEjTMove);
						
						//szMsg.Format("BH - EJ up final to %d", lEjectorLvl + lCompen_Ej);
						//DisplaySequence(szMsg);

						/*
						LONG lWaitTime = 0;

						lWaitTime = m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
						WaitForDelay(lWaitTime, s_dbStartBinEjTMove);

						TakeTime(Z3);
						BinEj_MoveTo(lBinEjectorLvl, SFM_NOWAIT);
						//BinEjCap_MoveTo(lBinEjectorLvl, SFM_NOWAIT);	//andrewng //2020-0601
						m_dBinEjDnStartTime = GetTime();

						szMsg.Format("BH - Bin EJ up final to %ld, WAIT Time = %ld", lBinEjectorLvl, lWaitTime);
						DisplaySequence(szMsg);

						/*
						if (lWTIndexTime > m_lUpTime_BinEj_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadBondDelay - m_lTime_BinEj;	//andrewng
							szMsg.Format("BH - lWaitTime = %d, lWTIndexTime = %d, m_lHeadBondDelay = %d, m_lTime_BinEj = %d", 
								lWaitTime, lWTIndexTime, m_lHeadBondDelay, m_lTime_BinEj);
						}
						else
						{
							lWaitTime =	m_lUpTime_BinEj_Theta + m_lHeadBondDelay - m_lTime_BinEj;
							szMsg.Format("BH - lWaitTime = %d, m_lUpTime_BinEj_Theta = %d, m_lHeadBondDelay = %d, m_lTime_BinEj = %d", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lHeadBondDelay, m_lTime_BinEj);
						}
						DisplaySequence(szMsg);

						if (m_lUpLevel_Bin_EJ_Pin != m_lStandby_Bin_EJ_Pin)
						{
							WaitForDelay(lWaitTime, s_dbStartBinEjTMove);
						}
						else
						{
							DisplaySequence("BH - m_lUpLevel_Bin_EJ_Pin == m_lStandby_Bin_EJ_Pin, No Wait Time");
						}
						*/

						//lWaitTime = m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
						//WaitForDelay(lWaitTime, s_dbStartEjTMove);

						/*
						lWaitTime = 0;
						if (lWTIndexTime > m_lUpTime_Ej_Theta)
						{
							lWaitTime =	lWTIndexTime + m_lHeadPickDelay - m_lTime_Ej;	//andrewng
							szMsg.Format("BH - lWaitTime = %d, lWTIndexTime = %d, m_lHeadPickDelay = %d, m_lTime_Ej = %d", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lHeadPickDelay, m_lTime_Ej);
						}
						else
						{
							lWaitTime =	m_lUpTime_Ej_Theta + m_lHeadPickDelay - m_lTime_Ej;
							szMsg.Format("BH - lWaitTime = %d, m_lUpTime_BinEj_Theta = %d, m_lHeadPickDelay = %d, m_lTime_Ej = %d", 
								lWaitTime, m_lUpTime_BinEj_Theta, m_lHeadPickDelay, m_lTime_Ej);
						}
						DisplaySequence(szMsg);

						//Need to keep to make sure WT and BHT arrive, even m_lUpLevel_Bin_EJ_Pin == m_lStandby_Bin_EJ_Pin, No Wait Time
						WaitForDelay(lWaitTime, s_dbStartEjTMove);
						
						szMsg.Format("BH - EJ up final to %d", lEjectorLvl + lCompen_Ej);
						DisplaySequence(szMsg);
						TakeTime(EJ3);
						*/

						//andrewng //2020-0522
						if (m_bEnableSyncMotionViaEjCmd)
						{
							//andrewng //2020-0526
							BinEj_Sync();
							Ej_Sync();
							m_bComplete_Ej		= TRUE;
							m_bComplete_BinEj	= TRUE;

							//Ej_SelectObwProfile((LONG)(lEjectorLvl - m_lCurLevel_Ej));
							//Ej_SelectControl((LONG)(lEjectorLvl - m_lCurLevel_Ej));

							TakeTime(LC1);
							MotionSyncViaEjCmdDistance(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);		//andrewng //2020-0526
							//MotionSyncViaEjCmdDistance(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);		//andrewng //2020-0526
							
							if (m_lSyncTriggerValue > 0)
							{
								s_dBHEJSyncTime = CalculateEjQueryTime((lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej), m_lSyncTriggerValue);
							}
							else
							{
								s_dBHEJSyncTime = 0;
							}

							s_dBHEJSyncTime += lTime_Ej;
						}

						TakeTime(EJ3);
						Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
						m_dEjDnStartTime = GetTime();

						szMsg.Format("BH - EJ up final to %ld (SYNC=%d), WAIT Time = %ld", 
								lEjectorLvl + lCompen_Ej, m_bEnableSyncMotionViaEjCmd, lWaitTime);
						DisplaySequence(szMsg);

						if (m_bEnableSyncMotionViaEjCmd)
						{
							//Wait a delay for sysnc with both BinEj and Ej
							//WaitForDelay(lTime_Ej, s_dbStartZ2EjUp);
							WaitForDelay(s_dBHEJSyncTime, s_dbStartZ2EjUp);
							m_bComplete_Ej = FALSE;
						}
						//New NVC Z-SyncPick fcn for Ej and BinEj
						else if ((m_lSyncZOffset > 0 || bEjUseContactLvl) && (m_lSyncZOffset <= 1000))
						{
							//andrewng //2020-0602
							BinEj_Sync();
							Ej_Sync();

							if ((lEjectorUpDelay > 0) && (lEjectorUpDelay <= 1000))
							{
								if (lEjectorUpDelay - 1 > 0)	//andrewng //2020-1208
								{
									Sleep(lEjectorUpDelay - 1);
								}
							}

							LONG lEjSyncOffsetZ		= m_lSyncZOffset;
							LONG lBinEjSyncZOffsetZ = m_lSyncZOffset;
							if (bEjUseContactLvl)
							{
								lEjSyncOffsetZ		= m_lEjectLevel_Ej - m_lContactLevel_Ej;
								if ( m_lSyncZOffset < (m_lEjectLevel_Ej - m_lContactLevel_Ej) )
								{
									lBinEjSyncZOffsetZ	= m_lEjectLevel_Ej - m_lContactLevel_Ej - m_lSyncZOffset;
								}
								else
								{
									lBinEjSyncZOffsetZ	= m_lEjectLevel_Ej - m_lContactLevel_Ej;
								}
							}


							//Ej_SelectObwProfile(m_lSyncZOffset);
							//BinEj_SelectObwProfile(m_lSyncZOffset);
							Ej_SelectControl(lEjSyncOffsetZ,		TRUE);
							BinEj_SelectControl(lBinEjSyncZOffsetZ, TRUE);

							TakeTime(LC1);
							s_dbStartEjUp = GetTime();		//andrewng //2020-1208

							Ej_Move(lEjSyncOffsetZ,			SFM_NOWAIT);
							BinEj_Move(lBinEjSyncZOffsetZ,	SFM_NOWAIT);
							
							lSynUpTime_Ej = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ, BH_MP_E_OBW_DEFAULT, m_lMinEjectOffset, lEjSyncOffsetZ, ULTRA_BH_SAMPLE_RATE, &m_stBHAxis_Ej);

							//andrewng //2020-1208
							if ( (lSynUpTime_Ej > 0) && (lSynUpTime_Ej <= 10) )
							{
								WaitForDelay(lSynUpTime_Ej, s_dbStartEjUp);
							}
							else
							{
								Ej_Sync();
								//BinEj_Sync();
							}

							s_dbStartEjUp = GetTime();		//andrewng //2020-1208

							TakeTime(EJ4);
							TakeTime(Z4);	//BJ4

							DisplaySequence("Sync Pick EJ and BinEj motion DONE at PICK level");

							Ej_SelectControl(lEjSyncOffsetZ,		FALSE);
							BinEj_SelectControl(lBinEjSyncZOffsetZ, FALSE);

							m_bComplete_Ej		= TRUE;
							m_bComplete_BinEj	= TRUE;
						}
						else
						{
							m_bComplete_Ej		= FALSE;
							m_bComplete_BinEj	= FALSE;
						}
					}

					//CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog("BH Ej Move To UP Level");
					//CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog("");
					//CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog("");

					OpUpdateMS100EjectorCounter(2);	
					//OpUpdateMS100ColletCounter();
				}

				if (s_dTotalTime_Ej < s_dBHEJSyncTime)
				{
					s_dTotalTime_Ej = s_dBHEJSyncTime;
				}

				m_qSubOperation = WAIT_PICK_DELAY_Q;	// -> normal flow	
			}
			break;

		case WAIT_PICK_DELAY_Q:
			//DisplaySequence("BH - Wait Pick Delay");
			DisplaySequence("BH - WAIT_PICK_DELAY_Q");
			
			////Chck CP2 completion before Bh to continue next cycle
			//if (s_bDownToPick && !m_bStop && !m_bIsWaferEnded)	
			//{
			//	if (!WaitCompDone())
			//	{
			//		break;
			//	}
			//	SetCompDone(FALSE);
			//}

			if (/*!m_bFirstCycle && */!m_bChangeGrade)
			{
				//DisplaySequence("BH - SetBTStartMoveWithoutDelay3==>FALSE");
				SetWTStartMove(FALSE);
				SetBTStartMoveWithoutDelay(FALSE);
				SetBTStartMove(FALSE);

				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;

				if (m_nBHAlarmCode != -1 && m_lBHStateCode != -1)	//If not MD-RETRY, index BT by 1	
				{
					//	V450X16	at bond, update bond grade and check BT block
					m_lAtBondDieGrade	= m_ucAtPickDieGrade;
					m_lAtBondDieRow		= m_ulAtPickDieRow;
					m_lAtBondDieCol		= m_ulAtPickDieCol;
					LONG lBTBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];	//	["BinTable"]["LastBlkInUse"]
					LONG lBTGrade		= (*m_psmfSRam)["BinTable"]["BondingGrade"];	//	wrong sort bug fix
					szMsg.Format("BH - map (%ld,%ld)%ld BHZ%d At BOND, Bin grade %ld, Blk %ld", m_lAtBondDieRow, m_lAtBondDieCol, m_lAtBondDieGrade,
							IsBHZ1ToPick()+1, lBTGrade, lBTBlkInUse);
					if (IsAutoSampleSort())
					{
						GetEncoderValue();
						szTemp.Format(", T %ld(%ld), Z1 %d, Z2 %d, EJ %ld",
								m_lEnc_T, m_lCurPos_T, m_lEnc_Z, m_lEnc_Z2, m_lEnc_Ej);
						szMsg = szMsg + szTemp;
					}
					DisplaySequence(szMsg);
					if (pAppMod->GetCustomerName() != "Primaopto")
					{
						if (IsBurnIn() == FALSE && SPECIAL_DEBUG_LOG_WS && m_lAtBondDieGrade > 0 && lBTGrade > 0 && m_lAtBondDieGrade != lBTGrade)
						{
							CString szErr;
							szErr.Format("ERROR: wrong die is bonded!  Current Bin = #%ld but die-grade = #%ld", 
							lBTGrade, m_lAtBondDieGrade);
							SetErrorMessage(szErr);
							HmiMessage_Red_Yellow(szErr);
							m_qSubOperation = HOUSE_KEEPING_Q;
							break;
						}
					}

					//	V450X16	at bond, update bond grade and check BT block
					SetBTReady(FALSE); //2018.3.28 
					m_lBondDieCount++;
					SetDieBonded(TRUE);    //Let BT task to NEXT DIE_Q
				}

				SetDieBondedForWT(TRUE);		//Let WT task to continue
			}
			else
			{
				//FOr Yealy MS100Plus Single Loader UNLOAD buffer sequence only
				if (!CMS896AStn::m_bWLReadyToUnloadBufferFrame)
				{
					CMS896AStn::m_bWLReadyToUnloadBufferFrame = TRUE; 
				}	
			}

			if (m_bComplete_Ej == FALSE || m_bComplete_BinEj == FALSE)
			{
				if (m_lHeadPickDelay <= m_lUpTime_Ej_Theta)
				{
					WaitForDelay(m_lUpTime_Ej_Theta, s_dbStartEjTMove);
					TakeTime(Z8);		//andrewng //2020-0507
				}
				if (m_lHeadBondDelay <= m_lUpTime_BinEj_Theta)
				{
					WaitForDelay(m_lUpTime_BinEj_Theta, s_dbStartBinEjTMove);
					TakeTime(T4);		//andrewng //2020-0507
				}

				//Ejector Pin Arrive
				WaitForDelay(m_lTime_Ej, m_dEjDnStartTime);
				TakeTime(EJ4);				//andrewng //2020-0507

				WaitForDelay(m_lTime_BinEj, m_dBinEjDnStartTime);
				TakeTime(Z4);	//BJ4		//andrewng //2020-0507

				s_dbStartEjUp = GetTime();		//andrewng //2020-1208

				EjCap_Sync();
				BinEjCap_Sync();

				m_bComplete_Ej = TRUE;
				m_bComplete_BinEj = TRUE;
			}

			//Set Hw Trigger Grab Share Image
			SetBhReadyForWPRHwTrigger(TRUE);	
			SetBhReadyForBPRHwTrigger(TRUE);		//andrewng //2020-0618

			//Wait for PICK/BOND delay
			//s_dbStartEjUp = GetTime();
			if (m_lPickDelay < m_lBondDelay)
			{
				szMsg.Format("BH - m_lPickDelay = %d", m_lPickDelay);
				DisplaySequence(szMsg);
				WaitForDelay(m_lPickDelay, s_dbStartEjUp);
			}
			else
			{
				szMsg.Format("BH - m_lBondDelay = %d", m_lBondDelay);
				DisplaySequence(szMsg);
				WaitForDelay(m_lBondDelay, s_dbStartEjUp);
			}
			TakeTime(LC8);

			if (bEnableWTIndexTest)
			{
				CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
				if (!pWaferPr->WPRIndexTest())
				{
					HmiMessage("WPR Index Test Fail");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			m_bFirstCycle		= FALSE;
			m_bBhInBondState	= FALSE;	
			m_bDetectDAirFlowAtPrePick = FALSE;
			m_bMoveFromPrePick	= FALSE;

			//if (IsBurnIn())
			//{
				//andrewng //2020-0617		
				//Reset once more for next cycle due to SetWTReady(TRUE) twice in WT::OpMoveTable() for BURNIN;
			//	SetWTReady(FALSE, "WAIT_PICK_DELAY_Q");
			//}

			//Collet-Hole shift test for dual arm
			//Update Ejector & Collet counter
			bWaferEnd	= IsWaferEnded();
			if (s_bDownToPick)	
			{
				OpUpdateMachineCounters();
			}
			
			if (!m_bFirstCycle && !m_bChangeGrade)
			{
				// Find the cycle time
				dCurrentTime = GetTime();
				if (m_dLastTime > 0.0)
				{
					m_dCycleTime = fabs(dCurrentTime - m_dLastTime);

					if (m_dCycleTime > 99999)
					{
						m_dCycleTime = 10000;
					}

					// Calculate the average cycle time
					m_dAvgCycleTime = (m_dAvgCycleTime * m_ulCycleCount + m_dCycleTime) / (m_ulCycleCount + 1);
					m_ulCycleCount++;

					CMSLogFileUtility::Instance()->WT_GetCTLog_CT_NoUpdate(m_dCycleTime);	//MS100+ CT study
				
					// 3501
					SetGemValue("AB_CurrentCycleTime", m_dCycleTime);
					SetGemValue("AB_AverageCycleTime", m_dAvgCycleTime);
					SetGemValue("AB_CycleCounter", m_ulCycleCount);
					// 7704
					// SendEvent(SG_CEID_AB_RUNTIME, FALSE);
				}
				m_dLastTime = dCurrentTime;

				if (m_nBHAlarmCode != -1 && m_lBHStateCode != -1)		//If not MD-RETRY
				{
					OpUpdateBondedUnit(m_dCycleTime);		//Update bonded unit
				}
				OpUpdateDieIndex();				// Update the die index
			}

			//store pick grade
			m_bSetEjectReadyDone = FALSE;
			if (s_bDownToPick)
			{
				//Record wafer map coordinate&Bin on bondhead
				m_ulAtPickDieRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
				m_ulAtPickDieCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
				m_ucAtPickDieGrade	= (UCHAR)(LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"] - m_WaferMapWrapper.GetGradeOffset();
				szMsg.Format("BH - map (%ld,%ld)%ld BHZ%d At PICK",
					m_ulAtPickDieRow, m_ulAtPickDieCol, m_ucAtPickDieGrade, IsBHZ2ToPick()+1);

				if (IsAutoSampleSort())
				{
					GetEncoderValue();
					szTemp.Format(", T %ld(%ld), Z1 %d, Z2 %d, EJ %ld",
							m_lEnc_T, m_lCurPos_T, m_lEnc_Z, m_lEnc_Z2, m_lEnc_Ej);
					szMsg = szMsg + szTemp;
				}

				DisplaySequence(szMsg);
			}

			//OpCheckPostBondEmptyCountToIncBHZ();
			(*m_psmfSRam)["BondHead"]["First Pick Die"] = TRUE;
			m_qSubOperation = EJ_DOWN_Q;
			break;

		case EJ_DOWN_Q: //MOVE_EJ_THEN_T_Q:
			DisplaySequence("BH - EJ_DOWN_Q");
			lArmPicBondDelay = 0;
			if (AllowMove() == TRUE)
			{
				TakeTime(CycleTime);
				NextBondRecord();
				CString szTemp;
				szTemp.Format("BH - MegaDa Cycle #%d", GetRecordNumber());
				DisplaySequence(szTemp);
				StartTime(GetRecordNumber());	// Restart the time	

				// Move Ej
				TakeTime(EJ1);		//andrewng //2020-0507
				Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
				m_dEjDnStartTime = GetTime();
				szMsg.Format("BH - in EJ THEN T, EJ down to standby %d", m_lStandbyLevel_Ej);
				DisplaySequence(szMsg);

				//andrewng //2020-0505
				if ((lEjectorDownDelay > 0) && (lEjectorDownDelay <= 100))
				{
					WaitForDelay(lEjectorDownDelay, m_dEjDnStartTime);
				}

				TakeTime(Z1);
						
				BinEj_MoveTo(m_lStandby_Bin_EJ_Pin, SFM_NOWAIT);
				//BinEjCap_MoveTo(m_lStandby_Bin_EJ_Cap, SFM_NOWAIT);		//andrewng //2020-0601

				m_dBinEjDnStartTime = GetTime();
				szMsg.Format("BH - in EJ THEN T, BIN EJ down to standby %d", m_lStandby_Bin_EJ_Pin);
				DisplaySequence(szMsg);

				m_bComplete_Ej		= FALSE;
				m_bComplete_BinEj	= FALSE;

				//** Special Speed-up mode for MS100 100ms testing **//
				bLFDie	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];
				bSwitchPR = m_bPrAoiSwitchCamera;

				m_qSubOperation = EJ_THETA_DOWN_Q;//MOVE_T_Q;
			}
			break;

		case EJ_THETA_DOWN_Q: //MOVE_EJ_THEN_T_Q:
			DisplaySequence("BH - EJ_THETA_DOWN_Q");
			lArmPicBondDelay = 0;
			if (AllowMove() == TRUE)
			{	
				if (lArmPickDelay > lArmBondDelay)
				{
					lArmPicBondDelay = lArmPickDelay; // - lEjectorDownDelay;
				}
				else
				{
					lArmPicBondDelay = lArmBondDelay; // - lEjectorDownDelay;
				}

				if (lArmPickDelay > m_lDnTime_Ej)
				{
					WaitForDelay(m_lDnTime_Ej, m_dEjDnStartTime);
					TakeTime(EJ2);
				}
				if (lArmBondDelay > m_lDnTime_BinEj)
				{
					WaitForDelay(m_lDnTime_BinEj, m_dBinEjDnStartTime);
					TakeTime(Z2);	//BJ2
				}

				EjCap_MoveTo(m_lStandby_EJ_Cap, SFM_NOWAIT);
				//szMsg.Format("BH - in EJ THEN T, EJ CAP down to standby %d", m_lStandby_EJ_Cap);
				//DisplaySequence(szMsg);

				BinEjCap_MoveTo(m_lStandby_Bin_EJ_Cap, SFM_NOWAIT);
				//szMsg.Format("BH - in EJ THEN T, BIN EJ CAP down to standby %d", m_lStandby_Bin_EJ_Cap);
				//DisplaySequence(szMsg);

				if (lArmBondDelay > lArmPickDelay)
				{
					if (lArmPickDelay > 0)
					{
						WaitForDelay(lArmPickDelay, m_dEjDnStartTime);
					}

					TakeTime(Z5);
					EjT_MoveTo(m_lStandby_EJ_Theta, SFM_NOWAIT);
					s_dbStartEjTMove = GetTime();
					szMsg.Format("BH - in EJ THEN T, EJT down to standby %d", m_lStandby_EJ_Theta);
					DisplaySequence(szMsg);

					if (lArmBondDelay > 0)
					{
						WaitForDelay(lArmBondDelay, m_dBinEjDnStartTime);
					}
					TakeTime(T1);
					BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta, SFM_NOWAIT);
					s_dbStartBinEjTMove = GetTime();
					szMsg.Format("BH - in EJ THEN T, BIN EJT down to standby %d", m_lStandby_EJ_Theta);
					DisplaySequence(szMsg);
				}
				else
				{
					if (lArmBondDelay > 0)
					{
						WaitForDelay(lArmBondDelay, m_dBinEjDnStartTime);
					}
					TakeTime(T1);		//BJT1
					BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta, SFM_NOWAIT);
					s_dbStartBinEjTMove = GetTime();
					szMsg.Format("BH - in EJ THEN T, BIN EJT down to standby %d", m_lStandby_EJ_Theta);
					DisplaySequence(szMsg);

					if (lArmPickDelay > 0)
					{
						WaitForDelay(lArmPickDelay, m_dEjDnStartTime);
					}
					TakeTime(Z5);		//EJT1
					EjT_MoveTo(m_lStandby_EJ_Theta, SFM_NOWAIT);
					s_dbStartEjTMove = GetTime();
					szMsg.Format("BH - in EJ THEN T, EJT down to standby %d", m_lStandby_EJ_Theta);
					DisplaySequence(szMsg);
				}

				m_bComplete_Ej		= FALSE;
				m_bComplete_BinEj	= FALSE;

				//** Special Speed-up mode for MS100 100ms testing **//
				bLFDie	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];
				bSwitchPR = m_bPrAoiSwitchCamera;
				
				//if ((lArmPicBondDelay >= lDnTime_Ej) && !bSwitchPR/*m_bPrAoiSwitchCamera*/)		// || !bLFDie)
				//{
				//Ej_Sync();

				m_qSubOperation = MOVE_T_Q;
			}
			break;

		case MOVE_T_Q:
			DisplaySequence("BH - MOVE_T_Q");

			//for Armpick or Amrbond delay, it will trigger to move theta
			WaitForDelay(lArmPicBondDelay, s_dArmPickUpStart);
			//============================================================================

			lMotionTime_BT	= (*m_psmfSRam)["BinTable"]["MotionTime"];
			m_lSPCResult	= (*m_psmfSRam)["BondPr"]["PostBondResult"];
			bWaferEnd		= IsWaferEnded();
			bLFDie			= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];
			if (m_lSPCResult != 1)
			{
				if (!CheckBPRErrorNeedAlarm(1))
				{
					m_lSPCResult = 1;
					(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
				}
			}

			if (!m_bFirstCycle && ((m_nBHAlarmCode != -1) && m_lBHStateCode!=-1) && (m_lBondDieCount > 2))
			{
				if (WaitBPRPostBondDone(5000)) 
				{
//					SetBPRPostBondDone(FALSE);
				}
				else
				{
					CString szErr;
					szErr = _T("ERROR: Postbond Inspection Timeout");
					SetErrorMessage(szErr);
					HmiMessage_Red_Yellow(szErr);

					Sleep(100);	
					//T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			if (IsOnlyRegionEnd())
			{
				if (m_bComplete_Ej == FALSE || m_bComplete_BinEj == FALSE)
				{
					WaitForDelay(m_lDnTime_Ej, m_dEjDnStartTime);
					TakeTime(EJ2);
					WaitForDelay(m_lDnTime_BinEj, m_dBinEjDnStartTime);
					TakeTime(Z2);

					WaitForDelay(m_lDnTime_Ej_Theta, s_dbStartEjTMove);
					TakeTime(Z6);
					WaitForDelay(m_lDnTime_BinEj_Theta, s_dbStartBinEjTMove);
					TakeTime(T2);

					EjCap_Sync();
					BinEjCap_Sync();

					m_bComplete_Ej = TRUE;
					m_bComplete_BinEj = TRUE;
				}

				CheckCoverOpenInAuto("Alarm Code Hanler stop");
				//T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				break;
			}
		
			if (m_bDisableBT)
			{
				m_lSPCResult = 1;
			}

			//SanAn
			if (m_lSPCResult != 1)
			{
				OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "3");	//Sanan & Semitek
			}

			if ((m_lSPCResult != 1) && (m_nBHAlarmCode != -4))
			{
				CString szLog;
				szLog.Format("BH: bPostBondNextCycleStop SET - %ld, %d, %lu", 
					m_lSPCResult, m_bSPCAlarmTwice, m_ulCycleCount);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

				//Need to stop at "Next Cycle" instead because curr-die already picked at this moment!
				m_lPrevDieSPCResult = m_lSPCResult;
				bPostBondNextCycleStop = TRUE;
			}
			else
			{
				bPostBondNextCycleStop = FALSE;
			}

			if ((m_nBHAlarmCode == -4) ||	//postbond already triggered in "last" cycle	
				(m_nBHAlarmCode == 6))		//postbond triggerd before BH down to PICK		
			{
				//TO resolve postbond error hangup problem	//TW
				if (m_bComplete_Ej == FALSE)
				{
					Ej_Sync();
					TakeTime(EJ2);
					m_bComplete_Ej = TRUE;
				}

				Sleep(100);		//Extra delay to avoid BH lose-power problem due to ILC	
				//T_MoveTo(m_lPrePickPos_T, SFM_WAIT);

				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = POSTBOND_FAIL_Q;
				break;
			}
			else if (m_bDBHStop)
			{
				OpAlarmCodeHandler();
				DisplaySequence("BH - |DBH stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			else
			{
				if (m_bComplete_Ej == FALSE || m_bComplete_BinEj == FALSE)
				{
					if (lArmPickDelay <= m_lDnTime_Ej)
					{
						WaitForDelay(m_lDnTime_Ej, m_dEjDnStartTime);
						TakeTime(EJ2);
					}
					if (lArmBondDelay <= m_lDnTime_BinEj)
					{
						WaitForDelay(m_lDnTime_BinEj, m_dBinEjDnStartTime);
						TakeTime(Z2);
					}
					
					//andrewng //2020-0506
					//WaitForDelay(m_lUpTime_Ej_Theta, s_dbStartEjTMove);
					//TakeTime(Z6);
					//WaitForDelay(m_lUpTime_BinEj_Theta, s_dbStartBinEjTMove);
					//TakeTime(T2);

					EjCap_Sync();
					BinEjCap_Sync();

					m_bComplete_Ej = TRUE;
					m_bComplete_BinEj = TRUE;
				}
				
				if (m_bMS60EjElevator)	
				{
					EjElevator_Sync();	
					BinEjElevator_Sync();	
				}

				/*if ( (lDnTime_Ej > 0) && (lDnTime_Ej < 10))
				{
					Sleep(lDnTime_Ej);
				}
				else
				{
					Ej_Sync();
					if (m_bMS60EjElevator)	
					{
						EjElevator_Sync();	
					}
				}*/

				m_bSetEjectReadyDone = TRUE;
				SetEjectorReady(TRUE, "Normal Flow");			//Let WT to INDEX

				BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];	
				if (bMS60LFCycle)
				{
					(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;	
				}
				else
				{
					if (!m_bDBHStop && !bWaferEnd && !m_bStop && !WaitWTStartMove(0) && !IsReachACCMaxCount())
					{
						//MS100 BH-T speedup for smaller Arm-PICK delay, if current dei is LF and no-compensate needed
						//No need to wait for WT start-move event
						(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;	
					}
				}

				SetWTStartMove(FALSE);

				if (!OpIsBinTableLevelAtDnLevel())		//Semitek
				{
					SetMotionCE(TRUE, "BT level at UP detected in MOVE_T_Q");
					SetErrorMessage("BH: BT platform at UP level in MOVE_T_Q !!");
					SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (!OpAlarmCodeHandler())
				{
					//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();	//Must wait T complete for ILC
					Sleep(100);	

					CheckCoverOpenInAuto("Alarm Code Hanler stop");
					T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					DisplaySequence("BH - |Alarm code stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (bPostBondNextCycleStop)
				{
					OpSetMS100AlarmCode(-4);		//postbond NextCycle stop
				}

				if (IsReachACCMaxTimeOut())
				{
					if (m_ulCleanCount < m_ulMaxCleanCount)
					{
						m_ulCleanCount = m_ulMaxCleanCount;
					}
				}
				if (m_bBPRErrorToCleanCollet)
				{
					if (m_ulCleanCount < m_ulMaxCleanCount)
					{
						m_ulCleanCount = m_ulMaxCleanCount;
					}
				}

				if (m_nBHLastAlarmCode == -2 ||	//If curr-pick-die is ChangeGrade NoPickDie
					m_nBHLastAlarmCode == -3)	//BIN-FULL
				{
					//Extra delay to avoid BH lose-power problem due to ILC		
					//T_Sync();		//Must wait T complete for ILC

					//andrewng //2020-0506
					WaitForDelay(m_lUpTime_Ej_Theta, s_dbStartEjTMove);
					TakeTime(Z6);
					WaitForDelay(m_lUpTime_BinEj_Theta, s_dbStartBinEjTMove);
					TakeTime(T2);

					Sleep(10);	

					CString szLog;
					if (IsBHZ1ToPick())	//If BH! towards PICK
						szLog = "BH: ChangeGrade; last die bonded by BH2"; 
					else
						szLog = "BH: ChangeGrade; last die bonded by BH1"; 

					//T_MoveTo(m_lPrePickPos_T);

					if (/*!m_bFirstCycle && */!m_bChangeGrade)	
					{
						SetBhTReady(TRUE, "1");
						SetBhTReadyForWT(TRUE, "2");		//Let WT to perform WPR	//MS60
					}

					if (m_bMS100EjtXY && m_bEnableMS100EjtXY)
					{
						m_bChangeGrade = TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
						
						szLog.Format("BH: EjtXY ChangeGrade 3; BH2 = %d", IsBHZ1ToPick()); 
						CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
						szLog.Format("BH - BH last alarm code %d with EjtXY ==> Prepick, -2=next die change grade; -3=next die bin full",
							m_nBHLastAlarmCode);

						//MS90 in general
						//If Last Grade last die is bonded by BH1, then Next Grade
						// 1st die will be picked & bonded by BH2; however, BT moves earlier after 
						// ChangeBin, so it uses wrong INDEX BH1.  Here, we reverse the BH sequence
						// for BT only, so it moves to correct BH offset (BH2 in this case)
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) !m_bBHZ2TowardsPick;
					}
					else
					{
						CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 3");
						DisplaySequence("BH - SetBTStartMoveWithoutDelay4==>FALSE");
						SetBTStartMoveWithoutDelay(FALSE);
						SetBTStartMove(FALSE);	// suggest by Andrew, HuaMao green light idle
						m_bFirstCycle		= TRUE;		
						m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
						szLog.Format("BH - BH last alarm code %d ==> Prepick, -2=next die change grade; -3=next die bin full",
							m_nBHLastAlarmCode);
					}

					DisplaySequence(szLog);
					//For BT to move to correct BH1/2 offset XY, in this case 1st die to bond must be BH1
					m_nBHAlarmCode		= 0;			
					m_nBHLastAlarmCode	= 0;	
					m_lBHStateCode		= 0;

					DisplaySequence("BH - SetBTStartMoveWithoutDelay5==>FALSE");
					SetBTStartMoveWithoutDelay(FALSE);
					SetBTStartMove(FALSE);

					//EnableBHTThermalControl(TRUE, TRUE);
					//SetBondHeadFan(FALSE);

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					Sleep(500);

					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					break;
				}	//	change grade or bin full

				bMS60NGPickNoLFCycle = FALSE;
				BOOL bPickDie = (BOOL)((LONG)(*m_psmfSRam)["WaferTable"]["PickDie"]);
				if (!bPickDie || bMS60NGPickNoLFCycle)
				{
					DisplaySequence("BH - T move, to prepick, Unpick die to Wait WT");
					//	CheckCoverOpenInAuto("Unpick to prepick");
					// The die is only for alignment, move BH to PrePick
					SetWTReady(FALSE, "!bPickDie || bMS60NGPickNoLFCycle");	
					
					//andrewng //2020-0506
					WaitForDelay(m_lUpTime_Ej_Theta, s_dbStartEjTMove);
					TakeTime(Z6);
					WaitForDelay(m_lUpTime_BinEj_Theta, s_dbStartBinEjTMove);
					TakeTime(T2);

					//Extra delay to avoid BH lose-power problem due to ILC		
					//T_Sync();				//Must wait T complete for ILC
					//T_MoveTo(m_lPrePickPos_T, SFM_NOWAIT);		//MS60 NGPick
					Sleep(10);

					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					m_bChangeGrade	= FALSE;	//	wrong sort bug fix
					m_bFirstCycle	= FALSE;
					m_bBhInBondState = FALSE;
					m_qSubOperation	= WAIT_WT_Q;

					if (/*!m_bFirstCycle && */!m_bChangeGrade)
					{
						SetBhTReady(TRUE, "2");
						SetBhTReadyForWT(TRUE, "3");		//Let WT to perform WPR	//MS60
					}

					break;
				}

				if (bSwitchPR || IsMaxColletEjector_CheckOnly(bWaferEnd))	
				{
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ2);

						//2019.3.28 remove SetWTReady becasue it will lead into confuse
//						SetWTReady(FALSE, "SetWTReady FALSE (m_bComplete_Ej == FALSE) 3");				//REset WT flag again before letting it to move
						m_bComplete_Ej = TRUE;
						if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE)	
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move T Pr2DCode or EjPreheat, let ej ready");    //Let WT to INDEX
						}			
					}

					//andrewng //2020-0506
					WaitForDelay(m_lUpTime_Ej_Theta, s_dbStartEjTMove);
					TakeTime(Z6);
					WaitForDelay(m_lUpTime_BinEj_Theta, s_dbStartBinEjTMove);
					TakeTime(T2);

					//Extra delay to avoid BH lose-power problem due to ILC		
					//T_Sync();	//Must wait T complete for ILC
					//T_MoveTo(m_lPrePickPos_T, SFM_NOWAIT);

					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
					
					if (bSwitchPR)
					{
						DOUBLE dTime = GetTime();
						DisplaySequence("BH - DBH move T q, T to prepick, PR switch camera. 4");
						SwitchPrAOICamera();
						CString szMsg;
						szMsg.Format("BH - DBH move T q, PR switch camera. 5 used time %d", LONG(GetTime() - dTime));
						DisplaySequence(szMsg);
					}

					//T_Sync();

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					if (/*!m_bFirstCycle && */!m_bChangeGrade)		
					{
						SetBhTReady(TRUE, "3");
						SetBhTReadyForWT(TRUE, "4");		//Let WT to perform WPR	//MS60
					}
				
					m_bFirstCycle	= FALSE;	
					m_bChangeGrade	= FALSE;	
					m_bBhInBondState = FALSE;
					
					DisplaySequence("BH - AOI PR switch ==> Wait WT");
					m_qSubOperation = WAIT_WT_Q;
					break;
				}

				m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;	//Toggle BHZ1 & BHZ2
				(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
				s_dbStartT2PB = GetTime();		
				m_qSubOperation = WAIT_Z_EJ_COMPLETE_Q;
			}
			break;

		case WAIT_Z_EJ_COMPLETE_Q:
			//DisplaySequence("BH - wait Z EJ complete q");
			DisplaySequence("BH - WAIT_Z_EJ_COMPLETE_Q");

			if (!m_bWaferPrMS60LFSequence)		
			{
				/*if (!OpMoveEjectorTableXY(FALSE))	
				{
					SetErrorMessage("Ejector XY module is off power");
					SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}*/
			}

			//andrewng //2020-0506	//normal flow
			lTemp = (LONG)(GetTime() - s_dbStartT2PB);
			lTMinPrUnblockTime = m_lDnTime_Ej_Theta - 6;
			WaitForDelay(lTMinPrUnblockTime, s_dbStartEjTMove);

			szMsg.Format("BH - lTMinPrUnblockTime = %d, m_lDnTime_Ej_Theta = %d", lTMinPrUnblockTime, m_lDnTime_Ej_Theta);
			DisplaySequence(szMsg);

			if (/*!m_bFirstCycle && */!m_bChangeGrade)	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
			{
				SetBhTReadyForWT(TRUE, "6");			//Let WT to perform WPR	//MS60
				DisplaySequence("BH - let BPR to grab, set BHT ready");
				SetBhTReady(TRUE, "4");					//Let BPR Grade image
			}
		
			if (m_bSetEjectReadyDone == FALSE)
			{
				m_bSetEjectReadyDone = TRUE;
				SetEjectorReady(TRUE, "Wait Z EJ Complete at last");
			}

			//andrewng //2020-0506	//normal flow
			WaitForDelay(m_lUpTime_Ej_Theta, s_dbStartEjTMove);
			TakeTime(Z6);
			WaitForDelay(m_lUpTime_BinEj_Theta, s_dbStartBinEjTMove);
			TakeTime(T2);

			m_bFirstCycle	= FALSE;
			m_bChangeGrade	= FALSE;
			m_qSubOperation = WAIT_WT_READY_Q;	
			break;

		//**** Currently not used ****//
		case WAIT_BT_CHANGE_GRADE:
			DisplaySequence("BH - Wait BT Change Grade");
			if (m_bStop)
			{
				DisplaySequence("BH - |Wait BT change grade stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if (WaitBTStartMove())
			{
				//Assum wafer table die is ready
				SetWTReady(FALSE, "WAIT_BT_CHANGE_GRADE");
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				DisplaySequence("BH - Wait BT ChangeGrade done ==> T Prepick to pick");
CMSLogFileUtility::Instance()->BL_LogStatus("BH - Wait BT ChangeGrade done ==> T Prepick to pick\n");	//By Andrew
			}
			else
			{
				Sleep(1);
			}
			break;


		case WAIT_BIN_FULL_Q:
			DisplaySequence("BH - Wait Bin Full");

			// Wait for bin full alert issued by Bin Table Station
			if (m_bStop)
			{
				SetErrorMessage("BH: BIN_FULL -> stop");
				DisplaySequence("BH - |Wait Bin full stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if (WaitBTStartMove())
			{
				//Assum wafer table die is ready
				SetWTReady(FALSE, "WAIT_BIN_FULL_Q");
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait Bin Full ==> T Prepick to pick");
				//	WPR confirm search done, but this cycle will not reset, next move to wait and loop time out.
				SetConfirmSrch(FALSE);
			}
			else
			{
				Sleep(1);
			}
			break;


		case POSTBOND_FAIL_Q:
			DisplaySequence("BH - SPC failed");
			lSPCResult = m_lPrevDieSPCResult;	//(*m_psmfSRam)["BondPr"]["PostBondResult"];
			EmptyCounter = (*m_psmfSRam)["BondPr"]["EmptyCounter"];
			EmptyCounter2 = (*m_psmfSRam)["BondPr"]["EmptyCounter2"];
			AccEmptyCounter = (*m_psmfSRam)["BondPr"]["AccEmptyCounter"];
			AccEmptyCounter2 = (*m_psmfSRam)["BondPr"]["AccEmptyCounter2"];
			AccEmptyCounterTotal = (*m_psmfSRam)["BondPr"]["AccEmptyCounterTotal"];
			GenMaxEmptyAllow = (*m_psmfSRam)["BondPr"]["MaxEmptyAllow"];
			GenMaxAccEmptyAllow = (*m_psmfSRam)["BondPr"]["MaxAccEmptyAllow"];

			if (m_bDisableBT)
			{
				lSPCResult = 1;
			}
			switch (lSPCResult)
			{
			case BPR_ERR_SPC_XY_LIMIT:
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_XY_LIMIT);		// Set Alarm
				szMsg = _T("Postbond hit XY Limit Count");
				break;

			case BPR_ERR_SPC_XY_LIMIT2:		
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_XY_LIMIT);		// Set Alarm
				szMsg = _T("Postbond hit XY Limit (BH2) Count");
				break;

			case BPR_ERR_SPC_MAX_ANGLE:
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_MAX_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Max Angle (BH1) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ANGLE2:	
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_MAX_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Max Angle (BH2) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ACC_ANGLE:	
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_MAX_ACC_ANGLE);	// Set Alarm
				szMsg = _T("Postbond hit Max ACC Angle (BH1) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ACC_ANGLE2:
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_MAX_ACC_ANGLE);	// Set Alarm
				szMsg = _T("Postbond hit Max ACC Angle (BH2) Limit Count");
				break;

			case BPR_ERR_SPC_AVG_ANGLE:
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_AVG_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Average Angle Limit");
				break;

			case BPR_ERR_SPC_DEFECT:
				SetAlert_Red_Yellow(IDS_BH_SPC_DEFECT);			// Set Alarm
				szMsg = _T("Postbond hit Defect Limit Count");
				break;

			case BPR_ERR_SPC_EMPTY:
				LogPBFailureCaseDumpFile();
				OpDisplayAlarmPage(TRUE);	
				szMsg.Format("Postbond hits EMPTY Limit Count.\nBH1: Continuation=%ld(%ld), Accumlation=%ld\nBH2: Continuation=%ld(%ld), Accumlation=%ld\nTotal Accumlation=%ld(%ld)\n", EmptyCounter, GenMaxEmptyAllow, AccEmptyCounter, EmptyCounter2, GenMaxEmptyAllow, AccEmptyCounter2, AccEmptyCounterTotal, GenMaxAccEmptyAllow);
				SetAlert_Msg_Red_Yellow(IDS_BH_SPC_EMPTY, szMsg);
				SetErrorMessage(szMsg);
				break;

			case BPR_ERR_SPC_EMPTY2:		
				LogPBFailureCaseDumpFile();
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_EMPTY);			// Set Alarm
				szMsg = _T("Postbond hit EMPTY Limit (BH2) Count");
				break;

			case BPR_ERR_SPC_REL_XY_LIMIT:						//Semitek	//Relative XY shift check
				SetAlert_Red_Yellow(IDS_BH_SPC_REL_XY_LIMIT);	// Set Alarm
				szMsg = _T("Postbond hit relative-XY shift Limit Count");
				break;

			default:
				SetAlert_Red_Yellow(IDS_BH_SPC_FAIL);			// Set Alarm
				szMsg.Format("Postbond Detect Error; code=%ld", lSPCResult);	
				break;
			}
			szTemp.Format(" die picked %d", GetNewPickCount());
			SetErrorMessage(szMsg + szTemp);
			DisplaySequence("BH - |post bond fail| ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("BH - DBH House Keeping Q");
			SetAutoCycleLog(FALSE);
			if (GetHouseKeepingToken("BondHeadStn") == TRUE)
			{
				SetHouseKeepingToken("BondHeadStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}

			// Make sure the motion is complete first
			// Wait Ej complete
			if (!m_bComplete_Ej || !m_bComplete_BinEj)
			{
				Ej_Sync();
				EjT_Sync();
				EjCap_Sync();
				EjElevator_Sync();

				BinEj_Sync();
				BinEjT_Sync();
				BinEjCap_Sync();
				BinEjElevator_Sync();

				m_bComplete_BinEj = TRUE;
				m_bComplete_Ej = TRUE;
			}					

			//Disable NuMotion data log earlier here
			CycleEnableDataLog(FALSE, TRUE, TRUE);

			SetEjectorVacuum(FALSE);
			Sleep(100);

			Ej_MoveTo(m_lStandbyLevel_Ej, SFM_WAIT);
			BinEj_MoveTo(m_lStandby_Bin_EJ_Pin, SFM_WAIT);
			//BinEjCap_MoveTo(m_lStandby_Bin_EJ_Cap, SFM_WAIT);		//andrewng //2020-0601
			
			EjT_MoveTo(m_lStandby_EJ_Theta, SFM_WAIT);			
			BinEjT_MoveTo(m_lStandby_Bin_EJ_Theta, SFM_WAIT);			

			TakeTime(CycleTime);
			StartTime(GetRecordNumber());	// Restart the time	

			CheckCoverOpenInAuto("During Bonding Cycle");
			
			bRegionEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"]; // xuzhijin_region_end
			bWaferEnd	= IsWaferEnded();
			bColletFull = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ColletFull"];

			if (bWaferEnd)							//Nichia MS100+
			{	
				LogCycleStopState("BH - WaferEnd");	//SEmitek
				OpResetEjKOffsetAtWaferEnd();		//Semitek
			}

			if (pSRInfo->IsRegionEnding())
			{
				CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
				if (pUtl->GetPrescanRegionMode())
				{
					m_ulEjCountForSubRegionKOffset = 0;
					m_ulEjCountForSubRegionSKOffset = 0;
					m_lEjSubRegionSmartValue = 0;
					pSRInfo->SetRegionEnding(FALSE);
					SaveScanTimeEvent("BH: REGION END reset k offset house keeping");
				}
			}

			CMS896AStn::m_lBondHeadAtSafePos = 1;
			(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;

			LogCycleStopState("BH - UpdateDieCounter");
			UpdateDieCounter();			//Update die counter in HMI

			LogCycleStopState("BH - LogWaferInfo");
			LogWaferInformation(FALSE);	//Update current wafer counter

			LogCycleStopState("BH - MachineStat");
			SaveMachineStatistic();		//update machine statistic	

			//ILC_SaveAllResult();

			// CMS896AStn::m_lBondHeadAtSafePos = 1;
			// Acknowledge other stations to stop
			StopCycle("BondHeadStn");
			LogCycleStopState("BH - dbh stop completely");
			State(STOPPING_Q);

			SetHouseKeepingToken("BondHeadStn", TRUE);

			if (IsPrescanning() && IsCoverOpen())
			{
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				SetStatusMessage("Machine Cover Open");
				SetErrorMessage("Machine Cover Open when prescanning");
			}
			break;

		default:
			DisplaySequence("BH - Unknown");
			m_nLastError = glINVALID_STATE;
			break;
		}

		if (m_nLastError != gnAMS_OK)
		{
			if (!IsAbort())
			{
				SetError(m_nLastError);
			}

			Result(gnNOTOK);
			State(ERROR_Q);
		}
		else
		{
			m_qPreviousSubOperation = m_qSubOperation;
		}
		if (m_bHeapCheck == TRUE)
		{
			// Check the heap
			INT nHeapStatus = _heapchk();
			if (nHeapStatus != _HEAPOK)
			{				
				CString szMsg;
				szMsg.Format("BH - Heap corrupted (%d) before BH", nHeapStatus);
				DisplayMessage(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		//Handle normal stop cycle task
		LogCycleStopState("BH ex - UpdateDieCounter");
		UpdateDieCounter();			

		LogCycleStopState("BH ex - LogWaferInfo");
		LogWaferInformation(FALSE);	

		LogCycleStopState("BH ex - MachineStat");
		SaveMachineStatistic();		

		CMS896AStn::m_lBondHeadAtSafePos = 1;

		DisplayException(e);
		Result(gnNOTOK);

		//State(ERROR_Q);
		State(STOPPING_Q);

		NeedReset(TRUE);
		StopCycle("BondHeadStn");

		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		CString szErr;
		szErr.Format("Exception detected in BH Cycle = %d, %d", m_qPreviousSubOperation, m_qSubOperation);
		SetMotionCE(TRUE, szErr);
	}
}
