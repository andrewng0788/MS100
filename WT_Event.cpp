/////////////////////////////////////////////////////////////////
// WT_Event.cpp : Event functions of the CWaferTable class
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
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Event Signals
////////////////////////////////////////////
VOID CWaferTable::SetAutoChangeColletDone(BOOL bState)
{
	if (bState)
	{
		m_evAutoChangeColletDone.SetEvent();
	}
	else
	{
		m_evAutoChangeColletDone.ResetEvent();
	}
}

VOID CWaferTable::SetEOT_Done(BOOL bState)
{
	SetBhTReadyForWT(bState);	//	for CP EOT raw data done
}

BOOL CWaferTable::WaitEOT_Done(INT nTimeout)
{
	return WaitBhTReadyForWT(nTimeout);	//	for CP to check EOT raw data done
}

VOID CWaferTable::SetProbeZReady(BOOL bState)
{
	SetEjectorReady(bState);
}

VOID CWaferTable::SetEjectorReady(BOOL bState, CString szLog)
{
	CString szTemp;

	if (bState)
	{
		m_evEjectorReady.SetEvent();

		//v4.48A29
		//szTemp = "WT: EjRdy -> TRUE (" + szLog + ")";
		//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		//szTemp = "WT - Set EJ Ready " + szLog;
		if (SPECIAL_DEBUG_LOG_WT)
		{
			DisplaySequence("WT - SetEjectorReady = TRUE (" + szLog + ")");			//andrewng //2020-0609
		}
	}
	else
	{
		m_evEjectorReady.ResetEvent();
		
		//v4.48A29
		//szTemp = "WT: EjRdy -> FALSE (" + szLog + ")";
		//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		//szTemp = "WT - Reset EJ Ready " + szLog;
		if (SPECIAL_DEBUG_LOG_WT)
		{
			DisplaySequence("WT - SetEjectorReady = FALSE (" + szLog + ")");		//andrewng //2020-0609
		}
	}
}

VOID CWaferTable::SetBhTReadyForWT(BOOL bState)
{
	if (bState)
	{
		m_evBhTReadyForWT.SetEvent();
	}
	else
	{
		m_evBhTReadyForWT.ResetEvent();
	}
}

VOID CWaferTable::SetWTStable(BOOL bState)
{
	if (bState)
	{
		m_evWTStable.SetEvent();
	}
	else
	{
		m_evWTStable.ResetEvent();
	}
}	

VOID CWaferTable::SetWTReady(BOOL bState, CString szTemp)
{
	if (bState)
	{
		m_evWTReady.SetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			//DisplaySequence(szTemp + ",WT TRUE(Set WT Ready)");
			DisplaySequence("WT - SetWTReady = TRUE (" + szTemp + ")");			//andrewng //2020-0609
		}
	}
	else
	{
		m_evWTReady.ResetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			//DisplaySequence(szTemp + ",WT FALSE(Set WT Ready)");
			DisplaySequence("WT - SetWTReady = FALSE (" + szTemp + ")");		//andrewng //2020-0609
		}
	}
}	

VOID CWaferTable::SetWTStartMove(BOOL bState)	//v3.66
{
	if (bState)
	{
		m_evWTStartMove.SetEvent();
	}
	else
	{
		m_evWTStartMove.ResetEvent();
	}
}

VOID CWaferTable::SetWTBacked(BOOL bState)
{
	if (bState)
	{
		m_evWTBacked.SetEvent();
	}
	else
	{
		m_evWTBacked.ResetEvent();
	}
}	

VOID CWaferTable::SetDieReady(BOOL bState)
{
	if (bState)
	{
		m_evDieReady.SetEvent();
	}
	else
	{
		m_evDieReady.ResetEvent();
	}
}	

VOID CWaferTable::SetCompDone(BOOL bState, CString szLog)
{
	if (bState)
	{
		m_evCompDone.SetEvent();
		if (SPECIAL_DEBUG_LOG_WT)
		{
			if (!szLog.IsEmpty())
			{
				//DisplaySequence(szLog + ",WT TRUE(Set Comp Done)");
				DisplaySequence("WT: SetCompDone = TRUE (" + szLog + ")");	//andrewng //2020-0617
			}
		}
	}
	else
	{
		m_evCompDone.ResetEvent();
		if (SPECIAL_DEBUG_LOG_WT)
		{
			if (!szLog.IsEmpty())
			{
				//DisplaySequence(szLog + ",WT FALSE(Set Comp Done)");
				DisplaySequence("WT: SetCompDone = FALSE (" + szLog + ")");	//andrewng //2020-0617
			}
		}
	}
}	

VOID CWaferTable::SetDieReadyForBT(BOOL bState)
{
	if (bState)
	{
		m_evDieReadyForBT.SetEvent();
	}
	else
	{
		m_evDieReadyForBT.ResetEvent();
	}

	if( SPECIAL_DEBUG_LOG_WT )
	{
		CString szMsg;
		szMsg.Format("WT - set die ready for bt %d", bState);
		DisplaySequence(szMsg);
	}
}	

VOID CWaferTable::SetDieInfoRead(BOOL bState)
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

	if( SPECIAL_DEBUG_LOG_WT )
	{
		CString szMsg;
		szMsg.Format("WT - set die info read %d", bState);
		DisplaySequence(szMsg);
	}
}	

VOID CWaferTable::SetBadDie(BOOL bState)
{
	if (bState)
	{
		m_evBadDie.SetEvent();
	}
	else
	{
		m_evBadDie.ResetEvent();
	}
}	

VOID CWaferTable::SetMoveBack(BOOL bState)
{
	(*m_psmfSRam)["BondHead"]["MoveBackForMissingDie"] = bState;
/*
	if (bState)
	{
		m_evMoveBack.SetEvent();
	}
	else
	{
		m_evMoveBack.ResetEvent();
	}
*/
}

VOID CWaferTable::SetDieBondedForWT(BOOL bState)
{
	if (bState)
	{
		m_evDieBondedForWT.SetEvent();
	}
	else
	{
		m_evDieBondedForWT.ResetEvent();
	}
}	

VOID CWaferTable::SetLFReady(BOOL bState)
{
	if (bState)
	{
		m_evLFReady.SetEvent();
	}
	else
	{
		m_evLFReady.ResetEvent();
	}
}

VOID CWaferTable::SetPRStart(BOOL bState)
{
	if (bState)
	{
		m_evPRStart.SetEvent();
	}
	else
	{
		m_evPRStart.ResetEvent();
	}
}

VOID CWaferTable::SetBPRLatched(BOOL bState)
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

VOID CWaferTable::SetWTinitT(BOOL bState)
{
	if (bState)
	{
		m_evWTInitT.SetEvent();
	}
	else
	{
		m_evWTInitT.ResetEvent();
	}
}

VOID CWaferTable::SetPreCompensate(BOOL bState)
{
	if (bState)
	{
		m_evPreCompensate.SetEvent();
	}
	else
	{
		m_evPreCompensate.ResetEvent();
	}
}

VOID CWaferTable::SetPRTesting(BOOL bState)
{
	if (bState)
	{
		m_evSetPRTesting.SetEvent();
	}
	else
	{
		m_evSetPRTesting.ResetEvent();
	}
}

VOID CWaferTable::SetWTReadyForWPREmptyCheck(BOOL bState)
{
	if (bState)
	{
		m_evWTReadyForWPREmptyCheck.SetEvent();
	}
	else
	{
		m_evWTReadyForWPREmptyCheck.ResetEvent();
	}
}

VOID CWaferTable::SetWPREmptyCheckDone(BOOL bState)
{
	if (bState)
	{
		m_evWPREmptyCheckDone.SetEvent();
	}
	else
	{
		m_evWPREmptyCheckDone.ResetEvent();
	}
}

VOID CWaferTable::SetWTStartToMoveForBT(BOOL bState, CString szTemp)
{
	if (bState)
	{
		m_evWTStartToMoveForBT.SetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			DisplaySequence(szTemp + ",WT TRUE(Set WT Start To Move For BT)");
		}
	}
	else
	{
		m_evWTStartToMoveForBT.ResetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			DisplaySequence(szTemp + ",WT FALSE(Set WT Start To Move For BT)");
		}
	}
}

BOOL CWaferTable::WaitAutoChangeColletDone(INT nTimeout)
{
	CSingleLock slLock(&m_evAutoChangeColletDone);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitProbeZReady(INT nTimeout)
{
	return WaitEjectorReady(nTimeout);
}

BOOL CWaferTable::WaitEjectorReady(INT nTimeout)
{
	CSingleLock slLock(&m_evEjectorReady);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitBhTReadyForWT(INT nTimeout)
{
	CSingleLock slLock(&m_evBhTReadyForWT);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitDieReady(INT nTimeout)
{
	CSingleLock slLock(&m_evDieReady);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitBadDie(INT nTimeout)
{
	CSingleLock slLock(&m_evBadDie);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitDiePicked(INT nTimeout)
{
	CSingleLock slLock(&m_evDiePicked);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitMoveBack(INT nTimeout)
{
	BOOL bMoveBack = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["MoveBackForMissingDie"];
	return bMoveBack;
//	CSingleLock slLock(&m_evMoveBack);
//	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitDieBondedForWT(INT nTimeout)
{
	CSingleLock slLock(&m_evDieBondedForWT);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitDieUnReadyForBT(INT nTimeout)
{
/*
	if( SPECIAL_DEBUG )
	{
		CSingleLock slLock(&m_evDieReadyForBT);
		return !(slLock.Lock(nTimeout));
	}
*/
	return TRUE;
}	//	V450X16

BOOL CWaferTable::WaitDieInfoRead(INT nTimeout)
{
	BOOL bReady = (BOOL)(LONG)(*m_psmfSRam)["WaferAndBin"]["BT Die Info Read For WT"];
	CSingleLock slLock(&m_evDieInfoRead);
	return bReady && slLock.Lock(nTimeout);	//	V450X16
}

BOOL CWaferTable::WaitBHInit(INT nTimeout)
{
	CSingleLock slLock(&m_evBHInit);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitLFReady(INT nTimeout)
{
	CSingleLock slLock(&m_evLFReady);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitExpInitForWt(INT nTimeout)
{
	CSingleLock slLock(&m_evExpInitForWt);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitPreCompensate(INT nTimeout)
{
	CSingleLock slLock(&m_evPreCompensate);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitWprInitAFZ(INT nTimeout)
{
	if( IsWprWithAF() )
	{
		CSingleLock slLock(&m_evInitAFZ);
		return slLock.Lock(nTimeout);
	}
	return TRUE;
}

BOOL CWaferTable::WaitWTReadyForWPREmptyCheck(INT nTimeout)
{
	CSingleLock slLock(&m_evWTReadyForWPREmptyCheck);
	return slLock.Lock(nTimeout);
}

BOOL CWaferTable::WaitWPREmptyCheckDone(INT nTimeout)
{
	CSingleLock slLock(&m_evWPREmptyCheckDone);
	return slLock.Lock(nTimeout);
}


