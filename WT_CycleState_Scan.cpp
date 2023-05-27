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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferTable::RunOperationWft_Scan()
{
	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		OpAbort1stDieFinderToPick();

		DisplaySequence("WT - Scan STOP");
		State(STOPPING_Q);
		return ;
	}

	try
	{
		BOOL	bFull = FALSE;
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		INT nLastError;

		m_nLastError	= gnOK;

		if (m_bStopAllMotion == TRUE)
		{
			if (m_qSubOperation != UPDATE_LAST_DIE_Q)
				m_qSubOperation = HOUSE_KEEPING_Q;
		}

		// Operation state
		switch (m_qSubOperation)
		{
		case PRESCAN_MOVE_TABLE_Q:
			if ( AllowMove() == TRUE )
			{
				if ( OpMoveTable_Prescan() == gnOK) // move table to target position
				{
					m_dIdleDiffTime = GetTime();
					m_qSubOperation = PRESCAN_WAIT_PR_DELAY_Q;
					DisplaySequence("WFT - scan move table ==> scan wait pr delay q");
				}
				else
				{
					DisplaySequence("WFT - scan move table ==> house keeping q");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;

		case PRESCAN_WAIT_PR_DELAY_Q:
			TakeTime((TIME_ENUM)SCAN_WFT_PR_DLY1);		// prescan_time scan PR delay begin
			if( m_lPrescanPrDelay>0 )
			{
				Sleep(m_lPrescanPrDelay);
			}

			TakeTime((TIME_ENUM)SCAN_WFT_PR_DLY2);		// prescan_time scan PR delay end
			SetWTStable(TRUE);							// Trigger PR to prescan wafer
			if( pUtl->GetPrescanAreaPickMode() )
				m_qSubOperation = PRESCAN_AP_GET_WAFER_POS_Q;
			else
				m_qSubOperation = PRESCAN_GET_WAFER_POS_Q;
			DisplaySequence("WFT - scan pr delay ==> scan get wafer pos");
			if( (GetTime() - m_dIdleDiffTime)>=5*60*1000 )
			{
				m_dIdleDiffTime = GetTime();
				CMSLogFileUtility::Instance()->MS_LogCycleState("running");
			}
			break;

		case PRESCAN_GET_WAFER_POS_Q:				// Get the scan position
			TakeTime((TIME_ENUM)SCAN_WFT_NEXT_1);	// prescan_time to get wft next index position begin
			if( IsPrescanEnded() )
			{
				nLastError = gnNOTOK;
			}
			else
			{
				nLastError = OpGetWaferPos_Prescan();
			}
			TakeTime((TIME_ENUM)SCAN_WFT_NEXT_2);	// prescan_time get wft next index position end normally
			if( (nLastError == gnOK) )
			{
				m_qSubOperation = PRESCAN_WAIT_DIE_READY_Q;
				DisplaySequence("WFT - scan get wafer next pos ==> wait die ready q");
			}
			else
			{
				Sleep(30);
				SetWTStable(TRUE);			// Trigger Wafer PR 
				m_qSubOperation = PRESCAN_WAIT_ALL_DONE_Q;
				if( IsWT2InUse() )
					SaveScanTimeEvent("    WFT: WT2 scan get wafer next pos ==> wait all done q");
				else
					SaveScanTimeEvent("    WFT: scan get wafer next pos ==> wait all done q");
				DisplaySequence("WFT - scan get wafer next pos ==> wait all done q");
			}
			if( m_dStopTime>0.0 )
			{
				m_dCurrTime += fabs(GetTime() - m_dStopTime)/1000.0;
				m_dScanTime += fabs(GetTime() - m_dStopTime)/1000.0;
				m_dScanMoveTime = m_dScanTime;
				m_dStopTime = GetTime();
			}
			break;

		case PRESCAN_WAIT_DIE_READY_Q:
			if (IsBHStopped())
			{
				DisplaySequence("WT - scan wait die ready to house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (WaitDieReady())
			{
				SetDieReady(FALSE);
				SetWTStable(FALSE);

				DisplaySequence("WFT - scan wait die ready ==> move table q");
				m_qSubOperation = PRESCAN_MOVE_TABLE_Q;
			}
			break;

		case PRESCAN_WAIT_ALL_DONE_Q:
			m_bWaferAlignComplete	= FALSE;
			m_bIsPrescanned = TRUE;
			if (IsBHStopped())
			{
				m_bWaferAlignComplete	= TRUE;//2019.05.22
				DisplaySequence("WT - scan wait all done to house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			if (WaitAllPrescanDone())
			{
				m_ulRescanAtBondDieCount = m_ulBondDieCount;
				DisplaySequence("WT - scan wait all PR done");
				SaveScanTimeToFile(GetNewPickCount());
				if( m_dStopTime>0 )
				{
					m_dScanTime += fabs(GetTime() - m_dStopTime)/1000.0;	// time after PR and physical map built
					m_dCurrTime = m_dScanTime;
					m_dStopTime = GetTime();
				}
				CString szLogMsg;
				szLogMsg.Format("PR + map time %.2f", m_dCurrTime);
				LONG lContinueState = 2;
				if( IsAutoRescanWafer() )
				{	//	auto rescan sorting wafer	after re-scan finish, into move table again for sorting
					SaveScanTimeEvent("    WFT: rescan wafer " + szLogMsg);
					SetAllPrescanDone(FALSE);
					if( RescanAllDoneOnWft() )
					{
						SaveScanTimeEvent("    WFT: rescan wafer all done to move table");

                        m_qSubOperation = MOVE_TABLE_Q;
					}
					else
					{
						SaveScanTimeEvent("    WFT: rescan wafer all done to house keeping");
                        m_qSubOperation = HOUSE_KEEPING_Q;
					}
					UpdateWaferYield();
					break;
				}	//	auto rescan sorting wafer	after re-scan finish, into move table again for sorting
				else
				{
					SaveScanTimeEvent("    WFT: prescan " + szLogMsg);
					SetAllPrescanDone(FALSE);
					lContinueState = PrescanAllDoneOnWft();			// in this function, define next state already
				}

				if( m_dStopTime>0 )
				{
					m_dCurrTime += fabs(GetTime() - m_dStopTime)/1000.0;	// time after PR and physical map built
					m_dStopTime = GetTime();
				}
				szLogMsg.Format("    WFT: prescan all done %.2f", m_dCurrTime);
				SaveScanTimeEvent(szLogMsg);

				//Get first region
				if (!m_pWaferMapManager->PrepareFirstPickingRegion())
				{
					//Wafer End and stop bonding
					 SaveScanTimeEvent("    WFT: Get First Regin Fail to HouseKeeping");
					 m_qSubOperation = HOUSE_KEEPING_Q;
					 break;
				}

				//Sorting the next slot
				if (m_pWaferMapManager->PrepareNextSortingRegion())
				{
					WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
					UCHAR ucSortingGrade = pSRInfo->GetSortingRegionGrade();
					CreatePrepareNextRegionStatisticsThread(ucSortingGrade);
				}

					//Send Wafer map Selected Grade to EM after sorting  2017.10.27
					SendWaferMapSelectedGradeList();
					//Create Barcode ID File
					CreateUploadBarcodeIDFile();
					//=======================================================

				//	should also log , the actual prescna time in seconds.
				CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(m_stPrescanStartTime, "PrescanWT");

				m_bPrescanJustFinishToProbe = TRUE;
				if (IsMS90HalfSortMode())
				{
					if (IsMS90Sorting2ndPart())
						CMS896AStn::m_bAutoGenerateWaferEndFile	= TRUE;
					else
						CMS896AStn::m_bAutoGenerateWaferEndFile	= FALSE;
				}
				else
					CMS896AStn::m_bAutoGenerateWaferEndFile	= TRUE;
				SetDieReady(FALSE);

				switch( lContinueState )
				{
				case 1:
					m_qSubOperation = WAFER_END_HANDLING_Q;
					CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(8);	//SORT_START		//v4.40T11
					m_bPrescanningMode = TRUE;
					break;
				case 2:
					m_qSubOperation = HOUSE_KEEPING_Q;
					CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(8);	//SORT_START		//v4.40T11
					m_bPrescanningMode = TRUE;
					if( IsESMachine() && GetScanAction()==PRESCAN_FINISHED_SORT_BY_GRADE )
					{
						m_bPrescanningMode = FALSE;
						m_bIntoNgPickCycle = TRUE;
					}
					break;
				case 0:
				default:
					m_qSubOperation = GET_WAFER_POS_Q;
					if (m_lChangeLightSCNContinue == 1)
					{
						m_qSubOperation = HOUSE_KEEPING_Q;//trigger stop in FivePointCheckDiePosition for changelight
					}
					CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(8);	//SORT_START		//v4.40T11
					m_bPrescanningMode = FALSE;
					if( IsESMachine() && GetScanAction()==PRESCAN_FINISHED_PICK_NG )
					{
						m_bIntoNgPickCycle = TRUE;
					}
					break;
				}
				X_Profile(NORMAL_PROF);
				Y_Profile(NORMAL_PROF);
				m_bWaferAlignComplete	= TRUE;
				SaveScanTimeEvent("    WFT: Wafer Align Complete");
			}
			break;

		case PRESCAN_AP_GET_WAFER_POS_Q:
			TakeTime((TIME_ENUM)SCAN_WFT_NEXT_1);	// prescan_time to get wft next index position begin
			if( IsPrescanEnded() )
			{
				nLastError = -1;
			}
			else
			{
				nLastError =  OpGetWaferPos_PrescanAreaPick(GetScanLastPosnX(), GetScanLastPosnY());
			}
			TakeTime((TIME_ENUM)SCAN_WFT_NEXT_2);	// prescan_time get wft next index position end normally

			if( nLastError==-1 && GetGoodTotalDie()<=0 )
			{
				Sleep(m_lPrescanPrDelay);
				SetWTStable(TRUE);			// Trigger Wafer PR 
				m_qSubOperation = PRESCAN_WAIT_ALL_DONE_Q;
				DisplaySequence("WFT - scan get wafer next pos ==> all done");
				m_lAreaPickStage = 3;
			}
			else if( nLastError==1 )
			{
				DisplaySequence("WFT - scan wafer pos ==> wait die ready");
				m_qSubOperation = PRESCAN_WAIT_DIE_READY_Q;
			}
			else
			{
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					lCounter++;
					if( lCounter>100 )
					{
						break;
					}
					if( m_bPrescanLastGrabbed &&pUtl->GetPrescanMapIndexIdle()  )
					{
						break;
					}
				}

				DisplaySequence("WFT - scan AP to construct sorting map");
				if( ConstructAreaPickMap()==FALSE && m_nPickListIndex<=0 )
				{
					LONG lY = GetScanLastPosnY() - GetPrescanPitchY();
					LONG lX = 0;
					CString szMsg;
					if( m_lAreaPickScanDirection==1 )
					{
						lX = m_lAreaPickScanUL_X + GetPrescanPitchX();
						m_lAreaPickScanDirection = -1;
						szMsg = "no die move direction -1";
					}
					else
					{
						lX = m_lAreaPickScanLR_X - GetPrescanPitchX();
						m_lAreaPickScanDirection = 1;
						szMsg = "no die move direction 1";
					}
					pUtl->PrescanMoveLog("WFT - scan to get next scan position" + szMsg);
					int nReturn = OpGetWaferPos_PrescanAreaPick(lX, lY);
					if( nReturn==-1 )
					{
						Sleep(m_lPrescanPrDelay);
						SetWTStable(TRUE);			// Trigger Wafer PR 
						m_qSubOperation = PRESCAN_WAIT_ALL_DONE_Q;
						DisplaySequence("WFT - scan AP no die ==> all done");
						m_lAreaPickStage = 3;
					}
					else
					{
						SetWTStable(FALSE);
						m_qSubOperation = PRESCAN_WAIT_DIE_READY_Q;
						DisplaySequence("WFT - scan AP no die ==> wait die ready q");
					}
				}
				else
				{
					ResetBuffer();	// to sorting mode
					m_bPrescanningMode = FALSE;
					m_lAreaPickStage = 2;
					m_lAreaPickScanDirection = -1;
					m_qSubOperation = PRESCAN_AP_NEXT_DIE_Q;
					if( m_lAPLastDone==1 || m_lAPLastDone==-1 )
						m_lAPLastDone	= 2;
					pUtl->PrescanMoveLog("have die move direction -1");
					DisplaySequence("WFT - scan AP get next pos ==> pick next die q");
				}
			}

			if( m_dStopTime>0.0 )
			{
				m_dCurrTime += fabs(GetTime() - m_dStopTime)/1000.0;
				m_dScanTime += fabs(GetTime() - m_dStopTime)/1000.0;
				m_dScanMoveTime = m_dScanTime;
				m_dStopTime = GetTime();
			}
			break;

		case PRESCAN_AP_NEXT_DIE_Q:	//	then calculate next die position and get time
			if (IsBHStopped())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				DisplaySequence("WFT - scan AP pick next die ==> house keeping");
				break;	// stopped
			}

			bFull = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinFull"];
			if( m_bStopBinLoader!=TRUE && IsBLEnable() && bFull )
			{
				SetBPRLatched(TRUE);
				SetEjectorReady(TRUE);
				break;
			}

			// Used in waferPr and pass to IM Vision for logging
			m_bIsWaferEnded	= FALSE;
			m_nLastError = OpGetWaferPos_AreaPick();
			if( m_nLastError==gnOK )
			{
				(*m_psmfSRam)["WaferTable"]["PickDie"]	= (LONG)TRUE;
				m_qSubOperation = PRESCAN_AP_MOVE_TABLE_Q;
				DisplaySequence("WFT - scan AP pick next ==> move table");
				m_bJustAlign = FALSE;
			}
			else
			{
				// no die left, to change to normal prescan mode
				SetWTStable(FALSE);
				ClearGoodInfo();
				LONG lLoop = 0;
				while( 1 )
				{
					LONG lState = (*m_psmfSRam)["BondHead"]["AtPrePick"];
					if( lState==1 )
						break;
					Sleep(10);
					lLoop++;
					if( lLoop>200 )
						break;
				}
				m_nPickListIndex = -1;
				pUtl->SetPrescanMapIndexIdle(FALSE);
				m_bPrescanningMode = TRUE;
				m_dStopTime = GetTime();

				if( m_lAreaPickStage==3 )
				{
					m_qSubOperation = PRESCAN_WAIT_ALL_DONE_Q;
					DisplaySequence("WFT - AP pick next ==> wait all done q");
				}
				if( m_lAreaPickStage==2 )
				{
					Sleep(500);
					m_lAreaPickStage = 1;
					ResetBuffer();	// to prescan mode
					SetWTStable(FALSE);

					m_lAreaPickScanDirection = -1;
					LONG lY = GetScanLastPosnY() - GetPrescanPitchY();
					LONG lX = m_lAreaPickScanUL_X + GetPrescanPitchX();
					pUtl->PrescanMoveLog("WFT - scan AP get next scan position after pick no die move direction -1");
					int nReturn = OpGetWaferPos_PrescanAreaPick(lX, lY);
					if( nReturn!=1 )
					{
						Sleep(m_lPrescanPrDelay);
						SetWTStable(TRUE);	// Trigger Wafer PR 
						m_qSubOperation = PRESCAN_WAIT_ALL_DONE_Q;
						m_lAreaPickStage = 3;
						DisplaySequence("WFT - AP pick next ==> prescan all done");
					}
					else
					{
						SetDieReady(FALSE);
						SetWTStable(FALSE);
						m_qSubOperation = PRESCAN_MOVE_TABLE_Q;
						DisplaySequence("WFT - AP pick next ==> prescan move table q");
					}
				}

				SetEjectorVacuum(FALSE);	// off to let prescan go
				(*m_psmfSRam)["WaferTable"]["PickDie"]	= FALSE;	// reset firstly, should at get wafer pos
				(*m_psmfSRam)["WaferTable"]["IndexTime"]			= 2000;
				(*m_psmfSRam)["WaferTable"]["IndexStart"]			= 0;
			}
			break;	// get area pick position and wait bin full finish

		case PRESCAN_AP_MOVE_TABLE_Q:	// move table to target die and let bh to pick but wafer pr freeze.
			if (IsBHStopped())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				DisplaySequence("WFT - scan AP pick move table ==> house keeping");
				break;	// stopped
			}

			if ( AllowMove() == TRUE )
			{
				CString szMsg;
				szMsg.Format("WFT - scan AP move table index %d", m_nPickListIndex);
				DisplaySequence(szMsg);
				OpMoveTable_AreaPick();	// ejecotr vacuum, move and confirm search(if have), pr delay.
			}
			else
			{
				break;
			}

			if( WaitDieInfoRead() )
			{
				SetDieInfoRead(FALSE);
				SetDieReadyForBT(TRUE);
				m_bBTInfoRead = TRUE;
				DisplaySequence("WFT - scan AP move table, die info read ");
			}
			else
			{
				m_bBTInfoRead = FALSE;
			}

			m_qSubOperation = PRESCAN_AP_WAIT_READY_Q;
			DisplaySequence("WFT - scan AP move table ==> wait die ready q");
			break;	// move table for area pick q

		case PRESCAN_AP_WAIT_READY_Q:	// wait die picked away
			if (IsBHStopped())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				DisplaySequence("WFT - scan AP wait die ready ==> house keeping");
				break;	// operator press stop
			}

			bFull = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinFull"];
			if( m_bStopBinLoader!=TRUE && IsBLEnable() && bFull )
			{
				DisplaySequence("WFT - scan AP wait die ready bin full");
				break;
			}

			if ( m_bBTInfoRead==FALSE )				// Check whether the die info is written
			{
				if (WaitDieInfoRead())
				{
					SetDieInfoRead(FALSE);
					SetDieReadyForBT(TRUE);
					m_bBTInfoRead = TRUE;
					DisplaySequence("WFT - scan AP wait die ready ==> wait die info read done");
				}
				else
				{
					break;	// bt side die info read
				}
			}

			if( WaitEjectorReady() )
			{
				SetEjectorReady(FALSE);
				m_qSubOperation = PRESCAN_AP_NEXT_DIE_Q;
				DisplaySequence("WFT - scan AP wait die ejector ready ==> pick next die q");
			}
			break;	// wait die picked q

		default:
			DisplaySequence("WT - Scan Unknown");
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
				szMsg.Format("WT - Heap corrupted (%d) before WT", nHeapStatus);
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
		SaveScanTimeEvent("    WFT: scan exception executed");
		(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
	}
}


