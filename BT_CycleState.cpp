/////////////////////////////////////////////////////////////////
// BT_CycleState.cpp : Cycle Operation State of the CBinTable class
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
#include "BinTable.h"
#include "CycleState.h"
#include "BT_Log.h"
#include "PrescanUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CString g_aszBTState[20] =
	{	"Wait Die Ready",		// WAIT_DIE_READY_Q
		"Index",				// INDEX_Q
		"Wait BPR Latched",		// WAIT_BPR_LATCHED_Q
		"Move Table",			// MOVE_TABLE_Q
		"Wait BT Ready",		// WAIT_BT_READY_Q
		"Wait Die Bonded",		// WAIT_DIE_BONDED_Q
		"Next Die",				// NEXT_DIE_Q
		"Move Back",			// MOVE_BACK_Q
		"Bin Full",				// BIN_FULL_Q
		"BL Change Grade",		// BL_CHANGE_GRADE_Q
		"Wait BL Change",		// WAIT_BL_CHANGE_READY_Q
		"BL Bin Full",			// BL_BIN_FULL_Q
		"Wait BL Bin Full",		// WAIT_BL_BIN_FULL_READY_Q
		"House Keeping"			// HOUSE_KEEPING_Q
		"Move To Unload"		// MOVE_TO_UNLOAD_Q
	};

VOID CBinTable::AutoOperation()
{
	RunOperation();
}

VOID CBinTable::DemoOperation()
{
	RunOperation();
}


VOID CBinTable::RunOperation()
{
	static BOOL   s_bLastErrorIndex = FALSE;
	LONG	lBondTime_Z, lDelay;
	INT		nWaitDelay=0;
	ULONG	ulLastBlk=0, ulBlkInUse=0;
	ULONG	ulBlk = 0;	//v4.59A25
	CString	szMsg, szTemp, szWaferId;
	ULONG	i;
	BOOL	bFirst, bFull, bPicked;
	BOOL	bGradeChanged = FALSE;
	LONG	lStopped = 0;
	LONG	lBHAtPick = 0;
	LONG	lBHAtSafe = 0;
	LONG	bMissingDie = FALSE;
	LONG	lLoopCounter = 0;
	//BOOL	lSubBinChangeGrade = TRUE;
	CString szLog;
	LONG lMoveTime = 0;

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP) )
	{
		DisplaySequence("BT - STOP");
		State(STOPPING_Q);
		return ;
	}

	try
	{
		m_nLastError = gnOK;
		BOOL bBinExArm	= (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["ExChgArmExist"];
		BOOL bDualBuffer= (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["DualBufferExist"];
		BOOL bDualTable	= m_bUseDualTablesOption;	
		BOOL bBinLoader = IsNLEnable();		//IsBLEnable();		//andrewng //2020-0720
		BOOL bEnableBHUplookPr = pAppMod->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
		LONG lPreBondPrDelay = (LONG)(*m_psmfSRam)["BondHead"]["HeadPrePickDelay"];	

		if ( m_bStopBinLoader == TRUE )
			bBinLoader = FALSE;			

		BOOL	bNoMotionHouseKeeping = FALSE;
		if (m_bStopAllMotion == TRUE)
		{
			if (m_qSubOperation != MOVE_TO_UNLOAD_Q)
				m_qSubOperation = HOUSE_KEEPING_Q;

			bNoMotionHouseKeeping = TRUE;
		}

		if (CMS896AStn::m_bAutoGenerateWaferEndFile == TRUE)
		{
			CMS896AStn::m_bAutoGenerateWaferEndFile = FALSE;
			AutoWaferEndFileGenerating();
		}

		// Operation state
		switch (m_qSubOperation)
		{
		case WAIT_DIE_READY_Q:
			
			//if( m_bStop )	{	if (OpGenAllTempFile())	DisplaySequence("BT - stop gen temp file in Wait Die Ready");}
			//else
			DisplaySequence("BT - WAIT_DIE_READY_Q");
			/*if (CMS896AApp::m_bEnableSubBin == TRUE && m_bSubBinFull)
			{
				m_qSubOperation = INDEX_Q;
				break;
			}*/
			if (IsBHStopped())		//v3.67T3
			{
				SetBTReady(TRUE);
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (m_bStop && s_bLastErrorIndex == FALSE)
			{
				LONG lWprFatalErr = (*m_psmfSRam)["WaferPr"]["WprFatalErr"];
				if (lWprFatalErr == 2)
				{
					s_bLastErrorIndex = TRUE;
					m_qSubOperation = INDEX_Q;
					break;
				}
			}


			bFull = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinFull"];
			if( m_bDisableBT )
				bFull = FALSE;

			if ( bFull == TRUE )	// &&  m_bEnableOsramBinMixMap == FALSE)
			{
				SetBTReady(FALSE);
				m_qNextSubOperation = m_qSubOperation;
				m_qSubOperation = BIN_FULL_Q;
				CMSLogFileUtility::Instance()->BL_LogStatus("**** BT: BIN-FULL 1 ****");	//v4.23T1
				SetErrorMessage("BT bin full to change");
				DisplaySequence("BT - bin full to YYYY change at WT die ready q");
				if( m_ulNewBondedCounter>10 )
				{
					m_ulNewBondedCounter = 0;
					SetAutoCycleLog(TRUE);//	HuaMao2	Bin Full 1
				}
				m_stStartChangeTime	= CTime::GetCurrentTime();
				m_unChangeStage	= 2;	// bin full
			}

		//	else if (WaitDieReadyForBT() && (m_bFirstCycleBT || WaitDieInfoUnRead()) )	//	V450X16
			else if (WaitDieReadyForBT() || m_bFirstCycleBT)
			{
				TakeTime(LC3);
				SetDieReadyForBT(FALSE);
				SetBTReady(FALSE);

				/*if (m_bEnableOsramBinMixMap)
				{
					CUIntArray aulSelectedGradeList;
					m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
					UCHAR ucSelectedGrade = aulSelectedGradeList.GetAt(0)-m_WaferMapWrapper.GetGradeOffset();
					szLog.Format("mapgrade:%d",ucSelectedGrade);
					CMSLogFileUtility::Instance()->BL_LogStatus(szLog);

					LONG lPrescanGoodDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];

					if (ucSelectedGrade == m_ucBinMixTypeAGrade)
					{
						if ((lPrescanGoodDie != 0) && (m_lBinMixTypeAQty != lPrescanGoodDie) &&  ((m_lBinMixCount == 0)))
						{
							LONG lDiff = m_lBinMixTypeAQty - lPrescanGoodDie;
							szLog.Format("A:Input Qty not equal to Prescan Good Die Qty,Reset Input Qty to Prescan Good Die Qty.Input Qty:%d,Prescan Qty:%d, Diff:%d",m_lBinMixTypeAQty,lPrescanGoodDie,lDiff);
							HmiMessage(szLog);
							m_lBinMixTypeAQty = lPrescanGoodDie;
						}
					}
					else if(ucSelectedGrade == m_ucBinMixTypeBGrade)
					{
						if( (lPrescanGoodDie != 0) && (m_lBinMixTypeBQty != lPrescanGoodDie) &&  ((m_lBinMixCount == 0)))
						{
							LONG lDiff = m_lBinMixTypeBQty - lPrescanGoodDie;
							szLog.Format("B:Input Qty not equal to Prescan Good Die Qty,Reset Input Qty to Prescan Good Die Qty.Input Qty:%d,Prescan Qty:%d, Diff:%d",m_lBinMixTypeBQty,lPrescanGoodDie,lDiff);
							HmiMessage(szLog);
							m_lBinMixTypeBQty = lPrescanGoodDie;
						}
					}
					OpUpdateOsramStopPoint();
				}*/

				m_qSubOperation = INDEX_Q;
			}
			break;
		
		case INDEX_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in Index");	}
			if (IsBHStopped())		//v3.67T3
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			DisplaySequence("BT - INDEX_Q");
			m_nLastError = OpIndex();
			if (m_nLastError == 1)
			{
				SetDieInfoRead(TRUE);

				m_ulNewBondedCounter++;
				m_qSubOperation = WAIT_BPR_LATCHED_Q;

				OpUpdateDieIndex();
				//if (m_bEnableOsramBinMixMap)
				//{
				//	OpOsramIndex();
				//}
			}
			else if( m_nLastError == -1 )
			{
				Sleep(5);
				DisplaySequence("BT - WT map next die not ready");
			}
			else
			{
				/*if (m_bEnableOsramBinMixMap)
				{
					OpOsramNotIndex();
					break;
				}
				else
				{*/
				// Reset Last Die as ready
				CMSLogFileUtility::Instance()->BL_LogStatus("**** BT: BIN-FULL 2 ****");	//v4.23T1
				m_qNextSubOperation = m_qSubOperation;
				m_qSubOperation = BIN_FULL_Q;
				DisplaySequence("BT - XXXX index bin full 2 to change");
				if( m_ulNewBondedCounter>10 )
				{
					m_ulNewBondedCounter = 0;
					SetAutoCycleLog(TRUE);//	HuaMao2	Bin Full 2
				}
				m_stStartChangeTime	= CTime::GetCurrentTime();
				m_unChangeStage	= 2;	// bin full
				//}
			}
			break;
		
		case WAIT_BPR_LATCHED_Q:
			DisplaySequence("BT - WAIT_BPR_LATCHED_Q");
			if (!WaitBPRLatched())
			{
				SetBTStable(TRUE);
			}
			//if( m_bStop )	{	if (OpGenAllTempFile())	DisplaySequence("BT - stop gen temp file in Wait BPR latched");	}
			if (IsBHStopped())	//v3.67T3
			{
				SetBTReady(TRUE);
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ((WaitBPRLatched() && WaitWTStartToMoveForBT()) || m_bDisableBT)
			{
				SetBPRLatched(FALSE);
				SetBTStable(FALSE);
				SetWTStartToMoveForBT(FALSE, "1");
				
				//CSP
				if (!m_bFirstCycleBT && m_bPrPreBondAlignment)
				{
					//OpPrPreBondLFPad();
					(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"] = FALSE;	//Currently LF pad is disabled !!!!
				}

				m_qSubOperation = MOVE_TABLE_Q;
			}
			break;
		
		case MOVE_TABLE_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in move table");	}
			ulLastBlk	= (*m_psmfSRam)["BinTable"]["LastBlkInUse"];
			ulBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];

			//v4.50A12
			szLog.Format("BT - MOVE_TABLE_Q - ulLastBlk=%d,ulBlkInUse=%d", ulLastBlk, ulBlkInUse);
			DisplaySequence(szLog);

			if ( (ulLastBlk != ulBlkInUse) && (bBinLoader == TRUE) )
				//&& (m_bEnableOsramBinMixMap == FALSE) && CheckIfChangeFrameForSubBin(ulLastBlk, ulBlkInUse))
			{
				m_lBTMoveDelayTime = 0;
				SetBTStartMoveWithoutDelay(FALSE);
				SetBTStartMove(FALSE);
				SetBTReady(FALSE);
				SetBhTReady(FALSE);
				//WftToSafeLog("6 inch wft bt in move table to change grade q");

				//if( m_ulNewBondedCounter>10 )
				//{
				//	m_ulNewBondedCounter = 0;
				//	SetAutoCycleLog(TRUE);//	HuaMao2	change grade
				//}

				m_stStartChangeTime	= CTime::GetCurrentTime();
				m_unChangeStage	= 1;	// change grade

				
				CTime stTime = CTime::GetCurrentTime();
				CString szMsg = "";
				szMsg.Format("BT Change Grade start time ;%02d:%02d:%02d", 
					stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
				SetStatusMessage(szMsg);

				CMSLogFileUtility::Instance()->BL_LogStatus("**** BT: CHANGE-GRADE ****");	//v4.23T1
				DisplaySequence("BT - MOVE_TABLE_Q to BL_CHANGE_GRADE_Q");
				
				m_qSubOperation = BL_CHANGE_GRADE_Q;
				break;
			}

			/*if (CMS896AApp::m_bEnableSubBin == TRUE && IsMS90Sorting2ndPart() && (ulLastBlk != ulBlkInUse))
			{
				IPC_CServiceMessage svMsg;
				ULONG ulID = ulBlkInUse;
				svMsg.InitMessage(sizeof(ULONG), &ulID);
				RealignPhysicalBlockCmd(svMsg);
				//RealignBinBlock(ulBlkInUse, TRUE, TRUE, FALSE, FALSE);
				DisplaySequence("Realign in table");
			}*/

			if ( AllowMove() == TRUE )
			{
				SetBTReady(FALSE);
				SetBTStable(FALSE);
				
				/*if (m_bEnableOsramBinMixMap)
				{
					m_nBTInUse = 0;		//Always use BT1 for MS109
				}*/
				
				m_nLastError = OpMoveTable();
				
				if (m_nLastError == gnOK)
				{
					LONG lBondTime_Z = (*m_psmfSRam)["BondHead"]["Z_BondTime"];

					if ( ulLastBlk == ulBlkInUse )
					{
						lDelay = m_lBTDelay_Short;
						//v4.42T17	//LeoLam
						if (m_lTime_BT >= 80)
						{
							m_lBTDelay = (*m_psmfSRam)["BinTable"]["BTDelay"];
							lDelay = m_lBTDelay;
						}
					}
					else
					{
						m_lBTDelay = (*m_psmfSRam)["BinTable"]["BTDelay"];
						lDelay = m_lBTDelay;
					}

					//v4.48A2
					//if ( (m_lTime_BT + lDelay) > lBondTime_Z )
					//	Sleep(m_lTime_BT + lDelay - lBondTime_Z);

					//the default BT delay is 5ms during auto-bond, when BHZ arrive at bond level, BT must move finished.
					LONG lRT_BTDelay = 5;
					//SetBTStartMoveWithoutDelay(TRUE);	//andrewng //2020-04-27
					m_lBTMoveDelayTime = m_lTime_BT + lDelay + lRT_BTDelay;

					if ((m_lTime_BT + lDelay + lRT_BTDelay) > lBondTime_Z)
					{
						Sleep(m_lTime_BT + lDelay + lRT_BTDelay - lBondTime_Z);
					}

					BOOL bBinTableThetaCorrection	= (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"];
					if (bBinTableThetaCorrection)
					{
						T_Sync();
					}

					//For SPC use
					(*m_psmfSRam)["BinTable"]["SPC"]["Block"]		= ulBlkInUse;
					(*m_psmfSRam)["BinTable"]["SPC"]["Index"]		= m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse);
					(*m_psmfSRam)["BinTable"]["SPC"]["Bonded"]		= FALSE;
					(*m_psmfSRam)["BinTable"]["SPC"]["DiePerRow"]	= m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse);	//v4.26T1	//Semitek
					(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"]	= m_oBinBlkMain.GrabWalkPath(ulBlkInUse);		//v4.26T1	//Semitek

					//v4.59A4	
					//Handle die shift at BOND side due to Gen TempFile delay in IfBondOK() 
					//	for every 500 dices;

					if (m_bNVRamUpdatedInIfBondOK)
					{
						m_bNVRamUpdatedInIfBondOK = FALSE;
						if (IsMS60())
						{
							Sleep(20);
						}
					}

					SetDieBonded(FALSE);	//2018.6.5 this sychnorize the Z of bond die with bin table, especially there has skip die in waftable
					SetBTStartMove(TRUE);
					szLog.Format("BT - MOVE_TABLE_Q begin and set BT start move");
					DisplaySequence(szLog);
					m_qSubOperation = WAIT_BT_READY_Q;
				}
				else
				{
					SetErrorMessage("BT: OpMove Table fails to HouseKeeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}

			//Request Bin ExArm to get next frame into buffer table
			/*if ((bBinLoader == TRUE) && 
				((bBinExArm == TRUE) || (bDualBuffer == TRUE) || (bDualTable == TRUE)) && 
				(WaitBLAOperate() == FALSE) && 
				!m_bPreLoadChangeFrame && !m_bPreLoadFullFrame )	
			{
				ULONG ulNextBlk = OpGetNextGradeBlk();
				if ( ulNextBlk != BT_DLA_NO_ACTION )
				{
					CString szMsg;
					szMsg.Format("BT: trigger Next blk = %d\n", ulNextBlk);
					CMSLogFileUtility::Instance()->BT_DLALogStatus(szMsg);

					DisplaySequence("BT - Bin Loader - Pre Change Action");
					(*m_psmfSRam)["BinTable"]["BLA_Mode"]["Next Blk"] = ulNextBlk;
					m_bPreLoadChangeFrame = TRUE;		
					SetBLAOperate(TRUE);
				}
			}*/

			//Request Bin Loader move to full position if necessary
			/*if( (bBinLoader == TRUE) && ((bBinExArm == TRUE)||(bDualBuffer == TRUE)) && 
				(WaitBLPreOperate() == FALSE) && 
				!m_bPreLoadChangeFrame &&		// Will not trigger BIN_FULL if Change-Grade is triggered for next frame
				!m_bPreLoadFullFrame )			// DO not check again if BIN_FULL already triggered in this cycle
			{
				BOOL bDisablePreLoadEmpty = ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD);
				if ( (!bDisablePreLoadEmpty) && (OpGetBlkIsNearFull() == TRUE) )
				{
					///***** LOG_LoadUnloadTime("BT: trigger FULL blk", "a+"); /*****
					CMSLogFileUtility::Instance()->BT_DLALogStatus("BT: trigger FULL blk");

					DisplaySequence("BT - Bin Loader - Pre Unload Action");
					m_bPreLoadFullFrame = TRUE;		//v2.67
					SetBLPreOperate(TRUE);
				}
			}*/
			break; 


		case WAIT_BT_READY_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in wait BT ready");	}
			DisplaySequence("BT - Wait BT READY");
			lBondTime_Z		= (*m_psmfSRam)["BondHead"]["Z_BondTime"];
			ulLastBlk		= (*m_psmfSRam)["BinTable"]["LastBlkInUse"];
			ulBlkInUse		= (*m_psmfSRam)["BinTable"]["BlkInUse"];
			bGradeChanged	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["GradeChanged"];

			// Determine the delay by checking the profile selected
			// Determine the delay by checking whetner the bin block is changed
			if ( bGradeChanged == TRUE )
			{
				(*m_psmfSRam)["BinTable"]["GradeChanged"] = FALSE;
				m_lBTDelay = (*m_psmfSRam)["BinTable"]["BTDelay"];
				lDelay = 0;		//m_lBTDelay_Short;		//v2.93T2
			}
			else
			{					
				if ( ulLastBlk == ulBlkInUse )
				{
					// Check for BinLoader
					lDelay = m_lBTDelay_Short;
				}
				else
				{
					m_lBTDelay = (*m_psmfSRam)["BinTable"]["BTDelay"];
					lDelay = m_lBTDelay;
				}
			}
/*
			//v2.97T1
			if ( (m_lTime_BT + lDelay) > lBondTime_Z )
			{
#ifdef NU_MOTION
				Sleep(lBondTime_Z);		//v3.66		//DBH only
#else
				Sleep(m_lTime_BT + lDelay - lBondTime_Z);
#endif
			}
*/

			lMoveTime = _round(GetTime() - m_dBTStartTime);
			if (m_lTime_BT + lDelay - lMoveTime > 0)
			{
				Sleep(m_lTime_BT + lDelay - lMoveTime);
			}

			if (m_bPrPreBondAlignment)		//v3.79
			{
				WaitMoveComplete();
				TakeTime(BT2);

				//CSP
				if ((lPreBondPrDelay > 0) && (lPreBondPrDelay <= 250))
				{
					Sleep(lPreBondPrDelay);	
				}
				else
				{
					Sleep(20);	
				}

				DisplaySequence("BT - prebond start");
				if (!OpPrPreBondAlignPad())
				{
					//v3.83
				//	SetAlert_Red_Yellow(IDS_BT_PRE_BOND_ALIGN_PAD_FAIL);
				//	SetErrorMessage("BT: Prebond pad alignment failure");
					DisplaySequence("BT - prebond PR fail and into house keeping q");
					(*m_psmfSRam)["BinTable"]["PreBondDone"] = 2;	//v3.83
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
				(*m_psmfSRam)["BinTable"]["PreBondDone"] = TRUE;	//v3.83
				SetBTReady(TRUE);
				DisplaySequence("BT - prebond done");
			}
			else
			{
				if (m_bFirstCycleBT)
				{
					X_Sync();
					Y_Sync();
					Sleep(m_lBTDelay);
				}
	 
				SetBTReady(TRUE);
//				WaitMoveComplete();		//For both BT1 & BT2
				TakeTime(BT2);
			} 
#ifndef NU_MOTION
			//For SPC use
			(*m_psmfSRam)["BinTable"]["SPC"]["Block"]		= ulBlkInUse;
			(*m_psmfSRam)["BinTable"]["SPC"]["Index"]		= m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse);
			(*m_psmfSRam)["BinTable"]["SPC"]["Bonded"]		= FALSE;
			(*m_psmfSRam)["BinTable"]["SPC"]["DiePerRow"]	= m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse);	//v4.26T1	//Semitek
			(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"]	= m_oBinBlkMain.GrabWalkPath(ulBlkInUse);		//v4.26T1	//Semitek
#endif
			m_bFirstCycleBT = FALSE;
			m_qSubOperation = WAIT_DIE_BONDED_Q;
			break;


		case WAIT_DIE_BONDED_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())	DisplaySequence("BT - stop gen temp file in wait die bonded");	}
			//else
				DisplaySequence("BT - WAIT DIE_BONDED_Q");
			bPicked = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["DiePickedForBT"];
			bMissingDie = (LONG)(*m_psmfSRam)["BondHead"]["MissingDie"];

			// Perform Uplook PR COMPENSATE first before waiting for BH Die_DONDED	//v4.57A13
			if (bEnableBHUplookPr && WaitBTCompensate())	//v4.52A16	
			{
				OpPerformUplookBTCompensation();	
			}
			// Check Die Bonded first to prevent missing die info
			else if ( WaitDieBonded() )
			{
TakeTime(LC6);
				(*m_psmfSRam)["BinTable"]["MissingDie"] = FALSE;

				//For SPC use
				(*m_psmfSRam)["BinTable"]["SPC"]["Bonded"] = TRUE;
				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;

				SetDieBonded(FALSE);
				SetBTStable(TRUE);
				m_qSubOperation = NEXT_DIE_Q;
			}
			else if ( bMissingDie >= TRUE )
			{
				(*m_psmfSRam)["BinTable"]["MissingDie"] = TRUE;
				(*m_psmfSRam)["BondHead"]["MissingDie"] = FALSE;

				// Update Wafer Map for missing die
				if (bMissingDie > TRUE)
				{
					m_WaferMapWrapper.SetDieState(m_lWT_Row ,m_lWT_Col, WT_MAP_DS_BH_UPLOOK_FAIL + GetWTGrade());
				}
				else
				{
					m_WaferMapWrapper.SetDieState(m_lWT_Row ,m_lWT_Col, WT_MAP_DIESTATE_MISSING + GetWTGrade());
				}
				m_qSubOperation = WAIT_DIE_READY_Q;
				DisplaySequence("BT - Die bonded to Wait Die Ready because missing die");
			}
			else if ( bPicked == FALSE )
			{
				if (IsBHStopped())	//v3.67T3
				{
					//Wait unit BondArm is in safe position
					if ( IsBHStopped() == 1)
					{
						SetErrorMessage("BT: Move=Back at WAIT DIE_BONDED_Q");
						m_qSubOperation = MOVE_BACK_Q;
					}
				}
			}
			else
			{
				BOOL bPitchAlarm = (BOOL)(LONG)(*m_psmfSRam)["MS899"]["PitchAlarm"];
				if ( (IsMotionCE() == TRUE) || bPitchAlarm )			//v3.30T1
				{
					SetErrorMessage("BT: Critical stop detected at WAIT DIE_BONDED_Q!");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if ( IsBHStopped() == 1 )		//v3.94T4
				{
					SetErrorMessage("BT: abnormal quit sequence in WAIT DIE_BONDED_Q state!");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;

		case NEXT_DIE_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in next die");	}
			DisplaySequence("BT - Next Die Q");
			m_nLastError = OpNextDie();
			if (m_nLastError == TRUE)
			{
				m_qSubOperation = WAIT_DIE_READY_Q;
			}
			else if( m_nLastError == -1 )	//	V450X16
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("**** BT: Next die error ****");
				m_qSubOperation = HOUSE_KEEPING_Q;
				SetErrorMessage("BT next die error");
				DisplaySequence("BT - next die error to house keeping q");
			}
			else
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("**** BT: BIN-FULL 3 ****");		//v4.23T1
				m_qSubOperation = BIN_FULL_Q;
				m_qNextSubOperation = WAIT_DIE_READY_Q;
				SetErrorMessage("BT next die bin full to change");
				DisplaySequence("BT - next die bin full to change YYYY");
				if( m_ulNewBondedCounter>10 )
				{
					m_ulNewBondedCounter = 0;
					SetAutoCycleLog(TRUE);//	HuaMao2	Bin Full
				}
				m_stStartChangeTime	= CTime::GetCurrentTime();
				m_unChangeStage	= 2;	// bin full
			}
			// Need to handle the case of post bond
			break;

		case MOVE_BACK_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in move back q");	}
			DisplaySequence("BT - Move Back");
			lBondTime_Z = (*m_psmfSRam)["BondHead"]["Z_BondTime"];
			nWaitDelay = (INT)(lBondTime_Z + (LONG)(*m_psmfSRam)["BondHead"]["BondDelay"]);
			nWaitDelay += 100;		// Add 100ms for safety
			if ( (nWaitDelay <= 0) || (WaitDieBonded(nWaitDelay) == FALSE) )
			{
				if (m_bUseDualTablesOption && (m_nBTInUse == 1))	//MS100 9Inch daul-table config	//v4.21T1
					OpMoveBack2();
				else
					OpMoveBack();
			}
			m_qSubOperation = HOUSE_KEEPING_Q;
			break;

		case BIN_FULL_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in bin full q");	}
			DisplaySequence("BT - Bin Full");
			(*m_psmfSRam)["BinTable"]["BinFull"] = TRUE;

			lLoopCounter = 0;
			while( 1 )
			{
				lBHAtPick = (*m_psmfSRam)["BondHead"]["AtPrePick"];
				if( lBHAtPick )
				{
					CMSLogFileUtility::Instance()->BL_LogStatus("in bin full q bh at prepick\n");
					break;
				}
				Sleep(100);
				lLoopCounter++;
				if( lLoopCounter>10 )
				{
					CMSLogFileUtility::Instance()->BL_LogStatus("in bin full q bh not at prepick but wait over 1 seconds\n");
					break;
				}
			}

			if( IsMotionCE() )
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("stop to house keeping in bin full q because critical error\n");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			// Check for BinLoader
			if ( bBinLoader == TRUE )
			{
				SetBTReady(FALSE);
				SetBTStartMoveWithoutDelay(FALSE);
				SetBTStartMove(FALSE);
			
				CTime stTime = CTime::GetCurrentTime();
				CString szMsg = "";
				szMsg.Format("BT Change Full start time ;%02d:%02d:%02d", 
					stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
				SetStatusMessage(szMsg);
				CMSLogFileUtility::Instance()->BL_LogStatus("BT: BIN_FULL wait BH PREPICK start");	//v3.94
				m_qSubOperation = BL_BIN_FULL_WAIT_BH_Q;	//v3.70T1
				break;
			}

			// If burn in mode, not issue an alert
			if ( IsBurnIn() == FALSE )
			{
				ULONG nDieCount =0;
				// Construct the message 
				char acTemp[20];
				_itoa_s(GetWTGrade(), acTemp, sizeof(acTemp), 10);
				szMsg.Format("Grade %s is full.\n(Bin Block: ", acTemp); 
				ULONG ulNumOfBinBlk = GetNoOfBinBlks();
				bFirst = TRUE;				
				for (i = 1; i <= ulNumOfBinBlk; i++ )
				{
					if ( GetBinBlkGrade(i) == GetWTGrade() )
					{
						if ( bFirst == TRUE )
						{
							szTemp.Format("%d", i);
                            bFirst = FALSE;
						}
						else
						{
							szTemp.Format(",%d", i);
						}
						nDieCount = nDieCount + m_oBinBlkMain.GrabNVNoOfBondedDie(i);
						szMsg += szTemp;
						
						m_ulBinBlkFull = i;
					}
				}
				szTemp.Format(")\n Count: %d\n",nDieCount);
				szMsg = szMsg + szTemp + "Please unload!";

				SetAlert_Msg_Red_Yellow(IDS_BT_BINFULL, szMsg);		// Set Alarm for Bin Full
				SetErrorMessage("Bin block full");
				m_bIsUnload = TRUE;
			}

			m_qSubOperation = HOUSE_KEEPING_Q;
			break;

		case BL_CHANGE_GRADE_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in bl change grade");	}
			DisplaySequence("BT - Bin Loader - Change Grade");
			
			(*m_psmfSRam)["BinTable"]["Change Grade"]  = TRUE;
			(*m_psmfSRam)["BinTable"]["BL_Mode"] = 2;
			WftToSafeLog("6 inch wft bt to change grade wait bh q");

			if( pApp->IsToSaveMap() )
			{
				OpBackupMap();
			}

			CMSLogFileUtility::Instance()->BL_LogStatus("BT: NuMotion ChangeGrade wait BH PREPICK start");	//v3.94
			m_qSubOperation = BL_CHANGE_GRADE_WAIT_BH_Q;
			break;

		case BL_CHANGE_GRADE_WAIT_BH_Q:		//For DBH only
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in bl change grade wait bh");	}
			DisplaySequence("BT - BL_CHANGE_GRADE_WAIT_BH_Q");
			
			lBHAtPick = (*m_psmfSRam)["BondHead"]["AtPrePick"];
			lBHAtSafe = IsBHStopped();			//v4.10T5	//TynTek hangup
			if (lBHAtPick != 1)
			{
				if (lBHAtSafe == 1)		//v4.11T1
				{
					Sleep(100);
				}
				else
				{
					Sleep(100);
					break;
				}
			}

			//v4.59A25
			ulBlk = (*m_psmfSRam)["BinTable"]["PhyBlkInUse"];	// physical block id
			(*m_psmfSRam)["BinTable"]["PhyBlkDieCount"] = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlk);

			LetWaferTableDoAdvSampling(GetWTGrade());

			//CMSLogFileUtility::Instance()->BL_LogStatus("BT: GEN ALL TEMP FILE");
			//DisplaySequence("BT - BL CHANGE GRADE Gen Temp, trigger BL");
			OpGenAllTempFile(CMS896AStn::m_bChangeGradeBackupTempFile);
			
			//andrewng //2020-0720
			//SetBLReady(FALSE);
			SetNLReady(FALSE);
			//SetBLOperate();
			SetNLOperate();

			CMS896AStn::m_bBTAskBLChangeGrade = TRUE;

			//CMSLogFileUtility::Instance()->BL_LogStatus("BT: ChangeGrade wait BH PREPICK done\n");	//v3.94
			m_qSubOperation = WAIT_BL_CHANGE_READY_Q;
			DisplaySequence("BT - BL Change Grade Wait BH ==> Bin Loader - Wait Change Grade");
			break;


		case BL_BIN_FULL_WAIT_BH_Q:			//For DBH only
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in bl bin full wait bh");	}
			DisplaySequence("BT - BL_BIN_FULL_WAIT_BH_Q");
			lBHAtPick = (*m_psmfSRam)["BondHead"]["AtPrePick"];
			lBHAtSafe = IsBHStopped();			//v4.10T5	//TynTek hangup
			if (lBHAtSafe)		//Re-enable at v4.15T8 to solve hang at Walsin LiWha	//v4.15T8
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("BT: stop to housekeeping BINFULL wait DBH\n");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			else if (lBHAtPick != 1)
			{
				Sleep(100);
				break;
			}
			LetWaferTableDoAdvSampling(GetWTGrade());
			CMSLogFileUtility::Instance()->BL_LogStatus("BT: BIN_FULL wait BH PREPICK done\n");	//v3.94
			m_qSubOperation = BL_BIN_FULL_Q;
			break;

		case WAIT_BL_CHANGE_READY_Q:
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (m_lWftAdvSamplingStage == 1)
			{
				Sleep(50);
				//if (WaitBLReady())
				if (WaitBLReady())		//andrewng //2020-0720
				{
					LONG lRet = DoRealignBinFrame();
					if (lRet != TRUE)
					{
						if (lRet == FALSE)
						{
							SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);
						}
						m_qSubOperation = HOUSE_KEEPING_Q;
						DisplaySequence("BT - BL Change grade, Realign failure");
						break;
					}
					DisplaySequence("BT - BL Change grade, Wait ADV done, BL Change ok");
				}
				else
				{
					DisplaySequence("BT - BL Change grade, Wait ADV done");
				}
				break;
			}	// adv sampling

			DisplaySequence("BT - WAIT_BL_CHANGE_READY_Q");
			//if (WaitBLReady())
			if (WaitNLReady())		//andrewng //2020-0720
			{
				LONG lRet = DoRealignBinFrame();
				if (lRet != TRUE)
				{
					if (lRet == FALSE)
					{
						SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);
					}
					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("BT - BL Change grade, Realign failure");
					break;
				}
//==============================================================================================Tang Suggestion
				SetDieBonded(TRUE);	//	 Suggested by Andrew	// HuaMao green light idle
				//Bin table move to the collet offet of first picking BH 2018.12.22
				(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"] = FALSE;
//==============================================================================================
				
				//SetBLReady(FALSE);
				SetNLReady(FALSE);		//andrewng //2020-0720
				CMS896AStn::m_bBTAskBLChangeGrade = FALSE;		//v4.43T9
			
				CTime stTime = CTime::GetCurrentTime();
				CString szMsg = "";
				szMsg.Format("BT Change Grade stop  time ;%02d:%02d:%02d", 
					stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
				SetStatusMessage(szMsg);
				//v4.21T1
				if (m_bUseDualTablesOption && (m_bEnableOsramBinMixMap == FALSE))
				{
					LONG lNewBTInUse = (*m_psmfSRam)["BinTable"]["BTInUse"];
					m_nBTInUse	= lNewBTInUse;
				}
				else
				{
					m_nBTInUse	= 0;	//Always BT1 for non-dual-table options
				}

				// Set the last block equal to current block
				ulBlkInUse = (*m_psmfSRam)["BinTable"]["BlkInUse"];                                       
				(*m_psmfSRam)["BinTable"]["LastBlkInUse"] = ulBlkInUse;
				(*m_psmfSRam)["BinTable"]["BondingGrade"] = GetWTGrade();
				(*m_psmfSRam)["BinTable"]["GradeChanged"] = TRUE;
				(*m_psmfSRam)["BinTable"]["BinChanged"]	= TRUE;
				(*m_psmfSRam)["BinTable"]["BinChangedForBH"] = TRUE;	//v4.49A11

				m_bPreLoadFullFrame = FALSE;		//v2.67
				m_bPreLoadChangeFrame = FALSE;		//v2.67

				if (m_bUseDualTablesOption)				//v4.23T2
				{
					CString szLog;
					szLog.Format("BT: ChangeGrade wait BL done -> MOVE_TABLE_Q; BTInUse=%d\n", m_nBTInUse);  
					CMSLogFileUtility::Instance()->BL_LogStatus(szLog);	

					//v4.36T2
					//Re-calculate m_lX & m_lY based on new dX/dY/Angle, which was just updated in MS109 bin realignment
					INT nStatus = OpIndex_MS109_AfterChangeGrade(TRUE);	//v4.42T16	//Testar
					if (nStatus == gnNOTOK)
					{
						SetErrorMessage("OpIndex_MS109 not OK in WAIT_BL_CHANGE_READY_Q 2");
					}
				}
				else
				{
					CMSLogFileUtility::Instance()->BL_LogStatus("BT: ChangeGrade wait BL done -> MOVE_TABLE_Q\n");		//v3.94
				}
				
				//OpSemitekCheckEmptyFrame(m_nBTInUse);	//v4.51A19	//Matt

				// should check it is read or not.
				m_qSubOperation = MOVE_TABLE_Q; 
				szLog.Format("BT - Wait BL Change Ready (%ld) ==> Move Table", ulBlkInUse);
				DisplaySequence(szLog);

				CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(m_stStartChangeTime, "ChangeGrade");
				m_unChangeStage	= 0;
			}
			else
			{
				Sleep(1);		//v2.93T2
			}
			break;


		case BL_BIN_FULL_Q:
			//if( m_bStop )	{	if (OpGenAllTempFile())		DisplaySequence("BT - stop gen temp file in bl bin full");	}
			DisplaySequence("BT - Bin Loader - Bin Full)");
			(*m_psmfSRam)["BinTable"]["BL_Mode"] = 1;
			(*m_psmfSRam)["BinTable"]["GradeLeft"] = GetGradeLeftOnWaferMap();

			//andrewng //2020-0720
			//SetBLReady(FALSE);
			//SetBLOperate();
			SetNLReady(FALSE);
			SetNLOperate();	

			CMS896AStn::m_bBTAskBLBinFull = TRUE;
			
			if (pApp->IsToSaveMap())
			{
				OpBackupMap();
			}

			if (m_bIfNeedUpdateBinFrameAngle)
			{
				m_bIfNeedUpdateBinFrameAngle = FALSE;
			}
			m_qSubOperation = WAIT_BL_BIN_FULL_READY_Q;
			break;


		case WAIT_BL_BIN_FULL_READY_Q:
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (m_lWftAdvSamplingStage == 1)
			{
				Sleep(50);
				DisplaySequence("BT - WFT - Wait Adv done");
				break;
			}	// adv sampling

			DisplaySequence("BT - Bin Loader - Wait Bin Full)");
			//if (WaitBLReady())
			if (WaitNLReady())		//andrewng //2020-0720
			{
				LONG lRet = DoRealignBinFrame();
				if (lRet != TRUE)
				{
					if (lRet == FALSE)
					{
						SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);
					}

					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("BT - BL Change grade, Realign failure");
					break;
				}

				//SetBLReady(FALSE); 
				SetNLReady(FALSE);		//andrewng //2020-0720
				CMS896AStn::m_bBTAskBLBinFull = FALSE;

				CTime stTime = CTime::GetCurrentTime();
				CString szMsg = "";
				szMsg.Format("BT Change Full stop  time ;%02d:%02d:%02d", 
					stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());
				SetStatusMessage(szMsg);
				CMSLogFileUtility::Instance()->BL_LogStatus("BT: BIN_FULL wait BL done\n");		//v3.94
				//v4.21T1
				if (m_bUseDualTablesOption && (m_bEnableOsramBinMixMap == FALSE))
				{
					LONG lNewBTInUse = (*m_psmfSRam)["BinTable"]["BTInUse"];
					m_nBTInUse	= lNewBTInUse;
				}
				else
				{
					m_nBTInUse	= 0;	//Always BT1 for non-dual-table options
				}

				//OpSemitekCheckEmptyFrame(m_nBTInUse);	//v4.51A19	//Matt
				
				//4.53D90 run time check frame full
				OptimizeBinCountPerWft();

				//Reset BinFull status
				(*m_psmfSRam)["BinTable"]["BinFull"] = FALSE;
				(*m_psmfSRam)["BinTable"]["BinChanged"] = TRUE;
				(*m_psmfSRam)["BinTable"]["BinChangedForBH"] = TRUE;	//v4.49A11

				m_bPreLoadFullFrame = FALSE;		//v2.67
				m_bPreLoadChangeFrame = FALSE;		//v2.67
				m_qSubOperation = (m_qNextSubOperation == -1) ? WAIT_DIE_READY_Q : m_qNextSubOperation;
				DisplaySequence("BT - BL bin full ready to Wait Die Ready");
				CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(m_stStartChangeTime, "ChangeFrame");
				m_unChangeStage	= 0;
				if (IsMS90() && (WaitDieReadyForBT()==FALSE))
				{
					DisplaySequence("BT - BL bin full ready to Wait Die Ready Set Info Read For WT");
					//2018.10.22 remove the SetDieInfoRead(TRUE) becasue it will trigger to update next die map coordinate and lead into map shift issue
//					SetDieInfoRead(TRUE);
				}
/*
//2019.1.14 remove it because it will lead into BT mismatch
				if (WaitDieReadyForBT()==FALSE)
				{
					SetDieReadyForBT(TRUE);
					SetDieInfoRead(TRUE);
				}
*/
				//v4.11T1 //temp. disable this call by andrew for Tyntek
				if (pApp->GetCustomerName() == "Lumileds")
				{
					IPC_CServiceMessage stMsg;
					m_comClient.SendRequest(WAFER_PR_STN, "RefreshPrScreen", stMsg);
				}
				else if (pApp->GetCustomerName() == "Cree")
				{
					CreateCreeStatisticReport(1);		//v4.49A5
				}
			}
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("BT - House Keeping Q");
			if (GetHouseKeepingToken("BinTableStn") == TRUE)
			{
				SetHouseKeepingToken("BinTableStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}

			s_bLastErrorIndex = FALSE;

			SetAutoCycleLog(FALSE);
			if (m_unChangeStage != 0)
			{
				if( m_unChangeStage==1 )
					CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(m_stStartChangeTime, "ChangeGrade");
				else
					CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(m_stStartChangeTime, "ChangeFrame");
				m_unChangeStage	= 0;
			}

			LogCycleStopState("BT - GenTempFiles");
			OpGenAllTempFile();

			(*m_psmfSRam)["BinTable"]["AtSafePos"]	= 1;	//v3.67T3

			LogCycleStopState("BT - BackupMap");
			//v4.52A11
			if( pApp->IsToSaveMap() )
			{
				OpBackupMap();
			}

			LogCycleStopState("BT - WaferStop");
			//OpGenWaferStopInfo();

			if (bNoMotionHouseKeeping == FALSE)
			{
				X_Profile(NORMAL_PROF);
				Y_Profile(NORMAL_PROF);
			}

			if (IsAllSorted() && CMS896AApp::m_bEnableSubBin == TRUE)
			{
				HmiMessage("Sub Bin Finished, Clear Bin");
				for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
				{
					BOOL bNeedToClear = FALSE;
					ClrBinCntCmdSubTasks(i, "All Blocks", m_szBinOutputFileFormat, TRUE, FALSE, bNeedToClear);
				}
			}

			// Acknowledge other stations to stop
			StopCycle("BinTableStn");				

			SetHouseKeepingToken("BinTableStn", TRUE);

			m_qSubOperation = MOVE_TO_UNLOAD_Q;
			break;

		case MOVE_TO_UNLOAD_Q:
			DisplaySequence("BT - Move to Unload");

			lStopped	= CMS896AStn::m_lBondPrStopped;

			if ((IsBHStopped() == 1) && (lStopped == 1)) 
			{
				LogCycleStopState("BT - MoveToUnload");

				if (bNoMotionHouseKeeping == FALSE)
				{
					OpMoveToUnload();
				}

				CMS896AStn::m_lBinTableStopped = 1;
				
				//BackupAllTempFile();

				LogCycleStopState("BT - stop completely");
				State(STOPPING_Q);
			}
			else
			{
				Sleep(10);			
			}

			break;

		default:
			DisplaySequence("BT - Unknown");
			m_nLastError = glINVALID_STATE;
			break;
		}
/*
		if ( m_bHeapCheck == TRUE )
		{
			// Check the heap
			INT nHeapStatus = _heapchk();
			if ( nHeapStatus != _HEAPOK )
			{				
				CString szMsg;
				szMsg.Format("BT - Heap corrupted (%d) before BT [%s]", nHeapStatus, 
							g_aszBTState[m_qSubOperation]);
				DisplayMessage(szMsg);

				LogCycleStopState(szMsg);
			}
		}
*/
	}
	catch (CAsmException e)
	{
		//Handle normal stop cycle task
		LogCycleStopState("BT ex - GenTempFiles");
		OpGenAllTempFile();

		LogCycleStopState("BT ex - BackupMap");
		OpBackupMap();

		LogCycleStopState("BT ex - WaferStop");
		OpGenWaferStopInfo();


		DisplayException(e);
		Result(gnNOTOK);

		CMS896AStn::m_lBinTableStopped = 1;
		State(STOPPING_Q);

		NeedReset(TRUE);
		StopCycle("BinTableStn");
		SetMotionCE(TRUE, "Bin Table Run Operation Stop");
	}
}


LONG CBinTable::GetBTRemainMoveDelay()
{
	LONG lBTRemainMoveDelay = _round(m_lBTMoveDelayTime - GetCurTime());
	if (lBTRemainMoveDelay < 0)
	{
		return 0;
	}

	return lBTRemainMoveDelay;
}
