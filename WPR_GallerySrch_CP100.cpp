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

//	Chip Prober probing position offset PO
LONG CWaferPr::GridLearnDieCalibration(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if ( PR_NotInit() )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( IsNormalDieLearnt() == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lPrDelay = 100;
	svMsg.GetMsg(sizeof(LONG), &lPrDelay);
	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	if( HmiMessage("Are you sure to do grid die calibration sampling?", "Prescan", glHMI_MBX_YESNO)==glHMI_YES )
	{
		CP_GridFixDieCalibration(lPrDelay);
		HmiMessage_Red_Back("Grid die calibration sampling complete!");
	}

	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::GridLearnDieCalibration_AutoMode(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if ( PR_NotInit() )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( IsNormalDieLearnt() == FALSE)
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lPrDelay = 100;
	svMsg.GetMsg(sizeof(LONG), &lPrDelay);
	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);
	CP_GridFixDieCalibration(lPrDelay);

	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CWaferPr::CP_GridFixDieCalibration(LONG lPrDelay)
{
	WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	PR_UWORD		usDieType;
	PR_COORD		stDiePosition; 
	PR_REAL			fDieRotate; 

    CMS896AApp::m_bStopAlign = FALSE;
	pCPInfo->InitDCPoints();

	INT nSubCut = pApp->GetProfileInt(gszPROFILE_SETTING, _T("CP Local DC Sub Cut"), 4);
	if( nSubCut==0 || nSubCut>8 )
		nSubCut = 4;
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("CP Local DC Sub Cut"), nSubCut);

	INT	lSpanX = 20000;
	INT	lSpanY = 20000;
	PR_COORD	stRelMove;
	stRelMove.x = GetPRWinLRX( )- GetPRWinULX();
	stRelMove.y = GetPRWinLRY() - GetPRWinULY();
	ConvertPixelToMotorStep(stRelMove, &lSpanX, &lSpanY);
	lSpanX = labs(lSpanX)/nSubCut;
	lSpanY = labs(lSpanY)/nSubCut;

	LONG lWaferDiameter = GetWaferDiameter();
	LONG lUL_X = GetWaferCenterX() + lWaferDiameter/2;
	LONG lUL_Y = GetWaferCenterY() + lWaferDiameter/2;
	ULONG ulGridCols = 10;
	ULONG ulGridRows = 10;
	if( lSpanX>0 && lSpanY>0 )
	{
		ulGridCols = (lWaferDiameter+lSpanX/2)/lSpanX;
		ulGridRows = (lWaferDiameter+lSpanY/2)/lSpanY;
	}

	CTime ctDateTime;
	ctDateTime = CTime::GetCurrentTime();
	CString szFileName = gszUSER_DIRECTORY + "\\History\\DieCalibration_" + ctDateTime.Format("%Y%m%d%H%M%S") + ".txt";

	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szFileName, "w");
	for (ULONG lRow = 0; lRow <= ulGridRows; lRow++)
	{
		for (ULONG lCol = 0; lCol <= ulGridCols; lCol++)
		{
			if (pApp->IsStopAlign())
			{
				pCPInfo->InitDCPoints();
				HmiMessage_Red_Back("Grid die calibration sampling stopped, please do it again!");
				if ((nErr == 0) && (fp != NULL))
				{
					fclose(fp);
				}
				return FALSE;
			}

			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;

			LONG lScanX = lUL_X - lCol*lSpanX;
			LONG lScanY = lUL_Y - lRow*lSpanY;
			DOUBLE	dClbX	= GetCalibX();
			DOUBLE	dClbXY	= GetCalibXY();
			DOUBLE	dClbY	= GetCalibY();
			DOUBLE	dClbYX	= GetCalibYX();

			LONG lState = 0;
			if( IsWithinWT1WaferLimit(lScanX, lScanY) )
			{
				MoveWaferTable(lScanX, lScanY);
				Sleep(lPrDelay);
				PR_WIN stSrchWin;
				stSrchWin = GetSrchArea();
				ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePosition, stSrchWin);
				if (DieIsAlignable(usDieType) == TRUE)
				{
					ManualDieCompenate(stDiePosition, fDieRotate);
					Sleep(lPrDelay);
					if( GridFindDieCalibration(lPrDelay, nSubCut)==0 )
					{
						lState = 1;
						dClbX	= m_dGridCalibX;
						dClbXY	= m_dGridCalibXY;
						dClbY	= m_dGridCalibY;
						dClbYX	= m_dGridCalibYX;
						if( fabs( (dClbX-GetCalibX())*100/GetCalibX() ) > 5 ||
							fabs( (dClbY-GetCalibY())*100/GetCalibY() ) > 5 )
						{
							lState = 0;
						}
					}

					if ((nErr == 0) && (fp != NULL))
					{
						CTime ctDateTime;
						ctDateTime = CTime::GetCurrentTime();
						CString szTime = ctDateTime.Format("%H%M%S");
						fprintf(fp, "%s - %s\n", (LPCTSTR) szTime, (LPCTSTR) m_szGridDCLog);	//Klocwork	//v4.46
					}
				}
			}
			pCPInfo->SetPointDC(ulPointNo, lScanX, lScanY, dClbX, dClbXY, dClbY, dClbYX, lState);
		}
	}

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "main calibration %3.4f, %3.4f, %3.4f, %3.4f\n", GetCalibX(), GetCalibXY(), GetCalibY(), GetCalibYX());
		fprintf(fp, "sample span %6d,%6d and die pitch %4ld,%4ld\n\n", lSpanX, lSpanY, GetDiePitchX_X(), GetDiePitchY_Y());
	}
	DOUBLE dMaxClbX = -20, dMinClbX = 20, dMaxClbXY = -20, dMinClbXY = 20;
	DOUBLE dMaxClbY = -20, dMinClbY = 20, dMaxClbYX = -20, dMinClbYX = 20;
	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		for(ULONG lCol = 0; lCol<=ulGridCols; lCol++)
		{
			LONG lScanX = 0, lScanY = 0;
			DOUBLE	dClbX	= 0, dClbXY	= 0, dClbY	= 0, dClbYX	= 0;

			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lState = pCPInfo->GetPointDC(ulPointNo, lScanX, lScanY, dClbX, dClbXY, dClbY, dClbYX);
			if( lState==1 )
			{
				if( dMaxClbX<dClbX )
					dMaxClbX = dClbX;
				if( dMinClbX>dClbX )
					dMinClbX = dClbX;
				if( dMaxClbXY<dClbXY )
					dMaxClbXY = dClbXY;
				if( dMinClbXY>dClbXY )
					dMinClbXY = dClbXY;
				if( dMaxClbY<dClbY )
					dMaxClbY = dClbY;
				if( dMinClbY>dClbY )
					dMinClbY = dClbY;
				if( dMaxClbYX<dClbYX )
					dMaxClbYX = dClbYX;
				if( dMinClbYX>dClbYX )
					dMinClbYX = dClbYX;
			}
			if ((nErr == 0) && (fp != NULL))
			{
				fprintf(fp, "%5lu(%ld), %8ld, %8ld, %3.4f, %3.4f, %3.4f, %3.4f\n", ulPointNo, lState, lScanX, lScanY, dClbX, dClbXY, dClbY, dClbYX);
			}
		}
	}

	// if up/down/left/right is ok, use avarage.
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%3.4f, %3.4f, %3.4f, %3.4f, %3.4f, %3.4f, %3.4f, %3.4f\n", 
			dMaxClbX, dMinClbX, dMaxClbXY, dMinClbXY, 
			dMaxClbY, dMinClbY, dMaxClbYX, dMinClbYX);
		fprintf(fp, "\n Fill in with surrounding good samples if center one is fail\n");
	}
	for(ULONG lRow=1; lRow<ulGridRows; lRow++)
	{
		for(ULONG lCol = 1; lCol<ulGridCols; lCol++)
		{
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lCheckX = 0, lCheckY = 0;
			if( pCPInfo->GetPointDC(ulPointNo, lCheckX, lCheckY)==FALSE )
			{
				DOUBLE	dChkClbX = 0,	dChkClbXY = 0,	dChkClbY = 0, dChkClbYX	= 0;
				DOUBLE	dClbX	= 0,	dClbXY	= 0,	dClbY	= 0, dClbYX	= 0;
				// left
				if( pCPInfo->GetPointDC(ulPointNo-1, dClbX, dClbXY, dClbY, dClbYX)==FALSE )
				{
					continue;
				}
				dChkClbX	+= dClbX;
				dChkClbXY	+= dClbXY;
				dChkClbY	+= dClbY;
				dChkClbYX	+= dClbYX;
				// right
				if( pCPInfo->GetPointDC(ulPointNo+1, dClbX, dClbXY, dClbY, dClbYX)==FALSE )
				{
					continue;
				}
				dChkClbX	+= dClbX;
				dChkClbXY	+= dClbXY;
				dChkClbY	+= dClbY;
				dChkClbYX	+= dClbYX;
				// top
				if( pCPInfo->GetPointDC(ulPointNo-ulGridCols-1, dClbX, dClbXY, dClbY, dClbYX)==FALSE )
				{
					continue;
				}
				dChkClbX	+= dClbX;
				dChkClbXY	+= dClbXY;
				dChkClbY	+= dClbY;
				dChkClbYX	+= dClbYX;
				// bottom
				if( pCPInfo->GetPointDC(ulPointNo+ulGridCols+1, dClbX, dClbXY, dClbY, dClbYX)==FALSE )
				{
					continue;
				}
				dChkClbX	+= dClbX;
				dChkClbXY	+= dClbXY;
				dChkClbY	+= dClbY;
				dChkClbYX	+= dClbYX;

				// average it
				dChkClbX	= dChkClbX/4;
				dChkClbXY	= dChkClbXY/4;
				dChkClbY	= dChkClbY/4;
				dChkClbYX	= dChkClbYX/4;
				pCPInfo->SetPointDC(ulPointNo, lCheckX, lCheckY, dChkClbX, dChkClbXY, dChkClbY, dChkClbYX, 1);
				if ((nErr == 0) && (fp != NULL))
				{
					fprintf(fp, "%5lu, %8ld, %8ld, %3.4f, %3.4f, %3.4f, %3.4f\n", ulPointNo, lCheckX, lCheckY, dChkClbX, dChkClbXY, dChkClbY, dChkClbYX);
				}
			}
		}
	}

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "\n CP shape\n\n\n");
	}
	for(ULONG lRow=0; lRow<=ulGridRows; lRow++)
	{
		for(ULONG lCol = 0; lCol<=ulGridCols; lCol++)
		{
			LONG lScanX = 0, lScanY = 0;
			ULONG ulPointNo = lRow*(ulGridCols+1) + lCol + 1;
			LONG lState = pCPInfo->GetPointDC(ulPointNo, lScanX, lScanY);
			if ((nErr == 0) && (fp != NULL))
			{
				fprintf(fp, " %ld", lState);
			}
		}

		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "\n\n");
		}
	}

	if ((nErr == 0) && (fp != NULL))
	{
		fclose(fp);
	}

	pCPInfo->SetDCState(TRUE);
	pCPInfo->SetDCDone(TRUE);

	IPC_CServiceMessage stMsg;
	m_comClient.SendRequest(WAFER_TABLE_STN, "CP_SaveData", stMsg);	//	SaveCP100ComData();

	return TRUE;
}

LONG CWaferPr::GridFindDieCalibration(LONG lPrDelay, INT nSubCut)
{
	typedef struct 
	{
		int X;
		int Y;
	} MOTORDATA;

	PR_UWORD		usDieType;
	PR_COORD		stDieShiftX, stDieShiftY; 
	PR_WIN			stDieSizeArea;
	PR_REAL			fDieRotate; 

	int				siSrchMargin = (32 * GetPrScaleFactor());
	int				siTempData1 = 0;
	int				siTempData2 = 0;
	float			fTemp = 0.0;

	PR_WORD			uwResult;
	int			nTempT, nTempX, nTempY, nLastX, nLastY;

	MOTORDATA	stDestX, stDestY;
	PR_COORD	stDiePrPos[2], stXPixelMove, stYPixelMove; 
	MOTORDATA	stTablePos[2], stXMotorMove, stYMotorMove;

	PR_WORD wPixelPitchX = (PR_WORD)(1.3 * GetNmlSizePixelX());
	PR_WORD wPixelPitchY = (PR_WORD)(1.3 * GetNmlSizePixelY());

	BOOL			bTempCorrection = m_bThetaCorrection;
	PR_WIN			stOrgSearchArea = GetSrchArea();

	PR_WORD wPrCtrX = (PR_WORD)GetPrCenterX();
	PR_WORD wPrCtrY = (PR_WORD)GetPrCenterY();
	PR_WORD	wULX = GetPRWinULX();
	PR_WORD wULY = GetPRWinULY();
	PR_WORD wLRX = GetPRWinLRX();
	PR_WORD wLRY = GetPRWinLRY();
	if (IsEnableZoom())
	{
		wULX = GetPRWinULX();
		wULY = GetPRWinULY();
		wLRX = GetPRWinLRX();
		wLRY = GetPRWinLRY();
	}
	if( nSubCut>100 )
	{
		wULX = wPrCtrX - (wPrCtrX-wULX)/nSubCut;
		wULY = wPrCtrY - (wPrCtrY-wULY)/nSubCut;
		wLRX = wPrCtrX + (wLRX-wPrCtrX)/nSubCut;
		wLRY = wPrCtrY + (wLRY-wPrCtrY)/nSubCut;
	}

	//Update Search area
	m_stSearchArea.coCorner1.x = wPrCtrX - wPixelPitchX;
	m_stSearchArea.coCorner1.y = wPrCtrY - wPixelPitchY;
	m_stSearchArea.coCorner2.x = wPrCtrX + wPixelPitchX;
	m_stSearchArea.coCorner2.y = wPrCtrY + wPixelPitchY;
	VerifyPRRegion(&m_stSearchArea);

	stDieSizeArea = m_stSearchArea;
	DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

	m_bThetaCorrection = FALSE;

	m_szGridDCLog.Empty();

	//Store Current position	
	GetWaferTableEncoder(&nLastX, &nLastY, &nTempT);

	stDestX.X = m_lPrCal1stStepSize;
	stDestX.Y = 0;     
	stDestY.Y = m_lPrCal1stStepSize;
	stDestY.X = 0;     
	LONG lReturn = 0;
	//Start learning
	for(short ssCycle = 0; ssCycle<=2; ssCycle++)
	{
		CString szMsg = "", szClbMsg = "", szTemp = "";
		//Search Die on current position
		m_stSearchArea = stDieSizeArea;

		Sleep(lPrDelay);
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePrPos[0], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE) )
		{
			lReturn = WPR_ERR_CALIB_NO_1ST_DIE;
			break;
		}

		if (ssCycle != 0)
		{
			siTempData1 = (int)(stDiePrPos[0].x - wPixelPitchX/2 - wULX - siSrchMargin);
			siTempData2 = (int)(wLRX - stDiePrPos[0].x - wPixelPitchX/2 - siSrchMargin);	

			if( ssCycle!=3 )	// divide by zero
				stDieShiftX.x = abs((min(siTempData1,siTempData2)) / (3-ssCycle));
			stDieShiftX.y = 0;
			ConvertPixelToMotorStep(stDieShiftX, &stDestX.X, &stDestX.Y, m_dGridCalibX, m_dGridCalibY, m_dGridCalibXY, m_dGridCalibYX);

			siTempData1 = (int)(stDiePrPos[0].y - wPixelPitchY/2 - wULY - siSrchMargin);
			siTempData2 = (int)(wLRY - stDiePrPos[0].y - wPixelPitchY/2 - siSrchMargin);

			stDieShiftY.x = 0;
			if( ssCycle!=3 )	// divide by zero
				stDieShiftY.y = abs((min(siTempData1,siTempData2)) / (3-ssCycle));
			ConvertPixelToMotorStep(stDieShiftY, &stDestY.X, &stDestY.Y, m_dGridCalibX, m_dGridCalibY, m_dGridCalibXY, m_dGridCalibYX);
		}

		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1,  stDieSizeArea.coCorner2,  PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.x = wPrCtrX - wPixelPitchX + stDieShiftX.x;
			m_stSearchArea.coCorner1.y = wPrCtrY - wPixelPitchY;
			m_stSearchArea.coCorner2.y = wPrCtrY + wPixelPitchY;
			m_stSearchArea.coCorner2.x = wPrCtrX + wPixelPitchX + stDieShiftX.x;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}

		//Get current motor pos to calculate next postion
		GetWaferTableEncoder(&nTempX, &nTempY, &nTempT);

		// Move table X to + direction
		MoveWaferTable(nTempX + stDestX.X, nTempY + stDestX.Y);
		Sleep(lPrDelay);
		GetWaferTableEncoder(&stTablePos[0].X, &stTablePos[0].Y, &nTempT);
		//Search PR		
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePrPos[0], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE) )
		{
			lReturn = WPR_ERR_CALIB_NO_LT_DIE;
			break;
		}

		// Move Table X - direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1,  stDieSizeArea.coCorner2,  PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.y = wPrCtrY - wPixelPitchY;
			m_stSearchArea.coCorner2.y = wPrCtrY + wPixelPitchY;
			m_stSearchArea.coCorner1.x = (-stDieShiftX.x) + wPrCtrX - wPixelPitchX;
			m_stSearchArea.coCorner2.x = (-stDieShiftX.x) + wPrCtrX + wPixelPitchX;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}

		//Move table X to - direction
		MoveWaferTable(nTempX - stDestX.X, nTempY - stDestX.Y);
		Sleep(lPrDelay);
		GetWaferTableEncoder(&stTablePos[1].X, &stTablePos[1].Y, &nTempT);
		//Search PR		
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePrPos[1], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE) )
		{
			lReturn = WPR_ERR_CALIB_NO_RT_DIE;
			break;
		}

		//Get Both X Value
		stXMotorMove.X	= stTablePos[1].X - stTablePos[0].X;
		stXMotorMove.Y	= stTablePos[1].Y - stTablePos[0].Y;
		stXPixelMove.x	= stDiePrPos[1].x - stDiePrPos[0].x;
		stXPixelMove.y	= stDiePrPos[1].y - stDiePrPos[0].y;

		szMsg = "";
		szTemp.Format("Posn %d,%d, move x %d,%d; y %d,%d", nTempX, nTempY, stDestX.X, stDestX.Y, stDestY.X, stDestY.Y);
		SetAlarmLog(szTemp);
		szTemp.Format("%d	WFT X: X %9d <- %9d, Y %9d <- %9d => Diff: %6d,%6d;	", ssCycle,
					stTablePos[1].X, stTablePos[0].X, stTablePos[1].Y, stTablePos[0].Y,
					stXMotorMove.X, stXMotorMove.Y);
		szClbMsg += szTemp;
		szMsg += szTemp;
		szTemp.Format("WPR X: X %4d <- %4d, Y %4d <- %4d => Diff: %5d,%5d;	",
					stDiePrPos[1].x, stDiePrPos[0].x, stDiePrPos[1].y, stDiePrPos[0].y,
					stXPixelMove.x, stXPixelMove.y);
		szClbMsg += szTemp + "\n";
		szMsg += szTemp;
		SetAlarmLog(szMsg);

		//Move table back to start poistion
		DrawSearchBox( PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1,  stDieSizeArea.coCorner2,  PR_COLOR_GREEN);

		//Move Table Y + direction search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1,  stDieSizeArea.coCorner2,  PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.y	= (stDieShiftY.y) + wPrCtrY - wPixelPitchY;
			m_stSearchArea.coCorner1.x	= wPrCtrX - wPixelPitchX;
			m_stSearchArea.coCorner2.x	= wPrCtrX + wPixelPitchX;
			m_stSearchArea.coCorner2.y	= (stDieShiftY.y) + wPrCtrY + wPixelPitchY;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}

		//Move table Y to + direction
		MoveWaferTable(nTempX + stDestY.X, nTempY + stDestY.Y);
		Sleep(lPrDelay);
		GetWaferTableEncoder(&stTablePos[0].X, &stTablePos[0].Y, &nTempT);
		//Search PR		
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePrPos[0], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE) )
		{
			lReturn = WPR_ERR_CALIB_NO_UP_DIE;
			break;
		}

		//Move Table Y - direction & search PR
		if (ssCycle != 0)
		{
			DrawRectangleBox(stDieSizeArea.coCorner1,  stDieSizeArea.coCorner2,  PR_COLOR_TRANSPARENT);
			DrawSearchBox( PR_COLOR_TRANSPARENT);

			//Update Search die area
			m_stSearchArea.coCorner1.x	= wPrCtrX - wPixelPitchX;
			m_stSearchArea.coCorner2.x	= wPrCtrX + wPixelPitchX;
			m_stSearchArea.coCorner1.y	= (-stDieShiftY.y) + wPrCtrY - wPixelPitchY;
			m_stSearchArea.coCorner2.y	= (-stDieShiftY.y) + wPrCtrY + wPixelPitchY;
			VerifyPRRegion(&m_stSearchArea);
			DrawSearchBox(PR_COLOR_GREEN);
		}

		//Move table Y to - direction
		MoveWaferTable(nTempX - stDestY.X, nTempY - stDestY.Y);
		Sleep(lPrDelay);
		GetWaferTableEncoder(&stTablePos[1].X, &stTablePos[1].Y, &nTempT);
		//Search PR		
		uwResult = ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePrPos[1], m_stSearchArea);
		if (uwResult == -1 || (DieIsAlignable(usDieType) == FALSE) )
		{
			lReturn = WPR_ERR_CALIB_NO_DN_DIE;
			break;
		}

		//Get Both Y Value
		stYMotorMove.X	= stTablePos[1].X - stTablePos[0].X;
		stYMotorMove.Y	= stTablePos[1].Y - stTablePos[0].Y;
		stYPixelMove.x	= stDiePrPos[1].x - stDiePrPos[0].x;
		stYPixelMove.y	= stDiePrPos[1].y - stDiePrPos[0].y;

		szMsg = "";
		szTemp.Format("%d	WFT Y: X %9d <- %9d, Y %9d <- %9d => Diff: %6d,%6d;	", ssCycle,
					stTablePos[1].X, stTablePos[0].X, stTablePos[1].Y, stTablePos[0].Y,
					stYMotorMove.X, stYMotorMove.Y);
		szClbMsg += szTemp;
		szMsg += szTemp;
		szTemp.Format("WPR Y: X %4d <- %4d, Y %4d <- %4d => Diff: %5d,%5d;	",
					stDiePrPos[1].x, stDiePrPos[0].x, stDiePrPos[1].y, stDiePrPos[0].y,
					stYPixelMove.x, stYPixelMove.y);
		szClbMsg += szTemp + "\n";
		szMsg += szTemp;
		SetAlarmLog(szMsg);

		// Check for zero error!
		fTemp = (float)(stXPixelMove.x * stYPixelMove.y - stYPixelMove.x * stXPixelMove.y);
		if (fabs(fTemp) < 0.000001 )
		{
			lReturn = WPR_ERR_CALIB_ZERO_VALUE;
			break;
		}

		m_dGridCalibX	= (DOUBLE)(stXMotorMove.X*stYPixelMove.y - stYMotorMove.X*stXPixelMove.y) / fTemp;
		if( stYPixelMove.y!=0 )
			m_dGridCalibXY = ((DOUBLE)stYMotorMove.X - m_dGridCalibX*(DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;
		else
			m_dGridCalibXY = 0.0;

		m_dGridCalibY	= (DOUBLE)(stYMotorMove.Y*stXPixelMove.x - stXMotorMove.Y*stYPixelMove.x) / fTemp;
		if( stXPixelMove.x!=0 )
			m_dGridCalibYX = ((DOUBLE)stXMotorMove.Y - m_dGridCalibY*(DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;
		else
			m_dGridCalibYX = 0.0;

		szTemp.Format("%d	CLB X(%3.4f,%3.4f) Y(%3.4f,%3.4f)  factor %f  ", ssCycle, 
			m_dGridCalibX, m_dGridCalibXY, m_dGridCalibY, m_dGridCalibYX, fTemp);
		szClbMsg += szTemp + "\n";
		SetAlarmLog(szTemp);
		m_szGridDCLog += szClbMsg;

		//Move table back to start poistion & do PR & update start position
		DrawSearchBox( PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1,  stDieSizeArea.coCorner2,  PR_COLOR_GREEN);

		MoveWaferTable(nLastX, nLastY);
		Sleep(lPrDelay);
		m_stSearchArea = stDieSizeArea;
		ManualSearchScanDie(PR_FALSE, &usDieType, &fDieRotate, &stDiePrPos[0], m_stSearchArea);
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDiePrPos[0], fDieRotate);
			Sleep(lPrDelay);
			GetWaferTableEncoder(&nLastX, &nLastY, &nTempT);
		}
	}

	m_stSearchArea = stOrgSearchArea;
	m_bThetaCorrection = bTempCorrection;

	return lReturn;
}
//	Chip Prober probing position offset PO

LONG CWaferPr::SearchDieInBigWindow(IPC_CServiceMessage& svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bStatus = TRUE;
	BOOL			bDieType = WPR_NORMAL_DIE;
	LONG			lRefDieNo = 1;
	int				siStepX = 0; 
	int				siStepY = 0; 
	BOOL			bBackupAlignTemp;

	PR_COORD	stSrchCorner1 = {GetPRWinULX(), GetPRWinULY()};
	PR_COORD	stSrchCorner2 = {GetPRWinLRX()-1, GetPRWinLRY()-1};

	stSrchCorner1.x = (PR_WORD)GetPrCenterX() - (PR_WORD)((12*1.0/4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	stSrchCorner1.y = (PR_WORD)GetPrCenterY() - (PR_WORD)((12*1.0/4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);
	stSrchCorner2.x = (PR_WORD)GetPrCenterX() + (PR_WORD)((12*1.0/4 + 0.5) * GetNmlSizePixelX() * WPR_SEARCHAREA_SHIFT_FACTOR);
	stSrchCorner2.y = (PR_WORD)GetPrCenterY() + (PR_WORD)((12*1.0/4 + 0.5) * GetNmlSizePixelY() * WPR_SEARCHAREA_SHIFT_FACTOR);

	typedef struct
	{
		BOOL		bShowPRStatus;
		BOOL		bNormalDie;
		LONG		lRefDieNo;
		BOOL		bDisableBackupAlign;
	} SRCH_TYPE;

	SRCH_TYPE	stSrchInfo;

	typedef struct {
		BOOL		bStatus;
		BOOL		bGoodDie;
		BOOL		bFullDie;
		LONG		lRefDieNo;
		LONG		lX;
		LONG		lY;
	} REF_TYPE;

	REF_TYPE	stInfo;


	if ( PR_NotInit() )
	{
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;
		svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
		return 1;
	}

	//Get Information from other station
	svMsg.GetMsg(sizeof(SRCH_TYPE), &stSrchInfo);

	//Do Search die on this position
	lRefDieNo = stSrchInfo.lRefDieNo;

	if ( stSrchInfo.bNormalDie == TRUE )
	{
		bDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bDieType = WPR_REFERENCE_DIE;
	}

	//v2.96T3
	PR_WORD wResult = 0;

	//Disable backup-align temporarily if currently enabled
	bBackupAlignTemp = m_bSrchEnableBackupAlign;
	if (stSrchInfo.bDisableBackupAlign == TRUE)
	{
		m_bSrchEnableBackupAlign = FALSE;
	}


	if ((bDieType == WPR_REFERENCE_DIE) && m_bAllRefDieCheck)	//v3.49T4
	{
		//PR_WIN stSrchArea;

		for (int i = 0; i < m_lLrnTotalRefDie; i++)
		{
			//GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE + WPR_GEN_RDIE_OFFSET + i);
			wResult = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE + i, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
			
			if (wResult != -1)
			{	
				if (!DieIsGood(usDieType))
				{
					continue;
				}	

				CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

				//Return result
				stInfo.bStatus	= TRUE;
				stInfo.bGoodDie	= DieIsGood(usDieType);
				stInfo.bFullDie	= DieIsAlignable(usDieType);
				stInfo.lX		= (LONG)(siStepX);
				stInfo.lY		= (LONG)(siStepY);

				m_bSrchEnableBackupAlign = bBackupAlignTemp;
				svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
				return 1;
			}
		}

		//Return NG result if all ref-die not matched
		stInfo.bStatus	= FALSE;
		stInfo.bGoodDie	= FALSE;
		stInfo.bFullDie	= FALSE;
		stInfo.lX		= 0;
		stInfo.lY		= 0;

		m_bSrchEnableBackupAlign = bBackupAlignTemp;
	}
	else
	{
#ifdef PR_INSP_MACHINE
		if (bDieType == WPR_REFERENCE_DIE)
			wResult = ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
		else
			wResult = IM_ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
#else
		wResult = ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);
#endif

		m_bSrchEnableBackupAlign = bBackupAlignTemp;

		if (wResult != -1)
		{
			if (DieIsAlignable(usDieType) == TRUE)
			{
				CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
			}

			//Return result
			stInfo.bStatus	= TRUE;
			stInfo.bGoodDie	= DieIsGood(usDieType);
			stInfo.bFullDie	= DieIsAlignable(usDieType);
			stInfo.lX		= (LONG)(siStepX);
			stInfo.lY		= (LONG)(siStepY);

			//Display PR status if necessary
			if ( stSrchInfo.bShowPRStatus == TRUE )
			{
				CString szDieResult;
				ConvertDieTypeToText(usDieType, szDieResult);
				m_szSrhDieResult = "Die Type: " + szDieResult;
			}
		}
		else
		{
			//Return result
			stInfo.bStatus	= FALSE;
			stInfo.bGoodDie	= FALSE;
			stInfo.bFullDie	= FALSE;
			stInfo.lX		= 0;
			stInfo.lY		= 0;
		}
	}
	
	svMsg.InitMessage(sizeof(REF_TYPE), &stInfo);
	return 1;
}

LONG CWaferPr::CP100CheckTableLinearity(IPC_CServiceMessage &svMsg)
{
	LONG lMoveStep = m_lLCMoveDistance;
	LONG lMoveDelay = m_lLCMoveDelay;
	UCHAR ucDir = 0;

	HmiMessage("begin table linearity check");
	svMsg.GetMsg(sizeof(UCHAR), &ucDir);

	if( lMoveStep<500 )
	{
		HmiMessage("move step is too small");
		return 1;
	}

	if( lMoveDelay<100 )
	{
		HmiMessage("move delay is too small");
		return 1;
	}

	if( lMoveDelay>10000 )
	{
		lMoveDelay = 2000;
	}

	INT lBaseX, lBaseY, lBaseT;
	GetWaferTableEncoder(&lBaseX, &lBaseY, &lBaseT);

	CString szMsg;

	DOUBLE dCalibX = GetCalibX(), dCalibY = GetCalibY(), dCalibXY = GetCalibXY(), dCalibYX = GetCalibYX();

	IPC_CServiceMessage stMsg;
	if( m_bLCUseBondCam )
	{
		SelectBondCamera(stMsg);
	//	CP_MoveTestStageUp(TRUE);
		dCalibX		= (*m_psmfSRam)["BondPr"]["Calibration"]["X"];
		dCalibY		= (*m_psmfSRam)["BondPr"]["Calibration"]["Y"];
		dCalibXY	= (*m_psmfSRam)["BondPr"]["Calibration"]["XY"];
		dCalibYX	= (*m_psmfSRam)["BondPr"]["Calibration"]["YX"];
		szMsg.Format("Bond X %f,%f Y %f,%f; pass in %d,%d,%d", dCalibX, dCalibXY, dCalibY, dCalibYX,
			ucDir, lMoveStep, lMoveDelay);
	}
	else
	{
		SelectWaferCamera(stMsg);
	//	CP_MoveTestStageUp(FALSE);
		szMsg.Format("Scan X %f,%f Y %f,%f; pass in %d,%d,%d", dCalibX, dCalibXY, dCalibY, dCalibYX,
			ucDir, lMoveStep, lMoveDelay);
	}

	SetAlarmLog(szMsg);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
    CMS896AApp::m_bStopAlign = FALSE;

	switch( ucDir )
	{
	case 0:	// UP
		szMsg = "_UP";
		break;
	case 1:	// LT
		szMsg = "_LT";
		break;
	case 2: // DN
		szMsg = "_DN";
		break;
	case 3:	// RT
	default:
		szMsg = "_RT";
		break;
	}
	CString szLogFile = gszUSER_DIRECTORY + "\\History\\TableLinearity_Scan_" + szMsg + ".txt";
	if( m_bLCUseBondCam )
	{
		szLogFile = gszUSER_DIRECTORY + "\\History\\TableLinearity_Bond_" + szMsg + ".txt";
	}
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFile, "w");
	INT lMoveX = lBaseX;
	INT lMoveY = lBaseY;
	INT lPrX, lPrY, lPrT;
	szMsg.Format("Cycle, CmdX, CmdY, CmdT, EncX, EncY, ResultX, ResultY");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%s\n", (LPCTSTR) szMsg);		//Klocwork	//v4.46
	}

	LONG lCounter = 0;
	LONG lPrOffX, lPrOffY;
	while( 1 )
	{
		lCounter++;
		Sleep(lMoveDelay);
		GetWaferTableEncoder(&lPrX, &lPrY, &lPrT);
		if( m_bLCUseBondCam )
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
			int nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDie", stMsg);
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
		}
		else
		{
			WprSearchDie(TRUE, 1, lPrOffX, lPrOffY);
		}
		szMsg.Format("%d, %d, %d, %d, %d, %d, %.2f, %.2f",
				lCounter, lMoveX, lMoveY, lBaseT, lPrX, lPrY, m_dPrDieCenterX, m_dPrDieCenterY);

		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "%s\n", (LPCTSTR) szMsg);	//Klocwork	//v4.46
		}
		switch( ucDir )
		{
		case 0:	// UP
			lMoveY = lMoveY + lMoveStep;
			break;
		case 1:	// LT
			lMoveX = lMoveX + lMoveStep;
			break;
		case 2: // DN
			lMoveY = lMoveY - lMoveStep;
			break;
		case 3:	// RT
		default:
			lMoveX = lMoveX - lMoveStep;
			break;
		}

		if( lCounter >= m_lLCMoveTotalLimit )
		{
			break;
		}

		if( IsWithinWT1WaferLimit(lMoveX, lMoveY, 1, m_bLCUseBondCam)==FALSE )
		{
			break;
		}

		if( MoveWftNoCheck(lMoveX, lMoveY, FALSE, SFM_WAIT, FALSE)==FALSE )
		{
			break;
		}

		if( pApp->IsStopAlign() )
		{
			break;
		}
	}

	if ((nErr == 0) && (fp != NULL))
	{
		fclose(fp);
	}

	HmiMessage("table linearity check complete");
//	MoveWftNoCheck(lBaseX, lBaseY, FALSE, SFM_WAIT, FALSE);

	return 1;
}

LONG CWaferPr::CPCheckOpticsDistortion(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

//V2.83T4 (With Syn/Without Syn Choice)
BOOL CWaferPr::MoveWftNoCheck(LONG lXAxis, LONG lYAxis, BOOL bToUnload, INT nMode, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;
 	RELPOS stPos;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lUnload;
	} RELPOS2;
 	RELPOS2 stPos2;

	stPos.lX	= lXAxis;
	stPos.lY	= lYAxis;
	stPos2.lX	= lXAxis;
	stPos2.lY	= lYAxis;

	if (bToUnload)				//v4.20
		stPos2.lUnload = 1;		//to UNLOAD
	else
		stPos2.lUnload = 0;		//to LOAD
	
	if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.16T3	//MS100 9Inch
	{
		stMsg.InitMessage(sizeof(RELPOS2), &stPos2);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_MoveToUnloadCmd",	stMsg);
	}
	else
	{
		stMsg.InitMessage(sizeof(RELPOS), &stPos);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY_MoveToCmd",			stMsg);
	}

	if (nMode == SFM_WAIT)
	{
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
		stMsg.GetMsg(sizeof(BOOL), &bResult);
	}

	return bResult;
}

LONG CWaferPr::DrawBlockProbeDie(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		LONG lRow;
		LONG lCol;
	}	GRABPR;
	GRABPR stData;

	svMsg.GetMsg(sizeof(GRABPR), &stData);

	PR_COORD stCorner1;
	PR_COORD stCorner2;
	LONG lDiePitchXX = GetScanPitchPixelXX();
	LONG lDiePitchYY = GetScanPitchPixelYY();

	LONG lDiff_X = stData.lCol;
	LONG lDiff_Y = stData.lRow;
	LONG lCtrX = GetPrCenterX() + lDiff_X * lDiePitchXX/2;
	LONG lCtrY = GetPrCenterY() + lDiff_Y * lDiePitchYY/2;

	stCorner1.x	= (PR_WORD)(lCtrX - GetScanNmlSizePixelX() / 2);
	stCorner1.y	= (PR_WORD)(lCtrY - GetScanNmlSizePixelY() / 2);
	stCorner2.x = (PR_WORD)(lCtrX + GetScanNmlSizePixelX() / 2);
	stCorner2.y = (PR_WORD)(lCtrY + GetScanNmlSizePixelY() / 2);

	DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_CYAN);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}
