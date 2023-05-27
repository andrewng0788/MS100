/////////////////////////////////////////////////////////////////
// WPR_Common.cpp : Common functions of the CWaferPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wednesday, October 10, 2007
//	Revision:	1.00
//
//	By:			Andrew Ng
//				AAA Product COB Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "MS896A.h"
#include "WaferPr.h"
#include "FileUtil.h"
#include "GallerySearchController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CWaferPr *CIMEvent::m_pWaferPr = NULL;

VOID CIMEvent::SetWaferPr(CWaferPr *pPr)
{
	m_pWaferPr = pPr;
}


PR_VVOID CIMEvent::PR_MouseClick(PR_RFUNC_ON_MOUSE_CLICK_IN const *pstInPar,
								 PR_RFUNC_ON_MOUSE_CLICK_OUT *pstOutPar)
{
	CMS896AStn::m_nMouseMode	= MOUSE_CLICK;
	CMS896AStn::m_dMouseClickX	= pstInPar->rTargetX;
	CMS896AStn::m_dMouseClickY	= pstInPar->rTargetY;
}

// callback function for mouse drag
PR_VVOID CIMEvent::PR_MouseDrag(PR_RFUNC_ON_MOUSE_DRAG_IN const *pstInPar,
								PR_RFUNC_ON_MOUSE_DRAG_OUT *pstOutPar)
{
	CMS896AStn::m_nMouseMode		= MOUSE_DRAG;
	CMS896AStn::m_nMouseDragState	= pstInPar->emMouseState;
	CMS896AStn::m_dMouseDragDist	= pstInPar->rDistance;
	CMS896AStn::m_dMouseDragAngle	= pstInPar->rAngle;
}

//============================================================================================
//

BOOL CWaferPr::PR_NotInit()
{
	return m_bPRInit==FALSE;
}

//================================================================
//   Created-By  : Andrew Ng
//   Date        : 10/10/2007 4:42:29 PM
//   Description :  
//   Remarks     : 
//================================================================
BOOL CWaferPr::PR_Init()
{
	PR_CONNECT_TO_VISION_STRUCT		stConnectToVisionStruct;
	PR_COMMON_RPY					stCommRpy;

	PR_UBYTE	i = 0;
	PR_UBYTE	ubInChNum	= 4 + MAX_PR_GALLERY_SEARCH_THREADS;
	PR_UBYTE	ubOutChNum	= 4 + MAX_PR_GALLERY_SEARCH_THREADS;
#ifdef	ES101
	ubInChNum	= 2 + MAX_PR_GALLERY_SEARCH_THREADS;
	ubOutChNum	= 2 + MAX_PR_GALLERY_SEARCH_THREADS;
#endif
	ubInChNum	= min(ubInChNum,	PR_MAX_COMM_IN_CH);
	ubOutChNum	= min(ubOutChNum,	PR_MAX_COMM_OUT_CH);

	PR_InitConnectToVisionStruct(&stConnectToVisionStruct);
	stConnectToVisionStruct.emIsMasterInit		= PR_TRUE;
	stConnectToVisionStruct.emCommMedia			= PR_COMM_MEDIA_EMSI;
	stConnectToVisionStruct.ubHostID			= (PR_UBYTE) MS_PR_INIT_SEND_ID;
	stConnectToVisionStruct.ubPRID				= (PR_UBYTE) MS_PR_INIT_RECV_ID;
	stConnectToVisionStruct.ulDprBaseAddress	= 0;
	stConnectToVisionStruct.ubNumOfHostInCh		= ubInChNum;
	stConnectToVisionStruct.ubNumOfHostOutCh	= ubOutChNum;

	stConnectToVisionStruct.ubNumOfRPCInCh		= 1;
	stConnectToVisionStruct.ubNumOfRPCOutCh		= 1;

	for (i = 0; i < ubInChNum; i++)
	{
		stConnectToVisionStruct.auwHostInChSize[i]	= 999;
#ifdef	ES101
		stConnectToVisionStruct.auwRPCInChSize[i]	= 999;
#endif		
	}

	for (i = 0; i < ubOutChNum; i++)
	{
		stConnectToVisionStruct.auwHostOutChSize[i]	= 999;
	}

	stConnectToVisionStruct.ulTimeout			= 1000 * 999;	//999 sec

#ifndef OFFLINE
	PR_ConnectToVision(&stConnectToVisionStruct, &stCommRpy);
	if (stCommRpy.uwCommunStatus != PR_COMM_NOERR || stCommRpy.uwPRStatus != PR_ERR_NOERR)
	{
		CString szErr;
		szErr.Format("Init Vision fail; comm = 0x%x, pr = 0x%x", stCommRpy.uwCommunStatus, stCommRpy.uwPRStatus);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
		return FALSE;
	}

	// Hook callback function
	PR_HOOK_RFUNC_ON_MOUSE_CLICK_CMD   stMouseClickCmd;
	PR_COMMON_RPY      stRpy;
	PR_InitHookRFuncOnMouseClickCmd(&stMouseClickCmd);
	stMouseClickCmd.pFunc = CIMEvent::PR_MouseClick;
	PR_HookRFuncOnMouseClickCmd(&stMouseClickCmd, MS_MOUSE_CLICK_SEND_ID, MS_MOUSE_CLICK_RECV_ID, &stRpy);
	if ((stRpy.uwCommunStatus != PR_COMM_NOERR) || (stRpy.uwPRStatus != PR_ERR_NOERR))
	{
		CString szErr;
		szErr.Format("Init Vision Mouse Click callback fcns fail; comm = 0x%x, pr = 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
		return FALSE;
	}

	PR_HOOK_RFUNC_ON_MOUSE_DRAG_CMD stMouseDragCmd;
	PR_InitHookRFuncOnMouseDragCmd(&stMouseDragCmd);
	stMouseDragCmd.pFunc = CIMEvent::PR_MouseDrag;
	PR_HookRFuncOnMouseDragCmd(&stMouseDragCmd, MS_MOUSE_CLICK_SEND_ID, MS_MOUSE_CLICK_RECV_ID, &stRpy);
	if ((stRpy.uwCommunStatus != PR_COMM_NOERR) || (stRpy.uwPRStatus != PR_ERR_NOERR))
	{
		CString szErr;
		szErr.Format("Init Vision Mouse Drag callback fcns fail; comm = 0x%x, pr = 0x%x", stRpy.uwCommunStatus, stRpy.uwPRStatus);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
		return FALSE;
	}
#endif
	return TRUE;
}


BOOL CWaferPr::PR_InitMSSystemPara()
{
	PR_UWORD uwStatus = PR_ERR_NOERR;
	PR_GET_VERSION_NO_RPY stRpy;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();

	// --- Get PR version ---
	PR_GetVersionNoCmd(ubSID, ubRID, &stRpy);
	if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		uwStatus = stRpy.stStatus.uwCommunStatus;
	}
	else if (stRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
	{
		uwStatus = stRpy.stStatus.uwPRStatus;
	}
	if (uwStatus != PR_ERR_NOERR)
	{
#ifdef OFFLINE
			DisplayMessage("Failed to get PR Version");
#else
			AfxMessageBox("Failed to get PR Version");
			return FALSE;
#endif
	}

#ifdef OFFLINE
	m_szSoftVersion.Format("offline");
	m_szSWReleaseNo.Format("offline");
#else
	m_szSoftVersion.Format("%s", stRpy.aubVersionNo);
	m_szSWReleaseNo.Format("%s", stRpy.aubSWReleaseNo);
#endif
	SetGemValue("HP_VisionSWVersion", m_szSoftVersion); // SG_CEID_EQ_STATUS  // 3601
	SetGemValue("HP_VisionSWPartNo", m_szSWReleaseNo); // SG_CEID_EQ_STATUS  // 3601
	(*m_psmfSRam)["MS896A"]["Vision Software Version"] = m_szSoftVersion;	//v4.21T7	//Walsin China for package file list fcn


	// --- Set FOV ---
	PR_RSIZE		szFov;
	PR_OPTIC		stOptic;
	PR_COMMON_RPY	stComRpy;
	szFov.x = 1.0;
	szFov.y = 1.0;


	// --- Set FOV to Wafer PR ---
	PR_GetOptic(ubPpsG, ubSID, ubRID, &stOptic, &stComRpy);
	PR_SetFov(&szFov, ubSID, ubRID, &stOptic, &stComRpy);
	if (stComRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		uwStatus = stComRpy.uwCommunStatus;
	}
	else if (stComRpy.uwPRStatus != PR_ERR_NOERR)
	{
		uwStatus = stComRpy.uwPRStatus;
	}
	if (uwStatus != PR_ERR_NOERR)
	{
#ifdef OFFLINE
		DisplayMessage("Failed to set FOV to Wafer PR");
#else
		AfxMessageBox("Failed to set FOV to Wafer PR");
#endif
	}


	// --- Set FOV to Bond PR ---
	if (m_bDisableBT == FALSE)
	{
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);
		PR_SetFov(&szFov, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);
		if (stComRpy.uwCommunStatus != PR_COMM_NOERR)
		{
			uwStatus = stComRpy.uwCommunStatus;
		}
		else if (stComRpy.uwPRStatus != PR_ERR_NOERR)
		{
			uwStatus = stComRpy.uwPRStatus;
		}
		if (uwStatus != PR_ERR_NOERR)
		{
#ifdef OFFLINE
			DisplayMessage("Failed to set FOV to Bond PR");
#else
			AfxMessageBox("Failed to set FOV to Bond PR");
#endif
		}
	}

	// Get the IM effective view
	IM_GetEffectiveView();

	ChangeCamera(0);		//default camera switch to wafer side after init PR OK & load data
	(*m_psmfSRam)["WaferPr"]["InitPR"] = 1;
	

	// --- Get the general light on Wafer side ---
	WPR_GetGeneralLighting();		
	GetGeneralExposureTime();


	// --- Get the general light on Bond side ---
	int nConvID = 0;
	IPC_CServiceMessage svMsg;
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "GetGeneralLighting", svMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, svMsg);
	

	// --- Get PostSeal lighting from Vision if available ---	//v3.71T5
	TurnOffPSLightings();
	//andrewng6
	SetPostSealGenLighting(FALSE, 6);	
	SetPostSealGenLighting(FALSE, 7);	

	//v4.41T5
	UpdateHmiVariable();
	UpdateSearchDieArea(GetWprDieNo(), GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE);

	return TRUE;
}

INT CWaferPr::IM_GetEffectiveView()
{
	PR_UWORD uwStatus	= PR_ERR_NOERR;
	PR_CAMERA_INFO_CMD	stCmd;
	PR_CAMERA_INFO_RPY	stRpy;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsG = GetRunPurposeG();
	if (IsEnableZoom())
	{
		PR_InitCameraInfoCmd(&stCmd);
		stCmd.emPurpose = (PR_PURPOSE)ubPpsG ;

		PR_CameraInfoCmd(&stCmd, ubSID, ubRID, &stRpy);

		if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			uwStatus = stRpy.stStatus.uwCommunStatus;
		}
		else if (stRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
		{
			uwStatus = stRpy.stStatus.uwPRStatus;
		}

		if (uwStatus != PR_ERR_NOERR)
		{
			#ifdef OFFLINE
			DisplayMessage("Failed to get effective view");
			#else
			AfxMessageBox("Failed to get effective view");
			#endif
		}

		m_stIMEffView = stRpy.stEffView;
	}
	else
	{
		m_stIMEffView.coCorner1.x = PR_MAX_WIN_ULC_X;
		m_stIMEffView.coCorner1.y = PR_MAX_WIN_ULC_Y;
		m_stIMEffView.coCorner2.x = PR_MAX_WIN_LRC_X;
		m_stIMEffView.coCorner2.y = PR_MAX_WIN_LRC_Y;
	}

	m_stWprRoiEffView = m_stIMEffView;	//	PR init to default

	return TRUE;
}

PR_WORD	CWaferPr::GetPRWinULX()
{
	return m_stIMEffView.coCorner1.x;
}

PR_WORD	CWaferPr::GetPRWinULY()
{
	return m_stIMEffView.coCorner1.y;
}

PR_WORD	CWaferPr::GetPRWinLRX()
{
	return m_stIMEffView.coCorner2.x;
}

PR_WORD	CWaferPr::GetPRWinLRY()
{
	return m_stIMEffView.coCorner2.y;
}

INT CWaferPr::IM_SetJoystickOn(CONST BOOL bOn)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	BOOL bJsOn = bOn;
	if (m_bCurrentCamera == WPR_CAM_BOND)	//v3.50T1
	{
		SetBinTableJoystick(bJsOn);
	}
	else
	{
		SetWaferTableJoystick(bJsOn);
	}
	return 0;
}

VOID CWaferPr::IM_ManualDieCompenate(PR_COORD stDieOffset, PR_REAL fDieRotate)
{
	if (m_bCurrentCamera != WPR_CAM_BOND)
	{
		ManualDieCompenate(stDieOffset, fDieRotate);
		return;
	}
	else
	{
		IPC_CServiceMessage stMsg;
		BOOL bResult = TRUE;

		typedef struct 
		{
			LONG lPixelX;
			LONG lPixelY;
		} BPR_MCLICK;
		BPR_MCLICK stInfo;

		stInfo.lPixelX = stDieOffset.x;
		stInfo.lPixelY = stDieOffset.y;
		stMsg.InitMessage(sizeof(stInfo), &stInfo);

		int nConvID = m_comClient.SendRequest(BOND_PR_STN, "ManualCompensate", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}
}


LONG CWaferPr::IMWaferEndChecking(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

VOID CWaferPr::IMGetPRDeviceId()
{
	(*m_psmfSRam)["WaferPr"]["DeviceId"] = "";
}

