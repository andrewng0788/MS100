/////////////////////////////////////////////////////////////////
// WPR_SrchDie.cpp : Search Die Function of the CWaferPr class
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
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "TakeTime.h"
#include "BondResult.h"
#include "MS896A.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"
#include "WaferMap.h"
#include "BondPr.h"
#include "BondHead.h"
#include "BinTable.h"
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferPr::DieIsAlignable(PR_UWORD usDieType)
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
        (usDieType == PR_ERR_INSUFF_EPOXY_COVERAGE)			||
		PR_WARN_STATUS(usDieType))
	{
        return TRUE;
    }
    return FALSE;
}


BOOL CWaferPr::DieIsGood(PR_UWORD usDieType)
{
	if (usDieType == PR_ERR_GOOD_DIE)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CWaferPr::DieIsDefective(PR_UWORD usDieType)
{
	if ((usDieType == PR_ERR_SCRATCH_DIE)			||
		(usDieType == PR_ERR_DEFECTIVE_DIE)			||
		(usDieType == PR_ERR_RJT_INT)				||
		(usDieType == PR_ERR_RJT_PAT)				||
		(usDieType == PR_ERR_DIE_SIZE_RJT)			||
		(usDieType == PR_ERR_PHOTORESIPR_ERR)		||
		(usDieType == PR_ERR_PROBE_MARK_SIZE_RJT)	||
		(usDieType == PR_ERR_BOND_PAD_SIZE_RJT)		||
		(usDieType == PR_ERR_BOND_PAD_OFFSET)		||
		(usDieType == PR_ERR_BOND_PAD_FOREIGN_MAT)	||
		(usDieType == PR_ERR_NO_CHAR_FOUND)			||
		(usDieType == PR_ERR_LIGHT_EMITTING_FOREIGN_MAT))
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CWaferPr::DieIsBadCut(PR_UWORD usDieType)
{
	if (usDieType == PR_ERR_INSUFF_EPOXY_COVERAGE)
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CWaferPr::DieIsInk(PR_UWORD usDieType)
{
	if (usDieType == PR_ERR_INK_DIE)
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CWaferPr::DieIsChip(PR_UWORD usDieType)
{
	if (usDieType == PR_ERR_CHIP_DIE)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CWaferPr::DieIsOutAngle(DOUBLE dDieAngle)
{
	if ( m_bThetaCorrection == TRUE ) 
	{
		if (fabs(dDieAngle) > m_dMaxDieAngle)
		{
			return TRUE;
		}
	}
	else
	{
		if (fabs(dDieAngle) > m_dAcceptDieAngle)
		{
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CWaferPr::DieNeedRotate(PR_REAL fDieRotate)
{
	if (m_bThetaCorrection == TRUE)
	{
		if ( (fabs(fDieRotate) <= m_dMaxDieAngle) 
			 && (fabs(fDieRotate) >= m_dMinDieAngle) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

VOID CWaferPr::UpdateDieTypeCounter(PR_UWORD usDieType, DOUBLE dDieAngle)
{
	//No need to update counters if current die not available on map to eliminate miss-counting
	ULONG ulRow = (ULONG)(LONG) (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
	ULONG ulCol = (ULONG)(LONG) (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];
	
	//Due to UpdateDieTypeCounter() is called in a bonding sequence 2 times, check NoDieSkipCount is updated?
	BOOL bIsNoDieSkipCountUpdated = FALSE;
	if (m_ulPreviousUpdateDieTypeCounterRow == ulRow && m_ulPreviousUpdateDieTypeCounterCol == ulCol)
	{
		bIsNoDieSkipCountUpdated = TRUE;
	}
	m_ulPreviousUpdateDieTypeCounterRow = ulRow;
	m_ulPreviousUpdateDieTypeCounterCol = ulCol;

	UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
	if ( (ucGrade == m_WaferMapWrapper.GetNullBin()) &&
		 (!m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol)) )
	{
		return;
	}

	if( IsBlkFunc2Enable() )
	{
		if (m_WaferMapWrapper.GetReader() != NULL)
		{
			UCHAR ucInvalidGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetInvalidBin();
			if( ucGrade==ucInvalidGrade )
			{
				return;
			}
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel() )
	{
		BOOL bIsGoodDie = FALSE;
		CUIntArray aulSelectedGradeList;
		m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
		for (int i=0; i<aulSelectedGradeList.GetSize(); i++)
		{
			if( ucGrade==(UCHAR)aulSelectedGradeList[i] )
			{
				bIsGoodDie = TRUE;
				break;
			}
		}
		if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) )
		{
			bIsGoodDie = TRUE;
		}

		if( bIsGoodDie==FALSE  && DieIsAlignable(usDieType) )
		{
			return ;
		}
	}

	if( IsPrescanEnable() )
	{
		ULONG ulDieState = GetMapDieState(ulRow, ulCol);
		UCHAR ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();
		if( IsDieUnpickScan(ulDieState) )	//	427TX	2
		{
			return ;
		}
		if( m_bPrescanSkipNgGrade )
		{
			if( IsScanMapNgGrade( ucGrade-ucGradeOffset ) ||
				(m_ucScanMapStartNgGrade==0 && ucGrade==ucGradeOffset) )
			{	// if PR step on the ng grade, skip the PR counter.
				return ;
			}
		}
	}

	//Update Die type counter
	if (DieIsGood(usDieType) == TRUE)
	{
		if (!IsNGBlock(ulRow, ulCol) && (DieIsOutAngle(dDieAngle) == TRUE))
		{
			m_ulPRSkipCount++;
			m_ulStartSkipCount++;	// out of die angle
		}
		else
		{
			m_ulSkipCount = 0;
			m_ulNoDieSkipCount = 0;
			m_ulPRSkipCount = 0;
			m_ulBadCutSkipCount = 0;
		}
	}
	else if (DieIsAlignable(usDieType) == FALSE)
	{
		if (m_lCurrSrchDieId == m_lMaxSortNmlDieID || !m_bCheckAllNmlDie )
		{
			if (!IsNGBlock(ulRow, ulCol))
			{
				m_ulSkipCount++;
				m_ulNoDieSkipCount++;
				m_ulWaferDefectDieCount++;
				m_ulStartSkipCount++;	//	empty(no) die
			}
		}
		CString szMsg;
		LONG lHmiRow = 0,lHmiCol = 0;
		ConvertAsmToHmiUser(ulRow,ulCol,lHmiRow,lHmiCol);
		szMsg.Format("PR No normal Die at (%lu,%lu),Hmi(%d,%d),MaxSortNmlDieID%d,CheckAllNmlDie%d,NG%d", 
			ulRow, ulCol,lHmiRow,lHmiCol,m_lMaxSortNmlDieID,m_bCheckAllNmlDie, IsNGBlock(ulRow, ulCol));
		//CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		DisplaySequence("WPR - " + szMsg);
/*
		if( m_lMinDieDistX!=0 || m_lMinDieDistY!=0 )
		{
			CString szMapFilename = GetMapNameOnly();
			LONG lUserRow = 0, lUserCol = 0, lDieX = 0, lDieY = 0;
			if( GetPrescanWftPosn(ulRow, ulCol, lDieX, lDieY) )
			{
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				szMsg.Format(",CaseNo0,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DieX,%ld,DieY,%ld",
					szMapFilename, ulRow, ulCol, lUserRow, lUserCol, lDieX, lDieY);
				SaveBadCutEvent(szMsg);
			}
			if( ulRow>0 && GetPrescanWftPosn(ulRow-1, ulCol, lDieX, lDieY) )
			{
				ConvertAsmToOrgUser(ulRow-1, ulCol, lUserRow, lUserCol);
				szMsg.Format(",CaseNo1,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DieX,%ld,DieY,%ld",
					szMapFilename, ulRow-1, ulCol, lUserRow, lUserCol, lDieX, lDieY);
				SaveBadCutEvent(szMsg);
			}
			if( ulCol>0 && GetPrescanWftPosn(ulRow, ulCol-1, lDieX, lDieY) )
			{
				ConvertAsmToOrgUser(ulRow, ulCol-1, lUserRow, lUserCol);
				szMsg.Format(",CaseNo2,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DieX,%ld,DieY,%ld",
					szMapFilename, ulRow, ulCol-1, lUserRow, lUserCol, lDieX, lDieY);
				SaveBadCutEvent(szMsg);
			}
			if( GetPrescanWftPosn(ulRow, ulCol+1, lDieX, lDieY) )
			{
				ConvertAsmToOrgUser(ulRow, ulCol+1, lUserRow, lUserCol);
				szMsg.Format(",CaseNo3,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DieX,%ld,DieY,%ld",
					szMapFilename, ulRow, ulCol+1, lUserRow, lUserCol, lDieX, lDieY);
				SaveBadCutEvent(szMsg);
			}
			if( GetPrescanWftPosn(ulRow+1, ulCol, lDieX, lDieY) )
			{
				ConvertAsmToOrgUser(ulRow+1, ulCol, lUserRow, lUserCol);
				szMsg.Format(",CaseNo4,%s,Y,%lu,X,%lu,UserY,%ld,UserX,%ld,DieX,%ld,DieY,%ld",
					szMapFilename, ulRow+1, ulCol, lUserRow, lUserCol, lDieX, lDieY);
				SaveBadCutEvent(szMsg);
			}
		}
*/
	}
	else if (DieIsDefective(usDieType) == TRUE)
	{
		m_ulSkipCount++;
		m_ulPRSkipCount++;
		m_ulDefectSkipCount++;
		m_ulWaferDefectDieCount++;
		m_ulStartSkipCount++;	//	defective die
	}
	else if (DieIsBadCut(usDieType) == TRUE)
	{
		m_ulSkipCount++;
		m_ulBadCutSkipCount++;
		m_ulWaferDefectDieCount++;
		m_ulStartSkipCount++;	//	bad cut die
	}
	else
	{
		// Treat unclassified die as defective die
		m_ulSkipCount++;
		m_ulPRSkipCount++;
		m_ulDefectSkipCount++;
		m_ulWaferDefectDieCount++;
		m_ulStartSkipCount++;	//	unclassified die as defective
	}
}

BOOL CWaferPr::LogDieResult(CONST LONG lRow, CONST LONG lCol, CONST LONG lStatus, PR_UWORD usDieType, BOOL bLookForward)
{
	CString szLine;
	CString szType;

	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);

	szLine.Format("%d,%d,(%d,%d),%d,%d,", lRow, lCol, lUserRow, lUserCol, bLookForward, lStatus);

	switch (usDieType)
	{
	case PR_ERR_GOOD_DIE:
		szType = "PR_ERR_GOOD_DIE";	break;
	case PR_ERR_INK_DIE:
 		szType = "PR_ERR_INK_DIE";	break;
	case PR_ERR_CHIP_DIE:
 		szType = "PR_ERR_CHIP_DIE";	break;
	case PR_ERR_BOND_PAD_SIZE_RJT:
		szType = "PR_ERR_BOND_PAD_SIZE_RJT";	break;
    case PR_ERR_BOND_PAD_FOREIGN_MAT:
		szType = "PR_ERR_BOND_PAD_FOREIGN_MAT";	break;
    case PR_ERR_LIGHT_EMITTING_FOREIGN_MAT:
		szType = "PR_ERR_LIGHT_EMITTING_FOREIGN_MAT";	break;
    case PR_ERR_DEFECTIVE_DIE:
		szType = "PR_ERR_DEFECTIVE_DIE";	break;
    case PR_ERR_EXCEED_INTENSITY_VAR:
		szType = "PR_ERR_EXCEED_INTENSITY_VAR";	break;
    case PR_WARN_SMALL_SRCH_WIN:
		szType = "PR_WARN_SMALL_SRCH_WIN";	break;
    case PR_WARN_TOO_MANY_DIE:
		szType = "PR_WARN_TOO_MANY_DIE";	break;
    case PR_WARN_BACKUP_ALIGNED:
		szType = "PR_WARN_BACKUP_ALIGNED";	break;
    case PR_ERR_NOERR:
		szType = "PR_ERR_NOERR";	break;
    case PR_ERR_INSUFF_EPOXY_COVERAGE:
		szType = "PR_ERR_INSUFF_EPOXY_COVERAGE";	break;
	case PR_ERR_SCRATCH_DIE:
		szType = "PR_ERR_SCRATCH_DIE";	break;
	case PR_ERR_RJT_INT:
		szType = "PR_ERR_RJT_INT";	break;
	case PR_ERR_RJT_PAT:
		szType = "PR_ERR_RJT_PAT";	break;
	case PR_ERR_DIE_SIZE_RJT:
		szType = "PR_ERR_DIE_SIZE_RJT";	break;
	case PR_ERR_PHOTORESIPR_ERR:
		szType = "PR_ERR_PHOTORESIPR_ERR";	break;
	case PR_ERR_PROBE_MARK_SIZE_RJT:
		szType = "PR_ERR_PROBE_MARK_SIZE_RJT";	break;
	case PR_ERR_BOND_PAD_OFFSET:
		szType = "PR_ERR_BOND_PAD_OFFSET";	break;
	case PR_ERR_NO_CHAR_FOUND:
		szType = "PR_ERR_NO_CHAR_FOUND";	break;
	default:
		szType.Format("Unknown Die Type = 0x%x", usDieType);
		break;
	}

	CMSLogFileUtility::Instance()->WT_GetPrLog(szLine + szType);
	return TRUE;
}


VOID CWaferPr::UpdateSearchDieArea(LONG lDieNo, DOUBLE dSearchCodeX, DOUBLE dSearchCodeY, BOOL bUpdate, BOOL bChangeCam)
{
	BOOL	bSeperateXY = TRUE;

	if ( PR_NotInit() )
	{
		return;
	}


	if( IsEnableZoom() && GetPrescanPrID()==3 && lDieNo==2 )
	{
		return ;
	}

	PR_DRAW_SRCH_AREA_CMD	stDrawAreaCmd;
	PR_DRAW_SRCH_AREA_RPY	stDrawAreaRpy;
	PR_WIN					stDispSearchArea;
	short					i = 0;


	//v4.04	//Klocwork		//v4.46T7	//PLLM
	stDispSearchArea.coCorner1.x = (PR_WORD)GetPrCenterX() - (PR_WORD)((dSearchCodeX * 1.0 / 4 + 0.5) * GetDieSizePixelX(lDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
	stDispSearchArea.coCorner1.y = (PR_WORD)GetPrCenterY() - (PR_WORD)((dSearchCodeY * 1.0 / 4 + 0.5) * GetDieSizePixelY(lDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
	stDispSearchArea.coCorner2.x = (PR_WORD)GetPrCenterX() + (PR_WORD)((dSearchCodeX * 1.0 / 4 + 0.5) * GetDieSizePixelX(lDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
	stDispSearchArea.coCorner2.y = (PR_WORD)GetPrCenterY() + (PR_WORD)((dSearchCodeY * 1.0 / 4 + 0.5) * GetDieSizePixelY(lDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);

	VerifyPRRegion(&stDispSearchArea);

	if (bChangeCam)
	{
		//Display search area on PR 
		ChangeCamera(WPR_CAM_WAFER);
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	stDrawAreaCmd.stWin = stDispSearchArea;
	PR_DrawSrchAreaCmd(&stDrawAreaCmd, ubSID, ubRID, &stDrawAreaRpy);

	if (bUpdate == TRUE)
	{
		m_stSearchArea = stDispSearchArea;

		//Update LookForward search area
		CalculateLFSearchArea();
	}
}

//calculate the search die area 
//VOID CWaferPr::GetSearchDieArea(PR_WIN* stSrchArea,INT nDieNo)
VOID CWaferPr::GetSearchDieArea(PR_WIN* stSrchArea, INT nDieNo, INT nEnlarge)
{
	nDieNo = min(nDieNo, WPR_MAX_DIE-1);	//Klocwork	//v4.02T5

	if (nEnlarge > 0)
	{
		stSrchArea->coCorner1.x = (PR_WORD)GetPrCenterX() - (PR_WORD)(((GetGenSrchAreaX(nDieNo)+nEnlarge)*1.0/4 + 0.5) * GetDieSizePixelX(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner1.y = (PR_WORD)GetPrCenterY() - (PR_WORD)(((GetGenSrchAreaY(nDieNo)+nEnlarge)*1.0/4 + 0.5) * GetDieSizePixelY(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.x = (PR_WORD)GetPrCenterX() + (PR_WORD)(((GetGenSrchAreaX(nDieNo)+nEnlarge)*1.0/4 + 0.5) * GetDieSizePixelX(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.y = (PR_WORD)GetPrCenterY() + (PR_WORD)(((GetGenSrchAreaY(nDieNo)+nEnlarge)*1.0/4 + 0.5) * GetDieSizePixelY(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
	}
	else	//Default
	{
		stSrchArea->coCorner1.x = (PR_WORD)GetPrCenterX() - (PR_WORD)((GetGenSrchAreaX(nDieNo)*1.0/4 + 0.5) * GetDieSizePixelX(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner1.y = (PR_WORD)GetPrCenterY() - (PR_WORD)((GetGenSrchAreaY(nDieNo)*1.0/4 + 0.5) * GetDieSizePixelY(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.x = (PR_WORD)GetPrCenterX() + (PR_WORD)((GetGenSrchAreaX(nDieNo)*1.0/4 + 0.5) * GetDieSizePixelX(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
		stSrchArea->coCorner2.y = (PR_WORD)GetPrCenterY() + (PR_WORD)((GetGenSrchAreaY(nDieNo)*1.0/4 + 0.5) * GetDieSizePixelY(nDieNo) * WPR_SEARCHAREA_SHIFT_FACTOR);
	}

	VerifyPRRegion(stSrchArea);
}

PR_UWORD CWaferPr::SearchDieCmd(BOOL bDieType, LONG lInputDieNo, PR_UBYTE ubSID, PR_UBYTE ubRID, PR_COORD stDieULC, PR_COORD stDieLRC, 
								PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, 
								LONG lGraphicInfo, FLOAT fOrientation, UCHAR ucColletID, BOOL bRepeatAngleChk)
{
    PR_2_POINTS_DIE_ALIGN_CMD       st2PointsAlignCmd;
    PR_QUAD_DIE_ALIGN_CMD           stQuadAlignCmd;
    PR_STREET_DIE_ALIGN_CMD         stStreetAlignCmd;
    PR_DIE_ALIGN_PAR                stAlignPar;

    PR_TMPL_DIE_INSP_CMD            stTmplInspCmd;
    PR_LED_DIE_INSP_CMD             stLedInspCmd;

    PR_DIE_ALIGN_CMD                stDieAlignCmd;
	PR_DIE_INSP_CMD 				stDieInspCmd;
	PR_POST_INSP_CMD 			    stBadCutInspCmd;
    PR_POST_INDIV_INSP_CMD			stBadCutCmd;


	PR_SRCH_DIE_CMD                 stSrchCmd; 

	PR_COORD						stCoDieCenter;
	PR_UWORD						lPrSrchID;
	PR_UWORD                        uwCommunStatus;
	PR_BOOLEAN						bBackupAlign = PR_FALSE;
	PR_BOOLEAN						bCheckDefect = PR_TRUE;
	PR_BOOLEAN						bCheckChip = PR_TRUE;
	PR_UBYTE						ucDefectThres = PR_NORMAL_DEFECT_THRESHOLD;
	LONG							lAlignAccuracy;
	LONG							lGreyLevelDefect;
	LONG							lDieNo = 0;
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
    PR_InitPostInspCmd(&stBadCutInspCmd);
    PR_InitPostIndivInspCmd(&stBadCutCmd);

	PR_POST_INDIV_INSP_EXT1_CMD		stBadCutExtCmd = stBadCutInspCmd.stEpoxy.stIndivExt1;	//v4.07
	

	PR_InitDieAlignPar(&stAlignPar);
    PR_InitSrchDieCmd(&stSrchCmd); 

	PR_REAL prStartAngle = (PR_REAL)(GetSearchStartAngle() + fOrientation);
	PR_REAL prEndAngle	= (PR_REAL)(GetSearchEndAngle() + fOrientation);
	PR_REAL prAngle = 0;


	if (IsMS90Sorting2ndPart())		//sort mode		//v4.57A2
	{
		prAngle = 180.0f;
	}

	//Search BH mark&Collot without wafer second part
	if ((bDieType == WPR_REFERENCE_DIE) && 
		((lInputDieNo == WPR_REFERENCE_PR_DIE_INDEX6) || (lInputDieNo == WPR_REFERENCE_PR_DIE_INDEX5) ||
		(lInputDieNo == WPR_REFERENCE_PR_DIE_INDEX7) || (lInputDieNo == WPR_REFERENCE_PR_DIE_INDEX8)))
	{
		prStartAngle = (PR_REAL)(PR_SRCH_START_ANGLE + fOrientation);
		prEndAngle	= (PR_REAL)(PR_SRCH_END_ANGLE + fOrientation);
		prAngle = 0;
	}

	/////////////////////////////////////////////
	// Special coding for Nichia MS90 Uplook PR to force using 0 degree orentiation
	// no matter 0 or 180 degree wafer rotation on MS90;
	if (fOrientation == 9)//v4.59A36
	{
		prStartAngle	= (PR_REAL) PR_SRCH_START_ANGLE;
		prEndAngle		= (PR_REAL) PR_SRCH_END_ANGLE;
		prAngle			= 0.0f;
	}
	/////////////////////////////////////////////


	if (lInputDieNo <= 0)
		lInputDieNo = 1;
	if (bDieType == WPR_NORMAL_DIE)
		lDieNo = WPR_GEN_NDIE_OFFSET + lInputDieNo;
	if (bDieType == WPR_REFERENCE_DIE)
		lDieNo = WPR_GEN_RDIE_OFFSET + lInputDieNo;
	lPrSrchID	= m_ssGenPRSrchID[lDieNo];

	if (lPrSrchID == 0)
	{
		return IDS_WPR_DIENOTLEARNT;
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


	//Is check defect ??
	if ((m_dGenSrchSingleDefectArea[lDieNo] == 0.0) || (m_dGenSrchTotalDefectArea[lDieNo] == 0.0))
	{
		bCheckDefect = PR_FALSE;
	}
		
	if (m_bGenSrchEnableDefectCheck[lDieNo] == FALSE)
	{
		bCheckDefect = PR_FALSE;
	}

	//Is check Chip ??
	if (m_dGenSrchChipArea[lDieNo] == 0.0)
	{
		bCheckChip = PR_FALSE;
	}

	if (m_bGenSrchEnableChipCheck[lDieNo] == FALSE)
	{
		bCheckChip = PR_FALSE;
	}

	//Check Input parameter
	if ((bInspect == PR_FALSE) || (bAlign == FALSE))
	{
		bCheckDefect = PR_FALSE;
		bCheckChip = PR_FALSE;
	}


	if (bDieType == WPR_REFERENCE_DIE)
	{
		bBackupAlign = PR_FALSE;
	}
	else
	{
		bBackupAlign = (PR_BOOLEAN)m_bSrchEnableBackupAlign;
	}

	//Calculate chip die area, min & total defect area
	dMinChipArea		= ((DOUBLE)(GetDieSizePixelX((UCHAR)lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchChipArea[lDieNo];
	dSingleDefectArea	= ((DOUBLE)(GetDieSizePixelX((UCHAR)lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	dTotalDefectArea	= ((DOUBLE)(GetDieSizePixelX((UCHAR)lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 

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
	st2PointsAlignCmd.rRefAngle					= prAngle;
	st2PointsAlignCmd.coMaxShift.x				= 0;
	st2PointsAlignCmd.coMaxShift.y				= 0;
	st2PointsAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	st2PointsAlignCmd.szPosConsistency.x		= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.szPosConsistency.y		= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	st2PointsAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	VerifyPRPoint(&stCoDieCenter);
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
	stStreetAlignCmd.rRefAngle					= prAngle;
	stStreetAlignCmd.emAlignAccuracy			= (PR_DIE_ALIGN_ACCURACY)(lAlignAccuracy);
	stStreetAlignCmd.szPosConsistency.x			= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	stStreetAlignCmd.szPosConsistency.y			= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	stStreetAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	stStreetAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	VerifyPRPoint(&stCoDieCenter);
	stStreetAlignCmd.coSelectPoint				= stCoDieCenter;
	stStreetAlignCmd.coProbableDieCentre		= stCoDieCenter;
	stStreetAlignCmd.rDieRotTol					= PR_SRCH_ROT_TOL;

	if (bRepeatAngleChk)
	{
		stStreetAlignCmd.emRepeatAngleCheck		= PR_REPEAT_ANGLE_CHK_REJECT;
		stStreetAlignCmd.rRepeatAngle			= 180;
		stStreetAlignCmd.emSelectMode			= PR_SRCH_DIE_SELECT_MODE_HIGHEST_SCORE;
	}
	else
	{
		stStreetAlignCmd.emRepeatAngleCheck		= PR_REPEAT_ANGLE_CHK_DISABLE;
	}

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

	//v3.17T1
	//Line Defect detection; only available for Normal-Die
	if (bDieType == WPR_NORMAL_DIE)
	{
		if (m_bEnableLineDefect)
		{
			stTmplInspCmd.emEnableLineDefectDetection		= PR_TRUE;
			stTmplInspCmd.uwNumLineDefects					= (PR_UWORD)m_lMinNumOfLineDefects;
			stTmplInspCmd.rMinLongestSingleLineDefectLength = (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength);		//mil -> pixel
			stTmplInspCmd.rMinLineClassLength				= (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength) / 2;
			stTmplInspCmd.rMinTotalLineDefectLength			= (PR_REAL)(LONG) ConvertDUnitToPixel(m_dMinLineDefectLength) * 2;

/*
CString szTemp;
szTemp.Format("Enable Line defect... %d  %.2f  %.2f  %.2f", stTmplInspCmd.uwNumLineDefects, 
													stTmplInspCmd.rMinLongestSingleLineDefectLength,
													stTmplInspCmd.rMinLineClassLength,
													stTmplInspCmd.rMinTotalLineDefectLength);
AfxMessageBox(szTemp, MB_SYSTEMMODAL);
*/		
		}
	}

	stDieAlignCmd.st2Points						= st2PointsAlignCmd;
	stDieAlignCmd.stStreet						= stStreetAlignCmd;
	if( m_bAOINgPickPartDie && bDieType==WPR_REFERENCE_DIE && lInputDieNo==1 )	//	search die CG Alignment parameter
	{
		PR_WIN stSrchArea;
		GetSearchDieArea(&stSrchArea, 1 + WPR_GEN_RDIE_OFFSET);
		stDieULC = stSrchArea.coCorner1;
		stDieLRC = stSrchArea.coCorner2;
		stDieAlignCmd.stLnFit.stLargeSrchWin.coCorner1 = stDieULC;
		stDieAlignCmd.stLnFit.stLargeSrchWin.coCorner2 = stDieLRC;

		stDieAlignCmd.stLnFit.stMinDieSizeTol.x = (PR_REAL)m_lNgPickDieAreaTolMin;
		stDieAlignCmd.stLnFit.stMinDieSizeTol.y = (PR_REAL)m_lNgPickDieAreaTolMin;
		stDieAlignCmd.stLnFit.stMaxDieSizeTol.x = (PR_REAL)m_lNgPickDieAreaTolMax;
		stDieAlignCmd.stLnFit.stMaxDieSizeTol.y = (PR_REAL)m_lNgPickDieAreaTolMax;
		stDieAlignCmd.stLnFit.emSingleDieSrch	= PR_TRUE;
		stDieAlignCmd.stLnFit.emPartialDieSrch	= PR_TRUE;
		stDieAlignCmd.stLnFit.emMaxDieSizeChk	= PR_TRUE;
		stDieAlignCmd.stLnFit.emMinDieSizeChk	= PR_TRUE;

		stSrchCmd.stDieAlignPar.rAngleRange		= 30.0f;
	}

	stDieInspCmd.stTmpl							= stTmplInspCmd;

	stAlignPar.emEnableBackupAlign				= bBackupAlign;
	stAlignPar.rStartAngle						= prStartAngle;
	stAlignPar.rEndAngle						= prEndAngle;		
	//stAlignPar.emIsTurnOnMarginalSrch			= PR_TRUE;
	//stAlignPar.ubMarginalRange					= 5;

	//Update the seach die command
	stSrchCmd.emAlignAlg = GetLrnAlignAlgo(lDieNo);
	stSrchCmd.emDieInspAlg = GetLrnInspMethod(lDieNo);

	//Setup badcut parameter
	if ( GetDieShape() == WPR_RECTANGLE_DIE )
	{
		stBadCutCmd.emEpoxyInsuffChk				= PR_TRUE;
		stBadCutCmd.emEpoxyExcessChk				= PR_FALSE;
		stBadCutCmd.uwMaxInsuffSide					= 0;
		stBadCutCmd.stMinPercent.x					= (PR_REAL)(100 - m_stBadCutTolerenceX);
		stBadCutCmd.stMinPercent.y					= (PR_REAL)(100 - m_stBadCutTolerenceY);
		stBadCutCmd.szScanWidth.x					= (PR_LENGTH)m_stBadCutScanWidthX;
		stBadCutCmd.szScanWidth.y					= (PR_LENGTH)m_stBadCutScanWidthY;
		stBadCutCmd.stEpoxyInsuffWin.coCorner1		= m_stBadCutPoint[0];
		stBadCutCmd.stEpoxyInsuffWin.coCorner2		= m_stBadCutPoint[1];
		stBadCutCmd.stEpoxyInsuffWin.coObjCentre.x	= (PR_WORD)GetPrCenterX();
		stBadCutCmd.stEpoxyInsuffWin.coObjCentre.y	= (PR_WORD)GetPrCenterY();

		stBadCutInspCmd.emEpoxyAlg					= PR_INDIV_INSP;
		stBadCutInspCmd.stEpoxy.stIndiv				= stBadCutCmd;
	}
	else
	{
		LONG	lCorners	= WPR_HEXAGON_CORNERS;				
		if ( GetDieShape() == WPR_TRIANGULAR_DIE )	//v4.06
		{
			lCorners	= WPR_TRIANGLE_CORNERS;				
		}
		else if ( GetDieShape() == WPR_RHOMBUS_DIE )
		{
			lCorners	= WPR_RHOMBUS_CORNERS;				
		}
		PR_LINE stLine;
		for (LONG i=0; i<lCorners; i++)
		{
			stLine.coPoint1 = m_stBadCutPoint[i];
	
			if ( i >= (lCorners-1) )
			{
				stLine.coPoint2 = m_stBadCutPoint[0];
			}
			else
			{
				stLine.coPoint2 = m_stBadCutPoint[i+1];
			}

			stBadCutExtCmd.auwScanWidth[i]	= (PR_LENGTH)m_stBadCutScanWidthX;
			stBadCutExtCmd.aubMinPercent[i] = (PR_UBYTE)(100 - m_stBadCutTolerenceX);
			stBadCutExtCmd.astInsuffLine[i]	= stLine;
		}
		stBadCutExtCmd.ubNumOfInsuffLine	= (PR_UBYTE)lCorners;

		stBadCutExtCmd.emEpoxyInsuffChk		= PR_TRUE;
		stBadCutExtCmd.emEpoxyExcessChk		= PR_FALSE;
		stBadCutExtCmd.uwMaxInsuffSide		= 0;
		stBadCutExtCmd.coObjCentre.x		= (PR_WORD)GetPrCenterX();
		stBadCutExtCmd.coObjCentre.y		= (PR_WORD)GetPrCenterY();
		stBadCutExtCmd.rObjAngle			= 0.0;

		stBadCutInspCmd.emEpoxyAlg			= PR_INDIV_INSP_EXT1;
		stBadCutInspCmd.stEpoxy.stIndivExt1	= stBadCutExtCmd;
	}
	stBadCutInspCmd.emEpoxyChk			= PR_TRUE;
#ifndef VS_5MCAM
	if( m_bBadcutBiDetect )	//	new library missing, need added in if in need in future.
	{
//		stBadCutInspCmd.emSegMode	= PR_POST_EPOXY_SEG_MODE_BINARY;
//		stBadCutInspCmd.ubThd		= (PR_UBYTE) m_lBadcutBiThreshold;
	}
#endif

	stSrchCmd.stDieAlignPar.rStartAngle				= prStartAngle;
	stSrchCmd.stDieAlignPar.rEndAngle				= prEndAngle;
	stSrchCmd.emRetainBuffer						= bLatch;
	stSrchCmd.emLatch								= bLatch;
	stSrchCmd.emAlign								= bAlign;
	stSrchCmd.emDefectInsp							= bInspect;
	stSrchCmd.emCameraNo							= GetRunCamID();
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
    stSrchCmd.emGraphicInfo							= (PR_GRAPHIC_INFO)(lGraphicInfo); 
	stSrchCmd.stPostBondInsp						= stBadCutInspCmd;
#ifdef	NU_MOTION
	//v4.50A26	//For MS100/MS60/MS90	//v4.51A7
	//typedef enum {PR_DISP_OPTION_DEFAULT,
	//				PR_DISP_OPTION_SKIP_AUTOBOND_IMG,
	//				PR_DISP_OPTION_END} PR_DISP_OPTION;
	stSrchCmd.emDispOption							= PR_DISP_OPTION_DEFAULT;
	if ( (m_ucMS100PrDisplayID != MS_PR_DISPLAY_DEFAULT) && 
		 (ucColletID > 0) && 
		 (bDieType != WPR_REFERENCE_DIE) )
	{
		if ((ucColletID == 1) && (m_ucMS100PrDisplayID != MS_PR_DISPLAY_COLLET1))
		{
			stSrchCmd.emDispOption = PR_DISP_OPTION_SKIP_AUTOBOND_IMG;
		}
		else if ((ucColletID == 2) && (m_ucMS100PrDisplayID != MS_PR_DISPLAY_COLLET2))
		{
			stSrchCmd.emDispOption = PR_DISP_OPTION_SKIP_AUTOBOND_IMG;
		}
	}
#endif

	//No need to care defect or chip check is enabled
	if ( (bAlign = PR_TRUE) && (bInspect == PR_TRUE) && (m_bBadCutDetection == TRUE) && (m_lCurBadCutSizeX > 0) && (m_lCurBadCutSizeY > 0))
	{
		m_bGetBadCutReply = PR_TRUE;
		stSrchCmd.emPostBondInsp = PR_TRUE;  
	}
	else
	{
		m_bGetBadCutReply = FALSE;
		stSrchCmd.emPostBondInsp = PR_FALSE;  
	}

	//No need to do badcut if this die is not normal die
	if (bDieType != WPR_NORMAL_DIE)
	{
		m_bGetBadCutReply = FALSE;
		stSrchCmd.emPostBondInsp = PR_FALSE;  
	}

	MotionSetOutputBit(WPR_SO_SEARCH_DIE_CMD, TRUE);
	PR_SrchDieCmd(&stSrchCmd, ubSID, ubRID, &uwCommunStatus);
    MotionSetOutputBit(WPR_SO_SEARCH_DIE_CMD, FALSE);

	(*m_psmfSRam)["WaferPr"]["Srch Die Record ID"] = stSrchCmd.auwRecordID[0];

	if (uwCommunStatus != PR_COMM_NOERR )
	{
		//CString csMsg;
		//csMsg.Format("WPR Search die error = %x", uwCommunStatus);
		//DisplayMessage(csMsg);
 
		return PR_COMM_ERR;
	}

	return PR_ERR_NOERR;
}


PR_UWORD CWaferPr::SearchDieRpy1(PR_UBYTE ubSID, PR_SRCH_DIE_RPY1 *stSrchRpy1)
{
//	if( IsBurnIn() && m_bNoWprBurnIn )
//	{
//		return PR_ERR_NOERR;
//	}

	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_1, TRUE);
	PR_SrchDieRpy1(ubSID, stSrchRpy1);
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_1, FALSE);

    if (stSrchRpy1->uwCommunStatus != PR_COMM_NOERR)
	{
		//CString csMsg;
		//csMsg.Format("WPR Get reply1 error = %x", stSrchRpy1->uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
    }

    if (stSrchRpy1->uwPRStatus != PR_ERR_NOERR)
	{
		return stSrchRpy1->uwPRStatus;
    }

	return PR_ERR_NOERR;
}


PR_UWORD CWaferPr::SearchDieRpy2(PR_UBYTE ubSID, PR_SRCH_DIE_RPY2 *stSrchRpy2)
{
//	if( IsBurnIn() && m_bNoWprBurnIn )
//	{
//		return PR_ERR_NOERR;
//	}
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_2, TRUE);
    PR_SrchDieRpy2(ubSID, stSrchRpy2);
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_2, FALSE);

	if (stSrchRpy2->stStatus.uwCommunStatus != PR_COMM_NOERR)
    {
		//CString csMsg;
		//csMsg.Format("WPR Get reply2 error = %x", stSrchRpy2->stStatus.uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
	}

	return stSrchRpy2->stStatus.uwPRStatus;
}


PR_UWORD CWaferPr::SearchDieRpy3(PR_UBYTE ubSID, PR_SRCH_DIE_RPY3 *stSrchRpy3)
{
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_3, TRUE);
    PR_SrchDieRpy3(ubSID, stSrchRpy3);
	MotionSetOutputBit(WPR_SO_SEARCH_DIE_REPLY_3, FALSE);

	if (stSrchRpy3->stStatus.uwCommunStatus != PR_COMM_NOERR)
    {
		//CString csMsg;
		//csMsg.Format("WPR Get reply3 error = %x", stSrchRpy3->stStatus.uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
	}

	return stSrchRpy3->stStatus.uwPRStatus;
}

PR_UWORD CWaferPr::SearchDieRpy4(PR_UBYTE ubSID, PR_SRCH_DIE_RPY4 *stSrchRpy4)
{
    PR_SrchDieRpy4(ubSID, stSrchRpy4);


	if (stSrchRpy4->stStatus.uwCommunStatus != PR_COMM_NOERR)
    {
		//CString csMsg;
		//csMsg.Format("WPR Get reply4 error = %x", stSrchRpy4->stStatus.uwCommunStatus);
		//DisplayMessage(csMsg);
		return PR_COMM_ERR;
	}

	return stSrchRpy4->stStatus.uwPRStatus;
}


PR_UWORD CWaferPr::ExtractDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, PR_COORD *stDieSize)
{
	CString szTemp, szLog;
	INT dX0, dX1, dY0, dY1;
	DOUBLE dReturnScore = 0.0;
	(*m_psmfSRam)["WaferTable"]["Die Orientation Score"] = (DOUBLE)-1000.0;
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	*fDieScore		= stSrchRpy2.stDieAlign[0].stGen.rMatchScore;
	switch(stSrchRpy2.stStatus.uwPRStatus)
	{
	case PR_ERR_NOERR:
	case PR_WARN_SMALL_SRCH_WIN:
	case PR_WARN_TOO_MANY_DIE:
	case PR_WARN_BACKUP_ALIGNED:
		if (bUseReply3 == TRUE)
		{
			*usDieType = stSrchRpy3.stStatus.uwPRStatus;

			//Check BadCut	
			if ((m_bGetBadCutReply == TRUE) && (*usDieType == PR_ERR_NOERR))
			{
				//memset(&stSrchRpy4,0,sizeof(stSrchRpy4));
				PR_SRCH_DIE_RPY4 stSrchRpy4;

				if (SearchDieRpy4(ubSID, &stSrchRpy4) == PR_ERR_INSUFF_EPOXY_COVERAGE)
				{
					*usDieType = PR_ERR_INSUFF_EPOXY_COVERAGE;
				}
			}
		}
		else
		{
			*usDieType	= stSrchRpy2.stStatus.uwPRStatus;
		}
		*fDieRotate		= stSrchRpy2.stDieAlign[0].stGen.rDieRot;

		(*m_psmfSRam)["WaferPr"]["Compensate"]["ThetaLog"] = (DOUBLE)stSrchRpy2.stDieAlign[0].stGen.rDieRot;

		if( IsMS90HalfSortMode() )
		{
			if( *fDieRotate>90 )
				*fDieRotate -= 180;
			else if( *fDieRotate<-90 )
				*fDieRotate += 180;
		}
		*stDieOffset	= stSrchRpy2.stDieAlign[0].stGen.coDieCentre;
		*fDieScore		= stSrchRpy2.stDieAlign[0].stGen.rMatchScore;

		dX0 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].x);
		dX1 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[1].x);
		stDieSize->x	= abs(dX0 - dX1);

		dY0 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].y);
		dY1 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[3].y);
		stDieSize->y	= abs(dY0 - dY1);

		if (*usDieType == PR_ERR_NOERR)
		{
			*usDieType	= PR_ERR_GOOD_DIE;
		}
		dReturnScore = stSrchRpy2.stDieAlign[0].stGen.rFuzzifiedScore;

		break;

	case PR_ERR_LOCATED_DEFECTIVE_DIE:
	case PR_ERR_ROTATION:
		*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
		*fDieRotate		= stSrchRpy2.stDieAlign[0].stGen.rDieRot;
		if (IsMS90HalfSortMode())
		{
			if( *fDieRotate>90 )
				*fDieRotate -= 180;
			else if( *fDieRotate<-90 )
				*fDieRotate += 180;
		}
		*stDieOffset	= stSrchRpy2.stDieAlign[0].stGen.coDieCentre;
		*fDieScore		= stSrchRpy2.stDieAlign[0].stGen.rMatchScore;

		dX0 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].x);
		dX1 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[1].x);
		stDieSize->x	= abs(dX0 - dX1);
		
		dY0 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].y);
		dY1 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[3].y);
		stDieSize->y	= abs(dY0 - dY1);

		dReturnScore = stSrchRpy2.stDieAlign[0].stGen.rFuzzifiedScore;
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
		stDieOffset->x	= (PR_WORD)GetPrCenterX();
		stDieOffset->y	= (PR_WORD)GetPrCenterY();
		stDieSize->x	= 0;
		stDieSize->y	= 0;
		break;
	}

	(*m_psmfSRam)["WaferTable"]["Die Orientation Score"] = (DOUBLE) dReturnScore;
	return 0;
}


PR_WORD CWaferPr::ManualSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
								  PR_COORD stCorner1, PR_COORD stCorner2, BOOL bLog, BOOL bRepeatAngleChk)
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	CString				csMsgA;
	CString				csMsgB;
	PR_UWORD			usSearchResult;
    PR_COORD			stPRCoorCentre;
    PR_COORD			stPRDieSize;
	BOOL				bGetRpy3 = FALSE;
	PR_GRAPHIC_INFO		emInfo;

	stPRCoorCentre.x  = (PR_WORD)GetPrCenterX();
	stPRCoorCentre.y  = (PR_WORD)GetPrCenterY();

	m_dCurSrchDieSizeX	= 0;
	m_dCurSrchDieSizeY	= 0;

	if ( m_bDebugVideoTest == TRUE )
		emInfo = PR_SHOW_DEFECT;
	else
		emInfo = PR_NO_DISPLAY;

	if( IsBurnIn() && (m_bNoWprBurnIn ||IsAOIOnlyMachine()) )
	{
        *usDieType = PR_ERR_NOERR;
		*fDieRotate = 0.0;
		stDieOffset->x = 0;
		stDieOffset->y = 0;
		*fDieScore = 0;
		return 0;
	}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	FLOAT fOrientation = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
//	if( m_bCheckDieOrientation)
//	{
//		fOrientation = (FLOAT) (pUtl->GetSearchDieRotation());
//	}

	if (pApp->IsPLLMRebel() && bDieType==WPR_REFERENCE_DIE)	// auto enlarge window for refer
	{
		stCorner1.x = GetPRWinULX();
		stCorner1.y = GetPRWinULY();
		stCorner2.x = GetPRWinLRX() - 1;
		stCorner2.y = GetPRWinLRY() - 1;
	}

	TakeTime(WPR4);
	usSearchResult = SearchDieCmd(bDieType, lDieNo, ubSID, ubRID, stCorner1, stCorner2, bLatch, bAlign, bInspect, emInfo, fOrientation, 0, bRepeatAngleChk);
//	pUtl->SetSearchDieRoation(0);
	if (usSearchResult != PR_ERR_NOERR)
	{
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			csMsgA.Format("WPR Not Learnt");
		}
		else
		{
			csMsgA.Format("Send Command = %x",usSearchResult);
		}
		SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);

		return -1;
	}
	
	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);

	if ((bDieType == WPR_REFERENCE_DIE) && ((lDieNo == WPR_REFERENCE_PR_DIE_INDEX6) || (lDieNo == WPR_REFERENCE_PR_DIE_INDEX5)))
	{
		SetSearchMarkReady(TRUE);
	}

	if (usSearchResult != PR_ERR_NOERR)
	{
		csMsgA.Format("Get Reply 1 = %x",usSearchResult);
		SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		return -1;
	}

	usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		csMsgA.Format("Get Reply 2 = %x",usSearchResult);
		SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		return -1;
	}

    *usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (*usDieType == PR_ERR_NOERR)
	{
        *usDieType = PR_ERR_GOOD_DIE;
    }

	if ((DieIsAlignable(*usDieType) == TRUE) && (bInspect == TRUE))
	{
		bGetRpy3 = TRUE;
		usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			csMsgA.Format("Get Reply 3 = %x",usSearchResult);
			SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
			return -1;
		}
	}


	//v4.49A10	//Only log result in non-AUTOBOND mode
	if (bLog)
	{
		LONG lInputDieNo = lDieNo;
		if (lInputDieNo <= 0)
			lInputDieNo = 1;
		if (bDieType == WPR_NORMAL_DIE)
			lInputDieNo = WPR_GEN_NDIE_OFFSET + lInputDieNo;
		if (bDieType == WPR_REFERENCE_DIE)
			lInputDieNo = WPR_GEN_RDIE_OFFSET + lInputDieNo;
		
		LogUserSearchDieResult(lInputDieNo, stSrchRpy2.stStatus.uwPRStatus, 
								stSrchRpy2.stDieAlign[0].stGen.coDieCentre.x, 
								stSrchRpy2.stDieAlign[0].stGen.coDieCentre.y, 
								stSrchRpy2.stDieAlign[0].stGen.rDieRot, 
								stSrchRpy2.stDieAlign[0].stGen.rMatchScore,
								stCorner1, stCorner2,
								ubSID, ubRID);		//v4.49A10
	}

	//Identify die type
	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore, &stPRDieSize);
	
	if( GetDieShape() == WPR_RHOMBUS_DIE && bDieType==WPR_NORMAL_DIE && lDieNo==0 && bLog )
	{
		for(int i=0; i<WPR_RHOMBUS_CORNERS; i++)
		{
			m_stLearnDieCornerPos[i].x	= (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[i].x);
			m_stLearnDieCornerPos[i].y	= (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[i].y);
		}
	}

	if ((stPRDieSize.x > 0) && (stPRDieSize.y > 0))
	{
		//shiraishi03
		ConvertPixelToDUnit(stPRDieSize, m_dCurSrchDieSizeX, m_dCurSrchDieSizeY, IsMS90());		// for setup display purpose only	//v3.01T1
	}
	
	CalculateDefectScore(*usDieType, lDieNo, &stSrchRpy3);

	return 0;
}


//================================================================
// CalculateDefectScore()
//   Created-By  : Andrew Ng
//   Date        : 1/25/2007 12:17:58 PM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferPr::CalculateDefectScore(PR_UWORD usDieType, CONST LONG lDieNo, PR_SRCH_DIE_RPY3 *stSrchRpy3)
{
	DOUBLE dSArea = stSrchRpy3->stDieInsp[0].stTmpl.aeLargestDefectArea;
	DOUBLE dTArea = stSrchRpy3->stDieInsp[0].stTmpl.aeTotalDefectArea;

	if ((GetDieSizePixelX(lDieNo) == 0) && (GetDieSizePixelY(lDieNo) == 0))
	{
		m_dSpSingleDefectScore	= 0.00;
		m_dSpTotalDefectScore	= 0.00;
		return FALSE;
	}

	if (usDieType == PR_ERR_CHIP_DIE)	// PR checks CHIP first before defect 
	{
		m_dSpSingleDefectScore	= 0.00;
		m_dSpTotalDefectScore	= 0.00;
		m_dSpChipAreaScore		= dSArea / (GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) * 100;
	}
	else
	{
		m_dSpSingleDefectScore	= dSArea / (GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) * 100;
		m_dSpTotalDefectScore	= dTArea / (GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) * 100;
		m_dSpChipAreaScore		= 0.00;
	}

	if ((usDieType == PR_ERR_GOOD_DIE) || (usDieType == PR_ERR_NOERR))
	{
		//Good die
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


//================================================================
// CalculateAutobondDefectScore()
//   Created-By  : Andrew Ng
//   Date        : 1/25/2007 4:15:36 PM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferPr::CalculateAutobondDefectScore(PR_UWORD usDieType, CONST LONG lDieNo, PR_SRCH_DIE_RPY3 *stSrchRpy3)
{
	DOUBLE dSArea = stSrchRpy3->stDieInsp[0].stTmpl.aeLargestDefectArea;
	DOUBLE dTArea = stSrchRpy3->stDieInsp[0].stTmpl.aeTotalDefectArea;

	if (!m_bGenSrchEnableDefectCheck[lDieNo])
	{
		m_dCurSingleDefectScore	= 0.00;
		m_dCurTotalDefectScore	= 0.00;
		m_dCurChipAreaScore		= 0.00;
		m_szCurPrStatus = "";		
		return FALSE;
	}

	if ((GetDieSizePixelX(lDieNo) == 0) && (GetDieSizePixelY(lDieNo) == 0))
	{
		m_dCurSingleDefectScore	= 0.00;
		m_dCurTotalDefectScore	= 0.00;
		m_dCurChipAreaScore		= 0.00;
		m_szCurPrStatus = "";		
		return FALSE;
	}

	if (usDieType == PR_ERR_CHIP_DIE)	// PR checks CHIP first before defect 
	{
		m_dCurSingleDefectScore	= 0.00;
		m_dCurTotalDefectScore	= 0.00;
		m_dCurChipAreaScore		= dSArea / (GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) * 100;
	}
	else
	{
		m_dCurSingleDefectScore	= dSArea / (GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) * 100;
		m_dCurTotalDefectScore	= dTArea / (GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) * 100;
		m_dCurChipAreaScore		= 0.00;
	}

	if ((usDieType == PR_ERR_GOOD_DIE) || (usDieType == PR_ERR_NOERR))
	{
		//Good die
		if (m_dCurSingleDefectScore > m_dGenSrchSingleDefectArea[lDieNo])
			m_dCurSingleDefectScore = m_dGenSrchSingleDefectArea[lDieNo];
		if (m_dCurTotalDefectScore > m_dGenSrchTotalDefectArea[lDieNo])
			m_dCurTotalDefectScore = m_dGenSrchTotalDefectArea[lDieNo];
	}

	ConvertDieTypeToText(usDieType, m_szCurPrStatus);

	return TRUE;
}


VOID CWaferPr::ConvertDieTypeToText(PR_UWORD usDieType, CString &szText)
{
	switch(usDieType)
	{
		case PR_ERR_GOOD_DIE:		
			szText.LoadString(HMB_PRS_GOOD_DIE);
			break;
		case PR_ERR_NO_DIE :		
			szText.LoadString(HMB_PRS_NO_DIE);
			break;
		case PR_ERR_CHIP_DIE:		
			szText.LoadString(HMB_PRS_CHIP_DIE);
			break;
		case PR_ERR_INK_DIE:		
			szText.LoadString(HMB_PRS_CHIP_DIE);
			break;
		case PR_ERR_HALF_DIE:		
			szText.LoadString(HMB_PRS_HALF_DIE);
			break;
		case PR_ERR_RJT_INT:		
			szText.LoadString(HMB_PRS_BLANK_DIE);
			break;
		case PR_ERR_RJT_PAT:		
			szText.LoadString(HMB_PRS_TEST_DIE);
			break;
		case PR_ERR_DEFECTIVE_DIE:	
			szText.LoadString(HMB_PRS_DEFECT_DIE);
			break;
		case PR_ERR_NON_LOCATED_DEFECTIVE_DIE:
			szText.LoadString(HMB_PRS_NL_DEFECT_DIE);
			break;
		case PR_ERR_LOCATED_DEFECTIVE_DIE:
			szText.LoadString(HMB_PRS_L_DEFECT_DIE);
			break;
		case PR_ERR_EXCEED_INTENSITY_VAR:
			szText.LoadString(HMB_PRS_AB_INTENSITY);
			break;
		case PR_ERR_DIE_SIZE_RJT:
			szText.LoadString(HMB_PRS_REJECT_SIZE);
			break;
		case PR_ERR_ROTATION:
			szText.LoadString(HMB_PRS_REJECT_ANGLE);
			break;
		case PR_ERR_ROTATION_EXCEED_SPEC:
			szText.LoadString(HMB_PRS_EXCEED_ANGLE);
			break;
		case PR_ERR_CORNER_ANGLE_FAIL:
			szText.LoadString(HMB_PRS_REJECT_CRN);
			break;
		case PR_ERR_BOND_PAD_SIZE_RJT:
			szText.LoadString(HMB_PRS_REJECT_PAD_SIZE);
			break;
		case PR_ERR_BOND_PAD_FOREIGN_MAT:
			szText.LoadString(HMB_PRS_REJECT_PAD_AREA);
			break;
		case PR_ERR_LIGHT_EMITTING_FOREIGN_MAT:
			szText.LoadString(HMB_PRS_REJECT_LE_AREA);
			break;
		case PR_ERR_INSUFF_EPOXY_COVERAGE:
			szText.LoadString(HMB_PRS_BAD_CUT_DIE);
			break;
		case PR_WARN_BACKUP_ALIGNED:
			szText.LoadString(HMB_PRS_BACKUP_ALIGN);
			break;
		case PR_ERR_SCORE_OUT_OF_SPEC:
			szText = " Score out of spec Die";
			break;

		default:
			szText.Format("Identify Error (%d)\n",usDieType);	
			break;
	}
}

// it should be the real index in the array, not the index for its own type
VOID CWaferPr::DisplaySearchDieResult(PR_UWORD usDieType, BOOL bDieType, CONST LONG lDieNo, PR_REAL fDieRotate, PR_COORD stDieOffset, PR_REAL fDieScore)
{
	CString		csMsgA;
	CString		csMsgB;
	CString		csMsgC = "";
	CString		szTitle;
	int			siXAxis; 
	int			siYAxis; 
	int			siTAxis;
	DOUBLE		dDieAngle = 0;

	CString szMsgRot;
	szMsgRot = "Angle:";
	//reference die number
	CString		csRefDieNo = "";
	if( (lDieNo > WPR_GEN_RDIE_OFFSET) && (bDieType==WPR_REFERENCE_DIE) )
	{
		if( m_bAOINgPickPartDie )	//	search die CG Alignment parameter
			szMsgRot = "Area:";
		csRefDieNo.Format("(Ref #%d)", lDieNo - WPR_GEN_RDIE_OFFSET);		//v3.97	//Lextar
	}

	ConvertDieTypeToText(usDieType, csMsgA);

	szTitle.LoadString(HMB_WPR_SRH_DIE_RESULT);

	GetWaferTableEncoder(&siXAxis, &siYAxis, &siTAxis);
	int			siStepX, 	siStepY;
	CString		csMsgD;
	CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	CalculateNDieOffsetXY(siStepX, siStepY);

	if (IsMS90())	//v4.59A16	//REnesas MS90
	{
		DOUBLE dUnitX=0, dUnitY=0;
		ConvertMotorStepToDUnit(siStepX, siStepY, dUnitX, dUnitY);	//v4.59A18

		csMsgD.Format("PosX:%d (%d) offset (%.1f um)\n PosY:%d (%d) offset (%.1f um)\n Score:%d\n", 
			stDieOffset.x, siXAxis, dUnitX, stDieOffset.y, siYAxis, dUnitY, (LONG)(fDieScore));
	}
	
	if(m_bLearnLookupCamera)
	{
		CString szLog;
		szLog.Format("Uplook --- Search PosX:%d(%d) offset(%d) PosY:%d(%d) offset(%d) Score:%d", 
			stDieOffset.x, siXAxis, siStepX, stDieOffset.y, siYAxis, siStepY, (LONG)(fDieScore));
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog); 
		LONG lLearnUplookX_BH1, lLearnUplookY_BH1, lLearnUplookX_BH2, lLearnUplookY_BH2; 
		LONG lCalDieCentreX_BH1, lCalDieCentreY_BH1, lPRDieCentreX_BH1, lPRDieCentreY_BH1;
		LONG lCalDieCentreX_BH2, lCalDieCentreY_BH2, lPRDieCentreX_BH2, lPRDieCentreY_BH2;
//		LONG lTempSearchUplookX, lTempSearchUplookY; 
//		LONG lOffsetUplookX, lOffsetUplookY;
		DOUBLE dCalibFactorBH1 = 0.27;  // um/pixel from Peter 
		DOUBLE dCalibFactorBH2 = 0.29;
		
		lLearnUplookX_BH1	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn GenDieSize X"];
		lLearnUplookY_BH1	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn GenDieSize Y"];
		lLearnUplookX_BH2	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn GenDieSize X"];
		lLearnUplookY_BH2	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn GenDieSize Y"];

		lCalDieCentreX_BH1	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn CalDieCentre X"];
		lCalDieCentreY_BH1	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn CalDieCentre Y"];
		lPRDieCentreX_BH1	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre X"];
		lPRDieCentreY_BH1	= (*m_psmfSRam)["MS896A"]["UpLook BH1 Learn PRDieCentre Y"];

		lCalDieCentreX_BH2	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn CalDieCentre X"];
		lCalDieCentreY_BH2	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn CalDieCentre Y"];
		lPRDieCentreX_BH2	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre X"];
		lPRDieCentreY_BH2	= (*m_psmfSRam)["MS896A"]["UpLook BH2 Learn PRDieCentre Y"];

		szLog.Format("Uplook --- Learn BH1 DieSize(%d,%d), BH2 DieSize(%d,%d)", lLearnUplookX_BH1,lLearnUplookY_BH1, lLearnUplookX_BH2,lLearnUplookY_BH2);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog); 
		szLog.Format("Uplook --- Learn BH1 CalDieCentre(%d,%d), BH2 CalDieCentre(%d,%d)", lCalDieCentreX_BH1,lCalDieCentreY_BH1, lCalDieCentreX_BH2,lCalDieCentreY_BH2);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog); 
		szLog.Format("Uplook --- Learn BH1 PRDieCentre(%d,%d), BH2 PRDieCentre(%d,%d)", lPRDieCentreX_BH1,lPRDieCentreY_BH1, lPRDieCentreX_BH2,lPRDieCentreY_BH2);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog); 
	}

	// If video test enabled and it is defective die, the message size should be smaller to prevent blocking the image
	if ( (usDieType == PR_ERR_DEFECTIVE_DIE) && ( m_bDebugVideoTest == TRUE ) )
	{
		csMsgA = "Type:\n  " + csMsgA + "\n";
		csMsgB.Format(" %s\n  %f\n %s", szMsgRot, fDieRotate, csMsgD);
		
		if (m_bGenSrchEnableDefectCheck[lDieNo] == TRUE)
		{
			csMsgC.Format(" S-Defect Score:\n  %.1f (%.1f)\n T-Defect Score:\n  %.1f (%.1f)\n", 
				m_dSpSingleDefectScore, m_dGenSrchSingleDefectArea[lDieNo], 
				m_dSpTotalDefectScore, m_dGenSrchTotalDefectArea[lDieNo]);
		}
		
		csMsgA = csMsgA + csMsgB + csMsgC;
		if (m_bGenSrchEnableDefectCheck[lDieNo] == TRUE)
			HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 200 + 100, 400 + 100, NULL, NULL, NULL, NULL);
		else
			HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 200 + 100, 350 + 100, NULL, NULL, NULL, NULL);
	}
	else
	{
		csMsgA = "Type: " + csMsgA + csRefDieNo + "\n";	// add reference die number 

		if (IsMS90())	//shiraishi03
		{
			csMsgB.Format("Size X: %.1f  Y: %.1f um\n %s %f\n %s ",
							m_dCurSrchDieSizeX, m_dCurSrchDieSizeY, szMsgRot, fDieRotate, csMsgD);		//v3.01T1
		}
		else
		{
			csMsgB.Format("Size X: %.1f  Y: %.1f mil\n %s %f\n %s ",
							m_dCurSrchDieSizeX, m_dCurSrchDieSizeY, szMsgRot, fDieRotate, csMsgD);		//v3.01T1
		}
		
		if (m_bGenSrchEnableDefectCheck[lDieNo] == TRUE)
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
		}
		
		csMsgA = csMsgA + csMsgB + csMsgC;
		if (m_bGenSrchEnableDefectCheck[lDieNo] == TRUE)
			HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400 + 100, 320 + 100, NULL, NULL, NULL, NULL);
		else
			HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400 + 100, 270 + 100, NULL, NULL, NULL, NULL);
	}
}


BOOL CWaferPr::CheckLookForward(VOID)
{
	if ( GetDieShape() == WPR_RECTANGLE_DIE || GetDieShape() == WPR_RHOMBUS_DIE )
	{
		if ((m_bSrchEnableLookForward == TRUE) && IsLFSizeOK() )
		{
			(*m_psmfSRam)["WaferPr"]["UseLookForward"] = TRUE;

			return TRUE;
		}
	}

	(*m_psmfSRam)["WaferPr"]["UseLookForward"] = FALSE;

	return FALSE;
}

BOOL CWaferPr::ResetAutoBondScreen(VOID)
{
	PR_COMMON_RPY				stAutoBondRpy;

	//Reset Autobond command
	PR_ResetAutoBondCmd(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stAutoBondRpy);
	if (stAutoBondRpy.uwCommunStatus != PR_COMM_NOERR || stAutoBondRpy.uwPRStatus != PR_ERR_NOERR)
	{
		//CString csMsg;
		//csMsg.Format("Reset autobond error! = %x, %x",stAutoBondRpy.uwCommunStatus,stAutoBondRpy.uwPRStatus);
		//DisplayMessage(csMsg);	
		return FALSE;
	}

	return TRUE;
}

//Call in Autobond sequence

BOOL CWaferPr::ConfirmRefDieCheck(VOID)
{
	if (IsBurnIn())
	{
		return FALSE;
	}

	//Get Wafermap check refdie action
	LONG lRefDieCheck = (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];

	if ( (lRefDieCheck == 1) && HasReferDieCheck() )
	{
		//Check ref die face value, for avago			//v4.48A21
		if (CMS896AStn::m_bUseRefDieFaceValueCheck == TRUE)
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["FaceMatch"] = FALSE;

			LONG lRefFaceValue = 0;

			//get face value of reference die from Wafer Table Stn
			lRefFaceValue = (*m_psmfSRam)["DieInfo"]["RefDie"]["FaceValue"];

			//compare with face value input from HMI (WaferPr->SearchDie->More->More)
			if (lRefFaceValue == m_lRefDieFaceValueNo)
			{
				(*m_psmfSRam)["DieInfo"]["RefDie"]["FaceMatch"] = TRUE;
				return TRUE;
			}
			else
			{
				(*m_psmfSRam)["DieInfo"]["RefDie"]["FaceMatch"] = FALSE;
				return FALSE;
			}
		}

		return TRUE;
	}

	return FALSE;
}


BOOL CWaferPr::ConfirmSpecialgradeRefDieCheck(VOID)
{
	if (IsBurnIn())
	{
		return FALSE;
	}

	LONG lSpRefDieCheck = 0;

	//Get ref-die check option for any special MAP grade
	lSpRefDieCheck = (*m_psmfSRam)["DieInfo"]["RefDie"]["SpCheck"];

	//Use the "Special Grade for Ref Die" option without enabling the "Enable Ref Die Check" option (for CMLT)
	if ( (lSpRefDieCheck == 1) && HasSpRefDieCheck() )
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CWaferPr::AutoBondScreen(BOOL bOn)
{
	if( IsBurnIn() && m_bNoWprBurnIn )
	{
		return TRUE;
	}

	if( !m_fHardware )
	{
		return TRUE;
	}

	if (!m_bUseAutoBondMode)
	{
		return TRUE;
	}

	DOUBLE dTime = GetTime();
	if( bOn==TRUE )
	{
		ResetAutoBondScreen();
	}

	// Digital zoom
	PR_COMMON_RPY				stAutoBondRpy;
	PR_AUTO_BOND_SCREEN_MODE	uiMode;
	PR_COORD					stBondCorner1;
	PR_COORD					stBondCorner2;
	PR_COORD					stBondDieSize;
	PR_COORD					stCrossHair;
	LONG						lBondPrCenterX = PR_DEF_CENTRE_X;
	LONG						lBondPrCenterY = PR_DEF_CENTRE_Y;

	PR_COORD stCollet1BondCorner1;
	PR_COORD stCollet1BondCorner2;
	PR_COORD stCollet2BondCorner1;
	PR_COORD stCollet2BondCorner2;
	
	DOUBLE dBprCalibX	= (DOUBLE)(*m_psmfSRam)["BondPr"]["Calibration"]["X"];
	DOUBLE dBprCalibY	= (DOUBLE)(*m_psmfSRam)["BondPr"]["Calibration"]["Y"];

	lBondPrCenterX		= (*m_psmfSRam)["BondPr"]["CursorCenter"]["X"];
	lBondPrCenterY		= (*m_psmfSRam)["BondPr"]["CursorCenter"]["Y"];

	UpdateAutoBondScreen(bOn);
	switch(m_lAutoScreenMode)
	{
	case 1:	
		uiMode = PR_AUTO_BOND_SCREEN_MODE_4;
		break;

	case 2:	
		uiMode = PR_AUTO_BOND_SCREEN_MODE_16;
		break;

	default:	
		uiMode = PR_AUTO_BOND_SCREEN_MODE_1;
		break;
	}

	if (bOn == FALSE)
	{
		uiMode = PR_AUTO_BOND_SCREEN_MODE_DISABLE;
	}

	PR_SetAutoBondModeCmd(uiMode, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stAutoBondRpy);

	if (stAutoBondRpy.uwCommunStatus != PR_COMM_NOERR || stAutoBondRpy.uwPRStatus != PR_ERR_NOERR)
	{
		SetErrorMessage("WPR AutoBond Screen Set auto bond mode cmd error");
		return FALSE;
	}

	m_bAutoBondMode = bOn;		//v4.12T1	//Cree US 

	if (bOn == FALSE)
	{
		PR_SELECT_VIDEO_SOURCE_CMD		stCmd;
		PR_SELECT_VIDEO_SOURCE_RPY		stRpy;
		m_bPostSealCamera		= FALSE;

		PR_InitSelectVideoSourceCmd(&stCmd);
		PR_CAMERA	ubCameraID	= GetRunCamID();
		PR_PURPOSE	ubPrPurPose = GetRunPurposeG();
		switch (m_bCurrentCamera)
		{
		case WPR_CAM_BOND:		//Bond PR		
			ubCameraID				= MS899_BOND_CAM_ID;
			ubPrPurPose				= MS899_BOND_GEN_PURPOSE;
			m_ulWaferCameraColor	= WPR_UNSELECTED_COLOR;
			m_ulBondCameraColor		= WPR_SELECTED_COLOR;

			//4.52D17Auto Bond Screen
			if(m_unCurrPostSealID == 6) 
			{
				// BH1 uplook
				ubPrPurPose = MS899_POSTSEAL_BH1_GEN_PURPOSE; // PR_PURPOSE_CAM_A3
				ubCameraID	= MS899_POSTSEAL_2D_BH1_CAM_ID;	// PR_CAMERA_4
				
				m_ulWaferCameraColor	= WPR_UNSELECTED_COLOR;
				m_ulBondCameraColor		= WPR_UNSELECTED_COLOR;
				m_ul2DCameraColor		= WPR_UNSELECTED_COLOR;		//pllm
				m_ulBH1UplookCameraColor = WPR_SELECTED_COLOR;
				m_ulBH2UplookCameraColor = WPR_UNSELECTED_COLOR;

				m_bPostSealCamera		= TRUE;
				CMSLogFileUtility::Instance()->MS_LogOperation("*Uplook --- Auto Bond Screen ID6");
			}
			
			if(m_unCurrPostSealID == 7)
			{
				// BH2 uplook
				ubPrPurPose = MS899_POSTSEAL_BH2_GEN_PURPOSE; // PR_PURPOSE_CAM_A4
				ubCameraID	= MS899_POSTSEAL_2D_BH2_CAM_ID; // PR_CAMERA_5

				m_ulWaferCameraColor	= WPR_UNSELECTED_COLOR;
				m_ulBondCameraColor		= WPR_UNSELECTED_COLOR;
				m_ul2DCameraColor		= WPR_UNSELECTED_COLOR;		//pllm
				m_ulBH1UplookCameraColor = WPR_UNSELECTED_COLOR;
				m_ulBH2UplookCameraColor = WPR_SELECTED_COLOR;

				m_bPostSealCamera		= TRUE;
				CMSLogFileUtility::Instance()->MS_LogOperation("*Uplook --- Auto Bond Screen ID7");
			}
			break;

		default:	//Wafer PR
			if (m_unCurrPostSealID > 0)		//v4.40T1
			{
				ubPrPurPose = MS899_POSTSEAL_PR_PURPOSE;
				ubCameraID	= MS899_POSTSEAL_CAM_ID;
				switch( m_unCurrPostSealID )
				{
				case 4:	// WL1
					ubPrPurPose = ES_CONTOUR_WL1_PR_PURPOSE;
					ubCameraID	= ES_CONTOUR_WL1_CAM_ID;
					break;

				case 5:	// WL2
					ubPrPurPose = ES_CONTOUR_WL2_PR_PURPOSE;
					ubCameraID	= ES_CONTOUR_WL2_CAM_ID;
					break;

				case 2:
					ubPrPurPose = MS899_POSTSEAL_BH1_PR_PURPOSE;
					ubCameraID	= MS899_POSTSEAL_2D_BH1_CAM_ID;
					break;

				case 3:
					ubPrPurPose = MS899_POSTSEAL_BH2_PR_PURPOSE;
					ubCameraID	= MS899_POSTSEAL_2D_BH2_CAM_ID;
					break;

				default:
					break;
				}
				if( m_unCurrPostSealID>1 )
				{
					m_ulWaferCameraColor	= WPR_UNSELECTED_COLOR;	
					m_ulBondCameraColor		= WPR_UNSELECTED_COLOR;
					m_bPostSealCamera		= TRUE;
				}
			}
			else
			{
				m_ulWaferCameraColor	= WPR_SELECTED_COLOR;
				m_ulBondCameraColor		= WPR_UNSELECTED_COLOR;
			}
			break;
		}
		stCmd.emCameraNo		= ubCameraID;
		stCmd.emPurpose			= ubPrPurPose;
CString szMsg;
szMsg.Format("Test --- (%d,%d,%d,%d)", m_bCurrentCamera, m_unCurrPostSealID, bOn, uiMode);

		PR_SelectVideoSourceCmd(&stCmd, MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, &stRpy);

		m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);

		if ( m_bCurrentCamera == WPR_CAM_WAFER )
		{
			//Draw Home cursor
			stCrossHair.x = (PR_WORD)GetPrCenterX();
			stCrossHair.y = (PR_WORD)GetPrCenterY();
			DrawHomeCursor(stCrossHair);
		}
		else
		{
			//Draw Home cursor
			stCrossHair.x = (PR_WORD)lBondPrCenterX;
			stCrossHair.y = (PR_WORD)lBondPrCenterY;
			DrawHomeCursor(stCrossHair);
		}
	}
	else
	{
		if ( m_bCurrentCamera == WPR_CAM_WAFER )
		{
			//Draw Home cursor
			stCrossHair.x = (PR_WORD)GetPrCenterX();
			stCrossHair.y = (PR_WORD)GetPrCenterY();
			DrawHomeCursor(stCrossHair);
		}
		else
		{
			//Draw Home cursor
			stCrossHair.x = (PR_WORD)lBondPrCenterX;
			stCrossHair.y = (PR_WORD)lBondPrCenterY;
			DrawHomeCursor(stCrossHair);
		}

		//Show die size info after screen is reset 
		if (m_bUsePostBond == TRUE)
		{
			stBondDieSize.x = (PR_WORD)((LONG)(*m_psmfSRam)["BondPr"]["Die Size X"]);
			stBondDieSize.y = (PR_WORD)((LONG)(*m_psmfSRam)["BondPr"]["Die Size Y"]);
			stBondCorner1.x = (PR_WORD)lBondPrCenterX - (stBondDieSize.x / 2);
			stBondCorner1.y = (PR_WORD)lBondPrCenterY - (stBondDieSize.y / 2);
			stBondCorner2.x = (PR_WORD)lBondPrCenterX + (stBondDieSize.x / 2);
			stBondCorner2.y = (PR_WORD)lBondPrCenterY + (stBondDieSize.y / 2);

			LONG lULX = (*m_psmfSRam)["BondPr"]["SearchArea"]["ULX"];
			LONG lULY = (*m_psmfSRam)["BondPr"]["SearchArea"]["ULY"];
			LONG lLRX = (*m_psmfSRam)["BondPr"]["SearchArea"]["LRX"];
			LONG lLRY = (*m_psmfSRam)["BondPr"]["SearchArea"]["LRY"];

			if (m_bEnableMS100EjtXY)	//v4.43T6
			{
				if ( (dBprCalibX != 0) && (dBprCalibY != 0) )
				{
					LONG lBHZ1BondPosOffsetXCount = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"];
					LONG lBHZ1BondPosOffsetYCount = (*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"];

					stCollet1BondCorner1.x = (PR_WORD)_round(lULX + (lBHZ1BondPosOffsetXCount / dBprCalibX));
					stCollet1BondCorner1.y = (PR_WORD)_round(lULY + (lBHZ1BondPosOffsetYCount / dBprCalibY));
					stCollet1BondCorner2.x = (PR_WORD)_round(lLRX + (lBHZ1BondPosOffsetXCount / dBprCalibX));
					stCollet1BondCorner2.y = (PR_WORD)_round(lLRY + (lBHZ1BondPosOffsetYCount / dBprCalibY));

					LONG lBHZ2BondPosOffsetXCount = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"];
					LONG lBHZ2BondPosOffsetYCount = (*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"];

					stCollet2BondCorner1.x = (PR_WORD)_round(lULX + (lBHZ2BondPosOffsetXCount / dBprCalibX));
					stCollet2BondCorner1.y = (PR_WORD)_round(lULY + (lBHZ2BondPosOffsetYCount / dBprCalibY));
					stCollet2BondCorner2.x = (PR_WORD)_round(lLRX + (lBHZ2BondPosOffsetXCount / dBprCalibX));
					stCollet2BondCorner2.y = (PR_WORD)_round(lLRY + (lBHZ2BondPosOffsetYCount / dBprCalibY));

					DrawRectangleBox(stCollet1BondCorner1, stCollet1BondCorner2, PR_COLOR_TRANSPARENT);
					DrawRectangleBox(stCollet2BondCorner1, stCollet2BondCorner2, PR_COLOR_TRANSPARENT);
				}
			}
		}

		//Clear drawn windows
		DrawRectangleDieSize(PR_COLOR_TRANSPARENT);
		DisplayLFSearchArea(PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stBondCorner1, stBondCorner2, PR_COLOR_TRANSPARENT);
		DrawSearchBox( PR_COLOR_TRANSPARENT);

		switch(m_bCurrentCamera)
		{
		case WPR_CAM_BOND:
			if (m_bUsePostBond == TRUE)
			{
				DrawRectangleBox(stBondCorner1, stBondCorner2, PR_COLOR_GREEN);
				if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)	//v4.43T6	//v4.47A1
				{
					DrawRectangleBox(stCollet1BondCorner1, stCollet1BondCorner2, PR_COLOR_YELLOW);
					DrawRectangleBox(stCollet2BondCorner1, stCollet2BondCorner2, PR_COLOR_RED);
				}
			}
			break;

		default:
			DrawRectangleDieSize(PR_COLOR_GREEN);
			DrawSearchBox(PR_COLOR_GREEN);
			DisplayLFSearchArea(PR_COLOR_YELLOW);
			break;
		}
	}

	return TRUE;
}


BOOL CWaferPr::AutoSearchDie(CONST BOOL bLogMap)
{
	PR_UWORD	usSearchResult;
	BOOL		bDieType = WPR_NORMAL_DIE;
	LONG		lRefDieNo = 1;//WPR_NORMAL_DIE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG lRow = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
	LONG lCol = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];

	PR_WIN stSrchArea;

	BOOL bRefDieCheck = ConfirmRefDieCheck();
	BOOL bSpecialGradeCheck =  ConfirmSpecialgradeRefDieCheck();

	SetDiePRResult(0, 0, 0, FALSE, WT_MAP_DS_PICK);

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	if (bRefDieCheck || bSpecialGradeCheck)		//v3.24T1
	{
		CMSLogFileUtility::Instance()->WT_GetIdxLog("WPR srch refer die");
		bDieType = WPR_REFERENCE_DIE;

		/***** Search Character Die *****/
		if ( IsCharDieInUse() && (m_lLrnTotalRefDie > WPR_GEN_CDIE_OFFSET) )
		{
			LONG		lRefDigit1,lRefDigit2;
			LONG		lRtn;
			PR_COORD	stDieOffset;
			PR_RCOORD	stDieScore;
			PR_COORD	stDieDigit;
			PR_REAL		fDieRotate;

			(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dx"] = 0;	//andrewngT6
 			(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dy"] = 0;	//andrewngT6

			Sleep(20);
			lRefDigit1 = (*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"];
			lRefDigit2 = (*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"];

			m_iMapDig1 = lRefDigit1;
			m_iMapDig2 = lRefDigit2;
			stDieDigit.x = (PR_WORD)-1;		//search by FULL Mode to avoid wrong checking
			stDieDigit.y = (PR_WORD)-1;
			lRtn = SearchNumberDie(&stDieOffset, &fDieRotate, &stDieScore, &stDieDigit);
			m_iWafDig1 = stDieDigit.x;
			m_iWafDig2 = stDieDigit.y;

			if (lRtn == FALSE || lRefDigit1 != stDieDigit.x || lRefDigit2 != stDieDigit.y)
			{
 				(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dx"] = 0;				//andrewngT6
 				(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dy"] = 0;				//andrewngT6
				m_bRefCheckRst = FALSE;
			}
			else
			{
				(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dx"] = stDieOffset.x;	//andrewngT6
 				(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dy"] = stDieOffset.y;	//andrewngT6
				m_bRefCheckRst = TRUE;
			}

			//Retry again
			if (!m_bRefCheckRst)
			{
				Sleep(500);
				lRefDigit1 = (*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit1"];
				lRefDigit2 = (*m_psmfSRam)["DieInfo"]["CRefDie"]["Digit2"];
				stDieDigit.x = (PR_WORD)-1;		//pass -1, -1 to search number die with FULL Mode
				stDieDigit.y = (PR_WORD)-1;
				lRtn = SearchNumberDie(&stDieOffset, &fDieRotate, &stDieScore, &stDieDigit);//0
				m_iWafDig1 = stDieDigit.x;
				m_iWafDig2 = stDieDigit.y;
				if (lRtn == FALSE || lRefDigit1 != stDieDigit.x || lRefDigit2 != stDieDigit.y)
				{
 					m_bRefCheckRst = FALSE;
				}
				else
				{
					(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dx"] = stDieOffset.x;	//andrewngT6
 					(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dy"] = stDieOffset.y;	//andrewngT6
					m_bRefCheckRst = TRUE;
				}
			}

			// Double check PR can recognize a correct number/
			if (m_bRefCheckRst == TRUE)
			{
				LONG lTempRtn;
				PR_COORD	stTempDieOffset;
				PR_RCOORD	stTempDieScore;
				PR_COORD	stTempDieDigit;
				PR_REAL		fTempDieRotate;

				if (lRefDigit1 != 9)
				{
					stTempDieDigit.x = (PR_WORD)lRefDigit1+1;
				}
				else
				{
					stTempDieDigit.x = (PR_WORD)lRefDigit1-1;
				}

				if (lRefDigit2 != 9)
				{
					stTempDieDigit.y = (PR_WORD)lRefDigit2+1;
				}
				else
				{
					stTempDieDigit.y = (PR_WORD)lRefDigit2-1;
				}

				lTempRtn = SearchNumberDie(&stTempDieOffset, &fTempDieRotate, &stTempDieScore, &stTempDieDigit);

				if (lTempRtn == TRUE && lRefDigit1 == stTempDieDigit.x && lRefDigit2 == stTempDieDigit.y)
				{
					(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dx"] = 0;
 					(*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dy"] = 0;

					m_iWafDig1 = -2;
					m_iWafDig2 = -2;
					m_bRefCheckRst = FALSE;
				}
			}

			lRefDieNo = m_lCurrSrchDieId;
			GetSearchDieArea(&stSrchArea, lRefDieNo + WPR_GEN_RDIE_OFFSET);

			//Use the whole FOV to search reference die for Block Pick		//Block2
			if ( IsBlkFunc2Enable() )
			{
				stSrchArea.coCorner1.x = GetPRWinULX();
				stSrchArea.coCorner1.y = GetPRWinULY();
				stSrchArea.coCorner2.x = GetPRWinLRX();
				stSrchArea.coCorner2.y = GetPRWinLRY();
			}

			if (IsNGBlock(lRow, lCol))	// Matthew 20181212
			{
				usSearchResult = SearchDieCmd(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9, ubSID, ubRID, stSrchArea.coCorner1, stSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE, PR_NO_DISPLAY);
			}
			else
			{
				usSearchResult = SearchDieCmd(bDieType, lRefDieNo, ubSID, ubRID, stSrchArea.coCorner1, stSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE, PR_NO_DISPLAY);
			}
		}
		/***** End Search Character Die *****/
		else
		{
			if (m_lSrchRefDieNo == 0)
			{
				m_lSrchRefDieNo = 1;
			}
			lRefDieNo = m_lSrchRefDieNo;

			if( m_lLrnTotalRefDie<2 )
				lRefDieNo = 1;

			if ( (m_lLrnTotalRefDie > 1) && (m_bAllRefDieCheck == TRUE) )
			{
				lRefDieNo = m_lCurrSrchRefDieId;
			}		

			// Must Use PR Record 2 For Special Grade Check
			if (bSpecialGradeCheck == TRUE)
				lRefDieNo = 2;

			if( lRefDieNo<=0 )
				lRefDieNo = 1;

			GetSearchDieArea(&stSrchArea, lRefDieNo + WPR_GEN_RDIE_OFFSET);

			//Use the whole FOV to search reference die for Block pick		//Block2
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if ( IsBlkFunc2Enable() || pApp->IsPLLMRebel())	// auto enlarge window for refer
			{
				stSrchArea.coCorner1.x = GetPRWinULX();
				stSrchArea.coCorner1.y = GetPRWinULY();
				stSrchArea.coCorner2.x = GetPRWinLRX();
				stSrchArea.coCorner2.y = GetPRWinLRY();
			}

			if (IsNGBlock(lRow, lCol)) // Matthew 20181212
			{
				usSearchResult = SearchDieCmd(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9, ubSID, ubRID, stSrchArea.coCorner1, stSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE, PR_NO_DISPLAY);
			}
			else
			{
				usSearchResult = SearchDieCmd(bDieType, lRefDieNo , ubSID, ubRID, stSrchArea.coCorner1, stSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_TRUE, PR_NO_DISPLAY);	
			}
		}
	}
	else
	{
		bDieType	= WPR_NORMAL_DIE;
		lRefDieNo	= m_lCurrSrchDieId;

		if( lRefDieNo<=0 )
			lRefDieNo = 1;
#ifdef NU_MOTION
		//choose which record to be used to search collet hole
		if (m_bPreBondAtPick)
		{
			BOOL bRecord1 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest"]["UseRecord1"]);
			
			if (bRecord1 == TRUE)
			{
				bDieType = WPR_REFERENCE_DIE;
			}
		}
#endif
		//v2.96T2
		//Log MPA data into IM Vision interface 
		UCHAR ucGrade = 0;
		UCHAR ucPickMode = 2;	//NOPICK mode

		LONG lAction = (LONG)(*m_psmfSRam)["WaferMap"]["CurrDie"]["Action"];
		lRow = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
		lCol = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];
		ucGrade = (UCHAR)(LONG)(*m_psmfSRam)["WaferMap"]["CurrDie"]["Grade"];

		CString szOddRowID;
		CString szEvenRowID;
		BOOL bExistRowID;
		szOddRowID	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER]["ODD_ROW_ID"];
		szEvenRowID = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER]["EVEN_ROW_ID"];
		bExistRowID	= (BOOL)(LONG)(*m_psmfSRam)["MS896A"][WT_MAP_HEADER]["EXIST_ROW_ID"];
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if(pApp->GetCustomerName() == "Macom" && bExistRowID) //4.52D7
		{
			CString szMsg;

			if(lRow%2 == 0)
			{
				lRefDieNo = atol(szEvenRowID);
				//v4.53	Klocwork
				lRefDieNo = min(lRefDieNo, WPR_MAX_DIE-3);
				if (lRefDieNo < 0)
					lRefDieNo = 0;

				szMsg.Format("WPR srch Even Row only with DieID %s, %ld", szEvenRowID, lRefDieNo);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			}
			else
			{
				lRefDieNo = atol(szOddRowID);
				szMsg.Format("WPR srch Odd Row only with DieID %s, %ld", szOddRowID, lRefDieNo);
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			}
		} // else use normal case

		if (lAction == WAF_CDieSelectionAlgorithm::PICK)
		{
			ucPickMode = 1;		//PICK mode
		}

		PR_BOOLEAN doInspection = PR_TRUE;
// prescan relative code	B
		if( IsPrescanEndToPickNg() )
		{
			doInspection = PR_FALSE;
		}
// prescan relative code	E
		if( pUtl->GetPickDefectDie() )
			doInspection = PR_FALSE;

		stSrchArea = GetSrchArea();

		//v2.83T6
		if (IsNGBlock(lRow, lCol)) // Matthew 20181212
		{
			usSearchResult = SearchDieCmd(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9, ubSID, ubRID, stSrchArea.coCorner1, stSrchArea.coCorner2, PR_TRUE, PR_TRUE, doInspection, PR_NO_DISPLAY);
		}
		else
		{
			usSearchResult = SearchDieCmd(bDieType, lRefDieNo, ubSID, ubRID, stSrchArea.coCorner1, stSrchArea.coCorner2, PR_TRUE, PR_TRUE, doInspection, PR_NO_DISPLAY);
		}
	}
	
/*
	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	PR_COORD stCrossHair;
	stCrossHair.x = (PR_WORD)GetPrCenterX();
	stCrossHair.y = (PR_WORD)GetPrCenterY();
	DrawHomeCursor(stCrossHair);
	DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_BLACK); 
*/

	if (usSearchResult != PR_ERR_NOERR)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CWaferPr::AutoGrabDone(VOID)
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_UWORD			usSearchResult;

	PR_UBYTE ubSID = GetRunSenID();
	//v2.83T6
	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);

	if (usSearchResult != PR_ERR_NOERR)
	{
		if (IsBurnIn())
		{
			return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}


BOOL CWaferPr::AutoDieResult(CONST BOOL bCheckScore)
{
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset, stDieSize; 
	PR_REAL				fDieScore = 0.0;
	BOOL				bGetRpy3 = FALSE;
	BOOL				bIsGood = FALSE;
	BOOL				bIsRefDie = FALSE;
	BOOL				bUpdateBadDieStatus = FALSE;

	int					siStepX, siStepY;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG lRow = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Row"];
	LONG lCol = (*m_psmfSRam)["WaferMap"]["CurrDie"]["Col"];


	//Reset Current die center
	m_stDieCenter.x	= (PR_WORD)GetPrCenterX();
	m_stDieCenter.y	= (PR_WORD)GetPrCenterY();
	m_fDieAngle = 0.0;		

	PR_UBYTE ubSID = GetRunSenID();
	//If Burn-In mode & die not learnt
	if ( ( IsNormalDieLearnt() == FALSE) && IsBurnIn() )
	{
		//Update SRAM variable
		return TRUE;
	}

	usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		return FALSE;
	}

    usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }

	PR_BOOLEAN doInspection = PR_TRUE;
// prescan relative code	B
	if( IsPrescanEndToPickNg() )
	{
		doInspection = PR_FALSE;
	}
// prescan relative code	E
	if( pUtl->GetPickDefectDie() )
		doInspection = PR_FALSE;

	if (DieIsAlignable(usDieType) == TRUE && doInspection)
	{
		usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			return FALSE;
		}
		bGetRpy3 = TRUE;
	}

	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);
	if (bCheckScore)	//andrew
	{
		m_dCurAlignPassScore = stSrchRpy2.stDieAlign[0].stStreet.rMatchScore;	// HMI display
		CalculateAutobondDefectScore(usDieType, WPR_NORMAL_DIE, &stSrchRpy3);
	}

	BOOL bRefDieCheck = ConfirmRefDieCheck();
	//Auto enlarge srch wnd to search die again in case current die is hitting srchWnd boundary
	if (m_bAutoEnlargeSrchWnd && !DieIsAlignable(usDieType))
	{
		if (!bRefDieCheck)
		{
			PR_SRCH_DIE_RPY2	stNewSrchRpy2;
			PR_SRCH_DIE_RPY3	stNewSrchRpy3;

			//Either no-die or die hitting boundary
			if (AutoEnlargeSrchWndSrchDie(&stNewSrchRpy2, &stNewSrchRpy3))
			{
				ExtractDieResult(stNewSrchRpy2, stNewSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);
			}
		}
	} //End Auto Enlarge Search Wnd

	if( usDieType==PR_ERR_NO_DIE )
		(*m_psmfSRam)["WaferPr"]["DieScore"] = 0;	//	(DOUBLE) fDieScore;
	else
		(*m_psmfSRam)["WaferPr"]["DieScore"] = (LONG) fDieScore;	//	(DOUBLE) fDieScore;
	//Update die center
	LogDieResult(lRow, lCol, 0, usDieType);		//v3.75
	m_stDieCenter = stDieOffset;
	m_fDieAngle = fDieRotate;

	//Set all defective to good die & all good to defective die
	if ( (m_bReverseDieStatus == TRUE) &&  (DieIsAlignable(usDieType) == TRUE) )
	{
		if ( DieIsGood(usDieType) == TRUE )
		{
			usDieType = PR_ERR_DEFECTIVE_DIE;
		}
		else
		{
			usDieType = PR_ERR_GOOD_DIE;
		}

		LogDieResult(lRow, lCol, 1, usDieType);		//v3.75
	}

	//If die is not good but can be alignable, still pick this die
	if ( (m_bPickAlignableDie == TRUE) && (DieIsGood(usDieType) == FALSE) && (DieIsAlignable(usDieType) == TRUE) )
	{
		usDieType = PR_ERR_GOOD_DIE;
		LogDieResult(lRow, lCol, 2, usDieType);		//v3.75
	}


	//Record current die center & update die type counter
	//v2.96T4
	//Need to make sure curr-die posn is not ref-die posn, even though ref-die check is not enabled
	if (bRefDieCheck == FALSE)		//Update counter if not ref-die posn
		UpdateDieTypeCounter(usDieType, (DOUBLE)(fDieRotate));


	//Check current die is normal or ref die
	if (bRefDieCheck == FALSE)
	{
		//This is normal die 	
		bIsGood = DieIsGood(usDieType);
		if (IsBurnIn())
		{
			bIsGood = TRUE;

			//v4.01	//MS100 100ms BURNIN cycle
			siStepX = 0;
			siStepY = 0;
			fDieRotate = 0.00;
		}

		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		CalculateNDieOffsetXY(siStepX, siStepY);		//v3.77

		if (bIsGood == TRUE)
		{
			//v4.21T6
			BOOL bLogPR = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LOGPR"];
			if (bLogPR)
			{
				(*m_psmfSRam)["WaferTable"]["LOGPR"] = FALSE;
				LONG lDir = (*m_psmfSRam)["WaferTable"]["Dir"];
				LONG lLFX = (*m_psmfSRam)["WaferTable"]["LF_X"];
				LONG lLFY = (*m_psmfSRam)["WaferTable"]["LF_Y"];

				CString szLog;
				szLog.Format(",%ld,%ld,%ld,%ld,%ld", lDir, lLFX, lLFY, siStepX, siStepY);
CMSLogFileUtility::Instance()->BL_BarcodeLog(szLog);
			}

			//m_dCurrentAngle += fDieRotate;
			if (!IsNGBlock(lRow, lCol) && (DieIsOutAngle((DOUBLE)(fDieRotate)) == TRUE))
				//	|| (IsWithinThetaLimit(fDieRotate) == FALSE) )
			{
				// treat the die as defective type (bad die) if angle is out min(Theta correction is disable) or max(Theta correction is enable)
				SetDiePRResult((LONG)(siStepX), (LONG)(siStepY), (DOUBLE)(fDieRotate), (BOOL)(DieNeedRotate(fDieRotate)), WT_MAP_DS_PR_ROTATE);
				return FALSE;
			}

			//v2.89T1
			if (DieNeedRotate(fDieRotate) && m_bConfirmSearch)
			{
				//v4.59A4
				//If AutoDieResult() is called in ConfirmSearch with rotation error,
				//	this SRAM status must be set in order for WaferTable to return rotation
				//	error;
				SetDiePRResult((LONG)(siStepX), (LONG)(siStepY), (DOUBLE)(fDieRotate), (BOOL)(DieNeedRotate(fDieRotate)), WT_MAP_DS_PR_ROTATE);

				return FALSE;	//-> CONFIRM-SEARCH T correction only if current die is going to be picked
			}

			SetDiePRResult((LONG)(siStepX), (LONG)(siStepY), (DOUBLE)(fDieRotate), (BOOL)(DieNeedRotate(fDieRotate)), WT_MAP_DS_PICK);
			if (m_bEnableWTIndexTest)
			{
				m_stWTIndexTestLFEnc.lX = siStepX;
				m_stWTIndexTestLFEnc.lY = siStepY;
			
				int	siOrigX, siOrigY, siOrigT;
				GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
				m_stWTIndexTestLFEnc.lX += siOrigX;
				m_stWTIndexTestLFEnc.lY += siOrigY;
			}
			return TRUE;
		}
		else
		{
			//v4.35T4	
			//BEtter to set status here even Multiple Search fcn is enabled, otherwise map die state may
			//not be updated correctly if Die Record #2-#3 are not learnt -> PLLM Lumiramic buyoff
			//if ( m_lCurrSrchDieId == 3 || !m_bCheckAllNmlDie )	
			//{

			LONG lStepX = (LONG)(siStepX);
			LONG lStepY = (LONG)(siStepY);
			LONG lDieResult = WT_MAP_DS_PR_EMPTY;

			if (DieIsDefective(usDieType) == TRUE)
			{
				lDieResult = WT_MAP_DS_PR_DEFECT;
			}
			else if (DieIsInk(usDieType) == TRUE)
			{
				lDieResult = WT_MAP_DS_PR_INK;
			}
			else if (DieIsChip(usDieType) == TRUE)
			{
				lDieResult = WT_MAP_DS_PR_CHIP;
			}
			else if (DieIsBadCut(usDieType) == TRUE)
			{
				lDieResult = WT_MAP_DS_PR_BADCUT;
			}
			else
			{
				lDieResult = WT_MAP_DS_PR_EMPTY;
				lStepX = 0;
				lStepY	= 0;
			}

			SetDiePRResult(lStepX, lStepY, 0.0, FALSE, lDieResult);
		}

		return FALSE;
		//}
	}
	else if (!DieIsGood(usDieType) && m_bAutoLearnRefDie && !m_bAutoLearnRefDieDone)	//v2.78T2
	{
		m_bAutoLearnRefDieDone = TRUE;
		SetDiePROffsetResult(0, 0, 0.0, FALSE);

		if (AutoLearnRefDie())
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["Result"]	= TRUE;
			return TRUE;
		}
		else
		{
			(*m_psmfSRam)["DieInfo"]["RefDie"]["Result"]	= FALSE;
			return FALSE;
		}
	}
	else
	{
		/***** Search character if necessary *****/
		if ( IsCharDieInUse() )
		{
			CString szAlgorithm;
			CString szPathFinder;
			m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);
			if (szAlgorithm != "Block Algorithm")
				m_bRefCheckRst = FALSE;
			
			if (m_bRefCheckRst == TRUE)		
			{
				//andrewngT6
				//Check number-die and ref-die position offset; they should match if same die position,
				//or wrong die position will be recorded in map memory!!
				LONG lNumDieDx = (*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dx"];
 				LONG lNumDieDy = (*m_psmfSRam)["DieInfo"]["CNumRefDie"]["dy"];
				DOUBLE dNumDieDx = lNumDieDx;
				DOUBLE dNumDieDy = lNumDieDy;
				CString szErr;

				if (m_lLrnTotalRefDie > WPR_GEN_CDIE_OFFSET)
				{
					if (fabs(dNumDieDx - m_stDieCenter.x) > (0.3 * GetDiePitchXX()))		//compared in pixel
					{
						szErr.Format("WPR: num-die and ref-die position not matched in X!  %.1f %d %d", dNumDieDx, m_stDieCenter.x, GetDiePitchXX());
						HmiMessage_Red_Green(szErr);
						SetErrorMessage(szErr);
						(*m_psmfSRam)["WaferPr"]["DieResult"] = WT_MAP_DS_PR_EMPTY;
						return FALSE;		
					}
					if (fabs(dNumDieDy - m_stDieCenter.y) > (0.3 * GetDiePitchYY()))		//compared in pixel
					{
						szErr.Format("WPR: num-die and ref-die position not matched in Y!  %.1f %d %d", dNumDieDy, m_stDieCenter.y, GetDiePitchYY());
						HmiMessage_Red_Green(szErr);
						SetErrorMessage(szErr);
						(*m_psmfSRam)["WaferPr"]["DieResult"] = WT_MAP_DS_PR_EMPTY;
						return FALSE;		
					}
				}

				CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

				bIsGood = DieIsGood(usDieType);
				SetDiePRResult((LONG)(siStepX), (LONG)(siStepY), 0.0, FALSE, WT_MAP_DS_PICK);
			}
		}
		/***** End Search Character Die *****/
		else
		{
			bIsGood = DieIsGood(usDieType);

			if (IsBurnIn())
			{
				bIsGood = TRUE;
			}

			LONG lValidRefer = (*m_psmfSRam)["DieInfo"]["RefDie"]["Valid"];
			if( lValidRefer==0 )
			{
				bIsGood = TRUE;
			}

			(*m_psmfSRam)["DieInfo"]["RefDie"]["Result"] = (LONG)bIsGood;

			if (bIsGood)
			{
				m_bRefCheckRst = TRUE;
			}
			else
			{
				if (m_bAllRefDieCheck == FALSE)
				{
					m_bRefCheckRst = FALSE;
					CMSLogFileUtility::Instance()->WT_GetIdxLog("WPR srch single refer die fail");
				}
			}

			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

			if ( (m_lCurrSrchDieId == m_lLrnTotalRefDie) || (m_bAllRefDieCheck == FALSE) )
			{
				LONG lStepX = (LONG)(siStepX);
				LONG lStepY = (LONG)(siStepY);
				LONG lDieResult = WT_MAP_DS_PR_EMPTY;

				if (DieIsDefective(usDieType) == TRUE)
				{
					lDieResult = WT_MAP_DS_PR_DEFECT;
				}
				else if (DieIsInk(usDieType) == TRUE)
				{
					lDieResult = WT_MAP_DS_PR_INK;
				}
				else if (DieIsChip(usDieType) == TRUE)
				{
					lDieResult = WT_MAP_DS_PR_CHIP;
				}
				else if (DieIsBadCut(usDieType) == TRUE)
				{
					lDieResult = WT_MAP_DS_PR_BADCUT;
				}
				else
				{
					lDieResult = WT_MAP_DS_PR_EMPTY;
					lStepX = 0;
					lStepY = 0;
				}
				SetDiePRResult(lStepX, lStepY, 0.0, FALSE, lDieResult);
			}

			return bIsGood;
		}
	}

	return TRUE;
}


BOOL CWaferPr::AutoLogDieResult()
{
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset, stDieSize; 
	PR_REAL				fDieScore;
	BOOL				bGetRpy3 = FALSE;
	BOOL				bIsGood = FALSE;
	BOOL				bIsRefDie = FALSE;
	BOOL				bUpdateBadDieStatus = FALSE;

	int					siStepX, siStepY;
	PR_UBYTE ubSID = GetRunSenID();

	usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		return FALSE;
	}

    usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }

	if (DieIsAlignable(usDieType) == TRUE)
	{
		usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			return FALSE;
		}
		//bGetRpy3 = TRUE;
	}

	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);

	if (DieIsAlignable(usDieType))
	{
		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	}
	else
	{
		siStepX = 0;
		siStepY = 0;
	}

#ifndef NU_MOTION
	//Log to WaferPr file
	CStdioFile oLogFile;
	if (oLogFile.Open(_T("c:\\Mapsorter\\UserData\\WaferPr.txt"), 
				CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText))
	{
		oLogFile.SeekToEnd();
		CString szLine;

		if (DieIsAlignable(usDieType))
			szLine.Format("%d,%d,%.2f\n", siStepX, siStepY, fDieRotate);
		else
			szLine = _T("0,0,0.00\n");

		oLogFile.WriteString(szLine);		
		oLogFile.Close();
	}
#else
	BOOL bRecord1 = (BOOL)(LONG)((*m_psmfSRam)["BondHead"]["ColletTest"]["UseRecord1"]);
	if ( bRecord1 == FALSE )
	{
		//Log to WaferPr file
		CStdioFile oLogFile;
		if (oLogFile.Open(_T("c:\\Mapsorter\\UserData\\WaferPr.txt"), 
					CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText))
		{
			oLogFile.SeekToEnd();
			CString szLine;

			if (DieIsAlignable(usDieType))
				szLine.Format("%d,%d,%.2f\n", siStepX, siStepY, fDieRotate);
			else
				szLine = _T("0,0,0.00\n");

			oLogFile.WriteString(szLine);		
			oLogFile.Close();
		}
	}
	else
	{
		//Log to WaferPr file
		CStdioFile oLogFile;
		if (oLogFile.Open(_T("c:\\Mapsorter\\UserData\\WaferPr2.txt"), 
					CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText))
		{
			oLogFile.SeekToEnd();
			CString szLine;

			if (DieIsAlignable(usDieType))
				szLine.Format("%d,%d,%.2f\n", siStepX, siStepY, fDieRotate);
			else
				szLine = _T("0,0,0.00\n");

			oLogFile.WriteString(szLine);		
			oLogFile.Close();
		}
	}
#endif

	return TRUE;
}


BOOL CWaferPr::AutoLookForwardDie(BOOL bEnable)
{
	//v2.83T6
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset, stDieSize; 
	PR_REAL				fDieScore;
	PR_BOOLEAN			doInspection;
	PR_BOOLEAN			bPrLatch;
	BOOL				bGetRpy3;
	LONG				lDirection;
	int					siStepX, siStepY;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	PR_WIN				stLFArea;
	PR_ULWORD			ulImageID;
	PR_ULWORD			ulStationID;

	LONG lRow	= (*m_psmfSRam)["WaferMap"]["NextDie"]["Row"];
	LONG lCol	= (*m_psmfSRam)["WaferMap"]["NextDie"]["Col"];

	//Init variable & get Lookforward direction
	siStepX		= 0; 
	siStepY		= 0;
	bGetRpy3	= FALSE;
	doInspection = PR_TRUE;		//PR_FALSE;		//v2.96T3
	bPrLatch	= PR_FALSE;		//v2.96T3
	lDirection	= (*m_psmfSRam)["WaferMap"]["NextDieDirection"];

	CString szMsg;
	szMsg.Format("AutoLookForwardDie lDirection = %d", lDirection);
	CMSLogFileUtility::Instance()->MS60_Log(szMsg);

	BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
	if (bLFNoComp)
	{
		bPrLatch = PR_TRUE;		//Need to grab a new image here (instead of using last PR image)
	}
	if (m_bPreBondAtPick)		//v4.48A15
		bPrLatch = PR_TRUE;		//Need to grab a new image here (instead of using last PR image)

	BOOL bMS60NewLFCycle = FALSE;

	//2018.1.2 (*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"] is only for MS60NewLFCycle
//	if (m_bWaferPrMS60LFSequence && m_bWaferPrUseMxNLFWnd && bLFNoComp)		//v4.47T2	//v4.47T3
	BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];	//2018.1.2
	if (m_bWaferPrMS60LFSequence && m_bWaferPrUseMxNLFWnd && bLFNoComp && bMS60LFCycle)
	{
		LONG lNextNextDir = (*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"];
		if (lNextNextDir != -1)
		{
			lDirection	= lNextNextDir;
			bMS60NewLFCycle = TRUE;		//v4.54A5

			szMsg.Format("AutoLookForwardDie bMS60NewLFCycle = TRUE, lDirection = %d", lDirection);
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		}
	}

	//Reset again becasue this value will be changed in AutoDieResult function
	//???? 2018.1.13
	if (bMS60NewLFCycle)
	{
		m_stDieCenter.x	= (PR_WORD)GetPrCenterX();
		m_stDieCenter.y	= (PR_WORD)GetPrCenterY();
	}

	(*m_psmfSRam)["WaferPr"]["LookForward"]["X"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["Y"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"]	= 0.0;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]	= FALSE;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"]	= FALSE;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= FALSE;

//set the map coordinate of lookahead value in OpGetWaferPos_CheckLFDie function
//	(*m_psmfSRam)["WaferMap"]["LookForward_X"] = 0;
//	(*m_psmfSRam)["WaferMap"]["LookForward_Y"] = 0;

	if (lDirection == -1)
	{
		//DisplayMessage("No need to do Lookforward");	
		//Allow WT & BH to proceed if next die not available for LF
		if (bLFNoComp)
		{
			//Curr-die status is good from last LF result
			SetPRStart(TRUE);			//Allow BH-T to PICK
			(*m_psmfSRam)["WaferPr"]["RPY1"] = GetTime();	//v3.61
			TakeTime(WPR2);		
			SetPRLatched(TRUE);			//Allow BT to MOVE
			SetWPRGrabImageReady(TRUE, "3");

			TakeTime(WPR3);		
			DisplaySequence("WPR3 5");
			SetBadDie(FALSE, "4");
			SetBadDieForT(FALSE);
			SetDieReady(TRUE);			//Allow BH-Z to PICK
		}
		return TRUE;
	}

	if ( bEnable == FALSE )
	{
		return TRUE;
	}

	if ((IsNormalDieLearnt() == FALSE) && IsBurnIn())
	{
		//2018.04.08 for burn-in mode
		(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]	= TRUE;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= TRUE;
		return TRUE;
	}

	if (bLFNoComp)		//v3.93
		TakeTime(WPR1);	

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	//Search die Start
	if (!m_bEnableMS100EjtXY)	//m_bMS100EjtXY)	//v4.43T11
	{
		stLFArea = GetSrchArea();
		if ((lDirection >= 0) && (lDirection < WPR_MAX_FORWARD))
		{
			stLFArea.coCorner1 = m_stLFSearchArea[lDirection].coCorner1;
			stLFArea.coCorner2 = m_stLFSearchArea[lDirection].coCorner2;
		}

		//v4.43T2	//Support 5x5 & 7x7 LF die for MS100PlusII (SanAn)
		if (m_bWaferPrUseMxNLFWnd)
		{
			DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
			if (FALSE/*dFov > WPR_LF_SIZE_13X13*/)	
			{
				GetLFSearchArea_13x13(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_11X11)	
			{
				GetLFSearchArea_11x11(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_9X9)
			{
				GetLFSearchArea_9x9(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_7X7)			//v4.43T9
			{
				GetLFSearchArea_7x7(stLFArea, lDirection);
			} 
			else if (dFov > WPR_LF_SIZE_5X5)	//v4.43T9
			{
				GetLFSearchArea_5x5(stLFArea, lDirection);
			}
		}
		if (IsNGBlock(lRow, lCol))
		{
			usSearchResult = SearchDieCmd(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9, ubSID, ubRID, 
							stLFArea.coCorner1, stLFArea.coCorner2, 
							bPrLatch, PR_TRUE, doInspection, PR_NO_DISPLAY);
		}
		else
		{
			usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID, 
							stLFArea.coCorner1, stLFArea.coCorner2, 
							bPrLatch, PR_TRUE, doInspection, PR_NO_DISPLAY);
		}
	}
	else	//v4.42T15
	{
		//PR_COORD NewAreaCorner1 = m_stLFSearchArea[lDirection].coCorner1;
		//PR_COORD NewAreaCorner2 = m_stLFSearchArea[lDirection].coCorner2;
		PR_WIN stLFArea = GetSrchArea();
		
		//v4.44A1	//SEmitek
		if ((lDirection >= 0) && (lDirection < WPR_MAX_FORWARD))
		{
			stLFArea.coCorner1 = m_stLFSearchArea[lDirection].coCorner1;
			stLFArea.coCorner2 = m_stLFSearchArea[lDirection].coCorner2;
		}

		//Support 5x5 & 7x7 LF die for MS100PlusII (SanAn)
		if (m_bWaferPrUseMxNLFWnd)
		{
			DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
			if (dFov > WPR_LF_SIZE_11X11)	
			{
				GetLFSearchArea_11x11(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_9X9)
			{
				GetLFSearchArea_9x9(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_7X7)			//v4.43T9
			{
				GetLFSearchArea_7x7(stLFArea, lDirection);
			} 
			else if (dFov > WPR_LF_SIZE_5X5)	//v4.43T9
			{
				GetLFSearchArea_5x5(stLFArea, lDirection);
			}
		}

		PR_COORD NewAreaCorner1 = stLFArea.coCorner1;
		PR_COORD NewAreaCorner2 = stLFArea.coCorner2;
		CString szTemp;
		UCHAR ucColletID = 0;		//v4.50A26

		if (bLFNoComp)
		{
			//BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
			//if ( !bBHZ2TowardsPick )	//BH1
			if (CMS896AStn::m_nWTAtColletPos == 1)			//BH1
			{
				ucColletID = 1;
				NewAreaCorner1.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
				NewAreaCorner1.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
				NewAreaCorner2.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
				NewAreaCorner2.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
				//szTemp.Format("C1 LA Wnd: %d,%d, %d,%d", NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y);
				//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
			}
			else if (CMS896AStn::m_nWTAtColletPos == 2)		//BH2
			{
				ucColletID = 2;
				NewAreaCorner1.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
				NewAreaCorner1.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
				NewAreaCorner2.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
				NewAreaCorner2.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
				//szTemp.Format("C2 LA Wnd: %d,%d, %d,%d", NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y);
				//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
			}

			CString szMsg;
			szMsg.Format("Search win with Ejt Offset(LeftTop(%d, %d), RightBottom(%d, %d), ucColletID = %d,Etj(%d,%d)(%d,%d)", 
				NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y, ucColletID, 
				m_lEjtCollet1OffsetX_Pixel,m_lEjtCollet1OffsetY_Pixel, m_lEjtCollet2OffsetX_Pixel,m_lEjtCollet2OffsetY_Pixel);
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);

			VerifyPRRegion_MxN(NewAreaCorner1, NewAreaCorner2);		//v4.46T17
		}
		
		if (IsNGBlock(lRow, lCol))
		{
			usSearchResult = SearchDieCmd(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9, ubSID, ubRID, 
								NewAreaCorner1, NewAreaCorner2, 
								bPrLatch, PR_TRUE, doInspection, 
								PR_NO_DISPLAY, 0, ucColletID);		//v4.50A26
		}
		else
		{
			usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID, 
								NewAreaCorner1, NewAreaCorner2, 
								bPrLatch, PR_TRUE, doInspection, 
								PR_NO_DISPLAY, 0, ucColletID);		//v4.50A26
		}
	}
	if (bPrLatch)
	{
		DisplaySequence("bPrLatch == TRUE in AutoLookForwardDie(), Grab a new image");
	}
	else
	{
		DisplaySequence("bPrLatch == FALSE in AutoLookForwardDie(), Use the last image");
	}
	TakeTime(WPR2);	

	if (usSearchResult != PR_ERR_NOERR && usSearchResult != 2)
	{
		//v4.45T2	//Semitek
		CString szErr;
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			szErr.Format("WPR Not Learnt");
		}
		else
		{
			szErr.Format("WPR Auto LookAheadDie cmd fails - ErrCode = %d", usSearchResult);
		}
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

/*
	//ChangeCamera(WPR_CAM_WAFER);
	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	//Draw Home cursor
	PR_COORD stCrossHair;
	stCrossHair.x = (PR_WORD)GetPrCenterX();
	stCrossHair.y = (PR_WORD)GetPrCenterY();
	DrawHomeCursor(stCrossHair);
	DrawRectangleBox(stLFArea.coCorner1, stLFArea.coCorner2, PR_COLOR_BLUE);
*/

	//Get reply 1
	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR&& usSearchResult != 2)
	{
		//v4.45T2	//Semitek
		CString szErr;
		szErr.Format("WPR Auto LookAheadDie RPY1 cmd fails - ErrCode = %d", usSearchResult);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	if (m_lLetBPRGrabFirstImage)
	{
		SetWPRGrabImageReady(TRUE, "4");
	}
	else
	{
		//BPR can Grab Image in next cycle
		m_lLetBPRGrabFirstImage = TRUE;
	}

	TakeTime(WPR3);
	DisplaySequence("WPR3 9");

	//v2.96T3
	// Allow WT & BH to proceed once LF image is grabbed
	if (bLFNoComp)		// If NO-COMPENSATE, current die at center now must be GOOD die	!!
	{
		//Curr-die status is good from last LF result
		SetPRStart(TRUE);			//Allow BH-T to PICK
		(*m_psmfSRam)["WaferPr"]["RPY1"] = GetTime();	//v3.61
		//TakeTime(WPR2);	
		SetPRLatched(TRUE);			//Allow BT to MOVE

		//TakeTime(WPR3);
		DisplaySequence("WPR3 6");
		SetBadDie(FALSE, "5");
		SetBadDieForT(FALSE);
		SetDieReady(TRUE);			//Allow BH-Z to PICK
	}


	//Get reply 2
	usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		return TRUE;
	}

	usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }


	//Init stSrchRpy3
	//memset(&stSrchRpy3,0,sizeof(stSrchRpy3));

	if( (DieIsAlignable(usDieType) ) && (doInspection == TRUE) )
	{
		usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			return TRUE;
		}
		bGetRpy3 = TRUE;
	}


	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);

	LogDieResult(lRow, lCol, 0, usDieType, TRUE);		//v3.75

	//v2.99T1
	//Set all defective to good die & all good to defective die
	if ( (m_bReverseDieStatus == TRUE) &&  (DieIsAlignable(usDieType) == TRUE) )
	{
		if ( DieIsGood(usDieType) == TRUE )
		{
			usDieType = PR_ERR_DEFECTIVE_DIE;
		}
		else
		{
			usDieType = PR_ERR_GOOD_DIE;
		}
	
		LogDieResult(lRow, lCol, 1, usDieType, TRUE);		//v3.75
	}

	if (IsBurnIn())
	{
		 usDieType = PR_ERR_GOOD_DIE;
	}

	BOOL bEmptyHoleStatus = AutoLookBackwardLastDie(bMS60NewLFCycle);	//v4.54A5
	if (!bEmptyHoleStatus)
	{
		CString szErr;
		szErr = "WPR: die is found at EMPTY die position (LookForward)";
CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		SetErrorMessage(szErr);
		HmiMessage_Red_Back(szErr, "Wafer PR");
		return FALSE;		//Stop AUTOBOND cycle
	}

	//v4.59A40
	if (CMS896AStn::m_bMS90WTPrVibrateTest)
	{
		PerformWTPrVibrationTest();	
	}

	if ( (DieIsAlignable(usDieType) == TRUE) )			//If die is alignable
	{
		PR_COORD stTempDieOffset = stDieOffset;
		szMsg.Format("AutoLookForwardDie (Pixel) PR Center = (%d,%d), Die Center = (%d,%d) Angle,%f", 
					 m_stDieCenter.x, m_stDieCenter.y, stDieOffset.x, stDieOffset.y,fDieRotate);
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);

		//Calculate motor distance with current center die pos
		stDieOffset.x = m_stDieCenter.x - stDieOffset.x;
		stDieOffset.y = m_stDieCenter.y - stDieOffset.y;

		ConvertPixelToMotorStep(stDieOffset, &siStepX, &siStepY);
		CalculateNDieOffsetXY(siStepX, siStepY);		//v3.77

		m_dCurrentAngle = (DOUBLE)(fDieRotate);

		//Update Sram stringmap
		(*m_psmfSRam)["WaferPr"]["LookForward"]["X"]		= (LONG)(siStepX);
		(*m_psmfSRam)["WaferPr"]["LookForward"]["Y"]		= (LONG)(siStepY);
		(*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"]	= (DOUBLE)(fDieRotate);
		(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]	= TRUE;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= DieIsGood(usDieType);		//v2.96T3
		
		if (m_bEnableWTIndexTest)
		{
			m_stWTIndexTestLFEnc.lX = siStepX;
			m_stWTIndexTestLFEnc.lY = siStepY;
			
			int	siOrigX, siOrigY, siOrigT;
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			m_stWTIndexTestLFEnc.lX += siOrigX;
			m_stWTIndexTestLFEnc.lY += siOrigY;
		}

		CString szLog;
		INT siTempX = 0, siTempY = 0;
		ConvertPixelToMotorStep(stTempDieOffset, &siTempX, &siTempY);
		szLog.Format("Look Forward Die Position (ENC),%d,%d,Die Offset (Pixel),%d,%d, Die Offset (ENC),%d,%d, Die Center(Pixel),%d,%d", 
			(LONG)siStepX, (LONG)siStepY, (LONG)stTempDieOffset.x, (LONG)stTempDieOffset.y, 
			(LONG)siTempX, (LONG)siTempY, (LONG)m_stDieCenter.x, (LONG)m_stDieCenter.y);

		CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog);

		if (DieIsGood(usDieType) == TRUE)
		{
			(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"] = (BOOL)(DieNeedRotate(fDieRotate));
			szMsg.Format("AutoLookForwardDie is Good Die");
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		}
		else
		{
		 	(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"] = FALSE;
			szMsg.Format("AutoLookForwardDie is Defective Die");
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		}

		if (!IsNGBlock(lRow, lCol) && DieIsOutAngle(fDieRotate))	//v2.96T4
		{
			(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= FALSE;
			(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"]	= FALSE;	//v2.97T4
		}

		return TRUE;
	}
	else
	{
		szMsg.Format("AutoLookForwardDie is not Align Die");
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		return TRUE;
	}

	return TRUE;
}


BOOL CWaferPr::AutoLookForwardDie_HwTrigger(BOOL bEnable)
{
	//v2.83T6
	PR_SRCH_DIE_CMD		stSrchCmd;
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	
	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset, stDieSize; 
	PR_REAL				fDieScore;
	PR_BOOLEAN			doInspection;
	PR_BOOLEAN			bPrLatch;
	BOOL				bGetRpy3;
	LONG				lDirection;
	int					siStepX, siStepY;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	PR_WIN				stLFArea;
	PR_ULWORD			ulImageID;
	PR_ULWORD			ulStationID;

	LONG lRow	= (*m_psmfSRam)["WaferMap"]["NextDie"]["Row"];
	LONG lCol	= (*m_psmfSRam)["WaferMap"]["NextDie"]["Col"];

	//Init variable & get Lookforward direction
	siStepX		= 0; 
	siStepY		= 0;
	bGetRpy3	= FALSE;
	doInspection = PR_FALSE;
	bPrLatch	= PR_FALSE;
	lDirection	= (*m_psmfSRam)["WaferMap"]["NextDieDirection"];

	CString szMsg;
	szMsg.Format("AutoLookForwardDie lDirection = %d", lDirection);
	CMSLogFileUtility::Instance()->MS60_Log(szMsg);

	BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
	if (bLFNoComp)
	{
		bPrLatch = PR_TRUE;		//Need to grab a new image here (instead of using last PR image)
	}
	if (m_bPreBondAtPick)		//v4.48A15
		bPrLatch = PR_TRUE;		//Need to grab a new image here (instead of using last PR image)

	BOOL bMS60NewLFCycle = FALSE;

	//2018.1.2 (*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"] is only for MS60NewLFCycle
//	if (m_bWaferPrMS60LFSequence && m_bWaferPrUseMxNLFWnd && bLFNoComp)		//v4.47T2	//v4.47T3
	BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];	//2018.1.2
	if (m_bWaferPrMS60LFSequence && m_bWaferPrUseMxNLFWnd && bLFNoComp && bMS60LFCycle)
	{
		LONG lNextNextDir = (*m_psmfSRam)["WaferMap"]["NextNext"]["ComingDieDirection"];
		if (lNextNextDir != -1)
		{
			lDirection	= lNextNextDir;
			bMS60NewLFCycle = TRUE;		//v4.54A5

			szMsg.Format("AutoLookForwardDie bMS60NewLFCycle = TRUE, lDirection = %d", lDirection);
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		}
	}

	//Reset again becasue this value will be changed in AutoDieResult function
	//???? 2018.1.13
	if (bMS60NewLFCycle)
	{
		m_stDieCenter.x	= (PR_WORD)GetPrCenterX();
		m_stDieCenter.y	= (PR_WORD)GetPrCenterY();
	}

	(*m_psmfSRam)["WaferPr"]["LookForward"]["X"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["Y"]		= 0;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"]	= 0.0;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]	= FALSE;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"]	= FALSE;
	(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= FALSE;

//set the map coordinate of lookahead value in OpGetWaferPos_CheckLFDie function
//	(*m_psmfSRam)["WaferMap"]["LookForward_X"] = 0;
//	(*m_psmfSRam)["WaferMap"]["LookForward_Y"] = 0;

	if (lDirection == -1)
	{
		//DisplayMessage("No need to do Lookforward");	
		//Allow WT & BH to proceed if next die not available for LF
		if (bLFNoComp)
		{
			//Curr-die status is good from last LF result
			SetPRStart(TRUE);			//Allow BH-T to PICK
			(*m_psmfSRam)["WaferPr"]["RPY1"] = GetTime();	//v3.61
			TakeTime(WPR2);		
			SetPRLatched(TRUE);			//Allow BT to MOVE
			SetWPRGrabImageReady(TRUE, "5");
			
			TakeTime(WPR3);		
			DisplaySequence("WPR3 5");
			SetBadDie(FALSE, "4");
			SetBadDieForT(FALSE);
			SetDieReady(TRUE);			//Allow BH-Z to PICK
		}
		return TRUE;
	}

	if ( bEnable == FALSE )
	{
		return TRUE;
	}

	if ((IsNormalDieLearnt() == FALSE) && IsBurnIn())
	{
		//2018.04.08 for burn-in mode
		(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]	= TRUE;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= TRUE;
		return TRUE;
	}

	//if (bLFNoComp)		//v3.93
	TakeTime(WPR1);	

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	/*usSearchResult = GrabShareImage(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID);
	if (usSearchResult != PR_ERR_NOERR)
	{
		szMsg.Format("Fail to GrabShareImage = %x", usSearchResult);
		DisplaySequence(szMsg);
		return FALSE;
	}
	DisplaySequence("WPR - GrabShareImage Done");*/


	DisplaySequence("WPR - GrabHwTrigger Start");
	GrabHwTrigger();
	DisplaySequence("WPR - GrabHwTrigger Done");

	TakeTime(WPR2);		//andrewng //2020-04-27

	if (bLFNoComp)		// If NO-COMPENSATE, current die at center now must be GOOD die	!!
	{
		//Curr-die status is good from last LF result
		SetWPRGrabImageReady(TRUE, "6");	//Allow BPR Grab Image
		SetPRStart(TRUE);			//Allow BH-T to PICK
		SetPRLatched(TRUE);			//Allow BT to MOVE
		SetDieReady(TRUE);			//Allow BH-Z to PICK
		DisplaySequence("WPR - SetDieReady(TURE)");
	}
	
	usSearchResult = GrabShareImageRpy2(ubSID, &ulImageID, &ulStationID);
	if (usSearchResult != PR_ERR_NOERR)
	{
		szMsg.Format("WPR - Fail to GrabHWTrigger = %x", usSearchResult);
		DisplaySequence(szMsg);
		return FALSE;
	}
	szMsg.Format("WPR - GrabShareImageRpy2() Done, ulImageID = %d ulStationID = %d", ulImageID, ulStationID);
	DisplaySequence(szMsg);

	IMG_UWORD uwCommStatus;
	stSrchCmd = m_stMultiSrchCmd;

	stSrchCmd.ulLatchImageID	= ulImageID;
	stSrchCmd.uwLatchStationID	= (PR_UWORD) ulStationID;
	stSrchCmd.emLatch			= PR_FALSE;

	//Vincent Mok Suggestion 2020/04/23
	stSrchCmd.stDieAlign.stStreet.emSelectMode		= PR_SRCH_DIE_SELECT_MODE_HIGHEST_SCORE;

	stSrchCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
	stSrchCmd.uwHostMallocMaxNoOfDieInRpy2			= 100;
	if (stSrchCmd.emDefectInsp == PR_TRUE)
	{
		stSrchCmd.uwHostMallocMaxNoOfDieInRpy3		= 100;
		doInspection = PR_TRUE;
	}
	if (stSrchCmd.emPostBondInsp == PR_TRUE)
	{
		stSrchCmd.uwHostMallocMaxNoOfDieInRpy4		= 100;
	}
	
	//Search die Start
	if (!m_bEnableMS100EjtXY)	//m_bMS100EjtXY)	//v4.43T11
	{
		stLFArea = GetSrchArea();
		if ((lDirection >= 0) && (lDirection < WPR_MAX_FORWARD))
		{
			stLFArea.coCorner1 = m_stLFSearchArea[lDirection].coCorner1;
			stLFArea.coCorner2 = m_stLFSearchArea[lDirection].coCorner2;
		}

		//v4.43T2	//Support 5x5 & 7x7 LF die for MS100PlusII (SanAn)
		if (m_bWaferPrUseMxNLFWnd)
		{
			DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
			if (FALSE/*dFov > WPR_LF_SIZE_13X13*/)	
			{
				GetLFSearchArea_13x13(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_11X11)	
			{
				GetLFSearchArea_11x11(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_9X9)
			{
				GetLFSearchArea_9x9(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_7X7)			//v4.43T9
			{
				GetLFSearchArea_7x7(stLFArea, lDirection);
			} 
			else if (dFov > WPR_LF_SIZE_5X5)	//v4.43T9
			{
				GetLFSearchArea_5x5(stLFArea, lDirection);
			}
		}
		stSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1	= stLFArea.coCorner1;
		stSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2	= stLFArea.coCorner2;
		stSrchCmd.stDieAlign.stStreet.coProbableDieCentre.x		= (stLFArea.coCorner1.x + stLFArea.coCorner2.x) / 2;
		stSrchCmd.stDieAlign.stStreet.coProbableDieCentre.y		= (stLFArea.coCorner1.y + stLFArea.coCorner2.y) / 2;

		if (IsNGBlock(lRow, lCol))
		{

			usSearchResult = SearchDieCmd(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9, ubSID, ubRID, 
							stLFArea.coCorner1, stLFArea.coCorner2, 
							bPrLatch, PR_TRUE, doInspection, PR_NO_DISPLAY);
		}
		else
		{
			PR_SrchDieCmd(&stSrchCmd, ubSID, ubRID, &uwCommStatus);
			/*usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID, 
							stLFArea.coCorner1, stLFArea.coCorner2, 
							bPrLatch, PR_TRUE, doInspection, PR_NO_DISPLAY);*/
		}
	}
	else	//v4.42T15
	{
		//PR_COORD NewAreaCorner1 = m_stLFSearchArea[lDirection].coCorner1;
		//PR_COORD NewAreaCorner2 = m_stLFSearchArea[lDirection].coCorner2;
		PR_WIN stLFArea = GetSrchArea();
		
		//v4.44A1	//SEmitek
		if ((lDirection >= 0) && (lDirection < WPR_MAX_FORWARD))
		{
			stLFArea.coCorner1 = m_stLFSearchArea[lDirection].coCorner1;
			stLFArea.coCorner2 = m_stLFSearchArea[lDirection].coCorner2;
		}

		//Support 5x5 & 7x7 LF die for MS100PlusII (SanAn)
		if (m_bWaferPrUseMxNLFWnd)
		{
			DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
			if (dFov > WPR_LF_SIZE_11X11)	
			{
				GetLFSearchArea_11x11(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_9X9)
			{
				GetLFSearchArea_9x9(stLFArea, lDirection);
			}
			else if (dFov > WPR_LF_SIZE_7X7)			//v4.43T9
			{
				GetLFSearchArea_7x7(stLFArea, lDirection);
			} 
			else if (dFov > WPR_LF_SIZE_5X5)	//v4.43T9
			{
				GetLFSearchArea_5x5(stLFArea, lDirection);
			}
		}

		PR_COORD NewAreaCorner1 = stLFArea.coCorner1;
		PR_COORD NewAreaCorner2 = stLFArea.coCorner2;
		CString szTemp;
		UCHAR ucColletID = 0;		//v4.50A26

		if (bLFNoComp)
		{
			//BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
			//if ( !bBHZ2TowardsPick )	//BH1
			if (CMS896AStn::m_nWTAtColletPos == 1)			//BH1
			{
				ucColletID = 1;
				NewAreaCorner1.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
				NewAreaCorner1.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
				NewAreaCorner2.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
				NewAreaCorner2.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
				//szTemp.Format("C1 LA Wnd: %d,%d, %d,%d", NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y);
				//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
			}
			else if (CMS896AStn::m_nWTAtColletPos == 2)		//BH2
			{
				ucColletID = 2;
				NewAreaCorner1.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
				NewAreaCorner1.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
				NewAreaCorner2.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
				NewAreaCorner2.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
				//szTemp.Format("C2 LA Wnd: %d,%d, %d,%d", NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y);
				//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
			}

			CString szMsg;
			szMsg.Format("Search win with Ejt Offset(LeftTop(%d, %d), RightBottom(%d, %d), ucColletID = %d,Etj(%d,%d)(%d,%d)", 
				NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y, ucColletID, 
				m_lEjtCollet1OffsetX_Pixel,m_lEjtCollet1OffsetY_Pixel, m_lEjtCollet2OffsetX_Pixel,m_lEjtCollet2OffsetY_Pixel);
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);

			VerifyPRRegion_MxN(NewAreaCorner1, NewAreaCorner2);		//v4.46T17
		}
		
		if (IsNGBlock(lRow, lCol))
		{
			usSearchResult = SearchDieCmd(WPR_REFERENCE_DIE, WPR_REFERENCE_PR_DIE_INDEX9, ubSID, ubRID, 
								NewAreaCorner1, NewAreaCorner2, 
								bPrLatch, PR_TRUE, doInspection, 
								PR_NO_DISPLAY, 0, ucColletID);		//v4.50A26
		}
		else
		{
			usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID, 
								NewAreaCorner1, NewAreaCorner2, 
								bPrLatch, PR_TRUE, doInspection, 
								PR_NO_DISPLAY, 0, ucColletID);		//v4.50A26
		}
	}
	if (bPrLatch)
	{
		DisplaySequence("bPrLatch == TRUE in AutoLookForwardDie(), Grab a new image");
	}
	else
	{
		DisplaySequence("bPrLatch == FALSE in AutoLookForwardDie(), Use the last image");
	}
	//TakeTime(WPR2);	

	if (usSearchResult != PR_ERR_NOERR && usSearchResult != 2)
	{
		//v4.45T2	//Semitek
		CString szErr;
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			szErr.Format("WPR Not Learnt");
		}
		else
		{
			szErr.Format("WPR Auto LookAheadDie cmd fails - ErrCode = %d", usSearchResult);
		}
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

/*
	//ChangeCamera(WPR_CAM_WAFER);
	m_pPrGeneral->PRClearScreenNoCursor(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID);
	//Draw Home cursor
	PR_COORD stCrossHair;
	stCrossHair.x = (PR_WORD)GetPrCenterX();
	stCrossHair.y = (PR_WORD)GetPrCenterY();
	DrawHomeCursor(stCrossHair);
	DrawRectangleBox(stLFArea.coCorner1, stLFArea.coCorner2, PR_COLOR_BLUE);
*/

	//Get reply 1
	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR && usSearchResult != 2)
	{
		//v4.45T2	//Semitek
		CString szErr;
		szErr.Format("WPR Auto LookAheadDie RPY1 cmd fails - ErrCode = %d", usSearchResult);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	TakeTime(WPR3);
	DisplaySequence("WPR3 9");

	//v2.96T3
	// Allow WT & BH to proceed once LF image is grabbed
	if (bLFNoComp)		// If NO-COMPENSATE, current die at center now must be GOOD die	!!
	{
		//Curr-die status is good from last LF result
		//SetPRStart(TRUE);			//Allow BH-T to PICK
		(*m_psmfSRam)["WaferPr"]["RPY1"] = GetTime();	//v3.61
		//TakeTime(WPR2);	
		//SetPRLatched(TRUE);			//Allow BT to MOVE

		//TakeTime(WPR3);
		DisplaySequence("WPR3 6");
		SetBadDie(FALSE, "5");
		SetBadDieForT(FALSE);
		//SetDieReady(TRUE);			//Allow BH-Z to PICK
	}

	//Get reply 2
	stSrchRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*)malloc(stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 * sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
	if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign == NULL)
	{
		szMsg = "ManualSearchDie_HWTri: Init stSrchRpy2 null pointer";
		HmiMessage_Red_Back(szMsg);
		return FALSE; 
	}
	PR_SrchDieRpy2(ubSID, &stSrchRpy2);
	if (stSrchRpy2.stStatus.uwCommunStatus == PR_COMM_ERR)
	{
		szMsg = "ManualSearchDie_HWTri: PR_SrchDieRpy2 Comm ERR";
		HmiMessage_Red_Back(szMsg);
		return FALSE; 
	}

	//DisplaySequence("WPR - AutoLookForwardDie_HwTrigger PR_SrchDieRpy2 Done");

	/*usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		return TRUE;
	}*/

	usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }


	//Init stSrchRpy3
	//memset(&stSrchRpy3,0,sizeof(stSrchRpy3));
	if (DieIsAlignable(usDieType) && doInspection)
	{
		stSrchRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*) malloc(stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 * sizeof(PR_DIE_INSP_MINIMAL_RPY));
		DisplaySequence("WPR - AutoLookForwardDie_HwTrigger stSrchRpy3 1");

		if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin == NULL)
		{
			//Free the reply 2
			if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
			{
				free(stSrchRpy2.pstDieAlign);
			}
			szMsg = "ManualSearchDie_HWTri: Init stSrchRpy3 null pointer";
			HmiMessage_Red_Back(szMsg);
			return FALSE;
		}

		PR_SrchDieRpy3(ubSID, &stSrchRpy3);
		DisplaySequence("WPR - AutoLookForwardDie_HwTrigger stSrchRpy3 2");
		if (stSrchRpy3.stStatus.uwCommunStatus == PR_COMM_ERR)
		{
			szMsg = "ManualSearchDie_HWTri: PR_SrchDieRpy3 Comm ERR";
			HmiMessage_Red_Back(szMsg);
			return FALSE;
		}
		DisplaySequence("WPR - AutoLookForwardDie_HwTrigger PR_SrchDieRpy3 Done");

		////Free the reply 2
		//if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
		//{
		//	free(stSrchRpy2.pstDieAlign);
		//}
		////Free the reply 3
		//if ( stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin != NULL )
		//{
		//	free(stSrchRpy3.pstDieInspExtMin);
		//}

		/*usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			return TRUE;
		}*/
		bGetRpy3 = TRUE;
	}
	
	ExtractGrabShareImgDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);
	//ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);
	DisplaySequence("WPR - AutoLookForwardDie_HwTrigger ExtractGrabShareImgDieResult Done");

	LogDieResult(lRow, lCol, 0, usDieType, TRUE);		//v3.75

	//v2.99T1
	//Set all defective to good die & all good to defective die
	if ( (m_bReverseDieStatus == TRUE) &&  (DieIsAlignable(usDieType) == TRUE) )
	{
		if ( DieIsGood(usDieType) == TRUE )
		{
			usDieType = PR_ERR_DEFECTIVE_DIE;
		}
		else
		{
			usDieType = PR_ERR_GOOD_DIE;
		}
	
		LogDieResult(lRow, lCol, 1, usDieType, TRUE);		//v3.75
	}

	if (IsBurnIn())
	{
		 usDieType = PR_ERR_GOOD_DIE;
	}

	BOOL bEmptyHoleStatus = AutoLookBackwardLastDie(bMS60NewLFCycle);	//v4.54A5
	if (!bEmptyHoleStatus)
	{
		CString szErr;
		szErr = "WPR: die is found at EMPTY die position (LookForward)";
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);
		SetErrorMessage(szErr);
		HmiMessage_Red_Back(szErr, "Wafer PR");
		return FALSE;		//Stop AUTOBOND cycle
	}

	//v4.59A40
	if (CMS896AStn::m_bMS90WTPrVibrateTest)
	{
		PerformWTPrVibrationTest();	
	}

	//Free the reply 2
	if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
	{
		free(stSrchRpy2.pstDieAlign);
		DisplaySequence("WPR - AutoLookForwardDie_HwTrigger free(stSrchRpy2.pstDieAlign) Done");
	}
	
	if (DieIsAlignable(usDieType) && doInspection)
	{
		szMsg.Format("WPR - AutoLookForwardDie_HwTrigger stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 = %d, stSrchRpy3.pstDieInspExtMin = %d",
			stSrchCmd.uwHostMallocMaxNoOfDieInRpy3, stSrchRpy3.pstDieInspExtMin);
		DisplaySequence(szMsg);
		//Free the reply 3
		if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin != NULL)
		{
			free(stSrchRpy3.pstDieInspExtMin);
			DisplaySequence("WPR - AutoLookForwardDie_HwTrigger free(stSrchRpy3.pstDieInspExtMin) Done");
		}
	}

	usSearchResult = m_pPrGeneral->RemoveShareImage(ubSID, ubRID, ulImageID, ulStationID);
	if (usSearchResult != PR_COMM_NOERR)
	{
		szMsg.Format("Fail to RemoveShareImage = %x", usSearchResult);
		DisplaySequence(szMsg);
		return FALSE;
	}
	DisplaySequence("WPR - RemoveShareImage Done");
	

	if ( (DieIsAlignable(usDieType) == TRUE) )			//If die is alignable
	{
		PR_COORD stTempDieOffset = stDieOffset;
		szMsg.Format("AutoLookForwardDie (Pixel) PR Center = (%d,%d), Die Center = (%d,%d) Angle,%f", 
					 m_stDieCenter.x, m_stDieCenter.y, stDieOffset.x, stDieOffset.y,fDieRotate);
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);

		//Calculate motor distance with current center die pos
		stDieOffset.x = m_stDieCenter.x - stDieOffset.x;
		stDieOffset.y = m_stDieCenter.y - stDieOffset.y;

		ConvertPixelToMotorStep(stDieOffset, &siStepX, &siStepY);
		CalculateNDieOffsetXY(siStepX, siStepY);		//v3.77

		m_dCurrentAngle = (DOUBLE)(fDieRotate);

		//Update Sram stringmap
		(*m_psmfSRam)["WaferPr"]["LookForward"]["X"]		= (LONG)(siStepX);
		(*m_psmfSRam)["WaferPr"]["LookForward"]["Y"]		= (LONG)(siStepY);
		(*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"]	= (DOUBLE)(fDieRotate);
		(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]	= TRUE;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= DieIsGood(usDieType);		//v2.96T3
		
		if (m_bEnableWTIndexTest)
		{
			m_stWTIndexTestLFEnc.lX = siStepX;
			m_stWTIndexTestLFEnc.lY = siStepY;
			
			int	siOrigX, siOrigY, siOrigT;
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			m_stWTIndexTestLFEnc.lX += siOrigX;
			m_stWTIndexTestLFEnc.lY += siOrigY;
		}

		CString szLog;
		INT siTempX = 0, siTempY = 0;
		ConvertPixelToMotorStep(stTempDieOffset, &siTempX, &siTempY);
		szLog.Format("Look Forward Die Position (ENC),%d,%d,Die Offset (Pixel),%d,%d, Die Offset (ENC),%d,%d, Die Center(Pixel),%d,%d", 
			(LONG)siStepX, (LONG)siStepY, (LONG)stTempDieOffset.x, (LONG)stTempDieOffset.y, 
			(LONG)siTempX, (LONG)siTempY, (LONG)m_stDieCenter.x, (LONG)m_stDieCenter.y);

		CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog);

		if (DieIsGood(usDieType) == TRUE)
		{
			(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"] = (BOOL)(DieNeedRotate(fDieRotate));
			szMsg.Format("AutoLookForwardDie is Good Die");
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		}
		else
		{
		 	(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"] = FALSE;
			szMsg.Format("AutoLookForwardDie is Defective Die");
			CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		}

		if (!IsNGBlock(lRow, lCol) && DieIsOutAngle(fDieRotate))	//v2.96T4
		{
			(*m_psmfSRam)["WaferPr"]["LookForward"]["GoodDie"]	= FALSE;
			(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"]	= FALSE;	//v2.97T4
		}

		return TRUE;
	}
	else
	{
		szMsg.Format("AutoLookForwardDie is not Align Die");
		CMSLogFileUtility::Instance()->MS60_Log(szMsg);
		return TRUE;
	}

	return TRUE;
}


BOOL CWaferPr::AutoLookBackwardLastDie(BOOL bMS60NewLFCycle)	//v4.54A5
{
	if (!CMS896AStn::m_bMS60NGPick)
	{
		return TRUE;
	}

	if (IsBurnIn())		//v4.59A4
	{
		return TRUE;
	}

	BOOL bReturn = TRUE;

	LONG lLastDieDirection	= (LONG)(*m_psmfSRam)["WaferMap"]["LastDieDirection"];
	BOOL bLFNoComp			= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
	//(*m_psmfSRam)["WaferMap"]["LastDie"]["LookBackward_X"];
	//(*m_psmfSRam)["WaferMap"]["LastDie"]["LookBackward_Y"];
	LONG lUserRow	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"];
	LONG lUserCol	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"];


	CString szLog;
	szLog.Format("WPR AutoLookBackwardLastDie - LastPos = %ld, bLFNoComp = %d, NewLFCycle=%d", 
		lLastDieDirection, bLFNoComp, bMS60NewLFCycle);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (lLastDieDirection == -1)
	{
		return TRUE;
	}
	if (!bLFNoComp)
	{
		return TRUE;
	}

	(*m_psmfSRam)["WaferPr"]["LookBackward"]["IsEmptyDie"] = TRUE;


	PR_WIN stLFArea = GetSrchArea();
	
	if ((lLastDieDirection >= 0) && (lLastDieDirection < WPR_MAX_FORWARD))
	{
		stLFArea.coCorner1 = m_stLFSearchArea[lLastDieDirection].coCorner1;
		stLFArea.coCorner2 = m_stLFSearchArea[lLastDieDirection].coCorner2;
	}
	
	//Support 5x5 & 7x7 LF die for MS100PlusII (SanAn)
	if (m_bWaferPrUseMxNLFWnd)
	{
		DOUBLE dFov = min(m_dLFSizeX, m_dLFSizeY);
		if (dFov > WPR_LF_SIZE_7X7)
		{
			GetLFSearchArea_7x7(stLFArea, lLastDieDirection);
		} 
		else if (dFov > WPR_LF_SIZE_5X5)
		{
			GetLFSearchArea_5x5(stLFArea, lLastDieDirection);
		}
	}

	if (bMS60NewLFCycle)
	{
		//Under new LF cycle, the EMPTY hole is always at cursor center posn !
		stLFArea.coCorner1 = GetSrchArea().coCorner1;
		stLFArea.coCorner2 = GetSrchArea().coCorner2;
	}

	PR_COORD	stSrchEpoxyCenter;
	stSrchEpoxyCenter.x = (PR_WORD)GetPrCenterX();
	stSrchEpoxyCenter.y = (PR_WORD)GetPrCenterY();

	PR_COORD NewAreaCorner1 = stLFArea.coCorner1;
	PR_COORD NewAreaCorner2 = stLFArea.coCorner2;
	CString szTemp;
	UCHAR ucColletID = 0;

	szLog.Format("WPR AutoLookBackwardLastDie - SrchWnd wo EJTXY (%d, %d, %d, %d)", 
		stLFArea.coCorner1.x, stLFArea.coCorner1.y, stLFArea.coCorner2.x, stLFArea.coCorner2.y);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (bLFNoComp)
	{
		//BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
		//if ( !bBHZ2TowardsPick )	//BH1
		if (CMS896AStn::m_nWTAtColletPos == 1)			//BH1
		{
			ucColletID = 1;
			NewAreaCorner1.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
			NewAreaCorner1.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
			NewAreaCorner2.x += (PR_WORD)m_lEjtCollet1OffsetX_Pixel;
			NewAreaCorner2.y += (PR_WORD)m_lEjtCollet1OffsetY_Pixel;
			//szTemp.Format("C1 LA Wnd: %d,%d, %d,%d", NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y);
			//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		}
		else if (CMS896AStn::m_nWTAtColletPos == 2)		//BH2
		{
			ucColletID = 2;
			NewAreaCorner1.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
			NewAreaCorner1.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
			NewAreaCorner2.x += (PR_WORD)m_lEjtCollet2OffsetX_Pixel;
			NewAreaCorner2.y += (PR_WORD)m_lEjtCollet2OffsetY_Pixel;
			//szTemp.Format("C2 LA Wnd: %d,%d, %d,%d", NewAreaCorner1.x, NewAreaCorner1.y, NewAreaCorner2.x, NewAreaCorner2.y);
			//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		}

		VerifyPRRegion_MxN(NewAreaCorner1, NewAreaCorner2);	
	}

	stSrchEpoxyCenter.x = (stLFArea.coCorner2.x + stLFArea.coCorner1.x) / 2;
	stSrchEpoxyCenter.y = (stLFArea.coCorner2.y + stLFArea.coCorner1.y) / 2;


	szLog.Format("WPR AutoLookBackwardLastDie - SrchWnd with EJTXY (%d, %d, %d, %d), Epoxy Center (%d, %d)", 
		stLFArea.coCorner1.x, stLFArea.coCorner1.y, stLFArea.coCorner2.x, stLFArea.coCorner2.y,
		stSrchEpoxyCenter.x, stSrchEpoxyCenter.y);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	UINT nX=0, nY=0;
	ULONG lEpoxySize = 0;
	CString szErrMsg = "";
	BOOL bEproxyStatus = AutoSearchEpoxy(TRUE, FALSE, FALSE, stSrchEpoxyCenter.x, stSrchEpoxyCenter.y, nX, nY, lEpoxySize, NULL,szErrMsg);

	szLog.Format("WPR AutoLookBackwardLastDie at (%ld, %ld) - IsEmpty = TRUE, result = %d", 
					lUserCol, lUserRow, bEproxyStatus);
	
	if (!bEproxyStatus)
	{
		//Epoxy not found -> no EMPTY HOLE -> fails !!!!
		(*m_psmfSRam)["WaferPr"]["LookBackward"]["IsEmptyDie"] = FALSE;
		szLog.Format("WPR AutoLookBackwardLastDie at (%ld, %ld) - IsEmpty = FALSE, result = %d", 
					lUserCol, lUserRow, bEproxyStatus);
		bReturn = FALSE;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return bReturn;
}

BOOL CWaferPr::PerformWTPrVibrationTest()	//v4.59A40
{
	if (!CMS896AStn::m_bMS90WTPrVibrateTest)
	{
		return TRUE;
	}

	if (IsBurnIn())		
	{
		return TRUE;
	}

	BOOL bReturn = TRUE;

	LONG lLastDieDirection	= (LONG)(*m_psmfSRam)["WaferMap"]["LastDieDirection"];
	BOOL bLFNoComp			= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];
	//(*m_psmfSRam)["WaferMap"]["LastDie"]["LookBackward_X"];
	//(*m_psmfSRam)["WaferMap"]["LastDie"]["LookBackward_Y"];
	LONG lUserRow	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserRow"];
	LONG lUserCol	= (*m_psmfSRam)["WaferMap"]["LastDie"]["UserCol"];

	CString szLog;
	szLog.Format("WPR PerformWTPrVibrationTest - LastPos = %ld, bLFNoComp = %d", 
		lLastDieDirection, bLFNoComp);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (!bLFNoComp)
	{
		return TRUE;
	}

	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset; 
	PR_REAL				fDieScore;
	PR_COORD			stDieSize;
	int					siStepX=0, siStepY=0;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_WIN stLFArea = GetSrchArea();
	
	//always at cursor center posn !
	stLFArea.coCorner1 = GetSrchArea().coCorner1;
	stLFArea.coCorner2 = GetSrchArea().coCorner2;

	//Search CENTER die again in same image
	usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID, 
							stLFArea.coCorner1, stLFArea.coCorner2, 
							PR_FALSE, PR_TRUE, PR_FALSE, 
							PR_NO_DISPLAY, 0, 0);
	
	if (usSearchResult != PR_ERR_NOERR)
	{
		CString szErr;
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			szErr.Format("WPR Not Learnt");
		}
		else
		{
			szErr.Format("WPR PrVibrationTest cmd fails - ErrCode = %d", usSearchResult);
		}
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR)
	{
		CString szErr;
		szErr.Format("WPR PrVibrationTest RPY1 cmd fails - ErrCode = %d", usSearchResult);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	//Get reply 2
	usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		CString szErr;
		szErr.Format("WPR PrVibrationTest RPY2 cmd fails - ErrCode = %d", usSearchResult);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
	{
        usDieType = PR_ERR_GOOD_DIE;
    }

	ExtractDieResult(stSrchRpy2, stSrchRpy3, FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);

	if (DieIsAlignable(usDieType))
	{
		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

		CString szLog;
		szLog.Format("%d,%d,%d,%d", stDieOffset.x, stDieOffset.y, siStepX, siStepY);
SetErrorMessage(szLog);
	}

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return bReturn;
}

BOOL CWaferPr::AutoLookAroundDie()
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	PR_UWORD			usSearchResult;
	PR_UWORD			usDieType;
	PR_REAL				fDieRotate; 
	PR_COORD			stDieOffset; 
	PR_REAL				fDieScore;
	PR_COORD			stDieSize;
	PR_BOOLEAN			bInspection;
	BOOL				bGetRpy3;
	LONG				lDirection;
	int					siStepX, siStepY;
	PR_BOOLEAN			bPrLatch = PR_FALSE;


	//Init variable & get Lookforward direction
	siStepX		= 0; 
	siStepY		= 0;
	bGetRpy3	= FALSE;
	bInspection = PR_FALSE;
	lDirection	= (*m_psmfSRam)["WaferMap"]["NextDieDirection"];

	//andrew2
	BOOL bLFNoComp = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"];		
	if (bLFNoComp)
	{
		bPrLatch = PR_TRUE;		//Need to LATCH if current cycle is NO-COMPENSATE
	}

	if ( ( IsNormalDieLearnt() == FALSE) && IsBurnIn() )
	{
		(*m_psmfSRam)["WaferPr"]["LookForward"]["X"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["Y"]		= 0;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["Theta"]	= 0.0;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["FullDie"]	= FALSE;
		(*m_psmfSRam)["WaferPr"]["LookForward"]["Rotate"]	= FALSE;
		return TRUE;
	}


	//Search die Start
	INT i;
	ULONG ulRow, ulCol, ulNextRow, ulNextCol;
	ULONG ulNumRow = 0, ulNumCol = 0, ulLaRow, ulLaCol;
	ulCol		= (LONG)(*m_psmfSRam)["WaferPr"]["LookAround"]["X"];
	ulRow		= (LONG)(*m_psmfSRam)["WaferPr"]["LookAround"]["Y"];
	ulNextRow	= (*m_psmfSRam)["WaferPr"]["LookAround"]["NextRow"];
	ulNextCol	= (*m_psmfSRam)["WaferPr"]["LookAround"]["NextCol"];
	BOOL bNextDie = FALSE;
	UCHAR ucPickMode = 4;

	WM_CWaferMap::Instance()->GetWaferMapDimension(ulNumRow, ulNumCol);

	ULONG ulCounter = 0;
	for (i=0; i<8; i++)
	{
		ulLaRow = ulLaCol = 1000;
		switch (i)
		{
		case WPR_LT_DIE: 
			 if (ulCol == 0) break;
			 ulLaRow = ulRow;
			 ulLaCol = ulCol-1;
			 break;
		case WPR_RT_DIE: 
			 if (ulCol == ulNumCol-1) break;
			 ulLaRow = ulRow;
			 ulLaCol = ulCol+1;
			 break;
		case WPR_UP_DIE: 
			 if (ulRow == 0) break;
			 ulLaRow = ulRow-1;
			 ulLaCol = ulCol;
			 break;
		case WPR_DN_DIE: 
			 if (ulRow == ulNumRow-1) break;
			 ulLaRow = ulRow+1;
			 ulLaCol = ulCol;
			 break;
		case WPR_UL_DIE: 
			 if (ulRow == 0 || ulCol == 0) break;
			 ulLaRow = ulRow-1;
			 ulLaCol = ulCol-1;
			 break;
		case WPR_DL_DIE: 
			 if (ulRow == ulNumRow-1 || ulCol == 0) break;
			 ulLaRow = ulRow+1;
			 ulLaCol = ulCol-1;
			 break;
		case WPR_UR_DIE: 
			 if (ulRow == 0 || ulCol == ulNumCol-1) break;
			 ulLaRow = ulRow-1;
			 ulLaCol = ulCol+1;
			 break;
		case WPR_DR_DIE: 
			 if (ulRow == ulNumRow-1 || ulCol == ulNumCol-1) break;
			 ulLaRow = ulRow+1;
			 ulLaCol = ulCol+1;
			 break;
		}

		if (ulLaRow == 1000 || ulLaCol == 1000) continue;

		if (!m_bBlkLongJump)	//v3.25T16		//OptoTech
		{
			if (m_WaferMapWrapper.GetDieState(ulLaRow,ulLaCol) == WT_MAP_DS_PR_EMPTY)
				continue;
			if (WM_CWaferMap::Instance()->IsMapNullBin(ulLaRow,ulLaCol))
				continue;
		}

		ulCounter++;

		//andrew2
		CString szDebug;
		if ( (ulNextRow != 0) && (ulNextRow == ulLaRow) && (ulNextCol != 0) && (ulNextCol == ulLaCol) )
		{
			//Need to to inspection for target LA next die
			if (CMS896AApp::m_lVisionPlatform == PR_SYSTEM_AOI)		//v4.05		//Klocwork
				bInspection = PR_TRUE;
			else
				bInspection = PR_FALSE;
			bNextDie	= TRUE;
			ucPickMode	= 3;
		}
		else
		{
			bInspection = PR_FALSE;
			bNextDie	= FALSE;
			ucPickMode	= 4;
		}

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

		usSearchResult = SearchDieCmd(WPR_NORMAL_DIE, WPR_NORMAL_DIE, 
										ubSID, 
										ubRID, 
										m_stLFSearchArea[i].coCorner1, 
										m_stLFSearchArea[i].coCorner2, 
										bPrLatch, 
										PR_TRUE, 
										bInspection, 
										PR_NO_DISPLAY);
		if (usSearchResult != PR_ERR_NOERR)
		{
			CString csMsg;
			if (usSearchResult == IDS_WPR_DIENOTLEARNT)
			{
				csMsg.Format("WPR Not Learnt");
			}
			else
			{
				csMsg.Format("Send LF SrchDie Cmd error! = %x",usSearchResult);
			}
			DisplayMessage(csMsg);	
			return FALSE;
		}

		//Get reply 1
		usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
		if (usSearchResult != PR_ERR_NOERR)
		{
			CString csMsg;
			csMsg.Format("Get LA reply 1 error! = %x (%d)",usSearchResult, i);
			DisplayMessage(csMsg);	
			return FALSE;
		}

		//andrew2
		//No need to LATCH for other LA dice beside die-1
		if (bPrLatch)	
		{
			bPrLatch = PR_FALSE;

			// Allow WT & BH to proceed once LF image is grabbed
			if (bLFNoComp)		// If NO-COMPENSATE, current die at center now must be GOOD die	!!
			{
				//Curr-die status is good from last LF result
				SetPRStart(TRUE);			//Allow BH-T to PICK
				TakeTime(WPR2);		
				SetPRLatched(TRUE);			//Allow BT to MOVE

				TakeTime(WPR3);
				DisplaySequence("WPR3 7");
				SetBadDie(FALSE, "6");
				SetBadDieForT(FALSE);
				SetDieReady(TRUE);			//Allow BH-Z to PICK
			}
		}


		//Get reply 2
		usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
		if (usSearchResult == PR_COMM_ERR)
		{
			CString csMsg;
			csMsg.Format("Get LA reply 2 error! = %x",usSearchResult);
			DisplayMessage(csMsg);	
			return FALSE;
		}

		usDieType = stSrchRpy2.stStatus.uwPRStatus;
		if (usDieType == PR_ERR_NOERR)
		{
			usDieType = PR_ERR_GOOD_DIE;
		}


		//andrew2
		//DieIsAlignable(usDieType);
		if( ( DieIsAlignable(usDieType) ) && bInspection)
		{
			usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
			if (usSearchResult == PR_COMM_ERR)
			{
				CString csMsg;
				csMsg.Format("Get LF reply 3 error! = %x",usSearchResult);
				DisplayMessage(csMsg);	
				return FALSE;
			}
		}


		ExtractDieResult(stSrchRpy2, stSrchRpy3, bInspection, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, &stDieSize);

		if (DieIsAlignable(usDieType) == TRUE)
		{
			m_WaferMapWrapper.SetDieState(ulLaRow,ulLaCol, WT_MAP_DIESTATE_15);
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
			CalculateNDieOffsetXY(siStepX, siStepY);		//v3.77

			if (i==0) 
			{
				(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundLT"]["PosY"] = siStepY;
			}
			else if (i==1)
			{
				(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundRT"]["PosY"] = siStepY;
			}
			else if (i==2)
			{
				(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundUP"]["PosY"] = siStepY;
			}
			else if (i==3)
			{
				(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundDN"]["PosY"] = siStepY;
			}
			else if (i==4)
			{
				(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundUL"]["PosY"] = siStepY;
			}
			else if (i==5)
			{
				(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundDL"]["PosY"] = siStepY;
			}
			else if (i==6)
			{
				(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundUR"]["PosY"] = siStepY;
			}
			else if (i==7)
			{
				(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosX"] = siStepX;
				(*m_psmfSRam)["WaferPr"]["LAroundDR"]["PosY"] = siStepY;
			}
		}
		else
		{
			m_WaferMapWrapper.SetMissingDie(ulLaRow,ulLaCol);
			m_WaferMapWrapper.SetDieState(ulLaRow,ulLaCol, WT_MAP_DS_PR_EMPTY);
		}

		//andrew2
		if (bNextDie)
		{
			(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsGood"] = DieIsGood(usDieType);
			(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieIsAlign"] = DieIsAlignable(usDieType);
			(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieT"]		= (DOUBLE) fDieRotate;

			if (DieNeedRotate(fDieRotate))
				(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieTNeedRotate"]	= (BOOL) TRUE;
			else
				(*m_psmfSRam)["WaferPr"]["LookAround"]["NextDieTNeedRotate"]	= (BOOL) FALSE;
		}

		if (ulCounter >= 8) break;
	}

	//andrew2
	//In case no LA die is arcound current No-Compenastate posn
	if (bPrLatch)	
	{
		bPrLatch = PR_FALSE;

		// Allow WT & BH to proceed once LF image is grabbed
		if (bLFNoComp)		// If NO-COMPENSATE, current die at center now must be GOOD die	!!
		{
			//Curr-die status is good from last LF result
			SetPRStart(TRUE);			//Allow BH-T to PICK
			TakeTime(WPR2);		
			SetPRLatched(TRUE);			//Allow BT to MOVE

			TakeTime(WPR3);
			DisplaySequence("WPR3 8");
			SetBadDie(FALSE, "7");
			SetBadDieForT(FALSE);
			SetDieReady(TRUE);			//Allow BH-Z to PICK
		}
	}

	return TRUE;
}

BOOL CWaferPr::AutoEnlargeSrchWndSrchDie(PR_SRCH_DIE_RPY2* pstNewSrchRpy2, PR_SRCH_DIE_RPY3* pstNewSrchRpy3)
{
	if (!m_bAutoEnlargeSrchWnd)
		return FALSE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(MS896A_FUNC_VISION_AUTO_ENLARGE_SRCHWND))
		return FALSE;

	PR_UWORD	usSearchResult;
	BOOL		bDieType = WPR_NORMAL_DIE;
	LONG		lRefDieNo = 1;//WPR_NORMAL_DIE

	PR_SRCH_DIE_RPY1	stSrchRpy1;

	PR_COORD	stSrchCorner1 = GetSrchArea().coCorner1;
	PR_COORD	stSrchCorner2 = GetSrchArea().coCorner2;

	//# Enlarge current Srch Wnd to search good-die again (ref-die not supported
	PR_WIN stArea;
	GetSearchDieArea(&stArea, WPR_NORMAL_DIE, 1);
	stSrchCorner1 = stArea.coCorner1;
	stSrchCorner2 = stArea.coCorner2;


	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	//Use PrLatch = FALSE on same image
	//#0 SrchDie Cmd
	lRefDieNo = m_lCurrSrchDieId;
	usSearchResult = SearchDieCmd(bDieType, lRefDieNo, ubSID, ubRID, stSrchCorner1, stSrchCorner2, PR_FALSE, PR_TRUE, PR_TRUE, PR_NO_DISPLAY);
	if (usSearchResult != PR_ERR_NOERR)
	{
		return FALSE;
	}


	//#1 RPY1
	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	if ((usSearchResult != PR_ERR_NOERR) || 
		(stSrchRpy1.uwCommunStatus != PR_COMM_NOERR) || 
		(stSrchRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		return FALSE;
	}


	//#2 RPY2
	usSearchResult = SearchDieRpy2(ubSID, pstNewSrchRpy2);
	if ((usSearchResult == PR_COMM_ERR) || 
		(pstNewSrchRpy2->stStatus.uwCommunStatus != PR_COMM_NOERR))
	{
		return FALSE;
	}


	//#3 RPY3
    USHORT usDieType = pstNewSrchRpy2->stStatus.uwPRStatus;
	if (usDieType == PR_ERR_NOERR)
        usDieType = PR_ERR_GOOD_DIE;
	if (DieIsAlignable(usDieType) == TRUE)
	{
		usSearchResult = SearchDieRpy3(ubSID, pstNewSrchRpy3);
		if ((usSearchResult == PR_COMM_ERR) ||
			(pstNewSrchRpy3->stStatus.uwCommunStatus == PR_COMM_ERR))
		{
			return FALSE;
		}
	}

	//If die no good, return FALSE
	if (!DieIsAlignable(usDieType))
		return FALSE;

	return DieIsAlignable(usDieType);
}

BOOL CWaferPr::IndexAndSearchRefDie(int *siPosX, int *siPosY, LONG lDirection, LONG lRefDieNo)
{
	PR_COORD	stSrchCorner1;
	PR_COORD	stSrchCorner2;
	stSrchCorner1.x = GetPRWinULX();
	stSrchCorner1.y = GetPRWinULY();
	stSrchCorner2.x = GetPRWinLRX();
	stSrchCorner2.y = GetPRWinLRY();
	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	LONG		lRefNo = lRefDieNo;
	LONG		lDiff_X = 0;
	LONG		lDiff_Y = 0;
	int			siOrigT = 0;
	int			siOrigX = 0;
	int			siOrigY = 0;
	int			i = 0;

	DOUBLE dLFSize = 1; 

	siOrigX = *siPosX;
	siOrigY = *siPosY;

	if ( IsLFSizeOK() )
	{
		dLFSize = 3;
	}

	switch (lDirection)
	{
	case 1:		//Right
		lDiff_X = (LONG) dLFSize;
		break;

	case 2:		//Up
		lDiff_Y = -(LONG) dLFSize;
		break;

	case 3:		//Down
		lDiff_Y = (LONG) dLFSize;
		break;

	case 0:		//Left
		lDiff_X = -(LONG) dLFSize;
		break;

	default:	//Center die
		lDiff_X = 0;
		lDiff_Y = 0;
		break;
	}


	//Move table
	*siPosX = *siPosX - lDiff_X * GetDiePitchXX() - lDiff_Y * GetDiePitchYX();
	*siPosY = *siPosY - lDiff_Y * GetDiePitchYY() - lDiff_X * GetDiePitchXY();

	MoveWaferTable(*siPosX, *siPosY);

	//Search reference die with user defined search region
	if (ManualSearchDie(WPR_REFERENCE_DIE, lRefNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2) == -1)
	{
		return FALSE;
	}

	if (DieIsGood(usDieType) == TRUE)
	{
		ManualDieCompenate(stDieOffset, fDieRotate);
		GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
		
		*siPosX = siOrigX;
		*siPosY = siOrigY;
		return TRUE;
	}

	//If no ref-die found, then try to locate a normal die at center for alignment purpose only
	PR_WORD wResult = 0;
	wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			if ( lDirection==-1 )
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			}
			else
			{
				int siStepX, siStepY;
				CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

				siOrigX = *siPosX;
				siOrigY = *siPosY;

				siOrigX += siStepX;
				siOrigY += siStepY;
			}

			*siPosX = siOrigX;
			*siPosY = siOrigY;
		}
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}

BOOL CWaferPr::IndexAndSearchRefHomeDie(int *siPosX, int *siPosY, LONG lDirection)
{
	m_bHomeDieUniqueResult = TRUE;
	if( IsPrescanMapIndex() )
	{
		return IndexAndFovSearchReferDie(siPosX, siPosY, lDirection);
	}

	PR_COORD	stSrchCorner1;
	PR_COORD	stSrchCorner2;
	stSrchCorner1.x = GetPRWinULX();
	stSrchCorner1.y = GetPRWinULY();
	stSrchCorner2.x = GetPRWinLRX();
	stSrchCorner2.y = GetPRWinLRY();
	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	PR_RCOORD	stCharDieScore;

	LONG		lRefNo = 1;
	LONG		lDiff_X = 0;
	LONG		lDiff_Y = 0;
	int			siOrigT = 0;
	int			siOrigX = 0;
	int			siOrigY = 0;
	int			i = 0;

	LONG lLFSizeX = 1, lLFSizeY = 1;
	if ( IsLFSizeOK() )
	{
		lLFSizeX = 3;
		lLFSizeY = 3;
	}

	lLFSizeX = GetSortFovCol();
	lLFSizeY = GetSortFovRow();
	if( lLFSizeX%2==0 )
		lLFSizeX--;
	if( lLFSizeY%2==0 )
		lLFSizeY--;
	if( lLFSizeX<=0 )
		lLFSizeX = 1;
	if( lLFSizeY<=0 )
		lLFSizeY = 1;


	siOrigX = *siPosX;
	siOrigY = *siPosY;

	switch (lDirection)
	{
	case 1:		//Right
		lDiff_X = lLFSizeX;
		break;

	case 2:		//Up
		lDiff_Y = -lLFSizeY;
		break;

	case 3:		//Down
		lDiff_Y = lLFSizeY;
		break;

	case 0:		//Left
		lDiff_X = -lLFSizeX;
		break;

	default:	//Center die
		lDiff_X = 0;
		lDiff_Y = 0;
		break;
	}

	DOUBLE dTime = GetTime();
	CMSLogFileUtility::Instance()->WL_LogStatus("WPR index to refer die");

	//Move table
	*siPosX = *siPosX - lDiff_X * GetDiePitchXX() - lDiff_Y * GetDiePitchYX();
	*siPosY = *siPosY - lDiff_Y * GetDiePitchYY() - lDiff_X * GetDiePitchXY();

	MoveWaferTable(*siPosX, *siPosY);

	CString szMsg;
	szMsg.Format(" prev. used time %.2f", GetTime()-dTime);
	dTime = GetTime();
	CMSLogFileUtility::Instance()->WL_LogStatus("WPR search refer die" + szMsg);
	//search character die
	if ( IsOcrAOIMode()==FALSE && IsCharDieInUse()  && (m_lLrnTotalRefDie > WPR_GEN_CDIE_OFFSET) )
	{
		PR_COORD	stDieDigit;
		LONG lRtn;
		PR_REAL		fDieRotate;
		stDieDigit.x = -1;
		stDieDigit.y = -1;
		lRtn = SearchNumberDie(&stDieOffset, &fDieRotate, &stCharDieScore, &stDieDigit);
		if (lRtn == TRUE)
		{
			fDieRotate = 0;
			ManualDieCompenate(stDieOffset, fDieRotate);
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			*siPosX = siOrigX;
			*siPosY = siOrigY;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		//Search reference die with user defined search region
		if (ManualSearchDie(WPR_REFERENCE_DIE, lRefNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2) == -1)
		{
			return FALSE;
		}

		if (DieIsGood(usDieType) == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WPR found refer die");
			ManualDieCompenate(stDieOffset, fDieRotate);
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			
			*siPosX = siOrigX;
			*siPosY = siOrigY;

			PR_COORD	stSrchUL = GetSrchArea().coCorner1;
			PR_COORD	stSrchLR = GetSrchArea().coCorner2;
			if( m_bHomeDieUniqueCheck )	// check normal die 1 and all other refer die>1
			{
				PR_UWORD	uwType;
				PR_COORD	stOffset; 
				PR_REAL		fRotate, fScore; 
				CString szMsg;
				PR_WORD wReturn = 0;
				CMSLogFileUtility::Instance()->WL_LogStatus("WPR unique check refer die");

				wReturn =    ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &uwType, &fRotate, &stOffset, &fScore, stSrchUL, stSrchLR);
				if( wReturn==-1 )
				{
					szMsg.Format("PR ERROR to search Normal die at Home die position!");
					HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
					SetErrorMessage(szMsg);
					m_bHomeDieUniqueResult = FALSE;
					return TRUE;
				}
				if( DieIsGood(uwType) )
				{
					szMsg.Format("Find Normal die at Home die position!");
					HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
					SetErrorMessage(szMsg);
					m_bHomeDieUniqueResult = FALSE;
					return TRUE;
				}

				for(int i=1; i<m_lLrnTotalRefDie; i++)
				{
					wReturn = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE+i, PR_TRUE, PR_TRUE, PR_TRUE, &uwType, &fRotate, &stOffset, &fScore, stSrchUL, stSrchLR);
					if( wReturn==-1 )
					{
						szMsg.Format("Search refer die PR error at Home die position");
						HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
						SetErrorMessage(szMsg);
						m_bHomeDieUniqueResult = FALSE;
						return TRUE;
					}

					if( DieIsGood(uwType) )
					{
						szMsg.Format("Find refer %d die at Home die position", WPR_REFERENCE_DIE+i);
						HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
						SetErrorMessage(szMsg);
						m_bHomeDieUniqueResult = FALSE;
						return TRUE;
					}
				}
			}

			if( m_bCompareHomeReferDieScore )
			{
				PR_UWORD	usDieType1;
				PR_COORD	stDieOffset1; 
				PR_REAL		fDieRotate1; 
				PR_REAL		fDieScore1;
				CString szMsg;
				CMSLogFileUtility::Instance()->WL_LogStatus("WPR compare score of refer die");
				if( ManualSearchDie(WPR_REFERENCE_DIE, lRefNo+1, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType1, &fDieRotate1, &stDieOffset1, &fDieScore1, stSrchUL, stSrchLR)!=-1 )
				{
					if( DieIsGood(usDieType1) )
					{
						if( fDieScore<=fDieScore1 )
						{
							szMsg.Format("Home die score (%.2f) is not larger than refer die score (%.2f)",
								fDieScore, fDieScore1);
							HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
							SetErrorMessage(szMsg);
							m_bHomeDieUniqueResult = FALSE;
							return TRUE;
						}
						else
						{
							szMsg.Format("Home die score (%.2f) is larger than refer die score (%.2f)",
								fDieScore, fDieScore1);
							SetErrorMessage(szMsg);
						}
					}
				}
				else
				{
					szMsg.Format("Search refer die PR error at Home die position");
					HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
					SetErrorMessage(szMsg);
					m_bHomeDieUniqueResult = FALSE;
					return TRUE;
				}
			}

			return TRUE;
		}
	}

	szMsg.Format(" prev. used time %.2f", GetTime()-dTime);
	dTime = GetTime();
	CMSLogFileUtility::Instance()->WL_LogStatus("WPR search normal die" + szMsg);
	//If no ref-die found, then try to locate a normal die at center for alignment purpose only
	PR_WORD wResult = 0;
	wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);

	if( (wResult != -1) && (DieIsAlignable(usDieType) == TRUE) )
	{
		if ( lDirection==-1 )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WPR compensate normal die die");
			ManualDieCompenate(stDieOffset, fDieRotate);
			GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
		}
		else
		{
			int siStepX, siStepY;
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

			siOrigX = *siPosX;
			siOrigY = *siPosY;

			siOrigX += siStepX;
			siOrigY += siStepY;
		}

		*siPosX = siOrigX;
		*siPosY = siOrigY;
	}
	szMsg.Format(" prev. used time %.2f", GetTime()-dTime);
	dTime = GetTime();
	CMSLogFileUtility::Instance()->WL_LogStatus("WPR normal die done" + szMsg);

	return FALSE;
}


BOOL CWaferPr::IndexAndSearchNormalDie(int *siPosX, int *siPosY, LONG lIdxPos, LONG lSrchPos, BOOL bMoveTable, BOOL bDoComp, PR_BOOLEAN bLatch, PR_BOOLEAN bCheckDefect)
{
	PR_WIN	 stSrchWin;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL	 fDieRotate; 
	PR_REAL	 fDieScore;

	LONG lDiff_X = 0;
	LONG lDiff_Y = 0;
	int	siOrigT = 0;
	int	siOrigX = 0;
	int	siOrigY = 0;
	int	i = 0;
	

	siOrigX = *siPosX;
	siOrigY = *siPosY;

	stSrchWin.coCorner1 = GetSrchArea().coCorner1;
	stSrchWin.coCorner2 = GetSrchArea().coCorner2;

	switch (lIdxPos)
	{
		case WPR_LT_DIE:		
			lDiff_X = -1;
			break;

		case WPR_RT_DIE:
			lDiff_X = 1;
			break;

		case WPR_UP_DIE:
			lDiff_Y = -1;
			break;

		case WPR_DN_DIE:		
			lDiff_Y = 1;
			break;

		case WPR_UL_DIE:		
			lDiff_X = -1;
			lDiff_Y = -1;
			break;

		case WPR_DL_DIE:
			lDiff_X = -1;
			lDiff_Y = 1;
			break;

		case WPR_UR_DIE:
			lDiff_X = 1;
			lDiff_Y = -1;
			break;

		case WPR_DR_DIE:		
			lDiff_X = 1;
			lDiff_Y = 1;
			break;

		default:	
			lDiff_X = 0;
			lDiff_Y = 0;
			break;
	}

	*siPosX = *siPosX - lDiff_X * GetDiePitchXX() - lDiff_Y * GetDiePitchYX();
	*siPosY = *siPosY - lDiff_Y * GetDiePitchYY() - lDiff_X * GetDiePitchXY();

	if ( bMoveTable == TRUE )
	{
		//Move table
		MoveWaferTable(*siPosX, *siPosY);
	}

	if ( lSrchPos != WPR_CT_DIE )
	{
		stSrchWin.coCorner1 = m_stLFSearchArea[lSrchPos].coCorner1;
		stSrchWin.coCorner2 = m_stLFSearchArea[lSrchPos].coCorner2;
	}

	//v2.83T7
	//Search reference die with user defined search region
	PR_WORD wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, bLatch, PR_TRUE, bCheckDefect, 
							&usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchWin.coCorner1, stSrchWin.coCorner2);

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			if ( bDoComp == TRUE )
			{
				ManualDieCompenate(stDieOffset, fDieRotate);
				Sleep(25);
				GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);
			}
			else
			{
				int siStepX, siStepY;
				CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
				
				siOrigX += siStepX;
				siOrigY += siStepY;
			}

			*siPosX = siOrigX;
			*siPosY = siOrigY;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}


BOOL CWaferPr::LookAroundNormalDie(int& siPosX, int& siPosY, LONG lSrchPos, PR_BOOLEAN bLatch, CONST LONG lDieNo)
{
	PR_WIN	 stSrchWin;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL	 fDieRotate; 
	PR_REAL	 fDieScore;

	LONG lSrchDieNo = WPR_NORMAL_DIE + lDieNo;

	// Define search window
	stSrchWin.coCorner1 = GetSrchArea().coCorner1;
	stSrchWin.coCorner2 = GetSrchArea().coCorner2;
	if ( lSrchPos != WPR_CT_DIE )
	{
		stSrchWin.coCorner1 = m_stLFSearchArea[lSrchPos].coCorner1;
		stSrchWin.coCorner2 = m_stLFSearchArea[lSrchPos].coCorner2;
	}

	//Search normal-die inside the defined search-window

	// add branch for IM machine
	if (ManualSearchDie(WPR_NORMAL_DIE, lSrchDieNo, bLatch, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchWin.coCorner1, stSrchWin.coCorner2) != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			int siStepX, siStepY;
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
			siPosX = siStepX;
			siPosY = siStepY;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}


BOOL CWaferPr::LookAroundRefDie(int& siPosX, int& siPosY, LONG lSrchPos, PR_BOOLEAN bLatch)
{
	PR_WIN	 stSrchWin;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL	 fDieRotate; 
	PR_REAL	 fDieScore;


	// Define search window
	stSrchWin.coCorner1 = GetSrchArea().coCorner1;
	stSrchWin.coCorner2 = GetSrchArea().coCorner2;
	if ( lSrchPos != WPR_CT_DIE )
	{
		stSrchWin.coCorner1 = m_stLFSearchArea[lSrchPos].coCorner1;
		stSrchWin.coCorner2 = m_stLFSearchArea[lSrchPos].coCorner2;
	}

	//Search normal-die inside the defined search-window

	// add branch for IM machine
	if (ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE, bLatch, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchWin.coCorner1, stSrchWin.coCorner2) != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			int siStepX, siStepY;
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
			siPosX = siStepX;
			siPosY = siStepY;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}


BOOL CWaferPr::LookAroundNormalDie_mxn(int& siPosX, int& siPosY, INT nLFRegionSize, LONG lSrchPos, PR_BOOLEAN bLatch)
{
	PR_WIN	 stSrchWin;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL	 fDieRotate; 
	PR_REAL	 fDieScore;


	// Define search window
	stSrchWin.coCorner1 = GetSrchArea().coCorner1;
	stSrchWin.coCorner2 = GetSrchArea().coCorner2;
	if ( lSrchPos != WPR_CT_DIE )
	{
		stSrchWin.coCorner1 = m_stLFSearchArea[lSrchPos].coCorner1;
		stSrchWin.coCorner2 = m_stLFSearchArea[lSrchPos].coCorner2;
	}

	PR_WIN stSrchWnd;
	switch (nLFRegionSize)
	{
	case REGION_SIZE_5x3:
		GetLFSearchArea_5x3(stSrchWnd, lSrchPos);
		break;
	case REGION_SIZE_3x5:
		return FALSE;
	case REGION_SIZE_3x3:
		GetLFSearchArea_3x3(stSrchWnd, lSrchPos);
		break;
	case REGION_SIZE_4x4:
		return FALSE;
	case REGION_SIZE_5x5:
		GetLFSearchArea_5x5(stSrchWnd, lSrchPos);
		break;
	case REGION_SIZE_7x7:
		return FALSE;
	default:
		return FALSE;
	}

	//Search normal-die inside the defined search-window
	if (ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, bLatch, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchWnd.coCorner1, stSrchWnd.coCorner2) != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			int siStepX, siStepY;
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
			siPosX = siStepX;
			siPosY = siStepY;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}


PR_WORD CWaferPr::ManualSearchCharDie(PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_RCOORD *stDieScore)
{
    PR_COORD			stPRCoorCentre;
	PR_GRAPHIC_INFO		emInfo;

	stPRCoorCentre.x  = (PR_WORD)GetPrCenterX();
	stPRCoorCentre.y  = (PR_WORD)GetPrCenterY();

	if ( m_bDebugVideoTest == TRUE )
		emInfo = PR_SHOW_DEFECT;
	else
		emInfo = PR_NO_DISPLAY;

	LONG lRtn;

	PR_COORD	stDieDigit;
	stDieDigit.x = -1;
	stDieDigit.y = -1;
	stDieScore->x = 0;
	stDieScore->y = 0;
	*fDieRotate = 0;
	lRtn = SearchNumberDie(stDieOffset, fDieRotate, stDieScore, &stDieDigit);
	if (lRtn)
	{
		*usDieType = PR_ERR_GOOD_DIE;
	}
	else
	{
		*usDieType = PR_ERR_NO_DIE;
	}

	if( IsOcrAOIMode()==FALSE )
	{
		CString csMsgD1 = "?";
		CString csMsgD2 = "?";

		if (stDieDigit.x != -1)
		{
			csMsgD1.Format("%d",stDieDigit.x);
		}
		if (stDieDigit.y != -1)
		{
			csMsgD2.Format("%d",stDieDigit.y);
		}
		m_szAoiOcrPrValue	= csMsgD1 + csMsgD2;	// used for non AOI display only
	}

	return 0;
}

PR_UWORD CWaferPr::SearchCharDie(PR_COORD stDieCenter, PR_RCOORD *stDieScore, PR_COORD *stDieDigit)
{
	PR_RGN_CHAR_CMD		stRgnCharCmd;
	PR_RGN_CHAR_RPY		stRgnCharRpy;
	PR_UWORD 			lReturn = 0;	
	PR_UWORD 			lSearchDieStauts[WPR_SRN_CDIE_CHAR_NO];	
	PR_REAL				fDieScore[WPR_SRN_CDIE_CHAR_NO];
	PR_UBYTE			ucDieDigit[WPR_SRN_CDIE_CHAR_NO];
	short				i = 0;
	short				j = 0;
	short				siLearntDigit = 0;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	//Check all Char die is learnt before search (char die is started from index 2)
	for (i=WPR_GEN_CDIE_OFFSET; i<(WPR_GEN_CDIE_MAX_DIE + WPR_GEN_CDIE_OFFSET); i++)
	{
		if (IsThisDieLearnt((UCHAR)i) == TRUE)
		{
			siLearntDigit++;
		}
	}

	//Search for 2 digit
	for (i=0; i<WPR_SRN_CDIE_CHAR_NO; i++)
	{
		fDieScore[i] = 0;
		ucDieDigit[i] = NULL;
		lSearchDieStauts[i] = 0;

		PR_InitRgnCharCmd(&stRgnCharCmd);

		stRgnCharCmd.uwNumOfCharInfo		= (PR_UWORD)siLearntDigit;
		stRgnCharCmd.emSameView				= PR_TRUE;			
		stRgnCharCmd.rPassScore				= 60.0;
		stRgnCharCmd.emDefectInsp			= PR_FALSE;
		stRgnCharCmd.aeMaxDefectArea		= 0;
		stRgnCharCmd.emGraphicInfo			= PR_NO_DISPLAY;

		//Look for upper direction
		stRgnCharCmd.stRgnWin.coCorner1.x	= stDieCenter.x - (GetDieSizePixelX(WPR_GEN_CDIE_OFFSET) / 2) - WPR_SRN_CDIE_OFFSET_X;
		stRgnCharCmd.stRgnWin.coCorner1.y	= stDieCenter.y - GetDieSizePixelY(WPR_GEN_CDIE_OFFSET) - WPR_SRN_CDIE_OFFSET_Y;
		stRgnCharCmd.stRgnWin.coCorner2.x	= stDieCenter.x + (GetDieSizePixelX(WPR_GEN_CDIE_OFFSET) / 2) + WPR_SRN_CDIE_OFFSET_X;
		stRgnCharCmd.stRgnWin.coCorner2.y	= stDieCenter.y + WPR_SRN_CDIE_OFFSET_Y;

		if (i == 1)
		{
			//Look for lower direction
			stRgnCharCmd.stRgnWin.coCorner1.x	= stDieCenter.x - (GetDieSizePixelX(WPR_GEN_CDIE_OFFSET) / 2) - WPR_SRN_CDIE_OFFSET_X;
			stRgnCharCmd.stRgnWin.coCorner1.y	= stDieCenter.y - WPR_SRN_CDIE_OFFSET_Y;
			stRgnCharCmd.stRgnWin.coCorner2.x	= stDieCenter.x + (GetDieSizePixelX(WPR_GEN_CDIE_OFFSET) / 2) + WPR_SRN_CDIE_OFFSET_X;
			stRgnCharCmd.stRgnWin.coCorner2.y	= stDieCenter.y + GetDieSizePixelY(WPR_GEN_CDIE_OFFSET) - WPR_SRN_CDIE_OFFSET_Y;
		}

		for (j=0; j<siLearntDigit; j++)
		{
			memset(stRgnCharCmd.astCharInfo[j].aubString,  NULL, sizeof(stRgnCharCmd.astCharInfo[j].aubString));
			stRgnCharCmd.astCharInfo[j].aubString[0] =  j + WPR_GEN_CDIE_START;
			stRgnCharCmd.astCharInfo[j].uwRecordID	 = m_ssGenPRSrchID[j+WPR_GEN_CDIE_OFFSET];   
		}

		//Search character die
		PR_RgnCharCmd(&stRgnCharCmd, ubSID, ubRID, &stRgnCharRpy);

		if (stRgnCharRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
		{
			return PR_COMM_NOERR;
		}

		fDieScore[i] = stRgnCharRpy.rScore;
		ucDieDigit[i] = stRgnCharRpy.stCharInfo.aubString[0];
		lSearchDieStauts[i] = stRgnCharRpy.stStatus.uwPRStatus;
		
		//if score not >= 90, classified as NO char found
		if (fDieScore[i] < WPR_SRN_CDIE_MIN_SCORE)
		{
			lSearchDieStauts[i] = PR_ERR_NO_CHAR_FOUND;
			ucDieDigit[i] = NULL;
		}
	}

	//Check result
	if ( (lSearchDieStauts[0] == PR_ERR_NOERR) && (lSearchDieStauts[1] == PR_ERR_NOERR) )	
	{
		lReturn = PR_ERR_GOOD_DIE;
	}
	else
	{
		lReturn = PR_ERR_NO_CHAR_FOUND;
	}

	if (ucDieDigit[0] == NULL)
	{
		stDieDigit->x = -1;
	}
	else
	{
		stDieDigit->x = ucDieDigit[0] - WPR_GEN_CDIE_START;
	}

	if (ucDieDigit[1] == NULL)
	{
		stDieDigit->y = -1;
	}
	else
	{
		stDieDigit->y = ucDieDigit[1] - WPR_GEN_CDIE_START;
	}

	stDieScore->x = fDieScore[0];
	stDieScore->y = fDieScore[1];


	return lReturn;
}

VOID CWaferPr::DisplaySearchCharDieResult(PR_UWORD usDieType, PR_REAL fDieRotate, PR_COORD stDieOffset, PR_RCOORD stDieScore)
{
	CString		csMsgA;
	CString		csMsgB;
	CString		szTitle;


	int			siXAxis; 
	int			siYAxis; 
	int			siTAxis;
	DOUBLE		dDieAngle = 0;
	BOOL		bShowResult = TRUE;

	switch(usDieType)
	{
	case PR_ERR_GOOD_DIE:
		csMsgA = " Pattern found";		
		break;

	case PR_ERR_NO_CHAR_FOUND:		
		csMsgA = " No pattern found";
		break;

	case PR_ERR_NO_DIE :		
		csMsgA = " No Die";	
		bShowResult = FALSE;
		break;

	case PR_ERR_NON_LOCATED_DEFECTIVE_DIE:
		csMsgA = " Non Located Defective Die";
		bShowResult = FALSE;
		break;

	case PR_ERR_LOCATED_DEFECTIVE_DIE:
		csMsgA = " Located Defective Die";
		bShowResult = FALSE;
		break;

	case PR_ERR_SCORE_OUT_OF_SPEC:
		csMsgA = " Score out of spec Die";
		bShowResult = FALSE;
		break;

	default:
		csMsgA.Format(" Identify Error (%d)",usDieType);	
		bShowResult = FALSE;
		break;
	}
	

	GetWaferTableEncoder(&siXAxis, &siYAxis, &siTAxis);
	
	if (bShowResult == TRUE)
	{
		csMsgA = csMsgA + ": [" + m_szAoiOcrPrValue + "]\n";	// display after manual search number die
	}
	else
	{
		csMsgA = csMsgA + "\n";
	}

	csMsgB.Format(" Die Angle: %f\n Die Pos X: %d (%d)\n Die Pos Y: %d (%d)\n Die Score: %d,%d\n",fDieRotate, stDieOffset.x, siXAxis, stDieOffset.y, siYAxis, (LONG)(stDieScore.x), (LONG)(stDieScore.y));
	csMsgA += csMsgB;

	szTitle.LoadString(HMB_WPR_SRH_DIE_RESULT);

	HmiMessage(csMsgA, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL);

}


PR_UWORD CWaferPr::GetRefDieSrchCmd(PR_SRCH_DIE_CMD* stSrchCmd, BOOL bDieType, LONG lInputDieNo, PR_COORD stDieULC, PR_COORD stDieLRC, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, LONG lGraphicInfo)
{
    PR_2_POINTS_DIE_ALIGN_CMD       st2PointsAlignCmd;
    PR_QUAD_DIE_ALIGN_CMD           stQuadAlignCmd;
    PR_STREET_DIE_ALIGN_CMD         stStreetAlignCmd;
    PR_DIE_ALIGN_PAR                stAlignPar;

    PR_TMPL_DIE_INSP_CMD            stTmplInspCmd;
    PR_LED_DIE_INSP_CMD             stLedInspCmd;

    PR_DIE_ALIGN_CMD                stDieAlignCmd;
	PR_DIE_INSP_CMD 				stDieInspCmd;
	PR_POST_INSP_CMD 			    stBadCutInspCmd;
    PR_POST_INDIV_INSP_CMD			stBadCutCmd;


	PR_COORD						stCoDieCenter;
	PR_UWORD						lPrSrchID;
//	PR_UWORD                        uwCommunStatus;
	PR_BOOLEAN						bBackupAlign = PR_FALSE;
	PR_BOOLEAN						bCheckDefect = PR_TRUE;
	PR_BOOLEAN						bCheckChip = PR_TRUE;
	PR_UBYTE						ucDefectThres = PR_NORMAL_DEFECT_THRESHOLD;
	LONG							lAlignAccuracy;
	LONG							lGreyLevelDefect;
	LONG							lDieNo = 0;
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
    PR_InitPostInspCmd(&stBadCutInspCmd);
    PR_InitPostIndivInspCmd(&stBadCutCmd);
	PR_InitDieAlignPar(&stAlignPar);

	PR_InitSrchDieCmd(stSrchCmd); 

	if (lInputDieNo <= 0)
		lInputDieNo = 1;
	if (bDieType == WPR_NORMAL_DIE)
		lDieNo = WPR_GEN_NDIE_OFFSET + lInputDieNo;
	if (bDieType == WPR_REFERENCE_DIE)
		lDieNo = WPR_GEN_RDIE_OFFSET + lInputDieNo;
	lPrSrchID	= m_ssGenPRSrchID[lDieNo];


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


	//Is check defect ??
	if ((m_dGenSrchSingleDefectArea[lDieNo] == 0.0) || (m_dGenSrchTotalDefectArea[lDieNo] == 0.0))
	{
		bCheckDefect = PR_FALSE;
	}
		
	if (m_bGenSrchEnableDefectCheck[lDieNo] == FALSE)
	{
		bCheckDefect = PR_FALSE;
	}

	//Is check Chip ??
	if (m_dGenSrchChipArea[lDieNo] == 0.0)
	{
		bCheckChip = PR_FALSE;
	}

	if (m_bGenSrchEnableChipCheck[lDieNo] == FALSE)
	{
		bCheckChip = PR_FALSE;
	}

	//Check Input parameter
	if ((bInspect == PR_FALSE) || (bAlign == FALSE))
	{
		bCheckDefect = PR_FALSE;
		bCheckChip = PR_FALSE;
	}


	if (bDieType == WPR_REFERENCE_DIE)
	{
		bBackupAlign = PR_FALSE;
	}
	else
	{
		bBackupAlign = (PR_BOOLEAN)m_bSrchEnableBackupAlign;
	}

	//Calculate chip die area, min & total defect area
	dMinChipArea		= ((DOUBLE)(GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchChipArea[lDieNo];
	dSingleDefectArea	= ((DOUBLE)(GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchSingleDefectArea[lDieNo];  
	dTotalDefectArea	= ((DOUBLE)(GetDieSizePixelX(lDieNo) * GetDieSizePixelY(lDieNo)) / 100) * m_dGenSrchTotalDefectArea[lDieNo]; 

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
	st2PointsAlignCmd.szPosConsistency.x		= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.szPosConsistency.y		= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	st2PointsAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	st2PointsAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	VerifyPRPoint(&stCoDieCenter);
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
	stStreetAlignCmd.szPosConsistency.x			= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_X + 10 * GetPrScaleFactor());
	stStreetAlignCmd.szPosConsistency.y			= (PR_LENGTH)(PR_DEF_POS_CONSISTENCY_Y + 10 * GetPrScaleFactor());
	stStreetAlignCmd.stLargeSrchWin.coCorner1	= stDieULC;
	stStreetAlignCmd.stLargeSrchWin.coCorner2	= stDieLRC;
	VerifyPRPoint(&stCoDieCenter);
	stStreetAlignCmd.coSelectPoint				= stCoDieCenter;
	stStreetAlignCmd.coProbableDieCentre		= stCoDieCenter;
	stStreetAlignCmd.rDieRotTol					= PR_SRCH_ROT_TOL;

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

	stDieAlignCmd.st2Points						= st2PointsAlignCmd;
	stDieAlignCmd.stStreet						= stStreetAlignCmd;
	stDieInspCmd.stTmpl							= stTmplInspCmd;

	stAlignPar.emEnableBackupAlign				= PR_FALSE;//bBackupAlign;
	stAlignPar.rStartAngle						= GetSearchStartAngle();	
	stAlignPar.rEndAngle						= GetSearchEndAngle();		
	//stAlignPar.emIsTurnOnMarginalSrch			= PR_TRUE;
	//stAlignPar.ubMarginalRange					= 5;

	//Update the seach die command
	stSrchCmd->emAlignAlg = GetLrnAlignAlgo(lDieNo);
	stSrchCmd->emDieInspAlg = GetLrnInspMethod(lDieNo);

	//Setup badcut parameter
	stBadCutCmd.emEpoxyInsuffChk					= PR_TRUE;
	stBadCutCmd.emEpoxyExcessChk					= PR_FALSE;
	stBadCutCmd.uwMaxInsuffSide						= 0;
	stBadCutCmd.stMinPercent.x						= (PR_REAL)(100 - m_stBadCutTolerenceX);
	stBadCutCmd.stMinPercent.y						= (PR_REAL)(100 - m_stBadCutTolerenceY);
	stBadCutCmd.szScanWidth.x						= (PR_LENGTH)m_stBadCutScanWidthX;
	stBadCutCmd.szScanWidth.y						= (PR_LENGTH)m_stBadCutScanWidthY;
	stBadCutCmd.stEpoxyInsuffWin.coCorner1			= m_stBadCutPoint[0];
	stBadCutCmd.stEpoxyInsuffWin.coCorner2			= m_stBadCutPoint[1];
	stBadCutCmd.stEpoxyInsuffWin.coObjCentre.x		= (PR_WORD)GetPrCenterX();
	stBadCutCmd.stEpoxyInsuffWin.coObjCentre.y		= (PR_WORD)GetPrCenterY();
	stBadCutInspCmd.emEpoxyChk						= PR_TRUE;
	stBadCutInspCmd.emEpoxyAlg						= PR_INDIV_INSP;
	stBadCutInspCmd.stEpoxy.stIndiv					= stBadCutCmd;


	stSrchCmd->stDieAlignPar.rStartAngle			= GetSearchStartAngle();	
	stSrchCmd->stDieAlignPar.rEndAngle				= GetSearchEndAngle();		
	stSrchCmd->emRetainBuffer						= bLatch;
	stSrchCmd->emLatch								= bLatch;
	stSrchCmd->emAlign								= bAlign;
	stSrchCmd->emDefectInsp							= bInspect;
	stSrchCmd->emCameraNo							= GetRunCamID();
    stSrchCmd->emVideoSource						= PR_IMAGE_BUFFER_A;
    stSrchCmd->uwNRecordID							= 1;                  
    stSrchCmd->auwRecordID[0]						= (PR_UWORD)(lPrSrchID);       
    stSrchCmd->coProbableDieCentre					= stCoDieCenter;
    stSrchCmd->ulRpyControlCode						= PR_DEF_SRCH_DIE_RPY;
    stSrchCmd->stDieAlign							= stDieAlignCmd;
    stSrchCmd->stDieAlignPar						= stAlignPar;
    stSrchCmd->stDieInsp							= stDieInspCmd;
    stSrchCmd->stDieAlignPar.rMatchScore			= (PR_REAL)(m_lGenSrchDieScore[lDieNo]);
	stSrchCmd->stDieAlignPar.emIsDefaultMatchScore	= PR_FALSE;
    stSrchCmd->emGraphicInfo						= (PR_GRAPHIC_INFO)(lGraphicInfo); 
	stSrchCmd->stPostBondInsp						= stBadCutInspCmd;

	if ((bCheckDefect == PR_TRUE) && (bCheckChip == PR_TRUE) && (m_bBadCutDetection == TRUE) && (m_lCurBadCutSizeX > 0) && (m_lCurBadCutSizeY > 0))
	{
		m_bGetBadCutReply = PR_TRUE;
		stSrchCmd->emPostBondInsp = PR_TRUE;  
	}
	else
	{
		m_bGetBadCutReply = FALSE;
		stSrchCmd->emPostBondInsp = PR_FALSE;  
	}

	//No need to do badcut if this die is not normal die
	if (bDieType != WPR_NORMAL_DIE)
	{
		m_bGetBadCutReply = FALSE;
		stSrchCmd->emPostBondInsp = PR_FALSE;  
	}

	return TRUE;
}


PR_UWORD CWaferPr::SearchNumberDie(PR_COORD *stDieOffset, PR_REAL *fDieRotate, PR_RCOORD *stDieScore, PR_COORD *stDieDigit)
{
	PR_UWORD lReturn =	0;	
	if( IsOcrAOIMode() )
	{
		return SearchAoiOcrDie(stDieOffset, fDieRotate, TRUE);
	}

	PR_UWORD 			lSearchDieStauts[WPR_SRN_CDIE_CHAR_NO];	
	PR_REAL				fDieScore[WPR_SRN_CDIE_CHAR_NO];
	PR_UBYTE			ucDieDigit[WPR_SRN_CDIE_CHAR_NO];
	short				i = 0;
	short				j = 0;
	short				siLearntDigit = 0;

	PR_COORD	stDieCenter;
	stDieCenter.x = PR_DEF_CENTRE_X;
	stDieCenter.y = PR_DEF_CENTRE_Y;

	m_szScanResult.Empty();

	PR_SRCH_NUMBER_DIE_CMD             stSrchNumberDieCmd;
	PR_SRCH_NUMBER_DIE_RPY1            stSrchNumberDieRpy1; 
	PR_SRCH_NUMBER_DIE_RPY2            stSrchNumberDieRpy2;

	PR_SRCH_DIE_CMD		stSrchRefCmd; 
	PR_SRCH_DIE_CMD		stSrchGoodCmd;
	PR_RGN_CHAR_CMD		stRgnCharCmd;
	PR_COORD	stSrchCorner1;
	PR_COORD	stSrchCorner2;
	stSrchCorner1.x = GetPRWinULX();
	stSrchCorner1.y = GetPRWinULY();
	stSrchCorner2.x = GetPRWinLRX();
	stSrchCorner2.y = GetPRWinLRY();
	PR_COORD	stSrchNmlCorner1 = GetSrchArea().coCorner1;
	PR_COORD	stSrchNmlCorner2 = GetSrchArea().coCorner2;

	PR_WIN stArea;
	GetSearchDieArea(&stArea, WPR_GEN_RDIE_OFFSET + 1);
	//Use the whole FOV to search Number Die
	stSrchNmlCorner1 = stSrchCorner1;
	stSrchNmlCorner2 = stSrchCorner2;

	stSrchCorner1 = stSrchNmlCorner1;
	stSrchCorner2 = stSrchNmlCorner2;

	//Limit srch region in Vision Standard
	if (stSrchCorner1.x < GetPRWinULX())	stSrchCorner1.x = GetPRWinULX();
	if (stSrchCorner1.y < GetPRWinULY())	stSrchCorner1.y = GetPRWinULY();
	if (stSrchCorner1.x >= GetPRWinLRX())	stSrchCorner1.x = GetPRWinLRX() - 1;
	if (stSrchCorner1.y >= GetPRWinLRY())	stSrchCorner1.y = GetPRWinLRY() - 1;
	if (stSrchCorner2.x < GetPRWinULX())	stSrchCorner2.x = GetPRWinULX();
	if (stSrchCorner2.y < GetPRWinULY())	stSrchCorner2.y = GetPRWinULY();
	if (stSrchCorner2.x >= GetPRWinLRX())	stSrchCorner2.x = GetPRWinLRX() - 1;
	if (stSrchCorner2.y >= GetPRWinLRY())	stSrchCorner2.y = GetPRWinLRY() - 1;

	if (stSrchCorner2.x - stSrchCorner1.x <128) stSrchCorner2.x = stSrchCorner1.x + 128;
	if (stSrchCorner2.y - stSrchCorner1.y <128) stSrchCorner2.y = stSrchCorner1.y + 128;

	PR_InitSrchNumberDie(&stSrchNumberDieCmd);

	LONG emInfo;
	if ( m_bDebugVideoTest == TRUE )
		emInfo = PR_SHOW_DEFECT;
	else
		emInfo = PR_NO_DISPLAY;
	PR_BOOLEAN bLatch = PR_TRUE;
	GetRefDieSrchCmd(&stSrchRefCmd, WPR_REFERENCE_DIE, 1, stSrchCorner1,stSrchCorner2, bLatch, PR_TRUE, PR_FALSE, emInfo);
	stSrchNumberDieCmd.stSrchRefDieCmd = stSrchRefCmd;
	stSrchNumberDieCmd.uwNumOfChar = WPR_SRN_CDIE_CHAR_NO;//2;

	GetRefDieSrchCmd(&stSrchGoodCmd, WPR_NORMAL_DIE, 1, stSrchNmlCorner1,stSrchNmlCorner2, bLatch, PR_TRUE, PR_FALSE, emInfo);

	//Check all Char die is learnt before search (char die is started from index 2)
	for (i=WPR_GEN_CDIE_OFFSET; i<(WPR_GEN_CDIE_MAX_DIE + WPR_GEN_CDIE_OFFSET); i++)
	{
		if (IsThisDieLearnt((UCHAR)i) == TRUE)
		{
			siLearntDigit++;
		}
	}

	CString szTemp, szMsg;
	//Search for 2 digit
	for (i=0; i<WPR_SRN_CDIE_CHAR_NO; i++)
	{
		fDieScore[i] = 0;
		ucDieDigit[i] = NULL;
		lSearchDieStauts[i] = 0;

		PR_InitRgnCharCmd(&stRgnCharCmd);

		stRgnCharCmd.uwNumOfCharInfo		= (PR_UWORD)siLearntDigit;
		stRgnCharCmd.emSameView				= PR_TRUE;			
		stRgnCharCmd.rPassScore				= 85.0;
		stRgnCharCmd.emDefectInsp			= PR_FALSE;
		stRgnCharCmd.aeMaxDefectArea		= 0;
		stRgnCharCmd.emGraphicInfo			= PR_NO_DISPLAY;
		stRgnCharCmd.rStartAngle			= GetSearchStartAngle();
		stRgnCharCmd.rEndAngle				= GetSearchEndAngle();

		if (stDieDigit->x != -1 && stDieDigit->y != -1)
		{
			stRgnCharCmd.emGivenExpectedString = PR_TRUE;
			stRgnCharCmd.emRgnMode = PR_RGN_CHAR_MODE_FAST_SRCH;
		}
		else
		{
			stRgnCharCmd.emGivenExpectedString = PR_FALSE;
			stRgnCharCmd.emRgnMode = PR_RGN_CHAR_MODE_FULL_SRCH;
		}

		PR_WORD lX1 = (GetDieSizePixelX(WPR_GEN_CDIE_OFFSET) / 2) + WPR_SRN_CDIE_OFFSET_X;
		PR_WORD lX2 = (GetDieSizePixelX(WPR_GEN_CDIE_OFFSET) / 2) + WPR_SRN_CDIE_OFFSET_X;
		PR_WORD lY1 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET) + WPR_SRN_CDIE_OFFSET_Y;
		PR_WORD lY2 = WPR_SRN_CDIE_OFFSET_Y;

		switch( m_ulOcrDieOrientation )
		{
		case 0:
			if( i==0 )
			{
				lX1 = GetDieSizePixelX(WPR_GEN_CDIE_OFFSET)*3/2;
				lX2 = WPR_SRN_CDIE_OFFSET_X;
			}
			else
			{
				lX1 = WPR_SRN_CDIE_OFFSET_X;
				lX2 = GetDieSizePixelX(WPR_GEN_CDIE_OFFSET)*3/2;
			}
			lY1 = lY2 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)*5/6;
			break;
		case 1:
			lX1 = lX2 = GetDieSizePixelX(WPR_GEN_CDIE_OFFSET)*5/6;
			if( i==0 )
			{
				lY1 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)*4/3;
				lY2 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)/3;
			}
			else
			{
				lY1 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)/3;
				lY2 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)*4/3;
			}
			break;

		case 2:
			if( i==1 )
			{
				lX1 = GetDieSizePixelX(WPR_GEN_CDIE_OFFSET)*3/2;
				lX2 = WPR_SRN_CDIE_OFFSET_X;
			}
			else
			{
				lX1 = WPR_SRN_CDIE_OFFSET_X;
				lX2 = GetDieSizePixelX(WPR_GEN_CDIE_OFFSET)*3/2;
			}
			lY1 = lY2 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)*5/6;
			break;
		case 3:
			lX1 = lX2 = GetDieSizePixelX(WPR_GEN_CDIE_OFFSET)*5/6;
			if( i==1 )
			{
				lY1 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)*4/3;
				lY2 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)/3;
			}
			else
			{
				lY1 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)/3;
				lY2 = GetDieSizePixelY(WPR_GEN_CDIE_OFFSET)*4/3;
			}
			break;
		}
		stRgnCharCmd.stRgnWin.coCorner1.x	= stDieCenter.x - lX1;
		stRgnCharCmd.stRgnWin.coCorner1.y	= stDieCenter.y - lY1;
		stRgnCharCmd.stRgnWin.coCorner2.x	= stDieCenter.x + lX2;
		stRgnCharCmd.stRgnWin.coCorner2.y	= stDieCenter.y + lY2;

		if( i==0 )
			stRgnCharCmd.aubExpectedString[0] = '0' + stDieDigit->x;
		else
			stRgnCharCmd.aubExpectedString[0] = '0' + stDieDigit->y;

		szMsg.Format("Char Info %d, pass score 85, RgnMode %d, search area %d,%d  %d,%d expectedstring %c",
			siLearntDigit, stRgnCharCmd.emRgnMode, lX1, lY1, lX2, lY2, stRgnCharCmd.aubExpectedString[0]);

		for (j=0; j<siLearntDigit; j++)
		{
			memset(stRgnCharCmd.astCharInfo[j].aubString,  NULL, sizeof(stRgnCharCmd.astCharInfo[j].aubString));
			stRgnCharCmd.astCharInfo[j].aubString[0] =  j + WPR_GEN_CDIE_START;
			stRgnCharCmd.astCharInfo[j].uwRecordID	 = m_ssGenPRSrchID[j+WPR_GEN_CDIE_OFFSET];   
			szTemp.Format(" String %d, RecordID %d",
				stRgnCharCmd.astCharInfo[j].aubString[0], stRgnCharCmd.astCharInfo[j].uwRecordID);
			szMsg += szTemp;
		}

		SetAlarmLog(szMsg);
		stSrchNumberDieCmd.astRgnCharCmd[i] = stRgnCharCmd;
	}
	stSrchNumberDieCmd.coRefDieCenter = stDieCenter;

	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();

	PR_SrchNumberDieExt(&stSrchNumberDieCmd, ubSID, ubRID, &stSrchNumberDieRpy1);
	if( (stSrchNumberDieRpy1.uwCommunStatus != PR_COMM_NOERR) || (stSrchNumberDieRpy1.uwPRStatus != PR_ERR_NOERR) )
	{
		stDieDigit->x = -2;
		stDieDigit->y = -2;
		m_szScanResult.Format("Srch Number die rpy1 comm %d(%d); %d(%d)",
			stSrchNumberDieRpy1.uwCommunStatus, PR_COMM_NOERR,
			stSrchNumberDieRpy1.uwPRStatus, PR_ERR_NOERR);
		SetAlarmLog(m_szScanResult);
		return FALSE;
	}
  
    PR_SrchNumberDieExtRpy(ubSID, &stSrchNumberDieRpy2);
	if (stSrchNumberDieRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		stDieDigit->x = -2;
		stDieDigit->y = -2;
		m_szScanResult.Format("Srch Number die rpy2 comm %d(%d)",
			stSrchNumberDieRpy2.stStatus.uwCommunStatus, PR_COMM_NOERR);
		SetAlarmLog(m_szScanResult);
		return FALSE;
	}

	//Check result
	if ( stSrchNumberDieRpy2.stStatus.uwPRStatus == PR_ERR_NOERR )	
	{
		*fDieRotate = stSrchNumberDieRpy2.rDieRot;
		lReturn = TRUE;//PR_ERR_GOOD_DIE;
		if (stSrchNumberDieRpy2.aaubString[0] == NULL)
			stDieDigit->x = -2;
		else
			stDieDigit->x = stSrchNumberDieRpy2.aaubString[0][0] - '0';
		if (stSrchNumberDieRpy2.aaubString[1] == NULL)
			stDieDigit->y = -2;
		else
			stDieDigit->y = stSrchNumberDieRpy2.aaubString[1][0] - '0';
	}
	else
	{
		m_szScanResult.Format("Srch Number die rpy2 status %d(%d)",
			stSrchNumberDieRpy2.stStatus.uwPRStatus, PR_ERR_NOERR);
		lReturn = FALSE;//PR_ERR_NO_CHAR_FOUND;
		stDieDigit->x = -2;
		stDieDigit->y = -2;
		SetAlarmLog(m_szScanResult);
	}

	//Add digital score
	stDieScore->x = stSrchNumberDieRpy2.arCharScore[0];
	stDieScore->y = stSrchNumberDieRpy2.arCharScore[1];

	//Check result
	stDieOffset->x = stSrchNumberDieRpy2.coCentre.x;
	stDieOffset->y = stSrchNumberDieRpy2.coCentre.y;

	return lReturn;
}


BOOL CWaferPr::CounterCheckReferenceDieWithNormalDie()
{
	if ( PR_NotInit() )
	{
		return TRUE;
	}

	if (CMS896AStn::m_bCounterCheckWithNormalDieRecord ==  FALSE)
	{
		return TRUE;
	}

	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	PR_WORD		wResult = 0;

	DrawSearchBox(PR_COLOR_GREEN);
	
	wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
	
	if (wResult != -1)
	{
		if (DieIsGood(usDieType) == TRUE)
		{
			return FALSE;
		}
	}
	
	return TRUE; 	
}


PR_UWORD CWaferPr::CG_SearchDieCmd()
{
	PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_WIN stSrchArea;
	GetSearchDieArea(&stSrchArea, 1 + WPR_GEN_RDIE_OFFSET);
	//  Search Die Cmd during sort mode to get CG center of a partial die
	return	SearchDieCmd(WPR_REFERENCE_DIE, 1, ubSID, ubRID, stSrchArea.coCorner1, stSrchArea.coCorner2, PR_TRUE, PR_TRUE, PR_FALSE, PR_NO_DISPLAY);
}

BOOL CWaferPr::IndexAndFovSearchReferDie(int *siPosX, int *siPosY, LONG lDirection)
{
	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	LONG		lRefNo = 1;
	LONG		lDiff_X = 0;
	LONG		lDiff_Y = 0;
	int			siOrigT = 0;

	PR_COORD	stSrchCorner1;
	PR_COORD	stSrchCorner2;
	stSrchCorner1.x = GetPRWinULX();
	stSrchCorner1.y = GetPRWinULY();
	stSrchCorner2.x = GetPRWinLRX();
	stSrchCorner2.y = GetPRWinLRY();


	LONG lJumpRow = GetSortFovRow();
	LONG lJumpCol = GetSortFovCol();

	switch (lDirection)
	{
	case WPR_RT_DIE:		//Right
		lDiff_X = lJumpCol;
		break;

	case WPR_UP_DIE:		//Up
		lDiff_Y = -lJumpRow;
		break;

	case WPR_DN_DIE:		//Down
		lDiff_Y = lJumpRow;
		break;

	case WPR_LT_DIE:		//Left
		lDiff_X = -lJumpCol;
		break;

	default:	//Center die
		break;
	}
//	CMSLogFileUtility::Instance()->WL_LogStatus("WPR Fov Refer Srch Move Table");
	//Move table
	*siPosX = *siPosX - lDiff_X * GetDiePitchXX() - lDiff_Y * GetDiePitchYX();
	*siPosY = *siPosY - lDiff_Y * GetDiePitchYY() - lDiff_X * GetDiePitchXY();

	MoveWaferTable(*siPosX, *siPosY);

//	CMSLogFileUtility::Instance()->WL_LogStatus("WPR Fov Refer Srch to find refer die");
	//Search reference die with user defined search region
	if( ManualSearchDie(WPR_REFERENCE_DIE, lRefNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2) == -1 )
	{
		return FALSE;
	}

	if (DieIsGood(usDieType) == TRUE)
	{
//		CMSLogFileUtility::Instance()->WL_LogStatus("WPR Fov Refer Srch compensation");
		ManualDieCompenate(stDieOffset, fDieRotate);
//		CMSLogFileUtility::Instance()->WL_LogStatus("WPR Fov Refer Srch get position");
		GetWaferTableEncoder(siPosX, siPosY, &siOrigT);

		PR_COORD	stSrchUL = GetSrchArea().coCorner1;
		PR_COORD	stSrchLR = GetSrchArea().coCorner2;
		if( m_bHomeDieUniqueCheck )	// check normal die 1 and all other refer die>1
		{
			PR_UWORD	uwType;
			PR_COORD	stOffset; 
			PR_REAL		fRotate, fScore; 
			CString szMsg;
			PR_WORD wReturn = 0;

			wReturn =    ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &uwType, &fRotate, &stOffset, &fScore, stSrchUL, stSrchLR);
			if( wReturn==-1 )
			{
				szMsg.Format("PR ERROR to search Normal die at Home die position!");
				HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
				SetErrorMessage(szMsg);
				m_bHomeDieUniqueResult = FALSE;
				return TRUE;
			}
			if( DieIsGood(uwType) )
			{
				szMsg.Format("Find Normal die at Home die position!");
				HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
				SetErrorMessage(szMsg);
				m_bHomeDieUniqueResult = FALSE;
				return TRUE;
			}

			for(int i=1; i<m_lLrnTotalRefDie; i++)
			{
				wReturn = ManualSearchDie(WPR_REFERENCE_DIE, WPR_REFERENCE_DIE+i, PR_TRUE, PR_TRUE, PR_TRUE, &uwType, &fRotate, &stOffset, &fScore, stSrchUL, stSrchLR);
				if( wReturn==-1 )
				{
					szMsg.Format("Search refer die PR error at Home die position");
					HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
					SetErrorMessage(szMsg);
					m_bHomeDieUniqueResult = FALSE;
					return TRUE;
				}

				if( DieIsGood(uwType) )
				{
					szMsg.Format("Find refer %d die at Home die position", WPR_REFERENCE_DIE+i);
					HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
					SetErrorMessage(szMsg);
					m_bHomeDieUniqueResult = FALSE;
					return TRUE;
				}
			}
		}
		if( m_bCompareHomeReferDieScore )
		{
			PR_UWORD	usDieType1;
			PR_COORD	stDieOffset1; 
			PR_REAL		fDieRotate1; 
			PR_REAL		fDieScore1;
			CString szMsg;
			if( ManualSearchDie(WPR_REFERENCE_DIE, lRefNo+1, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType1, &fDieRotate1, &stDieOffset1, &fDieScore1, GetSrchArea().coCorner1, GetSrchArea().coCorner2)!=-1 )
			{
				if( DieIsGood(usDieType1) )
				{
					if( fDieScore<=fDieScore1 )
					{
						szMsg.Format("Home die score (%.2f) is not larger than refer die score (%.2f)",
							fDieScore, fDieScore1);
						HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
						SetErrorMessage(szMsg);
						m_bHomeDieUniqueResult = FALSE;
						return TRUE;
					}
					else
					{
						szMsg.Format("Home die score (%.2f) is larger than refer die score (%.2f)",
							fDieScore, fDieScore1);
						SetErrorMessage(szMsg);
					}
				}
			}
			else
			{
				szMsg.Format("Search refer die PR error at Home die position");
				HmiMessage_Red_Back(szMsg, "Wafer Auto Alignment");
				SetErrorMessage(szMsg);
				m_bHomeDieUniqueResult = FALSE;
				return TRUE;
			}
		}

		return TRUE;
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("WPR Fov Refer Srch to find a normal die");
	//If no ref-die found, then try to locate a normal die at center for alignment purpose only
	PR_WORD wResult = 0;
	wResult =    ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

	if( wResult == -1 )
	{
		return FALSE;
	}

	if( DieIsAlignable(usDieType) )
	{
		int  nOffsetX, nOffsetY;
		CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);

		CMSLogFileUtility::Instance()->WL_LogStatus("WPR Fov Refer Srch to locate center");
		BOOL bLocateDie = FALSE;
		LONG lDiePitchX_X = GetDiePitchXX();
		LONG lDiePitchX_Y = GetDiePitchXY();
		LONG lDiePitchY_Y = GetDiePitchYY();
		LONG lDiePitchY_X = GetDiePitchYX();
		LONG lGrabOffsetX = 0, lGrabOffsetY = 0;
		if( lJumpCol%2==0 )
			lGrabOffsetX = lDiePitchX_X/2;
		if( lJumpRow%2==0 )
			lGrabOffsetY = lDiePitchY_Y/2;

		LONG lFindDieX = nOffsetX+lGrabOffsetX, lLoopDieX = 0;
		LONG lFindDieY = nOffsetY+lGrabOffsetY, lLoopDieY = 0;

		for(LONG iRow=0; iRow<=40; iRow++)
		{
			for(LONG jCol=0; jCol<=40; jCol++)
			{
				lLoopDieX = 0 - (jCol-20) * lDiePitchX_X - (iRow-20) * lDiePitchY_X;
				lLoopDieY = 0 - (iRow-20) * lDiePitchY_Y - (jCol-20) * lDiePitchX_Y;
				if( abs(lFindDieX-lLoopDieX)<abs(m_nDieSizeX/2) && 
					abs(lFindDieY-lLoopDieY)<abs(m_nDieSizeY/2) )
				{
					bLocateDie = TRUE;
					break;
				}
			}
			if( bLocateDie )
			{
				break;
			}
		}
		if( bLocateDie )
		{
			*siPosX = *siPosX + lFindDieX - lLoopDieX;
			*siPosY = *siPosY + lFindDieY - lLoopDieY;
		}
	}

	return FALSE;
}

BOOL CWaferPr::SpiralSearchNormalDie(LONG &lNewAlignMode)
{
	PR_COORD	stSrchCorner1;
	PR_COORD	stSrchCorner2;
	stSrchCorner1.x = GetPRWinULX();
	stSrchCorner1.y = GetPRWinULY();
	stSrchCorner2.x = GetPRWinLRX();
	stSrchCorner2.y = GetPRWinLRY();
	int			siOrigX;
	int			siOrigY;
	int			siOrigT;
	LONG		lUpIndex = 0;
	LONG		lDnIndex = 0;
	LONG		lLtIndex = 0;
	LONG		lRtIndex = 0;
	LONG		lMaxLoopCount = 0;
	LONG		lRow, lCol;
	LONG		lCurrentLoop = 1;
	LONG		lCurrentIndex = 0;
	LONG		lTotalIndex = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CMS896AApp::m_bStopAlign = FALSE;
	lMaxLoopCount = m_lSprialSize + 1;

	PrescanNormalInit(GetPrescanPrID(), FALSE);

	//Get current position
	GetWaferTableEncoder(&siOrigX, &siOrigY, &siOrigT);

	//Search from current position	
	if (IndexAndFovSearchNormalDie(&siOrigX, &siOrigY, WPR_CT_DIE, lNewAlignMode) == TRUE)
	{
		return TRUE;
	}

	lTotalIndex = 0;
	for(lCurrentLoop=1; lCurrentLoop<=lMaxLoopCount; lCurrentLoop++)
	{
		lRow = (lCurrentLoop*2 + 1);
		lCol = (lCurrentLoop*2 + 1);

		//Move to RIGHT & PR search on current die
		lTotalIndex++;
		lCurrentIndex = 1;

		if (IndexAndFovSearchNormalDie(&siOrigX, &siOrigY, WPR_RT_DIE, lNewAlignMode) == TRUE)
		{
			return TRUE;
		}
		Sleep(10);
		if( pApp->IsStopAlign() )
		{
			return FALSE;
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			m_comServer.ProcessRequest();

			MSG Msg; 
			if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
				::DispatchMessage(&Msg);
			}
			if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
				(Action() == glABORT_STOP)))
			{
				return FALSE;
			}

			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;
			lTotalIndex++;
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}

			if (IndexAndFovSearchNormalDie(&siOrigX, &siOrigY, WPR_UP_DIE, lNewAlignMode) == TRUE)
			{
				return TRUE;
			}	
			Sleep(10);
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}			

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			m_comServer.ProcessRequest();

			MSG Msg; 
			if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
				::DispatchMessage(&Msg);
			}
			if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
				(Action() == glABORT_STOP)))
			{
				return FALSE;
			}

			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;
			lTotalIndex++;
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}

			if (IndexAndFovSearchNormalDie(&siOrigX, &siOrigY, WPR_LT_DIE, lNewAlignMode) == TRUE)
			{
				return TRUE;
			}	
			Sleep(10);
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			m_comServer.ProcessRequest();

			MSG Msg; 
			if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
				::DispatchMessage(&Msg);
			}
			if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
				(Action() == glABORT_STOP)))
			{
				return FALSE;
			}

			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;
			lTotalIndex++;
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}

			if (IndexAndFovSearchNormalDie(&siOrigX, &siOrigY, WPR_DN_DIE, lNewAlignMode) == TRUE)
			{
				return TRUE;
			}	
			Sleep(10);
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}


		//Move to DOWN & PR search on current die
		lRtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			m_comServer.ProcessRequest();

			MSG Msg; 
			if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
				::DispatchMessage(&Msg);
			}
			if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
				(Action() == glABORT_STOP)))
			{
				return FALSE;
			}

			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;
			lTotalIndex++;
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}

			if (IndexAndFovSearchNormalDie(&siOrigX, &siOrigY, WPR_RT_DIE, lNewAlignMode) == TRUE)
			{
				return TRUE;
			}
			Sleep(10);
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL CWaferPr::IndexAndFovSearchNormalDie(int *siPosX, int *siPosY, LONG lDirection, LONG &lNewAlignMode)
{
	PR_COORD	stSrchCorner1;
	PR_COORD	stSrchCorner2;
	PR_UWORD	usDieType;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;

	stSrchCorner1.x = GetPRWinULX();
	stSrchCorner1.y = GetPRWinULY();
	stSrchCorner2.x = GetPRWinLRX();
	stSrchCorner2.y = GetPRWinLRY();
	LONG		lDiff_X = 0;
	LONG		lDiff_Y = 0;
	int			siOrigT = 0;

	DOUBLE dLFSize = 1; 

	if ( IsLFSizeOK() )
	{
		dLFSize = 3;
	}

	BOOL bMove = TRUE;
	switch (lDirection)
	{
	case 1:		//Right
		lDiff_X = (LONG) dLFSize;
		break;

	case 2:		//Up
		lDiff_Y = -(LONG) dLFSize;
		break;

	case 3:		//Down
		lDiff_Y = (LONG) dLFSize;
		break;

	case 0:		//Left
		lDiff_X = -(LONG) dLFSize;
		break;

	default:	//Center die
		bMove = FALSE;
		lDiff_X = 0;
		lDiff_Y = 0;
		break;
	}


	//Move table
	*siPosX = *siPosX - lDiff_X * GetDiePitchXX() - lDiff_Y * GetDiePitchYX();
	*siPosY = *siPosY - lDiff_Y * GetDiePitchYY() - lDiff_X * GetDiePitchXY();

	if( bMove )
	{
		MoveWaferTable(*siPosX, *siPosY);
	}

	CString szMsg;
	szMsg.Format("Fov Srch normal die at %ld,%ld", *siPosX, *siPosY);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	//If no ref-die found, then try to locate a normal die at center for alignment purpose only
	PR_WORD wResult = 0;
	if( m_bFovToFindGT==FALSE )
	{
		wResult =    ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSrchCorner1, stSrchCorner2);

		if (wResult == -1)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("FOV search normal die PR  error");
			return FALSE;
		}

		if (DieIsAlignable(usDieType) != TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("no die found");

			return FALSE;
		}

		ManualDieCompenate(stDieOffset, fDieRotate);
		Sleep(10);
		GetWaferTableEncoder(siPosX, siPosY, &siOrigT);

		if( lNewAlignMode==-1 )
		{
			return TRUE;
		}

		if( dLFSize<3 )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("find a die and FOV < 3");
			return TRUE;
		}

		return TRUE;
	}

	// below fov find gt is true case
	if( AutoLearnGTByDieAngle(*siPosX, *siPosY)==FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("auto learn wafer GT by die angle error");
		return FALSE;
	}

	if( m_bFovFindGTMove )
	{
		GetWaferTableEncoder(siPosX, siPosY, &siOrigT);
		if( AutoLearnWaferAngle(*siPosX, *siPosY, lNewAlignMode)==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("auto learn wafer GT MOVE error");
			return FALSE;
		}
		Sleep(10);

		SaveScanTimeEvent("WPR: FOV to find GT, confirm die position");
		wResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2);
		if( (wResult == -1) || (!DieIsAlignable(usDieType)) )
		{
			CString szErr;
			szErr.Format("WPR align global theta angle fails at CENTER-die (DieType = %0x%x)", usDieType);
			CMSLogFileUtility::Instance()->WL_LogStatus(szErr);
			return FALSE;
		}

		ManualDieCompenate(stDieOffset, fDieRotate);
		GetWaferTableEncoder(siPosX, siPosY, &siOrigT);
	}

	m_bFovFoundWaferAngle = TRUE;

	return TRUE;
}

BOOL CWaferPr::AutoLearnWaferAngle(LONG lPosX, LONG lPosY, LONG &lNewAlignMode)
{
	ULONG i, j;
	CDWordArray dwList;

	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
	}

	ClearGoodInfo();

	if( PrescanAutoMultiGrabDone(dwList, FALSE, TRUE)!=TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("multi srch error");
		return FALSE;
	}

	if( GetGoodTotalDie()==0 )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("no die found");
		return FALSE;
	}

	ULONG lFovDieCount = GetMapIndexStepRow()*GetMapIndexStepCol();
	DOUBLE dDieAngle = 0, dCheckAngle = 0;
	LONG lBaseX, lBaseY, lCheckX, lCheckY;
	LONG lOffsetX = 0, lOffsetY = 0;
	CString szMsg;
	szMsg.Format("multi srch die found %lu, FOV Max %ld at(%ld,%ld)", GetGoodTotalDie(), lFovDieCount, lPosX, lPosY);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	if( lFovDieCount==1 && GetGoodTotalDie()==1 )
	{
		if( GetGoodPosnOnly(1, lOffsetX, lOffsetY, dDieAngle) )
		{
			lPosX += lOffsetX;
			lPosY += lOffsetY;
			WprCompensateRotateDie(lPosX, lPosY, dDieAngle);
			szMsg.Format("Multi srch single rotate die %ld,%ld(angle %f)", lPosX, lPosY, dDieAngle);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			return TRUE;
		}
	}

	LONG lVertNum = 1, lHoriNum = 1, lVertMax = 1, lHoriMax = 1, lHoriDie = 1, lVertDie = 1;
	LONG lMostNum = 1, lLessNum = 1, lMostMax = 1, lLessMax = 1, lMostDie = 1;
	for(i=1; i<=GetGoodTotalDie(); i++)
	{
		if( GetGoodPosnOnly(i, lBaseX, lBaseY, dDieAngle) )
		{
			lVertNum = lHoriNum = 1;
			lMostNum = lLessNum = 0;
			for(j=1; j<=GetGoodTotalDie(); j++)
			{
				if( j==i )
					continue;
				if( GetGoodPosnOnly(j, lCheckX, lCheckY, dCheckAngle) )
				{
					if( labs(lCheckX-lBaseX)<labs(m_nDieSizeX/2) )
					{
						lVertNum++;
					}
					if( labs(lCheckY-lBaseY)<labs(m_nDieSizeY/2) )
					{
						lHoriNum++;
					}
					if( fabs(dCheckAngle-dDieAngle)<=1.0 )
					{
						lMostNum++;
					}
					else
					{
						lLessNum++;
					}
				}
			}
			if( lVertMax<lVertNum )
			{
				lVertMax = lVertNum;
				lVertDie = i;
			}
			if( lHoriMax<lHoriNum )
			{
				lHoriMax = lHoriNum;
				lHoriDie = i;
			}
			if( lMostMax<lMostNum )
			{
				lMostMax = lMostNum;
				lMostDie = i;
			}
			if( lLessMax<lLessNum )
			{
				lLessMax = lLessNum;
			}
			szMsg.Format("Multi srch die info %2ld(%6ld,%6ld)(angle %3.4f) => V %2ld H %2ld M %2ld L %2ld",
				i, lBaseX, lBaseY, dDieAngle, lVertNum, lHoriNum, lMostNum, lLessNum);
		//	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		}
	}

	szMsg.Format("Multi srch die size (%ld,%ld) result => V %ld(%ld) H %ld(%ld) M %ld(%ld) L %ld",
		m_nDieSizeX, m_nDieSizeY, lVertMax, lVertDie, lHoriMax, lHoriDie, lMostMax, lMostDie, lLessMax);
	SaveScanTimeEvent(szMsg);

	if( lHoriMax>1 || lVertMax>1 )
	{
		int	siLHSDieX = 0, siLHSDieY = 0, siRHSDieX = 0,siRHSDieY = 0 ;
		if( lHoriMax<lVertMax )
		{
			lNewAlignMode = 1;
			GetGoodPosnOnly(lVertDie, lBaseX, lBaseY, dDieAngle);
			siLHSDieX = siRHSDieX = lOffsetX = lBaseX;
			siLHSDieY = siRHSDieY = lOffsetY = lBaseY;
			for(i=1; i<=GetGoodTotalDie(); i++)
			{
				if( i==lVertDie )
					continue;
				if( GetGoodPosnOnly(i, lCheckX, lCheckY, dDieAngle) )
				{
					if( labs(lBaseX-lCheckX)<labs(m_nDieSizeX/2) )
					{
						if( siLHSDieY<lCheckY )
						{
							siLHSDieY = lCheckY;
							siLHSDieX = lCheckX;
						}
						if( siRHSDieY>lCheckY )
						{
							siRHSDieY = lCheckY;
							siRHSDieX = lCheckX;
						}
					}
				}
			}
			//Calcuate wafer angle	
			DOUBLE dDiffY = (double)(siLHSDieY - siRHSDieY);
			DOUBLE dDiffX = (double)(siLHSDieX - siRHSDieX);
			dDieAngle = atan2(dDiffX * -1.00, dDiffY) * (180 / PI);
		}
		else
		{
			lNewAlignMode = 0;
			GetGoodPosnOnly(lHoriDie, lBaseX, lBaseY, dDieAngle);
			siLHSDieX = siRHSDieX = lOffsetX = lBaseX;
			siLHSDieY = siRHSDieY = lOffsetY = lBaseY;
			for(i=1; i<=GetGoodTotalDie(); i++)
			{
				if( i==lHoriDie )
					continue;
				if( GetGoodPosnOnly(i, lCheckX, lCheckY, dDieAngle) )
				{
					if( labs(lBaseY-lCheckY)<labs(m_nDieSizeY/2) )
					{
						if( siLHSDieX<lCheckX )
						{
							siLHSDieY = lCheckY;
							siLHSDieX = lCheckX;
						}
						if( siRHSDieX>lCheckX )
						{
							siRHSDieY = lCheckY;
							siRHSDieX = lCheckX;
						}
					}
				}
			}
			//Calcuate wafer angle	
			DOUBLE dDiffY = (double)(siLHSDieY - siRHSDieY);
			DOUBLE dDiffX = (double)(siLHSDieX - siRHSDieX);
			dDieAngle = atan2(dDiffY , dDiffX) * (180 / PI);
		}
		lPosX += lOffsetX;
		lPosY += lOffsetY;
		WprCompensateRotateDie(lPosX, lPosY, dDieAngle);
		szMsg.Format("Multi srch rotate die %ld,%ld(angle %f) vert %d by (%ld,%ld) to (%ld,%ld)",
			lPosX, lPosY, dDieAngle, lNewAlignMode, siLHSDieX, siLHSDieY, siRHSDieX,siRHSDieY);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		return TRUE;
	}
	else
	{
		if( GetGoodTotalDie()>=3 )
		{
			if( lMostMax>lLessMax )
			{
				if( GetGoodPosnOnly(lMostDie, lOffsetX, lOffsetY, dDieAngle) )
				{
					lPosX += lOffsetX;
					lPosY += lOffsetY;
					WprCompensateRotateDie(lPosX, lPosY, dDieAngle);
					szMsg.Format("Multi srch common angle die %ld,%ld(angle %f) by die %ld", lPosX, lPosY, dDieAngle, lMostDie);
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


BOOL CWaferPr::AutoLearnGTByDieAngle(LONG lPosX, LONG lPosY)
{
	ULONG i;
	CDWordArray dwList;

	for(i=0; i<5; i++)
	{
		dwList.SetSize(i+1);
		dwList.SetAt(i, 0);
	}
	SaveScanTimeEvent("WPR: to learn GT by all dice angle");
	ClearGoodInfo();

	if( PrescanAutoMultiGrabDone(dwList, FALSE, TRUE)!=TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("multi srch error");
		return FALSE;
	}

	ULONG ulGoodTotalDie = GetGoodTotalDie();
	if( ulGoodTotalDie==0 )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("no die found");
		return FALSE;
	}

	ULONG lFovDieCount = GetMapIndexStepRow()*GetMapIndexStepCol();
	DOUBLE dDieAngle = 0, dRotAngle = 0;
	LONG lOffsetX = 0, lOffsetY = 0;
	CString szMsg;
	szMsg.Format("multi srch die found %lu, FOV Max %ld at(%ld,%ld)", ulGoodTotalDie, lFovDieCount, lPosX, lPosY);
	SaveScanTimeEvent(szMsg);
	if( lFovDieCount==1 && ulGoodTotalDie==1 )
	{
		if( GetGoodPosnOnly(1, lOffsetX, lOffsetY, dDieAngle) )
		{
			lPosX += lOffsetX;
			lPosY += lOffsetY;
			WprCompensateRotateDie(lPosX, lPosY, dDieAngle);
			szMsg.Format("Multi srch single rotate die %ld,%ld(angle %f)", lPosX, lPosY, dDieAngle);
			SaveScanTimeEvent(szMsg);
			return TRUE;
		}
	}

	if( ulGoodTotalDie<3 )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Good die less than 3 by die angle");
		return FALSE;
	}

	LONG lNearDist = 99999, lDist = 0;
	DOUBLE dMaxAngle = -360, dMinAngle = 360;
	ULONG ulNearDie = 1, ulMaxRotDie = 1, ulMinRotDie = 1;

	for(i=1; i<=ulGoodTotalDie; i++)
	{
		if( GetGoodPosnOnly(i, lOffsetX, lOffsetY, dDieAngle) )
		{
			DOUBLE dOffsetX = lOffsetX;
			DOUBLE dOffsetY = lOffsetY;
			lDist = (LONG)(sqrt((dOffsetX*dOffsetX) + (dOffsetY*dOffsetY)));
			if( lDist<lNearDist)
			{
				lNearDist = lDist;
				ulNearDie = i;
			}
			if( dDieAngle>dMaxAngle )
			{
				dMaxAngle = dDieAngle;
				ulMaxRotDie = i;
			}
			if( dDieAngle<dMinAngle )
			{
				dMinAngle = dDieAngle;
				ulMinRotDie = i;
			}

			szMsg.Format("Multi srch die info %2ld(%6ld,%6ld)(angle %3.4f)",
				i, lOffsetX, lOffsetY, dDieAngle);
		//	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		}
	}

	szMsg.Format("Multi srch die size (%ld,%ld) near %ld max angle %3.4f(%ld) min angle %3.4f(%ld)",
		m_nDieSizeX, m_nDieSizeY, ulNearDie, dMaxAngle, ulMaxRotDie, dMinAngle, ulMinRotDie);
	SaveScanTimeEvent(szMsg);

	for(i=1; i<=ulGoodTotalDie; i++)
	{
		if( i==ulMaxRotDie || i==ulMinRotDie )
			continue;
		if( GetGoodPosnOnly(i, lOffsetX, lOffsetY, dDieAngle) )
		{
			dRotAngle += dDieAngle;
		}
	}

	if( ulMaxRotDie!=ulMinRotDie )
	{
		dRotAngle = dRotAngle/((DOUBLE)(ulGoodTotalDie-2));
	}
	else
	{
		dRotAngle = dRotAngle/((DOUBLE)(ulGoodTotalDie-1));
	}

	if( GetGoodPosnOnly(ulNearDie, lOffsetX, lOffsetY, dDieAngle) )
	{
		lPosX += lOffsetX;
		lPosY += lOffsetY;
		WprCompensateRotateDie(lPosX, lPosY, dRotAngle);
		szMsg.Format("Multi srch common angle die %ld,%ld(angle %f) by die %ld", lPosX, lPosY, dRotAngle, ulNearDie);
		SaveScanTimeEvent(szMsg);
		return TRUE;
	}

	return FALSE;
}

LONG CWaferPr::GetSortFovRow()
{
	if( IsEnableZoom() )
	{
		DOUBLE dRowTol = 0, dColTol = 0;
		GetMapIndexTolerance(dRowTol, dColTol);
		LONG	nFovIndexStepRow = (LONG)(m_dLFSizeY - dRowTol);
		if( nFovIndexStepRow%2==0 )
			nFovIndexStepRow = nFovIndexStepRow - 1;
		if( nFovIndexStepRow<1 )
			nFovIndexStepRow = 1;
		return nFovIndexStepRow;
	}

	return GetMapIndexStepRow();
}

LONG CWaferPr::GetSortFovCol()
{
	if( IsEnableZoom() )
	{
		DOUBLE dRowTol = 0, dColTol = 0;
		GetMapIndexTolerance(dRowTol, dColTol);
		LONG	nFovIndexStepCol = (LONG)(m_dLFSizeX - dColTol);
		if( nFovIndexStepCol%2==0 )
			nFovIndexStepCol = nFovIndexStepCol - 1;
		if( nFovIndexStepCol<1 )
			nFovIndexStepCol = 1;
		return nFovIndexStepCol;
	}

	return GetMapIndexStepCol();
}

//4.52D17Manual search uplook fnc
PR_WORD CWaferPr::ManualUpLookSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
								  PR_COORD stCorner1, PR_COORD stCorner2, BOOL bLog)
{
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	CString				csMsgA;
	CString				csMsgB;
	PR_UWORD			usSearchResult;
    PR_COORD			stPRCoorCentre;
    PR_COORD			stPRDieSize;
	BOOL				bGetRpy3 = FALSE;
	PR_GRAPHIC_INFO		emInfo;

	stPRCoorCentre.x  = (PR_WORD)GetPrCenterX();
	stPRCoorCentre.y  = (PR_WORD)GetPrCenterY();

	m_dCurSrchDieSizeX	= 0;
	m_dCurSrchDieSizeY	= 0;

	if ( m_bDebugVideoTest == TRUE )
		emInfo = PR_SHOW_DEFECT;
	else
		emInfo = PR_NO_DISPLAY;

	if( IsBurnIn() && (m_bNoWprBurnIn ||IsAOIOnlyMachine()) )
	{
        *usDieType = PR_ERR_NOERR;
		*fDieRotate = 0.0;
		stDieOffset->x = 0;
		stDieOffset->y = 0;
		*fDieScore = 0;
		return 0;
	}

	//PR_UBYTE ubSID = GetRunSenID();	PR_UBYTE ubRID = GetRunRecID();
	PR_UBYTE ubSID = PSPR_SENDER_ID;
	PR_UBYTE ubRID = PSPR_RECV_ID;

	FLOAT fOrientation = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
/*
	if( m_bCheckDieOrientation)
	{
		fOrientation = (FLOAT) (pUtl->GetSearchDieRotation());
	}
*/
	fOrientation = 9;	//disable 180 degree rotation for ALL uplook PR //v4.59A36
	
	usSearchResult = SearchDieCmd(bDieType, lDieNo, ubSID, ubRID, stCorner1, stCorner2, bLatch, bAlign, bInspect, emInfo, fOrientation);
//	pUtl->SetSearchDieRoation(0);
	if (usSearchResult != PR_ERR_NOERR)
	{
		if (usSearchResult == IDS_WPR_DIENOTLEARNT)
		{
			csMsgA.Format("WPR Not Learnt");
		}
		else
		{
			csMsgA.Format("Send Command = %x",usSearchResult);
		}
		SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);

		return -1;
	}

	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR)
	{
		csMsgA.Format("Get Reply 1 = %x",usSearchResult);
		SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		return -1;
	}

	usSearchResult = SearchDieRpy2(ubSID, &stSrchRpy2);
	if (usSearchResult == PR_COMM_ERR)
	{
		csMsgA.Format("Get Reply 2 = %x",usSearchResult);
		SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
		return -1;
	}

    *usDieType = stSrchRpy2.stStatus.uwPRStatus;
	CString szTempLog;
	szTempLog.Format("Uplook --- Manual Search stSrchRpy2.stStatus.uwPRStatus:%x", stSrchRpy2.stStatus.uwPRStatus);
	CMSLogFileUtility::Instance()->MS_LogOperation(szTempLog);

	if (*usDieType == PR_ERR_NOERR)
	{
        *usDieType = PR_ERR_GOOD_DIE;
    }

	if ((DieIsAlignable(*usDieType) == TRUE) && (bInspect == TRUE))
	{
		bGetRpy3 = TRUE;
		usSearchResult = SearchDieRpy3(ubSID, &stSrchRpy3);
		if (usSearchResult == PR_COMM_ERR)
		{
			csMsgA.Format("Get Reply 3 = %x",usSearchResult);
			SetAlert_Msg_Red_Back(IDS_WPR_SRN_DIE_FAILED, csMsgA);
			return -1;
		}
	}


	//v4.49A10	//Only log result in non-AUTOBOND mode
	if (bLog)
	{
		LONG lInputDieNo = lDieNo;
		if (lInputDieNo <= 0)
			lInputDieNo = 1;
		if (bDieType == WPR_NORMAL_DIE)
			lInputDieNo = WPR_GEN_NDIE_OFFSET + lInputDieNo;
		if (bDieType == WPR_REFERENCE_DIE)
			lInputDieNo = WPR_GEN_RDIE_OFFSET + lInputDieNo;
		
		LogUserSearchDieResult(lInputDieNo, stSrchRpy2.stStatus.uwPRStatus, 
								stSrchRpy2.stDieAlign[0].stGen.coDieCentre.x, 
								stSrchRpy2.stDieAlign[0].stGen.coDieCentre.y, 
								stSrchRpy2.stDieAlign[0].stGen.rDieRot, 
								stSrchRpy2.stDieAlign[0].stGen.rMatchScore,
								stCorner1, stCorner2,
								ubSID, ubRID);		//v4.49A10
	}

	//Identify die type
	ExtractDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore, &stPRDieSize);
	
	//v4.59A1
	LONG lUPX = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].x);
	LONG lUPY = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].y);
	LONG lLRX = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[2].x);
	LONG lLRY = _round(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[2].y);
	CString szLog;
	szLog.Format("Manual Search Uplook Die: Type = %d, C(%d, %d), UL(%ld, %ld), LR(%ld, %ld)",
				usDieType, 
				stSrchRpy2.stDieAlign[0].stGen.coDieCentre.x, 
				stSrchRpy2.stDieAlign[0].stGen.coDieCentre.y, 
				lUPX, lUPY, lLRX, lLRY);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


	if( GetDieShape() == WPR_RHOMBUS_DIE && bDieType==WPR_NORMAL_DIE && lDieNo==0 && bLog )
	{
		for(int i=0; i<WPR_RHOMBUS_CORNERS; i++)
		{
			m_stLearnDieCornerPos[i].x	= (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[i].x);
			m_stLearnDieCornerPos[i].y	= (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[i].y);
		}
	}

	if ((stPRDieSize.x > 0) && (stPRDieSize.y > 0))
	{
		//shiraishi03
		ConvertPixelToDUnit(stPRDieSize, m_dCurSrchDieSizeX, m_dCurSrchDieSizeY, IsMS90());		// for setup display purpose only	//v3.01T1
	}
	CalculateDefectScore(*usDieType, lDieNo, &stSrchRpy3);
	return 0;
}

//Check Orientation
LONG CWaferPr::SearchAOTDie()
{
	if( GetRunZoom() != GetNmlZoom() )
	{
		LiveViewZoom(GetNmlZoom(), FALSE, FALSE, 21);
	}
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;

	PR_WIN max_win;
	max_win.coCorner1.x = (PR_WORD) GetPrCenterX() - GetNmlSizePixelX() * 2;
	max_win.coCorner1.y = (PR_WORD) GetPrCenterY() - GetNmlSizePixelY() * 2;
	max_win.coCorner2.x = (PR_WORD) GetPrCenterX() + GetNmlSizePixelX() * 2;
	max_win.coCorner2.y = (PR_WORD) GetPrCenterY() + GetNmlSizePixelY() * 2;

	if (!m_bCheckDieOrientation)
	{
		return 0;
	}
//e-pole modify
	PR_UWORD uwResult = ManualSearchDie(WPR_NORMAL_DIE, WPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, max_win.coCorner1, max_win.coCorner2, FALSE, TRUE);

	if (usDieType == PR_ERR_OBJ_ROTATED_TO_REPEAT_ANGLE)
	{
		CString szMsg = "Align Wafer";
		LONG nErr = HMB_WT_DIE_ORIENTATION_FAIL;

		SetErrorMessage("WPR: Check NmlDie Orientation fail in SearchAOTDie");	//v0.08

		//SetAlert_Red_Back(IDS_WPR_ERR_ORIENTATION);
		//SetAlert_Msg_Red_Back(HMB_WT_DIE_ORIENTATION_FAIL, szMsg);
		SetAlert_Red_Back(HMB_WT_DIE_ORIENTATION_FAIL);// Ivan request 20190702
		return nErr;
	}

	if ( (uwResult == -1) || !DieIsAlignable(usDieType) )
	{
		SetErrorMessage("WPR: Check NmlDie fail in SearchAOTDie");	//v0.08
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		return IDS_WPR_PRSEARCHERROR;
	}

	return 0;
}


LONG CWaferPr::ChangePrRecordZoomMode(LONG lPrID)
{
	LONG lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_4X;

	if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_1X)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_1X;
	}
	else if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_2X)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_2X;
	}
	else if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_4X)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_4X;
	}
	else if (m_szGenDieZoomMode[lPrID] == PR_ZOOM_VALUE_FF)
	{
		lPrZoomMode = CPrZoomSensorMode::PR_ZOOM_MODE_FF;
	}
	
	LiveViewZoom(lPrZoomMode, FALSE, FALSE, 22);

	return 1;
}


BOOL CWaferPr::PrescanGoodDiePercentageCheck(ULONG ulGoodDie)
{
	CString szMsg, szTitle;
	DOUBLE dGoodDiePercentage;
	CUIntArray aulAllGradesList, aulSelGradeList;
	ULONG ulTotalDie = 0, ulTotal = 0, ulLeft = 0, ulPick = 0;
	UCHAR ucNullBin	= m_WaferMapWrapper.GetNullBin();

	//Get the Total Die in Wafer Map
	m_WaferMapWrapper.GetAvailableGradeList(aulAllGradesList);
	for (int i = 0; i < aulAllGradesList.GetSize(); i++)
	{
		m_WaferMapWrapper.GetStatistics((UCHAR)aulAllGradesList[i], ulLeft, ulPick, ulTotal);
		ulTotalDie += ulTotal;
	}

	if (ulTotalDie == 0)
	{
		return TRUE;
	}

	dGoodDiePercentage = ((DOUBLE)ulGoodDie / (DOUBLE)ulTotalDie) * 100;
	if (dGoodDiePercentage <= 10)
	{
		szTitle = "Good Die Percentage check";
		szMsg = "Prescan Good Die Percentage check fail.";
		HmiMessage_Red_Back(szMsg, szTitle);
		SetErrorMessage(szMsg);
		return FALSE;
	}
	
	return TRUE;
}