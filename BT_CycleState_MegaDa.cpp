/////////////////////////////////////////////////////////////////
// BT_CycleState_MegaDa.cpp : Cycle Operation State of the CBinTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wed, Nov 6, 2019
//	Revision:	1.00
//
//	By:			Matthew Law
//					
//	Copyright @ ASM Assembly Automation Ltd., 2019.
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

VOID CBinTable::RunOperation_MegaDa()
{
	static BOOL s_bUpdateBinMap = TRUE;
	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP) )
	{
		DisplaySequence("BT - STOP");
		State(STOPPING_Q);
		return;
	}

	try
	{
		m_nLastError	= gnOK;
		// Operation state
		switch (m_qSubOperation)
		{
			case WAIT_DIE_READY_Q://For BPR but not use in MegaDa
				DisplaySequence("BT - WAIT_DIE_READY_Q");
				if (s_bUpdateBinMap)
				{
					s_bUpdateBinMap = FALSE;
					m_qSubOperation = INDEX_Q;
				}
				break;

			case INDEX_Q:
				DisplaySequence("BT - INDEX_Q");
				if (IsBondCountExceed())
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					LONG lCurrentBondCount = CBondingMap::GetBondCounter(); //Ascending
					m_lX = m_oBondingDie[lCurrentBondCount].m_lBondDieEnc_X;
					m_lY = m_oBondingDie[lCurrentBondCount].m_lBondDieEnc_Y;
					m_qSubOperation = MOVE_TABLE_Q;
				}
				break;

			case MOVE_TABLE_Q:
				DisplaySequence("BT - MOVE_TABLE_Q");
				XY_MoveTo(m_lX, m_lY, SFM_NOWAIT);
				m_qSubOperation = WAIT_BT_READY_Q;
				break;

			case WAIT_BT_READY_Q:
				DisplaySequence("BT - WAIT_BT_READY_Q");

				X_Sync();
				Y_Sync();

				SetBTReady(TRUE);//Ejector start to move

				m_qSubOperation = WAIT_DIE_BONDED_Q;
				break;

			case WAIT_DIE_BONDED_Q:
				DisplaySequence("BT - WAIT_DIE_BONDED_Q");

				if (WaitPRLatchedForBT())
				{	
					SetPRLatchedForBT(FALSE);
					m_qSubOperation = NEXT_DIE_Q;
				}
				break;

			case NEXT_DIE_Q:
				DisplaySequence("BT - NEXT_DIE_Q");
				
				m_oBondingDie[CBondingMap::GetBondCounter()].m_bIsDieBond = TRUE;
				CBondingMap::AddBondCounter();
				UpdateBinMap();
				s_bUpdateBinMap = TRUE;
				m_qSubOperation = WAIT_DIE_READY_Q;
				break;

			case HOUSE_KEEPING_Q:
				DisplaySequence("BT - HOUSE_KEEPING_Q");
				State(STOPPING_Q);
				break;
		}
	}
	catch (CAsmException e)
	{
		State(STOPPING_Q);
	}
}