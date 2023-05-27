/////////////////////////////////////////////////////////////////
// BH_SubState.cpp : State for Auto-cycle of the CBondHead class
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
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "WT_SubRegion.h"

#include "MS896A.h"		// For Timing
#include "PRFailureCaseLog.h"
#include "BH_AirFlowCompensation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Sequence Operations
////////////////////////////////////////////
VOID CBondHead::BackupBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bCheckMissingDie_Backup = m_bCheckMissingDie;
		m_bCheckMissingDie = FALSE;

		m_bCheckColletJam_Backup = m_bCheckColletJam;
		m_bCheckColletJam = FALSE;
	}
}

VOID CBondHead::RestoreBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bCheckMissingDie = m_bCheckMissingDie_Backup;
		m_bCheckColletJam = m_bCheckColletJam_Backup;
	}
}

INT CBondHead::OpInitialize()
{
	INT nResult					= gnOK;
	{
		m_qPreviousSubOperation	= -1;
		m_qSubOperation			= 0;

		NeedReset(FALSE);
	}

	DisplaySequence("BH - Operation Initialize");
	return nResult;
}


VOID CBondHead::ReadUpdateBHZ1ColletThresholdValue(BOOL bCheckThresholdLimit)
{
	if (!m_bMS100DigitalAirFlowSnr)
	{
		return;
	}

	LONG bCurrPickVacuumOn = m_bPickVacuumOn;
	if (!bCurrPickVacuumOn)
	{
		SetPickVacuum(TRUE);
		Sleep(500);
	}

	if (ReadBH1AirFlowValue())
	{
		m_ulBHZ1DAirFlowUnBlockValue	= m_ulBHZ1DAirFlowValue;

		//Threshold value for Missing Die
		ULONG ulBHZ1DAirFlowThreshold = GetBHZ1MissingDieThresholdValue(m_ulBHZ1DAirFlowUnBlockValue);
		//Threshold value for CJ
		ULONG ulBHZ1DAirFlowThresholdCJ = GetBHZ1ColletJamThresholdValue(m_ulBHZ1DAirFlowUnBlockValue);

		if (bCheckThresholdLimit)
		{
			if ((ulBHZ1DAirFlowThreshold > m_lBHZ1ThresholdLimit) && (ulBHZ1DAirFlowThreshold < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ1DAirFlowThreshold = ulBHZ1DAirFlowThreshold;
			}

			if ((ulBHZ1DAirFlowThresholdCJ > m_lBHZ1ThresholdLimit) && (ulBHZ1DAirFlowThresholdCJ < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ1DAirFlowThresholdCJ = ulBHZ1DAirFlowThresholdCJ;
			}
		}
		else
		{
			m_ulBHZ1DAirFlowThreshold = ulBHZ1DAirFlowThreshold;
	
			m_ulBHZ1DAirFlowThresholdCJ = ulBHZ1DAirFlowThresholdCJ;
		}
	}

	if (!bCurrPickVacuumOn)
	{
		SetPickVacuum(FALSE);
		Sleep(500);
	}
}


VOID CBondHead::ReadUpdateBHZ2ColletThresholdValue(BOOL bCheckThresholdLimit)
{
	if (!m_bMS100DigitalAirFlowSnr)
	{
		return;
	}

	LONG bCurrPickVacuumOn = m_bPickVacuumOnZ2;
	if (!bCurrPickVacuumOn)
	{
		SetPickVacuumZ2(TRUE);
		Sleep(500);
	}

	if (ReadBH2AirFlowValue())
	{
		m_ulBHZ2DAirFlowUnBlockValue	= m_ulBHZ2DAirFlowValue;

		//Threshold value for Missing Die
		ULONG ulBHZ2DAirFlowThreshold = GetBHZ2MissingDieThresholdValue(m_ulBHZ2DAirFlowUnBlockValue);
		//Threshold value for CJ
		ULONG ulBHZ2DAirFlowThresholdCJ = GetBHZ2ColletJamThresholdValue(m_ulBHZ2DAirFlowUnBlockValue);

		if (bCheckThresholdLimit)
		{
			if ((ulBHZ2DAirFlowThreshold > m_lBHZ1ThresholdLimit) && (ulBHZ2DAirFlowThreshold < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ2DAirFlowThreshold = ulBHZ2DAirFlowThreshold;
			}

			if ((ulBHZ2DAirFlowThresholdCJ > m_lBHZ1ThresholdLimit) && (ulBHZ2DAirFlowThresholdCJ < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ2DAirFlowThresholdCJ = ulBHZ2DAirFlowThresholdCJ;
			}
		}
		else
		{
			m_ulBHZ2DAirFlowThreshold = ulBHZ2DAirFlowThreshold;
	
			m_ulBHZ2DAirFlowThresholdCJ = ulBHZ2DAirFlowThresholdCJ;
		}
	}

	if (!bCurrPickVacuumOn)
	{
		SetPickVacuumZ2(FALSE);
		Sleep(500);
	}
}


INT CBondHead::SetProcessBlockList()
{
	CHAR szVacDestination[GMP_PORT_NAME_CHAR_LEN];
	CHAR szWeakBlowDestination[GMP_PORT_NAME_CHAR_LEN];
	GMP_U32 uiVacMask = 0, uiWeakBlowMask = 0;

	memset(szVacDestination, 0x00, GMP_PORT_NAME_CHAR_LEN);
	memset(szWeakBlowDestination, 0x00, GMP_PORT_NAME_CHAR_LEN);

	//Turn Off the Collet Vacuum of Bondhead Z1 and turn on weak blow at bond position
	//unsigned short usNextBlk = GMP_END_OF_PROCESS;
	LONG nTriggerTime = -COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL * 8;
	if (m_bNeutralVacState && (m_lNeutralDelay > 0))
	{
		BOOL bRet1 = TRUE;//MotionGetOutputBitInfo(BH_SO_VACUUM, szVacDestination, uiVacMask);
		BOOL bRet2 = TRUE;//MotionGetOutputBitInfo(BH_SO_STRONG_BLOW, szWeakBlowDestination, uiWeakBlowMask);

		if (bRet1 && bRet2)
		{
			SetProcessProf(&m_stBHAxis_Z, COLLET_VAC_OFF_PROCESS_BLK, COLLET_VAC_OFF_PROCESS_BLK, WEAK_BLOW_ON_PROCESS_BLK, nTriggerTime, szVacDestination, uiVacMask, uiVacMask);
			nTriggerTime++;
			SetProcessProf(&m_stBHAxis_Z, WEAK_BLOW_ON_PROCESS_BLK, WEAK_BLOW_ON_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szWeakBlowDestination, 0, uiWeakBlowMask);
		}
		else if (bRet1)
		{
			SetProcessProf(&m_stBHAxis_Z, COLLET_VAC_OFF_PROCESS_BLK, COLLET_VAC_OFF_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szVacDestination, uiVacMask, uiVacMask);
		}
	}
	else
	{
		if (MotionGetOutputBitInfo(BH_SO_VACUUM, szVacDestination, uiVacMask))
		{
			SetProcessProf(&m_stBHAxis_Z, COLLET_VAC_OFF_PROCESS_BLK, COLLET_VAC_OFF_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szVacDestination, uiVacMask, uiVacMask);
		}
	}

	//Move Z Up
	nTriggerTime = COLLET_VAC_ON_TIME_AFTER_Z_TO_SWING_LEVEL * 8;
	if (MotionGetOutputBitInfo(BH_SO_VACUUM, szVacDestination, uiVacMask))
	{
		SetProcessProf(&m_stBHAxis_Z, COLLET_VAC_ON_PROCESS_BLK, COLLET_VAC_ON_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szVacDestination, 0, uiVacMask);
	}

	unsigned short SaveIDList[2];
	SaveIDList[0] = COLLET_VAC_OFF_PROCESS_BLK;
	SetProcessList(&m_stBHAxis_Z, COLLET_VAC_OFF_PROCESS_BLK, 1, SaveIDList);
	SaveIDList[0] = COLLET_VAC_ON_PROCESS_BLK;
	SetProcessList(&m_stBHAxis_Z, COLLET_VAC_ON_PROCESS_BLK, 1, SaveIDList);

	//Turn Off the Collet Vacuum of Bondhead Z2  and turn on weak blow at bond position
	//usNextBlk = GMP_END_OF_PROCESS;
	nTriggerTime = -COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL * 8;
	if (m_bNeutralVacState && (m_lNeutralDelay > 0))
	{
		BOOL bRet1 = TRUE;//MotionGetOutputBitInfo(BH_SO_VACUUM_Z2, szVacDestination, uiVacMask);
		BOOL bRet2 = TRUE;//MotionGetOutputBitInfo(BH_SO_STRONG_BLOW_Z2, szWeakBlowDestination, uiWeakBlowMask);

		if (bRet1 && bRet2)
		{
			SetProcessProf(&m_stBHAxis_Z2, COLLET_VAC_OFF_PROCESS_BLK, COLLET_VAC_OFF_PROCESS_BLK, WEAK_BLOW_ON_PROCESS_BLK, nTriggerTime, szVacDestination, uiVacMask, uiVacMask);
			nTriggerTime++;
			SetProcessProf(&m_stBHAxis_Z2, WEAK_BLOW_ON_PROCESS_BLK, WEAK_BLOW_ON_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szWeakBlowDestination, 0, uiWeakBlowMask);
		}
		else if (bRet1)
		{
			SetProcessProf(&m_stBHAxis_Z2, COLLET_VAC_OFF_PROCESS_BLK, COLLET_VAC_OFF_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szVacDestination, uiVacMask, uiVacMask);
		}
	}
	else
	{
		if (MotionGetOutputBitInfo(BH_SO_VACUUM_Z2, szVacDestination, uiVacMask))
		{
			SetProcessProf(&m_stBHAxis_Z2, COLLET_VAC_OFF_PROCESS_BLK, COLLET_VAC_OFF_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szVacDestination, uiVacMask, uiVacMask);
		}
	}
	
	//Move Z Up
	nTriggerTime = COLLET_VAC_ON_TIME_AFTER_Z_TO_SWING_LEVEL * 8;
	if (MotionGetOutputBitInfo(BH_SO_VACUUM_Z2, szVacDestination, uiVacMask))
	{
		SetProcessProf(&m_stBHAxis_Z2, COLLET_VAC_ON_PROCESS_BLK, COLLET_VAC_ON_PROCESS_BLK, GMP_END_OF_PROCESS, nTriggerTime, szVacDestination, 0, uiVacMask);
	}

	SaveIDList[0] = COLLET_VAC_OFF_PROCESS_BLK;
	SetProcessList(&m_stBHAxis_Z2, COLLET_VAC_OFF_PROCESS_BLK, 1, SaveIDList);
	SaveIDList[0] = COLLET_VAC_ON_PROCESS_BLK;
	SetProcessList(&m_stBHAxis_Z2, COLLET_VAC_ON_PROCESS_BLK, 1, SaveIDList);
	return 0;
}


INT CBondHead::OpPreStart()	// HuaMao 2 not stop cycle, to house keeping firstly.
{
	INT nResult				= gnOK;
	BOOL bMotionReady;
	CString szCurrentAlgorithm, szCurrentPath;

	m_dStartTime = GetTime();

	//SetProcessBlockList();	//andrewng //2020-0526

	/*
	//BHZ1
	CString szMess;
	CMSLogFileUtility::Instance()->MissingDieThreshold_LogOpen(FALSE);
	szMess.Format("Start,BHZ1,%d\n",  m_ulColletCount);
	CMSLogFileUtility::Instance()->MissingDieThreshold_Log(FALSE, szMess);	

	//BHZ2
	CMSLogFileUtility::Instance()->MissingDieThreshold_LogOpen(TRUE);
	szMess.Format("Start,BHZ2,%d\n",  m_ulCollet2Count);
	CMSLogFileUtility::Instance()->MissingDieThreshold_Log(TRUE, szMess);

	if (m_pBHZ1AirFlowCompensation != NULL)
	{
		m_pBHZ1AirFlowCompensation->Init();
		m_pBHZ1AirFlowCompensation->SetAirFlowPara(m_dBHZ1ThresholdPercent, m_dBHZ2ThresholdPercent);
	}
	if (m_pBHZ2AirFlowCompensation != NULL)
	{
		m_pBHZ2AirFlowCompensation->Init();
		m_pBHZ2AirFlowCompensation->SetAirFlowPara(m_dBHZ1ThresholdPercent, m_dBHZ2ThresholdPercent);
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Semitek")
	{
		m_bCheckCoverSensor = TRUE;
	}
	*/

	(*m_psmfSRam)["WaferTable"]["AtSafePos"]		= 0;			//v3.67T3

	if ((m_fHardware == FALSE) || m_bDisableBH)
	{
		m_bHome_T = TRUE;
		m_bHome_Z = TRUE;
		m_bHome_Ej = TRUE;
		m_bComm_T = TRUE;
		m_bComm_Z = TRUE;
		m_bComm_Ej = TRUE;
	}

	bMotionReady = IsMotionHardwareReady();
	
	if (bMotionReady == TRUE)
	{
		m_qSubOperation	= WAIT_WT_Q;
		DisplaySequence("BH - prestart ==> Wait WT");

		if ( !OpMoveEjectorTableXY(TRUE))		//v4.42T15
		{
			SetErrorMessage("Ejector XY module is off power");
			SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
			m_qSubOperation = HOUSE_KEEPING_Q;
		}
	}
	else
	{
		DisplaySequence("BH - prestart hw not ready ==> house keeping");
		m_qSubOperation	= HOUSE_KEEPING_Q;
	}

	//Check Pusher
	if (IsEnaAutoChangeCollet() && !IsColletPusherAtSafePos())
	{
		SetErrorMessage("IsBondArmAbleToMove: Pusher is not at UP position");
		SetAlert_Red_Yellow(HMB_BH_CHANGE_COLLET_PUSHER_NOT_AT_UP_POSITION) ;
		m_qSubOperation = HOUSE_KEEPING_Q;
	}

	if (m_bAutoCleanCollet && !IsCleanColletSafetySensor())
	{
		SetErrorMessage("Clean Collet Not Safety");
		SetAlert_Red_Yellow(HMB_BH_NOT_CLEAN_COLLET_SAFETY) ;
		m_qSubOperation = HOUSE_KEEPING_Q;
	}

	Z_Profile(NORMAL_PROF);
	T_Profile(NORMAL_PROF);
	Ej_Profile(NORMAL_PROF);

	//Update profile for BH Theta
	T_UpdatePrePickProfile(abs(m_lPrePickPos_T - m_lPickPos_T));	//v3.65
	LogProfileTime();

	if (IsEnableILC() == TRUE)
	{
		if (ILC_UpdateContourMoveProfile() == FALSE)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart ILC ==> house keeping");
		}
	}

	CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if (pUtl->GetPrescanRegionMode())
	{
		if (pSRInfo->IsRegionEnding())
		{
			m_ulEjCountForSubRegionKOffset = 0;
			m_ulEjCountForSubRegionSKOffset = 0;
			m_lEjSubRegionSmartValue = 0;
			pSRInfo->SetRegionEnding(FALSE);
			SaveScanTimeEvent("BH: REGION END reset k offset prestart");
		}
	}

	m_BADataLog.bHasData = FALSE;
	m_BADataLog.bStartILCUpdate = FALSE;
	m_BADataLog.bEnable = FALSE;
	//m_BAPTBDataLog.bHasData = FALSE;
	//m_BAPTBDataLog.bStartILCUpdate = FALSE;
	//m_BABTPDataLog.bHasData = FALSE;
	//m_BABTPDataLog.bStartILCUpdate = FALSE;

	if (bMotionReady == TRUE)
	{
		if (m_bMS60EjElevator)		//v4.51A6
		{
			if (Z_MoveTo(m_lSwingLevel_Z, 1, TRUE) != gnOK)
			{
				m_qSubOperation	= HOUSE_KEEPING_Q;
				DisplaySequence("BH - prestart z move to ==> house keeping");
			}
		}
		else
		{
			if (Z_MoveTo(m_lSwingLevel_Z) != gnOK)
			{
				m_qSubOperation	= HOUSE_KEEPING_Q;
				DisplaySequence("BH - prestart z move to ==> house keeping");
			}
		}

		CString szMsg;
		szMsg.Format("BH - EJ Pin MoveTo to %d", m_lStandbyLevel_Ej);
		DisplaySequence(szMsg);
		if (Ej_MoveTo(m_lStandbyLevel_Ej) != gnOK)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart ejt move to ==> house keeping");
		}

		szMsg.Format("BH - BIN EJ Pin MoveTo to %d", m_lStandby_Bin_EJ_Pin);
		DisplaySequence(szMsg);
		if (BinEj_MoveTo(m_lStandby_Bin_EJ_Pin) != gnOK)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart BIN ejt move to ==> house keeping");
		}

		//andrewng //2020-0601
		szMsg.Format("BH - BIN EJ Cap MoveTo to %d", m_lStandby_Bin_EJ_Cap);
		DisplaySequence(szMsg);
		if (BinEjCap_MoveTo(m_lStandby_Bin_EJ_Cap) != gnOK)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart BIN ejt CAP move to ==> house keeping");
		}

		if (m_bMS60EjElevator)	
		{
			szMsg.Format("BH - EJ Elevator MoveTo to %d", m_lUpLevel_EJ_Elevator);
			DisplaySequence(szMsg);
			if (EjElevator_MoveTo(m_lUpLevel_EJ_Elevator) != gnOK)
			{
				HmiMessage_Red_Yellow("EJ Table Move fail to UP Position");
				m_qSubOperation	= HOUSE_KEEPING_Q;
				DisplaySequence("BH - prestart EjElv move to ==> house keeping");
			}

			szMsg.Format("BH - Bin EJ Elevator MoveTo to %d", m_lUpLevel_Bin_EJ_Elevator);
			DisplaySequence(szMsg);
			if (BinEjElevator_MoveTo(m_lUpLevel_Bin_EJ_Elevator) != gnOK)
			{
				HmiMessage_Red_Yellow("BIN EJ Table Move fail to UP Position");
				m_qSubOperation	= HOUSE_KEEPING_Q;
				DisplaySequence("BH - prestart BIN EjElv move to ==> house keeping");
			}
		}
	}

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	/*ULONG lRTUpdateMissingDie = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD);
	if (lRTUpdateMissingDie == 2)
	{
		ReadUpdateBHZ1ColletThresholdValue(TRUE);
		ReadUpdateBHZ2ColletThresholdValue(TRUE);
	}*/

	m_lBondDieCount = 0;
	SetSearchMarkDone(TRUE);
	SetEjectorReady(FALSE);
	SetBhTReady(FALSE);
	SetDiePicked(FALSE);			
	SetDieBonded(FALSE);			
	SetDieBondedForWT(FALSE);			
	SetMoveBack(FALSE);
	SetBhToPrePick(FALSE);
	SetBTReSrchDie(FALSE);				//v4.xx
	SetBPRPostBondDone(TRUE);			//v4.40T6
	SetBTCompensate(FALSE);				//v4.52A16
	SetBhReadyForWPRHwTrigger(FALSE);
	m_nPBDoneWaitCount = 0;

	CMS896AStn::m_bBhInBondState	= FALSE;			//v2.56
	CMS896AStn::m_bBhAutoCleanCollet = FALSE;

	if (m_lAutobondTimeInMin > 0)		//v2.83T27
	{
		m_lAutobondStartTime = (LONG)GetTime();    // in milli-second
	}	

	//SetComplete(FALSE);
	m_lMDTotalCount1	= (LONG)(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"];
	m_lMDTotalCount2	= (LONG)(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"];
	m_lCJTotalCount1	= (LONG)(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"];
	m_lCJTotalCount2	= (LONG)(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"];
	m_lMDCycleCount1	= 0;
	m_lMDCycleCount2	= 0;
	m_lMD_Count			= m_lMissingDie_Retry;		// Init the local count for missing die 
	m_lMD_Count2		= m_lMissingDie_Retry;		//v3.83	//MS100 only
	//v4.22T1		//HMI MD Retry sensor display!!
	m_lMDRetry1Counter	= 0;	// prestart
	m_lMDResetCounter	= 0;	// prestart
	m_bMDRetryOn		= FALSE;
	m_lMDRetry2Counter	= 0;	// prestart
	m_lMD2ResetCounter	= 0;	// prestart
	m_bMD2RetryOn		= FALSE;
	m_lNOMDCount1		= 0;
	m_lNOMDCount2		= 0;
	m_lCJRetryCounter	= 0;
	m_lCJResetCounter	= 0;
	m_bCJRetryOn		= FALSE;
	m_lCJ2RetryCounter	= 0;
	m_lCJ2ResetCounter	= 0;
	m_bCJ2RetryOn		= FALSE;
	m_bTesterTimeOut	= FALSE;
	m_lCJContinueCounter1		= 0;
	m_lCJContinueCounter2		= 0;
	m_lPr2DCodeCheckCounter	= 0;			//v4.41T3
	m_lSortTo2ndPartCounter	= 0;			//v4.59A8	//MS90 only 
	m_lCurrOffsetZ1byPBEmptyCheck	= 0;	//v4.48A8
	m_lCurrOffsetZ2byPBEmptyCheck	= 0;	//v4.48A8
	CMS896AStn::m_ulBH1UplookPrFailCount	= 0;	//v4.59A7	
	CMS896AStn::m_ulBH2UplookPrFailCount	= 0;	//v4.59A7	
	CMS896AStn::m_lABDummyCounter = 0;				//andrewng //2020-0727

	m_bSetEjectReadyDone = FALSE;
#ifdef NU_MOTION	//	V450X16	prestart, init pick and bond grade
	if( !IsESMachine() && m_bIsArm2Exist && (m_lDBHBondHeadMode==0))
	{
		m_ucAtPickDieGrade	= 0;	//v4.50A9
		m_ulAtPickDieRow = m_ulAtPickDieCol = 99999;
		m_lAtBondDieGrade = 0;
		m_lAtBondDieRow = m_lAtBondDieCol = -1;
	}
#endif

	//v4.50A4	//Sanan	//For PB EMPTY limit
	m_bAutoCleanColletNow1		= FALSE;		
	m_bAutoCleanColletNow2		= FALSE;		
	m_bEnableAutoCCNowCounter1	= FALSE;
	m_bEnableAutoCCNowCounter2	= FALSE;
	m_lAutoCCNowCounter			= 0;

	//v4.48A10
	m_dRmsRSum = m_dRmsSSum = m_dRmsTSum = 0;
	//m_nRmsCount	= 0;
	//m_dID = m_dIS = m_dIQ = 0;
	//OpBurnInPreStartBHTThermalValues();	//v4.49A2

	(*m_psmfSRam)["BondHead"]["BondDelay"] = m_lBondDelay;
	(*m_psmfSRam)["BondHead"]["WM_X"] = -1;
	(*m_psmfSRam)["BondHead"]["WM_Y"] = -1;
	(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
	CMS896AStn::m_lBondHeadAtSafePos = 0;
	(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
	(*m_psmfSRam)["BondHead"]["MissingDie"] = FALSE;
	(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"]	= FALSE;	//v4.47T16
	(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"]= FALSE;	//v4.47T17	//v4.47A3
	(*m_psmfSRam)["BinLoaderStn"]["GripperJam"]				= FALSE;	//v4.45T2	//Cree HuiZhou MS899
	(*m_psmfSRam)["BinTable"]["BinChangedForBH"]			= TRUE;		//v4.49A11		//Used for GelPad Config only
	(*m_psmfSRam)["BondPr"]["PostBond"]["ResetEmptyLimit1"]	= FALSE;	//v4.50A4
	(*m_psmfSRam)["BondPr"]["PostBond"]["ResetEmptyLimit2"] = FALSE;	//v4.50A4

	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_Pixel"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_Pixel"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_Pixel"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_Pixel"] = 0;

	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_um"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_um"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_um"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_um"] = 0;

	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] = 1;  //0 -- exceed the out of range 2 -- in range
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] = 1;  //0 -- exceed the out of range 2 -- in range

	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX1_um"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY1_um"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleX2_um"] = 0;
	(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjtColletHoleY2_um"] = 0;

	m_lBH1MarkCount = -1;
	m_lBH2MarkCount = -1;
	m_lBPRErrorToCleanColletCount = 0;
	m_bBPRErrorToCleanCollet	  = FALSE;
	//m_bSearchColletNow1			  = FALSE;
	//m_bSearchColletNow2			  = FALSE;

	m_dCycleTime = 0.0;

	m_dAvgCycleTime = 0.0;
	m_ulCycleCount = 0;
	m_dLastTime = -1;
	//Need to reset current wafer die counter if Pick & place or BurnIn
	m_WaferMapWrapper.GetAlgorithm(szCurrentAlgorithm, szCurrentPath);
	if ((szCurrentAlgorithm == "Pick and Place: TLH-Circle") || (szCurrentAlgorithm == "Pick and Place: BLH-Circle") || 
		(szCurrentAlgorithm == "Pick and Place: TLV-Circle") || (szCurrentAlgorithm == "Pick and Place: TRV-Circle"))
	{
		ResetCurrWaferCounter();
	}

	//andrew
	//air-flow sensor test
	m_bStartAirFlowMdCheck	= FALSE;
	m_bStartAirFlowJcCheck	= FALSE;
	m_lAirFlowMdTime		= 0;
	m_lAirFlowJcTime		= 0;
	m_lAirFlowStartTime		= 0;
	m_lTMoveTime			= 0;
	m_lTTravelTime			= 0;
	m_bPostBondAtBondCount	= 0;		//v2.61
	m_lEMOCheckCounter		= 0;		//v3.91		//MS100 only
	m_lMS60TempCheckCounter	= 0;		//v4.50A6
	m_bIsEMOTriggered		= FALSE;	//v4.24
	m_dEjDnStartTime		= 0;		//v4.22T1
	m_dBinEjDnStartTime		= 0;
	m_dBHZPickStartTime		= 0;		//shiraishi03
	m_bMS100TurnOnVac		= FALSE;	//v4.42T15
#ifndef NU_MOTION
	m_lPreBondAtPickCount	= 1;		//v3.34
#else
	m_lPreBondAtPickCount	= 0;
#endif

	//v3.66		//DBH only
	SetWTReady(FALSE,"20");			
	SetWTStartMove(FALSE);		
	m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
	(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) FALSE;
	m_lPrevDieSPCResult = -1;
	m_bSPCAlarmTwice	= FALSE;		//v4.51A16	//SanAn XM/XA
	m_bSPCAlarmTwiceZ2	= FALSE;		//v4.52A7	//Semitek
	m_nBHAlarmCode		= 0;	
	m_lBHStateCode		= 0;
	m_nBHLastAlarmCode	= 0;
	m_bPickDownFirst	= FALSE;	
	m_nBHZ1BHZ2Delay	= 0;
	m_bCheckPickZ2DownFirst = TRUE;		//v3.66T1
	m_bBHZ1HasDie		= FALSE;
	m_bBHZ2HasDie		= FALSE;
	m_bDBHStop			= FALSE;		//v3.67T2		//For MS100 only
	m_ulLastMapRow		= 0;			//For MS100 only
	m_ulLastMapCol		= 0;			//For MS100 only
	m_ucLastMapGrade	= 0;			//For MS100 only
	m_bLastMapChecked	= FALSE;
	m_bIsES101NGPickStarted	= FALSE;	//v4.24T10		//ES101 only
	m_bIs2DCodeDone		= TRUE;			//v4.40T1		//PLLM MS109 with 2 BH postseal cameras
	m_lSPCResult		= 1;			//v4.52A2
	m_bUplookResultFail	= FALSE;		//v4.57A11
	m_lUpLookPrStage	= 0;
	m_bMS90UplookPrDo1MoreCycle = FALSE;

	m_bAutoLearnPickLevelZ1	= FALSE;
	m_bAutoLearnPickLevelZ2 = FALSE;

	m_lAPLastDone				= 0;
	m_bFirstCycle				= TRUE;
	m_bFirstCycleAlreadyTakeLog	= FALSE;
	(*m_psmfSRam)["BondHead"]["FirstCycleTakeLog"] = m_bFirstCycleAlreadyTakeLog;

	m_bChangeGrade			= FALSE;	//v4.44A3
	m_bMoveFromPrePick		= FALSE;	//v3.79
	m_dWaitBTStartTime		= 0;

	m_nILCAutoUpdateAccErrCount	= 0;

	m_bCycleFirstProbe		= TRUE;

	(*m_psmfSRam)["WaferTable"]["PRDelay"]		= m_lPRDelay;
	(*m_psmfSRam)["WaferTable"]["VacOffDelay"]	= m_lEjVacOffDelay;			//v3.78
	(*m_psmfSRam)["BinTable"]["BTDelay"]		= m_lBinTableDelay;

#ifdef NU_MOTION
	switch (m_lDBHBondHeadMode)
	{
	case 2:	//BHZ2 only
		DisplayMessage("MS100 under BHZ2 mode");
		//m_bSel_Z = FALSE;
		break;
	case 1:		//BHZ1 only
		DisplayMessage("MS100 under BHZ1 mode");
		//m_bSel_Z2 = FALSE;
		break;
	case 0:		//DBH
	default:
		DisplayMessage("MS100 under DBH mode");
		//m_bSel_Z	= TRUE;
		//m_bSel_Z2	= TRUE;
		break;
	}
#else
	m_lDBHBondHeadMode = 0;		//default for MS899/810
#endif

	//xx check pick/bond level difference.	prestart
	CheckZ1PickBondLevelOffset();
	CheckZ2PickBondLevelOffset();

	// pass the position and moved state to bh for bond level offset calculation. after bh get, reset it.
	(*m_psmfSRam)["BinTable"]["Bond Posn X"]	= 0;
	(*m_psmfSRam)["BinTable"]["Bond Posn Y"]	= 0;
	(*m_psmfSRam)["BinTable"]["Bond Moved"]		= FALSE;

	CMSLogFileUtility::Instance()->BH_BackupDieCounterLog();
	CMSLogFileUtility::Instance()->BH_BackupLogStatus();				
	CMSLogFileUtility::Instance()->BH_BackupILCEventLog();		//v4.52A13				

	//v4.52A11
	m_lEjectorKOffsetForBHZ1MDCount = 0;
	m_lEjectorKOffsetForBHZ2MDCount = 0;

	if (m_bMS100DigitalAirFlowSnr && m_bEnableDAFlowRunTimeCheck)	//v4.52A11	//M75	//v4.53A1
	{
		OpPreStartInitDAirFlowThresold();
		m_bDetectDAirFlowAtPrePick	= FALSE;
		m_nDAirFlowBHZ1ThresFailCount	= 0;	//v4.52A18
		m_nDAirFlowBHZ2ThresFailCount	= 0;	//v4.52A18

		CMSLogFileUtility::Instance()->BH_Z1LogOpen();
		CMSLogFileUtility::Instance()->BH_Z2LogOpen();
		BH_Z_AirFlowLog(TRUE, "BHZ1,curr_Unclogged,Clogged,curr_Thres,curr_ThresCJ,Limit,LimitCJ");
		BH_Z_AirFlowLog(FALSE, "BHZ2,curr_Unclogged,Clogged,curr_Thres,curr_ThresCJ,Limit,LimitCJ");
	}

	if (LoopCheckCoverOpen("BH Prestart") == FALSE)
	{
		m_qSubOperation	= HOUSE_KEEPING_Q;
		DisplaySequence("BH - prestart cover open ==> house keeping");
	}
	if (IsEnableColletHole() && !IsPrescanning())
	{
		if (OpPrestartColletHoleCheck(TRUE) == FALSE)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart check BH1 collet Jam ==> house keeping");
			return gnNOTOK;
		}
		if (OpPrestartColletHoleCheck(FALSE) == FALSE)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart check BH2 collet Jam ==> house keeping");
			return gnNOTOK;
		}
		if (OpPrestartBPRColletHoleCheck(TRUE) == FALSE)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart BPR Collet Hole1 Check Fails ==> house keeping");
			return gnNOTOK;
		}
		if (OpPrestartBPRColletHoleCheck(FALSE) == FALSE)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			DisplaySequence("BH - prestart BPR Collet Hole2 Check Fails ==> house keeping");
			return gnNOTOK;
		}
	}

	CMSLogFileUtility::Instance()->LogCycleStartStatus("BH: PreStart Done\n");	//v4.52A11
	DisplaySequence("BH - Operation Prestart");
	CString szMsg;
	szMsg.Format("BH - T Pick %d, Bond %d prepick %d, Z1 pick %d, bond %d, Z2 pick %d, bond %d", 
		m_lPickPos_T, m_lBondPos_T, m_lPrePickPos_T, m_lPickLevel_Z, m_lBondLevel_Z, m_lPickLevel_Z2, m_lBondLevel_Z2);
	DisplaySequence(szMsg);
	return nResult;
}


INT CBondHead::OpBadDie(BOOL bWait)		// Handle Bad Die Situation
{
	if (m_bComplete_T == FALSE)
	{
		T_Sync();	// Wait T complete
		m_bComplete_T = TRUE;	//v4.43T1

#ifdef NU_MOTION	//v4.16T5
		if (IsBHZ1ToPick())	//If BHZ1
		{
			TakeTime(T4);
		}
		else
		{
			TakeTime(T2);
		}
#else
		TakeTime(T2);
#endif
	}

	
	// Move T to Prepick
#ifdef NU_MOTION
	if (OpIsBinTableLevelAtDnLevel())	//v4.44T1
	{
		T_MoveTo(m_lPrePickPos_T, bWait);
	}
#else
	T_MoveTo(m_lPrePickPos_T, bWait);
#endif

//	V450X16	SetEjectorReady(TRUE, "00");
	SetWTReady(FALSE,"21");
	
	(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
	return gnOK;
}

INT CBondHead::OpColletJam()	// Handle Collet Jam Situation
{
	INT nResult	= gnOK;

	//SetStatusMessage("Collet Jam");
	if (m_bComplete_T == FALSE)
	{
		T_Sync();	// Wait T complete
		TakeTime(T2);
	}

	//v4.48A10
	Z_MoveTo(m_lSwingLevel_Z,	SFM_WAIT);
#ifdef NU_MOTION
	Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
#endif

	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF);		// Change T profile
	T_SafeMoveToInAuto(m_lCleanColletPos_T, "Op Jam 1");		// Move to clean collet position

	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	//v3.66	
	{
		SetPickVacuum(FALSE);
		Sleep(500);							//Extra 500ms NEUTRAL state before StrongBlow	//v4.17T7
		SetStrongBlow(TRUE);
		Sleep(m_lHighBlowTime);				// Cleaning 
		SetStrongBlow(FALSE);
		Sleep(1000);	
		SetPickVacuum(TRUE);
	}
	else
	{
		SetPickVacuumZ2(FALSE);
		Sleep(500);							//Extra 500ms NEUTRAL state before StrongBlow	//v4.17T7
		SetStrongBlowZ2(TRUE);
		Sleep(m_lHighBlowTime);				// Cleaning 
		SetStrongBlowZ2(FALSE);
		Sleep(1000);	
		SetPickVacuumZ2(TRUE);
	}


#ifdef NU_MOTION
	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	//v4.48A10
	{
		T_SafeMoveToInAuto(m_lPickPos_T, "Op Jam 2");				// Move back to Pick position
	}
	else
	{
		T_SafeMoveToInAuto(m_lBondPos_T, "Op Jam 2");				// Move back to BOND position
	}
#else
	T_SafeMoveToInAuto(m_lPrePickPos_T, "Op Jam 2");
	T_Profile(NORMAL_PROF);					// Change T back to normal prof
	BA_MoveTo(m_lPickPos_T, SFM_WAIT);
#endif

	Sleep(BH_MIN_CHECK_COLLET_TIME);	
	return nResult;
}

INT CBondHead::OpReplaceDie()			// Handle Replace Die Situation
{
	INT nResult	= gnOK;

	//SetStatusMessage("Replace Die");

	// Wait T complete
	if (m_bComplete_T == FALSE)
	{
		T_Sync();
	}
	// Wait Z complete
	if (m_bComplete_Z == FALSE)
	{
		Z_Sync();
	}
	// Wait Ej complete
	if (m_bComplete_Ej == FALSE)
	{
		Ej_Sync();
	}	
	Ej_MoveTo(m_lStandbyLevel_Ej);		// Move Ej to Standby level
	T_MoveTo(m_lPickPos_T);				// Move T to Pick
	Z_MoveTo(m_lReplaceLevel_Z);		// Move Z to Replace level
	SetPickVacuum(FALSE);				// Replace Die
	Sleep(m_lBondDelay);				// Wait Bond Delay
	Z_MoveTo(m_lSwingLevel_Z);			// For safety, move Z to Swing
	T_MoveTo(m_lPrePickPos_T);			// For safety, move T to PrePick

	return nResult;
}


BOOL CBondHead::OpCheckMissingDie()		// Check whether missing die occurs
{
	CString szTemp;

#ifdef NU_MOTION
	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	//v3.66
	{
		if (m_bCheckMissingDie && m_bMS100DigitalAirFlowSnr)	//v4.43T11
		{
			OpCalKOffset_BH2();
		}

		// Check whether the checking is enabled and is missing die (Pick Vacuum should be ON)
		if ((m_bCheckMissingDie == TRUE)	&& 
			(m_bPickVacuumOnZ2 == TRUE)		&&		//v4.54A12
			(IsMissingDieZ2() == TRUE))
		{
			Sleep(1);
			if (IsMissingDieZ2() == FALSE)
			{
				TakeTime(MDO);
				m_bStartAirFlowMdCheck	= FALSE;
				return FALSE;
			}

			m_lMDRetry2Counter++;	//	missing die on BH2
			m_lMD2ResetCounter = 0;	//	missing die on BH2
			if (m_lMDRetry2Counter > 3)
			{
				m_bMD2RetryOn = TRUE;
			}

			for (INT i = 0; i < 500; i++)
			{
				Sleep(1);
				
				if (IsMissingDieZ2() == FALSE)
				{
					TakeTime(MDO);
					szTemp.Format("Warning: Missing Die BHZ2 With Retry by %d ms", (i + 1));
					CMSLogFileUtility::Instance()->BH_LogStatus(szTemp);	//v4.41T6
					m_bStartAirFlowMdCheck = FALSE;
					return FALSE;
				}
			}

			if (m_bEnableAirFlowTest)
			{
				Sleep(m_lAirFlowTestTimeOut);
			}
			m_bStartAirFlowMdCheck	= FALSE;	//andrew
			szTemp = "BH - Missing Die Detected on BHZ2";
			SetErrorMessage(szTemp);
			DisplaySequence(szTemp);
			return TRUE;
		}
		else
		{
			m_lMD2ResetCounter++;	//	BH2 non missing die
			if (m_lMD2ResetCounter > 20 && m_lMDRetry2Counter!=0 )	//50)	//v4.42T7	//LeoLam
			{
				//TUrn off MD display if 50 consecutive MD sccuess is counted;
				m_lMDRetry2Counter	= 0;
				m_bMD2RetryOn		= FALSE;
			}

			if( m_lMD2ResetCounter>=200 && m_lMDCycleCount2>0 )
			{
				m_lMDCycleCount2--;
				m_lMD2ResetCounter = 0;
			}

			//TakeTime(MDO);		//v4.42T16
			m_bStartAirFlowMdCheck	= FALSE;	//andrew
			return FALSE;
		}
	}
	else
#endif
	{
		if (m_bCheckMissingDie && m_bMS100DigitalAirFlowSnr)	//v4.43T11
		{
			OpCalKOffset_BH1();
		}

//v4.48A33
szTemp.Format("BH1 Check MD: Chk=%d, VacOn=%d", m_bCheckMissingDie, m_bPickVacuumOn);
//CMSLogFileUtility::Instance()->BH_LogStatus(szTemp);

		// Check whether the checking is enabled and is missing die (Pick Vacuum should be ON)
		if ((m_bCheckMissingDie == TRUE) && (m_bPickVacuumOn == TRUE) && (IsMissingDie() == TRUE))
		{
			Sleep(1);
			if (IsMissingDie() == FALSE)
			{
				TakeTime(MDO);
				m_bStartAirFlowMdCheck	= FALSE;
				return FALSE;
			}

			//LogBondHeadStatus("Missing Die With Retry");
			CMSLogFileUtility::Instance()->BH_LogStatus("Missing Die With Retry");
			m_lMDRetry1Counter++;	//	missing die on BH1
			m_lMDResetCounter = 0;	//	missing die on BH1
			if (m_lMDRetry1Counter > 3)
			{
				m_bMDRetryOn = TRUE;
			}

#ifdef NU_MOTION
			for (INT i = 0; i < 500; i++)
			{
				Sleep(1);
#else
			for (INT i = 0; i < 10; i++)
			{
				Sleep(50);
#endif
				if (IsMissingDie() == FALSE)
				{
					TakeTime(MDO);
					//CString szTemp;
#ifdef NU_MOTION
					szTemp.Format("Warning: Missing Die BHZ1 With Retry by %d ms", (i + 1));
#else
					szTemp.Format("Warning: Missing Die BHZ1 With Retry by %d ms", (i + 1) * 5);
#endif
					//SetErrorMessage(szTemp);
					CMSLogFileUtility::Instance()->BH_LogStatus(szTemp);	//v4.41T6
					m_bStartAirFlowMdCheck = FALSE;
					return FALSE;
				}
			}

			if (m_bEnableAirFlowTest)
			{
				Sleep(m_lAirFlowTestTimeOut);
			}
			m_bStartAirFlowMdCheck	= FALSE;	//andrew
			szTemp = "BH - Missing Die Detected on BHZ1";
			SetErrorMessage(szTemp);
			DisplaySequence(szTemp);
			return TRUE;
		}
		else
		{
			m_lMDResetCounter++;	//	BH1 non missing die
			if (m_lMDResetCounter > 20 && m_lMDRetry1Counter!=0 )
			{
				//TUrn off MD display if 50 consecutive MD sccuess is counted;
				m_lMDRetry1Counter	= 0;
				m_bMDRetryOn		= FALSE;
			}

			if( m_lMDResetCounter>=200 && m_lMDCycleCount1>0 )
			{
				m_lMDResetCounter	= 0;
				m_lMDCycleCount1--;
			}

			//TakeTime(MDO);		//v4.42T16
			m_bStartAirFlowMdCheck	= FALSE;	//andrew
			return FALSE;
		}
	}
}

BOOL CBondHead::OpCheckMissingDie_PLLM()		// Check whether missing die occurs
{
	CString szTemp;

	if (IsBHZ2ToPick())		//If BHZ2 at PICK
	{
		if (m_bCheckMissingDie && m_bMS100DigitalAirFlowSnr)
		{
			OpCalKOffset_BH2();
		}

		// Check whether the checking is enabled and is missing die (Pick Vacuum should be ON)
		if ((m_bCheckMissingDie == TRUE) && 
			(m_bPickVacuumOnZ2 == TRUE) &&				//v4.54A12
			(IsMissingDieZ2() == TRUE))
		{
			Sleep(1);
			if (IsMissingDieZ2() == FALSE)
			{
				//TakeTime(MDO);
				return FALSE;
			}

			for (INT i = 0; i < 200; i++)
			{
				Sleep(1);
				
				if (IsMissingDieZ2() == FALSE)
				{
					TakeTime(MDO);
					//CString szTemp;
					szTemp.Format("Warning: Missing Die BHZ2 With Retry by %d ms (PLLM)", (i + 1));
					//SetErrorMessage(szTemp);
					CMSLogFileUtility::Instance()->BH_LogStatus(szTemp);
					return FALSE;
				}
			}

			SetErrorMessage("Missing Die Detected on BHZ2 (PLLM)");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if (m_bCheckMissingDie && m_bMS100DigitalAirFlowSnr)
		{
			OpCalKOffset_BH1();
		}

		// Check whether the checking is enabled and is missing die (Pick Vacuum should be ON)
		if ((m_bCheckMissingDie == TRUE) && (m_bPickVacuumOn == TRUE) && (IsMissingDie() == TRUE))
		{
			Sleep(1);
			if (IsMissingDie() == FALSE)
			{
				//TakeTime(MDO);
				return FALSE;
			}

			for (INT i = 0; i < 200; i++)
			{
				Sleep(1);
				if (IsMissingDie() == FALSE)
				{
					//CString szTemp;
					szTemp.Format("Warning: Missing Die BHZ1 With Retry by %d ms (PLLM)", (i + 1));
					//SetErrorMessage(szTemp);
					CMSLogFileUtility::Instance()->BH_LogStatus(szTemp);
					return FALSE;
				}
			}

			SetErrorMessage("Missing Die Detected on BHZ1 (PLLM)");
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL CBondHead::OpCheckMissingDieResponseTime()		//v3.94
{
	BOOL bDieDetected = FALSE;
	CString szMsg;
	INT i = 0;

	if (!m_bEnableMDResponseChecking)
	{
		return TRUE;
	}

#ifdef NU_MOTION
	if (IsBHZ2ToPick())		//If BHZ2 towards PICK	//v3.66
	{
		szMsg = "BHZ2,timeout";
		for (i = 0; i < 2000; i++)
		{
			Sleep(1);
			
			if (IsMissingDieZ2() == FALSE)
			{
				szMsg.Format("BHZ2,%d", i);
				bDieDetected = TRUE;
				break;
			}
		}
	}
	else
#endif
	{
		szMsg = "BHZ1,timeout";
		for (i = 0; i < 2000; i++)
		{
			Sleep(1);
			
			if (IsMissingDie() == FALSE)
			{
				szMsg.Format("BHZ1,%d", i);
				bDieDetected = TRUE;
				break;
			}
		}
	}

	if (!bDieDetected)
	{
		BH_LOG(szMsg);
		return FALSE;
	}

	BH_LOG(szMsg);
	return TRUE;
}


BOOL CBondHead::OpCheckColletJam()		// Check whether collet jam occurs
{
	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	//v3.66	
	{
		// Check whether the checking is enabled and is jam (Pick Vacuum should be ON)
		if ((m_bCheckColletJam == TRUE) && (m_bPickVacuumOn == TRUE) && (IsColletJam() == TRUE))
		{
			Sleep(1);
			if (IsColletJam() == FALSE)
			{
				TakeTime(CJO);		//v4.42T16
				m_bStartAirFlowJcCheck	= FALSE;
				return FALSE;
			}

			CMSLogFileUtility::Instance()->BH_LogStatus("CJ BHZ1 With Retry");
			m_lCJRetryCounter++;
			m_lCJResetCounter = 0;
			if (m_lCJRetryCounter > 3)
			{
				m_bCJRetryOn = TRUE;
			}

			for (INT i = 0; i < 500; i++)
			{
				Sleep(1);

				if (IsColletJam() == FALSE)
				{
					TakeTime(CJO);		//v4.42T16
					CString szTemp;
					szTemp.Format("Warning: Collet Jam on BHZ1 With Retry by %d ms", 5 * (i + 1));
					SetErrorMessage(szTemp);
					m_bStartAirFlowJcCheck	= FALSE;
					return FALSE;	//Still OK ...
				}
			}

			if (m_bEnableAirFlowTest)
			{
				Sleep(m_lAirFlowTestTimeOut);
			}
			m_bStartAirFlowJcCheck	= FALSE;
			SetErrorMessage("Collet BHZ1 is Jammed");
			return TRUE;	//Collet-JAM !!!
		}
		else
		{
			m_lCJResetCounter++;
			if (m_lCJResetCounter > 200)	//change to 200 from 20
			{
				//reset consecutive collet jam count if there has not collet jam happen in consecutive 200 die after clean collet.
				m_lCJRetryCounter	= 0;
				m_lCJResetCounter	= 0;
				m_bCJRetryOn		= FALSE;

				m_lCJContinueCounter1 = 0; //reset
			}

			m_bStartAirFlowJcCheck	= FALSE;
			return FALSE;
		}
	}
	else
	{
		// Check whether the checking is enabled and is jam (Pick Vacuum should be ON)
		if ((m_bCheckColletJam == TRUE) && (m_bPickVacuumOn == TRUE) && (IsColletJamZ2() == TRUE))
		{
			Sleep(1);
			if (IsColletJamZ2() == FALSE)
			{
				TakeTime(CJO);		//v4.42T16
				m_bStartAirFlowJcCheck	= FALSE;
				return FALSE;
			}

			CMSLogFileUtility::Instance()->BH_LogStatus("CJ BHZ2 With Retry");
			m_lCJ2RetryCounter++;
			m_lCJ2ResetCounter = 0;
			if (m_lCJ2RetryCounter > 3)
			{
				m_bCJ2RetryOn = TRUE;
			}

			for (INT i = 0; i < 500; i++)
			{
				Sleep(1);

				if (IsColletJamZ2() == FALSE)
				{
					TakeTime(CJO);		//v4.42T16
					CString szTemp;
					szTemp.Format("Warning: Collet Jam on BHZ2 With Retry by %d ms", 5 * (i + 1));
					SetErrorMessage(szTemp);
					m_bStartAirFlowJcCheck	= FALSE;
					return FALSE;	//Still OK ...
				}
			}

			if (m_bEnableAirFlowTest)
			{
				Sleep(m_lAirFlowTestTimeOut);
			}
			m_bStartAirFlowJcCheck	= FALSE;
			SetErrorMessage("Collet BHZ2 is Jammed");
			return TRUE;	//COllet-Jam !!!
		}
		else
		{
			m_lCJ2ResetCounter++;
			if (m_lCJ2ResetCounter > 200)	//change to 200 from 20
			{
				//reset consecutive collet jam count if there has not collet jam happen in consecutive 200 die after clean collet.
				m_lCJ2RetryCounter	= 0;
				m_lCJ2ResetCounter	= 0;
				m_bCJ2RetryOn		= FALSE;

				m_lCJContinueCounter2 = 0;  //reset
			}

			m_bStartAirFlowJcCheck	= FALSE;	
			return FALSE;	//OK
		}
	}
}

VOID CBondHead::OpUpdateDieIndex()		// Update Die Index
{
}


BOOL CBondHead::IsMaxColletEjector_CheckOnly(CONST BOOL bWaferEnd)		//v4.50A15
{
	BOOL bReturn = FALSE;

	if( IsBurnIn() )
	{
		return FALSE;
	}

	//v4.40T6
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (!bWaferEnd)
	{
		if (pApp->GetCustomerName() == CTM_NICHIA && pApp->GetProductLine()==_T(""))		//v4.59A34
		{
			return FALSE;	//DO not check alarms for Nichia during bonding; only when waferend
		}
	}
	
	if ((m_ulMaxColletCount > 0) && (m_ulColletCount >= m_ulMaxColletCount) && IsBHZ2ToPick())
	{
		bReturn = TRUE;
	} 
	else if ((m_ulMaxCollet2Count > 0) && (m_ulCollet2Count >= m_ulMaxCollet2Count) && IsBHZ1ToPick())	
	{
		bReturn = TRUE;
	} 
	else if ((m_ulMaxEjectorCount > 0) && (m_ulEjectorCount >= m_ulMaxEjectorCount))
	{
		if (m_bEjNeedReplacement || m_bEjPinNeedReplacementAtUnloadWafer)		//v4.41T6	//SanAn
		{
		}
		else
		{
			bReturn = TRUE;
		}
	}
	else if ((m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0) && (m_lEjectorKOffsetTotal >= m_lEjectorKOffsetLimit))
	{
		bReturn = TRUE;
	}

	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
	if (bPLLMSpecialFcn)
	{
		if (m_bEnableCustomCount1 && (m_ulCustomCounter1 > m_ulCustomCounter1Limit))
		{
			bReturn = TRUE;
		}
	}
	return bReturn;
}

BOOL CBondHead::IsMaxColletEjector(CONST BOOL bWaferEnd)
{
	BOOL bReturn = FALSE;

	if( IsBurnIn() )
	{
		return FALSE;
	}

	//v4.40T6
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (!bWaferEnd)
	{
		if (pApp->GetCustomerName() == CTM_NICHIA && pApp->GetProductLine()==_T(""))	//v4.59A34
		{
			return FALSE;	//DO not check alarms for Nichia during bonding; only when waferend
		}
	}
	

	if (IsBHZ1ToPick() && (m_ulMaxColletCount > 0) && (m_ulColletCount >= m_ulMaxColletCount))
	{
		if (IsEnaAutoChangeCollet())	//v4.50A11
		{
			if (!WaitBTReady(1000) || WaitDieBonded(1000))
			{
				SetErrorMessage("BH: BT Timeout, autoLearn Z1 levels fail in IsMaxColletEjector");
				SetStatusMessage("BT Timeout, Collet lifetime is reached (AGC)");
				SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET); //4.51D8 BH0002
				if (m_bEjPinNeedReplacementAtUnloadWafer) // Ignore the Ej pin checking if collet need to replace
				{
					m_bEjPinNeedReplacementAtUnloadWafer = FALSE;
				}
				return TRUE;
			}
			if (OpAutoChangeColletZ1())
			{
				ResetEjectorKCount();
				ResetColletAndGenRecord(FALSE, TRUE);
 				ResetColletRecord();
				SetAutoChangeColletDone(TRUE);
//				m_bChangeColletDone = TRUE;
			}
			else
			{
				if( pApp->GetCustomerName()!="SanAn" )
				{
					ResetEjectorKCount();
					ResetColletCount();
				}
				m_bAutoChgCollet1Fail = TRUE;

				SetErrorMessage("BH: autoLearn Z1 levels fail in IsMaxColletEjector");
				SetStatusMessage("Collet lifetime is reached (AGC)");
				//SetAlert_Red_Yellow(IDS_BH_REPLACE_COLLET);
				SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET); //4.51D8 BH0002
				if (m_bEjPinNeedReplacementAtUnloadWafer) // Ignore the Ej pin checking if collet need to replace
				{
					m_bEjPinNeedReplacementAtUnloadWafer = FALSE;
				}
				bReturn = TRUE;
			}
		}
		else
		{
			//SetAlert_Red_Yellow(IDS_BH_REPLACE_COLLET);
			SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET); //4.51D8 BH0002
			if (m_bEjPinNeedReplacementAtUnloadWafer) // Ignore the Ej pin checking if collet need to replace
			{
				m_bEjPinNeedReplacementAtUnloadWafer = FALSE;
			}
			ResetColletCountSelection(1);
			bReturn = TRUE;
			SetStatusMessage("Collet lifetime is reached");
			SetErrorMessage("Collet lifetime is reached");
		}
	} 
	else if (IsBHZ2ToPick() && (m_ulMaxCollet2Count > 0) && (m_ulCollet2Count >= m_ulMaxCollet2Count))	//v3.92	//MS100
	{
		if (IsEnaAutoChangeCollet())	//v4.50A11
		{
			if (!WaitBTReady(1000) || WaitDieBonded(1000))
			{
				SetErrorMessage("BH: BT Timeout, autoLearn Z2 levels fail in IsMaxColletEjector");
				SetStatusMessage("BT Timeout, Collet lifetime is reached (AGC)");
				SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET); //4.51D8 BH0002
				if (m_bEjPinNeedReplacementAtUnloadWafer) // Ignore the Ej pin checking if collet need to replace
				{
					m_bEjPinNeedReplacementAtUnloadWafer = FALSE;
				}
				return TRUE;
			}

			if (OpAutoChangeColletZ2())
			{
				ResetEjectorKCount();
				ResetColletAndGenRecord(TRUE, TRUE);
				ResetColletRecord();
				SetAutoChangeColletDone(TRUE);
//				m_bChangeColletDone = TRUE;
			}
			else
			{
				if( pApp->GetCustomerName()!="SanAn" )
				{
					ResetEjectorKCount();
					ResetCollet2Count();
				}
				m_bAutoChgCollet2Fail = TRUE;

				//SetAlert_Red_Yellow(IDS_BH_REPLACE_COLLET);
				SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET); //4.51D8  BH0002
				bReturn = TRUE;
				SetStatusMessage("Collet 2 lifetime is reached (AGC)");
				SetErrorMessage("Collet 2 lifetime is reached (AGC)");
			}
		}
		else
		{
			//SetAlert_Red_Yellow(IDS_BH_REPLACE_COLLET2);
			SetAlert_Yellow_Green(IDS_BH_REPLACE_COLLET2); //4.51D8  BH0031
			bReturn = TRUE;
			SetStatusMessage("Collet 2 lifetime is reached");
			SetErrorMessage("Collet 2 lifetime is reached");
		}
	} 
	else if ((m_ulMaxEjectorCount > 0) && (m_ulEjectorCount >= m_ulMaxEjectorCount) ||
		 ((pApp->GetCustomerName() == "ChangeLight(XM)") && (m_ulMaxEjectorCount == 0)) )	//v3.92	//MS100
	{
		if (m_bEjNeedReplacement)		//v4.41T6	//SanAn
		{
		}
		else if (m_ulMaxEjectorCount == 0)
		{
			SetStatusMessage("Ejector lifetime is not input");
			SetErrorMessage("Ejector lifetime is not input");
			HmiMessage("Ejector lifetime is not input");
			//SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR); //4.51D8 BH0003
			bReturn = TRUE;
		}
		else
		{
			//v4.58A1	//Disabled by Andrew
			//4.53D26 Ej uncheck RT WongGuanFeng
			//if (pApp->GetCustomerName() != "Electech3E(DL)")
			//{
			//	SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR); //4.51D8 BH0003
			//	ResetEjectorSelection();
			//	bReturn = TRUE;
			//}

			if (!m_bEjPinNeedReplacementAtUnloadWafer)
			{
				SetStatusMessage("Ejector lifetime is reached");
				SetErrorMessage("Ejector lifetime is reached");
			}

			//v4.39T10	//Silan 9WanYiMing)
			CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
			BOOL bEjReplaceAtWaferEnd = pApp->GetFeatureStatus(MS896A_FUNC_EJ_REAPLCE_AT_WAFEREND);
			if (bEjReplaceAtWaferEnd)
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("Ejector Lifetime replacement At WaferEnd is enabled");
				m_bEjNeedReplacement = TRUE;
				bReturn = FALSE;
			}
			else
			{
				if (!m_bEjPinNeedReplacementAtUnloadWafer)
				{
					m_bEjPinNeedReplacementAtUnloadWafer = TRUE;
					CMSLogFileUtility::Instance()->MS_LogOperation("Ejector Lifetime replacement At Load Wafer Frame");
				}
				//SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR); //4.51D8 BH0003
			//	ResetEjectorSelection();
				bReturn = TRUE;
			}	//	return TRUE if REACH TO limit
		}
	}
	else if ((m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0) && (m_lEjectorKOffsetTotal >= m_lEjectorKOffsetLimit))
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13

		/*
		SetAlert_Red_Yellow(IDS_BH_KOFFSET_HIT_LIMIT);
		bReturn = TRUE;
		SetStatusMessage("Ejector K-offset is reached");
		SetErrorMessage("Ejector K-offset is reached");
		*/

		if(m_lEjectorKOffsetBHZ1Total >= m_lEjectorKOffsetLimit)
		{
			SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ1);
			bReturn = TRUE;
			SetStatusMessage("BH1 Ejector K-offset is reached");
			SetErrorMessage("BH1 Ejector K-offset is reached");
			m_lEjectorKOffsetBHZ1Total = 0;
		}
		else if(m_lEjectorKOffsetBHZ2Total >= m_lEjectorKOffsetLimit)
		{
			SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ2);
			bReturn = TRUE;
			SetStatusMessage("BH2 Ejector K-offset is reached");
			SetErrorMessage("BH2 Ejector K-offset is reached");
			m_lEjectorKOffsetBHZ2Total = 0;
		}
		else
		{
			SetAlert_Red_Yellow(IDS_BH_KOFFSET_HIT_LIMIT);
			bReturn = TRUE;
			SetStatusMessage("Ejector K-offset is reached");
			SetErrorMessage("Ejector K-offset is reached");
		}

		if (m_bResetEjKOffsetAtWaferEnd)	//v4.43T8	//Semitek
		{
			m_ulEjectorCountForKOffset = 0;
		}
	}

	//v3.76
	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
	if (bPLLMSpecialFcn)
	{
		if (m_bEnableCustomCount1 && (m_ulCustomCounter1 > m_ulCustomCounter1Limit))
		{
			//SetAlert_Red_Yellow(IDS_BH_REPLACE_EJECTOR);
			SetAlert_Yellow_Green(IDS_BH_REPLACE_EJECTOR); //4.51D8 BH0003
			bReturn = TRUE;
			SetStatusMessage("Custom Counter #1 limit is reached");
			SetErrorMessage("Custom Counter #1 limit is reached");
		}
	}


	return bReturn;
}


INT CBondHead::IsCleanCollet()		//MS100 only
{
	INT nReturn = 0;		//0=not hit, 1=abort, 2=Cleaned
	BOOL bReturn = FALSE;	

	if( IsBurnIn() )
	{
		return 0;
	}

#ifndef NU_MOTION
	return 0;	//Quit if not MS100
#endif

	//v4.50A4	//SanAn		//Smart AutoClean PB limit reset method
	BOOL bCleanNow = FALSE;
	if (m_bAutoCleanColletNow1 && IsBHZ2ToPick())
	{
		bCleanNow = TRUE;
	}
	else if (m_bAutoCleanColletNow2 && IsBHZ1ToPick())
	{
		bCleanNow = TRUE;
	}

	if ( IsReachACCMaxCount() || bCleanNow)
	{
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		BOOL bColletCleanPocket = pApp->GetFeatureStatus(MS896A_FUNC_COLLET_CLEAN_POCKET);

		CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet");

		//v4.50A4	//SanAn		//Smart PB EMPTY limit reset method
		if (bCleanNow && m_bAutoCleanColletNow1)				
		{
			CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet 1 NOW due to EMPTY Limit (SanAn)");
			m_bAutoCleanColletNow1 = FALSE;			//Disable immediately after Trigger
			m_bEnableAutoCCNowCounter1 = TRUE;		//ENable 100dices counting after AutoClean
		}
		if (bCleanNow && m_bAutoCleanColletNow2)				
		{
			CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet 2 NOW due to EMPTY Limit (SanAn)");
			m_bAutoCleanColletNow2 = FALSE;			//Disable immediately after Trigger
			m_bEnableAutoCCNowCounter2 = TRUE;		//ENable 100dices counting after AutoClean
		}

		if (CMS896AApp::m_bMS100Plus9InchOption)	//MS109		//v4.42T7
		{
			INT nCount = 0;
			while (m_bIsBLIdleinABMode != TRUE)
			{
				Sleep(1000);
				nCount++;
				if (nCount > 20)
				{
					SetStatusMessage("OpAutoCleanCollet (MS109) timeouts");
					SetErrorMessage("OpAutoCleanCollet (MS109) timeouts");
					return 0;
				}
			}
		}

		//v4.48A2
		BOOL bForceManualClean = FALSE;
		if (pApp->GetCustomerName() == "Electech3E(DL)")
		{
			CString szTemp;
			szTemp.Format("3E(DL): Custom #3 counter : %d %lu %lu", m_bEnableCustomCount3, m_ulCustomCounter3, m_ulCustomCounter3Limit);
			CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

			if ( m_bEnableCustomCount3 && (m_ulCustomCounter3Limit > 0) )
			{
				m_ulCustomCounter3++;

				if (m_ulCustomCounter3 >= m_ulCustomCounter3Limit)
				{
					bForceManualClean = TRUE;
					m_ulCustomCounter3 = 0;
				}

				*(m_pulStorage + BH_CUSTOM_COUNT_3)	= m_ulCustomCounter3;
			}
		}

		//v4.52A2
CString szLog;
szLog.Format("BH: IsCleanCollet - AutoCleanCollet = %d, bColletCleanPocket = %d, bForceManualClean = %d",
				m_bAutoCleanCollet, bColletCleanPocket, bForceManualClean);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		m_bBhAutoCleanCollet = TRUE;		//v4.42T7
		if (m_bAutoCleanCollet == TRUE && bColletCleanPocket && !bForceManualClean)
		{
			if (bCleanNow)		//v4.50A4	//SanAn	//Smart PB EMPTY limit reset method 
			{
				CString szMsg;
				szMsg = "MS100 AutoCleanCollet NOW";
				CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);
			}

			SetAlertTime(TRUE, EQUIP_ASSIST_TIME, "ACC", "Dual Arm Collet");
			CTime stStartTime = CTime::GetCurrentTime();
			CloseAlarm();
			SetAlert_WarningMsg(IDS_MS_WARNING_CLEAN_COLLET, "Clean Collet...Please Wait");
			if (!OpMS100AutoCleanCollet(IsBHZ1ToPick()))
			{
				bReturn = TRUE;
			}

			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AutoCleanCollet");
			SetAlertTime(FALSE, EQUIP_ASSIST_TIME, "ACC", "Dual Arm Collet");
		}
		else
		{
			bReturn = OpManualCleanCollet();
		}
		m_bBhAutoCleanCollet = FALSE;		//v4.42T7

		nReturn = 2;			//Cleaned & reset
		if (bReturn == TRUE)
		{
			nReturn = 1;    //user ABORT
		}		

		//Clean count reset at 2nd time BLOW only such that both BHZ1 and BHZ2 can blow in consecutive cycles
		if (!bReturn && (m_ulCleanCount > m_ulMaxCleanCount))	// FALSE = Continue & Reset, TRUE = Abort
		{

			m_ulCleanCount = 0;
			m_dLastPickTime = GetTime();	// ms100 after second head done
			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CLEANCOUNT_OFFSET) = 0;
			}
			SetErrorMessage("auto clean collet reset");
			SetStatusMessage("auto clean collet reset");
		}
	}

	return nReturn;
}

//v3.89 bugfix BH2
BOOL CBondHead::OpManualCleanCollet()
{
	BOOL bReturn = TRUE;	//ABORT AUTOBOND by default

	if (IsMS90())	//v4.59A7
	{
		//In MS90 there is a new garbage bin installed on BT,
		// so BH needs to move to T PICK/BOND position to perform manual-clean;
		// the original Clean-Collet pos is used for UNLOAD-WAFER only in MS90.

		if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
		{
			T_SafeMoveToInAuto(m_lPickPos_T, "Op MCC 2");	//Clean BH2 at BOND side
		}
		else
		{
			T_SafeMoveToInAuto(m_lBondPos_T, "Op MCC 1");	//Clean BH1 at BOND side
		}
	}
	else
	{
		T_SafeMoveToInAuto(m_lCleanColletPos_T, "Op MCC");
	}


	LONG lOption;
	CString szText = " ";
#ifdef NU_MOTION			//v3.87
	if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
	{
		//SetAlert_Red_Yellow(IDS_BH_CLEAN_COLLET_BH2);
		SetAlert_Red_Back(IDS_BH_CLEAN_COLLET_BH2);
		SetStatusMessage("BH2 Collet cleantime is reached");
		SetErrorMessage("BH2 Collet cleantime is reached");
	}
	else
	{
		//SetAlert_Red_Yellow(IDS_BH_CLEAN_COLLET_BH1);
		SetAlert_Red_Back(IDS_BH_CLEAN_COLLET_BH1);
		SetStatusMessage("BH1 Collet cleantime is reached");
		SetErrorMessage("BH1 Collet cleantime is reached");
	}
#else
	//SetAlert_Red_Yellow(IDS_BH_CLEAN_COLLET);
	SetAlert_Red_Back(IDS_BH_CLEAN_COLLET);
	SetStatusMessage("Collet cleantime is reached");
	SetErrorMessage("Collet cleantime is reached");
#endif
	bReturn = TRUE;

	//Ask user to reset counter & continue	
	//lOption = SetAlert_Msg(IDS_BH_RESET_CLEAN_COUNT, szText, "No", "Yes", "Clean");		
	lOption = SetAlert_Msg_Red_Back(IDS_BH_RESET_CLEAN_COUNT, szText, "Clean", "No", "Yes");		//v3.93
	// 1 = Clean
	// 5 = No
	// 8 = Yes


	//v4.59A7	//MS90
	IPC_CServiceMessage stMsg;
	BOOL bToGarbageBin = TRUE;
	if (IsMS90())
	{
		stMsg.InitMessage(sizeof(BOOL), &bToGarbageBin);
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToMS90GBinPosn"), stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!bReturn)
		{
			HmiMessage_Red_Yellow("Manual-Clean-Collet moves BT fails. (MS90)");
			SetErrorMessage("Manual-Clean-Collet moves BT fails. (MS90)");
		}
	}


	//v3.94T5
	if ((lOption != 1) && (IsCoverOpen() == TRUE))
	{
		SetAlert_Red_Back(IDS_BH_COVER_OPEN);
		lOption = 1;
	}

	//while (lOption == 8)				//CLEAN
	while (lOption == 1)				//CLEAN		//v3.93
	{
#ifdef NU_MOTION
		if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
		{
			//Clean BHZ2
			SetPickVacuumZ2(FALSE);
			Sleep(500);						//Extra 500ms NEUTRAL state before StrongBlow	//v4.17T7
			SetStrongBlowZ2(TRUE);
			Sleep(m_lHighBlowTime);			// Cleaning 
			SetStrongBlowZ2(FALSE);
			Sleep(100);
			SetPickVacuumZ2(TRUE);
		}
		else
#endif
		{
			//Clean BHZ1
			SetPickVacuum(FALSE);
			Sleep(500);						//Extra 500ms NEUTRAL state before StrongBlow	//v4.17T7
			SetStrongBlow(TRUE);
			Sleep(m_lHighBlowTime);			// Cleaning 
			SetStrongBlow(FALSE);
			Sleep(100);
			SetPickVacuum(TRUE);
		}
		
#ifdef NU_MOTION
		if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
		{
			lOption = SetAlert_Msg_Red_Back(IDS_BH_RESET_CLEAN_COUNT_BH2, szText, "Clean", "No", "Yes"); 
		}		
		else
		{
			lOption = SetAlert_Msg_Red_Back(IDS_BH_RESET_CLEAN_COUNT_BH1, szText, "Clean", "No", "Yes"); 
		}		
#else
		lOption = SetAlert_Msg_Red_Back(IDS_BH_RESET_CLEAN_COUNT, szText, "Clean", "No", "Yes");		
#endif

		//v3.94T5	//For safety reason/protection
		if ((lOption != 1) && (IsCoverOpen() == TRUE))
		{
			SetAlert_Red_Back(IDS_BH_COVER_OPEN);
			lOption = 1;
		}
	}

	
	switch (lOption)
	{
	case -1:	//CANCEL
	case 5:		//NO		//v3.93
		//case 1:		/NO
		bReturn = TRUE;
		SetErrorMessage("Manual clean collet abort");
		break;

		//case 2:
		//case 5:	//YES
	case 8:		//YES		//v3.93
#ifndef NU_MOTION
		m_ulCleanCount = 0;
//		m_dLastPickTime	= GetTime();	// manual clean collet done
		if (m_pulStorage != NULL)
		{
			*(m_pulStorage + BH_CLEANCOUNT_OFFSET) = 0;
		}
		SetErrorMessage("auto clean collet reset");
		SetStatusMessage("auto clean collet reset");
#endif
		bReturn = FALSE;
		break;
	}

	// Home to Prevent the bond head lost 
	// its torque after clean collet
#ifdef NU_MOTION
	if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
	{
		Z2_MoveToHome();
		Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);		//v4.44A1	//Semitek
	}
	else
#endif
	{
		Z_MoveToHome();
		Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	}

	if (IsAllMotorsEnable() == FALSE)
	{
		bReturn = TRUE;
	}
		

	//v4.59A7
	bToGarbageBin = FALSE;
	if (IsMS90())
	{
		T_SafeMoveToInAuto(m_lCleanColletPos_T, "Op MCC Done");

		stMsg.InitMessage(sizeof(BOOL), &bToGarbageBin);
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToMS90GBinPosn"), stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!bReturn)
		{
			HmiMessage_Red_Yellow("Manual-Clean-Collet moves BT fails. (MS90)");
			SetErrorMessage("Manual-Clean-Collet moves BT fails. (MS90)");
		}
	}

	CheckCoverOpenInAuto("Op MCC complete");
	return bReturn;
}


BOOL CBondHead::OpManualCleanCollet_MDCJ_MS90()	//v4.59A7
{
	CheckCoverOpenInAuto("MD1 blow");

	T_Profile(CLEAN_COLLET_PROF);	
	//T_SafeMoveToInAuto(m_lCleanColletPos_T, "UpLook PR Fail");	
	if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
	{
		T_SafeMoveToInAuto(m_lPickPos_T, "UpLook PR Fail 2");	//Clean BH2 at BOND side
	}
	else
	{
		T_SafeMoveToInAuto(m_lBondPos_T, "UpLook PR Fail 1");	//Clean BH1 at BOND side
	}


	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE;
	BOOL bToGarbageBin = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bToGarbageBin);
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToMS90GBinPosn"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!bReturn)
	{
		HmiMessage_Red_Yellow("Manual-Clean-Collet MD moves BT fails. (MS90)");
		SetErrorMessage("Manual-Clean-Collet MD moves BT fails. (MS90)");
		return FALSE;
	}


	TakeTime(PV2);	
	if (IsBHZ1ToPick())		//If BHZ1 towawrds PICK		
	{
		SetPickVacuumZ2(FALSE);		
		Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
		SetStrongBlowZ2(TRUE);
		Sleep(m_lHighBlowTime);	 
		SetStrongBlowZ2(FALSE);
	}
	else
	{
		SetPickVacuum(FALSE);	
		Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
		SetStrongBlow(TRUE);
		Sleep(m_lHighBlowTime);	
		SetStrongBlow(FALSE);
	}

				
	Sleep(1000);
	T_SafeMoveToInAuto(m_lCleanColletPos_T, "UpLook PR Fail");		// Move to clean collet position
	Sleep(1000);


	bToGarbageBin = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bToGarbageBin);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToMS90GBinPosn"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!bReturn)
	{
		HmiMessage_Red_Yellow("Manual-Clean-Collet MD moves BT fails. (MS90)");
		SetErrorMessage("Manual-Clean-Collet MD moves BT fails. (MS90)");
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::OpAutoCleanCollet()
{
	IPC_CServiceMessage stMsg;
	INT nConvID;
	BOOL bReturn = TRUE;

	// bond arm move to clean position and bond head up to swing level
	Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	Sleep(50);
	T_SafeMoveToInAuto(m_lAutoCleanCollet_T, "Op ACC");
	Sleep(50);

	// make sure bin table move back as it is indexed already or postbond not done yet
	BOOL bToBond = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bToBond);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToBondPosn"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!bReturn)	//v4.42T7
	{
		HmiMessage_Red_Yellow("Auto Clean Collet fails.");
		SetErrorMessage("Auto Clean Collet fails.");
		return FALSE;
	}


	// let bin table move to alcohol dropping position
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("BTMoveToACCLiquid"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	// press the alcohol to the cloth
	CleanColletPocketAlcoholOnce();

	// let bin table move to clean position
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("BTMoveToACCClean"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	// bond head down to level
	LONG lCleanLevel = m_lAutoCleanCollet_Z;
	if (m_lAutoCleanOffsetZ != 0)
	{
		lCleanLevel = m_lBondLevel_Z + m_lAutoCleanOffsetZ;
	}
	Z_MoveTo(lCleanLevel, SFM_WAIT);
	Sleep(50);

	// let the bin table move brush the collet, with a pattern and cycle and time delay
	BOOL bWhichBHZ = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bWhichBHZ);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("BTCycleACCOperation"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 3000000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	stMsg.GetMsg(sizeof(BOOL), &bReturn);

	if( m_ulCleanColletTimeOut>0 )
	{
		SetPickVacuum(FALSE);	// before down to clean, turn on block
		SetStrongBlow(TRUE);
	}

	Z_MoveToHome();

	Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);

	// make sure bin table move back as it is indexed already or postbond not done yet
	bToBond = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bToBond);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("MoveBinTableToBondPosn"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 300000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn == FALSE)
	{
		return TRUE;
	}

	//Clean BHZ1
	SetPickVacuum(FALSE);
	Sleep(500);				//additional NEUTRAL state before Strong-Blow

	//v4.16T5	//TW EverVision by Leo Hung
	if (m_lAutoCleanAirBlowLimit > 0)
	{
		INT nCount = 0;
		do 
		{
			SetStrongBlow(TRUE);
			Sleep(m_lHighBlowTime);			// Cleaning 
			SetStrongBlow(FALSE);
			Sleep(m_lHighBlowTime);
			nCount++;

		} 
		while (nCount < m_lAutoCleanAirBlowLimit);
	}

	Sleep(100);
	SetPickVacuum(TRUE);
	Sleep(200);

	if (!IsAllMotorsEnable())
	{
		SetErrorMessage("BH: Motor loses power in OpAutoCleanCollet");
		return TRUE;	//ABORT AUTOBOND mode if any motor is dead
	}

	return FALSE;
}


LONG CBondHead::OpCalCompen_Ej()
{
	m_lEjectorKOffsetTotal = 0;
	m_lEjSubRegionKOffsetTotal = 0;
	m_lEjSubRegionSKOffsetTotal = 0;

	if ((m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0))
	{
		//m_lEjectorKOffsetLimit -> max Z offset that can be applied to trigger KOffset alarm
		//m_lEjectorKCount  -> K counter to check if Ej counts exceeds and needs to add KOffset to Ej Up level
		//m_lEjectorKOffset -> in motor step; Z offset to increment when EjCount exceeds KCount
		
		if (m_bResetEjKOffsetAtWaferEnd)
		{
			m_lEjectorKOffsetTotal = (LONG)(m_ulEjectorCountForKOffset / m_lEjectorKCount) * m_lEjectorKOffset;
		}
		else
		{
			m_lEjectorKOffsetTotal = (LONG)(m_ulEjectorCount / m_lEjectorKCount) * m_lEjectorKOffset;
		}

		LONG lEjectorKOffsetTotal = m_lEjectorKOffsetTotal;	//For logging purpose only


		//Every "m_lEjSubRegionKCount" dices picked has to increase Ej level by "m_lEjSubRegionKOffset" steps
		CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
		if (/* pUtl->GetPrescanRegionMode()	&&*/ 
			 (m_lEjSubRegionKCount > 0)		&& 
			 (m_lEjSubRegionKOffset > 0) )
		{
			if (pUtl->GetPrescanRegionMode())
			{
				m_lEjSubRegionKOffsetTotal = (LONG)(m_ulEjCountForSubRegionKOffset / m_lEjSubRegionKCount) * m_lEjSubRegionKOffset;
			}
			else
			{
				m_lEjSubRegionKOffsetTotal = 0;
			}

			//v4.46T1	//Smart KOffset fcn
			if ( (m_lEjSubRegionSKCount > 0) && (m_lEjSubRegionSKOffset > 0) )
			{
				if (m_ulEjCountForSubRegionSKOffset >= (ULONG)m_lEjSubRegionSKCount)
				{
					if (m_lEjSubRegionSmartValue > 0)
						m_lEjSubRegionSmartValue--;
					if (m_lEjSubRegionSmartValue < 0)
						m_lEjSubRegionSmartValue = 0;

					m_ulEjCountForSubRegionSKOffset = 0;
				}

				m_lEjSubRegionSKOffsetTotal = m_lEjSubRegionSKOffset * m_lEjSubRegionSmartValue;

				//if (m_lEjSubRegionSmartValue > 0)
				//{
//					CString szLog2;
//					szLog2.Format("Ej-Smart-KOffset = %ld steps; (Count = %ld, KCount = %ld, KOffset = %ld, SmartVal = %ld)", 
//									m_lEjSubRegionSKOffsetTotal, m_ulEjCountForSubRegionSKOffset, m_lEjSubRegionSKCount, 
//									m_lEjSubRegionSKOffset, m_lEjSubRegionSmartValue);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog2);
				//}
			}

			m_lEjectorKOffsetTotal =	m_lEjectorKOffsetTotal		+ 
										m_lEjSubRegionKOffsetTotal	+ 
										m_lEjSubRegionSKOffsetTotal;

//			CString szLog;
//			szLog.Format("Ej-SubRegion-KOffset = %ld steps; (Count = %ld, KCount = %ld, KOffset = %ld)", 
//							m_lEjSubRegionKOffsetTotal, m_ulEjCountForSubRegionKOffset, m_lEjSubRegionKCount, m_lEjSubRegionKOffset);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}


		//v4.52A10	//XM SanAn
		if (IsBHZ1ToPick())	//If BHZ1 at PICK		
		{
			m_lEjectorKOffsetForBHZ1MDCount++; //M68
			if (m_lEjectorKOffsetForBHZ1MDCount > m_lEjectorKOffsetResetCount &&
				(m_lEjectorKOffsetBHZ1Total > 0)&& (m_lEjectorKOffsetResetCount >0) )
			{
				m_lEjectorKOffsetForBHZ1MDCount = 0;
				m_lEjectorKOffsetBHZ1Total = m_lEjectorKOffsetBHZ1Total - m_lEjectorKOffsetForBHZ1MD;
				if (m_lEjectorKOffsetBHZ1Total < 0)
					m_lEjectorKOffsetBHZ1Total = 0;
			}

			if (m_lEjectorKOffsetBHZ1Total > 0)
			{
				m_lEjectorKOffsetTotal = m_lEjectorKOffsetTotal + m_lEjectorKOffsetBHZ1Total;
			}
		}
		else
		{
			m_lEjectorKOffsetForBHZ2MDCount++; //M68
			if ((m_lEjectorKOffsetBHZ2Total > 0) && (m_lEjectorKOffsetResetCount >0) &&
				(m_lEjectorKOffsetForBHZ2MDCount > m_lEjectorKOffsetResetCount))
			{
				m_lEjectorKOffsetForBHZ2MDCount = 0;
				m_lEjectorKOffsetBHZ2Total = m_lEjectorKOffsetBHZ2Total - m_lEjectorKOffsetForBHZ2MD;
				if (m_lEjectorKOffsetBHZ2Total < 0)
					m_lEjectorKOffsetBHZ2Total = 0;
			}

			if (m_lEjectorKOffsetBHZ2Total > 0)
			{
				m_lEjectorKOffsetTotal = m_lEjectorKOffsetTotal + m_lEjectorKOffsetBHZ2Total;
			}
		}


		// Check whether the offset is exceed the limit
		if (m_lEjectorKOffsetTotal > m_lEjectorKOffsetLimit)
		{
			m_lEjectorKOffsetTotal = m_lEjectorKOffsetLimit;
		}

//		CString szLog;
//		szLog.Format("Ej UP Level = %ld; TotalOffset = %ld (K = %ld, SubRegion = %ld, Smart = %ld)", 
//						m_lEjectLevel_Ej, m_lEjectorKOffsetTotal, 
//						lEjectorKOffsetTotal, m_lEjSubRegionKOffsetTotal, m_lEjSubRegionSKOffsetTotal);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	return m_lEjectorKOffsetTotal;
}

LONG CBondHead::OpCalCompen_Z(BOOL bBHZ2)
{
	LONG lZOffsetTotal = 0;
	CString szLog;

	//This fcn only used for Semitek MS60 with spring-arm installed to reduce 
	//  impact force and improve collet lifetime;
		
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Semitek")
	{
		return lZOffsetTotal;
	}

	if (bBHZ2)	//If BH2 at PICK		
	{
		if ( (m_lZ2SmartKCount > 0) && (m_lZ2SmartKOffset > 0) )
		{
			m_ulZ2CountForSmartKOffset++;

			if (m_ulZ2CountForSmartKOffset >= (ULONG) m_lZ2SmartKCount)
			{
				//If more than 100 counts, decrease SmartValue by 1 step
				if (m_lZ2SmartValue > 0)
				{
					m_lZ2SmartValue--;

					szLog.Format("Z2-SKOffset: -1 due to %ld counts hit; curr Z2 SValue = %ld", 
						m_lZ2SmartKCount, m_lZ2SmartValue);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				}
				if (m_lZ2SmartValue < 0)
					m_lZ2SmartValue = 0;

				m_ulZ2CountForSmartKOffset = 0;
			}

			if (m_lZ2SmartValue > 0)
			{
				m_lZ2SmartKOffsetTotal = m_lZ2SmartKOffset * m_lZ2SmartValue;
				if (m_lZ2SmartKOffsetTotal > 50)
				{
					m_lZ2SmartKOffsetTotal = 50;
				}

				lZOffsetTotal = -1 * m_lZ2SmartKOffsetTotal;
			}
			else
			{
				m_lZ2SmartKOffsetTotal = 0;		//For runtime display on HMI
			}
		}
	}
	else
	{
		if ( (m_lZ1SmartKCount > 0) && (m_lZ1SmartKOffset > 0) )
		{
			m_ulZ1CountForSmartKOffset++;

			if (m_ulZ1CountForSmartKOffset >= (ULONG) m_lZ1SmartKCount)
			{
				//If more than 100 counts, decrease SmartValue by 1 step
				if (m_lZ1SmartValue > 0)
				{
					m_lZ1SmartValue--;

					szLog.Format("Z1-SKOffset: -1 due to %ld counts hit; curr Z1 SValue = %ld", 
						m_lZ1SmartKCount, m_lZ1SmartValue);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				}
				if (m_lZ1SmartValue < 0)
					m_lZ1SmartValue = 0;

				m_ulZ1CountForSmartKOffset = 0;
			}

			if (m_lZ1SmartValue > 0)
			{
				m_lZ1SmartKOffsetTotal = m_lZ1SmartKOffset * m_lZ1SmartValue;
				if (m_lZ1SmartKOffsetTotal > 50)
				{
					m_lZ1SmartKOffsetTotal = 50;
				}

				lZOffsetTotal = -1 * m_lZ1SmartKOffsetTotal;
			}
			else
			{
				m_lZ1SmartKOffsetTotal = 0;		//For runtime display on HMI
			}
		}
	}

	return lZOffsetTotal;
}

BOOL CBondHead::OpIncSKOffsetForBHZMD(BOOL bBHZ2)	//v4.53A25	//Semitek
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szLog;

	//v4.58A1
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Lumileds")
	{
		SetErrorMessage("OpIncSKOffsetForBHZMD: fcn is disabled for PLLM");
		return TRUE;
	}

	if (/* pUtl->GetPrescanRegionMode() && */(m_lEjSubRegionSKCount > 0) && (m_lEjSubRegionSKOffset > 0) )
	{
		m_lEjSubRegionSmartValue++;
	}

	if (bBHZ2)
	{
		if ( (m_lZ2SmartKCount > 0) && (m_lZ2SmartKOffset > 0) )
		{
			m_lZ2SmartValue++;

			LONG lTotal = m_lZ2SmartKOffset * m_lZ2SmartValue;
			if (lTotal > 100)	//50)	//v4.54A4
			{
				m_lZ2SmartValue = m_lZ2SmartValue - 1;
			}

			m_ulZ2CountForSmartKOffset = 0;		//Reset Z2 Smart counter

			szLog.Format("Z2-SKOffset: +1 due to MD; curr Z2 SValue = %ld (SKCount = %ld, SKOffset = %ld)", 
				m_lZ2SmartValue, m_lZ2SmartKCount, m_lZ2SmartKOffset);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}			
	else
	{
		if ( (m_lZ1SmartKCount > 0) && (m_lZ1SmartKOffset > 0) )
		{
			m_lZ1SmartValue++;

			LONG lTotal = m_lZ1SmartKOffset * m_lZ1SmartValue;
			if (lTotal > 100)	//50)	//v4.54A4
			{
				m_lZ1SmartValue = m_lZ1SmartValue - 1;
			}

			m_ulZ1CountForSmartKOffset = 0;		//Reset Z1 Smart counter

			szLog.Format("Z1-SKOffset: +1 due to MD; curr Z1 SValue = %ld (SKCount = %ld, SKOffset = %ld)", 
				m_lZ1SmartValue, m_lZ1SmartKCount, m_lZ1SmartKOffset);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}			

	return TRUE;
}

LONG CBondHead::OpIncEjKOffsetForBHZMD()
{
	LONG lOffsetTotal = 0;
	CString szLog;

	//v4.58A1
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Lumileds")
	{
		SetErrorMessage("OpIncEjKOffsetForBHZMD: fcn is disabled for PLLM");
		return 0;
	}

#ifdef NU_MOTION
	if (IsBHZ1ToPick())	//If BHZ1 at PICK		
	{
		if ((m_lEjectorKOffsetForBHZ2MD > 0) && (m_lEjectorKOffsetForBHZ2MD <= 100))
		{

			m_lEjectorKOffsetForBHZ2MDCount = 0;
			m_lEjectorKOffsetBHZ2Total = m_lEjectorKOffsetBHZ2Total + m_lEjectorKOffsetForBHZ2MD;
			lOffsetTotal = m_lEjectorKOffsetBHZ2Total;

			szLog.Format("BH: Ej KOffset for BHZ2 MD added - %ld (%ld)", 
				m_lEjectorKOffsetBHZ2Total, m_lEjectorKOffsetForBHZ2MD);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
	else
	{
		if ((m_lEjectorKOffsetForBHZ1MD > 0) && (m_lEjectorKOffsetForBHZ1MD <= 100))
		{
			m_lEjectorKOffsetForBHZ1MDCount = 0;
			m_lEjectorKOffsetBHZ1Total = m_lEjectorKOffsetBHZ1Total + m_lEjectorKOffsetForBHZ1MD;
			lOffsetTotal = m_lEjectorKOffsetBHZ1Total;

			szLog.Format("BH: Ej KOffset for BHZ1 MD added - %ld (%ld)", 
				m_lEjectorKOffsetBHZ1Total, m_lEjectorKOffsetForBHZ1MD);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
#endif
	return lOffsetTotal;
}

//v4.43T11
LONG CBondHead::OpCalKOffset_BH1()
{
	m_lDAFlowBH1KOffsetTotal = 0;
	if ((m_lDAFlowBH1KCount > 0) && (m_lDAFlowBH1KOffset > 0))
	{
		//m_lDAFlowBH1KOffsetLimit -> max airflow value that can be applied to trigger KOffset alarm
		//m_lDAFlowBH1KCount  -> K counter to check if airflow value exceeds and needs to add KOffset to Ej Up level
		//m_lDAFlowBH1KOffset -> in motor step; airflow value offset to increment when ColletCount exceeds KCount

		m_lDAFlowBH1KOffsetTotal = (LONG)(m_ulColletCount / m_lDAFlowBH1KCount) * m_lDAFlowBH1KOffset;

		// Check whether the offset is exceed the limit
		if (m_lDAFlowBH1KOffsetTotal > m_lDAFlowBH1KOffsetLimit)
		{
			m_lDAFlowBH1KOffsetTotal = m_lDAFlowBH1KOffsetLimit;
		}
	}
	return m_lDAFlowBH1KOffsetTotal;
}


LONG CBondHead::OpCalKOffset_BH2()
{
	m_lDAFlowBH2KOffsetTotal = 0;
	if ((m_lDAFlowBH2KCount > 0) && (m_lDAFlowBH2KOffset > 0))
	{
		//m_lDAFlowBH2KOffsetLimit -> max airflow value that can be applied to trigger KOffset alarm
		//m_lDAFlowBH2KCount  -> K counter to check if airflow value exceeds and needs to add KOffset to Ej Up level
		//m_lDAFlowBH2KOffset -> in motor step; airflow value offset to increment when ColletCount exceeds KCount

		m_lDAFlowBH2KOffsetTotal = (LONG)(m_ulCollet2Count / m_lDAFlowBH2KCount) * m_lDAFlowBH2KOffset;

		// Check whether the offset is exceed the limit
		if (m_lDAFlowBH2KOffsetTotal > m_lDAFlowBH2KOffsetLimit)
		{
			m_lDAFlowBH2KOffsetTotal = m_lDAFlowBH2KOffsetLimit;
		}
	}
	return m_lDAFlowBH2KOffsetTotal;
}


BOOL CBondHead::OpResetEjKOffsetAtWaferEnd()	//v4.43T8	//Semitek
{
	//CString szLog;
	//szLog.Format("OpResetEjKOffsetAtWaferEnd: %d  %ld  %ld", m_bResetEjKOffsetAtWaferEnd, m_lEjectorKCount, m_lEjectorKOffset);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	if (m_bResetEjKOffsetAtWaferEnd && (m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0))
	{
		m_lEjectorKOffsetTotal = 0;
		m_ulEjectorCountForKOffset = 0;
		//ResetEjectorCount();
		//CMSLogFileUtility::Instance()->MS_LogOperation("Ej KOffset reset at WaferEnd");
	}

	m_ulEjCountForSubRegionKOffset = 0;
	m_ulEjCountForSubRegionSKOffset = 0;
	m_lEjSubRegionSmartValue = 0;

	return TRUE;
}



VOID CBondHead::OpCheckColletsB4Autobond()
{
	//Unuse
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif

	//SB for 200ms before start
	SetPickVacuum(FALSE);
	Sleep(500);						//Extra 500ms NEUTRAL state before StrongBlow	//v4.17T7

	SetStrongBlow(TRUE);
#ifdef NU_MOTION
	SetPickVacuumZ2(FALSE);
	SetStrongBlowZ2(TRUE);
#endif
	Sleep(200);
	SetStrongBlow(FALSE);
#ifdef NU_MOTION
	SetStrongBlowZ2(FALSE);
#endif
}


VOID CBondHead::OpEMOStopAutobond()
{
	SetErrorMessage("EMO triggers machine STOP");
	CMSLogFileUtility::Instance()->MS_LogOperation("EMO triggers machine STOP");
	
//AfxMessageBox("EXIT ...", MB_SYSTEMMODAL);
	
	HANDLE hToken = NULL;	//Klocwork
	TOKEN_PRIVILEGES tkp;

	// Change the privilege to shutdown the system
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	// Shutdown now
	ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
				  SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED);

	PostMessage(NULL, WM_CLOSE, NULL, NULL);
	CloseHandle(hToken);			//Klocwork	//v4.22T9

	m_bIsEMOTriggered = TRUE;		//v4.24
}


BOOL CBondHead::OpIsThermalAlarmTriggered()		//v4.26T1
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBH)
	{
		return FALSE;
	}
	if (!CMS896AStn::m_bDBHThermalCheck)
	{
		return FALSE;
	}

#ifdef NU_MOTION
	//v4.31T11
	//LONG lAlarmCode = 0;
	//if (IsDBHThermalAlarm(lAlarmCode))
	//{
	//	CString szErr;
	//	szErr.Format("DBH Thermal sensor %d is triggered in AUTOBOND mode", lAlarmCode);
	//	SetErrorMessage(szErr);
	//	//HmiMessage_Red_Yellow(szErr);
	//	return TRUE;
	//}
	return FALSE;
#else
	return FALSE;
#endif
}


////////////////////////////////////////////
//	Check hardware ready 
////////////////////////////////////////////
BOOL CBondHead::IsMotionHardwareReady()	//	HuaMao 2 motor error, to house keeping and then stop
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBH)	//v3.60
	{
		return TRUE;
	}

	/*if (m_bIsArm2Exist == FALSE)
	{
		m_bHome_Z2 = TRUE;
	}*/

	BOOL bCheckMotionResult = TRUE;

	CString szText;

	/*if (m_bSel_T)
	{
		if (!T_IsPowerOn())
		{
			szText = "\nBondHead T is not power ON";
			SetStatusMessage("BondHead T NOT power ON yet");
			SetErrorMessage("BondHead T NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if( (CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T) == FALSE) || !m_bHome_T )
		{
			szText = "\nBondHeadT";
			SetStatusMessage("BondHeadT NOT home yet");
			SetErrorMessage("BondHeadT NOT home yet");
			bCheckMotionResult = FALSE;
		}
	}

	if (m_bSel_Z)
	{
		if (!Z_IsPowerOn())
		{
			szText = "\nBondHead Z is not power ON";
			SetStatusMessage("BondHead Z NOT power ON yet");
			SetErrorMessage("BondHead Z NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if ((CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z) == FALSE) || !m_bHome_Z)
		{
			szText = "\nBondHeadZ";
			SetStatusMessage("BondHeadZ NOT home yet");
			SetErrorMessage("BondHeadZ NOT home yet");
			bCheckMotionResult = FALSE;
		}
	}*/

	if (m_bSel_Ej)
	{
		if (!Ej_IsPowerOn())
		{
			szText = "\nEjector is not power ON";
			SetStatusMessage("Ejector NOT power ON yet");
			SetErrorMessage("Ejector NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if ((CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej) == FALSE) || !m_bHome_Ej)
		{
			szText = "\nEjector";
			SetStatusMessage("Ejector NOT home yet");
			SetErrorMessage("Ejector NOT home yet");
			bCheckMotionResult = FALSE;
		}
	}

	/*if (m_bSel_Ej && m_bMS100EjtXY)
	{
		if (!EjX_IsPowerOn())
		{
			szText = "\nEjector X is not power ON";
			SetStatusMessage("Ejector X NOT power ON yet");
			SetErrorMessage("Ejector X NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if (!EjY_IsPowerOn())
		{
			szText = "\nEjector Y is not power ON";
			SetStatusMessage("Ejector Y NOT power ON yet");
			SetErrorMessage("Ejector Y NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if ((CMS896AStn::MotionCheckResult(BH_AXIS_EJ_X, &m_stBHAxis_EjX) == FALSE) || !m_bHome_EjX)
		{
			szText = "\nEjector";
			SetStatusMessage("Ejector X NOT home yet");
			SetErrorMessage("Ejector X NOT home yet");
			bCheckMotionResult = FALSE;
		}

		if ((CMS896AStn::MotionCheckResult(BH_AXIS_EJ_Y, &m_stBHAxis_EjY) == FALSE) || !m_bHome_EjY)
		{
			szText = "\nEjector";
			SetStatusMessage("Ejector Y NOT home yet");
			SetErrorMessage("Ejector Y NOT home yet");
			bCheckMotionResult = FALSE;
		}
	}*/

#ifdef NU_MOTION
	/*if (m_bIsArm2Exist)
	{
		if (m_bSel_Z2)
		{
			if ((CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2) == FALSE) || !m_bHome_Z2)
			{
				szText = "\nBondHeadZ2";
				SetStatusMessage("BondHeadZ2 NOT home yet");
				SetErrorMessage("BondHeadZ2 NOT home yet");
				bCheckMotionResult = FALSE;
			}
		}
	}*/
#endif
	/*
	if (bCheckMotionResult == FALSE)
	{
		//StopCycle("BondHeadStn");
		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		SetMotionCE(TRUE, "bCheckMotionResult fail in IsMotionHardwareReady");	//v4.59A19
		
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		return FALSE;
	}

	if ((m_bEnableFrontCoverLock == TRUE) && (m_bIsFrontCoverExist == TRUE))
	{
		SetFrontCoverLock(FALSE);
		Sleep(500);
		SetFrontCoverLock(TRUE);
		Sleep(500);

		if (IsFrontCoverLocked() == FALSE)
		{
		//	StopCycle("BondHeadStn");

			SetErrorMessage("Front cover is unlocked");
			SetAlert_Red_Yellow(IDS_BH_FRONT_COVER_UNLOCK);
			return FALSE;
		}
	}

	if ((m_bEnableSideCoverLock == TRUE) && (m_bIsSideCoverExist == TRUE))
	{
		SetSideCoverLock(FALSE);
		Sleep(500);
		SetSideCoverLock(TRUE);
		Sleep(500);

		if (IsSideCoverLocked() == FALSE)
		{
		//	StopCycle("BondHeadStn");

			SetErrorMessage("Side cover is unlocked");
			SetAlert_Red_Yellow(IDS_BH_SIDE_COVER_UNLOCK);
			return FALSE;
		}
	}

	if ((m_bEnableBinElevatorCoverLock == TRUE) && (m_bIsBinElevatorCoverExist == TRUE))
	{
		SetBinElevatorCoverLock(FALSE);
		Sleep(500);
		SetBinElevatorCoverLock(TRUE);
		Sleep(500);

		if (IsBinElevatorCoverLocked() == FALSE)
		{
		//	StopCycle("BondHeadStn");

			SetErrorMessage("Bin Elevator cover is unlocked");
			SetAlert_Red_Yellow(IDS_BH_SIDE_COVER_UNLOCK);
			return FALSE;
		}
	}

	if (IsWLExpanderOpen() == TRUE)
	{
	//	StopCycle("BondHeadStn");

		SetErrorMessage("Expander not closed");
		return FALSE;
	}
	*/
	return TRUE;
}


VOID CBondHead::OpUpdateBondedUnit(const DOUBLE dCycleTime)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->m_bCycleStarted == TRUE)
	{
		pApp->m_eqMachine.UpdateUnit(dCycleTime);
		pApp->m_eqMachine2.UpdateUnit(dCycleTime);	//WH Sanan	//v4.40T4
	}

	m_ulBondDieCount++;
}

BOOL CBondHead::IsEnableSyncMove()
{
	return (m_bEnableSyncMove) && (m_dSyncSpeed > 0) && (m_lSyncDistance > 0);
}


INT CBondHead::OpSyncMove()
{
	INT nResult;	

	CString szMsg;
	GetEncoderValue();
	szMsg.Format("syn move before search T %d, Z %d, EJ %d", m_lEnc_T, m_lEnc_Z, m_lEnc_Ej);
	CMSLogFileUtility::Instance()->BH_DieCounterLog(szMsg);

	TakeTime(Z1);

	LONG lSpeed = (LONG)(m_dSyncSpeed * 128);
	try
	{
		CMS896AStn::MotionSelectSearchProfile(BH_AXIS_EJ, BH_SP_E_SYNC_MOVE, &m_stBHAxis_Ej);
		CMS896AStn::MotionSearch(BH_AXIS_EJ, 1, SFM_NOWAIT, &m_stBHAxis_Ej, BH_SP_E_SYNC_MOVE);
		if( SPECIAL_DEBUG_LOG_BH )
		{
			DisplaySequence("BH - Ej Cycle OpSyncMove");
		}
	}
	catch (CAsmException e)
	{
		CMSLogFileUtility::Instance()->BH_DieCounterLog("syn move of EJT exception");
		DisplayException(e);
		return gnNOTOK;
	}

	try
	{
		CMS896AStn::MotionSelectSearchProfile(BH_AXIS_Z, BH_SP_Z_SYNC_MOVE, &m_stBHAxis_Z);
		CMS896AStn::MotionSearch(BH_AXIS_Z, 1, SFM_NOWAIT, &m_stBHAxis_Z, BH_SP_Z_SYNC_MOVE);
	}
	catch (CAsmException e)
	{
		CMSLogFileUtility::Instance()->BH_DieCounterLog("syn move of BHZ exception");
		DisplayException(e);
		return gnNOTOK;
	}

	TakeTime(Z2);

	if ((nResult = Z_Sync()) != gnOK)
	{
		CMSLogFileUtility::Instance()->BH_DieCounterLog("syn move wait ejector error");
		return nResult;
	}
	TakeTime(Z3);
	if ((nResult = Ej_Sync()) != gnOK)
	{
		CMSLogFileUtility::Instance()->BH_DieCounterLog("syn move wait ejector error");
		return nResult;
	}
	TakeTime(Z4);

	GetEncoderValue();
	szMsg.Format("syn move after search T %d, Z %d, EJ %d", m_lEnc_T, m_lEnc_Z, m_lEnc_Ej);
	CMSLogFileUtility::Instance()->BH_DieCounterLog(szMsg);

	if (m_lPickDelay > 0)
	{
		Sleep(m_lPickDelay);
	}

	GetEncoderValue();
	szMsg.Format("syn move after 2nd pick delay T %d, Z %d, EJ %d\n", m_lEnc_T, m_lEnc_Z, m_lEnc_Ej);
	CMSLogFileUtility::Instance()->BH_DieCounterLog(szMsg);
	return gnOK;
}


INT CBondHead::OpCheckThermalTest()
{
	if (m_bEnableThermalTest == FALSE)
	{
		return TRUE;
	}

	// stop the bond arm for collect hole test
	if (m_bThermalTestFirstCycle == TRUE)
	{
		m_bSel_T = FALSE;
		m_bThermalTestFirstCycle = FALSE;
		m_ctBondStartTime = CTime::GetCurrentTime();
		return FALSE;
	}
	// move the bond arm for bonding
	else 
	{
		CTimeSpan cTimeDiff = CTime::GetCurrentTime() - m_ctBondStartTime;

		if (cTimeDiff.GetTotalMinutes() >= m_lCollectHoleCatpureTime)
		{
			m_bSel_T = TRUE;
		}
	}

	return TRUE;
}


INT CBondHead::OpUpdateMachineCounters()
{
	m_ulNewPickCounter++;
	m_ulPickDieCount++;

	if( IsBurnIn() )
	{
		return TRUE;
	}

//v3.98T1
#ifdef NU_MOTION
/*
	if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop)		//v4.40T7
	{
		if (IsBHZ1ToPick())		//If BHZ1 at PICK		
		{
			if (m_ulMaxColletCount > 0)
			{
				m_ulColletCount++; 
			}	
		}
		else
		{
			if (m_ulMaxCollet2Count > 0)	//v3.92
			{
				m_ulCollet2Count++; 
			}	
		}
	}
*/
#else
	if (m_ulMaxColletCount > 0)
	{
		m_ulColletCount++;
		CString szColletCount;
		szColletCount.Format("%d",m_ulColletCount);
		(*m_psmfSRam)["BinTable"]["ColletUsageCount"] = szColletCount;
	}	
#endif

#ifndef NU_MOTION
	if (m_ulMaxEjectorCount > 0)
	{
		m_ulEjectorCount++;
		if (m_bResetEjKOffsetAtWaferEnd)	//v4.43T8	//Semitek
		{
			m_ulEjectorCountForKOffset++;
		}
	}
#endif

	m_dLastPickTime	= GetTime();	//	cycle update counter
	if (m_ulMaxCleanCount > 0)
	{
		m_ulCleanCount++;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bLumileds = FALSE;
	if (pApp->GetCustomerName() == "Lumileds")
	{
		bLumileds = TRUE;
	}

	//v3.68T3
	if (m_bEnableCustomCount1 && !bLumileds)	//v3.76
	{
		m_ulCustomCounter1++;
	}
	if (m_bEnableCustomCount2)
	{
		m_ulCustomCounter2++;
	}

	if (pApp->GetCustomerName() != "Electech3E(DL)")	//v4.48A2
	{
		if (m_bEnableCustomCount3)
		{
			m_ulCustomCounter3++;
		}
	}

	if (m_pulStorage != NULL)
	{
#ifndef NU_MOTION
		if (m_ulMaxColletCount > 0)
		{
			*(m_pulStorage + BH_COLLETCOUNT_OFFSET) = m_ulColletCount;
		}
		if (m_ulMaxCollet2Count > 0)
		{
			*(m_pulStorage + BH_COLLET2COUNT_OFFSET) = m_ulCollet2Count;    //v3.92		//MS100
		}	
		if (m_ulMaxEjectorCount > 0)
		{
			*(m_pulStorage + BH_EJECTCOUNT_OFFSET)	= m_ulEjectorCount;
		}
#endif
		if (m_ulMaxCleanCount > 0)
		{
			*(m_pulStorage + BH_CLEANCOUNT_OFFSET)	= m_ulCleanCount;
		}

		//v3.68T3
		if (m_bEnableCustomCount1)
		{
			*(m_pulStorage + BH_CUSTOM_COUNT_1)		= m_ulCustomCounter1;
		}
		if (m_bEnableCustomCount2)
		{
			*(m_pulStorage + BH_CUSTOM_COUNT_2)		= m_ulCustomCounter2;
		}
		if (pApp->GetCustomerName() != "Electech3E(DL)")	//v4.48A2
		{
			if (m_bEnableCustomCount3)
			{
				*(m_pulStorage + BH_CUSTOM_COUNT_3)		= m_ulCustomCounter3;
			}
		}
	}

	return TRUE;
}

	
INT	CBondHead::OpUpdateMachineCounters2()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
	BOOL bLumileds = FALSE;
	if (pApp->GetCustomerName() == "Lumileds")
	{
		bLumileds = TRUE;
	}

	if (bLumileds)
	{
		if (m_bEnableCustomCount1)
		{
			m_ulCustomCounter1++;
			if (m_pulStorage != NULL)
			{
				*(m_pulStorage + BH_CUSTOM_COUNT_1)	= m_ulCustomCounter1;
			}
		}
	}

	return TRUE;
}

VOID CBondHead::OpUpdateMS100EjectorCounter(INT nState)
{
	if (m_ulMaxEjectorCount > 0)
	{
		m_ulEjectorCount++;
		*(m_pulStorage + BH_EJECTCOUNT_OFFSET)	= m_ulEjectorCount;

		if (m_bResetEjKOffsetAtWaferEnd)			//v4.43T8	//Semitek
		{
			m_ulEjectorCountForKOffset++;
		}

		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
		//if (pUtl->GetPrescanRegionMode())
		{
			m_ulEjCountForSubRegionKOffset++;			//v4.45T4
			if ( (m_lEjSubRegionSKCount > 0) && (m_lEjSubRegionSKOffset > 0) )
			{
				m_ulEjCountForSubRegionSKOffset++;		//v4.46T1
			}

			if (pUtl->GetPrescanRegionMode() && pSRInfo->IsRegionEnding())
			{
				m_ulEjCountForSubRegionKOffset = 0;
				m_ulEjCountForSubRegionSKOffset = 0;
				m_lEjSubRegionSmartValue = 0;
				pSRInfo->SetRegionEnding(FALSE);
				SaveScanTimeEvent("BH: REGION END reset k offset auto sort");
//CMSLogFileUtility::Instance()->MS_LogOperation("Ej-SubRegion-KOffset is reset in AUTOBOND sequence");
			}
		}
	}
}


VOID CBondHead::OpUpdateMS100ColletCounter()
{
	if( IsBurnIn() )
	{
		return ;
	}
	if (IsBHZ1ToPick())		//If BHZ1 at PICK		
	{
		if (m_ulMaxColletCount > 0)
		{
			m_ulColletCount++; 
			*(m_pulStorage + BH_COLLETCOUNT_OFFSET) = m_ulColletCount;
			CString szColletCount;
			szColletCount.Format("%d",m_ulColletCount);
			(*m_psmfSRam)["BinTable"]["ColletUsageCount"] = szColletCount;
		}	
	}
	else
	{
		if (m_ulMaxCollet2Count > 0)
		{
			m_ulCollet2Count++; 
			*(m_pulStorage + BH_COLLET2COUNT_OFFSET) = m_ulCollet2Count; 
			CString szCollet2Count;
			szCollet2Count.Format("%d",m_ulCollet2Count);
			(*m_psmfSRam)["BinTable"]["Collet2UsageCount"] = szCollet2Count;
		}	
	}
}


BOOL CBondHead::CheckCoverOpenInAuto(CString szEvent)
{
	INT nCoverCheckCount = 0;
	while (IsCoverOpen())
	{
		SetErrorMessage("Machine Cover Open detected in auto when " + szEvent);
		SetAlert_Red_Back(IDS_BH_COVER_OPEN);
		Sleep(500);
		nCoverCheckCount++;
		if (nCoverCheckCount > 10)		//3)	//v4.31T9
		{						
			LONG lHmiStatus = HmiMessage("Cover sensor still open\n Press Continue to continue to check sensor?\n Press Stop to stop to check sensor and bond arm will move!", 
										 "BondHead", glHMI_MBX_CONTINUESTOP);
			
			if (lHmiStatus != glHMI_CONTINUE)
			{
				HmiMessage("Bond arm will start to move!");
				return FALSE;
			}
			//break;
		}
	}
	return TRUE;
}

BOOL CBondHead::CheckCoverOpenInManual(CString szEvent)
{
	INT nCount = 0;
	
	while (IsCoverOpen())
	{
		SetErrorMessage("Machine Cover Open detected in manual when " + szEvent);

		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		nCount++;
		if (nCount >= 3)
		{
			LONG lHmiStatus = HmiMessage("Cover sensor still open! continue?", 
										 "BondHead", glHMI_MBX_CONTINUESTOP | 0x80000000);
			
			if (lHmiStatus != glHMI_CONTINUE)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CBondHead::LoopCheckCoverOpen(CString szEvent)
{
	INT nCount = 0;
	while (IsCoverOpen())
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		CString szMsg = "Machine cover open detected in " + szEvent;
		SetErrorMessage(szMsg);

		nCount++;
		if (nCount >= 3)
		{
			szMsg.Format("Cover sensor still open in %s; continue to move T motor?", szEvent);
			LONG lHmiStatus = HmiMessage_Red_Yellow(szMsg, "BondHead Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
			if (lHmiStatus != glHMI_CONTINUE)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

INT CBondHead::T_SafeMoveToInAuto(INT nPos, CString szWhere)
{
	CheckCoverOpenInAuto(szWhere);
	return T_MoveTo(nPos, SFM_WAIT);
}

INT CBondHead::T_SafeMoveToLoop(INT nPos, CString szWhere)
{
	if( LoopCheckCoverOpen("Start BArm Motion Test") == FALSE)
	{
		return FALSE;
	}

	T_MoveTo(nPos, SFM_WAIT);
	return TRUE;
}

VOID CBondHead::OpDisplayAlarmPage(BOOL bDisplay)		//v4.53A13
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName() == "SanAn" ||
		pApp->GetCustomerName() == "Semitek" ||
		pApp->GetFeatureStatus(MS896A_FUNC_ENABLE_BOND_ALARM_PAGE) )
	{
		m_bShowAlarmPage = bDisplay;
	}
}


VOID CBondHead::LogPBFailureCaseDumpFile()
{
	CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
	if (pPRFailureCaseLog->IsEnableFailureCaselog())
	{
		CloseAlarm();
		CString szMsg;
		szMsg = "Log PR Failure Case...";
		SetAlert_WarningMsg(IDS_MS_WARNING_LOG_PR_FAILURE_CASE, szMsg);
		pPRFailureCaseLog->LogFailureCaseDumpFile(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, 5);
		if (State() == IDLE_Q)
		{
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
		else
		{
			SetAlarmLamp_Green(FALSE, TRUE);
		}
		CloseAlarm();
	}
}

VOID CBondHead::GetWaferTableEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lT;
	} ENCVAL;

 	ENCVAL stEnc;

	int nConvID = 0;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetEncoderCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
	
			*lXAxis = stEnc.lX;
			*lYAxis = stEnc.lY;
			*lTAxis = stEnc.lT;
			break;
		}
		else
		{
			Sleep(10);
		}	
	}
}
