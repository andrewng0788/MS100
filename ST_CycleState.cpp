/////////////////////////////////////////////////////////////////
// ST_CycleState.cpp : Cycle Operation State of the CSafety class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, March 22, 2005
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
#include "Safety.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CSafety::AutoOperation()
{
	RunOperation();
}

VOID CSafety::DemoOperation()
{
	RunOperation();
}

VOID CSafety::StopOperation()
{
	Command(glAMS_NULL_COMMAND);
	State(IDLE_Q);
	Motion(FALSE);
	SignalInform(m_szStationName, gnSTATION_COMPLETE);
}

VOID CSafety::RunOperation()
{
    if ((Command() == glSTOP_COMMAND) &&
		(Action() == glABORT_STOP))
	{
		DisplaySequence("STS - STOP");
		State(STOPPING_Q);
	}
	else		//	else if (!m_bJogMode || m_bStep)
	{
		try
		{
			if (WaitBackupNVRam())
			{
				DisplaySequence("STS - backup NVRam");
				SetBackupNVRam(FALSE);
			}
			else if (m_bStStartPrintLabel)
			{
				DisplaySequence("STS - start print label");
				PrintLabelInAutoBondMode();
				m_bStStartPrintLabel = FALSE;
			}
			else if (m_bStop)
			{
				DisplaySequence("STS - House Keeping Q");
				if (GetHouseKeepingToken("SafetyStn") == TRUE)
				{
					SetHouseKeepingToken("SafetyStn", FALSE);

					StopCycle("SafetyStn");
					LogCycleStopState("STS - stop completely");
					State(STOPPING_Q);
					SetHouseKeepingToken("SafetyStn", TRUE);
				}
			}

			LightTowerOperate();

			if( m_bSignalStopCycle==FALSE )		//v4.52A11
			{
				if( MonitorFalseAutoRunning()==FALSE )
				{
					LONG lCounter = 0;
					while( 1 )
					{
						DisplaySequence("STS - Idle House Keeping Q");
						if (GetHouseKeepingToken("SafetyStn") == TRUE)
						{
							SetHouseKeepingToken("SafetyStn", FALSE);

							StopCycle("SafetyStn");
							LogCycleStopState("STS - stop completely because idle time out");
							State(STOPPING_Q);

							SetHouseKeepingToken("SafetyStn", TRUE);
							DisplaySequence("STS - stop because idle time out");
							break;
						}
						else
						{
							Sleep(1000);
							continue;
						}
						lCounter++;
						if( lCounter>10 )
						{
							break;
						}
					}
				}

				MonitorThermalControlAutoRunning();
			}
		}
		catch(CAsmException e)
		{
			DisplayException(e);
			Result(gnNOTOK);
			State(ERROR_Q);
			NeedReset(TRUE);
			StopCycle("SafetyStn");
			SetMotionCE(TRUE, "Safety Stn RunOperation Stop");
		}

		Sleep(10);
	}
}
