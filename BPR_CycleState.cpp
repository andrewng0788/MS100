/////////////////////////////////////////////////////////////////
// BPR_CycleState.cpp : Cycle State of BondPrStn
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
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondPr.h"
#include "CycleState.h"
#include "BPR_Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CString g_aszBPRState[20] =
	{	"Wait T Ready",		// WAIT_BHT_READY_Q
		"Wait BT Stable",	// WAIT_BT_STABLE_Q
		"Grab Image",		// GRAB_IMAGE_Q
		"Wait Grab Done",	// WAIT_GRAB_DONE_Q
		"Inspection",		// INSPECTION_Q
		"House Keeping",	// HOUSE_KEEPING_Q
		"Inspect Last Die"	// INSPECT_LAST_DIE_Q
	};

VOID CBondPr::AutoOperation()
{
	RunOperation();
}

VOID CBondPr::DemoOperation()
{
	RunOperation();
}

VOID CBondPr::RunOperation()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	BOOL	bGrabResult;
	LONG	lSPCResult;
	BOOL	bMS60LFCycle = FALSE;		//andrewng //2020-0730
	CString szMsg;


	if ((Command() == glSTOP_COMMAND) &&
		(Action() == glABORT_STOP))
	{
		DisplaySequence("BPR - STOP");
		State(STOPPING_Q);
	}
	else		
	{
		if (m_bStopAllMotion == TRUE)
		{
			if (m_qSubOperation != INSPECT_LAST_DIE_Q)
				m_qSubOperation = HOUSE_KEEPING_Q;
		}


		try
		{
			m_nLastError = gnOK;
			switch (m_qSubOperation)
			{
			case WAIT_BT_STABLE_Q:
				DisplaySequence("BPR - BT stable ==> Wait BHT Ready");
				if (IsBHStopped())	//v3.67T3
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if (WaitBTStable())
				{
TakeTime(LC5);
					SetBTStable(FALSE);
					//SetWPRGrabImageReady(FALSE, "BPR RESET for NEXT cycle");	//andrewng //2020-0730

					//andrewng //2020-0615
					if (IsBurnIn() && !pApp->m_bBurnInGrabImage)
					{
					}
					else
					{
						//m_bMissingDie = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MissingDie"];
						/*if (m_bUseHWTrigger && m_bUsePostBond)	// && (m_bMissingDie == FALSE))
						{
							DisplaySequence("BPR - BT stable ==> WAIT_BHT_READY_Q (PreAutoSearchDie_HWTri)");
							bGrabResult = PreAutoSearchDie_HWTri();
							if (!bGrabResult)
							{
								DisplaySequence("BPR - BT stable ==> house keeping (PreAutoSearchDie_HWTri)");
								m_qSubOperation = HOUSE_KEEPING_Q;
								break;
							}
							m_unHWGrabState = 1;	//=PreGrabDone
						}*/
					}

					m_qSubOperation = WAIT_BHT_READY_Q;	
				}
				break;


			case WAIT_BHT_READY_Q:
				DisplaySequence("BPR - wait BHT ready");
				if (IsBHStopped())	//v3.67T3
				{
					SetBPRLatched(TRUE);
					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("BPR - BHT ready ==> house keeping");
				}
				else if (m_bPostBondAtBond && WaitBhTReadyForBPR())		//v2.60
				{
					DisplaySequence("BPR - m_bPostBondAtBond WaitBhTReadyForBPR");

					SetBhTReadyForBPR(FALSE);
					if (AutoSearchDie())
					{
						if (AutoGrabDieDone())
						{
							AutoLogDieResult();		//Log data to files
						}
					}
				}
				else if (WaitBhReadyForBPRHwTrigger())		//andrewng //2020-0618
				{
					SetBhReadyForBPRHwTrigger(FALSE);
					if (m_bUseHWTrigger)
					{
						if (IsBurnIn() && !pApp->m_bBurnInGrabImage)
						{
						}
						else
						{
							PreAutoSearchDie_HWTri(TRUE);
						}
					}
				}
				else if (WaitBhTReady())	// || m_bDisableBT )	//v4.58A6
				{
					SetBPRPostBondDone(FALSE);
					SetBhTReady(FALSE);
					CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
					if( pUtl->GetPrescanAreaPickMode() && m_lAPLastDone>=1 )
					{
						if( m_lAPLastDone==1 )
							m_lAPLastDone = -1;
						else
                            m_lAPLastDone = 0;
						SetBPRLatched(TRUE);
						(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
						break;
					}

					if (m_bPostBondAtBond)	
					{
						TakeTime(BPR2);	
						//if (!m_bReSearchCurrDie)
						SetBPRLatched(TRUE);	
						SetBPRPostBondDone(TRUE);	//Trigger DBH task in PICK_DELAY state to continue next cycle
						(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
						TakeTime(BPR3);	
						m_qSubOperation = WAIT_BT_STABLE_Q;
						break;
					}

					m_qSubOperation = WAIT_WPR_GRAB_IMAGE_READY_Q;	//GRAB_IMAGE_Q;	
					OpUpdateDieIndex();
				}
				break;


			case WAIT_WPR_GRAB_IMAGE_READY_Q:
				
				DisplaySequence("BPR - WAIT_WPR_GRAB_IMAGE_READY_Q");
				if (IsBHStopped())
				{
					SetBPRLatched(TRUE);
					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("BPR - WAIT_WPR_GRAB_IMAGE_READY_Q ==> house keeping");
				}
				else if (WaitBhReadyForBPRHwTrigger())		//andrewng //2020-0618
				{
					SetBhReadyForBPRHwTrigger(FALSE);
					if (m_bUseHWTrigger)
					{
						if (IsBurnIn() && !pApp->m_bBurnInGrabImage)
						{
						}
						else
						{
							PreAutoSearchDie_HWTri(TRUE);
						}
					}

					bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];
					//if (!bMS60LFCycle)
					//{
					//}
				}
				else if (WaitWPRGrabImageReady())
				{
					SetWPRGrabImageReady(FALSE, "WAIT_WPR_GRAB_IMAGE_READY_Q");
					m_qSubOperation = GRAB_IMAGE_Q;
					DisplaySequence("BPR - WAIT_WPR_GRAB_IMAGE_READY_Q ==> GRAB_IMAGE_Q");
				}
				break;


			case GRAB_IMAGE_Q:
				//Check die is bonded on BT or not
				m_bMissingDie = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MissingDie"];
	
				if (!pApp->m_bBurnInGrabImage && IsBurnIn())
				{
					DisplaySequence("BPR - IsBurnIn(), m_bBurnInGrabImage = FALSE");
					Sleep(1);
					SetBPRLatched(TRUE);
					SetBPRPostBondDone(TRUE);	//Trigger DBH task in PICK_DELAY state to continue next cycle
					(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;

					m_qSubOperation = WAIT_BT_STABLE_Q;	
					break;
				}

				if ( (m_bUsePostBond == TRUE) && (m_bMissingDie == FALSE) )
				{
					TakeTime(BPR1);			// Take Time

					if ( m_fHardware == FALSE )
					{
						m_qSubOperation = WAIT_GRAB_DONE_Q;	
						break;
					}


					if (m_bUseHWTrigger)
					{
						//DisplaySequence("BPR - PreAutoSearchDie_HWTri (HW)");
						//if (PreAutoSearchDie_HWTri())
						//{
							//m_unHWGrabState = 1;	// = HW-Trigger Done
							//Sleep(30);
						if (m_unHWGrabState != 1)
						{
							DisplaySequence("BPR - auto search die (HW) fail due to PreAutoSearchDie_HWTri");
							bGrabResult = FALSE;
							m_unHWGrabState = 0;
						}
						else
						{
							DisplaySequence("BPR - grab image q, pb auto search die (HW)");
							bGrabResult = AutoSearchDie_HWTri(TRUE);
							if (bGrabResult)
							{
								m_unHWGrabState = 2;	// = HW-Trigger Done
							}
							else
							{
								DisplaySequence("BPR - auto search die (HW) -> FAIL");
								bGrabResult = FALSE;
								m_unHWGrabState = 0;	// = HW-Trigger Done
							}
						}
						//}
						//else
						//{
						//	bGrabResult = FALSE;
						//}
						TakeTime(LC2);
					}
					else
					{
						DisplaySequence("BPR - grab image q, pb auto search die");
						bGrabResult = AutoSearchDie();
					}
				}
				else
				{
					DisplaySequence("BPR - grab image q, no pb or missing die, auto grab image");
					TakeTime(BPR1);			// Take Time
					bGrabResult = AutoGrabImage();
					TakeTime(LC8);
				}

				if ( m_bDisableBT )
				{
					bGrabResult = TRUE;
				}
				if (bGrabResult == TRUE)
				{
					m_qSubOperation = WAIT_GRAB_DONE_Q;	
				}
				else
				{
					m_qSubOperation = HOUSE_KEEPING_Q;	
				}
				break;

			case WAIT_GRAB_DONE_Q:
				if ( (m_bUsePostBond == TRUE) && (m_bMissingDie == FALSE) )
				{
					if ( m_fHardware == FALSE )
					{
						Sleep(20);
						TakeTime(BPR2);			// Take Time
						SetBPRLatched(TRUE);
						m_qSubOperation = INSPECTION_Q;	
						break;
					}

					if (m_bUseHWTrigger)
					{
						DisplaySequence("BPR - Grab Done (HW), get rpy 1");
						TakeTime(LC3);
						bGrabResult = AutoGrabDieDone_HWTri();
						//m_unHWGrabState = 2;	// = HW-Trigger Done
					}
					else
					{
						DisplaySequence("BPR - Grab Done, get rpy 1");
						bGrabResult = AutoGrabDieDone();
					}
				}
				else
				{
					DisplaySequence("BPR - Grab Done, just pass");
					bGrabResult = TRUE;
				}

				if (bGrabResult == TRUE)
				{
					//For HW-Trigger fcn, this event is set inside AutoSearchDie_HWTri();
					if (!m_bUseHWTrigger)
					{
						TakeTime(BPR2);		
						SetBPRLatched(TRUE);	//Allow BT task to OpMoveTable
					}
					m_qSubOperation = INSPECTION_Q;	
				}
				else
				{
					m_qSubOperation = HOUSE_KEEPING_Q;	
				}
				break;

			case INSPECTION_Q:
				if ( (m_bUsePostBond == TRUE) && (m_bMissingDie == FALSE) )
				{
					bGrabResult = TRUE;
					
					if ( m_fHardware == FALSE )
					{
						Sleep(30);
						(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
						TakeTime(BPR3);		// Take Time
						m_qSubOperation = WAIT_BT_STABLE_Q;
						DisplaySequence("BPR - Inspect 1 no HW ==> Wait BT Stable");
						break;
					}

					if (m_bUseHWTrigger)
					{
						if (m_unHWGrabState == 2)
						{
							PR_UWORD usDType = 0;
							PR_COORD stDOffset;
							PR_REAL fDRotate = 0;
							PR_REAL fDScore = 0;

							DisplaySequence("BPR - PB Inspection (HW)");
							lSPCResult = AutoDieResult_HWTri(&usDType, &fDRotate, &stDOffset, &fDScore);
						
							m_unHWGrabState = 0;	// Reset to Not-Trigger
						}
						else
						{
							DisplaySequence("BPR - PB Inspection (HW) fail due to AutoSearchDie_HWTri");
							m_unHWGrabState = 0;	// Reset to Not-Trigger
						}
					}
					else
					{
						DisplaySequence("BPR - PB Inspection");
						lSPCResult = AutoDieResult();

						//v4.26T1	//Semitek
						if (lSPCResult == TRUE)
						{
							lSPCResult = OpCheckRelOffsetinFOV();	
						}
					}
				}
				else
				{
					DisplaySequence("BPR - Inspection, grab done only");
					bGrabResult = AutoGrabDone();

					if ( m_fHardware == FALSE )
					{
						Sleep(25);
						(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
						TakeTime(BPR3);		// Take Time
						m_qSubOperation = WAIT_BT_STABLE_Q;	
						DisplaySequence("BPR - inspection no HD ==> Wait BT Stable");
						break;				
					}

					lSPCResult = 1;
				}

				if( m_bDisableBT )
				{
					bGrabResult = TRUE;
					lSPCResult = 1;
				}

				if (bGrabResult == FALSE)
				{
					m_qSubOperation = HOUSE_KEEPING_Q;	
				}
				else
				{
					SetBPRPostBondDone(TRUE);	//Trigger DBH task in PICK_DELAY state to continue next cycle
					(*m_psmfSRam)["BondPr"]["PostBondResult"] = lSPCResult;
					TakeTime(BPR3);
					CString szMsg;
					szMsg.Format(" (PB=%d)", lSPCResult);
					DisplaySequence("BPR - Inspection ==> Wait BT Stable" + szMsg);
					m_qSubOperation = WAIT_BT_STABLE_Q;	
				}
				break;

			case HOUSE_KEEPING_Q:
				DisplaySequence("BPR - House Keeping Q");
				SetAutoCycleLog(FALSE);
				if (GetHouseKeepingToken("BPRStn") == TRUE)
				{
					SetHouseKeepingToken("BPRStn", FALSE);
				}
				else
				{
					Sleep(10);
					break;
				}

				if (m_bUseHWTrigger)	//andrewng //2020-0615
				{
					if (m_unHWGrabState == 1)
					{
						if (AutoSearchDie_HWTri() == TRUE)
						{
							Sleep(15);
							AutoGrabDieDone_HWTri();

							PR_UWORD usDType = 0;
							PR_COORD stDOffset;
							PR_REAL fDRotate = 0;
							PR_REAL fDScore = 0;

							DisplaySequence("BPR - PB Inspection (HW) at House Keeping Q");
							lSPCResult = AutoDieResult_HWTri(&usDType, &fDRotate, &stDOffset, &fDScore);
						
							m_unHWGrabState = 0;	// Reset to Not-Trigger
						}
					} 
					else if (m_unHWGrabState == 2)
					{
						PR_UWORD usDType = 0;
						PR_COORD stDOffset;
						PR_REAL fDRotate = 0;
						PR_REAL fDScore = 0;

						Sleep(15);
						DisplaySequence("BPR - PB Inspection 2 (HW) at House Keeping Q");
						lSPCResult = AutoDieResult_HWTri(&usDType, &fDRotate, &stDOffset, &fDScore);
						
						m_unHWGrabState = 0;	// Reset to Not-Trigger
					}
				}
				StopCycle("BPRStn");
				
				SetHouseKeepingToken("BPRStn", TRUE);

				m_qSubOperation = INSPECT_LAST_DIE_Q;
				break;

			case INSPECT_LAST_DIE_Q:
				DisplaySequence("BPR - Inspect Last Die");

				//Check die is bonded on BT or not
				m_bMissingDie = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MissingDie"];
				if ( (m_bUsePostBond == TRUE) && (m_bMissingDie == FALSE) && m_fHardware)
				{
					if ( IsBHStopped() != 1 )
					{
						Sleep(10);	
						break;
					}

					if (m_bUseHWTrigger)	//andrewng //2020-0616
					{
						/*
						LogCycleStopState("BPR - Auto Search Die (HW) SPC Last Die");
						if (PreAutoSearchDie_HWTri())
						{
							Sleep(15);
							AutoSearchDie_HWTri();
							LogCycleStopState("BPR - Auto Grab Done (HW) SPC Last Die");
							AutoGrabDieDone_HWTri();
							LogCycleStopState("BPR - Auto Grab Die Result (HW) SPC Last Die");

							PR_UWORD usDType = 0;
							PR_COORD stDOffset;
							PR_REAL fDRotate = 0;
							PR_REAL fDScore = 0;
							AutoDieResult_HWTri(&usDType, &fDRotate, &stDOffset, &fDScore);
						}*/
					}
					else
					{
						LogCycleStopState("BPR - Auto Search Die SPC Last Die");
						AutoSearchDie();
						LogCycleStopState("BPR - Auto Grab Done SPC Last Die");
						AutoGrabDieDone();
						LogCycleStopState("BPR - Auto Grab Die Result SPC Last Die");
						AutoDieResult();
					}

					LogCycleStopState("BPR - stop completely");
				}
				else
				{
					if (m_bDisableBT)
					{
						LogCycleStopState("BPR - stopped");
					}
					else
					{
						LogCycleStopState("BPR - SPC Last Die off");
					}
				}
				CMS896AStn::m_lBondPrStopped = 1;
				State(STOPPING_Q);
				break;

			default:
				DisplaySequence("BPR - Unknown");
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
					szMsg.Format("BPR - Heap corrupted (%d) before BPR [%s]", nHeapStatus, 
								g_aszBPRState[m_qSubOperation]);
					DisplayMessage(szMsg);
					
					LogCycleStopState(szMsg);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			Result(gnNOTOK);

			CMS896AStn::m_lBondPrStopped = 1;
			State(STOPPING_Q);

			NeedReset(TRUE);
			StopCycle("BPRStn");
			SetMotionCE(TRUE, "BPR Run Operation Stop");
		}
	}
}

VOID CBondPr::CycleOperation()
{
	if (Command() == glAMS_STOP_COMMAND)
	{
		State(STOPPING_Q);
	}
}
