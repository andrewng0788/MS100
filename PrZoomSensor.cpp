#include "stdafx.h"
#include <algorithm>
#include <mbstring.h>
#include <io.h>
#include "PrZoomSensor.h"
#include "prGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPrZoomSensorMode::CPrZoomSensorMode(const PR_CAMERA emCurCamera, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, CPrGeneral *pPrGeneral)
{
	m_emCurCamera	= emCurCamera;
	m_ubSenderID	= ubSenderID;
	m_ubReceiverID	= ubReceiverID;
	m_pPrGeneral	= pPrGeneral;

	m_bContinuousZoom		= FALSE;
	m_dZoomContinuousFactor = 0.0;
	m_dZoomROIShrink		= 100;
	m_ucFovShrinkMode		= 0;

	m_lZoomSensorMode = PR_ZOOM_MODE_4X;
	m_stAdjustedROI.rcoCenter.x = PR_DEF_CENTRE_X;
	m_stAdjustedROI.rcoCenter.y = PR_DEF_CENTRE_Y;
	m_stAdjustedROI.rszSize.x = 0;
	m_stAdjustedROI.rszSize.y = 0;
}

CPrZoomSensorMode::~CPrZoomSensorMode()
{

}

VOID CPrZoomSensorMode::SetSensorZoomCamera(const PR_CAMERA emCurCamera, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID)
{
	m_emCurCamera	= emCurCamera;
	m_ubSenderID	= ubSenderID;
	m_ubReceiverID	= ubReceiverID;
}


VOID CPrZoomSensorMode::SetSensorZoomFactor(const BOOL bContinuousZoom, const DOUBLE dZoomContinuousFactor, const DOUBLE dZoomROIShrink, const UCHAR ucFovShrinkMode)
{
	m_bContinuousZoom		= bContinuousZoom;
	m_dZoomContinuousFactor = dZoomContinuousFactor;
	m_dZoomROIShrink		= dZoomROIShrink;
	m_ucFovShrinkMode		= ucFovShrinkMode;
}


PR_WORD CPrZoomSensorMode::ZoomSensor(const LONG lZoomMode, const PR_WORD ssType, const BOOL bContinuousZoom)
{
#ifdef VS_5MCAM
	DOUBLE dZoomFactor = m_dZoomContinuousFactor;	//	continue zoom factor
	DOUBLE dZoomShrink = m_dZoomROIShrink;			//	ROI shrink factor

	DOUBLE dZoomMain = 1.0;
	CString szZoomDisplay = "";
	PR_REAL rSizeX = (PR_REAL)(PR_MAX_COORD + 1);
	PR_REAL rSizeY = (PR_REAL)(PR_MAX_COORD + 1);
	PR_ZOOM_SENSOR_MODE		emZoomMode = PR_ZOOM_SENSOR_MODE_4X_ZOOM_NORMAL;
	switch(lZoomMode)
	{
	case PR_ZOOM_MODE_FF:
		emZoomMode = PR_ZOOM_SENSOR_MODE_DISABLE;
		if (fabs(dZoomShrink - 100.0) > 0.000001 && m_ucFovShrinkMode == 1)
		{
			rSizeX = (PR_REAL)(rSizeX * dZoomShrink / 100.0);
			rSizeY = (PR_REAL)((PR_MAX_WIN_LRC_Y - PR_MAX_WIN_ULC_Y) * dZoomShrink / 100.0);
			emZoomMode = PR_ZOOM_SENSOR_MODE_FULL_FRAME_SUBCUT;
		}
		szZoomDisplay = PR_ZOOM_VALUE_FF;
		break;
	
	case PR_ZOOM_MODE_1X:
		emZoomMode = PR_ZOOM_SENSOR_MODE_1X_ZOOM_NORMAL_BIN;
		szZoomDisplay = PR_ZOOM_VALUE_1X;
		dZoomMain = 1.0;
		break;

	case PR_ZOOM_MODE_2X:
		emZoomMode = PR_ZOOM_SENSOR_MODE_2X_ZOOM_NORMAL_BIN;
		szZoomDisplay = PR_ZOOM_VALUE_2X;
		dZoomMain = 2.0;
		break;

	case PR_ZOOM_MODE_4X:
	default:
		szZoomDisplay = PR_ZOOM_VALUE_4X;
		dZoomMain = 3.0;
		break;
	}

	m_lZoomSensorMode = lZoomMode;

	PR_SET_VIDEO emType = PR_SET_VIDEO_BOTH;
	switch(ssType)
	{
	default:
		emType = PR_SET_VIDEO_BOTH;
		break;
	
	case 1:
		emType = PR_SET_VIDEO_GRAB;
		break;

	case 2:
		emType = PR_SET_VIDEO_LV;
		break;		
	}

	PR_ZOOM_SENSOR_METHOD	emZoomMethod = PR_DISCRETE_ZOOM_SENSOR;
	PR_CONT_ZOOM_SENSOR_PARA stContZoomPar;

	//Check Continue Zoom Mode
	if (bContinuousZoom)
	{
		emZoomMethod					= PR_CONTINUOUS_ZOOM_SENSOR;
		stContZoomPar.emBinSkip			= PR_ZOOM_BINSKIP_BINNING;	//	PR_ZOOM_BINSKIP_SKIPPING
		stContZoomPar.rContZoomFactor	= (PR_REAL)(dZoomMain + dZoomFactor / 100.0); //from 1 to 4
		szZoomDisplay.Format("%.1f", dZoomMain + dZoomFactor / 100);
	}

	m_szZoomFactor = szZoomDisplay;

	PR_REAL_ROI stROI;
	stROI.rcoCenter.x = PR_DEF_CENTRE_X;
	stROI.rcoCenter.y = PR_DEF_CENTRE_Y;
	stROI.rszSize.x	= rSizeX;
	stROI.rszSize.y	= rSizeY;

	PR_REAL_ROI RpyROI;
	LONG nErr = 0;
	if ((nErr = m_pPrGeneral->PRS_SensorZoom(m_emCurCamera, m_ubSenderID, m_ubReceiverID, emType, emZoomMode, emZoomMethod, stContZoomPar, stROI, &RpyROI, m_szErrMsg)) > 0)
	{
		return FALSE;
	}
#endif

	return TRUE;
}


LONG CPrZoomSensorMode::GetZoomSensorMode()
{
	return m_lZoomSensorMode;
}


PR_ZOOM_FACTOR CPrZoomSensorMode::GetZoomFactor(const LONG lZoomMode)
{
	PR_ZOOM_FACTOR emZoomFactor = PR_ZOOM_FACTOR_1X;

	switch(lZoomMode)
	{
	case PR_ZOOM_MODE_FF:
		emZoomFactor = PR_ZOOM_FACTOR_4X;
		break;
	
	case PR_ZOOM_MODE_1X:
		emZoomFactor = PR_ZOOM_FACTOR_1X;
		break;

	case PR_ZOOM_MODE_2X:
		emZoomFactor = PR_ZOOM_FACTOR_2X;
		break;

	case PR_ZOOM_MODE_4X:
	default:
		emZoomFactor = PR_ZOOM_FACTOR_4X;
		break;
	}

	return emZoomFactor;
}
