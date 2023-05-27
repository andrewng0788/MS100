/////////////////////////////////////////////////////////////////
// BL_Event.cpp : Event functions of the CBinLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, June 21, 2005
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
#include "BinLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Event Signals
////////////////////////////////////////////

VOID CBinLoader::SetBLOperate(BOOL bState)
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

VOID CBinLoader::SetBLReady(BOOL bState, INT nLog)
{
	if (bState)
	{
		m_evBLReady.SetEvent();
	}
	else
	{
		m_evBLReady.ResetEvent();
	}

	//if (nLog > 0)	//v4.49A7
	{
		CString szMsg;
		szMsg.Format("BL SetBLReady %d - (%d)", bState, nLog); 
		BL_DEBUGBOX(szMsg);
	}
}

BOOL CBinLoader::WaitBLOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evBLOperate);
	return slLock.Lock(nTimeout);
}

VOID CBinLoader::SetBLAOperate(BOOL bState)
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

BOOL CBinLoader::WaitBLAOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evBLAOperate);
	return slLock.Lock(nTimeout);
}

VOID CBinLoader::SetBLPreOperate(BOOL bState)
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

BOOL CBinLoader::WaitBLPreOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evBLPreOperate);
	return slLock.Lock(nTimeout);
}


BOOL CBinLoader::WaitBLPreChangeGrade(INT nTimeout)
{
	CSingleLock slLock(&m_evBLPreChangeGrade);
	return slLock.Lock(nTimeout);
}

VOID CBinLoader::SetBLPreChangeGrade(BOOL bState)
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


BOOL CBinLoader::WaitBLFrameLevelDn(INT nTimeout)
{
	CSingleLock slLock(&m_evBLFrameLevelDn);
	return slLock.Lock(nTimeout);
}

VOID CBinLoader::SetBLFrameLevelDn(BOOL bState)
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



