/////////////////////////////////////////////////////////////////
// BondHead.cpp : interface of the CBondHead class
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
#include "BH_AirFlowCompensation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBondHead, CMS896AStn)

CBondHead::CBondHead() : m_evPRLatched(FALSE, TRUE,			"WprLatchedEvt"),
						 m_evPRStart(FALSE, TRUE,			"WprStartEvt"),
     					 m_evBadDieForT(FALSE, TRUE,		"WprBadDieForTEvt"),
						 m_evEjectorReady(FALSE, TRUE,		"EjReadyEvt"), 
 						 m_evBTReady(FALSE, TRUE,			"BtReadyEvt"),
						 m_evBhTReady(FALSE, TRUE,			"BhTReadyEvt"),
						 m_evBhTReadyForBPR(FALSE, TRUE,	"BhTReadyForBPREvt"),		//v2.60
						 m_evBhTReadyForWPR(FALSE, TRUE,	"BhTReadyForWPREvt"),		//v3.34
						 m_evBhTReadyForWT(FALSE, TRUE,		"BhTReadyForWTEvt"),		//v4.46T22
						 m_evDiePicked(FALSE, TRUE,			"BhDiePickedEvt"),
						 m_evDieBonded(FALSE, TRUE,			"BhDieBondedEvt"),
						 m_evDieBondedForWT(FALSE, TRUE,	"BhDieBondedForWTEvt"),
						 m_evWTReady(FALSE, TRUE,			"WtReadyEvt"),
						 m_evWTStartMove(FALSE, TRUE,		"WtStartMoveEvt"),			//v3.66		//DBH only
						 m_evCompDone(FALSE, TRUE,			"CompDoneEvt"),				//v4.34T10
						 m_evMoveBack(FALSE, TRUE,			"WtMoveBackEvt"),
						 m_evWTBacked(FALSE, TRUE,			"WtBackedEvt"),
   					     m_evConfirmSrch(FALSE, TRUE,		"WtConfirmSrchEvt"),
						 m_evBhToPrePick(FALSE, TRUE,		"BhToPrePickEvt"),
						 m_evBHInit(FALSE, TRUE,			"BhInitEvt"),
						 m_evBTStartMoveWithoutDelay(FALSE, TRUE,		"BtStartMoveWithoutDelayEvt") ,
 						 m_evBTStartMove(FALSE, TRUE,		"BtStartMoveEvt") ,
						 m_evExpInit(FALSE, TRUE,			"ExpInitTEvt"),
						 m_evBackupNVRam(FALSE, TRUE,		"BackupNVRam"),
						 m_evBLFrameLevelDn(FALSE, TRUE,	"BlFrameLevelDn"),			//v4.01
						 m_evBTReSrchDie(FALSE, TRUE,		"BTReSchDie"),				//v4.xx
						 m_evBPRPostBondDone(FALSE, TRUE,	"BPRPostBondDone"),			//v4.40T6
						 m_evBTCompensate(FALSE, TRUE,		"BTCompensate"),			//v4.52A16
						 m_evAutoChangeColletDone(FALSE, TRUE,	"AutoChangeColletDone"),
						 m_evSearchMarkReady(FALSE, TRUE,	"SearchMarkReady"),
						 m_evSearchMarkDone(FALSE, TRUE,	"SearchMarkDone"),
						 m_evBhReadyForWPRHwTrigger(FALSE, TRUE, "BhReadyForWPRHwTrigger"),
						 m_evBhReadyForBPRHwTrigger(FALSE, TRUE, "BhReadyForBPRHwTrigger")
{
	m_szEventPrefix				= "BH";
	InitVariable();
}

CBondHead::~CBondHead()
{
	if (m_pThermalCtrl != NULL)
	{
		delete m_pThermalCtrl;
	}

	if (m_pBHZ1AirFlowCompensation != NULL)
	{
		delete m_pBHZ1AirFlowCompensation;
	}

	if (m_pBHZ2AirFlowCompensation != NULL)
	{
		delete m_pBHZ2AirFlowCompensation;
	}
}

BOOL CBondHead::InitInstance()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	LONG lDynamicControl = 0;
	LONG lStaticControl = 0;
	LONG lVelFilterGain = 0;
	LONG lVelFilterWeight = 0;
	LONG lMotorDirectionE = 0;
	LONG lMotorDirectionBinE = 0;

	m_lBondHeadToleranceLevel	= 500;
	m_lZPBLevelOffsetLimit		= 0;
	m_lBondHead1LearnLevelBackup = 0;
	m_lBondHead2LearnLevelBackup = 0;

	m_lMS60TempCheckCounter	= 0;
	
	CMS896AStn::InitInstance();
	m_pulStorage = NULL;

	//Check Front & Side cover is exist or not
	m_bIsFrontCoverExist	= CMS896AApp::m_lMainCoverLock;
	m_bIsSideCoverExist		= CMS896AApp::m_lSideCoverLock;
	m_bIsBinElevatorCoverExist = CMS896AApp::m_lBinElevatorCoverLock;

	//m_bIsFrontCoverExist	= TRUE;
	//m_bIsSideCoverExist		= TRUE;
	//m_bIsBinElevatorCoverExist = TRUE;

	m_bSel_Z2			= FALSE;
	m_bIsArm2Exist		= FALSE;
	m_bNuMotionSystem	= TRUE;

	if (CMS896AApp::m_bIsNuMotionSingleArmMachine == TRUE)
	{
		m_bSel_Z2		= FALSE;
		m_bIsArm2Exist	= FALSE;
	}

	m_bMS100EjtXY				= (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X,		MS896A_CFG_CH_ENABLE);
	m_bMS100DigitalAirFlowSnr	= (BOOL) GetChannelInformation(MS896A_CFG_CH_BONDHEAD,		"Use Digital MD");
	m_bMS60EjElevator			= (BOOL) GetChannelInformation(MS896A_CFG_CH_EJ_ELEVATOR,	MS896A_CFG_CH_ENABLE);
	m_bMS50ChgColletZT			= (BOOL) GetChannelInformation(MS896A_CFG_CH_CHGCOLLET_T,	MS896A_CFG_CH_ENABLE);
	m_bEnableEjectorTheta		= (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_T,		MS896A_CFG_CH_ENABLE);
	m_bEnableBinEjector			= (BOOL) GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR,	MS896A_CFG_CH_ENABLE);

	m_pBHZ1AirFlowCompensation = new AD_CBHAirFlowCompensation;
	m_pBHZ2AirFlowCompensation = new AD_CBHAirFlowCompensation;

	//for debug only
	PrintNuConfigData();

	//For Reset Collet and Ejector Count OutputFile only
	CreateResetColletEjectorCountOutputFileSelectionFolder();

	try
	{
		if (m_bDisableBH)
		{
			m_bSel_Ej		= FALSE;
			m_bSel_BinEj	= FALSE;
		}
		
		GetAxisInformation();	
		InitMinMachineDelays();

		if (m_fHardware && (!m_bDisableBH || m_bSel_Ej))
		{
			//Update Profile Time
			//m_lT_ProfileType		= GetChannelInformation(MS896A_CFG_CH_BONDARM,		MS896A_CFG_CH_PROFILE_TYPE);				
			//m_lZ_ProfileType		= GetChannelInformation(MS896A_CFG_CH_BONDHEAD,		MS896A_CFG_CH_PROFILE_TYPE);
			m_lE_ProfileType		= GetChannelInformation(MS896A_CFG_CH_EJECTOR,			MS896A_CFG_CH_PROFILE_TYPE);			//Linear Ej
			m_lECap_ProfileType		= GetChannelInformation(MS896A_CFG_CH_EJECTOR_CAP,		MS896A_CFG_CH_PROFILE_TYPE);
			m_lET_ProfileType		= GetChannelInformation(MS896A_CFG_CH_EJECTOR_T,		MS896A_CFG_CH_PROFILE_TYPE);
			
			m_lBinE_ProfileType		= GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR,		MS896A_CFG_CH_PROFILE_TYPE);			//Linear Ej
			m_lBinECap_ProfileType	= GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR_CAP,	MS896A_CFG_CH_PROFILE_TYPE);
			m_lBinET_ProfileType	= GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR_T,	MS896A_CFG_CH_PROFILE_TYPE);

			//m_lT_CommMethod		= GetChannelInformation(MS896A_CFG_CH_BONDARM,		MS896A_CFG_CH_SWCOMM);								
			//m_lZ_CommMethod		= GetChannelInformation(MS896A_CFG_CH_BONDHEAD,		MS896A_CFG_CH_SWCOMM);	
			m_lE_CommMethod			= GetChannelInformation(MS896A_CFG_CH_EJECTOR,		MS896A_CFG_CH_SWCOMM);	
			m_lBinE_CommMethod		= GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR,	MS896A_CFG_CH_SWCOMM);					
			lMotorDirectionE		= GetChannelInformation(MS896A_CFG_CH_EJECTOR,		MS896A_CFG_CH_MOTOR_DIRECTION);
			lMotorDirectionBinE		= GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR,	MS896A_CFG_CH_MOTOR_DIRECTION);								
	
			if (CMS896AStn::MotionIsServo(BH_AXIS_EJ, &m_stBHAxis_Ej))
			{
				// Reverse motor direction if needed
				if (lMotorDirectionE == -1)
				{
					CMS896AStn::MotionSetEncoderDirection(BH_AXIS_EJ, HP_NEGATIVE_DIR, &m_stBHAxis_Ej);
					CMS896AStn::MotionReverseMotorDirection(BH_AXIS_EJ, HP_ENABLE, &m_stBHAxis_Ej);
				}

				lDynamicControl		= GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_DYNA_CONTROL);				
				lStaticControl		= GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_STAT_CONTROL);	

				if (lDynamicControl != 0)
				{
					CMS896AStn::MotionSelectControlType(BH_AXIS_EJ, lDynamicControl, 2, &m_stBHAxis_Ej);
				}

				if (lStaticControl != 0)
				{
					CMS896AStn::MotionSelectControlType(BH_AXIS_EJ, lStaticControl, 1, &m_stBHAxis_Ej);
				}
			}
				
			if (CMS896AStn::MotionIsServo(MS896A_CFG_CH_BIN_EJECTOR, &m_stBHAxis_BinEj))
			{
				// Reverse motor direction if needed
				if (lMotorDirectionBinE == -1)
				{
					CMS896AStn::MotionSetEncoderDirection(MS896A_CFG_CH_BIN_EJECTOR, HP_NEGATIVE_DIR, &m_stBHAxis_BinEj);
					CMS896AStn::MotionReverseMotorDirection(MS896A_CFG_CH_BIN_EJECTOR, HP_ENABLE, &m_stBHAxis_BinEj);
				}

				lDynamicControl		= GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_DYNA_CONTROL);				
				lStaticControl		= GetChannelInformation(MS896A_CFG_CH_BIN_EJECTOR, MS896A_CFG_CH_STAT_CONTROL);	

				if (lDynamicControl != 0)
				{
					CMS896AStn::MotionSelectControlType(MS896A_CFG_CH_BIN_EJECTOR, lDynamicControl, 2, &m_stBHAxis_BinEj);
				}

				if (lStaticControl != 0)
				{
					CMS896AStn::MotionSelectControlType(MS896A_CFG_CH_BIN_EJECTOR, lStaticControl, 1, &m_stBHAxis_BinEj);
				}
			}

			//Z_Profile(NORMAL_PROF);
			//T_Profile(NORMAL_PROF);
			Ej_Profile(NORMAL_PROF);

			//SetSuckingHead(TRUE);	//v4.05
			//TurnOnBlowAndCatchModule(FALSE);	//v4.59A13
			//{
			/*if ( (Z_Home() != gnOK) || !m_bHome_Z)
			{
				Sleep(2000);
				SetErrorMessage("Init Z_HOME retry");
				DisplayMessage("Z_HOME retry");
				Z_Home();
			}
			if ( (Z2_Home() != gnOK) || !m_bHome_Z2)
			{
				Sleep(2000);
				SetErrorMessage("Init Z2_HOME retry");
				DisplayMessage("Z2_HOME retry");
				Z2_Home();
			}*/
			//}

			if (!m_bEnableEjectorTheta && m_bMS60EjElevator)	//1st test Jig only
			{
				DisplayMessage("Init MS50 Ej Elevator ...");
				//EjElevator_Home();

				//v4.54A1
				m_lStandby_EJ_Cap = pApp->GetProfileInt(gszPROFILE_SETTING, "EjCap Standby Z", 500);

				CString szLog;
				szLog.Format("BH EjCap STANDBY Z retrieved = %ld", m_lStandby_EJ_Cap);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

				//EjElevator_MoveTo(m_lStandby_EJ_Elevator);
			}

			if (m_bEnableEjectorTheta)		//2nd Test Jig
			{
				if (!HomeEjModule())
				{
					SetErrorMessage("BondHeadStn INIT ERROR - EJ MODULE HOME failure!");
					DisplayMessage("BondHeadStn INIT ERROR - EJ MODULE HOME failure!");
					return TRUE;
				}
				if (!HomeBinEjModule())
				{
					SetErrorMessage("BondHeadStn INIT ERROR - BIN EJ MODULE HOME failure!");
					DisplayMessage("BondHeadStn INIT ERROR - BIN EJ MODULE HOME failure!");
					return TRUE;
				}
			}

			//if (!IsMS60EjtHome())	//v4.55A6
			//{
			//	SetErrorMessage("BondHeadStn INIT ERROR - EJ HOME failure!");
			//	DisplayMessage("BondHeadStn INIT ERROR - EJ HOME failure!");
			//	return TRUE;
			//}

			//InitEjTableXY();

			//if (IsLowPressure())	//v2.56
			//{
			//	DisplayMessage("Warning - machine is low in pressure!");
			//}
				
			//if (IsEnaAutoChangeCollet() && !IsColletPusherAtSafePos())	//v4.52A1
			//{
			//	DisplayMessage("BondHeadStn ERROR - AGC PusherZ at DOWN position! ");
			//	return TRUE;
			//}

			SetBHInit(TRUE);						// BondHead init is ready

			//if (IsEnableWL() && (WaitExpInit(90000) == FALSE) )		//v4.56A1
			//{
			//	DisplayMessage("BondHeadStn - Timeout when wait expander init ");
			//	return TRUE;
			//}

			//T_Home(TRUE);
			
			//SetSuckingHead(FALSE);	//v4.05

			//if (CMS896AStn::m_bAutoChangeCollet)
			//{
			//	SetChgColletUploadPusher(FALSE);
			//	SetChgColletHolderVacuum(FALSE);
			//}

			//v4.59A44	//MS50 AutoChgCollet module
			//if(m_bMS50ChgColletZT)
			//{
			//	MS50ChgColletT_Home();
			//	MS50ChgColletZ_Home();
			//	//m_lAGCColletIndex = (*m_psmfSRam)["BinTable"]["AGC"]["ColletIndex"];
			//	MoveChgColletTPitch(m_lAGCColletIndex);
			//}

			CMS896AStn::MotionSetLastWarning(BH_AXIS_EJ,				HP_SUCCESS, &m_stBHAxis_Ej);
			CMS896AStn::MotionSetLastWarning(BH_AXIS_BIN_EJ,			HP_SUCCESS, &m_stBHAxis_BinEj);
			CMS896AStn::MotionSetLastWarning(BH_AXIS_EJ_T,				HP_SUCCESS, &m_stBHAxis_EjT);
			CMS896AStn::MotionSetLastWarning(BH_AXIS_BIN_EJ_T,			HP_SUCCESS, &m_stBHAxis_BinEjT);
			CMS896AStn::MotionSetLastWarning(BH_AXIS_EJCAP,				HP_SUCCESS, &m_stBHAxis_EjCap);
			CMS896AStn::MotionSetLastWarning(BH_AXIS_BIN_EJCAP,			HP_SUCCESS, &m_stBHAxis_BinEjCap);
			CMS896AStn::MotionSetLastWarning(BH_AXIS_EJELEVATOR,		HP_SUCCESS, &m_stBHAxis_EjElevator);
			CMS896AStn::MotionSetLastWarning(BH_AXIS_BIN_EJELEVATOR,	HP_SUCCESS, &m_stBHAxis_BinEjElevator);
		}
		else
		{
			SetBHInit(TRUE);			
		}

		m_bHardwareReady = TRUE;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}


	LARGE_INTEGER lnFreq;
	QueryPerformanceFrequency(&lnFreq);
	m_dFreqClk = (double) 1000 / lnFreq.QuadPart;

	m_WaferMapEvent.SetBondHead(this);
	return TRUE;
}


INT CBondHead::ExitInstance()
{
	if (m_fHardware)
	{
		if (m_bSel_Ej)
		{
			try 
			{
				CMS896AStn::MotionSync(BH_AXIS_EJ, 10000, &m_stBHAxis_Ej);
				CMS896AStn::MotionPowerOff(BH_AXIS_EJ, &m_stBHAxis_Ej);
				CMS896AStn::MotionClearError(BH_AXIS_EJ, HP_MOTION_ABORT, &m_stBHAxis_Ej);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}

			if (m_bEnableEjectorTheta)
			{
				try 
				{
					CMS896AStn::MotionSync(BH_AXIS_EJ_T, 10000, &m_stBHAxis_EjT);
					CMS896AStn::MotionPowerOff(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
					CMS896AStn::MotionClearError(BH_AXIS_EJ_T, HP_MOTION_ABORT, &m_stBHAxis_EjT);
				}
				catch (CAsmException e)
				{
					// Nothing can do ...
				}
			}

			if (m_bMS60EjElevator)
			{
				try 
				{
					CMS896AStn::MotionSync(BH_AXIS_EJELEVATOR, 10000, &m_stBHAxis_EjElevator);
					CMS896AStn::MotionPowerOff(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
					CMS896AStn::MotionClearError(BH_AXIS_EJELEVATOR, HP_MOTION_ABORT, &m_stBHAxis_EjElevator);
				}
				catch (CAsmException e)
				{
					// Nothing can do ...
				}
			}

			try 
			{
				CMS896AStn::MotionSync(BH_AXIS_EJCAP, 10000, &m_stBHAxis_EjCap);
				CMS896AStn::MotionPowerOff(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
				CMS896AStn::MotionClearError(BH_AXIS_EJCAP, HP_MOTION_ABORT, &m_stBHAxis_EjCap);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}

		if (m_bEnableBinEjector && m_bSel_BinEj)
		{
			try 
			{
				CMS896AStn::MotionSync(BH_AXIS_BIN_EJ, 10000,	&m_stBHAxis_BinEj);
				CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJ,		&m_stBHAxis_BinEj);
				CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ, HP_MOTION_ABORT, &m_stBHAxis_BinEj);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}

			if (m_bEnableEjectorTheta)
			{
				try 
				{
					CMS896AStn::MotionSync(BH_AXIS_BIN_EJ_T, 10000, &m_stBHAxis_BinEjT);
					CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
					CMS896AStn::MotionClearError(BH_AXIS_BIN_EJ_T, HP_MOTION_ABORT, &m_stBHAxis_BinEjT);
				}
				catch (CAsmException e)
				{
					// Nothing can do ...
				}
			}

			if (m_bMS60EjElevator)
			{
				try 
				{
					CMS896AStn::MotionSync(BH_AXIS_BIN_EJELEVATOR, 10000, &m_stBHAxis_BinEjElevator);
					CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJELEVATOR, &m_stBHAxis_BinEjElevator);
					CMS896AStn::MotionClearError(BH_AXIS_BIN_EJELEVATOR, HP_MOTION_ABORT, &m_stBHAxis_BinEjElevator);
				}
				catch (CAsmException e)
				{
					// Nothing can do ...
				}
			}

			try 
			{
				CMS896AStn::MotionSync(BH_AXIS_BIN_EJCAP, 10000, &m_stBHAxis_BinEjCap);
				CMS896AStn::MotionPowerOff(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
				CMS896AStn::MotionClearError(BH_AXIS_BIN_EJCAP, HP_MOTION_ABORT, &m_stBHAxis_BinEjCap);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}

	}

	return CMS896AStn::ExitInstance();
}


BOOL CBondHead::InitData()
{
	//string map file
	CMS896AApp* pApp = dynamic_cast<CMS896AApp*> (m_pModule);

	if (IsLoadingPortablePKGFile() == TRUE && IsLoadingPKGFile() == TRUE)
	{
		LoadBhOption(TRUE);
		SaveBhData();	
	}
	else
	{
		//InitNichiaSubSystem();
		LoadBhOption();
		LoadBhData();
	}
	
	(*m_psmfSRam)["WaferTable"]["PRDelay"]			= m_lPRDelay;
	(*m_psmfSRam)["WaferTable"]["VacOffDelay"]		= m_lEjVacOffDelay;	
	(*m_psmfSRam)["WaferTable"]["TDelay"]			= m_lWTTDelay;	
	(*m_psmfSRam)["WaferTable"]["SettlingDelay"]	= m_lWTSettlingDelay;

	try
	{
		if (m_fHardware && CMS896AApp::m_bRuntimeLoadPPKG==FALSE)
		{
			// Check whether the expander is open or not
			// Move Bond Head to Pre Pick Pos if the expander is open
		
			if ( IsWLExpanderOpen() == TRUE )
			{
				SetErrorMessage("Expander not closed");
			}
			else
			{				
				if (CMS896AStn::m_bDBHHeatingCoilFcn)
				{	
#ifdef NU_MOTION_MS60		//v4.50A16	//compatible with MS100Plus2/3
					CString szMsg;

					m_lPrePickPos_T = (m_lPickPos_T + m_lBondPos_T)/2;

					szMsg.Format("Pick Position:%d, Bond Position:%d, Prepick:%d", m_lPickPos_T,m_lBondPos_T,m_lPrePickPos_T);
					//T_MoveTo(m_lPrePickPos_T);

					m_lBPGeneral_1 = m_lPrePickPos_T;
					//T_MoveTo(m_lPrePickPos_T);
#endif
				}
				else
				{
					//if (!m_bDisableBH)		//v3.60
						//T_MoveTo(m_lPrePickPos_T);		// Move to pre-pick
				}
			}
			
			if (m_bMS100EjtXY)	//v4.22T3
			{
				if ( (m_lCDiePos_EjX != 0) && (m_lCDiePos_EjY != 0) )
				{
					//if (EjX_IsPowerOn())
						//EjX_MoveTo(m_lCDiePos_EjX);
					//if (EjY_IsPowerOn())
						//EjY_MoveTo(m_lCDiePos_EjY);
				}
			}

			if (m_bMS60EjElevator)	//v4.53A30
			{
				//EjElevator_MoveTo(m_lStandby_EJ_Elevator);
			}

			//v4.39T7	//Update ProfileTime.txt at sw startup->WarmStart
			//Z_Profile(NORMAL_PROF);
			//T_Profile(NORMAL_PROF);
			Ej_Profile(NORMAL_PROF);
			//PBTZ_Profile(NORMAL_PROF);
			LogProfileTime();

			//c4.48A12
			if (IsMS60())
			{
				//if (T_IsPowerOn())
				//{
				//	InitMS60ThermalCtrl();
				//}

				//m_bMS60DetectBHFan = GetChannelInformation(MS896A_CFG_CH_BONDARM, "MS60 BH Fan Control");				
			}
		}

		if (m_fHardware )	// Get the address of NVRAM for BondHeadStn
		{
			if ( m_pvNVRAM != NULL )
			{
				m_pulStorage		= (ULONG*)((unsigned long)m_pvNVRAM + glNVRAM_BondHead_Start);
				m_ulColletCount		= *(m_pulStorage + BH_COLLETCOUNT_OFFSET);
				m_ulCollet2Count	= *(m_pulStorage + BH_COLLET2COUNT_OFFSET);		//v3.92
				m_ulCleanCount		= *(m_pulStorage + BH_CLEANCOUNT_OFFSET);
				m_ulEjectorCount	= *(m_pulStorage + BH_EJECTCOUNT_OFFSET);
				m_ulMissingDieCount = *(m_pulStorage + BH_MISSINGDIE_OFFSET);
				m_ulProbePinCleanCount = *(m_pulStorage + BH_PROBEPINCOUNT_OFFSET);
				m_ulProbePinCount = *(m_pulStorage + BH_PROBEPINCLEANCOUNT_OFFSET);

				m_ulPickDieCount = *(m_pulStorage + BH_PICKDIE_OFFSET);
				m_ulBondDieCount = *(m_pulStorage + BH_BONDDIE_OFFSET);
				m_ulDefectDieCount = *(m_pulStorage + BH_DEFECTDIE_OFFSET);
				m_ulBadCutDieCount = *(m_pulStorage + BH_BADCUTDIE_OFFSET);
				m_ulRotateDieCount = *(m_pulStorage + BH_ROTATEDIE_OFFSET);
				m_ulEmptyDieCount = *(m_pulStorage + BH_EMPTYDIE_OFFSET);

				m_ulCurrWaferBondDieCount	= *(m_pulStorage + BH_CURWAF_BONDEDDIE_OFFSET);
				m_ulCurrWaferDefectDieCount	= *(m_pulStorage + BH_CURWAF_DEFECTDIE_OFFSET);
				m_ulCurrWaferChipDieCount	= *(m_pulStorage + BH_CURWAF_CHIPDIE_OFFSET);
				m_ulCurrWaferInkDieCount	= *(m_pulStorage + BH_CURWAF_INKDIE_OFFSET);
				m_ulCurrWaferBadCutDieCount	= *(m_pulStorage + BH_CURWAF_BADCUTDIE_OFFSET);
				m_ulCurrWaferRotateDieCount	= *(m_pulStorage + BH_CURWAF_ROTATEDIE_OFFSET);
				m_ulCurrWaferEmptyDieCount	= *(m_pulStorage + BH_CURWAF_EMPTYDIE_OFFSET);
				m_ulCurrWaferMissingDieCount = *(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET);

				m_ulCustomCounter1 = *(m_pulStorage + BH_CUSTOM_COUNT_1);
				m_ulCustomCounter2 = *(m_pulStorage + BH_CUSTOM_COUNT_2);
				m_ulCustomCounter3 = *(m_pulStorage + BH_CUSTOM_COUNT_3);

				//(*m_psmfSRam)["BondHead"]["CurrentWafer"]["MissingDie"] = m_ulCurrWaferMissingDieCount;
			}

		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
	}

	m_lAutoLearnLevelsIdleTime = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Auto Learn Level Idle Time"), 0);
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("Auto Learn Level Idle Time"), m_lAutoLearnLevelsIdleTime);
	
	//v4.59A25	//Allow user to ENABLE/DISABLE in HMI
	//m_bCheckEjPressureSnr = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Ej Pressure Check"), 0);	//v4.59A17
	
	CMSLogFileUtility::Instance()->BH_BackupDieCounterLog();
	CMSLogFileUtility::Instance()->BH_BackupLogStatus();		//v4.50A6		
	return TRUE;
}

VOID CBondHead::ClearData()
{
	if ( m_pvNVRAM != NULL )
	{
		ULONG *pulTemp = (ULONG*)((unsigned long)m_pvNVRAM + glNVRAM_BondHead_Start);
		// Keep the Collet and Ejector count
		*(pulTemp + BH_PICKDIE_OFFSET)		= 0;
		*(pulTemp + BH_BONDDIE_OFFSET)		= 0;
		*(pulTemp + BH_DEFECTDIE_OFFSET)	= 0;
		*(pulTemp + BH_BADCUTDIE_OFFSET)	= 0;
		*(pulTemp + BH_MISSINGDIE_OFFSET)	= 0;
		*(pulTemp + BH_ROTATEDIE_OFFSET)	= 0;
		*(pulTemp + BH_EMPTYDIE_OFFSET)		= 0;

		//Current wafer
		*(pulTemp + BH_CURWAF_BONDEDDIE_OFFSET)	= 0;
		*(pulTemp + BH_CURWAF_DEFECTDIE_OFFSET)	= 0;
		*(pulTemp + BH_CURWAF_CHIPDIE_OFFSET)	= 0;
		*(pulTemp + BH_CURWAF_INKDIE_OFFSET)	= 0;
		*(pulTemp + BH_CURWAF_BADCUTDIE_OFFSET)	= 0;
		*(pulTemp + BH_CURWAF_ROTATEDIE_OFFSET)	= 0;
		*(pulTemp + BH_CURWAF_EMPTYDIE_OFFSET)	= 0;
		*(pulTemp + BH_CURWAF_MISSINGDIE_OFFSET)= 0;
	}
}


// Get all sensor values from hardware
VOID CBondHead::GetSensorValue()
{
	if (m_fHardware && !m_bDisableBH)
	{
		//m_bHomeSnr_Z		= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_Z, &m_stBHAxis_Z);
		//m_bHomeSnr_T		= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_T, &m_stBHAxis_T);
		m_bHomeSnr_Ej			= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_EJ,			&m_stBHAxis_Ej);
		m_bHomeSnr_EjElevatorZ	= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_EJELEVATOR,	&m_stBHAxis_EjElevator); 
		m_bHomeSnr_EjCap		= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_EJCAP,			&m_stBHAxis_EjCap);
			
		if (m_bEnableEjectorTheta)
		{
			m_bHomeSnr_EjT		= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_EJ_T,			&m_stBHAxis_EjT);
		}

		if (m_bEnableBinEjector)
		{
			m_bHomeSnr_BinEj			= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_BIN_EJ,			&m_stBHAxis_BinEj);
			m_bHomeSnr_BinEjElevatorZ	= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_BIN_EJELEVATOR,	&m_stBHAxis_BinEjElevator); 
			m_bHomeSnr_BinEjT			= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_BIN_EJ_T,			&m_stBHAxis_BinEjT);
			m_bHomeSnr_BinEjCap			= CMS896AStn::MotionIsHomeSensorHigh(BH_AXIS_BIN_EJCAP,			&m_stBHAxis_BinEjCap);
		}

		//m_bPosLimitSnr_Z	= CMS896AStn::MotionIsPositiveLimitHigh(BH_AXIS_Z, &m_stBHAxis_Z);
		//m_bNegLimitSnr_Z	= CMS896AStn::MotionIsNegativeLimitHigh(BH_AXIS_Z, &m_stBHAxis_Z);
		//m_bPosLimitSnr_T	= CMS896AStn::MotionIsPositiveLimitHigh(BH_AXIS_T, &m_stBHAxis_T);
		//m_bNegLimitSnr_T	= CMS896AStn::MotionIsNegativeLimitHigh(BH_AXIS_T, &m_stBHAxis_T);

		/*if (m_bIsArm2Exist == TRUE)
		{
			m_bPosLimitSnr_Z2	= CMS896AStn::MotionIsPositiveLimitHigh(BH_AXIS_Z2, &m_stBHAxis_Z2);
			m_bNegLimitSnr_Z2	= CMS896AStn::MotionIsNegativeLimitHigh(BH_AXIS_Z2, &m_stBHAxis_Z2);
		}*/

		if (CMS896AApp::m_lCycleSpeedMode >= 4)			//For MS100Plus only
		{
			//m_bBHFan	= CMS896AStn::MotionReadInputBit(BH_SI_BH_FAN);
		}
	}
}

VOID CBondHead::GetCommanderValue()
{
	CString szAxis;
	if (m_fHardware && !m_bDisableBH)
	{
		try
		{
			szAxis = BH_AXIS_EJ;
			m_lCmd_Ej = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_Ej);

			szAxis = BH_AXIS_EJCAP;
			m_lCmd_EjCap = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_EjCap);

			if (m_bMS60EjElevator)
			{
				szAxis = BH_AXIS_EJELEVATOR;
				m_lCmd_EjElevatorZ = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_EjElevator);
			}
			
			if (m_bEnableEjectorTheta)
			{
				szAxis = BH_AXIS_EJ_T;
				m_lCmd_EjT = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_EjT);
			}

			if (m_bEnableBinEjector)
			{
				szAxis = BH_AXIS_BIN_EJ;
				m_lCmd_BinEj = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_BinEj);

				szAxis = BH_AXIS_BIN_EJCAP;
				m_lCmd_BinEjCap = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_BinEjCap);

				if (m_bMS60EjElevator)
				{
					szAxis = BH_AXIS_BIN_EJELEVATOR;
					m_lCmd_BinEjElevatorZ = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_BinEjElevator);
				}

				if (m_bEnableEjectorTheta)
				{
					szAxis = BH_AXIS_BIN_EJ_T;
					m_lCmd_BinEjT = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stBHAxis_BinEjT);
				}
			}
		}
		catch(CAsmException e)
		{
			DisplayException(e);
			if (szAxis == BH_AXIS_EJ)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_Ej);
			else if (szAxis == BH_AXIS_EJELEVATOR)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjElevator);
			else if (szAxis == BH_AXIS_EJCAP)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjCap);
			else if (szAxis == BH_AXIS_EJ_T)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjT);
			else if (szAxis == BH_AXIS_BIN_EJ)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEj);
			else if (szAxis == BH_AXIS_BIN_EJELEVATOR)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEjElevator);
			else if (szAxis == BH_AXIS_BIN_EJCAP)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEjCap);
			else //if (szAxis == BH_AXIS_BIN_EJ_T)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEjT);
		}
	}
}

// Get all encoder values from hardware
VOID CBondHead::GetEncoderValue()
{
	CString szAxis;
	if (m_fHardware && !m_bDisableBH)
	{
		try
		{
			/*szAxis = BH_AXIS_Z;
			if ( CMS896AStn::MotionIsServo(BH_AXIS_Z, &m_stBHAxis_Z) || CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z) )
			{
				m_lEnc_Z = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_Z, 1, &m_stBHAxis_Z);
			}
			else
			{
				m_lEnc_Z = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_Z, 0.8, &m_stBHAxis_Z);
			}

			szAxis = BH_AXIS_T;
			if ( CMS896AStn::MotionIsServo(BH_AXIS_T, &m_stBHAxis_T) )
			{	
				m_lEnc_T = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_T, 1, &m_stBHAxis_T);
			}
			else
			{
				m_lEnc_T = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_T, 0.8, &m_stBHAxis_T);
			}*/

			szAxis = BH_AXIS_EJ;
			if ( CMS896AStn::MotionIsServo(BH_AXIS_EJ, &m_stBHAxis_Ej) )
			{
				m_lEnc_Ej = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJ, m_dEjRes, &m_stBHAxis_Ej);
			}
			else
			{
				m_lEnc_Ej = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJ, m_dEjRes * 0.8, &m_stBHAxis_Ej);
			}

			szAxis = BH_AXIS_EJCAP;
			if ( CMS896AStn::MotionIsServo(BH_AXIS_EJCAP, &m_stBHAxis_EjCap) )
			{
				m_lEnc_EjCap = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJCAP, 1, &m_stBHAxis_EjCap);
			}
			else
			{
				m_lEnc_EjCap = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJCAP, 1 * 0.8, &m_stBHAxis_EjCap);
			}

			if (m_bEnableEjectorTheta)
			{
				szAxis = BH_AXIS_EJ_T;
				if ( CMS896AStn::MotionIsServo(BH_AXIS_EJ_T, &m_stBHAxis_EjCap) )
				{
					m_lEnc_EjT = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJ_T, 1, &m_stBHAxis_EjT);
				}
				else
				{
					m_lEnc_EjT = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJ_T, 1 * 0.8, &m_stBHAxis_EjT);
				}
			}

			//Bin Ejector
			if (m_bEnableBinEjector)
			{
				szAxis = BH_AXIS_BIN_EJ;
				if ( CMS896AStn::MotionIsServo(BH_AXIS_BIN_EJ, &m_stBHAxis_Ej) )
				{
					m_lEnc_BinEj = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_BIN_EJ, m_dEjRes, &m_stBHAxis_BinEj);
				}
				else
				{
					m_lEnc_BinEj = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_BIN_EJ, m_dEjRes * 0.8, &m_stBHAxis_BinEj);
				}

				szAxis = BH_AXIS_BIN_EJCAP;
				if ( CMS896AStn::MotionIsServo(BH_AXIS_BIN_EJCAP, &m_stBHAxis_EjCap) )
				{
					m_lEnc_BinEjCap = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_BIN_EJCAP, 1, &m_stBHAxis_BinEjCap);
				}
				else
				{
					m_lEnc_BinEjCap = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_BIN_EJCAP, 1 * 0.8, &m_stBHAxis_BinEjCap);
				}

				szAxis = BH_AXIS_BIN_EJ_T;
				if (CMS896AStn::MotionIsServo(BH_AXIS_BIN_EJ_T, &m_stBHAxis_EjCap))
				{
					m_lEnc_BinEjT = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_BIN_EJ_T, 1, &m_stBHAxis_BinEjT);
				}
				else
				{
					m_lEnc_BinEjT = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_BIN_EJ_T, 1 * 0.8, &m_stBHAxis_BinEjT);
				}
			}
#ifdef NU_MOTION
			//if (m_bIsArm2Exist == TRUE)
			//{
			//	szAxis = BH_AXIS_Z2;
			//	if ( CMS896AStn::MotionIsServo(BH_AXIS_Z2, &m_stBHAxis_Z2) || CMS896AStn::MotionIsDcServo(BH_AXIS_Z2, &m_stBHAxis_Z2) )
			//	{
			//		m_lEnc_Z2 = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_Z2, 1, &m_stBHAxis_Z2);
			//	}
			//	else
			//	{
			//		m_lEnc_Z2 = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_Z2, 0.8, &m_stBHAxis_Z2);
			//	}
			//}

			//if (m_bMS100EjtXY)		//v4.41T6
			//{
			//	szAxis = BH_AXIS_EJ_X;
			//	m_lEnc_EjX = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJ_X, 1, &m_stBHAxis_EjX);

			//	szAxis = BH_AXIS_EJ_Y;
			//	m_lEnc_EjY = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJ_Y, 1, &m_stBHAxis_EjY);
			//}

			if (m_bMS60EjElevator)	//v4.51A5	//MS60
			{
				//EJ Elevator axis (stepper motor)
				szAxis = BH_AXIS_EJELEVATOR;
				m_lEnc_EjElevatorZ = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_EJELEVATOR, m_dEjElevatorRes * 0.8, &m_stBHAxis_EjElevator);
				
				if (m_bEnableBinEjector)
				{
					szAxis = BH_AXIS_BIN_EJELEVATOR;
					m_lEnc_BinEjElevatorZ = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_BIN_EJELEVATOR, m_dEjElevatorRes * 0.8, &m_stBHAxis_BinEjElevator);
				}
			}
#endif
			/*
			szAxis = BH_AXIS_MS50CHGCOLLET_Z;
			if (CMS896AStn::MotionIsServo(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ) ||
				CMS896AStn::MotionIsDcServo(BH_AXIS_MS50CHGCOLLET_Z, &m_stBHAxis_ChgColletZ))
			{
				m_lEnc_ChgColletZ = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_MS50CHGCOLLET_Z, 1, &m_stBHAxis_ChgColletZ);
			}
			else
			{
				m_lEnc_ChgColletZ = CMS896AStn::MotionGetEncoderPosition(BH_AXIS_MS50CHGCOLLET_Z, 0.8, &m_stBHAxis_ChgColletZ);
			}
			*/
		}
		catch(CAsmException e)
		{
			DisplayException(e);
			if (szAxis == BH_AXIS_EJ)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_Ej);
			else if (szAxis == BH_AXIS_EJ_X)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjX);
			else if (szAxis == BH_AXIS_EJ_Y)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjY);
			else if (szAxis == BH_AXIS_Z)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_Z);
			else if (szAxis == BH_AXIS_Z2)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_Z2);
			else if (szAxis == BH_AXIS_EJELEVATOR)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjElevator);
			else if (szAxis == BH_AXIS_EJCAP)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjCap);
			else if (szAxis == BH_AXIS_EJ_T)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjT);
			else if (szAxis == BH_AXIS_BIN_EJ)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEj);
			else if (szAxis == BH_AXIS_BIN_EJELEVATOR)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEjElevator);
			else if (szAxis == BH_AXIS_BIN_EJCAP)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEjCap);
			else if (szAxis == BH_AXIS_BIN_EJ_T)
				CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_BinEjT);
			//else		//andrewng //2020-0708
			//	CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_T);
		}
	}
}


#define	CP_733_TABLE	0
VOID CBondHead::GetAxisInformation()	//NuMotion
{
#ifdef NU_MOTION

	InitAxisData(m_stBHAxis_Z);
	m_stBHAxis_Z.m_szName			= BH_AXIS_Z;
	m_stBHAxis_Z.m_szTag			= MS896A_CFG_CH_BONDHEAD;
	m_stBHAxis_Z.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBHAxis_Z);
	m_stBHAxis_Z.m_dEncResolution		= 1;

	InitAxisData(m_stBHAxis_Z2);
	m_stBHAxis_Z2.m_szName			= BH_AXIS_Z2;
	m_stBHAxis_Z2.m_szTag			= MS896A_CFG_CH_BONDHEAD2;
	m_stBHAxis_Z2.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBHAxis_Z2);
	m_stBHAxis_Z2.m_dEncResolution		= 1;

	InitAxisData(m_stBHAxis_T);
	m_stBHAxis_T.m_szName			= BH_AXIS_T;
	m_stBHAxis_T.m_szTag			= MS896A_CFG_CH_BONDARM;
	m_stBHAxis_T.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBHAxis_T);
	m_stBHAxis_T.m_dEncResolution		= 1;

	//Wafer Ejector
	InitAxisData(m_stBHAxis_Ej);
	m_stBHAxis_Ej.m_szName						= BH_AXIS_EJ;
	m_stBHAxis_Ej.m_szTag						= MS896A_CFG_CH_EJECTOR;
	m_stBHAxis_Ej.m_ucControlID					= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBHAxis_Ej);
	m_stBHAxis_Ej.m_dEncResolution				= 1;

	InitAxisData(m_stBHAxis_EjCap);
	m_stBHAxis_EjCap.m_szName					= BH_AXIS_EJCAP;
	m_stBHAxis_EjCap.m_szTag					= MS896A_CFG_CH_EJECTOR_CAP;
	m_stBHAxis_EjCap.m_ucControlID				= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBHAxis_EjCap);
	m_stBHAxis_EjCap.m_dEncResolution			= 1;

	InitAxisData(m_stBHAxis_EjElevator);
	m_stBHAxis_EjElevator.m_szName				= BH_AXIS_EJELEVATOR;
	m_stBHAxis_EjElevator.m_szTag				= MS896A_CFG_CH_EJ_ELEVATOR;
	m_stBHAxis_EjElevator.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBHAxis_EjElevator);
	m_stBHAxis_EjElevator.m_dEncResolution		= 1;

	if (m_bEnableEjectorTheta)
	{
		InitAxisData(m_stBHAxis_EjT);
		m_stBHAxis_EjT.m_szName						= BH_AXIS_EJ_T;
		m_stBHAxis_EjT.m_szTag						= MS896A_CFG_CH_EJECTOR_T;
		m_stBHAxis_EjT.m_ucControlID				= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_EjT);
		m_stBHAxis_EjT.m_dEncResolution				= 1;
	}
	
	//Bin Ejector
	if (m_bEnableBinEjector)
	{
		InitAxisData(m_stBHAxis_BinEj);
		m_stBHAxis_BinEj.m_szName					= BH_AXIS_BIN_EJ;
		m_stBHAxis_BinEj.m_szTag					= MS896A_CFG_CH_BIN_EJECTOR;
		m_stBHAxis_BinEj.m_ucControlID				= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_BinEj);
		m_stBHAxis_BinEj.m_dEncResolution			= 1;

		InitAxisData(m_stBHAxis_BinEjCap);
		m_stBHAxis_BinEjCap.m_szName				= BH_AXIS_BIN_EJCAP;
		m_stBHAxis_BinEjCap.m_szTag					= MS896A_CFG_CH_BIN_EJECTOR_CAP;
		m_stBHAxis_BinEjCap.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_BinEjCap);
		m_stBHAxis_BinEjCap.m_dEncResolution		= 1;

		InitAxisData(m_stBHAxis_BinEjElevator);
		m_stBHAxis_BinEjElevator.m_szName			= BH_AXIS_BIN_EJELEVATOR;
		m_stBHAxis_BinEjElevator.m_szTag			= MS896A_CFG_CH_BIN_EJ_ELEVATOR;
		m_stBHAxis_BinEjElevator.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_BinEjElevator);
		m_stBHAxis_BinEjElevator.m_dEncResolution	= 1;

		InitAxisData(m_stBHAxis_BinEjT);
		m_stBHAxis_BinEjT.m_szName					= BH_AXIS_BIN_EJ_T;
		m_stBHAxis_BinEjT.m_szTag					= MS896A_CFG_CH_BIN_EJECTOR_T;
		m_stBHAxis_BinEjT.m_ucControlID				= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_BinEjT);
		m_stBHAxis_BinEjT.m_dEncResolution			= 1;
	}


	//v4.41T5
	if (m_bMS100EjtXY)
	{
		InitAxisData(m_stBHAxis_EjX);
		m_stBHAxis_EjX.m_szName				= BH_AXIS_EJ_X;
		m_stBHAxis_EjX.m_szTag				= MS896A_CFG_CH_EJECTOR_X;
		m_stBHAxis_EjX.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_EjX);
		m_stBHAxis_EjX.m_dEncResolution		= 1;

		InitAxisData(m_stBHAxis_EjY);
		m_stBHAxis_EjY.m_szName				= BH_AXIS_EJ_Y;
		m_stBHAxis_EjY.m_szTag				= MS896A_CFG_CH_EJECTOR_Y;
		m_stBHAxis_EjY.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBHAxis_EjY);
		m_stBHAxis_EjY.m_dEncResolution		= 1;

		PrintAxisData(m_stBHAxis_EjX);
		PrintAxisData(m_stBHAxis_EjY);
	}

	//v4.59A44	//MS50
	if (m_bMS50ChgColletZT)
	{
		//Stepper T
		InitAxisData(m_stBHAxis_ChgColletT);
		m_stBHAxis_ChgColletT.m_szName				= BH_AXIS_MS50CHGCOLLET_T;
		m_stBHAxis_ChgColletT.m_szTag				= MS896A_CFG_CH_CHGCOLLET_T;
		m_stBHAxis_ChgColletT.m_ucControlID			= PL_DYNAMIC;	
		GetAxisData(m_stBHAxis_ChgColletT);
		m_stBHAxis_ChgColletT.m_dEncResolution		= 1;
		//Stepper Z on BT
		InitAxisData(m_stBHAxis_ChgColletZ);
		m_stBHAxis_ChgColletZ.m_szName				= BH_AXIS_MS50CHGCOLLET_Z;
		m_stBHAxis_ChgColletZ.m_szTag				= MS896A_CFG_CH_CHGCOLLET_Z;
		m_stBHAxis_ChgColletZ.m_ucControlID			= PL_DYNAMIC;		
		GetAxisData(m_stBHAxis_ChgColletZ);
		m_stBHAxis_ChgColletZ.m_dEncResolution		= 1;

		PrintAxisData(m_stBHAxis_ChgColletT);
		PrintAxisData(m_stBHAxis_ChgColletZ);
	}

	//For debug only
	PrintAxisData(m_stBHAxis_Z);
	PrintAxisData(m_stBHAxis_Z2);
	PrintAxisData(m_stBHAxis_T);

	PrintAxisData(m_stBHAxis_Ej);
	PrintAxisData(m_stBHAxis_EjElevator);
	PrintAxisData(m_stBHAxis_EjT);
	PrintAxisData(m_stBHAxis_EjCap);

	PrintAxisData(m_stBHAxis_BinEj);
	PrintAxisData(m_stBHAxis_BinEjElevator);
	PrintAxisData(m_stBHAxis_BinEjT);
	PrintAxisData(m_stBHAxis_BinEjCap);
#endif
}

VOID CBondHead::GetEjAxisInformation()	//M94
{
#ifdef NU_MOTION
	InitAxisData(m_stBHAxis_Ej);
	m_stBHAxis_Ej.m_szName			= BH_AXIS_EJ;
	m_stBHAxis_Ej.m_szTag			= MS896A_CFG_CH_EJECTOR;
	m_stBHAxis_Ej.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBHAxis_Ej);
	PrintAxisData(m_stBHAxis_Ej);

	if (m_bMS60EjElevator)		//v4.54A6	//MS60
	{
		InitAxisData(m_stBHAxis_EjElevator);
		m_stBHAxis_EjElevator.m_szName			= BH_AXIS_EJELEVATOR;
		m_stBHAxis_EjElevator.m_szTag			= MS896A_CFG_CH_EJ_ELEVATOR;
		m_stBHAxis_EjElevator.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		
		GetAxisData(m_stBHAxis_EjElevator);
		PrintAxisData(m_stBHAxis_EjElevator);
	}

#endif
}

BOOL CBondHead::InitES101Hardware()		//v4.24
{
	if (!m_fHardware)
		return TRUE;
	if (m_bES101Configuration != TRUE)
		return FALSE;

	LONG lDynamicControl	= 0;
	LONG lStaticControl		= 0;
	LONG lMotorDirectionE	= 0;

	m_bSel_Ej = TRUE;

	m_lE_ProfileType	= GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_PROFILE_TYPE);	//Linear Ej
	m_lE_CommMethod		= GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_SWCOMM);	
	
	if (m_lE_ProfileType == MS896A_OBW_PROFILE)
	{
		m_bIsLinearEjector = TRUE;
		(*m_psmfSRam)["BondHeadStn"]["LinearEjector"] = TRUE;
		DisplayMessage("BondHeadStn - LEJT is used");
	}
	else
	{
		(*m_psmfSRam)["BondHeadStn"]["LinearEjector"] = FALSE;
	}

	lMotorDirectionE = GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_MOTOR_DIRECTION);				
	//CMS896AStn::MotionEnableProtection(BH_AXIS_EJ, HP_LIMIT_SENSOR, FALSE, FALSE, &m_stBHAxis_Ej);
	//CMS896AStn::MotionEnableEncoder(BH_AXIS_EJ, &m_stBHAxis_Ej);
				
	// Reverse motor direction if needed
	if (lMotorDirectionE == -1)
	{
		CMS896AStn::MotionSetEncoderDirection(BH_AXIS_EJ, HP_NEGATIVE_DIR, &m_stBHAxis_Ej);
		CMS896AStn::MotionReverseMotorDirection(BH_AXIS_EJ, HP_ENABLE, &m_stBHAxis_Ej);
	}

	lDynamicControl	= GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_DYNA_CONTROL);				
	lStaticControl	= GetChannelInformation(MS896A_CFG_CH_EJECTOR, MS896A_CFG_CH_STAT_CONTROL);	

	if (lDynamicControl != 0)
	{
		CMS896AStn::MotionSelectControlType(BH_AXIS_EJ, lDynamicControl, 2, &m_stBHAxis_Ej);
	}

	if (lStaticControl != 0)
	{
		CMS896AStn::MotionSelectControlType(BH_AXIS_EJ, lStaticControl, 1, &m_stBHAxis_Ej);
	}


	Ej_Profile(NORMAL_PROF);
	Ej_Home();

	EjElevator_Home();

	SetNGPickReelStop(FALSE);	//v4.28
	SetNGPickUpDn(FALSE);		//v4.28
	CMS896AStn::MotionSelectProfile(BH_AXIS_NGPICK, "mpfNGPickT", &m_stBHAxis_NGPick);
	NGPickT_Home();

	return TRUE;
}

BOOL CBondHead::InitEjTableXY()			//v4.41T5
{
	if (!m_fHardware)
		return TRUE;
	if (!m_bMS100EjtXY)
		return TRUE;

	INT nResult = gnOK;
	nResult = EjX_Home();
	nResult = EjY_Home();

	CMS896AStn::MotionSelectProfile(BH_AXIS_EJ_X, BH_MP_E_X_NORMAL, &m_stBHAxis_EjX);
	CMS896AStn::MotionSelectProfile(BH_AXIS_EJ_Y, BH_MP_E_Y_NORMAL, &m_stBHAxis_EjY);

	if (nResult != gnOK)
		return FALSE;
	return TRUE;
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////

VOID CBondHead::UpdateOutput()
{
	static int nCount = 0;
	CString szAxis;

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
		if (m_fHardware)		//v3.60
		{
			GetSensorValue();

			GetEncoderValue();

			if (!m_bDisableBH)	
			{
				//szAxis = BH_AXIS_T;
				//m_bIsPowerOn_T = CMS896AStn::MotionIsPowerOn(BH_AXIS_T, &m_stBHAxis_T);

				//szAxis = BH_AXIS_Z;
				//m_bIsPowerOn_Z = CMS896AStn::MotionIsPowerOn(BH_AXIS_Z, &m_stBHAxis_Z);

				szAxis = BH_AXIS_EJ;
				m_bIsPowerOn_Ej = CMS896AStn::MotionIsPowerOn(BH_AXIS_EJ, &m_stBHAxis_Ej);

				if (m_bMS60EjElevator)	//v4.51A5
				{
					szAxis = BH_AXIS_EJELEVATOR;
					m_bIsPowerOn_EjElevator = CMS896AStn::MotionIsPowerOn(BH_AXIS_EJELEVATOR, &m_stBHAxis_EjElevator);
				}
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		/*
		if (szAxis == BH_AXIS_EJ)
			CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_Ej);
		//else if (szAxis == BH_AXIS_Z)
		//	CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_Z);
		//else if (szAxis == BH_AXIS_Z2)
		//	CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_Z2);
		else if (szAxis == BH_AXIS_EJ_X)
			CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjX);
		else if (szAxis == BH_AXIS_EJ_Y)
			CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_EjY);
		else
			CMS896AStn::MotionCheckResult(szAxis, &m_stBHAxis_T);
		*/
	}
}


VOID CBondHead::UpdateProfile()
{
	m_bUpdateProfile = FALSE;

	try
	{
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

VOID CBondHead::UpdatePosition()
{
	m_bUpdatePosition = FALSE;
}


VOID CBondHead::UpdateSafetySensor()
{
	static int nCount = 0;
	CString szAxis;

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
			/*m_bPressureSnr			= CMS896AStn::MotionReadInputBit(BH_SI_PRESSURE);
			m_bVacuumFlowSnr		= IsLowVacuumFlow();

			IsCleanColletSafetySensor();
			IsFrontCoverOpen();
			IsFrontLeftCoverOpen();
			IsFrontMiddleCoverOpen();
			IsRearLeftCoverOpen();
			IsRearRightCoverOpen();
			IsLeftCoverOpen();
	
			ReadBH1AirFlowValue();
			ReadBH2AirFlowValue();
			IsFrontCoverLocked();
			IsSideCoverLocked();
			IsBinElevatorCoverLocked();*/
		}
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

VOID CBondHead::RunDacTest()
{
#ifdef NU_MOTION
	NuMotionDataLog_DAC(BH_AXIS_T, m_stBHAxis_T, TRUE, 10, m_stDataLog, m_bBhTDacForwardMove);		//v4.16T1
#else
	SetupDataLogT();
	EnableDataLogT(TRUE);
#endif

	//Move T motor for logging
	if (m_bBhTDacForwardMove)
		T_MoveTo(m_lBondPos_T);

	else
		T_MoveTo(m_lPickPos_T);
	
#ifdef NU_MOTION
	NuMotionDataLog_DAC(BH_AXIS_T, m_stBHAxis_T, FALSE, 10, m_stDataLog, m_bBhTDacForwardMove);		//v4.16T1
#else
	EnableDataLogT(FALSE);
#endif

	m_bBhTDacForwardMove = !m_bBhTDacForwardMove;

	Sleep(m_lBondHeadDelay);	//Delay between forawrd/backward motion

}

LONG CBondHead::SetUBHZChannelProtection(BOOL bEnable)
{
	/*
	//hp_select_depend_channel(&m_pDcServo_Z->m_HpCcb , &m_pServo_T->m_HpCcb);
	m_pDcServo_Z->SelectDependCh(m_pServo_T);
	m_pDcServo_Z->EnableProtection(HP_DRIVER_FAULT, bEnable);
	m_pDcServo_Z->EnableProtection(HP_ENCODER_FAULT, bEnable);
	m_pDcServo_Z->EnableProtection(HP_ISOLATION_POWER_FAULT, bEnable);
	m_pDcServo_Z->EnableProtection(HP_OTHER_CH_FAULT, bEnable);
	m_pDcServo_Z->EnableProtection(HP_FLEXICONTROL_ERROR, bEnable);
	*/

	INT nReturn = 0;
	//Hp_ccb hp_servo_channel_Z = m_pDcServo_Z->m_HpCcb;
    //Hp_ccb hp_servo_channel_T = m_pServo_T->m_HpCcb;
	Hp_ccb hp_servo_channel_Z;
	Hp_ccb hp_servo_channel_T;

	try
	{
		if (GetAxisType(BH_AXIS_Z) == BH_DC_SVO_MOTOR)	
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(BH_AXIS_Z));
			
			if (pDcServo != NULL)
			{
				hp_servo_channel_Z = pDcServo->m_HpCcb;
			}
		}
		
		if (GetAxisType(BH_AXIS_T) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_T));
			
			if (pServo != NULL)
			{
				hp_servo_channel_T = pServo->m_HpCcb;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	hp_select_depend_channel(&hp_servo_channel_Z, &hp_servo_channel_T);

	if ((nReturn = hp_ch_protection(&hp_servo_channel_Z, HP_DRIVER_FAULT, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		//AfxMessageBox("HP_DRIVER_FAULT", MB_SYSTEMMODAL);
		return nReturn;
	}

	if ((nReturn = hp_ch_protection(&hp_servo_channel_Z, HP_ENCODER_FAULT, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		return nReturn;
	}

	if ((nReturn = hp_ch_protection(&hp_servo_channel_Z, HP_ISOLATION_POWER_FAULT, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		return nReturn;
	}

    if ((nReturn = hp_ch_protection(&hp_servo_channel_Z, HP_OTHER_CH_FAULT, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		return nReturn;
	}

	if ((nReturn = hp_ch_protection(&hp_servo_channel_Z, HP_FLEXICONTROL_ERROR, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		return nReturn;
	}

	return 1;
}

LONG CBondHead::SetUBHTChannelProtection(BOOL bEnable)
{
	INT nReturn =0;

	//Hp_ccb hp_servo_channel_Z = m_pDcServo_Z->m_HpCcb;
    //Hp_ccb hp_servo_channel_T = m_pServo_T->m_HpCcb;
	Hp_ccb hp_servo_channel_Z;
	Hp_ccb hp_servo_channel_T;

	try
	{
		if (GetAxisType(BH_AXIS_Z) == BH_DC_SVO_MOTOR)	
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(BH_AXIS_Z));
			
			if (pDcServo != NULL)
			{
				hp_servo_channel_Z = pDcServo->m_HpCcb;
			}
		}

		if (GetAxisType(BH_AXIS_T) == BH_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_T));
			
			if (pServo != NULL)
			{
				hp_servo_channel_T = pServo->m_HpCcb;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	hp_select_depend_channel(&hp_servo_channel_T,&hp_servo_channel_Z);
	
	if ((nReturn = hp_ch_protection(&hp_servo_channel_T, HP_DRIVER_FAULT, bEnable, HP_ACTIVE_LOW)) !=0)
	{
		return nReturn;
	}

	if ((nReturn = hp_ch_protection(&hp_servo_channel_T, HP_ENCODER_FAULT, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		return nReturn;
	}

	if ((nReturn = hp_ch_protection(&hp_servo_channel_T, HP_ISOLATION_POWER_FAULT, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		return nReturn;
	}

    if ((nReturn = hp_ch_protection(&hp_servo_channel_T, HP_OTHER_CH_FAULT, bEnable, HP_ACTIVE_LOW)) !=0)
	{
		return nReturn;
	}

	if ( (nReturn = hp_ch_protection(&hp_servo_channel_T, HP_FLEXICONTROL_ERROR, bEnable, HP_ACTIVE_LOW)) != 0)
	{
		return nReturn;
	}

	return 1;
}

BOOL CBondHead::GenerateAccumlateDieCountReport()
{
	CTime curTime = CTime::GetCurrentTime();
	CStdioFile cfFile;
	CString szDate, szTime;
	CString szMachineReportPath, szFilename, szFile;
	CString szMachineNo;
	CString szBondCount;

	szMachineReportPath = (*m_psmfSRam)["MS896A"]["Machine"]["Report Path"];
	szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
	szDate = curTime.Format("%Y%m");
	szFilename = szMachineNo + "_" + szDate + ".csv";

	szFile = szMachineReportPath + "\\" + szFilename;

	if (cfFile.Open(szFile, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate) == FALSE)
	{
		return FALSE;	
	}

	cfFile.SeekToEnd();
	
	if (cfFile.GetLength() == 0)
	{
		cfFile.WriteString("Date,Time,Bond Count\n");
	}

	szDate = curTime.Format("%Y/%m/%d");
	szTime = curTime.Format("%H:%M:%S");
	szBondCount.Format("%d", m_ulBondDieCount);
	cfFile.WriteString(szDate + "," + szTime + "," + szBondCount + "\n");
	cfFile.Close();

	return TRUE;
}

BOOL CBondHead::InitMS60ThermalCtrl()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//v4.59A21
	if (m_bDBHHeatingCoilFcnInit)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("MS60 Thermal Fcn re-initiated DONE");
		return TRUE;
	}


	//v4.50A6	//Modified
	if (CMS896AStn::m_bDBHHeatingCoilFcn)		//v4.49A5
	{
		m_bMS60ThermalCtrl = TRUE;				//v4.48A10
/*
		CString szValue;
		szValue = pApp->GetProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal IS"), "");
		if (szValue.GetLength() > 0)
		{
			DOUBLE dValue = atof((LPCTSTR) szValue);
			if ( (dValue >= 0) && (dValue < 1) )	//v4.48 Klocwork
			{
				m_dIS = dValue;
			}
		}

		szValue = pApp->GetProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal P"), "");
		if (szValue.GetLength() > 0)
		{
			DOUBLE dValue = atof((LPCTSTR) szValue);
			if ( (dValue >= 0) )
			{
				m_dMS60Thermal_P = dValue;
			}
		}

		szValue = pApp->GetProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal I"), "");
		if (szValue.GetLength() > 0)
		{
			DOUBLE dValue = atof((LPCTSTR) szValue);
			if ( (dValue >= 0) )
			{
				m_dMS60Thermal_I = dValue;
			}
		}

		szValue = pApp->GetProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal D"), "");
		if (szValue.GetLength() > 0)
		{
			DOUBLE dValue = atof((LPCTSTR) szValue);
			if ( (dValue >= 0) )
			{
				m_dMS60Thermal_D = dValue;
			}
		}
*/
		//Harcode
		m_dIS = 0.21;
		m_dMS60Thermal_P = 3000;
		m_dMS60Thermal_I = 30;
		m_dMS60Thermal_D = 0;

		m_unMS60TargetIdleTemperature = pApp->GetProfileInt(gszPROFILE_HW_CONFIG, _T("MS60 IDLE Temp"), 0);
		if ( (m_unMS60TargetIdleTemperature <= 0) || (m_unMS60TargetIdleTemperature >= MS60_BH_MAX_TEMPERATURE_IDLE) )
		{
			m_unMS60TargetIdleTemperature = MS60_BH_MAX_TEMPERATURE_IDLE - 5;
		}


		CString szMsg;
		szMsg.Format("MS50 Thermal Fcn enabled, Is = %.6f", m_dIS);

		//New for //v4.53A20
#ifdef NU_MOTION_MS60	//v4.54A3

		szMsg.Format("MS50 Thermal Fcn enabled, Is = %.6f, P = %.6f, I = %.6f, D = %.6f, TEMP = %d", 
			m_dIS, m_dMS60Thermal_P, m_dMS60Thermal_I, m_dMS60Thermal_D,
			m_unMS60TargetIdleTemperature);

		GMP_S16 sReturn = GMP_TRUE;
//		sReturn = InitThermalCtrl((GMP_FLOAT) m_dMS60Thermal_P, 
//								  (GMP_FLOAT) m_dMS60Thermal_I, 
//								  (GMP_FLOAT) m_dMS60Thermal_D, 
//								  (GMP_FLOAT) m_dIS);
		if (m_pThermalCtrl != NULL)
		{
			delete m_pThermalCtrl;
		}

		m_pThermalCtrl = new CAgmpCtrlPIDMotorThermalCtrl(NULL, 
														  NULL,
														  m_dIS, 
														  (GMP_FLOAT)m_unMS60TargetIdleTemperature,
														  (GMP_FLOAT) m_dMS60Thermal_P, 
														  (GMP_FLOAT) m_dMS60Thermal_I, 
														  (GMP_FLOAT) m_dMS60Thermal_D, 
														  200, 
														  0,
														  500, 
														  (GMP_FLOAT)0.00763);

#endif

		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		DisplayMessage(szMsg);

		m_bDBHHeatingCoilFcnInit = TRUE;	//v4.59A21

		EnableBHTThermalControl(TRUE);		
	}

	//}
	//if (m_bMS60ThermalCtrl)		//v4.48A16
	//	SetBondHeadFan(TRUE);

	//v4.59A18
	if (IsMS60() && CMS896AStn::m_bDBHThermostat)
	{
		//m_ulBHTThermostatADC		= lValue;
		//m_ulBHTThermostatReading	= (ULONG) (250.0 * lValue / 32767.0);

		USHORT usTemp = (USHORT)_round(MS60_BH_MAX_TEMPERATURE_AUTOBOND * 32767.0 / 250.0);		// = 65 degree

		CString szLog;
		szLog.Format("MS50 Thermal Fcn: enabling PROCESS-BLK at %d C", usTemp);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		DisplayMessage(szLog);

		MotionSetThermalCtrlTimer(BH_AXIS_T, usTemp, &m_stBHAxis_T);	//v4.59A21
	}

	return TRUE;
}

	
BOOL CBondHead::HomeEjModule()
{
	//AfxMessageBox("Homing Ej ....", MB_SYSTEMMODAL);
	
	Ej_Home();
	if (!Ej_IsPowerOn())
	{
		AfxMessageBox("Homing Ej FAIL", MB_SYSTEMMODAL);
		return FALSE;
	}
	Ej_MoveTo(-2000);
	
	//andrewng //2020-0831
	EjCap_Home();
	if (!EjCap_IsPowerOn())
	{
		AfxMessageBox("Homing Ej CAP FAIL", MB_SYSTEMMODAL);
		return FALSE;
	}
	EjCap_MoveTo(1000);

	if (m_bEnableEjectorTheta)
	{
		EjT_Home();
		if (!EjT_IsPowerOn())
		{
			AfxMessageBox("Homing EjT FAIL", MB_SYSTEMMODAL);
			return FALSE;
		}
		EjT_MoveTo(3000);
	}

	if (m_bMS60EjElevator)
	{
		EjElevator_Home();
		if (!EjElevator_IsPowerOn())
		{
			AfxMessageBox("Homing Ej Elv FAIL", MB_SYSTEMMODAL);
			return FALSE;
		}

		// Move To non hit WT position
		//EjElevator_MoveTo(-33500);

		DOUBLE dTargetHomeZ = -30000.0 / m_dEjElevatorRes;
		LONG lTargetZ = (LONG) dTargetHomeZ;
		EjElevator_MoveTo(lTargetZ);
	}

	return TRUE;
}

BOOL CBondHead::HomeBinEjModule()
{
	if (!m_bEnableBinEjector)
	{
		return TRUE;
	}
	//AfxMessageBox("Homing Bin Ej ....", MB_SYSTEMMODAL);

	BinEj_Home();
	if (!BinEj_IsPowerOn())
	{
		AfxMessageBox("Homing Bin Ej FAIL", MB_SYSTEMMODAL);
		return FALSE;
	}
	BinEj_MoveTo(1800);		//andrewng-0529		//BinEj encoder is reversed

	//andrewng //2020-0831
	BinEjCap_Home();
	if (!BinEjCap_IsPowerOn())
	{
		AfxMessageBox("Homing Bin Ej CAP FAIL", MB_SYSTEMMODAL);
		return FALSE;
	}
	BinEjCap_MoveTo(1000);

	if (m_bEnableEjectorTheta)
	{
		BinEjT_Home();
		if (!BinEjT_IsPowerOn())
		{
			AfxMessageBox("Homing Bin EjT FAIL", MB_SYSTEMMODAL);
			return FALSE;
		}
		BinEjT_MoveTo(3000);
	}

	if (m_bMS60EjElevator)
	{
		BinEjElevator_Home();
		if (!BinEjElevator_IsPowerOn())
		{
			AfxMessageBox("Homing Bin Ej Elv FAIL", MB_SYSTEMMODAL);
			return FALSE;
		}

		//DOUBLE dTargetHomeZ = -38500.0 / m_dEjElevatorRes;
		DOUBLE dTargetHomeZ = -35000.0 / m_dEjElevatorRes;
		LONG lTargetZ = (LONG) dTargetHomeZ;
		BinEjElevator_MoveTo(dTargetHomeZ);
	}

	return TRUE;
}

