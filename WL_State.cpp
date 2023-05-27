/////////////////////////////////////////////////////////////////
// BH_State.cpp : Operation State of the CBondHead class
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
#include "WaferLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CWaferLoader::Operation()
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
			
			CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
			//if(pApp->GetCustomerName() == "General") // for test
			if(pApp->GetCustomerName() == "OSRAM" && IsSecsGemInit())
			{
				UpdateWLMgzSensor();// v4.51D1 SecsGem
			}

			if (m_bUpdateProfile)
			{
				UpdateProfile();
			}

			if (m_bUpdatePosition)
			{
				UpdatePosition();
			}

			if (m_bStartWaferBarcodeTest)
			{
				WaferBarcodeMoveScanTest();
			}

			if (m_bContLoadUnloedStart)		//v4.45T1
			{
				ContLoadUnloadTest();
			}

			BOOL bOsramWaferUnload = FALSE;
			BOOL bOsramWaferEnd    = FALSE;
			BOOL bEnableOsramBinMixMap = FALSE;
			bOsramWaferEnd  = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["OsramWaferEnd"];
			bOsramWaferUnload = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Osram Wafer Unload"];
			bEnableOsramBinMixMap = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Enable Osram Bin Mix Map"];
			if ((bOsramWaferUnload ||  bOsramWaferEnd) && bEnableOsramBinMixMap)
			{	
				IPC_CServiceMessage svMsg;
				BOOL bInit = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bInit);
				ManualChangeFilmFrame(svMsg);
				//AfxMessageBox("Osram Auto Unload Wafer");
				bOsramWaferUnload = FALSE;
				bOsramWaferEnd    = FALSE;
				(*m_psmfSRam)["BinOutputFile"]["Osram Wafer Unload"]		= bOsramWaferUnload;
				(*m_psmfSRam)["WaferTable"]["OsramWaferEnd"]				= bOsramWaferEnd;
				(*m_psmfSRam)["WaferTable"]["OsramWaferEndOK"]				= TRUE;	
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


/////////////////////////////////////////////////////////////////
//	Operation State Functions
/////////////////////////////////////////////////////////////////

VOID CWaferLoader::PreStartOperation()
{
	m_qSubOperation = WAIT_LOADUNLOAD_Q;
	m_bFrameOperationInAutoMode = FALSE;

	CloseALFrontGateWithLock();
	SaveScanTimeEvent("  WFL: prestart into idle q");
	DisplaySequence("WL - prestart ==> Wait LoadUnload");
	Motion(FALSE);
	State(IDLE_Q);
}

VOID CWaferLoader::StopOperation()
{
	OpenALFrontGateWithLock();
	Motion(FALSE);
	State(IDLE_Q);
	Signal(gnSTATION_COMPLETE);
	LogCycleStopState("WL - Signal Complete");
}


/////////////////////////////////////////////////////////////////
//	ENd of WL_State.cpp
/////////////////////////////////////////////////////////////////

