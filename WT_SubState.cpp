/////////////////////////////////////////////////////////////////
// WT_SubState.cpp : SubState for Auto-cycle of the CWaferTable class
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_Log.h"
#include "math.h"
#include "PrescanUtility.h"
#include "BH_Constant.h"
#include "PrescanInfo.h"
#include "MarkDieRegionManager.h"
#include "BondPr.h"
#include "BondHead.h"
#include "WaferPr.h"
#include "BinTable.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Sequence Operations
////////////////////////////////////////////
VOID CWaferTable::BackupBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bEnablePrescan_Backup = m_bEnablePrescan;
		m_bEnablePrescan = FALSE;

		m_bMS90HalfSortMode_Backup = m_bMS90HalfSortMode;
		m_bMS90HalfSortMode = FALSE;
	}
}

VOID CWaferTable::RestoreBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bEnablePrescan	= m_bEnablePrescan_Backup;
		m_bMS90HalfSortMode = m_bMS90HalfSortMode_Backup;
	}
}

LONG CWaferTable::GetDiePROffsetX() const
{
	return (LONG)(*m_psmfSRam)["WaferPr"]["Compensate"]["X"];
}

LONG CWaferTable::GetDiePROffsetY() const
{
	return (LONG)(*m_psmfSRam)["WaferPr"]["Compensate"]["Y"];
}

double CWaferTable::GetDiePROffsetT() const
{
	return (*m_psmfSRam)["WaferPr"]["Compensate"]["Theta"];
}

BOOL CWaferTable::IsDiePRRotate()
{
	return (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["Compensate"]["Rotate"];
}

INT CWaferTable::OpInitialize()
{
	INT nResult					= gnOK;

	{
		m_qPreviousSubOperation	= -1;
		m_qSubOperation			= 0;

		NeedReset(FALSE);
	}

	DisplaySequence("WFT - Operation Initialize");
	return nResult;
}

INT CWaferTable::OpPreStart()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	INT nResult = gnOK;
	SaveScanTimeEvent("    WFT: opprestart begin");
	if (IsEnablePNP())
	{
		SetBlkFuncEnable(FALSE);
	}

	m_bForceRescan = FALSE;
	//m_ulRescanAtBondDieCount = m_ulBondDieCount;

	if( m_lWftAdvSamplingResult!=-1 )
		m_lWftAdvSamplingResult = 0;

	m_lLFPitchErrorCount	= 0;
	m_lLFPitchErrorCount2	= 0;
	m_szNextLocation		= "";
	m_ulPitchAlarmGoCheckLimit	= 0;
	m_ulPitchAlarmGoCheckCount	= 999;
	m_bPitchError			= FALSE;
	m_bVerifyMapWaferFail	= FALSE;
	m_lVerifyMapWaferRow	= -10000;
	m_lVerifyMapWaferCol	= -10000;
	
	if (!m_fHardware || m_bDisableWT)
	{
		m_bHome_X = TRUE;
		m_bHome_Y = TRUE;
		m_bComm_X = TRUE;
		m_bComm_Y = TRUE;
		m_bHome_T = TRUE;
		m_bComm_T = TRUE;
	}

	if( m_lRunPredScanPitchTolX < m_lPredScanPitchTolX )
		m_lRunPredScanPitchTolX = m_lPredScanPitchTolX;
	if( m_lRunPredScanPitchTolY < m_lPredScanPitchTolY )
		m_lRunPredScanPitchTolY = m_lPredScanPitchTolY;

	CString szLogMsg;
	szLogMsg.Format("    WFT: %s; predict=%d DEB(%d); ADV(match%d); RSN(All%d);", 
		GetMapNameOnly(), m_lPredictMethod, DEB_IsUseable(), IsVerifyMapWaferEnable(), m_bAutoRescanAllDice);
	SaveScanTimeEvent(szLogMsg);
	SetErrorMessage(szLogMsg);

	m_bMapWaferVerifyOK	= TRUE;
	m_ulVerifyMapWaferCounter	= 0;
	m_bLastDieUpdate	= FALSE;
	m_bGetAvgFOVOffset	= FALSE;
	if (m_bPrescanStarted == FALSE && IsPrescanEnable())
	{
		CString szMsg;
		CTime ctDateTime;
		ctDateTime = CTime::GetCurrentTime();
		szMsg = m_szMapFileName + "," + ctDateTime.Format("%Y%m%d%H%M%S,");
		LogScanDetailTime(szMsg);		//	1.	wafer id.	//	2.	scan start date and time.
		szMsg.Format("%ld,", m_lBackLightTravelTime);
		LogScanDetailTime(szMsg);			//	3.	BL travel time.
		m_dScanStartTime		= GetTime();
		m_stScanStartCTime		= CTime::GetCurrentTime();
		m_bPrescanStarted = TRUE;
		CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(6);	//MAPPING_START		//v4.40T11
	}	// log scan time into excel file
	else
	{
		CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(20);	//SORT_START2		//anichia007
	}

	// Temperature Controller
	if (TC_IsEnable() && TC_IsOnPower())
	{
		TC_TurnPowerOn(TRUE);
	}

	m_dIdleDiffTime = GetTime();	//	427TX	1
	CMSLogFileUtility::Instance()->MS_LogCycleState("start sorting");	//	427TX	1
	m_bScanPreunloadTable = FALSE;	//	4.24TX 4
	SetJoystickOn(FALSE);
	SetEjectorVacuum(FALSE);

// prescan relative code	B
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	pUtl->SetPickDefectDie(m_bSortingDefectDie);

	if (pUtl->GetPrescanAreaPickMode())
	{
		// Save the data to SRAM String Map File
		(*m_psmfSRam)["WaferMap"]["X"] = 110;
		(*m_psmfSRam)["WaferMap"]["Y"] = 110;
		(*m_psmfSRam)["WaferMap"]["Grade"] = m_ucDummyPrescanPNPGrade + m_WaferMapWrapper.GetGradeOffset();
	}

	if (IsMotionHardwareReady() == TRUE)
	{
// prescan relative code	B
		if (IsPrescanning())
		{
#ifdef NU_MOTION
			INT nResult = 0;
			if (IsES101() || IsES201())
			{
				nResult = X_Profile(NORMAL_PROF);
				nResult = Y_Profile(NORMAL_PROF);
				if (IsWT2InUse())
				{
					m_lWT2PositionStatus = 2;
				}
				else
				{
					m_lWT1PositionStatus = 2;
				}
			}
			else
			{
				nResult = X_Profile(LOW_PROF);
				nResult = Y_Profile(LOW_PROF);
			}
			if (nResult != gnOK)
			{
				AfxMessageBox("Fail to access SLOW WT profile ....", MB_SYSTEMMODAL);
			}
#endif
			BOOL bBLModule = (m_bDisableBL == FALSE);
			CString	szMachineModel		= (*m_psmfSRam)["MS896A"]["Machine Model"];
			if (szMachineModel.Find("STD") != -1)
			{
				bBLModule = FALSE;
			}
			if (pUtl->GetPrescanAreaPickMode())
			{
				bBLModule = FALSE;
			}

			if (bBLModule)
			{
				SaveScanTimeEvent("    WFT: down BL to ready begin");
				IPC_CServiceMessage svMsg;
				int nConvID = 0;
				BOOL bStatus;
				nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "DownElevatorZToReady", svMsg);
				while (pUtl->GetPrescanRegionMode()==FALSE)
				{
					if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
					{
						m_comClient.ReadReplyForConvID(nConvID, svMsg);
						svMsg.GetMsg(sizeof(BOOL), &bStatus);
						break;
					}
					else
					{
						Sleep(10);
					}
				}
			}

			//	move down after region alignment done
			// to prescan, move to last position q
			if (pUtl->GetPrescanAreaPickMode() && m_lAreaPickStage >= 2)
			{
				m_qSubOperation	= PRESCAN_AP_NEXT_DIE_Q;
				DisplaySequence("WFT - scan AP prestart to pick next left die q");
			}
			else
			{
				m_qSubOperation	= PRESCAN_MOVE_TABLE_Q;
				DisplaySequence("WFT - scan to move table");
			}
			m_dCurrTime = 0.0;
			m_dStopTime = GetTime();
		}
		else	// non prescanning
		{
			m_qSubOperation	= GET_WAFER_POS_Q;
			m_bPrescanningMode = FALSE;
			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);
			T_Profile(NORMAL_PROF);
		}
// prescan relative code	E
	}
	else
	{
		pUtl->RegionOrderLog("REGN hardwafer error in wafer table");
		m_qSubOperation	= HOUSE_KEEPING_Q;
		m_bPrescanningMode = FALSE;
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
		T_Profile(NORMAL_PROF);
		HmiMessage_Red_Back("Motion hardware not ready!", "Wafer Table");
	}

	SetWTStable(FALSE);
	SetWTReady(FALSE,"8");
	SetWTBacked(FALSE);
	SetMoveBack(FALSE);
	SetDieInfoRead(TRUE);
	SetDieReadyForBT(FALSE);
	SetWTStartMove(FALSE);		//v3.66	//DBH
	SetCompDone(FALSE);			//v4.34T10
	SetWTReadyForWPREmptyCheck(FALSE);	//v4.54A5
	SetWPREmptyCheckDone(FALSE);		//v4.54A5
	SetWTStartToMoveForBT(FALSE, "PreStart");

	m_bMS60NGPickWaitCheckEmptyDie = FALSE;

	m_bFirst			= TRUE;
	m_bGoodDie			= FALSE;
	m_bGoodDieFound		= FALSE;
	m_bConfirmSrch		= FALSE;
	m_bMoveBack			= FALSE;
	m_bIsUnload			= FALSE;
	m_bDieIsGet			= FALSE;
	m_szNextPath		= " ";
	m_lOrgDieCompen_X	= 0;
	m_lOrgDieCompen_Y	= 0;
	m_bPreCompensate	= FALSE;
	m_lX_NoEjOffset		= 0;
	m_lY_NoEjOffset		= 0;
	m_lLastGradeSetToEquip = 0;

	(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = FALSE;	//	prestart
	m_bThisDieNeedVerify	= TRUE;
	m_bThisDieNotLFed		= FALSE;
	m_ulPickingBlockIndex	= 0;
	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	ULONG  ulFovHalf	= (ULONG)labs( (LONG)(max(dFOVX, dFOVY) - 0.3) ) / 2;
	if( ulFovHalf<=1 )
		ulFovHalf = 2;

	if (m_ulAdvSampleJumpDistance > (ulFovHalf+1)*2 )
	{
		m_ulAdvSampleJumpDistance = (ulFovHalf+1)*2;
	}
	if (m_ulAdvSampleJumpDistance < (ulFovHalf+1))
	{
		m_ulAdvSampleJumpDistance = ulFovHalf + 1;
	}

	InitLFInfo(m_stLFDie);		// Init Look-Forward Info
	InitLFInfo(m_stLastLFDie);	// Init Look-Forward Info

	(*m_psmfSRam)["WaferMap"]["NextDieDirection"] = -1;
	(*m_psmfSRam)["WaferMap"]["ComingDieDirection"] = -1;
	(*m_psmfSRam)["DieInfo"]["WT_Index"] = 0;
	(*m_psmfSRam)["DieInfo"]["BT_Index"] = 0;
	(*m_psmfSRam)["DieInfo"]["BH_Index"] = 0;
	(*m_psmfSRam)["DieInfo"]["WPR_Index"] = 0;
	(*m_psmfSRam)["DieInfo"]["BPR_Index"] = 0;
	(*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"] = -1;	//MS60
	(*m_psmfSRam)["DieInfo"]["RefDie"]["FaceValue"] = 0;		//v4.48A26	//Avago
	
	//v4.54A5	//MS60 NGPick mode
	(*m_psmfSRam)["WaferMap"]["LastDieDirection"] = -1;	
	(*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"]	= 0;
	(*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"]	= 0;
	(*m_psmfSRam)["BondHead"]["First Pick Die"] = FALSE;	//v4.54A5

	//v2.96T2
	(*m_psmfSRam)["WaferMap"]["NgPick"]["PickLeft"] = 0;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"]		= 0;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"]		= 0;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"]	= 0;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"]	= WAF_CDieSelectionAlgorithm::CHECK;
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Row"]		= 0;
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Col"]		= 0;
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Grade"]	= 0;
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Action"]	= WAF_CDieSelectionAlgorithm::CHECK;

	//BLOCKPICK
	(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosY"]		= 0;

	GetEncoderValue();		
	(*m_psmfSRam)["WaferTable"]["Current"]["Theta"]		= GetCurrT();
	(*m_psmfSRam)["WaferTable"]["Current"]["X"]			= GetCurrX();
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"]			= GetCurrY();
	//andrewng //2020-0619
	(*m_psmfSRam)["WaferTable"]["WTEncX"]				= GetCurrX();
	(*m_psmfSRam)["WaferTable"]["WTEncY"]				= GetCurrY();

	(*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"]	= 0;		//v4.15T2
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"]	= 0;		//v4.15T2
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"]	= 0;
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"]= WAF_CDieSelectionAlgorithm::CHECK;

	(*m_psmfSRam)["WaferTable"]["CompInLastCycle"]	= FALSE;
	m_bDoCompInLastCycle	= FALSE;

	//v4.47T3
	m_bUseMS60NewLFCycle	= FALSE;
	(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"]	= FALSE;
	m_bMS60NewLFInLastCycle	= FALSE;
	m_stMS60NewLFDie.lX_Enc	= 0;
	m_stMS60NewLFDie.lY_Enc = 0;
	m_stMS60NewLFDie.lT_Enc = 0;
	m_stMS60LastLFDie.lX_Enc= 0;
	m_stMS60LastLFDie.lY_Enc= 0;
	m_stMS60LastLFDie.lT_Enc= 0;
	m_nMS60CycleCount		= 0;
	SetWTAtColletPos(0);
	m_nWTLastColletPos		= 0;
	m_nChangeColletOnWT		= FALSE;

	m_bSyncWTAndEjectorInBond	= TRUE;

	(*m_psmfSRam)["WaferTable"]["WaferFirstPartDone"]	= FALSE;
	(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"]	= FALSE;
	(*m_psmfSRam)["WaferTable"]["WaferEnd"]			= FALSE;
	(*m_psmfSRam)["WaferTable"]["VerifyRefDieFail"] = FALSE;
	(*m_psmfSRam)["WaferTable"]["Rotate"]			= FALSE;		//v3.62
	(*m_psmfSRam)["WaferTable"]["AtSafePos"]		= 0;			//v3.67T3
	(*m_psmfSRam)["WaferTable"]["LookForward"]["NeedRotate"]	= FALSE;	//v4.43T8
	if (IsLayerPicking())	// protection, no new ejector sequence
	{
		m_bNewEjectorSequence = FALSE;
	}
	(*m_psmfSRam)["WaferTable"]["NewEjSequence"]	= m_bNewEjectorSequence;	//v3.85
	(*m_psmfSRam)["WaferTable"]["IndexStart"]		= 0;			//v3.85
	(*m_psmfSRam)["WaferTable"]["IndexTime"]		= 2000;	// prestart
	m_bReadyToRealignOnWft	= FALSE;

	SetAlarmLamp_Green(FALSE, TRUE);	

	ResetBondRecord();

	m_lStepBurnInT = 100;		//BurnInT
	m_lCountBurnInT = 0;

	if (IsBlkFunc2Enable())	//Block2
	{
		m_pBlkFunc2->Blk2SetBondingLastDie();
	}
	m_pBlkFunc2->Blk2SetBlock4ReferCheckScore(m_lBlk4ReferCheckScore);

	//update Pitch & key code into wafermap (for SmartWalk only)
	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	lDiePitchX_X = -1 * labs(lDiePitchX_X);
	lDiePitchY_Y = -1 * labs(lDiePitchY_Y);
	m_WaferMapWrapper.SetAlgorithmParameter("X Pitch", lDiePitchX_X);
	m_WaferMapWrapper.SetAlgorithmParameter("Y Pitch", lDiePitchY_Y);
	m_WaferMapWrapper.SetAlgorithmParameter("Bond Cycle", 0x47505055);
	//m_WaferMapWrapper.SetAlgorithmParameter("Single Dir", TRUE);

	//BLOCKPICK
	(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBin"] = FALSE;
	(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBinTo"] = 0;
	(*m_psmfSRam)["WaferPr"]["LookAround"]["NextRow"]	= 0;
	(*m_psmfSRam)["WaferPr"]["LookAround"]["NextCol"]	= 0;
	(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieTNeedRotate"] = (BOOL) TRUE;
	m_bFirstCycleWT		= TRUE;
	m_bFirstInAutoCycle = TRUE;	//	pre-start

	OpSkipSEPGradeDie();
// prescan relative code	B
	(*m_psmfSRam)["WaferTable"]["WTReady"]	= 0;
	pSRInfo->SetRegionManualConfirm(FALSE);
	INT nFindOK = gnNOTOK;

	if ((pUtl->GetPrescanRegionMode() && IsPrescanning() && !pSRInfo->IsRegionAligned()) && GetWftSampleState() == 0)
	{
		INT nRegnFind = gnNOTOK;

		pSRInfo->SetRegionEnding(TRUE);	// to trigger BH reset ejector region k offset.
		CString szErr;

		m_pWaferMapManager->DrawAllRegionBoundary();
		szErr = "WFT: REGN pre start to begin finder reset k offset";
		pUtl->RegionOrderLog(szErr);
		SaveScanTimeEvent(szErr);

		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		if (pSRInfo->GetRegionState(ulTgtRegion) == WT_SUBREGION_STATE_BONDED)
		{
			szErr.Format("Current region %d was BONDED, please check region state", ulTgtRegion);
			HmiMessage_Red_Yellow(szErr);
			pUtl->RegionOrderLog(szErr);
			//2018.07.27
			// can not stop bonding and hang up machine
		}
		else
		{
			if (pSRInfo->IsManualAlignRegion())
			{
				if (AutoRegionAlignManual())
				{
					pUtl->RegionOrderLog("REGN manual pre start region finder ok");
					nRegnFind = gnOK;
					SetWTStable(TRUE);
					m_qSubOperation	= PRESCAN_WAIT_DIE_READY_Q;
					DisplaySequence("WT - scan prestart to wait die ready");
				}
			}
			else
			{
				if (IsRegionAlignRefBlk())
				{
					// switch to normal mode of 5M zoom.
					IPC_CServiceMessage stMsg;
					BOOL bToScan = FALSE;
					BOOL bDone = TRUE;
					for(int i=0; i<400; i++)
					{
						stMsg.InitMessage(sizeof(BOOL), &bToScan);
						int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleAutoZoomViewCmd", stMsg);
						while (1)
						{
							if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
							{
								m_comClient.ReadReplyForConvID(nConvID, stMsg);
								stMsg.GetMsg(sizeof(BOOL), &bDone);
								break;
							}
							else
							{
								Sleep(10);
							}
						}
						if( bDone )
						{
							break;
						}
						Sleep(50);
					}	// switch to normal mode
					
					if (AutoRegionAlignRefBlk())
					{
						pUtl->RegionOrderLog("REGN pre start region finder ok");
						nRegnFind = gnOK;
						SetWTStable(TRUE);
						m_qSubOperation	= PRESCAN_WAIT_DIE_READY_Q;
						DisplaySequence("WT - scan prestart to wait die ready");
					}

					// switch to scn mode of 5M zoom
					bToScan = TRUE;
					for(int i=0; i<40; i++)
					{
						stMsg.InitMessage(sizeof(BOOL), &bToScan);
						int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ToggleAutoZoomViewCmd", stMsg);
						while (1)
						{
							if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
							{
								m_comClient.ReadReplyForConvID(nConvID, stMsg);
								stMsg.GetMsg(sizeof(BOOL), &bDone);
								break;
							}
							else
							{
								Sleep(10);
							}
						}
						if( bDone )
						{
							break;
						}
						Sleep(50);
					}	// swtich back to prescan mode
				}
				else
				{
					HmiMessage_Red_Back("Manual align target region.\nBefore start sorting.", "Region Prescan");
				}
			}
		}

		if (nRegnFind != gnOK)
		{
//			SetMotionCE(TRUE,"REGN prestart region finder fail");
			m_qSubOperation	= HOUSE_KEEPING_Q;
			nResult = gnNOTOK;
			pUtl->RegionOrderLog("REGN prestart region finder fail");
		}
	}

//HmiMessage_Red_Back("Pre start failure", "Region Prescan");
//m_qSubOperation	= HOUSE_KEEPING_Q;
//nResult = gnNOTOK;

	if (IsPrescanning() && GetNewPickCount()==0 && GetWftSampleState()==0 )
	{
		//	move down after region aligment done, then to first start point
		if (pUtl->GetPrescanAreaPickMode() && m_lAreaPickStage >= 2)
		{
		}
	//	else if ((pUtl->GetPrescanRegionMode() && IsPrescanning() && !pSRInfo->IsRegionAligned()))
	//	{
	//	}
		else
		{
			X_Sync();
			Y_Sync();
			GetEncoderValue();
			m_lLastScanGrabPosnX = GetCurrX();
			m_lLastScanGrabPosnY = GetCurrY();

			XY_MoveToPrescanPosn();
			Sleep(500);
		}
		T_MoveTo(GetGlobalT(), SFM_WAIT);
		SaveScanTimeEvent("    WFT: opprestart move to last scan position");
		m_nPrescanNextWftPosnX	= GetScanLastPosnX();	// index to prescan
		if (pUtl->GetPrescanAreaPickMode() == FALSE)
		{
			m_nPrescanNextWftPosnY	= m_nPrescanLastWftPosnY;    // index to prescan
		}	
		m_lPrescanLastGrabRow	= m_nPrescanNextMapRow	= m_nPrescanLastMapRow;	// index to prescan
		m_lPrescanLastGrabCol	= m_nPrescanNextMapCol	= m_nPrescanLastMapCol;	// index to prescan
		m_lNextFrameRow = GetLastFrameRow();
		m_lNextFrameCol = GetLastFrameCol();

		if (IsPrescanMapIndex() || pUtl->GetPrescanAreaPickMode())	// update map display when move table
		{
			m_bPrescanLastGrabbed	= TRUE;
			pUtl->SetPrescanMapIndexIdle(TRUE);
		}

		// ES101 under prescanning, up back light
		if ((IsBLInUse()) && IsAlignedWafer())
		{
			X_Sync();
			Y_Sync();
			GetEncoderValue();
			if (IsWithinWaferLimit(GetCurrX(), GetCurrY()))
			{
				SaveScanTimeEvent("    WFT: opprestart up back light");
				MoveES101BackLightZUpDn(TRUE);	// in prestart, if align ok, check current is safe, UP BL Z

				if (IsWprWithAF())
				{
					BOOL bWft = FALSE;
					if (IsWT1UnderCamera())
					{
						bWft = FALSE;
					}
					if (IsWT2UnderCamera())
					{
						bWft = TRUE;
					}
					MoveFocusToWafer(bWft);
				}
			}
			SaveScanTimeEvent("    WFT: opprestart after down focus and up back light");
		}

		BOOL bGoOn = FALSE;
		CString szAlgorithm, szPathFinder;
		m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
		if( szAlgorithm.Find("Sorting Path 1")!=-1 )
		{
			if( m_lBuildShortPathInScanning==0 )	// prestart, short path, first time trigger.
			{
				m_lBuildShortPathInScanning = 1;
				CString szMsg = "WFT: trigger normal short path building in prestart";
				SaveScanTimeEvent(szMsg);
				pUtl->RegionOrderLog(szMsg);
			}
		}
		else if( m_bFirstMapIsOldMap )
		{
			m_bPrescanWaferEnd4ShortPath = TRUE;	//	old map, first time start, not short path
		}

		m_bFirstMapIsOldMap = FALSE;
	}

// prescan relative code	E

	if (IsBlkFunc1Enable())
	{
		m_lCurrBlockNum = 0;		//v3.30T3
		m_bBlkLongJump = FALSE;
	}

// Temperature Controller
	CString szMsg;
	if (TC_IsEnable() && TC_IsOnPower())
	{
		LONG lCount = 0;
		szMsg.Format("to check TC reach to setting temperature");
		SetErrorMessage(szMsg);
		while (1)
		{
			if (TC_CheckEjectorLimit() == 0)
			{
				break;
			}
			else
			{
				Sleep(500);
			}
			lCount++;
			if (lCount >= 100)
			{
				break;
			}
		}
		if (lCount >= 100)
		{
			m_qSubOperation	= HOUSE_KEEPING_Q;
			nResult = gnNOTOK;
			szMsg = "TC power on and tolerance over limit when prestart after 50 seconds!";
			HmiMessage_Red_Yellow(szMsg);
			SetErrorMessage(szMsg);
		}
	}

	pUtl->SetPrAbnormal(FALSE);	// pre start, reset

	(*m_psmfSRam)["BinLoader"]["MoveWftToSafeState"] = 0;
	WftToSafeLog(" wt pre start set state to 0");
	SaveScanTimeEvent("    WFT: opprestart end");
	DisplaySequence("WT - Operation Prestart");

	SetAutoCycleLog(TRUE);	// prestart to enable.

	return nResult;
}


VOID CWaferTable::OpResetLFCycle()
{
/*
//===================New Add=================================
	SetWTStable(FALSE);
	SetWTReady(FALSE);
	SetWTBacked(FALSE);
	SetMoveBack(FALSE);
	SetDieInfoRead(TRUE);
	SetDieReadyForBT(FALSE);
	SetWTStartMove(FALSE);		//v3.66	//DBH
	SetCompDone(FALSE);			//v4.34T10
	SetWTReadyForWPREmptyCheck(FALSE);	//v4.54A5
	SetWPREmptyCheckDone(FALSE);		//v4.54A5

	m_bMS60NGPickWaitCheckEmptyDie = FALSE;

	m_bFirst			= TRUE;
	m_bGoodDie			= FALSE;
	m_bGoodDieFound		= FALSE;
	m_bConfirmSrch		= FALSE;
	m_bMoveBack			= FALSE;
	m_bIsUnload			= FALSE;
	m_bDieIsGet			= FALSE;
	m_szNextPath		= " ";
	m_lOrgDieCompen_X	= 0;
	m_lOrgDieCompen_Y	= 0;
	m_bPreCompensate	= FALSE;

	m_bUseMS60NewLFCycle	= FALSE;
	(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"] = FALSE;
	m_bMS60NewLFInLastCycle	= FALSE;
	m_stMS60NewLFDie.lX_Enc	= 0;
	m_stMS60NewLFDie.lY_Enc = 0;
	m_stMS60NewLFDie.lT_Enc = 0;
	m_stMS60LastLFDie.lX_Enc= 0;
	m_stMS60LastLFDie.lY_Enc= 0;
	m_stMS60LastLFDie.lT_Enc= 0;
//===============================================================
*/
	m_bUseMS60NewLFCycle	= FALSE;
	m_bMS60NewLFInLastCycle = FALSE;
	m_stMS60LastLFDie.lX_Enc	= 0;
	m_stMS60LastLFDie.lY_Enc	= 0;
	m_stMS60LastLFDie.lT_Enc	= 0;
	m_nMS60CycleCount			= 0;//LFDEGUG

//===================New Add=================================
	InitLFInfo(m_stLFDie);		// Init Look-Forward Info
	InitLFInfo(m_stLastLFDie);	// Init Look-Forward Info
//==============================================================

}

////////////////////////////////////////////
//	Get the wafer position
////////////////////////////////////////////

INT CWaferTable::OpGetWaferPos()
{
	INT				nResult	= gnOK;
	unsigned long	ulX = 0, ulY = 0, ulIndex = 0;
	unsigned char   ucGrade;
	BOOL			bEndOfWafer;
	ULONG			ulRefValue = 0;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	DOUBLE			dDegree = 0;

	//v4.54A7	//re-position to BEFORE OpGetWaferPos_Region()
	m_bUseMS60NewLFCycle	= FALSE;		//v4.47T1
	(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"] = FALSE;


	if( pUtl->GetPrescanRegionMode() )
	{
		return OpGetWaferPos_Region();
	}

	if (IsBlkFuncEnable())
	{
		return OpGetWaferPos_BlockPick();	//v4.47T1
	}

	
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	m_bLastDieUpdate		= FALSE;
	m_bFirstCycleWT			= FALSE;
	//m_bUseMS60NewLFCycle	= FALSE;		//v4.47T1
	//(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"] = FALSE;


	BOOL bPickLeft = FALSE;
	bPickLeft = (BOOL)(LONG)((*m_psmfSRam)["WaferMap"]["NgPick"]["PickLeft"]);
	if (m_bAOINgPickPartDie && bPickLeft && GetNewPickCount() != 0)	// to get wafer posn, if still left, no need to peek next
	{
		m_bIsWaferEnded	= FALSE;
	
		ulY		= (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
		ulX		= (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];
		ucGrade	= (UCHAR)(LONG)(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"];
		LONG lAction	= (LONG)(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"];
		eAction = (WAF_CDieSelectionAlgorithm::WAF_EDieAction) lAction;
		dDegree = (*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"];
		m_stCurrentDie.lX = (LONG)ulX;
		m_stCurrentDie.lY = (LONG)ulY;
		m_stCurrentDie.ucGrade = ucGrade;
		m_stCurrentDie.eAction = eAction;
		m_stCurrentDie.dDegree = dDegree;
		return gnOK;
	}
		

	PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "OpGetPos");
	
	CString szLog;
	szLog.Format("OpGetWaferPos Start: (%ld, %ld), Grade=%d, eAction=%d, WaferEND=%d", 
				ulY, ulX, ucGrade, eAction, bEndOfWafer);
	CMSLogFileUtility::Instance()->MS60_Log(szLog);

	if (IsEnablePNP())
	{
		bEndOfWafer = FALSE;
	}

	if (bEndOfWafer)
	{
		if (m_pWaferMapManager->IsUserRegionPickingMode())
		{
			//Get the next region or next grade
			if (m_pWaferMapManager->PrepareNextPickingRegion())
			{
				PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "OpGetPos");
				if (m_pWaferMapManager->PrepareNextSortingRegion())
				{
					WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
					UCHAR ucSortingGrade = pSRInfo->GetSortingRegionGrade();
					CreatePrepareNextRegionStatisticsThread(ucSortingGrade);
				}
			}
		}
		else if( RestartDefectDieSort() )
		{
			PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "defect pick");
		}
	}
	else
	{
		if (!m_bEnableSmartWalk)
		{
			ULONG ulState = GetMapDieState(ulY, ulX);
			ULONG ulPeekCounter = 1;
			WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct = eAction;
			BOOL bLoopFind = FALSE;
			if( IsDieUnpickScan(ulState) )
			{
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
				bLoopFind = TRUE;
			}
			if( bLoopFind )
			{
				UpdMapLastDie("GetWaerPos");	//Update  LAST die
			}

			while (!bEndOfWafer && bLoopFind)
			{
				GetMapNextDie(ulY, ulX, ucGrade, eAction, bEndOfWafer);
				SetMapNextDie(ulY, ulX, ucGrade, eUpAct, ulState);

				ulPeekCounter++;
				if (ulPeekCounter > 1000)
				{
					break;
				}

				//Peek another new die after updating curr die state
				PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "Loop GetWaferPos");	
				eUpAct = eAction;
				ulState = GetMapDieState(ulY, ulX);
				bLoopFind = FALSE;
				if( IsDieUnpickScan(ulState) )
				{
					eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
					bLoopFind = TRUE;
				}
			}
		}
	}

	//CString szLogMsg;

	// xuzhijin_region check the next die is another region, stop sorting and continue align->scan next one
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CString szMsg;

	if (pUtl->GetRegionPickMode())	// region pick xuxuxu
	{
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		BOOL  bSameRegion = pSRInfo->IsWithinThisRegion(ulTgtRegion, ulY, ulX);
		if (bEndOfWafer)
		{
			szMsg.Format("REGN Peek wft end (%2d) die (%3d,%3d).", ulTgtRegion, ulY, ulX);
			RegionPickLog(szMsg);
		}
		else if (bSameRegion == FALSE)
		{
			ulTgtRegion = pSRInfo->GetWithinRegionNo(ulY, ulX);
			pSRInfo->SetTargetRegion(ulTgtRegion);
			szMsg.Format("REGN Peek wft new (%2d) die (%3d,%3d).", ulTgtRegion, ulY, ulX);
			RegionPickLog(szMsg);
		}
	}
	else
	{
		if (GetRegionSortOuter())
		{
			ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
			BOOL  bSameRegion = pSRInfo->IsWithinThisRegion(ulTgtRegion, ulY, ulX);
			if (bEndOfWafer)
			{
				pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDED);
				SaveRegionStateAndIndex();
			}
			else if (bSameRegion == FALSE)
			{
				pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDED);
				ulTgtRegion = pSRInfo->GetWithinRegionNo(ulY, ulX);
				pSRInfo->SetTargetRegion(ulTgtRegion);		// region pick xuxuxu
				pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDING);
				SaveRegionStateAndIndex();
			}
		}
	}

	//v2.99T1
	//SmartWalk Check die state for curr die
	if (m_bEnableSmartWalk && !bEndOfWafer)
	{
		ULONG ulState = GetMapDieState(ulY, ulX);
		while (!bEndOfWafer &&
				((ulState == WT_MAP_DS_PR_DEFECT)	|| 
				 (ulState == WT_MAP_DS_PR_INK)		||
				 (ulState == WT_MAP_DS_PR_CHIP)		||
				 (ulState == WT_MAP_DS_PR_BADCUT)	||
				 IsDieEmpty(ulState)		||
				 (ulState == WT_MAP_DS_PR_ROTATE)	||
				 IsDieUnpickAll(ulState)))
		{
			CMSLogFileUtility::Instance()->WT_GetDieLog("Op Get Wafer Pos Smart Walk bad die");
			GetMapNextDie(ulY, ulX, ucGrade, eAction, bEndOfWafer);

			WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAction = WAF_CDieSelectionAlgorithm::DEFECTIVE;
			ULONG ulUpState = WT_MAP_DS_PR_DEFECT;
			switch (ulState)
			{
			case WT_MAP_DS_PR_DEFECT:
			case WT_MAP_DS_PR_CHIP:
			case WT_MAP_DS_PR_BADCUT:
			case WT_MAP_DS_PR_ROTATE:
				break;
			case WT_MAP_DS_PR_INK:
				eUpAction = WAF_CDieSelectionAlgorithm::INKED;
				break;
			case WT_MAP_DS_PR_EMPTY:
				eUpAction = WAF_CDieSelectionAlgorithm::MISSING;
				break;
			case WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY:
			case WT_MAP_DS_UNPICK_REGRAB_EMPTY:
			case WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT:
			case WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT:
				eUpAction = WAF_CDieSelectionAlgorithm::INVALID;
				ulUpState = ulState;
				break;
			case WT_MAP_DIESTATE_UNPICK:
			default:
				eUpAction = WAF_CDieSelectionAlgorithm::INVALID;
				break;
			}
			SetMapNextDie(ulY, ulX, ucGrade, eUpAction, ulUpState);

			//Peek another new die after updating curr die state
			PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "smart walk");	
			ulState = m_WaferMapWrapper.GetDieState(ulY, ulX);
		}
	}


	(*m_psmfSRam)["WaferPr"]["LookAround"]["X"]			= ulX;
	(*m_psmfSRam)["WaferPr"]["LookAround"]["Y"]			= ulY;
	(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosY"]		= 0;

	if (m_bFirst == FALSE)
	{
		OpIsWithinPolygonLimit(ulY, ulX, ucGrade, eAction, bEndOfWafer);
	}

	if (bEndOfWafer == TRUE)
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("WAFER-END");
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End in WT");
		return Err_WTWaferEnd;
	}
	else
	{
		//Reset wafer end signal
		m_bIsWaferEnded	= FALSE;
	}

	(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["Valid"]	= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= 0;


	if (eAction == WAF_CDieSelectionAlgorithm::PICK)
	{	
		// Store data to Die Info
		ulIndex = (*m_psmfSRam)["DieInfo"]["WT_Index"];
		ulIndex++;
		(*m_psmfSRam)["DieInfo"]["WT_Index"] = ulIndex;

		if (DieIsNoPick(ulY, ulX) == TRUE)
		{
			eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		}

		if (DieIsSpecialRefGrade(ulY, ulX))				//v3.24T1
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"] = 1;		//Tell WPR to align this PICK die with Ref-Die pattern
		}

		m_lRealignDieNum++;		//Block2
	}
	else if (eAction == WAF_CDieSelectionAlgorithm::CHECK)
	{
		BlkFunc2ReferDieCheck(ulY, ulX);
		
		(*m_psmfSRam)["DieInfo"]["RefDie"]["X"]		= ulX;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Y"]		= ulY;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 1;

		if (IsBlkFunc2Enable() && IsCharDieInUse())
		{
			if (m_WaferMapWrapper.HasReferenceFaceValue(ulY, ulX) == TRUE)
			{
				ulRefValue = m_WaferMapWrapper.GetReferenceDieFaceValue(ulY, ulX);
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = ulRefValue / 10;
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = ulRefValue % 10;
			}
			else
			{
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = -1;
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = -1;
			}
		}

		//Check ref die face value, for Avago	//v4.48A26
		if (CMS896AStn::m_bUseRefDieFaceValueCheck == TRUE)
		{
			if (m_WaferMapWrapper.HasReferenceFaceValue(ulY, ulX) == TRUE)
			{
				SHORT sRefFaceValue = 0;
				sRefFaceValue = (SHORT)m_WaferMapWrapper.GetReferenceDieFaceValue(ulY, ulX);
				(*m_psmfSRam)["DieInfo"]["RefDie"]["FaceValue"] = sRefFaceValue;
			}
		}
	}
	else if (eAction == WAF_CDieSelectionAlgorithm::SEPARATION)
	{
		if (m_bCheckSepGrade == TRUE)
		{
			return Err_WTHitSeparationGrade; 
		}
	}
	
	BOOL bCheckAlnRefer = TRUE;
	if( (IsPrescanEnable()) && bCheckAlnRefer )
	{
		LONG lRow, lCol, lX, lY;
		pUtl->GetAlignPosition(lRow, lCol, lX, lY);
		if ((ulX == lCol) && (ulY == lRow) && m_WaferMapWrapper.IsReferenceDie(ulY, ulX) )
		{
			CString szMsg;
			szMsg.Format("HomeDie (%d,%d) is ReferDie in get pos general", ulY, ulX);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			if (eAction == WAF_CDieSelectionAlgorithm::PICK)
			{
				eAction = WAF_CDieSelectionAlgorithm::ALIGN;
			}
			(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"] = 1;
			BlkFunc2ReferDieCheck(ulY, ulX);
		}
	}

	if (m_bJustAlign == TRUE && IsPrescanEnable())
	{
		m_ulNewPickCounter = 0;
	}

	if (eAction == WAF_CDieSelectionAlgorithm::ALIGN)
	{
		if (DieIsSpecialRefGrade(ulY, ulX))				//v3.24T1
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"] = 1;		//Tell WPR to align this PICK die with Ref-Die pattern
		}
	}

	m_stCurrentDie.lX		= (LONG)ulX;
	m_stCurrentDie.lY		= (LONG)ulY;
	m_stCurrentDie.ucGrade	= ucGrade;

	dDegree = (*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"];
	m_stCurrentDie.dDegree	= dDegree;

	if( m_WaferMapWrapper.GetDieState(GetCurrentRow(), GetCurrentCol())== WT_MAP_DIESTATE_SKIP_PREDICTED )
	{
		eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		szMsg.Format("Get Posn %d,%d set to align because it is skipped", ulY, ulX);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	}

	m_stCurrentDie.eAction = eAction;

	//v2.96T2
	// Used in waferPr and pass to IM Vision for logging
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"]		= ulY;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"]		= ulX;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"]	= ucGrade;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"]	= eAction;
	
	if( m_bJustAlign )
	{
		LONG lRow, lCol, lWftX, lWftY;
		pUtl->GetAlignPosition(lRow, lCol, lWftX, lWftY);
		m_lRescanStartRow			= lRow;
		m_lRescanStartCol			= lCol;
		m_lRescanStartWftX			= lWftX;
		m_lRescanStartWftY			= lWftY;
	}

	// Save data to bond record
	//NextBondRecord();

	//v3.89	//MS100 LF die speedup for BH MOVE_T sequence
	if (CMS896AApp::m_lCycleSpeedMode >= 3)	
	{
		if (OpIsCurrentLFDie())
		{
			(*m_psmfSRam)["WaferTable"]["LFDie"] = TRUE;
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;
		}

		//v3.94T3		//DBH only
		if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
		{
			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;
		}
	}

	//v4.47T2
	OpGetWaferPos_CheckLFDie(ulY, ulX);

	return nResult;
}


INT CWaferTable::OpGetWaferPos_CheckLFDie(unsigned long ulY, unsigned long ulX)
{
	INT	nResult	= gnOK;

	unsigned long ulNextX = 0, ulNextY = 0;
	unsigned long ulNextNextX = 0, ulNextNextY = 0;
	BOOL bUseLookForward		= IsUseLF();
	LONG lNextDieDirection		= -1;
	LONG lNextNextDieDirection	= -1;
	LONG lFOVMode				= 0;
	CString			szLog;
	unsigned char   ucGrade;
	BOOL			bEndOfWafer;
	ULONG			ulRefValue = 0;
	//CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	szLog.Format("WT OpGetWaferPos_CheckLFDie : LF=%d", bUseLookForward);
	CMSLogFileUtility::Instance()->MS60_Log(szLog);

	if (bUseLookForward != TRUE)
	{
		CMSLogFileUtility::Instance()->MS60_Log(szLog);
		return nResult;
	}


	// Get one more die to determine the direction
	PeekMapNextDie(2, ulNextY, ulNextX, ucGrade, eAction, bEndOfWafer, "Check LF Die");

	szLog.Format("WT OpGetWaferPos_CheckLFDie : cur(%ld, %ld), Next(%ld, %ld), Grade=%d, eAction=%d, WaferEND=%d", 
				ulY, ulX, ulNextY, ulNextX, ucGrade, eAction, bEndOfWafer);
	CMSLogFileUtility::Instance()->MS60_Log(szLog);

	// Remark: Next Die must be exactly the next one. 
	if (bEndOfWafer == FALSE)
	{
		//2018.5.29
		//only handle picking die case for next-next wafer map die
		if (eAction == WAF_CDieSelectionAlgorithm::PICK)
		{
			lNextDieDirection = OpCalculateLFNextDieDirection(ulY, ulX, ulNextY, ulNextX, lFOVMode);
			szLog.Format("Op CalculateLFNextDieDirection(lNextDieDirection) : cur(%ld, %ld), Next(%ld, %ld), lNextDieDirection(%d), FOVMode,%d", 
						ulY, ulX, ulNextY, ulNextX, lNextDieDirection, lFOVMode);
			CMSLogFileUtility::Instance()->MS60_Log(szLog);
			//v4.59A15	//SanAn & Semitek
			//Requested by Roy Ip to disable LF for first few grade for improvement
			// of placement;
			if (CheckIfDisableLFSortGrade(ucGrade))
			{
				lNextDieDirection = -1;	
			}


			if (IsNGBlock(ucGrade))
			{
				lNextDieDirection = -1;	
				CMSLogFileUtility::Instance()->MS60_Log("Grade 149 disable LF");
			}
		}
		else //if (eAction == WAF_CDieSelectionAlgorithm::CHECK)
		{
			// prevent to use normal die record to check Ref-die in LF wnd
			lNextDieDirection = -1;	
		}
	}

	//v4.54A5	//MS60 NGPick PR LookBackward mode
	if (CMS896AStn::m_bMS60NGPick)
	{
		LONG lLastDieDirection = -1;
		(*m_psmfSRam)["WaferMap"]["LastDieDirection"] = lLastDieDirection;

		if (m_stLastDie.eAction == WAF_CDieSelectionAlgorithm::PICK)
		{
			ULONG ulLastY = m_stLastDie.lY;
			ULONG ulLastX = m_stLastDie.lX;

			lLastDieDirection = OpCalculateLFNextDieDirection(ulY, ulX, ulLastY, ulLastX, lFOVMode);

			(*m_psmfSRam)["WaferMap"]["LastDieDirection"]			= lLastDieDirection;
			(*m_psmfSRam)["WaferMap"]["LastDie"]["LookBackward_X"]	= ulLastX;
			(*m_psmfSRam)["WaferMap"]["LastDie"]["LookBackward_Y"]	= ulLastY;

			LONG lMapRow = ulLastY;
			LONG lMapCol = ulLastX;
			LONG lUserRow=0, lUserCol=0;
			ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);
			(*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"]			= lUserRow;
			(*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"]			= lUserCol;
		}
	}

	if (DieIsNoPick(ulNextY, ulNextX) == TRUE)
	{
		eAction = WAF_CDieSelectionAlgorithm::ALIGN;
	}
	(*m_psmfSRam)["WaferMap"]["ComingDieDirection"] = lNextDieDirection;
	(*m_psmfSRam)["WaferMap"]["LookForward_X"] = ulNextX;
	(*m_psmfSRam)["WaferMap"]["LookForward_Y"] = ulNextY;

	//v2.96T2
	//For IM Vision logging of LookForward Srch-Die fcn
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Row"]		= ulNextY;
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Col"]		= ulNextX;
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Grade"]	= ucGrade;
	(*m_psmfSRam)["WaferMap"]["NextDie"]["Action"]	= eAction;

	if (lNextDieDirection != -1)
	{
		m_lLookForwardCounter++;	//v3.55		//For statistics usage only
	}

	
	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	DOUBLE dFov = min(dFOVX, dFOVY);

	m_bUseMS60NewLFCycle = FALSE;	//v4.54A6
	//m_bNextNextGoodDie = FALSE;
	SYSTEMTIME sysTm;
	GetLocalTime(&sysTm);
	CString szTime;
	szTime.Format("Time:%04d-%02d-%02d (%02d:%02d:%02d.%03d)  ", 
			sysTm.wYear, sysTm.wMonth, sysTm.wDay, sysTm.wHour, sysTm.wMinute, sysTm.wSecond, sysTm.wMilliseconds);

	if ( (m_bWaferPrMS60LFSequence)	&&		//IsMS60()	//v4.47T3
		 m_bWaferPrUseMxNLFWnd		&& 
		 (dFov > WPR_LF_SIZE_5X5)	&&
//		 (m_nMS60CycleCount >= 1) )//LFDEBUG			//v4.47T1	//avoid triggering this mode in first few cycles
//		 (m_nMS60CycleCount >= 3) )		//v4.59A26
		 (m_nMS60CycleCount >= 2) )
	{
		(*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"] = -1;

		BOOL bThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"];

		//2018.5.30
		if (lNextDieDirection != -1)		//If Next LF die is available
		{
			if ( (GetCurrentCol() == m_stLFDie.lWM_X)	&& 
				 (GetCurrentRow() == m_stLFDie.lWM_Y)	&&
				 (m_stLFDie.bGoodDie) && 
				 (!bThetaCorrection || !m_stLFDie.bRotate) )
			{
				//Use current posn saved in m_stLastDie (with die already picked) to 
				// calculate new LookForward window before WT MoveTable;
				ULONG ulLastY = m_stLastDie.lY;
				ULONG ulLastX = m_stLastDie.lX;

				//calculate if this "NEXT-LF die" can also be seen by PR under current WT posn,
				// without moving table to "Current LF die" found in last cycle;
				lNextNextDieDirection = OpCalculateLFNextDieDirection(ulLastY, ulLastX, ulNextY, ulNextX, lFOVMode);
				szLog.Format("lNextNextDieDirection%d Op CalculateLFNextDieDirection : last(%ld, %ld), Next(%ld, %ld), Grade=%d, eAction=%d, WaferEND=%d, FOV,%d", 
							lNextNextDieDirection,ulLastY, ulLastX, ulNextY, ulNextX, ucGrade, eAction, bEndOfWafer, lFOVMode);
				CMSLogFileUtility::Instance()->MS60_Log(szLog + "," + szTime);
				//CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
				if (lNextNextDieDirection != -1)	
				{
					//If it can be seen, then use the new MS60 sequence to LF before movind table!
					m_bUseMS60NewLFCycle = TRUE;
					(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"]		= TRUE;
					(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"]	= TRUE;
					CMSLogFileUtility::Instance()->BPR_Arm1Log("NoComp set to TRUE (get LF)");	


					//Save & record current Enc posn when perform new LF wnd search
					GetEncoderValue();
					m_stMS60NewLFDie.lX_Enc	= m_lEnc_X;
					m_stMS60NewLFDie.lY_Enc	= m_lEnc_Y;
					m_stMS60NewLFDie.lT_Enc	= m_lEnc_T;
					
					szLog.Format("WT - Update m_stMS60NewLFDie (%d,%d,%d) in OpGetWaferPos_CheckLFDie()", 
						m_stMS60NewLFDie.lX_Enc, m_stMS60NewLFDie.lY_Enc, m_stMS60NewLFDie.lT_Enc);
					DisplaySequence(szLog);

					(*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"] = lNextNextDieDirection;
					(*m_psmfSRam)["WaferMap"]["NextNext"]["LookForward_X"]		= ulNextX;
					(*m_psmfSRam)["WaferMap"]["NextNext"]["LookForward_Y"]		= ulNextY;
				
CMSLogFileUtility::Instance()->MS60_Log("");
LONG lNextHmiRow = 0,lNextHmiCol = 0,lLastHmiRow = 0,lLastHmiCol = 0;
ConvertAsmToHmiUser(ulNextY,ulNextX,lNextHmiRow,lNextHmiCol);
ConvertAsmToHmiUser(ulLastY,ulLastX,lLastHmiRow,lLastHmiCol);
szLog.Format("WT LF Use MS60 cycle : LF(%lu, %lu)Hmi(%d,%d), curr(%lu, %lu)Hmi(%d,%d), Dir = %ld, EncXY(%ld, %ld)",
					ulNextY, ulNextX, lNextHmiRow, lNextHmiCol, ulLastY, ulLastX, lLastHmiRow, lLastHmiCol,
					lNextNextDieDirection, m_lEnc_X, m_lEnc_Y);
CMSLogFileUtility::Instance()->MS60_Log(szLog + "," + szTime);
DisplaySequence(szLog);
//HmiMessageEx(szLog);
				}
				else
				{
					//reset count 2018.5.9, it is the same as LONG JUMP case, not do lookahead PR with normal case.
					(*m_psmfSRam)["WaferMap"]["ComingDieDirection"] = -1;
					BOOL bGoodDie = m_stLFDie.bGoodDie;
					ResetLFCycle();

					//m_nMS60CycleCount = 0;
					CMSLogFileUtility::Instance()->MS60_Log("");
					szLog.Format("(lNextNextDieDirection = -1) LF new cycle OFF; row = %ld (%ld), col = %ld (%ld), GD = (%d, %d), Rot = %d", 
						m_stLFDie.lWM_Y, GetCurrentRow(), m_stLFDie.lWM_X, GetCurrentCol(),
						bGoodDie, m_stLFDie.bGoodDie, m_stLFDie.bRotate);
					CMSLogFileUtility::Instance()->MS60_Log(szLog + "," + szTime);
					DisplaySequence(szLog);
				}
			}
			else
			{
				//Else this Next-LF die is out of current PR window, so use original 
				BOOL bGoodDie = m_stLFDie.bGoodDie;
				ResetLFCycle();

				CMSLogFileUtility::Instance()->MS60_Log("");
				szLog.Format("LF new cycle OFF; row = %ld (%ld), col = %ld (%ld), GD = (%d, %d), Rot = %d", 
					m_stLFDie.lWM_Y, GetCurrentRow(), m_stLFDie.lWM_X, GetCurrentCol(),
					bGoodDie, m_stLFDie.bGoodDie, m_stLFDie.bRotate);
				CMSLogFileUtility::Instance()->MS60_Log(szLog + "," + szTime);
				DisplaySequence(szLog);

				//HmiMessageEx(szLog);
			}
		}
		else
		{
			//reset count 2018.5.9, it is the same as LONG JUMP case, not do lookahead PR with normal case.
			(*m_psmfSRam)["WaferMap"]["ComingDieDirection"] = -1;
			BOOL bGoodDie = m_stLFDie.bGoodDie;
			ResetLFCycle();

			//m_nMS60CycleCount = 0;
			CMSLogFileUtility::Instance()->MS60_Log("");
			szLog.Format("(lNextDieDirection = -1) LF new cycle OFF; row = %ld (%ld), col = %ld (%ld), GD = (%d, %d), Rot = %d", 
				m_stLFDie.lWM_Y, GetCurrentRow(), m_stLFDie.lWM_X, GetCurrentCol(),
				bGoodDie, m_stLFDie.bGoodDie, m_stLFDie.bRotate);
			CMSLogFileUtility::Instance()->MS60_Log(szLog + "," + szTime);
			DisplaySequence(szLog);
		}
	}	
	else
	{
		//m_bUseMS60NewLFCycle = FALSE;	//v4.54A5
		CMSLogFileUtility::Instance()->MS60_Log("");
		szLog.Format("LF new cycle OFF: MS60=%d, MxN=%d, FOV=%.1f, Cnt=%d", 
							m_bWaferPrMS60LFSequence, m_bWaferPrUseMxNLFWnd, dFov, m_nMS60CycleCount);
		CMSLogFileUtility::Instance()->MS60_Log(szLog + "," + szTime);
		DisplaySequence(szLog);
	}

	return nResult;
}


INT CWaferTable::OpGetWaferPos_NotOKHandling(INT nCode)
{
	INT	nResult	= gnOK;

	if( IsOnlyRegionEnd() )
	{
		SaveScanTimeEvent("    WFT get wafer position to region end waiting");
		DisplaySequence("WT - get wafer position to region end waiting");
		Sleep(1000);
		LONG lLoop = 0;

		while( 1 )
		{
			if ( m_lBuildShortPathInScanning == 2 && IsBHStopped() == 1 )
			{
				break;
			}

			lLoop++;
			if ( lLoop > 1000 )
				break;
			if (WaitEjectorReady())
			{
				SetEjectorReady(FALSE, "32");
				if ( m_bMoveBack == TRUE )
				{
					m_bMoveBack = FALSE;
				}
				break;
			}
			else if (m_bGoodDie == FALSE)
			{
				LONG lState = (*m_psmfSRam)["BondHead"]["AtPrePick"];
				if (lState == 1)
				{
					SetEjectorReady(TRUE, "Region End, NG, BH at Prepick");
				}
			}
			Sleep(5);
		}

		DisplaySequence("WT - REGN WFT get wafer position to end (wafer region) q");
		SaveScanTimeEvent("    WFT: REGN WFT get wafer position to end (wafer region) q");
	}//if( IsOnlyRegionEnd() )

	DisplaySequence("WT - get wafer position to end q");
	CMSLogFileUtility::Instance()->WT_GetIdxLog("into wafer end handling q");

	return nResult;
}


INT CWaferTable::OpGetWaferPos_BlockPick()
{
	INT				nResult	= gnOK;
	unsigned long	ulX = 0, ulY = 0, ulIndex = 0;
	unsigned char   ucGrade;
	BOOL			bEndOfWafer;
	ULONG			ulRefValue = 0;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	m_bLastDieUpdate = FALSE;
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	
	//BLOCKPICK (NOT for regular ref die)
	if (!m_bFirstCycleWT && IsBlkFunc1Enable())	//Block1
	{
		DieMapPos CurDieMap;
		DiePhyPos CurDiePhy;
		INT i;
		ULONG ulNumRow, ulNumCol;
		m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

		//v3.25T16
		DieMapPos CurrDie;
		CurrDie.Row = GetLastDieRow();
		CurrDie.Col = GetLastDieCol();
		BOOL bNeedUpdate = m_pBlkFunc->IsCurrBlkNeedUpdate(CurrDie);

		for (i = 0; i < 8; i++)
		{
			CurDieMap.Row = CurDieMap.Col = 1000;
			switch (i)
			{
				case 0:
					if (GetLastDieCol() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow();
					CurDieMap.Col = GetLastDieCol() - 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosY"];
					break;
				case 1:
					if (GetLastDieCol() == ulNumCol - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow();
					CurDieMap.Col = GetLastDieCol() + 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosY"];
					break;
				case 2:
					if (GetLastDieRow() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() - 1;
					CurDieMap.Col = GetLastDieCol();
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosY"];
					break;
				case 3:
					if (GetLastDieRow() == ulNumRow - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() + 1;
					CurDieMap.Col = GetLastDieCol();
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosY"];
					break;
				case 4:
					if (GetLastDieRow() == 0 || GetLastDieCol() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() - 1;
					CurDieMap.Col = GetLastDieCol() - 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosY"];
					break;
				case 5:
					if (GetLastDieRow() == ulNumRow - 1 || GetLastDieCol() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() + 1;
					CurDieMap.Col = GetLastDieCol() - 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosY"];
					break;
				case 6:
					if (GetLastDieRow() == 0 || GetLastDieCol() == ulNumCol - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() - 1;
					CurDieMap.Col = GetLastDieCol() + 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosY"];
					break;
				case 7:
					if (GetLastDieRow() == ulNumRow - 1 || GetLastDieCol() == ulNumCol - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() + 1;
					CurDieMap.Col = GetLastDieCol() + 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosY"];
					break;
			}

			if (CurDieMap.Row == 1000 || CurDieMap.Col == 1000) 
			{
				continue;
			}
			if (m_WaferMapWrapper.GetReader()->IsSelected(CurDieMap.Row, CurDieMap.Col))	//if EDGE die
			{
				continue;
			}

			//v3.25T13
			if (m_bBlkLongJump)
			{
				if (IsMapNullBin(CurDieMap.Row, CurDieMap.Col))
				{
					if ((CurDiePhy.x != 0) || (CurDiePhy.y != 0))
					{
						CString szErr;
						szErr.Format("ERROR: die found at EMPTY die position (%d) after LONG-JUMP!  Please realign wafer.", i);
						HmiMessage_Red_Yellow(szErr);
						SetErrorMessage(szErr);
						SetAlignmentStatus(FALSE);	//force to realign wafer
						return Err_WTBlkLongJump;
					}
				}
				else
				{
					if (m_WaferMapWrapper.GetReader()->IsMissingDie(CurDieMap.Row, CurDieMap.Col))
					{
					}
					else if (m_WaferMapWrapper.GetReader()->IsSelected(CurDieMap.Row, CurDieMap.Col))
					{
					}
					else
					{
						if ((CurDiePhy.x == 0) && (CurDiePhy.y == 0))
						{
							CString szErr;
							szErr.Format("ERROR: no die found at die position (%d) after LONG-JUMP!  Please realign wafer", i);
							HmiMessage_Red_Yellow(szErr);
							SetErrorMessage(szErr);
							SetAlignmentStatus(FALSE);	//force to realign wafer
							return Err_WTBlkLongJump;
						}
					}
				}
			}

			if (CurDiePhy.x == 0 && CurDiePhy.y == 0) 
			{
				continue;
			}

			//v3.25T16
			if (!bNeedUpdate)
			{
				if ((i == 3) || (i == 5) || (i == 7))	// (DL, DN, DR)	PR sub-wnd
				{
					continue;
				}
			}

			CurDiePhy.x += m_stPosOnPR.lX_Enc;
			CurDiePhy.y += m_stPosOnPR.lY_Enc;
			CurDieMap.Status = WAF_CDieSelectionAlgorithm::ALIGN;
			CurDiePhy.t = m_stPosOnPR.lT_Enc;
			m_pBlkFunc->SetCurDiePhyPos(CurDieMap, CurDiePhy, FALSE);

			UCHAR ucDieGrade = m_WaferMapWrapper.GetGrade((ULONG)(CurDieMap.Row), (ULONG)CurDieMap.Col);
			if (ucDieGrade != m_WaferMapWrapper.GetGrade(GetLastDieRow(), GetLastDieCol())
					&& ucDieGrade != 99 + m_WaferMapWrapper.GetGradeOffset()
					&& GetLastDieEAct() == WAF_CDieSelectionAlgorithm::PICK
					&& (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBin"] == FALSE)
			{
				(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBin"] = TRUE;
				(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBinTo"] = ucDieGrade;
			}
		}
	}

	m_bFirstCycleWT = FALSE;

	PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "OpGetPos BlkPick");

	if (bEndOfWafer)
	{
		if( RestartDefectDieSort() )
		{
			PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "Blk Pick defect pick");
		}
	}
	else
	{
		if ( !IsBlkFunc1Enable() )
		{
			ULONG ulState = GetMapDieState(ulY, ulX);
			ULONG ulPeekCounter = 1;
			WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct = eAction;
			BOOL bLoopFind = FALSE;

			if( IsDieUnpickScan(ulState) )
			{
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
				bLoopFind = TRUE;
			}

			if( bLoopFind )
			{
				UpdMapLastDie("GetWaerPos BlkPick");	//Update  LAST die
			}

			while (!bEndOfWafer && bLoopFind)
			{
				GetMapNextDie(ulY, ulX, ucGrade, eAction, bEndOfWafer);
				SetMapNextDie(ulY, ulX, ucGrade, eUpAct, ulState);

				ulPeekCounter++;
				if (ulPeekCounter > 1000)
				{
					break;
				}
				//Peek another new die after updating curr die state
				PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "Loop BlkPick");	
				eUpAct = eAction;
				ulState = GetMapDieState(ulY, ulX);

				bLoopFind = FALSE;
				if( IsDieUnpickScan(ulState) )
				{
					eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
					bLoopFind = TRUE;
				}
			}
		}
	}

	// xuzhijin_region check the next die is another region, stop sorting and continue align->scan next one
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	CString szMsg;

	if (GetRegionSortOuter())
	{
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		BOOL  bSameRegion = pSRInfo->IsWithinThisRegion(ulTgtRegion, ulY, ulX);
		if (bEndOfWafer)
		{
			pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDED);
			SaveRegionStateAndIndex();
		}
		else if (bSameRegion == FALSE)
		{
			pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDED);
			ulTgtRegion = pSRInfo->GetWithinRegionNo(ulY, ulX);
			pSRInfo->SetTargetRegion(ulTgtRegion);		// region pick xuxuxu
			pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDING);
			SaveRegionStateAndIndex();
		}
	}

	//BLOCKPICK (NOT for regular ref die)
	if (IsBlkFunc1Enable() &&	!bEndOfWafer && 
			(eAction == WAF_CDieSelectionAlgorithm::PICK || eAction == WAF_CDieSelectionAlgorithm::SKIP))
	{
		ULONG ulState = GetMapDieState(ulY, ulX);
		while (!bEndOfWafer &&
				((ulState == WT_MAP_DS_DEFAULT) || IsDieEmpty(ulState) || IsDieUnpickAll(ulState)))
		{
			CMSLogFileUtility::Instance()->WT_GetDieLog("Block pick loop die not good");
			GetMapNextDie(ulY, ulX, ucGrade, eAction, bEndOfWafer);

			WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct = eAction;
			ULONG lUpState = ulState;
			switch (ulState)
			{
			case WT_MAP_DS_DEFAULT:
				eUpAct = WAF_CDieSelectionAlgorithm::ALIGN;
				lUpState = WT_MAP_DS_DEFAULT;
				break;
			case WT_MAP_DS_PR_EMPTY:
				m_WaferMapWrapper.SetAlgorithmParameter("Row", ulY);
				m_WaferMapWrapper.SetAlgorithmParameter("Col", ulX);
				m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Empty);
				eUpAct = WAF_CDieSelectionAlgorithm::MISSING;
				lUpState = WT_MAP_DS_PR_EMPTY;
				break;
			case WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY:
			case WT_MAP_DS_UNPICK_REGRAB_EMPTY:
			case WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT:
			case WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT:
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
				break;
			case WT_MAP_DIESTATE_UNPICK:
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
				lUpState = WT_MAP_DIESTATE_UNPICK;
				break;
			}

			SetMapNextDie(ulY, ulX, ucGrade, eUpAct, lUpState);

			m_WaferMapWrapper.SetAlgorithmParameter("Row", 0);
			m_WaferMapWrapper.SetAlgorithmParameter("Col", 0);
			m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);

			//Peek another new die after updating curr die state
			PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "block pick 1");	
			ulState = m_WaferMapWrapper.GetDieState(ulY, ulX);
		}
	}

	//v3.32
	//Update current BLOCK number in Block-Pick mode (NOT for regular ref die)
	m_bBlkLongJump = FALSE;
	if (IsBlkFunc1Enable() && !bEndOfWafer && (eAction == WAF_CDieSelectionAlgorithm::PICK))	//Block2
	{
		DieMapPos MapPos;
		MapPos.Row = ulY;
		MapPos.Col = ulX;
		MapPos.Status = 0;	//Klocwork
		LONG lBlkRow = 0, lBlkCol = 0;

		LONG lBlk = m_pBlkFunc->GetCurrBlockNum(lBlkRow, lBlkCol, MapPos);

		LONG lCurrRow = m_lCurrBlockNum / 100;
		LONG lCurrCol = m_lCurrBlockNum % 100;
		LONG lNextRow = lBlk / 100;
		LONG lNextCol = lBlk % 100;

		if ((m_lCurrBlockNum != 0) && (lBlk != 0))
		{
			if (abs(lCurrRow - lNextRow) >= 2)
			{
				m_bBlkLongJump = TRUE;
			}
			else if (abs(lCurrCol - lNextCol) >= 3)
			{
				m_bBlkLongJump = TRUE;
			}
		}

		//v3.25T16	//OptoTech
		if (lBlk != m_lCurrBlockNum)
		{
			if (m_bBlkLongJump)
			{
				LOG_BLOCK_ALIGN("Long Jump!");
			}
			CString szLog;
			szLog.Format("Sorting %d  (old = %d)", lBlk, m_lCurrBlockNum);
			LOG_BLOCK_ALIGN(szLog);
		}

		m_lCurrBlockNum = lBlk;
	}


	//BLOCKPICK
	(*m_psmfSRam)["WaferPr"]["LookAround"]["X"]			= ulX;
	(*m_psmfSRam)["WaferPr"]["LookAround"]["Y"]			= ulY;
	(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosY"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosX"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosY"]		= 0;

	//BLOCKPICK (NOT for regular ref die)
	if (IsBlkFunc1Enable())	//Block2
	{
		(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = FALSE;	// get pos, block pick 1
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsAlign"]		= FALSE;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsGood"]			= FALSE;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieTNeedRotate"]	= FALSE;
	}


	if (m_bFirst == FALSE)
	{
		OpIsWithinPolygonLimit(ulY, ulX, ucGrade, eAction, bEndOfWafer);
	}

	if (bEndOfWafer == TRUE)
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("WAFER-END");
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End in WT");
		return Err_WTWaferEnd;
	}
	else
	{
		//Reset wafer end signal
		m_bIsWaferEnded	= FALSE;
	}

	(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["Valid"]	= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= 0;

	if (IsBlkFunc2Enable())	//Block2
	{
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = -1;
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = -1;
		(*m_psmfSRam)["WaferMap"]["ComingDieDirection"] = -1;
	}
	else
	{
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"]	= 0;
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"]	= 0;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (eAction == WAF_CDieSelectionAlgorithm::PICK)
	{	
		// Store data to Die Info
		ulIndex = (*m_psmfSRam)["DieInfo"]["WT_Index"];
		ulIndex++;
		(*m_psmfSRam)["DieInfo"]["WT_Index"] = ulIndex;

		if (DieIsNoPick(ulY, ulX) == TRUE)
		{
			eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		}

		if (DieIsSpecialRefGrade(ulY, ulX))				//v3.24T1
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"] = 1;		//Tell WPR to align this PICK die with Ref-Die pattern
		}

		m_lRealignDieNum++;		//Block2
	}
	else if (eAction == WAF_CDieSelectionAlgorithm::CHECK)
	{
		BlkFunc2ReferDieCheck(ulY, ulX);
		
		(*m_psmfSRam)["DieInfo"]["RefDie"]["X"]		= ulX;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Y"]		= ulY;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 1;

		if (IsBlkFunc2Enable() && IsCharDieInUse())
		{
			if (m_WaferMapWrapper.HasReferenceFaceValue(ulY, ulX) == TRUE)
			{
				ulRefValue = m_WaferMapWrapper.GetReferenceDieFaceValue(ulY, ulX);
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = ulRefValue / 10;
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = ulRefValue % 10;
			}
			else
			{
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = -1;
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = -1;
			}
		}
	}
	else if (eAction == WAF_CDieSelectionAlgorithm::SEPARATION)
	{
		if (m_bCheckSepGrade == TRUE)
		{
			return Err_WTHitSeparationGrade; 
		}
	}
	
	BOOL bCheckAlnRefer = TRUE;
	if( (IsPrescanEnable()) && bCheckAlnRefer )
	{
		LONG lRow, lCol, lX, lY;
		pUtl->GetAlignPosition(lRow, lCol, lX, lY);
		if ((ulX == lCol) && (ulY == lRow) && m_WaferMapWrapper.IsReferenceDie(ulY, ulX) )
		{
			CString szMsg;
			szMsg.Format("HomeDie (%d,%d) is ReferDie in get pos general", ulY, ulX);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			if (eAction == WAF_CDieSelectionAlgorithm::PICK)
			{
				eAction = WAF_CDieSelectionAlgorithm::ALIGN;
			}
			(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"] = 1;
			BlkFunc2ReferDieCheck(ulY, ulX);
		}
	}

	if (m_bJustAlign == TRUE && IsPrescanEnable())
	{
		m_ulNewPickCounter = 0;
	}

	if (eAction == WAF_CDieSelectionAlgorithm::ALIGN)
	{
		if (DieIsSpecialRefGrade(ulY, ulX))				//v3.24T1
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"] = 1;		//Tell WPR to align this PICK die with Ref-Die pattern
		}
	}

	m_stCurrentDie.lX = (LONG)ulX;
	m_stCurrentDie.lY = (LONG)ulY;
	m_stCurrentDie.ucGrade = ucGrade;

	if( m_WaferMapWrapper.GetDieState(GetCurrentRow(), GetCurrentCol())== WT_MAP_DIESTATE_SKIP_PREDICTED )
	{
		eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		szMsg.Format("Get Posn %d,%d set to align because it is skipped", ulY, ulX);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	}

	m_stCurrentDie.eAction = eAction;

	//v2.96T2
	// Used in waferPr and pass to IM Vision for logging
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"]		= ulY;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"]		= ulX;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"]	= ucGrade;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"]	= eAction;
	if( m_bJustAlign )
	{
		LONG lRow, lCol, lWftX, lWftY;
		pUtl->GetAlignPosition(lRow, lCol, lWftX, lWftY);
		m_lRescanStartRow			= lRow;
		m_lRescanStartCol			= lCol;
		m_lRescanStartWftX			= lWftX;
		m_lRescanStartWftY			= lWftY;
	}

	// Save data to bond record
	NextBondRecord();

	//v3.89	//MS100 LF die speedup for BH MOVE_T sequence
	if (CMS896AApp::m_lCycleSpeedMode >= 3)	
	{
		if (OpIsCurrentLFDie())
		{
			(*m_psmfSRam)["WaferTable"]["LFDie"] = TRUE;
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;
		}

		//v3.94T3		//DBH only
		if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
		{
			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;
		}
	}

	unsigned long	ulNextX = 0, ulNextY = 0;

	//BLOCKPICK (NOT for regular ref die)
	if (IsBlkFunc1Enable())	//Block2
	{
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextRow"]	= 0;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextCol"]	= 0;

		PeekMapNextDie(2, ulNextY, ulNextX, ucGrade, eAction, bEndOfWafer,"Block4");
		
		if (!bEndOfWafer && (eAction == WAF_CDieSelectionAlgorithm::PICK))
		{
			(*m_psmfSRam)["WaferPr"]["LookAround"]["NextRow"]	= ulNextY;
			(*m_psmfSRam)["WaferPr"]["LookAround"]["NextCol"]	= ulNextX;
		}
	}

	LONG lNextDieDirection = -1;
	BOOL bUseLookForward = IsUseLF();

	if (bUseLookForward)
	{
	//	m_lNoLookForwardCounter++;		//v3.55		//For statistics usage only
	}

	return nResult;
}


//v4.43T2
LONG CWaferTable::OpCalculateLFNextDieDirection(CONST ULONG ulCurRow, CONST ULONG ulCurCol,
												CONST ULONG ulNextRow, CONST ULONG ulNextCol, LONG& lFOVMode)	//SanAn MS100PlusII
{
	LONG lNextDieDirection = -1;
	lFOVMode			   = 0;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
		DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
		DOUBLE dFov = min(dFOVX, dFOVY);

		LONG lRowDiff = ulNextRow - ulCurRow;
		LONG lColDiff = ulNextCol - ulCurCol;
		INT nRegion = -1;
		if (FALSE/*dFov > WPR_LF_SIZE_13X13*/)
		{
			lFOVMode = 5;
			if ( (labs(lRowDiff) > 6) || (labs(lColDiff) > 6) )
			{
				return -1;	//No LF under 13x13 FOV
			}
			//Use 13x13 LF Window
			//////////////////////////
			//	1   2   3   4   5   6   7   8   9   10  11  12  13
			//	14  15  16  17  18  19  20  21  22  23  24  25  26
			//	27  28  29  30  31  32  33  34  35  36  37  38  39
			//  40  41  42  43  44  45  46  47  48  49  50  51  52
			//  53  54  55  56  57  58  59  60  61  62  63  64  65
			//  66  67  68  69  70  71  72  73  74  75  76  77  78
			//  79  80  81  82  83  84 [85] 86  87  88  89  90  91
			//	92  93  94  95  96  97  98  99  100 101 102 103 104
			//  105 106 107 108 109 110 111 112 113 114 115 116 117
			//  118 119 120 121 122 123 124 125 126 127 128 129 130
			//  131 132 133 134 135 136 137 138 139 140 141 142 143
			//  144 145 146 147 148 149 150 151 152 153 154 155 156
			//  157 158 159 160 161 162 163 164 165 166 167 168 169
			switch (lRowDiff)
			{
			case -6:
				nRegion = 0;
				break;
			case -5:
				nRegion = 13;
				break;
			case -4:
				nRegion = 26;
				break;
			case -3:
				nRegion = 39;
				break;
			case -2:
				nRegion = 52;
				break;
			case -1:
				nRegion = 65;
				break;
			case 1:
				nRegion = 91;
				break;
			case 2:
				nRegion = 104;
				break;
			case 3:
				nRegion = 117;
				break;
			case 4:
				nRegion = 130;
				break;
			case 5:
				nRegion = 143;
				break;
			case 6:
				nRegion = 156;
				break;
			case 0:
				nRegion = 78;
				break;
			}

			switch (lColDiff)
			{
			case -6:
				nRegion = nRegion + 1;
				break;
			case -5:
				nRegion = nRegion + 2;
				break;
			case -4:
				nRegion = nRegion + 3;
				break;
			case -3:
				nRegion = nRegion + 4;
				break;
			case -2:
				nRegion = nRegion + 5;
				break;
			case -1:
				nRegion = nRegion + 6;
				break;
			case 1:
				nRegion = nRegion + 8;
				break;
			case 2:
				nRegion = nRegion + 9;
				break;
			case 3:
				nRegion = nRegion + 10;
				break;
			case 4:
				nRegion = nRegion + 11;
				break;
			case 5:
				nRegion = nRegion + 12;
				break;
			case 6:
				nRegion = nRegion + 13;
				break;
			case 0:
				nRegion = nRegion + 7;
				break;
			}

			lNextDieDirection = nRegion;
			if (nRegion == 85)				//v4.47T10
				lNextDieDirection = -1;
			return lNextDieDirection;

		}
		else if (dFov > WPR_LF_SIZE_11X11)
		{
			lFOVMode = 4;
			if ( (labs(lRowDiff) > 5) || (labs(lColDiff) > 5) )
			{
				return -1;	//No LF under 11x11 FOV
			}
			//Use 11x11 LF Window
			//////////////////////////
			//	1   2   3   4   5   6   7   8   9   10  11
			//  12  13  14  15  16  17  18  19  20  21  22  
			//	23  24  25  26  27  28  29  30  31  32  33
			//  34  35  36  37  38  39  40  41  42  43  44
			//  45  46  47  48  49  50  51  52  53  54  55
			//  56  57  58  59  60 [61] 62  63  64  65  66
			//	67  68  69  70  71  72  73  74  75  76  77
			//	78  79  80  81  82  83  84  85  86  87  88
			//	89  90  91  92  93  94  95  96  97  98  99
			//	100 101 102 103 104 105 106 107 108 109 110
			//  111 112 113 114 115 116 117 118 119 120 121
			switch (lRowDiff)
			{
			case -5:
				nRegion = 0;
				break;
			case -4:
				nRegion = 11;
				break;
			case -3:
				nRegion = 22;
				break;
			case -2:
				nRegion = 33;
				break;
			case -1:
				nRegion = 44;
				break;
			case 1:
				nRegion = 66;
				break;
			case 2:
				nRegion = 77;
				break;
			case 3:
				nRegion = 88;
				break;
			case 4:
				nRegion = 99;
				break;
			case 5:
				nRegion = 110;
				break;
			case 0:
				nRegion = 55;
				break;
			}

			switch (lColDiff)
			{
			case -5:
				nRegion = nRegion + 1;
				break;
			case -4:
				nRegion = nRegion + 2;
				break;
			case -3:
				nRegion = nRegion + 3;
				break;
			case -2:
				nRegion = nRegion + 4;
				break;
			case -1:
				nRegion = nRegion + 5;
				break;
			case 1:
				nRegion = nRegion + 7;
				break;
			case 2:
				nRegion = nRegion + 8;
				break;
			case 3:
				nRegion = nRegion + 9;
				break;
			case 4:
				nRegion = nRegion + 10;
				break;
			case 5:
				nRegion = nRegion + 11;
				break;
			case 0:
				nRegion = nRegion + 6;
				break;
			}

			lNextDieDirection = nRegion;
			if (nRegion == 61)				//v4.47T10
				lNextDieDirection = -1;
			return lNextDieDirection;
		}
		else if (dFov > WPR_LF_SIZE_9X9)
		{
			lFOVMode = 3;
			if ( (labs(lRowDiff) > 4) || (labs(lColDiff) > 4) )
			{
				return -1;	//No LF under 9x9 FOV
			}
			//Use 9x9 LF Window
			//////////////////////////
			//	1   2   3   4   5   6   7   8   9
			//  10  11  12  13  14  15  16  17  18
			//  19  20  21  22  23  24  25  26  27
			//  28  29  30  31  32  33  34  35  36
			//  37  38  39  40 [41] 42  43  44  45
			//  46  47  48  49  50  51  52  53  54
			//  55  56  57  58  59  60  61  62  63
			//  64  65  66  67  68  69  70  71  72
			//  73  74  75  76  77  78  79  80  81
			switch (lRowDiff)
			{
			case -4:
				nRegion = 0;
				break;
			case -3:
				nRegion = 9;
				break;
			case -2:
				nRegion = 18;
				break;
			case -1:
				nRegion = 27;
				break;
			case 1:
				nRegion = 45;
				break;
			case 2:
				nRegion = 54;
				break;
			case 3:
				nRegion = 63;
				break;
			case 4:
				nRegion = 72;
				break;
			case 0:
				nRegion = 36;
				break;
			}

			switch (lColDiff)
			{
			case -4:
				nRegion = nRegion + 1;
				break;
			case -3:
				nRegion = nRegion + 2;
				break;
			case -2:
				nRegion = nRegion + 3;
				break;
			case -1:
				nRegion = nRegion + 4;
				break;
			case 1:
				nRegion = nRegion + 6;
				break;
			case 2:
				nRegion = nRegion + 7;
				break;
			case 3:
				nRegion = nRegion + 8;
				break;
			case 4:
				nRegion = nRegion + 9;
				break;
			case 0:
				nRegion = nRegion + 5;
				break;
			}

			lNextDieDirection = nRegion;
			if (nRegion == 41)				//v4.47T10
				lNextDieDirection = -1;
			return lNextDieDirection;
		}
		else if (dFov > WPR_LF_SIZE_7X7)
		{
			lFOVMode = 2;
			if ( (labs(lRowDiff) > 3) || (labs(lColDiff) > 3) )
			{
				return -1;	//No LF under 7x7 FOV
			}

			//Use 7x7 LF Window
			//////////////////////////
			//	1  2  3   4   5  6  7
			//  8  9  10  11  12 13 14
			//  15 16 17  18  19 20 21
			//  22 23 24 [25] 26 27 28
			//  29 30 31  32  33 34 35
			//  36 37 38  39  40 41 42
			//  43 44 45  46  47 48 49
			//
			switch (lRowDiff)
			{
			case -3:
				nRegion = 0;
				break;
			case -2:
				nRegion = 7;
				break;
			case -1:
				nRegion = 14;
				break;
			case 1:
				nRegion = 28;
				break;
			case 2:
				nRegion = 35;
				break;
			case 3:
				nRegion = 42;
				break;
			case 0:
				nRegion = 21;
				break;
			}

			switch (lColDiff)
			{
			case -3:
				nRegion = nRegion + 1;
				break;
			case -2:
				nRegion = nRegion + 2;
				break;
			case -1:
				nRegion = nRegion + 3;
				break;
			case 1:
				nRegion = nRegion + 5;
				break;
			case 2:
				nRegion = nRegion + 6;
				break;
			case 3:
				nRegion = nRegion + 7;
				break;
			case 0:
				nRegion = nRegion + 4;
				break;
			}

			lNextDieDirection = nRegion;
			if (nRegion == 25)				//v4.47T10
				lNextDieDirection = -1;
			return lNextDieDirection;
		}
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lFOVMode = 1;
			if ( (labs(lRowDiff) > 2) || (labs(lColDiff) > 2) )
			{
				return -1;	//No LF under 5x5 FOV
			}

			//Else use 5x5 LF Window
			//////////////////////
			//  1  2   3   4  5
			//  6  7   8   9  10
			//  11 12 [13] 14 15
			//  16 17  18  19 20
			//  21 22  23  24 25
			//
			switch (lRowDiff)
			{
			case -2:
				nRegion = 0;
				break;
			case -1:
				nRegion = 5;
				break;
			case 1:
				nRegion = 15;
				break;
			case 2:
				nRegion = 20;
				break;
			case 0:
				nRegion = 10;		//v4.43T3	//Bugfix
				break;
			}

			switch (lColDiff)
			{
			case -2:
				nRegion = nRegion + 1;
				break;
			case -1:
				nRegion = nRegion + 2;
				break;
			case 1:
				nRegion = nRegion + 4;
				break;
			case 2:
				nRegion = nRegion + 5;
				break;
			case 0:
				nRegion = nRegion + 3;
				break;
			}

			lNextDieDirection = nRegion;
			if (nRegion == 13)				//v4.47T10
				lNextDieDirection = -1;
			return lNextDieDirection;
		}
	}


	// Else, use traditional 3x3 FOV LF window
	////////////////
	//  4   2   6
	//  0   -1  1
	//  5   3   7

	if (ulCurRow == ulNextRow)			//If same row
	{
		if ((ulNextCol + 1) == ulCurCol)
		{
			lNextDieDirection = 0;			//WPR_LT_DIE=0 
		}
		else if ((ulNextCol - 1) == ulCurCol)
		{
			lNextDieDirection = 1;			//WPR_RT_DIE=1 
		}
	}
	else if (ulCurCol == ulNextCol)		//If same column
	{
		if ((ulNextRow + 1) == ulCurRow)
		{
			lNextDieDirection = 2;			//WPR_UP_DIE=2
		}
		else if ((ulNextRow - 1) == ulCurRow)
		{
			lNextDieDirection = 3;			//WPR_DN_DIE=3
		}
	}
	else if ((ulNextCol + 1) == ulCurCol)	//If LEFT col	
	{
		if ((ulNextRow + 1) == ulCurRow)
		{
			lNextDieDirection = 4;			//WPR_UL_DIE=4
		}
		else if ((ulNextRow - 1) == ulCurRow)
		{
			lNextDieDirection = 5;			//WPR_DL_DIE=5
		}
	}
	else if ((ulNextCol - 1) == ulCurCol)	//If RIGHT col
	{
		if ((ulNextRow + 1) == ulCurRow)
		{
			lNextDieDirection = 6;			//WPR_UR_DIE=6
		}
		else if ((ulNextRow - 1) == ulCurRow)
		{
			lNextDieDirection = 7;			//WPR_DR_DIE=7
		}
	}

	return lNextDieDirection;
}


INT CWaferTable::OpStoreDieInfo()
{
	if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
	{
		LONG lRow = GetCurrentRow();
		LONG lCol = GetCurrentCol();
		UCHAR ucCurGrade = GetCurrDieGrade();

		//v4.38T7	//Mixed bin sorting
		if ((m_bSortMultiToOne || m_ulMinGradeCount > 0) && (m_ulMinGradeBinNo > 0))
		{
			for (INT i = 0; i < m_unArrMinGradeBin.GetSize(); i++)
			{
				UCHAR ucGrade = m_unArrMinGradeBin.GetAt(i);
				if (ucGrade == ucCurGrade)
				{
					ucCurGrade = (UCHAR) (m_ulMinGradeBinNo + m_WaferMapWrapper.GetGradeOffset());		//Always map to Grade-1
					break;
				}
			}
		}

		if (m_bMultiGradeSortingtoSingleBin)	//v4.15T8	//Osram Germany Laer-sort fcn
		{
			ucCurGrade = 1 + m_WaferMapWrapper.GetGradeOffset();
		}

		if (!m_nChangeColletOnWT)
		{
			// Save the data to SRAM String Map File
			(*m_psmfSRam)["WaferMap"]["X"] = lCol;
			(*m_psmfSRam)["WaferMap"]["Y"] = lRow;
			(*m_psmfSRam)["WaferMap"]["Grade"] = ucCurGrade;
		}
		//v4.50A13
		if( SPECIAL_DEBUG_LOG_WT )
		{
			CString szLog;
			szLog.Format("WT - map (%ld,%ld)%ld Op StoreDieInfo", lRow, lCol, ucCurGrade - m_WaferMapWrapper.GetGradeOffset());
			DisplaySequence(szLog);
		}
	}
		
	return gnOK;
}


////////////////////////////////////////////
//	Move Table
////////////////////////////////////////////

BOOL CWaferTable::GetBlk2SortPosn(LONG lCurrRow, LONG lCurrCol, LONG lLastRow, LONG lLastCol, LONG &lMoveX, LONG &lMoveY, LONG &lMoveT)
{
	T_Sync();
	GetEncoderValue();

	// Get physical position from wafer map
	LONG lOutX = 0, lOutY = 0;
	if (m_pBlkFunc2->Blk2GetNextDiePhyPos(lCurrRow, lCurrCol, lLastRow, lLastCol, lOutX, lOutY) == FALSE)
	{
		if (GetMapPhyPosn(lCurrRow, lCurrCol, lOutX, lOutY) == FALSE)
		{
			if (GetDieValidPrescanPosn(lCurrRow, lCurrCol, 3, lOutX, lOutY) == FALSE)
			{
				X_Sync();
				Y_Sync();
				LONG	lDiePitchX_X = 0, lDiePitchX_Y = 0, lDiePitchY_X = 0, lDiePitchY_Y = 0;
				lDiePitchX_X = GetDiePitchX_X(); 
				lDiePitchX_Y = GetDiePitchX_Y();
				lDiePitchY_X = GetDiePitchY_X();
				lDiePitchY_Y = GetDiePitchY_Y();
				LONG lTheta		= 0;
				LONG lCurrPhX = GetCurrX();
				LONG lCurrPhY = GetCurrY();
				LONG lCurrPhT = GetCurrT();
				// Set physical position into wafermap
				DOUBLE dTheta = (DOUBLE)(GetGlobalT() - lCurrPhT);
				if (fabs(dTheta) < 5.0)
				{
					dTheta = 0.0;
				}
				dTheta = -(DOUBLE)m_lThetaMotorDirection * (dTheta * m_dThetaRes);
				RotateWaferTheta(&lCurrPhX, &lCurrPhY, &lTheta, dTheta);

				LONG lDiff_X	= lCurrCol - lLastCol;
				LONG lDiff_Y	= lCurrRow - lLastRow;

				lOutX = lCurrPhX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
				lOutY = lCurrPhY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			}
		}
	}

	lMoveT = GetGlobalT() - GetCurrT();
	lMoveX = lOutX;
	lMoveY = lOutY;

	return TRUE;
}

INT	CWaferTable::OpMoveTable()
{
	CString szMoveMsg;
	LONG	lMoveWfX = 0, lMoveWfY = 0, lMoveWfT = 0;
	LONG	lCalcX = 0, lCalcY = 0, lCalcT = 0;
	LONG	lDiePitchX_X = 0, lDiePitchX_Y = 0, lDiePitchY_X = 0, lDiePitchY_Y = 0;
	LONG	lDiff_X = 0, lDiff_Y = 0;
	LONG	lLastX = 0, lLastY = 0;
	BOOL	bRotate = FALSE;
	BOOL	bPrescanJumpRotate = FALSE;
	DOUBLE	dTheta = 0.0;
	BOOL	bDieLFed	= FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bUseIM = pApp->GetFeatureStatus(MS896A_FUNC_VISION_INSP_MACHINE);		//v3.02T6
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	BOOL	bUseLF	= IsUseLF();
	BOOL	bEjtLF	= FALSE;
	BOOL bPickAction = GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK;
	
	if(	m_bSortingDefectDie==FALSE &&
		( (m_bPrescanDefectAction && m_bPrescanDefectToNullBin==FALSE && m_bNgPickDefect) || 
		  (m_bPrescanBadCutAction && m_bPrescanBadCutToNullBin==FALSE && m_bNgPickBadCut) ) )
	{
		UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
		UCHAR ucNgDefectGrade = GetScanDefectGrade() + ucOffset;
		UCHAR ucNgBadcutGrade = GetScanBadcutGrade() + ucOffset;
		BOOL bPickDefect = FALSE;
		if( m_bNgPickDefect && ucNgDefectGrade==GetCurrDieGrade() )
		{
			bPickDefect = TRUE;
		}
		if( m_bNgPickBadCut && ucNgBadcutGrade==GetCurrDieGrade() )
		{
			bPickDefect = TRUE;
		}

		pUtl->SetPickDefectDie(bPickDefect);
	}
	if( m_bMapWaferVerifyOK	)
		m_ulVerifyMapWaferCounter = 0;
	else
		m_ulVerifyMapWaferCounter++;
	m_bMapWaferVerifyOK = TRUE;
	//v3.94T3	//Disabled for  SpeedMode = 3
	if (CMS896AApp::m_lCycleSpeedMode == 0)		//If non-speedup mode	
	{
		(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;	// reset firstly, should at get wafer pos
	}

	if( IsAdvRegionOffset() && (m_ulAdvStage1StartCount>0)  && (GetNewPickCount() < m_ulAdvStage1StartCount) )
	{
		m_stLastAdvSampleTime	= CTime::GetCurrentTime();	// not reach to counter, set to latest.
	}

	CTime stStartTime = CTime::GetCurrentTime();
	if (GetWftSampleState() == 2 && IsMapPosnOK() && IsAdvRegionOffset())	// before move table to do sampling
	{
		BOOL bDoSample = FALSE, bVerifyByFov = FALSE;
		// before index, this die should be set a long index time, after bh bond die,
		// it should move to prepick, befor sampling, should check bond arm at safe(prepick position).
		if (IsAutoSampleSetupOK())
		{
			CTimeSpan stTimeSpan = stStartTime - m_stLastAdvSampleTime;
			LONG lSpanUseTime = m_ulAdvStage1SampleSpan*100/1000;
			if( m_ulAdvStage3StartCount>0 && m_ulAdvStage3SampleSpan>0 && GetNewPickCount()>=m_ulAdvStage3StartCount )	// ejector pin broken case >
			{
				lSpanUseTime = m_ulAdvStage3SampleSpan*100/1000;
			}
			else if( m_ulAdvStage2StartCount>0 && m_ulAdvStage2SampleSpan>0 && GetNewPickCount()>=m_ulAdvStage2StartCount )	// ejector pin broken case >
			{
				lSpanUseTime = m_ulAdvStage2SampleSpan*100/1000;
			}
			LONG lSampleLimit = max(lSpanUseTime*5, 1500);		// 05 times of normal interal time

			LONG lVerifyLimit = max(lSpanUseTime*10, 3000);	// 10 times of normal interal time

			if( stTimeSpan.GetTotalSeconds()>=lSampleLimit )
			{
				bDoSample = TRUE;
			}
			if( lVerifyLimit>0 && stTimeSpan.GetTotalSeconds()>=lVerifyLimit )
			{
				if( pApp->GetCustomerName()==CTM_SANAN ||
					pApp->GetCustomerName()==CTM_SEMITEK )
				{
					bVerifyByFov = TRUE;
				}
			}
			if( GetNewPickCount()>=GetNextXXXCounter() )	// ejector pin broken case >
			{
				bDoSample = TRUE;
			}
		}	// adv die offset
		if( bDoSample )
		{
#ifdef	NU_MOTION
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			UINT unDataLog = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Auto Cycle NuMotion Data Log"), 0);
			UINT unItem4 = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP100Item4"), 1);
			if( IsAdvRegionOffset() && GetNewPickCount()>0 && unDataLog==1 && unItem4==1 )
			{
				if( NuMotionDataLogAdv(FALSE) )		//	A	just after theta to prepick, before ADV, turn off DataLog
				{
					CString szLogPath;
					CString szDataLog;
					CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\Test.csv";
					pUtl->GetPrescanLogPath(szLogPath);
					szDataLog.Format("%s_AdvA_%d.csv", szLogPath, GetNewPickCount());
					RenameFile(szFullLog, szDataLog);
				}

				NuMotionDataLogAdv(TRUE);	//	B	just after theta to prepick, before ADV, turn on DataLog
			}
#endif
			CString szMsg;
			SetWTReady(FALSE,"9");
			Sleep(1200);
			m_ulPdcRegionSampleCounter++;
			m_lWftAdvSamplingStage = 2;
			BH_EnableThermalControl(TRUE);	//	Adv region offset begin
			UpdateAllRegionDieOffset(-1, bVerifyByFov);
			BH_EnableThermalControl(FALSE);	//	Adv region offset end
			if( m_lWftAdvSamplingResult==0 )
			{
				RescanAdvOffsetNextLimit();
			}
			CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AdvSampleWT");
			m_lWftAdvSamplingStage = 0;
#ifdef	NU_MOTION
			if( IsAdvRegionOffset() && GetNewPickCount()>0 && unDataLog==1 && unItem4==1 )
			{
				if( NuMotionDataLogAdv(FALSE) )		//	B	just after theta to prepick, before ADV, turn off DataLog
				{
					CString szLogPath;
					CString szDataLog;
					CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\Test.csv";
					pUtl->GetPrescanLogPath(szLogPath);
					szDataLog.Format("%s_AdvB_%d.csv", szLogPath, GetNewPickCount());
					RenameFile(szFullLog, szDataLog);
				}

				NuMotionDataLogAdv(TRUE);	//	C	just after theta to prepick, before ADV, turn on DataLog
			}
#endif
		}

		m_lReadyToSampleOnWft = 0;	// adv region offset done
	}
	if( m_lWftAdvSamplingResult==-1 || m_lWftAdvSamplingResult==1 )
	{
		CString szMsg;
		szMsg.Format("Op Move Table Adv sampling fail %d", m_lWftAdvSamplingResult);
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
		return WT_PITCH_ERR_ADV_SAMPLE;	//	move table
	}

	if( (GetNewPickCount() < m_ulAdvStage1StartCount) )
	{
		m_stLastPickTime = CTime::GetCurrentTime();
	}

	if( IsAutoSampleSort() && IsAutoSampleSetupOK() && GetNewPickCount()>0 )
	{
		UINT unLowTimeLimit = m_lMC[WTMC_DAR_IdleLowLmt];
		if( unLowTimeLimit>3 && GetWftSampleState()==0 )	//	DEB OR ADV OR RESCAN
		{
			CTimeSpan stKeyDieSpan = stStartTime - m_stLastPickTime;
			LONGLONG lIdleTime = stKeyDieSpan.GetTotalMinutes();
			if( lIdleTime>unLowTimeLimit )
			{
				ChangeNextSampleLimit(3); DisplaySequence("WT - Add Next Sample Limit 3A");
			}
		}
	}	// idle time over limit, do sampling.
	m_stLastPickTime = CTime::GetCurrentTime();

	//xxxx	before move table to do sampling
	if( IsAutoSampleSetupOK() && GetNewPickCount()>0 && 
		DEB_IsUseable() && IsAutoRescanWafer()==FALSE )
	{
		if( GetWftSampleState()==2 )	//	DEB in use
		{
			// before index, this die should be set a long index time, after bh bond die,
			// it should move to prepick, befor sampling, should check bond arm at safe(prepick position).
			if( GetNewPickCount()>=GetNextXXXCounter() )	// ejector pin broken case >
			{
				CString szMsg;
				szMsg = "WT - DEB begin do sampling";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				DisplaySequence(szMsg);
				SetWTReady(FALSE,"10");
				Sleep(200);
				m_ulPdcRegionSampleCounter++;
				m_lWftAdvSamplingStage = 2;
				BH_EnableThermalControl(TRUE);	//	DEB key die sample begin
				BOOL bReturn = DEB_AutoRealignKeyDie();	// auto cycle move table
				BH_EnableThermalControl(FALSE);	//	DEB key die sample end
				RescanAdvOffsetNextLimit();
				m_lWftAdvSamplingStage = 0;
				CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "RealignKeyDieWT");
				szMsg = "WT - DEB after do sampling";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				DisplaySequence(szMsg);
				if (!bReturn)
				{
					m_lReadyToSampleOnWft = 0;	//	deb done fail
					CString szMsg = "Key die MISSING, invalid. Please realign wafer again.";
					HmiMessage_Red_Back(szMsg, "DEB Key");
					CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
					SetErrorMessage("Op move table " + szMsg);
					SetAlignmentStatus(FALSE);
					return WT_PITCH_ERR_DEB_SAMPLE;	//	move table
				}
			}
			m_lReadyToSampleOnWft = 0;	//	deb done ok
		}
	}	// for die enc base.

	WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
	if (pMarkDieRegionManager->IsScanRegionMarkDie() && (GetWftSampleState() == 2))
	{
		m_bWaferAlignComplete	= FALSE;
		CTime stScanMarkDieStartTime = CTime::GetCurrentTime();
		ScanRegionMarkDie();
		CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stScanMarkDieStartTime, "ScanRegionMarkDie");
		UpdateAllDiePosnByMarkDiePosn();
		m_bWaferAlignComplete	= TRUE;

		m_lReadyToSampleOnWft = 0;	//	deb done ok

		RescanAdvOffsetNextLimit();
		CString szMsg;
		szMsg = "WT - Mark Die Sampling Finished";
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		DisplaySequence(szMsg);
	}

	if (m_pEjPinCleanRegion->IsReachCleanDirtEJPin() && (GetWftCleanEjPinState() == 2))
	{
		m_bWaferAlignComplete	= FALSE;
		if (!CleanDirtEjectorPin())
		{
			m_bWaferAlignComplete	= TRUE;
			return WT_CLEAN_EJ_PIN_FAILED;
		}
		m_bWaferAlignComplete	= TRUE;
		//becasue after clean ejector pin, table move to prescan position instead of current die position
		//m_stLFDie.bGoodDie = FALSE; // current not use lookahead
		m_lReadyToCleanEjPinOnWft = 0;	//	deb done ok
		CString szMsg;
		szMsg = "WT - Clean Ejector Pin Finished";
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		DisplaySequence(szMsg);
	}

	lDiePitchX_X = GetDiePitchX_X(); 
	lDiePitchX_Y = GetDiePitchX_Y();
	lDiePitchY_X = GetDiePitchY_X();
	lDiePitchY_Y = GetDiePitchY_Y();

	//andrewng //2020-0617		//Default Die Pitch for BURNIN mode
	if (IsBurnIn() && !pApp->m_bBurnInGrabImage)	
	{
		lDiePitchX_X = NVC_WAF_BURNIN_DIE_PITCH_XX; 
		lDiePitchX_Y = NVC_WAF_BURNIN_DIE_PITCH_XY;
		lDiePitchY_X = NVC_WAF_BURNIN_DIE_PITCH_YX;
		lDiePitchY_Y = NVC_WAF_BURNIN_DIE_PITCH_YY;
	}

	lDiff_X	= GetCurrentCol() - GetLastDieCol();
	lDiff_Y	= GetCurrentRow() - GetLastDieRow();
	lLastX	= GetLastDieEncX();
	lLastY	= GetLastDieEncY();
	// lastx lasty
	CString szTemp;
	szTemp.Format( "#Last Coordinate: %d,%d", lLastX, lLastY);
	//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	

	dTheta	= -(DOUBLE)m_lThetaMotorDirection * ((DOUBLE)(GetGlobalT() - m_stLastDie.lT_Enc) * m_dThetaRes);
	// next die is NOT Long jump or		// No die founds/LF pitch-jump but PR failure
	bDieLFed = (m_stLFDie.lDirection != -1) && m_stLFDie.bGoodDie; // (m_stLFDie.bFullDie);
	
	//2018.1.2
	//CString szTemp;
	long lDirectionTemp	= (*m_psmfSRam)["WaferMap"]["NextDieDirection"];
	szTemp.Format("WT MoveTable Look ahead Direction(%ld) Full Die(%ld), NextDieDirection = %d", m_stLFDie.lDirection, m_stLFDie.bFullDie, lDirectionTemp);
	CMSLogFileUtility::Instance()->MS60_Log(szTemp);
	//BLOCKPICK
	(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"]	= FALSE;	// op move table
	

	// prescan relative code
	BOOL bGetPrescan = GetPrescanSortPosn(lCalcX, lCalcY, lCalcT);


	m_bThisDieNeedVerify = TRUE;
	if (labs(GetCurrentRow() - GetLastDieRow())<(LONG) m_ulAdvSampleJumpDistance &&
		labs(GetCurrentCol() - GetLastDieCol())<(LONG) m_ulAdvSampleJumpDistance && bDieLFed )
	{
		m_bThisDieNeedVerify = FALSE;
	}

	
	szTemp.Format( "\n#Get Prescan SortPosn(%d, %d)", lCalcX, lCalcY);	//v4.48A29
	CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	

	if (m_ulPitchAlarmLimit > 0 && m_ulPitchAlarmCount > m_ulPitchAlarmLimit)
	{
		if (HmiMessage_Red_Back("Pitch error occured over limit, need ENGINEER to fix?", "Pitch Check", glHMI_MBX_CONTINUESTOP) == glHMI_CONTINUE)
		{
			if (Check201277PasswordToGo())
			{
				SetErrorMessage("Pitch error happened, engineer to continue.");
				m_ulPitchAlarmCount = 0;
			}
			else
			{
				SetErrorMessage("Pitch error happened, wrong password and to stop.");
				return Err_WTableDiePitchFailed;
			}
		}
		else
		{
			SetErrorMessage("Pitch error happened, user to stop.");
			return Err_WTableDiePitchFailed;
		}
	}


	BOOL bGetAlign = FALSE;
	if (m_bJustAlign)
	{
		m_bJustAlign = FALSE;
		m_bThisDieNotLFed	= FALSE;
		m_ulNewPickCounter = 0;

		if (!m_bAOINgPickPartDie && !IsES101() && !IsES201())	//v4.37T10
		{
			if (IsPrescanEnable())
			{
				LONG lRow, lCol, lWftX, lWftY;
				pUtl->GetAlignPosition(lRow, lCol, lWftX, lWftY);
				if (lRow == GetCurrentRow() && lCol == GetCurrentCol())
				{
					bGetAlign = TRUE;
				}
			}
			else
			{
				bGetAlign = TRUE;
			}
		}
	}
	
	m_bThisDieLFed = FALSE;
	CString szDetailLog = "";
	if (bGetAlign)
	{
		if (IsPrescanEnable())
		{
			LONG lRow, lCol;
			pUtl->GetAlignPosition(lRow, lCol, lMoveWfX, lMoveWfY);
			szMoveMsg = "Aln PSCN";
		}
		else
		{
			lMoveWfX = m_lStart_X;
			lMoveWfY = m_lStart_Y;
			szMoveMsg = "Aln Just";
			/*if (IsBlkFunc2Enable())	//Block2
			{
				DieMapPos2 MapPos;
				DiePhyPos2 PhyPos;
				LONG lValueT;
				// Get physical position from wafer map
				MapPos.Row = GetCurrentRow();
				MapPos.Col = GetCurrentCol();
				m_pBlkFunc2->Blk2GetNextDiePosnAndT(MapPos, &PhyPos, &lValueT);

				lMoveWfX = PhyPos.x;
				lMoveWfY = PhyPos.y;
				lMoveWfT = lValueT;
				szMoveMsg = "Aln Blk2";
			}
			else if (IsBlkFunc1Enable())
			{
				DieMapPos MapPos;
				DiePhyPos PhyPos;
				// Get physical position from wafer map
				MapPos.Row = GetCurrentRow();
				MapPos.Col = GetCurrentCol();
				MapPos.Status = 0;	//Klocwork
				m_pBlkFunc->GetNextDiePhyPos(MapPos, &PhyPos);

				lMoveWfX = PhyPos.x;
				lMoveWfY = PhyPos.y;
				lMoveWfT = PhyPos.t;
				szMoveMsg = "Aln Blk1";
			}*/
		}

		bGetPrescan = FALSE;
		bRotate = FALSE;
	}
	/*else if (IsBlkFunc2Enable() && IsPrescanEnable() == FALSE && bGetPrescan == FALSE)	//	Block2
	{
		LONG lX = 0, lY = 0, lT = 0;		//Klocwork
		GetBlk2SortPosn(GetCurrentRow(), GetCurrentCol(), GetLastDieRow(), GetLastDieCol(), lX, lY, lT); 
		szMoveMsg.Format("Blk2 mve (%d,%d)", GetCurrentRow(), GetCurrentCol());

		lMoveWfX = lX;
		lMoveWfY = lY;
		lMoveWfT = lT;
		m_bRotate = IsDiePRRotate();
		bRotate = TRUE;
	}
	else if (IsBlkFunc1Enable() && IsPrescanEnable() == FALSE && bGetPrescan == FALSE)	// BLOCK 1
	{
		DieMapPos MapPos;
		DiePhyPos PhyPos;
		// Get physical position from wafer map
		MapPos.Row = GetCurrentRow();
		MapPos.Col = GetCurrentCol();
		MapPos.Status = 0;	//Klocwork
		m_pBlkFunc->GetNextDiePhyPos(MapPos, &PhyPos);

		lMoveWfX = PhyPos.x;
		lMoveWfY = PhyPos.y;
		lMoveWfT = PhyPos.t;
		bRotate = FALSE;
		m_bRotate = IsDiePRRotate();
		if (m_bRotate == TRUE)
		{
			bRotate = TRUE;
		}

		//No need to compensate T for ALIGN dies
		if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::ALIGN)
		{
			bRotate = FALSE;
		}

		//Relative pitch checking
		szMoveMsg.Format("Blk Fun1");
	}*/
	else
	{
		lDiePitchX_X = labs(lDiePitchX_X);
		lDiePitchY_Y = labs(lDiePitchY_Y);
		
		// Check whether the die is referring to the last Look-Forward result
		if ((GetCurrentCol() == m_stLFDie.lWM_X) && (GetCurrentRow() == m_stLFDie.lWM_Y))
		{
			// Check whether the current die location is already found by last Look-Forward
			if (bDieLFed)
			{
				//v2.96T3	//v2.97T3
				//NO need to perform compensation if curr-die is LF-die without CONFIRM-SEARCH enabled
				BOOL bConfirmSearch = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CFS"]["ConfirmSearch"];
				BOOL bNeedRotate	= m_stLFDie.bRotate;
				BOOL bThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"];
				BOOL bBinThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"];
				BOOL bSpRefCheck = (BOOL)(LONG)(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"];		//Tell WPR to align this PICK die with Ref-Die pattern


				if (m_bEnableMS100EjtXY)
				{
/*
					if (m_stLastLFDie.bFullDie != TRUE)
					{
						(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = TRUE;
						bEjtLF = TRUE;	
						CMSLogFileUtility::Instance()->BPR_Arm1Log("NoComp set to TRUE (1)");
						CMSLogFileUtility::Instance()->MS60_Log("m_bEnableMS100EjtXY m_stLastLFDie.bFullDie = FALSE");
					}
					else if (bSpRefCheck)
					{
						CMSLogFileUtility::Instance()->MS60_Log("m_bEnableMS100EjtXY bSpRefCheck");
						//Do compensate again if last die is special ref check
					}
					else
*/
					if (m_stLFDie.bGoodDie)			// If Alignable die with angle not out
					{
						(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = TRUE;	// look forward good die in op move table
						bEjtLF = TRUE;
						CMSLogFileUtility::Instance()->BPR_Arm1Log("NoComp set to TRUE (GD)");
						CMSLogFileUtility::Instance()->MS60_Log("m_bEnableMS100EjtXY : NoComp set to TRUE (GD)");
					}
					else if (m_stLFDie.bFullDie)
					{
						(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = FALSE;	// Do Compansation
						bEjtLF = TRUE;
						CMSLogFileUtility::Instance()->MS60_Log("m_bEnableMS100EjtXY m_stLFDie.bFullDie = TRUE");
					}
				}
				else if (m_stLastLFDie.bFullDie != TRUE)
				{
					//m_WaferMapWrapper.SetDieState(GetCurrentRow(), GetCurrentCol(), WT_MAP_DIESTATE_LF_NOT_FULL_DIE);
					CMSLogFileUtility::Instance()->WT_GetIdxLog("Op Move Full Die != TRUE");
					CMSLogFileUtility::Instance()->MS60_Log("m_bEnableMS100EjtXY = FALSE m_stLFDie.bFullDie = FALSE");
				}
				else if (bSpRefCheck)
				{
					//Do compensate again if last die is special ref check
					CMSLogFileUtility::Instance()->WT_GetIdxLog("Op Move Special refer die");
					CMSLogFileUtility::Instance()->MS60_Log("m_bEnableMS100EjtXY = FALSE Op Move Special refer die");
				}
				else if (m_stLFDie.bGoodDie)		// If Alignable die with angle not out
				{
					BOOL bCompDone = TRUE;
					if (IsLayerPicking())			//v4.16T1
					{
						bCompDone = FALSE;
					}
					else if (bThetaCorrection || bBinThetaCorrection)
					{
						if ( (bNeedRotate) )		//|| (bConfirmSearch) )	//v4.47T11
						{
							bCompDone = FALSE;
							CMSLogFileUtility::Instance()->WT_GetIdxLog("Need Rotate or Confirm Search");
						}
					}
					
					if( bCompDone )
					{
						CMSLogFileUtility::Instance()->BPR_Arm1Log("NoComp set to TRUE (GD)");	
						(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = TRUE;	// look forward good die in op move table
						bEjtLF = TRUE;		//v4.42T15
					}
				}
				else
				{
					CMSLogFileUtility::Instance()->WT_GetIdxLog("Op Move not good die");
					CMSLogFileUtility::Instance()->MS60_Log("Op Move not good die");
				}
				//2018.4.08
				if (IsBurnIn())
				{
					m_stLFDie.lX = -lDiff_X * lDiePitchX_X;
					m_stLFDie.lY = -lDiff_Y * lDiePitchY_Y;
				}
				//m_stLFDie is Look ahead with 2 Die or not?
				lMoveWfX = GetLastDieEncX() + m_stLFDie.lX;
				lMoveWfY = GetLastDieEncY() + m_stLFDie.lY;

				CString szTemp;
				szTemp.Format("WT - MoveTable Normal map(%ld, %ld): Enc (%ld, %ld), Last-Enc (%ld, %ld), LF-Offset (%ld, %ld),  Pitch (%ld, %ld), MS60LastCycle=%d", 
					GetCurrentRow(), GetCurrentCol(),
					lMoveWfX, lMoveWfY, GetLastDieEncX(), GetLastDieEncY(),
					m_stLFDie.lX, m_stLFDie.lY, lDiePitchX_X, lDiePitchY_Y, m_bMS60NewLFInLastCycle);
				//CMSLogFileUtility::Instance()->MS60_Log(szTemp);
				DisplaySequence(szTemp);

				//v4.47T3
				if (m_bMS60NewLFInLastCycle)
				{
					//2018.4.08
					if (IsBurnIn())
					{
						m_stLFDie.lX = -1 * lDiff_X * lDiePitchX_X;
						m_stLFDie.lY = -1 * lDiff_Y * lDiePitchY_Y;
						m_stMS60LastLFDie.lX_Enc = m_stMS60NewLFDie.lX_Enc;
						m_stMS60LastLFDie.lY_Enc = m_stMS60NewLFDie.lY_Enc;
					}
					lMoveWfX = m_stMS60LastLFDie.lX_Enc + m_stLFDie.lX;
					lMoveWfY = m_stMS60LastLFDie.lY_Enc + m_stLFDie.lY;
					
					CString szTemp;
					szTemp.Format("WT - MoveTable MS60 LF map(%ld, %ld): Enc (%ld, %ld), Last-Enc (%ld, %ld), LF-Offset (%ld, %ld)", 
						GetCurrentRow(), GetCurrentCol(),
						lMoveWfX, lMoveWfY, m_stMS60LastLFDie.lX_Enc, m_stMS60LastLFDie.lY_Enc,
						m_stLFDie.lX, m_stLFDie.lY);
					//CMSLogFileUtility::Instance()->MS60_Log(szTemp);
					//DisplaySequence(szTemp);

					m_bMS60NewLFInLastCycle = FALSE;
					m_stMS60LastLFDie.lX_Enc = 0;
					m_stMS60LastLFDie.lY_Enc = 0;
					m_stMS60LastLFDie.lT_Enc = 0;
				}
				
				//v4.42T15
				/*if (m_bEnableMS100EjtXY)
				{
					LONG lUserRow = 0, lUserCol = 0;
					LONG lMapRow = GetCurrentRow();
					LONG lMapCol = GetCurrentCol();
					ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);

					CString szTemp;
					//v4.48A29
					szTemp.Format("WT MoveTable LF to Die(%ld, %ld): LastEnc(%ld, %ld), LF-Offset(%ld,%ld) - %d %d %d %d %d", 
								lUserRow, lUserCol, GetLastDieEncX(), GetLastDieEncY(), m_stLFDie.lX, m_stLFDie.lY, 
								bEjtLF, m_stLastLFDie.bFullDie, bSpRefCheck, m_stLFDie.bGoodDie, bNeedRotate);
					CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
				}*/

				szDetailLog.Format(" LF %d,%d,ROT %.2f", m_stLFDie.lX, m_stLFDie.lY, m_stLFDie.dTheta);
				if (bThetaCorrection && bNeedRotate)
				{
					bRotate = TRUE;
					RotateWaferTheta(&lMoveWfX, &lMoveWfY, &lMoveWfT, m_stLFDie.dTheta);

					//v3.44		//SHowaDenko
					//Use Multi-COR Offset if enabled
					if (m_bUseMultiCorOffset)
					{
						DOUBLE dDX, dDY;
						LONG lDX, lDY;

						if (lMoveWfT > 0)		//Clockwise
						{
							dDX = m_dCorOffsetA1 * (GetLastDieEncY() - m_lWaferCalibY) + m_dCorOffsetB1;
							dDY = m_dCorOffsetA2 * (GetLastDieEncX() - m_lWaferCalibX) + m_dCorOffsetB2;
							lDX = (LONG) dDX;
							lDY = (LONG) dDY;
						}
						else				//anti-clockwise
						{
							dDX = m_dACorOffsetA1 * (GetLastDieEncY() - m_lWaferCalibY) + m_dACorOffsetB1;
							dDY = m_dACorOffsetA2 * (GetLastDieEncX() - m_lWaferCalibX) + m_dACorOffsetB2;
							lDX = (LONG) dDX;
							lDY = (LONG) dDY;
						}

						lMoveWfX = lMoveWfX + lDX;
						lMoveWfY = lMoveWfY + lDY;
					}
				}

				m_bThisDieLFed = TRUE;
				szMoveMsg.Format("Get LFED");
			}
			else
			{
				CString szTemp;
				szTemp.Format("WT - use PRESCAN coordinate, bDieLFed = %d, cur Map(%d,%d), LA(%d, %d), posn(%d, %d)", 
					bDieLFed, GetCurrentRow(), GetCurrentCol(), m_stLFDie.lWM_Y, m_stLFDie.lWM_X, 
					lCalcX, lCalcY);
				//CMSLogFileUtility::Instance()->MS60_Log(szTemp);
				DisplaySequence(szTemp);

				// die not lookforwarded by PR, but should be ok in map				
// prescan relative code	B
				lMoveWfX = lCalcX;
				lMoveWfY = lCalcY;
				lMoveWfT = lCalcT;
				if (bGetPrescan)
				{
					bRotate = TRUE;
					bPrescanJumpRotate = TRUE;
				}
// prescan relative code	E
				else
				{
					if (bUseLF)
					{
						bRotate = TRUE;
					}
				}
				szMoveMsg.Format("Get NoLF");

				//v4.42T15
				/*if (m_bEnableMS100EjtXY)
				{
					LONG lUserRow = 0, lUserCol = 0;
					LONG lMapRow = GetCurrentRow();
					LONG lMapCol = GetCurrentCol();
					ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);

					CString szTemp;
					szTemp.Format("\nOpMoveTable no-LF to Die(%ld, %ld): Pos(%ld, %ld)", 
									lUserRow, lUserCol, lMoveWfX, lMoveWfY);
					CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
				}*/
			}
		}
		else	//else Long-Jump or LF-disabled
		{
			CString szTemp;
			szTemp.Format("WT- use PRESCAN coordinate #2, bDieLFed = %d, cur Map(%d,%d), LA(%d, %d), posn(%d, %d)", 
						bDieLFed, GetCurrentRow(), GetCurrentCol(), m_stLFDie.lWM_Y, m_stLFDie.lWM_X, 
						lCalcX, lCalcY);
			//CMSLogFileUtility::Instance()->MS60_Log(szTemp);
			DisplaySequence(szTemp);

// prescan relative code	B
			lMoveWfX = lCalcX;
			lMoveWfY = lCalcY;
			lMoveWfT = lCalcT;
			if (bGetPrescan)
			{
				bRotate = TRUE;
				bPrescanJumpRotate = TRUE;
			}
			else
			{
// prescan relative code	E
				if (m_dMoveBackAngle == 0)
				{
					bRotate = TRUE;
				}
				else if ((m_dMoveBackAngle > 0) && (fabs(dTheta) > m_dMoveBackAngle))
				{
					bRotate = TRUE;
				}
			}
			szMoveMsg.Format("Get JUMP");

			//v4.42T15
			if (m_bEnableMS100EjtXY)
			{
				LONG lUserRow = 0, lUserCol = 0;
				LONG lMapRow = GetCurrentRow();
				LONG lMapCol = GetCurrentCol();
				ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);

				CString szTemp;
				szTemp.Format("\nOpMoveTable no-LF 2 to Die(%ld, %ld): Pos(%ld, %ld)", 
								lUserRow, lUserCol, lMoveWfX, lMoveWfY);
				CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	

			}
		}
	}


	CString szTempMsg;
	szTempMsg.Format(" %d,%d,T%d %s", lMoveWfX, lMoveWfY, lMoveWfT, szDetailLog);
	szMoveMsg = szMoveMsg + szTempMsg;
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);
	//v3.49T1		//** andrew: minor-modified IF logic here **
	//Use PHY position by default if LF is not available
	//If next die pos is large than 1, get physcial position from wafer map
	if ((!bUseLF || !bDieLFed) && bGetPrescan)
	{
		// jump case, not by look forward
		lMoveWfX = lCalcX;
		lMoveWfY = lCalcY;
		lMoveWfT = lCalcT;
		bPrescanJumpRotate = TRUE;
		bRotate = TRUE;
		szMoveMsg.Format("WT - Get LAST %d,%d,T%d", lMoveWfX, lMoveWfY, lMoveWfT);
		//CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);
		DisplaySequence(szMoveMsg);
	}

	m_lGetMveWftPosX = lMoveWfX;
	m_lGetMveWftPosY = lMoveWfY;

	if (IsDiePitchCheck(GetCurrentRow(), GetCurrentCol()) && !bDieLFed)	// check predict or prescan pitch , if look forward ok, should not check any more
	{
		INT nCheckPitchStatus = 0;
		if (IsAdvRegionOffset() && IsMapPosnOK())
		{
			nCheckPitchStatus = CheckAdvRegionPitch();
		}
		else if (DEB_IsUseable() && IsScanPosnOK())
		{
			nCheckPitchStatus = CheckDebScanPitch();
		}
		else if (IsPrescanEnable() && IsMapPosnOK())
		{
			nCheckPitchStatus = CheckPrescanPitch(lMoveWfX, lMoveWfY);
		}

		if( nCheckPitchStatus == WT_PITCH_ERR_SCN_PREDCT )	//	move table
		{
			GetEncoderValue();
			LONG lOrgX = GetCurrX();
			LONG lOrgY = GetCurrY();
			XY_SafeMoveTo(lMoveWfX, lMoveWfY);
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = lMoveWfX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = lMoveWfY;
			(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulGetMveMapRow;
			(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulGetMveMapCol;
			CString szMsg;

			if (IsVerifyMapWaferEnable() == FALSE)
			{
				szMsg = "Check map wafer match disable on HMI";
				CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
				SetErrorMessage(szMsg);
				return nCheckPitchStatus;
			}

			GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_PCV);	// pitch error
			WftMoveBondArmToSafe(TRUE);			// move pitch error, verify move
			Sleep(500);

			szMsg.Format("PCE: move table to verify scan-prdt");
			CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			if (CheckMapAndFindWaferMatch(m_ulGetMveMapRow, m_ulGetMveMapCol, lMoveWfX, lMoveWfY, FALSE, FALSE, FALSE) == FALSE)
			{
				szMsg.Format("PCE: move table to prescan tolerance to skip");
				CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);

				m_WaferMapWrapper.SetDieState(m_ulGetMveMapRow, m_ulGetMveMapCol, WT_MAP_DIESTATE_SKIP_PREDICTED);
				m_bMapWaferVerifyOK = FALSE;

				if( IsThisDieDirectPick() || m_lMC[WTMC_MWMV_DoLimit]==0 )
				{
					szMsg.Format("PCE: move table to prescan tolerance, find and match check fail");
					CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
					SetErrorMessage(szMsg);
					return nCheckPitchStatus;
				}
				else
				{
					if( m_ulVerifyMapWaferCounter >= (UINT)m_lMC[WTMC_MWMV_DoLimit] )
					{
						szMsg.Format("PCE: move table to prescan tolerance, find and match check fail");
						CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
						SetErrorMessage(szMsg);
						return nCheckPitchStatus;
					}
				}	//	need to skip and don't let bond arm to pick, 
			}
			else
			{
				LONG lNewOffsetX = lMoveWfX - m_lGetScnWftPosX;
				LONG lNewOffsetY = lMoveWfY - m_lGetScnWftPosY;
				if( m_bGetAvgFOVOffset )
				{
					lNewOffsetX = m_lAvgFovOffsetX;
					lNewOffsetY = m_lAvgFovOffsetY;
				}

				if( IsAdvRegionOffset() )
				{
					if( UpdateRegionDieOffset(m_ulGetMveMapRow, m_ulGetMveMapCol, lNewOffsetX, lNewOffsetY, TRUE)==FALSE )
					{
						szMsg = "Op Move Table update adv region offset error";
						CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
						SetErrorMessage(szMsg);
						SetAlignmentStatus(FALSE);
						m_lWftAdvSamplingResult = -1;
						return WT_PITCH_ERR_ADV_UPDATE;	//	move table
					}
				}

				m_lRunPredScanPitchTolX += 50;
				m_lRunPredScanPitchTolY += 50;
				szMsg.Format("increas pred scan pitch tolerance to %d, %d", m_lRunPredScanPitchTolX, m_lRunPredScanPitchTolY);
				CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
				SetErrorMessage(szMsg);
			}

			if (m_bStop == FALSE)
			{
				WftMoveBondArmToSafe(FALSE);	// move pitch error, verify move
			}
			m_lGetRgnWftPosX = lMoveWfX;
			m_lGetRgnWftPosY = lMoveWfY;
			m_lGetMveWftPosX = lMoveWfX;
			m_lGetMveWftPosY = lMoveWfY;
			XY_SafeMoveTo(lMoveWfX, lMoveWfY);
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = lMoveWfX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = lMoveWfY;
		}	// END IF nCheckPitchStatus
	}


	BOOL bThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"];
	if (bPrescanJumpRotate && bRotate && IsPrescanEnable() && bThetaCorrection && fabs(m_dPrescanRotateDegree) > 0)
	{
		DOUBLE dDiffDegree = 0;
		dDiffDegree = (DOUBLE) m_lThetaMotorDirection * ((DOUBLE)(lMoveWfT) * m_dThetaRes);
		if (m_dPrescanRotateDegree < 0)
		{
			dDiffDegree = 0 - dDiffDegree;
		}

		if (fabs(dDiffDegree) < fabs(m_dPrescanRotateDegree))
		{
			RotateWaferTheta(&lMoveWfX, &lMoveWfY, &lMoveWfT, dDiffDegree);
			CString szMsg;
			szMsg.Format("Rotate back new move posiiton %d,%d T %d", lMoveWfX, lMoveWfY, lMoveWfT);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			lMoveWfT = 0;
			bRotate = FALSE;
		}
	}

	//** Check Wafer Limit **
	if (IsEnablePNP())
	{
		if (IsWithinWaferLimit(lMoveWfX, lMoveWfY) == FALSE)
		{
			lMoveWfX = lLastX;
			lMoveWfY = lLastY;
			// Update die in wafermap
			CString szLogMsg;
			szLogMsg.Format("Out of wafer use old %d,%d", lMoveWfX, lMoveWfY);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogMsg);
		}
	}
	else
	{
		BOOL bNeedOffset = FALSE;
		
		// Check whether the found position is within wafer limit
		if ( IsWithinWaferLimit(lMoveWfX, lMoveWfY, 1, bNeedOffset) == FALSE )
		{
			CString szTemp;
			szTemp.Format("WT: Invalid die position; X = %d, Y = %d", lMoveWfX, lMoveWfY);
			SetErrorMessage(szTemp);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
	
			szTemp.Format("WT: Invalid die pos NAR: FALSE, wm(r%i,c%i), diff(r%i,c%i), enc(%d,%d)",
						  GetCurrentRow(), GetCurrentCol(),	lDiff_Y, lDiff_X,	lMoveWfX, lMoveWfY);
			DisplayMessage(szTemp);
	
			szTemp.Format("    Last die info: (r%i,c%i), enc(%i,%i)", GetLastDieRow(), GetLastDieCol(),
						  GetLastDieEncX(), GetLastDieEncY());
			DisplayMessage(szTemp);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
	
			//v3.76
			if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
			{
				SetAlignmentStatus(FALSE);	//Force to realign wafer if wafer table limit is hit!
			}
			
			if (m_bFullRefBlock == FALSE)	//Block2
			{
				return gnNOTOK;
			}
			else
			{
				return Err_WTableOutOfLimit;
			}
		}
	}

	BOOL bBurnInRotate = FALSE;
	if (IsBurnIn())		//BurnInT
	{
		//andrewng //2020-0617		//Disable this loop for NVC; no need to rotate T (no T motor)
		/*
		m_lCountBurnInT++;
		BOOL bThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"];
		if (bThetaCorrection && m_lCountBurnInT >= 50)
		{
			bBurnInRotate = TRUE;		
			m_lStepBurnInT *= -1;
			m_lCountBurnInT = 0;
			(*m_psmfSRam)["WaferTable"]["Rotate"] = TRUE;	
		}
		*/
	}
	
	m_lX_NoEjOffset = lMoveWfX;
	m_lY_NoEjOffset = lMoveWfY;
	//CString szMoveMsg;
	//szMoveMsg.Format("MoveTable(%d,%d),NewLF,%d",lMoveWfX,lMoveWfY,m_bUseMS60NewLFCycle);
	//CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMoveMsg);
	OpCalculateEjtOffsetXY( lMoveWfX, lMoveWfY, lMoveWfT,
							bEjtLF, TRUE, FALSE, 
							m_stLFDie.bRotate,
							bPickAction);		//v4.42T15	//Need to check LFNoCompenate here
	LONG lIndexTime = 0;
	m_lGetMveWftPosX = lMoveWfX;
	m_lGetMveWftPosY = lMoveWfY;
	//v3.66		//DBH only
	LONG lDistX = lMoveWfX - GetLastDieEncX();
	LONG lDistY = lMoveWfY - GetLastDieEncY();
	if( m_bFirst )
	{
		lDistX = lMoveWfX - GetCurrX();
		lDistY = lMoveWfY - GetCurrY();
	}
	LONG lDistT = lMoveWfT;
	if (bBurnInRotate)
	{
		lDistT = m_lStepBurnInT;
		bRotate = TRUE;
	}
	
	lIndexTime = OpCalculateMoveTableMotionTime(lDistX, lDistY, lDistT, bRotate, lMoveWfX, GetLastDieEncX());
//	if( lIndexTime>BH_WT_MAX_MOTION_TIME )
	{						//	ejector pin scratch mylar
		//SetWTReady(FALSE,"12");	//	SanAn MS100Plus, long jump, BH to prepick,
	}						// then come and pick but WFT moving.

	
	//==========================================
	if (lIndexTime != 0)
	{	
		//lIndexTime += 100;
	}
	else
	{
		DisplaySequence("lIndexTime = 0, WT do not need to move");
	}
	//==========================================

	BOOL bUseSCFProfileY = FALSE;
	bUseSCFProfileY = m_stWTAxis_Y.m_bUseSCFProfile;

	if (CMS896AApp::m_lCycleSpeedMode > 3)
	{
		if (IsThisDieDirectPick())
		{
			if (m_bUseMS60NewLFCycle && !m_bUseLF2ndAlignment)	//andrew //2020-4-16 6:00pm
			{
				if (m_bSyncWTAndEjectorInBond)
				{
					SetWTReady(TRUE, "115");	//Allow Ejector Theta to move down earilier in NVC
				}
				SetCompDone(TRUE, "In OpMoveTable()");				//v4.34T10
			}
			else if (IsBurnIn())
			{
				SetCompDone(TRUE, "In OpMoveTable(BURNIN)");		//andrewng //2020-0617
			}
		}
	}

	TakeTime(WT1);
	X_MoveTo(lMoveWfX, SFM_NOWAIT);
	if (bUseSCFProfileY == FALSE)
	{
		Y_MoveTo(lMoveWfY, SFM_NOWAIT);
	}
	else
	{	
		LONG lMovePosY = 0;
		GetCmdValue();
		lMovePosY = lMoveWfY - GetCurrCmdY();
		Y_ScfProfileMove(lMovePosY, SFM_NOWAIT);
	}

	CString szWTPostion;
	szWTPostion.Format("WT - MoveTo(%d, %d)", lMoveWfX, lMoveWfY);
	DisplaySequence(szWTPostion);
	CMSLogFileUtility::Instance()->MS60_Log(szWTPostion);
	
	//v4.59A39
	if (!OpMoveEjectorTableXY(FALSE, SFM_NOWAIT))
	{
//		SetErrorMessage("ERROR: WT OpMoveEjectorTableXY fail in AUTOBOND mode");
//		SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER);
		return HMB_BH_EJTXY_NO_POWER;
	}


	m_lOrgDieCompen_X = 0;
	m_lOrgDieCompen_Y = 0;
	//v4.06		//For MS100+ CT study
	LONG lUserRow = 0, lUserCol = 0;
	LONG lMapRow = GetCurrentRow();
	LONG lMapCol = GetCurrentCol();
	ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);
	CMSLogFileUtility::Instance()->WT_GetCTLog_Update(lUserRow, lUserCol, lIndexTime, bDieLFed, bRotate, bPickAction);
	szMoveMsg.Format("Get MOVE %d,%d,T%d(%d) map %d,%d(%d,%d)", lMoveWfX, lMoveWfY, lMoveWfT, GetCurrT(), lMapRow, lMapCol, lUserRow, lUserCol);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);
	

	(*m_psmfSRam)["WaferTable"]["Rotate"] = FALSE;	
	if (bBurnInRotate)		//BurnInT
	{
		T_Move(m_lStepBurnInT, SFM_NOWAIT);		
		
		//wait for theta stable (1 degree = 15ms by default)
		Sleep((LONG)(fabs((DOUBLE)(m_lStepBurnInT) * m_dThetaRes) * m_lTDelay));
	}
	else if ((bRotate == TRUE) && (abs(lMoveWfT) >= 3))
	{
		//	T_MoveTo(GetGlobalT(), SFM_NOWAIT);
		T_Move(lMoveWfT, SFM_NOWAIT);
		
		//T_Sync();	//v4.59A2

		(*m_psmfSRam)["WaferTable"]["Rotate"] = TRUE;	

		if ((CMS896AApp::m_lCycleSpeedMode <= 3) || !bPickAction)	//v4.59A2
		{
			T_Sync();	//v4.59A2

			//wait for theta stable (1 degree = 15ms by default)
			Sleep((LONG)(fabs((DOUBLE)(lMoveWfT) * m_dThetaRes) * m_lTDelay));
		}
	}

	if (bPickAction)
	{
		(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;
	}
	else
	{
		(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;
	}

	if (bPickAction && (lIndexTime > 40))	//v3.67T2	//from 30 to 40
	{
		//Sleep(lIndexTime-40);		//Wait for WT's long INDEX motion before letting T to move
	}

	if (!m_bUseMS60NewLFCycle)		//v4.47T5
		SetWTStartMove(TRUE);

	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lMoveWfX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lMoveWfY;


	if (!m_bSel_X && !m_bSel_Y)		//v3.86
	{
		Sleep(25);

		//v4.14T1	//Bug fix to sequence lockup when WT XY is not enabled in MS100Plus dummy-run mode
		//Only available for SpeedMode=4 (MS100+ 8xms CT)
		if ((CMS896AApp::m_lCycleSpeedMode > 3))	
		{
			if (bPickAction && IsThisDieDirectPick())
			{
				SetWTReady(TRUE, "13");	//Allow BH Z1/2 to move down earilier because no need to wait for compensation
			}
		}
	}
	else
	{
		//v4.05	//MS100Plus 8xms speedup
		BOOL bSpeedUpAfterMotionComplete = FALSE;

		//Only available for SpeedMode=4 (MS100+ 8xms CT)
		if ((CMS896AApp::m_lCycleSpeedMode > 3))	
		{
			if (bPickAction)
			{
				BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
				LONG lBHZPickTime = (*m_psmfSRam)["BondHead"]["Z_PickTime"];

				LONG lSpeedUp		= (LONG)(lIndexTime * 1.0 / 5);
				LONG lMaxSPeedUp	= (LONG)(lBHZPickTime / 2.0);
				LONG lIntervalTime  = 0;

				lSpeedUp = 0; //No Speed up in Mega Da

				//v4.21T4	//Speed up for 75ms cycle
				if (!IsMS50() && (lIndexTime < 50))		//v4.59A44
				{
					lSpeedUp = 10;
				}

				if (!bRotate)	//Already check motion complete above, so no need to Sleep() here
				{
					if (bLFNoComp && (CMS896AApp::m_lCycleSpeedMode >= 5))	//v4.37T3	//v4.47T3
					{
/*
						//if ((lIndexTime + 10 - lBHZPickTime) > 0)
						//{
						//	Sleep(lIndexTime + 10 - lBHZPickTime);
						//}
						//v4.43T2	//SanAn 73ms ct testing
						//Reduce extra 3ms delay before letting BHZ down to PICK/BOND level
*/
/*
						if (!m_bUseMS60NewLFCycle)	//v4.59A42	//MS50
						{
							//2018.12.31 , wafer table need 7->20 msec setting time
							if ((lIndexTime + 20 - lBHZPickTime) > 0)
							{
								Sleep(lIndexTime + 20 - lBHZPickTime);
								lIntervalTime = lIndexTime + 20 - lBHZPickTime;
							}
						}
*/
						if (m_bUseMS60NewLFCycle)
						{
							//2019.3.12 , wafer table need 5 msec setting time
							if ((lIndexTime + 5 - lBHZPickTime) > 0)
							{
								Sleep(lIndexTime + 5 - lBHZPickTime);
								lIntervalTime = lIndexTime + 5 - lBHZPickTime;
								szTemp.Format("WT - OpMoveTable #1 (m_bUseMS60NewLFCycle=TRUE), SleepTime=%d, lIndexTime=%d, lBHZPickTime=%d", (lIndexTime + 5 - lBHZPickTime), lIndexTime, lBHZPickTime);
								DisplaySequence(szTemp);
							}
							else
							{
								DisplaySequence("WT - OpMoveTable #1a No SLEEP");
							}
						}
						else
						{
							if ((lIndexTime + 5 - lBHZPickTime) > 0)
							{
								Sleep(lIndexTime + 5 - lBHZPickTime);
								lIntervalTime = lIndexTime + 5 - lBHZPickTime;
								szTemp.Format("WT - OpMoveTable #1 (m_bUseMS60NewLFCycle=FALSE), SleepTime=%d, lIndexTime=%d, lBHZPickTime=%d", (lIndexTime + 5 - lBHZPickTime), lIndexTime, lBHZPickTime);
								DisplaySequence(szTemp);
							}
							else
							{
								DisplaySequence("WT - OpMoveTable #1b No SLEEP");
							}
						}
					}
					else	//bLFNoComp = FALSE (Requires WT compensation)
					{
						//andrewng //2020-0617
						if (IsBurnIn())
						{
							lSpeedUp = lIndexTime;
						}

						if ((lIndexTime - lSpeedUp) > 0)		//v4.22T1
						{
							Sleep(lIndexTime - lSpeedUp);
							szTemp.Format("WT - OpMoveTable #2, SleepTime=%d, lIndexTime=%d, lSpeedUp=%d", (lIndexTime - lSpeedUp), lIndexTime, lSpeedUp);
							DisplaySequence(szTemp);
							lIntervalTime = lIndexTime - lSpeedUp;
						}
						else
						{
							DisplaySequence("WT - OpMoveTable #2 No SLEEP");
						}

						//andrewng //2020-0617
						if (IsBurnIn() && !pApp->m_bBurnInGrabImage)
						{
							SetWTReady(TRUE,	"OpMoveTable #2 for BURNIN Mode");
							SetCompDone(TRUE,	"OpMoveTable #2 for BURNIN Mode");
						}
					}
				}
				else	//else bRotate = TRUE (current DIE is rotated)
				{
					//andrewng //2020-0617
					if (IsBurnIn())
					{
						lSpeedUp = lIndexTime;
					}

					if ((lIndexTime - lSpeedUp) > 0)
					{
						Sleep(lIndexTime - lSpeedUp);
						szTemp.Format("WT - OpMoveTable #3, SleepTime=%d,lIndexTime=%d,lSpeedUp=%d", (lIndexTime - lSpeedUp), lIndexTime, lSpeedUp);
						DisplaySequence(szTemp);
						lIntervalTime = lIndexTime - lSpeedUp;
					}
					else
					{
						DisplaySequence("WT - OpMoveTable #3 No SLEEP");
					}

					//andrewng //2020-0617
					if (IsBurnIn() && !pApp->m_bBurnInGrabImage)
					{
						SetWTReady(TRUE,	"OpMoveTable #3 for BURNIN Mode");
						SetCompDone(TRUE,	"OpMoveTable #3 for BURNIN Mode");
					}
				}

				if (lSpeedUp < lMaxSPeedUp)
				{
					if (IsThisDieDirectPick())
					{
						if (!m_bSyncWTAndEjectorInBond)
						{
							SetWTReady(TRUE, "114");	//Allow BH Z1/2 to move down earilier because no need to wait for compensation  
						}	
					}
					//else if (IsBurnIn() && (CMS896AApp::m_lCycleSpeedMode >= 4))
					//{
					//	SetWTReady(TRUE,"15");		//v4.01T1	//Allow 100ms BURNIN cycle even without LF data
					//}
					else if (bLFNoComp && (CMS896AApp::m_lCycleSpeedMode >= 5) && !m_bUseLF2ndAlignment)	//andrewng	//2020-4-15
					{
						SetWTReady(TRUE, "16");
					}
				}
				else
				{
//					TakeData(LC4, lSpeedUp); 2018.3.5

					//Set WTReady after XYT motion complete below, if motion time is too long!
					bSpeedUpAfterMotionComplete = TRUE;
				}

				//2017.12.28=========================================================
				//turn on Ejector Vacuum On before wafer table stop to move
				//default is 7 ms
				if (m_bUseMS60NewLFCycle) //only for MS50
				{
					if (lIntervalTime < lIndexTime - 7)
					{
						Sleep(lIndexTime - 7 - lIntervalTime);
						szTemp.Format("WT - OpMoveTable #4, SleepTime=%d, lIndexTime=%d, lIntervalTime=%d", (lIndexTime - 7 - lIntervalTime), lIndexTime, lIntervalTime);
						DisplaySequence(szTemp);
					}
					else
					{
						DisplaySequence("WT - OpMoveTable #4 No SLEEP");
					}
					if ( GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK )
					{
						if( !IsLayerPicking() )
						{
							DisplaySequence("Turn On Ejector Vacuum Before WT Move End");
							SetEjectorVacuum(TRUE);			// Turn-on Ejector Vacuum after comp.
							CString szMsg;
							szMsg.Format("lIndexTime = %d, %d", lIntervalTime, lIndexTime);
							CMSLogFileUtility::Instance()->MS60_Log(szMsg);
						}
					}
				}
				//===================================================================
			}
			else
			{
				//Sleep(100);
				//szTemp = "MegaDa WT Move #5, SleepTime=100";
				DisplaySequence("WT - OpMoveTable #5, NoPick Action wait for compensation");
			}
		}


		X_Sync();
		Y_Sync();
		if (bBurnInRotate || bRotate)	//v3.66 //DBH only
		{
			T_Sync();
		}

		//v4.05	//MS100Plus 8xms speedup
		if (bSpeedUpAfterMotionComplete)	//if this flag is set from above...
		{
			if (IsThisDieDirectPick())
			{
				SetWTReady(TRUE, "17");		//Allow BH Z1/2 to move down earilier because no need to wait for compensation  
			}
			//else if (IsBurnIn() && (CMS896AApp::m_lCycleSpeedMode >= 4))
			//{
			//	SetWTReady(TRUE,	"18");		//v4.01T1	//Allow 100ms BURNIN cycle even without LF data
			//	SetCompDone(TRUE,	"18");
			//}
		}
	}

	//TakeTime(WT2);		// Take Time
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	X_SelectControl(PL_STATIC);
	Y_SelectControl(PL_STATIC);

	return gnOK;
}


LONG CWaferTable::OpCalculateMoveTableMotionTime(LONG lDistX, LONG lDistY, LONG lMoveWfT, 
												 BOOL bRotate, LONG lX1, LONG lX2)
{
	BOOL bUseSCFProfileY = FALSE;
	LONG lTime_X = 0, lTime_Y = 0, lTime_T = 0;

#ifdef NU_MOTION
	bUseSCFProfileY = m_stWTAxis_Y.m_bUseSCFProfile;
#endif

	lTime_X = X_ProfileTime(NORMAL_PROF, lDistX, lDistX);

	if (bUseSCFProfileY == FALSE)
	{
		lTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);
	}
	else
	{
		// scf profile must use normal
		lTime_Y = Y_ScfProfileTime(NORMAL_PROF, lDistY, lDistY);
	}
	
	lTime_T = 0;

	if ((bRotate == TRUE) && (abs(lMoveWfT) >= 3))
	{
		if (m_bSel_T)
		{
			lTime_T = T_ProfileTime(NORMAL_PROF, lMoveWfT, lMoveWfT);
		}
	}

#ifdef NU_MOTION
	if (labs(lDistX) > GetSlowProfileDist())
	{
		INT nResult = 0;
		nResult = X_Profile(LOW_PROF);
		if (nResult != gnOK)
		{
			AfxMessageBox("Fail to access SLOW WT profile ....", MB_SYSTEMMODAL);
		}
		lTime_X = X_ProfileTime(LOW_PROF, lDistX, lDistX);
	}
	else
	{
		// prober rod motors
		X_Profile(NORMAL_PROF);
	}	

	if (bUseSCFProfileY == FALSE)
	{
		if (labs(lDistY) > GetSlowProfileDist())
		{
			INT nResult = 0;
			nResult = Y_Profile(LOW_PROF);
			if (nResult != gnOK)
			{
				AfxMessageBox("Fail to access SLOW WT profile ....", MB_SYSTEMMODAL);
			}
			lTime_Y = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
		}
		else
		{
			{
				Y_Profile(NORMAL_PROF);
				lTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);
			}
		}
	}
	else
	{
		// must use normal profile for scf profile
		Y_Profile(NORMAL_PROF);
	}

#else
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	LONG lIndexTime = lTime_X;
	if (lTime_Y > lTime_X)
	{
		lIndexTime = lTime_Y;
	}
	LONG lExtraTSleepDelay = 0;
	if ((bRotate == TRUE) && (abs(lMoveWfT) >= 3) && (lTime_T > lIndexTime))
	{
		lExtraTSleepDelay = (LONG)(fabs((DOUBLE)(lMoveWfT) * m_dThetaRes) * m_lTDelay);
		LONG lIndexTimeT = lTime_T + lExtraTSleepDelay;

		//v4.59A2
		if (lIndexTimeT > lIndexTime)
			lIndexTime = lIndexTimeT;
	}

	if (!IsBurnIn())
	{
		LONG lWTSettlingDelay = (*m_psmfSRam)["WaferTable"]["SettlingDelay"];
		lIndexTime = lIndexTime + lWTSettlingDelay;
	}

	(*m_psmfSRam)["WaferTable"]["IndexTime"]				= lIndexTime;
	(*m_psmfSRam)["WaferTable"]["IndexStart"]				= GetTime();


	//v4.59A2
	if (bRotate)	
	{
		CString szLog;
		szLog.Format("WT Motion Time: X=%ld, Y=%ld, T=%ld (T-Dist=%ld), Extra-T delay=%ld, Final INDEX-Time = %ld",
						lTime_X, lTime_Y, lTime_T, lMoveWfT, lExtraTSleepDelay, lIndexTime);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}


	UCHAR ucCurGrade = GetCurrDieGrade();
	//Mixed bin sorting
	if ((m_bSortMultiToOne || m_ulMinGradeCount > 0) && (m_ulMinGradeBinNo > 0))
	{
		for (INT i = 0; i < m_unArrMinGradeBin.GetSize(); i++)
		{
			UCHAR ucGrade = m_unArrMinGradeBin.GetAt(i);
			if (ucGrade == ucCurGrade)
			{
				ucCurGrade = (UCHAR) (m_ulMinGradeBinNo + m_WaferMapWrapper.GetGradeOffset());		//Always map to Grade-1
				break;
			}
		}
	}

	if (m_bMultiGradeSortingtoSingleBin)	//Osram Germany Laer-sort fcn
	{
		ucCurGrade = 1 + m_WaferMapWrapper.GetGradeOffset();
	}
	// log down current map die information.
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"]		= GetCurrentRow();		//v4.15T2
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"]		= GetCurrentCol();		//v4.15T2
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"]		= ucCurGrade;	//v4.15T2
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"]	= GetCurrDieEAct();	//v4.15T2

	return lIndexTime;
}


BOOL CWaferTable::OpIsCurrentLFDie()				//v3.89		//MS100
{
//	if (IsScnLoaded())
//		return FALSE;
//	if (IsBlkFunc1Enable())
//		return FALSE;
//	if (IsBlkFunc2Enable())
//		return FALSE;

	if ((GetCurrentCol() == m_stLFDie.lWM_X) && (GetCurrentRow() == m_stLFDie.lWM_Y))
	{
		BOOL bConfirmSearch = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CFS"]["ConfirmSearch"];
		BOOL bNeedRotate	= m_stLFDie.bRotate;
		BOOL bThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"];

		if (m_stLFDie.bGoodDie)			// If Alignable die with angle not out
		{
			if( bThetaCorrection==FALSE )
			{
				return TRUE;
			}

			if (!bNeedRotate)				//1
			{
				return TRUE;
			}
			else if (!bConfirmSearch)		//2
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


////////////////////////////////////////////
//	Update Die State 
////////////////////////////////////////////

INT CWaferTable::OpUpdateDie()
{
	// Update the last die pos wafer map
	return UpdateMap(GetLastDieRow(), GetLastDieCol(), m_stLastDie.ucGrade, GetLastDieEAct(), m_stLastDie.ulStatus);
}


////////////////////////////////////////////
//	Update LF Die State for Smart-Walk 
////////////////////////////////////////////

INT CWaferTable::OpLFDieStatusForSmartWalk()
{
	INT	nResult	= gnOK;

	// Save the Look-Forward data for next die
	BOOL bUseLookForward = IsUseLF();

	if (!bUseLookForward)
	{
		return nResult;
	}

	if (m_stLFDie.lDirection != -1)			//Make sure LF result is valid before update MAP	//v3.01T5
	{
		if (!m_stLFDie.bFullDie)				//If not alignable, NO_DIE!!
		{
			m_WaferMapWrapper.SetDieState(m_stLFDie.lWM_Y, m_stLFDie.lWM_X, WT_MAP_DS_PR_EMPTY);
		}
		else if (!m_stLFDie.bGoodDie)			//Else if alignable but not good, DEFECT_DIE!!
		{
			m_WaferMapWrapper.SetDieState(m_stLFDie.lWM_Y, m_stLFDie.lWM_X, WT_MAP_DS_PR_DEFECT);
		}
	}

	return nResult;
}


////////////////////////////////////////////
//	Next Die State 
////////////////////////////////////////////

INT CWaferTable::OpNextDie()
{
	BOOL bEndOfWafer = FALSE;
	unsigned long ulMapDieCol = 0, ulMapDieRow = 0;
	unsigned char ucGrade = '0';

	LONG lCurrPhX	= 0;
	LONG lCurrPhY	= 0;
	LONG lCurrPhT	= 0;

	T_Sync();
	GetEncoderValue();		
	m_stLastDie = m_stCurrentDie;

	//v4.55A7	//NNS MS60 BH debugging using ColletHole Test
	if (!CMS896AStn::m_bMS60NGPick)
	{
		LONG lMapRow = m_stLastDie.lY;
		LONG lMapCol = m_stLastDie.lX;
		LONG lUserRow=0, lUserCol=0;
		ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);
		(*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"]	= lUserRow;
		(*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"]	= lUserCol;
	}

	//v4.42T15
	LONG lSavePhyX = GetCurrX() + m_lOrgDieCompen_X;
	LONG lSavePhyY = GetCurrY() + m_lOrgDieCompen_Y;

	lCurrPhX = m_stLastDie.lX_Enc = lSavePhyX;
	lCurrPhY = m_stLastDie.lY_Enc = lSavePhyY;
	lCurrPhT = m_stLastDie.lT_Enc = GetCurrT();
	TakeTime(LC9);

	CString szLog;
	szLog.Format("WT OpNextDie(): Update m_stLastDie.ENC(%ld, %ld, %ld)", m_stLastDie.lX_Enc, m_stLastDie.lY_Enc, m_stLastDie.lT_Enc);
	CMSLogFileUtility::Instance()->MS60_Log(szLog);

	szLog.Format("WT OpNextDie(): Update m_stLastDie.CMD(%ld, %ld)", m_lCmd_X, m_lCmd_Y);
	CMSLogFileUtility::Instance()->MS60_Log(szLog);

	(*m_psmfSRam)["WaferTable"]["Current"]["Theta"] = lCurrPhT;

	CString szLogText;

	CString szPickLog;
	szPickLog.Format("PHYS %d,%d,T(%d)", GetCurrX(), GetCurrY(), GetCurrT());

	BOOL bPickLeft = (BOOL)(LONG)((*m_psmfSRam)["WaferMap"]["NgPick"]["PickLeft"]);
	if (m_bAOINgPickPartDie && bPickLeft && GetNewPickCount() != 0)	// Op next die, use current one
	{
		// Set physical position into wafermap
		if (IsMotionCE() == FALSE)
		{
			szLogText.Format("Set (%ld,%ld) PICK %d,%d,T(%d)", GetLastDieRow(), GetLastDieCol(), lCurrPhX, lCurrPhY, lCurrPhT);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
			SetMapPhyPosn(GetLastDieRow(), GetLastDieCol(), lCurrPhX, lCurrPhY);
		}

		if (m_bGoodDie == TRUE)
		{
			m_ucLastPickDieGrade = m_stLastDie.ucGrade;
			m_bGoodDieFound = TRUE;
		}

		return gnOK;
	}

	// Get the die info again - Should be the same as before. Otherwise, error
    WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	if (GetMapNextDie(ulMapDieRow, ulMapDieCol, ucGrade, eAction, bEndOfWafer) == FALSE)
	{
		if( RestartDefectDieSort() )
		{
			if (GetMapNextDie(ulMapDieRow, ulMapDieCol, ucGrade, eAction, bEndOfWafer) == FALSE)
			{
				return gnNOTOK;
			}
		}
		else
		{
			CMSLogFileUtility::Instance()->WT_GetIdxLog("Op Next die false Return");
			return gnNOTOK;
		}
	}

	if (IsEnablePNP())
	{
		bEndOfWafer = FALSE;
	}

	if ((ulMapDieCol != (unsigned long)GetCurrentCol()) ||
		(ulMapDieRow != (unsigned long)GetCurrentRow())	||
		(ucGrade != GetCurrDieGrade())	||
		(bEndOfWafer == TRUE))
	{
		//GetDie Log
		CString szLogText;
		szLogText.Format("MIS %d,%d,%d,<=>%d,%d,%d", ulMapDieRow, ulMapDieCol, ucGrade, GetCurrentRow(), GetCurrentCol(), GetCurrDieGrade());
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);

		if (IsEnablePNP() == FALSE && IsBlkFunc2Enable() == FALSE)
		{
			m_stLastDie.lX = ulMapDieCol;
			m_stLastDie.lY = ulMapDieRow;

			CString szPreAlgorithm;
			CString szPrePath;
			int nCol = 0;
			m_WaferMapWrapper.GetAlgorithm(szPreAlgorithm, szPrePath);

			nCol = szPreAlgorithm.Find("Pick and Place");
			if (nCol != -1)
			{
				//Only handle on Pick & Place
				GetMapPhyPosn(ulMapDieRow, ulMapDieCol, lCurrPhX, lCurrPhY);
				m_stLastDie.lX_Enc = lCurrPhX;
				m_stLastDie.lY_Enc = lCurrPhY;
				lCurrPhT = GetGlobalT();
				(*m_psmfSRam)["WaferTable"]["Current"]["Theta"] = lCurrPhT;
				CMSLogFileUtility::Instance()->WT_GetIdxLog("WT: mismatch true and die position wrong");
			}
		}
	}

	if (IsBlkFunc1Enable())
	{
		DieMapPos CurDieMap;
		DiePhyPos CurDiePhy;
		CurDieMap.Row = GetLastDieRow();
		CurDieMap.Col = GetLastDieCol();
		CurDieMap.Status = GetLastDieEAct();
		CurDiePhy.x = lCurrPhX;
		CurDiePhy.y = lCurrPhY;
		CurDiePhy.t = lCurrPhT;
		m_pBlkFunc->SetCurDiePhyPos(CurDieMap, CurDiePhy);
	}

	LONG lTheta		= 0;
	DOUBLE dTheta	= 0;
	// Set physical position into wafermap
	dTheta = (DOUBLE)(GetGlobalT() - lCurrPhT);
	if (fabs(dTheta) < 5.0)
	{
		dTheta = 0.0;
	}
	dTheta = -(DOUBLE)m_lThetaMotorDirection * (dTheta * m_dThetaRes);
	RotateWaferTheta(&lCurrPhX, &lCurrPhY, &lTheta, dTheta);

	CString szTempTail = "";
	INT	nOpNextDieResult = gnOK;

	BOOL	bUpdateBack = TRUE;
	BOOL	bThetaCorrection	= (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"];
	DOUBLE	dAcceptDieAngle		= (*m_psmfSRam)["WaferPr"]["Theta Accept Angle"];
	DOUBLE	dMaxDieAngle		= (*m_psmfSRam)["WaferPr"]["Theta Max Die Angle"];
	if ( bThetaCorrection ) 
	{
		if (fabs(dTheta) > dMaxDieAngle)
		{
			bUpdateBack = FALSE;
			szTempTail.Format("Angle %f over correct limit %f, no update", dTheta, dMaxDieAngle);
		}
	}
	else
	{
		if ((fabs(dTheta) > dAcceptDieAngle) && !IsNGBlock(m_stLastDie.lY, m_stLastDie.lX))
		{
			bUpdateBack = FALSE;
			szTempTail.Format("Angle %f over Accept limit %f, no update", dTheta, dAcceptDieAngle);
		}
	}

	//	m_stCurrentDie.ulStatus==	ULONG lDieStatus = (ULONG)(LONG)(*m_psmfSRam)["WaferPr"]["DieResult"];
	if( m_stLastDie.ulStatus==WT_MAP_DS_PR_ROTATE )
	{
		bUpdateBack = FALSE;
	}

	if (IsMotionCE() == FALSE)
	{
		LONG oldx = lCurrPhX;
		LONG oldy = lCurrPhY;
		LONG lLastRow = GetLastDieRow();
		LONG lLastCol = GetLastDieCol();
		LONG lUserRow, lUserCol;
		ConvertAsmToOrgUser(lLastRow, lLastCol, lUserRow, lUserCol);
		CString szPreScan = "";
		if (m_bEnableMS100EjtXY && m_bEnableMS100EjtXY)
		{
			LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
			LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
			GetColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, (m_dXYRes * 1000), TRUE);

			if (m_nWTAtColletPos == 2)			//WT from Collet2 to CENTER
			{
				lCurrPhX = lCurrPhX - lCollet2OffsetX;
				lCurrPhY = lCurrPhY - lCollet2OffsetY;
				szPreScan.Format( "~~~Collet 2 map prescan-go shift: old-(%d,%d) shifted-(%d,%d)", oldx, oldy, lCurrPhX, lCurrPhY);
			}
			else if (m_nWTAtColletPos == 1)		//WT from Collet1 to CENTER
			{
				lCurrPhX = lCurrPhX - lCollet1OffsetX;
				lCurrPhY = lCurrPhY - lCollet1OffsetY;
				szPreScan.Format( "~~~Collet 1 map prescan-go shift: old-(%d,%d) shifted-(%d,%d)", oldx, oldy, lCurrPhX, lCurrPhY);
			}
			else
			{
				szPreScan.Format( "~~~map prescan-go no shift: (%d,%d)", lCurrPhX ,lCurrPhY );
			}	
		}

		if (bUpdateBack)
		{
			SetMapPhyPosn(lLastRow, lLastCol, lCurrPhX, lCurrPhY);
		}

		if (IsBlkFunc2Enable())	//Block2
		{
			LONG lStatus;
			if (GetLastDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
			{
				lStatus = REGN_HPICK2;
			}
			else if (GetLastDieEAct() == WAF_CDieSelectionAlgorithm::MISSING ||
					 GetLastDieEAct() == WAF_CDieSelectionAlgorithm::INVALID)
			{
				lStatus = REGN_EMPTY2;
			}
			else
			{
				lStatus = REGN_ALIGN2;
			}

			if( bUpdateBack )
				m_pBlkFunc2->Blk2SetCurDiePhyPos(lLastRow, lLastCol, lCurrPhX, lCurrPhY, lStatus);
		}

		if (m_bThisDieLFed && IsAutoSampleSort() && IsDiePitchCheck(lLastRow, lLastCol) && IsMapPosnOK() && IsScanPosnOK() &&
			(m_lLFPitchToleranceX!=0 || m_lLFPitchToleranceY!=0) && m_bUseMS60NewLFCycle)
		{
			LONG lTgtMoveX = 0, lTgtMoveY = 0;
			if( DEB_IsUseable()	)
			{
				lTgtMoveX = m_lGetAtcWftPosX;
				lTgtMoveY = m_lGetAtcWftPosY;
			}
			else if( IsAdvRegionOffset() )
			{
				lTgtMoveX = m_lGetRgnWftPosX;
				lTgtMoveY = m_lGetRgnWftPosY;
			}
			else if( IsAutoRescanEnable() )
			{
				lTgtMoveX = m_lGetScnWftPosX;
				lTgtMoveY = m_lGetScnWftPosY;
			}
			LONG lOffsetX = m_lX_NoEjOffset/*lCurrPhX*/ - lTgtMoveX;
			LONG lOffsetY = m_lY_NoEjOffset/*lCurrPhY*/ - lTgtMoveY;
			CString szMoveMsg;
			//szMoveMsg.Format("Move(%d,%d),Target(%d,%d)",m_lX_NoEjOffset,m_lY_NoEjOffset,lTgtMoveX,lTgtMoveY);
			//CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMoveMsg);
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

			//LONG lTemp				= pApp->GetProfileInt(gszPROFILE_SETTING, _T("PitchOverTooMuch"), 0);
			//szMoveMsg.Format("temp%d",lTemp);
			//CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMoveMsg);
			//if (lTemp > 300)
			//{
			//	lTemp = 300;
			//}
			if (!CheckLFPitchX(lOffsetX, 90) || !CheckLFPitchY(lOffsetY, 90))
			{
				//m_lLFPitchErrorCount2++;
				bUpdateBack	= FALSE;
				szMoveMsg.Format("die pitch over too much,at map %ld,%ld(%ld,%ld),Physical(%ld,%ld),Target(%ld,%ld),Compare(%d,%d)offset,%ld,%ld,counter,%ld",
					lLastRow, lLastCol, lUserRow, lUserCol,
					lCurrPhX, lCurrPhY, lTgtMoveX, lTgtMoveY, m_lX_NoEjOffset, m_lY_NoEjOffset, lOffsetX, lOffsetY, m_lLFPitchErrorCount2);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);
				CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMoveMsg);
				//SetErrorMessage(szMoveMsg);
				//if (m_lLFPitchErrorCount2 >= 2)
				//{
					nOpNextDieResult = WT_PITCH_OVER_TOO_MUCH;	//	move table LF - Prediction
				//}
				//nOpNextDieResult = WT_PITCH_ERR_ADV_UPDATE;
			}
			else if (!CheckLFPitchX(lOffsetX, m_lLFPitchToleranceX) || !CheckLFPitchY(lOffsetY, m_lLFPitchToleranceY))
			{
				m_lLFPitchErrorCount++;
				bUpdateBack	= FALSE;
				szMoveMsg.Format("DEB%d,ADV%d,RESCAN%d,ERR LFPD LF,at map %ld,%ld(%ld,%ld),Physical(%ld,%ld),Target(%ld,%ld),Compare(%d,%d)offset,%ld,%ld,counter,%ld",
					DEB_IsUseable(),IsAdvRegionOffset(),IsAutoRescanEnable(),lLastRow, lLastCol, lUserRow, lUserCol,
					lCurrPhX, lCurrPhY, lTgtMoveX, lTgtMoveY, m_lX_NoEjOffset, m_lY_NoEjOffset, lOffsetX, lOffsetY, m_lLFPitchErrorCount);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);
				CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMoveMsg);
				//SetErrorMessage(szMoveMsg);
				if (m_lLFPitchErrorLimit > 0 && m_lLFPitchErrorCount >= m_lLFPitchErrorLimit)
				{
					nOpNextDieResult = WT_PITCH_ERR_ADV_UPDATE;	//	move table LF - Prediction
				}
			}
		}
		else
		{
			m_lLFPitchErrorCount = 0;
		}

		LONG lScnWftPosX = m_lGetScnWftPosX;
		LONG lScnWftPosY = m_lGetScnWftPosY;
		LONG lPrdWftPosX = lScnWftPosX;
		LONG lPrdWftPosY = lScnWftPosY;
		LONG lScnOffsetX = lCurrPhX - lScnWftPosX;
		LONG lScnOffsetY = lCurrPhY - lScnWftPosY;
		if (IsAdvRegionOffset() && IsMapPosnOK() && m_bGoodDie)
		{
			lPrdWftPosX = m_lGetRgnWftPosX;
			lPrdWftPosY = m_lGetRgnWftPosY;
			LONG lOffsetX = lScnOffsetX;
			LONG lOffsetY = lScnOffsetY;
			if( bUpdateBack==FALSE )
			{
				lOffsetX = 0;
				lOffsetY = 0;
			}
			if( m_bGetAvgFOVOffset==FALSE )
			{
				if( UpdateRegionDieOffset(lLastRow, lLastCol, lOffsetX, lOffsetY)==FALSE )
				{
					SetAlignmentStatus(FALSE);
					m_lWftAdvSamplingResult = -1;
					CString szMsg = "Op Next Die update adv region offset error";
					CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
					SetErrorMessage(szMsg);
					nOpNextDieResult = WT_PITCH_ERR_ADV_UPDATE;	//	next die
				}
			}
		}
		m_bGetAvgFOVOffset = FALSE;

		//if (DEB_IsUseable() && IsScanPosnOK())		//v3.57T4
		//{
		//	lPrdWftPosX = m_lGetAtcWftPosX;
		//	lPrdWftPosY = m_lGetAtcWftPosY;
		//	INT nLastDieStatus;
		//	if (GetLastDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
		//	{
		//		nLastDieStatus = MS_Picked;
		//	}
		//	else if (GetLastDieEAct() == WAF_CDieSelectionAlgorithm::ALIGN)
		//	{
		//		nLastDieStatus = MS_Align;
		//	}
		//	else if (GetLastDieEAct() == WAF_CDieSelectionAlgorithm::MISSING)
		//	{
		//		nLastDieStatus = MS_Empty;
		//	}
		//	else
		//	{
		//		nLastDieStatus = MS_AtRef;
		//	}
	
		//	LONG lUpdX = lCurrPhX;
		//	LONG lUpdY = lCurrPhY;
		//	if( bUpdateBack==FALSE )
		//	{
		//		lUpdX = lPrdWftPosX;
		//		lUpdY = lPrdWftPosY;
		//	}
		//	DEB_UpdateDiePos(lLastRow, lLastCol, lUpdX, lUpdY, nLastDieStatus);
		//}

		LONG lPrdOffsetX = lCurrPhX - lPrdWftPosX;
		LONG lPrdOffsetY = lCurrPhY - lPrdWftPosY;

		szTempTail += szPreScan;
		szLogText.Format("SET PHYS %d,%d,T%d(%d), (%ld,%ld) at %s %s,Update,%d,WTNoEj,%d,%d",
			lCurrPhX, lCurrPhY, lTheta, lCurrPhT, lLastRow, lLastCol, szPickLog, szTempTail, bUpdateBack,m_lX_NoEjOffset,m_lY_NoEjOffset);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);

		if( IsAutoSampleSort() )
		{
							//row,col,row,col,scn,scn,prd,prd,pik,pik,
			szLogText.Format("%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,",
				lLastRow, lLastCol, lUserRow, lUserCol, lScnWftPosX, lScnWftPosY, lPrdWftPosX, lPrdWftPosY, lCurrPhX, lCurrPhY);
			if( DEB_IsUseable() )
			{
								//p-d,p-d,pd%,pd%,LF,
				szTempTail.Format("%ld,%ld,%ld,%ld,%d,",
					lPrdOffsetX, lPrdOffsetY, lPrdOffsetX*100/GetDiePitchX_X(), lPrdOffsetY*100/GetDiePitchY_Y(), m_bThisDieLFed);
				szLogText += szTempTail;
			}
			CTime theTime = CTime::GetCurrentTime();
			CString szTime = theTime.Format("%H:%M:%S");
			szTempTail.Format("%d,%s,", GetCurrDieGrade()-m_WaferMapWrapper.GetGradeOffset(), szTime);
			szLogText += szTempTail;

			LONG lDirX = 0, lDirY = 0;
			if( lScnOffsetX>0 )
				lDirX = 1;
			if( lScnOffsetX<0 )
				lDirX = -1;
			if( lScnOffsetY>0 )
				lDirY = 1;
			if( lScnOffsetY<0 )
				lDirY = -1;
			szTempTail.Format("%3ld,%3ld,%2d,%2d,", lScnOffsetX, lScnOffsetY, lDirX, lDirY);
			szLogText += szTempTail;

			LONG lOldX = lScnWftPosX, lOldY = lScnWftPosY;
			GetPrescanRunPosn(lLastRow, lLastCol, lOldX, lOldY);
			LONG lOffsetX = lCurrPhX - lOldX;
			LONG lOffsetY = lCurrPhY - lOldY;
			lDirX = lDirY = 0;
			if( lCurrPhX>lOldX )
				lDirX = 1;
			if( lCurrPhX<lOldX )
				lDirX = -1;
			if( lCurrPhY>lOldY )
				lDirY = 1;
			if( lCurrPhY<lOldY )
				lDirY = -1;
			szTempTail.Format("%3ld,%3ld,%2d,%2d",
					lOffsetX, lOffsetY, lDirX, lDirY);
			szLogText += szTempTail;

			CMSLogFileUtility::Instance()->WT_WriteMylarShiftLog(szLogText);
		}

		if( m_bEnableXCheck && (m_lXCheckToleranceX!=0 || m_lXCheckToleranceY!=0) )
		{
			BOOL bPitchFail = FALSE;
			if( m_lXCheckToleranceX!=0 )
			{
				if( labs(lScnOffsetX)>labs(GetDiePitchX_X()*m_lXCheckToleranceX/100) )
				{
					bPitchFail = TRUE;
				}
			}
			if( m_lXCheckToleranceY!=0 )
			{
				if( labs(lScnOffsetY)>labs(GetDiePitchY_Y()*m_lXCheckToleranceY/100) )
				{
					bPitchFail = TRUE;
				}
			}
			if( bPitchFail )
			{
				szLogText.Format("Pick-Scan pitch error at map %ld,%ld,(%ld,%ld)",
					lLastRow, lLastCol, lUserRow, lUserCol);
				HmiMessage_Red_Back(szLogText, "Auto Cycle");
				szTempTail.Format(" pick %ld,%ld, scan %ld,%ld, offset %ld,%ld",
					lCurrPhX, lCurrPhY, lScnWftPosX, lScnWftPosY, lScnOffsetX, lScnOffsetY);
				szLogText += szTempTail;
				szTempTail.Format(" tolerance %d,%d", m_lXCheckToleranceX, m_lXCheckToleranceY);
				szLogText += szTempTail;
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
				nOpNextDieResult = WT_PITCH_ERR_ADV_UPDATE;
			}
		}
	}

	if (m_bGoodDie == TRUE)
	{
		m_ucLastPickDieGrade = m_stLastDie.ucGrade;
		m_bGoodDieFound = TRUE;
	}

	return nOpNextDieResult;
}

////////////////////////////////////////////
//	Compensation State 
////////////////////////////////////////////
INT CWaferTable::OpCompensation(BOOL bGoodDie)	// for wait die ready (pr done) to do move direct or indirectly
{
	CString szMsg;

	BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];

	LONG lCompen_X = GetDiePROffsetX();
	LONG lCompen_Y = GetDiePROffsetY();

	m_lOrgDieCompen_X = 0;
	m_lOrgDieCompen_Y = 0;
	m_lTime_CP = 0;

	// If no need to do compensation, simply return
	if ((!m_bPerformCompensation) || (bLFNoComp) || IsBurnIn())		//andrewng //2020-0617
	{
		if (IsBurnIn())
			szMsg.Format("WT - OpCompensation(BURNIN), m_bPerformCompensation=%d, bLFNoComp=%d", m_bPerformCompensation, bLFNoComp);
		else
			szMsg.Format("WT - OpCompensation(), m_bPerformCompensation=%d, bLFNoComp=%d", m_bPerformCompensation, bLFNoComp);
		DisplaySequence(szMsg);
		m_lOrgDieCompen_X = lCompen_X;
		m_lOrgDieCompen_Y = lCompen_Y;

		//andrewng	//2020-4-15
		if (m_bUseLF2ndAlignment)
		{
			X_Sync();
			Y_Sync();
			LONG lWTSettlingDelay = (*m_psmfSRam)["WaferTable"]["SettlingDelay"];
			Sleep(lWTSettlingDelay);

			Sleep(30);	//andrewng //2020-0609

			//IPC_CServiceMessage stMsg;
			SRCH_TYPE stSrchInfo;
			REF_TYPE stInfo;

			stSrchInfo.bShowPRStatus = FALSE;
			stSrchInfo.bNormalDie = TRUE;
			stSrchInfo.lRefDieNo = 1;
			stSrchInfo.bDisableBackupAlign = TRUE;

			/*stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

			INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while (1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) {
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
					break;
				}
				else {
					Sleep(10);
				}
			}*/

			CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
			if (pWaferPr != NULL)
			{
				pWaferPr->SubSearchCurrentDie(stSrchInfo, stInfo);
			}

			if (stInfo.bStatus == TRUE) 
			{
				szMsg.Format("WT - LF 2nd Alignment Result (%d) - Offset (%ld, %ld)",  stInfo.bStatus, stInfo.lX, stInfo.lY);
				DisplaySequence(szMsg);
				
				szMsg.Format("LF 2nd Alignment Result,Enc Offset,%ld,%ld", stInfo.lX, stInfo.lY);
				CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szMsg);

				//Andrew: Then, either #1) logging, #2) do compensation here, or #3) log PR image, etc.
				
				if (m_bUseLF2ndAlignNoMotion)	//andrewng //2020-05-19
				{
					if (stInfo.lX > 20 || stInfo.lY > 20)
					{
						pWaferPr->WaferPrLogFailureCaseImage();
					}
				}
				else
				{
					//Solution #2.
					GetEncoderValue();
					X_Profile(NORMAL_PROF);
					Y_Profile(NORMAL_PROF);
					XY_MoveTo(m_lEnc_X + stInfo.lX, m_lEnc_Y + stInfo.lY, SFM_WAIT);
					Sleep(lWTSettlingDelay);
	
					REF_TYPE stInfo2;
					pWaferPr->SubSearchCurrentDie(stSrchInfo, stInfo2);
					if (stInfo2.bStatus == TRUE) 
					{
						szMsg.Format("WT - LF 3rd Alignment Result (%d) - Offset (%ld, %ld)",  stInfo2.bStatus, stInfo2.lX, stInfo2.lY);
						DisplaySequence(szMsg);
				
						szMsg.Format("LF 3rd Alignment Result,Enc Offset,%ld,%ld", stInfo2.lX, stInfo2.lY);
						CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szMsg);

						if (stInfo2.lX > 20 || stInfo2.lY > 20)
						{
							pWaferPr->WaferPrLogFailureCaseImage();
						}
					}
				}
			}

			BOOL bPickAction = GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK;
			if (bPickAction && IsThisDieDirectPick())
			{
				SetWTReady(TRUE, "LF 2nd Align");	//Allow BH Z1/2 to move down 
			}
		}

		return gnOK;
	}

	szMsg.Format("Compensation Offset(ENC),%d,%d,DoCompensation,%d", lCompen_X, lCompen_Y, m_bPerformCompensation);
	CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szMsg);

	szMsg.Format("WT - Compensation Distance (%d,%d), LFNoComp(%d)", lCompen_X, lCompen_Y, bLFNoComp);
	//CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	DisplaySequence(szMsg);

	LONG lRefDieCheck = (LONG)(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];
	if ((m_nWTAtColletPos==0) && lRefDieCheck != 1 && IsDiePitchCheck(GetCurrentRow(), GetCurrentCol()))	// Perform the die pitch check on the move COMPENSATION result
	{
		BOOL bXCheck = CheckPitchX(lCompen_X);
		BOOL bYCheck = CheckPitchY(lCompen_Y);
		if (m_bAutoSampleAfterPE && (bXCheck == FALSE || bYCheck == FALSE) )
		{
			// at least specified die counter picked after scan.
			if( GetNewPickCount()>=(ULONG)(m_lRescanPECounter + m_lRescanPELimit) )
			{
				if (bXCheck == FALSE )
				{
					bXCheck = CheckAlignPitchX(lCompen_X, 49);	//	100-m_lPitchX_Tolerance);
				}
				if( bYCheck == FALSE)
				{
					bYCheck = CheckAlignPitchY(lCompen_Y, 49);	//	100-m_lPitchY_Tolerance);
				}
				if( bXCheck && bYCheck && m_bToDoSampleOncePE==FALSE )
				{
					m_bToDoSampleOncePE = TRUE;
					ChangeNextSampleLimit(3); DisplaySequence("WT - Add Next Sample Limit 3B");
				}
			}
		}

		if (bXCheck == FALSE || bYCheck == FALSE)
		{
			if( IsVerifyMapWaferEnable()==FALSE )
			{
				szMsg = "Check map wafer match disable on HMI; PCE do compensation: no match check";
				CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
				SetErrorMessage("Exceed Pitch Tolerance in compensation");
				//if (DEB_IsUseable() && IsScanPosnOK() && IsMotionCE() == FALSE)
				//{
				//	LONG lCurrPhX = m_lGetAtcWftPosX;
				//	LONG lCurrPhY = m_lGetAtcWftPosY;
				//	DEB_UpdateDiePos(m_ulGetMveMapRow, m_ulGetMveMapCol, lCurrPhX, lCurrPhY, MS_Align);
				//}

				return Err_WTableDiePitchFailed;
			}

			CMSLogFileUtility::Instance()->WT_GetIdxLog("PCE Vrfy in compensation");

			LONG lCurrPhX = m_lGetMveWftPosX + lCompen_X;
			LONG lCurrPhY = m_lGetMveWftPosY + lCompen_Y;
			XY_SafeMoveTo(lCurrPhX, lCurrPhY);
			(*m_psmfSRam)["MS896A"]["WaferTableX"] = lCurrPhX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = lCurrPhY;
			(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulGetMveMapRow;
			(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulGetMveMapCol;
			GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_PCV);	// pitch error

			//if (DEB_IsUseable() && IsScanPosnOK() && IsMotionCE() == FALSE)
			//{
			//	DEB_UpdateDiePos(m_ulGetMveMapRow, m_ulGetMveMapCol, lCurrPhX, lCurrPhY, MS_Align);
			//}

			szMsg.Format("PCE do compensation: (%d,%d)", lCompen_X, lCompen_Y);
			CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

			m_bGetAvgFOVOffset = FALSE;
			Sleep(100);
			WftMoveBondArmToSafe(TRUE);			// compensation pitch error, verify move
			if (CheckMapAndFindWaferMatch(m_ulGetMveMapRow, m_ulGetMveMapCol, lCurrPhX, lCurrPhY, !bXCheck, !bYCheck) == FALSE)
			{
				ULONG ulUpRow = m_ulGetMveMapRow;
				ULONG ulUpCol = m_ulGetMveMapCol;
				m_WaferMapWrapper.SetDieState(ulUpRow, ulUpCol, WT_MAP_DIESTATE_SKIP_PREDICTED);

				m_bMapWaferVerifyOK = FALSE;
				szMsg = "PCE do compensation: find and match check fail to skip";
				CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);

				if( IsThisDieDirectPick() || m_lMC[WTMC_MWMV_DoLimit]==0 )
				{
					szMsg.Format("PCE do compensation: find and match check fail2");
					CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
					SetErrorMessage("Exceed Pitch Tolerance in compensation2 " + szMsg);
					return Err_WTableDiePitchFailed;
				}
				else
				{
					if( m_ulVerifyMapWaferCounter >= (UINT)m_lMC[WTMC_MWMV_DoLimit] )
					{
						szMsg.Format("PCE do compensation: find and match check fail3");
						CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
						SetErrorMessage("Exceed Pitch Tolerance in compensation3 " + szMsg);
						return Err_WTableDiePitchFailed;
					}
				}	//	need to skip and don't let bond arm to pick, 
			}
			else
			{
				if (IsAdvRegionOffset() && IsScanPosnOK() && IsMotionCE() == FALSE)
				{
					LONG lNewOffsetX = lCurrPhX - m_lGetScnWftPosX;
					LONG lNewOffsetY = lCurrPhY - m_lGetScnWftPosY;
					if( m_bGetAvgFOVOffset )
					{
						lNewOffsetX = m_lAvgFovOffsetX;
						lNewOffsetY = m_lAvgFovOffsetY;
					}

					if( UpdateRegionDieOffset(m_ulGetMveMapRow, m_ulGetMveMapCol, lNewOffsetX, lNewOffsetY, TRUE)==FALSE )
					{
						CString szMsg = "Op Compensation update adv region offset error";
						SetErrorMessage(szMsg);
						CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
						SetAlignmentStatus(FALSE);
						m_lWftAdvSamplingResult = -1;
						return WT_PITCH_ERR_ADV_UPDATE;	//	compensation
					}
				}
			}

			XY_SafeMoveTo(lCurrPhX, lCurrPhY);
			Sleep(10);
			GetEncoderValue();

			(*m_psmfSRam)["MS896A"]["WaferTableX"] = lCurrPhX;
			(*m_psmfSRam)["MS896A"]["WaferTableY"] = lCurrPhY;

			m_bGetAvgFOVOffset = FALSE;
			if (m_bStop == FALSE)
			{
				WftMoveBondArmToSafe(FALSE);	// compensation pitch error, verify move
			}
			lCompen_X = 0;
			lCompen_Y = 0;
		}
	}

	if (lRefDieCheck == 1)
	{
		LONG	lDiePitchX_X = 0, lDiePitchX_Y = 0, lDiePitchY_X = 0, lDiePitchY_Y = 0;
		lDiePitchX_X	= GetDiePitchX_X(); 
		lDiePitchX_Y	= GetDiePitchX_Y();
		lDiePitchY_X	= GetDiePitchY_X();
		lDiePitchY_Y	= GetDiePitchY_Y();

		if (labs(lCompen_X) > labs(lDiePitchX_X) * 0.5 || labs(lCompen_Y) > labs(lDiePitchY_Y) * 0.5)
		{
			if (IsBlkFunc2Enable())	//Block2
			{
				DisplaySequence("WT - BLK2 refer die offset over half pitch, do realignment");
				CMSLogFileUtility::Instance()->WT_GetDieLog("WT: BLK2 refer die offset over half pitch, do realignment");
				ULONG ulRow = 0, ulCol = 0;
				UCHAR ucBin = 0;
				BOOL bEOF = FALSE;
				m_WaferMapWrapper.PeekNextDie(1, WAF_CDieSelectionAlgorithm::PICK, ulRow, ulCol, ucBin, bEOF);	//	get row/col
				m_pBlkFunc2->Blk2RealignLocalRefDie(GetCurrentRow(), GetCurrentCol(), ulRow, ulCol);
			}
		}
	}

	LONG lX = GetCurrentEncX() + lCompen_X;
	LONG lY = GetCurrentEncY() + lCompen_Y;

	m_bRotate = IsDiePRRotate();

	if (CMS896AApp::m_lMotionPlatform == SYSTEM_NUMOTION)	//v4.05	//Klocwork
	{
		if (IsLayerPicking())	//v4.16T1
		{
		}
		else
		{
			m_bRotate	= FALSE;
		}
	}

	//v3.61
	//Already rotated by COnfirmSearch in WPR task; no need to rotate again in WT task
	BOOL bConfirmSearch = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CFS"]["ConfirmSearch"];
	BOOL bBinTableThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"];

	LONG	lTime_X = 0, lTime_Y = 0, lTime_T = 0;
	LONG	lTheta = 0;		
	DOUBLE	dTheta = 0.0;
	if (m_bRotate == TRUE)
	{
		dTheta = GetDiePROffsetT();
		RotateWaferTheta(&lX, &lY, &lTheta, dTheta);
		
		//v3.44		//ShowaDenko
		//Use Multi-COR Offset if enabled
		if (m_bUseMultiCorOffset)
		{
			DOUBLE dDX, dDY;
			LONG lDX, lDY;

			if (lTheta > 0)		//Clockwise
			{
				dDX = m_dCorOffsetA1 * (GetCurrentEncY() - m_lWaferCalibY) + m_dCorOffsetB1;
				dDY = m_dCorOffsetA2 * (GetCurrentEncX() - m_lWaferCalibX) + m_dCorOffsetB2;
				lDX = (LONG) dDX;
				lDY = (LONG) dDY;
			}
			else				//anti-clockwise
			{
				dDX = m_dACorOffsetA1 * (GetCurrentEncY() - m_lWaferCalibY) + m_dACorOffsetB1;
				dDY = m_dACorOffsetA2 * (GetCurrentEncX() - m_lWaferCalibX) + m_dACorOffsetB2;
				lDX = (LONG) dDX;
				lDY = (LONG) dDY;
			}

			lX = lX + lDX;
			lY = lY + lDY;
		}

		//update XY compensation again
		lCompen_X = lX - GetCurrentEncX();
		lCompen_Y = lY - GetCurrentEncY();

		lTime_T = T_ProfileTime(NORMAL_PROF, lTheta, lTheta);

		// Consider the settling for theta stable (1 degree = 15ms by default)
		if (abs(lTheta) > 0)
		{
			m_lReadyDelay = m_lMinDelay + (LONG)(fabs((DOUBLE)(lTheta) * m_dThetaRes) * m_lTDelay);
		}
		else
		{
			m_lReadyDelay = m_lMinDelay;
		}
		szMsg.Format("Rotate in compensation %d,%d,%d", lX, lY, lTheta);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	}
	else if (bBinTableThetaCorrection)
	{
		CString szLog;
		dTheta = GetDiePROffsetT();
		if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
		{
			//No LF
			CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
			pBinTable->BinTableThetaCompensation(dTheta);
			szLog.Format("WT - BT Theta OpCompensation PICK Degree,%f", dTheta);
			DisplaySequence(szLog);
		}
		else
		{
			(*m_psmfSRam)["BinTable"]["ThetaCorrection"] = dTheta;
			szLog.Format("WT OpCompensation not PICK Degree,%f", dTheta);
			CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);
		}
	}
	else
	{
		lTime_T = 0;
		lTheta = 0;
		m_lReadyDelay = m_lMinDelay;
	}

	BOOL bXYNoMove = FALSE;
	LONG lMinTravelCount = (LONG)(0.1 / m_dXYRes);
	if (IsBurnIn() && (abs(lCompen_X) == 0) && (abs(lCompen_Y) == 0))		//v4.01		//MS100 100ms BURNIN cycle
	{
		// Use normal profile if travel distance is > 0.1mm
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
		bXYNoMove	= TRUE;
		lTime_X = 0;
		lTime_Y = 0;
	}
	else //if ((abs(lCompen_X) > lMinTravelCount) || (abs(lCompen_Y) > lMinTravelCount))
	{
		// Use normal profile if travel distance is > 0.1mm
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);

		lTime_X = X_ProfileTime(NORMAL_PROF, lCompen_X, lCompen_X);
		lTime_Y = Y_ProfileTime(NORMAL_PROF, lCompen_Y, lCompen_Y);
	}
	//else
	//{
	//	// Use Fast profile if travel distance is <= 0.1mm
	//	X_Profile(FAST_PROF);
	//	Y_Profile(FAST_PROF);

	//	lTime_X = X_ProfileTime(FAST_PROF, lCompen_X, lCompen_X);
	//	lTime_Y = Y_ProfileTime(FAST_PROF, lCompen_Y, lCompen_Y);
	//}


	// Find max time
	m_lTime_CP = lTime_X;
	if (m_lTime_CP < lTime_Y)
	{
		m_lTime_CP = lTime_Y;
	}
	if (m_lTime_CP < lTime_T)
	{
		m_lTime_CP = lTime_T;
	}

	if (IsBurnIn() && bXYNoMove)		//v4.01	//MS100 100ms BURNIN cycle
	{
	}
	else
	{
		CMSLogFileUtility::Instance()->BPR_Arm1Log("{OpCalculate EjtOffsetXY at COMP}");	
		OpCalculateEjtOffsetXY( lX, lY, lTheta, FALSE, FALSE, TRUE,
								m_stLFDie.bRotate, GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK);	//v4.42T15

		CString szTemp;
		szTemp.Format("{END COMP: %d %d}",m_stLFDie.bRotate, GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK);
		CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		X_MoveTo(lX, SFM_NOWAIT);
		Y_MoveTo(lY, SFM_NOWAIT);

		//v4.59A39
		if (!OpMoveEjectorTableXY(FALSE, SFM_NOWAIT))
		{
			SetErrorMessage("ERROR: WT OpMoveEjectorTableXY fail in AUTOBOND mode");
			SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER);
			return gnNOTOK;
		}
	}

	if (m_bRotate == TRUE)
	{
		T_Move(lTheta, SFM_NOWAIT);
		(*m_psmfSRam)["WaferTable"]["Rotate"] = TRUE;		//v3.62
	}

	szMsg.Format("Com Move %d,%d,T%d,(%d,%d)", lX, lY, lTheta, lCompen_X, lCompen_Y);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);

	return gnOK;
}

////////////////////////////////////////////
//	Wait Table Move Complete
////////////////////////////////////////////

INT	CWaferTable::OpMoveComplete()
{
	if (m_lTime_CP > 0)
	{
		X_Sync();
		Y_Sync();
		if (m_bRotate == TRUE)
		{
			T1_Sync();
		}
		//Sleep(100);//For WT Settling Time
	}

	if (IsWT2InUse())
	{
		if (!m_bSel_X2 && !m_bSel_Y2)
		{
#ifdef NU_MOTION
			Sleep(30);
#else
			Sleep(50);
#endif
		}
	}
	else
	{
		if (!m_bSel_X && !m_bSel_Y)		//v3.86
		{
#ifdef NU_MOTION
			Sleep(30);
#else
			Sleep(50);
#endif
		}
	}

	return gnOK;
}

////////////////////////////////////////////
//	Roll Back 
////////////////////////////////////////////
INT CWaferTable::OpBlkRollBack()
{
	CString szLogMsg;

	szLogMsg.Format("Last (%d,%d) grade %d; Curr (%d,%d), grade %d",
					GetLastDieRow(), GetLastDieCol(), m_stLastDie.ucGrade,
					GetCurrentRow(), GetCurrentCol(), GetCurrDieGrade());
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);

	if (m_stLastDie.ulStatus == WT_MAP_DS_PICK || 
		GetLastDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
	{
		ULONG ulRow, ulMaxRow = 0; 
		ULONG ulCol, ulMaxCol = 0;
		LONG lMarkDieCount = 0;
		ULONG ulSta = 0;

		CDWordArray	dwaRow;
		CDWordArray dwaCol;
		CDWordArray dwaSta;

		dwaRow.SetSize(0);
		dwaCol.SetSize(0);
		dwaSta.SetSize(0);

		if (m_WaferMapWrapper.GetMapDimension(ulMaxRow, ulMaxCol) != TRUE)
		{
			return gnOK;
		}

		if ((ulMaxRow > MAP_MAX_ROW) && (ulMaxCol > MAP_MAX_COL ))
		{
			return gnOK;
		}
		for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
		{
			for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
			{
				if (m_WaferMapWrapper.GetDieState(ulRow, ulCol) == WT_MAP_DS_PR_EMPTY)
				{
					dwaRow.SetSize(lMarkDieCount + 1);
					dwaCol.SetSize(lMarkDieCount + 1);
					dwaSta.SetSize(lMarkDieCount + 1);

					dwaRow.SetAt(lMarkDieCount, ulRow);
					dwaCol.SetAt(lMarkDieCount, ulCol);
					dwaSta.SetAt(lMarkDieCount, m_WaferMapWrapper.GetDieState(ulRow, ulCol));

					lMarkDieCount++;
				}
			}
		}

		CMSLogFileUtility::Instance()->WT_GetDieLog("stop Map in roll back");
		m_WaferMapWrapper.StopMap();
		m_WaferMapWrapper.ReplaceDie(GetLastDieRow(), GetLastDieCol(), m_stLastDie.ucGrade);
		m_WaferMapWrapper.SetStartPosition(GetLastDieRow(), GetLastDieCol());
		m_WaferMapWrapper.SetCurrentPosition(GetLastDieRow(), GetLastDieCol());
		m_WaferMapWrapper.RestartMap();
		szLogMsg.Format("Restart map After roll back, replace die (%d,%d) grade %d and restart map", 
						GetLastDieRow(), GetLastDieCol(), m_stLastDie.ucGrade);
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
		m_lLastPickRow				= GetLastDieRow();
		m_lLastPickCol				= GetLastDieCol();
		m_bLastPickSet				= TRUE;

		m_WaferMapWrapper.ResetGrade();		//	maybe also affect the OCR list

		m_WaferMapWrapper.EnableSequenceCheck(FALSE);	// for block pick align, it is must

		m_pBlkFunc2->VirtualAlignBlockWafer();

		if ((dwaRow.GetSize() > 0) && (dwaCol.GetSize() > 0) && (dwaSta.GetSize() > 0))
		{
			szLogMsg.Format("Reset back missing die state grade");
			CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
			for (lMarkDieCount = 0; lMarkDieCount < dwaRow.GetSize(); lMarkDieCount++)
			{
				ulRow = dwaRow.GetAt(lMarkDieCount);
				ulCol = dwaCol.GetAt(lMarkDieCount);
				ulSta = dwaSta.GetAt(lMarkDieCount);
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, ulSta);
			}

			dwaRow.RemoveAll();
			dwaCol.RemoveAll();
			dwaSta.RemoveAll();
		}

		Sleep(500);
		m_WaferMapWrapper.StartMap();	// start again to let sotp operation to stop
		CMSLogFileUtility::Instance()->WT_GetDieLog("start map for blk roll back and stop");
	}

	return gnOK;
}


INT CWaferTable::OpRollBack()
{
	unsigned long	ulX = 0, ulY = 0;
	unsigned char	ucGrade = 0;
	BOOL			bEndOfWafer = FALSE;
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;

	// Roll Back the wafer map
	BOOL bReturn = m_WaferMapWrapper.RollBack(ulY, ulX, ucGrade, eAction, bEndOfWafer);

	//GetDie Log
	CString szLogText;
	szLogText.Format("RBO %d,%d, grade %d, action %d, end %d, return %d", ulY, ulX, ucGrade, eAction, bEndOfWafer, bReturn);
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
	
	if (bReturn == TRUE)
	{
		bReturn = m_WaferMapWrapper.RollBackUpdate(ulY, ulX, ucGrade, eAction, bEndOfWafer);

		szLogText.Format("RBU %d,%d, grade %d, act %d, bgn %d, return %d", ulY, ulX, ucGrade, eAction, bEndOfWafer, bReturn);
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
	}

	return gnOK;
}



////////////////////////////////////////////
//	Get Look-Forward Result
////////////////////////////////////////////

INT CWaferTable::OpGetLFResult()
{
	INT	nResult		= gnOK;

	// Save the Look-Forward data for next die
	BOOL bUseLookForward = IsUseLF();

	if (bUseLookForward == TRUE)
	{
		m_stLastLFDie = m_stLFDie;		//v3.01T1

		//2018.1.2
		m_stLFDie.lDirection	= (*m_psmfSRam)["WaferMap"]["NextDieDirection"];

		m_stLFDie.bRotate		= (BOOL)((LONG)(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"]);
		m_stLFDie.bFullDie		= (BOOL)((LONG)(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]);
		m_stLFDie.bGoodDie		= (BOOL)((LONG)(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]);	//v2.96T3
		m_stLFDie.lX			= (*m_psmfSRam)["WaferPr"]["LookForward"]["X"];
		m_stLFDie.lY			= (*m_psmfSRam)["WaferPr"]["LookForward"]["Y"];
		m_stLFDie.dTheta		= (*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"];
		m_stLFDie.lWM_X			= (*m_psmfSRam)["WaferMap"]["LookForward_X"];
		m_stLFDie.lWM_Y			= (*m_psmfSRam)["WaferMap"]["LookForward_Y"];

		BOOL bBinTableThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"];
		if (bBinTableThetaCorrection)
		{
			CString szLog;
			(*m_psmfSRam)["BinTable"]["ThetaCorrection"] = m_stLFDie.dTheta;
			szLog.Format("WT - OpGetLFResult() LF Set Theta Correction,%f,%d", m_stLFDie.dTheta, m_stLFDie.bGoodDie);
			
			CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);
			DisplaySequence(szLog);
		}
		
		if (m_bUseMS60NewLFCycle)		//v4.47T3
		{
			if ((m_stLFDie.lDirection != -1) && (m_stLFDie.bGoodDie))
			{
				m_bMS60NewLFInLastCycle = TRUE;

				//Save/backup WT curr encoder for NEXT WT MoveTable() LF-move usage
				m_stMS60LastLFDie.lX_Enc	= m_stMS60NewLFDie.lX_Enc;
				m_stMS60LastLFDie.lY_Enc	= m_stMS60NewLFDie.lY_Enc;
				m_stMS60LastLFDie.lT_Enc	= m_stMS60NewLFDie.lT_Enc;

				CString szTemp;
				szTemp.Format("LF Result: map(%ld, %ld): LF-Offset (%ld, %ld), Enc (%ld, %ld)", 
								m_stLFDie.lWM_Y, m_stLFDie.lWM_X, 
								m_stLFDie.lX, m_stLFDie.lY, m_stMS60LastLFDie.lX_Enc, m_stMS60LastLFDie.lY_Enc);
				CMSLogFileUtility::Instance()->MS60_Log(szTemp);

				szTemp.Format("WT - Update m_stMS60LastLFDie = m_stMS60NewLFDie (%d,%d,%d) in OpGetLFResult()", 
					m_stMS60LastLFDie.lX_Enc, m_stMS60LastLFDie.lY_Enc, m_stMS60LastLFDie.lT_Enc);
				DisplaySequence(szTemp);
			}
			else
			{
				m_bMS60NewLFInLastCycle = FALSE;
				m_stMS60LastLFDie.lX_Enc	= 0;
				m_stMS60LastLFDie.lY_Enc	= 0;
				m_stMS60LastLFDie.lT_Enc	= 0;
				m_nMS60CycleCount			= 0;//LFDEGUG
				DisplaySequence("LF Result: FAIL - m_nMS60CycleCount reset to 0");
			}
		}
		else if (m_bWaferPrMS60LFSequence && m_bWaferPrUseMxNLFWnd &&
				 (m_stLFDie.lDirection != -1) && !m_stLFDie.bGoodDie)
		{
			m_nMS60CycleCount			= 0;//LFDEGUG
		}

		//=============================2019.03.04========================================
		//   calculate the lookahead offset based on current die result offset
		//   so do not need to calculate the lookahead offset again after do compensate the current die
		//===============================================================================

/*
		//=============================2019.02.18========================================
		//   compensate the lookahead offset for only the second die in new lookforward option
		//===============================================================================
		BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
		if (!m_bUseMS60NewLFCycle && !bLFNoComp)
		{
			LONG lCompen_X = GetDiePROffsetX();
			LONG lCompen_Y = GetDiePROffsetY();

			m_stLFDie.lX -= lCompen_X;
			m_stLFDie.lY -= lCompen_Y;
			CString szMsg;
			szMsg.Format("m_nMS60CycleCount = %d, bLFNoComp  = 0, comp LF-Offset = %d, %d, Result LF-Offset = %d, %d", m_nMS60CycleCount, lCompen_X, lCompen_Y, m_stLFDie.lX, m_stLFDie.lY);
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);	
		}
		//===================================================================================
*/
		//v4.54A5
		if (CMS896AStn::m_bMS60NGPick)
		{
			BOOL bIsEmptyDie = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["LookBackward"]["IsEmptyDie"];
			CString szMsg;
			szMsg.Format("WT: OpGetLFResult LastDie IsEmpty = %d", bIsEmptyDie);
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}
	}

	m_stLFDie.lDirection	= (*m_psmfSRam)["WaferMap"]["NextDieDirection"];
	if (m_stLFDie.lDirection == -1)
	{
		InitLFInfo(m_stLFDie);
	}

	//v4.43T8
	if (!bUseLookForward)
		(*m_psmfSRam)["WaferTable"]["LookForward"]["NeedRotate"]	= FALSE;
	else
		(*m_psmfSRam)["WaferTable"]["LookForward"]["NeedRotate"]	= m_stLFDie.bRotate;

	return nResult;
}

////////////////////////////////////////////
//	Move back to Global Angle 
////////////////////////////////////////////

INT CWaferTable::OpRotateBack()
{
	INT nResult = gnOK;
	LONG lTheta = 0;
	DOUBLE dDiffDegree = 0;

	//Calculate last die pos @global angle
	if (IsEnablePNP() == FALSE)
	{
		dDiffDegree = -(DOUBLE)m_lThetaMotorDirection * ((DOUBLE)(GetGlobalT() - m_stLastDie.lT_Enc) * m_dThetaRes);
		RotateWaferTheta(&m_stLastDie.lX_Enc, &m_stLastDie.lY_Enc, &lTheta, dDiffDegree);
		m_stLastDie.lT_Enc = GetGlobalT();
	}

	if (IsWithinWaferLimit(GetLastDieEncX(),GetLastDieEncY()))
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);

		// Move back
		X_MoveTo(GetLastDieEncX(), SFM_NOWAIT);
		Y_MoveTo(GetLastDieEncY(), SFM_NOWAIT);
		T_MoveTo(GetGlobalT(), SFM_WAIT);
		X_Sync();
		Y_Sync();

		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}

	return nResult;
}


////////////////////////////////////////////
//	Wafer End move to unload pos
////////////////////////////////////////////
INT CWaferTable::OpDisplayWaferEndAlarm()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bShowAlarm = TRUE;
	BOOL bSanAn = FALSE;

	if (IsEnableWL() == TRUE)
	{
		if ( IsWLAutoMode() || IsWLSemiAutoMode())
		{
			bShowAlarm = FALSE;
		}

		if (pApp->GetCustomerName() == CTM_SANAN)
		{
			bShowAlarm = TRUE;
			bSanAn = TRUE;
		}
	}

	if ((IsBurnIn() == FALSE) && (bShowAlarm == TRUE))
	{
		SaveScanTimeEvent("    WFT: SHOW ALARM AS WAFER END");

		if (bSanAn)
		{
			OpDisplayWaferEndInfoAlarm();
		}
		else
		{
			SetAlert_Red_Yellow(IDS_WT_END); 
		//	SetAlignmentStatus(FALSE);	
			m_lPresortWftStatus += 8;//4.53D45
		}	

		if (m_bEjNeedReplacement == TRUE && pApp->GetCustomerName()!= "ChangeLight(XM)")
		{
			m_bEjNeedReplacement = FALSE;
			SetAlert_Red_Yellow(IDS_BH_REPLACE_EJECTOR);
			SetErrorMessage("WL: autobond cycle aborted (MANUAL) due to EJ pin lifetime & replacement alarm");
		}
	}

	return 0;
}	//	show wafer end alarm message only

INT CWaferTable::OpWaferEndMoveToUnload()
{
	INT nResult = gnOK;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	//Re-verify All Ref Dice found (for Cree)
	if (IsBlkFunc2Enable() ||	IsPrescanBlkPick())
	{
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End BLK2 to reverify refer die");
		if ((m_bUseReVerifyRefDie) && IsBurnIn() == FALSE && pUtl->GetPrescanRegionMode() == FALSE &&
				(m_bWaferEndUncheckRefer == FALSE))
		{
			if (m_pBlkFunc2->Blk2ReVerifyRefDiePos() == FALSE)
			{
				(*m_psmfSRam)["WaferTable"]["VerifyRefDieFail"] = TRUE;
			}
			else
			{
				(*m_psmfSRam)["WaferTable"]["VerifyRefDieFail"] = FALSE;
			}
		}
	}
	else
	{
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End non block pick");
		if ((m_bUseReVerifyRefDie) && IsBurnIn() == FALSE && m_bNonBlkPkEndVerifyRefDie &&
				(m_bWaferEndUncheckRefer == FALSE))
		{
			CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Reverify all refer die");
			if (ReVerifyAllReferDie() == FALSE)
			{
				(*m_psmfSRam)["WaferTable"]["VerifyRefDieFail"] = TRUE;
			}
			else
			{
				(*m_psmfSRam)["WaferTable"]["VerifyRefDieFail"] = FALSE;
			}
		}
	}

	//Rotate 180 degree
	if (IsAllSorted() && IsMS90HalfSortMode())
	{
		XY_SafeMoveTo(0, 0);
		LONG  lHomeWftT = m_lThetaMotorDirection * (LONG)(180.0/m_dThetaRes);
		T_Move(lHomeWftT, SFM_WAIT);
		Sleep(100);
		MS90Set1stDone( FALSE );
		m_bSortGoingTo2ndPart = FALSE;
		USHORT usRotate =m_ucMapRotation*90;
		if (pUtl->GetPrescanDummyMap())
		{
			usRotate = 0;
		}
//===============================================2018.1.6==================================
		m_WaferMapWrapper.Rotate(usRotate);
		//	MS90HalfSortMapAction(TRUE);	// restore back to first part.
	}

	if (m_bIsUnload )
	{
		INT nEjStatus = OpCheckEjStatusBeforeWaferEndUnload();		//v4.52A8

		if (nEjStatus == gnOK)
		{
			if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.20	//MS100 9Inch	
			{
				X_Profile(LOW_PROF1);
				Y_Profile(LOW_PROF1);

				if (XY_MoveTo(0, 0, SFM_WAIT) == gnOK)	
				{
					SetEJTSlideUpDn(FALSE);
					Sleep(500);
					XY_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
				}
				
				X_Profile(NORMAL_PROF);
				Y_Profile(NORMAL_PROF);
			}
			else
			{
#ifdef NU_MOTION
				X_Profile(LOW_PROF);
				Y_Profile(LOW_PROF);
#else
				X_Profile(NORMAL_PROF);
				Y_Profile(NORMAL_PROF);
#endif
				// Move to unload
				if (IsEnableWL())
				{
					XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_WAIT);
				}
				else
				{
					XY1_MoveTo(m_lWaferUnloadX, m_lWaferUnloadY, SFM_WAIT);
				}

#ifdef NU_MOTION
				X_Profile(NORMAL_PROF);
				Y_Profile(NORMAL_PROF);
#endif
			}
		}
		m_bIsUnload = FALSE;
	}

	return nResult;
}

INT CWaferTable::OpCheckEjStatusBeforeWaferEndUnload()	//v4.52A8
{
	INT nResult = gnOK;

	LONG lEnc_Ej = 0;
	CString szLog;

#ifdef NU_MOTION

	CMSNmAxisInfo stBHAxis_Ej;
	InitAxisData(stBHAxis_Ej);
	stBHAxis_Ej.m_szName		= BH_AXIS_EJ;
	stBHAxis_Ej.m_szTag			= MS896A_CFG_CH_EJECTOR;
	stBHAxis_Ej.m_ucControlID	= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(stBHAxis_Ej);

	CMS896AStn::MotionSync(BH_AXIS_EJ, 5000, &stBHAxis_Ej);	

	lEnc_Ej = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJ, 1, &stBHAxis_Ej);
	if (lEnc_Ej > 200)
	{
		HmiMessage_Red_Yellow("Ejector Z is not at HOME position in WaferEnd!", "WAFER END");
		szLog.Format("WT ERROR: Ej Enc = %ld not at HOME position in WAFEREND", lEnc_Ej);
		SetErrorMessage(szLog);
		nResult = gnNOTOK;
	}

#endif 

	szLog.Format("WT WAFERNED: Ej Enc = %ld", lEnc_Ej);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return nResult;
}

INT CWaferTable::OpES101NextToAlignPosition()
{
	INT nResult = gnOK;

	if (IsWLAutoMode()==FALSE || IsBurnIn())
	{
		return nResult;
	}

	if (XY_IsPowerOff())
	{
		SaveScanTimeEvent("    WFT: Preload Wafer Table XY not power ON!");
		HmiMessage_Red_Back("Preload Wafer Table XY not power ON!");
		return gnNOTOK;
	}

	if (IsWT2InUse())
	{
		if( IsWL1ExpanderSafeToMove() == FALSE )
		{
			return gnNOTOK;
		}

		if ((m_lWT1PositionStatus != 1 && m_lWT1PositionStatus != 2))
		{
			SaveScanTimeEvent("    WFT: move table 1 to align position");
			LONG	lCurrentX = 0;
			LONG	lCurrentY = 0;
			GetHomeDiePhyPosn(lCurrentX, lCurrentY);
			if( m_bUseContour )
			{
				lCurrentX = m_lWL1CenterX;
				lCurrentY = m_lWL1CenterY;
			}
			X1_Profile(LOW_PROF1);
			Y1_Profile(LOW_PROF1);
			XY1_MoveTo(lCurrentX, lCurrentY, SFM_NOWAIT);
			X1_Profile(NORMAL_PROF);
			Y1_Profile(NORMAL_PROF);
			m_lWT1PositionStatus = 1;
			RotateWFTTUnderCam(FALSE);
		}
	}
	else
	{
		if( IsWL2ExpanderSafeToMove() == FALSE )
		{
			return gnNOTOK;
		}

		if ((m_lWT2PositionStatus != 1 && m_lWT2PositionStatus != 2))
		{
			SaveScanTimeEvent("    WFT: move table 2 to align position");
			LONG	lCurrentX = 0;
			LONG	lCurrentY = 0;
			GetWT2HomeDiePhyPosn(lCurrentX, lCurrentY);
			if( m_bUseContour )
			{
				lCurrentX = m_lWL2CenterX;
				lCurrentY = m_lWL2CenterY;
			}
			X2_Profile(LOW_PROF1);
			Y2_Profile(LOW_PROF1);
			XY2_MoveTo(lCurrentX, lCurrentY, SFM_NOWAIT);
			X2_Profile(NORMAL_PROF);
			Y2_Profile(NORMAL_PROF);
			m_lWT2PositionStatus = 1;
			RotateWFTTUnderCam(TRUE);
		}
	}

	return nResult;
}	//	4.24TX 4

////////////////////////////////////////////
//	Supporting Functions
////////////////////////////////////////////

VOID CWaferTable::SetEjectorVacuum(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
	if (m_bDisableWT)	//v3.61
	{
		return;
	}

	//CMS896AStn::MotionSetOutputBit("oEjectorVacuum", bSet);
	return;
}

////////////////////////////////////////////
//	Check hardware ready 
////////////////////////////////////////////
BOOL CWaferTable::IsMotionHardwareReady()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableWT)	//v3.61
	{
		return TRUE;
	}

	CString szText;
	if (!X_IsPowerOn())
	{
		szText = "\nWaferTable X is not power ON";
		SetStatusMessage("WaferTable X NOT power ON yet");
		SetErrorMessage("WaferTable X NOT power ON yet");
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		return FALSE;
	}

	if (!Y_IsPowerOn())
	{
		szText = "\nWaferTable Y is not power ON";
		SetStatusMessage("WaferTable Y NOT power ON yet");
		SetErrorMessage("WaferTable Y NOT power ON yet");
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		return FALSE;
	}

	if (!T_IsPowerOn())
	{
		szText = "\nWaferTable T is not power ON";
		SetStatusMessage("WaferTable T NOT power ON yet");
		SetErrorMessage("WaferTable T NOT power ON yet");
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		return FALSE;
	}

	if ((CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X) == FALSE) || 
		(CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y) == FALSE) || 
		//(CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T) == FALSE) || 
		!m_bHome_X || !m_bHome_Y || !m_bHome_T)
	{
		StopCycle("WaferTableStn");
		SetMotionCE(TRUE, "WT Not Home(IsMotionHardwareReady)");

		BOOL bWarn = FALSE;
		if ((CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X) == FALSE) || !m_bHome_X)
		{
			szText = "\nWaferX";
			SetStatusMessage("WaferX NOT home yet");
			SetErrorMessage("WaferX NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			bWarn = TRUE;
		}
		if ((CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y) == FALSE) || !m_bHome_Y)
		{
			szText = "\nWaferY";
			SetStatusMessage("WaferY NOT home yet");
			SetErrorMessage("WaferY NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			bWarn = TRUE;
		}
		if ((CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T) == FALSE) || !m_bHome_T)
		{
			szText = "\nWaferT";
			SetStatusMessage("WaferT NOT home yet");
			SetErrorMessage("WaferT NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			bWarn = TRUE;
		}

		if (bWarn == FALSE)
		{
			szText = "\nWaferTable";
			SetStatusMessage("WaferTable NOT home yet");
			SetErrorMessage("WaferTable NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		}

		return FALSE;
	}

	if (IsESDualWT())
	{
		if ((CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2) == FALSE) || 
				(CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2) == FALSE) || 
				(CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2) == FALSE) || 
				!m_bHome_X2 || !m_bHome_Y2 || !m_bHome_T2)
		{
			StopCycle("WaferTableStn");
			SetMotionCE(TRUE, "WT2 Not Home(IsMotionHardwareReady)");

			CString szText;

			BOOL bWarn = FALSE;
			if (!m_bHome_X2)
			{
				szText = "\nWaferX2";
				SetStatusMessage("WaferX2 NOT home yet");
				SetErrorMessage("WaferX2 NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
				bWarn = TRUE;
			}
			if (!m_bHome_Y2)
			{
				szText = "\nWaferY2";
				SetStatusMessage("WaferY2 NOT home yet");
				SetErrorMessage("WaferY2 NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
				bWarn = TRUE;
			}
			if (!m_bHome_T2)
			{
				szText = "\nWaferT2";
				SetStatusMessage("WaferT2 NOT home yet");
				SetErrorMessage("WaferT2 NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
				bWarn = TRUE;
			}

			if (bWarn == FALSE)
			{
				szText = "\nWaferTable";
				SetStatusMessage("WaferTable NOT home yet");
				SetErrorMessage("WaferTable NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			}

			return FALSE;
		}
	}

	return TRUE;
}


INT CWaferTable::OpDisplayWaferEnd()
{
	BOOL bShowAlarm = TRUE;
	BOOL bSanAn = FALSE;

	if (IsEnableWL() == TRUE)
	{
		if ( IsWLAutoMode() || IsWLSemiAutoMode())     //v3.42T1   //AUTO or SEMI-AUTO mode
		{
			bShowAlarm = FALSE;
		}

		//v3.57T2
		//Special Wafer-End alarm dialog displayed for SanAn under fully-Auto mode only
		CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
		if (pAppMod->GetCustomerName() == CTM_SANAN)
		{
			bShowAlarm = TRUE;
			bSanAn = TRUE;
		}
	}

	//4.49 //Unload bin to full when wafer end under resort mode.
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (pApp->GetCustomerName() == CTM_SEMITEK && pUtl->GetPrescanDummyMap())
	{
		(*m_psmfSRam)["WaferTable"]["ResortModeWaferEnd"]		=  TRUE;
		CMSLogFileUtility::Instance()->MS_LogOperation("ResortModeWaferEnd");
		if (pApp->GetCustomerName() == CTM_SEMITEK && pUtl->GetPrescanDummyMap())
		{
			(*m_psmfSRam)["WaferTable"]["ResortModeWaferEnd"]           =  TRUE;
			CMSLogFileUtility::Instance()->MS_LogOperation("ResortModeWaferEnd");

			CString szOutputFileFolder          = (*m_psmfSRam)["MS896A"]["MapFilePath"];
			CMSLogFileUtility::Instance()->MS_LogOperation("From:" + szOutputFileFolder);

			CString szNewOutputFileFolder = (*m_psmfSRam)["MS896A"]["OutputFilePath"];
			CMSLogFileUtility::Instance()->MS_LogOperation("To:" + szOutputFileFolder);

			CString szFrameBarcode = (*m_psmfSRam)["WaferTable"]["WT1InBarcode"];
			CMSLogFileUtility::Instance()->MS_LogOperation("WaferBarcode:" + szFrameBarcode);

			CString szNewBarcode   = (*m_psmfSRam)["MS896A"]["CurrentLoadBarcode"];
			CMSLogFileUtility::Instance()->MS_LogOperation("BinBarcode:" + szNewBarcode);

			CString szNewOuputFilePath;

			if (_access(szOutputFileFolder,0) != -1)
			{
				CFileFind cfFind;

				CString szOutputFilePath = szOutputFileFolder + "\\*.*";

				BOOL bFind = cfFind.FindFile(szOutputFilePath);

				int nFind;

				/*AfxMessageBox(szLabelFile);*/

				while (bFind)
				{
					bFind = cfFind.FindNextFile();
					CString szName    = cfFind.GetFileName();

					CString szPath    = cfFind.GetFilePath();

					CMSLogFileUtility::Instance()->MS_LogOperation("Path:" + szPath);
					nFind = szName.Find(szFrameBarcode);
					if( nFind != -1)
					{
						szName.Replace(szFrameBarcode,szNewBarcode);
						CString szNew = szNewOutputFileFolder + "\\" + szName;
						remove(szNew);
						CopyFile(szPath,szNew,FALSE);
					}
				}
			}
		}
	}

	CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Display func");

	if ((IsBurnIn() == FALSE) && (bShowAlarm == TRUE))
	{
//		SaveScanTimeEvent("    WFT: SHOW ALARM AS WAFER END");
		SetStatusMessage("Current wafer is finished");


		SetErrorMessage("Current wafer is finished");
		if (bSanAn)
		{
//			OpDisplayWaferEndInfoAlarm();		//v3.57T2	//SanAn
			SetAlignmentStatus(FALSE);			//v3.80
		}
		else
		{
//			SetAlert_Red_Yellow(IDS_WT_END); 
		}	

		(*m_psmfSRam)["WaferTable"]["WaferEnd"]				= TRUE;		//v4.44A1	//SEmitek
		(*m_psmfSRam)["WaferTable"]["OsramWaferEnd"]		= TRUE;	
		//v4.39T10	//Silan (WanYiMing)
//		if (m_bEjNeedReplacement == TRUE)
//		{
//			m_bEjNeedReplacement = FALSE;
//			SetAlert_Red_Yellow(IDS_BH_REPLACE_EJECTOR);
//			SetErrorMessage("WL: autobond cycle aborted (MANUAL) due to EJ pin lifetime & replacement alarm");
//		}

		m_bIsUnload = TRUE;
	}

	//v2.91T1
	CMSLogFileUtility::Instance()->WT_GetDieLog("WAFER-END Handling");
	CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Handling");

	if( IsAllSorted() )
		m_bIsWaferEnded = TRUE;		//v4.20

	m_bIsWaferEndedFor3EDL = TRUE;		//v4.45T3

	//Output reference die status file
	OutputRefDieStatusFile();
	SaveScanTimeEvent("    WFT: Op Display Wafer End done");

	return 0;
}


INT CWaferTable::OpDisplayWaferEndInfoAlarm()	//	for SanAn only
{
	CUIntArray unaAvaGradeList;
	UCHAR ucGrade = 0, ucCurrGrade = 0;
	UCHAR ucSecLastGrade = '0', ucLastGrade = 0;
	ULONG ulLeft = 0, ulPicked = 0, ulTotal = 0;
	LONG lTotalDie = 0;
	LONG lSecLastGradePickedCount = 0;
	CString szMsg = "", szLog = "";
	CString szSecLastGradePickedCount = "", szTotalPickedCount = ""; 
	CString szSecLastGrade = "";
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);
	
	for (INT i = 0; i < unaAvaGradeList.GetSize(); i++)
	{
		ucGrade = unaAvaGradeList.GetAt(i);
		m_WaferMapWrapper.GetStatistics(ucGrade, ulLeft, ulPicked, ulTotal);
		lTotalDie = lTotalDie + ulPicked;
		
		if (ucGrade >= ucLastGrade)
		{
			ucLastGrade = ucGrade;
		}
	}

	szTotalPickedCount.Format("Total Picked : %d", lTotalDie);
	// define second last grade as last grade -1
	ucSecLastGrade = ucLastGrade - 1;
	szSecLastGrade.Format("Grade : %d", ucSecLastGrade - m_WaferMapWrapper.GetGradeOffset());

	for (INT i = 0; i < unaAvaGradeList.GetSize(); i++)
	{
		if (unaAvaGradeList.GetAt(i) == ucSecLastGrade)
		{
			m_WaferMapWrapper.GetStatistics(ucSecLastGrade, ulLeft, ulPicked, ulTotal);
			lSecLastGradePickedCount = ulPicked;
		}
	}

	szSecLastGradePickedCount.Format("BT/GT: %d", lSecLastGradePickedCount);
	szMsg = "wafer End: " + m_szMapFileName + "\n" + szSecLastGrade + "\n" + szTotalPickedCount + "\n" + szSecLastGradePickedCount;
	szLog = "wafer End: " + m_szMapFileName + "-" + szSecLastGrade + "-" + szTotalPickedCount + "-" + szSecLastGradePickedCount;
	
	if( pApp ->GetProductLine() != "XA" || IsWLAutoMode()==FALSE )
	{
		SetAlert_Msg_Red_Yellow(IDS_WT_END, szMsg);
	}
	else
	{
		SetAlert_NoMsg(IDS_WT_END, szMsg);
	}
	SetErrorMessage(szLog);

	return 0;
}


INT CWaferTable::OpSkipSEPGradeDie()
{
	if (((LONG)(*m_psmfSRam)["WaferTable"]["Hit SEP Grade"] == 1) && (m_bCheckSepGrade == TRUE) && (m_bJustAlign == FALSE))
	{
		unsigned long ulDiePos_X = 0, ulDiePos_Y = 0;
		unsigned long ulPeekCount = 1;
		unsigned char ucGrade = 0;
		ULONG ulLastDieStatus;
		
		BOOL bEndOfWafer = FALSE;
		WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
		CMSLogFileUtility::Instance()->WT_GetDieLog("skip seperate die");
		//Get next die & update the SEP Grade die, then cycle start will get die after SEP grade
		while (1)
		{
			PeekMapNextDie(ulPeekCount, ulDiePos_Y, ulDiePos_X, ucGrade, eAction, bEndOfWafer,"SkipSEP");

			if (eAction == WAF_CDieSelectionAlgorithm::CHECK)
			{
				if (GetMapNextDie(ulDiePos_Y, ulDiePos_X, ucGrade, eAction, bEndOfWafer) == TRUE)
				{
					ulLastDieStatus = m_WaferMapWrapper.GetDieState(ulDiePos_Y , ulDiePos_X);
	
					SetMapNextDie(ulDiePos_Y, ulDiePos_X, ucGrade, 
											 WAF_CDieSelectionAlgorithm::SEPARATION, ulLastDieStatus);
				}
				break;
			}

			if (bEndOfWafer == TRUE)
			{
				break;
			}

			ulPeekCount++;
		}
	}

	(*m_psmfSRam)["WaferTable"]["Hit SEP Grade"] = 0;
	return 0;
}

INT CWaferTable::OpIsWithinPolygonLimit(unsigned long &ulRow, unsigned long &ulCol, unsigned char &ucGrade, 
										WAF_CDieSelectionAlgorithm::WAF_EDieAction &eAction,
										BOOL &bEndOfWafer) 
{
	if (m_ucWaferLimitType == WT_POLYGON_LIMIT)
	{
		LONG lDiePitchX_X = labs(GetDiePitchX_X()); 
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();
		LONG lDiePitchY_Y = labs(GetDiePitchY_Y());
		
		LONG lX, lY;

		LONG lLastX = GetLastDieEncX();
		LONG lLastY = GetLastDieEncY();
		LONG lDiff_X = (LONG)ulCol - GetLastDieCol();
		LONG lDiff_Y = (LONG)ulRow - GetLastDieRow();

		lX = lLastX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lY = lLastY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

		if (IsInsidePolygon(lX, lY) == TRUE)
		{
			return TRUE;
		}

		BOOL bNextEndOfWafer = FALSE;
		ULONG ulNextRow = 0, ulNextCol = 0;
		UCHAR ucNextGrade = 0;
		DIE_INFO stLastDie = m_stLastDie;

		WAF_CDieSelectionAlgorithm::WAF_EDieAction eNextAction;

		while (1)
		{
			PeekMapNextDie(1, ulNextRow, ulNextCol, ucNextGrade, eNextAction, bNextEndOfWafer,"IsWithinLimit");
			if (bNextEndOfWafer == TRUE)
			{
				bEndOfWafer = TRUE;
				break;
			}

			lLastX = stLastDie.lX_Enc;
			lLastY = stLastDie.lY_Enc;
			lDiff_X = (LONG)ulNextCol - stLastDie.lX;
			lDiff_Y = (LONG)ulNextRow - stLastDie.lY;

			lX = stLastDie.lX_Enc - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = stLastDie.lY_Enc - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

			if (IsInsidePolygon(lX, lY) == TRUE)
			{
				ulRow		= ulNextRow;
				ulCol		= ulNextCol;
				ucGrade		= ucNextGrade;	 
				eAction		= eNextAction;
				bEndOfWafer = bNextEndOfWafer;
				break;		
			}
			else
			{
				//Mark out limit die on wafermap
				m_WaferMapWrapper.MarkDie(ulNextRow, ulNextCol, TRUE);

				SetMapNextDie(stLastDie.lY, stLastDie.lX, stLastDie.ucGrade, stLastDie.eAction, stLastDie.ulStatus);

				m_stLastDie.lY			= ulNextRow;	
				m_stLastDie.lX			= ulNextCol;
				m_stLastDie.lX_Enc		= lX;
				m_stLastDie.lY_Enc		= lY;
				m_stLastDie.ucGrade		= ucNextGrade;
				m_stLastDie.eAction		= WAF_CDieSelectionAlgorithm::SKIP;
				ULONG ulDieState = GetMapDieState(ulNextRow, ulNextCol);
				if (!IsDieUnpickAll(ulDieState))
				{
					ulDieState = WT_MAP_DIESTATE_UNPICK;
				}
				m_stLastDie.ulStatus	= ulDieState;
				stLastDie = m_stLastDie;

				GetMapNextDie(ulNextRow, ulNextCol, ucNextGrade, eNextAction, bNextEndOfWafer);
			}
		}
	}

	return TRUE;
}




//================================================================
// OpCalculateTNoMoveBackOffsetXY()
//   Created-By  : Andrew Ng
//   Date        : 10/3/2007 3:18:55 PM
//   Description : 
//   Remarks     : 
//================================================================
INT CWaferTable::OpCalculateTNoMoveBackOffsetXY(long &lDx, long &lDy, double dTheta, long lDistX, long lDistY)
{
	if (dTheta == 0) 
	{
		lDx = 0;
		lDy = 0;
		return TRUE;
	}

	double dRadian = dTheta / 180.0 * PI;

	double dYinX = 2.00 * lDistX * sin(dRadian / 2) * cos(dRadian / 2);
	double dXinX = 2.00 * lDistX * sin(dRadian / 2) * sin(dRadian / 2);
	double dXinY = 2.00 * lDistY * sin(dRadian / 2) * cos(dRadian / 2);
	double dYinY = 2.00 * lDistY * sin(dRadian / 2) * sin(dRadian / 2);

	lDx = (long)(dXinX + dXinY);
	lDy = (long)(dYinY + dYinX);

	/*
	CString szTemp;
	szTemp.Format("No-Move-Back offset   lDx = %d, lDy = %d", lDx, lDy);
	AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	*/

	return TRUE;
}

INT CWaferTable::OpCheckSortingCheckPoints()
{
	LONG lReply = 0;

	if (m_bEnableSortingCheckPts == FALSE)
	{
		return 1;
	}

	// Wait Operator to confirm if sorting check pts function is enabled
	CString szCheckPt, szText, szTitle, szTemp;
	LONG lUserRow = 0, lUserCol = 0;

	ConvertAsmToOrgUser(GetCurrentRow(), GetCurrentCol(), lUserRow, lUserCol);
	szCheckPt.Format("%d,%d", lUserRow, lUserCol);

	for (INT i = 0; i < m_szaSortingCheckPts.GetSize(); i++)
	{
		if (szCheckPt == m_szaSortingCheckPts.GetAt(i))
		{
			//szText.LoadString(HMB_WT_CONFIRM_SORTING_CHECK_PTS);
			szText.Format("Reach Sorting Check Point (%d, %d)\n Wait For Confirmation\n", lUserRow, lUserCol);
			
			lReply = SetAlert_Msg(IDS_WT_SORTING_CHECK_PTS, szText , "Continue", "Stop");

			return lReply;
		}
	}

	return 1;
}


BOOL CWaferTable::OpUpdateNextMotionTime()			//v3.94T4	//MS100
{
	BOOL bRotate = FALSE;
	BOOL	bPrescanJumpRotate = FALSE;

	LONG lMoveWfX = 0;
	LONG lMoveWfY = 0;
	LONG lMoveWfT = 0;

	LONG lCalcX = 0, lCalcY = 0, lCalcT = 0;

	if (CMS896AApp::m_lCycleSpeedMode < 3)	
	{
		return TRUE;
	}

	BOOL bGetPrescan	= GetPrescanSortPosn(lCalcX, lCalcY, lCalcT, TRUE);		//v4.02T4 Disable DEB access here

	BOOL bDieLFed		= (m_stLFDie.lDirection != -1) && (m_stLFDie.bGoodDie);  //(m_stLFDie.bFullDie);
	BOOL bUseLF			= IsUseLF();
	DOUBLE dTheta		= -(DOUBLE)m_lThetaMotorDirection * ((DOUBLE)(GetGlobalT() - m_stLastDie.lT_Enc) * m_dThetaRes);


	if (m_bJustAlign == TRUE)
	{
		lMoveWfX = m_lStart_X;
		lMoveWfY = m_lStart_Y;
		bGetPrescan = FALSE;
		bRotate = FALSE;
	}
	else if (IsBlkFunc2Enable() && IsPrescanEnable() == FALSE && bGetPrescan == FALSE)	//Block2
	{
		LONG lX = 0, lY = 0, lT = 0;		//Klocwork
		GetBlk2SortPosn(GetCurrentRow(), GetCurrentCol(), GetLastDieRow(), GetLastDieCol(), lX, lY, lT); 

		lMoveWfX = lX;
		lMoveWfY = lY;
		lMoveWfT = lT;
		m_bRotate = IsDiePRRotate();
		bRotate = TRUE;
	}
	else if (IsBlkFunc1Enable() && IsPrescanEnable() == FALSE && bGetPrescan == FALSE)	// BLOCK 1
	{
		DieMapPos MapPos;
		DiePhyPos PhyPos;
		// Get physical position from wafer map
		MapPos.Row = GetCurrentRow();
		MapPos.Col = GetCurrentCol();
		MapPos.Status = 0;		//Klocwork	

		m_pBlkFunc->GetNextDiePhyPos(MapPos, &PhyPos);

		lMoveWfX = PhyPos.x;
		lMoveWfY = PhyPos.y;
		lMoveWfT = PhyPos.t;
		bRotate = FALSE;
		m_bRotate = IsDiePRRotate();
		if (m_bRotate == TRUE)
		{
			bRotate = TRUE;
		}

		//No need to compensate T for ALIGN dies
		if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::ALIGN)
		{
			bRotate = FALSE;
		}
	}
	else
	{
		if ((GetCurrentCol() == m_stLFDie.lWM_X) && (GetCurrentRow() == m_stLFDie.lWM_Y))
		{
			// Check whether the current die location is already found by last Look-Forward
			if (bDieLFed)
			{
				//v2.96T3	//v2.97T3
				//NO need to perform compensation if curr-die is LF-die without CONFIRM-SEARCH enabled
				BOOL bConfirmSearch = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CFS"]["ConfirmSearch"];
				BOOL bNeedRotate	= m_stLFDie.bRotate;

				BOOL bSpRefCheck = (BOOL)(LONG)(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"];		//Tell WPR to align this PICK die with Ref-Die pattern


				lMoveWfX = GetLastDieEncX() + m_stLFDie.lX;
				lMoveWfY = GetLastDieEncY() + m_stLFDie.lY;

				if (bNeedRotate)
				{
					bRotate = TRUE;
					RotateWaferTheta(&lMoveWfX, &lMoveWfY, &lMoveWfT, m_stLFDie.dTheta);

					//v3.44		//SHowaDenko
					//Use Multi-COR Offset if enabled
					if (m_bUseMultiCorOffset)
					{
						DOUBLE dDX, dDY;
						LONG lDX, lDY;

						if (lMoveWfT > 0)		//Clockwise
						{
							dDX = m_dCorOffsetA1 * (GetLastDieEncY() - m_lWaferCalibY) + m_dCorOffsetB1;
							dDY = m_dCorOffsetA2 * (GetLastDieEncX() - m_lWaferCalibX) + m_dCorOffsetB2;
							lDX = (LONG) dDX;
							lDY = (LONG) dDY;
						}
						else				//anti-clockwise
						{
							dDX = m_dACorOffsetA1 * (GetLastDieEncY() - m_lWaferCalibY) + m_dACorOffsetB1;
							dDY = m_dACorOffsetA2 * (GetLastDieEncX() - m_lWaferCalibX) + m_dACorOffsetB2;
							lDX = (LONG) dDX;
							lDY = (LONG) dDY;
						}

						lMoveWfX = lMoveWfX + lDX;
						lMoveWfY = lMoveWfY + lDY;
					}
				}
			}
			else
			{
				// die not lookforwarded by PR, but should be ok in map				
				lMoveWfX = lCalcX;
				lMoveWfY = lCalcY;
				lMoveWfT = lCalcT;
				if (bGetPrescan)
				{
					bRotate = TRUE;
					bPrescanJumpRotate = TRUE;
				}
				else
				{
					if (bUseLF)
					{
						bRotate = TRUE;
					}
				}
			}
		}
		else	//else Long-Jump or LF-disabled
		{
			lMoveWfX = lCalcX;
			lMoveWfY = lCalcY;
			lMoveWfT = lCalcT;
			if (bGetPrescan)
			{
				bRotate = TRUE;
				bPrescanJumpRotate = TRUE;
			}
			else
			{
				if (m_dMoveBackAngle == 0)
				{
					bRotate = TRUE;
				}
				else if ((m_dMoveBackAngle > 0) && (fabs(dTheta) > m_dMoveBackAngle))
				{
					bRotate = TRUE;
				}
			}
		}
	}

	// to get the new position for time calculation
	if (bPrescanJumpRotate && bRotate && IsPrescanEnable() && fabs(m_dPrescanRotateDegree) > 0)
	{
		DOUBLE dDiffDegree = 0;
		dDiffDegree = (DOUBLE)m_lThetaMotorDirection * ((DOUBLE)(lMoveWfT) * m_dThetaRes);
		if (m_dPrescanRotateDegree < 0)
		{
			dDiffDegree = 0 - dDiffDegree;
		}

		if (fabs(dDiffDegree) < fabs(m_dPrescanRotateDegree))
		{
			RotateWaferTheta(&lMoveWfX, &lMoveWfY, &lMoveWfT, dDiffDegree);
			lMoveWfT = 0;
			bRotate = FALSE;
		}
	}
	
	OpCalculateNextEjtOffsetXY( lMoveWfX, lMoveWfY, lMoveWfT,
								bDieLFed, TRUE, FALSE, 
								m_stLFDie.bRotate,
								GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK);	//v4.47A7


	// to get the new position for time calculation
	//v3.66		//DBH only
	LONG lDistX = lMoveWfX - GetLastDieEncX();
	LONG lDistY = lMoveWfY - GetLastDieEncY();
	LONG lTime_X = 0;
	LONG lTime_Y = 0;
	LONG lTime_T = 0;
	BOOL bUseSCFProfileY = FALSE;

#ifdef NU_MOTION
	bUseSCFProfileY = m_stWTAxis_Y.m_bUseSCFProfile;
#endif

	lTime_X = X_ProfileTime(NORMAL_PROF, lDistX, lDistX);
	if (labs(lDistX) > GetSlowProfileDist())
	{
		lTime_X = X_ProfileTime(LOW_PROF, lDistX, lDistX);
	}

	if (bUseSCFProfileY == FALSE)
	{
		lTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);
		if (labs(lDistY) > GetSlowProfileDist())
		{
			lTime_Y = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
		}
	}
	else
	{	
		// must use normal profile for SCF one
		lTime_Y = Y_ScfProfileTime(NORMAL_PROF, lDistY, lDistY);
	}

	LONG lIndexTime = lTime_X;
	if (lTime_Y > lTime_X)
	{
		lIndexTime = lTime_Y;
	}

	if ((bRotate == TRUE) && (abs(lMoveWfT) >= 3))
	{
		LONG lExtraTSleepDelay = (LONG)(fabs((DOUBLE)(lMoveWfT) * m_dThetaRes) * m_lTDelay);	//v3.85
		lTime_T = T_ProfileTime(NORMAL_PROF, lMoveWfT, lMoveWfT) + lExtraTSleepDelay;
		if (lTime_T > lIndexTime)
		{
			lIndexTime = lTime_T;    //v3.85
		}	
	}

	if (!IsBurnIn())
	{
		LONG lWTSettlingDelay = (*m_psmfSRam)["WaferTable"]["SettlingDelay"];
		lIndexTime += lWTSettlingDelay;
	}

	(*m_psmfSRam)["WaferTable"]["IndexTime"]	= lIndexTime;

	return TRUE;
}


BOOL CWaferTable::UpdateMap(const ULONG ulRow, const ULONG ulCol, 
							const UCHAR ucGrade, const WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction, 
							const ULONG ulDieState)

{
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct;
	eUpAct = eAction;
	CString szState, szLogMsg;
	switch (ulDieState)
	{
		case WT_MAP_DS_DEFAULT:
			szState = " - DEFAULT";
			break;
		case WT_MAP_DS_PICK:
			szState = " - PICK";
			break;
		case WT_MAP_DS_PR_DEFECT:
			szState = " - DEFECT";
			break;
		case WT_MAP_DS_PR_INK:
			szState = " - INK";
			break;
		case WT_MAP_DS_PR_CHIP:
			szState = " - CHIP";
			break;
		case WT_MAP_DS_PR_BADCUT:
			szState = " - BADCUT";
			break;
		case WT_MAP_DS_PR_EMPTY:
			szState = " - EMPTY";
			break;
		case WT_MAP_DS_PR_ROTATE:
			szState = " - ROTATE";
			break;
		case WT_MAP_DIESTATE_UNPICK:
			szState = " - UNPICK";
			break;
		default:
			szState.Format(" - %d", ulDieState);
			break;
	}

	// prevent first die is reference die and used to align using normal die record and then updated to missing
	if (m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol))
	{
		ULONG ulStartRow, ulStartCol;
		m_WaferMapWrapper.GetStartPosition(ulStartRow, ulStartCol);

		if ((ulRow == ulStartRow) && (ulCol == ulStartCol) && (eUpAct == WAF_CDieSelectionAlgorithm::MISSING))
		{
			eUpAct = WAF_CDieSelectionAlgorithm::ALIGN;
		}
	}
	else if (IsBlkFunc2Enable() && ((eUpAct == WAF_CDieSelectionAlgorithm::MISSING) || ulDieState == WT_MAP_DS_PR_EMPTY))
	{
		if (m_WaferMapWrapper.GetReader() != NULL)
		{
			UCHAR ucInvalidGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetInvalidBin();
			if (ucGrade == ucInvalidGrade)
			{
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
			}
		}
	}
	
	szLogMsg.Format("MUD1 %d,%d,%d, act %d(%d), stt %s", ulRow, ulCol, ucGrade, eUpAct, eAction, szState);
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
	m_lGetNextDieRow = -1;
	m_lGetNextDieCol = -1;

	if( IsPrescanEnable() && eUpAct==WAF_CDieSelectionAlgorithm::PICK )
	{
		if( CPreBondEvent::m_nMapOrginalCount<=SCAN_SMS_DIE_MAX_LIMIT && m_bSmallMapSortRpt && m_bGenRptOfSMS)
		{
			CString szSMSRptFile = m_szPrescanLogPath + ".sms";	// sorting, update die in file
			CStdioFile cfFile;

			if (cfFile.Open(szSMSRptFile, 
								CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
			{
				CString szData;
				cfFile.SeekToEnd();
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				szData.Format("%ld,%ld,%lu,%lu\n", lUserRow, lUserCol, ulRow, ulCol);
				cfFile.WriteString(szData);
				cfFile.Close();
			}
		}
	}

	return UpdateMapDie(ulRow, ulCol, ucGrade, eUpAct, ulDieState);
}

VOID CWaferTable::OpStopES101NGPIckCycle()
{
	if (m_bIsWaferEnded == FALSE)
	{
		//1. ASk BH taks to move down EJT elevator before WT move
		IPC_CServiceMessage stMsg;
		BOOL bToUp		= FALSE;
		BOOL bReturn	= FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bToUp);
		INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "MoveEjtElvtToUpDown", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
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

		//2. move Table 1/2 to under the wafer camera
		if (bReturn)
		{
			Sleep(200);

			LONG lX = 0, lY = 0;
			if (IsWT2InUse())	//0=WT1 (1=WT2 for ES101)
			{
				X2_Profile(LOW_PROF);
				Y2_Profile(LOW_PROF);
				GetWT2HomeDiePhyPosn(lX, lY);
				XY2_MoveTo(lX, lY);
				X2_Profile(NORMAL_PROF);
				Y2_Profile(NORMAL_PROF);
			}
			else
			{
				X1_Profile(LOW_PROF);
				Y1_Profile(LOW_PROF);
				GetHomeDiePhyPosn(lX, lY);
				X1_MoveTo(lX, SFM_NOWAIT);
				Y1_MoveTo(lY, SFM_NOWAIT);
				X1_Sync();
				Y1_Sync();
				X1_Profile(NORMAL_PROF);
				Y1_Profile(NORMAL_PROF);
			}
		}
	}
}


INT CWaferTable::OpES101MoveToUnload()
{
	INT nResult = gnOK;

	if (IsWLManualMode()==FALSE)	//	non manual mode move to unload position
	{
		if (XY_IsPowerOff())
		{
			SaveScanTimeEvent("    WFT: Unload Wafer Table XY not power ON!");
			HmiMessage_Red_Back("Unload Wafer Table XY not power ON!");
			return gnNOTOK;
		}

		if (IsWT2InUse())
		{
			if( IsWL2ExpanderSafeToMove() == FALSE )
			{
				return gnNOTOK;
			}

			if ( (m_lWT2PositionStatus != 0 && m_lWT2PositionStatus != 3))
			{
				WFTTGoHomeUnderCam(TRUE);
				SaveScanTimeEvent("    WFT: move table 2 to unload position");
				X2_Profile(LOW_PROF1);
				Y2_Profile(LOW_PROF1);
				XY2_MoveTo(m_lUnloadPhyPosX2, m_lUnloadPhyPosY2, SFM_NOWAIT);
				X2_Profile(NORMAL_PROF);
				Y2_Profile(NORMAL_PROF);
				m_lWT2PositionStatus = 3;
			}
		}
		else
		{
			if( IsWL1ExpanderSafeToMove() == FALSE )
			{
				return gnNOTOK;
			}

			if ( (m_lWT1PositionStatus != 0 && m_lWT1PositionStatus != 3))
			{
				WFTTGoHomeUnderCam(FALSE);
				SaveScanTimeEvent("    WFT: move table 1 to unload position");
				X1_Profile(LOW_PROF1);
				Y1_Profile(LOW_PROF1);
				XY1_MoveTo(m_lUnloadPhyPosX, m_lUnloadPhyPosY, SFM_NOWAIT);
				X1_Profile(NORMAL_PROF);
				Y1_Profile(NORMAL_PROF);
				m_lWT1PositionStatus = 3;
			}	//	4.24TX 4
		}
	}

	return nResult;
}


VOID CWaferTable::OpCalculateEjtOffsetXY(LONG& lMoveWfX, LONG& lMoveWfY, LONG& lMoveWfT,
										 BOOL bIsLF,
										 BOOL bCheckLF, 
										 BOOL bDoComp,
										 BOOL bNeedRotate,
										 BOOL bPick,
										 BOOL bGDComp)
{
	//if (!m_bMS100EjtXY)	//v4.52A14
	//	return;
	if (!m_bEnableMS100EjtXY)
		return;

	CString szTemp;					//log
	//EJT OFfset XY in motor steps; encoder resolution of EJT is same as 
	//	WFT XY and BT XT of 0.5um/motor-step;
	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, (m_dXYRes * 1000), TRUE);

	LONG lOldX = lMoveWfX;			//log
	LONG lOldY = lMoveWfY;

	BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
	BOOL bLFNoComp			= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
	//BOOL bCompInLastCycle	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["CompInLastCycle"];
	

	
	LONG lX=0, lY=0, lT=0;			//log
	GetEncoder(&lX, &lY, &lT);
	
	//v4.47A5
	szTemp.Format("OpCalculate EjtOffsetXY - LF=%d, CheckLF=%d, NoComp=%d, Comp=%d (GD=%d), NeedRotate=%d, Pick=%d, BH2toPick=%d",	//v4.47A4
						bIsLF, bCheckLF, bLFNoComp, bDoComp, bGDComp, bNeedRotate, bPick, bBHZ2TowardsPick);
	CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
	
	if (bCheckLF)	//Op MoveTable
	{
		//szTemp.Format("bPick:%s bNeedRotate:%s", bPick?"TRUE":"FALSE", bNeedRotate?"TRUE":"FALSE");
		//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		if ((bNeedRotate || !m_stLFDie.bGoodDie) && bPick)
		{
			bLFNoComp = FALSE;
			(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = FALSE;
		}
		else if ((bNeedRotate || !m_stLFDie.bGoodDie) && !bPick)
		{
			bLFNoComp = TRUE;
			(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = TRUE;
			CMSLogFileUtility::Instance()->BPR_Arm1Log("NoComp set to TRUE (4)");	
		}

		if (!bPick)
		{
			if (m_bDoCompInLastCycle && bIsLF )//&& bLFNoComp)
			{
				if (m_nWTAtColletPos == 2)			//Current WT at Collet2 pos
				{
					lMoveWfX = lMoveWfX - lCollet2OffsetX;
					lMoveWfY = lMoveWfY - lCollet2OffsetY;
					
					szTemp.Format("(m_bDoCompInLastCycle && bLFNoComp) LF-Collet2 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					
				}
				else if (m_nWTAtColletPos == 1)		//Current WT at Collet1 pos
				{
					lMoveWfX = lMoveWfX - lCollet1OffsetX;
					lMoveWfY = lMoveWfY - lCollet1OffsetY;
				
					szTemp.Format("(m_bDoCompInLastCycle && bLFNoComp) LF-Collet1 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
				else
				{
					//No nothing
					szTemp.Format("(m_bDoCompInLastCycle && bLFNoComp) LF-Center to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
			}
			else if (bIsLF) //bLFNoComp)
			{
				if (m_nWTAtColletPos == 2)			//Current WT at Collet2 pos
				{
					//lMoveWfX = lMoveWfX - lCollet2OffsetX;
					//lMoveWfY = lMoveWfY - lCollet2OffsetY;
					
					szTemp.Format("LF-Collet2 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					
				}
				else if (m_nWTAtColletPos == 1)		//Current WT at Collet1 pos
				{
					//lMoveWfX = lMoveWfX - lCollet1OffsetX;
					//lMoveWfY = lMoveWfY - lCollet1OffsetY;
				
					szTemp.Format("LF-Collet1 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
				else
				{
					//No nothing

					szTemp.Format("LF-Center to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
			}
			else
			{
				//No nothing
				//2019.1.3 should do PR&lookahead for current die if not do new lookahead sequence
				(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = FALSE;
				szTemp.Format("No Look Forward to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
			}
			CMSLogFileUtility::Instance()->MS60_Log(szTemp);
			CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
			SetWTAtColletPos(0);
		}
		else if (m_bDoCompInLastCycle && bIsLF && (bNeedRotate||!m_stLFDie.bGoodDie))
		{
			if (m_nWTAtColletPos == 2)	
			{
				lMoveWfX = lMoveWfX - lCollet2OffsetX;
				lMoveWfY = lMoveWfY - lCollet2OffsetY;

				szTemp.Format("(m_bDoCompInLastCycle && bNeedRotate) Rotate - Collet2 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}
			else if (m_nWTAtColletPos == 1)	
			{
				lMoveWfX = lMoveWfX - lCollet1OffsetX;
				lMoveWfY = lMoveWfY - lCollet1OffsetY;

				szTemp.Format("(m_bDoCompInLastCycle && bNeedRotate) Rotate - Collet1 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}
			else 
			{
				szTemp.Format("(m_bDoCompInLastCycle && bNeedRotate) Rotate - Center to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}

			SetWTAtColletPos(0);
			m_bDoCompInLastCycle	= FALSE;
			CMSLogFileUtility::Instance()->MS60_Log(szTemp);
			CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}
		else if (IsBurnIn() || m_bDoCompInLastCycle && bIsLF)		//STATE3
		{
			//LF image grabbing was done at last CDie pos
			//if (!bBHZ2TowardsPick)	//BH1 to PICK

			/*if (!bPick)
			{
				if (m_nWTAtColletPos == 2)	
				{
					lMoveWfX = lMoveWfX - lCollet2OffsetX;
					lMoveWfY = lMoveWfY - lCollet2OffsetY;
					szTemp.Format("LFNoComp-CENTER to CENTER (From Collet2 not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
									lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				else if (m_nWTAtColletPos == 1)	
				{
					lMoveWfX = lMoveWfX - lCollet1OffsetX;
					lMoveWfY = lMoveWfY - lCollet1OffsetY;
					szTemp.Format("LFNoComp-CENTER to CENTER (From Collet1 not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
									lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				SetWTAtColletPos(0);
			}
			else*/ 
			if (m_nWTAtColletPos == 2)			//Current WT at Collet2 pos
			{
				if (!IsBurnIn())
				{
					lMoveWfX = lMoveWfX - lCollet2OffsetX	+ lCollet1OffsetX;
					lMoveWfY = lMoveWfY - lCollet2OffsetY	+ lCollet1OffsetY;
					///lMoveWfX = lMoveWfX + lCollet1OffsetX;
					///lMoveWfY = lMoveWfY + lCollet1OffsetY;
				}
				SetWTAtColletPos(1);
				
				szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	Collet2 to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld),C1(%d,%d),C2(%d,%d)", 
					lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY,lCollet1OffsetX,lCollet1OffsetY,lCollet2OffsetX,lCollet2OffsetY);
					
			}
			else if (m_nWTAtColletPos == 1)	//Current WT at Collet1 pos
			{
				if (!IsBurnIn())
				{
					lMoveWfX = lMoveWfX - lCollet1OffsetX	+ lCollet2OffsetX;
					lMoveWfY = lMoveWfY - lCollet1OffsetY	+ lCollet2OffsetY;
					///lMoveWfX = lMoveWfX + lCollet2OffsetX;
					///lMoveWfY = lMoveWfY + lCollet2OffsetY;
				}

				SetWTAtColletPos(2);
				
				szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	Collet1 to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld),C1(%d,%d),C2(%d,%d)", 
					lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY,lCollet1OffsetX,lCollet1OffsetY,lCollet2OffsetX,lCollet2OffsetY); 
			}
			else 
			{
				//v4.47 Klocwork
				//if (!bPick)
				//{
				//	SetWTAtColletPos(0);
				//	szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	CENTER to CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
				//			lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				//}
				//else
				//{
					//v4.47T2
					if (m_nWTLastColletPos == 1)			//BH2 to PICK
					{
						if (!IsBurnIn())
						{
							lMoveWfX = lMoveWfX + lCollet2OffsetX;
							lMoveWfY = lMoveWfY + lCollet2OffsetY;
						}
						szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	CENTER to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
								lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
						SetWTAtColletPos(2);
					}
					else									//BH1 to PICK
					{
						if (!IsBurnIn())
						{
							lMoveWfX = lMoveWfX + lCollet1OffsetX;
							lMoveWfY = lMoveWfY + lCollet1OffsetY;
						}
						szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	CENTER to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
								lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
						SetWTAtColletPos(1);
					}
				//}
/*
				else if (!bBHZ2TowardsPick)		//BH1 to PICK
				{
					lMoveWfX = lMoveWfX + lCollet1OffsetX;
					lMoveWfY = lMoveWfY + lCollet1OffsetY;
					szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	CENTER to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					SetWTAtColletPos(1);
				}
				else						//BH2 to PICK
				{
					lMoveWfX = lMoveWfX + lCollet2OffsetX;
					lMoveWfY = lMoveWfY + lCollet2OffsetY;
					szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	CENTER to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					SetWTAtColletPos(2);
				}
*/
				//SetWTAtColletPos(0);
			}

			m_bDoCompInLastCycle	= FALSE;
			CMSLogFileUtility::Instance()->MS60_Log(szTemp);
			CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}
		else if (bIsLF)		//bLFNoComp)				//STATE5
		{
			//Perform LF image grabbing on last LF position
			//if (!bBHZ2TowardsPick)	//BH1 to PICK
			if (bNeedRotate||!m_stLFDie.bGoodDie)
			{
				if (m_nWTAtColletPos == 2)	
				{
					//lMoveWfX = lMoveWfX - lCollet2OffsetX;
					//lMoveWfY = lMoveWfY - lCollet2OffsetY;
					szTemp.Format("LF Rotate Collet2 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				else if (m_nWTAtColletPos == 1)
				{
					//lMoveWfX = lMoveWfX + lCollet2OffsetX;
					//lMoveWfY = lMoveWfY + lCollet2OffsetY;
					szTemp.Format("LF Rotate Collet1 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				else
				{
					szTemp.Format("LF Rotate Center rotate, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				SetWTAtColletPos(0);
			}
			else if (m_nWTAtColletPos == 2)		//WT from Collet2 to Collet1
			{
				//So next die should be BH1 to PICK
				lMoveWfX = lMoveWfX + lCollet1OffsetX;
				lMoveWfY = lMoveWfY + lCollet1OffsetY;
				SetWTAtColletPos(1);
				
				szTemp.Format("LF to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);	
			}
			else if (m_nWTAtColletPos == 1)
			{
				//So next die should be BH2 to PICK
				lMoveWfX = lMoveWfX + lCollet2OffsetX;
				lMoveWfY = lMoveWfY + lCollet2OffsetY;
				SetWTAtColletPos(2);
				
				szTemp.Format("LF to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}
			else		//else at CENTER
			{
				//if (!bBHZ2TowardsPick)			//BH1 to PICK
				//v4.47T2
				if (m_nWTLastColletPos == 1)		//BH2 to PICK
				{
					//So next die should be BH2 to PICK
					lMoveWfX = lMoveWfX + lCollet2OffsetX;
					lMoveWfY = lMoveWfY + lCollet2OffsetY;
					SetWTAtColletPos(2);
					
					szTemp.Format("LF CENTER to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				else								//BH1 to PICK
				{
					//So next die should be BH1 to PICK
					lMoveWfX = lMoveWfX + lCollet1OffsetX;
					lMoveWfY = lMoveWfY + lCollet1OffsetY;
					SetWTAtColletPos(1);
					
					szTemp.Format("LF CENTER to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
			}
			CMSLogFileUtility::Instance()->MS60_Log(szTemp);
			CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}
		else							//STATE1
		{
			if (m_nWTAtColletPos != 0)	//If current NOT at CENTER
			{
				//Make next INDEX pos back to CENTER
				if (m_nWTAtColletPos == 2)			//WT from Collet2 to CENTER
				{
		/*			
					if (!IsPrescanEnable() )
					{
						lMoveWfX = lMoveWfX - lCollet2OffsetX;
						lMoveWfY = lMoveWfY - lCollet2OffsetY;
					}
		*/			
					szTemp.Format("No-LF Collet2 to CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
							lX, lY, lT, lMoveWfX, lMoveWfY);
					
				}
				else if (m_nWTAtColletPos == 1)		//WT from Collet1 to CENTER
				{
		/*			
					if (!IsPrescanEnable() )
					{
						lMoveWfX = lMoveWfX - lCollet1OffsetX;
						lMoveWfY = lMoveWfY - lCollet1OffsetY;
					}
		*/		
					szTemp.Format("No-LF Collet1 to CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
								lX, lY, lT, lMoveWfX, lMoveWfY);
					
				}
				else
				{
					
					szTemp.Format("No-LF CENTER to CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
								lX, lY, lT, lMoveWfX, lMoveWfY);
					
				}

				SetWTAtColletPos(0);
			}
			else
			{
				
				szTemp.Format("No-LF at CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
								lX, lY, lT, lMoveWfX, lMoveWfY);
				
			}

			m_bDoCompInLastCycle = FALSE;
			CMSLogFileUtility::Instance()->MS60_Log(szTemp);
			CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}

	}
	else	//DoComp	//STATE2
	{
		if (!bPick || 
			(bGDComp == FALSE))		//v4.48A25	//BadDieComp should go to CENTER because it is NOT picked
		{
			//do nothing
			SetWTAtColletPos(0);
			if (bGDComp == FALSE)
			{
				szTemp.Format("COMP (BADDIE) to C, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld); Offset(%ld,%ld)", 
					lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet1OffsetX, lCollet1OffsetY);
			}
			else
			{
				szTemp.Format("COMP to C, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld); Offset(%ld,%ld)", 
					lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet1OffsetX, lCollet1OffsetY);
			}
		}
		//v4.47A6
		else if (m_nWTLastColletPos == 1)
		{
			lMoveWfX = lMoveWfX + lCollet2OffsetX;
			lMoveWfY = lMoveWfY + lCollet2OffsetY;
			SetWTAtColletPos(2);
			
			szTemp.Format("COMP to C2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld); Offset(%ld,%ld)", 
				lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet2OffsetX, lCollet2OffsetY);
		}
		else
		{
			lMoveWfX = lMoveWfX + lCollet1OffsetX;
			lMoveWfY = lMoveWfY + lCollet1OffsetY;
			SetWTAtColletPos(1);
			
			szTemp.Format("COMP to C1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld); Offset(%ld,%ld)", 
				lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet1OffsetX, lCollet1OffsetY);
		}

/*
		else if (!bBHZ2TowardsPick)		//BH1 to PICK
		{
			lMoveWfX = lMoveWfX + lCollet1OffsetX;
			lMoveWfY = lMoveWfY + lCollet1OffsetY;
			SetWTAtColletPos(1);
			
			szTemp.Format("COMP to C1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld); Offset(%ld,%ld)", 
				lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet1OffsetX, lCollet1OffsetY);
		}
		else
		{
			lMoveWfX = lMoveWfX + lCollet2OffsetX;
			lMoveWfY = lMoveWfY + lCollet2OffsetY;
			SetWTAtColletPos(2);
			
			szTemp.Format("COMP to C2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld); Offset(%ld,%ld)", 
				lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet2OffsetX, lCollet2OffsetY);
		}
*/
		//(*m_psmfSRam)["WaferTable"]["CompInLastCycle"] = TRUE;
		m_bDoCompInLastCycle	= TRUE;
		CMSLogFileUtility::Instance()->MS60_Log(szTemp);
		CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
	}
}

VOID CWaferTable::OpCalculateNextEjtOffsetXY(LONG& lMoveWfX, LONG& lMoveWfY, LONG& lMoveWfT,
										 BOOL bIsLF,
										 BOOL bCheckLF, 
										 BOOL bDoComp,
										 BOOL bNeedRotate,
										 BOOL bPick)
{
	//if (!m_bMS100EjtXY)	//v4.52A14
	//	return;
	if (!m_bEnableMS100EjtXY)
		return;

	CString szTemp;					//log

	//EJT OFfset XY in motor steps; encoder resolution of EJT is same as 
	//	WFT XY and BT XT of 0.5um/motor-step;
	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, (m_dXYRes * 1000), TRUE);

	LONG lOldX = lMoveWfX;			//log
	LONG lOldY = lMoveWfY;

	BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
	BOOL bLFNoComp			= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
	//BOOL bCompInLastCycle	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["CompInLastCycle"];
	
	
	LONG lX=0, lY=0, lT=0;			//log
	GetEncoder(&lX, &lY, &lT);
	
	//v4.47A5
	szTemp.Format("OpCalculate NextEjtOffsetXY - LF=%d, CheckLF=%d, NoComp=%d, Comp=%d, NeedRotate=%d, Pick=%d, BH2toPick=%d",	//v4.47A4
						bIsLF, bCheckLF, bLFNoComp, bDoComp, bNeedRotate, bPick, bBHZ2TowardsPick);
	CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
	
	if (bCheckLF)	//Op MoveTable
	{
		if ( (bNeedRotate||!m_stLFDie.bGoodDie) && bPick )
		{
			bLFNoComp = FALSE;
			(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = FALSE;
		}
		else if ( (bNeedRotate||!m_stLFDie.bGoodDie) && !bPick )
		{
			bLFNoComp = TRUE;
			(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = TRUE;
			CMSLogFileUtility::Instance()->BPR_Arm1Log("Next NoComp set to TRUE (4)");	
		}

		if (!bPick)
		{
			if (m_bDoCompInLastCycle && bIsLF )//&& bLFNoComp)
			{
				if (m_nWTAtColletPos == 2)			//Current WT at Collet2 pos
				{
					lMoveWfX = lMoveWfX - lCollet2OffsetX;
					lMoveWfY = lMoveWfY - lCollet2OffsetY;
					
					szTemp.Format("(m_bDoCompInLastCycle && bLFNoComp) LF-Collet2 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					
				}
				else if (m_nWTAtColletPos == 1)		//Current WT at Collet1 pos
				{
					lMoveWfX = lMoveWfX - lCollet1OffsetX;
					lMoveWfY = lMoveWfY - lCollet1OffsetY;
				
					szTemp.Format("(m_bDoCompInLastCycle && bLFNoComp) LF-Collet1 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
				else
				{
					//No nothing
					szTemp.Format("(m_bDoCompInLastCycle && bLFNoComp) LF-Center to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
			}
			else if (bIsLF) //bLFNoComp)
			{
				if (m_nWTAtColletPos == 2)			//Current WT at Collet2 pos
				{
					szTemp.Format("LF-Collet2 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					
				}
				else if (m_nWTAtColletPos == 1)		//Current WT at Collet1 pos
				{
					szTemp.Format("LF-Collet1 to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
				else
				{
					//No nothing
					szTemp.Format("LF-Center to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
				}
			}
			else
			{
				//No nothing
				szTemp.Format("No Look Forward to Center(not pick), Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
			}

			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
			//SetWTAtColletPos(0);
			
		}
		else if (m_bDoCompInLastCycle && bIsLF && (bNeedRotate||!m_stLFDie.bGoodDie))
		{
			if (m_nWTAtColletPos == 2)	
			{
				lMoveWfX = lMoveWfX - lCollet2OffsetX;
				lMoveWfY = lMoveWfY - lCollet2OffsetY;
				szTemp.Format("(m_bDoCompInLastCycle && bNeedRotate) Rotate - Collet2 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}
			else if (m_nWTAtColletPos == 1)	
			{
				lMoveWfX = lMoveWfX - lCollet1OffsetX;
				lMoveWfY = lMoveWfY - lCollet1OffsetY;
				szTemp.Format("(m_bDoCompInLastCycle && bNeedRotate) Rotate - Collet1 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}
			else 
			{
				szTemp.Format("(m_bDoCompInLastCycle && bNeedRotate) Rotate - Center to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}

			//SetWTAtColletPos(0);
			//m_bDoCompInLastCycle	= FALSE;
			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}
		else if (m_bDoCompInLastCycle && bIsLF)		//STATE3
		{
			//LF image grabbing was done at last CDie pos
			//if (!bBHZ2TowardsPick)	//BH1 to PICK

			if (m_nWTAtColletPos == 2)			//Current WT at Collet2 pos
			{
				lMoveWfX = lMoveWfX - lCollet2OffsetX	+ lCollet1OffsetX;
				lMoveWfY = lMoveWfY - lCollet2OffsetY	+ lCollet1OffsetY;

				//SetWTAtColletPos(1);
				szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	Collet2 to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
					lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					
			}
			else if (m_nWTAtColletPos == 1)	//Current WT at Collet1 pos
			{
				lMoveWfX = lMoveWfX - lCollet1OffsetX	+ lCollet2OffsetX;
				lMoveWfY = lMoveWfY - lCollet1OffsetY	+ lCollet2OffsetY;
				
				//SetWTAtColletPos(2);
				szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	Collet1 to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
					lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY); 
			}
			else 
			{
				//v4.47T2
				if (m_nWTLastColletPos == 1)			//BH2 to PICK
				{
					lMoveWfX = lMoveWfX + lCollet2OffsetX;
					lMoveWfY = lMoveWfY + lCollet2OffsetY;
					szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	CENTER to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					//SetWTAtColletPos(2);
				}
				else									//BH1 to PICK
				{
					lMoveWfX = lMoveWfX + lCollet1OffsetX;
					lMoveWfY = lMoveWfY + lCollet1OffsetY;
					szTemp.Format("(m_bDoCompInLastCycle && bIsLF)	CENTER to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
					//SetWTAtColletPos(1);
				}
			}

			//m_bDoCompInLastCycle	= FALSE;
			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}
		else if (bIsLF)		//bLFNoComp)				//STATE5
		{
			//Perform LF image grabbing on last LF position
			//if (!bBHZ2TowardsPick)	//BH1 to PICK
			if (bNeedRotate||!m_stLFDie.bGoodDie)
			{
				if (m_nWTAtColletPos == 2)	
				{
					szTemp.Format("LF Rotate Collet2 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				else if (m_nWTAtColletPos == 1)
				{
					szTemp.Format("LF Rotate Collet1 to Center, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				else
				{
					szTemp.Format("LF Rotate Center rotate, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				//SetWTAtColletPos(0);
			}
			else if (m_nWTAtColletPos == 2)		//WT from Collet2 to Collet1
			{
				//So next die should be BH1 to PICK
				lMoveWfX = lMoveWfX + lCollet1OffsetX;
				lMoveWfY = lMoveWfY + lCollet1OffsetY;
				//SetWTAtColletPos(1);
				szTemp.Format("LF to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);	
			}
			else if (m_nWTAtColletPos == 1)
			{
				//So next die should be BH2 to PICK
				lMoveWfX = lMoveWfX + lCollet2OffsetX;
				lMoveWfY = lMoveWfY + lCollet2OffsetY;
				//SetWTAtColletPos(2);	
				szTemp.Format("LF to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
						lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
			}
			else		//else at CENTER
			{
				if (m_nWTLastColletPos == 1)		//BH2 to PICK
				{
					//So next die should be BH2 to PICK
					lMoveWfX = lMoveWfX + lCollet2OffsetX;
					lMoveWfY = lMoveWfY + lCollet2OffsetY;
					//SetWTAtColletPos(2);		
					szTemp.Format("LF CENTER to Collet2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
				else								//BH1 to PICK
				{
					//So next die should be BH1 to PICK
					lMoveWfX = lMoveWfX + lCollet1OffsetX;
					lMoveWfY = lMoveWfY + lCollet1OffsetY;
					//SetWTAtColletPos(1);	
					szTemp.Format("LF CENTER to Collet1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld)", 
							lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY);
				}
			}

			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}
		else							//STATE1
		{
			if (m_nWTAtColletPos != 0)	//If current NOT at CENTER
			{
				//Make next INDEX pos back to CENTER
				if (m_nWTAtColletPos == 2)			//WT from Collet2 to CENTER
				{
					szTemp.Format("No-LF Collet2 to CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
							lX, lY, lT, lMoveWfX, lMoveWfY);
					
				}
				else if (m_nWTAtColletPos == 1)		//WT from Collet1 to CENTER
				{
					szTemp.Format("No-LF Collet1 to CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
								lX, lY, lT, lMoveWfX, lMoveWfY);		
				}
				else
				{
					szTemp.Format("No-LF CENTER to CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
								lX, lY, lT, lMoveWfX, lMoveWfY);
				}
				//SetWTAtColletPos(0);
			}
			else
			{
				szTemp.Format("No-LF at CENTER, Enc(%ld, %ld, %ld), Target(%ld, %ld)", 
								lX, lY, lT, lMoveWfX, lMoveWfY);
			}

			//m_bDoCompInLastCycle = FALSE;
			//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
		}

	}
	else	//DoComp	//STATE2
	{
		if (!bPick)
		{
			//do nothing
			//SetWTAtColletPos(0);
			szTemp.Format("COMP to C, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC(%ld, %ld); Offset(%ld,%ld)", 
				lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet1OffsetX, lCollet1OffsetY);
		}
		else if (m_nWTLastColletPos == 1)
		{
			lMoveWfX = lMoveWfX + lCollet2OffsetX;
			lMoveWfY = lMoveWfY + lCollet2OffsetY;
			//SetWTAtColletPos(2);
			szTemp.Format("COMP to C2, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC2(%ld, %ld); Offset(%ld,%ld)", 
				lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet2OffsetX, lCollet2OffsetY);
		}
		else
		{
			lMoveWfX = lMoveWfX + lCollet1OffsetX;
			lMoveWfY = lMoveWfY + lCollet1OffsetY;
			//SetWTAtColletPos(1);
			szTemp.Format("COMP to C1, Enc(%ld, %ld, %ld), Target(%ld, %ld), TC1(%ld, %ld); Offset(%ld,%ld)", 
				lX, lY, lT, lOldX, lOldY, lMoveWfX, lMoveWfY, lCollet1OffsetX, lCollet1OffsetY);
		}

		//(*m_psmfSRam)["WaferTable"]["CompInLastCycle"] = TRUE;
		//m_bDoCompInLastCycle	= TRUE;
		//CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
	}
}


VOID CWaferTable::SetWTAtColletPos(CONST UINT unPos)
{
	if (m_nChangeColletOnWT)
	{
		return;
	}

	if (m_nWTAtColletPos != 0)
		m_nWTLastColletPos = m_nWTAtColletPos;		//v4.47T2
	m_nWTAtColletPos = unPos;
	
	CString szTemp;
	szTemp.Format("EJT ColletPos set to #%d (#%d)",  m_nWTAtColletPos, m_nWTLastColletPos);
	CMSLogFileUtility::Instance()->BPR_Arm1Log(szTemp);	
}


INT CWaferTable::OpGetWaferPos_Region()
{
	INT				nResult	= gnOK;
	unsigned long	ulX = 0, ulY = 0, ulIndex = 0;
	unsigned char   ucGrade;
	BOOL			bRegnEndOfWafer;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;

	if (IsBlkFunc1Enable() && !m_bFirstCycleWT )	//Block1
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("region pick + blk func 1 to get wafer pos");
		DieMapPos CurDieMap;
		DiePhyPos CurDiePhy;
		INT i;
		ULONG ulNumRow, ulNumCol;
		m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

		//v3.25T16
		DieMapPos CurrDie;
		CurrDie.Row = GetLastDieRow();
		CurrDie.Col = GetLastDieCol();
		BOOL bNeedUpdate = m_pBlkFunc->IsCurrBlkNeedUpdate(CurrDie);

		for (i = 0; i < 8; i++)
		{
			CurDieMap.Row = CurDieMap.Col = 1000;
			switch (i)
			{
				case 0:
					if (GetLastDieCol() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow();
					CurDieMap.Col = GetLastDieCol() - 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosY"];
					break;
				case 1:
					if (GetLastDieCol() == ulNumCol - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow();
					CurDieMap.Col = GetLastDieCol() + 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosY"];
					break;
				case 2:
					if (GetLastDieRow() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() - 1;
					CurDieMap.Col = GetLastDieCol();
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosY"];
					break;
				case 3:
					if (GetLastDieRow() == ulNumRow - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() + 1;
					CurDieMap.Col = GetLastDieCol();
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosY"];
					break;
				case 4:
					if (GetLastDieRow() == 0 || GetLastDieCol() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() - 1;
					CurDieMap.Col = GetLastDieCol() - 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosY"];
					break;
				case 5:
					if (GetLastDieRow() == ulNumRow - 1 || GetLastDieCol() == 0) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() + 1;
					CurDieMap.Col = GetLastDieCol() - 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosY"];
					break;
				case 6:
					if (GetLastDieRow() == 0 || GetLastDieCol() == ulNumCol - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() - 1;
					CurDieMap.Col = GetLastDieCol() + 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosY"];
					break;
				case 7:
					if (GetLastDieRow() == ulNumRow - 1 || GetLastDieCol() == ulNumCol - 1) 
					{
						break;
					}
					CurDieMap.Row = GetLastDieRow() + 1;
					CurDieMap.Col = GetLastDieCol() + 1;
					CurDiePhy.x = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosX"];
					CurDiePhy.y = (LONG)(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosY"];
					break;
			}

			if (CurDieMap.Row == 1000 || CurDieMap.Col == 1000) 
			{
				continue;
			}
			if (m_WaferMapWrapper.GetReader()->IsSelected(CurDieMap.Row, CurDieMap.Col))	//if EDGE die
			{
				continue;
			}

			//v3.25T13
			if (m_bBlkLongJump)
			{
				if (IsMapNullBin(CurDieMap.Row, CurDieMap.Col))
				{
					if ((CurDiePhy.x != 0) || (CurDiePhy.y != 0))
					{
						CString szErr;
						szErr.Format("ERROR: die found at EMPTY die position (%d) after LONG-JUMP!  Please realign wafer.", i);
						HmiMessage_Red_Yellow(szErr);
						SetErrorMessage(szErr);
						SetAlignmentStatus(FALSE);	//force to realign wafer
						return Err_WTBlkLongJump;
					}
				}
				else
				{
					if (m_WaferMapWrapper.GetReader()->IsMissingDie(CurDieMap.Row, CurDieMap.Col))
					{
					}
					else if (m_WaferMapWrapper.GetReader()->IsSelected(CurDieMap.Row, CurDieMap.Col))
					{
					}
					else
					{
						if ((CurDiePhy.x == 0) && (CurDiePhy.y == 0))
						{
							CString szErr;
							szErr.Format("ERROR: no die found at die position (%d) after LONG-JUMP!  Please realign wafer", i);
							HmiMessage_Red_Yellow(szErr);
							SetErrorMessage(szErr);
							SetAlignmentStatus(FALSE);	//force to realign wafer
							return Err_WTBlkLongJump;
						}
					}
				}
			}

			if (CurDiePhy.x == 0 && CurDiePhy.y == 0) 
			{
				continue;
			}

			//v3.25T16
			if (!bNeedUpdate)
			{
				if ((i == 3) || (i == 5) || (i == 7))	// (DL, DN, DR)	PR sub-wnd
				{
					continue;
				}
			}

			CurDiePhy.x += m_stPosOnPR.lX_Enc;
			CurDiePhy.y += m_stPosOnPR.lY_Enc;
			CurDieMap.Status = WAF_CDieSelectionAlgorithm::ALIGN;
			CurDiePhy.t = m_stPosOnPR.lT_Enc;
			m_pBlkFunc->SetCurDiePhyPos(CurDieMap, CurDiePhy, FALSE);

			UCHAR ucDieGrade = m_WaferMapWrapper.GetGrade((ULONG)(CurDieMap.Row), (ULONG)CurDieMap.Col);
			if (ucDieGrade != m_WaferMapWrapper.GetGrade(GetLastDieRow(), GetLastDieCol())
					&& ucDieGrade != 99 + m_WaferMapWrapper.GetGradeOffset()
					&& GetLastDieEAct() == WAF_CDieSelectionAlgorithm::PICK
					&& (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBin"] == FALSE)
			{
				(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBin"] = TRUE;
				(*m_psmfSRam)["WaferPr"]["LAround"]["ChgBinTo"] = ucDieGrade;
			}
		}
	}

	m_bFirstCycleWT = FALSE;
	PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bRegnEndOfWafer, "OpGetPos Regn");

	if (bRegnEndOfWafer)
	{
		if( RestartDefectDieSort() )
		{
			PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bRegnEndOfWafer, "Regn defect pick");
		}
	}
	else if ( !IsBlkFunc1Enable() )
	{
		ULONG ulState = GetMapDieState(ulY, ulX);
		ULONG ulPeekCounter = 1;
		WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct = eAction;
		BOOL bLoopFind = FALSE;	// region

		if( IsDieUnpickScan(ulState) )
		{
			eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
			bLoopFind = TRUE;
		}

		if( !IsPrescanBlkPick())
		{
			if( ulState==WAF_CDieSelectionAlgorithm::INVALID || 
				ulState==WAF_CDieSelectionAlgorithm::MISSING )
			{
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
				bLoopFind = TRUE;
			}
		}

		if( bLoopFind )
		{
			UpdMapLastDie("GetWaerPos Regn");
		}

		while (!bRegnEndOfWafer && bLoopFind)
		{
			GetMapNextDie(ulY, ulX, ucGrade, eAction, bRegnEndOfWafer);
			SetMapNextDie(ulY, ulX, ucGrade, eUpAct, ulState);

			ulPeekCounter++;
			if (ulPeekCounter > 1000)
			{
				break;
			}

			//Peek another new die after updating curr die state
			PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bRegnEndOfWafer, "Loop Regn");	
			eUpAct = eAction;
			ulState = GetMapDieState(ulY, ulX);

			bLoopFind = FALSE;
			if( IsDieUnpickScan(ulState) )
			{
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
				bLoopFind = TRUE;
			}
			if( !IsPrescanBlkPick())
			{
				if( ulState==WAF_CDieSelectionAlgorithm::INVALID || 
					ulState==WAF_CDieSelectionAlgorithm::MISSING )
				{
					eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
					bLoopFind = TRUE;
				}
			}
		}
	}

	// xuzhijin_region check the next die is another region, stop sorting and continue align->scan next one
	CString szMsg;

	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
	BOOL  bSameRegion = pSRInfo->IsWithinThisRegion(ulTgtRegion, ulY, ulX);
	if( bSameRegion==FALSE )
	{
		LONG lRow, lCol, lX, lY;
		pUtl->GetAlignPosition(lRow, lCol, lX, lY);
		if( ulY==lRow && ulX==lCol || m_bJustAlign || m_bFirst )
		{
			bSameRegion = TRUE;
		}
	}

	BOOL bOutOfRegion = FALSE;
	if (bRegnEndOfWafer)
	{
		bOutOfRegion = TRUE;
		pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDED);
		SaveRegionStateAndIndex();
	}
	else if (bSameRegion == FALSE)
	{
		bOutOfRegion = TRUE;
		pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_BONDED);
		SaveRegionStateAndIndex();
	}

	BOOL bToNextRegion = FALSE;
	if ( bOutOfRegion)	// region pick xuxuxu
	{
		m_ulReverifyReferRegion	= pSRInfo->GetTargetRegion();
		pUtl->RegionOrderLog("REGN Loop for next valid region(not bonded)");

		if( bSameRegion==FALSE )
		{
			if (GetRegionSortOuter())
			{
				ULONG ulNewTgtRegion = pSRInfo->GetWithinRegionNo(ulY, ulX);
				pSRInfo->SetTargetRegion(ulNewTgtRegion);		// region pick xuxuxu
				pSRInfo->SetRegionState(ulNewTgtRegion, WT_SUBREGION_STATE_BONDING);
			}
		}
		pSRInfo->SetRegionManualConfirm(FALSE);
		BOOL bFindNext = pSRInfo->FindNextAutoSortRegion(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(),
														 GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol());
		if (bFindNext == FALSE)
		{
			pUtl->RegionOrderLog("REGN Last auto region sorting complete");
			if (pSRInfo->IsAllRegionsBonded(IsMS90HalfSortMode(), IsRowModeSeparateHalfMap(), GetMS90HalfMapMaxRow(), GetMS90HalfMapMaxCol()) == FALSE)
			{
				pUtl->RegionOrderLog("but no all regions bonded");

				CString szTemp;
				szMsg = "Sorting need to manual align for regions\nDo you want to continue?\n";
				for (ULONG k = 1; k <= pSRInfo->GetTotalSubRegions(); k++)
				{
					if (pSRInfo->GetRegionState(k) == WT_SUBREGION_STATE_SKIPPED)
					{
						szTemp.Format("(%d) ", k);
						szMsg += szTemp;
					}
				}

				pUtl->RegionOrderLog(szMsg);

				SetErrorMessage(szMsg);
				if (HmiMessage_Red_Back(szMsg, "Region Sorting", glHMI_MBX_YESNO) == glHMI_YES)
				{
					szTemp = "Operator select continue when last auto region bonded";
					pSRInfo->SetRegionManualConfirm(TRUE);
					bRegnEndOfWafer = FALSE;
				}
				else
				{
					szTemp = "Operator select stop and wafer end when last auto region bonded, SKIPPED RESET TO BONDED";
					bRegnEndOfWafer = TRUE;
					for (ULONG k = 1; k <= pSRInfo->GetTotalSubRegions(); k++)
					{
						if (pSRInfo->GetRegionState(k) == WT_SUBREGION_STATE_SKIPPED)
						{
							pSRInfo->SetRegionState(k, WT_SUBREGION_STATE_BONDED);
						}
					}
				}
				SetErrorMessage(szTemp);
				pUtl->RegionOrderLog(szTemp);
			}
			else
			{
				bRegnEndOfWafer = TRUE;
			}
		}
		else
		{
			bRegnEndOfWafer = FALSE;
		}

		if (bRegnEndOfWafer == FALSE)
		{
			bToNextRegion = TRUE;
		}


		if (!IsMS90HalfSortMode() || m_b2Parts1stPartDone || (pSRInfo->GetSubRows() > 1) || (pSRInfo->GetSubCols() > 1))
		{
			if (!IsWLAutoMode() && pSRInfo->IsManualAlignRegion())
			{
				if (bRegnEndOfWafer == FALSE)
				{
					szMsg = "Manual Align Sorting Region Complete.";
				}
				else
				{
					szMsg = "Manual Align Sorting All Regions complete.";
				}
				HmiMessage_Red_Yellow(szMsg, "AUTOBOND");
				SetErrorMessage(szMsg);
				pUtl->RegionOrderLog(szMsg);
				if (bToNextRegion)
				{
					if( IsNextRegionAutoAlign() )
						szMsg = "Press the start button to continue auto sorting.";
					else
						szMsg = "Please manual align a region to continue sorting.";
					HmiMessage_Red_Yellow(szMsg, "AUTO BOND");
				}
			}
			else
			{
				if (bRegnEndOfWafer)
				{
					pUtl->RegionOrderLog("REGN All regions sorting complete");
				}
			}
		}

		SaveRegionStateAndIndex();
	}

	//Update current BLOCK number in Block-Pick mode (NOT for regular ref die)
	m_bBlkLongJump = FALSE;
	//BLOCKPICK (NOT for regular ref die)
	if (IsBlkFunc1Enable())	//Block2
	{
		if( !bRegnEndOfWafer && 
			(eAction == WAF_CDieSelectionAlgorithm::PICK || eAction == WAF_CDieSelectionAlgorithm::SKIP))
		{
			ULONG ulState = GetMapDieState(ulY, ulX);
			while (!bRegnEndOfWafer &&
					((ulState == WT_MAP_DS_DEFAULT) || IsDieEmpty(ulState) || IsDieUnpickAll(ulState)))
			{
				CMSLogFileUtility::Instance()->WT_GetDieLog("Regn get wafer pos loop of block");
				GetMapNextDie(ulY, ulX, ucGrade, eAction, bRegnEndOfWafer);

				WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct = eAction;
				ULONG lUpState = ulState;
				switch (ulState)
				{
				case WT_MAP_DS_DEFAULT:
					eUpAct = WAF_CDieSelectionAlgorithm::ALIGN;
					lUpState = WT_MAP_DS_DEFAULT;
					break;
				case WT_MAP_DS_PR_EMPTY:
					m_WaferMapWrapper.SetAlgorithmParameter("Row", ulY);
					m_WaferMapWrapper.SetAlgorithmParameter("Col", ulX);
					m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Empty);

					eUpAct = WAF_CDieSelectionAlgorithm::MISSING;
					lUpState = WT_MAP_DS_PR_EMPTY;
					break;
				case WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY:
				case WT_MAP_DS_UNPICK_REGRAB_EMPTY:
				case WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT:
				case WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT:
					eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
					lUpState = ulState;
					break;
				case WT_MAP_DIESTATE_UNPICK:
					eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
					lUpState = WT_MAP_DIESTATE_UNPICK;
					break;
				}
				SetMapNextDie(ulY, ulX, ucGrade, eUpAct, lUpState);

				m_WaferMapWrapper.SetAlgorithmParameter("Row", 0);
				m_WaferMapWrapper.SetAlgorithmParameter("Col", 0);
				m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);

				//Peek another new die after updating curr die state
				PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bRegnEndOfWafer, "block pick 1 in region");	
				ulState = m_WaferMapWrapper.GetDieState(ulY, ulX);
			}
		}

		if ( !bRegnEndOfWafer && (eAction == WAF_CDieSelectionAlgorithm::PICK))	//Block2
		{
			DieMapPos MapPos;
			MapPos.Row = ulY;
			MapPos.Col = ulX;
			MapPos.Status = 0;	//Klocwork
			LONG lBlkRow = 0, lBlkCol = 0;

			LONG lBlk = m_pBlkFunc->GetCurrBlockNum(lBlkRow, lBlkCol, MapPos);

			LONG lCurrRow = m_lCurrBlockNum / 100;
			LONG lCurrCol = m_lCurrBlockNum % 100;
			LONG lNextRow = lBlk / 100;
			LONG lNextCol = lBlk % 100;

			if ((m_lCurrBlockNum != 0) && (lBlk != 0))
			{
				if (abs(lCurrRow - lNextRow) >= 2)
				{
					m_bBlkLongJump = TRUE;
				}
				else if (abs(lCurrCol - lNextCol) >= 3)
				{
					m_bBlkLongJump = TRUE;
				}
			}

			//v3.25T16	//OptoTech
			if (lBlk != m_lCurrBlockNum)
			{
				if (m_bBlkLongJump)
				{
					LOG_BLOCK_ALIGN("Long Jump!");
				}
				CString szLog;
				szLog.Format("Sorting %d  (old = %d)", lBlk, m_lCurrBlockNum);
				LOG_BLOCK_ALIGN(szLog);
			}

			m_lCurrBlockNum = lBlk;
		}

		//BLOCKPICK
		(*m_psmfSRam)["WaferPr"]["LookAround"]["X"]			= ulX;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["Y"]			= ulY;
		(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosY"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosY"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosY"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosY"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosY"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosY"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosY"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosX"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosY"]		= 0;

		(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] = FALSE;	// get pos, block pick 1
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsAlign"]		= FALSE;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsGood"]			= FALSE;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieTNeedRotate"]	= FALSE;
	}


	if (bRegnEndOfWafer == TRUE)
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("WAFER-END");
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End in WT");
		return Err_WTWaferEnd;
	}

	//Reset wafer end signal
	m_bIsWaferEnded		= FALSE;

	if (bToNextRegion)
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		//Set the state of the next region as "Auto" or "Manual"
		pSRInfo->SetCurrentRegionState_HMI(IsNextRegionAutoAlign() ? "A" : "M");

		SaveScanTimeEvent("    WFT: REGION END region sorting complete reset k offset");
		pSRInfo->SetRegionEnd(TRUE);
		pSRInfo->SetRegionEnding(TRUE); // region end
		SetWTReady(FALSE, "19");
		DisplaySequence("WT - REGION END region sorting complete reset k offset");
		return Err_WTWaferEnd;
	}

	(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["Valid"]	= 0;
	(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"]	= 0;

	if (IsBlkFunc2Enable())	//Block2
	{
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = -1;
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = -1;
		(*m_psmfSRam)["WaferMap"]["ComingDieDirection"] = -1;
	}
	else
	{
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"]	= 0;
		(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"]	= 0;
	}

	if (eAction == WAF_CDieSelectionAlgorithm::PICK)
	{	
		// Store data to Die Info
		ulIndex = (*m_psmfSRam)["DieInfo"]["WT_Index"];
		ulIndex++;
		(*m_psmfSRam)["DieInfo"]["WT_Index"] = ulIndex;

		if (DieIsNoPick(ulY, ulX) == TRUE)
		{
			eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		}

		if (DieIsSpecialRefGrade(ulY, ulX))				//v3.24T1
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"] = 1;		//Tell WPR to align this PICK die with Ref-Die pattern
		}

		m_lRealignDieNum++;		//Block2
	}
	else if (eAction == WAF_CDieSelectionAlgorithm::CHECK)
	{
		BlkFunc2ReferDieCheck(ulY, ulX);
		
		(*m_psmfSRam)["DieInfo"]["RefDie"]["X"]		= ulX;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Y"]		= ulY;
		(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"]	= 1;

		if (IsBlkFunc2Enable() && IsCharDieInUse())
		{
			if (m_WaferMapWrapper.HasReferenceFaceValue(ulY, ulX) == TRUE)
			{
				ULONG ulRefValue = m_WaferMapWrapper.GetReferenceDieFaceValue(ulY, ulX);
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = ulRefValue / 10;
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = ulRefValue % 10;
			}
			else
			{
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"] = -1;
				(*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"] = -1;
			}
		}
	}
	else if (eAction == WAF_CDieSelectionAlgorithm::SEPARATION)
	{
		if (m_bCheckSepGrade == TRUE)
		{
			return Err_WTHitSeparationGrade; 
		}
	}
	
	if( m_WaferMapWrapper.IsReferenceDie(ulY, ulX) )
	{
		LONG lRow, lCol, lX, lY;
		pUtl->GetAlignPosition(lRow, lCol, lX, lY);
		if ((ulX == lCol) && (ulY == lRow))
		{
			CString szMsg;
			szMsg.Format("HomeDie (%d,%d) is ReferDie in get pos general", ulY, ulX);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			if (eAction == WAF_CDieSelectionAlgorithm::PICK)
			{
				eAction = WAF_CDieSelectionAlgorithm::ALIGN;
			}
			(*m_psmfSRam)["DieInfo"]["RefDie"]["Check"] = 1;
			BlkFunc2ReferDieCheck(ulY, ulX);
		}
	}


	if (eAction == WAF_CDieSelectionAlgorithm::ALIGN)
	{
		if (DieIsSpecialRefGrade(ulY, ulX))				//v3.24T1
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"] = 1;		//Tell WPR to align this PICK die with Ref-Die pattern
		}
	}

	m_stCurrentDie.lX = (LONG)ulX;
	m_stCurrentDie.lY = (LONG)ulY;
	m_stCurrentDie.ucGrade = ucGrade;


	if( m_WaferMapWrapper.GetDieState(GetCurrentRow(), GetCurrentCol())== WT_MAP_DIESTATE_SKIP_PREDICTED )
	{
		eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		szMsg.Format("Get Posn %d,%d set to align because it is skipped", ulY, ulX);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	}

	m_stCurrentDie.eAction = eAction;

	// Used in waferPr and pass to IM Vision for logging
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"]		= ulY;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"]		= ulX;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"]	= ucGrade;
	(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"]	= eAction;

	if( m_lRescanStartRow==-1 && m_lRescanStartCol==-1 )
	{
		if( pSRInfo->IsWithinThisRegion(ulTgtRegion, ulY, ulX) )
		{
			LONG lWftX = 0, lWftY = 0;
			if( GetMapPhyPosn(ulY, ulX, lWftX, lWftY) )
			{
				m_lRescanStartRow	= ulY;
				m_lRescanStartCol	= ulX;
				m_lRescanStartWftX	= lWftX;
				m_lRescanStartWftY	= lWftY;
			}
		}
	}

	// Save data to bond record
	NextBondRecord();

	//v3.89	//MS100 LF die speedup for BH MOVE_T sequence
	if (CMS896AApp::m_lCycleSpeedMode >= 3)	
	{
		if (OpIsCurrentLFDie())
		{
			(*m_psmfSRam)["WaferTable"]["LFDie"] = TRUE;
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;
		}

		if (GetCurrDieEAct() == WAF_CDieSelectionAlgorithm::PICK)
		{
			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)FALSE;
		}
	}

	//v4.53A12
	OpGetWaferPos_CheckLFDie(ulY, ulX);

	/*
	unsigned long	ulNextX = 0, ulNextY = 0;

	//BLOCKPICK (NOT for regular ref die)
	if (IsBlkFunc1Enable())	//Block2
	{
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextRow"]	= 0;
		(*m_psmfSRam)["WaferPr"]["LookAround"]["NextCol"]	= 0;

		PeekMapNextDie(2, ulNextY, ulNextX, ucGrade, eAction, bRegnEndOfWafer);
		
		if (!bRegnEndOfWafer && (eAction == WAF_CDieSelectionAlgorithm::PICK))
		{
			(*m_psmfSRam)["WaferPr"]["LookAround"]["NextRow"]	= ulNextY;
			(*m_psmfSRam)["WaferPr"]["LookAround"]["NextCol"]	= ulNextX;
		}
	}

	BOOL bUseLookForward = IsUseLF();

	if (bUseLookForward == TRUE && IsBlkFuncEnable() == FALSE)
	{
		// Get one more die to determine the direction
		LONG lNextDieDirection = -1;
		PeekMapNextDie(2, ulNextY, ulNextX, ucGrade, eAction, bRegnEndOfWafer);

		// Remark: Next Die must be exactly the next one. 
		if (bRegnEndOfWafer == FALSE)
		{
			if (eAction == WAF_CDieSelectionAlgorithm::CHECK)
			{
				// prevent to use normal die record to check Ref-die in LF wnd
				lNextDieDirection = -1;	
			}
			else
			{
				lNextDieDirection = OpCalculateLFNextDieDirection(ulY, ulX, ulNextY, ulNextX);
			}
		}

		if (DieIsNoPick(ulNextY, ulNextX) == TRUE)
		{
			eAction = WAF_CDieSelectionAlgorithm::ALIGN;
		}
		(*m_psmfSRam)["WaferMap"]["ComingDieDirection"] = lNextDieDirection;
		(*m_psmfSRam)["WaferMap"]["LookForward_X"] = ulNextX;
		(*m_psmfSRam)["WaferMap"]["LookForward_Y"] = ulNextY;

		//For IM Vision logging of LookForward Srch-Die fcn
		(*m_psmfSRam)["WaferMap"]["NextDie"]["Row"]		= ulNextY;
		(*m_psmfSRam)["WaferMap"]["NextDie"]["Col"]		= ulNextX;
		(*m_psmfSRam)["WaferMap"]["NextDie"]["Grade"]	= ucGrade;
		(*m_psmfSRam)["WaferMap"]["NextDie"]["Action"]	= eAction;

		if (lNextDieDirection != -1)
		{
			m_lLookForwardCounter++;	//v3.55		//For statistics usage only
		}
	}
	else if (bUseLookForward)
	{
		m_lNoLookForwardCounter++;		//v3.55		//For statistics usage only
	}
	*/
	return nResult;
}

VOID CWaferTable::OpDebPitchErrorBackup()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CTime stTime = CTime::GetCurrentTime();
	CString szDate = stTime.Format("%Y%m%d");
	CString szMcNo = GetMachineNo() + "_";
	CString szTime = stTime.Format("_%H%M%S");
	CString szSourcePath = m_szPrescanLogPath;
	CString szNewDebPath = gszUSER_DIRECTORY + "\\DebResult\\" + szDate;
	CString szTargetPath = szNewDebPath + "\\" + szMcNo + m_szMapFileName + szTime;

	CreateDirectory(szNewDebPath, NULL);
	pApp->SearchAndRemoveFiles(gszUSER_DIRECTORY + "\\DebResult", 60);

	CString szOldSavePath	= szSourcePath + PRESCAN_MSLOG_INIT;
	CString szTgtSavePath	= szTargetPath + PRESCAN_MSLOG_INIT;
	CopyFile(szOldSavePath, szTgtSavePath, FALSE);

	szOldSavePath	= szSourcePath + PRESCAN_MSLOG_KEYC;
	szTgtSavePath	= szTargetPath + PRESCAN_MSLOG_KEYC;
	CopyFile(szOldSavePath, szTgtSavePath, FALSE);

	szOldSavePath	= szSourcePath + PRESCAN_MSLOG_XY;
	szTgtSavePath	= szTargetPath + PRESCAN_MSLOG_XY;
	CopyFile(szOldSavePath, szTgtSavePath, FALSE);

	szTgtSavePath	= szTargetPath + "_screen.jpg";
	PrintScreen(szTgtSavePath);
}

VOID CWaferTable::OpBackupLogFiles()	//	close log files too.
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szSourcePath = m_szPrescanLogPath;
	CString szOldSavePath, szTgtSavePath, szTargetPath;

	szTargetPath.Format("%s_%07lu", szSourcePath, GetNewPickCount());

	szOldSavePath = gszUSER_DIRECTORY + WT_GET_IDX_LOG;
	CMSLogFileUtility::Instance()->WT_GetIdxLogClose();
	if( pUtl->GetPrescanRegionMode() )
	{
		szOldSavePath = szSourcePath + WT_PRESCAN_GET_IDX_LOG;
	}
	szTgtSavePath	= szTargetPath + WT_PRESCAN_GET_IDX_LOG;
	CopyFile(szOldSavePath, szTgtSavePath, FALSE);
	DeleteFile(szOldSavePath);

	CMSLogFileUtility::Instance()->WT_GetDieLogClose();
	szOldSavePath  = gszUSER_DIRECTORY + WT_GET_DIE_LOG;
	if( pUtl->GetPrescanRegionMode() || pApp->GetCustomerName()==CTM_SEMITEK )
	{
		szOldSavePath = szSourcePath + WT_PRESCAN_GET_DIE_LOG;
	}
	szTgtSavePath	= szTargetPath + WT_PRESCAN_GET_DIE_LOG;
	CopyFile(szOldSavePath, szTgtSavePath, FALSE);
	DeleteFile(szOldSavePath);
}

BOOL CWaferTable::OpMS60NGPickPrEmptyDiePos()
{
	if (!CMS896AStn::m_bMS60NGPick)
	{
		return TRUE;
	}

	if (IsBurnIn())		//v4.59A4
	{
		return TRUE;
	}

	if (OpIsCurrentLFDie())
	{
		//No need to perform PR EMPTY Hole if current cycle is LF -> use LookBackward mode instead
		return TRUE;
	}

	//This parameter is set at BH's PICK_DELAY state to make sure 1st die is picked
	BOOL bFirstPickDie = (BOOL)(LONG) ((*m_psmfSRam)["BondHead"]["First Pick Die"]);	
	if (m_bFirst || m_bJustAlign || !bFirstPickDie)
	{
		return TRUE;
	}

	LONG lMapRow = m_stLastDie.lY;
	LONG lMapCol = m_stLastDie.lX;
	LONG lUserRow=0, lUserCol=0;
	ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);
	(*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"]			= lUserRow;
	(*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"]			= lUserCol;

	Sleep(10);		//v4.54A10

	//Ask Wafer PR task to search die at PICKED position for EMPTY die checking
	SetWPREmptyCheckDone(FALSE);
	SetWTReadyForWPREmptyCheck(TRUE);	

	CString szMsg;
	szMsg = "OpMS60NGPickPrEmptyDiePos";
//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	m_bMS60NGPickWaitCheckEmptyDie = TRUE;
	return TRUE;
}

BOOL CWaferTable::OpMS60CheckPrEmptyDieResult()
{
	if (m_bMS60NGPickWaitCheckEmptyDie)
	{
		SetWPREmptyCheckDone(FALSE);

		m_bMS60NGPickWaitCheckEmptyDie = FALSE;

		BOOL bIsEmpty = (BOOL)(LONG) ((*m_psmfSRam)["WaferPr"]["NGPick"]["IsEmpty"]);

		CString szMsg;
		szMsg.Format("OpMS60CheckPrEmptyDieResult - EMPTY = %d", bIsEmpty);
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		if (!bIsEmpty)
		{
			szMsg = _T("WT: Die is found at EMPTY die position (Normal PR)");

//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Back(szMsg, "NGPick Empty Die Check");
			return FALSE;
		}
	}

	return TRUE;
}



// here to change PR defect or BH missing die by die state in map
INT CWaferTable::OpUpdateMapViaPR()
{
	if( m_bChangeGrade4PrNG==FALSE )	//	NICHIA_TESTING
	{
		return 0;
	}

	m_WaferMapWrapper.StopMap();
	BOOL bRefresh = FALSE;
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	for(ULONG ulRow = (unsigned long)m_lScannedMapMinRow; ulRow <= (unsigned long)m_lScannedMapMaxRow; ulRow++)
	{
		for(ULONG ulCol = (unsigned long)m_lScannedMapMinCol; ulCol <= (unsigned long)m_lScannedMapMaxCol; ulCol++)
		{
			ULONG ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
			UCHAR ucDieGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ulDieState > WT_MAP_DIESTATE_MISSING )
			{
				ulDieState = WT_MAP_DS_BH_MISSING;
			}
			UCHAR ucTgtGrade = (UCHAR)(180+ulDieState+ucOffset);

			if( ulDieState >= WT_MAP_DS_PR_DEFECT && ulDieState <= WT_MAP_DS_BH_MISSING )
			{
			//	LONG lUserRow = 0, lUserCol = 0;
			//	ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
			//	CString szMsg;
			//	szMsg.Format("Update PR %d,%d,(%d,%d) sta %d grd %d", lUserRow, lUserCol, ulRow, ulCol, ulDieState, ucDieGrade);
			//	SetAlarmLog(szMsg);
				if (ucDieGrade == ucTgtGrade)
				{
					continue;
				}
				else
				{
					m_WaferMapWrapper.AddDie(ulRow, ulCol, ucTgtGrade);
			//		WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAction = WAF_CDieSelectionAlgorithm::ALIGN;
			//		m_WaferMapWrapper.Update(ulRow, ulCol, ucTgtGrade, eUpAction, FALSE, WT_MAP_DS_DEFAULT);
			//		szMsg.Format("Change Gade %d to %d", ucDieGrade, ucTgtGrade);
			//		SetAlarmLog(szMsg);
			//		m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucTgtGrade);
			//		bRefresh = TRUE;
			//		eUpAction = WAF_CDieSelectionAlgorithm::DEFECTIVE;
			//		switch( ulDieState )
			//		{
			//		case WT_MAP_DS_PR_INK:
			//			eUpAction = WAF_CDieSelectionAlgorithm::INKED;
			//			break;
			//		case WT_MAP_DS_PR_EMPTY:
			//		case WT_MAP_DS_BH_MISSING:
			//			eUpAction = WAF_CDieSelectionAlgorithm::MISSING;
			//			break;
			//		default:
			//			break;
			//		}
			//		m_WaferMapWrapper.Update(ulRow, ulCol, ucTgtGrade, eUpAction, FALSE, ulDieState);
				}
			}
		}
	}

	if( bRefresh )
	{
		m_WaferMapWrapper.ResetStatistics();
		m_WaferMapWrapper.Redraw();
	}
	m_WaferMapWrapper.StartMap();

	return 1;
}



BOOL CWaferTable::IsNGBlock(const UCHAR ucGrade)
{
	UCHAR ucBlockGrade = ucGrade - m_WaferMapWrapper.GetGradeOffset();
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		return pBondPr->IsNGBlock(ucBlockGrade);
	}

	return FALSE;
}


BOOL CWaferTable::IsNGBlock(const LONG lRow, const LONG lCol)
{
	UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lRow, lCol);
	return IsNGBlock(ucGrade);
}

BOOL CWaferTable::OpSetGradeToEquip(LONG lGrade)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (m_lLastGradeSetToEquip != lGrade)
	{
		pApp->m_eqMachine.SetBondingGrade(lGrade);
		pApp->m_eqMachine2.SetBondingGrade(lGrade);
		m_lLastGradeSetToEquip = lGrade;
		DisplaySequence("WT - Set Grade To Equip");
	}
	return TRUE;
}
