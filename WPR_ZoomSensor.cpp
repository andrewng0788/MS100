#include "stdafx.h"
#include "MS896A_Constant.h"
#include "MS896A.h"
#include "WaferPr.h"
#include "FileUtil.h"
#include "GallerySearchController.h"
#include "PrZoomSensor.h"

//	Dual Path
BOOL CWaferPr::IsDP()
{
	return m_bDualPath;
}

BOOL CWaferPr::IsDP_ScanCam()
{
	BOOL bReturn = IsDP() && m_bRunIsDP_ScanCam;
	return bReturn;
}

PR_CAMERA	CWaferPr::GetRunCamID()
{
	if( IsDP_ScanCam() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Run cam ID %d", MS_DP_WAF_5M_CAM_ID);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_CAM_ID;
	}

	return MS899_WAF_CAM_ID;
}

PR_CAMERA	CWaferPr::GetScnCamID()
{
	if( IsDP() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Scn cam ID %d", MS_DP_WAF_5M_CAM_ID);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_CAM_ID;
	}
	return MS899_WAF_CAM_ID;
}

PR_UBYTE	CWaferPr::GetRunSenID()
{
	if( IsDP_ScanCam() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Run sen ID %d", MS_DP_WAF_5M_SEND_ID);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_SEND_ID;
	}
	return MS899_WAF_CAM_SEND_ID;
}

PR_UBYTE	CWaferPr::GetScnSenID()
{
	if( IsDP() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Scn sen ID %d", MS_DP_WAF_5M_SEND_ID);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_SEND_ID;
	}
	return MS899_WAF_CAM_SEND_ID;
}

PR_UBYTE	CWaferPr::GetRunRecID()
{
	if( IsDP_ScanCam() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Run rec ID %d", MS_DP_WAF_5M_RECV_ID);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_RECV_ID;
	}
	return MS899_WAF_CAM_RECV_ID;
}

PR_UBYTE	CWaferPr::GetScnRecID()
{
	if( IsDP() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Scn rec ID %d", MS_DP_WAF_5M_RECV_ID);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_RECV_ID;
	}
	return MS899_WAF_CAM_RECV_ID;
}

PR_PURPOSE	CWaferPr::GetRunPurposeI()	//	purpose for inspection
{
	if( IsDP_ScanCam() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Run PPS  I %d", MS_DP_WAF_5M_PR_PURPOSE);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_PR_PURPOSE;
	}
	return MS899_WAF_INSP_PURPOSE;
}

PR_PURPOSE	CWaferPr::GetScnPurpose()
{
	if( IsDP() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Scn PPS    %d", MS_DP_WAF_5M_GN_PURPOSE);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_GN_PURPOSE;
	}
	return MS899_WAF_GEN_PURPOSE;
}

PR_PURPOSE	CWaferPr::GetRunPurposeG()
{
	if( IsDP_ScanCam() )
	{
CString szMsg;
szMsg.Format("WPR: DP scan cam Run PPS  G %d", MS_DP_WAF_5M_GN_PURPOSE);	//SetAlarmLog(szMsg);
		return MS_DP_WAF_5M_GN_PURPOSE;
	}
	return MS899_WAF_GEN_PURPOSE;
}

BOOL CWaferPr::DP_GetAllLightings()
{
	if( IsDP_ScanCam() )
	{
		m_bContinuousZoomHMI		= m_bContinuousZoomDP5M;
		m_dConZoomSubFactorHMI		= m_dConZoomSubFactorDP5M;
		m_dWprZoomRoiShrinkHMI		= m_dWprZoomRoiShrinkDP5M;
		m_lWPRGenExposureTimeHmi	= m_lSCMGenExposureTimeLevel;
		m_lWPRLrnCoaxLightHmi		= m_lSCMLrnCoaxLightLevel;
		m_lWPRLrnRingLightHmi		= m_lSCMLrnRingLightLevel;
		m_lWPRLrnSideLightHmi		= m_lSCMLrnSideLightLevel;
		m_lWprLrnExposureTimeHmi	= m_lSCMLrnExposureTimeLevel;
	}
	else
	{
		m_bContinuousZoomHMI		= m_bContinuousZoom;
		m_dConZoomSubFactorHMI		= m_dNmlZoomSubFactor;
		m_dWprZoomRoiShrinkHMI		= m_dWprZoomRoiShrink;
		m_lWPRGenExposureTimeHmi	= m_lWPRGenExposureTimeLevel;
		m_lWPRLrnCoaxLightHmi		= m_lWPRLrnCoaxLightLevel[0];
		m_lWPRLrnRingLightHmi		= m_lWPRLrnRingLightLevel[0];
		m_lWPRLrnSideLightHmi		= m_lWPRLrnSideLightLevel[0];
		m_lWprLrnExposureTimeHmi	= m_lWPRLrnExposureTimeLevel;
	}
	WPR_GetGeneralLighting();	//	update when switch

	return TRUE;
}

LONG CWaferPr::WPR_ToggleZoom(BOOL bToScan, BOOL bGrab, INT nDebug)
{
	if( IsEnableZoom()==FALSE )
	{
		return -1;	//	not zoom mode
	}

	BOOL bToggle = FALSE;
	if( IsDP() )
	{
		if( bToScan != m_bRunIsDP_ScanCam )
		{
if( bToScan )
SetAlarmLog("WPR: DP toggle to scan cam");
else
SetAlarmLog("WPR: DP toggle to sort cam");
			bToggle = TRUE;
			m_bAutoBondMode = TRUE;
			m_bRunIsDP_ScanCam = bToScan;
			ChangeCamera(WPR_CAM_WAFER);
		}
	}
	else //	if( GetScnZoom()!=GetNmlZoom() || GetRunZoom()==-1 )
	{
		bToggle = TRUE;
	}

	if( bToggle )
	{
		LONG lZoom = GetNmlZoom();
		if( bToScan )
		{
			lZoom = GetScnZoom();
		}
		if (!LiveViewZoom(lZoom, bGrab, 1, 27))
		{
			CString szErr;
			szErr.Format("WPR_ToggleZoom: LiveViewZoom ERR = %d", nDebug);
			HmiMessage(szErr);
			return FALSE;
		}
		return 1;	//	toggle done
	}

	return -1;	//	no need toggle
}

//	Dual Path

BOOL CWaferPr::IsZoomFreezed()
{
	return m_stZoomView.m_bIsFixZoom;
}

BOOL CWaferPr::IsEnableZoom()
{
	return m_stZoomView.m_bEnableZoomSensor;
}

VOID CWaferPr::InitZoomView()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	m_stZoomView.m_bEnableZoomSensor = FALSE;
	m_stZoomView.m_szZoomFactor		= "1X";
	m_stZoomView.m_bIsFixZoom		= FALSE;
	m_bRunIsDP_ScanCam				= FALSE;

	m_lNmlDigitalZoom	= PR_ZOOM_FACTOR_1X;
	m_lRunDigitalZoom	= PR_ZOOM_FACTOR_1X;
	m_lRunZoomFactor	= -1;

	m_lGrabDigitalZoom	= 0;
	m_lNmlZoomFactor	= 0;
	m_lScnZoomFactor	= 0;
	m_bContinuousZoom	= FALSE;
	m_ucFovShrinkMode	= 0;
	m_dNmlZoomSubFactor		= 0.0;
	CString szCut = pApp->GetProfileString(gszPROFILE_SETTING, _T("CP FOV sub cut num(double)"), "1.0");	// init
	m_dWprZoomRoiShrink		= fabs(atof(szCut))*100.0;

	m_bDualPath = FALSE;
	m_bContinuousZoomHMI	= FALSE;
	m_dConZoomSubFactorHMI	= 0.0;
	m_dWprZoomRoiShrinkHMI	= 100.0;
	m_bContinuousZoomDP5M	= FALSE;
	m_dConZoomSubFactorDP5M	= 0.0;
	m_dWprZoomRoiShrinkDP5M	= 100.0;

	m_stZoomView.m_dScanCalibXX = 0.0;
	m_stZoomView.m_dScanCalibXY = 0.0;
	m_stZoomView.m_dScanCalibYY = 0.0;
	m_stZoomView.m_dScanCalibYX = 0.0;
}

// test whehter zoom sensor exist or not
PR_WORD CWaferPr::InitZoomSensor()
{
	m_bRunIsDP_ScanCam				= FALSE;
	m_stZoomView.m_bIsFixZoom		= FALSE;
	m_stZoomView.m_bEnableZoomSensor = FALSE;
#ifdef VS_5MCAM
	m_stZoomView.m_bEnableZoomSensor = TRUE;
#endif

	return TRUE;
}

VOID CWaferPr::SetZoomViewFixZoom(BOOL bSet)
{
	m_stZoomView.m_bIsFixZoom = bSet;
}

BOOL CWaferPr::LiveViewZoom(LONG lZoomFactor, BOOL bGrab, BOOL bChange, INT nDebug)
{
	if (IsEnableZoom() == FALSE)
	{
		return TRUE;
	}

#ifdef VS_5MCAM
	PR_COORD stROICenter;

	BOOL bConZoom = m_bContinuousZoom;
	if( IsDP_ScanCam() )
		bConZoom = m_bContinuousZoomDP5M;
	LONG lOldZoomFactor = GetRunZoom();
	if( IsDP()==FALSE && lOldZoomFactor==lZoomFactor && bConZoom==FALSE && bChange==FALSE )
	{
		return TRUE;
	}

	m_lRunZoomFactor = lZoomFactor;

	stROICenter.x = (PR_WORD)GetPrCenterX();
	stROICenter.y = (PR_WORD)GetPrCenterY();

	// for mouse move only
	(*m_psmfSRam)["WaferPr"]["CursorCenter"]["X"]	= stROICenter.x;
	(*m_psmfSRam)["WaferPr"]["CursorCenter"]["Y"]	= stROICenter.y;

	// temp to disable the sensor zoom function
	PR_WORD wZoomMode = CPrZoomSensorMode::PR_ZOOM_SNR_ZOOM_MODE_BOTH;
	if (bGrab)
	{
		wZoomMode = CPrZoomSensorMode::PR_ZOOM_SNR_ZOOM_MODE_GRAB;
	}

	if (!WPR_ZoomSensor(lZoomFactor, wZoomMode))
	{
		CString szErr;
		szErr.Format("LiveViewZoom WPR_ZoomSensor error = %d", nDebug);
		HmiMessage(szErr);		//andrewng //2020-0616
		return FALSE;
	}

	WPR_DigitalZoom((short)m_lNmlDigitalZoom);
	DrawHomeCursor(stROICenter);
#endif

	return TRUE;
}

VOID CWaferPr::ChangeDirectView(const BOOL bDirectView)
{
	//LiveViewZoom(GetNmlZoom(), FALSE);
	AutoBondScreen(bDirectView ? FALSE : TRUE);
}

PR_WORD CWaferPr::WPR_ZoomSensor(LONG lZoomMode, PR_WORD ssType)
{
#ifdef VS_5MCAM
	m_pPrZoomSensorMode->SetSensorZoomCamera(GetRunCamID(), GetRunSenID(), GetRunRecID());
	//Check Continue Zoom Mode
	BOOL bConZoom = FALSE;
	if( IsDP_ScanCam() )
	{
		if( lZoomMode!=0 && m_bContinuousZoomDP5M )
		{
			bConZoom = TRUE;
		}
	}
	else
	{
		if( lZoomMode!=0 && m_bContinuousZoom && (GetNmlZoom()==GetScnZoom() || lZoomMode!=GetScnZoom()) )
		{
			bConZoom = TRUE;
		}
	}

	DOUBLE dZoomFactor = m_dNmlZoomSubFactor;	//	continue zoom factor
	DOUBLE dZoomShrink = m_dWprZoomRoiShrink;
	if( IsDP_ScanCam() )
	{
		dZoomShrink = m_dWprZoomRoiShrinkDP5M;
		dZoomFactor = m_dConZoomSubFactorDP5M;
	}
	m_pPrZoomSensorMode->SetSensorZoomFactor(bConZoom, dZoomFactor, dZoomShrink, m_ucFovShrinkMode);

	LONG lOldZoomMode = m_pPrZoomSensorMode->GetZoomSensorMode();
	PR_WORD lRet = m_pPrZoomSensorMode->ZoomSensor(lZoomMode, ssType, bConZoom);

	LONG lCurZoomMode = m_pPrZoomSensorMode->GetZoomSensorMode();
	SetTransferEjtColletCoordInZoomMode(lOldZoomMode, lCurZoomMode);

	m_stZoomView.m_szZoomFactor = m_pPrZoomSensorMode->m_szZoomFactor;

	if (lRet == 0)
	{
		CString szMsg;
		szMsg = m_pPrZoomSensorMode->m_szErrMsg;
		SetAlarmLog("WPR: DP" + szMsg);
		HmiMessage_Red_Back(szMsg);
		return FALSE;
	}
#endif

	return TRUE;
}

VOID CWaferPr::SetTransferEjtColletCoordInZoomMode(LONG lCurZoomMode, LONG lNewZoomMode)
{
	PR_ZOOM_FACTOR emCurZoomFactor = m_pPrZoomSensorMode->GetZoomFactor(lCurZoomMode);
	PR_ZOOM_FACTOR emNewZoomFactor = m_pPrZoomSensorMode->GetZoomFactor(lNewZoomMode);
	PR_COORD coCurrentCenterIn1xImg = {4096, 4096};
	PR_COORD coZoomPointInCurrentImg, coResultPoint = {0, 0};
	PR_COORD coExpectCenterInCurrentImg = {4096, 4096};
	PR_BOOLEAN emIsWithinRange = PR_FALSE;
	coZoomPointInCurrentImg.x = (PR_WORD)m_lEjtCollet1CoorX;
	coZoomPointInCurrentImg.y = (PR_WORD)m_lEjtCollet1CoorY;
	PR_GetPtInExpZmImgFromCurrZmImg(emCurZoomFactor, coCurrentCenterIn1xImg, coZoomPointInCurrentImg, coExpectCenterInCurrentImg, emNewZoomFactor, &coResultPoint, &emIsWithinRange);
	if (emIsWithinRange)
	{
		m_lEjtCollet1CoorX = coResultPoint.x;
		m_lEjtCollet1CoorY = coResultPoint.y;
	}

	emIsWithinRange = PR_FALSE;
	coZoomPointInCurrentImg.x = (PR_WORD)m_lEjtCollet2CoorX;
	coZoomPointInCurrentImg.y = (PR_WORD)m_lEjtCollet2CoorY;

	PR_GetPtInExpZmImgFromCurrZmImg(emCurZoomFactor, coCurrentCenterIn1xImg, coZoomPointInCurrentImg, coExpectCenterInCurrentImg, emNewZoomFactor, &coResultPoint, &emIsWithinRange);
	if (emIsWithinRange)
	{
		m_lEjtCollet2CoorX = coResultPoint.x;
		m_lEjtCollet2CoorY = coResultPoint.y;
	}
}


VOID CWaferPr::SetNormalZoomFactor(LONG lZoomFacotr)
{
#ifdef VS_5MCAM
	if (IsEnableZoom() == FALSE)
	{
		return;
	}

	m_lNmlZoomFactor = lZoomFacotr;
#endif
}

VOID CWaferPr::SetPrescanZoomFactor(LONG lZoomFactor)
{
#ifdef VS_5MCAM
	if (IsEnableZoom() == FALSE)
	{
		return;
	}

	m_lScnZoomFactor = lZoomFactor;
#endif
}

VOID CWaferPr::SetZoomViewCalibration(double dCalibX, double dCalibY, double dCalibXY, double dCalibYX)
{
#ifdef VS_5MCAM
	if (IsEnableZoom() == FALSE)
	{
		return;
	}

	m_stZoomView.m_dScanCalibXX = dCalibX;
	m_stZoomView.m_dScanCalibYY = dCalibY;
	m_stZoomView.m_dScanCalibXY = dCalibXY;
	m_stZoomView.m_dScanCalibYX = dCalibYX;
#endif
}

VOID CWaferPr::SetFFCalibration(double dCalibX, double dCalibY, double dCalibXY, double dCalibYX)
{
#ifdef VS_5MCAM
	if (IsEnableZoom() == FALSE)
	{
		return;
	}

	m_dFFCalibX = dCalibX;
	m_dFFCalibY = dCalibY;
	m_dFFCalibXY = dCalibXY;
	m_dFFCalibYX = dCalibYX;

	CString szMsg;
	szMsg.Format("FF Calib is set,%f,%f,%f,%f",dCalibX,dCalibY,dCalibXY,dCalibYX);
	HmiMessage(szMsg);
#endif
}


// e.g from 4X to 1X

VOID CWaferPr::DrawSearchBox(PR_COLOR ssColor)
{
	return DrawRectangleBox(GetSrchArea().coCorner1, GetSrchArea().coCorner2, ssColor);
}

LONG CWaferPr::GetRunDigitalZoom()
{
	return m_lRunDigitalZoom;
}

LONG CWaferPr::GetRunZoom()
{
	return m_lRunZoomFactor;
}

LONG CWaferPr::GetNmlZoom()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetFeatureStatus(MS60_VISION_5M_SORTING_FF_MODE)==FALSE )	//	zoomview
	{
		if (m_lNmlZoomFactor < 1)
		{
			m_lNmlZoomFactor = 3;
		}
	}
	return m_lNmlZoomFactor;
}

LONG CWaferPr::GetScnZoom()
{
	return m_lScnZoomFactor;
}


LONG CWaferPr::FindScanCalibration(VOID)
{
#define	PR_CALIB_LOOP	2	//	must > 1
	typedef struct 
	{
		int X;
		int Y;
	} MOTORDATA;

	PR_REAL			fDieRotate; 
	PR_UWORD		usDieType;
	PR_WORD			uwResult;

	PR_COORD		stDieWprCtrPosn; 
	PR_COORD		stDieWprPosn[2]; 
	MOTORDATA		stDieWftPosn[2];
	PR_COORD		stXPixelMove, stYPixelMove; 
	MOTORDATA		stXMotorMove, stYMotorMove;

	PR_WIN			stOrgSearchArea = m_stSearchArea;
	BOOL			bTempCorrection = m_bThetaCorrection;

	PR_WIN			stCtrSrchWin, stRunSrchWin;

	PR_WORD 	wSrchHalfPixelX = (PR_WORD)(1.3 * GetDieSizePixelX(GetPrescanPrID()-1));
	PR_WORD 	wSrchHalfPixelY = (PR_WORD)(1.3 * GetDieSizePixelY(GetPrescanPrID()-1));

	INT		nPrFovUL_X = GetScanPRWinULX() + 32 * GetPrScaleFactor() + wSrchHalfPixelX;
	INT		nPrFovLR_X = GetScanPRWinLRX() - 32 * GetPrScaleFactor() - wSrchHalfPixelX;
	INT		nPrFovUL_Y = GetScanPRWinULY() + 32 * GetPrScaleFactor() + wSrchHalfPixelY;
	INT		nPrFovLR_Y = GetScanPRWinLRY() - 32 * GetPrScaleFactor() - wSrchHalfPixelY;

	//Update Search area
	stCtrSrchWin.coCorner1.x = (PR_WORD)GetPrCenterX() - wSrchHalfPixelX;
	stCtrSrchWin.coCorner1.y = (PR_WORD)GetPrCenterY() - wSrchHalfPixelY;
	stCtrSrchWin.coCorner2.x = (PR_WORD)GetPrCenterX() + wSrchHalfPixelX;
	stCtrSrchWin.coCorner2.y = (PR_WORD)GetPrCenterY() + wSrchHalfPixelY;
	VerifyPRRegion(&stCtrSrchWin);

	CString szMsg;

	//Temp to dsiable  Theta Correction
	m_bThetaCorrection = FALSE;

	INT			nCtrWftPosnX, nCtrWftPosnY, siThetaPos;
	INT			nWftStepXX = m_lPrCal1stStepSize, nWftStepXY = 0;
	INT			nWftStepYY = m_lPrCal1stStepSize, nWftStepYX = 0;

	PR_COORD	stPrStepX, stPrStepY; 
	stPrStepX.x = GetDieSizePixelX(GetPrescanPrID()-1);
	stPrStepX.y = 0;
	stPrStepY.x = 0;
	stPrStepY.y = GetDieSizePixelY(GetPrescanPrID()-1);

	PR_COORD stPixel;
	stPixel.x = GetDieSizePixelX(0);
	stPixel.y = GetDieSizePixelY(0);
	ConvertPixelToMotorStep(stPixel, &nWftStepXX, &nWftStepYY);
	LONG lReturn = 0;
	DOUBLE  dCalibX = 0, dCalibXY = 0, dCalibY = 0, dCalibYX = 0;
	//Start learning
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPps = GetScnPurpose();
	for (short ssCycle	= 0; ssCycle <= PR_CALIB_LOOP; ssCycle++)
	{
		szMsg.Format("Scan Calibration cycle %d", ssCycle);
		SetAlarmLog(szMsg);

		//Search Die on center/start position
		GetWaferTableEncoder(&nCtrWftPosnX, &nCtrWftPosnY, &siThetaPos);
		DrawRectangleBox(stCtrSrchWin.coCorner1, stCtrSrchWin.coCorner2, PR_COLOR_GREEN);
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDieWprCtrPosn, stCtrSrchWin);

		DrawRectangleBox(stCtrSrchWin.coCorner1, stCtrSrchWin.coCorner2, PR_COLOR_TRANSPARENT);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			HmiMessage("Please locate die to PR cross hair center.");
			lReturn = WPR_ERR_CALIB_NO_1ST_DIE;
			break;
		}

		if (ssCycle != 0)
		{
			//Only update search area & calculate motor step on cycle 1 & 2 
			stPrStepX.x = (PR_WORD)(min(labs(stDieWprCtrPosn.x - nPrFovUL_X), labs(nPrFovLR_X - stDieWprCtrPosn.x)) * ssCycle / PR_CALIB_LOOP);
			stPrStepX.y = 0;
			ConvertPixelToMotorStep(stPrStepX, &nWftStepXX, &nWftStepXY, dCalibX, dCalibY, dCalibXY, dCalibYX);

			stPrStepY.x = 0;
			stPrStepY.y = (PR_WORD)(min(labs(stDieWprCtrPosn.y - nPrFovUL_Y), labs(nPrFovLR_Y - stDieWprCtrPosn.y)) * ssCycle/PR_CALIB_LOOP);
			ConvertPixelToMotorStep(stPrStepY, &nWftStepYX, &nWftStepYY, dCalibX, dCalibY, dCalibXY, dCalibYX);
		}

		// Move Table X + direction & search PR
		BOOL bWftMove = MoveWaferTable(nCtrWftPosnX + nWftStepXX, nCtrWftPosnY + nWftStepXY);
		Sleep(100);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stDieWftPosn[0].X, &stDieWftPosn[0].Y, &siThetaPos);
		//Update Search die area
		stRunSrchWin.coCorner1.x = (PR_WORD)GetPrCenterX() + stPrStepX.x - wSrchHalfPixelX;
		stRunSrchWin.coCorner1.y = (PR_WORD)GetPrCenterY() + stPrStepX.y - wSrchHalfPixelY;
		stRunSrchWin.coCorner2.x = (PR_WORD)GetPrCenterX() + stPrStepX.x + wSrchHalfPixelX;
		stRunSrchWin.coCorner2.y = (PR_WORD)GetPrCenterY() + stPrStepX.y + wSrchHalfPixelY;
		VerifyPRRegion(&stRunSrchWin);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_GREEN);
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDieWprPosn[0], stRunSrchWin);
//AfxMessageBox("move and draw search windown right");
		Sleep(100);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_TRANSPARENT);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}

		// Move Table X - direction & search PR
		bWftMove = MoveWaferTable(nCtrWftPosnX - nWftStepXX, nCtrWftPosnY - nWftStepXY);
		Sleep(100);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stDieWftPosn[1].X, &stDieWftPosn[1].Y, &siThetaPos);
		//Update Search die area
		stRunSrchWin.coCorner1.x = (PR_WORD)GetPrCenterX() - stPrStepX.x - wSrchHalfPixelX;
		stRunSrchWin.coCorner1.y = (PR_WORD)GetPrCenterY() - stPrStepX.y - wSrchHalfPixelY;
		stRunSrchWin.coCorner2.x = (PR_WORD)GetPrCenterX() - stPrStepX.x + wSrchHalfPixelX;
		stRunSrchWin.coCorner2.y = (PR_WORD)GetPrCenterY() - stPrStepX.y + wSrchHalfPixelY;
		VerifyPRRegion(&stRunSrchWin);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_GREEN);
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDieWprPosn[1], stRunSrchWin);
//AfxMessageBox("move and draw search windown left");
		Sleep(100);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_TRANSPARENT);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_RT_DIE;
			break;
		}

		//Get Both X Value
		stXMotorMove.X	= stDieWftPosn[1].X - stDieWftPosn[0].X;
		stXMotorMove.Y	= stDieWftPosn[1].Y - stDieWftPosn[0].Y;
		stXPixelMove.x	= stDieWprPosn[1].x - stDieWprPosn[0].x;
		stXPixelMove.y	= stDieWprPosn[1].y - stDieWprPosn[0].y;
		szMsg.Format("WFT X    X %d -- %d, Y %d -- %d;		WPR X    X %d -- %d, Y %d -- %d", 
					 stDieWftPosn[1].X, stDieWftPosn[0].X, stDieWftPosn[1].Y, stDieWftPosn[0].Y,
					 stDieWprPosn[1].x, stDieWprPosn[0].x, stDieWprPosn[1].y, stDieWprPosn[0].y);
		SetAlarmLog(szMsg);
		szMsg.Format("Table X Diff: %d,%d; WPR Y Diff: %d,%d", 
					 stXMotorMove.X, stXMotorMove.Y, stXPixelMove.x, stXPixelMove.y);
		SetAlarmLog(szMsg);

		//Move Table Y + direction search PR
		bWftMove = MoveWaferTable(nCtrWftPosnX + nWftStepYX, nCtrWftPosnY + nWftStepYY);
		Sleep(100);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stDieWftPosn[0].X, &stDieWftPosn[0].Y, &siThetaPos);
		//Update Search die area
		stRunSrchWin.coCorner1.x	= (PR_WORD)GetPrCenterX() + stPrStepY.x - wSrchHalfPixelX;
		stRunSrchWin.coCorner1.y	= (PR_WORD)GetPrCenterY() + stPrStepY.y - wSrchHalfPixelY;
		stRunSrchWin.coCorner2.x	= (PR_WORD)GetPrCenterX() + stPrStepY.x + wSrchHalfPixelX;
		stRunSrchWin.coCorner2.y	= (PR_WORD)GetPrCenterY() + stPrStepY.y + wSrchHalfPixelY;
		VerifyPRRegion(&stRunSrchWin);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_GREEN);
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDieWprPosn[0], stRunSrchWin);
//AfxMessageBox("move and draw search windown down");
		Sleep(100);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_TRANSPARENT);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_UP_DIE;
			break;
		}

		//Move Table Y - direction & search PR
		bWftMove = MoveWaferTable(nCtrWftPosnX - nWftStepYX, nCtrWftPosnY - nWftStepYY);
		Sleep(100);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		GetWaferTableEncoder(&stDieWftPosn[1].X, &stDieWftPosn[1].Y, &siThetaPos);
		//Update Search die area
		stRunSrchWin.coCorner1.x	= (PR_WORD)GetPrCenterX() - stPrStepY.x - wSrchHalfPixelX;
		stRunSrchWin.coCorner1.y	= (PR_WORD)GetPrCenterY() - stPrStepY.y - wSrchHalfPixelY;
		stRunSrchWin.coCorner2.x	= (PR_WORD)GetPrCenterX() - stPrStepY.x + wSrchHalfPixelX;
		stRunSrchWin.coCorner2.y	= (PR_WORD)GetPrCenterY() - stPrStepY.y + wSrchHalfPixelY;
		VerifyPRRegion(&stRunSrchWin);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_GREEN);
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDieWprPosn[1], stRunSrchWin);
//AfxMessageBox("move and draw search windown up");
		Sleep(100);
		DrawRectangleBox(stRunSrchWin.coCorner1, stRunSrchWin.coCorner2, PR_COLOR_TRANSPARENT);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_DN_DIE;
			break;
		}

		//Get Both Y Value
		stYMotorMove.X	= stDieWftPosn[1].X - stDieWftPosn[0].X;
		stYMotorMove.Y	= stDieWftPosn[1].Y - stDieWftPosn[0].Y;
		stYPixelMove.x	= stDieWprPosn[1].x - stDieWprPosn[0].x;
		stYPixelMove.y	= stDieWprPosn[1].y - stDieWprPosn[0].y;
		szMsg.Format("WFT Y    X %d -- %d, Y %d -- %d;		WPR Y    X %d -- %d, Y %d -- %d", 
					 stDieWftPosn[1].X, stDieWftPosn[0].X, stDieWftPosn[1].Y, stDieWftPosn[0].Y, 
					 stDieWprPosn[1].x, stDieWprPosn[0].x, stDieWprPosn[1].y, stDieWprPosn[0].y);
		SetAlarmLog(szMsg);
		szMsg.Format("Table Y Diff: %d,%d; WPR Y Diff: %d,%d", 
					 stYMotorMove.X, stYMotorMove.Y, stYPixelMove.x, stYPixelMove.y);
		SetAlarmLog(szMsg);

		// Check for zero error!
		float fTemp = (float)(stXPixelMove.x * stYPixelMove.y - stYPixelMove.x * stXPixelMove.y);
		if (fabs(fTemp) < 0.000001)
		{
			lReturn = WPR_ERR_CALIB_ZERO_VALUE;
			break;
		}

		dCalibX	= (DOUBLE)(stXMotorMove.X * stYPixelMove.y - stYMotorMove.X * stXPixelMove.y) / fTemp;
		if (stYPixelMove.y != 0)
		{
			dCalibXY = ((DOUBLE)stYMotorMove.X - dCalibX * (DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;
		}
		else
		{
			dCalibXY = 0.0;
		}
		dCalibY	= (DOUBLE)(stYMotorMove.Y * stXPixelMove.x - stXMotorMove.Y * stYPixelMove.x) / fTemp;
		if (stXPixelMove.x != 0)
		{
			dCalibYX = ((DOUBLE)stXMotorMove.Y - dCalibY * (DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;
		}
		else
		{
			dCalibYX = 0.0;
		}

		szMsg.Format("Scan Calibration X %.4f,%.4f; Y %.4f,%.4f; factor %.4f", dCalibX, dCalibXY, dCalibY, dCalibYX, fTemp);
		SetAlarmLog(szMsg);

		SetZoomViewCalibration(dCalibX, dCalibY, dCalibXY, dCalibYX);

		PR_RSIZE		stFov;
		PR_OPTIC		stOptic;
		PR_COMMON_RPY	stComRpy;
		stFov.x = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * GetCalibX() * PR_MAX_COORD);
		stFov.y = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * GetCalibY() * PR_MAX_COORD);
		// Set FOV to Wafer PR
		PR_GetOptic(ubPps, ubSID, ubRID, &stOptic, &stComRpy);
		PR_SetFov(&stFov, ubSID, ubRID, &stOptic, &stComRpy);

		//Move table back to start poistion & do PR & update start position
		bWftMove = MoveWaferTable(nCtrWftPosnX, nCtrWftPosnY);
		Sleep(100);
		if (bWftMove == FALSE)
		{
			HmiMessage_Red_Back("Wafer Table move hit limit.", "Die Calibration");
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}
		// search die
		DrawRectangleBox(stCtrSrchWin.coCorner1, stCtrSrchWin.coCorner2, PR_COLOR_GREEN);
		ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDieWprCtrPosn, stCtrSrchWin);
		if (DieIsAlignable(usDieType) == TRUE)
		{
			PR_COORD	stRelMove;

			stRelMove.x = (PR_WORD)GetPrCenterX() - stDieWprCtrPosn.x;
			stRelMove.y = (PR_WORD)GetPrCenterY() - stDieWprCtrPosn.y;

			int	siStepX, siStepY;
			ConvertPixelToMotorStep(stRelMove, &siStepX, &siStepY, dCalibX, dCalibY, dCalibXY, dCalibYX);

			int	siOrigX, siOrigY, siOrigT;
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

			siOrigX += siStepX;
			siOrigY += siStepY;

			MoveWaferTable(siOrigX, siOrigY);
		}
//AfxMessageBox("move and draw search windown center with compensation.");
		Sleep(100);
	}

	MoveWaferTable(nCtrWftPosnX, nCtrWftPosnY);
	Sleep(1000);
	DrawRectangleBox(stCtrSrchWin.coCorner1, stCtrSrchWin.coCorner2, PR_COLOR_GREEN);

	m_stSearchArea = stOrgSearchArea;
	m_bThetaCorrection = bTempCorrection;

	return lReturn;
}
