#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "CycleState.h"
#include "math.h"
#include "BondPr.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBondHead::RunOperation_AP()
{
	CString szMsg;
	LONG	lTemp = 0, lDelay = 0, lCompen_Ej = 0, lSPCResult = 0;	//v4.47T12
	LONG	lEjectorLvl = 0;
	LONG	lTime_Ej	= m_lTime_Ej;
	LONG	lDnTime_Ej	= m_lTime_Ej;	//v4.27T3
	ULONG	j = 0;
	BOOL	bPickDie=TRUE, bCriticalError=FALSE;
	BOOL	bWaferEnd, bColletFull, bLFDie;
	BOOL	bRegionEnd = FALSE;
	double	dCurrentTime = 0;
	static	BOOL	s_bUpdateNVRAM = TRUE;
	static	DOUBLE	s_dbStart = 0, s_dbStart2 = 0, s_dbEnd = 0, s_dbSettling = 100;
	LONG lMotionTime_BT = 0;
	LONG lTMoveDelay = 0, lTMaxUnblockTime = 0;
	DOUBLE dWTIndexStart = 0, dWTIndexDuration = 0;
	LONG lEjVacOffDelay = 0;
	LONG lWTIndexDuration	= 0;
	LONG lEjCompleteTime	= 0;				//v4.04
	LONG lEjDnDuration		= 0;				//v4.22T1
	DOUBLE dEjDuration		= 0;				//v4.04
	LONG lWTLongJumpExDelay = 0;
	LONG lWTLongJumpMotionTime = 0;
	BOOL bIsNeedRestartUpdate = FALSE;
	BOOL bPostBondNextCycleStop = FALSE;	//v4.21T8	//Cree HuiZhou
	BOOL bAPContinue = TRUE;
	BOOL bWTNeedRotate = FALSE;
	INT nResult = gnOK;


	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

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

	if (m_bUseDefaultDelays)		//v2.97T1
	{
		lHeadPickDelay	= m_lPickTime_Z;
		lHeadBondDelay	= m_lBondTime_Z;
		lArmPickDelay	= m_lPickTime_Z;
		lArmBondDelay	= m_lBondTime_Z;
	}

	if (m_bEnableSyncMotionViaEjCmd == TRUE)
	{
		lDnTime_Ej = lDnTime_Ej - 3;		//v4.27T3
	}

	LONG lExtraPrGrabDelay = (*m_psmfSRam)["WaferPr"]["ExtraGrabDelay"];	//v.49A3

	//v3.97
	if (CMS896AApp::m_lCycleSpeedMode <= 3)
	{
		if (lArmPickDelay < MS100_MODE3_MIN_ARMPICK_DELAY)
		{
			lArmPickDelay = MS100_MODE3_MIN_ARMPICK_DELAY;    //8ms
		}			
		if (lArmBondDelay < MS100_MODE3_MIN_ARMPICK_DELAY)
		{
			lArmBondDelay = MS100_MODE3_MIN_ARMPICK_DELAY;    //8ms
		}			

		if (lEjectorDownDelay > 0)
		{
			if (lHeadPickDelay < MS100_MODE3_MIN_HEADPICK_DELAY)
			{
				lHeadPickDelay = MS100_MODE3_MIN_HEADPICK_DELAY;    //28ms
			}	
		}
		else
		{
			if (lHeadPickDelay < MS100_MODE3_MIN_HEADBOND_DELAY)
			{
				lHeadPickDelay = MS100_MODE3_MIN_HEADBOND_DELAY;    //30ms
			}	
		}
		if (lHeadBondDelay < MS100_MODE3_MIN_HEADBOND_DELAY)
		{
			lHeadBondDelay = MS100_MODE3_MIN_HEADBOND_DELAY;    //30ms
		}		

		if (lEjectorUpDelay < MS100_MODE3_MIN_EJUP_DELAY)
		{
			lEjectorUpDelay = MS100_MODE3_MIN_EJUP_DELAY;    //10ms
		}			
	}

	LONG lWTStop		= (*m_psmfSRam)["WaferTable"]["AtSafePos"];
	LONG lBTStop		= (*m_psmfSRam)["BinTable"]["AtSafePos"];

	//v3.91
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
				(*m_psmfSRam)["MS899"]["CriticalError"] = bCriticalError;
			}
		}
	}

	//v4.50A6
	if (IsMS60() && CMS896AStn::m_bDBHThermostat)
	{
		m_lMS60TempCheckCounter++;

		if (m_lMS60TempCheckCounter > MS60_BH_THERMALCHECK_CYCLE)
		{
			m_lMS60TempCheckCounter = 0;
			if (IsMS60TempertureOverLoaded(TRUE, TRUE))		//v4.53A19
			{
				HmiMessage_Red_Yellow("BonHead Temperature is overheat!  Machine is aborted!");
				bCriticalError = TRUE;
				(*m_psmfSRam)["MS899"]["CriticalError"] = bCriticalError;
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
		case BOND_Z_TO_SWING_Q:
			break;
		case WAIT_PR_START_Q:
		case MOVE_T_TO_PICK_Q:
			break;
		case WAIT_BT_START_MOVE_Q:
			break;
		case MOVE_Z_TO_BOND_Q:
			break;
		case WAIT_BOND_DELAY_Q:
			break;


		case WAIT_WT_Q:
			DisplaySequence("BH - dbh wait WT Q");
			bWaferEnd	= IsWaferEnded();

			// Check whether the collet & ejector count is larger than the user-defined maximum
			if (lWTStop || (IsMaxColletEjector() == TRUE))
			{
				if (m_bStop)
				{
					if (m_lAreaPickStage == 1)
					{
						DisplaySequence("BH - |area pick stage 1| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
					else
					{
						m_bDBHStop = TRUE;
						m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
						DisplaySequence("BH - Wait WT lWTstop ==> T Prepick to pick");
					}
				}
				else
				{
					DisplaySequence("BH - |WT stop or max collet ejt| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			else if (m_bCheckPr2DCode && !m_bIs2DCodeDone)	//v4.40T1	//PLLM MS109 with BH 2D BC
			{
				CString szCode = _T("");
				BOOL b2DStatus = Search2DBarCodePattern(szCode, IsBHZ1ToPick(), TRUE);
				m_bIs2DCodeDone = TRUE;

				if (!b2DStatus)		//v4.41T3
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			else if (WaitWTReady() || bWaferEnd)	//v3.98T1 add WaferEnd checking
			{	
				SetWTReady(FALSE);
				SetPRStart(FALSE);
				SetPRLatched(FALSE);
				SetEjectorReady(FALSE, "1");
				SetDieBondedForWT(FALSE);
				SetConfirmSrch(FALSE);	
				SetWTStartMove(FALSE);			//v3.89		//Clear it once again as die-ready means WT not yet indexed for sure!
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait WT ready ==> T Prepick to pick");
			}
			else if (m_bStop)	// && m_bFirstCycle)	//v3.68T5	//v3.84	
			{
				//Allow BH to bond last-die on collet if bondarm is waiting at PRE-PICK
				if (m_bFirstCycle || m_bChangeGrade || (m_lAreaPickStage == 1) )
				{
					DisplaySequence("BH - |1st cycle or ap stage 1| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;	
				}
				else
				{
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

			if( IsPrescanning() )
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if( (pApp->GetCustomerName() == "OSRAM") )			//v4.47T16	//Osram MS109 buy-off
				{
					Sleep(50);
					//Check machine pressure
					if (IsLowPressure() == TRUE)
					{
						SetStatusMessage("Machine low pressure");
						SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);
						SetErrorMessage("Machine low pressure");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}

					// check machine cover state
					if ( IsCoverOpen() == TRUE )
					{
						SetStatusMessage("Machine Cover Open");
						SetErrorMessage("Machine Cover Open prescanning");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}
			}
			break;


		case MOVE_T_PREPICK_TO_PICK_Q:
			if (IsSortTo2ndPart())	// sort mode, to align 2nd part wafer
			{
				DisplaySequence("BH - to house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
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

				if (m_bIsWaferEnded && m_nPickListIndex < 0)
				{
					m_qSubOperation = HOUSE_KEEPING_Q;	
					DisplaySequence("BH - scan AP wafer end PrePick to Pick ==> House keeping");
					break;
				}

				//** Wait for BT-Start to avoid extra head-PICK delay at PICK side **//
				if (m_bStop && (m_bFirstCycle || m_bChangeGrade))		//v3.68T5
				{
					DisplaySequence("BH - |1st cycle stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}
				else if (!WaitBTStartMove(200))
				{
					if (m_bStop)
					{
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
					else if (IsBTChngGrd() && CMS896AStn::m_bMS100OriginalBHSequence == FALSE)		//v4.15T7
					{
						//Normal CHANGE-GRADE triggering position in MS100 sequence when BH at PREPICK
						DisplaySequence("BH - reset change grade to false");
						(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
						CMSLogFileUtility::Instance()->BL_LogStatus("BH: reset change grade state");

						if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
						{
CString szLog;
szLog.Format("BH: EjtXY ChangeGrade 1; BH2 = %d", IsBHZ1ToPick()); 
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

							(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"] = TRUE;	//v4.47A3
							m_bChangeGrade	= TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
						}
						else
						{
CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 1");	//v4.47T10
CMSLogFileUtility::Instance()->BT_BackupTableIndexLog();

							m_bFirstCycle		= TRUE;		
							m_bBHZ2TowardsPick	= FALSE;			//BH1 at PICK = FALSE
							(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
							(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"] = TRUE;	//v4.47T9
						}

						m_nBHAlarmCode		= 0;	
						m_nBHLastAlarmCode	= 0;
						break;
					}
					DisplaySequence("BH - T Prepick to pick wait BT start move");
					break;
				}
				else if ((CMS896AApp::m_bBondHeadILCFirstCycle) && (IsEnableILC() == TRUE))
				{
					if (IsMS60())	//v4.49A2
					{
						EnableBHTThermalControl(FALSE);	//v4.49A5
					}

					m_qSubOperation = ILC_RESTART_UPDATE_Q;
					DisplaySequence("BH - T Prepick to pick ==> ILC Restart update q");
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
						nColletCleanStatus = IsCleanCollet();
						m_bBHZ2TowardsPick = bOldState;
						if( nColletCleanStatus==1 )
						{
							DisplaySequence("BH - Prestart ACC error 2");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
					}

					if( m_lAutoLearnLevelsIdleTime>0 )	//	do auto learn p/b levels after start sort. WT and BT should be OK.
					{
						if( OpAutoLearnPickBondLevels(TRUE)==FALSE )
						{
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
					}

					m_bCycleFirstProbe = FALSE;
				}

				if( IsBTChngGrd() )	// HuaMao green light idle, prepick to pick, reset
				{
					SetEjectorReady(FALSE, "2");
					(*m_psmfSRam)["BinTable"]["Change Grade"]  = FALSE;
					DisplaySequence("BH - T Prepick to Pick (BT start move, ejector not ready, change grade to false)");
				}

				TakeTime(PV1);	
				if (IsBHZ1ToPick())			//If BHZ1 towards PICK
				{
					SetPickVacuum(TRUE); 
				}	
				else
				{
					SetPickVacuumZ2(TRUE);
				}

				//v4.49A11
				BOOL bBinChanged	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinChangedForBH"];		//v4.49A11
				if (bBinChanged)
				{
					(*m_psmfSRam)["BinTable"]["BinChangedForBH"] = FALSE;
					if (pAppMod->GetFeatureStatus(MS896A_FUNC_BINBLK_GELPAD))
					{
						OpAutoLearnBHZPickBondLevel(IsBHZ1ToPick(), TRUE);
					}
				}

				if (!OpMoveEjectorTableXY(0, SFM_WAIT))		//v4.42T3
				{
					SetErrorMessage("Ejector XY module is off power!");
					SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("BH - move ejt table error -> house keeping");
					break;
				}

				// Wait for Pick Vacuum settling
				lDelay = (LONG)(s_dbSettling + m_lPickTime_Z - lHeadPickDelay - m_lPrePickTime_T);
				if (lDelay > 0)
				{
					Sleep(lDelay);
				}
				//TakeTime(T1);	

				// Make sure the T is at Pick before Z down
				if (IsZMotorsEnable() == FALSE)
				{
					(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetErrorMessage("BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					break;
				}

				if (!OpIsBinTableLevelAtDnLevel())				//v4.22T8	//Walsin China
				{
					(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetErrorMessage("BH: BT platform at UP level in MOVE_T_PREPICK_TO_PICK_Q !!");
					SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				CheckCoverOpenInAuto("at BH PrePick state");	//v4.31T9

				EnableBHTThermalControl(FALSE);				//v4.49A5

				if (IsBHZ1ToPick())						//If BHZ1 towards PICK	
				{
					//TakeTime(T3);	//v4.49A9
					BA_MoveTo(m_lPickPos_T, SFM_NOWAIT);
				}
				else
				{
					//TakeTime(T1);	//v4.49A9				
					BA_MoveTo(m_lBondPos_T, SFM_NOWAIT);
				}

				m_bComplete_T = FALSE;
				m_bMoveFromPrePick = TRUE;		//v4.40T11	//Sanan	//Enable Head-PrePick delay for Sanan MS100+
				m_lTime_T = m_lPrePickTime_T;
				s_dbStart2 = GetTime();
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 0;
				(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
				m_dWaitBTStartTime	= GetTime();	
				m_qSubOperation		= PICK_Z_DOWN_Q;
				DisplaySequence("BH - T Prepick to Pick ==> Pick Z Down");
			}
			break;


		case WAIT_WT_READY_Q:
			DisplaySequence("BH - wait WT ready q");
			// Check whether Z is completed
			if (m_bComplete_T == FALSE)
			{
				if (T_IsComplete() == TRUE)
				{
					if (IsBHZ2ToPick())		//If BHZ1 towards BOND
					{
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
				if (m_nPickListIndex < 0)
				{
					DisplaySequence("BH - |AP finish| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					Sleep(100);
					SetBTStartMove(TRUE);
					m_qSubOperation = WAIT_BT_READY_Q;	
					DisplaySequence("BH - Wait WT ready stop ==> Wait BT Ready");
				}

				//**** Normal STOP triggering position when user presses STOP button on menu ****//
				m_bDBHStop = TRUE;
			}
			else if (WaitWTReady() || bWaferEnd || bRegionEnd || (m_nPickListIndex == 0))
			{	
				TakeTime(BD);
				SetConfirmSrch(FALSE);		
				SetWTReady(FALSE);
				SetEjectorReady(FALSE, "4");
				SetDieBondedForWT(FALSE);
				SetWTStartMove(FALSE);					//v3.89		//Clear it once again as die-ready means WT not yet indexed for sure!
				m_dWaitBTStartTime = GetTime();			//v2.93T2
				if ((m_nPickListIndex < 0 || bWaferEnd))
				{
					DisplaySequence("BH - |ap end or wafer end| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					m_qSubOperation = WAIT_BT_READY_Q;	
					DisplaySequence("BH - wait WT ready ok ==> wait BT ready");
				}
			}
			else if (WaitBadDieForT())
			{
				SetBadDieForT(FALSE); 
				OpBadDie(TRUE);				//v3.93T3
				StartTime(GetRecordNumber());	// Restart the time
				OpUpdateDieIndex();				// Update the die index
				m_qSubOperation = WAIT_WT_Q;
				DisplaySequence("BH - Bad die to Wait WT");
				s_bUpdateNVRAM = TRUE;
			}
			else if (WaitConfirmSrch())
			{
				SetConfirmSrch(FALSE);
				OpBadDie(TRUE);					// Treat as bad die case	//BH-T to PRE-PICK
				OpUpdateDieIndex();				// Update the die index
				SetBhToPrePick(TRUE);			// Tell WPR that BH is now on PRE-PICK & go for CONFIRM-SEARCH
				m_qSubOperation = WAIT_WT_Q;
				s_bUpdateNVRAM = TRUE;
				DisplaySequence("BH - Confirm search to Wait WT");
			}
			else
			{
//	xu_semitek green light no production
				//Re-implemented for Cree HuiZhou for "NextBinFull NoPickDie" fcn only
				if (m_nBHLastAlarmCode == -2)			//ChangeGrade NoPickDIe	//v4.15T1
				{
					m_qSubOperation = WAIT_BT_READY_Q;	//Go directly to WAIT_BT_READY_Q to change bin!
					DisplaySequence("BH - Wait WT ready alarm code -2 ==> wait BT ready");
				}

				if( IsBTChngGrd() || IsBinFull() )	// HuaMao green light idle, at pick position, move away.
				{
					CString szMsg;
					szMsg.Format("BH - Wait WT ready (Next change %d change GRADE %d, BinFull %d) ==> wait BT Ready Q",
							FALSE, IsBTChngGrd(), IsBinFull());
					DisplaySequence(szMsg);
					m_qSubOperation = WAIT_BT_READY_Q;	//Go directly to WAIT_BT_READY_Q to change bin!
					break;
				}

				if (m_lAreaPickStage < 2)
				{
					SetEjectorReady(FALSE, "7");	
					OpBadDie(TRUE);					// Treat as bad die case	//BH-T to PRE-PICK
					SetEjectorReady(TRUE, "7");
					m_qSubOperation = WAIT_WT_Q;
					s_bUpdateNVRAM = TRUE;
					DisplaySequence("BH - AP complete to WAIT WT Q");
					CMSLogFileUtility::Instance()->WT_GetIdxLog("AP BH no die and to prepick");
				}
				if (IsBinFull())
				{
					CMSLogFileUtility::Instance()->WT_GetIdxLog("AP BH bin full and to prepick");
					T_Sync();	//Must wait T complete for ILC
					Sleep(100);	

					EnableBHTThermalControl(TRUE);
					CheckCoverOpenInAuto("MS100 area pick bin full");
					T_MoveTo(m_lPrePickPos_T);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					m_qSubOperation = WAIT_BIN_FULL_Q;
					if (!m_bFirstCycle)	
					{
						SetBhTReady(TRUE);
					}
					DisplaySequence("BH - AP wait bin full q");
				}

				BOOL bConfirmSrch = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["ConfirmSearch"];
				if (bConfirmSrch)
				{
					Sleep(50);
					CMSLogFileUtility::Instance()->WT_GetIdxLog("AP BH confirm search and to prepick");
					T_Sync();	//Must wait T complete for ILC
					CheckCoverOpenInAuto("MS100 area pick bin full");
					T_MoveTo(m_lPrePickPos_T);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					m_qSubOperation = WAIT_WT_Q;
					DisplaySequence("BH - AP confirm searc ==> wait WT");
				}
			}
			break;


		case WAIT_BT_READY_Q:
			DisplaySequence("BH - Wait BT Ready");
			bWaferEnd		= IsWaferEnded();		//v3.99T1

			if (lBTStop)	//m_bStop)
			{
				DisplaySequence("BH - |BT stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (IsBinFull())
			{
				//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
				T_Sync();	//Must wait T complete for ILC
				Sleep(100);	
			
				EnableBHTThermalControl(TRUE);	//v4.49A5

				CheckCoverOpenInAuto("MS100 bin full");
				T_MoveTo(m_lPrePickPos_T);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
				m_qSubOperation = WAIT_BIN_FULL_Q;
			}	
			else if (WaitBTStartMove(200))		//v3.99T1	//v4.01
			{
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				TakeTime(RD);
				SetBTStartMove(FALSE);
				m_qSubOperation = PICK_Z_DOWN_Q;
				DisplaySequence("BH - Wait BT Ready ==> Pick Z Down");
			}
			else if ( IsBTChngGrd() )	// HuaMao green light idle, wt bt ready
			{
				//Normal CHANGE-GRADE triggering position in MS100 sequence
			
				//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
				T_Sync();	//Must wait T complete for ILC
				Sleep(100);	

				CheckCoverOpenInAuto("MS100 change grade");
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				T_MoveTo(m_lPrePickPos_T);

				//v4.15T1
				if (CMS896AStn::m_bMS100OriginalBHSequence == FALSE)
				{
					if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
					{
						SetErrorMessage("Andrew: EjtXY ChangeGrade 2");
						m_bChangeGrade = TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
					}
					else
					{
CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 2");	//v4.47T10

						m_bFirstCycle		= TRUE;		
						m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
					}

					//Need to reset NoPickDie warning here
					m_nBHAlarmCode		= 0;		//v4.15T6	
					m_nBHLastAlarmCode	= 0;		//v4.15T6
				}

				EnableBHTThermalControl(TRUE);	//v4.49A5
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;

				CMSLogFileUtility::Instance()->BL_LogStatus("BH: change grade to prepick");
				DisplaySequence("BH - Wait BT ready ==> T Prepick to pick");
				if( IsBTChngGrd() )	// HuaMao green light idle, reset ejector ready state
				{
					SetEjectorReady(FALSE, "8");
				}
				break;
			}
			else if (bWaferEnd)			//v3.99T1
			{
				//SetErrorMessage("BH: WaferEnd encountered in WAIT_BT_READY state -> STOP");
				DisplaySequence("BH - |WaferEnd encountered in WAIT_BT_READY state| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitBTReSrchDie())		//v4.xx
			{
				SetBTReSrchDie(FALSE);
				SetWTReady(TRUE);
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
					if (!WaitBTStartMove(1000))
					{
						SetErrorMessage("BH: abnormal STOP encountered in WAIT_BT_READY state -> STOP");
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
				}
			}
			break;


		case PICK_Z_DOWN_Q:
			if( IsBHZ1ToPick() )	// HuaMao green light idle
				DisplaySequence("BH - pick z1 down q");
			else
				DisplaySequence("BH - pick z2 down q");

			if (m_nPickListIndex == 0)
			{
				m_qSubOperation = AP_BOND_LAST_DIE_Q;
				DisplaySequence("BH - AP last die bond to bin ==> AP BOND Last Die");
				break;
			}
			lSPCResult = (*m_psmfSRam)["BondPr"]["PostBondResult"];
			if (m_bDisableBT)
			{
				lSPCResult = 1;
			}

			if (lSPCResult != 1)		//v3.82
			{
				m_lPrevDieSPCResult = lSPCResult;
				OpSetMS100AlarmCode(6);			//BPR PostBond-fail
			}

			if (AllowMove() == TRUE)
			{
				LONG	lPickDelay = 0, lBondDelay = 0;
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

				// check machine cover state
				if (IsCoverOpen() == TRUE)
				{
					SetStatusMessage("Machine Cover Open");
					SetErrorMessage("Machine Cover Open");
					OpSetMS100AlarmCode(2);			//CoverOpen
				}

				//Check Thermal Sensor if present	//4.26T1
				if (OpIsThermalAlarmTriggered() == TRUE)
				{
					SetStatusMessage("Machine DBH Thermal sensor is triggered");
					SetErrorMessage("Machine DBH Thermal sensor is triggered");
					OpSetMS100AlarmCode(7);			//Thermal Alarm
				}


				if (!OpCheckValidAccessMode(TRUE))		//v4.34T4	//SanAn
				{
					m_bDBHStop = TRUE;
				}

				if ( IsMS60() == TRUE )		//Check BH Fan sensor
				{
					if ( m_lCheckFanCount >= 50 )	// 0.06x50 = 3sec
					{
						if (OpCheckMS60BHFanOK() != TRUE)	//v4.48A1
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

				// Check whether it is suitable time for Collet Jam check and Move Z
				s_dbEnd = GetTime();
				lTemp = (LONG)(s_dbEnd - s_dbStart2);

				if (IsBHZ1ToPick())		//If BHZ1
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )		//v4.40T11
					{
						lPickDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z - lTemp + m_lHeadPrePickDelay; 
					}
					else
					{
						lPickDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z - lTemp - 1;	//v4.43T7
					}
				}	
				else
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )		//v4.40T11
					{
						lPickDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z2 - lTemp + m_lHeadPrePickDelay;
					}
					else
					{
						lPickDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z2 - lTemp - 1;	//v4.43T7
					}
				}

				if (IsBHZ1ToPick())		//If BHZ1
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )	//v4.49A9
						lBondDelay = m_lTime_T + lHeadBondDelay - m_lBondTime_Z2 - lTemp + m_lHeadPrePickDelay;	
					else
						lBondDelay = m_lTime_T + lHeadBondDelay - m_lBondTime_Z2 - lTemp - 1;		//v4.43T7
				}
				else
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )	//v4.49A9
						lBondDelay = m_lTime_T + lHeadBondDelay - m_lBondTime_Z - lTemp + m_lHeadPrePickDelay;	
					else
						lBondDelay = m_lTime_T + lHeadBondDelay - m_lBondTime_Z - lTemp - 1;		//v4.43T7
				}

				m_bMoveFromPrePick = FALSE;		//v4.49A9

				//v3.86
				//****************************************//
				if (lPickDelay <= lBondDelay)
				{
					m_bPickDownFirst = TRUE;
				}
				else
				{
					m_bPickDownFirst = FALSE;
				}
				//****************************************//

				//v4.05
				if (m_bPickDownFirst)
				{
					m_nBHZ1BHZ2Delay = lBondDelay - labs(lPickDelay) - 2;	//v4.47T12
				}
				else
				{
					m_nBHZ1BHZ2Delay = lPickDelay - labs(lBondDelay) - 2;	//v4.47T12
				}


				if (m_bComplete_T == FALSE)
				{
					if (CMS896AApp::m_lCycleSpeedMode >= 5)		//v4.27T1	//70ms CT
					{
						if (m_bPickDownFirst)
						{
							if (lPickDelay - 2 > 0)
							{
								Sleep(lPickDelay - 2);
							}
						}
						else
						{
							if (lBondDelay - 2 > 0)
							{
								Sleep(lBondDelay - 2);
							}
						}
					}
					else
					{
						if (m_bPickDownFirst)
						{
							if (lPickDelay - 2 > 0)
							{
								Sleep(lPickDelay - 2);	//v4.47T12
							}
						}
						else
						{
							if (lBondDelay - 2 > 0)
							{
								Sleep(lBondDelay - 2);	//v4.47T12
							}
						}
					}
					if ( (lDelay > 0) && !IsES101() )	
					{
						Sleep(lDelay);
					}
				}

				if (m_bPickDownFirst)
				{
					TakeTime(CJ);		// Take Time
					if (m_ulColletJam_Retry > 0)
					{
						for (j = 0; j < m_ulColletJam_Retry; j++)	//v2.58
						{
							if (OpCheckColletJam() == FALSE)
							{
								break;
							}
							TakeTime(CJO);

							if (m_bSemitekBLMode && 				//v4.44A1	//Semitek
								m_bAutoCleanCollet && 
								m_bMDRetryUseAutoCleanCollet)		//v4.49A3
							{
								OpMS100AutoCleanCollet_MDCJ(FALSE);
							}
							else
							{
								CTime stStartTime = CTime::GetCurrentTime();
								OpColletJam();
								CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AirFlowByJam");
							}
						}
					}

				
					if ((OpCheckColletJam() == TRUE))	//v2.58
					{
						SetErrorMessage("3# Collet is Jammed");
						(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					
						if (IsBHZ1ToPick())		//If BHZ1	
						{
							m_lCJTotalCount1++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ1);
						}
						else
						{
							m_lCJTotalCount2++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"] = m_lCJTotalCount2;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ2);
						}

						CheckCoverOpenInAuto("CJ1 alarm code 3");
						OpSetMS100AlarmCode(3);		//CJ
					}
					else
					{
						//v4.15T1	//CMLT
						if (OpIsNextDieBinFull())	//v4.21T2
						{
							OpSetMS100AlarmCode(-3);	//NextBinFull NoPickDie
						}

						if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop)		//v4.15T1
						{
							CString szMsg;
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								TakeTime(Z3);
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH1 to PICK");		//v4.47T9
								AC_Z1P_MoveToOrSearch(m_lPickLevel_Z, SFM_NOWAIT);
								
								m_bComplete_Z = FALSE;
								szMsg.Format("BH - 1 to pick pd1st true pick z down q");
							}
							else
							{
								TakeTime(Z7);
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH2 to PICK");		//v4.47T9
								AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2, SFM_NOWAIT);
								m_bComplete_Z2 = FALSE;
								szMsg.Format("BH - 2 to pick pd1st true pick z down q");
							}
							CString szTemp;
							szTemp.Format(" pick index %d", m_nPickListIndex);
							szMsg += szTemp;
							m_nPickListIndex--;
							DisplaySequence(szMsg);
						}
					}

					s_dbStart = GetTime();	
					m_qSubOperation = PICK_Z2_DOWN_Q;	// -> normal flow
					DisplaySequence("BH - Pick Z Down pick 1st ==> Pick Z2 Down");
					SetBhTReady(FALSE);
				}
				else
				{
					TakeTime(MD);	

					if (m_bFirstCycle || m_bChangeGrade)
					{


					}
					else if (OpCheckMissingDie())
					{
						TakeTime(MDO);
						IncreaseMissingDieCounter();

						if (m_pulStorage != NULL)
						{
							*(m_pulStorage + BH_MISSINGDIE_OFFSET) = m_ulMissingDieCount;
							*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = m_ulCurrWaferMissingDieCount;
						}

						// If retry is 0, simply stop
						LONG lMissingDieStop = 0;
						if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
						{
							m_lMDCycleCount1++;
							m_lMDTotalCount1++;
							(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"] = m_lMDTotalCount1; //4.54T15

							if( m_lMDCycleLimit>0 && (m_lMDCycleCount1>=m_lMDCycleLimit) )
							{
								lMissingDieStop = 3;
							}
							if ( (m_lMD_Count == 0) )
							{
								lMissingDieStop = 1;
							}
						}
						else
						{
							m_lMDCycleCount2++;
							m_lMDTotalCount2++;
							(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"] = m_lMDTotalCount2; //4.54T15

							if( m_lMDCycleLimit>0 && (m_lMDCycleCount2>=m_lMDCycleLimit) )
							{
								lMissingDieStop = 4;
							}
							if ( (m_lMD_Count2 == 0) )
							{
								lMissingDieStop = 2;
							}
						}


						if( lMissingDieStop>0 )
						{
							CString szLogMsg;
							if( lMissingDieStop==1 )
							{
								SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ1);
								szLogMsg = "BH 1 Missing Die Detected";
							}
							if( lMissingDieStop==2 )
							{
								SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ2);
								szLogMsg = "BH 2 Missing Die Detected";
							}
							if( lMissingDieStop==3 )
							{
								SetAlert_Red_Yellow(IDS_BH_MD_ACCUM_BHZ1);
								szLogMsg = "BH 1 Accumulated Missing Die Detected";
							}
							if( lMissingDieStop==4 )
							{
								SetAlert_Red_Yellow(IDS_BH_MD_ACCUM_BHZ2);
								szLogMsg = "BH 2 Accumulated Missing Die Detected";
							}
							SetErrorMessage(szLogMsg);

							//BH back to SAFE position & abort PICK action 
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
							}
							else
							{
								AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
							}

							CheckCoverOpenInAuto("MD1 stop");

							T_MoveTo(m_lCleanColletPos_T);		// Move T to Blow position
						
							// Treat as no die picked
							(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
							//SetDiePicked(FALSE);
							TakeTime(PV2);		
						
							if (IsBHZ1ToPick())		//If BHZ1 towawrds PICK
							{
								SetPickVacuumZ2(FALSE); 
							}			
							else
							{
								SetPickVacuum(FALSE); 
							}			

							Sleep(1000);
							(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
							m_qSubOperation = HOUSE_KEEPING_Q;	
							DisplaySequence("BH - |missing die| ==> house keeping");
							break;		//v3.86T3
						}
						else
						{
							// Wait T complete
							if (m_bComplete_T == FALSE)
							{
								T_Sync();
								m_bComplete_T = TRUE;
								
								//v4.43T1
								if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
								{
									TakeTime(T4);
								}
								else
								{
									TakeTime(T2);
								}
							}

							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								m_lMD_Count2--;
							}
							else
							{
								m_lMD_Count--;
							}
							(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;

							//v3.81
							//First move Z/Z2 up at PICK side before moving BH to ColletClean pos
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								if (m_bComplete_Z == FALSE)
								{
									Z_Sync();
								}
								AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
							}
							else
							{
								if (m_bComplete_Z2 == FALSE)
								{
									Z2_Sync();
								}
								AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
							}

							if (m_bSemitekBLMode && 			//v4.44A1	//SEmitek
								m_bAutoCleanCollet && 
								m_bMDRetryUseAutoCleanCollet)	//v4.49A3	//WH SanAn
							{
								OpMS100AutoCleanCollet_MDCJ(TRUE);
								T_Profile(CLEAN_COLLET_PROF);		// Change T profile
								T_MoveTo(m_lCleanColletPos_T);		// Move to clean collet position
							}
							else
							{
								CTime stStartTime = CTime::GetCurrentTime();
								CheckCoverOpenInAuto("MD1 blow");
								T_Profile(CLEAN_COLLET_PROF);		// Change T profile
								T_MoveTo(m_lCleanColletPos_T);		// Move to clean collet position

								TakeTime(PV2);	
								if (IsBHZ1ToPick())		//If BHZ1 towawrds PICK		
								{
									SetPickVacuumZ2(FALSE);		
									Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
									SetStrongBlowZ2(TRUE);
									Sleep(m_lHighBlowTime);	 
									SetStrongBlowZ2(FALSE);
								}
								else
								{
									SetPickVacuum(FALSE);	
									Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
									SetStrongBlow(TRUE);
									Sleep(m_lHighBlowTime);	
									SetStrongBlow(FALSE);
								}
								CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AirFlowByMD");
							}
							Sleep(1000);
			
							if (IsZMotorsEnable() == FALSE)
							{
								(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
								SetErrorMessage("BondHead module is off power");
								SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
								m_qSubOperation = HOUSE_KEEPING_Q;
								break;
							}

							CheckCoverOpenInAuto("MD Bond To Pick");
						
							if (IsBHZ1ToPick())	//If BHZ1 towards PICK
							{
								//BA_MoveTo(m_lPickPos_T, SFM_WAIT);
								T_MoveTo(m_lPickPos_T, SFM_WAIT);		//v4.46T14
							}
							else
							{
								//BA_MoveTo(m_lBondPos_T, SFM_WAIT);
								T_MoveTo(m_lBondPos_T, SFM_WAIT);		//v4.46T14
							}

							T_Profile(NORMAL_PROF);

							//v3.81
							//Finally move Z/Z2 down to PICK level when done
							CString szMsg;
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								szMsg.Format("BH - 1 repick missing die");
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH1 to REPICK 1");
								AC_Z1P_MoveToOrSearch(m_lPickLevel_Z, SFM_WAIT);
							}
							else
							{
								szMsg.Format("BH - 2 repick missing die");
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH2 to REPICK 1");
								AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2, SFM_WAIT);
							}

							CString szTemp;
							szTemp.Format(" pick index %d", m_nPickListIndex);
							szMsg += szTemp;
							m_nPickListIndex--;
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
							DisplaySequence(szMsg);
							//BH Z 1/2 do not go to BOND level in this case; however,
							//need to continue PICK_Z2 DOWN_Q for PICK;
						
							OpSetMS100AlarmCode(-1);	//MD-Retry warning
						}
					}
					else
					{
						if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						{
							m_lMD_Count2	= m_lMissingDie_Retry;    // Reset the local count
						}	
						else
						{
							m_lMD_Count		= m_lMissingDie_Retry;    // Reset the local count
						}

						//v4.35T2
						lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];
						if (lMotionTime_BT > 150)
						{
							Sleep(m_lBinTableDelay);
						}

						if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						{
							TakeTime(Z7);
							CMSLogFileUtility::Instance()->BT_TableIndexLog("BH2 to BOND");

							LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);		//v4.48A8
							AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT);

							m_bComplete_Z2 = FALSE;
							szMsg.Format("BH - 2 to bond bd1st false pick z down q");
						}
						else
						{
							TakeTime(Z3);
							CMSLogFileUtility::Instance()->BT_TableIndexLog("BH1 to BOND");

							LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);	//v4.48A8
							AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT);

							m_bComplete_Z = FALSE;
							szMsg.Format("BH - 1 to bond bd1st false pick z down q");
						}								
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						DisplaySequence(szMsg);
					}

					//s_dbStart = GetTime();	
					m_qSubOperation = PICK_Z2_DOWN_Q;	
					DisplaySequence("BH - Pick Z Down bd1rst ==> Pick Z2 Down");
					SetBhTReady(FALSE);
				}
			}
			break;


		case PICK_Z2_DOWN_Q:
			if( IsBHZ1ToPick() )	// HuaMao green light idle
				DisplaySequence("BH - PICK Z2_DOWN_Q pick z1 down q");
			else
				DisplaySequence("BH - PICK Z2_DOWN_Q pick z2 down q");
			if (AllowMove() == TRUE)
			{
				if (m_nBHZ1BHZ2Delay > 0)	//v4.47T7
				{
					Sleep(m_nBHZ1BHZ2Delay);
				}

				if (!m_bPickDownFirst)
				{
					if (IsBHZ1ToPick())		//If BHZ1
					{
						if (m_lPickTime_Z <= lHeadPickDelay)
						{
							if (m_bComplete_T == FALSE)
							{
								T_Sync();
								m_bComplete_T = TRUE;	//v4.43T1
								TakeTime(T4);
							}
						}
					}
					else
					{
						if (m_lPickTime_Z2 <= lHeadPickDelay)
						{
							if (m_bComplete_T == FALSE)
							{
								T_Sync();
								m_bComplete_T = TRUE;	//v4.43T1
								TakeTime(T2);
							}
						}
					}

					TakeTime(CJ);		// Take Time
					if (m_ulColletJam_Retry > 0)
					{
						for (j = 0; j < m_ulColletJam_Retry; j++)	//v2.58
						{
							if (OpCheckColletJam() == FALSE)
							{
								break;
							}
							TakeTime(CJO);

							if (m_bSemitekBLMode && 			//v4.44A1	//Semitek
								m_bAutoCleanCollet &&	
								m_bMDRetryUseAutoCleanCollet)	//v4.49A3
							{
								OpMS100AutoCleanCollet_MDCJ(FALSE);
							}
							else
							{
								CTime stStartTime = CTime::GetCurrentTime();
								OpColletJam();
								CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AirFlowByJam");
							}
						}
					}

					if ((OpCheckColletJam() == TRUE))	//v2.58
					{
						SetErrorMessage("4# Collet is Jammed");
						(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
						
						if (IsBHZ1ToPick())		//If BHZ1	
						{
							m_lCJTotalCount1++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ1);
						}
						else
						{
							m_lCJTotalCount2++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"] = m_lCJTotalCount2;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ2);
						}

						CheckCoverOpenInAuto("CJ2 alarm code 3");

						OpSetMS100AlarmCode(3);		//CJ
					}
					else
					{
						//v4.15T1	//CMLT
						if (OpIsNextDieBinFull())	//v4.21T2
						{
							OpSetMS100AlarmCode(-3);	//NextBinFull NoPickDie
						}

						if (OpCheckAlarmCodeOKToPick()	&& !m_bDBHStop)		//v4.15T1
						{
							CString szMsg;
							if (IsBHZ1ToPick())	//If BHZ1	
							{
								TakeTime(Z3);
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH1 to PICK");		//v4.47T9
								AC_Z1P_MoveToOrSearch(m_lPickLevel_Z, SFM_NOWAIT);

								m_bComplete_Z = FALSE;
								szMsg.Format("BH - 1 to pick bd1st false PICK Z2_DOWN_Q");
							}
							else
							{
								TakeTime(Z7);
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH2 to PICK");

								AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2, SFM_NOWAIT);

								m_bComplete_Z2 = FALSE;
								szMsg.Format("BH - 2 to pick bd1st false PICK Z2_DOWN_Q");
							}

							CString szTemp;
							szTemp.Format(" pick index %d", m_nPickListIndex);
							szMsg += szTemp;
							m_nPickListIndex--;
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
							DisplaySequence(szMsg);
						}

						s_dbStart = GetTime();	
					}

					m_qSubOperation = EJ_UP_Q_PRESCAN_AREA_PICK;
					DisplaySequence("BH - Pick Z2 Down bd1st ==> EJ UP");
				}
				else
				{
					if (IsBHZ1ToPick())		//If BHZ1
					{
						if (m_lBondTime_Z2 <= lHeadBondDelay)
						{
							if (m_bComplete_T == FALSE)
							{
								T_Sync();
								m_bComplete_T = TRUE;	//v4.43T1
								TakeTime(T4);
							}
						}
					}
					else
					{
						if (m_lBondTime_Z <= lHeadBondDelay)
						{
							if (m_bComplete_T == FALSE)
							{
								T_Sync();
								m_bComplete_T = TRUE;	//v4.43T1
								TakeTime(T2);
							}
						}
					}

					TakeTime(MD);	

					if (m_bFirstCycle || m_bChangeGrade)
					{
						//v3.67
						m_qSubOperation = EJ_UP_Q_PRESCAN_AREA_PICK;				// <- Normal flow 
						DisplaySequence("BH - Pick Z2 Down pick 1st 1st cycle ==> EJ UP");
					}
					else if (OpCheckMissingDie() == TRUE)
					{
						TakeTime(MDO);
						IncreaseMissingDieCounter();

						if (m_pulStorage != NULL)
						{
							*(m_pulStorage + BH_MISSINGDIE_OFFSET) = m_ulMissingDieCount;
							*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = m_ulCurrWaferMissingDieCount;
						}

						// If retry is 0, simply stop
						LONG lMissingDieStop = 0;
						if (IsBHZ2ToPick())	//If BHZ1 towards PICK	
						{
							m_lMDCycleCount1++;
							m_lMDTotalCount1++;
							(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"] = m_lMDTotalCount1; //4.54T15

							if( m_lMDCycleLimit>0 && (m_lMDCycleCount1>=m_lMDCycleLimit) )
							{
								lMissingDieStop = 3;
							}
							if ( (m_lMD_Count == 0) )
							{
								lMissingDieStop = 1;
							}
						}
						else
						{
							m_lMDCycleCount2++;
							m_lMDTotalCount2++;
							(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"] = m_lMDTotalCount2; //4.54T15

							if( m_lMDCycleLimit>0 && (m_lMDCycleCount2>=m_lMDCycleLimit) )
							{
								lMissingDieStop = 4;
							}
							if ( (m_lMD_Count2 == 0) )
							{
								lMissingDieStop = 2;
							}
						}

						if( lMissingDieStop>0 )
						{
							CString szLogMsg;
							if( lMissingDieStop==1 )
							{
								SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ1);
								szLogMsg = "BH 1 Missing Die Detected";
							}
							if( lMissingDieStop==2 )
							{
								SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ2);
								szLogMsg = "BH 2 Missing Die Detected";
							}
							if( lMissingDieStop==3 )
							{
								SetAlert_Red_Yellow(IDS_BH_MD_ACCUM_BHZ1);
								szLogMsg = "BH 1 Accumulated Missing Die Detected";
							}
							if( lMissingDieStop==4 )
							{
								SetAlert_Red_Yellow(IDS_BH_MD_ACCUM_BHZ2);
								szLogMsg = "BH 2 Accumulated Missing Die Detected";
							}
							SetErrorMessage(szLogMsg);

							//BH back to SAFE position & abort PICK action 
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
							}
							else
							{
								AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
							}

							CheckCoverOpenInAuto("MD2 stop");

							T_MoveTo(m_lCleanColletPos_T);		// Move T to Blow position
						
							// Treat as no die picked
							(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
							//SetDiePicked(FALSE);
							TakeTime(PV2);		
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK
							{
								SetPickVacuumZ2(FALSE); 
							}		
							else
							{
								SetPickVacuum(FALSE); 
							}	
							Sleep(1000);
			
							(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
							DisplaySequence("BH - |missing| ==> house keeping");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;		//v3.86T3
						}
						else
						{
							// Wait T complete
							if (m_bComplete_T == FALSE)
							{
								T_Sync(); 
								m_bComplete_T = TRUE;		//v4.43T1
								//v4.43T1
								if (IsBHZ1ToPick())	//If BHZ1 towards PICK	
								{
									TakeTime(T4);
								}
								else
								{
									TakeTime(T2);
								}
							}

							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								m_lMD_Count2--;
							}
							else
							{
								m_lMD_Count--;
							}
							(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;

							//Move BHZ/Z2 up at PICK side before moving to CleanCOllet pos
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								if (m_bComplete_Z == FALSE)
								{
									Z_Sync();
								}
								AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
							}
							else
							{
								if (m_bComplete_Z2 == FALSE)
								{
									Z2_Sync();
								}
								AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
							}

							
							if (m_bSemitekBLMode && 				//v4.44A1	//SEmitek
								m_bAutoCleanCollet &&
								m_bMDRetryUseAutoCleanCollet)		//v4.49A3	//WH SanAn
							{
								OpMS100AutoCleanCollet_MDCJ(TRUE);
								T_Profile(CLEAN_COLLET_PROF);		// Change T profile
								T_MoveTo(m_lCleanColletPos_T);		// Move to clean collet position
							}
							else
							{
								CTime stStartTime = CTime::GetCurrentTime();
								CheckCoverOpenInAuto("MD2 blow");
								T_Profile(CLEAN_COLLET_PROF);		// Change T profile
								T_MoveTo(m_lCleanColletPos_T);		// Move to clean collet position

								TakeTime(PV2);	
								//Blow collet at BOND side
								if (IsBHZ1ToPick())		//If BHZ1 towards PICK
								{
									SetPickVacuumZ2(FALSE);
									Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
									SetStrongBlowZ2(TRUE);
									Sleep(m_lHighBlowTime);		
									SetStrongBlowZ2(FALSE);
								}
								else
								{
									SetPickVacuum(FALSE);
									Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
									SetStrongBlow(TRUE);
									Sleep(m_lHighBlowTime);		
									SetStrongBlow(FALSE);
								}
								CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AirFlowByMD");
							}
							Sleep(1000);

							if (IsZMotorsEnable() == FALSE)
							{
								(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
								SetErrorMessage("BondHead module is off power");
								SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
								m_qSubOperation = HOUSE_KEEPING_Q;
								break;
							}

							CheckCoverOpenInAuto("MD Bond To Pick");

							if (IsBHZ1ToPick())
							{
								T_MoveTo(m_lPickPos_T,		SFM_WAIT);		//v4.46T14
							}
							else
							{
								T_MoveTo(m_lBondPos_T,		SFM_WAIT);		//v4.46T14
							}

							T_Profile(NORMAL_PROF);
					
							//v3.81
							CString szMsg;
							if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
							{
								szMsg.Format("BH1 - repick missing die");
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH1 to REPICK");
								AC_Z1P_MoveToOrSearch(m_lPickLevel_Z, SFM_WAIT);
							}
							else
							{
								szMsg.Format("BH2 - repick missing die");
								CMSLogFileUtility::Instance()->BPR_Arm1Log("BH2 to REPICK");
								AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2, SFM_WAIT);
							}

							CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
							m_qSubOperation = EJ_UP_Q_PRESCAN_AREA_PICK;				// <- Normal flow	//v4.08
							DisplaySequence("BH - Pick Z2 Down pick 1st repick slow ==> EJ UP");

							OpSetMS100AlarmCode(-1);		//MD-Retry warning	
						}
					}
					else
					{
						if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						{
							m_lMD_Count2	= m_lMissingDie_Retry;    // Reset the local count
						}	
						else
						{
							m_lMD_Count		= m_lMissingDie_Retry;    // Reset the local count
						}	

						//v4.35T2
						lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];
						if ( (lMotionTime_BT > 150))	//v4.47T8
						{
							Sleep(m_lBinTableDelay);
						}

						if (IsBHZ1ToPick())		//If BHZ1
						{
							TakeTime(Z7);
							CMSLogFileUtility::Instance()->BT_TableIndexLog("BH2 to BOND");

							LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);	//v4.48A8
							AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT);
							m_bComplete_Z2 = FALSE;
							szMsg.Format("BH - 2 to bond pd1st true PICK Z2_DOWN_Q");
						}
						else
						{
							TakeTime(Z3);
							CMSLogFileUtility::Instance()->BT_TableIndexLog("BH1 to BOND");
							
							LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);	//v4.48A8
							AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT);

							m_bComplete_Z = FALSE;
							szMsg.Format("BH - 1 to bond pd1st true PICK Z2_DOWN_Q");
						}
						DisplaySequence(szMsg + " ==> EJ UP");
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);

						m_qSubOperation = EJ_UP_Q_PRESCAN_AREA_PICK;
					}
				}
			}
			break;

		case EJ_UP_Q_PRESCAN_AREA_PICK:
			if (AllowMove() == TRUE)
			{
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

				if (m_lSyncPickDelay != 0)
				{
					lTime_Ej = m_lSlowUpTime_Ej;
				}
				else
				{
					lTime_Ej = m_lTime_Ej;
				}

				if (m_bEnableSyncMotionViaEjCmd || (lEjectorUpDelay >= lTime_Ej))
				{
					//Syn BH at PICK side BEFORE EJ UP
					if (IsBHZ1ToPick())		//If BHZ1 at PICK		
					{
						Z_Sync();
						TakeTime(Z4);
						//Sync BH at BOND side if motion completed before EJ UP		//v3.86
						if ((lHeadBondDelay - lHeadPickDelay) < (lEjectorUpDelay - lTime_Ej))
						{
							Z2_Sync();
							TakeTime(Z8);
						}
					}
					else
					{
						Z2_Sync();
						TakeTime(Z8);
						//Sync BH at BOND side if motion completed before EJ UP		//v3.86
						if ((lHeadBondDelay - lHeadPickDelay) < (lEjectorUpDelay - lTime_Ej))
						{
							Z_Sync();
							TakeTime(Z4);
						}
					}
				}

				lEjectorLvl = m_lEjectLevel_Ej;
				if (m_bEnableSyncMove == TRUE)
				{
					lEjectorLvl = m_lEjectLevel_Ej - m_lSyncDistance;
				}

				// Calculate the compensation for wear out of ejector pin
				lCompen_Ej = OpCalCompen_Ej();

				s_dbEnd = GetTime();
				lTemp = (LONG)(s_dbEnd - s_dbStart);

				if (IsBHZ1ToPick())		//If BHZ1
				{
					lDelay = m_lPickTime_Z + lEjectorUpDelay - lTime_Ej - lTemp - 3;    //v3.81		2 -> 3
				}	
				else
				{
					lDelay = m_lPickTime_Z2 + lEjectorUpDelay - lTime_Ej - lTemp - 3;    //v3.81		2 -> 3
				}	
			
				if (lDelay > 0)	// T complete before Ej start
				{
					Sleep(lDelay);
				}				

				//v3.86T5
				//double-check post-bond result here before Ej UP
				lSPCResult = (*m_psmfSRam)["BondPr"]["PostBondResult"];
				if (m_bDisableBT)
				{
					lSPCResult = 1;
				}

				//v4.26T1	//Fixed EJ Up delay bug when Sync-Pick fcn is used
				if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop)		//v4.15T1
				{
					if (m_bEnableSyncMotionViaEjCmd)
					{
						if (IsBHZ1ToPick())
						{
							MotionSyncViaEjCmdDistance(BH_AXIS_Z, &m_stBHAxis_Z, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);
						}
						else
						{
							MotionSyncViaEjCmdDistance(BH_AXIS_Z2, &m_stBHAxis_Z2, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);
						}
					}
				}

				if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop && m_bDisableBH == FALSE)		//v4.15T1
				{
					bWaferEnd	= IsWaferEnded();
					if (bWaferEnd && IsWLManualMode())		//v4.40T12
					{
					}
					else
					{
						CMSLogFileUtility::Instance()->WT_GetIdxLog("BH AP EJ to up");
						TakeTime(EJ1);	
						Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
						m_bComplete_Ej = FALSE;
						OpUpdateMS100EjectorCounter(1);	//v4.40T12
						OpUpdateMS100ColletCounter();	//v4.40T12
					}
				}

				s_dbStart = GetTime();

				if (lEjectorUpDelay < lTime_Ej)
				{
					//Syn BH at PICK side AFTER EJ UP
					if (IsBHZ1ToPick())		//If BHZ1 at PICK		
					{
						if (m_bComplete_Z == FALSE)
						{
							Z_Sync();
							TakeTime(Z4);
						}
						if (m_bComplete_Z2 == FALSE)
						{
							Z2_Sync();
							TakeTime(Z8);
						}
					}
					else
					{
						if (m_bComplete_Z2 == FALSE)
						{
							Z2_Sync();
							TakeTime(Z8);
						}
						if (m_bComplete_Z == FALSE)
						{
							Z_Sync();
							TakeTime(Z4);
						}
					}
				}
				else
				{
					//Sync BH at BOND side if motion completed after EJ UP		//v3.86
					if (IsBHZ1ToPick())		//If BHZ1 at PICK		
					{
						if (m_bComplete_Z2 == FALSE)
						{
							Z2_Sync();
							TakeTime(Z8);
						}
					}
					else
					{
						if (m_bComplete_Z == FALSE)
						{
							Z_Sync();
							TakeTime(Z4);
						}
					}
				}

				if (m_lSyncPickDelay >= 0)		//v3.83
				{
					if ( m_bComplete_Ej == FALSE )
					{
						Ej_Sync();
						TakeTime(EJ2);
						m_bComplete_Ej = TRUE;
					}
				}

				m_qSubOperation = WAIT_PICK_DELAY_Q;	// -> normal flow		//v4.08
				break;
			}
			break;


		case WAIT_PICK_DELAY_Q:
			DisplaySequence("BH - Wait Pick Delay");

			//Syn BH at BOND side
			if (IsBHZ1ToPick())			//If BHZ1 at PICK	
			{
				if (m_bComplete_Z2 == FALSE)
				{
					Z2_Sync();
					TakeTime(Z8);
				}
				SetPickVacuumZ2(FALSE);				//Turn off BHZ2 vac on BOND side

				if (m_bNeutralVacState && (m_lBondDelay > 0))
				{
					SetStrongBlowZ2(TRUE);    //NEUTRAL state
				}		
			}
			else
			{
				if (m_bComplete_Z == FALSE)
				{
					Z_Sync();
					TakeTime(Z4);
				}
				SetPickVacuum(FALSE);				//Else Turn off BHZ1 Vac on BOND side

				if (m_bNeutralVacState && (m_lBondDelay > 0))
				{
					SetStrongBlow(TRUE);    //NEUTRAL state
				}		
			}

			//Chck CP2 completion before Bh to continue next cycle
			if (!m_bFirstCycle && !m_bChangeGrade)
			{
				SetWTStartMove(FALSE);
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
		
				if (m_nBHAlarmCode != -1)		//If not MD-RETRY, index BT by 1	
				{
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
				if ( m_lPickDelay > 0 )
				{
					Sleep(m_lPickDelay);
				}
			}

			m_bBhInBondState	= FALSE;	

			//v4.00T1
			if (m_bNeutralVacState && (m_lBondDelay > 0))
			{
				//Use PLLM REBEL WB sequence
				if (IsBHZ1ToPick())			//If BHZ1 at PICK	
				{
					SetStrongBlowZ2(FALSE);			//BHZ2 to WEAKBLOW
					Sleep(m_lBondDelay);
				}
				else
				{
					SetStrongBlow(FALSE);			//BHZ1 to WEAKBLOW
					Sleep(m_lBondDelay);
				}
			}

			//MD response time measurement option in SERVICE page
			if (m_bEnableMDResponseChecking)	//v3.94
			{
				OpCheckMissingDieResponseTime();
			}

			//Update Ejector & Collet counter
			bWaferEnd	= IsWaferEnded();
			if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop && !bWaferEnd)		//v4.43T12
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

					CMSLogFileUtility::Instance()->WT_GetCTLog_CT_NoUpdate(m_dCycleTime);		//v4.06		//MS100+ CT study
				
					// 3501
					SetGemValue("AB_CurrentCycleTime", m_dCycleTime);
					SetGemValue("AB_AverageCycleTime", m_dAvgCycleTime);
					SetGemValue("AB_CycleCounter", m_ulCycleCount);
					// 7704
					// SendEvent(SG_CEID_AB_RUNTIME, FALSE);
				}
				m_dLastTime = dCurrentTime;
				if (m_nBHAlarmCode != -1)		//If not MD-RETRY	//v4.43T12
				{
					//	V450X16	update bond grade
					m_lAtBondDieGrade = m_ucAtPickDieGrade;
					m_lAtBondDieRow = m_ulAtPickDieRow;
					m_lAtBondDieCol = m_ulAtPickDieCol;
					//	V450X16	update bond grade

					OpUpdateBondedUnit(m_dCycleTime);		//Update bonded unit
				}
				OpUpdateDieIndex();				// Update the die index

			}

			//	V450X16	store pick grade
			m_bSetEjectReadyDone = FALSE;
			m_ulAtPickDieRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
			m_ulAtPickDieCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
			m_ucAtPickDieGrade	= (UCHAR)(LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"] - m_WaferMapWrapper.GetGradeOffset();
			//	V450X16	store pick grade

			OpCheckPostBondEmptyCountToIncBHZ();	//v4.48A4

			m_qSubOperation = MOVE_Z_PICK_UP_Q;
			break;


		case MOVE_Z_PICK_UP_Q:
			DisplaySequence("BH - Move Z Pick Up");

			if (AllowMove() == TRUE)
			{
				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = TRUE;

				TakeTime(CycleTime);
				StartTime(GetRecordNumber());	// Restart the time	

				if (!m_bEnableBHZOpenDac && (m_lSyncPickDelay != 0))		//v3.83	//v3.84
				{
					if ((10 + m_lSyncPickDelay) > 0)
					{
						Sleep(10 + m_lSyncPickDelay);
					}
				}

				TakeTime(Z1);
				if (m_bEnableBHZOpenDac)
				{
					Z_SetOpenDACSwPort(FALSE);
				}
				AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_NOWAIT);

				TakeTime(Z5);
				if (m_bEnableBHZOpenDac)
				{
					Z2_SetOpenDACSwPort(FALSE); 
				}	
				AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_NOWAIT);
			
				if (m_bComplete_Ej == FALSE)		//v3.84
				{
					Ej_Sync();
					TakeTime(EJ2);
					m_bComplete_Ej = TRUE;
				}

				if (lEjectorDownDelay < 0)
				{
					TakeTime(EJ3);
					Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
					m_dEjDnStartTime = GetTime();
					m_bComplete_Ej = FALSE;
				}

				if (m_bDBHStop)		//v3.67T2
				{
					Sleep(30);
				}

				m_bComplete_Z	= FALSE;
				m_bComplete_Z2	= FALSE;

				// trigger for NVRam backup
				SetBackupNVRam(TRUE);
				m_bSetEjectReadyDone = FALSE;

				m_qSubOperation = MOVE_EJ_THEN_T_Q;
			}
			break;


		case MOVE_EJ_THEN_T_Q:
			DisplaySequence("BH - Move Ej then T");
			if (AllowMove() == TRUE)
			{
				if (lArmPickDelay > lArmBondDelay)
				{
					lDelay = lArmPickDelay - lEjectorDownDelay - 1;
				}
				else
				{
					lDelay = lArmBondDelay - lEjectorDownDelay - 1;
				}

				// Move Ej
				if (lEjectorDownDelay > 0)
				{
					Sleep(lEjectorDownDelay);		//Linear Ej
				}		
				if (lEjectorDownDelay >= 0)
				{
					TakeTime(EJ3);	
					Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);

					m_dEjDnStartTime = GetTime();
					m_bComplete_Ej = FALSE;
				}

				if (CMS896AApp::m_lCycleSpeedMode >= 3)
				{
					//** Special Speed-up mode for MS100 100ms testing **//
					bLFDie	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];
					if ((lDelay >= lDnTime_Ej) && !m_bPrAoiSwitchCamera)	//v4.05		// || !bLFDie)	//v3.94
					{
						Ej_Sync();
						TakeTime(EJ4);
						m_bComplete_Ej = TRUE;

						//v4.15T2	//Correct bug with big ArmPick delay values to resolve camera blocking problem
						LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"] + m_lPRDelay;
						LONG lBHTMotion		= (LONG)(lDelay - lDnTime_Ej + (1.0 / 3.0 * m_lBondTime_T));
						LONG lExtraWTDelay	= 0;

						if (lBHTMotion > lWTIndexTime)
						{
							lExtraWTDelay = lBHTMotion - lWTIndexTime; 
							Sleep(lExtraWTDelay);
						}

						SetWTReady(FALSE);						//v3.87T1	//REset WT flag again before letting it to move
						if (!m_bDBHStop)						//v4.02T3
						{
							if (!IsReachACCMaxCount() && OpCheckAlarmCodeOKToPick())	//v4.15T1	//CMLT
							{
								m_bSetEjectReadyDone = TRUE;
								SetEjectorReady(TRUE, "Move EJ then T CS>=3");			//Let WT to INDEX
								TakeTime(LC1);	//v4.42T6
							}
						}
						if (CMS896AApp::m_lCycleSpeedMode >= 4)		//v4.27T3	//70ms CT
						{
							if ((lDelay - lDnTime_Ej - lExtraWTDelay - 2) > 0)
							{
								Sleep(lDelay - lDnTime_Ej - lExtraWTDelay - 2);
							}
						}
						else
						{
							if ((lDelay - lDnTime_Ej - lExtraWTDelay) > 0)	//v4.15T2
							{
								Sleep(lDelay - lDnTime_Ej - lExtraWTDelay);
							}
						}
					}
					else
					{
						//v4.48A1	//MS100P3
						if (CMS896AApp::m_lCycleSpeedMode >= 5)		//v4.27T3	//70ms CT
						{
							if ((lDelay - 1) > 0)	
							{
								Sleep(lDelay - 1);
							}
						}
						else
						{
							if ((lDelay - 2) > 0)	
							{
								Sleep(lDelay - 2);
							}
						}
					}
				}
				else	//** normal MS100 125ms mode **//
				{
					Ej_Sync();
					TakeTime(EJ4);
					m_bComplete_Ej = TRUE;
					
					SetWTReady(FALSE);						//v3.87T1	//REset WT flag again before letting it to move
					if (!m_bDBHStop)						//v4.02T3
					{
						if (!IsReachACCMaxCount() && OpCheckAlarmCodeOKToPick())	//v4.15T1	//CMLT
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move EJ then T CS<3");			//Let WT to INDEX
							TakeTime(LC1);	//v4.42T6
						}
					}

					if ((lDelay - lDnTime_Ej) > 0)
					{
						Sleep(lDelay - lDnTime_Ej);
					}
				}

				m_qSubOperation = MOVE_T_Q;
			}
			break;


		case MOVE_T_Q:		//v2.99T1
			DisplaySequence("BH - T Move AP");
			lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];
			lSPCResult	= (*m_psmfSRam)["BondPr"]["PostBondResult"];
			bWaferEnd	= IsWaferEnded();
			bLFDie		= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];

			if (m_bDisableBT)
			{
				lSPCResult = 1;
			}
			if ((lSPCResult != 1) && (m_nBHAlarmCode != -4))
			{
				//Need to stop at "Next Cycle" instead because curr-die already picked at this moment!
				m_lPrevDieSPCResult = lSPCResult;	//v4.24T7
				bPostBondNextCycleStop = TRUE;
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
//**** Normal WAFEREND quit position to HouseKeeping state ****//
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
					(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetErrorMessage("BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (!OpIsBinTableLevelAtDnLevel())		//v4.44T1	//Semitek
				{
					(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetErrorMessage("BH: BT platform at UP level in MOVE_T_Q !!");
					SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				//v3.86
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

				//v3.86T5
				if( IsReachACCMaxTimeOut() )
				{
					if( m_ulCleanCount < m_ulMaxCleanCount )
					{
						m_ulCleanCount = m_ulMaxCleanCount;
					}
				}

				INT nColletCleanStatus = IsCleanCollet();

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
						TakeTime(LC1);	//v4.42T6
						SetErrorMessage("BH: Clean Collet Let WT to INDEX");
					}

					T_Sync();		//Must wait T complete for ILC
					Sleep(10);	
					CheckCoverOpenInAuto("CC2");
					T_MoveTo(m_lPrePickPos_T);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

					if (nColletCleanStatus == 2)		//Cleaned
					{
						//v4.25
						if (pAppMod->GetCustomerName() == "Testar")
						{
							SetErrorMessage("BH: Clean Collet is reset & continue");		//v4.25
						}

						m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
						if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
						{
							SetBhTReady(TRUE);
							SetBhTReadyForWT(TRUE);		//Let WT to perform WPR	//MS60	//v4.46T22
						}
						m_bFirstCycle = FALSE;	//v3.67
						m_bChangeGrade = FALSE;
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
				else if ((m_nBHLastAlarmCode == -2) ||		//If curr-pick-die is ChangeGrade NoPickDie
					(m_nBHLastAlarmCode == -3) )	//BIN-FULL //v4.43T10
				{
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
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE);		//Let WT to perform WPR	//MS60	//v4.46T22
					}

					if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
					{
						m_bChangeGrade = TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.

szLog.Format("BH: EjtXY ChangeGrade 3; BH2 = %d", IsBHZ1ToPick()); 
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
					}
					else
					{
CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 3");	//v4.47T10

						SetBTStartMove(FALSE);	// suggest by Andrew, HuaMao green light idle
						m_bFirstCycle		= TRUE;		
						m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
					}

					//For BT to move to correct BH1/2 offset XY, in this case 1st die to bond must be BH1
					//(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = TRUE;	//v4.35T4	//MS109		
					m_nBHAlarmCode		= 0;			
					m_nBHLastAlarmCode	= 0;	

					SetBTStartMove(FALSE);

					EnableBHTThermalControl(TRUE);	//v4.49A5

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					Sleep(500);
					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					DisplaySequence("BH - Alarm code -2 Move T ==> Prepick");
					break;
				}

				bPickDie = (BOOL)((LONG)(*m_psmfSRam)["WaferTable"]["PickDie"]);
				if (!bPickDie)
				{
					//	CheckCoverOpenInAuto("Unpick to prepick");
					// The die is only for alignment, move BH to PrePick
					SetWTReady(FALSE);		//v3.80
					//v4.22T4				//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();				//Must wait T complete for ILC
					Sleep(10);	

					T_MoveTo(m_lPrePickPos_T);
					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
					{
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE);		//Let WT to perform WPR	//MS60	//v4.46T22
					}
					m_bFirstCycle = FALSE;	//v3.67
					m_bBhInBondState = FALSE;
					m_qSubOperation = WAIT_WT_Q;
					DisplaySequence("BH - Unpick die to Wait WT");

					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						SetWTReady(FALSE);				//REset WT flag again before letting it to move
						m_bComplete_Ej = TRUE;
						if( OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone == FALSE )		//v4.15T1
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move T WT Die not pick, let ej ready");    //Let WT to INDEX
							TakeTime(LC1);	//v4.42T6
						}			
					}
					break;
				}

				if (m_bPrAoiSwitchCamera || IsCheckPr2DCode() )							//v4.47T7
				{
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						SetWTReady(FALSE);				//REset WT flag again before letting it to move
						m_bComplete_Ej = TRUE;
						if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone == FALSE )		//v4.15T1
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move T Pr2DCode or Switch Cam; let ej ready");    //Let WT to INDEX
							TakeTime(LC1);	//v4.42T6
						}			
					}

					//Sleep(10);			//v4.21T4
					//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();	//Must wait T complete for ILC
					Sleep(10);	

					T_MoveTo(m_lPrePickPos_T, SFM_NOWAIT);
					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					if (m_bPrAoiSwitchCamera)
					{
						SwitchPrAOICamera();
					}

					T_Sync();

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
					{
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE);		//Let WT to perform WPR	//MS60	//v4.46T22
					}
				
					m_bFirstCycle = FALSE;	//v3.67
					m_bChangeGrade = FALSE;	
					m_bBhInBondState = FALSE;
					DisplaySequence("BH - AOI PR switch ==> Wait WT");
					m_qSubOperation = WAIT_WT_Q;
					break;
				}

				//v4.50A6	//TUrn on VAC before T delay //LeoLam
				//v4.42T15	//Now turn on Bh vac earlier (5ms after BOND delay)
				TakeTime(PV1);	
				if (IsBHZ1ToPick())		//If BHZ2 to PICK at NEXT cycle
				{
					SetPickVacuumZ2(TRUE);
				}		
				else
				{
					SetPickVacuum(TRUE);
				}

				//v4.43T8
				bWTNeedRotate	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LookForward"]["NeedRotate"];
				LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"];

				if (lWTIndexTime > BH_WT_MAX_MOTION_TIME || bWTNeedRotate) 
				{
					Ej_Sync();
					TakeTime(EJ4);
					if( bWTNeedRotate )
						SetWTReady(FALSE);		//REset WT flag again before letting it to move
					m_bComplete_Ej = TRUE;
					if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone == FALSE )		//v4.15T1
					{
						m_bSetEjectReadyDone = TRUE;
						SetEjectorReady(TRUE, "Move T WTIndexTime>80 or WTNeedRotate");    //Let WT to INDEX
						TakeTime(LC1);	//v4.42T6
					}				

					//v4.22T4				//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();				//Must wait T complete for ILC
					Sleep(10);	

					//v4.35T1
					if (m_bComplete_Z == FALSE)
					{
						if (m_lPickUpTime_Z2 > m_lPickUpTime_Z)
						{
							Z_Sync();
							TakeTime(Z2);
							Z2_Sync();
							TakeTime(Z6);
						}
						else
						{
							Z2_Sync();
							TakeTime(Z6);
							Z_Sync();
							TakeTime(Z2);
						}
					}				
				
					T_MoveTo(m_lPrePickPos_T);

					CString szMsg;
					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
					{
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE);		//Let WT to perform WPR	//MS60	//v4.46T22
					}

					m_bFirstCycle = FALSE;	//v3.67
					m_bChangeGrade = FALSE;
					m_bBhInBondState = FALSE;
					m_qSubOperation = WAIT_WT_Q;
					DisplaySequence("BH - >80 or ready realign on WFT to Wait WT");
					break;
				}

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

				//Currently set to zero due to camera blocking bug found in MS100;
				lWTIndexDuration = 0;	

				lEjVacOffDelay = 0;
				if (m_lEjVacOffDelay > 0)
					lEjVacOffDelay = m_lEjVacOffDelay + 1;

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

				if (CMS896AApp::m_bMS100Plus9InchOption)
				{
					lTMaxUnblockTime = m_lBondTime_T - 17;		//4.46T12	//SeoulSemi MS109
				}

				//v4.10T1	//Add extra PR delay for long-travel motion
				if (CMS896AApp::m_lCycleSpeedMode >= 4)	
				{
					lWTLongJumpExDelay		= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpDelay"];
					lWTLongJumpMotionTime	= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpMotion"];

					if ((lWTLongJumpMotionTime >= 30) && (lWTIndexTime >= lWTLongJumpMotionTime))
					{
						lTMoveDelay = lTMoveDelay + lWTLongJumpExDelay + 1;		//v4.46T12	//SeoulSemi
					}
				}

				if (lTMoveDelay > lTMaxUnblockTime)
				{
					if (CMS896AApp::m_lCycleSpeedMode >= 3)		//v3.93T1
					{
						LONG lTDelay	= lTMoveDelay - lTMaxUnblockTime;
					
						LONG lEjTime		= (LONG)(GetTime() - m_dEjDnStartTime);
						LONG lEjRemainTime	= lDnTime_Ej - lEjTime;
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
								Ej_Sync();
								TakeTime(EJ4);
								SetWTReady(FALSE);				//REset WT flag again before letting it to move
								if( m_bSetEjectReadyDone == FALSE )
								{
									m_bSetEjectReadyDone = TRUE;
									SetEjectorReady(TRUE, "Move T CS>=3");			//Let WT to INDEX
								}
								m_bComplete_Ej = TRUE;
								TakeTime(LC1);	//v4.42T6
							}

							//lEjCompleteTime = m_lTime_Ej - lEjTime;	
							lTDelay = lTDelay - lEjRemainTime;	//Remaing Delay after waiting for Ej Motion Completion
						}

						lTMoveDelay = lTDelay;
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
						Sleep(lTMoveDelay);
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
				s_dbStart2 = GetTime();			//v2.83T7
				m_qSubOperation = WAIT_Z_EJ_COMPLETE_Q;
				DisplaySequence("BH - move T ==> Wait Z and Ej complete");
			}
			break;

		case WAIT_Z_EJ_COMPLETE_Q:
			DisplaySequence("BH - wait Z EJ complete q");
			// Allow WT to move earlier than Ej completed
			if (m_bComplete_Ej == FALSE)
			{
				if (lArmPickDelay > lArmBondDelay)
				{
					lDelay = m_lDnTime_Ej + lEjectorDownDelay - lArmPickDelay;
				}
				else
				{
					lDelay = m_lDnTime_Ej + lEjectorDownDelay - lArmBondDelay;
				}

				if (lDelay > 0)
					Sleep(lDelay);
				TakeTime(EJ4);
				SetWTReady(FALSE);						//REset WT flag again before letting it to move
				
				if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone == FALSE )		//v4.15T1
				{
					m_bSetEjectReadyDone = TRUE;
					SetEjectorReady(TRUE, "Wait Z EJ complete");    //Let WT to INDEX
					TakeTime(LC1);	//v4.42T6
				}		

				Ej_Sync();
				m_bComplete_Ej = TRUE;
			}

			if (CMS896AApp::m_lCycleSpeedMode < 4)		//v4.02T5
			{
				//Original Checking sequence for >=100ms cycle mode
				if (m_bComplete_Z == FALSE)
				{
					if (m_lPickUpTime_Z2 > m_lPickUpTime_Z)
					{
						Z_Sync();
						TakeTime(Z2);
						Z2_Sync();
						TakeTime(Z6);
					}
					else
					{
						Z2_Sync();
						TakeTime(Z6);
						Z_Sync();
						TakeTime(Z2);
					}
				}				
			}

			if (IsEnableILC() == TRUE)
			{
				BOOL bILCResult = TRUE;
				if (IsBHZ2ToPick())	
				{
					bILCResult = ILC_AutoUpdateInCycle(BH_T_PICK_TO_BOND);
				}			
				else
				{
					bILCResult = ILC_AutoUpdateInCycle(BH_T_BOND_TO_PICK);
				}

				if (!bILCResult)	//v4.47T9
				{
					SetErrorMessage("DBH: ILC Update fails");
CycleEnableDataLog(FALSE, TRUE, TRUE);
					HmiMessage_Red_Yellow("ILC Update fails; machine will be stopped");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			if (!OpMoveEjectorTableXY())	//v4.42T3
			{
				SetErrorMessage("Ejector XY module is off power");
				SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			s_dbEnd = GetTime();
			lTemp = (LONG)(s_dbEnd - s_dbStart2);
			if (m_lTime_T < 60)		//v4.02T5	//if using 55ms motion profile, BPR blocking delay could be reduced to 15
			{
				if (CMS896AApp::m_lCycleSpeedMode >= 6)	//MS100P3	//v4.48A16
				{
					if (lTemp < 8)
					{
						Sleep(8 - lTemp);
					}
				}
				else if (CMS896AApp::m_lCycleSpeedMode >= 4)		//v4.23T1
				{
					if (lTemp < 12)
					{
						Sleep(12 - lTemp);
					}
				}
				else
				{
					if (lTemp < 15)
					{
						Sleep(15 - lTemp);
					}
				}
			}
			else
			{
				if (lTemp < 20)
				{
					Sleep(20 - lTemp);
				}
			}

			if (!m_bFirstCycle && !m_bChangeGrade)	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
			{
				DisplaySequence("BH - let BPR to grab, set BHT ready");
				SetBhTReady(TRUE);						//Let BPR Grade image
				SetBhTReadyForWT(TRUE);		//Let WT to perform WPR	//MS60
			}
		
			if (m_bComplete_Z == FALSE)
			{
				if (!IsMS60())	//v4.50A4
				{
					if (m_lPickUpTime_Z2 > m_lPickUpTime_Z)
					{
						Z_Sync();
						TakeTime(Z2);
						Z2_Sync();
						TakeTime(Z6);
					}
					else
					{
						Z2_Sync();
						TakeTime(Z6);
						Z_Sync();
						TakeTime(Z2);
					}
				}				
			}				

			if( m_bSetEjectReadyDone == FALSE )
			{
				m_bSetEjectReadyDone = TRUE;
				SetEjectorReady(TRUE, "Wait Z EJ Complete at last");
			}

			m_bFirstCycle = FALSE;
			m_bChangeGrade = FALSE;
			m_qSubOperation = WAIT_WT_READY_Q;	
			DisplaySequence("BH - Wait Z EJ complete ==> Wait WT Ready");
			break;

		case AP_BOND_LAST_DIE_Q:
			DisplaySequence("BH - AP last die Q");
			if (IsCoverOpen() == TRUE)
			{
				SetStatusMessage("Machine Cover Open");
				SetErrorMessage("Machine Cover Open");
				DisplaySequence("BH - AP last die cove open ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			T_Sync();
			m_bComplete_T = TRUE;
			m_bComplete_Z2 = TRUE;
			m_bComplete_Z = TRUE;
			Ej_MoveTo(m_lStandbyLevel_Ej);

			bAPContinue = TRUE;
			if (OpCheckMissingDie())
			{
				DisplaySequence("BH - AP last die missing");
				bAPContinue = FALSE;
			}

			if (OpCheckColletJam())
			{
				bAPContinue = FALSE;
				SetErrorMessage("5# Collet is Jammed");
				DisplaySequence("BH - AP last die collet jam"); 
				if (IsBHZ1ToPick())		//If BHZ1	
				{
					SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ1);
				}
				else
				{
					SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ2);
				}
			}

			if (bAPContinue)
			{
				if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
				{
					LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);	//v4.48A8
					AC_Z2B_MoveToOrSearch(lBondZ2, SFM_WAIT);
					szMsg.Format("BH - 2 to bond AP");
					SetPickVacuumZ2(FALSE);				//Turn off BHZ2 vac on BOND side
					if (m_bNeutralVacState && (m_lBondDelay > 0))
					{
						SetStrongBlowZ2(TRUE);    //NEUTRAL state
					}		
				}
				else
				{
					LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);	//v4.48A8
					AC_Z1B_MoveToOrSearch(lBondZ1, SFM_WAIT);

					szMsg.Format("BH - 1 to bond AP");
					SetPickVacuum(FALSE);				//Else Turn off BHZ1 Vac on BOND side
					if (m_bNeutralVacState && (m_lBondDelay > 0))
					{
						SetStrongBlow(TRUE);    //NEUTRAL state
					}		
				}
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				DisplaySequence(szMsg);
				SetBhTReady(FALSE);

				DisplaySequence("BH - AP last die bond Delay");

				SetWTStartMove(FALSE);
				SetBTStartMove(FALSE);
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;

				if (m_nBHAlarmCode != -1)		//If not MD-RETRY, index BT by 1	
				{
					SetDieBonded(TRUE);    //Let BT task to NEXT DIE_Q
				}			
				SetDieBondedForWT(TRUE);		//Let WT task to continue

				if (m_lPickDelay > 0)
				{
					Sleep(m_lPickDelay);
				}

				m_bBhInBondState = FALSE;	

				if (m_bNeutralVacState && (m_lBondDelay > 0))
				{
					//Use PLLM REBEL WB sequence
					if (IsBHZ1ToPick())			//If BHZ1 at PICK	
					{
						SetStrongBlowZ2(FALSE);			//BHZ2 to WEAKBLOW
						Sleep(m_lBondDelay);
					}
					else
					{
						SetStrongBlow(FALSE);			//BHZ1 to WEAKBLOW
						Sleep(m_lBondDelay);
					}
				}

				//Update Ejector & Collet counter
				OpUpdateMachineCounters();

				if (!m_bFirstCycle && !m_bChangeGrade)
				{
					OpUpdateBondedUnit(m_dCycleTime);		//Update bonded unit
					OpUpdateDieIndex();			// Update the die index
				}
			}	// END BOND DIE
			else
			{
				DisplaySequence("BH - AP last die not continue");
				if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
				{
					SetPickVacuumZ2(FALSE);				//Turn off BHZ2 vac on BOND side
				}
				else
				{
					SetPickVacuum(FALSE);				//Else Turn off BHZ1 Vac on BOND side
				}
				SetBhTReady(FALSE);

				SetWTStartMove(FALSE);
				SetBTStartMove(FALSE);
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;

				if (m_nBHAlarmCode != -1)		//If not MD-RETRY, index BT by 1	
				{
					SetDieBonded(TRUE);    //Let BT task to NEXT DIE_Q
				}			
				SetDieBondedForWT(TRUE);		//Let WT task to continue

				if (m_lPickDelay > 0)
				{
					Sleep(m_lPickDelay);
				}

				m_bBhInBondState = FALSE;	
			}

			m_lAPLastDone	= 1;
		
			DisplaySequence("BH - AP last die Swing up");
			if (m_bEnableBHZOpenDac)
			{
				Z_SetOpenDACSwPort(FALSE);
			}
			AC_Z1_MoveTo(m_lSwingLevel_Z);

			if (m_bEnableBHZOpenDac)
			{
				Z2_SetOpenDACSwPort(FALSE); 
			}	
			AC_Z2_MoveTo(m_lSwingLevel_Z2);

			SetWTStartMove(FALSE);
			SetWTReady(FALSE);	//REset WT flag again before letting it to move
			// trigger for NVRam backup
			SetBackupNVRam(TRUE);

			DisplaySequence("BH - AP Move Ej then T");

			CheckCoverOpenInAuto("AP bond last die");
			DisplaySequence("BH - AP last die T to prepick"); 
			T_MoveTo(m_lPrePickPos_T);
			m_bBHZ2TowardsPick = FALSE;
			(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
			(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
			if (!m_bFirstCycle && !m_bChangeGrade)	
			{
				SetBhTReady(TRUE);
				SetBhTReadyForWT(TRUE);		//Let WT to perform WPR	//MS60	//v4.46T22
			}

			m_bBhInBondState = FALSE;
			m_bFirstCycle = TRUE;
			Ej_MoveTo(m_lStandbyLevel_Ej);
			SetEjectorReady(TRUE, "AP Bond Last die");
			m_qSubOperation = WAIT_WT_Q;
			DisplaySequence("BH - AP bonded last die to wait WT");
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
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait Bin Full ==> T Prepick to pick");
			}
			else
			{
				Sleep(1);
			}
			break;


		case POSTBOND_FAIL_Q:
			DisplaySequence("BH - SPC failed");

			lSPCResult = m_lPrevDieSPCResult;	//(*m_psmfSRam)["BondPr"]["PostBondResult"];	//v4.24T7
			if (m_bDisableBT)
			{
				lSPCResult = 1;
			}
			switch (lSPCResult)
			{
				case BPR_ERR_SPC_XY_LIMIT:
					SetAlert_Red_Yellow(IDS_BH_SPC_XY_LIMIT);		// Set Alarm
					SetErrorMessage("Postbond hit XY Limit Count");
					break;

				case BPR_ERR_SPC_MAX_ANGLE:
					SetAlert_Red_Yellow(IDS_BH_SPC_MAX_ANGLE);		// Set Alarm
					SetErrorMessage("Postbond hit Max Angle Limit Count");
					break;

				case BPR_ERR_SPC_AVG_ANGLE:
					SetAlert_Red_Yellow(IDS_BH_SPC_AVG_ANGLE);		// Set Alarm
					SetErrorMessage("Postbond hit Average Angle Limit");
					break;

				case BPR_ERR_SPC_DEFECT:
					SetAlert_Red_Yellow(IDS_BH_SPC_DEFECT);			// Set Alarm
					SetErrorMessage("Postbond hit Defect Limit Count");
					break;

				case BPR_ERR_SPC_EMPTY:
					SetAlert_SE_Red_Yellow(IDS_BH_SPC_EMPTY);			// Set Alarm
					SetErrorMessage("Postbond hit EMPTY Limit Count");
					break;

				case BPR_ERR_SPC_REL_XY_LIMIT:						//v4.26T1	//Semitek	//Relative XY shift check
					SetAlert_Red_Yellow(IDS_BH_SPC_REL_XY_LIMIT);	// Set Alarm
					SetErrorMessage("Postbond hit relative-XY shift Limit Count");
					break;

				default:
					SetAlert_Red_Yellow(IDS_BH_SPC_FAIL);			// Set Alarm
					szMsg.Format("Postbond Detect Error; code=%ld", lSPCResult);	//v4.23T2
					SetErrorMessage(szMsg);
					break;
			}
			DisplaySequence("BH - |post bond fail| ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
			break;

		case ILC_RESTART_UPDATE_Q:

			CMS896AApp::m_bBondHeadILCFirstCycle = FALSE;
			bIsNeedRestartUpdate = ILC_IsNeedRestartUpdate();

			if (!OpIsBinTableLevelAtDnLevel())					//v4.22T8	//Walsin China
			{
				(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
				SetErrorMessage("BH: BT platform at UP level in ILC_RESTART_UPDATE_Q !!");
				SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (bIsNeedRestartUpdate == TRUE)
			{
				CTime tIlcStart = CTime::GetCurrentTime();
				if (ILC_OpRestartUpdateWarmCool() == FALSE)
				{
					CheckCoverOpenInAuto("Restart Update Warm Cool");
					T_MoveTo(m_lPrePickPos_T);
					DisplaySequence("BH - |ILC fail stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (ILC_OpRestartUpdate(bIsNeedRestartUpdate) == FALSE)
				{
					CheckCoverOpenInAuto("Restart Update");
					T_MoveTo(m_lPrePickPos_T);
					DisplaySequence("BH - |ILC fail stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
				CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(tIlcStart, "StartUp ILC");
			}

			m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
			DisplaySequence("BH - ILC ==> T Prepick to pick");
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("BH - AP House Keeping Q");
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
			if (Z_IsPowerOn())			//v3.13T3
			{
				AC_Z1_MoveTo(0, SFM_WAIT);    // Move Z to Zero
			}		

			if (Z2_IsPowerOn())
			{
				AC_Z2_MoveTo(0, SFM_WAIT);
			}

			Ej_MoveTo(0, SFM_WAIT);			// Move Ej to Zero

			//v3.98T3
			TakeTime(CycleTime);
			StartTime(GetRecordNumber());	// Restart the time	

			if (IsBHZ1ToPick())		//If BHZ1 at PICK	
				LogCycleStopState("BH - MoveArm; BH2 to BOND at last cycle");
			else
				LogCycleStopState("BH - MoveArm; BH1 to BOND at last cycle");

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

			SetPickVacuum(FALSE);		// Turn-off the pick vacuum after T-Move done		//v2.77
			SetPickVacuumZ2(FALSE);		// Turn-off the pick vacuum after T-Move done		//v3.86

			if (bWaferEnd)				//v4.40T2	//Nichia MS100+
			{	
				LogCycleStopState("BH - WaferEnd");	//v4.44A1	//SEmitek
				OpResetEjKOffsetAtWaferEnd();	//v4.43T8	//Semitek
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

			// Acknowledge other stations to stop
			StopCycle("BondHeadStn");				
			LogCycleStopState("BH - AP stop completely");
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
		(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
	}
}

BOOL CBondHead::IsBHZ1ToPick()
{
	return !m_bBHZ2TowardsPick;
}

BOOL CBondHead::IsBHZ2ToPick()
{
	return m_bBHZ2TowardsPick;
}

BOOL CBondHead::IsBTChngGrd()
{
	return (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["Change Grade"];
}

BOOL CBondHead::IsBinFull()
{
	return (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinFull"];
}
