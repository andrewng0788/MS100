/////////////////////////////////////////////////////////////////
// BPR_State.cpp : State of BondPrStn
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
#include "BondPr.h"
#include "BPR_Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CBondPr::IdleOperation()
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

VOID CBondPr::Operation()
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

	if (State() == IDLE_Q)
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

		if (m_bStartOffline)
		{
		//	DoOfflinePostBond();
		}

		Sleep(10);
	}

//	Sleep(10);
}

VOID CBondPr::InitOperation()
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

		DisplaySequence("BPR - Initialize Operation Completed");
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

VOID CBondPr::PreStartOperation()
{
	m_nLastError	= gnOK;
	m_bIsPostBondDataExported = FALSE;	//v3.93

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

		//CMSLogFileUtility::Instance()->BPR_Arm1LogOpen();
		//CMSLogFileUtility::Instance()->BPR_Arm2LogOpen();

		DisplaySequence("BPR - PreStartOperation Completed");
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

VOID CBondPr::StopOperation()
{
	State(IDLE_Q);
	Motion(FALSE);

	Signal(gnSTATION_COMPLETE);
	LogCycleStopState("BPR - Signal Complete");

	m_qSubOperation = 0;

	BPRLog_SaveRecordToFile();		// Save the log information to file
	PrintRtBHXYArray();				//v4.48A18	//WH Sanan

	//v3.98T5	//SaveRT PB data timestamp into registry
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SaveStopBondTsToRegistry(TRUE);
	pApp->SaveStopBondTsToRegistry(FALSE);
	//Write all SPC data into temp directory

	if ( m_bUsePostBond == TRUE )
	{	
		ULONG ulBin = 0;

		for (ulBin = 0; ulBin < BPR_MAX_BINBLOCK; ulBin++)
		{
			ExportPostBondData(ulBin);

#ifdef NU_MOTION
			ExportPostBondData_Arm1(ulBin);
			ExportPostBondData_Arm2(ulBin);
#endif
		}

		ResetPostBondSPCFlags();	//v3.93
	}

	m_bIsPostBondDataExported = TRUE;	//v3.93
}
