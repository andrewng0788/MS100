/////////////////////////////////////////////////////////////////
// NL_Command.cpp : interface of the CNVCLoader class
//
//	Description:
//		
//	Date:		23 June 2020
//	Revision:	1.00
//
//	By:	Andrew Ng		
//				
//	Copyright @ ASM Pacific Technology Ltd., .
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NVCLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CNVCLoader::RegisterVariables()
{
	try
	{
        CString str;
        
		// Register Variables
		//RegVariable(_T("BL_bIsEnabled"),		&m_bIsEnabled);
		RegVariable(_T("NL_bSel_X"),			&m_bSel_X);
		RegVariable(_T("NL_bSel_Y"),			&m_bSel_Y);
		RegVariable(_T("NL_bSel_Z1"),			&m_bSel_Z1);
		RegVariable(_T("NL_bSel_Z2"),			&m_bSel_Z2);

		RegVariable(_T("NL_lEnc_X"),			&m_lEnc_X);
		RegVariable(_T("NL_lEnc_Y"),			&m_lEnc_Y);
		RegVariable(_T("NL_lEnc_Z1"),			&m_lEnc_Z1);
		RegVariable(_T("NL_lEnc_Z2"),			&m_lEnc_Z2);
		
		RegVariable(_T("NL_ucLoopTestNo"),			&m_ucLoopTestNo);
		RegVariable(_T("NL_lLoopTestStartGrade"),	&m_lLoopTestStartGrade);
		RegVariable(_T("NL_lLoopTestEndGrade"),		&m_lLoopTestEndGrade);
		RegVariable(_T("NL_lDiagMoveDistance"),		&m_lDiagMoveDistance);

		RegVariable(_T("NL_bIsFrameExist1"),		&m_bIsGripper1FrameExist);
		RegVariable(_T("NL_bIsFrameExist2"),		&m_bIsGripper2FrameExist);

		RegVariable(_T("NL_lCurrBTBlock"),			&m_lCurrBTBlock);
		RegVariable(_T("NL_lCurrWafSlotID"),		&m_lCurrWafSlotID);
		RegVariable(_T("NL_lCurrWTBlock"),			&m_lCurrWTBlock);
		RegVariable(_T("NL_lRTCurrMgzn1"),			&m_lCurrHmiMgzn1);
		RegVariable(_T("NL_lRTCurrMgzn2"),			&m_lCurrHmiMgzn2);
		RegVariable(_T("NL_lRTCurrSlot1"),			&m_lCurrHmiSlot1);
		RegVariable(_T("NL_lRTCurrSlot2"),			&m_lCurrHmiSlot2);
		RegVariable(_T("NL_lRTCurrBlock1"),			&m_lCurrBlock1);
		RegVariable(_T("NL_lRTCurrBlock2"),			&m_lCurrBlock2);
		RegVariable(_T("NL_szCurrMgznDisplayName1"),	&m_szCurrHmiMgznName1);
		RegVariable(_T("NL_szCurrMgznDisplayName2"),	&m_szCurrHmiMgznName2);
		RegVariable(_T("NL_szRTBarcode1"),			&m_szFrameBarcode1);
		RegVariable(_T("NL_szRTBarcode2"),			&m_szFrameBarcode2);

		RegVariable(_T("NL_lRTOperationMode"),		&m_lOMRT);
		RegVariable(_T("NL_lSelMagazineID"),		&m_lSelMagazineID);
		RegVariable(_T("NL_lSelSlotID"),			&m_lSelSlotID);
		RegVariable(_T("NL_dSlotPitch"),			&m_dSlotPitch);

		RegVariable(_T("NL_lNLGeneral_1"),			&m_lNLGeneral_1);
        RegVariable(_T("NL_lNLGeneral_2"),			&m_lNLGeneral_2);
        RegVariable(_T("NL_lNLGeneral_3"),			&m_lNLGeneral_3);
        RegVariable(_T("NL_lNLGeneral_4"),			&m_lNLGeneral_4);
        RegVariable(_T("NL_lNLGeneral_5"),			&m_lNLGeneral_5);
        RegVariable(_T("NL_lNLGeneral_6"),			&m_lNLGeneral_6);
        RegVariable(_T("NL_lNLGeneral_7"),			&m_lNLGeneral_7);
		RegVariable(_T("NL_lNLGeneral_8"),			&m_lNLGeneral_8);
		RegVariable(_T("NL_lNLGeneral_9"),			&m_lNLGeneral_9);	
		RegVariable(_T("NL_lNLGeneral_10"),			&m_lNLGeneral_10);	
		RegVariable(_T("NL_lNLGeneral_11"),			&m_lNLGeneral_11);
		RegVariable(_T("NL_lNLGeneral_12"),			&m_lNLGeneral_12);
        RegVariable(_T("NL_lNLGeneral_TmpA"),		&m_lNLGeneral_TmpA);

        // operation mode setup
 		RegVariable(_T("NL_OM_szOMSP"),				&m_szOMSP);		
		RegVariable(_T("NL_bSetSlotBlockEnable"),	&m_bSetSlotBlockEnable);
		RegVariable(_T("NL_OM_szSlotUsage"),		&m_szNLSlotUsage);
        RegVariable(_T("NL_OM_lMgznSelect"),		&m_ulMgznSelected);
        RegVariable(_T("NL_OM_lSlotSelect"),		&m_ulSlotSelected);
        RegVariable(_T("NL_OM_lSlotBlock"),			&m_ulSlotPhyBlock);
		//RegVariable(_T("BL_OM_szSlotUsage"),		&m_szBLSlotUsage);
		for (INT i = 0; i<MS_BL_MGZN_NUM; i++)
        {
            str.Format("NL_OM_szMgznUse%d", i+1);
            RegVariable(str, &m_szNLMgznUse[i]);
        }

		RegVariable(_T("NL_lStandByPos_X"),			&m_lStandByPos_X);
		RegVariable(_T("NL_lStandByPos_Y"),			&m_lStandByPos_Y);
		RegVariable(_T("NL_lWTLoadUnloadPos1_X"),	&m_lWTLoadUnloadPos1_X);
		RegVariable(_T("NL_lWTLoadUnloadPos1_Y"),	&m_lWTLoadUnloadPos1_Y);
		RegVariable(_T("NL_lWTLoadUnloadPos2_X"),	&m_lWTLoadUnloadPos2_X);
		RegVariable(_T("NL_lWTLoadUnloadPos2_Y"),	&m_lWTLoadUnloadPos2_Y);	
		RegVariable(_T("NL_lBTLoadUnloadPos1_X"),	&m_lBTLoadUnloadPos1_X);
		RegVariable(_T("NL_lBTLoadUnloadPos1_Y"),	&m_lBTLoadUnloadPos1_Y);
		RegVariable(_T("NL_lBTLoadUnloadPos2_X"),	&m_lBTLoadUnloadPos2_X);
		RegVariable(_T("NL_lBTLoadUnloadPos2_Y"),	&m_lBTLoadUnloadPos2_Y);

		RegVariable(_T("NL_lStandByPos_Z1"),		&m_lStandByPos_Z1);
		RegVariable(_T("NL_lStandByPos_Z2"),		&m_lStandByPos_Z2);
		RegVariable(_T("NL_lWafLoadPos_Z1"),		&m_lWafLoadPos_Z1);
		RegVariable(_T("NL_lWafLoadPos_Z2"),		&m_lWafLoadPos_Z2);
		RegVariable(_T("NL_lBinLoadPos_Z1"),		&m_lBinLoadPos_Z1);
		RegVariable(_T("NL_lBinLoadPos_Z2"),		&m_lBinLoadPos_Z2);

		RegVariable(_T("NL_lWTUnloadOffsetX"),		&m_lWTUnloadOffsetX);
		RegVariable(_T("NL_lWTUnloadOffsetY"),		&m_lWTUnloadOffsetY);
		RegVariable(_T("NL_lWTUnloadOffsetZ"),		&m_lWTUnloadOffsetZ);
		RegVariable(_T("NL_lWTLoadOffsetX"),		&m_lWTLoadOffsetX);
		RegVariable(_T("NL_lWTLoadOffsetY"),		&m_lWTLoadOffsetY);
		RegVariable(_T("NL_lWTLoadOffsetZ"),		&m_lWTLoadOffsetZ);

		RegVariable(_T("NL_lBTUnloadOffsetX"),		&m_lBTUnloadOffsetX);
		RegVariable(_T("NL_lBTUnloadOffsetY"),		&m_lBTUnloadOffsetY);
		RegVariable(_T("NL_lBTUnloadOffsetZ"),		&m_lBTUnloadOffsetZ);
		RegVariable(_T("NL_lBTLoadOffsetX"),		&m_lBTLoadOffsetX);
		RegVariable(_T("NL_lBTLoadOffsetY"),		&m_lBTLoadOffsetY);
		RegVariable(_T("NL_lBTLoadOffsetZ"),		&m_lBTLoadOffsetZ);

		RegVariable(_T("NL_lMgznUnloadOffsetX"),	&m_lMgznUnloadOffsetX);
		RegVariable(_T("NL_lMgznUnloadOffsetY"),	&m_lMgznUnloadOffsetY);
		RegVariable(_T("NL_lMgznUnloadOffsetZ"),	&m_lMgznUnloadOffsetZ);
		RegVariable(_T("NL_lMgznLoadOffsetX"),		&m_lMgznLoadOffsetX);
		RegVariable(_T("NL_lMgznLoadOffsetY"),		&m_lMgznLoadOffsetY);
		RegVariable(_T("NL_lMgznLoadOffsetZ"),		&m_lMgznLoadOffsetZ);

		RegVariable(_T("NL_bNoSensorCheck"),			&m_bNoSensorCheck);
		RegVariable(_T("NL_bWTUseGripper1ForSetup"),	&m_bWTUseGripper1ForSetup);
		RegVariable(_T("NL_bBTUseGripper1ForSetup"),	&m_bBTUseGripper1ForSetup);

		DisplayMessage("NVCLoaderStn variables Registered...");


		////////////////////////////////////////////////////////////////////////////////////
		// Register Commands
		// Test Functions
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Test"),					&CNVCLoader::Test);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartLoopTest"),		&CNVCLoader::StartLoopTest);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopLoopTest"),			&CNVCLoader::StopLoopTest);
		// Loader Service/Setup Diagnostic Fcns  
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateOutput"),			&CNVCLoader::UpdateOutput);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),			&CNVCLoader::UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveData"),				&CNVCLoader::SaveData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerOnMotor"),			&CNVCLoader::PowerOnMotor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PowerOffMotor"),		&CNVCLoader::PowerOffMotor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HomeMotor"),			&CNVCLoader::HomeMotor);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagMoveX"),			&CNVCLoader::Diag_Move_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagMoveY"),			&CNVCLoader::Diag_Move_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagMoveZ1"),			&CNVCLoader::Diag_Move_Z1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagMoveZ2"),			&CNVCLoader::Diag_Move_Z2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagNegMoveX"),			&CNVCLoader::Diag_NegMove_X);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagNegMoveY"),			&CNVCLoader::Diag_NegMove_Y);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagNegMoveZ1"),		&CNVCLoader::Diag_NegMove_Z1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DiagNegMoveZ2"),		&CNVCLoader::Diag_NegMove_Z2);
		// Loader LOAD/UNLOAD XY pos Setup Fcns
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePosLoadUnloadPos"),		&CNVCLoader::MovePosLoadUnloadPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveNegLoadUnloadPos"),		&CNVCLoader::MoveNegLoadUnloadPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInLoadUnloadPos"),		&CNVCLoader::KeyInLoadUnloadPos);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachLoadUnloadPosXY"),		&CNVCLoader::TeachLoadUnloadPosXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmLoadUnloadPosXY"),	&CNVCLoader::ConfirmLoadUnloadPosXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelLoadUnloadPosXY"),	&CNVCLoader::CancelLoadUnloadPosXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveWTLoadOffsetXY"),		&CNVCLoader::MoveWTLoadOffsetXY);
		// Loader Z LOAD/UNLOAD pos Setup Fcns
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachLoadUnloadLevel"),		&CNVCLoader::TeachLoadUnloadLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmLoadUnloadLevel"),	&CNVCLoader::ConfirmLoadUnloadLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelLoadUnloadLevel"),	&CNVCLoader::CancelLoadUnloadLevel);
		// Loader XYZ MGZN pos setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMagazine"),			&CNVCLoader::SelectMagazine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeMagazineIndex"),		&CNVCLoader::ChangeMagazineIndex);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("KeyInValue"),				&CNVCLoader::KeyInValue);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("IncreaseValue"),			&CNVCLoader::IncreaseValue);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DecreaseValue"),			&CNVCLoader::DecreaseValue);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmMagazineSetup"),		&CNVCLoader::ConfirmMagazineSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelMagazineSetup"),		&CNVCLoader::CancelMagazineSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateSlotPitch"),			&CNVCLoader::UpdateSlotPitch);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToSlot"),				&CNVCLoader::MoveToSlot);
		//NVCLoader Slot Summary menu
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMagazineDisplay"),	&CNVCLoader::SelectMagazineDisplay);
		// Bin Operation Mode Setup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetOperationMode"),			&CNVCLoader::SetOperationMode);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectMgznSetup"),			&CNVCLoader::SelectOMMgznSetup);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectSlotSetup"),			&CNVCLoader::SelectOMSlotSetup);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetMgznUsage"),				&CNVCLoader::SetMgznUsage);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSlotUsage"),				&CNVCLoader::SetSlotUsage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSlotBlock"),				&CNVCLoader::SetSlotBlock);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadOperationMode"),		&CNVCLoader::LoadOperationMode);
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadCustomOpMode"),			&CNVCLoader::LoadCustomOpMode);	
        m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOperationMode"),		&CNVCLoader::SaveOperationMode);
		// Main BIN LOADER Functions
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadFilmFrame"),		&CNVCLoader::ManualLoadFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadFilmFrame"),	&CNVCLoader::ManualUnloadFilmFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadFrameFromMgzn"),		&CNVCLoader::ManualLoadFrameFromMgzn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadFrameToTable"),		&CNVCLoader::ManualLoadFrameToTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadFrameFromTable"),	&CNVCLoader::ManualUnloadFrameFromTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadFrameToMgzn"),		&CNVCLoader::ManualUnloadFrameToMgzn);

		// Main Wafer LOADER Functions
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadWaferFrame"),		&CNVCLoader::ManualLoadWaferFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadWaferFrame"),	&CNVCLoader::ManualUnloadWaferFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadWaferFromMgzn"),	&CNVCLoader::ManualLoadWaferFromMgzn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualLoadWaferToTable"),	&CNVCLoader::ManualLoadWaferToTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadWaferFromTable"),	&CNVCLoader::ManualUnloadWaferFromTable);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualUnloadWaferToMgzn"),	&CNVCLoader::ManualUnloadWaferToMgzn);
		//Other Main LOADER Functions
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetMagazineCmd"),			&CNVCLoader::ResetMagazineCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RealignBinFrameCmd"),       &CNVCLoader::RealignBinFrameCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OpenCloseWafExpander"),		&CNVCLoader::OpenCloseWafExpander);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OpenCloseBinExpander"),		&CNVCLoader::OpenCloseBinExpander);
		// IO Setup Functions
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetBinExpanderSol"),		&CNVCLoader::SetBinExpanderSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetWafExpanderSol"),		&CNVCLoader::SetWafExpanderSol);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OpenCloseWafGripper"),		&CNVCLoader::OpenCloseWafGripper);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("OpenCloseBinGripper"),		&CNVCLoader::OpenCloseBinGripper);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}


VOID CNVCLoader::UpdateAllSGVariables()
{
}

LONG CNVCLoader::Test(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);


	LoadMgznRTData();

	/*
	if (bEnable)
	{
		HmiMessage("Set BinExp Open TRUE ....");
		SetWafGripperOpen(TRUE);

		HmiMessage("Set WafExp Open TRUE ....");
		SetWafGripperClose(TRUE);

	}
	else
	{
		HmiMessage("Set BinExp Open FALSE ....");
		SetWafGripperOpen(FALSE);

		HmiMessage("Set WafExp Open FALSE ....");
		SetWafGripperClose(FALSE);

	}
	*/

	HmiMessage("Test DONE");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::StartLoopTest(IPC_CServiceMessage& svMsg)
{
	CString szMsg;
	szMsg.Format("Start NVC Loader Loop Test #%d ...", m_ucLoopTestNo);

	MoveXYToStandby(TRUE, TRUE);

	SetWafGripper(TRUE);
	SetBinGripper(TRUE);

	HmiMessage(szMsg);
	SetStatusMessage(szMsg);

	m_lCurrMgzn1	= 0;
	m_lCurrSlotID1	= 0;
	m_lCurrBlock1	= 0;
	m_lCurrMgzn2	= 0;
	m_lCurrSlotID2	= 0;
	m_lCurrBlock2	= 0;
	m_lCurrWTBlock	= 0;
	m_lCurrBTBlock	= 0;

	m_lCurrWafSlotID = 1;

	if (m_ucLoopTestNo == 6)
	{
		m_lCurrWafSlotID = m_lLoopTestStartGrade;
	}

	m_bLoopTestForward = TRUE;
	m_bLoopTestAbort = FALSE;
	m_bLoopTestStart = TRUE;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CNVCLoader::StopLoopTest(IPC_CServiceMessage& svMsg)
{
	m_bLoopTestAbort = TRUE;

	SetStatusMessage("NVC Loader Loop Test is stopped");
	//HmiMessage("NVC Loader Loop Test is stopped");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CNVCLoader::UpdateOutput(IPC_CServiceMessage& svMsg)
{
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	m_bUpdateOutput = bEnable;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::UpdateAction(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::SaveData(IPC_CServiceMessage& svMsg)
{
	SaveData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::PowerOnMotor(IPC_CServiceMessage& svMsg)
{
	LONG lMotorID = 0;
	svMsg.GetMsg(sizeof(LONG), &lMotorID);

	switch (lMotorID)
	{
	case 1:		//X
		HmiMessage("Power ON X ...");
		X_PowerOn(TRUE);
		break;
	case 2:		//Y
		HmiMessage("Power ON Y ...");
		Y_PowerOn(TRUE);
		break;
	case 3:		//Z1
		HmiMessage("Power ON Z1 ...");
		Z1_PowerOn(TRUE);
		break;
	case 4:		//Z2
		HmiMessage("Power ON Z2 ...");
		Z2_PowerOn(TRUE);
		break;

	default:
		HmiMessage("Power ON ALL ...");
		X_PowerOn(TRUE);
		Y_PowerOn(TRUE);
		Z1_PowerOn(TRUE);
		Z2_PowerOn(TRUE);
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::PowerOffMotor(IPC_CServiceMessage& svMsg)
{
	LONG lMotorID = 0;
	svMsg.GetMsg(sizeof(LONG), &lMotorID);

	switch (lMotorID)
	{
	case 1:		//X
		HmiMessage("Power OFF X ...");
		X_PowerOn(FALSE);
		break;
	case 2:		//Y
		HmiMessage("Power OFF Y ...");
		Y_PowerOn(FALSE);
		break;
	case 3:		//Z1
		HmiMessage("Power OFF Z1 ...");
		Z1_PowerOn(FALSE);
		break;
	case 4:		//Z2
		HmiMessage("Power OFF Z2 ...");
		Z2_PowerOn(FALSE);
		break;

	default:
		HmiMessage("Power OFF ALL ...");
		X_PowerOn(FALSE);
		Y_PowerOn(FALSE);
		Z1_PowerOn(FALSE);
		Z2_PowerOn(FALSE);
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::HomeMotor(IPC_CServiceMessage& svMsg)
{
	LONG lMotorID = 0;
	svMsg.GetMsg(sizeof(LONG), &lMotorID);

	switch (lMotorID)
	{
	case 1:		//X
		HmiMessage("HOME X ...");
		X_Home();
		break;
	case 2:		//Y
		HmiMessage("HOME Y ...");
		Y_Home();
		break;
	case 3:		//Z1
		HmiMessage("HOME Z1 ...");
		Z1_Home();
		break;
	case 4:		//Z2
		HmiMessage("HOME Z2 ...");
		Z2_Home();
		break;

	default:
		HmiMessage("HOME ALL ...");
		HomeNLModule();
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// NVCLoader Motor Diagnostic MOVE menu fcn
//////////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::Diag_Move_X(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = lDist;

	LONG lCmdPosX = CMS896AStn::MotionGetCommandPosition(NL_AXIS_X, &m_stNLAxis_X);

	CString szLog;
	szLog.Format("Diag_Move_X: Target = %ld + %ld (%ld, %ld)", lCmdPosX, lDiagMoveDistance, m_lLLimit_X, m_lHLimit_X);
	HmiMessage(szLog);

	if ( (m_lLLimit_X <= lCmdPosX + lDiagMoveDistance) && (m_lHLimit_X >= lCmdPosX + lDiagMoveDistance) )
	{
		X_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::Diag_NegMove_X(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = -1 * lDist;

	LONG lCmdPosX = CMS896AStn::MotionGetCommandPosition(NL_AXIS_X, &m_stNLAxis_X);

	CString szLog;
	szLog.Format("Diag_NegMove_X: Target = %ld + %ld (%ld, %ld)", lCmdPosX, lDiagMoveDistance, m_lLLimit_X, m_lHLimit_X);
	HmiMessage(szLog);

	if ( (m_lLLimit_X <= lCmdPosX + lDiagMoveDistance) && (m_lHLimit_X >= lCmdPosX + lDiagMoveDistance) )
	{
		X_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::Diag_Move_Y(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = lDist;

	LONG lCmdPosY = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Y, &m_stNLAxis_Y);

	CString szLog;
	szLog.Format("Diag_Move_Y: Target = %ld + %ld (%ld, %ld)", lCmdPosY, lDiagMoveDistance, m_lLLimit_Y, m_lHLimit_Y);
	HmiMessage(szLog);

	if ( (m_lLLimit_Y <= lCmdPosY + lDiagMoveDistance) && (m_lHLimit_Y >= lCmdPosY + lDiagMoveDistance) )
	{
		Y_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::Diag_NegMove_Y(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = -1 * lDist;

	LONG lCmdPosY = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Y, &m_stNLAxis_Y);

	CString szLog;
	szLog.Format("Diag_NegMove_Y: Target = %ld + %ld (%ld, %ld)", lCmdPosY, lDiagMoveDistance, m_lLLimit_Y, m_lHLimit_Y);
	HmiMessage(szLog);

	if ( (m_lLLimit_Y <= lCmdPosY + lDiagMoveDistance) && (m_lHLimit_Y >= lCmdPosY + lDiagMoveDistance) )
	{
		Y_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::Diag_Move_Z1(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = lDist;

	LONG lCmdPosZ = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z1, &m_stNLAxis_Z1);

	CString szLog;
	szLog.Format("Diag_Move_Z1: Target = %ld + %ld (%ld, %ld)", lCmdPosZ, lDiagMoveDistance, m_lLLimit_Z1, m_lHLimit_Z1);
	HmiMessage(szLog);

	if ( (m_lLLimit_Z1 <= lCmdPosZ + lDiagMoveDistance) && (m_lHLimit_Z1 >= lCmdPosZ + lDiagMoveDistance) )
	{
		Z1_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::Diag_NegMove_Z1(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = -1 * lDist;

	LONG lCmdPosZ = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z1, &m_stNLAxis_Z1);

	CString szLog;
	szLog.Format("Diag_NegMove_Z1: Target = %ld + %ld (%ld, %ld)", lCmdPosZ, lDiagMoveDistance, m_lLLimit_Z1, m_lHLimit_Z1);
	HmiMessage(szLog);

	if ( (m_lLLimit_Z1 <= lCmdPosZ + lDiagMoveDistance) && (m_lHLimit_Z1 >= lCmdPosZ + lDiagMoveDistance) )
	{
		Z1_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::Diag_Move_Z2(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = lDist;

	LONG lCmdPosZ = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z2, &m_stNLAxis_Z2);

	CString szLog;
	szLog.Format("Diag_Move_Z2: Target = %ld + %ld (%ld, %ld)", lCmdPosZ, lDiagMoveDistance, m_lLLimit_Z2, m_lHLimit_Z2);
	HmiMessage(szLog);

	if ( (m_lLLimit_Z2 <= lCmdPosZ + lDiagMoveDistance) && (m_lHLimit_Z2 >= lCmdPosZ + lDiagMoveDistance) )
	{
		Z2_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::Diag_NegMove_Z2(IPC_CServiceMessage& svMsg)
{
	LONG lDist = 0;
	svMsg.GetMsg(sizeof(LONG), &lDist);

	LONG lDiagMoveDistance = -1 * lDist;

	LONG lCmdPosZ = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z2, &m_stNLAxis_Z2);

	CString szLog;
	szLog.Format("Diag_NegMove_Z2: Target = %ld + %ld (%ld, %ld)", lCmdPosZ, lDiagMoveDistance, m_lLLimit_Z2, m_lHLimit_Z2);
	HmiMessage(szLog);

	if ( (m_lLLimit_Z2 <= lCmdPosZ + lDiagMoveDistance) && (m_lHLimit_Z2 >= lCmdPosZ + lDiagMoveDistance) )
	{
		Z2_Move(lDiagMoveDistance);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// NVCLoader XY Setup menu
//////////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::MovePosLoadUnloadPos(IPC_CServiceMessage& svMsg)
{
	LONG lStepSize = 0;
	svMsg.GetMsg(sizeof(LONG), &lStepSize);

	if (!CheckGripperFrameExistForXYMove(TRUE) || !CheckGripperFrameExistForXYMove(FALSE))
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	switch (m_lBackupPosition)
	{
	case 1:
		m_lStandByPos_X = m_lStandByPos_X + lStepSize;
		if (IsWithinXLimit(m_lStandByPos_X))
		{
			X_MoveTo(m_lStandByPos_X);
		}
		break;
	case 2:
		m_lStandByPos_Y = m_lStandByPos_Y + lStepSize;
		if (IsWithinYLimit(m_lStandByPos_Y))
		{
			Y_MoveTo(m_lStandByPos_Y);
		}
		break;
	case 3:
		m_lWTLoadUnloadPos1_X = m_lWTLoadUnloadPos1_X + lStepSize;
		if (IsWithinXLimit(m_lWTLoadUnloadPos1_X))
		{
			X_MoveTo(m_lWTLoadUnloadPos1_X);
		}
		break;
	case 4:
		m_lWTLoadUnloadPos1_Y = m_lWTLoadUnloadPos1_Y + lStepSize;
		if (IsWithinYLimit(m_lWTLoadUnloadPos1_Y))
		{
			Y_MoveTo(m_lWTLoadUnloadPos1_Y);
		}
		break;
	case 5:
		m_lWTLoadUnloadPos2_X = m_lWTLoadUnloadPos2_X + lStepSize;
		if (IsWithinXLimit(m_lWTLoadUnloadPos2_X))
		{
			X_MoveTo(m_lWTLoadUnloadPos2_X);
		}
		break;
	case 6:
		m_lWTLoadUnloadPos2_Y = m_lWTLoadUnloadPos2_Y + lStepSize;
		if (IsWithinYLimit(m_lWTLoadUnloadPos2_Y))
		{
			Y_MoveTo(m_lWTLoadUnloadPos2_Y);
		}
		break;
	case 7:
		m_lBTLoadUnloadPos1_X = m_lBTLoadUnloadPos1_X + lStepSize;
		if (IsWithinXLimit(m_lBTLoadUnloadPos1_X))
		{
			X_MoveTo(m_lBTLoadUnloadPos1_X);
		}
		break;
	case 8:
		m_lBTLoadUnloadPos1_Y = m_lBTLoadUnloadPos1_Y + lStepSize;
		if (IsWithinYLimit(m_lBTLoadUnloadPos1_Y))
		{
			Y_MoveTo(m_lBTLoadUnloadPos1_Y);
		}
		break;
	case 9:
		m_lBTLoadUnloadPos2_X = m_lBTLoadUnloadPos2_X + lStepSize;
		if (IsWithinXLimit(m_lBTLoadUnloadPos2_X))
		{
			X_MoveTo(m_lBTLoadUnloadPos2_X);
		}
		break;
	case 10:
		m_lBTLoadUnloadPos2_Y = m_lBTLoadUnloadPos2_Y + lStepSize;
		if (IsWithinYLimit(m_lBTLoadUnloadPos2_Y))
		{
			Y_MoveTo(m_lBTLoadUnloadPos2_Y);
		}
		break;

	case 11:
		m_lStandByPos_Z1 = m_lStandByPos_Z1 + lStepSize;
		if (IsWithinZ1Limit(m_lStandByPos_Z1))
		{
			Z1_MoveTo(m_lStandByPos_Z1);
		}
		break;
	case 12:
		m_lStandByPos_Z2 = m_lStandByPos_Z2 + lStepSize;
		if (IsWithinZ2Limit(m_lStandByPos_Z2))
		{
			Z2_MoveTo(m_lStandByPos_Z2);
		}
		break;

	case 13:
		m_lWafLoadPos_Z1 = m_lWafLoadPos_Z1 + lStepSize;
		if (IsWithinZ1Limit(m_lWafLoadPos_Z1))
		{
			Z1_MoveTo(m_lWafLoadPos_Z1);
		}
		break;
	case 14:
		m_lWafLoadPos_Z2 = m_lWafLoadPos_Z2 + lStepSize;
		if (IsWithinZ2Limit(m_lWafLoadPos_Z2))
		{
			Z2_MoveTo(m_lWafLoadPos_Z2);
		}
		break;

	case 15:
		m_lBinLoadPos_Z1 = m_lBinLoadPos_Z1 + lStepSize;
		if (IsWithinZ1Limit(m_lBinLoadPos_Z1))
		{
			Z1_MoveTo(m_lBinLoadPos_Z1);
		}
		break;
	case 16:
		m_lBinLoadPos_Z2 = m_lBinLoadPos_Z2 + lStepSize;
		if (IsWithinZ2Limit(m_lBinLoadPos_Z2))
		{
			Z2_MoveTo(m_lBinLoadPos_Z2);
		}
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::MoveNegLoadUnloadPos(IPC_CServiceMessage& svMsg)
{
	LONG lStepSize = 0;
	svMsg.GetMsg(sizeof(LONG), &lStepSize);

	if (!CheckGripperFrameExistForXYMove(TRUE) || !CheckGripperFrameExistForXYMove(FALSE))
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	switch (m_lBackupPosition)
	{
	case 1:
		m_lStandByPos_X = m_lStandByPos_X - lStepSize;
		if (IsWithinXLimit(m_lStandByPos_X))
		{
			X_MoveTo(m_lStandByPos_X);
		}
		break;
	case 2:
		m_lStandByPos_Y = m_lStandByPos_Y - lStepSize;
		if (IsWithinYLimit(m_lStandByPos_Y))
		{
			Y_MoveTo(m_lStandByPos_Y);
		}
		break;
	case 3:
		m_lWTLoadUnloadPos1_X = m_lWTLoadUnloadPos1_X - lStepSize;
		if (IsWithinXLimit(m_lWTLoadUnloadPos1_X))
		{
			X_MoveTo(m_lWTLoadUnloadPos1_X);
		}
		break;
	case 4:
		m_lWTLoadUnloadPos1_Y = m_lWTLoadUnloadPos1_Y - lStepSize;
		if (IsWithinYLimit(m_lWTLoadUnloadPos1_Y))
		{
			Y_MoveTo(m_lWTLoadUnloadPos1_Y);
		}
		break;
	case 5:
		m_lWTLoadUnloadPos2_X = m_lWTLoadUnloadPos2_X - lStepSize;
		if (IsWithinXLimit(m_lWTLoadUnloadPos2_X))
		{
			X_MoveTo(m_lWTLoadUnloadPos2_X);
		}
		break;
	case 6:
		m_lWTLoadUnloadPos2_Y = m_lWTLoadUnloadPos2_Y - lStepSize;
		if (IsWithinYLimit(m_lWTLoadUnloadPos2_Y))
		{
			Y_MoveTo(m_lWTLoadUnloadPos2_Y);
		}
		break;
	case 7:
		m_lBTLoadUnloadPos1_X = m_lBTLoadUnloadPos1_X - lStepSize;
		if (IsWithinXLimit(m_lBTLoadUnloadPos1_X))
		{
			X_MoveTo(m_lBTLoadUnloadPos1_X);
		}
		break;
	case 8:
		m_lBTLoadUnloadPos1_Y = m_lBTLoadUnloadPos1_Y - lStepSize;
		if (IsWithinYLimit(m_lBTLoadUnloadPos1_Y))
		{
			Y_MoveTo(m_lBTLoadUnloadPos1_Y);
		}
		break;
	case 9:
		m_lBTLoadUnloadPos2_X = m_lBTLoadUnloadPos2_X - lStepSize;
		if (IsWithinXLimit(m_lBTLoadUnloadPos2_X))
		{
			X_MoveTo(m_lBTLoadUnloadPos2_X);
		}
		break;
	case 10:
		m_lBTLoadUnloadPos2_Y = m_lBTLoadUnloadPos2_Y - lStepSize;
		if (IsWithinYLimit(m_lBTLoadUnloadPos2_Y))
		{
			Y_MoveTo(m_lBTLoadUnloadPos2_Y);
		}
		break;

	case 11:
		m_lStandByPos_Z1 = m_lStandByPos_Z1 - lStepSize;
		if (IsWithinZ1Limit(m_lStandByPos_Z1))
		{
			Z1_MoveTo(m_lStandByPos_Z1);
		}
		break;
	case 12:
		m_lStandByPos_Z2 = m_lStandByPos_Z2 - lStepSize;
		if (IsWithinZ2Limit(m_lStandByPos_Z2))
		{
			Z2_MoveTo(m_lStandByPos_Z2);
		}
		break;

	case 13:
		m_lWafLoadPos_Z1 = m_lWafLoadPos_Z1 - lStepSize;
		if (IsWithinZ1Limit(m_lWafLoadPos_Z1))
		{
			Z1_MoveTo(m_lWafLoadPos_Z1);
		}
		break;
	case 14:
		m_lWafLoadPos_Z2 = m_lWafLoadPos_Z2 - lStepSize;
		if (IsWithinZ2Limit(m_lWafLoadPos_Z2))
		{
			Z2_MoveTo(m_lWafLoadPos_Z2);
		}
		break;

	case 15:
		m_lBinLoadPos_Z1 = m_lBinLoadPos_Z1 - lStepSize;
		if (IsWithinZ1Limit(m_lBinLoadPos_Z1))
		{
			Z1_MoveTo(m_lBinLoadPos_Z1);
		}
		break;
	case 16:
		m_lBinLoadPos_Z2 = m_lBinLoadPos_Z2 - lStepSize;
		if (IsWithinZ2Limit(m_lBinLoadPos_Z2))
		{
			Z2_MoveTo(m_lBinLoadPos_Z2);
		}
		break;

	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::KeyInLoadUnloadPos(IPC_CServiceMessage& svMsg)
{
	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if (!CheckGripperFrameExistForXYMove(TRUE) || !CheckGripperFrameExistForXYMove(FALSE))
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	switch (m_lBackupPosition)
	{
	case 1:
		m_lStandByPos_X = lPosition;
		if (IsWithinXLimit(m_lStandByPos_X))
		{
			X_MoveTo(m_lStandByPos_X);
		}
		else
		{
			m_lStandByPos_X = m_lBackupPos_X;
		}
		break;
	case 2:
		m_lStandByPos_Y = lPosition;
		if (IsWithinYLimit(m_lStandByPos_Y))
		{
			Y_MoveTo(m_lStandByPos_Y);
		}
		else
		{
			m_lStandByPos_Y = m_lBackupPos_Y;
		}
		break;
	case 3:
		m_lWTLoadUnloadPos1_X = lPosition;
		if (IsWithinXLimit(m_lWTLoadUnloadPos1_X))
		{
			X_MoveTo(m_lWTLoadUnloadPos1_X);
		}
		else
		{
			m_lWTLoadUnloadPos1_X = m_lBackupPos_X;
		}
		break;
	case 4:
		m_lWTLoadUnloadPos1_Y = lPosition;
		if (IsWithinYLimit(m_lWTLoadUnloadPos1_Y))
		{
			Y_MoveTo(m_lWTLoadUnloadPos1_Y);
		}
		else
		{
			m_lWTLoadUnloadPos1_Y = m_lBackupPos_Y;
		}
		break;
	case 5:
		m_lWTLoadUnloadPos2_X = lPosition;
		if (IsWithinXLimit(m_lWTLoadUnloadPos2_X))
		{
			X_MoveTo(m_lWTLoadUnloadPos2_X);
		}
		else
		{
			m_lWTLoadUnloadPos2_X = m_lBackupPos_X;
		}
		break;
	case 6:
		m_lWTLoadUnloadPos2_Y = lPosition;
		if (IsWithinYLimit(m_lWTLoadUnloadPos2_Y))
		{
			Y_MoveTo(m_lWTLoadUnloadPos2_Y);
		}
		else
		{
			m_lWTLoadUnloadPos2_Y = m_lBackupPos_Y;
		}
		break;
	case 7:
		m_lBTLoadUnloadPos1_X = lPosition;
		if (IsWithinXLimit(m_lBTLoadUnloadPos1_X))
		{
			X_MoveTo(m_lBTLoadUnloadPos1_X);
		}
		else
		{
			m_lBTLoadUnloadPos1_X = m_lBackupPos_X;
		}
		break;
	case 8:
		m_lBTLoadUnloadPos1_Y = lPosition;
		if (IsWithinYLimit(m_lBTLoadUnloadPos1_Y))
		{
			Y_MoveTo(m_lBTLoadUnloadPos1_Y);
		}
		else
		{
			m_lBTLoadUnloadPos1_Y = m_lBackupPos_Y;
		}
		break;
	case 9:
		m_lBTLoadUnloadPos2_X = lPosition;
		if (IsWithinXLimit(m_lBTLoadUnloadPos2_X))
		{
			X_MoveTo(m_lBTLoadUnloadPos2_X);
		}
		else
		{
			m_lBTLoadUnloadPos2_X = m_lBackupPos_X;
		}
		break;
	case 10:
		m_lBTLoadUnloadPos2_Y = lPosition;
		if (IsWithinYLimit(m_lBTLoadUnloadPos2_Y))
		{
			Y_MoveTo(m_lBTLoadUnloadPos2_Y);
		}
		else
		{
			m_lBTLoadUnloadPos2_Y = m_lBackupPos_Y;
		}
		break;

	case 11:
		m_lStandByPos_Z1 = lPosition;
		if (IsWithinZ1Limit(m_lStandByPos_Z1))
		{
			Z1_MoveTo(m_lStandByPos_Z1);
		}
		else
		{
			m_lStandByPos_Z1 = m_lBackupPos_Z;
		}
		break;
	case 12:
		m_lStandByPos_Z2 = lPosition;
		if (IsWithinZ2Limit(m_lStandByPos_Z2))
		{
			Z2_MoveTo(m_lStandByPos_Z2);
		}
		else
		{
			m_lStandByPos_Z2 = m_lBackupPos_Z;
		}
		break;

	case 13:
		m_lWafLoadPos_Z1 = lPosition;
		if (IsWithinZ1Limit(m_lWafLoadPos_Z1))
		{
			Z1_MoveTo(m_lWafLoadPos_Z1);
		}
		else
		{
			m_lWafLoadPos_Z1 = m_lBackupPos_Z;
		}
		break;
	case 14:
		m_lWafLoadPos_Z2 = lPosition;
		if (IsWithinZ2Limit(m_lWafLoadPos_Z2))
		{
			Z2_MoveTo(m_lWafLoadPos_Z2);
		}
		else
		{
			m_lWafLoadPos_Z2 = m_lBackupPos_Z;
		}
		break;

	case 15:
		m_lBinLoadPos_Z1 = lPosition;
		if (IsWithinZ1Limit(m_lBinLoadPos_Z1))
		{
			Z1_MoveTo(m_lBinLoadPos_Z1);
		}
		else
		{
			m_lBinLoadPos_Z1 = m_lBackupPos_Z;
		}
		break;
	case 16:
		m_lBinLoadPos_Z2 = lPosition;
		if (IsWithinZ2Limit(m_lBinLoadPos_Z2))
		{
			Z2_MoveTo(m_lBinLoadPos_Z2);
		}
		else
		{
			m_lBinLoadPos_Z2 = m_lBackupPos_Z;
		}
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::TeachLoadUnloadPosXY(IPC_CServiceMessage& svMsg)
{
	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	m_lBackupPosition = lPosition;

	CString szMsg;
	szMsg.Format("Position = %ld", m_lBackupPosition);
	HmiMessage(szMsg);

	switch (lPosition)
	{
	case 1:
	case 2:
		m_lBackupPos_X = m_lStandByPos_X;
		m_lBackupPos_Y = m_lStandByPos_Y;
		break;
	case 3:
	case 4:
		m_lBackupPos_X = m_lWTLoadUnloadPos1_X;
		m_lBackupPos_Y = m_lWTLoadUnloadPos1_Y;
		break;
	case 5:
	case 6:
		m_lBackupPos_X = m_lWTLoadUnloadPos2_X;
		m_lBackupPos_Y = m_lWTLoadUnloadPos2_Y;
		break;
	case 7:
	case 8:
		m_lBackupPos_X = m_lBTLoadUnloadPos1_X;
		m_lBackupPos_Y = m_lBTLoadUnloadPos1_Y;
		break;
	case 9:
	case 10:
		m_lBackupPos_X = m_lBTLoadUnloadPos2_X;
		m_lBackupPos_Y = m_lBTLoadUnloadPos2_Y;
		break;
	}


	if (!CheckGripperFrameExistForXYMove(TRUE) || !CheckGripperFrameExistForXYMove(FALSE))
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	CString szTemp;
	szTemp.Format("Loader XY will move to (%ld, %ld) at# %d", m_lBackupPos_X, m_lBackupPos_Y, lPosition);
	HmiMessage(szTemp);
	XY_MoveTo(m_lBackupPos_X, m_lBackupPos_Y);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CNVCLoader::ConfirmLoadUnloadPosXY(IPC_CServiceMessage& svMsg)
{
	SaveData();

	if (m_lBackupPosition >= 11)
	{
		//Z1_MoveTo(0, SFM_NOWAIT);
		//Z2_MoveTo(0);
		Z1_Home();
		Z2_Home();
		HmiMessage("Loader Z will move back to HOME ....");
		XY_MoveTo(0, 0);
	}
	else
	{
		Z1_Home();
		Z2_Home();
		HmiMessage("Loader XY will move back to HOME ....");
		XY_MoveTo(0, 0);
	}

	m_lBackupPosition = 0;
	m_lBackupPos_X = 0;
	m_lBackupPos_Y = 0;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::CancelLoadUnloadPosXY(IPC_CServiceMessage& svMsg)
{
	switch (m_lBackupPosition)
	{
	case 1:
	case 2:
		m_lStandByPos_X			= m_lBackupPos_X;
		m_lStandByPos_Y			= m_lBackupPos_Y;
		break;
	case 3:
	case 4:
		m_lWTLoadUnloadPos1_X	= m_lBackupPos_X;
		m_lWTLoadUnloadPos1_Y	= m_lBackupPos_Y;
		break;
	case 5:
	case 6:
		m_lWTLoadUnloadPos2_X	= m_lBackupPos_X;
		m_lWTLoadUnloadPos2_Y	= m_lBackupPos_Y;
		break;
	case 7:
	case 8:
		m_lBTLoadUnloadPos1_X	= m_lBackupPos_X;
		m_lBTLoadUnloadPos1_Y	= m_lBackupPos_Y;
		break;
	case 9:
	case 10:
		m_lBTLoadUnloadPos2_X	= m_lBackupPos_X;
		m_lBTLoadUnloadPos2_Y	= m_lBackupPos_Y;
		break;

	case 11:
		m_lStandByPos_Z1 = m_lBackupPos_Z;
		break;
	case 12:
		m_lStandByPos_Z2 = m_lBackupPos_Z;
		break;
	
	case 13:
		m_lWafLoadPos_Z1 = m_lBackupPos_Z;
		break;
	case 14:
		m_lWafLoadPos_Z2 = m_lBackupPos_Z;
		break;

	case 15:
		m_lBinLoadPos_Z1 = m_lBackupPos_Z;
		break;
	case 16:
		m_lBinLoadPos_Z2 = m_lBackupPos_Z;
		break;
	}

	if (m_lBackupPosition >= 11)
	{
		Z1_Home();
		Z2_Home();
		HmiMessage("Loader Z will move back to HOME ....");
		XY_MoveTo(0, 0);
	}
	else
	{
		Z1_Home();
		Z2_Home();
		HmiMessage("Loader XY will move back to HOME ....");
		XY_MoveTo(0, 0);
	}

	m_lBackupPosition = 0;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::MoveWTLoadOffsetXY(IPC_CServiceMessage& svMsg)
{
	BOOL bMove = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bMove);

	if (labs(m_lWTLoadOffsetX) <= 2000)
	{
		if (bMove)
		{
			X_Move(m_lWTLoadOffsetX);
			HmiMessage("MoveWTLoadOffsetXY = TRUE");
		}
		else
		{
			X_Move(-1 * m_lWTLoadOffsetX);
			HmiMessage("MoveWTLoadOffsetXY = FALSE");
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::TeachLoadUnloadLevel(IPC_CServiceMessage& svMsg)
{
	CString szTemp;
	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if (!CheckGripperFrameExistForXYMove(TRUE) || !CheckGripperFrameExistForXYMove(FALSE))
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	m_lBackupPosition = lPosition;

	switch (lPosition)
	{
	case 11:
		m_lBackupPos_Z = m_lStandByPos_Z1;
		szTemp.Format("Loader Z1 will move to STANDBY (%ld) at# %d", m_lStandByPos_Z1, lPosition);
		HmiMessage(szTemp);
		XY_MoveTo(m_lStandByPos_X, m_lStandByPos_Y);
		Z1_MoveTo(m_lStandByPos_Z1);
		break;
	case 12:
		m_lBackupPos_Z = m_lStandByPos_Z2;
		szTemp.Format("Loader Z2 will move to STANDBY (%ld) at# %d", m_lStandByPos_Z2, lPosition);
		HmiMessage(szTemp);
		XY_MoveTo(m_lStandByPos_X, m_lStandByPos_Y);
		Z2_MoveTo(m_lStandByPos_Z2);
		break;
	
	case 13:
		m_lBackupPos_Z = m_lWafLoadPos_Z1;
		szTemp.Format("Loader Z1 will move to WAF-LOAD (%ld) at# %d", m_lWafLoadPos_Z1, lPosition);
		HmiMessage(szTemp);
		XY_MoveTo(m_lWTLoadUnloadPos1_X, m_lWTLoadUnloadPos1_Y);
		Z1_MoveTo(m_lWafLoadPos_Z1);
		break;
	case 14:
		m_lBackupPos_Z = m_lWafLoadPos_Z2;
		szTemp.Format("Loader Z2 will move to WAF-LOAD (%ld) at# %d", m_lWafLoadPos_Z2, lPosition);
		HmiMessage(szTemp);
		XY_MoveTo(m_lWTLoadUnloadPos2_X, m_lWTLoadUnloadPos2_Y);
		Z2_MoveTo(m_lWafLoadPos_Z2);
		break;

	case 15:
		m_lBackupPos_Z = m_lBinLoadPos_Z1;
		szTemp.Format("Loader Z1 will move to BIN-LOAD (%ld) at# %d", m_lBinLoadPos_Z1, lPosition);
		HmiMessage(szTemp);
		XY_MoveTo(m_lBTLoadUnloadPos1_X, m_lBTLoadUnloadPos1_Y);
		Z1_MoveTo(m_lBinLoadPos_Z1);
		break;
	case 16:
		m_lBackupPos_Z = m_lBinLoadPos_Z2;
		szTemp.Format("Loader Z2 will move to BIN-LOAD (%ld) at# %d", m_lBinLoadPos_Z2, lPosition);
		HmiMessage(szTemp);
		XY_MoveTo(m_lBTLoadUnloadPos2_X, m_lBTLoadUnloadPos2_Y);
		Z2_MoveTo(m_lBinLoadPos_Z2);
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::ConfirmLoadUnloadLevel(IPC_CServiceMessage& svMsg)
{
	m_lBackupPosition = 0;
	m_lBackupPos_Z = 0;

	SaveData();

	HmiMessage("Loader Z1 Z2 will move back to HOME ....");
	Z1_Home();
	Z2_Home();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::CancelLoadUnloadLevel(IPC_CServiceMessage& svMsg)
{
	switch (m_lBackupPosition)
	{
	case 11:
		m_lStandByPos_Z1 = m_lBackupPos_Z;
		break;
	case 12:
		m_lStandByPos_Z2 = m_lBackupPos_Z;
		break;
	
	case 13:
		m_lWafLoadPos_Z1 = m_lBackupPos_Z;
		break;
	case 14:
		m_lWafLoadPos_Z2 = m_lBackupPos_Z;
		break;

	case 15:
		m_lBinLoadPos_Z1 = m_lBackupPos_Z;
		break;
	case 16:
		m_lBinLoadPos_Z2 = m_lBackupPos_Z;
		break;
	}

	m_lBackupPosition = 0;

	HmiMessage("Loader Z1 Z2 will move back to HOME ....");
	Z1_Home();
	Z2_Home();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// Mgzn Setup menu
//////////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::SelectMagazine(IPC_CServiceMessage& svMsg)
{
	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if ((lPosition < 0) || (lPosition >= MS_BL_MGZN_NUM))
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (!IsAllMotorsEnable())
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_lSelMagazineID	= lPosition;
	m_lSelSlotID		= 1;

	CString szTemp;
	szTemp.Format("NL: SelectMagazine - MGZN=%ld", lPosition);
	//HmiMessage(szTemp);

    m_lNLGeneral_1  = m_stNVCMgznRT[lPosition].m_lTopLevel;		//TopPosnX
    m_lNLGeneral_2  = m_stNVCMgznRT[lPosition].m_lMidPosnY;
    m_lNLGeneral_3  = m_stNVCMgznRT[lPosition].m_lSlotPitch;
    m_lNLGeneral_4  = m_stNVCMgznRT[lPosition].m_lNoOfSlots;
	m_lNLGeneral_5	= m_stNVCMgznRT[lPosition].m_lMidPosnZ1;		//andrewng //2020-0908
	m_lNLGeneral_6	= m_stNVCMgznRT[lPosition].m_lMidPosnZ2;		//andrewng //2020-0908
    m_lNLGeneral_7  = m_stNVCMgznRT[lPosition].m_lTopLevel2;		//TopPosnX for Gripper2

	//Show Slot Pitch in mm
	m_dSlotPitch	= (DOUBLE)(m_lNLGeneral_3) * m_dXRes;

	if (m_fHardware && !m_bDisableNL)
	{
		/*if (!Z1_IsPowerOn() || !Z2_IsPowerOn())
		{
			if (!Z1_IsPowerOn())
			{
				Z1_Home();
			}
			if (!Z2_IsPowerOn())
			{
				Z2_Home();
			}
		}

		Z1_MoveTo(m_lStandByPos_Z1, SFM_NOWAIT);
		Z2_MoveTo(m_lStandByPos_Z2);
		Z1_Sync();
		*/

		if (!CheckGripperFrameExistForXYMove(TRUE) || !CheckGripperFrameExistForXYMove(FALSE))
		{
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return TRUE;
		}

		BOOL bUseGripper1 = TRUE;
		if (lPosition == NL_MGZ8_BTM_2)	//Wafer Mgzn
		{
			bUseGripper1 = m_bWTUseGripper1ForSetup;
		}
		else
		{
			bUseGripper1 = m_bBTUseGripper1ForSetup;
		}

		LONG lX = m_lNLGeneral_1;
		LONG lY = m_lNLGeneral_2;

		if (!bUseGripper1)
		{
			lX = m_lNLGeneral_7;
		}

		if (IsAllMotorsEnable())
		{
			Z1_Home();
			Z2_Home();

			if (IsWithinXYLimit(lX, lY))
			{
				CString szLog;
				szLog.Format("XY MoveTo ENC X = %ld, Y = %ld ...",  lX, lY);
				HmiMessage(szLog);
				XY_MoveTo(lX, lY);
			}
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CNVCLoader::ChangeMagazineIndex(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);


	/*if ( (lPosition < 0) || (lPosition >= MS_BL_MGZN_NUM) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}*/

	//m_lSelMagazineID = lPosition;

	CString szTemp;
	szTemp.Format("NL: ChangeMagazineIndex - Mgzn=%ld, POS=%ld", m_lSelMagazineID, lPosition);
	//HmiMessage(szTemp);

	switch (lPosition)
	{
	case 0:			// magazine top posn X
		X_MoveTo(m_lNLGeneral_1, SFM_WAIT);
		m_lNLGeneral_TmpA = m_lNLGeneral_1;
		break;

	case 1:			// magazine center posn Y
		Y_MoveTo(m_lNLGeneral_2, SFM_WAIT);
		m_lNLGeneral_TmpA = m_lNLGeneral_2;
		break;

	case 2:			// slot pitch
		m_lNLGeneral_TmpA = m_lNLGeneral_3;
		break;

	case 3:			// total slots
		m_lNLGeneral_TmpA = m_lNLGeneral_4;
		break;

	case 4:			// Mgzn Level Z1
		Z1_MoveTo(m_lNLGeneral_5, SFM_WAIT);
		m_lNLGeneral_TmpA = m_lNLGeneral_5;
		break;
	case 5:			// Mgzn Level Z2
		Z2_MoveTo(m_lNLGeneral_6, SFM_WAIT);
		m_lNLGeneral_TmpA = m_lNLGeneral_6;
		break;

	case 6:			// magazine top posn X2
		X_MoveTo(m_lNLGeneral_7, SFM_WAIT);
		m_lNLGeneral_TmpA = m_lNLGeneral_7;
		break;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::KeyInValue(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	typedef struct 
	{
		LONG lPosition;
		LONG lStep;	
	} BLSETUP;
	BLSETUP stInfo;
	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	/*if ( (stInfo.lPosition < 0) || (stInfo.lPosition >= MS_BL_MGZN_NUM) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}*/

	CString szTemp;
	szTemp.Format("NL: KeyInValue - MGZN=%ld, POS=%ld, VALUE = %ld", m_lSelMagazineID, stInfo.lPosition, stInfo.lStep);
	//HmiMessage(szTemp);

	switch (stInfo.lPosition)
	{
	case 0:		//Top posn X
		m_lNLGeneral_1 = stInfo.lStep;
		if (IsWithinXLimit(m_lNLGeneral_1))
		{
			X_MoveTo(m_lNLGeneral_1, SFM_WAIT);
		}
		break;

	case 1:		//y position
		m_lNLGeneral_2 = stInfo.lStep;
		if (IsWithinYLimit(m_lNLGeneral_2))
		{
			Y_MoveTo(m_lNLGeneral_2, SFM_WAIT);
		}
		break;

	case 2:		// pitch
		m_lNLGeneral_3 = stInfo.lStep;
		break;

	case 3:		// total slots
		if ( (stInfo.lStep > 0) && (stInfo.lStep < MS_BL_MAX_MGZN_SLOT) )
		{
			m_lNLGeneral_4 = stInfo.lStep;
		}
		break;

	case 4:			// Mgzn Level Z1
		m_lNLGeneral_5 = stInfo.lStep;
		if (IsWithinZ1Limit(m_lNLGeneral_5))
		{
			Z1_MoveTo(m_lNLGeneral_5, SFM_WAIT);
		}
		break;
	case 5:			// Mgzn Level Z2
		m_lNLGeneral_6 = stInfo.lStep;
		if (IsWithinZ2Limit(m_lNLGeneral_6))
		{
			Z2_MoveTo(m_lNLGeneral_6, SFM_WAIT);
		}
		break;

	case 6:		//Top posn X2
		m_lNLGeneral_7 = stInfo.lStep;
		if (IsWithinXLimit(m_lNLGeneral_7))
		{
			X_MoveTo(m_lNLGeneral_7, SFM_WAIT);
		}
		break;
	}

	m_dSlotPitch = (DOUBLE)(m_lNLGeneral_3) * m_dZRes;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::IncreaseValue(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG lPosition;
		LONG lStep;	
	} BLSETUP;

	BLSETUP stInfo;
	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	/*if ( (stInfo.lPosition < 0) || (stInfo.lPosition >= MS_BL_MGZN_NUM) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}*/

	CString szTemp;
	szTemp.Format("NL: IncreaseValue - MGZN=%ld, POS=%ld, STEP = %ld", m_lSelMagazineID, stInfo.lPosition, stInfo.lStep);
	//HmiMessage(szTemp);

	switch(stInfo.lPosition)
	{
		case 0:	
			lCurrentPos = m_lNLGeneral_1 + stInfo.lStep;
			if (IsWithinXLimit(lCurrentPos))
			{
				X_MoveTo(lCurrentPos);
				m_lNLGeneral_1 = lCurrentPos;
			}
			break;		// top posn X	

		case 1:	
			lCurrentPos = m_lNLGeneral_2 + stInfo.lStep;
			if (IsWithinYLimit(lCurrentPos))
			{
				Y_MoveTo(lCurrentPos);
				m_lNLGeneral_2 = lCurrentPos;
			}
			break;		// y position
		
		case 2:	
			lCurrentPos = m_lNLGeneral_3 + stInfo.lStep;
			m_lNLGeneral_3 = lCurrentPos;
			break;		// pitch
		
		case 3:	
			lCurrentPos = m_lNLGeneral_4 + stInfo.lStep;
			if ( (lCurrentPos > 0) && (lCurrentPos < MS_BL_MAX_MGZN_SLOT) )
			{
				m_lNLGeneral_4 = lCurrentPos;
			}
			break;		// total slots

		case 4:	
			lCurrentPos = m_lNLGeneral_5 + stInfo.lStep;
			if (IsWithinZ1Limit(lCurrentPos))
			{
				Z1_MoveTo(lCurrentPos);
				m_lNLGeneral_5 = lCurrentPos;
			}
			break;		// Z1
		case 5:	
			lCurrentPos = m_lNLGeneral_6 + stInfo.lStep;
			if (IsWithinZ2Limit(lCurrentPos))
			{
				Z2_MoveTo(lCurrentPos);
				m_lNLGeneral_6 = lCurrentPos;
			}
			break;		// Z2

		case 6:	
			lCurrentPos = m_lNLGeneral_7 + stInfo.lStep;
			if (IsWithinXLimit(lCurrentPos))
			{
				X_MoveTo(lCurrentPos);
				m_lNLGeneral_7 = lCurrentPos;
			}
			break;		// top posn X2	
	}

	m_dSlotPitch = (DOUBLE)(m_lNLGeneral_3) * m_dZRes;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::DecreaseValue(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG lPosition;
		LONG lStep;	
	} BLSETUP;

	BLSETUP stInfo;
	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	/*if ( (stInfo.lPosition < 0) || (stInfo.lPosition >= MS_BL_MGZN_NUM) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}*/

	CString szTemp;
	szTemp.Format("NL: DecreaseValue - MGZN=%ld, POS=%ld, STEP = %ld", m_lSelMagazineID, stInfo.lPosition, stInfo.lStep);
	//HmiMessage(szTemp);

	switch(stInfo.lPosition)
	{
		case 0:	
			lCurrentPos = m_lNLGeneral_1 - stInfo.lStep;
			if (IsWithinXLimit(lCurrentPos))
			{
				X_MoveTo(lCurrentPos);
				m_lNLGeneral_1 = lCurrentPos;
			}
			break;		// top posn X	

		case 1:	
			lCurrentPos = m_lNLGeneral_2 - stInfo.lStep;
			if (IsWithinYLimit(lCurrentPos))
			{
				Y_MoveTo(lCurrentPos);
				m_lNLGeneral_2 = lCurrentPos;
			}
			break;		// y position
		
		case 2:	
			lCurrentPos = m_lNLGeneral_3 - stInfo.lStep;
			m_lNLGeneral_3 = lCurrentPos;
			break;		// pitch
		
		case 3:	
			lCurrentPos = m_lNLGeneral_4 - stInfo.lStep;
			if ( (lCurrentPos > 0) && (lCurrentPos < MS_BL_MAX_MGZN_SLOT) )
			{
				m_lNLGeneral_4 = lCurrentPos;
			}
			break;		// total slots

		case 4:	
			lCurrentPos = m_lNLGeneral_5 - stInfo.lStep;
			if (IsWithinZ1Limit(lCurrentPos))
			{
				Z1_MoveTo(lCurrentPos);
				m_lNLGeneral_5 = lCurrentPos;
			}
			break;		// Z1
		case 5:	
			lCurrentPos = m_lNLGeneral_6 - stInfo.lStep;
			if (IsWithinZ2Limit(lCurrentPos))
			{
				Z2_MoveTo(lCurrentPos);
				m_lNLGeneral_6 = lCurrentPos;
			}
			break;		// Z2

		case 6:	
			lCurrentPos = m_lNLGeneral_7 - stInfo.lStep;
			if (IsWithinXLimit(lCurrentPos))
			{
				X_MoveTo(lCurrentPos);
				m_lNLGeneral_7 = lCurrentPos;
			}
			break;		// top posn X2	
	}

	m_dSlotPitch = (DOUBLE)(m_lNLGeneral_3) * m_dZRes;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::UpdateSlotPitch(IPC_CServiceMessage& svMsg)
{
	m_lNLGeneral_3 = (LONG)((m_dSlotPitch / m_dZRes) + 0.5);
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::CancelMagazineSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if ( (lPosition < 0) || (lPosition >= MS_BL_MGZN_NUM) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szTemp;
	szTemp.Format("NL: CancelMagazineSetup - MGZN=%ld (%ld)", lPosition, m_lSelMagazineID);
	//HmiMessage(szTemp);

	//Restore HMI variable
    m_lNLGeneral_1  = m_stNVCMgznRT[lPosition].m_lTopLevel;
    m_lNLGeneral_2  = m_stNVCMgznRT[lPosition].m_lMidPosnY;
    m_lNLGeneral_3  = m_stNVCMgznRT[lPosition].m_lSlotPitch;
    m_lNLGeneral_4  = m_stNVCMgznRT[lPosition].m_lNoOfSlots;
	m_lNLGeneral_5  = m_stNVCMgznRT[lPosition].m_lMidPosnZ1;		//andrewng //2020-0908
	m_lNLGeneral_6  = m_stNVCMgznRT[lPosition].m_lMidPosnZ2;		//andrewng //2020-0908
	m_lNLGeneral_7  = m_stNVCMgznRT[lPosition].m_lTopLevel2;		//andrewng //2020-0910

	//Show Slot Pitch in mm
	m_dSlotPitch	= (DOUBLE)(m_lNLGeneral_3) * m_dZRes;

	if (!Z1_IsPowerOn() || !Z2_IsPowerOn())
	{
		if (!Z1_IsPowerOn())
		{
			Z1_Home();
		}
		if (!Z2_IsPowerOn())
		{
			Z2_Home();
		}
	}

	Z1_Home();
	Z2_Home();

	Z1_MoveTo(m_lStandByPos_Z1, SFM_NOWAIT);
	Z2_MoveTo(m_lStandByPos_Z2);
	Z1_Sync();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ConfirmMagazineSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lPosition = 0;
	svMsg.GetMsg(sizeof(LONG), &lPosition);

	if ( (lPosition < 0) || (lPosition >= MS_BL_MGZN_NUM) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CString szTemp;
	szTemp.Format("NL: ConfirmMagazineSetup - MGZN=%ld (%ld)", lPosition, m_lSelMagazineID);
	//HmiMessage(szTemp);

	if (m_stNVCMgznRT[lPosition].m_lTopLevel != m_lNLGeneral_1)
	{
		m_stNVCMgznRT[lPosition].m_lTopLevel = m_lNLGeneral_1;
		//LogItems(BL_MAGAZINE_TOP_SLOT_POS);	
	}
	else if (m_stNVCMgznRT[lPosition].m_lMidPosnY  != m_lNLGeneral_2)
	{
		m_stNVCMgznRT[lPosition].m_lMidPosnY  = m_lNLGeneral_2;
		//LogItems(BL_MAGAZINE_CENTER_Y_POS);
	}
	else if (m_stNVCMgznRT[lPosition].m_lSlotPitch != m_lNLGeneral_3)
	{
		m_stNVCMgznRT[lPosition].m_lSlotPitch = m_lNLGeneral_3;
		//LogItems(BL_MAGAZINE_SLOT_PITCH);
	}
	else if (m_stNVCMgznRT[lPosition].m_lMidPosnZ1  != m_lNLGeneral_5)
	{
		m_stNVCMgznRT[lPosition].m_lMidPosnZ1  = m_lNLGeneral_5;
		//LogItems(BL_MAGAZINE_CENTER_Y_POS);
	}
	else if (m_stNVCMgznRT[lPosition].m_lMidPosnZ2  != m_lNLGeneral_6)
	{
		m_stNVCMgznRT[lPosition].m_lMidPosnZ2  = m_lNLGeneral_6;
		//LogItems(BL_MAGAZINE_CENTER_Y_POS);
	}
	if (m_stNVCMgznRT[lPosition].m_lTopLevel2 != m_lNLGeneral_7)
	{
		m_stNVCMgznRT[lPosition].m_lTopLevel2 = m_lNLGeneral_7;
		//LogItems(BL_MAGAZINE_TOP_SLOT_POS);	
	}

	//Update variable
	m_stNVCMgznRT[lPosition].m_lTopLevel   = m_lNLGeneral_1;
    m_stNVCMgznRT[lPosition].m_lMidPosnY   = m_lNLGeneral_2;		
    m_stNVCMgznRT[lPosition].m_lSlotPitch  = m_lNLGeneral_3;	
    m_stNVCMgznRT[lPosition].m_lNoOfSlots  = m_lNLGeneral_4;		
	m_stNVCMgznRT[lPosition].m_lMidPosnZ1  = m_lNLGeneral_5;		
    m_stNVCMgznRT[lPosition].m_lMidPosnZ2  = m_lNLGeneral_6;		
    m_stNVCMgznRT[lPosition].m_lTopLevel2  = m_lNLGeneral_7;		

	//SaveMgznRTData();
	SaveData();

	if (!Z1_IsPowerOn() || !Z2_IsPowerOn())
	{
		if (!Z1_IsPowerOn())
		{
			Z1_Home();
		}
		if (!Z2_IsPowerOn())
		{
			Z2_Home();
		}
	}

	Z1_Home();
	Z2_Home();

	//Z1_MoveTo(m_lStandByPos_Z1, SFM_NOWAIT);
	//Z2_MoveTo(m_lStandByPos_Z2);
	//Z1_Sync();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::MoveToSlot(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lMgzn = 0;
	ULONG ulCurrentSlot = 1;

	typedef struct 
	{
		LONG	lMgzn;
		ULONG	ulSlot;
	} BLSETUP;
	BLSETUP stInfo;

	svMsg.GetMsg(sizeof(BLSETUP), &stInfo);

	m_lSelMagazineID	= stInfo.lMgzn;
	m_lSelSlotID		= stInfo.ulSlot;

	BOOL bUseGripper1 = TRUE;
	if (stInfo.lMgzn == NL_MGZ8_BTM_2)	//Wafer Mgzn
	{
		bUseGripper1 = m_bWTUseGripper1ForSetup;
	}
	else
	{
		bUseGripper1 = m_bBTUseGripper1ForSetup;
	}

	CString szTemp;
	szTemp.Format("NL: MoveToSlot - Mgzn=%ld, Slot=%ld", m_lSelMagazineID, m_lSelSlotID);
	//HmiMessage(szTemp);

	if (m_lSelSlotID > m_stNVCMgznRT[stInfo.lMgzn].m_lNoOfSlots)
	{
		m_lSelSlotID = m_stNVCMgznRT[stInfo.lMgzn].m_lNoOfSlots;
	}
	else if (m_lSelSlotID < 1)
	{
		m_lSelSlotID = 1;
	}

	ulCurrentSlot = m_lSelSlotID - 1;

	if ( ulCurrentSlot >= (ULONG)(m_stNVCMgznRT[stInfo.lMgzn].m_lNoOfSlots - 1) )
	{
		ulCurrentSlot = m_stNVCMgznRT[stInfo.lMgzn].m_lNoOfSlots - 1;
	}

	if (m_fHardware && !m_bDisableNL)
	{
		LONG lPosX = m_stNVCMgznRT[stInfo.lMgzn].m_lTopLevel + m_stNVCMgznRT[stInfo.lMgzn].m_lSlotPitch * ulCurrentSlot;
		if (!bUseGripper1)
		{
			lPosX = m_stNVCMgznRT[stInfo.lMgzn].m_lTopLevel2 + m_stNVCMgznRT[stInfo.lMgzn].m_lSlotPitch * ulCurrentSlot;
		}

		LONG lPosY = m_stNVCMgznRT[stInfo.lMgzn].m_lMidPosnY;

		if (IsAllMotorsEnable())
		{
			/*if (!Z1_IsPowerOn() || !Z2_IsPowerOn())
			{
				if (!Z1_IsPowerOn())
				{
					Z1_Home();
				}
				if (!Z2_IsPowerOn())
				{
					Z2_Home();
				}
			}
			else
			{
				Z1_MoveTo(m_lStandByPos_Z1, SFM_NOWAIT);
				Z2_MoveTo(m_lStandByPos_Z2);
				Z1_Sync();
			}*/
			Z1_Home();
			Z2_Home();

			if (IsWithinXYLimit(lPosX, lPosY))
			{
				CString szLog;
				szLog.Format("XY MoveTo ENC X = %ld, Y = %ld ...",  lPosX, lPosY);
				HmiMessage(szLog);

				XY_MoveTo(lPosX, lPosY);
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::SelectMagazineDisplay(IPC_CServiceMessage& svMsg)
{
    LONG lMgzn;
    svMsg.GetMsg(sizeof(LONG), &lMgzn);
	m_lSelMagazineID = lMgzn;

	GenerateOMRTTableFile(lMgzn);
	
	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

LONG CNVCLoader::SetOperationMode(IPC_CServiceMessage& svMsg)
{
    int i;
    svMsg.GetMsg(sizeof(LONG), &m_lOMSP);


	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	m_lOMSP		= NL_MODE_A;
	m_szOMSP	= _T("A");

    LoadMgznOMData();	//Load from custom MSD file

    for (i = 0; i < MS_BL_MGZN_NUM; i++)
    {
        m_szNLMgznUse[i] = GetMgznUsage(m_stMgznOM[i].m_lMgznUsage);
    }

    m_ulMgznSelected	= 0;
    m_ulSlotSelected	= 1;
    m_szNLSlotUsage		= GetOMSlotUsage(m_ulMgznSelected, 0);
    m_ulSlotPhyBlock	= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[0]; 

	if (m_stMgznOM[m_ulMgznSelected].m_lMgznUsage == NL_MGZN_USAGE_UNUSE)
	{
		m_bSetSlotBlockEnable = FALSE;
	}
	else if (m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0] == NL_SLOT_USAGE_ACTIVE)
	{
		m_bSetSlotBlockEnable = TRUE;
	}
	else
	{
		m_bSetSlotBlockEnable = FALSE;
	}

    GenerateOMSPTableFile(m_ulMgznSelected);

	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}

LONG CNVCLoader::SetMgznUsage(IPC_CServiceMessage &svMsg)
{
    LONG lUsageOld = 0, lUsageNew = 0, lMgznIndex = 0;

    svMsg.GetMsg(sizeof(LONG), &lMgznIndex);
    lUsageOld = m_stMgznOM[lMgznIndex].m_lMgznUsage;

    switch (m_lOMSP)
    {
    case NL_MODE_A:
    case NL_MODE_F:	

        switch( lMgznIndex )
        {
        case NL_MGZN_TOP1:		// unuse or empty
            //lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, BL_MGZN_USAGE_FULL);
			lUsageNew = SelectMgznUsage_UNUSE_TARGET2(lUsageOld, NL_MGZN_USAGE_FULL, NL_MGZN_USAGE_EMPTY);
            ChangeOMMgznSlotUsage(lMgznIndex, lUsageNew);
			GenerateOMSPTableFile(lMgznIndex);
			break;

        case NL_MGZN_MID1:		// unuse or full
			lUsageNew = SelectMgznUsage_UNUSE_TARGET2(lUsageOld, NL_MGZN_USAGE_EMPTY, NL_MGZN_USAGE_FULL);
            ChangeOMMgznSlotUsage(lMgznIndex, lUsageNew);
			GenerateOMSPTableFile(lMgznIndex);
            break;

        case NL_MGZN_BTM1:		// unuse or active
        case NL_MGZN8_BTM1:		// unuse or active
        case NL_MGZN_TOP2:		// unuse or full
        case NL_MGZN_MID2:		// unuse or active
        case NL_MGZN_BTM2:		// unuse or active
        case NL_MGZN8_BTM2:		// unuse or active
			lUsageNew = SelectMgznUsage_UNUSE_TARGET(lUsageOld, NL_MGZN_USAGE_ACTIVE); 
			break;
        }
        break;
	}

    m_stMgznOM[lMgznIndex].m_lMgznUsage = lUsageNew;
    m_szNLMgznUse[lMgznIndex] = GetMgznUsage(m_stMgznOM[lMgznIndex].m_lMgznUsage);

	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CNVCLoader::SelectOMMgznSetup(IPC_CServiceMessage &svMsg)
{
	ULONG ulSelected = 0;
    svMsg.GetMsg(sizeof(LONG), &ulSelected);

	m_ulMgznSelected	= ulSelected;
    m_ulSlotSelected	= 1;
    m_szNLSlotUsage		= GetOMSlotUsage(m_ulMgznSelected, m_ulSlotSelected - 1);
    m_ulSlotPhyBlock	= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1];

	if (m_stMgznOM[m_ulMgznSelected].m_lMgznUsage == NL_MGZN_USAGE_UNUSE)
	{
		m_bSetSlotBlockEnable = FALSE;
	}
	else if (m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[0] == NL_SLOT_USAGE_ACTIVE)
	{
		m_bSetSlotBlockEnable = TRUE;
	}
	else
	{
		m_bSetSlotBlockEnable = FALSE;
	}

    GenerateOMSPTableFile(m_ulMgznSelected);
	
	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return TRUE;
}


LONG CNVCLoader::SelectOMSlotSetup(IPC_CServiceMessage &svMsg)
{
    svMsg.GetMsg(sizeof(LONG), &m_ulSlotSelected);

    m_szNLSlotUsage		= GetOMSlotUsage(m_ulMgznSelected, m_ulSlotSelected-1);
    m_ulSlotPhyBlock	= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1];

	if (m_stMgznOM[m_ulMgznSelected].m_lMgznUsage == NL_MGZN_USAGE_UNUSE)
	{
		m_bSetSlotBlockEnable = FALSE;
	}
    else if (m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected-1] == NL_SLOT_USAGE_ACTIVE)
	{
		m_bSetSlotBlockEnable = TRUE;
	}
	else
    {
		m_bSetSlotBlockEnable = FALSE;
	}

	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CNVCLoader::SetSlotUsage(IPC_CServiceMessage &svMsg)
{
    LONG lUsageOld = 0, lUsageNew = 0;
	CString szMsg;

    lUsageOld = m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected - 1];

    switch ( m_lOMSP )
    {
    case NL_MODE_A:
    case NL_MODE_F:	

        switch(m_ulMgznSelected)
        {
        case NL_MGZN_TOP1:		// unuse or empty
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, NL_SLOT_USAGE_FULL);
            break;

        case NL_MGZN_MID1:		// unuse or full
            lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, NL_SLOT_USAGE_EMPTY);
            break;

        case NL_MGZN_BTM1:		// unuse or active
        case NL_MGZN8_BTM1:		// unuse or active
		case NL_MGZN_TOP2:		// unuse or full
        case NL_MGZN_MID2:		// unuse or active
        case NL_MGZN_BTM2:		// unuse or active
        case NL_MGZN8_BTM2:		// unuse or active
			lUsageNew = SelectSlotUsage_UNUSE_TARGET(lUsageOld, NL_SLOT_USAGE_ACTIVE);
            break;
        }
        break;
    }

    m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected - 1] = lUsageNew;

	if ( (lUsageNew == NL_SLOT_USAGE_FULL) || (lUsageNew == NL_SLOT_USAGE_EMPTY) )
	{
		m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected - 1] = 0;	
		m_ulSlotPhyBlock = 0;
	}
    
	m_szNLSlotUsage = GetSlotUsage(lUsageNew);

	if (m_stMgznOM[m_ulMgznSelected].m_lMgznUsage == NL_MGZN_USAGE_UNUSE)
	{
		m_bSetSlotBlockEnable = FALSE;
	}
	else if ( m_stMgznOM[m_ulMgznSelected].m_lSlotUsage[m_ulSlotSelected - 1] == NL_SLOT_USAGE_ACTIVE)
	{
		m_bSetSlotBlockEnable = TRUE;
	}
	else
	{
		m_bSetSlotBlockEnable = FALSE;
	}

	GenerateOMSPTableFile(m_ulMgznSelected);
 	
	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::SetSlotBlock(IPC_CServiceMessage& svMsg)
{
    ULONG i, j;

    svMsg.GetMsg(sizeof(ULONG), &m_ulSlotPhyBlock);
    if ( CheckPhysicalBlockValid(m_ulSlotPhyBlock) != TRUE )
    {
		CString szMsg;
		szMsg.Format("\n%d", m_ulSlotPhyBlock);
		SetAlert_Msg_Red_Yellow(IDS_BL_NOTHIS_PHYBLK, szMsg);		
        m_ulSlotPhyBlock = m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected - 1];
		SetErrorMessage("NL Invalid physical block");
        return TRUE;
    }

    for (i = 0; i < MS_BL_MGZN_NUM; i++)
    {
        for (j = 0; j < MS_BL_MGZN_SLOT; j++)
        {
            if (m_stMgznOM[i].m_lSlotBlock[j] == m_ulSlotPhyBlock) // && i!=m_ulMgznSelected && j!=(m_ulSlotSelected-1) 
            {
                m_stMgznOM[i].m_lSlotBlock[j] = 0;
            }
        }
    }

    m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected - 1] = m_ulSlotPhyBlock;
    GenerateOMSPTableFile(m_ulMgznSelected);
	
	BOOL bReturn = TRUE;
    svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::LoadOperationMode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	SetOperationMode_A();

	m_lSelMagazineID = 0;

   for (INT i=0; i<MS_BL_MGZN_NUM; i++)
    {
        m_szNLMgznUse[i] = GetMgznUsage(m_stMgznOM[i].m_lMgznUsage);
    }

    m_ulMgznSelected = 0;
    m_ulSlotSelected = 1;
    m_szNLSlotUsage		= GetOMSlotUsage(m_ulMgznSelected, m_ulSlotSelected-1);
    m_ulSlotPhyBlock	= m_stMgznOM[m_ulMgznSelected].m_lSlotBlock[m_ulSlotSelected-1];
	
	m_szOMSP = _T("A");

    GenerateOMSPTableFile(m_lSelMagazineID);

	HmiMessage("Load DEFAULT Operation mode done.");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::LoadCustomOpMode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CNVCLoader::SaveOperationMode(IPC_CServiceMessage& svMsg)
{
	if (SaveMgznOMData())
	{
		CString szMsg;
		szMsg = "NOTE: New customized MODE-" + m_szOMSP + " will only take effect when \nyou reset ALL magainzes next time.";
		HmiMessage(szMsg);
	}

	SaveData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// General NVCLoader Setup Functions
//////////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::ManualLoadFilmFrame(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn	= TRUE;
    BOOL bStatus	= TRUE;
	LONG lBlock		= 0;
	CString szLog;

	svMsg.GetMsg(sizeof(LONG), &lBlock);

    if (lBlock < 1 || lBlock > MS_MAX_BIN)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
        return TRUE;
	}

	BOOL bUseGripper1 = m_bBTUseGripper1ForSetup;

	SetBinTableJoystick(FALSE);

	szLog.Format("Manual Load Bin Frame #%ld using Gripper %d ....", lBlock, bUseGripper1? 1:2);
	HmiMessage(szLog);

	bStatus = LoadBinFrameFromMgzn(lBlock, bUseGripper1);

	if (bStatus)
	{
		bStatus = LoadBinFrameToTable(FALSE, bUseGripper1);
		//MoveXYToStandby(FALSE);
	}

	SaveMgznRTData();
	SaveData();

	HmiMessage("NVC: ManualLoadFilmFrame Done");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualUnloadFilmFrame(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szLog;


	SetBinTableJoystick(FALSE);

	BOOL bUseGripper1 = m_bBTUseGripper1ForSetup;

	szLog.Format("Manual Unload Bin Frame using Gripper %d ....", bUseGripper1? 1:2);
	HmiMessage(szLog);

	BOOL bStatus = UnloadBinFrameFromTable(bUseGripper1);

	if (bStatus)
	{
		bStatus = UnloadBinFrameToMgzn(bUseGripper1);
		MoveXYToStandby(FALSE);
	}

	SaveMgznRTData();
	SaveData();

	HmiMessage("NVC: ManualUnloadFilmFrame Done");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualLoadFrameFromMgzn(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	LONG lLoadBlock		= 1;
	BOOL bUseGripper1	= FALSE;

	svMsg.GetMsg(sizeof(LONG), &lLoadBlock);

	bUseGripper1 = m_bBTUseGripper1ForSetup;

	CString szLog;
	szLog.Format("Manual LOAD Frame from MGZN (Grade #%d) to BUFFER using Gripper %d ....", 
					lLoadBlock, bUseGripper1? 1 : 2);
	HmiMessage(szLog);

	BOOL bStatus = LoadBinFrameFromMgzn(lLoadBlock, bUseGripper1);
	if (!bStatus)
	{
		CString szLog;
		szLog.Format("ManualLoadFrameFromMgzn: LoadBinFrameFromMgzn fail (%ld)", lLoadBlock);
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualLoadFrameToTable(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	BOOL bUseGripper1	= FALSE;
	LONG lCurrBlock		= 0;

	bUseGripper1 = m_bBTUseGripper1ForSetup;

	CString szLog;
	if (bUseGripper1)
	{
		lCurrBlock = m_lCurrBlock1;
		szLog.Format("Manual LOAD Frame from BUFFER (Grade #%d) to TABLE using Gripper %d ....", 
					m_lCurrBlock1, bUseGripper1? 1 : 2);
	}
	else
	{
		lCurrBlock = m_lCurrBlock2;
		szLog.Format("Manual LOAD Frame from BUFFER (Grade #%d) to TABLE using Gripper %d ....", 
					m_lCurrBlock2, bUseGripper1? 1 : 2);
	}
	HmiMessage(szLog);

	BOOL bStatus = LoadBinFrameToTable(bUseGripper1);
	if (!bStatus)
	{
		CString szLog;
		szLog.Format("ManualLoadFrameToTable: LoadBinFrameToTable fail (%ld)", lCurrBlock);
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualUnloadFrameFromTable(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	BOOL bUseGripper1	= FALSE;

	bUseGripper1 = m_bBTUseGripper1ForSetup;

	CString szLog;
	szLog.Format("Manual UNLOAD Frame from TABLE (Grade #%d) to BUFFER using Gripper %d ....", 
					m_lCurrBTBlock, bUseGripper1? 1 : 2);
	HmiMessage(szLog);

	BOOL bStatus = UnloadBinFrameFromTable(bUseGripper1, TRUE);
	if (!bStatus)
	{
		CString szLog;
		szLog.Format("ManualUnloadFrameFromTable: UnloadBinFrameFromTable fail (%ld)", m_lCurrBTBlock);
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualUnloadFrameToMgzn(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	BOOL bUseGripper1	= FALSE;

	bUseGripper1 = m_bBTUseGripper1ForSetup;

	CString szLog;
	szLog.Format("Manual UNLOAD Frame from BUFFER (Grade #%d) to MGZN using Gripper %d ....", 
					m_lCurrBlock2, bUseGripper1? 1 : 2);
	HmiMessage(szLog);

	BOOL bStatus = UnloadBinFrameToMgzn(bUseGripper1);
	if (!bStatus)
	{
		CString szLog;
		szLog.Format("ManualUnloadFrameToMgzn: UnloadBinFrameToMgzn fail (%ld)", m_lCurrBlock2);
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


//////////////////////////////////////////////////////////////////////////////
// NVC Main wafer Setup Functions
//////////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::ManualLoadWaferFrame(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szLog;

	LONG lSlotID = 1;
	svMsg.GetMsg(sizeof(LONG), &lSlotID);

	m_lCurrWafSlotID = lSlotID;

	BOOL bUseGripper1 = m_bWTUseGripper1ForSetup;

	if (bUseGripper1)
	{
		szLog.Format("ManualLoadWaferFrame: loading SLOT #%d using gripper 1 ....", m_lCurrWafSlotID);
	}
	else
	{
		szLog.Format("ManualLoadWaferFrame: loading SLOT #%d using gripper 2 ....", m_lCurrWafSlotID);
	}
	HmiMessage(szLog);

	BOOL bStatus = LoadWaferFrame(bUseGripper1, TRUE);
	if (!bStatus)
	{
		HmiMessage("LoadWaferFrame Fail!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualUnloadWaferFrame(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szLog;

	//m_lCurrWafSlotID = 1;
	BOOL bUseGripper1 = m_bWTUseGripper1ForSetup;

	if (bUseGripper1)
	{
		HmiMessage("ManualUnloadWaferFrame: Unloading using Gripper 1 ...");
	}
	else
	{
		HmiMessage("ManualUnloadWaferFrame: Unloading using Gripper 2 ...");
	}

	BOOL bStatus = UnloadWaferFrame(TRUE, TRUE);
	if (!bStatus)
	{
		HmiMessage("UnloadWaferFrame Fail!");
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualLoadWaferFromMgzn(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	BOOL bUseGripper1	= TRUE;
	LONG lSlotID		= 1;


	svMsg.GetMsg(sizeof(LONG), &lSlotID);

	m_lCurrWafSlotID = lSlotID;
	
	bUseGripper1 = m_bWTUseGripper1ForSetup;

	CString szLog;
	szLog.Format("Manual LOAD wafer from MGZN Slot #%d to BUFFER using Gripper %d ....", 
					m_lCurrWafSlotID, bUseGripper1? 1:2);
	HmiMessage(szLog);

	if (!LoadWaferFromMgzn(bUseGripper1))
	{
		szLog = "ManualLoadWaferFromMgzn: LoadWaferFromMgzn fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualLoadWaferToTable(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	BOOL bUseGripper1	= TRUE;
	LONG lSlotID		= 1;

	bUseGripper1 = m_bWTUseGripper1ForSetup;

	CString szLog;
	szLog.Format("Manual LOAD wafer from BUFFER (#%d) to TABLE using Gripper %d ....", 
		m_lCurrWafSlotID, bUseGripper1? 1:2);
	HmiMessage(szLog);

	if (!LoadWaferToWT(bUseGripper1, FALSE))
	{
		szLog = "ManualLoadWaferToTable: LoadWaferToWT fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualUnloadWaferFromTable(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	BOOL bUseGripper1	= TRUE;
	LONG lSlotID		= 1;

	bUseGripper1 = m_bWTUseGripper1ForSetup;

	CString szLog;
	szLog.Format("Manual UNLOAD wafer from TABLE (#%d) to BUFFER using Gripper %d ....", 
				m_lCurrWTBlock, bUseGripper1? 1:2);
	HmiMessage(szLog);

	if (!UnloadWaferFromWT(bUseGripper1))
	{
		szLog = "ManualUnloadWaferFromTable: UnloadWaferFromWT fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}
	else
	{
		//Close wafer expander
		INT nConvID = 0;
		BOOL bStatus = LoadUnloadWafExpander(FALSE, nConvID, TRUE);
		if (!bStatus)
		{
			szLog = "ManualUnloadWaferFromTable: Unload Wafer Expander fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::ManualUnloadWaferToMgzn(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn		= TRUE;
	BOOL bUseGripper1	= TRUE;
	LONG lSlotID		= 1;

	bUseGripper1 = m_bWTUseGripper1ForSetup;

	CString szLog;
	szLog.Format("Manual UNLOAD wafer from BUFFER (#%d) to MGZN using Gripper %d ....", 
		m_lCurrBlock1, bUseGripper1? 1:2);
	HmiMessage(szLog);

	if (!UnloadWaferToMgzn(m_lCurrWafSlotID, bUseGripper1, FALSE))
	{
		szLog = "ManualUnloadWaferToMgzn: UnloadWaferToMgzn fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


//////////////////////////////////////////////////////////////////////////////
// Other NVC Main Loader Functions
//////////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::ResetMagazineCmd(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	CString szMsg;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();


	szTitle.LoadString(HMB_BL_RESET_MAGAZINE);
	szContent.LoadString(HMB_GENERAL_AREUSURE);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CStringList szList;
	CString szLog;
	LONG lMgzn = 0, i = 0;

	szList.AddTail("All");
	for (i = 0; i < MS_BL_MGZN_NUM; i++)		//default 8-mag config for MS100
	{
		szList.AddTail(GetMagazineName(i).MakeUpper());
	}

	szList.AddTail("Output Tables only");	
	szContent.LoadString(HMB_BL_SELECT_RESET_MAGZ);
	
	LONG lSet = 0;
	lMgzn = HmiSelection(szContent, szTitle, szList, lSet);
	if (lMgzn < 0)
	{
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	szContent.LoadString(HMB_GENERAL_AREUSURE);
	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		if ( lMgzn == 0 )
		{
			// check if all bin cleared, if not pop up message for operator to select
			/*
			if (IsMagazineCanReset(0, TRUE) == FALSE)
			{
				szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
				HmiMessage_Red_Yellow(szContent, szTitle);
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}*/

			CMSLogFileUtility::Instance()->MS_LogOperation("NL: Manual Reset All Magazine");	
			//if ((m_lOMRT == NL_MODE_A) || (m_lOMRT == NL_MODE_F))
			//{
			//	SetOperationMode_A();
			//	SaveMgznOMData();
			//}
			
			m_lCurrBlock1	= 0;
			m_lCurrBlock2	= 0;
			m_szFrameBarcode1	= "";
			m_szFrameBarcode2	= "";

			//m_lCurrMgzn		= 0;
			//m_lCurrSlot		= 0;
			m_lCurrHmiMgzn1		= 0;	
			m_lCurrHmiMgzn2		= 0;	
			m_lCurrHmiSlot1		= 0;
			m_lCurrHmiSlot2		= 0;

			m_lCurrBTBlock	= 0;
			m_lCurrBTMgzn	= 0;
			m_lCurrBTSlotID	= 0;

			m_lCurrWafSlotID;
			m_lCurrWTBlock;

			for (int i = 0 ; i < MS_BL_MGZN_NUM ; i++)
			{
				if (IsMagazineCanReset(i, FALSE) == FALSE)
				{
					szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
					HmiMessage_Red_Yellow(szContent, szTitle);
					BOOL bReturn = FALSE;
					svMsg.InitMessage(sizeof(BOOL), &bReturn);
					return 1;
				}

				ResetMagazine(i);	
			}
		}
		else if (lMgzn == 9)
		{
			LONG lTempBlk		= m_lCurrBlock1;

			//Reset Buffer Table Status
			m_lCurrBlock1	= 0;
			m_lCurrBlock2	= 0;
			m_szFrameBarcode1	= "";
			m_szFrameBarcode2	= "";

			//Reset BT Status
			m_lCurrBTBlock	= 0;
			m_lCurrBTMgzn	= 0;
			m_lCurrBTSlotID	= 0;

			//Reset HMI Buffer Status
			//m_lCurrMgzn		= 0;
			//m_lCurrSlot		= 0;
			m_lCurrHmiMgzn1		= 0;	
			m_lCurrHmiMgzn2		= 0;	
			m_lCurrHmiSlot1		= 0;
			m_lCurrHmiSlot2		= 0;

			for (INT i = 0; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stNVCMgznRT[i].m_lMgznUsage != NL_MGZN_USAGE_ACTIVE)
				{
					continue;
				}

				for (INT j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if ( (lTempBlk > 0) && (m_stNVCMgznRT[i].m_lSlotBlock[j] == lTempBlk) )
					{
						m_stNVCMgznRT[i].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
					}
				}
			}

			szLog = "Manual Reset BL Output Table indexes";
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
		else if ( lMgzn > 0 )	//Other Grade Magazines
		{
			BOOL bIsGradeMgzn = FALSE;
			if (m_stNVCMgznRT[lMgzn - 1].m_lMgznUsage == NL_MGZN_USAGE_ACTIVE)
			{
				bIsGradeMgzn = TRUE;
			}

			// cannot reset bin as bin frame not clear die count
			if (IsMagazineCanReset(lMgzn - 1, FALSE) == FALSE)
			{
				szContent.LoadString(HMB_BL_RESET_MAG_BIN_NOT_CLEARED);
				HmiMessage_Red_Yellow(szContent, szTitle);
				BOOL bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}

			szLog = "Manual Reset BL magazine " + GetMagazineName(lMgzn - 1);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			if (m_lOMRT == NL_MODE_A)
			{
				SetOperationMode_A();
				SaveMgznOMData();
			}

			ResetMagazine(lMgzn - 1);	
		}		
	}

	TRY 
	{
		//SaveMgznRTData();
		SaveData();

	} CATCH (CFileException, e)
	{
	}
	END_CATCH

	HmiMessage("Reset Magazine Done");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CNVCLoader::RealignBinFrameCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::OpenCloseWafExpander(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOpen = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bOpen);

	IPC_CServiceMessage stMsg;
	INT nConvID = 0;
	BOOL bON = FALSE;
	BOOL bResult = TRUE;

	if (bOpen)
	{
		LoadUnloadWafExpander(TRUE, nConvID, TRUE);
	}
	else
	{
		LoadUnloadWafExpander(FALSE, nConvID, TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::OpenCloseBinExpander(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bOpen = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bOpen);

	IPC_CServiceMessage stMsg;
	INT nConvID = 0;
	BOOL bON = FALSE;
	BOOL bResult = TRUE;


	if (bOpen)
	{
		INT nConvID = 0;
		LoadUnloadBinExpander(TRUE, nConvID, TRUE);
	}
	else
	{
		INT nConvID = 0;
		LoadUnloadBinExpander(FALSE, nConvID, TRUE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


//////////////////////////////////////////////////////////////////////////////
// NVC IO Menu Functions
//////////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::SetBinExpanderSol(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (bOn)
	{
		SetBinExpanderOpen(TRUE);
		HmiMessage("BIn Expander OPEN is ON");
	}
	else
	{
		SetBinExpanderOpen(FALSE);
		HmiMessage("BIn Expander OPEN is OFF");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::SetWafExpanderSol(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (bOn)
	{
		SetWafExpanderOpen(TRUE);
		HmiMessage("Wafer Expander OPEN is ON");
	}
	else
	{
		SetWafExpanderOpen(FALSE);
		HmiMessage("Wafer Expander OPEN is OFF");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}


LONG CNVCLoader::OpenCloseWafGripper(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (bOn)
	{
		//SetWafGripperOpen(TRUE);
		//Sleep(100);
		//SetWafGripperOpen(FALSE);
		SetWafGripper(TRUE);
		HmiMessage("Wafer gripper is OPEN");
	}
	else
	{
		//SetWafGripperClose(TRUE);
		//Sleep(100);
		//SetWafGripperClose(FALSE);
		SetWafGripper(FALSE);
		HmiMessage("Wafer gripper is CLOSE");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

LONG CNVCLoader::OpenCloseBinGripper(IPC_CServiceMessage &svMsg)
{
	BOOL bOn = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (bOn)
	{
		//SetBinGripperOpen(TRUE);
		//Sleep(200);
		//SetBinGripperOpen(FALSE);
		SetBinGripper(TRUE);
		HmiMessage("Bin gripper is OPEN");
	}
	else
	{
		//SetBinGripperClose(TRUE);
		//Sleep(200);
		//SetBinGripperClose(FALSE);
		SetBinGripper(FALSE);	
		HmiMessage("Bin gripper is CLOSE");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
    return 1;
}

BOOL CNVCLoader::SetWafGripper(BOOL bOpen)
{
	if (bOpen)
	{
		SetWafGripperOpen(TRUE);
		Sleep(100);
		SetWafGripperOpen(FALSE);
	}
	else
	{
		SetWafGripperClose(TRUE);
		Sleep(100);
		SetWafGripperClose(FALSE);
	}

	return TRUE;
}

BOOL CNVCLoader::SetBinGripper(BOOL bOpen)
{
	if (bOpen)
	{
		SetBinGripperOpen(TRUE);
		Sleep(200);
		SetBinGripperOpen(FALSE);
	}
	else
	{
		SetBinGripperClose(TRUE);
		Sleep(200);
		SetBinGripperClose(FALSE);
	}

	return TRUE;
}

