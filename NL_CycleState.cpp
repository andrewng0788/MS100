/////////////////////////////////////////////////////////////////
// NL_CycleState.cpp : interface of the CNVCLoader class
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


VOID CNVCLoader::Operation()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	switch (State())
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


	if (m_bUpdateProfile)
	{
		UpdateProfile();
	}

	if (m_bUpdatePosition)
	{
		UpdatePosition();
	}

	if (State() == IDLE_Q)	//|| (State() == AUTO_Q) || (State() == DEMO_Q) )
	{
		try
		{	
			if (m_bUpdateOutput)
			{
				UpdateOutput();
			}

			if (m_bLoopTestStart)
			{
				PerformLoopTest();
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}

	}
}

VOID CNVCLoader::IdleOperation()
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

VOID CNVCLoader::InitOperation()
{
	INT nLastError = OpInitialize();

	DisplaySequence("NL - Initialize Operation Completed");
	Motion(FALSE);
	State(IDLE_Q);
}

VOID CNVCLoader::PreStartOperation()
{
	INT nLastError = gnOK;

	try
	{
		nLastError = OpPreStart();

		DisplaySequence("NL - PreStartOperation Completed");

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

VOID CNVCLoader::AutoOperation()
{
	RunOperation();
}

VOID CNVCLoader::DemoOperation()
{
	RunOperation();
}

VOID CNVCLoader::CycleOperation()
{
	if (Command() == glAMS_STOP_COMMAND)
	{
		State(STOPPING_Q);
	}
}

VOID CNVCLoader::StopOperation()
{
	m_qSubOperation = WAIT_IDLE_Q;

	Command(glAMS_NULL_COMMAND);
	State(IDLE_Q);
	Motion(FALSE);
	Signal(gnSTATION_COMPLETE);
	LogCycleStopState("NL - Signal Complete");
}

VOID CNVCLoader::RunOperation()
{
	if ((Command() == glSTOP_COMMAND) &&
		(Action() == glABORT_STOP))
	{
		DisplaySequence("NL - STOP");
		State(STOPPING_Q);
	}

	LONG lMode = 0;


	try
	{
		if ( IsMotionCE() == TRUE )
		{
			DisplaySequence("NL - critical error ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case WAIT_IDLE_Q:
			if (m_bStop)
			{
				DisplaySequence("NL - WAIT_IDLE_Q ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitNLOperate())
			{
				lMode = (*m_psmfSRam)["BinTable"]["BL_Mode"];

				if (lMode == 1)
				{
					DisplaySequence("NL - Bin FULL");
					m_qSubOperation = BINFULL_O_FRAME_Q;
				}
				else
				{
					DisplaySequence("NL - Change Grade");
					m_qSubOperation = CHANGE_O_FRAME_Q;
				}
				
				//m_qSubOperation = HOUSE_KEEPING_Q;
				SetNLOperate(FALSE);
			}
			else if (WaitNLPreOperate())
			{
				DisplaySequence("NL - PreLoad");
				m_qSubOperation = PRELOAD_O_FRAME_Q;
				SetNLPreOperate(FALSE);
			}
			break;

		case PRELOAD_O_FRAME_Q:
			DisplaySequence("NL - PRELOAD_O_FRAME_Q -> WAIT_IDLE_Q");
			m_qSubOperation = WAIT_IDLE_Q;
			break;

		case BINFULL_O_FRAME_Q:
			DisplaySequence("NL - BINFULL_O_FRAME_Q -> WAIT_IDLE_Q");
			SetNLReady(TRUE);
			m_qSubOperation = WAIT_IDLE_Q;
			break;

		case CHANGE_O_FRAME_Q:
			DisplaySequence("NL - CHANGE_O_FRAME_Q -> WAIT_IDLE_Q");
			SetNLReady(TRUE);
			m_qSubOperation = WAIT_IDLE_Q;
			break;


		case WAIT_LOADUNLOAD_Q:
		case PRELOAD_I_FRAME_Q:
		case LOAD_I_FRAME_Q:
		case UNLOAD_I_FRAME_Q:

		case LOAD_O_FRAME_Q:
		case UNLOAD_O_FRAME_Q:
			if (m_bStop)
			{
				DisplaySequence("NL - AUTOBOND IDLE ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("NL - House Keeping Q");
			if (GetHouseKeepingToken("NVCLoaderStn") == TRUE)
			{
				SetHouseKeepingToken("NVCLoaderStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}

			StopCycle("NVCLoaderStn");
			LogCycleStopState("NL - stop completely");
			State(STOPPING_Q);
			SetHouseKeepingToken("NVCLoaderStn", TRUE);
			break;
		}

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		Result(gnNOTOK);
		State(ERROR_Q);
		NeedReset(TRUE);
		StopCycle("NVCLoaderStn");
		SetMotionCE(TRUE, "NVCLoaderStn RunOperation Exception Stop");
	}

	Sleep(10);
}

BOOL CNVCLoader::PerformLoopTest()
{
	BOOL bResult = TRUE;
	INT nConvID = 0;
	CString szLog;

	BOOL bWTUseGripper1 = m_bWTUseGripper1ForSetup;
	BOOL bBTUseGripper1 = m_bBTUseGripper1ForSetup;

	if (m_bLoopTestForward)
	{
		if (m_ucLoopTestNo == 6)
		{
			szLog.Format("NL LOOPTEST #6: loading BIN SLOT #%ld ...", m_lCurrWafSlotID);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			bResult = OpLoadBinFrame(m_lCurrWafSlotID);
		}
		else if (m_ucLoopTestNo == 5)
		{
			szLog.Format("NL LOOPTEST #5: loading WAF SLOT #%ld ...", m_lCurrWafSlotID);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			bResult = OpLoadWaferFrame(m_lCurrWafSlotID);
		}
		else if (m_ucLoopTestNo == 4)
		{
			szLog.Format("NL LOOPTEST #4: loading WAF SLOT #%ld ...", m_lCurrWafSlotID);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			bResult = LoadWaferFrame(m_lCurrWafSlotID, bWTUseGripper1, TRUE);
			if (!bResult)
			{
				szLog = "LoadWaferFrame fail!";
				SetErrorMessage(szLog);
				HmiMessage(szLog);
			}

		}
		else if (m_ucLoopTestNo == 3)
		{
			//bResult = UnloadWaferFrame(TRUE, TRUE);				//Use Waf Gripper
			bResult = UnloadWaferFromWT(TRUE);
			if (!bResult)
			{
				szLog = "MUnloadWaferFromWT fail!";
				SetErrorMessage(szLog);
				HmiMessage(szLog);
			}
			else
			{
				//Close wafer expander
				INT nConvID = 0;
				bResult = LoadUnloadWafExpander(FALSE, nConvID, FALSE);
				if (!bResult)
				{
					szLog = "Unload Wafer Expander fail!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
				}

				bResult = MoveXYToStandby(TRUE);
				if (!bResult)
				{
					szLog = "UnloadWaferFromWT MoveXYToStandby fail!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
				}

				bResult = LoadUnloadWafExpander_Sync(FALSE, nConvID);
				if (!bResult)
				{
					szLog = "Unload Wafer Expander SYNC fail!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
				}
			}
		}
		else if (m_ucLoopTestNo == 2)
		{
			bResult = UnloadBinFrameFromTable(FALSE, TRUE);		//Use Bin Gripper

		}
		else if (m_ucLoopTestNo == 1)
		{
			bResult = LoadUnloadBinExpander(TRUE, nConvID, TRUE);
		}
		else
		{
			bResult = LoadUnloadWafExpander(TRUE, nConvID, TRUE);
		}
		
		m_bLoopTestForward = FALSE;
	}
	else
	{
		if (m_ucLoopTestNo == 6)
		{
			bResult = OpUnloadLastBinFrame();

			if (bResult)
			{
				m_lCurrWafSlotID++;
				if (m_lCurrWafSlotID > m_lLoopTestEndGrade)
				{
					m_lCurrWafSlotID = m_lLoopTestStartGrade;
				}
			}
		}
		else if (m_ucLoopTestNo == 5)
		{
			bResult = OpUnloadLastWaferFrame();

			if (bResult)
			{
				m_lCurrWafSlotID++;
				if (m_lCurrWafSlotID > 4)
				{
					m_lCurrWafSlotID = 1;
				}
			}
		}
		else if (m_ucLoopTestNo == 4)
		{
			bResult = UnloadWaferFrame(bWTUseGripper1, FALSE);
			if (!bResult)
			{
				szLog = "UnloadWaferFrame fail!";
				SetErrorMessage(szLog);
				HmiMessage(szLog);
			}
		}
		else if (m_ucLoopTestNo == 3)
		{
			CString szLog;
			bResult = LoadWaferToWT(TRUE, FALSE);
			if (!bResult)
			{
				szLog = "LoadWaferToWT fail!";
				SetErrorMessage(szLog);
				HmiMessage(szLog);
			}

			bResult = MoveXYToStandby(TRUE);
			if (!bResult)
			{
				szLog = "LoadWaferToWT MoveXYToStandby fail!";
				SetErrorMessage(szLog);
				HmiMessage(szLog);
			}


			//if (HmiMessage("Continue?", "WT LOAD UNLOAD Test", glHMI_MBX_YESNO) != glHMI_YES )
			//{
			//	bResult = FALSE;
			//}

		}
		else if (m_ucLoopTestNo == 2)
		{
			bResult = LoadBinFrameToTable(FALSE, TRUE);			//Use Bin Gripper
			if (bResult)
			{
				m_ucLoopTestNo = 3;		//To Toggle between #2 and #3
			}
		}
		else if (m_ucLoopTestNo == 1)
		{
			bResult = LoadUnloadBinExpander(FALSE, nConvID, TRUE);
		}
		else
		{
			bResult = LoadUnloadWafExpander(FALSE, nConvID, TRUE);
		}

		m_bLoopTestForward = TRUE;
	}

	Sleep(500);

	if (bResult == FALSE)
	{
		m_bLoopTestAbort = TRUE;
		m_bLoopTestStart = FALSE;
		return FALSE;
	}

	if (m_bLoopTestAbort)	// && m_bLoopTestForward)
	{
		if (m_ucLoopTestNo == 5)
		{
			if (m_bLoopTestForward)
			{
				m_bLoopTestStart = FALSE;
			}
		}
		else
		{
			m_bLoopTestStart = FALSE;
		}
	}

	return TRUE;
}


