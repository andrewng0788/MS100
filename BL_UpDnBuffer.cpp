
/////////////////////////////////////////////////////////////////
// BL_UDBuffer.cpp : Main functions of the CBinLoader class
//
//	Description:
//		MS899 Mapping Die Sorter
//
//	Date:		Friday, Sept 29, 2017
//	Revision:	1.00
//
//	By:			Andrew (PLLM)
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004-2017
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MS896A_Constant.h"
#include "BinLoader.h"
#include "MS_SecCommConstant.h"	
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CBinLoader::GetPreUnloadPosn(CBinGripperBuffer *pGripperBuffer)
{
	return pGripperBuffer->GetPreUnloadPos_X();
}

LONG CBinLoader::GetReadyPosn(CBinGripperBuffer *pGripperBuffer)
{
	return pGripperBuffer->GetReadyPos_X();
}

LONG CBinLoader::GetLoadMagPosn(CBinGripperBuffer *pGripperBuffer)
{
	return pGripperBuffer->GetLoadMagPos_X();
}

LONG CBinLoader::GetUnloadMagPosn(CBinGripperBuffer *pGripperBuffer)
{
	return pGripperBuffer->GetUnloadMagPos_X();
}

LONG CBinLoader::GetBarcodePosn(CBinGripperBuffer *pGripperBuffer)
{
	return pGripperBuffer->GetBarcodePos_X();
}

LONG CBinLoader::GetUnloadPosn(CBinGripperBuffer *pGripperBuffer)
{
	return pGripperBuffer->GetUnloadPos_X();
}


INT CBinLoader::SetGripperPower(CBinGripperBuffer *pGripperBuffer, BOOL bOn)
{
	INT lRet = 0;
	if (pGripperBuffer->IsUpperBuffer())
	{
		lRet = SetLoaderXPower(bOn);
	}
	else
	{
		lRet = SetLoaderX2Power(bOn);
	}

	return lRet;
}

BOOL CBinLoader::IsGripperFrameExist(const CString szBL_SI_DualBufferExistName)
{
	// check frame on Upper/Lower Buffer table physically
	BOOL bFrameExist = IsDualBufferExist(szBL_SI_DualBufferExistName);
	if (!bFrameExist)
	{
		Sleep(200);
		bFrameExist = IsDualBufferExist(szBL_SI_DualBufferExistName);
	}
	return bFrameExist;
}

VOID CBinLoader::GripperMotionStop(CBinGripperBuffer *pGripperBuffer)
{
	if (pGripperBuffer->IsUpperBuffer())
	{
		CMS896AStn::MotionStop(BL_AXIS_UPPER, &m_stBLAxis_Upper);
	}
	else
	{
		CMS896AStn::MotionStop(BL_AXIS_LOWER, &m_stBLAxis_Lower);
	}
}

VOID CBinLoader::GripperHouseKeeping(CBinGripperBuffer *pGripperBuffer,
									 BOOL bEnableBT, BOOL bIsMissingStep, BOOL bByPassOutOfMagCheck)
{
	LONG lReadyPos_X = GetReadyPosn(pGripperBuffer);
	if (pGripperBuffer->IsUpperBuffer())
	{
		HouseKeeping(lReadyPos_X, bEnableBT, bIsMissingStep, bByPassOutOfMagCheck);
	}
	else
	{
		HouseKeeping2(lReadyPos_X, bEnableBT, bIsMissingStep, bByPassOutOfMagCheck);
	}
}


LONG CBinLoader::GripperMoveSearchJam(CBinGripperBuffer *pGripperBuffer,
									  INT nPos, BOOL bCheckReAlign, BOOL bReadBarCode, BOOL bUnload)
{
	LONG lStatus = 0;
	if (pGripperBuffer->IsUpperBuffer())
	{
		lStatus = BinGripperMoveSearchJam(nPos, bCheckReAlign, bReadBarCode, bUnload);
	}
	else
	{
		lStatus = BinGripper2MoveSearchJam(nPos, bCheckReAlign, bReadBarCode, bUnload);
	}

	return lStatus;
}

LONG CBinLoader::GripperSearchInClamp(CBinGripperBuffer *pGripperBuffer,
									  INT siDirection, LONG lSearchDistance, LONG lDriveInDistance,
									  BOOL bWaitComplete, BOOL bReadBarcode, BOOL bSync)//2019.04.24 Matt:hardcode drivein in 400
{
	LONG lStatus = 0;
	if (pGripperBuffer->IsUpperBuffer())
	{
		lStatus = BinGripperSearchInClamp(siDirection, lSearchDistance, lDriveInDistance, bWaitComplete, bReadBarcode, bSync);
	}
	else
	{
		lStatus = BinGripper2SearchInClamp(siDirection, lSearchDistance, lDriveInDistance, bWaitComplete, bReadBarcode, bSync);
	}
	return lStatus;
}

LONG CBinLoader::GripperSearchScan(CBinGripperBuffer *pGripperBuffer,
								   INT siDirection, LONG lSearchDistance)
{
	LONG lStatus = 0;
	if (pGripperBuffer->IsUpperBuffer())
	{
		lStatus = BinGripperSearchScan(siDirection, lSearchDistance);
	}
	else
	{
		lStatus = BinGripper2SearchScan(siDirection, lSearchDistance);
	}

	return lStatus;
}


LONG CBinLoader::GripperMoveTo(CBinGripperBuffer *pGripperBuffer, INT nPos, BOOL bReadBarCode)
{
	LONG lStatus = 0;
	if (pGripperBuffer->IsUpperBuffer())
	{
		lStatus = BinGripperMoveTo(nPos, bReadBarCode);
	}
	else
	{
		lStatus = BinGripper2MoveTo(nPos, bReadBarCode);
	}

	return lStatus;
}


LONG CBinLoader::GripperMoveToReadyPosn(CBinGripperBuffer *pGripperBuffer, BOOL bWait)
{
	LONG lStatus = 0;
	LONG lReadyPos_X = GetReadyPosn(pGripperBuffer);
	if (pGripperBuffer->IsUpperBuffer())
	{
		lStatus = BinGripperMoveTo_Auto(lReadyPos_X, SFM_NOWAIT);
	}
	else
	{
		lStatus = BinGripper2MoveTo_Auto(lReadyPos_X, SFM_NOWAIT);
	}

	return lStatus;
}


LONG CBinLoader::GripperMoveToPreUnloadPosn(CBinGripperBuffer *pGripperBuffer, BOOL bWait)
{
	LONG lStatus = 0;
	LONG lPreUnloadPos_X = GetPreUnloadPosn(pGripperBuffer);
	if (pGripperBuffer->IsUpperBuffer())
	{
		lStatus = BinGripperMoveTo_Auto(lPreUnloadPos_X, SFM_NOWAIT);
	}
	else
	{
		lStatus = BinGripper2MoveTo_Auto(lPreUnloadPos_X, SFM_NOWAIT);
	}

	return lStatus;
}


VOID CBinLoader::HomeGripper(CBinGripperBuffer *pGripperBuffer)
{
	if (pGripperBuffer->IsUpperBuffer())
	{
		Upper_Home();
	}
	else
	{
		Lower_Home();
	}
}

BOOL CBinLoader::GripperSyncMove_Auto(CBinGripperBuffer *pGripperBuffer)
{
	if (pGripperBuffer->IsUpperBuffer())
	{
		return BinGripperSynMove_Auto();
	}
	return BinGripper2SynMove_Auto();
}


LONG CBinLoader::GripperSync(CBinGripperBuffer *pGripperBuffer)
{
	if (pGripperBuffer->IsUpperBuffer())
	{
		return Upper_Sync();
	}
	return Lower_Sync();
}


LONG CBinLoader::UDB_DownElevatorToReady(CBinGripperBuffer *pGripperBuffer, BOOL bWait)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	if (!CheckSafeToMove())
	{
		return FALSE;
	}

	if (m_clLowerGripperBuffer.IsTriggerThetaHome() || m_clUpperGripperBuffer.IsTriggerThetaHome())
	{
		m_clLowerGripperBuffer.SetTriggerThetaHome(FALSE);
		m_clUpperGripperBuffer.SetTriggerThetaHome(FALSE);
		Z_SMoveTo(0, SFM_WAIT);
		Z_Home();
		Theta_Home();
	}

	BL_DEBUGBOX("Down Elevator to ready");

	INT nPosT = m_lReadyPos_T;
	INT nLevelZ = m_lReadyLvlZ;

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		if (!pGripperBuffer->IsUpperBuffer())
		{
			nLevelZ = nLevelZ + m_lUpperToLowerBufferOffsetZ;
		}
	}
	return MoveElevatorToWithCheckMissingStep("UDB_DownElevatorToReady", nPosT, nLevelZ, bWait);
}

LONG CBinLoader::MoveElevatorToWithCheckMissingStep(const CString szTitle, const LONG nPosT, const LONG nLevelZ, const BOOL bWait, const BOOL bDisableOutOfMgznCheck)
{
	LONG lRet = MoveElevatorTo(szTitle, nPosT, nLevelZ, bWait, bDisableOutOfMgznCheck);

	if (lRet == Err_BinLoaderThetaMissingStep)
	{
		Theta_Home();
		lRet = MoveElevatorTo(szTitle, nPosT, nLevelZ, TRUE, bDisableOutOfMgznCheck);
		if ((lRet == Err_BinLoaderThetaMissingStep) || (lRet == FALSE))
		{
			if (lRet == Err_BinLoaderThetaMissingStep)
			{
				SetAlert_Red_Yellow(IDS_BL_MOTOR_THETA_MISSING_STEP);
			}
			lRet = FALSE;
		}
	}

	return lRet;
}

LONG CBinLoader::MoveElevatorTo(const CString szTitle, const LONG nPosT, const LONG nLevelZ, const BOOL bWait, const BOOL bDisableOutOfMgznCheck)
{
	CString szMess;
	INT nResultT	= Theta_MoveTo(nPosT, SFM_NOWAIT);
	INT nResultZ	= Z_SMoveTo(nLevelZ, SFM_NOWAIT);

	if ((nResultT != gnOK) || (nResultZ != gnOK))
	{
		szMess.Format("%s motion error", (const char*)szTitle);
		BL_DEBUGBOX(szMess);
		return FALSE;
	}

	INT nCount = 0;

	if (bWait)
	{
		while (1)
		{
			if ((CMS896AStn::MotionIsComplete(BL_AXIS_Z, &m_stBLAxis_Z) == TRUE) && 
				(CMS896AStn::MotionIsComplete(BL_AXIS_THETA, &m_stBLAxis_Theta) == TRUE))
			{
				if (Z_Sync() == gnNOTOK)
				{
					return FALSE;
				}
				if (Theta_Sync() == gnNOTOK)
				{
					return FALSE;
				}
				break;
			}

			if (!bDisableOutOfMgznCheck && !CheckCoverOpen())
			{
				return FALSE;
			}

			nCount++;
			if (nCount > 10000)
			{
				szMess.Format("BL: %s timeout !!", (const char*)szTitle);
				BL_DEBUGBOX(szMess);
				nCount = 0;
			}

			Sleep(1);
		}		

		Sleep(100);
		GetEncoderValue();
		szMess.Format("%s = Theta(cmd = %d, Enc = %d), Z(cmd = %d, Enc = %d)", (const char*)szTitle, nPosT, m_lEnc_T, nLevelZ, m_lEnc_Z);
		SetStatusMessage(szMess);
		SetErrorMessage(szMess);
		//=======================================================================================
		//if the Missing Step of Binloader theta is more than 10 count, need do home.
		if (abs(nPosT - m_lEnc_T) > m_lBIN_THETA_MISSING_STEP_TOL)
		{
			return Err_BinLoaderThetaMissingStep;
		}
		//=====================================================================================
	}
	szMess.Format("%s completed, bWait = %d", (const char*)szTitle, bWait);
	BL_DEBUGBOX(szMess);
	return TRUE;
}



BOOL CBinLoader::BinLoaderUnloadJamMessage_UDB(BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer)
{
	LONG lReturn = 0;
	LONG lUnloadJamRetryCount = 0;
	LONG lJamRetryLimit = BL_JAM_UNLOAD_RETRY_LIMIT;

	if( bBurnInUnload == TRUE || m_bDisableBL == TRUE)
	{
		return TRUE;
	}

	SetGripperPower(pGripperBuffer, FALSE);

	while(1)
	{
		if (lUnloadJamRetryCount > lJamRetryLimit)
		{
			return FALSE;
		}

		lUnloadJamRetryCount++;

		//lReturn = BL_OK_CANCEL("Frame is jammed!\nPush it into the magazine slot and Continue?", "Unload Frame Message");
		lReturn = 0;
		SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_BUFFER_TO_TABLE);

		if (lReturn == FALSE)
		{
			if (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()) == TRUE)
			{
				SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_BUFFER_TO_TABLE);
				/*SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);

				CString szLog;
				szLog.Format("BL %s Gripper Jam quit", (const char*)pGripperBuffer->GetName());
				SetErrorMessage(szLog);*/
			}

			return FALSE;
		}
			
		if (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()) == TRUE)
		{		
			SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_BUFFER_TO_TABLE);
			/*SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
			CString szLog;
			szLog.Format("BL %s Gripper Jam quit", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szLog);*/
			continue;
		}

		if (lReturn == FALSE)
		{
			// Return to allow safe stoppage of the machine
			return FALSE;
		}
		else
		{
			if (!m_bNoSensorCheck && IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))	
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
				CString szLog;
				szLog.Format("BL frame is detected in %s gripper", (const char*)pGripperBuffer->GetName());
				SetErrorMessage(szLog);
				continue;
			}

			// Return to allow sequence to continue
			SetGripperPower(pGripperBuffer, TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

VOID CBinLoader::BTFramePlatformUp()
{
	if (!IsFrameLevelOn())
	{
		SetFrameLevel(TRUE);	// Up bin table first if necessary
		Sleep(1000);
	}
}

VOID CBinLoader::BTFramePlatformDown()
{
	SetFrameLevel(FALSE);	// down bin table first if necessary
	Sleep(500);
}

BOOL CBinLoader::IsBTFramePlatformDown()
{
	if (!IsFrameLevel())	//TRUE = at DOWN level, FALSE = at UP level
	{
		BTFramePlatformDown();
			
		if (!IsFrameLevel())
		{
			Sleep(500);
			if (!IsFrameLevel())
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

LONG CBinLoader::UDB_SubUnloadBinFrame(BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer, BOOL bBinFull, BOOL bOffline, 
									   BOOL bClearBin, BOOL bCheckRealignRpy, BOOL bTransferWIPSlot, BOOL bClearAllFrameTOP2Mode)
{
	LONG lStatus		= TRUE;

	LONG lBufferBlock	= pGripperBuffer->GetBufferBlock();
	BL_DEBUGBOX("Sub Unload Bin Frame");
	if (lBufferBlock > 0)
	{
		lStatus = UDB_UnloadFromBufferToMgz(m_bBurnInTestRun, FALSE, pGripperBuffer, bClearBin, bCheckRealignRpy, bTransferWIPSlot, bClearAllFrameTOP2Mode);
		Sleep(200);
	}

	if (lStatus == FALSE)
	{
		SetErrorMessage("BL ManualUnloadFilmFrame (UDB): UNLOAD buffer fails");
		return FALSE;
	}

	//Set Magzine&Slot Number
	m_lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
	m_lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
	pGripperBuffer->SetMgznSlotNo(m_lCurrMgzn, m_lCurrSlot);
	pGripperBuffer->SetBinFull(bBinFull);

	lStatus = UDB_UnloadFromTableToBuffer(m_bBurnInTestRun, bBinFull, pGripperBuffer, bOffline);
	if (lStatus == FALSE)
	{
		SetErrorMessage("BL ManualUnloadFilmFrame (UDB): UNLOAD from table to Buffer fails");
		return FALSE;
	}
	
	Sleep(100);

	lStatus = UDB_UnloadFromBufferToMgz(m_bBurnInTestRun, bBinFull, pGripperBuffer, bClearBin, bCheckRealignRpy, bTransferWIPSlot, bClearAllFrameTOP2Mode);
	if (lStatus == FALSE)
	{
		SetErrorMessage("BL ManualUnloadFilmFrame (UDB): UNLOAD from Buffer to Mgzn fails");
		return FALSE;
	}
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////
//	UDB_UnloadFromTableToBuffer()
///////////////////////////////////////////////////////////////////////////////////
LONG CBinLoader::UDB_UnloadFromTableToBuffer(BOOL bBurnInUnload, BOOL bBinFull, CBinGripperBuffer *pGripperBuffer, BOOL bOffline)
{
	LONG lFrameOnBT = BL_FRAME_ON_CENTER;
	LONG lReturn = 0;
	
	CString szMsg	= "";
	CString szText1 = "";
/*
	//Case#1 , trigger "output slot full"
	if (bBinFull && IsMSAutoLineMode())
	{
		//if WIP slot is sorting and there has not output empty slot, so ask SIS to take out the frame from output slots
		if (IsFullOutputSlot_AutoLine())
		{
			return Err_BinMagzineOutputWIPFull;
		}
	}
*/
	m_lCurrMgzn = pGripperBuffer->GetMgznNo();
	m_lCurrSlot = pGripperBuffer->GetSlotNo();
	szMsg.Format("UDB - UNLOAD from Table to Buffer start lCurrMgzn = %d, lCurrSlot = %d, BTCurrentBlock = %d", m_lCurrMgzn, m_lCurrSlot, m_lBTCurrentBlock);
    BL_DEBUGBOX(szMsg);

    // check frame on bin table logically
	if (!(IsAutoLoadUnloadQCTest() && ((m_lCurrMgzn == BL_MGZ_TOP_1) || (m_lCurrMgzn == BL_MGZ_TOP_2) || 
		 (IsMSAutoLineStandloneMode() && (m_lCurrMgzn == BL_MGZ_MID_2)))) && 
		(m_lBTCurrentBlock == 0))
    {
		SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_FRAME);
		SetErrorMessage("BL Current logical block is zero");
		return FALSE;
    }

	pGripperBuffer->SetUnloadDieGradeCount(GetNoOfSortedDie((UCHAR)m_lBTCurrentBlock));

    // check frame on bin table physically
    lFrameOnBT = CheckFrameOnBinTable();
	if( (bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
    {
		lFrameOnBT = BL_FRAME_ON_CENTER;
    }

	switch( lFrameOnBT )
	{
    case BL_FRAME_NOT_EXIST: 
		SetFrameVacuum(FALSE);
//		SetFrameAlign(FALSE);
		if ( bOffline == TRUE )
		{
			SetErrorMessage("UDB_UnloadFromTableToBuffer: FRAME not detected on table in UNLOAD");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_ON_TABLE);
			return FALSE;

//			if( BL_YES_NO(HMB_BL_PUT_FRAME_INTO_BT, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
//			{
//				if( BL_YES_NO(HMB_BL_UNLOAD_MANUALLY, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
//				{
//					return FALSE;
//				}
//				else
//				{
//					SetAlert_Red_Yellow(IDS_BL_MAN_UNLOAD);
//					SetErrorMessage("BL manual unload this frame");
//					return TRUE;
//				}
//			}
//			else	//continue to UNLOAD to Buffer ....
//			{
////				SetFrameAlign(TRUE);
////				Sleep(m_lBTAlignFrameDelay);
//				SetFrameVacuum(TRUE);
//			}
		}
		else
		{
			SetErrorMessage("UDB_UnloadFromTableToBuffer: FRAME not detected on table in UNLOAD");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_ON_TABLE);
			return FALSE;
		}
        break;

    case BL_FRAME_ON_CENTER: 
        break;
    }

	//Also check in burn in cycle for safety purpose
	if (!(bBurnInUnload || m_bNoSensorCheck || m_bDisableBL) &&
		(IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()) || IsGripperFrameExist(pGripperBuffer->GetFrameExistSensorName())))
	{
		szMsg.Format("BL Frame exists in %s Buffer", (const char*)pGripperBuffer->GetName());
		SetErrorMessage(szMsg);
		SetAlert_Red_Yellow(pGripperBuffer->IsUpperBuffer() ? IDS_BL_FRMAE_EXIST_IN_U_BUFFER : IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
		return FALSE;
	}

	if (!IsBTFramePlatformDown())
	{
		szMsg = _T("Bin Table Frame level at UP level (UNLOAD)");
		SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);		
		SetErrorMessage("UDB_LoadFromBufferToTable: check BT Frame Level fail at UNLOAD");
		return FALSE;
	}

	if (BinGripperSynMove_Auto() == FALSE)
	{
		SetErrorMessage("UDB_UnloadFromTableToBuffer: SYNC Upper gripper error");
		return FALSE;
	}

	if (BinGripper2SynMove_Auto() == FALSE)
	{
		SetErrorMessage("UDB_UnloadFromTableToBuffer: SYNC Lower gripper error");
		return FALSE;
	}

	///////////////////////////////////////////////////
	// Move Elevator and BT to UNLOAD positions
	///////////////////////////////////////////////////
	if (!UDB_DownElevatorToReady(pGripperBuffer, FALSE))
	{
		return FALSE;
	}


//=============================================================================================
//    Platform Up&Move table at the same time
//=============================================================================================
	//Move the theta of Bin Table to ZERO
	if (!MoveBinTableThetaToZero())
	{
		SetErrorMessage("UDB_UnloadFromTableToBuffer: BT Theta to UNLOAD error");
		return FALSE;
	}
	LONG lStartTime = GetCurTime();
	BOOL bWaitFramePlateformUp = FALSE;
	SetFrameVacuum(FALSE);
	if (!IsFrameLevelOn())
	{
		SetFrameLevel(TRUE);	// Up bin table first if necessary
		bWaitFramePlateformUp = TRUE;
	}
//=============================================================================================

	if (MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y) == FALSE)
	{
		SetErrorMessage("UDB_UnloadFromTableToBuffer: BT to UNLOAD error");
		return FALSE;
	}

//=============================================================================================
	if (bWaitFramePlateformUp)
	{
		LONG lCurTime = GetCurTime();
		if ((lCurTime - lStartTime) < 1000)
		{
			Sleep(1000 - (lCurTime - lStartTime));
		}
	}
//=============================================================================================
	BL_DEBUGBOX("SettleDown for UNLOAD");
/*
	SetFrameVacuum(FALSE);
//	Sleep(500);
	BTFramePlatformUp();
*/
	if (Z_Sync() == gnNOTOK)
	{
		return FALSE;
	}
	if (Theta_Sync() == gnNOTOK)
	{
		return FALSE;
	}
	Sleep(100);
	//Log Encoder
	GetEncoderValue();
	szMsg.Format("%s = Theta(cmd = %d, Enc = %d), Z(cmd = %d, Enc = %d)", "UDB_DownElevatorToReady", m_lReadyPos_T, m_lEnc_T, m_lReadyLvlZ, m_lEnc_Z);
	SetStatusMessage(szMsg);

	//=======================================================================================
	//if the Missing Step of Binloader theta is more than 10 count, need do home.
	if (abs(m_lReadyPos_T - m_lEnc_T) > m_lBIN_THETA_MISSING_STEP_TOL)
	{
		Theta_Home();
		if (!UDB_DownElevatorToReady(pGripperBuffer, TRUE))
		{
			return FALSE;
		}
	}
	//=====================================================================================

	//Search Frame and Clamp it ========================================================
	LONG lOffsetX = 0;
	if ((m_lUnloadOffsetX < 500) && (m_lUnloadOffsetX > -500))
	{
		lOffsetX = m_lUnloadOffsetX;
	}

	LONG lUnloadPos_X = GetUnloadPosn(pGripperBuffer);
	szMsg.Format("%s Gripper to UNLOAD %ld + %ld", (const char*)pGripperBuffer->GetName(), lUnloadPos_X, lOffsetX);
	BL_DEBUGBOX(szMsg);

	if (!GripperMoveSearchJam(pGripperBuffer, lUnloadPos_X + lOffsetX))
	{
		SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_TABLE_TO_BUFFER);
		/*SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
		szMsg.Format("BL %s gripper jam", (const char*)pGripperBuffer->GetName());
		SetErrorMessage(szMsg);*/
		SetGripperState(pGripperBuffer, FALSE);
		Sleep(100);
		HomeGripper(pGripperBuffer);
		return FALSE;
	}

	Sleep(100);
	if (IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))
	{
		lReturn = TRUE;
	}
	else if (lOffsetX < 0)
	{
		lReturn = GripperSearchInClamp(pGripperBuffer, HP_POSITIVE_DIR, 2 * lOffsetX, 400);
	}
	else if (lOffsetX == 0)
	{
		lReturn = GripperSearchInClamp(pGripperBuffer, HP_POSITIVE_DIR, -500, 400);
	}

	if (!lReturn)
	{
		if (!bBurnInUnload && !m_bDisableBL)
		{
			if (1/*BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE*/)
			{
				/*szMsg.Format("BL No Frame is detected in %s gripper", (const char*)pGripperBuffer->GetName());
				SetErrorMessage(szMsg);
				SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);*/
				SetFrameNotDetectedAlarm(pGripperBuffer, IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_UNLOAD);
				LONG lReadyPosn = GetReadyPosn(pGripperBuffer);
				GripperMoveTo(pGripperBuffer, lReadyPosn);
				GripperHouseKeeping(pGripperBuffer);
				return FALSE;
			}
			else
			{
				if (!IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))
				{
					/*szMsg.Format("BL No Frame is detected in %s gripper", (const char*)pGripperBuffer->GetName());
					SetErrorMessage("szMsg");
					SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);*/
					SetFrameNotDetectedAlarm(pGripperBuffer, IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_UNLOAD);
					LONG lReadyPosn = GetReadyPosn(pGripperBuffer);
					GripperMoveTo(pGripperBuffer, lReadyPosn);
					GripperHouseKeeping(pGripperBuffer);
					return FALSE;
				}
			}
		}
	}

	Sleep(100);
	SetGripperState(pGripperBuffer, TRUE);
	Sleep(800);
	//PULL out the frame a little bit to detach from back-magnet
	//this function was removed because the gripper is clamp designer
//	GripperSearchScan(pGripperBuffer, HP_NEGATIVE_DIR, -500);

	//======================================================================================

	///////////////////////////////////////////////////
	// Scanning Barcode
	///////////////////////////////////////////////////
	if (m_bUseBarcode)
	{
		BL_DEBUGBOX("Scann barcode ....");
		//Read Barcode when Load frame
		LONG nErr = UDB_ReadBarCodeAtUnload(bBinFull, bBurnInUnload, pGripperBuffer);

		if (nErr != TRUE)
		{
			if (nErr == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_READ_BC_FAILURE);
			}
			szMsg.Format("UDB: %s Gripper ReadBarcode failure in Unload", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szMsg);
			SetGripperState(pGripperBuffer, FALSE);
			HomeGripper(pGripperBuffer);
			return FALSE;
		}
	}

	///////////////////////////////////////////////////
	// Gripper is gripping BT frame at this moment ...
	///////////////////////////////////////////////////
	szMsg.Format("%s Gripper MOVE to Ready Position", (const char*)pGripperBuffer->GetName());
	BL_DEBUGBOX(szMsg);
	LONG lReadyPos_X = GetReadyPosn(pGripperBuffer);
	if (!GripperMoveSearchJam(pGripperBuffer, lReadyPos_X))
	{
		SetGripperState(pGripperBuffer, FALSE);

		//push frame to bin table again========================================
		LONG lUnloadPos_X = GetUnloadPosn(pGripperBuffer);
		lReturn = GripperMoveSearchJam(pGripperBuffer, lUnloadPos_X);
		HomeGripper(pGripperBuffer);
		
		//=====================================================================

		if (!bBurnInUnload && !m_bDisableBL && 
			1/*BL_OK_CANCEL(HMB_BL_PUT_JAM_FRAME_TO_BUFFER, IDS_BL_UNLOAD_FRAME)==FALSE */)
		{
			szMsg.Format("BL %s gripper is jammed", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szMsg);
			SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_TABLE_TO_BUFFER);
			//HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
		else
		{
			if (IsBufferSafetySensor() || IsFrameJam(pGripperBuffer->GetFrameJamSensorName()) == TRUE)
			{
				SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_TABLE_TO_BUFFER);
				/*SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
				szMsg.Format("BL %s gripper jam", (const char*)pGripperBuffer->GetName());
				SetErrorMessage(szMsg);*/
				return FALSE;
			}
		}
	}

	szMsg.Format("%s Gripper OPEN", (const char*)pGripperBuffer->GetName());
	BL_DEBUGBOX(szMsg);
//	Sleep(100);
	SetGripperState(pGripperBuffer, FALSE);
//	Sleep(100);


	///////////////////////////////////////////////////
	// Assume frame is delieverd to BUFFER slot at this stage
	///////////////////////////////////////////////////
	pGripperBuffer->SetBufferInfo(m_lBTCurrentBlock, m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]);
	pGripperBuffer->SetBinFull(bBinFull);
	m_lBTCurrentBlock		= 0;
	m_szBinFrameBarcode		= "";

	UpdateExArmMgzSlotNum(m_lBTCurrentBlock, m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]);	//Update slot number on menu

	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

 	TRY {
		SaveMgznRTData();	
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH


	BL_DEBUGBOX("Check frame EXIST in BUFFER slot");

	//Check upper/Lower Buffer has frame or not
	if (!(bBurnInUnload || m_bNoSensorCheck || m_bDisableBL) &&
		!(IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()) || IsGripperFrameExist(pGripperBuffer->GetFrameExistSensorName())))
	{
		szMsg.Format("%s buffer frame not detected1 ...", (const char*)pGripperBuffer->GetName());
		BL_DEBUGBOX(szMsg);
		HmiMessage_Red_Back(szMsg);

		if (1/*BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE*/)
		{
			szMsg.Format("%s buffer frame not detected and aborted", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szMsg);
			//HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}
	}


	if (bOffline)
	{
		BL_DEBUGBOX("Down BT Level");
		BTFramePlatformDown();	// down bin table first if necessary
		if (!IsBTFramePlatformDown())
		{
			szMsg = _T("Bin Table Frame level at UP level (UNLOAD)");
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);		
			SetErrorMessage("UDB_LoadFromBufferToTable: check BT Frame Level fail at UNLOAD");
			return FALSE;
		}

		Sleep(500);
	}

	szText1.Format("U.Buffer = %d, L.Buffer = %d, Current = %d", 
					m_clUpperGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBlock(), m_lBTCurrentBlock);
	BL_DEBUGBOX(szText1);
    BL_DEBUGBOX("UDB UNLOAD from Table to Buffer finish\n");
	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////////
//	UDB_UnloadFromBufferToMgz()
///////////////////////////////////////////////////////////////////////////////////
LONG CBinLoader::UDB_UnloadFromBufferToMgzWithLock(BOOL bBurnInUnload, 
												   BOOL bBinFull, 
												   CBinGripperBuffer *pGripperBuffer,
												   BOOL bClearBin,
												   BOOL bCheckRealignRpy,
												   BOOL bTransferWIPSlot)
{
	LONG lRet = 0;

	bBinFull = pGripperBuffer->IsBinFull();

	if (bBinFull || bTransferWIPSlot)
	{
		if (!LoadUnloadLockMutex())
		{
			CString szLog;

			szLog.Format("LoadUnloadLockMutex  UDB_LoadFromMgzToBufferWithLock  = %ld", m_lLoadUnloadSECSCmdObject);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}

		BL_DEBUGBOX("UDB_UnloadFromBufferToMgzWithLock CheckClosedALBackGate");
		lRet = CheckClosedALBackGate();
		if (lRet)
		{
			LoadUnloadUnlock();
			return lRet;
		}
	}

	lRet = UDB_UnloadFromBufferToMgz(bBurnInUnload, bBinFull, pGripperBuffer, bClearBin, bCheckRealignRpy, bTransferWIPSlot);

	if (bBinFull || bTransferWIPSlot)
	{
		LONG lRet1 = CloseALFrontGate();
	
		if (lRet == TRUE)
		{
			lRet = lRet1;
		}
		LoadUnloadUnlock();
	}
	return lRet;
}

LONG CBinLoader::UDB_UnloadFromBufferToMgz(BOOL bBurnInUnload, 
										   BOOL bBinFull, 
										   CBinGripperBuffer *pGripperBuffer,
										   BOOL bClearBin,
										   BOOL bCheckRealignRpy,
										   BOOL bTransferWIPSlot,
										   BOOL bClearAllFrameTOP2Mode)

{
	CString szLog;
	LONG lBinNo = 0;
	LONG lFullStatus = 0;
	LONG lMgzn = pGripperBuffer->GetMgznNo();
	LONG lSlot = pGripperBuffer->GetSlotNo();

	//2018.4.22
	bBinFull = pGripperBuffer->IsBinFull();  //set it again

	m_lCurrMgzn = lMgzn;
	m_lCurrSlot = lSlot;

	if (bBinFull)
	{
		bClearBin = TRUE;
	}

	szLog.Format("UDB - UNLOAD from Buffer to Mgzn start: bBurnInUnload=%d, bBinFull=%d, bBufferLevel=%d, bCheckRealignRpy=%d",
					bBurnInUnload, bBinFull, pGripperBuffer->IsUpperBuffer(), bCheckRealignRpy);
	BL_DEBUGBOX(szLog);


	//Check Gripper position is valid
	if (IsMagazineSafeToMove() == FALSE)
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
        return FALSE;
	}


	// check frame on Lower / Upper buffer table logically
	if (!(IsAutoLoadUnloadQCTest() && ((m_lCurrMgzn == BL_MGZ_TOP_1) || (m_lCurrMgzn == BL_MGZ_TOP_2) || 
		 (IsMSAutoLineStandloneMode() && (m_lCurrMgzn == BL_MGZ_MID_2)))) && 
		!pGripperBuffer->IsBufferBlockExist())
	{
		SetAlert_Red_Yellow(pGripperBuffer->IsUpperBuffer() ? IDS_BL_NOLOGICAL_UBUFFER_FRAME : IDS_BL_NOLOGICAL_LBUFFER_FRAME);
		szLog.Format("BL %s buffer block is zero", (const char*)pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		return FALSE;
	}

	//Check Lower/Upper Buffer has frame or not
	if (!(bBurnInUnload || m_bNoSensorCheck || m_bDisableBL) && 
		!(IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()) || IsGripperFrameExist(pGripperBuffer->GetFrameExistSensorName())))
	{
		if ( 1/*BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE*/ )
		{
			CString szMsg;
			szMsg.Format("%s buffer frame not detected2 ...", (const char*)pGripperBuffer->GetName());
			BL_DEBUGBOX(szMsg);
			HmiMessage_Red_Back(szMsg);
			return FALSE;
		}
	}

	LONG lDualBufferTempBlock = pGripperBuffer->GetBufferBlock();

	BL_DEBUGBOX(bBinFull ? "Unload(FULL)" : "Unload(NORM)");

	lBinNo = lDualBufferTempBlock;

	LONG lOldMgzn = m_lCurrMgzn;
	LONG lOldSlot = m_lCurrSlot;
	CString szBCName = pGripperBuffer->GetBufferBarcode();
	if (szBCName.IsEmpty())
	{
		szBCName = GetCassetteSlotBCName(m_lCurrMgzn, m_lCurrSlot);
	}
	//Get the Magzine & Slot Number
    if ((bBinFull == TRUE) || bTransferWIPSlot)
    {
		//Get current buffer block original magazine & slot No
		if (!IsAutoLoadUnloadQCTest())
		{
			LONG lRet = FALSE; 
			if (IsMSAutoLineMode())
			{
				//find a the Mgzn & SLOT number with lDualBufferTempBlock in WIP buffer
				if (bClearAllFrameTOP2Mode)
				{
					//case #1 : load a frame to buffer from the source magazine and unload a frame from buffer to the target magzine
					//unloading frame comes directly from the source magazine 
					lRet = GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_ACTIVE, szBCName, lDualBufferTempBlock, lMgzn, lSlot);
					if (!lRet)
					{
						//case #2: unloading frame comes from BT 
						lRet = GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lDualBufferTempBlock, lMgzn, lSlot);
					}
				}
				else
				{
					lRet = GetWIPSortingSlot_AutoLine(lDualBufferTempBlock, lMgzn, lSlot);
					if (lRet && (szBCName != GetCassetteSlotBCName(lMgzn, lSlot)))
					{
						//use output slot with sorting as the full slot
						lRet = GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lDualBufferTempBlock, lMgzn, lSlot);
					}

					if (!lRet)
					{
						lRet = GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, lMgzn, lSlot);
					}
				}
			}
			else
			{
				BOOL bUseEmpty = FALSE;
				lRet = GetLoadMgzSlot(bUseEmpty, lDualBufferTempBlock);
			}

			if (!lRet)
			{
				szLog.Format("\n%d", lDualBufferTempBlock);
				//SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szLog);		
				SetErrorMessage("BL No slot is assigned to this physical block");
				return FALSE;
			}
			else
			{
				lMgzn = m_lCurrMgzn;
				lSlot = m_lCurrSlot;
			}

			lOldMgzn = lMgzn;
			lOldSlot = lSlot;
		}

		if (!IsAutoLoadUnloadQCTest())
		{
			if (bClearAllFrameTOP2Mode)
			{
				lFullStatus = GetClearFrameFullMgznSlot_AutoLine(lMgzn, lSlot);
			}
			else
			{
				lFullStatus = GetFullMgznSlot(lMgzn, lSlot);
			}

			if (lFullStatus == FALSE)
			{
				szLog.Format("UNLOAD: cannot find a FULL slot for #%d", lDualBufferTempBlock);
				HmiMessage_Red_Back(szLog);		
				SetErrorMessage(szLog);
				return FALSE;
			}
		}

		szLog.Format("UNLOAD filmframe to FULL - M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		BL_DEBUGBOX(szLog);	
	}
    else
    {
		if (!IsAutoLoadUnloadQCTest())
		{
			//If current frame was pulled out from INPUT slot (Slot #1 - #15),
			// then need to assign a new WIP slot (Slot #11 - 30) for UNLOAD;
			if (IsMSAutoLineMode())
			{
				LONG lUnloadMgzn = m_lCurrMgzn;
				LONG lUnloadSlot = m_lCurrSlot;
				if (szBCName.IsEmpty())
				{
					//Find a sorting slot again becasue there has machine hang up etc
					if (!GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lDualBufferTempBlock, lUnloadMgzn, lUnloadSlot))
					{
						GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lDualBufferTempBlock, lUnloadMgzn, lUnloadSlot);
					}
				}

				//Update Old Slot information
				lOldMgzn = lUnloadMgzn;
				lOldSlot = lUnloadSlot;

				GetUnloadMgznSlot_AutoLine(szBCName, lDualBufferTempBlock, lUnloadMgzn, lUnloadSlot);

				m_lCurrMgzn	= lUnloadMgzn;
				m_lCurrSlot = lUnloadSlot;
			}
			else
			{
				BOOL bUseEmpty = FALSE;
				lFullStatus = GetLoadMgzSlot(bUseEmpty, lDualBufferTempBlock);

				if (lFullStatus == FALSE)
				{
					szLog.Format("\n%d", lDualBufferTempBlock);
					//SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szLog);		
					SetErrorMessage("BL No slot is assigned to this physical block");
					return FALSE;
				}
			}
		}
		
		lMgzn = m_lCurrMgzn;
		lSlot = m_lCurrSlot;
		szLog.Format("UULOAD filmframe to - M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		BL_DEBUGBOX(szLog);	
	}

	if (!bBurnInUnload && !IsMagazineExist(lMgzn))
	{
		if (IsMagazineExist(lMgzn, FALSE, TRUE)==FALSE)
		{
			BL_DEBUGBOX("magazine not exist");
			szLog.Format("UNLOAD: Magazine Exist Sensor Error, Please Unload Manually Current to Magazine#%d After Checking", lMgzn);
			HmiMessage_Red_Back(szLog);	
			return FALSE;
		}
	}

	if (MoveElevatorToUnload(lMgzn, lSlot, pGripperBuffer->IsUpperBuffer(), TRUE, TRUE) == FALSE)
	{
		szLog.Format("UNLOAD: Fail to move BUFFER table for #%d", lDualBufferTempBlock);
		HmiMessage_Red_Back(szLog);		
		SetErrorMessage(szLog);
		return FALSE;
	}

	//Get the Magzine & Slot Number
	if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
	{
		lMgzn = lOldMgzn;
		lSlot = lOldSlot;

		if ((bBinFull == TRUE) || bTransferWIPSlot)
		{
			lFullStatus = GetFullMgznSlotWithUpdate_AutoLine(bClearAllFrameTOP2Mode, pGripperBuffer->GetUnloadDieGradeCount(), bTransferWIPSlot, lMgzn, lSlot);

			if (lFullStatus == FALSE)
			{
				szLog.Format("UNLOAD: cannot find a FULL slot for #%d", lDualBufferTempBlock);
				HmiMessage_Red_Back(szLog);		
				SetErrorMessage(szLog);
				return FALSE;
			}
	
			szLog.Format("UNLOAD filmframe to FULL(AutoLine) - M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
			BL_DEBUGBOX(szLog);	
		}
		else
		{
			GetUnloadMgznSlotWithUpdate_AutoLine(szBCName, lDualBufferTempBlock, pGripperBuffer->GetUnloadDieGradeCount(), lMgzn, lSlot);
		}
	}

	Sleep(100);

	BOOL bRealignOutputSlot = bClearAllFrameTOP2Mode ? FALSE : TRUE;
	return UDB_UnloadFromBufferToMgzSlot(bBurnInUnload, lBinNo, lFullStatus, lMgzn, lSlot, pGripperBuffer, bClearBin, bCheckRealignRpy, bTransferWIPSlot, TRUE, bRealignOutputSlot);
}


LONG CBinLoader::UDB_UnloadFromBufferToMgzSlot(BOOL bBurnInUnload,
											   LONG lBinNo,
											   BOOL lFullStatus,
											   LONG lMgzn,
										       LONG lSlot,
											   CBinGripperBuffer *pGripperBuffer,
											   BOOL bClearBin,
											   BOOL bCheckRealignRpy,
											   BOOL bTransferWIPSlot,
											   BOOL bUpdateMagazineStatus,
											   BOOL bRealignOutputSlot)
{
	CString szLog;
	BOOL bBinFull = pGripperBuffer->IsBinFull();
	BOOL bUnloadJam		= FALSE;
	BOOL bRealignBin	= TRUE;

	szLog.Format("%s Gripper move to unload from buffer to mgz", (const char*)pGripperBuffer->GetName());
	BL_DEBUGBOX(szLog);
	pGripperBuffer->SetMgznSlotNo(lMgzn, lSlot);

	LONG lLoadPos_X = GetUnloadMagPosn(pGripperBuffer) + pGripperBuffer->GetUnloadMagClampOffsetX();
	LONG lGripperUnloadStatus = GripperMoveSearchJam(pGripperBuffer, lLoadPos_X, TRUE, FALSE, TRUE);//check if 1,encoder>20000 2,outofmgz sensor trigger inside


	if (lGripperUnloadStatus == FALSE)	
    {
		bUnloadJam = TRUE;

		szLog.Format("%s Gripper jam at UnLOAD", (const char*)pGripperBuffer->GetName());
		BL_DEBUGBOX(szLog);
		SetGripperState(pGripperBuffer, FALSE);

		BOOL bOk = BinLoaderUnloadJamMessage_DB(pGripperBuffer, bBurnInUnload);

        if ((bBurnInUnload == FALSE) && !m_bDisableBL && (bOk == FALSE))
        {
			//Assume frame is push into magazine, updated the status so that it will not put into same full slot
			if (bUpdateMagazineStatus && (bBinFull == TRUE))
			{
				UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, FALSE, bBurnInUnload, pGripperBuffer);
			}
			//Has to clear because assume frame is pushed into magazine!!
			SendBinTableUnloadEvent_8011(lMgzn, lSlot, GetCassettePositionName(lMgzn));
			pGripperBuffer->InitBufferInfo();

			szLog.Format("%s gripper jam at LOAD abort\n", pGripperBuffer->GetName());
			BL_DEBUGBOX(szLog);
			return FALSE;
        }
		else
		{
			szLog.Format("%s gripper jam at LOAD continue", pGripperBuffer->GetName());
			BL_DEBUGBOX(szLog);
			HomeGripper(pGripperBuffer);

			if (bUpdateMagazineStatus && (bBinFull == TRUE))
			{
				UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, FALSE, bBurnInUnload, pGripperBuffer);
			}
			SendBinTableUnloadEvent_8011(lMgzn, lSlot, GetCassettePositionName(lMgzn));
			pGripperBuffer->InitBufferInfo();
		}
    }

	szLog.Format("Gripper %s OFF at LOAD pos", (const char*)pGripperBuffer->GetName());
	BL_DEBUGBOX(szLog);
	SetGripperState(pGripperBuffer, FALSE);
	Sleep(500);

	if (bUnloadJam)	
	{
		BL_DEBUGBOX("gripper HOME because of JAM");
		HomeGripper(pGripperBuffer);
		Sleep(200);
	}


	BL_DEBUGBOX("Gripper move to READY");
	//lGripperUnloadStatus = GripperMoveToReadyPosn(pGripperBuffer, SFM_NOWAIT);
	lGripperUnloadStatus = GripperMoveToPreUnloadPosn(pGripperBuffer, SFM_NOWAIT);

	if (bUpdateMagazineStatus && UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, FALSE, bBurnInUnload, pGripperBuffer) == FALSE)
	{
		SendBinTableUnloadEvent_8011(lMgzn, lSlot, GetCassettePositionName(lMgzn));
		return FALSE;
	}

/*
	//when transfer frame from inline to standlonemode
	if (!bUpdateMagazineStatus && IsMSAutoLineStandloneMode())
	{
		if (bClearBin)
		{
			m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
			m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
		}
	}
*/

	if ((m_nExArmReAlignBinConvID != 0) && (bCheckRealignRpy))
	{
		bRealignBin = RealignBinFrameRpy();
	}

	SendBinTableUnloadEvent_8011(lMgzn, lSlot, GetCassettePositionName(lMgzn));
	if (!GripperSyncMove_Auto(pGripperBuffer))
	{
		return FALSE;
	}

	pGripperBuffer->InitBufferInfo();

	GenerateOMRTTableFile(m_lSelMagazineID);
 	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

 	TRY {
		SaveMgznRTData();	
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH


	if (bCheckRealignRpy)
	{
		if (!bRealignBin)
		{
			BL_DEBUGBOX("Re-align bin frame - FAIL");	
			SetErrorMessage("BL: Realign bin frame fails 2!");
			return FALSE;
		}
		else if (m_nExArmReAlignBinConvID != 0)
		{
			BL_DEBUGBOX("Re-align bin frame - ID not 0!?");	
		}
	}

	szLog.Format("U.Buffer = %d, L.Buffer = %d, Current = %d", 
				m_clUpperGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBlock(), m_lBTCurrentBlock);
	BL_DEBUGBOX(szLog);
    BL_DEBUGBOX("Unload from Buffer to Mgz finish\n");

	CString szBinBlkGrade;
	szBinBlkGrade.Format("%d", lBinNo);	
	CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("UNLOAD Frame", GetMagazineName(lMgzn).MakeUpper(), 
		lSlot+1, m_szBCName, szBinBlkGrade, GetBinBlkBondedCount(lSlot + 1), m_bUseBarcode);

	//if there has sorting status in WIP buffer, and will move it to output buffer
	if (bRealignOutputSlot && !IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
	{
		BOOL bRet1 = TransferSortingMgznSlotFromWIPToOutput_AutoLine();
		BOOL bRet2 = AdjustOutputSlotSortingPosn_AutoLine();
		if (bRet1 || bRet2)
		{
			SaveData();
		}
	}
	return TRUE;
}

//===================================================================================================
//				Load magazine
//===================================================================================================
LONG CBinLoader::UDB_LoadFromMgzToBufferWithLock(BOOL bBurnInLoad, 
												LONG lLoadBlock, 
												CBinGripperBuffer *pGripperBuffer,
												BOOL bManualChangeFrame)
{
	LONG lRet = UDB_SubLoadFromMgzToBufferWithLock(bBurnInLoad, lLoadBlock, pGripperBuffer);

	if (lRet == Err_BinBackGateNotClosed)
	{
		return FALSE;
	}

	if (!(IsAutoLoadUnloadQCTest() || IsAutoLoadUnloadSISTest()) && IsMSAutoLineMode() && (lRet == Err_BinMagzineOutputWIPFull))
	{
		if (!WaitForAutoLineToRemoveOutputWIPFrame(bManualChangeFrame))
		{
			return Err_BinMagzineOutputWIPFull; //no response from autoline.
		}

		//Reset Trigger "TransferBinFrame"
		m_bTriggerTransferBinFrame = FALSE; 

		CString szLog;
		szLog.Format("BL: Err_BinMagzineOutputWIPFull");
		BL_DEBUGBOX(szLog);

		LONG lMgzn = 0, lSlot = 0;

		//there has three cases when load a fram from WIP slot into output slot.
		BOOL bTransferWIPBinFullFrame = GetWIPFullSlot_AutoLine(lMgzn, lSlot);
		BOOL bTransferWIPNotSelectedBinFrame = FALSE;
		BOOL bTransferWIPBinOnlyOneFrame = FALSE;
		CUIntArray aulSelectdGradeList; 
		CUIntArray aulSelectdGradeLeftDieCountList;
		BOOL bASCIIGradeDisplayMode = FALSE;

		LONG lTransferWIPSlotMode = bTransferWIPBinFullFrame ? TRANSFER_WIP_FULL_BIN_FRAME : TRANSFER_WIP_NONE_FRAME;

//		not trigger this case in MS50 becasue there has already 175 WIP slots
/*
		if (!bTransferWIPBinFullFrame)
		{
			CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
			if (pWaferTable != NULL)
			{
				pWaferTable->GetWaferMapSelectedGradeList(aulSelectdGradeList, aulSelectdGradeLeftDieCountList);
				bASCIIGradeDisplayMode = pWaferTable->IsASCIIGradeDisplayMode();
			}

			bTransferWIPNotSelectedBinFrame = GetWIPActiveSlotWithoutGradeList_AutoLine(bASCIIGradeDisplayMode, aulSelectdGradeList, aulSelectdGradeLeftDieCountList, lMgzn, lSlot);
			if (bTransferWIPNotSelectedBinFrame)
			{
				lTransferWIPSlotMode = TRANSFER_WIP_NOT_SELECTED_BIN_FRAME;
			}
		}
		if (!bTransferWIPBinFullFrame && !bTransferWIPNotSelectedBinFrame)
		{
			bTransferWIPBinOnlyOneFrame = TRUE;
			lTransferWIPSlotMode = TRANSFER_WIP_ONLY_ONE_BIN_FRAME;
		}
*/
		szLog.Format("LoadBinFrame: Get WIP Frame Mode = %d, %d, %d, %d, bASCIIGradeDisplayMode = %d", lTransferWIPSlotMode, 
					 bTransferWIPBinFullFrame, bTransferWIPNotSelectedBinFrame, bTransferWIPBinOnlyOneFrame, bASCIIGradeDisplayMode);
		BL_DEBUGBOX(szLog);

		while (TRUE)
		{
			LONG lWIPLoadBlock = lLoadBlock;
			if (!IsOutputEmptySlot_AutoLine())
			{
				break;
			}

			//Case#1: Only Get the FULL state's Frame in WIP slot
			if (bTransferWIPBinFullFrame && !GetWIPFullSlot_AutoLine(lMgzn, lSlot))
			{
				break;
			}
/*
			//Case#2: Only Get the ACTIVE state's Frame in WIP slot, which is not belong to the selected grade list
			//of the wafer map
			if (bTransferWIPNotSelectedBinFrame && !GetWIPActiveSlotWithoutGradeList_AutoLine(bASCIIGradeDisplayMode, aulSelectdGradeList, aulSelectdGradeLeftDieCountList, lMgzn, lSlot))
			{
				break;
			}

			//Case#3: Only Get a ACTIVE state's Frame in WIP slot according to the sequence
			if (bTransferWIPBinOnlyOneFrame && !GetWIPActiveSlot_AutoLine(lMgzn, lSlot))
			{
				break;
			}
*/
			lWIPLoadBlock = GetCassetteSlotGradeBlock(lMgzn, lSlot);
			szLog.Format("LoadBinFrame: Get WIP Frame Slot(%d,%d), (WIP)Bin Block = %d", lMgzn, lSlot, lWIPLoadBlock);
			BL_DEBUGBOX(szLog);

			//Get a WIP frame with full state to the output slot
			lRet = UDB_TransferFrame(bBurnInLoad, lLoadBlock, pGripperBuffer);

			if (lRet == Err_BinBackGateNotClosed)
			{
				return FALSE;
			}

			//Reset SRAM becasue transfer WIP slot failure
			(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= 0;
			(*m_psmfSRam)["BinTable"]["BlkInUse"]		= 0;
			return lRet;

			//Only transfer one frame from WIP slots to output.
			if (bTransferWIPBinOnlyOneFrame)
			{
				break;
			}
		}

		LONG lRet = UDB_SubLoadFromMgzToBufferWithLock(bBurnInLoad, lLoadBlock, pGripperBuffer);

		if (lRet == Err_BinBackGateNotClosed)
		{
			return FALSE;
		}
	}

	return lRet;
}

LONG CBinLoader::UDB_TransferFrame(BOOL bBurnInLoad,
								   LONG lLoadBlock,
								   CBinGripperBuffer *pGripperBuffer,
								   BOOL bBinFull,
								   BOOL bManualChangeFrame,
								   BOOL bTransferWIPSlot,
								   BOOL bClearAllMode)

{
	//if bClearAllFrameTOP2Mode is TRUE, machine transfers the frame from WIP to TOP2 under inline-mode
	BOOL bClearAllFrameTOP2Mode = IsMSAutoLineMode() && bClearAllMode;
	BL_DEBUGBOX("Transfer Frame");
	LONG lRet = 0;
	if (!bClearAllFrameTOP2Mode)
	{
		if (!LoadUnloadLockMutex())
		{
			CString szLog;

			szLog.Format("LoadUnloadLockMutex  UDB_TransferFrame  = %ld", m_lLoadUnloadSECSCmdObject);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}

		BL_DEBUGBOX("UDB_TransferFrame CheckClosedALBackGate");
		lRet = CheckClosedALBackGate();
		if (lRet)
		{
			LoadUnloadUnlock();
			return lRet;
		}
	}
	//Get a WIP frame with full state to the output slot
	if (!(lRet = UDB_LoadFromMgzToBuffer(bBurnInLoad, lLoadBlock, pGripperBuffer, bManualChangeFrame)))
	{
		SetErrorMessage("BL UDB_LoadFromMgzToBuffer: LOAD a WIP frame from Mgzn to Buffer fails");
	}
		
	if (lRet)
	{
		Sleep(200);
		pGripperBuffer->SetBinFull(bBinFull);
		if (!(lRet = UDB_UnloadFromBufferToMgz(bBurnInLoad, bBinFull, pGripperBuffer, FALSE, FALSE, bTransferWIPSlot, bClearAllFrameTOP2Mode)))
		{
			SetErrorMessage("BL UDB_UnloadFromBufferToMgz: UNLOAD buffer fails");
		}
	}

	if (!bClearAllFrameTOP2Mode)
	{
		LONG lRet1 = CloseALFrontGate();
	
		if (lRet == TRUE)
		{
			lRet = lRet1;
		}

		LoadUnloadUnlock();
	}
	return lRet;
}


LONG CBinLoader::UDB_SubLoadFromMgzToBufferWithLock(BOOL bBurnInLoad,
													LONG lLoadBlock,
													CBinGripperBuffer *pGripperBuffer)
{
	BOOL bLoadFrameAtMgazineTOP1 = FALSE;
	BOOL bUseEmpty = TRUE;
	if (IsMSAutoLineMode())
	{
		bLoadFrameAtMgazineTOP1 = TRUE;
		if (!IsAutoLoadUnloadQCTest() && GetLoadMgzSlot(bUseEmpty, lLoadBlock))
		{
			if (!bUseEmpty)
			{
				bLoadFrameAtMgazineTOP1 = FALSE;
			}
		}
	}

	LONG lRet = 0;
	if (bLoadFrameAtMgazineTOP1)
	{
		if (!LoadUnloadLockMutex())
		{
			CString szLog;

			szLog.Format("LoadUnloadLockMutex  UDB_LoadFromMgzToBufferWithLock  = %ld", m_lLoadUnloadSECSCmdObject);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}

		BL_DEBUGBOX("UDB_ SubLoadFromMgzToBufferWithLock CheckClosedALBackGate");
		lRet = CheckClosedALBackGate();
		if (lRet)
		{
			LoadUnloadUnlock();
			return lRet;
		}
	}

	lRet = UDB_LoadFromMgzToBuffer(bBurnInLoad, lLoadBlock, pGripperBuffer);

	if (bLoadFrameAtMgazineTOP1)
	{
		LONG lRet1 = CloseALFrontGate();
	
		if (lRet == TRUE)
		{
			lRet = lRet1;
		}

		LoadUnloadUnlock();
	}
	return lRet;
}


LONG CBinLoader::UDB_SubLoadBinFrame(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, LONG lLoadBlock, BOOL bOffline,
									 BOOL bManualChangeFrame, LONG lTransferWIPSlotMode, BOOL bClearAllFrameTOP2Mode)
{
	LONG lStatus		= TRUE;
	//CBinGripperBuffer	*pGripperBuffer = &m_clUpperGripperBuffer;
	LONG lBufferBlock	= pGripperBuffer->GetBufferBlock();
	BL_DEBUGBOX("Sub Load Bin Frame");
	if ((lBufferBlock > 0) && (lBufferBlock != lLoadBlock))
	{
		lStatus = UDB_UnloadFromBufferToMgz(bBurnInLoad, FALSE, pGripperBuffer, FALSE, FALSE, FALSE, bClearAllFrameTOP2Mode);

		if (lStatus == FALSE)
		{
			SetErrorMessage("BL ManualLoadFilmFrame (UDB): UNLOAD buffer fails");
			return FALSE;
		}

		Sleep(200);
	}

	lBufferBlock	= pGripperBuffer->GetBufferBlock();
	if (!((lBufferBlock > 0) && (lBufferBlock == lLoadBlock)))
	{
		lStatus = UDB_LoadFromMgzToBuffer(bBurnInLoad, lLoadBlock, pGripperBuffer, bManualChangeFrame, lTransferWIPSlotMode);
		if (lStatus == FALSE)
		{
			SetErrorMessage("BL ManualLoadFilmFrame (UDB): LOAD from Mgzn to Buffer fails");
			return FALSE;
		}
	
		Sleep(200);
	}

	lStatus = UDB_LoadFromBufferToTable(bBurnInLoad, pGripperBuffer, bOffline, lTransferWIPSlotMode);
	if (lStatus == FALSE)
	{
		SetErrorMessage("BL ManualLoadFilmFrame (UDB): LOAD from Buffer to Table fails");
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//	UDB_LoadFromMgzToBuffer()
///////////////////////////////////////////////////////////////////////////////////
LONG CBinLoader::UDB_LoadFromMgzToBuffer(BOOL bBurnInLoad, 
										 LONG lLoadBlock, 
										 CBinGripperBuffer *pGripperBuffer,
										 BOOL bManualChangeFrame, LONG lTransferWIPSlotMode, BOOL bReScanBarcodeOnly)
{
    LONG lMgzn = 0;
    LONG lSlot = 0;
	BOOL bFrameExist = FALSE;
	CString szLog;

    BL_DEBUGBOX("UDB - LOAD from Mgzn to Buffer start");

	if (IsMagazineSafeToMove() == FALSE)
	{
		BL_DEBUGBOX("Gripper is not in safe position\n");
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		return FALSE;
	}

	// check frame on upper buffer table logically
	if (pGripperBuffer->IsBufferBlockExist() && (bBurnInLoad == FALSE) && (!m_bDisableBL) )
	{
		if (BL_YES_NO(HMB_BL_BUFFBLK_NOT_ZERO, IDS_BL_LOAD_FRAME) == FALSE)
		{
			szLog.Format("Logical Block of %s Buffer is not Zero", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szLog);
			return FALSE;
		}
	}

	//Check Upper Buffer has frame or not physically
	bFrameExist = IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()) || 
				  IsGripperFrameExist(pGripperBuffer->GetFrameExistSensorName());
	if (bBurnInLoad || m_bNoSensorCheck || m_bDisableBL)
	{
		if (bFrameExist && m_bNoSensorCheck)
		{
			szLog.Format("Frame exists in %s Buffer but by-pass by Disabling Sensor Check", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szLog);
		}
		bFrameExist = FALSE;
	}

	if (bFrameExist == TRUE)
	{
		szLog.Format("BL Frame exists in %s Buffer", (const char*)pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		SetAlert_Red_Yellow(pGripperBuffer->IsUpperBuffer() ? IDS_BL_FRMAE_EXIST_IN_U_BUFFER : IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
		return FALSE;
	}

	BOOL bUseEmpty = FALSE;
	if (lTransferWIPSlotMode && !IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
	{
		BOOL bGetSlot = FALSE;
		CString szTitle;
		CUIntArray aulSelectdGradeList; 
		CUIntArray aulSelectdGradeLeftDieCountList;
		BOOL bASCIIGradeDisplayMode = FALSE;
		CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));

		switch (lTransferWIPSlotMode)
		{
		case TRANSFER_WIP_FULL_BIN_FRAME:
			if (GetWIPFullSlot_AutoLine(lMgzn, lSlot))
			{
				bGetSlot = TRUE;
				szTitle = _T("GetWIPFullSlot_AutoLine");
			}
			break;
		case TRANSFER_WIP_NOT_SELECTED_BIN_FRAME:
			if (pWaferTable != NULL)
			{
				pWaferTable->GetWaferMapSelectedGradeList(aulSelectdGradeList, aulSelectdGradeLeftDieCountList);
				bASCIIGradeDisplayMode = pWaferTable->IsASCIIGradeDisplayMode();
			}

			if (GetWIPActiveSlotWithoutGradeList_AutoLine(bASCIIGradeDisplayMode, aulSelectdGradeList, aulSelectdGradeLeftDieCountList, lMgzn, lSlot))
			{
				bGetSlot = TRUE;
				szTitle = _T("GetWIPActiveSlotWithGradeList_AutoLine");
			}
			break;
		case TRANSFER_WIP_ONLY_ONE_BIN_FRAME:
			if (GetWIPActiveSlot_AutoLine(lMgzn, lSlot))
			{
				bGetSlot = TRUE;
				szTitle = _T("GetWIPActiveSlot_AutoLine");
			}
			break;
		}

		if (bGetSlot)
		{
			m_lCurrMgzn = lMgzn;
			m_lCurrSlot = lSlot;

			szLog.Format("BL: %s = %ld, %d", (const char*)szTitle, m_lCurrMgzn, m_lCurrSlot);
			BL_DEBUGBOX(szLog);
		}
		else
		{
			CString szMsg;
			szMsg.Format("\n%d", lLoadBlock);
			//SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);	
			SetErrorMessage("BL No WIP slot is assigned to this physical block");
			return FALSE;
		}
	}
    else if (!IsAutoLoadUnloadQCTest() && !GetLoadMgzSlot(bUseEmpty, lLoadBlock))
	{
		BOOL bRet = FALSE;
		if (!bManualChangeFrame && IsMSAutoLineMode())
		{
			LoadUnloadUnlock();
			bRet = WaitForAutoLineToInputFrame();
			if (bRet && !GetLoadMgzSlot(bUseEmpty, lLoadBlock))
			{
				bRet = FALSE;
			}
			LoadUnloadLock();

			if (bRet)
			{
				//Open Front Gate
				LONG lRet = 0;
				BL_DEBUGBOX("UDB_LoadFromMgzToBuffer CheckClosedALBackGate");
				lRet = CheckClosedALBackGate();
				if (lRet)
				{
					LoadUnloadUnlock();
					return lRet;
				}
			}
		}

		if (!bRet)
		{
			if (IsMSAutoLineMode() && !bManualChangeFrame && !m_bAutoLineLoadDone)
			{
				BL_DEBUGBOX("UDB Load Exit!!");
				return FALSE; //exit
			}

			CString szMsg;
			szMsg.Format("\n%d", lLoadBlock);
			//SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
			SetErrorMessage("BL No slot is assigned to this physical block");
			SetBufferLevel(FALSE);
			return FALSE;
		}
	}
/*
	//Case#1 , trigger "output slot full"
	if (bUseEmpty && IsMSAutoLineMode())
	{
		//if WIP slot is sorting and there has not output empty slot, so ask SIS to take out the frame from output slots
		if (IsWIPSortingSlotExisted_AutoLine(lLoadBlock) && IsFullOutputSlot_AutoLine())
		{
			return Err_BinMagzineOutputWIPFull;
		}
	}
*/
	//Case#1 , trigger "output slot full", it will slow UPH
	if (IsMSAutoLineMode())
	{
		//if WIP slot is sorting and there has not output empty slot, so ask SIS to take out the frame from output slots
		if (IsFullOutputSlot_AutoLine())
		{
			BL_DEBUGBOX("if WIP slot is sorting and there has not output empty slot, so ask SIS to take out the frame from output slots");
			return Err_BinMagzineOutputWIPFull;
		}
	}

	return UDB_LoadFromMgzSlotToBuffer(bBurnInLoad, lLoadBlock, m_lCurrMgzn, m_lCurrSlot, bUseEmpty, bReScanBarcodeOnly, pGripperBuffer);
}


LONG CBinLoader::UDB_LoadFromMgzSlotToBuffer(BOOL bBurnInLoad, 
											 LONG lLoadBlock,
											 LONG lCurrMgzn,
											 LONG lCurrSlot,
											 BOOL bUseEmpty,
											 BOOL bReScanBarcodeOnly,
											 CBinGripperBuffer *pGripperBuffer)
{
	CString szLog;
	CString szText1;
	CString szBinBlkGrade;

    LONG lMgzn = 0;
    LONG lSlot = 0;

	m_lCurrMgzn = lCurrMgzn;
	m_lCurrSlot = lCurrSlot;

    if( ElevatorZSynMove_Auto()==FALSE )
	{
		SetBufferLevel(FALSE);
		BL_DEBUGMESSAGE("Elevator Z syn move _Auto fail");
        return FALSE;
	}

//	LONG lLoadSearchInClampOffsetX = m_lLoadSearchInClampOffsetX;
//	if (lLoadSearchInClampOffsetX < 1500)
//	{
	LONG lLoadSearchInClampOffsetX = 2000; //1500;//2019.04.24 Matt: modification due to load frame error 
//	}
	 BL_DEBUGMESSAGE("Load From Mgz Slot to Buffer start");
	if( bUseEmpty == FALSE )
    {
        // move elevator to the slot and gripper to ready
        lMgzn = m_lCurrMgzn;
        lSlot = m_lCurrSlot;
        szLog.Format("LOAD filmframe from M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
        BL_DEBUGMESSAGE(szLog);
		
		/*if ( CheckElevatorCover() == FALSE )
        {
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
        }*/

		pGripperBuffer->SetMgznSlotNo(lMgzn, lSlot);
        BL_DEBUGBOX("magazine move to load");
		Sleep(200);
		if (MoveElevatorToLoad(lMgzn, lSlot, pGripperBuffer) == FALSE)
		{
			return FALSE;
		}

		if ( bBurnInLoad==FALSE && IsMagazineExist(lMgzn)==FALSE )
		{
			if (IsMagazineExist(lMgzn, FALSE, TRUE)==FALSE)
			{
				BL_DEBUGBOX("magazine not exist");
				return FALSE;
			}
		}

        /*if ( CheckElevatorCover() == FALSE )
        {
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
        }*/
  
		//Move to before InClamp checking
		if (IsAllMotorsEnable() == FALSE)	
		{
			SetErrorMessage("Motor is OFF during LOAD mag frame");
			SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
			//X_Home();
			//HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}

		BOOL bFrameJam = FALSE;

		//Gripper move to load position
		BL_DEBUGBOX("gripper move to load");
		Sleep(200);
		LONG lLoadPos_X = GetUnloadMagPosn(pGripperBuffer) + pGripperBuffer->GetUnloadMagClampOffsetX() - 1000;
		if (!GripperMoveSearchJam(pGripperBuffer, lLoadPos_X))
		{
			SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_MGZ_TO_BUFFER);
			/*SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
			szLog.Format("BL %s gripper jam", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szLog);*/
			SetGripperState(pGripperBuffer, FALSE);
			Sleep(100);
			HomeGripper(pGripperBuffer);
			return FALSE;
		}

		bFrameJam = IsFrameJam(pGripperBuffer->GetFrameJamSensorName());
		if (bFrameJam == TRUE )
		{
			GripperMotionStop(pGripperBuffer);
			Sleep(100);

			SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_MGZ_TO_BUFFER);
			//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			HomeGripper(pGripperBuffer);
			GripperHouseKeeping(pGripperBuffer);
			return FALSE;
		}

		if (!IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))
		{
			// search the frame in clamp
			//if ( BinGripperSearchInClamp(HP_POSITIVE_DIR, 1500, 100)==FALSE )
			if (GripperSearchInClamp(pGripperBuffer, HP_POSITIVE_DIR, lLoadSearchInClampOffsetX, 400) == FALSE)
			{
				if ( IsAllMotorsEnable() == FALSE )	
				{
					SetErrorMessage("Motor is OFF during LOAD mag frame");
					SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
					HomeGripper(pGripperBuffer);
					GripperHouseKeeping(pGripperBuffer);
					return FALSE;
				}

				if (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()) == TRUE)
				{
					GripperMotionStop(pGripperBuffer);
					Sleep(100);

					SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_MGZ_TO_BUFFER);
					//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
					HomeGripper(pGripperBuffer);
					GripperHouseKeeping(pGripperBuffer);
					return FALSE;
				}

				if (bBurnInLoad == FALSE)
				{
					BOOL bResult = 0/*BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, IDS_BL_LOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg)*/;
					if (bResult == TRUE)
					{
						if (!IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()) )
						{
							/*SetErrorMessage("BL No frame is detected in gripper");
							SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);*/
							SetFrameNotDetectedAlarm(pGripperBuffer, IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_LOAD);
							HomeGripper(pGripperBuffer);
							GripperHouseKeeping(pGripperBuffer);
							return FALSE;
						}
					}
					else
					{
						/*SetErrorMessage("BL No frame is detected in gripper");
						SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);*/
						SetFrameNotDetectedAlarm(pGripperBuffer, IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_LOAD);
						HomeGripper(pGripperBuffer);
						GripperHouseKeeping(pGripperBuffer);
						return FALSE;
					}
				}
			}
		}	//if ( !IsFrameInClamp() )
		else if (!m_bNoSensorCheck)
		{
			SetErrorMessage("BL has frame before load_Empty");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
			HomeGripper(pGripperBuffer);
			GripperHouseKeeping(pGripperBuffer);
			return FALSE;
		}
    }
    else	//search empty frame
    {
		INT nRetryCount = 0;
        while (1)
        {
			nRetryCount++;
			if (m_lLoadFrameRetryCount > 0 && (nRetryCount > m_lLoadFrameRetryCount))
			{
				CString szErr;
				szErr.Format("Load BL EMPTY frame retry fail - Count = %ld", m_lLoadFrameRetryCount);
				BL_DEBUGBOX(szErr);
				SetErrorMessage(szErr);
				SetAlert_Red_Yellow(IDS_BL_EX_LOAD_EMPTY_FRAME_FAIL);
				GripperHouseKeeping(pGripperBuffer);
				return FALSE;
			}

            BL_DEBUGBOX("gripper ready and magazine to empty");
			LONG lGetNullStatus = GetNullMgznSlot(lMgzn, lSlot, lLoadBlock);
			
			if (lGetNullStatus == FALSE)
			{
				GripperHouseKeeping(pGripperBuffer);
				return FALSE;
			}

			if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
			{
				//Output&WIP Full, wait for EM...
				if (lGetNullStatus == Err_BinMagzineOutputWIPFull)
				{
					return Err_BinMagzineOutputWIPFull;
				}
				else
				{
					lMgzn = m_lCurrMgzn;
					lSlot = m_lCurrSlot;
				}
				szLog.Format("Direct Load filmframe from Empty (AUTOLINE) M%d, S%d", m_lCurrMgzn, m_lCurrSlot);
			}
			else
			{
				szLog.Format("Direct Load filmframe from Empty M%d, S%d", lMgzn, lSlot);
			}
			BL_DEBUGBOX(szLog);

			/*if ( CheckElevatorCover() == FALSE )
            {
				RestoreNullMgznSlot(lMgzn, lSlot);
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
            }*/

			pGripperBuffer->SetMgznSlotNo(lMgzn, lSlot);
			Sleep(200);
			if (MoveElevatorToLoad(lMgzn, lSlot, pGripperBuffer) == FALSE)
			{
				RestoreNullMgznSlot(lMgzn, lSlot);	
				return FALSE;
			}
			
            if ( bBurnInLoad==FALSE && IsMagazineExist(lMgzn)==FALSE )
            {	
				Sleep(200);	
				if (IsMagazineExist(lMgzn, FALSE, TRUE)==FALSE)
				{
					RestoreNullMgznSlot(lMgzn, lSlot);
					GripperHouseKeeping(pGripperBuffer);
					return FALSE;
				}
            }

            /*if ( CheckElevatorCover() == FALSE )
            {
				RestoreNullMgznSlot(lMgzn, lSlot);
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
            }*/

			LONG lLoadPos_X = GetUnloadMagPosn(pGripperBuffer) + pGripperBuffer->GetUnloadMagClampOffsetX() - 1000;
			if (!GripperMoveSearchJam(pGripperBuffer, lLoadPos_X))
			{
				SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_MGZ_TO_BUFFER);
				/*SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
				szLog.Format("BL %s gripper jam", (const char*)pGripperBuffer->GetName());
				SetErrorMessage(szLog);*/
				SetGripperState(pGripperBuffer, FALSE);
				Sleep(100);
				HomeGripper(pGripperBuffer);
				return FALSE;
			}

			if ( IsAllMotorsEnable() == FALSE )	
			{
				SetErrorMessage("Motor is OFF during LOAD mag EMPTY frame");
				SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
				HomeGripper(pGripperBuffer);	
				RestoreNullMgznSlot(lMgzn, lSlot);
				GripperHouseKeeping(pGripperBuffer);
				return FALSE;
			}

			if (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()) == TRUE)
			{
				GripperMotionStop(pGripperBuffer);
				SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_MGZ_TO_BUFFER);
				//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				HomeGripper(pGripperBuffer);
				RestoreNullMgznSlot(lMgzn, lSlot);
				GripperHouseKeeping(pGripperBuffer);
				return FALSE;
			}			

			if (!IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))
			{
				if (GripperSearchInClamp(pGripperBuffer, HP_POSITIVE_DIR, lLoadSearchInClampOffsetX, 400, TRUE, FALSE, FALSE)==FALSE)
				{
					if (IsAllMotorsEnable() == FALSE)	
					{
						SetErrorMessage("Motor is OFF during LOAD EMPTY mag frame");
						SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
						HomeGripper(pGripperBuffer);
						GripperHouseKeeping(pGripperBuffer);
						RestoreNullMgznSlot(lMgzn, lSlot);
						return FALSE;
					}

					if (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()) == TRUE)
					{
						//GripperMotionStop(pGripperBuffer);
						//SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_MGZ_TO_BUFFER);
						//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
						HomeGripper(pGripperBuffer);
						//GripperHouseKeeping(pGripperBuffer);
						//RestoreNullMgznSlot(lMgzn, lSlot);
						//return FALSE;
						//2019.04.24 Matt:Enlarge search distance is easy to trigger frame jam
					}

					if( bBurnInLoad==FALSE )
					{
						Sleep(100);
						LONG lReadyPos_X = GetReadyPosn(pGripperBuffer);
						if (GripperMoveTo(pGripperBuffer, lReadyPos_X) != TRUE)
						{
							//Additional protection breaking loop in case gripper-move has problem
							SetErrorMessage("Gripper move back to READY fails during LOAD EMPTY mag frame");
							RestoreNullMgznSlot(lMgzn, lSlot);	
							return FALSE;
						}
						continue;
					}
				}
			}	//if ( !IsFrameInClamp() )
			else if (!m_bNoSensorCheck)
			{
				SetErrorMessage("BL has frame before load");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_DETECTED_IN_BL_ERR_HANDLE);
				HomeGripper(pGripperBuffer);
				GripperHouseKeeping(pGripperBuffer);
				return FALSE;
			}

            break;
        }
    }

	//No matter any gripper errors happened below, the frame is assumed LOAD into buffer,
	//  so slot status has to be updated for EMPTY frame here
	if( bUseEmpty == TRUE )
	{
		CString szText1;
		szText1.Format("S%d", m_lCurrSlot+1);
		szLog = "Upate EMPTY frame slot status for " + GetMagazineName(m_lCurrMgzn).MakeUpper() + ", " + szText1 + " (E), BLBarcode = " + m_szBCName;
		BL_DEBUGBOX(szLog);

//		m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE;
	}


	///////////////////////////////////////////////////
	// Gripper is gripping BT frame in MGZN ...
	///////////////////////////////////////////////////
	BL_DEBUGBOX("Gripper state ON in Mgzn");

	Sleep(100);
	SetGripperState(pGripperBuffer, TRUE);
	Sleep(800);
	//PULL out the frame a little bit to detach from back-magnet
	GripperSearchScan(pGripperBuffer, HP_NEGATIVE_DIR, -500);
	Sleep(300);

	///////////////////////////////////////////////////
	// Scanning Barcode
	///////////////////////////////////////////////////
	if (m_bUseBarcode)
	{
		BL_DEBUGBOX("Scann barcode ....");
		//Read Barcode when Load frame
		BOOL bLoadCompareBarcode = (!bUseEmpty || IsMSAutoLineMode()) ? TRUE : FALSE;
		if (bReScanBarcodeOnly)
		{
			bLoadCompareBarcode = FALSE;
		}
		LONG nErr = UDB_ReadBarCodeAtLoad(bLoadCompareBarcode, bBurnInLoad, pGripperBuffer, lLoadBlock);

		if (nErr != TRUE)
		{
			if (nErr == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_READ_BC_FAILURE);
			}
			szLog.Format("UDB: %s Gripper ReadBarcode failure in LOAD", (const char*)pGripperBuffer->GetName());
			SetErrorMessage(szLog);
			SetGripperState(pGripperBuffer, FALSE);
			Sleep(500);
			HomeGripper(pGripperBuffer);
			return FALSE;
		}
	}


	///////////////////////////////////////////////////
	// Gripper is pulling the frame to BUFFER slot
	///////////////////////////////////////////////////
	BL_DEBUGBOX("gripper move to HOME");
	LONG lReadyPos_X = GetReadyPosn(pGripperBuffer);
	LONG lGripperLoadStatus = GripperMoveTo(pGripperBuffer, lReadyPos_X);
	if (lGripperLoadStatus != TRUE)	
	{
		//HouseKeeping2(m_lReadyPos_X2, FALSE);
		//HouseKeeping(m_lReadyPos_X, FALSE);
		szLog.Format("UDB: %s Gripper failure in LOAD", (const char*)pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		SetGripperState(pGripperBuffer, FALSE);
		Sleep(500);
		HomeGripper(pGripperBuffer);
		return FALSE;
	}

	Sleep(100);
	SetGripperState(pGripperBuffer, FALSE);	//Release gripper clamp; frame is held by magnet only
	Sleep(500);

	if (!(bBurnInLoad || m_bNoSensorCheck || m_bDisableBL) && !(IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()) || IsDualBufferExist(pGripperBuffer->GetFrameExistSensorName())))
	{
		SetGripperState(pGripperBuffer, FALSE);
		Sleep(500);
		/*szLog.Format("UDB: NO frame detected in %s gripper", (const char*)pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);	*/
		SetFrameNotDetectedAlarm(pGripperBuffer, IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_LOAD);
		//HouseKeeping(m_lReadyPos_X, FALSE);
		HomeGripper(pGripperBuffer);
		return FALSE;
	}

/*
	if (!m_bNoSensorCheck && !IsDualBufferExist(pGripperBuffer->GetFrameExistSensorName()))
	{
		//szLog.Format("UDB: NO frame detected in %s Buffer slot", (const char*)pGripperBuffer->GetName());
		//SetErrorMessage(szLog);
		//SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);
		SetFrameNotDetectedAlarm(pGripperBuffer, IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_LOAD);
		HomeGripper(pGripperBuffer);
		return FALSE;
	}
*/
	//===============================================================================================
	if (IsMSAutoLineMode())
	{
		m_szBCName = m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot];
	}

	pGripperBuffer->SetBufferInfo(lLoadBlock, m_szBCName);
	pGripperBuffer->SetMgznSlotNo(m_lCurrMgzn, m_lCurrSlot);
	pGripperBuffer->SetBinFull(FALSE);
	pGripperBuffer->SetFromEmptyFrame(bUseEmpty);
	pGripperBuffer->SetUnloadDieGradeCount(GetNoOfSortedDie((UCHAR)lLoadBlock));

	if (bUseEmpty == TRUE)
	{
		//The status should be active after loading from mgz to buffer by Matthew 20190527
		m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE;

		szText1.Format("S%d", lSlot+1);
		szLog = "BL FilmFrame is loaded from " + GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + " (E), BLBarcode = " + m_szBCName;
	}
	else
	{
		szText1.Format("S%d, B%d", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		szLog = "BL FilmFrame is loaded from " + GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 +  ", BLBarcode =" + m_szBCName;

		//Set SECSGEM Value
		ULONG ulBindieCount = 0;
		IPC_CServiceMessage stMsg;
		LONG lBinNo = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];

		CString szCassettePos =	GetCassettePositionName(lMgzn);
		SendBinCasseteUnloadWIPEvent_8015(m_lCurrMgzn, m_lCurrSlot, m_szBCName, szCassettePos);
	}

	SetStatusMessage(szLog);
    BL_DEBUGBOX(szLog);

	szBinBlkGrade.Format("%d", lLoadBlock);
	CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Load Frame", GetMagazineName(lMgzn).MakeUpper(), 
		lSlot+1, m_szBCName, szBinBlkGrade, GetBinBlkBondedCount(lSlot + 1), m_bUseBarcode);

	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData on LoadFrame !!!");
	}
	END_CATCH

	szLog.Format("U.Buffer = %d, L.Buffer = %d, Current = %d", 
				m_clUpperGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBlock(), m_lBTCurrentBlock);
	BL_DEBUGBOX(szLog);
    BL_DEBUGBOX("Load from Mgn to Buffer finish\n");
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////
//	UDB_LoadFromBufferToTable()
///////////////////////////////////////////////////////////////////////////////////
LONG CBinLoader::UDB_LoadFromBufferToTable(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, BOOL bOffline,
										   LONG lTransferWIPSlotMode)
{
	LONG lFrameOnBT		= 0;
	BOOL bUseEmpty		= FALSE;
	CString szLog;
	CString szText1;

	m_lCurrMgzn = pGripperBuffer->GetMgznNo();
	m_lCurrSlot = pGripperBuffer->GetSlotNo();
	
    BL_DEBUGBOX("UDB - LOAD from Buffer to Table start");

	// check frame on bin table logically
    if ((m_lBTCurrentBlock != 0) && (bBurnInLoad == FALSE) )
    {
		if( BL_YES_NO(HMB_BL_LOGICALBLK_NOT_ZERO, IDS_BL_LOAD_FRAME)==FALSE )
		{	
			SetErrorMessage("BL Current logical block is not zero");
			return FALSE;
		}
    }

    lFrameOnBT = CheckFrameOnBinTable();
	if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
    {
		lFrameOnBT = BL_FRAME_NOT_EXIST;
    }
	
	if( lFrameOnBT != BL_FRAME_NOT_EXIST )
    {
		if ( BL_OK_CANCEL(HMB_BL_REMOVE_BT_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
        {
			SetErrorMessage("BL frame exist on table");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_ON_TABLE);
			return FALSE;
        }
		else
		{
			/*if (CheckFrameOnBinTable() != BL_FRAME_NOT_EXIST)	
			{
				SetErrorMessage("BL frame exist on table when checking again");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_ON_TABLE);
				return FALSE;
			}*/
		}
    }

	// check frame on Upper/Lower Buffer table logically
	if (!(IsAutoLoadUnloadQCTest() && ((m_lCurrMgzn == BL_MGZ_TOP_1) || (m_lCurrMgzn == BL_MGZ_TOP_2) || 
		 (IsMSAutoLineStandloneMode() && (m_lCurrMgzn == BL_MGZ_MID_2)))) && 
		!pGripperBuffer->IsBufferBlockExist())
	{
		szLog.Format("BL Current %s logical block is zero (%ld)", pGripperBuffer->GetName(), pGripperBuffer->GetBufferBlock());
		SetErrorMessage(szLog);
		SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_FRAME);
		//HmiMessage_Red_Back(szLog, "UDB");
		return FALSE;
	}

	//Gripper Move To Ready Position to check the frame exist or not
	LONG lGripperStatus = GripperMoveToReadyPosn(pGripperBuffer, SFM_WAIT);
	if (lGripperStatus == FALSE )
	{
		szLog.Format("UDB_LoadFromBufferToTable: After %s Gripper UNLOAD fail", pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		GripperSync(pGripperBuffer);
		HomeGripper(pGripperBuffer);
		return FALSE;
	}

	// check frame on Upper/Lower Buffer table physically
	if (!(bBurnInLoad || m_bNoSensorCheck || m_bDisableBL) &&
		!(IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()) || IsGripperFrameExist(pGripperBuffer->GetFrameExistSensorName())))
	{
		szLog.Format("BL No Frame is detected in %s Buffer slot", pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		if ( 1/*BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE*/ )
		{
			szLog.Format("BL No Frame is detected in %s Buffer slot -> abort", pGripperBuffer->GetName());
			SetErrorMessage(szLog);
			SetAlert_Red_Yellow(pGripperBuffer->IsUpperBuffer() ? IDS_BL_FRMAE_NOT_DETECTED_IN_U_BUFFER : IDS_BL_FRMAE_NOT_DETECTED_IN_L_BUFFER);
			return FALSE;
		}
	}

	//update the magazine and slot to match with frame to be loaded to table
	if (!IsMSAutoLineMode())
	{
		if ((GetLoadMgzSlot(bUseEmpty, pGripperBuffer->GetBufferBlock()) == FALSE) && (bBurnInLoad == FALSE) )
		{
			CString szMsg;
			szMsg.Format("\n%d", pGripperBuffer->GetBufferBlock());
			//SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
			SetErrorMessage("BL No slot is assigned to this physical block");
			return FALSE;
		}
	}

	if (!GripperSyncMove_Auto(pGripperBuffer))
	{
		return FALSE;
	}

    //if ( CheckElevatorCover() == FALSE )
    //{
	//	return FALSE;
    //}

	/*
	if (!bOffline)
	{
		BL_DEBUGBOX("BL Z to READY (AUTO)");
		if (!UDB_DownElevatorToReady(pGripperBuffer, FALSE))
		{
			reutrn FALSE;
		}
	}
	else
*/
	{
		BL_DEBUGBOX("BT move to UNLOAD (OFFLINE)");
/*
		if (!IsBTFramePlatformDown())
		{
			szLog = _T("Bin Table Frame level at UP level");
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szLog);		
			SetErrorMessage("UDB_LoadFromBufferToTable: check BT Frame Level fail");
			return FALSE;
		}
*/
		if (!UDB_DownElevatorToReady(pGripperBuffer, FALSE))
		{
			return FALSE;
		}

		LONG lBTUnloadPos_Y = m_lBTUnloadPos_Y;
		if ((m_lBTLoadOffset_Y != 0) && (labs(m_lBTLoadOffset_Y) < 1000))	
		{
			lBTUnloadPos_Y += m_lBTLoadOffset_Y;
		}

		if (MoveBinTable(m_lBTUnloadPos_X, lBTUnloadPos_Y) == FALSE)
		{
			szLog.Format("UDB_LoadFromBufferToTable: MoveBinTable (OrgUnloadY=%d, UnloadY=%d)", m_lBTUnloadPos_Y, lBTUnloadPos_Y);
			SetErrorMessage(szLog);
			return FALSE;
		}

//		SetFrameAlign(FALSE);
//		Sleep(500);

		BTFramePlatformUp();
	}
	
	if (Z_Sync() == gnNOTOK)
	{
		return FALSE;
	}
	if (Theta_Sync() == gnNOTOK)
	{
		return FALSE;
	}
	Sleep(100);

	//Log Encoder
	GetEncoderValue();
	szLog.Format("%s = Theta(cmd = %d, Enc = %d), Z(cmd = %d, Enc = %d)", "UDB_DownElevatorToReady", m_lReadyPos_T, m_lEnc_T, m_lReadyLvlZ, m_lEnc_Z);
	SetStatusMessage(szLog);

	//=======================================================================================
	//if the Missing Step of Binloader theta is more than 10 count, need do home.
	if (abs(m_lReadyPos_T - m_lEnc_T) > m_lBIN_THETA_MISSING_STEP_TOL)
	{
		Theta_Home();
		if (!UDB_DownElevatorToReady(pGripperBuffer, TRUE))
		{
			return FALSE;
		}
	}
	//=====================================================================================

	BL_DEBUGBOX("Gripper to UNLOAD towards BT");
	LONG lUnloadPos_X = GetUnloadPosn(pGripperBuffer);
	lGripperStatus = GripperMoveSearchJam(pGripperBuffer, lUnloadPos_X);
	if (lGripperStatus == FALSE)
	{
		SetGripperPower(pGripperBuffer, FALSE);
		SetGripperState(pGripperBuffer, FALSE);

		BOOL bOk = BinLoaderUnloadJamMessage_UDB(bBurnInLoad, pGripperBuffer);
		if (bBurnInLoad == FALSE && !m_bDisableBL && bOk == FALSE)
		{
			SetErrorMessage("UDB_LoadFromBufferToTable: UNLOAD fail to ABORT");
			HomeGripper(pGripperBuffer);
			Sleep(1000);
			BTFramePlatformDown();
			return FALSE;
		}
		else
		{
			BL_DEBUGBOX("Gripper HOME because of JAM");
			HomeGripper(pGripperBuffer);
		}
	}


	if (!GripperSyncMove_Auto(pGripperBuffer))
	{
		return FALSE;
	}

	if (pGripperBuffer->IsGripperClampStatusOn())
	{
		SetGripperState(pGripperBuffer, FALSE);
		Sleep(200);
	}
		
	BL_DEBUGBOX("Gripper move to READY");
	//lGripperStatus = GripperMoveToReadyPosn(pGripperBuffer, SFM_NOWAIT);
	lGripperStatus = GripperMoveToPreUnloadPosn(pGripperBuffer, SFM_NOWAIT);
	if (lGripperStatus == FALSE)
	{
		szLog.Format("UDB_LoadFromBufferToTable: After %s Gripper UNLOAD fail", pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		GripperSync(pGripperBuffer);
		HomeGripper(pGripperBuffer);
		return FALSE;
	}
	Sleep(200);//2019.05.22
	///////////////////////////////////////////////////
	// Move DOWN BT level before checking frame EXIST
	///////////////////////////////////////////////////
	BTFramePlatformDown();	// down bin table first if necessary
	if (!IsBTFramePlatformDown())
	{
		SetErrorMessage("check frame level error");
		BL_DEBUGBOX("check frame level error");
		CString szTemp = "UDB LoadfromBufferToTable";
		SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szTemp);		
		//HouseKeeping(0);
		return FALSE;
	}

	SetFrameVacuum(TRUE);
	if (m_bUseBinTableVacuum/* && (m_lBTVacuumDelay > 0)*/)	
	{
		Sleep(max(m_lBTVacuumDelay, 200));	//Used AFTER Vac is applied, before BT motion
	}
	
	if (!GripperSyncMove_Auto(pGripperBuffer))
	{
		return FALSE;
	}

	///////////////////////////////////////////////////
	// Detect frame on BT after LOAD
	///////////////////////////////////////////////////
	BL_DEBUGBOX("check frame on bin table");
    lFrameOnBT = CheckFrameOnBinTable();
	if ((bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL))
	{
		lFrameOnBT = BL_FRAME_ON_CENTER;
	}

	if (lFrameOnBT != BL_FRAME_ON_CENTER)
    {
		SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
		SetErrorMessage("No Frame exists on Bin Table");
		//HouseKeeping(0);
        return FALSE;
    }
	
	m_lBTCurrentBlock = pGripperBuffer->GetBufferBlock();
	m_szBinFrameBarcode = pGripperBuffer->GetBufferBarcode();
	UpdateBTCurrMgznSlot(pGripperBuffer->GetMgznNo(), pGripperBuffer->GetSlotNo());

	CString szMsg;
	szMsg.Format("updated to %s block %d", (const char*)pGripperBuffer->GetName(), m_lBTCurrentBlock);
	BL_DEBUGBOX(szMsg);

	UpdateExArmMgzSlotNum(m_lBTCurrentBlock);	//Update slot number on menu
	ResetFrameIsAligned(m_lBTCurrentBlock);		

	m_szBCName = pGripperBuffer->GetBufferBarcode();
	//Send bin table Load event
	CString szCassettePos =	GetCassettePositionName(m_lCurrMgzn);
	SendBinTableLoadEvent_8010(m_lCurrMgzn, m_lCurrSlot, m_szBCName, szCassettePos, "MT");

	if (!IsMSAutoLineMode() && pGripperBuffer->IsFromEmptyFrame())  
	{
		m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE;
	}

	BOOL bRet = TRUE;
	//===================================================================================
	//1. if the frame was loaded into table, it should be set the slot as EMPTY, for RGV to load frame
	//find a output slot and move the input&WIP slot information to this output slot.
	//2. when transfer from the WIP slot to the output slot, it should not change the slot status.
	if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode()/* && IsInputSlot_AutoLine(m_lCurrMgzn, m_lCurrSlot)*/)
	{
		if (lTransferWIPSlotMode)
		{
			//Transfer frame from WIP to output slot, and change the status as SORTING
			m_stMgznRT[m_lCurrMgzn].m_lTransferSlotUsage[m_lCurrSlot] = m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot];
			SetCassetteSlotUsage(m_lCurrMgzn, m_lCurrSlot, BL_SLOT_USAGE_SORTING);
			if (!CreateWIPOutputFileWithBarcode(m_lCurrMgzn, m_lCurrSlot))
			{
				return FALSE;
			}
		}
		else
		{
			LONG lOutputMgzn = 0, lOutputSlot = 0;
			BOOL bLoadFromInputSlot = IsInputSlot_AutoLine(m_lCurrMgzn, m_lCurrSlot);
/*
			if (!bLoadFromInputSlot)
			{
				//Load frame from WIP and sorting, and change the status as SORTING
				SetCassetteSlotUsage(m_lCurrMgzn, m_lCurrSlot, BL_SLOT_USAGE_SORTING);
			}
			else 
*/
			if (IsWIPActiveSlot_AutoLine(m_szBCName, m_lBTCurrentBlock, m_lCurrMgzn, m_lCurrSlot))
			{
				//Load frame from WIP and sorting, and change the status as SORTING
				SetCassetteSlotUsage(m_lCurrMgzn, m_lCurrSlot, BL_SLOT_USAGE_SORTING);
			}
			else if (GetEmptyOutputWIPSlot_AutoLine(m_szBCName, m_lBTCurrentBlock, lOutputMgzn, lOutputSlot))
			{
				//Becasue the grade of input is ZERO.
				SetCassetteSlotGradeBlock(m_lCurrMgzn, m_lCurrSlot, m_lBTCurrentBlock);

				TransferBondingMgznSlot_AutoLine(m_lCurrMgzn, m_lCurrSlot, lOutputMgzn, lOutputSlot);
				UpdateBTCurrMgznSlot(lOutputMgzn, lOutputSlot);
				//Is WIP Slot, create a temporary file
				if (bLoadFromInputSlot && 
					(GetCassetteSlotGradeBlock(m_lCurrMgzn, m_lCurrSlot) > 0) && 
					(GetCassetteSlotWIPCounter(m_lCurrMgzn, m_lCurrSlot) > 0))
				{
					if (!CreateWIPTempFile(m_lCurrMgzn, m_lCurrSlot))
					{
						return FALSE;
					}
				}
			}
			else
			{
				//how to handle this error, need study it...
				SetAlert_Red_Yellow(IDS_BL_MA_NOEPYFRAME_IN_EPYMAG);
				SetErrorMessage("BL error: No Empty frame in EMPTY magazine (AUTOLINE)");
				szMsg.Format("GetEmptyOutputWIPSlot_AutoLine failure(Mgzn = %d, slot = %d", m_lCurrMgzn, m_lCurrSlot);
				BL_DEBUGBOX(szMsg);
				bRet = FALSE;
			}
		}
	}

	if(bUseEmpty)
	{
		GenerateEmptyFrameTextFile();
	}

	//===================================================================================
	pGripperBuffer->InitBufferInfo();
	pGripperBuffer->SetMgznSlotNo(0, 0); //init
	pGripperBuffer->SetBinFull(FALSE);
	pGripperBuffer->SetFromEmptyFrame(FALSE);

	(*m_psmfSRam)["BinLoader"]["CurrMgzn"]		= m_lCurrMgzn;
	(*m_psmfSRam)["BinLoader"]["CurrSlot"]		= m_lCurrSlot;

	GenerateOMRTTableFile(m_lSelMagazineID);
	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

 	TRY {
		SaveMgznRTData();	
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH


	if (!bRet)
	{
		return FALSE;
	}
	///////////////////////////////////////////////////
	// OFFLINE or AUTOBOND mode ?
	///////////////////////////////////////////////////

	if ( bOffline == TRUE )
	{
		BL_DEBUGBOX("BT to Home");
		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= FALSE;

		if (MoveBinTable(0, 0, 0, FALSE) == FALSE)
			return FALSE;
	}
	else
	{
		//For BT T barcode scanning	//v4.40T5	//Nichia MS100+
		(*m_psmfSRam)["BinLoader"]["BCScanRange"]	= m_lScanRange;
		(*m_psmfSRam)["BinLoader"]["BCRetryLimit"]	= m_lTryLimits;

		//Move to Block 1 each time
		IPC_CServiceMessage svMsg;
		int nConvID = 0;
		ULONG ulBlkNo = (ULONG)m_lBTCurrentBlock;

		svMsg.InitMessage(sizeof(ULONG), &ulBlkNo);
		if (m_bRealignBinFrame)
		{
/*
			//BL_DEBUGBOX("BT to 1st die for alignment");
			BL_DEBUGBOX("BT to last die (Realign)");

			//BT Realign Frame Option: 0=DEFAULT(3-pt); 1=1-pt; 2=2-pt
			LONG lRealignFrameOption = (LONG)(*m_psmfSRam)["BinTable"]["RealignFrameOption"];	//v3.71T5

			if (lRealignFrameOption == 1)	//1-pt realignment
			{
				nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinBlk", svMsg);		//Move to last-die
			}
			else							//default = 3-pt realignment
			{
				nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinBlk1stDie", svMsg);	//MOVE to UL die
			}
*/
		}
		else
		{
			BL_DEBUGBOX("BT to last die");
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToBinBlk", svMsg);			//Move directly to last-die
		}
		
		if (nConvID > 0)
		{
			BOOL bMoveStatus = TRUE;
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, svMsg);
					svMsg.GetMsg(sizeof(BOOL), &bMoveStatus);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (!bMoveStatus)
			{
				SetErrorMessage("UDB LoadFromBufferToTable fail due to Realignment failure");
				return FALSE;
			}
		}

		//Re-check frame on table after BT MOVE
		BL_DEBUGBOX("check frame on bin table 2");

		lFrameOnBT = CheckFrameOnBinTable();
		if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
		{
			lFrameOnBT = BL_FRAME_ON_CENTER;
		}
		if (lFrameOnBT != BL_FRAME_ON_CENTER)
		{
			Sleep(50);
			lFrameOnBT = CheckFrameOnBinTable();
			if (lFrameOnBT != BL_FRAME_ON_CENTER)
			{
				SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
				SetErrorMessage("No Frame exists on Bin Table 2");
				return FALSE;
			}
		}

		//========================================SetAlignBinMode===============================
		BOOL bTemp;
		IPC_CServiceMessage stMsg;

		bTemp = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bTemp);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetAlignBinMode", stMsg);
		m_comClient.ScanReplyForConvID(nConvID, 30000);
		m_comClient.ReadReplyForConvID(nConvID, stMsg);
		//======================================================================================

		//=======================================================================================
		//  Realign Bin
		//======================================================================================
		//Allow RealignBinframe in BURNIN mode
		BL_DEBUGBOX(bUseEmpty ? "Re-align EMPTY frame start" : "Re-align bin frame start");

		//must sync first! Otherwise bond arm may crash with gripper!
		if (GripperSync(pGripperBuffer) == gnNOTOK)
		{
			HomeGripper(pGripperBuffer);
		}

		BOOL bRtn = RealignBinFrameReq(m_lBTCurrentBlock, bUseEmpty);
		if (bRtn == TRUE)
		{
			BL_DEBUGBOX("Re-align EMPTY frame - sent");
			bRtn = RealignBinFrameRpy(FALSE, TRUE);
			BL_DEBUGBOX(bRtn ? "Re-align EMPTY frame - Done" : "Re-align EMPTY frame - fail");
		}
		else
		{
			BL_DEBUGBOX("Re-align EMPTY frame - failed");
		}

		szText1.Format("U.Buffer = %d, L.Buffer = %d, Current = %d",
						m_clUpperGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBlock(), m_lBTCurrentBlock);
		BL_DEBUGBOX(szText1);
		BL_DEBUGBOX("Load from Buffer to Table finish\n");
		return bRtn;
	}
	
	szText1.Format("U.Buffer = %d, L.Buffer = %d, Current = %d", 
					m_clUpperGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBlock(), m_lBTCurrentBlock);
	BL_DEBUGBOX(szText1);
    BL_DEBUGBOX("UDB Load from Buffer to Table finish\n");

	BackupToNVRAM();
	return TRUE;
}


LONG CBinLoader::UDB_MgznSlotToMgznSlot(CBinGripperBuffer *pGripperBuffer, const LONG lSourceMgznNo, const LONG lSourceSlotNo,
										const LONG lTargetMgznNo, const LONG lTargetSlotNo, const BOOL bReScanBarcodeOnly)
{
	CString szBCName = m_stMgznRT[lSourceMgznNo].m_SlotBCName[lSourceSlotNo];
	LONG lLoadBlock = m_stMgznRT[lSourceMgznNo].m_lSlotBlock[lSourceSlotNo];
	LONG lSlotUsage = m_stMgznRT[lSourceMgznNo].m_lSlotUsage[lSourceSlotNo];
	
	CString szMsg;
	szMsg.Format("Mgzn Slot To Mgzn Slot,Source(%d,%d,usage,%d),Target(%d,%d)",lSourceMgznNo,lSourceSlotNo,lSlotUsage,lTargetMgznNo,lTargetSlotNo);
	BL_DEBUGBOX(szMsg);
	if (szBCName.IsEmpty() && (lLoadBlock == 0) && (lSlotUsage == BL_SLOT_USAGE_EMPTY))
	{
		return TRUE;
	}

	//Load a frame from soure (lSourceMgznNo, lSourceSlotNo)
	LONG lRet = UDB_LoadFromMgzSlotToBuffer(FALSE, lLoadBlock, lSourceMgznNo, lSourceSlotNo, FALSE, bReScanBarcodeOnly, pGripperBuffer);
	if (lRet != TRUE)
	{
		return lRet;
	}

	if (MoveElevatorToUnload(lTargetMgznNo, lTargetSlotNo, pGripperBuffer->IsUpperBuffer(), TRUE, TRUE) == FALSE)
	{
		CString szLog;
		szLog.Format("UNLOAD: Fail to move BUFFER table for #%d", pGripperBuffer->GetBufferBlock());
		HmiMessage_Red_Back(szLog);		
		SetErrorMessage(szLog);
		return FALSE;
	}

	//Get the Magzine & Slot Number
	TransferMgznSlot_AutoLine(lSourceMgznNo, lSourceSlotNo, lTargetMgznNo, lTargetSlotNo);

	Sleep(100);

	BOOL bRealignOutputSlot = FALSE;
	return UDB_UnloadFromBufferToMgzSlot(FALSE, lLoadBlock, FALSE, lTargetMgznNo, lTargetSlotNo, pGripperBuffer, FALSE, FALSE, FALSE, FALSE, bRealignOutputSlot);
}

//================================================================================================
//					Barcode Reader Related Function
//================================================================================================

//================================================================================================
// Function Name: 		UDB_ReadBarCodeAtUnload
// Input arguments:		1. bBurnIn
//						2. pGripperBuffer
// Output arguments:	None
// Description:   		Read Barcode at Load/Unload frame
// Return:				TRUE -- Read a barcode 
//						FALSE -- Can not read and error handling
// Remarks:				None
//================================================================================================
LONG CBinLoader::UDB_ReadBarCodeAtUnload(BOOL bBinFull, BOOL bBurnIn, CBinGripperBuffer *pGripperBuffer)
{
	LONG nErr = UDB_ReadBarCode(FALSE, TRUE, bBurnIn, pGripperBuffer);
	
	if (nErr == TRUE)
	{
		//if scanning barcode is not matached with the barcode of  slot,
		//machine will read it again
		if (!GripperRescanningUnloadBarCode(pGripperBuffer))
		{
			pGripperBuffer->SetBufferBarcode("");
			return FALSE;
		}

		//Check barcode whether they are belong to the characters or not?
		BOOL bManualKeyIn = FALSE;
		if (!CheckBarcodeCharacters(m_szBCName))
		{
			bManualKeyIn = TRUE;
		}

		if ((m_bCompareBarcode || IsMSAutoLineMode()) && (m_szBCName == BL_DEFAULT_BARCODE || m_szBCName == "" || bManualKeyIn))
		{
			CString szText = "\n";
			//AlertMsgForManualInputBarcode(GetCurrBinOnBT(), m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
			AlertMsgForManualInputBarcode_NoSaveToRecord(m_szBCName, szText);
			BL_DEBUGBOX("Mannual input barcode done - new = " + m_szBCName);
		}

		if (!CheckUnloadBarCodeValid(pGripperBuffer, bBinFull))
		{
			pGripperBuffer->SetBufferBarcode("");
			return FALSE;
		}
		pGripperBuffer->SetBufferBarcode(m_szBCName);
		m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
		return TRUE;
	}

	return nErr;
}

BOOL CBinLoader::GripperRescanningUnloadBarCode(CBinGripperBuffer *pGripperBuffer)
{
	BOOL bRet = TRUE;
	if (GetCurrMgznSlotBC() != m_szBCName)
	{
		CString szOldBC		= m_szBCName;
		CString szSlotBC	= GetCurrMgznSlotBC();

		BL_DEBUGBOX("#3 Rescan barcode - curr = " + m_szBCName);
		bRet = GripperScanningBarcode(pGripperBuffer, FALSE, TRUE);
		BL_DEBUGBOX("#3 Rescan barcode done - new = " + m_szBCName);

		CString szErr;
		szErr.Format("RESCAN BC: MGZN #%ld, SLOT #%ld - Slot BC = " + szSlotBC, m_lCurrMgzn, m_lCurrSlot + 1);
		szErr = szErr + "; Scan-BC = " + szOldBC + "; ReScan-BC = " + m_szBCName;
		CMSLogFileUtility::Instance()->BL_BarcodeLog(szErr);
	}
	return bRet;
}


BOOL CBinLoader::CheckUnloadBarCodeValid(CBinGripperBuffer *pGripperBuffer, BOOL bBinFull)
{
	if (GetCurrMgznSlotBC() != m_szBCName)
	{
		if (!m_bCompareBarcode && !IsMSAutoLineMode())
		{
			m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
			SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
			return TRUE;
		}

		CString szText;	
		LONG lOption;
		//v3.71T3	//PLLM special feature

		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
		ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);

		if (pApp->GetProductLine() == "Lumiramic")		//v4.35T1
		{
			bPLLMSpecialFcn = TRUE;    //Lumiramic Dual-DLA also uses REBEL sequence!!		//v4.02T4
		}		

		if (pApp->IsBLBCUseOldGoStop())
		{
			bPLLMSpecialFcn = TRUE;
		}

		szText = "#6 Unload Bin Frame\n";
		szText += "Old: [@@" + GetCurrMgznSlotBC() + "@@]\nNew: [@@" + m_szBCName + "@@]";

		if (bPLLMSpecialFcn)	
		{
			if ((m_szBCName == "") || (m_szBCName == BL_DEFAULT_BARCODE))	//v3.71T7
			{
				szText = "Output frame barcode cannot be read!\n";
				szText += "old label: " + GetCurrMgznSlotBC();
			}
			else
			{
				szText = "Barcode read on output frame is different from\n";
				szText += "old label: " + GetCurrMgznSlotBC();
			}
			lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "CONTINUE", "STOP");
		}
		else
		{
			CString szMsg;
			szMsg = szText;
			szMsg.Replace('\n', ' ');
			SetStatusMessage(szMsg);
			lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Old", "New", NULL, glHMI_ALIGN_LEFT);		
		}

		BL_DEBUGBOX(szText);
		switch (lOption)
		{
			case -1:
				BL_DEBUGBOX("UNLOAD: barcode comparison fails -> abort ");
				SetStatusMessage("DLUL Compare barcode fail: Abort");
				HouseKeeping(m_lPreUnloadPos_X);
				m_szBCName = GetCurrMgznSlotBC();
				SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
				return FALSE;

			case 1:		//OLD or CONTINUE 
				BL_DEBUGBOX("UNLOAD: barcode comparison fails -> use old " + GetCurrMgznSlotBC());	
				if (bPLLMSpecialFcn)	//v3.71T3
				{
					SaveBarcodeData(GetCurrBinOnBT(), GetCurrMgznSlotBC());
				}
				else
				{
					//v3.93
					m_szBCName = GetCurrMgznSlotBC();
					SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
							
					SetStatusMessage("DLUL Compare barcode fail: Use old barcode -> " + m_szBCName);
					BL_DEBUGBOX("UNLOAD: barcode comparison fails -> use OLD: " + GetCurrMgznSlotBC() + " (" + m_szBCName + ")");		//v2.93T2
					SetErrorMessage("UNLOAD: barcode comparison fails -> use OLD: " + GetCurrMgznSlotBC() + " (" + m_szBCName + ")");	//v3.65
				}
				break;

			case 2:		//NEW or STOP
				if (bPLLMSpecialFcn)
				{
					BL_DEBUGBOX("UNLOAD: barcode comparison fails ABORT -> use old " + GetCurrMgznSlotBC());	
					SaveBarcodeData(GetCurrBinOnBT(), GetCurrMgznSlotBC());
				}
				else
				{
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
					SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
					//v3.93
					SetStatusMessage("DLUL Compare barcode fail: Use new barcode -> " + m_szBCName);
					BL_DEBUGBOX("UNLOAD: barcode comparison fails -> use NEW: " + m_szBCName + " (" + GetCurrMgznSlotBC() + ")");	
					SetErrorMessage("UNLOAD: barcode comparison fails -> use NEW: " + m_szBCName + " (" + GetCurrMgznSlotBC() + ")");
				}
				break;
		}
	}
	else if ((bBinFull == TRUE) && (GetCurrMgznSlotBC() == BL_DEFAULT_BARCODE) && (m_szBCName == BL_DEFAULT_BARCODE))
	{
		CString szText = "\n";
		AlertMsgForManualInputBarcode(GetCurrBinOnBT(), m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
		//v4.13T6	//BugFix: after user input above BC should save into memory & MSD file
		m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
		SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
	}
	else
	{
		m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
		SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
	}
	return TRUE;
}



//================================================================================================
// Function Name: 		UDB_ReadBarCodeAtLoad
// Input arguments:		1. bBurnIn
//						2. pGripperBuffer
// Output arguments:	None
// Description:   		Read Barcode at Load/Unload frame
// Return:				TRUE -- Read a barcode 
//						FALSE -- Can not read and error handling
// Remarks:				None
//================================================================================================
LONG CBinLoader::UDB_ReadBarCodeAtLoad(const BOOL bLoadCompareBarcode, const BOOL bBurnIn, CBinGripperBuffer *pGripperBuffer, const LONG lLoadBlock)
{
	LONG nErr = UDB_ReadBarCode(TRUE, bLoadCompareBarcode, bBurnIn, pGripperBuffer);

	if (nErr == TRUE)
	{
		CString szCurrBarcode = _T("");
		szCurrBarcode = m_szBCName;
		if (!bLoadCompareBarcode)
		{
			m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = szCurrBarcode;
		}
		
		if (m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] == szCurrBarcode)
		{
			pGripperBuffer->SetBufferBarcode(szCurrBarcode);
			SaveBarcodeData(lLoadBlock, m_szBCName, m_lCurrMgzn, m_lCurrSlot);	
			return TRUE;
		}

		if (GripperPushBackFrame(pGripperBuffer, TRUE) != TRUE)
		{
			return FALSE;
		}
		return FALSE;
	}
	return nErr;
}


//================================================================================================
// Function Name: 		UDB_ReadBarCode
// Input arguments:		1. BloadUnload  Load frame -- TRUE  unload Frame -- FALSE, 
//						2. bBurnIn
//						3. pGripperBuffer
// Output arguments:	None
// Description:   		Read Barcode at Load/Unload frame
// Return:				TRUE -- Read a barcode 
//						FALSE -- Can not read and error handling
// Remarks:				None
//================================================================================================
LONG CBinLoader::UDB_ReadBarCode(const BOOL bLoadUnload, const BOOL bLoadCompareBarcode, const BOOL bBurnIn, CBinGripperBuffer *pGripperBuffer)
{
	LONG nErr = 0;
	///////////////////////////////////////////////////
	// Move Gripper to BARCODE position ...
	///////////////////////////////////////////////////
	LONG lBarcodePosn = GetBarcodePosn(pGripperBuffer);
	if ((nErr = GripperMoveTo(pGripperBuffer, lBarcodePosn, FALSE)) != TRUE)	
	{
		CString szLog;
		szLog.Format("UDB: %s Gripper MOVE failure (m_lBarcodePos_X) in LOAD", (const char*)pGripperBuffer->GetName());
		SetErrorMessage(szLog);
		return nErr;	
	}

	if (!bBurnIn && !IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))
	{
		BOOL bExist = FALSE;
		if (!m_bNoSensorCheck && IsGripperFrameExist(pGripperBuffer->GetFrameExistSensorName()))
		{
			bExist = TRUE;		
		}
		else if ( bBurnIn || m_bNoSensorCheck )	
		{
			bExist = TRUE;		
		}

		//search frame again if it is detected in buffer
		if (bExist)
		{
			SetGripperState(pGripperBuffer, FALSE);
			Sleep(500);
			GripperSearchInClamp(pGripperBuffer, HP_POSITIVE_DIR, 1000, 400, TRUE);
			Sleep(200);

			if (!bBurnIn && !IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))	
			{
				SetGripperState(pGripperBuffer, FALSE);

				LONG lRet = 0/*BL_YES_NO(HMB_BL_AUTO_PUSH_FRAME_BACK_TO_MGZ, IDS_BL_LOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg)*/;
				if (!lRet)
				{	
					//No PushBack and ABORT
					HmiMessage_Red_Yellow("Frame in clamp detect failure,Abort!");
					HomeGripper(pGripperBuffer);
					pGripperBuffer->SetBufferBlock(0);
				}
				else
				{
					/////////////////////////////////////////////////////////////////////////////////
					// Push back frame and ABORT if read barcode failure occurs
					/////////////////////////////////////////////////////////////////////////////////
					LONG lPosnX = bLoadUnload ? GetUnloadMagPosn(pGripperBuffer) + pGripperBuffer->GetUnloadMagClampOffsetX() : GetUnloadPosn(pGripperBuffer);
					if (GripperPushBackFrame(pGripperBuffer, lPosnX) == TRUE)
					{
						if (!bLoadUnload)
						{
							//Close Bin Table
							BTFramePlatformDown();	// down bin table first if necessary
						}
					}
				}
				return FALSE;
			}
			else
			{
				BL_DEBUGBOX("Retry scan barcode");

				SetGripperState(pGripperBuffer, TRUE);
				Sleep(800);	
					
				CString szLog;
				szLog.Format("%s gripper move to barcode pos", pGripperBuffer->GetName());
				BL_DEBUGBOX(szLog);

				if ((nErr = GripperMoveTo(pGripperBuffer, lBarcodePosn)) != TRUE)
				{
					SetGripperState(pGripperBuffer, FALSE);
					Sleep(200);
					HomeGripper(pGripperBuffer);
					pGripperBuffer->SetBufferBlock(0);
					return nErr;	
				}
			}
		}
		else
		{
			//SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);		
			SetFrameNotDetectedAlarm(pGripperBuffer, IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_SCAN_BARCODE);
			if (bLoadUnload)
			{
				HmiMessage_Red_Yellow("Frame will push back to slot by gripper ...");
			}
			else
			{
				HmiMessage_Red_Yellow("Frame will push back to bin table by gripper ...");
			}
			
			/////////////////////////////////////////////////////////////////////////////////
			// Push back frame and ABORT if read barcode failure occurs
			/////////////////////////////////////////////////////////////////////////////////
			LONG lPosnX = bLoadUnload ? GetUnloadMagPosn(pGripperBuffer) + pGripperBuffer->GetUnloadMagClampOffsetX() : GetUnloadPosn(pGripperBuffer);
			if (GripperPushBackFrame(pGripperBuffer, lPosnX) == TRUE)
			{
				if (!bLoadUnload)
				{
					//Close Bin Table
					BTFramePlatformDown();	// down bin table first if necessary
				}
			}
			
			//return IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER;

			if (pGripperBuffer->IsUpperBuffer() == TRUE)
			{
				return IDS_BGP_FRAME_NOT_DETECTED_IN_UP_GRIPPER_IN_SCAN_BARCODE;
			}
			else
			{
				return IDS_BGP_FRAME_NOT_DETECTED_IN_LOW_GRIPPER_IN_SCAN_BARCODE;
			}
		}
	}		//if (!bBurnInLoad && !IsFrameInClamp2())

	return GripperScanningBarcode(pGripperBuffer, bLoadUnload, bLoadCompareBarcode);
}


// if read the barcode of the frame failure in WIP, how to handle it 2018.4.27?
BOOL CBinLoader::GripperScanningBarcode(CBinGripperBuffer *pGripperBuffer, const BOOL bLoadUnload, const BOOL bLoadCompareBarcode)
{
	///////////////////////////////////////////////////
	// Scan barcode by gripper ...
	///////////////////////////////////////////////////
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableScanTwiceFcn = pApp->GetFeatureStatus(MS896A_FUNC_BLBC_EMPTY_SCAN_TWICE);
	BOOL bScanBarcodeOK = TRUE;

	LONG lStatus = ScanningBarcode(pGripperBuffer, bEnableScanTwiceFcn);
	
	if ((lStatus == FALSE) || (lStatus == -1))
	{
		CString szLog;
		if (lStatus == FALSE)
		{
			szLog.Format("No barcode read (%s Gripper)", (const char*)pGripperBuffer->GetName());
		}
		else
		{
			szLog.Format("Read Barcode failure (%s Gripper)", (const char*)pGripperBuffer->GetName());
		}
		BL_DEBUGBOX(szLog);
		bScanBarcodeOK = FALSE;
	}

	CString szSlotBarcode;
	szSlotBarcode = GetCassetteSlotBCName(m_lCurrMgzn, m_lCurrSlot);

	//after read barcode, and this frame is empty if it is not under SIS system. then it do not comapre barcode.
	if (bScanBarcodeOK && !bLoadCompareBarcode)
	{
		szSlotBarcode = m_szBCName;
	}

	if (!IsAutoLoadUnloadQCTest() && bLoadUnload && (!szSlotBarcode.IsEmpty() || IsMSAutoLineMode()))
	{
		if (bScanBarcodeOK || (!bScanBarcodeOK && (lStatus == FALSE)))
		{
			if (bScanBarcodeOK && !m_bCompareBarcode && !IsMSAutoLineMode())
			{
				//Auto-Update the barcode for the current slot if disbale "Compare barcode" option
				bScanBarcodeOK = TRUE;
			}
			else if (m_szBCName != szSlotBarcode)
			{
				if (!m_bCompareBarcode && !IsMSAutoLineMode())
				{
					//if disable "Compare barcode" option
					// set as reject status for the current slot if read barcode failure
					SetSlotRejectStatus(m_lCurrMgzn, m_lCurrSlot, (!bScanBarcodeOK && (lStatus == FALSE)) ? MS_FRAME_NO_BARCODE : MS_FRAME_BARCODE_NOT_MATCHED_SIS, szSlotBarcode);
					bScanBarcodeOK = FALSE;
				}
				else
				{
//					SetAlert_Red_Yellow(IDS_BL_NOT_MATCHED_SIS_BARCODE);
					CSingleLock slLock(&(pApp->m_csMachineAlarmLamp));
					slLock.Lock();
					LONG lAlarmStatus = GetAlarmLamp_Status();
					//Set Assistance time
					SetAlertTime(TRUE, EQUIP_ASSIST_TIME, "Machine Assist");
					SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
					EquipStateProductiveToUnscheduleDown();

					CString szBarCode	= _T("");
					while( 1 )
					{
						szBarCode	= _T("");
						CString szTitle		= "Please input BarCode ID: ";

						BOOL bReturn = HmiStrInputKeyboard(szTitle, szBarCode);
						if (bReturn && !szBarCode.IsEmpty())
						{
							break;
						}
					}

					EquipStateUnscheduleDwonToProductive();
					SetAlarmLamp_Green(FALSE, TRUE);
					//Clear Assistance time
					SetAlertTime(FALSE, EQUIP_ASSIST_TIME, "Machine Assist");
					slLock.Unlock();

					m_szBCName = szBarCode;
					if (m_szBCName != szSlotBarcode)
					{
						// set as reject status for the current slot
						SetSlotRejectStatus(m_lCurrMgzn, m_lCurrSlot, (!bScanBarcodeOK && (lStatus == FALSE)) ? MS_FRAME_NO_BARCODE : MS_FRAME_BARCODE_NOT_MATCHED_SIS, szSlotBarcode);
						bScanBarcodeOK = FALSE;
					}
					else
					{
						bScanBarcodeOK = TRUE;
					}
				}
			}
		}
	}

	if (!bScanBarcodeOK)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// Push back frame and ABORT if read barcode failure occurs
		/////////////////////////////////////////////////////////////////////////////////
		if (GripperPushBackFrame(pGripperBuffer, bLoadUnload) != TRUE)
		{
			return FALSE;
		}
	}

	return bScanBarcodeOK;
}

BOOL CBinLoader::GripperPushBackFrame(CBinGripperBuffer *pGripperBuffer, const BOOL bLoadUnload)
{
	LONG nErr = 0;

	LONG lPosnX = bLoadUnload ? GetUnloadMagPosn(pGripperBuffer) + pGripperBuffer->GetUnloadMagClampOffsetX() : GetUnloadPosn(pGripperBuffer);
	if (GripperPushBackFrame(pGripperBuffer, lPosnX) != TRUE)
	{
		return FALSE;
	}

	if (!bLoadUnload)
	{
		//Close Bin Table
		BTFramePlatformDown();	// down bin table first if necessary
	}

	return TRUE;
}

LONG CBinLoader::GripperPushBackFrame(CBinGripperBuffer *pGripperBuffer, LONG lPosnX)
{
	LONG nErr = 0;

	SetGripperState(pGripperBuffer, FALSE);
	Sleep(500);

	if (GripperMoveSearchJam(pGripperBuffer, lPosnX) == FALSE)
	{
		HomeGripper(pGripperBuffer);
	}
	else
	{
		LONG lReadyPos_X = GetReadyPosn(pGripperBuffer);
		nErr = GripperMoveTo(pGripperBuffer, lReadyPos_X);
	}

	pGripperBuffer->SetBufferBlock(0);

	return nErr;
}



LONG CBinLoader::GetBTCurrentBlock()
{
	return m_lBTCurrentBlock;
}


