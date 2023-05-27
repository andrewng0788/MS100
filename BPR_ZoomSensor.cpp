/////////////////////////////////////////////////////////////////
// BPR_LrnDie.cpp : Learn Die Function of the CBondPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wednesday, December 1, 2004
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBondPr::SetSensorZoomFFMode(BOOL bSensorZoomFFMode)
{
	m_bSensorZoomFFMode = bSensorZoomFFMode;
}

BOOL CBondPr::IsSensorZoomFFMode()
{
	return m_bSensorZoomFFMode;
}

LONG CBondPr::GetBondPRDieNo()
{
	return IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 : BPR_NORMAL_PR_DIE_INDEX1;
}

LONG CBondPr::GetBondPRDieNo(LONG lDieNo)
{
	return IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 : lDieNo;
}

BOOL CBondPr::IsEnableZoom()
{
	return TRUE; //m_stZoomView.m_bEnableZoomSensor;
}

VOID CBondPr::InitZoomView()
{
	//Zoom Sensor
	m_bSensorZoomFFMode	= FALSE;
	m_szBprZoomFactor	= "1X";

	m_bBprContinuousZoom	= FALSE;
	m_lBprContZoomFactor	= 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_lBprZoomRoiShrink		= pApp->GetProfileInt(gszPROFILE_SETTING, _T("BPR FOV ROI Shrink Factor"), 100);
	if( m_lBprZoomRoiShrink<50 || m_lBprZoomRoiShrink>100 )
	{
		m_lBprZoomRoiShrink = 100;
	}

	m_lBprZoomDigital	= 0;
	m_lBprRunZoomFactor	= -1;
	m_lBprNmlZoomFactor	= 0;
}

VOID CBondPr::BPR_ZoomScreen(short hZoomFactor)
{
	m_lBprZoomDigital = hZoomFactor;
	m_pPrGeneral->DigitalZoomScreen(hZoomFactor, (PR_WORD)GetPrCenterX(), (PR_WORD)GetPrCenterY(), MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID);
}

//	Bond PR relative zoom command.
LONG CBondPr::BPR_GetNmlZoom()
{
	if (m_lBprNmlZoomFactor < 1)
	{
		m_lBprNmlZoomFactor = 3;
	}
	return m_lBprNmlZoomFactor;
}

LONG CBondPr::BPR_GetRunZoom()
{
	return m_lBprRunZoomFactor;
}

VOID CBondPr::BPR_SetNmlZoomFactor(LONG lZoomFacotr)
{
#ifdef VS_5MCAM
		m_lBprNmlZoomFactor = lZoomFacotr;
		SavePrData(FALSE);
#endif
}

LONG CBondPr::BPR_GetZoomDigital()
{
	return m_lBprZoomDigital;
}


BOOL CBondPr::BPR_LiveViewZoom()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("BPR FOV ROI Shrink Factor"), m_lBprZoomRoiShrink);
	return BPR_LiveViewZoom(BPR_GetRunZoom());
}

BOOL CBondPr::BPR_LiveViewZoom(LONG lZoomFactor)
{
	if (IsEnableZoom() == FALSE)
	{
		return TRUE;
	}

#ifdef VS_5MCAM

//	if( BPR_GetRunZoom()==lZoomFactor )
//	{
//		return TRUE;
//	}

	m_lBprRunZoomFactor = lZoomFactor;

	// temp to disable the sensor zoom function
	BPR_ZoomSensor(lZoomFactor, CPrZoomSensorMode::PR_ZOOM_SNR_ZOOM_MODE_BOTH);
#endif

	return TRUE;
}

BOOL CBondPr::BPR_ZoomSensor(LONG lZoomMode, PR_WORD ssType)
{
#ifdef VS_5MCAM
	m_pPrZoomSensorMode->SetSensorZoomCamera(MS899_BOND_CAM_ID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID);

	//Check Continue Zoom Mode
	BOOL bConZoom = FALSE;
	if (lZoomMode != 0 && m_bBprContinuousZoom)
	{
		bConZoom = TRUE;
	}

	m_pPrZoomSensorMode->SetSensorZoomFactor(bConZoom, m_lBprContZoomFactor, m_lBprZoomRoiShrink, 1);

	PR_WORD lRet = m_pPrZoomSensorMode->ZoomSensor(lZoomMode, ssType, bConZoom);

	m_szBprZoomFactor = m_pPrZoomSensorMode->m_szZoomFactor;

	if (lRet == 0)
	{
		CString szMsg;
		szMsg = m_pPrZoomSensorMode->m_szErrMsg;
		SetAlarmLog("BPR: DP" + szMsg);
		HmiMessage_Red_Back("BPR: DP" + szMsg);
		return FALSE;
	}

#endif
	SetSensorZoomFFMode((lZoomMode == CPrZoomSensorMode::PR_ZOOM_MODE_FF) ? TRUE : FALSE);
	
	return TRUE;
}


LONG CBondPr::ToggleLearnBondPosnZoomScreenCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bTo4X = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bTo4X);

	if (bTo4X)
	{
		BPR_LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X);
	}
	else
	{
		BPR_LiveViewZoom(BPR_GetNmlZoom());
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	// Toggle zoom screen between 4X and learn die
