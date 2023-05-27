/////////////////////////////////////////////////////////////////
// BT_State.cpp : Operation State of the CBinTable class
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
#include "BinTable.h"
#include "BT_Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//	Operation State Functions
/////////////////////////////////////////////////////////////////

VOID CBinTable::Operation()
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

	if ((m_qState == IDLE_Q) || (m_qState == AUTO_Q) || (m_qState == DEMO_Q)) 
	{
		if (CMS896AStn::m_bAutoGenBinBlkCountSummary == TRUE)
		{
			AutoGenerateBinBlkSummary();
		}
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

			if (m_bStartCalibrate)				//v4.49A9
			{
				MoveWaferTheta();
			}

			if (m_bDoOfflinePostbondTest)
			{
				OfflinePostBondTest1();
			}

			//andrewng //2020-0804
			if (m_bDoOfflinePostbondTest2)
			{
				OfflinePostBondTest2();
			}

			MotionTest();

			//Lumileds Offline postbond test	//v4.11T1
			if (m_bOfflinePostBondTest)	
				RunOfflinePostBondTest();

			CreateCreeStatisticReport(600);		//v4.49A4

		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}

		if (m_bDoOfflinePostbondTest2)
		{
			Sleep(0);
		}
		else
		{
			Sleep(10);
		}
	}
	else if (State() == ERROR_Q)
	{
		Sleep(10);
	}
}


// Get all sensor values from hardware
VOID CBinTable::GetSensorValue()
{
	//if (m_bDisableBT)	//v3.60
	//	return;

	try
	{
		m_bHomeSnr_X		= CMS896AStn::MotionIsHomeSensorHigh(BT_AXIS_X, &m_stBTAxis_X);
		m_bHomeSnr_Y		= CMS896AStn::MotionIsHomeSensorHigh(BT_AXIS_Y, &m_stBTAxis_Y);
		//m_bPosLimitSnr_X	= CMS896AStn::MotionIsPositiveLimitHigh(BT_AXIS_X, &m_stBTAxis_X);
		//m_bPosLimitSnr_Y	= CMS896AStn::MotionIsPositiveLimitHigh(BT_AXIS_Y, &m_stBTAxis_Y);
		//m_bNegLimitSnr_X	= CMS896AStn::MotionIsNegativeLimitHigh(BT_AXIS_X, &m_stBTAxis_X);
		//m_bNegLimitSnr_Y	= CMS896AStn::MotionIsNegativeLimitHigh(BT_AXIS_Y, &m_stBTAxis_Y);
		//
		//if (m_bUseDualTablesOption)	//v4.16T3	//MS100 9Inch
		//{
		//	m_bPosLimitSnr_X2	= CMS896AStn::MotionIsPositiveLimitHigh(BT_AXIS_X2, &m_stBTAxis_X2);
		//	m_bPosLimitSnr_Y2	= CMS896AStn::MotionIsPositiveLimitHigh(BT_AXIS_Y2, &m_stBTAxis_Y2);
		//	m_bNegLimitSnr_X2	= CMS896AStn::MotionIsNegativeLimitHigh(BT_AXIS_X2, &m_stBTAxis_X2);
		//	m_bNegLimitSnr_Y2	= CMS896AStn::MotionIsNegativeLimitHigh(BT_AXIS_Y2, &m_stBTAxis_Y2);
		//}
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

// Get all encoder values from hardware
VOID CBinTable::GetEncoderValue()
{	
	if (m_fHardware == FALSE)
		return;
	if (m_bDisableBT)
		return;
	if (CMS896AStn::m_bMS60NGPick)	//v4.57A1
	{
		m_lEnc_X = 0;
		m_lEnc_Y = 0;
		return;
	}

	CString szAxis;
	try
	{
		szAxis		= BT_AXIS_X;
		m_lEnc_X	= CMS896AStn::MotionGetEncoderPosition(szAxis, 1, &m_stBTAxis_X);
		m_lCmd_X	= CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBTAxis_X);		//CSP
		
		szAxis		= BT_AXIS_Y;
		m_lEnc_Y	= CMS896AStn::MotionGetEncoderPosition(szAxis, 1, &m_stBTAxis_Y);
		m_lCmd_Y	= CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBTAxis_Y);		//CSP

		if (m_bEnable_T)
		{
			szAxis		= BT_AXIS_T;
			m_lEnc_T = GetTEncoderValue(szAxis);
		}
	}
	catch(CAsmException e)
	{
		DisplayException(e);
		if (szAxis == BT_AXIS_X)
			CMS896AStn::MotionCheckResult(szAxis, &m_stBTAxis_X);
		else if (szAxis == BT_AXIS_Y)
			CMS896AStn::MotionCheckResult(szAxis, &m_stBTAxis_Y);
		else if (szAxis == BT_AXIS_X2)
			CMS896AStn::MotionCheckResult(szAxis, &m_stBTAxis_X2);
		else
			CMS896AStn::MotionCheckResult(szAxis, &m_stBTAxis_Y2);
	}
}

LONG CBinTable::GetTEncoderValue(CString szAxis)
{
	LONG lEnc_T = 0;

	if (m_bUseTEncoder)
	{
		//if (CMS896AStn::MotionIsServo(szAxis, &m_stBTAxis_T))
		//{
		//	lEnc_T = CMS896AStn::MotionGetEncoderPosition(szAxis, m_BT_T_RESOLUTION, &m_stBTAxis_T);
		//}
		//else
		//{
		lEnc_T = CMS896AStn::MotionGetEncoderPosition(szAxis, m_BT_T_RESOLUTION * 0.8, &m_stBTAxis_T);
		//}
	}
	else
	{
		lEnc_T = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBTAxis_T);
	}

	return lEnc_T;
}


BOOL CBinTable::SaveEncoderValue() //called in UpdateOutput()
{
	if (m_ulJoystickFlag == 1)   
	{
		m_lPhyUpperLeftX = ConvertXEncoderValueForDisplay(m_lEnc_X);
		m_lPhyUpperLeftY = ConvertYEncoderValueForDisplay(m_lEnc_Y);
	}
	else if (m_ulJoystickFlag == 2)
	{
		m_lPhyLowerRightX = ConvertXEncoderValueForDisplay(m_lEnc_X);
		m_lPhyLowerRightY = ConvertYEncoderValueForDisplay(m_lEnc_Y);
	}
	else if (m_ulJoystickFlag == 3)
	{
		m_lBinUpperLeftX = ConvertXEncoderValueForDisplay(m_lEnc_X);
		m_lBinUpperLeftY = ConvertYEncoderValueForDisplay(m_lEnc_Y);
	}
	else if (m_ulJoystickFlag == 4)
	{
		m_lBinLowerRightX = ConvertXEncoderValueForDisplay(m_lEnc_X);
		m_lBinLowerRightY = ConvertYEncoderValueForDisplay(m_lEnc_Y);
	}

	return TRUE;
} //end SaveEncoderValue


VOID CBinTable::GetAxisInformation()	//NuMotion
{
#ifdef NU_MOTION

	InitAxisData(m_stBTAxis_X);
	m_stBTAxis_X.m_szName				= BT_AXIS_X;
	m_stBTAxis_X.m_szTag				= MS896A_CFG_CH_BINTABLE_X;
	m_stBTAxis_X.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBTAxis_X);
	m_stBTAxis_X.m_dEncResolution		= 1;

	InitAxisData(m_stBTAxis_Y);
	m_stBTAxis_Y.m_szName				= BT_AXIS_Y;
	m_stBTAxis_Y.m_szTag				= MS896A_CFG_CH_BINTABLE_Y;
	m_stBTAxis_Y.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBTAxis_Y);
	m_stBTAxis_Y.m_dEncResolution		= 1;

	PrintAxisData(m_stBTAxis_X);
	PrintAxisData(m_stBTAxis_Y);

	//9INCH_MS		//v4.16T1
	BOOL bUseBT2 = (BOOL) GetChannelInformation(MS896A_CFG_CH_BINTABLE2_Y, MS896A_CFG_CH_ENABLE);
	if (bUseBT2)
	{
		m_ulMachineType = BT_MACHTYPE_DTABLE;

		InitAxisData(m_stBTAxis_X2);
		m_stBTAxis_X2.m_szName				= BT_AXIS_X2;
		m_stBTAxis_X2.m_szTag				= MS896A_CFG_CH_BINTABLE2_X;
		m_stBTAxis_X2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBTAxis_X2);
		m_stBTAxis_X2.m_dEncResolution		= 1;

		InitAxisData(m_stBTAxis_Y2);
		m_stBTAxis_Y2.m_szName				= BT_AXIS_Y2;
		m_stBTAxis_Y2.m_szTag				= MS896A_CFG_CH_BINTABLE2_Y;
		m_stBTAxis_Y2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBTAxis_Y2);
		m_stBTAxis_Y2.m_dEncResolution		= 1;

		PrintAxisData(m_stBTAxis_X2);
		PrintAxisData(m_stBTAxis_Y2);
	}

	if (m_bEnable_T)	//v4.39T7	//MS100 Nichia
	{
		InitAxisData(m_stBTAxis_T);
		m_stBTAxis_T.m_szName				= BT_AXIS_T;
		m_stBTAxis_T.m_szTag				= MS896A_CFG_CH_BINTABLE_T;
		m_stBTAxis_T.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBTAxis_T);
		m_stBTAxis_T.m_dEncResolution		= 1;
		PrintAxisData(m_stBTAxis_T);
	}

#endif

}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CBinTable::UpdateOutput()
{
	static int nCount = 0;
	CString szAxis;

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
		return;

	if (nCount++ < 15)
	{
		return;
	}
	else
	{
		nCount = 0;
	}

	try
	{
		if (m_fHardware)
		{
			//if (m_szStationName == "BinTableStn")
			if (m_fHardware && !m_bDisableBT)		//v4.59A45
			{
				GetSensorValue();
				GetEncoderValue();
				SaveEncoderValue();		//joystick

				szAxis = BT_AXIS_X;
				m_bIsPowerOn_X = CMS896AStn::MotionIsPowerOn(BT_AXIS_X, &m_stBTAxis_X);
				szAxis = BT_AXIS_Y;
				m_bIsPowerOn_Y = CMS896AStn::MotionIsPowerOn(BT_AXIS_Y, &m_stBTAxis_Y);

				if (m_bEnable_T)			//v4.59A45
				{
					szAxis = BT_AXIS_T;
					m_bIsPowerOn_T = CMS896AStn::MotionIsPowerOn(BT_AXIS_T, &m_stBTAxis_T);
				}

			}
		}
	}
	catch(CAsmException e)
	{
		DisplayException(e); 
		if (szAxis == BT_AXIS_X)
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
		else if (szAxis == BT_AXIS_X2)
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
		else if (szAxis == BT_AXIS_Y2)
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
		else
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
	}
}

VOID CBinTable::UpdateProfile()
{
	//Add Your Code Here
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachine);

	m_bUpdateProfile = FALSE;

	try
	{
/*
		UpdateSearchProfile("BinTableXAxis", "spfBinTableXHome");
		UpdateMotionProfile("BinTableXAxis", "mpfBinTableXNormal");
		UpdateSearchProfile("BinTableYAxis", "spfBinTableYHome");
		UpdateMotionProfile("BinTableYAxis", "mpfBinTableYNormal");
*/
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
	slLock.Unlock();
}

VOID CBinTable::UpdatePosition()
{
	//Add Your Code Here
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csDevice);
	
	m_bUpdatePosition = FALSE;

	try
	{
//		m_bSelBinTable	= (LONG) (*m_psmfDevice)["Select"]["BinTable"];
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	slLock.Unlock();
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////

VOID CBinTable::IdleOperation()
{
	switch (Command())
	{
	case glAMS_NULL_COMMAND:
		Sleep(20);
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

void CBinTable::DiagOperation()
{
	m_nLastError	= gnOK;
	
	LONG	lAxis, lAction, lUnit;
	AxisAction(lAxis, lAction, lUnit);
	//SFM_CHipecAcServo*		pServo;

	try
	{
		CString	szAxisID = "";
		
		if (m_fHardware)
		{
			szAxisID = GetAxisID(lAxis);
		}

		switch (lAction)
		{
		case glPOSITION_ACTION:
			switch (lAxis)
			{
			case 0:
				if (lUnit == HOME_POS)
				{
					SetInitHome(szAxisID, FALSE);
					m_nLastError = X_Home();
				}
				break;

			case 1:
				if (lUnit == HOME_POS)
				{
					SetInitHome(szAxisID, FALSE);
					m_nLastError = Y_Home();
				}
				break;

			case 3:
				if (lUnit == HOME_POS)
				{
					m_nLastError = XY_Home();
				}
				break;
			}
			break;

		case glRELATIVE_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = X_Move(lUnit);
				break;

			case 1:
				m_nLastError = Y_Move(lUnit);
				break;
			}
			break;

		case glABSOLUTE_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = X_MoveTo(lUnit);
				break;

			case 1:
				m_nLastError = Y_MoveTo(lUnit);
				break;
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
/*
					if (lAxis == 0)
					{					
						m_pServo_X->PowerOff();
					}
					else
					{
						m_pServo_Y->PowerOff();
					}
*/
					PowerOffAxis(szAxisID);
				}
				else
				{
/*
					if (lAxis == 0)
					{
						m_pServo_X->PowerOn();
					}
					else
					{
						m_pServo_Y->PowerOn();
					}
*/
					PowerOnAxis(szAxisID);
				}
			}
			break;

		case 88:
			INT i;
			for ( i=0; i < 10; i++ )
			{
				X_Move(lUnit, SFM_NOWAIT);
				Y_Move(lUnit, SFM_WAIT);
				//pServo	= GetHipecAcServo("srvBinTableX");
				//pServo->Synchronize(15000);
				try
				{
					CMS896AStn::MotionSync("BinTableXAxis", 15000, &m_stBTAxis_X);
				}
				catch (CAsmException e)
				{
				}

				X_Move(-lUnit, SFM_NOWAIT);
				Y_Move(-lUnit, SFM_WAIT);
				//pServo	= GetHipecAcServo("srvBinTableX");
				//pServo->Synchronize(15000);
				try
				{
					CMS896AStn::MotionSync("BinTableXAxis", 15000, &m_stBTAxis_X);
				}
				catch (CAsmException e)
				{
				}
			}			
			break;

		case glSELECT_PROFILE_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = X_Profile(lUnit);
				break;

			case 1:
				m_nLastError = Y_Profile(lUnit);
				break;
			}
			break;

		case glJOY_STICK_ACTION:
			if (m_fHardware)
			{
				if (lUnit == 0)
				{
					SetJoystickOn(FALSE);
				}
				else
				{
					SetJoystickOn(TRUE);
				}
			}
			break;
/*
		case glSELECT_JOY_PRF_ACTION:
			switch (lAxis)
			{
			case 0:
				m_nLastError = XJoyProfile(lUnit);
				break;

			case 1:
				m_nLastError = YJoyProfile(lUnit);
				break;
			}
			break;
*/
		case glCOMMUTATE_ACTION:
			if (m_fHardware)
			{
				m_nLastError = CommutateServo(szAxisID);

				switch (lAxis)
				{
				case 0:
					if (m_nLastError == gnOK)
					{
						m_bComm_X			= TRUE;
					}
					else
					{
						m_bComm_X			= FALSE;
						m_nLastError		= Err_BinTableXCommutate;
					}
					break;

				case 1:
					if (m_nLastError == gnOK)
					{
						m_bComm_Y			= TRUE;
					}
					else
					{
						m_bComm_Y			= FALSE;
						m_nLastError		= Err_BinTableYCommutate;
					}
					break;
				}
			}
			break;

		case glCLEAR_SERVO_ACTION:
			if (m_fHardware)
			{
				ClearServoError(szAxisID);
			}
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

VOID CBinTable::InitOperation()
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

		DisplaySequence("BT - Initialize Operation Completed");
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

VOID CBinTable::PreStartOperation()
{
	m_nLastError	= gnOK;

	try
	{
		m_nLastError = OpPreStart();

#ifdef NU_MOTION
		if (m_bEnableBTMotionLog)
		{
			LogAxisPerformance2(BT_AXIS_X, BT_AXIS_Y, &m_stBTAxis_X, &m_stBTAxis_Y, TRUE);
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

		//v4.44A4
		CMSLogFileUtility::Instance()->BT_BackupReAlignBinFrameLog();
		CMSLogFileUtility::Instance()->BT_BackupTableIndexLog();
		CMSLogFileUtility::Instance()->BT_BackupGenTempLog();
		CMSLogFileUtility::Instance()->BT_BackupLogStatus();
		CMSLogFileUtility::Instance()->BT_BackupOptimizeGradeLog();
		CMSLogFileUtility::Instance()->BT_BackupDLALogStatus();
		CMSLogFileUtility::Instance()->BT_BackupClearBinCounterLog();
		CMSLogFileUtility::Instance()->BT_BackupExchangeFrameLog();	
		CMSLogFileUtility::Instance()->BT_BackupBinRemaingCountLog();
		CMSLogFileUtility::Instance()->BT_BackupBinSerialLog();
		CMSLogFileUtility::Instance()->BT_BackupPostBondLog();
		CMSLogFileUtility::Instance()->BT_BackupThetaCorrectionLog();

		DisplaySequence("BT - PreStartOperation Completed");
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

VOID CBinTable::StopOperation()
{
	if (m_fHardware && !m_bDisableBT && m_bStopAllMotion == FALSE)	//v3.60
	{
		try
		{
			CMS896AStn::MotionSync("BinTableXAxis", 10000, &m_stBTAxis_X);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult("BinTableXAxis", &m_stBTAxis_X);
		}

		try
		{
			CMS896AStn::MotionSync("BinTableYAxis", 10000, &m_stBTAxis_Y);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult("BinTableYAxis", &m_stBTAxis_Y);
		}
	}

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

//	XY_Home();

	State(IDLE_Q);
	Motion(FALSE);

	Signal(gnSTATION_COMPLETE);
	LogCycleStopState("BT - Signal Complete");

#ifdef NU_MOTION
	if (m_bEnableBTMotionLog)
	{
		LogAxisPerformance2(BT_AXIS_X, BT_AXIS_Y, &m_stBTAxis_X, &m_stBTAxis_Y, FALSE);
	}
#endif

	m_qSubOperation = 0;
	CMS896AStn::m_bBTAskBLChangeGrade	= FALSE;	//v4.43T9
	CMS896AStn::m_bBTAskBLBinFull		= FALSE;	//v4.43T9

	BTLog_SaveRecordToFile();		// Save the log information to file
}
