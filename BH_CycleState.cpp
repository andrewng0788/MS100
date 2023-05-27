/////////////////////////////////////////////////////////////////
// BH_CycleState.cpp : Cycle Operation State of the CBondHead class
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
#include "BondHead.h"
#include "CycleState.h"
#include "math.h"
#include "BondPr.h"
#include "Mmsystem.h"
#include "WT_SubRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID LogCycle(CONST INT nData)		//v3.13T1
{
	CStdioFile oFile;
	if (oFile.Open("c:\\MapSorter\\UserData\\History\\ChgTime.txt", 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::typeText))
	{
		oFile.SeekToEnd();

		CString szTemp;
		szTemp.Format("%d\n", nData);
		oFile.WriteString(szTemp);

		oFile.Close();
	}
}



VOID CBondHead::AutoOperation()
{
	RunOperation();
}

VOID CBondHead::DemoOperation()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	RunOperation_MegaDa();

	//if (!m_bIsArm2Exist)		//v4.24T10
	//{
	//	RunOperation();
	//}
	//else
	//{
	//	if (m_lDBHBondHeadMode == 1)
	//	{
	//		RunOperation();
	//	}
	//	else
	//	{
	//		if( pUtl->GetPrescanAreaPickMode() )
	//		{
	//			RunOperation_AP();
	//		}
	//		else
	//		{
	//			RunOperation_MegaDa();
	//			//RunOperation_dbh();
	//		}
	//	}
	//}
}

VOID CBondHead::RunOperation()
{
	CString szMsg;
	LONG	i, lTemp, lDelay, lCompen_Ej=0, lSPCResult;
	LONG	lEjectorLvl = 0;
	ULONG	j;
	BOOL	bPickDie;
	BOOL	bWaferEnd, bColletFull;
	BOOL	bRegionEnd = FALSE;
	double	dCurrentTime;
	static	BOOL	s_bUpdateNVRAM=TRUE;
	static	DOUBLE	s_dbStart=0, s_dbStart2=0, s_dbEnd=0, s_dbSettling=100;
	static	DOUBLE	s_dWftComp = 0;
	LONG lMotionTime_BT = 0;
	LONG lWftBlkRealign = 0;
	BOOL bPreBond=FALSE, bPreBondDone=FALSE;	//v3.83
	BOOL bNoMotionHouseKeeping = FALSE;

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	// Store Wafer Table to common area
	//bEnableWafflePadBonding = (*m_psmfSRam)["BondHead"]["WafflePad"]["Enable"];

    if ((Command() == glSTOP_COMMAND) &&
		(Action() == glABORT_STOP))
	{
		DisplaySequence("BH - STOP");
		State(STOPPING_Q);
		return ;
	}

	m_bStep = FALSE;

	//Support of default HeadPick/HeadBond delay = -1
	LONG lHeadPickDelay = m_lHeadPickDelay;
	if (m_lHeadPickDelay <= -1)
		lHeadPickDelay = m_lPickTime_Z;
	
	LONG lHeadBondDelay = m_lHeadBondDelay;
	if (m_lHeadBondDelay <= -1)
		lHeadBondDelay = m_lBondTime_Z;
	
	LONG lArmPickDelay = m_lArmPickDelay;
	if (m_lArmPickDelay <= -1)
		lArmPickDelay = m_lPickTime_Z;

	LONG lArmBondDelay = m_lArmBondDelay;
	if (m_lArmBondDelay <= -1)
		lArmBondDelay = m_lBondTime_Z;

	if (m_bUseDefaultDelays)		//v2.97T1
	{
		lHeadPickDelay	= m_lPickTime_Z;
		lHeadBondDelay	= m_lBondTime_Z;
		lArmPickDelay	= m_lPickTime_Z;
		lArmBondDelay	= m_lBondTime_Z;
	}

	LONG lAutobondCurrTime = 0;
	BOOL bNewCT = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["New CT"];				//v2.83T7
	if (m_bIsLinearEjector)		//v3.53
		bNewCT = TRUE;
	BOOL bBLGripperJam	= (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["GripperJam"];	//v4.45T2

	try
	{
		if ( IsMotionCE() == TRUE )
		{
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		if ( IsBLBCRepeatCheckFail() )
		{
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		if (m_bStopAllMotion == TRUE)
		{
			m_qSubOperation = HOUSE_KEEPING_Q;
			bNoMotionHouseKeeping = TRUE;
		}

		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case WAIT_WT_Q:
			if( IsAOIOnlyMachine()==FALSE )
				DisplaySequence("BH - Wait WT");

			(*m_psmfSRam)["BondHead"]["AtPrePickAndWftReady"] = 0;
			// Check whether the collet & ejector count is larger than the user-defined maximum
			if ( m_bStop )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitWTReady())
			{	
				(*m_psmfSRam)["BondHead"]["AtPrePickAndWftReady"] = 1;
				SetWTReady(FALSE);
				SetPRStart(FALSE);
				SetPRLatched(FALSE);
				SetEjectorReady(FALSE);
				SetDieBondedForWT(FALSE);
				SetConfirmSrch(FALSE);		//andrew
				WftToSafeLog("6 inch wft bh wait wt q to prepick to pick q");
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				s_dWftComp = GetTime();
			}
			else
			{
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

				if ( s_bUpdateNVRAM == TRUE )
				{
					SetBackupNVRam(TRUE);
				}
				
			}

			if( IsPrescanning() )
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if( (pApp->GetCustomerName() == "OSRAM") || m_bScanCheckCoverSensor)	//Osram MS109 buy-off
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
					//	SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
						SetErrorMessage("Machine Cover Open prescanning");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}
			}
			break;

		case MOVE_T_PREPICK_TO_PICK_Q:
			DisplaySequence("BH - Move T from PrePick to Pick");
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if( IsOnlyRegionEnd() )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				pUtl->RegionOrderLog("T Prepick to pick q, region end, to house keeping");
			}
			else if ( AllowMove() == TRUE )
			{
				//Check BHT motion is completed before start other action
				if ( T_IsComplete() == FALSE )
				{
					T_Sync();				
				}
	
				if (CMS896AApp::m_lCycleSpeedMode > 0)	//v3.62
				{
					//** Wait for BT-Start to avoid extra head-PICK delay at PICK side **//
					if (!WaitBTStartMove())
					{
						break;
					}
				}

				if( m_bCycleFirstProbe )
				{
					if( IsMaxColletEjector() == TRUE )
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
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

				TakeTime(PV1);				// Take Time
				SetPickVacuum(TRUE);		// Turn-on the pick vacuum

				if (CMS896AStn::m_bEnableWafflePadBonding == TRUE)
				{
					SetStrongBlow(FALSE);	//to WEAKBLOW
				}

				// Wait for Pick Vacuum settling
				lDelay = (LONG)(s_dbSettling + m_lPickTime_Z - lHeadPickDelay - m_lPrePickTime_T);
				if ( (lDelay > 0) )		//v4.28		//v4.37T10
					Sleep(lDelay);
				TakeTime(T1);				// Take Time
				// Make sure the T is at Pick before Z down
				if  (IsZMotorsEnable() == FALSE)
				{
					SetMotionCE(TRUE, "BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					break;
				}

				if (CMS896AApp::m_lCycleSpeedMode == 2)		//v3.80
				{
					if (m_bFirstCycle)
					{
						T_MoveTo(m_lPickPos_T, SFM_WAIT);
						Sleep(20);
						T_MoveTo(m_lBondPos_T, SFM_WAIT);
						Sleep(20);
						T_MoveTo(m_lPickPos_T, SFM_WAIT);
						Sleep(20);
						T_MoveTo(m_lBondPos_T, SFM_WAIT);
						Sleep(50);
					}
					else
					{
						T_MoveTo(m_lBondPos_T, SFM_WAIT);
						Sleep(20);
					}
				}

				T_MoveTo(m_lPickPos_T, SFM_NOWAIT);
				m_bComplete_T = FALSE;

				if (CMS896AApp::m_lCycleSpeedMode == 2)		//v3.80
					m_lTime_T = m_lBondTime_T;
				else
					m_lTime_T = m_lPrePickTime_T;
				s_dbStart = GetTime();

				m_bMoveFromPrePick = TRUE;		//v3.79
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 0;
				m_dWaitBTStartTime	= GetTime();			//v2.93T2
				m_qSubOperation		= WAIT_BT_START_MOVE_Q;
				WftToSafeLog("6 inch wft bh prepick to pick to bt start move q");
			}
			break;

		case BOND_Z_TO_SWING_Q:
			DisplaySequence("BH - Bond Z to Swing");
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ( AllowMove() == TRUE )
			{
				TakeTime(CycleTime);
				StartTime(GetRecordNumber());		// Restart the time	
				//TakeTime(Z1);		// Take Time
				

				if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
					(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
				{
					CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_STATIC_CONTROL_OBW_Z, &m_stBHAxis_Z);
					CMS896AStn::MotionSelectControlType(BH_AXIS_Z, HP_CTRL_GFL_PD_INTG, 2, &m_stBHAxis_Z);
				}

				AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_NOWAIT);
				
				if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
					(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
				{
					CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_DYNAMIC_CONTROL_OBW_Z, &m_stBHAxis_Z);
					CMS896AStn::MotionSelectControlType(BH_AXIS_Z, HP_CTRL_GFL_FFC_PD_INTG, 2, &m_stBHAxis_Z);
				}

				m_bComplete_Z = FALSE;
				if ( lArmPickDelay > m_lBondTime_Z )
				{
					Z_Sync();	// Wait Z complete first
					TakeTime(Z2);
					Sleep(lArmPickDelay - m_lBondTime_Z);
				}
				else
				{
					if (bNewCT)		//v2.83T7
					{
						if ((m_lArmPickDelay - 2) > 0)
							Sleep(m_lArmPickDelay - 2);
					}
					else
						Sleep(lArmPickDelay);
				}
				// Check whether the collet & ejector count is larger than the user-defined maximum
				if ( IsMaxColletEjector() == TRUE )
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				} 
				else
				{
TakeTime(Z1);
					m_qSubOperation = WAIT_PR_START_Q;
				}
			}
			break;

		case WAIT_PR_START_Q:
			DisplaySequence("BH - Wait PR Start");
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ( (WaitMoveBack() == FALSE) && WaitPRStart() )		//v4.37T10
			{
				SetPRStart(FALSE);

				// Check whether the die is going to be pick
				bPickDie = (BOOL)((LONG)(*m_psmfSRam)["WaferTable"]["PickDie"]);
				if( IsOnlyRegionEnd() )
				{
					pUtl->RegionOrderLog("PR start q, region end, to prepick and wait wt q");
					bPickDie = FALSE;
				}

				if (IsBLEnable() && bBLGripperJam)		//v4.45T2
				{
					//If gripper Jam loses power, emergency stop!!!
					SetErrorMessage("BH: BL gripper jam forces BH to HOUSE_KEEPING_Q");
					CMSLogFileUtility::Instance()->BL_LogStatus("BH: BL gripper jam forces BH to HOUSE_KEEPING_Q");
					(*m_psmfSRam)["BinLoaderStn"]["GripperJam"] = FALSE;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
				else if ( bPickDie )
				{
					m_qSubOperation = MOVE_T_TO_PICK_Q;

					//v3.65	//Semitek
					if ( (CMS896AApp::m_lCycleSpeedMode > 0) )
					{
						BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];	
						BOOL bWtRotate = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["Rotate"];	

						if (!bLFNoComp)		//If current cycle is not LF mode
						{
							DOUBLE dRpy1Time	= (DOUBLE)(*m_psmfSRam)["WaferPr"]["RPY1"];
							DOUBLE dCurrTime	= GetTime() - dRpy1Time;
							LONG lRpy1Time		= (LONG) dCurrTime;
							
							if (lRpy1Time > 1000)	//v3.79
								lRpy1Time = 1000;
							else if (lRpy1Time < 0)
								lRpy1Time = 0;

							//Change default WT motion time + PR time from 90 to 100	//v3.68T4
							if ((m_lTime_T + lHeadPickDelay) < (100 - lRpy1Time))
								Sleep(100 - lRpy1Time - m_lTime_T - lHeadPickDelay);
						}
					}
				}
				else
				{
					// The die is only for alignment, move BH to PrePick
					T_MoveTo(m_lPrePickPos_T);
					SetPickVacuum(FALSE);		//v3.82
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					SetBhTReady(TRUE);
					m_bBhInBondState = FALSE;
					m_qSubOperation = WAIT_WT_Q;
					s_bUpdateNVRAM = TRUE;
				}
			}
			else if (m_bComplete_Z == FALSE)
			{
				if ( Z_IsComplete() == TRUE )
				{
					TakeTime(Z2);
					m_bComplete_Z = TRUE;
				}
			}
			break;

		case MOVE_T_TO_PICK_Q:
			DisplaySequence("BH - Move T to Pick");
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ( AllowMove() == TRUE )
			{
				TakeTime(T1);				// Take Time

				if  (IsZMotorsEnable() == FALSE)
				{
					SetMotionCE(TRUE, "BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					break;
				}

				m_ctBHTAutoCycleLastMoveTime = CTime::GetCurrentTime();
				T_MoveTo(m_lPickPos_T, SFM_NOWAIT);
				m_bComplete_T = FALSE;
				m_lTime_T = m_lBondTime_T;
				s_dbStart = GetTime();

				//andrew
				if (m_bEnableAirFlowTest)
				{
					m_lAirFlowStartTime = (LONG) timeGetTime();	
					m_bStartAirFlowJcCheck	= TRUE;	//andrew
				}

				TakeTime(PV1);				// Take Time
				SetPickVacuum(TRUE);		// Turn-on the pick vacuum
				//if (CMS896AStn::m_bWafflePadBondDie == TRUE)
				if (CMS896AStn::m_bEnableWafflePadBonding == TRUE)
				{
					SetStrongBlow(FALSE);	//to WEAKBLOW
				}

				Sleep(m_lTime_T/3);
				SetBhTReady(TRUE);
				m_bBhInBondState = FALSE;

				if (bNewCT)		//v2.83T7
				{
					if ( m_bComplete_Z == FALSE )
					{
						Z_Sync();
						TakeTime(Z2);
					}
				}
				else
				{
					// Check whether the delay is needed
					if ( (m_lBondTime_Z + m_lPickTime_Z) < (lArmPickDelay + m_lTime_T + lHeadPickDelay) )
					{
						lDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z - (m_lTime_T / 3);
						for ( i=0; i < lDelay; i++ )
						{	
							// If the time is correct, Z must be completed within this for-loop
							if ( ((m_bComplete_Z == FALSE)) && (Z_IsComplete() == TRUE) )
							{
								TakeTime(Z2);
								m_bComplete_Z = TRUE;
								Sleep(lDelay - i);
								break;
							}
							else
							{
								Sleep (1);
							}
						}
					}
					else	// No delay is needed and simply wait the Z finish
					{
						if ( m_bComplete_Z == FALSE )
						{
							Z_Sync();
							TakeTime(Z2);
						}
					}
				}

				// Z2 must be completed here
				m_qSubOperation = WAIT_WT_READY_Q;
				s_bUpdateNVRAM = TRUE;
			}
			break;

		case WAIT_WT_READY_Q:
			DisplaySequence("BH - Wait WT Ready");

			lWftBlkRealign = (LONG)(*m_psmfSRam)["WaferTable"]["WTReady"];
			if( IsOnlyRegionEnd() )
			{
				pUtl->RegionOrderLog("wait wt ready q, region end, to prepick and wait wt q");
				lWftBlkRealign = 1;
			}
			// Check whether Z is completed
			if (m_bComplete_T == FALSE)
			{
				if ( T_IsComplete() == TRUE )
				{
					TakeTime(T2);
					m_bComplete_T = TRUE;
				}
			}

			(*m_psmfSRam)["BondHead"]["AtPrePickAndWftReady"] = 0;
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitWTReady())
			{
				SetConfirmSrch(FALSE);		
				SetWTReady(FALSE);
				SetEjectorReady(FALSE);
				SetDieBondedForWT(FALSE);
				m_dWaitBTStartTime = GetTime();			//v2.93T2
				m_qSubOperation = WAIT_BT_START_MOVE_Q;
				(*m_psmfSRam)["BondHead"]["AtPrePickAndWftReady"] = 1;
				s_dWftComp = GetTime();
			}
			else if (WaitBadDieForT())
			{
				SetBadDieForT(FALSE); 
				SetEjectorReady(FALSE, "Wait WT Ready @ Bad Die");	
				TakeTime(BD);
				OpBadDie(FALSE);
				SetEjectorReady(TRUE, "Wait WT Ready @ Bad Die");
				SetPickVacuum(FALSE);			//v3.82
				StartTime(GetRecordNumber());	// Restart the time
				OpUpdateDieIndex();				// Update the die index
				m_qSubOperation = WAIT_WT_Q;
				s_bUpdateNVRAM = TRUE;
			}
			else if (WaitConfirmSrch())
			{
				CMSLogFileUtility::Instance()->WT_GetIdxLog("bond arm to prepick while multiple srch or confirm srch");
				SetConfirmSrch(FALSE);
				SetEjectorReady(FALSE, "Wait WT Ready @ Confirm search");	
				OpBadDie(TRUE);						// Treat as bad die case	//BH-T to PRE-PICK
				SetEjectorReady(TRUE, "Wait WT Ready @ Confirm search");
				OpUpdateDieIndex();				// Update the die index
				SetBhToPrePick(TRUE);			// Tell WPR that BH is now on PRE-PICK & go for CONFIRM-SEARCH
				m_qSubOperation = WAIT_WT_Q;
				s_bUpdateNVRAM = TRUE;
				CMSLogFileUtility::Instance()->WT_GetIdxLog("bond arm at prepick");
			}
			else if( lWftBlkRealign==1 )
			{
				CString szTemp;
				szTemp = "BH at pick above, wait WT ready, wft blk realign, move to prepick";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
				(*m_psmfSRam)["WaferTable"]["WTReady"] = 0;
				SetEjectorReady(FALSE, "Wait WT Ready @ Wft Blk Realign");	
				OpBadDie(TRUE);		// Treat as bad die case	//BH-T to PRE-PICK
				SetEjectorReady(TRUE, "Wait WT Ready @ Wft Blk Realign");
				m_qSubOperation = WAIT_WT_Q;
				s_bUpdateNVRAM = TRUE;
				szTemp = "BH at prepick , into Wait WT q, for wft blk realign";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
			}
			else
			{	
				if ( s_bUpdateNVRAM == TRUE )
				{
					SetBackupNVRam(TRUE);
					s_bUpdateNVRAM = FALSE;
				}
				
			}
			break;


		case WAIT_BT_START_MOVE_Q:
			DisplaySequence("BH - Wait BT Start Move");
			if (m_bStop)			// For Bin Loader
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if ( WaitBTStartMove() )		//v4.28	
			{
				if (m_bDLAChangeGrade == TRUE)
				{
					m_bDLAChangeGrade = FALSE;
				}

				SetBTStartMove(FALSE);
				m_qSubOperation = PICK_Z_DOWN_Q;
			}
			else
			{
				if( IsOnlyRegionEnd() )
				{
					CheckCoverOpenInAuto("Regn End");
					SetPickVacuum(FALSE);		//v3.82
					T_Sync();
					T_MoveTo(m_lPrePickPos_T);
					SetEjectorReady(FALSE);
					SetWTReady(FALSE);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					m_qSubOperation = HOUSE_KEEPING_Q;
					pUtl->RegionOrderLog("wait bt start move q, region end, to prepick and house keeping");
				}
				if (IsBLEnable() == TRUE && IsBTChngGrd())
				{
					WftToSafeLog("6 inch wft bh to wait bt change grade");
					(*m_psmfSRam)["BinTable"]["Change Grade"]  = FALSE;
					CheckCoverOpenInAuto("Change Grade");
					T_MoveTo(m_lPrePickPos_T);
					SetPickVacuum(FALSE);		//v3.82
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

					WftToSafeLog("BondHead set to prepick position");

					m_qSubOperation = WAIT_BT_CHANGE_GRADE;
				}
				else 
					Sleep(1);	//v2.93T2
			}
			break;

		case WAIT_BT_CHANGE_GRADE:

			DisplaySequence("BH - Wait BT Change Grade");
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if (WaitBTStartMove())
			{
				OpUpdateMachineCounters2();		//v3.76	
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
			}
			else
			{
				Sleep(1);
			}

			break;

		case PICK_Z_DOWN_Q:
			DisplaySequence("BH - Pick Z Down");
			lSPCResult = (*m_psmfSRam)["BondPr"]["PostBondResult"];
			if (m_bStop)
			{				
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			else if ( lSPCResult != 1 )
			{
				m_qSubOperation = POSTBOND_FAIL_Q;
				break;
			}
			else if ( AllowMove() == TRUE )
			{
				// Check whether it is suitable time for Collet Jam check and Move Z
				if ( m_bComplete_T == FALSE )
				{
					s_dbEnd = GetTime();
					lTemp = (LONG)(s_dbEnd - s_dbStart);

					if (bNewCT)			//v2.83T7
					{
						if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )		//v3.79
						{
							lDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z - lTemp - 3 + m_lHeadPrePickDelay;
							m_bMoveFromPrePick = FALSE;
						}
						else
							lDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z - lTemp - 3;
					}
					else
						lDelay = m_lTime_T + lHeadPickDelay - m_lPickTime_Z - lTemp;

					if( IsLayerPicking() && m_lTCUpHeatingTime>=0 )
					{
						lDelay = 0;
					}

					if ( (lDelay > 0) )	//v4.28
					{
						Sleep(lDelay);
					}
				}

				//Check BinFull
				if ( IsBinFull() == TRUE )
				{
					(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					m_qSubOperation = WAIT_BIN_FULL_Q;
					break;
				}

				if (m_lAutobondTimeInMin > 0)
				{
					LONG lAutobondCurrTime = (LONG)GetTime();
					LONG lDuration = m_lAutobondTimeInMin * 60 * 1000;		// min to msec
					LONG lCurrTotalTime = m_lAutobondTotalTime + (lAutobondCurrTime - m_lAutobondStartTime);

					if ( (lCurrTotalTime) >= lDuration )
					{
						CString szTemp;
						szTemp.Format("Autobond alarm timeout by %d minutes; please check quality.", m_lAutobondTimeInMin);		//v2.83T10
						SetStatusMessage(szTemp);
						HmiMessage_Red_Yellow(szTemp);
						m_lAutobondTotalTime = 0;	
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				if( IsLowVacuumFlow() )
				{
					SetAlert_Red_Yellow(IDS_BH_LOW_VACUUM_FLOW);
					SetErrorMessage("Machine low vacuum flow in auto");
					SetStatusMessage("Machine low vacuum flow in auto");

					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

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
					SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
					SetErrorMessage("Machine Cover Open");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
                }

				//v4.37T2	//SanAn
				if (!OpCheckValidAccessMode(TRUE))	
				{
					SetStatusMessage("Access mode Validated (SanAn)");
					SetErrorMessage("Access mode Validated (SanAn)");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if(!OpCheckStartLotCount())
				{
					SetStatusMessage("CheckStartLotCount");
					SetErrorMessage("CheckStartLotCount");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				//v3.83		//Cree China PreBond Waffle Pad alignment
				bPreBond		= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];
				bPreBondDone	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondDone"];
				if (bPreBond)
				{
					if (!bPreBondDone)	//Must check PreAlign result before BH down to PICK level
					{
						if ( m_bComplete_T == FALSE )
						{
							T_Sync();
							TakeTime(T2);
						}
						Sleep(10);
						break;
					}
					(*m_psmfSRam)["BinTable"]["PreBondDone"] = FALSE;
				}

				// Check whether the collet is jam
				if (CMS896AApp::m_lCycleSpeedMode == 0)		//v3.61
				{
					if ((m_bCheckColletJam == TRUE) && (m_bPickVacuumOn == TRUE) && (m_bFirstCycle == TRUE))
					{
						Sleep(BH_MIN_CHECK_COLLET_TIME);
						m_bFirstCycle = FALSE;
					}
				}

				TakeTime(CJ);		// Take Time
				if (!m_bFirstCycle)
				{
					for ( j=0; j < m_ulColletJam_Retry; j++ )	//v2.58
					{
						if ( OpCheckColletJam() == FALSE )
						{
							break;
						}
						TakeTime(CJO);
						OpColletJam();
					}
				}

				// Remark: Retry count is zero (default) means alarm if collet-jam	//clean once time and no quit
				if (!m_bFirstCycle && (m_ulColletJam_Retry == 0) && (OpCheckColletJam() == TRUE) )
				{
					(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					m_lCJTotalCount1++;	//4.54T15
					(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
					// Collet Jam and Quit
					SetAlert_Red_Yellow(IDS_BH_COLLET_JAM);
					SetErrorMessage("1# Collet is Jammed");

					CheckCoverOpenInAuto("CJ stop");

					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if ( !m_bFirstCycle && (OpCheckColletJam() == TRUE) )	//v2.58
				{
					(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					m_lCJTotalCount1++;	//4.54T15
					(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
					// Collet Jam and Quit
					SetAlert_Red_Yellow(IDS_BH_COLLET_JAM);
					SetErrorMessage("2# Collet is Jammed");

					CheckCoverOpenInAuto("CJ 2 stop");

					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					TakeTime(Z3);		// Take Time

					if ( IsOnlyRegionEnd() )
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						pUtl->RegionOrderLog("REGN BH Z NOT DN (REGION END)");
						break;
					}

					if( IsBHZOnVacuumOnLayer() )
					{
						SetPickVacuum(FALSE);
					}
					if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
					{
						AC_Z1_MoveTo(m_lPickLevel_Z, SFM_NOWAIT);
						if( pUtl->GetPrescanAreaPickMode() )
						{
							m_nPickListIndex--;
						}
					}

					m_bComplete_Z = FALSE;
					s_dbStart = GetTime();				//v2.83T7
					m_qSubOperation = EJ_UP_Q;
					m_bFirstCycle = FALSE;	//v3.61
				}

				//andrew
				if (m_bEnableAirFlowTest)
				{
					CString szMsg;
					szMsg.Format("MD=%d  JC=%d  T=%d", m_lAirFlowMdTime, m_lAirFlowJcTime, m_lTTravelTime);
					BH_LOG(szMsg);
				}
			}
			break;

		case EJ_UP_Q:
			DisplaySequence("BH - Ej Up");
			if ( AllowMove() == TRUE )
			{
				lEjectorLvl = m_lEjectLevel_Ej;
				if ( IsEnableSyncMove() )
				{
					lEjectorLvl = m_lEjectLevel_Ej - m_lSyncDistance;
				}

				// Calculate the compensation for wear out of ejector pin
				lCompen_Ej = OpCalCompen_Ej();

				if( IsLayerPicking() && m_lTCUpHeatingTime>=0 )
				{
					TakeTime(EJ1);		// Take Time
					SetEjectorVacuum(TRUE);
					Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
					m_bComplete_Ej = FALSE;
					s_dbStart = GetTime();

					if ( m_bComplete_T == FALSE )
					{
						T_Sync();
						TakeTime(T2);
					}
					Ej_Sync();
					TakeTime(EJ2);

					m_qSubOperation = WAIT_PICK_DELAY_Q;
					break;
				}

				if (bNewCT)			//v2.83T7
				{
					s_dbEnd = GetTime();
					lTemp = (LONG)(s_dbEnd - s_dbStart);
					lDelay = m_lPickTime_Z + m_lEjectorUpDelay - m_lTime_Ej - lTemp;

					if ( (lDelay > 0) )	// T complete before Ej start	//v4.28
					{
						//v3.66
						LONG lRemainTimeT = m_lPickTime_Z - lHeadPickDelay - lTemp;
						if (lRemainTimeT < lDelay)
						{
							if ( m_bComplete_T == FALSE )
							{
								T_Sync();
								TakeTime(T2);
							}

							s_dbEnd = GetTime();
							lTemp = (LONG)(s_dbEnd - s_dbStart);
							lDelay = m_lPickTime_Z + m_lEjectorUpDelay - m_lTime_Ej - lTemp;
							if (lDelay > 0)
								Sleep(lDelay);
						}
						else
							Sleep(lDelay);
					}

					TakeTime(EJ1);		// Take Time
					Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
					m_bComplete_Ej = FALSE;
					s_dbStart = GetTime();

					if ( m_bComplete_T == FALSE )
					{
						T_Sync();
						TakeTime(T2);
					}

					if (m_bComplete_Z == FALSE)
					{
						if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
							(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
						{
							Z_Sync();

							/*if (m_bEnableOpenDacBondOption == TRUE)
							{
								EnableBHZOpenDacWithFlexiControl(TRUE);

								Z_UpdateStepMoveProf();
								Z_StepMove(SFM_NOWAIT);
							}*/

							TakeTime(Z4);
						}
						else
						{
							Z_Sync();
							TakeTime(Z4);
						}
					}

					s_dbEnd = GetTime();
					lTemp = (LONG)(s_dbEnd - s_dbStart);
					if (m_lTime_Ej > (lTemp+3))
						Sleep(m_lTime_Ej - lTemp - 3);

					TakeTime(EJ2);
				}
				else
				{
					if ( m_lTime_Ej > m_lEjectorUpDelay )	// Ej start before Z end
					{
						lDelay = lHeadPickDelay + m_lEjectorUpDelay - m_lTime_Ej;
						if ( lDelay > 0 )	// T complete before Ej start
						{
							if ( m_bComplete_T == FALSE )
							{
								T_Sync();
								TakeTime(T2);
							}
							Sleep(lDelay);
						}
						else
						{
							// Wait delay before Ej up
							lDelay = m_lPickTime_Z + m_lEjectorUpDelay - m_lTime_Ej;
							if ( lDelay > 0 )
								Sleep (lDelay);
						}

						TakeTime(EJ1);		// Take Time
						Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
						m_bComplete_Ej = FALSE;

						if ( m_bComplete_T == FALSE )
						{
							T_Sync();
							TakeTime(T2);
						}
						
						Z_Sync();
						TakeTime(Z4);
					}
					else	// Ej start after Z completed
					{
						// T2 must be completed before Z4 (since HeadPickDelay >= 0)
						if ( m_bComplete_T == FALSE )
						{
							T_Sync();
							TakeTime(T2);
						}
						
						Z_Sync();
						TakeTime(Z4);
						lDelay = m_lEjectorUpDelay - m_lTime_Ej;
						if ( lDelay > 0 )
							Sleep(lDelay);

						TakeTime(EJ1);		// Take Time
						Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
						m_bComplete_Ej = FALSE;
					}
				
					Ej_Sync();
					TakeTime(EJ2);
				}

				m_qSubOperation = WAIT_PICK_DELAY_Q;
			}
			break;

		case WAIT_PICK_DELAY_Q:
			DisplaySequence("BH - Wait Pick Delay");

			if( IsEnableSyncMove() )
			{
				Z_Sync();
				Ej_Sync();
				GetEncoderValue();
				CString szMsg;
				szMsg.Format("syn move before pick delay T %d, Z %d, EJ %d", m_lEnc_T, m_lEnc_Z, m_lEnc_Ej);
				CMSLogFileUtility::Instance()->BH_DieCounterLog(szMsg);
			}

			if (IsLayerPicking())
			{
				if (m_lTCUpPreheatTime >= 0)
				{
					SetPRStart(FALSE);
				}

				// when pick down off already, do it once more
				if (IsBHZOnVacuumOnLayer())
				{
					SetPickVacuum(FALSE);
				}

				if (m_lTCUpHeatingTime >= 0)
				{
					Sleep(m_lTCUpHeatingTime);
					TakeTime(Z3);
					AC_Z1_MoveTo(m_lPickLevel_Z);
					Z_Sync();
					TakeTime(Z4);
				}
				if (IsBHZOnVacuumOnLayer())
				{
					Sleep(m_lTCDnOnVacDelay);
					SetPickVacuum(TRUE);
				}
				if (m_lPickDelay > 0)
				{
					Sleep(m_lPickDelay);
				}
				if (m_lTCUpHeatingTime <0)
				{
					SetEjectorVacuum(TRUE);
					Sleep(labs(m_lTCUpHeatingTime));
				}
			}
			else
			{
				if (m_lPickDelay > 0)
				{
					Sleep(m_lPickDelay);
				}
			}

			m_bBhInBondState = FALSE;

			//Collet-Hole shift measurement only	//v3.34
			if (m_bPreBondAtPick)	
			{
				m_lPreBondAtPickCount++;
				if (m_lPreBondAtPickCount >= 30)
				{
					if (bNewCT)	
						Sleep(50);
					SetBhTReadyForWPR();
					Sleep(100);
					m_lPreBondAtPickCount = 1;
				}
			}

			//Update Ejector & Collet counter
			OpUpdateMachineCounters();

			m_qSubOperation = MOVE_Z_PICK_UP_Q;
			if ( IsEnableSyncMove() )
			{
				m_qSubOperation = MOVE_Z_EJ_SYNC_Q;
			}
			break;

		case MOVE_Z_EJ_SYNC_Q:
			DisplaySequence("BH - Move Z & Ejector Sync");

			if ( AllowMove() == TRUE )
			{
				OpSyncMove();
				m_qSubOperation = MOVE_Z_PICK_UP_Q;
			}
			break;

		case MOVE_Z_PICK_UP_Q:
			DisplaySequence("BH - Move Z Pick Up");

			if ( AllowMove() == TRUE )
			{
				SetDiePicked(TRUE);	
				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = TRUE;
				TakeTime(Z5);		// Take Time

				/*if (m_bEnableOpenDacBondOption == TRUE)
				{						
					EnableBHZOpenDacWithFlexiControl(FALSE);
				}*/

				if (m_lEjectorDownDelay < 0)
				{
					Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
				}

				if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
					(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
				{
					CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_STATIC_CONTROL_OBW_Z, &m_stBHAxis_Z);
					CMS896AStn::MotionSelectControlType(BH_AXIS_Z, HP_CTRL_GFL_PD_INTG, 2, &m_stBHAxis_Z);
				}
				
				AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_NOWAIT);

				if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
					(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
				{
					CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_DYNAMIC_CONTROL_OBW_Z, &m_stBHAxis_Z);
					CMS896AStn::MotionSelectControlType(BH_AXIS_Z, HP_CTRL_GFL_FFC_PD_INTG, 2, &m_stBHAxis_Z);
				}

				m_bComplete_Z = FALSE;
				m_qSubOperation = MOVE_EJ_THEN_T_Q;
			}
			break;

		case MOVE_EJ_THEN_T_Q:
			DisplaySequence("BH - Move Ej then T");

			if ( AllowMove() == TRUE )
			{
				// Move Ej
				if (m_lEjectorDownDelay <= lArmBondDelay)		//v3.66
				{
					if (m_lEjectorDownDelay > 0)
					{
						Sleep(m_lEjectorDownDelay);		//Linear Ej
					}

					TakeTime(EJ3);		// Take Time
					Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
					m_bComplete_Ej = FALSE;
					s_dbStart = GetTime();
				}

				// Check whether need to wait Z completed before T move
				if ( (lArmBondDelay > m_lPickTime_Z) && (m_bComplete_Z == FALSE) )	//v4.28	
				{
					Z_Sync();
					TakeTime(Z6);
					Sleep(lArmBondDelay - m_lPickTime_Z);
				}
				else
				{
					//v3.02T3	//v3.66	//Need to remove extra time from Ej Down Dleya here
					if ((m_lArmBondDelay - 2 - m_lEjectorDownDelay) > 0)
						Sleep(m_lArmBondDelay - 2 - m_lEjectorDownDelay);
				}

				m_qSubOperation = MOVE_T_Q;
			}
			break;


		case MOVE_T_Q:		//v2.99T1
			DisplaySequence("BH - T Move");
			lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];

			if ( AllowMove() == TRUE )
			{
				if (lMotionTime_BT > 200)		//v3.01T1
				{
					if (WaitBTReady())
					{
						// Move T
						T_Sync();			//v3.61
						TakeTime(T3);		// Take Time
						
						if  (IsZMotorsEnable() == FALSE)
						{
							SetMotionCE(TRUE, "BondHead module is off power");
							SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
							break;
						}
						
						m_ctBHTAutoCycleLastMoveTime = CTime::GetCurrentTime();
						T_MoveTo(m_lBondPos_T, SFM_NOWAIT);
						m_lTime_T = m_lBondTime_T;		//v3.61
						s_dbStart2 = GetTime();			//v2.83T7

						if (m_bEnableAirFlowTest)
						{
							m_lTMoveTime = m_lAirFlowStartTime = (LONG) timeGetTime();
							m_bStartAirFlowMdCheck	= TRUE;
						}

						m_bComplete_T = FALSE;
						m_qSubOperation = WAIT_Z_EJ_COMPLETE_Q;
					}
				}
				else
				{
					// Move T
					//T_Sync();			//v3.61
					TakeTime(T3);		// Take Time

					if (IsZMotorsEnable() == FALSE)
					{
						SetMotionCE(TRUE, "BondHead module is off power");
						SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
						break;
					}

					m_ctBHTAutoCycleLastMoveTime = CTime::GetCurrentTime();
					T_MoveTo(m_lBondPos_T, SFM_NOWAIT);
					m_lTime_T = m_lBondTime_T;		//v3.61
					s_dbStart2 = GetTime();			//v2.83T7

					if (m_bEnableAirFlowTest)
					{
						m_lTMoveTime = m_lAirFlowStartTime = (LONG) timeGetTime();
						m_bStartAirFlowMdCheck	= TRUE;
					}

					m_bComplete_T = FALSE;
					m_qSubOperation = WAIT_Z_EJ_COMPLETE_Q;
				}

				//v3.66
				if (m_lEjectorDownDelay > lArmBondDelay)
				{
					if ( (m_lEjectorDownDelay - lArmBondDelay - 2) > 0 )
						Sleep(m_lEjectorDownDelay - lArmBondDelay - 2);		//Linear Ej

					TakeTime(EJ3);
					Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
					m_bComplete_Ej = FALSE;
					s_dbStart = GetTime();
				}
			}
			break;

		case WAIT_Z_EJ_COMPLETE_Q:
			DisplaySequence("BH - Wait Z and Ej complete");
			// Check whether Z complete first
			if ( m_lTime_Ej > m_lPickTime_Z )
			{
				if ( m_bComplete_Z == FALSE )
				{
					Z_Sync();		// Wait Z complete
					TakeTime(Z6);
				}
			
				// Allow WT to move earlier than Ej completed
				s_dbEnd = GetTime();
				lDelay = m_lTime_Ej - (LONG)(s_dbEnd - s_dbStart) - m_lEjReadyDelay;
				if ( lDelay > 0 )
					Sleep(lDelay);
				SetEjectorReady(TRUE);

				if (!bNewCT)		//v2.83T7	//v4.28
					Ej_Sync();				// Wait Ej complete
				TakeTime(EJ4);
				lDelay = m_lTime_Ej;
			}
			else
			{
				// Allow WT to move earlier than Ej completed
				s_dbEnd = GetTime();
				lDelay = m_lTime_Ej - (LONG)(s_dbEnd - s_dbStart) - m_lEjReadyDelay;
				if ( lDelay > 0 )
					Sleep(lDelay);
				SetEjectorReady(TRUE);

				if (!bNewCT)	//v4.28
					Ej_Sync();
				TakeTime(EJ4);
				
				if ( m_bComplete_Z == FALSE )
				{
					Z_Sync();
					TakeTime(Z6);
				}
				lDelay = m_lPickTime_Z;
			}
			if( IsLayerPicking() )
			{
				SetEjectorVacuum(FALSE);
			}
			m_qSubOperation = WAIT_BT_READY_Q;
			break;

		case WAIT_BT_READY_Q:
			DisplaySequence("BH - Wait BT Ready");
			if (WaitBTReady() && (IsBinFull() == FALSE))
			{
				SetBTReady(FALSE);
				m_qSubOperation = MOVE_Z_TO_BOND_Q;
			}
			else if ( IsBinFull() == TRUE )
			{
				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
				m_qSubOperation = WAIT_BIN_FULL_Q;
			}	
			if( IsAOIOnlyMachine() )
			{
				if( m_bStop )
				{
					DisplaySequence("BH - wait BT ready stopped by other station");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;

		case MOVE_Z_TO_BOND_Q:
			DisplaySequence("BH - Wait Pick Z");
			
			if ( AllowMove() == TRUE )
			{
				if (bNewCT)	
				{
					s_dbEnd = GetTime();
					lTemp = (LONG)(s_dbEnd - s_dbStart2);

					//v3.62						
					LONG lExtraDelay = 0;
					if (CMS896AApp::m_lCycleSpeedMode > 0)	//160ms-Mode
					{	
						//** Insert extra ejector-up delay if sorting direction may cause die collision on BOND side **//
						LONG lBtSortDir = (*m_psmfSRam)["BinTable"]["SortDirection"];
						if ((lBtSortDir == 2) || (lBtSortDir == 3))		//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
						{
							//lExtraDelay = 5;
						}
					}

					lDelay = m_lTime_T + (lHeadBondDelay + lExtraDelay) - m_lBondTime_Z - lTemp - 3;
				}
				else
				{					
					s_dbEnd = GetTime();
					lTemp = (LONG)(s_dbEnd - s_dbStart2);
					lDelay = m_lTime_T + lHeadBondDelay - m_lBondTime_Z - lTemp;
				}	
				if ( (lDelay > 0) )	//v4.28	
					Sleep(lDelay);

				TakeTime(MD);		// Take Time

				//andrew
				if (m_bEnableAirFlowTest)
				{
					m_lTMoveTime = ((LONG) timeGetTime()) - m_lTMoveTime;
					m_lTTravelTime = m_lTMoveTime;
				}

				if ( OpCheckMissingDie() == TRUE )
				{
					//v3.70T3	//PLLM special feature
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);

					LONG lUserConfirm = glHMI_STOP;
					if (bPLLMSpecialFcn)
					{
						//v2.83T70
						lUserConfirm = HmiMessage_Red_Green("Missing die encountered!  Continue PLACE die?", "AUTOBOND", glHMI_MBX_CONTINUESTOP);

						if (lUserConfirm == glHMI_CONTINUE)
						{
							//LogBondHeadStatus("Missing Die - CONTINUE");
						}
						else
						{
							lUserConfirm = glHMI_STOP;
						}
					}

					if (lUserConfirm == glHMI_STOP)
					{
						TakeTime(MDO);
						IncreaseMissingDieCounter();

						if ( m_pulStorage != NULL )
						{
							*(m_pulStorage + BH_MISSINGDIE_OFFSET) = m_ulMissingDieCount;
							*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = m_ulCurrWaferMissingDieCount;
						}

						LONG lMissingDieStop = 0;
						m_lMDCycleCount1++;
						m_lMDTotalCount1++;
						(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"] = m_lMDTotalCount1; //4.54T15
						if( m_lMDCycleLimit>0 && m_lMDCycleCount1>=m_lMDCycleLimit )
						{
							lMissingDieStop = 3;
						}
						// If retry is 0, simply stop
						if ( m_lMD_Count == 0 )		
						{
							lMissingDieStop = 1;
						}
						if( lMissingDieStop>0 )
						{
							
							if( lMissingDieStop==1 )
							{
								SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED);
								SetErrorMessage("Missing Die Detected");
							}
							else
							{
								SetAlert_Red_Yellow(IDS_BH_MD_ACCUM_BHZ1);
								SetErrorMessage("Accumulated Missing Die Detected");
							}

							CheckCoverOpenInAuto("MD stop");

							// Treat as no die picked
							T_MoveTo(m_lCleanColletPos_T);		// Move T to Blow position
							(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
							SetDiePicked(FALSE);
							TakeTime(PV2);						// Take Time
							SetPickVacuum(FALSE);				// Release the pick vacuum
							Sleep(1000);
 							(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
							m_qSubOperation = HOUSE_KEEPING_Q;	
						}
						else
						{
							// Wait T complete
							if ( m_bComplete_T == FALSE )
							{
								T_Sync(); 
								TakeTime(T4);
							}

							// If retry is -ve, move back the Wafer Table
							if ( m_lMD_Count < 0 )
							{
//								SetEjectorReady(FALSE);		// Clear the ejector ready event
								SetPRStart(FALSE);			// Clear the PR start event
								SetMoveBack(TRUE);			// Trigger Wafer Table to move back
	 							(*m_psmfSRam)["BondHead"]["MoveBackForMissingDie"] = TRUE;
								m_lMD_Count++;
							}
							else
							{
								m_lMD_Count--;
							}
							SetDiePicked(FALSE);
 							(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;

							CheckCoverOpenInAuto("MD blow");
							T_Profile(CLEAN_COLLET_PROF);		// Change T profile
							T_MoveTo(m_lCleanColletPos_T);		// Move to clean collet position

							TakeTime(PV2);						// Take Time
							SetPickVacuum(FALSE);
							SetStrongBlow(TRUE);
							Sleep(m_lHighBlowTime);				// Cleaning 
							SetStrongBlow(FALSE);
							Sleep(1000);	
			
							if (IsZMotorsEnable() == FALSE)
							{
								SetMotionCE(TRUE, "BondHead module is off power");
								SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
								break;
							}

							CheckCoverOpenInAuto("MD after blow");
							
							T_MoveTo(m_lBondPos_T);
							T_Profile(NORMAL_PROF);

							if ( m_lBondDelay > 0 )
								Sleep(m_lBondDelay);

							dCurrentTime = GetTime();
							m_dLastTime = dCurrentTime;

							m_qSubOperation = BOND_Z_TO_SWING_Q;
						}

						break;
					}
				}

				m_lMD_Count = m_lMissingDie_Retry;	// Reset the local count
				TakeTime(Z7);		// Take Time

				LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);
				AC_Z1_MoveTo(lBondZ1, SFM_NOWAIT, TRUE);

				m_bComplete_Z = FALSE;
				
				SetBackupNVRam(TRUE);
				// Wait T complete

				if ( m_bComplete_T == FALSE )
				{
					T_Sync();
					TakeTime(T4);
				}

				Z_Sync();

				TakeTime(Z8);
				m_qSubOperation = WAIT_BOND_DELAY_Q;
			}
			break;

		case WAIT_BOND_DELAY_Q:
			DisplaySequence("BH - Wait Bond Delay");
			SetDiePicked(FALSE);
			TakeTime(PV2);						// Take Time
			SetPickVacuum(FALSE);				// Release the pick vacuum

			//if (CMS896AStn::m_bWafflePadBondDie == FALSE)
			if (CMS896AStn::m_bEnableWafflePadBonding == FALSE)
			{
				if (m_bNeutralVacState)				//v3.81
					SetStrongBlow(TRUE);			//to NEUTRAL state
			}
			m_bBhInBondState = TRUE;
			(*m_psmfSRam)["BondHead"]["BondDelay"] = m_lBondDelay;

			//if (m_bEnableOpenDacBondOption)		//Changed to OPEN-DAC control	//andrew
				//EnableBHZOpenDac(TRUE);

			if ( m_lBondDelay > 0 )
				Sleep(m_lBondDelay);
			
			//if (CMS896AStn::m_bWafflePadBondDie == FALSE)
			if (CMS896AStn::m_bEnableWafflePadBonding == FALSE)
			{
				//v3.81
				if ( m_bNeutralVacState && (m_lBondDelay > 0) )
				{
					SetStrongBlow(FALSE);	//to WEAKBLOW
					Sleep(m_lBondDelay);
				}
			}
			else
			{
				SetStrongBlow(TRUE);	//to NEUTRAL state
				Sleep(m_lBondDelay);
			}

			OpCheckThermalTest();

			if (m_bPostBondAtBond)				// Perform postbond of collet hole at BOND posn		//v2.60
			{
				m_bPostBondAtBondCount++;
				if (m_bPostBondAtBondCount > 40)
				{
					if (bNewCT)					//v2.83T7
						Sleep(100);

					SetBhTReadyForBPR();
					Sleep(50);
					m_bPostBondAtBondCount = 0;
				}
			}

			// Find the cycle time
			dCurrentTime = GetTime();
			if ( m_dLastTime > 0.0 )
			{
				m_dCycleTime = fabs(dCurrentTime - m_dLastTime);

				if ( m_dCycleTime > 99999 )
				{
					m_dCycleTime = 10000;
				}

				// Calculate the average cycle time
				m_dAvgCycleTime = (m_dAvgCycleTime * m_ulCycleCount + m_dCycleTime) / (m_ulCycleCount + 1);
				m_ulCycleCount++;

                // 3501
                SetGemValue("AB_CurrentCycleTime",  m_dCycleTime);
                SetGemValue("AB_AverageCycleTime",  m_dAvgCycleTime);
                SetGemValue("AB_CycleCounter",      m_ulCycleCount);
                // 7704
                // SendEvent(SG_CEID_AB_RUNTIME, FALSE);
			}
			m_dLastTime = dCurrentTime;
			OpUpdateBondedUnit(m_dCycleTime);			//Update bonded unit

			SetDieBonded(TRUE);			
			SetDieBondedForWT(TRUE);
			OpUpdateDieIndex();					// Update the die index

			//SetStatusMessage("Die Bonded");
			if  (IsZMotorsEnable() == FALSE)
			{
				SetMotionCE(TRUE, "BondHead module is off power");
				SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
				break;
			}

			m_qSubOperation = BOND_Z_TO_SWING_Q;
			break;

		case WAIT_BIN_FULL_Q:
			DisplaySequence("BH - Wait Bin Full");
			// Wait for bin full alert issued by Bin Table Station
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else
			{
				Sleep(1);
			}
			break;

		case POSTBOND_FAIL_Q:
			DisplaySequence("BH - SPC failed");
			lSPCResult = (*m_psmfSRam)["BondPr"]["PostBondResult"];
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
				SetAlert_Red_Yellow(IDS_BH_SPC_DEFECT);		// Set Alarm
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
				SetAlert_Red_Yellow(IDS_BH_SPC_FAIL);		// Set Alarm
				SetErrorMessage("Postbond Detect Error");
				break;
			}
			m_qSubOperation = HOUSE_KEEPING_Q;
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("BH - House Keeping Q");
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

			if (bNoMotionHouseKeeping == FALSE)
			{
				// Make sure the motion is complete first
				// Wait Z complete
				if ( m_bComplete_Z == FALSE )
				{
					Z_Sync();
				}					
				// Wait Ej complete
				if ( m_bComplete_Ej == FALSE )
				{
					Ej_Sync();
				}					
				// Wait T complete
				if ( m_bComplete_T == FALSE )
				{
					T_Sync();
				}

				//SetPickVacuum(FALSE);			// Turn-off the pick vacuum		//v2.77
				if (Z_IsPowerOn())			//v3.13T3
				{
					AC_Z1_MoveTo(0, SFM_WAIT);		// Move Z to Zero

					//v4.46T20
					CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
					if ( (pApp->GetCustomerName() == "Cree") )
					{
						if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= 4100) )
						{
							Sleep(100);
							Z_Move(pApp->GetBHZ1HomeOffset());
						}
					}
				}

				Ej_MoveTo(0, SFM_WAIT);			// Move Ej to Zero
			}


			LogCycleStopState("BH - MoveArm");

			bWaferEnd	= IsWaferEnded();
			bRegionEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"];	// xuzhijin_region_end move to clean collet position
			bColletFull = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ColletFull"];

			if (pSRInfo->IsRegionEnding())
			{
				CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
				if (pUtl->GetPrescanRegionMode())
				{
					m_ulEjCountForSubRegionKOffset = 0;
					pSRInfo->SetRegionEnding(FALSE);
					SaveScanTimeEvent("BH: REGION END reset k offset house keeping");
				}
			}

			if (bNoMotionHouseKeeping == FALSE)
			{
				if (Z_IsPowerOn() && T_IsPowerOn())			//v3.13T3
				{
					if( IsPrescanning()==FALSE )
						CheckCoverOpenInAuto("House Keeping");
					if ( (bWaferEnd == TRUE) || (bColletFull == TRUE) || (bRegionEnd == TRUE) )
					{
						T_MoveTo(m_lCleanColletPos_T, SFM_WAIT);	// Move T to Blow position
					}
					else
					{
						T_MoveTo(m_lPrePickPos_T, SFM_WAIT);		// Move T to Pre-Pick
					}
				}

				SetPickVacuum(FALSE);		// Turn-off the pick vacuum after T-Move done		//v2.77
			}
			CycleEnableDataLog(FALSE, FALSE, TRUE);

			CMS896AStn::m_lBondHeadAtSafePos = 1;

			LogCycleStopState("BH - UpdateDieCounter");
			UpdateDieCounter();			//Update die counter in HMI

			LogCycleStopState("BH - LogWaferInfo");
			LogWaferInformation(FALSE);	//Update current wafer counter

			LogCycleStopState("BH - MachineStat");
			SaveMachineStatistic();		//update machine statistic	

			// Acknowledge other stations to stop
			StopCycle("BondHeadStn");				
			LogCycleStopState("BH - stop completely");
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
		if ( m_bHeapCheck == TRUE )
		{
			// Check the heap
			INT nHeapStatus = _heapchk();
			if ( nHeapStatus != _HEAPOK )
			{				
				CString szMsg;
				szMsg.Format("BH - Heap corrupted (%d) before BH", nHeapStatus);
				DisplayMessage(szMsg);
				
				LogCycleStopState(szMsg);
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
		SetMotionCE(TRUE, "BondHead Run Operation");
	}
}



VOID CBondHead::CycleOperation()
{
	if (Command() == glAMS_STOP_COMMAND)
	{
		State(STOPPING_Q);
	}
}




