#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "GallerySearchController.h"
#include "WaferTable.h"
#include "WL_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LONG CWaferPr::SwitchContourLighting(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		BOOL bWT2;
		BOOL bTurnOn;
	}	WPR_CONTOUR_LIGHT;

	WPR_CONTOUR_LIGHT	stCntuLgt;

	svMsg.GetMsg(sizeof(WPR_CONTOUR_LIGHT), &stCntuLgt);

	SetContourLighting(stCntuLgt.bTurnOn, stCntuLgt.bWT2);

	if (stCntuLgt.bTurnOn)
	{
		PR_UWORD	usCoax	= (PR_UWORD)m_lWL1CoaxLightLevel;
		PR_UWORD	usRing	= (PR_UWORD)m_lWL1RingLightLevel;
		PR_UWORD	usSide	= (PR_UWORD)m_lWL1SideLightLevel;
		PR_UWORD	uwBack	= (PR_UWORD)m_lWL1BackLightLevel;
		if( stCntuLgt.bWT2 )
		{
			usCoax	= (PR_UWORD)m_lWL2CoaxLightLevel;
			usRing	= (PR_UWORD)m_lWL2RingLightLevel;
			usSide	= (PR_UWORD)m_lWL2SideLightLevel;
			uwBack	= (PR_UWORD)m_lWL2BackLightLevel;
		}
		if( usCoax==0 && usRing==0 && usSide==0 && uwBack==0 )
		{
			CString szMsg;
			if( stCntuLgt.bWT2 )
			{
				szMsg = "WT2 Contour lighting is zero, please check.";
			}
			else
			{
				szMsg = "WT1 Contour lighting is zero, please check.";
			}
			HmiMessage_Red_Back(szMsg, "ES101 Contour");
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// --- Use Mouse ---
LONG CWaferPr::WLUsePRMouse(IPC_CServiceMessage &svMsg)
{
	if (m_bUseMouse == FALSE)
	{
		BOOL bWT2 = (BOOL)(m_unCurrPostSealID!=4);
		SetWaferTableJoystick(FALSE, bWT2);

		DrawAndEraseCursor(m_stCalibCornerPos[PR_UPPER_LEFT],  PR_UPPER_LEFT,  1);
		DrawAndEraseCursor(m_stCalibCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);

		DrawAndEraseCursor(m_stCalibCornerPos[PR_UPPER_LEFT],  PR_UPPER_LEFT,  0);
		DrawAndEraseCursor(m_stCalibCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_RED);

		// Set the mouse size first
		m_pPrGeneral->MouseSet2PointRegion(m_stCalibCornerPos[PR_UPPER_LEFT], m_stCalibCornerPos[PR_LOWER_RIGHT], PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_RED);

		m_bUseMouse = TRUE;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WLDrawComplete(IPC_CServiceMessage &svMsg)
{
	if (m_bUseMouse == TRUE)
	{
		BOOL bWT2 = (BOOL)(m_unCurrPostSealID!=4);

		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stCalibCornerPos[PR_UPPER_LEFT],
														   m_stCalibCornerPos[PR_LOWER_RIGHT]);

		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			DrawAndEraseCursor(m_stCalibCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 1);
			DrawAndEraseCursor(m_stCalibCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);
		}

		m_bUseMouse = FALSE;
		DrawRectangleBox(m_stCalibCornerPos[PR_UPPER_LEFT], m_stCalibCornerPos[PR_LOWER_RIGHT], PR_COLOR_GREEN);
		SetWaferTableJoystick(TRUE, bWT2);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WLCancelMouseDraw(IPC_CServiceMessage &svMsg)
{
	if (m_bUseMouse == TRUE)
	{
		BOOL bWT2 = (BOOL)(m_unCurrPostSealID!=4);

		// Erase the mouse object
		m_pPrGeneral->EraseMouseCtrlObj();

		m_bUseMouse = FALSE;
		SetWaferTableJoystick(TRUE, bWT2);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WLMovePRCursor(IPC_CServiceMessage &svMsg)
{
	PR_COORD		sttmpStartPos;
	PR_WORD			sstmpPixelStep;

	unsigned char	uctmpCornerNo;

	typedef struct 
	{
		unsigned char ucDirection;
		unsigned char ucPixelStep;
		unsigned char ucDieCorner;
	} MOVECURSOR;


	MOVECURSOR	stInfo;
	svMsg.GetMsg(sizeof(MOVECURSOR), &stInfo);

	//Get Pixel step move
	switch (stInfo.ucPixelStep)
	{
	case 1:		//10 pixel
		sstmpPixelStep = (PR_WORD)(10 * GetPrScaleFactor());
		break;

	case 2:		//30 pixel
		sstmpPixelStep = (PR_WORD)(30 * GetPrScaleFactor());
		break;
	
	default:	//1 pixel
		sstmpPixelStep = (PR_WORD)(1 * GetPrScaleFactor());
		break;
	}

	//Check current learn corner pos
	switch (stInfo.ucDieCorner)
	{
	case PR_LOWER_RIGHT:	//Lower Right		
		uctmpCornerNo = PR_LOWER_RIGHT;
		break;
	
	default:				//Upper Left
		uctmpCornerNo = PR_UPPER_LEFT;
		break;
	}
	sttmpStartPos = m_stCalibCornerPos[uctmpCornerNo];
	
	if (m_bPRInit == FALSE)
	{
		return 1;
	}

	//Remove Current Cursor
	DrawAndEraseCursor(sttmpStartPos, uctmpCornerNo, 0);

	switch (stInfo.ucDirection)
	{
	case 1:
		sttmpStartPos.x -= sstmpPixelStep;
		break;

	case 2:
		sttmpStartPos.y += sstmpPixelStep;
		break;

	case 3:
		sttmpStartPos.x += sstmpPixelStep;
		break;

	default:
		sttmpStartPos.y -= sstmpPixelStep;
		break;
	}
	m_stCalibCornerPos[uctmpCornerNo] = sttmpStartPos;
	DrawAndEraseCursor(sttmpStartPos, uctmpCornerNo, 1);

	return 1;
}

BOOL CWaferPr::SetWTJoystickLimit(BOOL bMax, CONST BOOL bWT2)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BOOL), &bMax);

 	SetWaferTableJoystick(FALSE, bWT2);
	if( IsESDualWT() && bWT2 )
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetWT2JoystickLimitCmd", stMsg);
	}
	else
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetWT1JoystickLimitCmd", stMsg);
	}

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return TRUE;
}

LONG CWaferPr::WLCalibrateToCorner(IPC_CServiceMessage &svMsg)
{
	BOOL bWT2 = (BOOL)(m_unCurrPostSealID!=4);

	if( bWT2 )
	{
		if( m_lWL2WaferOffsetX!=0 && m_lWL2WaferOffsetY!=0 )
		{
			LONG lCtrX = GetWft2CenterX() + m_lWL2WaferOffsetX;
			LONG lCtrY = GetWft2CenterY() + m_lWL2WaferOffsetY;
			MoveWftNoCheck(lCtrX, lCtrY, FALSE, SFM_WAIT, TRUE);
			Sleep(200);
		}
	}
	else
	{
		if( m_lWL1WaferOffsetX!=0 && m_lWL1WaferOffsetY!=0 )
		{
			LONG lCtrX = GetWft1CenterX() + m_lWL1WaferOffsetX;
			LONG lCtrY = GetWft1CenterY() + m_lWL1WaferOffsetY;
			MoveWftNoCheck(lCtrX, lCtrY, FALSE, SFM_WAIT, FALSE);
			Sleep(200);
		}
	}

	DrawRectangleBox(m_stCalibCornerPos[PR_UPPER_LEFT], m_stCalibCornerPos[PR_LOWER_RIGHT], PR_COLOR_RED);
	SetWTJoystickLimit(TRUE, bWT2);
	SetWaferTableJoystick(TRUE, bWT2);

	HmiMessageEx("Please move the pattern point to PR up - left!", "Contour Camera",
			 glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300, 0, 0, 0, 0, 700, 400);
 	SetWaferTableJoystick(FALSE, bWT2);

	LONG lT = 0;
	GetES101WTEncoder(&m_lContourULX, &m_lContourULY, &lT, bWT2);

	SetWaferTableJoystick(TRUE, bWT2);

	HmiMessageEx("Please move the pattern point to PR low - right!", "Contour Camera",
					 glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300, 0, 0, 0, 0, 700, 400);
    
	SetWTJoystickLimit(FALSE, bWT2);
	GetES101WTEncoder(&m_lContourLRX, &m_lContourLRY, &lT, bWT2);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::WLConfirmCalibrate(IPC_CServiceMessage &svMsg)
{
	LONG lWtULX = m_lContourULX;
	LONG lWtULY = m_lContourULY;
	LONG lWtURX = m_lContourLRX;
	LONG lWtURY = m_lContourULY;
	LONG lWtLRX = m_lContourLRX;
	LONG lWtLRY = m_lContourLRY;

	LONG lPrULX = m_stCalibCornerPos[PR_UPPER_LEFT].x;
	LONG lPrULY = m_stCalibCornerPos[PR_UPPER_LEFT].y;
	LONG lPrURX = m_stCalibCornerPos[PR_LOWER_RIGHT].x;
	LONG lPrURY = m_stCalibCornerPos[PR_UPPER_LEFT].y;
	LONG lPrLRX = m_stCalibCornerPos[PR_LOWER_RIGHT].x;
	LONG lPrLRY = m_stCalibCornerPos[PR_LOWER_RIGHT].y;

CString szMsg;
szMsg.Format("table UL %d,%d UR %d,%d LR %d,%d", lWtULX, lWtULY, lWtURX, lWtURY, lWtLRX, lWtLRY);
SetAlarmLog(szMsg);
szMsg.Format("pixel UL %d,%d UR %d,%d LR %d,%d", lPrULX, lPrULY, lPrURX, lPrURY, lPrLRX, lPrLRY);
SetAlarmLog(szMsg);
	//Get Both X Value	// UR to UL
	DOUBLE dXMoveX	= lWtULX - lWtURX;
	DOUBLE dXMoveY	= lWtULY - lWtURY;
	DOUBLE dYMoveX	= lWtURX - lWtLRX;
	DOUBLE dYMoveY	= lWtURY - lWtLRY;

	//Get Both Y Value	// LR to UP
	DOUBLE dXPixelX	= lPrURX - lPrULX;
	DOUBLE dXPixelY	= lPrURY - lPrULY;
	DOUBLE dYPixelX	= lPrLRX - lPrURX;
	DOUBLE dYPixelY	= lPrLRY - lPrURY;


	// Check for zero error!
	DOUBLE dTemp = (dXPixelX * dYPixelY - dYPixelX * dXPixelY);
	if (fabs(dTemp) >= 0.000001)
	{
		DOUBLE dCalibXX = 0, dCalibXY = 0, dCalibYY = 0, dCalibYX = 0;
		dCalibXX	= (dXMoveX * dYPixelY - dYMoveX * dXPixelY) / dTemp;
		if (dYPixelY != 0)
		{
			dCalibXY = (dYMoveX - dCalibXX * dYPixelX) / dYPixelY;
		}
		else
		{
			dCalibXY = 0.0;
		}

		dCalibYY	= (dYMoveY * dXPixelX - dXMoveY * dYPixelX) / dTemp;
		if (dXPixelX != 0)
		{
			dCalibYX = (dXMoveY - dCalibYY * dXPixelY) / dXPixelX;
		}
		else
		{
			dCalibYX = 0.0;
		}

		if( m_unCurrPostSealID==4 )
		{
			m_bWL1Calibrated	= TRUE;
			m_dWL1CalibXX		= dCalibXX;
			m_dWL1CalibXY		= dCalibXY;
			m_dWL1CalibYY		= dCalibYY;
			m_dWL1CalibYX		= dCalibYX;
szMsg.Format("WL1 Calibrate %f,%f;  %f,%f", dCalibXX, dCalibXY, dCalibYY, dCalibYX);
		}
		else
		{
			m_bWL2Calibrated	= TRUE;
			m_dWL2CalibXX		= dCalibXX;
			m_dWL2CalibXY		= dCalibXY;
			m_dWL2CalibYY		= dCalibYY;
			m_dWL2CalibYX		= dCalibYX;
szMsg.Format("WL2 Calibrate %f,%f;  %f,%f", dCalibXX, dCalibXY, dCalibYY, dCalibYX);
		}
		SavePrData(FALSE);
SetAlarmLog(szMsg);
	}
	else
	{
		HmiMessage("Contoure calibration fail, please check!", "Contoure Camera");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WLTeachCameraOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bWT2 = (BOOL)(m_unCurrPostSealID!=4);

	if( bWT2 )
	{
		if( m_lWL2WaferOffsetX!=0 && m_lWL2WaferOffsetY!=0 )
		{
			LONG lCtrX = GetWft2CenterX() + m_lWL2WaferOffsetX;
			LONG lCtrY = GetWft2CenterY() + m_lWL2WaferOffsetY;
			MoveWftNoCheck(lCtrX, lCtrY, FALSE, SFM_WAIT, TRUE);
			Sleep(200);
		}
	}
	else
	{
		if( m_lWL1WaferOffsetX!=0 && m_lWL1WaferOffsetY!=0 )
		{
			LONG lCtrX = GetWft1CenterX() + m_lWL1WaferOffsetX;
			LONG lCtrY = GetWft1CenterY() + m_lWL1WaferOffsetY;
			MoveWftNoCheck(lCtrX, lCtrY, FALSE, SFM_WAIT, FALSE);
			Sleep(200);
		}
	}

	SetWTJoystickLimit(TRUE, bWT2);
	SetWaferTableJoystick(TRUE, bWT2);

	HmiMessageEx("Please locate the contour pattern center!", "Contoure Camera",
					 glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300, 0, 0, 0, 0, 700, 400);
	SetWaferTableJoystick(FALSE, bWT2);
	LONG lT = 0;
	LONG lContourCtrX = 0, lContourCtrY = 0, lWaferCtrX = 0, lWaferCtrY = 0;
	GetES101WTEncoder(&lContourCtrX, &lContourCtrY, &lT, bWT2);
	LONG lXAxis = 0, lYAxis = 0;
	if( bWT2 )
	{
		GetWT2HomeDiePhyPosn(lXAxis, lYAxis);
	}
	else
	{
		GetHomeDiePhyPosn(lXAxis, lYAxis);
	}

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG nMode;
	} WT_XY_HOME_LOAD_STRUCT;
 	WT_XY_HOME_LOAD_STRUCT stEsPos;

	stEsPos.lX	= lXAxis;
	stEsPos.lY	= lYAxis;
	stEsPos.nMode = SFM_WAIT;

	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	stMsg.InitMessage(sizeof(WT_XY_HOME_LOAD_STRUCT), &stEsPos);
	if (bWT2)
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY2_MoveToHomeLoadCmd",	stMsg);
	else
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY1_MoveToHomeLoadCmd",	stMsg);

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}	
	}

	SelectWaferCamera(stMsg);
	SetWaferTableJoystick(TRUE, bWT2);

	GotoFocusLevel(stMsg);
	MoveBackLightWithTableCheck(TRUE);

	HmiMessageEx("Please locate the wafer pattern center!", "Contoure Camera",
					 glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300, 0, 0, 0, 0, 700, 400);

	SetWaferTableJoystick(FALSE, bWT2);
	MoveBackLightWithTableCheck(FALSE);

	GetES101WTEncoder(&lWaferCtrX, &lWaferCtrY, &lT, bWT2);

	SetWTJoystickLimit(FALSE, bWT2);

	if( bWT2 )
	{
		m_lWL2WaferOffsetX = lContourCtrX - lWaferCtrX;
		m_lWL2WaferOffsetY = lContourCtrY - lWaferCtrY;
CString szMsg;
szMsg.Format("WL2 camera offset %d,%d", m_lWL2WaferOffsetX, m_lWL2WaferOffsetY);
SetAlarmLog(szMsg);
	}
	else
	{
		m_lWL1WaferOffsetX = lContourCtrX - lWaferCtrX;
		m_lWL1WaferOffsetY = lContourCtrY - lWaferCtrY;
CString szMsg;
szMsg.Format("WL1 camera offset %d,%d", m_lWL1WaferOffsetX, m_lWL1WaferOffsetY);
SetAlarmLog(szMsg);
	}

	SavePrData(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// Host side software support list

BOOL CWaferPr::SetContourLighting(CONST BOOL bOn, CONST BOOL bWT2)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA))
	{
		return FALSE;
	}

	PR_UWORD		uwBack	= 0;
	PR_UWORD		usCoax	= 0;
	PR_UWORD		usRing	= 0;
	PR_UWORD		usSide	= 0;
	if (bOn)
	{
		usCoax	= (PR_UWORD)m_lWL1CoaxLightLevel;
		usRing	= (PR_UWORD)m_lWL1RingLightLevel;
		usSide	= (PR_UWORD)m_lWL1SideLightLevel;
		uwBack	= (PR_UWORD)m_lWL1BackLightLevel;
		if( bWT2 )
		{
			usCoax	= (PR_UWORD)m_lWL2CoaxLightLevel;
			usRing	= (PR_UWORD)m_lWL2RingLightLevel;
			usSide	= (PR_UWORD)m_lWL2SideLightLevel;
			uwBack	= (PR_UWORD)m_lWL2BackLightLevel;
		}
	}

	PR_UBYTE	ubSendID	= ES_CONTOUR_CAM_SEND_ID;
	PR_UBYTE	ubRecvID	= ES_CONTOUR_CAM_RECV_ID;
	PR_PURPOSE	ubGnPurpose	= ES_CONTOUR_WL1_GN_PURPOSE;
	PR_PURPOSE	ubPrPurPose = ES_CONTOUR_WL1_PR_PURPOSE;
	if( bWT2 )
	{
		ubGnPurpose	= ES_CONTOUR_WL2_GN_PURPOSE;
		ubPrPurPose = ES_CONTOUR_WL2_PR_PURPOSE;
	}

	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stGnOptic, stPrOptic;

	PR_SOURCE emBackID = PR_BACK_LIGHT;
	PR_SOURCE emCoaxID = PR_COAXIAL_LIGHT;
	PR_SOURCE emRingID = PR_RING_LIGHT;
	PR_SOURCE emSideID = PR_SIDE_LIGHT;

	PR_GetOptic(ubGnPurpose, ubSendID, ubRecvID, &stGnOptic, &stRpy);	
	PR_SetLighting(emCoaxID, usCoax, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	PR_SetLighting(emRingID, usRing, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	PR_SetLighting(emSideID, usSide, ubSendID, ubRecvID, &stGnOptic, &stRpy);
	PR_SetLighting(emBackID, uwBack, ubSendID, ubRecvID, &stGnOptic, &stRpy);

	if (bOn)	//Only update PR lighting if bOn = TRUE
	{
		PR_GetOptic(ubPrPurPose, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emCoaxID, usCoax, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emRingID, usRing, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emSideID, usSide, ubSendID, ubRecvID, &stPrOptic, &stRpy);
		PR_SetLighting(emBackID, uwBack, ubSendID, ubRecvID, &stPrOptic, &stRpy);
	}

	return TRUE;
}
		

BOOL CWaferPr::TurnOffPSLightings()
{
	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if (pApp->GetFeatureStatus(MS896A_FUNC_VISION_POSTSEAL_OPTICS) &&
	//	!pApp->GetFeatureStatus(ES_FUNC_VISION_WL_CONTOUR_CAMERA))
	//{
	//	SetPostSealLighting(FALSE, 1);
	//}
	//if (pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS))
	//{
	//	//4.52D17light off
	//	if(!pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR))
	//	{
	//		SetPostSealLighting(FALSE, 2);
	//		SetPostSealLighting(FALSE, 3);
	//	}
	//}
	SetPostSealGenLighting(FALSE, 6);	
	SetPostSealGenLighting(FALSE, 7);	
	return TRUE;
}

VOID CWaferPr::MoveWaferThetaTo(LONG lTheta, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(LONG), &lTheta);

	// Get the reply for the encoder value
	if( IsESDualWT() )
	{
		if( bWT2 )
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T2_MoveToCmd", stMsg);
		else
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T1_MoveToCmd", stMsg);
	}
	else
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T_MoveToCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}	
	}
}


