//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_BlkFunc.h"
#include "math.h"
#include "MS896A.h"
//#include "MS896AStn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//================================================================
// global and static variable declaration
//================================================================

//================================================================
// Static function prototype
//================================================================
BOOL	CWT_BlkFunc::Blk_bDebug = FALSE;

//================================================================
// Constructor / destructor implementation section
//================================================================
	
CWT_BlkFunc::CWT_BlkFunc()
{
	m_stnWaferTable = NULL;
	MnSrchRefGrid = 10;
	MnMaxJumpCtr = 15;
	MnMaxJumpEdge = 1;
	MnNoDieGrade = 17;
	MnAlignReset = 0;
	FDCBasic.HaveAligned = 0;

	m_bRefRhombus = TRUE;
}

CWT_BlkFunc::~CWT_BlkFunc()
{
	;
}

void CWT_BlkFunc::SetWaferTable(CWaferTable* stnWaferTable)
{
	m_stnWaferTable = stnWaferTable;
}

void CWT_BlkFunc::SetWaferAlignment(CHAR cAligned, LONG lMapRow, LONG lMapCol, LONG lPhyX, LONG lPhyY)
{
	LONG lX,lY,lT;
	GetWTPosition(&lX, &lY, &lT);

	FDCBasic.HaveAligned = cAligned;
	FDCBasic.HomeDie.MapPos.Row = lMapRow;
	FDCBasic.HomeDie.MapPos.Col = lMapCol;
	FDCBasic.HomeDie.PhyPos.x = lPhyX;
	FDCBasic.HomeDie.PhyPos.y = lPhyY;
	FDCBasic.HomeDie.PhyPos.t = lT;

	DiePhyPrep[lMapRow][lMapCol].Status = Regn_Align;
	DiePhyPrep[lMapRow][lMapCol].x = lPhyX;
	DiePhyPrep[lMapRow][lMapCol].y = lPhyY;
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", lMapRow);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", lMapCol);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);
	m_stnWaferTable->UpdateMapDie(lMapRow, lMapCol, 1, WAF_CDieSelectionAlgorithm::ALIGN, WT_MAP_DIESTATE_15);

	LONG PitchX_X, PitchX_Y;
	LONG PitchY_X, PitchY_Y;
    m_stnWaferTable->GetDiePitchX(&PitchX_X, &PitchX_Y);
    m_stnWaferTable->GetDiePitchY(&PitchY_X, &PitchY_Y);
	FDCBasic.HPitchDie.x = -PitchX_X;
	FDCBasic.HPitchDie.y = -PitchX_Y;
	FDCBasic.VPitchDie.x = -PitchY_X;
	FDCBasic.VPitchDie.y = -PitchY_Y;

	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Empty);

	LONG lPosArX, lPosArY, lDir;
	BOOL bArResult[8];
	DieMapPos CurDie;
	ULONG ulNumRow = 0, ulNumCol = 0, ulLaRow, ulLaCol;
	m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);
	CurDie.Row = lMapRow;
	CurDie.Col = lMapCol;

	for (lDir=0;lDir<8;lDir++)
	{
		bArResult[lDir] = SearchAroundDie(lDir,&lPosArX, &lPosArY);
		ulLaRow = ulLaCol = 1000;
		switch (lDir)
		{
		case 0:
			if (CurDie.Col == 0) break;
			ulLaRow = CurDie.Row;
			ulLaCol = CurDie.Col-1;
			break;
		case 1:
			if (CurDie.Col == ulNumCol-1) break;
			ulLaRow = CurDie.Row;
			ulLaCol = CurDie.Col+1;
			break;
		case 2:
			if (CurDie.Row == 0) break;
			ulLaRow = CurDie.Row-1;
			ulLaCol = CurDie.Col;
			break;
		case 3:
			if (CurDie.Row == ulNumRow-1) break;
			ulLaRow = CurDie.Row+1;
			ulLaCol = CurDie.Col;
			break;
		case 4:
			if (CurDie.Row == 0 || CurDie.Col == 0) break;
			ulLaRow = CurDie.Row-1;
			ulLaCol = CurDie.Col-1;
			break;
		case 5:
			if (CurDie.Row == ulNumRow-1 || CurDie.Col == 0) break;
			ulLaRow = CurDie.Row+1;
			ulLaCol = CurDie.Col-1;
			break;
		case 6:
			if (CurDie.Row == 0 || CurDie.Col == ulNumCol-1) break;
			ulLaRow = CurDie.Row-1;
			ulLaCol = CurDie.Col+1;
			break;
		case 7:
			if (CurDie.Row == ulNumRow-1 || CurDie.Col == ulNumCol-1) break;
			ulLaRow = CurDie.Row+1;
			ulLaCol = CurDie.Col+1;
			break;
		}
		if (ulLaRow==1000 || ulLaCol==1000)
		{
			continue;
		}

		if ( bArResult[lDir] == TRUE )
		{
			m_stnWaferTable->m_WaferMapWrapper.SetDieState(ulLaRow,ulLaCol, WT_MAP_DIESTATE_15);
		}
	}

}

LONG CWT_BlkFunc::InitBlockFunc()
{
	LONG i,j;

	RefDieInfo.sNum = 0;
	RefDieInfo.RefDist.Row = 0;
	RefDieInfo.RefDist.Col = 0;
	RefDieInfo.MaxRC.Row = 0;
	RefDieInfo.MaxRC.Col = 0;
	RefDieInfo.MinRC.Row = 0;
	RefDieInfo.MinRC.Col = 0;
	for (i=0; i<MaxNumOfStep; i++)
	{
		RefDieInfo.arrJStep[i] = 0;
	}
	for (i=0; i<MaxNumOfFDC; i++)
	{
		RefDieInfo.RefDie[i].InMap = 0;
		RefDieInfo.RefDie[i].InWaf = 0;
	}

	for (i=0; i<MaxRow; i++)
	for (j=0; j<MaxCol; j++)
	{
		DiePhyPrep[i][j].x = 0;
		DiePhyPrep[i][j].y = 0;
		DiePhyPrep[i][j].t = 0;
		DiePhyPrep[i][j].Status = Regn_Init;
	}

	FDCBasic.HaveAligned = 0;

	LONG PitchX_X, PitchX_Y;
	LONG PitchY_X, PitchY_Y;
    m_stnWaferTable->GetDiePitchX(&PitchX_X, &PitchX_Y);
    m_stnWaferTable->GetDiePitchY(&PitchY_X, &PitchY_Y);
	FDCBasic.HPitchDie.x = -PitchX_X;
	FDCBasic.HPitchDie.y = -PitchX_Y;
	FDCBasic.VPitchDie.x = -PitchY_X;
	FDCBasic.VPitchDie.y = -PitchY_Y;

	FDCBasic.HPitchFDC.x = FDCBasic.HPitchFDC.y = 0;
	FDCBasic.VPitchFDC.x = FDCBasic.VPitchFDC.y = 0;

	//Delete debug file
	strTemp.Format("FDCRegn.log");
	TRY
	{
		CFile::Remove(strTemp);
	}
	CATCH(CFileException,e)
	{
		;//e->ReportError();
	}
	END_CATCH
	strTemp.Format("FDCBlock.log");
	TRY
	{
		CFile::Remove(strTemp);
	}
	CATCH(CFileException,e)
	{
		;//e->ReportError();
	}
	END_CATCH

	if (m_stnWaferTable->m_bFullRefBlock)
	{
		GetRefDieArray();
	}

	return TRUE;
}

BOOL CWT_BlkFunc::DrawBlockMap()
{
	DieMapPos DieTL,DieBR;

	for (int i = 1; i < MaxRefWidth - 1; i++)
	{
		DieTL.Row = RefDieInfo.RefDie[i*MaxRefWidth+0].MapPos.Row+1;
		DieTL.Col = RefDieInfo.RefDie[i*MaxRefWidth+0].MapPos.Col;
		DieBR.Row = RefDieInfo.RefDie[(i+1)*MaxRefWidth+MaxRefWidth-1].MapPos.Row+1;
		DieBR.Col = RefDieInfo.RefDie[(i+1)*MaxRefWidth+MaxRefWidth-1].MapPos.Col+1;
		m_stnWaferTable->m_pWaferMapManager->AddBoundary(DieTL.Row,DieBR.Row,DieTL.Col,DieBR.Col);
	}

	for (int j = 1; j < MaxRefWidth - 1; j++)
	{
		DieTL.Row = RefDieInfo.RefDie[j].MapPos.Row;
		DieTL.Col = RefDieInfo.RefDie[j].MapPos.Col+1;
		DieBR.Row = RefDieInfo.RefDie[(MaxRefWidth-1)*MaxRefWidth+j+1].MapPos.Row+1;
		DieBR.Col = RefDieInfo.RefDie[(MaxRefWidth-1)*MaxRefWidth+j+1].MapPos.Col+1;
		m_stnWaferTable->m_pWaferMapManager->AddBoundary(DieTL.Row,DieBR.Row,DieTL.Col,DieBR.Col);
	}

	return TRUE;
}

//================================================================
// GET function implementation section
//================================================================
BOOL CWT_BlkFunc::SearchCurrDie(LONG lSrchWin, LONG lPrId, LONG *lPosX, LONG *lPosY)
{
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	LONG	lX,lY;

	LONG	lSrchDieMsg[2];

	typedef struct
	{
		BOOL bRtn;
		LONG lX;
		LONG lY;
	} WTPos;
 	WTPos stPRS;

	// Init Message
	lSrchDieMsg[0] = lSrchWin;
	lSrchDieMsg[1] = lPrId;
	stMsg.InitMessage(2*sizeof(LONG), lSrchDieMsg);

	// Get the reply for PR result
	nConvID = m_stnWaferTable->GetIPCClientCom().SendRequest(WAFER_PR_STN, "SearchDieInWafer", stMsg);
	while (1)
	{
		if( m_stnWaferTable->GetIPCClientCom().ScanReplyForConvID(nConvID, 30000) == TRUE )
		{
			m_stnWaferTable->GetIPCClientCom().ReadReplyForConvID(nConvID, stMsg);

			stMsg.GetMsg(sizeof(WTPos), &stPRS);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (stPRS.bRtn == TRUE)
	{
		lX = stPRS.lX;
		lY = stPRS.lY;
	}
	else
	{
		lX = 0;
		lY = 0;
		return FALSE;
	}

	*lPosX = *lPosX + lX;
	*lPosY = *lPosY + lY;

	if (m_stnWaferTable->GetRtnForCheckWaferLimit(*lPosX,*lPosY) == FALSE)
	{
		return FALSE;	//WT_ALN_OUT_WAFLIMIT;
	}
	WftMoveXYTo(*lPosX, *lPosY);
	Sleep(50);

	return TRUE;
}

BOOL CWT_BlkFunc::SearchAroundDie(LONG lDir, LONG *lPosX, LONG *lPosY)
{
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	LONG	lX,lY;

	LONG	lSrchDieMsg[2];

	typedef struct
	{
		LONG lSrchMode;
		LONG lX;
		LONG lY;
		LONG lResult;
	} WTPos;
 	WTPos stPRS;


	// Init Message
	lSrchDieMsg[0] = 3;
	lSrchDieMsg[1] = lDir;
	stMsg.InitMessage(2*sizeof(LONG), lSrchDieMsg);

	// Get the reply for PR result
	nConvID = m_stnWaferTable->GetIPCClientCom().SendRequest(WAFER_PR_STN, "SearchNmlDieAround", stMsg);
	while (1)
	{
		if( m_stnWaferTable->GetIPCClientCom().ScanReplyForConvID(nConvID, 30000) == TRUE )
		{
			m_stnWaferTable->GetIPCClientCom().ReadReplyForConvID(nConvID, stMsg);

			stMsg.GetMsg(sizeof(WTPos), &stPRS);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (stPRS.lResult == TRUE)
	{
		lX = stPRS.lX;
		lY = stPRS.lY;
	}
	else
	{
		lX = 0;
		lY = 0;
		return FALSE;
	}

	*lPosX = *lPosX + lX;
	*lPosY = *lPosY + lY;

	Sleep(10);
	return TRUE;
}


VOID CWT_BlkFunc::GetWTPosition(LONG *lX, LONG *lY, LONG *lT)
{
	// Get Encoder Value
	m_stnWaferTable->GetEncoder(lX, lY, lT);

}


LONG CWT_BlkFunc::GetNormalDieStatus(LONG *lCurX, LONG *lCurY)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	typedef struct {
		BOOL bStatus;
		BOOL bGoodDie;
		BOOL bFullDie;
		LONG lRefDieNo;
		LONG lX;
		LONG lY;
	} REF_TYPE;
	REF_TYPE	stInfo;

	typedef struct {
		BOOL		bShowPRStatus;
		BOOL		bNormalDie;
		LONG		lRefDieNo;
		BOOL		bDisableBackupAlign;
	} SRCH_TYPE;

	SRCH_TYPE	stSrchInfo;

	//Init Message
	stSrchInfo.bShowPRStatus = FALSE;
	stSrchInfo.bNormalDie	= TRUE;
	stSrchInfo.lRefDieNo	= 1;
	stSrchInfo.bDisableBackupAlign = FALSE;

	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
	nConvID = m_stnWaferTable->GetIPCClientCom().SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
	while (1)
	{
		if( m_stnWaferTable->GetIPCClientCom().ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_stnWaferTable->GetIPCClientCom().ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	*lCurX += stInfo.lX;	
	*lCurY += stInfo.lY;

	//Move table to updated position
	if (m_stnWaferTable->GetRtnForCheckWaferLimit(*lCurX,*lCurY) == FALSE)
	{
		return WT_ALN_OUT_WAFLIMIT;
	}

	WftMoveXYTo(*lCurX, *lCurY);
	Sleep(50);

	if (stInfo.bStatus == FALSE)
	{
		return WT_ALN_SEARCH_DIE_ERROR;
	}
	else
	{
		if (stInfo.bFullDie == TRUE)
		{
			if (stInfo.bGoodDie == FALSE)
			{
				return WT_ALN_IS_DEFECT;	//Defect die found
			}
		}
		else
		{
			return WT_ALN_IS_EMPTY;		//Empty die found
		}
	}

	return WT_ALN_IS_GOOD;	//Good die found
}


LONG CWT_BlkFunc::ResetJStep(LONG lJDist)
{
	int		i;
	LONG	lDistLeft;

	for (i=0; i<MaxNumOfStep; i++)
	{
		RefDieInfo.arrJStep[i] = 0;
	}

	if ( MnSrchRefGrid <= 0 || lJDist <= 0 ) return FALSE;

	lDistLeft = lJDist;
	i = 0;
	while (lDistLeft >=0)
	{
		lDistLeft -= MnSrchRefGrid;
		if (lDistLeft >= 0)
			RefDieInfo.arrJStep[i] = MnSrchRefGrid;
		else
			RefDieInfo.arrJStep[i] = MnSrchRefGrid + lDistLeft;

		if (Blk_bDebug == 1)
		{
			FILE	*fp = NULL;
			errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
			if ((nErr == 0) && (fp != NULL))		//Klocwork
			{
				fclose(fp);
			}
		}

		i++;
	}

	return TRUE;
}

LONG CWT_BlkFunc::GetNextRefDieXY(LONG lRow, LONG lCol, LONG *lX, LONG *lY)
{
	int i;
	DiePhyPos	CurPhyPos;
	DiePhyPos	SrcPhyPos;

	LONG	lJStep;
	LONG	lResult;
	LONG	lPosX, lPosY;

	SrcPhyPos.x = *lX;
	SrcPhyPos.y = *lY;

	CurPhyPos.x = *lX;
	CurPhyPos.y = *lY;

	//Move on y direction
	if (lRow != 0)
	{
		i = 0;
		ResetJStep(abs(lRow));
		while (RefDieInfo.arrJStep[i] != 0)
		{
			if (lRow < 0)
			{
				//To Up Direction
				lJStep = RefDieInfo.arrJStep[i];
			}
			else
			{
				//To Dn Direction
				lJStep = RefDieInfo.arrJStep[i]*(-1);
			}

			CurPhyPos.x -= lJStep*(-FDCBasic.VPitchDie.x);
			CurPhyPos.y -= lJStep*(-FDCBasic.VPitchDie.y);

			if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhyPos.x,CurPhyPos.y) == FALSE)
			{
				*lX = SrcPhyPos.x;
				*lY = SrcPhyPos.y;
				return FALSE;
			}
			WftMoveXYTo(CurPhyPos.x, CurPhyPos.y);

			//Display map index
			Sleep(200);
			if (RefDieInfo.arrJStep[i+1]!=0)
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult = GetNormalDieStatus(&lPosX, &lPosY);

				if ( lResult == WT_ALN_IS_GOOD || lResult == WT_ALN_IS_DEFECT )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
				}
				i++;
			}
			else
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult = SearchCurrDie(1, 1, &lPosX, &lPosY);
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
					i++;
				}
				else
				{
					*lX = SrcPhyPos.x;
					*lY = SrcPhyPos.y;
					return FALSE;
				}
			}
		}//endwhile
	}//end y direction

	//Move on x direction
	if (lCol != 0)
	{
		i = 0;
		ResetJStep(abs(lCol));
		while (RefDieInfo.arrJStep[i] != 0)
		{
			if (lCol < 0)
			{
				//To Lf Direction
				lJStep = RefDieInfo.arrJStep[i];
			}
			else
			{
				//To Rt Direction
				lJStep = RefDieInfo.arrJStep[i]*(-1);
			}

			CurPhyPos.x -= lJStep*(-FDCBasic.HPitchDie.x);
			CurPhyPos.y -= lJStep*(-FDCBasic.HPitchDie.y);

			if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhyPos.x,CurPhyPos.y) == FALSE)
			{
				*lX = SrcPhyPos.x;
				*lY = SrcPhyPos.y;
				return FALSE;
			}
			WftMoveXYTo(CurPhyPos.x, CurPhyPos.y);

			//Display map index
			Sleep(200);
			if (RefDieInfo.arrJStep[i+1]!=0)
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult = GetNormalDieStatus(&lPosX, &lPosY);

				if ( lResult == WT_ALN_IS_GOOD || lResult == WT_ALN_IS_DEFECT )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
				}
				i++;
			}
			else
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult = SearchCurrDie(1, 1, &lPosX, &lPosY);
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
					i++;
				}
				else
				{
					*lX = SrcPhyPos.x;
					*lY = SrcPhyPos.y;
					return FALSE;
				}
			}
		}//endwhile
	}//end x direction

	*lX = CurPhyPos.x;
	*lY = CurPhyPos.y;
	return TRUE;
}

LONG CWT_BlkFunc::GetNextRefDieXY(DiePhyPos PhyInput, LONG lRow, LONG lCol, DiePhyPos *PhyOutput)
{
	int i;
	DiePhyPos	CurPhyPos;
	DiePhyPos	SrcPhyPos;

	LONG	lJStep;
	LONG	lResult;
	LONG	lPosX, lPosY;//, lPosT;

	SrcPhyPos.x = PhyInput.x;
	SrcPhyPos.y = PhyInput.y;

	CurPhyPos.x = PhyInput.x;
	CurPhyPos.y = PhyInput.y;

	//Move on y direction
	if (lRow != 0)
	{
		i = 0;
		ResetJStep(abs(lRow));
		while (RefDieInfo.arrJStep[i] != 0)
		{
			if (lRow < 0)
			{
				//To Up Direction
				lJStep = RefDieInfo.arrJStep[i];
			}
			else
			{
				//To Dn Direction
				lJStep = RefDieInfo.arrJStep[i]*(-1);
			}

			CurPhyPos.x -= lJStep*(-FDCBasic.VPitchDie.x);
			CurPhyPos.y -= lJStep*(-FDCBasic.VPitchDie.y);

			if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhyPos.x,CurPhyPos.y) == FALSE)
			{
				PhyOutput->x = SrcPhyPos.x;
				PhyOutput->y = SrcPhyPos.y;
				return FALSE;
			}
			WftMoveXYTo(CurPhyPos.x, CurPhyPos.y);
			Sleep(200);

			//Display map index
			if (RefDieInfo.arrJStep[i+1]!=0)
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);

				//if ( lResult == WT_ALN_IS_GOOD || lResult == WT_ALN_IS_DEFECT )
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
				}
				i++;
			}
			else
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult = SearchCurrDie(1, 1, &lPosX, &lPosY);
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
					i++;
				}
				else
				{
					if ( m_bRefRhombus )
					{
						lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);
						if ( lResult == TRUE )
						{
							CurPhyPos.x = lPosX;
							CurPhyPos.y = lPosY;
							i++;
						}
						else
						{
							PhyOutput->x = SrcPhyPos.x;
							PhyOutput->y = SrcPhyPos.y;
							return FALSE;
						}
					}
					else
					{
						PhyOutput->x = SrcPhyPos.x;
						PhyOutput->y = SrcPhyPos.y;
						return FALSE;
					}
				}
			}
		}//endwhile
	}//end y direction

	//Move on x direction
	if (lCol != 0)
	{
		i = 0;
		ResetJStep(abs(lCol));
		while (RefDieInfo.arrJStep[i] != 0)
		{
			if (lCol < 0)
			{
				//To Lf Direction
				lJStep = RefDieInfo.arrJStep[i];
			}
			else
			{
				//To Rt Direction
				lJStep = RefDieInfo.arrJStep[i]*(-1);
			}

			CurPhyPos.x -= lJStep*(-FDCBasic.HPitchDie.x);
			CurPhyPos.y -= lJStep*(-FDCBasic.HPitchDie.y);

			if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhyPos.x,CurPhyPos.y) == FALSE)
			{
				PhyOutput->x = SrcPhyPos.x;
				PhyOutput->y = SrcPhyPos.y;
				return FALSE;	//WT_ALN_OUT_WAFLIMIT;
			}
			WftMoveXYTo(CurPhyPos.x, CurPhyPos.y);
			Sleep(200);

			//Display map index
			if (RefDieInfo.arrJStep[i+1]!=0)
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				//lResult = GetNormalDieStatus(&lPosX, &lPosY);
				lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);

				//if ( lResult == WT_ALN_IS_GOOD || lResult == WT_ALN_IS_DEFECT )
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
				}
				i++;
			}
			else
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult = SearchCurrDie(1, 1, &lPosX, &lPosY);
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
					i++;
				}
				else
				{
					if ( m_bRefRhombus )
					{
						lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);
						if ( lResult == TRUE )
						{
							CurPhyPos.x = lPosX;
							CurPhyPos.y = lPosY;
							i++;
						}
						else
						{
							PhyOutput->x = SrcPhyPos.x;
							PhyOutput->y = SrcPhyPos.y;
							return FALSE;
						}
					}
					else
					{
						PhyOutput->x = SrcPhyPos.x;
						PhyOutput->y = SrcPhyPos.y;
						return FALSE;
					}
				}
			}
		}//endwhile
	}//end x direction

	PhyOutput->x = CurPhyPos.x;
	PhyOutput->y = CurPhyPos.y;
	return TRUE;
}

BOOL CWT_BlkFunc::SetHomeDiePos(ULONG ulRow, ULONG ulCol)
{
	LONG	lX,lY,lT;
	BOOL	bResult;

	InitBlockFunc();
	m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
	DrawBlockMap();

	GetWTPosition(&lX, &lY, &lT);
	bResult = SearchCurrDie(0, 0, &lX, &lY);

	FDCBasic.HomeDie.PhyPos.x = lX;
	FDCBasic.HomeDie.PhyPos.y = lY;
	FDCBasic.HomeDie.PhyPos.t = lT;
	FDCBasic.HomeDie.MapPos.Row = ulRow;
	FDCBasic.HomeDie.MapPos.Col = ulCol;

	return TRUE;
}

BOOL CWT_BlkFunc::FoundAllRefDiePos()
{
	LONG		i,j;
	DiePhyPos**	pRefDiePos;
	DieMapPos*	pDieStack;
	LONG		lX,lY,lT;
	int			iJStep;

	ULONG		ulIndex;
	LONG		sResult;

	int			DieStackPtr;
	DieMapPos	CurrRC, MinRC, MaxRC;
	DiePhyPos	CurPhy, TgtPhy, HomePhy;
	int			iRefCount = 0;

    CMS896AApp::m_bStopAlign = FALSE;

	GetWTPosition(&lX, &lY, &lT);
	sResult = SearchCurrDie(1, 1, &lX, &lY);
	if (sResult == FALSE)
	{
		return FALSE;
	}

	CurPhy.x = lX;
	CurPhy.y = lY;
	HomePhy.x = lX;
	HomePhy.y = lY;
	HomePhy.t = lT;
	FDCBasic.HomeDie.PhyPos.x = lX;
	FDCBasic.HomeDie.PhyPos.y = lY;
	FDCBasic.HomeDie.PhyPos.t = lT;

	InitBlockFunc();
	m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
	DrawBlockMap();

	pRefDiePos = new DiePhyPos*[MaxRefWidth];
	for (i=0; i<MaxRefWidth; i++) 
	{
		pRefDiePos[i] = new DiePhyPos[MaxRefWidth];
		for (j=0; j<MaxRefWidth; j++) 
		{
			pRefDiePos[i][j].x = 0;
			pRefDiePos[i][j].y = 0;
			pRefDiePos[i][j].t = FDCBasic.HomeDie.PhyPos.t;
			pRefDiePos[i][j].Status = 0;
		}
	}

	pDieStack = new DieMapPos[MaxNumOfFDC];
	for (i=0; i<MaxNumOfFDC; i++)
	{
		pDieStack[i].Row = 0;
		pDieStack[i].Col = 0;
		pDieStack[i].Status = 0;
	}

	iJStep = RefDieInfo.RefDist.Row;

	ulIndex = (MaxRefWidth/2) * MaxRefWidth + MaxRefWidth/2;
	pRefDiePos[MaxRefWidth/2][MaxRefWidth/2].x = lX;
	pRefDiePos[MaxRefWidth/2][MaxRefWidth/2].y = lY;
	pRefDiePos[MaxRefWidth/2][MaxRefWidth/2].Status = 1;

	MinRC.Row = MinRC.Col = MaxRefWidth/2;
	MaxRC.Row = MaxRC.Col = MaxRefWidth/2;
	CurrRC.Row = CurrRC.Col = MaxRefWidth/2;

	pDieStack[0].Row = MaxRefWidth/2;
	pDieStack[0].Col = MaxRefWidth/2;
	DieStackPtr = 1;

	GetWTPosition(&lX, &lY, &lT);
	CurPhy.x = lX;
	CurPhy.y = lY;

	// Alignment in Cross Path
	sResult = IndexCrossRefDiePos(pRefDiePos, pDieStack, &DieStackPtr);
	if (sResult == FALSE) 
	{
		//Release Pointer
		for (i=0; i<MaxRefWidth; i++)
			delete[] pRefDiePos[i];
		delete[] pRefDiePos;
		delete[] pDieStack;
		return FALSE;
	}

	while (DieStackPtr > 0)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			//Release Pointer
			for (i=0; i<MaxRefWidth; i++)
				delete[] pRefDiePos[i];
			delete[] pRefDiePos;
			delete[] pDieStack;
			return FALSE;
		}

		if (CurrRC.Row < MinRC.Row ) MinRC.Row = CurrRC.Row;
		if (CurrRC.Col < MinRC.Col ) MinRC.Col = CurrRC.Col;
		if (CurrRC.Row > MaxRC.Row ) MaxRC.Row = CurrRC.Row;
		if (CurrRC.Col > MaxRC.Col ) MaxRC.Col = CurrRC.Col;
	
		if (   CurrRC.Row > 0
			&& pRefDiePos[CurrRC.Row-1][CurrRC.Col].Status == 0
		   )
		{
			sResult = GetNextRefDieXY(CurPhy,iJStep, 0, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row - 1;
				pDieStack[DieStackPtr].Col = CurrRC.Col;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Row -= 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].Status = -1;
			}
		}
		if (   CurrRC.Col > 0
			&& pRefDiePos[CurrRC.Row][CurrRC.Col-1].Status == 0
		   )
		{
			sResult = GetNextRefDieXY(CurPhy, 0, iJStep, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row;
				pDieStack[DieStackPtr].Col = CurrRC.Col - 1;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Col -= 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].Status = -1;
			}
		}
		if (   CurrRC.Row < MaxRefWidth-1
			&& pRefDiePos[CurrRC.Row+1][CurrRC.Col].Status == 0
		   )
		{
			sResult = GetNextRefDieXY(CurPhy, -iJStep, 0, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row + 1;
				pDieStack[DieStackPtr].Col = CurrRC.Col;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Row += 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].Status = -1;
			}
		}
		if (   CurrRC.Col < MaxRefWidth-1
			&& pRefDiePos[CurrRC.Row][CurrRC.Col+1].Status == 0
		   )
		{
			sResult = GetNextRefDieXY(CurPhy, 0, -iJStep, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row;
				pDieStack[DieStackPtr].Col = CurrRC.Col + 1;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Col += 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].Status = -1;
			}
		}

		iRefCount = iRefCount + 1;
		DieStackPtr--;
		if (DieStackPtr!=0)
		{
			CurrRC.Row = pDieStack[DieStackPtr-1].Row;
			CurrRC.Col = pDieStack[DieStackPtr-1].Col;
			CurPhy.x = pRefDiePos[CurrRC.Row][CurrRC.Col].x;
			CurPhy.y = pRefDiePos[CurrRC.Row][CurrRC.Col].y;
		}
		if (CurrRC.Row < MinRC.Row ) MinRC.Row = CurrRC.Row;
		if (CurrRC.Col < MinRC.Col ) MinRC.Col = CurrRC.Col;
		if (CurrRC.Row > MaxRC.Row ) MaxRC.Row = CurrRC.Row;
		if (CurrRC.Col > MaxRC.Col ) MaxRC.Col = CurrRC.Col;
	
	}//endwhile

	MinRC.Row = MinRC.Col = MaxRefWidth/2;
	MaxRC.Row = MaxRC.Col = MaxRefWidth/2;
	for (i=0; i<MaxRefWidth; i++)
	for (j=0; j<MaxRefWidth; j++)
	{
		if (pRefDiePos[i][j].Status == 1)
		{		
			if (i < MinRC.Row ) MinRC.Row = i;
			if (j < MinRC.Col ) MinRC.Col = j;
			if (i > MaxRC.Row ) MaxRC.Row = i;
			if (j > MaxRC.Col ) MaxRC.Col = j;
		}
	}
    
	if ( Blk_bDebug )
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"iRefCount = %d\n",iRefCount);
			fprintf(fp,"MinRC(%ld,%ld),MaxRC(%ld,%ld)\n",MinRC.Row,MinRC.Col,MaxRC.Row,MaxRC.Col);
			fclose(fp);
		}
	}

	sResult = MergeAlignMap(pRefDiePos, MinRC, MaxRC);

	//Release Pointer
	for (i=0; i<MaxRefWidth; i++)
		delete[] pRefDiePos[i];
	delete[] pRefDiePos;
	delete[] pDieStack;

	if (sResult == FALSE)
	{
		LONG lTmp;

		lTmp = m_stnWaferTable->GetRtnForHmiMessage("Do you want to continue by\n\n    Manual-Alignment", "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		if (lTmp == glHMI_NO)
		{
			m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
			return FALSE;
		}
		sResult = ManAlignWafer(HomePhy);
		if (sResult == FALSE)
		{
			m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
			return FALSE;
		}
	}

	//Show RefDie
	if (sResult == TRUE)
	{
		DieMapPos DieTL,DieBR;
		ULONG	ulIndex,Count;
		LONG	ulRow, ulCol;

		Count = 0;
        for (i=0; i<MaxRefWidth; i++)
		for (j=0; j<MaxRefWidth; j++)
		{
			ulIndex = i*MaxRefWidth + j;
			if (RefDieInfo.RefDie[ulIndex].InWaf == 1)
			{
				Count++;
				DieTL.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
				DieTL.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
				DieBR.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row+2;
				DieBR.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col+2;

				m_stnWaferTable->m_pWaferMapManager->AddBoundary(DieTL.Row,DieBR.Row,DieTL.Col,DieBR.Col);

				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", RefDieInfo.RefDie[ulIndex].MapPos.Row);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", RefDieInfo.RefDie[ulIndex].MapPos.Col);
				m_stnWaferTable->m_WaferMapWrapper.SetPhysicalPosition(DieTL.Row,DieTL.Col,RefDieInfo.RefDie[ulIndex].PhyPos.x,RefDieInfo.RefDie[ulIndex].PhyPos.y);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);

				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", i);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", j);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_AtRef);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);
			}
		}

		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", MnMaxJumpCtr);
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", MnMaxJumpEdge);
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Init);
		m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);

		SetStartDie(HomePhy);
		GetStartDie(&ulRow, &ulCol, &lX, &lY);
		m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(ulRow,ulCol);

	}

	FDCBasic.HaveAligned = 1;
	AlignTableTheta(HomePhy);

	//Get edge position
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->IsStopAlign())		//v4.05	//Klocwork
	{
		m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
		return FALSE;
	}
	RealignRefDiePos(TRUE,FALSE);

	CheckDieInSpecialGrade(MnNoDieGrade);

	//Reset update value
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);

	return TRUE;
}

BOOL CWT_BlkFunc::MergeAlignMap(DiePhyPos **pRefArray, DieMapPos MinRC, DieMapPos MaxRC)
{
	int i,j;
	short ulCounter;
	unsigned long ulIndex1, ulIndex2;

	if (Blk_bDebug == 1)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"MergeAlignMap()---Begin\n");
			fclose(fp);
		}
	}

	//Update RefDieBlock
	ulCounter = 0;
	for (i=MinRC.Row; i<=MaxRC.Row; i++)
	for (j=MinRC.Col; j<=MaxRC.Col; j++)
	{
		ulIndex1 = i*MaxRefWidth + j;
		ulIndex2 = (i-MinRC.Row+1)*MaxRefWidth + j-MinRC.Col+1;
		if (pRefArray[i][j].Status == 1
			&& RefDieInfo.RefDie[ulIndex2].InMap == 1)
		{
			RefDieInfo.RefDie[ulIndex2].PhyPos.x = pRefArray[i][j].x;
			RefDieInfo.RefDie[ulIndex2].PhyPos.y = pRefArray[i][j].y;
			RefDieInfo.RefDie[ulIndex2].PhyPos.t = pRefArray[i][j].t;
			RefDieInfo.RefDie[ulIndex2].InWaf = 1;

			DiePhyPrep[RefDieInfo.RefDie[ulIndex2].MapPos.Row][RefDieInfo.RefDie[ulIndex2].MapPos.Col].x = RefDieInfo.RefDie[ulIndex2].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex2].MapPos.Row][RefDieInfo.RefDie[ulIndex2].MapPos.Col].y = RefDieInfo.RefDie[ulIndex2].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex2].MapPos.Row][RefDieInfo.RefDie[ulIndex2].MapPos.Col].t = RefDieInfo.RefDie[ulIndex2].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex2].MapPos.Row][RefDieInfo.RefDie[ulIndex2].MapPos.Col].Status = Regn_Align;

			ulCounter++;
		}
	}

	if (ulCounter == RefDieInfo.sNum)
	{
		if (Blk_bDebug == 1)
		{
			FILE	*fp = NULL;
			errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
			if ((nErr == 0) && (fp != NULL))		//Klocwork
			{
				fprintf(fp,"MergeAlignMap()---END TRUE\n");
				fclose(fp);
			}
			PrintFDC();
		}

		return TRUE;
	}

	if (   RefDieInfo.MaxRC.Row - RefDieInfo.MinRC.Row == MaxRC.Row - MinRC.Row
		&& RefDieInfo.MaxRC.Col - RefDieInfo.MinRC.Col == MaxRC.Col - MinRC.Col )
	{
		if ( Blk_bDebug )
		{
			FILE	*fp = NULL;
			errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
			if ((nErr == 0) && (fp != NULL))		//Klocwork
			{
				fprintf(fp,"MergeAlignMap()---END TRUE\n");
				fclose(fp);
			}
			PrintFDC();
		}

		return TRUE;
	}

	if ( Blk_bDebug )
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"MergeAlignMap()---END FALSE\n");
			fclose(fp);
		}
	}

	return FALSE;
}

LONG CWT_BlkFunc::IndexCrossRefDiePos(DiePhyPos **pRefDie, DieMapPos *pStack, int *pStackPtr)
{
	int			i,j;
	DieMapPos	HomeRC, CurrRC;
	DiePhyPos	HomePhy,CurrPhy;
	DieMapPos	MinRC, MaxRC;
	DiePhyPos	TgtPhy = {0,0};

	int			DieStackPtr;
	int			iRefWidth = MaxRefWidth;

	LONG		sResult;
	LONG		iJStep;
	LONG		lX, lY, lT;

	//--------
	if (Blk_bDebug == 1)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"IndexCrossRefDiePos()--Begin\n");
			fclose(fp);
		}
	}

	DieStackPtr = *pStackPtr;
	CurrRC.Row = pStack[DieStackPtr-1].Row;
	CurrRC.Col = pStack[DieStackPtr-1].Col;
	GetWTPosition(&lX, &lY, &lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//Save HomeDie
	HomeRC = CurrRC;
	HomePhy = CurrPhy;

	iJStep = RefDieInfo.RefDist.Row;
	MinRC = MaxRC = CurrRC;
	while(CurrRC.Row > 0)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextRefDieXY(CurrPhy, iJStep, 0, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row-1][CurrRC.Col].x = TgtPhy.x;
			pRefDie[CurrRC.Row-1][CurrRC.Col].y = TgtPhy.y;
			pRefDie[CurrRC.Row-1][CurrRC.Col].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row - 1;
			pStack[DieStackPtr].Col = CurrRC.Col;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Row -= 1;
			if (CurrRC.Row < MinRC.Row ) MinRC.Row = CurrRC.Row;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row-1][CurrRC.Col].Status = -1;
			break;
		}
	}

	//Move to HomeDie
	CurrRC = HomeRC;
	CurrPhy = HomePhy; 
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	while(CurrRC.Row < MaxRefWidth-1)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextRefDieXY(CurrPhy, -iJStep, 0, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row+1][CurrRC.Col].x = TgtPhy.x;
			pRefDie[CurrRC.Row+1][CurrRC.Col].y = TgtPhy.y;
			pRefDie[CurrRC.Row+1][CurrRC.Col].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row + 1;
			pStack[DieStackPtr].Col = CurrRC.Col;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Row += 1;
			if (CurrRC.Row > MaxRC.Row ) MaxRC.Row = CurrRC.Row;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row+1][CurrRC.Col].Status = -1;
			break;
		}
	}

	//Move to HomeDie
	CurrRC = HomeRC;
	CurrPhy = HomePhy; 
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	iJStep = RefDieInfo.RefDist.Col;
	while(CurrRC.Col > 0)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextRefDieXY(CurrPhy, 0, iJStep, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col-1].x = TgtPhy.x;
			pRefDie[CurrRC.Row][CurrRC.Col-1].y = TgtPhy.y;
			pRefDie[CurrRC.Row][CurrRC.Col-1].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row;
			pStack[DieStackPtr].Col = CurrRC.Col - 1;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Col -= 1;
			if (CurrRC.Col < MinRC.Col ) MinRC.Col = CurrRC.Col;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col-1].Status = -1;
			break;
		}
	}

	//Move to HomeDie
	CurrRC = HomeRC;
	CurrPhy = HomePhy; 
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	while(CurrRC.Col < MaxRefWidth-1)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextRefDieXY(CurrPhy, 0, -iJStep, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col+1].x = TgtPhy.x;
			pRefDie[CurrRC.Row][CurrRC.Col+1].y = TgtPhy.y;
			pRefDie[CurrRC.Row][CurrRC.Col+1].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row;
			pStack[DieStackPtr].Col = CurrRC.Col + 1;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Col += 1;
			if (CurrRC.Col > MaxRC.Col ) MaxRC.Col = CurrRC.Col;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col+1].Status = -1;
			break;
		}
	}

	//Update Input Parameter
	*pStackPtr = DieStackPtr;

	// Add limit
	if ( RefDieInfo.MinRC.Row - RefDieInfo.MaxRC.Row == MinRC.Row - MaxRC.Row )
	{
		for (j=0; j<MaxRefWidth; j++)
		{
			pRefDie[MinRC.Row-1][j].Status = 2;
			pRefDie[MaxRC.Row+1][j].Status = 2;
		}
	}

	if ( RefDieInfo.MinRC.Col - RefDieInfo.MaxRC.Col == MinRC.Col - MaxRC.Col )
	{
		for (i=0; i<MaxRefWidth; i++)
		{
			pRefDie[i][MinRC.Col-1].Status = 2;
			pRefDie[i][MaxRC.Col+1].Status = 2;
		}
	}

	//Move to HomeDie
	WftMoveXYTo(HomePhy.x, HomePhy.y);

	if (Blk_bDebug == 1)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"%d RefDie in Cross\n", DieStackPtr);
			fprintf(fp,"IndexCrossRefDiePos()---End\n");
			fclose(fp);
		}
	}

	return TRUE;
}

LONG CWT_BlkFunc::ManAlignWafer(DiePhyPos CurTbl)
{
	int i,j;
	ULONG		ulIndex, ulIndex1, ulIndex2;
	LONG		iJStep;
	DieMapPos	CurrMap;
	DiePhyPos	CurrPhy, TgtPhy;
	int			iLimitMin=0, iLimitMax=0;	//Klocwork
	LONG		lX, lY, lT;

	LONG		sResult;
	DieMapPos	RefDieMap, CtrMap;
	DiePhyPos	RefDiePhy;
	BOOL		bFindCtr;
	LONG		lTmp;

	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		RefDieInfo.RefDie[ulIndex].InWaf = 0;
	}

	//Move to center
 	CurrPhy.x = CurTbl.x;	//0;
	CurrPhy.y = CurTbl.y;	//0;
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	//Adjust Joystick
	IPC_CServiceMessage stMsg;
	LONG lRtn = 0;
	LONG lLevel = 1;

	stMsg.InitMessage(sizeof(LONG), &lLevel);
	lRtn = m_stnWaferTable->SetJoystickSpeedCmd(stMsg);


	//Ask for moving table to center
	m_stnWaferTable->GetRtnForSetJoystick(TRUE);
	//m_stnWaferTable->GetRtnForHmiMessage("Please move to center(500,500)\nAnd press ENTER", "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	m_stnWaferTable->GetRtnForHmiMessageEx("Please move to center(500,500)\nAnd press ENTER", "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	m_stnWaferTable->GetRtnForSetJoystick(FALSE);
	GetWTPosition(&lX,&lY,&lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;

	bFindCtr = FALSE;
	CtrMap.Row = 500;
	CtrMap.Col = 500;
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].SrcMap.Row == CtrMap.Row && RefDieInfo.RefDie[ulIndex].SrcMap.Col == CtrMap.Col)
		{
			sResult = SearchCurrDie(1, 1, &lX, &lY);
			if ( sResult == TRUE )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				RefDieInfo.RefDie[ulIndex].InWaf = 1;
			}
			else
			{
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				RefDieInfo.RefDie[ulIndex].InWaf = 2;
			}

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;

			CurrMap.Row = i;
			CurrMap.Col = j;
			bFindCtr = TRUE;
			break;
		}
	}

	if (bFindCtr == FALSE) return FALSE;

	// Find TopLeft
	i = CurrMap.Row;
	j = CurrMap.Col;
	iJStep = RefDieInfo.RefDist.Row;

	// Top
	while (i>RefDieInfo.MinRC.Row)
	{
		i--;
		ulIndex = i*MaxRefWidth + j;
		sResult = GetNextRefDieXY(CurrPhy, iJStep, 0, &TgtPhy);
		if (sResult == TRUE)
		{
			RefDieInfo.RefDie[ulIndex].PhyPos.x = TgtPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = TgtPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			RefDieInfo.RefDie[ulIndex].InWaf = 1;

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;

			CurrPhy = TgtPhy;
			//continue;
		}
		else if (sResult == FALSE)
		{
			if (RefDieInfo.RefDie[ulIndex].InMap == 0) 
			{
				GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				continue;
			}

			m_stnWaferTable->GetRtnForSetJoystick(TRUE);
			strTemp.Format("Please move to (r%d,c%d)",RefDieInfo.RefDie[ulIndex].SrcMap.Row,RefDieInfo.RefDie[ulIndex].SrcMap.Col);
			lTmp = m_stnWaferTable->GetRtnForHmiMessageEx(strTemp, "Manual Align Wafer", glHMI_MBX_OKRETRYSTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			m_stnWaferTable->GetRtnForSetJoystick(FALSE);

			if ( lTmp == glHMI_OK )
			{
				// Align RefDie
				GetWTPosition(&lX,&lY,&lT);
               if (m_stnWaferTable->GetRtnForCheckWaferLimit(lX,lY) == FALSE)
               {
               	return FALSE;	//WT_ALN_OUT_WAFLIMIT;
               }
    
				sResult = SearchCurrDie(1, 1, &lX, &lY);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				if ( sResult == TRUE )
				{
					RefDieInfo.RefDie[ulIndex].InWaf = 1;
				}
				else
				{
					RefDieInfo.RefDie[ulIndex].InWaf = 2;
				}

				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
			}
			else if ( lTmp == glHMI_STOP )
			{
				return FALSE;
			}

	}
	}

	//Left
	i = RefDieInfo.MinRC.Row;
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InMap==1)
		{
			iLimitMin = j;
			break;
		}
	}
	for (j=RefDieInfo.MaxRC.Col; j>=RefDieInfo.MinRC.Col; j--)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InMap==1)
		{
			iLimitMax = j;
			break;
		}
	}

	j = CurrMap.Col;
	iJStep = RefDieInfo.RefDist.Col;
	while (j>iLimitMin)
	{
		j--;
		ulIndex = i*MaxRefWidth + j;
		sResult = GetNextRefDieXY(CurrPhy, 0, iJStep, &TgtPhy);
		if (sResult == TRUE)
		{
			RefDieInfo.RefDie[ulIndex].PhyPos.x = TgtPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = TgtPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			RefDieInfo.RefDie[ulIndex].InWaf = 1;

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;

			CurrPhy = TgtPhy;
			//continue;
		}
		else if (sResult == FALSE)
		{
			if (RefDieInfo.RefDie[ulIndex].InMap == 0) 
			{
				GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				continue;
			}

			m_stnWaferTable->GetRtnForSetJoystick(TRUE);
			strTemp.Format("Please move to (r%d,c%d)",RefDieInfo.RefDie[ulIndex].SrcMap.Row,RefDieInfo.RefDie[ulIndex].SrcMap.Col);
			lTmp = m_stnWaferTable->GetRtnForHmiMessageEx(strTemp, "Manual Align Wafer", glHMI_MBX_OKRETRYSTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			m_stnWaferTable->GetRtnForSetJoystick(FALSE);

			if ( lTmp == glHMI_OK )
			{
				// Align RefDie
				GetWTPosition(&lX,&lY,&lT);
				if (m_stnWaferTable->GetRtnForCheckWaferLimit(lX,lY) == FALSE)
				{
					return FALSE;	//WT_ALN_OUT_WAFLIMIT;
				}

				sResult = SearchCurrDie(1, 1, &lX, &lY);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				if ( sResult == TRUE )
				{
					RefDieInfo.RefDie[ulIndex].InWaf = 1;
				}
				else
				{
					RefDieInfo.RefDie[ulIndex].InWaf = 2;
				}

				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
			}
			else if ( lTmp == glHMI_STOP )
			{
				return FALSE;
			}
	}
	}

	ulIndex1 = RefDieInfo.MinRC.Row*MaxRefWidth + iLimitMin;
	ulIndex2 = RefDieInfo.MinRC.Row*MaxRefWidth + CurrMap.Col;
	if (iLimitMin != CurrMap.Col && RefDieInfo.RefDie[ulIndex1].InWaf == 1)
	{
		FDCBasic.HPitchFDC.x = (RefDieInfo.RefDie[ulIndex2].PhyPos.x - RefDieInfo.RefDie[ulIndex1].PhyPos.x)//*1.0
			/(CurrMap.Col-iLimitMin);
		FDCBasic.HPitchFDC.y = (RefDieInfo.RefDie[ulIndex2].PhyPos.y - RefDieInfo.RefDie[ulIndex1].PhyPos.y)//*1.0
			/(CurrMap.Col-iLimitMin);
	}
	else
	{
		FDCBasic.HPitchFDC.x = FDCBasic.HPitchDie.x * RefDieInfo.RefDist.Col;
		FDCBasic.HPitchFDC.y = FDCBasic.HPitchDie.y * RefDieInfo.RefDist.Col;
	}

	ulIndex1 = RefDieInfo.MinRC.Row*MaxRefWidth + CurrMap.Col;
	ulIndex2 = CurrMap.Row*MaxRefWidth + CurrMap.Col;
	if (RefDieInfo.MinRC.Row != CurrMap.Row && RefDieInfo.RefDie[ulIndex1].InWaf == 1)
	{
		FDCBasic.VPitchFDC.x = (RefDieInfo.RefDie[ulIndex2].PhyPos.x - RefDieInfo.RefDie[ulIndex1].PhyPos.x)//*1.0
			/(CurrMap.Row-RefDieInfo.MinRC.Row);
		FDCBasic.VPitchFDC.y = (RefDieInfo.RefDie[ulIndex2].PhyPos.y - RefDieInfo.RefDie[ulIndex1].PhyPos.y)//*1.0
			/(CurrMap.Row-RefDieInfo.MinRC.Row);
	}
	else
	{
		FDCBasic.VPitchFDC.x = FDCBasic.VPitchDie.x * RefDieInfo.RefDist.Row;
		FDCBasic.VPitchFDC.y = FDCBasic.VPitchDie.x * RefDieInfo.RefDist.Row;
	}

	//Begin Index
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].InMap == 0) continue;
		if (RefDieInfo.RefDie[ulIndex].InWaf != 0) continue;

		GetNearRefDie(RefDieInfo.RefDie[ulIndex].MapPos, &RefDieMap, &RefDiePhy);

		CurrPhy.x = RefDiePhy.x
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Col-RefDieMap.Col)*1.0/RefDieInfo.RefDist.Col*FDCBasic.HPitchFDC.x)
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Row-RefDieMap.Row)*1.0/RefDieInfo.RefDist.Row*FDCBasic.VPitchFDC.x);
		CurrPhy.y = RefDiePhy.y
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Col-RefDieMap.Col)*1.0/RefDieInfo.RefDist.Col*FDCBasic.HPitchFDC.y)
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Row-RefDieMap.Row)*1.0/RefDieInfo.RefDist.Row*FDCBasic.VPitchFDC.y);

		if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurrPhy.x,CurrPhy.y) == FALSE)
		{
			continue;
			//return FALSE;	//WT_ALN_OUT_WAFLIMIT;	//Klocwork	//v4.04
		}
		WftMoveXYTo(CurrPhy.x, CurrPhy.y);
		Sleep(200);

		// Align RefDie
		GetWTPosition(&lX,&lY,&lT);
		sResult = SearchCurrDie(1, 1, &lX, &lY);
		if ( sResult == TRUE )
		{
			CurrPhy.x = lX;
			CurrPhy.y = lY;
			RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			RefDieInfo.RefDie[ulIndex].InWaf = 1;

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
		}
		else
		{
			m_stnWaferTable->GetRtnForSetJoystick(TRUE);
			strTemp.Format("Please move to (r%d,c%d)",RefDieInfo.RefDie[ulIndex].SrcMap.Row,RefDieInfo.RefDie[ulIndex].SrcMap.Col);
			lTmp = m_stnWaferTable->GetRtnForHmiMessageEx(strTemp, "Manual Align Wafer", glHMI_MBX_OKRETRYSTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			m_stnWaferTable->GetRtnForSetJoystick(FALSE);

			if ( lTmp == glHMI_OK )
			{
				GetWTPosition(&lX,&lY,&lT);
				if (m_stnWaferTable->GetRtnForCheckWaferLimit(lX,lY) == FALSE)
				{
					return FALSE;	//WT_ALN_OUT_WAFLIMIT;
				}
				sResult = SearchCurrDie(1, 1, &lX, &lY);
				if ( sResult == TRUE )
				{
					CurrPhy.x = lX;
					CurrPhy.y = lY;
					RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
					RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
					RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
					RefDieInfo.RefDie[ulIndex].InWaf = 1;
				}
				else
				{
					RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
					RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
					RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
					RefDieInfo.RefDie[ulIndex].InWaf = 2;
				}

				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
			}
			else if ( lTmp == glHMI_STOP )
			{
				return FALSE;
			}
		}
	}

	if (Blk_bDebug == 1)
	{
		PrintFDC();
	}

	return TRUE;
}

LONG CWT_BlkFunc::GetNearRefDie(DieMapPos SrcDie, DieMapPos *RDieMap, DiePhyPos *RDiePhy)
{
	int			i,j;
	int			iRow,iCol;
	DieMapPos	sMinDistRC;
	double		dMinDist;
	double		dTmpDist;
	ULONG		ulIndex;

	dMinDist = sqrt(MaxRow*MaxRow*1.0 + MaxCol*MaxCol*1.0);
	sMinDistRC.Row = -1;
	sMinDistRC.Col = -1;
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].InWaf == 0) continue;
		iRow = RefDieInfo.RefDie[ulIndex].MapPos.Row;
		iCol = RefDieInfo.RefDie[ulIndex].MapPos.Col;
		dTmpDist = sqrt((SrcDie.Row-iRow)*(SrcDie.Row-iRow)*1.0 + (SrcDie.Col-iCol)*(SrcDie.Col-iCol)*1.0);
		if (dTmpDist < dMinDist)
		{
			dMinDist = dTmpDist;
			sMinDistRC.Row = i;
			sMinDistRC.Col = j;
		}
	}
	if ( sMinDistRC.Row == -1 || sMinDistRC.Col == -1 )
		return FALSE;

	ulIndex = sMinDistRC.Row*MaxRefWidth + sMinDistRC.Col;
	RDieMap->Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
	RDieMap->Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
	RDiePhy->x = RefDieInfo.RefDie[ulIndex].PhyPos.x; 
	RDiePhy->y = RefDieInfo.RefDie[ulIndex].PhyPos.y;

	return TRUE;
}

LONG CWT_BlkFunc::GetNextDiePhyPos(DieMapPos MapPos, DiePhyPos *PhyPos)
{
	DiePhyPos	HPitch, VPitch;
	LONG		lResult;
	LONG		lX, lY, lT;
	DOUBLE		dResolution = 1;
	DOUBLE		dThetaAngle;

	DieMapPos	RefDieMap, FDCPos;
	DiePhyPos	RefDiePhy;

	RefDieMap.Row = 1;
	RefDieMap.Col = 1;
	RefDiePhy.x = 0;
	RefDiePhy.y = 0;

	//Get Pitch
	GetNextDiePitch(&HPitch, &VPitch);
	FDCPos.Row = -1;
	FDCPos.Col = -1;
	lResult = GetNearDieMapPos(MapPos, &FDCPos);
	if (MnAlignReset == 1)
	{
//		FDCPos.Row = RefDieMap.Row;
//		FDCPos.Col = RefDieMap.Col;
	}

	if (lResult == FALSE)
	{
		return FALSE;
	}
	else
	{
		PhyPos->x = DiePhyPrep[FDCPos.Row][FDCPos.Col].x
						+ (MapPos.Col - FDCPos.Col)*HPitch.x
						+ (MapPos.Row - FDCPos.Row)*VPitch.x;
		PhyPos->y = DiePhyPrep[FDCPos.Row][FDCPos.Col].y
						+ (MapPos.Col - FDCPos.Col)*HPitch.y
						+ (MapPos.Row - FDCPos.Row)*VPitch.y;
	}

	//Convert X,Y,T
	GetWTPosition(&lX, &lY, &lT);
	lX = PhyPos->x;
	lY = PhyPos->y;
	m_stnWaferTable->GetThetaRes(&dResolution);
	dThetaAngle = -(DOUBLE)(lT - FDCBasic.HomeDie.PhyPos.t) * dResolution;
	m_stnWaferTable->GetRtnForRotateWaferTheta(&lX, &lY, &lT, dThetaAngle);
	PhyPos->x = lX;
	PhyPos->y = lY;
	PhyPos->t = lT;

	if (Blk_bDebug == 1)
	{
		FILE	*fp;
		char Buf1[80];
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr != 0) || (fp == NULL))		//Klocwork
		{
			return FALSE;
		}

		sprintf(Buf1, "Ref(%ld,%ld),Phy(%ld,%ld),HP(%ld,%ld),VP(%ld,%ld)---------\n",
				RefDieMap.Row,RefDieMap.Col,
				RefDiePhy.x,  RefDiePhy.y,
				HPitch.x, HPitch.y, VPitch.x, VPitch.y);
		strTemp.Format("%s",Buf1);
		fprintf(fp,"%s",Buf1);

		sprintf(Buf1, "FDCPos(%ld,%ld),Phy(%ld,%ld)\n",
				FDCPos.Row,FDCPos.Col,DiePhyPrep[FDCPos.Row][FDCPos.Col].x,DiePhyPrep[FDCPos.Row][FDCPos.Col].y);
		strTemp.Format("%s",Buf1);
		fprintf(fp,"%s",Buf1);

		sprintf(Buf1, "Map(%ld,%ld),Phy(%ld,%ld)\n",
				MapPos.Row,MapPos.Col,PhyPos->x,PhyPos->y);
		strTemp.Format("%s",Buf1);

		fprintf(fp,"%s",Buf1);
		fclose(fp);
	}

	return TRUE;
}

LONG CWT_BlkFunc::GetNextDiePitch(DiePhyPos *HoriPitch, DiePhyPos *VertPitch)
{
	//Source Pitch
	HoriPitch->x = FDCBasic.HPitchDie.x;
	HoriPitch->y = FDCBasic.HPitchDie.y;

	VertPitch->x = FDCBasic.VPitchDie.x;
	VertPitch->y = FDCBasic.VPitchDie.y;

	return TRUE;
}

BOOL CWT_BlkFunc::GetNearDieMapPos(DieMapPos SrcPos, DieMapPos *TgtPos)
{
	int			i,j;
	DieMapPos	sMinDistRC;
	double		dMinDist;
	double		dTmpDist;

	dMinDist = sqrt(MaxRow*MaxRow*1.0 + MaxCol*MaxCol*1.0);
	sMinDistRC.Row = -1;
	sMinDistRC.Col = -1;
	for (i=0; i<MaxRow; i++)
	for (j=0; j<MaxCol; j++)
	{
		if (DiePhyPrep[i][j].Status != Regn_Align &&
			DiePhyPrep[i][j].Status != Regn_HPick ) continue;
		dTmpDist = sqrt((SrcPos.Row-i)*(SrcPos.Row-i)*1.0 + (SrcPos.Col-j)*(SrcPos.Col-j)*1.0);
		if (dTmpDist < dMinDist)
		{
			dMinDist = dTmpDist;
			sMinDistRC.Row = i;
			sMinDistRC.Col = j;
		}
	}
	if ( sMinDistRC.Row == -1 || sMinDistRC.Col == -1 )
		return FALSE;

	TgtPos->Row = sMinDistRC.Row;
	TgtPos->Col = sMinDistRC.Col;

	return TRUE;
}

BOOL CWT_BlkFunc::SetCurDiePhyPos(DieMapPos MapPos, DiePhyPos PhyPos, BOOL bUpdMap)
{
	LONG	lX,lY,lT;
	DOUBLE	dThetaAngle = 0;
	DOUBLE	dResolution = 1;

	lX = lY = lT = 0;
	DiePhyPrep[MapPos.Row][MapPos.Col].x = PhyPos.x;
	DiePhyPrep[MapPos.Row][MapPos.Col].y = PhyPos.y;
	DiePhyPrep[MapPos.Row][MapPos.Col].t = PhyPos.t;

	//Convert X,Y,T to home angle.
	lX = PhyPos.x;
	lY = PhyPos.y;
	m_stnWaferTable->GetThetaRes(&dResolution);
	dThetaAngle = -(DOUBLE)(FDCBasic.HomeDie.PhyPos.t - PhyPos.t) * dResolution;
	m_stnWaferTable->GetRtnForRotateWaferTheta(&lX, &lY, &lT, dThetaAngle);
	DiePhyPrep[MapPos.Row][MapPos.Col].x = lX;
	DiePhyPrep[MapPos.Row][MapPos.Col].y = lY;
	DiePhyPrep[MapPos.Row][MapPos.Col].t = lT;

	//Update
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", MapPos.Row);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", MapPos.Col);
	if ( MapPos.Status == WAF_CDieSelectionAlgorithm::PICK )
	{
		DiePhyPrep[MapPos.Row][MapPos.Col].Status = Regn_HPick;
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_HPick);
		m_stnWaferTable->UpdateMapDie(MapPos.Row, MapPos.Col, 1, WAF_CDieSelectionAlgorithm::ALIGN, WT_MAP_DIESTATE_PICK);
	}
	if ( MapPos.Status == WAF_CDieSelectionAlgorithm::ALIGN )
	{
		DiePhyPrep[MapPos.Row][MapPos.Col].Status = Regn_Align;
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);
		if (bUpdMap)
			m_stnWaferTable->UpdateMapDie(MapPos.Row, MapPos.Col, 1, WAF_CDieSelectionAlgorithm::ALIGN, WT_MAP_DIESTATE_DEFECT);
	}
	if ( MapPos.Status == WAF_CDieSelectionAlgorithm::DEFECTIVE)
	{
		DiePhyPrep[MapPos.Row][MapPos.Col].Status = Regn_Align;
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Inked);
		m_stnWaferTable->UpdateMapDie(MapPos.Row, MapPos.Col, 1, WAF_CDieSelectionAlgorithm::ALIGN, WT_MAP_DIESTATE_DEFECT);
	}
	if ( MapPos.Status == WAF_CDieSelectionAlgorithm::MISSING)
	{
		DiePhyPrep[MapPos.Row][MapPos.Col].Status = Regn_Empty;
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Empty);
		m_stnWaferTable->UpdateMapDie(MapPos.Row, MapPos.Col, 1, WAF_CDieSelectionAlgorithm::MISSING, WT_MAP_DIESTATE_EMPTY);
	}


	if (Blk_bDebug)
	{
		pFile = NULL;
		errno_t nErr = fopen_s(&pFile, "FDCRegn.log","a+");
		strTemp.Format("SetPhyPos (%d,%d),Src(%d,%d,%d),Tgt(%d,%d,%d),Act(%d)\n",
			MapPos.Row,MapPos.Col,PhyPos.x,PhyPos.y,PhyPos.t,lX,lY,lT,MapPos.Status);
		if ((nErr == 0) && (pFile != NULL))		//Klocwork
		{
			fprintf(pFile,"%s", (LPCTSTR) strTemp);
			fclose(pFile);
		}
	}

	//Reset update value
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);

	return TRUE;
}

BOOL CWT_BlkFunc::GetStartDie(LONG *lRow, LONG *lCol, LONG *lX, LONG *lY)
{
	*lRow = FDCBasic.HomeDie.MapPos.Row;
	*lCol = FDCBasic.HomeDie.MapPos.Col;
	*lX = FDCBasic.HomeDie.PhyPos.x;
	*lY = FDCBasic.HomeDie.PhyPos.y;

	return TRUE;
}

BOOL CWT_BlkFunc::SetStartDie(DiePhyPos HomePhy)
{
	int			i,j;
	LONG		lPosX,lPosY;
	DieMapPos	sMinDistRC;
	double		dMinDist;
	double		dTmpDist;
	ULONG		ulIndex;

	dMinDist = 10000;
	sMinDistRC.Row = -1;
	sMinDistRC.Col = -1;
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].InMap == 0) continue;
		if (RefDieInfo.RefDie[ulIndex].InWaf == 0) continue;
		lPosX = RefDieInfo.RefDie[ulIndex].PhyPos.x;
		lPosY = RefDieInfo.RefDie[ulIndex].PhyPos.y;
		DOUBLE dDiffX = (HomePhy.x-lPosX);
		DOUBLE dDiffY = (HomePhy.y-lPosY);
		dTmpDist = sqrt(dDiffX*dDiffX*1.0 + dDiffY*dDiffY*1.0);
		if (dTmpDist < dMinDist)
		{
			dMinDist = dTmpDist;
			sMinDistRC.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
			sMinDistRC.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
		}
	}
	if ( sMinDistRC.Row == -1 || sMinDistRC.Col == -1 )
		return FALSE;

	FDCBasic.HomeDie.MapPos.Row = sMinDistRC.Row;
	FDCBasic.HomeDie.MapPos.Col = sMinDistRC.Col;
	FDCBasic.HomeDie.PhyPos.x = HomePhy.x;
	FDCBasic.HomeDie.PhyPos.y = HomePhy.y;
	FDCBasic.HomeDie.PhyPos.t = HomePhy.t;

	if (Blk_bDebug)
	{
		pFile = NULL;
		errno_t nErr = fopen_s(&pFile, "FDCRegn.log","a+");
		strTemp.Format("SetStartDie (%d,%d),(%d,%d)\n",sMinDistRC.Row,sMinDistRC.Col,HomePhy.x,HomePhy.y);
		if ((nErr == 0) && (pFile != NULL))		//Klocwork
		{
			fprintf(pFile,"%s", (LPCTSTR) strTemp);
			fclose(pFile);
		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc::GetRefDieArray()
{
	LONG	i,j,k,lTmp;
	LONG	iRow, iCol;
	LONG	BaseRow, BaseCol, DistRow, DistCol;
	ULONG	ulNumRefDie, ulIndex;
	WAF_CMapDieInformation *arrRef;
	DieMapPos	*arrRefSort;
	UCHAR	ucGrade;

	ulNumRefDie = m_stnWaferTable->m_WaferMapWrapper.GetNumberOfReferenceDice();
	arrRef = new WAF_CMapDieInformation [ulNumRefDie];
	arrRefSort = new DieMapPos [ulNumRefDie];

	RefDieInfo.sNum = (LONG)ulNumRefDie;
	if (Blk_bDebug)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		strTemp.Format("s Num(%d)\n",ulNumRefDie);
		//AfxMessageBox(strTemp);
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"%s", (LPCTSTR) strTemp);
			fclose(fp);
		}
	}

	m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieList(arrRef, ulNumRefDie);
	BaseRow = BaseCol = MaxRow;
	for (i=0; i<(LONG)ulNumRefDie; i++)
	{
		if (arrRef[i].GetRow() < (ULONG)BaseRow)
			BaseRow = arrRef[i].GetRow();
		if (arrRef[i].GetColumn() < (ULONG)BaseCol)
			BaseCol = arrRef[i].GetColumn();

		arrRefSort[i].Row = (LONG)arrRef[i].GetRow();
		arrRefSort[i].Col = (LONG)arrRef[i].GetColumn();
	}

	// Sorting
	for (i=0; i<(LONG)ulNumRefDie-1; i++)
	for (j=i+1; j<(LONG)ulNumRefDie; j++)
	{
		if (arrRefSort[i].Row > arrRefSort[j].Row)
		{
			lTmp = arrRefSort[i].Row;
			arrRefSort[i].Row = arrRefSort[j].Row;
			arrRefSort[j].Row = lTmp;
		}

		if (arrRefSort[i].Col > arrRefSort[j].Col)
		{
			lTmp = arrRefSort[i].Col;
			arrRefSort[i].Col = arrRefSort[j].Col;
			arrRefSort[j].Col = lTmp;
		}
	}

	DistRow = MaxRow;
	DistCol = MaxCol;
	for (i=0; i<(LONG)ulNumRefDie-1; i++)
	{
		lTmp = arrRefSort[i+1].Row - arrRefSort[i].Row;
		if (lTmp != 0 && lTmp < DistRow)
		{
			DistRow = lTmp;
		}
		lTmp = arrRefSort[i+1].Col - arrRefSort[i].Col;
		if (lTmp != 0 && lTmp < DistCol)
		{
			DistCol = lTmp;
		}
	}

	RefDieInfo.RefDist.Row = DistRow;
	RefDieInfo.RefDist.Col = DistCol;
	if (Blk_bDebug)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		strTemp.Format("Dist RC(%d,%d)\n",DistRow,DistCol);
		//AfxMessageBox(strTemp);
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"%s", (LPCTSTR) strTemp);
			fclose(fp);
		}
	}

	LONG RefDieMaxRow, RefDieMaxCol;
	LONG lUserRow, lUserCol, lAngle;
	RefDieMaxRow = RefDieMaxCol = 0;
	lAngle = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetConfiguration().GetOrientedAngle();
	for (i=0; i<MaxRefWidth; i++)
	{
		if (i==0) iRow = 0;
		else if (i==1) iRow = BaseRow;
		else iRow += DistRow;

		for (j=0; j<MaxRefWidth; j++)
		{
			ulIndex = i*MaxRefWidth + j;
			RefDieInfo.RefDie[ulIndex].MapPos.Row = iRow;
			if (j==0) iCol = 0;
			else if (j==1) iCol = BaseCol;
			else iCol += DistCol;
			RefDieInfo.RefDie[ulIndex].MapPos.Col = iCol;

			ucGrade = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetDieInformation(iRow,iCol);
			if ( ucGrade == WAF_CMapConfiguration::DEFAULT_NULL_BIN ) continue;

			k = 0;
			while ( k<(LONG)ulNumRefDie)
			{
				if (arrRef[k].GetRow() == iRow && arrRef[k].GetColumn() == iCol)
				{
					RefDieInfo.RefDie[ulIndex].InMap = 1;
					if (i>RefDieMaxRow) RefDieMaxRow = i;
					if (j>RefDieMaxCol) RefDieMaxCol = j;
					m_stnWaferTable->ConvertAsmToOrgUser(iRow,iCol,lUserRow,lUserCol);
					RefDieInfo.RefDie[ulIndex].SrcMap.Row = lUserRow;
					RefDieInfo.RefDie[ulIndex].SrcMap.Col = lUserCol;
					break;
				}
				k++;
			}
		}
	}

	delete[] arrRef;
	delete[] arrRefSort;

	RefDieInfo.MaxRC.Row = RefDieMaxRow;
	RefDieInfo.MaxRC.Col = RefDieMaxCol;
	RefDieInfo.MinRC.Row = 1;
	RefDieInfo.MinRC.Col = 1;
	if (Blk_bDebug == TRUE)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"MinRC(%ld,%ld), MaxRC(%ld,%ld)\n",
						RefDieInfo.MinRC.Row,
						RefDieInfo.MinRC.Col,
						RefDieInfo.MaxRC.Row,
						RefDieInfo.MaxRC.Col);
			fclose(fp);
		}
	}

	if (Blk_bDebug == TRUE)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");

		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			for (i=0; i<MaxRefWidth; i++)
			for (j=0; j<MaxRefWidth; j++)
			{
				ulIndex = i*MaxRefWidth + j;
				if (RefDieInfo.RefDie[ulIndex].InMap == 1)
				{
					fprintf(fp,"no.%lu, Src(%ld,%ld), Map(%ld,%ld)\n",ulIndex,
						RefDieInfo.RefDie[ulIndex].SrcMap.Row,
						RefDieInfo.RefDie[ulIndex].SrcMap.Col,
						RefDieInfo.RefDie[ulIndex].MapPos.Row,
						RefDieInfo.RefDie[ulIndex].MapPos.Col);
				}
			}

			fclose(fp);
		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc::SetRefDieArray(LONG lDistRow, LONG lDistCol)
{
	LONG	i,j;
	ULONG	iRow, iCol;
	LONG	BaseRow, BaseCol;
	ULONG	ulIndex;
	UCHAR	ucGrade;

	ULONG ulNumRow = 1000;
	ULONG ulNumCol = 1000;
	m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

	BaseRow = lDistRow < MaxRefWidth ? lDistRow-1 : MaxRefWidth-1;
	BaseCol = lDistCol < MaxRefWidth ? lDistCol-1 : MaxRefWidth-1;
	
	RefDieInfo.RefDist.Row = BaseRow+1;
	RefDieInfo.RefDist.Col = BaseCol+1;

	LONG RefDieMaxRow, RefDieMaxCol;
	LONG lUserRow = 0, lUserCol = 0;
	RefDieMaxRow = RefDieMaxCol = 0;
	for (i=0; i<MaxRefWidth; i++)
	{
		if (i==0) iRow = 0;
		else if (i==1) iRow = BaseRow;
		else iRow += lDistRow;

		for (j=0; j<MaxRefWidth; j++)
		{
			ulIndex = i*MaxRefWidth + j;
			RefDieInfo.RefDie[ulIndex].MapPos.Row = iRow;
			if (j==0) iCol = 0;
			else if (j==1) iCol = BaseCol;
			else iCol += lDistCol;
			RefDieInfo.RefDie[ulIndex].MapPos.Col = iCol;

			if (iRow >= ulNumRow || iCol >= ulNumCol) continue;

			ucGrade = m_stnWaferTable->m_WaferMapWrapper.GetReader()->GetDieInformation(iRow,iCol);
			if ( ucGrade == WAF_CMapConfiguration::DEFAULT_NULL_BIN ) continue;
			if ( m_stnWaferTable->m_WaferMapWrapper.GetReader()->IsMissingDie(iRow,iCol) ) continue;
			if ( m_stnWaferTable->m_WaferMapWrapper.GetReader()->IsPickedDie(iRow,iCol) ) continue;

			RefDieInfo.RefDie[ulIndex].InMap = 1;
			if (i>RefDieMaxRow) RefDieMaxRow = i;
			if (j>RefDieMaxCol) RefDieMaxCol = j;
			m_stnWaferTable->ConvertAsmToOrgUser(iRow,iCol,lUserRow,lUserCol);
			RefDieInfo.RefDie[ulIndex].SrcMap.Row = lUserRow;
			RefDieInfo.RefDie[ulIndex].SrcMap.Col = lUserCol;
		}
	}

	RefDieInfo.MaxRC.Row = RefDieMaxRow;
	RefDieInfo.MaxRC.Col = RefDieMaxCol;
	RefDieInfo.MinRC.Row = 1;
	RefDieInfo.MinRC.Col = 1;

	m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
	DrawBlockMap();

	return TRUE;
}

LONG CWT_BlkFunc::CheckDieInSpecialGrade(LONG iGrade)
{
	ULONG		i,j;
	DieMapPos	CurrDie, RefDieMap;
	DiePhyPos	CurrTblPos, RefDiePhy;
	ULONG	ulCount;

	ULONG	ulNumOfRow=0, ulNumOfCol=0;
	LONG	lPosX, lPosY;
	LONG	lResult;
	BOOL	bRtn;
	
	if (!m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulNumOfRow, ulNumOfCol))
	{
		return TRUE;
	}

	CString strFileNmWaf, strFileNmBin, strFileNmLog;
	CStdioFile	pFileBin, pFileLog;
	BOOL		bOutput;

	//Return If 0
	if ( iGrade == 0 ) return TRUE;

	ulCount = 0;
	EmptyCheck.ulNumOfDie = 0;
	bRtn = TRUE;
	for(i=0;i<ulNumOfRow;i++)
	{
		for(j=0;j<ulNumOfCol;j++)
		{
			if( m_stnWaferTable->m_WaferMapWrapper.GetGrade((ULONG)i,(ULONG)j) != iGrade + m_stnWaferTable->m_WaferMapWrapper.GetGradeOffset()) continue;

			// Index
			CurrDie.Row = i;
			CurrDie.Col = j;
			lResult = GetNearRefDie(CurrDie, &RefDieMap, &RefDiePhy);
			if ( lResult == FALSE) continue;
			WftMoveXYTo(RefDiePhy.x, RefDiePhy.y);
			Sleep(200);

			CurrTblPos = RefDiePhy;
			lResult = IndexToNormalDie(RefDieMap, CurrDie, &CurrTblPos, 0);
			if ( lResult == FALSE) continue;
			lPosX = CurrTblPos.x;
			lPosY = CurrTblPos.y;
			lResult = GetNormalDieStatus(&lPosX, &lPosY);

			// Save error die
			if (lResult == TRUE)
			{
				LONG lTmp;

				EmptyCheck.ulNumOfDie++;
				EmptyCheck.MapPos[ulCount].Row = CurrDie.Row;
				EmptyCheck.MapPos[ulCount].Col = CurrDie.Col;
				ulCount++;

				strTemp.Format("Error! Found normal die!");
				lTmp = m_stnWaferTable->HmiMessage_Red_Green(strTemp, "Empty Die Check", glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				if ( lTmp == glHMI_STOP)
				{
					return FALSE;
				}
				Sleep(500);
				
				bRtn = FALSE;
			}
			else
			{
				strTemp.Format("OK! No die here!");
				//m_stnWaferTable->GetRtnForHmiMessage(strTemp, "Empty Die Check", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 1000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
		}
		//if (ulCount>20) break;
	}

	if ( bRtn == FALSE )
	{
		//Get Output Path
		bOutput = GetIniOutputFileNm(&strFileNmWaf,&strFileNmBin,&strFileNmLog);
		//if ( !bOutput ) return TRUE;		//Klocwork	//v4.04

		//Write Sum file
		bOutput = pFileBin.Open(strFileNmBin, CFile::modeCreate|CFile::modeWrite);
		if ( !bOutput ) return TRUE;
		strTemp.Format("%d",EmptyCheck.ulNumOfDie);
		pFileBin.WriteString(strTemp);
		pFileBin.Close();

		//Write SDS file
		bOutput = pFileLog.Open(strFileNmLog, CFile::modeCreate|CFile::modeWrite);
		if ( !bOutput ) return TRUE;
		pFileLog.SeekToEnd();
		//Write Header
		{
			//Initial File Name
			pFileLog.WriteString("Initial File Name, \n");

			//Bin Block Unload Date
			pFileLog.WriteString("Bin Block Unload Date, \n");

			//Bin Block Unload Time
			pFileLog.WriteString("Bin Block Unload Time, \n");

			//Bin Block
			strTemp.Format("Bin Block, %d\n", iGrade);
			pFileLog.WriteString(strTemp);

			//Sorter
			strTemp.Format("Sorter, %s\n", m_strMachineID);
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
			pFileLog.WriteString("Begin "+strTemp+"\n");

		}

		//Write Data
		for (i=0; i<EmptyCheck.ulNumOfDie; i++)
		{
			LONG lUserRow, lUserCol;
			m_stnWaferTable->ConvertAsmToOrgUser(EmptyCheck.MapPos[i].Row,EmptyCheck.MapPos[i].Col,lUserRow,lUserCol);

			strTemp.Format("%3d,%3d,%3d,%3d\n",
				lUserRow, lUserCol,
				0, 0);
			pFileLog.WriteString(strTemp);
		}
		pFileLog.Close();
	}

	return TRUE;
}

BOOL CWT_BlkFunc::PrintFDC()
{
	int		i,j;
	unsigned long ulIndex;
	FILE	*fp = NULL;

	errno_t nErr = fopen_s(&fp, "c:\\Mapsorter\\UserData\\History\\FDCBlock.log", "w+");		//v3.31
	if ((nErr != 0) || (fp == NULL))		//Klocwork
	{
		return FALSE;
	}

	fprintf(fp,"s Num,%ld\n",RefDieInfo.sNum);
	fprintf(fp,"Referenc Die\n");
	fprintf(fp,"No.,SrcMap,AsmMap,PhyPos,0,InMap,InWaf\n");
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		fprintf(fp,"%lu,(%ld;%ld),(%ld;%ld),(%ld;%ld),%d,%d,%d\n",
				ulIndex,
				RefDieInfo.RefDie[ulIndex].SrcMap.Row,	RefDieInfo.RefDie[ulIndex].SrcMap.Col,
				RefDieInfo.RefDie[ulIndex].MapPos.Row,  RefDieInfo.RefDie[ulIndex].MapPos.Col,
				RefDieInfo.RefDie[ulIndex].PhyPos.x,	RefDieInfo.RefDie[ulIndex].PhyPos.y,
				0,
				RefDieInfo.RefDie[ulIndex].InMap,
				RefDieInfo.RefDie[ulIndex].InWaf );
	}
	fprintf(fp,"RefDist(%ld,%ld)\n",RefDieInfo.RefDist.Row, RefDieInfo.RefDist.Col);
	fprintf(fp,"HPitch(%ld,%ld), VPitch(%ld,%ld)\n",
		FDCBasic.HPitchDie.x,FDCBasic.HPitchDie.y,FDCBasic.VPitchDie.x,FDCBasic.VPitchDie.y);
	fclose(fp);

	return TRUE;
}

BOOL CWT_BlkFunc::SetBlockParameter(LONG RefGrid,LONG HomeGrid,LONG JumpCtr,LONG JumpEdge,LONG NoDieGrade,LONG AlignReset, LONG lMinForPick)
{
	MnSrchRefGrid = RefGrid;
	MnMaxJumpCtr = JumpCtr;
	MnMaxJumpEdge = JumpEdge>0?JumpEdge:1;
	MnNoDieGrade = NoDieGrade;
	MnAlignReset = AlignReset;

	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", MnMaxJumpCtr);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", MnMaxJumpEdge);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Init);
	m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);

	//Reset update value
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", 0);
	m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);

	return TRUE;
}

LONG CWT_BlkFunc::IndexToNormalDie(DieMapPos SrcDie, DieMapPos TgtDie, DiePhyPos *CurPhyPos, INT iMissCount)
{
	DieMapPos	CurDie,CurBkDie;
	DiePhyPos	CurPhy;
	LONG		lPosX, lPosY, lPosT;
	LONG		lDirRow, lDirCol;
	LONG		lStep = 1;
	LONG		lResult;
	INT			iStop = 0;
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

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

	lStep = 1;

	LONG lPosArX, lPosArY, lDir;
	BOOL bArResult[8];
	for (lDir=0;lDir<8;lDir++)
	{
		bArResult[lDir] = SearchAroundDie(lDir,&lPosArX, &lPosArY);
	}

	// Move
	while ( CurDie.Row != TgtDie.Row || CurDie.Col != TgtDie.Col )
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		CurBkDie = CurDie;

 		if ( CurDie.Row != TgtDie.Row )
		{
			CurDie.Row = CurDie.Row + lStep*lDirRow;
			if ( lDirRow == 1 && CurDie.Row > TgtDie.Row )
				CurDie.Row = TgtDie.Row;
			if ( lDirRow == -1 && CurDie.Row < TgtDie.Row )
				CurDie.Row = TgtDie.Row;
		}
 		if ( CurDie.Col != TgtDie.Col )
		{
			CurDie.Col = CurDie.Col + lStep*lDirCol;
			if ( lDirCol == 1 && CurDie.Col > TgtDie.Col )
				CurDie.Col = TgtDie.Col;
			if ( lDirCol == -1 && CurDie.Col < TgtDie.Col )
				CurDie.Col = TgtDie.Col;
		}

		if (   ( m_stnWaferTable->m_WaferMapWrapper.GetGrade(CurDie.Row,CurDie.Col) == 46 )
			&& ( CurDie.Row != TgtDie.Row || CurDie.Col != TgtDie.Col )	)
		{
			//continue;
		}


		CurPhy.x += (CurDie.Row - CurBkDie.Row)*(FDCBasic.VPitchDie.x)+(CurDie.Col - CurBkDie.Col)*(FDCBasic.HPitchDie.x);
		CurPhy.y += (CurDie.Row - CurBkDie.Row)*(FDCBasic.VPitchDie.y)+(CurDie.Col - CurBkDie.Col)*(FDCBasic.HPitchDie.y);

		if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
		{
			return FALSE; //WT_ALN_OUT_WAFLIMIT;
		}
		WftMoveXYTo(CurPhy.x, CurPhy.y);
		Sleep(80);

		lPosX = CurPhy.x;
		lPosY = CurPhy.y;
		m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(CurDie.Row,CurDie.Col);
		lResult = GetNormalDieStatus(&lPosX, &lPosY);

		GetWTPosition(&lPosX, &lPosY, &lPosT);
		if (lResult == TRUE)
		{
			CurPhy.x = lPosX;
			CurPhy.y = lPosY;
			CurPhy.t = FDCBasic.HomeDie.PhyPos.t;
			CurDie.Status = WAF_CDieSelectionAlgorithm::ALIGN;
			iStop = 0;
		}
		else
		{
			CurPhy.x = lPosX;
			CurPhy.y = lPosY;
			m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(CurDie.Row,CurDie.Col);
			Sleep(200);
			iStop++;
			if (iStop == iMissCount && iMissCount != 0) return FALSE;
		}

		CurPhyPos->x = CurPhy.x;
		CurPhyPos->y = CurPhy.y;
	}

	return TRUE;
}

LONG CWT_BlkFunc::IndexToCheckKeyDie(DieMapPos SrcDie, DieMapPos TgtDie, DiePhyPos *CurPhyPos, INT iMissCount)
{
	DieMapPos	CurDie,CurBkDie,ArDie;
	DiePhyPos	CurPhy;//ArDiePhy;
	LONG		lPosX, lPosY, lPosT;
	LONG		lStep = 1;
	LONG		lResult;
	INT			iStop = 0;
	LONG		lTgtDir;
	ULONG		ulCounter = 0;

	// Init Status
	CurDie = SrcDie;
	CurPhy.x = CurPhyPos->x;
	CurPhy.y = CurPhyPos->y;

	LONG lPosArX, lPosArY, lDir;
	BOOL bArResult[8];
	ULONG ulNumRow = 0, ulNumCol = 0, ulLaRow, ulLaCol;
	m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);
	for (lDir=0;lDir<8;lDir++)
	{
		ulLaRow = ulLaCol = 1000;
		switch (lDir)
		{
		case 0:
			if (CurDie.Col == 0) break;
			ulLaRow = CurDie.Row;
			ulLaCol = CurDie.Col-1;
			break;
		case 1:
			if (CurDie.Col == ulNumCol-1) break;
			ulLaRow = CurDie.Row;
			ulLaCol = CurDie.Col+1;
			break;
		case 2:
			if (CurDie.Row == 0) break;
			ulLaRow = CurDie.Row-1;
			ulLaCol = CurDie.Col;
			break;
		case 3:
			if (CurDie.Row == ulNumRow-1) break;
			ulLaRow = CurDie.Row+1;
			ulLaCol = CurDie.Col;
			break;
		case 4:
			if (CurDie.Row == 0 || CurDie.Col == 0) break;
			ulLaRow = CurDie.Row-1;
			ulLaCol = CurDie.Col-1;
			break;
		case 5:
			if (CurDie.Row == ulNumRow-1 || CurDie.Col == 0) break;
			ulLaRow = CurDie.Row+1;
			ulLaCol = CurDie.Col-1;
			break;
		case 6:
			if (CurDie.Row == 0 || CurDie.Col == ulNumCol-1) break;
			ulLaRow = CurDie.Row-1;
			ulLaCol = CurDie.Col+1;
			break;
		case 7:
			if (CurDie.Row == ulNumRow-1 || CurDie.Col == ulNumCol-1) break;
			ulLaRow = CurDie.Row+1;
			ulLaCol = CurDie.Col+1;
			break;
		default:
			;
		}
		if (ulLaRow==1000 || ulLaCol==1000) continue;
		ArDie.Row = ulLaRow;
		ArDie.Col = ulLaCol;
		bArResult[lDir] = SearchAroundDie(lDir,&lPosArX, &lPosArY);
		if (bArResult[lDir] == TRUE)
		{
			DieArPhy[ArDie.Row][ArDie.Col] = 1;
		}
	}

	// Move
	while ( CurDie.Row != TgtDie.Row || CurDie.Col != TgtDie.Col )
	{
		CurBkDie = CurDie;
		ulCounter++;

		//v3.30T1
		if (CMS896AStn::m_bStopGoToBlkDie == TRUE)
		{
//AfxMessageBox("Operation is aborted.", MB_SYSTEMMODAL);
			return FALSE;
		}

 		if ( CurDie.Row == TgtDie.Row )
		{
			if ( CurDie.Col == TgtDie.Col )
				lTgtDir = 0;
			else if ( CurDie.Col > TgtDie.Col ) 
				lTgtDir = 1;
			else
				lTgtDir = 2;
		}
		else if ( CurDie.Row > TgtDie.Row )
		{
	 		if ( CurDie.Col == TgtDie.Col ) 
				lTgtDir = 3;
			else if ( CurDie.Col > TgtDie.Col )
				lTgtDir = 5;
			else
				lTgtDir = 7;
		}
		else
		{
	 		if ( CurDie.Col == TgtDie.Col ) 
				lTgtDir = 4;
			else if ( CurDie.Col > TgtDie.Col )
				lTgtDir = 6;
			else
				lTgtDir = 8;
		}

		BOOL bGetNxt;
		bGetNxt = GetNextCheckKeyDie(lTgtDir, CurBkDie, &CurDie);
		if (bGetNxt)
		{
			CurPhy.x += (CurDie.Row - CurBkDie.Row)*(FDCBasic.VPitchDie.x)+(CurDie.Col - CurBkDie.Col)*(FDCBasic.HPitchDie.x);
			CurPhy.y += (CurDie.Row - CurBkDie.Row)*(FDCBasic.VPitchDie.y)+(CurDie.Col - CurBkDie.Col)*(FDCBasic.HPitchDie.y);

			if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
			{
AfxMessageBox("Hit wafer limit!", MB_SYSTEMMODAL);	//andrew3
				return FALSE;
			}
			WftMoveXYTo(CurPhy.x, CurPhy.y);
			Sleep(50);

			lPosX = CurPhy.x;
			lPosY = CurPhy.y;
			m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(CurDie.Row,CurDie.Col);
			lResult = GetNormalDieStatus(&lPosX, &lPosY);

			for (lDir=0;lDir<8;lDir++)
			{
				bArResult[lDir] = SearchAroundDie(lDir,&lPosArX, &lPosArY);
				ulLaRow = ulLaCol = 1000;
				switch (lDir)
				{
				case 0:
					if (CurDie.Col == 0) break;
					ulLaRow = CurDie.Row;
					ulLaCol = CurDie.Col-1;
					break;
				case 1:
					if (CurDie.Col == ulNumCol-1) break;
					ulLaRow = CurDie.Row;
					ulLaCol = CurDie.Col+1;
					break;
				case 2:
					if (CurDie.Row == 0) break;
					ulLaRow = CurDie.Row-1;
					ulLaCol = CurDie.Col;
					break;
				case 3:
					if (CurDie.Row == ulNumRow-1) break;
					ulLaRow = CurDie.Row+1;
					ulLaCol = CurDie.Col;
					break;
				case 4:
					if (CurDie.Row == 0 || CurDie.Col == 0) break;
					ulLaRow = CurDie.Row-1;
					ulLaCol = CurDie.Col-1;
					break;
				case 5:
					if (CurDie.Row == ulNumRow-1 || CurDie.Col == 0) break;
					ulLaRow = CurDie.Row+1;
					ulLaCol = CurDie.Col-1;
					break;
				case 6:
					if (CurDie.Row == 0 || CurDie.Col == ulNumCol-1) break;
					ulLaRow = CurDie.Row-1;
					ulLaCol = CurDie.Col+1;
					break;
				case 7:
					if (CurDie.Row == ulNumRow-1 || CurDie.Col == ulNumCol-1) break;
					ulLaRow = CurDie.Row+1;
					ulLaCol = CurDie.Col+1;
					break;
				}
				if (ulLaRow==1000 || ulLaCol==1000)
				{
					continue;
				}
				ArDie.Row = ulLaRow;
				ArDie.Col = ulLaCol;
				if ( bArResult[lDir] == TRUE )
				{
					if (DieArPhy[ArDie.Row][ArDie.Col] == 0)
						DieArPhy[ArDie.Row][ArDie.Col] = 1;
				}
			}
		}
		else
		{
			return FALSE;
		}

		if (ulCounter > ((ULONG)abs(SrcDie.Row-TgtDie.Row)+(ULONG)abs(SrcDie.Col-TgtDie.Col))*2.5) return FALSE;

		GetWTPosition(&lPosX, &lPosY, &lPosT);
		if (lResult == TRUE)
		{
			CurPhy.x = lPosX;
			CurPhy.y = lPosY;
			CurPhy.t = FDCBasic.HomeDie.PhyPos.t;
			CurDie.Status = WAF_CDieSelectionAlgorithm::ALIGN;
			SetCurDiePhyPos(CurDie, CurPhy);
			iStop = 0;
		}
		else
		{
			CurPhy.x = lPosX;
			CurPhy.y = lPosY;
			m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(CurDie.Row,CurDie.Col);
			Sleep(20);
			iStop++;
			if (iStop == iMissCount && iMissCount != 0) 
			{
				return FALSE;
			}
		}

		CurPhyPos->x = CurPhy.x;
		CurPhyPos->y = CurPhy.y;
	}

	return TRUE;
}

BOOL CWT_BlkFunc::GetNextCheckKeyDie(LONG lTgtDir, DieMapPos SrcDie, DieMapPos *TgtDie)
{
	ULONG ulNumRow = 0, ulNumCol = 0;
	m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

	switch (lTgtDir)
	{
	case 1:
		if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1)//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		break;
	case 2:
		if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1 )//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		break;
	case 3:
		if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1 )//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		break;
	case 4:
		if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1 )//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		break;
	case 5:
		if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1 )//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		break;
	case 6:
		if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1 )//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		break;
	case 7:
		if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1 )//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		break;
	case 8:
		if (DieArPhy[SrcDie.Row+1][SrcDie.Col+1] == 1)//DR
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col+1] == 1)//R
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col] == 1)//D
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row+1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col+1] == 1)//UR
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col + 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col+1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row+1][SrcDie.Col-1] == 1)//DL
		{
			TgtDie->Row = SrcDie.Row + 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row+1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col] == 1)//U
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col;
			DieArPhy[SrcDie.Row-1][SrcDie.Col] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row][SrcDie.Col-1] == 1)//L
		{
			TgtDie->Row = SrcDie.Row;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row][SrcDie.Col-1] = 2;
			return TRUE;
		}
		else if (DieArPhy[SrcDie.Row-1][SrcDie.Col-1] == 1 )//UL
		{
			TgtDie->Row = SrcDie.Row - 1;
			TgtDie->Col = SrcDie.Col - 1;
			DieArPhy[SrcDie.Row-1][SrcDie.Col-1] = 2;
			return TRUE;
		}
		break;
	default:
		;
	}

	return FALSE;
}


BOOL CWT_BlkFunc::IsCurrBlkNeedUpdate(DieMapPos MapPos)
{
	ULONG ulIndex = 0;
	int i, j;
	BOOL bRowMeet = FALSE;

	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	{
		for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
		{
			ulIndex = i*MaxRefWidth + j;

			if ( (RefDieInfo.RefDie[ulIndex].InMap == 1)  &&
				 (RefDieInfo.RefDie[ulIndex].MapPos.Row > (MapPos.Row)) )
			{
				if (bRowMeet)
					return FALSE;
				else
					return TRUE;
			}

			//if current die pos is one row below ref dices grid
			if (MapPos.Row == (RefDieInfo.RefDie[ulIndex].MapPos.Row))
			{
				bRowMeet = TRUE;

				if ( (MapPos.Col > RefDieInfo.RefDie[ulIndex].MapPos.Col) &&
					 (MapPos.Col <= (RefDieInfo.RefDie[ulIndex].MapPos.Col + RefDieInfo.RefDist.Col)) )
				{
					if (i%2 == 1)	//ODD row
					{
						if ( j == RefDieInfo.MaxRC.Col )
						{
//AfxMessageBox("ODD: Update RIGHTMOST", MB_SYSTEMMODAL);
							return TRUE;
						}
						else if ( (RefDieInfo.RefDie[ulIndex+1].InMap == 0) )
						{
//AfxMessageBox("ODD: Update MIDDLE", MB_SYSTEMMODAL);
							return TRUE;
						}
						else
						{
							return FALSE;	//No Update
						}
					}
					else			//EVEN row
					{
						if ( (RefDieInfo.RefDie[ulIndex].InMap == 0) )
						{
//AfxMessageBox("EVEN: Update LEFT", MB_SYSTEMMODAL);
							return TRUE;
						}
						else
						{
							return FALSE;	//No Update
						}
					}
				}
				else if ( (MapPos.Col < RefDieInfo.RefDie[ulIndex].MapPos.Col) && (j == RefDieInfo.MinRC.Col) )
				{
					if (i%2 == 0)		//If EVEN row
					{
//AfxMessageBox("EVEN: LEFTMOST Update 1", MB_SYSTEMMODAL);
						return TRUE;
					}
					else
					{
						return FALSE;		//No Update
					}
				}
			}
		}
	}
	return FALSE;
}


LONG CWT_BlkFunc::GetCurrBlockNum(LONG& lRow, LONG& lCol, DieMapPos MapPos)
{
	ULONG ulIndex = 0;
	LONG lBlockNum = 0;

	//v3.25T13
	for (int i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	{
		for (int j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
		{
			ulIndex = i*MaxRefWidth + j;

			if ( (MapPos.Row > (RefDieInfo.RefDie[ulIndex].MapPos.Row)) &&
				 (MapPos.Row <= (RefDieInfo.RefDie[ulIndex].MapPos.Row + RefDieInfo.RefDist.Row)) )
			{
				if (MapPos.Col > (RefDieInfo.RefDie[ulIndex].MapPos.Col))
				{
					if (MapPos.Col <= (RefDieInfo.RefDie[ulIndex].MapPos.Col + RefDieInfo.RefDist.Col))
					{
						//Middle columns within a row
						lRow = i+1;
						lCol = j+1;
						lBlockNum = lRow * MaxRefWidth + lCol;
						return lBlockNum;
					}
				}
				else if ( (MapPos.Col <= (RefDieInfo.RefDie[ulIndex].MapPos.Col)) &&
						  (j == RefDieInfo.MinRC.Col) )
				{
					lRow = i+1;
					lCol = j;
					lBlockNum = lRow * MaxRefWidth + lCol;
					return lBlockNum;
				}
			}
			else if ( (MapPos.Row <= RefDieInfo.RefDie[ulIndex].MapPos.Row) &&
					  (i == RefDieInfo.MinRC.Row) )
			{
				lRow = i;

				if (MapPos.Col > (RefDieInfo.RefDie[ulIndex].MapPos.Col))
				{
					if (MapPos.Col <= (RefDieInfo.RefDie[ulIndex].MapPos.Col + RefDieInfo.RefDist.Col))
					{
						//Middle columns within a row
						lCol = j+1;
						lBlockNum = lRow * MaxRefWidth + lCol;
						return lBlockNum;
					}
				}
				else if ( (MapPos.Col <= (RefDieInfo.RefDie[ulIndex].MapPos.Col)) &&
						  (j == RefDieInfo.MinRC.Col) )
				{
					lCol = j;
					lBlockNum = lRow * MaxRefWidth + lCol;
					return lBlockNum;
				}
			}
		}
	}
	return lBlockNum;

/*
	ULONG ulIndex = 0;
	LONG lBlockNum = 0;

	for (int i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	{
		for (int j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
		{
			ulIndex = i*MaxRefWidth + j;

			//if (RefDieInfo.RefDie[ulIndex].InMap == 0)
			//	continue;

			if ( (MapPos.Row > (RefDieInfo.RefDie[ulIndex].MapPos.Row)) &&
				 (MapPos.Row <= (RefDieInfo.RefDie[ulIndex].MapPos.Row + RefDieInfo.RefDist.Row)) )
			{
				if (MapPos.Col > (RefDieInfo.RefDie[ulIndex].MapPos.Col))
				{
					if ( (RefDieInfo.RefDie[ulIndex+1].InMap == 1) &&
						 (MapPos.Col <= (RefDieInfo.RefDie[ulIndex+1].MapPos.Col)) )
					{
						//Middle columns within a row
						lRow = i+1;
						lCol = j+1;
						lBlockNum = lRow * MaxRefWidth + lCol;
						return lBlockNum;
					}
					else if (RefDieInfo.RefDie[ulIndex+1].InMap == 0)
					{
						//Rightmost column in the row
						lRow = i+1;
						lCol = j+1;
						lBlockNum = lRow * MaxRefWidth + lCol;
						return lBlockNum;
					}
				}
				else if ( (MapPos.Col <= (RefDieInfo.RefDie[ulIndex].MapPos.Col)) &&
						  (j == RefDieInfo.MinRC.Col) )
				{
					lRow = i+1;
					lCol = j;
					lBlockNum = lRow * MaxRefWidth + lCol;
					return lBlockNum;
				}
			}
			else if ( (MapPos.Row <= RefDieInfo.RefDie[ulIndex].MapPos.Row) &&
					  (i == RefDieInfo.MinRC.Row) )
			{
				lRow = i;

				if (MapPos.Col > (RefDieInfo.RefDie[ulIndex].MapPos.Col))
				{
					if ( (RefDieInfo.RefDie[ulIndex+1].InMap == 1) &&
						 (MapPos.Col <= (RefDieInfo.RefDie[ulIndex+1].MapPos.Col)) )
					{
						//Middle columns within a row
						lCol = j+1;
						lBlockNum = lRow * MaxRefWidth + lCol;
						return lBlockNum;
					}
					else if (RefDieInfo.RefDie[ulIndex+1].InMap == 0)
					{
						//Rightmost column in the row
						lCol = j+1;
						lBlockNum = lRow * MaxRefWidth + lCol;
						return lBlockNum;
					}
				}
				else if ( (MapPos.Col <= (RefDieInfo.RefDie[ulIndex].MapPos.Col)) &&
						  (j == RefDieInfo.MinRC.Col) )
				{
					lCol = j;
					lBlockNum = lRow * MaxRefWidth + lCol;
					return lBlockNum;
				}
			}
		}
	}

	return lBlockNum;
*/
}


BOOL CWT_BlkFunc::GotoTargetDie(LONG lSrcRow, LONG lSrcCol)
{
	ULONG	ulRow = 0, ulCol = 0;
	DieMapPos	CurDie, RefDieMap;
	DiePhyPos	CurPhy, RefDiePhy;
	LONG		lPosX, lPosY;
	LONG		lResult;

	if (FDCBasic.HaveAligned == 0)
	{
		return FALSE;
	}

	// Joystick off
	m_stnWaferTable->GetRtnForSetJoystick(FALSE);

	//Get Row/Col
	m_stnWaferTable->ConvertOrgUserToAsm(lSrcRow,lSrcCol,ulRow,ulCol);

	//Jump
	LONG lUserRow = 0, lUserCol = 0;
	m_stnWaferTable->m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
	m_stnWaferTable->ConvertAsmToOrgUser(ulRow,ulCol,lUserRow,lUserCol);

	CurDie.Row = ulRow;
	CurDie.Col = ulCol;
	lResult = GetNearRefDie(CurDie, &RefDieMap, &RefDiePhy);
	if ( lResult == FALSE) return FALSE;

	GetNextDiePhyPos(CurDie, &CurPhy);
	if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
	{
		if (Blk_bDebug)
		{
			pFile = NULL;
			errno_t nErr = fopen_s(&pFile, "FDCRegn.log","a+");
			strTemp.Format("Out of WafLimit(%d,%d)\n",CurPhy.x, CurPhy.y);
			if ((nErr == 0) && (pFile != NULL))		//Klocwork
			{
				fprintf(pFile,"%s", (LPCTSTR) strTemp);
				fclose(pFile);
			}
		}
		return FALSE;
	}
	WftMoveXYTo(CurPhy.x, CurPhy.y);
	Sleep(200);

	lPosX = CurPhy.x;
	lPosY = CurPhy.y;
	//lResult = GetNormalDieStatus(&lPosX, &lPosY);	//Don't Align
	//

	// Joystick off
	m_stnWaferTable->GetRtnForSetJoystick(TRUE);
	
	//if ( lResult == FALSE ) return FALSE;	//Klocwork	//v4.04
	return TRUE;
}

BOOL CWT_BlkFunc::GotoReferenceKey(LONG lSrcRow, LONG lSrcCol, LONG lTgtRow, LONG lTgtCol)
{
	ULONG	ulIntTgtRow = 0, ulIntTgtCol = 0;
	DieMapPos	CurDie, TgtDie;
	DiePhyPos	CurPhy;
	LONG		lResult;


	if (FDCBasic.HaveAligned == 0) 
	{
AfxMessageBox("FDCBasic.HaveAligned = 0", MB_SYSTEMMODAL);
		return FALSE;
	}

	INT i,j;
	for (i=0; i<MaxRow; i++)
	for (j=0; j<MaxCol; j++)
		DieArPhy[i][j] = 0;

	// Joystick off
	m_stnWaferTable->GetRtnForSetJoystick(FALSE);

	CurPhy.x = FDCBasic.HomeDie.PhyPos.x;
	CurPhy.y = FDCBasic.HomeDie.PhyPos.y;
	CurDie.Row = FDCBasic.HomeDie.MapPos.Row;
	CurDie.Col = FDCBasic.HomeDie.MapPos.Col;
	if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
	{
AfxMessageBox("GOTO die exceeds wafer limit!", MB_SYSTEMMODAL);
		return FALSE;
	}
	WftMoveXYTo(CurPhy.x, CurPhy.y);
	Sleep(50);

	m_stnWaferTable->ConvertOrgUserToAsm(lTgtRow,lTgtCol,ulIntTgtRow,ulIntTgtCol);
	TgtDie.Row = ulIntTgtRow;
	TgtDie.Col = ulIntTgtCol;

	lResult = IndexToCheckKeyDie(CurDie, TgtDie, &CurPhy, 0);

	// Joystick off
	m_stnWaferTable->GetRtnForSetJoystick(TRUE);
	
	if ( lResult == FALSE ) return FALSE;
	return TRUE;
}

BOOL CWT_BlkFunc::AlignTableTheta(DiePhyPos CurPos)
{
	LONG		i,j;
//	LONG		lThetaStep;
//	double		dThetaAngle;
//	double		dHoriOffX, dHoriOffY;
	DieMapPos	MapCtr, Point1, Point2;
	DiePhyPos	LftPos, RgtPos, TopPos, BtmPos;
//	LONG		lX, lY, lT;
	BOOL		bFindCtr;
	ULONG		ulIndex;

	//Get center
	bFindCtr = FALSE;
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].SrcMap.Row == 500 && RefDieInfo.RefDie[ulIndex].SrcMap.Col == 500)
		{
			MapCtr.Row = i;
			MapCtr.Col = j;
			bFindCtr = TRUE;
			break;
		}
	}
	if (bFindCtr == FALSE) return FALSE;

	// Set left & right
	bFindCtr = FALSE;
	i = MapCtr.Row;
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InWaf == 1)
		{
			Point1.Row = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Row;
			Point1.Col = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Col;
			LftPos.x = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.x;
			LftPos.y = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.y;
			bFindCtr = TRUE;
			break;
		}
	}
	if (bFindCtr == FALSE) return FALSE;

	bFindCtr = FALSE;
	for (j=RefDieInfo.MaxRC.Col; j>=RefDieInfo.MinRC.Col; j--)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InWaf == 1)
		{
			Point2.Row = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Row;
			Point2.Col = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Col;
			RgtPos.x = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.x;
			RgtPos.y = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.y;
			bFindCtr = TRUE;
			break;
		}
	}
	if (bFindCtr == FALSE) return FALSE;
	if (LftPos.x == RgtPos.x && LftPos.y == RgtPos.y) return FALSE;

	if( Point1.Col!=Point2.Col )	// divide by zero
	{
		FDCBasic.HPitchDie.x = (LftPos.x - RgtPos.x) / (Point1.Col - Point2.Col);
		FDCBasic.HPitchDie.y = (LftPos.y - RgtPos.y) / (Point1.Col - Point2.Col);
	}

	// Set top & bottom
	bFindCtr = FALSE;
	j = MapCtr.Col;
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; j++)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InWaf == 1)
		{
			Point1.Row = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Row;
			Point1.Col = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Col;
			TopPos.x = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.x;
			TopPos.y = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.y;
			bFindCtr = TRUE;
			break;
		}
	}
	if (bFindCtr == FALSE) return FALSE;

	bFindCtr = FALSE;
	for (i=RefDieInfo.MaxRC.Row; j>=RefDieInfo.MinRC.Row; j--)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InWaf == 1)
		{
			Point2.Row = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Row;
			Point2.Col = RefDieInfo.RefDie[i*MaxRefWidth+j].MapPos.Col;
			BtmPos.x = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.x;
			BtmPos.y = RefDieInfo.RefDie[i*MaxRefWidth+j].PhyPos.y;
			bFindCtr = TRUE;
			break;
		}
	}
	if (bFindCtr == FALSE) return FALSE;
	if (TopPos.x == BtmPos.x && TopPos.y == BtmPos.y) return FALSE;

	if( Point1.Row!=Point2.Row )	// divide by zero
	{
		FDCBasic.VPitchDie.x = (TopPos.x - BtmPos.x) / (Point1.Row - Point2.Row);
		FDCBasic.VPitchDie.y = (TopPos.y - BtmPos.y) / (Point1.Row - Point2.Row);
	}

	return TRUE;
}

BOOL CWT_BlkFunc::GetEdgePosition(LONG lStep)
{
	INT	i,j;
	INT	iUp,iDn,iLft,iRgt;
	DieMapPos RefDie, RefDieMap;
	DiePhyPos CurPhy;
	ULONG	ulIndex;


	//Index
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].InMap == 0) continue;
		if (RefDieInfo.RefDie[ulIndex].InWaf == 0) continue;
		RefDie.Row = i;
		RefDie.Col = j;
		HaveRefdieOn4Dir(RefDie,&iUp,&iDn,&iLft,&iRgt);

		if (iUp == FALSE)
		{
			RefDieMap.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
			RefDieMap.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
			CurPhy.x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			CurPhy.y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			IndexToEdgeDie(RefDieMap, &CurPhy, REGN_UP);
		}

		if (iLft == FALSE)
		{
			RefDieMap.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
			RefDieMap.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
			CurPhy.x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			CurPhy.y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			IndexToEdgeDie(RefDieMap, &CurPhy, REGN_LEFT);
		}

		if (iRgt == FALSE)
		{
			RefDieMap.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
			RefDieMap.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
			CurPhy.x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			CurPhy.y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			IndexToEdgeDie(RefDieMap, &CurPhy, REGN_RIGHT);
		}

		if (iDn == FALSE)
		{
			RefDieMap.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
			RefDieMap.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
			CurPhy.x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			CurPhy.y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			IndexToEdgeDie(RefDieMap, &CurPhy, REGN_DOWN);
		}

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

	}

	return TRUE;
}

BOOL CWT_BlkFunc::HaveRefdieOn4Dir(DieMapPos iRefCtr, INT *iUp, INT *iDown, INT *iLeft, INT *iRight)
{
	INT		i,j;
	ULONG	ulIndex;

	//Init
	*iUp = FALSE;
	*iDown = FALSE;
	*iLeft = FALSE;
	*iRight = FALSE;

	//Hori
	i = iRefCtr.Row;
	for (j=iRefCtr.Col-1; j>=RefDieInfo.MinRC.Col; j--)
	{
		ulIndex = i*MaxRefWidth+j;
		if (RefDieInfo.RefDie[ulIndex].InWaf!=0)
		{
			*iLeft = TRUE;
			break;
		}
	}
	for (j=iRefCtr.Col+1; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth+j;
		if (RefDieInfo.RefDie[ulIndex].InWaf!=0)
		{
			*iRight = TRUE;
			break;
		}
	}

	//Vert
	j = iRefCtr.Col;
	for (i=iRefCtr.Row-1; i>=RefDieInfo.MinRC.Row; i--)
	{
		ulIndex = i*MaxRefWidth+j;
		if (RefDieInfo.RefDie[ulIndex].InWaf!=0)
		{
			*iUp = TRUE;
			break;
		}
	}
	for (i=iRefCtr.Row+1; i<=RefDieInfo.MaxRC.Row; i++)
	{
		ulIndex = i*MaxRefWidth+j;
		if (RefDieInfo.RefDie[ulIndex].InWaf!=0)
		{
			*iDown = TRUE;
			break;
		}
	}

	return TRUE;
}

LONG CWT_BlkFunc::IndexToEdgeDie(DieMapPos SrcDie, DiePhyPos *CurPhyPos, LONG lDir)
{
	DieMapPos	CurDie, TgtDie;
//	DiePhyPos	*CurPhy;
	LONG		lStep = 1;
	LONG		lResult;

	ULONG		i,j;
	ULONG		ulRowNum=0, ulColNum=0;
	UCHAR		ucGrade;

	// Joystick off
	m_stnWaferTable->GetRtnForSetJoystick(FALSE);

	// Init Status
	CurDie = SrcDie;
	WftMoveXYTo(CurPhyPos->x, CurPhyPos->y);
	Sleep(200);

	if (!m_stnWaferTable->m_pWaferMapManager->GetWaferMapDimension(ulRowNum,ulColNum))
	{
		return FALSE;
	}

	lResult = FALSE;
 	if ( lDir == REGN_UP )
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

 	if ( lDir == REGN_DOWN )
	{
		j = CurDie.Col;
		ULONG ii = 0;
		for(ii=0; ii<=ulRowNum-1; ii++)
		{
			i = ulRowNum-1-ii;
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
 	
	if ( lDir == REGN_LEFT )
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
	
	if ( lDir == REGN_RIGHT )
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
		IndexToNormalDie(CurDie, TgtDie, CurPhyPos, 3);
	}

	return TRUE;
}

BOOL CWT_BlkFunc::RealignRefDiePos(BOOL bReset, BOOL bEdge)
{
	INT		i,j;
	LONG	lX, lY, lT;
	DiePhyPos CurPhy;
	ULONG	ulIndex;
	BOOL	bResult, bGetPstn;

	if (Blk_bDebug)
	{
		pFile = NULL;
		errno_t nErr = fopen_s(&pFile, "FDCBlock.log","a+");
		strTemp.Format("\nRealign FDC\n");
		if ((nErr == 0) && (pFile != NULL))		//Klocwork
		{
			fprintf(pFile,"%s", (LPCTSTR) strTemp);
			fclose(pFile);
		}
	}

	bGetPstn = FALSE;

	if (FDCBasic.HaveAligned == 0)
		return FALSE;

	if ( bEdge )
	{
		for (i=0; i<MaxRow; i++)
		for (j=0; j<MaxCol; j++)
		{
			DiePhyPrep[i][j].x = 0;
			DiePhyPrep[i][j].y = 0;
			DiePhyPrep[i][j].t = 0;
			DiePhyPrep[i][j].Status = Regn_Init;
		}
	}

	// Joystick off
	m_stnWaferTable->GetRtnForSetJoystick(FALSE);

	//Search RefDie
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].InMap == 0) continue;
		if (RefDieInfo.RefDie[ulIndex].InWaf == 0) continue;

		CurPhy.x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
		CurPhy.y = RefDieInfo.RefDie[ulIndex].PhyPos.y;

		if (Blk_bDebug)
		{
			pFile = NULL;
			errno_t nErr = fopen_s(&pFile, "FDCBlock.log","a+");
			strTemp.Format("Map(%3d;%3d),Phy(%6d,%6d),",
				RefDieInfo.RefDie[ulIndex].SrcMap.Row, RefDieInfo.RefDie[ulIndex].SrcMap.Col,
				RefDieInfo.RefDie[ulIndex].PhyPos.x,   RefDieInfo.RefDie[ulIndex].PhyPos.y);
			if ((nErr == 0) && (pFile != NULL))		//Klocwork
			{
				fprintf(pFile,"%s", (LPCTSTR) strTemp);
				fclose(pFile);
			}
		}

		if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhy.x,CurPhy.y) == FALSE)
		{
			return FALSE;
		}
		WftMoveXYTo(CurPhy.x, CurPhy.y);
		Sleep(800);

		// Align RefDie
		GetWTPosition(&lX,&lY,&lT);
		bResult = SearchCurrDie(1, 1, &lX, &lY);
		if ( bResult == TRUE )
		{
			bGetPstn = TRUE;

			CurPhy.x = lX;
			CurPhy.y = lY;

			if (Blk_bDebug)
			{
				pFile = NULL;
				errno_t nErr = fopen_s(&pFile, "FDCBlock.log","a+");
				strTemp.Format("PhyUpd(%6d,%6d),Offset(%4d,%4d)\n",
					CurPhy.x, CurPhy.y,
					RefDieInfo.RefDie[ulIndex].PhyPos.x - CurPhy.x,
					RefDieInfo.RefDie[ulIndex].PhyPos.y - CurPhy.y);
				if ((nErr == 0) && (pFile != NULL))		//Klocwork
				{
					fprintf(pFile, "%s", (LPCTSTR) strTemp);	//Klocwork
					fclose(pFile);
				}
			}

			if (bReset)
			{
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;

				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
				DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
			}//bReset

		}//SearchDie is TRUE;
		else
		{
			RefDieInfo.RefDie[ulIndex].InWaf = 0;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Init;
		}

	}

	if ( !bGetPstn ) return FALSE; 
		

	if ( bEdge )
	{
		GetEdgePosition(1);
	}

	return TRUE;
}

BOOL CWT_BlkFunc::GetIniOutputPath(CString *strPathWaf, CString *strPathBin, CString *strPathLog)
{
	BOOL bIfFileExist;
	CStdioFile cfCreePathFile;
	CString szLineData, szOutputLogFilePath, szOutputBinFilePath, szTempPath;
	INT		iIndex;

	//Get ouput file path
	bIfFileExist = cfCreePathFile.Open(_T("C:\\MapSorter\\Exe\\Cree.Ini"), CFile::modeRead|CFile::shareDenyNone|CFile::typeText);

	if ( !bIfFileExist ) return FALSE;

	cfCreePathFile.SeekToBegin();
	while (cfCreePathFile.ReadString(szLineData))
	{
		if (szLineData == "[Output Log File]")
		{
			cfCreePathFile.ReadString(szTempPath);
			iIndex = szTempPath.Find("=");
		if( iIndex!=-1 )
			szOutputLogFilePath = szTempPath.Mid(iIndex + 2);
		}
		else if (szLineData == "[Output Bin File]")
		{
			cfCreePathFile.ReadString(szTempPath);
			iIndex = szTempPath.Find("=");
		if( iIndex!=-1 )
			szOutputBinFilePath = szTempPath.Mid(iIndex + 2);
		}
	}
	cfCreePathFile.Close();

	*strPathWaf = "";
	*strPathBin = szOutputBinFilePath;
	*strPathLog = szOutputLogFilePath;

	return TRUE;
}

BOOL CWT_BlkFunc::GetIniOutputFileNm(CString *strWafEnd, CString *strBinFullBin, CString *strBinFullLog)
{
	CString	strPathWaf, strPathBin, strPathLog;
	CString	strFileNmWaf, strFileNmBin, strFileNmLog;
	CString	strMchNo, strGrd;
	int	iMchNo, iGrd;

	//Get Output Path
	GetIniOutputPath(&strPathWaf,&strPathBin,&strPathLog);

	m_strMachineID = "9999";
	m_stnWaferTable->GetRtnForMachineNo(&m_strMachineID);
	iMchNo = atoi(m_strMachineID);
	if (iMchNo < 0)
		strMchNo = "0000";
	else if (iMchNo < 10)
		strMchNo.Format("000%d", iMchNo);
	else if (iMchNo < 100)
		strMchNo.Format("00%d", iMchNo);
	else if (iMchNo < 1000)
		strMchNo.Format("0%d", iMchNo);
	else if (iMchNo < 10000)
		strMchNo.Format("%d", iMchNo);
	else
		strMchNo = "9999";

	iGrd = (INT)(MnNoDieGrade);
	if (iGrd <= 0)
		strGrd = "0000";
	else if (iGrd < 10)
		strGrd.Format("000%d", iGrd);
	else if (iGrd < 100)
		strGrd.Format("00%d", iGrd);
	else if (iGrd < 1000)
		strGrd.Format("%0d", iGrd);
	else if (iGrd < 10000)
		strGrd.Format("%d", iGrd);
	else
		strGrd = "9999";


	strFileNmWaf.Format("%s\\Waf_End.txt", strPathWaf);
	strFileNmBin.Format("%s\\%d.txt", strPathBin, iGrd);
	//strFileNmLog.Format("%s\\%s%s01.txt", strPathLog, strMchNo, strGrd);
	strFileNmLog.Format("%s\\%s%s.txt", strPathLog, strMchNo, strGrd);

	*strWafEnd = strFileNmWaf;
	*strBinFullBin = strFileNmBin;
	*strBinFullLog = strFileNmLog;

	return TRUE;
}





//For Character reference die functions
//Suppport function

LONG CWT_BlkFunc::SearchCurrCharDie(LONG *lPosX, LONG *lPosY, LONG *lDigit1, LONG *lDigit2)
{
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	LONG	lX,lY;
	LONG	lStatus = 0;

	typedef struct
	{
		LONG lStatus;
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
	} DIE_POS;

	DIE_POS stInfo;

	// Get the reply for PR result
	nConvID = m_stnWaferTable->GetIPCClientCom().SendRequest(WAFER_PR_STN, "SearchCharDieInFOV", stMsg);
	while (1)
	{
		if( m_stnWaferTable->GetIPCClientCom().ScanReplyForConvID(nConvID, 300000) == TRUE )
		{
			m_stnWaferTable->GetIPCClientCom().ReadReplyForConvID(nConvID, stMsg);

			stMsg.GetMsg(sizeof(DIE_POS), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	switch(stInfo.lStatus)
	{
		case 1:		//Only good die is found
			lStatus = Regn_IsGoodDie;
			break;

		case 2:		//Character die is found
			lStatus = Regn_IsHomeDie;
			break;

		default:	//No good die is found
			lStatus = Regn_IsEmptyDie;
			break;
	}

	lX = stInfo.lX;
	lY = stInfo.lY;
	*lDigit1 = stInfo.lDigit1;
	*lDigit2 = stInfo.lDigit2;

	*lPosX = *lPosX + lX;
	*lPosY = *lPosY + lY;

	if (m_stnWaferTable->GetRtnForCheckWaferLimit(*lPosX,*lPosY) == FALSE)
	{
		return WT_ALN_OUT_WAFLIMIT;	//WT_ALN_OUT_WAFLIMIT;
	}

	//Move to the found die position
	WftMoveXYTo(*lPosX, *lPosY);
	Sleep(50);

	return lStatus;
}



LONG CWT_BlkFunc::GetNextCharDieXY(DiePhyPos PhyInput, LONG lRow, LONG lCol, DiePhyPos *PhyOutput)
{
	int i;
	DiePhyPos	CurPhyPos;
	DiePhyPos	SrcPhyPos;

	LONG	lJStep;
	LONG	lResult;
	LONG	lPosX, lPosY;

	LONG	PitchX_X = 0;
	LONG	PitchX_Y = 0;
	LONG	PitchY_X = 0;
	LONG	PitchY_Y = 0;

	LONG	lDigit1, lDigit2;

	// Get die pitch
	m_stnWaferTable->GetDiePitchX(&PitchX_X, &PitchX_Y);
	m_stnWaferTable->GetDiePitchY(&PitchY_X, &PitchY_Y);

	SrcPhyPos.x = PhyInput.x;
	SrcPhyPos.y = PhyInput.y;

	CurPhyPos.x = PhyInput.x;
	CurPhyPos.y = PhyInput.y;

	//Move on y direction
	if (lRow != 0)
	{
		i = 0;
		ResetJStep(abs(lRow));
		while (RefDieInfo.arrJStep[i] != 0)
		{
			if (lRow < 0)
			{
				//To Up Direction
				lJStep = RefDieInfo.arrJStep[i];
			}
			else
			{
				//To Dn Direction
				lJStep = RefDieInfo.arrJStep[i]*(-1);
			}

			CurPhyPos.x -= lJStep*PitchY_X;
			CurPhyPos.y -= lJStep*PitchY_Y;

			if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhyPos.x,CurPhyPos.y) == FALSE)
			{
				PhyOutput->x = SrcPhyPos.x;
				PhyOutput->y = SrcPhyPos.y;
				return FALSE; //WT_ALN_OUT_WAFLIMIT;
			}
			WftMoveXYTo(CurPhyPos.x, CurPhyPos.y);
			Sleep(200);

			//Display map index
			if (RefDieInfo.arrJStep[i+1]!=0)
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				//lResult = GetNormalDieStatus(&lPosX, &lPosY);
				lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);

				//if ( lResult == WT_ALN_IS_GOOD || lResult == WT_ALN_IS_DEFECT )
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
				}
				i++;
			}
			else
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult	= SearchCurrCharDie(&lPosX, &lPosY, &lDigit1, &lDigit2);
				if ( lResult == Regn_IsHomeDie )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
					i++;
				}
				else
				{
					if ( m_bRefRhombus )
					{
						lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);
						if ( lResult == TRUE )
						{
							CurPhyPos.x = lPosX;
							CurPhyPos.y = lPosY;
							i++;
						}
						else
						{
							PhyOutput->x = SrcPhyPos.x;
							PhyOutput->y = SrcPhyPos.y;
							return FALSE;
						}
					}
					else
					{
						PhyOutput->x = SrcPhyPos.x;
						PhyOutput->y = SrcPhyPos.y;
						return FALSE;
					}
				}
			}
		}//endwhile
	}//end y direction

	//Move on x direction
	if (lCol != 0)
	{
		i = 0;
		ResetJStep(abs(lCol));
		while (RefDieInfo.arrJStep[i] != 0)
		{
			if (lCol < 0)
			{
				//To Lf Direction
				lJStep = RefDieInfo.arrJStep[i];
			}
			else
			{
				//To Rt Direction
				lJStep = RefDieInfo.arrJStep[i]*(-1);
			}

			CurPhyPos.x -= lJStep*PitchX_X;
			CurPhyPos.y -= lJStep*PitchX_Y;

			if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurPhyPos.x,CurPhyPos.y) == FALSE)
			{
				PhyOutput->x = SrcPhyPos.x;
				PhyOutput->y = SrcPhyPos.y;
				return FALSE;	//WT_ALN_OUT_WAFLIMIT;
			}
			WftMoveXYTo(CurPhyPos.x, CurPhyPos.y);
			Sleep(200);

			//Display map index
			if (RefDieInfo.arrJStep[i+1]!=0)
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				//lResult = GetNormalDieStatus(&lPosX, &lPosY);
				lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);

				//if ( lResult == WT_ALN_IS_GOOD || lResult == WT_ALN_IS_DEFECT )
				if ( lResult == TRUE )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
				}
				i++;
			}
			else
			{
				lPosX = CurPhyPos.x;
				lPosY = CurPhyPos.y;
				lResult	= SearchCurrCharDie(&lPosX, &lPosY, &lDigit1, &lDigit2);
				if ( lResult == Regn_IsHomeDie )
				{
					CurPhyPos.x = lPosX;
					CurPhyPos.y = lPosY;
					i++;
				}
				else
				{
					if ( m_bRefRhombus )
					{
						lResult = SearchCurrDie(0, 0, &lPosX, &lPosY);
						if ( lResult == TRUE )
						{
							CurPhyPos.x = lPosX;
							CurPhyPos.y = lPosY;
							i++;
						}
						else
						{
							PhyOutput->x = SrcPhyPos.x;
							PhyOutput->y = SrcPhyPos.y;
							return FALSE;
						}
					}
					else
					{
						PhyOutput->x = SrcPhyPos.x;
						PhyOutput->y = SrcPhyPos.y;
						return FALSE;
					}
				}
			}
		}//endwhile
	}//end x direction

	PhyOutput->x = CurPhyPos.x;
	PhyOutput->y = CurPhyPos.y;
	return TRUE;
}


LONG CWT_BlkFunc::IndexCrossCharDiePos(DiePhyPos **pRefDie, DieMapPos *pStack, int *pStackPtr)
{
	int			i,j;
	DieMapPos	HomeRC, CurrRC;
	DiePhyPos	HomePhy,CurrPhy;
	DieMapPos	MinRC, MaxRC;
	DiePhyPos	TgtPhy = {0,0};
		
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	int			DieStackPtr;
	int			iRefWidth = MaxRefWidth;

	LONG		sResult;
	LONG		iJStep;
	LONG		lX, lY, lT;

	//--------
	if (Blk_bDebug == 1)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"IndexCrossRefDiePos()--Begin\n");
			fclose(fp);
		}
	}

	DieStackPtr = *pStackPtr;
	CurrRC.Row = pStack[DieStackPtr-1].Row;
	CurrRC.Col = pStack[DieStackPtr-1].Col;
	GetWTPosition(&lX, &lY, &lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;


	//Save HomeDie
	HomeRC = CurrRC;
	HomePhy = CurrPhy;

	iJStep = RefDieInfo.RefDist.Row;
	MinRC = MaxRC = CurrRC;
	while(CurrRC.Row > 0)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextCharDieXY(CurrPhy, iJStep, 0, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row-1][CurrRC.Col].x = TgtPhy.x;
			pRefDie[CurrRC.Row-1][CurrRC.Col].y = TgtPhy.y;
			pRefDie[CurrRC.Row-1][CurrRC.Col].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row - 1;
			pStack[DieStackPtr].Col = CurrRC.Col;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Row -= 1;
			if (CurrRC.Row < MinRC.Row ) MinRC.Row = CurrRC.Row;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row-1][CurrRC.Col].Status = -1;
			break;
		}
	}

	//Move to HomeDie
	CurrRC = HomeRC;
	CurrPhy = HomePhy; 
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	while(CurrRC.Row < MaxRefWidth-1)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextCharDieXY(CurrPhy, -iJStep, 0, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row+1][CurrRC.Col].x = TgtPhy.x;
			pRefDie[CurrRC.Row+1][CurrRC.Col].y = TgtPhy.y;
			pRefDie[CurrRC.Row+1][CurrRC.Col].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row + 1;
			pStack[DieStackPtr].Col = CurrRC.Col;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Row += 1;
			if (CurrRC.Row > MaxRC.Row ) MaxRC.Row = CurrRC.Row;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row+1][CurrRC.Col].Status = -1;
			break;
		}
	}

	//Move to HomeDie
	CurrRC = HomeRC;
	CurrPhy = HomePhy; 
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	iJStep = RefDieInfo.RefDist.Col;
	while(CurrRC.Col > 0)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextCharDieXY(CurrPhy, 0, iJStep, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col-1].x = TgtPhy.x;
			pRefDie[CurrRC.Row][CurrRC.Col-1].y = TgtPhy.y;
			pRefDie[CurrRC.Row][CurrRC.Col-1].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row;
			pStack[DieStackPtr].Col = CurrRC.Col - 1;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Col -= 1;
			if (CurrRC.Col < MinRC.Col ) MinRC.Col = CurrRC.Col;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col-1].Status = -1;
			break;
		}
	}

	//Move to HomeDie
	CurrRC = HomeRC;
	CurrPhy = HomePhy; 
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	while(CurrRC.Col < MaxRefWidth-1)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			return FALSE;
		}

		sResult = GetNextCharDieXY(CurrPhy, 0, -iJStep, &TgtPhy);
		if (sResult == TRUE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col+1].x = TgtPhy.x;
			pRefDie[CurrRC.Row][CurrRC.Col+1].y = TgtPhy.y;
			pRefDie[CurrRC.Row][CurrRC.Col+1].Status = 1;
			pStack[DieStackPtr].Row = CurrRC.Row;
			pStack[DieStackPtr].Col = CurrRC.Col + 1;
			DieStackPtr++;
			CurrPhy = TgtPhy;
			CurrRC.Col += 1;
			if (CurrRC.Col > MaxRC.Col ) MaxRC.Col = CurrRC.Col;
			continue;
		}
		else if (sResult == FALSE)
		{
			pRefDie[CurrRC.Row][CurrRC.Col+1].Status = -1;
			break;
		}
	}

	//Update Input Parameter
	*pStackPtr = DieStackPtr;

	// Add limit
	if ( RefDieInfo.MinRC.Row - RefDieInfo.MaxRC.Row == MinRC.Row - MaxRC.Row )
	{
		for (j=0; j<MaxRefWidth; j++)
		{
			pRefDie[MinRC.Row-1][j].Status = 2;
			pRefDie[MaxRC.Row+1][j].Status = 2;
		}
	}

	if ( RefDieInfo.MinRC.Col - RefDieInfo.MaxRC.Col == MinRC.Col - MaxRC.Col )
	{
		for (i=0; i<MaxRefWidth; i++)
		{
			pRefDie[i][MinRC.Col-1].Status = 2;
			pRefDie[i][MaxRC.Col+1].Status = 2;
		}
	}

	//Move to HomeDie
	WftMoveXYTo(HomePhy.x, HomePhy.y);

	if (Blk_bDebug == 1)
	{
		FILE	*fp = NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");
		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"%d RefDie in Cross\n", DieStackPtr);
			fprintf(fp,"IndexCrossRefDiePos()---End\n");
			fclose(fp);
		}
	}

	return TRUE;
}


LONG CWT_BlkFunc::ManAlignCharDieWafer(DiePhyPos CurTbl)
{
	int i,j;
	ULONG		ulIndex, ulIndex1, ulIndex2;
	LONG		iJStep;
	DieMapPos	CurrMap;
	DiePhyPos	CurrPhy, TgtPhy;
	int			iLimitMin=0, iLimitMax=0;	//Klocwork
	LONG		lX, lY, lT;
	LONG		lDigit1, lDigit2;
	ULONG		ulStartRow = 9999, ulStartCol = 9999;
	ULONG		ulRefNo = 0;
	ULONG		ulNumRefDie;
	WAF_CMapDieInformation *arrRef;

	LONG		sResult;
	DieMapPos	RefDieMap, CtrMap;
	DiePhyPos	RefDiePhy;
	BOOL		bFindCtr;



	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		RefDieInfo.RefDie[ulIndex].InWaf = 0;
	}

	//Move to center
 	CurrPhy.x = CurTbl.x;	//0;
	CurrPhy.y = CurTbl.y;	//0;
	WftMoveXYTo(CurrPhy.x, CurrPhy.y);

	//Find "19" in refercen die array list from wafermap
	ulNumRefDie = m_stnWaferTable->m_WaferMapWrapper.GetNumberOfReferenceDice();
	arrRef = new WAF_CMapDieInformation [ulNumRefDie];
	m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieList(arrRef, ulNumRefDie);
	for (i=0; i<(LONG)ulNumRefDie; i++)
	{
		if (m_stnWaferTable->m_WaferMapWrapper.HasReferenceFaceValue(arrRef[i].GetRow(), arrRef[i].GetColumn()) == TRUE)
		{
			ulRefNo = m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieFaceValue(arrRef[i].GetRow(), arrRef[i].GetColumn());
			if (ulRefNo == 19)
			{
				ulStartRow = arrRef[i].GetRow();
				ulStartCol = arrRef[i].GetColumn();
				break;		
			}
		}
	}

	//If there is no "19", use 1st reference die to start
	if ( (ulStartRow == 9999) || (ulStartCol == 9999) )
	{
		ulStartRow = arrRef[0].GetRow();
		ulStartCol = arrRef[0].GetColumn();
	}

	delete[] arrRef;


	//Conver to user coord
	m_stnWaferTable->ConvertAsmToOrgUser(ulStartRow, ulStartCol, CtrMap.Row, CtrMap.Col);

	//Ask for moving table to center
	bFindCtr = FALSE;

	if (m_stnWaferTable->m_WaferMapWrapper.HasReferenceFaceValue(ulStartRow, ulStartCol) == TRUE)
	{
		ulRefNo = m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieFaceValue(ulStartRow, ulStartCol);
	}
	strTemp.Format("Please move to center(r%d,c%d, R:%d)\nAnd press ENTER", CtrMap.Row, CtrMap.Col, ulRefNo);

	m_stnWaferTable->GetRtnForSetJoystick(TRUE);
	m_stnWaferTable->GetRtnForHmiMessage(strTemp, "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	m_stnWaferTable->GetRtnForSetJoystick(FALSE);
	GetWTPosition(&lX,&lY,&lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;

	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].SrcMap.Row == CtrMap.Row && RefDieInfo.RefDie[ulIndex].SrcMap.Col == CtrMap.Col)
		{
			sResult	= SearchCurrCharDie(&lX, &lY, &lDigit1, &lDigit2);
			if ( sResult == Regn_IsHomeDie )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				RefDieInfo.RefDie[ulIndex].InWaf = 1;
			}
			else
			{
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				RefDieInfo.RefDie[ulIndex].InWaf = 2;
			}

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;

			CurrMap.Row = i;
			CurrMap.Col = j;
			bFindCtr = TRUE;
			break;
		}
	}

	if (bFindCtr == FALSE) return FALSE;

	// Find TopLeft
	i = CurrMap.Row;
	j = CurrMap.Col;
	iJStep = RefDieInfo.RefDist.Row;

	// Top
	while (i>RefDieInfo.MinRC.Row)
	{
		i--;
		ulIndex = i*MaxRefWidth + j;
		sResult = GetNextRefDieXY(CurrPhy, iJStep, 0, &TgtPhy);
		if (sResult == TRUE)
		{
			RefDieInfo.RefDie[ulIndex].PhyPos.x = TgtPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = TgtPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			RefDieInfo.RefDie[ulIndex].InWaf = 1;

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;

			CurrPhy = TgtPhy;
			//continue;
		}
		else if (sResult == FALSE)
		{
			if (RefDieInfo.RefDie[ulIndex].InMap == 0) 
			{
				GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				continue;
			}

			m_stnWaferTable->GetRtnForSetJoystick(TRUE);

			m_stnWaferTable->ConvertOrgUserToAsm(RefDieInfo.RefDie[ulIndex].SrcMap.Row, RefDieInfo.RefDie[ulIndex].SrcMap.Col, ulStartRow, ulStartCol);
			if (m_stnWaferTable->m_WaferMapWrapper.HasReferenceFaceValue(ulStartRow, ulStartCol) == TRUE)
			{
				ulRefNo = m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieFaceValue(ulStartRow, ulStartCol);
			}
			strTemp.Format("Please move to center(r%d,c%d, R:%d)\nAnd press ENTER", RefDieInfo.RefDie[ulIndex].SrcMap.Row, RefDieInfo.RefDie[ulIndex].SrcMap.Col, ulRefNo);
			m_stnWaferTable->GetRtnForHmiMessage(strTemp, "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			m_stnWaferTable->GetRtnForSetJoystick(FALSE);

			// Align RefDie
			GetWTPosition(&lX,&lY,&lT);
			sResult	= SearchCurrCharDie(&lX, &lY, &lDigit1, &lDigit2);
			CurrPhy.x = lX;
			CurrPhy.y = lY;
			RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			if ( sResult == Regn_IsHomeDie )
			{
				RefDieInfo.RefDie[ulIndex].InWaf = 1;
			}
			else
			{
				RefDieInfo.RefDie[ulIndex].InWaf = 2;
			}

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
		}
	}

	//Left
	i = RefDieInfo.MinRC.Row;
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InMap==1)
		{
			iLimitMin = j;
			break;
		}
	}
	for (j=RefDieInfo.MaxRC.Col; j>=RefDieInfo.MinRC.Col; j--)
	{
		if (RefDieInfo.RefDie[i*MaxRefWidth+j].InMap==1)
		{
			iLimitMax = j;
			break;
		}
	}

	j = CurrMap.Col;
	iJStep = RefDieInfo.RefDist.Col;
	while (j>iLimitMin)
	{
		j--;
		ulIndex = i*MaxRefWidth + j;
		sResult = GetNextRefDieXY(CurrPhy, 0, iJStep, &TgtPhy);
		if (sResult == TRUE)
		{
			RefDieInfo.RefDie[ulIndex].PhyPos.x = TgtPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = TgtPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			RefDieInfo.RefDie[ulIndex].InWaf = 1;

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;

			CurrPhy = TgtPhy;
			//continue;
		}
		else if (sResult == FALSE)
		{
			if (RefDieInfo.RefDie[ulIndex].InMap == 0) 
			{
				GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				continue;
			}

			m_stnWaferTable->GetRtnForSetJoystick(TRUE);

			m_stnWaferTable->ConvertOrgUserToAsm(RefDieInfo.RefDie[ulIndex].SrcMap.Row, RefDieInfo.RefDie[ulIndex].SrcMap.Col, ulStartRow, ulStartCol);
			if (m_stnWaferTable->m_WaferMapWrapper.HasReferenceFaceValue(ulStartRow, ulStartCol) == TRUE)
			{
				ulRefNo = m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieFaceValue(ulStartRow, ulStartCol);
			}
			strTemp.Format("Please move to center(r%d,c%d, R:%d)\nAnd press ENTER", RefDieInfo.RefDie[ulIndex].SrcMap.Row, RefDieInfo.RefDie[ulIndex].SrcMap.Col, ulRefNo);

			m_stnWaferTable->GetRtnForHmiMessage(strTemp, "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			m_stnWaferTable->GetRtnForSetJoystick(FALSE);

			// Align RefDie
			GetWTPosition(&lX,&lY,&lT);
			sResult	= SearchCurrCharDie(&lX, &lY, &lDigit1, &lDigit2);
			CurrPhy.x = lX;
			CurrPhy.y = lY;
			RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			if ( sResult == Regn_IsHomeDie )
			{
				RefDieInfo.RefDie[ulIndex].InWaf = 1;
			}
			else
			{
				RefDieInfo.RefDie[ulIndex].InWaf = 2;
			}

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
		}

	}

	ulIndex1 = RefDieInfo.MinRC.Row*MaxRefWidth + iLimitMin;
	ulIndex2 = RefDieInfo.MinRC.Row*MaxRefWidth + CurrMap.Col;
	if (iLimitMin != CurrMap.Col)
	{
		FDCBasic.HPitchFDC.x = (RefDieInfo.RefDie[ulIndex2].PhyPos.x - RefDieInfo.RefDie[ulIndex1].PhyPos.x)//*1.0
			/(CurrMap.Col-iLimitMin);
		FDCBasic.HPitchFDC.y = (RefDieInfo.RefDie[ulIndex2].PhyPos.y - RefDieInfo.RefDie[ulIndex1].PhyPos.y)//*1.0
			/(CurrMap.Col-iLimitMin);
	}
	else
	{
		LONG PitchX_X, PitchX_Y;
        m_stnWaferTable->GetDiePitchX(&PitchX_X, &PitchX_Y);
		FDCBasic.HPitchFDC.x = -PitchX_X * RefDieInfo.RefDist.Col;
		FDCBasic.HPitchFDC.y = -PitchX_Y * RefDieInfo.RefDist.Col;
	}

	ulIndex1 = RefDieInfo.MinRC.Row*MaxRefWidth + CurrMap.Col;
	ulIndex2 = CurrMap.Row*MaxRefWidth + CurrMap.Col;
	if (RefDieInfo.MinRC.Row != CurrMap.Row)
	{
		FDCBasic.VPitchFDC.x = (RefDieInfo.RefDie[ulIndex2].PhyPos.x - RefDieInfo.RefDie[ulIndex1].PhyPos.x)//*1.0
			/(CurrMap.Row-RefDieInfo.MinRC.Row);
		FDCBasic.VPitchFDC.y = (RefDieInfo.RefDie[ulIndex2].PhyPos.y - RefDieInfo.RefDie[ulIndex1].PhyPos.y)//*1.0
			/(CurrMap.Row-RefDieInfo.MinRC.Row);
	}
	else
	{
		LONG PitchY_X, PitchY_Y;
		m_stnWaferTable->GetDiePitchY(&PitchY_X, &PitchY_Y);
		FDCBasic.VPitchFDC.x = -PitchY_X * RefDieInfo.RefDist.Row;
		FDCBasic.VPitchFDC.y = -PitchY_Y * RefDieInfo.RefDist.Row;
	}

	//Begin Index
	for (i=RefDieInfo.MinRC.Row; i<=RefDieInfo.MaxRC.Row; i++)
	for (j=RefDieInfo.MinRC.Col; j<=RefDieInfo.MaxRC.Col; j++)
	{
		ulIndex = i*MaxRefWidth + j;
		if (RefDieInfo.RefDie[ulIndex].InMap == 0) continue;
		if (RefDieInfo.RefDie[ulIndex].InWaf != 0) continue;

		GetNearRefDie(RefDieInfo.RefDie[ulIndex].MapPos, &RefDieMap, &RefDiePhy);

		CurrPhy.x = RefDiePhy.x
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Col-RefDieMap.Col)*1.0/RefDieInfo.RefDist.Col*FDCBasic.HPitchFDC.x)
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Row-RefDieMap.Row)*1.0/RefDieInfo.RefDist.Row*FDCBasic.VPitchFDC.x);
		CurrPhy.y = RefDiePhy.y
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Col-RefDieMap.Col)*1.0/RefDieInfo.RefDist.Col*FDCBasic.HPitchFDC.y)
			+ (LONG)((RefDieInfo.RefDie[ulIndex].MapPos.Row-RefDieMap.Row)*1.0/RefDieInfo.RefDist.Row*FDCBasic.VPitchFDC.y);

		if (m_stnWaferTable->GetRtnForCheckWaferLimit(CurrPhy.x,CurrPhy.y) == FALSE)
		{
			return FALSE;	//WT_ALN_OUT_WAFLIMIT;
		}
		WftMoveXYTo(CurrPhy.x, CurrPhy.y);
		Sleep(200);

		// Align RefDie
		GetWTPosition(&lX,&lY,&lT);
		sResult	= SearchCurrCharDie(&lX, &lY, &lDigit1, &lDigit2);
		if ( sResult == Regn_IsHomeDie )
		{
			CurrPhy.x = lX;
			CurrPhy.y = lY;
			RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
			RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
			RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
			RefDieInfo.RefDie[ulIndex].InWaf = 1;

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
		}
		else
		{
			m_stnWaferTable->GetRtnForSetJoystick(TRUE);

			m_stnWaferTable->ConvertOrgUserToAsm(RefDieInfo.RefDie[ulIndex].SrcMap.Row, RefDieInfo.RefDie[ulIndex].SrcMap.Col, ulStartRow, ulStartCol);
			if (m_stnWaferTable->m_WaferMapWrapper.HasReferenceFaceValue(ulStartRow, ulStartCol) == TRUE)
			{
				ulRefNo = m_stnWaferTable->m_WaferMapWrapper.GetReferenceDieFaceValue(ulStartRow, ulStartCol);
			}
			strTemp.Format("Please move to center(r%d,c%d, R:%d)\nAnd press ENTER", RefDieInfo.RefDie[ulIndex].SrcMap.Row, RefDieInfo.RefDie[ulIndex].SrcMap.Col, ulRefNo);

			m_stnWaferTable->GetRtnForHmiMessage(strTemp, "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			m_stnWaferTable->GetRtnForSetJoystick(FALSE);

			GetWTPosition(&lX,&lY,&lT);
			sResult	= SearchCurrCharDie(&lX, &lY, &lDigit1, &lDigit2);
			if ( sResult == Regn_IsHomeDie )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				RefDieInfo.RefDie[ulIndex].InWaf = 1;
			}
			else
			{
				RefDieInfo.RefDie[ulIndex].PhyPos.x = CurrPhy.x;
				RefDieInfo.RefDie[ulIndex].PhyPos.y = CurrPhy.y;
				RefDieInfo.RefDie[ulIndex].PhyPos.t = FDCBasic.HomeDie.PhyPos.t;
				RefDieInfo.RefDie[ulIndex].InWaf = 2;
			}

			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].x = RefDieInfo.RefDie[ulIndex].PhyPos.x;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].y = RefDieInfo.RefDie[ulIndex].PhyPos.y;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].t = RefDieInfo.RefDie[ulIndex].PhyPos.t;
			DiePhyPrep[RefDieInfo.RefDie[ulIndex].MapPos.Row][RefDieInfo.RefDie[ulIndex].MapPos.Col].Status = Regn_Align;
		}
	}

	if (Blk_bDebug == 1)
	{
		PrintFDC();
	}

	return TRUE;
}



BOOL CWT_BlkFunc::FoundAllCharDiePos()
{
	LONG		i,j;
	DiePhyPos**	pRefDiePos;
	DieMapPos*	pDieStack;
	LONG		lX,lY,lT;
	int			iJStep;

	ULONG		ulIndex;
	LONG		sResult;

	int			DieStackPtr;
	DieMapPos	CurrRC, MinRC, MaxRC;
	DiePhyPos	CurPhy, TgtPhy, HomePhy;
	int			iRefCount = 0;

	LONG		lDigit1, lDigit2;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	GetWTPosition(&lX, &lY, &lT);
	sResult = SearchCurrCharDie(&lX, &lY, &lDigit1, &lDigit2);
	if (sResult == Regn_IsEmptyDie)
	{
		return FALSE;
	}

	CurPhy.x = lX;
	CurPhy.y = lY;
	HomePhy.x = lX;
	HomePhy.y = lY;
	HomePhy.t = lT;
	FDCBasic.HomeDie.PhyPos.x = lX;
	FDCBasic.HomeDie.PhyPos.y = lY;
	FDCBasic.HomeDie.PhyPos.t = lT;

	InitBlockFunc();
	m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
	DrawBlockMap();

	pRefDiePos = new DiePhyPos*[MaxRefWidth];
	for (i=0; i<MaxRefWidth; i++) 
	{
		pRefDiePos[i] = new DiePhyPos[MaxRefWidth];
		for (j=0; j<MaxRefWidth; j++) 
		{
			pRefDiePos[i][j].x = 0;
			pRefDiePos[i][j].y = 0;
			pRefDiePos[i][j].t = FDCBasic.HomeDie.PhyPos.t;
			pRefDiePos[i][j].Status = 0;
		}
	}

	pDieStack = new DieMapPos[MaxNumOfFDC];
	for (i=0; i<MaxNumOfFDC; i++)
	{
		pDieStack[i].Row = 0;
		pDieStack[i].Col = 0;
		pDieStack[i].Status = 0;
	}

	iJStep = RefDieInfo.RefDist.Row;

	ulIndex = (MaxRefWidth/2) * MaxRefWidth + MaxRefWidth/2;
	pRefDiePos[MaxRefWidth/2][MaxRefWidth/2].x = lX;
	pRefDiePos[MaxRefWidth/2][MaxRefWidth/2].y = lY;
	pRefDiePos[MaxRefWidth/2][MaxRefWidth/2].Status = 1;

	MinRC.Row = MinRC.Col = MaxRefWidth/2;
	MaxRC.Row = MaxRC.Col = MaxRefWidth/2;
	CurrRC.Row = CurrRC.Col = MaxRefWidth/2;

	pDieStack[0].Row = MaxRefWidth/2;
	pDieStack[0].Col = MaxRefWidth/2;
	DieStackPtr = 1;

	GetWTPosition(&lX, &lY, &lT);
	CurPhy.x = lX;
	CurPhy.y = lY;

	// Alignment in Cross Path
	sResult = IndexCrossCharDiePos(pRefDiePos, pDieStack, &DieStackPtr);
	if (sResult == FALSE) 
	{
		//Release Pointer
		for (i=0; i<MaxRefWidth; i++)
			delete[] pRefDiePos[i];
		delete[] pRefDiePos;
		delete[] pDieStack;
		return FALSE;
	}


	while (DieStackPtr > 0)
	{
		if (pApp->IsStopAlign())		//v4.05	//Klocwork
		{
			//Release Pointer
			for (i=0; i<MaxRefWidth; i++)
				delete[] pRefDiePos[i];
			delete[] pRefDiePos;
			delete[] pDieStack;
			return FALSE;
		}

		if (CurrRC.Row < MinRC.Row ) MinRC.Row = CurrRC.Row;
		if (CurrRC.Col < MinRC.Col ) MinRC.Col = CurrRC.Col;
		if (CurrRC.Row > MaxRC.Row ) MaxRC.Row = CurrRC.Row;
		if (CurrRC.Col > MaxRC.Col ) MaxRC.Col = CurrRC.Col;
	
		if (   CurrRC.Row > 0
			&& pRefDiePos[CurrRC.Row-1][CurrRC.Col].Status == 0
		   )
		{
			sResult = GetNextCharDieXY(CurPhy,iJStep, 0, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row - 1;
				pDieStack[DieStackPtr].Col = CurrRC.Col;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Row -= 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row-1][CurrRC.Col].Status = -1;
			}
		}
		if (   CurrRC.Col > 0
			&& pRefDiePos[CurrRC.Row][CurrRC.Col-1].Status == 0
		   )
		{
			sResult = GetNextCharDieXY(CurPhy, 0, iJStep, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row;
				pDieStack[DieStackPtr].Col = CurrRC.Col - 1;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Col -= 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col-1].Status = -1;
			}
		}
		if (   CurrRC.Row < MaxRefWidth-1
			&& pRefDiePos[CurrRC.Row+1][CurrRC.Col].Status == 0
		   )
		{
			sResult = GetNextCharDieXY(CurPhy, -iJStep, 0, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row + 1;
				pDieStack[DieStackPtr].Col = CurrRC.Col;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Row += 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row+1][CurrRC.Col].Status = -1;
			}
		}
		if (   CurrRC.Col < MaxRefWidth-1
			&& pRefDiePos[CurrRC.Row][CurrRC.Col+1].Status == 0
		   )
		{
			sResult = GetNextCharDieXY(CurPhy, 0, -iJStep, &TgtPhy);
			if (sResult == TRUE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].x = TgtPhy.x; //lX;
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].y = TgtPhy.y; //lY;
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].Status = 1;
				pDieStack[DieStackPtr].Row = CurrRC.Row;
				pDieStack[DieStackPtr].Col = CurrRC.Col + 1;
				DieStackPtr++;
				CurPhy = TgtPhy;
				CurrRC.Col += 1;
				continue;
			}
			else if (sResult == FALSE)
			{
				pRefDiePos[CurrRC.Row][CurrRC.Col+1].Status = -1;
			}
		}

		iRefCount = iRefCount + 1;
		DieStackPtr--;
		if (DieStackPtr!=0)
		{
			CurrRC.Row = pDieStack[DieStackPtr-1].Row;
			CurrRC.Col = pDieStack[DieStackPtr-1].Col;
			CurPhy.x = pRefDiePos[CurrRC.Row][CurrRC.Col].x;
			CurPhy.y = pRefDiePos[CurrRC.Row][CurrRC.Col].y;
		}
	}//endwhile

	if ( Blk_bDebug )
	{
		FILE	*fp =  NULL;
		errno_t nErr = fopen_s(&fp, "FDCRegn.log","a+");

		if ((nErr == 0) && (fp != NULL))		//Klocwork
		{
			fprintf(fp,"iRefCount = %d\n",iRefCount);
			fprintf(fp,"MinRC(%ld,%ld),MaxRC(%ld,%ld)\n",MinRC.Row,MinRC.Col,MaxRC.Row,MaxRC.Col);
			fclose(fp);
		}
	}

	sResult = MergeAlignMap(pRefDiePos, MinRC, MaxRC);

	//Release Pointer
	for (i=0; i<MaxRefWidth; i++)
		delete[] pRefDiePos[i];
	delete[] pRefDiePos;
	delete[] pDieStack;

	if (sResult == FALSE)
	{
		LONG lTmp;

		lTmp = m_stnWaferTable->GetRtnForHmiMessage("Do you want to continue by\n\n    Manual-Alignment", "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		if (lTmp == glHMI_NO)
		{
			return FALSE;
		}
		sResult = ManAlignCharDieWafer(HomePhy);
		if (sResult == FALSE)
		{
			return FALSE;
		}
	}

	//Show RefDie
	if (sResult == TRUE)
	{
		DieMapPos DieTL,DieBR;
		ULONG	ulIndex,Count;
		LONG	ulRow, ulCol;

		Count = 0;
        for (i=0; i<MaxRefWidth; i++)
		for (j=0; j<MaxRefWidth; j++)
		{
			ulIndex = i*MaxRefWidth + j;
			if (RefDieInfo.RefDie[ulIndex].InWaf == 1)
			{
				Count++;
				DieTL.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row;
				DieTL.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col;
				DieBR.Row = RefDieInfo.RefDie[ulIndex].MapPos.Row+2;
				DieBR.Col = RefDieInfo.RefDie[ulIndex].MapPos.Col+2;

				m_stnWaferTable->m_pWaferMapManager->AddBoundary(DieTL.Row,DieBR.Row,DieTL.Col,DieBR.Col);

				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", RefDieInfo.RefDie[ulIndex].MapPos.Row);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", RefDieInfo.RefDie[ulIndex].MapPos.Col);
				m_stnWaferTable->m_WaferMapWrapper.SetPhysicalPosition(DieTL.Row,DieTL.Col,RefDieInfo.RefDie[ulIndex].PhyPos.x,RefDieInfo.RefDie[ulIndex].PhyPos.y);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Align);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);

				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", i);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", j);
				m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_AtRef);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);
			}
		}

		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Row", MnMaxJumpCtr);
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Col", MnMaxJumpEdge);
		m_stnWaferTable->m_WaferMapWrapper.SetAlgorithmParameter("Align", Regn_Init);
		m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);

		SetStartDie(HomePhy);
		GetStartDie(&ulRow, &ulCol, &lX, &lY);
		m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(ulRow,ulCol);
	}

	if (pApp->IsStopAlign())		//v4.05	//Klocwork
	{
		m_stnWaferTable->m_WaferMapWrapper.DeleteAllBoundary();
		return FALSE;
	}

	FDCBasic.HaveAligned = 1;

	RealignRefDiePos(TRUE,FALSE);

	CheckDieInSpecialGrade(MnNoDieGrade);

	//Align Theta
	if (sResult == TRUE)
	{
		LONG lRow,lCol;
		GetStartDie(&lRow, &lCol, &lX, &lY);
		m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(lRow,lCol);
	}

	return TRUE;
}

BOOL CWT_BlkFunc::WftMoveXYTo(LONG lPosX, LONG lPosY)
{
	return m_stnWaferTable->XY_SafeMoveTo(lPosX, lPosY);
}