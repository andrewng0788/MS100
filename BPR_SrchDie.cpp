/////////////////////////////////////////////////////////////////
// BPR_SrchDie.cpp : Search Die Function of the CBondPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wednesday, December 1, 2004
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
#include "MS896A_Constant.h"
#include "BondPr.h"
#include "BPR_Constant.h"
#include "Spc_CDataAccessWrapper.h"
#include "Resource.h"
#include "BondHead.h"
#include "BinLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CBondPr::DieIsAlignable(PR_UWORD usDieType)
{
    if ((usDieType == PR_ERR_GOOD_DIE)						||
		(usDieType == PR_ERR_INK_DIE)						||
        (usDieType == PR_ERR_CHIP_DIE)						||
        (usDieType == PR_ERR_BOND_PAD_SIZE_RJT)				||
        (usDieType == PR_ERR_BOND_PAD_FOREIGN_MAT)			||
        (usDieType == PR_ERR_LIGHT_EMITTING_FOREIGN_MAT)	||
        (usDieType == PR_ERR_DEFECTIVE_DIE)					||
        (usDieType == PR_ERR_EXCEED_INTENSITY_VAR)			||
        (usDieType == PR_WARN_SMALL_SRCH_WIN)				||
        (usDieType == PR_WARN_TOO_MANY_DIE)					||
        (usDieType == PR_WARN_BACKUP_ALIGNED)				||
        (usDieType == PR_ERR_NOERR)							||
   //     (usDieType == PR_ERR_ROTATION)						||
        (usDieType == PR_ERR_INSUFF_EPOXY_COVERAGE)			||
		PR_WARN_STATUS(usDieType))
	{
        return TRUE;
    }
    return FALSE;
}


BOOL CBondPr::DieIsGood(PR_UWORD usDieType)
{
	if (usDieType == PR_ERR_GOOD_DIE)
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CBondPr::DieIsEmpty(PR_UWORD usDieType)
{
	if (DieIsAlignable(usDieType) == FALSE)
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CBondPr::DieIsDefective(PR_UWORD usDieType)
{
	if ((usDieType == PR_ERR_SCRATCH_DIE)			||
		(usDieType == PR_ERR_CHIP_DIE)				||
		(usDieType == PR_ERR_DEFECTIVE_DIE)			||
		(usDieType == PR_ERR_RJT_INT)				||
		(usDieType == PR_ERR_RJT_PAT)				||
		(usDieType == PR_ERR_DIE_SIZE_RJT)			||
		(usDieType == PR_ERR_INSUFF_EPOXY_COVERAGE)	||
		(usDieType == PR_ERR_INK_DIE)				||
		(usDieType == PR_ERR_PHOTORESIPR_ERR)		||
		(usDieType == PR_ERR_PROBE_MARK_SIZE_RJT)	||
		(usDieType == PR_ERR_BOND_PAD_SIZE_RJT)		||
		(usDieType == PR_ERR_BOND_PAD_OFFSET)		||
		(usDieType == PR_ERR_BOND_PAD_FOREIGN_MAT)	||
		(usDieType == PR_ERR_LIGHT_EMITTING_FOREIGN_MAT))
	{
		return TRUE;
	}

	return FALSE;
}

VOID CBondPr::UpdateSearchDieArea(DOUBLE dSearchCodeX, DOUBLE dSearchCodeY, BOOL bUpdate, BOOL bRedraw)
{
	BOOL	bSeperateXY = TRUE;
	LONG	lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];

	if (lInitPR != 1)
	{
		return;
	}

	//PR_DRAW_SRCH_AREA_CMD	stDrawAreaCmd;
	//PR_DRAW_SRCH_AREA_RPY	stDrawAreaRpy;
	PR_WIN					stDispSearchArea;

	//v4.04		//Klocwork
	LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
		
	//Use same calculation as WPR
	stDispSearchArea.coCorner1.x = (PR_WORD)m_lPrCenterX - (PR_WORD)((dSearchCodeX / 4 + 0.5) * m_stGenDieSize[lDieNo].x * BPR_SEARCHAREA_SHIFT_FACTOR );
	stDispSearchArea.coCorner1.y = (PR_WORD)m_lPrCenterY - (PR_WORD)((dSearchCodeY / 4 + 0.5) * m_stGenDieSize[lDieNo].y * BPR_SEARCHAREA_SHIFT_FACTOR );
	stDispSearchArea.coCorner2.x = (PR_WORD)m_lPrCenterX + (PR_WORD)((dSearchCodeX / 4 + 0.5) * m_stGenDieSize[lDieNo].x * BPR_SEARCHAREA_SHIFT_FACTOR );
	stDispSearchArea.coCorner2.y = (PR_WORD)m_lPrCenterY + (PR_WORD)((dSearchCodeY / 4 + 0.5) * m_stGenDieSize[lDieNo].y * BPR_SEARCHAREA_SHIFT_FACTOR );

	VerifyPRRegion(&stDispSearchArea);

	if (bRedraw)
	{
		//Display search area on PR 
		SelectBondCamera();

		DrawRectangleBox(stDispSearchArea.coCorner1, stDispSearchArea.coCorner2, PR_COLOR_YELLOW);
	}

	if (bUpdate == TRUE)
	{
		if (IsSensorZoomFFMode())
		{
			(*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["ULX"]	= (LONG) stDispSearchArea.coCorner1.x;
			(*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["ULY"]	= (LONG) stDispSearchArea.coCorner1.y;
			(*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["LRX"]	= (LONG) stDispSearchArea.coCorner2.x;
			(*m_psmfSRam)["BondPr"]["FFModeSearchArea"]["LRY"]	= (LONG) stDispSearchArea.coCorner2.y;
		}
		else
		{
			m_stSearchArea = stDispSearchArea;

			(*m_psmfSRam)["BondPr"]["SearchArea"]["ULX"]	= (LONG) m_stSearchArea.coCorner1.x;
			(*m_psmfSRam)["BondPr"]["SearchArea"]["ULY"]	= (LONG) m_stSearchArea.coCorner1.y;
			(*m_psmfSRam)["BondPr"]["SearchArea"]["LRX"]	= (LONG) m_stSearchArea.coCorner2.x;
			(*m_psmfSRam)["BondPr"]["SearchArea"]["LRY"]	= (LONG) m_stSearchArea.coCorner2.y;
		}
	}
}


PR_UWORD CBondPr::SearchDieCmd(BOOL bDieType, LONG lInputDieNo, PR_COORD stDieULC, PR_COORD stDieLRC, 
							   PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect,
							   UCHAR ucColletID, DOUBLE dRefAngle)
{
	LONG lDieNo = 0;
	LONG lPrSrchID = 0;
    PR_2_POINTS_DIE_ALIGN_CMD       st2PointsAlignCmd;
    PR_QUAD_DIE_ALIGN_CMD           stQuadAlignCmd;
    PR_STREET_DIE_ALIGN_CMD         stStreetAlignCmd;
    PR_DIE_ALIGN_PAR                stAlignPar;

	PR_TMPL_DIE_INSP_CMD            stTmplInspCmd;
    PR_LED_DIE_INSP_CMD             stLedInspCmd;
    PR_DIE_ALIGN_CMD                stDieAlignCmd;
	PR_DIE_INSP_CMD 				stDieInspCmd;
	PR_SRCH_DIE_CMD                 stSrchCmd; 

	PR_COORD						stCoDieCenter;
	PR_UWORD                        uwCommunStatus;
	PR_BOOLEAN						bCheckDefect = PR_TRUE;
	PR_BOOLEAN						bCheckChip = PR_TRUE;
	PR_UBYTE						ucDefectThres = PR_NORMAL_DEFECT_THRESHOLD;
	LONG							lAlignAccuracy;
	LONG							lGreyLevelDefect;
	DOUBLE							dMinChipArea = 0.0;
	DOUBLE							dSingleDefectArea = 0.0;
	DOUBLE							dTotalDefectArea = 0.0;


    PR_Init2PointsDieAlignCmd(&st2PointsAlignCmd);
    PR_InitQuadDieAlignCmd(&stQuadAlignCmd);
    PR_InitStreetDieAlignCmd(&stStreetAlignCmd);
    PR_InitTmplDieInspCmd(&stTmplInspCmd);
    PR_InitLedDieInspCmd(&stLedInspCmd);
    PR_InitDieAlignCmd(&stDieAlignCmd);
    PR_InitDieInspCmd(&stDieInspCmd);
	PR_InitDieAlignPar(&stAlignPar);
    PR_InitSrchDieCmd(&stSrchCmd); 

	if (lInputDieNo <= 0)
	{
		lInputDieNo = 1;
	}

	if (bDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	}
	

	if (bDieType == BPR_REFERENCE_DIE)
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	}

	lPrSrchID	= m_ssGenPRSrchID[lDieNo];

	if (lPrSrchID == 0)
	{
		return IDS_BPR_DIENOTLEARNT;
	}

	//CString szTemp;
	//szTemp.Format("ID: %d %d",lDieNo, m_ssGenPRSrchID[lDieNo]	);
	//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	switch(m_lGenSrchAlignRes[lDieNo])
	{
		case 1:		
			lAlignAccuracy	= PR_LOW_DIE_ALIGN_ACCURACY;	
			break;

		default:	
			lAlignAccuracy	= PR_HIGH_DIE_ALIGN_ACCURACY;	
			break;
	}


	switch(m_lGenSrchGreyLevelDefect[lDieNo])
	{
		case 1: 
			lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_BRIGHT;
			break;
		
		case 2:
			lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_DARK;
			break;

		default:
			lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_BOTH;
			break;
	}

	ucDefectThres = (PR_UBYTE)m_lGenSrchDefectThres[lDieNo];

	if ((m_dGenSrchSingleDefectArea[lDieNo] == 0.0) && (m_dGenSrchTotalDefectArea[lDieNo] == 0.0))
	{
		bCheckDefect = PR_FALSE;
	}
		
	if (m_dGenSrchChipArea[lDieNo] == 0.0)
	{
		bCheckChip = PR_FALSE;
	}

	dMinChipArea		= ((DOUBLE)(m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) / 100) * m_dGenSrchChipArea[lDieNo];
	dSingleDefectArea	= ((DOUBLE)(m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	dTotalDefectArea	= ((DOUBLE)(m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 

	stCoDieCenter.x = (stDieULC.x + stDieLRC.x) / 2;
	stCoDieCenter.y = (stDieULC.y + stDieLRC.y) / 2;

	//Setup Search Die parameter
	st2PointsAlignCmd.emConsistentDiePos		= PR_TRUE;
	st2PointsAlignCmd.emBackupTmpl				= PR_FALSE;      
	st2PointsAlignCmd.emSingleTmplSrch			= PR_FALSE;
	st2PointsAlignCmd.emSingleDieSrch			= PR_TRUE;
	st2PointsAlignCmd.emPartialDieSrch			= PR_FALSE;     
	st2PointsAlignCmd.emDieRotChk				= PR_TRUE;
	st2PointsAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emMinDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emDieEdgeChk				= PR_TRUE;
	st2PointsAlignCmd.emPositionChk				= PR_FALSE;
	st2PointsAlignCmd.coRefPoint.x				= 0;
	st2PointsAlignCmd.coRefPoint.y				= 0;
	st2PointsAlignCmd.rRefAngle					= 0;
	st2PointsAlignCmd.coMaxShift.x				= 0;
	st2PointsAlignCmd.coMaxShift.y				= 0;
	st2PointsAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	st2PointsAlignCmd.szPosConsistency.x		= PR_DEF_POS_CONSISTENCY_X + 10 * PR_SCALE_FACTOR;
	st2PointsAlignCmd.szPosConsistency.y		= PR_DEF_POS_CONSISTENCY_Y + 10 * PR_SCALE_FACTOR;
	st2PointsAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	st2PointsAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	st2PointsAlignCmd.coSelectPoint				= stCoDieCenter;
	st2PointsAlignCmd.coProbableDieCentre		= stCoDieCenter;
	st2PointsAlignCmd.rDieRotTol				= PR_SRCH_ROT_TOL;

	stStreetAlignCmd.emConsistentDiePos			= PR_TRUE;
	stStreetAlignCmd.emSingleDieSrch			= PR_TRUE;
	stStreetAlignCmd.emPartialDieSrch			= PR_FALSE;
	stStreetAlignCmd.emDieRotChk				= PR_TRUE;
	stStreetAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emMinDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emDieEdgeChk				= PR_TRUE;
	stStreetAlignCmd.emPositionChk				= PR_FALSE;
	stStreetAlignCmd.coRefPoint.x				= 0;
	stStreetAlignCmd.coRefPoint.y				= 0;
	stStreetAlignCmd.coMaxShift.x				= 0;
	stStreetAlignCmd.coMaxShift.y				= 0;
	stStreetAlignCmd.rRefAngle					= 0;
	stStreetAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	stStreetAlignCmd.szPosConsistency.x			= PR_DEF_POS_CONSISTENCY_X + 10 * PR_SCALE_FACTOR;
	stStreetAlignCmd.szPosConsistency.y			= PR_DEF_POS_CONSISTENCY_Y + 10 * PR_SCALE_FACTOR;
	stStreetAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	stStreetAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	stStreetAlignCmd.coSelectPoint				= stCoDieCenter;
	stStreetAlignCmd.coProbableDieCentre		= stCoDieCenter;
	stStreetAlignCmd.rDieRotTol					= PR_SRCH_ROT_TOL;

	//Vincent Mok Suggestion 2020/04/23
	stStreetAlignCmd.emSelectMode				= PR_SRCH_DIE_SELECT_MODE_HIGHEST_SCORE;

	stTmplInspCmd.emChipDieChk					= bCheckChip;
	stTmplInspCmd.emGeneralDefectChk			= bCheckDefect;
	stTmplInspCmd.emInkOnlyChk					= PR_FALSE;     
	stTmplInspCmd.emInspIncompleteDie			= PR_FALSE;     
	stTmplInspCmd.aeMinSingleDefectArea			= (PR_AREA)dSingleDefectArea;
	stTmplInspCmd.aeMinTotalDefectArea			= (PR_AREA)dTotalDefectArea;  
	stTmplInspCmd.aeMinChipArea					= (PR_AREA)dMinChipArea; 
	stTmplInspCmd.emNewDefectCriteria			= PR_TRUE;
	stTmplInspCmd.ubIntensityVariation			= PR_DEF_INTENSITY_VARIATION;
	stTmplInspCmd.ubDefectThreshold				= ucDefectThres; 
	stTmplInspCmd.rMinInkSize					= PR_FALSE;     
	stTmplInspCmd.emDefectAttribute				= (PR_DEFECT_ATTRIBUTE)(lGreyLevelDefect);


	if( m_bEnableTotalDefect )
	{
		stTmplInspCmd.emEnableBlobDefectDetection	= PR_TRUE;
		stTmplInspCmd.uwNumBlobDefects				= (PR_WORD)m_lMaxNumOfDefects;
	}
	//v3.25T2
	//Line Defect detection; only available for Normal-Die
	if (m_bEnableLineDefect)
	{
		stTmplInspCmd.emEnableLineDefectDetection		= PR_TRUE;
		stTmplInspCmd.uwNumLineDefects					= (PR_UWORD)m_lMinNumOfLineDefects;
		stTmplInspCmd.rMinLongestSingleLineDefectLength = (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength);		//mil -> pixel
		stTmplInspCmd.rMinLineClassLength				= (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength) / 2;
		stTmplInspCmd.rMinTotalLineDefectLength			= (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength) * 2;
	}

	stDieAlignCmd.st2Points						= st2PointsAlignCmd;
	stDieAlignCmd.stStreet						= stStreetAlignCmd;
	stDieInspCmd.stTmpl							= stTmplInspCmd;

	stAlignPar.emEnableBackupAlign				= PR_FALSE;
	stAlignPar.rStartAngle						= PR_SRCH_START_ANGLE;	
	stAlignPar.rEndAngle						= PR_SRCH_END_ANGLE;	

	//v4.59A17
	//m_b2Parts2ndPartStart is used instead of m_b2Parts1stPartDone because 
	// m_b2Parts1stPartDone may be set before 1st part sorting is done, cuasing 180
	// rotation searching of last 1/2 dies at BOND side;
	//if (IsMS90() && IsMS90Sorting2ndPart())		//sort mode		//v4.57A2
	BOOL bRotate180 = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"];
	if (IsMS90() && (bRotate180 /*|| CMS896AStn::m_b2Parts2ndPartStart*/))
	{
		stAlignPar.rStartAngle						= 180.0 + PR_SRCH_START_ANGLE;	
		stAlignPar.rEndAngle						= 180.0 + PR_SRCH_END_ANGLE;
	}
	else if (dRefAngle != 0)
	{
		stAlignPar.rStartAngle						= dRefAngle + PR_SRCH_START_ANGLE;	
		stAlignPar.rEndAngle						= dRefAngle + PR_SRCH_END_ANGLE;
	}

	//Update the seach die command
	stSrchCmd.emAlignAlg = GetLrnAlignAlgo(lDieNo);

	switch (m_lGenLrnInspMethod[lDieNo])
	{
		case 1:  
			stSrchCmd.emDieInspAlg = PR_DIE_INSP_ALG_BIN;
			break;

		default: 
			stSrchCmd.emDieInspAlg = PR_GOLDEN_DIE_TMPL;
			break;
	}

	stSrchCmd.stDieAlignPar.rStartAngle				= PR_SRCH_START_ANGLE;	
	stSrchCmd.stDieAlignPar.rEndAngle				= PR_SRCH_END_ANGLE;		
	stSrchCmd.emRetainBuffer						= bLatch;
	stSrchCmd.emLatch								= bLatch;
	stSrchCmd.emAlign								= bAlign;
	stSrchCmd.emDefectInsp							= bInspect;
    stSrchCmd.emCameraNo							= MS899_BOND_CAM_ID;
    stSrchCmd.emVideoSource							= PR_IMAGE_BUFFER_A;
    stSrchCmd.uwNRecordID							= 1;                  
    stSrchCmd.auwRecordID[0]						= (PR_UWORD)(lPrSrchID);       
    stSrchCmd.coProbableDieCentre					= stCoDieCenter;
    stSrchCmd.ulRpyControlCode						= PR_DEF_SRCH_DIE_RPY;
    stSrchCmd.stDieAlign							= stDieAlignCmd;
    stSrchCmd.stDieAlignPar							= stAlignPar;
    stSrchCmd.stDieInsp								= stDieInspCmd;
    stSrchCmd.stDieAlignPar.rMatchScore				= (PR_REAL)(m_lGenSrchDieScore[lDieNo]);
	stSrchCmd.stDieAlignPar.emIsDefaultMatchScore	= PR_FALSE;
    stSrchCmd.emGraphicInfo							= PR_NO_DISPLAY; //PR_SHOW_CRITICAL_DEFECT;
	
	//v4.50A26	//For MS100/MS60/MS90	//v4.51A7
	//typedef enum {PR_DISP_OPTION_DEFAULT,
	//				PR_DISP_OPTION_SKIP_AUTOBOND_IMG,
	//				PR_DISP_OPTION_END} PR_DISP_OPTION;
#ifdef	NU_MOTION
	stSrchCmd.emDispOption							= PR_DISP_OPTION_DEFAULT;
	if ( (m_ucMS100PrDisplayID != MS_PR_DISPLAY_DEFAULT) && 
		 (ucColletID > 0) && 
		 (bDieType != BPR_REFERENCE_DIE) )
	{
		//CString szMsg;
		//szMsg.Format("BPR postbond - %d %d, type = %d", ucColletID, m_ucMS100PrDisplayID, bDieType);
		if ((ucColletID == 1) && (m_ucMS100PrDisplayID != MS_PR_DISPLAY_COLLET1))
		{
			stSrchCmd.emDispOption = PR_DISP_OPTION_SKIP_AUTOBOND_IMG;
			//szMsg.Format("BPR disable Collet 1 image - %d %d", ucColletID, m_ucMS100PrDisplayID);
		}
		else if ((ucColletID == 2) && (m_ucMS100PrDisplayID != MS_PR_DISPLAY_COLLET2))
		{
			stSrchCmd.emDispOption = PR_DISP_OPTION_SKIP_AUTOBOND_IMG;
			//szMsg.Format("BPR disable Collet 2 image - %d %d", ucColletID, m_ucMS100PrDisplayID);
		}
		//stSrchCmd.emDispOption = PR_DISP_OPTION_SKIP_AUTOBOND_IMG;
		//szMsg.Format("BPR disable Collet BOTH image - %d %d", ucColletID, m_ucMS100PrDisplayID);
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	}
#endif

	PR_SrchDieCmd(&stSrchCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &uwCommunStatus);
    
	if (uwCommunStatus != PR_COMM_NOERR )
	{
		//CString csMsg;
		//csMsg.Format("BPR Search die error = %x",uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
	}

	return PR_ERR_NOERR;
}


PR_UWORD CBondPr::SearchDieRpy1(PR_SRCH_DIE_RPY1 *stSrchRpy1)
{
	PR_SrchDieRpy1(MS899_BOND_CAM_SEND_ID, stSrchRpy1);

    if (stSrchRpy1->uwCommunStatus != PR_COMM_NOERR)
	{
		//CString csMsg;
		//csMsg.Format("BPR Get reply1 error = %x", stSrchRpy1->uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
    }

    if (stSrchRpy1->uwPRStatus != PR_ERR_NOERR)
	{
		return stSrchRpy1->uwPRStatus;
    }

	return PR_ERR_NOERR;
}


PR_UWORD CBondPr::SearchDieRpy2(PR_SRCH_DIE_RPY2 *stSrchRpy2)
{
    PR_SrchDieRpy2(MS899_BOND_CAM_SEND_ID, stSrchRpy2);


	if (stSrchRpy2->stStatus.uwCommunStatus != PR_COMM_NOERR)
    {
		//CString csMsg;
		//csMsg.Format("BPR Get reply2 error = %x",stSrchRpy2->stStatus.uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
	}

	return stSrchRpy2->stStatus.uwPRStatus;
}


PR_UWORD CBondPr::SearchDieRpy3(PR_SRCH_DIE_RPY3 *stSrchRpy3)
{
    PR_SrchDieRpy3(MS899_BOND_CAM_SEND_ID, stSrchRpy3);


	if (stSrchRpy3->stStatus.uwCommunStatus != PR_COMM_NOERR)
    {
		//CString csMsg;
		//csMsg.Format("BPR Get reply3 error = %x",stSrchRpy3->stStatus.uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
	}

	return stSrchRpy3->stStatus.uwPRStatus;
}


PR_UWORD CBondPr::ExtractDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore)
{
	switch(stSrchRpy2.stStatus.uwPRStatus)
	{
	case PR_ERR_NOERR:
	case PR_WARN_SMALL_SRCH_WIN:
	case PR_WARN_TOO_MANY_DIE:
	case PR_WARN_BACKUP_ALIGNED:
		if (bUseReply3 == TRUE)
		{
			*usDieType		= stSrchRpy3.stStatus.uwPRStatus;
		}
		else
		{
			*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
		}
		if (*usDieType == PR_ERR_NOERR)
		{
			*usDieType	= PR_ERR_GOOD_DIE;
		}

		*fDieRotate		= stSrchRpy2.stDieAlign[0].stGen.rDieRot;
		*stDieOffset	= stSrchRpy2.stDieAlign[0].stGen.coDieCentre;
		*fDieScore		= stSrchRpy2.stDieAlign[0].stGen.rMatchScore;
		break;

	case PR_ERR_LOCATED_DEFECTIVE_DIE:
	case PR_ERR_ROTATION:
		*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
		*fDieRotate		= stSrchRpy2.stDieAlign[0].stGen.rDieRot;
		*stDieOffset	= stSrchRpy2.stDieAlign[0].stGen.coDieCentre;
		*fDieScore		= stSrchRpy2.stDieAlign[0].stGen.rMatchScore;
		break;

	case PR_ERR_DIE_SIZE_RJT:
	case PR_ERR_DEFECTIVE_DIE:
	case PR_ERR_CORNER_ANGLE_FAIL:
	case PR_ERR_NO_DIE:
	case PR_ERR_ROTATION_EXCEED_SPEC:
	case PR_ERR_NON_LOCATED_DEFECTIVE_DIE:
	case PR_ERR_NO_SYS_MEM:
	case PR_ERR_FAIL_CONTRAST:
	case PR_ERR_PID_NOT_LD:
	default:
		*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
		if (*usDieType == PR_ERR_DEFECTIVE_DIE)
		{
			*usDieType = PR_ERR_NON_LOCATED_DEFECTIVE_DIE;
		}

		*fDieRotate		= 0.0;
		stDieOffset->x	= (PR_WORD)m_lPrCenterX;
		stDieOffset->y	= (PR_WORD)m_lPrCenterY;
		break;
	}

	return 0;
}

//Klocwork
PR_WORD CBondPr::ManualSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, 
								  PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore,
								  PR_COORD stCorner1, PR_COORD stCorner2, DOUBLE dRefAngle, INT nDebug)
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	CString				csMsgA;
	CString				csMsgB;
	PR_UWORD			usSearchResult;
	BOOL				bGetRpy3 = FALSE;
	PR_WIN				stSearchArea;

	stSearchArea.coCorner1 = stCorner1;
	stSearchArea.coCorner2 = stCorner2;
	VerifyPRRegion(&stSearchArea);

	MotionSetOutputBit(BPR_SO_SEARCH_DIE_CMD, TRUE);
	usSearchResult = SearchDieCmd(bDieType, lDieNo, stSearchArea.coCorner1, stSearchArea.coCorner2, bLatch, bAlign, bInspect, 0, dRefAngle);
	if (usSearchResult != PR_ERR_NOERR)
	{
		if (usSearchResult == IDS_BPR_DIENOTLEARNT)
		{
			csMsgA.Format("BPR Not Learnt");
		}
		else
		{
			csMsgA.Format("Send Command = %x",usSearchResult);
		}
		SetAlert_Msg(IDS_BPR_SRH_DIE_FAILED, csMsgA);
		return -1;
	}
	MotionSetOutputBit(BPR_SO_SEARCH_DIE_CMD, FALSE);


	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_1, TRUE);
	usSearchResult = SearchDieRpy1(&stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR)
	{
		csMsgA.Format("Get Reply 1 = %x (nDEBUG = %d)",usSearchResult, nDebug);
		SetAlert_Msg(IDS_BPR_SRH_DIE_FAILED, csMsgA);
		*usDieType = PR_ERR_NO_DIE;
		return -1;
	}
	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_1, FALSE);


	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_2, TRUE);
	usSearchResult = SearchDieRpy2(&stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		csMsgA.Format("Get Reply 2 = %x",usSearchResult);
		SetAlert_Msg(IDS_BPR_SRH_DIE_FAILED, csMsgA);
		*usDieType = PR_ERR_NO_DIE;
		return -1;
	}
	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_2, FALSE);

    *usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (*usDieType == PR_ERR_NOERR)
	{
        *usDieType = PR_ERR_GOOD_DIE;
    }

	if ((DieIsAlignable(*usDieType) == TRUE) && (bInspect == TRUE))
	{
		bGetRpy3 = TRUE;

		MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_3, TRUE);
		usSearchResult = SearchDieRpy3(&stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			csMsgA.Format("Get Reply 3 = %x",usSearchResult);
			SetAlert_Msg(IDS_BPR_SRH_DIE_FAILED, csMsgA);
			return -1;
		}
		MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_3, FALSE);
	}

	//Identify die type
	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore);
	CalculateDefectScore(*usDieType, bDieType, lDieNo, &stSrchRpy3);		//v2.96T4

	return 0;
}


BOOL CBondPr::CalculateDefectScore(PR_UWORD usDieType, BOOL bDieType, LONG lInputDieNo, PR_SRCH_DIE_RPY3 *stSrchRpy3)
{
	LONG lDieNo =0;

	DOUBLE dSArea = stSrchRpy3->stDieInsp[0].stTmpl.aeLargestDefectArea;
	DOUBLE dTArea = stSrchRpy3->stDieInsp[0].stTmpl.aeTotalDefectArea;

	if (lInputDieNo <= 0)
	{
		lInputDieNo = 1;
	}

	if (bDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	}
	
	if (bDieType == BPR_REFERENCE_DIE)
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	}

	if ((m_stGenDieSize[lDieNo].x == 0) && (m_stGenDieSize[lDieNo].y == 0))
	{
		m_dSpSingleDefectScore	= 0.00;
		m_dSpTotalDefectScore	= 0.00;
		return FALSE;
	}

	if (usDieType == PR_ERR_CHIP_DIE)	// PR checks CHIP first before defect 
	{
		m_dSpSingleDefectScore	= 0.00;
		m_dSpTotalDefectScore	= 0.00;
		m_dSpChipAreaScore		= dSArea / (m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) * 100;
	}
	else
	{
		m_dSpSingleDefectScore	= dSArea / (m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) * 100;
		m_dSpTotalDefectScore	= dTArea / (m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) * 100;
		m_dSpChipAreaScore		= 0.00;
	}

	if ((usDieType == PR_ERR_GOOD_DIE) || (usDieType == PR_ERR_NOERR))
	{
		//Good die
		//if (m_dSpSingleDefectScore > m_dSrchSingleDefectArea)
		//	m_dSpSingleDefectScore = m_dSrchSingleDefectArea;
		//if (m_dSpTotalDefectScore > m_dSrchTotalDefectArea)
		//	m_dSpTotalDefectScore = m_dSrchTotalDefectArea;
	}
	else
	{
		//No-Good die
		if ((usDieType != PR_ERR_CHIP_DIE) &&
			(m_dSpSingleDefectScore < m_dGenSrchSingleDefectArea[lDieNo]) && 
			(m_dSpTotalDefectScore < m_dGenSrchTotalDefectArea[lDieNo]))
		{
			if ((m_dGenSrchSingleDefectArea[lDieNo] - m_dSpSingleDefectScore) < 1)
				m_dSpSingleDefectScore = m_dGenSrchSingleDefectArea[lDieNo];
			if ((m_dGenSrchTotalDefectArea[lDieNo] - m_dSpTotalDefectScore) < 1)
				m_dSpTotalDefectScore = m_dGenSrchTotalDefectArea[lDieNo];
		}
	}
	return TRUE;
}


VOID CBondPr::GetSearchDieArea(PR_WIN* stSrchArea, INT nDieNo, INT nEnlarge)
{
	if (nEnlarge > 0)
	{
		stSrchArea->coCorner1.x = (PR_WORD)m_lPrCenterX - (PR_WORD)(((m_lGenSrchDieAreaX[nDieNo]+nEnlarge)*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].x * BPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner1.y = (PR_WORD)m_lPrCenterY - (PR_WORD)(((m_lGenSrchDieAreaY[nDieNo]+nEnlarge)*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].y * BPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.x = (PR_WORD)m_lPrCenterX + (PR_WORD)(((m_lGenSrchDieAreaX[nDieNo]+nEnlarge)*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].x * BPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.y = (PR_WORD)m_lPrCenterY + (PR_WORD)(((m_lGenSrchDieAreaY[nDieNo]+nEnlarge)*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].y * BPR_SEARCHAREA_SHIFT_FACTOR);
	}
	else	//Default
	{
		stSrchArea->coCorner1.x = (PR_WORD)m_lPrCenterX - (PR_WORD)((m_lGenSrchDieAreaX[nDieNo]*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].x * BPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner1.y = (PR_WORD)m_lPrCenterY - (PR_WORD)((m_lGenSrchDieAreaY[nDieNo]*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].y * BPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.x = (PR_WORD)m_lPrCenterX + (PR_WORD)((m_lGenSrchDieAreaX[nDieNo]*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].x * BPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.y = (PR_WORD)m_lPrCenterY + (PR_WORD)((m_lGenSrchDieAreaY[nDieNo]*1.0/4 + 0.5) * m_stGenDieSize[nDieNo].y * BPR_SEARCHAREA_SHIFT_FACTOR);
	}

	VerifyPRRegion(stSrchArea);
}


VOID CBondPr::DisplaySearchDieResult(PR_UWORD usDieType, BOOL bDieType, LONG lInputDieNo, PR_REAL fDieRotate, PR_COORD stDieOffset, PR_REAL fDieScore)
{
	LONG		lDieNo = 0;
	CString		csMsgA;
	CString		csMsgB;
	CString		csMsgC = "";
	int			siXAxis; 
	int			siYAxis;

	if (lInputDieNo <= 0)
	{
		lInputDieNo = 1;
	}
	
	if (bDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	}
	
	if (bDieType == BPR_REFERENCE_DIE)
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	}

	switch(usDieType)
	{
		case PR_ERR_GOOD_DIE:		
			csMsgA.LoadString(HMB_PRS_GOOD_DIE);
			break;
		case PR_ERR_NO_DIE :		
			csMsgA.LoadString(HMB_PRS_NO_DIE);
			break;
		case PR_ERR_CHIP_DIE:		
			csMsgA.LoadString(HMB_PRS_CHIP_DIE);
			break;
		case PR_ERR_INK_DIE:		
			csMsgA.LoadString(HMB_PRS_CHIP_DIE);
			break;
		case PR_ERR_HALF_DIE:		
			csMsgA.LoadString(HMB_PRS_HALF_DIE);
			break;
		case PR_ERR_RJT_INT:		
			csMsgA.LoadString(HMB_PRS_BLANK_DIE);
			break;
		case PR_ERR_RJT_PAT:		
			csMsgA.LoadString(HMB_PRS_TEST_DIE);
			break;
		case PR_ERR_DEFECTIVE_DIE:	
			csMsgA.LoadString(HMB_PRS_DEFECT_DIE);
			break;
		case PR_ERR_NON_LOCATED_DEFECTIVE_DIE:
			csMsgA.LoadString(HMB_PRS_NL_DEFECT_DIE);
			break;
		case PR_ERR_LOCATED_DEFECTIVE_DIE:
			csMsgA.LoadString(HMB_PRS_L_DEFECT_DIE);
			break;
		case PR_ERR_EXCEED_INTENSITY_VAR:
			csMsgA.LoadString(HMB_PRS_AB_INTENSITY);
			break;
		case PR_ERR_DIE_SIZE_RJT:
			csMsgA.LoadString(HMB_PRS_REJECT_SIZE);
			break;
		case PR_ERR_ROTATION:
			csMsgA.LoadString(HMB_PRS_REJECT_ANGLE);
			break;
		case PR_ERR_ROTATION_EXCEED_SPEC:
			csMsgA.LoadString(HMB_PRS_EXCEED_ANGLE);
			break;
		case PR_ERR_CORNER_ANGLE_FAIL:
			csMsgA.LoadString(HMB_PRS_REJECT_CRN);
			break;
		case PR_ERR_BOND_PAD_SIZE_RJT:
			csMsgA.LoadString(HMB_PRS_REJECT_PAD_SIZE);
			break;
		case PR_ERR_BOND_PAD_FOREIGN_MAT:
			csMsgA.LoadString(HMB_PRS_REJECT_PAD_AREA);
			break;
		case PR_ERR_LIGHT_EMITTING_FOREIGN_MAT:
			csMsgA.LoadString(HMB_PRS_REJECT_LE_AREA);
			break;
		case PR_ERR_INSUFF_EPOXY_COVERAGE:
			//csMsgA = "Bad Cut Die";
			break;
		default:
			csMsgA.Format("Identify Error (%d)\n",usDieType);	
			break;
	}
	

	GetBinTableEncoder(&siXAxis, &siYAxis);

	if (bDieType == BPR_REFERENCE_DIE)		//v3.80
		csMsgA = "Ref Die Type: " + csMsgA + "\n";
	else
		csMsgA = "Die Type: " + csMsgA + "\n";


	int	siStepX = 0, siStepY = 0;
	if (DieIsAlignable(usDieType) == TRUE)
	{
		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	}
	csMsgB.Format(" Die Angle: %f\n",fDieRotate);
	csMsgA += csMsgB;
	csMsgB.Format(" Die X: %d (%d) offset (%d)\n", stDieOffset.x, siXAxis, siStepX);
	csMsgA += csMsgB;
	csMsgB.Format(" Die Y: %d (%d) offset (%d)\n", stDieOffset.y, siYAxis, siStepY);
	csMsgA += csMsgB;
	csMsgB.Format(" Die Score: %d\n", (LONG)(fDieScore));
	csMsgA += csMsgB;

	//v2.96T4
	//Display defect result if enabled
	if (m_bGenSrchEnableDefectCheck[lDieNo])
	{
		if (usDieType == PR_ERR_CHIP_DIE)
		{
			csMsgC.Format(" Chip Score: %.1f (%.1f)\n", m_dSpChipAreaScore, m_dGenSrchChipArea[lDieNo]);
		}
		else
		{
			csMsgC.Format(" Single-Defect Score: %.1f (%.1f)\n Total-Defect Score: %.1f (%.1f)\n", 
				m_dSpSingleDefectScore, m_dGenSrchSingleDefectArea[lDieNo], 
				m_dSpTotalDefectScore, m_dGenSrchTotalDefectArea[lDieNo]);
		}
		csMsgA += csMsgC;
	}

	//HmiMessage(csMsgA,"Search Die Result" , 2, 1);
	csMsgB.LoadString(HMB_BRP_SRN_DIE_RESULT);
	HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
}


BOOL CBondPr::AutoSearchDie(VOID)
{
	if (m_bDisableBT)	//v3.64
		return TRUE;

	LONG lDieNo = BPR_NORMAL_DIE;

#ifdef NU_MOTION
	//choose which record to be used to search collet hole
	if (m_bPostBondAtBond)
	{
		BOOL bRecord1 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"]);
		if (bRecord1 == TRUE)
		{
			lDieNo = BPR_NORMAL_DIE + 1;	//v4.47A8
		}
	}
#endif

	PR_UWORD	usSearchResult;
	CString		szDebugMsg;

#ifdef NU_MOTION
	BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
	BOOL bBTAtBH1Pos		= (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];		//v4.48A19

	DOUBLE dCentreOffsetX = 0;
	DOUBLE dCentreOffsetY = 0;

	//Show die size
	PR_COORD stBondDieSize;

	stBondDieSize.x = (PR_WORD)((LONG)(*m_psmfSRam)["BondPr"]["Die Size X"]);
	stBondDieSize.y = (PR_WORD)((LONG)(*m_psmfSRam)["BondPr"]["Die Size Y"]);

	m_lBTCol				= (LONG)(*m_psmfSRam)["BinTable"]["BT Bond Col"];
	m_lBTRow				= (LONG)(*m_psmfSRam)["BinTable"]["BT Bond Row"];

	PR_COORD stCollet1BondCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stCollet1BondCorner2 = m_stSearchArea.coCorner2;
	PR_COORD stCollet2BondCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stCollet2BondCorner2 = m_stSearchArea.coCorner2;

	CString szBH1Log, szBH2Log;
	UpdateBH1BH2SearchRange(stCollet1BondCorner1, stCollet1BondCorner2, stCollet2BondCorner1, stCollet2BondCorner2, szBH1Log, szBH2Log);

	if (!bBTAtBH1Pos)	//bBHZ2TowardsPick)	//Arm 2
	{
		if (m_bPostBondAtBond)		//v4.47A8
		{
			//Use whole screen area for collet hole searching
			PR_WIN stColletSrchArea;
			stColletSrchArea.coCorner1.x = PR_MAX_WIN_ULC_X;
			stColletSrchArea.coCorner1.y = PR_MAX_WIN_ULC_Y;
			stColletSrchArea.coCorner2.x = PR_MAX_WIN_LRC_X;
			stColletSrchArea.coCorner2.y = PR_MAX_WIN_LRC_Y;

			lDieNo = BPR_NORMAL_DIE;			//BPR_NORMAL_DIE=0=1 Normal Die Record #1 
			BOOL bRecord1 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"]);
			if (bRecord1 == TRUE)
			{
				lDieNo = BPR_NORMAL_DIE + 2;	//v4.47A8	//Normal Die Record #2
			}

			if (IsNGBlockNow())// Matthew 20181212
			{
				usSearchResult = SearchDieCmd(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9, stColletSrchArea.coCorner1, stColletSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
			else
			{
				usSearchResult = SearchDieCmd(BPR_NORMAL_DIE, lDieNo, stColletSrchArea.coCorner1, stColletSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
		}
		else if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)	//v4.52A14
		{
			//v4.56A1
			PR_WIN stArea;
			stArea.coCorner1 = stCollet2BondCorner1;
			stArea.coCorner2 = stCollet2BondCorner2;
			VerifyPRRegion(&stArea);

			if(IsNGBlockNow())// Matthew 20181212
			{
				usSearchResult = SearchDieCmd(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9, stArea.coCorner1, stArea.coCorner2, 
											PR_TRUE, PR_TRUE, PR_TRUE, 2);		//v4.50A26
			}
			else
			{
				usSearchResult = SearchDieCmd(BPR_NORMAL_DIE, lDieNo, stArea.coCorner1, stArea.coCorner2, 
											PR_TRUE, PR_TRUE, PR_TRUE, 2);		//v4.50A26
			}
		}
		else
		{
			if(IsNGBlockNow())// Matthew 20181212
			{
				usSearchResult = SearchDieCmd(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
			else
			{
				usSearchResult = SearchDieCmd(BPR_NORMAL_DIE, lDieNo, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
		}

		(*m_psmfSRam)["BondPr"]["IsArm1"] = FALSE;

	}
	else	//Arm1
	{
		//v4.56A1
		if (m_bPostBondAtBond)		//v4.47A8
		{
			//Use whole screen area for collet hole searching
			PR_WIN stColletSrchArea;
			stColletSrchArea.coCorner1.x = PR_MAX_WIN_ULC_X;
			stColletSrchArea.coCorner1.y = PR_MAX_WIN_ULC_Y;
			stColletSrchArea.coCorner2.x = PR_MAX_WIN_LRC_X;
			stColletSrchArea.coCorner2.y = PR_MAX_WIN_LRC_Y;

			lDieNo = BPR_NORMAL_DIE;			//BPR_NORMAL_DIE=0=1 Normal Die Record #1 
			BOOL bRecord1 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"]);
			if (bRecord1 == TRUE)
			{
				lDieNo = BPR_NORMAL_DIE + 2;	//v4.47A8	//Normal Die Record #2
			}

			if(IsNGBlockNow())// Matthew 20181212
			{
				usSearchResult = SearchDieCmd(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9, stColletSrchArea.coCorner1, stColletSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
			else
			{
				usSearchResult = SearchDieCmd(BPR_NORMAL_DIE, lDieNo, stColletSrchArea.coCorner1, stColletSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
		}
		else if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)	//v4.52A14
		{
			//v4.56A1
			PR_WIN stArea;
			stArea.coCorner1 = stCollet1BondCorner1;
			stArea.coCorner2 = stCollet1BondCorner2;
			VerifyPRRegion(&stArea);

			if(IsNGBlockNow())// Matthew 20181212
			{
				usSearchResult = SearchDieCmd(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9, stArea.coCorner1, stArea.coCorner2, 
									PR_TRUE, PR_TRUE, PR_TRUE, 1);
			}
			else
			{
				usSearchResult = SearchDieCmd(BPR_NORMAL_DIE, lDieNo, stArea.coCorner1, stArea.coCorner2, 
									PR_TRUE, PR_TRUE, PR_TRUE, 1);
			}
		}
		else
		{
			if(IsNGBlockNow())// Matthew 20181212
			{
				usSearchResult = SearchDieCmd(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
			else
			{
				usSearchResult = SearchDieCmd(BPR_NORMAL_DIE, lDieNo, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
			}
		}

		(*m_psmfSRam)["BondPr"]["IsArm1"] = TRUE;
	}

#else
	if(IsNGGradeNow())// Matthew 20181212
	{	
		usSearchResult = SearchDieCmd(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX9, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
	}
	else
	{
		usSearchResult = SearchDieCmd(BPR_NORMAL_DIE, lDieNo, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE);
	}
#endif

	if (usSearchResult != PR_ERR_NOERR)
	{
		CString csMsg;
		if (usSearchResult == IDS_BPR_DIENOTLEARNT)
		{
			csMsg.Format("BPR Not Learnt");
		}
		else
		{
			csMsg.Format("BPR: AutoSearchDie  Cmd error = 0x%x",usSearchResult);
		}
		SetErrorMessage(csMsg);		//v3.93	

		HmiMessage_Red_Yellow(csMsg, "Bond PR");
		return FALSE;
	}

	return TRUE;
}


BOOL CBondPr::AutoGrabDieDone(VOID)
{
	if (m_bDisableBT)	//v3.64
		return TRUE;

	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_UWORD			usSearchResult;

	usSearchResult = SearchDieRpy1(&stSrchRpy1);

	if (usSearchResult != PR_ERR_NOERR)
	{
		CString csMsg;
		csMsg.Format("BPR: AutoGrabDieDone error = 0x%x", usSearchResult);
		SetErrorMessage(csMsg);		//v3.93

		HmiMessage_Red_Yellow(csMsg, "Bond PR");
		return FALSE;
	}

	return TRUE;
}


LONG CBondPr::AutoDieResult(VOID)
{
	if (m_bDisableBT)	//v3.64
		return TRUE;

	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset; 
	PR_REAL				fDieScore;
	BOOL				bGetRpy3 = FALSE;


	usSearchResult = SearchDieRpy2(&stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		CString csMsg;
		csMsg.Format("Get reply 2 error! = %x",usSearchResult);
		DisplaySequence(csMsg);	
		return FALSE;
	}

    usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }

	//Init stSrchRpy3
	//memset(&stSrchRpy3,0,sizeof(stSrchRpy3));

	if (DieIsAlignable(usDieType) == TRUE)
	{
		usSearchResult = SearchDieRpy3(&stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			CString csMsg;
			csMsg.Format("Get reply 3 error! = %x",usSearchResult);
			DisplaySequence(csMsg);	
			return FALSE;
		}
		bGetRpy3 = TRUE;
	}

	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore);

	if (DieIsAlignable(usDieType) == TRUE)
	{
		m_stAutoDieOffset.x = stDieOffset.x; 
		m_stAutoDieOffset.y = stDieOffset.y; 
	}
	else
	{
		m_stAutoDieOffset.x = (PR_WORD)m_lPrCenterX; 
		m_stAutoDieOffset.y = (PR_WORD)m_lPrCenterY; 
	}

	//Log result into Postbond SPC
	return (LogPostBondData(BPR_POSTBOND_RUNTIME, usDieType, fDieRotate, stDieOffset));
}


LONG CBondPr::AutoLogDieResult(VOID)
{
	if (m_bDisableBT)	//v3.64
		return TRUE;

	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset; 
	PR_REAL				fDieScore;
	BOOL				bGetRpy3 = FALSE;


	usSearchResult = SearchDieRpy2(&stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		return FALSE;
	}

    usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }

	//Init stSrchRpy3
	if (DieIsAlignable(usDieType) == TRUE)
	{
		usSearchResult = SearchDieRpy3(&stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			return FALSE;
		}
		bGetRpy3 = TRUE;
	}

	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore);


	int		siStepX, siStepY;
	if (DieIsAlignable(usDieType))
	{
		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	}
	else
	{
		siStepX = 0;
		siStepY = 0;
	}

	LONG lColletHoleEnc_T = 0;
	LONG lColletHoleEnc_Z = 0;
	LONG lColletHoleEnc_Z2 = 0;
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead != NULL)
	{
		lColletHoleEnc_T = pBondHead->GetColletHoleEnc_T();
		lColletHoleEnc_Z = pBondHead->GetColletHoleEnc_Z();
		lColletHoleEnc_Z2 = pBondHead->GetColletHoleEnc_Z2();
	}

	//Obtained from stLastDie in WT:OpNextDie()		//v4.55A7
	LONG lUserRow	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"];
	LONG lUserCol	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"];
	BOOL bBondFromPrePick = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BondFromPrePick"];


	DOUBLE dAvgUnitOffsetX=0, dAvgUnitOffsetY=0;
	LONG lAvgOffsetX=0, lAvgOffsetY=0;

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format("%Y-%m-%d (%H:%M:%S),");

	BOOL bRecord1 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"]);
	if ( bRecord1 == FALSE )
	{
		if (DieIsAlignable(usDieType))
		{
			AddRTBHCompXY(2, siStepX, siStepY);		//v3.86
			UpdateAvgRTCompOffsetXY(2, lAvgOffsetX, lAvgOffsetY, dAvgUnitOffsetX, dAvgUnitOffsetY);
		}

		//Log to WaferPr file
		CStdioFile oLogFile;
		if (oLogFile.Open(_T("c:\\Mapsorter\\UserData\\BondPr.txt"), 
					CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText))
		{
			oLogFile.SeekToEnd();
			CString szLine;

			if (m_bEnableRTCompXY && !IsBurnIn())		//v3.86		
				szLine.Format("%d,%d,%d,%d,%.2f,%d,%d,0x%x,%d,%.1f,%d,%d,%.1f\n", 
								lUserRow, lUserCol,
								siStepX - lAvgOffsetX, siStepY - lAvgOffsetY, fDieRotate, 
								lAvgOffsetX, lAvgOffsetY, 
								usDieType,
								bBondFromPrePick,
								m_dBHTThermostatReading,
								lColletHoleEnc_T, lColletHoleEnc_Z, fDieScore);
			else
				szLine.Format("%d,%d,%d,%d,%.2f,0x%x,%d,%.1f,%d,%d,%.1f\n", 
								lUserRow, lUserCol, siStepX, siStepY, fDieRotate, usDieType, bBondFromPrePick,
								m_dBHTThermostatReading,
								lColletHoleEnc_T, lColletHoleEnc_Z, fDieScore);

			oLogFile.WriteString(szTime + szLine);		
			oLogFile.Close();
		}
	}
	else
	{
		if (DieIsAlignable(usDieType))
		{
			AddRTBHCompXY(1, siStepX, siStepY);		//v3.86
			UpdateAvgRTCompOffsetXY(1, lAvgOffsetX, lAvgOffsetY, dAvgUnitOffsetX, dAvgUnitOffsetY);
		}

		//Log to WaferPr file
		CStdioFile oLogFile;
		if (oLogFile.Open(_T("c:\\Mapsorter\\UserData\\BondPr2.txt"), 
					CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText))
		{
			oLogFile.SeekToEnd();
			CString szLine;

			if (m_bEnableRTCompXY && !IsBurnIn())		//v3.86		
				szLine.Format("%d,%d,%d,%d,%.2f,%d,%d,0x%x,%d,%.1f,%d,%d,%.1f\n", 
								lUserRow, lUserCol,
								siStepX-lAvgOffsetX, siStepY-lAvgOffsetY, fDieRotate, 
								lAvgOffsetX, lAvgOffsetY, usDieType, 
								bBondFromPrePick, m_dBHTThermostatReading,
								lColletHoleEnc_T, lColletHoleEnc_Z2, fDieScore);
			else
				szLine.Format("%d,%d,%d,%d,%.2f,0x%x,%d,%.1f,%d,%d,%.1f\n", 
								lUserRow, lUserCol, siStepX, siStepY, fDieRotate, usDieType, bBondFromPrePick,
								m_dBHTThermostatReading,
								lColletHoleEnc_T, lColletHoleEnc_Z2, fDieScore);

			oLogFile.WriteString(szTime + szLine);		
			oLogFile.Close();
		}
	}

	//v4.47A7
	DOUBLE dUnitX = 0, dUnitY = 0;
	ConvertUnit(siStepX, siStepY, &dUnitX, &dUnitY);
	*m_pDspDieX				= dUnitX;
	*m_pDspDieY				= dUnitY;
	*m_pDspDieTheta			= (double)(fDieRotate);
	m_pDspBin->Log();

	//Collet Hole tset always log postbond data into BIn #1 SPC 
	*m_pDieX[0]				= dUnitX;
	*m_pDieY[0]				= dUnitY;
	*m_pDieTheta[0]			= (double)(fDieRotate);
	m_pBin[0]->Log();

	CString szMsg;

	szMsg.Format("PBI result pixel(%d,%d), Offset Count(%d,%d), Offset Mil(%.3f, %.03f)", 
						 stDieOffset.x, stDieOffset.y, siStepX, siStepY, dUnitX, dUnitY);
	CMSLogFileUtility::Instance()->MS60_Log(szMsg);

	//Log result into Postbond SPC
	//return (LogPostBondData(BPR_POSTBOND_RUNTIME, usDieType, fDieRotate, stDieOffset));
	return TRUE;	//v4.47 Klocwork
}


BOOL CBondPr::AutoLookAroundDieInFOV(BOOL& bCriticalError, ULONG ulBin, BOOL bIsArm1)
{
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
//	PR_COORD stSrchCenter; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;
	int siStepX=0, siStepY=0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	if (m_dFovSize < BPR_LF_SIZE)
	{
		return TRUE;	//Assume FOV has dices
	}
	//if (pApp->GetCustomerName().Find(CTM_SANAN) == -1)		//Only available for SanAn
	//{
	//	return TRUE;	//Assume FOV has dices
	//}
	if (m_lOutCamBlockEmptyCounter >= 4)
	{
		bCriticalError = TRUE;
		return FALSE;
	}


	//LONG lBTIndexDir = (*m_psmfSRam)["BinTable"]["IndexDirection"];

	LONG lSearchDie = 0;
	PR_WIN stSrchArea1;
	PR_WIN stSrchArea2;
	PR_WIN stSrchArea3;
	PR_WIN stSrchArea4;
	stSrchArea4.coCorner1.x = PR_MAX_WIN_ULC_X;
	stSrchArea4.coCorner1.y = PR_MAX_WIN_ULC_Y;
	stSrchArea4.coCorner2.x = PR_MAX_WIN_LRC_X;
	stSrchArea4.coCorner2.y = PR_MAX_WIN_LRC_Y;
	stSrchArea1 = m_stSearchArea;
	stSrchArea2 = m_stSearchArea;
	stSrchArea3 = m_stSearchArea;

	CalculateLFSearchArea(BPR_UP_DIE, 1, &stSrchArea1);
	VerifyPRRegion(&stSrchArea1);
	CalculateLFSearchArea(BPR_RT_DIE, 1, &stSrchArea2);	//LF RIGHT die
	VerifyPRRegion(&stSrchArea2);
	CalculateLFSearchArea(BPR_LT_DIE, 1, &stSrchArea3);	//LF LEFT die
	VerifyPRRegion(&stSrchArea3);

CString szLog;
//szLog.Format("4: %d, %d, %d, %d", stSrchArea1.coCorner1.x, stSrchArea1.coCorner1.y,
//	stSrchArea1.coCorner2.x, stSrchArea1.coCorner2.y);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	lSearchDie = ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_FALSE, 
							PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
							stSrchArea1.coCorner1, stSrchArea1.coCorner2, 0, 39);
	
	if (DieIsAlignable(usDieType) == TRUE)
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Auto LookAround FOV 1 finds UP die");
		m_lOutCamBlockEmptyCounter = 0;
        return TRUE;	//Die is found under Bond PR FOV -> EMPTY + 1
	}

//szLog.Format("5: %d, %d, %d, %d", stSrchArea2.coCorner1.x, stSrchArea2.coCorner1.y,
//	stSrchArea2.coCorner2.x, stSrchArea2.coCorner2.y);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	lSearchDie = ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_FALSE, 
							PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
							stSrchArea2.coCorner1, stSrchArea2.coCorner2, 0, 40);

	if (DieIsAlignable(usDieType) == TRUE)
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Auto LookAround FOV 2 finds RT die");
		m_lOutCamBlockEmptyCounter = 0;
        return TRUE;	//Die is found under Bond PR FOV -> EMPTY + 1
	}


//szLog.Format("6: %d, %d, %d, %d", stSrchArea3.coCorner1.x, stSrchArea3.coCorner1.y,
//	stSrchArea3.coCorner2.x, stSrchArea3.coCorner2.y);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	lSearchDie = ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_FALSE, 
							PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
							stSrchArea3.coCorner1, stSrchArea3.coCorner2, 0, 41);

	if (DieIsAlignable(usDieType) == TRUE)
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Auto LookAround FOV 3 finds LT die");
		m_lOutCamBlockEmptyCounter = 0;
        return TRUE;	//Die is found under Bond PR FOV -> EMPTY + 1
	}


//szLog.Format("7: %d, %d, %d, %d", stSrchArea4.coCorner1.x, stSrchArea4.coCorner1.y,
//	stSrchArea4.coCorner2.x, stSrchArea4.coCorner2.y);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_FALSE, PR_TRUE, PR_FALSE, 
						&usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
						stSrchArea4.coCorner1, stSrchArea4.coCorner2, 0, 42);	

	if (DieIsAlignable(usDieType) == TRUE)
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Auto LookAround FOV 4 finds LA die");
		m_lOutCamBlockEmptyCounter = 0;
        return TRUE;	//Die is found under Bond PR FOV -> EMPTY + 1
	}

	m_lOutCamBlockEmptyCounter++;
	if (m_lOutCamBlockEmptyCounter >= 4)
	{
		bCriticalError = TRUE;
	}

//CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Camera blocked (SanAn)");
	return FALSE;		//No other dice found under FOV, camera blocking !!!
}


BOOL CBondPr::AutoSearchRTPadOffsetXY(BOOL bIsArm1, 
									  INT nDieCenterXInPixel, INT nDieCenterYInPixel,
									  INT siDieStepX, INT siDieStepY, 
									  DOUBLE dCenterXInPixel, DOUBLE dCenterYInPixel)	//andrewng
{
	BOOL bBTAtBH1Pos		= (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];
	BOOL bPreBondAlign		= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];
	if (!bPreBondAlign)
	{
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPr	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);

//	PR_UWORD	usSearchResult;
	BOOL bStatus = TRUE;

	DOUBLE dCentreOffsetX = 0;
	DOUBLE dCentreOffsetY = 0;

	//Show die size
	PR_COORD stBondDieSize;
	PR_COORD stBondCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stBondCorner2 = m_stSearchArea.coCorner2;

	stBondDieSize.x = (PR_WORD)((LONG)(*m_psmfSRam)["BondPr"]["Die Size X"]);
	stBondDieSize.y = (PR_WORD)((LONG)(*m_psmfSRam)["BondPr"]["Die Size Y"]);

	PR_COORD stCorner1, stCorner2;
	PR_COORD stCollet2BondCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stCollet2BondCorner2 = m_stSearchArea.coCorner2;
	PR_COORD stCollet1BondCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stCollet1BondCorner2 = m_stSearchArea.coCorner2;

	CString szBH1Log, szBH2Log;
	UpdateBH1BH2SearchRange(stCollet1BondCorner1, stCollet1BondCorner2,	stCollet2BondCorner1, stCollet2BondCorner2, szBH1Log, szBH2Log);

	if (!bIsArm1)	//bBTAtBH1Pos)	//bBHZ2TowardsPick)	//Arm 2
	{
		stCorner1 = stCollet2BondCorner1;
		stCorner2 = stCollet2BondCorner2;
	}
	else	//Arm1
	{
		stCorner1 = stCollet1BondCorner1;
		stCorner2 = stCollet1BondCorner2;
	}

	CString		szLog;
//	PR_WORD		rtnPR;
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	PR_COORD	stDieOffset; 

	PR_WORD nResult = 0;
	nResult = ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, 
								&usDieType, &fDieRotate, &stDieOffset, &fDieScore,
								stCorner1, stCorner2, 0, 43);

	if ( nResult == -1 )
    {
		bStatus = FALSE;
		CMSLogFileUtility::Instance()->MS_LogOperation("BPR PostBond Pad fail");
		return FALSE;
    }

	if (!DieIsAlignable(usDieType))
	{
		CString szLog2;
		bStatus = FALSE;
		if (bIsArm1)	//bBTAtBH1Pos)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR PostBond Pad fail 1");
			szLog.Format("BPR PostBond PAD Offset (BH1) fails; Type=%d; SrchWnd(%d,%d, %d,%d)", 
				usDieType, stCorner1.x, stCorner1.y, stCorner2.x, stCorner2.y);
			szLog2 = "1,0,0,0,0,";
			
			(*m_psmfSRam)["BondPr"]["BH1PadOffset"]["X"] = 0;
			(*m_psmfSRam)["BondPr"]["BH1PadOffset"]["Y"] = 0;
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR PostBond Pad fail 2");
			szLog.Format("BPR PostBond PAD Offset (BH2) fails; Type=%d; SrchWnd(%d,%d, %d,%d)", 
				usDieType, stCorner1.x, stCorner1.y, stCorner2.x, stCorner2.y);
			szLog2 = "2,0,0,0,0,";

			(*m_psmfSRam)["BondPr"]["BH2PadOffset"]["X"] = 0;
			(*m_psmfSRam)["BondPr"]["BH2PadOffset"]["Y"] = 0;
		}

		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		//CMSLogFileUtility::Instance()->BT_TableIndex2Log(szLog2);


		return FALSE;
	}

	
	int siStepX=0, siStepY=0;

	//CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	//PR_COORD	stRelMove;
	//stRelMove.x = (PR_WORD) m_lPrCenterX - stDieOffset.x;
	//stRelMove.y = (PR_WORD) m_lPrCenterY - stDieOffset.y;
	//ConvertPixelToMotorStep(stRelMove, &siStepX, &siStepY);
	bStatus = TRUE;

	LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
	LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;
	DOUBLE dCalibX = GetCalibX();
	DOUBLE dCalibY = GetCalibY();

	if (!bIsArm1)	//bBTAtBH1Pos)	//If BHZ2
	{
		//calculate PR Centre with offset
		LONG lBHZ2BondPosOffsetX = 0; //(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"];
		LONG lBHZ2BondPosOffsetY = 0; //(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"];
		DOUBLE dPrCentreWithOffsetX = 0;
		DOUBLE dPrCentreWithOffsetY = 0;
		DOUBLE dOrgPrCentreWithOffsetX = 0;
		DOUBLE dOrgPrCentreWithOffsetY = 0;
		DOUBLE dtestCol2X = 0;
		DOUBLE dtestCol2Y = 0;
		LONG lBH2UplookOffSetX = 0;
		LONG lBH2UplookOffSetY = 0;

		if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)		//v4.52A14
		{
			GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, FALSE);
		}
		
		if (fabs(dCalibX) > 0.0000001)
		{
			dPrCentreWithOffsetX = m_lPrCenterX + ((lBHZ2BondPosOffsetX - lCollet2OffsetX) / dCalibX);
			dtestCol2X			 = (lBHZ2BondPosOffsetX - lCollet2OffsetX) / dCalibX;
		}

		if (fabs(dCalibY) > 0.0000001)
		{
			dPrCentreWithOffsetY = m_lPrCenterY + ((lBHZ2BondPosOffsetY - lCollet2OffsetY) / dCalibY);
			dtestCol2Y			 = (lBHZ2BondPosOffsetY - lCollet2OffsetY) / dCalibY;

		}

		//4.52D17cal PostBond (add uplook offset) 
		if(bEnableBHUplookPr)
		{
			lBH2UplookOffSetX = (LONG)(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetX"];
			lBH2UplookOffSetY = (LONG)(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetY"];

//			DOUBLE dOffsetInPixel;
			if (fabs(dCalibX) > 0.0000001)
			{
				dOrgPrCentreWithOffsetX = dPrCentreWithOffsetX;
				dPrCentreWithOffsetX	= dPrCentreWithOffsetX - (lBH2UplookOffSetX / 0.64); //CSP006
				
			}

			if (fabs(dCalibY) > 0.0000001)
			{
				dOrgPrCentreWithOffsetY = dPrCentreWithOffsetY;
				dPrCentreWithOffsetY	= dPrCentreWithOffsetY - (lBH2UplookOffSetY / 0.64); //CSP006
			}
		}

		//calculate die compensation with Centre Offset
		PR_COORD	stRelMove;
		stRelMove.x = (PR_WORD)dCenterXInPixel - stDieOffset.x;
		stRelMove.y = (PR_WORD)dCenterYInPixel - stDieOffset.y;

		ConvertPixelToMotorStep(stRelMove, &siStepX, &siStepY);
		//ConvertUnit(siStepX, siStepY, &dUnitX, &dUnitY);

		INT nXDiff = siDieStepX - siStepX;
		INT nYDiff = siDieStepY - siStepY;

		(*m_psmfSRam)["BondPr"]["BH2PadOffset"]["X"] = nXDiff;
		(*m_psmfSRam)["BondPr"]["BH2PadOffset"]["Y"] = nYDiff;

		szLog.Format("BPR PostBond PAD Offset (BH2 in pixel): Collet2 Center (%d, %d), Die Center (%d, %d), PAD Center (%d, %d)",
						(LONG) dCenterXInPixel, (LONG) dCenterYInPixel, 
						nDieCenterXInPixel, nDieCenterYInPixel,
						(INT) stDieOffset.x, (INT) stDieOffset.y);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);


		//andrewng12
		CString szLog;

		int nXDiffInStep = nXDiff;
		int nYDiffInStep = nYDiff;
		AddRTPreBondDieNPadXY(2, nXDiffInStep, nYDiffInStep);
		UpdateAvgRTPreBondDieNPadXY(2);


		szLog.Format("BPR PostBond PAD Offset (BH2 in motor steps): DIE(%ld, %ld), PAD(%ld, %ld), final OFFSET = DIE - PAD = (%d, %d), RT(%d, %d)", 
						siDieStepX, siDieStepY, siStepX, siStepY, nXDiff, nYDiff, nXDiffInStep, nYDiffInStep);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

	}
	else
	{
		//calculate PR Centre with offset
		LONG lBHZ1BondPosOffsetX = 0; //(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"];
		LONG lBHZ1BondPosOffsetY = 0; //(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"];
		DOUBLE dPrCentreWithOffsetX = 0;
		DOUBLE dPrCentreWithOffsetY = 0;
		DOUBLE dOrgPrCentreWithOffsetX = 0;
		DOUBLE dOrgPrCentreWithOffsetY = 0;
		DOUBLE dtestCol1X = 0;
		DOUBLE dtestCol1Y = 0;
		LONG lBH1UplookOffSetX = 0;
		LONG lBH1UplookOffSetY = 0;

		
		if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)
		{
			GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, FALSE);
		}

		if (fabs(dCalibX) > 0.0000001)
		{
			dPrCentreWithOffsetX = m_lPrCenterX + ((lBHZ1BondPosOffsetX - lCollet1OffsetX) / dCalibX);
			dtestCol1X = (lBHZ1BondPosOffsetX - lCollet1OffsetX) / dCalibX;
		}

		if (fabs(dCalibY) > 0.0000001)
		{
			dPrCentreWithOffsetY = m_lPrCenterY + ((lBHZ1BondPosOffsetY - lCollet1OffsetY) / dCalibY);
			dtestCol1Y = (lBHZ1BondPosOffsetY - lCollet1OffsetY) / dCalibY;
		}

		//4.52D17cal PostBond (add uplook offset) 
		if(bEnableBHUplookPr)
		{
			lBH1UplookOffSetX = (LONG)(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetX"];
			lBH1UplookOffSetY = (LONG)(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetY"];

			if (fabs(dCalibX) > 0.0000001)
			{
				dOrgPrCentreWithOffsetX = dPrCentreWithOffsetX;
				dPrCentreWithOffsetX	= dPrCentreWithOffsetX - (lBH1UplookOffSetX / 0.64);  //CSP006

			}
			if (fabs(dCalibY) > 0.0000001)
			{
				dOrgPrCentreWithOffsetY = dPrCentreWithOffsetY;
				dPrCentreWithOffsetY	= dPrCentreWithOffsetY - (lBH1UplookOffSetY / 0.64);  //CSP006
			}
		}

		//calculate die compensation with Centre Offset
		PR_COORD	stRelMove;
		stRelMove.x = (PR_WORD)dCenterXInPixel - stDieOffset.x;
		stRelMove.y = (PR_WORD)dCenterYInPixel - stDieOffset.y;

		ConvertPixelToMotorStep(stRelMove, &siStepX, &siStepY);

		INT nXDiff = siDieStepX - siStepX;
		INT nYDiff = siDieStepY - siStepY;

		(*m_psmfSRam)["BondPr"]["BH1PadOffset"]["X"] = nXDiff;
		(*m_psmfSRam)["BondPr"]["BH1PadOffset"]["Y"] = nYDiff;

		szLog.Format("BPR PostBond PAD Offset (BH1 in pixel): Collet2 Center (%d, %d), Die Center (%d, %d), PAD Center (%d, %d)",
						(LONG) dCenterXInPixel, (LONG) dCenterYInPixel, 
						nDieCenterXInPixel, nDieCenterYInPixel,
						(INT) stDieOffset.x, (INT) stDieOffset.y);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);


		CString szLog;
		int nXDiffInStep = nXDiff;
		int nYDiffInStep = nYDiff;
		AddRTPreBondDieNPadXY(1, nXDiffInStep, nYDiffInStep);
		UpdateAvgRTPreBondDieNPadXY(1);


		szLog.Format("BPR PostBond PAD Offset (BH1 in motor steps): DIE(%ld, %ld), PAD(%ld, %ld), final OFFSET = DIE - PAD = (%d, %d), RT(%d, %d)", 
						siDieStepX, siDieStepY, siStepX, siStepY, nXDiff, nYDiff, nXDiffInStep, nYDiffInStep);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}

	return bStatus;
}


BOOL CBondPr::AutoLFLastRowDieInFOV(BOOL& bCriticalError, ULONG ulBin, PR_COORD stCDieOffset, BOOL bIsArm1)
{
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
//	PR_COORD stSrchCenter; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;
	int siStepX=0, siStepY=0;
	DOUBLE dUnitX=0, dUnitY=0;
	CString szMsg;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	BOOL bLFLastRowDie = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinMapLastRowDie"];
	CString szCustomerName = pApp->GetCustomerName();

	if (m_dFovSize < BPR_LF_SIZE)
	{
		return TRUE;	//Assume OK
	}
	
	if (szCustomerName.Find(CTM_SANAN) != -1)		//Only available for SanAn
	{
	}
	else if (szCustomerName.Find("ChangeLight") != -1)
	{
	}
	else
	{
		return TRUE;	//Only available for SanAn & changelight
	}

	if (szCustomerName.Find(CTM_SANAN) != -1)		//At SanAn, only this fcn available for MS60 machines
	{
		if (IsMS60() || m_bEnableMS100EjtXY)	//Not availabe for EJT XY module at the moment	//v4.47T12
		{
			return TRUE;
		}
	}


	m_lLFLastRowDieCounter++;
	if (m_lLFLastRowDieCounter < 10)
	{
		return TRUE;
	}
	m_lLFLastRowDieCounter = 0;

	//LONG lBTIndexDir = (*m_psmfSRam)["BinTable"]["IndexDirection"];

	PR_WIN stSrchArea1;
	stSrchArea1 = m_stSearchArea;
	CalculateLFSearchArea(BPR_UP_DIE, 1, &stSrchArea1);
	VerifyPRRegion(&stSrchArea1);

	LONG lSearchDie = ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_FALSE, 
							PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore,
							stSrchArea1.coCorner1, stSrchArea1.coCorner2, 0, 44);
	
	if (DieIsAlignable(usDieType) != TRUE)
	{
		//szMsg.Format("SANAN BPR TRow Die: no die found (Type = %d); result = OK", usDieType);
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
        return TRUE;	//Die is found under Bond PR FOV -> EMPTY + 1
	}

	PR_COORD stNewDieOffset; 
	stNewDieOffset.x = stCDieOffset.x - stDieOffset.x;
	stNewDieOffset.y = stCDieOffset.y - stDieOffset.y;

	//CalculateDieCompenate(stNewDieOffset, &siStepX, &siStepY);
	ConvertPixelToMotorStep(stNewDieOffset, &siStepX, &siStepY);
	ConvertUnit(siStepX, siStepY, &dUnitX, &dUnitY);

	if ((fabs(dUnitX) > m_dGenDieShiftX[ulBin]))
	{
		m_lLFLastRowDieErrAccCount++;

//szMsg.Format("SANAN BPR TRow Die: TDie XY (%d, %d); CDie XY(%d, %d); Xoffset = %d (%.1f); limit = %.1f; result = FAIL (%ld)", 
//			 stDieOffset.x, stDieOffset.y, stCDieOffset.x, stCDieOffset.y, stNewDieOffset.x, 
//			 dUnitX, m_dGenDieShiftX[ulBin], m_lLFLastRowDieErrAccCount);
//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		
		if (m_lLFLastRowDieErrAccCount < 5)
			return TRUE;
		return FALSE;
	}

	m_lLFLastRowDieErrAccCount = 0;	//v4.47T10

//szMsg.Format("SANAN BPR TRow Die: TDie XY (%d, %d); CDie XY(%d, %d); Xoffset = %d (%.1f); limit = %.1f; result = OK", 
//			 stDieOffset.x, stDieOffset.y, stCDieOffset.x, stCDieOffset.y, stNewDieOffset.x, dUnitX, m_dGenDieShiftX[ulBin]);
//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	return TRUE;		//No other dice found under FOV, camera blocking !!!
}


LONG CBondPr::OpBPRReSrchDie(PR_SRCH_DIE_RPY2 &stSrchRpy2, PR_SRCH_DIE_RPY3 &stSrchRpy3)
{
	if (!AutoSearchDie())
		return 0;

	if (!AutoGrabDieDone())
		return 0;

	//PR_SRCH_DIE_RPY2	stSrchRpy2;
	//PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
//	PR_REAL				fDieRotate; 
//	PR_COORD			stDieOffset; 
//	PR_REAL				fDieScore;
	BOOL				bGetRpy3 = FALSE;

	usSearchResult = SearchDieRpy2(&stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		//CString csMsg;
		//csMsg.Format("Get reply 2 error! = %x",usSearchResult);
		//DisplayMessage(csMsg);	
		return 0;
	}

    usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }

	if (DieIsAlignable(usDieType) != TRUE)
	{
		return 0;
	}

	usSearchResult = SearchDieRpy3(&stSrchRpy3);
	if (usSearchResult == PR_COMM_ERR)
	{
		return 0;
	}
	
	return 1;
}

//use in Autobond sequence (Grab Bond image)
BOOL CBondPr::AutoGrabImage(VOID)
{
	if (m_bDisableBT)	//v3.64
		return TRUE;

	PR_GRAB_DISP_IMG_EXT_CMD	stBondImageCmd;
	PR_GRAB_DISP_IMG_EXT_RPY	stRpy;

	PR_InitGrabDispImgExtCmd(&stBondImageCmd);
	stBondImageCmd.emGrab		= PR_TRUE;
	stBondImageCmd.emDisplay	= PR_TRUE;
	stBondImageCmd.emPurpose	= PR_PURPOSE_CAM_A1;
//	stBondImageCmd.emShareGrab	= PR_TRUE;

	PR_GrabDispImgExtCmd(&stBondImageCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stRpy);
	if (stRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		CString csMsg;
		csMsg.Format("BPR: PR_GrabDispImgExtCmd comm error = 0x%x", stRpy.uwCommunStatus);
		SetErrorMessage(csMsg);		//v3.93

		HmiMessage_Red_Yellow(csMsg, "Bond PR");
		return FALSE;		
	}

	return TRUE;
}


BOOL CBondPr::AutoGrabDone(VOID)
{
	if (m_bDisableBT)	//v3.64
		return TRUE;

	PR_GRAB_DISP_IMG_EXT_RPY2	stBondImageRpy;
	PR_GrabDispImgExtRpy(MS899_BOND_CAM_SEND_ID, &stBondImageRpy);

	if (stBondImageRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		CString csMsg;
		csMsg.Format("BPR: PR_GrabDispImgExtRpy comm error = 0x%x",stBondImageRpy.stStatus.uwCommunStatus);
		SetErrorMessage(csMsg);		//v3.93
		
		HmiMessage_Red_Yellow(csMsg, "Bond PR");
		return FALSE;
	}

	if (stBondImageRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
	{
		CString csMsg;
		csMsg.Format("BPR: PR_GrabDispImgExtRpy PR error = 0x%x", stBondImageRpy.stStatus.uwPRStatus);
		SetErrorMessage(csMsg);		//v3.93

		HmiMessage_Red_Yellow(csMsg, "Bond PR");
		return FALSE;
	}

	return TRUE;
}


PR_WORD CBondPr::AutoSearchCircle(LONG pcx, LONG pcy, LONG ecx, LONG ecy, UINT &nX, UINT &nY) 
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
	pstCmd.uwRecordID = m_ssGenPRSrchID[BPR_GEN_RDIE_OFFSET+3];
	pstCmd.rDetectScore = 70;
	pstCmd.emSelectMode = PR_SHAPE_SELECT_MODE_CENTRE;
	pstCmd.coExpectCentre = expectcenter;
	pstCmd.coPositionVar = prcenter;


	PR_DetectShapeCmd(&pstCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &pstRpy1);

	if ( (pstRpy1.uwCommunStatus != PR_COMM_NOERR) || (pstRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("BPR PR_DetectShapeCmd Error1: COMM = %d, PR = %d", pstRpy1.uwCommunStatus, pstRpy1.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}
	
	PR_DetectShapeRpy(MS899_BOND_CAM_SEND_ID, &pstRpy2);

	if ( (pstRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) || (pstRpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("BPR PR_DetectShapeRpy Error2: COMM = %d, PR = %d", pstRpy2.stStatus.uwCommunStatus, pstRpy2.stStatus.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}

	m_pPrGeneral->DrawSmallCursor(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, 
								  (PR_WORD)_round(pstRpy2.stShapeRes.rcoCentre.x),
								  (PR_WORD)_round(pstRpy2.stShapeRes.rcoCentre.y));

	if ( pstRpy2.emShapeFound != PR_SHAPE_FOUND_CIRCLE &&  pstRpy2.emShapeFound != PR_SHAPE_FOUND_ELLIPSE )
	{
		HmiMessageEx("Find circle error!!!!!");
		return PR_FALSE;
	}
	
	if (pstRpy2.emShapeFound == PR_SHAPE_FOUND_CIRCLE)
	{	
		CString szLog;
		nX = (UINT) pstRpy2.stCircleRes.coCentre.x;
		nY = (UINT) pstRpy2.stCircleRes.coCentre.y;
		szLog.Format("Centre coordinate : %d, %d", pstRpy2.stCircleRes.coCentre.x, pstRpy2.stCircleRes.coCentre.y);
		//HmiMessageEx(szLog);
		return PR_TRUE;
	}
	else if ( pstRpy2.emShapeFound == PR_SHAPE_FOUND_ELLIPSE )
	{
		CString szLog;
		nX = (UINT) pstRpy2.stShapeRes.rcoCentre.x;
		nY = (UINT) pstRpy2.stShapeRes.rcoCentre.y;
		szLog.Format("Centre coordinate : %d, %d", nX, nY);
		//HmiMessageEx(szLog);
		return PR_TRUE;
	}

	return PR_FALSE;
}

BOOL CBondPr::PrescanAutoMultiGrabDone(UCHAR ucCorner, CDWordArray &dwList, BOOL bDrawDie, BOOL bIsFindGT) //4.51D20
{
	if( IsBurnIn() && IsAOIOnlyMachine() )
	{
		return TRUE;
	}

	BOOL bReturn = ScanAutoMultiSearch5M(ucCorner, dwList, bDrawDie, bIsFindGT);
	return bReturn;
}

BOOL CBondPr::ScanAutoMultiSearch5M(UCHAR ucCorner, CDWordArray &dwList, BOOL bDrawDie, BOOL bIsFindGT) //4.51D20
{
	CString szMsg;

	LONG lX = (LONG)(*m_psmfSRam)["BinTable"]["Current"]["X"];
	LONG lY = (LONG)(*m_psmfSRam)["BinTable"]["Current"]["Y"];

	#define	FOV_5M_ROW_COL	100
	typedef	struct
	{
		BOOL	bFound;
		LONG	lDieX;
		LONG	lDieY;
		DOUBLE	dDieAngle;
		CString	szDieBin;
	}	ScanDieResult;
	ScanDieResult asDieList[FOV_5M_ROW_COL][FOV_5M_ROW_COL];

	//PR_UBYTE	ubSenderID	 =  GetScnSenID();
	//PR_UBYTE	ubReceiverID =  GetScnRecID();
	PR_UBYTE	ubSenderID	 =  MS899_BOND_CAM_SEND_ID;
	PR_UBYTE	ubReceiverID =  MS899_BOND_CAM_RECV_ID;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search --- 5M multi search prepare", "a+");

	PR_GRAB_SHARE_IMAGE_CMD		stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);
	// AOI partial die step 3 During Prescan, 
	//stGrbCmd.emPurpose			= GetScnPurpose();
	stGrbCmd.emPurpose			= MS899_BOND_GEN_PURPOSE;
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stMultiSrchCmd.auwRecordID[0];
	//stGrbCmd.uwRecordID			= m_ssPSPRSrchID;		//Not Sure that is right variable
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_NORMAL;

	PR_GRAB_SHARE_IMAGE_RPY		stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		szMsg = "Search --- grab share image cmd fail";
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		
		HmiMessage(szMsg);
		return FALSE;
	}

	PR_GRAB_SHARE_IMAGE_RPY2	stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("Search --- grab share image rpy fail, stGrbRpy2.stStatus.uwCommunStatus:%d, stGrbRpy2.stStatus.uwPRStatus:%d",stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		HmiMessage(szMsg);
		return FALSE;
	}

	int nSubImageNum = m_uwFrameSubImageNum;  //Not sure how to desgin 
	//int nSubImageNum = 1;
	if( bIsFindGT && GetMapIndexStepRow()>10 && GetMapIndexStepCol()>10 )
	{
		nSubImageNum = 1;
	}
	//FILE *fp = NULL;
	//CTime ctDateTime;
	//ctDateTime = CTime::GetCurrentTime();
	//CString szTime;
	//szTime = ctDateTime.Format("%Y%m%d%H%M%S");
	//if( pUtl->GetPrescanDebug() )
	//{
		//WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();  //Not Sure how to change the WT_CPO_PointsInfo class
		//CString szLogFileName;
		//szLogFileName.Format("%s\\History\\FovOneSearch_DC%d", gszUSER_DIRECTORY, pCPInfo->GetDCState());
		//szLogFileName = szLogFileName + ".txt";
		//fp = fopen(szLogFileName, "w");
		//if( fp!=NULL )
		//{
		//	fprintf(fp, "col,row,prx,pry,offx,offy,posnx,posny,via clb,%d\n", nSubImageNum);
		//}
	//}
	szMsg.Format("Search --- Sub Image No:%d", nSubImageNum);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search --- Start find out all dice posn", "a+");

	for(int i = 1; i<=nSubImageNum; i++)
	{
		PR_UWORD uwRpy1Status, uwRpy1ComStt, uwRpy2ComStt;
		IMG_UWORD		uwCommStatus;
		PR_WORD lULX = PR_MAX_WIN_ULC_X, lULY = PR_MAX_WIN_ULC_Y, lLRX = PR_MAX_WIN_LRC_X, lLRY = PR_MAX_WIN_LRC_Y;
	//	GetSubWindow(i, lULX, lULY, lLRX, lLRY);  // later design
		// bIsFindGT is always FALSE
		//if( bIsFindGT && GetMapIndexStepRow()>10 && GetMapIndexStepCol()>10 && GetDieSizeX()!=0 && GetDieSizeY()!=0 )
		//{
			//PR_WORD wScanPitchPixelX = GetScanNmlSizePixelX()*GetDiePitchX_X()*5/GetDieSizeX();
			//PR_WORD wScanPitchPixelY = GetScanNmlSizePixelY()*GetDiePitchY_Y()*5/GetDieSizeY();

			//lULX = PR_DEF_CENTRE_X - wScanPitchPixelX;
			//lLRX = PR_DEF_CENTRE_X + wScanPitchPixelX;
			//lULY = PR_DEF_CENTRE_Y - wScanPitchPixelY;
			//lLRY = PR_DEF_CENTRE_Y + wScanPitchPixelY;
			//if (lULX < GetPRWinULX())
			//{
			//	lULX = GetPRWinULX();
			//}
			//if (lLRX > GetPRWinLRX())
			//{
			//	lLRX = GetPRWinLRX();
			//}
			//if (lULY < GetPRWinULY())
			//{
			//	lULY = GetPRWinULY();
			//}
			//if (lLRY > GetPRWinLRY())
			//{
			//	lLRY = GetPRWinLRY();
			//}
			//if( fp!=NULL )
			//{
			//	fprintf(fp, "%d,%d,%d,%d,%d,%d\n", lULX, lULY, lLRX, lLRY, GetScanNmlSizePixelX(), GetScanNmlSizePixelY());
			//}
		//}
		PR_WORD lCTX = (lULX + lLRX)/2;
		PR_WORD lCTY = (lULY + lLRY)/2;

		if( bDrawDie )
		{
			PR_COLOR sColor = PR_COLOR_YELLOW;
			if( i%2==0 )
				sColor = PR_COLOR_CYAN;
			PR_COORD stCorner1;
			PR_COORD stCorner2;
			stCorner1.x = lULX;
			stCorner1.y = lULY;
			stCorner2.x = lLRX;
			stCorner2.y = lLRY;
			DrawRectangleBox(stCorner1, stCorner2, sColor);
		}

		PR_SRCH_DIE_CMD stInspCmd	= m_stMultiSrchCmd;  //used the m_stMultiSrchCmd

		stInspCmd.ulLatchImageID	= ulImageID;
		stInspCmd.uwLatchStationID	= (PR_UWORD) ulStationID;

		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;

		stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
		stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= 1000;//BOND_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= 1000;//BOND_ONE_SEARCH_MAX_DIE;
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= 1000;//BOND_ONE_SEARCH_MAX_DIE;
		PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);

		if ( uwCommStatus != PR_COMM_NOERR )
		{
			szMsg.Format("Search --- PR_MultiSrchDieCmd finished with status %u!", uwCommStatus);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			HmiMessage(szMsg);
			//v4.39T10	//Klocwork
			//if( fp!=NULL )
			//{
			//	fprintf(fp, "\n\n");
			//	fclose(fp);
			//}
			return FALSE;
		}

		PR_SRCH_DIE_RPY1 stInspRpy1;
		PR_SrchDieRpy1(ubSenderID, &stInspRpy1);
		uwRpy1ComStt = stInspRpy1.uwCommunStatus;
		uwRpy1Status = stInspRpy1.uwPRStatus;

		if (uwRpy1ComStt != PR_COMM_NOERR || PR_ERROR_STATUS(uwRpy1Status))
		{
			szMsg.Format("Search --- PR MultiSrchDieRpy1 finished with status %u, %u!\n",	uwRpy1ComStt, uwRpy1Status);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			HmiMessage(szMsg);
			//v4.39T10	//Klocwork
			//if( fp!=NULL )
			//{
			//	fprintf(fp, "\n\n");
			//	fclose(fp);
			//}
			return FALSE;
		}

		PR_UWORD		usDieType;
		PR_SRCH_DIE_RPY2	stInspRpy2;

		stInspRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*)malloc (stInspCmd.uwHostMallocMaxNoOfDieInRpy2*sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign == NULL)
		{
			szMsg = "Search --- Init stDieAlign null pointer";
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			HmiMessage(szMsg);
			break; 
		}
		//#2 Alignment
		PR_SrchDieRpy2(ubSenderID, &stInspRpy2);
		uwRpy2ComStt = stInspRpy2.stStatus.uwCommunStatus;
		if (uwRpy2ComStt != PR_COMM_NOERR)
		{
			if (stInspRpy2.pstDieAlign != NULL)
			{
				free(stInspRpy2.pstDieAlign);
			}
			szMsg.Format("Search --- PR MultiSrchDieRpy2 finished with status %u!\n", uwRpy2ComStt);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			HmiMessage(szMsg);
			break;
		}
		usDieType = stInspRpy2.stStatus.uwPRStatus;

		PR_SRCH_DIE_RPY3	stInspRpy3;
		PR_SRCH_DIE_RPY4	stInspRpy4;
		if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
		{
			stInspRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*)malloc( stInspCmd.uwHostMallocMaxNoOfDieInRpy3*sizeof(PR_DIE_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin==NULL )
			{
				if (stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				szMsg = "Search --- Rpy3 Init stDieAlign null pointer";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

				HmiMessage(szMsg);
				break; 
			}
		}
		if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
		{
			stInspRpy4.pstPostBondInspMin = (PR_POST_INSP_MINIMAL_RPY*)malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy4*sizeof(PR_POST_INSP_MINIMAL_RPY));
			if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin==NULL )
			{
				if (stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				szMsg = "Search --- Pry4 Init stDieAlign null pointer";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				HmiMessage(szMsg);
				break; 
			}
		}

		if( ((PR_TRUE==m_stMultiSrchCmd.emDefectInsp) || (PR_TRUE==m_stMultiSrchCmd.emPostBondInsp)) &&
			(PR_COMM_NOERR==uwRpy2ComStt) && !PR_ERROR_STATUS(usDieType) )
		{
			//#3 Defect Inspection
			PR_SrchDieRpy3(ubSenderID,&stInspRpy3);
			PR_UWORD uwRpy3ComStt = stInspRpy3.stStatus.uwCommunStatus;
			if (uwRpy3ComStt != PR_COMM_NOERR)
			{
				if (stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
				{
					free(stInspRpy4.pstPostBondInspMin);
				}	// free point for one search
				szMsg.Format("Search --- PR MultiSrchDieRpy3 finished with status %u!\n",	uwRpy3ComStt);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				HmiMessage(szMsg);
				break;
			}

			PR_UWORD uwRpy3Status = stInspRpy3.stStatus.uwPRStatus;

			// inspection result
			if( (PR_TRUE == m_stMultiSrchCmd.emPostBondInsp) &&
				PR_COMM_NOERR == uwRpy3ComStt &&
				!PR_ERROR_STATUS(uwRpy3Status) )
			{
				//#4 Postbond Inspection
				PR_SrchDieRpy4(ubSenderID,&stInspRpy4);

				PR_UWORD uwRpy4ComStt = stInspRpy4.stStatus.uwCommunStatus;
				if (uwRpy4ComStt != PR_COMM_NOERR)
				{
					if (stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3>0 && stInspRpy3.pstDieInspExtMin!=NULL )
					{
						free(stInspRpy3.pstDieInspExtMin);
					}
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4>0 && stInspRpy4.pstPostBondInspMin!=NULL )
					{
						free(stInspRpy4.pstPostBondInspMin);
					}	// free point for one search
					szMsg.Format("Search --- PR_MultiSrchDieRpy4 finished with status %u!\n", uwRpy4ComStt);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
					HmiMessage(szMsg);
					break;
				}
			}
		}

		USHORT usDieSum = stInspRpy2.uwNResults;
		//if( fp!=NULL )
		//{
		//	fprintf(fp, "found total dice is %d\n", usDieSum);
		//	fprintf(fp, "CalibX,%f,%f, calibY(%f,%f\n", 
		//		GetScanCalibX(), GetScanCalibXY(), GetScanCalibY(), GetScanCalibYX());
		//}
		szMsg.Format("Search --- found total dice is %d, CalibX(%f,%f), calibY(%f,%f)", usDieSum, GetScanCalibX(), GetScanCalibXY(), GetScanCalibY(), GetScanCalibYX());
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

//find the corner die;
		LONG lCornerIndex = -1;
		double dXMin = 0, dXMax = 0;
		double dYMin = 0, dYMax = 0;
		switch (ucCorner)
		{
			case REALIGN_UPPER_LEFT_CORNER:
				dXMax = 0;
				dYMax = 0;					
 				break;

			case REALIGN_UPPER_RIGHT_CORNER:
				dXMin = 8192;
				dYMax = 0;
				break;

			case REALIGN_LOWER_LEFT_CORNER:
				dXMax = 0;
				dYMin = 8192;
				break;

			case REALIGN_LOWER_RIGHT_CORNER:
				dXMin = 8192;
				dYMin = 8192;
				break;
		}

		//ucCorner == 5 shows that it is only display all searched dice
		if (ucCorner < 5)
		{
			int nPitchX	 =  GetScanNmlSizePixelX();
			int nPitchY	 =  GetScanNmlSizePixelY();

			for (USHORT usIndex = 0; usIndex < usDieSum; usIndex++)
			{
				PR_RCOORD	rcDieCtr = stInspRpy2.pstDieAlign[usIndex].rcoDieCentre;
				switch (ucCorner)
				{
					case REALIGN_UPPER_LEFT_CORNER:
						if (fabs(rcDieCtr.y  - dYMax) >= fabs(nPitchY / 2.0))
						{
							if (rcDieCtr.y  > dYMax)
							{
								lCornerIndex = usIndex;
								dXMax = rcDieCtr.x;
								dYMax = rcDieCtr.y;
							}
						}
						else if (fabs(rcDieCtr.x  - dXMax) >= fabs(nPitchX / 2.0))
						{
							if (rcDieCtr.x  > dXMax)
							{
								lCornerIndex = usIndex;
								dXMax = rcDieCtr.x;
								dYMax = rcDieCtr.y;
							}
						}
 						break;

					case REALIGN_UPPER_RIGHT_CORNER:
						if (fabs(rcDieCtr.y  - dYMax) >= fabs(nPitchY / 2.0))
						{
							if (rcDieCtr.y  > dYMax)
							{
								lCornerIndex = usIndex;
								dXMin = rcDieCtr.x;
								dYMax = rcDieCtr.y;
							}
						}
						else if (fabs(rcDieCtr.x  - dXMin) >= fabs(nPitchX / 2.0))
						{
							if (rcDieCtr.x  < dXMin)
							{
								lCornerIndex = usIndex;
								dXMin = rcDieCtr.x;
								dYMax = rcDieCtr.y;
							}
						}
 						break;

					case REALIGN_LOWER_LEFT_CORNER:
						if (fabs(rcDieCtr.y  - dYMin) >= fabs(nPitchY / 2.0))
						{
							if (rcDieCtr.y  < dYMin)
							{
								lCornerIndex = usIndex;
								dXMax = rcDieCtr.x;
								dYMin = rcDieCtr.y;
							}
						}
						else if (fabs(rcDieCtr.x  - dXMax) >= fabs(nPitchX / 2.0))
						{
							if (rcDieCtr.x  > dXMax)
							{
								lCornerIndex = usIndex;
								dXMax = rcDieCtr.x;
								dYMin = rcDieCtr.y;
							}
						}
						break;

					case REALIGN_LOWER_RIGHT_CORNER:
						if (fabs(rcDieCtr.y  - dYMin) >= fabs(nPitchY / 2.0))
						{
							if (rcDieCtr.y  < dYMin)
							{
								lCornerIndex = usIndex;
								dXMin = rcDieCtr.x;
								dYMin = rcDieCtr.y;
							}
						}
						else if (fabs(rcDieCtr.x  - dXMin) >= fabs(nPitchX / 2.0))
						{
							if (rcDieCtr.x  < dXMin)
							{
								lCornerIndex = usIndex;
								dXMin = rcDieCtr.x;
								dYMin = rcDieCtr.y;
							}
						}
						break;
				}
			}
		}

		for(USHORT usIndex = 0; usIndex < usDieSum; usIndex++)
		{
			CString szDieBin = "0";
			PR_RCOORD	rcDieCtr = stInspRpy2.pstDieAlign[usIndex].rcoDieCentre;
			PR_REAL		dDieRot  = stInspRpy2.pstDieAlign[usIndex].rDieRot;
			BOOL bFound = FALSE;
			if (lCornerIndex >= 0)
			{
				bFound = (usIndex == (USHORT)lCornerIndex) ? TRUE : FALSE;
			}

			if( IsMS90HalfSortMode() )
			{
				if (dDieRot > 90)
				{
					dDieRot -= 180;
				}
				else if (dDieRot < -90)
				{
					dDieRot += 180;
				}
			}

			PR_UWORD uwRpy3Status = 0, uwRpy4Status = 0;
			if( m_stMultiSrchCmd.emDefectInsp==PR_TRUE )
			{
				uwRpy3Status = stInspRpy3.pstDieInspExtMin[usIndex].uwPRStatus;
			}

			if( m_stMultiSrchCmd.emPostBondInsp==PR_TRUE )
			{
				uwRpy4Status = stInspRpy4.pstPostBondInspMin[usIndex].uwPRStatus;
			}

			int siStepX = 0, siStepY = 0;
			CalcScanDiePosition(lX, lY, rcDieCtr, siStepX, siStepY);
			
			//Set all Dies position
			SetMultiDiePosition(usIndex, siStepX, siStepY, dDieRot, szDieBin, 0, 0);

			if (bFound && (bIsFindGT == FALSE))
			{
				LONG lPosn = GetDieCoordinate(rcDieCtr);
				if (lPosn != 4)
				{
					if( lPosn>10000 )
					{
						lPosn = lPosn - 10000;
						dwList.SetAt(3, 1); // "1: mean have detected having dies
						//szMsg.Format("lPosn:%d,dwList.GetAt(3):%d", lPosn, dwList.GetAt(3));
					}
					if( lPosn>1000 )
					{
						lPosn = lPosn - 1000;
						dwList.SetAt(1, 1);
						//szMsg.Format("lPosn:%d,dwList.GetAt(1):%d", lPosn, dwList.GetAt(1));
					}

					if( lPosn>100 )
					{
						lPosn = lPosn - 100;
						dwList.SetAt(2, 1);
						//szMsg.Format("lPosn:%d,dwList.GetAt(2):%d", lPosn, dwList.GetAt(2));
					}
					if( lPosn>10 )
					{
						lPosn = lPosn - 10;
						dwList.SetAt(0, 1);
						//szMsg.Format("lPosn:%d,dwList.GetAt(0):%d", lPosn, dwList.GetAt(0));
					}
				}
				else if (usDieSum == 1)
				{
					//Only one Die
					switch (ucCorner)
					{
						case REALIGN_UPPER_LEFT_CORNER:
							dwList.SetAt(0, 1);  //nDieStateLeft
							dwList.SetAt(1, 1);  //nDieStateUp
							dwList.SetAt(2, 0);  //nDieStateRight
							dwList.SetAt(3, 0);  //nDieStateDown
							break;

						case REALIGN_UPPER_RIGHT_CORNER:
							dwList.SetAt(0, 0);  //nDieStateLeft
							dwList.SetAt(1, 1);  //nDieStateUp
							dwList.SetAt(2, 1);  //nDieStateRight
							dwList.SetAt(3, 0);  //nDieStateDown
							break;

						case REALIGN_LOWER_LEFT_CORNER:
							dwList.SetAt(0, 1);  //nDieStateLeft
							dwList.SetAt(1, 0);  //nDieStateUp
							dwList.SetAt(2, 0);  //nDieStateRight
							dwList.SetAt(3, 1);  //nDieStateDown
							break;

						case REALIGN_LOWER_RIGHT_CORNER:
							dwList.SetAt(0, 0);  //nDieStateLeft
							dwList.SetAt(1, 0);  //nDieStateUp
							dwList.SetAt(2, 1);  //nDieStateRight
							dwList.SetAt(3, 1);  //nDieStateDown
							break;
					}
				}
				else
				{
					dwList.SetAt(4, 1);
					//szMsg.Format("lPosn:%d,dwList.GetAt(4):%d", lPosn, dwList.GetAt(4));
				}

				
				//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			}

		   if ((bFound || ucCorner == 5) && bDrawDie)
		   {
				PR_COLOR sColor;
				sColor = PR_COLOR_GREEN;//Assume sColor = PR_COLOR_GREEN;

				PR_COORD stCorner1;
				PR_COORD stCorner2;
				if( GetDieShape()==BPR_RHOMBUS_DIE )
				{
					stCorner1.x	= (PR_WORD)_round(rcDieCtr.x - GetScanNmlSizePixelX() / 4);
					stCorner1.y	= (PR_WORD)_round(rcDieCtr.y - GetScanNmlSizePixelY() / 4);
					stCorner2.x = (PR_WORD)_round(rcDieCtr.x + GetScanNmlSizePixelX() / 4);
					stCorner2.y = (PR_WORD)_round(rcDieCtr.y + GetScanNmlSizePixelY() / 4);
				}
				else
				{
					stCorner1.x	= (PR_WORD)_round(rcDieCtr.x - GetScanNmlSizePixelX() / 2);
					stCorner1.y	= (PR_WORD)_round(rcDieCtr.y - GetScanNmlSizePixelY() / 2);
					stCorner2.x = (PR_WORD)_round(rcDieCtr.x + GetScanNmlSizePixelX() / 2);
					stCorner2.y = (PR_WORD)_round(rcDieCtr.y + GetScanNmlSizePixelY() / 2);
				}

				
				DrawRectangleBox(stCorner1, stCorner2, sColor);
			}
		} //end of for loop


		// For finding out the upper right ist Die position only
		CompareDiesPositionToFindFirstDies(ucCorner, usDieSum);

		//Free the reply 2
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stInspRpy2.pstDieAlign != NULL)
		{
			free(stInspRpy2.pstDieAlign);
		}
		//Free the reply 3
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stInspRpy3.pstDieInspExtMin!=NULL )
		{
			free(stInspRpy3.pstDieInspExtMin);
		}
		//Free the reply 4
		if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4 > 0 && stInspRpy4.pstPostBondInspMin!=NULL )
		{
			free(stInspRpy4.pstPostBondInspMin);
		}	// free point for one search
	}

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search --- 5M multi search finish and remove image", "a+");
	
	PR_REMOVE_SHARE_IMAGE_CMD     stRmvCmd;
	PR_REMOVE_SHARE_IMAGE_RPY     stRmvRpy;

	PR_InitRemoveShareImgCmd(&stRmvCmd);
	stRmvCmd.ulImageID = ulImageID;
	stRmvCmd.emFreeAll = PR_TRUE; // You can set this to PR_TRUE if you want to clear all Share Image for this channel
	PR_RemoveShareImgCmd(&stRmvCmd, ubSenderID, ubReceiverID, &stRmvRpy);

	if (PR_COMM_NOERR != stRmvRpy.uwCommunStatus)
	{
		szMsg = "Search --- One Search fail to delete the image";
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		HmiMessage(szMsg);
	}

	return TRUE;
}

LONG CBondPr::GetDieCoordinate(PR_RCOORD coDieCtr) //4.51D20
{
	//int nPitchX = GetScanNmlSizePixelX();
	//int nPitchY = GetScanNmlSizePixelY();
	int nPitchX	 =  GetScanNmlSizePixelX();//20170824 Leo m_lDiePitchX;
	int nPitchY	 =  GetScanNmlSizePixelY();// m_lDiePitchY;
	int nPosnX = (int) coDieCtr.x;
	int nPosnY = (int) coDieCtr.y;

	LONG lValue = 4;

	if (nPosnX < (GetPrCenterX() - nPitchX))
	{
		lValue += 10;
	}

	if (nPosnX > (GetPrCenterX() + nPitchX))
	{
		lValue += 100;
	}

	if (nPosnY < (GetPrCenterY() - nPitchY))
	{
		lValue += 1000;
	}

	if (nPosnY > (GetPrCenterY() + nPitchY))
	{
		lValue += 10000;
	}
	//20170824 Leo
	CString szMsg;
	szMsg.Format("Die X %d,Y %d,PRX %d, PRY %d,PX %d,PY %d,Value %d)", nPosnX,nPosnY,GetPrCenterX(),GetPrCenterY(),nPitchX,nPitchY,lValue); 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	return lValue;
}

VOID CBondPr::CalcScanDiePosition(CONST LONG lGrabX, CONST LONG lGrabY, PR_RCOORD stDieOffset, INT &siStepX, INT &siStepY)  //4.51D20
{
	//WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bFindNear = FALSE;

	CString szGridDCLog;

	if( bFindNear==FALSE )
	{
		if( stDieOffset.x<0 || stDieOffset.x>8192 || 
			stDieOffset.y<0 || stDieOffset.y>8192 )
		{
			siStepX = 0;
			siStepY = 0;

			szGridDCLog.Format("Search --- PR abnormal at (%f,%f), PR center (%ld,%ld), calibX(%f,%f) calibY(%f,%f), NORMAL,%d,%d, grab %d,%d, die posn %d,%d", 
				stDieOffset.x, stDieOffset.y, GetPrCenterX(), GetPrCenterY(), GetScanCalibX(), GetScanCalibXY(), GetScanCalibY(), GetScanCalibYX(), 
				siStepX, siStepY, lGrabX, lGrabY, lGrabX+siStepX, lGrabY+siStepY);
			//pUtl->PrescanMoveLog(szGridDCLog);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szGridDCLog, "a+");
		}
		else
			RCalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		szGridDCLog.Format("Search --- Cal Scan: DF(%f,%f), Calib(%f,%f), NORMAL(%d,%d), Grab(%d,%d), DiePosn(%d,%d)", 
			stDieOffset.x, stDieOffset.y, GetScanCalibX(), GetScanCalibY(), siStepX, siStepY,
			lGrabX, lGrabY, lGrabX+siStepX, lGrabY+siStepY);
	}

	//CalculateNDieOffsetXY(siStepX, siStepY);  //later desgin

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szGridDCLog, "a+");

}

VOID CBondPr::CalculateNDieOffsetXY(INT &siStepX, INT &siStepY)	//3.77 Cree  //4.51D20
{
	//if ((m_lNDieOffsetInMotorStepX != 0))
	//{
	//	siStepX = siStepX + m_lNDieOffsetInMotorStepX;			//all in motor steps
	//}
	//if ((m_lNDieOffsetInMotorStepY != 0))
	//{
	//	siStepY = siStepY + m_lNDieOffsetInMotorStepY;			//all in motor steps
	//}
}



void CBondPr::GetSubWindow(LONG lIndex, PR_WORD &lULX, PR_WORD &lULY, PR_WORD &lLRX, PR_WORD &lLRY) //4.51D20
{
	ULONG ulIndex = lIndex;
	if( ulIndex>=100 )
		ulIndex = 99;
	if( ulIndex<1 )
		ulIndex = 1;

	lULX = m_stSubImageWindow[ulIndex].coCorner1.x;
	lULY = m_stSubImageWindow[ulIndex].coCorner1.y;
	lLRX = m_stSubImageWindow[ulIndex].coCorner2.x;
	lLRY = m_stSubImageWindow[ulIndex].coCorner2.y;
}

VOID CBondPr::RCalculateDieCompenate(PR_RCOORD stDieOffset, int *siStepX, int *siStepY)  //4.51D20
{
	PR_RCOORD	stRelMove;

	stRelMove.x = (PR_REAL)GetPrCenterX() - stDieOffset.x;
	stRelMove.y = (PR_REAL)GetPrCenterY() - stDieOffset.y;

	double dCalibX = GetScanCalibX();
	double dCalibY = GetScanCalibY();
	double dCalibXY= GetScanCalibXY();
	double dCalibYX= GetScanCalibYX();

	RConvertPixelToMotorStep(stRelMove, siStepX, siStepY, dCalibX, dCalibY, dCalibXY, dCalibYX);
}

VOID CBondPr::RConvertPixelToMotorStep(PR_RCOORD stPixel, int *siStepX, int *siStepY, 
									   double dCalibX, double dCalibY, double dCalibXY, double dCalibYX)  //4.51D20
{
	*siStepX = (int)((DOUBLE)stPixel.x * dCalibX + (DOUBLE)stPixel.y * dCalibXY);
	*siStepY = (int)((DOUBLE)stPixel.y * dCalibY + (DOUBLE)stPixel.x * dCalibYX);
}

BOOL CBondPr::IsThisDieLearnt(UCHAR ucDieIndex) //4.51D20
{
	//if( IsBurnIn() && m_bNoWprBurnIn )
	if( IsBurnIn())  // Not sure m_bNoWprBurnIn -- CP 
	{
		return TRUE;
	}

	ucDieIndex = min(ucDieIndex, BPR_MAX_DIE-1);
	return (m_bGenDieLearnt[ucDieIndex] && (m_ssGenPRSrchID[ucDieIndex] > 0));
}

INT	CBondPr::OpPrescanInit(UCHAR ucDieNo, BOOL bFastHomeMerge) //4.51D20
{
	if (IsAOIOnlyMachine())
	{
		SaveScanTimeEvent("BPR: init prescan PR, resume pr");
	}

	if (IsThisDieLearnt(ucDieNo - 1) != TRUE)
	{
		CString szMsg;
		szMsg.Format("Normal die PR %d not learnt yet!", ucDieNo);
		HmiMessage_Red_Back(szMsg);
		return gnNOTOK;
	}

	remove(gszUSER_DIRECTORY + "\\History\\FovSearch_Map.txt");
	if (IsAOIOnlyMachine())
	{
		SaveScanTimeEvent("BPR: init prescan PR, setup multi search die cmd");
	}

	BOOL bInspect = TRUE; //Assume Temperary TRUE from  m_bAoiPrescanInspction //not sure 
	if (GetNewPickCount() > 0)
	{
		bInspect = FALSE;
	}

	if (!SetupMultiSearchDieCmd(ucDieNo, bFastHomeMerge, bInspect, TRUE))
	{
		return gnNOTOK;
	}

	//if( IsEnableZoom() )
	//{
	//	CalculateDieInView(TRUE);	//	Init prescan  //Not sure how to design
	//}
 

	CString szMsg;
	szMsg.Format("Search --- Init: prescan PR ucDieNo: %d, setup multi search die cmd inspec=%d done, SubWin:%d", ucDieNo, bInspect, m_uwFrameSubImageNum);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	SaveScanTimeEvent(szMsg);

	return gnOK;
}

BOOL CBondPr::SetupMultiSearchDieCmd(LONG lInputDieNo, BOOL bFastHomeMerge, BOOL bInspect, BOOL bUseScanAngle) // 4.51D20
{
	CString szMsg;
	szMsg = "Search --- Start Test Setup Multi Search Die Cmd";
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	PR_BOOLEAN	bBackupAlign = PR_FALSE;
	PR_BOOLEAN	bCheckChip = PR_TRUE;
	PR_BOOLEAN	bCheckDefect = PR_TRUE;
	PR_UBYTE	ucDefectThres = PR_NORMAL_DEFECT_THRESHOLD;
	LONG		lAlignAccuracy;
	LONG		lGreyLevelDefect;
	DOUBLE		dMinChipArea = 0.0;
	DOUBLE		dSingleDefectArea = 0.0;
	DOUBLE		dTotalDefectArea = 0.0;
	
	PR_UBYTE	ubSenderID	 =  MS899_BOND_CAM_SEND_ID;//PR_UBYTE	ubSenderID	 =  GetScnSenID();
	PR_UBYTE	ubReceiverID =  MS899_BOND_CAM_RECV_ID;//PR_UBYTE	ubReceiverID =  GetScnRecID();

	UCHAR							lDieNo = 0;
	PR_UWORD						lPrSrchID;
	if (lInputDieNo <= 0)
		lInputDieNo = 1;
	
	lDieNo		= BPR_GEN_NDIE_OFFSET + (UCHAR)lInputDieNo;//lDieNo	= WPR_GEN_NDIE_OFFSET + (UCHAR)lInputDieNo;
	lPrSrchID	= GetDiePrID(lDieNo);

	if (lPrSrchID <= 0)
	{
		CString szMsg;
		szMsg = _T("Multi-Search PR ID Not Leant");
		HmiMessage_Red_Back(szMsg);
		return FALSE;
	}
	
	if (lDieNo >= BPR_MAX_DIE)		//Klocwork	//v4.46 //if (lDieNo >= WPR_MAX_DIE)
	{
		return FALSE;
	}

	switch(m_lGenSrchAlignRes[lDieNo])
	{
	case 1:		
		lAlignAccuracy	= PR_LOW_DIE_ALIGN_ACCURACY;	
		break;

	default:	
		lAlignAccuracy	= PR_HIGH_DIE_ALIGN_ACCURACY;	
		break;
	}

	switch(m_lGenSrchGreyLevelDefect[lDieNo])
	{
	case 1: 
		lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_BRIGHT;
		break;
	
	case 2:
		lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_DARK;
		break;

	default:
		lGreyLevelDefect = PR_DEFECT_ATTRIBUTE_BOTH;
		break;
	}

	ucDefectThres = (PR_UBYTE)m_lGenSrchDefectThres[lDieNo];

	if (m_dGenSrchChipArea[lDieNo] == 0.0)
	{
		bCheckChip = PR_FALSE;
	}

	if (m_bGenSrchEnableChipCheck[lDieNo] == FALSE)
	{
		bCheckChip = PR_FALSE;
	}

	if ((m_dGenSrchSingleDefectArea[lDieNo] == 0.0) || (m_dGenSrchTotalDefectArea[lDieNo] == 0.0))
	{
		bCheckDefect = PR_FALSE;
	}

	if (m_bGenSrchEnableDefectCheck[lDieNo] == FALSE)
	{
		bCheckDefect = PR_FALSE;
	}

	//Check Input parameter
	if ( (bInspect == FALSE) )
	{
		bCheckDefect = PR_FALSE;
		bCheckChip = PR_FALSE;
	}

//	bBackupAlign = (PR_BOOLEAN)m_bSrchEnableBackupAlign;
	bBackupAlign = (PR_BOOLEAN)FALSE; // Not sure how to use m_bSrchEnableBackupAlign

	//Calculate chip die area, min & total defect area
	//dMinChipArea		= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchChipArea[lDieNo];
	//dSingleDefectArea	= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	//dTotalDefectArea	= ((DOUBLE)(GetScanDieSizePixelX(lDieNo) * GetScanDieSizePixelY(lDieNo)) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 
	
	//try to use the searchDieCmd() for finding the  right search window
		dMinChipArea		= ((DOUBLE)(m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) / 100) * m_dGenSrchChipArea[lDieNo];
	dSingleDefectArea	= ((DOUBLE)(m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	dTotalDefectArea	= ((DOUBLE)(m_stGenDieSize[lDieNo].x * m_stGenDieSize[lDieNo].y) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 


	PR_2_POINTS_DIE_ALIGN_CMD       st2PointsAlignCmd;
	PR_Init2PointsDieAlignCmd(&st2PointsAlignCmd);
	//Setup Search Die parameter
	st2PointsAlignCmd.emConsistentDiePos		= PR_TRUE;
	st2PointsAlignCmd.emBackupTmpl				= PR_FALSE;      
	st2PointsAlignCmd.emSingleTmplSrch			= PR_FALSE;
	st2PointsAlignCmd.emSingleDieSrch			= PR_TRUE;
	st2PointsAlignCmd.emPartialDieSrch			= PR_FALSE;     
	st2PointsAlignCmd.emDieRotChk				= PR_TRUE;
	st2PointsAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emMinDieSizeChk			= PR_FALSE;
	st2PointsAlignCmd.emDieEdgeChk				= PR_TRUE;
	st2PointsAlignCmd.emPositionChk				= PR_FALSE;
	st2PointsAlignCmd.coRefPoint.x				= 0;
	st2PointsAlignCmd.coRefPoint.y				= 0;
	st2PointsAlignCmd.rRefAngle					= 0;
	st2PointsAlignCmd.coMaxShift.x				= 0;
	st2PointsAlignCmd.coMaxShift.y				= 0;
	st2PointsAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	st2PointsAlignCmd.szPosConsistency.x		= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.szPosConsistency.y		= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.rDieRotTol				= GetScanRotTol(bUseScanAngle);

	PR_STREET_DIE_ALIGN_CMD         stStreetAlignCmd;
	PR_InitStreetDieAlignCmd(&stStreetAlignCmd);
	stStreetAlignCmd.emConsistentDiePos			= PR_TRUE;
	stStreetAlignCmd.emSingleDieSrch			= PR_TRUE;
	stStreetAlignCmd.emPartialDieSrch			= PR_FALSE;
	stStreetAlignCmd.emDieRotChk				= PR_TRUE;
	stStreetAlignCmd.emMaxDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emMinDieSizeChk			= PR_FALSE;
	stStreetAlignCmd.emDieEdgeChk				= PR_TRUE;
	stStreetAlignCmd.emPositionChk				= PR_FALSE;
	stStreetAlignCmd.coRefPoint.x				= 0;
	stStreetAlignCmd.coRefPoint.y				= 0;
	stStreetAlignCmd.coMaxShift.x				= 0;
	stStreetAlignCmd.coMaxShift.y				= 0;
	stStreetAlignCmd.rRefAngle					= 0;
	stStreetAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	stStreetAlignCmd.szPosConsistency.x			= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	stStreetAlignCmd.szPosConsistency.y			= (PR_LENGTH) (PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	stStreetAlignCmd.rDieRotTol					= GetScanRotTol(bUseScanAngle);

	PR_DIE_ALIGN_CMD                stDieAlignCmd;
	PR_InitDieAlignCmd(&stDieAlignCmd);
	stDieAlignCmd.st2Points						= st2PointsAlignCmd;
	stDieAlignCmd.stStreet						= stStreetAlignCmd;

	PR_TMPL_DIE_INSP_CMD            stTmplInspCmd;
	PR_InitTmplDieInspCmd(&stTmplInspCmd);
	stTmplInspCmd.emInspIncompleteDie			= PR_FALSE;     
	//if (m_bEnableLineDefect)  // Design later
	//{
	//	stTmplInspCmd.emEnableLineDefectDetection		= PR_TRUE;
	//	stTmplInspCmd.uwNumLineDefects					= (PR_UWORD) m_lMinNumOfLineDefects;
	//	stTmplInspCmd.rMinLongestSingleLineDefectLength = (PR_REAL) ConvertScanDUnitToPixel(m_dMinLineDefectLength);		//mil -> pixel
	//	stTmplInspCmd.rMinLineClassLength				= (PR_REAL) ConvertScanDUnitToPixel(m_dMinLineDefectLength) / 2;
	//	stTmplInspCmd.rMinTotalLineDefectLength			= (PR_REAL) ConvertScanDUnitToPixel(m_dMinLineDefectLength) * 2;
	//}
	//Line Defect detection; only available for Normal-Die
	if (m_bEnableLineDefect)
	{
		stTmplInspCmd.emEnableLineDefectDetection		= PR_TRUE;
		stTmplInspCmd.uwNumLineDefects					= (PR_UWORD)m_lMinNumOfLineDefects;
		stTmplInspCmd.rMinLongestSingleLineDefectLength = (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength);		//mil -> pixel
		stTmplInspCmd.rMinLineClassLength				= (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength) / 2;
		stTmplInspCmd.rMinTotalLineDefectLength			= (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength) * 2;
	}

	stTmplInspCmd.emChipDieChk					= bCheckChip;
	stTmplInspCmd.emGeneralDefectChk			= bCheckDefect;
	stTmplInspCmd.emInkOnlyChk					= PR_FALSE;     
	stTmplInspCmd.aeMinSingleDefectArea			= (PR_AREA)dSingleDefectArea;
	stTmplInspCmd.aeMinTotalDefectArea			= (PR_AREA)dTotalDefectArea;  
	stTmplInspCmd.aeMinChipArea					= (PR_AREA)dMinChipArea; 
	stTmplInspCmd.emNewDefectCriteria			= PR_TRUE;
	stTmplInspCmd.ubIntensityVariation			= PR_DEF_INTENSITY_VARIATION;
	stTmplInspCmd.ubDefectThreshold				= ucDefectThres; 
	stTmplInspCmd.rMinInkSize					= PR_FALSE;     
	stTmplInspCmd.emDefectAttribute				= (PR_DEFECT_ATTRIBUTE)(lGreyLevelDefect);

	PR_DIE_INSP_CMD 				stDieInspCmd;
	PR_InitDieInspCmd(&stDieInspCmd);
	stDieInspCmd.stTmpl							= stTmplInspCmd;

	PR_DIE_ALIGN_PAR                stAlignPar;
	PR_InitDieAlignPar(&stAlignPar);
	stAlignPar.emEnableBackupAlign				= bBackupAlign;
	stAlignPar.rStartAngle						= GetScanStartAngle(bUseScanAngle);	
	stAlignPar.rEndAngle						= GetScanEndAngle(bUseScanAngle);		

	PR_POST_INDIV_INSP_CMD			stBadCutCmd;
	PR_InitPostIndivInspCmd(&stBadCutCmd);
	PR_POST_INSP_CMD 			    stBadCutInspCmd;

	PR_InitPostInspCmd(&stBadCutInspCmd);

	PR_POST_INDIV_INSP_EXT1_CMD		stBadCutExtCmd = stBadCutInspCmd.stEpoxy.stIndivExt1;

	////Setup badcut parameter
	////if ( GetDieShape() == WPR_RECTANGLE_DIE )
	//if ( GetDieShape() == BPR_RECTANGLE_DIE )          // Not need check badcut
	//{
	//	stBadCutCmd.emEpoxyInsuffChk				= PR_TRUE;
	//	stBadCutCmd.emEpoxyExcessChk				= PR_FALSE;
	//	stBadCutCmd.uwMaxInsuffSide					= 0;
	//	stBadCutCmd.stMinPercent.x					= (PR_REAL)(100 - m_stBadCutTolerenceX);
	//	stBadCutCmd.stMinPercent.y					= (PR_REAL)(100 - m_stBadCutTolerenceY);
	//	stBadCutCmd.szScanWidth.x					= (PR_LENGTH)m_stBadCutScanWidthX;
	//	stBadCutCmd.szScanWidth.y					= (PR_LENGTH)m_stBadCutScanWidthY;
	//	stBadCutCmd.stEpoxyInsuffWin.coCorner1		= m_stBadCutPoint[0];
	//	stBadCutCmd.stEpoxyInsuffWin.coCorner2		= m_stBadCutPoint[1];
	//	stBadCutCmd.stEpoxyInsuffWin.coObjCentre.x	= (PR_WORD) GetPrCenterX();
	//	stBadCutCmd.stEpoxyInsuffWin.coObjCentre.y	= (PR_WORD) GetPrCenterY();

	//	stBadCutInspCmd.emEpoxyAlg					= PR_INDIV_INSP;
	//	stBadCutInspCmd.stEpoxy.stIndiv				= stBadCutCmd;
	//}
	//else
	//{
	//	//PR_UBYTE lCorners = WPR_HEXAGON_CORNERS;
	//	PR_UBYTE lCorners = BPR_HEXAGON_CORNERS;
	//	//if ( GetDieShape() == WPR_RHOMBUS_DIE )
	//	if ( GetDieShape() == BPR_RHOMBUS_DIE )
	//	{
	//		//lCorners	= WPR_RHOMBUS_CORNERS;
	//		lCorners	= BPR_RHOMBUS_CORNERS;	
	//	}	//	rhombus die
	//	//else if( GetDieShape() == WPR_TRIANGULAR_DIE )
	//	else if( GetDieShape() == BPR_TRIANGULAR_DIE )
	//	{
	//		//lCorners	= WPR_TRIANGLE_CORNERS;		
	//		lCorners	= BPR_TRIANGLE_CORNERS;	
	//	}
	//	PR_LINE stLine;
	//	for (LONG i=0; i<lCorners; i++)
	//	{
	//		stLine.coPoint1 = m_stBadCutPoint[i];
	//
	//		if ( i >= (lCorners-1) )
	//		{
	//			stLine.coPoint2 = m_stBadCutPoint[0];
	//		}
	//		else
	//		{
	//			stLine.coPoint2 = m_stBadCutPoint[i+1];
	//		}

	//		stBadCutExtCmd.auwScanWidth[i]	= (PR_LENGTH)m_stBadCutScanWidthX;
	//		stBadCutExtCmd.aubMinPercent[i] = (PR_UBYTE)(100 - m_stBadCutTolerenceX);
	//		stBadCutExtCmd.astInsuffLine[i]	= stLine;
	//	}
	//	stBadCutExtCmd.ubNumOfInsuffLine	= lCorners;

	//	stBadCutExtCmd.emEpoxyInsuffChk		= PR_TRUE;
	//	stBadCutExtCmd.emEpoxyExcessChk		= PR_FALSE;
	//	stBadCutExtCmd.uwMaxInsuffSide		= 0;
	//	stBadCutExtCmd.coObjCentre.x		= (PR_WORD) GetPrCenterX();
	//	stBadCutExtCmd.coObjCentre.y		= (PR_WORD) GetPrCenterY();
	//	stBadCutExtCmd.rObjAngle			= 0.0;

	//	stBadCutInspCmd.emEpoxyAlg			= PR_INDIV_INSP_EXT1;
	//	stBadCutInspCmd.stEpoxy.stIndivExt1	= stBadCutExtCmd;
	//}
	stBadCutInspCmd.emEpoxyChk			= PR_TRUE;
//#ifndef VS_5MCAM //Not need check badcut
//	if( m_bBadcutBiDetect )	//	new library missing, need added in if in need in future.
//	{
//	//	stBadCutInspCmd.emSegMode	= PR_POST_EPOXY_SEG_MODE_BINARY;
//	//	stBadCutInspCmd.ubThd		= (PR_UBYTE) m_lBadcutBiThreshold;
//	}
//#endif

	//Klocwork	//v4.02T5
	stBadCutInspCmd.stEpoxy.stIndivExt1.ubNumOfExcessLine	= 0;
	stBadCutInspCmd.stEpoxy.stIndivExt1.uwMaxExcessSide		= 0;

	PR_SRCH_DIE_CMD stMultiSrchCmd;
	PR_InitSrchDieCmd(&stMultiSrchCmd);
	stMultiSrchCmd.emLatch			= PR_TRUE;
	stMultiSrchCmd.uwNRecordID		= 1;
	stMultiSrchCmd.auwRecordID[0]		= (PR_UWORD)(lPrSrchID);
	stMultiSrchCmd.emAlign			= PR_TRUE;
	stMultiSrchCmd.emDefectInsp		= (PR_BOOLEAN) bInspect;
	//stMultiSrchCmd.emCameraNo			= GetScnCamID();
	stMultiSrchCmd.emCameraNo			= MS899_BOND_CAM_ID;
	stMultiSrchCmd.emVideoSource		= PR_IMAGE_BUFFER_A;
	stMultiSrchCmd.ulRpyControlCode	= PR_DEF_SRCH_DIE_RPY;

	stMultiSrchCmd.stDieAlign			= stDieAlignCmd;
	stMultiSrchCmd.stDieInsp			= stDieInspCmd;
	stMultiSrchCmd.stDieAlignPar		= stAlignPar;
	stMultiSrchCmd.stPostBondInsp		= stBadCutInspCmd;
#ifdef VS_5MCAM
	stMultiSrchCmd.stDieAlign.stStreet.emPartialDieSrch	= PR_FALSE;	//	PR_TRUE;
	stMultiSrchCmd.stDieAlign.st2Points.emPartialDieSrch	= PR_FALSE;	//	PR_TRUE;
#endif

	stMultiSrchCmd.stDieAlignPar.rStartAngle		= GetScanStartAngle(bUseScanAngle);	
	stMultiSrchCmd.stDieAlignPar.rEndAngle			= GetScanEndAngle(bUseScanAngle);	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	{
		if (m_lGenSrchDieScore[BPR_NORMAL_DIE] < 70)		//v2.83T58
		{
			m_lGenSrchDieScore[BPR_NORMAL_DIE] = 70;
		}
	}
	stMultiSrchCmd.stDieAlignPar.rMatchScore			= (PR_REAL)m_lSrchDieScore;
	stMultiSrchCmd.stDieAlignPar.emIsDefaultMatchScore	= PR_FALSE;

	stMultiSrchCmd.emGraphicInfo		= PR_NO_DISPLAY;
	stMultiSrchCmd.emLatchMode		= PR_LATCH_FROM_GALLERY;

	stMultiSrchCmd.emPostBondInsp = PR_FALSE; // Not need check BadCut

	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = GetScanPRWinULX();
	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = GetScanPRWinULY();
	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = GetScanPRWinLRX();
	stMultiSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = GetScanPRWinLRY();

	stMultiSrchCmd.emDieInspAlg		= GetLrnInspMethod(lDieNo);
	stMultiSrchCmd.emAlignAlg		= GetLrnAlignAlgo(lDieNo);

	m_stMultiSrchCmd	= stMultiSrchCmd;

	//m_pPrescanPrCtrl->SetupSearchCommand(m_stMultiSrchCmd);  // Not need prescan
	//m_pPrescanPrCtrl->SetupSubImageNum(m_uwFrameSubImageNum);
	m_uwFrameSubImageNum = 1;

	//m_uwScanRecordID[0] = lPrSrchID;  // Not sure how to use m_uwScanRecordID[0]
	
	//if( m_bEnable2ndPrSearch)	// MS init PR record ID 2 multi-search parameters. //Not Sure how to use m_bEnable2ndPrSearch
	//{
	//	UCHAR ucDieNoIndex	= 0;
	//	//if( IsPrescanReferDie() )
	//	//{
	//	//	//ucDieNoIndex	= WPR_GEN_NDIE_OFFSET + 4;	// refer 1st die
	//	//	ucDieNoIndex	= BPR_GEN_NDIE_OFFSET + 4;	// refer 1st die
	//	//}
	//	//else if( IsThisDieLearnt(1) )
	//	//{
	//	//	ucDieNoIndex	= 1;	// normal die 2
	//	//}

	//	if( IsThisDieLearnt(1) )    // No need consider Prescan on BinTable
	//	{
	//		ucDieNoIndex	= 1;	// normal die 2
	//	}

	//	DisplaySequence("BPR - down and init 2nd PR scan search record"); // desgin later
	//	SetupScanSearchCmd2(ucDieNoIndex, FALSE);
	//}

	return TRUE;
}

PR_WORD CBondPr::GetDiePrID(UCHAR ucDieIndex)  //4.51D20
{
	//ucDieIndex = min(ucDieIndex, WPR_MAX_DIE-1);
	ucDieIndex = min(ucDieIndex, BPR_MAX_DIE-1);
	return	m_ssGenPRSrchID[ucDieIndex];
}

PR_DIE_INSP_ALG	CBondPr::GetLrnInspMethod(LONG lDieNo) //4.51D20
{
	PR_DIE_INSP_ALG usInspAlg;
	
	//lDieNo = min(lDieNo, WPR_MAX_DIE-1);	//Klocwork	//v4.02T5
	lDieNo = min(lDieNo, BPR_MAX_DIE-1);	//Klocwork	//v4.02T5

	switch (m_lGenLrnInspMethod[lDieNo])
	{
	case 1:  
		usInspAlg = PR_DIE_INSP_ALG_BIN;
		break;

	default: 
		usInspAlg = PR_GOLDEN_DIE_TMPL;
		break;
	}
	return usInspAlg;
}

PR_WORD CBondPr::GetScanNmlSizePixelX() //4.51D20
{
	UCHAR ucDieNo = (UCHAR)GetBondPRDieNo();
	return GetScanDieSizePixelX(ucDieNo - 1); // Set m_ucPrescanDiePrID = 1
}

PR_WORD CBondPr::GetScanNmlSizePixelY() //4.51D20
{
	UCHAR ucDieNo = (UCHAR)GetBondPRDieNo();
	return GetScanDieSizePixelY(ucDieNo - 1);
}

PR_WORD CBondPr::GetScanDieSizePixelX(UCHAR ucDieNo)//4.51D20 
{
	UCHAR ucIndex = ucDieNo;
	ucIndex = min(ucIndex, BPR_MAX_DIE-1);  // WPR_MAX_DIE = 40;
	return m_stGenDieSize[ucIndex].x;
}

PR_WORD CBondPr::GetScanDieSizePixelY(UCHAR ucDieNo) //4.51D20
{
	UCHAR ucIndex = ucDieNo;
	ucIndex = min(ucIndex, BPR_MAX_DIE-1);
	return m_stGenDieSize[ucIndex].y;
}

LONG CBondPr::GetPrScaleFactor() //4.51D20
{
	return PR_SCALE_FACTOR;
}

PR_REAL CBondPr::GetScanStartAngle(BOOL bUseScanAngle) //4.51D20
{
	PR_REAL rStart = PR_SRCH_START_ANGLE;
	if( bUseScanAngle )	//	IsEnableZoom() && 
	{
		//rStart = 0-fabs(m_dScanAcceptAngle);
		rStart = 0- 0;  // Not sure how to use m_dScanAcceptAngle
		if (rStart == 0)
		{
			rStart = 0 - 15.0;
		}
	}

//	if( IsMS90Sorting2ndPart() )	// sort mode
//2018.6.13
	BOOL bRotate180 = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"];
	if (IsMS90() && (bRotate180 /*|| CMS896AStn::m_b2Parts2ndPartStart*/))
	{
		rStart += 180;
	}

	return rStart;
}

PR_REAL CBondPr::GetScanEndAngle(BOOL bUseScanAngle) //4.51D20
{
	PR_REAL rEnd = PR_SRCH_END_ANGLE;

	if( bUseScanAngle )	//	IsEnableZoom() && 
	{
		//rEnd = fabs(m_dScanAcceptAngle);
		rEnd = 0; // Not sure how to use m_dScanAcceptAngle
		if (rEnd == 0)
		{
			rEnd = 15.0;
		}
	}

//	if( IsMS90Sorting2ndPart() )	// sort mode
	BOOL bRotate180 = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"];
	if (IsMS90() && (bRotate180 /*|| CMS896AStn::m_b2Parts2ndPartStart*/))
	{
		rEnd += 180;
	}

	return rEnd;
}

PR_REAL CBondPr::GetScanRotTol(BOOL bUseScanAngle) //4.51D20
{
	DOUBLE dAcptAngle = PR_SRCH_ROT_TOL;

	if( bUseScanAngle )	//	IsEnableZoom() && 
	{
		//dAcptAngle = fabs(m_dScanAcceptAngle);
		dAcptAngle = 0; //Not sure how to use m_dScanAcceptAngle
		if (dAcptAngle == 0)
		{
			dAcptAngle = 15.0;
		}
	}

	return (PR_REAL) dAcptAngle;
}

PR_WORD	CBondPr::GetScanPRWinULX()
{
	return GetScanPRWholeWindow().coCorner1.x;
}

PR_WORD	CBondPr::GetScanPRWinULY()
{
	return GetScanPRWholeWindow().coCorner1.y;
}

PR_WORD	CBondPr::GetScanPRWinLRX()
{
	return GetScanPRWholeWindow().coCorner2.x;
}

PR_WORD	CBondPr::GetScanPRWinLRY()
{
	return GetScanPRWholeWindow().coCorner2.y;
}

PR_WIN CBondPr::GetScanPRWholeWindow()
{
	PR_WIN PRTempWindow;
	PRTempWindow.coCorner1.x = GetPRWinULX();
	PRTempWindow.coCorner1.y = GetPRWinULY();
	PRTempWindow.coCorner2.x = GetPRWinLRX();
	PRTempWindow.coCorner2.y = GetPRWinLRY();

#ifdef VS_5MCAM		//v4.49 Klocwork
	// if the scanCut is invaild or it is not Prober
	CString szTemp = "";
	DOUBLE dShrinkRatio = 1.0;
	//if (SubCutScanFOV(szTemp, dShrinkRatio) ) //Not Sure How to use the SubCutScanFOV
	//{
	//	PR_WORD CenterX = GetPrCenterX();
	//	PR_WORD CenterY = GetPrCenterY();
	//	DOUBLE TempLengthX = fabs((DOUBLE)GetPRWinULX() - (DOUBLE)GetPRWinLRX()) * dShrinkRatio;
	//	DOUBLE TempLengthY = fabs((DOUBLE)GetPRWinULY() - (DOUBLE)GetPRWinLRY()) * dShrinkRatio;
	//	PR_WORD HalfRatioWindowLengthX = (PR_WORD)(LONG)(TempLengthX/2.0);
	//	PR_WORD HalfRatioWindowLengthY = (PR_WORD)(LONG)(TempLengthY/2.0);

	//	PRTempWindow.coCorner1.x = CenterX - HalfRatioWindowLengthX; //ULX
	//	PRTempWindow.coCorner1.y = CenterY - HalfRatioWindowLengthY; //ULY
	//	PRTempWindow.coCorner2.x = CenterX + HalfRatioWindowLengthX; //LRX
	//	PRTempWindow.coCorner2.y = CenterY + HalfRatioWindowLengthY; //LRY
	//}
#endif

	return PRTempWindow;
}

PR_WORD	CBondPr::GetPRWinULX()
{
	return m_stIMEffView.coCorner1.x;
}

PR_WORD	CBondPr::GetPRWinULY()
{
	return m_stIMEffView.coCorner1.y;
}

PR_WORD	CBondPr::GetPRWinLRX()
{
	return m_stIMEffView.coCorner2.x;
}

PR_WORD	CBondPr::GetPRWinLRY()
{
	return m_stIMEffView.coCorner2.y;
}

BOOL CBondPr::RectBondSingleCornerCheck(INT &nDieStateRight, INT &nDieStateLeft, INT &nDieStateUp, INT &nDieStateDown, UCHAR ucCorner) //4.51D20
{
	//		1
	//	0	4	2
	//		3
	//
	CString szTitle;
	LONG	lColDiff = 0, lRowDiff = 0;
	LONG lDiePitchX_X	= m_lDiePitchX;
	LONG lDiePitchY_Y	= m_lDiePitchY;
	LONG	lDiePitchX_Y = 0, lDiePitchY_X = 0;

	szTitle = "Search --- Start Check FOV Single Corner";
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szTitle, "a+");

	
	BPR_MULTI_SRCH_RESULT stMsch; //MULTI_SRCH_RESULT	stMsch;


	//======================================================= Start Call BT_MultiSearchDie() 
	BOOL bDrawDie = TRUE; //Asume always TRUE

	if( bDrawDie==TRUE )
	{
		OpenWaitingAlert();
		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	}
	int i;
	CDWordArray dwList;
	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
		stMsch.bDieState[i] = 0;
	}

	PrescanAutoMultiGrabDone(ucCorner, dwList, bDrawDie);

	for(i=0; i<5; i++)
	{
		stMsch.bDieState[i] = dwList.GetAt(i);
	}

	if( bDrawDie==TRUE )
	{
		CloseWaitingAlert();
	}
	//======================================================= End of Call BT_MultiSearchDie()


	//		1
	//	0	4	2
	//		3
	//
	nDieStateLeft 	= stMsch.bDieState[0];
	nDieStateUp		= stMsch.bDieState[1];
	nDieStateRight	= stMsch.bDieState[2];
	nDieStateDown	= stMsch.bDieState[3];

	CString szMsg;
	szMsg.Format("Search --- Single Corner Check, ucCorner:%d, \n \t\t stMsch.bDieState[0]:%d, stMsch.bDieState[1]:%d, \n \t\t stMsch.bDieState[2]:%d, stMsch.bDieState[3]:%d ", ucCorner, stMsch.bDieState[0],stMsch.bDieState[1],stMsch.bDieState[2],stMsch.bDieState[3]);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	//20170824 Leo  reset the value to 0
	stMsch.bDieState[0] = 0;
	stMsch.bDieState[1] = 0;
	stMsch.bDieState[2] = 0;
	stMsch.bDieState[3] = 0;

	if (IsSensorZoomFFMode() && ((nDieStateLeft = 1) || (nDieStateRight == 1) || (nDieStateUp == 1) || (nDieStateDown == 1)))
	{
		return TRUE;
	}

	switch (ucCorner)
	{
		case REALIGN_UPPER_LEFT_CORNER:
			// UL for further design if use other bonding path //20170824 Leo
			if( nDieStateLeft  == 1 && nDieStateUp == 1 && nDieStateRight == 0 && nDieStateDown == 0)
			{
				szMsg = "Find Die Upper Left with Corners,=: [0]1, [1]1, [2]0, [3]0";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				return TRUE;
			}
			else
			{
				szMsg = " Find Die Upper Left with Corners Fail";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				szMsg.Format("Search --- Single Corner Check, ucCorner:%d, \n \t\t nDieStateLeft:%d, nDieStateUp:%d, \n \t\t nDieStateRight:%d, nDieStateDown:%d ", ucCorner, nDieStateLeft,nDieStateUp,nDieStateRight,nDieStateDown);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			}
			break;
		case REALIGN_UPPER_RIGHT_CORNER:
			if(nDieStateLeft  == 0 && nDieStateUp == 1 && nDieStateRight == 1 && nDieStateDown == 0)
			{
				szMsg = "Find Die Upper Right with Corners,=: [0]0, [1]1, [2]1, [3]0";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				
				return TRUE;
			}
			else
			{
				szMsg = " Find Die Upper Right with Corners Fail";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				szMsg.Format("Search --- Single Corner Check, ucCorner:%d, \n \t\t nDieStateLeft:%d, nDieStateUp:%d, \n \t\t nDieStateRight:%d, nDieStateDown:%d ", ucCorner, nDieStateLeft,nDieStateUp,nDieStateRight,nDieStateDown);
			}
			break;

		case REALIGN_LOWER_LEFT_CORNER:
			if(nDieStateLeft  == 1 && nDieStateUp == 0 && nDieStateRight == 0 && nDieStateDown == 1)
			{
				szMsg = "Find Die Lower Left with Corners,=: [0]1, [1]0, [2]0, [3]1";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				
				return TRUE;
			}
			else
			{
				szMsg = " Find Die Lower Left with Corners Fail";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				szMsg.Format("Search --- Single Corner Check, ucCorner:%d, \n \t\t nDieStateLeft:%d, nDieStateUp:%d, \n \t\t nDieStateRight:%d, nDieStateDown:%d ", ucCorner, nDieStateLeft,nDieStateUp,nDieStateRight,nDieStateDown);
			}
			break;

		case REALIGN_LOWER_RIGHT_CORNER:
			if(nDieStateLeft  == 0 && nDieStateUp == 0 && nDieStateRight == 1 && nDieStateDown == 1)
			{
				szMsg = "Find Die Lower Right with Corners,=: [0]0, [1]0, [2]1, [3]1";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				
				return TRUE;
			}
			else
			{
				szMsg = " Find Die Lower Right with Corners Fail";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				szMsg.Format("Search --- Single Corner Check, ucCorner:%d, \n \t\t nDieStateLeft:%d, nDieStateUp:%d, \n \t\t nDieStateRight:%d, nDieStateDown:%d ", ucCorner, nDieStateLeft,nDieStateUp,nDieStateRight,nDieStateDown);
			}
			break;
	}
	return FALSE;
}

LONG CBondPr::RectBondCornerCheck(INT &nDieStateRight, INT &nDieStateLeft, INT &nDieStateUp, INT &nDieStateDown, UCHAR ucCorner)  //4.51D20
{
	CString szMsg;
	szMsg = "Search --- BPR: Rect Bin Corner Check";
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	
	//================================================================== Init
	if (!IsPRLearnt(GetBondPRDieNo()))
	{
		//PR not learnt
		return IDS_BPR_DIENOTLEARNT;
	}
	
	if (OpPrescanInit((UCHAR)GetBondPRDieNo()) == gnNOTOK)
	{
		return FALSE;
	}
	//================================================================== end Init	
	Sleep(100);

	//				||
	//	   First Die||
	//	 ----------- |
	//   ---------------------------
	//				 |
	//				 |

	//// check UP LEFT CORNER DIE  if Low Right First Die

	//if( RectBondSingleCornerCheck(ulRow, ulCol, 0)==FALSE )
	//{
	//	szMsg = "BPR: check UP LEFT CORNER DIE Fail";
	//	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	//	return FALSE;
	//}



	//				|
	//				|First Die
	//   ---------------------------
	//				|
	//				|
	//// check UP RIGHT CORNER DIE if Low Left First Die
	//ulRow = 0;
	//ulCol = ulNumOfCol-1;
	//if( RectBondSingleCornerCheck(ulRow, ulCol, 1)==FALSE )
	//{
	//	szMsg = "BPR: check UP RIGHT CORNER DIE Fail";
	//	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	//	return FALSE;
	//}

	//				|
	//				|
	//   ---------------------------
	//	First Die	|
	//				|
	//// check LOW LEFT CORNER DIE if Top Right First Die
	//ulRow = ulNumOfRow-1;
	//ulCol = 0;
	//if( RectBondSingleCornerCheck(ulRow, ulCol, 2)==FALSE )
	//{
	//	szMsg = "BPR: check LOW LEFT CORNER DIE Fail";
	//	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	//	return FALSE;
	//}




	//				|
	//				|
	//   ---------------------------
	//				| First Die
	//				|
	// check LOW RIGHT CORNER DIE if Find Top Left First Die

	if (!RectBondSingleCornerCheck(nDieStateRight, nDieStateLeft, nDieStateUp, nDieStateDown, ucCorner))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CBondPr::IsLFSizeOK()  //4.51D20
{
	//if( m_dLFSize >= WPR_LF_SIZE )
	if (m_dFovSize < BPR_LF_SIZE) // LF: look forword
		return TRUE; //Assume FOV has dices
	else
		return FALSE;
}


void CBondPr::SetMultiDiePosition(USHORT usIndex,LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin, LONG nRow, LONG nCol)//4.51D20
{
	CString szMsg;

	(*m_psmfSRam)["BinTable"][usIndex]["siStepX"]["X"]= nDieX;
	(*m_psmfSRam)["BinTable"][usIndex]["siStepY"]["Y"]= nDieY;
}


VOID CBondPr::CompareDiesPositionToFindFirstDies(const UCHAR ucCorner, const USHORT usDieSum)
{
	//20170824 Leo add checking for Walking Path
	switch (ucCorner)
	{
		case REALIGN_UPPER_LEFT_CORNER:
			 FindFirstDieInUpperLeftCorner(usDieSum);
			 break;
		case REALIGN_UPPER_RIGHT_CORNER:
			 FindFirstDieInUpperRightCorner(usDieSum);
			 break;
		case REALIGN_LOWER_LEFT_CORNER:
			 FindFirstDieInLowerLeftCorner(usDieSum);
			 break;
		case REALIGN_LOWER_RIGHT_CORNER:
			FindFirstDieInLowerRightCorner(usDieSum);
			break;
	}
}


 VOID CBondPr::FindFirstDieInUpperLeftCorner(const ULONG usDieSum)
{
	LONG nFirstDieStepX = 0,  nFirstDieStepY = 0;
	LONG nTempFirstDieStepX = 0, nTempFirstDieStepY = 0;
	CString szMsg;
	BOOL bTempUpdate = FALSE;

	LONG nOffsetX = m_lDiePitchX*(-1)/2;
	LONG nOffsetY = m_lDiePitchY*(-1)/2;
	if(nOffsetX < 100 || nOffsetY < 100)
	{
		nOffsetY = 200;
		nOffsetX = 200;
	}

	for (USHORT usIndex = 0; usIndex < usDieSum; usIndex++)
	{
		nTempFirstDieStepX = (*m_psmfSRam)["BinTable"][usIndex]["siStepX"]["X"];
		nTempFirstDieStepY = (*m_psmfSRam)["BinTable"][usIndex]["siStepY"]["Y"];

		if(usIndex == 0)
		{
			szMsg.Format("Search --- Compare Posn: Step(%d,%d), DiePitch(%d,%d) AllowedOffset(%d,%d)", nFirstDieStepX, nFirstDieStepY, m_lDiePitchX, m_lDiePitchY, nOffsetX, nOffsetY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			//20170824 Leo no need to set -30000,-30000 for the initial check point
			nFirstDieStepX = nTempFirstDieStepX - m_lDiePitchX; //make sure the first die comparison is not the rightmost
			nFirstDieStepY = nTempFirstDieStepY - m_lDiePitchY; //make sure the first die comparison is not the bottommost
			(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nTempFirstDieStepX;
			(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nTempFirstDieStepY;
		}

		if(abs(nTempFirstDieStepY - nFirstDieStepY) > nOffsetY ) // prevent the same horizonal row die 
		{
			if( nTempFirstDieStepY < nFirstDieStepY )  // go to down
			{
				nFirstDieStepX = nTempFirstDieStepX;
				nFirstDieStepY = nTempFirstDieStepY;
				(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
				(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
				//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

				szMsg.Format("Search -- Temp First Die Go to bottom(TendTo_-ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				bTempUpdate = TRUE;
			}
		}
		else
		{
			if (abs(nTempFirstDieStepX - nFirstDieStepX) > nOffsetX ) 
			{
				if(nTempFirstDieStepX < nFirstDieStepX)   // go to right
				{
					nFirstDieStepX = nTempFirstDieStepX;
					nFirstDieStepY = nTempFirstDieStepY;
					(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
					(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
					//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

					szMsg.Format("Search -- Temp First Die Go to Right (TendTo_-ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
					bTempUpdate = TRUE;
				}
			}
		}
	}
}

VOID CBondPr::FindFirstDieInUpperRightCorner(const ULONG usDieSum)
{
	LONG nFirstDieStepX = 0,  nFirstDieStepY = 0;
	LONG nTempFirstDieStepX = 0, nTempFirstDieStepY = 0;
	CString szMsg;
	BOOL bTempUpdate = FALSE;

	LONG nOffsetX = m_lDiePitchX*(-1)/2;
	LONG nOffsetY = m_lDiePitchY*(-1)/2;
	if(nOffsetX < 100 || nOffsetY < 100)
	{
		nOffsetY = 200;
		nOffsetX = 200;
	}

	for (USHORT usIndex = 0; usIndex < usDieSum; usIndex++)
	{
		nTempFirstDieStepX = (*m_psmfSRam)["BinTable"][usIndex]["siStepX"]["X"];
		nTempFirstDieStepY = (*m_psmfSRam)["BinTable"][usIndex]["siStepY"]["Y"];

		if(usIndex == 0)
		{
			szMsg.Format("Search --- Compare Posn: Step(%d,%d), DiePitch(%d,%d) AllowedOffset(%d,%d)", nFirstDieStepX, nFirstDieStepY, m_lDiePitchX, m_lDiePitchY, nOffsetX, nOffsetY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			//20170824 Leo no need to set -30000,-30000 for the initial check point
			nFirstDieStepX = nTempFirstDieStepX - m_lDiePitchX; //make sure the first die comparison is not the rightmost
			nFirstDieStepY = nTempFirstDieStepY - m_lDiePitchY; //make sure the first die comparison is not the bottommost
			(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nTempFirstDieStepX;
			(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nTempFirstDieStepY;
		}

		if(abs(nTempFirstDieStepY - nFirstDieStepY) > nOffsetY ) // prevent the same horizonal row die 
		{
			if( nTempFirstDieStepY < nFirstDieStepY )  // go to down
			{
				nFirstDieStepX = nTempFirstDieStepX;
				nFirstDieStepY = nTempFirstDieStepY;
				(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
				(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
				//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

				szMsg.Format("Search -- Temp First Die Go to bottom(TendTo_-ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				bTempUpdate = TRUE;
			}
		}
		else
		{
			if (abs(nTempFirstDieStepX - nFirstDieStepX) > nOffsetX ) 
			{
				if(nTempFirstDieStepX > nFirstDieStepX)   // go to left X-axis not right
				{
					nFirstDieStepX = nTempFirstDieStepX;
					nFirstDieStepY = nTempFirstDieStepY;
					(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
					(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
					//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

					szMsg.Format("Search -- Temp First Die Go to Left (TendTo_+ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
					bTempUpdate = TRUE;
				}
			}
		}
	}
}

VOID CBondPr::FindFirstDieInLowerLeftCorner(const ULONG usDieSum)
{
	LONG nFirstDieStepX = 0,  nFirstDieStepY = 0;
	LONG nTempFirstDieStepX = 0, nTempFirstDieStepY = 0;
	CString szMsg;
	BOOL bTempUpdate = FALSE;

	LONG nOffsetX = m_lDiePitchX*(-1)/2;
	LONG nOffsetY = m_lDiePitchY*(-1)/2;
	if(nOffsetX < 100 || nOffsetY < 100)
	{
		nOffsetY = 200;
		nOffsetX = 200;
	}

	for (USHORT usIndex = 0; usIndex < usDieSum; usIndex++)
	{
		nTempFirstDieStepX = (*m_psmfSRam)["BinTable"][usIndex]["siStepX"]["X"];
		nTempFirstDieStepY = (*m_psmfSRam)["BinTable"][usIndex]["siStepY"]["Y"];

		if(usIndex == 0)
		{
			//20170824 Leo no need to set -30000,-30000 for the initial check point
			nFirstDieStepX = nTempFirstDieStepX + m_lDiePitchX; //make sure the first die comparison is not the leftmost
			nFirstDieStepY = nTempFirstDieStepY + m_lDiePitchY; //make sure the first die comparison is not the uppermost
			(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nTempFirstDieStepX;
			(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nTempFirstDieStepY;

			szMsg.Format("Search --- Compare Posn: Step(%d,%d), DiePitch(%d,%d) AllowedOffset(%d,%d)", nFirstDieStepX, nFirstDieStepY, m_lDiePitchX, m_lDiePitchY, nOffsetX, nOffsetY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		}

		if(abs(nTempFirstDieStepY - nFirstDieStepY) > nOffsetY ) // prevent the same horizonal row die 
		{
			if( nTempFirstDieStepY > nFirstDieStepY )  // go to upper not down
			{
				nFirstDieStepX = nTempFirstDieStepX;
				nFirstDieStepY = nTempFirstDieStepY;
				(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
				(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
				//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

				szMsg.Format("Search -- Temp First Die Go to upper(TendTo_+ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				bTempUpdate = TRUE;
			}
		}
		else
		{
			if (abs(nTempFirstDieStepX - nFirstDieStepX) > nOffsetX ) 
			{
				if(nTempFirstDieStepX < nFirstDieStepX)   // go to right
				{
					nFirstDieStepX = nTempFirstDieStepX;
					nFirstDieStepY = nTempFirstDieStepY;
					(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
					(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
					//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

					szMsg.Format("Search -- Temp First Die Go to Right (TendTo_-ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
					bTempUpdate = TRUE;
				}
			}
		}
	}
}


VOID CBondPr::FindFirstDieInLowerRightCorner(const ULONG usDieSum)
{
	LONG nFirstDieStepX = 0,  nFirstDieStepY = 0;
	LONG nTempFirstDieStepX = 0, nTempFirstDieStepY = 0;
	CString szMsg;
	BOOL bTempUpdate = FALSE;

	LONG nOffsetX = m_lDiePitchX*(-1)/2;
	LONG nOffsetY = m_lDiePitchY*(-1)/2;
	if(nOffsetX < 100 || nOffsetY < 100)
	{
		nOffsetY = 200;
		nOffsetX = 200;
	}

	for (USHORT usIndex = 0; usIndex < usDieSum; usIndex++)
	{
		nTempFirstDieStepX = (*m_psmfSRam)["BinTable"][usIndex]["siStepX"]["X"];
		nTempFirstDieStepY = (*m_psmfSRam)["BinTable"][usIndex]["siStepY"]["Y"];

		if(usIndex == 0)
		{
			//20170824 Leo no need to set -30000,-30000 for the initial check point
			nFirstDieStepX = nTempFirstDieStepX + m_lDiePitchX; //make sure the first die comparison is not the leftmost
			nFirstDieStepY = nTempFirstDieStepY + m_lDiePitchY; //make sure the first die comparison is not the uppermost
			(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nTempFirstDieStepX;
			(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nTempFirstDieStepY;
			szMsg.Format("Search --- Compare Posn: Step(%d,%d), DiePitch(%d,%d) AllowedOffset(%d,%d)", nFirstDieStepX, nFirstDieStepY, m_lDiePitchX, m_lDiePitchY, nOffsetX, nOffsetY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		}

		if(abs(nTempFirstDieStepY - nFirstDieStepY) > nOffsetY ) // prevent the same horizonal row die 
		{
			if( nTempFirstDieStepY > nFirstDieStepY )  // go to upper not down
			{
				nFirstDieStepX = nTempFirstDieStepX;
				nFirstDieStepY = nTempFirstDieStepY;
				(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
				(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
				//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

				szMsg.Format("Search -- Temp First Die Go to upper(TendTo_+ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				bTempUpdate = TRUE;
			}
		}
		else
		{
			if (abs(nTempFirstDieStepX - nFirstDieStepX) > nOffsetX ) 
			{
				if(nTempFirstDieStepX > nFirstDieStepX)   // go to left X-axis not right
				{
					nFirstDieStepX = nTempFirstDieStepX;
					nFirstDieStepY = nTempFirstDieStepY;
					(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= nFirstDieStepX;
					(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= nFirstDieStepY;
					//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"]= usIndex;

					szMsg.Format("Search -- Temp First Die Go to Left (TendTo_+ve) --- Index:%d, Step(%d,%d)",usIndex, nFirstDieStepX, nFirstDieStepY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
					bTempUpdate = TRUE;
				}
			}
		}
	}
}




BOOL CBondPr::SearchFrameEdgeAngle(DOUBLE& dAngle)	//v4.59A1
{
	CString szLog;
	szLog.Format("BPR: SearchFrameEdgeAngle Ref #2 = %d ....", m_ssGenPRSrchID[BPR_GEN_RDIE_OFFSET+2]);
	HmiMessage(szLog);

	PR_UWORD uwPRStatus = 0;
	PR_EXTRACT_OBJ_CMD stCmd;
	PR_InitExtractObjCmd(&stCmd);

	//stCmd.stLatchPar[0].emLatch	= PR_LATCH_FROM_CAMERA;
	stCmd.emShapeType			= PR_OBJSHAPE_TYPE_MULTILINE;	//PR_OBJSHAPE_TYPE_EDGE;
	stCmd.rPassScore			= 70.0;
	stCmd.emGraphicInfo			= PR_SHOW_SEARCH_CONTOUR;		//PR_DISPLAY_EDGE_PT;
	stCmd.stLatchPar.emLatch	= PR_LATCH_FROM_CAMERA;
	stCmd.stLatchPar.emGrab		= PR_GRAB_FROM_RECORD_ID;		//PR_GRAB_FROM_PURPOSE;
	stCmd.stLatchPar.uwRecordID	= m_ssGenPRSrchID[BPR_GEN_RDIE_OFFSET+2];	//Use Ref Die #2

	stCmd.stLatchPar.emPurpose	= MS899_BOND_GEN_PURPOSE;
	stCmd.emObjAttribute		= PR_OBJ_ATTRIBUTE_BOTH;

	stCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint1 = m_stSearchArea.coCorner1;
	stCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint2 = m_stSearchArea.coCorner2;
	stCmd.stExtractObjPar.stMultiLineInput.coAlignRetPt.x = (m_stSearchArea.coCorner1.x + m_stSearchArea.coCorner2.x) / 2;
	stCmd.stExtractObjPar.stMultiLineInput.coAlignRetPt.y = (m_stSearchArea.coCorner1.y + m_stSearchArea.coCorner2.y) / 2;
	stCmd.stExtractObjPar.stMultiLineInput.rMaxScale	= 101;
	stCmd.stExtractObjPar.stMultiLineInput.rMinScale	= 99;
	//stCmd.stExtractObjPar.stMultiLineInput.uwNumOfLine	= 1;
	stCmd.stExtractObjPar.stMultiLineInput.wEdgeMag		= 20;

	stCmd.stOpRegion.uwNumOfCorners = PR_NO_OF_CORNERS;	//4;
	stCmd.stOpRegion.acoCorners[PR_UPPER_LEFT]		= m_stSearchArea.coCorner1;
	stCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT]		= m_stSearchArea.coCorner2;
	stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x	= m_stSearchArea.coCorner2.x;
	stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y	= m_stSearchArea.coCorner1.y;
	stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x	= m_stSearchArea.coCorner1.x;
	stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y	= m_stSearchArea.coCorner2.y;

	PR_ExtractObjCmd(&stCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &uwPRStatus);

	if (uwPRStatus != PR_ERR_NOERR)
	{
		CString szErr;
		szErr.Format("BPR: PR_ExtractObjCmd error - code = 0x%x",  uwPRStatus);
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		return FALSE;

	}
	
	PR_EXTRACT_OBJ_RPY1 stRpy1;
	PR_EXTRACT_OBJ_RPY2 stRpy2;

	PR_ExtractObjRpy1(MS899_BOND_CAM_SEND_ID, &stRpy1);
	if ((stRpy1.uwCommunStatus != PR_COMM_NOERR) || (stRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString szErr;
		szErr.Format("BPR: PR_ExtractObjRpy1 error - COMM = 0x%x, PR = 0x%x",  
						stRpy1.uwCommunStatus, stRpy1.uwPRStatus);
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		return FALSE;
	}

	PR_ExtractObjRpy2(MS899_BOND_CAM_SEND_ID, &stRpy2);

	if ((stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) || 
		(stRpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		CString szErr;
		szErr.Format("BPR: PR_ExtractObjRpy2 error - COMM = 0x%x, PR = 0x%x",  
						stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		return FALSE;
	}

	INT nNoOfLines = stRpy2.stResult.uwNumOfObject;
	DOUBLE dLineAngle = 0;

	for (INT i=0; i<nNoOfLines; i++)
	{
		dLineAngle = stRpy2.stResult.astMultiLineResult[i].rAngle;
	}

	dAngle = dLineAngle;

/*
	INT nNoOfEdgePts = stRpy2.stResult.uwNumOfObject;

	DOUBLE dX1=0, dX2=0;
	DOUBLE dY1=0, dY2=0;
	for (INT i=0; i<nNoOfEdgePts; i++)
	{
		DOUBLE dX = stRpy2.stResult.astEdgeResult[i].rcoEdgePt.x;
		DOUBLE dY = stRpy2.stResult.astEdgeResult[i].rcoEdgePt.y;

		if (i == 0)
		{
			dX1 = dX;
			dY1 = dY; 
		}
		else if (i == nNoOfEdgePts-1)
		{
			dX2 = dX;
			dY2 = dY; 
		}
	}

	DOUBLE dLengthX = fabs(dX2 - dX1);
	DOUBLE dLengthY = fabs(dY2 - dY1);

	if (dLengthY == 0)
	{
		CString szErr;
		szErr.Format("BPR: PR_ExtractObjRpy2 error - zero DY detected (%f, %f) (%f, %f)",  
						dX1, dY1, dX2, dY2);
		SetErrorMessage(szErr);
		HmiMessage(szErr);
		return FALSE;
	}

	dAngle = atan(dLengthX / dLengthY) * 180.0 / PI;
*/

	return TRUE;
}

PR_UWORD CBondPr::ExtractObjectCmd(BOOL bColletOffset,double &dX,double &dY,double &dAngle)
{
	CString szMsg;
	LONG lThreshold = (*m_psmfSRam)["BondPr"]["BlockThreshold"];
	PR_EXTRACT_OBJ_CMD	stCmd;
	PR_EXTRACT_OBJ_RPY1 stRpy1;
	PR_EXTRACT_OBJ_RPY2 stRpy2;

	PR_DEFINE_MOUSE_CTRL_OBJ_CMD	stDefineObjCmd;
	PR_DEFINE_MOUSE_CTRL_OBJ_RPY	stDefineObjRpy;

	PR_GET_MOUSE_CTRL_OBJ_CMD		stGetObjCmd;
	PR_GET_MOUSE_CTRL_OBJ_RPY		stGetObjRpy;

	PR_COMMON_RPY			stRpy;

	PR_ULWORD				ulStartTime = 0, ulEndTime = 0;

	PR_WIN		stWin;
	PR_REAL		rPassScore = 0;
	PR_REAL		rRadius = 0;
	PR_WORD		wChoice = 0;
	PR_WORD		wExtractObjMode = 0;
	PR_UWORD	uwCommunStatus = 0;
	PR_UWORD	i = 0;

	PR_InitExtractObjCmd(&stCmd);
	PR_InitDefineMouseCtrlObjCmd(&stDefineObjCmd);
	PR_InitGetMouseCtrlObjCmd(&stGetObjCmd);

	//fflush(stdin);
	//printf("Select purpose: ");
	//scanf("%d", &nChoice);
	//fflush(stdin);
	stCmd.stLatchPar.emPurpose = MS899_BOND_GEN_PURPOSE;

	//fflush(stdin);
	//printf("Select shape type: \n");
	//printf("  1. Blob\n");
	//printf("  2. Circle\n");
	//printf("  3. Rectangle\n");
	//printf("  4. Cross Landmark\n");
	//printf("  5. Dome\n");
	//printf("  6. LD\n");
	//printf("  7. Multi Line\n");
	//printf("  8. Single Line\n");
	//printf("  9. Edge\n");
	//printf("  10. Cross point\n");

	//printf("Enter choice (0--10): ");
	//scanf("%hd", &wExtractObjMode);
	//fflush(stdin);
	wExtractObjMode = 1;

	switch (wExtractObjMode)
	{
	case 1:
		stCmd.emShapeType = PR_OBJSHAPE_TYPE_BLOB;
		//printf("Select Blob extract alg (0:C.G.(Grey Level)/1:C.G.(Binary)/2:ER/4:MAER): \n");
		//scanf("%hd", &wChoice);//set 0
		stCmd.stExtractObjPar.stBlobInput.emBlobAlg = (PR_BLOB_ALG)1/*0*//*PR_BLOB_ALG_GREY_LEVEL_CG*//*wChoice*/;
		//printf("Set threshold: \n");
		//scanf("%d", &nChoice);
		stCmd.stExtractObjPar.stBlobInput.ubThreshold = (PR_UBYTE)lThreshold/*190*//*220*//*nChoice*/;
		//printf("Blob type (0:hollow/1:solid): \n");
		//scanf("%hd", &wChoice);//1
		stCmd.stExtractObjPar.stBlobInput.emBlobType = (PR_BLOB_TYPE)0/*wChoice*/;
		stCmd.stExtractObjPar.stBlobInput.rLowerArea = 150000;
		szMsg.Format("Extract Object Cmd,Threshold,%d",lThreshold);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		break;
	//case 2:
	//	stCmd.emShapeType = PR_OBJSHAPE_TYPE_CIRCLE;

	//	fflush(stdin);
	//	printf("Input Circle Radius: ");
	//	scanf("%f", &rRadius);
	//	stCmd.stExtractObjPar.stCircleInput.rRadius = rRadius;

	//	stCmd.stExtractObjPar.stCircleInput.rMinScale = 50;
	//	stCmd.stExtractObjPar.stCircleInput.rMaxScale = 150;
	//	stCmd.stExtractObjPar.stCircleInput.wEdgeMag = 20;
	//	stCmd.stExtractObjPar.stCircleInput.uwRoughness = 16;

	//	break;
	//case 4:
	//	stCmd.emShapeType = PR_OBJSHAPE_TYPE_CROSS_LANDMARK;
	//	stCmd.stExtractObjPar.stCrosslandmarkInput.rCrossSize = PR_MIN_CROSS_LANDMARK_SIZE;
	//	stCmd.stExtractObjPar.stCrosslandmarkInput.rMinScale = 90;
	//	stCmd.stExtractObjPar.stCrosslandmarkInput.rMaxScale = 110;
	//	stCmd.stExtractObjPar.stCrosslandmarkInput.rStartAngle = PR_DEF_EXT_OBJ_START_ANGLE;
	//	stCmd.stExtractObjPar.stCrosslandmarkInput.rEndAngle = PR_DEF_EXT_OBJ_END_ANGLE;
	//	stCmd.stExtractObjPar.stCrosslandmarkInput.wEdgeMag = 10;
	//		
	//		//Get cross land mark size 
	//	IMG_WORD	wCrossSize;
	//	printf("Draw cross land mark size:\n");
	//	//if (GetWindowFromScreen(&stWin) != 0) {break;}
	//	//wCrossSize = stWin.coCorner2.x - stWin.coCorner1.x;

	//	if (wCrossSize < 0)
	//	{
	//		stCmd.stExtractObjPar.stCrosslandmarkInput.rCrossSize = (IMG_REAL) - wCrossSize;
	//	}
	//	else
	//	{
	//		stCmd.stExtractObjPar.stCrosslandmarkInput.rCrossSize = (IMG_REAL) wCrossSize;
	//	}
	//	break;
	//case 8:
	//	stCmd.emShapeType = PR_OBJSHAPE_TYPE_MULTILINE;
	//	fflush(stdin);
	//	printf("Select LinePolarity (If you walk on the line from start pt to end pt, BRIGHT means right is dark and left is bright: 1=bright, 2=dark): ");
	//	scanf("%hd", &wChoice);
	//	stCmd.stExtractObjPar.stMultiLineInput.aemLinePolarity[0] = (PR_OBJ_ATTRIBUTE)wChoice;

	//		//Draw polygon on screen to inscribe the die		
	//	fflush(stdin);
	//	stDefineObjCmd.emShape = PR_SHAPE_TYPE_LINE;
	//	stDefineObjCmd.emColor = PR_COLOR_GREEN;
	//	stDefineObjCmd.ulObjectId = 1;
	//	PR_DefineMouseCtrlObjCmd(&stDefineObjCmd, ubSenderID, ubReceiverID, &stDefineObjRpy);
	//	printf("Draw a Line to inscribe the shape, then press any key to continue...");
	//	fflush(stdin);
	//	getchar();
	//	fflush(stdin);

	//		//Get back the drawn polygon and extract the corners
	//	stGetObjCmd.ulObjectId = 1;
	//	PR_GetMouseCtrlObjCmd(&stGetObjCmd, ubSenderID, ubReceiverID, &stGetObjRpy);
	//	stCmd.stExtractObjPar.stMultiLineInput.uwNumOfLine = 1;

	//	stCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint1 = stGetObjRpy.acoObjCorner[0];
	//	stCmd.stExtractObjPar.stMultiLineInput.astLine[0].coPoint2 = stGetObjRpy.acoObjCorner[1];
	//	stCmd.stExtractObjPar.stMultiLineInput.coAlignRetPt.x = (stGetObjRpy.acoObjCorner[0].x + stGetObjRpy.acoObjCorner[1].x) / 2;
	//	stCmd.stExtractObjPar.stMultiLineInput.coAlignRetPt.y = (stGetObjRpy.acoObjCorner[0].y + stGetObjRpy.acoObjCorner[1].y) / 2;

	//	stCmd.stExtractObjPar.stMultiLineInput.rMaxScale = 101;
	//	stCmd.stExtractObjPar.stMultiLineInput.rMinScale = 99;

	//	stCmd.stExtractObjPar.stMultiLineInput.wEdgeMag = 20;
	//	break;
	//default:
	//	printf("Not support yet or Invalid input!\n");
	//	break;
	}

	//fflush(stdin);
	//printf("Select PR_OBJ_ATTRIBUTE (0=both, 1=bright, 2=dark): ");
	//scanf("%hd", &wChoice);
	stCmd.emObjAttribute = (PR_OBJ_ATTRIBUTE)1/*wChoice*/;

	//fflush(stdin);
	//printf("Input Passing Score: ");
	//scanf("%f", &rPassScore);
	stCmd.rPassScore = 60/*rPassScore*/;

	//printf("Draw Op win:");
	//GetWindowFromScreen(&stWin);
	BOOL bBTAtBH1Pos		= (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];
	DOUBLE dCentreOffsetX = 0;
	DOUBLE dCentreOffsetY = 0;
	PR_COORD stCollet2BondCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stCollet2BondCorner2 = m_stSearchArea.coCorner2;
	PR_COORD stCollet1BondCorner1 = m_stSearchArea.coCorner1;
	PR_COORD stCollet1BondCorner2 = m_stSearchArea.coCorner2;

	LONG lCollet2OffsetX	= (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["X"];
	LONG lCollet2OffsetY	= (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet2Offset"]["Y"];
	LONG lCollet1OffsetX	= (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["X"];
	LONG lCollet1OffsetY	= (LONG)(*m_psmfSRam)["WaferPr"]["EjtCollet1Offset"]["Y"];

	stCollet2BondCorner1.x = stCollet2BondCorner1.x - (lCollet2OffsetX * (1.0 / (m_dBTXRes * 2000)/ m_dCalibX));
	stCollet2BondCorner1.y = stCollet2BondCorner1.y - (lCollet2OffsetY * (1.0 / (m_dBTXRes * 2000)/ m_dCalibY));
	stCollet2BondCorner2.x = stCollet2BondCorner2.x - (lCollet2OffsetX * (1.0 / (m_dBTXRes * 2000)/ m_dCalibX));
	stCollet2BondCorner2.y = stCollet2BondCorner2.y - (lCollet2OffsetY * (1.0 / (m_dBTXRes * 2000)/ m_dCalibY));

	stCollet1BondCorner1.x = stCollet1BondCorner1.x - (lCollet1OffsetX * (1.0 / (m_dBTXRes * 2000)/ m_dCalibX));
	stCollet1BondCorner1.y = stCollet1BondCorner1.y - (lCollet1OffsetY * (1.0 / (m_dBTXRes * 2000)/ m_dCalibY));
	stCollet1BondCorner2.x = stCollet1BondCorner2.x - (lCollet1OffsetX * (1.0 / (m_dBTXRes * 2000)/ m_dCalibX));
	stCollet1BondCorner2.y = stCollet1BondCorner2.y - (lCollet1OffsetY * (1.0 / (m_dBTXRes * 2000)/ m_dCalibY));
	if (!bBTAtBH1Pos)	//bBHZ2TowardsPick)	//Arm 2
	{
		//v4.49A12
		//Move collet offset
		//Add BH2 offset XY
		//calculate PR Centre offset
		LONG lBHZ2BondPosOffsetX = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"];
		LONG lBHZ2BondPosOffsetY = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"];
		if( m_dCalibX!=0.0 )
			dCentreOffsetX = lBHZ2BondPosOffsetX / m_dCalibX;
		if( m_dCalibY!=0.0 )
			dCentreOffsetY = lBHZ2BondPosOffsetY / m_dCalibY;
		stCollet2BondCorner1.x += dCentreOffsetX;
		stCollet2BondCorner1.y += dCentreOffsetY;
		stCollet2BondCorner2.x += dCentreOffsetX;
		stCollet2BondCorner2.y += dCentreOffsetY;

		stWin.coCorner1 = stCollet2BondCorner1;
		stWin.coCorner2 = stCollet2BondCorner2;
	}
	else
	{
		LONG lBHZ1BondPosOffsetX = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"];
		LONG lBHZ1BondPosOffsetY = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"];
		if( m_dCalibX!=0.0 )
			dCentreOffsetX = lBHZ1BondPosOffsetX / m_dCalibX;
		if( m_dCalibY!=0.0 )
			dCentreOffsetY = lBHZ1BondPosOffsetY / m_dCalibY;
		stCollet1BondCorner1.x += dCentreOffsetX;
		stCollet1BondCorner1.y += dCentreOffsetY;
		stCollet1BondCorner2.x += dCentreOffsetX;
		stCollet1BondCorner2.y += dCentreOffsetY;

		stWin.coCorner1 = stCollet1BondCorner1;
		stWin.coCorner2 = stCollet1BondCorner2;
	}

	stCmd.stOpRegion.uwNumOfCorners = PR_NO_OF_CORNERS;
	if (bColletOffset)
	{
		stCmd.stOpRegion.acoCorners[PR_UPPER_LEFT] = stWin.coCorner1;
		stCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT] = stWin.coCorner2;
		stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x = stWin.coCorner2.x;
		stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y = stWin.coCorner1.y;
		stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x = stWin.coCorner1.x;
		stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y = stWin.coCorner2.y;
	}
	else
	{
		stCmd.stOpRegion.acoCorners[PR_UPPER_LEFT] = m_stSearchArea.coCorner1;
		stCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT] = m_stSearchArea.coCorner2;
		stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x = m_stSearchArea.coCorner2.x;
		stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y = m_stSearchArea.coCorner1.y;
		stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x = m_stSearchArea.coCorner1.x;
		stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y = m_stSearchArea.coCorner2.y;
	}

	stCmd.emGraphicInfo = PR_SHOW_SEARCH_CONTOUR;

	//SYS_GetTime(&ulStartTime);

	PR_ExtractObjCmd(&stCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &uwCommunStatus);
	if (uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(uwCommunStatus))
	{
		//printf("PR_ExtShapeCmd() is finished with status %u!\n",
		//		uwCommunStatus);
		//break;
		szMsg.Format("PR_ExtShapeCmd() is finished with status %u!\n",
				uwCommunStatus);
		HmiMessage(szMsg);
	}
	PR_ExtractObjRpy1(MS899_BOND_CAM_SEND_ID, &stRpy1);

	if (stRpy1.uwCommunStatus != PR_COMM_NOERR ||	PR_ERROR_STATUS(stRpy1.uwPRStatus))
	{
		//printf("PR_ExtractObjRpy1 is finished with status %u, %u!\n",
		//		stRpy1.uwCommunStatus, stRpy1.uwPRStatus);
		//break;
		szMsg.Format("PR_ExtractObjRpy1 is finished with status %u, %u!\n",
				stRpy1.uwCommunStatus, stRpy1.uwPRStatus);
		HmiMessage(szMsg);
	}
	PR_ExtractObjRpy2(MS899_BOND_CAM_SEND_ID, &stRpy2);

	//SYS_GetTime(&ulEndTime);

	if (stRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR ||	PR_ERROR_STATUS(stRpy2.stStatus.uwPRStatus))
	{
		//printf("PR_ExtShapeRpy is finished with status %u, %u!\n",
		//		stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
		//break;
		szMsg.Format("PR_ExtShapeRpy is finished with status %u, %u!\n",
				stRpy2.stStatus.uwCommunStatus, stRpy2.stStatus.uwPRStatus);
		//MFEvent(szMsg);
	}

	switch (stRpy2.emShapeFound)
	{
	case PR_OBJSHAPE_FOUND_NO:
		{
			szMsg.Format("No Object find");HmiMessage(szMsg);
		}
		break;
	case PR_OBJSHAPE_FOUND_BLOB:
		{
			szMsg.Format("Number of Blob Object %d", stRpy2.stResult.uwNumOfObject);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			for (i = 0; i < stRpy2.stResult.uwNumOfObject && i < PR_MAX_ALIGN_MULTI_OBJECTS; i++)
			{
				szMsg.Format("Blob Object %d: Center: (%f, %f)\n", i, stRpy2.stResult.astBlobResult[i].rcoCenter.x, stRpy2.stResult.astBlobResult[i].rcoCenter.y);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				szMsg.Format("Blob Object %d: Area: %f\n", i, stRpy2.stResult.astBlobResult[i].rArea/*,stRpy2.stResult.astBlobResult[i].rAngle*/);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				szMsg.Format("Blob Object %d: Width: %f, Height: %f\n", i, stRpy2.stResult.astBlobResult[i].rszSize.x, stRpy2.stResult.astBlobResult[i].rszSize.y);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				szMsg.Format("Blob Object Search Win,%d,%d",(stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x+stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x)/2,
					(stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y+stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y)/2);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			}
			PR_COORD stRelMove;
			INT nOffsetX = 0,nOffsetY = 0;
			stRelMove.x = stRpy2.stResult.astBlobResult[0].rcoCenter.x - (stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x+stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x)/2;
			stRelMove.y = stRpy2.stResult.astBlobResult[0].rcoCenter.y - (stCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y+stCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y)/2;
			szMsg.Format("pixel,%d,%d",stRelMove.x,stRelMove.y);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			ConvertPixelToMotorStep(stRelMove, &nOffsetX, &nOffsetY);
			CalculateNDieOffsetXY(nOffsetX, nOffsetY);		//v3.77
			szMsg.Format("offset,%d,%d",nOffsetX,nOffsetY);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			dX = m_dBTXRes *1000 *(DOUBLE)nOffsetX;
			dY = m_dBTYRes *1000 *(DOUBLE)nOffsetY;
			//dAngle = stRpy2.stResult.astBlobResult[0].rAngle;
		}
		break;
	case PR_OBJSHAPE_FOUND_CIRCLE:
		{
			szMsg.Format("Number of Circle Object %d", stRpy2.stResult.uwNumOfObject);
			//MFEvent(szMsg);

			for (i = 0; i < stRpy2.stResult.uwNumOfObject && i < PR_MAX_ALIGN_MULTI_OBJECTS; i++)
			{
				szMsg.Format("Circle Object %d: Center: (%f, %f)\n", i, stRpy2.stResult.astCircleResult[i].rcoCentre.x, stRpy2.stResult.astCircleResult[i].rcoCentre.y);
				//MFEvent(szMsg);
				szMsg.Format("Circle Object %d: Radius: %f\n", i, stRpy2.stResult.astCircleResult[i].rRadius);
				//MFEvent(szMsg);
				szMsg.Format("Circle Object %d: Score: %f\n", i, stRpy2.stResult.astCircleResult[i].rScore);
				//MFEvent(szMsg);
			}
		}
		break;
	case PR_OBJSHAPE_FOUND_MULTILINE:
		{
			szMsg.Format("Number of Circle Object %d", stRpy2.stResult.uwNumOfObject);
			//MFEvent(szMsg);

			for (i = 0; i < stRpy2.stResult.uwNumOfObject && i < PR_MAX_ALIGN_MULTI_OBJECTS; i++)
			{
				szMsg.Format("MultiLine Object %d: Center: (%f, %f)\n", i, stRpy2.stResult.astMultiLineResult[i].rcoPos.x, stRpy2.stResult.astMultiLineResult[i].rcoPos.y);
				//MFEvent(szMsg);
				szMsg.Format("MultiLine Object %d: Angle: %f\n", i, stRpy2.stResult.astMultiLineResult[i].rAngle);
				//MFEvent(szMsg);
				szMsg.Format("MultiLine Object %d: Score: %f\n", i, stRpy2.stResult.astMultiLineResult[i].rScore);
			    //MFEvent(szMsg);
			}
		}
		break;
	case PR_OBJSHAPE_FOUND_CROSS_LANDMARK:
		{
			szMsg.Format("Number of Landmark Object: %u \n", stRpy2.stResult.uwNumOfObject);
			//MFEvent(szMsg);
			for (i = 0; i < stRpy2.stResult.uwNumOfObject && i < PR_MAX_ALIGN_MULTI_OBJECTS; i++)
			{
				szMsg.Format("Cross Landmark Object %d ( %.2f , %.2f )( %.2f , %.2f )\n",
						i, stRpy2.stResult.astCrosslandmarkResult[i].rcoCentre.x, stRpy2.stResult.astCrosslandmarkResult[i].rcoCentre.y,
						stRpy2.stResult.astCrosslandmarkResult[i].rcoCentre.x / 16, stRpy2.stResult.astCrosslandmarkResult[i].rcoCentre.y / 16);
				//MFEvent(szMsg);
			}
		}
	}
	//_getch();

	return PR_TRUE;
}


LONG CBondPr::GetBondPosOffsetXPixel()
{
	return m_lBondPosOffsetXPixel;
}


LONG CBondPr::GetBondPosOffsetYPixel()
{
	return m_lBondPosOffsetYPixel;
}