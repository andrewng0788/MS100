/////////////////////////////////////////////////////////////////
// WPR_LrnDie.cpp : Learn Die Function of the CWaferPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, November 9, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "prtype.h"
#include "PrZoomSensor.h"
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferPr::IsWaferRotated180()
{
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (pWaferTable->GetWaferSortPart() == 2)
	{
		return TRUE;
	}

	return FALSE;
}


PR_UWORD CWaferPr::AutoDetectDieCorner(BOOL bDieType, LONG lRefDieNo)
{
	PR_AUTO_DETECT_DIE_CMD		stAutoDetectCmd;
	PR_AUTO_DETECT_DIE_RPY1 	stAutoDetectRpy1;
	PR_AUTO_DETECT_DIE_RPY2 	stAutoDetectRpy2;
	PR_PURPOSE ubPpsI = GetRunPurposeI();

	//v2.78T1
	LONG lDieNo = 0;
	if (bDieType == WPR_NORMAL_DIE)
		lDieNo = WPR_GEN_NDIE_OFFSET + lRefDieNo;
	else
		lDieNo = WPR_GEN_RDIE_OFFSET + lRefDieNo;


	PR_InitAutoDetectDieCmd(&stAutoDetectCmd);

	stAutoDetectCmd.emCameraNo			= GetRunCamID();
	PR_COORD stPoint;
	stPoint.x = (PR_WORD)GetPrCenterX();
	stPoint.y = (PR_WORD)GetPrCenterY();
	VerifyPRPoint(&stPoint);
	stAutoDetectCmd.coSelectPoint		= stPoint;
	stAutoDetectCmd.emLrnDieAlg			= PR_STREET_MATCHING;
	stAutoDetectCmd.emPurpose			= ubPpsI;

	stAutoDetectCmd.emObjSizeRatioChk	= PR_TRUE;
	stAutoDetectCmd.emObjSizeChk		= PR_TRUE;
	stAutoDetectCmd.rObjSizeRatio		= 1.0;
	stAutoDetectCmd.rszObjSize.x		= GetDieSizePixelX(lDieNo);
	stAutoDetectCmd.rszObjSize.y		= GetDieSizePixelY(lDieNo);

	PR_WIN stSrchArea;
	GetSearchDieArea(&stSrchArea, WPR_REFERENCE_DIE);
	stAutoDetectCmd.stDetectWin.coCorner1	= stSrchArea.coCorner1;
	stAutoDetectCmd.stDetectWin.coCorner2	= stSrchArea.coCorner2;
	stAutoDetectCmd.emBackgroundInfo		= GetLrnBackGround(lDieNo);

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	PR_AutoDetectDieCmd(&stAutoDetectCmd, ubSID, ubRID, &stAutoDetectRpy1);
	if (stAutoDetectRpy1.uwCommunStatus != PR_COMM_NOERR)
	{
		return PR_COMM_ERR;
	}
	if (stAutoDetectRpy1.uwPRStatus != PR_ERR_NOERR)
	{
		return stAutoDetectRpy1.uwPRStatus;
	}


	PR_AutoDetectDieRpy(ubSID, &stAutoDetectRpy2);
	if (stAutoDetectRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		return PR_COMM_ERR;
	}

	if (stAutoDetectRpy2.stStatus.uwPRStatus != PR_ERR_NOERR)
	{
		return stAutoDetectRpy2.stStatus.uwPRStatus;
	}
	else
	{
		int nUR_X = stAutoDetectRpy2.acoDieCorners[PR_UPPER_RIGHT].x;
		int nUR_Y = stAutoDetectRpy2.acoDieCorners[PR_UPPER_RIGHT].y;
		int nUL_X = stAutoDetectRpy2.acoDieCorners[PR_UPPER_LEFT].x;
		int nUL_Y = stAutoDetectRpy2.acoDieCorners[PR_UPPER_LEFT].y;
		int nLL_X = stAutoDetectRpy2.acoDieCorners[PR_LOWER_LEFT].x;
		int nLL_Y = stAutoDetectRpy2.acoDieCorners[PR_LOWER_LEFT].y;
		int nLR_X = stAutoDetectRpy2.acoDieCorners[PR_LOWER_RIGHT].x;
		int nLR_Y = stAutoDetectRpy2.acoDieCorners[PR_LOWER_RIGHT].y;

		if( GetDieShape()==WPR_RHOMBUS_DIE )
		{
			m_stLearnDieCornerPos[PR_UPPER_RIGHT].x	= nUR_X;
			m_stLearnDieCornerPos[PR_UPPER_RIGHT].y	= nUR_Y;
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= nUL_X;
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= nUL_Y;
			m_stLearnDieCornerPos[PR_LOWER_LEFT].x	= nLL_X;
			m_stLearnDieCornerPos[PR_LOWER_LEFT].y	= nLL_Y;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x	= nLR_X;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y	= nLR_Y;
		}
		else
		{
			int nMinX = min(nUL_X, nLL_X);
			int nMaxX = max(nUR_X, nLR_X);
			int nMinY = min(nUL_Y, nUR_Y);
			int nMaxY = max(nLL_Y, nLR_Y);

			m_stLearnDieCornerPos[PR_UPPER_RIGHT].x	= nMaxX;
			m_stLearnDieCornerPos[PR_UPPER_RIGHT].y	= nMinY;
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= nMinX;
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= nMinY;
			m_stLearnDieCornerPos[PR_LOWER_LEFT].x	= nMinX;
			m_stLearnDieCornerPos[PR_LOWER_LEFT].y	= nMaxY;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x	= nMaxX;
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y	= nMaxY;
		}
	}

	return PR_ERR_NOERR;
}

PR_WORD CWaferPr::AutoSearchCircle(LONG pcx, LONG pcy, LONG ecx, LONG ecy, UINT &nX, UINT &nY) 
{
	PR_DETECT_SHAPE_CMD     pstCmd;
	PR_DETECT_SHAPE_RPY1    pstRpy1;
	PR_DETECT_SHAPE_RPY2    pstRpy2;

	PR_InitDetectShapeCmd(&pstCmd);
	PR_COORD prcenter;
	PR_COORD expectcenter;
	prcenter.x = (PR_WORD)pcx;
	prcenter.y = (PR_WORD)pcy;
	expectcenter.x = (PR_WORD)ecx;
	expectcenter.y = (PR_WORD)ecy;

	pstCmd.emSameView = PR_FALSE;
	pstCmd.uwRecordID = m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET+3];
	pstCmd.rDetectScore = 70 ;
	pstCmd.emSelectMode = PR_SHAPE_SELECT_MODE_CENTRE ;
	pstCmd.coExpectCentre = expectcenter;
	pstCmd.coPositionVar = prcenter;


	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_DetectShapeCmd(&pstCmd, ubSID, ubRID, &pstRpy1);

	if ( (pstRpy1.uwCommunStatus != PR_COMM_NOERR) || (pstRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("PR_LrnShapeCmd Error1: COMM = %d, PR = %d", pstRpy1.uwCommunStatus, pstRpy1.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}
	
	PR_DetectShapeRpy(ubSID, &pstRpy2);

	if ( (pstRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) || (pstRpy2.stStatus.uwPRStatus != PR_ERR_NOERR) )
	{
		CString szLog;
		szLog.Format("PR_LrnShapeCmd Error2: COMM = %d, PR = %d", pstRpy2.stStatus.uwCommunStatus, pstRpy2.stStatus.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}

	m_pPrGeneral->DrawSmallCursor(ubSID, ubRID, (PR_WORD)pstRpy2.stShapeRes.rcoCentre.x, (PR_WORD)pstRpy2.stShapeRes.rcoCentre.y);
	
	if ( pstRpy2.emShapeFound == PR_SHAPE_FOUND_CIRCLE )
	{	
		CString szLog;
		nX = (UINT) pstRpy2.stCircleRes.coCentre.x;
		nY = (UINT) pstRpy2.stCircleRes.coCentre.y;
		szLog.Format("Centre coordinate : %d, %d", pstRpy2.stCircleRes.coCentre.x, pstRpy2.stCircleRes.coCentre.y);
		HmiMessageEx(szLog);
		return PR_TRUE;
	}
	else if ( pstRpy2.emShapeFound == PR_SHAPE_FOUND_ELLIPSE )
	{
		CString szLog;
		nX = (UINT) pstRpy2.stShapeRes.rcoCentre.x;
		nY = (UINT) pstRpy2.stShapeRes.rcoCentre.y;
		szLog.Format("Centre coordinate : %d, %d", nX, nY);
		HmiMessageEx(szLog);
		return PR_TRUE;
	}
	//if ( pstRpy2.emShapeFound != PR_SHAPE_FOUND_CIRCLE && pstRpy2.emShapeFound != PR_SHAPE_FOUND_ELLIPSE )
	else 
	{
		HmiMessageEx("Find circle error!!!!!");
		//return PR_FALSE;
	}

	return PR_FALSE;
}

PR_UWORD CWaferPr::AutoLearnDie(BOOL bDieType, LONG lRefDieNo)
{
	PR_MANUAL_LRN_DIE_CMD		stLrnDieCmd;
	PR_MANUAL_LRN_DIE_RPY1		stLrnDieRpy1;
	PR_MANUAL_LRN_DIE_RPY2		stLrnDieRpy2;
	PR_DEFECT_WIN				stInspDefectWin;
	PR_DEFECT_WIN				stAlignDefectWin;
	PR_UWORD					usInspAlg;
	PR_UWORD 					lLearnDieStauts;				
	LONG						lObjType = 0;			
	LONG						lDieNo = 0;
	short						i;
	PR_PURPOSE ubPpsI = GetRunPurposeI();

	//4.52D17learn Change Purpose ID
	if(m_bLearnLookupCamera)
	{
		if (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 )
		{
			ubPpsI = MS899_POSTSEAL_BH1_GEN_PURPOSE;
		}
		else if (m_lCurRefDieNo == MS899_UPLOOK_REF_DIENO_BH2)
		{
			ubPpsI = MS899_POSTSEAL_BH2_GEN_PURPOSE;
		}
		else
		{
			AfxMessageBox("David: Check RefNo", MB_SYSTEMMODAL);
		}
		HmiMessage("Uplook ---  Start learn Post Seal Die");
		CMSLogFileUtility::Instance()->WPR_LearnPitchLog("Uplook ---  Start learn Post Seal Die");
	}
	//Update Others die corner
	if ( GetDieShape() == WPR_RECTANGLE_DIE )
	{
		m_stLearnDieCornerPos[PR_UPPER_RIGHT].x	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].x; 
		m_stLearnDieCornerPos[PR_UPPER_RIGHT].y	= m_stLearnDieCornerPos[PR_UPPER_LEFT].y; 
		m_stLearnDieCornerPos[PR_LOWER_LEFT].x	= m_stLearnDieCornerPos[PR_UPPER_LEFT].x; 
		m_stLearnDieCornerPos[PR_LOWER_LEFT].y	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].y; 
	}


	//Update Inspection detect / ignore PR structure
	if (m_lInspDetectWinNo == 0)
	{
		stInspDefectWin.uwNDetectWin = 0;
	}
	else
	{
		stInspDefectWin.uwNDetectWin = (PR_UWORD)m_lInspDetectWinNo;
		for (i=0; i<m_lInspDetectWinNo; i++)
		{
			stInspDefectWin.astDetectWin[i] = m_stInspDetectWin[i];
		}
	}

	if (m_lInspIgnoreWinNo == 0)
	{
		stInspDefectWin.uwNIgnoreWin = 0;
	}
	else
	{
		stInspDefectWin.uwNIgnoreWin = (PR_UWORD)m_lInspIgnoreWinNo;
		for (i=0; i<m_lInspIgnoreWinNo; i++)
		{
			stInspDefectWin.astIgnoreWin[i] = m_stInspIgnoreWin[i];
		}
	}

	//Update Alignment detect / ignore PR structure
	if (m_lAlnDetectWinNo == 0)
	{
		stAlignDefectWin.uwNDetectWin = 0;
	}
	else
	{
		stAlignDefectWin.uwNDetectWin = (PR_UWORD)m_lAlnDetectWinNo;
		for (i=0; i<m_lAlnDetectWinNo; i++)
		{
			stAlignDefectWin.astDetectWin[i] = m_stAlnDetectWin[i];
		}
	}

	if (m_lAlnIgnoreWinNo == 0)
	{
		stAlignDefectWin.uwNIgnoreWin = 0;
	}
	else
	{
		stAlignDefectWin.uwNIgnoreWin = (PR_UWORD)m_lAlnIgnoreWinNo;
		for (i=0; i<m_lAlnIgnoreWinNo; i++)
		{
			stAlignDefectWin.astIgnoreWin[i] = m_stAlnIgnoreWin[i];
		}
	}


	if ( bDieType == WPR_NORMAL_DIE )
		lDieNo = WPR_GEN_NDIE_OFFSET + lRefDieNo;
	else
	{
		lDieNo = WPR_GEN_RDIE_OFFSET + lRefDieNo;
		if( IsOcrAOIMode() && IsCharDieInUse() && lRefDieNo>=WPR_GEN_OCR_DIE_START)
		{
			lDieNo = lRefDieNo;
		}
	}


	if( IsEnableZoom() && (bDieType == WPR_NORMAL_DIE) )
	{
		if( lDieNo==0 )
		{
			SetNormalZoomFactor(GetRunZoom());
		}
		if( lDieNo==(GetPrescanPrID()-1) )
		{
			SetPrescanZoomFactor(GetRunZoom());
			GetScanFovWindow();	//	when learn prescan die PR record.
		}
	}

	if (bDieType == WPR_NORMAL_DIE)
	{
		if( m_bNormalRoughLedDie )
		{
			lObjType = PR_OBJ_TYPE_LED_DIE;//PR_OBJ_TYPE_ROUGH_LED_DIE;
		}
		else
		{
			lObjType = PR_OBJ_TYPE_LED_DIE;
		}
#ifdef VS_5MCAM
		BOOL bScanPr = FALSE;
		if( IsDP() )
		{
			if( IsDP_ScanCam() )
				bScanPr = TRUE;
		}
		else
		{
			if( IsEnableZoom() && GetRunZoom()==GetScnZoom() )
				bScanPr = TRUE;
		}
		if( bScanPr && lRefDieNo==GetPrescanPrID())
		{
			lObjType = PR_OBJ_TYPE_FAST_PRESCAN_LED_DIE;
		}
#endif
	}
	else
	{
		lObjType = PR_OBJ_TYPE_LED_DIE; //PR_OBJ_TYPE_LED_REF_DIE;
/*
		//v4.56A12
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		BOOL bEnableBHUplookPrFcn	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
		if ( bEnableBHUplookPrFcn && 
			(m_bSelectDieType == WPR_REFERENCE_DIE) && 
			(lRefDieNo == MS899_UPLOOK_REF_DIENO_BH1 || lRefDieNo == MS899_UPLOOK_REF_DIENO_BH2) ) 
		{
			lObjType = PR_OBJ_TYPE_LED_DIE;
		}
*/
	}

	usInspAlg = GetLrnInspMethod(lDieNo);

	//Free current PR record 
	PR_UWORD uwRecordID = (PR_UWORD)m_ssGenPRSrchID[lDieNo];
	m_pPrGeneral->FreePRRecord(uwRecordID, GetRunSenID(), GetRunRecID());
	m_ssGenPRSrchID[lDieNo] = 0;

	PR_InitManualLrnDieCmd(&stLrnDieCmd);
	stLrnDieCmd.emCameraNo 							= GetRunCamID();
	stLrnDieCmd.emObjectType						= (PR_OBJ_TYPE)lObjType;
	stLrnDieCmd.aemPurpose[0]						= ubPpsI;
	stLrnDieCmd.emIsCornerFitting					= PR_FALSE;

	if ( GetDieShape() == WPR_RECTANGLE_DIE )
	{
		stLrnDieCmd.emIsRectObj						= PR_TRUE;
		stLrnDieCmd.uwNumOfDieCorners				= 4;
		stLrnDieCmd.acoDieCorners[PR_UPPER_RIGHT]	= m_stLearnDieCornerPos[PR_UPPER_RIGHT];
		stLrnDieCmd.acoDieCorners[PR_UPPER_LEFT]	= m_stLearnDieCornerPos[PR_UPPER_LEFT];
		stLrnDieCmd.acoDieCorners[PR_LOWER_LEFT]	= m_stLearnDieCornerPos[PR_LOWER_LEFT];
		stLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT]	= m_stLearnDieCornerPos[PR_LOWER_RIGHT];
	}
	else if ( GetDieShape() == WPR_TRIANGULAR_DIE )	//v4.06
	{
//AfxMessageBox("Andrew: Triangular die ...", MB_SYSTEMMODAL);
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= WPR_TRIANGLE_CORNERS;
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_2];
	}
	else if ( GetDieShape() == WPR_RHOMBUS_DIE )
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= WPR_RHOMBUS_CORNERS;
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_2];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_3]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_3];
		CString szMsg = "Auto Learn Die ";
		CString szTemp;
		for(int i=0; i<4; i++)
		{
			szTemp.Format("%d,%d ", stLrnDieCmd.acoDieCorners[i].x, stLrnDieCmd.acoDieCorners[i].y);
			szMsg += szTemp;
		}
		CMSLogFileUtility::Instance()->WPR_LearnPitchLog(szMsg);
	}
	else
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= WPR_HEXAGON_CORNERS;
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_2];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_3]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_3];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_4]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_4];
		stLrnDieCmd.acoDieCorners[WPR_DIE_CORNER_5]	= m_stLearnDieCornerPos[WPR_DIE_CORNER_5];
	}

	stLrnDieCmd.stDefectWin							= stInspDefectWin;		//Inspection ignore/detect region
	stLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin	= stAlignDefectWin;		//Alignment ignore/detect region
	stLrnDieCmd.emPreRot							= PR_TRUE;
	stLrnDieCmd.emIsGrabFromPurpose					= PR_TRUE;
	stLrnDieCmd.emInspResolution					= (PR_RESOLUTION)(m_lGenLrnInspRes[lDieNo]);		//v3.17T1
	stLrnDieCmd.emBackgroundInfo					= GetLrnBackGround(lDieNo);
	short sLrnAlign;

	//v4.08
	switch( m_lGenLrnFineSrch[lDieNo] )
	{
	case 0:
		sLrnAlign = PR_RESOLUTION_LOW;
		break;
	case 2:
		sLrnAlign = PR_RESOLUTION_HIGH;
		break;
	case 1:
	default:
		sLrnAlign = PR_RESOLUTION_MEDIUM;
		break;
	}

	if( IsOcrAOIMode() && IsCharDieInUse() && bDieType!=WPR_NORMAL_DIE && lDieNo>=WPR_GEN_OCR_DIE_START )
	{
		sLrnAlign = PR_RESOLUTION_HIGH;
		stLrnDieCmd.emCoarseSrchResolution = PR_RESOLUTION_HIGH;
		CString szMsg;
		szMsg.Format("Learn OCR die set to HIGH resolution for ID %d", lDieNo);
		SetAlarmLog(szMsg);
	}

	stLrnDieCmd.emFineSrchResolution				= (PR_RESOLUTION)sLrnAlign;
	stLrnDieCmd.emAlignAlg							= GetLrnAlignAlgo(lDieNo);
	stLrnDieCmd.emInspAlg							= (PR_DIE_INSP_ALG)(usInspAlg);


	//v2.77
	//PR Die Circle Detection option
	//** Need dot use Vision v3.08 lib **//
	if (m_bPrCircleDetection && (bDieType == WPR_NORMAL_DIE))
	{
		stLrnDieCmd.emIsLrnShapeCfm		= PR_TRUE;
		stLrnDieCmd.emShapeType			= PR_SHAPE_TYPE_CIRCLE;
		stLrnDieCmd.emShapeAttribute	= PR_OBJ_ATTRIBUTE_DARK;
		stLrnDieCmd.emIsUseDefaultShapeWin = PR_TRUE;
	}


	//v3.44T1
	if ( m_bEnable2Lighting && (bDieType == WPR_NORMAL_DIE) )
	{
		stLrnDieCmd.emIsGrabFromPurpose						= PR_TRUE;
		stLrnDieCmd.aemPurpose[PR_PURPOSE_INDEX_ALIGNMENT]	= PR_PURPOSE_CAM_A0;
		stLrnDieCmd.aemPurpose[PR_PURPOSE_INDEX_INSPECTION]	= ubPpsI;
	}


	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	//Start AutoLearn Die & Get Reply 1
		//4.52D17Learn Change SID RID (auto learn)
	if(m_bLearnLookupCamera)
	{
		ubSID = PSPR_SENDER_ID;
		ubRID = PSPR_RECV_ID;

	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( IsEnableZoom() && GetRunZoom() == CPrZoomSensorMode::PR_ZOOM_MODE_FF && pApp->GetFeatureStatus(MS60_VISION_5M_SORTING_FF_MODE) )
	{
		stLrnDieCmd.emCoarseSrchResolution = PR_RESOLUTION_MEDIUM;	//	PR_RESOLUTION_HIGH;
		CString szMsg;
		szMsg.Format("WPR auto learn die, set emCoarseSrchResolution medium(2)high(3) to %d",
			stLrnDieCmd.emCoarseSrchResolution);
		SetAlarmLog(szMsg);
	}

	if (IsWaferRotated180())
	{
		stLrnDieCmd.rExpectedAngle = 180;
	}
	else
	{
		stLrnDieCmd.rExpectedAngle = 0;
	}

	PR_ManualLrnDieCmd(&stLrnDieCmd, ubSID, ubRID, &stLrnDieRpy1);

	if ((stLrnDieRpy1.uwCommunStatus == PR_COMM_NOERR) && (stLrnDieRpy1.uwPRStatus == PR_ERR_NOERR))
	{
		//Get Replay 2	
		PR_ManualLrnDieRpy(ubSID, &stLrnDieRpy2);

		if (stLrnDieRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR)
		{
			if ((stLrnDieRpy2.stStatus.uwPRStatus == PR_ERR_NOERR) || PR_WARN_STATUS(stLrnDieRpy2.stStatus.uwPRStatus))
			{
				//Get PR ID & Die size
				short siMinX = (short)GetPrCenterX(), siMaxX = (short)GetPrCenterX();
				short siMinY = (short)GetPrCenterY(), siMaxY = (short)GetPrCenterY();

				lDieNo = min(lDieNo, WPR_MAX_DIE-1);	//v4.51A20	//Klocwork

				for ( i=0; i<(short)stLrnDieRpy2.uwNumOfDieCorners; i++ )
				{
					siMinX = min(siMinX, stLrnDieRpy2.acoDieCorners[i].x);
					siMaxX = max(siMaxX, stLrnDieRpy2.acoDieCorners[i].x);

					siMinY = min(siMinY, stLrnDieRpy2.acoDieCorners[i].y);
					siMaxY = max(siMaxY, stLrnDieRpy2.acoDieCorners[i].y);
				}

				m_stGenDieSize[lDieNo].x	= siMaxX - siMinX;
				m_stGenDieSize[lDieNo].y	= siMaxY - siMinY;
				m_ssGenPRSrchID[lDieNo]		= stLrnDieRpy2.uwRecordID;
				m_bGenDieLearnt[lDieNo]		= TRUE;

				if(m_bLearnLookupCamera)
				{
					LONG lCalDieCentreX = (LONG)( siMaxX + siMinX )/2;
					LONG lCalDieCentreY = (LONG)( siMaxY + siMinY )/2;

					LONG lPRDieCentreX  = stLrnDieRpy2.coDieCentre.x;
					LONG lPRDieCentreY  = stLrnDieRpy2.coDieCentre.y;
					CString szTempMsg; 
					if(lRefDieNo == MS899_UPLOOK_REF_DIENO_BH1)
					{
						 //4.52D17cal (learn die centre)				
						(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn GenDieSize X"]		= (LONG)m_stGenDieSize[lDieNo].x;
						(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn GenDieSize Y"]		= (LONG)m_stGenDieSize[lDieNo].y;

						(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn CalDieCentre X"]	= lCalDieCentreX;
						(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn CalDieCentre Y"]	= lCalDieCentreY;

						(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre X"]		= lPRDieCentreX;
						(*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre Y"]		= lPRDieCentreY;


						m_lPrUplookBH1LearnCenterX = lPRDieCentreX;
						m_lPrUplookBH1LearnCenterY = lPRDieCentreY;

						szTempMsg.Format("Uplook --- Auto Learn Die,BH1 DieSize(%d,%d),CalDieCentre(%d,%d),PRDieCentre(%d,%d) ",(LONG)m_stGenDieSize[lDieNo].x, (LONG)m_stGenDieSize[lDieNo].y,lCalDieCentreX, lCalDieCentreY, lPRDieCentreX,lPRDieCentreY);

					}
					else if(lRefDieNo == MS899_UPLOOK_REF_DIENO_BH2)
					{
						(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn GenDieSize X"]		= (LONG)m_stGenDieSize[lDieNo].x;
						(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn GenDieSize Y"]		= (LONG)m_stGenDieSize[lDieNo].y;

						(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn CalDieCentre X"]	= lCalDieCentreX;
						(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn CalDieCentre Y"]	= lCalDieCentreY;

						(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre X"]		= lPRDieCentreX;
						(*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre Y"]		= lPRDieCentreY;

						m_lPrUplookBH2LearnCenterX = lPRDieCentreX;
						m_lPrUplookBH2LearnCenterY = lPRDieCentreY;

						szTempMsg.Format("Uplook --- Auto Learn Die,BH2 DieSize(%d,%d),CalDieCentre(%d,%d),PRDieCentre(%d,%d) ",(LONG)m_stGenDieSize[lDieNo].x, (LONG)m_stGenDieSize[lDieNo].y,lCalDieCentreX, lCalDieCentreY, lPRDieCentreX,lPRDieCentreY);
					}
					else
					{	
						szTempMsg.Format("Uplook --- Auto Learn Die Fail with RefDieNo%d", lRefDieNo);
						HmiMessage(szTempMsg);

					}
					
					CMSLogFileUtility::Instance()->MS_LogOperation(szTempMsg);
				}

				if (IsOcrAOIMode() && IsCharDieInUse() && GetDiePrID((UCHAR)lDieNo) != (lDieNo + 1))
				{
					PR_UWORD uwOldPrID	= GetDiePrID((UCHAR)lDieNo);
					PR_UWORD uwNewPrID	= (PR_UWORD)(lDieNo + 1);
					CString szMsg;
					PR_COPY_RECORD_CMD                    stCmd;
					PR_COPY_RECORD_RPY                     stRpy;
					PR_InitCopyRecordCmd(&stCmd);
					stCmd.uwRecordID		= uwOldPrID;
					stCmd.uwOutputRecordID	= uwNewPrID;
					stCmd.emIsOverwrite		= PR_TRUE;
					PR_CopyRecordCmd(&stCmd, ubSID, ubRID, &stRpy);
					szMsg.Format("PR_CopyRecordCmd() old %d, target %d; done with %u, %u! new %u",
						uwOldPrID, uwNewPrID, stRpy.stStatus.uwCommunStatus, stRpy.stStatus.uwPRStatus, stRpy.uwRecordID);
					SetAlarmLog(szMsg);
					if( stRpy.stStatus.uwCommunStatus==PR_COMM_NOERR && stRpy.stStatus.uwPRStatus==PR_ERR_NOERR )
					{
						lDieNo = min(lDieNo, WPR_MAX_DIE-1);	//v4.53	Klocwork
						m_ssGenPRSrchID[lDieNo]	= stRpy.uwRecordID;
						m_pPrGeneral->FreePRRecord(uwOldPrID, GetRunSenID(), GetRunRecID());
						szMsg.Format("PR free record %d", uwOldPrID);
						SetAlarmLog(szMsg);
					}
				}
				
				//v4.49A10
				LogLearnDieRecord(lDieNo, GetRunCamID(), ubPpsI, 
									GetDieShape(), stLrnDieRpy2.stStatus.uwPRStatus, 
									ubSID, ubRID);
//v4.49A7
CString szLog;
szLog.Format("WPR: AutoLearnDie OK - DieNo=%ld, RecordID=%lu", lDieNo, GetDiePrID((UCHAR)lDieNo));
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
CheckWaferIDWithBondIDs(m_ssGenPRSrchID[lDieNo]);

				CloneCoordinate(stLrnDieRpy2.acoDieCorners,m_stDieCoordinate,stLrnDieRpy2.uwNumOfDieCorners);

				switch(stLrnDieRpy2.stStatus.uwPRStatus)
				{
					case PR_WARN_TOO_DARK:
					case PR_WARN_TOO_BRIGHT:
					case PR_WARN_ROTATION_INVARIANT:
						lLearnDieStauts = stLrnDieRpy2.stStatus.uwPRStatus;
						break;
					default:
						lLearnDieStauts = PR_ERR_NOERR;
						break;
				}
			}
			else
			{
				m_bGenDieLearnt[lDieNo] = FALSE;
				lLearnDieStauts = stLrnDieRpy2.stStatus.uwPRStatus;
			}
		}
		else
		{
			lLearnDieStauts = PR_COMM_TIMEOUT;
		}
	}
	else
	{
		lLearnDieStauts = PR_COMM_ERR;
	}

	//Update total reference die learnt
	if (lDieNo > 0)
	{
		m_lLrnTotalRefDie = 0;
		for (i=WPR_GEN_RDIE_OFFSET+1; i<WPR_MAX_DIE; i++)
		{
			if (IsThisDieLearnt((UCHAR)i) == TRUE)
			{
				m_lLrnTotalRefDie++;
			}
		}
	}	

	//Reset Inspection & Alignment detect & ignore win no
	m_lInspDetectWinNo = 0;
	m_lInspIgnoreWinNo = 0;
	m_lAlnDetectWinNo = 0;
	m_lAlnIgnoreWinNo = 0;

	return lLearnDieStauts;
}

PR_UWORD CWaferPr::AutoLearnCircle(PR_COORD stULCorner, PR_COORD stLRCorner)
{
	PR_LRN_SHAPE_CMD pstCmd;
	PR_LRN_SHAPE_RPY1 pstRpy1;
	PR_LRN_SHAPE_RPY2 pstRpy2;
	PR_PURPOSE ubPpsI = GetRunPurposeI();

	PR_InitLrnShapeCmd(&pstCmd);
	pstCmd.emSameView = PR_FALSE;							// Use previous grab image
	pstCmd.emPurpose = ubPpsI;				// PR Logical Optics
//	pstCmd.coRefPoint;										// Reference Point
//	pstCmd.rRefAngle;										// Reference Angle
	pstCmd.emIsAligned = PR_FALSE;							// Is the object aligned?
	pstCmd.emShapeType = PR_SHAPE_TYPE_CIRCLE;				// Shape type (e.g: Circle, Rectangle ...etc)
	pstCmd.stEncRect.coCorner1 = stULCorner;				// Enclosing/Inscribing Rectangle (Used for Circle Extraction only)
	pstCmd.stEncRect.coCorner2 = stLRCorner;				// Enclosing/Inscribing Rectangle (Used for Circle Extraction only)
	pstCmd.emRectMode = PR_RECT_MODE_ENCLOSE;				// Is the Rectangle Enclosing or Inscribing?
	pstCmd.emDetectAlg = PR_SHAPE_DETECT_ALG_EDGE_PTS;		// Detection Algorithm
//	pstCmd.emAccuracy;			// Accuracy
	pstCmd.emObjAttribute = PR_OBJ_ATTRIBUTE_BRIGHT;		// Object Attribute (Bright\Dark\Both)
//	pstCmd.ubThreshold;			// Threshold
//	pstCmd.wEdgeMag;			// Edge Magnitude
//	pstCmd.uwRoughness;			// Roughness of the shape
	pstCmd.emIsSaveRecord = PR_TRUE;		// Save Record (Yes: Lrn & Srch/ No: Extract once only)
	pstCmd.uwMaskID = 0;					// Which Mask will be used? (0: Not use any Mask)
	pstCmd.rPassScore = 70;					// Pass Score for extraction
	pstCmd.uwNumOfLine;			// No of Line of the shape (Used for Rect, Square, Right Angle Corner & Line)
//	pstCmd.astLineCoord[PR_MAX_NO_OF_SHAPE_LINE];	// Line coord.
	pstCmd.emCentreDefinition = PR_CENTRE_DEFINITION_BY_CORNER;	// Definition of Centre of the shape (By Corner or By edge point)
//	pstCmd.uwNConfirmWin;
//	pstCmd.astConfirmWin[PR_MAX_NO_OF_SHA_WIN];	// SHA Windows (Confirm/Pos Emp/Neg Emp/Blob)
	pstCmd.emExtMethod = PR_EXT_CORNER;			// Extraction Method (Line/Corner Extraction)
	pstCmd.uwNIgnoreWin = 0;		// no. of ignore windows 
//	pstCmd.astIgnoreWin[PR_MAX_DETECT_IGNORE_WIN];
	pstCmd.emObjectType = PR_OBJ_TYPE_LED_PACKAGE;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	/////////
	PR_LrnShapeCmd(&pstCmd, ubSID, ubRID, &pstRpy1);
	if ( (pstRpy1.uwCommunStatus != PR_COMM_NOERR) || (pstRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("PR_LrnShapeCmd Error1: COMM = 0x%x, PR = 0x%x", pstRpy1.uwCommunStatus, pstRpy1.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}
    PR_LrnShapeRpy(ubSID, &pstRpy2);
	
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
	m_ssGenPRSrchID[WPR_GEN_RDIE_OFFSET+3]          = pstRpy2.uwRecordID;
	m_bGenDieLearnt[WPR_GEN_RDIE_OFFSET+3]          = TRUE;

	m_pPrGeneral->DrawSmallCursor(ubSID, ubRID, pstRpy2.stCircleRes.coCentre.x, pstRpy2.stCircleRes.coCentre.y);

	HmiMessageEx("Learn collet hole success !!!!!!!!");
	//Get PR ID & Die size
	short siMinX = (short)GetPrCenterX(), siMaxX = (short)GetPrCenterX();
	short siMinY = (short)GetPrCenterY(), siMaxY = (short)GetPrCenterY();
	siMinX = siMinX - (short)pstRpy2.stCircleRes.rRadius;
	siMaxX = siMaxX + (short)pstRpy2.stCircleRes.rRadius;
	siMinY = siMinY - (short)pstRpy2.stCircleRes.rRadius;
	siMaxY = siMinY + (short)pstRpy2.stCircleRes.rRadius;
	m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].x         = siMaxX - siMinX;
	m_stGenDieSize[WPR_GEN_RDIE_OFFSET+3].y         = siMaxY - siMinY;

	return PR_TRUE;
}



BOOL CWaferPr::DisplayLearnDieResult(PR_UWORD lLearnStatus)
{
	CString		csMsgA;
	CString		csMsgB;
	ULONG		ulMsgCode;

	switch(lLearnStatus)
	{
		case PR_WARN_TOO_DARK:
			//csMsgA.Format("Die Too Dark");
			//csMsgB.Format("Learn Die Warning");
			ulMsgCode = IDS_WPR_DIETOODARK;
			break;

		case PR_WARN_TOO_BRIGHT:
			//csMsgA.Format("Die Too Bright");
			//csMsgB.Format("Learn Die Warning");
			ulMsgCode = IDS_WPR_DIETOOBRIGHT;
			break;

		case PR_WARN_ROTATION_INVARIANT:
			//csMsgA.Format("Die Rotation Invariant");
			//csMsgB.Format("Learn Die Warning");
			ulMsgCode = IDS_WPR_DIEROTATIONINVARIANT;
			break;

		case PR_ERR_NOERR:
			csMsgA.LoadString(HMB_WPR_LRN_DIE_OK);
			csMsgB.LoadString(HMB_WPR_LRN_DIE);
			HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			return TRUE;
//			ulMsgCode = IDS_WPR_LEARNDIEOK;
//			break;

		case PR_COMM_TIMEOUT:
			//csMsgA.Format("Get Die Reply 2 Error");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_WPR_GETDIEREPLY2ERROR;
			break;

		case PR_COMM_ERR:
			//csMsgA.Format("Get Die Reply 1 Error");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_WPR_GETDIEREPLY1ERROR;
			break;

		case PR_ERR_ROTATION:
			//csMsgA.Format("Die Angle too large");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_WPR_DIEANGLETOOLARGE;
			break;

		case PR_ERR_NO_DIE:
		case PR_ERR_NO_OBJECT_FOUND:
			//csMsgA.Format("No Die Found");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_WPR_NODIEFOUND;
			break;

		case PR_ERR_DIE_SIZE_RJT:
			//csMsgA.Format("Invalid Die Size");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_WPR_INVALIDDIESIZE;
			break;
		
		case WPR_ERR_LRN_CHAR_DIE_OUT_LIMIT:
			//csMsgA.Format("Invalid Die No Selected");
			//csMsgB.Format("Learn Character Die Error");
			//HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			csMsgA.LoadString(HMB_WPR_INVALID_DIE_NO);
			SetAlert_Msg(IDS_WPR_LRN_DIE_FAILED, csMsgA); 
			return FALSE;

		default:
			//csMsgA.Format("Unknown Die Error = %x",lLearnStatus);
			//csMsgB.Format("Learn Die Error");
			//HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			//break;
			csMsgA.Format("Status = %x",lLearnStatus);
			SetAlert_Msg(IDS_WPR_LRN_DIE_FAILED, csMsgA); 
			return FALSE;
	}

	//HmiMessage(csMsgA,csMsgB,2,1);
	//HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);

	SetAlert(ulMsgCode);

	return FALSE;
}


PR_UWORD CWaferPr::AutoLearnCharDie(LONG lDigitNo)
{
	PR_LRN_CHAR_CMD		stLrnCharCmd;
	PR_LRN_CHAR_RPY		stLrnCharRpy;
	PR_UWORD 			lLearnDieStauts;	
	short				i = 0;	
	PR_PURPOSE ubPpsI = GetRunPurposeI();


	if ( (lDigitNo < 0) || (lDigitNo >= WPR_GEN_CDIE_MAX_DIE) )
	{
		return WPR_ERR_LRN_CHAR_DIE_OUT_LIMIT;
	}

	PR_InitLrnCharCmd(&stLrnCharCmd);
	
	stLrnCharCmd.emPurpose					= ubPpsI;
	stLrnCharCmd.stCharWin.coCorner1		= m_stLearnDieCornerPos[PR_UPPER_LEFT];
	stLrnCharCmd.stCharWin.coCorner2		= m_stLearnDieCornerPos[PR_LOWER_RIGHT];
	stLrnCharCmd.emRgnCharAlg				= PR_RGN_CHAR_ALG_ALIGN;
	stLrnCharCmd.stIgnoreWin.uwNDetectWin	= 0;
	stLrnCharCmd.stIgnoreWin.uwNIgnoreWin	= 0;
	stLrnCharCmd.aubString[0]				= (PR_UBYTE)(lDigitNo + WPR_GEN_CDIE_START);
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	PR_LrnCharCmd(&stLrnCharCmd, ubSID, ubRID, &stLrnCharRpy);

	if (stLrnCharRpy.stStatus.uwCommunStatus == PR_COMM_NOERR)
	{
		if ( (stLrnCharRpy.stStatus.uwPRStatus == PR_ERR_NOERR) || PR_WARN_STATUS(stLrnCharRpy.stStatus.uwPRStatus) )
		{
			m_stGenDieSize[lDigitNo + WPR_GEN_CDIE_OFFSET].x	= (m_stLearnDieCornerPos[PR_LOWER_RIGHT].x - m_stLearnDieCornerPos[PR_UPPER_LEFT].x);
			m_stGenDieSize[lDigitNo + WPR_GEN_CDIE_OFFSET].y	= (m_stLearnDieCornerPos[PR_LOWER_RIGHT].y - m_stLearnDieCornerPos[PR_UPPER_LEFT].y);
			m_ssGenPRSrchID[lDigitNo + WPR_GEN_CDIE_OFFSET]		= stLrnCharRpy.stCharInfo.uwRecordID;
			m_bGenDieLearnt[lDigitNo + WPR_GEN_CDIE_OFFSET]		= TRUE;
			lLearnDieStauts = PR_ERR_NOERR;

			switch (lDigitNo)
			{
				case 0: m_bLrnDigit0 = TRUE; break;
				case 1: m_bLrnDigit1 = TRUE; break;
				case 2: m_bLrnDigit2 = TRUE; break;
				case 3: m_bLrnDigit3 = TRUE; break;
				case 4: m_bLrnDigit4 = TRUE; break;
				case 5: m_bLrnDigit5 = TRUE; break;
				case 6: m_bLrnDigit6 = TRUE; break;
				case 7: m_bLrnDigit7 = TRUE; break;
				case 8: m_bLrnDigit8 = TRUE; break;
				case 9: m_bLrnDigit9 = TRUE; break;
				default:
					;
			}
		}
		else
		{
			lLearnDieStauts = stLrnCharRpy.stStatus.uwPRStatus;
		}
	}
	else
	{
		lLearnDieStauts = PR_COMM_ERR;
	}


	//Update total reference die learnt
	m_lLrnTotalRefDie = 0;
	for (i=WPR_GEN_RDIE_OFFSET+1; i<WPR_MAX_DIE; i++)
	{
		if (IsThisDieLearnt((UCHAR)i) == TRUE)
		{
			m_lLrnTotalRefDie++;
		}
	}

	return lLearnDieStauts;
}

PR_UWORD CWaferPr::AutoLearnOCRDie(LONG lDigitNo)
{
	/*
	PR_UWORD 			lLearnDieStauts=0;	
	ULONG				i = 0;

	if ( (lDigitNo < 0) || (lDigitNo >= WPR_GEN_CDIE_MAX_DIE) )
	{
		return WPR_ERR_LRN_CHAR_DIE_OUT_LIMIT;
	}

	PR_LRN_CHAR_EX_CMD		stLrnCharExCmd;
	PR_LRN_CHAR_EX_RPY		stLrnCharExRpy;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();
	PR_InitLrnCharExCmd(&stLrnCharExCmd);
	stLrnCharExCmd.emPurpose = ubPpsI;
	stLrnCharExCmd.rAngle = (PR_REAL)(90.0*m_ulOcrDieOrientation);          // Orientation of char
	stLrnCharExCmd.ubChar = (PR_UBYTE)(lDigitNo + WPR_GEN_CDIE_START);
	stLrnCharExCmd.stDetectIgnoreRegion.uwNDetectRegion = 1;
	stLrnCharExCmd.stDetectIgnoreRegion.astDetectRegion[0].uwNumOfCorners = 4;
	stLrnCharExCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].x = m_stLearnDieCornerPos[PR_LOWER_RIGHT].x;
	stLrnCharExCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[0].y = m_stLearnDieCornerPos[PR_UPPER_LEFT].y;
	stLrnCharExCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[1] = m_stLearnDieCornerPos[PR_UPPER_LEFT];
	stLrnCharExCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].x = m_stLearnDieCornerPos[PR_UPPER_LEFT].x;
	stLrnCharExCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[2].y = m_stLearnDieCornerPos[PR_LOWER_RIGHT].y;
	stLrnCharExCmd.stDetectIgnoreRegion.astDetectRegion[0].acoCorners[3] = m_stLearnDieCornerPos[PR_LOWER_RIGHT];
	PR_LrnCharExCmd( &stLrnCharExCmd, ubSID, ubRID, &stLrnCharExRpy );

	if (stLrnCharExRpy.stStatus.uwCommunStatus == PR_COMM_NOERR)
	{
		if ( (stLrnCharExRpy.stStatus.uwPRStatus == PR_ERR_NOERR) || PR_WARN_STATUS(stLrnCharExRpy.stStatus.uwPRStatus) )
		{
			ULONG TotalRecID;
			TotalRecID = stLrnCharExRpy.uwNumOfRecord;
			for (i=0;i<TotalRecID;i++)
			{
				if ((i + WPR_GEN_CDIE_OFFSET) < WPR_MAX_DIE)
				{
					m_stGenDieSize[i + WPR_GEN_CDIE_OFFSET].x	= (m_stLearnDieCornerPos[PR_LOWER_RIGHT].x - m_stLearnDieCornerPos[PR_UPPER_LEFT].x);
					m_stGenDieSize[i + WPR_GEN_CDIE_OFFSET].y	= (m_stLearnDieCornerPos[PR_LOWER_RIGHT].y - m_stLearnDieCornerPos[PR_UPPER_LEFT].y);
					if( i>=20 )
						m_ssGenPRSrchID[i + WPR_GEN_CDIE_OFFSET]	= 0;
					else
						m_ssGenPRSrchID[i + WPR_GEN_CDIE_OFFSET]	= stLrnCharExRpy.astCharInfo[i].uwRecordID;
					m_bGenDieLearnt[i + WPR_GEN_CDIE_OFFSET]	= TRUE;
				}

				lLearnDieStauts = PR_ERR_NOERR;

				switch (i)
				{
					case 0: m_bLrnDigit0 = TRUE; break;
					case 1: m_bLrnDigit1 = TRUE; break;
					case 2: m_bLrnDigit2 = TRUE; break;
					case 3: m_bLrnDigit3 = TRUE; break;
					case 4: m_bLrnDigit4 = TRUE; break;
					case 5: m_bLrnDigit5 = TRUE; break;
					case 6: m_bLrnDigit6 = TRUE; break;
					case 7: m_bLrnDigit7 = TRUE; break;
					case 8: m_bLrnDigit8 = TRUE; break;
					case 9: m_bLrnDigit9 = TRUE; break;
					default:
						;
				}
			}
		}
		else
		{
			lLearnDieStauts = stLrnCharExRpy.stStatus.uwPRStatus;
CString szTemp;
szTemp.Format("lLearnDieStauts=%d", lLearnDieStauts);
AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		}
	}
	else
	{
		lLearnDieStauts = PR_COMM_ERR;
	}

	//Update total reference die learnt
	m_lLrnTotalRefDie = 0;
	for (i= WPR_GEN_RDIE_OFFSET + 1; i<WPR_MAX_DIE; i++)
	{
		if (IsThisDieLearnt((UCHAR)i) == TRUE)
		{
			m_lLrnTotalRefDie++;
		}
	}

	return lLearnDieStauts;*/
	return TRUE;
}

BOOL CWaferPr::AutoLearnRefDie(VOID)
{
	PR_COMMON_RPY		stRpy;
	PR_OPTIC			stOptic;
	PR_UWORD			usLrnDieCLevel;
	PR_UWORD			usLrnDieRLevel;
	PR_UWORD			usLrnDieSLevel;
	PR_UWORD			usLrnDieBLevel;
	PR_UWORD			lLearnStatus;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_PURPOSE ubPpsI = GetRunPurposeI();
	//Set Learn-Die Lighting
	PR_GetOptic(ubPpsI, ubSID, ubRID, &stOptic, &stRpy);
	PR_GetLighting(&stOptic, PR_COAXIAL_LIGHT, ubSID, ubRID, &usLrnDieCLevel, &stRpy);
	PR_GetLighting(&stOptic, PR_RING_LIGHT, ubSID, ubRID, &usLrnDieRLevel, &stRpy);
	PR_GetLighting(&stOptic, PR_SIDE_LIGHT, ubSID, ubRID, &usLrnDieSLevel, &stRpy);
	PR_GetLighting(&stOptic, PR_BACK_LIGHT, ubSID, ubRID, &usLrnDieBLevel, &stRpy);
	PR_SetLighting(PR_COAXIAL_LIGHT, usLrnDieCLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_RING_LIGHT, usLrnDieRLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_SIDE_LIGHT, usLrnDieSLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(PR_BACK_LIGHT, usLrnDieBLevel, ubSID, ubRID, &stOptic, &stRpy);

	if (AutoDetectDieCorner(WPR_REFERENCE_DIE, 1) != PR_ERR_NOERR)
	{
		SetErrorMessage("WPR: Auto detect ref die corner fails!");
		return FALSE;
	}

	//Start Learn Die & display result
	lLearnStatus = AutoLearnDie(WPR_REFERENCE_DIE, 1);
	if (lLearnStatus != PR_ERR_NOERR)
	{
		CString szTemp;
		szTemp.Format("WPR: Auto learn ref die fails; status = %d", lLearnStatus);
		SetErrorMessage(szTemp);
		return FALSE;
	}

	//Clear Camera & Restore General Ligthting
	//ChangeCamera(WPR_CAM_WAFER);
	SavePrData(TRUE);
	return TRUE;
}


BOOL CWaferPr::CheckWaferIDWithBondIDs(CONST LONG lWaferID)
{
	LONG lBondID = 0;

	for (INT i=0; i<10; i++)
	{
		lBondID = (LONG)(*m_psmfSRam)["BondPr"]["Die Record"][i];
		if (lBondID == 0)
			continue;
		
		if (lWaferID == lBondID)
		{
			CString szError;
			szError.Format("Wafer PR record ID checking fails: Wafer ID = %ld, BOND ID = %ld (%d)",
								lWaferID, lBondID, i);
			SetErrorMessage(szError);
			HmiMessage_Red_Yellow(szError, "Wafer PR");
			HmiMessage("Please clear and re-learn all WAFER & BOND PR records.");
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CWaferPr::AutoLearnEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner)
{
	PR_COORD prcenter;
	prcenter.x = (PR_WORD)GetPrCenterX();
	prcenter.y = (PR_WORD)GetPrCenterY();
	PR_UBYTE ubSID = GetRunSenID();	
	PR_UBYTE ubRID = GetRunRecID();

	LONG lThreshold			=  126;
	PR_PRE_EPOXY_SHOW_CMD	stEpoxyShowCmd;


	CString szContent = "Is this image OK?";
	CString szTitle   = "WPR Show Epoxy";

	CString szErrMsg;

	do 
    {
		if (!m_pPrGeneral->PreEpoxyShow(stULCorner, stLRCorner, prcenter, ubSID, ubRID, MS899_WAF_INSP_PURPOSE, lThreshold, stEpoxyShowCmd, szErrMsg))
		{
			SetErrorMessage(szErrMsg);
			HmiMessageEx(szErrMsg);
			return FALSE;
		}

	} while (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_NO );

	PR_UWORD uwRecordID = 0;
	PR_AREA aeEpoxyArea = 0;
	LONG lPRIndex = WPR_GEN_RDIE_OFFSET + WPR_REFERENCE_PR_DIE_INDEX4;
	if (!m_pPrGeneral->LearnEpoxyExt(prcenter, stEpoxyShowCmd, ubSID, ubRID, uwRecordID, aeEpoxyArea, szErrMsg))
	{
		m_ssGenPRSrchID[lPRIndex]		= 0;
		m_bGenDieLearnt[lPRIndex]		= FALSE;
		m_stGenDieSize[lPRIndex].x		= 0;
		m_stGenDieSize[lPRIndex].y		= 0;
		m_lGenSrchDieScore[lPRIndex]	= 0;

		SetErrorMessage(szErrMsg);
		HmiMessageEx(szErrMsg);
		return FALSE;
	}

	m_ssGenPRSrchID[lPRIndex]		= uwRecordID;
	m_bGenDieLearnt[lPRIndex]		= TRUE;
	m_stGenDieSize[lPRIndex].x		= (PR_WORD)aeEpoxyArea;
	m_stGenDieSize[lPRIndex].y		= (PR_WORD)aeEpoxyArea;
	m_lGenSrchDieScore[lPRIndex]	= (LONG)aeEpoxyArea;

	CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);

	return TRUE;
}


BOOL CWaferPr::AutoSearchEpoxy(BOOL bAuto, BOOL bLatch, BOOL bDisplayResult,
							   LONG pcx, LONG pcy, UINT &nX, UINT &nY,
							   ULONG &ulEpoxyAreaSize, CEvent *pevGrabImageReady,CString &szErrMsg)
{
	nX = nY = 0;
	ulEpoxyAreaSize = 0;
	LONG lPRIndex = WPR_GEN_RDIE_OFFSET + WPR_REFERENCE_PR_DIE_INDEX4;
	if (!m_bGenDieLearnt[lPRIndex])	//Use normal Die #2 for this eproxy Search
	{
		HmiMessage_Red_Back("Collet Hole pattern is not learnt !!", "Wafer Optics Collet Hole Search");
		return FALSE;
	}
/*
	if (m_dMaxEpoxySizeFactor < 1)
	{
		return TRUE;
	}
*/
	PR_COORD prcenter;
	prcenter.x = (PR_WORD)pcx;
	prcenter.y = (PR_WORD)pcy;
	if ( (pcx == 0) || (pcy == 0) )
	{
		prcenter.x = (PR_WORD)GetPrCenterX();
		prcenter.y = (PR_WORD)GetPrCenterY();
	}

	LONG lPRID = m_ssGenPRSrchID[lPRIndex];
	DOUBLE dLearnEpoxySize	= m_lGenSrchDieScore[lPRIndex];
	DOUBLE dMinEpoxySize	= dLearnEpoxySize * 0.05;
	DOUBLE dMaxEpoxySize	= _round(10 * dLearnEpoxySize); //_round(m_dMaxEpoxySizeFactor * dLearnEpoxySize);
	PR_UBYTE ubSID = GetRunSenID();	
	PR_UBYTE ubRID = GetRunRecID();

	PR_WORD wRet = m_pPrGeneral->SearchEpoxyExt(lPRID, dMinEpoxySize, dMaxEpoxySize, prcenter, TRUE, TRUE, TRUE,
												bAuto ? FALSE : TRUE, bLatch, ubSID, ubRID,
												nX, nY, ulEpoxyAreaSize, pevGrabImageReady, szErrMsg);
	if (wRet == PR_FALSE)
	{
		szErrMsg = "WPR  " + szErrMsg;
		SetErrorMessage(szErrMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);
		HmiMessage_Red_Yellow(szErrMsg);
		return FALSE;
	}
	else if (wRet == 2)
	{
		CString szMsg;
		szMsg.Format("WPR Search Eproxy Cmd fails - EPOXY size = %lu (learned = %.2f, MAX = %.2f (Factor=%.2f)",
						ulEpoxyAreaSize, dLearnEpoxySize, dMaxEpoxySize, m_dMaxEpoxySizeFactor);
		SetErrorMessage(szErrMsg);
		szErrMsg = szMsg;
		CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);
		if ((State() != AUTO_Q)	&& (State() != DEMO_Q))
		{
			//HmiMessage_Red_Back(szMsg, "Wafer PR Search Epoxy");
			SetAlert_Msg_Red_Yellow(IDS_WPR_ERR_COLLETEPOXY,szMsg);
			SetAlarmLamp_Green(FALSE, TRUE);
		}
		return FALSE;
	}
	else
	{	
		CString szMsg;
		szMsg.Format("WPR Search Eproxy Cmd found at (%ld, %ld); LATCH = %d, EPOXY size = %lu (learned = %.2f) (%.2f)", 
					  nX, nY, bLatch, ulEpoxyAreaSize, dLearnEpoxySize, ulEpoxyAreaSize);
		SetErrorMessage(szErrMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);
		if (bDisplayResult)
		{
			//HmiMessage(szMsg, "Wafer PR Search Epoxy");
			SetAlert_Msg_Red_Yellow(IDS_WPR_ERR_COLLETEPOXY,szMsg);
		}
	}

	return TRUE;
}