#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "math.h"
#include "MS_SecCommConstant.h"
#include "FileUtil.h"
#include "WaferPr.h"
#include "WaferLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::SearchHomeDie()
{
	BOOL bReturn = FALSE;

	if( m_bSearchHomeOption==0 )
	{
		//Do sprial search
		CMSLogFileUtility::Instance()->WT_LogStatus("Sprial Search Start");
		bReturn = SpiralSearchHomeDie();
		CMSLogFileUtility::Instance()->WT_LogStatus("Sprial Search end");
	}
	else
	{
		//Do cornerDie search
		CMSLogFileUtility::Instance()->WT_LogStatus("Corner Search Start");
		XY_SafeMoveTo(m_nCornerHomeDiePosX, m_nCornerHomeDiePosY);
		bReturn = TRUE;
	}

	return bReturn;
}

BOOL CWaferTable::FindWaferHomeDie()
{
	IPC_CServiceMessage stMsg;
	LONG lCurrentX, lCurrentY;
	//Align wafer angle
	CMSLogFileUtility::Instance()->WT_LogStatus("Correct wafer angle");
	//Request wafer table stn to update global angle
	Sleep(200);
	GetEncoderValue();
	SetGlobalTheta();

	//Request switch camera to wafer side
	ChangeCameraToWafer();

	CreeAutoSearchMapLimit();	// semi auto button

	//Find HOME die on wafer
	CMSLogFileUtility::Instance()->WT_LogStatus("Find Global Theta start");

	//Move to HomeDie Position
	lCurrentX = m_lHomeDieWftPosnX;
	lCurrentY = m_lHomeDieWftPosnY;
	if (XY_SafeMoveTo(lCurrentX, lCurrentY))
	{
	}
	else 
	{
		return FALSE;
	}

	if( m_bEnableGlobalTheta )
	{
		//Move to HomeDie Position
		LONG lGTX, lGTY;
		lGTX = lCurrentX;
		lGTY = lCurrentY;
		if( m_bNewGTPosition )
		{
				lGTX = m_lNewGTPositionX;
				lGTY = m_lNewGTPositionY;
		}
		if (XY_SafeMoveTo(lGTX, lGTY))
		{
		}
		else 
		{
			return FALSE;
		}

		if (FindGlobalAngle() == FALSE)
		{
			CMSLogFileUtility::Instance()->WT_LogStatus("Cannot correct wafer angle");
			SetErrorMessage("Cannot correct wafer angle");

			SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
			return FALSE;
		}
	}

	//Move to HomeDie Position
	if( m_bNewGTPosition )
	{
		XY_SafeMoveTo(lCurrentX, lCurrentY);
	}

	BOOL bHome = SearchHomeDie();

	// for cree with joystick to locate home die 
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( bHome==FALSE && (pApp->GetCustomerName()=="Cree") )
	{
		LONG lReturn ;
		CString szContent, szTitle;
		szTitle.LoadString(HMB_WT_ALIGN_WAFER);
		szContent.LoadString(HMB_WT_SET_ALIGN_POS);
		SetJoystickOn(TRUE);
		lReturn = HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		SetJoystickOn(FALSE);
		if ( lReturn == glHMI_CONTINUE)
		{
			bHome = SearchHomeDie();
		}
	}
	// for cree with joystick to locate home die 

	if( bHome==FALSE )
	{
		CMSLogFileUtility::Instance()->WT_LogStatus("Search Home die failed");
		SetErrorMessage("Search Home die failed");
		SetAlert_Red_Yellow(IDS_WL_SRCH_HOMEDIE_FAIL);
		return FALSE;
	}
	CMSLogFileUtility::Instance()->WT_LogStatus("Find home die finish");

	//Perform auto wafer alignment
	CMSLogFileUtility::Instance()->WT_LogStatus("Align wafer start in auto button");
	if( FullAutoAlignWafer(m_bSearchHomeOption, m_ulCornerSearchOption)==FALSE )
	{
		CMSLogFileUtility::Instance()->WT_LogStatus("Align wafer failed in auto button\n");
		SetErrorMessage("Align wafer failed\n");
		SetAlert_Red_Yellow(IDS_WL_ALIGNWAF_FAIL);
		return FALSE;
	}

	//  auto align wafer, by auto align button or start button
	if( IsFastHomeScan() )	//	4.24TX 3
		m_bPrescan2ndTimeStart = TRUE;
	PrescanUpdateWaferAlignment(m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol, 3);	// fully auto, std machine

	CMSLogFileUtility::Instance()->WT_LogStatus("Align wafer finish");

	return TRUE;
}

BOOL CWaferTable::FindGlobalAngle()	//	426TX	2
{
	IPC_CServiceMessage stMsg;

	//Request switch camera to wafer side
	ChangeCameraToWafer();

	//Find HOME die on wafer
	CMSLogFileUtility::Instance()->WT_LogStatus("Find Global Theta start");

	//Locate any die in the wafer table
	CMSLogFileUtility::Instance()->WT_LogStatus("Find 1st die on wafer");
	Sleep(50);
	BOOL bFindFirstDie = SearchAndAlignDie(TRUE, FALSE, TRUE);

	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	DOUBLE dFov = min(dFOVX, dFOVY);
	if( IsAOIOnlyMachine() && dFov<2.5 && bFindFirstDie==FALSE )
	{
		int			siTempX = 0;
		int			siTempY = 0;
		for(int i=0; i<2; i++)
		{
			Sleep(100);
			GetEncoderValue();
			siTempX = GetCurrX();
			siTempY = GetCurrY();
			if( i==0 )
				siTempX = siTempX + GetDiePitchX_X()/2;
			else
				siTempY = siTempY + GetDiePitchY_Y()/2;
			XY_SafeMoveTo(siTempX, siTempY);
			Sleep(100);
			bFindFirstDie = SearchAndAlignDie(TRUE, FALSE, TRUE);
			if( bFindFirstDie )
			{
				break;
			}
		}
	}


	if (bFindFirstDie == FALSE)
	{		
		CMSLogFileUtility::Instance()->WT_LogStatus("WT: Find 1st die on wafer fails");
		SetErrorMessage("WT: Find 1st die on wafer fails");

		SetAlert_Red_Yellow(IDS_WL_NODIE_IN_WAFER);
		return FALSE;
	}

	// Get the reply for the encoder value
	int			siTempX = 0;
	int			siTempY = 0;
	int			siLHSDieX = 0;
	int			siLHSDieY = 0;
	int			siRHSDieX = 0;
	int			siRHSDieY = 0;
	LONG		siOrigX;
	LONG		siOrigY;
	LONG		siOrigT;
	LONG		i;

	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();

	//Save original position
	Sleep(50);
	GetEncoderValue();
	siLHSDieX = siRHSDieX = siOrigX = siTempX = GetCurrX();
	siLHSDieY = siRHSDieY = siOrigY = siTempY = GetCurrY();
	siOrigT = GetCurrT();

	CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
	LONG lLHSIndexCount = pWaferLoader->GetAngleLHSCount();
	for (i=0; i<lLHSIndexCount; i++)
	{
		//Index table to next pitch on LEFT/TOP
		if (m_lAlignWaferAngleMode_Std == 1)	//Vert
		{
			XY_SafeMoveTo(siTempX + lDiePitchY_X, siTempY + lDiePitchY_Y);
		}
		else					//Horiz
		{
			XY_SafeMoveTo(siTempX + lDiePitchX_X, siTempY + lDiePitchX_Y);
		}

		//Search die at curr position
		Sleep(50);
		if( SearchAndAlignDie(TRUE, TRUE, TRUE) )
		{
			Sleep(50);
			GetEncoderValue();
			siLHSDieX = siTempX = GetCurrX();
			siLHSDieY = siTempY = GetCurrY();
		}
		else
		{
			//v4.08
			Sleep(50);
			GetEncoderValue();
			siTempX = GetCurrX();
			siTempY = GetCurrY();
			//break;
		}
	}

	//Move to original position
	XY_SafeMoveTo(siOrigX, siOrigY);
	siTempX = siOrigX;
	siTempY = siOrigY;
	Sleep(200);

	LONG lRHSIndexCount = pWaferLoader->GetAngleRHSCount();
	for (i = 0; i < lRHSIndexCount; i++)
	{
		//Index table to next pitch on right/DOWN
		if (m_lAlignWaferAngleMode_Std == 1)	//Vert
		{
			XY_SafeMoveTo(siTempX - lDiePitchY_X, siTempY - lDiePitchY_Y);
		}
		else					//Horiz
		{
			XY_SafeMoveTo(siTempX - lDiePitchX_X, siTempY - lDiePitchX_Y);
		}

		Sleep(50);
		if( SearchAndAlignDie(TRUE, TRUE, TRUE) )
		{
			Sleep(50);
			GetEncoderValue();
			siRHSDieX = siTempX = GetCurrX();
			siRHSDieY = siTempY = GetCurrY();
		}
		else
		{
			//v4.08
			Sleep(50);
			GetEncoderValue();
			siTempX = GetCurrX();
			siTempY = GetCurrY();
			//break;
		}
	}


	//Move to original position
	XY_SafeMoveTo(siOrigX, siOrigY);
	Sleep(50);

	//Calcuate wafer angle	
	DOUBLE		dThetaAngle	= 0.0;
	DOUBLE dDiffY = (double)(siLHSDieY - siRHSDieY);
	DOUBLE dDiffX = (double)(siLHSDieX - siRHSDieX);
	if (m_lAlignWaferAngleMode_Std == 1)	//Vert
	{
		dThetaAngle = atan2(dDiffX * -1.00 , dDiffY) * (180 / PI);
	}
	else
	{
		dThetaAngle = atan2(dDiffY , dDiffX) * (180 / PI);
	}

	//Rotate angle 
	RotateWaferTheta(&siOrigX, &siOrigY, &siOrigT, dThetaAngle);

	T_Move(siOrigT, SFM_WAIT);
	XY_SafeMoveTo(siOrigX, siOrigY);

	//Update Last Die status & global theta position
	Sleep(50);
	GetEncoderValue();
	SetGlobalTheta();
	GetPrescanDiePitch();	// semi auto find gt

	//Locate any die in the wafer table
	CMSLogFileUtility::Instance()->WT_LogStatus("Finding die in wafer again");
	Sleep(50);
	if (SearchAndAlignDie(TRUE, FALSE, TRUE) == FALSE)
	{
		CMSLogFileUtility::Instance()->WT_LogStatus("Cannot find any die in wafer");
		SetErrorMessage("Cannot find any die in wafer");

		SetAlert_Red_Yellow(IDS_WL_NODIE_IN_WAFER);
		return FALSE;
	}

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (pWaferPr != NULL)
	{
		LONG nErr = pWaferPr->SearchAOTDie();
		if (nErr > 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

LONG CWaferTable::AutoAlignStdWafer(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if( m_bAutoSrchHome && IsAlignedWafer()==FALSE )
	{
		m_dStartWaferAlignTime = GetTime();
// prescan relative code
		CreateDummyMapForPrescanPicknPlace();	// semi auto button for Std
		SetAlignmentStatus(FALSE);
		bReturn = FindWaferHomeDie();	// presss start button, std machine
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

// new for standard machine auto align wafer
VOID CWaferTable::SetGlobalTheta()
{
	m_lGlobalTheta = GetCurrT();
}

LONG CWaferTable::GetGlobalT()
{
	return m_lGlobalTheta;
}

BOOL CWaferTable::FullAutoAlignWafer(BOOL bAlignMethod, ULONG ulCornerSearchOption)
{
	BOOL	bResult;
	ULONG	ulRow, ulCol;
	LONG	lX, lY, lT;

	if (IsEnablePNP() == TRUE)
	{
		AlignUpdatePickAndPlacePosn();
		SetAlignmentStatus(TRUE);
		CMSLogFileUtility::Instance()->MS_LogOperation("Pick And Place Fully-AutoAlign Wafer completed");
		return TRUE;
	}

	GetMapAlignHomeDie(ulRow, ulCol);
	m_ulCurrentRow = m_ulAlignRow = ulRow;
	m_ulCurrentCol = m_ulAlignCol = ulCol;
	GetEncoder(&lX, &lY, &lT);
	m_lCurrent_X = m_lAlignPosX = lX;
	m_lCurrent_Y = m_lAlignPosY = lY;
	m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);	


	//v4.53A22
	CString szTemp;
	szTemp.Format("FullAutoAlignWafer: HOME Die at INT(%ld, %ld), User(%ld, %ld); Ref-Die=%d", 
					ulRow, ulCol, m_lHomeDieMapRow, m_lHomeDieMapCol,
					m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol));
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);


	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetCustomerName() == "Lumileds") && 
		 (m_ucPLLMWaferAlignOption == 2) )		//v4.53A22	//Only for U2U wafer tiles
	{	
		// check the home die is valid die position
		if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol)==FALSE )
		{
			HmiMessage_Red_Back("Home die is invalid, please manually align wafer!", "Auto Align Wafer");
			return FALSE;
		}
	}

	if ( IsBlkFunc2Enable() || IsPrescanBlkPick() )	//Block2
	{
		LONG lAlarmStatus = GetAlarmLamp_Status();

		bResult = FALSE;
	
		//Set SECSGEM Value
		LONG lSlotNo = 1;
		try
		{
			SetGemValue(MS_SECS_SV_WAFER_ID, m_szWaferId);		//3003
			SetGemValue(MS_SECS_SV_SLOT_NO, lSlotNo);			//3004
			SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");		//3005
			SendEvent(SG_CEID_StartAlignment, TRUE);
		}
		catch(...)
		{
			DisplaySequence("SG_CEID_StartAlignment failure");
		}

		m_WaferMapWrapper.EnableSequenceCheck(FALSE);
		SaveBlkData();
		bResult = Blk2FindAllReferDiePosn();
		if( bResult==FALSE )
		{
			m_WaferMapWrapper.DeleteAllBoundary();
		}

		SetAlarmLamp_Back(lAlarmStatus, FALSE, TRUE);
		SetAlarmLog("Blk2 semi auto align wafer");

		SetAlignmentStatus(bResult);

		if( bResult==TRUE )
		{
			SetPrescanAutoAlignMap();

			m_pBlkFunc2->Blk2GetStartDie(&m_lBlkHomeRow, &m_lBlkHomeCol, &m_lBlkHomeX, &m_lBlkHomeY);
			//Return to Align position
			SetAlignStartPoint(FALSE, m_lBlkHomeX, m_lBlkHomeY, m_lBlkHomeRow, m_lBlkHomeCol);

			//Update Wafertable & wafer map start position
			m_lStart_X	= m_lBlkHomeX;
			m_lStart_Y	= m_lBlkHomeY;
		}
		if( pApp->GetCustomerName()=="OSRAM" )
		{
			if( bResult )
				HmiMessage_Red_Back("Auto Align Finish", "Align Wafer Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 60000, glHMI_MSG_MODAL);
			else
				HmiMessage_Red_Back("Auto Align Failure", "Align Wafer Message", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 60000, glHMI_MSG_MODAL);
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("block2 Fully-AutoAlign Wafer completed");
	}
	else
	{
		//Check SCN file is loaded or not
		if ( CheckSCNFileLoaded() == FALSE )
		{
			return FALSE;
		}

		bResult = FALSE;
		if ( IsBlkFunc1Enable() )
		{
			m_WaferMapWrapper.EnableSequenceCheck(FALSE);
			SaveBlkData();
			Blk1SetAlignParameter();
			if ( IsCharDieInUse() )
			{
				bResult = m_pBlkFunc->FoundAllCharDiePos();
			}
			else
			{
				bResult = m_pBlkFunc->FoundAllRefDiePos();
			}
		}
		else
		{	
			m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
			SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);
			SetPrescanAutoAlignMap();

			m_lStart_X	= lX;
			m_lStart_Y	= lY;

			if ( bAlignMethod == WT_SPIRAL_SEARCH )
			{
				if (pApp->GetCustomerName() == "IsoFoton")
					bResult = AlignAllRefDiePositions();
				else
					bResult = TRUE;
			}
			else
			{
				// hard code for test
				LONG lStatus = AlignRectWafer(ulCornerSearchOption);
				//LONG lStatus = TRUE;

				if ( lStatus == WT_ALN_OK )
				{
					bResult = TRUE;
				}
			}

			//Return to Align position	
			SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);

			m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol); 
		}

		if( bResult )
		{
			bResult = CheckTekCoreAutoAlignResult(m_ulAlignRow, m_ulAlignCol);
		}

		SetAlignmentStatus(bResult);

		CMSLogFileUtility::Instance()->MS_LogOperation("auto button Fully-AutoAlign Wafer completed");
	}
	(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulAlignRow;
	(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulAlignCol;
	(*m_psmfSRam)["MS896A"]["WaferTableX"] = m_lCurrent_X;
	(*m_psmfSRam)["MS896A"]["WaferTableY"] = m_lCurrent_Y;
	GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_MAN);
	return bResult;
}

BOOL CWaferTable::AlignUpdatePickAndPlacePosn()
{
	//Set parameters
	SetWaferIndexingPathParameters();

	GetEncoderValue();
	SetAlignmentStatus(TRUE);
	m_bScnLoaded	= FALSE;
	m_lStart_X		= GetCurrX();
	m_lStart_Y		= GetCurrY();

	m_stLastDie.lX = 0;
	m_stLastDie.lY = 0;
	m_stLastDie.ucGrade = 48 + m_ucDummyPrescanPNPGrade;
	m_stLastDie.lX_Enc = GetCurrX();
	m_stLastDie.lY_Enc = GetCurrY();
	m_stLastDie.lT_Enc = GetCurrT();
	m_stCurrentDie = m_stLastDie;

	if (m_bEnableSmartWalkInPicknPlace == FALSE)
	{
		m_WaferMapWrapper.SetAlgorithmParameter("StartX", m_lStart_X);
		m_WaferMapWrapper.SetAlgorithmParameter("StartY", m_lStart_Y);
		m_WaferMapWrapper.SetPhysicalPosition(0, 0, m_lStart_X, m_lStart_Y);
		m_WaferMapWrapper.SetStartPosition(0, 0);
	}
	else
	{
		LONG lWaferUpperLeftX, lWaferUpperLeftY, lStartRow, lStartCol, lHoriDiePitchX, lVertDiePitchY;
		LONG lHoriDiePitchY, lVertDiePitchX;

		lWaferUpperLeftX = GetCircleWaferMaxX();			
		lWaferUpperLeftY = GetCircleWaferMaxY();

		lHoriDiePitchX	= GetDiePitchX_X();
		lHoriDiePitchY	= GetDiePitchX_Y();

		lVertDiePitchX	= GetDiePitchY_X();
		lVertDiePitchY	= GetDiePitchY_Y();
	
		lStartCol = labs(m_lStart_X - lWaferUpperLeftX)/(lHoriDiePitchX + lVertDiePitchX);
		lStartRow = labs(m_lStart_Y - lWaferUpperLeftY)/(lVertDiePitchY + lHoriDiePitchY);

		m_WaferMapWrapper.SetAlgorithmParameter("StartX", m_lStart_X);
		m_WaferMapWrapper.SetAlgorithmParameter("StartY", m_lStart_Y);
		m_WaferMapWrapper.SetPhysicalPosition(lStartRow, lStartCol, m_lStart_X, m_lStart_Y);
		m_WaferMapWrapper.SetStartPosition(lStartRow, lStartCol);
	}

	return TRUE;
}

LONG CWaferTable::AutoAlignStandardWafer(IPC_CServiceMessage &svMsg)
{	//	426TX	1
	BOOL bReply = FALSE;
	BOOL bAlignResult = FALSE;
	CString szTitle, szContent;

	m_dStartWaferAlignTime = GetTime();
	CMSLogFileUtility::Instance()->MS_LogOperation("manual AutoAlign Wafer start");
	szTitle.LoadString(HMB_WT_ALIGN_WAFER);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	// prescan relative code
	SetAlignmentStatus(FALSE);

	CreateDummyMapForPrescanPicknPlace();	// auto align button

	//Check SCN file is loaded or not
	if (m_bFullRefBlock == FALSE || pUtl->GetPrescanRegionMode() )	//Block2
	{
		SetupSubRegionMode();		// auto align button
	}

	//Do not pop message when Auto search home is enable
	if (m_bFullRefBlock == FALSE)	//Block2
	{
		if (AutoLoadRankIDFile() != TRUE ||
			IsMapLoadingOk() != TRUE )
		{
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	//Check Wafermap Lot ID when TCP/IP is enabled
	if ( m_bUseTcpIp == TRUE )
	{
		CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	
		if ( pAppMod->m_bHostLotIDMatch == FALSE )
		{
			szContent.LoadString(HMB_WT_LOTID_NOT_MATCH);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);

			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}
	if (!m_WaferMapWrapper.IsMapValid())
	{
		HmiMessage("Map is invalid!\nPlease check", "Align Wafer Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 450, 180, NULL, NULL, NULL, NULL);
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	((CMS896AApp*)AfxGetApp())->m_bOnBondMode = TRUE;	//ANDREW_SC

	SetJoystickOn(FALSE);
	bAlignResult = FindWaferHomeDie();	// press auto align button, std machine.

	if ( bAlignResult && (IsBlkFuncEnable() == FALSE) )
	{
		ResetMapStauts();
		m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
		DisplayNextDirection(m_ulAlignRow, m_ulAlignCol);				//v3.01T1

		SetCtmRefDieOptions();			//v3.03

		//Refresh wafermap
		m_WaferMapWrapper.Redraw();
	}

	//v3.31
	//Remember 1st auto-align posn for OptoTech
	if (bAlignResult && IsBlkFunc1Enable() )
	{
		GetEncoder(&m_lBlkAutoAlignPosX, &m_lBlkAutoAlignPosY, &m_lBlkAutoAlignPosT);
		m_WaferMapWrapper.GetCurrentPosition(m_ulBlkAutoAlignRow, m_ulBlkAutoAlignCol);
	}

	if (m_bFullRefBlock == FALSE)	//Block2
	{
		m_WaferMapWrapper.ResetGrade();		//v2.56
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoAlign Wafer stop");		//v3.55
	bReply = bAlignResult;
	svMsg.InitMessage(sizeof(BOOL), &bReply);

	((CMS896AApp*)AfxGetApp())->m_bOnBondMode = FALSE;
	SetJoystickOn(TRUE);

	return 1;
}

LONG CWaferTable::SetWftHomeDiePosition(IPC_CServiceMessage& svMsg)
{
	if (!m_fHardware || m_bDisableWT)
	{	
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( IsWT2InUse() )
	{
		HmiMessage("Please use main table to setup this position");
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetJoystickOn(FALSE);
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X);		
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y);

	XY_SafeMoveTo(m_lHomeDieWftPosnX, m_lHomeDieWftPosnY);

	SetJoystickOn(TRUE);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferTable::ConfirmWftHomeDiePosition(IPC_CServiceMessage& svMsg)
{
	LONG lThetaPosition = 0;
	BOOL bReturn=TRUE;

	if (!m_fHardware || m_bDisableWT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	if( IsWT2InUse() )
	{
		HmiMessage("Please use main table to setup this position");
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	try
	{
		GetEncoderValue();

		m_lHomeDieWftPosnX = GetCurrX();
		m_lHomeDieWftPosnY = GetCurrY();

		//SaveWaferTblData();
		SaveData();

		//Restore original limit 
		SetJoystickOn(FALSE);
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);		


		SetJoystickOn(TRUE);

		SetStatusMessage("Wafer home die position is set");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
		bReturn = FALSE;
	}	

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	SetStatusMessage("WT HomeDie position is set");

	return TRUE;
}


LONG CWaferTable::FindWaferAngle(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if( (IsPrescanBlkPick() || IsBlkFuncEnable()) )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bWaferLoader = (BOOL) ((LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"]);
	if( bWaferLoader==FALSE && m_bAutoSrchHome )
	{
		StartLoadingAlert();
		SetJoystickOn(FALSE);
		//Move to HomeDie Position
		LONG lCurrentX, lCurrentY;
		lCurrentX = m_lHomeDieWftPosnX;
		lCurrentY = m_lHomeDieWftPosnY;
		if( m_bNewGTPosition )
		{
				lCurrentX = m_lNewGTPositionX;
				lCurrentY = m_lNewGTPositionY;
		}
		if( IsWT2InUse() )
		{
			lCurrentX = lCurrentX + m_lWT2OffsetX;
			lCurrentY = lCurrentY + m_lWT2OffsetY;
		}
		if (XY_SafeMoveTo(lCurrentX, lCurrentY))
		{
		}
		else 
		{
			return FALSE;
		}

		bReturn = FindGlobalAngle();
		SetJoystickOn(TRUE);
		CloseLoadingAlert();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::IsAutoAlignElCornerWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bIs = m_bAutoSrchHome;

	//v4.46T9	//PLLM REBEL U2U wafer debugging
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && m_bAutoSrchHome &&
		 (m_bSearchHomeOption == WT_SPIRAL_SEARCH))	
	{
		m_bAutoSrchHome = FALSE;	//U2U wafer do not use AutoSearchHome option
		bIs = m_bAutoSrchHome;
	}

	//2018.5.23
	if(bIs && CMS896AStn::m_bMS90HalfSortMode && CMS896AStn::m_b2PartsAllDone==FALSE && (CMS896AStn::m_bSortGoingTo2ndPart || CMS896AStn::m_b2Parts1stPartDone) )
	{
		//if press "AutoAlign" button, it can do auto-align
		m_b2PartsSortAutoAlignWay	= TRUE;
	}

	if (bIs)
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		pSRInfo->SetManualAlignRegion(FALSE);
	}
	svMsg.InitMessage(sizeof(BOOL), &bIs);
	return 1;
}


LONG CWaferTable::AutoRegionAlignBlock(IPC_CServiceMessage& svMsg)
{
	LONG lAlnStatus = -1;

	if( IsRegionAlignRefBlk() )
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		pSRInfo->SetRegionAligned(FALSE);	// for target region finder
		if( IsAlignedWafer() )
			lAlnStatus = 1;
		else
			lAlnStatus = 0;
		if( CheckEjectorPinLifeB4Start()==FALSE )
		{
			lAlnStatus = 0;
		}
	}

	svMsg.InitMessage(sizeof(LONG), &lAlnStatus);

	return 1;
}

BOOL CWaferTable::SpiralSearchRegionReferDie(CONST LONG lMaxLoopCount, LONG lReferPRID)
{
	LONG		lOrigX, lOrigY, lOrigT;
	LONG		lUpIndex = 0;
	LONG		lDnIndex = 0;
	LONG		lLtIndex = 0;
	LONG		lRtIndex = 0;
	LONG		lRow, lCol;
	LONG		lCurrentLoop = 1;
	LONG		lCurrentIndex = 0;
	LONG		lTotalIndex = 0;

	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lX, lY, lDiff_X, lDiff_Y;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	CString szMessage;
	LONG lRefDieLrn = (*m_psmfSRam)["WaferPr"]["RefDie"]["Count"];
	if ( lRefDieLrn == 0 )
	{
		szMessage.LoadString(HMB_WT_NO_REF_LEARNT);
		HmiMessage_Red_Back(szMessage, "Region Prescan");
		SetErrorMessage(szMessage);
		return FALSE;
	}
	if( lReferPRID>lRefDieLrn )
	{
		szMessage = "Scn Check reference die is not learnt!";
		HmiMessage_Red_Back(szMessage, "Region Prescan");
		SetErrorMessage(szMessage);
		return FALSE;
	}

	//Get current position
	GetEncoder(&lOrigX, &lOrigY, &lOrigT);
	lX = lOrigX;
	lY = lOrigY;

	CString szMsg;
	szMsg.Format("spiral search refer die 1st try %d(%d)", lReferPRID, lRefDieLrn);
	WT_SpecialLog(szMsg);

	//Search from current position	
	if (SearchAndAlignReferDie(lReferPRID))
	{
		return TRUE;
	}


	LONG lLFSizeX = 1, lLFSizeY = 1;
	if ( IsLFSizeOK() )
	{
		lLFSizeX = 3;
		lLFSizeY = 3;
	}

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));//Leo 20181213
	lLFSizeX = pWaferPr->GetSortFovCol();//GetMapIndexStepCol();
	lLFSizeY = pWaferPr->GetSortFovRow();//GetMapIndexStepRow();
	if( lLFSizeX%2==0 )
		lLFSizeX--;
	if( lLFSizeY%2==0 )
		lLFSizeY--;
	if( lLFSizeX<=0 )
		lLFSizeX = 1;
	if( lLFSizeY<=0 )
		lLFSizeY = 1;

	szMsg.Format("Spiral search region refer die %d,%d loop %d", lLFSizeX, lLFSizeY, lMaxLoopCount);
	WT_SpecialLog(szMsg);

	lTotalIndex = 0;
	for (lCurrentLoop=1; lCurrentLoop<=lMaxLoopCount; lCurrentLoop++)
	{
		lRow = (lCurrentLoop*2 + 1);
		lCol = (lCurrentLoop*2 + 1);

		WT_SpecialLog("loop find refer right");
		//Move to RIGHT & PR search on current die
		lTotalIndex++;
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1*lLFSizeX;
		lDiff_Y = 0*lLFSizeY;
		lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		XY_SafeMoveTo(lX, lY);
		Sleep(20);

		if (SearchAndAlignReferDie(lReferPRID))
		{
			return TRUE;
		}

		if( pApp->IsStopAlign() )
		{
			return FALSE;
		}

		WT_SpecialLog("loop find refer to up");
		//Move to UP & PR search on current die
		lUpIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;
			lTotalIndex++;

			WT_SpecialLog("loop find refer to up loop");
			//Move table to UP
			lDiff_X = 0*lLFSizeX;
			lDiff_Y = -1*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(20);

			if (SearchAndAlignReferDie(lReferPRID))
			{
				return TRUE;
			}	
			Sleep(20);
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}			

		//Move to LEFT & PR search on current die
		WT_SpecialLog("loop find refer left");
		lLtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;
			lTotalIndex++;

			WT_SpecialLog("loop find refer to left loop");
			//Move table to LEFT
			lDiff_X = -1*lLFSizeX;
			lDiff_Y = 0*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(20);

			if (SearchAndAlignReferDie(lReferPRID))
			{
				return TRUE;
			}	
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}


		//Move to DOWN & PR search on current die
		WT_SpecialLog("loop find refer to down");
		lDnIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;
			lTotalIndex++;

			WT_SpecialLog("loop find refer to down loop");
			//Move table to DN
			lDiff_X = 0*lLFSizeX;
			lDiff_Y = 1*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(100);

			if (SearchAndAlignReferDie(lReferPRID))
			{
				return TRUE;
			}	
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}


		//Move to right & PR search on current die
		WT_SpecialLog("loop find refer right");
		lRtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;
			lTotalIndex++;

			//Move table to RIGHT
			WT_SpecialLog("loop find refer to right loop");
			lDiff_X = 1*lLFSizeX;
			lDiff_Y = 0*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(20);

			if (SearchAndAlignReferDie(lReferPRID))
			{
				return TRUE;
			}
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}
	}

	WT_SpecialLog("loop find refer move back");
	XY_SafeMoveTo(lOrigX, lOrigY);

	return FALSE;
}

BOOL CWaferTable::SearchAndAlignReferDie(LONG lReferPRID)
{
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;
	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;

	//Klocwork
	stInfo.bFullDie = FALSE;
	stInfo.bStatus = FALSE;
	stInfo.lX = 0;
	stInfo.lY = 0;

	LONG lRefDieLrn = (*m_psmfSRam)["WaferPr"]["RefDie"]["Count"];
	LONG lDieNo;
	LONG lRefDieStart = 1;
	LONG lRefDieStop  = lRefDieLrn;
	if( lReferPRID>0 )
	{
		lRefDieStart = lRefDieStop  = lReferPRID;
	}

	CString szMsg;
	for ( lDieNo=lRefDieStart; lDieNo<=lRefDieStop; lDieNo++ )
	{
		szMsg.Format("search refer die pr id %d in FOV", lDieNo);
		WT_SpecialLog(szMsg);
		// 1.  Find any GOOD REFER die under FOV
		stSrchInfo.bShowPRStatus	= FALSE;
		stSrchInfo.bNormalDie		= FALSE;
		stSrchInfo.lRefDieNo		= lDieNo;
		stSrchInfo.bDisableBackupAlign = TRUE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDieInFOV_Spiral", stMsg);	//Use whole FOV for searching

		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if ((!stInfo.bStatus) || (!stInfo.bFullDie))		//If no GOOD/REF die is found
			continue;
		else
			break;
	}

	BOOL bFindRef = TRUE;
	if ((!stInfo.bStatus) || (!stInfo.bFullDie))		//If no GOOD/REF die is found
	{
		bFindRef = FALSE;
		WT_SpecialLog("to find a normal good die at current");
		// 1.  Find GOOD die under CURRENT DEFAULT WINDOW
		stSrchInfo.bShowPRStatus	= FALSE;
		stSrchInfo.bNormalDie		= TRUE;		//Normal
		stSrchInfo.lRefDieNo		= 1;
		stSrchInfo.bDisableBackupAlign = FALSE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if ((!stInfo.bStatus) || (!stInfo.bFullDie))		//If no GOOD/REF die is found
		{
			return FALSE;
		}
	}

	WT_SpecialLog("do compensation as die found");
	//Do PR compensation
	LONG lCurrX, lCurrY, lCurrT;
	GetEncoder(&lCurrX, &lCurrY, &lCurrT);
	lCurrX	+= stInfo.lX;	
	lCurrY	+= stInfo.lY;

	XY_SafeMoveTo(lCurrX, lCurrY);
	X1_Sync();
	Y1_Sync();
	Sleep(20);

	return bFindRef;
}



LONG CWaferTable::WT_GetNewGTPosition(IPC_CServiceMessage& svMsg)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
		BOOL bEnable;
	} NewGTPosition;
 	NewGTPosition stPos;

	stPos.bEnable = m_bNewGTPosition;
	
	if(m_bNewGTUse2 == 0)
	{
		stPos.lX = m_lNewGTPositionX;
		stPos.lY = m_lNewGTPositionY;
	}
	else
	{
		stPos.lX = m_lNewGTPositionX2;
		stPos.lY = m_lNewGTPositionY2;
	}
	if( IsWT2InUse() )
	{
		stPos.lX = stPos.lX + m_lWT2OffsetX;
		stPos.lY = stPos.lY + m_lWT2OffsetY;
	}
	//CString szTemp;
	//szTemp.Format("enable,%d,X1,%d,Y1,%d,X2,%d,Y2,%d",m_bNewGTUse2,m_lNewGTPositionX,m_lNewGTPositionY,m_lNewGTPositionX2,m_lNewGTPositionY2);
	//HmiMessage(szTemp);
	svMsg.InitMessage(sizeof(NewGTPosition), &stPos);

	return 1;
}

LONG CWaferTable::WT_SetNewGTPosition(IPC_CServiceMessage& svMsg)
{
	if (!m_fHardware || m_bDisableWT)
	{	
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( IsWT2InUse() )
	{
		HmiMessage_Red_Back("Please setup this positin with main table!", "New GT");
		BOOL bReturn=TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	SetJoystickOn(FALSE);
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X);		
	CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y);

	if (m_bNewGTUse2 == 0)
		XY_SafeMoveTo(m_lNewGTPositionX, m_lNewGTPositionY);
	else
		XY_SafeMoveTo(m_lNewGTPositionX2, m_lNewGTPositionY2);

	//CString szTemp;
	//szTemp.Format("enable,%d,X1,%d,Y1,%d,X2,%d,Y2,%d",m_bNewGTUse2,m_lNewGTPositionX,m_lNewGTPositionY,m_lNewGTPositionX2,m_lNewGTPositionY2);
	//HmiMessage(szTemp);

	SetJoystickOn(TRUE);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return TRUE;
}

LONG CWaferTable::WT_ConfirmNewGTPosition(IPC_CServiceMessage& svMsg)
{
	LONG lThetaPosition = 0;
	BOOL bReturn=TRUE;

	if (!m_fHardware || m_bDisableWT)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if( IsWT2InUse() )
	{
		HmiMessage_Red_Back("Please setup this positin with main table!", "New GT");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	try
	{
		GetEncoderValue();

		if (m_bNewGTUse2 == 0)
		{
			m_lNewGTPositionX = GetCurrX();
			m_lNewGTPositionY = GetCurrY();
		}
		else
		{
			m_lNewGTPositionX2 = GetCurrX();
			m_lNewGTPositionY2 = GetCurrY();
		}
	  //CString szTemp;
	//	szTemp.Format("enable,%d,X1,%d,Y1,%d,X2,%d,Y2,%d",m_bNewGTUse2,m_lNewGTPositionX,m_lNewGTPositionY,m_lNewGTPositionX2,m_lNewGTPositionY2);
	//	HmiMessage(szTemp);
		//SaveWaferTblData();
		SaveData();		//V4.50a2

		//Restore original limit 
		SetJoystickOn(FALSE);
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X);		
		CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y);		
		SetJoystickOn(TRUE);

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
		bReturn = FALSE;
	}	

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	SetStatusMessage("Wafer new global theta position is set");

	return TRUE;
}



