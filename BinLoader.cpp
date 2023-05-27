/////////////////////////////////////////////////////////////////
// BinLoader.cpp : interface of the CBinLoader class
//
//	Description:
//		
//
//	Date:		12 August 2004
//	Revision:	1.00
//
//	By:			
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBinLoader, CMS896AStn)

CBinLoader::CBinLoader(): m_evBLOperate(FALSE, TRUE,		"BlOperateEvt"),
						  m_evBLReady(FALSE, TRUE,			"BlReadyEvt"),
						  m_evBLPreChangeGrade(FALSE, TRUE, "BlPreChangeGrade"),
						  m_evBLAOperate(FALSE, TRUE,		"BlAOperateEvt"),
						  m_evBLPreOperate(FALSE, TRUE,		"BlPreOperateEvt"),
						  m_evBLFrameLevelDn(FALSE, TRUE,	"BlFrameLevelDn")		//v4.01
{
	m_szEventPrefix			= "BL";
	InitVariable();
}

CBinLoader::~CBinLoader()
{
}

BOOL CBinLoader::InitInstance()
{
	CMS896AStn::InitInstance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	(*m_psmfSRam)["BinLoaderStn"]["Enabled"] = !m_bDisableBL;
	m_bIsEnabled = TRUE;

	//Check if ExChange Arm is exist or not
	m_bIsExChgArmExist	= FALSE;	//(BOOL)GetChannelInformation(MS896A_CFG_CH_BINEXCHGARMR, MS896A_CFG_CH_ENABLE);
	(*m_psmfSRam)["BinLoaderStn"]["ExChgArmExist"] = m_bIsExChgArmExist;
	m_bGripperPusher = m_bIsExChgArmExist;		//v2.93T2

	m_bUseLargeBinArea		= (BOOL)GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_LARGE_TRAVEL);
	//m_lBinLoaderConfig		= GetChannelInformation(MS896A_CFG_CH_BINGRIPPER,		MS896A_CFG_CH_BL_CONFIG);
	//m_bUseTimeBeltMotorY	= GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y,		MS896A_CFG_CH_BLY_TIMEBELT_MOTOR);	//v3.94T3
	m_bDisableLoaderY		= TRUE;	//GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y,		MS896A_CFG_CH_DISABLE);				//v4.55A7	//MS60 Autoline
	BOOL bUseTheta			= GetChannelInformation(MS896A_CFG_CH_BINLOADER_THETA,	MS896A_CFG_CH_ENABLE);				//v4.59A40	//MS50
	
	//Default, it is for standard
	//For MS50
	m_lBinLoaderConfig = BL_CONFIG_DL_WITH_UPDN_BUFFER;	
	m_bDualBufferPreloadLevel = BL_BUFFER_UPPER;	//default is Upper

	m_bIsDualBufferExist = TRUE;
	(*m_psmfSRam)["BinLoaderStn"]["DualBufferExist"] = TRUE;
	//v3.86		//Change all D-Buffer to new design (except MS100 eng machine)
	m_bIsNewDualBuffer = TRUE;	//GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_BL_NEW_DUAL_BUFFER);
	
	m_bNewZConfig	= FALSE;
	
	BOOL bNewStepperEnc		= pApp->GetFeatureStatus(MS896A_FUNC_NEW_STEPPER_ENC);
	m_bGripperUseEncoder	= (BOOL) GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_ENABLE_ENCODER);
	BOOL bEnableBufPusher	= FALSE; //(BOOL) GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, "Enable Buffer Pusher");
	
	m_dThetaXRes = 1.6; //standard theta
	m_lBIN_THETA_MISSING_STEP_TOL = 20;
	if (pApp->m_bEnableThetaWithBrake)
	{
		m_lBIN_THETA_MISSING_STEP_TOL = 10000; //_round(m_lBIN_THETA_MISSING_STEP_TOL * 4.4);
		m_dThetaXRes = 1.6 * 4.4;
	}

	if (bNewStepperEnc)
	{
		m_dGripperXRes = 8;
	}
	else
	{
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
		{
			//MS50
			m_dGripperXRes = 1.6;
		}
		else if (CMS896AApp::m_bMS60)		//v4.46T28
		{
			m_dGripperXRes = 1.0;
		}
		else if (m_lBinLoaderConfig == BL_CONFIG_DUAL_DL)
		{
			m_dGripperXRes = 1.6;
		}
		else
		{
			m_dGripperXRes = 0.8;
		}
	}

	try
	{
		m_bSel_X		= TRUE;
		m_bSel_Z		= TRUE;
		m_bSel_Arm		= TRUE;
		
		//MS50 config	//v4.59A40
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
		{
			m_bSel_X		= FALSE;
			m_bSel_Z		= TRUE;
			m_bSel_Theta	= TRUE;
			m_bSel_Upper	= TRUE;
			m_bSel_Lower	= TRUE;
		}

		GetAxisInformation();	//NuMotion

		if (m_fHardware && !m_bDisableBL)		//v3.60
		{
			//Get Channel attribute from config file for control type
//			LONG lDynamicControlY	= GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_DYNA_CONTROL);				
//			LONG lStaticControlY	= GetChannelInformation(MS896A_CFG_CH_BINLOADER_Y, MS896A_CFG_CH_STAT_CONTROL);				
//			LONG lMotorDirectionX	= GetChannelInformation(MS896A_CFG_CH_BINGRIPPER, MS896A_CFG_CH_MOTOR_DIRECTION);

			//Setup Static & dynamic control type
			LONG lDynamicControlZ = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_DYNA_CONTROL);
			if (lDynamicControlZ != 0)
			{
				//m_pServo_Z->SelectControlType(lDynamicControlZ);
				CMS896AStn::MotionSelectControlType(BL_AXIS_Z, lDynamicControlZ, 2, &m_stBLAxis_Z);
			}

			LONG lStaticControlZ = GetChannelInformation(MS896A_CFG_CH_BINLOADER_Z, MS896A_CFG_CH_STAT_CONTROL);
			if (lStaticControlZ != 0)
			{
				//m_pServo_Z->SelectStaticControlType(lStaticControlZ);   
				CMS896AStn::MotionSelectControlType(BL_AXIS_Z, lStaticControlZ, 1, &m_stBLAxis_Z);
			}

			// Reverse motor direction if needed
			LONG lMotorDirectionUpperX = GetChannelInformation(MS896A_CFG_CH_BINLOADER_UPPER, MS896A_CFG_CH_MOTOR_DIRECTION);
			if (lMotorDirectionUpperX == -1)
			{
				CMS896AStn::MotionSetEncoderDirection(BL_AXIS_UPPER,	HP_NEGATIVE_DIR,	&m_stBLAxis_Upper);
				CMS896AStn::MotionReverseMotorDirection(BL_AXIS_UPPER,	HP_ENABLE,			&m_stBLAxis_Upper);
			}

			LONG lMotorDirectionLowerX = GetChannelInformation(MS896A_CFG_CH_BINLOADER_LOWER, MS896A_CFG_CH_MOTOR_DIRECTION);
			if (lMotorDirectionLowerX == -1)
			{
				CMS896AStn::MotionSetEncoderDirection(BL_AXIS_LOWER,	HP_NEGATIVE_DIR,	&m_stBLAxis_Lower);
				CMS896AStn::MotionReverseMotorDirection(BL_AXIS_LOWER,	HP_ENABLE,			&m_stBLAxis_Lower);
			}

			//Only allow elevator to move if no frame blocks the frame-out sensor; for safety checking
			INT nCount = 0;
			while (IsFrameOutOfMgz())	//v3.34
			{
				#ifdef OFFLINE
					break;
				#endif
				AfxMessageBox(_T("Bin frame OUT-OF-MAG sensor is triggered; please remove output frame manually in loader area!."), MB_SYSTEMMODAL);
				nCount++;
				if (nCount >= 3)
				{
					if (AfxMessageBox("By-pass output loader OUT-OF-MAG sensor checking?", MB_SYSTEMMODAL|MB_OKCANCEL) != IDOK)
						return FALSE;
					break;
				}
			}

			//v4.01	//Check BT platform before homing BH T
#ifndef OFFLINE
			if ( !m_bDisableBL && (IsFrameLevel() == FALSE) )	//if at UP position (sensor flag not cut)	//v4.04
			{
				BTFramePlatformDown();
				AfxMessageBox(_T("Bin table #1 frame-level sensor is not at DOWN position!"), MB_SYSTEMMODAL);
			
				if ( IsFrameLevel() == FALSE )
				{
					if (AfxMessageBox(_T("Bin table #1 frame-level sensor is not at DOWN position; abort?"), MB_YESNO|MB_SYSTEMMODAL) == IDYES)
						return FALSE;
				}
			}
#endif
			SetBLFrameLevelDn(TRUE);	//Trigger BHT to home

			if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
			{
				nCount = 0;
				while (IsDualBufferUpperExist())	//Upper Buffer should have no frame, otherwise binloader X could not HOME
				{
					AfxMessageBox(_T("Bin frame exists in upper buffer; please remove frame manually to continue."), MB_SYSTEMMODAL);
					nCount++;
					if (nCount >= 3)
					{
						if (AfxMessageBox("By-pass buffer-table upper sensor checking?", MB_SYSTEMMODAL|MB_OKCANCEL) != IDOK)
							return FALSE;
						break;
					}
				}
				
				nCount = 0;
				if ( IsDualBufferLowerExist() )
				{
					SetBufferLevel(FALSE);
					Sleep(300);

					if ( IsDualBufferLevelDown() == FALSE )
					{
						AfxMessageBox(_T("Bin frame exists in lower buffer and buffer cannot be down; machine startup is aborted."), MB_SYSTEMMODAL);
						return FALSE;
					}
				}
				
			//	if ( !IsDualBufferLeftProtect() || !IsDualBufferRightProtect() )
			//	{
			//		AfxMessageBox(_T("Bin frame is out of buffer table; machine startup is aborted."), MB_SYSTEMMODAL);
			//		return FALSE;
			//	}

				nCount = 0;
				while (!CheckMS90BugPusherAtSafePos(bEnableBufPusher))	//Upper Buffer should have no frame, otherwise binloader X could not HOME
				{
					AfxMessageBox(_T("Bin Table Pusher not at SAFE position!"), MB_SYSTEMMODAL);
					nCount++;
					if (nCount >= 3)
					{
						if (AfxMessageBox("By-pass Bin Table Pusher sensor checking?", MB_SYSTEMMODAL|MB_OKCANCEL) != IDOK)
							return FALSE;
						break;
					}
				}
			}

			// Home the motors
			if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A46
			{
				//UPPER = X in MS90
				if (Upper_Home() == Err_BinGripperMoveHome)
				{
					AfxMessageBox("BinLoaderStn - Bin Gripper search home fail");
					return TRUE;
				}

				//LOWER = X2 in MS109
				if (Lower_Home() == Err_BinGripperMoveHome)
				{
					AfxMessageBox("BinLoaderStn - Bin Gripper 2 search home fail");
					return TRUE;
				}

				Theta_Home();

				if (m_bHome_Upper && m_bHome_Lower)
				{
					Z_Home();
				}

				// Clear the last warning
				CMS896AStn::MotionSetLastWarning(BL_AXIS_UPPER, HP_SUCCESS, &m_stBLAxis_Upper);
				CMS896AStn::MotionSetLastWarning(BL_AXIS_LOWER, HP_SUCCESS, &m_stBLAxis_Lower);
				CMS896AStn::MotionSetLastWarning(BL_AXIS_THETA, HP_SUCCESS, &m_stBLAxis_Theta);
				CMS896AStn::MotionSetLastWarning(BL_AXIS_Z, HP_SUCCESS, &m_stBLAxis_Z);
			}

			// Create Barcode Scanner
			//CreateBarcodeScanner();
			//CMS896AStn::MotionEnableEncoder(BL_AXIS_X, &m_stBLAxis_X);
			//CMS896AStn::MotionEnableProtection(BL_AXIS_Z, HP_LIMIT_SENSOR, TRUE, FALSE, &m_stBLAxis_Z);

		}
		else if (m_fHardware && m_bDisableBL)		//v3.60
		{
			m_bSel_X		= FALSE;
			//m_bSel_Y		= FALSE;
			m_bSel_Z		= FALSE;
			//m_bSel_Arm	= FALSE;
			m_bSel_Theta	= FALSE;
			m_bSel_Upper	= FALSE;
			m_bSel_Lower	= FALSE;
		}

		m_bHardwareReady = TRUE;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}


INT CBinLoader::ExitInstance()
{
	SetBufferLevel(FALSE);
	SetFrameLevel(FALSE);
	
	ShutDown();

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		//SetFrameLevel2(FALSE);
		SetLoaderX2Power(FALSE);
		SetLoaderTPower(FALSE);	
	}

	//SaveData();					// Save data to file before exit
    SetLoaderXPower(FALSE);			// off all motors
    SetLoaderZPower(FALSE);
    SetGripperLevel(FALSE);

	return CMS896AStn::ExitInstance();
}

BOOL CBinLoader::InitData()
{
	// Load data from SMF
	ValidateBLBarcodeData();

	if (IsLoadingPKGFile() == TRUE && IsLoadingPortablePKGFile() == TRUE)
	{
		LoadBinLoaderOption();
		SaveData();

		//v4.19	//Ubilux
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Ubilux")
		{
		}
		//else
		//	ResetAllMagazine();
	}
    else if (IsLoadingPKGFile() == TRUE)		//v4.57A12	//Added else
	{
		LoadData();
		LoadBinLoaderOption();
		SaveData();

		LoadMgznOMData();				//v4.57A12	//Finisar, if BLMgzSetup.msd is added into \Exe\DF_FileList.msd

		//v4.19	//Ubilux
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Ubilux")
		{
		}
		//else
		//	ResetAllMagazine();
	}
	else
	{
		LoadBinLoaderRunTimeData();
		LoadData();
		LoadBinLoaderOption();

		// Load Magazeine default setting data
		LoadMgznOMData();
		// Load Magazeine current setting data
		//LoadMgznRTData();			//andrewng //2020-0721
	}
	
	
	//Turn On Alignment & Vacuum if necessary
	if ( (m_fHardware == TRUE) && !m_bDisableBL )
	{
		if ( (CheckFrameOnBinTable() == BL_FRAME_ON_CENTER) && (GetCurrBinOnBT() != 0) )
		{
			SetFrameAlign(TRUE);
			Sleep(500);
			SetFrameVacuum(m_bUseBinTableVacuum);
		}		

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A45
		{
			GetEncoderValue();
			Theta_MoveTo(m_lReadyPos_T, SFM_NOWAIT);
			Z_SMoveTo(m_lReadyLvlZ, SFM_NOWAIT);
		}
		else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			if ( IsDualBufferUpperExist() )
			{
				SetErrorMessage("BL Frame exists in Upper Buffer");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			}
			else if ( IsDualBufferLowerExist() )
			{
				SetBufferLevel(FALSE);
				Sleep(300);

				if ( IsDualBufferLevelDown() == FALSE )
				{
					SetErrorMessage("BL Frame exists in Lower Buffer");
					SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
				}
				else
				{
					X_Home();
				}
			}
			else
			{
				X_Home();
			}

			CMS896AStn::MotionSetSoftwareLimit(BL_AXIS_X, 2000, -26500, &m_stBLAxis_X);		//v4.03	
			GetEncoderValue();

			if (!IsFrameOutOfMgz())	
			{
				//Make sure FRAme-OUT sensor is not blocked before elevator motion.
				Z_MoveTo(m_lReadyLvlZ, SFM_NOWAIT);
			}
		}
	}


	// Create Barcode Scanner 
	CreateBarcodeScanner();
	if (m_lBinLoaderConfig == BL_CONFIG_DUAL_DL)	//MS100 9Inch dual-table config		//v4.21
		CreateBarcodeScanner2();

	if (m_bDisableBL)	//v3.94
	{
		m_bNoSensorCheck	= TRUE;
		BL_DEBUGBOX("disable bl in init data");

		m_lBTCurrentBlock	= 0;
		m_lBTCurrentBlock2	= 0;
		m_lExArmBufferBlock = 0;
		m_szBinFrameBarcode = "";
		m_szBinFrame2Barcode = "";
		m_szBufferFrameBarcode = "";
		m_clUpperGripperBuffer.SetBufferBarcode("");
		m_clLowerGripperBuffer.SetBufferBarcode("");
		m_lCurrMgzn			= 0;
		m_lCurrSlot			= 0;
		m_lCurrHmiMgzn		= 0;
		m_lCurrHmiSlot		= 0;
		m_lCurrMgzn2		= 0;
		m_lCurrSlot2		= 0;
		m_lCurrHmiSlot2		= 0;

		for(int i=0; i<MS_BL_MGZN_NUM; i++)
			ResetMagazine(i);
	}

	if (IsMSAutoLineMode())
	{
		SECS_InitCassetteSlotInfo();	//v4.55A7
	}
	InitMSAutoLineBLModule();		//autoline1

	//BackupStatusInfo();			//v2.93T2
	// Check if the log excess the file size limit and do the backup
	CMSLogFileUtility::Instance()->BL_BackupLogStatus();
	CMSLogFileUtility::Instance()->BL_BackupChangeGradeLog();
	CMSLogFileUtility::Instance()->BL_BackupLoadUnloadLog();
	CMSLogFileUtility::Instance()->BL_BackupBarcodeLog();
	CMSLogFileUtility::Instance()->BL_BackupLoadUnloadTimeLog();
	return TRUE;
}

VOID CBinLoader::ClearData()
{

}

// Get all sensor values from hardware
VOID CBinLoader::GetSensorValue()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CString szAxis = "";
	try
	{
		// serial io bit detection
		m_bFrameInClampDetect   = CMS896AStn::MotionReadInputBit(BL_SI_FrameDetect);
		m_bFrameInClampDetect2	= CMS896AStn::MotionReadInputBit(BL_SI_FrameDetect2);

		m_bFrameOnBTPosition	= !CMS896AStn::MotionReadInputBit(BL_SI_FramePos);
		m_bFrameProtection		= CMS896AStn::MotionReadInputBit(BL_SI_FrameProtect);
		m_bFrameInMagazine		= CMS896AStn::MotionReadInputBit(BL_SI_FrameInMag);
		
		if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())		//autoline1
		{
//			m_bMagazineExist[0] = !CMS896AStn::MotionReadInputBit(BL_SI_MagExist);

			m_bFrontGateSensor	= IsFrontGateSensorOn();
			m_bBackGateSensor	= IsBackGateSensorOn();
		}

		if ((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) ||
			CMS896AStn::m_bUseBinMultiMgznSnrs)
		{
			if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())
			{
				m_bMagazineExist[0] = TRUE;
			}
			else
			{
				m_bMagazineExist[0] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist1);
			}
			m_bMagazineExist[1] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist2);
			m_bMagazineExist[2] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist3);
			m_bMagazineExist[3] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist4);
			m_bMagazineExist[4] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist5);
			m_bMagazineExist[5] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist6);
			m_bMagazineExist[6] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist7);
			m_bMagazineExist[7] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist8);	
		}
		else
		{
			if( pApp->GetCustomerName()==CTM_WOLFSPEED )
				m_bMagazineExist[0] =  CMS896AStn::MotionReadInputBit(BL_SI_MagExist);
			else
				m_bMagazineExist[0] =  CMS896AStn::MotionReadInputBit(BL_SI_MagExist);
		}

		IsBufferSafetySensor();
		IsElevatorDoorCoverOpen();
		IsFrontRightElevatorDoorCoverOpen();

		m_bFrameOutOfMgz	= CMS896AStn::MotionReadInputBit(BL_SI_FrameOutMag) || CMS896AStn::MotionReadInputBit(BL_SI_FrameOutMag2);

		IsFrameLevel();

		if ((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER) ||
			(m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) )
		{
			//Dual Buffer Table Sensors
			IsDualBufferUpperExist();	//TRUE = exist; FALSE = not
			IsDualBufferLowerExist();	//TRUE = exist; FALSE = not	
			IsPressureSensorAlarmOn();
		}

		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			m_bDualBufferRightProtect	= CMS896AStn::MotionReadInputBit(BL_SI_DualBufferRightProtect);	//TRUE = safe; FALSE = out position
			m_bDualBufferLeftProtect	= CMS896AStn::MotionReadInputBit(BL_SI_DualBufferLeftProtect);	//TRUE = safe; FALSE = out position
			m_bDualBufferLevel			= !CMS896AStn::MotionReadInputBit(BL_SI_DualBufferLevel);		//TRUE = down; FALSE = up
		}

		IsFrameJam();
		IsFrameJam2();

		//v4.59A40	//Finisar TX MS90 new TableAlign pusher
		//it is not for MS50
		//IsMS90BufPusherXAtLimitPos();
		//IsMS90BufPusherXAtHomePos();
		//IsMS90BufPusherZAtSafePos();

		//m_bHomeSnr_X	= CMS896AStn::MotionIsHomeSensorHigh(szAxis,	&m_stBLAxis_X);
		//m_bHomeSnr_Y	= CMS896AStn::MotionIsHomeSensorHigh(szAxis,	&m_stBLAxis_Y);

		m_bHomeSnr_Z = IsHomeSensorHigh(BL_AXIS_Z, &m_stBLAxis_Z);
		
		m_bHomeSnr_Upper = IsHomeSensorHigh(BL_AXIS_UPPER, &m_stBLAxis_Upper);
		m_bHomeSnr_X	= m_bHomeSnr_Upper;

		m_bHomeSnr_Lower = IsHomeSensorHigh(BL_AXIS_LOWER, &m_stBLAxis_Lower);
		m_bHomeSnr_X2	= m_bHomeSnr_Lower;

		m_bHomeSnr_Theta = IsHomeSensorHigh(BL_AXIS_THETA, &m_stBLAxis_Theta);

		m_bULimitSnr_Y	= m_bLLimitSnr_Y = FALSE;
		m_bULimitSnr_Z	= m_bLLimitSnr_Z = FALSE;

	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}




VOID CBinLoader::GetAxisInformation()
{
/*
	InitAxisData(m_stBLAxis_X);
	m_stBLAxis_X.m_szName				= BL_AXIS_X;
	m_stBLAxis_X.m_szTag				= MS896A_CFG_CH_BINGRIPPER;
	m_stBLAxis_X.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBLAxis_X);

	InitAxisData(m_stBLAxis_Y);
	m_stBLAxis_Y.m_szName				= BL_AXIS_Y;
	m_stBLAxis_Y.m_szTag				= MS896A_CFG_CH_BINLOADER_Y;
	m_stBLAxis_Y.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBLAxis_Y);
*/
	InitAxisData(m_stBLAxis_Z);
	m_stBLAxis_Z.m_szName				= BL_AXIS_Z;
	m_stBLAxis_Z.m_szTag				= MS896A_CFG_CH_BINLOADER_Z;
	m_stBLAxis_Z.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stBLAxis_Z);
	m_stBLAxis_Z.m_dEncResolution = 1;

	//v4.59A41
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		InitAxisData(m_stBLAxis_Upper);
		m_stBLAxis_Upper.m_szName				= BL_AXIS_UPPER;
		m_stBLAxis_Upper.m_szTag				= MS896A_CFG_CH_BINLOADER_UPPER;
		m_stBLAxis_Upper.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBLAxis_Upper);
		m_stBLAxis_Upper.m_dEncResolution = m_dGripperXRes;

		InitAxisData(m_stBLAxis_Lower);
		m_stBLAxis_Lower.m_szName				= BL_AXIS_LOWER;
		m_stBLAxis_Lower.m_szTag				= MS896A_CFG_CH_BINLOADER_LOWER;
		m_stBLAxis_Lower.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBLAxis_Lower);
		m_stBLAxis_Lower.m_dEncResolution = m_dGripperXRes;

		InitAxisData(m_stBLAxis_Theta);
		m_stBLAxis_Theta.m_szName				= BL_AXIS_THETA;
		m_stBLAxis_Theta.m_szTag				= MS896A_CFG_CH_BINLOADER_THETA;
		m_stBLAxis_Theta.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBLAxis_Theta);
		m_stBLAxis_Theta.m_dEncResolution = m_dThetaXRes;

		PrintAxisData(m_stBLAxis_Upper);
		PrintAxisData(m_stBLAxis_Lower);
		PrintAxisData(m_stBLAxis_Theta);
	}

	//For debug only
//	PrintAxisData(m_stBLAxis_X);
//	PrintAxisData(m_stBLAxis_Y);
	PrintAxisData(m_stBLAxis_Z);
	m_stBLAxis_X.m_dEncResolution = m_dGripperXRes;
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CBinLoader::UpdateOutput()
{
	static int nCount = 0;

	if (!m_fHardware)
		return;


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
		//Update sensor here
		GetSensorValue();
		GetEncoderValue();
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

VOID CBinLoader::UpdateProfile()
{
	//Add Your Code Here
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csMachine);

	m_bUpdateProfile = FALSE;

	try
	{
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	slLock.Unlock();
}

VOID CBinLoader::UpdatePosition()
{
	//Add Your Code Here
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CSingleLock slLock(&pAppMod->m_csDevice);
	
	m_bUpdatePosition = FALSE;

	try
	{
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	slLock.Unlock();
}

