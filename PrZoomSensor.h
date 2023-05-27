#ifndef __PR_ZOOM_SENSOR_H
#define __PR_ZOOM_SENSOR_H

#pragma once
#include <prheader.h>
#include <prmsg.h>
#include <prstatus.h>
#include <prtype.h>

#define PR_ZOOM_VALUE_FF		"FF"
#define PR_ZOOM_VALUE_1X		"1X"
#define PR_ZOOM_VALUE_2X		"2X"
#define PR_ZOOM_VALUE_4X		"4X"

class CPrGeneral;
class CPrZoomSensorMode
{
	//Add Digital Zoom
public:
	enum emZoomSnrZoomMode	{PR_ZOOM_SNR_ZOOM_MODE_BOTH = 0,
							 PR_ZOOM_SNR_ZOOM_MODE_GRAB,
							 PR_ZOOM_SNR_ZOOM_MODE_LV
							};

	enum emZoomFactorMode	{PR_ZOOM_MODE_FF = 0,
							 PR_ZOOM_MODE_1X,
							 PR_ZOOM_MODE_2X,
							 PR_ZOOM_MODE_4X
							};

	CString m_szZoomFactor;
	CString m_szErrMsg;
private:
	PR_CAMERA m_emCurCamera;
	PR_UBYTE m_ubSenderID;
	PR_UBYTE m_ubReceiverID;
	CPrGeneral	*m_pPrGeneral;

	//Continue Zoom
	BOOL	m_bContinuousZoom;			// Is Continuous Zoom
	DOUBLE	m_dZoomContinuousFactor;	// Continuous zoom factor
	DOUBLE	m_dZoomROIShrink;			// ROI shrink factor
	UCHAR	m_ucFovShrinkMode;

	long m_lZoomSensorMode;
	PR_REAL_ROI m_stAdjustedROI;  //Output

public:
	CPrZoomSensorMode(const PR_CAMERA emCurCamera, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, CPrGeneral *pPrGeneral);
	virtual ~CPrZoomSensorMode();

	VOID SetSensorZoomCamera(const PR_CAMERA emCurCamera, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID);
	VOID SetSensorZoomFactor(const BOOL bContinuousZoom, const DOUBLE dZoomContinuousFactor, const DOUBLE dZoomROIShrink, const UCHAR ucFovShrinkMode);
	PR_WORD ZoomSensor(const LONG lZoomMode, const PR_WORD ssType, const BOOL bContinuousZoom);
	LONG GetZoomSensorMode();
	PR_ZOOM_FACTOR GetZoomFactor(const LONG lZoomMode);
};

#endif