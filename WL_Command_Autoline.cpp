///////////////////////////////////////////////////////////////////////
// WL_Command.cpp : HMI Registered Command of the CWaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Barry Chu	
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
///////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"
#include "SC_Constant.h"
#include "CTmpChange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferLoader::SECS_SetSecsGemHostCommandErr(const LONG lRet)
{
	//if SECS/GEM time-out, machine does not display error message and stop machine
	if ((lRet == HCACK_LOADER_BACK_GATE_NOT_OPENED) || (lRet == HCACK_LOADER_BACK_GATE_NOT_CLOSED) ||
		(lRet == HCACK_LOADER_BACK_GATE_OTHER_ERR))
	{
		m_lSecsGemHostCommandErr = lRet;
	}
}


LONG CWaferLoader::SECS_LoadFrameCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lSlotID;
		LONG	lSlotGrade;	
	} MSAUTOLINESETUP;

	MSAUTOLINESETUP stInfo;
	stInfo.lSlotID = 0;
	stInfo.lSlotGrade = 0;
	svMsg.GetMsg(sizeof(MSAUTOLINESETUP),	&stInfo);	//v4.56A11

	CString szLog;
	szLog.Format("WL: SECS_LoadFrameCmd - SlotID=%ld, STATE=%d", stInfo.lSlotID, State());
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	CSingleLock lock(&m_csLoadUnloadMutex);
	lock.Lock();

	LONG lRet = 0;
	if (!WaitForLoadUnloadObject())
	{
		szLog.Format("WaitForLoadUnloadObject  SECS_LoadFrameCmd  = %ld", m_lLoadUnloadObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		lRet = HCACK_LOADER_RESPONSE_TIME_OUT;
	}

	//Check Barcode ID before Load
	if (!lRet && !CheckHostLoadSlotUsageStatus())
	{
		lRet = HCACK_LOADER_PARA_CPVAL_NOT_DESIRED;
	}

	if (!lRet)
	{
		LoadUnloadSECSCmdLock();
		lRet = OpMoveLoaderZToUnload_AutoLine(TRUE, stInfo.lSlotID);
		if (lRet)
		{
			CloseALBackGate(FALSE);
			LoadUnloadSECSCmdUnlock();
		}
		m_bAutoLineLoadDone			= FALSE;
	}

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("1");
	SECS_UpdateLoadUnloadStatus(stInfo.lSlotID, TRUE, lRet);

	SECS_SetSecsGemHostCommandErr(lRet);

	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}

LONG CWaferLoader::SECS_LoadFrameDoneCmd(IPC_CServiceMessage& svMsg)
{
	LONG lStatus = 0;
	svMsg.GetMsg(sizeof(LONG),	&lStatus);

	CString szBarCode = (*m_psmfSRam)["MS"]["AutoLineWL"]["FrameBarCode"];
	CString szLog;
	szLog.Format("WL: SECS_LoadFrameDoneCmd - Status=%ld, SlotID=%ld, BC=%s", 
		lStatus, m_lAutoLineUnloadSlotID, szBarCode);	//v4.57A15
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	LONG lRet = 0;
	if (!CloseALBackGate(FALSE))
	{
		lRet = HCACK_LOADER_BACK_GATE_NOT_CLOSED; //Not Close Door
	}

	if (lStatus != 0)	//0=FAIL, 1=SUCCESS
	{
		if ((m_lAutoLineUnloadSlotID > 0) && (m_lAutoLineUnloadSlotID <= WL_MAX_MGZN_SLOT))
		{
			UpdateBulkLoadSlots();		//v4.59A11
			GenerateOMRTTableFile(0);
		}

		SaveData();		//Update m_stWaferMagazine[] stored in MSD
		for (LONG j = 0; j < MS_WL_AUTO_LINE_MGZN_SLOT; j++)
		{
			SECS_UpdateCassetteSlotInfo(j + 1);	//Update SV SECSGEM variables
		}
	}

	ResetBulkLoadData();

	m_lAutoLineUnloadSlotID	= 0;
	m_bAutoLineLoadDone		= TRUE;

	szLog.Format("SECS_LoadFrameDoneCmd: m_bAutoLineLoadDone = %ld", m_bAutoLineLoadDone);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	LoadUnloadSECSCmdUnlock();

	SECS_SetSecsGemHostCommandErr(lRet);
	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}

LONG CWaferLoader::SECS_UnloadFrameCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lSlotID;
		LONG	lSlotGrade;	
	} MSAUTOLINESETUP;

	MSAUTOLINESETUP stInfo;
	stInfo.lSlotID = 0;
	stInfo.lSlotGrade = 0;
	svMsg.GetMsg(sizeof(MSAUTOLINESETUP),	&stInfo);	//v4.56A11

	CString szLog;
	szLog.Format("WL: SECS_UnloadFrameCmd - SlotID=%ld, STATE=%d", stInfo.lSlotID, State());
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	CSingleLock lock(&m_csLoadUnloadMutex);
	lock.Lock();

	LONG lRet = 0;
	if (!WaitForLoadUnloadObject())
	{
		szLog.Format("WaitForLoadUnloadObject  SECS_UnloadFrameCmd  = %ld", m_lLoadUnloadObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		lRet = HCACK_LOADER_RESPONSE_TIME_OUT;
	}

	//Check SlotID&Barcode ID before Unload
	if (!lRet && !CheckHostUnloadSlotIDBarcode())
	{
		lRet = HCACK_PARA_INVALID;
	}

	if (!lRet)
	{
		LoadUnloadSECSCmdLock();
		lRet = OpMoveLoaderZToUnload_AutoLine(FALSE, stInfo.lSlotID);
		if (lRet)
		{
			CloseALBackGate(FALSE);
			LoadUnloadSECSCmdUnlock();
		}

		m_bAutoLineUnloadDone = FALSE;
	}

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("2");
	SECS_UpdateLoadUnloadStatus(stInfo.lSlotID, FALSE, lRet);

	SECS_SetSecsGemHostCommandErr(lRet);

	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}

LONG CWaferLoader::SECS_UnloadFrameDoneCmd(IPC_CServiceMessage& svMsg)
{
	LONG lStatus = 0;
	svMsg.GetMsg(sizeof(LONG),	&lStatus);

	CString szLog;
	szLog.Format("WL: SECS_UnloadFrameDoneCmd - Status=%ld", lStatus);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	LONG lRet = 0;
	if (!CloseALBackGate(FALSE))
	{
		lRet = HCACK_LOADER_BACK_GATE_NOT_CLOSED; //Not Close Door
	}

	if (lStatus != 0)	//0=FAIL, 1=SUCCESS
	{
		if ((m_lAutoLineUnloadSlotID > 0) && (m_lAutoLineUnloadSlotID <= WL_MAX_MGZN_SLOT))
		{
			UpdateBulkLoadSlots(TRUE);
			GenerateOMRTTableFile(0);
		}

		SaveData();												//Update m_stWaferMagazine[] in MSD
		for (LONG j = 0; j < MS_WL_AUTO_LINE_MGZN_SLOT; j++)
		{
			SECS_UpdateCassetteSlotInfo(j + 1);	//Update SV SECSGEM variables
		}
	}

	ResetBulkLoadData();
	m_lAutoLineUnloadSlotID		= 0;
	m_bAutoLineUnloadDone		= TRUE;

	LoadUnloadSECSCmdUnlock();

	SECS_SetSecsGemHostCommandErr(lRet);

	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}

BOOL CWaferLoader::IsCloseALBackGate()
{
	return (IsFrontGateSensorOn() && !IsBackGateSensorOn());
}

BOOL CWaferLoader::CloseALFrontGateWithLock()
{
	if (!LoadUnloadLockMutex())
	{
		CString szLog;

		szLog.Format("LoadUnloadLockMutex  CloseALFrontGateWithLock  = %ld", m_lLoadUnloadSECSCmdObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	BOOL bRet = CloseALFrontGate();

	LoadUnloadUnlock();

	return bRet;
}


BOOL CWaferLoader::OpenALFrontGateWithLock()
{
	if (!LoadUnloadLockMutex())
	{
		CString szLog;

		szLog.Format("LoadUnloadLockMutex  OpenALFrontGateWithLock  = %ld", m_lLoadUnloadSECSCmdObject);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	BOOL bRet = OpenALFrontGate();

	LoadUnloadUnlock();

	return bRet;
}


BOOL CWaferLoader::CloseALFrontGate()
{
	if (!IsMSAutoLineMode())
	{
		return TRUE;
	}

	Sleep(500);
	SetALFrontGate(TRUE);	//Close FRONT gate

	LONG lTimeOut = 2000;	// Timeout 2000ms
	LONG lCurrentTime = GetCurTime();
	while (GetCurTime() < lCurrentTime + lTimeOut)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (!IsFrontGateSensorOn())
		{
			break;
		}
		Sleep(100);
	}

	Sleep(500);
	if (IsFrontGateSensorOn())
	{
		CString szMsg;
		szMsg = _T("Wafer Loader Front Gate Not Closed");
		SetAlert_Msg_Red_Yellow(IDS_WL_FRONT_GATE_NOT_CLOSED, szMsg);		
		SetErrorMessage(szMsg);
		return FALSE; //Not Open
	}

	return TRUE;
}


BOOL CWaferLoader::OpenALFrontGate()
{
	if (!IsMSAutoLineMode())
	{
		return TRUE;
	}

	SetALFrontGate(FALSE);	//Open FRONT gate for MS

	LONG lTimeOut = 2000;	// Timeout 2000ms
	LONG lCurrentTime = GetCurTime();
	while (GetCurTime() < lCurrentTime + lTimeOut)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (IsFrontGateSensorOn())
		{
			break;
		}
		Sleep(100);
	}

	Sleep(500);
	if (!IsFrontGateSensorOn())
	{
		CString szMsg;
		szMsg = _T("Wafer Loader Front Gate Not Opened");
		SetAlert_Msg_Red_Yellow(IDS_WL_FRONT_GATE_NOT_OPENED, szMsg);		
		SetErrorMessage(szMsg);
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferLoader::CloseALBackGate(BOOL bOpenFrontGate)
{
	SetALBackGate(TRUE);	//Close BACK Gate when RGV done

	LONG lTimeOut = 2000;	// Timeout 2000ms
	LONG lCurrentTime = GetCurTime();
	while (GetCurTime() < lCurrentTime + lTimeOut)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (!IsBackGateSensorOn())
		{
			break;
		}
		Sleep(100);
	}

	if (bOpenFrontGate)
	{
		Sleep(500);
		SetALFrontGate(FALSE);	//Open FRONT gate for MS

		lTimeOut = 2000;	// Timeout 2000ms
		lCurrentTime = GetCurTime();
		while (GetCurTime() < lCurrentTime + lTimeOut)
		{
			m_comServer.ProcessRequest();

			MSG Msg; 
			if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
				::DispatchMessage(&Msg);
			}

			if (IsFrontGateSensorOn() && !IsBackGateSensorOn())
			{
				break;
			}
			Sleep(100);
		}

		Sleep(200); //waiting
		if (!IsFrontGateSensorOn() || IsBackGateSensorOn())
		{
			return FALSE;
		}
	}
	else
	{
		Sleep(200); //waiting
		if (IsBackGateSensorOn())
		{
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CWaferLoader::OpenALBackGate()
{
	SetALFrontGate(TRUE);	//Close FRONT gate

	LONG lTimeOut = 2000;	// Timeout 2000ms
	LONG lCurrentTime = GetCurTime();
	while (GetCurTime() < lCurrentTime + lTimeOut)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (!IsFrontGateSensorOn())
		{
			break;
		}
		Sleep(100);
	}

	Sleep(500);
	SetALBackGate(FALSE);	//Open BACK Gate for RGV

	lTimeOut = 2000;	// Timeout 2000ms
	lCurrentTime = GetCurTime();
	while (GetCurTime() < lCurrentTime + lTimeOut)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (!IsFrontGateSensorOn() && IsBackGateSensorOn())
		{
			break;
		}
		Sleep(100);
	}

	Sleep(200); //waiting
	if (IsFrontGateSensorOn() || !IsBackGateSensorOn())
	{
		return FALSE; //Not Open
	}

	return TRUE;
}


LONG CWaferLoader::OpMoveLoaderZToUnload_AutoLine(CONST BOOL bIsLoad, CONST LONG lSlotID)
{
	LONG lStatus = 0;

	if (m_fHardware == FALSE)
	{
		return 0;
	}

	CString szLog;
	szLog.Format("WL: OpMoveLoaderZToUnload_AutoLine - SlotID=%ld, LOAD=%d", lSlotID, bIsLoad);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if ((lSlotID < 0) || (lSlotID > 30))
	{
		CString szErr;
		szErr.Format("WL: Slot ID is invalid (%ld)", lSlotID);
		SetErrorMessage(szErr);
		HmiMessage_Red_Back(szErr, "AL UNLOAD");
		lStatus = HCACK_LOADER_PARA_CPVAL_NOT_DESIRED;
	}

	if (!lStatus)
	{
		LONG lTopSlotLevel_Z	= m_lALUnload_Z;	//m_stWaferMagazine[lMagazineNo].m_lTopLevel_Z;
		DOUBLE dSlotPitch		= m_dSlotPitch;	//m_stWaferMagazine[0].m_dSlotPitch;

//		2018.1.5 machine will not move offset(solt number), RGV will move up/down to get the frame
		LONG lPitch			= 0;
		//LONG lPitch			= (LONG)((((lSlotID-1) * dSlotPitch) / m_dZRes) + 0.5);
		INT nMoveState = Z_MoveTo(lTopSlotLevel_Z + lPitch);
		if (nMoveState != gnOK)
		{
			lStatus = HCACK_LOADER_BACK_GATE_OTHER_ERR;
		}

		szLog.Format("WL: OpMoveLoaderZToUnload_AutoLine - SlotID=%ld, LOAD=%d, Encoder=%ld (%ld, %.3f, %.3f), status=%d",
			lSlotID, bIsLoad, 
			lTopSlotLevel_Z + lPitch, lTopSlotLevel_Z, dSlotPitch, m_dZRes, 
			lStatus);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	}

	if (!lStatus)
	{
		if (!OpenALBackGate())
		{
			lStatus = HCACK_LOADER_BACK_GATE_NOT_OPENED;
		}
		m_lAutoLineUnloadSlotID		= lSlotID;		// Slot #1 - 30
	}

	return lStatus;
}
