/////////////////////////////////////////////////////////////////
// BH_Event.cpp : Event functions of the CBondHead class
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
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Event Signals
////////////////////////////////////////////
VOID CBondHead::SetAutoChangeColletDone(BOOL bState)
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

VOID CBondHead::SetEOT_Done(BOOL bState)
{
	SetBhTReadyForWT(bState);	//	for CP EOT raw data done
}

VOID CBondHead::SetProbeZReady(BOOL bState)
{
	SetEjectorReady(bState);
}

VOID CBondHead::SetEjectorReady(BOOL bState, CString szLog)
{
	CString szTemp;
	if (bState)
	{
		m_evEjectorReady.SetEvent();

		//v4.48A29
		//szTemp = "BH: EjRdy -> TRUE (" + szLog + ")";
		//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		szTemp = "BH - SetEjectorReady = TRUE (" + szLog + ")";
	}
	else
	{
		m_evEjectorReady.ResetEvent();
		
		//v4.48A29
		//szTemp = "BH: EjRdy -> FALSE (" + szLog + ")";
		//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		szTemp = "BH - SetEjectorReady = FALSE (" + szLog + ")";
	}
	
	if ( SPECIAL_DEBUG_LOG_BH )
	{
		DisplaySequence(szTemp);
	}
}

VOID CBondHead::SetBTReady(BOOL bState)
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

VOID CBondHead::SetPRLatched(BOOL bState)
{
	if (bState)
	{
		m_evPRLatched.SetEvent();
	}
	else
	{
		m_evPRLatched.ResetEvent();
	}
}

VOID CBondHead::SetPRStart(BOOL bState)
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

VOID CBondHead::SetBadDieForT(BOOL bState)
{
	if (bState)
	{
		m_evBadDieForT.SetEvent();
	}
	else
	{
		m_evBadDieForT.ResetEvent();
	}
}

VOID CBondHead::SetBhTReady(BOOL bState, CString szLog)
{
	CString szTemp;
	if (bState)
	{
		m_evBhTReady.SetEvent();
		szTemp = "Set BHT Ready - " + szLog;
		DisplaySequence(szTemp);	//v4.59A26
	}
	else
	{
		m_evBhTReady.ResetEvent();
		szTemp = "Reset BHT Ready - " + szLog;
		DisplaySequence(szTemp);	//v4.59A26
	}
}

VOID CBondHead::SetBhTReadyForBPR(BOOL bState)
{
	if (bState)
	{
		m_evBhTReadyForBPR.SetEvent();
	}
	else
	{
		m_evBhTReadyForBPR.ResetEvent();
	}
}

VOID CBondHead::SetBhTReadyForWPR(BOOL bState)
{
	if (bState)
	{
		m_evBhTReadyForWPR.SetEvent();
	}
	else
	{
		m_evBhTReadyForWPR.ResetEvent();
	}
}

VOID CBondHead::SetBhTReadyForWT(BOOL bState, CString szLog)
{
	CString szTemp;

	if (bState)
	{
		m_evBhTReadyForWT.SetEvent();

		szTemp = "Set BHT Ready for WT - " + szLog;
		DisplaySequence(szTemp);	//v4.59A26
	}
	else
	{
		m_evBhTReadyForWT.ResetEvent();

		szTemp = "Reset BHT Ready for WT FALSE - " + szLog;
		DisplaySequence(szTemp);	//v4.59A26
	}
}

VOID CBondHead::SetWTReady(BOOL bState, CString szTemp)
{
	if (bState)
	{
		m_evWTReady.SetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			//DisplaySequence(szTemp + ",BH TRUE(Set WT Ready)");
			DisplaySequence("BH - SetWTReady = TRUE (" + szTemp + ")");		//andrewng //2020-0617
		}
	}
	else
	{
		m_evWTReady.ResetEvent();
		if( SPECIAL_DEBUG_LOG_WT )
		{
			//DisplaySequence(szTemp + ",BH FALSE(Set WT Ready)");
			DisplaySequence("BH - SetWTReady = FALSE (" + szTemp + ")");	//andrewng //2020-0617
		}
	}
	
}

VOID CBondHead::SetWTStartMove(BOOL bState)		//DBH only
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

VOID CBondHead::SetCompDone(BOOL bState)		//DBH only
{
	if (bState)
	{
		m_evCompDone.SetEvent();
		DisplaySequence("BH - SetCompDone = TRUE");			//andrewng //2020-0617
	}
	else
	{
		m_evCompDone.ResetEvent();
		DisplaySequence("BH - SetCompDone = FALSE");		//andrewng //2020-0617
	}
}

VOID CBondHead::SetDiePicked(BOOL bState)
{
	if (bState)
	{
		m_evDiePicked.SetEvent();
	}
	else
	{
		m_evDiePicked.ResetEvent();
	}
}

BOOL CBondHead::WaitDieBonded(INT nTimeout)
{
	CSingleLock slLock(&m_evDieBonded);
	return slLock.Lock(nTimeout);
}

VOID CBondHead::SetDieBonded(BOOL bState)
{
	CString szTemp;

	if (bState)
	{
		m_evDieBonded.SetEvent();

		//v4.48A29	//v4.51A17
		//szTemp = "BH: SetDieBonded -> TRUE";
		//CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);
	}
	else
	{
		m_evDieBonded.ResetEvent();

		//v4.48A29	//v4.51A17
		//szTemp = "BH: SetDieBonded -> FALSE";
		//CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);
	}
}

VOID CBondHead::SetDieBondedForWT(BOOL bState)
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

VOID CBondHead::SetMoveBack(BOOL bState)
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

VOID CBondHead::SetWTBacked(BOOL bState)
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

VOID CBondHead::SetBHInit(BOOL bState)
{
	if (bState)
	{
		m_evBHInit.SetEvent();
	}
	else
	{
		m_evBHInit.ResetEvent();
	}
}

VOID CBondHead::SetConfirmSrch(BOOL bState)
{
	if (bState)
	{
		m_evConfirmSrch.SetEvent();
	}
	else
	{
		m_evConfirmSrch.ResetEvent();
	}
}

VOID CBondHead::SetBhToPrePick(BOOL bState)
{
	if (bState)
	{
		m_evBhToPrePick.SetEvent();
	}
	else
	{
		m_evBhToPrePick.ResetEvent();
	}
}

VOID CBondHead::SetBTStartMoveWithoutDelay(BOOL bState)
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

VOID CBondHead::SetBTStartMove(BOOL bState)
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

VOID CBondHead::SetBackupNVRam(BOOL bState)
{
	if (bState)
	{
		m_evBackupNVRam.SetEvent();
	}
	else
	{
		m_evBackupNVRam.ResetEvent();
	}
}

VOID CBondHead::SetBLFrameLevelDn(BOOL bState)
{
	if (bState)
	{
		m_evBLFrameLevelDn.SetEvent();
	}
	else
	{
		m_evBLFrameLevelDn.ResetEvent();
	}
}

VOID CBondHead::SetBTReSrchDie(BOOL bState)
{
	if (bState)
	{
		m_evBTReSrchDie.SetEvent();
	}
	else
	{
		m_evBTReSrchDie.ResetEvent();
	}
}

VOID CBondHead::SetBPRPostBondDone(BOOL bState)
{
	if (bState)
	{
		m_evBPRPostBondDone.SetEvent();
	}
	else
	{
		m_evBPRPostBondDone.ResetEvent();
	}
}

VOID CBondHead::SetBTCompensate(BOOL bState)
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

VOID CBondHead::SetSearchMarkReady(BOOL bState)
{
	if (bState)
	{
		m_evSearchMarkReady.SetEvent();
	}
	else
	{
		m_evSearchMarkReady.ResetEvent();
	}
}

VOID CBondHead::SetSearchMarkDone(BOOL bState)
{
	if (bState)
	{
		m_evSearchMarkDone.SetEvent();
	}
	else
	{
		m_evSearchMarkDone.ResetEvent();
	}
}

VOID CBondHead::SetBhReadyForWPRHwTrigger(BOOL bState)
{
	if (bState)
	{
		m_evBhReadyForWPRHwTrigger.SetEvent();
	}
	else
	{
		m_evBhReadyForWPRHwTrigger.ResetEvent();
	}
}

VOID CBondHead::SetBhReadyForBPRHwTrigger(BOOL bState)
{
	if (bState)
	{
		m_evBhReadyForBPRHwTrigger.SetEvent();
	}
	else
	{
		m_evBhReadyForBPRHwTrigger.ResetEvent();
	}
}

BOOL CBondHead::WaitPRStart(INT nTimeout)
{
	CSingleLock slLock(&m_evPRStart);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitPRLatched(INT nTimeout)
{
	CSingleLock slLock(&m_evPRLatched);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBadDieForT(INT nTimeout)
{
	CSingleLock slLock(&m_evBadDieForT);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBTReady(INT nTimeout)
{
	CSingleLock slLock(&m_evBTReady);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitWTReady(INT nTimeout)
{
	CSingleLock slLock(&m_evWTReady);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitCompDone(INT nTimeout)
{
	CSingleLock slLock(&m_evCompDone);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitWTStartMove(INT nTimeout)
{
	CSingleLock slLock(&m_evWTStartMove);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitDiePicked(INT nTimeout)
{
	CSingleLock slLock(&m_evDiePicked);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitMoveBack(INT nTimeout)
{
	BOOL bMoveBack = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["MoveBackForMissingDie"];
	return bMoveBack;
/*
	CSingleLock slLock(&m_evMoveBack);
	return slLock.Lock(nTimeout);
*/
}

BOOL CBondHead::WaitWTBacked(INT nTimeout)
{
	CSingleLock slLock(&m_evWTBacked);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitConfirmSrch(INT nTimeout)
{
	CSingleLock slLock(&m_evConfirmSrch);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBTStartMoveWithoutDelay(INT nTimeout)
{
	CSingleLock slLock(&m_evBTStartMoveWithoutDelay);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBTStartMove(INT nTimeout)
{
	CSingleLock slLock(&m_evBTStartMove);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitExpInit(INT nTimeout)
{
	CSingleLock slLock(&m_evExpInit);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBLFrameLevelDn(INT nTimeout)
{
	CSingleLock slLock(&m_evBLFrameLevelDn);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBTReSrchDie(INT nTimeout)
{
	CSingleLock slLock(&m_evBTReSrchDie);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBPRPostBondDone(INT nTimeout)
{
	CSingleLock slLock(&m_evBPRPostBondDone);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitBTCompensate(INT nTimeout)
{
	CSingleLock slLock(&m_evBTCompensate);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitSearchMarkReady(INT nTimeout)
{
	CSingleLock slLock(&m_evSearchMarkReady);
	return slLock.Lock(nTimeout);
}

BOOL CBondHead::WaitSearchMarkDone(INT nTimeout)
{
	CSingleLock slLock(&m_evSearchMarkDone);
	return slLock.Lock(nTimeout);
}

