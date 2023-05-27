#include "stdafx.h"
#include "MS896A_Constant.h"
#include "Safety.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CSafety::SetBackupNVRam(BOOL bState)
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

BOOL CSafety::WaitBackupNVRam(INT nTimeout)
{
	CSingleLock slLock(&m_evBackupNVRam);
	return slLock.Lock(nTimeout);
}