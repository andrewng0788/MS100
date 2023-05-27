//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "MS896A.h"
#include "PrescanInfo.h"
#include "WT_Log.h"
#include "LogFileUtil.h"
#include "PrescanUtility.h"
#include "WT_RegionPredication.h"
#include "Encryption.h"

//#include "spswitch.h"		//v4.47T5
//using namespace AsmSw;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::WFT_SearchPrDie(LONG &lOffsetX, LONG &lOffsetY, BOOL bProbeCam, BOOL bBigWindow)
{
	lOffsetX = 0;
	lOffsetY = 0;

	IPC_CServiceMessage stMsg;
	if( bProbeCam )
	{
		typedef struct 
		{
			int		siStepX;
			int		siStepY;
    		BOOL    bResult;
		} BPR_DIEOFFSET;
		BPR_DIEOFFSET stInfo;

		stInfo.bResult = FALSE;

		// Get the reply
		int nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDie", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
				break;
			}
			else
			{
				Sleep(1);
			}
		}

		if( stInfo.bResult )
		{
			lOffsetX = stInfo.siStepX - m_lPrbCamOffsetX;
			lOffsetY = stInfo.siStepY - m_lPrbCamOffsetY;
		}

		return stInfo.bResult;
	}
	else
	{
		REF_TYPE	stInfo;
		SRCH_TYPE	stSrchInfo;

		//Init Message
		stSrchInfo.bNormalDie	= TRUE;
		stSrchInfo.lRefDieNo	= 1;
		stSrchInfo.bShowPRStatus = TRUE;
		stSrchInfo.bDisableBackupAlign = FALSE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		int nConvID = 0;
		if( bBigWindow )
		{
			nConvID = m_comClient.SendRequest("WaferPrStn", "SearchDieInBigWindow", stMsg);
		}
		else
		{
			nConvID = m_comClient.SendRequest("WaferPrStn", "SearchCurrentDie", stMsg);
		}
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if( (stInfo.bStatus == FALSE) )	// PR error
		{
			return FALSE;
		}

		if( (stInfo.bFullDie == FALSE) )	// empty die
		{
			return FALSE;
		}

		lOffsetX = stInfo.lX;	
		lOffsetY = stInfo.lY;
		return TRUE;
	}
}

LONG CWaferTable::ScanCheckDieOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if( (IsPrescanEnable() && IsPrescanEnded()==FALSE) ||
		(IsScnLoaded() && IsAlignedWafer()==FALSE) )
	{
		HmiMessage("Prescan doesn't complete yet!");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BOOL bProbeCam = FALSE;
	SelectWaferCamera();
	CP_MoveTestStageUp(FALSE);

	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
  	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMS896AApp::m_bStopAlign = FALSE;
	ULONG ulJUMP_STEP = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP Scan Check Step"), 3);
	if( ulJUMP_STEP<=0 )
	{
		ulJUMP_STEP = 3;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Scan Check Step"), ulJUMP_STEP);

	ULONG ulMaxRow = 0, ulMaxCol = 0;
	GetWaferMapDimension(ulMaxRow, ulMaxCol);
	ulMaxCol = (ulMaxCol/ulJUMP_STEP)*ulJUMP_STEP;

	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\";
	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szTemp;
	if( bProbeCam )
		szTemp.Format("BondSearch_DC%dPO%d_", pCPInfo->GetDCState(), pCPInfo->GetPOState());
	else
		szTemp.Format("ScanSearch_DC%dPO%d_", pCPInfo->GetDCState(), pCPInfo->GetPOState());
	szTemp.Format("ScanSearch_DC%lu_", GetNewPickCount());
	szLogFileName = szLogFileName + szTemp + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";
	FILE *fp = fopen(szLogFileName, "w");
	LONG lGoX = 0, lGoY = 0, lMaxOffsetX = 0, lMaxOffsetY = 0, lMinOffsetX = 100, lMinOffsetY = 100;
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	DOUBLE dTime = GetTime();
	LONG lPrDelay = m_lCPClbPrDelay;
	lPrDelay = m_lPrescanPrDelay;
	if( fp!=NULL && bProbeCam )
	{
		fprintf(fp, "main probe offset X %ld, Y %ld\n\n", m_lProbeOffsetX, m_lProbeOffsetY);
	}
	if( fp!=NULL )
	{
		fprintf(fp, "row,col,posnx,posny,offx,offy\n");
	}

	UCHAR ucDummyGrade = m_ucDummyPrescanPNPGrade + ucOffset;
	for(ULONG ulRow=0; ulRow<ulMaxRow; ulRow++)
	{
		for(LONG ulCol = 0; ulCol<ulMaxCol; ulCol++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade!=ucNullBin && ucGrade!=ucDummyGrade )
			{
				m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucDummyGrade);
			}
		}
	}

	for(ULONG ulRow=0; ulRow<ulMaxRow; ulRow=ulRow+ulJUMP_STEP)
	{
		LONG ulCol = 0;
		if( ulRow%(ulJUMP_STEP*2)!=0 )
			ulCol = ulMaxCol;
		while( 1 )
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade==ucNullBin )
			{
				if( ulRow%(ulJUMP_STEP*2)!=0 )
				{
					ulCol = ulCol - ulJUMP_STEP;
					if( ulCol<0 )
						break;
				}
				else
				{
					ulCol = ulCol + ulJUMP_STEP;
					if( ulCol>(LONG)ulMaxCol )
						break;
				}
				continue;
			}
			LONG lScanX, lScanY;
			if( GetPrescanWftPosn(ulRow, ulCol, lScanX, lScanY)==FALSE )
			{
				if( ulRow%(ulJUMP_STEP*2)!=0 )
				{
					ulCol = ulCol - ulJUMP_STEP;
					if( ulCol<0 )
						break;
				}
				else
				{
					ulCol = ulCol + ulJUMP_STEP;
					if( ulCol>(LONG)ulMaxCol )
						break;
				}
				continue;
			}
			if( bProbeCam )
			{
				GetProbePosition(lScanX, lScanY, lGoX, lGoY);
			}
			else
			{
				lGoX = lScanX;
				lGoY = lScanY;
			}

			XY_SafeMoveTo(lGoX, lGoY, SFM_WAIT);
			Sleep(lPrDelay);
			GetEncoderValue();
			LONG lGoToX = GetCurrX();
			LONG lGoToY = GetCurrY();
			LONG lOffsetX = 0, lOffsetY = 0;
			BOOL bPrResult = WFT_SearchPrDie(lOffsetX, lOffsetY, bProbeCam);
			LONG lOffsetX2 = 0, lOffsetY2 = 0;
			if( bPrResult )
			{
				if( WFT_SearchPrDie(lOffsetX2, lOffsetY2, bProbeCam) )
				{
					if( labs(lOffsetX)>labs(lOffsetX2) )
						lOffsetX = lOffsetX2;
					if( labs(lOffsetY)>labs(lOffsetY2) )
						lOffsetY = lOffsetY2;
				}
			}
			UCHAR ucNewGrade = 100 + ucOffset;
			if( bPrResult==FALSE )
				ucNewGrade = 200 + ucOffset;
			{
				if( labs(lOffsetX)>125 || labs(lOffsetY)>125 )
				{
					if( labs(lOffsetX)>125 )
						ucNewGrade += (abs(lOffsetX)+25)/50;
					if( labs(lOffsetY)>125 )
						ucNewGrade += ((abs(lOffsetY)+25)/50)*10;
				}
				if( fp!=NULL )
				{
					LONG lHmiRow, lHmiCol;
					ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);
					fprintf(fp, "%4ld,%4ld,%8ld,%8ld,%4ld,%4ld,%4ld,%4ld\n",	//Klocwork //v4.40T8
						lHmiRow, lHmiCol, lGoX, lGoY, lOffsetX, lOffsetY, lOffsetX2, lOffsetY2);
				}
			}

			m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNewGrade);
			m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
			if( (lMaxOffsetX)<(lOffsetX) )
				lMaxOffsetX = lOffsetX;
			if( (lMaxOffsetY)<(lOffsetY) )
				lMaxOffsetY = lOffsetY;
			if( (lMinOffsetX)>(lOffsetX) )
				lMinOffsetX = lOffsetX;
			if( (lMinOffsetY)>(lOffsetY) )
				lMinOffsetY = lOffsetY;
			if( pApp->IsStopAlign() )
			{
				break;
			}
			if( ulRow%(ulJUMP_STEP*2)!=0 )
			{
				ulCol = ulCol - ulJUMP_STEP;
				if( ulCol<0 )
					break;
			}
			else
			{
				ulCol = ulCol + ulJUMP_STEP;
				if( ulCol>(LONG)ulMaxCol )
					break;
			}
		}
		if( pApp->IsStopAlign() )
		{
			break;
		}
	}

	if( fp!=NULL )
	{
		fprintf(fp, "min %ld,%ld max %ld,%ld, used time %.4f\n",	//Klocwork	//v4.40T8
			lMinOffsetX, lMinOffsetY, lMaxOffsetX, lMaxOffsetY, (GetTime()-dTime)/1000.0);
		fclose(fp);
	}

	HmiMessage_Red_Back("Scan check whole wafer complete");
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::FOVCheckDieOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bProbeCam = FALSE;
	if( HmiMessage("Do you want to check probe camera side?", "prescan offset", glHMI_MBX_YESNO)==glHMI_YES )
	{
		bProbeCam = TRUE;
	}

	LONG lLoopCounter = 1;
	while( 1 )
	{
		CString szOpId	= _T("1");
		CString szTitle		= "Please input loop test counter: ";
		BOOL bReturn = HmiStrInputKeyboard(szTitle, szOpId);
		if( bReturn && szOpId.IsEmpty()!=TRUE )
		{
			lLoopCounter = atoi(szOpId);
			break;
		}
	}

	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMS896AApp::m_bStopAlign = FALSE;

	MultiSrchInitNmlDie1(FALSE);	// hmi multi search test button

	GetEncoderValue();
	LONG lGrabX = GetCurrX();
	LONG lGrabY = GetCurrY();

	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lGrabX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lGrabY;

for(int i=0; i<lLoopCounter; i++)
{
	BOOL bDrawDie = FALSE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
	int nConvID = m_comClient.SendRequest("WaferPrStn", "MultiSearchNmlDie1", stMsg);
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

	if( bProbeCam )
	{
		SelectBondCamera();
		CP_MoveTestStageUp(TRUE);
	}
	else
	{
		SelectWaferCamera();
		CP_MoveTestStageUp(FALSE);
	}

	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\";
	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szTemp;

	if( bProbeCam )
		szTemp.Format("FovBond_DC%dPO%d_", pCPInfo->GetDCState(), pCPInfo->GetPOState());
	else
		szTemp.Format("FovScan_DC%dPO%d_", pCPInfo->GetDCState(), pCPInfo->GetPOState());
	szLogFileName = szLogFileName + szTemp + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";
	FILE *fp = fopen(szLogFileName, "w");
	LONG lMaxX = 0, lMaxY = 0, lMinX = 100, lMinY = 100;
	LONG lPrDelay = m_lCPClbPrDelay;
	if( fp!=NULL && bProbeCam )
	{
		fprintf(fp, "main probe offset X %ld, Y %ld\n\n", m_lProbeOffsetX, m_lProbeOffsetY);
	}
	if( fp!=NULL )
	{
		fprintf(fp, "grab die %ld,%ld\n", lGrabX, lGrabY);
		fprintf(fp, "col,row,grd,posnx,posny,movex,movey,offx,offy,realx,realy");
		if( bProbeCam )
			fprintf(fp, ",pdcinfo");
		fprintf(fp, "\n");
	}
	for(ULONG i=1; i<=GetGoodTotalDie(); i++)
	{
		LONG lPosX = 0, lPosY = 0, lOffsetX = 0, lOffsetY = 0;
		DOUBLE dAngle = 0;
		CString szDieBin;
		LONG lRow = 0, lCol = 0;
		GetGoodPosnAll(i, lPosX, lPosY, dAngle, szDieBin, lRow, lCol);
		LONG lScanX = lGrabX + lPosX;
		LONG lScanY = lGrabY + lPosY;
		LONG lMoveX = 0, lMoveY = 0;
		if( bProbeCam )
		{
			GetProbePosition(lScanX, lScanY, lMoveX, lMoveY);
		}
		else
		{
			lMoveX = lScanX;
			lMoveY = lScanY;
		}
		XY_MoveTo(lMoveX, lMoveY, SFM_WAIT);
		Sleep(lPrDelay);
		BOOL bPrResult = WFT_SearchPrDie(lOffsetX, lOffsetY, bProbeCam);
		LONG lOffsetX2 = 0, lOffsetY2 = 0;
		if( bPrResult )
		{
			LONG lOffsetX2, lOffsetY2;
			if( WFT_SearchPrDie(lOffsetX2, lOffsetY2, bProbeCam) )
			{
				if( labs(lOffsetX)>labs(lOffsetX2) )
					lOffsetX = lOffsetX2;
				if( labs(lOffsetY)>labs(lOffsetY2) )
					lOffsetY = lOffsetY2;
			}
		}
		if( (lMaxY)<(lOffsetY) )
			lMaxY = lOffsetY;
		if( (lMinX)>(lOffsetX) )
			lMinX = lOffsetX;
		if( (lMinY)>(lOffsetY) )
			lMinY = lOffsetY;
		if( fp!=NULL )
		{
			LONG ucGrade = 100;
			if( labs(lOffsetX)>=20 )
				ucGrade += (labs(lOffsetX)+5)/10*10;
			if( labs(lOffsetY)>=20 )
				ucGrade += (labs(lOffsetY)+5)/10;
			fprintf(fp, "%3ld,%3ld,%3ld,%8ld,%8ld,%8ld,%8ld,%4ld,%4ld,%8ld,%8ld",
				lCol, lRow, ucGrade, lScanX, lScanY, lMoveX, lMoveY, lOffsetX, lOffsetY, lMoveX+lOffsetX, lMoveY+lOffsetY);
			if( bProbeCam )
				fprintf(fp, ",%s", (LPCTSTR) m_szCP100PdcLog);	//Klocwork	//v4.40T8
			fprintf(fp, "\n");
		}
		if( pApp->IsStopAlign() )
		{
			break;
		}
	}

	if( fp!=NULL )
	{
		fprintf(fp, "min %ld,%ld max %ld,%ld\n", lMinX, lMinY, lMaxX, lMaxY);	//Klocwork	//v4.40T8
		fclose(fp);
	}

	XY_MoveTo(lGrabX, lGrabY, SFM_WAIT);

	if( pApp->IsStopAlign() )
	{
		break;
	}
	Sleep(2000);
}

	HmiMessage_Red_Back("FOV check offset complete!");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CP_SaveData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	SaveCP100ComData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::CP_GridSampleOffset(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();

	if( HmiMessage("Are you sure to do this probe offset sampling?\nProbe stage will move up, pin level ok?", "Prescan", glHMI_MBX_YESNO)==glHMI_YES )
	{
		m_lPrbCamOffsetX = 0;
		m_lPrbCamOffsetY = 0;
		CP_GridFixSampleOffset();
		/*
		if( pCPInfo->GetPODone() )
		{
			HmiMessage_Red_Back("Grid sampling offset complete!");
		}
		*/
	}

	//if( pCPInfo->GetPODone() && HmiMessage("Are you sure to check probe offset sampling?\nProbe stage will move up, pin level ok?", "Prescan", glHMI_MBX_YESNO)==glHMI_YES )
	if( pCPInfo->GetPODone() )
	{
		CP_GridCheckFixOffset();
		//HmiMessage_Red_Back("Grid checking offset complete!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

BOOL CWaferTable::SelectBondCamera()
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest("WaferPrStn", "SelectBondCamera", stMsg);
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

	return bReturn;
}


BOOL CWaferTable::GetProbePosition(CONST LONG lScanX, CONST LONG lScanY, LONG &lProbeX, LONG &lProbeY)
{
	LONG lProbeOffsetX = m_lProbeOffsetX;
	LONG lProbeOffsetY = m_lProbeOffsetY;
	LONG lOffsetX = 0, lOffsetY = 0;
	if( CP_PredictScanProbeOffset(lScanX, lScanY, lOffsetX, lOffsetY) )
	{
		lProbeOffsetX += lOffsetX;
		lProbeOffsetY += lOffsetY;
	}

	lProbeX = lScanX + lProbeOffsetX;
	lProbeY = lScanY + lProbeOffsetY;

	return TRUE;
}

BOOL CWaferTable::GetScanPosition(LONG lProbeX, LONG lProbeY, LONG &lScanX, LONG &lScanY)
{
	LONG lProbeOffsetX = m_lProbeOffsetX;
	LONG lProbeOffsetY = m_lProbeOffsetY;
	lScanX = lProbeX - lProbeOffsetX;
	lScanY = lProbeY - lProbeOffsetY;
	LONG lOffsetX = 0, lOffsetY = 0;
	if( CP_PredictScanProbeOffset(lScanX, lScanY, lOffsetX, lOffsetY) )
	{
		lProbeOffsetX += lOffsetX;
		lProbeOffsetY += lOffsetY;
	}

	lScanX = lProbeX - lProbeOffsetX;
	lScanY = lProbeY - lProbeOffsetY;

	return TRUE;
}



//	Chip Prober probing position offset PO
BOOL CWaferTable::CP_MoveTestStageUp(BOOL bUp)
{
	return TRUE;
}

BOOL CWaferTable::WFT_MoveSearchPrDie(LONG &lMoveX, LONG &lMoveY, LONG &lOffsetX, LONG &lOffsetY)
{
	LONG lPrDelay = m_lCPClbPrDelay;
	CP_MoveTestStageUp(FALSE);
	XY_MoveTo(lMoveX, lMoveY, SFM_WAIT);
	Sleep(lPrDelay*2);
	LONG lScanOX = 0, lScanOY = 0;
	if( WFT_SearchPrDie(lScanOX, lScanOY, FALSE, TRUE)==FALSE )
	{
		return FALSE;
	}

	lMoveX = lMoveX + lScanOX;
	lMoveY = lMoveY + lScanOY;

	XY_MoveTo(lMoveX, lMoveY, SFM_WAIT);
	Sleep(lPrDelay*2);
	if( WFT_SearchPrDie(lScanOX, lScanOY, FALSE)==FALSE )
	{
		return FALSE;
	}
	lMoveX = lMoveX + lScanOX;
	lMoveY = lMoveY + lScanOY;

	LONG lProbeX = lMoveX + m_lProbeOffsetX;
	LONG lProbeY = lMoveY + m_lProbeOffsetY;

	XY_MoveTo(lProbeX, lProbeY, SFM_WAIT);
	CP_MoveTestStageUp(TRUE);
	Sleep(lPrDelay*2);
    if( WFT_SearchPrDie(lOffsetX, lOffsetY, TRUE) )
	{
		LONG lOffsetX2 = 0, lOffsetY2 = 0;
		if( WFT_SearchPrDie(lOffsetX2, lOffsetY2, TRUE) )
		{
			if( labs(lOffsetX2)<labs(lOffsetX) )
				lOffsetX = lOffsetX2;
			if( labs(lOffsetY2)<labs(lOffsetY) )
				lOffsetY = lOffsetY2;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CWaferTable::CP_PredictScanProbeOffset(CONST LONG lScanX, CONST LONG lScanY, LONG &lOffsetX, LONG &lOffsetY)
{
	lOffsetX = 0;
	lOffsetY = 0;

	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();

	m_szCP100PdcLog.Empty();
	if( pCPInfo->GetPOState()==FALSE )
	{
		return FALSE;
	}

	BOOL bUseNearDist = TRUE;
	ULONG ulPointNoUL = 0, ulPointNoUR = 0, ulPointNoLL = 0, ulPointNoLR = 0;
	pCPInfo->GetPOPoint_Region(lScanX, lScanY, ulPointNoUL, ulPointNoUR, ulPointNoLL, ulPointNoLR);

	//	11-------------21
	//	----------------
	//	----------------
	//	12-------------22

	LONG lScanX_11, lScanY_11, lScanX_21, lScanY_21, lScanX_12, lScanY_12, lScanX_22, lScanY_22;
	LONG  lPO_X_11,  lPO_Y_11,  lPO_X_21,  lPO_Y_21,  lPO_X_12,  lPO_Y_12,  lPO_X_22,  lPO_Y_22;

	BOOL bGood_11 = pCPInfo->GetPointPO(ulPointNoUL, lScanX_11, lScanY_11, lPO_X_11, lPO_Y_11);
	BOOL bGood_21 = pCPInfo->GetPointPO(ulPointNoUR, lScanX_21, lScanY_21, lPO_X_21, lPO_Y_21);
	BOOL bGood_12 = pCPInfo->GetPointPO(ulPointNoLL, lScanX_12, lScanY_12, lPO_X_12, lPO_Y_12);
	BOOL bGood_22 = pCPInfo->GetPointPO(ulPointNoLR, lScanX_22, lScanY_22, lPO_X_22, lPO_Y_22);

	if( !bGood_11 && !bGood_21 && !bGood_12 && !bGood_22 )
	{
		m_szCP100PdcLog = "CP BLI err ";
	}
	else
	{
		if( !bGood_11 )
		{
			if( bGood_21 )
			{
				lPO_X_11 = lPO_X_21;
				lPO_Y_11 = lPO_Y_21;
			}
			else if( bGood_12 )
			{
				lPO_X_11 = lPO_X_12;
				lPO_Y_11 = lPO_Y_12;
			}
			else
			{
				lPO_X_11 = lPO_X_22;
				lPO_Y_11 = lPO_Y_22;
			}
		}

		if( !bGood_21 )
		{
			if( bGood_11 )
			{
				lPO_X_21 = lPO_X_11;
				lPO_Y_21 = lPO_Y_11;
			}
			else if( bGood_22 )
			{
				lPO_X_21 = lPO_X_22;
				lPO_Y_21 = lPO_Y_22;
			}
			else
			{
				lPO_X_21 = lPO_X_12;
				lPO_Y_21 = lPO_Y_12;
			}
		}

		if( !bGood_12 )
		{
			if( bGood_11 )
			{
				lPO_X_12 = lPO_X_11;
				lPO_Y_12 = lPO_Y_11;
			}
			else if( bGood_22 )
			{
				lPO_X_12 = lPO_X_22;
				lPO_Y_12 = lPO_Y_22;
			}
			else
			{
				lPO_X_12 = lPO_X_21;
				lPO_Y_12 = lPO_Y_21;
			}
		}

		if( !bGood_22 )
		{
			if( bGood_21 )
			{
				lPO_X_22 = lPO_X_21;
				lPO_Y_22 = lPO_Y_21;
			}
			else if( bGood_12 )
			{
				lPO_X_22 = lPO_X_12;
				lPO_Y_22 = lPO_Y_12;
			}
			else
			{
				lPO_X_22 = lPO_X_11;
				lPO_Y_22 = lPO_Y_11;
			}
		}

		LONG lX1 = max(lScanX_11, lScanX_12);
		LONG lX2 = min(lScanX_21, lScanX_22);
		LONG lY1 = max(lScanY_11, lScanY_21);
		LONG lY2 = min(lScanY_12, lScanY_22);

		if( lScanX>lX1 )
			lX1 = lScanX;
		if( lScanX<lX2 )
			lX2 = lScanX;
		if( lScanY>lY1 )
			lY1 = lScanY;
		if( lScanY<lY2 )
			lY2 = lScanY;
		if( lScanX>lX1 || lScanX<lX2 || lScanY>lY1 || lScanY<lY2 )
		{
			m_szCP100PdcLog.Format("CP BLI out, X%4ld(%4ld,%4ld,%4ld,%4ld), Y%4ld(%4ld,%4ld,%4ld,%4ld), X%8ld,(%8ld,%8ld), Y%8ld,(%8ld,%8ld) ", 
				lOffsetX, lPO_X_11, lPO_X_21, lPO_X_12, lPO_X_22,
				lOffsetY, lPO_Y_11, lPO_Y_21, lPO_Y_12, lPO_Y_22,
				lScanX, lX1, lX2, lScanY, lY1, lY2);
		}
		else
		{
			lOffsetX = pCPInfo->BiLinearInterpolation(lX1, lY1, lX2, lY2, lPO_X_11, lPO_X_21, lPO_X_12, lPO_X_22, lScanX, lScanY);
			lOffsetY = pCPInfo->BiLinearInterpolation(lX1, lY1, lX2, lY2, lPO_Y_11, lPO_Y_21, lPO_Y_12, lPO_Y_22, lScanX, lScanY);

			m_szCP100PdcLog.Format("CP BLI pdc,%4ld,%4ld,%4ld,%4ld X%4ld(%4ld,%4ld,%4ld,%4ld), Y%4ld(%4ld,%4ld,%4ld,%4ld), X%8ld,(%8ld,%8ld), Y%8ld,(%8ld,%8ld)", 
				ulPointNoUL, ulPointNoUR, ulPointNoLL, ulPointNoLR,
				lOffsetX, lPO_X_11, lPO_X_21, lPO_X_12, lPO_X_22,
				lOffsetY, lPO_Y_11, lPO_Y_21, lPO_Y_12, lPO_Y_22,
				lScanX, lX1, lX2, lScanY, lY1, lY2);
			bUseNearDist = FALSE;
		}
	}

	if( bUseNearDist )
	{
		// find a nearest point and use that point to predict die shift
		ULONG ulPointNo = 0;
		LONG lPointX = 0, lPointY = 0;
		if( pCPInfo->GetNearestPointPO(lScanX, lScanY, lPointX, lPointY, lOffsetX, lOffsetY, ulPointNo)==FALSE )
		{
			m_szCP100PdcLog = m_szCP100PdcLog + "CP NER err";
			return FALSE;
		}

		CString szMsg;
		szMsg.Format("CP NER pdc, X%4ld, Y%4ld, X%8ld,(%8ld), Y%8ld,(%8ld), %4ld", 
			lOffsetX, lOffsetY, lScanX, lPointX, lScanY, lPointY, ulPointNo);
		m_szCP100PdcLog = m_szCP100PdcLog + szMsg;
	}

	return TRUE;
}

ULONG CWaferTable::CP_GetGridRows()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulGridRows = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP Grid Rows"), 40);
	if( ulGridRows<1 )
	{
		ulGridRows = 10;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Grid Rows"), ulGridRows);

	return ulGridRows;
}

ULONG CWaferTable::CP_GetGridCols()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulGridCols = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP Grid Cols"), 40);
	if( ulGridCols<1 )
	{
		ulGridCols = 10;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Grid Cols"), ulGridCols);

	return ulGridCols;
}

LONG CWaferTable::CP_DetectFixTargetOffset(ULONG ulPointNo, LONG &lScanX, LONG &lScanY, LONG &lOffsetX, LONG &lOffsetY)
{
//	m_lSprialSize = 1;
//	SpiralSearchHomeDie();
	BOOL bFindTgtDie = WFT_MoveSearchPrDie(lScanX, lScanY, lOffsetX, lOffsetY);
	if( lOffsetX==0 && lOffsetY==0 )
	{
	//	bFindTgtDie = FALSE;
	}
	if( labs(lOffsetX)>labs(GetDiePitchX_X()*45/100) || labs(lOffsetY)>labs(GetDiePitchY_Y())*45/100 )
	{
		bFindTgtDie = FALSE;
	}

	if( bFindTgtDie==FALSE )
	{
		lOffsetX = WT_PO_OFFSET_FAIL;
		lOffsetY = WT_PO_OFFSET_FAIL;
	}

	return bFindTgtDie;
}

BOOL CWaferTable::CP_GridFixSampleOffset()
{
	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	ULONG ulGridRows = CP_GetGridRows();
	ULONG ulGridCols = CP_GetGridCols();
    CMS896AApp::m_bStopAlign = FALSE;
	pCPInfo->InitPOPoints(ulGridRows, ulGridCols);

	LONG lWaferDiameter = GetWaferDiameter();
	LONG lUL_X = GetWaferCenterX() + lWaferDiameter/2;
	LONG lUL_Y = GetWaferCenterY() + lWaferDiameter/2;
	LONG	lSpanY = labs(lWaferDiameter/ulGridRows);
	LONG	lSpanX = labs(lWaferDiameter/ulGridCols);
	pCPInfo->SetGridSpan(lSpanX, lSpanY);

	// sampling at scan camera side.
	SelectWaferCamera();
	CP_MoveTestStageUp(FALSE);
	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		LONG lCol = 0;
		if( lRow%2!=0 )
			lCol = ulGridCols;
		while( 1 )
		{
			if( pApp->IsStopAlign() )
			{
				pCPInfo->InitPOPoints(ulGridRows, ulGridCols);
				HmiMessage_Red_Back("Probing offset sampling operation stopped, please do it again!");
				return FALSE;
			}

			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;

			LONG lScanX = lUL_X - lCol*lSpanX;
			LONG lScanY = lUL_Y - lRow*lSpanY;
			LONG lMoveX = lScanX;
			LONG lMoveY = lScanY;
			LONG lOffsetX = WT_PO_OFFSET_INIT;
			LONG lOffsetY = WT_PO_OFFSET_INIT;
			LONG lState = 0;
			if( IsWithinWaferLimit(lScanX, lScanY) )
			{
				lState = CP_SampleScanPosition(lMoveX, lMoveY);	// to do auto offset at this die or around
			}
			pCPInfo->SetPointPO(ulPointNo, lScanX, lScanY, lOffsetX, lOffsetY, lState);
			pCPInfo->SetPointSP(ulPointNo, lMoveX, lMoveY);
			if( lRow%2!=0 )
			{
				lCol--;
				if( lCol<0 )
				{
					break;
				}
			}
			else
			{
				lCol++;
				if( lCol>ulGridCols )
				{
					break;
				}
			}
		}
	}

	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szFileName = gszUSER_DIRECTORY + "\\History\\ProbeLog_" + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";
	FILE *fp = fopen(szFileName, "w");
	SelectBondCamera();
	BOOL bFirstMove = FALSE;
	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		LONG lCol = 0;
		if( lRow%2!=0 )
			lCol = ulGridCols;
		while( 1 )
		{
			if( pApp->IsStopAlign() )
			{
				pCPInfo->InitPOPoints(ulGridRows, ulGridCols);
				HmiMessage_Red_Back("Probing offset sampling operation stopped, please do it again!");
				if( fp!=NULL )
					fclose(fp);
				return FALSE;
			}

			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lScanX = lUL_X - lCol*lSpanX;
			LONG lScanY = lUL_Y - lRow*lSpanY;
			LONG lMoveX = lScanX;
			LONG lMoveY = lScanY;
			LONG lOffsetX = WT_PO_OFFSET_INIT;
			LONG lOffsetY = WT_PO_OFFSET_INIT;
			LONG lState = 0;

			lState = pCPInfo->GetPointPO(ulPointNo, lScanX, lScanY, lOffsetX, lOffsetY);
			pCPInfo->GetPointSP(ulPointNo, lMoveX, lMoveY);
			if( lState!=0 )
			{
				if( bFirstMove==FALSE )
				{
					LONG lProbeX = lMoveX + m_lProbeOffsetX;
					LONG lProbeY = lMoveY + m_lProbeOffsetY;
					XY_MoveTo(lProbeX, lProbeY, SFM_WAIT);
					CP_MoveTestStageUp(TRUE);
					bFirstMove = TRUE;
				}
				lState = CP_DetectProbeOffset(lMoveX, lMoveY, lOffsetX, lOffsetY);	// to do auto offset at this die or around
			}
			pCPInfo->SetPointPO(ulPointNo, lScanX, lScanY, lOffsetX, lOffsetY, lState);
			if( fp!=NULL )
			{
				LONG lProbeX = lMoveX + m_lProbeOffsetX;
				LONG lProbeY = lMoveY + m_lProbeOffsetY;
				fprintf(fp, "%lu,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",		//Klocwork	//v4.46
								ulPointNo, lScanX, lScanY, lMoveX, lMoveY, 
								lProbeX, lProbeY, lProbeX + lOffsetX, lProbeY + lOffsetY);
			}
			if( lRow%2!=0 )
			{
				lCol--;
				if( lCol<0 )
				{
					break;
				}
			}
			else
			{
				lCol++;
				if( lCol>ulGridCols )
				{
					break;
				}
			}
		}
	}
	if( fp!=NULL )
		fclose(fp);

	szFileName = gszUSER_DIRECTORY + "\\History\\ProbeOffset_" + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";
	fp = fopen(szFileName, "w");
	if( fp!=NULL )
	{
		fprintf(fp, "main probe offset X %ld, Y %ld\n\n", m_lProbeOffsetX, m_lProbeOffsetY);
		fprintf(fp, "\n Fill in with surrounding good samples if center one is fail\n");
	}
	for(ULONG lRow=1; lRow<ulGridRows; lRow++)
	{
		for(ULONG lCol = 1; lCol<ulGridCols; lCol++)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lCheckX = 0, lCheckY = 0, lCheckOX = 0, lCheckOY = 0;
			if( pCPInfo->GetPointPO(ulPointNo, lCheckX, lCheckY, lCheckOX, lCheckOY)==FALSE )
			{
				LONG lScanX = 0, lScanY = 0, lOffsetX = 0, lOffsetY = 0;
				// left
				if( pCPInfo->GetPointPO(ulPointNo-1, lScanX, lScanY, lOffsetX, lOffsetY)==FALSE )
				{
					continue;
				}
				lCheckOX = lOffsetX;
				lCheckOY = lOffsetY;
				// right
				if( pCPInfo->GetPointPO(ulPointNo+1, lScanX, lScanY, lOffsetX, lOffsetY)==FALSE )
				{
					continue;
				}
				lCheckOX += lOffsetX;
				lCheckOY += lOffsetY;
				// top
				if( pCPInfo->GetPointPO(ulPointNo-ulGridCols-1, lScanX, lScanY, lOffsetX, lOffsetY)==FALSE )
				{
					continue;
				}
				lCheckOX += lOffsetX;
				lCheckOY += lOffsetY;
				// bottom
				if( pCPInfo->GetPointPO(ulPointNo+ulGridCols+1, lScanX, lScanY, lOffsetX, lOffsetY)==FALSE )
				{
					continue;
				}
				lCheckOX += lOffsetX;
				lCheckOY += lOffsetY;

				// average it
				lCheckOX = lCheckOX/4;
				lCheckOY = lCheckOY/2;
				pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
				if( fp!=NULL )
				{
					fprintf(fp, "CP IN, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
				}
			}
		}
	}
#if 0
	if( fp!=NULL )
	{
		fprintf(fp, "\n UL adjust result\n");
	}
	for(ULONG lRow=0; lRow<=ulGridRows/2; lRow++)
	{
		for(ULONG lCol = 0; lCol<=ulGridCols/2; lCol++)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lCheckX = 0, lCheckY = 0, lCheckOX = 0, lCheckOY = 0;
			if( pCPInfo->GetPointPO(ulPointNo, lCheckX, lCheckY, lCheckOX, lCheckOY)==FALSE )
			{
				LONG lScanX = 0, lScanY = 0, lOffsetX = 0, lOffsetY = 0;
				if( pCPInfo->GetPointPO(ulPointNo+1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX + lSpanX;
					lCheckY = lScanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP UL, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
				if( pCPInfo->GetPointPO(ulPointNo+ulGridCols+1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX;
					lCheckY = lScanY + lSpanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP down, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
			}
		}
	}

	if( fp!=NULL )
	{
		fprintf(fp, "\n UR adjust result\n");
	}
	for(ULONG lRow=0; lRow<=ulGridRows/2; lRow++)
	{
		for(ULONG lCol=ulGridCols; lCol>=ulGridCols/2; lCol--)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lCheckX = 0, lCheckY = 0, lCheckOX = 0, lCheckOY = 0;
			if( pCPInfo->GetPointPO(ulPointNo, lCheckX, lCheckY, lCheckOX, lCheckOY)==FALSE )
			{
				LONG lScanX = 0, lScanY = 0, lOffsetX = 0, lOffsetY = 0;
				if( pCPInfo->GetPointPO(ulPointNo-1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX - lSpanX;
					lCheckY = lScanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP UL, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
				if( pCPInfo->GetPointPO(ulPointNo+ulGridCols+1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX;
					lCheckY = lScanY + lSpanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP down, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
			}
		}
	}

	if( fp!=NULL )
	{
		fprintf(fp, "\n LR adjust result\n");
	}
	for(ULONG lRow=ulGridRows; lRow>=ulGridRows/2; lRow--)
	{
		for(ULONG lCol=ulGridCols; lCol>=ulGridCols/2; lCol--)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lCheckX = 0, lCheckY = 0, lCheckOX = 0, lCheckOY = 0;
			if( pCPInfo->GetPointPO(ulPointNo, lCheckX, lCheckY, lCheckOX, lCheckOY)==FALSE )
			{
				LONG lScanX = 0, lScanY = 0, lOffsetX = 0, lOffsetY = 0;
				if( pCPInfo->GetPointPO(ulPointNo-1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX - lSpanX;
					lCheckY = lScanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP rigt, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
				if( pCPInfo->GetPointPO(ulPointNo-ulGridCols-1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX;
					lCheckY = lScanY - lSpanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP down, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
			}
		}
	}

	if( fp!=NULL )
	{
		fprintf(fp, "\n LL adjust result\n");
	}
	for(ULONG lRow=ulGridRows; lRow>=ulGridRows/2; lRow--)
	{
		for(ULONG lCol = 0; lCol<=ulGridCols/2; lCol++)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lCheckX = 0, lCheckY = 0, lCheckOX = 0, lCheckOY = 0;
			if( pCPInfo->GetPointPO(ulPointNo, lCheckX, lCheckY, lCheckOX, lCheckOY)==FALSE )
			{
				LONG lScanX = 0, lScanY = 0, lOffsetX = 0, lOffsetY = 0;
				if( pCPInfo->GetPointPO(ulPointNo+1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX + lSpanX;
					lCheckY = lScanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP rigt, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
				if( pCPInfo->GetPointPO(ulPointNo-ulGridCols-1, lScanX, lScanY, lOffsetX, lOffsetY) )
				{
					lCheckX = lScanX;
					lCheckY = lScanY - lSpanY;
					pCPInfo->SetPointPO(ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY, 1);
					if( fp!=NULL )
					{
						fprintf(fp, "CP down, %5lu,%8ld,%8ld,%4ld,%4ld\n", ulPointNo, lCheckX, lCheckY, lOffsetX, lOffsetY);
					}
					continue;
				}
			}
		}
	}
#endif

	if( fp!=NULL )
	{
		fprintf(fp, "\n sample span %6ld,%6ld and die pitch %4ld,%4ld\n\n", lSpanX, lSpanY, GetDiePitchX_X(), GetDiePitchY_Y());
	}
	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		LONG lScanX = 0, lScanY = 0, lMoveX = 0, lMoveY = 0, lOffsetX = 0, lOffsetY = 0;
		for(ULONG lCol = 0; lCol<=ulGridCols; lCol++)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			pCPInfo->GetPointSP(ulPointNo, lMoveX, lMoveY);
			LONG lState = pCPInfo->GetPointPO(ulPointNo, lScanX, lScanY, lOffsetX, lOffsetY);
			if( fp!=NULL )
			{
				fprintf(fp, "%5lu, %8ld, %8ld,%7ld,%7ld, %8ld, %8ld\n", ulPointNo, lScanX, lScanY, lOffsetX, lOffsetY, lMoveX-lScanX, lMoveY-lScanY);
			}
		}
	}

	if( fp!=NULL )
	{
		fprintf(fp, "\n CP shape\n\n\n");
	}
	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		LONG lScanX = 0, lScanY = 0, lOffsetX = 0, lOffsetY = 0;
		for(ULONG lCol = 0; lCol<=ulGridCols; lCol++)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lState = pCPInfo->GetPointPO(ulPointNo, lScanX, lScanY, lOffsetX, lOffsetY);
			if( fp!=NULL )
			{
				fprintf(fp, " %ld", lState);	//Klocwork	//v4.40T8
			}
		}
		if( fp!=NULL )
		{
			fprintf(fp, "\n\n");
		}
	}

	if( fp!=NULL )
	{
		fclose(fp);
	}

	m_lPOProbeOffsetX	= m_lProbeOffsetX;
	m_lPOProbeOffsetY	= m_lProbeOffsetY;

	pCPInfo->SetPOState(TRUE);
	pCPInfo->SetPODone(TRUE);
	SaveCP100ComData();

	return TRUE;
}

BOOL CWaferTable::CP_GridCheckFixOffset()
{
	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG lPrDelay = m_lCPClbPrDelay;

	ULONG ulGridRows = CP_GetGridRows();
	ULONG ulGridCols = CP_GetGridCols();
    CMS896AApp::m_bStopAlign = FALSE;

	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szFileName = gszUSER_DIRECTORY + "\\History\\ProbeCheck_" + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";
	FILE *fp = fopen(szFileName, "w");

	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		for(ULONG lCol=0; lCol<=ulGridCols; lCol++)
		{
			if( pApp->IsStopAlign() )
			{
				HmiMessage_Red_Back("Probing offset checking operation stopped, please do it again!");
				if( fp!=NULL )
					fclose(fp);
				return FALSE;
			}

			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lGoX, lGoY;
			if( pCPInfo->GetPointSP(ulPointNo, lGoX, lGoY) )
			{
				if( fp!=NULL )
					fprintf(fp, "%lu,%ld,%ld", ulPointNo, lGoX, lGoY);		//Klocwork	//v4.46
				GetProbePosition(lGoX, lGoY, lGoX, lGoY);
				XY_MoveTo(lGoX, lGoY, SFM_WAIT);
				Sleep(lPrDelay);
				LONG lOffsetX = 0, lOffsetY = 0;
				WFT_SearchPrDie(lOffsetX, lOffsetY, TRUE);
				if( fp!=NULL )
					fprintf(fp, "%ld,%ld,%ld,%ld\n", lGoX, lGoY, lGoX + lOffsetX, lGoY + lOffsetY);		//Klocwork	//v4.46
			}
		}
	}

	if( fp!=NULL )
		fclose(fp);

	return TRUE;
}

LONG CWaferTable::CP_SampleScanPosition(LONG &lMoveX, LONG &lMoveY)
{
	LONG lPrDelay = m_lCPClbPrDelay;
	XY_MoveTo(lMoveX, lMoveY, SFM_WAIT);
	Sleep(lPrDelay*2);
	LONG lScanOX = 0, lScanOY = 0;
	if( WFT_SearchPrDie(lScanOX, lScanOY, FALSE, TRUE)==FALSE )
	{
		return FALSE;
	}

	lMoveX = lMoveX + lScanOX;
	lMoveY = lMoveY + lScanOY;

	XY_MoveTo(lMoveX, lMoveY, SFM_WAIT);
	Sleep(lPrDelay*2);
	if( WFT_SearchPrDie(lScanOX, lScanOY, FALSE)==FALSE )
	{
		return FALSE;
	}
	lMoveX = lMoveX + lScanOX;
	lMoveY = lMoveY + lScanOY;

	return TRUE;
}

LONG CWaferTable::CP_DetectProbeOffset(LONG lMoveX, LONG lMoveY, LONG &lOffsetX, LONG &lOffsetY)
{
	LONG lPrDelay = m_lCPClbPrDelay;

	LONG lProbeX = lMoveX + m_lProbeOffsetX;
	LONG lProbeY = lMoveY + m_lProbeOffsetY;

	XY_MoveTo(lProbeX, lProbeY, SFM_WAIT);
	Sleep(lPrDelay*2);
    if( WFT_SearchPrDie(lOffsetX, lOffsetY, TRUE)==FALSE )
	{
		return FALSE;
	}

	LONG lOffsetX2 = 0, lOffsetY2 = 0;
	BOOL bFindTgtDie = WFT_SearchPrDie(lOffsetX2, lOffsetY2, TRUE);
	if( bFindTgtDie )
	{
		if( labs(lOffsetX2)<labs(lOffsetX) )
			lOffsetX = lOffsetX2;
		if( labs(lOffsetY2)<labs(lOffsetY) )
			lOffsetY = lOffsetY2;
	}

	if( lOffsetX==0 && lOffsetY==0 )
	{
	//	bFindTgtDie = FALSE;
	}

	if( labs(lOffsetX)>labs(GetDiePitchX_X()*45/100) || labs(lOffsetY)>labs(GetDiePitchY_Y())*45/100 )
	{
		bFindTgtDie = FALSE;
	}

	if( bFindTgtDie==FALSE )
	{
		lOffsetX = WT_PO_OFFSET_FAIL;
		lOffsetY = WT_PO_OFFSET_FAIL;
	}

	return bFindTgtDie;
}
//	Chip Prober probing position offset PO


LONG CWaferTable::XY_MoveToProbeViaScanCmd(IPC_CServiceMessage& svMsg)
{
	typedef struct {
		LONG lX;
		LONG lY;
	} WT_XY_STRUCT;

	WT_XY_STRUCT stPos;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);
	
	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (!m_bSel_X || !m_bSel_Y)
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	LONG lScanX = stPos.lX;
	LONG lScanY = stPos.lY;

	INT nResult = 0;
	nResult = X1_Profile(LOW_PROF);
	nResult = Y1_Profile(LOW_PROF);
	if (nResult != gnOK)
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	LONG lGoX = 0, lGoY = 0;
	GetProbePosition(lScanX, lScanY, lGoX, lGoY);

	if( XY_MoveTo(lGoX, lGoY, SFM_WAIT) == gnOK )
		bResult = TRUE;
	else
		bResult = FALSE;

	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CWaferTable::WT_PitchMoveX(IPC_CServiceMessage& svMsg)
{
	typedef struct {
		BOOL bDir;
		LONG lDist;
	} WT_XY_STRUCT;

	WT_XY_STRUCT stPos;

	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);

	LONG lDiePitchX_X	= GetDiePitchX_X();
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();

	LONG lDiffX = 1, lDiffY = 0;
	if( stPos.bDir==FALSE )
		lDiffX = -1;
	X_Sync();
	Y_Sync();
	GetEncoderValue();
	LONG lOldX = GetCurrX();
	LONG lOldY = GetCurrY();
	LONG	lTgtWfX = lOldX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
	LONG	lTgtWfY = lOldY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;

	XY_MoveTo(lTgtWfX, lTgtWfY);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::WT_PitchMoveY(IPC_CServiceMessage& svMsg)
{
	typedef struct {
		BOOL bDir;
		LONG lDist;
	} WT_XY_STRUCT;

	WT_XY_STRUCT stPos;

	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);

	LONG lDiePitchX_X	= GetDiePitchX_X();
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();

	LONG lDiffX = 0, lDiffY = 1;
	if( stPos.bDir==FALSE )
		lDiffY = -1;
	X_Sync();
	Y_Sync();
	GetEncoderValue();
	LONG lOldX = GetCurrX();
	LONG lOldY = GetCurrY();
	LONG	lTgtWfX = lOldX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
	LONG	lTgtWfY = lOldY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;

	XY_MoveTo(lTgtWfX, lTgtWfY);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::MoveSearchToUL(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	LONG lX = 0, lY = 0, lT = 0;
	GetEncoder(&lX, &lY, &lT);
	if( WftMoveSearchDie(lX, lY, TRUE) )
	{
		XY_SafeMoveTo(lX, lY);

		typedef struct
		{
			LONG lRow;
			LONG lCol;
		}	GRABPR;
		GRABPR stData;

		LONG lSpanRow = m_lBlockProbeSpanRow + 1;
		LONG lSpanCol = m_lBlockProbeSpanCol + 1;
		LONG lGridRow = m_lBlockProbeRow;
		LONG lGridCol = m_lBlockProbeCol;

		LONG lCoverRow = (lGridRow - 1) * lSpanRow;
		LONG lCoverCol = (lGridCol - 1) * lSpanCol;

		IPC_CServiceMessage stMsg;
		for(LONG lStepRow=0; lStepRow<lGridRow; lStepRow++)
		{
			for(LONG lStepCol=0; lStepCol<lGridCol; lStepCol++)
			{
				UCHAR ucOffsetRow = lStepRow*lSpanRow;
				UCHAR ucOffsetCol = lStepCol*lSpanCol;
				stData.lCol = ucOffsetCol*2 - lCoverCol;
				stData.lRow = ucOffsetRow*2 - lCoverRow;
				stMsg.InitMessage(sizeof(stData), &stData);
				INT nConvID = m_comClient.SendRequest("WaferPrStn", "DrawBlockProbeDie", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID,stMsg);
						break;
					}
					else
					{
						Sleep(1);
					}
				}
			}
		}	//	block probe draw die on PR window
	}
	else
	{
		HmiMessage("Please locate on a normal die and do again.", "Prober setup");
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferTable::IsProbingEnd()
{
	return m_bIsWaferEnded;
}
