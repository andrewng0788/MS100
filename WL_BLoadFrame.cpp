/////////////////////////////////////////////////////////////////
// BL_WLoadFrame.cpp : Move functions of the CBinLoader class
//
//	Description:
//		MS899 Mapping Die Sorter
//
//	Date:		November 5, 2012
//	Revision:	1.00
//
//	By:			Andrew
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004-2012
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "MS_SecCommConstant.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ==========================================================================================================
//	WL IPC commands for Yealy MS100Plus Single Loader config
// ==========================================================================================================

LONG CWaferLoader::ManualLoadFromMgznToBuffer(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bUseUpperSlot = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseUpperSlot);

	if (!m_fHardware || !m_bUseBLAsLoaderZ)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_bDisableWL || m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_lCurrentMagNo > GetWL1MaxMgznNo())
	{
		m_lCurrentMagNo = GetWL1MaxMgznNo();
	}

	if (m_lCurrentSlotNo > m_stWaferMagazine[m_lCurrentMagNo-1].m_lNoOfSlots)
	{
		m_lCurrentSlotNo = m_stWaferMagazine[m_lCurrentMagNo-1].m_lNoOfSlots;
	}

	if (bUseUpperSlot)
	{
		if (m_bFrameInUpperSlot || IsUpperSlotFrameExist() )
		{
			CString szMsg1;
			szMsg1.Format("Frame is detected on buffer UPPER slot; continue to LOAD frame at Mag #%ld Slot #%ld?", 
							m_lCurrentMagNo, m_lCurrentSlotNo);
			if (HmiMessage(szMsg1, "Manual LOAD (Mgzn -> Buf)", glHMI_MBX_OKCANCEL|0x80000000) == glHMI_CANCEL)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
	}
	else
	{
		if (m_bFrameInLowerSlot || IsLowerSlotFrameExist() )
		{
			CString szMsg1;
			szMsg1.Format("Frame is detected on buffer LOWER slot; continue to LOAD frame at Mag #%ld Slot #%ld?", 
							m_lCurrentMagNo, m_lCurrentSlotNo);
			if (HmiMessage(szMsg1, "Manual LOAD (Mgzn -> Buf)", glHMI_MBX_OKCANCEL|0x80000000) == glHMI_CANCEL)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
	}

//HmiMessage("ManualLoadFromMgznToBuffer ....");

	bReturn = MoveToBLMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, TRUE, bUseUpperSlot);
	if (!bReturn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	MoveToBLMagazineSlot_Sync();

	CMSLogFileUtility::Instance()->MS_LogOperation("Manual WL LOAD frame Mgzn->Buffer");	
	LONG lStatus = WL_LoadFromMgzToBuffer(FALSE, bUseUpperSlot);
	if (lStatus != TRUE)
	{
		//bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::ManualUnloadFromBufferToMgzn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bUseUpperSlot = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseUpperSlot);


	if (!m_fHardware || !m_bUseBLAsLoaderZ)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_bDisableWL || m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_lCurrentMagNo > GetWL1MaxMgznNo())
	{
		m_lCurrentMagNo = GetWL1MaxMgznNo();
	}

	if (m_lCurrentSlotNo > m_stWaferMagazine[m_lCurrentMagNo-1].m_lNoOfSlots)
	{
		m_lCurrentSlotNo = m_stWaferMagazine[m_lCurrentMagNo-1].m_lNoOfSlots;
	}

	if (bUseUpperSlot)
	{
		if (!m_bFrameInUpperSlot || !IsUpperSlotFrameExist() )
		{
			if (HmiMessage("Frame is not detected on buffer UPPER slot; continue to UNLOAD frame?", 
				"Manual UNLOAD (Buf -> Mgzn)", glHMI_MBX_OKCANCEL|0x80000000) == glHMI_CANCEL)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
	}
	else
	{
		if (!m_bFrameInLowerSlot || !IsLowerSlotFrameExist() )
		{
			if (HmiMessage("Frame is not detected on buffer LOWER slot; continue to UNLOAD frame?", 
				"Manual UNLOAD (Buf -> Mgzn)", glHMI_MBX_OKCANCEL|0x80000000) == glHMI_CANCEL)
			{
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return TRUE;
			}
		}
	}

//HmiMessage("ManualUnloadFromBufferToMgzn");
	bReturn = MoveToBLMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, TRUE, FALSE, bUseUpperSlot);
	if (!bReturn)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	MoveToBLMagazineSlot_Sync();

	CMSLogFileUtility::Instance()->MS_LogOperation("Manual WL UNLOAD frame buffer->Mgzn");	
	LONG lStatus = WL_UnloadFromBufferToMgz(FALSE, bUseUpperSlot);
	if (lStatus != TRUE)
	{
		//bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::ManualLoadFromBufferToTable(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bUseUpperSlot = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseUpperSlot);


	if (!m_fHardware || !m_bUseBLAsLoaderZ)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_bDisableWL || m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (m_bFrameExistOnExp)
	{
		if (HmiMessage("Frame existed on table; continue to load film frame?", "Manual Load Frame", glHMI_MBX_OKCANCEL|0x80000000) == glHMI_CANCEL)
		{
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

//HmiMessage("ManualLoadFromBufferToTable");
	CMSLogFileUtility::Instance()->MS_LogOperation("Manual WL LOAD frame buffer->Table");	
	LONG lStatus = WL_LoadFromBufferToTable(FALSE, FALSE, bUseUpperSlot);
	if (lStatus != TRUE)
	{
		SetErrorMessage("Manual WL LOAD frame buffer->Table fails");
		//bReturn = FALSE;
	}

	SendGemEvent_WL_Load(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferLoader::ManualUnloadFromTableToBuffer(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bUseUpperSlot = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseUpperSlot);

	if (!m_fHardware || !m_bUseBLAsLoaderZ)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_bDisableWL || m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (!m_bFrameExistOnExp)
	{
		if (HmiMessage("Frame not existed on table; continue to Unload film frame?", 
							"Manual Unload Frame", glHMI_MBX_OKCANCEL|0x80000000) == glHMI_CANCEL)
		{
			bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}
	}

//HmiMessage("ManualUnloadFromTableToBuffer");

	CMSLogFileUtility::Instance()->MS_LogOperation("Manual WL LOAD frame buffer->Table");	
	LONG lStatus = WL_UnloadFromTableToBuffer(FALSE, bUseUpperSlot);
	if (lStatus != TRUE)
	{
		SetErrorMessage("Manual WL UNLOAD frame Table->Buffer fails");
		//bReturn = FALSE;
	}

	SendGemEvent_WL_Load(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

typedef struct 
{
	LONG	lCurrMgznNo;
	LONG	lCurrSlotNo;	
	LONG	lLSlotStatus;
	LONG	lUSlotStatus;
} BLKDATA;

LONG CWaferLoader::GetCurrentMgznSlotNo(IPC_CServiceMessage& svMsg)
{
	BLKDATA stInfo;
	stInfo.lCurrMgznNo	= m_lCurrentMagNo;		//1-based index
	stInfo.lCurrSlotNo	= m_lCurrentSlotNo;		//1-based index
	stInfo.lLSlotStatus	= 0;
	if (m_bFrameInLowerSlot)
		stInfo.lLSlotStatus	= 1;
	stInfo.lUSlotStatus	= 0;
	if (m_bFrameInUpperSlot)
		stInfo.lUSlotStatus	= 1;

	svMsg.InitMessage(sizeof(BLKDATA), &stInfo);
	return 1;
}

LONG CWaferLoader::SetCurrentMgznSlotNo(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	BLKDATA stInfo;
	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	LONG lCurrentMagNo	= stInfo.lCurrMgznNo;		//1-based index
	LONG lCurrentSlotNo	= stInfo.lCurrSlotNo;		//1-based index
	
	if ( (lCurrentMagNo < 1) || (lCurrentMagNo > GetWL1MaxMgznNo()) )
	{
		bReturn = FALSE;
	}
	else if ((lCurrentSlotNo < 1) || (lCurrentSlotNo > m_stWaferMagazine[lCurrentMagNo-1].m_lNoOfSlots) )
	{
		bReturn = FALSE;
	}
	else
	{
		m_lCurrentMagNo		= lCurrentMagNo;
		m_lCurrentSlotNo	= lCurrentSlotNo;
		SaveData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferLoader::AutoPreloadFrameFromMgznToBuffer(IPC_CServiceMessage& svMsg)
{
	LONG lReturnCode = TRUE;
	BLKDATA stInfo;
	svMsg.GetMsg(sizeof(BLKDATA), &stInfo);

	if (!m_fHardware || !m_bUseBLAsLoaderZ)
	{
		svMsg.InitMessage(sizeof(LONG), &lReturnCode);
		return 1;
	}
	if (m_bDisableWL || m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(LONG), &lReturnCode);
		return TRUE;
	}
	if (m_bFrameInLowerSlot)
	{
		HmiMessage("WL Error: Lower buffer Slot already has frame!");
		SetErrorMessage("WL: auto Preload frame detects frame in Buffer Table LOWER slot!");
		lReturnCode = FALSE;
		svMsg.InitMessage(sizeof(LONG), &lReturnCode);
		return TRUE;
	}

	BOOL bUseUpperSlotForUnload = TRUE;

	CMSLogFileUtility::Instance()->MS_LogOperation("Auto WL LOAD frame Mgzn->Buffer");	
	lReturnCode = WL_LoadFromMgzToBuffer(IsBurnIn(), !bUseUpperSlotForUnload, TRUE);		//PRELOAD TO Lower Slot
	//if (lStatus != TRUE)
	//{
	//	lReturnCode = FALSE;
	//}

	m_lCurrentMagNo		= stInfo.lCurrMgznNo;
	m_lCurrentSlotNo	= stInfo.lCurrSlotNo;
	SaveData();

	svMsg.InitMessage(sizeof(LONG), &lReturnCode);
	return 1;
}

LONG CWaferLoader::AutoUnloadFrameFromBufferToMgzn(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_fHardware || !m_bUseBLAsLoaderZ)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_bDisableWL || m_bDisableWLWithExp)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (!m_bFrameInUpperSlot)
	{
		HmiMessage("WL Error: Upper buffer Slot has no frame to be UNLOAD!");
		SetErrorMessage("WL: auto UNLOAD frame detects no frame in Buffer Table UPPER slot!");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bUseUpperSlotForUnload = TRUE;

	CMSLogFileUtility::Instance()->MS_LogOperation("Manual WL UNLOAD frame buffer->Mgzn");	
	LONG lStatus = WL_UnloadFromBufferToMgz(IsBurnIn(), bUseUpperSlotForUnload, TRUE);
	if (lStatus != TRUE)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferLoader::ResetBufferTable(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bUseBLAsLoaderZ)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bFrameInUpperSlot)
	{
		if (IsUpperSlotFrameExist())
		{
			HmiMessage_Red_Yellow("RESET Buffer: frame detected on UPPER buffer slot; please remove it manually");
			SetErrorMessage("RESET-BUFFER: upper buffer frame detected by sensor");
		}

		m_szUpperSlotBarcode	= _T("");
		m_bFrameInUpperSlot		= FALSE;
	}

	if (m_bFrameInLowerSlot)
	{
		if (IsUpperSlotFrameExist())
		{
			HmiMessage_Red_Yellow("RESET Buffer: frame detected on LOWER buffer slot; please remove it manually");
			SetErrorMessage("RESET-BUFFER: lower buffer frame detected by sensor");
		}

		m_szLowerSlotBarcode	= _T("");
		m_bFrameInLowerSlot		= FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


// ==========================================================================================================
// ==========================================================================================================

INT CWaferLoader::CheckPreLoadUnloadStatus(BOOL bCheckExpander)
{
	//Check Banana scope
	if ( bCheckExpander && (IsScopeDown() == TRUE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Scope is down\n");
		SetErrorMessage("Scope is down");

		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}
	if (!X_IsPowerOn())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper power not ON\n");
		HmiMessage_Red_Yellow("Gripper power not ON!", "Load Film Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
		return FALSE;
	}
	// Check frame is on gripper before moving the gripper
	if (IsFrameDetect() == TRUE && m_bNoSensorCheck == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper detected frame on gripper before LOAD\n");
		SetErrorMessage("wafer gripper detected frame on gripper before LOAD");
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		return Err_FrameExistInGripper;
	}

	if (bCheckExpander)
	{
		if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && IsExpanderLock() )	
		{
			SetExpanderLock(WL_OFF);
			Sleep(500);
			if (IsExpanderLock())
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander unlock fail at Load\n");
				SetErrorMessage("Expander unlock fail at Load");
				SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
				return Err_ExpanderLockFailed;
			}
		}
	}

	return TRUE;
}


// ==========================================================================================================
// ==========================================================================================================

LONG CWaferLoader::WL_LoadFromMgzToBuffer(BOOL bBurnInLoad, BOOL bBufferLevel, BOOL bIsAutoBond)
{
	CString szLog;
	szLog.Format("Load (Mgzn to Buffer) start  %d  %d", bBurnInLoad, bBufferLevel);
	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);


	INT nPreStatus = CheckPreLoadUnloadStatus(FALSE);
	if (nPreStatus != TRUE)
		return nPreStatus;

	//Check Magazine is inside the loader Z
	if ( (IsMagazineExist() == FALSE) && (bBurnInLoad == FALSE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Magazine is not exist");
		SetErrorMessage("Magazine not exist");

		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);	
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	if (!bIsAutoBond)
		X_MoveTo(m_lReadyPos_X, FALSE);
	if (bBufferLevel == WL_BUFFER_UPPER)
		Z_MoveTo(m_lBufTableUSlot_Z);
	else
		Z_MoveTo(m_lBufTableLSlot_Z);
	X_Sync();


	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper Down");
	SetGripperLevel(TRUE);
	SetGripperState(WL_OFF);


	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper move to LOAD position");
	if (WaferGripperMoveSearchJam(m_lLoadPos_X) == FALSE)		//v3.89
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to load from mgz to buffer");
		SetErrorMessage("Gripper is Jam @ to load");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at LOAD");
		SetErrorMessage("Gripper is OFF @ at LOAD");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to load from mgz to buffer 2");
		SetErrorMessage("Gripper is Jam @ to load");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}


	BOOL bFrameDetected = IsFrameDetect();
	if (bBurnInLoad)
	{
		bFrameDetected = TRUE;
	}
	else if (bFrameDetected != TRUE)
	{
		Sleep(200);
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)
		{
			LONG lSearchDistanceX = WL_GRIPPER_SEARCH_DIST;
			if (MoveSearchFrame(-1 * (lSearchDistanceX )) != TRUE)	
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Move Search Frame fails at LOAD");
				SetErrorMessage("Move Search Frame fails at LOAD");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
				return Err_FrameJammed;
			}
		}
	}

	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at LOAD");
		SetErrorMessage("Gripper is OFF @ at LOAD");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ at LOAD");
		SetErrorMessage("Gripper is Jam @ to load");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		if (X_IsPowerOn())
		{
			CMS896AStn::MotionPowerOff(WL_AXIS_X, &m_stWLAxis_X);		//v3.89	//for safety purpose
		}
		return Err_FrameJammed;
	}

	if (bFrameDetected != TRUE && m_bNoSensorCheck == FALSE)
	{
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)	
		{
			CString szErr;
			szErr.Format("No Frame is found at LOAD in Mgzn=%ld, Slot=%ld", m_lCurrentMagNo, m_lCurrentSlotNo); 
			CMSLogFileUtility::Instance()->WL_LogStatus(szErr);
			SetErrorMessage(szErr);
			SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
			if (bIsAutoBond)
				X_MoveTo(m_lLoadPos_X + 2000, TRUE);
			else
				X_MoveTo(m_lReadyPos_X, TRUE);
			return Err_NoFrameDetected;
		}
	}


	SetGripperState(WL_ON);
	Sleep(200);

	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper to Buffer LOAD");

	if (!bBurnInLoad && m_bUseBarcode)
	{
		CString szBC = _T("");
		INT nBCStatus = ScanBySymbolOnBufferTable(TRUE, szBC);
		X_MoveTo(m_lBufferLoadPos_X + 200, SFM_NOWAIT);
		if (nBCStatus != TRUE)
		{


		}

		if (bBufferLevel == WL_BUFFER_UPPER)
			m_szUpperSlotBarcode = szBC;
		else
			m_szLowerSlotBarcode = szBC;
	}
	else
	{
		X_MoveTo(m_lBufferLoadPos_X + 200, FALSE);
	}

	X_Sync();
	bFrameDetected = IsFrameDetect();
	SetGripperState(WL_OFF);
	Sleep(200);
	X_MoveTo(m_lReadyPos_X, TRUE);
	SetGripperLevel(WL_OFF);


	if (bBurnInLoad)
	{
		bFrameDetected = TRUE;
	}
	else if (bFrameDetected != TRUE && m_bNoSensorCheck == FALSE)	
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No Frame is detected at BUFFER LOAD");
		SetErrorMessage("No Frame is detected at BUFFER LOAD");
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
		return Err_FrameShifted;
	}


	//Check if Frame is sit in slot properly
	CMSLogFileUtility::Instance()->WL_LogStatus("Check Frame in Slot");
	BOOL bFrameInSlot = FALSE;
	if (bBufferLevel == WL_BUFFER_UPPER)
		bFrameInSlot = IsUpperSlotFrameExist();
	else
		bFrameInSlot = IsLowerSlotFrameExist();
	if (bBurnInLoad)
	{
		bFrameInSlot = TRUE;
	}
	if (!bFrameInSlot && m_bNoSensorCheck == FALSE)
	{
		if (bBufferLevel == WL_BUFFER_UPPER)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No Frame is detected at upper Buffer Slot\n");
			SetErrorMessage("No Frame is detected at upper Buffer Slot");
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No Frame is detected at lower Buffer Slot\n");
			SetErrorMessage("No Frame is detected at lower Buffer Slot");
		}
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
		return Err_FrameShifted;
	}

	if (!IsBufferFrameProtectSafe() && m_bNoSensorCheck == FALSE)
	{
		if (bBufferLevel == WL_BUFFER_UPPER)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame hit FrameProtect sensor at upper Buffer Slot\n");
			SetErrorMessage("Frame hit FrameProtect sensor at upper Buffer Slot");
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame hit FrameProtect sensor at lower Buffer Slot\n");
			SetErrorMessage("Frame hit FrameProtect sensor at lower Buffer Slot");
		}
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
		return Err_FrameShifted;
	}


	if (bBufferLevel == WL_BUFFER_UPPER)
		m_bFrameInUpperSlot	= TRUE;
	else
		m_bFrameInLowerSlot	= TRUE;
	m_bIsGetCurrentSlot = FALSE;		//Get NEXT frame slot

	SaveData();
	CMSLogFileUtility::Instance()->WL_LogStatus("Load (Mgzn to Buffer) done\n");
	return TRUE;
}


LONG CWaferLoader::WL_UnloadFromBufferToMgz(BOOL bBurnInUnload, BOOL bBufferLevel, BOOL bIsAutoBond)
{
	CString szLog;
	szLog.Format("Unload (Buffer to Mgzn) start  %d  %d", bBurnInUnload, bBufferLevel);
	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);


	INT nPreStatus = CheckPreLoadUnloadStatus(FALSE);
	if (nPreStatus != TRUE)
		return nPreStatus;

	X_MoveTo(m_lReadyPos_X, FALSE);
	if (bBufferLevel == WL_BUFFER_UPPER)
		Z_MoveTo(m_lBufTableUSlot_Z);
	else
		Z_MoveTo(m_lBufTableLSlot_Z);
	X_Sync();


	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper Down");
	SetGripperLevel(TRUE);
	SetGripperState(WL_OFF);


CString szMsg;
szMsg.Format("LOAD: gripper to BUFFER UNLOAD at ENC = %ld ...", m_lBufferLoadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE*2);
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);
	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper move to BUFFER LOAD + UnloadOffset");
	X_MoveTo(m_lBufferLoadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE);

	Sleep(200);
	FilmFrameSearchOnTable();
	Sleep(200);

	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at Buffer UNLOAD");
		SetErrorMessage("Gripper is OFF @ at Buffer UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ at Buffer UNLOAD");
		SetErrorMessage("Gripper is Jam @ at Buffer UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}


	BOOL bFrameDetected = IsFrameDetect();
	if (bBurnInUnload)
	{
		bFrameDetected = TRUE;
	}
	else if (bFrameDetected != TRUE)
	{
		Sleep(100);
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)
		{
			Sleep(200);
			FilmFrameSearchOnTable();
			Sleep(200);
		}
	}


	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at Buffer UNLOAD 2");
		SetErrorMessage("Gripper is OFF @ at Buffer UNLOAD 2");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ at Buffer UNLOAD 2");
		SetErrorMessage("Gripper is Jam @ at Buffer UNLOAD 2");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		if (X_IsPowerOn())
		{
			CMS896AStn::MotionPowerOff(WL_AXIS_X, &m_stWLAxis_X);		//v3.89	//for safety purpose
		}
		return Err_FrameJammed;
	}

	if (bFrameDetected != TRUE && m_bNoSensorCheck == FALSE)
	{
		Sleep(100);
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)	
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No Frame is detected at Buffer UNLOAD");
			SetErrorMessage("No Frame is detected at Buffer UNLOAD");
			SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
			X_MoveTo(m_lReadyPos_X, TRUE);
			return Err_NoFrameDetected;
		}
	}


	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper MoveSearchJam to LOAD");
	//X_MoveTo(m_lLoadPos_X, FALSE);
	//X_Sync();
	if (WaferGripperMoveSearchJam(m_lLoadPos_X, TRUE) == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to unload from buffer to mgz");
		SetErrorMessage("Gripper is Jam @ to load");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		if (X_IsPowerOn())
		{
			CMS896AStn::MotionPowerOff(WL_AXIS_X, &m_stWLAxis_X);	//for safety purpose
		}
		return Err_FrameJammed;
	}


	Sleep(500);
	if (bIsAutoBond)
	{
		X_MoveTo(m_lLoadPos_X + 2000, TRUE);
	}
	else
	{
		X_MoveTo(m_lReadyPos_X, TRUE);
		SetGripperLevel(WL_OFF);
	}


	if (bBufferLevel == WL_BUFFER_UPPER)
	{
		m_szUpperSlotBarcode = _T("");
		m_bFrameInUpperSlot	= FALSE;
	}
	else
	{
		m_szLowerSlotBarcode = _T("");
		m_bFrameInLowerSlot	= FALSE;
	}

	SaveData();
	CMSLogFileUtility::Instance()->WL_LogStatus("Unload (Buffer to Mgzn) done\n");
	return TRUE;
}


LONG CWaferLoader::WL_LoadFromBufferToTable(BOOL bBurnInLoad, BOOL bSearchHomeDie, BOOL bBufferLevel, BOOL bIsAutoBond)
{
	CString szLog;
	szLog.Format("Unload (Buffer to Table) start  %d  %d", bBurnInLoad, bBufferLevel);
	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);


	INT nPreStatus = CheckPreLoadUnloadStatus();
	if (nPreStatus != TRUE)
		return nPreStatus;

	//Wafer Table to UNLOAD & gripper to READY at DOWN state
	if (FilmFrameGearReady(SFM_WAIT, TRUE, TRUE, TRUE)== FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL load fileframe gear ready fail");
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return FALSE;
	}


	if (bBufferLevel == WL_BUFFER_UPPER)
		Z_MoveTo(m_lBufTableUSlot_Z, SFM_NOWAIT);
	else
		Z_MoveTo(m_lBufTableLSlot_Z, SFM_NOWAIT);


	//Gear up expander to LOAD frame
	INT nExpStatus = 0;
	CMSLogFileUtility::Instance()->WL_LogStatus("Expander starts to gear UP...");

	if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||	
			(GetExpType() == WL_EXP_GEAR_DIRECTRING) )	
	{
		nExpStatus = ExpanderDCMotorPlatform(WL_UP, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
	}
	else if (GetExpType() == WL_EXP_CYLINDER)
	{
		nExpStatus = ExpanderCylinderMotorPlatform(WL_UP,TRUE, FALSE, FALSE);
	}
	else
	{
		nExpStatus = ExpanderGearPlatform(WL_UP, TRUE, FALSE, FALSE);		//v2.56
	}

	if ( nExpStatus == Err_ExpanderAlreadyOpen )
	{
		if (!bIsAutoBond)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
			SetErrorMessage("Expander already open");
			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderAlreadyOpen;
		}
	}
	else if ( nExpStatus != TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot open");
		SetErrorMessage("Expander cannot open");

		SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);	
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return Err_ExpanderOpenCloseFailed;
	}

	Z_Sync();
	CMSLogFileUtility::Instance()->WL_LogStatus("Expander at UP posn");

CString szMsg;
szMsg.Format("LOAD: gripper to BUFFER LOAD at ENC = %ld ...", m_lBufferLoadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE);
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper move to BUFFER LOAD + UnloadOffset");
	X_MoveTo(m_lBufferLoadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE);

	Sleep(200);
	FilmFrameSearchOnTable();
	Sleep(200);

	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at Buffer UNLOAD");
		SetErrorMessage("Gripper is OFF @ at Buffer UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}
	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ at Buffer UNLOAD");
		SetErrorMessage("Gripper is Jam @ at Buffer UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}


	BOOL bFrameDetected = IsFrameDetect();
	if (bBurnInLoad)
	{
		bFrameDetected = TRUE;
	}
	else if (bFrameDetected != TRUE)
	{
		Sleep(100);
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)
		{
			Sleep(200);
			FilmFrameSearchOnTable();
			Sleep(200);
		}
	}

	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at Buffer UNLOAD 2");
		SetErrorMessage("Gripper is OFF @ at Buffer UNLOAD 2");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}
	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ at Buffer UNLOAD 2");
		SetErrorMessage("Gripper is Jam @ at Buffer UNLOAD 2");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		if (X_IsPowerOn())
		{
			CMS896AStn::MotionPowerOff(WL_AXIS_X, &m_stWLAxis_X);		//v3.89	//for safety purpose
		}
		return Err_FrameJammed;
	}

	if (bFrameDetected != TRUE && m_bNoSensorCheck == FALSE)
	{
		Sleep(100);
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)	
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No Frame is detected at Buffer UNLOAD");
			SetErrorMessage("No Frame is detected at Buffer UNLOAD");
			SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
			X_MoveTo(m_lReadyPos_X, TRUE);
			return Err_NoFrameDetected;
		}
	}


	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is ON");
	SetGripperState(WL_ON);

	//Move Gripper to load pos
	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper to UNLOAD...");
	X_MoveTo(m_lUnloadPos_X);

	//Get more into expander
	if (GetExpType() == WL_EXP_VACUUM)
	{
		FilmFrameDriveIn(HP_POSITIVE_DIR, 30, 50, TRUE);
	}
	else if (GetExpType() != WL_EXP_GEAR_DIRECTRING)
	{
		FilmFrameDriveIn(HP_POSITIVE_DIR, 200, 50, TRUE);
	}


	BOOL bFrameExisted = TRUE;
	if (GetExpType() == WL_EXP_VACUUM)
	{
		bFrameExisted = (IsFramePosition() && IsFrameProtection());	//Check Frame Position sensor
	}
	else
	{
		bFrameExisted = TRUE;	//IsFrameExist();		//Check Frame Exist sensor
		CString szMsg;
		szMsg.Format("WL frame exist @ table = %d", bFrameExisted);		
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	}

	bFrameDetected = IsFrameDetect();
	if (bBurnInLoad == TRUE)
	{
		bFrameExisted	= TRUE;		//Frame detected on expander?
		bFrameDetected	= TRUE;		//Frame also detected on gripper?
	}


	//Off Clip
	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper OFF");
	Sleep(100);
	SetGripperState(WL_OFF);
	Sleep(100);
	X_MoveTo(0);	//-250);


	if (bBufferLevel == WL_BUFFER_UPPER)
	{
		m_szBarcodeName	= m_szUpperSlotBarcode;
		m_szUpperSlotBarcode = _T("");
		m_bFrameInUpperSlot	= FALSE;
	}
	else
	{
		m_szBarcodeName	= m_szLowerSlotBarcode;
		m_szLowerSlotBarcode = _T("");
		m_bFrameInLowerSlot	= FALSE;
	}

	if ((!bFrameDetected || !bFrameExisted) && m_bNoSensorCheck == FALSE)
	{
		CString szErr;
		szErr.Format("No Frame is detected on expander - %d %d",bFrameDetected, bFrameExisted);
		CMSLogFileUtility::Instance()->WL_LogStatus(szErr);
		SetErrorMessage("No Frame is detected on expander");
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
		return Err_NoFrameDetected;
	}


	//Expander gears DOWN
	CMSLogFileUtility::Instance()->WL_LogStatus("Expander starts to gear down...");
	if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) || (GetExpType() == WL_EXP_GEAR_DIRECTRING) )	
	{
		nExpStatus = ExpanderDCMotorPlatform(WL_DOWN, TRUE, FALSE, TRUE, m_lExpDCMotorDacValue, FALSE);
	}
	else if (GetExpType() == WL_EXP_CYLINDER)
	{
		nExpStatus = ExpanderCylinderMotorPlatform(WL_DOWN, TRUE, FALSE, TRUE);
	}
	else
	{
		nExpStatus = ExpanderGearPlatform(WL_DOWN, TRUE, FALSE, TRUE);
	}

	if (nExpStatus != 1)
	{
#ifndef NU_MOTION	//Klocwork	//v4.46
		if (nExpStatus == Err_NoFrameExist)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No Frame Exist");
			SetErrorMessage("No Frame Exist");
			SetAlert_Red_Yellow(IDS_WL_EXP_NO_FRMAE_EXIST);
			HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
			return Err_NoFrameExist;
		}
		else
#endif
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot close");
			SetErrorMessage("Expander cannot close");
			SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
			HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
			return Err_ExpanderOpenCloseFailed;
		}
	}


	CMSLogFileUtility::Instance()->WL_LogStatus("Expander at down posn");
	HouseKeeping(WL_ON, FALSE, TRUE, TRUE);

	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && (IsExpanderLock() == TRUE) )
	{
		Sleep(1000);
		if (IsExpanderLock())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander unlock failed at Load FilmFrame");
			SetErrorMessage("Expander unlock failed");

			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);
			return Err_ExpanderLockFailed;
		}
	}


	if (CMS896AApp::m_bMS100Plus9InchOption)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("9Inch table to HOME");
		MoveWaferTableLoadUnload(0, 0, FALSE);
	}


	//Exist if burn-in is enable
	if ( bBurnInLoad == TRUE )
	{
		WPR_MoveToFocusLevel();
		m_bFrameExistOnExp = TRUE;	
		CMSLogFileUtility::Instance()->WL_LogStatus("Load (Buffer to Table) done (BURNIN)\n");
		return TRUE;
	}

	//Check Frame is exit on expander or not
	if (GetExpType() == WL_EXP_VACUUM)
	{
		bFrameExisted = IsExpanderVacuum();	
		m_bFrameExistOnExp = bFrameExisted;
	}
	else
	{
		bFrameExisted = TRUE;	//IsFrameExist();	
		m_bFrameExistOnExp = bFrameExisted;		
	}


	BOOL bKeepBarcodePos = FALSE;

	SaveData();
	/****/ CMSLogFileUtility::Instance()->WL_LogStatus("Load (Buffer to Table) done\n"); /****/
	
	
	//rotate WT before load map
	CMSLogFileUtility::Instance()->WL_LogStatus("WL Auto Rotate Wafer");
	AutoRotateWafer(bKeepBarcodePos);

	(*m_psmfSRam)["WaferTable"]["WT1LoadMgzn"]	= m_lCurrentMagNo;
	(*m_psmfSRam)["WaferTable"]["WT1LoadSlot"]	= m_lCurrentSlotNo;
	(*m_psmfSRam)["WaferTable"]["WT1InBarcode"]	= m_szBarcodeName;

	CMSLogFileUtility::Instance()->WL_LogStatus("WL Auto Align Wafer start");
	
	m_szLoadAlignBarcodeName = m_szBarcodeName;
	return AlignFrameWafer(bFrameDetected, bFrameExisted, bSearchHomeDie);
	
	return TRUE;
}


LONG CWaferLoader::WL_UnloadFromTableToBuffer(BOOL bBurnInUnload, BOOL bBufferLevel, BOOL bIsAutoBond)
{
	CString szLog;
	szLog.Format("Unload (Table to Buffer) start  %d  %d", bBurnInUnload, bBufferLevel);
	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);


	INT nPreStatus = CheckPreLoadUnloadStatus();
	if (nPreStatus != TRUE)
		return nPreStatus;

	//Wafer Table to UNLOAD & gripper to READY at DOWN state
	if (FilmFrameGearReady(SFM_WAIT, TRUE, TRUE, FALSE)== FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL load fileframe gear ready fail");
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return FALSE;
	}

	if (bBufferLevel == WL_BUFFER_UPPER)
		Z_MoveTo(m_lBufTableUSlot_Z, SFM_NOWAIT);
	else
		Z_MoveTo(m_lBufTableLSlot_Z, SFM_NOWAIT);

	//Gear up expander to LOAD frame
	INT nExpStatus = 0;
	CMSLogFileUtility::Instance()->WL_LogStatus("Expander starts to gear UP...");

	if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||	
			(GetExpType() == WL_EXP_GEAR_DIRECTRING) )	
	{
		nExpStatus = ExpanderDCMotorPlatform(WL_UP, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
	}
	else if (GetExpType() == WL_EXP_CYLINDER)
	{
		nExpStatus = ExpanderCylinderMotorPlatform(WL_UP,TRUE, FALSE, FALSE);
	}
	else
	{
		nExpStatus = ExpanderGearPlatform(WL_UP, TRUE, FALSE, FALSE);		//v2.56
	}

	if ( nExpStatus == Err_ExpanderAlreadyOpen )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander already open");
		SetErrorMessage("Expander already open");
		SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return Err_ExpanderAlreadyOpen;
	}
	else if ( nExpStatus != TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot open");
		SetErrorMessage("Expander cannot open");

		SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);	
		HouseKeeping(WL_OFF, FALSE, FALSE, FALSE);
		return Err_ExpanderOpenCloseFailed;
	}

	Z_Sync();
	CMSLogFileUtility::Instance()->WL_LogStatus("Expander at UP posn");


CString szMsg;
szMsg.Format("LOAD: gripper to UNLOAD at ENC = %ld ...", m_lBufferLoadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE);
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper move to UNLOAD + UnloadOffset");
	X_MoveTo(m_lUnloadPos_X + WL_GRIPPER_SEARCH_DIST_ON_TABLE);

	Sleep(200);
	FilmFrameSearchOnTable();
	Sleep(200);

	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at Buffer UNLOAD");
		SetErrorMessage("Gripper is OFF @ at UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}
	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ at Buffer UNLOAD");
		SetErrorMessage("Gripper is Jam @ at UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}


	BOOL bFrameDetected = IsFrameDetect();
	if (bBurnInUnload)
	{
		bFrameDetected = TRUE;
	}
	else if (bFrameDetected != TRUE)
	{
		Sleep(100);
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)
		{
			Sleep(200);
			FilmFrameSearchOnTable();
			Sleep(200);
		}
	}

	if (!IsAllMotorsEnable())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is OFF @ at Buffer UNLOAD 2");
		SetErrorMessage("Gripper is OFF @ at Buffer UNLOAD 2");
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}
	if (IsFrameJam() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ at Buffer UNLOAD 2");
		SetErrorMessage("Gripper is Jam @ at Buffer UNLOAD 2");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		if (X_IsPowerOn())
		{
			CMS896AStn::MotionPowerOff(WL_AXIS_X, &m_stWLAxis_X);		//v3.89	//for safety purpose
		}
		return Err_FrameJammed;
	}

	if (bFrameDetected != TRUE && m_bNoSensorCheck == FALSE)
	{
		Sleep(100);
		bFrameDetected = IsFrameDetect();
		if (bFrameDetected != TRUE)	
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No Frame is detected at Buffer UNLOAD");
			SetErrorMessage("No Frame is detected at Buffer UNLOAD");
			SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
			HouseKeeping(WL_OFF, TRUE, FALSE, TRUE);
			return Err_NoFrameDetected;
		}
	}

	if (m_lStepOnUnloadPos != 0)
	{
		X_Move(m_lStepOnUnloadPos);
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is ON");
	SetGripperState(WL_ON);
	Sleep(200);
	m_bFrameExistOnExp = FALSE;


	if (WaferGripperMoveSearchJam(m_lBufferLoadPos_X, FALSE, 0) == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ to Buffer load");
		SetErrorMessage("Gripper is Jam @ to Buffer load");
		ResetGripperHitLimit();
		SetGripperState(WL_OFF);
		SetGripperPower(FALSE);		
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		return Err_FrameJammed;
	}

	SetGripperState(WL_OFF);
	Sleep(200);

	if (!IsAllMotorsEnable())
	{
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		return Err_FrameJammed;
	}


	//Move Gripper to load pos
	/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Gripper back to ready"); /*******/	//v2.93T1
	X_MoveTo(m_lReadyPos_X, SFM_NOWAIT);

	if (bBufferLevel == WL_BUFFER_UPPER)
	{
		m_szUpperSlotBarcode	= m_szBarcodeName;
		m_szBarcodeName			= _T("");
		m_bFrameInUpperSlot		= TRUE;
	}
	else
	{
		m_szLowerSlotBarcode	= m_szBarcodeName;
		m_szBarcodeName			= _T("");
		m_bFrameInLowerSlot		= TRUE;
	}

	//DOwn Expander
	nExpStatus = 1;
	if (!bIsAutoBond)
	{
		if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) || (GetExpType() == WL_EXP_GEAR_DIRECTRING) )	
		{
			nExpStatus = ExpanderDCMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
		}
		else if (GetExpType() == WL_EXP_CYLINDER)
		{
			nExpStatus = ExpanderCylinderMotorPlatform(WL_DOWN, TRUE, FALSE, FALSE);
		}
		else
		{
			nExpStatus = ExpanderGearPlatform(WL_DOWN, TRUE, FALSE, FALSE);
		}

		SetGripperLevel(WL_OFF);	//Up gripper level
	}

	X_Sync();
	SaveData();

	//Log into History file
	if (CMS896AStn::m_bForceClearMapAfterWaferEnd == FALSE)
	{
		LogWaferEndInformation(m_lCurrentSlotNo, m_szBarcodeName);
	}

	if (!IsBufferFrameProtectSafe() && m_bNoSensorCheck == FALSE)
	{
		if (bBufferLevel == WL_BUFFER_UPPER)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame hit FrameProtect sensor at upper Buffer Slot\n");
			SetErrorMessage("Frame hit FrameProtect sensor at upper Buffer Slot");
		}
		else
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame hit FrameProtect sensor at lower Buffer Slot\n");
			SetErrorMessage("Frame hit FrameProtect sensor at lower Buffer Slot");
		}

		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
		return Err_FrameShifted;
	}

	if (nExpStatus != 1)
	{
		/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot close"); /*******/
		SetErrorMessage("Expander cannot close");
		SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);	
		//HouseKeeping(WL_OFF, FALSE, FALSE, TRUE);
		return Err_ExpanderOpenCloseFailed;
	}

	/****/ CMSLogFileUtility::Instance()->WL_LogStatus("Unload (Table to Buffer) done\n"); /****/
	return TRUE;
}




