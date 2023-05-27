/////////////////////////////////////////////////////////////////
// BPR_Event.cpp : Event functions of the CBondPr class
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
#include "BondPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Event Signals
////////////////////////////////////////////

VOID CBondPr::SetBPRLatched(BOOL bState)
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

VOID CBondPr::SetBhTReady(BOOL bState)
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

VOID CBondPr::SetBhTReadyForBPR(BOOL bState)
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

VOID CBondPr::SetBTStable(BOOL bState)
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

VOID CBondPr::SetBTReSrchDie(BOOL bState)
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

VOID CBondPr::SetBPRPostBondDone(BOOL bState)
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

VOID CBondPr::SetWPRGrabImageReady(BOOL bState, CString szMsg)
{
	if (bState)
	{
		m_evWPRGrabImageReady.SetEvent();
		if (!szMsg.IsEmpty())
		{
			DisplaySequence("BPR - SetWPRGrabImageReady(TRUE) " + szMsg);
		}
	}
	else
	{
		m_evWPRGrabImageReady.ResetEvent();
		if (!szMsg.IsEmpty())
		{
			DisplaySequence("BPR - SetWPRGrabImageReady(FALSE) " + szMsg);
		}
	}
}

VOID CBondPr::SetBhReadyForBPRHwTrigger(BOOL bState)
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

BOOL CBondPr::WaitBhTReady(INT nTimeout)
{
	CSingleLock slLock(&m_evBhTReady);
	return slLock.Lock(nTimeout);
}

BOOL CBondPr::WaitBhTReadyForBPR(INT nTimeout)
{
	CSingleLock slLock(&m_evBhTReadyForBPR);
	return slLock.Lock(nTimeout);
}

BOOL CBondPr::WaitBTStable(INT nTimeout)
{
	CSingleLock slLock(&m_evBTStable);
	return slLock.Lock(nTimeout);
}

BOOL CBondPr::WaitBTReSrchDie(INT nTimeout)
{
	CSingleLock slLock(&m_evBTReSrchDie);
	return slLock.Lock(nTimeout);
}

BOOL CBondPr::WaitWPRGrabImageReady(INT nTimeout)
{
	CSingleLock slLock(&m_evWPRGrabImageReady);
	return slLock.Lock(nTimeout);
}

BOOL CBondPr::WaitBhReadyForBPRHwTrigger(INT nTimeout)
{
	CSingleLock slLock(&m_evBhReadyForBPRHwTrigger);
	return slLock.Lock(nTimeout);
}

