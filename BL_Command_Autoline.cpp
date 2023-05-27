///////////////////////////////////////////////////////////////////////
// BL_Command.cpp : HMI Registered Command of the CBinrLoader class
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
#include "BinLoader.h"
#include "HmiDataManager.h"
#include "BT_CmdName.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"
#include "LogFileUtil.h"
#include "SC_Constant.h"
#include "CTmpChange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ==========================================================================================================
//			Related Host Command FRAME_LOAD, FRAME_LOAD_COMPLETE, 
//			FRAME_UNLOAD, FRAME_UNLOAD_COMPLETE function
// ==========================================================================================================
VOID CBinLoader::SECS_SetSecsGemHostCommandErr(const LONG lRet)
{
	//if SECS/GEM time-out, machine does not display error message and stop machine
	if ((lRet == HCACK_LOADER_BACK_GATE_NOT_OPENED) || (lRet == HCACK_LOADER_BACK_GATE_NOT_CLOSED) ||
		(lRet == HCACK_LOADER_BACK_GATE_OTHER_ERR))
	{
		m_lSecsGemHostCommandErr = lRet;
	}
}


LONG CBinLoader::SECS_LoadFrameCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lSlotID;
		LONG	lSlotGrade;	
	} MSAUTOLINESETUP;
	
	MSAUTOLINESETUP stInfo;
	svMsg.GetMsg(sizeof(MSAUTOLINESETUP), &stInfo);

	LONG lRet = SECS_SubLoadFrameCmd(stInfo.lSlotID, stInfo.lSlotGrade);

	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}

LONG CBinLoader::SECS_SubLoadFrameCmd(const LONG lSlotID, const LONG lSlotGrade)
{
	CString szLog;
	szLog.Format("BL: SECS_LoadFrameCmd - SlotID=%ld, GRADE=%ld, STATE=%d", 
					lSlotID, lSlotGrade, State());
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

	if (!lRet)
	{
		m_lAutoLineUnloadSlotID		= lSlotID;

		//Check Slot usage status before Load
		if (!CheckHostLoadSlotUsageStatus())
		{
			lRet = HCACK_LOADER_PARA_CPVAL_NOT_DESIRED;
		}
	}

	if (!lRet)
	{
		LoadUnloadSECSCmdLock();
		lRet = OpMoveLoaderZToUnload_AutoLine(TRUE, lSlotID);
		if (lRet)
		{
			CloseALBackGate(FALSE);
			LoadUnloadSECSCmdUnlock();
		}
		m_bAutoLineLoadDone			= FALSE;
	}

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("1");
	SECS_UpdateLoadUnloadStatus(1, BL_MGZ_TOP_1, lSlotID, TRUE, lRet);

	SECS_SetSecsGemHostCommandErr(lRet);

    return lRet;
}

LONG CBinLoader::SECS_LoadFrameDoneCmd(IPC_CServiceMessage& svMsg)
{
	LONG lStatus = 0;
	svMsg.GetMsg(sizeof(LONG),	&lStatus);

	LONG lRet = SECS_SubLoadFrameDoneCmd(lStatus);

	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}


LONG CBinLoader::SECS_SubLoadFrameDoneCmd(const LONG lStatus)
{
	CString szLog;
	szLog.Format("BL: SECS_LoadFrameDoneCmd - Status=%ld, SlotID = %d", lStatus, m_lAutoLineUnloadSlotID);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	LONG lRet = 0;
	if (!CloseALBackGate(FALSE))
	{
		lRet = HCACK_LOADER_BACK_GATE_NOT_CLOSED; //Not Close Door
	}

	if (lStatus != 0)	//0=FAIL, 1=SUCCESS
	{
		if ((m_lAutoLineUnloadSlotID > 0) && (m_lAutoLineUnloadSlotID <= MS_BL_MGZN_SLOT))
		{
			UpdateBulkLoadSlots();
			GenerateOMRTTableFile(BL_MGZ_TOP_1);
		}

		SaveMgznRTData();		//Update MgznRT MSD file
		for (long i = 0; i < MS_BL_MAX_MGZN_SLOT; i++)
		{
			SECS_UpdateCassetteSlotInfo(BL_MGZ_TOP_1, i + 1);
		}
		//Update WIP data for grade
		SECS_UpdateInputSlotGradeWIPCounter();
	}

	ResetBulkLoadData();

	m_lAutoLineUnloadSlotID		= 0;
	m_bAutoLineLoadDone		= TRUE;

	LoadUnloadSECSCmdUnlock();

	SECS_SetSecsGemHostCommandErr(lRet);

    return lRet;
}

LONG CBinLoader::SECS_UnloadFrameCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct 
	{
		LONG	lSlotID;
		LONG	lSlotGrade;	
	} MSAUTOLINESETUP;
	
	MSAUTOLINESETUP stInfo;
	svMsg.GetMsg(sizeof(MSAUTOLINESETUP), &stInfo);

	LONG lRet = SECS_SubUnloadFrameCmd(stInfo.lSlotID, stInfo.lSlotGrade);

	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}

LONG CBinLoader::SECS_SubUnloadFrameCmd(const LONG lSlotID, const LONG lSlotGrade)
{
	CString szLog;
	szLog.Format("BL: SECS_UnloadFrameCmd - SlotID=%ld, GRADE=%ld, STATE=%d", 
					lSlotID, lSlotGrade, State());
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

	if (!lRet)
	{
		m_lAutoLineUnloadSlotID		= lSlotID;

		//Check Barcode ID before Unload
		if (!CheckHostUnloadSlotIDBarcode())
		{
			lRet = HCACK_PARA_INVALID;
		}
	}

	if (!lRet)
	{
		LoadUnloadSECSCmdLock();
		lRet = OpMoveLoaderZToUnload_AutoLine(FALSE, lSlotID);
		if (lRet)
		{
			CloseALBackGate(FALSE);
			LoadUnloadSECSCmdUnlock();
		}
		m_bAutoLineUnloadDone	= FALSE;
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("Auto Line Unload Done Set To FALSE");
	}

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("SECS Update Load Unload Status in Sub Unload Frame Cmd");
	SECS_UpdateLoadUnloadStatus(1, BL_MGZ_TOP_1, lSlotID, FALSE, lRet);

	SECS_SetSecsGemHostCommandErr(lRet);

	return lRet;
}

LONG CBinLoader::SECS_UnloadFrameDoneCmd(IPC_CServiceMessage& svMsg)
{
	LONG lStatus = 0;
	svMsg.GetMsg(sizeof(LONG),	&lStatus);

	LONG lRet = SECS_SubUnloadFrameDoneCmd(lStatus);

	svMsg.InitMessage(sizeof(LONG), &lRet);
    return TRUE;
}


LONG CBinLoader::SECS_SubUnloadFrameDoneCmd(const LONG lStatus)
{
	CString szLog;
	szLog.Format("BL: SECS_UnloadFrameDoneCmd - Status=%ld, SlotID=%ld", 
		lStatus, m_lAutoLineUnloadSlotID);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	LONG lRet = 0;
	if (!CloseALBackGate(FALSE))
	{
		lRet = HCACK_LOADER_BACK_GATE_NOT_CLOSED; //Not Close Door
	}

	if (lStatus != 0)	//0=FAIL, 1=SUCCESS
	{
		if ((m_lAutoLineUnloadSlotID > 0) && (m_lAutoLineUnloadSlotID <= MS_BL_MGZN_SLOT))
		{
			UpdateBulkLoadSlots(TRUE);
			GenerateOMRTTableFile(BL_MGZ_TOP_1);
		}

		SaveMgznRTData();		//Update MgznRT MSD file
		for (long i = 0; i < MS_BL_MAX_MGZN_SLOT; i++)
		{
			SECS_UpdateCassetteSlotInfo(BL_MGZ_TOP_1, i + 1);
		}
	}

	ResetBulkLoadData();

	m_lAutoLineUnloadSlotID		= 0;
	m_bAutoLineUnloadDone		= TRUE;
	szLog.Format("BL: SECS_UnloadFrameDoneCmd - m_bAutoLineUnloadDone=%ld", m_bAutoLineUnloadDone);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
	{
		BOOL bRet1 = TransferSortingMgznSlotFromWIPToOutput_AutoLine();
		BOOL bRet2 = AdjustOutputSlotSortingPosn_AutoLine();
		if (bRet1 || bRet2)
		{
			SaveData();
		}
	}

//====================================
//SendBinTransferEvent_8024(0,5);
//=================================================
	LoadUnloadSECSCmdUnlock();

	m_bTriggerTransferBinFrame = FALSE;
	if ((lRet == 0) && IsTransferFullWIPtoOutput())
	{
		LONG lFrameOnBT = CheckFrameOnBinTable();
		if (lFrameOnBT == BL_FRAME_NOT_EXIST)
		{
			//there has not bin frame in the expander and will trigger to transfer Bin from WIP to Output
			m_bTriggerTransferBinFrame = TRUE;
		}
	}

	SECS_SetSecsGemHostCommandErr(lRet);

	return lRet; 
}

BOOL CBinLoader::IsCloseALBackGate()
{
	return (IsFrontGateSensorOn() && !IsBackGateSensorOn());
}


BOOL CBinLoader::CloseALFrontGateWithLock()
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


BOOL CBinLoader::OpenALFrontGateWithLock()
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

// Open Status Will trigger sensor
// Close Status will leave sensor position
BOOL CBinLoader::CloseALFrontGate()
{
return TRUE;
	if (!IsMSAutoLineMode())
	{
		return TRUE;
	}

	Sleep(500);
	CMSLogFileUtility::Instance()->BL_LogStatus("CloseALFrontGate");
	//CMSLogFileUtility::Instance()->MS_LogSECSGEM("CloseALFrontGate");

	SetALFrontGate(FALSE);	//Close FRONT gate
	
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
		szMsg = _T("Bin Loader Front Gate Not Closed");
		SetAlert_Msg_Red_Yellow(IDS_BL_FRONT_GATE_NOT_CLOSED, szMsg);		
		SetErrorMessage(szMsg);
		return FALSE; //Not Closed
	}

	return TRUE;
}


// Open Status Will trigger sensor
// Close Status will leave sensor position
BOOL CBinLoader::OpenALFrontGate()
{
return TRUE;
	if (!IsMSAutoLineMode())
	{
		return TRUE;
	}
	Sleep(500);
	CMSLogFileUtility::Instance()->BL_LogStatus("OpenALFrontGate");
	SetALFrontGate(TRUE);	//Open FRONT gate
	
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
		szMsg = _T("Bin Loader Front Gate Not Opened");
		SetAlert_Msg_Red_Yellow(IDS_BL_FRONT_GATE_NOT_OPENED, szMsg);		
		SetErrorMessage(szMsg);
		return FALSE; //Not Open
	}

	return TRUE;
}

// Open Status Will trigger sensor
// Close Status will leave sensor position
BOOL CBinLoader::CloseALBackGate(BOOL bOpenFrontGate)
{
return TRUE;
	CMSLogFileUtility::Instance()->BL_LogStatus("CloseALBackGate SetALBackGate(FALSE)");
	SetALBackGate(FALSE);	//Close BACK Gate when RGV done
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
		CMSLogFileUtility::Instance()->BL_LogStatus("CloseALBackGate SetALFrontGate(FALSE)");
		SetALFrontGate(TRUE);	//Open FRONT gate for MS

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

BOOL CBinLoader::OpenALBackGate()
{
return TRUE;
	CMSLogFileUtility::Instance()->BL_LogStatus("OpenALBackGate SetALFrontGate(FALSE)");
	SetALFrontGate(FALSE);	//Close FRONT gate
	
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
	CMSLogFileUtility::Instance()->BL_LogStatus("OpenALBackGate SetALBackGate(TRUE)");
	SetALBackGate(TRUE);	//Open BACK Gate for RGV

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

	Sleep(200);
	if (IsFrontGateSensorOn() || !IsBackGateSensorOn())
	{
		return FALSE; //Not Open
	}

	return TRUE;
}


LONG CBinLoader::OpMoveLoaderZToUnload_AutoLine(CONST BOOL bIsLoad, CONST LONG lSlotID)
{
	LONG lStatus = 0;

	if ( (lSlotID < 0) || (lSlotID > 30) )
	{
		CString szErr;
		szErr.Format("BL: Slot ID is invalid (%ld)", lSlotID);
		SetErrorMessage(szErr);
//		HmiMessage_Red_Back(szErr, "AL UNLOAD");
		lStatus = HCACK_LOADER_PARA_CPVAL_NOT_DESIRED;
	}
/*
	if (!lStatus && (IsFrameOutOfMgz() == TRUE))
	{
		SetErrorMessage("BL Frame is out of magazine in OpMoveLoaderZToUnload_AutoLine");
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		lStatus = HCACK_LOADER_BACK_GATE_OTHER_ERR;
	}

	//Check gripper encoder also
	if (!lStatus && (IsMagazineSafeToMove() == FALSE))
	{
		SetErrorMessage("Gripper is not in safe position in OpMoveLoaderZToUnload_AutoLine");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		lStatus = HCACK_LOADER_BACK_GATE_OTHER_ERR;
	}
*/
	if (!lStatus)
	{
		CString szLog;
		szLog.Format("BL: OpMoveLoaderZToUnload_AutoLine - SlotID=%ld", lSlotID);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		m_lAutoLineUnloadSlotID = lSlotID;							// Slot #1 - 30

//		MS50 Autoline Z position of TOP1 magazinw is fixed, RGV will adjust the Z level to
//		to get/put frame
/*
		LONG lOffsetZ = 0;
//		2018.1.5 machine will not move offset(solt number), RGV will move up/down to get the frame
//		if (lSlotID > 0)
//		{
//			lOffsetZ = m_stMgznRT[BL_MGZ_MID_1].m_lSlotPitch * (lSlotID - 1);	//0-29
//		}
		Z_MoveTo(m_lAutoLineUnloadZ + lOffsetZ, SFM_WAIT);
*/
		if (!OpenALBackGate())
		{
			lStatus = HCACK_LOADER_BACK_GATE_NOT_OPENED;
		}
	}

	return lStatus;
}


//===================================================================================================================================
//			Transfer Inline to Standlone or Standlone to Inline
//====================================================================================================================================
BOOL CBinLoader::IsSIS_StandardMode()
{
	return ((m_stMgznOM[0].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE) && (m_stMgznOM[3].m_lMgznUsage == BL_MGZN_USAGE_UNUSE) && 
			(m_stMgznOM[4].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE));
}

BOOL CBinLoader::IsSIS_StandaloneMode()
{
	return ((m_stMgznOM[0].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE) && (m_stMgznOM[3].m_lMgznUsage == BL_MGZN_USAGE_FULL) &&
			(m_stMgznOM[4].m_lMgznUsage == BL_MGZN_USAGE_EMPTY));
}

LONG CBinLoader::SIS_TransferMgznToMgzn(const LONG lSrcMgzn, const LONG lTargetMgzn)
{
	//ResetMagazine(lTargetMgzn);
	for (LONG lSlot = 0; lSlot < MS_BL_AUTO_LINE_OUTPUT_END_MGZN_SLOT; lSlot++)
	{
		if ((m_stMgznRT[lSrcMgzn].m_lSlotUsage[lSlot] != BL_SLOT_USAGE_UNUSE) &&
			(m_stMgznRT[lSrcMgzn].m_lSlotUsage[lSlot] != BL_SLOT_USAGE_EMPTY) && 
			!m_stMgznRT[lSrcMgzn].m_SlotBCName[lSlot].IsEmpty())
		{
			LONG lRet = UDB_MgznSlotToMgznSlot(&m_clUpperGripperBuffer, lSrcMgzn, lSlot, lTargetMgzn, lSlot);
			if (lRet != TRUE)
			{
				return lRet;
			}
		}
	}
	return TRUE;
}


LONG CBinLoader::SIS_TransferEmptyFrameMgznToMgzn(const LONG lSrcMgzn, const LONG lTargetMgzn)
{
	//ResetMagazine(lTargetMgzn);
	for (LONG lSlot = MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1; lSlot < MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT; lSlot++)
	{
		if ( (m_stMgznRT[lSrcMgzn].m_lSlotUsage[lSlot] == BL_SLOT_USAGE_ACTIVE)	&& 
			 (m_stMgznRT[lSrcMgzn].m_lSlotBlock[lSlot] == 0)					&& 
			 !m_stMgznRT[lSrcMgzn].m_SlotBCName[lSlot].IsEmpty())
		{
			LONG lRet = UDB_MgznSlotToMgznSlot(&m_clUpperGripperBuffer, lSrcMgzn, lSlot, lTargetMgzn, lSlot);
			if (lRet != TRUE)
			{
				return lRet;
			}
		}
	}
	return TRUE;
}


LONG CBinLoader::SIS_TransferInlineToStandalone(IPC_CServiceMessage& svMsg)
{
	LONG lRet = TRUE;

	if (IsMSAutoLineStandloneMode() && IsSIS_StandardMode())
	{
		LONG lStatus = BL_YES_NO(HMB_BL_INLINE_TO_STANDALONE, IDS_BL_OPERATION);

		if (lStatus == TRUE)
		{
			if (m_lTransferingInlineToStandalone1 == 0)
			{
				ResetMagazine(BL_MGZ_TOP_2);
				m_lTransferingInlineToStandalone1 = 1;
			}
			
			if (m_lTransferingInlineToStandalone1 == 1)
			{
				lRet = SIS_TransferMgznToMgzn(BL_MGZ_TOP_1, BL_MGZ_TOP_2);
			}

			if (lRet == TRUE)
			{
				m_lTransferingInlineToStandalone1 = 2; //Finished

				if (m_lTransferingInlineToStandalone2 == 0)
				{
					ResetMagazine(BL_MGZ_TOP_1);
					m_lTransferingInlineToStandalone2 = 1;
				}

				if (m_lTransferingInlineToStandalone2 == 1)
				{
					lRet = SIS_TransferMgznToMgzn(BL_MGZ_MID_2, BL_MGZ_TOP_1);
				}
			}

			if (lRet == TRUE)
			{
				m_lTransferingInlineToStandalone2 = 2;

				if (m_lTransferingInlineToStandalone3 == 0)
				{
					ResetMagazine(BL_MGZ_MID_2);
					m_lTransferingInlineToStandalone3 = 1;
				}

				if (m_lTransferingInlineToStandalone3 == 1)
				{
					lRet = SIS_TransferEmptyFrameMgznToMgzn(BL_MGZ_TOP_2, BL_MGZ_MID_2);
				}
			}
		
			if (lRet == TRUE)
			{
				//Change standard F Mode
				SetOperationMode_Standard_F();
				SetOperationMode_SIS_Standalone_F();
				SaveMgznOMData();
				m_lTransferingInlineToStandalone1 = 0;
				m_lTransferingInlineToStandalone2 = 0;
				m_lTransferingInlineToStandalone3 = 0;
			}
		}
		else
		{
			lRet = FALSE;
		}
	}

	if (lRet)
	{
		HmiMessage("Transfer Inline To Standalone Finished");
	}
	SaveData();
	SaveMgznRTData();
    svMsg.GetMsg(sizeof(BOOL), &lRet);
	return TRUE;
}


LONG CBinLoader::SIS_TransferStandaloneToInline(IPC_CServiceMessage& svMsg)
{
	LONG lRet = TRUE;
	if (IsMSAutoLineMode() && IsSIS_StandaloneMode())
	{
		LONG lStatus = BL_YES_NO(HMB_BL_STANDALONE_TO_INLINE, IDS_BL_OPERATION);

		if (lStatus == TRUE)
		{
			if (m_lTransferingStandaloneToInline == 0)
			{
				ResetMagazine(BL_MGZ_MID_2);
				m_lTransferingStandaloneToInline = 1;
			}

			if (m_lTransferingStandaloneToInline == 1)
			{
				lRet = SIS_TransferMgznToMgzn(BL_MGZ_TOP_1, BL_MGZ_MID_2);
			}

			if (lRet == TRUE)
			{
				//Change Mode F(autoline)
				ResetMagazine(BL_MGZ_TOP_1);
				SetOperationMode_F_AutoLine();
				SetOperationMode_SIS_F();
				SaveMgznOMData();
				m_lTransferingStandaloneToInline = 0;
			}
		}
		else
		{
			lRet = FALSE;
		}
	}

	if (lRet)
	{
		HmiMessage("Transfer Standalone To Inline Finished");
	}
	SaveData();
	SaveMgznRTData();
    svMsg.GetMsg(sizeof(BOOL), &lRet);
	return TRUE;
}
//==============================================================================================================================


VOID CBinLoader::SetOperationMode_SIS_Standalone_F()
{
    short j;

	// top1
	// top1 as WIP magazine under SIS-standlone machine
	m_stMgznRT[0].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznRT[0].m_lSlotBlock[j] = j + 26;
		if (m_stMgznRT[0].m_SlotBCName[j].IsEmpty() || m_stMgznRT[0].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
		{
			m_stMgznRT[0].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}

	// middle1
	m_stMgznRT[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for(j=0; j<MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznRT[1].m_lSlotBlock[j] = j + 1;
		if (m_stMgznRT[1].m_SlotBCName[j].IsEmpty() || m_stMgznRT[1].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
		{
			m_stMgznRT[1].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}

	// bottom1
	m_stMgznRT[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznRT[2].m_lSlotBlock[j] = j + 51;
		if (m_stMgznRT[2].m_SlotBCName[j].IsEmpty() || m_stMgznRT[2].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
		{
			m_stMgznRT[2].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}
	
	// Mode8 bottom1
	m_stMgznRT[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznRT[6].m_lSlotBlock[j] = j + 101;
		if (m_stMgznRT[6].m_SlotBCName[j].IsEmpty() || m_stMgznRT[6].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
		{
			m_stMgznRT[6].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}

	// top2
	// top2 as FULL Magazine under SIS-standlone
	m_stMgznRT[3].m_lMgznUsage = BL_MGZN_USAGE_FULL;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		if (m_stMgznRT[3].m_SlotBCName[j].IsEmpty())
		{
			m_stMgznRT[3].m_lSlotBlock[j] = 0;
			m_stMgznRT[3].m_lSlotWIPCounter[j] = 0;
		}

		m_stMgznRT[3].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
	}

	// middle2
	// middle2 as EMPTY Magazine under SIS-standlone
	m_stMgznRT[4].m_lMgznUsage = BL_MGZN_USAGE_EMPTY;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		if (m_stMgznRT[4].m_SlotBCName[j].IsEmpty())
		{
			m_stMgznRT[4].m_lSlotBlock[j] = 0;
			m_stMgznRT[4].m_lSlotWIPCounter[j] = 0;
		}
		m_stMgznRT[4].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
	}

	// bottom2
	m_stMgznRT[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznRT[5].m_lSlotBlock[j] = j + 76;
		if (m_stMgznRT[5].m_SlotBCName[j].IsEmpty() || m_stMgznRT[5].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
		{
			m_stMgznRT[5].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}
	
	// Mode8 bottom2
	m_stMgznRT[7].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznRT[7].m_lSlotBlock[j] = j + 126;
		if (m_stMgznRT[7].m_SlotBCName[j].IsEmpty() || m_stMgznRT[7].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
		{
			m_stMgznRT[7].m_lSlotUsage[j] = BL_SLOT_USAGE_ACTIVE2FULL;
		}
	}
}

VOID CBinLoader::SetEmptySlotStatus(const LONG lMgzn, const LONG lSlot, const LONG lSlotGradeBlock)
{
	m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lSlotGradeBlock;
	if (m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] == BL_SLOT_USAGE_ACTIVE2FULL)
	{
		m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_EMPTY;
		m_stMgznRT[lMgzn].m_lSlotWIPCounter[lSlot] = 0;
	}
}


VOID CBinLoader::SetOperationMode_SIS_F()
{
    short j;

	// top1
	m_stMgznRT[0].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		if ((m_stMgznRT[0].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE) &&
			(m_stMgznRT[0].m_SlotBCName[j].IsEmpty()))
		{
			m_stMgznRT[0].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
			m_stMgznRT[0].m_lSlotWIPCounter[j] = 0;
		}
	}

	// middle1		//The only mag available for LOAD/UNLOAD
	m_stMgznRT[1].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetEmptySlotStatus(1, j, j + 1);
	}

	// bottom1
	m_stMgznRT[2].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j=0; j<MS_BL_MGZN_SLOT; j++)
	{
		SetEmptySlotStatus(2, j, j + 51);
	}

	// Mode8 bottom1
	m_stMgznRT[6].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetEmptySlotStatus(6, j, j + 101);
	}

	// top2 --- not used
	m_stMgznRT[3].m_lMgznUsage = BL_MGZN_USAGE_UNUSE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznRT[3].m_lSlotBlock[j] = 0;
		m_stMgznRT[3].m_lSlotUsage[j] = BL_SLOT_USAGE_EMPTY;
		m_stMgznRT[3].m_SlotBCName[j].Empty();
		m_stMgznRT[3].m_lSlotWIPCounter[j] = 0;
	}

	// middle2
	m_stMgznRT[4].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetEmptySlotStatus(4, j, j + 26);
	}

	// bottom2
	m_stMgznRT[5].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetEmptySlotStatus(5, j, j + 76);
	}

	// Mode8 bottom2
	m_stMgznRT[7].m_lMgznUsage = BL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		SetEmptySlotStatus(7, j, j + 126);
	}
}
