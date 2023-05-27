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

VOID CWaferTable::RunOperationWft_ES()
{
	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		DisplaySequence("WTNG - STOP");
		State(STOPPING_Q);
		return ;
	}

	BOOL	bNoMotionHouseKeeping = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG lmPRDelay = 0;

	try
	{
		if (m_bStopAllMotion == TRUE)
		{
			if (m_qSubOperation != UPDATE_LAST_DIE_Q)
				m_qSubOperation = HOUSE_KEEPING_Q;

			bNoMotionHouseKeeping = TRUE;
		}

		// Operation state
		switch (m_qSubOperation)
		{
		case GET_WAFER_POS_Q:		// Get the die position from Wafer Map
			DisplaySequence("WTNG - Get Wafer Position");

			m_nLastError = OpGetWaferPos_ES();

			if ((m_nLastError) == gnOK)
			{
				if ( m_bFirst == TRUE )
				{
					m_qSubOperation = MOVE_TABLE_Q;
				}
				else
				{
					// update next motion time, inform BH to prepick
					m_qSubOperation = WAIT_EJECTOR_READY_Q;
					DisplaySequence("WTNG - Get Wafer Pos ==> Wait Ejector Ready");
				}
			}
			else
			{
				OpUpdateLastDie_ES();			// Update the wafer map
				m_bDieIsGet = FALSE;

				m_qSubOperation = WAFER_END_HANDLING_Q;
			}
			break;

		case WAIT_EJECTOR_READY_Q:	
			if (m_bStop)
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				m_bDieIsGet = TRUE;		//To make sure Update Die is called in HouseKeeping since Get NextDie is already called
				break;
			}

			if (WaitEjectorReady())
			{
				SetEjectorReady(FALSE);

				OpUpdateLastDie_ES();				// Update the wafer map
				m_bDieIsGet = FALSE;

				m_qSubOperation = MOVE_TABLE_Q;
				DisplaySequence("WTNG - ejector ready ==> move table");
			}
			else if (m_bGoodDie == FALSE)
			{
				LONG lState = (*m_psmfSRam)["BondHead"]["AtPrePick"];
				if (lState == 1)
				{
					SetEjectorReady(TRUE);
				}
			}
			break;

		case MOVE_TABLE_Q:
			DisplaySequence("WTNG - Move Table");

			if ( AllowMove() == TRUE )
			{
				TakeTime(EV1);			
				SetEjectorVacuum(FALSE);		// Release the vacuum

				//VAC off delay re-added for Excelitas
				LONG lVacOffDelay = (*m_psmfSRam)["WaferTable"]["VacOffDelay"];
				if ( (lVacOffDelay > 0) && (lVacOffDelay < 10000) )
				{
					Sleep(lVacOffDelay);
				}

				if ((m_nLastError = OpMoveTable_ES()) == gnOK) // move table to target position
				{
					m_qSubOperation = WAIT_PR_DELAY_Q;
				}
				else
				{
					SetAlert_Red_Yellow(IDS_WT_INVALID_POS);		// Set Alarm
					SetErrorMessage("Invalid die position OR die outside Wafer Limit");

					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			break;


		case WAIT_PR_DELAY_Q:
			DisplaySequence("WTNG - Wait PR Delay");

			lmPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];
			if( lmPRDelay>0 )
			{
				Sleep(lmPRDelay);
			}

			if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
			{
				SetWTReady(TRUE);		//Allow BH Z1/2 to move down earilier because no need to wait for compensation  
				(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;
			}
			else	
			{
				(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;
			}

			GetEncoderValue();	
			m_stCurrentDie.lX_Enc = GetCurrX();
			m_stCurrentDie.lY_Enc = GetCurrY();
			m_stCurrentDie.lT_Enc = GetCurrT();
			(*m_psmfSRam)["WaferPr"]["WTEncX"] = GetCurrX();	//v4.42T15
			(*m_psmfSRam)["WaferPr"]["WTEncY"] = GetCurrY();	//v4.42T15

			m_qSubOperation = WAIT_DIE_READY_Q;
			DisplaySequence("WTNG - PR delay ==> Wait Die Ready");
			break;


		case WAIT_DIE_READY_Q:
#ifdef NU_MOTION
			if (IsBHStopped())	//v3.67T3
#else
			if (m_bStop)
#endif
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			// Update current die's PR status as Good value
			m_stCurrentDie.ulStatus =  m_WaferMapWrapper.GetDieState(GetCurrentRow() , GetCurrentCol());

			OpStoreDieInfo();
			//BlockPick (with regular ref die)
			if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
			{
				m_stCurrentDie.ulStatus = WT_MAP_DIESTATE_PICK;
				SetEjectorVacuum(TRUE);			// Turn-on Ejector Vacuum after comp.
				TakeTime(EV2);
			}
			else
			{
				SetEjectorReady(TRUE);
			}

			m_bGoodDie = TRUE;
			m_qSubOperation = WT_NEXT_DIE_Q;
			break;

		case WT_NEXT_DIE_Q:
			DisplaySequence("WTNG - Next Die");
#ifdef NU_MOTION
			if (IsBHStopped())
#else
			if (m_bStop)
#endif
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			m_bFirst = FALSE;
			if ((m_nLastError = OpNextDie_ES()) == gnOK)
			{
				m_bDieIsGet = TRUE;
				m_qSubOperation = GET_WAFER_POS_Q;
			}
			else
			{
				m_stLastDie = m_stCurrentDie;

				OpUpdateLastDie_ES();		// Update the wafer map
				m_bDieIsGet = FALSE;
				m_qSubOperation = WAFER_END_HANDLING_Q;
			}
			break;


		case WAFER_END_HANDLING_Q:
			m_bIntoNgPickCycle	= FALSE;
			DisplaySequence("WTNG - Wafer End");

			remove("c:\\mapsorter\\userdata\\history\\WaferPrMsd.txt");

			//Cannot check BT last-die, else may dead-lock		//v3.68T5
			OpDisplayWaferEnd_ES();
			m_qSubOperation = HOUSE_KEEPING_Q;
			(*m_psmfSRam)["WaferTable"]["WaferEnd"] = TRUE;
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("WTNG - House Keeping Q");
			SetAutoCycleLog(FALSE);
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

			StopCycle("WaferTableStn");
			Result(gnNOTOK);

			//Disable 1st die finder if machine stop
			SetHouseKeepingToken("WaferTableStn", TRUE);

			if( IsBLInUse() )
			{
				DisplaySequence("WTNG - down BL again to sure it is safe");
				MoveES101BackLightZUpDn(FALSE);	// in HOUSE KEEPING, DOWN BL Z
			}

			// state to home Ej & BHZ before WT to UNLOAD during WaferEnd!!
			if( bNoMotionHouseKeeping == FALSE )	//	4.24TX 4
			{
				//v4.02T1	//Must wait DBH to HouseKeeping to make sure Ej at safe pos before moving WT
				LONG lTryLimit = 0;
				LogCycleStopState("WT - house keeping to wait BH at safe");
				while( IsBHStopped()!=1 )
				{
					Sleep(100);
					lTryLimit++;
					if( lTryLimit>3000 )	// over 300 seconds
					{
						break;
					}
				}
				LogCycleStopState("WT - after BH at safe");
				SetEjectorVacuum(FALSE);		// Release the vacuum after bh at safe
				Sleep(100);

				if( IsEjtElvtInUse() )	//	wait BH done picking and at house keeping
				{
					DisplaySequence("WTNG - down ejector elevator in house keeping");
					MoveES101EjtElvtZToUpDown(FALSE); // IN HOUSE KEEPING, BEFORE UNLOAD, DOWN
				}

				OpRotateBack();
				if( m_bIsWaferEnded )
				{
					DisplaySequence("WFT: ES wafer end, move table to unload");
					OpES101MoveToUnload();
					DisplaySequence("WTNG - try to load next table");
					OpWaferEndMoveToUnload();
					if( ( m_bDisableWT2InAutoBondMode==FALSE) && WaitES101PreloadFrameDone() )
					{
						OpES101NextToAlignPosition();	// auto mode, ready
						SetES101PreloadFrameDone(FALSE);	// pull next frame to home
					}	//	4.24TX 4
					CString szMsg;
					szMsg.Format("    WTNG - Align to finish time %.2f", (GetTime()-m_dStartWaferAlignTime));
					DisplaySequence(szMsg);
				}
			}

			(*m_psmfSRam)["WaferTable"]["AtSafePos"] = 1;
			m_qSubOperation = UPDATE_LAST_DIE_Q;

			CMSLogFileUtility::Instance()->MS_LogCycleState("stop sorting");	//	427TX	1
			DisplaySequence("WTNG - House Keeping ==> Update Last Die");
			break;

		case UPDATE_LAST_DIE_Q:
			LogCycleStopState("WT - Update Last Die");
			if ( m_bDieIsGet == TRUE )
			{
				OpUpdateLastDie_ES();

				WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
				ULONG ulRow, ulCol;
				UCHAR ucGrade;
				BOOL bEndOfWafer;
				PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer, "Update last");

				LONG lX = (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded X"];
				LONG lY = (LONG)(*m_psmfSRam)["BinTable"]["Die Bonded Y"];

				if ( (GetLastDieRow() != lY) || (GetLastDieCol() != lX) )
				{
					OpRollBack();
					WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
					ULONG ulRow, ulCol;
					UCHAR ucGrade;
					BOOL bEndOfWafer;
					PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer, "Update Last again");
					//Rollback once more if LastDIe not matched in DBH config
					if ((GetLastDieRow() != ulRow) || (GetLastDieCol() != ulCol))
						OpRollBack();
				}

				PeekMapNextDie(1, ulRow, ulCol, ucGrade, eAction, bEndOfWafer, "Update last last");
			}

			//reset wafer alignment status if any PR SKIP die error is triggered on WPR station
			LogCycleStopState("WT - Stop completely");
			State(STOPPING_Q);
			break;

		default:
			DisplaySequence("WTNG - Unknown");
			break;
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
}

INT CWaferTable::OpGetWaferPos_ES()
{
	unsigned long	ulX = 0, ulY = 0;
	unsigned char   ucGrade;
	BOOL			bEndOfWafer;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;

	PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "OpGetPos ES");

	if (bEndOfWafer == TRUE)
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("WAFER-END");
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End in WT");
		return Err_WTWaferEnd;
	}
	else
	{
		//Reset wafer end signal
		m_bIsWaferEnded	= FALSE;
	}

	(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= 0;

	(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"]	= 0;
	(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"]	= 0;

	if (eAction == WAF_CDieSelectionAlgorithm::CHECK)
	{
		(*m_psmfSRam)["DieInfo"]["RefDie"]["X"]		= ulX;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Y"]		= ulY;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 1;
	}
	
	LONG lRow, lCol, lX, lY;
	pUtl->GetAlignPosition(lRow, lCol, lX, lY);
	if ((ulX == lCol) && (ulY == lRow) && m_WaferMapWrapper.IsReferenceDie(ulY, ulX) )
	{
		CString szMsg;
		szMsg.Format("HomeDie (%d,%d) is ReferDie in get pos general", ulY, ulX);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		if (eAction == WAF_CDieSelectionAlgorithm::PICK)
		{
			eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		}
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"] = 1;
	}

	if (m_bJustAlign == TRUE)
	{
		m_ulNewPickCounter = 0;
	}

	if (eAction == WAF_CDieSelectionAlgorithm::ALIGN)
	{
		if (DieIsSpecialRefGrade(ulY, ulX))				//v3.24T1
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"] = 1;		//Tell WPR to align this PICK die with Ref-Die pattern
		}
	}

	m_stCurrentDie.lX = (LONG)ulX;
	m_stCurrentDie.lY = (LONG)ulY;
	m_stCurrentDie.ucGrade = ucGrade;
	m_stCurrentDie.eAction = eAction;

	// Used in waferPr and pass to IM Vision for logging
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"]		= ulY;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"]		= ulX;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"]	= ucGrade;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"]	= eAction;

	// Save data to bond record
	NextBondRecord();

	if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
	{
		(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;
	}
	else
	{
		(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;
	}

	return gnOK;
}

INT	CWaferTable::OpMoveTable_ES()
{
	CString szMoveMsg;
	LONG	lMoveWfX = 0, lMoveWfY = 0, lMoveWfT = 0;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	TakeTime(WT1);
	// prescan relative code
	BOOL bGetPrescan = FALSE;
	if( ( IsBurnIn() == FALSE) )
	{
		bGetPrescan = GetES101PrescanSortPosn(lMoveWfX, lMoveWfY, lMoveWfT);    //v4.28
	}			
	else
	{
		bGetPrescan = GetPrescanSortPosn(lMoveWfX, lMoveWfY, lMoveWfT);
	}

	if (m_bJustAlign)
	{
		m_bJustAlign = FALSE;
	}

	szMoveMsg.Format("Get JUMP %d,%d,T%d", lMoveWfX, lMoveWfY, lMoveWfT);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);

	m_lGetMveWftPosX = lMoveWfX;
	m_lGetMveWftPosY = lMoveWfY;

	// Check whether the found position is within wafer limit
	if ((IsWithinWaferLimit(lMoveWfX, lMoveWfY, 1, TRUE) == FALSE || 
		IsWithinMapDieLimit(lMoveWfX, lMoveWfY) == FALSE))
	{
		CString szTemp;
		szTemp.Format("WT: Invalid die position; X = %d, Y = %d", lMoveWfX, lMoveWfY);
		SetErrorMessage(szTemp);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);

		szTemp.Format("WT: Invalid die pos NAR: FALSE, wm(r%i,c%i), diff(r%i,c%i), enc(%d,%d)",
						GetCurrentRow(), GetCurrentCol(),	0, 0,	lMoveWfX, lMoveWfY);
		DisplayMessage(szTemp);

		szTemp.Format("    Last die info: (r%i,c%i), enc(%i,%i)", GetLastDieRow(), GetLastDieCol(),
						GetLastDieEncX(), GetLastDieEncY());
		DisplayMessage(szTemp);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);

		TakeTime(WT2);
		return gnNOTOK;
	}
					

	LONG lDistX = lMoveWfX - GetLastDieEncX();
	LONG lDistY = lMoveWfY - GetLastDieEncY();
	if( m_bFirst )
	{
		lDistX = lMoveWfX - GetCurrX();
		lDistY = lMoveWfY - GetCurrY();
	}
	LONG lDistT = lMoveWfT;

	OpCalculateMoveTableMotionTime(lDistX, lDistY, lDistT, TRUE);

	if (IsWT2InUse())
	{
		X2_MoveTo(lMoveWfX, SFM_NOWAIT);
		Y2_MoveTo(lMoveWfY, SFM_NOWAIT);
	}
	else
	{
		X_MoveTo(lMoveWfX, SFM_NOWAIT);
	
		BOOL bUseSCFProfileY = FALSE;
#ifdef NU_MOTION
		bUseSCFProfileY = m_stWTAxis_Y.m_bUseSCFProfile;
#endif
		if (bUseSCFProfileY == FALSE)
		{
			Y_MoveTo(lMoveWfY, SFM_NOWAIT);
		}
		else
		{	
			LONG lMovePosY = 0;
			GetCmdValue();
			lMovePosY = lMoveWfY - GetCurrCmdY();
			Y_ScfProfileMove(lMovePosY, SFM_NOWAIT);
		}
	}

	LONG lMapRow = GetCurrentRow();
	LONG lMapCol = GetCurrentCol();

	szMoveMsg.Format("Get MOVE %d,%d,T%d(%d) map %d,%d", lMoveWfX, lMoveWfY, lMoveWfT, GetCurrT(), lMapRow, lMapCol);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);

	if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
	{
		(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;
	}
	else
	{
		(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;
	}

	SetWTStartMove(TRUE);

	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lMoveWfX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lMoveWfY;

	X_Sync();
	Y_Sync();
	T_Sync();

	TakeTime(WT2);		// Take Time

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	DisplaySequence("WTNG - OpMove Table complete");

	return gnOK;
}

INT CWaferTable::OpNextDie_ES()
{
	GetEncoderValue();		
	m_stLastDie = m_stCurrentDie;

	m_stLastDie.lX_Enc = GetCurrX();
	m_stLastDie.lY_Enc = GetCurrY();
	m_stLastDie.lT_Enc = GetCurrT();

	(*m_psmfSRam)["WaferTable"]["Current"]["Theta"] = GetCurrT();

	// Get the die info again - Should be the same as before. Otherwise, return error
	BOOL bEndOfWafer = FALSE;
	unsigned char ucGrade = '0';
	unsigned long ulDiePos_X = 0, ulDiePos_Y = 0;
    WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	if (GetMapNextDie(ulDiePos_Y, ulDiePos_X, ucGrade, eAction, bEndOfWafer) == FALSE)
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("OP NEXT fail last restore to current into wafer end");
		return gnNOTOK;
	}

	if ((ulDiePos_X != (unsigned long)GetCurrentCol()) ||
		(ulDiePos_Y != (unsigned long)GetCurrentRow()) ||
		(ucGrade != GetCurrDieGrade())	||	(bEndOfWafer == TRUE))
	{
		//GetDie Log
		CString szLogText;
		szLogText.Format("MIS %d,%d,%d,%d,%d,%d", ulDiePos_Y, ulDiePos_X, ucGrade, GetCurrentRow(), GetCurrentCol(), GetCurrDieGrade());
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);
		m_stLastDie.lX = ulDiePos_X;
		m_stLastDie.lY = ulDiePos_Y;
	}

	if (m_bGoodDie == TRUE)
	{
		m_ucLastPickDieGrade = m_stLastDie.ucGrade;
		m_bGoodDieFound = TRUE;
	}

	return gnOK;
}


INT CWaferTable::OpUpdateLastDie_ES()
{
	UpdateMap(GetLastDieRow(), GetLastDieCol(), m_stLastDie.ucGrade, GetLastDieEAct(), m_stLastDie.ulStatus);

	return gnOK;
}
