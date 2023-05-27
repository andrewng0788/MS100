/////////////////////////////////////////////////////////////////
// BT_Event.cpp : Event functions of the CBinTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
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
#include "MS896A_Constant.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Event Signals
////////////////////////////////////////////

VOID CBinTable::SetBTReady(BOOL bState)
{
	if (bState)
	{
		m_evBTReady.SetEvent();
	}
	else
	{
		m_evBTReady.ResetEvent();
	}
}

VOID CBinTable::SetBTStable(BOOL bState)
{
	if (bState)
	{
		m_evBTStable.SetEvent();
	}
	else
	{
		m_evBTStable.ResetEvent();
	}
}

VOID CBinTable::SetDieReadyForBT(BOOL bState)
{
	if (bState)
	{
		m_evDieReadyForBT.SetEvent();
	}
	else
	{
		m_evDieReadyForBT.ResetEvent();
	}

	if ( 1 )	//SPECIAL_DEBUG_LOG_BT )
	{
		CString szMsg;
		szMsg.Format("BT - SetDieReadyForBT = %d", bState);
		DisplaySequence(szMsg);
	}
}

VOID CBinTable::SetBPRLatched(BOOL bState)
{
	if (bState)
	{
		m_evBPRLatched.SetEvent();
	}
	else
	{
		m_evBPRLatched.ResetEvent();
	}
}


VOID CBinTable::SetDieBonded(BOOL bState)
{
	if (bState)
	{
		m_evDieBonded.SetEvent();
	}
	else
	{
		m_evDieBonded.ResetEvent();
	}
}

VOID CBinTable::SetDieInfoRead(BOOL bState)
{
	(*m_psmfSRam)["WaferAndBin"]["BT Die Info Read For WT"] = bState;
	if (bState)
	{
		m_evDieInfoRead.SetEvent();
	}
	else
	{
		m_evDieInfoRead.ResetEvent();
	}

	if ( 1) //SPECIAL_DEBUG_LOG_BT )
	{
		CString szMsg;
		szMsg.Format("BT - SetDieInfoRead = %d", bState);
		DisplaySequence(szMsg);
	}
}

VOID CBinTable::SetBLOperate(BOOL bState)
{
	if (bState)
	{
		m_evBLOperate.SetEvent();
	}
	else
	{
		m_evBLOperate.ResetEvent();
	}
}

VOID CBinTable::SetBLReady(BOOL bState)
{
	if (bState)
	{
		m_evBLReady.SetEvent();
	}
	else
	{
		m_evBLReady.ResetEvent();
	}
}

VOID CBinTable::SetBLAOperate(BOOL bState)
{
	if (bState)
	{
		m_evBLAOperate.SetEvent();
	}
	else
	{
		m_evBLAOperate.ResetEvent();
	}
}

VOID CBinTable::SetBLPreOperate(BOOL bState)
{
	if (bState)
	{
		m_evBLPreOperate.SetEvent();
	}
	else
	{
		m_evBLPreOperate.ResetEvent();
	}
}


VOID CBinTable::SetBhTReady(BOOL bState)
{
	if (bState)
	{
		  m_evBhTReady.SetEvent();
	}
	else
	{
		m_evBhTReady.ResetEvent();
	}
}

VOID CBinTable::SetBTStartMoveWithoutDelay(BOOL bState)
{
	if (bState)
	{
		m_evBTStartMoveWithoutDelay.SetEvent();
	}
	else
	{
		m_evBTStartMoveWithoutDelay.ResetEvent();
	}
}

VOID CBinTable::SetBTStartMove(BOOL bState)
{
	if (bState)
	{
		m_evBTStartMove.SetEvent();
	}
	else
	{
		m_evBTStartMove.ResetEvent();
	}
}

VOID CBinTable::SetBLPreChangeGrade(BOOL bState)
{
	if (bState)
	{
		m_evBLPreChangeGrade.SetEvent();
	}
	else
	{
		m_evBLPreChangeGrade.ResetEvent();
	}
}

VOID CBinTable::SetBTCompensate(BOOL bState)
{
	if (bState)
	{
		m_evBTCompensate.SetEvent();
	}
	else
	{
		m_evBTCompensate.ResetEvent();
	}
}

VOID CBinTable::SetWTStartToMoveForBT(BOOL bState, CString szTemp)
{
	if (bState)
	{
		m_evWTStartToMoveForBT.SetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			DisplaySequence(szTemp + ",BT TRUE(Set WT Start To Move For BT)");
		}
	}
	else
	{
		m_evWTStartToMoveForBT.ResetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			DisplaySequence(szTemp + ",BT FALSE(Set WT Start To Move For BT)");
		}
	}
}

VOID CBinTable::SetNLOperate(BOOL bState)
{
	if (bState)
	{
		m_evNLOperate.SetEvent();
	}
	else
	{
		m_evNLOperate.ResetEvent();
	}
}

VOID CBinTable::SetNLReady(BOOL bState)
{
	if (bState)
	{
		m_evNLReady.SetEvent();
	}
	else
	{
		m_evNLReady.ResetEvent();
	}
}

VOID CBinTable::SetNLPreOperate(BOOL bState)
{
	if (bState)
	{
		m_evNLPreOperate.SetEvent();
	}
	else
	{
		m_evNLPreOperate.ResetEvent();
	}
}


BOOL CBinTable::WaitDieInfoUnRead(INT nTimeout)
{
	if( SPECIAL_DEBUG )
	{
		BOOL bReady = (BOOL)(LONG)(*m_psmfSRam)["WaferAndBin"]["BT Die Info Read For WT"];
		CSingleLock slLock(&m_evDieInfoRead);
		return !(bReady && slLock.Lock(nTimeout));	//	V450X16
	}
	return TRUE;
}	//	V450X16

UCHAR CBinTable::GetWTGrade()
{
	return m_ucWTGrade;	// real grade. 1,2,3
}	//	V450X16

ULONG CBinTable::GetBTBlock()	//v4.52A6	//Changed return type from UCHAR to ULONG
{
	return m_ulBondedBlk;
}

BOOL CBinTable::WaitDieReadyForBT(INT nTimeout)
{
	CSingleLock slLock(&m_evDieReadyForBT);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitBPRLatched(INT nTimeout)
{
	CSingleLock slLock(&m_evBPRLatched);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitDieBonded(INT nTimeout)
{
	CSingleLock slLock(&m_evDieBonded);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitBHInit(INT nTimeout)
{
	CSingleLock slLock(&m_evBHInit);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitBLReady(INT nTimeout)
{
	CSingleLock slLock(&m_evBLReady);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitBLAOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evBLAOperate);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitBLPreOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evBLPreOperate);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitBLPreChangeGrade(INT nTimeout)
{
	CSingleLock slLock(&m_evBLPreChangeGrade);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitBTCompensate(INT nTimeout)
{
	CSingleLock slLock(&m_evBTCompensate);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitWTStartToMoveForBT(INT nTimeout)
{
	CSingleLock slLock(&m_evWTStartToMoveForBT);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitNLReady(INT nTimeout)
{
	CSingleLock slLock(&m_evNLReady);
	return slLock.Lock(nTimeout);
}

BOOL CBinTable::WaitNLPreOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evNLPreOperate);
	return slLock.Lock(nTimeout);
}


