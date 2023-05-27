#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "GallerySearchController.h"
#include "WAF_CPhysicalInformation.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"
#include "LastScanPosition.h"
#include "TakeTime.h"
#include "WT_SubRegion.h"
#include "MS896A.h"
#include "WaferTable.h"
#include "WL_Constant.h"
#include "BondPr.h"
#include "PrZoomSensor.h"

PR_WIN CWaferPr::GetSrchArea()
{
	PR_WIN stWin = m_stSearchArea;
	//	Dual Path
	if( GetRunZoom()!=GetNmlZoom() && GetRunZoom()==GetScnZoom() )
	{
		stWin.coCorner1.x = (PR_WORD)_round((stWin.coCorner1.x - GetPrCenterX()) / 4 + GetPrCenterX());
		stWin.coCorner1.y = (PR_WORD)_round((stWin.coCorner1.y - GetPrCenterY()) / 4 + GetPrCenterY());
		stWin.coCorner2.x = (PR_WORD)_round((stWin.coCorner2.x - GetPrCenterX()) / 4 + GetPrCenterX());
		stWin.coCorner2.y = (PR_WORD)_round((stWin.coCorner2.y - GetPrCenterY()) / 4 + GetPrCenterY());
	}

	return stWin;
}

PR_WORD CWaferPr::GetScanNmlSizePixelX()
{
	return GetScanDieSizePixelX(GetPrescanPrID()-1);
}

PR_WORD CWaferPr::GetScanNmlSizePixelY()
{
	return GetScanDieSizePixelY(GetPrescanPrID()-1);
}

PR_WORD CWaferPr::GetScanDieSizePixelX(UCHAR ucDieNo)
{
	UCHAR ucIndex = ucDieNo;
	ucIndex = min(ucIndex, WPR_MAX_DIE-1);
	return m_stGenDieSize[ucIndex].x;
}

PR_WORD CWaferPr::GetScanDieSizePixelY(UCHAR ucDieNo)
{
	UCHAR ucIndex = ucDieNo;
	ucIndex = min(ucIndex, WPR_MAX_DIE-1);
	return m_stGenDieSize[ucIndex].y;
}

int  CWaferPr::GetScanPitchPixelXX()
{
	int nPitchPixel = GetPitchPixelXX();
	if( IsEnableZoom() )
	{
		nPitchPixel = abs((int)((DOUBLE)m_siDiePitchXX / GetScanCalibX()));
	}

	return nPitchPixel;
}

int	 CWaferPr::GetScanPitchPixelYY()
{
	int nPitchPixel = GetPitchPixelYY();
	if( IsEnableZoom() )
	{
		nPitchPixel = abs((int)((DOUBLE)m_siDiePitchYY / GetScanCalibY()));
	}

	return nPitchPixel;
}

DOUBLE CWaferPr::GetScanCalibX()
{
	DOUBLE dCalib = GetCalibX();
	if( IsEnableZoom() )
	{
		dCalib = m_stZoomView.m_dScanCalibXX;
	}

	return dCalib;
}

DOUBLE CWaferPr::GetScanCalibY()
{
	DOUBLE dCalib = GetCalibY();
	if( IsEnableZoom() )
	{
		dCalib = m_stZoomView.m_dScanCalibYY;
	}

	return dCalib;
}

DOUBLE CWaferPr::GetScanCalibXY()
{
	DOUBLE dCalib = GetCalibXY();
	if( IsEnableZoom() )
	{
		dCalib = m_stZoomView.m_dScanCalibXY;
	}

	return dCalib;
}

DOUBLE CWaferPr::GetScanCalibYX()
{
	DOUBLE dCalib = GetCalibYX();
	if( IsEnableZoom() )
	{
		dCalib = m_stZoomView.m_dScanCalibYX;
	}

	return dCalib;
}

DOUBLE CWaferPr::ConvertScanDUnitToPixel(CONST DOUBLE dUnit)
{
	DOUBLE dMotorStep	= 0.0;
	if (m_dWTXinUm != 0)
	{
		dMotorStep = dUnit / m_dWTXinUm / 1000.0 * 25.4;    //v4.28T5	//Used in bLineDefect in AUTOBOND whihc may slower down CT!!
	}		
	
	if (GetScanCalibX() != 0.0)	// divide by zero
	{
		return  dMotorStep / GetScanCalibX();
	}
	if (GetScanCalibY() != 0.0)	// divide by zero
	{
		return  dMotorStep / GetScanCalibY();
	}

	return 0;
}

VOID CWaferPr::WPR_DigitalZoom(short hZoomFactor)
{
	m_lRunDigitalZoom = hZoomFactor;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	m_pPrGeneral->DigitalZoomScreen(hZoomFactor, (PR_WORD)GetPrCenterX(), (PR_WORD)GetPrCenterY(), ubSID, ubRID);
}


LONG CWaferPr::ConfirmDigitalZoom(IPC_CServiceMessage& svMsg)
{
	LONG lZoomScreenFactor = 0;
	svMsg.GetMsg(sizeof(LONG), &lZoomScreenFactor);

	m_lGrabDigitalZoom = lZoomScreenFactor;
	CString szMsg;
	szMsg.Format("zoom digital %d", m_lGrabDigitalZoom);
	HmiMessage(szMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ZoomScreenCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bIsUp = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bIsUp);

	// disable Live Zoom
	if ( IsZoomFreezed() || !IsEnableZoom())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lZoomScreen = GetRunDigitalZoom();
	if (m_bCurrentCamera != WPR_CAM_WAFER)
	{
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			lZoomScreen = pBondPr->BPR_GetZoomDigital();
		}
	}

	if (bIsUp)
	{
		lZoomScreen = lZoomScreen + 1;

		if (lZoomScreen > PR_ZOOM_FACTOR_4X)
		{
			lZoomScreen = PR_ZOOM_FACTOR_4X;
		}
	}
	else
	{
		lZoomScreen = lZoomScreen - 1;

		if( GetRunZoom()==0 || IsMS90() )
		{
			if (lZoomScreen < 0-PR_ZOOM_FACTOR_8X)
			{
				lZoomScreen = 0-PR_ZOOM_FACTOR_8X;
			}
		}
		else
		{
			if (lZoomScreen <= PR_ZOOM_FACTOR_1X)
			{
				lZoomScreen = PR_ZOOM_FACTOR_1X;
			}
		}
	}

	if( m_bCurrentCamera == WPR_CAM_WAFER )
	{
		WPR_DigitalZoom((short)lZoomScreen);
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( GetRunZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF && pApp->GetFeatureStatus(MS60_VISION_5M_SORTING_FF_MODE) )
		{
			m_lNmlDigitalZoom	= GetRunDigitalZoom();
		}
		else
		{
			if( IsMS90() )
				m_lNmlDigitalZoom	= GetRunDigitalZoom();
			else
				m_lNmlDigitalZoom	= PR_ZOOM_FACTOR_1X;
		}
		SavePrData(FALSE);
	}
	else
	{
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			pBondPr->BPR_ZoomScreen((short)lZoomScreen);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

