#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "CycleState.h"
#include "math.h"
#include "BondPr.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CBondHead::RunOperation_ES()
{
	CString szMsg;
	double	dCurrentTime;
	static	BOOL	s_bUpdateNVRAM=TRUE;
	BOOL bNoMotionHouseKeeping = FALSE;

	// Store Wafer Table to common area
	(*m_psmfSRam)["WaferTable"]["PRDelay"]		= m_lPRDelay;
	(*m_psmfSRam)["WaferTable"]["VacOffDelay"]	= m_lEjVacOffDelay;			//v3.78

    if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		DisplaySequence("BHNG - STOP");
		State(STOPPING_Q);
		return ;
	}

	m_bStep = FALSE;

	try
	{
		if ( IsMotionCE() == TRUE )
		{
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		if (m_bStopAllMotion == TRUE)
		{
			m_qSubOperation = HOUSE_KEEPING_Q;
			bNoMotionHouseKeeping = TRUE;
		}

		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case WAIT_WT_Q:
		case WAIT_WT_READY_Q:
			DisplaySequence("BHNG - Wait WT Ready to pick");

			// Check whether the collet & ejector count is larger than the user-defined maximum
			if ( m_bStop )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			
			//Check machine pressure
			if (IsLowPressure() == TRUE)
			{
				SetStatusMessage("Machine low pressure");
				SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);
				SetErrorMessage("Machine low pressure");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

            // check machine cover state
			if ( IsCoverOpen() == TRUE )
			{
				SetStatusMessage("Machine Cover Open");
				if( IsPrescanEnded() )
					SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				SetErrorMessage("Machine Cover Open prescanning");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			if (WaitWTReady())
			{	
				if( m_qSubOperation==WAIT_WT_Q )
				{
					DisplaySequence("BHNG - Wait WT Q Start NG Pick Module");
					ResetBuffer();	// to sorting mode
					OpPreStartNGPickModule(TRUE);
				}
				
				SetWTReady(FALSE);
				SetEjectorReady(FALSE);

				DisplaySequence("BHNG - Move TO PICK Z DOWN");

				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 0;

				m_qSubOperation = EJ_UP_Q;
			}
			else
			{
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

				if ( s_bUpdateNVRAM == TRUE )
				{
					SetBackupNVRam(TRUE);
					s_bUpdateNVRAM = FALSE;
				}
			}
			break;

		case EJ_UP_Q:
			DisplaySequence("BHNG - Ej Up");
			if (m_bStop)
			{				
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			if ( AllowMove() == TRUE )
			{
				if( m_lEjectorUpDelay>0 )
				{
					Sleep(m_lEjectorUpDelay);
				}
				// Calculate the compensation for wear out of ejector pin
				TakeTime(EJ1);		// Take Time
				Ej_MoveTo(m_lEjectLevel_Ej, SFM_NOWAIT);
				Ej_Sync();
				TakeTime(EJ2);

				DisplaySequence("BHNG - Wait Pick Delay");
				if ( m_lPickDelay > 0 )
				{
					Sleep(m_lPickDelay);
				}

				//Update Ejector & Collet counter
				OpUpdateMachineCounters();

				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = TRUE;

				TakeTime(EJ3);		// Take Time
				Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
				Ej_Sync();
				TakeTime(EJ4);
				if (m_lEjectorDownDelay > 0)
				{
					Sleep(m_lEjectorDownDelay);
				}

				(*m_psmfSRam)["BinTable"]["Die Bonded X"] = (*m_psmfSRam)["WaferMap"]["X"];
				(*m_psmfSRam)["BinTable"]["Die Bonded Y"] = (*m_psmfSRam)["WaferMap"]["Y"];

				m_qSubOperation = WAIT_Z_EJ_COMPLETE_Q;
			}
			break;

		case WAIT_Z_EJ_COMPLETE_Q:
			DisplaySequence("BHNG - Wait Z and Ej complete");
			// Check whether Z complete first
			SetEjectorReady(TRUE);
			TakeTime(T1);
			OpNGPickIndexReelT();
			NGPickT_Sync();	
			TakeTime(T2);

			SetBackupNVRam(TRUE);

			m_qSubOperation = WAIT_BOND_DELAY_Q;
			break;

		case WAIT_BOND_DELAY_Q:
			DisplaySequence("BHNG - Wait Bond Delay");
			if( m_bStop )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}

			TakeTime(T3);						// Take Time
			if (!OpNGPickCheckSensorStatus())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			TakeTime(T4);						// Take Time

			OpUpdateBondedUnit();				//Update bonded unit

			OpUpdateDieIndex();					// Update the die index

			// Find the cycle time
			dCurrentTime = GetTime();
			if ( m_dLastTime > 0.0 )
			{
				m_dCycleTime = fabs(dCurrentTime - m_dLastTime);

				if ( m_dCycleTime > 99999 )
				{
					m_dCycleTime = 10000;
				}

				// Calculate the average cycle time
				m_dAvgCycleTime = (m_dAvgCycleTime * m_ulCycleCount + m_dCycleTime) / (m_ulCycleCount + 1);
				m_ulCycleCount++;

                // 3501
                SetGemValue("AB_CurrentCycleTime",  m_dCycleTime);
                SetGemValue("AB_AverageCycleTime",  m_dAvgCycleTime);
                SetGemValue("AB_CycleCounter",      m_ulCycleCount);
			}

			m_dLastTime = dCurrentTime;
			TakeTime(CycleTime);
			StartTime(GetRecordNumber());		// Restart the time	
			SetBhTReady(TRUE);

			s_bUpdateNVRAM = TRUE;

			m_qSubOperation = WAIT_WT_READY_Q;
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("BHNG - House Keeping Q");
			SetAutoCycleLog(FALSE);
			if (GetHouseKeepingToken("BondHeadStn") == TRUE)
			{
				SetHouseKeepingToken("BondHeadStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}

			if (bNoMotionHouseKeeping == FALSE)
			{
				if( IsPrescanning()==FALSE )
				{
					CheckCoverOpenInAuto("House Keeping");
				}
			}

			CycleEnableDataLog(FALSE, FALSE, TRUE);

			LogCycleStopState("BH - UpdateDieCounter");
			UpdateDieCounter();			//Update die counter in HMI

			LogCycleStopState("BH - LogWaferInfo");
			LogWaferInformation(FALSE);	//Update current wafer counter

			LogCycleStopState("BH - MachineStat");
			SaveMachineStatistic();		//update machine statistic	

			// Acknowledge other stations to stop
			StopCycle("BondHeadStn");				
			LogCycleStopState("BHNG - stop completely");
			State(STOPPING_Q);

			SetHouseKeepingToken("BondHeadStn", TRUE);
			if( IsPrescanning() && IsCoverOpen() )
			{
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				SetStatusMessage("Machine Cover Open");
				SetErrorMessage("Machine Cover Open when prescanning");
			}
			OpPreStartNGPickModule(FALSE);		//v4.24T10
			CMS896AStn::m_lBondHeadAtSafePos = 1;
			break;

		default:
			DisplaySequence("BHNG - Unknown");
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
	}
	catch (CAsmException e)
	{
		//Handle normal stop cycle task
		LogCycleStopState("BH ex - UpdateDieCounter");
		UpdateDieCounter();			

		LogCycleStopState("BH ex - LogWaferInfo");
		LogWaferInformation(FALSE);	

		LogCycleStopState("BH ex - MachineStat");
		SaveMachineStatistic();		

		CMS896AStn::m_lBondHeadAtSafePos = 1;

		DisplayException(e);
		Result(gnNOTOK);

		//State(ERROR_Q);
		State(STOPPING_Q);

		NeedReset(TRUE);
		StopCycle("BondHeadStn");
		(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
	}
}


BOOL CBondHead::OpPreStartNGPickModule(BOOL bStart)
{
	if (!m_fHardware && m_bDisableBH ||(!IsESMachine()) )
	{
		return TRUE;
	}

	if (bStart)
	{
		if (!m_bIsES101NGPickStarted)
		{
			//Move BackLight to READY pos
			BOOL bOK = TRUE;
			IPC_CServiceMessage stMsg;
			INT nConvID = m_comClient.SendRequest("WaferPrStn", "MoveBackLightToSafeLevel", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);	
				}
			}

			stMsg.GetMsg(sizeof(BOOL), &bOK);
			if (!bOK)
			{
				AfxMessageBox("BH: OpPreStart NGPickModule: fail to move BackLight Z to SAFE level!", MB_SYSTEMMODAL);
				return FALSE;
			}

			m_bIsES101NGPickStarted	= TRUE;
			SetNGPickReelStop(TRUE);
			SetNGPickUpDn(TRUE);

			EjElevator_MoveTo(m_lUpLevel_El);

			return OpReloadNGPickReelT();
		}
	}
	else
	{
		m_bIsES101NGPickStarted	= FALSE;
		SetNGPickReelStop(FALSE);
		SetNGPickUpDn(FALSE);
		EjElevator_MoveTo(0);
	}

	return TRUE;
}


BOOL CBondHead::OpNGPickIndexReelT()
{
	if (!m_fHardware || !IsESMachine() || m_bDisableBH)
	{
		return TRUE;
	}

	if ((m_lNGPickIndexT < 0) || (m_lNGPickIndexT > 1000))
	{
		return FALSE;
	}

	if (!NGPickT_IsPowerOn())
	{
		SetErrorMessage("ES101: NGPick T is not power ON");
		return FALSE;
	}

	if (m_lNGPickIndexT > 0)
	{
		NGPickT_Move(m_lNGPickIndexT, FALSE);
	}

	return TRUE;
}

BOOL CBondHead::OpReloadNGPickReelT()
{
	if (!m_fHardware || m_bDisableBH)
	{
		return TRUE;
	}
	if (!IsESMachine())
	{
		return FALSE;
	}

	if (!NGPickT_IsPowerOn())
	{
		SetErrorMessage("ES101: INDEX reel power is OFF");
		HmiMessage_Red_Yellow("ES101: INDEX reel power is OFF!");
		return FALSE;
	}

	if (m_lNGPickIndexT <= 0)	//for BURNIN purpose
	{
		return TRUE;
	}


	if (!IsTensionCheck())
	{
		LONG lCount		= 0;
		LONG lOnCount	= 0;

		while (lOnCount < 3)
		{
			if (IsTensionCheck())
			{
				lOnCount++;
				Sleep(50);
			}
			else
			{
				lOnCount = 0;
			}

			if (lCount == 0)
			{
				NGPickT_Move(m_lNGPickIndexT * 5);
			}
			else
			{
				NGPickT_Move(m_lNGPickIndexT);
			}
			Sleep(200);
			lCount++;

			if (lCount > 30)
			{
				SetErrorMessage("ES101: fail to manual reload INDEX reel");
				HmiMessage_Red_Yellow("Fails to reload Index REEL!");
				return FALSE;
			}
		}
		
		Sleep(100);
	}

	return TRUE;
}

BOOL CBondHead::OpNGPickCheckSensorStatus()
{
	if (!m_fHardware || (!IsESMachine()) )
	{
		return TRUE;
	}

	return OpReloadNGPickReelT();
}

