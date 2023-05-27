/////////////////////////////////////////////////////////////////
// Safety.cpp : interface of the CSafety class
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
#include "Safety.h"
#include "Mmsystem.h"
#include "winioctl.h"
#include "iphlpapi.h"
#include "ntddndis.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSafety, CMS896AStn)

CSafety::CSafety():m_evBackupNVRam(FALSE, TRUE, "BackupNVRam")
{
	m_bStPrintLabelInBkgd	= FALSE;	//v2.64
	m_bStPrintLabelInABMode	= FALSE;	//v2.64
	m_bStStartPrintLabel	= FALSE;	//v2.64

	//m_lProgressBarStep = 0;	
	//m_lProgressBarLimit = 1;
	m_lMS60ThermalStateBH	= 0;	//	constructor

	m_bSetAlarmBlinkTimer	= FALSE;
	m_ctAlarmBlinkTimer		= CTime::GetCurrentTime();

	//For LAN Connection
	m_bCheckLANConnection	= FALSE;
	m_bIsLANEnabled			= TRUE;

	m_bInitBondHeadPreHeat = FALSE;
	m_lBHTThermalSetPoint = 8000;

	m_bEnableBinMapFcn		= FALSE;
}

CSafety::~CSafety()
{
}

BOOL CSafety::InitInstance()
{
	CMS896AStn::InitInstance();
	m_ulAutoUploadCounter	= 999;
	m_stLastAccessTime	= CTime::GetCurrentTime();
	//v2.64	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Lumileds")		// This feature only available for Lumileds DLA 
		m_bStPrintLabelInBkgd	= TRUE;	

	m_bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);

	try
	{
		GetAxisInformation();	//NuMotion

		if (m_fHardware)
		{
			SetAlarmLamp_Yellow(FALSE, TRUE);
		}
		m_bHardwareReady = TRUE;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
	return TRUE;
}

INT CSafety::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	//v4.52A11
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( !pApp->IsToSaveMap() )
	{
		//XM Sanan disables Store temp map in Stop() (by SetStoreTempOnStop(FALSE) in InitWafeMap),
		// So instead we have to store this temp map before program exit for next WarmStart
		// in program startup;
		m_WaferMapWrapper.SaveTemporary();	
	}

	return CMS896AStn::ExitInstance();
}

BOOL CSafety::InitData()
{
	m_ulAutoMaxPauseTime = 300000;
	m_lMS60ThermalStateBH	= 0;	//	init data
	LoadData();
	m_ulAutoUploadCounter	= 0;
	m_stLastAccessTime	= CTime::GetCurrentTime();
	return TRUE;
}


/////////////////////////////////////////////////////////////////
//State Operation
/////////////////////////////////////////////////////////////////
VOID CSafety::Operation()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	UINT unBreak = 300;

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
			m_ulMonitorPickCounter	= GetNewPickCount();
			//m_szCycleMessageST	= "";
			unBreak = pApp->GetProfileInt(gszPROFILE_SETTING, _T("False Auto Running Time Limit(S)"), 300);
			if( unBreak<30 )	// xu_semitek
				unBreak = 300;
			m_ulAutoMaxPauseTime = unBreak*1000*2;
			m_stPickTime			= CTime::GetCurrentTime();
			m_dPreviousUpdateTime	= GetTime();	//	prestart
			m_lMS60ThermalStateBH	= 0;	//	prestart
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

	if( m_lBuildShortPathInScanning==1 )
	{
		m_lBuildShortPathInScanning = 2;
		SaveScanTimeEvent("SFT: begin short path building");
		m_WaferMapWrapper.ResumeAlgorithmPrepNoLock(TRUE);
		SaveScanTimeEvent("SFT: called short path building");
		//	It has lock to protect the commands from HMI. 
		//	Hence, do not call this function in wafer map event.
		//	After this function is called, do not call any functions that destroy the map (such as LoadMap)
		//	and any functions that change the path. 
	}
	if( m_bPrescanWaferEnd4ShortPath )
	{
		m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);
		m_bPrescanWaferEnd4ShortPath = FALSE;
	}

	m_WaferMapWrapper.Run();
	if ( IsAOIOnlyMachine() || m_bEnableBinMapFcn)
	{
		m_BinMapWrapper.Run();		//v3.74T45		//v4.03
	}

	UpdateOutput();

	if (m_bUpdateProfile)
	{
		UpdateProfile();
	}

	if (m_bUpdatePosition)
	{
		UpdatePosition();
	}

	//v3.76
	if ((State() == IDLE_Q) || (State() == UN_INITIALIZE_Q))
	{
		LightTowerOperate();
	}

	if ( (State() == IDLE_Q) || (State() == AUTO_Q) || (State() == DEMO_Q) )
	{
		try
		{	
			CheckLANConnection();
			CheckBondHeadTThermal();
			MouseJoystickMove();
			CheckFileCopyingQueue();
		}
		catch (CAsmException e)
		{
			DisplayException(e);	
		}

		if( m_bAutoUploadOutputFile )
		{
			AutoUploadOutputFiles();
		}
	}

	//andrew
	//Air-Flow sensor test
	if (m_bEnableAirFlowTest)
	{
		if (m_bStartAirFlowMdCheck)
		{
			Sleep(1);

			//SFM_CHipecDigitalInput* piMissingDie = NULL;
			
			try
			{
				/*
				if (CMS896AApp::m_lHardwareConfigNo == MS896A_DDBS_WITH_RING_MASTER)
					piMissingDie = m_pInitOperation->GetSiBit("iMissingDie");

				else
					piMissingDie = m_pInitOperation->GetInputBit("iMissingDie");
				*/
			}
			catch (CAsmException e) 
			{
				DisplayException(e);
				//piMissingDie = NULL;
			}

			//if (!piMissingDie->IsLow())	// Is missing die = FALSE
			if ( CMS896AStn::MotionReadInputBit("iMissingDie") )
			{
				m_lAirFlowMdTime = ((LONG) timeGetTime()) - m_lAirFlowStartTime;
				m_bStartAirFlowMdCheck = FALSE;
			}
		}
		else if (m_bStartAirFlowJcCheck)
		{
			//m_lAirFlowJcTime = 33;	
			//m_bStartAirFlowJcCheck = FALSE;
			
			Sleep(1);

			//SFM_CHipecInput* piMissingDie = m_pInitOperation->GetInputBit("iMissingDie");
			//SFM_CHipecDigitalInput* piMissingDie = NULL;
			
			try
			{
				/*
				if (CMS896AApp::m_lHardwareConfigNo == MS896A_DDBS_WITH_RING_MASTER)
					piMissingDie = m_pInitOperation->GetSiBit("iMissingDie");
				
				else
					piMissingDie = m_pInitOperation->GetInputBit("iMissingDie");
				*/		
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				//piMissingDie = NULL;
			}

			//if (!piMissingDie->IsHigh())		// Is JC = FALSE
			if ( !CMS896AStn::MotionReadInputBit("iMissingDie") )
			{
				m_lAirFlowJcTime = ((LONG) timeGetTime()) - m_lAirFlowStartTime;
				m_bStartAirFlowJcCheck = FALSE;
			}
		}
	}

	Sleep(10);
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CSafety::UpdateOutput()
{
	if (!m_fHardware)
		return;


	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
		return;

	//Update sensor 
	//SFM_CHipecInPort* pInPort = NULL;
	//SFM_CHipecSiPort* pSiPort = NULL;
	USHORT usTemp = 0;
	try
	{

	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	try
	{
		//Update sensor here
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}


}

VOID CSafety::UpdateProfile()
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

VOID CSafety::UpdatePosition()
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

VOID CSafety::CheckFileCopyingQueue()
{
	//CSingleLock slLock(&m_csCopyFileQueueList);
	//slLock.Lock();
	//if( m_saCopySrcNameList.GetSize()<=0 )
	//{
	//	slLock.Lock();
	//	return ;
	//}
	//CString szSrcName = m_saCopySrcNameList.GetAt(0);
	//CString szTgtName = m_saCopyTgtNameList.GetAt(0);
	//BOOL	bDeleteSrc = m_baDeleteSrcList.GetAt(0);
	//slLock.Unlock();

	//BOOL bReturn = TRUE;
	//for(int i=0; i<3; i++)
	//{
	//	//bReturn = CopyFile(szSrcName, szTgtName, FALSE);
	//	//if( bReturn )
	//	//{
	//	//	if( bDeleteSrc )
	//	//	{
	//	//		DeleteFile(szSrcName);
	//	//	}
	//	//	break;
	//	//}
	//	//Sleep(100);
	//}

	//if( bReturn )
	//{
	//	CSingleLock slLock(&m_csCopyFileQueueList);
	//	slLock.Lock();
	//	m_saCopySrcNameList.RemoveAt(0);
	//	m_saCopyTgtNameList.RemoveAt(0);
	//	m_baDeleteSrcList.RemoveAt(0);
	//	slLock.Unlock();
	//}
}	// check and clean the copy queue.

VOID CSafety::CheckLANConnection()
{
	static int nLastLANSec = 0;


	if ( m_bCheckLANConnection == FALSE )
	{
		return;
	}

	//Only check if machine is started
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( pApp->m_bMachineStarted == FALSE )
	{
		return;
	}
	
	//get time & check for every 5 sec
	CTime theTime = CTime::GetCurrentTime();

	if ( abs(theTime.GetSecond() - nLastLANSec) < 5 )
	{
		return;
	}
	else
	{
		PIP_ADAPTER_INFO pAdapterInfo = NULL;
		PIP_ADAPTER_INFO pTempAdapterInfo = NULL;
		CStringArray astrAdapterName;
		CUIntArray aunAdapterSpeed;
		CUIntArray abAdapterConnected;

		unsigned long OutBufLen = 0;

		// Get the adapter information size
		GetAdaptersInfo(pAdapterInfo, &OutBufLen);
		pAdapterInfo = (PIP_ADAPTER_INFO) new char[OutBufLen];

		// Get all adaptor names
		if (pAdapterInfo != NULL) 
		{
			if (GetAdaptersInfo(pAdapterInfo, &OutBufLen) == ERROR_SUCCESS) 
			{
				pTempAdapterInfo = pAdapterInfo;
				while (pTempAdapterInfo != NULL) 
				{
					astrAdapterName.Add(pTempAdapterInfo->AdapterName);
					pTempAdapterInfo = pTempAdapterInfo->Next;
				}
			}
			delete [] pAdapterInfo;
		}

		// Get the status of each adaptor
		int i;
		for (i=0; i<astrAdapterName.GetSize(); i++) 
		{
			CString strAdapter;
			HANDLE hNIC;
			BOOL bConnected = FALSE;
			unsigned long ulSpeed = 0;

			strAdapter = "\\\\.\\" + astrAdapterName[i];
			if ((hNIC = CreateFile(strAdapter, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
			{
				unsigned long ulOidCode = OID_GEN_MEDIA_CONNECT_STATUS;
				unsigned long ulConnState = 0;
				unsigned long ulReqBuffer = 0;

				// Query the connection status
				if (DeviceIoControl(hNIC, IOCTL_NDIS_QUERY_GLOBAL_STATS, &ulOidCode, sizeof(unsigned long), 
					&ulConnState, sizeof(unsigned long), &ulReqBuffer, NULL) != 0)
				{
					if (ulConnState == NdisMediaStateConnected) 
						bConnected = TRUE;
				}
				// Query the speed of the adapter
				if (bConnected) 
				{
					ulOidCode = OID_GEN_LINK_SPEED;
					DeviceIoControl(hNIC, IOCTL_NDIS_QUERY_GLOBAL_STATS, &ulOidCode, sizeof(unsigned long), &ulSpeed, 
						sizeof(unsigned long), &ulReqBuffer, NULL);
				}
				CloseHandle(hNIC);
			}

			// Store the result
			if (bConnected) 
			{
				// Speed is in the unit of 100bps
				aunAdapterSpeed.Add(ulSpeed*100); 
				abAdapterConnected.Add(TRUE);
			}
			else 
			{
				aunAdapterSpeed.Add(0);
				abAdapterConnected.Add(FALSE);
			}
		}

		//Check status
		if ( abAdapterConnected.GetSize() > 0 )
		{
			if ( abAdapterConnected.GetAt(0) == FALSE )
			{
				if ( m_bIsLANEnabled == TRUE )
				{
					SetAlert_Red_Yellow(IDS_ST_LAN_DISCONNECTED);

					SetStatusMessage("LAN connection is disabled!");
				}
				m_bIsLANEnabled = FALSE;
			}
			else
			{
				if ( m_bIsLANEnabled == FALSE )
				{
					SetStatusMessage("LAN connection is restored!");
				}
				m_bIsLANEnabled = TRUE;
			}
		}
	}

	nLastLANSec = theTime.GetSecond();
}

BOOL CSafety::MouseJoystickMove()
{
	if ( m_fHardware == FALSE )
	{
		return FALSE;
	}

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		if (m_nMouseMode == MOUSE_CLICK)
		{
			MouseClickMove();

			m_nMouseMode = MOUSE_STOP;
		}
		else if (m_nMouseMode == MOUSE_DRAG)
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_NONE;

			if (m_nMouseDragState == PR_MOUSE_EVENT_STATE_DRAG)
			{
				MouseDragMove();
			}
			else
			{
				m_nMouseMode = MOUSE_STOP;
			}	
		}
	}

	return TRUE;
}

BOOL CSafety::MouseClickMove()
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		PR_COORD stOffset;
		int		 siStepX;
		int		 siStepY;
		LONG	 lEnc_X, lEnc_Y;
		stOffset.x = (PR_WORD)m_dMouseClickX;
		stOffset.y = (PR_WORD)m_dMouseClickY;

		if ( CalculateMouseClickPos(stOffset, &siStepX, &siStepY) == FALSE)
		{
			return FALSE;
		}

		if (GetTableEncoder(&lEnc_X, &lEnc_Y) == FALSE)
		{
			SetErrorMessage("Mouse Click - Get Encoder Error");
			return FALSE;
		}
		TableMoveTo(lEnc_X+siStepX, lEnc_Y+siStepY);
	}

	return TRUE;
}

BOOL CSafety::MouseDragMove()
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		INT nDirectionX = 0, nDirectionY = 0;
		INT nMagnitude = 0;
		INT nTargetX = 0, nTargetY = 0;

		//Direction
		if ( (m_dMouseDragAngle > -22.5) && (m_dMouseDragAngle <= 22.5) )			//RIGHT
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_RIGHT;
			nDirectionX = 1;
			nDirectionY = 0;
		}
		else if ( (m_dMouseDragAngle > 22.5) && (m_dMouseDragAngle <= 67.5) )		//RIGHT + DOWN
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_DOWN_RIGHT;
			nDirectionX = 1;
			nDirectionY = 1;
		}
		else if ( (m_dMouseDragAngle > 67.5) && (m_dMouseDragAngle <= 112.5) )		//DOWN
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_DOWN;
			nDirectionX = 0;
			nDirectionY = 1;
		}
		else if ( (m_dMouseDragAngle > 112.5) && (m_dMouseDragAngle <= 157.5) )		//DOWN + LEFT
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_DOWN_LEFT;
			nDirectionX = -1;
			nDirectionY = 1;
		}
		else if ( (m_dMouseDragAngle > 157.5) || (m_dMouseDragAngle <= -157.5) )	//LEFT
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_LEFT;
			nDirectionX = -1;
			nDirectionY = 0;
		}
		else if ( (m_dMouseDragAngle > -157.5) && (m_dMouseDragAngle <= -112.5) )	//LEFT + UP
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_UP_LEFT;
			nDirectionX = -1;
			nDirectionY = -1;
		}
		else if ( (m_dMouseDragAngle > -112.5) && (m_dMouseDragAngle <= -67.5) )	//UP
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_UP;
			nDirectionX = 0;
			nDirectionY = -1;
		}
		else if ( (m_dMouseDragAngle > -67.5) && (m_dMouseDragAngle <= -22.5) )		//UP + RIGHT
		{
			m_lMouseDragDirection = MOUSE_JOY_DIRECTION_UP_RIGHT;
			nDirectionX = 1;
			nDirectionY = -1;
		}

		//Move only 4 directions if Speed=Slow and Drag Distance<=1000
		if ( (m_lMouseJoystickSpeed == MOUSE_JOY_SPEED_SLOW) && (CMS896AStn::m_dMouseDragDist <= 1000) )
		{
			if ( (m_dMouseDragAngle > -45) && (m_dMouseDragAngle <= 45) )			//RIGHT
			{
				nDirectionX = 1;
				nDirectionY = 0;
			}
			else if ( (m_dMouseDragAngle > 45) && (m_dMouseDragAngle <= 135) )		//DOWN
			{
				nDirectionX = 0;
				nDirectionY = 1;
			}
			else if ( (m_dMouseDragAngle > 135) || (m_dMouseDragAngle <= -135) )	//LEFT
			{
				nDirectionX = -1;
				nDirectionY = 0;
			}
			else if ( (m_dMouseDragAngle > -135) && (m_dMouseDragAngle <= -45) )	//UP
			{
				nDirectionX = 0;
				nDirectionY = -1;
			}
		}

		//v4.16T6		//Huga special request fcn
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		BOOL bManualAdjustJSSpeed = pApp->GetFeatureStatus(MS896A_FUNC_MANUAL_PR_JS_SPEED);
		if (bManualAdjustJSSpeed)
		{
			switch (m_lMouseJoystickSpeed)
			{
			case MOUSE_JOY_SPEED_SLOW:
				if ( (m_lJsTableMode == 1) )		//BT	//v4.37T11
					nMagnitude = 10;
				else
					nMagnitude = 20;
				break;
			case MOUSE_JOY_SPEED_FAST:
				nMagnitude = 1200;
				break;
			case MOUSE_JOY_SPEED_MEDIUM:
			default:
				nMagnitude = 400;
				break;
			}
		}
		else
		{
			//v3.93
			//Use dragging distance to determine JS speed
			if (CMS896AStn::m_dMouseDragDist <= 150)
			{
				m_lMouseJoystickSpeed = MOUSE_JOY_SPEED_SLOW;
				nDirectionX = 0;
				nDirectionY = 0;
			}
			else if (CMS896AStn::m_dMouseDragDist <= 1200)
			{
				m_lMouseJoystickSpeed = MOUSE_JOY_SPEED_SLOW;
			}
			else if (CMS896AStn::m_dMouseDragDist <= 5000)
			{
				m_lMouseJoystickSpeed = MOUSE_JOY_SPEED_MEDIUM;
			}
			else
			{
				m_lMouseJoystickSpeed = MOUSE_JOY_SPEED_FAST;
			}

			switch (m_lMouseJoystickSpeed)
			{
			case MOUSE_JOY_SPEED_SLOW:
				if ( (m_lJsTableMode == 1) )		//BT		//v3.79		//v4.37T11
					nMagnitude = 10;
				else
					nMagnitude = 20;
				break;

			case MOUSE_JOY_SPEED_MEDIUM:
				nMagnitude = 400;
				break;

			case MOUSE_JOY_SPEED_FAST:
				nMagnitude = 1200;
				break;

			default:
				nMagnitude = 400;
				break;
			}
		}

		nTargetX = nMagnitude * nDirectionX;
		nTargetY = nMagnitude * nDirectionY;

		TableMove(nTargetX, nTargetY);
	}

	return TRUE;
}

VOID CSafety::GetAxisInformation()
{
	InitAxisData(m_stWTAxis_X);
	m_stWTAxis_X.m_szName				= JS_WT_AXIS_X;
	m_stWTAxis_X.m_szTag				= MS896A_CFG_CH_WAFTABLE_X;
	m_stWTAxis_X.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stWTAxis_X);
	m_stWTAxis_X.m_dEncResolution		= 1;

	InitAxisData(m_stWTAxis_Y);
	m_stWTAxis_Y.m_szName				= JS_WT_AXIS_Y;
	m_stWTAxis_Y.m_szTag				= MS896A_CFG_CH_WAFTABLE_Y;
	m_stWTAxis_Y.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stWTAxis_Y);
	m_stWTAxis_Y.m_dEncResolution		= 1;

	{
		InitAxisData(m_stBTAxis_X);
		m_stBTAxis_X.m_szName				= "BinTableXAxis";
		m_stBTAxis_X.m_szTag				= MS896A_CFG_CH_BINTABLE_X;
		m_stBTAxis_X.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBTAxis_X);
		m_stBTAxis_X.m_dEncResolution		= 1;

		InitAxisData(m_stBTAxis_Y);
		m_stBTAxis_Y.m_szName				= "BinTableYAxis";
		m_stBTAxis_Y.m_szTag				= MS896A_CFG_CH_BINTABLE_Y;
		m_stBTAxis_Y.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBTAxis_Y);
		m_stBTAxis_Y.m_dEncResolution		= 1;
	}

	//v4.17T1	//MS100 9Inch dual-table config
	if (CMS896AApp::m_bMS100Plus9InchOption)
	{
		InitAxisData(m_stBTAxis_X2);
		m_stBTAxis_X2.m_szName				= "BinTableX2Axis";
		m_stBTAxis_X2.m_szTag				= MS896A_CFG_CH_BINTABLE2_X;
		m_stBTAxis_X2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBTAxis_X2);
		m_stBTAxis_X2.m_dEncResolution		= 1;

		InitAxisData(m_stBTAxis_Y2);
		m_stBTAxis_Y2.m_szName				= "BinTableY2Axis";
		m_stBTAxis_Y2.m_szTag				= MS896A_CFG_CH_BINTABLE2_Y;
		m_stBTAxis_Y2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBTAxis_Y2);
		m_stBTAxis_Y2.m_dEncResolution		= 1;
	}
}


BOOL CSafety::MonitorFalseAutoRunning()
{
	BOOL bReturn = TRUE;
	if( IsPrescanEnable() && GetNewPickCount()>0 )
	{
		Sleep(100);
		if( m_ulMonitorPickCounter != GetNewPickCount() )
		{
			m_ulMonitorPickCounter = GetNewPickCount();
			m_dPreviousUpdateTime = GetTime();	//	bonding
			m_stPickTime			= CTime::GetCurrentTime();
		}
		else if( GetAlarmLamp_Status()==ALARM_GREEN )
		{
			DisplaySequence("STS - check time lasting");
			ULONG ulIdleGreenTime = (ULONG)(GetTime() - m_dPreviousUpdateTime);
			if( ulIdleGreenTime>m_ulAutoMaxPauseTime )
			{
				CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(m_stPickTime, "GreenIdle");

				CString szMsg;
				szMsg.Format(" %d seconds.", m_ulAutoMaxPauseTime/1000);
				DisplaySequence("STS - Machine idle time in auto sort mode over" + szMsg);

				//SetAlertTime(TRUE, EQUIP_ALARM_TIME, szMsg, "Safety Auto Idle");
				HmiMessage_Red_Back("Please press Stop button.", "Auto Looping");
				//SetAlertTime(FALSE, EQUIP_ALARM_TIME, szMsg, "Safety Auto Idle");
				bReturn = FALSE;

				Sleep(100);

				m_dPreviousUpdateTime = GetTime();	//	alarm because time out, stop
			//	m_stPickTime			= CTime::GetCurrentTime();
			}
		}
		else if( GetAlarmLamp_Status()>=ALARM_RED )
		{
			m_dPreviousUpdateTime = GetTime();	//	bonding and a red alarm is on.
		}
	}

	return bReturn;
}

//4.55T17
BOOL CSafety::ST_EnableThermalControl(BOOL bEnable)
{
	return BH_EnableThermalControl(bEnable);
}


BOOL CSafety::ST_UpdateThermalTemperature()
{
	return OpUpdateBHTThermalRMSValues(30, FALSE);
}
//4.55T17
BOOL CSafety::MonitorThermalControlAutoRunning()
{
	BOOL bReturn = TRUE;
	CString szMsg;

	if( GetNewPickCount()>0 )
	{
		//Sleep(100);
		if( GetAlarmLamp_Status()==ALARM_GREEN)
		{
			if( m_lMS60ThermalStateBH == 1 )	//	thermal on already, but now alarm in green
			{
				DisplaySequence("STS - Disable Thermal Control");
				ST_EnableThermalControl(FALSE); //alarm done

				m_lMS60ThermalStateBH = 0;	//	reset after alarm in green
			}
		}
		else if( GetAlarmLamp_Status()>=ALARM_RED )
		{
			DisplaySequence("STS - Thermal Control check time lasting");
			CTimeSpan stSpan = CTime::GetCurrentTime() - m_stPickTime;
			ULONG ulIdleTime = (ULONG)stSpan.GetTotalSeconds();
			if( ulIdleTime>1000 )
			{
				if( m_lMS60ThermalStateBH == 0)	//	not green
				{
					szMsg.Format(" %d seconds.", ulIdleTime/1000);
					DisplaySequence("STS - Open Thermal Control Machine idle time in auto sort mode over" + szMsg);
				
					ST_EnableThermalControl(TRUE); //alarm start

					m_lMS60ThermalStateBH = 1;	//	alarm in red, turn on thermal.

					bReturn = FALSE;
				}
			}

			if( m_lMS60ThermalStateBH == 1 )	//	already turn on red, trigger BH to update thermal temperature.
			{
				ST_UpdateThermalTemperature();
			}	//	update thermal temperature
		}
	}

	return bReturn;
}

BOOL CSafety::AutoUploadOutputFiles()
{
	if( m_bAutoUploadOutputFile )
	{
		CString szSrcPath = gszOUTPUT_FILE_TEMP_PATH;
		CString szTgtPath = (*m_psmfSRam)["MS896A"]["OutputFilePath"];
		if( m_ulAutoUploadCounter==999 )
		{
			return TRUE;
		}
		if( _access(szTgtPath, 0) == -1 )
		{
			CTimeSpan stSpan = CTime::GetCurrentTime() - m_stLastAccessTime;
			if( stSpan.GetTotalMinutes()>30 )
			{
				CString szMsg = "Fail to access OutputFile Path at: " + szTgtPath;
				SetErrorMessage(szMsg + "\n");
				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg + "\n");
				SetAlert_Msg(IDS_BT_NO_DIE_TO_GEN_OTF, szMsg);
				m_stLastAccessTime	= CTime::GetCurrentTime();
			}
			return FALSE;
		}
		m_stLastAccessTime	= CTime::GetCurrentTime();
		m_ulAutoUploadCounter = 0;
		if( szSrcPath.CompareNoCase(szTgtPath)!=0 )
		{
			return LoopUploadOutputFiles(szSrcPath, szTgtPath);
		}
	}

	return TRUE;
}

BOOL CSafety::LoopUploadOutputFiles(CString szSrcPath, CString szTgtPath)
{
	WIN32_FIND_DATA FileData;
	HANDLE hSearch = FindFirstFile(szSrcPath + "\\" + "*.*" , &FileData);
	
	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	CString szSrcSubPath, szTgtSubPath;

	do 
	{
		// if it is a folder recurive call to remove file
		if ((FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) )	//v4.46T13
		{
			CString szSubDir = FileData.cFileName;
			if( (szSubDir != ".") && (szSubDir != "..") )
			{
				szSrcSubPath = szSrcPath + "\\" + szSubDir;
				szTgtSubPath = szTgtPath + "\\" + szSubDir;
				CreateDirectory(szTgtSubPath, NULL);
				LoopUploadOutputFiles(szSrcSubPath, szTgtSubPath);
			}
		}
		// if it is a file delete the file
		else
		{
			CTime curTime = CTime::GetCurrentTime();
			CTime ctFileTime(FileData.ftCreationTime);
			CTimeSpan timeDiff = curTime - ctFileTime;
		
			// keep file for a month(certain time)
			if ((timeDiff.GetTotalSeconds() > 10) )
			{
				CString szOnlyName = FileData.cFileName;
				szSrcSubPath = szSrcPath + "\\" + szOnlyName;
				szTgtSubPath = szTgtPath + "\\" + szOnlyName;
				if( CopyFile(szSrcSubPath, szTgtSubPath, FALSE) )
				{
					if (_access(szTgtSubPath, 0) != -1)
					{
						DisplaySequence("STS - moved " + szSrcSubPath + " to " + szTgtSubPath);
						DeleteFile(szSrcSubPath);
						m_ulAutoUploadCounter++;
					}
				}
			}
		}

		if( m_ulAutoUploadCounter>=3 )
		{
			break;
		}
	}while (FindNextFile(hSearch, &FileData) == TRUE);

	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE) 
	{ 
		return FALSE;
	} 

	return TRUE;
}
