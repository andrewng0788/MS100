#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "TakeTime.h"
#include "MS896A.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"
#include "Utility.H"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//	xxxx manual side use this function too.
LONG CWaferLoader::LearnWLContourAuto(BOOL bWT2)
{
	CString szMsg;
	CString szTable = "WT1 ";
	if( bWT2 )
	{
		szTable = "WT2 ";
		szMsg = "  WFL : to learn contour edge of WT2";
	}
	else
	{
		szMsg = "  WFL : to learn contour edge of WT1";
	}
	SaveScanTimeEvent(szMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	typedef struct
	{
		BOOL bWT2;
		BOOL bTurnOn;
	}	WPR_CONTOUR_LIGHT;

	WPR_CONTOUR_LIGHT	stCntuLgt;
	IPC_CServiceMessage stMsg;
	INT nConvID = 0;
	stCntuLgt.bWT2 = bWT2;
	stCntuLgt.bTurnOn = TRUE;
	stMsg.InitMessage(sizeof(WPR_CONTOUR_LIGHT), &stCntuLgt);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SwitchContourLighting", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	PR_UBYTE	ubSendID	= ES_CONTOUR_CAM_SEND_ID;
	PR_UBYTE	ucRecvID	= ES_CONTOUR_CAM_RECV_ID;
	LONG lDiameter   = GetWaferDiameter();
	LONG lMoveDistY	 = lDiameter/CONTOUR_STEPS;
	LONG lCamOffsetX = m_lWL1WaferOffsetX;
	LONG lCamOffsetY = m_lWL1WaferOffsetY;
	LONG lGrabCtrX	 = GetWft1CenterX() + lCamOffsetX;
	LONG lGrabCtrY   = GetWft1CenterY() + lCamOffsetY;
	CString szBarcodeName = m_szBarcodeName;
	if( bWT2 )
	{
		lCamOffsetX = m_lWL2WaferOffsetX;
		lCamOffsetY = m_lWL2WaferOffsetY;
		lGrabCtrX = GetWft2CenterX() + lCamOffsetX;
		lGrabCtrY = GetWft2CenterY() + lCamOffsetY;
		m_lWL2CenterX	= GetWft2CenterX();
		m_lWL2CenterY	= GetWft2CenterY();
		szBarcodeName = m_szBarcodeName2;
	}
	else
	{
		m_lWL1CenterX	= GetWft1CenterX();
		m_lWL1CenterY	= GetWft1CenterY();
	}

	if( szBarcodeName.IsEmpty() )
		szBarcodeName = "DummySearch";

	DOUBLE dGrabPrX = 0, dGrabPrY = 0;
	GetPrInContour(bWT2, lGrabCtrX, lGrabCtrY, dGrabPrX, dGrabPrY);
	LONG lCntuLftX = lGrabCtrX + GetWaferDiameter()/2;
	LONG lCntuTopY = lGrabCtrY + GetWaferDiameter()/2;
	LONG lCntuRgtX = lGrabCtrX - GetWaferDiameter()/2;
	LONG lCntuBtmY = lGrabCtrY - GetWaferDiameter()/2;
	DOUBLE	dULPrX = 0, dULPrY = 0, dLRPrX = 0, dLRPrY = 0;
	GetPrInContour(bWT2, lCntuLftX, lCntuTopY, dULPrX, dULPrY);
	GetPrInContour(bWT2, lCntuRgtX, lCntuBtmY, dLRPrX, dLRPrY);
	szMsg.Format("%9ld,%9ld,%9ld,%9ld", lCamOffsetX, lCamOffsetY, GetPrescanPitchX(), GetPrescanPitchY());
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	szMsg.Format(",,%9ld,%9ld,%9.2f,%9.2f\n,,%9ld,%9ld,%9.2f,%9.2f\n,,%9ld,%9ld,%9.2f,%9.2f",
		lCntuLftX, lCntuTopY, dULPrX, dULPrY,
		lCntuRgtX, lCntuBtmY, dLRPrX, dLRPrY,
		lGrabCtrX, lGrabCtrY, dGrabPrX, dGrabPrY);
	ContourLog(szMsg, bWT2, TRUE);

	typedef struct
	{
		BOOL bWT2;
		LONG lDist;
	}	WT_MOVE_TIME;

	WT_MOVE_TIME stTime;
	stTime.bWT2 = bWT2;
	stTime.lDist = lMoveDistY;
	stMsg.InitMessage(sizeof(WT_MOVE_TIME), &stTime);

	LONG lMoveTime = 0;
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetYSlowMoveTime",	stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(LONG), &lMoveTime);
			break;
		}
		else
		{
			Sleep(10);
		}	
	}

	BOOL bPrError = FALSE;
#ifdef	PR_INSP_MACHINE
	for(int iCap=1; iCap<=2; iCap++)
	{
		if( bWT2 )
		{
			if( m_uwWL2ContourRecordID != 0 )
			{
				PR_FREE_RECORD_ID_CMD		stFreeCmd;
				PR_FREE_RECORD_ID_RPY		stFreeRpy;
				PR_InitFreeRecordIDCmd(&stFreeCmd);
				
				stFreeCmd.uwRecordID = (PR_UWORD)(m_uwWL2ContourRecordID);
				PR_FreeRecordIDCmd(&stFreeCmd, ubSendID, ucRecvID, &stFreeRpy);
				m_uwWL2ContourRecordID = 0;
			}

			DeleteFile(ES_CONTOUR_WL2_FILE);
			m_uwWL2ContourRecordID = 0;
		}
		else
		{
			if( m_uwWL1ContourRecordID != 0 )
			{
				PR_FREE_RECORD_ID_CMD		stFreeCmd;
				PR_FREE_RECORD_ID_RPY		stFreeRpy;
				PR_InitFreeRecordIDCmd(&stFreeCmd);
				
				stFreeCmd.uwRecordID = (PR_UWORD)(m_uwWL1ContourRecordID);
				PR_FreeRecordIDCmd(&stFreeCmd, ubSendID, ucRecvID, &stFreeRpy);
				m_uwWL1ContourRecordID = 0;
			}

			DeleteFile(ES_CONTOUR_WL1_FILE);
			m_uwWL1ContourRecordID = 0;
		}

		LONG lStartY = lGrabCtrY + lDiameter/2 - lMoveDistY/2;
		LONG lGrabX = 0, lGrabY = 0;
		CString szMsg;
		PR_WAF_CNTR_CMD	stCntrCmd;
		for(LONG lStep=1; lStep<=CONTOUR_STEPS; lStep++)
		{
			lGrabX = lGrabCtrX;
			lGrabY = lStartY - lMoveDistY * (lStep-1);
			ES101MoveWaferTable(lGrabX, lGrabY, SFM_NOWAIT, bWT2);
			
			Sleep(lMoveTime + 200);

			PR_WAF_CNTR_RPY1	stCtLrnRpy1;
			PR_WAF_CNTR_RPY2	stCtLrnRpy2;

			DOUBLE dPrX = 0, dPrY = 0;
			GetPrInContour(bWT2, lGrabX, lGrabY, dPrX, dPrY);

			PR_PURPOSE	uwPurpose = ES_CONTOUR_WL1_PR_PURPOSE;
			PR_UWORD uwRecordID = m_uwWL1ContourRecordID;
			if( bWT2 )
			{
				uwPurpose = ES_CONTOUR_WL2_PR_PURPOSE;
				uwRecordID = m_uwWL2ContourRecordID;
			}
			// [Move to upper position], init member variables
			if( lStep==1 )
			{
				PR_InitWafCntrCmd(&stCntrCmd);

				stCntrCmd.emGUIMode	= PR_WAF_CNTR_NOGUI_AUTO_CNTR_MULTI_IMG;
				stCntrCmd.emObjType	= PR_OBJ_TYPE_GENERAL;
				stCntrCmd.emPurpose	= uwPurpose;
				stCntrCmd.emGrab	= PR_WAF_CNTR_GRAB_MODE_STOP_AND_GRAB;
				stCntrCmd.emWafType	= PR_WAFER_TYPE_IRREGULAR;
				stCntrCmd.emLrnMode	= PR_WAF_CNTR_LRN_MODE_INCOMPLETE_LL;

				stCntrCmd.stSingleImgPar.emObjAttr			= PR_OBJ_ATTRIBUTE_DARK;
				stCntrCmd.stSingleImgPar.emCheckBKIntensity	= PR_FALSE;	
				strcpy_s((char *)stCntrCmd.aubWaferID, sizeof(stCntrCmd.aubWaferID), (LPCTSTR) szBarcodeName);
			}

			// Learn contour ---------------------------------------------------------------
			// put the record id returned from upper position learn here 
			stCntrCmd.uwRecordID			= uwRecordID;
			stCntrCmd.rcoCameraPosition.x	= (PR_REAL)dPrX;
			stCntrCmd.rcoCameraPosition.y	= (PR_REAL)dPrY;

			if( lStep==CONTOUR_STEPS )
			{
				stCntrCmd.emIsEndOfMultiImg = PR_TRUE;
			}

			PR_WafCntrCmd(&stCntrCmd, ubSendID, ucRecvID, &stCtLrnRpy1);
			if( stCtLrnRpy1.stStatus.uwCommunStatus!=PR_COMM_NOERR || 
				stCtLrnRpy1.stStatus.uwPRStatus!=PR_ERR_NOERR )
			{
				szMsg.Format("PR WafCntrCmd reply 1 comm %d, status %d", 
					stCtLrnRpy1.stStatus.uwCommunStatus, stCtLrnRpy1.stStatus.uwPRStatus);
				szMsg = szTable + szMsg;
				HmiMessage_Red_Back(szMsg, "ES101 Contour");
				SaveScanTimeEvent(szMsg);
				bPrError = TRUE;
			}

			PR_WafCntrRpy(ubSendID, ucRecvID, &stCtLrnRpy2);
			if( stCtLrnRpy2.stStatus.uwCommunStatus!=PR_COMM_NOERR || 
				stCtLrnRpy2.stStatus.uwPRStatus!=PR_ERR_NOERR )
			{
				szMsg.Format("PR WafCntrRpy reply 2 comm %d, status %d", 
					stCtLrnRpy2.stStatus.uwCommunStatus, stCtLrnRpy2.stStatus.uwPRStatus);
				szMsg = szTable + szMsg;
				HmiMessage_Red_Back(szMsg, "ES101 Contour");
				SaveScanTimeEvent(szMsg);
				bPrError = TRUE;
			}

			if( lStep==1 )
			{
				if( bWT2 )
				{
					m_uwWL2ContourRecordID	= stCtLrnRpy2.uwRecordID;
				}
				else
				{
					m_uwWL1ContourRecordID	= stCtLrnRpy2.uwRecordID;
				}
			}

			if( iCap==2 || m_bEnableContourGT==FALSE )
			{
				szMsg.Format(",,%9ld,%9ld,%9.2f,%9.2f", lGrabX, lGrabY, dPrX, dPrY);
				ContourLog(szMsg, bWT2);
			}

			if( lStep==CONTOUR_STEPS )
			{
				DOUBLE dPrULX = stCtLrnRpy2.rcoWaferUL.x;
				DOUBLE dPrULY = stCtLrnRpy2.rcoWaferUL.y;
				DOUBLE dPrLRX = stCtLrnRpy2.rcoWaferLR.x;
				DOUBLE dPrLRY = stCtLrnRpy2.rcoWaferLR.y;
				PR_REAL rSceneAngle = 360;
				if( stCtLrnRpy2.wIsSceneAngleFound == PR_TRUE )
				{
					rSceneAngle	= stCtLrnRpy2.rSceneAngle;
				}

				ConvertContourPrToPosition(bWT2, dPrULX, dPrULY, lCntuLftX, lCntuTopY);
				ConvertContourPrToPosition(bWT2, dPrLRX, dPrLRY, lCntuRgtX, lCntuBtmY);
				szMsg.Format(",,%9ld,%9ld,%9.2f,%9.2f", lCntuLftX, lCntuTopY, dPrULX, dPrULY);
				ContourLog(szMsg, bWT2);
				szMsg.Format(",,%9ld,%9ld,%9.2f,%9.2f,%f", lCntuRgtX, lCntuBtmY, dPrLRX, dPrLRY, rSceneAngle);
				ContourLog(szMsg, bWT2);

				if( iCap<2 && rSceneAngle!=360 && m_bEnableContourGT )
				{
					LONG lTheta	= -m_lThetaMotorDirection * (LONG)(rSceneAngle/m_dThetaRes);
					LONG lX = 0, lY = 0, lT;
					GetES101WTEncoder(&lX, &lY, &lT, bWT2);
					lTheta = lT + lTheta;
					MoveWaferThetaTo(lTheta, bWT2);
					Sleep(500);
				}
			}
		}

		if( m_bEnableContourGT==FALSE )
		{
			break;
		}
	}
#endif

	SaveScanTimeEvent("  WFL : learn contour edge done");
	CMSLogFileUtility::Instance()->WL_LogStatus("WFL : learn contour edge done");

	stCntuLgt.bWT2 = bWT2;
	stCntuLgt.bTurnOn = FALSE;
	stMsg.InitMessage(sizeof(WPR_CONTOUR_LIGHT), &stCntuLgt);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SwitchContourLighting", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	SaveScanTimeEvent("  WFL : to search edge matrix");

	LONG lX1 = GetWft1CenterX() + GetWaferDiameter()/2;
	LONG lX2 = GetWft1CenterX() - GetWaferDiameter()/2;
	if( bWT2 )
	{
		lX1 = GetWft2CenterX() + GetWaferDiameter()/2;
		lX2 = GetWft2CenterX() - GetWaferDiameter()/2;
	}

	LONG lWaferLftX = lCntuLftX - lCamOffsetX;
	LONG lWaferTopY = lCntuTopY - lCamOffsetY;
	LONG lWaferRgtX = lCntuRgtX - lCamOffsetX;
	LONG lWaferBtmY = lCntuBtmY - lCamOffsetY;

	LONG nExtraMarginY = (LONG) ceil(m_dContourExtraMarginY);
	LONG lMarginX = _round(0 - GetPrescanPitchX() * m_dContourExtraMarginX);
	LONG lMarginY = _round(0 - GetPrescanPitchY() * nExtraMarginY);
	LONG lLftX = lWaferLftX - lMarginX;
	LONG lTopY = lWaferTopY - lMarginY + GetPrescanPitchY();
	LONG lRgtX = lWaferRgtX + lMarginX;
	LONG lBtmY = lWaferBtmY + lMarginY - GetPrescanPitchY();

	LONG lCenterX = (lWaferLftX + lWaferRgtX)/2;
	LONG lCenterY = (lWaferTopY + lWaferBtmY)/2;

	szMsg.Format("%9ld,%9ld,%9ld,%9ld\n%9ld,%9ld,%9ld,%9ld\n%9ld,%9ld,,\n%9ld,%9ld,,",
		lWaferLftX, lWaferTopY, lCntuLftX, lCntuTopY,
		lWaferRgtX, lWaferBtmY, lCntuRgtX, lCntuBtmY,
		lLftX, lTopY, lRgtX, lBtmY);
	ContourLog(szMsg, bWT2);

	FILE *fpEdge = NULL;
	CString szFileName =ES_CONTOUR_WL1_FILE ;
	if (bWT2)
	{
		szFileName = ES_CONTOUR_WL2_FILE;
	}

	errno_t nErr = fopen_s(&fpEdge, szFileName, "w");
	if ((nErr == 0) && (fpEdge != NULL))
	{
		szMsg.Format("%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld", lLftX, lTopY, lRgtX, lBtmY, lWaferLftX, lWaferTopY, lWaferRgtX, lWaferBtmY);
		fprintf(fpEdge, "%s\n", (LPCTSTR) szMsg);		//Klocwork	//v4.46
	}

	LONG lScanY = lTopY;
	LONG lResult = 0;
	LONG lLastScnY = lScanY, lLastLftX = 0, lLastRgtX = 0;
	BOOL bFirstFound = FALSE;
	while( 1 )
	{
		LONG lY1 = lScanY + GetPrescanPitchY()/2;
		LONG lY2 = lScanY - GetPrescanPitchY()/2;
		LONG lOutLftX = 0, lOutTopY = 0, lOutRgtX = 0, lOutBtmY = 0;
		lResult = ObtainContourEdge(bWT2, lX1, lY1, lX2, lY2, lOutLftX, lOutTopY, lOutRgtX, lOutBtmY);
		if( lResult<=0 )
		{
			bPrError = TRUE;
			break;
		}
		if( lResult>=2 )
		{
			lLastLftX = lOutLftX - lMarginX;
			lLastRgtX = lOutRgtX + lMarginX;
			lLastScnY = lScanY;
			if( fpEdge!=NULL )
			{
				if( bFirstFound==FALSE && nExtraMarginY>0 )
				{
					for(int nY = nExtraMarginY; nY>0; nY--)
					{
						LONG lNewCtrY = lLastScnY + GetPrescanPitchY()*nY;
						szMsg.Format("%ld,%ld,%ld", lLastLftX, lNewCtrY, lLastRgtX);
						fprintf(fpEdge, "%s\n", (LPCTSTR) szMsg);
						szMsg.Format("%ld,%ld,,\n%ld,%ld,,", lLastLftX, lNewCtrY, lLastRgtX, lNewCtrY);
						ContourLog(szMsg, bWT2);
					}
					bFirstFound = TRUE;
				}
				//	left scan x,center y,right scan x,left out x,right out x
				szMsg.Format("%ld,%ld,%ld,%ld,%ld", lLastLftX, lLastScnY, lLastRgtX, lOutLftX, lOutRgtX);
				fprintf(fpEdge, "%s\n", (LPCTSTR) szMsg);
			}
			szMsg.Format("%ld,%ld,,\n%ld,%ld,,", lLastLftX, lLastScnY, lLastRgtX, lLastScnY);
			ContourLog(szMsg, bWT2);
		}
		lScanY -= GetPrescanPitchY();
		if( lScanY<lBtmY )
		{
			break;
		}
	}
	szMsg.Format("%ld,%ld,,\n%ld,%ld,,", lLastLftX, lLastScnY, lLastRgtX, lLastScnY);
	ContourLog(szMsg, bWT2);

	if( fpEdge!=NULL && nExtraMarginY>0 )
	{
		for(int nY = 1; nY<=nExtraMarginY; nY++)
		{
			LONG lNewCtrY = lLastScnY - GetPrescanPitchY()*nY;
			szMsg.Format("%ld,%ld,%ld", lLastLftX, lNewCtrY, lLastRgtX);
			fprintf(fpEdge, "%s\n", (LPCTSTR) szMsg);
			szMsg.Format("%ld,%ld,,\n%ld,%ld,,", lLastLftX, lNewCtrY, lLastRgtX, lNewCtrY);
			ContourLog(szMsg, bWT2);
		}
	}

	if( fpEdge!=NULL )
	{
		fclose(fpEdge);
	}

	if( lResult<=0 )
	{
		DeleteFile(szFileName);
	}

	szMsg = " Search contour center error";
	if( bPrError==FALSE )
	{
		if( bWT2 )
		{
			m_lWL2CenterX	= lCenterX;
			m_lWL2CenterY	= lCenterY;
			szMsg.Format(" WT2 new contour center %d,%d", lCenterX, lCenterY);
		}
		else
		{
			m_lWL1CenterX	= lCenterX;
			m_lWL1CenterY	= lCenterY;
			szMsg.Format(" WT1 new contour center %d,%d", lCenterX, lCenterY);
		}
	}

	SaveScanTimeEvent("  WFL : search edge matrix done" + szMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus("WFL : search edge matrix done" + szMsg);

	if( bWT2 )
	{
		if( m_uwWL2ContourRecordID != 0 )
		{
			PR_FREE_RECORD_ID_CMD		stFreeCmd;
			PR_FREE_RECORD_ID_RPY		stFreeRpy;
			PR_InitFreeRecordIDCmd(&stFreeCmd);
			
			stFreeCmd.uwRecordID = (PR_UWORD)(m_uwWL2ContourRecordID);
			PR_FreeRecordIDCmd(&stFreeCmd, ubSendID, ucRecvID, &stFreeRpy);
			m_uwWL2ContourRecordID = 0;
		}
	}
	else
	{
		if( m_uwWL1ContourRecordID != 0 )
		{
			PR_FREE_RECORD_ID_CMD		stFreeCmd;
			PR_FREE_RECORD_ID_RPY		stFreeRpy;
			PR_InitFreeRecordIDCmd(&stFreeCmd);
			
			stFreeCmd.uwRecordID = (PR_UWORD)(m_uwWL1ContourRecordID);
			PR_FreeRecordIDCmd(&stFreeCmd, ubSendID, ucRecvID, &stFreeRpy);
			m_uwWL1ContourRecordID = 0;
		}
	}

	if( m_bContourPreloadMap && IsBurnIn()==FALSE )
	{
		CreatePreloadContourMap(bWT2);
	}

	ES101MoveWaferTable(lGrabCtrX, lGrabCtrY, SFM_WAIT, bWT2);
	szMsg.Format("Contoure done, move to contour camera center %ld,%ld of table %d", lGrabCtrX, lGrabCtrY, bWT2+1);
	CMSLogFileUtility::Instance()->WL_LogStatus("WFL : " + szMsg);

	return 1;
}

//	return 
//		-1	reply 1 error	-2	reply 2 error	-3, out of wafer limit
//		0	not learnt, use default limit
//		1	no edge found, invalid edge
//		2	edge data found, need to check
LONG CWaferLoader::ObtainContourEdge(BOOL bWT2, LONG lInULX, LONG lInULY, LONG lInLRX, LONG lInLRY, 
									LONG &lOutULX, LONG &lOutULY, LONG &lOutLRX, LONG &lOutLRY)
{
	lOutULX = lInULX;
	lOutULY = lInULY;
	lOutLRX = lInLRX;
	lOutLRY = lInLRY;
	CString szMsg;

	LONG lCamOffsetX = m_lWL1WaferOffsetX;
	LONG lCamOffsetY = m_lWL1WaferOffsetY;
	PR_UWORD uwRecordID = 0;
	CString	szTable = "WT1 ";
	if( bWT2 )
	{
		szTable = "WT2 ";
		lCamOffsetX = m_lWL2WaferOffsetX;
		lCamOffsetY = m_lWL2WaferOffsetY;
		uwRecordID = m_uwWL2ContourRecordID;
		if( m_bWL2Calibrated==FALSE )
			uwRecordID = 0;
	}
	else
	{
		uwRecordID = m_uwWL1ContourRecordID;
		if( m_bWL1Calibrated==FALSE )
			uwRecordID = 0;
	}

	if( uwRecordID==0 )
	{
		szMsg = "Contour no record id learnt";
		szMsg = szTable + szMsg;
		SaveScanTimeEvent(szMsg);
		HmiMessage_Red_Back(szMsg, "ES101 Contour");
		return 0;
	}

	PR_UBYTE	ubSenderID		= ES_CONTOUR_CAM_SEND_ID;
	PR_UBYTE	ubReceiverID	= ES_CONTOUR_CAM_RECV_ID;
#ifdef	PR_INSP_MACHINE

	PR_SRCH_WAF_EDGE_BY_CNTR_CMD	stCmd;
	PR_SRCH_WAF_EDGE_BY_CNTR_RPY1	stRpy1;
	PR_SRCH_WAF_EDGE_BY_CNTR_RPY2	stRpy2; 

	PR_InitSrchWafEdgeByCntrCmd(&stCmd);

	stCmd.emSrchMode = PR_WAF_EDGE_SRCH_MODE_START_END_PT;
	stCmd.uwRecordID = uwRecordID;

	LONG lCorX, lCorY;
	DOUBLE	dCorPrX = 0, dCorPrY = 0;
	if( bWT2 )
	{
		lCorX = GetWft2CenterX() + lCamOffsetX;
		lCorY = GetWft2CenterY() + lCamOffsetY;
	}
	else
	{
		lCorX = GetWft1CenterX() + lCamOffsetX;
		lCorY = GetWft1CenterY() + lCamOffsetY;
	}

	GetPrInContour(bWT2, lCorX, lCorY, dCorPrX, dCorPrY);

	LONG lInCntLftX = lInULX + lCamOffsetX;
	LONG lInCntTopY = lInULY + lCamOffsetY;
	LONG lInCntRgtX = lInLRX + lCamOffsetX;
	LONG lInCtnBtmY = lInLRY + lCamOffsetY;

	DOUBLE	dULPrX = 0, dULPrY = 0, dLRPrX = 0, dLRPrY = 0;
	GetPrInContour(bWT2, lInCntLftX, lInCntTopY, dULPrX, dULPrY);
	GetPrInContour(bWT2, lInCntRgtX, lInCtnBtmY, dLRPrX, dLRPrY);

	stCmd.rRotation		= (PR_REAL)0.0;
	stCmd.rcoRefPt.x	= (PR_REAL)dCorPrX;
	stCmd.rcoRefPt.y	= (PR_REAL)dCorPrY;
	stCmd.rcoLinePt1.x	= (PR_REAL)dULPrX;
	stCmd.rcoLinePt1.y	= (PR_REAL)dULPrY;
	stCmd.rcoLinePt2.x	= (PR_REAL)dLRPrX;
	stCmd.rcoLinePt2.y	= (PR_REAL)dLRPrY;

	PR_SrchWafEdgeByCntrCmd(&stCmd, ubSenderID, ubReceiverID, &stRpy1);
	if( stRpy1.stStatus.uwCommunStatus!=PR_COMM_NOERR || stRpy1.stStatus.uwPRStatus!=PR_ERR_NOERR )
	{
		szMsg.Format("PR_SrchWafEdgeByCntrCmd reply 2 comm %d, status %d", stRpy1.stStatus.uwCommunStatus, stRpy1.stStatus.uwPRStatus);
		szMsg = szTable + szMsg;
		HmiMessage_Red_Back(szMsg, "ES101 Contour");
		SaveScanTimeEvent(szMsg);
		return -1;
	}

	PR_SrchWafEdgeByCntrRpy(ubSenderID, &stRpy2);
	if( stRpy2.stStatus.uwCommunStatus!=PR_COMM_NOERR || stRpy2.stStatus.uwPRStatus!=PR_ERR_NOERR )
	{
		szMsg.Format("PR_SrchWafEdgeByCntrRpy reply 2 comm %d, status %d", stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
		szMsg = szTable + szMsg;
		HmiMessage_Red_Back(szMsg, "ES101 Contour");
		SaveScanTimeEvent(szMsg);
		return -2;
	}

	if( stRpy2.uwNoOfPts==0 )
	{
		return 1;
	}

	DOUBLE dPrLftX = 0, dPrTopY = 0, dPrBtmY = 0, dPrRgtX = 0;
	for ( int i = 0; i < stRpy2.uwNoOfPts; ++i )
	{
		if( i==0 )
		{
			dPrLftX = stRpy2.arcoStartPt[i].x;
			dPrTopY = stRpy2.arcoStartPt[i].y;
			dPrRgtX = stRpy2.arcoEndPt[i].x;
			dPrBtmY = stRpy2.arcoEndPt[i].y;
		}
		else
		{
			if( dPrLftX > stRpy2.arcoStartPt[i].x )
				dPrLftX = stRpy2.arcoStartPt[i].x;
			if( dPrTopY > stRpy2.arcoStartPt[i].y )
				dPrTopY = stRpy2.arcoStartPt[i].y;
			if( dPrRgtX < stRpy2.arcoEndPt[i].x )
				dPrRgtX = stRpy2.arcoEndPt[i].x;
			if( dPrBtmY < stRpy2.arcoEndPt[i].y )
				dPrBtmY = stRpy2.arcoEndPt[i].y;
		}
	}

	LONG lCntLftX, lCntTopY, lCntRgtX, lCntBtmY;
	ConvertContourPrToPosition(bWT2, dPrLftX, dPrTopY, lCntLftX, lCntTopY);
	ConvertContourPrToPosition(bWT2, dPrRgtX, dPrBtmY, lCntRgtX, lCntBtmY);

	lCntTopY = lCntBtmY = (lInCntTopY + lInCtnBtmY)/2;

	lOutULX = lCntLftX - lCamOffsetX;
	lOutULY = lCntTopY - lCamOffsetY;
	lOutLRX = lCntRgtX - lCamOffsetX;
	lOutLRY = lCntBtmY - lCamOffsetY;

	szMsg.Format(",,%9d,%9d,%9.2f,%9.2f\n,,%9d,%9d,%9.2f,%9.2f",
				lCntLftX, lCntTopY, dPrLftX, dPrTopY,
				lCntRgtX, lCntBtmY, dPrRgtX, dPrBtmY);
	ContourLog(szMsg, bWT2);

	if( bWT2 )
	{
		if( IsWithinWT2WaferLimit(lOutULX, lOutULY)==FALSE || IsWithinWT2WaferLimit(lOutLRX, lOutLRY)==FALSE )
		{
			szMsg = "WT2 Find edge out of wafer limit.\nPlease check limit or wafer status.";
			SaveScanTimeEvent(szMsg);
			HmiMessage_Red_Back(szMsg, "ES101 Contour");
			return -3;
		}
	}
	else
	{
		if( IsWithinWT1WaferLimit(lOutULX, lOutULY)==FALSE || IsWithinWT1WaferLimit(lOutLRX, lOutLRY)==FALSE )
		{
			szMsg = "WT1 Find edge out of wafer limit.\nPlease check limit or wafer status.";
			SaveScanTimeEvent(szMsg);
			HmiMessage_Red_Back(szMsg, "ES101 Contour");
			return -3;
		}
	}
#endif

	return 2;
}

LONG CWaferLoader::ConvertContourPrToPosition(BOOL bWT2, DOUBLE dPrX, DOUBLE dPrY, LONG &lX, LONG &lY)
{
	DOUBLE dPrCtrX = 4096, dPrCtrY = 4096;
	PR_COORD stPrPoint;
	stPrPoint.x = (PR_WORD)(dPrX - dPrCtrX);
	stPrPoint.y = (PR_WORD)(dPrY - dPrCtrY);
	INT nOutX = 0, nOutY = 0;
	if( bWT2 )
	{
		ConvertPixelToMotorStep(stPrPoint, &nOutX, &nOutY,  m_dWL2CalibXX, m_dWL2CalibYY, m_dWL2CalibXY, m_dWL2CalibYX);
	}
	else
	{
		ConvertPixelToMotorStep(stPrPoint, &nOutX, &nOutY,  m_dWL1CalibXX, m_dWL1CalibYY, m_dWL1CalibXY, m_dWL1CalibYX);
	}

	lX = nOutX;
	lY = nOutY;
	LONG lCtrX = GetWft1CenterX() + m_lWL1WaferOffsetX;
	LONG lCtrY = GetWft1CenterY() + m_lWL1WaferOffsetY;
	if( bWT2 )
	{
		lCtrX = GetWft2CenterX() + m_lWL2WaferOffsetX;
		lCtrY = GetWft2CenterY() + m_lWL2WaferOffsetY;
	}

	lX = lCtrX + lX;
	lY = lCtrY + lY;

	return 1;
}

VOID CWaferLoader::ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY, 
									   double dCalibX, double dCalibY, double dCalibXY, double dCalibYX)
{
	*siStepX = (int)((DOUBLE)stPixel.x * dCalibX + (DOUBLE)stPixel.y * dCalibXY);
	*siStepY = (int)((DOUBLE)stPixel.y * dCalibY + (DOUBLE)stPixel.x * dCalibYX);
}

LONG CWaferLoader::SearchWTContourCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	if( m_bUseContour==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return bReturn;
	}

	IPC_CServiceMessage stMsg;
	int nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ESMoveTablesToUnload",	stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

	if( bReturn )
	{
		if( IsWT2InUse() )
		{
			bReturn = SearchWT2ContourCmd(svMsg);
		}
		else
		{
			bReturn = SearchWT1ContourCmd(svMsg);
		}
	}

	if( bReturn )
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ESMoveTablesToUnload",	stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return bReturn;
}

LONG CWaferLoader::SearchWT1ContourCmd(IPC_CServiceMessage &svMsg)
{
	if( m_bWL1Calibrated==FALSE )
	{
		HmiMessage_Red_Back("WL1 contour calibration not done", "WL Contour");
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return bReturn;
	}

	if( m_lWL1WaferOffsetX==0 && m_lWL1WaferOffsetY==0 )
	{
		HmiMessage_Red_Back("Please teach WL1 contour offset!", "WL Contour");
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return bReturn;
	}

	LearnWLContourAuto(FALSE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return bReturn;
}

//	search/learn contour edge, if before barcode reading again, need to move back to original position.
//	after frame in table, expander close and down,
LONG CWaferLoader::SearchWT2ContourCmd(IPC_CServiceMessage &svMsg)
{
	if( m_bWL2Calibrated==FALSE )
	{
		HmiMessage_Red_Back("WL2 contour calibration not done", "WL Contour");
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return bReturn;
	}

	if( m_lWL2WaferOffsetX==0 && m_lWL2WaferOffsetY==0 )	// CONTOUR default value
	{
		HmiMessage_Red_Back("Please teach WL2 contour offset!", "WL Contour");
		BOOL bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return bReturn;
	}

	LearnWLContourAuto(TRUE);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return bReturn;
}

BOOL CWaferLoader::CreatePreloadContourMap(BOOL bWT2)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap()==FALSE )	// create dummy map for pick and place
	{
		return TRUE;
	}

	SaveScanTimeEvent("  WFL - contour to create preload dummy map");

	CString szMapName = m_szBarcodeName;
	if( bWT2 )
	{
		szMapName = m_szBarcodeName2;
	}

	if( szMapName.IsEmpty() )
	{
		if( bWT2 )
		{
			szMapName = "DummyWft2";
		}
		else
		{
			szMapName = "DummyWft1";
		}
	}
	CString szScanPsmPath = PRESCAN_RESULT_FULL_PATH + szMapName + ".PSM";
	CString szScanScnPath = PRESCAN_RESULT_FULL_PATH + szMapName + ".SCN";
	CString szMovePsnPath = PRESCAN_RESULT_FULL_PATH + szMapName + PRESCAN_MOVE_POSN;
	remove(szScanPsmPath);
	remove(szScanScnPath);
	remove(szMovePsnPath);

	CString 	szMapFileExt = (*m_psmfSRam)["MS896A"]["MapFileExt"];
	if ( szMapFileExt.IsEmpty() == FALSE)
	{
		szMapName = szMapName + "." + szMapFileExt;
	}
	CString szMapPathName = PRESCAN_RESULT_FULL_PATH + szMapName;

	LONG lAlnWfX = 0;
	LONG lAlnWfY = 0;
	if( bWT2 )
		GetWT2HomeDiePhyPosn(lAlnWfX, lAlnWfY);
	else
		GetHomeDiePhyPosn(lAlnWfX, lAlnWfY);

	if( m_bUseContour )
	{
		if( bWT2 )
		{
			lAlnWfX = m_lWL2CenterX;
			lAlnWfY = m_lWL2CenterY;
		}
		else
		{
			lAlnWfX = m_lWL1CenterX;
			lAlnWfY = m_lWL1CenterY;
		}
	}

	LONG lScanMapMaxY = lAlnWfY;
	LONG lScanMapMinY = lAlnWfY;

	BOOL	bContourEdge		= FALSE;
	ULONG	ulContourEdgePoints	= 0;
	LONG	lCntrEdgeY_C[SCAN_MAX_MATRIX_ROW+1];
	LONG	lCntrEdgeX_L[SCAN_MAX_MATRIX_ROW+1];
	LONG	lCntrEdgeX_R[SCAN_MAX_MATRIX_ROW+1];
	memset(lCntrEdgeY_C, 0, sizeof(lCntrEdgeY_C));
	memset(lCntrEdgeX_L, 0, sizeof(lCntrEdgeX_L));
	memset(lCntrEdgeX_R, 0, sizeof(lCntrEdgeX_R));

	CString szMsg;
	CString szFilename = ES_CONTOUR_WL1_FILE;
	if( bWT2 )
		szFilename = ES_CONTOUR_WL2_FILE;
	CStdioFile fpEdge;
	CString szReading;
	CStringArray szaDataList;
	if( fpEdge.Open(szFilename, CFile::modeRead|CFile::shareDenyNone) )
	{
		fpEdge.SeekToBegin();
		// get the fail counter limit
		if( fpEdge.ReadString(szReading) )
		{
			szaDataList.RemoveAll();
			CUtility::Instance()->ParseRawData(szReading, szaDataList);
			if( szaDataList.GetSize()<4 )
			{
				pUtl->PrescanMoveLog("Contour file format wrong");
			}
			else
			{
				bContourEdge = TRUE;
			}
		}

		while( bContourEdge )
		{
			INT nPosnY = 0;
			LONG lLftX, lRgtX, lCtrY;
			if( fpEdge.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				CUtility::Instance()->ParseRawData(szReading, szaDataList);
				if( szaDataList.GetSize()<3 )
				{
					bContourEdge = FALSE;
					pUtl->PrescanMoveLog("Contour file data wrong");
					break;
				}
				else
				{
					lLftX = atoi(szaDataList.GetAt(0));	//	left x
					lCtrY = atoi(szaDataList.GetAt(1));	//	center y
					lRgtX = atoi(szaDataList.GetAt(2));	//	right x
					lCntrEdgeY_C[ulContourEdgePoints] = lCtrY;
					lCntrEdgeX_L[ulContourEdgePoints] = lLftX;
					lCntrEdgeX_R[ulContourEdgePoints] = lRgtX;
					ulContourEdgePoints++;
					if( ulContourEdgePoints>=SCAN_MAX_MATRIX_ROW )
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		pUtl->PrescanMoveLog("Contour file can not open");
	}

	fpEdge.Close();

	szMsg.Format("  WFL: Dummy contour to create wafer map at %d,%d", lAlnWfX, lAlnWfY);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	LONG lRadius	= GetWaferDiameter()/2;
	LONG lCenterX	= GetWft1CenterX();
	LONG lCenterY	= GetWaferCenterY();
	if( bWT2 )
	{
		lCenterX	= GetWft2CenterX();
		lCenterY	= GetWft2CenterY();
	}

	LONG lLftX = lCenterX + lRadius;
	LONG lRgtX = lCenterX - lRadius;
	LONG lTopY = lCenterY + lRadius;
	LONG lBtmY = lCenterY - lRadius;

	LONG lDiePitchX_X	= labs(GetDiePitchX_X());
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_Y	= labs(GetDiePitchY_Y());
	LONG lDiePitchY_X	= GetDiePitchY_X();

	LONG ulNoOfRowsTop = 0, ulNoOfRowsBtm = 0, ulNoOfColsLft = 0, ulNoOfColsRgt = 0;
	if (lDiePitchY_Y != 0)
	{
		ulNoOfRowsTop = labs((lTopY-lAlnWfY) / lDiePitchY_Y);
		ulNoOfRowsBtm = labs((lAlnWfY-lBtmY) / lDiePitchY_Y);
		ulNoOfRowsTop += 5;
		ulNoOfRowsBtm += 5;
	}

	if (lDiePitchX_X != 0)
	{
		ulNoOfColsLft = labs((lLftX-lAlnWfX) / lDiePitchX_X);
		ulNoOfColsRgt = labs((lAlnWfX-lRgtX) / lDiePitchX_X);
		ulNoOfColsLft += 5;
		ulNoOfColsRgt += 5;
	}

	LONG ulNoOfRows = ulNoOfRowsTop + ulNoOfRowsBtm;
	LONG ulNoOfCols = ulNoOfColsLft + ulNoOfColsRgt;

	szMsg.Format("  WFL: PRESCAN Create dummy preload map %s at %d,%d", szMapPathName, lAlnWfX, lAlnWfY);
	SaveScanTimeEvent(szMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	LONG lAlnRow = m_lHomeDieMapRow;
	LONG lAlnCol = m_lHomeDieMapCol;

	FILE *fpMap = NULL;
	errno_t nErr = fopen_s(&fpMap, szMapPathName, "w");
	if ((nErr == 0) && (fpMap != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(fpMap, "DataFileName,,%s\n", (LPCTSTR) szMapPathName);
		fprintf(fpMap, "LotNumber,,\n");
		fprintf(fpMap, "DeviceNumber,,\n");
		fprintf(fpMap, "wafer id=\n");
		fprintf(fpMap, "TestTime,%s,%s\n", (LPCTSTR) theTime.Format("%Y%m%d"), (LPCTSTR) theTime.Format("%H%M%S"));
		fprintf(fpMap, "MapFileName,,%s\n", (LPCTSTR) szMapPathName);
		fprintf(fpMap, "TransferTime,%s,%s\n", (LPCTSTR) theTime.Format("%Y%m%d"), (LPCTSTR) theTime.Format("%H%M%S"));
		fprintf(fpMap, "\n");
		fprintf(fpMap, "map data\n");
		fprintf(fpMap, "%ld,%ld,\n", lAlnCol, lAlnRow);
	}

	if( bContourEdge )
	{
		lScanMapMaxY =	lCntrEdgeY_C[0] + GetPrescanPitchY()/2;
		lScanMapMinY =	lCntrEdgeY_C[0] - GetPrescanPitchY()/2;
		if( ulContourEdgePoints>0 )
		{
			lScanMapMinY =	lCntrEdgeY_C[ulContourEdgePoints-1] - GetPrescanPitchY()/2;
		}
	}

	for(LONG lRow=0; lRow<=ulNoOfRows; lRow++)
	{
		for(LONG lCol=0; lCol<=ulNoOfCols; lCol++)
		{
			LONG lDiff_X = lCol - ulNoOfColsLft;
			LONG lDiff_Y = lRow - ulNoOfRowsTop;

			LONG lDieX = lAlnWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			LONG lDieY = lAlnWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

			if( bContourEdge )
			{
				if( lDieY>lScanMapMaxY || lDieY<lScanMapMinY )
				{
					continue;
				}

				LONG lNearestRow = 0;
				LONG lNearestY = labs(lDieY - lCntrEdgeY_C[0]);
				for(ULONG kk=0; kk<ulContourEdgePoints; kk++)
				{
					if( labs(lDieY-lCntrEdgeY_C[kk])<lNearestY )
					{
						lNearestY = labs(lDieY-lCntrEdgeY_C[kk]);
						lNearestRow = kk;
					}
				}

				if( lDieX>(lCntrEdgeX_L[lNearestRow]) || 
					lDieX<(lCntrEdgeX_R[lNearestRow]) )
				{
					continue;
				}
			}

			if( bWT2 )
			{
				if( IsWithinWT2WaferLimit(lDieX, lDieY)==FALSE )
				{
					continue;
				}
			}
			else
			{
				if( IsWithinWT1WaferLimit(lDieX, lDieY)==FALSE )
				{
					continue;
				}
			}

			if( fpMap!=NULL )
			{
				LONG lMapRow = lDiff_Y + lAlnRow;
				LONG lMapCol = lDiff_X + lAlnCol;
				fprintf(fpMap, "%ld,%ld,%d\n", lMapCol, lMapRow, m_ucDummyPrescanPNPGrade);
			}
		}
	}

	if( fpMap!=NULL )
	{
		fclose(fpMap);
	}

	//	BOOL WriteCachedMap(LPCTSTR strFileName);
	szMsg = "WL: Preload new created map";
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	m_WaferMapWrapper.InitCachedMap();
	m_WaferMapWrapper.SetCachedStorageName(szMapPathName);
	if( m_WaferMapWrapper.LoadCachedMap(szMapPathName, "")==FALSE )
	{
		SaveScanTimeEvent("  WFL: scan contour preLoad map FALSE");
		return FALSE;
	}

	SaveScanTimeEvent("  WFL: scan contour preLoad map done");
	return TRUE;
}	//	Create and/or preload map for contor.
