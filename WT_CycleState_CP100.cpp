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


VOID CWaferTable::RunOperation_CP(CONST BOOL bFastDaily)
{
	BOOL	bNoMotionHouseKeeping = FALSE;

	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		DisplaySequence("CWT - STOP");
		State(STOPPING_Q);
		return ;
	}

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	try
	{
		m_nLastError	= gnOK;

		if (m_bStopAllMotion == TRUE)
		{
			if (m_qSubOperation != CP_UPDATE_LAST_DIE_Q)
				m_qSubOperation = CP_WT_HOUSE_KEEPING_Q;

			bNoMotionHouseKeeping = TRUE;
		}

		// Operation state
		switch (m_qSubOperation)
		{
		case CP_GET_WAFER_POS_Q:		// Get the die position from Wafer Map
			DisplaySequence("CWT - Get Wafer Position");

			m_nLastError = OpGetWaferPos_CP(bFastDaily);
			if( m_bWaferRepeatTest && m_bWaferNonStopTest && m_nLastError!=gnOK )
			{
				//m_ulNewPickCounter = 0;
				DeleteFile(CP_PROBE_TEMP_FILE);		DeleteFile(CP_ITEM_GRADE_DATA_FILE);
				m_WaferMapWrapper.StartMap();
				m_WaferMapWrapper.RestartMap();

				UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
				UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
				UCHAR ucDefectGrade = GetScanDefectGrade() + ucOffset;
				UCHAR ucBadcutGrade = GetScanBadcutGrade() + ucOffset;
				UCHAR ucProbeGrade = ucOffset + m_ucDummyPrescanPNPGrade;

				m_WaferMapWrapper.StopMap();
				ObtainMapValidRange();
				if( IsBlockProbe() )
				{
					m_lLastProbeRow = GetMapValidMinRow();
					m_lLastProbeCol	= GetMapValidMinCol();
					m_bBlockProbingToRight	= TRUE;			//	block probing
					m_lBlockProbeLoopRow	= 0;
					m_lBlockProbeLoopCol	= 0;
				}	//	block probe
				for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
				{
					for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
					{
						UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
						if( ucGrade==ucNullBin || ucGrade==ucDefectGrade || ucGrade==ucBadcutGrade )
						{
							continue;
						}
						if( m_WaferMapWrapper.GetReader()->IsDefectiveDie(ulRow, ulCol) )
						{
							continue;
						}
						ULONG ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
						if( IsDieUnpickAll(ulDieState) || (ulDieState==WT_MAP_DIESTATE_SCAN_EDGE) )
						{
							continue;
						}

						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucProbeGrade);
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_DEFAULT);
					}
				}

				UCHAR aaTempGrades[5];
				aaTempGrades[0] = ucProbeGrade;

				m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
				m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);
				PrescanRealignWafer();
				m_WaferMapWrapper.StartMap();

				SetWTStable(FALSE);
				SetWTReady(FALSE);
				SetWTBacked(FALSE);
				m_ulPickingBlockIndex	= 0;

				m_bIsPrescanning = FALSE;

				m_nLastError = OpGetWaferPos_CP(FALSE);
			}

			if ((m_nLastError) == gnOK)
			{
				if( IsBlockProbe() )
				{
				if ( m_bFirstPrb )	// first cycle move to ISP or PD
				{
					DisplaySequence("CWT - GET WAFER POS FIRST ==> reset buffer");
					ResetBuffer();	// to sorting mode
				}
				DisplaySequence("CWT - Get Wafer Pos ==> move table q");
				m_qSubOperation = CP_MOVE_TABLE_Q;
				}	//	block probe
				else
				{
					if ( m_bFirstPrb )	// first cycle move to ISP or PD
					{
						DisplaySequence("CWT - GET WAFER POS FIRST ==> move table q");
						ResetBuffer();	// to sorting mode

						m_qSubOperation = CP_MOVE_TABLE_Q;
			}
			else
			{
						DisplaySequence("CWT - Get Wafer Pos ==> Wait probe z Ready");
						// Log the status
						m_qSubOperation = CP_WAIT_PROBEZ_READY_Q;
					}
				}
			}
			else
			{
				m_qSubOperation = CP_WAFER_END_HANDLING_Q;
				CMSLogFileUtility::Instance()->WT_GetIdxLog("into wafer end handling q");
			}
			break;

		case CP_WAIT_PROBEZ_READY_Q:
			DisplaySequence("CWT - wait probe z ready q");

			if (m_bStop)
			{
				m_qSubOperation = CP_WT_HOUSE_KEEPING_Q;
				m_bNextDieIsGet = TRUE;		//To make sure Update Die is called in HouseKeeping since Get NextDie is already called
				break;
			}

			if( WaitProbeZReady() )	// probe z down to safe, let WT begin move
//			if (WaitEOT_Done())
			{
				TakeTime((TIME_ENUM) UTB);
				SetProbeZReady(FALSE);
//				DisplaySequence("CWT - wait probe z ready q update map");
//				SetEOT_Done(FALSE);	//	EOT ok.

				// but now bh change to another position. XUXUXU
				// get the probe grade
				// Update the last die pos wafer map
				OpUpdateDie_CP(bFastDaily);		// Update the wafer map

				m_bNextDieIsGet = FALSE;

				if( CheckRuntimeUpRowProbeResult()==FALSE )
				{
					m_qSubOperation = CP_WT_HOUSE_KEEPING_Q;
					break;
				}

				TakeTime((TIME_ENUM) UTE);
//			}	//	EOT raw data process
//			if( WaitProbeZReady() )	 // probe z down to safe, let WT begin move
//			{
//				SetProbeZReady(FALSE);
				TakeTime((TIME_ENUM) CycleTime);
				StartTime(GetRecordNumber());		// Restart the time						

				if( IsBlockProbe() )
				{
					DisplaySequence("CWT - Probe Z ready and Data done ==> get wafer posn q");
					m_qSubOperation = CP_GET_WAFER_POS_Q;
				}	//	block probe
				else
				{
					DisplaySequence("CWT - Probe Z ready and Data done ==> move table");
					m_qSubOperation = CP_MOVE_TABLE_Q;
				}
			}
			else if (m_bProbeGoodDie == FALSE)
			{
//				SetEOT_Done(TRUE);
				SetProbeZReady(TRUE);	// let good die, no probing, let WT move
			}
			break;

		case CP_MOVE_TABLE_Q:
			DisplaySequence("CWT - Move Table");

			if( m_bWaferRepeatTest && m_lRepeatTestLimit>0 && GetNewPickCount()>=(ULONG)m_lRepeatTestLimit )
			{
				m_qSubOperation = CP_WAFER_END_HANDLING_Q;
				break;
			}

			if( m_bProbeOver1K==FALSE )
			{
				if( GetNewPickCount()>1000 || GetScanMethod()==CP_SCAN_CONT_PROBE_WAFER )
				{
					m_bProbeOver1K = TRUE;
					WaferMapTempOperation(1);	// probing die over 1K, back up and set flag for warm start.
				}
			}

			if ( AllowMove() == TRUE )
			{
				// check whether the contact sensor is on before move table
				if (IsContactSensorsOn(TRUE) == TRUE)
				{
					SetAlert_Red_Back(IDS_WT_PROBE_TABLE_NOT_DOWN);
					m_qSubOperation = CP_WT_HOUSE_KEEPING_Q;
					break;
				}

				m_nLastError = OpMoveTable_CP(bFastDaily);
				if (m_nLastError == gnOK) // move table to target position
				{
					m_qSubOperation = CP_WAIT_PR_DELAY_Q;
				}
				else
				{
					SetAlert_Red_Yellow(IDS_WT_INVALID_POS);		// Set Alarm
					SetErrorMessage("Invalid die position OR die outside Wafer Limit");

					m_qSubOperation = CP_WT_HOUSE_KEEPING_Q;
				}
			}
			break;

		case CP_WAIT_PR_DELAY_Q:
			DisplaySequence("CWT - Wait PR Delay");
#ifdef NU_MOTION
			if (IsBHStopped())	//v3.67T3
#else
			if (m_bStop)
#endif
			{
				m_qSubOperation = CP_WT_HOUSE_KEEPING_Q;
				break;
			}

			if (m_lWTDelay > 0)
			{
				Sleep(m_lWTDelay);
			}

			DisplaySequence("CWT - WT delay ==> Wait Die Ready");

			// Update current die's PR status as Good value
			m_bProbeGoodDie = GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK;

			if ( m_bProbeGoodDie )
			{
				m_stCurrentDie.ulStatus = WT_MAP_DIESTATE_PICK;
			}

			if( IsBurnIn() || m_bProbeGoodDie )
			{
				SetWTReady(TRUE);
			}

			GetEncoderValue();	
			m_stCurrentDie.lX_Enc = GetCurrX();
			m_stCurrentDie.lY_Enc = GetCurrY();
			m_stCurrentDie.lT_Enc = GetCurrT();

			m_qSubOperation = CP_WT_NEXT_DIE_Q;
			break;


		case CP_WT_NEXT_DIE_Q:
			DisplaySequence("CWT - Next Die");
			m_bFirstPrb = FALSE;
			m_nLastError = OpNextDie_CP(bFastDaily);
			if (m_nLastError == gnOK)
			{
				m_bNextDieIsGet = TRUE;
				if( IsBlockProbe() )	//	block probe
					m_qSubOperation = CP_WAIT_PROBEZ_READY_Q;
				else
					m_qSubOperation = CP_GET_WAFER_POS_Q;
			}
			else
			{
				m_stLastDie = m_stCurrentDie;
				CMSLogFileUtility::Instance()->WT_GetIdxLog("OP NEXT fail last restore to current");

				m_qSubOperation = CP_WAFER_END_HANDLING_Q;
			}
			break;

		case CP_WAFER_END_HANDLING_Q:
			DisplaySequence("CWT - Wafer Probing End");

			UpdateWaferLotLoadStatus();
			OpDisplayWaferEnd_CP();

			m_qSubOperation = CP_WT_HOUSE_KEEPING_Q;

			break;

		case CP_WT_HOUSE_KEEPING_Q:
			DisplaySequence("CWT - House Keeping Q");

			m_stStopTime = CTime::GetCurrentTime();	// machine sorting stopped

			if (GetHouseKeepingToken("WaferTableStn") == TRUE)
			{
				SetHouseKeepingToken("WaferTableStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}

			m_bCPItemMapToTop		= FALSE;
			StopCycle("WaferTableStn");
			Result(gnNOTOK);

			//Disable 1st die finder if machine stop
			SetHouseKeepingToken("WaferTableStn", TRUE);

			// state to home Ej & BHZ before WT to UNLOAD during WaferEnd!!
			if( bNoMotionHouseKeeping == FALSE )
			{
				LONG lTryLimit = 0;
				LogCycleStopState("CWT - WAFEREND to wait BH at safe");
				while( IsBHStopped()!=1 )
				{
					Sleep(100);
					lTryLimit++;
					if( lTryLimit>3000 )	// in case cleaning pin
					{
						break;
					}
				}
				LogCycleStopState("CWT - WAFEREND to UNLOAD pos");
				Sleep(100);

				BOOL bMoveWT = TRUE;
				if (IsContactSensorsOn(TRUE) == TRUE)
				{
					SetAlert_Red_Back(IDS_WT_PROBE_TABLE_NOT_DOWN);
					bMoveWT = FALSE;
				}
				else
				{
					if( IsProber() && GetNewPickCount()==0 )
						;
					else
						OpRotateBack();
				}

				if( m_bIsWaferEnded && bMoveWT && m_bStopUnloadInWaferEnd == FALSE )
				{
					X_Profile(CP_VLOW_PROFILE);
					Y_Profile(CP_VLOW_PROFILE);

					// Move to unload
					if ( m_bDisableWL==FALSE )
					{
						XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
					}
					else
					{
						XY1_MoveTo(m_lWaferUnloadX, m_lWaferUnloadY, SFM_WAIT);

						SetWaferTableVacuum(FALSE);
						T_Home();
					}

					X_Profile(NORMAL_PROF);
					Y_Profile(NORMAL_PROF);

					if( IsBurnInCP() )
					{
						Sleep(5000);
					}
				}
			}

			SwitchPrLighting(TRUE);	// house keeping, turn on lighting for idle.

			if( m_bIsWaferProbed && m_bReCheckAfterWaferEnd==FALSE )
			{
				GenWaferEndOutputMapFile();
			}
			else if( GetNewPickCount()>0 )
			{
				WaferMapTempOperation(2);	// normal stop, backup reset flag for warm start.
			}

			if( !IsBurnIn()&& m_bMapDummyPrescanAlign && m_bIsWaferProbed && m_bReCheckAfterWaferEnd )
			{
				SetAlert_Red_Yellow(IDS_WT_END); 
			}

			CMSLogFileUtility::Instance()->MS_LogCycleState("stop sorting");
			DisplaySequence("CWT - House Keeping ==> Update Last Die");

			m_qSubOperation = CP_UPDATE_LAST_DIE_Q;
			break;

		case CP_UPDATE_LAST_DIE_Q:	// need to follow up as it is different to normal sort
			DisplaySequence("CWT - update last die");

			LogCycleStopState("CWT - Update Last Die");

			if ( m_bNextDieIsGet == TRUE )	// get and not (probed or update)
			{
				WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
				ULONG ulRow, ulCol;
				UCHAR ucGrade;
				BOOL bEndOfWafer;
				if ( m_bIsProbeErrorInCycle == TRUE )
				{
					//Chris:change grade 
					m_bIsProbeErrorInCycle = FALSE;
					
					LONG lastX = 0, lastY = 0;
					ULONG ulLastRow = (ULONG)GetLastDieRow();
					ULONG ulLastCol = (ULONG)GetLastDieCol();
					ConvertAsmToOrgUser(ulLastRow, ulLastCol, lastX, lastY);
					UpdateMap_CP(ulLastRow, ulLastCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT, GetLastDieEAct(), m_stLastDie.ulStatus);
					m_WaferMapWrapper.AddDie(ulLastRow, ulLastCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT + m_WaferMapWrapper.GetGradeOffset());

					CString szMsg;
					szMsg.Format("%d %d %d %d",lastX, lastY, ulLastRow, ulLastCol);
					CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
				}
				else
				{
					OpUpdateDie_CP(bFastDaily);

					if( bFastDaily==FALSE )
					{
						PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);

						LONG lX = (LONG)(*m_psmfSRam)["BondHeadPBT"]["Die Probed X"];
						LONG lY = (LONG)(*m_psmfSRam)["BondHeadPBT"]["Die Probed Y"];
						if ( (GetLastDieRow() != lY) || (GetLastDieCol() != lX) )
						{
							OpRollBack();
							PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
							if ((GetLastDieRow() != ulRow) || (GetLastDieCol() != ulCol) )
								OpRollBack();
						}
					}
				}

				if( bFastDaily==FALSE )
					PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
			}

			LogCycleStopState("CWT - Stop completely");
			State(STOPPING_Q);
			break;

		default:
			DisplaySequence("CWT - Unknown");
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
				szMsg.Format("CWT - Heap corrupted (%d) before WT [%s]", nHeapStatus, 
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
		(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
	}
}	// run operation for CP normal probing

