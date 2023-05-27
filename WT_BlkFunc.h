#ifndef WT_BLKFUNC_H
#define WT_BLKFUNC_H

//================================================================
// #include and #define
//================================================================

#define MaxRow			600
#define MaxCol			600
#define MaxRefWidth		100
#define	MaxNumOfFDC		(MaxRefWidth*MaxRefWidth)
#define MaxNumOfStep	(MaxRow)
#define MaxEmptyCheck	100

#define Regn_Init	0
#define Regn_AtMap	1
#define Regn_WPick	2
#define Regn_Align	3
#define Regn_HPick	4
#define Regn_Empty	5
#define Regn_AtRef	6
#define Regn_Inked	7

#define Regn_IsEmptyDie	0
#define Regn_IsGoodDie	1
#define Regn_IsHomeDie	2
#define Regn_IsRefDie	3
#define Regn_IsHitLimit	4

#define REGN_UP		1
#define REGN_DOWN	2
#define REGN_LEFT	3
#define REGN_RIGHT	4

//================================================================
// struct, enum definition
//================================================================
typedef struct
{
	LONG	Row;
	LONG	Col;
	LONG	Status;
}DieMapPos;

typedef struct
{
	long	x;
	long	y;
	long	t;
	short	Status;
}DiePhyPos;

typedef struct
{
	DieMapPos	SrcMap;
	DieMapPos	MapPos;
	DiePhyPos	PhyPos;
	char		InMap;
	char		InWaf;
}FDC_Data;

typedef struct
{
	DieMapPos	MapPos;
	DiePhyPos	PhyPos;
}FDC_Home;

typedef struct
{
	char		HaveAligned;
	FDC_Home	HomeDie;
	DiePhyPos	HPitchDie;
	DiePhyPos	VPitchDie;
	DiePhyPos	HPitchFDC;
	DiePhyPos	VPitchFDC;
}FDC_Fundament;

typedef struct
{
	LONG		sNum;
	DieMapPos	MinRC;
	DieMapPos	MaxRC;
	DieMapPos	RefDist;				//Block Height&Width
	LONG		arrJStep[MaxNumOfStep];
	FDC_Data	RefDie[MaxNumOfFDC];	//RefDie Detail
}FDC_Info;

typedef struct
{
	ULONG		ulNumOfDie;
	DieMapPos	MapPos[MaxEmptyCheck];
}FDC_SDSOPUT;



//================================================================
// Class definition
//================================================================
//class CMS896AStn;

class CWaferTable;
class CWT_BlkFunc : public CObject
{
public:
	// ===========================================================
	// Constructor and destructor
	CWT_BlkFunc();
	~CWT_BlkFunc();
	
	// ===========================================================
	// Get Functions
	void SetWaferTable(CWaferTable* stnWaferTable);
	void SetWaferAlignment(CHAR cAligned, LONG lMapRow, LONG lMapCol, LONG lPhyX, LONG lPhyY);
	BOOL SearchCurrDie(LONG lSrchWin, LONG lPrId, LONG *lPosX, LONG *lPosY);
	BOOL SearchAroundDie(LONG lDir, LONG *lPosX, LONG *lPosY);
	VOID GetWTPosition(LONG *lX, LONG *lY, LONG *lT);
	BOOL FoundAllRefDiePos();
	LONG GetNextRefDieXY(LONG lRow, LONG lCol, LONG *lX, LONG *lY);
	LONG GetNextRefDieXY(DiePhyPos PhyInput, LONG lRow, LONG lCol, DiePhyPos *PhyOutput);
	LONG ResetJStep(LONG sJDist);
	LONG InitBlockFunc();
	LONG GetNormalDieStatus(LONG *lCurX, LONG *lCurY);
	BOOL MergeAlignMap(DiePhyPos **pRefArray, DieMapPos MinRC, DieMapPos MaxRC);
	LONG IndexCrossRefDiePos(DiePhyPos **pRefDie, DieMapPos *pStack, int *pStackPtr);
	BOOL DrawBlockMap();
	LONG ManAlignWafer(DiePhyPos CurTbl);
	BOOL SetCurDiePhyPos(DieMapPos MapPos, DiePhyPos PhyPos, BOOL bUpdMap=TRUE);
	LONG GetNextDiePhyPos(DieMapPos MapPos, DiePhyPos *PhyPos);
	LONG GetNearRefDie(DieMapPos SrcDie, DieMapPos *RDieMap, DiePhyPos *RDiePhy);
	BOOL GetNearDieMapPos(DieMapPos SrcPos, DieMapPos *TgtPos);
	LONG GetNextDiePitch(DiePhyPos *HoriPitch, DiePhyPos *VertPitch);
	BOOL GetStartDie(LONG *lRow, LONG *lCol, LONG *lX, LONG *lY);
	BOOL SetStartDie(DiePhyPos HomePhy);
	BOOL GetRefDieArray();
	BOOL SetRefDieArray(LONG lDistRow, LONG lDistCol);
	LONG CheckDieInSpecialGrade(LONG iGrade);

	BOOL GotoReferenceKey(LONG lSrcRow, LONG lSrcCol, LONG lTgtRow, LONG lTgtCol);
	BOOL SetBlockParameter( LONG RefGrid, LONG HomeGrid, LONG JumpCtr, LONG JumpEdge,
							LONG NoDieGrade, LONG AlignReset, LONG lMinForPick);
	LONG IndexToNormalDie(DieMapPos SrcDie, DieMapPos TgtDie, DiePhyPos *CurPhyPos, INT iMissCount);
	LONG IndexToCheckKeyDie(DieMapPos SrcDie, DieMapPos TgtDie, DiePhyPos *CurPhyPos, INT iMissCount);
	BOOL GotoTargetDie(LONG lSrcRow, LONG lSrcCol);
	BOOL PrintFDC();
	BOOL AlignTableTheta(DiePhyPos CurPos);
	BOOL GetEdgePosition(LONG lStep);
	BOOL HaveRefdieOn4Dir(DieMapPos iRefCtr, INT *iUp, INT *iDown, INT *iLeft, INT *iRight);
	LONG IndexToEdgeDie(DieMapPos SrcDie, DiePhyPos *CurPhyPos, LONG lDir);
	BOOL RealignRefDiePos(BOOL bReset, BOOL bEdge);

	BOOL GetIniOutputPath(CString *strPathWaf, CString *strPathBin, CString *strPathLog);
	BOOL GetIniOutputFileNm(CString *strWafEnd, CString *strBinFullBin, CString *strBinFullLog);
	BOOL SetHomeDiePos(ULONG ulRow, ULONG ulCol);
	BOOL GetNextCheckKeyDie(LONG lTgtDir, DieMapPos SrcDie, DieMapPos *TgtDie);
	//v3.30T2
	BOOL IsCurrBlkNeedUpdate(DieMapPos MapPos);		//v3.25T16
	LONG GetCurrBlockNum(LONG& lRow, LONG& lCol, DieMapPos MapPos);

	//For Character reference die
	LONG SearchCurrCharDie(LONG *lPosX, LONG *lPosY, LONG *lDigit1, LONG *lDigit2);
	BOOL FoundAllCharDiePos();
	LONG IndexCrossCharDiePos(DiePhyPos **pRefDie, DieMapPos *pStack, int *pStackPtr);
	LONG GetNextCharDieXY(DiePhyPos PhyInput, LONG lRow, LONG lCol, DiePhyPos *PhyOutput);
	LONG ManAlignCharDieWafer(DiePhyPos CurTbl);
	BOOL WftMoveXYTo(LONG lPosX, LONG lPosY);

	static BOOL	Blk_bDebug;

protected:
	CWaferTable* m_stnWaferTable;

private:
	FILE	*pFile;
	CString	strTemp;
	BOOL	m_bRefRhombus;

	LONG	MnSrchRefGrid;
	LONG	MnMaxJumpCtr;
	LONG	MnMaxJumpEdge;
	LONG	MnNoDieGrade;
	LONG	MnAlignReset;

	FDC_Fundament	FDCBasic;
	FDC_Info		RefDieInfo;
	DiePhyPos		DiePhyPrep[MaxRow][MaxCol];
	FDC_SDSOPUT		EmptyCheck;
	LONG			DieArPhy[MaxRow][MaxCol];

	CString	m_strMachineID;
};


#endif  // WT_BLKFUNC_H

//================================================================
// End of file WT_BlkFunc.h
//================================================================
