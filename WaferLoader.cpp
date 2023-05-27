/////////////////////////////////////////////////////////////////
// WaferLoader.cpp : interface of the CWaferLoader class
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
#include "WaferLoader.h"
#include "FileUtil.h"
#include "winuser.h"
#include "MS_SecCommConstant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CWaferLoader, CMS896AStn)

CWaferLoader::CWaferLoader(): m_evWTInitT(FALSE, TRUE, "WTInitTEvt"),
							  m_evExpInit(FALSE, TRUE, "ExpInitTEvt"),
							  m_evInitAFZ(FALSE, TRUE, "InitAFZ"),
							  m_evES101WldPreloadDone(FALSE, TRUE, "ES101WldPreloadDoneEvt"),
							  m_evExpInitForWt(FALSE, TRUE, "ExpInitForWtTEvt")
{
	InitVariable();
}

CWaferLoader::~CWaferLoader()
{
}


BOOL CWaferLoader::InitInstance()
{
	CMS896AStn::InitInstance();

	(*m_psmfSRam)["WaferLoaderStn"]["Enabled"] = TRUE;
	m_bIsEnabled = TRUE;

	if (m_bDisableWL)
	{
		m_bNoSensorCheck = TRUE;
		m_bIsEnabled = FALSE;
		(*m_psmfSRam)["WaferLoaderStn"]["Enabled"] = FALSE;
	}

	m_bIsExpDCMotorExist = (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFEXPDCMOTOR, MS896A_CFG_CH_ENABLE);
	if (!IsWaferTableWithExpander())
	{
		m_bIsExpDCMotorExist = FALSE;
	}

	if (m_bIsExpDCMotorExist)	//v4.38T6	//Knowles Peneng
	{
		m_bExpDCMotorUseEncoder =  (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFEXPDCMOTOR, MS896A_CFG_CH_ENABLE_ENCODER);
	}
	m_bWLXHasEncoder = (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFGRIPPER, MS896A_CFG_CH_ENABLE_ENCODER);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()==CTM_NICHIA && IsMS90() )
	{
		m_bWLXHasEncoder = TRUE;
	}

	if (CMS896AApp::m_bES100v2DualWftOption == TRUE)
	{
		m_bUseDualTablesOption	= TRUE;

		//v4.34T10	//ES201
		m_bES201DisableTable2	= (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFTABLE2_X, MS896A_CFG_CH_DISABLE);	//v4.34T9
		m_bSel_X2 = TRUE;
		m_bSel_Z2 = TRUE;
	}

	if (CMS896AApp::m_bMS100SingleLoaderOption == TRUE)		//v4.31T10	//Yearly MS100Plus use BL
	{
		m_bUseBLAsLoaderZ = TRUE;
	}

	try
	{
		GetAxisInformation();	//NU_Motion

		if (m_fHardware && !m_bDisableWL)
		{
			if (m_bDisableWLWithExp == TRUE)
			{
				m_bSel_Z = FALSE;
				m_bSel_X = FALSE;
			}

			// Wait WT T motor INIT complete before checking expander
			if ( m_bWprWithAF_Z && (WaitWprInitAFZ(300000) == FALSE) )				
			{
				DisplayMessage("WaferLoader - Timeout when wait WPR AF Z init ");
				return FALSE;
			}

			// Wait WT T motor INIT complete before checking expander
			if ( !m_bDisableWT && WaitWTInitT(300000) == FALSE )	//v4.16T1			
			{
				DisplayMessage("WaferLoader - Timeout when wait WT T init ");
				return FALSE;
			}

			if (m_bIsExpDCMotorExist && m_bExpDCMotorUseEncoder)	//v4.38T6	//Knowles Peneng
			{
				CMS896AStn::MotionEnableEncoder(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
			}

			CWinApp* pApp = AfxGetApp();
			m_lExpanderType		= pApp->GetProfileInt(gszPROFILE_SETTING, gszEXPANDER_TYPE, 100);
			m_ucBarcodeModel	= pApp->GetProfileInt(gszPROFILE_SETTING, gszWL_BC_READER_MODEL, 100);
			// open config file for expander type checking
			//Check gripper condition
			if (IsFrameDetect() == TRUE)
			{
				AfxMessageBox("Frame is detected on wafer input gripper; please remove it now!");
			}

			BOOL bVacuumMode = FALSE;
			bVacuumMode = (BOOL)GetChannelInformation(MS896A_CFG_CH_WAFLOADER_Z, MS896A_CFG_EXPANDER_VACUUM_MODE);
			// Check expander
			if (GetExpType() == WL_EXP_VACUUM || bVacuumMode)
			{
				CheckVacuumExpander();
			}
			else if (GetExpType() == WL_EXP_NONE)	//v4.39T10	//Knowles MS109
			{
			}
			else
			{
				if (!m_bDisableWT)		//v4.16T1
				{
					INT nStatus = CheckExpander();
					if (nStatus == Err_ExpanderLockFailed)			// if cannot engage
					{
						nStatus = ManualCheckExpander();			// try manual-engage with T motor off
						if (nStatus == Err_ExpanderLockFailed) {	// if still cannot engage
							DisplayMessage("WaferLoader - engage-expander fail");
							return TRUE;
						}
					}
				}

				if ( IsESDualWL() )
				{
					if (!m_bDisableWT)		//v4.16T1
					{
						INT nStatus = CheckExpander2();
						if (nStatus == Err_ExpanderLockFailed)			// if cannot engage
						{
							nStatus = ManualCheckExpander2();			// try manual-engage with T motor off
							if (nStatus == Err_ExpanderLockFailed) {	// if still cannot engage
								DisplayMessage("WaferLoader - engage-expander fail");
								return TRUE;
							}
						}
					}
					
				}
			}

			SetExpInit(TRUE);		// Trigger BH-T_Home
			SetExpInitForWt(TRUE);	// Trigger WT XY home	//v2.63
			
			// Home the motors
			X_Home();
			Z_Home();

			if (m_bUseDualTablesOption == FALSE)
			{
				Z_MoveTo(m_lTopSlotLevel_Z, SFM_NOWAIT);
			}

			if ( IsESDualWL() )
			{
				X2_Home();
				Z2_Home();
			}

			if (m_bIsExpDCMotorExist && m_bExpDCMotorUseEncoder)	//v4.38T6	//Knowles Peneng
			{
				CMS896AStn::MotionSetPosition(WL_AXIS_EXP_Z, 0, &m_stWLExpAxis_Z);
			}

			//v4.40T10	//ES101 with DEFAULT_COM testing; moved to WarmStart InitData() fcn
/*
			// Create Barcode Scanner
			CreateBarcodeScanner();
			if ( IsESDualWL() )
			{
				CreateBarcodeScanner2();
			}
*/
			if (m_bDisableWLWithExp != TRUE)
			{
				// Init the XY motors
				CMS896AStn::MotionEnableEncoder(WL_AXIS_Z, &m_stWLAxis_Z);
				CMS896AStn::MotionEnableEncoder(WL_AXIS_X, &m_stWLAxis_X);

				if (CMS896AApp::m_bIsPrototypeMachine)
					CMS896AStn::MotionEnableProtection(WL_AXIS_Z, HP_LIMIT_SENSOR, TRUE, 1, &m_stWLAxis_Z); //prototype MS100
				//else
				//{
				//	CMS896AStn::MotionEnableProtection(WL_AXIS_Z, HP_LIMIT_SENSOR, FALSE, 1, &m_stWLAxis_Z);
				//}

				// Clear the last warning
				CMS896AStn::MotionSetLastWarning(WL_AXIS_X, HP_SUCCESS, &m_stWLAxis_X);
				CMS896AStn::MotionSetLastWarning(WL_AXIS_Z, HP_SUCCESS, &m_stWLAxis_Z);
			}

			if( m_bWLXHasEncoder )
			{
				CMS896AStn::MotionEnableEncoder(WL_AXIS_X, &m_stWLAxis_X);
			}
		}
		else if (m_fHardware && m_bDisableWL)
		{
			m_bSel_Z = FALSE;
			m_bSel_X = FALSE;
			SetExpInit(TRUE);		// Trigger BH-T_Home
			SetExpInitForWt(TRUE);	// Trigger WT XY home
			// Create Barcode Scanner
			CreateBarcodeScanner();
		}

		m_bHardwareReady = TRUE;
	}
	catch (CAsmException e) {
		DisplayMessage("WL: InitInstance main loop exception!");
		DisplayException(e);
	}
	return TRUE;
}


INT CWaferLoader::ExitInstance()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if(m_fHardware && pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA))
	{
		PR_UBYTE	ubSendID	= ES_CONTOUR_CAM_SEND_ID;
		PR_UBYTE	ucRecvID	= ES_CONTOUR_CAM_RECV_ID;
		PR_FREE_RECORD_ID_CMD		stFreeCmd;
		PR_FREE_RECORD_ID_RPY		stFreeRpy;
		if( m_uwWL1ContourRecordID != 0 )
		{
			PR_InitFreeRecordIDCmd(&stFreeCmd);
			stFreeCmd.uwRecordID = (PR_UWORD)(m_uwWL1ContourRecordID);
			PR_FreeRecordIDCmd(&stFreeCmd, ubSendID, ucRecvID, &stFreeRpy);
		}
		if( m_uwWL2ContourRecordID != 0 )
		{
			PR_InitFreeRecordIDCmd(&stFreeCmd);
			stFreeCmd.uwRecordID = (PR_UWORD)(m_uwWL2ContourRecordID);
			PR_FreeRecordIDCmd(&stFreeCmd, ubSendID, ucRecvID, &stFreeRpy);
		}
		DeleteFile(ES_CONTOUR_WL1_FILE);
		DeleteFile(ES_CONTOUR_WL2_FILE);
	}

	//SaveData();				// Save data to file before exit
	//SaveExitData();
	if (GetBCModel() == WL_BAR_DEFAULT)	//v4.38T1
		UnhookWindowsHookEx(CMS896AApp::m_hKeyboardHook);

	if (m_fHardware && !m_bDisableWL && !m_bDisableWLWithExp)	//v3.61
	{
		try
		{
			CMS896AStn::MotionSync(WL_AXIS_X, 10000, &m_stWLAxis_X);
			CMS896AStn::MotionPowerOff(WL_AXIS_X, &m_stWLAxis_X);
			CMS896AStn::MotionClearError(WL_AXIS_X, HP_MOTION_ABORT, &m_stWLAxis_X);
		}
		catch (CAsmException e)
		{
			// Nothing can do ...
		}

		try
		{
			CMS896AStn::MotionSync(WL_AXIS_Z, 10000, &m_stWLAxis_Z);
			CMS896AStn::MotionPowerOff(WL_AXIS_Z, &m_stWLAxis_Z);
			CMS896AStn::MotionClearError(WL_AXIS_Z, HP_MOTION_ABORT, &m_stWLAxis_Z);
		}
		catch (CAsmException e)
		{
			// Nothing can do ...
		}

		//New Expander DC motor in MS100+ NuMotion platform
		if (m_bIsExpDCMotorExist)			//v4.01
		{
			try
			{
				CMS896AStn::MotionSync(WL_AXIS_EXP_Z, 10000, &m_stWLExpAxis_Z);
				CMS896AStn::MotionPowerOff(WL_AXIS_EXP_Z, &m_stWLExpAxis_Z);
				CMS896AStn::MotionClearError(WL_AXIS_EXP_Z, HP_MOTION_ABORT, &m_stWLExpAxis_Z);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}

		if( IsESDualWL() )		//v4.24T4
		{
			try
			{
				CMS896AStn::MotionSync(WL_AXIS_X2, 10000, &m_stWLAxis_X2);
				CMS896AStn::MotionPowerOff(WL_AXIS_X2, &m_stWLAxis_X2);
				CMS896AStn::MotionClearError(WL_AXIS_X2, HP_MOTION_ABORT, &m_stWLAxis_X2);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}

			try
			{
				CMS896AStn::MotionSync(WL_AXIS_Z2, 10000, &m_stWLAxis_Z2);
				CMS896AStn::MotionPowerOff(WL_AXIS_Z2, &m_stWLAxis_Z2);
				CMS896AStn::MotionClearError(WL_AXIS_Z2, HP_MOTION_ABORT, &m_stWLAxis_Z2);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}

			if (m_bIsExpDCMotorExist)			//v4.01
			{
				try
				{
					CMS896AStn::MotionSync(WL_AXIS_EXP_Z2, 10000, &m_stWLExpAxis_Z2);
					CMS896AStn::MotionPowerOff(WL_AXIS_EXP_Z2, &m_stWLExpAxis_Z2);
					CMS896AStn::MotionClearError(WL_AXIS_EXP_Z2, HP_MOTION_ABORT, &m_stWLExpAxis_Z2);
				}
				catch (CAsmException e)
				{
					// Nothing can do ...
				}
			}
		}

		//if (GetBCModel() == WL_BAR_REGAL)
		//{
		//	RegalStartReadBarcode(FALSE);
		//}
	}

	return CMS896AStn::ExitInstance();
}

LRESULT CALLBACK WLKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG *msg;
	msg = (MSG *) lParam;

	if (nCode < 0)
	{
		return CallNextHookEx(CMS896AApp::m_hKeyboardHook, nCode, wParam, lParam);
	}

    PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
	if (CMS896AApp::m_bEnableKeyboardHook)
	{
		if ( (wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN) )
		//if (wParam == WM_KEYDOWN)
		{
			// key is in p->vkCode
			//UCHAR ucCode = (UCHAR) p->vkCode;

			//UCHAR ucCode = MapVirtualKey(p->vkCode, 2);
			UCHAR ucCode = (UCHAR)p->vkCode;

			if ((p->vkCode == VK_OEM_MINUS) || (p->vkCode == VK_SUBTRACT))
			{
				ucCode = '-';
			}
			else if ((p->vkCode >= 0x30) && (p->vkCode <= 0x39))	//0-9
			{
				ucCode = (UCHAR)('0' + (p->vkCode - 0x30));
			}
			else if ((p->vkCode >= 0x60) && (p->vkCode <= 0x69))	//NUMPAD 0-9
			{
				ucCode = (UCHAR)('0' + (p->vkCode - 0x60));
			}
			else if ((p->vkCode >= 0x41) && (p->vkCode <= 0x5A))	//A-Z
			{
				ucCode = (UCHAR)('A' + (p->vkCode - 0x41));
			}
			else if (p->vkCode == VK_SPACE)
			{
				ucCode = ' ';
			}
			else if ( (p->vkCode == 0x2E) || (p->vkCode == VK_DECIMAL) || (p->vkCode == VK_OEM_PERIOD) )	//v4.40T13
			{
				ucCode = '.';
			}
			else
			{
				return 1;
			}

			CString szCode;
			szCode.Format("%c", ucCode);

			//if ( (szCode != "\n") && (szCode != "\r") )
			//{
			CMS896AApp::m_szKeyboard = CMS896AApp::m_szKeyboard + szCode;
			//}
			//else
			//{
			//	return 1;
			//}

			if ( (szCode == "\n") || (szCode == "\r") || (szCode == "\n\r") )
			{
				return 1;
			}
		}
	}

    return CallNextHookEx(CMS896AApp::m_hKeyboardHook, nCode, wParam, lParam);
}

BOOL CWaferLoader::InitData()
{	
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (IsLoadingPKGFile() == TRUE && IsLoadingPortablePKGFile() == TRUE)	//Manual Load PPKG on menu
	{
		LoadWaferLoaderOption();
		SaveData();
	}
	else if ( (pApp->GetCustomerName() == "Lumileds") &&	//v4.46T21
			  IsLoadingPKGFile() == TRUE)	//Manual Load PKG file on menu	//v4.46T15	//PLLM Rebel
	{
		LoadWaferLoaderOption();			//Machine Independent settings, such as device options
		SaveData();							//Machine dependent parameters, such as encoder positions
	}
	else	//auto load PKG in WarmStart
	{
		LoadWaferLoaderOption();
		LoadData();

		//v4.40T10	//Moved from InitInstance to here for DEFAULT_COM testing in SanAn
		// Create Barcode Scanner
		CreateBarcodeScanner();
		if ( IsESDualWL() )
		{
			CreateBarcodeScanner2();
		}
	}

	if (m_fHardware)
	{
		MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo, 0, FALSE);	//v4.37T3
	}

	if (GetBCModel() == WL_BAR_DEFAULT)		//v4.38T1
	{
		HINSTANCE hInst = AfxGetInstanceHandle();
		CMS896AApp::m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, WLKeyboardProc, hInst, 0);	//v4.38T1
	}

	//ShowWaferLotData();
	ShowBarcodeInCassette();
	
	if (IsMSAutoLineMode())		//autoline1
	{
		SECS_InitCassetteSlotInfo();
	}
	InitMSAutoLineWLModule();

	//BackupStatusInfo();		//v2.93T2
	CMSLogFileUtility::Instance()->WL_BackupLogStatus();
	CMSLogFileUtility::Instance()->WL_BackupDebugLog();
	return TRUE;
}


VOID CWaferLoader::ClearData()
{
}


VOID CWaferLoader::UpdateWLMgzSensor()  // 4.51D1 SecsGem
{
	CString szAxis = "";
	BOOL bMagazineExist;

	if (m_fHardware)
	{
		try
		{

			if (m_bUseDualTablesOption)
			{
				m_bMagazineExist	= !CMS896AStn::MotionReadInputBit(WL_SI_MagExist);
				m_bWaferFrameDetect = !CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect);
			}
			else
			{
				bMagazineExist	= CMS896AStn::MotionReadInputBit(WL_SI_MagExist);
				if(!m_bMagazineExist && bMagazineExist) 
				{
					CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t>>> -- Sent S6F11 SG_CEID_LoadCassette 8002");
					//SendEvent(8002, TRUE);			// SG_CEID_LoadCassette 8002
					//HmiMessage("Send SecsGem 8002"); 
				}
				m_bMagazineExist = bMagazineExist;
				m_bWaferFrameDetect = CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szAxis);
			if (szAxis == WL_AXIS_X)
				CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_X);
			else
				CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_Z);
		}
	}

}
// Get all sensor values from hardware
VOID CWaferLoader::GetSensorValue()
{
	CString szAxis = "";
	BOOL bMagazineExist;

	if (m_fHardware)
	{
		try
		{
			//Common
			m_bScopeLevel		= CMS896AStn::MotionReadInputBit(WL_SI_ScopeLevel);
			m_bFrameDetect		= CMS896AStn::MotionReadInputBit(WL_SI_FrameDetect);

			if (m_bUseDualTablesOption)
			{
				m_bMagazineExist	= !CMS896AStn::MotionReadInputBit(WL_SI_MagExist);
				m_bWaferFrameDetect = !CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect);
			}
			else
			{
				bMagazineExist	= CMS896AStn::MotionReadInputBit(WL_SI_MagExist);
				if(!m_bMagazineExist && bMagazineExist)  
				{
					CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t>>> --Sent S6F11 SG_CEID_LoadCassette 8002");
					//OsramTrip 8/22
					//SendEvent(8002, TRUE);			// SG_CEID_LoadCassette // 8002 // v4.51D1 Secs
				}
				m_bMagazineExist = bMagazineExist;
				m_bWaferFrameDetect = CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect);
			}

			//Gear
			m_bExpanderClose	= CMS896AStn::MotionReadInputBit(WL_SI_ExpanderClose);
			m_bExpanderOpen		= CMS896AStn::MotionReadInputBit(WL_SI_ExpanderOpen);
			m_bExpanderLock		= CMS896AStn::MotionReadInputBit(WL_SI_PusherLock);
			m_bFrameExist		= CMS896AStn::MotionReadInputBit(WL_SI_FrameExist);

			if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())
			{
				m_bFrontGateSensor	= IsFrontGateSensorOn();
				m_bBackGateSensor	= IsBackGateSensorOn();
			}


			//Vacuum
    		m_bFramePosition	= CMS896AStn::MotionReadInputBit(WL_SI_FramePos);
    		m_bFrameProtection	= CMS896AStn::MotionReadInputBit(WL_SI_FrameProtection);
    		m_bExpanderVacuum	= CMS896AStn::MotionReadInputBit(WL_SI_ExpanderClose);
    		m_bExpanderLevel	= CMS896AStn::MotionReadInputBit(WL_SI_PusherLock);
			
			if ( IsESDualWL() )		//v4.24T4	//ES100v2
			{
				m_bMagazineExist2			= !CMS896AStn::MotionReadInputBit(WL_SI_MagExist2);
				m_bFrameDetect2				= CMS896AStn::MotionReadInputBit(WL_SI_FrameDetect2);
				m_bWaferFrameDetect2		= !CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect2);
				m_bExpander2Close			= CMS896AStn::MotionReadInputBit(WL_SI_Expander2Close);
				m_bExpander2Open			= CMS896AStn::MotionReadInputBit(WL_SI_Expander2Open);
				m_bExpander2Lock			= CMS896AStn::MotionReadInputBit(WL_SI_PusherLock2);
				m_bFrameExist2				= CMS896AStn::MotionReadInputBit(WL_SI_FrameExist2);

				m_bLeftLoaderCoverSensor	= IsLeftLoaderCoverOpen();
				m_bRightLoaderCoverSensor	= IsRightLoaderCoverOpen();

				if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
				{
					m_bFrameJam2 = !CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X2,	&m_stWLAxis_X2);
				}
				else
				{
					m_bFrameJam2 = CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X2,	&m_stWLAxis_X2);
				}
			}

			if (m_bUseBLAsLoaderZ)			//v4.31T11	//Yealy MS100 Single Loader with Buffer Table
			{
				IsUpperSlotFrameExist();
				IsLowerSlotFrameExist();
				IsBufferFrameProtectSafe();
			}

			szAxis = WL_AXIS_X;

			if (m_bUseDualTablesOption)		//v4.24T4	//ES101v2
			{
				if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
				{
					m_bFrameJam	= !CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X,	&m_stWLAxis_X);
				}
				else
				{
					m_bFrameJam	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X,	&m_stWLAxis_X);
				}
			}
			else if (IsWprWithAF())	// ES100
			{
				m_bFrameJam = CMS896AStn::MotionReadInputBit("iwFrameJam");
			}
			else
			{
				if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
				{
					m_bFrameJam	= !CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X,	&m_stWLAxis_X);
				}
				else
				{
					m_bFrameJam	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X,	&m_stWLAxis_X);
				}
			}

			m_bHomeSnr_X	= CMS896AStn::MotionIsHomeSensorHigh(WL_AXIS_X,		&m_stWLAxis_X);
#ifdef NU_MOTION
			m_bULimitSnr_X = FALSE;
			m_bLLimitSnr_X	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X,	&m_stWLAxis_X);		//Gripper X JAM sensor
#else
			m_bULimitSnr_X	= CMS896AStn::MotionIsPositiveLimitHigh(WL_AXIS_X,	&m_stWLAxis_X);
			m_bLLimitSnr_X	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X,	&m_stWLAxis_X);
#endif

			if ( IsESDualWL() )		//v4.24T4	//ES100v2
			{
				m_bHomeSnr_X2 = CMS896AStn::MotionIsHomeSensorHigh(WL_AXIS_X2,		&m_stWLAxis_X2);
				m_bULimitSnr_X2 = FALSE;
				m_bLLimitSnr_X2 = CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X2,	&m_stWLAxis_X2);		//Gripper X JAM sensor
			}

			szAxis = WL_AXIS_Z;

			m_bHomeSnr_Z	= CMS896AStn::MotionIsHomeSensorHigh(WL_AXIS_Z,		&m_stWLAxis_Z);
#ifdef NU_MOTION
			if (CMS896AApp::m_bIsPrototypeMachine)
			{
				m_bULimitSnr_Z	= CMS896AStn::MotionIsPositiveLimitHigh(WL_AXIS_Z,	&m_stWLAxis_Z);	//prototype MS100
				m_bLLimitSnr_Z	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_Z,	&m_stWLAxis_Z);
			}
			else
			{
				m_bULimitSnr_Z	= FALSE;
				m_bLLimitSnr_Z	= FALSE;
				//m_bULimitSnr_Z	= !CMS896AStn::MotionIsPositiveLimitHigh(WL_AXIS_Z,	&m_stWLAxis_Z);
				//m_bLLimitSnr_Z	= !CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_Z,	&m_stWLAxis_Z);
			}
#else
			m_bULimitSnr_Z	= CMS896AStn::MotionIsPositiveLimitHigh(WL_AXIS_Z,	&m_stWLAxis_Z);
			m_bLLimitSnr_Z	= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_Z,	&m_stWLAxis_Z);
#endif

			if ( IsESDualWL() )		//v4.24T4	//ES100v2
			{
				m_bHomeSnr_Z2 = CMS896AStn::MotionIsHomeSensorHigh(WL_AXIS_Z2,		&m_stWLAxis_Z2);
				m_bULimitSnr_Z2 = FALSE;
				m_bLLimitSnr_Z2 = FALSE;
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			//CheckHiPECResult(szAxis);
			if (szAxis == WL_AXIS_X)
				CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_X);
			else
				CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_Z);
		}
	}
}


// Get all encoder values from hardware
VOID CWaferLoader::GetEncoderValue()
{
	CString szAxis;
	
	if (m_bDisableWLWithExp)
		return;

	//if (m_fHardware && !m_bDisableWL)	//v3.61
	if (m_fHardware)
	{
		try 
		{
			szAxis = WL_AXIS_X;
			if (IsMS60())	//v4.46T28
			{
				m_lEnc_X = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stWLAxis_X);
			}
			else if ( MotionIsServo(szAxis, &m_stWLAxis_X) )
			{
				m_lEnc_X = CMS896AStn::MotionGetEncoderPosition(szAxis, 1.00, &m_stWLAxis_X);
			}
			else
			{
				if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.21T2	//MS100 9Inch	
					m_lEnc_X = CMS896AStn::MotionGetEncoderPosition(szAxis, 1.6, &m_stWLAxis_X);
				else
					m_lEnc_X = CMS896AStn::MotionGetEncoderPosition(szAxis, 0.8, &m_stWLAxis_X);
			}

			if( m_bWLXHasEncoder )
			{
				m_lEnc_X = CMS896AStn::MotionGetEncoderPosition(szAxis, 0.8, &m_stWLAxis_X);
			}

			if (IsMS60())		//v4.46T28
			{
				szAxis = WL_AXIS_Z;
				m_lEnc_Z = CMS896AStn::MotionGetCommandPosition(szAxis, &m_stWLAxis_Z);
			}
			else if ( IsESDualWL() )		//v4.24T4	//ES100v2
			{
				szAxis = WL_AXIS_X2;
				m_lEnc_X2 = CMS896AStn::MotionGetEncoderPosition(szAxis, 0.8,	&m_stWLAxis_X2);

				//Both WL Z motors uses AC servo motor (same as BL Z motor) instead of stepper motor 
				szAxis = WL_AXIS_Z2;
				m_lEnc_Z2 = CMS896AStn::MotionGetEncoderPosition(szAxis, 1,		&m_stWLAxis_Z2);
				szAxis = WL_AXIS_Z;
				m_lEnc_Z = CMS896AStn::MotionGetEncoderPosition( szAxis, 1,		&m_stWLAxis_Z);
			}
			else
			{
				szAxis = WL_AXIS_Z;
				if (CMS896AApp::m_bMS100Plus9InchOption)	//v4.21T2	//MS100 9Inch	
					m_lEnc_Z = CMS896AStn::MotionGetEncoderPosition(szAxis ,1.6, &m_stWLAxis_Z);
				else
					m_lEnc_Z = CMS896AStn::MotionGetEncoderPosition(szAxis ,0.8, &m_stWLAxis_Z);
			}


			if (m_bIsExpDCMotorExist && m_bExpDCMotorUseEncoder)	//v4.38T6	//Knowles Peneng
			{
				szAxis = WL_AXIS_EXP_Z;
				m_lEnc_ExpZ = _round(CMS896AStn::MotionGetEncoderPosition(szAxis, 1, &m_stWLExpAxis_Z) * 0.01);
			}

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			if (szAxis == WL_AXIS_X)
				CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_X);
			if (szAxis == WL_AXIS_X2)
			{
				if ( IsESDualWL() )
					CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_X2);
			}
			if (szAxis == WL_AXIS_Z2)
			{
				if ( IsESDualWL() )
					CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_Z2);
			}
			else
				CMS896AStn::MotionCheckResult(szAxis, &m_stWLAxis_Z);
		}
	}
}


VOID CWaferLoader::GetAxisInformation()
{
#ifdef NU_MOTION
	InitAxisData(m_stWLAxis_X);
	m_stWLAxis_X.m_szName				= WL_AXIS_X;
	m_stWLAxis_X.m_szTag				= MS896A_CFG_CH_WAFGRIPPER;
	m_stWLAxis_X.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stWLAxis_X);
	m_stWLAxis_X.m_dEncResolution		= 1;

	InitAxisData(m_stWLAxis_Z);
	m_stWLAxis_Z.m_szName				= WL_AXIS_Z;
	m_stWLAxis_Z.m_szTag				= MS896A_CFG_CH_WAFLOADER_Z;
	m_stWLAxis_Z.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stWLAxis_Z);
	m_stWLAxis_Z.m_dEncResolution		= 1;

	//New Expander DC motor in MS100+ NuMotion platform
	if (m_bIsExpDCMotorExist)			//v4.01
	{
		InitAxisData(m_stWLExpAxis_Z);
		m_stWLExpAxis_Z.m_szName		= WL_AXIS_EXP_Z;
		m_stWLExpAxis_Z.m_szTag			= MS896A_CFG_CH_WAFEXPDCMOTOR;
		m_stWLExpAxis_Z.m_ucControlID	= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stWLExpAxis_Z);
		m_stWLExpAxis_Z.m_dEncResolution		= 1;
	}

	if ( IsESDualWL() )			//v4.24T4	//ES100v2
	{
		InitAxisData(m_stWLAxis_X2);
		m_stWLAxis_X2.m_szName				= WL_AXIS_X2;
		m_stWLAxis_X2.m_szTag				= MS896A_CFG_CH_WAFGRIPPER2;
		m_stWLAxis_X2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stWLAxis_X2);
		m_stWLAxis_X2.m_dEncResolution		= 1;

		InitAxisData(m_stWLAxis_Z2);
		m_stWLAxis_Z2.m_szName				= WL_AXIS_Z2;
		m_stWLAxis_Z2.m_szTag				= MS896A_CFG_CH_WAFLOADER2_Z;
		m_stWLAxis_Z2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stWLAxis_Z2);
		m_stWLAxis_Z2.m_dEncResolution		= 1;

		if (m_bIsExpDCMotorExist)
		{
			InitAxisData(m_stWLExpAxis_Z2);
			m_stWLExpAxis_Z2.m_szName		= WL_AXIS_EXP_Z2;
			m_stWLExpAxis_Z2.m_szTag		= MS896A_CFG_CH_WAFEXPDCMOTOR2;
			m_stWLExpAxis_Z2.m_ucControlID	= PL_DYNAMIC;		//Use Dynamic as default
			GetAxisData(m_stWLExpAxis_Z2);
			m_stWLExpAxis_Z2.m_dEncResolution		= 1;
		}

		PrintAxisData(m_stWLAxis_X2);
		PrintAxisData(m_stWLAxis_Z2);
		PrintAxisData(m_stWLExpAxis_Z2);
	}


	//For debug only
	PrintAxisData(m_stWLAxis_X);
	PrintAxisData(m_stWLAxis_Z);
	PrintAxisData(m_stWLExpAxis_Z);
#endif
}



/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CWaferLoader::UpdateOutput()
{
	static int nCount = 0;

	if (!m_fHardware)
		return;

	if (m_bDisableWLWithExp)
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
		//Update sensor here4
		GetSensorValue();
		GetEncoderValue();
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

VOID CWaferLoader::UpdateProfile()
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

VOID CWaferLoader::UpdatePosition()
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
