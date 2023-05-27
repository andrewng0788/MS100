//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_BlkFunc2.h"
#include "math.h"
#include "MS896A.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CWT_BlkFunc2::Blk2InitBlockFunc()
{
	LONG i, j;

	m_bRhombus4RefDie	= FALSE;
	m_stFDCBasic.m_ulReferDieNum = 0;
	m_stFDCBasic.m_ulFDCPitchRow = 0;
	m_stFDCBasic.m_ulFDCPitchCol = 0;
	m_stFDCBasic.m_ulMaxRowIdx = 0;
	m_stFDCBasic.m_ulMaxColIdx = 0;
	m_stFDCBasic.m_ulBlksColNum = 0;
	m_stFDCBasic.m_ulBlksRowNum = 0;
	m_ulBlk2WaferReferNum	= 0;	//	init.

	for (i=0; i<MAXFDC2NUM; i++)
	{
		m_stReferWftInfo[i].m_bInMap = FALSE;
		m_stReferWftInfo[i].m_cInWaf = 0;
		m_stReferWftInfo[i].m_lFaceValue = 0;
	}

	for (i=0; i<MAPMAXROW2; i++)
	{
		for (j=0; j<MAPMAXCOL2; j++)
		{
			m_stDiePhyPrep[i][j].m_lDieX = 0;
			m_stDiePhyPrep[i][j].m_lDieY = 0;
			m_stDiePhyPrep[i][j].m_sState = REGN_INIT2;
		}
	}

	m_stFDCBasic.m_bHaveAligned = FALSE;
	m_lAlignMatchLvl = 0;

	CString	strTemp;

	SetFDCRegionLog("Init block 2 function");

	LONG	lTmp;
	LONG	iRow, iCol;
	ULONG	ulBaseRow, ulBaseCol, ulFarRow, ulFarCol, ulDistRow, ulDistCol;
	ULONG	ulNumRefDie = 0, ulIndex;
	WAF_CMapDieInformation *astRefInfo;
	DieMapPos2	astRefInMap[MAXFDC2NUM];
	UCHAR	ucGrade;
	CString szMsg;

	ULONG ulNumReferInMap = m_stnWaferTable->m_WaferMapWrapper.GetNumberOfReferenceDice();
	astRefInfo  = new WAF_CMapDieInformation[ulNumReferInMap];

	szMsg.Format("Map Refer Die list, total %d", ulNumReferInMap);
	SetFDCRegionLog(szMsg);

	m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieList(astRefInfo, ulNumReferInMap);
	ulBaseRow = ulBaseCol = MAPMAXROW2;
	ulFarRow = ulFarCol = 0;
	for (i=0; i<(LONG)ulNumReferInMap; i++)
	{
		ULONG ulRow = astRefInfo[i].GetRow();
		ULONG ulCol = astRefInfo[i].GetColumn();
		if( m_stnWaferTable->IsOutMS90SortingPart(ulRow, ulCol) )
		{
			continue;
		}
		astRefInMap[ulNumRefDie].Row = (LONG)ulRow;
		astRefInMap[ulNumRefDie].Col = (LONG)ulCol;
		if (ulRow < ulBaseRow)
			ulBaseRow = ulRow;
		if (ulCol < ulBaseCol)
			ulBaseCol = ulCol;
		if( ulRow > ulFarRow )
			ulFarRow = ulRow;
		if( ulCol > ulFarCol )
			ulFarCol = ulCol;
		ulNumRefDie++;

		szMsg.Format("Map %3d %4d,%4d", ulNumRefDie, ulRow, ulCol);
		SetFDCRegionLog(szMsg);
	}
	delete[] astRefInfo;

	// Sorting
	for (i=0; i<(LONG)ulNumRefDie-1; i++)
	{
		for (j=i+1; j<(LONG)ulNumRefDie; j++)
		{
			if( astRefInMap[i].Row > astRefInMap[j].Row &&
				astRefInMap[i].Col > astRefInMap[j].Col )
			{
				lTmp = astRefInMap[i].Row;
				astRefInMap[i].Row = astRefInMap[j].Row;
				astRefInMap[j].Row = lTmp;
				lTmp = astRefInMap[i].Col;
				astRefInMap[i].Col = astRefInMap[j].Col;
				astRefInMap[j].Col = lTmp;
			}
		}
	}

	szMsg.Format("Refer Die list %d In Map after sorting", ulNumRefDie);
	SetFDCRegionLog(szMsg);
	for (i=0; i<(LONG)ulNumRefDie; i++)
	{
		szMsg.Format("Map %3d %4d,%4d", i+1, astRefInMap[i].Row, astRefInMap[i].Col);
		SetFDCRegionLog(szMsg);
	}

	ulDistRow = MAPMAXROW2;
	ulDistCol = MAPMAXCOL2;
	ULONG ulDist;
	for (i=0; i<(LONG)ulNumRefDie-1; i++)
	{
		ulDist = labs(astRefInMap[i+1].Row - astRefInMap[i].Row);
		if (ulDist != 0 && ulDist < ulDistRow)
		{
			ulDistRow = ulDist;
		}
		ulDist = labs(astRefInMap[i+1].Col - astRefInMap[i].Col);
		if (ulDist != 0 && ulDist < ulDistCol)
		{
			ulDistCol = ulDist;
		}
	}

	if( ulNumRefDie==4 )
	{
		ULONG ulSameLine = 0;
		for (i=0; i<(LONG)ulNumRefDie; i++)
		{
			ulSameLine = 0;
			for (j=0; j<(LONG)ulNumRefDie; j++)
			{
				if( astRefInMap[i].Row==astRefInMap[j].Row ||
					astRefInMap[i].Col==astRefInMap[j].Col )
				{
					ulSameLine++;
				}
			}
			if( ulSameLine!=2 )
			{
				break;
			}
		}
		if( ulSameLine==2 )
		{
			m_bRhombus4RefDie = TRUE;
		}
	}

	//if( Blk2IsSrchCharDie() )	xuzhijin OSRAM
	//{
	//	if(ulDistRow>ulDistCol )
	//		ulDistRow = ulDistCol;
	//	else
	//		ulDistCol = ulDistRow;
	//}

	m_stFDCBasic.m_ulFDCPitchRow = ulDistRow;
	m_stFDCBasic.m_ulFDCPitchCol = ulDistCol;

	ULONG lRefDieMaxRowIdx = (ULONG)(((ulFarRow - ulBaseRow)*1.0)/(1.0*GetBlkPitchRow()) +1.5);
	ULONG lRefDieMaxColIdx = (ULONG)(((ulFarCol - ulBaseCol)*1.0)/(1.0*GetBlkPitchCol()) +1.5);
	if( Blk2IsSrchCharDie()==FALSE )
	{
		lRefDieMaxRowIdx = (ulFarRow - ulBaseRow)/GetBlkPitchRow() + 1;
		lRefDieMaxColIdx = (ulFarCol - ulBaseCol)/GetBlkPitchCol() + 1;
	}
	m_stFDCBasic.m_ulMaxRowIdx = lRefDieMaxRowIdx;
	m_stFDCBasic.m_ulMaxColIdx = lRefDieMaxColIdx;
	m_stFDCBasic.m_ulReferDieNum = ulNumRefDie;

	SetFDCRegionLog("\n");

	ULONG ulNumOfRow=0, ulNumOfCol=0;

	m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulNumOfRow, ulNumOfCol);
	if( m_stnWaferTable->IsOutMS90SortingPart(ulNumOfRow, ulNumOfCol) )
	{
		if (m_stnWaferTable->IsRowModeSeparateHalfMap())
		{
			ulNumOfRow = m_stnWaferTable->GetMS90HalfMapMaxRow();
		}
		else
		{
			//ulNumOfCol = m_stnWaferTable->GetMS90HalfMapMaxCol();
		}
	}

	for (i=0; i<MAXREFERWIDTH; i++)
	{
		if (i==0)
			iRow = 0;
		else if (i==1)
			iRow = ulBaseRow;
		else
			iRow += GetBlkPitchRow();

		if( ((ULONG)iRow>=ulNumOfRow) && m_stFDCBasic.m_ulBlksRowNum==0 )
		{
			m_stFDCBasic.m_ulBlksRowNum = i;
		}

		for (j=0; j<MAXREFERWIDTH; j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if (j==0)
				iCol = 0;
			else if (j==1)
				iCol = ulBaseCol;
			else
				iCol += GetBlkPitchCol();

			m_stReferWftInfo[ulIndex].m_MapPos.Row = iRow;
			m_stReferWftInfo[ulIndex].m_MapPos.Col = iCol;

			if( (ULONG)iCol >= ulNumOfCol && m_stFDCBasic.m_ulBlksColNum==0 )
			{
				m_stFDCBasic.m_ulBlksColNum = j;
			}

			if ((ULONG)iRow >= ulNumOfRow || (ULONG)iCol >= ulNumOfCol)
			{
				continue;
			}

			ucGrade = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetDieInformation(iRow,iCol);
			if ( ucGrade == WAF_CMapConfiguration::DEFAULT_NULL_BIN )
			{
				continue;
			}

			for(ULONG k = 0; k<ulNumRefDie; k++)
			{
				if( astRefInMap[k].Row == iRow && astRefInMap[k].Col == iCol )
				{
					m_stReferWftInfo[ulIndex].m_bInMap = TRUE;
					if (m_stnWaferTable->m_WaferMapWrapper.HasReferenceFaceValue(iRow, iCol) == TRUE)
						m_stReferWftInfo[ulIndex].m_lFaceValue = m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieFaceValue(iRow, iCol);
					else
						m_stReferWftInfo[ulIndex].m_lFaceValue = 0;
					break;
				}
			}

			szMsg.Format("Matrix %2dx%2d, Map(%4d,%4d), Src(%4d,%4d), status(%d) Value(%2d), grade %3d",
				i, j,
				GetReferMapPosRow(ulIndex),		GetReferMapPosCol(ulIndex),
				GetReferSrcMapRow(ulIndex),		GetReferSrcMapCol(ulIndex),
				GetReferInMap(ulIndex),			GetReferMapPosValue(ulIndex),
				ucGrade - m_stnWaferTable->m_WaferMapWrapper.GetGradeOffset());
			SetFDCRegionLog(szMsg);
		}
	}

	m_stFDCBasic.m_lFDCPitchX_X = GetDiePitchHX() * GetBlkPitchCol();
	m_stFDCBasic.m_lFDCPitchX_Y = GetDiePitchHY() * GetBlkPitchCol();
	m_stFDCBasic.m_lFDCPitchY_X = GetDiePitchVX() * GetBlkPitchRow();
	m_stFDCBasic.m_lFDCPitchY_Y = GetDiePitchVY() * GetBlkPitchRow();

	SetFDCRegionLog("\n");
	strTemp.Format("Get Refer Min RC(%2d,%2d), Max RC(%2d,%2d), base (%2d,%2d), Far (%2d,%2d), FDC Pitch (%2d,%2d)", 
		GetBlkMinRowIdx(), GetBlkMinColIdx(), GetBlkMaxRowIdx(), GetBlkMaxColIdx(), 
		ulBaseRow, ulBaseCol, ulFarRow, ulFarCol,
		GetBlkPitchRow(), GetBlkPitchCol());
	SetFDCRegionLog(strTemp);
	strTemp.Format("Region Blocks row num %ld, column num %ld", GetBlocksRowNum(), GetBlocksColNum());
	SetFDCRegionLog(strTemp);
	ULONG ulIndexUL, ulIndexLR, ulBlkIndex;
	LONG lULRow, lULCol, lLRRow, lLRCol;
	for(i=1; i<=(LONG)GetBlocksRowNum(); i++)
	{
		for(j=1; j<=(LONG)GetBlocksColNum(); j++)
		{
			ulIndexUL = (i-1)*MAXREFERWIDTH + (j-1);
			ulIndexLR = (i-0)*MAXREFERWIDTH + (j-0);
			ulBlkIndex = (i-1)*GetBlocksColNum() + j;

			lULRow = GetReferMapPosRow(ulIndexUL);
			lULCol = GetReferMapPosCol(ulIndexUL);
			lLRRow = GetReferMapPosRow(ulIndexLR);
			lLRCol = GetReferMapPosCol(ulIndexLR);

			strTemp.Format("Block index	%4lu	(%4ld	%4ld)	(%4ld	%4ld)",
				ulBlkIndex, lULRow, lULCol, lLRRow, lLRCol);
			SetFDCRegionLog(strTemp);
		}
	}

	for (i=0; i<MAXREFERWIDTH; i++) 
	{
		for (j=0; j<MAXREFERWIDTH; j++) 
		{
			m_paaRefDiePos[i][j].m_WfX = 0;
			m_paaRefDiePos[i][j].m_WfY = 0;
			m_paaRefDiePos[i][j].m_lReferValue = 0;
			m_paaRefDiePos[i][j].m_sStatus = 0;
		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2DrawMap()
{
	int i,j;
	LONG lTLRow, lTLCol, lBRRow, lBRCol;

	for (i=1; i<MAXREFERWIDTH-1; i++)
	{
		lTLRow = GetReferMapPosRow(i*MAXREFERWIDTH) + 1;
		lTLCol = GetReferMapPosCol(i*MAXREFERWIDTH);
		lBRRow = GetReferMapPosRow((i+1)*MAXREFERWIDTH+MAXREFERWIDTH-1) + 1;
		lBRCol = GetReferMapPosCol((i+1)*MAXREFERWIDTH+MAXREFERWIDTH-1) + 1;
		m_stnWaferTable->m_pWaferMapManager->AddBoundary(lTLRow, lBRRow, lTLCol, lBRCol);
	}

	for (j=1; j<MAXREFERWIDTH-1; j++)
	{
		lTLRow = GetReferMapPosRow(j);
		lTLCol = GetReferMapPosCol(j) + 1;
		lBRRow = GetReferMapPosRow((MAXREFERWIDTH-1)*MAXREFERWIDTH+j+1) + 1;
		lBRCol = GetReferMapPosCol((MAXREFERWIDTH-1)*MAXREFERWIDTH+j+1) + 1;
		m_stnWaferTable->m_pWaferMapManager->AddBoundary(lTLRow, lBRRow, lTLCol, lBRCol);
	}

	return TRUE;
}

//================================================================
// GET function implementation section
//================================================================
BOOL CWT_BlkFunc2::Blk2SearchReferDieAtCurr(LONG *lPosX, LONG *lPosY)
{
	LONG lDig1 = 0, lDig2 = 0;
	return Blk2SearchReferDie(lPosX, lPosY, &lDig1, &lDig2, TRUE, FALSE);
}


LONG CWT_BlkFunc2::NextReferDiePositionValue(DiePhyPos2 PhyInput, UCHAR ucDirection, DiePhyPos2 *PhyOutput, LONG *lCharValue)
{
	LONG	lJumpCol = 0;
	LONG	lJumpRow = 0;

	switch( ucDirection )
	{
	case BLK_REFER_UP:
		lJumpRow = GetBlkPitchRow();
		lJumpCol = 0;
		break;
	case BLK_REFER_DOWN:
		lJumpRow = -GetBlkPitchRow();
		lJumpCol = 0;
		break;
	case BLK_REFER_LEFT:
		lJumpRow = 0;
		lJumpCol = GetBlkPitchCol();
		break;
	case BLK_REFER_RIGHT:
		lJumpRow = 0;
		lJumpCol = -GetBlkPitchCol();
		break;
	case BLK_REFER_UPLEFT:
		lJumpRow = GetBlkPitchRow();
		lJumpCol = GetBlkPitchCol();
		break;
	case BLK_REFER_UPRIGHT:
		lJumpRow = GetBlkPitchRow();
		lJumpCol = -GetBlkPitchCol();
		break;
	case BLK_REFER_DOWNLEFT:
		lJumpRow = -GetBlkPitchRow();
		lJumpCol = GetBlkPitchCol();
		break;
	case BLK_REFER_DOWNRIGHT:
		lJumpRow = -GetBlkPitchRow();
		lJumpCol = -GetBlkPitchCol();
		break;
	default:
		return FALSE;
		break;
	}

	int i;

	LONG	lResult;
	LONG	lPosX, lPosY;
	LONG	lDig1, lDig2;
	LONG	lJumpStepRow, lJumpStepCol;

	lJumpStepRow = lJumpRow;
	lJumpStepCol = lJumpCol;

	lPosX = PhyInput.x - lJumpStepRow*GetDiePitchVX() - lJumpStepCol*GetDiePitchHX();
	lPosY = PhyInput.y - lJumpStepRow*GetDiePitchVY() - lJumpStepCol*GetDiePitchHY();

	PhyOutput->x = lPosX;
	PhyOutput->y = lPosY;

	CString szMsg;
	szMsg.Format("Next Refer Die XY, %8d, %8d from %8d, %8d, by(%2d, %2d)",
		lPosX, lPosY, PhyInput.x, PhyInput.y, lJumpRow, lJumpCol);
	SetFDCRegionLog(szMsg);

	if (CheckWaferLimit(lPosX, lPosY) == FALSE)
	{
		return FALSE;
	}

	if ( MoveXYTo(lPosX, lPosY) == FALSE )
	{
		return REGN_IS_MOTORERROR;
	}
	Sleep(m_lPrSrchDelay);
	
	//Display map index
	lResult = Blk2SearchReferDieInFOV(&lPosX, &lPosY, &lDig1, &lDig2, FALSE);
	if ( lResult == TRUE )
	{
		PhyOutput->x = lPosX;
		PhyOutput->y = lPosY;
		*lCharValue = lDig1*10+lDig2;
		szMsg.Format("      Srch Die OK, %8d, %8d", lPosX, lPosY);
		SetFDCRegionLog(szMsg);
		return TRUE;
	}

	LONG lJumpStep = abs(lJumpRow)>abs(lJumpCol)?abs(lJumpRow):abs(lJumpCol);
	if( GetReferGrid()>=lJumpStep )
	{
		szMsg.Format("srch ref %2d jump step %2d", GetReferGrid(), lJumpStep);
		SetFDCRegionLog(szMsg);
		return FALSE;
	}


	LONG	lMoveLoop = lJumpStep/GetReferGrid();

	if (lJumpRow < 0)
		lJumpStepRow = 0-GetReferGrid();
	else
		lJumpStepRow = GetReferGrid();

	if (lJumpCol < 0)
		lJumpStepCol = 0-GetReferGrid();
	else
		lJumpStepCol = GetReferGrid();

	lPosX = PhyInput.x;
	lPosY = PhyInput.y;
	for(i=0; i<lMoveLoop; i++)
	{
		if( abs(lJumpRow)<=(i+1)*GetReferGrid() )
		{
			lJumpStepRow = 0;
		}
		if( abs(lJumpCol)<=(i+1)*GetReferGrid() )
		{
			lJumpStepCol = 0;
		}
		lPosX = lPosX - lJumpStepRow*GetDiePitchVX() - lJumpStepCol*GetDiePitchHX();
		lPosY = lPosY - lJumpStepRow*GetDiePitchVY() - lJumpStepCol*GetDiePitchHY();

		szMsg.Format("      Next Nml %d Die XY, %8d, %8d", i, lPosX, lPosY);
		SetFDCRegionLog(szMsg);

		if (CheckWaferLimit(lPosX, lPosY) == FALSE)
		{
			return FALSE;
		}

		if ( MoveXYTo(lPosX, lPosY) == FALSE )
		{
			return REGN_IS_MOTORERROR;
		}
		Sleep(m_lPrSrchDelay);

		lResult = Blk2SearchNormalDie(0, &lPosX, &lPosY, FALSE);
		szMsg.Format("      Srch Nml Die XY %d, %8d,%8d", lResult, lPosX, lPosY);
		SetFDCRegionLog(szMsg);
	}

	lJumpStepRow = lJumpRow%GetReferGrid();
	lJumpStepCol = lJumpCol%GetReferGrid();
	lPosX = lPosX - lJumpStepRow*GetDiePitchVX() - lJumpStepCol*GetDiePitchHX();
	lPosY = lPosY - lJumpStepRow*GetDiePitchVY() - lJumpStepCol*GetDiePitchHY();

	szMsg.Format("      Last Refer Die XY, %8d,%8d", lPosX, lPosY);
	SetFDCRegionLog(szMsg);
	if (CheckWaferLimit(lPosX, lPosY) == FALSE)
	{
		return FALSE;
	}
	if ( MoveXYTo(lPosX, lPosY) == FALSE )
	{
		return REGN_IS_MOTORERROR;
	}
	Sleep(m_lPrSrchDelay);
	lResult = Blk2SearchReferDieInFOV( &lPosX, &lPosY, &lDig1, &lDig2, FALSE);
	if ( lResult == TRUE )
	{
		PhyOutput->x = lPosX;
		PhyOutput->y = lPosY;
		*lCharValue = lDig1*10+lDig2;
	}

	szMsg.Format("      Srch Last %d, %8d,%8d", lResult, lPosX, lPosY);
	SetFDCRegionLog(szMsg);

	return lResult;
}

BOOL CWT_BlkFunc2::Blk2FoundAllReferDiePos()
{
	if( m_stnWaferTable->IsMS90HalfSortMode() )
	{
		return 	Blk2FindAlignAllReferHalfWafer();
	}

	LONG		i,j;
	DieMapRowColIdx	staDieStack[MAXFDC2NUM];
	LONG		lX,lY,lT, lHomeTheta;
	LONG		lDig1,lDig2;

	ULONG		ulIndex;
	LONG		sResult;

	int			nDieStackIndex;
	LONG lCurRowIdx, lCurColIdx;
	DiePhyPos2	CurPhy, TgtPhy;
	DiePhyPos2	stHomePhy;
	CString szMsg;
	CString	strTemp;

	Blk2InitBlockFunc();

	Blk2GetWTPosition(&lX, &lY, &lT);

	sResult = Blk2SearchReferDieInFOV( &lX, &lY, &lDig1, &lDig2, FALSE);
	if (sResult == FALSE)
	{
		szMsg = "No ref die found in current position!";
		SetDisplayErrorMessage(szMsg);
		return FALSE;
	}

	CurPhy.x = stHomePhy.x = lX;
	CurPhy.y = stHomePhy.y = lY;
	lHomeTheta = lT;
	m_stFDCBasic.m_stHomeDiePhyX = lX;
	m_stFDCBasic.m_stHomeDiePhyY = lY;
	m_stFDCBasic.m_stHomeDiePhyT = lT;

	m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();

	for (i=0; i<MAXFDC2NUM; i++)
	{
		staDieStack[i].m_lRowIdx = 0;
		staDieStack[i].m_lColIdx = 0;
	}

	// set the virtual center as the current row/col
	ulIndex = (REFER_BASE) * MAXREFERWIDTH + REFER_BASE;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_WfX = lX;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_WfY = lY;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_sStatus = 1;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_lReferValue = lDig1*10+lDig2;

	lCurRowIdx = staDieStack[0].m_lRowIdx = REFER_BASE;
	lCurColIdx = staDieStack[0].m_lColIdx = REFER_BASE;
	nDieStackIndex = 1;

	// Alignment in Cross Path
	SetFDCRegionLog("\n");
	strTemp.Format("Home Die Position: (%2d,%2d),    %8d,%8d,%8d, status %d\n",
		REFER_BASE, REFER_BASE, lX, lY, lT, 1);
	SetLogCtmMessage(strTemp);
	SetFDCRegionLog(strTemp);

	sResult = Blk2IndexCrossRefDiePos(CurPhy, staDieStack, &nDieStackIndex);	//	Home Die Up/Down/Left/Right
	if (sResult == FALSE) 
	{
		SetFDCRegionLog("cross find refer die fail");
		return FALSE;
	}

	LONG lNxtRow, lNxtCol;
	UCHAR ucNxtGo;
	BOOL  bNxtDie = FALSE, bRowColSame = TRUE;
	CurPhy.x = stHomePhy.x;
	CurPhy.y = stHomePhy.y;
	SetFDCRegionLog("Blk index cross refer done");
	SetFDCRegionLog("\n");

	while (nDieStackIndex > 0)
	{
		bNxtDie = FALSE;
		if (IsToStopAlign())
		{
			return FALSE;
		}

		for(ucNxtGo=BLK_REFER_UP; ucNxtGo<=BLK_REFER_DOWNRIGHT; ucNxtGo++)
		{
			bRowColSame = TRUE;
			switch( ucNxtGo )
			{
			case BLK_REFER_UP:
				lNxtRow = lCurRowIdx-1;
				lNxtCol = lCurColIdx;
				break;
			case BLK_REFER_LEFT:
				lNxtRow = lCurRowIdx;
				lNxtCol = lCurColIdx-1;
				break;
			case BLK_REFER_DOWN:
				lNxtRow = lCurRowIdx+1;
				lNxtCol = lCurColIdx;
				break;
			case BLK_REFER_RIGHT:
				lNxtRow = lCurRowIdx;
				lNxtCol = lCurColIdx+1;
				break;
			case BLK_REFER_UPRIGHT:
				lNxtRow = lCurRowIdx-1;
				lNxtCol = lCurColIdx+1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			case BLK_REFER_UPLEFT:
				lNxtRow = lCurRowIdx-1;
				lNxtCol = lCurColIdx-1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			case BLK_REFER_DOWNLEFT:
				lNxtRow = lCurRowIdx+1;
				lNxtCol = lCurColIdx-1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			case BLK_REFER_DOWNRIGHT:
				lNxtRow = lCurRowIdx+1;
				lNxtCol = lCurColIdx+1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			}

			if ( bRowColSame && GetRefDieStatus(lNxtRow, lNxtCol) == 0 )
			{
				bNxtDie = TRUE;
				break;
			}
		}

		if( bNxtDie )
		{
			LONG lCharValue;
			sResult = NextReferDiePositionValue(CurPhy, ucNxtGo, &TgtPhy, &lCharValue);
			if (sResult == TRUE)
			{
				m_paaRefDiePos[lNxtRow][lNxtCol].m_WfX = TgtPhy.x;
				m_paaRefDiePos[lNxtRow][lNxtCol].m_WfY = TgtPhy.y;
				m_paaRefDiePos[lNxtRow][lNxtCol].m_sStatus = 1;
				m_paaRefDiePos[lNxtRow][lNxtCol].m_lReferValue = lCharValue;
				staDieStack[nDieStackIndex].m_lRowIdx = lNxtRow;
				staDieStack[nDieStackIndex].m_lColIdx = lNxtCol;
				nDieStackIndex++;
				CurPhy = TgtPhy;
				lCurRowIdx = lNxtRow;
				lCurColIdx = lNxtCol;
				szMsg.Format("matrix found (%2d,%2d), posn (%8d,%8d), state %d, OCR %2d",
					lNxtRow, lNxtRow, TgtPhy.x, TgtPhy.y, 1, lCharValue);
				SetFDCRegionLog(szMsg);
				continue;
			}
			else if (sResult == FALSE)
			{
				m_paaRefDiePos[lNxtRow][lNxtCol].m_sStatus = -1;
			}
			else
			{
				return FALSE;
			}
		}

		nDieStackIndex--;
		if (nDieStackIndex!=0)
		{
			lCurRowIdx = staDieStack[nDieStackIndex-1].m_lRowIdx;
			lCurColIdx = staDieStack[nDieStackIndex-1].m_lColIdx;
			CurPhy.x = GetRefDieWftX(lCurRowIdx, lCurColIdx);
			CurPhy.y = GetRefDieWftY(lCurRowIdx, lCurColIdx);
			szMsg.Format("all check to index (%2d,%2d), posn (%8d,%8d)", lCurRowIdx, lCurColIdx, CurPhy.x, CurPhy.y);
			SetFDCRegionLog(szMsg);
		}
	}//endwhile

	SetFDCRegionLog("\n");
	szMsg.Format("Find Refer idx(), wft()");
	SetFDCRegionLog(szMsg);
	for(i=0; i<MAXREFERWIDTH; i++)
	{
		for(j=0; j<MAXREFERWIDTH; j++)
		{
			if( GetRefDieStatus(i, j)==1 )
			{
				m_ulBlk2WaferReferNum++;
				szMsg.Format("index (%2d,%2d), posn (%8d,%8d), OCR %2d",
					i, j, GetRefDieWftX(i, j), GetRefDieWftY(i, j), GetRefDieValue(i, j));
				SetFDCRegionLog(szMsg);
			}
		}
	}

	sResult = Blk2MergeAlignMap();

	// for HMI display purpose
	LONG lRefFoundInDeed = m_ulBlk2WaferReferNum>GetMapReferNum()?GetMapReferNum():m_ulBlk2WaferReferNum;
	m_stnWaferTable->SetRefDieCounts(GetMapReferNum(), lRefFoundInDeed);

	strTemp.Format("Found Refer die Count = %lu; indeed %ld, merge align result %d\n",
		m_ulBlk2WaferReferNum, lRefFoundInDeed, sResult);
	SetFDCRegionLog(strTemp);

	BOOL bManAlign = FALSE;
	if (sResult == FALSE)
	{
		if ( IsAutoManualAlign() )	//Auto proceed to manual align
		{
			strTemp = "BlockPick: Auto Align fails. Auto execute Manual Align.";
			SetErrorLogMessage(strTemp);
			SetFDCRegionLog(strTemp);
		}
		else
		{
			LONG lTmp;
			strTemp = "Do you want to continue by\n\n    Manual-Alignment";
			SetErrorLogMessage(strTemp);
			SetFDCRegionLog(strTemp);
			lTmp = SetHmiMessage(strTemp, "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			if (lTmp == glHMI_NO)
			{
				return FALSE;
			}
		}
		bManAlign = TRUE;
	}
	else
	{
		DOUBLE dPercent = lRefFoundInDeed*100.0/GetMapReferNum();
		if (dPercent < m_lBlk2MnPassPercent)
		{
			if ( IsAutoManualAlign() )	//Auto proceed to manual align
			{
				strTemp.Format("Only %.2f Percent of reference dice can be found.\n Auto execute Manual Align...", dPercent);
				SetErrorLogMessage(strTemp);
				SetFDCRegionLog(strTemp);
			}
			else
			{
				LONG lTmp;
				strTemp.Format("Only %.2f% of reference dice can be found.\n\nDo you want to continue by\n    Manual-Alignment?", dPercent);
				SetFDCRegionLog(strTemp);
				lTmp = SetHmiMessage(strTemp, "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
				if (lTmp == glHMI_NO)
				{
					return FALSE;
				}
			}
			bManAlign = TRUE;
		}
	}

	if( bManAlign )
	{
		LONG lX, lY, lT;
		sResult = Blk2ManAlignWafer(stHomePhy);
		Blk2GetWTPosition(&lX, &lY, &lT);
		lHomeTheta = lT;
	}

	SetFDCRegionLog("\n");
	SetFDCRegionLog("After alignment finished");
	PrintFDC2();
	if( bManAlign && (sResult == FALSE) )
	{
		return FALSE;
	}

	sResult = CheckRelativePstnInFDC();
	if (sResult == FALSE)
	{
		return FALSE;
	}

	m_stFDCBasic.m_bHaveAligned = TRUE;

	//v2.61T7
	//-- Find farthest ref-die position for COR checking --//
	if (m_stnWaferTable->IsCheckCOREnabled())
	{
		LONG lx, ly;
		LONG lCorX, lCorY;
		m_stnWaferTable->GetWaferCalibXY(lCorX, lCorY);
		DOUBLE dMaxDist = 0, dDist = 0;
		LONG lMaxX=0, lMaxY=0;
		for (i=0; i<MAXREFERWIDTH; i++) 
		{
			for (j=0; j<MAXREFERWIDTH; j++) 
			{
				if (GetRefDieStatus(i, j) == 1)
				{		
					lx = GetRefDieWftX(i, j);
					ly = GetRefDieWftY(i, j);
					if (CheckWaferLimit(lx, ly) == FALSE)
					{
						continue;
					}
					dDist = GetDistIn2Points(lCorX, lCorY, lx, ly);
					if (dDist > dMaxDist)
					{
						dMaxDist = dDist;
						lMaxX = lx;
						lMaxY = ly;
					}
				}
			}
		}

		LONG lOldX, lOldY, lOldT;
		Blk2GetWTPosition(&lOldX, &lOldY, &lOldT);
		MoveXYTo(lMaxX, lMaxY);
		Sleep(m_lPrSrchDelay);

		if (!Blk2CheckCOR())
		{
			SetDisplayErrorMessage("COR-check fails on farthest ref-die!");
			SetErrorLogMessage("COR-check fails on farthest ref-die!");
			return FALSE;
		}

		MoveXYTo(lOldX, lOldY);
		Sleep(m_lPrSrchDelay);
	}

	//Get edge position
	if (IsToStopAlign())
	{
		return FALSE;
	}

	Blk2DrawMap();		//Block2

	sResult = Blk2RealignRefDiePos(m_bBlk2MnEdgeAlign);
	if (sResult == FALSE)
	{
		return FALSE;
	}

	//Show RefDie
	LONG	lTLRow, lTLCol;
	LONG	ulRow, ulCol;

    for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex) && GetReferInWfa(ulIndex) == 1)
			{
				lTLRow = GetReferMapPosRow(ulIndex);
				lTLCol = GetReferMapPosCol(ulIndex);

				m_stnWaferTable->m_pWaferMapManager->AddBoundary(lTLRow, lTLRow+2, lTLCol, lTLCol+2);

				m_stnWaferTable->SetMapPhyPosn(lTLRow, lTLCol, GetReferDiePhyX(ulIndex), GetReferDiePhyY(ulIndex));

				SetMapAlgorithmPrm("Row", i);
				SetMapAlgorithmPrm("Col", j);
				SetMapAlgorithmPrm("Align", REGN_ATREF2);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);
			}
		}
	}

	Blk2SetStartDie(stHomePhy.x, stHomePhy.y, lHomeTheta);
	m_stFDCBasic.m_stHomeDiePhyT = lHomeTheta;
	Blk2GetStartDie(&ulRow, &ulCol, &lX, &lY);
	m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);

	if (CheckEmptyDieGrade() == FALSE)
	{
		return FALSE;
	}

	Blk2SetBondingLastDie();

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2MergeAlignMap()
{
	LONG i,j;
	unsigned long ulIndex;
	INT k1,k2;
	INT iMergeRow, iMergeCol,iMerge=0;
	BOOL bRtn;
	LONG lMinRowIdx, lMinColIdx, lMaxRowIdx, lMaxColIdx;

	if( m_bRhombus4RefDie )
	{
		return Blk2MergeAlignRhombusMap();
	}

	if( Blk2IsSrchCharDie()==FALSE )
	{
		return Blk2MergeAlignMapLessRefer();
	}

	lMinRowIdx = lMinColIdx = REFER_BASE;
	lMaxRowIdx = lMaxColIdx = REFER_BASE;
	for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			if (GetRefDieStatus(i, j) == 1)
			{		
				if (i < lMinRowIdx ) lMinRowIdx = i;
				if (j < lMinColIdx ) lMinColIdx = j;
				if (i > lMaxRowIdx ) lMaxRowIdx = i;
				if (j > lMaxColIdx ) lMaxColIdx = j;
			}
		}
	}

	for (i=0; i<MAXREFERWIDTH - GetBlkMaxRowIdx() + GetBlkMinRowIdx(); i++)
	{
		for (j=0; j<MAXREFERWIDTH - GetBlkMaxColIdx() + GetBlkMinColIdx(); j++)
		{
			bRtn = TRUE;
			for (k1=lMinRowIdx; k1<=lMaxRowIdx; k1++)
			{
				for (k2=lMinColIdx; k2<=lMaxColIdx; k2++)
				{
					ulIndex = (k1-lMinRowIdx+GetBlkMinRowIdx()+i)*MAXREFERWIDTH + 
							  (k2-lMinColIdx+GetBlkMinColIdx()+j);
					if ( GetReferInMap(ulIndex)==FALSE && GetRefDieStatus(k1, k2) == 1 )
					{
						bRtn = FALSE;
						break;
					}
				}
				if (bRtn == FALSE)
				{
					break;
				}
			}
			if (bRtn == TRUE) 
			{
				iMergeRow = i;
				iMergeCol = j;
				iMerge++;
			}
		}
	}

	CString strTemp;
	SetFDCRegionLog("\n");
	strTemp.Format("Min RC Index(%2d,%2d), Max RC Index(%2d,%2d), merge match %d",
		lMinRowIdx, lMinColIdx, lMaxRowIdx, lMaxColIdx, iMerge);
	SetFDCRegionLog(strTemp);

	if ( iMerge == 1 )
	{
		for (k1=lMinRowIdx; k1<=lMaxRowIdx; k1++)
		{
			for (k2=lMinColIdx; k2<=lMaxColIdx; k2++)
			{
				ulIndex =  (k1-lMinRowIdx+GetBlkMinRowIdx()+iMergeRow)*MAXREFERWIDTH +
							(k2-lMinColIdx+GetBlkMinColIdx()+iMergeCol);
				if ( GetRefDieStatus(k1, k2) == 1 )
				{
					SetReferWftInfo(ulIndex, GetRefDieWftX(k1, k2), GetRefDieWftY(k1,k2));
					m_stReferWftInfo[ulIndex].m_cInWaf = 1;
					SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

LONG CWT_BlkFunc2::Blk2IndexCrossRefDiePos(DiePhyPos2 HomeCtrPhy, DieMapRowColIdx *pDieStack, int *pStackPtr)
{
	DiePhyPos2	CurrPhy;
	DiePhyPos2	NextPhy = {0,0};
	LONG lNextRowIdx, lNextColIdx;
	CString szMsg, szTemp;

	int			nDieStackPtr;

	LONG		sResult;

	nDieStackPtr = *pStackPtr;

	int nNextDir = BLK_REFER_UP;
	LONG ulTryTime = 0, ulTryLimit = 0;
	for(nNextDir=BLK_REFER_UP; nNextDir<=BLK_REFER_RIGHT; nNextDir++)
	{
		//Move to Home Die
		ulTryTime = 0;
		lNextRowIdx = REFER_BASE;
		lNextColIdx = REFER_BASE;
		CurrPhy = HomeCtrPhy;
		MoveXYTo(CurrPhy.x, CurrPhy.y);
		Sleep(m_lPrSrchDelay);
		Blk2MatchAlignCharMap();
		SetFDCRegionLog("\n");
		szMsg.Format("move to home die %8d,%8d at dir %d", CurrPhy.x, CurrPhy.y, nNextDir);
		SetFDCRegionLog(szMsg);

		while(1)
		{
			if (IsToStopAlign())
			{
				return FALSE;
			}

			switch( nNextDir )
			{
			// look up to find all the refer die
			case BLK_REFER_UP:
				lNextRowIdx = lNextRowIdx-1;
				ulTryLimit = (GetBlkMaxRowIdx()-GetBlkMinRowIdx());
				break;
			// look left to find all refer die in same row
			case BLK_REFER_LEFT:
				lNextColIdx = lNextColIdx-1;
				ulTryLimit = (GetBlkMaxColIdx()-GetBlkMinColIdx());
				break;
			// look down to find all refer die
			case BLK_REFER_DOWN:
				lNextRowIdx = lNextRowIdx+1;
				ulTryLimit = (GetBlkMaxRowIdx()-GetBlkMinRowIdx());
				break;
			// look right to find all refer die in same row
			case BLK_REFER_RIGHT:
				lNextColIdx = lNextColIdx+1;
				ulTryLimit = (GetBlkMaxColIdx()-GetBlkMinColIdx());
				break;
			}

			szMsg.Format("dir %d, NextIdx(%2d,%2d), status %d", nNextDir, lNextRowIdx, lNextColIdx, GetRefDieStatus(lNextRowIdx, lNextColIdx));

			if( lNextRowIdx >= 0 && lNextRowIdx < MAXREFERWIDTH && 
				lNextColIdx >= 0 && lNextColIdx < MAXREFERWIDTH && 
				GetRefDieStatus(lNextRowIdx, lNextColIdx) == 0 )
			{
			}
			else
			{
				SetFDCRegionLog(szMsg);
				break;
			}

			LONG lCharValue;
			ulTryTime++;
			sResult = NextReferDiePositionValue(CurrPhy, nNextDir, &NextPhy, &lCharValue);
			if( sResult==REGN_IS_MOTORERROR )
			{
				return FALSE;
			}

			if (sResult == TRUE)
			{
				m_paaRefDiePos[lNextRowIdx][lNextColIdx].m_lReferValue = lCharValue;
				m_paaRefDiePos[lNextRowIdx][lNextColIdx].m_sStatus = 1;
			}

			m_paaRefDiePos[lNextRowIdx][lNextColIdx].m_WfX = NextPhy.x;
			m_paaRefDiePos[lNextRowIdx][lNextColIdx].m_WfY = NextPhy.y;
			pDieStack[nDieStackPtr].m_lRowIdx = lNextRowIdx;
			pDieStack[nDieStackPtr].m_lColIdx = lNextColIdx;
			nDieStackPtr++;
			CurrPhy = NextPhy;
			if( sResult==FALSE )
			{
				if( ulTryTime>=ulTryLimit )
				{
					break;
				}
			}
		}
	}

	//Update Input Parameter
	*pStackPtr = nDieStackPtr;

	Blk2MatchAlignCharMap();

	SetFDCRegionLog("\n");
	szMsg.Format("move to home die %8d,%8d original", HomeCtrPhy.x, HomeCtrPhy.y);
	SetFDCRegionLog(szMsg);
	//Move to Home Die
	MoveXYTo(HomeCtrPhy.x, HomeCtrPhy.y);
	Sleep(m_lPrSrchDelay);

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2MatchAlignCharMap()	// only for char die
{
	int i,j;
	ULONG ulCounter, ulIndex1, ulIndex2;

	BOOL bFound, bIsCharDie;
	LONG lRowInStick, lColInStick;
	LONG lRowOff, lColOff;
	BOOL bContMatch = FALSE;

	if( m_bRhombus4RefDie )
	{
		return TRUE;
	}

	if( Blk2IsSrchCharDie()==FALSE )
	{
		return TRUE;
	}

	if (m_lAlignMatchLvl == 2)
	{
		return TRUE;
	}

	//Found base
	bFound = FALSE;
	for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			if (GetRefDieStatus(i,j) == 1)
			{
				lRowInStick = i;
				lColInStick = j;
				bFound = TRUE;
				break;
			}
		}
		if (bFound)
		{
			break;
		}
	}
	if (!bFound)
	{
		return FALSE;
	}

	bFound = FALSE;
	for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			ulIndex1 = i*MAXREFERWIDTH + j;
			bIsCharDie = GetReferMapPosValue(ulIndex1) == GetRefDieValue(lRowInStick, lColInStick);
			if( bIsCharDie
				&& GetReferInMap(ulIndex1)
				&& GetRefDieStatus(lRowInStick, lColInStick) == 1)
			{
				lRowOff = lRowInStick - i;
				lColOff = lColInStick - j;
				bFound = TRUE;
				break;
			}
		}
		if (bFound)
		{
			break;
		}
	}
	if (!bFound)
	{
		return FALSE;
	}

	//Add limit
	ulCounter = 0;
	for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			ulIndex2 = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex2) != TRUE)
			{
				if ( i+lRowOff<0 || i+lRowOff>=MAXREFERWIDTH )
				{
					continue;
				}
				if ( j+lColOff<0 || j+lColOff>=MAXREFERWIDTH )
				{
					continue;
				}
				m_paaRefDiePos[i+lRowOff][j+lColOff].m_sStatus = 2;
			}
			else
			{
				bIsCharDie = GetRefDieValue(i+lRowOff, j+lColOff) == GetReferMapPosValue(ulIndex2);
				if( GetRefDieStatus(i+lRowOff, j+lColOff) == 1 && bIsCharDie )
				{
					ulCounter++;
				}
				if (ulCounter >= 1)
				{
					m_lAlignMatchLvl = 1;
				}
				if (ulCounter >= 3)
				{
					m_lAlignMatchLvl = 2;
				}
			}
		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::GetUserMapCenterDie(DieMapPos2 *CtrDie, DiePhyPos2 *PhyPos, LONG *lDigital)
{
	LONG		i,j;
	ULONG	ulIndex;

	*lDigital = 0;
	i = (GetBlkMinRowIdx() + GetBlkMaxRowIdx())/2;
	j = (GetBlkMinColIdx() + GetBlkMaxColIdx())/2;
	ulIndex = i*MAXREFERWIDTH + j;
	if ( GetReferInMap(ulIndex) )
	{
		CtrDie->Row = GetReferSrcMapRow(ulIndex);
		CtrDie->Col = GetReferSrcMapCol(ulIndex);
		PhyPos->x = GetReferDiePhyX(ulIndex);
		PhyPos->y = GetReferDiePhyY(ulIndex);
		*lDigital = GetReferMapPosValue(ulIndex);
		return TRUE;
	}

	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex) )
			{
				CtrDie->Row = GetReferSrcMapRow(ulIndex);
				CtrDie->Col = GetReferSrcMapCol(ulIndex);
				PhyPos->x = GetReferDiePhyX(ulIndex);
				PhyPos->y = GetReferDiePhyY(ulIndex);
				*lDigital = GetReferMapPosValue(ulIndex);
				return TRUE;
			}
		}
	}

	return FALSE;
}

LONG CWT_BlkFunc2::Blk2GetNearRefDie(DieMapPos2 SrcDie, DieMapPos2 *RDieMap, DiePhyPos2 *RDiePhy)
{
	int			i,j;
	int			iRow,iCol;
	DieMapPos2	sMinDistRC;
	double		dMinDist;
	double		dTmpDist;
	ULONG		ulIndex;

	dMinDist = GetDistIn2Points(MAPMAXROW2, MAPMAXCOL2, 0, 0);
	sMinDistRC.Row = -1;
	sMinDistRC.Col = -1;
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}
			if (GetReferInWfa(ulIndex) == 0)
			{
				continue;
			}
			iRow = GetReferMapPosRow(ulIndex);
			iCol = GetReferMapPosCol(ulIndex);
			dTmpDist = GetDistIn2Points(SrcDie.Row, SrcDie.Col, iRow, iCol);
			if (dTmpDist < dMinDist)
			{
				dMinDist = dTmpDist;
				sMinDistRC.Row = i;
				sMinDistRC.Col = j;
			}
		}
	}

	if ( sMinDistRC.Row == -1 || sMinDistRC.Col == -1 )
	{
		return FALSE;
	}

	ulIndex = sMinDistRC.Row*MAXREFERWIDTH + sMinDistRC.Col;
	RDieMap->Row = GetReferMapPosRow(ulIndex);
	RDieMap->Col = GetReferMapPosCol(ulIndex);
	RDiePhy->x = GetPrepDieWftX(RDieMap->Row, RDieMap->Col);
	RDiePhy->y = GetPrepDieWftY(RDieMap->Row, RDieMap->Col);

	return TRUE;
}

LONG CWT_BlkFunc2::Blk2GetNextDiePosnAndT(DieMapPos2 stNextMap, DiePhyPos2 *PhyPos, LONG *lValueT)
{
	LONG		lX, lY, lT;
	DOUBLE		dResolution = 1;
	DOUBLE		dThetaAngle;

	DieMapPos2	stNearMap;
	CString szMsg;

	//Get Pitch
	stNearMap.Row = -1;
	stNearMap.Col = -1;
	if (Blk2GetNearDieMapPos(stNextMap, &stNearMap) == FALSE)
	{
		szMsg.Format("Next Die XYT near die error");
		SetFDCRegionLog(szMsg);
		return FALSE;
	}

	PhyPos->x = GetPrepDieWftX(stNearMap.Row, stNearMap.Col) + (stNextMap.Col - stNearMap.Col)*GetDiePitchHX() + (stNextMap.Row - stNearMap.Row)*GetDiePitchVX();
	PhyPos->y = GetPrepDieWftY(stNearMap.Row, stNearMap.Col) + (stNextMap.Col - stNearMap.Col)*GetDiePitchHY() + (stNextMap.Row - stNearMap.Row)*GetDiePitchVY();

	//Convert X,Y,T
	Blk2GetWTPosition(&lX, &lY, &lT);
	lX = PhyPos->x;
	lY = PhyPos->y;
	m_stnWaferTable->GetThetaRes(&dResolution);
	dThetaAngle = -(DOUBLE)(lT - GetHomeTheta()) * dResolution;
	m_stnWaferTable->GetRtnForRotateWaferTheta(&lX, &lY, &lT, dThetaAngle);
	PhyPos->x = lX;
	PhyPos->y = lY;
	*lValueT = lT;

	szMsg.Format("Near Map(%d,%d) Map(%d,%d),Phy(%d,%d)", stNearMap.Row,stNearMap.Col, stNextMap.Row, stNextMap.Col, PhyPos->x, PhyPos->y);
	DisplayDebugString(szMsg);

	return TRUE;
}

LONG CWT_BlkFunc2::Blk2GetNextDiePosn(DieMapPos2 MapPos, DiePhyPos2 *PhyPos)
{
	LONG lTValue;
	return Blk2GetNextDiePosnAndT(MapPos, PhyPos, &lTValue);
}

//New method to calculate next die physical position
BOOL CWT_BlkFunc2::Blk2GetNextDiePhyPos(LONG lRow, LONG lCol, LONG lLastRow, LONG lLastCol, LONG &lOutX, LONG &lOutY, BOOL bLog)
{
	DieMapPos2	NextMapPos, RefDieMap, NearDieMap;
	DiePhyPos2	RefDiePhy;

	NextMapPos.Row = lRow;
	NextMapPos.Col = lCol;

	CString szHead, szDetail;
	szHead.Format("Next Die (%ld,%ld), last (%ld,%ld) ", lRow, lCol, lLastRow, lLastCol);

	DOUBLE dLastDieDist = 2000, dMinDistance = 2000, dRefrDieDist = 2000, dNearDieDist = 2000;

	// Get reference die
	if ( Blk2GetNearRefDie(NextMapPos, &RefDieMap, &RefDiePhy) == FALSE)
	{
		szDetail = "get near refer die wrong";
		DisplayDebugString(szHead + szDetail);
		return FALSE;
	}

	/* * * The Default Method * * */
	if (Blk2GetNearDieMapPos(NextMapPos, &NearDieMap) == FALSE)
	{
		szDetail = "get near walked die wrong";
		DisplayDebugString(szHead + szDetail);
		return FALSE;
	}

	dNearDieDist = GetDistIn2Points(NearDieMap.Row, NearDieMap.Col, lRow, lCol);
	dRefrDieDist = GetDistIn2Points(RefDieMap.Row, RefDieMap.Col, lRow, lCol);

	if( GetPrepDieStatus(lLastRow, lLastCol) == REGN_ALIGN2 || 
		GetPrepDieStatus(lLastRow, lLastCol) == REGN_HPICK2 )
	{
		dLastDieDist = GetDistIn2Points(lLastRow, lLastCol, lRow, lCol);
	}

    //If the target die has been gone through before, use the previous stored physical position	
	if( (lCol == NearDieMap.Col) && (lRow == NearDieMap.Row) )
	{
		lOutX = GetPrepDieWftX(lRow, lCol);
		lOutY = GetPrepDieWftY(lRow, lCol);

		szDetail.Format("From walked die posn (%d,%d)", lOutX, lOutY);
		DisplayDebugString(szHead + szDetail);
		return TRUE;
	}

	dMinDistance = min(dRefrDieDist, dLastDieDist);
	dMinDistance = min(dMinDistance, dNearDieDist);
	szDetail.Format("Refer dist %d %.2f(%d,%d); near dist %.2f(%d,%d); last dist %.2f stt(%d)", 
		m_lBlk2MaxJumpCtr,
		dRefrDieDist, RefDieMap.Row, RefDieMap.Col, dNearDieDist, NearDieMap.Row, NearDieMap.Col, 
		dLastDieDist, GetPrepDieStatus(lLastRow, lLastCol));

	//Use the new method only if the distance of the die used to calculate is not too far away from the target die
	LONG lNearestRow, lNearestCol, lNearPosX, lNearPosY;
	CString szLog = "";
	if( dMinDistance==dLastDieDist || dLastDieDist<m_lBlk2MaxJumpCtr )
	{
		lNearestRow = lLastRow;
		lNearestCol = lLastCol;
		szLog = "last";
	}
	else if( dMinDistance==dRefrDieDist )
	{
		lNearestRow = RefDieMap.Row;
		lNearestCol = RefDieMap.Col;
		szLog = "Refr";
	}
	else if( dMinDistance==dNearDieDist )
	{
		if( dMinDistance > m_lBlk2MaxJumpCtr )
		{
			lNearestRow = RefDieMap.Row;
			lNearestCol = RefDieMap.Col;
			szLog = "Refr";
		}
		else
		{
			lNearestRow = NearDieMap.Row;
			lNearestCol = NearDieMap.Col;
			szLog = "near";
		}
	}
	else	//use back the old method
	{
		DisplayDebugString(szHead + szDetail + "near is xxxxxx die");
		return FALSE;
	}

	lNearPosX = GetPrepDieWftX(lNearestRow, lNearestCol);
	lNearPosY = GetPrepDieWftY(lNearestRow, lNearestCol);

	lOutX = lNearPosX + (lCol - lNearestCol)*GetDiePitchHX() + (lRow - lNearestRow)*GetDiePitchVX();
	lOutY = lNearPosY + (lCol - lNearestCol)*GetDiePitchHY() + (lRow - lNearestRow)*GetDiePitchVY();

	szHead = szHead + szDetail;
	szDetail.Format(" From %s (%d,%d) wft(%ld,%ld), to (%d,%d)",
		szLog, lNearestRow, lNearestCol, lNearPosX, lNearPosY, lOutX, lOutY);

	DisplayDebugString(szHead + szDetail);

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2GetNearDieMapPos(DieMapPos2 SrcPos, DieMapPos2 *TgtPos)
{
	BOOL	bFindNearDie = FALSE;
	LONG	lTmpRow, lTmpCol;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	LONG ulIntRow = SrcPos.Row;
	LONG ulIntCol = SrcPos.Col;
	ULONG ulLoop = MAPMAXROW2;
	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;

	short  sDieState = GetPrepDieStatus(lTmpRow, lTmpCol);
	if (sDieState == REGN_ALIGN2 || sDieState == REGN_HPICK2 )
	{
		TgtPos->Row = lTmpRow;
		TgtPos->Col = lTmpCol;
		return TRUE;
	}

	for (ulCurrentLoop=1; ulCurrentLoop<=ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop*2 + 1);
		lCol = (ulCurrentLoop*2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if( lTmpRow>=0 && lTmpCol>=0 )
		{
			sDieState = GetPrepDieStatus(lTmpRow, lTmpCol);
			if (sDieState == REGN_ALIGN2 || sDieState == REGN_HPICK2 )
			{
				bFindNearDie = TRUE;
				break;
			}
		}

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

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				sDieState = GetPrepDieStatus(lTmpRow, lTmpCol);
				if ( sDieState == REGN_ALIGN2 || sDieState == REGN_HPICK2 )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				sDieState = GetPrepDieStatus(lTmpRow, lTmpCol);
				if (sDieState== REGN_ALIGN2 || sDieState == REGN_HPICK2 )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow-1)-lCurrentIndex;
		while(1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				sDieState = GetPrepDieStatus(lTmpRow, lTmpCol);
				if (sDieState== REGN_ALIGN2 || sDieState == REGN_HPICK2 )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol-1)-lCurrentIndex;
		while(1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if( lTmpRow>=0 && lTmpCol>=0 )
			{
				sDieState = GetPrepDieStatus(lTmpRow, lTmpCol);
				if (sDieState== REGN_ALIGN2 || sDieState == REGN_HPICK2 )
				{
					bFindNearDie = TRUE;
					break;
				}
			}
		}

		if( bFindNearDie )
		{
			break;
		}
	}

	if ( bFindNearDie )
	{
		TgtPos->Row = lTmpRow;
		TgtPos->Col = lTmpCol;
	}

	return bFindNearDie;
}

BOOL CWT_BlkFunc2::Blk2SetCurDiePhyPos(LONG lRow, LONG lCol, LONG lInX, LONG lInY, LONG lStatus)
{
	m_stDiePhyPrep[lRow][lCol].m_lDieX = lInX;
	m_stDiePhyPrep[lRow][lCol].m_lDieY = lInY;
	m_stDiePhyPrep[lRow][lCol].m_sState = (SHORT) lStatus;

	CString szMsg;
	szMsg.Format("Blk2 Set (%ld,%ld) Curr Phy %ld,%ld stt %ld", lRow, lCol, lInX, lInY, lStatus);
	DisplayDebugString(szMsg);
	//Update
	SetMapAlgorithmPrm("Row", lRow);
	SetMapAlgorithmPrm("Col", lCol);
	SetMapAlgorithmPrm("Align", lStatus);
	m_stnWaferTable->UpdateMapDie(lRow, lCol, 1, WAF_CDieSelectionAlgorithm::ALIGN);


	INT iDig1, iDig2;
	LONG i,j;
	BOOL bRtn;
	bRtn = Blk2IsReferDie((ULONG)lRow, (ULONG)lCol, &iDig1, &iDig2);

	if ( bRtn )
	{
		for (i=lRow-5;i<=lRow+5;i++)
		{
			for (j=lCol-5;j<=lCol+5;j++)
			{
				if (i < 0 || i >= MAPMAXROW2 )		//bound the array to prevent software crash
					continue;
				
				if (j < 0 || j >= MAPMAXCOL2 )
					continue;

				bRtn = Blk2IsReferDie((ULONG)i,(ULONG)j,&iDig1,&iDig2);
				if (bRtn==TRUE )
					continue;
				UpdatePrepDieStatus(i, j);
			}
		}
	}

	SetMapAlgorithm();

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2GetStartDie(LONG *lRow, LONG *lCol, LONG *lX, LONG *lY)
{
	*lRow = m_stFDCBasic.m_lHomeMapRow;
	*lCol = m_stFDCBasic.m_lHomeMapCol;
	*lX = m_stFDCBasic.m_stHomeDiePhyX;
	*lY = m_stFDCBasic.m_stHomeDiePhyY;

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2SetStartDie(LONG lX, LONG lY, LONG lT)
{
	int			i,j;
	LONG		lPosX,lPosY;
	DieMapPos2	sMinDistRC;
	double		dMinDist, dTmpDist;
	ULONG		ulIndex;

	dMinDist = 10000;
	sMinDistRC.Row = -1;
	sMinDistRC.Col = -1;
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}
			if (GetReferInWfa(ulIndex) == 0)
			{
				continue;
			}
			lPosX = GetReferDiePhyX(ulIndex);
			lPosY = GetReferDiePhyY(ulIndex);
			dTmpDist = GetDistIn2Points(lX, lY, lPosX, lPosY);
			if (dTmpDist < dMinDist)
			{
				dMinDist = dTmpDist;
				sMinDistRC.Row = GetReferMapPosRow(ulIndex);
				sMinDistRC.Col = GetReferMapPosCol(ulIndex);
			}
		}
	}

	if ( sMinDistRC.Row == -1 || sMinDistRC.Col == -1 )
	{
		return FALSE;
	}

	m_stFDCBasic.m_lHomeMapRow = sMinDistRC.Row;
	m_stFDCBasic.m_lHomeMapCol = sMinDistRC.Col;
	m_stFDCBasic.m_stHomeDiePhyX = lX;
	m_stFDCBasic.m_stHomeDiePhyY = lY;

	CString	strTemp;
	strTemp.Format("Set Start Die (%d,%d),(%d,%d,%d)",sMinDistRC.Row,sMinDistRC.Col, lX, lY, lT);
	SetFDCRegionLog(strTemp);
	SetFDCRegionLog("\n");

	return TRUE;
}


LONG CWT_BlkFunc2::CheckEmptyDieGrade()
{
	ULONG		i,j;
	DieMapPos2	CurrDie, RefDieMap;
	DiePhyPos2	CurrTblPos, RefDiePhy;
	ULONG	ulErrorCount, ulOkCount;

	ULONG	ulNumOfRow=0, ulNumOfCol=0;
	LONG	lPosX, lPosY;
	LONG	lResult;
	ULONG	ulLoop = 0;
	LONG lEmptyGrade = 0;
	UCHAR ucGradeOffset;

	ucGradeOffset = m_stnWaferTable->m_WaferMapWrapper.GetGradeOffset();
	lEmptyGrade = GetEmptyDieGrade();
	//Return If 0
	if( lEmptyGrade == 0 )
	{
		return TRUE;
	}

	if (!m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulNumOfRow, ulNumOfCol))
	{
		SetErrorLogMessage("Get Map Dimension Fails!");
		return TRUE;
	}
	if (m_stnWaferTable->IsOutMS90SortingPart(ulNumOfRow, ulNumOfCol))
	{
		if (m_stnWaferTable->IsRowModeSeparateHalfMap())
		{
			ulNumOfRow = m_stnWaferTable->GetMS90HalfMapMaxRow();
		}
		else
		{
			//ulNumOfCol = m_stnWaferTable->GetMS90HalfMapMaxCol();
		}
	}
	SetLogCtmMessage("Bin 17 Empty check Start!");

	CString strFileNmBin, strFileNmLog;
	CString strLocalFileNmBin, strLocalFileNmLog;
	CStdioFile	pFileBin, pFileLog;
	BOOL		bOutput;

	//Get Output Path
	CString	strTemp;
	CString	strMachineID;
	m_stnWaferTable->GetRtnForMachineNo(&strMachineID);
	bOutput = Blk2GetIniOutputFileName(&strFileNmBin, &strFileNmLog, &strLocalFileNmBin, &strLocalFileNmLog);
	if ( !bOutput ) 
	{
		strTemp.Format("Fails to initial output file path. Files will only be generated to local drive!");
		LONG lTemp = SetHmiMessage(strTemp, "Empty Die Check", glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
		SetErrorLogMessage(strTemp);

		if ( lTemp == glHMI_STOP)
		{
			SetErrorLogMessage("User chooses STOP!");
			SetErrorLogMessage("Auto Align FAIL!\n");
			return FALSE;
		}
		else
		{
			SetErrorLogMessage("User chooses CONTINUE!");
		}
	}

	ulErrorCount = 0;
	ulOkCount = 0;
	for(i=0;i<ulNumOfRow;i++)
	{
		for(j=0;j<ulNumOfCol;j++)
		{
			if( m_stnWaferTable->m_WaferMapWrapper.GetGrade(i,j) != lEmptyGrade + ucGradeOffset)
			{
				continue;
			}
			ulLoop++;

			// Index
			CurrDie.Row = i;
			CurrDie.Col = j;
			lResult = Blk2GetNearRefDie(CurrDie, &RefDieMap, &RefDiePhy);
			if ( lResult == FALSE) 
			{
				continue;
			}

			MoveXYTo(RefDiePhy.x,RefDiePhy.y);
			Sleep(m_lPrSrchDelay);
			CurrTblPos = RefDiePhy;
			lResult = Blk2IndexToNormalDie(RefDieMap, CurrDie, &CurrTblPos, 0);
			if ( lResult == FALSE) 
			{
				continue;
			}

			lPosX = CurrTblPos.x;
			lPosY = CurrTblPos.y;
			if( m_stnWaferTable->WftSearchNormalDie(lPosX, lPosY, TRUE)!=TRUE )	// EMPTY DIE OR PR EROR OR OUT OF LIMIT
			{
				ulOkCount++;
				continue;
			}

			strTemp.Format("No of Grade 17 Die found = %d", ulLoop);
			SetLogCtmMessage(strTemp);
			SetLogCtmMessage("Error! Normal Die is found!");

			LONG lTmp;

			ulErrorCount++;

			//* Generate Bin 17 (for Cree) Error Log whenever error is found //
			//Write Sum file
			bOutput = pFileBin.Open(strLocalFileNmBin, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText);

			if ( !bOutput )
			{
				strTemp.Format("Fails to open Sum file.");
				SetHmiMessage(strTemp, "Empty Die Check", glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
				SetErrorLogMessage(strTemp);
				SetErrorLogMessage("Auto Align FAIL!\n");
				return FALSE;
			}
			strTemp.Format("%d", ulErrorCount);
			pFileBin.WriteString(strTemp);
			pFileBin.Close();

			if ( CopyFile(strLocalFileNmBin, strFileNmBin, FALSE) == FALSE)
			{
				SetErrorLogMessage("Fail to copy Sum file to the network path! Please Check!");
			}

			//Write SDS file
			if (ulErrorCount == 1)		//Write the header and the first error found
			{
				bOutput = pFileLog.Open(strLocalFileNmLog, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText);
				if ( !bOutput )
				{
					strTemp.Format("Fails to open SDS file.");
					SetHmiMessage(strTemp, "Empty Die Check", glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
					SetErrorLogMessage(strTemp);
					SetErrorLogMessage("Auto Align FAIL!\n");	
					return FALSE;
				}
			
				pFileLog.SeekToEnd();
				
				//Write Header
				pFileLog.WriteString("Initial File Name, \n");
				pFileLog.WriteString("Bin Block Unload Date, \n");
				pFileLog.WriteString("Bin Block Unload Time, \n");
				strTemp.Format("Bin Block, %d\n", lEmptyGrade);
				pFileLog.WriteString(strTemp);
				strTemp.Format("Sorter, %s\n", strMachineID);
				pFileLog.WriteString(strTemp);
				//Average die rotation
				pFileLog.WriteString("Average die rotation, 0.00\n");
				//Number of visual defects
				pFileLog.WriteString("Number of visual defects, 0\n");
				//Number of Good die
				pFileLog.WriteString("Number of Good die, 0\n");
				//Package File Name
				pFileLog.WriteString("Package File Name, \n");
				pFileLog.WriteString("\nNumerical data is as follows: WaferX, WaferY, DiesheetX, DieSheetY\n\n");
				//Application sw version
				m_stnWaferTable->GetRtnForSwVerNo(&strTemp);
				pFileLog.WriteString(strTemp+"\n");
				//Map file name
				m_stnWaferTable->GetRtnForMapFileNm(&strTemp);
				//Ensure the WaferID is shown in the file
				if (strTemp.IsEmpty() == TRUE)
				{
					m_stnWaferTable->m_WaferMapWrapper.GetWaferID(strTemp);
					INT nIndex = strTemp.ReverseFind('.');
					if ( nIndex != -1 )
					{
						strTemp = strTemp.Left(nIndex);
					}
				}
				pFileLog.WriteString("Begin "+strTemp+"\n");
			}
			else	//Write the rest error found
			{
				bOutput = pFileLog.Open(strLocalFileNmLog, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText);

				if ( !bOutput ) 
				{
					strTemp.Format("Fails to open SDS file.");
					SetHmiMessage(strTemp, "Empty Die Check", glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
					SetErrorLogMessage("Fails to open SDS file");
					SetErrorLogMessage("Auto Align FAIL!\n");
					return FALSE;
				}

				pFileLog.SeekToEnd();
			}

			//Write Data
			LONG lUserCol, lUserRow;
			m_stnWaferTable->ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);

			strTemp.Format("%3d,%3d,%3d,%3d\n", lUserCol, lUserRow, i, j);
			pFileLog.WriteString(strTemp);
			pFileLog.Close();

			if ( CopyFile(strLocalFileNmLog, strFileNmLog, FALSE) == FALSE)
			{
				SetErrorLogMessage("Fail to copy SDS file to the network path! Please Check!");
			}

			if( m_ulBlk2EmptyGradeMinCount==0 )
			{
				strTemp.Format("Error! Found normal die!");
				lTmp = SetHmiMessage(strTemp, "Empty Die Check", glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
				if ( lTmp == glHMI_STOP)
				{
					SetLogCtmMessage("Normal die is found and User chooses STOP!");
					SetLogCtmMessage("Auto Align FAIL!\n");
					return FALSE;
				}
				else
				{
					SetLogCtmMessage("Normal die is found and User chooses CONTINUE!");
				}
			}
			Sleep(500);
			if (ulErrorCount>20)
			{
				break;
			}
		}
		if (ulErrorCount>20)
		{
			break;
		}
	}

	if( m_ulBlk2EmptyGradeMinCount>0 && ulOkCount<m_ulBlk2EmptyGradeMinCount && ulErrorCount>0 )
	{
		return FALSE;
	}
	strTemp.Format("No of Grade 17 Die found = %d", ulLoop);
	SetLogCtmMessage(strTemp);
	SetLogCtmMessage("Auto Align FINISH!\n");

	return TRUE;
}

BOOL CWT_BlkFunc2::PrintFDC2()
{
	int		i,j;
	unsigned long ulIndex;
	CString szMsg;

	szMsg.Format("Refer Num, %d", GetMapReferNum());
	SetFDCRegionLog(szMsg);
	szMsg.Format("No. , AsmRowCol,OCR,IM,IW, PhyPos");
	SetFDCRegionLog(szMsg);

	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			szMsg.Format("%04d, (%03d;%03d), %02d, %d, %d, (%d;%d)",
					ulIndex,
					GetReferMapPosRow(ulIndex), GetReferMapPosCol(ulIndex),
					GetReferMapPosValue(ulIndex),
					GetReferInMap(ulIndex),
					GetReferInWfa(ulIndex),
					GetReferDiePhyX(ulIndex),	GetReferDiePhyY(ulIndex) );
			SetFDCRegionLog(szMsg);
		}
	}

	szMsg.Format("FDC Pitch(%d,%d), DiePitchH(%d,%d), DiePitchV(%d,%d)", GetBlkPitchRow(), GetBlkPitchCol(),
		GetDiePitchHX(), GetDiePitchHY(), GetDiePitchVX(), GetDiePitchVY());
	SetFDCRegionLog(szMsg);

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2SetBondingLastDie()
{
	SetMapAlgorithmPrm("Row", m_lBlk2MaxJumpCtr);
	SetMapAlgorithmPrm("Col", m_lBlk2MaxJumpEdge);
	SetMapAlgorithmPrm("Align", REGN_INIT2);
	m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);

	SetMapAlgorithm();

	return TRUE;
}

BOOL CWT_BlkFunc2::SetMapAlgorithm()
{
	SetMapAlgorithmPrm("Row", 0);
	SetMapAlgorithmPrm("Col", 0);
	SetMapAlgorithmPrm("Align", REGN_EMPTY2);

	return TRUE;
}

VOID CWT_BlkFunc2::SetMapAlgorithmPrm(LPCTSTR strPrm, CONST LONG lPrm)
{
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter(strPrm, lPrm);
}

LONG CWT_BlkFunc2::Blk2IndexToNormalDie(DieMapPos2 SrcDie, DieMapPos2 TgtDie, DiePhyPos2 *CurPhyPos, INT iMissCount)
{
	DieMapPos2	CurDie;
	DiePhyPos2	CurPhy;
	LONG		lPosX, lPosY, lPosT;
	LONG		lDirRow, lDirCol;
	LONG		lResult;
	INT			iStop = 0;

	// Init Status
	CurDie = SrcDie;
	CurPhy.x = CurPhyPos->x;
	CurPhy.y = CurPhyPos->y;
 	if ( SrcDie.Row <= TgtDie.Row )
		lDirRow = 1;
	else
		lDirRow = -1;
 	if ( SrcDie.Col <= TgtDie.Col )
		lDirCol = 1;
	else
		lDirCol = -1;

	// Move
	while ( CurDie.Row != TgtDie.Row || CurDie.Col != TgtDie.Col )
	{
		if (IsToStopAlign())
		{
			return FALSE;
		}

 		if ( CurDie.Row != TgtDie.Row )
		{
			CurDie.Row = CurDie.Row + m_lBlk2MaxJumpEdge*lDirRow;
			if ( lDirRow == 1 && CurDie.Row > TgtDie.Row )
				CurDie.Row = TgtDie.Row;
			if ( lDirRow == -1 && CurDie.Row < TgtDie.Row )
				CurDie.Row = TgtDie.Row;
		}
 		if ( CurDie.Col != TgtDie.Col )
		{
			CurDie.Col = CurDie.Col + m_lBlk2MaxJumpEdge*lDirCol;
			if ( lDirCol == 1 && CurDie.Col > TgtDie.Col )
				CurDie.Col = TgtDie.Col;
			if ( lDirCol == -1 && CurDie.Col < TgtDie.Col )
				CurDie.Col = TgtDie.Col;
		}

		Blk2GetNextDiePosn(CurDie, &CurPhy);
		if (CheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
		{
			return FALSE; //WT_ALN_OUT_WAFLIMIT;
		}
		MoveXYTo(CurPhy.x,CurPhy.y);
		Sleep(m_lPrSrchDelay);
		m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(CurDie.Row,CurDie.Col);

		lPosX = CurPhy.x;
		lPosY = CurPhy.y;
		lResult = Blk2GetNormalDieStatus(&lPosX, &lPosY);
		Blk2GetWTPosition(&lPosX, &lPosY, &lPosT);
		if (lResult == WT_ALN_IS_DEFECT || lResult == WT_ALN_IS_GOOD)
		{
			Blk2SetCurDiePhyPos(CurDie.Row, CurDie.Col, lPosX, lPosY, REGN_ALIGN2);
			iStop = 0;
		}
		else
		{
			CurPhy.x = lPosX;
			CurPhy.y = lPosY;
			m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(CurDie.Row,CurDie.Col);
			Sleep(100);
			iStop++;
			if (iStop == iMissCount && iMissCount != 0) return TRUE;
		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2GetEdgePosition()
{
	INT	i,j;
	BOOL	iUp,iDn,iLft,iRgt;
	DieMapPos2 RefDie, RefDieMap;
	DiePhyPos2 CurPhy;
	ULONG	ulIndex;

	iUp=iDn=iLft=iRgt=FALSE;

	//Index
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex)==FALSE ) continue;
			if (GetReferInWfa(ulIndex) == 0) continue;
			RefDie.Row = i;
			RefDie.Col = j;
			Blk2HaveRefdieOn4Dir(RefDie,&iUp,&iDn,&iLft,&iRgt);

			if (iUp == FALSE)
			{
				RefDieMap.Row = GetReferMapPosRow(ulIndex);
				RefDieMap.Col = GetReferMapPosCol(ulIndex);
				CurPhy.x = GetReferDiePhyX(ulIndex);
				CurPhy.y = GetReferDiePhyY(ulIndex);
				Blk2IndexToEdgeDie(RefDieMap, &CurPhy, BLK_REFER_UP);
			}

			if (iLft == FALSE)
			{
				RefDieMap.Row = GetReferMapPosRow(ulIndex);
				RefDieMap.Col = GetReferMapPosCol(ulIndex);
				CurPhy.x = GetReferDiePhyX(ulIndex);
				CurPhy.y = GetReferDiePhyY(ulIndex);
				Blk2IndexToEdgeDie(RefDieMap, &CurPhy, BLK_REFER_LEFT);
			}

			if (iRgt == FALSE)
			{
				RefDieMap.Row = GetReferMapPosRow(ulIndex);
				RefDieMap.Col = GetReferMapPosCol(ulIndex);
				CurPhy.x = GetReferDiePhyX(ulIndex);
				CurPhy.y = GetReferDiePhyY(ulIndex);
				Blk2IndexToEdgeDie(RefDieMap, &CurPhy, BLK_REFER_RIGHT);
			}

			if (iDn == FALSE)
			{
				RefDieMap.Row = GetReferMapPosRow(ulIndex);
				RefDieMap.Col = GetReferMapPosCol(ulIndex);
				CurPhy.x = GetReferDiePhyX(ulIndex);
				CurPhy.y = GetReferDiePhyY(ulIndex);
				Blk2IndexToEdgeDie(RefDieMap, &CurPhy, BLK_REFER_DOWN);
			}

			if (IsToStopAlign())
			{
				return FALSE;
			}

		}
	}

	return TRUE;
}

LONG CWT_BlkFunc2::Blk2IndexToEdgeDie(DieMapPos2 SrcDie, DiePhyPos2 *CurPhyPos, LONG lDir)
{
	DieMapPos2	CurDie, TgtDie;
	LONG		lResult;

	ULONG		i,j;
	ULONG		ulRowNum=0, ulColNum=0;
	UCHAR		ucGrade;

	// Init Status
	CurDie = SrcDie;
	MoveXYTo(CurPhyPos->x,CurPhyPos->y);
	Sleep(m_lPrSrchDelay);

	if (!m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulRowNum,ulColNum))
	{
		return FALSE;
	}
	if (m_stnWaferTable->IsOutMS90SortingPart(ulRowNum, ulColNum))
	{
		if (m_stnWaferTable->IsRowModeSeparateHalfMap())
		{
			ulRowNum = m_stnWaferTable->GetMS90HalfMapMaxRow();
		}
		else
		{
			//ulColNum = m_stnWaferTable->GetMS90HalfMapMaxCol();
		}
	}
	lResult = FALSE;
 	if ( lDir == BLK_REFER_UP )
	{
		j = CurDie.Col;
		for (i=0; i<ulRowNum; i++)
		{
			ucGrade = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetDieInformation(i,j);
			if ( ucGrade != (UCHAR)(m_stnWaferTable->m_WaferMapWrapper.GetNullBin()) )
			{
				TgtDie.Row = i;
				TgtDie.Col = j;
				lResult = TRUE;
				break;
			}
		}
	}

 	if ( lDir == BLK_REFER_DOWN )
	{
		j = CurDie.Col;
		ULONG ii = 0;
		for(ii=0; ii<=ulRowNum-1; ii++)
		{
			i = ulRowNum-1 - ii;
			ucGrade = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetDieInformation(i,j);
			if ( ucGrade != (UCHAR)(m_stnWaferTable->m_WaferMapWrapper.GetNullBin()) )
			{
				TgtDie.Row = i;
				TgtDie.Col = j;
				lResult = TRUE;
				break;
			}
		}
	}
 	
	if ( lDir == BLK_REFER_LEFT )
	{
		i = CurDie.Row;
		for (j=0; j<ulColNum; j++)
		{
			ucGrade = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetDieInformation(i,j);
			if ( ucGrade != (UCHAR)(m_stnWaferTable->m_WaferMapWrapper.GetNullBin()) )
			{
				TgtDie.Row = i;
				TgtDie.Col = j;
				lResult = TRUE;
				break;
			}
		}
	}
	
	if ( lDir == BLK_REFER_RIGHT )
	{
		i = CurDie.Row;
		ULONG ii = 0;
		for(ii=0; ii<=ulColNum-1; ii++)
		{
			j = ulColNum-1-ii;
			ucGrade = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetDieInformation(i,j);
			if ( ucGrade != (UCHAR)(m_stnWaferTable->m_WaferMapWrapper.GetNullBin()) )
			{
				TgtDie.Row = i;
				TgtDie.Col = j;
				lResult = TRUE;
				break;
			}
		}
	}

	// Move
	if (lResult == TRUE)
	{
		Blk2IndexToNormalDie(CurDie, TgtDie, CurPhyPos, 2);
	}

	return TRUE;
}


BOOL CWT_BlkFunc2::Blk2HaveRefdieOn4Dir(DieMapPos2 iRefCtr, BOOL *iUp, BOOL *iDn, BOOL *iLt, BOOL *iRt)
{
	INT		i,j;
	ULONG	ulIndex = 0;	//Klocwork

	//Init
	*iUp = FALSE;
	*iDn = FALSE;
	*iLt = FALSE;
	*iRt = FALSE;

	//Hori
	i = iRefCtr.Row;
	for (j=iRefCtr.Col-1; j>=GetBlkMinColIdx(); j--)
	{
		ulIndex = i*MAXREFERWIDTH+j;
		if ((LONG)ulIndex < 0 || ulIndex >= MAXFDC2NUM )	//bound the array to prevent software crash
			continue;
		if (GetReferInWfa(ulIndex)==1)
		{
			*iLt = TRUE;
			break;
		}
	}
	for (j=iRefCtr.Col+1; j<=GetBlkMaxColIdx(); j++)
	{
		ulIndex = i*MAXREFERWIDTH+j;
		if (GetReferInWfa(ulIndex)==1)
		{
			*iRt = TRUE;
			break;
		}
	}

	//Vert
	j = iRefCtr.Col;
	for (i=iRefCtr.Row-1; i>=GetBlkMinRowIdx(); i--)
	{
		if ((LONG)ulIndex < 0 || ulIndex >= MAXFDC2NUM )	//bound the array to prevent software crash
			continue;
			
		ulIndex = i*MAXREFERWIDTH+j;
		ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork

		if (GetReferInWfa(ulIndex)==1)
		{
			*iUp = TRUE;
			break;
		}
	}
	for (i=iRefCtr.Row+1; i<=GetBlkMaxRowIdx(); i++)
	{
		ulIndex = i*MAXREFERWIDTH+j;
		if (GetReferInWfa(ulIndex)==1)
		{
			*iDn = TRUE;
			break;
		}
	}

	return TRUE;
}


BOOL CWT_BlkFunc2::Blk2RealignRefDiePos(BOOL bEdge)
{
	INT		i,j;
	LONG	lX, lY, lT;
	LONG	lDig1, lDig2;
	DiePhyPos2 CurPhy;
	ULONG	ulIndex;
	BOOL	bResult, bGetPstn;

	CString		szText1 = "Continue";
	CString		szText2 = "Skip";
	CString		szText3 = "STOP";
	LONG lTmp;

	bGetPstn = FALSE;

	if( GetBlkAligned()==FALSE )
	{
		return TRUE;
	}

	for (i=0; i<MAPMAXROW2; i++)
	{
		for (j=0; j<MAPMAXCOL2; j++)
		{
			m_stDiePhyPrep[i][j].m_lDieX = 0;
			m_stDiePhyPrep[i][j].m_lDieY = 0;
			UpdatePrepDieStatus(i, j);
		}
	}

	if (IsToStopAlign())
	{
		return FALSE;
	}

	// Move theta home
	CString	strTemp;
	m_stnWaferTable->GetRtnForTMoveTo(GetHomeTheta(), SFM_WAIT);

	//Search RefDie
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for(j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			if (IsToStopAlign())
			{
				return FALSE;
			}

			if( i%2==0 )
				ulIndex = i*MAXREFERWIDTH + j;
			else
				ulIndex = i*MAXREFERWIDTH + GetBlkMaxColIdx() + GetBlkMinColIdx() - j;

			if ( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}

			if (GetReferInWfa(ulIndex) == 1)
			{
				CurPhy.x = GetReferDiePhyX(ulIndex);
				CurPhy.y = GetReferDiePhyY(ulIndex);
			}
			else
			{
				Blk2GetNextDiePosn(m_stReferWftInfo[ulIndex].m_MapPos, &CurPhy);
			}

			if (CheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
			{
				continue;
			}
			MoveXYTo(CurPhy.x,CurPhy.y);
			
			Sleep(m_lPrSrchDelay);

			// Align RefDie
			Blk2GetWTPosition(&lX,&lY,&lT);

			bResult = Blk2SearchReferDieInFOV( &lX, &lY, &lDig1, &lDig2);
			if ( bResult == FALSE )
			{
				Sleep(200);
				bResult = Blk2SearchReferDieInFOV( &lX, &lY, &lDig1, &lDig2);
			}

			if ( bResult == TRUE )
			{
				bGetPstn = TRUE;
				CurPhy.x = lX;
				CurPhy.y = lY;

				bResult = MatchRefDigitFromMap(ulIndex, lDig1, lDig2);
				if (bResult == FALSE)
				{
					strTemp.Format("Please check:\nThis reference die should be No.%d.\nContinue or Stop?\n\n\n\nOn PRS(%d%d)", GetReferMapPosValue(ulIndex), lDig1, lDig2);
					lTmp = SetHmiMessage(strTemp, "Match Digital Warning", glHMI_MBX_TRIPLEBUTTON, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, &szText1, &szText2, &szText3);

					if (lTmp == 8)
					{
						return FALSE;
					}

					if (lTmp == 5) 
					{
						m_stReferWftInfo[ulIndex].m_cInWaf = 0;
						SetReferDiePrepStatus(ulIndex, REGN_INIT2);
						continue;
					}
				}

				//Log for customer Osram
				strTemp.Format("No.%2d, Phy(%6d,%6d), PhyUpd(%6d,%6d), Offset(%4d,%4d)",
					GetReferMapPosValue(ulIndex),
					GetReferDiePhyX(ulIndex),   GetReferDiePhyY(ulIndex),
					CurPhy.x, CurPhy.y,
					GetReferDiePhyX(ulIndex) - CurPhy.x,
					GetReferDiePhyY(ulIndex) - CurPhy.y);
				SetLogCtmMessage(strTemp);

				SetReferWftInfo(ulIndex, CurPhy.x, CurPhy.y);
				SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);
			}
			else
			{
				m_stReferWftInfo[ulIndex].m_cInWaf = 0;
				SetReferDiePrepStatus(ulIndex, REGN_INIT2);
			}
		}
	}

	SetFDCRegionLog("\n");
	SetFDCRegionLog("after realign FDC position");
	PrintFDC2();

	if ( !bGetPstn )
	{
		return FALSE; 
	}

	if ( bEdge && m_bBlk2MnEdgeAlign )
	{
		Blk2GetEdgePosition();
	}

	DieMapPos2 CurMap;
	LONG lDigital;
	if( GetUserMapCenterDie(&CurMap, &CurPhy, &lDigital) )
	{
		MoveXYTo(CurPhy.x,CurPhy.y);
	}
	
	Sleep(m_lPrSrchDelay);

	Blk2SetBondingLastDie();

	return TRUE;
}

BOOL CWT_BlkFunc2::CheckRelativePstnInFDC()
{
	INT		i,j;
	DiePhyPos2 CurrPhy, NearPhy;
	DieMapPos2 CurrMap;
	ULONG	ulIndex, ulDist, ulHDist, ulVDist, ulNearIdx;
	CString	strTemp;

	//Search RefDie
	ulHDist = (ULONG) GetDistIn2Points(GetBlkPitchHX(), GetBlkPitchHY(), 0, 0);
	ulVDist = (ULONG) GetDistIn2Points(GetBlkPitchVX(), GetBlkPitchVY(), 0, 0);

	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}
			if (GetReferInWfa(ulIndex) == 0)
			{
				continue;
			}

			CurrPhy.x = GetReferDiePhyX(ulIndex);
			CurrPhy.y = GetReferDiePhyY(ulIndex);
			CurrMap.Row = GetReferSrcMapRow(ulIndex);
			CurrMap.Col = GetReferSrcMapCol(ulIndex);

			if ( j!=GetBlkMinColIdx() )
			{
				ulNearIdx = i*MAXREFERWIDTH + j -1;
				if( GetReferInMap(ulNearIdx) && GetReferInWfa(ulNearIdx) != 0 )
				{
					NearPhy.x = GetReferDiePhyX(ulNearIdx);
					NearPhy.y = GetReferDiePhyY(ulNearIdx);
					ulDist = (ULONG) GetDistIn2Points(CurrPhy.x, CurrPhy.y, NearPhy.x, NearPhy.y);
					if ( labs(ulDist - ulHDist) > ulHDist*0.5 )
					{
						strTemp.Format("Position shift in reference die to left.\n(%d,%d)---(%d,%d)\n(%d,%d)---(%d,%d)\n\nPlease do alignment again.\n(%d,%d)",
							CurrMap.Row, CurrMap.Col, CurrPhy.x, CurrPhy.y,
							0, 0, NearPhy.x, NearPhy.y, ulDist, ulHDist);
						SetHmiMessage(strTemp, "Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
						return FALSE;
					}
				}
			}

			if ( j!=GetBlkMaxColIdx() )
			{
				ulNearIdx = i*MAXREFERWIDTH + j + 1;
				if(  GetReferInMap(ulNearIdx) && GetReferInWfa(ulNearIdx) != 0 )
				{
					NearPhy.x = GetReferDiePhyX(ulNearIdx);
					NearPhy.y = GetReferDiePhyY(ulNearIdx);
					ulDist = (ULONG) GetDistIn2Points(CurrPhy.x, CurrPhy.y, NearPhy.x, NearPhy.y);
					if ( labs(ulDist - ulHDist) > ulHDist*0.5 )
					{
						strTemp.Format("Position shift in reference die to right.\n(%d,%d)---(%d,%d)\n(%d,%d)---(%d,%d)\n\nPlease do alignment again.\n(%d,%d)",
							CurrMap.Row, CurrMap.Col, CurrPhy.x, CurrPhy.y,
							0, 0, NearPhy.x, NearPhy.y, ulDist, ulHDist );
						SetHmiMessage(strTemp, "Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
						SetFDCRegionLog(strTemp);
						return FALSE;
					}
				}
			}

			if ( i!=GetBlkMinRowIdx() )
			{
				ulNearIdx = (i-1)*MAXREFERWIDTH + j;
				if( GetReferInMap(ulNearIdx) && GetReferInWfa(ulNearIdx) != 0 )
				{
					NearPhy.x = GetReferDiePhyX(ulNearIdx);
					NearPhy.y = GetReferDiePhyY(ulNearIdx);
					ulDist = (ULONG) GetDistIn2Points(CurrPhy.x, CurrPhy.y, NearPhy.x, NearPhy.y);
					if ( labs(ulDist - ulVDist) > ulVDist*0.5 )
					{
						strTemp.Format("Position shift in reference die to up.\n(%d,%d)---(%d,%d)\n(%d,%d)---(%d,%d)\n\nPlease do alignment again.\n(%d,%d)",
							CurrMap.Row, CurrMap.Col, CurrPhy.x, CurrPhy.y,
							0, 0, NearPhy.x, NearPhy.y, ulDist,ulVDist );
						SetHmiMessage(strTemp, "Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
						return FALSE;
					}
				}
			}

			if ( i!=GetBlkMaxRowIdx() )
			{
				ulNearIdx = (i+1)*MAXREFERWIDTH + j;
				if(  GetReferInMap(ulNearIdx) && GetReferInWfa(ulNearIdx) != 0 )
				{
					NearPhy.x = GetReferDiePhyX(ulNearIdx);
					NearPhy.y = GetReferDiePhyY(ulNearIdx);
					ulDist = (ULONG) GetDistIn2Points(CurrPhy.x, CurrPhy.y, NearPhy.x, NearPhy.y);
					if ( labs(ulDist - ulVDist) > ulVDist*0.5 )
					{
						strTemp.Format("Position shift in reference die to down.\n(%d,%d)---(%d,%d)\n(%d,%d)---(%d,%d)\n\nPlease do alignment again.\n(%d,%d)",
							CurrMap.Row, CurrMap.Col, CurrPhy.x, CurrPhy.y,
							0, 0, NearPhy.x, NearPhy.y, ulDist,ulVDist );
						SetHmiMessage(strTemp, "Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
						return FALSE;
					}
				}
			}

		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::MatchRefDigitFromMap(ULONG ulIndex, LONG lDig1, LONG lDig2)
{
	if (Blk2IsSrchCharDie())
	{
		LONG lMapDig1, lMapDig2;
		lMapDig1 = GetReferMapPosValue(ulIndex) / 10;
		lMapDig2 = GetReferMapPosValue(ulIndex) % 10;

		if (lMapDig1 == lDig1 && lMapDig2 == lDig2)
			return TRUE;
		else
			return FALSE;
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2IsReferDie(ULONG ulRow, ULONG ulCol, INT *iDigit1, INT *iDigit2)
{
	LONG	i,j;
	ULONG	ulIndex, ulRefValue, ulReferRow, ulReferCol;

	LONG lDir = 0;
	for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;

			if ( GetReferInMap(ulIndex) && GetReferInWfa(ulIndex) == 1)
			{
				ulReferRow = GetReferMapPosRow(ulIndex);
				ulReferCol = GetReferMapPosCol(ulIndex);
				if (ulReferRow == ulRow && ulReferCol == ulCol)
					lDir = 1;
				else if (ulReferRow == ulRow && ulReferCol == ulCol-1)
					lDir = 2;
				else if (ulReferRow == ulRow-1 && ulReferCol == ulCol-1)
					lDir = 3;
				else if (ulReferRow == ulRow-1 && ulReferCol == ulCol)
					lDir = 4;
				else if (ulReferRow == ulRow-1 && ulReferCol == ulCol+1)
					lDir = 5;
				else if (ulReferRow == ulRow && ulReferCol == ulCol+1)
					lDir = 6;
				else if (ulReferRow == ulRow+1 && ulReferCol == ulCol+1)
					lDir = 7;
				else if (ulReferRow == ulRow+1 && ulReferCol == ulCol)
					lDir = 8;
				else if (ulReferRow == ulRow+1 && ulReferCol == ulCol-1)
					lDir = 9;

				if (lDir == 0)
					continue;

				if (m_stnWaferTable->m_WaferMapWrapper.HasReferenceFaceValue(ulReferRow, ulReferCol) == TRUE)
				{
					ulRefValue = m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieFaceValue(ulReferRow, ulReferCol);
					*iDigit1 = ulRefValue / 10;
					*iDigit2 = ulRefValue % 10;
				}
				else
				{
					*iDigit1 = -1;
					*iDigit2 = -1;
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL CWT_BlkFunc2::Blk2CheckCOR()
{
	LONG lX,lY,lT;
	LONG lX0,lY0;
	LONG lDiffUmX, lDiffUmY;
	BOOL bResult;
	CString	strTemp;

	m_stnWaferTable->DisplayRtMessage("Start wafer COR checking...");

	LONG lOldX=0,lOldY=0,lOldT=0;
	Blk2GetWTPosition(&lOldX, &lOldY, &lOldT);

	m_stnWaferTable->CheckCOR(5.0);			//v3.32T6
	Sleep(500);		//1000);				//v3.32T6
	Blk2GetWTPosition(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	bResult = Blk2SearchReferOnlyInFOV( &lX, &lY, FALSE);

	MoveXYTo(lOldX, lOldY);
	m_stnWaferTable->GetRtnForTMoveTo(lOldT, SFM_WAIT);
	Sleep(m_lPrSrchDelay);

	lDiffUmX = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lY-lY0));
	if ( bResult == FALSE )		//v3.32T6
	{
		strTemp.Format("Check COR failure(1) no number-die found!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}

	if ( m_stnWaferTable->CheckCOROffset((lX-lX0), (lY-lY0))==FALSE )
	{
		strTemp.Format("Check COR failure(1)!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}

	strTemp.Format("Check COR (1) PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
	SetLogCtmMessage(strTemp);

	//Rotate back and check
	Blk2GetWTPosition(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	bResult = Blk2SearchReferOnlyInFOV( &lX, &lY);

	lDiffUmX = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lY-lY0));
	if (bResult==FALSE)			//v3.32T6
	{
		strTemp.Format("Check COR failure(2) no number-die found!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}

	if ( m_stnWaferTable->CheckCORTolerance((lX-lX0), (lY-lY0))==FALSE )
	{
		strTemp.Format("Check COR failure(2)!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}
	strTemp.Format("Check COR (2) PR=%d, XY_Err(%d,%d)", bResult, lDiffUmX, lDiffUmY );
	SetLogCtmMessage(strTemp);


	m_stnWaferTable->CheckCOR(-5.0);
	Sleep(500);
	Blk2GetWTPosition(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	bResult = Blk2SearchReferOnlyInFOV( &lX, &lY, FALSE);

	MoveXYTo(lOldX, lOldY);
	m_stnWaferTable->GetRtnForTMoveTo(lOldT, SFM_WAIT);
	Sleep(m_lPrSrchDelay);
	
	lDiffUmX = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lY-lY0));
	if ( bResult == FALSE)		//andrewng
	{
		strTemp.Format("Check COR failure(3) no number-die found!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}

	if( m_stnWaferTable->CheckCOROffset((lX-lX0), (lY-lY0))==FALSE )
	{
		strTemp.Format("Check COR failure(3)!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}
	strTemp.Format("Check COR (3) PR=%d, XY_Err(%d,%d)", bResult, lDiffUmX, lDiffUmY );
	SetLogCtmMessage(strTemp);

	//Rotate back and check
	Blk2GetWTPosition(&lX,&lY,&lT);
	lX0=lX; lY0=lY;
	bResult = Blk2SearchReferOnlyInFOV( &lX, &lY);

	lDiffUmX = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lX-lX0));
	lDiffUmY = (LONG)(m_stnWaferTable->ConvertMotorStepToUnit(lY-lY0));
	if (bResult == FALSE)	//v3.32T6
	{
		strTemp.Format("Check COR failure(4) no number-die found!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}

	if ( m_stnWaferTable->CheckCORTolerance((lX-lX0), (lY-lY0))==FALSE )	//Limit 2 mil
	{
		strTemp.Format("Check COR failure(4)!");
		SetLogCtmMessage(strTemp);
		strTemp.Format("PR=%d, XY_Err(%d,%d)",	bResult, lDiffUmX, lDiffUmY );
		SetLogCtmMessage(strTemp);
		return FALSE;
	}
	strTemp.Format("Check COR (4) PR=%d, XY_Err(%d,%d)", bResult, lDiffUmX, lDiffUmY );
	SetLogCtmMessage(strTemp);

	strTemp.Format("Check COR --> OK!");
	SetLogCtmMessage(strTemp);

	return TRUE;
}


BOOL CWT_BlkFunc2::Blk2UpdateRefDieOffset(LONG lOffsetX, LONG lOffsetY, LONG lRow, LONG lCol)
{
	LONG lStartRow	= lRow - GetBlkPitchRow() / 2;
	LONG lStartCol	= lCol - GetBlkPitchCol() / 2;
	LONG lCurrRow	= lStartRow;
	LONG lCurrCol	= lStartCol;
	LONG lX0, lY0;

	CString szMsg;
	szMsg.Format("BLK2 refer offset (%d,%d) at map (%d,%d)", lOffsetX, lOffsetY, lRow, lCol);
	DisplayDebugString(szMsg);

	for (int i=0; i<=GetBlkPitchRow(); i++)
	{
		lCurrRow = lStartRow + i;

		for (int j=0; j<=GetBlkPitchCol(); j++)
		{
			lCurrCol = lStartCol + j;

			if( (lCurrRow < 0) || (lCurrCol < 0) )
			{
				continue;
			}
			if ((lCurrRow == lRow) && (lCurrCol == lCol))
			{
				continue;
			}
			if(	(GetPrepDieStatus(lCurrRow, lCurrCol) != REGN_ALIGN2) && 
				(GetPrepDieStatus(lCurrRow, lCurrCol) != REGN_HPICK2) ) 
			{
				continue;
			}

			lX0 = GetPrepDieWftX(lCurrRow, lCurrCol) + lOffsetX;
			lY0 = GetPrepDieWftY(lCurrRow, lCurrCol) + lOffsetY;

			m_stDiePhyPrep[lCurrRow][lCurrCol].m_lDieX = lX0;
			m_stDiePhyPrep[lCurrRow][lCurrCol].m_lDieY = lY0;
		}
	}

	return TRUE;
}

//Re-verify Reference Position at wafer end
BOOL CWT_BlkFunc2::Blk2ReVerifyRefDiePos()
{
	INT			i,j;
	LONG		lX, lY, lT;
	DiePhyPos2	CurPhy;
	ULONG		ulIndex;
	BOOL		bResult;

	CString		strFiducialsLog, strLocalFiducialsLog;
	BOOL		bOutput=FALSE;
	CStdioFile	pFileLog;
	LONG		lUserCol=0, lUserRow=0;
	LONG		lFiducialsFound = 0;
	CString	strTemp;

	if( GetBlkAligned()==FALSE )
	{
		return FALSE;
	}

	BOOL bGenOutput = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="Cree" )
		bGenOutput = TRUE;

	SetLogCtmMessage("Start re-verify reference dice before wafer end.");

	// Get the reply for PR result
	m_stnWaferTable->SetAutoBondMode(FALSE);

	// Move theta home
	m_stnWaferTable->GetRtnForTMoveTo(GetHomeTheta(), SFM_WAIT);

	CString strMachineID;
	m_stnWaferTable->GetRtnForMachineNo(&strMachineID);
	if( bGenOutput )
	{
		//Get Output Path
		CString	strPathBin, strPathLog;
		strLocalFiducialsLog = "C:\\MapSorter\\OutputFiles\\Fiducials.txt";
		bOutput = Blk2GetIniOutputPath(&strPathBin, &strPathLog);
		if ( !bOutput ) 
		{
			strTemp.Format("Fails to initial Fiducials log file path. Files will only be generated to local drive!");
			SetErrorLogMessage(strTemp);
		}
		else
		{
			strFiducialsLog.Format("%s\\Fiducials.txt", strPathLog);
		}
	}

	//Search RefDie
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for(j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			if( i%2==0 )
				ulIndex = i*MAXREFERWIDTH + j;
			else
				ulIndex = i*MAXREFERWIDTH + GetBlkMaxColIdx() + GetBlkMinColIdx() - j;

			if ( GetReferInMap(ulIndex)==FALSE )
				continue;
			if (GetReferInWfa(ulIndex) == 0)
				continue;

			if (GetReferInWfa(ulIndex) == 1)
			{
				CurPhy.x = GetReferDiePhyX(ulIndex);
				CurPhy.y = GetReferDiePhyY(ulIndex);
			}
			else
			{
				Blk2GetNextDiePosn(m_stReferWftInfo[ulIndex].m_MapPos, &CurPhy);
			}

			if (CheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
			{
				continue;
			}

			MoveXYTo(CurPhy.x,CurPhy.y);
			Sleep(m_lPrSrchDelay);

			// Align RefDie
			Blk2GetWTPosition(&lX,&lY,&lT);

			bResult = Blk2SearchReferOnlyInFOV( &lX, &lY);
			if ( bResult == FALSE )
			{
				Sleep(200);
				bResult = Blk2SearchReferOnlyInFOV( &lX, &lY);
			}

			if ( bResult == FALSE )
			{
				lFiducialsFound++;

				if(lFiducialsFound == 1 && bGenOutput)
				{
					bOutput = pFileLog.Open(strLocalFiducialsLog, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText);
					if ( !bOutput )
					{
						SetErrorLogMessage("Fails to open Fiducials Log file");
					}
				
					pFileLog.SeekToEnd();
					
					//Write Header
					pFileLog.WriteString("Initial File Name, \n");
					pFileLog.WriteString("Bin Block Unload Date, \n");
					pFileLog.WriteString("Bin Block Unload Time, \n");
					pFileLog.WriteString("Bin Block, \n");
					strTemp.Format("Sorter, %s\n", strMachineID);
					pFileLog.WriteString(strTemp);
					//Average die rotation
					pFileLog.WriteString("Average die rotation, 0.00\n");
					//Number of visual defects
					pFileLog.WriteString("Number of visual defects, 0\n");
					//Number of Good die
					pFileLog.WriteString("Number of Good die, 0\n");
					//Package File Name
					pFileLog.WriteString("Package File Name, \n");
					pFileLog.WriteString("\nNumerical data is as follows: WaferX, WaferY, DiesheetX, DieSheetY\n\n");
					//Application sw version
					m_stnWaferTable->GetRtnForSwVerNo(&strTemp);
					pFileLog.WriteString(strTemp+"\n");

					//Map file name
					m_stnWaferTable->GetRtnForMapFileNm(&strTemp);

					//Ensure the WaferID is shown in the file
					if (strTemp.IsEmpty() == TRUE)
					{
						m_stnWaferTable->m_WaferMapWrapper.GetWaferID(strTemp);

						INT nIndex = strTemp.ReverseFind('.');
							
						if ( nIndex != -1 )
						{
							strTemp = strTemp.Left(nIndex);
						}
					}

					pFileLog.WriteString("Begin "+strTemp+"\n");

					pFileLog.Close();
				}

				if( bGenOutput )
				{
					bOutput = pFileLog.Open(strLocalFiducialsLog, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText);
					if ( !bOutput )
					{
						SetErrorLogMessage("Fails to open Fiducials Log file");
					}
				
					pFileLog.SeekToEnd();
					lUserRow = GetReferSrcMapRow(ulIndex);
					lUserCol = GetReferSrcMapCol(ulIndex);

					strTemp.Format("%3d,%3d,%3d,%3d\n",
							lUserCol, lUserRow,
							GetReferMapPosCol(ulIndex), GetReferMapPosRow(ulIndex));
					pFileLog.WriteString(strTemp);

					pFileLog.Close();

					if ( CopyFile(strLocalFiducialsLog, strFiducialsLog, FALSE) == FALSE)
					{
						SetErrorLogMessage("Fail to copy Fiducials Log file to the network path! Please Check!");
					}
				}
				
				strTemp.Format("Fails to re-verify ref die at (%3d, %3d)!\nContinue or Stop?", lUserCol, lUserRow);
				SetErrorLogMessage(strTemp);
				LONG lTmp = 0;
				if( m_stnWaferTable->IsPrescanBlkPick() )
				{
					lTmp = m_stnWaferTable->HmiMessageEx_Timeout_Red_Back(strTemp, "Re-verify Ref Die Warning", glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
				}
				else
				{
					lTmp = SetHmiMessage(strTemp, "Re-verify Ref Die Warning", glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
				}

				if ( lTmp == glHMI_CONTINUE)
				{
					SetErrorLogMessage("Re-verify Fiducials fails but User chooses CONTINUE!");
				}
				else if ( lTmp == glHMI_STOP)
				{
					SetErrorLogMessage("Re-verify Fiducials fails and User chooses STOP!");

					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

// prescan relative code	B
BOOL CWT_BlkFunc2::Blk2GetRefDiePosn(ULONG ulIndex, LONG &lMapRow, LONG &lMapCol, LONG &lTableX, LONG &lTableY)
{
	int	iRow, iCol, iRefIdx;
	ULONG ulMapIdx, ulWidth, ulHeight;

	ulWidth  = (GetBlkMaxColIdx()-GetBlkMinColIdx()+1);
	ulHeight = (GetBlkMaxRowIdx()-GetBlkMinRowIdx()+1);
	// from 1 to max no of row in this map
	for(iRow=GetBlkMinRowIdx(); iRow<=GetBlkMaxRowIdx(); iRow++)
	{
		// from 1 to max no of col in this map
		for(iCol=GetBlkMinColIdx(); iCol<=GetBlkMaxColIdx(); iCol++)
		{
			ulMapIdx = (iRow-GetBlkMinRowIdx())*ulWidth + iCol-GetBlkMinColIdx() + 1;
			if( ulIndex==ulMapIdx )
			{
				iRefIdx = iRow*MAXREFERWIDTH + iCol;
				if (GetReferInMap(iRefIdx) == FALSE)
					return FALSE;
				if (GetReferInWfa(iRefIdx) == 0)
					return FALSE;

				lMapRow = GetReferMapPosRow(iRefIdx);
				lMapCol = GetReferMapPosCol(iRefIdx);
				lTableX = GetPrepDieWftX(lMapRow, lMapCol);
				lTableY = GetPrepDieWftY(lMapRow, lMapCol);
				return TRUE;
			}
		}
	}

	return FALSE;
}

LONG CWT_BlkFunc2::NextReferDiePosition(DiePhyPos2 PhyInput, UCHAR ucDirection, DiePhyPos2 *PhyOutput)
{
	LONG lCharValue;
	return NextReferDiePositionValue(PhyInput, ucDirection, PhyOutput, &lCharValue);
}

BOOL CWT_BlkFunc2::Blk2MergeAlignRhombusMap()
{
	LONG i, j, lCtrRowIdx, lCtrColIdx, lRowIdx, lColIdx;
	unsigned long ulIndex;
	LONG lMinRowIdx, lMinColIdx, lMaxRowIdx, lMaxColIdx;
	ULONG ulGoodCount = 0;

	lMinRowIdx = lMinColIdx = REFER_BASE;
	lMaxRowIdx = lMaxColIdx = REFER_BASE;
	for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			if (GetRefDieStatus(i, j) == 1)
			{
				ulGoodCount++;
				if (i < lMinRowIdx )
					lMinRowIdx = i;
				if (j < lMinColIdx )
					lMinColIdx = j;
				if (i > lMaxRowIdx )
					lMaxRowIdx = i;
				if (j > lMaxColIdx )
					lMaxColIdx = j;
			}
		}
	}

	CString strTemp;
	SetFDCRegionLog("\n");
	strTemp.Format("Rhombus Min RC Index(%d,%d), Max RC Index(%d,%d)", lMinRowIdx, lMinColIdx, lMaxRowIdx, lMaxColIdx);
	SetFDCRegionLog(strTemp);

	if( ulGoodCount!=4 )
	{
		strTemp.Format("Found refer die count %d", ulGoodCount);
		SetFDCRegionLog(strTemp);
		return FALSE;
	}

	lCtrRowIdx = (lMinRowIdx+lMaxRowIdx)/2;
	lCtrColIdx = (lMinColIdx+lMaxColIdx)/2;
	if( GetRefDieStatus(lCtrRowIdx-1, lCtrColIdx)==1 && 
		GetRefDieStatus(lCtrRowIdx,   lCtrColIdx-1)==1 &&
		GetRefDieStatus(lCtrRowIdx,   lCtrColIdx+1)==1 && 
		GetRefDieStatus(lCtrRowIdx+1, lCtrColIdx)==1 )
	{
		ulGoodCount = 0;
		for(i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
		{
			for(j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
			{
				ulIndex = i*MAXREFERWIDTH + j;
				if( GetReferInMap(ulIndex) && ulGoodCount<4 )
				{
					ulGoodCount++;
					switch(ulGoodCount)
					{
					case 1:
						lRowIdx = lCtrRowIdx-1;
						lColIdx = lCtrColIdx;
						break;
					case 2:
						lRowIdx = lCtrRowIdx;
						lColIdx = lCtrColIdx-1;
						break;
					case 3:
						lRowIdx = lCtrRowIdx;
						lColIdx = lCtrColIdx+1;
						break;
					case 4:
						lRowIdx = lCtrRowIdx+1;
						lColIdx = lCtrColIdx;
						break;
					}
					SetReferWftInfo(ulIndex, GetRefDieWftX(lRowIdx, lColIdx), GetRefDieWftY(lRowIdx, lColIdx));
					m_stReferWftInfo[ulIndex].m_cInWaf = 1;
					SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CWT_BlkFunc2::Blk2FindAlignAllReferHalfWafer()
{
	LONG		i,j;
	DieMapRowColIdx	staDieStack[MAXFDC2NUM];
	LONG		lX,lY,lT, lHomeTheta;
	LONG		lDig1,lDig2;

	ULONG		ulIndex;
	LONG		sResult;

	int			nDieStackIndex;
	LONG lCurRowIdx, lCurColIdx;
	DiePhyPos2	CurPhy, TgtPhy;
	DiePhyPos2	stHomePhy;
	CString szMsg;
	CString	strTemp;

	Blk2InitBlockFunc();

	Blk2GetWTPosition(&lX, &lY, &lT);

	sResult = Blk2SearchReferDieInFOV( &lX, &lY, &lDig1, &lDig2, FALSE);
	if (sResult == FALSE)
	{
		szMsg = "No ref die found in current position!";
		SetDisplayErrorMessage(szMsg);
		return FALSE;
	}

	SetFDCRegionLog("\n");
	strTemp.Format("Home Die Position:    %d,%d,%d\n", lX, lY, lT);
	SetLogCtmMessage(strTemp);

	CurPhy.x = stHomePhy.x = lX;
	CurPhy.y = stHomePhy.y = lY;
	lHomeTheta = lT;
	m_stFDCBasic.m_stHomeDiePhyX = lX;
	m_stFDCBasic.m_stHomeDiePhyY = lY;
	m_stFDCBasic.m_stHomeDiePhyT = lT;

	m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();

	for (i=0; i<MAXFDC2NUM; i++)
	{
		staDieStack[i].m_lRowIdx = 0;
		staDieStack[i].m_lColIdx = 0;
	}

	// set the virtual center as the current row/col
	ulIndex = (REFER_BASE) * MAXREFERWIDTH + REFER_BASE;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_WfX = lX;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_WfY = lY;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_sStatus = 1;
	m_paaRefDiePos[REFER_BASE][REFER_BASE].m_lReferValue = lDig1*10+lDig2;

	lCurRowIdx = staDieStack[0].m_lRowIdx = REFER_BASE;
	lCurColIdx = staDieStack[0].m_lColIdx = REFER_BASE;
	nDieStackIndex = 1;

	// Alignment in Cross Path
	SetFDCRegionLog("\n");
	sResult = Blk2IndexCrossRefDiePos(CurPhy, staDieStack, &nDieStackIndex);
	if (sResult == FALSE) 
	{
		SetFDCRegionLog("cross find refer die fail");
		return FALSE;
	}

	LONG lNxtRow, lNxtCol;
	UCHAR ucNxtGo;
	BOOL  bNxtDie = FALSE, bRowColSame = TRUE;
	CurPhy.x = stHomePhy.x;
	CurPhy.y = stHomePhy.y;

	while (nDieStackIndex > 0)
	{
		bNxtDie = FALSE;
		if (IsToStopAlign())
		{
			return FALSE;
		}

		for(ucNxtGo=BLK_REFER_UP; ucNxtGo<=BLK_REFER_DOWNRIGHT; ucNxtGo++)
		{
			bRowColSame = TRUE;
			switch( ucNxtGo )
			{
			case BLK_REFER_UP:
				lNxtRow = lCurRowIdx-1;
				lNxtCol = lCurColIdx;
				break;
			case BLK_REFER_LEFT:
				lNxtRow = lCurRowIdx;
				lNxtCol = lCurColIdx-1;
				break;
			case BLK_REFER_DOWN:
				lNxtRow = lCurRowIdx+1;
				lNxtCol = lCurColIdx;
				break;
			case BLK_REFER_RIGHT:
				lNxtRow = lCurRowIdx;
				lNxtCol = lCurColIdx+1;
				break;
			case BLK_REFER_UPRIGHT:
				lNxtRow = lCurRowIdx-1;
				lNxtCol = lCurColIdx+1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			case BLK_REFER_UPLEFT:
				lNxtRow = lCurRowIdx-1;
				lNxtCol = lCurColIdx-1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			case BLK_REFER_DOWNLEFT:
				lNxtRow = lCurRowIdx+1;
				lNxtCol = lCurColIdx-1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			case BLK_REFER_DOWNRIGHT:
				lNxtRow = lCurRowIdx+1;
				lNxtCol = lCurColIdx+1;
				bRowColSame = GetBlkPitchCol() == GetBlkPitchRow();
				break;
			}

			if ( bRowColSame && GetRefDieStatus(lNxtRow, lNxtCol) == 0 )
			{
				bNxtDie = TRUE;
				break;
			}
		}

		if( bNxtDie )
		{
			sResult = NextReferDiePosition(CurPhy, ucNxtGo, &TgtPhy);
			if (sResult == TRUE)
			{
				m_paaRefDiePos[lNxtRow][lNxtCol].m_WfX = TgtPhy.x;
				m_paaRefDiePos[lNxtRow][lNxtCol].m_WfY = TgtPhy.y;
				m_paaRefDiePos[lNxtRow][lNxtCol].m_sStatus = 1;
				staDieStack[nDieStackIndex].m_lRowIdx = lNxtRow;
				staDieStack[nDieStackIndex].m_lColIdx = lNxtCol;
				nDieStackIndex++;
				CurPhy = TgtPhy;
				lCurRowIdx = lNxtRow;
				lCurColIdx = lNxtCol;
				continue;
			}
			else if (sResult == FALSE)
			{
				m_paaRefDiePos[lNxtRow][lNxtCol].m_sStatus = -1;
			}
			else
			{
				return FALSE;
			}
		}

		m_ulBlk2WaferReferNum++;
		nDieStackIndex--;
		if (nDieStackIndex!=0)
		{
			lCurRowIdx = staDieStack[nDieStackIndex-1].m_lRowIdx;
			lCurColIdx = staDieStack[nDieStackIndex-1].m_lColIdx;
			CurPhy.x = GetRefDieWftX(lCurRowIdx, lCurColIdx);
			CurPhy.y = GetRefDieWftY(lCurRowIdx, lCurColIdx);
			szMsg.Format("all check to index (%d,%d), posn (%d,%d)", lCurRowIdx, lCurColIdx, CurPhy.x, CurPhy.y);
			SetFDCRegionLog(szMsg);
		}
	}//endwhile

	SetFDCRegionLog("\n");
	szMsg.Format("Find Refer valid idx, wft posn");
	SetFDCRegionLog(szMsg);
	for(i=0; i<MAXREFERWIDTH; i++)
	{
		for(j=0; j<MAXREFERWIDTH; j++)
		{
			if( GetRefDieStatus(i, j)==1 )
			{
				szMsg.Format("index (%2d,%2d), posn (%d,%d)", i, j, 
					GetRefDieWftX(i, j), GetRefDieWftY(i, j));
				SetFDCRegionLog(szMsg);
			}
		}
	}

	sResult = Blk2MergeAlignMap();	//	MS90 block pick alignment

	// for HMI display purpose
	LONG lRefFoundInDeed = m_ulBlk2WaferReferNum>GetMapReferNum()?GetMapReferNum():m_ulBlk2WaferReferNum;
	m_stnWaferTable->SetRefDieCounts(GetMapReferNum(), lRefFoundInDeed);

	strTemp.Format("Half Wafer Found Refer die Count = %lu; indeed %ld\n", m_ulBlk2WaferReferNum, lRefFoundInDeed);
	SetFDCRegionLog(strTemp);

	BOOL bManAlign = FALSE;
	if (sResult == FALSE)
	{
		if ( IsAutoManualAlign() )	//Auto proceed to manual align
		{
			SetErrorLogMessage("BlockPick: Auto Align fails. Auto execute Manual Align.");
		}
		else
		{
			LONG lTmp;
			lTmp = SetHmiMessage("Do you want to continue by\n\n    Manual-Alignment", "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			if (lTmp == glHMI_NO)
			{
				return FALSE;
			}
		}
		bManAlign = TRUE;
	}
	else
	{
		DOUBLE dPercent = lRefFoundInDeed*100.0/GetMapReferNum();
		if (dPercent < m_lBlk2MnPassPercent)
		{
			if ( IsAutoManualAlign() )	//Auto proceed to manual align
			{
				strTemp.Format("Only %.2f Percent of reference dice can be found.\n Auto execute Manual Align...", dPercent);
				SetErrorLogMessage(strTemp);
			}
			else
			{
				LONG lTmp;
				strTemp.Format("Only %.2f% of reference dice can be found.\n\nDo you want to continue by\n    Manual-Alignment?", dPercent);
				lTmp = SetHmiMessage(strTemp, "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
				if (lTmp == glHMI_NO)
				{
					return FALSE;
				}
			}
			bManAlign = TRUE;
		}
	}

	if( bManAlign )
	{
		LONG lX, lY, lT;
		sResult = Blk2ManAlignHalfWafer(stHomePhy);
		Blk2GetWTPosition(&lX, &lY, &lT);
		lHomeTheta = lT;
	}

	SetFDCRegionLog("\n");
	SetFDCRegionLog("After alignment finished");
	PrintFDC2();
	if( bManAlign && (sResult == FALSE) )
	{
		return FALSE;
	}

	sResult = CheckRelativePstnInFDC();
	if (sResult == FALSE)
	{
		return FALSE;
	}

	m_stFDCBasic.m_bHaveAligned = TRUE;

	//v2.61T7
	//-- Find farthest ref-die position for COR checking --//
	if (m_stnWaferTable->IsCheckCOREnabled())
	{
		LONG lx, ly;
		LONG lCorX, lCorY;
		m_stnWaferTable->GetWaferCalibXY(lCorX, lCorY);
		DOUBLE dMaxDist = 0, dDist = 0;
		LONG lMaxX=0, lMaxY=0;
		for (i=0; i<MAXREFERWIDTH; i++) 
		{
			for (j=0; j<MAXREFERWIDTH; j++) 
			{
				if (GetRefDieStatus(i, j) == 1)
				{		
					lx = GetRefDieWftX(i, j);
					ly = GetRefDieWftY(i, j);
					if (CheckWaferLimit(lx, ly) == FALSE)
					{
						continue;
					}
					dDist = GetDistIn2Points(lCorX, lCorY, lx, ly);
					if (dDist > dMaxDist)
					{
						dMaxDist = dDist;
						lMaxX = lx;
						lMaxY = ly;
					}
				}
			}
		}

		LONG lOldX, lOldY, lOldT;
		Blk2GetWTPosition(&lOldX, &lOldY, &lOldT);
		MoveXYTo(lMaxX, lMaxY);
		Sleep(m_lPrSrchDelay);

		if (!Blk2CheckCOR())
		{
			SetDisplayErrorMessage("COR-check fails on farthest ref-die!");
			SetErrorLogMessage("COR-check fails on farthest ref-die!");
			return FALSE;
		}

		MoveXYTo(lOldX, lOldY);
		Sleep(m_lPrSrchDelay);
	}

	//Get edge position
	if (IsToStopAlign())
	{
		return FALSE;
	}

	Blk2DrawMap();		//Block2

	sResult = Blk2RealignRefDiePos(m_bBlk2MnEdgeAlign);
	if (sResult == FALSE)
	{
		return FALSE;
	}

	//Show RefDie
	LONG	lTLRow, lTLCol;
	LONG	ulRow, ulCol;

    for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if ( GetReferInMap(ulIndex) && GetReferInWfa(ulIndex) == 1)
			{
				lTLRow = GetReferMapPosRow(ulIndex);
				lTLCol = GetReferMapPosCol(ulIndex);

				m_stnWaferTable->m_pWaferMapManager->AddBoundary(lTLRow, lTLRow+2, lTLCol, lTLCol+2);

				m_stnWaferTable->SetMapPhyPosn(lTLRow, lTLCol, GetReferDiePhyX(ulIndex), GetReferDiePhyY(ulIndex));

				SetMapAlgorithmPrm("Row", i);
				SetMapAlgorithmPrm("Col", j);
				SetMapAlgorithmPrm("Align", REGN_ATREF2);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);
			}
		}
	}

	Blk2SetStartDie(stHomePhy.x, stHomePhy.y, lHomeTheta);
	m_stFDCBasic.m_stHomeDiePhyT = lHomeTheta;
	Blk2GetStartDie(&ulRow, &ulCol, &lX, &lY);
	m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);

	if (CheckEmptyDieGrade() == FALSE)
	{
		return FALSE;
	}

	Blk2SetBondingLastDie();

	return TRUE;
}

LONG CWT_BlkFunc2::Blk2ManAlignHalfWafer(DiePhyPos2 &HomeNewPhy)
{
	int i,j;
	ULONG		ulIndex, ulIndex1, ulIndex2;
	LONG	CtrRowIdx, CtrColIdx;
	DiePhyPos2	CurrPhy, TgtPhy;
	int			iTopLeftCol;
	LONG		lX, lY, lT;
	LONG		lDig1, lDig2;

	LONG		sResult;
	DieMapPos2	RefDieMap;
	DieMapPos2  CtrMap;
	DiePhyPos2	RefDiePhy;
	LONG		lTmp;
	CString		strTemp;

	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			m_stReferWftInfo[ulIndex].m_cInWaf = 0;
		}
	}

	//Move to center , first find refer die position
 	CurrPhy.x = HomeNewPhy.x;
	CurrPhy.y = HomeNewPhy.y;
	if (CheckWaferLimit(CurrPhy.x, CurrPhy.y) == TRUE)
	{
		MoveXYTo(CurrPhy.x, CurrPhy.y);
		Sleep(m_lPrSrchDelay);
	}
	else
	{
		return FALSE;
	}

	//Adjust Joystick
	IPC_CServiceMessage stMsg;
	LONG lRtn = 0;
	LONG lLevel = 1;
	LONG lDigital = 0;

	stMsg.InitMessage(sizeof(LONG), &lLevel);
	lRtn = m_stnWaferTable->SetJoystickSpeedCmd(stMsg);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Cree")	
		GetRealMapCenterDie(&CtrMap);
	else
		GetUserMapCenterDie(&CtrMap, &CurrPhy, &lDigital);

	Blk2GetWTPosition(&lX,&lY,&lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;

	BOOL	bAutoManualAlignOK = FALSE;
	if ( IsAutoManualAlign() )
	{
		LONG bResult = Blk2SearchReferDieInFOV( &lX, &lY, &lDig1, &lDig2);

		if( bResult==FALSE )
		{
			strTemp = "No ref die found in current position!";
			SetDisplayErrorMessage(strTemp);
		}
		else
		{
			for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
			{
				for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
				{
					ulIndex = i*MAXREFERWIDTH + j;

					ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

					if( Blk2IsSrchCharDie() )
					{
						if ( GetReferMapPosValue(ulIndex)==(lDig1*10+lDig2) )
						{
							CtrMap.Row = GetReferSrcMapRow(ulIndex);
							CtrMap.Col = GetReferSrcMapCol(ulIndex);

							//if Ref Die found does not exist in map, AutoManual Align fails
							if (GetReferInMap(ulIndex))
							{
								bAutoManualAlignOK = TRUE;
								break;
							}
							else
							{
								bAutoManualAlignOK = FALSE;
								break;
							}
						}
					}
					else
					{
						bAutoManualAlignOK = TRUE;
						break;
					}
				}

				if (bAutoManualAlignOK == TRUE)
				{
					break;
				}
			}
		}

		if( (bAutoManualAlignOK == FALSE) )
		{
			//if Auto Manual Align fails, ask user to manual align
			lTmp = SetHmiMessage("Ref Die Found does not exist in map!\nDo you want to continue by\n\n    Manual-Alignment", "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			if (lTmp == glHMI_NO)
			{
				return FALSE;
			}
		}
	}

	if ( (bAutoManualAlignOK == FALSE) )
	{
		//Ask for moving table to center, should also display the char value from wafer map
		SetJoystick(TRUE);
		if( Blk2IsSrchCharDie() )
			strTemp.Format("Please move to refer die(r%d,c%d)(%d)...\n\nAnd press ENTER", CtrMap.Row, CtrMap.Col, lDigital);
		else
			strTemp.Format("Please move to refer die(r%d,c%d)...\n\nAnd press ENTER", CtrMap.Row, CtrMap.Col);
		SetHmiMessageEx(strTemp, "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
		SetFDCRegionLog(strTemp);
		SetJoystick(FALSE);
	}

	Blk2GetWTPosition(&lX, &lY, &lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;

	BOOL bFindCtr = FALSE;
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;

			ulIndex = min(ulIndex, MAXFDC2NUM-1);		//Klocwork	//v4.02T5

			if( GetReferSrcMapRow(ulIndex) == CtrMap.Row && GetReferSrcMapCol(ulIndex) == CtrMap.Col )
			{
				sResult = Blk2SearchReferOnlyInFOV( &lX, &lY);
				if ( sResult == TRUE )
				{
					CurrPhy.x = lX;
					CurrPhy.y = lY;
					m_stReferWftInfo[ulIndex].m_cInWaf = 1;
				}
				else
				{
					m_stReferWftInfo[ulIndex].m_cInWaf = 2;
				}

				SetReferWftInfo(ulIndex, CurrPhy.x, CurrPhy.y);
				SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);

				CtrRowIdx = i;
				CtrColIdx = j;
				bFindCtr = TRUE;
				break;
			}
		}
		if( bFindCtr )
		{
			break;
		}
	}


	HomeNewPhy.x = lX;
	HomeNewPhy.y = lY;

	if (bFindCtr == FALSE)
	{
		SetHmiMessage("Manual Align can not find center die", "Block Alignment");
		return FALSE;
	}

	i = CtrRowIdx;
	j = CtrColIdx;

	// look up to Top
	while (i>GetBlkMinRowIdx())
	{
		i--;
		ulIndex = i*MAXREFERWIDTH + j;
		sResult = NextReferDiePosition(CurrPhy, BLK_REFER_UP, &TgtPhy);
		if (sResult == TRUE)
		{
			SetReferWftInfo(ulIndex, TgtPhy.x, TgtPhy.y);
			m_stReferWftInfo[ulIndex].m_cInWaf = 1;
			SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);

			CurrPhy = TgtPhy;
		}
		else if (sResult == FALSE)
		{
			if ( GetReferInMap(ulIndex)==FALSE ) 
			{
				Blk2GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				SetReferWftInfo(ulIndex, CurrPhy.x, CurrPhy.y);
				continue;
			}

			lTmp = Blk2ManualLocateReferDie(ulIndex, lX, lY);

			if ( lTmp == 1 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 5 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 8 )
			{
				return FALSE;
			}
		}
		else	// motor error
		{
			return FALSE;
		}
	}

	// top row, column min and max
	iTopLeftCol = -1;
	i = GetBlkMinRowIdx();
	for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
	{
		ulIndex = i*MAXREFERWIDTH + j;
		ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

		if (GetReferInMap(ulIndex))
		{
			iTopLeftCol = j;
			break;
		}
	}
	for (j=GetBlkMaxColIdx(); j>=GetBlkMinColIdx(); j--)
	{
		ulIndex = i*MAXREFERWIDTH + j;
		ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

		if (GetReferInMap(ulIndex))
		{
			break;
		}
	}

	// search top left part from top center to top left
	j = CtrColIdx;
	while (j>iTopLeftCol)
	{
		j--;
		ulIndex = i*MAXREFERWIDTH + j;
		ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

		sResult = NextReferDiePosition(CurrPhy, BLK_REFER_LEFT, &TgtPhy);
		if (sResult == TRUE)
		{
			SetReferWftInfo(ulIndex, TgtPhy.x, TgtPhy.y);
			m_stReferWftInfo[ulIndex].m_cInWaf = 1;
			SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);

			CurrPhy = TgtPhy;
		}
		else if (sResult == FALSE)
		{
			if ( GetReferInMap(ulIndex)==FALSE ) 
			{
				Blk2GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				SetReferWftInfo(ulIndex, CurrPhy.x, CurrPhy.y);
				continue;
			}

			lTmp = Blk2ManualLocateReferDie(ulIndex, lX, lY);

			if ( lTmp == 1 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 5 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 8 )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	// update hori FDC pitch
	ulIndex1 = GetBlkMinRowIdx()*MAXREFERWIDTH + iTopLeftCol;
	ulIndex2 = GetBlkMinRowIdx()*MAXREFERWIDTH + CtrColIdx;
	LONG lBlkPitchXX = GetDiePitchHX() * GetBlkPitchCol();
	LONG lBlkPitchXY = GetDiePitchHY() * GetBlkPitchCol();

	ulIndex1 = min(ulIndex1, MAXFDC2NUM-1);		//Klocwork

	if (iTopLeftCol != CtrColIdx && GetReferInWfa(ulIndex1) == 1)
	{
		LONG lNewPitchXX = (GetReferDiePhyX(ulIndex2) - GetReferDiePhyX(ulIndex1)) / (CtrColIdx-iTopLeftCol);
		LONG lNewPitchXY = (GetReferDiePhyY(ulIndex2) - GetReferDiePhyY(ulIndex1)) / (CtrColIdx-iTopLeftCol);
		ULONG ulBlkDist = (ULONG) GetDistIn2Points(lBlkPitchXX, lBlkPitchXY, 0, 0);
		ULONG ulNewDist = (ULONG) GetDistIn2Points(lNewPitchXX, lNewPitchXY, 0, 0);

		if( labs(ulNewDist-ulBlkDist)>(ulBlkDist*0.5) )
		{
			lBlkPitchXX = lNewPitchXX;
			lBlkPitchXY = lNewPitchXY;
		}
	}
	m_stFDCBasic.m_lFDCPitchX_X = lBlkPitchXX;
	m_stFDCBasic.m_lFDCPitchX_Y = lBlkPitchXY;

	// update vert FDC pitch
	ulIndex1 = GetBlkMinRowIdx()*MAXREFERWIDTH + CtrColIdx;
	ulIndex2 = CtrRowIdx*MAXREFERWIDTH + CtrColIdx;
	LONG lBlkPitchYY = GetDiePitchVY() * GetBlkPitchRow();
	LONG lBlkPitchYX = GetDiePitchVX() * GetBlkPitchRow();

	ulIndex1 = min(ulIndex1, MAXFDC2NUM-1);		//Klocwork

	if (GetBlkMinRowIdx() != CtrRowIdx && GetReferInWfa(ulIndex1) == 1)
	{
		LONG lNewPitchYX = (GetReferDiePhyX(ulIndex2) - GetReferDiePhyX(ulIndex1)) / (CtrRowIdx-GetBlkMinRowIdx());
		LONG lNewPitchYY = (GetReferDiePhyY(ulIndex2) - GetReferDiePhyY(ulIndex1)) / (CtrRowIdx-GetBlkMinRowIdx());
		ULONG ulNewDist = (ULONG) GetDistIn2Points(lNewPitchYX, lNewPitchYY, 0, 0);
		ULONG ulBlkDist = (ULONG) GetDistIn2Points(lBlkPitchYX, lBlkPitchYY, 0, 0);
		if( labs(ulNewDist-ulBlkDist)>(ulBlkDist*0.5) )
		{
			lBlkPitchYY = lNewPitchYY;
			lBlkPitchYX = lNewPitchYX;
		}
	}
	m_stFDCBasic.m_lFDCPitchY_Y = lBlkPitchYY;
	m_stFDCBasic.m_lFDCPitchY_X = lBlkPitchYX;

	//Begin Index
	// from top left to search, horizontal, then vertical; then next row from left to right
	LONG lDiffRow, lDiffCol;
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

			if ( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}
			if (GetReferInWfa(ulIndex) != 0)
			{
				continue;
			}

			Blk2GetNearRefDie(m_stReferWftInfo[ulIndex].m_MapPos, &RefDieMap, &RefDiePhy);

			lDiffRow = GetReferMapPosRow(ulIndex) - RefDieMap.Row;
			lDiffCol = GetReferMapPosCol(ulIndex) - RefDieMap.Col;
			CurrPhy.x = RefDiePhy.x + lDiffCol*GetDiePitchHX() + lDiffRow*GetDiePitchVX();
			CurrPhy.y = RefDiePhy.y + lDiffCol*GetDiePitchHY() + lDiffRow*GetDiePitchVY();

			if (CheckWaferLimit(CurrPhy.x, CurrPhy.y) == FALSE)
			{
				continue;
			}

			//v4.04		//Klocwork
			MoveXYTo(CurrPhy.x, CurrPhy.y);

			Sleep(m_lPrSrchDelay);

			// Align RefDie
			Blk2GetWTPosition(&lX,&lY,&lT);
			sResult = Blk2SearchReferOnlyInFOV( &lX, &lY);
			if ( sResult == TRUE )
			{
				SetReferWftInfo(ulIndex, lX, lY);
				m_stReferWftInfo[ulIndex].m_cInWaf = 1;
				SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);
			}
			else
			{
				lTmp = Blk2ManualLocateReferDie(ulIndex, lX, lY);

				if ( lTmp == 8 )	// stop
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2MergeAlignMapLessRefer()
{
	LONG lWafMinRowIdx, lWafMinColIdx, lWafMaxRowIdx, lWafMaxColIdx;
	lWafMinRowIdx = lWafMinColIdx = REFER_BASE;
	lWafMaxRowIdx = lWafMaxColIdx = REFER_BASE;
	for (LONG i=0; i<MAXREFERWIDTH; i++)
	{
		for (LONG j=0; j<MAXREFERWIDTH; j++)
		{
			if (GetRefDieStatus(i, j) == 1)
			{		
				if (i < lWafMinRowIdx ) lWafMinRowIdx = i;
				if (j < lWafMinColIdx ) lWafMinColIdx = j;
				if (i > lWafMaxRowIdx ) lWafMaxRowIdx = i;
				if (j > lWafMaxColIdx ) lWafMaxColIdx = j;
			}
		}
	}

	LONG lMapMinRowIdx = GetBlkMinRowIdx();
	LONG lMapMinColIdx = GetBlkMinColIdx();
	LONG lMapMaxRowIdx = GetBlkMaxRowIdx();
	LONG lMapMaxColIdx = GetBlkMaxColIdx();

	CString strTemp;
	SetFDCRegionLog("\n");
	strTemp.Format("No OCR Wafer Min RC (%2d,%2d), Max RC (%2d,%2d), total %lu",
		lWafMinRowIdx, lWafMinColIdx, lWafMaxRowIdx, lWafMaxColIdx, m_ulBlk2WaferReferNum);
	SetFDCRegionLog(strTemp);
	strTemp.Format("No OCR   Map Min RC (%2d,%2d), Max RC (%2d,%2d), total %d",
		lMapMinRowIdx, lMapMinColIdx, lMapMaxRowIdx, lMapMaxColIdx, GetMapReferNum());
	SetFDCRegionLog(strTemp);

	LONG lReferMax = m_ulBlk2WaferReferNum>GetMapReferNum()?GetMapReferNum():m_ulBlk2WaferReferNum;
	if( lReferMax<=0 )
	{
		return FALSE;
	}
	LONG lReferLimit = lReferMax*m_lBlk2MnPassPercent/100;

	CUIntArray aunMergedListRow, aunMergedListCol, aunMergedListNum;
	aunMergedListRow.RemoveAll();
	aunMergedListCol.RemoveAll();
	aunMergedListNum.RemoveAll();
	INT iMergedMax = 0;
	LONG lPhyMapDiffRow = (lWafMaxRowIdx-lWafMinRowIdx) - (lMapMaxRowIdx-lMapMinRowIdx);
	LONG lPhyMapDiffCol = (lWafMaxColIdx-lWafMinColIdx) - (lMapMaxColIdx-lMapMinColIdx);
	LONG lMaxMatchPoints = 0, lMaxMatchOffsetRow = 0, lMaxMatchOffsetCol = 0;
	for(LONG iLoopRow=0; iLoopRow<=labs(lPhyMapDiffRow); iLoopRow++)
	{
		for(LONG iLoopCol=0; iLoopCol<=labs(lPhyMapDiffCol); iLoopCol++)
		{
			LONG lMatchPoints = 0;
			LONG iOffRow = iLoopRow;
			LONG iOffCol = iLoopCol;
			if( lPhyMapDiffRow<0 )
				iOffRow = 0 - iLoopRow;
			if( lPhyMapDiffCol<0 )
				iOffCol = 0 - iLoopCol;
			for(LONG iMap=lMapMinRowIdx; iMap<=lMapMaxRowIdx; iMap++)
			{
				for(LONG jMap=lMapMinColIdx; jMap<=lMapMaxColIdx; jMap++)
				{
					ULONG ulMapIndex = iMap*MAXREFERWIDTH + jMap;
					ulMapIndex = min(ulMapIndex, MAXFDC2NUM-1);
					if( GetReferInMap(ulMapIndex) )
					{
						LONG k1 = lWafMinRowIdx + iMap - lMapMinRowIdx + iOffRow;
						LONG k2 = lWafMinColIdx + jMap - lMapMinColIdx + iOffCol;
						if( k1<lWafMinRowIdx || k1>lWafMaxRowIdx || k2<lWafMinColIdx || k2>lWafMaxColIdx )
						{
							continue;
						}
						if ( GetRefDieStatus(k1, k2) == 1 )
						{
							lMatchPoints++;
						}
					}
				}
			}

			if( lMatchPoints>=lReferLimit )
			{
				aunMergedListRow.Add(iLoopRow);
				aunMergedListCol.Add(iLoopCol);
				aunMergedListNum.Add(lMatchPoints);
				iMergedMax++;
				if( lMaxMatchPoints<lMatchPoints )
				{
					lMaxMatchPoints = lMatchPoints;
					lMaxMatchOffsetRow = iOffRow;
					lMaxMatchOffsetCol = iOffCol;
				}
			}
		}
	}

	strTemp.Format("No OCR merge match %d, max match points %ld,offset %ld,%ld",
		iMergedMax, lMaxMatchPoints, lMaxMatchOffsetRow, lMaxMatchOffsetCol);
	SetFDCRegionLog(strTemp);

	LONG lMaxTotal = 0;
	for(int i=0; i<iMergedMax; i++)
	{
		LONG iOffRow = aunMergedListRow.GetAt(i);
		LONG iOffCol = aunMergedListCol.GetAt(i);
		LONG iPoints = aunMergedListNum.GetAt(i);
		if( lPhyMapDiffRow<0 )
			iOffRow = 0 - iOffRow;
		if( lPhyMapDiffCol<0 )
			iOffCol = 0 - iOffCol;
		if( iPoints==lMaxMatchPoints )
		{
			lMaxTotal++;
		}
		strTemp.Format("merge list %02d offset %2d,%2d,%d", i+1, iOffRow, iOffCol, iPoints);
		SetFDCRegionLog(strTemp);
	}

	if( lMaxTotal!=1 )
	{
		return FALSE;
	}

	LONG iOffRow = lMaxMatchOffsetRow;
	LONG iOffCol = lMaxMatchOffsetCol;
	for (LONG k1=lWafMinRowIdx; k1<=lWafMaxRowIdx; k1++)
	{
		for (LONG k2=lWafMinColIdx; k2<=lWafMaxColIdx; k2++)
		{
			LONG iMap = k1 - lWafMinRowIdx + lMapMinRowIdx - iOffRow;
			LONG jMap = k2 - lWafMinColIdx + lMapMinColIdx - iOffCol;
			ULONG ulMapIndex = iMap*MAXREFERWIDTH + jMap;
			ulMapIndex = min(ulMapIndex, MAXFDC2NUM-1);

			if ( GetRefDieStatus(k1, k2) == 1 )
			{
				SetReferWftInfo(ulMapIndex, GetRefDieWftX(k1, k2), GetRefDieWftY(k1,k2));
				m_stReferWftInfo[ulMapIndex].m_cInWaf = 1;
				SetReferDiePrepStatus(ulMapIndex, REGN_ALIGN2);
			}
		}
	}

	return TRUE;
}	//	Align map with wafer but not all reference die exist

