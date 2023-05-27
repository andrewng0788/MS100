/////////////////////////////////////////////////////////////////
// WT_CycleState_MegaDa.cpp : Cycle Operation State of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tue, Nov 5, 2019
//	Revision:	1.00
//
//	By:			Matthew Law
//
//	Copyright @ ASM Assembly Automation Ltd., 2019.
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


VOID CWaferTable::RunOperation_MegaDa()
{
	CString szLog;
	LONG lMoveWfX, lMoveWfY;

	if (Command() == glSTOP_COMMAND && Action() == glABORT_STOP)
	{
		DisplaySequence("WT - STOP");
		State(STOPPING_Q);
		return;
	}

	try
	{
		m_nLastError	= gnOK;
		// Operation state
		switch (m_qSubOperation)
		{
			case WAIT_EJECTOR_READY_Q:
				DisplaySequence("WT - wait EJECTOR ready q");
				if (WaitEjectorReady())
				{
					SetEjectorReady(FALSE);
					m_qSubOperation = WAIT_PR_DELAY_MS60_Q;
				}
				break;

			case WAIT_PR_DELAY_MS60_Q:
				DisplaySequence("WT - wait PR delay ms60 q");
				LONG lDelay;
				Sleep(lDelay);

				SetWTStable(TRUE);				// Trigger Wafer PR

				m_qSubOperation = WAIT_GRAB_READY_MS60_Q;
				break;

			case WAIT_GRAB_READY_MS60_Q:
				DisplaySequence("WT - grab ready ms60");
				if (WaitPRLatchedForWT())
				{
					SetPRLatchedForWT(FALSE);
					CMSLogFileUtility::Instance()->MS60_Log("WT: WAIT_GRAB_READY_MS60_Q OK");
					m_qSubOperation = GET_WAFER_POS_Q;
				}
				break;

			case GET_WAFER_POS_Q:	// Get the die position from Wafer Map
				DisplaySequence("WT - get wafer pos");
				StartTime(GetRecordNumber());

				GetEncoderValue();
				szLog.Format("WT Start Move : EncXYT(%ld, %ld, %ld), LF-Offset(%d, %d)", m_lEnc_X, m_lEnc_Y, m_lEnc_T, m_stLFDie.lX, m_stLFDie.lY);
				CMSLogFileUtility::Instance()->MS60_Log(szLog);
	
				m_stLFDie.lX = (*m_psmfSRam)["WaferPr"]["LookForward"]["X"];
				m_stLFDie.lY = (*m_psmfSRam)["WaferPr"]["LookForward"]["Y"];
				lMoveWfX	 = m_stLastDie.lX_Enc + m_stLFDie.lX;
				lMoveWfY	 = m_stLastDie.lY_Enc + m_stLFDie.lY;

				//OpUpdateDie();				// Update the wafer map
				m_qSubOperation = MOVE_TABLE_Q;
				break;

			case MOVE_TABLE_Q:
				DisplaySequence("WT - move TABLE q");
				
				SetWTStartMove(TRUE);

				TakeTime(WT1);
				XY_MoveTo(lMoveWfX, lMoveWfY);
				TakeTime(WT2);

				GetEncoderValue();
				szLog.Format("WT End Move : EncXYT(%ld, %ld, %ld), LF-Offset(%d, %d)", m_lEnc_X, m_lEnc_Y, m_lEnc_T, m_stLFDie.lX, m_stLFDie.lY);
				CMSLogFileUtility::Instance()->MS60_Log(szLog);

				m_stLastDie.lX_Enc = GetCurrX();
				m_stLastDie.lY_Enc = GetCurrY();
				m_stLastDie.lT_Enc = GetCurrT();

				m_qSubOperation = WAIT_LF_READY_Q;
				break;

			case WAIT_LF_READY_Q:
				DisplaySequence("WT - wait LF Ready");
				if (WaitLFReady())
				{
					SetLFReady(FALSE);
					//Get LF Result
					m_qSubOperation = WT_NEXT_DIE_Q;
				}
				break;

			case WT_NEXT_DIE_Q:
				DisplaySequence("WT - next die");
				//Some operation before next cycle
				m_qSubOperation = WAIT_EJECTOR_READY_Q;
				break;

			case HOUSE_KEEPING_Q:
				DisplaySequence("WT - House Keeping Q");
				State(STOPPING_Q);
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
		SaveScanTimeEvent("    WFT: exception executed");
		SetMotionCE(TRUE, "WT RunOperation_MS60 Stop");
	}
}