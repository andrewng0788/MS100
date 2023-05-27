/////////////////////////////////////////////////////////////////
// WPR_Common_Ejt.cpp : Common functions of the CWaferPr class for EJT
//
//	Description:
//		MS100 Mapping Die Sorter
//
//	Date:		Tuesday, Sept 24, 2013
//	Revision:	1.00
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2004-2013.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "MS896A.h"
#include "WaferPr.h"
#include "FileUtil.h"
#include "prGeneral.h"
#include "BinTable.h"
#include "BondPr.h"
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	EJT_CAP_CALIBRATION_PR_ID		10
#define	EJT_CAP_FFCALIBRATION_PR_ID		9


VOID CWaferPr::RegisterEjtVariables()
{
	try
	{
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDieWithEjt"),		&CWaferPr::UserSearchDieWithEjt);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie_Ejt"),		&CWaferPr::UserSearchDie_Ejt);	
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnEjtColletOffsetXY"),	&CWaferPr::LearnEjtColletOffsetXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmEjtColletOffsetXY"),	&CWaferPr::ConfirmEjtColletOffsetXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelEjtColletOffsetXY"),	&CWaferPr::CancelEjtColletOffsetXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchColletHoleEpoxyPattern"),		&CWaferPr::SearchColletHoleEpoxyPattern);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoSearchColletHole"),		&CWaferPr::AutoSearchColletHole);			//v4.43T5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SearchColletHole"),			&CWaferPr::SearchColletHole);			//v4.47A5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ShowEjtColletOffsetXY"),	&CWaferPr::ShowEjtColletOffsetXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LearnEjtXYCalibration"),	&CWaferPr::LearnEjtXYCalibration);	

		// Move Collet Center
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayEjtColletOffsetCenter1"),	&CWaferPr::DisplayEjtColletOffsetCenter1);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DisplayEjtColletOffsetCenter2"),	&CWaferPr::DisplayEjtColletOffsetCenter2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WPR_EjtDisplayCollet_Move_Up"),		&CWaferPr::WPR_EjtDisplayCollet_Move_Up);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WPR_EjtDisplayCollet_Move_Down"),	&CWaferPr::WPR_EjtDisplayCollet_Move_Down);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WPR_EjtDisplayCollet_Move_Left"),	&CWaferPr::WPR_EjtDisplayCollet_Move_Left);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WPR_EjtDisplayCollet_Move_Right"),	&CWaferPr::WPR_EjtDisplayCollet_Move_Right);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WPR_ResetEjtCenter"),				&CWaferPr::WPR_ResetEjtCenter);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WPR_ConfirmEjtSetUp"),				&CWaferPr::WPR_ConfirmEjtSetUp);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableEjtColletOffset"),			&CWaferPr::EnableEjtColletOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WPR_UpdateAGCOffset"),				&CWaferPr::WPR_UpdateAGCOffset);

	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}


LONG CWaferPr::UserSearchDieWithEjt(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	BOOL			bNoDieFound = TRUE;
	PR_WORD			rtnPR;
	BOOL			bIsArm2 = FALSE;
	int nOffsetXInMotorStep	= 0;
	int nOffsetYInMotorStep	= 0;

	PR_COORD	stSrchCorner1 = m_stSearchArea.coCorner1;
	PR_COORD	stSrchCorner2 = m_stSearchArea.coCorner2;
	if (State() == IDLE_Q)
	{
		stSrchCorner1.x = GetPRWinULX();
		stSrchCorner1.y = GetPRWinULY();
		stSrchCorner2.x = GetPRWinLRX()-1;
		stSrchCorner2.y = GetPRWinLRY()-1;
	}

	if (m_bPRInit == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	svMsg.GetMsg(sizeof(BOOL), &bIsArm2);		//v4.44A1	//Semitek

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	rtnPR = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_FALSE, 
								&usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
								stSrchCorner1, stSrchCorner2);
	
	if ( (rtnPR != -1) && DieIsAlignable(usDieType) )
	{
		if (CMS896AStn::m_bMS100EjtXY)
		{
			CalculateDieCompenate(stDieOffset, &nOffsetXInMotorStep, &nOffsetYInMotorStep);
		
			if (bIsArm2 == FALSE)
			{
				nOffsetXInMotorStep = nOffsetXInMotorStep + m_lEjtCollet1OffsetX;
				nOffsetYInMotorStep = nOffsetYInMotorStep + m_lEjtCollet1OffsetY;
			}
			else
			{
				nOffsetXInMotorStep = nOffsetXInMotorStep + m_lEjtCollet2OffsetX;
				nOffsetYInMotorStep = nOffsetYInMotorStep + m_lEjtCollet2OffsetY;
			}

			int siOrigX=0, siOrigY=0, siOrigT=0;
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			siOrigX += nOffsetXInMotorStep;
			siOrigY += nOffsetYInMotorStep;
			MoveWaferTable(siOrigX, siOrigY);
			Sleep(500);
		}
		else
		{
			ManualDieCompenate(stDieOffset, fDieRotate);
		}
	}
	else
	{
		CString szMsg;

		if (bIsArm2)		//BHZ2
		{
			szMsg = "Please move a die to Collet 2 cursor center to continue.";
			m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
		}
		else
		{
			szMsg = "Please move a die to Collet 1 cursor center to continue.";
			m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
		}

		SetWaferTableJoystick(TRUE);
		HmiMessageEx(szMsg, "Search Die", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 
			36000000, glHMI_MSG_MODAL, 0, 400, 300);
		SetWaferTableJoystick(FALSE);
	}

	ChangeCamera(WPR_CAM_WAFER);	//Clear Screen

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::UserSearchDie_Ejt(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	BOOL			bNoDieFound = TRUE;

	if (m_bPRInit == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	//v4.52A14
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14
	if (bColletOffsetWoEjtXY)
	{
		//No need to Search because EjtXY is NOT available in the config
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	DrawSearchBox(PR_COLOR_GREEN);
	PR_WORD rtnPR;

	PR_WIN stSrchArea;
	//GetSearchDieArea(&stSrchArea, WPR_NORMAL_DIE+1);
	stSrchArea.coCorner1.x = GetPRWinULX();
	stSrchArea.coCorner1.y = GetPRWinULY();
	stSrchArea.coCorner2.x = GetPRWinLRX();
	stSrchArea.coCorner2.y = GetPRWinLRY();

	rtnPR = ManualSearchDie(WPR_REFERENCE_DIE, EJT_CAP_CALIBRATION_PR_ID, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2);
	
	if (rtnPR != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
//AfxMessageBox("ALIGN", MB_SYSTEMMODAL);
			bNoDieFound = FALSE;
			bReturn = TRUE;
			
			//Allow die to rotate
			ManualEjtDieCompenate(stDieOffset);
			DrawSearchBox( PR_COLOR_TRANSPARENT);
		}
		else
		{
//AfxMessageBox("ALIGN fails", MB_SYSTEMMODAL);
			bReturn = FALSE;
		}

		//Display Message on HMI
		DisplaySearchDieResult(usDieType, WPR_NORMAL_DIE, WPR_NORMAL_DIE+1, fDieRotate, stDieOffset, fDieScore);
	}
	else
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
	}

	//On Joystick & Clear PR screen
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

VOID CWaferPr::InitCenter()
{
	m_lEjtCollet1OffsetX	= GetEjtCollet1OffsetX();
	m_lEjtCollet1OffsetY	= GetEjtCollet1OffsetY();
	m_lEjtCollet2OffsetX	= GetEjtCollet2OffsetX();
	m_lEjtCollet2OffsetY	= GetEjtCollet2OffsetY();

	m_lChangeColletTempCenter1X = m_lEjtCollet1CoorX;
	m_lChangeColletTempCenter1Y = m_lEjtCollet1CoorY;
	m_lChangeColletTempCenter2X = m_lEjtCollet2CoorX;
	m_lChangeColletTempCenter2Y = m_lEjtCollet2CoorY;
}

LONG CWaferPr::DisplayEjtColletOffsetCenter1(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	InitCenter();
	m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX,m_lEjtCollet1CoorY);
	
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CWaferPr::DisplayEjtColletOffsetCenter2(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	InitCenter();
	m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX,m_lEjtCollet2CoorY);
		
	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;
}

LONG CWaferPr::WPR_EjtDisplayCollet_Move_Up(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	PR_COORD prd;
	if (bColletOffsetWoEjtXY)	//v4.52A14
	{
		prd.x = 0;
		prd.y = (PR_WORD)stInfo.lStep;
	}
	else
	{
		ConvertMotorStepToPixel(0, stInfo.lStep, prd);
	}

	//v4.44A1	//SEmitek
	LONG lPitchX = GetDiePitchX_X();
	LONG lPitchY = GetDiePitchY_Y();

	//v4.43T9
	LONG lHalfSrchWndX = _round((GetSrchDieAreaX() * 1.0 / 4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY = _round((GetSrchDieAreaY() * 1.0 / 4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lSizeX = PR_DEF_CENTRE_X - lPitchX - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - lPitchY - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * lPitchY - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * lPitchY - lHalfSrchWndY;
		}
	}
	//v4.46T9
	if (lSizeX < 1700)
	{
		lSizeX = 1700;
	}
	if (lSizeY < 1700)
	{
		lSizeY = 1700;
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}


	PR_COORD coULCorner;
	PR_COORD coLRCorner;
	coULCorner.x = (PR_WORD)(GetPrCenterX() - lSizeX);
	coULCorner.y = (PR_WORD)(GetPrCenterY() - lSizeY);
	coLRCorner.x = (PR_WORD)(GetPrCenterX() + lSizeX);
	coLRCorner.y = (PR_WORD)(GetPrCenterY() + lSizeY);
	DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_RED);

	if ( m_bChangeCollet1 )
	{
		m_lEjtCollet1CoorY -= prd.y;

		if (m_lEjtCollet1CoorY < coULCorner.y)
		{
			m_lEjtCollet1CoorY = coULCorner.y;
		}

		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
	}
	else if ( m_bChangeCollet2 )
	{
		m_lEjtCollet2CoorY -= prd.y;

		if (m_lEjtCollet2CoorY < coULCorner.y)
		{
			m_lEjtCollet2CoorY = coULCorner.y;
		}

		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
	}
	
	int siStepX, siStepY;
	ConvertPixelToMotorStep(prd, &siStepX, &siStepY);

	int	siDestinationX, siDestinationY, siOrigX, siOrigY;
	GetEjtXYEncoder(&siOrigX, &siOrigY);
	siDestinationX = siOrigX - siStepX;
	siDestinationY = siOrigY - siStepY;

	MoveEjectorTable(siDestinationX, siDestinationY);

	//v4.46T9
	CString szMsg;
	szMsg.Format("EjtXY crosshair (%d %d) UP: cursor(%ld, %ld); UL(%ld, %ld); LR(%ld, %ld); Size(%ld, %ld); pitch(%ld, %ld), 1/2SrchWnd(%ld, %ld)", 
					m_bChangeCollet1, m_bChangeCollet2, m_lEjtCollet2CoorX, m_lEjtCollet2CoorY,
					coULCorner.x, coULCorner.y, 
					coLRCorner.x, coLRCorner.y,
					lSizeX, lSizeY, lPitchX, lPitchY, lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;	
}

LONG CWaferPr::WPR_EjtDisplayCollet_Move_Down(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	PR_COORD prd;
	if (bColletOffsetWoEjtXY)		//v4.52A14
	{
		prd.x = 0;
		prd.y = (PR_WORD)stInfo.lStep;
	}
	else
	{
		ConvertMotorStepToPixel(0, stInfo.lStep, prd);
	}

	//v4.44A1	//SEmitek
	LONG lPitchX = GetDiePitchX_X();
	LONG lPitchY = GetDiePitchY_Y();

	//v4.43T9
	LONG lHalfSrchWndX = _round((GetSrchDieAreaX() * 1.0 / 4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY = _round((GetSrchDieAreaY() * 1.0 / 4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lSizeX = PR_DEF_CENTRE_X - lPitchX - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - lPitchY - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * lPitchY - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * lPitchY - lHalfSrchWndY;
		}
	}
	//v4.46T9
	if (lSizeX < 1700)
	{
		lSizeX = 1700;
	}
	if (lSizeY < 1700)
	{
		lSizeY = 1700;
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}


	PR_COORD coULCorner;
	PR_COORD coLRCorner;
	coULCorner.x = (PR_WORD)(GetPrCenterX() - lSizeX);
	coULCorner.y = (PR_WORD)(GetPrCenterY() - lSizeY);
	coLRCorner.x = (PR_WORD)(GetPrCenterX() + lSizeX);
	coLRCorner.y = (PR_WORD)(GetPrCenterY() + lSizeY);
	DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_RED);

	if ( m_bChangeCollet1 )
	{
		m_lEjtCollet1CoorY += prd.y;
		if (m_lEjtCollet1CoorY > coLRCorner.y)
		{
			m_lEjtCollet1CoorY = coLRCorner.y;
		}
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
	}
	else if ( m_bChangeCollet2 )
	{
		m_lEjtCollet2CoorY += prd.y;
		if (m_lEjtCollet2CoorY > coLRCorner.y)
		{
			m_lEjtCollet2CoorY = coLRCorner.y;
		}
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
	}
		
	int siStepX, siStepY;
	ConvertPixelToMotorStep(prd, &siStepX, &siStepY);

	int	siDestinationX, siDestinationY, siOrigX, siOrigY;
	GetEjtXYEncoder(&siOrigX, &siOrigY);
	siDestinationX = siOrigX + siStepX;
	siDestinationY = siOrigY + siStepY;

	MoveEjectorTable(siDestinationX, siDestinationY);

	//v4.46T9
	CString szMsg;
	szMsg.Format("EjtXY crosshair (%d %d) DN: cursor(%ld, %ld); UL(%ld, %ld); LR(%ld, %ld); Size(%ld, %ld); pitch(%ld, %ld), 1/2SrchWnd(%ld, %ld)", 
					m_bChangeCollet1, m_bChangeCollet2, m_lEjtCollet2CoorX, m_lEjtCollet2CoorY,
					coULCorner.x, coULCorner.y, 
					coLRCorner.x, coLRCorner.y,
					lSizeX, lSizeY, lPitchX, lPitchY, lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;	
}

LONG CWaferPr::WPR_EjtDisplayCollet_Move_Left(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	PR_COORD prd;
	if (bColletOffsetWoEjtXY)		//v4.52A14
	{
		prd.x = (PR_WORD)stInfo.lStep;
		prd.y = 0;
	}
	else
	{
		ConvertMotorStepToPixel(stInfo.lStep, 0, prd);
	}

	//v4.44A1	//SEmitek
	LONG lPitchX = GetDiePitchX_X();
	LONG lPitchY = GetDiePitchY_Y();

	//v4.43T9
	LONG lHalfSrchWndX = (PR_WORD)_round((GetSrchDieAreaX() * 1.0 / 4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY = (PR_WORD)_round((GetSrchDieAreaY() * 1.0 / 4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lSizeX = PR_DEF_CENTRE_X - lPitchX - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - lPitchY - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * lPitchY - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * lPitchY - lHalfSrchWndY;
		}
	}
	//v4.46T9
	if (lSizeX < 1700)
	{
		lSizeX = 1700;
	}
	if (lSizeY < 1700)
	{
		lSizeY = 1700;
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}


	PR_COORD coULCorner;
	PR_COORD coLRCorner;
	coULCorner.x = (PR_WORD)(GetPrCenterX() - lSizeX);
	coULCorner.y = (PR_WORD)(GetPrCenterY() - lSizeY);
	coLRCorner.x = (PR_WORD)(GetPrCenterX() + lSizeX);
	coLRCorner.y = (PR_WORD)(GetPrCenterY() + lSizeY);
	DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_RED);

	if ( m_bChangeCollet1 )
	{
		m_lEjtCollet1CoorX -= prd.x;
		if (m_lEjtCollet1CoorX < coULCorner.x)
		{
			m_lEjtCollet1CoorX = coULCorner.x;
		}
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
	}
	else if ( m_bChangeCollet2 )
	{
		m_lEjtCollet2CoorX -= prd.x;
		if (m_lEjtCollet2CoorX < coULCorner.x)
		{
			m_lEjtCollet2CoorX = coULCorner.x;
		}
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
	}
	
	int siStepX, siStepY;
	ConvertPixelToMotorStep(prd, &siStepX, &siStepY);

	int	siDestinationX, siDestinationY, siOrigX, siOrigY;
	GetEjtXYEncoder(&siOrigX, &siOrigY);
	siDestinationX = siOrigX - siStepX;
	siDestinationY = siOrigY - siStepY;

	MoveEjectorTable(siDestinationX, siDestinationY);

	//v4.46T9
	CString szMsg;
	szMsg.Format("EjtXY crosshair (%d %d) LT: cursor(%ld, %ld); UL(%ld, %ld); LR(%ld, %ld); Size(%ld, %ld); pitch(%ld, %ld), 1/2SrchWnd(%ld, %ld)", 
					m_bChangeCollet1, m_bChangeCollet2, m_lEjtCollet2CoorX, m_lEjtCollet2CoorY,
					coULCorner.x, coULCorner.y, 
					coLRCorner.x, coLRCorner.y,
					lSizeX, lSizeY, lPitchX, lPitchY, lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;	
}

LONG CWaferPr::WPR_EjtDisplayCollet_Move_Right(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	typedef struct 
	{
		LONG	lStep;	
	} BHSETUP;

	BHSETUP stInfo;
	svMsg.GetMsg(sizeof(BHSETUP), &stInfo);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	PR_COORD prd;
	if (bColletOffsetWoEjtXY)	//v4.52A14
	{
		prd.x = (PR_WORD)stInfo.lStep;
		prd.y = 0;
	}
	else
	{
		ConvertMotorStepToPixel(stInfo.lStep, 0, prd);
	}

	//v4.44A1	//SEmitek
	LONG lPitchX = GetDiePitchX_X();
	LONG lPitchY = GetDiePitchY_Y();

	//v4.43T9
	LONG lHalfSrchWndX = _round((GetSrchDieAreaX()*1.0/4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY = _round((GetSrchDieAreaY()*1.0/4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lSizeX = PR_DEF_CENTRE_X - lPitchX - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - lPitchY - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * lPitchY - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * lPitchY - lHalfSrchWndY;
		}
	}
	//v4.46T9
	if (lSizeX < 1700)
	{
		lSizeX = 1700;
	}
	if (lSizeY < 1700)
	{
		lSizeY = 1700;
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}


	PR_COORD coULCorner;
	PR_COORD coLRCorner;
	coULCorner.x = (PR_WORD)(GetPrCenterX() - lSizeX);
	coULCorner.y = (PR_WORD)(GetPrCenterY() - lSizeY);
	coLRCorner.x = (PR_WORD)(GetPrCenterX() + lSizeX);
	coLRCorner.y = (PR_WORD)(GetPrCenterY() + lSizeY);
	DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_RED);

	if ( m_bChangeCollet1 )
	{
		m_lEjtCollet1CoorX += prd.x;
		if (m_lEjtCollet1CoorX > coLRCorner.x)
		{
			m_lEjtCollet1CoorX = coLRCorner.x;
		}
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
	}
	else if ( m_bChangeCollet2 )
	{
		m_lEjtCollet2CoorX += prd.x;
		if (m_lEjtCollet2CoorX > coLRCorner.x)
		{
			m_lEjtCollet2CoorX = coLRCorner.x;
		}
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
	}

	int siStepX, siStepY;
	ConvertPixelToMotorStep(prd, &siStepX, &siStepY);

	int	siDestinationX, siDestinationY, siOrigX, siOrigY;
	GetEjtXYEncoder(&siOrigX, &siOrigY);
	siDestinationX = siOrigX + siStepX;
	siDestinationY = siOrigY + siStepY;

	MoveEjectorTable(siDestinationX, siDestinationY);

	//v4.46T9
	CString szMsg;
	szMsg.Format("EjtXY crosshair (%d %d) RT: cursor(%ld, %ld); UL(%ld, %ld); LR(%ld, %ld); Size(%ld, %ld); pitch(%ld, %ld), 1/2SrchWnd(%ld, %ld)", 
					m_bChangeCollet1, m_bChangeCollet2, m_lEjtCollet2CoorX, m_lEjtCollet2CoorY,
					coULCorner.x, coULCorner.y, 
					coLRCorner.x, coLRCorner.y,
					lSizeX, lSizeY, lPitchX, lPitchY, lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return TRUE;	
}

LONG CWaferPr::WPR_ResetEjtCenter(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	//BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14
	
	if (!CMS896AStn::m_bMS100EjtXY && !bColletOffsetWoEjtXY)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( m_bChangeCollet1 || m_bChangeCollet2 )
	{
		m_lEjtCollet1CoorX = m_lChangeColletTempCenter1X;
		m_lEjtCollet1CoorY = m_lChangeColletTempCenter1Y;
		m_lEjtCollet2CoorX = m_lChangeColletTempCenter2X;
		m_lEjtCollet2CoorY = m_lChangeColletTempCenter2Y;

		m_bChangeCollet1		= FALSE;
		m_bChangeCollet2		= FALSE;
	}

	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
		
	PR_COORD stCrossHair;
	stCrossHair.x = (PR_WORD)GetPrCenterX();
	stCrossHair.y = (PR_WORD)GetPrCenterY();
	DrawHomeCursor(stCrossHair);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

//For AGC Fcn only	//v4.50A31
LONG CWaferPr::WPR_UpdateAGCOffset(IPC_CServiceMessage &svMsg)
{
	CString szMsg;
	BOOL bReturn = TRUE;
	BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14
	
	if (!bMS100EjtXY && !bColletOffsetWoEjtXY)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	typedef struct 
	{
		BOOL bBHCollet2;
		LONG lAGCColletOffsetXinStep;
		LONG lAGCColletOffsetYinStep;
	} AGC_OFFSET;

	AGC_OFFSET stInfo;
	svMsg.GetMsg(sizeof(AGC_OFFSET), &stInfo);

	LONG lPixelOffsetX = ConvertEjtXMotorStepToPixel(stInfo.lAGCColletOffsetXinStep);
	LONG lPixelOffsetY = ConvertEjtYMotorStepToPixel(stInfo.lAGCColletOffsetYinStep);

	if (stInfo.bBHCollet2)
	{
		m_lEjtCollet2CoorX = m_lEjtCollet2CoorX + lPixelOffsetX;
		m_lEjtCollet2CoorY = m_lEjtCollet2CoorY + lPixelOffsetY;

		m_lEjtCollet2OffsetX =   m_lEjtCollet2OffsetX + stInfo.lAGCColletOffsetXinStep;
		m_lEjtCollet2OffsetY =   m_lEjtCollet2OffsetY + stInfo.lAGCColletOffsetYinStep;
		SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);

		double dEjtAdjCollet2OffsetX_um	= stInfo.lAGCColletOffsetXinStep * m_dEjtXYRes;
		double dEjtAdjCollet2OffsetY_um	= stInfo.lAGCColletOffsetYinStep * m_dEjtXYRes;
		CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
		if (pBinTable)
		{
			pBinTable->SaveBTAdjCollet2Offset_um(-dEjtAdjCollet2OffsetX_um, -dEjtAdjCollet2OffsetY_um);
		}


		szMsg.Format("WPR_UpdateAGCOffset: EJTXY Collet2 center in pixel (%ld %ld), EJTXY in step (%ld, %ld)", 
						m_lEjtCollet2CoorX, m_lEjtCollet2CoorY,
						m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	}
	else
	{
		m_lEjtCollet1CoorX = m_lEjtCollet1CoorX + lPixelOffsetX;
		m_lEjtCollet1CoorY = m_lEjtCollet1CoorY + lPixelOffsetY;

		m_lEjtCollet1OffsetX =   m_lEjtCollet1OffsetX + stInfo.lAGCColletOffsetXinStep;
		m_lEjtCollet1OffsetY =   m_lEjtCollet1OffsetY + stInfo.lAGCColletOffsetYinStep;
		SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);


		double dEjtAdjCollet1OffsetX_um	= stInfo.lAGCColletOffsetXinStep * m_dEjtXYRes;
		double dEjtAdjCollet1OffsetY_um	= stInfo.lAGCColletOffsetYinStep * m_dEjtXYRes;
		CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
		if (pBinTable)
		{
			pBinTable->SaveBTAdjCollet1Offset_um(-dEjtAdjCollet1OffsetX_um, -dEjtAdjCollet1OffsetY_um);
		}

		szMsg.Format("WPR_UpdateAGCOffset: EJTXY Collet1 center in pixel (%ld %ld), EJTXY in step (%ld, %ld)", 
						m_lEjtCollet1CoorX, m_lEjtCollet1CoorY,
						m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	}

	m_lEjtCollet1OffsetX_Pixel = m_lEjtCollet1CoorX - GetPrCenterX();
	m_lEjtCollet1OffsetY_Pixel = m_lEjtCollet1CoorY - GetPrCenterY();
	m_lEjtCollet2OffsetX_Pixel = m_lEjtCollet2CoorX - GetPrCenterX();
	m_lEjtCollet2OffsetY_Pixel = m_lEjtCollet2CoorY - GetPrCenterY();
	SavePrData(FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


VOID CWaferPr::CalcEjtColletOffsetCount(const LONG lEjtColletCoorX, const LONG lEjtColletCoorY, LONG &lStepX, LONG &lStepY)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	INT nStepX = 0, nStepY = 0;
	PR_COORD stDieOffset;
	stDieOffset.x = (PR_WORD)lEjtColletCoorX;
	stDieOffset.y = (PR_WORD)lEjtColletCoorY;
	if (bColletOffsetWoEjtXY)
		CalculateDieCompenate(stDieOffset, &nStepX, &nStepY);		//Use WT factor
	else
		CalculateEjtDieCompenate(stDieOffset, &nStepX, &nStepY);	//User EjtXY factor

	lStepX =   -1 * nStepX;
	lStepY =   -1 * nStepY;
}


VOID CWaferPr::CalcEjtColletOffset(const DOUBLE dRatio)
{
	m_lEjtCollet1OffsetX_Pixel = m_lEjtCollet1CoorX - GetPrCenterX();
	m_lEjtCollet1OffsetY_Pixel = m_lEjtCollet1CoorY - GetPrCenterY();
	m_lEjtCollet2OffsetX_Pixel = m_lEjtCollet2CoorX - GetPrCenterX();
	m_lEjtCollet2OffsetY_Pixel = m_lEjtCollet2CoorY - GetPrCenterY();

	CalcEjtColletOffsetCount(m_lEjtCollet1CoorX, m_lEjtCollet1CoorY, m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY);
	m_lEjtCollet1OffsetX = _round(m_lEjtCollet1OffsetX * dRatio);
	m_lEjtCollet1OffsetY = _round(m_lEjtCollet1OffsetY * dRatio);
	SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);

	CalcEjtColletOffsetCount(m_lEjtCollet2CoorX, m_lEjtCollet2CoorY, m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY);
	m_lEjtCollet2OffsetX = _round(m_lEjtCollet2OffsetX * dRatio);
	m_lEjtCollet2OffsetY = _round(m_lEjtCollet2OffsetY * dRatio);
	SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);
}


VOID CWaferPr::CalcBTColletOffset(const DOUBLE dRatio,
								  const LONG lChangeColletTempCenter1X, const LONG lChangeColletTempCenter1Y,
								  const LONG lChangeColletTempCenter2X, const LONG lChangeColletTempCenter2Y)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14
	PR_COORD stDieOffset;
	INT nStepX = 0, nStepY = 0;

	//Collet #1
	stDieOffset.x = (PR_WORD)(m_lEjtCollet1CoorX - lChangeColletTempCenter1X);
	stDieOffset.y = (PR_WORD)(m_lEjtCollet1CoorY - lChangeColletTempCenter1Y);
	if (bColletOffsetWoEjtXY)	//v4.52A14
		ConvertPixelToMotorStep(stDieOffset, &nStepX, &nStepY);
	else
		ConvertPixelToEjtMotorStep(stDieOffset, &nStepX, &nStepY);

	nStepX = _round(dRatio * nStepX);
	nStepY = _round(dRatio * nStepY);
	double dEjtAdjCollet1OffsetX_um	= nStepX * m_dEjtXYRes;
	double dEjtAdjCollet1OffsetY_um	= nStepY * m_dEjtXYRes;
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable)
	{
		pBinTable->SaveBTAdjCollet1Offset_um(-dEjtAdjCollet1OffsetX_um, -dEjtAdjCollet1OffsetY_um);
	}

	//Collet #2
	stDieOffset.x = (PR_WORD)(m_lEjtCollet2CoorX - lChangeColletTempCenter2X);
	stDieOffset.y = (PR_WORD)(m_lEjtCollet2CoorY - lChangeColletTempCenter2Y);
	if (bColletOffsetWoEjtXY)	//v4.52A14
		ConvertPixelToMotorStep(stDieOffset, &nStepX, &nStepY);
	else
		ConvertPixelToEjtMotorStep(stDieOffset, &nStepX, &nStepY);

	nStepX = _round(dRatio * nStepX);
	nStepY = _round(dRatio * nStepY);
	double dEjtAdjCollet2OffsetX_um	= nStepX * m_dEjtXYRes;
	double dEjtAdjCollet2OffsetY_um	= nStepY * m_dEjtXYRes;
	if (pBinTable)
	{
		pBinTable->SaveBTAdjCollet2Offset_um(-dEjtAdjCollet2OffsetX_um, -dEjtAdjCollet2OffsetY_um);
	}
}


LONG CWaferPr::WPR_ConfirmEjtSetUp(IPC_CServiceMessage &svMsg)
{
	IPC_CServiceMessage stMsg;
	RecoverBackupLighting(stMsg);
	
	if (m_bChangeCollet1)
	{
		MoveBhToPick(TRUE, 180000);
		HmiMessage("Please plug the air tube of Arm1, then Press any key");
		MoveBhToPick(FALSE);
	}

	BOOL bReturn = TRUE;
	BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	if (!bMS100EjtXY && !bColletOffsetWoEjtXY)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//v4.44A1	//SEmitek
	LONG lPitchX = GetDiePitchX_X();
	LONG lPitchY = GetDiePitchY_Y();

	//v4.43T9
	LONG lHalfSrchWndX = _round((GetSrchDieAreaX() * 1.0 / 4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY = _round((GetSrchDieAreaY() * 1.0 / 4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lSizeX = PR_DEF_CENTRE_X - lPitchX - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - lPitchY - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * lPitchY - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * lPitchY - lHalfSrchWndY;
		}
	}

	//v4.46T9
	if (lSizeX < 1700)
	{
		lSizeX = 1700;
	}
	if (lSizeY < 1700)
	{
		lSizeY = 1700;
	}

	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}

	//v4.46T9
	CString szMsg;
	szMsg.Format("WPR_ConfirmEjtSetUp: EJTXY Collet Wnd: X width = %ld, Y Width = %ld; Pitch(%ld %ld); 1/2SrchWnd(%ld %ld)", 
					lSizeX, lSizeY,
					lPitchX, lPitchY, 
					lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);


	PR_COORD coULCorner;
	PR_COORD coLRCorner;
	coULCorner.x = (PR_WORD)(GetPrCenterX() - lSizeX);
	coULCorner.y = (PR_WORD)(GetPrCenterY() - lSizeY);
	coLRCorner.x = (PR_WORD)(GetPrCenterX() + lSizeX);
	coLRCorner.y = (PR_WORD)(GetPrCenterY() + lSizeY);
	DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_TRANSPARENT);

	if ( m_bChangeCollet1 || m_bChangeCollet2 )
	{
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (m_bChangeCollet1)
		{
			pBondPr->ResetRtBHCompXYArrays(TRUE);
			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_Pixel"]		= 0;
			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_Pixel"]		= 0;

			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_um"]			= 0;
			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_um"]			= 0;
		}
		else
		{
			pBondPr->ResetRtBHCompXYArrays(FALSE);
			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_Pixel"]		= 0;
			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_Pixel"]		= 0;

			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_um"]			= 0;
			(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_um"]			= 0;
		}

		DOUBLE dRatio = 1;
		if (m_szGenDieZoomMode[10 + WPR_GEN_RDIE_OFFSET] == "1X")
		{
			dRatio = 0.25;
		}
		else if (m_szGenDieZoomMode[10 + WPR_GEN_RDIE_OFFSET] == "2X")
		{
			dRatio = 0.5;
		}

		CalcEjtColletOffset(dRatio);

		CheckColletOffsetOver500();
		SavePrData(FALSE);

		//enable BH Mark
		if (IsEnableBHMark())
		{
			IPC_CServiceMessage svMsg;
			LONG lLoopTest = 0;
			svMsg.InitMessage(sizeof(LONG), &lLoopTest);
			if (m_bChangeCollet1)
			{
				BH_TnZToPick1();
				UserSearchMark(svMsg, "BHMark", 1, WPR_REFERENCE_PR_DIE_INDEX5, m_lBHColletHoleRef1X, m_lBHColletHoleRef1Y, m_lBHMarkRef1X, m_lBHMarkRef1Y);
			}
			else
			{
				BH_TnZToPick2();
				UserSearchMark(svMsg, "BHMark", 2, WPR_REFERENCE_PR_DIE_INDEX6, m_lBHColletHoleRef2X, m_lBHColletHoleRef2Y, m_lBHMarkRef2X, m_lBHMarkRef2Y);
			}
			MoveBhToPick(FALSE);
		}

		//Update Bin Table Bond Position with Offset
		CalcBTColletOffset(dRatio,
						   m_lChangeColletTempCenter1X, m_lChangeColletTempCenter1Y,
						   m_lChangeColletTempCenter2X, m_lChangeColletTempCenter2Y);

		m_bChangeCollet1		= FALSE;
		m_bChangeCollet2		= FALSE;
	}

	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::EnableEjtColletOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bEnable = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	if (!bEnable)
	{
		m_lEjtCollet1OffsetX_Pixel = 0;
		m_lEjtCollet1OffsetY_Pixel = 0;
		m_lEjtCollet2OffsetX_Pixel = 0;
		m_lEjtCollet2OffsetY_Pixel = 0;

		m_lEjtCollet1CoorX = GetPrCenterX();
		m_lEjtCollet1CoorY = GetPrCenterY();
		m_lEjtCollet2CoorX = GetPrCenterX();
		m_lEjtCollet2CoorY = GetPrCenterY();

		m_lEjtCollet1OffsetX = 0;
		m_lEjtCollet1OffsetY = 0;
		m_lEjtCollet2OffsetX = 0;
		m_lEjtCollet2OffsetY = 0;

		SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);
		SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);

		SavePrData(FALSE);
		m_bChangeCollet1		= FALSE;
		m_bChangeCollet2		= FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferPr::CheckColletOffsetOver500()
{
	//v4.44A1	//SEmitek
	LONG lPitchX = GetDiePitchX_X();
	LONG lPitchY = GetDiePitchY_Y();

	//v4.43T9
	LONG lHalfSrchWndX = _round((GetSrchDieAreaX() * 1.0 / 4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY = _round((GetSrchDieAreaY() * 1.0 / 4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lSizeX = PR_DEF_CENTRE_X - lPitchX - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - lPitchY - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * lPitchY - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * lPitchY - lHalfSrchWndY;
		}
	}
	//v4.46T9
	if (lSizeX < 1700)
	{
		lSizeX = 1700;
	}
	if (lSizeY < 1700)
	{
		lSizeY = 1700;
	}
	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}

	//v4.52A14
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14

	//v4.46T9
	CString szMsg;
	szMsg.Format("CheckColletOffsetOver500: EJTXY Collet Wnd: X width = %ld, Y Width = %ld; Pitch(%ld %ld); 1/2SrchWnd(%ld %ld)", 
					lSizeX, lSizeY,
					lPitchX, lPitchY, 
					lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);


	if ( m_lEjtCollet1OffsetX_Pixel > lSizeX || m_lEjtCollet1OffsetX_Pixel < (-1 * lSizeX) || 
		 m_lEjtCollet1OffsetY_Pixel > lSizeY || m_lEjtCollet1OffsetY_Pixel < (-1 * lSizeY) ) 
	{
		HmiMessage("Error Collet 1 Offset Exceed look ahead limit ! \nThe Collet 1 RESET!");


		szMsg.Format("Error: Collet 1 Offset Exceed LookAhead limit: X = %ld (%ld), Y = %ld (%ld)", 
					m_lEjtCollet1OffsetX_Pixel, lSizeX,
					m_lEjtCollet1OffsetY_Pixel, lSizeY);
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		m_lEjtCollet1OffsetX_Pixel = 0;
		m_lEjtCollet1OffsetY_Pixel = 0;
		m_lEjtCollet1CoorX = GetPrCenterX();
		m_lEjtCollet1CoorY = GetPrCenterY();
		PR_COORD		stDieCoor; 
		stDieCoor.x = (PR_WORD)m_lEjtCollet1CoorX;
		stDieCoor.y = (PR_WORD)m_lEjtCollet1CoorY;
		int nStepX=0, nStepY=0;

		if (bColletOffsetWoEjtXY)		//v4.52A14
			CalculateDieCompenate(stDieCoor, &nStepX, &nStepY);
		else
			CalculateEjtDieCompenate(stDieCoor, &nStepX, &nStepY);
		m_lEjtCollet1OffsetX =  -1 * nStepX;
		m_lEjtCollet1OffsetY =  -1 * nStepY;

		SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);

		SavePrData(FALSE);
		return FALSE;
	}

	if  ( m_lEjtCollet2OffsetX_Pixel > lSizeX || m_lEjtCollet2OffsetX_Pixel < (-1 * lSizeX) || 
		  m_lEjtCollet2OffsetY_Pixel > lSizeY || m_lEjtCollet2OffsetY_Pixel < (-1 * lSizeY) ) 
	{
		HmiMessage("Error Collet 2 Offset Exceed look ahead limit ! \nThe Collet 2 RESET!");
		
		szMsg.Format("Error: Collet 2 Offset Exceed LookAhead limit: X = %ld (%ld), Y = %ld (%ld)", 
					m_lEjtCollet2OffsetX_Pixel, lSizeX,
					m_lEjtCollet2OffsetY_Pixel, lSizeY);
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		
		m_lEjtCollet2OffsetX_Pixel = 0;
		m_lEjtCollet2OffsetY_Pixel = 0;
		m_lEjtCollet2CoorX = GetPrCenterX();
		m_lEjtCollet2CoorY = GetPrCenterY();
		PR_COORD		stDieCoor; 
		stDieCoor.x = (PR_WORD)m_lEjtCollet2CoorX;
		stDieCoor.y = (PR_WORD)m_lEjtCollet2CoorY;
		int nStepX=0, nStepY=0;
		if (bColletOffsetWoEjtXY)		//v4.52A14
			CalculateDieCompenate(stDieCoor,&nStepX, &nStepY);
		else
			CalculateEjtDieCompenate(stDieCoor,&nStepX, &nStepY);
		m_lEjtCollet2OffsetX =  -1 * nStepX;
		m_lEjtCollet2OffsetY =  -1 * nStepY;

		SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);

		SavePrData(FALSE);
		return FALSE;
	}

	if ( abs( m_lEjtCollet2OffsetX - m_lEjtCollet1OffsetX ) > 1000 || abs( m_lEjtCollet2OffsetY - m_lEjtCollet1OffsetY ) > 1000 )
	{
		HmiMessage("ERROR: Collect offset difference of > 1000 \nThe Collet 1,2 RESET!");
		m_lEjtCollet1OffsetX_Pixel = 0;
		m_lEjtCollet1OffsetY_Pixel = 0;
		m_lEjtCollet1CoorX = GetPrCenterX();
		m_lEjtCollet1CoorY = GetPrCenterY();
		m_lEjtCollet2OffsetX_Pixel = 0;
		m_lEjtCollet2OffsetY_Pixel = 0;
		m_lEjtCollet2CoorX = GetPrCenterX();
		m_lEjtCollet2CoorY = GetPrCenterY();
		PR_COORD		stDieCoor1; 
		stDieCoor1.x = (PR_WORD)m_lEjtCollet1CoorX;
		stDieCoor1.y = (PR_WORD)m_lEjtCollet1CoorY;
		int nStepX1=0, nStepY1=0;
		PR_COORD		stDieCoor2; 
		stDieCoor2.x = (PR_WORD)m_lEjtCollet2CoorX;
		stDieCoor2.y = (PR_WORD)m_lEjtCollet2CoorY;
		int nStepX2=0, nStepY2=0;
		if (bColletOffsetWoEjtXY)		//v4.52A14
			CalculateDieCompenate(stDieCoor1, &nStepX1, &nStepY1);
		else 
			CalculateEjtDieCompenate(stDieCoor1, &nStepX1, &nStepY1);
		m_lEjtCollet1OffsetX =  -1 * nStepX1;
		m_lEjtCollet1OffsetY =  -1 * nStepY1;
		if (bColletOffsetWoEjtXY)		//v4.52A14
			CalculateDieCompenate(stDieCoor2, &nStepX2, &nStepY2);
		else
			CalculateEjtDieCompenate(stDieCoor2, &nStepX2, &nStepY2);
		m_lEjtCollet2OffsetX =  -1 * nStepX2;
		m_lEjtCollet2OffsetY =  -1 * nStepY2;

		SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);
		SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);

		SavePrData(FALSE);
		return FALSE;
	}
	
	return TRUE;
}

LONG CWaferPr::LearnEjtColletOffsetXY(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);

	if (!bMS100EjtXY && !bColletOffsetWoEjtXY)		//v4.52A14
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//4.53D22 check Frame Up Down //Check BT platform before homing BH T
	BOOL bFrameLevel	= CMS896AStn::MotionReadInputBit("ibFrameLevel");
	if (!bFrameLevel)
	{
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
		SetErrorMessage("Learn Collect: BT frame level not DOWN");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if ( IsWLExpanderOpen())	//if at UP position (sensor flag not cut)	//v4.04
	{
		SetErrorMessage("Learn Collect: WT Expander Open");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);

	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead->IsColletJamForBothBH() != 0) //Matthew 20190415
	{
		CString szError;
		szError.Format("Collet Jam!!!!!!!");
		HmiMessage_Red_Yellow(szError, "Collet Jam!");
		IPC_CServiceMessage stMsg;
		BOOL bToggleZoomScreen = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bToggleZoomScreen);
		ToggleZoomScreenCmd(stMsg);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//1. Select ARM 1 or 2 position to learn the collet offset
	CString szSelection1 = "Arm1";
	CString szSelection2 = "Arm2";
	CString szSelection3 = "Cancel";
	INT nArmSelection = HmiMessage("Please choose which arm to be learnt.", "Learn Collet Hole", glHMI_MBX_TRIPLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2, &szSelection3);
	//v4.43T5
	if (nArmSelection == 1)
	{
		m_bChangeCollet1	= TRUE;
		m_bChangeCollet2	= FALSE;
	}
	else if (nArmSelection == 5)
	{
		m_bChangeCollet1	= FALSE;
		m_bChangeCollet2	= TRUE;
	}
	else
	{
		IPC_CServiceMessage stMsg;
		BOOL bToggleZoomScreen = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bToggleZoomScreen);
		ToggleZoomScreenCmd(stMsg);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	ChangeCamera(WPR_CAM_WAFER);

	//Set Lighting
	typedef struct 
	{
		LONG lGroupID;
		LONG lCoaxLevel;
		LONG lRingLevel;
		LONG lSideLevel;
		LONG lBackLevel;
	} SETLIGHT;
	SETLIGHT stInfo;

	stInfo.lGroupID = 0;
	stInfo.lCoaxLevel = 0;
	stInfo.lSideLevel = 0;
	stInfo.lRingLevel = 0;
	stInfo.lBackLevel = 100;

	stMsg.InitMessage(sizeof(SETLIGHT), &stInfo);
	SetGeneralLighting2(stMsg);

	//Set Exposure Time
	LONG lExposureTimeLevel = 9;
	stMsg.InitMessage(sizeof(LONG), &lExposureTimeLevel);
	SetGeneralExposureTime(stMsg);

	//1.5  Backup current EjtCollet XY Pixel before update	//v4.54A9
	m_lChangeColletTempCenter1X = m_lEjtCollet1CoorX;
	m_lChangeColletTempCenter1Y = m_lEjtCollet1CoorY;
	m_lChangeColletTempCenter2X = m_lEjtCollet2CoorX;
	m_lChangeColletTempCenter2Y = m_lEjtCollet2CoorY;
	if (IsEnableBHMark())
	{
		if (m_bChangeCollet1)
		{
			LONG lPixelX = (*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX1_Pixel"];
			LONG lPixelY = (*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY1_Pixel"];

			m_lEjtCollet1CoorX += lPixelX;
			m_lEjtCollet1CoorY += lPixelY;
		}

		if (m_bChangeCollet2)
		{
			LONG lPixelX = (*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjX2_Pixel"];
			LONG lPixelY = (*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["EjY2_Pixel"];

			m_lEjtCollet2CoorX += lPixelX;
			m_lEjtCollet2CoorY += lPixelY;
		}
	}

	MoveEjectorTableToColletOffset(m_bChangeCollet2);

	//2. Try to find a die under the FOV
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	int nOffsetXInMotorStep=0, nOffsetYInMotorStep=0;
	
	PR_WIN stSrchArea;
	stSrchArea.coCorner1.x = GetPRWinULX();
	stSrchArea.coCorner1.y = GetPRWinULY();
	stSrchArea.coCorner2.x = GetPRWinLRX();
	stSrchArea.coCorner2.y = GetPRWinLRY();

	PR_WORD wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, 
										&usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
										stSrchArea.coCorner1, stSrchArea.coCorner2);
	if (wResult != -1)
	{
		LONG BHPickPosnMode = (*m_psmfSRam)["BondHead"]["BHPickPosnMode"];
		if(BHPickPosnMode == 2)
		{
			//MoveWaferTable(-127074, 52723);
			LONG WTNoDiePosX = (*m_psmfSRam)["WaferTable"]["Wafer Table No Die X Coordinate"];
			LONG WTNoDiePosY = (*m_psmfSRam)["WaferTable"]["Wafer Table No Die Y Coordinate"];

			MoveWaferTable(WTNoDiePosX, WTNoDiePosY);
		}
		else
		{
			if ( (DieIsAlignable(usDieType) == TRUE) && !bColletOffsetWoEjtXY )
			{
				CalculateDieCompenate(stDieOffset, &nOffsetXInMotorStep, &nOffsetYInMotorStep);
		
				if (nArmSelection == 1)		//BHZ1
				{
					nOffsetXInMotorStep = nOffsetXInMotorStep + m_lEjtCollet1OffsetX;
					nOffsetYInMotorStep = nOffsetYInMotorStep + m_lEjtCollet1OffsetY;
				}
				else
				{
					nOffsetXInMotorStep = nOffsetXInMotorStep + m_lEjtCollet2OffsetX;
					nOffsetYInMotorStep = nOffsetYInMotorStep + m_lEjtCollet2OffsetY;
				}

				int siOrigX=0, siOrigY=0, siOrigT=0;
				GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
				siOrigX += nOffsetXInMotorStep;
				siOrigY += nOffsetYInMotorStep;
				MoveWaferTable(siOrigX, siOrigY);
		
				Sleep(500);
			}
			else	//v4.43T6
			{
				CString szMsg;
				if (m_bChangeCollet1)		//BHZ1
				{
					m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
					szMsg = "Please move a die to Collet 1 cursor center to continue.";
				}
				else if (m_bChangeCollet2)
				{
					m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
					szMsg = "Please move a die to Collet 2 cursor center to continue.";
				}


				SetWaferTableJoystick(TRUE);
				HmiMessageEx(szMsg, "Learn Collet Offset", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 
					36000000, glHMI_MSG_MODAL, 0, 400, 300);
				SetWaferTableJoystick(FALSE);
			}
		}

		if (m_bChangeCollet1)		//BHZ1
		{
			m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
			MoveBhToPick(TRUE, 180000);
			HmiMessage("Please unplug the air tube of Arm1, then Press any key");
			MoveBhToPick(FALSE);
			Sleep(500);
			MoveBhToPick(TRUE);
//HmiMessage("Die to Collet 1 offset?");
		}
		else if (m_bChangeCollet2)
		{
			m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
			MoveBhToPick_Z2(TRUE);
//HmiMessage("Die to Collet 2 offset?");
		}
	}

/*
	if (nArmSelection == 1)		//BHZ1
	{
		MoveBhToPick(TRUE);
	}
	else						//BHZ2
	{
		MoveBhToPick_Z2(TRUE);
	}

	
	PR_WORD			rtnPR;
	//PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;
	//PR_COORD		stDieOffset; 
	//PR_REAL		fDieRotate; 
	//PR_REAL		fDieScore;
	int nStepX=0, nStepY=0;


	PR_WIN stSrchArea;
	stSrchArea.coCorner1.x = GetPRWinULX();
	stSrchArea.coCorner1.y = GetPRWinULY();
	stSrchArea.coCorner2.x = GetPRWinLRX();
	stSrchArea.coCorner2.y = GetPRWinLRY();

	PR_DETECT_SHAPE_RPY2    pstRpy  ;


	//Use Ref Die #3 to serach for collet hole
	Sleep(1000);
//	rtnPR = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE+2, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2);
	//find circle

	UINT stDieOffsetx = stDieOffset.x ;
	UINT stDieOffsety = stDieOffset.y ;

	m_lEjtCollet1OffsetX	= GetEjtCollet1OffsetX();
	m_lEjtCollet1OffsetY	= GetEjtCollet1OffsetY();
	m_lEjtCollet2OffsetX	= GetEjtCollet2OffsetX();
	m_lEjtCollet2OffsetY	= GetEjtCollet2OffsetY();

	rtnPR = AutoSearchCircle(GetPrCenterX(), GetPrCenterY(), GetDieSizePixelX(WPR_GEN_RDIE_OFFSET+3), GetDieSizePixelY(WPR_GEN_RDIE_OFFSET+3), stDieOffsetx, stDieOffsety) ;
	// if it is circle
	if (rtnPR != -1)
	{
		if (rtnPR == PR_TRUE)
		{
			bReturn = TRUE;
			stDieOffset.x = stDieOffsetx ;
			stDieOffset.y = stDieOffsety ;
			if ( nArmSelection == 1 ) {  // save arm 1
				m_lEjtCollet1CoorX = stDieOffset.x ;
				m_lEjtCollet1CoorY = stDieOffset.y ;
				m_lEjtCollet1OffsetX_Pixel = m_lEjtCollet1CoorX - GetPrCenterX() ;
				m_lEjtCollet1OffsetY_Pixel = m_lEjtCollet1CoorY - GetPrCenterY() ;
			}
			else {  // save arm 2
				m_lEjtCollet2CoorX = stDieOffset.x ;
				m_lEjtCollet2CoorY = stDieOffset.y ;
				m_lEjtCollet2OffsetX_Pixel = m_lEjtCollet2CoorX - GetPrCenterX() ;
				m_lEjtCollet2OffsetY_Pixel = m_lEjtCollet2CoorY - GetPrCenterY() ;
			}
			CalculateEjtDieCompenate(stDieOffset, &nStepX, &nStepY);
			DrawSearchBox( PR_COLOR_TRANSPARENT);
		}
		else
		{
			bReturn = FALSE;
		}
	}
	else
	{
		bReturn = FALSE;
	}

	if (nArmSelection == 1)		//BHZ1
	{
		MoveBhToPick(FALSE);
	}
	else						//BHZ2
	{
		MoveBhToPick_Z2(FALSE);
	}

	if (bReturn)
	{
		//int nEjtEncX=0, nEjtEncY=0;
		//GetEjtXYEncoder(&nEjtEncX, &nEjtEncY);
		//int lTargetEjtX = nEjtEncX - nStepX;
		//int lTargetEjtY = nEjtEncY - nStepY;
		BOOL bBH2 = FALSE;
		CString szContent;
		szContent.Format("Move EJT to target offset (%d, %d)?", -1 * nStepX, -1 * nStepY);
		//LONG lReturn = HmiMessageEx(szContent, "Learn EJT Collet Offset", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
		if (nArmSelection == 1)		//BHZ1
		{
			m_lEjtCollet1OffsetX =  -1 * nStepX;
			m_lEjtCollet1OffsetY =  -1 * nStepY;
			SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);
		}
		else						//BHZ2
		{
			bBH2 = TRUE;
			m_lEjtCollet2OffsetX =  -1 * nStepX;
			m_lEjtCollet2OffsetY =  -1 * nStepY;
			SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);
		}
		m_bBTIsMoveCollet = TRUE;
		
		SavePrData(FALSE);
		if ( CheckColletOffsetOver500())
		{
			LONG lReturn = HmiMessageEx(szContent, "Learn EJT Collet Offset", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
			if (lReturn == glHMI_YES)
			{
				//MoveEjectorTable(lTargetEjtX, lTargetEjtY);
				MoveEjectorTableToColletOffset(bBH2);
			}
		}
	}
	else
	{
		if (nArmSelection == 1)		//BHZ1
		{
			m_lEjtCollet1OffsetX =  0;
			m_lEjtCollet1OffsetY =  0;
			m_lEjtCollet1CoorX = GetPrCenterX();
			m_lEjtCollet1CoorY = GetPrCenterY();
			m_lEjtCollet1OffsetX_Pixel = m_lEjtCollet1CoorX - GetPrCenterX() ;
			m_lEjtCollet1OffsetY_Pixel = m_lEjtCollet1CoorY - GetPrCenterY() ;
			SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);
		}
		else						//BHZ2
		{
			m_lEjtCollet2OffsetX =  0;
			m_lEjtCollet2OffsetY =  0;
			m_lEjtCollet2CoorX = GetPrCenterX();
			m_lEjtCollet2CoorY = GetPrCenterY();
			m_lEjtCollet2OffsetX_Pixel = m_lEjtCollet2CoorX - GetPrCenterX() ;
			m_lEjtCollet2OffsetY_Pixel = m_lEjtCollet2CoorY - GetPrCenterY() ;
			SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);
		}

		SavePrData(FALSE);
		CheckColletOffsetOver500();
	}
*/
	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ConfirmEjtColletOffsetXY(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	//v4.44A1	//SEmitek
	LONG lPitchX = GetDiePitchX_X();
	LONG lPitchY = GetDiePitchY_Y();

	//v4.43T9
	LONG lHalfSrchWndX = _round((GetSrchDieAreaX() * 1.0 / 4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lHalfSrchWndY = _round((GetSrchDieAreaY() * 1.0 / 4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	LONG lSizeX = PR_DEF_CENTRE_X - lPitchX - lHalfSrchWndX;
	LONG lSizeY = PR_DEF_CENTRE_Y - lPitchY - lHalfSrchWndY;
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)	
		{
			lSizeX = PR_DEF_CENTRE_X - 3 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 3 * lPitchY - lHalfSrchWndY;
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			lSizeX = PR_DEF_CENTRE_X - 2 * lPitchX - lHalfSrchWndX;
			lSizeY = PR_DEF_CENTRE_Y - 2 * lPitchY - lHalfSrchWndY;
		}
	}
	//v4.46T9
	if (lSizeX < 1700)
	{
		lSizeX = 1700;
	}
	if (lSizeY < 1700)
	{
		lSizeY = 1700;
	}

	if ( (lHalfSrchWndX == 0) || (lHalfSrchWndY == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}
	if ( (GetPitchPixelXX() == 0) || (GetPitchPixelYY() == 0) )
	{
		lSizeX = 1700;
		lSizeY = 1700;
	}

	CString szMsg;
	szMsg.Format("EJTXY Collet Wnd: X width = %ld, Y Width = %ld; Pitch(%ld %ld); 1/2SrchWnd(%ld %ld)", lSizeX, lSizeY,
					GetPitchPixelXX(), GetPitchPixelYY(), lHalfSrchWndX, lHalfSrchWndY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);


	PR_COORD coULCorner;
	PR_COORD coLRCorner;
	coULCorner.x = (PR_WORD)(GetPrCenterX() - lSizeX);
	coULCorner.y = (PR_WORD)(GetPrCenterY() - lSizeY);
	coLRCorner.x = (PR_WORD)(GetPrCenterX() + lSizeX);
	coLRCorner.y = (PR_WORD)(GetPrCenterY() + lSizeY);
	DrawRectangleBox(coULCorner, coLRCorner, PR_COLOR_RED);

	if (m_bChangeCollet1)
	{
		if ((m_lEjtCollet1CoorX > coLRCorner.x) || (m_lEjtCollet1CoorX < coULCorner.x))
			m_lEjtCollet1CoorX = GetPrCenterX();
		if ((m_lEjtCollet1CoorY > coLRCorner.y) || (m_lEjtCollet1CoorY < coULCorner.y))
			m_lEjtCollet1CoorY = GetPrCenterY();
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
	}
	else
	{
		if ((m_lEjtCollet2CoorX > coLRCorner.x) || (m_lEjtCollet2CoorX < coULCorner.x))
			m_lEjtCollet2CoorX = GetPrCenterX();
		if ((m_lEjtCollet2CoorY > coLRCorner.y) || (m_lEjtCollet2CoorY < coULCorner.y))
			m_lEjtCollet2CoorY = GetPrCenterY();
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::CancelEjtColletOffsetXY(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bChangeCollet1)
	{
		MoveBhToPick(FALSE);
	}
	else
	{
		MoveBhToPick_Z2(FALSE);
	}

	IPC_CServiceMessage stMsg;
	RecoverBackupLighting(stMsg);

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SearchColletHoleEpoxyPattern(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	UINT stDieOffsetx = 0;
	UINT stDieOffsety = 0;

	ULONG lEpoxyAreaSize = 0;
	CString szErrMsg = "";
	BOOL bEpoxyStatus = AutoSearchEpoxy(FALSE, TRUE, FALSE, 0, 0, stDieOffsetx, stDieOffsety, lEpoxyAreaSize, NULL, szErrMsg);

	if (bEpoxyStatus)
	{
		if (m_bChangeCollet1)
		{
			m_lEjtCollet1CoorX = stDieOffsetx;
			m_lEjtCollet1CoorY = stDieOffsety;
		}
		else if (m_bChangeCollet2)
		{
			m_lEjtCollet2CoorX = stDieOffsetx;
			m_lEjtCollet2CoorY = stDieOffsety;
		}
	}

	if (m_bChangeCollet1)
	{
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
	}
	else
	{
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::AutoSearchColletHole(IPC_CServiceMessage &svMsg)		//v4.43T5
{
	BOOL bReturn = TRUE;
	PR_WORD		rtnPR;
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD	stDieOffset; 
//	PR_REAL		fDieRotate; 
//	PR_REAL		fDieScore;
	int nStepX=0, nStepY=0;

	PR_WIN stSrchArea;
	stSrchArea.coCorner1.x = GetPRWinULX();
	stSrchArea.coCorner1.y = GetPRWinULY();
	stSrchArea.coCorner2.x = GetPRWinLRX();
	stSrchArea.coCorner2.y = GetPRWinLRY();

//	PR_DETECT_SHAPE_RPY2 pstRpy;

	UINT stDieOffsetx = stDieOffset.x = 0;
	UINT stDieOffsety = stDieOffset.y = 0;

	m_lEjtCollet1OffsetX	= GetEjtCollet1OffsetX();
	m_lEjtCollet1OffsetY	= GetEjtCollet1OffsetY();
	m_lEjtCollet2OffsetX	= GetEjtCollet2OffsetX();
	m_lEjtCollet2OffsetY	= GetEjtCollet2OffsetY();

	rtnPR = AutoSearchCircle(GetPrCenterX(), GetPrCenterY(), GetDieSizePixelX(WPR_GEN_RDIE_OFFSET+3), GetDieSizePixelY(WPR_GEN_RDIE_OFFSET + 3), stDieOffsetx, stDieOffsety);
	// if it is circle
	if (rtnPR == PR_TRUE)
	{
		if (m_bChangeCollet1)
		{
			m_lEjtCollet1CoorX = stDieOffsetx;
			m_lEjtCollet1CoorY = stDieOffsety;
		}
		else if (m_bChangeCollet2)
		{
			m_lEjtCollet2CoorX = stDieOffsetx;
			m_lEjtCollet2CoorY = stDieOffsety;
		}
	}

	if (m_bChangeCollet1)
	{
		if ((m_lEjtCollet1CoorX > m_stSearchArea.coCorner2.x) || 
			(m_lEjtCollet1CoorX < m_stSearchArea.coCorner1.x))
			m_lEjtCollet1CoorX = GetPrCenterX();
		if ((m_lEjtCollet1CoorY > m_stSearchArea.coCorner2.y) || 
			(m_lEjtCollet1CoorY < m_stSearchArea.coCorner1.y))
			m_lEjtCollet1CoorY = GetPrCenterY();
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet1CoorX, m_lEjtCollet1CoorY);
	}
	else
	{
		if ((m_lEjtCollet2CoorX > m_stSearchArea.coCorner2.x) || 
			(m_lEjtCollet2CoorX < m_stSearchArea.coCorner1.x))
			m_lEjtCollet2CoorX = GetPrCenterX();
		if ((m_lEjtCollet2CoorY > m_stSearchArea.coCorner2.y) || 
			(m_lEjtCollet2CoorY < m_stSearchArea.coCorner1.y))
			m_lEjtCollet2CoorY = GetPrCenterY();
		m_pPrGeneral->DrawSmallCursor(GetRunSenID(), GetRunRecID(), m_lEjtCollet2CoorX, m_lEjtCollet2CoorY);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::SearchColletHole(IPC_CServiceMessage &svMsg)		//v4.47A5
{
	BOOL bReturn = TRUE;
	PR_WORD		rtnPR;
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	PR_COORD	stDieOffset; 
	int nStepX=0, nStepY=0;

	BOOL bBH2 = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bBH2);		//v4.48A8

	typedef struct {
		BOOL	bStatus;
		INT		nOffsetX;
		INT		nOffsetY;
	} SRCH_CHOLE_RESULT;

	SRCH_CHOLE_RESULT stResult;
	stResult.bStatus = FALSE;
	stResult.nOffsetX = 0;
	stResult.nOffsetY = 0;

	PR_WIN stSrchArea;
	stSrchArea.coCorner1.x = GetPRWinULX();
	stSrchArea.coCorner1.y = GetPRWinULY();
	stSrchArea.coCorner2.x = GetPRWinLRX();
	stSrchArea.coCorner2.y = GetPRWinLRY();
/*
	PR_DETECT_SHAPE_RPY2 pstRpy;

	UINT stDieOffsetx = 0;
	UINT stDieOffsety = 0;

	rtnPR = AutoSearchCircle(GetPrCenterX(), GetPrCenterY(), m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].x, m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].y, stDieOffsetx, stDieOffsety);

	// if it is circle
	if (rtnPR == PR_TRUE)
	{
		stResult.bStatus	= TRUE;
		stResult.nOffsetX	= stDieOffsetx;
		stResult.nOffsetY	= stDieOffsety;
	}
	else
	{
		stResult.bStatus	= FALSE;
	}
*/	
	if (bBH2)
	{
		//rtnPR = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
		//			stSrchCorner1, stSrchCorner2);
		rtnPR = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
					stSrchArea.coCorner1, stSrchArea.coCorner2);
	}
	else
	{
		rtnPR = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
					stSrchArea.coCorner1, stSrchArea.coCorner2);
	}

	INT siStepX=0, siStepY=0;
	if ( (rtnPR == -1) || (DieIsAlignable(usDieType) == FALSE) )
	{
		stResult.bStatus	= FALSE;
	}
	else
	{
		//CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		//stRelMove.x = (PR_WORD)GetPrCenterX() - stDieOffset.x;
		//stRelMove.y = (PR_WORD)GetPrCenterY() - stDieOffset.y;
		stResult.bStatus	= TRUE;
		stResult.nOffsetX	= GetPrCenterX() - stDieOffset.x;
		stResult.nOffsetY	= GetPrCenterY() - stDieOffset.y;
	}

	svMsg.InitMessage(sizeof(SRCH_CHOLE_RESULT), &stResult);
	return 1;
}


LONG CWaferPr::ShowEjtColletOffsetXY(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14
	BOOL bMS100EjtXY = (BOOL) GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_ENABLE);
	if (!bMS100EjtXY && !bColletOffsetWoEjtXY)		//v4.52A14
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);


	CString szSelection1 = "Arm1";
	CString szSelection2 = "Arm2";
	INT nArmSelection = HmiMessage("Please choose which arm to be learnt.", "Learn Collet Hole", glHMI_MBX_DOUBLEBUTTON, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, 0, &szSelection1, &szSelection2);
	m_nArmSelection = nArmSelection;
	if (bColletOffsetWoEjtXY)		//v4.52A14
	{
		if (nArmSelection == 1)		//BHZ1
		{
			MoveBhToPick(TRUE);
		}
		else						//BHZ2
		{
			MoveBhToPick_Z2(TRUE);
		}
	}

	
//	PR_WORD			rtnPR;
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD		stDieOffset; 
	//PR_REAL			fDieRotate; 
	//PR_REAL			fDieScore;
	int nStepX=0, nStepY=0;


	PR_WIN stSrchArea;
	stSrchArea.coCorner1.x = GetPRWinULX();
	stSrchArea.coCorner1.y = GetPRWinULY();
	stSrchArea.coCorner2.x = GetPRWinLRX();
	stSrchArea.coCorner2.y = GetPRWinLRY();

//	PR_DETECT_SHAPE_RPY2    pstRpy  ;


	//Use Ref Die #3 to serach for collet hole
	Sleep(1000);
//	rtnPR = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE+2, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchArea.coCorner1, stSrchArea.coCorner2);
	//find circle

	PR_WORD  stDieOffsetx , stDieOffsety = 0 ;

	if ( nArmSelection == 1 ) 
	{  // load arm 1 coor
		stDieOffsetx = (PR_WORD)m_lEjtCollet1CoorX ;
		stDieOffsety = (PR_WORD)m_lEjtCollet1CoorY ;
	}
	else 
	{  // load arm 2 coor
		stDieOffsetx = (PR_WORD)m_lEjtCollet2CoorX ;
		stDieOffsety = (PR_WORD)m_lEjtCollet2CoorY ;
	}

//	PR_COORD stCrossHair;
//	stCrossHair.x = (PR_WORD)GetPrCenterX();
//	stCrossHair.y = (PR_WORD)GetPrCenterY();
//	DrawHomeCursor(stCrossHair);

	PR_UBYTE ubSID = GetRunSenID();
	PR_UBYTE ubRID = GetRunRecID();
	m_pPrGeneral->DrawSmallCursor(ubSID, ubRID, stDieOffsetx, stDieOffsety);

//	rtnPR = AutoSearchCircle(GetPrCenterX(), GetPrCenterY(), m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].x, m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].y, stDieOffsetx, stDieOffsety) ;
	// if it is circle
//	if (rtnPR != -1)
//	{
//		if (rtnPR == PR_TRUE)
//		{
	bReturn = TRUE;
	stDieOffset.x = stDieOffsetx ;
	stDieOffset.y = stDieOffsety ;
	CalculateEjtDieCompenate(stDieOffset, &nStepX, &nStepY);
	DrawSearchBox( PR_COLOR_TRANSPARENT);


	for (int i = 0; i < WPR_CUR_FORWARD; i++)
	{
		PR_COORD NewAreaCorner1 = m_stLFSearchArea[i].coCorner1;
		PR_COORD NewAreaCorner2 = m_stLFSearchArea[i].coCorner2;

		if ( nArmSelection == 1 )	//BH1
		{
			NewAreaCorner1.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
			NewAreaCorner1.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
			NewAreaCorner2.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
			NewAreaCorner2.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
		}
		else						//BH2
		{
			NewAreaCorner1.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
			NewAreaCorner1.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
			NewAreaCorner2.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
			NewAreaCorner2.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
		}
		CString ss ;
		ss.Format("%d %d %d %d", NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y);
		//HmiMessage(ss);
		if ( nArmSelection == 1 ) 
		{
			DisplayRectArea(NewAreaCorner1, NewAreaCorner2, PR_COLOR_YELLOW);
		}
		else 
		{
			DisplayRectArea(NewAreaCorner1, NewAreaCorner2, PR_COLOR_RED);
		}
	}


	if (bColletOffsetWoEjtXY && bReturn)		//v4.52A14
	{
		LONG lReturn = HmiMessageEx("Press CLOSE to end.", "Learn EJT Collet Offset", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);

		if (nArmSelection == 1)		//BHZ1
		{
			MoveBhToPick(FALSE);
		}
		else						//BHZ2
		{
			MoveBhToPick_Z2(FALSE);
		}
	}
	else if (bMS100EjtXY && bReturn)		//v4.52A14
	{
		//int nEjtEncX=0, nEjtEncY=0;
		//GetEjtXYEncoder(&nEjtEncX, &nEjtEncY);
		//int lTargetEjtX = nEjtEncX - nStepX;
		//int lTargetEjtY = nEjtEncY - nStepY;
		BOOL bBH2 = FALSE;
		CString szContent;
		szContent.Format("Move EJT to target offset (%d, %d)?", -1 * nStepX, -1 * nStepY);
		LONG lReturn = HmiMessageEx(szContent, "Learn EJT Collet Offset", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
		if (nArmSelection == 1)		//BHZ1
		{
			//m_lEjtCollet1OffsetX =  -1 * nStepX;
			//m_lEjtCollet1OffsetY =  -1 * nStepY;
			//SaveEjtCollet1Offset(m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY, m_dEjtXYRes);
		}
		else						//BHZ2
		{
			bBH2 = TRUE;
			//m_lEjtCollet2OffsetX =  -1 * nStepX;
			//m_lEjtCollet2OffsetY =  -1 * nStepY;
			//SaveEjtCollet2Offset(m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY, m_dEjtXYRes);
		}

		//SavePrData(FALSE);

		if (lReturn == glHMI_YES)
		{
			//MoveEjectorTable(lTargetEjtX, lTargetEjtY);
			MoveEjectorTableToColletOffset(bBH2);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferPr::LearnEjtXYCalibration(IPC_CServiceMessage &svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bColletOffsetWoEjtXY = pApp->GetFeatureStatus(MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY);		//v4.52A14
	BOOL bReturn = TRUE;
	
	if (bColletOffsetWoEjtXY)	//v4.52A14
	{
		//No EjtXY for this calibration; sw will use current WT calibration factor only;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	LONG lReturn = 0;
	lReturn = FindEjtXYCalibration();
	if (lReturn == 0)
	{
		//Reset BH1 & BH2 Collet offsets
		m_lEjtCollet1OffsetX = 0;
		m_lEjtCollet1OffsetY = 0;
		m_lEjtCollet2OffsetX = 0;
		m_lEjtCollet2OffsetY = 0;

		SavePrData(FALSE);
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::FindEjtXYCalibration(VOID)		//v4.42T3
{
	PR_UWORD		usDieType;
	PR_COORD		stDiePosition[2]; 
	PR_COORD		stDieShiftX, stDieShiftY; 
	PR_COORD		stXPixelMove; 
	PR_COORD		stYPixelMove; 
	PR_WIN			stOrgSearchArea = m_stSearchArea;
	PR_WIN			stDieSizeArea;
	PR_REAL			fDieRotate; 
	
	int				siSrchMargin = (32 * GetPrScaleFactor());
	int				siTempData1 = 0;
	int				siTempData2 = 0;
//	int				siThetaPos;
	short			ssCycle;			
	float			fTemp = 0.0;
	BOOL			bTempCorrection = m_bThetaCorrection;
	PR_WORD			uwResult;
	PR_REAL			fDieScore;
	CString szMsg;

	typedef struct 
	{
		int XPos;
		int YPos;
	} MOTORDATA;

	MOTORDATA	stDestX, stDestY;
	MOTORDATA	stLastPos, stTempPos;
	MOTORDATA	stTablePos[2];
	MOTORDATA	stXMotorMove, stYMotorMove;

	UCHAR ucDieNo = WPR_GEN_RDIE_OFFSET + EJT_CAP_CALIBRATION_PR_ID;
	//Use normal die record #1 to learn ejector cap pattern
	PR_WORD wPixelPitchX = (PR_WORD)(1.3 * GetDieSizePixelX(ucDieNo));
	PR_WORD wPixelPitchY = (PR_WORD)(1.3 * GetDieSizePixelY(ucDieNo));

	LONG lMode = 0;
	LONG lOldZoom = GetRunZoom();
	UCHAR ucEjtRecord = EJT_CAP_CALIBRATION_PR_ID;
/*	
	if (IsEnableBHMark())
	{
		CString szMsg;
		CStringList szList;
		szList.AddTail("Current FOV");
		szList.AddTail("FFMode");

		CString szContent = "Please select FOV";
		CString szTitle =  "Ejt Calib";
		LONG lSet = 0;
		lMode = HmiSelection(szContent, szTitle, szList, lSet);
		if (lMode == 1)
		{
			LiveViewZoom(0, FALSE, FALSE);
		}
	}

	if (lMode == 1)
	{
		ucEjtRecord = EJT_CAP_FFCALIBRATION_PR_ID;
	}
*/
	PR_WIN stSearchArea;
	//Update Search area
	stSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - wPixelPitchX;
	stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - wPixelPitchY;
	stSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + wPixelPitchX;
	stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + wPixelPitchY;
	VerifyPRRegion(&stSearchArea);

	stDieSizeArea = stSearchArea;
	DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

	//Temp to dsiable  Theta Correction
	//m_bThetaCorrection = FALSE;
//AfxMessageBox("FindEjtXYCalibration ...", MB_SYSTEMMODAL);
	
	//v4.46T21		//Get EjtXY motors travel range
	LONG lXMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_MIN_DISTANCE);								
	LONG lXMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_X, MS896A_CFG_CH_MAX_DISTANCE);	
	LONG lYMinPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_Y, MS896A_CFG_CH_MIN_DISTANCE);								
	LONG lYMaxPos = GetChannelInformation(MS896A_CFG_CH_EJECTOR_Y, MS896A_CFG_CH_MAX_DISTANCE);	
	szMsg.Format("Find EjtXY Calibration start: X (%ld, %ld), Y (%ld, %ld)", lXMinPos, lXMaxPos,  lYMinPos, lYMaxPos);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	//Store Current position	
	GetEjtXYEncoder(&stLastPos.XPos, &stLastPos.YPos);

	LONG lReturn = 0;
	for (ssCycle= 0; ssCycle < 2; ssCycle++)	//	<=2
	{
		szMsg.Format("EjtXY Calibration cycle %d", ssCycle);
		SetAlarmLog(szMsg);
		//Search Die on current position
		stSearchArea = stDieSizeArea;

		uwResult	= ManualSearchDie(WPR_REFERENCE_DIE, ucEjtRecord, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_1ST_DIE;
			break;
		}

		if (ssCycle == 0)
		{
			stDestX.XPos = m_lPrCal1stStepSize;	
			stDestX.YPos = 0;     
			stDestY.YPos = m_lPrCal1stStepSize;	
			stDestY.XPos = 0;     
		}
		else
		{
			//Only update search area & calculate motor step on cycle 1 & 2 
			// if in full frame mode the GetPRWinULX & GetPRWinLRX is too large
			siTempData1 = (int)(stDiePosition[0].x - wPixelPitchX / 2 - GetPRWinULX() - siSrchMargin);
			siTempData2 = (int)(GetPRWinLRX() - (stDiePosition[0].x + wPixelPitchX / 2) - siSrchMargin);

			szMsg.Format("Ejx Cal,siTempData1,%f,stDiePosition[0].x,%f,wPixelPitchX,%f,GetPRWinULX(),%f,siSrchMargin,%f,siTempData2,%f,GetPRWinLRX(),%f",
				siTempData1,stDiePosition[0].x,wPixelPitchX,GetPRWinULX(),siSrchMargin,siTempData2,GetPRWinLRX());
			SetAlarmLog(szMsg);

			if (ssCycle != 3)	// divide by zero
			{
				stDieShiftX.x = abs((min(siTempData1, siTempData2)) / (3 - ssCycle));
			}
			stDieShiftX.y = 0;
			ConvertPixelToEjtMotorStep(stDieShiftX, &stDestX.XPos, &stDestX.YPos);

			//v4.46T21	//Check X travel range
			LONG lXPosHWLimit	= lXMaxPos;		//1000;
			LONG lXNegHWLimit	= lXMinPos;		//-6000;
			LONG lPosXLimit		= stLastPos.XPos + stDestX.XPos;
			LONG lNegXLimit		= stLastPos.XPos - stDestX.XPos;
			BOOL bXChanged = FALSE;
			szMsg.Format("Ejx Cal2,lXPosHWLimit,%f,lXNegHWLimit,%f,stLastPos.XPos,%f,stDestX.XPos,%f,lPosXLimit,%f,lNegXLimit,%f",
				lXPosHWLimit,lXNegHWLimit,stLastPos.XPos,stDestX.XPos,lPosXLimit,lNegXLimit);
			SetAlarmLog(szMsg);
			if (lPosXLimit > lXPosHWLimit)
			{
				stDestX.XPos = lXPosHWLimit - stLastPos.XPos;
				bXChanged = TRUE;
				lNegXLimit = stLastPos.XPos - stDestX.XPos;
			}
			if (lNegXLimit < lXNegHWLimit)
			{
				stDestX.XPos = stLastPos.XPos - lNegXLimit;
				bXChanged = TRUE;
			}
			 
			if (bXChanged)
			{
				int nXPos=0, nYPos=0;

				do {
					stDieShiftX.x = stDieShiftX.x - 10;
					ConvertPixelToEjtMotorStep(stDieShiftX, &nXPos, &nYPos);

				} while (nXPos > stDestX.XPos);

				ConvertPixelToEjtMotorStep(stDieShiftX, &stDestX.XPos, &stDestX.YPos);

				szMsg.Format("Find EjtXY Calibration Cycle #%d X-Index changed: XDestX=%ld, XDestY=%ld, LastPosX=%ld", 
					ssCycle, stDestX.XPos, stDestX.YPos, stLastPos.XPos);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				SetAlarmLog(szMsg);
			}


			// if in full frame mode the GetPRWinULY & GetPRWinLRY is too large
			siTempData1 = (int)(stDiePosition[0].y - wPixelPitchY / 2 - GetPRWinULY() - siSrchMargin);
			siTempData2 = (int)(GetPRWinLRY() - (stDiePosition[0].y + wPixelPitchY / 2) - siSrchMargin);

			szMsg.Format("Ejx Cal3,siTempData1,%f,stDiePosition[0].y,%f,wPixelPitchY,%f,GetPRWinULX(),%f,siSrchMargin,%f,siTempData2,%f,GetPRWinLRX(),%f",
				siTempData1,stDiePosition[0].y,wPixelPitchY,GetPRWinULX(),siSrchMargin,siTempData2,GetPRWinLRX());
			SetAlarmLog(szMsg);
			if (ssCycle != 3)	// divide by zero
			{
				stDieShiftY.y = abs((min(siTempData1, siTempData2)) / (3 - ssCycle));
			}
			stDieShiftY.x = 0;
			ConvertPixelToEjtMotorStep(stDieShiftY, &stDestY.XPos, &stDestY.YPos);

			//v4.46T21
			LONG lYPosHWLimit	= lYMaxPos;		//6800;
			LONG lYNegHWLimit	= lYMinPos;		//200;
			LONG lPosYLimit		= stLastPos.YPos + stDestY.YPos;
			LONG lNegYLimit		= stLastPos.YPos - stDestY.YPos;
			BOOL bYChanged = FALSE;
			szMsg.Format("Ejx Cal2,lYPosHWLimit,%f,lYNegHWLimit,%f,stLastPos.YPos,%f,stDestY.YPos,%f,lPosYLimit,%f,lNegYLimit,%f",
				lYPosHWLimit,lYNegHWLimit,stLastPos.YPos,stDestY.YPos,lPosYLimit,lNegYLimit);
			SetAlarmLog(szMsg);
			if (lPosYLimit > lYPosHWLimit)
			{
				stDestY.YPos = lYPosHWLimit - stLastPos.YPos;
				bYChanged	= TRUE;
				lNegYLimit	= stLastPos.YPos - stDestY.YPos;

				szMsg.Format("#%d: Y INDEX-dist changed due to Y POS-Limit: YPosHWLimit = %d, LastDieY = %d, INDEXY = %d, NegYLimit = %d", 
					ssCycle, lYPosHWLimit, stLastPos.YPos, stDestY.YPos, lNegYLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				SetAlarmLog(szMsg);
			}

			if (lNegYLimit < lYNegHWLimit)
			{
				stDestY.YPos = stLastPos.YPos - lYNegHWLimit;
				bYChanged = TRUE;

				szMsg.Format("#%d: Y INDEX-dist changed due to Y NEG-Limit: LastDieY = %d, YNegLimit = %d, INDEXY = %d", 
					ssCycle, stLastPos.YPos, lYNegHWLimit, stDestY.YPos);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				SetAlarmLog(szMsg);
			}
			 
			if (bYChanged)
			{
				int nXPos=0, nYPos=0;
				do {
					stDieShiftY.y = stDieShiftY.y - 10;
					ConvertPixelToEjtMotorStep(stDieShiftY, &nXPos, &nYPos);

				} while (nYPos > stDestY.YPos);

				ConvertPixelToEjtMotorStep(stDieShiftY, &stDestY.XPos, &stDestY.YPos);

				szMsg.Format("Find EjtXY Calibration Cycle #%d Y-Index changed: YDestX=%ld, YDestY=%ld, LastPosY=%ld", 
					ssCycle, stDestY.XPos, stDestY.YPos, stLastPos.YPos);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				SetAlarmLog(szMsg);
			}
		}

		//Get current motor pos to calculate next postion
		GetEjtXYEncoder(&stTempPos.XPos, &stTempPos.YPos);


		// Move Table X + direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_TRANSPARENT);
			DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);

			//Update Search die area
			stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - wPixelPitchY;
			stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + wPixelPitchY;
			stSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - wPixelPitchX + stDieShiftX.x;
			stSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + wPixelPitchX + stDieShiftX.x;
			VerifyPRRegion(&stSearchArea);
			DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_GREEN);
		}

		//////////////////////////////////////////////////
		/////////// Move table to X+ direction /////////// 
		MoveEjectorTable(stTempPos.XPos + stDestX.XPos, stTempPos.YPos + stDestX.YPos);
		Sleep(1000);
		GetEjtXYEncoder(&stTablePos[0].XPos, &stTablePos[0].YPos);
		
		uwResult = ManualSearchDie(WPR_REFERENCE_DIE, ucEjtRecord, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			szMsg.Format("EJT Calibration: no LT die found by PR; PR result = 0x%x", usDieType);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Yellow(szMsg, "EJT Calibration");
			break;
		}
		else
		{
			szMsg.Format("EJT-Cal: LT die found at: pixel(%d, %d), PR result = 0x%x, SrchWnd(%d, %d,  %d, %d)", 
				stDiePosition[0].x, stDiePosition[0].y, usDieType, 
				stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
				stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}

		// Move Table X - direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_TRANSPARENT);
			DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);

			//Update Search die area
			stSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - wPixelPitchY;
			stSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + wPixelPitchY;
			stSearchArea.coCorner1.x = (-stDieShiftX.x) + (PR_WORD)GetPrCenterX() - wPixelPitchX;
			stSearchArea.coCorner2.x = (-stDieShiftX.x) + (PR_WORD)GetPrCenterX() + wPixelPitchX;
			VerifyPRRegion(&stSearchArea);
			DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_GREEN);
		}

		//////////////////////////////////////////////////
		/////////// Move table to X- direction ///////////
		MoveEjectorTable(stTempPos.XPos - stDestX.XPos, stTempPos.YPos - stDestX.YPos);
		Sleep(1000);
		GetEjtXYEncoder(&stTablePos[1].XPos, &stTablePos[1].YPos);

		uwResult	= ManualSearchDie(WPR_REFERENCE_DIE, ucEjtRecord, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[1], &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_RT_DIE;
			szMsg.Format("EJT Calibration: no RT die found by PR; PR result = 0x%x", usDieType);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Yellow(szMsg, "EJT Calibration");
			break;
		}
		else
		{
			szMsg.Format("EJT-Cal: RT die found at: pixel(%d, %d), PR result = 0x%x, SrchWnd(%d, %d,  %d, %d)", 
				stDiePosition[1].x, stDiePosition[1].y, usDieType, 
				stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
				stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}


		//Get Both X Value
		stXMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stXMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stXPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stXPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;
		szMsg.Format("WFT X    X %d -- %d, Y %d -- %d;		WPR X    X %d -- %d, Y %d -- %d", 
					 stTablePos[1].XPos, stTablePos[0].XPos, stTablePos[1].YPos, stTablePos[0].YPos,
					 stDiePosition[1].x, stDiePosition[0].x, stDiePosition[1].y, stDiePosition[0].y);
		SetAlarmLog(szMsg);
		szMsg.Format("Table X Diff: %d,%d; WPR X Diff: %d,%d", 
					 stXMotorMove.XPos, stXMotorMove.YPos, stXPixelMove.x, stXPixelMove.y);
		SetAlarmLog(szMsg);

		//Move table back to start poistion
		DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

		//Move Table Y + direction search PR
		if (ssCycle != 0)
		{
			//Update Search die area
			stSearchArea.coCorner1.x	= (PR_WORD)GetPrCenterX() - wPixelPitchX;
			stSearchArea.coCorner2.x	= (PR_WORD)GetPrCenterX() + wPixelPitchX;
			stSearchArea.coCorner1.y	= (stDieShiftY.y) + (PR_WORD)GetPrCenterY() - wPixelPitchY;
			stSearchArea.coCorner2.y	= (stDieShiftY.y) + (PR_WORD)GetPrCenterY() + wPixelPitchY;
			VerifyPRRegion(&stSearchArea);
			DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_GREEN);
		}

		//////////////////////////////////////////////////
		/////////// Move table to Y+ direction ///////////
		MoveEjectorTable(stTempPos.XPos + stDestY.XPos, stTempPos.YPos + stDestY.YPos);
		Sleep(1000);
		GetEjtXYEncoder(&stTablePos[0].XPos, &stTablePos[0].YPos);
		
		uwResult	= ManualSearchDie(WPR_REFERENCE_DIE, ucEjtRecord, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_UP_DIE;
			szMsg.Format("EJT Calibration: no UP die found by PR; PR result = 0x%x", usDieType);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Yellow(szMsg, "EJT Calibration");
			break;
		}
		else
		{
			szMsg.Format("EJT-Cal: UP die found at: pixel(%d, %d), PR result = 0x%x, SrchWnd(%d, %d,  %d, %d)", 
				stDiePosition[0].x, stDiePosition[0].y, usDieType, 
				stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
				stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}

		//Move Table Y - direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_TRANSPARENT);
			DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);

			//Update Search die area
			stSearchArea.coCorner1.x	= (PR_WORD)GetPrCenterX() - wPixelPitchX;
			stSearchArea.coCorner2.x	= (PR_WORD)GetPrCenterX() + wPixelPitchX;
			stSearchArea.coCorner1.y	= (-stDieShiftY.y) + (PR_WORD)GetPrCenterY() - wPixelPitchY;
			stSearchArea.coCorner2.y	= (-stDieShiftY.y) + (PR_WORD)GetPrCenterY() + wPixelPitchY;
			VerifyPRRegion(&stSearchArea);
			DrawRectangleBox(stSearchArea.coCorner1, stSearchArea.coCorner2, PR_COLOR_GREEN);
		}
		
		//////////////////////////////////////////////////
		/////////// Move table to Y- direction ///////////
		MoveEjectorTable(stTempPos.XPos - stDestY.XPos, stTempPos.YPos - stDestY.YPos);
		Sleep(1000);
		GetEjtXYEncoder(&stTablePos[1].XPos, &stTablePos[1].YPos);
		
		uwResult	= ManualSearchDie(WPR_REFERENCE_DIE, ucEjtRecord, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[1], &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE))
		{
			lReturn = WPR_ERR_CALIB_NO_DN_DIE;
			szMsg.Format("EJT Calibration: no DOWN die found by PR; PR result = 0x%x", usDieType);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Yellow(szMsg, "EJT Calibration");
			break;
		}
		else
		{
			szMsg.Format("EJT-Cal: DOWN die found at: pixel(%d, %d), PR result = 0x%x, SrchWnd(%d, %d,  %d, %d)", 
				stDiePosition[1].x, stDiePosition[1].y, usDieType, 
				stSearchArea.coCorner1.x, stSearchArea.coCorner1.y,
				stSearchArea.coCorner2.x, stSearchArea.coCorner2.y);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}


		//Get Both Y Value
		stYMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stYMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stYPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stYPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;
		szMsg.Format("WFT Y    X %d -- %d, Y %d -- %d;		WPR Y    X %d -- %d, Y %d -- %d", 
					 stTablePos[1].XPos, stTablePos[0].XPos, stTablePos[1].YPos, stTablePos[0].YPos, 
					 stDiePosition[1].x, stDiePosition[0].x, stDiePosition[1].y, stDiePosition[0].y);
		SetAlarmLog(szMsg);
		szMsg.Format("Table Y Diff: %d,%d; WPR Y Diff: %d,%d", 
					 stYMotorMove.XPos, stYMotorMove.YPos, stYPixelMove.x, stYPixelMove.y);
		SetAlarmLog(szMsg);


		// Check for zero error!
		fTemp = (float)(stXPixelMove.x * stYPixelMove.y - stYPixelMove.x * stXPixelMove.y);
		if (fabs(fTemp) < 0.000001)
		{
			lReturn = WPR_ERR_CALIB_ZERO_VALUE;
			szMsg = "EJT Calibration: calibration ZERO value is found!";
			SetErrorMessage(szMsg);
			HmiMessage_Red_Yellow(szMsg, "EJT Calibration");
			break;
		}


		if (lMode == 1)
		{
			m_dEjtFFCalibX	= (DOUBLE)(stXMotorMove.XPos * stYPixelMove.y - stYMotorMove.XPos * stXPixelMove.y) / fTemp;
			if (stYPixelMove.y != 0)
			{
				m_dEjtFFCalibXY = ((DOUBLE)stYMotorMove.XPos - GetCalibX() * (DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;
			}
			else
			{
				m_dEjtFFCalibXY = 0.0;
			}
			m_dEjtFFCalibY	= (DOUBLE)(stYMotorMove.YPos * stXPixelMove.x - stXMotorMove.YPos * stYPixelMove.x) / fTemp;
			if (stXPixelMove.x != 0)
			{
				m_dEjtFFCalibYX = ((DOUBLE)stXMotorMove.YPos - GetCalibY() * (DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;
			}
			else
			{
				m_dEjtFFCalibYX = 0.0;
			}
			szMsg.Format("FFCalibration #%d - X: %.4f, %.4f;\nY %.4f, %.4f;\nfactor %.4f", 
							ssCycle, m_dEjtFFCalibX, m_dEjtFFCalibXY, m_dEjtFFCalibY, m_dEjtFFCalibYX, fTemp);
			SetAlarmLog(szMsg);
			szMsg.Format("FFCalibration #%d - X: %.4f, %.4f; Y %.4f, %.4f; factor %.4f", 
							ssCycle, m_dEjtFFCalibX, m_dEjtFFCalibXY, m_dEjtFFCalibY, m_dEjtFFCalibYX, fTemp);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			HmiMessage(szMsg);
			//LiveViewZoom(lOldZoom, FALSE, FALSE);////
		}
		else
		{
		m_dEjtCalibX	= (DOUBLE)(stXMotorMove.XPos * stYPixelMove.y - stYMotorMove.XPos * stXPixelMove.y) / fTemp;
		if (stYPixelMove.y != 0)
		{
			m_dEjtCalibXY = ((DOUBLE)stYMotorMove.XPos - GetCalibX() * (DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;
		}
		else
		{
			m_dEjtCalibXY = 0.0;
		}
		m_dEjtCalibY	= (DOUBLE)(stYMotorMove.YPos * stXPixelMove.x - stXMotorMove.YPos * stYPixelMove.x) / fTemp;
		if (stXPixelMove.x != 0)
		{
			m_dEjtCalibYX = ((DOUBLE)stXMotorMove.YPos - GetCalibY() * (DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;
		}
		else
		{
			m_dEjtCalibYX = 0.0;
		}

		szMsg.Format("Calibration #%d - X: %.4f, %.4f;\nY %.4f, %.4f;\nfactor %.4f", 
						ssCycle, m_dEjtCalibX, m_dEjtCalibXY, m_dEjtCalibY, m_dEjtCalibYX, fTemp);
		SetAlarmLog(szMsg);
		szMsg.Format("Calibration #%d - X: %.4f, %.4f; Y %.4f, %.4f; factor %.4f", 
						ssCycle, m_dEjtCalibX, m_dEjtCalibXY, m_dEjtCalibY, m_dEjtCalibYX, fTemp);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}
//HmiMessageEx(szMsg);

		//Move table back to start poistion & do PR & update start position
		DrawRectangleBox(stSearchArea.coCorner1,  stSearchArea.coCorner2,  PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

		MoveEjectorTable(stLastPos.XPos, stLastPos.YPos);
		Sleep(1000);
		stSearchArea = stDieSizeArea;

		ManualSearchDie(WPR_REFERENCE_DIE, ucEjtRecord, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2);
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualEjtDieCompenate(stDiePosition[0]);
			GetEjtXYEncoder(&stLastPos.XPos, &stLastPos.YPos);
		}
	}

	if (lMode == 1)
	{
		LiveViewZoom(lOldZoom, FALSE, FALSE, 20);//
	}
	m_stSearchArea = stOrgSearchArea;
	//m_bThetaCorrection = bTempCorrection;
	return lReturn;
}

//v4.42T3
VOID CWaferPr::ManualEjtDieCompenate(PR_COORD stDieOffset)
{
	int			siStepX = 0;
	int			siStepY = 0;
	int			siOrigX = 0;
	int			siOrigY = 0;

	//CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	//CalculateNDieOffsetXY(siStepX, siStepY);
	CalculateEjtCompenate(stDieOffset, &siStepX, &siStepY);
	GetEjtXYEncoder(&siOrigX, &siOrigY);

	siOrigX += siStepX;
	siOrigY += siStepY;

	MoveEjectorTable(siOrigX, siOrigY);
	return;
}

VOID CWaferPr::CalculateEjtDieCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY, BOOL bFF)
{
	PR_COORD	stRelMove;

	stRelMove.x = (PR_WORD)GetPrCenterX() - stDieOffset.x;
	stRelMove.y = (PR_WORD)GetPrCenterY() - stDieOffset.y;

//	if (bFF)
//	{
//		ConvertFFPixelToEjtMotorStep(stRelMove, siStepX, siStepY);
//	}
//	else
//	{
	ConvertPixelToEjtMotorStep(stRelMove, siStepX, siStepY);
//	}
}

BOOL CWaferPr::MoveEjectorTable(int siXAxis, int siYAxis)
{
	IPC_CServiceMessage stMsg;
	
	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;

	RELPOS stPos;
	stPos.lX = siXAxis;
	stPos.lY = siYAxis;

	stMsg.InitMessage(sizeof(RELPOS), &stPos);
	INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "EjtXY_MoveToCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
	Sleep(20);

	BOOL bReturn = TRUE;
	//stMsg.GetMsg(sizeof(BOOL), &bReturn);
	return bReturn;
}

BOOL CWaferPr::MoveEjectorTableToColletOffset(CONST BOOL bBH2)
{
	IPC_CServiceMessage stMsg;
	
	BOOL bIsBH2 = bBH2;

	stMsg.InitMessage(sizeof(BOOL), &bIsBH2);
	INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "EjtXY_MoveToColletOffset", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
	Sleep(20);

	return TRUE;
}

//v4.42T3
VOID CWaferPr::GetEjtXYEncoder(int *siXAxis, int *siYAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} ENCVAL;

	ENCVAL stEnc;

	int nConvID = 0;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "GetEjtXYEncoderCmd", stMsg);

	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);

			stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
			
			*siXAxis = stEnc.lX;
			*siYAxis = stEnc.lY;
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

VOID CWaferPr::ConvertPixelToEjtMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY)
{
	*siStepX = (int)((DOUBLE)stPixel.x * m_dEjtCalibX + (DOUBLE)stPixel.y * m_dEjtCalibXY);
	*siStepY = (int)((DOUBLE)stPixel.y * m_dEjtCalibY + (DOUBLE)stPixel.x * m_dEjtCalibYX);
}

VOID CWaferPr::ConvertFFPixelToEjtMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY)
{
	*siStepX = (int)((DOUBLE)stPixel.x * m_dEjtFFCalibX + (DOUBLE)stPixel.y * m_dEjtFFCalibXY);
	*siStepY = (int)((DOUBLE)stPixel.y * m_dEjtFFCalibY + (DOUBLE)stPixel.x * m_dEjtFFCalibYX);
}


VOID CWaferPr::ConvertFFPixelToUM(PR_COORD stPixel, double &dStepX_um, double &dStepY_um)
{
	double dStepX = ((DOUBLE)stPixel.x * m_stZoomView.m_dScanCalibXX + (DOUBLE)stPixel.y * m_stZoomView.m_dScanCalibXY);
	double dStepY = ((DOUBLE)stPixel.y * m_stZoomView.m_dScanCalibYY + (DOUBLE)stPixel.x * m_stZoomView.m_dScanCalibYX);

	dStepX_um = dStepX * m_dWTXYRes;
	dStepY_um = dStepY * m_dWTXYRes;
}


LONG CWaferPr::ConvertEjtXMotorStepToPixel(int nStepX)
{
	if (m_dEjtCalibX == 0)
		return 0;
	return (LONG) (nStepX / m_dEjtCalibX);
}

LONG CWaferPr::ConvertEjtYMotorStepToPixel(int nStepY)
{
	if (m_dEjtCalibX == 0)
		return 0;
	return (LONG) (nStepY / m_dEjtCalibY);
}

VOID CWaferPr::CalculateEjtCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY)
{
	PR_COORD	stRelMove;
	stRelMove.x = (PR_WORD)GetPrCenterX() - stDieOffset.x;
	stRelMove.y = (PR_WORD)GetPrCenterY() - stDieOffset.y;
	ConvertPixelToEjtMotorStep(stRelMove, siStepX, siStepY);
}

/*
LONG CWaferPr::WPR_EjtUsePRMouse(IPC_CServiceMessage& svMsg)
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetWaferTableJoystick(FALSE);
	}

	PR_SET_MOUSE_CTRL_OBJ_CMD		stSetCmd;
   	PR_SET_MOUSE_CTRL_OBJ_RPY		stSetRpy;
	PR_DEFINE_MOUSE_CTRL_OBJ_CMD	stDefineCmd;
    PR_DEFINE_MOUSE_CTRL_OBJ_RPY    stDefineRpy;

	if ( m_bUseMouse == FALSE )
	{
		// Erase the original region
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT],PR_UPPER_LEFT,0);
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT],PR_LOWER_RIGHT,0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_RED);

		// Set the mouse size first
		PR_COORD coPoint1, coPoint2;
		coPoint1.x = 1000;
		coPoint1.y = 1000;
		coPoint2.x = 1000;
		coPoint2.y = 1000;
		m_pPrGeneral->MouseSet2PointRegion(coPoint1, coPoint2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_RED);
		m_bUseMouse = TRUE;
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::WPR_EjtDrawRectComplete(IPC_CServiceMessage& svMsg)
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetWaferTableJoystick(TRUE);
	}

	PR_COORD stAGCCalRectCornerPos[BPR_MAX_DIE_CORNER];

	if ( m_bUseMouse == TRUE )
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(stAGCCalRectCornerPos[PR_UPPER_LEFT],
														   stAGCCalRectCornerPos[PR_LOWER_RIGHT]);

		if (uwNumOfCorners == 2)
		{
			DrawAndEraseCursor(stCalRectCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 1);
			DrawAndEraseCursor(stCalRectCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);
			//CalibrateRectangle(1.7, m_stAGCCalRectCornerPos[PR_UPPER_LEFT], m_stAGCCalRectCornerPos[PR_LOWER_RIGHT]);
			CalibrateRectangle(1.2, stCalRectCornerPos[PR_UPPER_LEFT], stCalRectCornerPos[PR_LOWER_RIGHT]);
		}

		m_bUseMouse = FALSE;
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/
