/////////////////////////////////////////////////////////////////
// WPR_CycleState.cpp : Cycle State of WaferPrStn
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Saturday, December 4, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "CycleState.h"
#include "WPR_Log.h"
#include "GallerySearchController.h"
#include "LastScanPosition.h"
#include "WAF_CPhysicalInformation.h"
#include "FileUtil.h"
#include "PrescanInfo.h"
#include "WT_SubRegion.h"
#include "PRFailureCaseLog.h"
#include "WaferMap.h"
#include "PrZoomSensor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CString g_aszWPRState[20] =
{	"Wait WT Stable",	// WAIT_WT_STABLE_Q
	"Grab Image",		// GRAB_IMAGE_Q
	"Wait Grab Done",	// WAIT_GRAB_DONE_Q
	"Inspection",		// INSPECTION_Q
	"LookForward",		// LOOKFORWARD_Q
	"LookForward IM RPY3",	// LOOKFORWARD_Q Wait RPY3 for IM
// prescan relative code	B
	"Prescan Wait WT Stable",	//	PRESCAN_WAIT_WT_STABLE_Q
	"Prescan Grab Image",		//	PRESCAN_GRAB_Q,
	"Prescan Wait Pr Ready",	//	PRESCAN_WAIT_PR_READY_Q,
	"Prescan Wait Map Done",	//	PRESCAN_WAIT_MAP_DONE_Q
	"Prescan Wait Bar Done",	//	PRESCAN_WAIT_BAR_DONE_Q
// prescan relative code	E
	"House Keeping"		// HOUSE_KEEPING_Q
};

VOID CWaferPr::AutoOperation()
{
	RunOperation();
}

VOID CWaferPr::DemoOperation()
{
	RunOperation();
}

LONG CWaferPr::GetDiePROffsetX() const
{
	return (LONG)(*m_psmfSRam)["WaferPr"]["Compensate"]["X"];
}

LONG CWaferPr::GetDiePROffsetY() const
{
	return (LONG)(*m_psmfSRam)["WaferPr"]["Compensate"]["Y"];
}

VOID CWaferPr::SetDiePROffsetResult(const LONG lXOffset, const LONG lYOffset, const DOUBLE dTOffset, const BOOL bRotate)
{
	(*m_psmfSRam)["WaferPr"]["Compensate"]["X"]			= lXOffset;
	(*m_psmfSRam)["WaferPr"]["Compensate"]["Y"]			= lYOffset;
	(*m_psmfSRam)["WaferPr"]["Compensate"]["Theta"]		= dTOffset;
	(*m_psmfSRam)["WaferPr"]["Compensate"]["Rotate"]	= bRotate;
}


VOID CWaferPr::SetDiePRResult(const LONG lXOffset, const LONG lYOffset, const DOUBLE dTOffset, const BOOL bRotate, const LONG lDieResult)
{
	SetDiePROffsetResult(lXOffset, lYOffset, dTOffset, bRotate);
	(*m_psmfSRam)["WaferPr"]["DieResult"]				= lDieResult;
}


VOID CWaferPr::RunOperation()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((Command() == glSTOP_COMMAND) &&
		(Action() == glABORT_STOP))
	{
		DisplaySequence("WPR - STOP");
		State(STOPPING_Q);
		return ;
	}

	if (m_bStopAllMotion == TRUE)
	{
		m_qSubOperation = HOUSE_KEEPING_Q;
	}

	if( m_qSubOperation==PRESCAN_WAIT_WT_STABLE_Q	|| 
		m_qSubOperation==PRESCAN_GRAB_Q	|| 
		m_qSubOperation==PRESCAN_WAIT_PR_READY_Q	|| 
		m_qSubOperation==PRESCAN_WAIT_MAP_DONE_Q	|| 
		m_qSubOperation==PRESCAN_WAIT_BAR_DONE_Q	)
	{
		return RunOperationWpr_Scan();
	}

	CString szMsg;
	BOOL bAutoDieResult = FALSE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"] && IsUseLF();	
	BOOL bRefDieCheck = ConfirmRefDieCheck();
	BOOL bSpecialGradeCheck =  ConfirmSpecialgradeRefDieCheck();
	if (bRefDieCheck || bSpecialGradeCheck)		//v3.24T1
	{
		bLFNoComp = FALSE;
	}

	try
	{
		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case WAIT_WT_STABLE_Q:
			DisplaySequence("WPR - wait WT stable q");

			if ( m_bSetAutoBondScreen == TRUE)
			{
				AutoBondScreen(TRUE);
				m_bSetAutoBondScreen = FALSE;
			}

			if( IsAutoRescanWafer() && m_lRescanRunStage==1 )	
			{	//	auto rescan sorting wafer	;	Wait WT stable to prescan
				SaveScanTimeEvent("WPR: rescan into auto cycle");
				m_dwaRsnBaseRow.RemoveAll();
				m_dwaRsnBaseCol.RemoveAll();
				m_dwaRsnBaseWfX.RemoveAll();
				m_dwaRsnBaseWfY.RemoveAll();
				OpPrescanInit(GetPrescanPrID());	// download when press start button
				SetAllPrescanDone(FALSE);
				m_bWprInAutoSortingMode	= FALSE;
				m_lRescanRunStage		= 2;	//	MS rescan
				WSClearScanRemainDieList();
				m_qSubOperation			= PRESCAN_WAIT_WT_STABLE_Q;
				if( IsEnableZoom() )
				{
					SaveScanTimeEvent("WPR: rescan, switch to scan zoom mode");
					WPR_ToggleZoom(TRUE, TRUE, 7);
				}
				break;
			}	//	auto rescan sorting wafer	;	Wait WT stable to prescan

			if (IsBHStopped() || (OpIsMaxSkipCount() == TRUE) )
			{
				DisplaySequence("WPR - wait WT stable q ==> HOUSE_KEEPING_Q");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitWTStable())
			{
				//TakeTime(LC4);		//v4.48A3

				SetWTStable(FALSE);
				DisplaySequence("WPR - SetWTStable(FALSE)");

				if( m_fHardware == FALSE || IsES101() || IsES201() )
				{
					TakeTime(WPR1);			// Take Time
					SetPRStart(TRUE);			//Allow BH-T to PICK
					(*m_psmfSRam)["WaferPr"]["RPY1"]					= GetTime();
					if( m_fHardware == FALSE )
						Sleep(10);
					SetPRLatched(TRUE);
					TakeTime(WPR2);		// Take Time
					if( m_fHardware == FALSE )
						Sleep(30);
					SetDiePRResult(0, 0, 0, FALSE, WT_MAP_DIESTATE_PICK);
					TakeTime(WPR3);		// Take Time
					DisplaySequence("WPR3 1");
					SetBadDie(FALSE, "2");
					SetBadDieForT(FALSE);
					SetDieReady(TRUE);
					SetLFReady();
					if( m_fHardware == FALSE )
						Sleep(20);
					TakeTime(WPR4);
					DisplaySequence("WPR4 1");
				}
				else
				{
					m_qSubOperation = GRAB_IMAGE_Q;
					if( m_bAutoBondMode == FALSE )
					{
						AutoBondScreen(TRUE);
					}
					else if( GetNewPickCount()==0 )
					{
						AutoBondScreen(TRUE);
						m_bSetAutoBondScreen = FALSE;
					}

					if( m_bKeepShowBondInAuto && m_bCurrentCamera==WPR_CAM_WAFER )
					{
						if( (GetNewPickCount()>m_ulAutoSwitchPrCount) &&
							(GetNewPickCount()>(m_ulAutoSwitchPrCount+10)) )
						{
							m_ulAutoSwitchPrCount = GetNewPickCount();
#ifdef NU_MOTION
							m_bCurrentCamera = WPR_CAM_BOND;
							m_ulPrWindowHeight		= 480;
							m_ulPrWindowWidth		= 512;
							m_bCPItemMapToTop		= FALSE;
							m_bPrAoiSwitchCamera = TRUE;
							DisplaySequence("WPR - WT stabel, before grab, swith PR to Bond Side.");
#endif
						}
					}
					else
					{
						m_ulAutoSwitchPrCount = GetNewPickCount();
					}
				}

				OpUpdateDieIndex();
			}
			else if (WaitBhTReadyForWPR())		//v3.34		//For WaferPr collet-hole measurement only
			{
				PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
				m_pPrGeneral->InitDisplayVideo(FALSE, ubSID, ubRID, PR_PURPOSE_CAMERA_1);
				SetBhTReadyForWPR(FALSE);
				if (IsEnableBHMark())
				{
					OpAutoSearchBHMark();
				}
				else
				{
					//Search Collet hole with Epoxy Pattern
					OpAutoSearchColletHoleWithEpoxyPattern();
				}
				SetSearchMarkDone(TRUE);
				//m_pPrGeneral->InitDisplayVideo(TRUE, ubSID, ubRID, PR_PURPOSE_CAMERA_1);
				break;
			}
			else if (WaitBhReadyForWPRHwTrigger())
			{
				DisplaySequence("WPR - WaitBhReadyForWPRHwTrigger() == TRUE");
				SetBhReadyForWPRHwTrigger(FALSE);
				BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];
				if (m_bUseHWTrigger && bLFNoComp && bMS60LFCycle && !IsBurnIn())
				{
					PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
					PR_UWORD usSearchResult = GrabShareImage(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID);
					if (usSearchResult != PR_ERR_NOERR)
					{
						szMsg.Format("Fail to GrabShareImage = %x", usSearchResult);
						DisplaySequence(szMsg);
					}

					m_unHWGrabState = 1;	//PreGrab Done
					DisplaySequence("WPR - WaitBhReadyForWPRHwTrigger() GrabShareImage Done");
				}
				break;
			}
			else if (WaitSetPRTesting())
			{
				SetPRTesting(FALSE);
				DisplaySequence("StartUserSearch2");
				OpSetPRTesting();
				break;
			}
			else if (WaitWTReadyForWPREmptyCheck())		//v4.54A5	//MS60 new NGPick EMPTY die check fcn
			{
//TakeTime(LC3); 2018.3.5
				SetWTReadyForWPREmptyCheck(FALSE);
				OpAutoSearchEmptyDie();
//TakeTime(LC4); 2018.3.5
				break;
			}
			break;

		case GRAB_IMAGE_Q:
			DisplaySequence("WPR - Grab Image");

			if (!pApp->m_bBurnInGrabImage && IsBurnIn())
			{
				DisplaySequence("WPR - IsBurnIn(), m_bBurnInGrabImage = FALSE");
				Sleep(1);

				SetPRStart(TRUE);			//Allow BH-T to PICK
				SetPRLatched(TRUE);			//Allow BT to MOVE
				SetBadDie(FALSE, "5");
				SetBadDieForT(FALSE);
				SetDieReady(TRUE);
				SetWPRGrabImageReady(TRUE, "1");
				
				m_qSubOperation = WAIT_WT_STABLE_Q;	
				SetLFReady(); //MS50 only

				break;
			}

			TakeTime(WPR0);

			if (bLFNoComp)			//v2.96T3
			{
				CMSLogFileUtility::Instance()->MS60_Log("bLFNoComp = 1, new Lookhead");				
				CMSLogFileUtility::Instance()->BPR_Arm1Log("WPR: LFNoComp in GRAB_IMAGE_Q");	//v4.49A3
				DisplaySequence("WPR - LF no comp -> wait grab done");
//				m_qSubOperation = WAIT_GRAB_DONE_Q;	
				m_qSubOperation = LOOKFORWARD_Q;
				break;
			}

			CMSLogFileUtility::Instance()->MS60_Log("bLFNoComp = 0, Current Die");
			if ( AutoSearchDie(TRUE) == TRUE )			//v2.96T2
			{
				TakeTime(WPR1);			// Take Time
				m_qSubOperation = WAIT_GRAB_DONE_Q;	
			}
			else
			{
				HmiMessage_Red_Yellow("WPR AutoSearchDie Search DieCmd error!");		//v4.44A1	//Semitek
				SetErrorMessage("WPR AutoS earchDie cmd fails.");
				m_qSubOperation = HOUSE_KEEPING_Q;	
			}
			break;

		case WAIT_GRAB_DONE_Q:
			DisplaySequence("WPR - Wait Grab Done");

			if (bLFNoComp)			//v2.96T3
			{
				m_qSubOperation = INSPECTION_Q;	
				DisplaySequence("WPR - LF No Comp -> Inspection q");
				break;
			}

			if ( AutoGrabDone() == TRUE )
			{
				// In IM config should wait for RPY1 before Move T to PICK
				// in order to avoid wafer camera blocking problem due to PR logging
				SetPRStart(TRUE);			//Allow BH-T to PICK
				(*m_psmfSRam)["WaferPr"]["RPY1"] = GetTime();		//v3.61
				SetPRLatched(TRUE);

				//andrewng //2020-0622
				//if (!IsUseLF() || m_lLetBPRGrabFirstImage)
				//{
				SetWPRGrabImageReady(TRUE, "2");
				//}

				TakeTime(WPR5);		// Take Time
				//SetBadDie(FALSE, "2");
				//SetBadDieForT(FALSE);
				//SetDieReady(TRUE);
				m_qSubOperation = INSPECTION_Q;	
			}
			else
			{
				m_qSubOperation = HOUSE_KEEPING_Q;	
			}
			break;

		case INSPECTION_Q:
			DisplaySequence("WPR - Inspection");

			if (bLFNoComp)			//v2.96T3
			{
CMSLogFileUtility::Instance()->BPR_Arm1Log("WPR: LFNoComp in INSPECTION_Q");	//v4.49A3

				DisplaySequence("WPR - LF No Comp -> Look forward q");
				m_qSubOperation = LOOKFORWARD_Q;	
				break;
			}

			//v2.83T6
			bAutoDieResult = FALSE;
			(*m_psmfSRam)["WaferMap"]["NgPick"]["PickLeft"] = 0;

			bAutoDieResult = AutoDieResult(TRUE);

			DOUBLE dThetaLog;
			dThetaLog = (*m_psmfSRam)["WaferPr"]["Compensate"]["ThetaLog"];
			szMsg.Format("WPR NO LF Degree,%f", dThetaLog);
			CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szMsg);

			if( bAutoDieResult==FALSE )
			{
				bAutoDieResult = OpSearchMultipleDie();
				if ( bAutoDieResult==FALSE )
				{
					bAutoDieResult = OpConfirmSearch();
				}
#ifdef NU_MOTION
				if ( bAutoDieResult==FALSE )
				{
					bAutoDieResult = OpAutoResearchDie1();		//v4.41T5
				}
#endif
			}

			if( IsUseLF() && (ConfirmRefDieCheck() || ConfirmSpecialgradeRefDieCheck()) )
			{
				DisplaySequence("WPR - research normal die after refer for LF");
				CMSLogFileUtility::Instance()->WT_GetIdxLog("WPR srch research normal die after refer for LF");
				OpResearchNormalDieAtReferPosn();	// after refer check, search normal die for next LF
			}

			if( bAutoDieResult )
			{
				TakeTime(WPR6);		// Take Time
				//DisplaySequence("WPR3 2");
				SetBadDie(FALSE, "2");
				SetBadDieForT(FALSE);
				SetDieReady(TRUE);
			}
			else
			{
				(*m_psmfSRam)["WaferMap"]["NgPick"]["PickLeft"] = 0;
				if (ConfirmSpecialgradeRefDieCheck() == TRUE)
				{
					(*m_psmfSRam)["WaferPr"]["RefGradeCheck"]["Result"]	= FALSE;
				}

CString szMsg;
szMsg.Format("WPR: SetBadDie to TRUE - %d", bLFNoComp); 
CMSLogFileUtility::Instance()->BPR_Arm1Log(szMsg);	//v4.49A3	

				TakeTime(WPR3);		// Take Time
				DisplaySequence("WPR3 3");
				SetDieReady(FALSE);
				SetBadDie(TRUE, "3");
				SetBadDieForT(TRUE);
				DisplaySequence("WPR - inspect bad die");
			}

			m_qSubOperation = LOOKFORWARD_Q;	
			break;

		case LOOKFORWARD_Q:
			DisplaySequence("WPR - Look Forward");

			// Check whether wafer table want to use Look-Forward
			if (IsUseLF() == TRUE)
			{
				LONG lMS50Cycle		= (*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];
				LONG lDirection		= (*m_psmfSRam)["WaferMap"]["NextDieDirection"];
				LONG lNNDirection	= (*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"];
				szMsg.Format("WPR - Lookforward = %d, %d (MS50 Cycle = %d)", lDirection, lNNDirection, lMS50Cycle);
				DisplaySequence(szMsg);
				//DisplaySequence("WPR - Lookforward");

				//BLOCKPICK
				if (IsBlkFunc1Enable())
				{
					AutoLookAroundDie();
				}
				else
				{
					BOOL bAutoLookForwardDieResult = FALSE;
					BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];
					if (m_bUseHWTrigger && bMS60LFCycle && !IsBurnIn())
					{
						bAutoLookForwardDieResult = AutoLookForwardDie_HwTrigger(TRUE);
						m_unHWGrabState = 0;
					}
					else
					{
						bAutoLookForwardDieResult = AutoLookForwardDie(TRUE);
					}

					DOUBLE dThetaLog;
					dThetaLog = (*m_psmfSRam)["WaferPr"]["Compensate"]["ThetaLog"];
					if (lMS50Cycle == 1)
					{
						//Next Next Die LF
						szMsg.Format("WPR Next Next Die LF Degree,%f", dThetaLog);
					}
					else if (lDirection != -1 && lNNDirection == -1)
					{
						//Next Die LF
						szMsg.Format("WPR Next Die LF Degree,%f", dThetaLog);
					}
					else
					{
						szMsg.Format("WPR No LF");
					}
					CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szMsg);

					if (bAutoLookForwardDieResult == FALSE)
					{
						DisplaySequence("WPR - AutoLookForwardDie(TRUE)==FALSE");
						m_qSubOperation = HOUSE_KEEPING_Q;	
						break;
					}
				}
			}	

			m_qSubOperation = WAIT_WT_STABLE_Q;	
			DisplaySequence("WPR - LF ==> Wait WT Stable");
			TakeTime(WPR4);
			DisplaySequence("WPR4 2");
			SetLFReady(); //MS50 only
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("WPR - House Keeping Q");
			SetAutoCycleLog(FALSE);
			if (GetHouseKeepingToken("WPRStn") == TRUE)
			{
				SetHouseKeepingToken("WPRStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}
			if (m_ulLogPRCaseInHQ > 0)
			{
				m_ulLogPRCaseInHQ = 0;
				OpAutoLogPRCase(TRUE,5);
			}
			if (m_bUseHWTrigger && (m_unHWGrabState > 0))	//andrewng 2020-0616
			{
				ReceiveHWTriRpyinHouseKeeping();
				m_unHWGrabState = 0;
			}
			if ((IsAutoRescanWafer() || IsPrescanEnable()) && pUtl->GetPrAbnormal() )	//	house keeping, dump log
			{
				szMsg = "PR abnormal error and stop, please copy files.";
				SaveScanTimeEvent(szMsg);
				SaveScanTimeToFile(GetNewPickCount());
				HmiMessage_Red_Back(szMsg, "PR error");
				SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);

//				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
//				UINT unDebug = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Asm special debug flag"), 0);
//				if( unDebug>0 )
//				{
//					PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
//					PR_DumpCommLog(ubSID, ubRID);
//				}

				//CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
				//if (pPRFailureCaseLog->IsEnableFailureCaselog())
				//{
					//CloseAlarm();
					//CString szMsg;
					//szMsg = "Log PR Failure Case...";
					//SetAlert_WarningMsg(IDS_MS_WARNING_LOG_PR_FAILURE_CASE, szMsg);
					//pPRFailureCaseLog->LogFailureCaseDumpFile(GetRunSenID(), GetRunRecID(), 200);
					//SetAlarmLamp_Green(FALSE, TRUE);
				//}
				OpAutoLogPRCase(FALSE,200);
				m_pPrescanPrCtrl->DeleteRemainImages();
				GetImageNumInGallery();
			}

			StopCycle("WPRStn");
			LogCycleStopState("WPR - stop completely");
			State(STOPPING_Q);

			if( m_bIsWaferEnded || IsOnlyRegionEnd() )
			{
				CString szMsg;
				szMsg.Format("WPR - wafer(region=%ld) sorting end, total %ld, picked %ld, rescan missing %ld",
					pUtl->GetPrescanRegionMode(), m_lPrescanSortingTotal, GetNewPickCount(), m_lRescanMissingTotal);
				SaveScanTimeEvent(szMsg);
				DOUBLE dRatio = 0.0;
				if( m_lPrescanSortingTotal!=0 )
				{
					dRatio = ((DOUBLE)m_lRescanMissingTotal)/((DOUBLE)m_lPrescanSortingTotal);
				}
				szMsg.Format("%s,%s,end,%ld,%ld,%ld,%ld,%f\n", GetMachineNo(), GetMapNameOnly(),
					 m_lPrescanSortingTotal, 0, 0, m_lRescanMissingTotal, dRatio);
				CString szFileName = gszUSER_DIR_MAPFILE_OUTPUT + "\\" + GetMachineNo() + "_RescanResult.csv";
				CMSLogFileUtility::Instance()->AppendLogWithTime(szFileName, szMsg);
			}

			SetHouseKeepingToken("WPRStn", TRUE);

// prescan relative code	B
			if( IsPrescanning() )
			{
				LONG lCounter = 0;
				while( 1 )
				{
					if( pUtl->GetPrAbnormal() )
					{
						break;
					}
					if( IsScanPrIdle() )
					{
						SaveScanTimeEvent("WPR - normal scan to resume PR");
						m_pPrescanPrCtrl->ResumePR();
						SaveScanTimeEvent("WPR - normal scan to get remain image number");
						GetImageNumInGallery();
						break;
					}
					else
					{
						Sleep(100);
						lCounter++;
					}
					if( lCounter>=500 )
					{
						SaveScanTimeEvent("WPR - normal scan PR halted");
						break;
					}
				}
				pUtl->SetPrescanMapIndexIdle( IsScanPrIdle() );
			}
			if( IsEnableZoom() )
			{
				SetZoomViewFixZoom(TRUE);
				WPR_ToggleZoom(FALSE, FALSE, 8);
			}
			SavePrData(FALSE);
// prescan relative code	E
			// Remark: Must after setting the STOPPING state
			SaveScanTimeEvent("WPR: after house keeping into stopping q");
			break;

		default:
			DisplaySequence("WPR - Unknown");
			m_nLastError = glINVALID_STATE;
			break;
		}

		if (m_nLastError != gnAMS_OK)
		{
			if (!IsAbort())
			{
				SetError(m_nLastError);
			}

			Result(gnNOTOK);
			State(ERROR_Q);
		}
		else
		{
			m_qPreviousSubOperation = m_qSubOperation;
		}

		if ( m_bHeapCheck == TRUE )
		{
			// Check the heap
			INT nHeapStatus = _heapchk();
			if ( nHeapStatus != _HEAPOK )
			{				
				CString szMsg;
				szMsg.Format("WPR - Heap corrupted (%d) before WPR [%s]", nHeapStatus, 
							g_aszWPRState[m_qSubOperation]);
				DisplayMessage(szMsg);

				LogCycleStopState(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		Result(gnNOTOK);
		State(ERROR_Q);
		NeedReset(TRUE);
		StopCycle("WPRStn");
		SetMotionCE(TRUE, "WPR RunOperation Stop");
	}
}

VOID CWaferPr::CycleOperation()
{
	if (Command() == glAMS_STOP_COMMAND)
	{
		State(STOPPING_Q);
	}
}


VOID CWaferPr::RunOperationWpr_Scan()
{
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bRescanGoOn = TRUE;

	try
	{
		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case PRESCAN_WAIT_WT_STABLE_Q:
			if( IsPrescanning() )
			{
				pUtl->SetPrescanMapIndexIdle( IsScanPrIdle() );
			}
			if( pUtl->GetPrescanAreaPickMode() )
			{
				if( m_lAreaPickStage>=2 && m_nPickListIndex>0 && m_bWprInAutoSortingMode==FALSE )
				{
					m_bWprInAutoSortingMode = TRUE;
					m_bSetAutoBondScreen = TRUE;
				}
				if( m_lAreaPickStage==1 && m_nPickListIndex>0 && m_bWprInAutoSortingMode==TRUE )
				{
					m_bWprInAutoSortingMode = FALSE;
					m_bSetAutoBondScreen = TRUE;
				}
			}
			if ( m_bSetAutoBondScreen == TRUE)
			{
				AutoBondScreen(TRUE);
				m_bSetAutoBondScreen = FALSE;
			}
			if (IsBHStopped() )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (WaitWTStable())
			{
				SetWTStable(FALSE);
				if( pUtl->GetPrescanAreaPickMode() && m_lAreaPickStage>=2 && 
					pUtl->GetPrescanMapIndexIdle() && m_bPrescanLastGrabbed==TRUE )
				{
					CString szTemp;
					m_lCurrSrchDieId = 1;
					if ( AutoSearchDie() == TRUE )
					{
						TakeTime(WPR1);
						if ( AutoGrabDone() == TRUE )
						{
							TakeTime(WPR2);
							AutoDieResult();
							TakeTime(WPR3);
							DisplaySequence("WPR3 4");
						}
						else
						{
							szTemp.Format("WPR area pick grab fail");
							CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
						}
					}
					else
					{
						szTemp.Format("WPR area pick search fail");
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
					}
					DisplaySequence("WPR - Wait WT Stable ==> grab die for area pick");
				}
				else
				{
					DisplaySequence("WPR - Wait WT Stable ==> prescan grab q");
					m_qSubOperation = PRESCAN_GRAB_Q;
					break;
				}
			}

			if( IsPrescanning() && IsPrescanEnded() && WaitAllPrescanDone()==FALSE )
			{
				pUtl->PrescanMoveLog("WPR - into wait map done q");
				DisplaySequence("WPR - wait Wait WT Stable ==> Wait Map Done q");
				m_dScanEndTime = GetTime();
				m_stScanEndCTime   = CTime::GetCurrentTime();
				m_qSubOperation = PRESCAN_WAIT_MAP_DONE_Q;
				GetImageNumInGallery();
				m_lScanIndexStopCount = m_lPrescanImageCount;
				CString szMsg;
				szMsg.Format("WPR: all WFT indexing and grab done, %d images left for PR treatment", m_lScanIndexStopCount);
				SaveScanTimeEvent(szMsg);
			}
			break;

		case PRESCAN_GRAB_Q:
			if (IsBHStopped() )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if( IsAutoRescanWafer() && pUtl->GetPrAbnormal() )	//	check during scanning. before grab next.
			{
				szMsg = "PR abnormal error during scanning.";
				HmiMessage_Red_Back(szMsg, "Alarm");
				SetErrorMessage(szMsg);
				m_qSubOperation = HOUSE_KEEPING_Q;	
				break;
			}
			//// in here, we determine if there is enough memory to grab more, if not, change to pick mode
			GetImageNumInGallery();
			m_lScanIndexStopCount = m_lPrescanImageCount;

			TakeData((TIME_ENUM)SCAN_WPR_IMG_LEFT, m_lPrescanImageCount);		// prescan_time WFT ready for next grab

			if (m_lPrescanImageCount >= m_lPrescanImageLimit)
			{
				Sleep(5);
				break;
			}

			if( IsWprWithAF() && (m_bAFGridSampling || m_bAFDynamicAdjust) )	//	427TX	4	auto cycle move
			{
				Z_Sync();
			}

			TakeTime((TIME_ENUM)SCAN_WFT_CT_44);	// prescan_time move to get CT
			StartTime(m_nPrescanIndexCounter);		//	SCAN_WPR_REC_NUM	// prescan_time to reset for next cycle
			m_nPrescanIndexCounter++;
			DisplaySequence("WPR - grab image ==> wait PR ready q");
			if ( AutoGrabShareImage() == TRUE )
			{
				m_qSubOperation = PRESCAN_WAIT_PR_READY_Q; 
			}
			else
			{
				SetErrorMessage("WPR Gallery Grab ImageCmd cmd fails.");
				m_qSubOperation = HOUSE_KEEPING_Q;	
			}

			if( IsWprWithAF() )	//	427TX	4
			{
				AF_ObtainScore_MoveToLevel();
			}
			break;

		case PRESCAN_WAIT_PR_READY_Q:
			if (IsBHStopped() )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if( IsScanPrReadyToGrab() )
			{
				DisplaySequence("WPR - wait PR buffer ready to grab ==> Wait WT Stable q");
				TakeTime((TIME_ENUM)SCAN_WPR_CTR_RDY);	// prescan, PR controller ready, then to check table stable and to grab next image.
				m_qSubOperation = PRESCAN_WAIT_WT_STABLE_Q;	
			}
			break;

		case PRESCAN_WAIT_MAP_DONE_Q:
			// check if all search threads are done
			if( IsBLInUse() && m_lBackLightZStatus==2 && m_bPrescanTwicePrDone && IsEnableFPC()==FALSE )
			{
				m_lTimeSlot[19]	= m_nPrescanIndexCounter;				//	19. num of frame grabbed.
				m_lTimeSlot[4] = (LONG)(GetTime() - m_dScanStartTime);	//	04.	scan grab finished used time.
				SaveScanTimeEvent("WPR: all grab done, BLZ DN");
				m_lBackLightZStatus = 3;
				m_dBackLightZDownTime = GetTime();
				BLZ_MoveTo(m_lBackLightElevatorStandByLevel, SFM_NOWAIT);
				SaveScanTimeEvent("WPR: all grab done, BLZ DN command called");
			}

			if ( IsScanPrIdle() )
			{
				m_dScanPrCleanTime = fabs(GetTime() - m_dScanEndTime)/1000.0;
				{
					m_lTimeSlot[5] = (LONG)(GetTime() - m_dScanEndTime);	//	05.	to find out all left frame result used time
					CString szPrMsg;
					if( IsAutoRescanWafer() )
						szPrMsg.Format("WPR: all PR result found, total die %d, begin rescan construct map", WSGetPrescanTotalDie());
					else
						szPrMsg.Format("WPR: all PR result found, total die %d, begin construct map", WSGetPrescanTotalDie());
					SaveScanTimeEvent(szPrMsg);
					m_bWprInAutoSortingMode = TRUE;
					StartLoadingAlert();	// stop/close by wafer table
					if( IsAutoRescanWafer() )
					{	//	auto rescan sorting wafer	prescan finish, into normal sorting
						bRescanGoOn = RescanConstructMap();
						if (IsPrescanEnable() && (pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()))
						{
							WM_CWaferMap::Instance()->GetRegionStatistics();
						}
						SaveScanTimeEvent("WPR: rescan Construct map complete");
						if( IsEnableZoom() )
						{
							SaveScanTimeEvent("WPR: all PR done, switch to sort zoom mode");
							WPR_ToggleZoom(FALSE, TRUE);
						}
					}	//	auto rescan sorting wafer	prescan finish, into normal sorting
					else
					{
						if( IsEnableZoom() )
						{
							SaveScanTimeEvent("WPR: all PR done, switch to sort zoom mode");
							WPR_ToggleZoom(FALSE, TRUE, 9);
							if( GetScnZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF && m_dWprZoomRoiShrink==100.0 )
							{
								Sleep(2000);
							}
						}
						ConstructPrescanMap();
						if (IsPrescanEnable() && (pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()))
						{
							WM_CWaferMap::Instance()->GetRegionStatistics();
						}
						SaveScanTimeEvent("WPR: Construct prescan map complete");
					}
					CloseLoadingAlert();	// start/open by wafer pr station
					if( m_bAOINgPickPartDie )
					{
						m_bSetAutoBondScreen = TRUE;
						m_bAutoBondMode	= FALSE;
					}
					m_qSubOperation = WAIT_WT_STABLE_Q;	
					DisplaySequence("WPR - scan all done ==> Wait WT Stable");
					SaveScanTimeEvent("WPR: scan all done ==> Wait WT Stable");
				}

				SetAllPrescanDone(TRUE);
			}
			else if( IsStitchMode() )
			{
				m_pPrescanPrCtrl->PartialResumePR();
			}

			Sleep(10);
			GetImageNumInGallery();
			m_dPreviousUpdateTime = GetTime();
			if( IsEnableZoom() && GetScnZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF && IsScanPrIdle()==FALSE && (m_dPreviousUpdateTime-m_dScanEndTime)>2*60*1000 )	//	
			{	//	m_lPrescanImageCount	m_lScanIndexStopCount
				szMsg = "PR abnormal after table index done, time out over 120 seconds";
				SaveScanTimeEvent(szMsg);
				SetErrorMessage(szMsg);
				m_qSubOperation = HOUSE_KEEPING_Q;
				pUtl->SetPrAbnormal(TRUE, szMsg);	//	get out all pr result time out
			}
			if( bRescanGoOn==FALSE )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				CString szMsg = "WPR - rescan all done error ==> House Keeping Q";
				DisplaySequence(szMsg);
				SaveScanTimeEvent(szMsg);
				m_bWaferAlignComplete	= TRUE;//2019.05.16 quit auto cycle
			}
			break;

		case PRESCAN_WAIT_BAR_DONE_Q:
			if( m_ucOcrBarMapStage==1 )
			{
				Sleep(1000);
				break;
			}
			if( m_ucOcrBarMapStage==2 )
			{
				ConfirmScanBarMap_Finisar();
				DisplaySequence("WPR - prescan wait bar confirm done ==> Wait WT stable q");
				SetAllPrescanDone(TRUE);
				m_qSubOperation = WAIT_WT_STABLE_Q;
				m_ucOcrBarMapStage = 0;
			}
			break;
// prescan relative code	E	//	426TX	1
		}

		if (m_nLastError != gnAMS_OK)
		{
			if (!IsAbort())
			{
				SetError(m_nLastError);
			}

			Result(gnNOTOK);
			State(ERROR_Q);
		}
		else
		{
			m_qPreviousSubOperation = m_qSubOperation;
		}

		if ( m_bHeapCheck == TRUE )
		{
			// Check the heap
			INT nHeapStatus = _heapchk();
			if ( nHeapStatus != _HEAPOK )
			{				
				CString szMsg;
				szMsg.Format("WPR - Heap corrupted (%d) before WPR [%s]", nHeapStatus, 
							g_aszWPRState[m_qSubOperation]);
				DisplayMessage(szMsg);

				LogCycleStopState(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		Result(gnNOTOK);
		State(ERROR_Q);
		NeedReset(TRUE);
		StopCycle("WPRStn");
		SetMotionCE(TRUE, "WPR RunOperationWpr_Scan Stop");
	}
}
