#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "GallerySearchController.h"
#include "WaferTable.h"
#include "WL_Constant.h"
#include "BondPr.h"
#include "PrZoomSensor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LONG CWaferPr::ToggleZoomViewCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bTo4X = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bTo4X);

	if( IsEnableZoom() )
	{
		if( bTo4X )
		{
			LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X, FALSE, 1, 1);
		}
		else
		{
			LiveViewZoom(GetNmlZoom(), FALSE, 1, 2);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	// Toggle zoom sensor between 4X and learn die

LONG CWaferPr::ToggleAutoZoomViewCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	BOOL bScan = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bScan);

	if( m_qSubOperation==PRESCAN_WAIT_WT_STABLE_Q )
	{
		WPR_ToggleZoom(bScan, 0, 3);
		bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ToggleNml4XCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bTo4X = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bTo4X);

	if( IsEnableZoom() )
	{
		if( bTo4X )
		{
			if( GetRunZoom() != CPrZoomSensorMode::PR_ZOOM_MODE_4X )
			{
				BOOL bOldConZoom = m_bContinuousZoom;
				m_bContinuousZoom = FALSE;
				LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X, FALSE, FALSE, 3);
				m_bContinuousZoom = bOldConZoom;
				HmiMessageEx("Please check die PR image.", "PR Zoom", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, 
					glHMI_MSG_MODAL, NULL, 500, 200, NULL, NULL, NULL, NULL, 100, 100);
			}
		}
		else
		{
			if( GetRunZoom()!=GetNmlZoom() )
			{
				LiveViewZoom(GetNmlZoom(), FALSE, FALSE, 4);
			}
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	// Toggle zoom sensor between 4X and learn die with alarm message


LONG CWaferPr::ToggleLearnColletZoomScreenCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bTo4X = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bTo4X);

	if (IsEnableZoom())
	{
		if (bTo4X)
		{
			LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X, FALSE, FALSE, 5);
		}
		else
		{
			LockLiveViewZoom(svMsg);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bTo4X);
	return ToggleZoomScreenCmd(svMsg);
}	// Toggle zoom screen between 4X and learn die


LONG CWaferPr::ToggleZoomScreenCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bTo4X = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bTo4X);

	if( IsEnableZoom() )
	{
		if( bTo4X )
		{
			WPR_DigitalZoom(PR_ZOOM_FACTOR_4X);
		}
		else
		{
			WPR_DigitalZoom((short)m_lNmlDigitalZoom);	//	toggle
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	// Toggle zoom screen between 4X and learn die

LONG CWaferPr::LiveViewZoomCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bIsUp = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bIsUp);

	// disable Live Zoom
	if ( !IsEnableZoom())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( IsZoomFreezed() )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lCurZoomFactor = GetRunZoom();
	if (m_bCurrentCamera != WPR_CAM_WAFER)
	{
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			lCurZoomFactor = pBondPr->BPR_GetRunZoom();
		}
	}

	if (bIsUp)
	{
		lCurZoomFactor = lCurZoomFactor + 1;

		if (lCurZoomFactor > 3 )
		{
			lCurZoomFactor = 3;
		}
	}
	else
	{
		lCurZoomFactor = lCurZoomFactor - 1;

		if (lCurZoomFactor < 0)
		{
			lCurZoomFactor = 0;
		}

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		BOOL IsBTRealignWithFFMode = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BT_REALIGN_FF_MODE);
		if( pApp->GetFeatureStatus(MS60_VISION_5M_SORTING_FF_MODE)==FALSE )	//	zoomview
		{
			if ((m_bSelectDieType == WPR_NORMAL_DIE /*&& GetPrescanPrID()==3 && m_lCurRefDieNo==3*/) ||
				 (m_bSelectDieType == WPR_REFERENCE_DIE && (m_lCurRefDieNo == 4 || 
				  m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX5 || m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX6 ||
				  m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX7 || m_lCurRefDieNo == WPR_REFERENCE_PR_DIE_INDEX8 ||
				  m_lCurRefDieNo == 9)) ||
				(m_bSelectDieType == WPR_NORMAL_DIE && m_bEnable2ndPrSearch && m_lCurRefDieNo==GetPrescan2ndPrID()) )
			{
			}
			else if (!IsBTRealignWithFFMode || (m_bCurrentCamera == WPR_CAM_WAFER))  //2018.6.28 for Bond FF Mode
			{
				if (lCurZoomFactor < 1)
				{
					lCurZoomFactor = 1;
				}
			}
		}
	}

	if( m_bCurrentCamera == WPR_CAM_WAFER )
	{
		LiveViewZoom(lCurZoomFactor, FALSE, 1, 6);
	}
	else
	{
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			pBondPr->BPR_LiveViewZoom(lCurZoomFactor);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::LiveViewConZoomCmd(IPC_CServiceMessage &svMsg)
{
	// disable Live Zoom
	if ( IsZoomFreezed() || !IsEnableZoom())
	{
		BOOL bReturn  = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( m_bCurrentCamera != WPR_CAM_WAFER )
	{
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			pBondPr->BPR_LiveViewZoom();
		}
	}
	else
	{
		if( IsDP_ScanCam() )
		{
			m_bContinuousZoomDP5M	= m_bContinuousZoomHMI;
			m_dConZoomSubFactorDP5M	= m_dConZoomSubFactorHMI;
			m_dWprZoomRoiShrinkDP5M	= m_dWprZoomRoiShrinkHMI;
		}
		else
		{
			m_bContinuousZoom		= m_bContinuousZoomHMI;
			m_dNmlZoomSubFactor		= m_dConZoomSubFactorHMI;
			m_dWprZoomRoiShrink		= m_dWprZoomRoiShrinkHMI;	//	update by HMI
		}

		CString szTemp;
		szTemp.Format("%.2f", m_dWprZoomRoiShrink/100.0);	//	update registry
		pApp->WriteProfileString(gszPROFILE_SETTING, _T("CP FOV sub cut num(double)"), szTemp);	//	update

		LiveViewZoom(GetRunZoom(), FALSE, 1, 7);
	}

	BOOL bReturn  = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::LiveViewRoiZoomCmd(IPC_CServiceMessage &svMsg)
{
	// disable Live Zoom
	if ( IsZoomFreezed() || !IsEnableZoom())
	{
		BOOL bReturn  = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_bCurrentCamera != WPR_CAM_WAFER )
	{
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			pBondPr->BPR_LiveViewZoom();
		}
	}
	else
	{
		if( IsDP_ScanCam() )
		{
			m_bContinuousZoomDP5M	= m_bContinuousZoomHMI;
			m_dConZoomSubFactorDP5M	= m_dConZoomSubFactorHMI;
			m_dWprZoomRoiShrinkDP5M	= m_dWprZoomRoiShrinkHMI;
		}
		else
		{
			m_bContinuousZoom		= m_bContinuousZoomHMI;
			m_dNmlZoomSubFactor		= m_dConZoomSubFactorHMI;
			m_dWprZoomRoiShrink		= m_dWprZoomRoiShrinkHMI;	//	update by HMI
		}

		CString szTemp;
		szTemp.Format("%.2f", m_dWprZoomRoiShrink/100.0);	//	update registry
		pApp->WriteProfileString(gszPROFILE_SETTING, _T("CP FOV sub cut num(double)"), szTemp);	//	update

		if( GetRunZoom()==GetNmlZoom() )
			WPR_ToggleZoom(TRUE, FALSE, 4);
		else
			LiveViewZoom(GetRunZoom(), FALSE, 1, 8);

		GetScanFovWindow();	//	modify by HMI
	}

	BOOL bReturn  = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::LockLiveViewZoom(IPC_CServiceMessage &svMsg)
{
	BOOL bLock;
	svMsg.GetMsg(sizeof(BOOL), &bLock);

	if( IsEnableZoom() )
	{
		SetZoomViewFixZoom(bLock);
		if( m_bCurrentCamera == WPR_CAM_WAFER )
		{
			WPR_ToggleZoom(FALSE, FALSE, 5);
			DP_GetAllLightings();	
		}
		else
		{
			CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
			if (pBondPr != NULL)
			{
				pBondPr->BPR_LiveViewZoom(pBondPr->BPR_GetNmlZoom());
				pBondPr->BPR_ZoomScreen(0);
			}
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SwitchWaferDualPath(IPC_CServiceMessage &svMsg)
{
	BOOL bLock;
	svMsg.GetMsg(sizeof(BOOL), &bLock);

	if( IsDP() && IsZoomFreezed()==FALSE )
	{
		WPR_ToggleZoom(!m_bRunIsDP_ScanCam, 0, 6);
		DP_GetAllLightings();
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	switch dual path by HMI
