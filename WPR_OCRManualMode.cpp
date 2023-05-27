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
#include "WL_Constant.h"

LONG CWaferPr::WprLearnBarPitch(CONST BOOL bInX)
{
	if (PR_NotInit())
	{
		return 0;
	}

	ChangeCamera(WPR_CAM_WAFER);

	CString	szText, szTitle;

	INT nStartX = 0, nStartY = 0, nSecondX = 0, nSecondY = 0, nThetaT = 0;

	szTitle.LoadString(HMB_WPR_MANUAL_DIE_PITCH);

	SetWaferTableJoystick(TRUE);
	szText.LoadString(HMB_WPR_MLRN_PITCH_STEP_1);
	if (HmiMessageEx(szText, szTitle, glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_CANCEL)
	{
		return FALSE;
	}
	SetWaferTableJoystick(FALSE);
	GetWaferTableEncoder(&nStartX, &nStartY, &nThetaT);

	if( bInX )
	{
		szText.LoadString(HMB_WPR_MLRN_PITCH_STEP_2);
	}
	else
	{
		szText.LoadString(HMB_WPR_MLRN_PITCH_STEP_3);
	}
	SetWaferTableJoystick(TRUE);
	if (HmiMessageEx(szText, szTitle, glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_CANCEL)
	{
		return FALSE;
	}
	SetWaferTableJoystick(FALSE);
	GetWaferTableEncoder(&nSecondX, &nSecondY, &nThetaT);

	//Move back to Oringinal Position
	MoveWaferTable(nStartX, nStartY);

	INT nBarPitchX = labs(nSecondX - nStartX);
	INT nBarPitchY = labs(nSecondY - nStartY);
	if( bInX )
	{
		if (nBarPitchX < GetDieSizeX())
		{
			SetAlert(IDS_WPR_NODIEONLEFTSIDE);
			return FALSE;
		}
		double dBarPitchXmm = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_X) * (DOUBLE)nBarPitchX;
		m_siBarPitchX = nBarPitchX;	// Bar Pitch X Enc Pos
	}
	else
	{
		if (nBarPitchY < GetDieSizeY())
		{
			SetAlert(IDS_WPR_NODIEONUPSIDE);
			return FALSE;
		}
		double dBarPitchYmm = GetChannelResolution(MS896A_CFG_CH_WAFTABLE_Y) * (DOUBLE)nBarPitchY;
		m_siBarPitchY = nBarPitchY;	// Bar Pitch Y Enc Pos
	}

	//Force to switch camera
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(TRUE);

	SavePrData(TRUE);

	return TRUE;
}

LONG CWaferPr::LearnBarPitch(IPC_CServiceMessage &svMsg)
{
	BOOL bLearnX = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bLearnX);

	WprLearnBarPitch(bLearnX);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SetOcrDieViewWindow(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	if (PR_NotInit())
	{
		HmiMessage("Vision system not initializaed.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bUseMouse)
	{
		HmiMessage("Please press the Confirm button firstly.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	LONG lOffsetX = 0, lOffsetY = 0;
	if( WprSearchDie(TRUE, 1, lOffsetX, lOffsetY)==FALSE )
	{
		HmiMessage("Please locate PR cursor to a normal die.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	INT nWftX = 0, nWftY = 0, nWftT = 0;
	GetWaferTableEncoder(&nWftX, &nWftY, &nWftT);
	nWftX += lOffsetX;
	nWftY += lOffsetY;
	MoveWaferTable(nWftX, nWftY);

	PR_COORD coCorner1, coCorner2;
	coCorner1 = m_stOcrViewWindow.coCorner1;
	coCorner2 = m_stOcrViewWindow.coCorner2;
	// Draw the defined region
	DrawRectangleBox(coCorner1, coCorner2, PR_COLOR_BLUE);

	if( coCorner1.x==0 || coCorner1.y==0 || coCorner2.x==0 ||coCorner2.y==0 )
	{
		coCorner1.x = (PR_WORD)GetPrCenterX() - WPR_DETECT_DIE_CORNER;
		coCorner1.y = (PR_WORD)GetPrCenterY() - WPR_DETECT_DIE_CORNER;
		coCorner2.x = (PR_WORD)GetPrCenterX() + WPR_DETECT_DIE_CORNER;
		coCorner2.y = (PR_WORD)GetPrCenterY() + WPR_DETECT_DIE_CORNER;
	}

	// Erase the original region
	DrawAndEraseCursor(coCorner1, PR_UPPER_LEFT, 0);
	DrawAndEraseCursor(coCorner2, PR_LOWER_RIGHT, 0);

	// Define by using mouse
	m_pPrGeneral->MouseDefineRegion(PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

	// Set the mouse size first
	m_pPrGeneral->MouseSet2PointRegion(coCorner1, coCorner2, PR_SHAPE_TYPE_RECTANGLE, PR_COLOR_YELLOW);

	m_bUseMouse = TRUE;

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::GetOcrDieViewWindow(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	if (m_bUseMouse == FALSE)
	{
		HmiMessage("Please press the Set button firstly.", "OCR die");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	// Get the mouse defined region
	LONG uwNumOfCorners = m_pPrGeneral->MouseGetRegion(m_stOcrViewWindow.coCorner1,
													   m_stOcrViewWindow.coCorner2);

	if (uwNumOfCorners == 2)
	{
		// Draw the defined region
		DrawRectangleBox(m_stOcrViewWindow.coCorner1, m_stOcrViewWindow.coCorner2, PR_COLOR_BLUE);
	}

	m_bUseMouse = FALSE;

	SavePrData(TRUE);

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
#define	SHOW_TIME	1000
#define	WT_DELAY	100
#define	PR_IMG_TIME	200

BOOL CWaferPr::BarSelectDieAction(UCHAR ucAction)
{
	ULONG ulRow = 0, ulCol = 0;
	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);

	if (ucAction!=0 && WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol)==FALSE)
	{
		HmiMessage("Select die has no grade");
		return FALSE;
	}

	if( ulRow!=m_lOcrBarCurrRow )
	{
		HmiMessage("Select die is not in current row");
		return FALSE;
	}

	LONG lCtrCol = BAR_MAP_CTR_COL-5;
	if( IsMS90Sorting2ndPart() )
	{
		lCtrCol = BAR_MAP_CTR_COL + 5;
	}
	if( m_bOcrBarRightSide )
	{
		if (ulCol < (ULONG)lCtrCol)
		{
			HmiMessage("Select die is not in right bar side");
			return FALSE;
		}
	}
	else
	{
		if (ulCol > (ULONG)lCtrCol)
		{
			HmiMessage("Select die is not in left bar side");
			return FALSE;
		}
	}

	LONG lColOffset = 0, lDummy = 0;
	if( GetScanKeyDie(ulRow, m_bOcrBarRightSide+1, lColOffset, lDummy)==FALSE )
	{
		HmiMessage("Select die is not in good row");
		return FALSE;
	}

	if( m_lOcrBarCurrMapCol	== ulCol )
	{
		HmiMessage("Select die is same to old");
		return FALSE;
	}

	LONG lBarFirstX = 0, lBarFirstY = 0;
	INT lX = 0, lY = 0, lT = 0;
	GetWaferTableEncoder(&lX, &lY, &lT);
	lBarFirstX = lX;
	lBarFirstY = lY;
	if( ucAction==0 )
	{
		LONG lScanRow = ulRow;
		LONG lScanCol = m_lOcrBarCurrScnCol + m_lOcrBarCurrMapCol - ulCol;
		ULONG lIndex = 0;
		DOUBLE dScore = 0;
		if( GetGrabFocusData(lScanRow, lScanCol, lBarFirstX, lBarFirstY, lIndex, dScore)==FALSE )
		{
			HmiMessage("Select die has no scanned position");
			return FALSE;
		}
		m_lOcrBarCurrScnCol	= lScanCol;
		m_WaferMapWrapper.SetSelectedPosition(ulRow, m_lOcrBarCurrMapCol);
		m_WaferMapWrapper.SetCurrentPosition(ulRow, m_lOcrBarCurrMapCol);
	}
	else
	{
		m_lOcrBarCurrMapCol	= ulCol;
		m_WaferMapWrapper.GetSpecialDieName(ulRow, ulCol, m_szOcrValueLFBar[0]);
		LONG lUserRow = 0, lUserCol = 0;
		ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
		m_szOcrValueLFBar[1].Format("R%ld_C%ld", lUserRow, lUserCol);
	}

	ULONG ulMapValidMinCol	= GetMapValidMinCol();
	ULONG ulMapValidMaxCol	= GetMapValidMaxCol();

	m_szOcrValueLLBar[0] = "";
	m_szOcrValueLLBar[1] = "";
	LONG lEncX = 0, lEncY = 0;
	ULONG ulLastCol = 0;
	LONG lNewOffset = m_lOcrBarCurrScnCol - m_lOcrBarCurrMapCol;
	BOOL bFindLast = FALSE;
	if( m_bOcrBarRightSide==FALSE )
	{
		for(ulLastCol=lCtrCol; ulLastCol>(ulMapValidMinCol+1); ulLastCol--)
		{
			ULONG ulPhyCol= ulLastCol + lNewOffset;
			ULONG lIndex = 0;
			DOUBLE dScore = 0;
			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulLastCol) && GetGrabFocusData(ulRow, ulPhyCol, lEncX, lEncY, lIndex, dScore))
			{
				bFindLast = TRUE;
				break;
			}
		}
	}
	else
	{
		for (ulLastCol = ulMapValidMaxCol; ulLastCol > (ULONG)lCtrCol; ulLastCol--)
		{
			ULONG ulPhyCol= ulLastCol+ lNewOffset;
			ULONG lIndex = 0;
			DOUBLE dScore = 0;
			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulLastCol) && GetGrabFocusData(ulRow, ulPhyCol, lEncX, lEncY, lIndex, dScore))
			{
				bFindLast = TRUE;
				break;
			}
		}
	}

	if( bFindLast )
	{
		m_szBarFirstDieImage	= BAR_OCR_NO_IMG_PATH;
		m_szBarLastDieImage		= BAR_OCR_NO_IMG_PATH;
	//	m_ulPrWindowHeight		= 480;		m_ulPrWindowWidth		= 512;
	//	HmiMessage("PR ing", "MS90", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, SHOW_TIME, glHMI_MSG_MODAL, NULL, 500, 300, NULL, NULL, NULL, NULL, 10 , 10);
		LONG lDeltaX = 0, lDeltaY = 0;
		CString szPrvImagePath;
		LONG lUserRow = 0, lUserCol = 0;
		ConvertAsmToOrgUser(ulRow, ulLastCol, lUserRow, lUserCol);
		m_szOcrValueLLBar[1].Format("R%ld_C%ld", lUserRow, lUserCol);
		m_WaferMapWrapper.GetSpecialDieName(ulRow, ulLastCol, m_szOcrValueLLBar[0]);

		MoveWaferTable(lEncX, lEncY);
		Sleep(WT_DELAY);
		WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
		szPrvImagePath.Format("%s\\Die_%s_%s", BAR_OCR_IMG_PATH, m_szOcrValueLLBar[1], m_szOcrValueLLBar[0]);
		for(INT mmm=0; mmm<100; mmm++)
		{
			CString szTemp;
			szTemp.Format("%s_%03d.JPG", szPrvImagePath, mmm);
			if (_access(szTemp, 0) == -1)
			{
				szPrvImagePath = szTemp;
				break;
			}
		}
		SavePrImage(szPrvImagePath);
		Sleep(PR_IMG_TIME);
		m_szBarLastDieImage = szPrvImagePath;

        MoveWaferTable(lBarFirstX, lBarFirstY);
		Sleep(WT_DELAY);
		WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
		szPrvImagePath.Format("%s\\Die_%s_%s.JPG", BAR_OCR_IMG_PATH, m_szOcrValueLFBar[1], m_szOcrValueLFBar[0]);
		for(INT mmm=0; mmm<100; mmm++)
		{
			CString szTemp;
			szTemp.Format("%s_%03d.JPG", szPrvImagePath, mmm);
			if (_access(szTemp, 0) == -1)
			{
				szPrvImagePath = szTemp;
				break;
			}
		}
		SavePrImage(szPrvImagePath);
		Sleep(PR_IMG_TIME);
		m_szBarFirstDieImage	= szPrvImagePath;
	//	m_ulPrWindowHeight		= 0;		m_ulPrWindowWidth		= 0;
	}

	return TRUE;
}


LONG CWaferPr::BarScanGoDie(IPC_CServiceMessage &svMsg)
{
	BarSelectDieAction(0);	// WFT go

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferPr::ChangeMapOcrValue(IPC_CServiceMessage &svMsg)
{
	BarSelectDieAction(1);	// MAP go

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ConfirmBarResult(IPC_CServiceMessage &svMsg)
{
	CString szMsg;
	szMsg.Format("Bar %02d map row %lu OK=%d", m_lOcrBarIndex, m_lOcrBarCurrRow, m_bOcrResultBarL[0]);
	SetAlarmLog(szMsg);

	LONG lColOffset = m_lOcrBarCurrScnCol - m_lOcrBarCurrMapCol;
	SetScanKeyDie(m_lOcrBarCurrRow, m_bOcrBarRightSide+1, lColOffset, m_bOcrResultBarL[0]);

	m_lOcrBarCurrRow++;
	if( NextBarIndex()==FALSE )	// confirm
	{
		m_szBarFirstDieImage	= BAR_OCR_NO_IMG_PATH;
		m_szBarLastDieImage		= BAR_OCR_NO_IMG_PATH;
		m_bOcrConfirmButton = TRUE;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::SkipBarResult(IPC_CServiceMessage &svMsg)
{
	CString szMsg;
	szMsg.Format("Skip Bar %02d map row %lu OK=%d", m_lOcrBarIndex, m_lOcrBarCurrRow, m_bOcrResultBarL[0]);
	SetAlarmLog(szMsg);

	m_lOcrBarCurrRow++;
	if( NextBarIndex()==FALSE )	//	skip
	{
		m_szBarFirstDieImage	= BAR_OCR_NO_IMG_PATH;
		m_szBarLastDieImage		= BAR_OCR_NO_IMG_PATH;
		m_bOcrConfirmButton = TRUE;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ConfirmAllDone(IPC_CServiceMessage &svMsg)
{
	m_ucOcrBarMapStage		= 2;
	m_lOcrImageRotation		= 0;
	m_bOcrConfirmControl	= FALSE;
	m_ulPrWindowHeight		= 480;	m_ulPrWindowWidth		= 512;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


ULONG CWaferPr::GetPrescanMapMaxRow(const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow)
{
	LONG lEncX = 0, lEncY = 0;
	ULONG ulScnMaxRow = ulMapValidMinRow;
	for(LONG ulRow = (LONG)ulMapValidMinRow; ulRow <= (LONG)ulMapValidMaxRow; ulRow++)
	{
		for(ULONG ulCol = 0; ulCol <= BAR_PHY_END_COL; ulCol++)
		{
			ULONG lIndex = 0;
			DOUBLE dScore = 0;
			if( GetGrabFocusData(ulRow, ulCol, lEncX, lEncY, lIndex, dScore)==FALSE )
			{
				continue;
			}

			if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
			{
				continue;
			}

			if (ulRow > (LONG)ulScnMaxRow )
			{
				ulScnMaxRow = ulRow;
			}
		}
	}
	ulScnMaxRow = ulScnMaxRow + 1;

	return ulScnMaxRow;
}

ULONG CWaferPr::GetPrescanMapMaxCol(const ULONG ulMapValidMinCol, const ULONG ulMapValidMaxCol)
{
	LONG lEncX = 0, lEncY = 0;
	ULONG ulScnMaxCol = ulMapValidMinCol;
	for (LONG ulCol = (LONG)ulMapValidMinCol; ulCol <= (LONG)ulMapValidMaxCol; ulCol++)
	{
		for(ULONG ulRow = 0; ulRow <= BAR_PHY_END_ROW; ulRow++)
		{
			ULONG lIndex = 0;
			DOUBLE dScore = 0;
			if( GetGrabFocusData(ulRow, ulCol, lEncX, lEncY, lIndex, dScore)==FALSE )
			{
				continue;
			}

			if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
			{
				continue;
			}

			if (ulCol > (LONG)ulScnMaxCol)
			{
				ulScnMaxCol = ulCol;
			}
		}
	}
	ulScnMaxCol = ulScnMaxCol + 1;

	return ulScnMaxCol;
}


VOID CWaferPr::ConstructPrescanMap_FinisarBW()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	CString szConLogFile = szLogPath + PRESCAN_PASSIN_PSN;
	FILE *fpPassIn = NULL;
	errno_t nPassInErr = fopen_s(&fpPassIn, szConLogFile, "w");
	szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpBarOut = NULL;
	errno_t nBarOutErr = fopen_s(&fpBarOut, szConLogFile, "w");

	ULONG ulDiePitchX = labs(GetDiePitchX_X());
	ULONG ulDiePitchY = labs(GetDiePitchY_Y());
	LONG  lStreetY = GetBarPitchY();
	LONG  lStreetX = GetBarPitchX();
	if( m_nDieSizeX!=0 )
	{
		ulDiePitchY = labs(ulDiePitchX*m_nDieSizeY/m_nDieSizeX);
	}

	ULONG ulMapValidMinRow = 0, ulMapValidMaxRow = 0;
	ULONG ulMapValidMinCol = 0, ulMapValidMaxCol = 0;
	GetMapValidSize(ulMapValidMaxRow, ulMapValidMinRow,	ulMapValidMaxCol, ulMapValidMinCol);

	// add all alignment point and refer points to list
	LONG lHomeRow, lHomeCol, lHomeWfX, lHomeWfY;
	pUtl->GetAlignPosition(lHomeRow, lHomeCol, lHomeWfX, lHomeWfY);

	MoveWaferTable(lHomeWfX, lHomeWfY);
	if( fpPassIn!=NULL )
	{
		fprintf(fpPassIn, "map range, %ld,%ld,%ld,%ld\n", ulMapValidMinRow, ulMapValidMinCol,
													ulMapValidMaxRow, ulMapValidMaxCol);
		fprintf(fpPassIn, "street,%ld,%ld\n", lStreetX, lStreetY);
	}

	LONG lCtrCol = BAR_MAP_CTR_COL-5;
	if( IsMS90Sorting2ndPart() )
	{
		lCtrCol = BAR_MAP_CTR_COL + 5;
	}
	BOOL bRightMap = FALSE;
	ULONG ulIndex = 0;
	LONG lAlgnRow = 0, lAlgnCol = 0, lAlgnWfX = 0, lAlgnWfY = 0;
	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			if( lAlgnRow==lHomeRow && lAlgnCol==lHomeCol )
			{
				continue;
			}
			if( lAlgnCol<lCtrCol )
			{
				continue;
			}
			bRightMap = TRUE;
			break;
		}
	}
	LONG lCenterX = lHomeWfX - (ulMapValidMaxCol + 5 - lHomeCol)*ulDiePitchX - lStreetX;
	LONG lTopY = lHomeWfY;
	LONG lBtmY = lHomeWfY;
	if( bRightMap )
	{
		lCenterX = lAlgnWfX + 5*ulDiePitchX;
		lTopY = max(lHomeWfY, lAlgnWfY);
		lBtmY = min(lHomeWfY, lAlgnWfY);
	}
	lTopY = _round(lTopY + (lHomeRow + 1 - ulMapValidMinRow) * lStreetY * (100 + m_dPrescanPitchTolY) / 100);
	lBtmY = _round(lBtmY - (ulMapValidMaxRow - ulMapValidMinRow) * lStreetY * (100 + m_dPrescanPitchTolY) / 100);

	WSSetPrescanPosition(lHomeWfX, lHomeWfY, 0.0, "0", 1);
	if( bRightMap )
		WSSetPrescanPosition(lAlgnWfX, lAlgnWfY, 0.0, "0", 1);
	LONG lEncX = 0, lEncY = 0;
	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);
	ulDieSizeYTol		= (ULONG) (lStreetY * m_dPrescanPitchTolY/100.0);
	CString szMsg;
	// build physical map
	int nMaxIterateCount = -1, nMaxAllowLeft = 10, nMaxSpan = 2;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE, bDieOverLap = FALSE;

	m_pbGetOut = new bool[WSGetPrescanTotalDie()+10];
	memset(m_pbGetOut, 0, sizeof(m_pbGetOut));
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		m_pbGetOut[ulIndex] = false;
	}

	ClearPrescanInfo();
	ResetMapPhyPosn();

	if( fpPassIn!=NULL )
	{
		fprintf(fpPassIn, "Top %ld, Btm %ld, Ctr %ld, right side = %d\n", lTopY, lBtmY, lCenterX, bRightMap);
		fprintf(fpPassIn, "presort %d,%d,%d,%d\n", bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);
		fprintf(fpPassIn, "build map %lu,%lu,%lu,%lu\n", ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, lStreetY);
		fprintf(fpPassIn, "die size %ld,%ld, ideal pitch %ld,%ld\n", 
			GetDieSizeX(), GetDieSizeY(), GetDiePitchX_X(), GetDiePitchY_Y());
	}

	WAF_CPhysicalInformationMap cSglMapL;
	CDWordArray dwaPM_WfXL, dwaPM_WfYL, dwaPM_RowL, dwaPM_ColL;
	DelGrabFocusData();

	dwaPM_WfXL.RemoveAll();		dwaPM_WfYL.RemoveAll();
	dwaPM_RowL.RemoveAll();		dwaPM_ColL.RemoveAll();
	dwaPM_WfXL.Add(lHomeWfX);	dwaPM_WfYL.Add(lHomeWfY);
	dwaPM_RowL.Add(lHomeRow);	dwaPM_ColL.Add(lHomeCol + BAR_SCAN_MAP_OFFSET_L);
	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		LONG lAsstWfX = 0, lAsstWfY = 0, lAsstRow = 0, lAsstCol = 0;
		if( pUtl->GetAssistPosition(ulIndex, lAsstRow, lAsstCol, lAsstWfX, lAsstWfY) )
		{
			if( lAsstCol<lCtrCol )
			{
				dwaPM_WfXL.Add(lAsstWfX);	dwaPM_WfYL.Add(lAsstWfY);
				dwaPM_RowL.Add(lAsstRow);	dwaPM_ColL.Add(lAsstCol + BAR_SCAN_MAP_OFFSET_L);
			}
		}
	}

	if( fpPassIn!=NULL )
	{
		fprintf(fpPassIn, "Left Home, %ld,%ld,%ld,%ld\n", lHomeWfX, lHomeWfY, lHomeRow, lHomeCol);
		for(INT i=0; i<dwaPM_WfXL.GetSize(); i++)
		{
			fprintf(fpPassIn, "%ld,%ld,%ld,%ld\n", 
				dwaPM_WfXL.GetAt(i), dwaPM_WfYL.GetAt(i), dwaPM_RowL.GetAt(i), dwaPM_ColL.GetAt(i));
		}
	}

	cSglMapL.RemoveAll();
	cSglMapL.KeepIsolatedDice(FALSE);
	cSglMapL.SetScan2Pr(FALSE);
	cSglMapL.SetXOffset(0);

	ULONG ulPyiRowMax = 0, ulPyiColMax = 0;
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
		{
			continue;
		}

		if( bRightMap && lEncX<lCenterX )
		{
			continue;
		}

		cSglMapL.SortAdd_Tail(lEncX, lEncY, ulIndex, bDieOverLap);
		if( fpPassIn!=NULL )
		{
			fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
		}
	}

	cSglMapL.ConstructMap(dwaPM_WfXL, dwaPM_WfYL, dwaPM_RowL, dwaPM_ColL,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, lStreetY,
		bPreSorted, nMaxIterateCount, 10, nMaxSpan);

	cSglMapL.GetDimension(ulPyiRowMax, ulPyiColMax);
	if( fpBarOut!=NULL )
	{
		fprintf(fpBarOut, "Left physical dim %ld,%ld, map %d,%d\n", ulPyiRowMax, ulPyiColMax, ulMapValidMaxRow, ulMapValidMaxCol);
	}
	for(ULONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<BAR_PHY_CTR_COL; ulCol++)
		{
			LONG lIndex = 0;
			if( cSglMapL.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap)==FALSE )
			{
				continue;
			}
			if( lIndex>=0 )
			{
				m_pbGetOut[lIndex] = 1;
				SetGrabFocusData(ulRow, ulCol, lEncX, lEncY, lIndex, 0.0);
				if( fpBarOut!=NULL )
				{
					fprintf(fpBarOut, "%ld,%ld,%ld,%ld\n", ulRow, ulCol, lEncX, lEncY);
				}
				if( ulRow==ulMapValidMinRow )
				{
					lTopY = max(lTopY, lEncY);
				}
				if( ulRow==ulMapValidMaxRow )
				{
					lBtmY = min(lBtmY, lEncY);
				}
			}
		}
	}
	cSglMapL.RemoveAll();
	dwaPM_WfXL.RemoveAll();		dwaPM_WfYL.RemoveAll();
	dwaPM_RowL.RemoveAll();		dwaPM_ColL.RemoveAll();

if( bRightMap )
{
	WAF_CPhysicalInformationMap cSglMapR;
	CDWordArray dwaPM_WfXR, dwaPM_WfYR, dwaPM_RowR, dwaPM_ColR;
	dwaPM_WfXR.RemoveAll();		dwaPM_WfYR.RemoveAll();
	dwaPM_RowR.RemoveAll();		dwaPM_ColR.RemoveAll();
	dwaPM_WfXR.Add(lAlgnWfX);	dwaPM_WfYR.Add(lAlgnWfY);
	dwaPM_RowR.Add(lAlgnRow);	dwaPM_ColR.Add(lAlgnCol + BAR_SCAN_MAP_OFFSET_R);
	cSglMapR.RemoveAll();
	cSglMapR.KeepIsolatedDice(FALSE);
	cSglMapR.SetScan2Pr(FALSE);
	cSglMapR.SetXOffset(0);

	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		LONG lAsstWfX = 0, lAsstWfY = 0, lAsstRow = 0, lAsstCol = 0;
		if( pUtl->GetAssistPosition(ulIndex, lAsstRow, lAsstCol, lAsstWfX, lAsstWfY) )
		{
			if( lAsstCol>lCtrCol )
			{
				dwaPM_WfXR.Add(lAsstWfX);	dwaPM_WfYR.Add(lAsstWfY);
				dwaPM_RowR.Add(lAsstRow);	dwaPM_ColR.Add(lAsstCol + BAR_SCAN_MAP_OFFSET_R);
			}
		}
	}

	if( fpPassIn!=NULL )
	{
		fprintf(fpPassIn, "Right Align, %ld,%ld,%ld,%ld\n", lAlgnWfX, lAlgnWfY, lAlgnRow, lAlgnCol);
		for(INT i=0; i<dwaPM_WfXR.GetSize(); i++)
		{
			fprintf(fpPassIn, "%ld,%ld,%ld,%ld\n", 
				dwaPM_WfXR.GetAt(i), dwaPM_WfYR.GetAt(i), dwaPM_RowR.GetAt(i), dwaPM_ColR.GetAt(i));
		}
	}

	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
		{
			continue;
		}

		if( lEncX>lCenterX )
		{
			continue;
		}

		cSglMapR.SortAdd_Tail(lEncX, lEncY, ulIndex, bDieOverLap);
		if( fpPassIn!=NULL )
		{
			fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
		}
	}
	cSglMapR.ConstructMap(dwaPM_WfXR, dwaPM_WfYR, dwaPM_RowR, dwaPM_ColR,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, lStreetY,
		bPreSorted, nMaxIterateCount, 10, nMaxSpan);

	cSglMapR.GetDimension(ulPyiRowMax, ulPyiColMax);
	if( fpBarOut!=NULL )
	{
		fprintf(fpBarOut, "right physical dim %ld,%ld\n", ulPyiRowMax, ulPyiColMax);
	}
	for(ULONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
	{
		for(ULONG ulCol=BAR_PHY_CTR_COL; ulCol<BAR_PHY_END_COL; ulCol++)
		{
			LONG lIndex = 0;
			if( cSglMapR.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap)==FALSE )
			{
				continue;
			}
			if( lIndex>=0 )
			{
				m_pbGetOut[lIndex] = 1;
				SetGrabFocusData(ulRow, ulCol, lEncX, lEncY, lIndex, 0.0);
				if( fpBarOut!=NULL )
				{
					fprintf(fpBarOut, "%ld,%ld,%ld,%ld\n", ulRow, ulCol, lEncX, lEncY);
				}
				if( ulRow==ulMapValidMinRow )
				{
					lTopY = max(lTopY, lEncY);
				}
				if( ulRow==ulMapValidMaxRow )
				{
					lBtmY = min(lBtmY, lEncY);
				}
			}
		}
	}
	cSglMapR.RemoveAll();
	dwaPM_WfXR.RemoveAll();		dwaPM_WfYR.RemoveAll();
	dwaPM_RowR.RemoveAll();		dwaPM_ColR.RemoveAll();
}

	if( fpPassIn!=NULL )
		fclose(fpPassIn);

	if( fpBarOut!=NULL )
	{
		fprintf(fpBarOut, "\n\n");
	}

	if( IsScanThenDivideMap() && IsMS90HalfSortMode() && IsMS90Sorting2ndPart()==FALSE )
	{
/*
		ULONG ulScnMaxRow = ulMapValidMinRow;
		for(LONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
		{
			for(ULONG ulCol=0; ulCol<=BAR_PHY_END_COL; ulCol++)
			{
				ULONG lIndex = 0;
				DOUBLE dScore = 0;
				if( GetGrabFocusData(ulRow, ulCol, lEncX, lEncY, lIndex, dScore)==FALSE )
				{
					continue;
				}

				if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
				{
					continue;
				}

				if( ulRow>ulScnMaxRow )
				{
					ulScnMaxRow = ulRow;
				}
			}
		}
		ulScnMaxRow = ulScnMaxRow + 1;
*/
		ULONG ulAsmHalfRow = 0;
		ULONG ulAsmHalfCol = 0;
		BOOL bRowModeSeparateHalfMap = IsRowModeSeparateHalfMap();

		if (bRowModeSeparateHalfMap)
		{
			ULONG ulScnMaxRow = GetPrescanMapMaxRow(ulMapValidMinRow, ulMapValidMaxRow);

			//		MS90 scan align, to get new scan area physical range and set this as the half sort area.
			//		for first part only. should inner several rows by looping check and check within wafer tablelimit.
			ulAsmHalfRow = ulScnMaxRow;
			ulAsmHalfCol = ulMapValidMaxCol/2;
		}
		else
		{
			ULONG ulScnMaxCol = GetPrescanMapMaxCol(ulMapValidMinCol, ulMapValidMaxCol);

			//		MS50 scan align, to get new scan area physical range and set this as the half sort area.
			//		for first part only. should inner several rows by looping check and check within wafer tablelimit.
			ulAsmHalfRow = ulMapValidMaxRow / 2;
			ulAsmHalfCol = ulScnMaxCol;
		}

		SetNextHalfMapAsMissingDie(bRowModeSeparateHalfMap, ulAsmHalfRow, ulAsmHalfCol);

		if( IsMS90HalfSortMode() )
		{
			if (IsRowModeSeparateHalfMap())
			{
				ulMapValidMaxRow	= GetMS90HalfMapMaxRow();
			}
			else
			{
				ulMapValidMinCol	= GetMS90HalfMapMaxCol();
			}
		}
	}

	lTopY = lTopY + lStreetY/2;
	lBtmY = lBtmY - lStreetY/2;
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
		{
			continue;
		}
		if( m_pbGetOut[ulIndex] )
		{
			continue;
		}

		if( lEncY>lTopY || lEncY<lBtmY )
		{
			m_pbGetOut[ulIndex] = 1;
		}
	}

	if( fpBarOut!=NULL )
	{
		fprintf(fpBarOut, "bar check\n");
		fclose(fpBarOut);
	}

	m_lOcrImageRotation	= 0;
	if( IsMS90Sorting2ndPart() )
		m_lOcrImageRotation = 180;
	m_ucOcrBarMapStage = 0;
	m_bOcrConfirmControl = FALSE;
	m_bOcrConfirmButton	 = FALSE;
	DelScanKeyDie();
	m_bOcrBarRightSide = FALSE;
	m_lOcrBarIndex = 0;
	m_lOcrBarCurrRow = ulMapValidMinRow;
	AutoBondScreen(FALSE);
	if( m_bCurrentCamera!=WPR_CAM_WAFER )
		ChangeCamera(WPR_CAM_WAFER, FALSE);
	CreateDirectory(BAR_OCR_IMG_PATH, NULL);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(BAR_OCR_IMG_PATH, 0, TRUE);
	CreateDirectory(BAR_OCR_IMG_PATH, NULL);
	m_szBarFirstDieImage	= BAR_OCR_NO_IMG_PATH;
	m_szBarLastDieImage		= BAR_OCR_NO_IMG_PATH;
	if( NextBarIndex() )	//	construct map and first move
	{
		m_ulPrWindowHeight		= 0;			m_ulPrWindowWidth		= 0;
		m_bOcrConfirmControl = TRUE;
		m_ucOcrBarMapStage = 1;
		HmiMessage_Red_Back("Begin check the bar result in this scan");
	}
	else
	{
		m_ucOcrBarMapStage = 2;
	}
}

BOOL CWaferPr::NextBarIndex()
{
//	m_szBarFirstDieImage	= BAR_OCR_NO_IMG_PATH;
//	m_szBarLastDieImage		= BAR_OCR_NO_IMG_PATH;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	CString szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpBarOut = NULL;
	errno_t nErr = fopen_s(&fpBarOut, szConLogFile, "a");

	ULONG ulMapValidMinRow = 0, ulMapValidMaxRow = 0;
	ULONG ulMapValidMinCol = 0, ulMapValidMaxCol = 0;
	GetMapValidSize(ulMapValidMaxRow, ulMapValidMinRow,	ulMapValidMaxCol, ulMapValidMinCol);

	m_szOcrValueLFBar[0] = "";
	m_szOcrValueLFBar[1] = "";
	m_szOcrValueLLBar[0] = "";
	m_szOcrValueLLBar[1] = "";
	LONG lEncX = 0, lEncY = 0;
	BOOL bFindOneBar = FALSE;
	BOOL bLeftSideDone = TRUE;
	LONG lCtrCol = BAR_MAP_CTR_COL-5;
	if (IsMS90Sorting2ndPart())
	{
		lCtrCol = BAR_MAP_CTR_COL + 5;
	}

	if ((nErr == 0) && (fpBarOut != NULL))
	{
		fprintf(fpBarOut, "center col is %ld\n", lCtrCol);
	}

	for (LONG ulRow = m_lOcrBarCurrRow; ulRow <= (LONG)ulMapValidMaxRow; ulRow++)
	{
		if( m_bOcrBarRightSide )
		{
			break;
		}
		LONG lColOffset = 0;
		LONG lBarFirstX = 0, lBarFirstY = 0;
		CString szFirst = "", szLast = "";

		for (ULONG ulCol = ulMapValidMinCol; ulCol < (ULONG)lCtrCol; ulCol++)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}
			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol))
			{
				for(ULONG ulPhyCol=0; ulPhyCol<BAR_PHY_CTR_COL; ulPhyCol++)
				{
					ULONG lIndex = 0;
					DOUBLE dScore = 0;
					if( GetGrabFocusData(ulRow, ulPhyCol, lEncX, lEncY, lIndex, dScore) )
					{
						m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
						m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
						LONG lUserRow = 0, lUserCol = 0;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
						m_szOcrValueLFBar[1].Format("R%ld_C%ld", lUserRow, lUserCol);
						m_WaferMapWrapper.GetSpecialDieName(ulRow, ulCol, m_szOcrValueLFBar[0]);
						lBarFirstX = lEncX;
						lBarFirstY = lEncY;
						m_lOcrBarCurrScnCol = ulPhyCol;
						m_lOcrBarCurrMapCol = ulCol;
						lColOffset = m_lOcrBarCurrScnCol - m_lOcrBarCurrMapCol;
						SetScanKeyDie(ulRow, 1, lColOffset, 0);
						bFindOneBar = TRUE;
						szFirst.Format(" first map %s OCR %s offset %03ld at(row %03ld col %03ld<->%03ld) wft %ld,%ld",
							m_szOcrValueLFBar[1], m_szOcrValueLFBar[0], lColOffset, ulRow, ulCol, ulPhyCol, lEncX, lEncY);
						break;
					}
				}
				if( bFindOneBar )
				{
					break;
				}
			}
		}

		if( bFindOneBar )
		{
		//	m_ulPrWindowHeight		= 480;			m_ulPrWindowWidth		= 512;
		//	HmiMessage("PR ing", "MS90", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, SHOW_TIME, glHMI_MSG_MODAL, NULL, 500, 300, NULL, NULL, NULL, NULL, 10 , 10);
			CString szPrvImagePath;
			for(LONG ulCol = lCtrCol; ulCol > (LONG)(ulMapValidMinCol+1); ulCol--)
			{
				ULONG ulPhyCol= ulCol+ lColOffset;
				ULONG lIndex = 0;
				DOUBLE dScore = 0;
				if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol) && GetGrabFocusData(ulRow, ulPhyCol, lEncX, lEncY, lIndex, dScore))
				{
					LONG lUserRow = 0, lUserCol = 0;
					ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
					m_szOcrValueLLBar[1].Format("R%ld_C%ld", lUserRow, lUserCol);
					m_WaferMapWrapper.GetSpecialDieName(ulRow, ulCol, m_szOcrValueLLBar[0]);
					MoveWaferTable(lEncX, lEncY);
					Sleep(WT_DELAY);
					if( m_lOcrBarIndex==0 )
						Sleep(1000);
					LONG lDeltaX = 0, lDeltaY = 0;
					WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
					szPrvImagePath.Format("%s\\Die_%s_%s.JPG", BAR_OCR_IMG_PATH, m_szOcrValueLLBar[1], m_szOcrValueLLBar[0]);
					for(INT mmm=0; mmm<100; mmm++)
					{
						CString szTemp;
						szTemp.Format("%s_%03d.JPG", szPrvImagePath, mmm);
						if (_access(szTemp, 0) == -1)
						{
							szPrvImagePath = szTemp;
							break;
						}
					}
					SavePrImage(szPrvImagePath);
					Sleep(PR_IMG_TIME);
					m_szBarLastDieImage = szPrvImagePath;
					szLast.Format(" last map %s OCR %s offset %03ld at(row %03ld col %03ld<->%03ld) wft %ld,%ld",
						m_szOcrValueLLBar[1], m_szOcrValueLLBar[0], lColOffset, ulRow, ulCol, ulPhyCol, lEncX, lEncY);
					break;
				}
			}
			MoveWaferTable(lBarFirstX, lBarFirstY);
			Sleep(WT_DELAY);
			LONG lDeltaX = 0, lDeltaY = 0;
			WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
			szPrvImagePath.Format("%s\\Die_%s_%s.JPG", BAR_OCR_IMG_PATH, m_szOcrValueLFBar[1], m_szOcrValueLFBar[0]);
			for(INT mmm=0; mmm<100; mmm++)
			{
				CString szTemp;
				szTemp.Format("%s_%03d.JPG", szPrvImagePath, mmm);
				if (_access(szTemp, 0) == -1)
				{
					szPrvImagePath = szTemp;
					break;
				}
			}
			SavePrImage(szPrvImagePath);
			Sleep(PR_IMG_TIME);
			m_szBarFirstDieImage = szPrvImagePath;
			bLeftSideDone = FALSE;
			m_lOcrBarCurrRow = ulRow;
			m_lOcrBarIndex++;
			m_szOcrBarName[0].Format("Left  Bar %02d", m_lOcrBarIndex);
			if( fpBarOut!=NULL )
			{
				fprintf(fpBarOut, "%s %s\n", m_szOcrBarName[0], szFirst);
				fprintf(fpBarOut, "%s %s\n", m_szOcrBarName[0], szLast);
			}
		//	m_ulPrWindowHeight		= 0;			m_ulPrWindowWidth		= 0;
			break;
		}
	}

	//	set this offset.
	if( bLeftSideDone && m_bOcrBarRightSide==FALSE )
	{
		m_lOcrBarIndex = 0;
		m_bOcrBarRightSide = TRUE;
		m_lOcrBarCurrRow = ulMapValidMinRow;
		if( fpBarOut!=NULL )
		{
			fprintf(fpBarOut, "\n\n");
		}
	}

	for(LONG ulRow = m_lOcrBarCurrRow; ulRow <= (LONG)ulMapValidMaxRow; ulRow++)
	{
		if( m_bOcrBarRightSide==FALSE )
		{
			break;
		}
		ULONG lColOffset = 0;
		LONG lBarFirstX = 0, lBarFirstY = 0;
		CString szFirst = "", szLast = "";
		for(ULONG ulCol=lCtrCol; ulCol<=ulMapValidMaxCol; ulCol++)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}
			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol))
			{
				for(ULONG ulPhyCol=BAR_PHY_CTR_COL; ulPhyCol<BAR_PHY_END_COL; ulPhyCol++)
				{
					ULONG lIndex = 0;
					DOUBLE dScore = 0;
					if( GetGrabFocusData(ulRow, ulPhyCol, lEncX, lEncY, lIndex, dScore) )
					{
						m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
						m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
						LONG lUserRow = 0, lUserCol = 0;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
						m_szOcrValueLFBar[1].Format("R%ld_C%ld", lUserRow, lUserCol);
						m_WaferMapWrapper.GetSpecialDieName(ulRow, ulCol, m_szOcrValueLFBar[0]);
						lBarFirstX = lEncX;
						lBarFirstY = lEncY;
						m_lOcrBarCurrScnCol = ulPhyCol;
						m_lOcrBarCurrMapCol = ulCol;
						lColOffset = m_lOcrBarCurrScnCol - m_lOcrBarCurrMapCol;
						bFindOneBar = TRUE;
						SetScanKeyDie(ulRow, 2, lColOffset, 0);
						szFirst.Format(" first map %s OCR %s offset %03ld at(row %03ld col %03ld<->%03ld) wft %ld,%ld",
							m_szOcrValueLFBar[1], m_szOcrValueLFBar[0], lColOffset, ulRow, ulCol, ulPhyCol, lEncX, lEncY);
						break;
					}
				}
				if( bFindOneBar )
				{
					break;
				}
			}
		}

		if( bFindOneBar )
		{
		//	m_ulPrWindowHeight		= 480;			m_ulPrWindowWidth		= 512;
		//	HmiMessage("PR ing", "MS90", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, SHOW_TIME, glHMI_MSG_MODAL, NULL, 500, 300, NULL, NULL, NULL, NULL, 10 , 10);
			CString szPrvImagePath;
			for (ULONG ulCol = ulMapValidMaxCol; ulCol > (ULONG)lCtrCol; ulCol--)
			{
				ULONG ulPhyCol= ulCol+ lColOffset;
				ULONG lIndex = 0;
				DOUBLE dScore = 0;
				if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol) && GetGrabFocusData(ulRow, ulPhyCol, lEncX, lEncY, lIndex, dScore))
				{
					LONG lUserRow = 0, lUserCol = 0;
					ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
					m_szOcrValueLLBar[1].Format("R%ld_C%ld", lUserRow, lUserCol);
					m_WaferMapWrapper.GetSpecialDieName(ulRow, ulCol, m_szOcrValueLLBar[0]);
					MoveWaferTable(lEncX, lEncY);
					Sleep(WT_DELAY);
					if( m_lOcrBarIndex==0 )
						Sleep(1000);
					LONG lDeltaX = 0, lDeltaY = 0;
					WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
					szPrvImagePath.Format("%s\\Die_%s_%s.JPG", BAR_OCR_IMG_PATH, m_szOcrValueLLBar[1], m_szOcrValueLLBar[0]);
					for(INT mmm=0; mmm<100; mmm++)
					{
						CString szTemp;
						szTemp.Format("%s_%03d.JPG", szPrvImagePath, mmm);
						if (_access(szTemp, 0) == -1)
						{
							szPrvImagePath = szTemp;
							break;
						}
					}
					SavePrImage(szPrvImagePath);
					Sleep(PR_IMG_TIME);
					m_szBarLastDieImage = szPrvImagePath;
					szLast.Format("  last map %s OCR %s offset %03ld at(row %03ld col %03ld<->%03ld) wft %ld,%ld",
						m_szOcrValueLLBar[1], m_szOcrValueLLBar[0], lColOffset, ulRow, ulCol, ulPhyCol, lEncX, lEncY);
					break;
				}
			}
			LONG lDeltaX = 0, lDeltaY = 0;
			MoveWaferTable(lBarFirstX, lBarFirstY);
			Sleep(WT_DELAY);
			WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
			WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
			szPrvImagePath.Format("%s\\Die_%s_%s.JPG", BAR_OCR_IMG_PATH, m_szOcrValueLFBar[1], m_szOcrValueLFBar[0]);
			for(INT mmm=0; mmm<100; mmm++)
			{
				CString szTemp;
				szTemp.Format("%s_%03d.JPG", szPrvImagePath, mmm);
				if (_access(szTemp, 0) == -1)
				{
					szPrvImagePath = szTemp;
					break;
				}
			}
			SavePrImage(szPrvImagePath);
			Sleep(PR_IMG_TIME);
			m_szBarFirstDieImage = szPrvImagePath;
			m_lOcrBarCurrRow = ulRow;
			m_lOcrBarIndex++;
			m_szOcrBarName[0].Format("Right Bar %02d", m_lOcrBarIndex);
			if( fpBarOut!=NULL )
			{
				fprintf(fpBarOut, "%s %s\n", m_szOcrBarName[0], szFirst);
				fprintf(fpBarOut, "%s %s\n", m_szOcrBarName[0], szLast);
			}
		//	m_ulPrWindowHeight		= 0;			m_ulPrWindowWidth		= 0;
			break;
		}
	}
	if( fpBarOut!=NULL )
	{
		fclose(fpBarOut);
	}

	m_bOcrResultBarL[0] = FALSE;
	return bFindOneBar;
}

VOID CWaferPr::ConfirmScanBarMap_Finisar()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
	UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;
	UCHAR ucFakeGrade	= m_ucScanFakeEmptyGrade;

	BOOL  bBadCutAction	= m_bPrescanBadCutAction;
	BOOL  bDefectAction	= m_bPrescanDefectAction;
	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;
	BOOL  bFakeAction	= m_bScanDetectFakeEmpty;

	if( m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;
	if( m_bPrescanDefectToNullBin )
		ucDefectGrade = ucNullBin;
	if (m_bPrescanBadCutToNullBin)
		ucBadCutGrade = ucNullBin;

	m_bToUploadScanMapFile = TRUE;

	ULONG ulMapValidMinRow = 0, ulMapValidMaxRow = 0;
	ULONG ulMapValidMinCol = 0, ulMapValidMaxCol = 0;
	GetMapValidSize(ulMapValidMaxRow, ulMapValidMinRow,	ulMapValidMaxCol, ulMapValidMinCol);

	CString szMsg;
	szMsg.Format("WPR: scan map change grade: Badcut %d(%d); Defect %d(%d); Empty %d(%d); Good %d(%d); extra %d(%d); Fake %d(%d)", 
		bBadCutAction,	ucBadCutGrade,
		bDefectAction,	ucDefectGrade,
		bEmptyAction,	ucEmptyGrade,
		bGoodAction,	ucGoodGrade,
		0,	0,
		bFakeAction,	ucFakeGrade);
	SaveScanTimeEvent(szMsg);

	DOUBLE dUsedTime = GetTime();

	m_lPrescanSortingTotal	= 0; 
	m_lRescanMissingTotal	= 0;
	m_lPrescanVerifyResult	= 0;

	m_lTimeSlot[6] = 0;		//	06.	Process Mgt Cmd time before build map.

	// capture last image
	LONG lEncX, lEncY;

	SavePrescanInfoPr();
	ClearGoodInfo();
	ClearPrescanInfo();
	SaveScanTimeEvent("WPR: to reset map phy posn");
	ResetMapPhyPosn();

	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	m_lTimeSlot[7] = (LONG)(GetTime()-dUsedTime);	//	07.	constuct map used time.

	ULONG ulRow, ulCol;
	ULONG ulFrameID = 0;
	DOUBLE dDieAngle;
	CString szDieScore = "0";
	CString szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpBarOut = NULL;
	errno_t nErr = fopen_s(&fpBarOut, szConLogFile, "a");
	if ((nErr == 0) && (fpBarOut != NULL))
	{
		fprintf(fpBarOut, "\n\n");
		fprintf(fpBarOut, "bar confirm\n");
	}

	dUsedTime = GetTime();

	LONG lCtrCol = BAR_MAP_CTR_COL-5;
	if( IsMS90Sorting2ndPart() )
	{
		lCtrCol = BAR_MAP_CTR_COL + 5;
	}
	SaveScanTimeEvent("WPR: before set scan info done from builder.");

	for(ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
	{
		LONG lColOffsetL = 0, lColOffsetR = 0;
		LONG bLeft = FALSE, bRight = FALSE;
		GetScanKeyDie(ulRow, 1, lColOffsetL, bLeft);
		GetScanKeyDie(ulRow, 2, lColOffsetR, bRight);
		if ((nErr == 0) && (fpBarOut != NULL))
		{
			fprintf(fpBarOut, "left row %d, offset %d, ok=%d; right row %d, offset %d, ok=%d\n",
				ulRow, lColOffsetL, bLeft, ulRow, lColOffsetR, bRight);
		}
		for(ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}
			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol)==FALSE)
			{
				continue;
			}

			LONG ulScanRow = ulRow;
			LONG ulScanCol = ulCol + lColOffsetL;

			if (ulCol > (ULONG)lCtrCol)
			{
				if( bRight==0 )
				{
					continue;
				}
				ulScanCol = ulCol + lColOffsetR;
			}
			else
			{
				if( bLeft==0 )
				{
					continue;
				}
			}

			if ((nErr == 0) && (fpBarOut != NULL))
			{
				fprintf(fpBarOut, "    map row %d, col %d; scan row %d, col %d\n", 
					ulRow, ulCol, ulScanRow, ulScanCol);
			}

			ULONG lIndex = 0;
			DOUBLE dScore = 0;
			BOOL bInfo = GetGrabFocusData(ulScanRow, ulScanCol, lEncX, lEncY, lIndex, dScore);
			if( bInfo==FALSE )
			{
				if( fpBarOut!=NULL )
				{
					fprintf(fpBarOut, "    no raw data scan row %d, col %d; wft %d, %d\n", 
						ulScanRow, ulScanCol, lEncX, lEncY);
				}
				continue;
			}

			if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
			{
				if ((nErr == 0) && (fpBarOut != NULL))
				{
					fprintf(fpBarOut, "    out of limit map row %d, col %d; wft %d, %d\n", 
						ulRow, ulCol, lEncX, lEncY);
				}
			//	continue;
			}

			dDieAngle = 0.0;
			szDieScore = "0";
			ulFrameID = 0;
			USHORT usDieState = 0;
			if( lIndex>0 )
			{
				PR_UWORD uwScanDiePrID = 0;
				LONG lDummyX = 0, lDummyY = 0;
				WSGetScanPosition(lIndex, lDummyX, lDummyY, dDieAngle, szDieScore, usDieState, uwScanDiePrID, ulFrameID);
			}

			SetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);	//	from physical map builder
			if ((nErr == 0) && (fpBarOut != NULL))
			{
				fprintf(fpBarOut, "    set row %d, col %d, x %d, y %d, state=%d\n",
					ulRow, ulCol, lEncX, lEncY, usDieState);
			}
		}
	}

	LONG lDieTolX = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	LONG lDieTolY = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);
	LONG lMaxSpan = 2;
	for(ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
	{
		for(ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol)==FALSE)
			{
				continue;
			}

			USHORT usDieState = 0;
			BOOL bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);
			if( bInfo )
			{
				continue;
			}

			if( GetDieValidInX(ulRow, ulCol, lMaxSpan, lEncX, lEncY, FALSE) )
			{
				for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
				{
					LONG lScnX = 0, lScnY = 0;
					PR_UWORD uwScanDiePrID = 0;
					BOOL bScnInfo = WSGetScanPosition(ulIndex, lScnX, lScnY, dDieAngle, szDieScore, usDieState,
														uwScanDiePrID, ulFrameID);
					if( bScnInfo==false )
					{
						continue;
					}
					if( m_pbGetOut[ulIndex] )
					{
						continue;
					}
					if( labs(lScnX-lEncX)<lDieTolX && labs(lScnY-lEncY)<lDieTolY )
					{
						m_pbGetOut[ulIndex] = 1;
						SetScanInfo(ulRow, ulCol, lScnX, lScnY, dDieAngle, szDieScore, usDieState);
						break;
					}
				}
			}
		}	//	from left to right

		for (LONG ulCol = (LONG)ulMapValidMaxCol; ulCol >= (LONG)ulMapValidMinCol; ulCol--)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol)==FALSE)
			{
				continue;
			}

			USHORT usDieState = 0;
			BOOL bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);
			if( bInfo )
			{
				continue;
			}

			if( GetDieValidInX(ulRow, ulCol, 0-lMaxSpan, lEncX, lEncY, FALSE) )
			{
				for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
				{
					LONG lScnX = 0, lScnY = 0;
					PR_UWORD uwScanDiePrID = 0;
					BOOL bScnInfo = WSGetScanPosition(ulIndex, lScnX, lScnY, dDieAngle, szDieScore, usDieState,
														uwScanDiePrID, ulFrameID);
					if( bScnInfo==false )
					{
						continue;
					}
					if( m_pbGetOut[ulIndex] )
					{
						continue;
					}
					if( labs(lScnX-lEncX)<lDieTolX && labs(lScnY-lEncY)<lDieTolY )
					{
						m_pbGetOut[ulIndex] = 1;
						SetScanInfo(ulRow, ulCol, lScnX, lScnY, dDieAngle, szDieScore, usDieState);
						break;
					}
				}
			}
		}	//	from right to left
	}	//	to find any exist PR scan die that matches to the empty


	for(ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
	{
		for(ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol)==FALSE)
			{
				continue;
			}

			USHORT usDieState = 0;
			BOOL bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);
			if( bInfo )
			{
				continue;
			}

			if( GetDieValidInX(ulRow, ulCol, lMaxSpan, lEncX, lEncY, FALSE) )
			{
				MoveWaferTable(lEncX, lEncY);
				Sleep(100);
				LONG lDeltaX = 0, lDeltaY = 0;
				PR_UWORD usDieType = PR_ERR_GOOD_DIE;
				PR_REAL dDieAngle = 0.0;
				if( WprSearchInspectDie(TRUE, 1, lDeltaX, lDeltaY, dDieAngle, usDieType) )
				{
					lEncX = lEncX + lDeltaX;
					lEncY = lEncY + lDeltaY;
					LONG lScore = (LONG)(*m_psmfSRam)["WaferPr"]["DieScore"];
					szDieScore.Format("%d", lScore);
					BOOL bIsDefect = DieIsDefective(usDieType) || DieIsInk(usDieType) || DieIsChip(usDieType);
					BOOL bIsBadCut = DieIsBadCut(usDieType);
					usDieState = EncodeDieState(bIsDefect, bIsBadCut, FALSE);
					SetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);
					if( fpBarOut!=NULL )
					{
						fprintf(fpBarOut, "    nearby regrab row %d, col %d, x %d, y %d, state=%d\n",
							ulRow, ulCol, lEncX, lEncY, usDieState);
					}
				}
				else
				{
					if ((nErr == 0) && (fpBarOut != NULL))
					{
						fprintf(fpBarOut, "    nearby no die row %d, col %d, x %d, y %d, state=%d\n",
							ulRow, ulCol, 0, 0, -2);
					}
				}
			}
			else
			{
				if ((nErr == 0) && (fpBarOut != NULL))
				{
					fprintf(fpBarOut, "    nearby fail row %d, col %d, x %d, y %d, state=%d\n",
						ulRow, ulCol, 0, 0, -1);
				}
			}
		}	//	from left to right

		for(LONG ulCol = (LONG)ulMapValidMaxCol; ulCol >= (LONG)ulMapValidMinCol; ulCol--)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol)==FALSE)
			{
				continue;
			}

			USHORT usDieState = 0;
			BOOL bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);
			if( bInfo )
			{
				continue;
			}

			if( GetDieValidInX(ulRow, ulCol, 0-lMaxSpan, lEncX, lEncY, FALSE) )
			{
				MoveWaferTable(lEncX, lEncY);
				Sleep(100);
				LONG lDeltaX = 0, lDeltaY = 0;
				PR_UWORD usDieType = PR_ERR_GOOD_DIE;
				PR_REAL dDieAngle = 0.0;
				if( WprSearchInspectDie(TRUE, 1, lDeltaX, lDeltaY, dDieAngle, usDieType) )
				{
					lEncX = lEncX + lDeltaX;
					lEncY = lEncY + lDeltaY;
					LONG lScore = (LONG)(*m_psmfSRam)["WaferPr"]["DieScore"];
					szDieScore.Format("%d", lScore);
					BOOL bIsDefect = DieIsDefective(usDieType) || DieIsInk(usDieType) || DieIsChip(usDieType);
					BOOL bIsBadCut = DieIsBadCut(usDieType);
					usDieState = EncodeDieState(bIsDefect, bIsBadCut, FALSE);
					SetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);
					if( fpBarOut!=NULL )
					{
						fprintf(fpBarOut, "    nearby regrab row %d, col %d, x %d, y %d, state=%d\n",
							ulRow, ulCol, lEncX, lEncY, usDieState);
					}
				}
				else
				{
					if ((nErr == 0) && (fpBarOut != NULL))
					{
						fprintf(fpBarOut, "    nearby no die row %d, col %d, x %d, y %d, state=%d\n",
							ulRow, ulCol, 0, 0, -2);
					}
				}
			}
			else
			{
				if ((nErr == 0) && (fpBarOut != NULL))
				{
					fprintf(fpBarOut, "    nearby fail row %d, col %d, x %d, y %d, state=%d\n",
						ulRow, ulCol, 0, 0, -1);
				}
			}
		}	//	from right to left
	}	//	go to the empty die location and do PR search, if find, update the data.

	SaveScanTimeEvent("WPR: after set scan info done from builder.");

	SaveScanTimeEvent("WPR: to remove all data of physical map builder.");
	// transfer data from prescan result to map physical position array
	ULONG ulPickEmptyCount = 0, ulPickDefectCount = 0, ulPickBadCutCount = 0, ulPickGoodCount = 0;
	ULONG ulEmptyIdx = 0, ulDefectIdx = 0, ulBadCutIdx = 0, ulGoodIdx = 0;
	ULONG ulMapTotal = 0;

	DelPrescanRunPosn();

	SaveScanTimeEvent("WPR: loop to fill in map position.");

	for(ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
	{
		for(ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
		{
			if( IsOutMS90SortingPart(ulRow, ulCol) )
			{
				continue;
			}

			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow, ulCol)==FALSE)
			{
				continue;
			}

			ulMapTotal++;

			UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);

			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			BOOL bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieScore, usDieState);
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);

			if( !bInfo )
			{
				SetEmptyFull(ulEmptyIdx, ulRow, ulCol, ucB4ScanGrade);
				ulEmptyIdx++;
				if( usDieState==9889 )
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_UNPICK_REGRAB_EMPTY);
				else
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
				ulPickEmptyCount++;

				if( bEmptyAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
				}
				else if( m_bPrescanEmptyMarkUnPick )
				{
					m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
				}
				continue;
			}

			m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
			SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);

			if (bIsBadCut)
			{
				SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucB4ScanGrade);
				ulBadCutIdx++;
				ulPickBadCutCount++;

				if( IsNoPickBadCut() )
				{
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
				}

				if( bBadCutAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucBadCutGrade);
				}
				else
				{
					if( m_bPrescanBadCutMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
					}
				}
				continue;
			}

			if( bIsFakeEmpty )
			{
				SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
				ulDefectIdx++;
				ulPickDefectCount++;

				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);

				if( bFakeAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucFakeGrade);
				}
				continue;
			}
			
			if( bIsDefect )
			{
				SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
				ulDefectIdx++;
				ulPickDefectCount++;

				if( IsNoPickDefect() )
				{
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
				}

				if( bDefectAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucDefectGrade);
				}
				else
				{
					if( m_bPrescanDefectMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
					}
				}
				continue;
			}
			
			ulGoodIdx++;
			ulPickGoodCount++;
			if( bGoodAction )
			{
				m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucGoodGrade);
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();

	m_lTimeSlot[8] = (LONG)(GetTime()-dUsedTime);	//	08.	update wafer map grade and position

	if ((nErr == 0) && (fpBarOut != NULL))
	{
		fclose(fpBarOut);
	}

	LONG lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY;
	pUtl->GetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);
	pUtl->SetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);

	if( IsMS90Sorting2ndPart() )
	{
		ULONG ulUpGood	 = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];
		ULONG ulUpDefect = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
		ULONG ulUpBadcut = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
		ULONG ulUpEmpty  = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];
		ULONG ulUpTotal	 = (*m_psmfSRam)["WaferTable"]["PRESCAN_TOTAL_DIE"];
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulUpGood   + ulGoodIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulUpDefect + ulDefectIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulUpBadcut + ulBadCutIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulUpEmpty  + ulEmptyIdx;
		SetMapTotalDie(ulMapTotal + ulUpTotal);
	}
	else
	{
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutIdx;
		(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulEmptyIdx;
		(*m_psmfSRam)["WaferTable"]["PRESCAN_TOTAL_DIE"]		= ulMapTotal;
		SetMapTotalDie(ulMapTotal);
	}
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= 0;

	// build prescan map complete
	ClearGoodInfo();	ClearWSPrescanInfo();

	DelGrabFocusData();
	DelScanKeyDie();
	delete [] m_pbGetOut;
//	AutoBondScreen(PR_TRUE);
}

VOID CWaferPr::CheckOcrViewWindow()
{
	LONG lSizeX = GetNmlSizePixelX();
	LONG lSizeY = GetNmlSizePixelY();

	m_stOcrViewWindow.coCorner1.x = max(m_stOcrViewWindow.coCorner1.x, PR_MAX_WIN_ULC_X);
	m_stOcrViewWindow.coCorner1.y = max(m_stOcrViewWindow.coCorner1.y, PR_MAX_WIN_ULC_Y);
	m_stOcrViewWindow.coCorner1.x = min(m_stOcrViewWindow.coCorner1.x, PR_MAX_WIN_LRC_X);
	m_stOcrViewWindow.coCorner1.y = min(m_stOcrViewWindow.coCorner1.y, PR_MAX_WIN_LRC_Y);
	m_stOcrViewWindow.coCorner2.x = max(m_stOcrViewWindow.coCorner2.x, PR_MAX_WIN_ULC_X);
	m_stOcrViewWindow.coCorner2.y = max(m_stOcrViewWindow.coCorner2.y, PR_MAX_WIN_ULC_Y);
	m_stOcrViewWindow.coCorner2.x = min(m_stOcrViewWindow.coCorner2.x, PR_MAX_WIN_LRC_X);
	m_stOcrViewWindow.coCorner2.y = min(m_stOcrViewWindow.coCorner2.y, PR_MAX_WIN_LRC_Y);

	if (m_stOcrViewWindow.coCorner2.x <= m_stOcrViewWindow.coCorner1.x ||
		m_stOcrViewWindow.coCorner2.y <= m_stOcrViewWindow.coCorner1.y)
	{
		m_stOcrViewWindow.coCorner1.x = (PR_WORD)_round((PR_MAX_WIN_ULC_X + PR_MAX_WIN_LRC_X - lSizeX) / 2);
		m_stOcrViewWindow.coCorner1.y = (PR_WORD)_round((PR_MAX_WIN_ULC_Y + PR_MAX_WIN_LRC_Y - lSizeY) / 2);
		m_stOcrViewWindow.coCorner2.x = (PR_WORD)_round((PR_MAX_WIN_ULC_X + PR_MAX_WIN_LRC_X + lSizeX) / 2);
		m_stOcrViewWindow.coCorner2.y = (PR_WORD)_round((PR_MAX_WIN_ULC_Y + PR_MAX_WIN_LRC_Y + lSizeY) / 2);
	}

	LONG lWinX = m_stOcrViewWindow.coCorner2.x - m_stOcrViewWindow.coCorner1.x;
	LONG lWinY = m_stOcrViewWindow.coCorner2.y - m_stOcrViewWindow.coCorner1.y;
	if( lWinX<2048 )
	{
		lWinX = _round((2048 - lWinX) / 2);
		m_stOcrViewWindow.coCorner1.x -= (PR_WORD)lWinX;
		m_stOcrViewWindow.coCorner2.x += (PR_WORD)lWinX;
	}
	if( lWinY<1920 )
	{
		lWinY = _round((1920-lWinY) / 2);
		m_stOcrViewWindow.coCorner1.y -= (PR_WORD)lWinY;
		m_stOcrViewWindow.coCorner2.y += (PR_WORD)lWinY;
	}
}
//	LONG lDeltaX = 0, lDeltaY = 0;
//	DrawRectangleBox(m_stOcrViewWindow.coCorner1, m_stOcrViewWindow.coCorner2, PR_COLOR_BLUE);
//	WprSearchDie(TRUE, 1, lDeltaX, lDeltaY);
//	SavePrImage("c:\\MapSorter\\GrabImage.jpg");

BOOL CWaferPr::SavePrImage(CString szImageFilename)
{
//	return PrintPartScreen(szImageFilename, 702, 160, 1214, 640);
	PR_SAVE_DISP_IMG_CMD stSaveDispCmd;
	PR_SAVE_DISP_IMG_RPY stRpy;

	PR_InitSaveDispImgCmd(&stSaveDispCmd);
#if 0
	stSaveDispCmd.emIsWinValid = PR_TRUE; //It is must to set TRUE if saving ROI image
	stSaveDispCmd.stWin.coCorner1.x = m_stOcrViewWindow.coCorner1.x;
	stSaveDispCmd.stWin.coCorner1.y = m_stOcrViewWindow.coCorner1.y;
	stSaveDispCmd.stWin.coCorner2.x = m_stOcrViewWindow.coCorner2.x;
	stSaveDispCmd.stWin.coCorner2.y = m_stOcrViewWindow.coCorner2.y;
	CString szMsg;
	szMsg.Format("grab area UL %d,%d, LR %d,%d",
		stSaveDispCmd.stWin.coCorner1.x, stSaveDispCmd.stWin.coCorner1.y, stSaveDispCmd.stWin.coCorner2.x, stSaveDispCmd.stWin.coCorner2.y);
	SetAlarmLog(szMsg);
#endif
	CString szTempPath = "C:\\MapSorter\\UserData\\Capture.JPG";//in case szImageFilename is too long
	//strcpy_s((char*) stSaveDispCmd.aubSourceLogFileNameWithPath, sizeof( stSaveDispCmd.aubSourceLogFileNameWithPath), (LPCTSTR) szImageFilename);
	strcpy_s((char*) stSaveDispCmd.aubSourceLogFileNameWithPath, sizeof(stSaveDispCmd.aubSourceLogFileNameWithPath),  (LPCTSTR) szTempPath);
	PR_SaveDispImgCmd(&stSaveDispCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stRpy);

	BOOL bReturn = TRUE;
	if( (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR) ||
		(stRpy.stStatus.uwPRStatus!= PR_ERR_NOERR) )
	{
		CString szMsg;
		szMsg.Format("PR_SaveDispImgCmd() is finished with status %u, %u!\n",
			stRpy.stStatus.uwCommunStatus, stRpy.stStatus.uwPRStatus);
		SetAlarmLog(szMsg);
		bReturn = FALSE;
	}
	CopyFile(szTempPath,szImageFilename,FALSE);
	DeleteFile(szTempPath);


	return bReturn;
}

BOOL CWaferPr::WprSearchInspectDie(CONST BOOL bNormalDie, CONST LONG lPrID,
								   LONG &lDieOffsetX, LONG &lDieOffsetY, PR_REAL &fDieRotate, PR_UWORD &usDieType)
{
	if (PR_NotInit())
	{
		return FALSE;
	}

	//Do Search die on this position
	LONG lRefDieNo = lPrID;
	BOOL bDieType = WPR_NORMAL_DIE;
	if (bNormalDie == TRUE)
	{
		bDieType = WPR_NORMAL_DIE;
	}
	else
	{
		bDieType = WPR_REFERENCE_DIE;
	}
	BOOL	bBackupAlignTemp = m_bSrchEnableBackupAlign;
	m_bSrchEnableBackupAlign = FALSE;

	PR_COORD		stDieOffset; 
	PR_REAL			fDieScore = 0;
	BOOL			bStatus = TRUE;
	int				siStepX = 0; 
	int				siStepY = 0; 
	PR_BOOLEAN bInspect = PR_TRUE;
	PR_WORD wResult = ManualSearchDie(bDieType, lRefDieNo, PR_TRUE, PR_TRUE, bInspect, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
	if( usDieType==PR_ERR_NO_DIE )
		(*m_psmfSRam)["WaferPr"]["DieScore"] = 0;	//	(DOUBLE) fDieScore;
	else
		(*m_psmfSRam)["WaferPr"]["DieScore"] = (LONG) fDieScore;	//	(DOUBLE) fDieScore;
	m_bSrchEnableBackupAlign = bBackupAlignTemp;

	m_dPrDieCenterX = 0;
	m_dPrDieCenterY = 0;
	lDieOffsetX = 0;
	lDieOffsetY = 0;
	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			m_dPrDieCenterX = stDieOffset.x;
			m_dPrDieCenterY = stDieOffset.y;
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		}

		//Return result
		lDieOffsetX	= (LONG)(siStepX);
		lDieOffsetY	= (LONG)(siStepY);

		//Display PR status if necessary
		CString szDieResult;
		ConvertDieTypeToText(usDieType, szDieResult);
		m_szSrhDieResult = "Die Type: " + szDieResult;

		return DieIsAlignable(usDieType) || DieIsGood(usDieType);
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}
