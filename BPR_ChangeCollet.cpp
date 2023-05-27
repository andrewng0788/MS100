/////////////////////////////////////////////////////////////////
// WPR_Common_Ejt.cpp : Common functions of the CWaferPr class for EJT
//
//	Description:
//		MS100 Mapping Die Sorter
//
//	Date:		Tuesday, June 3, 2015
//	Revision:	1.00
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2004-2015.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "MS896A.h"
#include "BondPr.h"
#include "FileUtil.h"
#include "PrCrossHairDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//================================================================================
//  Holder Upload Collet PR
//================================================================================
LONG CBondPr::AGC_SearchUploadColletPR(IPC_CServiceMessage &svMsg)
{
	typedef struct {
		BOOL	bStatus;
		INT		nOffsetX;
		INT		nOffsetY;
	} SRCH_CHOLE_RESULT;

	SRCH_CHOLE_RESULT stResult;
	stResult.bStatus = FALSE;
	stResult.nOffsetX = 0;
	stResult.nOffsetY = 0;

	LONG lDxInPixel = 0;
	LONG lDyInPixel = 0;

	if (AGC_SearchUploadColletPR(lDxInPixel, lDyInPixel))
	{
		lDxInPixel -= 4096;
		lDyInPixel -= 4096;
		stResult.bStatus = TRUE;
		CString szMsg;
		if ((m_dAGCCalibX != 0) && (m_dAGCCalibY != 0))		//v4.50A30
		{
			DOUBLE dDx = lDxInPixel * m_dAGCCalibX;	// in um
			DOUBLE dDy = lDyInPixel * m_dAGCCalibY;	// in um

			stResult.nOffsetX = (INT) dDx;
			stResult.nOffsetY = (INT) dDy;

			szMsg.Format("Collet hole is found - (%.2f, %.2f) um", dDx, dDy); 
		}
		else
		{
			stResult.nOffsetX = 0;
			stResult.nOffsetY = 0;
			szMsg.Format("Collet hole is found - (%ld, %ld) pixels", lDxInPixel, lDyInPixel);
		}

		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);		//v4.50A30
//		HmiMessage(szMsg);
	}
	else
	{
		stResult.bStatus = FALSE;
//		HmiMessage("Collet hole is not found");
	}

	svMsg.InitMessage(sizeof(SRCH_CHOLE_RESULT), &stResult);
	return 1;
}

BOOL CBondPr::AGC_SearchUploadColletPR(LONG& lDXInPixel, LONG& lDYInPixel)
{
	UINT nPosX = 0, nPosY = 0;
	lDXInPixel = 0;
	lDYInPixel = 0;
	
	PR_WORD rtnPR = AGC_AutoSearchEpoxy(m_ssPSPRSrchID[1], m_lLearnEpoxySize[1],
										m_lPrCenterX, m_lPrCenterY, 
										0, 0, 
										nPosX, nPosY);
	if (rtnPR != PR_TRUE)
	{
		return FALSE;
	}


	lDXInPixel = nPosX;
	lDYInPixel = nPosY;
	return TRUE;
}


LONG CBondPr::AGC_SearchColletHole(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

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

	if ( (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE) || (m_bDieCalibrated == FALSE) )
	{
		HmiMessage_Red_Yellow("Bond PR: no record ID found in SearchColletHole");
		svMsg.InitMessage(sizeof(SRCH_CHOLE_RESULT), &stResult);
		return 1;
	}

	LONG lDxInPixel=0, lDyInPixel=0;
	if (AGC_SearchColletHole(bBH2, lDxInPixel, lDyInPixel))
	{
		stResult.bStatus = TRUE;
		//stResult.nOffsetX = lDxInPixel;
		//stResult.nOffsetY = lDyInPixel;

		CString szMsg;
		if ((m_dAGCCalibX != 0) && (m_dAGCCalibY != 0))		//v4.50A30
		{
			DOUBLE dDx = lDxInPixel * m_dAGCCalibX;	// in um
			DOUBLE dDy = lDyInPixel * m_dAGCCalibY;	// in um

			stResult.nOffsetX = (INT) dDx;
			stResult.nOffsetY = (INT) dDy;

			szMsg.Format("Collet hole is found - (%.2f, %.2f) um", dDx, dDy); 
		}
		else
		{
			stResult.nOffsetX = 0;
			stResult.nOffsetY = 0;
			szMsg.Format("Collet hole is found - (%ld, %ld) pixels", lDxInPixel, lDyInPixel);
		}

		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);		//v4.50A30
		//HmiMessage(szMsg);
	}
	else
	{
		//HmiMessage("Collet hole is not found");
		stResult.bStatus = FALSE;
	}

	svMsg.InitMessage(sizeof(SRCH_CHOLE_RESULT), &stResult);
	return 1;
}


LONG CBondPr::AGC_SearchColletInTray(IPC_CServiceMessage &svMsg)
{
	BOOL bStatus = FALSE;
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	PR_COORD	stDieOffset; 

	PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

	LONG lOldZoomSensorMode = BPR_GetRunZoom();
	BPR_LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_1X);

	//Use Max search area to locate 1st die
	stOrgSrchCorner1.x = PR_MAX_WIN_ULC_X + 1000;
	stOrgSrchCorner1.y = PR_MAX_WIN_ULC_Y + 1000;
	stOrgSrchCorner2.x = PR_MAX_WIN_LRC_X - 1000;
	stOrgSrchCorner2.y = PR_MAX_WIN_LRC_Y - 1000;

	LONG lTrayIndex		= (*m_psmfSRam)["BinTable"]["AGC"]["ColletIndex"];
	lTrayIndex			= lTrayIndex - 1;

	//No inspection on here!
	LONG lResult = 0;
	lResult = ManualSearchDie(BPR_REFERENCE_DIE, 5, PR_TRUE, PR_TRUE, PR_FALSE, 
								&usDieType, &fDieRotate, &stDieOffset, &fDieScore,
								stOrgSrchCorner1, stOrgSrchCorner2, 0, 1);

	if ( lResult != -1 )
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			if (fabs(fDieRotate) < 10.0)
			{
				bStatus = TRUE;
			}
		}
	}
	BPR_LiveViewZoom(lOldZoomSensorMode);
	//v4.53A12
	CString szLog;
	szLog.Format("AGC : SearchColletInTray status = %d, usDieType = %d, Rotation = %.2f deg",
			bStatus, usDieType, fDieRotate);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	svMsg.InitMessage(sizeof(BOOL), &bStatus);
	return 1;
}

LONG CBondPr::AGC_ResetColletCentre(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	m_lAGCC1CenterXInPixel = 0;
	m_lAGCC1CenterYInPixel = 0;
	m_lAGCC2CenterXInPixel = 0;
	m_lAGCC2CenterYInPixel = 0;

	SavePrData();

	//HmiMessage("BPR: AGC Collet Center positions are reset");		//v4.53A21
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBondPr::AGC_SearchColletHole(CONST BOOL bBHZ2, LONG& lDXInPixel, LONG& lDYInPixel)
{
	//Reuse postseal camera channel for bin uplook camera
	BOOL bReturn = TRUE;
	
	UINT stDieOffsetx = 0;
	UINT stDieOffsety = 0;

	UINT nPosX=0, nPosY=0;
	
	if (bBHZ2)
	{
		nPosX = m_lAGCC2CenterXInPixel;
		nPosY = m_lAGCC2CenterYInPixel;
	}
	else
	{
		nPosX = m_lAGCC1CenterXInPixel;
		nPosY = m_lAGCC1CenterYInPixel;
	}

	PR_WORD rtnPR = AGC_AutoSearchEpoxy(m_ssPSPRSrchID[0], m_lLearnEpoxySize[0],
										m_lPrCenterX, m_lPrCenterY, 
										m_stGenDieSize[0].x, m_stGenDieSize[0].y, 
										nPosX, nPosY);

	if (rtnPR != PR_TRUE)
	{
		return FALSE;
	}

	CString szLog;
	LONG lOffsetX=0, lOffsetY=0;

	if (bBHZ2)
	{
		if ((m_lAGCC2CenterXInPixel == 0) && (m_lAGCC2CenterYInPixel == 0))
		{
			m_lAGCC2CenterXInPixel = nPosX;
			m_lAGCC2CenterYInPixel = nPosY;
			lOffsetX = 0;
			lOffsetY = 0;

			SavePrData();

			szLog.Format("AGC Collet2 pos XY updated to (%ld, %ld)", m_lAGCC2CenterXInPixel, m_lAGCC2CenterYInPixel);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.50A22
		}
		else
		{
			lOffsetX = m_lAGCC2CenterXInPixel - nPosX;
			lOffsetY = m_lAGCC2CenterYInPixel - nPosY;
			//v4.51A1
			m_lAGCC2CenterXInPixel = nPosX;
			m_lAGCC2CenterYInPixel = nPosY;
			
			szLog.Format("AGC Collet2 offset XY in pixel (%ld, %ld), new pos updated to (%ld, %ld)", 
				lOffsetX, lOffsetY, m_lAGCC2CenterXInPixel, m_lAGCC2CenterYInPixel);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.50A22
		}
	}
	else
	{
		if ((m_lAGCC1CenterXInPixel == 0) && (m_lAGCC1CenterYInPixel == 0))
		{
			m_lAGCC1CenterXInPixel = nPosX;
			m_lAGCC1CenterYInPixel = nPosY;
			lOffsetX = 0;
			lOffsetY = 0;

			SavePrData();

			szLog.Format("AGC Collet1 pos XY updated to (%ld, %ld)", m_lAGCC1CenterXInPixel, m_lAGCC1CenterYInPixel);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.50A22
		}
		else
		{
			lOffsetX = m_lAGCC1CenterXInPixel - nPosX;
			lOffsetY = m_lAGCC1CenterYInPixel - nPosY;
			//v4.51A1
			m_lAGCC1CenterXInPixel = nPosX;
			m_lAGCC1CenterYInPixel = nPosY;
			
			szLog.Format("AGC Collet1 offset XY in pixel (%ld, %ld), new pos updated to (%ld, %ld)", 
				lOffsetX, lOffsetY, m_lAGCC1CenterXInPixel, m_lAGCC1CenterYInPixel);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.50A22
		}
	}

	lDXInPixel = lOffsetX;
	lDYInPixel = lOffsetY;
	return bReturn;
}

PR_WORD CBondPr::AGC_AutoShowEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner, const LONG lLearnUPLPRIndex)
{
	PR_COMMON_RPY                 stCommonRpy;
	PR_PRE_EPOXY_SHOW_CMD         stEpoxyShowCmd;
	PR_PRE_EPOXY_SHOW_RPY2        stEpoxyShowRpy2;
	PR_COORD prcenter;
	prcenter.x = 4096;
	prcenter.y = 4096;
	PR_InitPreEpoxyShowCmd(&stEpoxyShowCmd);

	stEpoxyShowCmd.stWin.coCorner1 = stULCorner;//Input Window
	stEpoxyShowCmd.stWin.coCorner2 = stLRCorner;
	stEpoxyShowCmd.emIsGrabFromPurpose = PR_TRUE;
	stEpoxyShowCmd.aemPurpose[0] = (PR_PURPOSE)MS899_POSTSEAL_PR_PURPOSE; //Lighting Purpose;
	stEpoxyShowCmd.emSubType = (PR_OBJ_TYPE)PR_OBJ_TYPE_BGA_SUBSTRATE;// (Bright Background Dark Object)PR_OBJ_TYPE_HORIZONTAL_LEADFRAME,
	stEpoxyShowCmd.emEpoxyInspAlg = PR_SINGLE_DOT_WO_NOISE;
	// (Dark Background Bright Object)PR_OBJ_TYPE_BGA_SUBSTRATE;

	//stEpoxyShowCmd.emEpoxyInspAlg = (PR_EPOXY_INSP_ALG) PR_SINGLE_DOT_EPOXY_INSP;
	stEpoxyShowCmd.emEpoxySegMode = (PR_PRE_EPOXY_SEG_MODE) PR_PRE_EPOXY_SEG_MODE_MANUAL_THRES;
	stEpoxyShowCmd.emDisplayInfo = PR_DISPLAY_INFO_BINARY;

	stEpoxyShowCmd.coRefPoint = prcenter;//Image Center (4096,4096)

	LONG lThreshold =  126;
	stEpoxyShowCmd.ubThreshold = (PR_UBYTE)lThreshold;// Threshold (0--255)

	CString szContent = "Is this image OK?";
	CString szTitle   = "Show Epoxy";
	do 
    {
		stEpoxyShowCmd.ubThreshold = (PR_UBYTE)lThreshold;// Threshold (0--255);
		PR_PreEpoxyShowCmd(&stEpoxyShowCmd, PSPR_SENDER_ID, PSPR_RECV_ID, &stCommonRpy);
		if (stCommonRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCommonRpy.uwPRStatus))
		{
			CString szLog;
			szLog.Format("PR_PreEpoxyShowCmd Error1: COMM = %d, PR = %d", stCommonRpy.uwCommunStatus, stCommonRpy.uwPRStatus);
			HmiMessageEx(szLog);
			return PR_FALSE;
		}

		PR_PreEpoxyShowRpy(PSPR_SENDER_ID, &stEpoxyShowRpy2);
		if (stEpoxyShowRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stEpoxyShowRpy2.stStatus.uwPRStatus))
		{
			CString szLog;
			szLog.Format("PR_PreEpoxyShowRpy Error1: COMM = %d, PR = %d", stEpoxyShowRpy2.stStatus.uwCommunStatus, stEpoxyShowRpy2.stStatus.uwPRStatus);
			HmiMessageEx(szLog);
			return PR_FALSE;
		}

	} while (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_NO );

	PR_LRN_EPOXY_EXT_CMD          stLrnEpoxyCmd;
    PR_LRN_EPOXY_EXT_RPY2         stLrnEpoxyRpy2;

 
	PR_InitLrnEpoxyExtCmd(&stLrnEpoxyCmd);
	stLrnEpoxyCmd.emLatchMode = PR_LATCH_NO_LATCH;
	stLrnEpoxyCmd.emIsGrabFromPurpose = PR_TRUE;
	stLrnEpoxyCmd.aemPurpose[0] = stEpoxyShowCmd.aemPurpose[0];

	stLrnEpoxyCmd.emEpoxyAttribute = (stEpoxyShowCmd.emSubType == PR_OBJ_TYPE_HORIZONTAL_LEADFRAME) ? PR_OBJ_ATTRIBUTE_DARK : PR_OBJ_ATTRIBUTE_BRIGHT;
	stLrnEpoxyCmd.emEpoxyInspAlg = stEpoxyShowCmd.emEpoxyInspAlg;
	stLrnEpoxyCmd.emEpoxySegMode = stEpoxyShowCmd.emEpoxySegMode;
	stLrnEpoxyCmd.ubThreshold = stEpoxyShowCmd.ubThreshold;
	stLrnEpoxyCmd.leMax = stEpoxyShowCmd.leUpperLimit;
	stLrnEpoxyCmd.leMin = stEpoxyShowCmd.leLowerLimit;

	stLrnEpoxyCmd.stOpRegion.uwNumOfCorners = PR_NO_OF_CORNERS;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_LEFT] = stEpoxyShowCmd.stWin.coCorner1;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT] = stEpoxyShowCmd.stWin.coCorner2;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x = stEpoxyShowCmd.stWin.coCorner2.x;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y = stEpoxyShowCmd.stWin.coCorner1.y;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x = stEpoxyShowCmd.stWin.coCorner1.x;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y = stEpoxyShowCmd.stWin.coCorner2.y;
	stLrnEpoxyCmd.coRefPoint = prcenter;//Image Center (4096,4096)

    PR_LrnEpoxyExtCmd(&stLrnEpoxyCmd, PSPR_SENDER_ID, PSPR_RECV_ID, &stCommonRpy);
	stLrnEpoxyCmd.emEpoxyInspAlg = PR_SINGLE_DOT_WO_NOISE;

    if (stCommonRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCommonRpy.uwPRStatus))
    {
        CString szLog;
		szLog.Format("PR_LrnEpoxyExtCmd Error1: COMM = %d, PR = %d", stCommonRpy.uwCommunStatus, stCommonRpy.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
    }
    PR_LrnEpoxyExtRpy(PSPR_SENDER_ID, &stLrnEpoxyRpy2);
	m_ssPSPRSrchID[lLearnUPLPRIndex]	= stLrnEpoxyRpy2.uwRecordID;
	m_lLearnEpoxySize[lLearnUPLPRIndex] = stLrnEpoxyRpy2.aeEpoxyArea;
	CString szMsg;
	szMsg.Format("LearnEpoxySize:%d",m_lLearnEpoxySize[lLearnUPLPRIndex]);
	SaveColletSizeEvent(szMsg);
    if (stLrnEpoxyRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stLrnEpoxyRpy2.stStatus.uwPRStatus))
    {
        CString szLog;
		szLog.Format("PR_LrnEpoxyExtRpy Error1: COMM = %d, PR = %d", stLrnEpoxyRpy2.stStatus.uwCommunStatus, stLrnEpoxyRpy2.stStatus.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
    }

	return PR_TRUE;
}

BOOL CBondPr::AutoLearnPreBondEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner)
{
	PR_COORD prcenter;
	prcenter.x = (PR_WORD)GetPrCenterX();
	prcenter.y = (PR_WORD)GetPrCenterY();

	LONG lThreshold				=  126;
	PR_PRE_EPOXY_SHOW_CMD         stEpoxyShowCmd;

	CString szContent = "Is this image OK?";
	CString szTitle   = "BPR Show Epoxy";

	CString szErrMsg;

	do 
    {
		if (!m_pPrGeneral->PreEpoxyShow(stULCorner, stLRCorner, prcenter, PSPR_SENDER_ID, PSPR_RECV_ID, MS899_BOND_PB_PURPOSE, lThreshold, stEpoxyShowCmd, szErrMsg))
		{
			SetErrorMessage(szErrMsg);
			HmiMessageEx(szErrMsg);
			return FALSE;
		}

	} while (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_NO );

	PR_UWORD uwRecordID = 0;
	PR_AREA aeEpoxyArea = 0;
	if (!m_pPrGeneral->LearnEpoxyExt(prcenter, stEpoxyShowCmd, PSPR_SENDER_ID, PSPR_RECV_ID, uwRecordID, aeEpoxyArea, szErrMsg))
	{
		m_ssPSPRSrchID[0]	= 0;
		m_lLearnEpoxySize[0] = 0;

		SetErrorMessage(szErrMsg);
		HmiMessageEx(szErrMsg);
		return FALSE;
	}
   
	m_ssPSPRSrchID[0]		= uwRecordID;
	m_lLearnEpoxySize[0]	= aeEpoxyArea;

	CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);

	return TRUE;

/*
	PR_COMMON_RPY                 stCommonRpy;
	PR_PRE_EPOXY_SHOW_CMD         stEpoxyShowCmd;
	PR_PRE_EPOXY_SHOW_RPY2        stEpoxyShowRpy2;
	PR_COORD prcenter;
	prcenter.x = 4096;
	prcenter.y = 4096;
	PR_InitPreEpoxyShowCmd(&stEpoxyShowCmd);

	stEpoxyShowCmd.stWin.coCorner1 = stULCorner;
	stEpoxyShowCmd.stWin.coCorner2 = stLRCorner;
	stEpoxyShowCmd.emIsGrabFromPurpose = PR_TRUE;
	stEpoxyShowCmd.aemPurpose[0] = (PR_PURPOSE)MS899_POSTSEAL_PR_PURPOSE;	//Lighting Purpose;
	stEpoxyShowCmd.emSubType = (PR_OBJ_TYPE)PR_OBJ_TYPE_BGA_SUBSTRATE;		// (Bright Background Dark Object)PR_OBJ_TYPE_HORIZONTAL_LEADFRAME,
	stEpoxyShowCmd.emEpoxyInspAlg = PR_SINGLE_DOT_WO_NOISE;

	stEpoxyShowCmd.emEpoxySegMode = (PR_PRE_EPOXY_SEG_MODE) PR_PRE_EPOXY_SEG_MODE_MANUAL_THRES;
	stEpoxyShowCmd.emDisplayInfo = PR_DISPLAY_INFO_BINARY;
	stEpoxyShowCmd.coRefPoint = prcenter;

	LONG lThreshold =  126;
	stEpoxyShowCmd.ubThreshold = (PR_UBYTE) lThreshold;		// Threshold (0--255)

	CString szContent = "Is this image OK?";
	CString szTitle   = "Show Epoxy";
	do 
    {
		stEpoxyShowCmd.ubThreshold = (PR_UBYTE)lThreshold;	// Threshold (0--255);
		PR_PreEpoxyShowCmd(&stEpoxyShowCmd, PSPR_SENDER_ID, PSPR_RECV_ID, &stCommonRpy);
		if (stCommonRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCommonRpy.uwPRStatus))
		{
			CString szLog;
			szLog.Format("PR_PreEpoxyShowCmd Error1: COMM = %d, PR = %d", stCommonRpy.uwCommunStatus, stCommonRpy.uwPRStatus);
			HmiMessageEx(szLog);
			return FALSE;
		}

		PR_PreEpoxyShowRpy(PSPR_SENDER_ID, &stEpoxyShowRpy2);
		if (stEpoxyShowRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stEpoxyShowRpy2.stStatus.uwPRStatus))
		{
			CString szLog;
			szLog.Format("PR_PreEpoxyShowRpy Error1: COMM = %d, PR = %d", stEpoxyShowRpy2.stStatus.uwCommunStatus, stEpoxyShowRpy2.stStatus.uwPRStatus);
			HmiMessageEx(szLog);
			return FALSE;
		}

	} while (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_NO );

	PR_LRN_EPOXY_EXT_CMD          stLrnEpoxyCmd;
    PR_LRN_EPOXY_EXT_RPY2         stLrnEpoxyRpy2;

 
	PR_InitLrnEpoxyExtCmd(&stLrnEpoxyCmd);
	stLrnEpoxyCmd.emLatchMode = PR_LATCH_NO_LATCH;
	stLrnEpoxyCmd.emIsGrabFromPurpose = PR_TRUE;
	stLrnEpoxyCmd.aemPurpose[0] = stEpoxyShowCmd.aemPurpose[0];

	stLrnEpoxyCmd.emEpoxyAttribute = (stEpoxyShowCmd.emSubType == PR_OBJ_TYPE_HORIZONTAL_LEADFRAME) ? PR_OBJ_ATTRIBUTE_DARK : PR_OBJ_ATTRIBUTE_BRIGHT;
	stLrnEpoxyCmd.emEpoxyInspAlg = stEpoxyShowCmd.emEpoxyInspAlg;
	stLrnEpoxyCmd.emEpoxySegMode = stEpoxyShowCmd.emEpoxySegMode;
	stLrnEpoxyCmd.ubThreshold = stEpoxyShowCmd.ubThreshold;
	stLrnEpoxyCmd.leMax = stEpoxyShowCmd.leUpperLimit;
	stLrnEpoxyCmd.leMin = stEpoxyShowCmd.leLowerLimit;

	stLrnEpoxyCmd.stOpRegion.uwNumOfCorners = PR_NO_OF_CORNERS;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_LEFT] = stEpoxyShowCmd.stWin.coCorner1;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT] = stEpoxyShowCmd.stWin.coCorner2;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x = stEpoxyShowCmd.stWin.coCorner2.x;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y = stEpoxyShowCmd.stWin.coCorner1.y;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x = stEpoxyShowCmd.stWin.coCorner1.x;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y = stEpoxyShowCmd.stWin.coCorner2.y;
	stLrnEpoxyCmd.coRefPoint = prcenter;//Image Center (4096,4096)

    PR_LrnEpoxyExtCmd(&stLrnEpoxyCmd, PSPR_SENDER_ID, PSPR_RECV_ID, &stCommonRpy);
	stLrnEpoxyCmd.emEpoxyInspAlg = PR_SINGLE_DOT_WO_NOISE;

    if (stCommonRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCommonRpy.uwPRStatus))
    {
        CString szLog;
		szLog.Format("PR_LrnEpoxyExtCmd Error1: COMM = %d, PR = %d", stCommonRpy.uwCommunStatus, stCommonRpy.uwPRStatus);
		HmiMessageEx(szLog);
		return FALSE;
    }

    PR_LrnEpoxyExtRpy(PSPR_SENDER_ID, &stLrnEpoxyRpy2);
	m_ssPSPRSrchID	= stLrnEpoxyRpy2.uwRecordID;
	m_lLearnEpoxySize = stLrnEpoxyRpy2.aeEpoxyArea;

    if (stLrnEpoxyRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stLrnEpoxyRpy2.stStatus.uwPRStatus))
    {
        CString szLog;
		szLog.Format("PR_LrnEpoxyExtRpy Error1: COMM = %d, PR = %d", stLrnEpoxyRpy2.stStatus.uwCommunStatus, stLrnEpoxyRpy2.stStatus.uwPRStatus);
		HmiMessageEx(szLog);
		return FALSE;
    }

	return TRUE;
*/
}

PR_WORD CBondPr::AGC_AutoSearchEpoxy(const LONG lPRID, const LONG lLearnEpoxySize,
									 LONG pcx, LONG pcy, LONG ecx, LONG ecy, UINT &nX, UINT &nY)
{
	CString szMsg;
	szMsg.Format("The Original AGC Centre coordinate : %d, %d", nX, nY);
	//HmiMessageEx(szMsg);
	SaveColletSizeEvent(szMsg);

	CString szErrMsg;
	PR_COORD prcenter;
	prcenter.x = 4096;
	prcenter.y = 4096;

	for (int nSearchTime = 0;nSearchTime < 5;nSearchTime ++)
	{
		ULONG ulEpoxyAreaSize = 0;
		DOUBLE dMinEpoxySize = lLearnEpoxySize * 0.3;
		DOUBLE dMaxEpoxySize = lLearnEpoxySize * 2;
		PR_WORD wRet = m_pPrGeneral->SearchEpoxyExt(lPRID, dMinEpoxySize, dMaxEpoxySize, prcenter, FALSE, FALSE, FALSE,
													TRUE, TRUE, PSPR_SENDER_ID, PSPR_RECV_ID, nX, nY, ulEpoxyAreaSize, NULL, szErrMsg);
		if (wRet == PR_FALSE)
		{
			SetErrorMessage(szMsg);
			HmiMessage_Red_Yellow(szErrMsg);
/*
			SelectLiveVideoMode();
			SelectPostSealCamera();

			CPrCrossHairDlg dlg;
			//dlg.SetCursorXY(4096, 4096);
			dlg.SetCursorXY(nX, nY);
			dlg.DoModal();
			nX = dlg.GetCursorX();
			nY = dlg.GetCursorY();

			SelectAutoBondMode();
*/
			return PR_FALSE;
		}

		CString szMsg;
		CString szColletCount = (*m_psmfSRam)["BinTable"]["ColletUsageCount"];
		CString szCollet2Count = (*m_psmfSRam)["BinTable"]["Collet2UsageCount"];
		szMsg.Format("EpoxySize:%d,Collet:%s,Collet2:%s,search failure time:%d,Learn Epoxy Size:%d",
			ulEpoxyAreaSize, szColletCount, szCollet2Count, nSearchTime, m_lLearnEpoxySize[0]);
		SaveColletSizeEvent(szMsg);

		if (wRet == PR_TRUE)
		{
			return PR_TRUE;
		}
		else if (wRet == 2)
		{
			//the Result area size is not in the range of the leart area size
			if (nSearchTime < 4)
			{
				nSearchTime ++;
				Sleep(500);
			}
			else
			{
				CString szLog;
				szLog.Format("AGC: SearchCollet Epoxy Size Error = %lu", ulEpoxyAreaSize);
				HmiMessage_Red_Yellow(szLog);		//v4.52A8

				SelectPostSealCamera();

				IPC_CServiceMessage svMsg;
				TurnOnOffPrLighting(TRUE);

				SelectLiveVideoMode(); //4.53D19 lighting issue
				//SelectPostSealCamera();

				GetGeneralLighting(svMsg);
				RestoreGeneralLighting(svMsg);

				CPrCrossHairDlg dlg;
				//dlg.SetCursorXY(4096, 4096);
				dlg.SetCursorXY(nX, nY);
				PR_WORD bRet = (dlg.DoModal() == IDOK) ? PR_TRUE : PR_FALSE;

				nX = dlg.GetCursorX();
				nY = dlg.GetCursorY();

				SelectAutoBondMode();
				return bRet;
			}
		}
	}
	return PR_FALSE;
}

BOOL CBondPr::AutoSearchPreBondEpoxy(BOOL bAuto, BOOL bLatch, 
										LONG pcx, LONG pcy, 
										UINT &nX, UINT &nY,
										ULONG& ulEpoxyAreaSize)
{
	nX = nY = 0;
	ulEpoxyAreaSize = 0;
	if (m_ssPSPRSrchID[0] == 0)
	{
		HmiMessage_Red_Back("Epoxy pattern is not learnt !!", "Bond PR PreBond Epoxy Search");
		return FALSE;
	}

	PR_COORD prcenter;
	prcenter.x = (PR_WORD)pcx;
	prcenter.y = (PR_WORD)pcy;
	if ( (pcx == 0) || (pcy == 0) )
	{
		prcenter.x = (PR_WORD)GetPrCenterX();
		prcenter.y = (PR_WORD)GetPrCenterY();
	}

	CString szErrMsg;
	LONG lPRID = m_ssPSPRSrchID[0];
	DOUBLE dLearnEpoxySize	= m_lLearnEpoxySize[0];
	DOUBLE dMinEpoxySize	= dLearnEpoxySize * 0.5;
	DOUBLE dMaxEpoxySize	= dLearnEpoxySize * 3.0;	
	PR_UBYTE ubSID = PSPR_SENDER_ID;	
	PR_UBYTE ubRID = PSPR_RECV_ID;
	
	PR_WORD wRet = m_pPrGeneral->SearchEpoxyExt(lPRID, dMinEpoxySize, dMaxEpoxySize, prcenter, TRUE, TRUE, TRUE,
												bAuto ? FALSE : TRUE, bLatch, ubSID, ubRID, nX, nY, ulEpoxyAreaSize, NULL, szErrMsg);
	if (wRet == PR_FALSE)
	{
		szErrMsg = "BPR  " + szErrMsg;
		if (!bAuto)
		{
			HmiMessage_Red_Yellow(szErrMsg);
		}
		SetErrorMessage(szErrMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);
		return FALSE;
	}
	else if (wRet == 2)
	{
		CString szMsg;
		szMsg.Format("BPR Search Eproxy Cmd fails - EPOXY size = %lu (learned = %.2f, MAX = %.2f)",
					  ulEpoxyAreaSize, dLearnEpoxySize, dMaxEpoxySize);
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		if (!bAuto)
		{
			HmiMessage_Red_Back(szMsg, "Bond PR Search Epoxy");
		}
		return FALSE;
	}
	else
	{	
		CString szMsg;
		szMsg.Format("BPR Search Eproxy Cmd found at (%ld, %ld); LATCH = %d, EPOXY size = %lu (learned = %f) (%d)", 
					 nX, nY, bLatch, ulEpoxyAreaSize, dLearnEpoxySize, ulEpoxyAreaSize);
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		if (!bAuto)
		{
			HmiMessage(szMsg, "Bond PR Search Epoxy");
		}
	}

	return TRUE;
}


PR_WORD CBondPr::AGC_AutoSearchCircle(LONG pcx, LONG pcy, LONG ecx, LONG ecy, UINT &nX, UINT &nY) 
{
	PR_DETECT_SHAPE_CMD     pstCmd;
	PR_DETECT_SHAPE_RPY1    pstRpy1;
	PR_DETECT_SHAPE_RPY2    pstRpy2;

	PR_InitDetectShapeCmd(&pstCmd);
	PR_COORD prcenter;
	PR_COORD expectcenter;
	prcenter.x		= (PR_WORD)pcx;
	prcenter.y		= (PR_WORD)pcy;
	expectcenter.x	= (PR_WORD)pcx;		//ecx;
	expectcenter.y	= (PR_WORD)pcy;		//ecy;

	pstCmd.emSameView		= PR_FALSE;
	pstCmd.uwRecordID		= m_ssPSPRSrchID[0];	//m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET+3];
	pstCmd.rDetectScore		= 70;
	pstCmd.emSelectMode		= PR_SHAPE_SELECT_MODE_CENTRE;
	pstCmd.coExpectCentre	= expectcenter;
	pstCmd.coPositionVar	= prcenter;
	pstCmd.rSizePercentVar	= 20.0;		//v4.51A2


	PR_DetectShapeCmd(&pstCmd, PSPR_SENDER_ID, PSPR_RECV_ID, &pstRpy1);

	if ( (pstRpy1.uwCommunStatus != PR_COMM_NOERR) || (pstRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("PR_DetectShapeCmd Error1: COMM = %d, PR = %d", pstRpy1.uwCommunStatus, pstRpy1.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}
	
	PR_DetectShapeRpy(PSPR_SENDER_ID, &pstRpy2);

	if ( (pstRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) || (pstRpy2.stStatus.uwPRStatus != PR_ERR_NOERR) )
	{
		CString szLog;
		szLog.Format("PR_DetectShapeCmd Error2: COMM = %d, PR = %d", pstRpy2.stStatus.uwCommunStatus, pstRpy2.stStatus.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}

//	PR_WORD lCrossCenterX = (PR_WORD)pstRpy2.stShapeRes.rcoCentre.x;
//	PR_WORD lCrossCenterY = (PR_WORD)pstRpy2.stShapeRes.rcoCentre.y;
	
	if ( pstRpy2.emShapeFound == PR_SHAPE_FOUND_CIRCLE )
	{
		nX = (UINT) pstRpy2.stCircleRes.coCentre.x;
		nY = (UINT) pstRpy2.stCircleRes.coCentre.y;
		m_pPrGeneral->DrawSmallCursor(PSPR_SENDER_ID, PSPR_RECV_ID, nX, nY);

		return PR_TRUE;
	}
	else if ( pstRpy2.emShapeFound == PR_SHAPE_FOUND_ELLIPSE )
	{
		nX = (UINT) pstRpy2.stShapeRes.rcoCentre.x;
		nY = (UINT) pstRpy2.stShapeRes.rcoCentre.y;
		m_pPrGeneral->DrawSmallCursor(PSPR_SENDER_ID, PSPR_RECV_ID, nX, nY);

		return PR_TRUE;
	}
	
	return PR_FALSE;
}

PR_UWORD CBondPr::AGC_AutoLearnCircle(PR_COORD stULCorner, PR_COORD stLRCorner)
{
	PR_LRN_SHAPE_CMD pstCmd;
	PR_LRN_SHAPE_RPY1 pstRpy1;
	PR_LRN_SHAPE_RPY2 pstRpy2;

	PR_InitLrnShapeCmd(&pstCmd);
	pstCmd.stEncRect.coCorner1 = stULCorner;					// Enclosing/Inscribing Rectangle (Used for Circle Extraction only)
	pstCmd.stEncRect.coCorner2 = stLRCorner;					// Enclosing/Inscribing Rectangle (Used for Circle Extraction only)
	pstCmd.emSameView		= PR_FALSE;							// Use previous grab image
	pstCmd.emPurpose		= MS899_POSTSEAL_PR_PURPOSE;		// PR Logical Optics
	pstCmd.emIsAligned		= PR_FALSE;							// Is the object aligned?
	pstCmd.emShapeType		= PR_SHAPE_TYPE_CIRCLE;				// Shape type (e.g: Circle, Rectangle ...etc)
	pstCmd.emRectMode		= PR_RECT_MODE_ENCLOSE;				// Is the Rectangle Enclosing or Inscribing?
	pstCmd.emDetectAlg		= PR_SHAPE_DETECT_ALG_EDGE_PTS;		// Detection Algorithm
	pstCmd.emObjAttribute	= PR_OBJ_ATTRIBUTE_BRIGHT;			// Object Attribute (Bright\Dark\Both)
	pstCmd.emIsSaveRecord	= PR_TRUE;							// Save Record (Yes: Lrn & Srch/ No: Extract once only)
	pstCmd.uwMaskID			= 0;								// Which Mask will be used? (0: Not use any Mask)
	pstCmd.rPassScore		= 70;								// Pass Score for extraction
	pstCmd.emCentreDefinition = PR_CENTRE_DEFINITION_BY_CORNER;	// Definition of Centre of the shape (By Corner or By edge point)
	pstCmd.emExtMethod		= PR_EXT_CORNER;					// Extraction Method (Line/Corner Extraction)
	pstCmd.uwNIgnoreWin		= 0;								// no. of ignore windows 
	pstCmd.emObjectType		= PR_OBJ_TYPE_LED_PACKAGE;
	//pstCmd.coRefPoint;										// Reference Point
	//pstCmd.rRefAngle;											// Reference Angle
	//pstCmd.emAccuracy;										// Accuracy
	//pstCmd.ubThreshold;										// Threshold
	//pstCmd.wEdgeMag;											// Edge Magnitude
	//pstCmd.uwRoughness;										// Roughness of the shape
	//pstCmd.astLineCoord[PR_MAX_NO_OF_SHAPE_LINE];				// Line coord.
	//pstCmd.uwNConfirmWin;
	//pstCmd.astConfirmWin[PR_MAX_NO_OF_SHA_WIN];				// SHA Windows (Confirm/Pos Emp/Neg Emp/Blob)
	//pstCmd.astIgnoreWin[PR_MAX_DETECT_IGNORE_WIN];
	//pstCmd.uwNumOfLine;										// No of Line of the shape (Used for Rect, Square, Right Angle Corner & Line)


	/////////
	PR_LrnShapeCmd(&pstCmd, PSPR_SENDER_ID, PSPR_RECV_ID, &pstRpy1);
	if ( (pstRpy1.uwCommunStatus != PR_COMM_NOERR) || (pstRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("PR_LrnShapeCmd Error1: COMM = 0x%x, PR = 0x%x", pstRpy1.uwCommunStatus, pstRpy1.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}

    PR_LrnShapeRpy(PSPR_SENDER_ID, &pstRpy2);
	if ( (pstRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) || (pstRpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("PR_LrnShapeCmd Error2: COMM = 0x%x, PR = 0x%x", pstRpy2.stStatus.uwCommunStatus, pstRpy2.stStatus.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}

	if (pstRpy2.emShapeFound != PR_SHAPE_FOUND_CIRCLE)
	{
		HmiMessageEx("ERROR: Shape found is NOT circle!!!");
		return PR_FALSE;
	}

	//m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET+3]	= pstRpy2.uwRecordID;
	//m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET+3]	= TRUE;
	m_ssPSPRSrchID[0]	= pstRpy2.uwRecordID;

	m_pPrGeneral->DrawSmallCursor(MS899_WAF_CAM_SEND_ID, MS899_WAF_CAM_RECV_ID, pstRpy2.stCircleRes.coCentre.x, pstRpy2.stCircleRes.coCentre.y);

	HmiMessageEx("Learn collet hole success !!!!!!!!");
	
	//Get PR ID & Die size
	short siMinX = (short)m_lPrCenterX, siMaxX = (short)m_lPrCenterX;
	short siMinY = (short)m_lPrCenterY, siMaxY = (short)m_lPrCenterY;
	siMinX = siMinX - (short)pstRpy2.stCircleRes.rRadius;
	siMaxX = siMaxX + (short)pstRpy2.stCircleRes.rRadius;
	siMinY = siMinY - (short)pstRpy2.stCircleRes.rRadius;
	siMaxY = siMinY + (short)pstRpy2.stCircleRes.rRadius;
	//m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].x         = siMaxX - siMinX;
	//m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].y         = siMaxY - siMinY;

	return PR_TRUE;
}


BOOL CBondPr::CalibrateRectangle(DOUBLE dXYDimension, PR_COORD stDieULC, PR_COORD stDieLRC)
{
	if (dXYDimension <= 0)
		return FALSE;

	long lPixelX = 0;
	long lPixelY = 0;
	CString szTemp;

/*
	PR_GET_RECT_SIZE_CMD	cmd;
	PR_GET_RECT_SIZE_RPY1	rpy1;
	PR_GET_RECT_SIZE_RPY2	rpy2;
	
//HmiMessage("CalibrateRectangle start ....");

	PR_InitGetRectSizeCmd(&cmd);
	cmd.emCameraNo = MS899_POSTSEAL_CAM_ID;
	cmd.acoRectCorners[PR_UPPER_LEFT].x    = stDieULC.x;
	cmd.acoRectCorners[PR_UPPER_LEFT].y    = stDieULC.y;
	cmd.acoRectCorners[PR_UPPER_RIGHT].x   = stDieLRC.x;
	cmd.acoRectCorners[PR_UPPER_RIGHT].y   = stDieULC.y;
	cmd.acoRectCorners[PR_LOWER_LEFT].x    = stDieULC.x;
	cmd.acoRectCorners[PR_LOWER_LEFT].y    = stDieLRC.y;
	cmd.acoRectCorners[PR_LOWER_RIGHT].x   = stDieLRC.x;
	cmd.acoRectCorners[PR_LOWER_RIGHT].y   = stDieLRC.y;

	PR_GetRectSizeCmd(&cmd, PSPR_SENDER_ID, PSPR_RECV_ID, &rpy1);
	if ((rpy1.uwCommunStatus != PR_COMM_NOERR) || (rpy1.uwPRStatus != PR_ERR_NOERR))
	{
		szTemp.Format("ERROR: GetRectSize RPY1 fails: CommRpy = 0x%x   PRStatus = 0x%x", 
					  rpy1.uwCommunStatus, rpy1.uwPRStatus);
HmiMessage_Red_Back(szTemp);
		return FALSE;
	}

    PR_GetRectSizeRpy(PSPR_SENDER_ID, &rpy2);
	if ((rpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) || (rpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		szTemp.Format("ERROR: GetRectSize RPY2 fails: CommRpy = 0x%x   PRStatus = 0x%x", 
					  rpy2.stStatus.uwCommunStatus, rpy2.stStatus.uwPRStatus);
		//m_usCommError		= rpy2.stStatus.uwCommunStatus;
		//m_usPrError		= rpy2.stStatus.uwPRStatus;
HmiMessage_Red_Back(szTemp);
		return FALSE;
	}
*/

	//Compute calibration factor
	//lPixelX = rpy2.szRectSize.x;
	//lPixelY = rpy2.szRectSize.y;
	lPixelX = stDieLRC.x - stDieULC.x;
	lPixelY = stDieLRC.y - stDieULC.y;

	if ((lPixelX == 0) || (lPixelY == 0))
	{
		szTemp.Format("ERROR: CalibrateRectangle fails; return value = %ld, %ld", 
					  lPixelX, lPixelY);
		HmiMessage_Red_Back(szTemp);
		return FALSE;
	}


	DOUBLE dFactorX = 1.0;
	DOUBLE dFactorY = 1.0;
	dFactorX =  1000.0 * dXYDimension / lPixelX;		// um / pixel
	dFactorY =  1000.0 * dXYDimension / lPixelY;		// um / pixel

	m_dAGCCalibX = dFactorX;	//um per pixel
	m_dAGCCalibY = dFactorY;	//um per pixel

	szTemp.Format("PSPR: calibation factor - Pixel (%ld, %ld), dimen(%.2f), Factor (%.2f, %.2f) um per pixel", 
					lPixelX, lPixelY, dXYDimension, m_dAGCCalibX, m_dAGCCalibY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
	HmiMessage(szTemp);

	SavePrData();
	return TRUE;
}

LONG CBondPr::AGC_UsePRMouse(IPC_CServiceMessage& svMsg)
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(FALSE);
	}

	if ( m_bUseMouse == FALSE )
	{
		// Erase the original region
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_UPPER_LEFT],PR_UPPER_LEFT,0);
		DrawAndEraseCursor(m_stLearnDieCornerPos[PR_LOWER_RIGHT],PR_LOWER_RIGHT,0);

		// Define by using mouse
		m_pPrGeneral->MouseDefineRegion();

		// Set the mouse size first
		PR_COORD coCorner1, coCorner2;
		coCorner1.x = 1000;
		coCorner1.y = 1000;
		coCorner2.x = 7000;
		coCorner2.y = 7000;
		m_pPrGeneral->MouseSet2PointRegion(coCorner1, coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_RED);

		m_bUseMouse = TRUE;
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::AGC_DrawRectComplete(IPC_CServiceMessage& svMsg)
{
	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		SetBinTableJoystick(TRUE);
	}

	if ( m_bUseMouse == TRUE )
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stAGCCalRectCornerPos[PR_UPPER_LEFT],
														   m_stAGCCalRectCornerPos[PR_LOWER_RIGHT]);

		// Draw the defined region
		if (uwNumOfCorners == 2)
		{
			DrawAndEraseCursor(m_stAGCCalRectCornerPos[PR_UPPER_LEFT], PR_UPPER_LEFT, 1);
			DrawAndEraseCursor(m_stAGCCalRectCornerPos[PR_LOWER_RIGHT], PR_LOWER_RIGHT, 1);
			//CalibrateRectangle(1.7, m_stAGCCalRectCornerPos[PR_UPPER_LEFT], m_stAGCCalRectCornerPos[PR_LOWER_RIGHT]);
			CalibrateRectangle(1.2, m_stAGCCalRectCornerPos[PR_UPPER_LEFT], m_stAGCCalRectCornerPos[PR_LOWER_RIGHT]);	//v4.51A21
		}

		m_bUseMouse = FALSE;
	}

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CBondPr::IsAGCCenterInPixelValidate()
{
	return (((m_lAGCC1CenterXInPixel != 0) || (m_lAGCC1CenterYInPixel != 0)) &&
			((m_lAGCC2CenterXInPixel != 0) && (m_lAGCC2CenterYInPixel != 0)));
}
