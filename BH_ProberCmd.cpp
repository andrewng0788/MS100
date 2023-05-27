#include "stdafx.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "HmiDataManager.h"
#include "WaferStat.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "BL_Constant.h"
#ifdef	PROBER
#include "agmp_ctrl_fun_ext.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBondHead::RegisterProberVariables()
{
	//Prober	//v4.37T10
	RegVariable(_T("PBT_lEnc_PBTZ"),		&m_lEnc_PBTZ);
	RegVariable(_T("BH_bNoTesterBurnInCP"),	&m_bNoTesterBurnInCP);
	
	RegVariable(_T("PBT_bSel_PBTZ"),		&m_bSel_PBTZ);
	RegVariable(_T("PBT_bIsPowerOn_PBTZ"),	&m_bIsPowerOn_PBTZ);
	RegVariable(_T("PBT_bIsPPPClose"),		&m_bIsPPPClose);
	RegVariable(_T("PBT_lPPPOpenDelay"),	&m_lPPPOpenDelay);
	RegVariable(_T("PBT_lPPPCloseDelay"),	&m_lPPPCloseDelay);

	RegVariable(_T("PBT_ulProbeDelay"),				&m_ulProbeDelay);
	RegVariable(_T("PBT_ulProbeTestTimeout"),		&m_ulProbeTestTimeout);
	RegVariable(_T("PBT_ulProbeTestTime"),			&m_ulProbeTestTime);
	RegVariable(_T("PBT_lTestGrade"),				&m_lTestGrade);
	RegVariable(_T("PBT_szProbeTestResult"),		&m_szProbeTestResult);

	// Prober level
	RegVariable(_T("PBT_bEnableSearchFixDist"),		&m_bEnableSearchFixDist);
	RegVariable(_T("PBT_lPbContactLevelOffset"),	&m_lPbContactLevelOffset);
	RegVariable(_T("PBT_lPbContactSearchDist"),		&m_lPbContactSearchDist);
	RegVariable(_T("PBT_lPbDriveInDist"),			&m_lPbDriveInDist);
	RegVariable(_T("PBT_lStandByLevelOffset_PBTZ"),	&m_lStandByLevelOffset_PBTZ);
	RegVariable(_T("PBT_dPbSearchVelocity"),		&m_dPbSearchVelocity);
	RegVariable(_T("PBT_dPbDriveInVelocity"),		&m_dPbDriveInVelocity);
	RegVariable(_T("PBT_lPbSearchDebounce"),		&m_lPbSearchDebounce);
	
	RegVariable(_T("PBT_lFocusLevel_PBTZ"),			&m_lWaferFocusLevelZ);
	RegVariable(_T("PBT_lProbeFocusLevelZ"),		&m_lProbeFocusLevelZ);
	RegVariable(_T("PBT_lProbingLevelOffset"),		&m_lProbingLevelOffset);

	RegVariable(_T("PBT_bUseLearnProbeTableLevel"),	&m_bUseLearnProbeTableLevel);

	for ( int a = 0 ; a < 5 ; a++ )
	{
		CString szTempString;
		if ( a == 0 )
		{
			szTempString = "PBT_ulTotalProbeCount";
		}
		else
		{
			szTempString.Format("PBT_ulTotalProbeCount%d", a);
		}
		RegVariable(szTempString, &(m_ulTotalProbeCount[a]) );
	}


	RegVariable(_T("PBT_ulProbePinCount"),			&m_ulProbePinCount);
	RegVariable(_T("PBT_ulProbePinCleanCount"),		&m_ulProbePinCleanCount);
	RegVariable(_T("PBT_ulMaxProbePinCount"),		&m_ulMaxProbePinCount);
	RegVariable(_T("PBT_ulMaxProbePinCleanCount"),	&m_ulMaxProbePinCleanCount);
	RegVariable(_T("PBT_ulContinueProbeLimit"),		&m_ulContinueProbeLimit);
	RegVariable(_T("PBT_ulReProbeLimit"),			&m_ulReProbeLimit);
	RegVariable(_T("PBT_bReProbeViaVF"),			&m_bReProbeViaVF);
	RegVariable(_T("PBT_bReProbeViaIR"),			&m_bReProbeViaIR);

	RegVariable(_T("PBT_bUseMultiProbeLevel"),		&m_bUseMultiProbeLevel);
	RegVariable(_T("PBT_lPrbMultiLevelGridRows"),	&m_lPrbMultiLevelGridRows);
	RegVariable(_T("PBT_lPrbMultiLevelGridCols"),	&m_lPrbMultiLevelGridCols);
	RegVariable(_T("PBT_lPrbLearnLevelOffset"),		&m_lPrbLearnLevelOffset);

	// CP100 ACP reg var
	RegVariable(_T("PBT_bACPEnable"),				&m_bACPEnable);
	RegVariable(_T("PBT_bACPAskFillLiquid"),		&m_bACPAskFillLiquid);
	RegVariable(_T("PBT_lACPPointLifeLimit"),		&m_lACPPointLifeLimit);
	RegVariable(_T("PBT_lACPPointCycle"),			&m_lACPPointCycle);
	RegVariable(_T("PBT_lACPUsePoints"),			&m_lACPUsePoints);
	RegVariable(_T("PBT_lACPDriveInDist"),			&m_lACPDriveInDist);
	RegVariable(_T("PBT_lACPBrushDistX"),			&m_lACPBrushDistX);
	RegVariable(_T("PBT_lACPBrushDistY"),			&m_lACPBrushDistY);
	RegVariable(_T("PBT_lACPUpDelay"),				&m_lACPUpDelay);
	RegVariable(_T("PBT_lACPStandbyDelay"),			&m_lACPStandbyDelay);
	RegVariable(_T("PBT_lACPEvaporateDelay"),		&m_lACPEvaporateDelay);
	RegVariable(_T("PBT_lACPPointIndex"),			&m_lACPPointIndex);
	RegVariable(_T("PBT_lACPPointCount"),			&m_lACPPointCount);

	RegVariable(_T("PBT_ulAcuContactFailLimit"),	&m_ulProbeContactFailAcuLimit);
	RegVariable(_T("PBT_ulConContactFailLimit"),	&m_ulProbeContactFailConLimit);
	RegVariable(_T("PBT_ulProbeFailCountLimit"),	&m_ulProbeFailCountLimit);
	RegVariable(_T("PBT_ulProbeMarkCheckLimit"),	&m_lProbeMarkCheckLimit);
	RegVariable(_T("PBT_bIsCheckFirstProbePos"),	&m_bIsCheckFirstProbePos);
	RegVariable(_T("PBT_bCheckProbeHeadContactSnr"), &m_bCheckProbeHeadContactSnr);


	// sensor
	RegVariable(_T("PBT_bPbContactSensor1"),		&m_bPbContactSensor1);
	RegVariable(_T("PBT_bPbContactSensor2"),		&m_bPbContactSensor2);
	RegVariable(_T("PBT_bPbContactSensor3"),		&m_bPbContactSensor3);
	RegVariable(_T("PBT_bPbContactSensor4"),		&m_bPbContactSensor4);
	RegVariable(_T("PBT_bPbContactSensor5"),		&m_bPbContactSensor5);
	RegVariable(_T("PBT_bPbContactSensor6"),		&m_bPbContactSensor6);
	RegVariable(_T("PBT_lPbAdcValueContact1"),		&m_lPbAdcValueContact1);
	RegVariable(_T("PBT_lPbAdcValueContact2"),		&m_lPbAdcValueContact2);
	RegVariable(_T("PBT_lPbAdcValueContact3"),		&m_lPbAdcValueContact3);
	RegVariable(_T("PBT_lPbAdcValueContact4"),		&m_lPbAdcValueContact4);
	RegVariable(_T("PBT_lPbAdcValueContact5"),		&m_lPbAdcValueContact5);
	RegVariable(_T("PBT_lPbAdcValueContact6"),		&m_lPbAdcValueContact6);
	RegVariable(_T("PBT_bPbContactAdcRead"),		&m_bPbContactAdcRead);

	// Contact Sensor Check
	RegVariable(_T("PBT_lReCheckCycleNumber"),		&m_lReCheckCycleNumber);	// Recheck the diff after x count
	RegVariable(_T("PBT_lSensorMaxTorlance"),		&m_lSensorMaxTorlance);		
	RegVariable(_T("PBT_bIsEnableRegularCheck"),	&m_bIsEnableRegularCheck);

	// Contact Sensor Range
	RegVariable(_T("PBT_lContactLevelWithinRange"),	&m_lContactLevelWithinRange);
	RegVariable(_T("PBT_bIsCheckContactInRange"),	&m_bIsCheckContactInRange);

	// Real time check
	RegVariable(_T("PBT_bPBTZUseDynamicLevel"),		&m_bPBTZUseDynamicLevel);
	RegVariable(_T("PBT_bPBTZUseCommendInDynamicLevel"),&m_bPBTZUseCommendInDynamicLevel);

	// Flag of using process block
	RegVariable(_T("PBT_bMotionApplyProcessBlock"),		&m_bMotionApplyProcessBlock);
	RegVariable(_T("PBT_bProcessBlockSavingEncorder"),	&m_bProcessBlockSavingEncorder);
	RegVariable(_T("PBT_ulProcessBlockFailCount"),		&m_ulProcessBlockFailCount);
	RegVariable(_T("PBT_ulProcessBlockStopLimit"),		&m_ulProcessBlockStopLimit);
	RegVariable(_T("PBT_lProcessBlockAbortDiff"),		&m_lProcessBlockAbortDiff);
	RegVariable(_T("PBT_ulProcessBlockAbortLevel_LE"),	&m_ulProcessBlockAbortLevel_LE);
	RegVariable(_T("PBT_bProcessBlockCycleCheckContactDiff"),	&m_bProcessBlockCycleCheckContactDiff);
	RegVariable(_T("PBT_bProcessBlockCycleCheckAbortLevel"),	&m_bProcessBlockCycleCheckAbortLevel);
	RegVariable(_T("PBT_lProcessBlockSoftwarePortValue"),		&m_lProcessBlockSoftwarePortValue);
	RegVariable(_T("PBT_ulProcessBlockDetectDebounce"),			&m_ulProcessBlockDetectDebounce);

	// WHAT NUMBER OF PIN IS USING
	RegVariable(_T("PBT_lProberContactSensors"),		&m_lProberContactSensors);

	RegVariable(_T("PBT_bMultiTestersProbing"),			&m_bMultiTestersProbing);
	RegVariable(_T("PBT_bIsEnableOpenDac"),				&m_bIsEnableOpenDac);
	RegVariable(_T("PBT_lPBTSetupSearchingSpeed"),		&m_lPBTSetupSearchingSpeed);

	RegVariable(_T("PBT_lAutoOpenDacRow"),			&m_lAutoOpenDacRow);
	RegVariable(_T("PBT_lAutoOpenDacCol"),			&m_lAutoOpenDacCol);
	RegVariable(_T("PBT_lAutoDacDataIndex"),		&m_lAutoDacDataIndex);
	RegVariable(_T("PBT_lProbeOverShootLimit"),		&m_lOverShootLimit);
	RegVariable(_T("PBT_lProbeDriveInLimit"),		&m_lDriveInLimit);

	RegVariable(_T("BH_lBPGeneral_BHZ5"),	&m_lBPGeneral_BHZ5);
	RegVariable(_T("BH_lBPGeneral_BHZ6"),	&m_lBPGeneral_BHZ6);
}

VOID CBondHead::RegisterProberCommand()
{
	if (!IsProber())	//v4.53A21
	{
		return;
	}

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_TesterCorrelation"),	&CBondHead::PBT_TesterCorrelation);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_SetCleanPinAirCmd"),	&CBondHead::PBT_SetCleanPinAirCmd);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_SingleProbeTest"),		&CBondHead::PBT_SingleProbeTest);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_StartProbeTest2"),		&CBondHead::PBT_StartProbeTest2);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PB_IsZAtSafeLevel"),		&CBondHead::PB_IsZAtSafeLevel);

	// prober setup command
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeFocusZPosition"),	&CBondHead::BH_ChangeFocusZPosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeProbeFocusLevel"),	&CBondHead::BH_ChangeProbeFocusLevel);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeProbeZeroLevel"),	&CBondHead::BH_ChangeProbeZeroLevel);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ChangeProberZPosition"),	&CBondHead::BH_ChangeProberZPosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_KeyInProberZPosition"),	&CBondHead::BH_KeyInProberZPosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MovePosProberZPosition"), &CBondHead::BH_MovePosProberZPosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveNegProberZPosition"), &CBondHead::BH_MoveNegProberZPosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ConfrimProberZSetup"),	&CBondHead::BH_ConfrimProberZSetup);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CancelProberZSetup"),	&CBondHead::BH_CancelProberZSetup);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_HomeProberZ"),			&CBondHead::BH_HomeProberZ);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Comm_ProberZ"),		&CBondHead::Diag_Comm_ProberZ);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_PowerOn_ProberZ"),	&CBondHead::Diag_PowerOn_ProberZ);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Home_ProberZ"),		&CBondHead::Diag_Home_ProberZ);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Diag_Move_ProberZ"),		&CBondHead::Diag_Move_ProberZ);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_Move_ProberZ"),			&CBondHead::Move_ProberZ);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveTo_ProberZ"),		&CBondHead::MoveTo_ProberZ);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_SafeMove_ProberZ"),		&CBondHead::SafeMove_ProberZ);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveHome_ProberZ_Cmd"),		&CBondHead::MoveHome_ProberZ_Cmd);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_MoveHome_ProberZ0_Cmd"),		&CBondHead::MoveHome_ProberZ0_Cmd);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_TheProcedureOfThirdButton"),	&CBondHead::BH_TheProcedureOfThirdButton);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_AutoLearnProberZContactLevel"),	&CBondHead::AutoLearnProberZContactLevel);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_CheckProberZContactLevel"),		&CBondHead::CheckProberZContactLevel);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PB_CheckProbeSensorLevel"),			&CBondHead::PB_CheckProbeSensorLevel);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ProberZ_SearchAndGetContactLevel"),	&CBondHead::ProberZ_SearchAndGetContactLevel);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PB_SearchMultiProbeLevel"),		&CBondHead::PB_SearchMultiProbeLevel);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_ResetProbePinCount"),			&CBondHead::BH_ResetProbePinCount);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_VerifyProbeTableLevelByRegion"),	&CBondHead::VerifyProbeTableLevelByRegion);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_LearnProbeTableLevel"),		&CBondHead::LearnProbeTableLevel);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("BH_VerifyProbeTableLevel"),		&CBondHead::VerifyProbeTableLevel);

	// CP100 ACP reg cmd
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CleanProbePinTest"),		&CBondHead::CleanProbePinTest);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ACPSetTotalPoints"),		&CBondHead::ACPSetTotalPoints);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ACPResetCounters"),			&CBondHead::ACPResetCounters);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ACPReplaceCloth"),			&CBondHead::ACPReplaceCloth);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_UpdatePinEncorder"),	&CBondHead::PBT_UpdatePinEncorder);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_ProbeEncDiag"),			&CBondHead::PBT_ProbeEncDiag);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBTZ_CalculateTheAbortLevel"),	&CBondHead::PBTZ_CalculateTheAbortLevel);

	// "GO" Button
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_SafetyCheckingBeforeGo"),	&CBondHead::PBT_SafetyCheckingBeforeGo);

	// CP100 Die Thickness
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PB_PBTZMoveToLevel"),		&CBondHead::PB_PBTZMoveToLevel);

	
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_ManualResetPin"),	&CBondHead::PBT_ManualResetPin);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PBT_ClosePPPCmd"),		&CBondHead::PBT_ClosePPPCmd);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoFineTuneOpenDac"),	&CBondHead::AutoFineTuneOpenDac);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CalibrateOpenDac"),		&CBondHead::CalibrateOpenDac);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetAutoOpenDacData"),	&CBondHead::ResetAutoOpenDacData);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SpinDacDataDisplay"),	&CBondHead::SpinDacDataDisplay);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmAutoOpenDac"),	&CBondHead::ConfirmAutoOpenDac);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExitAutoOpenDacPage"),	&CBondHead::ExitAutoOpenDacPage);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoopTestAutoOpenDac"),	&CBondHead::LoopTestAutoOpenDac);
}

LONG CBondHead::PBT_TesterCorrelation(IPC_CServiceMessage &svMsg)
{
	CString szMsg, szTitle;
	BOOL bReturn = TRUE;

	if (!IsProber())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bIsBlockProbe = FALSE;
	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest("WaferTableStn", "CheckIsBlockProbe", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bIsBlockProbe);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	m_ucProbeTestPinNo = 0;	//	tester correlation
	if( bIsBlockProbe )
	{
		CStringList szPinList;
		szPinList.AddTail("Pin 1~2");
		szPinList.AddTail("Pin 3~4");
		szPinList.AddTail("Pin 5~6");
		LONG lSelect = HmiSelection("Please select pin no", "CP 100", szPinList, 0);
		if( lSelect!=-1 )
			m_ucProbeTestPinNo = lSelect+1;
		else
			m_ucProbeTestPinNo = 1;
	}	//	block probe
	else
	{
		BOOL bResult = TRUE;
		IPC_CServiceMessage stMsg;
		INT nConvID = m_comClient.SendRequest("WaferPrStn", "UserSearchDie_Rotate", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!bResult)
		{
			AfxMessageBox("No die is found at current position", MB_SYSTEMMODAL);
		}
	}

	//2. Move table to PROBE offset XY position
	if (MoveToProberOffsetXY(TRUE) == FALSE)
	{
		//v4.39T10	//Klocwork
		//AfxMessageBox("Fails to move wafer table to Prober Offset XY position.", MB_SYSTEMMODAL);
	}

	PBTZ_UpToContactLevel(FALSE, SFM_WAIT, m_ucProbeTestPinNo);	//	correlation

	szTitle.LoadString(HMB_BH_TESTER);
	szMsg.LoadString(HMB_BH_TESTER_CORRELATE);
	HmiMessage(szMsg, szTitle);

	PBTZ_MoveToFocusLevel();

	//4. Move table back to PR center position
	if (MoveToProberOffsetXY(FALSE) == FALSE)
	{
		//v4.30T10	//Klocwork
		//AfxMessageBox("Fails to move wafer table to center position.", MB_SYSTEMMODAL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBT_SingleProbeTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!IsProber())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bIsBlockProbe = FALSE;
	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest("WaferTableStn", "CheckIsBlockProbe", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bIsBlockProbe);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	m_ucProbeTestPinNo = 0;	//	single test
	if( bIsBlockProbe )
	{
		CStringList szPinList;
		szPinList.AddTail("Pin 1~2");
		szPinList.AddTail("Pin 3~4");
		szPinList.AddTail("Pin 5~6");
		LONG lSelect = HmiSelection("Please select pin no", "CP 100", szPinList, 0);
		if( lSelect!=-1 )
			m_ucProbeTestPinNo = lSelect+1;
		else
			m_ucProbeTestPinNo = 1;
	}	//	block probe
	else
	{
		BOOL bResult = TRUE;
		IPC_CServiceMessage stMsg;
		INT nConvID = m_comClient.SendRequest("WaferPrStn", "UserSearchDie", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!bResult)
		{
			AfxMessageBox("No die is found at current position", MB_SYSTEMMODAL);
		}
	}

	//2. Move table to PROBE offset XY position
	if (MoveToProberOffsetXY(TRUE) == FALSE)
	{
		//v4.30T10	//Klocwork
		//AfxMessageBox("Fails to move wafer table to Prober Offset XY position.", MB_SYSTEMMODAL);
	}

	StartProbeTest();

	HmiMessage("Testing done");

	//4. Move table back to PR center position
	if (MoveToProberOffsetXY(FALSE) == FALSE)
	{
		//v4.30T10	//Klocwork
		//AfxMessageBox("Fails to move wafer table to center position.", MB_SYSTEMMODAL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondHead::PBT_StartProbeTest2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!IsProber())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bStart = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bStart);

	m_ucProbeTestPinNo	= 0;	//	loop test
	if (bStart)
	{
		BOOL bIsBlockProbe = FALSE;
		IPC_CServiceMessage stMsg;
		INT nConvID = m_comClient.SendRequest("WaferTableStn", "CheckIsBlockProbe", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bIsBlockProbe);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if( bIsBlockProbe )
		{
			CStringList szPinList;
			szPinList.AddTail("Pin 1~2");
			szPinList.AddTail("Pin 3~4");
			szPinList.AddTail("Pin 5~6");
			LONG lSelect = HmiSelection("Please select pin no", "CP 100", szPinList, 0);
			if( lSelect!=-1 )
				m_ucProbeTestPinNo = lSelect+1;
			else
				m_ucProbeTestPinNo = 1;
		}	//	block probe
		else
		{
			BOOL bResult = TRUE;
			IPC_CServiceMessage stMsg;
			INT nConvID = m_comClient.SendRequest("WaferPrStn", "UserSearchDie", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		m_bStartProbeTest = TRUE;
		MoveToProberOffsetXY(TRUE);
	}
	else
	{
		m_bStartProbeTest = FALSE;
		PBTZ_MoveToFocusLevel();
		MoveToProberOffsetXY(FALSE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CBondHead::StartProbeTest()
{
	if (!IsProber())
	{
		return FALSE;
	}

	//	block probe
	PBTZ_UpToContactLevel(FALSE, SFM_WAIT, m_ucProbeTestPinNo);	//	probe test

	if (m_ulProbeDelay > 0)
	{
		Sleep(m_ulProbeDelay);
	}

	if( m_ucProbeTestPinNo==0 )
	{
		OpStartSOT(0);
	}
	else
	{
		if( m_bMultiTestersProbing )	//	block probe with multi tester/sot, send sot, button test.
		{
			CByteArray ucaDutList;
			ucaDutList.RemoveAll();
			ucaDutList.Add(m_ucProbeTestPinNo);
			OpStartMultiSOT(ucaDutList);	//	block probing send multi tester probing sot with dut
		}
		else
		{
			OpStartSOT(m_ucProbeTestPinNo-1);
		}
	}
	OpWaitTestDone();
	OpWaitEOT();
	PBTZ_MoveToFocusLevel();

	m_ulProbeTestTime = (ULONG)m_dProbeTestDuration;

	return TRUE;
}


LONG CBondHead::BH_ChangeProberZPosition(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	
	typedef struct 
	{
		LONG	lPosition;
		BOOL	bFromZero;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	//If learn from Home Position
	if (stInfo.bFromZero == TRUE)
	{
		m_lBPGeneral_TmpE = 0;

		switch (stInfo.lPosition)
		{
		case 1:		
			m_lBPGeneral_H = 0;	
			break;
		default:	
			m_lBPGeneral_G = 0;	
			break;
		}
	}

	switch (stInfo.lPosition)
	{
	case 1:
		if (stInfo.bFromZero == FALSE)
		{
			PBTZ_MoveTo(m_lStandByLevel_PBTZ);
			m_lBPGeneral_TmpE = m_lStandByLevel_PBTZ;
		}
		break;
	default:
		if (stInfo.bFromZero == FALSE)
		{
			PBTZ_MoveTo(m_lContactLevel_PBTZ);
			m_lBPGeneral_TmpE = m_lContactLevel_PBTZ;
		}
		break;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_MovePosProberZPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}


	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_H + stInfo.lStep;	
			break;
		default:	
			lCurrentPos = m_lBPGeneral_G + stInfo.lStep;	
			break;
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_PROBERZ, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_PROBERZ, MS896A_CFG_CH_MAX_DISTANCE);								
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		PBTZ_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_H = lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_G = lCurrentPos;	
				break;
		}
	}

	return TRUE;
}


LONG CBondHead::BH_MoveNegProberZPosition(IPC_CServiceMessage &svMsg)
{
	LONG lMaxPos = 0;
	LONG lMinPos = 0;
	LONG lCurrentPos = 0;

	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	switch (stInfo.lPosition)
	{
		case 1:		
			lCurrentPos = m_lBPGeneral_H - stInfo.lStep;	
			break;	//Bond
		default:	
			lCurrentPos = m_lBPGeneral_G - stInfo.lStep;	
			break;	//Pick
	}

	//Check limit range
	lMinPos = GetChannelInformation(MS896A_CFG_CH_PROBERZ, MS896A_CFG_CH_MIN_DISTANCE);								
	lMaxPos = GetChannelInformation(MS896A_CFG_CH_PROBERZ, MS896A_CFG_CH_MAX_DISTANCE);								
	if ((lCurrentPos >= lMinPos) && (lCurrentPos <= lMaxPos))
	{
		PBTZ_MoveTo(lCurrentPos);

		switch (stInfo.lPosition)
		{
			case 1:		
				m_lBPGeneral_H = lCurrentPos;	
				break;
			default:	
				m_lBPGeneral_G = lCurrentPos;	
				break;
		}
	}

	return TRUE;
}

LONG CBondHead::BH_KeyInProberZPosition(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG	lPosition;
		LONG	lStep;	
	} BHSETUP;
	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	switch (stInfo.lPosition)
	{
		case 1:
			m_lBPGeneral_H = stInfo.lStep;
			PBTZ_MoveTo(m_lBPGeneral_H);
			break;

		default:
			m_lBPGeneral_G = stInfo.lStep;
			PBTZ_MoveTo(m_lBPGeneral_G);
			break;
	}

	return TRUE;
}

LONG CBondHead::BH_ConfrimProberZSetup(IPC_CServiceMessage &svMsg)
{
	//v4.08
	if (m_bDisableBH)
	{
		//bResult = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bResult);
		return TRUE;
	}

	StartLoadingAlert();	//v3.86

	// not to use home to prevent free falling from a high position
	PBTZ_MoveToFocusLevel();
	CloseLoadingAlert();	//v3.86
	
	//Update variable
	m_lContactLevel_PBTZ = m_lBPGeneral_G;
	m_lStandByLevel_PBTZ = m_lBPGeneral_H;
	
	SaveBhData();

	SetStatusMessage("ProberZ position is updated");
	return TRUE;
}


LONG CBondHead::BH_CancelProberZSetup(IPC_CServiceMessage &svMsg)
{
	//v4.08
	if (m_bDisableBH)
	{
		//bReturn = TRUE;
		//svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	StartLoadingAlert();

	// not to use home to prevent free falling from a high position
	PBTZ_MoveToFocusLevel();

	//Restore HMI variable
	m_lBPGeneral_G = m_lContactLevel_PBTZ;
	m_lBPGeneral_H = m_lStandByLevel_PBTZ;
	
	CloseLoadingAlert();
	return TRUE;
}

// Prober Diag

LONG CBondHead::BH_HomeProberZ(IPC_CServiceMessage &svMsg)
{
	PBTZ_Home();
	return TRUE;
}

LONG CBondHead::Diag_PowerOn_ProberZ(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	PBTZ_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Comm_ProberZ(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	PBTZ_Comm();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Home_ProberZ(IPC_CServiceMessage &svMsg)
{
	StartLoadingAlert();
	PBTZ_Home();
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Diag_Move_ProberZ(IPC_CServiceMessage &svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	StartLoadingAlert();
	PBTZ_Move(lPos);
	CloseLoadingAlert();
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::SafeMove_ProberZ(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		BOOL		bDirection;
		LONG		lStepSize;
		
	} REF_TYPE;
	
	REF_TYPE	stInfo;

	svMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
	LONG lStep = abs(stInfo.lStepSize);

	if (stInfo.bDirection)
	{
		PBTZ_Move(lStep);
	}
	else
	{
		PBTZ_Move(-1 * lStep);
	}

	if (IsContactSensorsOn() == TRUE)
	{
		CString szContent;
		szContent.LoadString(HMB_BH_CONTACT_SNRS_ON);
		HmiMessage(szContent);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::Move_ProberZ(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		BOOL		bDirection;
		LONG		lStepSize;
		
	} REF_TYPE;
	
	REF_TYPE	stInfo;

	svMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
	LONG lStep = abs(stInfo.lStepSize);
	
	if (stInfo.bDirection)
	{
		PBTZ_Move(lStep);
	}
	else
	{
		PBTZ_Move(-1 * lStep);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::MoveTo_ProberZ(IPC_CServiceMessage &svMsg)
{
	LONG lProberZLevel;
	svMsg.GetMsg(sizeof(LONG), &lProberZLevel);

	PBTZ_MoveTo(lProberZLevel);
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::MoveHome_ProberZ_Cmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	PBTZ_MoveToFocusLevel();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::MoveHome_ProberZ0_Cmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	PBTZ_MoveTo0Level();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::AutoLearnProberZContactLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lSensorNo = 0;

	svMsg.GetMsg(sizeof(LONG), &lSensorNo);

	PBTZ_SearchContactLevel((INT)lSensorNo);
	PBTZ_Sync();
	
	Sleep(100);

	GetEncoderValue();
	m_lBPGeneral_G = m_lEnc_PBTZ;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BH_ResetProbePinCount(IPC_CServiceMessage &svMsg)
{
	CString szTitle, szContent;
	BOOL bReturn = FALSE;

	szTitle.LoadString(HMB_BH_CHANGE_PROBE_PIN_CNT);
	szContent.LoadString(HMB_BH_RESET_PROBE_PIN_CONF);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		ResetProbePinCount();
		szContent.LoadString(HMB_GENERAL_RESET);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}

	szTitle.LoadString(HMB_BH_CLEAN_PROBE_PIN_CNT);
	szContent.LoadString(HMB_BH_RESET_CLEAN_PROBE_PIN_CONF);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		bReturn = TRUE;
		ResetCleanProbePinCount();
		szContent.LoadString(HMB_GENERAL_RESET);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBT_ClosePPPCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bClose = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bClose);	

	ClosePPP(bClose);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBTZ_CalculateTheAbortLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	m_ulProcessBlockAbortLevel_LE = m_lContactLevel_PBTZ + m_lProcessBlockAbortDiff;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBT_ManualResetPin(IPC_CServiceMessage &svMsg)
{
	UINT ucNum = 0;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(UINT), &ucNum);	
	
	ResetTotalProbeCount(ucNum);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::VerifyProbeTableLevelByRegion(IPC_CServiceMessage &svMsg)
{
	CString szMsg;
	BOOL bResult = TRUE;

	LONG lCalLevel_Z = GetProbingLevel(FALSE);
	
	PBTZ_SearchContactLevel();

	Sleep(100);

	if (IsContactSensorsOn() == FALSE)
	{
		bResult = TRUE;
		PBTZ_MoveToFocusLevel();
		szMsg.LoadString(HMB_BH_FAIL_TO_SEARCH_CONTACT_SENSOR);
		HmiMessage_Red_Yellow(szMsg);
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	GetEncoderValue();
	Sleep(1000);
	LONG lSrchLevel_Z = m_lEnc_PBTZ;

	PBTZ_MoveToFocusLevel();

	szMsg.Format("Calculate Z level:%d Search Z Level:%d Difference:%d", lCalLevel_Z, lSrchLevel_Z, lCalLevel_Z - lSrchLevel_Z);
	HmiMessage(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CBondHead::LearnProbeTableLevel(IPC_CServiceMessage &svMsg)
{
	CString szMsg, szTitle;
	LONG lX, lY, lT;
	BOOL bResult = TRUE, bReturn = TRUE;
	LONG lReturn;

	szTitle.LoadString(HMB_BH_LEARN_PROBE_TABLE_LEVEL);
	szMsg.LoadString(HMB_BH_MOVE_TO_POINT);
	
	lReturn = HmiMessageEx(szMsg + " 1", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn != glHMI_YES)
	{
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	GetES101WTEncoder(&lX, &lY, &lT, FALSE);
	PBTZ_SearchContactLevel();
	if (IsContactSensorsOn() == FALSE)
	{
		szMsg.LoadString(HMB_BH_FAIL_TO_SEARCH_CONTACT_SENSOR);
		HmiMessage_Red_Yellow(szMsg, szTitle);
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	GetEncoderValue();
	Sleep(1000);
	m_lProbeTablePosX[0] = lX;
	m_lProbeTablePosY[0] = lY;
	m_lProbeTablePosZ[0] = m_lEnc_PBTZ;
	PBTZ_MoveToFocusLevel();
	Sleep(1000);

	lReturn = HmiMessageEx(szMsg + " 2", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn != glHMI_YES)
	{
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	GetES101WTEncoder(&lX, &lY, &lT, FALSE);
	PBTZ_SearchContactLevel();
	if (IsContactSensorsOn() == FALSE)
	{
		szMsg.LoadString(HMB_BH_FAIL_TO_SEARCH_CONTACT_SENSOR);
		HmiMessage_Red_Yellow(szMsg, szTitle);
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	GetEncoderValue();
	Sleep(1000);
	m_lProbeTablePosX[1] = lX;
	m_lProbeTablePosY[1] = lY;
	m_lProbeTablePosZ[1] = m_lEnc_PBTZ;
	PBTZ_MoveToFocusLevel();
	Sleep(1000);

	lReturn = HmiMessageEx(szMsg + " 3", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn != glHMI_YES)
	{
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	GetES101WTEncoder(&lX, &lY, &lT, FALSE);
	PBTZ_SearchContactLevel();
	if (IsContactSensorsOn() == FALSE)
	{
		szMsg.LoadString(HMB_BH_FAIL_TO_SEARCH_CONTACT_SENSOR);
		HmiMessage_Red_Yellow(szMsg, szTitle);
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	GetEncoderValue();
	Sleep(1000);
	m_lProbeTablePosX[2] = lX;
	m_lProbeTablePosY[2] = lY;
	m_lProbeTablePosZ[2] = m_lEnc_PBTZ;
	PBTZ_MoveToFocusLevel();

	lReturn = HmiMessageEx(szMsg + " 4", szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn != glHMI_YES)
	{
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	GetES101WTEncoder(&lX, &lY, &lT, FALSE);
	PBTZ_SearchContactLevel();
	if (IsContactSensorsOn() == FALSE)
	{
		szMsg.LoadString(HMB_BH_FAIL_TO_SEARCH_CONTACT_SENSOR);
		HmiMessage_Red_Yellow(szMsg, szTitle);
		PBTZ_MoveToFocusLevel();
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	GetEncoderValue();
	Sleep(1000);
	m_lProbeTablePosX[3] = lX;
	m_lProbeTablePosY[3] = lY;
	m_lProbeTablePosZ[3] = m_lEnc_PBTZ;
	PBTZ_MoveToFocusLevel();
	Sleep(1000);

	SaveBhData();

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CBondHead::VerifyProbeTableLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CBondHead::ProberZ_SearchAndGetContactLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	typedef struct
	{
		BOOL bResult;
		LONG lEnc;
	} CONTACT_LEVEL;
	
	CONTACT_LEVEL stLevel;

	PBTZ_SearchContactLevel();
	Sleep(100);
	
	if (IsContactSensorsOn() == FALSE)
	{
		szMsg.LoadString(HMB_BH_FAIL_TO_SEARCH_CONTACT_SENSOR);
		HmiMessage_Red_Yellow(szMsg);
		PBTZ_MoveToFocusLevel();
		stLevel.bResult = FALSE;
		stLevel.lEnc = 0;
		svMsg.InitMessage(sizeof(CONTACT_LEVEL), &stLevel);
		return 1;
	}
	
	GetEncoderValue();
	stLevel.lEnc = m_lEnc_PBTZ;
	stLevel.bResult = TRUE;

	// back to standby level
	PBTZ_MoveTo(m_lStandByLevel_PBTZ);

	svMsg.InitMessage(sizeof(CONTACT_LEVEL), &stLevel);
	return 1;
}

LONG CBondHead::BH_ChangeFocusZPosition(IPC_CServiceMessage &svMsg)
{
	if (m_bDisableBH==FALSE)
	{
		PBTZ_MoveToFocusLevel();
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_ChangeProbeFocusLevel(IPC_CServiceMessage &svMsg)
{
	if (m_bDisableBH==FALSE)
	{
		PBTZ_MoveTo(m_lProbeFocusLevelZ);
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::BH_ChangeProbeZeroLevel(IPC_CServiceMessage &svMsg)
{
	if (m_bDisableBH==FALSE)
	{
		PBTZ_MoveTo(0);
	}

	BOOL bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CBondHead::ACPResetCounters(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	ResetACPCounters();
	SaveBhData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CBondHead::ACPReplaceCloth(IPC_CServiceMessage &svMsg)
{
	PBTZ_MoveTo0Level();	//	PBTZ_MoveToFocusLevel();	//	CPP to home, down probe z to focus level.
	if( m_lACPStandbyDelay>0 )
		Sleep(m_lACPStandbyDelay);
	OpMoveToUnloadToCleanProbePin(2);	// replace clean material, i.e. unload position.
	ResetACPCounters();
	SaveBhData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CBondHead::CleanProbePinTest(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if( m_bACPEnable )
	{
		bReturn = OpAutoCleanProbePin();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CBondHead::ACPSetTotalPoints(IPC_CServiceMessage &svMsg)
{
	typedef struct 
	{
		LONG lRow;
		LONG lCol;
	} TotalPoints;

	TotalPoints stTotal;

	svMsg.GetMsg(sizeof(TotalPoints),&stTotal);

	m_lACPTotalPoints = stTotal.lRow * stTotal.lCol;
	SaveBhData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CBondHead::PB_SearchMultiProbeLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CString szLogFileName = gszUSER_DIRECTORY + WT_LEVEL_LOG;
	DeleteFile(szLogFileName);
	if( IsPrescanEnded() && m_bUseMultiProbeLevel )
	{
		ULONG ulMinRow = GetMapValidMinRow();
		ULONG ulMinCol = GetMapValidMinCol();
		ULONG ulMaxRow = GetMapValidMaxRow();
		ULONG ulMaxCol = GetMapValidMaxCol();
		ULONG ulGridRows = m_lPrbMultiLevelGridRows;
		ULONG ulGridCols = m_lPrbMultiLevelGridCols;

		LONG lWftX = 0, lWftY = 0, lWftT = 0, lPL_Z;
		LONG lSpanRow = (ulMaxRow - ulMinRow)/(ulGridRows );
		LONG lSpanCol = (ulMaxCol - ulMinCol)/(ulGridCols);
		LONG lPLSpanX = labs(lSpanCol * GetDiePitchX_X());
		LONG lPLSpanY = labs(lSpanRow * GetDiePitchY_Y());
		ULONG ulPLCtrRow = PB_MPL_GRID_ROWS/2;
		ULONG ulPLCtrCol = PB_MPL_GRID_COLS/2;
		ULONG ulPLCtrRowOffset = 0;
		ULONG ulPLCtrColOffset = 0;
		if( ulGridRows<PB_MPL_GRID_ROWS )
		{
			ulPLCtrRowOffset = (PB_MPL_GRID_ROWS - ulGridRows)/2;
		}
		if( ulGridCols<PB_MPL_GRID_COLS )
		{
			ulPLCtrColOffset = (PB_MPL_GRID_COLS - ulGridCols)/2;
		}

		m_stM_P_L.InitPLPoints();

		for(ULONG ulPLRow=0; ulPLRow<=ulGridRows; ulPLRow++)
		{
			for(ULONG ulPLCol=0; ulPLCol<=ulGridCols; ulPLCol++)
			{
				ULONG ulMapRow = ulMinRow + ulPLRow * lSpanRow;
				ULONG ulMapCol = ulMinCol + ulPLCol * lSpanCol;
				if( IsWithinMapLimit(ulMapRow, ulMapCol, ulMinRow, ulMinCol, ulMaxRow, ulMaxCol)==FALSE )
				{
					continue;
				}

				if( FindValidMapDieInBlock(5, ulMapRow, ulMapCol)==FALSE )
				{
					continue;
				}

				if( GetMapPhyPosn(ulMapRow, ulMapCol, lWftX, lWftY)==FALSE )
				{
					continue;
				}

				MoveWftToProbeViaScanPosn(lWftX, lWftY);
				Sleep(100);
				PBTZ_SearchContactLevel();
				Sleep(1000);
			//	if (IsContactSensorsOn() == FALSE)
			//	{
			//	}

				GetEncoderValue();
				lPL_Z = m_lEnc_PBTZ;

				if( labs(lPL_Z-m_lContactLevel_PBTZ)>500 )
				{
					lPL_Z = m_lContactLevel_PBTZ;
				}

				PBTZ_MoveToFocusLevel();

				GetES101WTEncoder(&lWftX, &lWftY, &lWftT, FALSE);
				ULONG ulInRow = ulPLRow + ulPLCtrRowOffset;
				ULONG ulInCol = ulPLCol + ulPLCtrColOffset;
				m_stM_P_L.SetPointPL(ulInRow, ulInCol, lWftX, lWftY, lPL_Z, TRUE);
			}
		}

		if( m_stM_P_L.GetPointPL(ulPLCtrRow, ulPLCtrCol, lWftX, lWftY, lPL_Z)>0 )
		{
			MoveWaferTableNoCheck(lWftX, lWftY);
			Sleep(100);
		}

		LONG lPL_X = 0, lPL_Y = 0;
		FILE *fp;
		CTime stTime;
		stTime = CTime::GetCurrentTime();
		CString szTime;
		szTime = stTime.Format("%Y%m%d%H%M%S");
		fp = fopen("C:\\MapSorter\\Userdata\\History\\CP_MPL_" + szTime +".txt", "w");
		if( fp!=NULL )
		{
			fprintf(fp, "sample data just after detection\n");
			for(LONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
			{
				for(LONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
				{
					LONG lState = m_stM_P_L.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
					fprintf(fp, " %ld", lState);		//Klocwork	//v4.46
				}
				fprintf(fp, " \n");
			}
			fprintf(fp, "\n");
			for(LONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
			{
				for(LONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
				{
					LONG lState = m_stM_P_L.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
					fprintf(fp, " %ld,%ld,%ld,%ld,%ld\n", lRow, lCol, lPL_X, lPL_Y, lPL_Z);
				}
			}
		}
		if( m_stM_P_L.ReorganizeSampleData(lPLSpanX, lPLSpanY) )
		{
			m_stM_P_L.SetPLState(TRUE);
			if( fp!=NULL )
			{
				fprintf(fp, "sample data just after detection span %ld,%ld\n", lPLSpanX, lPLSpanY);		//Klocwork	//v4.46
				for(LONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
				{
					for(LONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
					{
						LONG lState = m_stM_P_L.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
						fprintf(fp, " %ld", lState);
					}
					fprintf(fp, " \n");
				}
				fprintf(fp, "\n");
				for(LONG lRow=0; lRow<PB_MPL_GRID_ROWS; lRow++)
				{
					for(LONG lCol=0; lCol<PB_MPL_GRID_COLS; lCol++)
					{
						LONG lState = m_stM_P_L.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
						fprintf(fp, " %ld,%ld,%ld,%ld,%ld\n", lRow, lCol, lPL_X, lPL_Y, lPL_Z);		//Klocwork	//v4.46
					}
					fprintf(fp, " \n");
				}
			}
		}
		else
		{
			HmiMessage_Red_Back("Auto Detect multi probe level error!", "CP100");
			bReturn = FALSE;
		}

		if( fp!=NULL )
		{
			fclose(fp);
		}
		SaveBhData();
	}

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CBondHead::BH_TheProcedureOfThirdButton(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	IPC_CServiceMessage stMsg;
	// Turn On light
	LONG lOpticsID = 0;
	stMsg.InitMessage(sizeof(LONG), &lOpticsID);		
	int nConvID = m_comClient.SendRequest("BondPrStn", "SetCleanProbeLighting", stMsg);

	HmiMessageEx("Please adjust the probe pin position!","The Probe Level TEST",glHMI_MBX_CLOSE,
						glHMI_ALIGN_CENTER,36000000,glHMI_MSG_MODAL,
						NULL,-1,-1,NULL,NULL,NULL,NULL,-1,0);

	if ( PB_CheckProberZContactDifference(TRUE, m_lSensorMaxTorlance) )
	{
		PBTZ_MoveToFocusLevel();
		// Find the contact position
		LONG lSearchLevel = max(m_lBPGeneral_BHZ1, m_lBPGeneral_BHZ2);

		Sleep(1000);
		//PBTZ_MoveToFocusLevel();
		PBTZ_MoveTo(m_lProbeFocusLevelZ);
		
		// Move down 1/2 die pitch(WT_Diag_Move_Y)
		LONG lHalfDiePitch = GetDiePitchY_Y();

		LONG lTarget = lHalfDiePitch/2;

		// WT move 1/2 die pitch
		IPC_CServiceMessage stMsg2;
		stMsg2.InitMessage(sizeof(LONG), &lTarget);
		nConvID = m_comClient.SendRequest("WaferTableStn", "WT_Diag_Move_Y", stMsg2);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg2);
				break;
			}
			else
			{
				Sleep(10);
			}	
		}
		
		// Output Message
		CString szReplyMessage, szTitle;
		szReplyMessage.Format("Contact Sensor 1 open level : %d \nContact Sensor 2 open level : %d \ndifference = %d", m_lBPGeneral_BHZ1, m_lBPGeneral_BHZ2, m_lBPGeneral_BHZ1 - m_lBPGeneral_BHZ2);
		szTitle.Format("The Probe Level TEST(%d)", m_lContactLevel_PBTZ);

		
		HmiMessage(szReplyMessage,szTitle,glHMI_MBX_CLOSE,
					glHMI_ALIGN_CENTER,36000000,glHMI_MSG_MODAL,
					NULL,-1,-1,NULL,NULL,NULL,NULL,-1,0);
		

		bReturn = TRUE;

		lTarget = -lHalfDiePitch/2;
		stMsg2.InitMessage(sizeof(LONG), &lTarget);
		int	nConvID = m_comClient.SendRequest("WaferTableStn", "WT_Diag_Move_Y", stMsg2);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg2);
				break;
			}
			else
			{
				Sleep(10);
			}	
		}

		
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::CheckProberZContactLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = PBT_UpdatePinsContactLevel();

	PBTZ_MoveToFocusLevel();

	CString szMsg, szTemp;
	szMsg.Format("Contact Sensor 1 Enc:%d  \nContact Sensor 2 Enc:%d", m_lBPGeneral_BHZ1, m_lBPGeneral_BHZ2);
	if( GetPinNo()>=4 )
	{
		szTemp.Format("Contact Sensor 3 Enc:%d  \nContact Sensor 4 Enc:%d", m_lBPGeneral_BHZ3, m_lBPGeneral_BHZ4);
		szMsg = szMsg + "\n" + szTemp;
	}
	if( GetPinNo()>=6 )
	{
		szTemp.Format("Contact Sensor 5 Enc:%d  \nContact Sensor 6 Enc:%d", m_lBPGeneral_BHZ5, m_lBPGeneral_BHZ6);
		szMsg = szMsg + "\n" + szTemp;
	}
	HmiMessage(szMsg);

	szMsg = "";
	if( labs(m_lBPGeneral_BHZ1-m_lBPGeneral_BHZ2)>300 )
	{
		szTemp.Format("Pin 1_2 Level difference over %d counters.\n", m_lBPGeneral_BHZ1-m_lBPGeneral_BHZ2);
		szMsg = szMsg + szTemp;
	}

	if( GetPinNo()>=4 )
	{
		if( labs(m_lBPGeneral_BHZ3-m_lBPGeneral_BHZ4)>300 )
		{
			szTemp.Format("Pin 3_4 Level difference over %d counters.\n", m_lBPGeneral_BHZ3-m_lBPGeneral_BHZ4);
			szMsg = szMsg + szTemp;
		}
	}

	if( GetPinNo()>=6 )
	{
		if( labs(m_lBPGeneral_BHZ5-m_lBPGeneral_BHZ6)>300 )
		{
			szTemp.Format("Pin 5_6 Level difference over %d counters.\n", m_lBPGeneral_BHZ5-m_lBPGeneral_BHZ6);
			szMsg = szMsg + szTemp;
		}
	}

	if( szMsg!="" )
		HmiMessage(szMsg, "Warning (over 300)");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PB_CheckProbeSensorLevel(IPC_CServiceMessage &svMsg)
{
	LONG lProbeZ = GetProbingLevel(FALSE);
	CString szMsg, szTitle;

	PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);
	Sleep(500);
	// Search contact sensor 1
	PBTZ_SearchContactLevel();

	if (IsContactSensorsOn() == FALSE)
	{
		szTitle = "none open";
		szMsg.LoadString(HMB_BH_SEARCH_CONTACT_SNR_FAIL);
	}
	else
	{
		GetEncoderValue();
		LONG lContactZ = m_lEnc_PBTZ;
		if( IsEnableSearch() )
		{
			szTitle = "2 Levels";
			szMsg.Format("Move Search Level %d;\nContact Sensor Level %d.", lProbeZ, lContactZ);
		}
		else
		{
			szTitle = "3 Levels";
			LONG lLevelZ = lProbeZ - m_lPrbLearnLevelOffset;
			szMsg.Format("Move Level %d;\nLearnt Level %d;\nContact Sensor Level %d.", lProbeZ, lLevelZ, lContactZ);
		}
	}

	HmiMessage(szMsg, szTitle);
	PBTZ_MoveToFocusLevel();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondHead::PB_IsZAtSafeLevel(IPC_CServiceMessage &svMsg)
{
	Sleep(100);
	GetEncoderValue();
	LONG lLevel_Z = m_lEnc_PBTZ;

	BOOL bReturn = TRUE;
	if( lLevel_Z>(m_lContactLevel_PBTZ-20) )
	{
		HmiMessage_Red_Back("Probe Z at contact level.\nNo table movement!", "CP100");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CBondHead::PB_PBTZMoveToLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	LONG lLevel_Z;
	svMsg.GetMsg(sizeof(LONG), &lLevel_Z);

	GetEncoderValue();
	LONG lCurrent_Z = m_lEnc_PBTZ;

	if (PBTZ_MoveTo(m_lWaferFocusLevelZ + lLevel_Z) != gnOK)
	{
		CString k;
		k.Format("Move to Motor Count %d Error!", lLevel_Z);
		AfxMessageBox(k);
		PBTZ_MoveTo(lCurrent_Z);
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBT_SetCleanPinAirCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	SetCleanPinAir(bOn);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBT_ProbeEncDiag(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if ( IsProber() == FALSE || m_bDisableBH == TRUE || PBTZ_IsPowerOn() == FALSE )
	{
		HmiMessage("It is not applied for this function.");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lContactSensorNo;
	svMsg.GetMsg(sizeof(LONG), &lContactSensorNo);

	if ( lContactSensorNo < 0 )
	{
		bReturn = FALSE;
		HmiMessage("It is wrong setting for this function.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if ( lContactSensorNo > GetPinNo() )
	{
		HmiMessage("Pin number over its hardware limit.");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	// 0:all pin
	// 1,2,3,4: pin1,2,3,4
	if ( lContactSensorNo == 0 && m_bMotionApplyProcessBlock == TRUE)
	{
		PBTZ_DownLoadSearchProfile(0);
		PBTZ_SearchContactLevel_PB();
	}
	else
	{
		// Search contact sensor
		PBTZ_SearchContactLevel(lContactSensorNo);
	}
	GetEncoderValue();

	CString szMsg, szTitle;
	szTitle.Format("Diag");
	szMsg.Format("Encoder Position: %d", m_lEnc_PBTZ);

	if ( lContactSensorNo != 0 )
	{
		HmiMessage(szMsg,szTitle);
		PBTZ_MoveTo(m_lProbeFocusLevelZ);
	}

	PBTZ_DownLoadSearchProfile(0);	//	detect pin level at service page

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBT_UpdatePinEncorder(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = PBT_UpdatePinsContactLevel();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::PBT_SafetyCheckingBeforeGo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lAllPinIsNotTouch = (LONG)pow(2, 1) + (LONG)pow(2, 2);

	m_lTheTouchedContactSensor = 0;

	if (IsPbContactSensor1On() == FALSE)
	{
		m_lTheTouchedContactSensor += (LONG)pow(2, 1);
	}

	if (IsPbContactSensor2On() == FALSE)
	{
		m_lTheTouchedContactSensor += (LONG)pow(2, 2);
	}
	
	if ( GetPinNo() == 4 )
	{
		lAllPinIsNotTouch += (LONG)pow(2, 3) + (LONG)pow(2, 4);
		if (IsPbContactSensor3On() == FALSE)
		{
			m_lTheTouchedContactSensor += (LONG)pow(2, 3);
		}
				
		if (IsPbContactSensor4On() == FALSE)
		{
			m_lTheTouchedContactSensor += (LONG)pow(2, 4);
		}
	}

	if ( m_lTheTouchedContactSensor != lAllPinIsNotTouch )
	{
		CString szGoError;
		szGoError.Format("GO ERROR! (%d)", m_lTheTouchedContactSensor);
		HmiMessage_Red_Back("ES is open!", szGoError);
		bReturn = FALSE;
	}
	else
	{ 
		GetEncoderValue();
		if ( m_lEnc_PBTZ > m_lProbeFocusLevelZ )
		{
			PBTZ_MoveTo(m_lProbeFocusLevelZ, SFM_WAIT);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

#if 0
BOOL CBondHead::PBTZ_OpenDacFineTune(INT *pnContactLevel, INT *pnOverShotLevel, INT *pnFinalLevel)
{
	if (IsEnableSearch() == FALSE)
	{
		HmiMessage("PBT Z move search is disable.");
		return FALSE;
	}
	if (!m_fHardware || !m_bSel_PBTZ || m_bDisableBH)
	{
		HmiMessage("PBT Z hardware invalid.");
		return FALSE;
	}
	if (IsMotionCE() == TRUE)
	{
		HmiMessage("PBT Z Motion system critical error.");
		return FALSE;
	}
	if (!m_bHome_PBTZ)
	{
		HmiMessage("PBT Z not home, please do it first.");
		return FALSE;
	}
	if ( m_bIsEnableOpenDac == FALSE )
	{
		HmiMessage("PBT Z OpenDac is disable.");
		return FALSE;
	}

#ifdef PROBER
	CMSNmAxisInfo	*pAxis_PZ = &m_stBHAxis_ProberZ;
	if (pAxis_PZ == NULL)
	{
		HmiMessage("PBT Z NULL Axis Info in Open Dac Fine Tune.");
		return FALSE;
	}

	LONG lContactLevelZ = m_lContactLevel_PBTZ;
	INT	nMoveDistance = lContactLevelZ + m_lPbContactLevelOffset;

	PBTZ_DownLoadSearchProfile(0);

	PBTZ_SelectObwProfile((LONG)abs(nMoveDistance));

	try
	{
		GMP_MOTION_ID	*pstMotID = &(pAxis_PZ->m_stMotionID);
		GMP_EVT_OBJECT		stSrchEvt;
		GMP_MOV_INPUT		stMvSrchIn,		stMvOnlyIn;
		GMP_PROF			stMvSrchPf,		stMvOnlyPf;
		GMP_STATIC_INPUT	stOpenDacSC,	stBe4MoveSC;
		GMP_U8 ucSrchDirection;
		GMP_FLOAT fSrcVel, fMaxVel;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcMoveCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcSrchCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		if( pAxis_PZ->m_szName.IsEmpty() )
		{
			CAsmException e((UINT)0, "No such ch name", "PBTZ OpenDacFineTune");
			throw e;
		}

		strcpy(pcChName, pAxis_PZ->m_szName);
		// static control
		//if( BH_AXIS_PROBER_Z==BH_AXIS_PROBER_Z && IsProber() )
		//	strcpy(pcSrchCtrlID, pAxis_PZ->m_stControl[PL_MOVESEARCH].m_szID);
		//else
			strcpy(pcSrchCtrlID, pAxis_PZ->m_stControl[PL_STATIC].m_szID);

		UCHAR ucMovePfID = pAxis_PZ->m_ucMoveProfileID;
		UCHAR ucSrchPfID = pAxis_PZ->m_ucSrchProfileID;
		strcpy(pcMoveCtrlID, pAxis_PZ->m_stControl[pAxis_PZ->m_ucControlID].m_szID);
		strcpy(pcPortID, pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_szPort);

		//Protect SearchVel should not be > Move MaxVel
		fSrcVel = (GMP_FLOAT)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_dSpeed;
		fMaxVel = (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxVel;
		if ( fSrcVel > fMaxVel )
		{
			fSrcVel = fMaxVel;
		}

		stSrchEvt.enDrvInType		= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrchEvt.enLmtType			= GMP_EVT_LMT_DIST; 
		stSrchEvt.bEnaErrChk		= GMP_FALSE;
		stSrchEvt.enType			= (GMP_EVT_TYPE)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_lType;
		stSrchEvt.pcPort1			= pcPortID;
		stSrchEvt.u16SrhDebounceSmpl= (GMP_U16) m_lPbSearchDebounce;           
		stSrchEvt.fSrhDrvInVel		= (GMP_FLOAT)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_dDrvInSpeed;
		stSrchEvt.u32DrvInCnt		= (GMP_U32)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_ulDrvInDistance;
		stSrchEvt.fSrhVel			= fSrcVel;
		stSrchEvt.u32SrhLmt			= (GMP_U32)(abs(pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_lDistance));
		stSrchEvt.u32Mask			= (GMP_U32)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_ulMask;
		stSrchEvt.u32ActiveState	= (GMP_U32)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_ulAState;

		if ( stSrchEvt.fSrhDrvInVel > stSrchEvt.fSrhVel )
		{
			stSrchEvt.fSrhDrvInVel	= stSrchEvt.fSrhVel;
		}

		if ( nMoveDistance < 0 )
		{
			ucSrchDirection = GMP_NEGATIVE_SRH_DIR;
		}
		else
		{
			ucSrchDirection = GMP_POSITIVE_SRH_DIR;
		}

		//Define MoveSrch values
		if ( pAxis_PZ->m_lProfileType == MS896A_OBW_PROFILE)
		{
			stMvSrchPf.enProfType					= FIFTH_ORD_MOV_PROF_B;
			stMvSrchPf.st5OrdMoveProfB.fFinalVel	= stSrchEvt.fSrhVel;	//Final Velocity should equal to search speed
			stMvSrchPf.st5OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxAcc;
			stMvSrchPf.st5OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxDec;
			stMvSrchPf.st5OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxVel;

			stMvOnlyPf.enProfType					= FIFTH_ORD_MOV_PROF_B;
			stMvOnlyPf.st5OrdMoveProfB.fFinalVel	= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dFinalVel;
			stMvOnlyPf.st5OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxAcc;
			stMvOnlyPf.st5OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxDec;
			stMvOnlyPf.st5OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxVel;

			stMvSrchIn.enInputType							= GMP_OBWMOVSRH_TYPE;
			stMvSrchIn.pcCtrlType							= pcMoveCtrlID;
			stMvSrchIn.stObwMovSrhInput.pcSrhCtrlType		= pcSrchCtrlID;
			stMvSrchIn.stObwMovSrhInput.bEnaSetPosn			= GMP_FALSE;
			stMvSrchIn.stObwMovSrhInput.s32DesiredAbsPosn	= 0;
			stMvSrchIn.stObwMovSrhInput.s32Dist				= (GMP_S32)nMoveDistance;
			stMvSrchIn.stObwMovSrhInput.u8UserMotID			= 1;
			stMvSrchIn.stObwMovSrhInput.u8SrhDir			= ucSrchDirection;

			stMvOnlyIn.enInputType							= GMP_OBWMOV_B_TYPE;
			stMvOnlyIn.pcCtrlType							= pcMoveCtrlID;
			stMvOnlyIn.stObwMovBInput.s32Dist				= (GMP_S32)m_lStandByLevel_PBTZ;
			stMvOnlyIn.stObwMovBInput.u8UserMotID			= 1;
		}
		else
		{
			stMvSrchPf.enProfType					= THIRD_ORD_MOV_PROF_B;
			stMvSrchPf.st3OrdMoveProfB.fFinalVel	= stSrchEvt.fSrhVel;	//Final Velocity should equal to search speed
			stMvSrchPf.st3OrdMoveProfB.fJerk		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dJerk;
			stMvSrchPf.st3OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxVel;
			stMvSrchPf.st3OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxAcc;
			stMvSrchPf.st3OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxDec;

			stMvOnlyPf.enProfType					= THIRD_ORD_MOV_PROF_B;
			stMvOnlyPf.st3OrdMoveProfB.fFinalVel	= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dFinalVel;
			stMvOnlyPf.st3OrdMoveProfB.fJerk		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dJerk;
			stMvOnlyPf.st3OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxVel;
			stMvOnlyPf.st3OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxAcc;
			stMvOnlyPf.st3OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxDec;

			stMvSrchIn.enInputType							= GMP_3ORD_MOVSRH_TYPE;
			stMvSrchIn.pcCtrlType							= pcMoveCtrlID;
			stMvSrchIn.st3OrdMovSrhInput.pcSrhCtrlType		= pcSrchCtrlID;
			stMvSrchIn.st3OrdMovSrhInput.bEnaSetPosn		= GMP_FALSE;
			stMvSrchIn.st3OrdMovSrhInput.s32DesiredAbsPosn	= 0;
			stMvSrchIn.st3OrdMovSrhInput.s32Dist			= (GMP_S32)nMoveDistance;
			stMvSrchIn.st3OrdMovSrhInput.u8UserMotID		= 1;
			stMvSrchIn.st3OrdMovSrhInput.u8SrhDir			= ucSrchDirection;

			stMvOnlyIn.enInputType					= GMP_3ORD_MOV_B_TYPE;
			stMvOnlyIn.pcCtrlType					= pcMoveCtrlID;
			stMvOnlyIn.st3OrdMovBInput.s32Dist		= (GMP_S32)m_lStandByLevel_PBTZ;
			stMvOnlyIn.st3OrdMovBInput.u8UserMotID	= 1;
		}

		//	CTRL_API AGMP_CTRL_FUNC_RETURN  
		//Search point is the search to contact active state encoder position, 
		//Max point is the over shot encoder position, 
		//Final point drive in (final encoder position).
		//
		//This function includes the same before as search -> open dac -> move down.
		// ? the idle time is default internal and need to double check.
		//
		CHAR pcLv1CtrlID[GMP_CH_NAME_CHAR_LEN];
		strcpy(pcLv1CtrlID, pAxis_PZ->m_stControl[PL_OPEN_DAC].m_szID);
		stOpenDacSC.pcStaticCtrlType  = pcLv1CtrlID;
		strcpy(pcLv1CtrlID, pAxis_PZ->m_stControl[PL_STATIC].m_szID);
		stBe4MoveSC.pcStaticCtrlType = pcLv1CtrlID;

		agmp_ctrl_CP_search_contact_with_drivein(	pcChName,	stSrchEvt,	stMvSrchIn,	100,
									stOpenDacSC, stBe4MoveSC,	GMP_NON_SYNC_GROUP,	pstMotID,
									stMvOnlyIn,	stMvOnlyPf,	pnContactLevel, pnOverShotLevel, pnFinalLevel);
		pAxis_PZ->m_sErrCode = 0;

		//if ( ( sReturn!= 0 )
		//{
		//	CAsmException e((UINT)pAxis_PZ->m_sErrCode, "", pAxis_PZ->m_szName);
		//	throw e;
		//}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return FALSE;
	}
#endif

	return TRUE;
}
#endif

//	true for auto tune, false for verify.
LONG CBondHead::AutoFineTuneOpenDac(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	BOOL bTune = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bTune);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (IsEnableSearch() == FALSE)
	{
		HmiMessage("PBT Z move search is disable.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}
	if (!m_fHardware || !m_bSel_PBTZ || m_bDisableBH)
	{
		HmiMessage("PBT Z hardware invalid.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}
	if (IsMotionCE() == TRUE)
	{
		HmiMessage("PBT Z Motion system critical error.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}
	if (!m_bHome_PBTZ)
	{
		HmiMessage("PBT Z not home, please do it first.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}
	if ( m_bIsEnableOpenDac == FALSE )
	{
		HmiMessage("PBT Z OpenDac is disable.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}

	ULONG ulGridRows = m_lAutoOpenDacRow;
	ULONG ulGridCols = m_lAutoOpenDacCol;

    CMS896AApp::m_bStopAlign = FALSE;

	LONG lWaferDiameter = GetWaferDiameter();
	LONG lUL_X = GetWaferCenterX() + lWaferDiameter/2;
	LONG lUL_Y = GetWaferCenterY() + lWaferDiameter/2;
	LONG	lSpanY = labs(lWaferDiameter/ulGridRows);
	LONG	lSpanX = labs(lWaferDiameter/ulGridCols);

	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest("WaferPrStn", "SelectBondCamera", stMsg);
	Sleep(100);

	LONG lCLevelMax = -9999999, lCLevelMin = 9999999, lCLevelAvg = 0;
	LONG lOLevelMax = -9999999, lOLevelMin = 9999999, lOLevelAvg = 0;
	LONG lDLevelMax = -9999999, lDLevelMin = 9999999, lDLevelAvg = 0;
	// sampling at scan camera side.
	PBTZ_MoveTo(0, SFM_WAIT);

	CString szSourceFile = "c:\\MapSorter\\UserData\\History\\AutoData.csv";
	FILE *fp = fopen(szSourceFile, "w");
	ULONG ulPointNo = 0;

	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		LONG lCol = 0;
		if( lRow%2!=0 )
			lCol = ulGridCols;
		while( 1 )
		{
			if( pApp->IsStopAlign() )
			{
				HmiMessage_Red_Back("Probing offset sampling operation stopped, please do it again!");
				break;
			}


			LONG lScanX = lUL_X - lCol*lSpanX;
			LONG lScanY = lUL_Y - lRow*lSpanY;

			if( IsWithinInputWaferLimit(lScanX, lScanY) )
			{
				if( MoveWftToProbeViaScanPosn(lScanX, lScanY) )
				{
					INT nCLevel = 0, nOLevel = 0, nDLevel = 0;
					Sleep(100);
					PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);
					if( PBTZ_OpenDacFineTune(&nCLevel, &nOLevel, &nDLevel)==FALSE )
					{
						break;
					}
					PBTZ_MoveTo(0, SFM_WAIT);
					ulPointNo++;
					if( fp!=NULL )
					{
						fprintf(fp, "%lu,%ld,%ld,%d,%d,%d,%ld\n",	//v4.51A20	//Klocwork
							ulPointNo, lScanX, lScanY, nCLevel, nOLevel, nDLevel, m_lContactLevel_PBTZ);
					}
					LONG lCDiff = nCLevel - m_lContactLevel_PBTZ;
					LONG lODiff = nOLevel - nCLevel;
					LONG lNDiff = nDLevel - nCLevel;
					if( lCLevelMax<lCDiff )
						lCLevelMax = lCDiff;
					if( lCLevelMin>lCDiff )
						lCLevelMin = lCDiff;
					lCLevelAvg += lCDiff;
					if( lOLevelMax<lODiff )
						lOLevelMax = lODiff;
					if( lOLevelMin>lODiff )
						lOLevelMin = lODiff;
					lOLevelAvg += lODiff;
					if( lDLevelMax<lNDiff )
						lDLevelMax = lNDiff;
					if( lDLevelMin>lNDiff )
						lDLevelMin = lNDiff;
					lDLevelAvg += lNDiff;
				}
			}
			if( lRow%2!=0 )
			{
				lCol--;
				if( lCol<0 )
				{
					break;
				}
			}
			else
			{
				lCol++;
				if( lCol>(LONG)ulGridCols )
				{
					break;
				}
			}
		}
		if( pApp->IsStopAlign() )
		{
			break;
		}
	}

	if( fp!=NULL )
	{
		fclose(fp);
	}

	if( ulPointNo!=0 )
	{
		lCLevelAvg = lCLevelAvg/ulPointNo;
		lOLevelAvg = lOLevelAvg/ulPointNo;
		lDLevelAvg = lDLevelAvg/ulPointNo;
	}

	szSourceFile = "c:\\MapSorter\\UserData\\History\\AutoDacSummaryList.csv";
	CStdioFile fCheckFile;
	CString szLastResult = "";
	if( fCheckFile.Open(szSourceFile, CFile::modeRead|CFile::shareDenyNone) )
	{
		fCheckFile.SeekToBegin();
		CString szReading;
		while( fCheckFile.ReadString(szReading) )
		{
			szLastResult = szReading;
		}
	}
	int nAccess = access(szSourceFile, 0);
	fp = fopen(szSourceFile, "a");
	if( fp!=NULL )
	{
		if( nAccess==-1 )
		{
			fprintf(fp, "C Max,C Min,C Avg,O Max,O Min,O Avg,D Max,D Min,D Avg,\n");
		}
		fprintf(fp, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,\n", 
			lCLevelMax, lCLevelMin, lCLevelAvg, lOLevelMax, lOLevelMin, lOLevelAvg, lDLevelMax, lDLevelMin, lDLevelAvg);
		fclose(fp);
	}

	szSourceFile = "c:\\MapSorter\\Exe\\AutoDacSummary.csv";
	fp = fopen(szSourceFile, "w");
	if( fp!=NULL )
	{
		fprintf(fp, "Max,Min,Avg,Max,Min,Avg,Max,Min,Avg,\n");
		if( szLastResult!="" )
			fprintf(fp, "%s\n", (LPCTSTR) szLastResult);			//v4.51A20	//Klocwork
		fprintf(fp, "%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,\n",		//v4.51A20	//Klocwork
			lCLevelMax, lCLevelMin, lCLevelAvg, lOLevelMax, lOLevelMin, lOLevelAvg, lDLevelMax, lDLevelMin, lDLevelAvg);
		fclose(fp);
	}

	MoveWftToProbeViaScanPosn(GetWaferCenterX(), GetWaferCenterY());
	PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);
	m_lAutoDacDataIndex = -1;
	DisplayProbeLevel();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::CalibrateOpenDac(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	//! Fine tune Control parameter based on search output\n
#ifdef PROBER
	CMSNmAxisInfo	*pAxis_PZ = &m_stBHAxis_ProberZ;
	if (pAxis_PZ == NULL)
	{
		HmiMessage("PBT Z NULL Axis Info in Open Dac Fine Tune.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}

	CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
	CHAR pcOpenDacCtrlID[GMP_CH_NAME_CHAR_LEN];

	strcpy(pcChName, pAxis_PZ->m_szName);
	strcpy(pcOpenDacCtrlID, pAxis_PZ->m_stControl[PL_OPEN_DAC].m_szID);

	CString szSourceFile = "c:\\MapSorter\\UserData\\History\\AutoData.csv";
	LONG lLineNo = 0;
	CStdioFile fCheckFile;
	if( fCheckFile.Open(szSourceFile, CFile::modeRead|CFile::shareDenyNone) )
	{
		fCheckFile.SeekToBegin();
		CString szReading;
		while( fCheckFile.ReadString(szReading) )
		{
			lLineNo++;
		}
	}

	if( lLineNo>0 )
	{
		GMP_S32*	ps32OverShootPoints;
		GMP_S32*	ps32DriveInPoints;

		ps32OverShootPoints = new GMP_S32[lLineNo+5];
		ps32DriveInPoints	= new GMP_S32[lLineNo+5];

		fCheckFile.SeekToBegin();
		CString szReading;
		LONG lIndexNo = 0;
		CStringArray szaDataList;
		while( fCheckFile.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			ParseRawData(szReading, szaDataList);
			if( szaDataList.GetSize()>=7 )
			{
				ps32OverShootPoints[lIndexNo]	= atoi(szaDataList.GetAt(4)) - atoi(szaDataList.GetAt(3));
				ps32DriveInPoints[lIndexNo]		= atoi(szaDataList.GetAt(5)) - atoi(szaDataList.GetAt(3));
				CString szMsg;
				szMsg.Format("%d,%d,%d", lIndexNo+1, ps32OverShootPoints[lIndexNo], ps32DriveInPoints[lIndexNo]);
				SetAlarmLog(szMsg);
				lIndexNo++;
			}
		}

		agmp_ctrl_CP_tune_search_contact_control(pcChName, pcOpenDacCtrlID,
			ps32OverShootPoints, lIndexNo, ps32DriveInPoints, lIndexNo, m_lOverShootLimit, m_lDriveInLimit);

		delete [] ps32OverShootPoints;
		delete [] ps32DriveInPoints;
	}

	fCheckFile.Close();
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ConfirmAutoOpenDac(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	//! Fine tune Control parameter based on search output\n
#ifdef PROBER
	CMSNmAxisInfo	*pAxis_PZ = &m_stBHAxis_ProberZ;
	if (pAxis_PZ == NULL)
	{
		HmiMessage("PBT Z NULL Axis Info in Open Dac Fine Tune.");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return FALSE;
	}

	CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
	CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

	strcpy(pcChName, pAxis_PZ->m_szName);
	strcpy(pcCtrlID, pAxis_PZ->m_stControl[PL_OPEN_DAC].m_szID);

	//! Upload search contact control parameters
	float fStaticKd = 0;
	GMP_S32 sPosBias = 0, sNegBias = 0;
	agmp_ctrl_CP_upload_search_contact_control(pcChName, pcCtrlID, &fStaticKd, &sPosBias, &sNegBias);
	m_bAutoDacTuned	= TRUE;	// confirm
	m_fTuneOpenDacKd		= fStaticKd;
	m_s32TuneOpenDacPosBias	= sPosBias;
	m_s32TuneOpenDacNdgBias	= sNegBias;
	CString szMsg;
	szMsg.Format("confirm agmp upload control %f,%d,%d", fStaticKd, sPosBias, sNegBias);
	SetAlarmLog(szMsg);

    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    if (pUtl->LoadCP_ComConfig() )
	{
		// get file pointer
		psmf = pUtl->GetCP_ComConfigFile();
		//Check Load/Save Data
		if (psmf != NULL)
		{
			(*psmf)["CP Open Dac Control"]["Tune Done"]			= m_bAutoDacTuned;	// confirm
			(*psmf)["CP Open Dac Control"]["Static Kd"]			= fStaticKd;
			(*psmf)["CP Open Dac Control"]["Static Pos Bias"]	= sPosBias;
			(*psmf)["CP Open Dac Control"]["Static Neg Bias"]	= sNegBias;
			
			//Check Load/Save Data
			pUtl->SaveCP_ComConfig() ;
			// close config file
			pUtl->CloseCP_ComConfig();
		}
	}
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ResetAutoOpenDacData(IPC_CServiceMessage &svMsg)
{
	CString szSourceFile = "c:\\MapSorter\\UserData\\History\\AutoData.csv";
	DeleteFile(szSourceFile);
	szSourceFile = "c:\\MapSorter\\UserData\\History\\AutoDacSummaryList.csv";
	DeleteFile(szSourceFile);
	szSourceFile = "c:\\MapSorter\\Exe\\AutoDacSummary.csv";
	FILE* fp = fopen(szSourceFile, "w");
	if( fp!=NULL )
	{
		fprintf(fp, "Max,Min,Avg,Max,Min,Avg,Max,Min,Avg,\n");
		fclose(fp);
	}

	m_lAutoDacDataIndex = -1;
	DisplayProbeLevel();

#ifdef PROBER
	m_bAutoDacTuned = FALSE;	// reset
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    if (pUtl->LoadCP_ComConfig() )
	{
		// get file pointer
		psmf = pUtl->GetCP_ComConfigFile();

		//Check Load/Save Data
		if (psmf != NULL)
		{
			(*psmf)["CP Open Dac Control"]["Tune Done"]			= m_bAutoDacTuned;	// reset
			
			//Check Load/Save Data
			pUtl->SaveCP_ComConfig() ;
			// close config file
			pUtl->CloseCP_ComConfig();
		}
	}

	CMSNmAxisInfo	*pAxis_PZ = &m_stBHAxis_ProberZ;
	if( IsProber() && (pAxis_PZ != NULL))
	{
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		strcpy(pcChName, pAxis_PZ->m_szName);
		strcpy(pcCtrlID, pAxis_PZ->m_stControl[PL_OPEN_DAC].m_szID);

		float fStaticKd	 = m_fScfOpenDacKd;
		GMP_S32 sPosBias = m_s32ScfOpenDacPosBias;
		GMP_S32 sNegBias = m_s32ScfOpenDacNdgBias;
		agmp_ctrl_CP_download_search_contact_control(pcChName, pcCtrlID, fStaticKd, sPosBias, sNegBias); // reset
		CString szMsg;
		szMsg.Format("reset agmp download control %f,%d,%d", fStaticKd, sPosBias, sNegBias);
		SetAlarmLog(szMsg);
	}
#endif

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ExitAutoOpenDacPage(IPC_CServiceMessage &svMsg)
{
#ifdef PROBER
	CMSNmAxisInfo	*pAxis_PZ = &m_stBHAxis_ProberZ;
	if( IsProber() && (pAxis_PZ != NULL))
	{
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		strcpy(pcChName, pAxis_PZ->m_szName);
		strcpy(pcCtrlID, pAxis_PZ->m_stControl[PL_OPEN_DAC].m_szID);
		float fStaticKd	 = m_fScfOpenDacKd;
		GMP_S32 sPosBias = m_s32ScfOpenDacPosBias;
		GMP_S32 sNegBias = m_s32ScfOpenDacNdgBias;
		if( m_bAutoDacTuned )	// exit
		{
			fStaticKd= m_fTuneOpenDacKd;
			sPosBias = m_s32TuneOpenDacPosBias;
			sNegBias = m_s32TuneOpenDacNdgBias;
		}
		agmp_ctrl_CP_download_search_contact_control(pcChName, pcCtrlID, fStaticKd, sPosBias, sNegBias);	// exit page
		CString szMsg;
		szMsg.Format("exit, agmp download control %f,%d,%d", m_fScfOpenDacKd, m_s32ScfOpenDacPosBias, m_s32ScfOpenDacNdgBias);
		SetAlarmLog(szMsg);
	}
#endif

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//	input index
LONG CBondHead::SpinDacDataDisplay(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(LONG), &m_lAutoDacDataIndex);
	DisplayProbeLevel();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::DisplayProbeLevel()
{
	CString szSourceFile = "c:\\MapSorter\\UserData\\History\\AutoData.csv";
	CString szFileName = "c:\\MapSorter\\Exe\\AutoOpenDacData.csv";

	LONG lDataIndex = labs(m_lAutoDacDataIndex);
	LONG lStartIndex = (lDataIndex-1)*10;
	LONG lEndIndex = (lDataIndex)*10;
	LONG lLineNo = 0;
	CStdioFile fCheckFile;
	if( fCheckFile.Open(szSourceFile, CFile::modeRead|CFile::shareDenyNone) )
	{
		fCheckFile.SeekToBegin();
		CString szReading;
		while( fCheckFile.ReadString(szReading) )
		{
			lLineNo++;
		}
		fCheckFile.Close();
	}
	if( lLineNo<lStartIndex )
	{
		lDataIndex--;
	}
	lStartIndex = (lDataIndex-1)*10;
	lEndIndex = (lDataIndex)*10;

	m_lAutoDacDataIndex = 0-lDataIndex;


	FILE *fp = fopen(szFileName, "w");
	//order,wafer x, wafer y, contact, overshoot,drive in, probe
	if( fp!=NULL )
	{
		CStdioFile fCheckFile;
		if( fCheckFile.Open(szSourceFile, CFile::modeRead|CFile::shareDenyNone) )
		{
			LONG lLineNo = 0;
			fCheckFile.SeekToBegin();
			CString szReading;
			while( fCheckFile.ReadString(szReading) )
			{
				if( lLineNo>=lStartIndex && lLineNo<lEndIndex )
				{
					fprintf(fp, "%s,\n", (LPCTSTR) szReading);		//v4.51A20	//Klocwork
				}
				lLineNo++;
				if(lLineNo>=lEndIndex )
				{
					break;
				}
			}
			fCheckFile.Close();
		}
		fclose(fp);
	}

	return 1;
}

BOOL CBondHead::PBTZ_OpenDacFineTune(INT *pnContactLevel, INT *pnOverShotLevel, INT *pnFinalLevel)	// search up, move down
{
#ifdef PROBER
	CMSNmAxisInfo	*pAxis_PZ = &m_stBHAxis_ProberZ;
	if (pAxis_PZ == NULL)
	{
		HmiMessage("PBT Z NULL Axis Info in Open Dac Fine Tune.");
		return FALSE;
	}

	INT	nMoveDistance = labs(m_lPbContactSearchDist);

	PBTZ_DownLoadSearchProfile(0);
	PBTZ_SelectObwProfile((LONG)abs(nMoveDistance));

	try
	{
		GMP_MOTION_ID	*pstMotID = &(pAxis_PZ->m_stMotionID);
		GMP_EVT_OBJECT		stSrchEvt;
		GMP_MOV_INPUT		stSearchUpIn,		stMoveDownIn;
		GMP_PROF			stMoveDownPf;
		GMP_STATIC_INPUT	stOpenDacSC,	stBe4MoveSC;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcMoveCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcSrchCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		if( pAxis_PZ->m_szName.IsEmpty() )
		{
			CAsmException e((UINT)0, "No such ch name", "PBTZ OpenDacFineTune");
			throw e;
		}

		strcpy(pcChName, pAxis_PZ->m_szName);
		strcpy(pcSrchCtrlID, pAxis_PZ->m_stControl[PL_STATIC].m_szID);

		UCHAR ucMovePfID = pAxis_PZ->m_ucMoveProfileID;
		UCHAR ucSrchPfID = pAxis_PZ->m_ucSrchProfileID;
		CString szMsg;
		strcpy(pcMoveCtrlID, pAxis_PZ->m_stControl[pAxis_PZ->m_ucControlID].m_szID);
		strcpy(pcPortID, pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_szPort);

		stSrchEvt.enDrvInType		= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrchEvt.enLmtType			= GMP_EVT_LMT_DIST; 
		stSrchEvt.bEnaErrChk		= GMP_FALSE;
		stSrchEvt.enType			= (GMP_EVT_TYPE)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_lType;
		stSrchEvt.pcPort1			= pcPortID;
		stSrchEvt.u16SrhDebounceSmpl= (GMP_U16) m_lPbSearchDebounce;	//	0          
		stSrchEvt.fSrhDrvInVel		= (GMP_FLOAT)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_dDrvInSpeed;
		stSrchEvt.u32DrvInCnt		= (GMP_U32)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_ulDrvInDistance;
		stSrchEvt.fSrhVel			= (GMP_FLOAT)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_dSpeed;
		stSrchEvt.u32SrhLmt			= (GMP_U32)(abs(pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_lDistance));
		stSrchEvt.u32Mask			= (GMP_U32)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_ulMask;
		stSrchEvt.u32ActiveState	= (GMP_U32)pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_ulAState;

		if ( stSrchEvt.fSrhDrvInVel > stSrchEvt.fSrhVel )
		{
			stSrchEvt.fSrhDrvInVel	= stSrchEvt.fSrhVel;
		}

		stSearchUpIn.enInputType						= GMP_SEARCH_TYPE;
		stSearchUpIn.pcCtrlType						= pcSrchCtrlID;
		stSearchUpIn.stSrhInput.bEnaSetPosn			= GMP_FALSE;
		stSearchUpIn.stSrhInput.s32DesiredAbsPosn		= 0;
		stSearchUpIn.stSrhInput.u8UserMotID			= 1;
		stSearchUpIn.stSrhInput.u8SrhDir				= GMP_POSITIVE_SRH_DIR;
		if ( pAxis_PZ->m_stSrchProfile[ucSrchPfID].m_lDistance < 0 )
		{
			stSearchUpIn.stSrhInput.u8SrhDir			= GMP_NEGATIVE_SRH_DIR;
		}

		//Define MoveSrch values
		if ( pAxis_PZ->m_lProfileType == MS896A_OBW_PROFILE)
		{
			stMoveDownPf.enProfType					= FIFTH_ORD_MOV_PROF_B;
			stMoveDownPf.st5OrdMoveProfB.fFinalVel	= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dFinalVel;
			stMoveDownPf.st5OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxAcc;
			stMoveDownPf.st5OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxDec;
			stMoveDownPf.st5OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxVel;

			stMoveDownIn.enInputType							= GMP_OBWMOV_B_TYPE;
			stMoveDownIn.stObwMovBInput.s32Dist				= (GMP_S32)m_lStandByLevel_PBTZ;
			stMoveDownIn.stObwMovBInput.u8UserMotID			= 1;
		}
		else
		{
			stMoveDownPf.enProfType					= THIRD_ORD_MOV_PROF_B;
			stMoveDownPf.st3OrdMoveProfB.fFinalVel	= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dFinalVel;
			stMoveDownPf.st3OrdMoveProfB.fJerk		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dJerk;
			stMoveDownPf.st3OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxVel;
			stMoveDownPf.st3OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxAcc;
			stMoveDownPf.st3OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxis_PZ->m_stMoveProfile[ucMovePfID].m_dMaxDec;

			stMoveDownIn.enInputType					= GMP_3ORD_MOV_B_TYPE;
			stMoveDownIn.st3OrdMovBInput.s32Dist		= (GMP_S32)m_lStandByLevel_PBTZ;
			stMoveDownIn.st3OrdMovBInput.u8UserMotID	= 1;
		}
		stMoveDownIn.pcCtrlType					= pcMoveCtrlID;

		//	CTRL_API AGMP_CTRL_FUNC_RETURN  
		//Search point is the search to contact active state encoder position, 
		//Max point is the over shot encoder position, 
		//Final point drive in (final encoder position).
		//
		//This function includes the same before as search -> open dac -> move down.
		// ? the idle time is default internal and need to double check.
		//
		CHAR pcOpenDacCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcB4MoveCtrlID[GMP_CH_NAME_CHAR_LEN];
		strcpy(pcOpenDacCtrlID, pAxis_PZ->m_stControl[PL_OPEN_DAC].m_szID);
		stOpenDacSC.pcStaticCtrlType  = pcOpenDacCtrlID;
		strcpy(pcB4MoveCtrlID, pAxis_PZ->m_stControl[PL_STATIC].m_szID);
		stBe4MoveSC.pcStaticCtrlType = pcB4MoveCtrlID;

		agmp_ctrl_CP_search_contact_with_drivein(	pcChName,	stSrchEvt,	stSearchUpIn,	100,
									stOpenDacSC, stBe4MoveSC,	GMP_NON_SYNC_GROUP,	pstMotID,
									stMoveDownIn,	stMoveDownPf,	pnContactLevel, pnOverShotLevel, pnFinalLevel);
		pAxis_PZ->m_sErrCode = 0;

		//if ( ( sReturn!= 0 )
		//{
		//	CAsmException e((UINT)pAxis_PZ->m_sErrCode, "", pAxis_PZ->m_szName);
		//	throw e;
		//}
		PBTZ_Sync();
		Sleep(50);
		GetEncoderValue();

		// do a search as it should be down to original start position.
		//GMP_PROCESS_BLK_LIST stPBL;
		//stPBL.u8PBLSize								= 0;
		//stSrchEvt.u16SrhDebounceSmpl				= 0;           
		//if ( (pAxis_PZ->m_sErrCode = gmp_srh(pcChName, &stSearchUpIn, &stSrchEvt, &stPBL, GMP_NON_SYNC_GROUP, &pAxis_PZ->m_stMotionID)) != 0 )
		//{
		//	CAsmException e((UINT)pAxis_PZ->m_sErrCode, "gmp_srh", pAxis_PZ->m_szName);
		//	throw e;
		//}
		//PBTZ_Sync();
		//Sleep(50);
		//GetEncoderValue();
		// do a search as it should be down to original start position.
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return FALSE;
	}
#endif

	return TRUE;
}	// move to contact, do something, down to standby level (start level)

BOOL CBondHead::PBT_UpdatePinsContactLevel()
{
	CString szMsg, szTitle;
	if ( IsProberPlus() && m_bIsPPPClose == FALSE)
	{
		HmiMessage("Please Close platform!");
		return FALSE;
	}

	if ( m_bMotionApplyProcessBlock == TRUE && IsEnableSearch() == TRUE )
	{
		PBTZ_Cycle_UpTo_Probe();
	}
	else
	{
		for(INT i=1; i<=GetPinNo(); i++)
		{
			PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);
			Sleep(500);

			// Search contact sensor i
			PBTZ_SearchContactLevel(i);

			if (IsPbContactSensorOn(i) == FALSE)
			{
				szMsg.LoadString(HMB_BH_SEARCH_CONTACT_SNR_FAIL);
				szTitle.Format("%d close", i);
				SetErrorMessage(szTitle + " : " + szMsg);
				HmiMessage(szTitle + " : " + szMsg);
				PBTZ_MoveToFocusLevel();
				break;
			}

			GetEncoderValue();
			switch( i )
			{
			case 1:
				m_lBPGeneral_BHZ1 = m_lEnc_PBTZ;
				break;
			case 2:
				m_lBPGeneral_BHZ2 = m_lEnc_PBTZ;
				break;
			case 3:
				m_lBPGeneral_BHZ3 = m_lEnc_PBTZ;
				break;
			case 4:
				m_lBPGeneral_BHZ4 = m_lEnc_PBTZ;
				break;
			case 5:
				m_lBPGeneral_BHZ5 = m_lEnc_PBTZ;
				break;
			case 6:
				m_lBPGeneral_BHZ6 = m_lEnc_PBTZ;
				break;
			}
		}
	}

	PBTZ_DownLoadSearchProfile(0);

	PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);

	return TRUE;
}

LONG CBondHead::LoopTestAutoOpenDac(IPC_CServiceMessage &svMsg)
{
	CMS896AApp::m_bStopAlign = FALSE;
	PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);

	if (IsEnableSearch())
	{
		PBTZ_DownLoadSearchProfile(0);	// prestart, indeed no need, as will download for every move
	}
	PBTZ_Profile(NORMAL_PROF);		
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	while( 1 )
	{
		if( PBTZ_Cycle_UpTo_Probe()==gnNOTOK )
		{
			break;
		}
//		PBTZ_Sync();
		MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_OPEN_DAC);
		PBTZ_Move(0, SFM_NOWAIT);		//PL_OPEN_DAC
		PBTZ_Sync();

		if (m_ulProbeDelay > 0)	//	block probing sleep the first time
		{
			Sleep(m_ulProbeDelay);
		}

		PBTZ_Cycle_DownTo_Standby();

		PBTZ_Sync();
		if (m_ulProbeDelay > 0)	//	block probing sleep the first time
		{
			Sleep(m_ulProbeDelay);
		}

		if( pApp->IsStopAlign() )
		{
			break;
		}
	}

	MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_STATIC);	// PL_OPEN_DAC

	PBTZ_Sync();

	PBTZ_MoveToFocusLevel();

	return 1;
}	//	auto loop to test open dac

//	m_bEnableUseProbeTableLevelByRegion		CalculateProbeZLevelFromXYPosByRegion