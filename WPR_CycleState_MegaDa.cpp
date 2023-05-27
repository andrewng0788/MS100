/////////////////////////////////////////////////////////////////
// WPR_CycleState_MegaDa.cpp : Cycle Operation State of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Fri, Nov 15, 2019
//	Revision:	1.00
//
//	By:			Matthew Law
//
//	Copyright @ ASM Assembly Automation Ltd., 2019.
//	ALL rights reserved.
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

VOID CWaferPr::RunOperation_MegaDa()
{
	if (Command() == glSTOP_COMMAND && Action() == glABORT_STOP)
	{
		DisplaySequence("WPR - STOP");
		State(STOPPING_Q);
		return ;
	}

	try
	{
		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case MEGA_DA_LOOKFORWARD_Q:
			DisplaySequence("WPR - Mega Da Look Forward q");
			MegaDaSearchDieLookForward(0 ,2);
			m_qSubOperation = MEGA_DA_WAIT_LOOKFORWARD_Q;
			break;
		case MEGA_DA_WAIT_LOOKFORWARD_Q:
			DisplaySequence("WPR - Mega Da Wait Look Forward q");
			if (theApp.m_bMegaCycleStart)
			{
				Sleep(1);
			}
			else
			{
				Command(glNULL_COMMAND);
				State(IDLE_Q);
			}
			break;
		case WAIT_WT_STABLE_Q:
			DisplaySequence("WPR - wait WT stable q");
			if (m_bSetAutoBondScreen)
			{
				AutoBondScreen(TRUE);
				m_bSetAutoBondScreen = FALSE;
			}

			if (WaitBhTReadyForWPR() && WaitWTStable())
			{
				SetBhTReadyForWPR(FALSE);
				SetWTStable(FALSE);

				m_qSubOperation = GRAB_IMAGE_Q;
			}

			break;
		case GRAB_IMAGE_Q:	//SearchDieCmd
			DisplaySequence("WPR - Grab Image");

			if (MegaDaAutoSearchDie())
			{
				TakeTime(WPR1);
				m_qSubOperation = WAIT_GRAB_DONE_Q;	
			}
			else
			{
				HmiMessage_Red_Yellow("WPR AutoSearchDie Search DieCmd error!");		//v4.44A1	//Semitek
				SetErrorMessage("WPR AutoS earchDie cmd fails.");
				m_qSubOperation = HOUSE_KEEPING_Q;	
			}
			break;

		case WAIT_GRAB_DONE_Q:	//Reply 1
			DisplaySequence("WPR - Wait Grab Done");
			
			if (AutoGrabDone())
			{
				SetPRLatched(TRUE);
				SetPRLatchedForWT(TRUE);
				SetPRLatchedForBT(TRUE);
				TakeTime(WPR2);

				m_qSubOperation = INSPECTION_Q;	
			}
			else
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			break;

		case INSPECTION_Q:	//Reply 2 and 3
			DisplaySequence("WPR - Inspection");
			
			if (MegaDaAutoDieResult())
			{
				SetLFReady(TRUE);
				LFDieTakeLog();
				m_qSubOperation = WAIT_WT_STABLE_Q;
			}
			else
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			break;
		case HOUSE_KEEPING_Q:

			break;
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