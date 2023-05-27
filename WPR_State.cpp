/////////////////////////////////////////////////////////////////
// WPR_State.cpp : State of WaferPrStn
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
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
#include "WPR_Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CWaferPr::IdleOperation()
{
	int i=0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	// Init PR 
#ifndef MS_DEBUG
	if ( PR_NotInit() )	// && m_fHardware)		//v4.19T1	//v4.46T12
#else
	if ( PR_NotInit() /*&& m_fHardware == TRUE*/ ) // Chris: For Debug Mode
#endif
	{
		switch (m_qSubOperation)
		{
		case DPR_INIT_Q:

			if (m_bAppInitialized)		//all stations in IDLE state before allowing PR init
			{
				m_qSubOperation = INIT_IM_Q;
			}
			else
			{
				Sleep(200);
			}
			break;

		case INIT_IM_Q:		//DEFAULT PR init sequence 

#ifdef ALLDIESORT
			//if (PR_InitDieMapSort())
			m_qSubOperation = INIT_COMPLETED_Q;
			break;
#else
			if (PR_Init())
			{	
				PR_InitMSSystemPara();
				m_bPRInit = TRUE;
				m_qSubOperation = INIT_COMPLETED_Q;

				// Remark: Not call ShowHmi() of MS896A since it will show the application window
				CWnd *pHMI = CWnd::FindWindow(NULL, _T("ASM Human Machine Interface"));	// Get the HMI handle
				if ( pHMI != NULL )
				{
					pHMI->SetForegroundWindow();
				}
			}
			else
			{
				break;
			}
			m_qSubOperation = INIT_COMPLETED_Q;
#endif
			break;
		}
	}
	else
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
}

VOID CWaferPr::Operation()
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

	if (State() == IDLE_Q)		//v2.83T2
	{
		UpdateOutput();

		if (m_bUpdateProfile)
		{
			UpdateProfile();
		}

		if (m_bUpdatePosition)
		{
			UpdatePosition();
		}

		if ( m_bSetAutoBondScreen == TRUE)
		{
			AutoBondScreen(FALSE);
			m_bSetAutoBondScreen = FALSE;
		}

		MotionTest();
	}
}

VOID CWaferPr::InitOperation()
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

		DisplaySequence("WPR - Initialize Operation Completed");
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

VOID CWaferPr::PreStartOperation()
{
	m_nLastError	= gnOK;

	try
	{
		m_nLastError = OpPreStart();

		if (m_nLastError != gnOK)
		{
			if (!IsAbort())
			{
				SetAlert(m_nLastError);
			}

			Result(gnNOTOK);
		}

		DisplaySequence("WPR - PreStartOperation Completed");
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

VOID CWaferPr::StopOperation()
{
	State(IDLE_Q);
	Motion(FALSE);

	Signal(gnSTATION_COMPLETE);
	LogCycleStopState("WPR - Signal Complete");

	if( IsAOIOnlyMachine()==FALSE )
	{
		AutoBondScreen(FALSE);
		if (m_bCurrentCamera > WPR_CAM_BOND)
		{
			m_bCurrentCamera = WPR_CAM_BOND;
		}
		ChangeCamera(m_bCurrentCamera);
	}

	TurnOffPSLightings();

	m_qSubOperation = 0;
	WPRLog_SaveRecordToFile();		// Save the log information to file
}
