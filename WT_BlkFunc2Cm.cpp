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

//================================================================
// Constructor / destructor implementation section
//================================================================
	
CWT_BlkFunc2::CWT_BlkFunc2()
{
	m_stnWaferTable				= NULL;
	m_lBlk2SrchRefGrid			= 10;
	m_lBlk2MaxJumpCtr			= 15;
	m_lBlk2MaxJumpEdge			= 1;
	m_ulBlk2EmptyGradeMinCount	= 0;
	m_bBlk2MnEdgeAlign			= FALSE;
	m_bDigitalF					= FALSE;
	m_lBlk2MnPassPercent		= 70;
	m_stFDCBasic.m_bHaveAligned = FALSE;

	m_bSrchCharDie				= FALSE;
	m_lReferDieLeartNum			= 1;
	m_lPrSrchDelay				= 30;
	m_l4ReferCheckScore			= 0;
	m_ulBlk2WaferReferNum		= 0;
}

CWT_BlkFunc2::~CWT_BlkFunc2()
{
	;
}

void CWT_BlkFunc2::SetWaferTable(CWaferTable* stnWaferTable)
{
	m_stnWaferTable = stnWaferTable;
}

LONG CWT_BlkFunc2::GetReferGrid()
{
	if( m_lBlk2SrchRefGrid<5 )
		m_lBlk2SrchRefGrid = 5;
	if( m_lBlk2SrchRefGrid>GetBlkPitchRow() )
		return GetBlkPitchRow();
	if( m_lBlk2SrchRefGrid>GetBlkPitchCol() )
		return GetBlkPitchCol();

	return m_lBlk2SrchRefGrid;
}

LONG CWT_BlkFunc2::GetEmptyDieGrade()
{
	if (CMS896AStn::m_bUseEmptyCheck == FALSE)
		return 0;
	return CMS896AStn::m_lMnNoDieGrade;
}

VOID CWT_BlkFunc2::Blk2SetSearch(LONG RefGrid, LONG JumpCtr, LONG JumpEdge)
{
	m_lBlk2SrchRefGrid = RefGrid;
	m_lBlk2MaxJumpCtr  = JumpCtr;
	m_lBlk2MaxJumpEdge = JumpEdge>0?JumpEdge:1;
}

VOID CWT_BlkFunc2::Blk2SetEmptyGradeMinCount(LONG lEmptyGradeMinCount)
{
	m_ulBlk2EmptyGradeMinCount = lEmptyGradeMinCount;
}

VOID CWT_BlkFunc2::Blk2SetAlignment(BOOL bEnableEdge,LONG lPassPercent,BOOL bDigitalF)
{
	m_bBlk2MnEdgeAlign = bEnableEdge;
	m_lBlk2MnPassPercent = lPassPercent;
	if( m_lBlk2MnPassPercent<0 || m_lBlk2MnPassPercent>100 )
		m_lBlk2MnPassPercent = 70;
	m_bDigitalF = bDigitalF;
}

VOID CWT_BlkFunc2::Blk2SetReferMode(BOOL bCharRefMode, LONG lTotalReferNum)	// should be enable to set to 0 for all refer die
{
	m_bSrchCharDie = bCharRefMode;

	m_lReferDieLeartNum = lTotalReferNum;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_lReferDieLeartNum<=0 || (pApp->GetCustomerName() != CTM_CREE) )
	{
		m_lReferDieLeartNum = 1;
	}
}

VOID CWT_BlkFunc2::Blk2SetBlock4ReferCheckScore(LONG lScore)
{
	m_l4ReferCheckScore = lScore;
}

BOOL  CWT_BlkFunc2::Blk2IsSrchCharDie()
{
	return m_bSrchCharDie;
}

DOUBLE CWT_BlkFunc2::GetDistIn2Points(DOUBLE P1X, DOUBLE P1Y, DOUBLE P2X, DOUBLE P2Y)
{
	return (sqrt((P1X-P2X)*(P1X-P2X)+(P1Y-P2Y)*(P1Y-P2Y)));
}

ULONG CWT_BlkFunc2::GetTotalRefPoints()
{
	return (GetBlkMaxRowIdx()-GetBlkMinRowIdx()+1) * (GetBlkMaxColIdx()-GetBlkMinColIdx()+1);
}


// 8888888888888888888888888888888888888
// wafer table motion relative functions
// 8888888888888888888888888888888888888
VOID CWT_BlkFunc2::Blk2GetWTPosition(LONG *lX, LONG *lY, LONG *lT)
{
	m_stnWaferTable->GetEncoder(lX, lY, lT);
}


VOID CWT_BlkFunc2::SetLogCtmMessage(CONST CString& szMsg)
{
	DisplayDebugString(szMsg);
	m_stnWaferTable->GetRtnSetCtmLogMessage(szMsg);
	SetFDCRegionLog(szMsg);
}

VOID CWT_BlkFunc2::SetFDCRegionLog(CONST CString &szMsg)
{
	FILE	*fp = NULL;
	errno_t nErr = fopen_s(&fp, gszUSER_DIRECTORY + "\\History\\Blk2FDCRegn.log","a+");		//v3.34
	if ((nErr == 0) && (fp != NULL))
	{
		CString szTime;
		CTime theTime = CTime::GetCurrentTime();
		szTime.Format("(%2d:%2d:%2d)", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());

		if( szMsg=="\n" )
			fprintf(fp, "\n");
		else
			fprintf(fp, "%s    %s\n", (LPCTSTR) szTime, (LPCTSTR) szMsg);
		fclose(fp);
	}
}

VOID CWT_BlkFunc2::DisplayDebugString(CONST CString szMsg)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetBlk2DebugLog() )
		OutputDebugString(szMsg);
}


VOID CWT_BlkFunc2::SetErrorLogMessage(CONST CString &szMsg)
{
	m_stnWaferTable->GetRtnSetErrorLogMessage(szMsg);
}

BOOL CWT_BlkFunc2::IsToStopAlign()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->IsStopAlign())		//v4.05	//Klocwork
	{
		SetFDCRegionLog("stopped by manual press stop align or front cover open");
		return TRUE;
	}

	return FALSE;
}

VOID CWT_BlkFunc2::SetDisplayErrorMessage(CONST CString szMsg)
{
	m_stnWaferTable->DisplayErrorMessage(szMsg);
}

LONG CWT_BlkFunc2::SetHmiMessage(const CString& szText,	const CString& szTitle,
							LONG lType,
							LONG lAlignment,
							LONG lTimeout,
							LONG lMsgMode,
							LONG* plMsgID,
							LONG lWidth,
							LONG lHeight,
							CString* pszBitmapFile,
							CString* pszCustomText1,
							CString* pszCustomText2,
							CString* pszCustomText3)
{
	LONG lReturn = m_stnWaferTable->HmiMessage_Red_Back(szText,szTitle,lType,lAlignment,lTimeout,lMsgMode,
				plMsgID,lWidth,lHeight,pszBitmapFile,pszCustomText1,pszCustomText2,pszCustomText3);

	return lReturn;
}

LONG CWT_BlkFunc2::SetHmiMessageEx(const CString& szText,const CString& szTitle,
							LONG lType,
							LONG lAlignment,
							LONG lTimeout,
							LONG lMsgMode,
							LONG* plMsgID,
							LONG lWidth,
							LONG lHeight,
							CString* pszBitmapFile,
							CString* pszCustomText1,
							CString* pszCustomText2,
							CString* pszCustomText3)
{
	LONG lReturn = m_stnWaferTable->HmiMessageEx_Red_Back(szText,szTitle,lType,lAlignment,lTimeout,lMsgMode,
				plMsgID,lWidth,lHeight,pszBitmapFile,pszCustomText1,pszCustomText2,pszCustomText3);
	return lReturn;
}

VOID CWT_BlkFunc2::SetJoystick(BOOL bOn)
{
	m_stnWaferTable->GetRtnForSetJoystick(bOn);
}

BOOL CWT_BlkFunc2::GetBlkAligned()
{
	return	m_stFDCBasic.m_bHaveAligned;
}

LONG CWT_BlkFunc2::GetBlkPitchRow()
{
	return	m_stFDCBasic.m_ulFDCPitchRow;
}

LONG CWT_BlkFunc2::GetBlkPitchCol()
{
	return	m_stFDCBasic.m_ulFDCPitchCol;
}

LONG CWT_BlkFunc2::GetBlkMinRowIdx()
{
	return	1;
}

LONG CWT_BlkFunc2::GetBlkMinColIdx()
{
	return	1;
}

LONG CWT_BlkFunc2::GetBlkMaxRowIdx()
{
	return	m_stFDCBasic.m_ulMaxRowIdx;
}

LONG CWT_BlkFunc2::GetBlkMaxColIdx()
{
	return	m_stFDCBasic.m_ulMaxColIdx;
}

LONG CWT_BlkFunc2::GetDiePitchHX()
{
	return	(0 - m_stnWaferTable->GetDiePitchX_X());
}

LONG CWT_BlkFunc2::GetDiePitchHY()
{
	return	(0 - m_stnWaferTable->GetDiePitchX_Y());
}

LONG CWT_BlkFunc2::GetDiePitchVX()
{
	return	(0 - m_stnWaferTable->GetDiePitchY_X());
}

LONG CWT_BlkFunc2::GetDiePitchVY()
{
	return	(0 - m_stnWaferTable->GetDiePitchY_Y());
}

LONG CWT_BlkFunc2::GetBlkPitchHX()
{
	return	m_stFDCBasic.m_lFDCPitchX_X;
}

LONG CWT_BlkFunc2::GetBlkPitchHY()
{
	return	m_stFDCBasic.m_lFDCPitchX_Y;
}

LONG CWT_BlkFunc2::GetBlkPitchVX()
{
	return	m_stFDCBasic.m_lFDCPitchY_X;
}

LONG CWT_BlkFunc2::GetBlkPitchVY()
{
	return	m_stFDCBasic.m_lFDCPitchY_Y;
}

BOOL CWT_BlkFunc2::CheckWaferLimit(LONG lX, LONG lY)
{
	BOOL bLimit = m_stnWaferTable->GetRtnForCheckWaferLimit(lX, lY);
	if( bLimit==FALSE )
	{
		CString szMsg;
		szMsg.Format("      check wafer limit out", lX, lY);
		SetFDCRegionLog(szMsg);
	}
	return bLimit;
}

LONG CWT_BlkFunc2::GetHomeTheta()
{
	return	m_stFDCBasic.m_stHomeDiePhyT;
}

ULONG CWT_BlkFunc2::GetMapReferNum()
{
	return m_stFDCBasic.m_ulReferDieNum;
}

BOOL CWT_BlkFunc2::IsAutoManualAlign()	// cree not use true
{
	return CMS896AStn::m_bUseAutoManualAlign;
}

LONG CWT_BlkFunc2::GetReferDiePhyX(ULONG ulIndex)
{
	return	m_stReferWftInfo[ulIndex].m_lPhyPosX;
}

LONG CWT_BlkFunc2::GetReferDiePhyY(ULONG ulIndex)
{
	return	m_stReferWftInfo[ulIndex].m_lPhyPosY;
}

LONG CWT_BlkFunc2::GetReferSrcMapRow(ULONG ulIndex)
{
	ULONG ulRow = GetReferMapPosRow(ulIndex);
	ULONG ulCol = GetReferMapPosCol(ulIndex);
	LONG lOrgRow = 0, lOrgCol = 0;
	m_stnWaferTable->ConvertAsmToOrgUser(ulRow, ulCol, lOrgRow, lOrgCol);
	return	lOrgRow;
}

LONG CWT_BlkFunc2::GetReferSrcMapCol(ULONG ulIndex)
{
	ULONG ulRow = GetReferMapPosRow(ulIndex);
	ULONG ulCol = GetReferMapPosCol(ulIndex);
	LONG lOrgRow = 0, lOrgCol = 0;
	m_stnWaferTable->ConvertAsmToOrgUser(ulRow, ulCol, lOrgRow, lOrgCol);
	return	lOrgCol;
}

LONG CWT_BlkFunc2::GetReferMapPosRow(ULONG ulIndex)
{
	return	m_stReferWftInfo[ulIndex].m_MapPos.Row;
}

LONG CWT_BlkFunc2::GetReferMapPosCol(ULONG ulIndex)
{
	return	m_stReferWftInfo[ulIndex].m_MapPos.Col;
}

LONG CWT_BlkFunc2::GetReferMapPosValue(ULONG ulIndex)
{
	return	m_stReferWftInfo[ulIndex].m_lFaceValue;
}

BOOL CWT_BlkFunc2::GetReferInMap(ULONG ulIndex)
{
	return m_stReferWftInfo[ulIndex].m_bInMap;
}

char CWT_BlkFunc2::GetReferInWfa(ULONG ulIndex)
{
	return m_stReferWftInfo[ulIndex].m_cInWaf;
}

BOOL CWT_BlkFunc2::Blk2SearchReferOnlyInFOV(LONG *lPosX, LONG *lPosY, BOOL bAlign)
{
	LONG lDig1 = 0, lDig2 = 0;
	return Blk2SearchReferDie(lPosX, lPosY, &lDig1, &lDig2, bAlign, TRUE);
}

BOOL CWT_BlkFunc2::Blk2SearchNmlReferDie(BOOL bFov, LONG lPrID, LONG &lPrOffsetX, LONG &lPrOffsetY)
{
	IPC_CServiceMessage stMsg;
	int		nConvID = 0;
	LONG	lSrchRefMsg[2];

	typedef struct
	{
		LONG lSrchMode;
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
		LONG lResult;
	} WTPos;
	WTPos stPRS;


	lPrOffsetX = 0;
	lPrOffsetY = 0;
	lSrchRefMsg[0] = bFov;
	lSrchRefMsg[1] = lPrID;
	stMsg.InitMessage(2*sizeof(LONG), lSrchRefMsg);
	// Get the reply for PR result
	nConvID = m_stnWaferTable->GetIPCClientCom().SendRequest(WAFER_PR_STN, "SearchReferDieInWafer", stMsg);
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

	if (stPRS.lResult != REGN_IS_GOODDIE)
	{
		return FALSE;
	}

	lPrOffsetX = stPRS.lX;
	lPrOffsetY = stPRS.lY;

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2SearchDgtReferDie(LONG &lPrOffsetX, LONG &lPrOffsetY, LONG *lDig1, LONG *lDig2)
{
	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
		LONG lResult;
	} WTPos;

	WTPos stPRS;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	nConvID = m_stnWaferTable->GetIPCClientCom().SendRequest(WAFER_PR_STN, "SearchDigitalInWafer", stMsg);
	while (1)
	{
		if( m_stnWaferTable->GetIPCClientCom().ScanReplyForConvID(nConvID, 300000) == TRUE )
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

	lPrOffsetX = 0;
	lPrOffsetY = 0;
	*lDig1 = 0;
	*lDig2 = 0;
	BOOL bReferRtn = FALSE;
	if( stPRS.lResult==1 )
	{
		bReferRtn = TRUE;
		lPrOffsetX = stPRS.lX;
		lPrOffsetY = stPRS.lY;
		*lDig1 = stPRS.lDigit1;
		*lDig2 = stPRS.lDigit2;
	}

	CString szMsg;
	szMsg.Format("Blk2 Search Dgt ReferDie %d, pr(%ld,%ld), dig(%ld,%ld)", 
		stPRS.lResult, lPrOffsetX, lPrOffsetY, *lDig1, *lDig2);
	DisplayDebugString(szMsg);

	return bReferRtn;
}

BOOL CWT_BlkFunc2::Blk2SearchReferDieInFOV(LONG *lPosX, LONG *lPosY, LONG *lDig1, LONG *lDig2, BOOL bAlign)
{
	return Blk2SearchReferDie(lPosX, lPosY, lDig1, lDig2, bAlign, TRUE);
}

LONG CWT_BlkFunc2::Blk2GetNormalDieStatus(LONG *lCurX, LONG *lCurY)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;

	//Init Message
	stSrchInfo.bNormalDie	= TRUE;
	stSrchInfo.lRefDieNo	= 1;
	stSrchInfo.bShowPRStatus = TRUE;
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
	if (CheckWaferLimit(*lCurX,*lCurY) == FALSE)
	{
		return WT_ALN_OUT_WAFLIMIT;
	}

	MoveXYTo(*lCurX, *lCurY);
	Sleep(m_lPrSrchDelay);

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

BOOL CWT_BlkFunc2::Blk2SearchNormalDie(LONG lSrchWin, LONG *lPosX, LONG *lPosY, BOOL bAlign)
{
	IPC_CServiceMessage stMsg;

	int		nConvID = 0;
	LONG	lX,lY;
	LONG	lRtn;

	LONG	lSrchNmlMsg[2];

	typedef struct
	{
		LONG lSrchMode;
		LONG lX;
		LONG lY;
		LONG lDigit1;
		LONG lDigit2;
		LONG lResult;
	} WTPos;
 	WTPos stPRS;

	lX = lY = 0;
	lRtn = FALSE;

	// Init Message
	lSrchNmlMsg[0] = lSrchWin;
	lSrchNmlMsg[1] = 1;	// lPrId
	stMsg.InitMessage(2*sizeof(LONG), lSrchNmlMsg);

	// Get the reply for PR result
	nConvID = m_stnWaferTable->GetIPCClientCom().SendRequest(WAFER_PR_STN, "SearchNormalDieInWafer", stMsg);
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

	if (stPRS.lResult == REGN_IS_GOODDIE)
	{
		lX = stPRS.lX;
		lY = stPRS.lY;
		lRtn = TRUE;
	}
	else
	{
		lX = 0;
		lY = 0;
		lRtn = FALSE;
	}


	*lPosX = *lPosX + lX;
	*lPosY = *lPosY + lY;

	if (CheckWaferLimit(*lPosX,*lPosY) == FALSE)
	{
		return FALSE;
	}

	if ( bAlign )
	{
		MoveXYTo(*lPosX, *lPosY);
		Sleep(m_lPrSrchDelay);
	}

	return lRtn;
}


short CWT_BlkFunc2::GetPrepDieStatus(LONG lRowIdx, LONG lColIdx)
{
	if( lRowIdx>=0 && lRowIdx<MAPMAXROW2 && lColIdx>=0 && lColIdx<MAPMAXCOL2 )
		return m_stDiePhyPrep[lRowIdx][lColIdx].m_sState;

	return REGN_INIT2;
}

LONG CWT_BlkFunc2::GetPrepDieWftX(LONG lRowIdx, LONG lColIdx)
{
	if( lRowIdx>=0 && lRowIdx<MAPMAXROW2 && lColIdx>=0 && lColIdx<MAPMAXCOL2 )
		return m_stDiePhyPrep[lRowIdx][lColIdx].m_lDieX;

	return 0;
}

LONG CWT_BlkFunc2::GetPrepDieWftY(LONG lRowIdx, LONG lColIdx)
{
	if( lRowIdx>=0 && lRowIdx<MAPMAXROW2 && lColIdx>=0 && lColIdx<MAPMAXCOL2 )
		return m_stDiePhyPrep[lRowIdx][lColIdx].m_lDieY;

	return 0;

}

VOID CWT_BlkFunc2::SetReferWftInfo(ULONG ulIndex, LONG lX, LONG lY)
{
	m_stReferWftInfo[ulIndex].m_lPhyPosX = lX;
	m_stReferWftInfo[ulIndex].m_lPhyPosY = lY;
}

short CWT_BlkFunc2::GetRefDieStatus(LONG lRow, LONG lCol)
{
	if( lRow>=0 && lRow<MAXREFERWIDTH && lCol>=0 && lCol<MAXREFERWIDTH )
		return m_paaRefDiePos[lRow][lCol].m_sStatus;
	else
		return -1;
}

LONG CWT_BlkFunc2::GetRefDieWftX(LONG lRowIdx, LONG lColIdx)
{
	if( lRowIdx>=0 && lRowIdx<MAXREFERWIDTH && lColIdx>=0 && lColIdx<MAXREFERWIDTH )
		return m_paaRefDiePos[lRowIdx][lColIdx].m_WfX;
	else
		return -1;
}

LONG CWT_BlkFunc2::GetRefDieWftY(LONG lRowIdx, LONG lColIdx)
{
	if( lRowIdx>=0 && lRowIdx<MAXREFERWIDTH && lColIdx>=0 && lColIdx<MAXREFERWIDTH )
		return m_paaRefDiePos[lRowIdx][lColIdx].m_WfY;
	else
		return -1;
}

LONG CWT_BlkFunc2::GetRefDieValue(LONG lRowIdx, LONG lColIdx)
{
	if( lRowIdx>=0 && lRowIdx<MAXREFERWIDTH && lColIdx>=0 && lColIdx<MAXREFERWIDTH )
		return m_paaRefDiePos[lRowIdx][lColIdx].m_lReferValue;
	else
		return 0;
}

VOID CWT_BlkFunc2::SetReferDiePrepStatus(ULONG ulIndex, SHORT sStatus)
{
	ULONG ulRowIndex, ulColIndex;
	ulRowIndex = GetReferMapPosRow(ulIndex);
	ulColIndex = GetReferMapPosCol(ulIndex);
	m_stDiePhyPrep[ulRowIndex][ulColIndex].m_lDieX = GetReferDiePhyX(ulIndex);
	m_stDiePhyPrep[ulRowIndex][ulColIndex].m_lDieY = GetReferDiePhyY(ulIndex);
	m_stDiePhyPrep[ulRowIndex][ulColIndex].m_sState = sStatus;
}

VOID CWT_BlkFunc2::UpdatePrepDieStatus(ULONG ulRow, ULONG ulCol)
{
	if (m_stDiePhyPrep[ulRow][ulCol].m_sState == REGN_HPICK2)
		m_stDiePhyPrep[ulRow][ulCol].m_sState = REGN_EMPTY2;
	else
		m_stDiePhyPrep[ulRow][ulCol].m_sState = REGN_INIT2;
}

LONG CWT_BlkFunc2::MoveXYTo(LONG lPosX, LONG lPosY)
{
	return m_stnWaferTable->XY_SafeMoveTo(lPosX, lPosY);
}

BOOL CWT_BlkFunc2::Blk2GetIniOutputPath(CString *strPathBin, CString *strPathLog)
{
	BOOL bIfFileExist;
	CStdioFile cfCreePathFile;
	CString szLineData, szOutputLogFilePath, szOutputBinFilePath, szTempPath;
	INT		iIndex;

	//Get ouput file path
	bIfFileExist = cfCreePathFile.Open(_T("C:\\MapSorter\\Exe\\Cree.Ini"), CFile::modeRead|CFile::shareDenyNone|CFile::typeText);

	if ( !bIfFileExist )
	{
		return FALSE;
	}

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

	*strPathBin = szOutputBinFilePath;
	*strPathLog = szOutputLogFilePath;

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2GetIniOutputFileName(CString *strBinFullBin, CString *strBinFullLog, 
									  CString *strLocalBinFullBin, CString *strLocalBinFullLog)
{
	CString	strPathBin, strPathLog;
	CString	strFileNmBin, strFileNmLog;
	CString	strLocalFileNmBin, strLocalFileNmLog;
	int	iMchNo;

	CString strMachineID;
	m_stnWaferTable->GetRtnForMachineNo(&strMachineID);
	iMchNo = atoi(strMachineID);

	strLocalFileNmBin.Format("C:\\MapSorter\\OutputFiles\\%d.txt", GetEmptyDieGrade());
	strLocalFileNmLog.Format("C:\\MapSorter\\OutputFiles\\%04d%04d.txt", iMchNo, GetEmptyDieGrade());

	*strLocalBinFullBin = strLocalFileNmBin;
	*strLocalBinFullLog = strLocalFileNmLog;

	//Get Output Path
	if (Blk2GetIniOutputPath(&strPathBin, &strPathLog) != TRUE)
	{
		return FALSE;
	}

	strFileNmBin.Format("%s\\%d.txt", strPathBin, GetEmptyDieGrade());
	strFileNmLog.Format("%s\\%04d%04d.txt", strPathLog, iMchNo, GetEmptyDieGrade());

	*strBinFullBin = strFileNmBin;
	*strBinFullLog = strFileNmLog;

	return TRUE;
}

//Generate Log file when missing reference is detected during auto-bond 	(block2)
BOOL CWT_BlkFunc2::Blk2ExportRefCheckFailLog(LONG lX, LONG lY)
{
	CString		strRefCheckLog, strLocalRefCheckLog, strTemp;
	BOOL		bOutput=FALSE;
	CStdioFile	pFileLog;
	LONG		lUserCol=0, lUserRow=0;
	LONG		lRefCheckFail = 0;

	CString strMachineID;
	m_stnWaferTable->GetRtnForMachineNo(&strMachineID);
	//Get Output Path
	strLocalRefCheckLog = "C:\\MapSorter\\OutputFiles\\RefCheck.txt";

	CString	strPathBin, strPathLog;
	if( Blk2GetIniOutputPath(&strPathBin, &strPathLog) )
	{
		strRefCheckLog.Format("%s\\RefCheck.txt", strPathLog);
		bOutput = pFileLog.Open(strLocalRefCheckLog, CFile::modeCreate|CFile::modeReadWrite|CFile::typeText);
		if ( !bOutput )
		{
			SetErrorLogMessage("Fails to open Ref Check Log file");
			return FALSE;
		}
	}
	else
	{
		CString	strTemp;
		strTemp.Format("Fails to initial Ref Check log file path. Files will only be generated to local drive!");
		SetErrorLogMessage(strTemp);
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

	m_stnWaferTable->ConvertAsmToOrgUser((ULONG)lY, (ULONG)lX, lUserRow, lUserCol);

	strTemp.Format("%3d,%3d,%3d,%3d\n", lUserCol, lUserRow,	lX, lY);
	pFileLog.WriteString(strTemp);

	pFileLog.Close();

	if ( CopyFile(strLocalRefCheckLog, strRefCheckLog, FALSE) == FALSE)
	{
		SetErrorLogMessage("Fail to copy Ref Check Log file to the network path! Please Check!");
	}

	return TRUE;
}


BOOL CWT_BlkFunc2::ClearAlignRegion(LONG lRowIdx, LONG lColIdx)
{
	INT		i, j;
	ULONG	ulIndex;
	INT		iDig1, iDig2;
	BOOL	bFound;

	ulIndex = lRowIdx*MAXREFERWIDTH + lColIdx;
	for (i=GetReferMapPosRow(ulIndex)-GetBlkPitchRow()/2; i<=GetReferMapPosRow(ulIndex+MAXREFERWIDTH) + GetBlkPitchRow()/2; i++)
	{
		for (j=GetReferMapPosCol(ulIndex)-GetBlkPitchCol()/2; j<=GetReferMapPosCol(ulIndex+1) + GetBlkPitchCol()/2; j++)
		{ 
			if (i<0 || i>=MAPMAXROW2 || j<0 || j>=MAPMAXCOL2)
				continue;
			bFound = Blk2IsReferDie((ULONG)i,(ULONG)j,&iDig1,&iDig2);
			if (bFound==TRUE )
				continue;
			UpdatePrepDieStatus(i, j);
		}
	}
	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2RealignLocalRefDie(LONG lRow, LONG lCol, ULONG ulPkRow, ULONG ulPkCol)
{	// called after aligned, in sorting
	INT		i, j, iRowIdx, iColIdx;
	ULONG	ulIndex;
	BOOL	bFound;

	//Search RefDie
	bFound = FALSE;
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;

			ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

			if (GetReferMapPosRow(ulIndex) == lRow && GetReferMapPosCol(ulIndex) == lCol)
			{
				iRowIdx = i;
				iColIdx = j;
				bFound = TRUE;
				break;
			}
		}
		if (bFound==TRUE)
		{
			break;
		}
	}

	if (bFound)
	{
		if ((ULONG)lRow >= ulPkRow && (ULONG)lCol >= ulPkCol)
			ClearAlignRegion(iRowIdx-1, iColIdx-1);
		else if ((ULONG)lRow >= ulPkRow && (ULONG)lCol < ulPkCol)
			ClearAlignRegion(iRowIdx-1, iColIdx);
		else if ((ULONG)lRow < ulPkRow && (ULONG)lCol >= ulPkCol)
			ClearAlignRegion(iRowIdx, iColIdx-1);
		else if ((ULONG)lRow < ulPkRow && (ULONG)lCol < ulPkCol)
			ClearAlignRegion(iRowIdx, iColIdx);
	}

	return TRUE;
}

ULONG CWT_BlkFunc2::GetBlocksRowNum()
{
	return m_stFDCBasic.m_ulBlksRowNum;
}

ULONG CWT_BlkFunc2::GetBlocksColNum()
{
	return m_stFDCBasic.m_ulBlksColNum;
}

ULONG CWT_BlkFunc2::GetBlockIndex(ULONG ulRow, ULONG ulCol)
{
	BOOL bFind = FALSE;
	ULONG i, j, ulIndexUL, ulIndexLR, ulIndexCT;
	ULONG lULRow, lULCol, lLRRow, lLRCol;

	ulIndexCT = GetBlocksRowNum()/2*GetBlocksColNum() + GetBlocksColNum()/2;
	for(i=1; i<=GetBlocksRowNum(); i++)
	{
		for(j=1; j<=GetBlocksColNum(); j++)
		{
			ulIndexUL = (i-1)*MAXREFERWIDTH + (j-1);
			ulIndexLR = (i-0)*MAXREFERWIDTH + (j-0);

			//v4.10T7	//Klocwork
			ulIndexUL = min(ulIndexUL, 1599);
			ulIndexLR = min(ulIndexLR, 1599);

			lULRow = GetReferMapPosRow(ulIndexUL);
			lULCol = GetReferMapPosCol(ulIndexUL);
			lLRRow = GetReferMapPosRow(ulIndexLR);
			lLRCol = GetReferMapPosCol(ulIndexLR);
			if( (ulRow>=lULRow) && (ulRow<lLRRow) && (ulCol>=lULCol) && (ulCol<lLRCol) )
			{
				ulIndexCT = (i-1)*GetBlocksColNum() + j;
				bFind = TRUE;
				break;
			}
		}
		if( bFind )
		{
			break;
		}
	}

	return ulIndexCT;
}

BOOL CWT_BlkFunc2::GetBlockKnotPosn(ULONG ulRow, ULONG ulCol, LONG &lReferX, LONG &lReferY)
{
	int			i,j;
	ULONG		ulIndex;
	int			iRow,iCol;

	for(i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for(j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			ulIndex = min(ulIndex, MAXFDC2NUM-1);		//Klocwork

			iRow = GetReferMapPosRow(ulIndex);
			iCol = GetReferMapPosCol(ulIndex);
			if( iRow==ulRow && iCol==ulCol )
			{
				if( (GetReferInMap(ulIndex)==FALSE) || (GetReferInWfa(ulIndex)==0) )
				{
					return FALSE;
				}
				lReferX = GetReferDiePhyX(ulIndex);
				lReferY = GetReferDiePhyY(ulIndex);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CWT_BlkFunc2::UpdateBlockKnotPosn(ULONG ulRow, ULONG ulCol, LONG lReferX, LONG lReferY)
{
	int			i,j;
	ULONG		ulIndex;
	int			iRow,iCol;

	for(i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for(j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			ulIndex = min(ulIndex, MAXFDC2NUM-1);		//Klocwork

			iRow = GetReferMapPosRow(ulIndex);
			iCol = GetReferMapPosCol(ulIndex);
			if( iRow==ulRow && iCol==ulCol )
			{
				if( (GetReferInMap(ulIndex)==FALSE) || (GetReferInWfa(ulIndex)==0) )
				{
					return FALSE;
				}
				SetReferWftInfo(ulIndex, lReferX, lReferY);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CWT_BlkFunc2::VerifyBlock4Corners(ULONG ulInRow, ULONG ulInCol, LONG lPrDelay, CString &szErrorCode)
{
	BOOL bFind = FALSE;
	ULONG i, j, ulIndexUL, ulIndexLR, ulIndexCT;
	ULONG ulULRow = 0, ulULCol = 0, ulLRRow = 0, ulLRCol = 0;
	CString szMoveMsg, szAppend;
	CString szMsg;

	ulIndexCT = GetBlocksRowNum()/2*GetBlocksColNum() + GetBlocksColNum()/2;
	for(i=1; i<=GetBlocksRowNum(); i++)
	{
		for(j=1; j<=GetBlocksColNum(); j++)
		{
			ulIndexUL = (i-1)*MAXREFERWIDTH + (j-1);
			ulIndexLR = (i-0)*MAXREFERWIDTH + (j-0);

			ulIndexUL = min(ulIndexUL, 1599);
			ulIndexLR = min(ulIndexLR, 1599);

			ulULRow = GetReferMapPosRow(ulIndexUL);
			ulULCol = GetReferMapPosCol(ulIndexUL);
			ulLRRow = GetReferMapPosRow(ulIndexLR);
			ulLRCol = GetReferMapPosCol(ulIndexLR);
			if( (ulInRow>=ulULRow) && (ulInRow<ulLRRow) && (ulInCol>=ulULCol) && (ulInCol<ulLRCol) )
			{
				ulIndexCT = (i-1)*GetBlocksColNum() + j;
				bFind = TRUE;
				break;
			}
		}
		if( bFind )
		{
			break;
		}
	}

	SetFDCRegionLog("\n");
	szMoveMsg.Format("OCR for Die (%ld,%ld) of block %lu corner (%ld,%ld) (%ld,%ld)",
		ulInRow, ulInCol, ulIndexCT, ulULRow, ulULCol, ulLRRow, ulLRCol);
	SetFDCRegionLog(szMoveMsg);
	if( bFind==FALSE )
	{
		return TRUE;
	}

	ULONG ulRow, ulCol;
	LONG lReferX, lReferY;
	ULONG ulReferDieTotal = 0, ulReferDiePass = 0;
	for(i=0; i<4; i++)
	{
		switch( i )
		{
		case 0:
			ulRow = ulULRow;
			ulCol = ulULCol;
			break;
		case 1:
			ulRow = ulULRow;
			ulCol = ulLRCol;
			break;
		case 2:
			ulRow = ulLRRow;
			ulCol = ulULCol;
			break;
		case 3:
		default:
			ulRow = ulLRRow;
			ulCol = ulLRCol;
			break;
		}

		if( GetBlockKnotPosn(ulRow, ulCol, lReferX, lReferY) )
		{
			m_stnWaferTable->m_WaferMapWrapper.SetCurrentPosition(ulRow, ulCol);
			szMoveMsg.Format("OCR map (%ld,%ld) wft (%ld,%ld) ", ulRow, ulCol, lReferX, lReferY);
			LONG lHmiRow, lHmiCol;
			m_stnWaferTable->ConvertAsmToHmiUser(ulRow, ulCol, lHmiRow, lHmiCol);

			if( MoveXYTo(lReferX, lReferY) )
			{
				ulReferDieTotal++;
				Sleep(lPrDelay);
				LONG lOffsetX, lOffsetY;
				LONG lDig1 = 0, lDig2 = 0;
				Blk2SearchNmlReferDie(FALSE, 1, lOffsetX, lOffsetY);
				BOOL bCheck = Blk2SearchReferDie(&lReferX, &lReferY, &lDig1, &lDig2, TRUE, FALSE);
				if ( bCheck == FALSE )
				{
					Sleep(200);
					bCheck = Blk2SearchReferDie(&lReferX, &lReferY, &lDig1, &lDig2, TRUE, FALSE);
				}
				szAppend.Format("PR posn %ld,%ld, result %d",
					lReferX, lReferY, bCheck);
				SetFDCRegionLog(szMoveMsg+szAppend);
				if( bCheck==FALSE )
				{
					szMsg.Format("Refer die %ld,%ld (%lu,%lu) Corner %d, PR check fail!", 
						lHmiRow, lHmiCol, ulRow, ulCol, i+1);
					SetFDCRegionLog(szMsg);
					SetHmiMessage(szMsg);
					szErrorCode = szErrorCode + szMsg + "\n";
				//	return FALSE;
				}
				else
				{
					ulReferDiePass++;
					Sleep(lPrDelay);
					UpdateBlockKnotPosn(ulRow, ulCol, lReferX, lReferY);
				}
			}
			else
			{
				szMsg.Format("Refer die %ld,%ld (%lu,%lu) Corner %d, motion error!", 
					lHmiRow, lHmiCol, ulRow, ulCol, i+1);
				SetFDCRegionLog(szMsg);
				szErrorCode = szErrorCode + szMsg + "\n";
				return FALSE;
			}
		}
	}

	if( ulReferDieTotal>0 && m_l4ReferCheckScore>0 )
	{
		if( (ulReferDiePass*100)>=m_l4ReferCheckScore*ulReferDieTotal )
			return TRUE;
		else
			return FALSE;
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::RemoveMissingReferDieInMap()
{
	INT			i, j;
	ULONG		ulIndex = 0, ulRow = 0, ulCol = 0;
	LONG		lUserCol=0, lUserRow=0;
	UCHAR		ucNullBin = m_stnWaferTable->m_WaferMapWrapper.GetNullBin();
	CString	szLog;

	if( GetBlkAligned()==FALSE )
	{
		return FALSE;
	}

	//Search RefDie
	for(i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for(j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;

			ulIndex = min(ulIndex, MAXFDC2NUM-1);
			if( GetReferInMap(ulIndex)==FALSE )
				continue;

			if( GetReferInWfa(ulIndex)!=0 )
				continue;

			ulRow = GetReferMapPosRow(ulIndex);
			ulCol = GetReferMapPosCol(ulIndex);
			lUserRow = GetReferSrcMapRow(ulIndex);
			lUserCol = GetReferSrcMapCol(ulIndex);

			m_stnWaferTable->m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, FALSE);
			m_stnWaferTable->m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucNullBin);
			szLog.Format("Refer die (%ld,%ld) map(%ld,%ld), set to null refer", 
				ulRow, ulCol, lUserRow, lUserCol);
			SetFDCRegionLog(szLog);
		}
	}

	return TRUE;
}

BOOL CWT_BlkFunc2::Blk2SearchReferDie(LONG *lPosX, LONG *lPosY, LONG *lDig1, LONG *lDig2, BOOL bAlign, BOOL bFovSrch)
{
	LONG	lOffsetX, lOffsetY;
	BOOL	bReferRtn = FALSE;

	*lDig1 = 0;
	*lDig2 = 0;
	LONG	lPrID = 1;
	CString szMsg;
	for(lPrID=1; lPrID<=m_lReferDieLeartNum; lPrID++)
	{
		lOffsetX = lOffsetY = 0;
		bReferRtn = FALSE;

		if( Blk2IsSrchCharDie() )
		{
			if( m_bDigitalF )
			{
				bReferRtn = Blk2SearchNmlReferDie(bFovSrch, lPrID, lOffsetX, lOffsetY);
				if( bReferRtn )
				{
					LONG	lFPosX, lFPosY;
					lFPosX = *lPosX + lOffsetX - GetDiePitchHX();
					lFPosY = *lPosY + lOffsetY - GetDiePitchHY();
					if (CheckWaferLimit(lFPosX,lFPosY) == FALSE)
					{
						return FALSE;
					}
					MoveXYTo(lFPosX,lFPosY);
					Sleep(m_lPrSrchDelay);
					*lPosX = lFPosX;
					*lPosY = lFPosY;
				}
			}

			bReferRtn = Blk2SearchDgtReferDie(lOffsetX, lOffsetY, lDig1, lDig2);
		}
		else
		{
			bReferRtn = Blk2SearchNmlReferDie(bFovSrch, lPrID, lOffsetX, lOffsetY);
		}

		*lPosX = *lPosX + lOffsetX;
		*lPosY = *lPosY + lOffsetY;

		if (CheckWaferLimit(*lPosX,*lPosY) == FALSE)
		{
			return FALSE;
		}

		if ( bAlign )
		{
			MoveXYTo(*lPosX, *lPosY);
			Sleep(m_lPrSrchDelay);
		}

		szMsg.Format("Srch refer FOV %d result %d at pr id %ld", bFovSrch, bReferRtn, lPrID);
		DisplayDebugString(szMsg);
		if( bReferRtn )
		{
			break;
		}
	}

	return bReferRtn;
}


BOOL CWT_BlkFunc2::VirtualAlignBlockWafer()
{
	// from realign block wafer
	Blk2SetBondingLastDie();

	LONG	ulIndex=0, i=0, j=0;	//Klocwork	//v4.24T11
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	// below from found all refer die
    for (i=0; i<MAXREFERWIDTH; i++)
	{
		for (j=0; j<MAXREFERWIDTH; j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			if( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}

			LONG lTLRow = GetReferMapPosRow(ulIndex);
			LONG lTLCol = GetReferMapPosCol(ulIndex);
			if( GetReferInWfa(ulIndex) == 1 )
			{
				m_stnWaferTable->m_pWaferMapManager->AddBoundary(lTLRow, lTLRow+2, lTLCol, lTLCol+2);

				SetMapAlgorithmPrm("Row", lTLRow);
				SetMapAlgorithmPrm("Col", lTLCol);
				m_stnWaferTable->SetMapPhyPosn(lTLRow, lTLCol, GetReferDiePhyX(ulIndex), GetReferDiePhyY(ulIndex));
				SetMapAlgorithmPrm("Align", Regn_Align);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);

				SetMapAlgorithmPrm("Row", i);
				SetMapAlgorithmPrm("Col", j);
				SetMapAlgorithmPrm("Align", REGN_ATREF2);
				m_stnWaferTable->UpdateMapDie(1, 1, 1, WAF_CDieSelectionAlgorithm::ALIGN);
			}
			else
			{
				m_stnWaferTable->m_WaferMapWrapper.SetReferenceDie(lTLRow, lTLCol, FALSE);
				m_stnWaferTable->m_WaferMapWrapper.SetDieState(lTLRow, lTLCol, WT_MAP_DIESTATE_DIMREFER);
			}
		}
	}

	Blk2SetBondingLastDie();

	return TRUE;
}

BOOL CWT_BlkFunc2::GetOtherCornerRefDie(DieMapPos2 SrcDie, DieMapPos2 NearRefDie, DieMapPos2 &RefTL, DieMapPos2 &RefTR, DieMapPos2 &RefBL, DieMapPos2 &RefBR)
{
	INT		i,j;
	ULONG	ulIndex, ulCheckRow, ulCheckCol;
	ULONG	ulUpRow, ulDnRow, ulLtCol, ulRtCol;
	BOOL	bTLPass = FALSE, bTRPass = FALSE, bBLPass = FALSE, bBRPass = FALSE;

	if( labs(SrcDie.Row-NearRefDie.Row)>GetBlkPitchRow() || 
		labs(SrcDie.Col-NearRefDie.Col)>GetBlkPitchCol() )
	{
		return FALSE;
	}
	if (GetMapReferNum() < 4)
	{
		return FALSE;
	}
	if( GetBlkMinRowIdx()==GetBlkMaxRowIdx() ||
		GetBlkMinColIdx()==GetBlkMaxColIdx() )
	{
		return FALSE;
	}

	if( NearRefDie.Row<SrcDie.Row )
	{
		ulUpRow = NearRefDie.Row;
		ulDnRow = NearRefDie.Row + GetBlkPitchRow();
	}
	else if( NearRefDie.Row==SrcDie.Row )
	{
		ulUpRow = NearRefDie.Row;
		ulDnRow = NearRefDie.Row;
	}
	else
	{
		ulUpRow = NearRefDie.Row - GetBlkPitchRow();
		ulDnRow = NearRefDie.Row;
	}

	if( NearRefDie.Col<SrcDie.Col )
	{
		ulLtCol = NearRefDie.Col;
		ulRtCol = NearRefDie.Col + GetBlkPitchCol();
	}
	else if( NearRefDie.Col==SrcDie.Col )
	{
		ulLtCol = NearRefDie.Col;
		ulRtCol = NearRefDie.Col;
	}
	else
	{
		ulLtCol = NearRefDie.Col - GetBlkPitchCol();
		ulRtCol = NearRefDie.Col;
	}

	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.24T11

			if ( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}
			if (GetReferInWfa(ulIndex) == 0)
			{
				continue;
			}
			ulCheckRow = GetReferMapPosRow(ulIndex);
			ulCheckCol = GetReferMapPosCol(ulIndex);

			if( ulUpRow==ulCheckRow && ulLtCol==ulCheckCol )
			{
				bTLPass = TRUE;
			}
			if( ulUpRow==ulCheckRow && ulRtCol==ulCheckCol )
			{
				bTRPass = TRUE;
			}
			if( ulDnRow==ulCheckRow && ulLtCol==ulCheckCol )
			{
				bBLPass = TRUE;
			}
			if( ulDnRow==ulCheckRow && ulRtCol==ulCheckCol )
			{
				bBRPass = TRUE;
			}
		}
	}

	if( bTLPass )
	{
		RefTL.Row = ulUpRow;
		RefTL.Col = ulLtCol;
	}
	if( bTRPass )
	{
		RefTR.Row = ulUpRow;
		RefTR.Col = ulRtCol;
	}
	if( bBLPass )
	{
		RefBL.Row = ulDnRow;
		RefBL.Col = ulLtCol;
	}
	if( bBRPass )
	{
		RefBR.Row = ulDnRow;
		RefBR.Col = ulRtCol;
	}

	return TRUE;
}
