/////////////////////////////////////////////////////////////////
// WL_AlignWafer.cpp : Common functions of the CWaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
//	Revision:	1.00
//
//	By:			BarryChu
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "MS896A.h"

#include <prheader.h>
#include <prmsg.h>
#include <prstatus.h>
#include <prtype.h>
#include "WaferPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




BOOL CWaferLoader::FindFirstDie(LONG *lPosX, LONG *lPosY)
{
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	BOOL	bReturn = FALSE;	
	LONG	lPosT = 0;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "FindFirstDieInWafer", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 36000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);

			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			GetWaferTableEncoder(lPosX, lPosY, &lPosT);
			break;
		}
		else
		{
			Sleep(2);
		}
	}

	DOUBLE dFOVX	= (*m_psmfSRam)["WaferPr"]["FOV"]["X"];
	DOUBLE dFOVY	= (*m_psmfSRam)["WaferPr"]["FOV"]["Y"];
	DOUBLE dFov = min(dFOVX, dFOVY);
	if( IsAOIOnlyMachine() && dFov<2.5 && bReturn==FALSE )
	{
		SaveScanTimeEvent("  WFL: Find first Die, small FOV,move again");
		for(int i=0; i<2; i++)
		{
			if( i==0 )
				*lPosX = *lPosX + GetDiePitchX_X()/2;
			else
				*lPosY = *lPosY + GetDiePitchY_Y()/2;
			MoveWaferTable(*lPosX, *lPosY);
			Sleep(100);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "FindFirstDieInWafer", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 36000000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);

					stMsg.GetMsg(sizeof(BOOL), &bReturn);
					GetWaferTableEncoder(lPosX, lPosY, &lPosT);
					break;
				}
				else
				{
					Sleep(2);
				}
			}
			if( bReturn )
			{
				break;
			}
		}
	}
	
	return bReturn;
}


BOOL CWaferLoader::CheckFirstDieAngle()
{
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	BOOL	bReturn = FALSE;	
	LONG	lPosT = 0;
	BOOL	bJustCheck = FALSE;

	if (m_bSearchHomeOption == WL_SPIRAL_SEARCH)
	{
		bJustCheck = TRUE;
	}

	stMsg.InitMessage(sizeof(BOOL), &bJustCheck);

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "FindFirstDieAngleInWafer", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);

			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(2);
		}
	}
	
	return bReturn;
}


BOOL CWaferLoader::CorrectWaferAngle(LONG *lPosX, LONG *lPosY)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	BOOL	bReturn = FALSE;	
	LONG	lPosT = 0;

	typedef struct 
	{
		LONG	lLHS;
		LONG	lRHS;
		LONG	lAlignMode;		//v3.02T6
	} ALNSETUP;

	ALNSETUP stInfo;

	LONG	lCurrentX = *lPosX;
	LONG	lCurrentY = *lPosY;

	//Move to HomeDie Position
	typedef struct
	{
		LONG lX;
		LONG lY;
		BOOL bEnable;
	} NewGTPosition;
 	NewGTPosition stPos;

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "WT_GetNewGTPosition", stMsg);
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

	stMsg.GetMsg(sizeof(NewGTPosition), &stPos);

	if( stPos.bEnable )
	{
		lCurrentX = stPos.lX;
		lCurrentY = stPos.lY;
	}


	//v4.41T4	//Nichia
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bScanAlignFromFile = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["ScanAlignFromFile"]; 
	if (pApp->GetCustomerName() == CTM_NICHIA	&& 
		pApp->GetProductLine() == _T("")		&&		//v4.59A34
		bScanAlignFromFile)
	{
		CString szBasePt = CMS896AStn::m_oNichiaSubSystem.GetMapBasePt();

		LONG lX = lCurrentX;
		LONG lY = lCurrentY;

		if (szBasePt == "B")		//Left partial wafer
		{
//AfxMessageBox("WL: Nichia Type B parital wafer to LEFT ....", MB_SYSTEMMODAL);
			lX = lX + CMS896AStn::m_oNichiaSubSystem.GetMapBasePtColWidth() * GetDiePitchX_X();
			lY = lY + CMS896AStn::m_oNichiaSubSystem.GetMapBasePtColWidth() * GetDiePitchX_Y();
		}
		else if (szBasePt == "D")	//Right partial wafer
		{
//AfxMessageBox("WL: Nichia Type D parital wafer to RIGHT ....", MB_SYSTEMMODAL);
			lX = lX - CMS896AStn::m_oNichiaSubSystem.GetMapBasePtColWidth() * GetDiePitchX_X();
			lY = lY - CMS896AStn::m_oNichiaSubSystem.GetMapBasePtColWidth() * GetDiePitchX_Y();
		}
		else						//For other BasePt options, use center TEG pattern for alignment
		{
//AfxMessageBox("WL: Nichia other parital wafer to CENTER ....", MB_SYSTEMMODAL);
		}

		MoveWaferTable(lX, lY);
		lCurrentX = lX;
		lCurrentY = lY;
	}
	else
	{
		MoveWaferTable(lCurrentX, lCurrentY);
	}
	Sleep(100);


	//Align wafer angle
	CMSLogFileUtility::Instance()->WL_LogStatus("Correct wafer angle Functition");

	// Set Index parameter
	stInfo.lLHS = m_lAngleLHSCount;
	stInfo.lRHS = m_lAngleRHSCount;
	stInfo.lAlignMode = m_lAlignWaferAngleMode;		// 0=Horizontal, 1=Vertical		//v3.02T7
	stMsg.InitMessage(sizeof(ALNSETUP), &stInfo);

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AlignWaferAngle", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 36000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);

			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			GetWaferTableEncoder(&lCurrentX, &lCurrentY, &lPosT);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn)
	{
		if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
		{
			//xyz
			//Align & rotate current die in case there is still theta angular offset
			CMSLogFileUtility::Instance()->WL_LogStatus("Check global angle");
			if (CheckFirstDieAngle() == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Global angle check is failed!");
				SetErrorMessage("Global angle check is failed!");

				HmiMessage_Red_Yellow("Wafer Alignment: Global wafer angle is out!");
				return FALSE;
			}
		}
	}
	
	if( bReturn && stPos.bEnable )
	{
		lCurrentX = *lPosX;
		lCurrentY = *lPosY;
		MoveWaferTable(lCurrentX, lCurrentY);
		Sleep(10);
		//Locate any die in the wafer table
		CMSLogFileUtility::Instance()->WL_LogStatus("Find Wafer Angle Finding die in wafer again");
	}

	return bReturn;
}


BOOL CWaferLoader::FindWaferGlobalAngle(CONST BOOL bFullAuto)
{
	IPC_CServiceMessage stMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	BOOL bFindGT = TRUE;
	int		nConvID = 0;
	BOOL	bReturn	= FALSE;	

	if (CMS896AApp::m_bEnableGripperLoadUnloadTest == TRUE)
	{
		Sleep(500);
		return TRUE;
	}
	//1115
	bFindGT = TRUE;
	//if( bFullAuto==FALSE )
	//{
	//	bFindGT = (BOOL)(LONG)((*m_psmfSRam)["MS896A"]["FindGlobalTheta"]);
	//}
	//Align wafer angle
	LONG	lCurrentX = 0;
	LONG	lCurrentY = 0;

	CString szMsg;
	if( m_bUseContour )
	{
		bFindGT = (BOOL)(LONG)((*m_psmfSRam)["MS896A"]["FindGlobalTheta"]);
		if( IsWT2InUse() )
		{
			lCurrentX = m_lWL2CenterX;
			lCurrentY = m_lWL2CenterY;
			szMsg.Format(" WT2 contour center %d,%d", lCurrentX, lCurrentY);
		}
		else
		{
			lCurrentX = m_lWL1CenterX;
			lCurrentY = m_lWL1CenterY;
			szMsg.Format(" WT1 contour center %d,%d", lCurrentX, lCurrentY);
		}
	}
	else
	{
		if( IsWT2InUse() )
		{
			GetWT2HomeDiePhyPosn(lCurrentX, lCurrentY);
			szMsg.Format(" WT2 home center %d,%d", lCurrentX, lCurrentY);
		}
		else
		{
			GetHomeDiePhyPosn(lCurrentX, lCurrentY);
			szMsg.Format(" WT1 home center %d,%d", lCurrentX, lCurrentY);
		}
	}
	MoveWaferTable(lCurrentX, lCurrentY);
	SaveScanTimeEvent("  WFL: rotate WFT");
//	Sleep(50);
	RotateWFTTUnderCam( IsWT2InUse() );

	SaveScanTimeEvent("  WFL: Find GT " + szMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus("Find GT " + szMsg);
	if( bFindGT )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Find GT Correct wafer angle");

		if (CorrectWaferAngle(&lCurrentX, &lCurrentY) == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Cannot correct wafer angle");
			SetErrorMessage("Cannot correct wafer angle");

			SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
			return FALSE;
		}
	}
	else
	{
		//Locate any die in the wafer table
		CMSLogFileUtility::Instance()->WL_LogStatus("Find Home Die Finding die in wafer");
		SaveScanTimeEvent("  WFL: Find first Die in wafer");
		if (FindFirstDie(&lCurrentX, &lCurrentY) == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Cannot find any die in wafer");
			SetErrorMessage("Cannot find any die in wafer");

			SetAlert_Red_Yellow(IDS_WL_NODIE_IN_WAFER);
			return FALSE;
		}
	}

	if( IsScanAlignWafer() )
	{
		CString szMsg;
		szMsg = "  WFL: Scan align, find a normal first after GT done";
		//Locate any die in the wafer table
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		SaveScanTimeEvent(szMsg);
		if (FindFirstDie(&lCurrentX, &lCurrentY) == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Cannot find any die in wafer");
			SetErrorMessage("Cannot find any die in wafer");

			SetAlert_Red_Yellow(IDS_WL_NODIE_IN_WAFER);
			return FALSE;
		}

		BOOL bReferOnNormal = TRUE;
		IPC_CServiceMessage stMsg;
		int	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "CounterCheckReferOnNormalDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);		
			}
		}
		stMsg.GetMsg(sizeof(BOOL), &bReferOnNormal);
		if( bReferOnNormal==FALSE )
		{
			CString szTitle, szContent;
			szContent = "Normal die can be recognized as refer die.\nPlease re-learn refer die again!";
			szTitle.LoadString(HMB_WT_ALIGN_DIE_FAILED);
			HmiMessage_Red_Back(szContent, szTitle);
			CString szMsg;
			szMsg = "Auto Align Wafer fails because normal die recognized as refer.";
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

			return FALSE;
		}
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

BOOL CWaferLoader::AutoLearnAdaptWafer()
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bReturn);
	if( (CMS896AStn::m_bEnablePRAdaptWafer == FALSE) || (m_bAutoAdpatWafer == FALSE) )
	{
		return TRUE;
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("load auto adapt wafer");
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "AdaptWaferInitHighLevelVisionInfo", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if( bReturn==FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("PR Auto Adapt Wafer failed\n");
		SetErrorMessage("PR Auto Adapt Wafer failed\n");
		SetAlert_Red_Yellow(IDS_WL_ALIGNWAF_FAIL);
	}

	return bReturn;
}


BOOL CWaferLoader::FindHomeDie(CONST BOOL bFullAuto)
{
	IPC_CServiceMessage stMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	int		nConvID = 0;
	BOOL	bReturn	= FALSE;	

	if( m_bUseContour && IsPrescanMapIndex()==FALSE && pApp->GetCustomerName()=="Yealy" )
	{
		return TRUE;
	}

	//v4.01	//Do not perform HOME die search in semi-Auto mode for Lumileds
	if ( IsWLSemiAutoMode() && (pApp->GetCustomerName()=="Lumileds") )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Find home die finish (Semi-AUTO)");
		return TRUE;
	}

	if( IsFastHomeScan() )		// align wafer
	{
		SaveScanTimeEvent("  WFL: fast home map index, no home die finding action");
		return TRUE;
	}


	//2019.1.18 it should classified as two case with reference die and no reference die
	if ((IsScanAlignWafer() == SCAN_ALIGN_WAFER_WITHOUT_REF) && !pUtl->GetPrescanRegionMode())		// no need to find refer die, normal die as home die.
	{
		SaveScanTimeEvent("  WFL: map dummy scan align, no home die finding action");
		return TRUE;
	}

	if (IsBurnIn())
	{
		return TRUE;
	}

	if ( m_bSearchHomeOption == WL_SPIRAL_SEARCH )
	{
		//Do sprial search
		CMSLogFileUtility::Instance()->WL_LogStatus("Spiral Search Start");
		
		stMsg.InitMessage(sizeof(LONG), &m_lSprialSize);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SpiralSearchHomeDie", stMsg);
	}
	else
	{
		//Do cornerDie search
		CMSLogFileUtility::Instance()->WL_LogStatus("Corner Search Start");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Find home before corner die found");
		stMsg.InitMessage(sizeof(ULONG), &m_ulCornerSearchOption);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "CornerSearchHomeDie", stMsg);
	}

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 36000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(2);	
		}
	}

	// for cree with joystick to locate home die 
	if( bReturn==FALSE && (pApp->GetCustomerName()=="Cree") )
	{
		LONG lReturn ;
		CString szContent, szTitle;
		szTitle.LoadString(HMB_WT_ALIGN_WAFER);
		szContent.LoadString(HMB_WT_SET_ALIGN_POS);

		SetWaferTableJoystickSpeed(1);
		SetWaferTableJoystick(TRUE);
		lReturn = HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		SetWaferTableJoystick(FALSE);

		if ( lReturn == glHMI_CONTINUE)
		{
			LONG	lCurrentX = 0;
			LONG	lCurrentY = 0;
			if (FindFirstDie(&lCurrentX, &lCurrentY) == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Cannot find any die in wafer");
				SetErrorMessage("Cannot find any die in wafer");

				SetAlert_Red_Yellow(IDS_WL_NODIE_IN_WAFER);
				return FALSE;
			}

			if ( m_bSearchHomeOption == WL_SPIRAL_SEARCH )
			{
				//Do sprial search
				CMSLogFileUtility::Instance()->WL_LogStatus("CREE Spiral Search Start");
				stMsg.InitMessage(sizeof(LONG), &m_lSprialSize);
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SpiralSearchHomeDie", stMsg);
			}
			else
			{
				//Do cornerDie search
				CMSLogFileUtility::Instance()->WL_LogStatus("Corner Search Start");
				stMsg.InitMessage(sizeof(ULONG), &m_ulCornerSearchOption);
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "CornerSearchHomeDie", stMsg);
			}

			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 36000000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bReturn);
					break;
				}
				else
				{
					Sleep(2);	
				}
			}
		}
	} // for cree with joystick to locate home die 

	if (bReturn == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Search Home die failed");
		SetErrorMessage("Search Home die failed");
		SetAlert_Red_Yellow(IDS_WL_SRCH_HOMEDIE_FAIL);
		return FALSE;
	}

	LONG lX, lY, lT;
	GetWaferTableEncoder(&lX, &lY, &lT);
	CString szMsg;
	szMsg.Format("  WFL: Find home die finish at WFT (%d,%d)", lX, lY);
	SaveScanTimeEvent(szMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	return TRUE;
}

BOOL CWaferLoader::AlignInputWafer(BOOL bFullAuto)
{
	//Perform auto wafer alignment
	BOOL bReturn;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BOOL), &bFullAuto);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AlignInputWafer", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}

	return bReturn;
}