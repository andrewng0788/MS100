/////////////////////////////////////////////////////////////////
// BL_State.cpp : State of BinLoaderStn
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, June 21, 2005
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
#include "BinLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CBinLoader::IdleOperation()
{
	switch (Command())
	{
		case glAMS_NULL_COMMAND:
			Sleep(10);
			break;

		case glAMS_DIAGNOSTIC_COMMAND:
			State(DIAGNOSTICS_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_INITIAL_COMMAND:
			State(SYSTEM_INITIAL_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_PRESTART_COMMAND:
			State(PRESTART_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_AUTO_COMMAND:
			State(AUTO_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_MANUAL_COMMAND:
			m_lManualAction	= m_lAction;
			State(MANUAL_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_DEMO_COMMAND:
			State(DEMO_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_CYCLE_COMMAND:
			State(CYCLE_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_STOP_COMMAND:
		case glAMS_RESET_COMMAND:
			Motion(FALSE);
			Command(glAMS_NULL_COMMAND);
			break;

		case glAMS_DEINITIAL_COMMAND:
			State(DE_INITIAL_Q);
			Command(glAMS_NULL_COMMAND);
			break;

		default:
			Command(glAMS_NULL_COMMAND);
			State(IDLE_Q);
			Result(gnNOTOK);
			SetAlert(IDS_SYS_INVALID_COMD);
			break;
	}
}

/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CBinLoader::Operation()
{

	switch(State())
	{
		case IDLE_Q:
			IdleOperation();
			break;

		case DIAGNOSTICS_Q:
			DiagOperation();
			break;

		case SYSTEM_INITIAL_Q:
			InitOperation();
			break;

		case PRESTART_Q:
			PreStartOperation();
			break;

		case AUTO_Q:
			AutoOperation();
			break;

		case DEMO_Q:
			DemoOperation();
			break;

		case MANUAL_Q:
			ManualOperation();
			break;

		case ERROR_Q:
			ErrorOperation();
			break;

		case STOPPING_Q:
			StopOperation();
			break;

		case DE_INITIAL_Q:
			DeInitialOperation();
			break;

		default:
			State(IDLE_Q);
			Motion(FALSE);
			break;
	}


	if ((State() == IDLE_Q) || (State() == UN_INITIALIZE_Q))
	{
		try
		{
			if (m_bUpdateOutput)
			{
				UpdateOutput();
			}

			if (m_bUpdateProfile)
			{
				UpdateProfile();
			}

			if (m_bUpdatePosition)
			{
				UpdatePosition();
			}

			if (m_bStartManualLoadUnloadTest)	//v3.71T1
			{
				BOOL bLoad = FALSE;
				if ((m_lTestCycle % 2) == 0)
					bLoad = TRUE;
				BOOL bStatus = ManualLoadUnloadFilmFrame(bLoad);
				if (!bStatus)
				{
					m_bStartManualLoadUnloadTest = FALSE;
				}
				m_lTestCycle++;
				Sleep(1000);
			}

			//v2.83T45
			//UNLOAD all bin frames when barcode read failure is triggered
			if (m_bBarcodeReadAbort)
			{
HmiMessage("Bin frames will be unloaded back to magazine....");
				//OpUnloadAllFrames();
				//AfxMessageBox("Andrew: UNLOAD", MB_SYSTEMMODAL);
				m_bBarcodeReadAbort = FALSE;
			}
			//4.49 //Unload bin to full when wafer end under resort mode.
			BOOL bNeedUnloadToFull = FALSE;
			bNeedUnloadToFull = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["ResortModeWaferEnd"];
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if (bNeedUnloadToFull && pApp->GetCustomerName() == "Semitek")
			{
				IPC_CServiceMessage svMsg;
				AutoUnloadFilmFrameToFull(svMsg);
				(*m_psmfSRam)["WaferTable"]["ResortModeWaferEnd"] = FALSE;
			}

			BOOL bBinMixNeedUnload = FALSE;
			bBinMixNeedUnload = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Osram Bin Mix Map Need Unload"];
			if (bBinMixNeedUnload)
			{
				IPC_CServiceMessage svMsg;
				BOOL bStatus = ManualUnloadFilmFrame(svMsg);
//AfxMessageBox("manual load film frame begin");
				ManualLoadFilmFrame(svMsg);
				bBinMixNeedUnload = FALSE;
				(*m_psmfSRam)["BinOutputFile"]["Osram Bin Mix Map Need Unload"] = bBinMixNeedUnload;
				(*m_psmfSRam)["BinOutputFile"]["Osram Bin Mix Map Need Clear Bin"] = TRUE;
			}

			//BOOL bSkipRowUnload = FALSE;
			//bSkipRowUnload		= (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["SkipRowWaferEndUnload"];
			//if (bSkipRowUnload)
			//{
			//	IPC_CServiceMessage svMsg;
			//	BOOL bStatus = ManualUnloadFilmFrame(svMsg);
			//	ManualLoadFilmFrame(svMsg);
			//	(*m_psmfSRam)["BinOutputFile"]["SkipRowWaferEndUnload"] = FALSE;
			//}

			//v4.42T17	//Cree HuiZhou		//v4.52A14	//v4.53A6
			if (m_bFrameToBeUnloadAtIdleState == TRUE)
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->State() == IDLE_Q)
				{
					if ( (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	||		//v4.53A11
						 pApp->m_bMSAutoLineMode == 1)									//v4.57A15
					{
						OpUnloadFrameOnTable();		
						m_bFrameToBeUnloadAtIdleState = FALSE;
					}
				}
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}

		Sleep(10);
	}
	else if (State() == ERROR_Q)
	{
		Sleep(10);
	}
}


VOID CBinLoader::InitOperation()
{
	m_nLastError	= gnOK;

	try
	{
		m_nLastError = OpInitialize();

		if (m_nLastError != gnOK)
		{
			if (!IsAbort())
			{
				SetAlert(m_nLastError);
			}

			Result(gnNOTOK);
		}

		DisplaySequence("BL - Initialize Operation Completed");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		NeedReset(TRUE);
		Result(gnNOTOK);
	}

	Motion(FALSE);
	State(IDLE_Q);
}

VOID CBinLoader::PreStartOperation()
{
	m_nLastError	= gnOK;

	try
	{
		m_szLoadReadBarcode.Empty();
		m_nLastError = OpPreStart();

#ifdef NU_MOTION
		if (m_bEnableBLMotionLog)
		{
			LogAxisPerformance3(BL_AXIS_UPPER, BL_AXIS_LOWER, BL_AXIS_Z, &m_stBLAxis_Upper, &m_stBLAxis_Lower, &m_stBLAxis_Z, TRUE);
		}
#endif
		if (m_nLastError == gnOK)
		{
			CloseALFrontGateWithLock();
			//if (!CloseALFrontGateWithLock())
			//{
			//	m_nLastError = gnNOTOK;
			//}
		}

		if (m_nLastError != gnOK)
		{
			if (!IsAbort())
			{
				SetAlert(m_nLastError);
			}

			Result(gnNOTOK);
		}

		DisplaySequence("BL - PreStartOperation Completed");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		NeedReset(TRUE);
		Result(gnNOTOK);
	}
	Motion(FALSE);
	State(IDLE_Q);
}

VOID CBinLoader::CycleOperation()
{
	if (Command() == glAMS_STOP_COMMAND)
	{
		State(STOPPING_Q);
	}
}

VOID CBinLoader::StopOperation()
{
	if (m_bStopAllMotion == FALSE)
	{
	#ifdef NU_MOTION
		if (m_bEnableBLMotionLog)
		{
			LogAxisPerformance3(BL_AXIS_UPPER, BL_AXIS_LOWER, BL_AXIS_Z, &m_stBLAxis_Upper, &m_stBLAxis_Lower, &m_stBLAxis_Z, FALSE);
		}
	#endif

		if (m_bCoverSensorAbort)	//v3.60
		{
			SetAlert_Red_Yellow(IDS_BL_ELEVATOR_COVER_OPEN);
		}
	}

	m_qSubOperation		= 0;
	m_szDBPreScanBCName = "";	//pllm

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (m_lBinLoaderConfig == BL_CONFIG_DUAL_DL)		//v4.37T3
	{
		//Make sure all levels are down and level UP gripper level for safety reason in MS109
		SetGripper2Level(FALSE);
		SetGripperLevel(FALSE);
		SetFrameLevel2(FALSE);
		SetFrameLevel(FALSE);
	}

	BOOL bWaferEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferEnd"];

	if ( bWaferEnd && 
		(IsWLManualMode()						|| 
		(pApp->GetCustomerName() == "NanoJoin")	||
		(pApp->GetCustomerName() == "Finisar")	||		//v4.58A1	//Finisar
		pApp->m_bMSAutoLineMode	== 1			||		//v4.57A15
		(pApp->GetCustomerName() == "Cree")) )			//v4.40T7	//v4.52A14
	{
		LogCycleStopState("BL UnloadFrame at WAFEREND is enabled");
		CMSLogFileUtility::Instance()->MS_LogOperation("Cree: BL UnloadFrame at WAFEREND is enabled");
		m_bFrameToBeUnloadAtIdleState = TRUE;
	}

	if ( (pApp->GetCustomerName() == "Lumileds" || pApp->GetCustomerName()=="OSRAM") && (m_lBinLoaderConfig == BL_CONFIG_DUAL_DL) )		//v4.35T2	//PLLM MS109
	{
		if ( !IsElevatorCoverOpen() && !IsFrameOutOfMgz() && IsMagazineSafeToMove())
		{
			Z_MoveTo(m_lReadyLvlZ);
		}
	}

	//v4.42T4
	//if ( (CMS896AApp::m_lBinElevatorCoverLock == TRUE) && (m_bEnableBinElevatorCoverLock == TRUE) )
	if ( (m_bEnableBinElevatorCoverLock == TRUE) )
	{
		SetBinElevatorCoverLock(FALSE);
	}
	if (m_bEnableSideCoverLock == TRUE)
	{
		SetSideCoverLock(FALSE);
	}

	OpenALFrontGateWithLock();
	State(IDLE_Q);
	Motion(FALSE);
	Signal(gnSTATION_COMPLETE);
	LogCycleStopState("BL - Signal Complete");
}

