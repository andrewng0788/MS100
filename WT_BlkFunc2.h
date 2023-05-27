#ifndef WT_BLKFUNC2_H
#define WT_BLKFUNC2_H

//================================================================
// #include and #define
//================================================================

#define MAXREFERWIDTH		40
#define	REFER_BASE			20
#define	MAXFDC2NUM			(MAXREFERWIDTH*MAXREFERWIDTH)

#define REGN_INIT2				0
#define REGN_ATMAP				1
#define REGN_WPICK				2
#define REGN_ALIGN2				3
#define REGN_HPICK2				4
#define REGN_EMPTY2				5
#define REGN_ATREF2				6

#define REGN_IS_EMPTYDIE		0
#define REGN_IS_GOODDIE			1
#define REGN_IS_HOMEDIE			2
#define REGN_IS_REFDIE			3
#define REGN_IS_HITLIMIT		4
#define REGN_IS_DIGITDIE		5
#define REGN_IS_MOTORERROR		6

#define LIGHTTOWER_RED			1
#define LIGHTTOWER_YELLOW		2
#define LIGHTTOWER_GREEN		3


// ANTI CLOCKWISE
#define	BLK_REFER_UP			0
#define	BLK_REFER_LEFT			1
#define	BLK_REFER_DOWN			2
#define	BLK_REFER_RIGHT			3
#define	BLK_REFER_UPRIGHT		4
#define	BLK_REFER_UPLEFT		5
#define	BLK_REFER_DOWNLEFT		6
#define	BLK_REFER_DOWNRIGHT		7

//================================================================
// struct, enum definition
//================================================================

typedef struct
{
	LONG	m_lRowIdx;
	LONG	m_lColIdx;
} DieMapRowColIdx;

typedef struct
{
	LONG	Row;
	LONG	Col;
}DieMapPos2;

typedef struct
{
	long	x;
	long	y;
}DiePhyPos2;

typedef struct
{
	long	m_WfX;
	long	m_WfY;
	long	m_lReferValue;
	short	m_sStatus;
}RefDiePhyPos2;

typedef struct
{
	long	m_lDieX;
	long	m_lDieY;
	short	m_sState;
}PrepDiePhyPos2;

typedef struct
{
	DieMapPos2	m_MapPos;
	LONG		m_lFaceValue;
	LONG		m_lPhyPosX;
	LONG		m_lPhyPosY;
	BOOL		m_bInMap;
	char		m_cInWaf;
}FDC_Data2;

typedef struct
{
	// wafer alignment result
	BOOL		m_bHaveAligned;
	LONG		m_lHomeMapRow;
	LONG		m_lHomeMapCol;
	LONG		m_stHomeDiePhyX;
	LONG		m_stHomeDiePhyY;
	LONG		m_stHomeDiePhyT;
	// alignment block pitch of wft and map
	LONG		m_lFDCPitchX_X;
	LONG		m_lFDCPitchX_Y;
	LONG		m_lFDCPitchY_Y;
	LONG		m_lFDCPitchY_X;
	LONG		m_ulFDCPitchRow;
	LONG		m_ulFDCPitchCol;
	// refer die distribution and num.
	ULONG		m_ulReferDieNum;
	ULONG		m_ulMaxRowIdx;
	ULONG		m_ulMaxColIdx;
	// num of blocks in row/column direction
	ULONG		m_ulBlksRowNum;
	ULONG		m_ulBlksColNum;
}FDC_Fundament2;

//================================================================
// Class definition
//================================================================
//class CMS896AStn;

class CWaferTable;

class CWT_BlkFunc2 : public CObject
{
public:
	// ===========================================================
	// Constructor and destructor
	CWT_BlkFunc2();
	~CWT_BlkFunc2();

	// ===========================================================
	// Get Functions
	void SetWaferTable(CWaferTable* stnWaferTable);

	//Block function for PR
	BOOL Blk2SearchReferDieInFOV(LONG *lPosX, LONG *lPosY, LONG *lDig1, LONG *lDig2, BOOL bAlign=TRUE);
	BOOL Blk2SearchReferOnlyInFOV(LONG *lPosX, LONG *lPosY, BOOL bAlign=TRUE);
	BOOL Blk2SearchReferDieAtCurr(LONG *lPosX, LONG *lPosY);
	BOOL Blk2SearchNmlReferDie(BOOL bFov, LONG lPrID, LONG &lPrOffsetX, LONG &lPrOffsetY);
	BOOL Blk2SearchDgtReferDie(LONG &lPrOffsetX, LONG &lPrOffsetY, LONG *lDig1, LONG *lDig2);
	BOOL Blk2SearchNormalDie(LONG lSrchWin, LONG *lPosX, LONG *lPosY, BOOL bAlign=TRUE);
	BOOL Blk2SearchReferDie(LONG *lPosX, LONG *lPosY, LONG *lDig1, LONG *lDig2, BOOL bAlign, BOOL bFovSrch);

	BOOL Blk2SetCurDiePhyPos(LONG lRow, LONG lCol, LONG lInX, LONG lInY, LONG lStatus);

	LONG Blk2GetNextDiePosn(DieMapPos2 MapPos, DiePhyPos2 *PhyPos);
	LONG Blk2GetNearRefDie(DieMapPos2 SrcDie, DieMapPos2 *RDieMap, DiePhyPos2 *RDiePhy);
	LONG Blk2GetNextDiePosnAndT(DieMapPos2 MapPos, DiePhyPos2 *PhyPos, LONG *lValueT);
	BOOL Blk2GetNextDiePhyPos(LONG lRow, LONG lCol, LONG lLastRow, LONG lLastCol, LONG &lOutX, LONG &lOutY, BOOL bLog=FALSE);

	ULONG GetBlocksRowNum();
	ULONG GetBlocksColNum();
	ULONG GetBlockIndex(ULONG ulRow, ULONG ulCol);

	BOOL GetBlockKnotPosn(ULONG ulRow, ULONG ulCol, LONG &lReferX, LONG &lReferY);
	BOOL UpdateBlockKnotPosn(ULONG ulRow, ULONG ulCol, LONG lReferX, LONG lReferY);
	BOOL VerifyBlock4Corners(ULONG ulRow, ULONG ulCol, LONG lPrDelay, CString &szErrorCode);
	VOID SetFDCRegionLog(CONST CString &szMsg);
	VOID DisplayDebugString(CONST CString szMsg);

// prescan relative code	B
	BOOL Blk2GetRefDiePosn(ULONG ulIndex, LONG &lMapRow, LONG &lMapCol, LONG &lTableX, LONG &lTableY);
	ULONG GetTotalRefPoints();
// prescan relative code	E
	LONG GetHomeTheta();

	//New method to calculate next die physical position
	BOOL Blk2GetStartDie(LONG *lRow, LONG *lCol, LONG *lX, LONG *lY);

	BOOL Blk2IsReferDie(ULONG ulRow, ULONG ulCol, INT *iDigit1, INT *iDigit2);

	BOOL Blk2RealignLocalRefDie(LONG lRow, LONG lCol, ULONG ulPkRow, ULONG ulPkCol);
	BOOL Blk2ReVerifyRefDiePos();

	// alignment kernel part for wafer table UI
	VOID Blk2SetSearch(LONG RefGrid, LONG JumpCtr, LONG JumpEdge);
	VOID Blk2SetEmptyGradeMinCount(LONG lEmptyGradeMinCount);
	VOID Blk2SetAlignment(BOOL bEnableEdge, LONG lPassPercent, BOOL bDigitalF);
	VOID Blk2SetReferMode(BOOL bCharRefMode, LONG lTotalReferNum=3);
	VOID Blk2SetBlock4ReferCheckScore(LONG lScore);
	BOOL Blk2FoundAllReferDiePos();
	BOOL Blk2FindAlignAllReferHalfWafer();
	LONG Blk2ManAlignHalfWafer(DiePhyPos2 &CurTbl);
	BOOL Blk2RealignRefDiePos(BOOL bEdge = FALSE);
	BOOL RemoveMissingReferDieInMap();

	BOOL Blk2SetBondingLastDie();

	//Utility Functions
	BOOL Blk2UpdateRefDieOffset(LONG lOffsetX, LONG lOffsetY, LONG lRow, LONG lCol);			//v3.34

	BOOL Blk2GetIniOutputFileName(CString *strBinFullBin, CString *strBinFullLog,
							CString *strLocalBinFullBin, CString *strLocalBinFullLog);
	//Check all reference die once again at wafer end	
	BOOL Blk2GetIniOutputPath(CString *strPathBin, CString *strPathLog);
	//Generate Log file when missing reference is detected during auto-bond 	(block2)
	BOOL Blk2ExportRefCheckFailLog(LONG lX, LONG lY);

	BOOL GetOtherCornerRefDie(DieMapPos2 SrcDie, DieMapPos2 NearRefDie, DieMapPos2 &RefTL, DieMapPos2 &RefTR, DieMapPos2 &RefBL, DieMapPos2 &RefBR);
	BOOL VirtualAlignBlockWafer();

protected:
	LONG Blk2GetNormalDieStatus(LONG *lCurX, LONG *lCurY);
	LONG NextReferDiePosition(DiePhyPos2 PhyInput, UCHAR ucDirection, DiePhyPos2 *PhyOutput);
	LONG NextReferDiePositionValue(DiePhyPos2 PhyInput, UCHAR ucDirection, DiePhyPos2 *PhyOutput, LONG *lCharValue);

	LONG Blk2InitBlockFunc();
	BOOL Blk2MergeAlignMap();
	BOOL Blk2MergeAlignRhombusMap();
	BOOL Blk2MergeAlignMapLessRefer();
	LONG Blk2IndexCrossRefDiePos(DiePhyPos2 CurPhy, DieMapRowColIdx *pDieStack, int *pStackPtr);
	BOOL Blk2DrawMap();
	BOOL Blk2MatchAlignCharMap();
	LONG Blk2ManAlignWafer(DiePhyPos2 &CurTbl);
	LONG Blk2ManualLocateReferDie(ULONG ulIndex, LONG &lX, LONG &lY);
	BOOL Blk2GetNearDieMapPos(DieMapPos2 SrcPos, DieMapPos2 *TgtPos);

	BOOL Blk2SetStartDie(LONG lX, LONG lY, LONG lT);
	LONG CheckEmptyDieGrade();
	BOOL Blk2CheckCOR();
	BOOL SetMapAlgorithm();
	BOOL PrintFDC2();
	BOOL Blk2GetEdgePosition();
	BOOL Blk2HaveRefdieOn4Dir(DieMapPos2 iRefCtr, BOOL *iUp, BOOL *iDn, BOOL *iLt, BOOL *iRt);
	LONG Blk2IndexToEdgeDie(DieMapPos2 SrcDie, DiePhyPos2 *CurPhyPos, LONG lDir);

	LONG Blk2IndexToNormalDie(DieMapPos2 SrcDie, DieMapPos2 TgtDie, DiePhyPos2 *CurPhyPos, INT iMissCount);
	BOOL ClearAlignRegion(LONG lRow, LONG lCol);
	DOUBLE GetDistIn2Points(DOUBLE P1X, DOUBLE P1Y, DOUBLE P2X, DOUBLE P2Y);
	BOOL CheckRelativePstnInFDC();
	BOOL MatchRefDigitFromMap(ULONG ulIndex, LONG lDig1, LONG lDig2);

	BOOL GetUserMapCenterDie(DieMapPos2 *CtrDie, DiePhyPos2 *PhyPos, LONG *lDigital);
	BOOL GetRealMapCenterDie(DieMapPos2 *CtrDie);

protected:
	CWaferTable* m_stnWaferTable;

	VOID SetMapAlgorithmPrm(LPCTSTR strPrm, CONST LONG lPrm);
	VOID SetLogCtmMessage(CONST CString& szMsg);
	VOID SetErrorLogMessage(CONST CString &szMsg);
	VOID SetDisplayErrorMessage(CONST CString szMsg);
	LONG SetHmiMessage(const CString& szText,	const CString& szTitle = _T(""),
							LONG lType = glHMI_MBX_CLOSE,
							LONG lAlignment = glHMI_ALIGN_CENTER,
							LONG lTimeout = 36000000,
							LONG lMsgMode = glHMI_MSG_MODAL,
							LONG* plMsgID = NULL,
							LONG lWidth = -1,
							LONG lHeight = -1,
							CString* pszBitmapFile = NULL,
							CString* pszCustomText1 = NULL,
							CString* pszCustomText2 = NULL,
							CString* pszCustomText3 = NULL);

	LONG SetHmiMessageEx(const CString& szText,	const CString& szTitle = _T(""),
							LONG lType = glHMI_MBX_CLOSE,
							LONG lAlignment = glHMI_ALIGN_CENTER,
							LONG lTimeout = 36000000,
							LONG lMsgMode = glHMI_MSG_MODAL,
							LONG* plMsgID = NULL,
							LONG lWidth = -1,
							LONG lHeight = -1,
							CString* pszBitmapFile = NULL,
							CString* pszCustomText1 = NULL,
							CString* pszCustomText2 = NULL,
							CString* pszCustomText3 = NULL);

	BOOL IsToStopAlign();

	VOID Blk2GetWTPosition(LONG *lX, LONG *lY, LONG *lT);
	LONG MoveXYTo(LONG lPosX, LONG lPosY);
	BOOL CheckWaferLimit(LONG lX, LONG lY);
	VOID SetJoystick(BOOL bOn);

	BOOL Blk2IsSrchCharDie();

	BOOL GetBlkAligned();
	LONG GetBlkPitchRow();
	LONG GetBlkPitchCol();
	LONG GetBlkPitchHX();
	LONG GetBlkPitchHY();
	LONG GetBlkPitchVX();
	LONG GetBlkPitchVY();
	LONG GetDiePitchHX();
	LONG GetDiePitchHY();
	LONG GetDiePitchVX();
	LONG GetDiePitchVY();
	LONG GetBlkMinRowIdx();
	LONG GetBlkMinColIdx();
	LONG GetBlkMaxRowIdx();
	LONG GetBlkMaxColIdx();
	ULONG GetMapReferNum();
	BOOL IsAutoManualAlign();

	LONG GetReferGrid();
	LONG GetEmptyDieGrade();

	VOID SetReferWftInfo(ULONG ulIndex, LONG lX, LONG lY);
	LONG GetReferDiePhyX(ULONG ulIndex);
	LONG GetReferDiePhyY(ULONG ulIndex);
	LONG GetReferSrcMapRow(ULONG ulIndex);
	LONG GetReferSrcMapCol(ULONG ulIndex);
	LONG GetReferMapPosRow(ULONG ulIndex);
	LONG GetReferMapPosCol(ULONG ulIndex);
	LONG GetReferMapPosValue(ULONG ulIndex);
	BOOL GetReferInMap(ULONG ulIndex);
	char GetReferInWfa(ULONG ulIndex);

	short GetRefDieStatus(LONG lRowIdx, LONG lColIdx);
	LONG  GetRefDieWftX(LONG lRowIdx, LONG lColIdx);
	LONG  GetRefDieWftY(LONG lRowIdx, LONG lColIdx);
	LONG  GetRefDieValue(LONG lRowIdx, LONG lColIdx);

	VOID  SetReferDiePrepStatus(ULONG ulIndex, SHORT sStatus);
	short GetPrepDieStatus(LONG lRowIdx, LONG lColIdx);
	LONG  GetPrepDieWftX(LONG lRowIdx, LONG lColIdx);
	LONG  GetPrepDieWftY(LONG lRowIdx, LONG lColIdx);
	VOID  UpdatePrepDieStatus(ULONG ulRow, ULONG ulCol);

private:
	BOOL	m_bSrchCharDie;
	BOOL	m_bBlk2MnEdgeAlign;
	BOOL	m_bDigitalF;
	BOOL	m_bRhombus4RefDie;

	LONG	m_l4ReferCheckScore;
	LONG	m_lReferDieLeartNum;
	LONG	m_lBlk2SrchRefGrid;
	LONG	m_lBlk2MaxJumpCtr;
	LONG	m_lBlk2MaxJumpEdge;
	ULONG	m_ulBlk2EmptyGradeMinCount;
	LONG	m_lBlk2MnPassPercent;
	ULONG	m_ulBlk2WaferReferNum;
	
	FDC_Fundament2		m_stFDCBasic;
	// alignment result include map and wft information
	FDC_Data2			m_stReferWftInfo[MAXFDC2NUM];
	// alignment refer die position and status and OCR value, useless after alignment complete
	RefDiePhyPos2		m_paaRefDiePos[MAXREFERWIDTH][MAXREFERWIDTH];
	// for run time sorting purpose mainly.
	PrepDiePhyPos2		m_stDiePhyPrep[MAPMAXROW2][MAPMAXCOL2];

	LONG	m_lAlignMatchLvl;

	LONG	m_lPrSrchDelay;
};
#endif  // WT_BLKFUNC2_H

//================================================================
// End of file WT_BlkFunc2.h
//================================================================
