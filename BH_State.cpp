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
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CBondHead::Operation()
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

		case CYCLE_Q:
			CycleOperation();
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

	if (theApp.m_bAppClosing)
	{
		Sleep(10);
		return;
	}

	if ((State() == IDLE_Q) || (State() == UN_INITIALIZE_Q))
	{
		try
		{
			UpdateSafetySensor();

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

			if (m_bStartDacTTest)
			{
				//Move T to PICK posn before test
				RunDacTest();
			}

			if (m_bILCAutoLearn)
			{
				ILC_AutoLearnFunc();
			}
			
			if (m_bCoolingFanWillTurnOff	&& 
				IsMS60()					&& 
				!m_bILCAutoLearn) 
				//&& !m_bMS60ThermalCtrl)	//v4.48A12
			{
				Sleep(1);
				m_lBondHeadCoolingSecond++;

				if (IsMotionCE())		//v4.59A45
				{
				}
				else
				{
					if ( m_lBondHeadCoolingSecond >= 1000)	//3000)		//v4.48A10	/v4.55A11
					{
						ResetBondHeadFanTimerAndCounter(TRUE);	// Stop Fan is Finish
						SetBondHeadFan(FALSE);
					}
				}
			}

			OpUpdateBHTThermalRMSValues(30, FALSE);			//v4.48A10

			if (IsMS60() && 
				CMS896AStn::m_bDBHThermostat	&& 
				!m_bCoolingFanWillTurnOff		&&			//v4.55A5
				!m_bILCAutoLearn)							//v4.56A3
			{
				m_lMS60TempCheckCounter++;

				if (m_lMS60TempCheckCounter > MS60_BH_THERMALCHECK_CYCLE_IDLE)
				{
					m_lMS60TempCheckCounter = 0;
					
					if (IsMS60TempertureOverLoaded())
					{
						//v4.50A9	//Placed BEFORE the alarm
						if (m_bEnableMS60ThermalCheck)	//If Heating Coil fcn is enabled
						{	
							EnableBHTThermalControl(FALSE);
							m_bEnableMS60ThermalCheck = FALSE;	//Disable

							CString szLog;
							szLog.Format("PREHEAT is TURNOFF - BH Temp (OverHeat) = %lu deg (limit=51)", 
								m_ulBHTThermostatReading);
							CMSLogFileUtility::Instance()->BH_LogStatus(szLog);

							CString szErr;
							if (m_ulBHTThermostatReading == 0)
								szErr = "Please make sure that the thermo couple is installed properly!";
							else
								szErr.Format("BondHead Temperature is overheat (%ld degree)!", m_ulBHTThermostatReading);
							SetErrorMessage(szErr);
							HmiMessage_Red_Yellow(szErr);
						}
					}

					if (IsMS60TempertureUnderHeat())	//v4.53A19
					{
						if (m_bMS60ThermalCtrl && !m_bEnableMS60ThermalCheck)	//v4.59A19
						{	
							CString szLog;
							szLog.Format("PREHEAT is TURNON - BH Temp (UnderHeat) = %lu deg (limit=45)", 
								m_ulBHTThermostatReading);
							CMSLogFileUtility::Instance()->BH_LogStatus(szLog);

							EnableBHTThermalControl(TRUE);
						}
					}

					if (m_ulBHTThermostatCounter >= 10)
						m_ulBHTThermostatCounter = 0;
				}
			}

			NVC_MotionTest();

			if (m_bIsOpenDACCalibStart)
			{
				OpenDACCalibration(m_lOpenDACCurrBHZ);
			}
			else if (m_bIsOpenDACForceCheckStart)
			{
				OpenDACForceCheck(m_lOpenDACCurrBHZ);
			}

			if (CMS896AStn::m_bEMOChecking)
			{
				m_lEMOCheckCounter++;
				if (m_lEMOCheckCounter > 20)
				{
					m_lEMOCheckCounter = 0;
					if (IsEMOTriggered() && !m_bIsEMOTriggered)
					{
						OpEMOStopAutobond();
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
	else if ((State() == AUTO_Q) || (State() == DEMO_Q))	//v4.49A5
	{
		OpUpdateBHTThermalRMSValues(30, FALSE);
	}
	else if (State() == ERROR_Q)
	{
		Sleep(10);
	}
}

/////////////////////////////////////////////////////////////////
//	Operation State Functions
/////////////////////////////////////////////////////////////////

VOID CBondHead::IdleOperation()
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

void CBondHead::DiagOperation()
{
	//SFM_CHipecAcServo*		pServo;
	m_nLastError	= gnOK;
	
	LONG	lAxis, lAction, lUnit;
	AxisAction(lAxis, lAction, lUnit);

	try
	{
		CString	szAxisID = "";
		
		if (m_fHardware && (lAxis < 5))
		{
			szAxisID = GetAxisID(lAxis);
		}

		switch (lAction)
		{
		case glPOSITION_ACTION:
			switch (lAxis)
			{
			case 1:
				switch (lUnit)
				{
				case 0:
					SetInitHome(szAxisID, FALSE);
					m_nLastError = Z_Home();
					break;

				case 1:
					m_nLastError = Z_MoveTo(m_lPickLevel_Z);
					break;

				case 2:
					m_nLastError = Z_MoveTo(m_lReplaceLevel_Z);
					break;

				case 3:
					m_nLastError = Z_MoveTo(m_lBondLevel_Z);
					break;

				case 4:
					m_nLastError = Z_MoveTo(m_lSwingLevel_Z);
					break;
				}
				break;

			case 2:
				switch (lUnit)
				{
				case 0:
					SetInitHome(szAxisID, FALSE);
					m_nLastError = T_Home();
					break;

				case 1:
					m_nLastError = T_MoveTo(m_lPrePickPos_T);
					break;

				case 2:
					m_nLastError = T_MoveTo(m_lPickPos_T);
					break;

				case 3:
					m_nLastError = T_MoveTo(m_lPreBondPos_T);
					break;

				case 4:
					m_nLastError = T_MoveTo(m_lBondPos_T);
					break;

				case 5:
					m_nLastError = T_MoveTo(m_lCleanColletPos_T);
					break;
				}
				break;

			case 0:
				switch (lUnit)
				{
				case 0:
					SetInitHome(szAxisID, FALSE);
					m_nLastError = Ej_Home();
					break;

				case 1:
					m_nLastError = Ej_MoveTo(m_lEjectLevel_Ej);
					break;

				case 2:
					m_nLastError = Ej_MoveTo(m_lPreEjectLevel_Ej);
					break;

				case 3:
					m_nLastError = Ej_MoveTo(m_lStandbyLevel_Ej);
					break;
				}
				break;

			case 6:
				switch (lUnit)
				{
				case 0:
//					m_doColletVac.Off();
					break;

				case 1:
//					m_doColletVac.Hold();
					break;

				case 2:
//					m_doColletVac.Pulse();
					break;
				}
				break;

			case 7:
				switch (lUnit)
				{
				case 0:
//					m_doColletPress.Off();
					break;

				case 1:
//					m_doColletPress.Hold();
					break;

				case 2:
//					m_doColletPress.Pulse();
					break;
				}
				break;

			case 8:
				switch (lUnit)
				{
				case 0:
//					m_doRotaryCollet.Off();
					break;

				case 1:
//					m_doRotaryCollet.Hold();
					break;

				case 2:
//					m_doRotaryCollet.Pulse();
					break;
				}
				break;

			case 9:
				switch (lUnit)
				{
				case 0:
//					m_doMissDie.Off();
					break;

				case 1:
//					m_doMissDie.Hold();
					break;

				case 2:
//					m_doMissDie.Pulse();
					break;
				}
				break;

			case 10:
				switch (lUnit)
				{
				case 0:
//					m_doBondForce.Off();
					break;

				case 1:
//					m_doBondForce.Hold();
					break;

				case 2:
//					m_doBondForce.Pulse();
					break;
				}
				break;
			}
			break;

		case glRELATIVE_ACTION:
			if (m_fHardware)
			{
				//m_nLastError = MoveRelative(szAxisID, lUnit, SFM_WAIT, 0);
				{
					CString szTemp;

					szTemp.Format("Axis: %s, Unit: %ld", szAxisID, lUnit);
					DisplayMessage(szTemp);
				}

				switch (lAxis)
				{
					case 0:
						m_nLastError = Ej_Move(lUnit);
						break;

					case 1:
						m_nLastError = Z_Move(lUnit);
						break;

					case 2:
						m_nLastError = T_Move(lUnit);
						break;
				}	
			
			}
			break;

		case glABSOLUTE_ACTION:
			if (m_fHardware)
			{
				m_nLastError = MoveAbsolute(szAxisID, lUnit, SFM_WAIT, 0);
			}
			break;

		case  glFAST_HOME_ACTION:
			if (m_fHardware)
			{
				m_nLastError = MoveActuator(szAxisID, GetPositionID(szAxisID, FAST_HOME_POS, lUnit));
			}
			break;

		case glPOWER_ON_ACTION:
			if (m_fHardware)
			{
				if (lUnit == 0)
				{
					PowerOffAxis(szAxisID);
				}
				else
				{
					PowerOnAxis(szAxisID);
				}
			}
			break;

		case glCOMMUTATE_ACTION:
			if (m_fHardware)
			{
				m_nLastError = CommutateServo(szAxisID);
			}
			break;

		case glCLEAR_SERVO_ACTION:
			if (m_fHardware)
			{
				switch (lAxis)
				{
/*
				case 0:
					ClearServoError("srvBondHeadX");
					break;

				case 1:
					ClearServoError("srvBondHeadY");
					break;
*/
				case 2:
					//ClearServoError("srvBondHeadZ");
					CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);
					break;

				case 3:
					//ClearServoError("srvBondHeadT");
					CMS896AStn::MotionClearError(BH_AXIS_T, &m_stBHAxis_T);
					break;

				case 4:
					//ClearServoError("srvEjector");
					CMS896AStn::MotionClearError(BH_AXIS_EJ, &m_stBHAxis_Ej);
					break;
				}
			}
		case 88:
			INT i;
			for ( i=0; i < 10; i++ )
			{
				T_Move(lUnit, SFM_NOWAIT);
				Z_Move(lUnit, SFM_WAIT);
				//pServo	= GetHipecAcServo("srvBondHeadT");
				//pServo->Synchronize(15000);
				try
				{
					CMS896AStn::MotionSync(BH_AXIS_T, 15000, &m_stBHAxis_T);
				}
				catch (CAsmException e)
				{
				}

				T_Move(-lUnit, SFM_NOWAIT);
				Z_Move(-lUnit, SFM_WAIT);
				//pServo	= GetHipecAcServo("srvBondHeadT");
				//pServo->Synchronize(15000);
				try
				{
					CMS896AStn::MotionSync(BH_AXIS_T, 15000, &m_stBHAxis_T);
				}
				catch (CAsmException e)
				{
				}
			}
			break;

		}
	}
	catch (CAsmException e)
	{
		NeedReset(TRUE);
		DisplayException(e);
	}
	
	SetAlert(m_nLastError);

	Motion(FALSE);
	State(IDLE_Q);
}

VOID CBondHead::InitOperation()
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

		DisplaySequence("BH - Initialize Operation Completed");
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

VOID CBondHead::PreStartOperation()
{
	m_nLastError	= gnOK;

	if (m_bEnableOpenDacBondOption)		//Changed to OPEN-DAC control	//andrew
		HmiMessage("Enabling BH-Z Open DAC control at BOND position....");

	try
	{
		m_lBondCount = 0;
		m_nLastError = OpPreStart();

		if (m_bEnableAirFlowTest || m_bEnableMDResponseChecking)
			BH_LOG_OPEN();

		//Motion Log
#ifdef NU_MOTION
		CycleEnableDataLog(TRUE, FALSE, TRUE);
#else
		if (m_bEnableMotionLogZ)	
		{
			SetupDataLogZ();
			EnableDataLogZ(TRUE);
		}
		else if (m_bEnableMotionLogT)
		{
			SetupDataLogT();
			EnableDataLogT(TRUE);
		}
		else if (m_bEnableMotionLogEJ)
		{
			SetupDataLogEjector();
			EnableDataLogEjector(TRUE);
		}
		else if (m_bEnableMotionLogZ2)
		{
		}
#endif

		if (m_nLastError != gnOK)
		{
			if (!IsAbort())
			{
				SetAlert(m_nLastError);
			}

			Result(gnNOTOK);
		}

#ifdef NU_MOTION
		if (m_qSubOperation	!= HOUSE_KEEPING_Q)		//v4.40T6
		{
			if ( IsMS60() == TRUE )
			{
				ResetBondHeadFanTimerAndCounter(TRUE);
			}
			SetBondHeadFan(TRUE);					//v4.26T1	//MS100Plus v2.2 new thermal control fcn
		}
#endif

		DisplaySequence("BH - PreStartOperation Completed");
		SetStatusMessage("Bonding cycle started");
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

VOID CBondHead::StopOperation()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (m_fHardware && !m_bDisableBH && m_bStopAllMotion == FALSE)	//v3.60
	{
		/*
		if( m_bSel_Z )
		{
			try
			{
				CMS896AStn::MotionSync(BH_AXIS_Z, 10000, &m_stBHAxis_Z);
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			}
		}

		if( m_bSel_Z2 )
		{
			try
			{
				CMS896AStn::MotionSync(BH_AXIS_Z2, 10000, &m_stBHAxis_Z2);
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			}
		}

		if( m_bSel_T )
		{
			try
			{
				CMS896AStn::MotionSync(BH_AXIS_T, 10000, &m_stBHAxis_T);
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			}
		}
		*/

		if( m_bSel_Ej )
		{
			try
			{
				CMS896AStn::MotionSync(BH_AXIS_EJ, 10000, &m_stBHAxis_Ej);
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			}
		}
	}

	Z_Profile(NORMAL_PROF);
	T_Profile(NORMAL_PROF);
	Ej_Profile(NORMAL_PROF);

	//v4.33T1	//SanAn feature of not allowing non-OP to run AUTOBOND for more than 1 min ~ 600 dices
	if (OpCheckValidAccessMode(TRUE) != TRUE)
	{
		SetAlert_Red_Yellow(IDS_MS_ACCESS_RIGHT);

		//HmiMessage_Red_Yellow("Non-OP mode triggers machine stop; please login machine again with OP mode", "Sanan Access Right Check");
		SetErrorMessage("Bonding cycle stopped by SanAn non-OP access");
	}

	State(IDLE_Q);
	Motion(FALSE);

	SignalInform(m_szStationName, gnSTATION_COMPLETE);

	//andrew
	if (m_bEnableAirFlowTest || m_bEnableMDResponseChecking)
		BH_LOG_CLOSE();

	if (m_bMS100DigitalAirFlowSnr && m_bEnableDAFlowRunTimeCheck)	//v4.52A18	//v4.53A1
	{
		CMSLogFileUtility::Instance()->BH_Z1LogClose();
  		CMSLogFileUtility::Instance()->BH_Z2LogClose();
	}

	if (!IsMS60())		//v4.47T2	
	{
		SetBondHeadFan(FALSE);				//v4.26T1		//MS100Plus v2.2 new thermal control fcn
	}
	else
	{
		ResetBondHeadFanTimerAndCounter(FALSE);

		if (m_bMS60ThermalCtrl)		//v4.49A6
		{
			EnableBHTThermalControl(TRUE);
		}
	}
		

	if (m_bStopAllMotion == FALSE)
	{
		// removed the CycleEnableDataLog(...) at 4.44 version
#ifndef NU_MOTION
		if (m_bEnableMotionLogZ)
		{
			EnableDataLogZ(FALSE);
		}
		else if (m_bEnableMotionLogT)
		{
			EnableDataLogT(FALSE);
		}
		else if (m_bEnableMotionLogEJ)
		{
			EnableDataLogEjector(FALSE);
		}
		else if (m_bEnableMotionLogZ2)
		{
		}
#endif
		//SetComplete(FALSE);
		//CString szMsg = _T("   ") +  GetName() + "--- Operation Stop";
		//DisplayMessage(szMsg);
	}

	//4.42T3
	m_qSubOperation = 0;

	if ( (m_bEnableFrontCoverLock == TRUE) && (m_bIsFrontCoverExist == TRUE) )
	{
		SetFrontCoverLock(FALSE);
	}

	//v4.42T2
	//if ( (m_bEnableSideCoverLock == TRUE) && (m_bIsSideCoverExist == TRUE) )
	//{
	//	SetSideCoverLock(FALSE);
	//}

	//v4.42T4
/*	if (CMS896AApp::m_bMS100Plus9InchOption && (pApp->GetCustomerName() == "Lumileds") )
	{
	}
	else
	{
		if ( (m_bEnableBinElevatorCoverLock == TRUE) && (m_bIsBinElevatorCoverExist == TRUE) )
		{
			SetBinElevatorCoverLock(FALSE);
		}
	}
*/
	if (m_lAutobondTimeInMin > 0)
	{
		LONG lAutobondCurrTime = (LONG)GetTime();
		m_lAutobondTotalTime = m_lAutobondTotalTime + (lAutobondCurrTime - m_lAutobondStartTime);
	}

/*
	//v4.33T1	//SanAn feature of not allowing non-OP to run AUTOBOND for more than 1 min ~ 600 dices
	if (OpCheckValidAccessMode(TRUE) != TRUE)
	{
		HmiMessage_Red_Yellow("Non-OP mode triggers machine stop; please login machine again with OP mode", "Sanan Access Right Check");
		SetErrorMessage("Bonding cycle stopped by SanAn non-OP access");
	}
*/
	//BHZ1
	CString szMess;
	szMess.Format("END,BHZ1,%d\n",  m_ulColletCount);
	CMSLogFileUtility::Instance()->MissingDieThreshold_Log(FALSE, szMess);	
	CMSLogFileUtility::Instance()->MissingDieThreshold_LogClose(FALSE);
	//BHZ2
	szMess.Format("END,BHZ2,%d\n",  m_ulCollet2Count);
	CMSLogFileUtility::Instance()->MissingDieThreshold_Log(TRUE, szMess);	
	CMSLogFileUtility::Instance()->MissingDieThreshold_LogClose(TRUE);
	SetStatusMessage("Bonding cycle stopped");
}



