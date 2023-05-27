/////////////////////////////////////////////////////////////////
// BH_ChangeCollet.cpp : Event functions of the CBondHead class
//
//	Description:
//		MS Mapping Die Sorter
//
//	Date:		Tue, May 12, 2015
//	Revision:	1.00
//
//	By:			Andrew
//
//	Copyright @ ASM Assembly Automation Ltd., 2015.
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "BH_AirFlowCompensation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////
//	HMI command fcns
////////////////////////////////////////////

VOID CBondHead::RegisterAGCCommand()
{
	try
	{
		// Register variable
		RegVariable(_T("BH_bAutoChangeCollet"),			&CMS896AStn::m_bAutoChangeCollet);  //defined in MSOption.msd
		RegVariable(_T("BH_lAGCClampLevelOffsetZ"),		&m_lAGCClampLevelOffsetZ);
		RegVariable(_T("BH_lAGCUplookPRLevelOffsetZ"),	&m_lAGCUplookPRLevelOffsetZ);
		RegVariable(_T("BH_lAGCHolderUploadZ"),			&m_lAGCHolderUploadZ);
		RegVariable(_T("BH_lAGCHolderInstallZ"),		&m_lAGCHolderInstallZ);
		RegVariable(_T("BH_lAGCHolderBlowDieZ"),		&m_lAGCHolderBlowDieZ);
		RegVariable(_T("BH_lAGCColletStartAngle"),		&m_lAGCColletStartAng);
		RegVariable(_T("BH_lAGCColletIndex"),			&m_lAGCColletIndex);

		//RegVariable(_T("BH_lAGCCollet1OffsetX"),		&m_lAGCCollet1OffsetX);
		//RegVariable(_T("BH_lAGCCollet1OffsetY"),		&m_lAGCCollet1OffsetY);
		//RegVariable(_T("BH_lAGCCollet2OffsetX"),		&m_lAGCCollet2OffsetX);
		//RegVariable(_T("BH_lAGCCollet2OffsetY"),		&m_lAGCCollet2OffsetY);

		// Register command
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SetIndex"),					&CBondHead::AGC_SetIndex);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SetUplookPRLevelOffset"),	&CBondHead::AGC_SetUplookPRLevelOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SetClamp"),					&CBondHead::AGC_SetClamp);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SetPusher"),				&CBondHead::AGC_SetPusher);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SetUploadPusher"),			&CBondHead::AGC_SetUploadPusher);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SetHolderVacuum"),			&CBondHead::AGC_SetHolderVacuum);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SetHolderStrongBlow"),		&CBondHead::AGC_SetHolderStrongBlow);

        m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_DetachCollet"),				&CBondHead::AGC_DetachCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_UploadCollet"),				&CBondHead::AGC_UploadCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_InstallCollet"),			&CBondHead::AGC_InstallCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SearchUploadCollet"),		&CBondHead::AGC_SearchUploadCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_SearchCollet"),				&CBondHead::AGC_SearchCollet);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_MoveCollet"),				&CBondHead::AGC_MoveCollet);		//v4.50A30
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_ResetColletOffset"),		&CBondHead::AGC_ResetColletOffset);	//v4.50A30
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AGC_AutoChangeCollet"),			&CBondHead::AGC_AutoChangeCollet);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeChgColletPosition"),	&CBondHead::BH_ChangeChgColletPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInChgColletPosition"),	&CBondHead::BH_KeyInChgColletPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosChgColletPosition"),	&CBondHead::BH_MovePosChgColletPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegChgColletPosition"),	&CBondHead::BH_MoveNegChgColletPosition);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelChgColletSetup"),			&CBondHead::CancelChgColletSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmChgColletSetup"),		&CBondHead::ConfirmChgColletSetup);

		DisplayMessage("BondHeadStn AGC variables registered ...");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}

BOOL CBondHead::IsEnaAutoChangeCollet()
{
	return (CMS896AStn::m_bAutoChangeCollet && !m_bAutoChangeColletOnOff);
}

LONG CBondHead::AGC_SetIndex(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lTemp = m_lAGCColletIndex;
	if (UserHmiNumericKeys("Input Collet Index", "BH_lAGCColletIndex", 20, 1, &lTemp))
	{
		if ((lTemp < 1) || (lTemp > GetCGMaxIndex()))
		{
			lTemp = 1;
		}

		m_lAGCColletIndex = lTemp;
		MoveChgColletTPitch(m_lAGCColletIndex);
		SaveBhData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_SetUplookPRLevelOffset(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(LONG), &m_lAGCUplookPRLevelOffsetZ);

	return TRUE;
}

//============================================================================
//  Senoid (Clamp, Pusher, Upload Pusher and Holder Vacuum)
//============================================================================
LONG CBondHead::AGC_SetClamp(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSet = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bSet);

	SetChgColletClamp(bSet);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_SetPusher(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSet = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bSet);

	SetChgColletPusher(bSet);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_SetUploadPusher(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSet = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bSet);

	SetChgColletUploadPusher(bSet);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_SetHolderVacuum(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSet = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bSet);

	SetChgColletHolderVacuum(bSet);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CBondHead::AGC_SetHolderStrongBlow(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSet = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bSet);

	SetColletHolderStrongBlow(bSet);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

//===================================================================================

////////////////////////////////////////////
//	Support Fcns
////////////////////////////////////////////

VOID CBondHead::SetChgColletClamp(BOOL bSet)
{
	//No BHT in MegaDa
	return;
	if (m_fHardware == FALSE)
	{
		return;
	}
	if (m_bDisableBH)
	{
		return;
	}
	if (!CMS896AStn::m_bAutoChangeCollet)
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit(MS60_SO_COLLET_CLAMP, bSet);
}

VOID CBondHead::SetChgColletPusher(BOOL bSet)
{
	//No BHT in MegaDa
	return;
	if (m_fHardware == FALSE)
	{
		return;
	}
	if (m_bDisableBH)
	{
		return;
	}
	if (!CMS896AStn::m_bAutoChangeCollet)
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit(MS60_SO_COLLET_PUSHER, bSet);
}


//When get a new collet from the upload module, it need turn on Holder Vacuum and Uplod Pusher
VOID CBondHead::SetChgColletUploadPusher(BOOL bSet)
{
	//No BHT in MegaDa
	return;
	if (m_fHardware == FALSE)
	{
		return;
	}
	if (m_bDisableBH)
	{
		return;
	}
	if (!CMS896AStn::m_bAutoChangeCollet)
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit(MS60_SO_COLLET_UPLOAD_PUSHER, bSet);
}

//When get a new collet from the upload module, it need turn on Holder Vacuum and Uplod Pusher
VOID CBondHead::SetChgColletHolderVacuum(BOOL bSet)
{
	//No BHT in MegaDa
	return;
	if (m_fHardware == FALSE)
	{
		return;
	}
	if (m_bDisableBH)
	{
		return;
	}
	if (!CMS896AStn::m_bAutoChangeCollet)
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit(MS60_SO_COLLET_HOLDER_VACUUM, bSet);
}

//When do nozzle clean, it need turn on Holder strong blow to blow Alcohol
VOID CBondHead::SetColletHolderStrongBlow(BOOL bSet)
{
	//No BHT in MegaDa
	return;
	if (m_fHardware == FALSE)
	{
		return;
	}
	if (m_bDisableBH)
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit(CLEAN_COLLET_SO_HOLDER_STRONG_BLOW, bSet);
}


BOOL CBondHead::IsColletPusherAtSafePos()		//v4.51A17
{
	//No BHT in MegaDa
	return TRUE;

	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBH)
	{
		return TRUE;
	}

	if (!CMS896AStn::m_bAutoChangeCollet)
	{
		return TRUE;
	}

	//Use MS109 CEMark CoverSensor2 (Osram) as this sensor bit;
	// So reuse its name in Machine MSD file;
	BOOL bPusherSafePos = !CMS896AStn::MotionReadInputBit("iCoverSensor2");
	return bPusherSafePos;
}


//================================================================
// Function Name: 		IsBHBTReady
// Input arguments:     None
// Output arguments:	None
// Description:   		Check BH & Bin table are ready or not
// Return:				TRUE/FALSE
// Remarks:				called by other function
//================================================================
BOOL CBondHead::IsBHBTReady(CString &szErrorMess)
{
	//No BHT in MegaDa
	return TRUE;
	if (!IsAllMotorsEnable())
	{
		szErrorMess = _T("fcn is disabled because of Motor OFF");
		return FALSE;
	}

	if (m_bDisableBH)
	{
		szErrorMess = _T("fcn is disabled because m_bDisableBH");
		return TRUE;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		szErrorMess = _T("Wafer Expander not closed");
		return FALSE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bBinLoader	= IsBLEnable();
	if (bBinLoader && !IsBT1FrameLevel())
	{
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
		SetErrorMessage("BT frame level not DOWN");
		return FALSE;
	}

	return TRUE;
}


//================================================================
// Function Name: 		IsAutoChangeColletReady
// Input arguments:     None
// Output arguments:	None
// Description:   		Check BH & Bin table are ready or not
// Return:				TRUE/FALSE
// Remarks:				called by other function
//================================================================
BOOL CBondHead::IsAutoChangeColletReady()
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (!CMS896AStn::m_bAutoChangeCollet)
	{
		HmiMessage_Red_Yellow("ChangeCollet fcn is not supported");
		return FALSE;
	}

	CString szErrorMess;
	if (!IsBHBTReady(szErrorMess))
	{
		if (!szErrorMess.IsEmpty())
		{
			szErrorMess = _T("ChangeCollet: ") + szErrorMess;
			HmiMessage_Red_Yellow(szErrorMess);
		}
		return FALSE;
	}

	//v4.51A17
CString szTemp;
szTemp.Format("Pusher UP Sensor = %d", IsColletPusherAtSafePos());
//HmiMessage(szTemp);

	if (!IsColletPusherAtSafePos())
	{
		HmiMessage_Red_Yellow("AGC : Pusher is not at UP position");
		SetErrorMessage("AGC : Pusher is not at UP position");
		return FALSE;
	}

	return TRUE;
}


//================================================================
// Function Name: 		TurnOffPickVacuum
// Input arguments:     None
// Output arguments:	None
// Description:   		Turn Off the Pick Vacuum
// Return:				None
// Remarks:				called by other function
//================================================================
VOID CBondHead::TurnOffPickVacuum(const BOOL bUseBHZ2)
{
	if (!bUseBHZ2)
	{
		SetPickVacuum(FALSE);
	}
	else
	{
		SetPickVacuumZ2(FALSE);
	}
}


//================================================================
// Function Name: 		MoveToChangeColletSafePosn
// Input arguments:     None
// Output arguments:	None
// Description:   		Move BHZ1 & BHZ2 to safe position
// Return:				None
// Remarks:				called by other function
//================================================================
LONG CBondHead::MoveToChangeColletSafePosn()
{
	LONG lRet = Z_MoveTo(m_lSwingLevel_Z, SFM_NOWAIT);
	
	if (lRet != gnOK)
	{
		lRet = Z2_MoveTo(m_lSwingLevel_Z2);
	}

	if (lRet != gnOK)
	{
		lRet = Z_Sync();
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ((lRet != gnOK) && (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET))
	{
		Sleep(100);
		lRet = Z_Move(pApp->GetBHZ1HomeOffset());
	}
	if ((lRet != gnOK) && (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET))
	{
		Sleep(100);
		lRet = Z2_Move(pApp->GetBHZ2HomeOffset());
	}

	return lRet;
}


//================================================================
// Function Name: 		BTMoveToAGCClampPos
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to Change Collet clamp position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToAGCClampPos(BOOL bBHZ2)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCClampPos"), bBHZ2))
	{
		CString szErr;

		szErr = "BH DetachCollet: Bin Table is not able to move to CLAMP position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

//================================================================
// Function Name: 		BTMoveToAGCUPLUploadPos
// Input arguments:     None
// Output arguments:	None
// Description:   		BT will move to Change Collet Uplook-Upload position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToAGCUPLUploadPos()
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCUPLUploadPos"), FALSE))
	{
		CString szErr;

		szErr.Format("BH InstallCollet: Bin Table is not able to move to Uplook Upload position");
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}


//================================================================
// Function Name: 		BTMoveToAGCHolderUploadPos
// Input arguments:     None
// Output arguments:	None
// Description:   		BT will move to Holder Upload position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToAGCHolderUploadPos()
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCHolderUploadPos"), FALSE))
	{
		CString szErr;

		szErr.Format("BH InstallCollet: Bin Table is not able to move to Holder Upload position");
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

//================================================================
// Function Name: 		BTMoveToAGCHolderInstallPos
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to Holder Install position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToAGCHolderInstallPos(BOOL bBHZ2)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCHolderInstallPos"), bBHZ2))
	{
		CString szErr;

		szErr.Format("BH InstallCollet: Bin Table is not able to move to Holder Upload position");
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

//================================================================
// Function Name: 		BTMoveToAGCPusher3Pos
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to the pusher position to tighen the collet
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToAGCPusher3Pos(BOOL bBHZ2)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCPusher3Pos"), bBHZ2))
	{
		CString szErr;

		szErr = "BH DetachCollet: Bin Table is not able to move to PUSHER position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}


//================================================================
// Function Name: 		BTMoveToAGCUplookPos
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to uplook position to check the new collet on BH
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToAGCUplookPos(BOOL bBHZ2)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCUpLookPos"), bBHZ2))
	{
		CString szErr;

		szErr.Format("BH InstallCollet: Bin Table is not able to move to BH UPLOOK position");
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}


//================================================================
// Function Name: 		BTMoveToBondPosn
// Input arguments:     bToBond
// Output arguments:	None
// Description:   		BT will move to Bond Position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToBondPosn(BOOL bToBond)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("MoveBinTableToBondPosn"), bToBond))
	{
		return FALSE;
	}
	return TRUE;
}

//================================================================
// Function Name: 		BTMoveToAGCBondPosn
// Input arguments:     bToBond
// Output arguments:	None
// Description:   		BT will move to AGC Bond Position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToAGCBondPosn(BOOL bToBond)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("AGCMoveBinTableToBondPosn"), bToBond))
	{
		return FALSE;
	}
	return TRUE;
}


//================================================================
// Function Name: 		MoveBTForAutoCColletAutoLearnZ
// Input arguments:     bShift
// Output arguments:	None
// Description:   		BT will move to the position of Auto changing Collet and Auto Learning Z
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::MoveBTForAutoCColletAutoLearnZ(BOOL bShift)
{
	// bShift == FALSE, for go back the original postion
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("MoveBTForAutoCColletAutoLearnZ"), bShift))
	{
		return FALSE;
	}
	return TRUE;
}


//===================================================================================
LONG CBondHead::AGC_DetachCollet(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bUseBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseBHZ2);

	if (IsAutoChangeColletReady())
	{
		DetachCollet(bUseBHZ2);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_UploadCollet(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	UploadCollet();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_InstallCollet(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bUseBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseBHZ2);

	InstallCollet(bUseBHZ2, FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_SearchUploadCollet(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if (IsAutoChangeColletReady())
	{
		LONG lOffsetX_UM = 0, lOffsetY_UM = 0;

		if (SearchUploadCollet(lOffsetX_UM, lOffsetY_UM))
		{
			CString szErr;
			szErr.Format("Offset(X,Y = %d um, %d um", lOffsetX_UM, lOffsetY_UM);
			HmiMessage(szErr);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_SearchCollet(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bUseBHZ2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bUseBHZ2);

	if (IsAutoChangeColletReady())
	{
		if (SearchCollet(bUseBHZ2))
		{
			if (bUseBHZ2)
			{
				m_bAutoChgCollet2Fail = FALSE;
				CMSLogFileUtility::Instance()->MS_LogOperation("AGC #2 FAIL status is reset");
			}
			else
			{
				m_bAutoChgCollet1Fail = FALSE;
				CMSLogFileUtility::Instance()->MS_LogOperation("AGC #1 FAIL status is reset");
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


VOID CBondHead::DisplayError(LONG nErr)
{
	if (nErr == Err_BhZMove)
	{
		//SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		HmiMessage_Red_Yellow("BHZ move error");
	}
	else if (nErr == Err_BhTMove)
	{
		//SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		HmiMessage_Red_Yellow("BHT move error");
	}
	else if (nErr == Err_ChgColletTMove)
	{
		HmiMessage_Red_Yellow("BH Change Collet Theta Move Error");
	}
	else if (nErr == Err_ChgColletZMove)
	{
		HmiMessage_Red_Yellow("BH Change Collet Z Move Error");
	}
	else if (nErr == gnNOTOK)
	{
		HmiMessage_Red_Yellow("BH motor error");
	}
}

//v4.50A30
LONG CBondHead::AGC_MoveCollet(IPC_CServiceMessage &svMsg)
{
	BOOL bMove = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bMove);

	BOOL bReturn = AGC_SubMoveCollet(bMove);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}
	
BOOL CBondHead::AGC_SubMoveCollet(const BOOL bMove)
{
	LONG nErr = 0;
	BOOL bReturn = TRUE;

	if (!IsAutoChangeColletReady())
	{
		return FALSE;
	}

	BOOL bIsBHZ2 = FALSE;
	//1. Move BHZ1 & BHZ2 to safe position
	TurnOffPickVacuum(bIsBHZ2);
	if (nErr = MoveToChangeColletSafePosn())
	{
		DisplayError(nErr);
		return FALSE;
	}

	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("DetachCollet")==FALSE )
	{
		return FALSE;
	}

	if (!bMove)
	{
//HmiMessage("1. BH back to READY pos ....");
		nErr = T_MoveTo(m_lPrePickPos_T, TRUE);
	}
	else
	{
		//2. Move BT to UPLOOK position
//HmiMessage("1. BT to UpLOOK position ....");
		if (!BTMoveToAGCUplookPos(bIsBHZ2))
		{
			return FALSE;
		}

//HmiMessage("2. BH to UpLOOK position ....");
		Sleep(200);
		nErr = T_MoveTo(m_lBondPos_T);
		if (!nErr)
		{
			nErr = Z_MoveTo(m_lBondLevel_Z);
		}
	}

	if (nErr)
	{
		DisplayError(nErr);
		return FALSE;
	}
	return TRUE;
}


LONG CBondHead::AGC_ResetColletOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
//4.53D23
//AfxMessageBox("Start AGC",MB_SYSTEMMODAL );
	//Update EjtXY display values on menu
	if (m_bEnableMS100EjtXY)
	{
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
		m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
		m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
		m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;
	}

	//HmiMessage("AGC Collet Center position (in pixel) is reset");		//v4.53A21

	SaveBhData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::AGC_AutoChangeCollet(IPC_CServiceMessage &svMsg)
{
	if (CMS896AStn::m_bAutoChangeCollet==FALSE || m_bAutoChangeColletOnOff)
	{
		HmiMessage("Auto Change Collet function is disabled.", "Bond Head");
		return 1;
	}

	CStringList szPinList;
	szPinList.AddTail("Change Collet 1?");
	szPinList.AddTail("Change Collet 2?");
	LONG lSelect = HmiSelection("Please select Collet", "Bond Head", szPinList, 0);
	if (lSelect == -1)
	{
		return 1;
	}

	if (lSelect == 0)
	{
		if (OpAutoChangeColletZ1())
		{
			BH_ResetColletCount(svMsg);
		}
		else
		{
			m_bAutoChgCollet1Fail = TRUE;
			SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET);
		}
	}
	else
	{
		if (OpAutoChangeColletZ2())
		{
			BH_ResetCollet2Count(svMsg);
		}
		else
		{
			m_bAutoChgCollet2Fail = TRUE;
			SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET);
		}
	}

	return 1;
}

VOID CBondHead::ResetColletAndGenRecord(BOOL bCollet2, BOOL bAuto)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if( CMS896AStn::m_bEnableToolsUsageRecord )
	{
		if( bCollet2 )
		{
			m_oColletRecord.m_szPickLevel.Format("%d", m_lPickLevel_Z2);
			m_oColletRecord.m_szBondLevel.Format("%d", m_lBondLevel_Z2);
			m_oColletRecord.m_szColletBondedDieCount.Format("%d", m_ulCollet2Count);
			m_oColletRecord.m_szBondArmNo = "2";
		}
		else
		{
			m_oColletRecord.m_szPickLevel.Format("%d", m_lPickLevel_Z);
			m_oColletRecord.m_szBondLevel.Format("%d", m_lBondLevel_Z);
			m_oColletRecord.m_szColletBondedDieCount.Format("%d", m_ulColletCount);
			m_oColletRecord.m_szBondArmNo = "1";
		}
		m_oColletRecord.m_szProductType		 = (*m_psmfSRam)["MS896A"]["PKG Filename"];
		m_oColletRecord.m_szMachineNo		 = (*m_psmfSRam)["MS896A"]["MachineNo"];
		if ((pApp->GetCustomerName()=="SanAn" && pApp->GetProductLine()=="XA") || pApp->GetCustomerName() == "ChangeLight(XM)")
			m_oColletRecord.m_szOperatorLogInId = (*m_psmfSRam)["MS896A"]["PasswordID"];//(*m_psmfSRam)["MS896A"]["Operator Id"];
		else
			m_oColletRecord.m_szOperatorLogInId = (*m_psmfSRam)["MS896A"]["Operator Id"];
		if( pApp->GetCustomerName()=="SanAn" || 
			pApp->GetCustomerName()=="SiLan" )
		{
			m_oColletRecord.m_szRemark = pApp->GetPKGFilename();
		}
		if( bAuto )
		{
			m_oColletRecord.m_szOperatorId	= m_oColletRecord.m_szOperatorLogInId;
			m_oColletRecord.m_szReason		= "FULL Auto";
		}
		if( m_oColletRecord.m_szType.IsEmpty() )
		{
			m_oColletRecord.m_szType		= m_oColletRecord.m_szTypeOld;
		}
		if( pApp->GetCustomerName()=="SanAn" && m_oColletRecord.m_szReason!="Change Device" )
		{
			m_oColletRecord.m_szTypeOld		= "";
			m_oColletRecord.m_szRemarkOld	= "";
		}
		if( bCollet2 )
			m_oColletRecord.m_szColletModeOld = m_oColletRecord.m_szCollet2Mode;
		else
			m_oColletRecord.m_szColletModeOld = m_oColletRecord.m_szCollet1Mode;
		if( m_oColletRecord.m_szColletMode.IsEmpty() )
		{
			m_oColletRecord.m_szColletMode = m_oColletRecord.m_szColletModeOld;
		}
		pApp->GenerateColletUsageRecordFile(m_oColletRecord);
		m_oColletRecord.m_szTypeOld		= m_oColletRecord.m_szType;
		m_oColletRecord.m_szRemarkOld	= m_oColletRecord.m_szRemark;
		if( bCollet2 )
			m_oColletRecord.m_szCollet2Mode = m_oColletRecord.m_szColletMode;
		else
			m_oColletRecord.m_szCollet1Mode = m_oColletRecord.m_szColletMode;
		m_nHmiResetCollet = 0;
		SaveFileHWD();
	}
	if( bCollet2 )
	{
		ResetCollet2Count();
	}
	else
	{
		ResetColletCount();
	}
}

VOID CBondHead::ResetColletRecord()
{
	m_oColletRecord.m_bShowColletDiagPage = FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "SanAn")
	{
		return ;
	}
	m_oColletRecord.m_szOperatorId	= "";
	m_oColletRecord.m_szReason		= "";
	m_oColletRecord.m_szRemark		= "";
	m_oColletRecord.m_szShiftNo		= "";
	m_oColletRecord.m_szType		= "";
	m_oColletRecord.m_szColletMode	= "";
}

VOID CBondHead::ResetEjectorRecord()
{
	m_oEjectorRecord.m_bShowEjectorDiagPage = FALSE;
	m_oEjectorRecord.m_szOperatorId	= "";
	m_oEjectorRecord.m_szReason		= "";
	m_oEjectorRecord.m_szRemark		= "";
	m_oEjectorRecord.m_szShiftNo	= "";
	m_oEjectorRecord.m_szType		= "";
	m_oEjectorRecord.m_szUsageType	= "";
}


BOOL CBondHead::OpAutoChangeColletZ(const BOOL bBHZ2, CString &szErrMess)
{
	BOOL bResult = TRUE;

	//3. Check current Collet Counter	
//	m_lAGCColletIndex	= (*m_psmfSRam)["BinTable"]["AGC"]["ColletIndex"];				//v4.51A5
//	BOOL bUseTray2		= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["AGC"]["UseTray2"];
	if (m_lAGCColletIndex > GetCGMaxIndex())
	{
		CString szLog = "AGC: Collet Tray are EMPTY; please refill collets!";
		HmiMessage_Red_Yellow(szLog, "Auto Change Collet");
		szErrMess = szLog;
		return FALSE;
	}

	//1. Make sure bin table move back as it is indexed already or postbond not done yet
	if (!BTMoveToBondPosn(FALSE))
	{
		szErrMess = _T("MoveBinTableToBondPosn = FALSE fails");
		return FALSE;
	}

	//2. Detach collet Z1/Z2
	bResult = DetachCollet(bBHZ2);
	if (!bResult)
	{
		szErrMess = _T("DetachCollet fails");
		return FALSE;
	}

	//4. Check Collet Existance on BH	//v4.51A5
	if (OpCheckColletExistOnBH(bBHZ2))
	{
		CString szLog;
		if (!bBHZ2)
		{
			szLog = "AGC: Collet still detected on BondHead Z1!  Please replace collet manually.";
		}
		else
		{
			szLog = "AGC: Collet still detected on BondHead Z2!  Please replace collet manually.";
		}
		HmiMessage_Red_Yellow(szLog, "Auto Change Collet");
		szErrMess = szLog;
		return FALSE;
	}

	//5.1 Install new collet Z1/Z2
	bResult = UploadCollet();
	if (!bResult)
	{
		szErrMess = _T("InstallCollet fails");
		return FALSE;
	}

	//5.2 Install new collet Z1/Z2
	bResult = InstallCollet(bBHZ2, TRUE);
	if (!bResult)
	{
		szErrMess = _T("InstallCollet fails");
		return FALSE;
	}

	//6. Search collet Z1/Z2 by LookUp camera
	bResult = SearchCollet(bBHZ2);
	if (!bResult)
	{
		szErrMess = _T("SearchCollet fails");
		return FALSE;
	}

	if (!IsAllMotorsEnable())
	{
		szErrMess = _T("motor power is OFF");
		return FALSE;
	}

	//7. Make sure bin table move back as it is indexed already or postbond not done yet
//HmiMessage("5. Move BT back to original pos XY ....");
	if (!BTMoveToAGCBondPosn(TRUE))
	{
		szErrMess = _T("MoveBinTableToBondPosn = TRUE fails");
		return FALSE;
	}

	//8. Auto Learn Z1/Z2 BOND levels
	if (!OpAutoLearnBHZPickBondLevel2(bBHZ2))	
	{
		szErrMess = _T("Op AutoLearnBHZPickBondLevel2 fails");
		return FALSE;
	}

	return TRUE;
}


BOOL CBondHead::OpAutoChangeColletZ1() 
{
	CString szReturnErrMess;

	if (!OpAutoChangeColletZ(FALSE, szReturnErrMess))
	{
		Z_Home();
		szReturnErrMess = "OpAutoChangeColletZ1: " + szReturnErrMess;
		SetErrorMessage(szReturnErrMess);
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::OpAutoChangeColletZ2()
{
	CString szReturnErrMess;

	if (!OpAutoChangeColletZ(TRUE, szReturnErrMess))
	{
		Z2_Home();
		szReturnErrMess = "OpAutoChangeColletZ2: " + szReturnErrMess;
		SetErrorMessage(szReturnErrMess);
		return FALSE;
	}

	return TRUE;
}


BOOL CBondHead::DetachCollet(const BOOL bBHZ2)
{
	CString szErr, szLog;
	LONG nErr = 0;

	if (!IsAutoChangeColletReady())
	{
		return FALSE;
	}

	//1. Move BHZ1 & BHZ2 to safe position
	TurnOffPickVacuum(bBHZ2);
	if (nErr = MoveToChangeColletSafePosn())
	{
		DisplayError(nErr);
		return FALSE;
	}

	//2. Move BT to CLAMP position
	if (!BTMoveToAGCClampPos(bBHZ2))
	{
		return FALSE;
	}

	//3. Move Bond Arm to BOND position and open clamp
	LONG lEncZ = 0;
	SetChgColletClamp(TRUE);
	Sleep(200);
	
	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("DetachCollet")==FALSE )
	{
		return FALSE;
	}

	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		if (nErr = T_MoveTo(m_lPickPos_T))
		{
			DisplayError(nErr);
			return FALSE;
		}

		lEncZ = m_lBondLevel_Z2 - m_lAGCClampLevelOffsetZ;

//		szLog.Format("BHZ2 to CLAMP level = %ld ...", lEncZ);

		//BHZ2 to CLAMP level
		if (nErr = Z2_MoveTo(lEncZ))
		{
			DisplayError(nErr);
			return FALSE;
		}

		SetChgColletClamp(FALSE);
		Sleep(1000);

		if (nErr = Z2_MoveTo(m_lBondLevel_Z2))
		{
			DisplayError(nErr);
			return FALSE;
		}

		Sleep(1000);

		if (nErr = Z2_MoveTo(m_lSwingLevel_Z2))
		{
			DisplayError(nErr);
			return FALSE;
		}

		Sleep(1000);

		if (!Z2_IsPowerOn())
		{
			Z2_Home();
		}
	}
	else
	{
		if (nErr = T_MoveTo(m_lBondPos_T))
		{
			DisplayError(nErr);
			return FALSE;
		}		
		lEncZ = m_lBondLevel_Z - m_lAGCClampLevelOffsetZ;	

//		szLog.Format("BHZ1 to CLAMP level = %ld ...", lEncZ);

		//BHZ1 to CLAMP level
		if (nErr = Z_MoveTo(lEncZ))
		{
			DisplayError(nErr);
			return FALSE;
		}

		SetChgColletClamp(FALSE);
		Sleep(1000);

		if (nErr = Z_MoveTo(m_lBondLevel_Z))
		{
			DisplayError(nErr);
			return FALSE;
		}
		Sleep(1000);

		if (nErr = Z_MoveTo(m_lSwingLevel_Z))
		{
			DisplayError(nErr);
			return FALSE;
		}
		Sleep(1000);

		if (!Z_IsPowerOn())
		{
			Z_Home();
		}
	}
	SetChgColletClamp(TRUE);
	Sleep(2000);
	SetChgColletClamp(FALSE);


	//4. Move Bond Arm back to PrePick
	if( LoopCheckCoverOpen("DetachCollet")==FALSE )
	{
		return FALSE;
	}

	if (nErr = T_MoveTo(m_lPrePickPos_T, TRUE))
	{
		DisplayError(nErr);
		return FALSE;
	}
	return TRUE;
}


BOOL CBondHead::UploadCollet()
{
	LONG nErr = 0;
	BOOL bResult = TRUE;
	CString szErr, szLog;

	if (!IsAutoChangeColletReady())
	{
		return FALSE;
	}
	//1. Move BHZ1 & BHZ2 to safe position
	if (nErr = MoveToChangeColletSafePosn())
	{
		DisplayError(nErr);
		return FALSE;
	}

	BOOL bColletFoundInTray = TRUE;
	do
	{
		//m_lAGCColletIndex = (*m_psmfSRam)["BinTable"]["AGC"]["ColletIndex"];
		if (nErr = MoveChgColletTPitch(m_lAGCColletIndex, SFM_NOWAIT))
		{
			DisplayError(nErr);
			return FALSE;
		}

		//2. Move BT to Uplook PR for new Collet detection
//		if (!BTMoveToAGCUPLUploadPos())
//		{
//			return FALSE;
//		}
		MS50ChgColletT_Sync();
		Sleep(100);

		//3. Search Collet'PR in Upload
//HmiMessage("3. Search collet in tray ...");
		Sleep(1000);
		LONG lOffsetX_UM = 0, lOffsetY_UM = 0;
/*
		if (!SearchUploadCollet(lOffsetX_UM, lOffsetY_UM))
		{
			szErr.Format("BH InstallCollet: Collet is not detected on TRAY #%ld", lTrayIndex);
			HmiMessage_Red_Yellow(szErr);
			SetErrorMessage(szErr);
			bColletFoundInTray = FALSE;

			m_lAGCColletIndex++;
			(*m_psmfSRam)["BinTable"]["AGC"]["ColletIndex"] = m_lAGCColletIndex;

			//3.5 Move ChangeCollet_T to the next collet position
			MoveChgColletTPitch(m_lAGCColletIndex);
		}
		else
*/
		{
			bColletFoundInTray = TRUE;
		}

	} while (!bColletFoundInTray);


	SetChgColletUploadPusher(FALSE);  	//Release Holder pusher for PR Seacrch

	//4. Move BT to Uplook PR for new Collet detection
	if (!BTMoveToAGCHolderUploadPos())
	{
		return FALSE;
	}

	//5. Holder Z(MS50ChgColletZ) move to upload posn
	if (nErr = MS50ChgColletZ_MoveTo(m_lAGCHolderUploadZ))
	{
		DisplayError(nErr);
		return FALSE;
	}

	Sleep(1500);
	SetChgColletHolderVacuum(TRUE); 	//Release Tray pusher2 for collet install	//v4.52A10
	Sleep(1000);
	SetChgColletUploadPusher(TRUE);
	Sleep(2000);

	
	//Get a new Collet on the Holder
	if (nErr = MS50ChgColletZ_MoveTo(m_lAGCHolderInstallZ, SFM_NOWAIT))
	{
		DisplayError(nErr);
		return FALSE;
	}
	Sleep(500);
	SetChgColletUploadPusher(FALSE);

	m_lAGCColletIndex++;
//	(*m_psmfSRam)["BinTable"]["AGC"]["ColletIndex"] = m_lAGCColletIndex;
	SaveBhData();
	MS50ChgColletZ_Sync();
	if (nErr = MoveChgColletTPitch(m_lAGCColletIndex, SFM_NOWAIT))
	{
		DisplayError(nErr);
		return FALSE;
	}
	return TRUE;
}


BOOL CBondHead::IsColletClogged(const BOOL bBHZ2)
{
	if (m_bMS100DigitalAirFlowSnr)
	{
		ULONG ulUnBlockValue = 0;
		if (bBHZ2)
		{
			SetPickVacuumZ2(TRUE);
			Sleep(1000);
			ulUnBlockValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD) - m_ulBHZ2DAirFlowSetZeroOffset;
			
			if (ulUnBlockValue < 13000)
			{
				CString szErr = "InstallCollet BHZ2: collet is clogged before collet-installation";
				HmiMessage_Red_Yellow(szErr);
				SetErrorMessage(szErr);
				return FALSE;
			}
		}
		else
		{
			SetPickVacuum(TRUE);
			Sleep(1000);
			ulUnBlockValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD) - m_ulBHZ1DAirFlowSetZeroOffset;

			if (ulUnBlockValue < 13000)
			{
				CString szErr = "InstallCollet BHZ1: collet is clogged before collet-installation";
				HmiMessage_Red_Yellow(szErr);
				SetErrorMessage(szErr);
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CBondHead::InstallCollet(const BOOL bBHZ2, const BOOL bAuto)
{
	LONG nErr = 0;
	BOOL bResult = TRUE;
	CString szErr, szLog;

	if (!IsAutoChangeColletReady())
	{
		return FALSE;
	}

	//Check existance of collet before installation
	if (!IsColletClogged(bBHZ2))
	{
		return FALSE;
	}

//HmiMessage("1. BH Install Collet start ....");
	//1. Move BHZ1 & BHZ2 to safe position
	TurnOffPickVacuum(bBHZ2);
	if (!bAuto)
	{
		if (nErr = MoveToChangeColletSafePosn())
		{
			DisplayError(nErr);
			return FALSE;
		}
	}

	//4. Move BT to Holder Install position
	if (!BTMoveToAGCHolderInstallPos(bBHZ2))
	{
		return FALSE;
	}

	//5. Move Bond Arm to BOND position and open clamp
	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("DetachCollet")==FALSE )
	{
		return FALSE;
	}

	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		nErr = T_MoveTo(m_lPickPos_T);
		if (!nErr)
		{
			nErr = MS50ChgColletZ_MoveTo(m_lAGCHolderInstallZ);
		}

		if (nErr)
		{
			DisplayError(nErr);
			return FALSE;
		}

		Sleep(100);

		//BHZ2 to Pusher level
		Z2_PowerOn(FALSE);
		Sleep(2000);
		SetChgColletPusher(TRUE);
		Sleep(3000); // 4.53D25 Acc Pusher time 5000 to 3000
		SetChgColletHolderVacuum(FALSE);	//Release Vacuum for collet install
		SetPickVacuumZ2(TRUE);
		Sleep(1000);
		SetChgColletPusher(FALSE);
		Sleep(2000);
		//More puseher once time
		SetChgColletPusher(TRUE);
		Sleep(3000); // 4.53D25 Acc Pusher time 5000 to 3000
		SetChgColletPusher(FALSE);
		Sleep(1000);
		SetPickVacuumZ2(FALSE);
	}
	else
	{
		nErr = T_MoveTo(m_lBondPos_T);
		if (!nErr)
		{
			nErr = MS50ChgColletZ_MoveTo(m_lAGCHolderInstallZ);
		}

		if (nErr)
		{
			DisplayError(nErr);
			return FALSE;
		}

		Sleep(100);

		//BHZ1 to Pusher level
		Z_PowerOn(FALSE);
		Sleep(2000);
		SetChgColletPusher(TRUE);
		Sleep(3000);// 4.53D25 Acc Pusher time 5000 to 3000
		SetChgColletHolderVacuum(FALSE);	//Release Vacuum for collet install
		SetPickVacuum(TRUE);
		Sleep(1000);
		SetChgColletPusher(FALSE);
		Sleep(2000);
		//More puseher once time
		SetChgColletPusher(TRUE);
		Sleep(3000); // 4.53D25 Acc Pusher time 5000 to 3000
		SetChgColletPusher(FALSE);
		Sleep(1000);
		SetPickVacuum(FALSE);
	}

	//v4.51A5
	//6. Check Z position to make sure pusher is at UP level before moving BT
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		Z2_Home();
		if (!Z2_IsPowerOn())
		{
			return FALSE;
		}
		Sleep(500);
		Z2_PowerOn(FALSE);
	}
	else
	{
		Z_Home();
		if (!Z2_IsPowerOn())
		{
			return FALSE;
		}
		Sleep(500);
		Z_PowerOn(FALSE);
	}

	if (nErr = MS50ChgColletZ_MoveTo(0, SFM_NOWAIT))		//Holder Move to home position before BT move
	{
		DisplayError(nErr);
		return FALSE;
	}
	//Sleep(3000);


	//7. Move BT to PUSHER2 position
	if (!BTMoveToAGCPusher3Pos(bBHZ2))
	{
		if (bBHZ2)			//If BHZ2 towards PICK	
		{
			Z2_Home();
		}
		else
		{
			Z_Home();
		}

		return FALSE;
	}

	if (nErr = MS50ChgColletZ_Sync())
	{
		DisplayError(nErr);
		return FALSE;
	}
	//8. Move Bond Arm to BOND position and push again
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		//Pusher BHZ2 and tighten the installed collet
		Z2_PowerOn(FALSE);
		Sleep(1000);
		SetChgColletPusher(TRUE);
		Sleep(3000);// 4.53D25 Acc Pusher time 5000 to 3000
		SetChgColletPusher(FALSE);
		Sleep(1000);

		Z2_Home();
		Sleep(1000);
		
		//BHZ2 Install-collet done;
		if (!Z2_IsPowerOn())
		{
			//Z2_Home();
			return FALSE;	//v4.51A5
		}
	}
	else
	{
		//Pusher BHZ1 and tighten the installed collet
		Z_PowerOn(FALSE);
		Sleep(1000);
		SetChgColletPusher(TRUE);
		Sleep(3000);// 4.53D25 Acc Pusher time 5000 to 3000
		SetChgColletPusher(FALSE);
		Sleep(1000);

		Z_Home();
		Sleep(1000);

		//BHZ1 Install-collet done;
		if (!Z_IsPowerOn())
		{
			//Z_Home();
			return FALSE;	//v4.51A5
		}
	}

	//9. Move Bond Arm back to PrePick
//HmiMessage("9. BondArm to PrePick ....");
	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("DetachCollet")==FALSE )
	{
		return FALSE;
	}

	if (!bAuto)
	{
		if (nErr = T_MoveTo(m_lPrePickPos_T, TRUE))
		{
			DisplayError(nErr);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CBondHead::SearchUploadCollet(LONG &lOffsetX_UM, LONG &lOffsetY_UM)
{
	LONG nErr = 0;
	BOOL bResult = TRUE;
	CString szErr, szLog;

	if (!IsAutoChangeColletReady())
	{
		return FALSE;
	}

	//1. Move BHZ1 & BHZ2 to safe position
	if (nErr = MoveToChangeColletSafePosn())
	{
		DisplayError(nErr);
		return FALSE;
	}

	//2. Move BT to Upload UPLOOK position
	if (!BTMoveToAGCUPLUploadPos())
	{
		return FALSE;
	}

	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("SearchUploadCollet")==FALSE )
	{
		return FALSE;
	}

	//4. Search collet by Uplook PR
	typedef struct {
		BOOL	bStatus;
		INT		nOffsetX;
		INT		nOffsetY;
	} SRCH_CHOLE_RESULT;

	SRCH_CHOLE_RESULT stResult;
	stResult.bStatus = FALSE;
	stResult.nOffsetX = 0;
	stResult.nOffsetY = 0;
	
	BOOL bUseBH2 = FALSE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bUseBH2);

	INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "AGC_SearchUploadColletPR", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(SRCH_CHOLE_RESULT), &stResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!stResult.bStatus)
	{
		szErr = "ERROR: Seach Upload Collet Failure";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}
	else
	{
		lOffsetX_UM = stResult.nOffsetX;
		lOffsetY_UM = stResult.nOffsetY;
	}
	return TRUE;
}


BOOL CBondHead::SearchCollet(const BOOL bBHZ2)
{
	LONG nErr = 0;
	BOOL bResult = TRUE;
	CString szErr, szLog;

	if (!IsAutoChangeColletReady())
	{
		return FALSE;
	}

	//1. Move BHZ1 & BHZ2 to safe position
//HmiMessage("1. BH Search Collet start ....");		//v4.50A15
	TurnOffPickVacuum(bBHZ2);
	if (nErr = MoveToChangeColletSafePosn())
	{
		DisplayError(nErr);
		return FALSE;
	}

	//2. Move BT to UPLOOK position
//HmiMessage("2. BT to UpLOOK position ....");
	if (!BTMoveToAGCUplookPos(bBHZ2))
	{
		return FALSE;
	}

	//3. Move Bond Arm to BOND position and open clamp
	Sleep(200);
	
	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("DetachCollet")==FALSE )
	{
		return FALSE;
	}

	if (bBHZ2)
	{
		//If BHZ2 towards PICK	
		nErr = T_MoveTo(m_lPickPos_T);
		LONG lEncZ = m_lBondLevel_Z2 - m_lAGCUplookPRLevelOffsetZ;
		if (!nErr)
		{
			nErr = Z2_MoveTo(lEncZ);
		}
	}
	else
	{
		nErr = T_MoveTo(m_lBondPos_T);
		LONG lEncZ = m_lBondLevel_Z - m_lAGCUplookPRLevelOffsetZ;
		if (!nErr)
		{
			nErr = Z_MoveTo(lEncZ);
		}
	}

	if (nErr)
	{
		DisplayError(nErr);
		return FALSE;
	}
	//4. Search collet hole by BOND PR
//HmiMessage("4. Search collet hole ....");
	Sleep(30);

	typedef struct {
		BOOL	bStatus;
		INT		nOffsetX;
		INT		nOffsetY;
	} SRCH_CHOLE_RESULT;

	SRCH_CHOLE_RESULT stResult;
	stResult.bStatus = FALSE;
	stResult.nOffsetX = 0;
	stResult.nOffsetY = 0;
	
	BOOL bUseBH2 = bBHZ2;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bUseBH2);

	INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "AGC_SearchColletHole", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(SRCH_CHOLE_RESULT), &stResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bSearchHoleSucess = TRUE;
	if (stResult.bStatus)
	{
		CString szTemp;
		//HmiMessage("BH: collet hole is found");

		//m_lCDiePos_EjX = m_lEnc_EjX;
		LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
		LONG lCollet1OffsetY = GetEjtCollet1OffsetY();
		LONG lCollet2OffsetX = GetEjtCollet2OffsetX();
		LONG lCollet2OffsetY = GetEjtCollet2OffsetY();

		//m_lArm1Pos_EjX = m_lCDiePos_EjX + lCollet1OffsetX;
		//m_lArm1Pos_EjY = m_lCDiePos_EjY + lCollet1OffsetY;
		//m_lArm2Pos_EjX = m_lCDiePos_EjX + lCollet2OffsetX;
		//m_lArm2Pos_EjY = m_lCDiePos_EjY + lCollet2OffsetY;


		//v4.50A30
		LONG lDiffX = 0,lDiffY = 0;
		//EjtXY motor resolution = 0.488 um per motor step (Machine MSD)
		DOUBLE dEjtXYRes = GetChannelResolution(MS896A_CFG_CH_EJECTOR_X) * 1000.0;// M/Count ==> um/count
		if (bBHZ2)	//If search BH2 collet	
		{
			LONG lAGCCollet2OffsetX = stResult.nOffsetX;	//in um
			LONG lAGCCollet2OffsetY = stResult.nOffsetY;	//in um
			lAGCCollet2OffsetX = _round(lAGCCollet2OffsetX / dEjtXYRes);	//um to motor step
			lAGCCollet2OffsetY = _round(lAGCCollet2OffsetY / dEjtXYRes);	//um to motor step

			LONG lCollet2OffsetXOld = GetEjtCollet2OffsetX();
			LONG lCollet2OffsetYOld = GetEjtCollet2OffsetY();

			AGC_UpdateWprEjtOffset(bBHZ2, lAGCCollet2OffsetX, lAGCCollet2OffsetY);

			lCollet2OffsetX = GetEjtCollet2OffsetX();
			lCollet2OffsetY = GetEjtCollet2OffsetY();

			lDiffX = lCollet2OffsetX - lCollet2OffsetXOld;
			lDiffY = lCollet2OffsetY - lCollet2OffsetYOld;

			szTemp.Format("AGC BH2 SearchCollet: EJTXY (%ld, %ld), new C2-Offset XY (%ld, %ld) in motor step, new (%ld, %ld),old(%ld, %ld),Diff(%ld,%ld)", 
				m_lCDiePos_EjX,m_lCDiePos_EjY,
				m_lCDiePos_EjX + lCollet2OffsetX, 
				m_lCDiePos_EjY + lCollet2OffsetY, 
				lAGCCollet2OffsetX, lAGCCollet2OffsetY, lCollet2OffsetX,lCollet2OffsetY,lCollet2OffsetXOld,lCollet2OffsetYOld,lDiffX,lDiffY);
		}
		else
		{
			LONG lAGCCollet1OffsetX = stResult.nOffsetX;	//in um
			LONG lAGCCollet1OffsetY = stResult.nOffsetY;	//in um

			lAGCCollet1OffsetX = _round(lAGCCollet1OffsetX / dEjtXYRes);	//um to motor step
			lAGCCollet1OffsetY = _round(lAGCCollet1OffsetY / dEjtXYRes);	//um to motor step

			LONG lCollet1OffsetXOld = GetEjtCollet1OffsetX();
			LONG lCollet1OffsetYOld = GetEjtCollet1OffsetY();

			AGC_UpdateWprEjtOffset(bBHZ2, lAGCCollet1OffsetX, lAGCCollet1OffsetY);

			LONG lCollet1OffsetX = GetEjtCollet1OffsetX();
			LONG lCollet1OffsetY = GetEjtCollet1OffsetY();

			lDiffX = lCollet1OffsetX - lCollet1OffsetXOld;
			lDiffY = lCollet1OffsetY - lCollet1OffsetYOld;

			szTemp.Format("AGC BH1 SearchCollet: EJTXY (%ld, %ld), new C1-Offset XY (%ld, %ld) in motor step, new (%ld, %ld),old(%ld, %ld),Diff(%ld,%ld)", 
				m_lCDiePos_EjX,m_lCDiePos_EjY,
				m_lCDiePos_EjX + lCollet1OffsetX, 
				m_lCDiePos_EjY + lCollet1OffsetY,lCollet1OffsetX,lCollet1OffsetY,lCollet1OffsetXOld,lCollet1OffsetYOld,lDiffX,lDiffY);
		}
		double dDiffX_um = lDiffX * dEjtXYRes;
		double dDiffY_um = lDiffY * dEjtXYRes;
		if ((fabs(dDiffX_um) > 150) || (labs(lDiffY) > 150))
		{
			CString szLog;
			szLog.Format("AGC: Uplook Collet Offset(%.2f um, %.2f um) exceed limit (150 um)", dDiffX_um, dDiffY_um);
			HmiMessage_Red_Yellow(szLog);		//v4.52A8
			bSearchHoleSucess = FALSE;
			//return FALSE;
		}

		if (bSearchHoleSucess)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);

			//WTXY motor resolution = 0.5 um per motor step (Machine MSD)
			DOUBLE dWTXYRes = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * 1000.0;// M/Count ==> um/count

			LONG lWT_DiffX_Step = _round(dDiffX_um / dWTXYRes); //um to motor step
			LONG lWT_DiffY_Step = _round(dDiffY_um / dWTXYRes); //um to motor step
			AGC_WTMoveDiff(bBHZ2, lWT_DiffX_Step, lWT_DiffY_Step);

			BOOL bStatus = OpMoveEjectorTableXY(FALSE, SFM_WAIT);
			if (!bStatus)
			{
				SetErrorMessage("Ejector XY module is off power!");
				SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
				bSearchHoleSucess = FALSE;
				//return FALSE;
			}
		}
	}
	else
	{
		bSearchHoleSucess = FALSE;
		//return FALSE;
	}


	//5. Move Bond Arm back to PrePick
//HmiMessage("5. BondArm to PrePick ....");
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		nErr = Z2_MoveTo(m_lSwingLevel_Z2);
	}
	else
	{
		nErr = Z_MoveTo(m_lSwingLevel_Z);
	}

	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("DetachCollet")==FALSE )
	{
		return FALSE;
	}

	if (!nErr)
	{
		nErr = T_MoveTo(m_lPrePickPos_T, TRUE);
	}

	if (!bSearchHoleSucess)
	{
		return FALSE;
	}

	if (nErr)
	{
		DisplayError(nErr);
		return FALSE;
	}
	SaveBhData();


	//6.	//v4.50A31
	if (!stResult.bStatus)
	{
		if (bBHZ2)			//If BHZ2 towards PICK	
			szErr = "ERROR: Collet #2 is not found in AGC fcn";
		else
			szErr = "ERROR: Collet #1 is not found in AGC fcn";

		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	//7. update wafer table according to the difference

	return TRUE;
}



BOOL CBondHead::AGC_UpdateWprEjtOffset(BOOL bBHZ2, LONG lOffsetXinStep, LONG lOffsetYinStep)	
{
	IPC_CServiceMessage rReqMsg;

	typedef struct 
	{
		BOOL bBHCollet2;
		LONG lAGCColletOffsetXinStep;
		LONG lAGCColletOffsetYinStep;
	} AGC_OFFSET;

	BOOL bResult = TRUE;
	AGC_OFFSET stInfo;
	stInfo.bBHCollet2 = bBHZ2;
	stInfo.lAGCColletOffsetXinStep = lOffsetXinStep;
	stInfo.lAGCColletOffsetYinStep = lOffsetYinStep;

	rReqMsg.InitMessage(sizeof(AGC_OFFSET), &stInfo);
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "WPR_UpdateAGCOffset", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!bResult)
	{
		CString szErr = "BH AGC_UpdateWprEjtOffset fails.";
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}


BOOL CBondHead::OpAutoLearnBHZPickBondLevel2(BOOL bBHZ2)	//After Auto Change Collet
{
	//if enable auto changing collet option, it should auto learn bond level
	if (!(m_bAutoCleanAutoLearnZLevels /*|| IsEnaAutoChangeCollet()*/))
	{
		SetErrorMessage("Op AutoLearnBHZPickBondLevel2: fcn is disabled");
		return TRUE;
	}

	CString szErrorMess;
	if (!IsBHBTReady(szErrorMess))
	{
		if (!szErrorMess.IsEmpty())
		{
			szErrorMess = _T("Op AutoLearnBHZPickBondLevel2: ") + szErrorMess;
			SetErrorMessage(szErrorMess);
		}
		return FALSE;
	}

	BOOL bStatus = TRUE;
	CMSLogFileUtility::Instance()->MS_LogOperation("BH: AutoLearn BOND Z1 & Z2 start ...");

	LONG lValue = 0;
	if (m_bMS100DigitalAirFlowSnr)	
	{
		if (bBHZ2)
		{
			SetPickVacuumZ2(FALSE);
			SetStrongBlowZ2(TRUE);
		}
		else
		{
			SetPickVacuum(FALSE);
			SetStrongBlow(TRUE);
		}
	}

	LONG nErr = 0;
	if (nErr = MoveToChangeColletSafePosn())
	{
		DisplayError(nErr);
		return FALSE;
	}

	if (m_bMS100DigitalAirFlowSnr)		//v4.50A3
	{
		Sleep(1000);
		if (bBHZ2)
		{
			SetStrongBlowZ2(FALSE);
		}
		else
		{
			SetStrongBlow(FALSE);
		}
		Sleep(500);

		//if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
		if (bBHZ2)
		{
			m_ulBHZ2DAirFlowSetZeroOffset = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		}
		else
		{
			m_ulBHZ1DAirFlowSetZeroOffset = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		}
	}

	if (bBHZ2)
	{
		SetPickVacuumZ2(TRUE);
	}
	else
	{
		SetPickVacuum(TRUE);
	}
	Sleep(100);

	if (m_bMS100DigitalAirFlowSnr)
	{
		//Take the un-clogged reading
		Sleep(500);

//		if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
		if (bBHZ2)
		{
			lValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
			lValue = lValue - m_ulBHZ2DAirFlowSetZeroOffset;
			if (lValue < 0)
				lValue = 0;
			m_ulBHZ2DAirFlowUnBlockValue	= (ULONG) lValue;
		}
		else
		{
			lValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
			lValue = lValue - m_ulBHZ1DAirFlowSetZeroOffset;
			if (lValue < 0)
				lValue = 0;
			m_ulBHZ1DAirFlowUnBlockValue	= (ULONG) lValue;
		}
	}

	//HmiMessage("BinTable shift one pitch ....");
	if (!MoveBTForAutoCColletAutoLearnZ(TRUE))
	{
		return FALSE;
	}

	LONG lNewEncZ1	= 0;
	LONG lNewEncZ2	= 0;
	LONG lEncZ		= 0;

	// BT Level tilting, to adjust and change pick level.
	LONG lBTLevelOffset = 0;
	if (m_bUseMultiProbeLevel)	// auto learn 2 after change collet.
	{
		LONG lX = 0, lY = 0;
		BT_Get_XY_Posn(lX, lY);
		//	get bin table current position X and Y.
		lBTLevelOffset = GetBTLevelOffset(bBHZ2, lX, lY);
		if (bBHZ2)
		{
			m_lZ2BondLevelBT_X = lX;
			m_lZ2BondLevelBT_Y = lY;
			m_bZ2BondLevelBT_R = IsMS90BTRotated();
		}
		else
		{
			m_lZ1BondLevelBT_X = lX;
			m_lZ1BondLevelBT_Y = lY;
			m_bZ1BondLevelBT_R = IsMS90BTRotated();
		}
		BH_MBL_LogLevel();
		CString szMsg;
		szMsg.Format("auto Learn 2 BHZ%d new BT %d,%d offset %d, BT Rotate %d", bBHZ2+1, lX, lY, lBTLevelOffset, IsMS90BTRotated());
		SetErrorMessage(szMsg);
	}

	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("OpAutoLearnBHZPickBondLevel2")==FALSE )
	{
		MoveBTForAutoCColletAutoLearnZ(FALSE);
		return FALSE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (bBHZ2)
	{
		//HmiMessage("BHZ2 to BOND for AUTOLEARN ....");

		if (nErr = T_MoveTo(m_lPickPos_T))
		{
			DisplayError(nErr);
			return FALSE;
		}

		lEncZ = m_lBondLevel_Z2 + 2000;	//m_lSwingLevel_Z2;		//v4.49A9	//v4.50A3

		//HmiMessage("BHZ2 auto-learn Z starts ....");

		if (!AutoLearnBHZ2BondLevel(lEncZ) || !Z2_IsPowerOn())
		{
			Z2_Home();
			SetPickVacuum(FALSE);
			SetPickVacuumZ2(FALSE);
			if (nErr = T_MoveTo(m_lPrePickPos_T))
			{
				DisplayError(nErr);
				return FALSE;
			}
			SetErrorMessage("Op AutoLearnBHZPickBondLevel2: Auto Learn Z2 fails");
			if( !Z2_IsPowerOn() )
			{
				CString szErr = "\nBHZ2 is not power on!";
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			}	//	reduce surplus alarm
			MoveBTForAutoCColletAutoLearnZ(FALSE);
			return FALSE;
		}
		
		lNewEncZ2 = lEncZ + m_lBondDriveIn;

		if (m_bMS100DigitalAirFlowSnr)
		{
			//Take the clogged reading
			Sleep(500);
			lValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
			lValue = lValue - m_ulBHZ2DAirFlowSetZeroOffset;
			if (lValue < 0)
				lValue = 0;
			m_ulBHZ2DAirFlowBlockValue	= (ULONG) lValue;
		}

		nErr = Z2_MoveTo(m_lSwingLevel_Z2);
		if (!nErr && (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET))
		{
			Sleep(100);
			nErr = Z2_Move(pApp->GetBHZ2HomeOffset());
		}

		Sleep(100);
	}
	else
	{
		//HmiMessage("BHZ1 to BOND for AUTOLEARN ....");

		if (nErr = T_MoveTo(m_lBondPos_T))
		{
			DisplayError(nErr);
			return FALSE;
		}
		lEncZ = m_lBondLevel_Z + 2000;		//m_lSwingLevel_Z;	//v4.50A3

		//HmiMessage("BHZ1 auto-learn Z starts ....");

		if (!AutoLearnBHZ1BondLevel(lEncZ) || !Z_IsPowerOn())
		{
			Z_Home();
			SetPickVacuum(FALSE);
			SetPickVacuumZ2(FALSE);
			if (nErr = T_MoveTo(m_lPrePickPos_T))
			{
				DisplayError(nErr);
				return FALSE;
			}
			SetErrorMessage("Op AutoLearnBHZPickBondLevel2: Auto Learn Z fails");
			if( !Z_IsPowerOn() )
			{
				CString szErr = "\nBHZ is not power on!";
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			}	//	reduce surplus alarm

			MoveBTForAutoCColletAutoLearnZ(FALSE);
			return FALSE;
		}
		
		lNewEncZ1 = lEncZ + m_lBondDriveIn;

		if (m_bMS100DigitalAirFlowSnr)
		{
			//Take the clogged reading
			Sleep(500);
			lValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
			lValue = lValue - m_ulBHZ1DAirFlowSetZeroOffset;
			if (lValue < 0)
				lValue = 0;
			m_ulBHZ1DAirFlowBlockValue	= (ULONG) lValue;
		}

		nErr = Z_MoveTo(m_lSwingLevel_Z);
		if (!nErr && (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET))
		{
			Sleep(100);
			nErr = Z_Move(pApp->GetBHZ1HomeOffset());
		}

		Sleep(100);
	}

	
	//Extra protection to protect against BH collision/safety
	if( LoopCheckCoverOpen("OpAutoLearnBHZPickBondLevel2")==FALSE )
	{
		MoveBTForAutoCColletAutoLearnZ(FALSE);
		return FALSE;
	}	//	check before rotate T

	if (!nErr)
	{
		nErr = T_MoveTo(m_lPrePickPos_T, FALSE);
	}
	if (nErr)
	{
		DisplayError(nErr);
		return FALSE;
	}

	if (!MoveBTForAutoCColletAutoLearnZ(FALSE))
	{
		return FALSE;
	}

	LONG lCurrPickZ  = m_lPickLevel_Z;
	LONG lCurrPickZ2 = m_lPickLevel_Z2;
	LONG lCurrBondZ  = m_lBondLevel_Z;
	LONG lCurrBondZ2 = m_lBondLevel_Z2;
	LONG lOffsetZ	 = 0;


	CString szMsg;
	ULONG ulThresholdValue = 0;

	if (bBHZ2)
	{
		lOffsetZ = lNewEncZ2 - m_lBondLevel_Z2 - lBTLevelOffset;

		szMsg.Format("Op AutoLearnBHZPickBondLevel2 - new BONDZ2 = %ld (%ld); BT tilting %d; ZOffset = %ld steps",
						lNewEncZ2, lCurrBondZ2, lBTLevelOffset, lOffsetZ);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		//v4.51A5	//xx check new/old level difference.
	//	if( CheckNewOldLevelOffset(lOffsetZ)==FALSE )	//	new change collet, should > than settings.
		if (labs(lOffsetZ) > 1000)
		{
			T_Sync();
			CString szErr = "Auto-Learn Z2 fails because Offset > 1000 steps!";
		//	CString szErr;
		//	szErr.Format("Auto-Learn2 Z2 bond fails because Offset %d > %d steps!",
		//		lOffsetZ, m_lBondHeadToleranceLevel);
			SetErrorMessage(szErr);
			szErr = "\n" + szErr;
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			return FALSE;
		}

		m_lBondLevel_Z2		= lNewEncZ2;
		m_lBPGeneral_C		= m_lBondLevel_Z2;

		m_lPickLevel_Z2	= m_lPickLevel_Z2 + lOffsetZ;
		m_lBPGeneral_B	= m_lPickLevel_Z2;

		if (m_bMS100DigitalAirFlowSnr)
		{
			
			m_ulBHZ2DAirFlowThreshold = GetBHZ2MissingDieThresholdValue(m_ulBHZ2DAirFlowUnBlockValue);
			//v4.50A7	//Threshold value for CJ
			m_ulBHZ2DAirFlowThresholdCJ = GetBHZ2ColletJamThresholdValue(m_ulBHZ2DAirFlowUnBlockValue);

			bStatus = CheckDigitalAirFlowThresholdLimit(TRUE);
			m_pBHZ2AirFlowCompensation->ResetDieAirFlowValueList();
			szMsg.Format("AUTO-Learn DAirFlow BHZ2: Unclog = %lu, Clog = %lu, Pct = %.1f, Thres = %lu, THres(CJ) = %lu, Status = %d",
							m_ulBHZ2DAirFlowUnBlockValue, m_ulBHZ2DAirFlowBlockValue, 
							m_dBHZ1ThresholdPercent, m_ulBHZ2DAirFlowThreshold, m_ulBHZ2DAirFlowThresholdCJ, 
							bStatus);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			//HmiMessage(szMsg);	//v4.50A4
		}

		//xx check pick/bond level difference.
		CheckZ2PickBondLevelOffset();

		szMsg.Format("Op AutoLearnBHZPickBondLevel2 BHZ2 - new PICKZ2=%ld, BONDZ2=%ld, BONDZ=%ld",
						m_lPickLevel_Z2, m_lBondLevel_Z2, m_lBondLevel_Z);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage(szMsg);
		//HmiMessage_Red_Back("BH: AutoLearn Z2 levels done.");
	}
	else
	{
		lOffsetZ = lNewEncZ1 - m_lBondLevel_Z - lBTLevelOffset;

		szMsg.Format("Op AutoLearnBHZPickBondLevel2 - new BONDZ = %ld (%ld); BT Tilting %d; ZOffset = %ld steps",
						lNewEncZ1, lCurrBondZ, lBTLevelOffset, lOffsetZ);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		//v4.51A5	//xx check new/old level difference.
	//	if( CheckNewOldLevelOffset(lOffsetZ)==FALSE )	//	new change collet, should > than settings.
		if (labs(lOffsetZ) > 1000)
		{
			T_Sync();
			CString szErr = "Auto-Learn Z1 fails because Offset > 1000 steps!";
		//	CString szErr;
		//	szErr.Format("Auto-Learn2 Z1 fails because Offset %d > %d steps!", lOffsetZ, m_lBondHeadToleranceLevel);
			SetErrorMessage(szErr);
			szErr = "\n" + szErr;
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			return FALSE;
		}

		m_lBondLevel_Z		= lNewEncZ1;
		m_lBPGeneral_7		= m_lBondLevel_Z;

		m_lPickLevel_Z	= m_lPickLevel_Z + lOffsetZ;
		m_lBPGeneral_6	= m_lPickLevel_Z;

		if (m_bMS100DigitalAirFlowSnr)
		{
			m_ulBHZ1DAirFlowThreshold = GetBHZ1MissingDieThresholdValue(m_ulBHZ1DAirFlowUnBlockValue);

			//v4.50A7	//THreshold value for CJ
			m_ulBHZ1DAirFlowThresholdCJ = GetBHZ1ColletJamThresholdValue(m_ulBHZ1DAirFlowUnBlockValue);

			bStatus = CheckDigitalAirFlowThresholdLimit(FALSE);
			m_pBHZ1AirFlowCompensation->ResetDieAirFlowValueList();
			szMsg.Format("AUTO-Learn DAirFlow BHZ1: Unclog = %lu, Clog = %lu, Pct = %.1f, Thres = %lu, Thres(CJ) = %lu, Status = %d",
							m_ulBHZ1DAirFlowUnBlockValue, m_ulBHZ1DAirFlowBlockValue, 
							m_dBHZ1ThresholdPercent, m_ulBHZ1DAirFlowThreshold, m_ulBHZ1DAirFlowThresholdCJ, 
							bStatus);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			//HmiMessage(szMsg);	//v4.50A4
		}

		//xx check pick/bond level difference.
		CheckZ1PickBondLevelOffset();

		szMsg.Format("Op AutoLearnBHZPickBondLevel2 BHZ1 - new PICKZ=%ld, BONDZ=%ld, BONDZ2=%ld",
						m_lPickLevel_Z, m_lBondLevel_Z, m_lBondLevel_Z2);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage(szMsg);
		//HmiMessage_Red_Back("BH: AutoLearn Z1 levels done.");
	}

	SetBondPadLevel(bBHZ2);	//	auto learn level2

	SaveBhData();

	T_Sync();
	return bStatus;
}


BOOL CBondHead::OpCheckColletExistOnBH(BOOL bBHZ2)				//v4.51A5
{
	if (!CMS896AStn::m_bAutoChangeCollet)
		return FALSE;
	if (!m_bMS100DigitalAirFlowSnr)	
		return FALSE;

	if (bBHZ2)
		CMSLogFileUtility::Instance()->MS_LogOperation("BH: OpCheckColletExistOnBH2 start ...");
	else
		CMSLogFileUtility::Instance()->MS_LogOperation("BH: OpCheckColletExistOnBH1 start ...");

	LONG lValue = 0;
	CString szLog;
	BOOL bColletExist = FALSE;

	if (bBHZ2)
	{
		SetPickVacuumZ2(TRUE);
		Sleep(1000);

		lValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		lValue = lValue - m_ulBHZ2DAirFlowSetZeroOffset;
		
		SetPickVacuumZ2(FALSE);

		if (lValue < 13000)
			bColletExist = TRUE;	//Collet detected at BHZ2 !!!

		szLog.Format("OpCheckColletExistOnBH2: ColletExist = %d; reading = %ld (ZeroOffset = %ld)",
							bColletExist, lValue, m_ulBHZ2DAirFlowSetZeroOffset);

	}
	else
	{
		SetPickVacuum(TRUE);
		Sleep(1000);

		lValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		lValue = lValue - m_ulBHZ1DAirFlowSetZeroOffset;
		
		SetPickVacuum(FALSE);
		
		if (lValue < 13000)
			bColletExist = TRUE;	//Collet detected at BHZ1 !!!

		szLog.Format("OpCheckColletExistOnBH1: ColletExist = %d; reading = %ld (ZeroOffset = %ld)",
							bColletExist, lValue, m_ulBHZ1DAirFlowSetZeroOffset);
	}

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return bColletExist;
}


BOOL CBondHead::OpCheckSPCAlarmTwice(BOOL bBHZ2, CString szLog)	//v4.52A7
{
	if (!CMS896AStn::m_bPBErrorCleanCollet)
		return TRUE;

	if (bBHZ2)		//If BHZ2 at PICK
	{
		if (m_bSPCAlarmTwiceZ2)
		{
		}
		else
		{
			m_bSPCAlarmTwiceZ2 = TRUE;
			m_lSPCResult = 1;
			(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
			if (m_ulMaxCleanCount > 0)
			{
				m_ulCleanCount = m_ulMaxCleanCount;
			}
		}
	}
	else
	{
		if (m_bSPCAlarmTwice)
		{
		}
		else
		{
			m_bSPCAlarmTwice = TRUE;
			m_lSPCResult = 1;
			(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
			if (m_ulMaxCleanCount > 0)
			{
				m_ulCleanCount = m_ulMaxCleanCount;
			}
		}
	}

	CString szMsg;
	szMsg.Format("BH: BPR SPC result %s = %d (SanAn); Twice = %d (%d); CycleCnt = %ld", 
					(LPCTSTR) szLog, m_lSPCResult, 
					m_bSPCAlarmTwice, m_bSPCAlarmTwiceZ2, m_ulCycleCount);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);	//v4.53A21
	return TRUE;
}


VOID CBondHead::OpPreStartInitDAirFlowThresold()	//v4.52A11
{
	if (!m_bMS100DigitalAirFlowSnr)	
		return;

	m_ulBHZ1CurrThresholdCount = 0;
	m_ulBHZ2CurrThresholdCount = 0;

	for (INT i=0; i<BH_MAX_THRESHOLD_DAIRFLOW; i++)
	{
		m_ulBHZ1CurrThreshold[i]	= 0;
		m_ulBHZ2CurrThreshold[i]	= 0;
	}
}


BOOL CBondHead::OpUpdateDAirFlowThresholdValue(BOOL bBHZ2)	//v4.52A11
{
	return TRUE;
/*
	if (!m_bMS100DigitalAirFlowSnr)	
		return TRUE;
	if (!m_bEnableDAFlowRunTimeCheck)	//v4.53A1
		return TRUE;
			
	CString szMsg;
	ULONG ulThresholdValue = 0;
	ULONG ulThresholdValueCJ = 0;
	BOOL bStatus = TRUE;

	if (bBHZ2)
	{
		LONG lValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		lValue = lValue - m_ulBHZ2DAirFlowSetZeroOffset;

		// filter noise value by using 2*curr_thres values
		if (lValue > _round(m_ulBHZ2DAirFlowThreshold * 2))
		{
			szMsg.Format("BHZ2 ADC value = %ld (%ld) is filtered",  lValue, m_ulBHZ2DAirFlowThreshold * 2);
			BH_Z_AirFlowLog(FALSE, szMsg);
			return TRUE;
		}

		m_ulBHZ2CurrThreshold[m_ulBHZ2CurrThresholdCount % BH_MAX_THRESHOLD_DAIRFLOW] = lValue;
		m_ulBHZ2CurrThresholdCount++;

		if (m_ulBHZ2CurrThresholdCount >= BH_MAX_THRESHOLD_DAIRFLOW)
		{
			m_ulBHZ2CurrThresholdCount = 0;
	
			DOUBLE dAverage = 0;
			ULONG ulHigh	= m_ulBHZ2CurrThreshold[0];
			ULONG ulLow		= m_ulBHZ2CurrThreshold[0];

			for (INT i=0; i<BH_MAX_THRESHOLD_DAIRFLOW; i++)
			{
				if (m_ulBHZ2CurrThreshold[i] > ulHigh)
					ulHigh	= m_ulBHZ2CurrThreshold[i];
				else if (m_ulBHZ2CurrThreshold[i] < ulLow)
					ulLow	= m_ulBHZ2CurrThreshold[i];

				dAverage = dAverage + m_ulBHZ2CurrThreshold[i];
				m_ulBHZ2CurrThreshold[i] = 0;
			}

			dAverage = (dAverage - ulLow - ulHigh) / (BH_MAX_THRESHOLD_DAIRFLOW - 2);
			ULONG ulAvgUncloggedValue = (ULONG) dAverage;

			ulThresholdValue = GetBHZ2MissingDieThresholdValue(ulAvgUncloggedValue);
			ulThresholdValueCJ = GetBHZ2ColletJamThresholdValue(ulAvgUncloggedValue);

			if ( (m_lBHZ1ThresholdLimit > 0) && (ulThresholdValue < m_lBHZ1ThresholdLimit) )
			{
				szMsg.Format("BHZ2 Update THRESHOLD FAIL(too LOW) 1 - Curr = %lu, New = %lu, NewCJ = %lu, Lower limit1 = %lu", 
					m_ulBHZ2DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, m_lBHZ1ThresholdLimit); 
				BH_Z_AirFlowLog(FALSE, szMsg);
				bStatus = FALSE;
			}

			if ( (m_lBHZ1ThresholdUpperLimit > 0) && (ulThresholdValue > m_lBHZ1ThresholdUpperLimit) )
			{
				szMsg.Format("BHZ2 Update THRESHOLD FAIL(too UPPER) 1 - Curr = %lu, New = %lu, NewCJ = %lu, Upper limit1 = %lu", 
					m_ulBHZ2DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, m_lBHZ1ThresholdUpperLimit); 
				BH_Z_AirFlowLog(FALSE, szMsg);
				bStatus = FALSE;
			}

			if ( (m_lBHZ2ThresholdLimit > 0) && (ulThresholdValueCJ < m_lBHZ2ThresholdLimit) )
			{
				szMsg.Format("BHZ2 Update THRESHOLD FAIL 2 - Curr = %lu, New = %lu, NewCJ = %lu, limit1 = %lu", 
					m_ulBHZ2DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, m_lBHZ2ThresholdLimit); 
				BH_Z_AirFlowLog(FALSE, szMsg);
				//return FALSE;
				bStatus = FALSE;
			}

			m_ulBHZ2DAirFlowThreshold		= ulThresholdValue;
			m_ulBHZ2DAirFlowThresholdCJ		= ulThresholdValueCJ;
			m_pBHZ2AirFlowCompensation->ResetDieAirFlowValueList();
			szMsg.Format("BHZ2: THRESHOLD updated - Curr = %lu, New = %lu, NewCJ = %lu, Unclogged = %lu, Limit(%ld, %ld)", 
				m_ulBHZ2DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, ulAvgUncloggedValue,
				m_lBHZ1ThresholdLimit, m_lBHZ2ThresholdLimit); 
			BH_Z_AirFlowLog(FALSE, szMsg);
		}
	}
	else
	{
		LONG lValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		lValue = lValue - m_ulBHZ1DAirFlowSetZeroOffset;

		// filter noise value by using 2*curr_thres values
		if (lValue > _round(m_ulBHZ1DAirFlowThreshold * 2))
		{
			szMsg.Format("BHZ1 ADC value = %ld (%ld) is filtered",  lValue, m_ulBHZ1DAirFlowThreshold * 2);
			BH_Z_AirFlowLog(TRUE, szMsg);
			return TRUE;
		}

		m_ulBHZ1CurrThreshold[m_ulBHZ1CurrThresholdCount % BH_MAX_THRESHOLD_DAIRFLOW] = lValue;
		m_ulBHZ1CurrThresholdCount++;

		if (m_ulBHZ1CurrThresholdCount >= BH_MAX_THRESHOLD_DAIRFLOW)
		{
			m_ulBHZ1CurrThresholdCount = 0;

			DOUBLE dAverage = 0;
			ULONG ulHigh	= m_ulBHZ1CurrThreshold[0];
			ULONG ulLow		= m_ulBHZ1CurrThreshold[0];

			for (INT i=0; i<BH_MAX_THRESHOLD_DAIRFLOW; i++)
			{
				if (m_ulBHZ1CurrThreshold[i] > ulHigh)
					ulHigh	= m_ulBHZ2CurrThreshold[i];
				else if (m_ulBHZ1CurrThreshold[i] < ulLow)
					ulLow	= m_ulBHZ2CurrThreshold[i];

				dAverage = dAverage + m_ulBHZ1CurrThreshold[i];
				m_ulBHZ1CurrThreshold[i] = 0;
			}

			dAverage = (dAverage - ulLow - ulHigh) / (BH_MAX_THRESHOLD_DAIRFLOW - 2);
			ULONG ulAvgUncloggedValue = (ULONG) dAverage;

			ulThresholdValue = GetBHZ1MissingDieThresholdValue(ulAvgUncloggedValue);
			ulThresholdValueCJ = GetBHZ1ColletJamThresholdValue(ulAvgUncloggedValue);

			if ( (m_lBHZ1ThresholdLimit > 0) && (ulThresholdValue < m_lBHZ1ThresholdLimit) )
			{
				szMsg.Format("BHZ1 Update THRESHOLD FAIL(too Lower) 1 - Curr = %lu, New = %lu, NewCJ = %lu, limit1 = %lu", 
					m_ulBHZ1DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, m_lBHZ1ThresholdLimit); 
				BH_Z_AirFlowLog(TRUE, szMsg);
				//return FALSE;
				bStatus = FALSE;
			}

			if ( (m_lBHZ1ThresholdUpperLimit > 0) && (ulThresholdValue > m_lBHZ1ThresholdUpperLimit) )
			{
				szMsg.Format("BHZ1 Update THRESHOLD FAIL(too Upper) 1 - Curr = %lu, New = %lu, NewCJ = %lu, limit1 = %lu", 
					m_ulBHZ1DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, m_lBHZ1ThresholdUpperLimit); 
				BH_Z_AirFlowLog(TRUE, szMsg);
				bStatus = FALSE;
			}

			if ( (m_lBHZ2ThresholdLimit > 0) && (ulThresholdValueCJ < m_lBHZ2ThresholdLimit) )
			{
				szMsg.Format("BHZ1 Update THRESHOLD FAIL 2 - Curr = %lu, New = %lu, NewCJ = %lu, limit2 = %lu", 
					m_ulBHZ1DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, m_lBHZ2ThresholdLimit); 
				BH_Z_AirFlowLog(TRUE, szMsg);
				//return FALSE;
				bStatus = FALSE;
			}

			m_ulBHZ1DAirFlowThreshold		= ulThresholdValue;
			m_ulBHZ1DAirFlowThresholdCJ		= ulThresholdValueCJ;
			m_pBHZ1AirFlowCompensation->ResetDieAirFlowValueList();
			szMsg.Format("BHZ1: THRESHOLD updated - Curr = %lu, New = %lu, NewCJ = %lu, Unclogged = %lu, limit (%ld, %ld)", 
				m_ulBHZ1DAirFlowThreshold, ulThresholdValue, ulThresholdValueCJ, ulAvgUncloggedValue,
				m_lBHZ1ThresholdLimit, m_lBHZ2ThresholdLimit); 
			BH_Z_AirFlowLog(TRUE, szMsg);
		}
	}


	//v4.52A18
	if (!bStatus)
	{
		if (bBHZ2)
		{
			m_nDAirFlowBHZ2ThresFailCount++;
			if (m_nDAirFlowBHZ2ThresFailCount <= 1)
			{
				bStatus = TRUE;

				//Force to clean collet once
				if (m_ulMaxCleanCount > 0)
				{
					m_ulCleanCount = m_ulMaxCleanCount;
					szMsg.Format("Force BHZ2 Clean Collet - %d %d", m_nDAirFlowBHZ1ThresFailCount, m_nDAirFlowBHZ2ThresFailCount);
					BH_Z_AirFlowLog(FALSE, szMsg);
				}
			}
			else
			{
				SetErrorMessage(szMsg);
				BH_Z_AirFlowLog(FALSE, "BHZ2 DAirFlow Check fail to abort");
				HmiMessage_Red_Yellow(szMsg);
				bStatus = FALSE;
			}
		}
		else
		{
			m_nDAirFlowBHZ1ThresFailCount++;
			if (m_nDAirFlowBHZ1ThresFailCount <= 1)
			{
				bStatus = TRUE;

				//Force to clean collet once
				if (m_ulMaxCleanCount > 0)
				{
					m_ulCleanCount = m_ulMaxCleanCount;
					szMsg.Format("Force BHZ1 Clean Collet - %d %d", m_nDAirFlowBHZ1ThresFailCount, m_nDAirFlowBHZ2ThresFailCount);
					BH_Z_AirFlowLog(TRUE, szMsg);
				}
			}
			else
			{
				SetErrorMessage(szMsg);
				BH_Z_AirFlowLog(TRUE, "BHZ1 DAirFlow Check fail to abort");
				HmiMessage_Red_Yellow(szMsg);
				bStatus = FALSE;
			}
		}
	}

	return bStatus;
*/
}

BOOL CBondHead::BH_Z_AirFlowLog(CONST BOOL bIsZ1, CONST CString szText)
{
	//if( m_bMS100DigitalAirFlowSnr && m_bEnableDAFlowRunTimeCheck )
	//{
	//	if( bIsZ1 )
	//		CMSLogFileUtility::Instance()->BH_Z1Log(szText, FALSE);
	//	else
	//		CMSLogFileUtility::Instance()->BH_Z2Log(szText, FALSE);
	//}

	return TRUE;
}


LONG CBondHead::MoveChgColletTPitch(const LONG lIndex, INT nMode)
{
	return MS50ChgColletT_MoveTo(m_lAGCColletTPitch * (lIndex - 1) + m_lAGCColletStartAng, nMode);
}

BOOL CBondHead::IsMS50ChgColletZAtHome()
{
	return CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_MS50CHGCOLLET_Z,		&m_stBHAxis_ChgColletZ);
}
////////////////////////////////////////////
//	MS50 AGC Support Fcns
////////////////////////////////////////////
BOOL CBondHead::MS50ChgColletT_Home()
{
	INT nResult	= gnOK;

	m_bComm_ChgColletT = FALSE;
	m_bHome_ChgColletT = FALSE;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT) != 0)
			{
				m_bComm_ChgColletT = FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT);
			}

			if (!m_bComm_ChgColletT)
			{
				m_bHome_ChgColletT	= FALSE;
				if ((nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT)) == gnAMS_OK)
				{
					m_bComm_ChgColletT = TRUE;
				}
			}

			MS50ChgColletT_PowerOn(TRUE);

			if (nResult == gnAMS_OK)
			{
				if ((nResult = CMS896AStn::MotionMoveHome(BH_AXIS_MS50CHGCOLLET_T, 1, 0, &m_stBHAxis_ChgColletT)) == gnAMS_OK)
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BH_AXIS_MS50CHGCOLLET_T, 0, &m_stBHAxis_ChgColletT);
					m_bComm_ChgColletT		= TRUE;
					m_bHome_ChgColletT		= TRUE;
					m_lCurPos_ChgColletT	= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_ChgColletT	= FALSE;
					nResult				= gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT);
			CMS896AStn::MotionClearError(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT);

			m_bComm_ChgColletT	= FALSE;
			m_bHome_ChgColletT	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
	}
	
	return nResult;
}

BOOL CBondHead::MS50ChgColletZ_Home()
{
	INT nResult	= gnOK;

	m_bComm_ChgColletZ = FALSE;
	m_bHome_ChgColletZ = FALSE;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		try
		{
			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ) != 0)
			{
				m_bComm_ChgColletZ = FALSE;
				CMS896AStn::MotionClearError(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);
			}

			if (!m_bComm_ChgColletZ)
			{
				m_bHome_ChgColletZ	= FALSE;
				if ((nResult = CMS896AStn::MotionCommutateServo(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ)) == gnAMS_OK)
				{
					m_bComm_ChgColletZ = TRUE;
				}
			}

			MS50ChgColletZ_PowerOn(TRUE);

			if (nResult == gnAMS_OK)
			{
				if ((nResult = CMS896AStn::MotionMoveHome(BH_AXIS_MS50CHGCOLLET_Z, 1, 0, &m_stBHAxis_ChgColletZ)) == gnAMS_OK)
				{
					Sleep(100);
					CMS896AStn::MotionSetPosition(BH_AXIS_MS50CHGCOLLET_Z, 0, &m_stBHAxis_ChgColletZ);
					m_bComm_ChgColletZ		= TRUE;
					m_bHome_ChgColletZ		= TRUE;
					m_lCurPos_ChgColletZ	= 0;
				}
				else if (nResult == gnNOTOK)
				{
					m_bHome_ChgColletZ	= FALSE;
					nResult				= gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);
			CMS896AStn::MotionClearError(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);

			m_bComm_ChgColletZ	= FALSE;
			m_bHome_ChgColletZ	= FALSE;
			nResult = gnNOTOK;
		}
	}
	else
	{
	}
	
	return nResult;
}

//==================================================================
//		Change Collet T Move
//==================================================================
INT CBondHead::MS50ChgColletT_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bMS50ChgColletZT)
	{
		nResult = CMS896AStn::SetMotorPower(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT, bOn);
		if (nResult == gnOK)
		{
			m_bIsPowerOn_ChgColletT = bOn;
		}
	}

	return nResult;
}


INT CBondHead::MS50ChgColletT_MoveTo(INT nPos, INT nMode)
{
	INT nResult	= gnOK;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		try
		{
			if (!m_bHome_ChgColletT)
			{
				nResult	= Err_ChgColletTNotSafe;
			}

			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT) != 0)
			{
				m_bHome_ChgColletT	= FALSE;
				nResult		= Err_ChgColletTMove;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionSelectProfile(BH_AXIS_MS50CHGCOLLET_T, BH_MP_CHG_COLLET_T, &m_stBHAxis_ChgColletT);
				CMS896AStn::MotionMoveTo(BH_AXIS_MS50CHGCOLLET_T, nPos, SFM_NOWAIT, &m_stBHAxis_ChgColletT);

				m_lCurPos_ChgColletT	= nPos;

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BH_AXIS_MS50CHGCOLLET_T, 3000, &m_stBHAxis_ChgColletT);
					if (nResult != gnOK)
					{
						nResult = Err_ChgColletTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT);
			CMS896AStn::MotionClearError(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT);

			m_bHome_ChgColletT	= FALSE;
			nResult = gnNOTOK;
		}
	}

	if ( nResult != gnOK )
	{
		//SetMotionCE(TRUE, "BH Change Collet T moveto");
	}

	return nResult;
}

INT CBondHead::MS50ChgColletT_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		try
		{
			if (!m_bHome_ChgColletT)
			{
				nResult = Err_ChgColletTNotSafe;
			}

			if (CMS896AStn::MotionReportErrorStatus(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT) != 0)
			{
				m_bHome_ChgColletT	= FALSE;
				nResult		= Err_ChgColletTMove;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionSelectProfile(BH_AXIS_MS50CHGCOLLET_T, BH_MP_CHG_COLLET_T, &m_stBHAxis_ChgColletT);
				CMS896AStn::MotionMove(BH_AXIS_MS50CHGCOLLET_T, nPos, SFM_NOWAIT, &m_stBHAxis_ChgColletT);

				m_lCurPos_ChgColletT += nPos;

				if (nMode == SFM_WAIT)
				{
					nResult = CMS896AStn::MotionSync(BH_AXIS_MS50CHGCOLLET_T, 3000, &m_stBHAxis_ChgColletT);
					if (nResult != gnOK)
					{
						nResult = Err_ChgColletTMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT);

			m_bHome_ChgColletT	= FALSE;
			nResult = gnNOTOK;
		}
	}

	if ( nResult != gnOK )
	{
		//SetMotionCE(TRUE, "BH Change Collet T moveto");
	}

	return nResult;
}

INT CBondHead::MS50ChgColletT_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		try
		{
			nResult = CMS896AStn::MotionSync(BH_AXIS_MS50CHGCOLLET_T, 3000, &m_stBHAxis_ChgColletT);
			if (nResult != gnOK)
			{
				nResult = Err_ChgColletTMove;
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_T, &m_stBHAxis_ChgColletT);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		//SetMotionCE(TRUE, "BH Change Collet T Sync");
	}

	return nResult;
}


//==================================================================
//		Change Collet Z Move
//==================================================================
INT CBondHead::MS50ChgColletZ_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && m_bMS50ChgColletZT)
	{
		nResult = CMS896AStn::SetMotorPower(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ, bOn);
		if (nResult == gnOK)
		{
			m_bIsPowerOn_ChgColletZ = bOn;
		}
	}

	return nResult;
}

INT CBondHead::MS50ChgColletZ_MoveTo(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			if (!m_bHome_ChgColletZ)
			{
				nResult = Err_ChgColletZNotSafe;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionSelectProfile(BH_AXIS_MS50CHGCOLLET_Z, BH_MP_CHG_COLLET_Z, &m_stBHAxis_ChgColletZ);
				CMS896AStn::MotionMoveTo(BH_AXIS_MS50CHGCOLLET_Z, nPos, SFM_NOWAIT, &m_stBHAxis_ChgColletZ);
				m_lCurPos_ChgColletZ	= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_MS50CHGCOLLET_Z, 20000, &m_stBHAxis_ChgColletZ)) != gnOK)
					{
						nResult = Err_ChgColletZMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);
			CMS896AStn::MotionClearError(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);

			m_bHome_ChgColletZ	= FALSE;
			nResult = gnNOTOK;
		}
	}

	if ( nResult != gnOK )
	{
		//SetMotionCE(TRUE, "BH Change Collet Z MoveTo Fail");
	}

	return nResult;
}

INT CBondHead::MS50ChgColletZ_Move(INT nPos, INT nMode)
{
	INT nResult			= gnOK;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		if (IsMotionCE() == TRUE)
		{
			nResult = gnNOTOK;
			return nResult;
		}

		try
		{
			if (!m_bHome_ChgColletZ)
			{
				nResult = Err_ChgColletZNotSafe;
			}

			if (nResult == gnOK)
			{
				CMS896AStn::MotionSelectProfile(BH_AXIS_MS50CHGCOLLET_Z, BH_MP_CHG_COLLET_Z, &m_stBHAxis_ChgColletZ);
				CMS896AStn::MotionMove(BH_AXIS_MS50CHGCOLLET_Z, nPos, SFM_NOWAIT, &m_stBHAxis_ChgColletZ);
				m_lCurPos_ChgColletZ	+= nPos;

				if (nMode == SFM_WAIT)
				{
					if ((nResult = CMS896AStn::MotionSync(BH_AXIS_MS50CHGCOLLET_Z, 20000, &m_stBHAxis_ChgColletZ)) != gnOK)
					{
						nResult = Err_ChgColletZMove;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);
			CMS896AStn::MotionClearError(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);

			m_bHome_ChgColletZ	= FALSE;
			nResult = gnNOTOK;
		}
	}

	if ( nResult != gnOK )
	{
		//SetMotionCE(TRUE, "BH Change Collet Z MoveTo Fail");
	}

	return nResult;
}

INT CBondHead::MS50ChgColletZ_Sync()
{
	INT nResult = gnOK;

	if (m_fHardware && m_bMS50ChgColletZT)
	{
		try
		{
			if ((nResult = CMS896AStn::MotionSync(BH_AXIS_MS50CHGCOLLET_Z, 3000, &m_stBHAxis_ChgColletZ)) != gnOK)
			{
				nResult = Err_ChgColletZMove;
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ);
			nResult = gnNOTOK;
		}	
	}

	if ( nResult != gnOK )
	{
		//SetMotionCE(TRUE, "BH Change Collet Z Sync");
	}

	return nResult;
}


//==================================================================================
//			Setup Upload & Install Z and Collet Start Angle
//==================================================================================
LONG CBondHead::BH_MoveChgColletPosition(const LONG lPosition, const LONG lMovePosn)
{
	if (!m_bMS50ChgColletZT)
	{
		return 0;
	}

	switch (lPosition)
	{
		case 0:		//Holder Upload Z
		case 1:		//Holder Install Z
		case 3:		//Holder Blow Die Z
			if ((lMovePosn <= m_stBHAxis_ChgColletZ.m_lMaxPos) &&
				(lMovePosn >= m_stBHAxis_ChgColletZ.m_lMinPos))
			{
				return MS50ChgColletZ_MoveTo(lMovePosn);
			}
			break;
		case 2:		//Collet Start Angle
			if ((lMovePosn <= m_stBHAxis_ChgColletT.m_lMaxPos) &&
				(lMovePosn >= m_stBHAxis_ChgColletT.m_lMinPos))
			{
				return MS50ChgColletT_MoveTo(lMovePosn);
			}
			break;
	}
	return 0;
}

VOID CBondHead::BH_SetChgColletPosition(const LONG lPosition, const LONG lMovePosn)
{
	switch (lPosition)
	{
		case 0:		//Holder Upload Z
			m_lAGCHolderUploadZ = lMovePosn;
			break;
		case 1:		//Holder Install Z
			m_lAGCHolderInstallZ = lMovePosn;
			break;
		case 2:		//Collet Start Angle
			m_lAGCColletStartAng = lMovePosn;
			break;
		case 3:  //Holder Install Z
			m_lAGCHolderBlowDieZ = lMovePosn;
			break;
	}
}

LONG CBondHead::BH_GetChgColletPosition(const LONG lPosition, const LONG lMoveOffset)
{
	LONG lMovePosn = 0;
	switch (lPosition)
	{
		case 0:		//Holder Upload Z
			lMovePosn = m_lAGCHolderUploadZ + lMoveOffset;
			break;
		case 1:		//Holder Install Z
			lMovePosn = m_lAGCHolderInstallZ + lMoveOffset;
			break;
		case 2:		//Collet Start Angle
			lMovePosn = m_lAGCColletStartAng + lMoveOffset;
			break;
		case 3:    //Holder Blow die Z
			lMovePosn = m_lAGCHolderBlowDieZ + lMoveOffset;
	}
	return lMovePosn;
}

LONG CBondHead::BH_ChangeChgColletPosition(IPC_CServiceMessage &svMsg)
{
	LONG nErr = 0;
	BOOL bResult = TRUE;
	typedef struct 
	{
		LONG	lPosition;
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	LONG lMovePosn = BH_GetChgColletPosition(stInfo.lPosition, 0);
	m_lAGCSetupValue = lMovePosn;
	if (nErr = BH_MoveChgColletPosition(stInfo.lPosition, lMovePosn))
	{
		DisplayError(nErr);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_KeyInChgColletPosition(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	LONG nErr = BH_MoveChgColletPosition(stInfo.lPosition, stInfo.lStep);
	if (nErr)
	{
		DisplayError(nErr);
	}
	else
	{
		BH_SetChgColletPosition(stInfo.lPosition, stInfo.lStep);
	}
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_MovePosChgColletPosition(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	LONG lMovePosn = BH_GetChgColletPosition(stInfo.lPosition, stInfo.lStep);

	LONG nErr = BH_MoveChgColletPosition(stInfo.lPosition, lMovePosn);
	if (nErr)
	{
		DisplayError(nErr);
	}
	else
	{
		BH_SetChgColletPosition(stInfo.lPosition, lMovePosn);
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_MoveNegChgColletPosition(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	LONG lMovePosn = BH_GetChgColletPosition(stInfo.lPosition, -stInfo.lStep);

	LONG nErr = BH_MoveChgColletPosition(stInfo.lPosition, lMovePosn);
	if (nErr)
	{
		DisplayError(nErr);
	}
	else
	{
		BH_SetChgColletPosition(stInfo.lPosition, lMovePosn);
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::CancelChgColletSetup(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	LONG nErr = BH_MoveChgColletPosition(lPosition, 0);
	if (nErr)
	{
		DisplayError(nErr);
	}
	else
	{
		BH_SetChgColletPosition(lPosition, 0);  //m_lAGCSetupValue
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ConfirmChgColletSetup(IPC_CServiceMessage& svMsg)
{
	LONG	lPosition;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	BH_MoveChgColletPosition(lPosition, 0);
	//save Data
	SaveBhData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//==================================================================================

BOOL CBondHead::AGC_WTMoveDiff(BOOL bBHZ2, LONG lDiffX, LONG lDiffY)	//TestarBuyoff
{
	IPC_CServiceMessage rReqMsg;

	typedef struct 
	{
		BOOL bBHCollet2;
		LONG lDiffX;
		LONG lDiffY;
	} AGC_OFFSET;

	BOOL bResult = TRUE;
	AGC_OFFSET stInfo;
	stInfo.bBHCollet2 = bBHZ2;
	stInfo.lDiffX = lDiffX;
	stInfo.lDiffY = lDiffY;

	rReqMsg.InitMessage(sizeof(AGC_OFFSET), &stInfo);
	INT nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AGC_WTMoveDiff", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!bResult)
	{
		CString szErr = "BH AGC_WTMoveDiff fails.";
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}

