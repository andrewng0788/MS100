/////////////////////////////////////////////////////////////////
// WPR_Event.cpp : Event functions of the CWaferPr class
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
#include "WaferPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Event Signals
////////////////////////////////////////////

VOID CWaferPr::SetWTStable(BOOL bState)
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

VOID CWaferPr::SetPRLatched(BOOL bState)
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

VOID CWaferPr::SetPRStart(BOOL bState)
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

VOID CWaferPr::SetDieReady(BOOL bState)
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


VOID CWaferPr::SetBadDie(BOOL bState, CString szLog)
{
	CString szTemp;

	if (bState)
	{
		m_evBadDie.SetEvent();

		//v4.49A3
		szTemp = "WPR: SetBadDie -> TRUE (" + szLog + ")";
		CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
	}
	else
	{
		m_evBadDie.ResetEvent();

		//v4.49A3
		//szTemp = "WPR: SetBadDie -> FALSE (" + szLog + ")";
		//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
	}
}	

VOID CWaferPr::SetBadDieForT(BOOL bState)
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

VOID CWaferPr::SetLFReady(BOOL bState)
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

VOID CWaferPr::SetConfirmSrch(BOOL bState)
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

VOID CWaferPr::SetBhToPrePick(BOOL bState)
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

VOID CWaferPr::SetPreCompensate(BOOL bState)
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

BOOL CWaferPr::WaitWTStable(INT nTimeout)
{
	CSingleLock slLock(&m_evWTStable);
	return slLock.Lock(nTimeout);
}

BOOL CWaferPr::WaitBhToPrePick(INT nTimeout)
{
	CSingleLock slLock(&m_evBhToPrePick);
	return slLock.Lock(nTimeout);
}

VOID CWaferPr::SetBhTReadyForWPR(BOOL bState)
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

VOID CWaferPr::SetSearchMarkReady(BOOL bState)
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

VOID CWaferPr::SetSearchMarkDone(BOOL bState)
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

VOID CWaferPr::SetPRTesting(BOOL bState)
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

BOOL CWaferPr::WaitBhTReadyForWPR(INT nTimeout)
{
	CSingleLock slLock(&m_evBhTReadyForWPR);
	return slLock.Lock(nTimeout);
}

BOOL CWaferPr::WaitSetPRTesting(INT nTimeout)
{
	CSingleLock slLock(&m_evSetPRTesting);
	return slLock.Lock(nTimeout);
}

VOID CWaferPr::SetWTReadyForWPREmptyCheck(BOOL bState)
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

BOOL CWaferPr::WaitWTReadyForWPREmptyCheck(INT nTimeout)
{
	CSingleLock slLock(&m_evWTReadyForWPREmptyCheck);
	return slLock.Lock(nTimeout);
}

VOID CWaferPr::SetWPREmptyCheckDone(BOOL bState)
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

BOOL CWaferPr::WaitWPREmptyCheckDone(INT nTimeout)
{
	CSingleLock slLock(&m_evWPREmptyCheckDone);
	return slLock.Lock(nTimeout);
}

VOID CWaferPr::SetInitAFZState(BOOL bState)
{
	if( bState )
	{
		m_evInitAFZ.SetEvent();
	}
	else
	{
		m_evInitAFZ.ResetEvent();
	}
}

BOOL CWaferPr::WaitBhReadyForWPRHwTrigger(INT nTimeout)
{
	CSingleLock slLock(&m_evBhReadyForWPRHwTrigger);
	return slLock.Lock(nTimeout);
}

VOID CWaferPr::SetBhReadyForWPRHwTrigger(BOOL bState)
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

VOID CWaferPr::SetWPRGrabImageReady(BOOL bState, CString szMsg)
{
	if (bState)
	{
		m_evWPRGrabImageReady.SetEvent();
		if (!szMsg.IsEmpty())
		{
			DisplaySequence("WPR - SetWPRGrabImageReady(TRUE) " + szMsg);
		}
	}
	else
	{
		m_evWPRGrabImageReady.ResetEvent();
		if (!szMsg.IsEmpty())
		{
			DisplaySequence("WPR - SetWPRGrabImageReady(FALSE) " + szMsg);
		}
	}
}