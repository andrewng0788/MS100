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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CBondHead::RunOperation_MegaDa()
{
	CString szMsg, szTemp;
	LONG	lTemp = 0, lDelay = 0, lSPCResult = 0;
	LONG	lTotal = 0;
	LONG	lEjectorLvl = 0;
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
	static BOOL bSwitchPR			= FALSE;
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
		lHeadPickDelay = m_lPickTime_Z;
	}
	
	LONG lHeadBondDelay = m_lHeadBondDelay;
	if (m_lHeadBondDelay <= -1)
	{
		lHeadBondDelay = m_lBondTime_Z;
	}
	
	LONG lArmPickDelay = m_lArmPickDelay;
	if (m_lArmPickDelay <= -1)
	{
		lArmPickDelay = m_lPickTime_Z;
	}

	LONG lArmBondDelay = m_lArmBondDelay;
	if (m_lArmBondDelay <= -1)
	{
		lArmBondDelay = m_lBondTime_Z;
	}

	LONG lEjectorUpDelay	= m_lEjectorUpDelay;
	LONG lEjectorDownDelay	= m_lEjectorDownDelay;

	if (m_bUseDefaultDelays)	
	{
		lHeadPickDelay	= m_lPickTime_Z;
		lHeadBondDelay	= m_lBondTime_Z;
		lArmPickDelay	= m_lPickTime_Z;
		lArmBondDelay	= m_lBondTime_Z;
	}

	if (m_bEnableSyncMotionViaEjCmd == TRUE)
	{
		lTime_Ej = lTime_Ej + lEnableSyncMotionViaEjCmdExtraTime;
	}

	LONG lExtraPrGrabDelay = (*m_psmfSRam)["WaferPr"]["ExtraGrabDelay"];

	LONG lAutobondCurrTime = 0;
	LONG lWTStop		= (*m_psmfSRam)["WaferTable"]["AtSafePos"];
	LONG lBTStop		= (*m_psmfSRam)["BinTable"]["AtSafePos"];
	BOOL bEnableBHUplookPr	= pAppMod->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);	//v4.52A16


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

		if (IsBLBCRepeatCheckFail())
		{
			DisplaySequence("BH - |BL BC repeat fail| ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case WAIT_WT_Q:
			DisplaySequence("Mega Da BH - WAIT_WT_Q");
			//DisplaySequence("BH - dbh wait WT Q");

			bWaferEnd	= IsWaferEnded();

			if (m_bFirstCycle && !m_bFirstCycleAlreadyTakeLog)
			{
				CString szTemp;
				szTemp.Format("MegaDa Cycle #%d", GetRecordNumber());
				DisplaySequence(szTemp);
				StartTime(GetRecordNumber());

				m_bFirstCycleAlreadyTakeLog						= TRUE;
				(*m_psmfSRam)["BondHead"]["FirstCycleTakeLog"]	= m_bFirstCycleAlreadyTakeLog;
			}

			if( IsAutoRescanWafer() && (GetWftSampleState()==1 || GetWftCleanEjPinState()==1) && m_bFirstCycle )	// wait wt, first cycle
			{
				if (IsEnableBHMark() || IsEnableColletHole())
				{
					if (!WaitSearchMarkDone(200))
					{
						break;
					}
				}

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
			else if (m_bCheckPr2DCode && !m_bIs2DCodeDone)	//v4.40T1	//PLLM MS109 with BH 2D BC
			{
				CString szCode = _T("");
				BOOL b2DStatus = Search2DBarCodePattern(szCode, IsBHZ1ToPick(), TRUE);
				m_bIs2DCodeDone = TRUE;
				DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy2 Done");

				if (!b2DStatus)		//v4.41T3
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			//Only receive RPY2 here when BIN_FULL to avoid waiting for too long
			else if (IsBinFull() && bEnableBHUplookPr && m_lUpLookPrStage==2)
			{
				BOOL b2DStatus = TRUE;
				b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				m_lUpLookPrStage = 0;
				DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy2 Bin Full Done");
			}
			else if (bEnableBHUplookPr && m_lUpLookPrStage==1)		//v4.52A16	//v4.57A11
			{
				BOOL b2DStatus = TRUE;
				if (!m_bFirstCycle && !m_bChangeGrade)
				{
					T_Sync();
					TakeTime(T2);

					if (m_lUplookDelay > 0)
						Sleep(m_lUplookDelay);

					b2DStatus = UplookPrSearchDie_Rpy1(!m_bBHZ2TowardsPick);
					//TakeTime(LC3);	//v4.57A13	//v4.59A3
					if(b2DStatus)
					{
						m_lUpLookPrStage = 2;
						DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy1 Done");
					}
					else
					{
						DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy1 Fail");
					}
				}

				if (!b2DStatus)	
				{
					//SetErrorMessage("BHZ Uplook PR Search Failure to machine stop");
					//m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			else if (WaitWTReady() || bWaferEnd || IsSortTo2ndPart() || IsOnlyRegionEnd())	//v3.98T1 add WaferEnd checking
			{	
				//TakeTime(LC4);	//v4.57A13	//v4.59A2
				if (m_bFirstCycle)
				{
					DisplaySequence("Mega Da BH - First CycleWaitWTReady() Completed in WAIT_WT_Q");
				}
				else
				{					
					CString szTemp;
					szTemp.Format("WaitWTReady() Completed in WAIT_WT_Q,bWaferEnd=%d,IsSortTo2ndPart()=%d,IsOnlyRegionEnd()=%d", bWaferEnd, IsSortTo2ndPart(), IsOnlyRegionEnd());
					DisplaySequence("Mega Da BH - " + szTemp);
				}

				SetWTReady(FALSE, "SetWTReady FALSE WAIT_WT_Q");
				SetPRStart(FALSE);
				SetPRLatched(FALSE);
				SetEjectorReady(FALSE, "1");	//	Wait WT Q; WT moved or wafer/region end
				SetDieBondedForWT(FALSE);
				SetConfirmSrch(FALSE);
				SetWTStartMove(FALSE);				//Clear it once again as die-ready means WT not yet indexed for sure!
				m_qSubOperation = BOND_Z_WAIT_BT_READY_Q;//MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait WT ready ==> T Prepick to pick(BOND_Z_WAIT_BT_READY_Q)");
			}
			else if (m_bStop)	// && 	
			{
				//Allow BH to bond last-die on collet if bondarm is waiting at PRE-PICK
				if (m_bFirstCycle || m_bChangeGrade)
				{
					szMsg.Format("BH - Wait WT |firstcycle=%d or changegrade=%d| ==> HK", m_bFirstCycle, m_bChangeGrade);
					DisplaySequence(szMsg);
					m_qSubOperation = HOUSE_KEEPING_Q;	
				}
				else
				{
					if (IsMS90() && bEnableBHUplookPr&& (IsSortTo2ndPart() || m_b2PartsAllDone))	//andrewng
						;
					else
						m_bDBHStop = TRUE;
					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					DisplaySequence("BH - Wait WT stop ==> T Prepick to pick");
				}
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
			DisplaySequence("Mega Da BH - MOVE_T_PREPICK_TO_PICK_Q");
			if (IsAutoSampleSort())	// rescan block camera
			{
				if( IsAutoRescanWafer() )
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

				if( GetWftSampleState()>1 && m_bFirstCycle==FALSE && //	OR LET THE ADV SAMPLE COUNTER + 2 TO DELAY IT
					!IsBTChngGrd())	// if changing grade, wt at wait ej ready (not sampling), here should go on and sampling would do at opmovetable
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
				//Check BHT motion is completed before start other action
				if (T_IsComplete() == FALSE)
				{
					T_Sync();				
				}

				//** Wait for BT-Start to avoid extra head-PICK delay at PICK side **//
				if (m_bStop && (m_bFirstCycle || m_bChangeGrade || CMS896AStn::m_bBTAskBLBinFull==2))
				{
					szMsg.Format("BH - T Prepick to pick bstop |firstcycle=%d or changegrade=%d| ==> HK",
						m_bFirstCycle, m_bChangeGrade);
					DisplaySequence(szMsg);
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}

				if( IsMS90() && !WaitBTStartMove(200) )
				{
					if (bEnableBHUplookPr && m_lUpLookPrStage==2)
					{
						BOOL b2DStatus = TRUE;
						b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
						m_lUpLookPrStage = 0;
						DisplaySequence("BH - MS90 |WAIT WT | Uplook Rpy2 wait bt start move too long");
					}
					if (m_bStop && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE)
					{
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
					if (m_bStop )
					{
						if( CMS896AStn::m_bBTAskBLBinFull==2 || m_bDisableBL)
						{
							szMsg.Format("BH - 90 T Prepick to pick |bin loader error| ==> HK");
							DisplaySequence(szMsg);
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
					}
					DisplaySequence("BH - MS90 T Prepick to pick wait BT start move");
					break;
				}	//	for MS90 only
				else if (!IsMS90() && !WaitBTStartMove(200))	/*MS90 feature*/
				{
					if (bEnableBHUplookPr && m_lUpLookPrStage==2)
					{
						BOOL b2DStatus = TRUE;
						b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
						m_lUpLookPrStage = 0;
						DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy2 wait bt start move too long");
					}
					if (m_bStop)
					{
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
					else if (IsBTChngGrd() && CMS896AStn::m_bMS100OriginalBHSequence == FALSE)	//v4.15T7
					{
						//Normal CHANGE-GRADE triggering position in MS100 sequence when BH at PREPICK
						(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
						CMSLogFileUtility::Instance()->BL_LogStatus("BH: reset change grade state");

						if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
						{
							CString szLog;
							szLog.Format("BH: EjtXY ChangeGrade 1; BH2 = %d", IsBHZ1ToPick()); 
							CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

							(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"] = TRUE;	//v4.47A3
							m_bChangeGrade	= TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
							szMsg = "BH - Prepick To Pick changegrade=1 with EjtXY";
						}
						else
						{
							CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 1");	//v4.47T10
							CMSLogFileUtility::Instance()->BT_BackupTableIndexLog();

							m_bFirstCycle		= TRUE;		
							m_bBHZ2TowardsPick	= FALSE;			//BH1 at PICK = FALSE
							(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
							(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"] = TRUE;	//v4.47T9
							szMsg = "BH - Prepick To Pick firstcycle=1 without EjtXY";
						}
						DisplaySequence(szMsg);

						m_lBHStateCode		= 0;
						m_nBHAlarmCode		= 0;	
						m_nBHLastAlarmCode	= 0;
						break;
					}
					DisplaySequence("BH - T Prepick to pick wait BT start move");
					break;
				}

				if ((CMS896AApp::m_bBondHeadILCFirstCycle) && (IsEnableILC() == TRUE) && m_bFirstCycle)
				{	
					if( WaitBTStartMove() )
					{
						if (IsMS60())	//v4.49A2
						{
							EnableBHTThermalControl(FALSE, TRUE);	//v4.49A5
						}

						CMS896AApp::m_bBondHeadILCFirstCycle = FALSE;
						m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;//ILC_RESTART_UPDATE_Q;

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

				if( m_bCycleFirstProbe )
				{
					if( IsReachACCMaxTimeOut() )
					{
						DisplaySequence("BH - start first cycle, ACC time out, do auto clean");
						BOOL bOldState = m_bBHZ2TowardsPick;
						m_ulCleanCount = m_ulMaxCleanCount;
						INT nColletCleanStatus = IsCleanCollet();
						if( nColletCleanStatus==1 )
						{
							DisplaySequence("BH - Prestart ACC error 1");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
						m_ulCleanCount++;
						if( bOldState )
							m_bBHZ2TowardsPick = FALSE;
						else
							m_bBHZ2TowardsPick = TRUE;
						DisplaySequence("BH - start first cycle, one head done, switch to another");
						nColletCleanStatus = IsCleanCollet();
						m_bBHZ2TowardsPick = bOldState;
						if( nColletCleanStatus==1 )
						{
							DisplaySequence("BH - Prestart ACC error 2");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
						DisplaySequence("BH - start first cycle, both done");
					}

					if( m_lAutoLearnLevelsIdleTime>0 )	//	do auto learn p/b levels after start sort. WT and BT should be OK.
					{
						DisplaySequence("BH - start first cycle, Auto learn pickbond levels begin");
						if( OpAutoLearnPickBondLevels(TRUE)==FALSE )
						{
							DisplaySequence("BH - start first cycle, Auto learn pickbond levels fail, house keeping");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
						DisplaySequence("BH - start first cycle, Auto learn pickbond levels done");
					}

					m_bCycleFirstProbe = FALSE;
				}

				if( IsBTChngGrd() )	// HuaMao green light idle, prepick to pick, reset
				{
					SetEjectorReady(FALSE, "2");
					(*m_psmfSRam)["BinTable"]["Change Grade"]  = FALSE;
					DisplaySequence("BH - T Prepick to Pick (BT start move, ejector not ready, change grade to false)");
				}

				if( !m_bOnVacuumAtPick )	//	prepick
				{
					TakeTime(PV1);	
					if (IsBHZ1ToPick())			//If BHZ1 towards PICK
					{
						SetPickVacuum(TRUE); 
					}	
					else
					{
						SetPickVacuumZ2(TRUE);
					}
				}

				//v4.49A11
				BOOL bBinChanged	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinChangedForBH"];			//v4.49A11
				if (bBinChanged)
				{
					(*m_psmfSRam)["BinTable"]["BinChangedForBH"] = FALSE;
					if (pAppMod->GetFeatureStatus(MS896A_FUNC_BINBLK_GELPAD))
					{
						OpAutoLearnBHZPickBondLevel(IsBHZ1ToPick(), TRUE);
					}
				}

				//v4.59A39
				if (m_bFirstCycle /*|| m_bChangeColletDone*/)
				{
					bStatus = OpMoveEjectorTableXY(TRUE, SFM_WAIT);
//					m_bChangeColletDone = FALSE;
				}
				else
				{
					if (m_bWaferPrMS60LFSequence)
						bStatus = TRUE;
					else
						bStatus = OpMoveEjectorTableXY(FALSE, SFM_WAIT);
				}
				if (!bStatus)
				{
					SetErrorMessage("Ejector XY module is off power!");
					SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("BH - move ejt table error -> house keeping");
					break;
				}

				m_bDetectDAirFlowAtPrePick = TRUE;
				if (m_bMS100DigitalAirFlowSnr)	
				{
				//	DisplaySequence("BH - DBH at T prepick to pick, Digi air flow snr");
					BH_Z_AirFlowLog(IsBHZ1ToPick(), "PrePick to PICK,");
				}

				// Wait for Pick Vacuum settling
				lDelay = (LONG)(s_dbSettling + m_lPickTime_Z - lHeadPickDelay - m_lPrePickTime_T);
				if (lDelay > 0)
				{
					//if (!IsEjtUpAndPreheat())	
					//	Sleep(lDelay);
				}

				//CSP
				BOOL bPreBondAlignPad = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];	//CSP
				if (bPreBondAlignPad)
				{
					//Need to add extra delay for BT to perform Pad alignment motion
					if ((m_lHeadPrePickDelay > 0) && (m_lHeadPrePickDelay <= 250))
					{
						Sleep(m_lHeadPrePickDelay + 20);
						//Sleep(m_lHeadPrePickDelay); //DavidTest
					}
					else
					{
						Sleep(50);
					}
				}
	
				if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
				{
					m_lUpLookPrStage = 0;	//reset
					BOOL b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
					DisplaySequence("BH - get uplook rpy2");
					if(!b2DStatus)
					{
						SetErrorMessage("BHZ *Uplook PR Search Rpy2 Failure to machine stop");
						m_bUplookResultFail = TRUE;		//v4.57A11
					}
				}

				if (bPreBondAlignPad && !m_bFirstCycle)	
				{
					
					LONG lPreBondDone	= (LONG)(*m_psmfSRam)["BinTable"]["PreBondDone"];
					if (lPreBondDone==0)	//Must check PreAlign result before BH down to PICK level
					{
						if ( m_bComplete_T == FALSE )
						{
							T_Sync();
						}
						Sleep(10);
						DisplaySequence("BH - waiting prebond PR result");
						break;
					}
					if( lPreBondDone==2 )
					{
						Sleep(10);
						DisplaySequence("BH - prebond PR fail, into house keeping q");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					(*m_psmfSRam)["BinTable"]["PreBondDone"] = FALSE;
				}

				// Make sure the T is at Pick before Z down
				if (IsZMotorsEnable() == FALSE)
				{
					//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetMotionCE(TRUE, "BH motion error in MOVE_T_PREPICK_TO_PICK_Q state");	//v4.59A19
					SetErrorMessage("BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					break;
				}

				if (!OpIsBinTableLevelAtDnLevel())				//v4.22T8	//Walsin China
				{
					//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetMotionCE(TRUE, "BT level UP detected in MOVE_T_PREPICK_TO_PICK_Q state");	//v4.59A19
					SetErrorMessage("BH: BT platform at UP level in MOVE_T_PREPICK_TO_PICK_Q !!");
					SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

		//		DisplaySequence("BH - DBH at T prepick to pick, check cover sen");
				CheckCoverOpenInAuto("at BH PrePick state");	//v4.31T9

		//		DisplaySequence("BH - DBH at T prepick to pick, enable thermal control");
				EnableBHTThermalControl(FALSE, TRUE);			//v4.49A5

		//		DisplaySequence("BH - DBH at T prepick to pick, move BH T");
				if (IsBHZ1ToPick())						//If BHZ1 towards PICK	
				{
					TakeTime(T3);
					BA_MoveTo(m_lPickPos_T, SFM_NOWAIT);
				}
				else
				{
					if (bEnableBHUplookPr)	//v4.57A14
						TakeTime(T3);
					else
						TakeTime(T1);			
					BA_MoveTo(m_lBondPos_T, SFM_NOWAIT);
				}

				m_bComplete_T = FALSE;
				m_bMoveFromPrePick = TRUE;		//v4.40T11	//Sanan	//Enable Head-PrePick delay for Sanan MS100+
				m_lTime_T = m_lPrePickTime_T;
				s_dbStartT2PB = GetTime();
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 0;
				(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
				m_dWaitBTStartTime	= GetTime();
	
				//2018.6.1 for trigger to bond arm down
//				SetBTStartMove(TRUE);

				m_qSubOperation		= BOND_Z_WAIT_BT_READY_Q;//PICK_Z_DOWN_Q;
				//DisplaySequence("BH - T Prepick to Pick ==> Pick Z Down");
			}
			break;


		case WAIT_WT_READY_Q:	// must after die picked
			DisplaySequence("Mega Da BH - WAIT_WT_READY_Q");
			// Check whether Z is completed
			if (m_bComplete_T == FALSE)
			{
				if (T_IsComplete() == TRUE)
				{
					if (IsBHZ2ToPick())		//If BHZ1 towards BOND
					{
						if (bEnableBHUplookPr)	//v4.57A13
							TakeTime(T4);
						else
							TakeTime(T2);
					}
					else
					{
						TakeTime(T4);
					}
					m_bComplete_T = TRUE;
				}
			}

			bWaferEnd	= IsWaferEnded();
			bRegionEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"];	// xuzhijin_region_end 

			if (m_bStop)
			{
				TakeTime(BD);		//v3.99T1
				SetEjectorReady(FALSE, "3");
				SetDieBondedForWT(FALSE);
				m_dWaitBTStartTime = GetTime();			//v2.93T2
				m_qSubOperation = WAIT_BT_READY_Q;	
				DisplaySequence("BH - Wait WT ready stop ==> Wait BT Ready");

				//**** Normal STOP triggering position when user presses STOP button on menu ****//
				if (IsMS90() && (IsSortTo2ndPart() || m_b2PartsAllDone))
				{
					DisplaySequence("BH - detect wafer end, do 1 more cycle in wait wt ready");
				}
				else
				{
					m_bDBHStop = TRUE;
				}
			}
			else if (WaitWTReady() || bWaferEnd || bRegionEnd || IsOnlyRegionEnd() )
			{	
				TakeTime(BD);
				SetConfirmSrch(FALSE);		
				SetWTReady(FALSE, "SetWTReady FALSE WAIT_WT_READY_Q");
				SetEjectorReady(FALSE, "4");	//	Wait WT ready q, WT moved or wafer/region end
				SetDieBondedForWT(FALSE);
				SetWTStartMove(FALSE);					//v3.89		//Clear it once again as die-ready means WT not yet indexed for sure!
				m_dWaitBTStartTime = GetTime();			//v2.93T2
				m_qSubOperation = WAIT_BT_READY_Q;	
				DisplaySequence("BH - wait WT ready ok ==> wait BT ready");
			}
			else if (WaitBadDieForT())
			{
				SetBadDieForT(FALSE); 
	//	V450X16	SetEjectorReady(FALSE, "5");
				OpBadDie(TRUE);				//v3.93T3
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
			else
			{
/*
				//v4.22T7	//Re-implemented for Cree HuiZhou for "NextBinFull NoPickDie" fcn only
				if ((pAppMod->GetCustomerName() == "Cree") && IsBinFull())		
				{
					SetErrorMessage("BH: BIN_FULL detected at WAIT WT_READY_Q");

					//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();	//Must wait T complete for ILC
					Sleep(100);	
				
					EnableBHTThermalControl(TRUE, TRUE);		//v4.49A5
					//SetBondHeadFan(FALSE);					//v4.54A7
					CheckCoverOpenInAuto("MS100 bin full");
					T_MoveTo(m_lPrePickPos_T);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					m_qSubOperation = WAIT_BIN_FULL_Q;
				}
				else 
*/
				if (m_nBHLastAlarmCode == -2)			//ChangeGrade NoPickDIe	//v4.15T1
				{
					m_qSubOperation = WAIT_BT_READY_Q;	//Go directly to WAIT_BT_READY_Q to change bin!
					DisplaySequence("BH - Wait WT ready alarm code -2 ==> wait BT ready");
				}

				if ( IsBTChngGrd() || IsBinFull() )	// HuaMao green light idle, at pick position, move away.
				{
					CString szMsg;
					szMsg.Format("BH - Wait WT ready (Next change %d change GRADE %d, BinFull %d) ==> wait BT Ready Q",
							OpIsNextDieChangeGrade(), IsBTChngGrd(), IsBinFull());
					DisplaySequence(szMsg);
					m_qSubOperation = WAIT_BT_READY_Q;	//Go directly to WAIT_BT_READY_Q to change bin!
					break;
				}
			}
			break;

		case WAIT_BT_READY_Q:
			DisplaySequence("Mega Da BH - WAIT_BT_READY_Q");
			bWaferEnd		= IsWaferEnded();		//v3.99T1

			if (lBTStop)
			{
				if( (IsMS90() && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE) )
				{
					DisplaySequence("BH - |BT stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			if (IsBinFull())
			{
				//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
				T_Sync();	//Must wait T complete for ILC
				Sleep(100);	
			
				EnableBHTThermalControl(TRUE, TRUE);	//v4.49A5
				//SetBondHeadFan(FALSE);			//v4.54A7

				CheckCoverOpenInAuto("MS100 bin full");
				T_MoveTo(m_lPrePickPos_T);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
				m_qSubOperation = WAIT_BIN_FULL_Q;
			}
/*
			else if (WaitBTStartMove(200))		//v3.99T1	//v4.01
			{
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				TakeTime(RD);
				SetBTStartMove(FALSE);
				m_qSubOperation = PICK_Z_DOWN_Q;
			}
*/
//=============================================================================
//  PICK Z down do not wait for WaitBTStartMove event
//=============================================================================
			else if (WaitBTStartMoveWithoutDelay(200))
			{
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				SetBTStartMoveWithoutDelay(FALSE);
				DisplaySequence("SetBTStartMoveWithoutDelay==>FALSE");
				m_qSubOperation = BOND_Z_WAIT_BT_READY_Q;//PICK_Z_DOWN_Q;
			}
//=============================================================================

			else if ( IsBTChngGrd() || IsOnlyRegionEnd() )
			{
				//Normal CHANGE-GRADE triggering position in MS100 sequence
			
				//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
				T_Sync();	//Must wait T complete for ILC
				Sleep(100);	

				CheckCoverOpenInAuto("MS100 change grade");
				if( !(IsBTChngGrd() && pUtl->GetPrescanRegionMode()) )
				{
					(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				}
				T_MoveTo(m_lPrePickPos_T);

				szMsg = "BH - Wait BT ready ==> T Prepick to pick ";
				//v4.15T1
				if (CMS896AStn::m_bMS100OriginalBHSequence == FALSE)
				{
					if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
					{
						szMsg += "Andrew: EjtXY ChangeGrade 2";
						SetErrorMessage("Andrew: EjtXY ChangeGrade 2");
						m_bChangeGrade = TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
					}
					else
					{
						CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 2");	//v4.47T10
						szMsg += "BH: Change Grade No Pick Die 2";
						m_bFirstCycle		= TRUE;		
						m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
					}

					//Need to reset NoPickDie warning here
					m_lBHStateCode		= 0;
					m_nBHAlarmCode		= 0;		//v4.15T6	
					m_nBHLastAlarmCode	= 0;		//v4.15T6
				}

				EnableBHTThermalControl(TRUE, TRUE);	//v4.49A5
				//SetBondHeadFan(FALSE);				//v4.54A7
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;

				if( pUtl->GetPrescanRegionMode() )	// xu_semitek
				{
					m_lBondDieCount++;
					SetBTReady(FALSE); //2018.3.28 
					SetDieBonded(TRUE);
					m_qSubOperation =  WAIT_BT_CHANGE_GRADE;
				}

				CMSLogFileUtility::Instance()->BL_LogStatus("BH: change grade to prepick");
				DisplaySequence(szMsg);
				if( IsBTChngGrd() )	// HuaMao green light idle, reset ejector ready state
				{
					SetEjectorReady(FALSE, "8");
				}
				break;
			}
			else if ((bWaferEnd && !IsMS90()) )// || (IsMS90() && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE ) )	//	!m_bMS90UplookPrDo1MoreCycle)			//v3.99T1
			{
				//SetErrorMessage("BH: WaferEnd encountered in WAIT_BT_READY state -> STOP");
				DisplaySequence("BH - |WaferEnd encountered in WAIT_BT_READY state| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ( bWaferEnd && IsMS90() && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE )	//	!m_bMS90UplookPrDo1MoreCycle)			//v3.99T1
			{
				//SetErrorMessage("BH: WaferEnd encountered in WAIT_BT_READY state -> STOP");
				DisplaySequence("BH - MS90 |WaferEnd encountered in WAIT_BT_READY state| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitBTReSrchDie())		//v4.xx
			{
				SetBTReSrchDie(FALSE);
				SetWTReady(TRUE, "SetWTReady TRUE WaitBTReSrchDie");
				T_Sync();			//Must wait T complete for ILC
				Sleep(20);	

				SetErrorMessage("BH: BPR ReSrchDie triggers BH to PrePICK");
				CheckCoverOpenInAuto("MS100 ReSrchDie");
				T_MoveTo(m_lPrePickPos_T);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = WAIT_WT_Q;
				break;
			}
			else	//v3.99T6
			{
				//Emergency STOP loop to avoid sw hangup here !! //v3.99 version
				if (m_bStop)
				{
					//if (!WaitBTStartMove(1000))
					if (!WaitBTStartMove(200))		//v4.59A31
					{
						SetErrorMessage("BH: abnormal STOP encountered in WAIT_BT_READY state -> STOP");
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
				}
			}
			break;


		case PICK_Z_DOWN_Q:
			DisplaySequence("Mega Da BH - PICK_Z_DOWN_Q");

			//if (IsBHZ1ToPick() && m_bAutoLearnPickLevelZ1)
			//{
			//	DisplaySequence("BH - Learn Pick Level Z1 before Z Down");
			//	if (m_bComplete_T == FALSE)
			//	{
			//		T_Sync();
			//		m_bComplete_T = TRUE;
			//		TakeTime(T2);
			//	}
			//	//if (!OpPrestartColletHoleCheck(TRUE,FALSE))
			//	//{
			//	//	m_qSubOperation = HOUSE_KEEPING_Q;	
			//	//	break;
			//	//}
			//	//T_MoveTo(m_lPickPos_T);
			//	//Sleep(100);
			//	if (!OpAutoLearnBHZPickLevel(FALSE))
			//	{
			//		m_qSubOperation = HOUSE_KEEPING_Q;	
			//		break;
			//	}
			//	DisplaySequence("BH - Learn Pick Level Z1 Completed");

			//}
			//else if (IsBHZ2ToPick() && m_bAutoLearnPickLevelZ2)
			//{
			//	DisplaySequence("BH - Learn Pick Level Z2 ");
			//	if (m_bComplete_T == FALSE)
			//	{
			//		T_Sync();
			//		m_bComplete_T = TRUE;
			//		TakeTime(T4);
			//	}

			//	if (!OpAutoLearnBHZPickLevel(TRUE))
			//	{
			//		m_qSubOperation = HOUSE_KEEPING_Q;	
			//		break;
			//	}
			//	DisplaySequence("BH - Learn Pick Level Z2 Completed");
			//}

			m_lSPCResult = (*m_psmfSRam)["BondPr"]["PostBondResult"];
			if (m_bDisableBT)
			{
				m_lSPCResult = 1;
			}
			if (m_lSPCResult != 1)
			{
				if (!CheckBPRErrorNeedAlarm(0))
				{
					m_lSPCResult = 1;
					(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
				}
			}

			if (m_lSPCResult != 1)
			{
				OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "1");	//v4.52A7	//Sanan & Semitek
			}

			if (m_lSPCResult != 1)		//v3.82
			{
				m_lPrevDieSPCResult = m_lSPCResult;
				OpSetMS100AlarmCode(6);			//BPR PostBond-fail
			}

			s_bDownToPick = FALSE;
			if (AllowMove() == TRUE)
			{
				//Check machine pressure
				//if (IsLowPressure() == TRUE)
				//{
				//	SetStatusMessage("Machine low pressure");
				//	SetErrorMessage("Machine low pressure");
				//	OpSetMS100AlarmCode(1);			//LowPressure
				//}

				//Check machine pressure
				//if (IsLowVacuumFlow())
				//{
				//	SetStatusMessage("Machine low vacuum flow in auto");
				//	SetErrorMessage("Machine low vacuum flow in auto");
				//	OpSetMS100AlarmCode(16);			//LowPressure
				//}

				// Check whether it is suitable time for Collet Jam check and Move Z
				LONG	lPickDelay = 0, lBondDelay = 0;
				LONG lTMvTm = (LONG)(GetTime() - s_dbStartT2PB);
				if (IsBHZ1ToPick())		//If BHZ1
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )		//v4.40T11
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z + m_lHeadPrePickDelay;
					}
					else
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z/* - 1*/;	//v4.43T7
					}
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )	//v4.49A9
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z2 + m_lHeadPrePickDelay;	
					else
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z2/* - 1*/;		//v4.43T7
				}	
				else
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )		//v4.40T11
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z2 + m_lHeadPrePickDelay;
					}
					else
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z2/* - 1*/;	//v4.43T7
					}
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )	//v4.49A9
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z + m_lHeadPrePickDelay;	
					else
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z/* - 1*/;		//v4.43T7
				}

				//m_bMoveFromPrePick = FALSE;		//v4.49A9

				//v3.86
				//****************************************//
				if (lPickDelay <= lBondDelay)
				{
					m_bPickDownFirst = TRUE;
					m_nBHZ1BHZ2Delay = labs(lBondDelay - lPickDelay); // - 2;	//v4.47T12
				}
				else
				{
					m_bPickDownFirst = FALSE;
					m_nBHZ1BHZ2Delay = labs(lPickDelay - lBondDelay); // - 2;	//v4.47T12
				}
				//****************************************//

				if (!WaitBTStartMoveWithoutDelay(0) && !m_bFirstCycle && !m_bPickDownFirst)
				{
					//====================================================================================
					//  Wait for BT table move event
					//====================================================================================
					if  (WaitBTStartMove(200))
					{
						TakeTime(RD);
						DisplaySequence("SetBTStartMove1==>FALSE");
						SetBTStartMove(FALSE);
					}
					else
					{
						break;
					}
				}
				//====================================================================================

				//4.52D17Auto rpy2 before bonding (steal time)	(PICK_Z_DOWN_Q)
				//if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
				//{
				//	s_dbStartZ2PK = GetTime();	

				//	BOOL b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				//
				//	m_lUpLookPrStage = 0;	//reset

				//	if(!b2DStatus)
				//	{
				//		SetErrorMessage("BHZ *Uplook PR Search Rpy2 Failure to machine stop");
				//		m_bUplookResultFail = TRUE;		//v4.57A11
				//		//m_qSubOperation = HOUSE_KEEPING_Q;
				//		//break;
				//	}

				//	lTemp = (LONG)(GetTime() - s_dbStartZ2PK);
				//	//m_nBHZ1BHZ2Delay = m_nBHZ1BHZ2Delay - lTemp;

				//	if (m_bPickDownFirst)
				//	{
				//		lPickDelay = lPickDelay - lTemp;
				//	}
				//	else
				//	{
				//		lBondDelay = lBondDelay - lTemp;
				//	}
				//}

				// check machine cover state
				if (IsCoverOpen() == TRUE)
				{
					SetStatusMessage("Machine Cover Open");
					SetErrorMessage("Machine Cover Open");
					OpSetMS100AlarmCode(2);			//CoverOpen
				}
				
				//20171025 Leo Protection of hit hand
				CheckCoverOpenInAuto("During Bonding Cycle");

				if (!OpCheckValidAccessMode(TRUE))		//v4.34T4	//SanAn
				{
					SetErrorMessage("OpCheckValidAccessMode fails");	//v4.53A23
					m_bDBHStop = TRUE;
				}

				if(!OpCheckStartLotCount())
				{
					//SetStatusMessage("OpCheckStartLotCount fails");
					SetErrorMessage("OpCheckStartLotCount fails");		//v4.53A23
					m_bDBHStop = TRUE;
				}

				//if ( IsMS60() == TRUE )		//Check BH Fan sensor
				//{
				//	if ( m_lCheckFanCount >= 50 )	// 0.06x50 = 3sec
				//	{
				//		if (OpCheckMS60BHFanOK() != TRUE)	//v4.48A1
				//		{
				//			OpSetMS100AlarmCode(8);	
				//		}
				//		m_lCheckFanCount = 0;
				//	}
				//	else
				//	{
				//		m_lCheckFanCount++;
				//	}
				//}


				if (m_bPickDownFirst)	//	pick side down firstly
				{
					TakeTime(CJ);		// Take Time

					BOOL bIsColletJam = TRUE;
					BOOL bReachedColletJamCount = FALSE;
					//if (!CheckColletJamCount(bIsColletJam, bReachedColletJamCount))
					//{
					//	SetErrorMessage("OpMS100AutoCleanCollet_MDCJ fails");
					//	m_bDBHStop = TRUE;
					//}
					//if (bIsColletJam && (bReachedColletJamCount || (OpCheckColletJam() == TRUE)))	//v2.58
					//{
					//	LogColletJamThresholdValue(IsBHZ1ToPick());
					//	//Disable NuMotion data log earlier here	//LeoLam	//v4.50A24
					//	CycleEnableDataLog(FALSE, TRUE, TRUE);

					//	SetErrorMessage("6# Collet is Jammed");
					//	(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					//
					//	if (IsBHZ1ToPick())		//If BHZ1	
					//	{
					//		m_lCJTotalCount1++;
					//		(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
					//		SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ1);
					//	}
					//	else
					//	{
					//		m_lCJTotalCount2++;
					//		(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"] = m_lCJTotalCount2;
					//		SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ2);
					//	}

					//	CheckCoverOpenInAuto("CJ1 alarm code 3");
					//	OpSetMS100AlarmCode(3);		//CJ
					//}
					//else
					{
						RuntimeUpdateColletJamThresholdValue();
						//v4.52A11	//XM SanAn
						//if (!m_bDetectDAirFlowAtPrePick && 
						//	!OpUpdateDAirFlowThresholdValue(IsBHZ2ToPick()))
						//{
						//	//OpSetMS100AlarmCode(11);	// 11: new alarm, not yet implemented !!!
						//	//		or
						//	m_qSubOperation = HOUSE_KEEPING_Q;	
						//	break;
						//}
						//v4.15T1	//CMLT
						if (OpIsNextDieChangeGrade())
						{
							OpSetMS100AlarmCode(-2);	//ChangeGrade NoPickDie
						}
						else if (OpIsNextDieBinFull())	//v4.21T2
						{
							OpSetMS100AlarmCode(-3);	//NextBinFull NoPickDie
						}

						BOOL bPickDie = TRUE;
						LONG eAction = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"];
						if( (WAF_CDieSelectionAlgorithm::WAF_EDieAction)eAction!=WAF_CDieSelectionAlgorithm::PICK )
						{
							bPickDie = FALSE;
						}
						LONG lNextPickRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
						LONG lNextPickCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
						lLastPickRow	= m_ulAtPickDieRow;
						lLastPickCol	= m_ulAtPickDieCol;
						if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop)
						{
							s_bDownToPick = TRUE;
							if( (lLastPickRow==lNextPickRow) && (lLastPickCol==lNextPickCol) )
							{
								s_bDownToPick = FALSE;
							}
							if( bPickDie==FALSE )
							{
								s_bDownToPick = FALSE;
							}
						}
						
						szMsg.Format("BH - PICKDOWN %d 1ST pick(%ld,%ld),bond(%ld,%ld); WPR %d BHZ%d",
							s_bDownToPick, lNextPickRow, lNextPickCol, lLastPickRow, lLastPickCol, bPickDie, IsBHZ1ToPick());
						DisplaySequence(szMsg);
						
						//if (s_bDownToPick)	
						//{
						//	CString szMsg = "";
						//	LONG lCompen_Z = OpCalCompen_Z(IsBHZ2ToPick());		//v4.53A25
						//	
						//	if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
						//	{
						//		TakeTime(Z3);
						//		if( IsBHZOnVacuumOnLayer() )
						//		{
						//			SetPickVacuum(FALSE);
						//		}
						//		if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
						//		{
						//			AC_Z1P_MoveToOrSearch(m_lPickLevel_Z + lCompen_Z, SFM_NOWAIT);
						//		}
						//		
						//		m_bComplete_Z = FALSE;
						//		szMsg.Format("BH - BHZ1 to pick %ld (COMP=%ld) in PICK_Z_DOWN_Q", m_lPickLevel_Z, lCompen_Z);
						//	}
						//	else
						//	{
						//		TakeTime(Z7);
						//		if( IsBHZOnVacuumOnLayer() )
						//		{
						//			SetPickVacuum(FALSE);
						//		}
						//		if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
						//		{
						//			AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2 + lCompen_Z, SFM_NOWAIT);
						//		}
						//		
						//		m_bComplete_Z2 = FALSE;
						//		szMsg.Format("BH - BHZ2 to pick %ld (COMP=%ld) in PICK_Z_DOWN_Q", m_lPickLevel_Z2, lCompen_Z);
						//	}

						//	m_dBHZPickStartTime = GetTime();	//shiraishi03
						//	DisplaySequence(szMsg);
						//	CMSLogFileUtility::Instance()->BPR_Arm1Log(szMsg);
						//}
					}
					s_dbStartZ2PK = GetTime();	//	pick down first
				}
				else
				{
					TakeTime(MD);	
					lLastPickRow	= m_ulAtPickDieRow;
					lLastPickCol	= m_ulAtPickDieCol;
					if (m_bFirstCycle || m_bChangeGrade)
					{
						m_lMD_Count		= GetMissingDieRetryCount();
						m_lMD_Count2	= GetMissingDieRetryCount();
						szMsg.Format("BH - Bond Down first firstcycle=%d, changegrade=%d", m_bFirstCycle, m_bChangeGrade);
						DisplaySequence(szMsg);
					}
					//else if (m_bUplookResultFail	|| 		//v4.57A11
					//		 OpCheckMissingDie())
					//{
					//	TakeTime(MDO);
					//	LONG lMissingDieStop = 0;

					//	if (!m_bUplookResultFail)
					//	{
					//		m_ulMissingDieCount++;
					//		WriteUnpickInfo(0, lLastPickRow, lLastPickCol);
					//		if (m_pulStorage != NULL)
					//		{
					//			*(m_pulStorage + BH_MISSINGDIE_OFFSET) = m_ulMissingDieCount;
					//			*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = m_ulCurrWaferMissingDieCount;
					//		}

					//		OpIncSKOffsetForBHZMD(IsBHZ1ToPick());

					//		// If retry is 0, simply stop
					//		if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
					//		{
					//			m_lMD_Count--;//for retry
					//			m_lMDCycleCount1++;
					//			m_lMDTotalCount1++;
					//			m_lNOMDCount1 = 0;
					//			(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"] = m_lMDTotalCount1; //4.54T15

					//			if (GetMDCycleLimit() > 0 && (m_lMDCycleCount1 >= GetMDCycleLimit()))
					//			{
					//				lMissingDieStop = 3;
					//			}

					//			if (m_lMD_Count <= 0)
					//			{
					//				lMissingDieStop = 1;
					//			}
					//		}
					//		else
					//		{
					//			m_lMD_Count2--;//for retry
					//			m_lMDCycleCount2++;
					//			m_lMDTotalCount2++;
					//			m_lNOMDCount2 = 0;
					//			(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"] = m_lMDTotalCount2; //4.54T15

					//			if (GetMDCycleLimit() > 0 && (m_lMDCycleCount2 >= GetMDCycleLimit()))
					//			{
					//				lMissingDieStop = 4;
					//			}

					//			if (m_lMD_Count2 <= 0)
					//			{
					//				lMissingDieStop = 2;
					//			}
					//		}
					//	}
					//	//v4.55		//v4.57A11
					//	//Uplook PR fail count as 1 MD count
					//	//if (m_bUplookResultFail)
					//	else
					//	{
					//		m_bUplookResultFail = FALSE;
					//		bUpLookFailAtMD = TRUE;

					//		//v4.59A5
					//		if (IsBHZ1ToPick())
					//		{
					//			if (m_lBHUplookPrFailLimit > 0)
					//			{
					//				m_ulBH2UplookPrFailCount++;
					//				if ((long)m_ulBH2UplookPrFailCount >= m_lBHUplookPrFailLimit) 
					//				{
					//					lMissingDieStop = 6;	//BHZ2 Uplook PR failure
					//					m_bUplookResultFail = TRUE;
					//				}
					//			}
					//		}
					//		else
					//		{
					//			if (m_lBHUplookPrFailLimit > 0)
					//			{
					//				m_ulBH1UplookPrFailCount++;
					//				if ((long)m_ulBH1UplookPrFailCount >= m_lBHUplookPrFailLimit) 
					//				{
					//					lMissingDieStop = 5;	//BHZ1 Uplook PR failure
					//					m_bUplookResultFail = TRUE;
					//				}
					//			}
					//		}
					//	}

					//	LogMissingDieThresholdValue(IsBHZ2ToPick());

					//	if (lMissingDieStop > 0)
					//	{
					//		BOOL bMissingDieStop = ReachMissingDieCountLimit();
					//		BOOL bReturn = TRUE;
					//		if (!bMissingDieStop)
					//		{
					//			bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, TRUE);
					//		}
	
					//		if (!bReturn || bMissingDieStop)
					//		{
					//			if (!bReturn)
					//			{
					//				SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z_DOWN_Q state");	//v4.59A19
					//				SetErrorMessage("BondHead module is off power");
					//				SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					//			}
					//			else
					//			{
					//				OpMissingDieUplookPrFailToStop(lMissingDieStop);	//v4.59A5
					//			}

					//			if (lMissingDieStop == 5 || lMissingDieStop == 6)
					//			{
					//				WriteUnpickInfo(8 ,lLastPickRow, lLastPickCol);
					//			}

					//			if( bUpLookFailAtMD )
					//				(*m_psmfSRam)["BondHead"]["MissingDie"] = 2;
					//			else
					//				(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
					//			m_qSubOperation = HOUSE_KEEPING_Q;	
					//			DisplaySequence("BH - |missing die| ==> house keeping");
					//			break;
					//		}
					//		// reset retry-count
					//		if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
					//		{
					//			m_lMD_Count		= GetMissingDieRetryCount();
					//			m_lMDCycleCount1 = 0;
					//		}
					//		else
					//		{
					//			m_lMD_Count2	= GetMissingDieRetryCount();
					//			m_lMDCycleCount2 = 0;
					//		}
					//		//Reset for BT table
					//		DisplaySequence("BH - PICK_Z_DOWN_Q, Missing die up look");
					//		OpSetMS100AlarmCode(-1);	//MD-Retry warning
					//	}
					//	else
					//	{
					//		BOOL bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, FALSE);		//v4.59A5

					//		if (!bReturn)
					//		{
					//			SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z_DOWN_Q state");	//v4.59A19
					//			SetErrorMessage("BondHead module is off power");
					//			SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					//			m_qSubOperation = HOUSE_KEEPING_Q;
					//			WriteUnpickInfo(IDS_BH_MODULE_NO_POWER,lLastPickRow,lLastPickCol);
					//			break;
					//		}
					//		else
					//		{
					//			//Reset for BT table
					//			DisplaySequence("BH - PICK_Z_DOWN_Q, Missing die up look");
					//			OpSetMS100AlarmCode(-1);	//MD-Retry warning
					//		}
					//	}
					//}
					else
					{
						////RuntimeUpdateMissingDieThresholdValue();
						//if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						//{
						//	m_lNOMDCount2++;
						//	m_lMD_Count2	= GetMissingDieRetryCount();    // Reset the local count
						//	if (m_lNOMDCount2 > 200)
						//	{
						//		DisplaySequence("BH - NO MD Count2 > 200 A");
						//		RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
						//	}
						//}	
						//else
						//{
						//	m_lNOMDCount1++;
						//	m_lMD_Count		= GetMissingDieRetryCount();    // Reset the local count
						//	if (m_lNOMDCount1 > 200)
						//	{
						//		DisplaySequence("BH - NO MD Count2 > 200 B");
						//		RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
						//	}
						//}

						////v4.35T2
						//lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];
						//if (lMotionTime_BT > 150)
						//{
						//	Sleep(m_lBinTableDelay);
						//}

						//if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						//{
						//	TakeTime(Z7);

						//	LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);		//v4.48A8

						//	AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
						//	//AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT);

						//	m_bComplete_Z2 = FALSE;
						//	szMsg.Format("BH - BHZ2 to bond %d false PICK_Z_DOWN_Q", lBondZ2);
						//}
						//else
						//{
						//	TakeTime(Z3);

						//	LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);	//v4.48A8

						//	AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
						//	//AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT);

						//	m_bComplete_Z = FALSE;
						//	szMsg.Format("BH - BHZ1 to bond %d false in PICK_Z_DOWN_Q", lBondZ1);
						//}
						//s_dBondZDownStart = GetTime();
						//CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						//DisplaySequence(szMsg);
						//CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);
					}
				}
				m_qSubOperation = BOND_Z_WAIT_BT_READY_Q; //PICK_Z2_DOWN_Q;	// -> normal flow
			}
			break;

		case BOND_Z_WAIT_BT_READY_Q:
			DisplaySequence("Mega Da BH - BOND_Z_WAIT_BT_READY_Q");

			m_bPickDownFirst = TRUE;
			if (!m_bFirstCycle && m_bPickDownFirst && (m_nBHZ1BHZ2Delay > 0))
			{
				//waiting the remain headbond delay and allow Z to move bond position
				Sleep(m_nBHZ1BHZ2Delay);
				m_nBHZ1BHZ2Delay = 0;
			}
			
			if (!WaitBTStartMoveWithoutDelay(0) && !m_bFirstCycle && m_bPickDownFirst)
			{
				//====================================================================================
				//  Wait for BT table move event
				//====================================================================================
				if  (WaitBTStartMove(200))
				{
					m_nBHZ1BHZ2Delay = 0;
					TakeTime(RD);
					DisplaySequence("SetBTStartMove2==>FALSE");
					SetBTStartMove(FALSE);
					//m_qSubOperation = PICK_Z2_DOWN_Q;	// -> normal flow
					m_qSubOperation = EJ_UP_Q;
				}
				break;
				//====================================================================================
			 } //go to PICK_Z2_DOWN_Q

		//case PICK_Z2_DOWN_Q:
		//	DisplaySequence("Mega Da BH - PICK_Z2_DOWN_Q");
		//	//if( IsBHZ1ToPick() )	// HuaMao green light idle
		//	//	DisplaySequence("BH - PICK_Z2_DOWN_Q pick z1 down q");
		//	//else
		//	//	DisplaySequence("BH - PICK_Z2_DOWN_Q pick z2 down q");

		//	if (AllowMove() == TRUE)
		//	{
		//		if (!m_bFirstCycle)	//ashiraishi03
		//		{
		//			if (m_nBHZ1BHZ2Delay > 0)	//v4.47T7
		//			{
		//				Sleep(m_nBHZ1BHZ2Delay);
		//			}
		//		}

		//		if (!m_bPickDownFirst)
		//		{
		//			if (IsBHZ1ToPick())		//If BHZ1
		//			{
		//				if (m_lPickTime_Z <= lHeadPickDelay && (m_bComplete_T == FALSE))
		//				{
		//					T_Sync();
		//					m_bComplete_T = TRUE;	//v4.43T1
		//					TakeTime(T4);
		//				}
		//			}
		//			else
		//			{
		//				if (m_lPickTime_Z2 <= lHeadPickDelay && (m_bComplete_T == FALSE))
		//				{
		//					T_Sync();
		//					m_bComplete_T = TRUE;	//v4.43T1
		//					
		//					if (bEnableBHUplookPr)	//v4.57A13
		//						TakeTime(T4);
		//					else
		//						TakeTime(T2);
		//				}
		//			}

		//			TakeTime(CJ);		// Take Time

		//			BOOL bIsColletJam = TRUE;
		//			BOOL bReachedColletJamCount = FALSE;
		//			if (!CheckColletJamCount(bIsColletJam, bReachedColletJamCount))
		//			{
		//				SetErrorMessage("OpMS100AutoCleanCollet_MDCJ fails");
		//				m_bDBHStop = TRUE;
		//			}

		//			if (bIsColletJam && (bReachedColletJamCount || (OpCheckColletJam() == TRUE)))	//v2.58
		//			{
		//				LogColletJamThresholdValue(IsBHZ1ToPick());

		//				//Disable NuMotion data log earlier here	//LeoLam	//v4.50A24
		//				CycleEnableDataLog(FALSE, TRUE, TRUE);

		//				SetErrorMessage("7# Collet is Jammed");
		//				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
		//				
		//				if (IsBHZ1ToPick())		//If BHZ1	
		//				{
		//					m_lCJTotalCount1++;
		//					(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
		//					SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ1);
		//				}
		//				else
		//				{
		//					m_lCJTotalCount2++;
		//					(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"] = m_lCJTotalCount2;
		//					SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ2);
		//				}

		//				CheckCoverOpenInAuto("CJ2 alarm code 3");

		//				OpSetMS100AlarmCode(3);		//CJ
		//			}
		//			else
		//			{
		//				RuntimeUpdateColletJamThresholdValue();
		//				//v4.52A11	//XM SanAn
		//				if (!m_bDetectDAirFlowAtPrePick && 
		//					!OpUpdateDAirFlowThresholdValue(IsBHZ2ToPick()))
		//				{
		//					//OpSetMS100AlarmCode(11);	// 11: new alarm, not yet implemented !!!
		//					//		or
		//					m_qSubOperation = HOUSE_KEEPING_Q;	
		//					break;
		//				}

		//				//v4.15T1	//CMLT
		//				if (OpIsNextDieChangeGrade())
		//				{
		//					OpSetMS100AlarmCode(-2);	//ChangeGrade NoPickDie
		//				}
		//				else if (OpIsNextDieBinFull())	//v4.21T2
		//				{
		//					OpSetMS100AlarmCode(-3);	//NextBinFull NoPickDie
		//				}

		//				BOOL bPickDie = TRUE;
		//				LONG eAction = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"];
		//				if( (WAF_CDieSelectionAlgorithm::WAF_EDieAction)eAction!=WAF_CDieSelectionAlgorithm::PICK )
		//				{
		//					bPickDie = FALSE;
		//				}

		//				LONG lNextPickRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
		//				LONG lNextPickCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
		//				lLastPickRow	= m_ulAtPickDieRow;
		//				lLastPickCol	= m_ulAtPickDieCol;
		//				if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop)		//v4.15T1
		//				{
		//					s_bDownToPick = TRUE;
		//					if( (lLastPickRow==lNextPickRow) && (lLastPickCol==lNextPickCol) )
		//					{
		//						s_bDownToPick = FALSE;
		//					}
		//					if( bPickDie==FALSE )
		//					{
		//						s_bDownToPick = FALSE;
		//					}
		//				}
		//				szMsg.Format("BH - PICKDOWN %d 2ND pick(%ld,%ld),bond(%ld,%ld); WPR %d BHZ%d",
		//					s_bDownToPick, lNextPickRow, lNextPickCol, lLastPickRow, lLastPickCol, bPickDie, IsBHZ1ToPick());
		//				DisplaySequence(szMsg);
		//				if( s_bDownToPick )
		//				{
		//					CString szMsg;
		//					if (IsBHZ1ToPick())	//If BHZ1	
		//					{
		//						TakeTime(Z3);
		//						if( IsBHZOnVacuumOnLayer() )
		//						{
		//							SetPickVacuum(FALSE);
		//						}
		//						if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
		//						{	
		//							AC_Z1P_MoveToOrSearch(m_lPickLevel_Z, SFM_NOWAIT);
		//						}
		//						m_bComplete_Z = FALSE;
		//						szMsg.Format("BH - BHZ1 to pick %d in PICK_Z2_DOWN_Q", m_lPickLevel_Z);
		//					}
		//					else
		//					{
		//						TakeTime(Z7);
		//						//v4.47T9
		//						//
		//						if( IsBHZOnVacuumOnLayer() )
		//						{
		//							SetPickVacuum(FALSE);
		//						}
		//						if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
		//						{
		//							AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2, SFM_NOWAIT);
		//						}
		//						//
		//						m_bComplete_Z2 = FALSE;
		//						szMsg.Format("BH - BHZ2 to pick %d in PICK_Z2_DOWN_Q", m_lPickLevel_Z2);
		//					}

		//					m_dBHZPickStartTime = GetTime();	//shiraishi03

		//					CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		//					DisplaySequence(szMsg);
		//					CMSLogFileUtility::Instance()->BPR_Arm1Log(szMsg);
		//				}

		//				s_dbStartZ2PK = GetTime();	//	pick down secondly
		//			}

		//			//m_qSubOperation = EJ_UP_Q;
		//			m_qSubOperation = SEARCH_MARK_Q;
		//			DisplaySequence("BH - Pick Z2 Down bd1st ==> EJ UP");
		//		}
		//		else
		//		{
		//			if (IsBHZ1ToPick())		//If BHZ1
		//			{
		//				if( (m_lBondTime_Z2 <= lHeadBondDelay) && (m_bComplete_T == FALSE))
		//				{
		//					T_Sync();
		//					m_bComplete_T = TRUE;	//v4.43T1
		//					TakeTime(T4);
		//				}
		//			}
		//			else
		//			{
		//				if( (m_lBondTime_Z <= lHeadBondDelay) && (m_bComplete_T == FALSE))
		//				{
		//					T_Sync();
		//					m_bComplete_T = TRUE;	//v4.43T1

		//					if (bEnableBHUplookPr)	//v4.57A13
		//						TakeTime(T4);
		//					else
		//						TakeTime(T2);
		//				}
		//			}

		//			TakeTime(MD);	

		//			if (m_bFirstCycle || m_bChangeGrade)
		//			{
		//				//m_qSubOperation = EJ_UP_Q;				// <- Normal flow 
		//				m_lMD_Count		= GetMissingDieRetryCount();
		//				m_lMD_Count2	= GetMissingDieRetryCount();
		//				m_qSubOperation = SEARCH_MARK_Q;
		//				szMsg.Format("BH - Pick_Z2_Down pick firstcycle=%d, changegrade=%d ==> EJ UP", m_bFirstCycle, m_bChangeGrade);
		//				DisplaySequence(szMsg);
		//			}
		//			else if (m_bUplookResultFail ||	(OpCheckMissingDie() == TRUE))
		//			{
		//				TakeTime(MDO);
		//				LONG lMissingDieStop = 0;

		//				lLastPickRow	= m_ulAtPickDieRow;
		//				lLastPickCol	= m_ulAtPickDieCol;
		//				if (!m_bUplookResultFail)	//v4.59A7
		//				{
		//					IncreaseMissingDieCounter();
		//					WriteUnpickInfo(0, lLastPickRow, lLastPickCol);
		//					if (m_pulStorage != NULL)
		//					{
		//						*(m_pulStorage + BH_MISSINGDIE_OFFSET) = m_ulMissingDieCount;
		//						*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = m_ulCurrWaferMissingDieCount;
		//					}

		//					//v4.53A25	//Semitek
		//					OpIncSKOffsetForBHZMD(IsBHZ1ToPick());

		//					// If retry is 0, simply stop
		//					if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
		//					{
		//						m_lMD_Count--;//for retry
		//						m_lMDCycleCount1++;
		//						m_lMDTotalCount1++;
		//						m_lNOMDCount1 = 0;
		//						(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"] = m_lMDTotalCount1; //4.54T15
		//						if( GetMDCycleLimit() > 0 && (m_lMDCycleCount1 >= GetMDCycleLimit()) )
		//						{
		//							lMissingDieStop = 3;
		//						}
		//						if (m_lMD_Count <= 0)
		//						{
		//							lMissingDieStop = 1;
		//						}
		//					}
		//					else
		//					{
		//						m_lMD_Count2--;//for retry
		//						m_lMDCycleCount2++;
		//						m_lMDTotalCount2++;
		//						m_lNOMDCount2 = 0;
		//						(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"] = m_lMDTotalCount2; //4.54T15

		//						if( GetMDCycleLimit() > 0 && (m_lMDCycleCount2 >= GetMDCycleLimit()) )
		//						{
		//							lMissingDieStop = 4;
		//						}
		//						if (m_lMD_Count2 <= 0)
		//						{
		//							lMissingDieStop = 2;
		//						}
		//					}
		//				}
		//				//v4.55		//v4.57A11
		//				//Uplook PR fail count as 1 MD count
		//				//if (m_bUplookResultFail)
		//				else
		//				{
		//					m_bUplookResultFail = FALSE;
		//					bUpLookFailAtMD = TRUE;

		//					//v4.59A5
		//					if (IsBHZ1ToPick())
		//					{
		//						m_ulBH2UplookPrFailCount++;
		//						if ((long)m_ulBH2UplookPrFailCount >= m_lBHUplookPrFailLimit) 
		//						{
		//							m_bUplookResultFail = TRUE;
		//							lMissingDieStop = 6;	//BHZ2 Uplook PR failure
		//						}
		//					}
		//					else
		//					{
		//						m_ulBH1UplookPrFailCount++;
		//						if ((long)m_ulBH1UplookPrFailCount >= m_lBHUplookPrFailLimit) 
		//						{
		//							m_bUplookResultFail = TRUE;
		//							lMissingDieStop = 5;	//BHZ1 Uplook PR failure
		//						}
		//					}
		//				}

		//				LogMissingDieThresholdValue(IsBHZ2ToPick());

		//				if (lMissingDieStop > 0)
		//				{
		//					BOOL bMissingDieStop = ReachMissingDieCountLimit();
		//					BOOL bReturn = TRUE;
		//					if (!bMissingDieStop)
		//					{
		//						bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, TRUE);
		//					}
	
		//					if (!bReturn || bMissingDieStop)
		//					{
		//						if (!bReturn)
		//						{
		//							SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z_DOWN_Q state");	//v4.59A19
		//							SetErrorMessage("BondHead module is off power");
		//							SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		//						}
		//						else
		//						{
		//							OpMissingDieUplookPrFailToStop(lMissingDieStop);	//v4.59A5
		//						}

		//						if (lMissingDieStop == 5 || lMissingDieStop == 6 )
		//						{
		//							WriteUnpickInfo(8 ,lLastPickRow, lLastPickCol);
		//						}

		//						if( bUpLookFailAtMD )
		//							(*m_psmfSRam)["BondHead"]["MissingDie"] = 2;
		//						else
		//							(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
		//						DisplaySequence("BH - |missing| ==> house keeping");
		//						m_qSubOperation = HOUSE_KEEPING_Q;	
		//						break;		//v3.86T3
		//					}
		//					// reset retry-count
		//					if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
		//					{
		//						m_lMD_Count		= GetMissingDieRetryCount();
		//						m_lMDCycleCount1 = 0;
		//					}
		//					else
		//					{
		//						m_lMD_Count2	= GetMissingDieRetryCount();
		//						m_lMDCycleCount2 = 0;
		//					}
		//					//Reset for BT table
		//					//m_qSubOperation = EJ_UP_Q;				// <- Normal flow	//v4.08
		//					m_qSubOperation = SEARCH_MARK_Q;
		//					DisplaySequence(szMsg + " slow ==> EJ UP");
		//					OpSetMS100AlarmCode(-1);		//MD-Retry warning	
		//					DisplaySequence("BH - PICK_Z2_DOWN_Q, Missing die up look ==> EJ UP");
		//				}
		//				else
		//				{
		//					BOOL bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, FALSE);		//v4.59A5
		//					
		//					if (!bReturn)
		//					{
		//						//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		//						SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z2_DOWN_Q state");	//v4.59A19
		//						
		//						SetErrorMessage("BondHead module is off power");
		//						SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		//						m_qSubOperation = HOUSE_KEEPING_Q;
		//						WriteUnpickInfo(IDS_BH_MODULE_NO_POWER,lLastPickRow,lLastPickCol);
		//						break;
		//					}
		//					else
		//					{
		//						//Reset for BT table
		//						//m_qSubOperation = EJ_UP_Q;				// <- Normal flow	//v4.08
		//						m_qSubOperation = SEARCH_MARK_Q;
		//						DisplaySequence(szMsg + " slow ==> EJ UP");
		//						OpSetMS100AlarmCode(-1);		//MD-Retry warning	
		//						DisplaySequence("BH - PICK_Z2_DOWN_Q, Missing die up look ==> EJ UP");
		//					}
		//				}
		//			}
		//			else
		//			{
		//										//RuntimeUpdateMissingDieThresholdValue();
		//				if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
		//				{
		//					m_lNOMDCount2++;
		//					m_lMD_Count2	= GetMissingDieRetryCount();    // Reset the local count
		//					if (m_lNOMDCount2 > 200)
		//					{
		//						DisplaySequence("BH - NO MD Count2 > 200 C");
		//						RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
		//					}
		//				}	
		//				else
		//				{
		//					m_lNOMDCount1++;
		//					m_lMD_Count		= GetMissingDieRetryCount();    // Reset the local count
		//					if (m_lNOMDCount1 > 200)
		//					{
		//						DisplaySequence("BH - NO MD Count1 > 200 D");
		//						RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
		//					}
		//				}	

		//				//v4.35T2
		//				lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];
		//				if (!(IsEjtUpAndPreheat()) && (lMotionTime_BT > 150))	//v4.47T8
		//				{
		//					Sleep(m_lBinTableDelay);
		//				}

		//				if (IsBHZ1ToPick())		//If BHZ1
		//				{
		//					TakeTime(Z7);

		//					LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);	//v4.48A8
		//					AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
		//					//AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT);
		//					m_bComplete_Z2 = FALSE;
		//					szMsg.Format("BH - BHZ2 to bond %d in PICK_Z2_DOWN_Q", lBondZ2);
		//				}
		//				else
		//				{
		//					TakeTime(Z3);
		//					
		//					LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);	//v4.48A8
		//					AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
		//					//AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT);
		//					m_bComplete_Z = FALSE;
		//					szMsg.Format("BH - BHZ1 to bond %d in PICK_Z2_DOWN_Q", lBondZ1);
		//				}
		//				DisplaySequence(szMsg + " ==> EJ UP");
		//				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		//				CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);
		//				s_dBondZDownStart = GetTime();
		//				m_qSubOperation = EJ_UP_Q;			//v4.08	
		//				//m_qSubOperation = SEARCH_MARK_Q;
		//			}
		//		}
		//	}
		//	break;

		case EJ_UP_Q:
			DisplaySequence("Mega Da BH - EJ_UP_Q");
			if (IsEnableBHMark()/* || IsEnableColletHole()*/)
			{
				if (!WaitSearchMarkReady())
				{
					break;
				}
				DisplaySequence("BH - SearchBHMark Done!");
			}

			if (AllowMove() == TRUE)
			{
				if (m_bEnableBHZOpenDac)
				{
					Z_SetOpenDACSwPort(TRUE);
					Z2_SetOpenDACSwPort(TRUE);
				}
				
				if (m_bComplete_T == FALSE)
				{
					T_Sync();
					m_bComplete_T = TRUE;		//v4.43T1

					if (IsBHZ2ToPick())		//If BHZ1 towards BOND
					{
						TakeTime(T2);
					}
					else
					{
						TakeTime(T4);
					}
				}
				bWaferEnd = IsWaferEnded();
				BOOL bOnlyRegionEnd	= IsOnlyRegionEnd(); // xuzhijin_region_end

				OpSyncEjectorTableXY();	//v4.42T3
				//v4.46T10	//SEmitek
				if (s_bDownToPick && !bWaferEnd && !bOnlyRegionEnd)
				{
					if (!OpCheckEjtXYBHPos(IsBHZ1ToPick()))
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}
				if (m_bEnableSyncMotionViaEjCmd || (lEjectorUpDelay >= lTime_Ej))
				{
					//Syn BH at PICK side BEFORE EJ UP
					if (IsBHZ1ToPick())		//If BHZ1 at PICK		
					{
						if (m_bComplete_Z == FALSE)
						{	
							if (!m_bEnableSyncMotionViaEjCmd)
							{
								Z_Sync();
							}
							m_bComplete_Z = TRUE;
							TakeTime(Z4);
						}
						//Sync BH at BOND side if motion completed before EJ UP		//v3.86
					}
					else
					{
						if (m_bComplete_Z2 == FALSE)
						{
							if (!m_bEnableSyncMotionViaEjCmd)
							{
								Z2_Sync();
							}
							m_bComplete_Z2 = TRUE;
							TakeTime(Z8);
						}
					}
				}
				
				lEjectorLvl = m_lEjectLevel_Ej;
				// Calculate the compensation for wear out of ejector pin
				LONG lCompen_Ej = OpCalCompen_Ej();
				//Because BHZ not yet motion completed, so need to wait for delay to move the ejector up
				//WaitForDelayToEjectorUpMove(lTime_Ej, lEjectorUpDelay, s_dbStartZ2PK);
				if (m_bEnableSyncMotionViaEjCmd)
				{
					if (IsBHZ1ToPick())
					{
						TakeTime(Z4);
					}
					else
					{
						TakeTime(Z8);
					}
				}
				//v4.51A16	//SanAn
				if (m_lSPCResult != 1)
				{
					OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "2");	//v4.52A7	//Sanan & Semitek
				}
				BOOL bEjtToUp = FALSE;
				//v4.26T1	//Fixed EJ Up delay bug when Sync-Pick fcn is used
				if (s_bDownToPick && /*!m_bDisableBH &&*/ (IsMS90() || !bWaferEnd) && (IsMS90() || !bOnlyRegionEnd))		//v4.15T1
				{	//	XU_WOLFSPEED
					bEjtToUp = TRUE;
				}
				s_dBHEJSyncTime = 0;
				if (bEjtToUp)
				{
					if (m_bEnableSyncMotionViaEjCmd)
					{
						CString szMsg;
						szMsg.Format("BH - EJ sync up begin to %d, Compen %d", lEjectorLvl, lCompen_Ej);
						DisplaySequence(szMsg);
						TakeTime(EJ1);	

						//v4.53A10
						Ej_SelectObwProfile((LONG)(lEjectorLvl - m_lCurLevel_Ej));
						Ej_SelectControl((LONG)(lEjectorLvl - m_lCurLevel_Ej));

						szMsg = "";
						//v4.46T2	//Semitek for SyncPick with KOffset
						if ((m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0))
						{
							MotionSetupSyncViaEjCmdDistance(BH_AXIS_EJ, &m_stBHAxis_Ej, (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej));
							szMsg.Format("BH - Ej SyncCmd, standby %d ", m_lStandbyLevel_Ej);
						}
TakeTime(LC1);//Matt:to check why Ej2-Ej1 over Ej motion time.
						if (IsBHZ1ToPick())
						{
							MotionSyncViaEjCmdDistance(BH_AXIS_Z, &m_stBHAxis_Z, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);
							szMsg = szMsg + "BH - Z1 SyncCmd";
						}
						else
						{
							MotionSyncViaEjCmdDistance(BH_AXIS_Z2, &m_stBHAxis_Z2, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);
							szMsg = szMsg + "BH - Z2 SyncCmd";
						}
						
//						LONG lSamplingRate = ULTRA_BH_SAMPLE_RATE;
//						CString szProfileID = m_stBHAxis_Ej.m_stMoveProfile[m_stBHAxis_Ej.m_ucMoveProfileID].m_szID;
//						s_dBHEJSyncTime = MotionGetFifthOrderProfileTime(BH_AXIS_EJ, szProfileID, lSamplingRate, (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej), &m_stBHAxis_Ej);
						if (m_lSyncTriggerValue > 0)
						{
							s_dBHEJSyncTime = CalculateEjQueryTime((lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej), m_lSyncTriggerValue);
						}
						else
						{
							s_dBHEJSyncTime = 0;
						}
						//s_dBHEJSyncTime += CalculateEjQueryTime((lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej) - m_lSyncZOffset, (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej) - m_lSyncZOffset);
						s_dBHEJSyncTime += lTime_Ej;
						if( szMsg!="" && SPECIAL_DEBUG_LOG_BH )
							DisplaySequence(szMsg);
						//Ej_Sync();	//v4.54A3
					}
					else
					{
						TakeTime(EJ1);	
					}
				}

				s_dbStartZ2EjUp = GetTime();
				s_dTotalTime_Ej = lTime_Ej;
				if ( bEjtToUp )		//v4.15T1
				{
					CString szMsg;
					szMsg.Format("BH - EJ up final to %d", lEjectorLvl + lCompen_Ej);
					DisplaySequence(szMsg);
					Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
					m_bComplete_Ej = FALSE;

					OpUpdateMS100EjectorCounter(2);	
					OpUpdateMS100ColletCounter();
				}
TakeTime(LC2);

				if ( m_bComplete_Ej == FALSE )
				{
					BOOL bSyncEj = FALSE;
					if ( m_bEnableSyncMotionViaEjCmd && (CMS896AApp::m_lCycleSpeedMode >= 4) )
					{
						if ((m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0))
						{
							//v4.46T2
							//Because Ej motion time is not the same for each cycle due to KOffset,
							//So need to wait for motion completion instead of using Sleep();
							//LONG lEjKTime = CalculateEjTime(lEjectorLvl + lCompen_Ej) + lEnableSyncMotionViaEjCmdExtraTime;
							LONG lEjKTime = CalculateEjQueryTime((lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej) , (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej));

							//Wait a delay for sysnc with both bondhead Z and Ejector
							WaitForDelay(lEjKTime, s_dbStartZ2EjUp);
							s_dTotalTime_Ej = lEjKTime;
						}
						else
						{
							//Wait a delay for sysnc with both bondhead Z and Ejector
							WaitForDelay(lTime_Ej, s_dbStartZ2EjUp);
						}
					}
					else
					{
						bSyncEj = TRUE;
						//Ej_Sync();
					}
					//TakeTime(EJ2);
					m_bComplete_Ej = TRUE;

					if (bSyncEj)
					{
						Ej_Sync();
					}
					TakeTime(EJ2);
				}

				//Ejector Cap Start to move after Ej Pin arrive at Up level
				if (!m_bDBHStop && m_bMS60EjElevator && (m_lDnOffset_EJ_Cap > 0))
				{
					//EjElevator_MoveTo(m_lStandby_EJ_Cap - m_lDnOffset_EJ_Cap, SFM_NOWAIT);
					//s_dEjectorCapStartTime = GetTime();
				}

				if (IsBHZ1ToPick())		//If BHZ1 at PICK		
				{
					if (m_bComplete_Z2 == FALSE)
					{
//						Z2_Sync();
						//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
						//WaitForDelay(m_lBondTime_Z2, s_dBondZDownStart);
						m_bComplete_Z2 = TRUE;
						TakeTime(Z8);
					}
				}
				else
				{
					if (m_bComplete_Z == FALSE)
					{
//						Z_Sync();
						//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
						//WaitForDelay(m_lBondTime_Z, s_dBondZDownStart);
						m_bComplete_Z = TRUE;
						TakeTime(Z4);
					}
				}

				if (s_dTotalTime_Ej < s_dBHEJSyncTime)
				{
					s_dTotalTime_Ej = s_dBHEJSyncTime;
				}

				//v4.59A41	//re-position to AFTER Z motion complete
				//Collet-Hole shift test for dual arm
				//Matt test begin
				//v4.59A30
				OpUpdateColletHoleShiftTestCounters();

				m_qSubOperation = WAIT_PICK_DELAY_Q;	// -> normal flow		//v4.08
			}
			break;

		case WAIT_PICK_DELAY_Q:
			DisplaySequence("Mega Da BH - WAIT_PICK_DELAY_Q");

			//Syn BH at BOND side
			if (IsBHZ1ToPick())			//If BHZ1 at PICK	
			{
				if (s_bDownToPick)
				{
					m_bBHZ1HasDie = TRUE;
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ1 Pick Die");
				}
				if (m_bComplete_Z2 == FALSE)
				{
//					Z2_Sync();
					//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
					//WaitForDelay(m_lBondTime_Z2, s_dBondZDownStart);
					TakeTime(Z8);
				}
				SetPickVacuumZ2(FALSE);				//Turn off BHZ2 vac on BOND side
				CMSLogFileUtility::Instance()->BT_TableIndexLog("BH2 at BOND");

				if ( (m_bNeutralVacState && !m_bFirstCycle && (m_lNeutralDelay > 0)) || m_bNGPick )
				{
					SetStrongBlowZ2(TRUE);    //NEUTRAL state
				}
			}
			else
			{
				if( !m_bFirstCycle && s_bDownToPick )
				{
					m_bBHZ2HasDie = TRUE;
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ2 Pick Die");
				}
				if (m_bComplete_Z == FALSE)
				{
//					Z_Sync();
					//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
					//WaitForDelay(m_lBondTime_Z, s_dBondZDownStart);
					TakeTime(Z4);
				}
				SetPickVacuum(FALSE);				//Else Turn off BHZ1 Vac on BOND side
				CMSLogFileUtility::Instance()->BT_TableIndexLog("BH1 at BOND");

				if ( (m_bNeutralVacState && (m_lNeutralDelay > 0)) || m_bNGPick )
				{
					SetStrongBlow(TRUE);    //NEUTRAL state
				}		
			}



			//v4.34T10		//Chck CP2 completion before Bh to continue next cycle
			if (s_bDownToPick && !m_bStop && !m_bIsWaferEnded)	
			{
				if (!WaitCompDone(1000))
				{
					break;
				}
				SetCompDone(FALSE);
			}

			if (/*!m_bFirstCycle && */!m_bChangeGrade)
			{
				DisplaySequence("SetBTStartMoveWithoutDelay3==>FALSE");
				SetWTStartMove(FALSE);
				SetBTStartMoveWithoutDelay(FALSE);
				SetBTStartMove(FALSE);

				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;

				if ( (m_nBHAlarmCode != -1)	&& m_lBHStateCode!=-1 )	//If not MD-RETRY, index BT by 1	
				{
					//	V450X16	at bond, update bond grade and check BT block
					m_lAtBondDieGrade	= m_ucAtPickDieGrade;
					m_lAtBondDieRow		= m_ulAtPickDieRow;
					m_lAtBondDieCol		= m_ulAtPickDieCol;
					LONG lBTBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];	//	["BinTable"]["LastBlkInUse"]
					LONG lBTGrade		= (*m_psmfSRam)["BinTable"]["BondingGrade"];	//	wrong sort bug fix
					szMsg.Format("BH - map (%ld,%ld)%ld BHZ%d At BOND, Bin grade %ld, Blk %ld", m_lAtBondDieRow, m_lAtBondDieCol, m_lAtBondDieGrade,
							IsBHZ1ToPick()+1, lBTGrade, lBTBlkInUse);
					if( IsAutoSampleSort() )
					{
						GetEncoderValue();
						szTemp.Format(", T %ld(%ld), Z1 %d, Z2 %d, EJ %ld",
								m_lEnc_T, m_lCurPos_T, m_lEnc_Z, m_lEnc_Z2, m_lEnc_Ej);
						szMsg = szMsg + szTemp;
					}
					DisplaySequence(szMsg);
					if(pAppMod->GetCustomerName() != "Primaopto")  //4.51D12 
					{
						if( IsBurnIn()==FALSE && SPECIAL_DEBUG_LOG_WS && (m_lAtBondDieGrade > 0) && (lBTGrade > 0) && (m_lAtBondDieGrade != lBTGrade) )
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
			else	//v4.31T12
			{
				//FOr Yealy MS100Plus Single Loader UNLOAD buffer sequence only
				if (!CMS896AStn::m_bWLReadyToUnloadBufferFrame)
				{
					CMS896AStn::m_bWLReadyToUnloadBufferFrame = TRUE; 
				}	
			}

			if (m_bPreBondAtPick || m_bPostBondAtBond)		//v4.48A8
			{					
			}
			else
			{
				if (m_bOnVacuumAtPick)	//	at pick delay
				{
					if( m_lTCDnOnVacDelay>0 )
						Sleep(m_lTCDnOnVacDelay);

					TakeTime(PV1);	
					if (IsBHZ1ToPick())			//If BHZ1 towards PICK
					{
						DisplaySequence("BH - on BHZ1 vacuum just before pick delay");
						SetPickVacuum(TRUE); 
					}	
					else
					{
						DisplaySequence("BH - on BHZ2 vacuum just before pick delay");
						SetPickVacuumZ2(TRUE);
					}
				}	//	turn on bond head vaccum at pick level
			}

			//Wait for weak blow delay
			if (m_bNeutralVacState && !m_bFirstCycle && (m_lNeutralDelay > 0))
			{
				if (m_lBondDelay > m_lNeutralDelay - COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL)
				{
					WaitTurnOffStrongBlow(m_lNeutralDelay - COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL, s_dBondZDownStart);
					//Wait for Bond delay
					WaitForBondDelay(s_dBondZDownStart);
				}
				else
				{
					//Wait for Bond delay
					WaitForBondDelay(s_dBondZDownStart);
					WaitTurnOffStrongBlow(m_lNeutralDelay - COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL, s_dBondZDownStart);
				}
			}
			else
			{
				//Wait for Bond delay
				//WaitForBondDelay(s_dBondZDownStart);
				//TakeTime(LC9);
			}



			//Wait for pick delay
			//WaitForPickDelay(s_dTotalTime_Ej, lEjectorUpDelay, m_lPickDelay, s_dbStartZ2PK);
			//TakeTime(LC10);

			if (m_bMS60EjElevator)
			{
				//EjElevator_MoveTo(m_lStandby_EJ_Cap, SFM_NOWAIT);
			}

			m_bBhInBondState	= FALSE;	
			//m_bIs2DCodeDone		= FALSE;			//v4.40T1
			m_bDetectDAirFlowAtPrePick = FALSE;

			//v4.55A8
			m_bMoveFromPrePick = FALSE;


			//MD response time measurement option in SERVICE page
			if (m_bEnableMDResponseChecking)	//v3.94
			{
				OpCheckMissingDieResponseTime();
			}

			////Collet-Hole shift test for dual arm
			//Update Ejector & Collet counter
			bWaferEnd	= IsWaferEnded();
			if (s_bDownToPick && (IsMS90() ||!bWaferEnd))		//v4.43T12
			{
				OpUpdateMachineCounters();
			}

			if( !m_bFirstCycle )
			{
				if (IsBHZ1ToPick())
				{
					m_bBHZ2HasDie = FALSE;	//
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ2 Bond Die");
				}
				else
				{
					m_bBHZ1HasDie = FALSE;	//
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ1 Bond Die");
				}
			}	//	for die left
			
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

					CMSLogFileUtility::Instance()->WT_GetCTLog_CT_NoUpdate(m_dCycleTime);		//v4.06		//MS100+ CT study
				
					// 3501
					SetGemValue("AB_CurrentCycleTime", m_dCycleTime);
					SetGemValue("AB_AverageCycleTime", m_dAvgCycleTime);
					SetGemValue("AB_CycleCounter", m_ulCycleCount);
					// 7704
					// SendEvent(SG_CEID_AB_RUNTIME, FALSE);
				}
				m_dLastTime = dCurrentTime;

				if ( (m_nBHAlarmCode != -1) && m_lBHStateCode!=-1 )		//If not MD-RETRY	//v4.43T12
				{
					OpUpdateBondedUnit(m_dCycleTime);		//Update bonded unit
				}
				OpUpdateDieIndex();				// Update the die index
			}

			//	V450X16	store pick grade
			m_bSetEjectReadyDone = FALSE;
			if (s_bDownToPick)
			{
				//Record wafer map coordinate&Bin on bondhead
				m_ulAtPickDieRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
				m_ulAtPickDieCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
				m_ucAtPickDieGrade	= (UCHAR)(LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"] - m_WaferMapWrapper.GetGradeOffset();
				szMsg.Format("BH - map (%ld,%ld)%ld BHZ%d At PICK",
					m_ulAtPickDieRow, m_ulAtPickDieCol, m_ucAtPickDieGrade, IsBHZ2ToPick()+1);
			}
			if( IsAutoSampleSort() )
			{
				GetEncoderValue();
				szTemp.Format(", T %ld(%ld), Z1 %d, Z2 %d, EJ %ld",
						m_lEnc_T, m_lCurPos_T, m_lEnc_Z, m_lEnc_Z2, m_lEnc_Ej);
				szMsg = szMsg + szTemp;
			}
			if (s_bDownToPick)
			{
				DisplaySequence(szMsg);
			}
			//	V450X16	store pick grade

			OpCheckPostBondEmptyCountToIncBHZ();	//v4.48A4
			(*m_psmfSRam)["BondHead"]["First Pick Die"] = TRUE;		//v4.54A5

			//m_qSubOperation = MOVE_Z_PICK_UP_Q;
			m_qSubOperation = EJ_DOWN_Q;//MOVE_EJ_THEN_T_Q;
			break;

		//case MOVE_EJ_THEN_T_Q:
		case EJ_DOWN_Q:	//Move to Standby Level
			DisplaySequence("Mega Da BH - EJ_DOWN_Q");
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

				TakeTime(CycleTime);
				NextBondRecord();
				CString szTemp;
				szTemp.Format("MegaDa Cycle #%d", GetRecordNumber());
				DisplaySequence(szTemp);
				StartTime(GetRecordNumber());	// Restart the time

				// Move Ej
				if (lEjectorDownDelay > 0)
				{
					Sleep(lEjectorDownDelay);		//Linear Ej
				}		
				if (lEjectorDownDelay >= 0)
				{
					TakeTime(EJ3);	
					Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
					szMsg.Format("BH - in EJ THEN T, EJT down to standby %d", m_lStandbyLevel_Ej);
					DisplaySequence(szMsg);

					m_dEjDnStartTime = GetTime();
					m_bComplete_Ej = FALSE;
				}

				//** Special Speed-up mode for MS100 100ms testing **//
				bLFDie	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];
				bSwitchPR = m_bPrAoiSwitchCamera;
				if ((lArmPicBondDelay >= lDnTime_Ej) && !bSwitchPR/*m_bPrAoiSwitchCamera*/)	//v4.05		// || !bLFDie)	//v3.94
				{
					//v4.59A42
					if ( (lDnTime_Ej > 0) && (lDnTime_Ej < 10))
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
					}

					TakeTime(EJ4);
					m_bComplete_Ej = TRUE;
					DisplaySequence("BH - TakeTimeEj4, PickBondDelay > EJDownTime & not switch pr");
					//v4.15T2	//Correct bug with big ArmPick delay values to resolve camera blocking problem
					LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"] + m_lPRDelay;
					LONG lBHTMotion		= (LONG)(lArmPicBondDelay - lDnTime_Ej + (1.0 / 3.0 * m_lBondTime_T));
					LONG lExtraWTDelay	= 0;

					if (lBHTMotion > lWTIndexTime)
					{
						lExtraWTDelay = lBHTMotion - lWTIndexTime;
						if (lExtraWTDelay > 0)
						{
							Sleep(lExtraWTDelay);
						}
					}
				}

				m_qSubOperation = MOVE_T_Q;
			}
			break;

		case MOVE_T_Q:		//v2.99T1
			DisplaySequence("Mega Da BH - MOVE_T_Q");
			
			//Trigger EJ ready to let table move==2019.1.31 after search collet hole======
			if (s_bDownToPick)
			{
				if ((lArmPicBondDelay >= lDnTime_Ej) && !bSwitchPR/*m_bPrAoiSwitchCamera*/)
				{
					if (!IsReachACCMaxCount())
					{
						m_bSetEjectReadyDone = TRUE;
						SetEjectorReady(TRUE, "Move EJ then T");	//Let WT to INDEX
						TakeTime(LC1);
					}
				}
			}

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

					//TO resolve postbond error hangup problem	//TW
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						m_bComplete_Ej = TRUE;
					}

					Sleep(100);	
					T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}
		
			if (m_bDisableBT)
			{
				m_lSPCResult = 1;
			}

			//v4.51A16	//SanAn
			if (m_lSPCResult != 1)
			{
				OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "3");	//v4.52A7	//Sanan & Semitek
			}

			if (bBHMarkStop || (m_lSPCResult != 1) && (m_nBHAlarmCode != -4))
			{
				CString szLog;
				szLog.Format("BH: bPostBondNextCycleStop SET - %ld, %d, %lu", 
					m_lSPCResult, m_bSPCAlarmTwice, m_ulCycleCount);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

				//Need to stop at "Next Cycle" instead because curr-die already picked at this moment!
				m_lPrevDieSPCResult = m_lSPCResult;	//v4.24T7
				bPostBondNextCycleStop = TRUE;
			}
			else
			{
				bPostBondNextCycleStop = FALSE;
			}

			if ((m_nBHAlarmCode == -4) ||	//postbond already triggered in "last" cycle	//v4.21T8
				(m_nBHAlarmCode == 6))		//postbond triggerd before BH down to PICK		//v4.21T8	
			{
				//TO resolve postbond error hangup problem	//TW
				if (m_bComplete_Ej == FALSE)
				{
					Ej_Sync();
					TakeTime(EJ4);
					m_bComplete_Ej = TRUE;
				}

				//Sleep(100);
				Sleep(100);		//Extra delay to avoid BH lose-power problem due to ILC		//v4.22T4
				T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
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
			else if (1)	//AllowMove() == TRUE)	//v4.47T7
			{
				BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];	//v4.47T5
				if (bMS60LFCycle)	//v4.47T5
				{
					(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;		//v4.10T1
				}
				else
				{
					if (!m_bDBHStop && !bWaferEnd && !m_bStop && !WaitWTStartMove(0) && !IsReachACCMaxCount())
					{
						if ((CMS896AApp::m_lCycleSpeedMode >= 3))		//&& bLFDie)	//v3.89		//v3.94	
						{
							//MS100 BH-T speedup for smaller Arm-PICK delay, if current dei is LF and no-compensate needed
							//No need to wait for WT start-move event
							(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;		//v4.10T1
						}
						else
						{
							Sleep(1);
							break;
						}
					}
				}

				SetWTStartMove(FALSE);

				if (IsZMotorsEnable() == FALSE)
				{
					SetMotionCE(TRUE, "IsZMotorsEnable fail in MOVE_T_Q");	//v4.59A19
					SetErrorMessage("BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (!OpIsBinTableLevelAtDnLevel())		//v4.44T1	//Semitek
				{
					SetMotionCE(TRUE, "BT level at UP detected in MOVE_T_Q");	//v4.59A19
					SetErrorMessage("BH: BT platform at UP level in MOVE_T_Q !!");
					SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (!OpAlarmCodeHandler())
				{
					//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
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
					OpSetMS100AlarmCode(-4);		//postbond NextCycle stop	//v4.21T8	
				}

				if (IsReachACCMaxTimeOut())
				{
					if( m_ulCleanCount < m_ulMaxCleanCount )
					{
						m_ulCleanCount = m_ulMaxCleanCount;
					}
				}
				if (m_bBPRErrorToCleanCollet)
				{
					if( m_ulCleanCount < m_ulMaxCleanCount )
					{
						m_ulCleanCount = m_ulMaxCleanCount;
					}
				}

				INT nColletCleanStatus = 0;		// rescan block camera
				if ((GetWftSampleState() == 0) && (GetWftCleanEjPinState() == 0))	//	block camera when sampling
				{
					nColletCleanStatus = IsCleanCollet();	//	return 0,1,2
				}
				m_bBPRErrorToCleanCollet = FALSE;

				if (nColletCleanStatus != 0)	// do auto clean collet
				{
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						m_bComplete_Ej = TRUE;
					}

					if( m_bSetEjectReadyDone==FALSE )
					{
						m_bSetEjectReadyDone = TRUE;
						SetEjectorReady(TRUE, "Move T Clean Collet != 0");
					}
					TakeTime(LC1);	//v4.42T6
					//v4.22T4		//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();		//Must wait T complete for ILC
					Sleep(10);	
					CheckCoverOpenInAuto("CC2");
					T_MoveTo(m_lPrePickPos_T);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

					if (nColletCleanStatus == 2)		//Cleaned
					{
						if (pAppMod->GetCustomerName() == "Testar")
						{
							SetErrorMessage("BH: Clean Collet is reset & continue");		//v4.25
						}

						m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
						if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
						{
							SetBhTReady(TRUE, "1");
							SetBhTReadyForWT(TRUE, "1");		//Let WT to perform WPR	//MS60	//v4.46T22
						}
						m_bFirstCycle = FALSE;	//v3.67
						m_bChangeGrade	= FALSE;
						m_bBhInBondState = FALSE;
						m_qSubOperation = WAIT_WT_Q;
						DisplaySequence("BH - Clean Status 2 to Wait WT");
						break;
					}
					else if (nColletCleanStatus == 1)	//ABORT
					{
						SetErrorMessage("BH: CleanCollet is aborted");		//v4.25
						DisplaySequence("BH - |clean collet aborted| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				if (IsEnableILC() == TRUE)
				{
					ILC_AutoUpdateApplyingCycleCountInc();
				}

				//v4.15T8
				BOOL bEnableOsramBinMixMap = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Enable Osram Bin Mix Map"];
				if ((m_nBHLastAlarmCode == -3) && bEnableOsramBinMixMap == TRUE)
				{
					T_MoveTo(m_lPrePickPos_T);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if ((m_nBHLastAlarmCode == -2) ||	//If curr-pick-die is ChangeGrade NoPickDie
					(m_nBHLastAlarmCode == -3) )	//BIN-FULL //v4.43T10
				{
					//v4.62A12
					//** BIN-FULL (-3) only applicable in this loop for CREE only, when 
					//** 	NextBinFull fcn is enabled;

					//v4.22T4		//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();		//Must wait T complete for ILC
					Sleep(10);	

					CString szLog;
					if (IsBHZ1ToPick())	//If BH! towards PICK
						szLog = "BH: ChangeGrade; last die bonded by BH2"; 
					else
						szLog = "BH: ChangeGrade; last die bonded by BH1"; 

					T_MoveTo(m_lPrePickPos_T);

					if (!m_bFirstCycle && !m_bChangeGrade)	
					{
						SetBhTReady(TRUE, "2");
						SetBhTReadyForWT(TRUE, "2");		//Let WT to perform WPR	//MS60	//v4.46T22
					}

					if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
					{
						m_bChangeGrade = TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
						szLog.Format("BH: EjtXY ChangeGrade 3; BH2 = %d", IsBHZ1ToPick()); 
						CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
						szLog.Format("BH - BH last alarm code %d with EjtXY ==> Prepick, -2=next die change grade; -3=next die bin full",
							m_nBHLastAlarmCode);

						//v4.62A12		//MS90 in general
						//If Last Grade last die is bonded by BH1, then Next Grade
						// 1st die will be picked & bonded by BH2; however, BT moves earlier after 
						// ChangeBin, so it uses wrong INDEX BH1.  Here, we reverse the BH sequence
						// for BT only, so it moves to correct BH offset (BH2 in this case)
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) !m_bBHZ2TowardsPick;

					}
					else
					{
						CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 3");	//v4.47T10
						DisplaySequence("SetBTStartMoveWithoutDelay4==>FALSE");
						SetBTStartMoveWithoutDelay(FALSE);
						SetBTStartMove(FALSE);	// suggest by Andrew, HuaMao green light idle
						m_bFirstCycle		= TRUE;		
						m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
						szLog.Format("BH - BH last alarm code %d ==> Prepick, -2=next die change grade; -3=next die bin full",
							m_nBHLastAlarmCode);
					}

					DisplaySequence(szLog);
					//For BT to move to correct BH1/2 offset XY, in this case 1st die to bond must be BH1
					m_nBHAlarmCode		= 0;			
					m_nBHLastAlarmCode	= 0;	
					m_lBHStateCode		= 0;

					DisplaySequence("SetBTStartMoveWithoutDelay5==>FALSE");
					SetBTStartMoveWithoutDelay(FALSE);
					SetBTStartMove(FALSE);

					EnableBHTThermalControl(TRUE, TRUE);	//v4.49A5
					//SetBondHeadFan(FALSE);				//v4.54A7

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					Sleep(500);
					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					break;
				}	//	change grade or bin full

				//v4.54A5
				bMS60NGPickNoLFCycle = FALSE;
				if (CMS896AStn::m_bMS60NGPick && !bLFDie)
				{
					//In NGPick Cycle, if no LF, then BH must move to PREPICK for 
					// WaferPR station to perform 2 PRs, 1 for EMPTY pos and one for next-Die;
					bMS60NGPickNoLFCycle = TRUE;
				}

				BOOL bPickDie = (BOOL)((LONG)(*m_psmfSRam)["WaferTable"]["PickDie"]);
				if (!bPickDie || bMS60NGPickNoLFCycle)
				{
					DisplaySequence("BH - T move, to prepick, Unpick die to Wait WT");
					//	CheckCoverOpenInAuto("Unpick to prepick");
					// The die is only for alignment, move BH to PrePick
					SetWTReady(FALSE, "SetWTReady FALSE !bPickDie || bMS60NGPickNoLFCycle");		//v3.80
					//v4.22T4				//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();				//Must wait T complete for ILC
					Sleep(10);	

					T_MoveTo(m_lPrePickPos_T, SFM_NOWAIT);		//v4.54A10	//MS60 NGPick
					Sleep(10);

					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					m_bChangeGrade	= FALSE;	//	wrong sort bug fix
					m_bFirstCycle	= FALSE;	//v3.67
					m_bBhInBondState = FALSE;
					m_qSubOperation	= WAIT_WT_Q;

					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						SetWTReady(FALSE, "SetWTReady FALSE m_bComplete_Ej = FALSE 1");				//REset WT flag again before letting it to move
						m_bComplete_Ej = TRUE;
						if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE )
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move T WT Die not pick, let ej ready");    //Let WT to INDEX
							TakeTime(LC1);	//v4.42T6
						}			
					}

					if (!m_bFirstCycle && !m_bChangeGrade)
					{
						SetBhTReady(TRUE, "3");
						SetBhTReadyForWT(TRUE, "3");		//Let WT to perform WPR	//MS60	//v4.46T22
					}

					break;
				}

				if (bSwitchPR/*m_bPrAoiSwitchCamera*/	|| 
					IsCheckPr2DCode()		|| 
					IsEjtUpAndPreheat()		||
					bEnableBHUplookPr		||						//v4.52A16
					IsMaxColletEjector_CheckOnly(bWaferEnd) )		//v4.50A29
				{
					szMsg.Format("BH - DBH move T q, maybe PR switch camera. 3,switchpr%d,2DCode,%d,EjtUpPreheat,%d,UpLook,%d,CheckCollet,%d,CompleteEj,%d,Pick,%d,Ejready,%d",
					bSwitchPR,IsCheckPr2DCode(),IsEjtUpAndPreheat(),bEnableBHUplookPr,IsMaxColletEjector_CheckOnly(bWaferEnd),m_bComplete_Ej,OpCheckAlarmCodeOKToPick(TRUE),m_bSetEjectReadyDone);
					DisplaySequence(szMsg);
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);

						//2019.3.28 remove SetWTReady becasue it will lead into confuse
//						SetWTReady(FALSE, "SetWTReady FALSE (m_bComplete_Ej == FALSE) 3");				//REset WT flag again before letting it to move
						m_bComplete_Ej = TRUE;
						if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE)		//v4.15T1
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move T Pr2DCode or EjPreheat, let ej ready");    //Let WT to INDEX
							TakeTime(LC1);	//v4.42T6
						}			
					}

					//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();	//Must wait T complete for ILC
					Sleep(10);	

					T_MoveTo(m_lPrePickPos_T, SFM_NOWAIT);

					//v4.57A13
					if (bEnableBHUplookPr)
					{
						TakeTime(T1);
					}

					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					if (bSwitchPR/*m_bPrAoiSwitchCamera*/)
					{
						DOUBLE dTime = GetTime();
						DisplaySequence("BH - DBH move T q, T to prepick, PR switch camera. 4");
						SwitchPrAOICamera();
						CString szMsg;
						szMsg.Format("BH - DBH move T q, PR switch camera. 5 used time %d", LONG(GetTime() - dTime));
						DisplaySequence(szMsg);
					}

					if (IsEjtUpAndPreheat())	//v4.47T8
					{
						Sleep(30);
					}
					else if (bEnableBHUplookPr)	//v4.57A13
					{
						Sleep(10);				
					}
					else
					{
						T_Sync();
					}

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
					{
						SetBhTReady(TRUE, "4");
						SetBhTReadyForWT(TRUE, "4");		//Let WT to perform WPR	//MS60	//v4.46T22
					}
				
					m_bFirstCycle	= FALSE;	//v3.67
					m_bChangeGrade	= FALSE;	
					m_bBhInBondState = FALSE;
					if (IsCheckPr2DCode())
					{
						m_bIs2DCodeDone	= FALSE;	//v4.52A16
					}
					if (bEnableBHUplookPr)
					{
						m_lUpLookPrStage	= 1;	//v4.52A16
					}

					if (((m_bStop && !m_bBHZ1HasDie && !m_bBHZ2HasDie) || IsSortTo2ndPart() || m_b2PartsAllDone))	//andrewng
					{
						DisplaySequence("BH - MS90 Uplook PR Do 1More Cycle ==> Wait WT");
						m_bDBHStop = TRUE;
					}
					
					DisplaySequence("BH - AOI PR switch ==> Wait WT");
					m_qSubOperation = WAIT_WT_Q;
					break;
				}

				if ((m_bStop && !m_bBHZ1HasDie && !m_bBHZ2HasDie))
				{
					DisplaySequence("BH - MS90 Do 1More Cycle ==> Wait WT NO UPLOOK");
					m_bDBHStop = TRUE;
				}

				//v4.43T8
				bWTNeedRotate	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LookForward"]["NeedRotate"];
				LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"];

				//if (lWTIndexTime > BH_WT_MAX_MOTION_TIME || m_bReadyToRealignOnWft || (GetWftSampleState() >= 1) || (GetWftCleanEjPinState() >= 1) || bWTNeedRotate)
				//{
				//	Ej_Sync();
				//	TakeTime(EJ4);
				//	if (m_bReadyToRealignOnWft || (GetWftSampleState() >= 1) || (GetWftCleanEjPinState() >= 1) || bWTNeedRotate)
				//	{
				//		SetWTReady(FALSE, "m_bReadyToRealignOnWft || (GetWftSampleState() >= 1) || (GetWftCleanEjPinState() >= 1) || bWTNeedRotate");	//REset WT flag again before letting it to move
				//	}
				//	m_bComplete_Ej = TRUE;

				//	if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE )	//	V450X16
				//	{
				//		//	should check it is set already after pick die
				//		m_bSetEjectReadyDone = TRUE;
				//		SetEjectorReady(TRUE, "Move T WTIndexTime>80 or WTNeedRotate");    //Let WT to INDEX
				//		TakeTime(LC1);	//v4.42T6
				//	}				

				//	//v4.22T4				//Extra delay to avoid BH lose-power problem due to ILC		
				//	T_Sync();				//Must wait T complete for ILC
				//	Sleep(10);	

				//	//v4.35T1
				//	if (m_bComplete_Z == FALSE)
				//	{
				//		if (m_lPickUpTime_Z2 > m_lPickUpTime_Z)
				//		{
				//			Z_Sync();
				//			TakeTime(Z2);
				//			Z2_Sync();
				//			TakeTime(Z6);
				//		}
				//		else
				//		{
				//			Z2_Sync();
				//			TakeTime(Z6);
				//			Z_Sync();
				//			TakeTime(Z2);
				//		}
				//	}				
				//
				//	T_MoveTo(m_lPrePickPos_T);

				//	m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
				//	(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
				//	(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				//	if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
				//	{
				//		SetBhTReady(TRUE);
				//		SetBhTReadyForWT(TRUE, "5");		//Let WT to perform WPR	//MS60	//v4.46T22
				//	}

				//	CString szMsg;
				//	szMsg.Format("BH - DBH move to prepick because WTIndexTime %ld, RealignOnWft %d, Sample On Wft %d, %d, WT Rotate %d",
				//		lWTIndexTime, m_bReadyToRealignOnWft, GetWftSampleState(), GetWftCleanEjPinState(), bWTNeedRotate);
				//	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				//	DisplaySequence(szMsg);

				//	m_bFirstCycle	= FALSE;	//v3.67
				//	m_bChangeGrade	= FALSE;
				//	m_bBhInBondState = FALSE;
				//	m_bReadyToRealignOnWft = FALSE;
				//	if (GetWftSampleState() == 1) 
				//	{
				//		szMsg = "BH - move T to sample adv or rescan set 2";
				//		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				//		DisplaySequence(szMsg);
				//		m_lReadyToSampleOnWft = 2;
				//	}
				//	else if (GetWftCleanEjPinState() == 1)
				//	{
				//		szMsg = "BH - move T to Clean Ej Pin set 2";
				//		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				//		DisplaySequence(szMsg);
				//		m_lReadyToCleanEjPinOnWft = 2;
				//	}
				//	m_qSubOperation = WAIT_WT_Q;
				//	break;
				//}

				//v3.94	
				//No need to count WT duration because WT not yet start move not yet called);
				//Only enter this loop if EJ motion not yet completed -> Arm-PICK/BOND delay < EJ motion time (e.g. 6ms < 8ms)
				if (m_bComplete_Ej == FALSE)
				{
					lWTIndexDuration	= 0;
				
					//v4.37T6	//SpeedMode=5 70ms testing
					if (CMS896AApp::m_lCycleSpeedMode >= 5)		//v4.47T5
					{
						LONG lArmDelay = lArmBondDelay;
						if (lArmPickDelay > lArmBondDelay)
						{
							lArmDelay = lArmPickDelay;
						}
						LONG lEjDnDelay = lEjectorDownDelay;
						if (lEjectorDownDelay < 0)
						{
							lEjDnDelay = 0;
						}
						lEjDnDuration	= lDnTime_Ej - lArmDelay + lEjDnDelay;
					}
					else
					{
						lEjDnDuration	= (LONG)(lDnTime_Ej - (GetTime() - m_dEjDnStartTime));
					}

					if (lEjDnDuration < 0)
					{
						lEjDnDuration = 0;
					}
				}
				else
				{
					dWTIndexStart		= (DOUBLE)(*m_psmfSRam)["WaferTable"]["IndexStart"];
					dWTIndexDuration	= GetTime() - dWTIndexStart;
					lWTIndexDuration	= (LONG) dWTIndexDuration;
					if (dWTIndexStart == 0)
						lWTIndexDuration = 0;		//v4.41T4
					if (lWTIndexDuration > lWTIndexTime)
						lWTIndexDuration = 0;		//v4.41T4
					lEjDnDuration		= 0;
					(*m_psmfSRam)["WaferTable"]["IndexStart"] = 0;			//v4.41T4
				}
			
				lTMoveDelay	= 0;

				//v4.41T4
				//Currently set to zero due to camera blocking bug found in MS100;
				lWTIndexDuration = 0;	

				//v4.46T12	//SeoulSemi
				lEjVacOffDelay = 0;
				if (m_lEjVacOffDelay > 0)
				{
					lEjVacOffDelay = m_lEjVacOffDelay + 1;
				}
				
				if (!bMS60LFCycle)
				{
					CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
					ULONG ulDelay = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_DELAY_BEFORE_TURN_OFF_EJ_VAC);
					if ((ulDelay > 0) && (ulDelay < 200))
					{
						lEjVacOffDelay += ulDelay;
					}
				}

				//v4.52A1
				if (CMS896AApp::m_lCycleSpeedMode >= 6)			//v4.48A22
				{
					//Add extra 2ms to calculate camera blocking for 40ms profile
					lTMoveDelay = lEjDnDuration + lWTIndexTime - lWTIndexDuration + m_lPRDelay + lEjVacOffDelay + 2;	
				
					//v4.49A3	//Extra PR delay due to longer exposure time for WPR normal die
					if (bPickDie && (lExtraPrGrabDelay > 0))
					{
						lTMoveDelay = lTMoveDelay + lExtraPrGrabDelay;
					}
				}
				else if (CMS896AApp::m_lCycleSpeedMode >= 3)			//v3.93T1
				{
					lTMoveDelay = lEjDnDuration + lWTIndexTime - lWTIndexDuration + m_lPRDelay + lEjVacOffDelay;
				}
				else
				{
					lTMoveDelay = lWTIndexTime - lWTIndexDuration + m_lPRDelay + lEjVacOffDelay + 4;
				}

				lTMaxUnblockTime = (LONG)(2.0 / 3.0 * m_lBondTime_T) + 2;			//v4.24T5	//WanYiMing		
				CString szMsg;

				//v4.10T1	//Add extra PR delay for long-travel motion
				if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS60/MS100P3/MS90		//v4.50A24
				{
					lWTLongJumpExDelay		= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpDelay"];
					lWTLongJumpMotionTime	= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpMotion"];

					if ((lWTLongJumpMotionTime >= 25) && (lWTIndexTime >= lWTLongJumpMotionTime))
					{
						lTMoveDelay = lTMoveDelay + lWTLongJumpExDelay + 1;		
					}
				}
				else if (CMS896AApp::m_lCycleSpeedMode >= 4)	
				{
					lWTLongJumpExDelay		= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpDelay"];
					lWTLongJumpMotionTime	= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpMotion"];

					if ((lWTLongJumpMotionTime >= 30) && (lWTIndexTime >= lWTLongJumpMotionTime))
					{
						lTMoveDelay = lTMoveDelay + lWTLongJumpExDelay + 1;		//v4.46T12	//SeoulSemi
					}
				}

				//v4.54A6
				if (bMS60LFCycle)
				{
					lTMoveDelay = 0;
				}

				if (lTMoveDelay > lTMaxUnblockTime)
				{
					if (CMS896AApp::m_lCycleSpeedMode >= 3)		//v3.93T1
					{
						//dEjDuration = GetTime();
						LONG lTDelay		= lTMoveDelay - lTMaxUnblockTime;
						LONG lEjTime		= (LONG)(GetTime() - m_dEjDnStartTime);
						LONG lEjRemainTime	= lDnTime_Ej - lEjTime;
						LONG lEjExtraMotionCompleteTime = 0;
						if (lEjRemainTime < 0)
						{
							lEjRemainTime = 0;
						}

						if (lTDelay >= lEjRemainTime)
						{
							//Let WT to proceed if still enough time for T to wait
							//Only enter this loop if EJ motion not yet completed -> Arm-PICK/BOND delay < EJ motion time (e.g. 6ms < 8ms)
							if (m_bComplete_Ej == FALSE)
							{
								if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS60 & above only
								{
									if (lEjRemainTime > 0)
										Sleep(lEjRemainTime);
								}
								else
								{
									Ej_Sync();
								}

								TakeTime(EJ4);

								if( m_bSetEjectReadyDone==FALSE )
								{
									m_bSetEjectReadyDone = TRUE;
									SetEjectorReady(TRUE, "Move T CS>=3");			//Let WT to INDEX
								}
								m_bComplete_Ej = TRUE;

								//v4.53A8	//XM SanAn
								lEjExtraMotionCompleteTime = (LONG)(GetTime() - m_dEjDnStartTime);
								if ( lEjExtraMotionCompleteTime > (lDnTime_Ej+1) )
								{
									lEjExtraMotionCompleteTime = lEjExtraMotionCompleteTime - lDnTime_Ej - 1;
								}
								else
								{
									lEjExtraMotionCompleteTime = 0;
								}
							}

							//lEjCompleteTime = m_lTime_Ej - lEjTime;	
							lTDelay = lTDelay - lEjRemainTime + lEjExtraMotionCompleteTime;	//Remaing Delay after waiting for Ej Motion Completion
						}
						else
						{
							//Else no need to wait Ej completion here and let T move first!
						}

						lTMoveDelay = lTDelay;

						if (m_lBlockCameraDelay > 0)
						{
							lTMoveDelay = lTMoveDelay + m_lBlockCameraDelay;	//v4.52A3
						}
					}
					else
					{
						lTMoveDelay = lTMoveDelay - lTMaxUnblockTime + 2;
					}

					if (m_bDBHStop || m_bStop)
					{
						if (lTMoveDelay <= 0)
						{
							lTMoveDelay = 10;
						}
						else
						{
							lTMoveDelay = lTMoveDelay + 10;    //v4.21T3	//75ms cycle testing
						}		
					}

					if (lTMoveDelay > 0)
					{
						//Sleep(lTMoveDelay);
					}
				}
				else
				{
					if (m_bDBHStop || m_bStop)
					{
						Sleep(10);    //v4.21T3	//75ms cycle testing
					}							
				}

				m_ctBHTAutoCycleLastMoveTime = CTime::GetCurrentTime();
				if (IsBHZ1ToPick())		//If BHZ1 currently at PICK 	
				{
					TakeTime(T1);
					nResult = BA_MoveTo(m_lBondPos_T, SFM_NOWAIT);
				}
				else
				{
					TakeTime(T3);
					nResult = BA_MoveTo(m_lPickPos_T, SFM_NOWAIT);
				}

				if (nResult != gnOK)	//v4.47A1
				{
					HmiMessage_Red_Yellow("Bond Arm channel error");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				m_bComplete_T = FALSE;
				m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;	//Toggle BHZ1 & BHZ2
				(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
				m_lTime_T = m_lBondTime_T;		//v3.61
				s_dbStartT2PB = GetTime();			//v2.83T7

				m_qSubOperation = WAIT_Z_EJ_COMPLETE_Q;
			}
			break;

		case WAIT_Z_EJ_COMPLETE_Q:
			DisplaySequence("Mega Da BH - WAIT_Z_EJ_COMPLETE_Q");
			//Re-enabled for MS100 100ms speedup //No need to be disabled
			// Allow WT to move earlier than Ej completed
			if (m_bComplete_Ej == FALSE)
			{
				lDelay = m_lDnTime_Ej - _round(GetTime() - m_dEjDnStartTime);
				if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS60 & above only		//v4.53A23
				{
					if (lDelay > 0)
						Sleep(lDelay);
				}
				else
				{
					Ej_Sync();
				}

				TakeTime(EJ4);				

				//2019.03.27 remove SetWTReady option becasue it will lead into confuse.
//				SetWTReady(FALSE, "SetWTReady FALSE WAIT_Z_EJ_COMPLETE_Q");						//REset WT flag again before letting it to move
				
				if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE )		//v4.15T1
				{
					m_bSetEjectReadyDone = TRUE;
					SetEjectorReady(TRUE, "Wait Z EJ complete");    //Let WT to INDEX
					TakeTime(LC1);	//v4.42T6
				}		

				//Ej_Sync();
				m_bComplete_Ej = TRUE;
			}

		
			//MS60 LF sequence will call this fcn in WT::OpMoveTable to move EJT 
			//together with WT XY;
			if (!m_bWaferPrMS60LFSequence)		//v4.59A39
			{
				if (!OpMoveEjectorTableXY(FALSE))	//v4.42T3
				{
					SetErrorMessage("Ejector XY module is off power");
					SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			if (/*!m_bFirstCycle && */!m_bChangeGrade)	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
			{
				DisplaySequence("BH - let BPR to grab, set BHT ready");
//				TakeTime(LC3);
				SetBhTReady(TRUE, "6");						//Let BPR Grade image
				SetBhTReadyForWT(TRUE, "6");		//Let WT to perform WPR	//MS60
			}

			if( m_bSetEjectReadyDone == FALSE )
			{
				m_bSetEjectReadyDone = TRUE;
				SetEjectorReady(TRUE, "Wait Z EJ Complete at last");
			}

			m_bFirstCycle	= FALSE;
			m_bChangeGrade	= FALSE;
			m_qSubOperation = WAIT_WT_READY_Q;	
			break;

		//**** Currently not used ****//
		case WAIT_BT_CHANGE_GRADE:
			DisplaySequence("Mega Da BH - WAIT_BT_CHANGE_GRADE");
			if (m_bStop)
			{
				DisplaySequence("BH - |Wait BT change grade stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if (WaitBTStartMove())
			{
				//Assum wafer table die is ready
				SetWTReady(FALSE, "FALSE WAIT_BT_CHANGE_GRADE");
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				DisplaySequence("BH - Wait BT ChangeGrade done ==> T Prepick to pick");
CMSLogFileUtility::Instance()->BL_LogStatus("BH - Wait BT ChangeGrade done ==> T Prepick to pick\n");	//v4.48A26	//By Andrew
			}
			else
			{
				Sleep(1);
			}
			break;


		case WAIT_BIN_FULL_Q:
			DisplaySequence("Mega Da BH - WAIT_BIN_FULL_Q");

			// Wait for bin full alert issued by Bin Table Station
			if (m_bStop)
			{
				SetErrorMessage("BH: BIN_FULL -> stop");
				DisplaySequence("BH - |Wait Bin full stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			//v4.57A11
			if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
			{
				BOOL b2DStatus = TRUE;
				b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				m_lUpLookPrStage = 0;	//reset
			}

			if (WaitBTStartMove())
			{
				//Assum wafer table die is ready
				SetWTReady(FALSE, "FALSE WAIT_BIN_FULL_Q");
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
			DisplaySequence("Mega Da BH - POSTBOND_FAIL_Q");
			lSPCResult = m_lPrevDieSPCResult;	//(*m_psmfSRam)["BondPr"]["PostBondResult"];	//v4.24T7
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
				OpDisplayAlarmPage(TRUE);	//v4.53A13
				SetAlert_Red_Yellow(IDS_BH_SPC_XY_LIMIT);		// Set Alarm
				szMsg = _T("Postbond hit XY Limit Count");
				break;

			case BPR_ERR_SPC_XY_LIMIT2:		//v4.57A14
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_XY_LIMIT);		// Set Alarm
				szMsg = _T("Postbond hit XY Limit (BH2) Count");
				break;

			case BPR_ERR_SPC_MAX_ANGLE:
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_MAX_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Max Angle (BH1) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ANGLE2:		//v4.57A14
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_MAX_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Max Angle (BH2) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ACC_ANGLE:		//v4.58A5
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_MAX_ACC_ANGLE);	// Set Alarm
				szMsg = _T("Postbond hit Max ACC Angle (BH1) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ACC_ANGLE2:	//v4.58A5
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
				OpDisplayAlarmPage(TRUE);	//v4.53A13
				szMsg.Format("Postbond hits EMPTY Limit Count.\nBH1: Continuation=%ld(%ld), Accumlation=%ld\nBH2: Continuation=%ld(%ld), Accumlation=%ld\nTotal Accumlation=%ld(%ld)\n", EmptyCounter, GenMaxEmptyAllow, AccEmptyCounter, EmptyCounter2, GenMaxEmptyAllow, AccEmptyCounter2, AccEmptyCounterTotal, GenMaxAccEmptyAllow);
				SetAlert_Msg_Red_Yellow(IDS_BH_SPC_EMPTY, szMsg);
				SetErrorMessage(szMsg);
				break;

			case BPR_ERR_SPC_EMPTY2:		//v4.57A14
				LogPBFailureCaseDumpFile();
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_EMPTY);			// Set Alarm
				szMsg = _T("Postbond hit EMPTY Limit (BH2) Count");
				break;

			case BPR_ERR_SPC_REL_XY_LIMIT:						//v4.26T1	//Semitek	//Relative XY shift check
				SetAlert_Red_Yellow(IDS_BH_SPC_REL_XY_LIMIT);	// Set Alarm
				szMsg = _T("Postbond hit relative-XY shift Limit Count");
				break;

			default:
				SetAlert_Red_Yellow(IDS_BH_SPC_FAIL);			// Set Alarm
				szMsg.Format("Postbond Detect Error; code=%ld", lSPCResult);	//v4.23T2
				break;
			}
			szTemp.Format(" die picked %d", GetNewPickCount());
			SetErrorMessage(szMsg + szTemp);
			DisplaySequence("BH - |post bond fail| ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("Mega Da BH - HOUSE_KEEPING_Q");
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


			if( IsMS90() && (pAppMod->GetCustomerName()=="WolfSpeed" || pAppMod->GetCustomerName()=="Finisar"))
			{
				if( m_bBHZ2HasDie )
				{
					SetPickVacuumZ2(TRUE);
					Sleep(500);
					if( (m_bCheckMissingDie == TRUE) &&	(m_bPickVacuumOnZ2 == TRUE) && (IsMissingDieZ2() == FALSE) )
					{
						HmiMessage_Red_Back("Bond head 2 has die un-bonded", "Auto Sort");
					}
					else
					{
						DisplaySequence("BH - BHZ2 logic die left, but lost");
						SetPickVacuumZ2(FALSE);
						m_bBHZ2HasDie = FALSE;
					}
				}
				else if( m_bBHZ1HasDie )
				{
					SetPickVacuum(TRUE);
					Sleep(500);
					if ((m_bCheckMissingDie == TRUE) && (m_bPickVacuumOn == TRUE) && (IsMissingDie() == FALSE))
					{
						HmiMessage_Red_Back("Bond head 1 has die un-bonded", "Auto Sort");
					}
					else
					{
						DisplaySequence("BH - BHZ1 logic die left, but lost");
						SetPickVacuum(FALSE);
						m_bBHZ1HasDie = FALSE;
					}
				}
			}
			else
			{
				SetPickVacuum(FALSE); // Matthew by Harry Ho
				Sleep(500);
				SetBHStrongBlow(0, TRUE);
				Sleep(500);
				SetPickVacuumZ2(FALSE);
				Sleep(500);
				SetBHStrongBlow(1, TRUE);
				Sleep(500);
							
				m_bBHZ2HasDie = FALSE;
				m_bBHZ1HasDie = FALSE;
			}

			// Make sure the motion is complete first
			// Wait Z complete
			if (m_bComplete_Z == FALSE)
			{
				Z_Sync();
				Z2_Sync();
			}					
			// Wait Ej complete
			if (m_bComplete_Ej == FALSE)
			{
				Ej_Sync();
				m_bComplete_Ej = TRUE;
			}					
			// Wait T complete
			if (m_bComplete_T == FALSE)
			{
				T_Sync();
			}

			//Disable NuMotion data log earlier here	//v4.17T7
			CycleEnableDataLog(FALSE, TRUE, TRUE);

			//SetPickVacuum(FALSE);			// Turn-off the pick vacuum		//v2.77
			if (Z_IsPowerOn())				//v3.13T3
			{
				AC_Z1_MoveTo(0, SFM_WAIT);		// Move Z to Zero
			}		

			if (Z2_IsPowerOn())
			{
				AC_Z2_MoveTo(0, SFM_WAIT);
			}

			SetEjectorVacuum(FALSE);
			Sleep(100);
			Ej_MoveTo(0, SFM_WAIT);			// Move Ej to Zero

			if (m_bMS60EjElevator)	
			{
				if (EjElevator_IsPowerOn())	
				{
					//EjElevator_MoveTo(m_lStandby_EJ_Cap, SFM_WAIT);	

				}
				else
				{
					EjElevator_Home();
				}
			}

			//v3.98T3
			TakeTime(CycleTime);
			StartTime(GetRecordNumber());	// Restart the time	
			//andrew
			if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
			{
				BOOL b2DStatus = TRUE;
				b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				m_lUpLookPrStage = 0; //reset
			}

			//if ((m_ulMaxColletCount > 0) && (m_ulColletCount > 0))	//v4.40T7
			//{
			//	m_ulColletCount = m_ulColletCount - 1;
			//}
			//20171025 Leo Protection of hit hand
			CheckCoverOpenInAuto("During Bonding Cycle");
			
			if (IsBHZ1ToPick())		//If BHZ1 at PICK	
				LogCycleStopState("BH - MoveArm; BH2 to prepick(clean collet) at last cycle");
			else
				LogCycleStopState("BH - MoveArm; BH1 to prepick(clean collet) at last cycle");

			bRegionEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"]; // xuzhijin_region_end
			bWaferEnd	= IsWaferEnded();
			bColletFull = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ColletFull"];

			if (Z_IsPowerOn() && T_IsPowerOn() && Z2_IsPowerOn())			//v3.13T3
			{
				if( IsPrescanning()==FALSE )
					CheckCoverOpenInAuto("House Keeping");
				if ((bWaferEnd == TRUE) || (bColletFull == TRUE) || (bRegionEnd == TRUE))
				{
					T_MoveTo(m_lCleanColletPos_T, SFM_WAIT);	// Move T to Blow position
				}
				else
				{
					T_MoveTo(m_lPrePickPos_T, SFM_WAIT);		// Move T to Pre-Pick
				}
			}

			if (!m_bUplookResultFail)		//v4.59A7	//MS90
			{
				if( IsMS90() && (pAppMod->GetCustomerName()=="WolfSpeed" ||pAppMod->GetCustomerName()=="Finisar" )&& m_bBHZ1HasDie )
				{
				}
				else
					SetPickVacuum(FALSE);		// Turn-off the pick vacuum after T-Move done		//v2.77
				if( IsMS90() && (pAppMod->GetCustomerName()=="WolfSpeed" ||pAppMod->GetCustomerName()=="Finisar" ) && m_bBHZ2HasDie )
				{
				}
				else
					SetPickVacuumZ2(FALSE);		// Turn-off the pick vacuum after T-Move done		//v3.86
			}

			if (bWaferEnd)				//v4.40T2	//Nichia MS100+
			{	
				LogCycleStopState("BH - WaferEnd");	//v4.44A1	//SEmitek
				OpUpdateNichiaRawMaterialRecordsAtWaferEnd();
				OpResetEjKOffsetAtWaferEnd();	//v4.43T8	//Semitek
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

			ILC_SaveAllResult();

		//	CMS896AStn::m_lBondHeadAtSafePos = 1;
			// Acknowledge other stations to stop
			StopCycle("BondHeadStn");
			LogCycleStopState("BH - dbh stop completely");
			State(STOPPING_Q);

			SetHouseKeepingToken("BondHeadStn", TRUE);

			if( IsPrescanning() && IsCoverOpen() )
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
		SetMotionCE(TRUE, szErr);	//v4.59A19
	}
}
