/////////////////////////////////////////////////////////////////
// WaferPr.cpp : interface of the CWaferPr class
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
#include "WaferPr.h"
#include "GallerySearchController.h"
#include "PRFailureCaseLog.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include "BondPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CWaferPr, CMS896AStn)

CWaferPr::CWaferPr(): m_evWTStable(FALSE, TRUE, "WtStableEvt"),
					  m_evPRLatched(FALSE, TRUE, "WprLatchedEvt"),
					  m_evPRStart(FALSE, TRUE, "WprStartEvt"),
					  m_evDieReady(FALSE, TRUE, "WprDieReadyEvt"),
  					  m_evBadDie(FALSE, TRUE, "WprBadDieEvt"),
  					  m_evBadDieForT(FALSE, TRUE, "WprBadDieForTEvt"),
  					  m_evLFReady(FALSE, TRUE, "WprLFReadyEvt"),
					  m_evConfirmSrch(FALSE, TRUE, "WtConfirmSrchEvt"),
					  m_evBhToPrePick(FALSE, TRUE, "BhToPrePickEvt"),
					  m_evPreCompensate(FALSE, TRUE, "PreCompensateEvt"),
					  m_evBhTReadyForWPR(FALSE, TRUE, "BhTReadyForWPREvt"),		//v3.34
					  m_evWTReadyForWPREmptyCheck(FALSE, TRUE, "WTReadyForWPREmptyCheckEvt"),		//v4.54A5
					  m_evWPREmptyCheckDone(FALSE, TRUE, "WPREmptyCheckDoneEvt"),		//v4.54A5
					  m_evInitAFZ(FALSE, TRUE, "InitAFZ"),
// prescan relative code
					  m_evAllPrescanDone(FALSE, TRUE, "AllPrescanDoneEvt"),
					  m_evSetPRTesting(FALSE, TRUE, "SetPRTesting"),
					  m_evSearchMarkReady(FALSE, TRUE, "SearchMarkReady"),
					  m_evSearchMarkDone(FALSE, TRUE,	"SearchMarkDone"),
					  m_evBhReadyForWPRHwTrigger(FALSE, TRUE, "BhReadyForWPRHwTrigger"),
					  m_evWPRGrabImageReady(FALSE, TRUE, "WPRGrabImageReadyEvt")

{
	InitVariable();
}

CWaferPr::~CWaferPr()
{
}


BOOL CWaferPr::InitInstance()
{
	CMS896AStn::InitInstance();

	//Init Zoom and Focus motors
	InitCameraMotors();

	//v4.04	//MS100 AOI ZOOM Z 
	
	//v4.28T5
	m_dWTXinUm	= GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X);
	m_dWTYinUm	= GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y);

	m_bWprWithAF_Z = (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFERPR_Z, MS896A_CFG_CH_ENABLE);
	if (IsWprWithAF())
	{
		m_bSelBL_Z = TRUE;
		GetAxisInformation();
		Z_Home();
		m_lFocusZState = 0;

		if ( IsBLInUse() )
		{
			BLZ_Home();
		}

		SetInitAFZState(TRUE);
	}
	else
	{
		SetInitAFZState(TRUE);	//v4.08
	}

	m_qSubOperation = DPR_INIT_Q;
	m_bHardwareReady = TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	m_bDualPath = pApp->GetFeatureStatus(MS60_FUNC_VISION_DUAL_PATH);

	m_pPrZoomSensorMode = new CPrZoomSensorMode(GetRunCamID(), GetRunSenID(), GetRunRecID(), m_pPrGeneral);
	CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
	CString szFailureCaselogPath;
	szFailureCaselogPath = _T("C:\\MapSorter\\UserData\\PRFailureCaseLog");
	if (_access(szFailureCaselogPath, F_OK) != F_OK)
	{
		_mkdir(szFailureCaselogPath);
	}

	LONG plImageNo = 0;
	pPRFailureCaseLog->SetData(&plImageNo);
	pPRFailureCaseLog->SetData(FALSE, szFailureCaselogPath);
	return TRUE;
}

INT CWaferPr::ExitInstance()
{
// Temporary comment: Save data will make search die fail
	PR_TerminateSI();
	close_si_drvr();

	if( m_fHardware && IsWprWithAF() )
	{
		try
		{
			CMS896AStn::MotionSync(WAFERPR_AXIS_Z, 10000, &m_stZoomAxis_Z);
			CMS896AStn::MotionPowerOff(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
			CMS896AStn::MotionClearError(WAFERPR_AXIS_Z, HP_MOTION_ABORT, &m_stZoomAxis_Z);
		}
		catch(CAsmException e){
		}

		if ( IsBLInUse() )
		{
			try
			{
				BLZ_MoveTo(0);
				CMS896AStn::MotionSync(WAFERPR_BACKLIGHT_Z, 10000, &m_stBackLight_Z);
				CMS896AStn::MotionPowerOff(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
				CMS896AStn::MotionClearError(WAFERPR_BACKLIGHT_Z, HP_MOTION_ABORT, &m_stBackLight_Z);
			}
			catch(CAsmException e) {
			}
		}
	}

	return CMS896AStn::ExitInstance();
}

BOOL CWaferPr::InitData()
{
	if( IsOcrAOIMode() )
	{
		m_lAoiOcrBoxHeight	= 90;
		m_lAoiOcrBoxWidth	= 610;
	}
	if (IsLoadingPortablePKGFile() == TRUE && IsLoadingPKGFile() == TRUE)
	{
		LoadWaferPrOption();
		SavePrData(FALSE);

		if (CMS896AApp::m_bErasePortablePackageFileImage == TRUE)
		{
			m_szPKGNormalDieRecordPath = "";
			m_szPKGRefDieRecordPath = "";

			// just load the image and force to learn pr record again
			// Update Normal Die Record
			UpdatePKGFileRecordImage(m_ssGenPRSrchID[0], TRUE);
			// Update Reference Die Record
			UpdatePKGFileRecordImage(m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET + 1], FALSE);
		
			SavePrData(FALSE);
			
			// since all len zoom and foucs are different in different machine, so clear the pr record
			// HUGA request to just load the image but not the record
			FreeAllPrRecords();
			
			// save data again to update display
			SavePrData(FALSE);
		}
	}
	else if (CMS896AStn::m_bNoPRRecordForPKGFile == TRUE && IsLoadingPKGFile() == TRUE)
	{
		FreeAllPrRecords();
			
		// save data again to update display
		SavePrData(FALSE);
	}
	else
	{
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		BOOL bIsManualLoadPkgFile = pApp->IsManualLoadPkgFile();

		LoadWaferPrOption();
		LoadPrData();

		if (bIsManualLoadPkgFile)	//v4.53A22
		{
			//Save new PR record IDs (by auto-assign method)from PR_DownloadRecordProcessCmd 
			//	into MSD file after Manual-Load PKG;
			//SavePrData(FALSE);
			SaveRecordID();
		}
	}

	//Resume the zoom & focus level
	ResumeCameraStatus();
	RemoveFilesInFolder(WPR_SHOW_IMAGE_PATH);
	CMSLogFileUtility::Instance()->WPR_BackupAlignWaferLog();
	CMSLogFileUtility::Instance()->WPR_BackupLearnPitchLog();
	CMSLogFileUtility::Instance()->WPR_BackupWaferDieOffsetLog();

	//	zoomview change zoom sensor when warm start
//#ifndef MS_DEBUG
	LONG lInitFailCounter = 0;
	while( 1 )
	{
		if( m_bPRInit )
		{
			break;
		}
		lInitFailCounter++;
		Sleep(100);
		if ( lInitFailCounter > 10000 )
		{
			return FALSE;
		}
	}
//#endif
	InitZoomSensor();
	WPR_ToggleZoom(TRUE, FALSE, 1);
	GetScanFovWindow();	//	Init data
	WPR_ToggleZoom(FALSE, FALSE, 2);
	WPR_DigitalZoom((short)m_lNmlDigitalZoom);	//	start up, changed to the set zoom view factor
	SetZoomViewFixZoom(TRUE);

	if( IsAOIOnlyMachine() )	// warm start
	{
		LoadRuntimeTwoRecords();

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		CString szCurrPkgName	= pApp->GetPKGFilename();
		m_szPkgNameCurrent	= szCurrPkgName;
		for (int i = 0; i < WPR_MAX_DIE ; i++)
		{
			m_saPkgRecordCurrent[i] = m_ssGenPRSrchID[i];
		}

		CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";
		CString szSrcFile = szExePath + "WaferPr.msd";
		CString szTgtFile = szExePath + "WaferPr_" + szCurrPkgName + ".msd";
		CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
		szSrcFile = szExePath + "WaferTable.msd";
		szTgtFile = szExePath + "WaferTable_" + szCurrPkgName + ".msd";
		CopyFileWithRetry(szSrcFile, szTgtFile, FALSE);
		CString szLog;
		szLog.Format("Warm Start PKG file, curr %s, prev %s", m_szPkgNameCurrent, m_szPkgNamePrevious);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

		SaveRuntimeTwoRecords();	//	Save data
	}

	return TRUE;
}

VOID CWaferPr::ClearData()
{
	LoadPrData();
	FreeAllPrRecords();			// Free all PR records cold start
}

VOID CWaferPr::GetAxisInformation()
{
	InitAxisData(m_stZoomAxis_Z);
	m_stZoomAxis_Z.m_szName				= WAFERPR_AXIS_Z;	//"WaferPrZAxis";
	m_stZoomAxis_Z.m_szTag				= MS896A_CFG_CH_WAFERPR_Z;
	m_stZoomAxis_Z.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stZoomAxis_Z);

	if ( IsBLInUse() )
	{
		InitAxisData(m_stBackLight_Z);
		m_stBackLight_Z.m_szName		= WAFERPR_BACKLIGHT_Z;
		m_stBackLight_Z.m_szTag			= MS896A_CFG_CH_BACKLIGHT_Z;
		m_stBackLight_Z.m_ucControlID	= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(m_stBackLight_Z);

		PrintAxisData(m_stBackLight_Z);

		CMS896AStn::MotionSelectProfile(WAFERPR_BACKLIGHT_Z, WPR_MP_BACKLIGHT_Z_NORMAL, &m_stBackLight_Z);	//v4.39T8
	}

	//For debug only
	PrintAxisData(m_stZoomAxis_Z);

	//Select motion profile			
	CMS896AStn::MotionSelectProfile("WaferPrZAxis", "mpfWaferPrZNormal", &m_stZoomAxis_Z);
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CWaferPr::UpdateOutput()
{
	if (!m_fHardware)
		return;

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
		return;

	static int nCount = 0;
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
		if (m_szStationName == "WaferPrStn")
		{
			//Update the Zoom & Focus Sensor Status if Motorized Zoom is used
/*
			if (m_bUseMotorizedZoom)
			{
				m_bZoomPower = CMS896AStn::MotionIsPowerOn("WaferPrZoomZAxis");
				m_bZoomHomeSensor = CMS896AStn::MotionIsHomeSensorHigh("WaferPrZoomZAxis");
				m_bZoomLimitSensor = CMS896AStn::MotionIsPositiveLimitHigh("WaferPrZoomZAxis");

				m_bFocusPower = CMS896AStn::MotionIsPowerOn("WaferPrFocusZAxis");
				m_bFocusHomeSensor = CMS896AStn::MotionIsHomeSensorHigh("WaferPrFocusZAxis");
				m_bFocusLimitSensor = CMS896AStn::MotionIsPositiveLimitHigh("WaferPrFocusZAxis");
			}
*/
			if( IsWprWithAF() )
			{
				m_bAutoFocusPower		= CMS896AStn::MotionIsPowerOn(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
				m_bAutoFocusHomeSensor	= CMS896AStn::MotionIsHomeSensorHigh(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
				m_bAutoFocusLimitSensor	= CMS896AStn::MotionIsPositiveLimitHigh(WAFERPR_AXIS_Z, &m_stZoomAxis_Z);
				WPR_GetEncoderValue();
			}

			if ( IsBLInUse() )
			{
				m_bBLZPower = CMS896AStn::MotionIsPowerOn(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
				m_bBLZHome	= CMS896AStn::MotionIsHomeSensorHigh(WAFERPR_BACKLIGHT_Z, &m_stBackLight_Z);
			}
		}
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}


VOID CWaferPr::UpdateProfile()
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

VOID CWaferPr::UpdatePosition()
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
//Init Zoom and Focus motors
BOOL CWaferPr::InitCameraMotors()
{
	BOOL bReturn = TRUE;

	try
	{
		m_bUseMotorizedZoom = ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_PRS_MOTORIZEDZOOM);
/*		
		if (m_fHardware && m_bUseMotorizedZoom)
		{
			SetZoomZPower(TRUE);
			
			//Move outside the Home sensor and search home
			if ( CMS896AStn::MotionIsHomeSensorHigh("WaferPrZoomZAxis") )
				Z_Zoom_Move(10000,SFM_WAIT,FALSE);
			
			Z_HomeZoom();
			SetZoomZPower(FALSE);

			
			SetFocusZPower(TRUE);

			//Move outside the Home sensor and search home
			if ( CMS896AStn::MotionIsHomeSensorHigh("WaferPrFocusZAxis") )
				Z_Focus_Move(-2000,SFM_WAIT,FALSE);
			
			Z_HomeFocus();
			SetFocusZPower(FALSE);


		}
*/
	}
	catch(CAsmException e)
	{
		bReturn = FALSE;
		DisplayException(e);
	}

	return bReturn;
}


//Resume Focus & Zoom Level
BOOL CWaferPr::ResumeCameraStatus()
{
	BOOL bReturn = TRUE;

	if (m_fHardware && m_bUseMotorizedZoom)
	{
		try
		{
			SetZoomZPower(TRUE);
			Z_Zoom_Move(m_lEnc_Zoom);
			SetZoomZPower(FALSE);

			
			SetFocusZPower(TRUE);
			Z_Focus_Move(m_lEnc_Focus);
			SetFocusZPower(FALSE);
		}
		catch(CAsmException e)
		{
			bReturn = FALSE;
			DisplayException(e);
		}
	}

	return bReturn;
}



