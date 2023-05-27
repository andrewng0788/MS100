/////////////////////////////////////////////////////////////////
// NVCLoader.cpp : interface of the CNVCLoader class
//
//	Description:
//		
//	Date:		23 June 2020
//	Revision:	1.00
//
//	By:	Andrew Ng		
//				
//	Copyright @ ASM Pacific Technology Ltd., .
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "NVCLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CNVCLoader, CMS896AStn)

CNVCLoader::CNVCLoader():	m_evNLOperate(FALSE,	TRUE,		"NlOperateEvt"),
							m_evNLReady(FALSE,		TRUE,		"NlReadyEvt"),
							m_evNLPreOperate(FALSE, TRUE,		"NlPreOperateEvt")

{
	InitVariable();
}

CNVCLoader::~CNVCLoader()
{
}

BOOL CNVCLoader::InitInstance()
{
	CMS896AStn::InitInstance();

	m_bXYUseEncoder = (BOOL) GetChannelInformation(MS896A_CFG_CH_NVCLOADER_X, MS896A_CFG_CH_ENABLE_ENCODER);

	try
	{
		GetAxisInformation();	

		if (m_fHardware && !m_bDisableNL)
		{
			DisplayMessage("NL: InitInstance with m_bDisableNL = FALSE");

			SetWafExpanderOpen(FALSE);
			SetBinExpanderOpen(FALSE);
			SetWafGripper(TRUE);
			SetBinGripper(TRUE);

			HomeNLModule();
			InitMotorSwLimits();

			CMS896AStn::MotionSetLastWarning(NL_AXIS_X,		HP_SUCCESS,		&m_stNLAxis_X);
			CMS896AStn::MotionSetLastWarning(NL_AXIS_Y,		HP_SUCCESS,		&m_stNLAxis_Y);
			CMS896AStn::MotionSetLastWarning(NL_AXIS_Z1,	HP_SUCCESS,		&m_stNLAxis_Z1);
			CMS896AStn::MotionSetLastWarning(NL_AXIS_Z2,	HP_SUCCESS,		&m_stNLAxis_Z2);
		}
		else
		{
			DisplayMessage("NL: InitInstance with m_bDisableNL = TRUE");

			m_bSel_X = FALSE;
			m_bSel_Y = FALSE;
			m_bSel_Z1 = FALSE;
			m_bSel_Z2 = FALSE;
		}

		CMS896AStn::m_bHardwareReady = TRUE;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

INT	CNVCLoader::ExitInstance()
{
	if (m_fHardware && !m_bDisableNL)
	{
		if (m_bSel_X)
		{
			try 
			{
				CMS896AStn::MotionSync(NL_AXIS_X, 10000, &m_stNLAxis_X);
				CMS896AStn::MotionPowerOff(NL_AXIS_X, &m_stNLAxis_X);
				CMS896AStn::MotionClearError(NL_AXIS_X, HP_MOTION_ABORT, &m_stNLAxis_X);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}

		if (m_bSel_Y)
		{
			try 
			{
				CMS896AStn::MotionSync(NL_AXIS_Y, 10000, &m_stNLAxis_Y);
				CMS896AStn::MotionPowerOff(NL_AXIS_Y, &m_stNLAxis_Y);
				CMS896AStn::MotionClearError(NL_AXIS_Y, HP_MOTION_ABORT, &m_stNLAxis_Y);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}

		if (m_bSel_Z1)
		{
			try 
			{
				CMS896AStn::MotionSync(NL_AXIS_Z1, 10000, &m_stNLAxis_Z1);
				CMS896AStn::MotionPowerOff(NL_AXIS_Z1, &m_stNLAxis_Z1);
				CMS896AStn::MotionClearError(NL_AXIS_Z1, HP_MOTION_ABORT, &m_stNLAxis_Z1);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}

		if (m_bSel_Z2)
		{
			try 
			{
				CMS896AStn::MotionSync(NL_AXIS_Z2, 10000, &m_stNLAxis_Z2);
				CMS896AStn::MotionPowerOff(NL_AXIS_Z2, &m_stNLAxis_Z2);
				CMS896AStn::MotionClearError(NL_AXIS_Z2, HP_MOTION_ABORT, &m_stNLAxis_Z2);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}
	}

	return CMS896AStn::ExitInstance();
}

BOOL CNVCLoader::InitData()		//WARM-START in C896aApp::MachineWarmStart
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (IsLoadingPKGFile() == TRUE && IsLoadingPortablePKGFile() == TRUE)
	{
		/*
		LoadBinLoaderOption();
		SaveData();
		ResetAllMagazine();
		*/
	}
    else if (IsLoadingPKGFile() == TRUE)		//v4.57A12	//Added else
	{
		LoadData();
		/*
		LoadData();
		LoadBinLoaderOption();
		SaveData();

		LoadMgznOMData();				//v4.57A12	//Finisar, if BLMgzSetup.msd is added into \Exe\DF_FileList.msd
		ResetAllMagazine();
		*/
	}
	else
	{
		LoadMgznOMData();
		LoadMgznRTData();

		LoadData();

		//ResetAllMagazine();
	}

	if (m_fHardware == TRUE && !m_bDisableNL)
	{
		DisplayMessage("NL: InitData with m_bDisableNL = FALSE");
	}
	else
	{
		DisplayMessage("NL: InitData with m_bDisableNL = TRUE");
	}

	return TRUE;
}

VOID CNVCLoader::ClearData()
{
}

VOID CNVCLoader::FlushMessage()
{
}

VOID CNVCLoader::UpdateStationData()
{
}

VOID CNVCLoader::UpdateOutput()
{
	if (!m_fHardware || m_bDisableNL)
	{
		return;
	}
	if (State() != IDLE_Q)
	{
		return;
	}

	try
	{
		GetSensorValue();
		Sleep(100);
		
		GetEncoderValue();
		Sleep(100);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}

VOID CNVCLoader::UpdateProfile()
{
}

VOID CNVCLoader::UpdatePosition()
{
}

VOID CNVCLoader::GetSensorValue()
{
	if (m_fHardware && !m_bDisableNL)
	{
		try
		{
			IsGripper1FrameExist();
			Sleep(10);
			IsGripper2FrameExist();
			Sleep(10);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
	}

}

VOID CNVCLoader::GetEncoderValue()
{
	CString szAxis;

	if (m_fHardware && !m_bDisableNL)
	{
		try
		{
			szAxis = NL_AXIS_X;
			if (m_bXYUseEncoder)
			{
				m_lEnc_X = CMS896AStn::MotionGetEncoderPosition(NL_AXIS_X, 3.2, &m_stNLAxis_X);
			}
			else
			{
				m_lEnc_X = CMS896AStn::MotionGetCommandPosition(NL_AXIS_X, &m_stNLAxis_X);
			}

			szAxis = NL_AXIS_Y;
			if (m_bXYUseEncoder)
			{
				m_lEnc_Y = CMS896AStn::MotionGetEncoderPosition(NL_AXIS_Y, 3.2, &m_stNLAxis_Y);
			}
			else
			{
				m_lEnc_Y = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Y, &m_stNLAxis_Y);
			}

			szAxis = NL_AXIS_Z1;
			m_lEnc_Z1 = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z1, &m_stNLAxis_Z1);

			szAxis = NL_AXIS_Z2;
			m_lEnc_Z2 = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z2, &m_stNLAxis_Z2);

		}
		catch (CAsmException e)
		{
			DisplayException(e);

			if (szAxis == NL_AXIS_X)
				CMS896AStn::MotionCheckResult(szAxis, &m_stNLAxis_X);
			else if (szAxis == NL_AXIS_Y)
				CMS896AStn::MotionCheckResult(szAxis, &m_stNLAxis_Y);
			else if (szAxis == NL_AXIS_Z1)
				CMS896AStn::MotionCheckResult(szAxis, &m_stNLAxis_Z1);
			else if (szAxis == NL_AXIS_Z2)
				CMS896AStn::MotionCheckResult(szAxis, &m_stNLAxis_Z2);
		}
	}
}

VOID CNVCLoader::GetAxisInformation()
{
	InitAxisData(m_stNLAxis_X);
	m_stNLAxis_X.m_szName				= NL_AXIS_X;
	m_stNLAxis_X.m_szTag				= MS896A_CFG_CH_NVCLOADER_X;
	m_stNLAxis_X.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stNLAxis_X);

	InitAxisData(m_stNLAxis_Y);
	m_stNLAxis_Y.m_szName				= NL_AXIS_Y;
	m_stNLAxis_Y.m_szTag				= MS896A_CFG_CH_NVCLOADER_Y;
	m_stNLAxis_Y.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stNLAxis_Y);

	InitAxisData(m_stNLAxis_Z1);
	m_stNLAxis_Z1.m_szName				= NL_AXIS_Z1;
	m_stNLAxis_Z1.m_szTag				= MS896A_CFG_CH_NVCLOADER_Z1;
	m_stNLAxis_Z1.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stNLAxis_Z1);
	
	InitAxisData(m_stNLAxis_Z2);
	m_stNLAxis_Z2.m_szName				= NL_AXIS_Z2;
	m_stNLAxis_Z2.m_szTag				= MS896A_CFG_CH_NVCLOADER_Z2;
	m_stNLAxis_Z2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
	GetAxisData(m_stNLAxis_Z2);

	//For debug only
	PrintAxisData(m_stNLAxis_X);
	PrintAxisData(m_stNLAxis_Y);
	PrintAxisData(m_stNLAxis_Z1);
	PrintAxisData(m_stNLAxis_Z2);
}


///////////////////////////////////////////////////////////////////////////
//	NL sw Event Signal Functions 
///////////////////////////////////////////////////////////////////////////

VOID CNVCLoader::SetNLReady(BOOL bState)
{
	if (bState)
	{
		m_evNLReady.SetEvent();
	}
	else
	{
		m_evNLReady.ResetEvent();
	}
}

VOID CNVCLoader::SetNLOperate(BOOL bState)
{
	if (bState)
	{
		m_evNLOperate.SetEvent();
	}
	else
	{
		m_evNLOperate.ResetEvent();
	}
}

VOID CNVCLoader::SetNLPreOperate(BOOL bState)
{
	if (bState)
	{
		m_evNLPreOperate.SetEvent();
	}
	else
	{
		m_evNLPreOperate.ResetEvent();
	}
}

BOOL CNVCLoader::WaitNLReady(INT nTimeout)
{
	CSingleLock slLock(&m_evNLReady);
	return slLock.Lock(nTimeout);
}

BOOL CNVCLoader::WaitNLPreOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evNLPreOperate);
	return slLock.Lock(nTimeout);
}

BOOL CNVCLoader::WaitNLOperate(INT nTimeout)
{
	CSingleLock slLock(&m_evNLOperate);
	return slLock.Lock(nTimeout);
}
