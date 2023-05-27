#include "stdafx.h"
#include <afxmt.h>
#include "PrescanInfo.h"
#include <algorithm>
#include "PrescanConstant.h"
#include "MS896A_Constant.h"
#include "PrescanUtility.h"

// prescan pr data info 
typedef struct PrescanPrInfo
{
	PrescanPrInfo() : m_nDieEncX(0), m_nDieEncY(0), m_dDieAngle(0.0), 
		m_szDieBin("0"), m_usDieStatus(0), m_uwDiePrID(1),  m_ulFrameID(0) {}
	int		m_nDieEncX;
	int		m_nDieEncY;
	double	m_dDieAngle;
	USHORT	m_uwDiePrID;
	USHORT	m_usDieStatus;
	CString	m_szDieBin;
	ULONG	m_ulFrameID;
} PrescanPrInfo;

typedef struct PrescanRunPosition
{
	PrescanRunPosition() : m_nDieEncX(0), m_nDieEncY(0) {}
	int		m_nDieEncX;
	int		m_nDieEncY;
} PrescanRunPosition;

typedef struct PrescanRemainInfo
{
	PrescanRemainInfo() : m_nDieEncX(0), m_nDieEncY(0), m_ulWSIndex(0) {}
	int		m_nDieEncX;
	int		m_nDieEncY;
	ULONG	m_ulWSIndex;
} PrescanRemainInfo;

// prescan map data info 
typedef struct PrescanMap
{
	PrescanMap() : m_nDieEncX(0), m_nDieEncY(0), m_dDieAngle(0.0), m_szDieBin("0"), m_usDieState(0) {}
	int		m_nDieEncX;
	int		m_nDieEncY;
	USHORT	m_usDieState;
	double	m_dDieAngle;
	CString	m_szDieBin;
} PrescanMap;

// prescan map data frame ID 
typedef struct PrescanMapFrame
{
	PrescanMapFrame() : m_ulPrFrameID(0) {}
	unsigned long	m_ulPrFrameID;
} PrescanMapFrame;

// for prescan twice, that stores all prid=1 all good die record, for later to erase die mark part
typedef struct PrescanGood
{
	PrescanGood() : m_nDieEncX(0), m_nDieEncY(0), m_dDGoodieAngle(0.0),	m_szDieBinGood("0"), m_nRow(0), m_nCol(0) {}
	int		m_nDieEncX;
	int		m_nDieEncY;
	double	m_dDGoodieAngle;
	int		m_nRow;
	int		m_nCol;
	CString	m_szDieBinGood;
} PrescanGood;

typedef struct PrescanNGInfo
{
	PrescanNGInfo() : m_ulMapRow(0), m_ulMapCol(0) {}
	ULONG	m_ulMapRow;
	ULONG	m_ulMapCol;
} PrescanNGInfo;

// prescan run time frame focus level and frame ID, position ... information.
typedef struct PrescanAutoFocusData
{
	PrescanAutoFocusData() : m_lGrabX(0), m_lGrabY(0), m_ulFrameID(0), m_dGrabScore(-1.0) {}
	LONG	m_lGrabX;
	LONG	m_lGrabY;
	ULONG	m_ulFrameID;
	DOUBLE	m_dGrabScore;
} PrescanAutoFocusData;

typedef struct RescanBasePoint
{
	RescanBasePoint() : m_nDieEncX(0), m_nDieEncY(0), m_nRow(0), m_nCol(0) {}
	int		m_nDieEncX;
	int		m_nDieEncY;
	int		m_nRow;
	int		m_nCol;
} RescanBasePoint;

typedef struct RescanRemainPoint
{
	RescanRemainPoint() : m_nDieEncX(0), m_nDieEncY(0), m_nScanIndex(0) {}
	int		m_nDieEncX;
	int		m_nDieEncY;
	int		m_nScanIndex;
} RescanRemainPoint;

typedef struct ScanDiePrRecordID
{
	ScanDiePrRecordID() : m_ulPrRecordID(0) {}
	ULONG	m_ulPrRecordID;
} ScanDiePrRecordID;

static map<ULONG, PrescanPrInfo>		m_gstPrescanInfo;
static ULONG	m_ulWsDieCounter;
static map<ULONG, PrescanRunPosition>	m_gstScanReferList;
static ULONG	m_ulWsReferDieTotal;
static map<ULONG, PrescanRemainInfo>	m_gstScanRemainDieList;
static ULONG	m_ulScanRemainDieTotal;

// loword is row	// hiword  is col
static map<ULONG, PrescanMap>			m_gstPrescanMap;

static map<ULONG, PrescanMapFrame>		m_gstPrescanMapFrame;
static map<ULONG, ScanDiePrRecordID>	m_gstScanMapRecordID;

// for prescan data run time usage, especially for realign of cree case
static map<ULONG, PrescanRunPosition>	m_gstPrescanRunMap;
static map<ULONG, PrescanRunPosition>	m_gstPrescanTmpMap;

static map<ULONG, RescanBasePoint>		m_gstFrameRescanBasePointMap;
static map<ULONG, RescanRemainPoint>	m_gstRescanRemainPointMap;
static map<ULONG, PrescanRunPosition>	m_gstRescanLostDataMap;
static map<ULONG, PrescanRunPosition>	m_gstRescanRefillMap;
static map<ULONG, ScanDiePrRecordID>	m_gstRescanFrameBaseNumMap;
static map<ULONG, PrescanRunPosition>	m_gstScanKeyDieMap;

static map<ULONG, PrescanNGInfo>		m_gstEmptyMap;
static map<ULONG, PrescanNGInfo>		m_gstDefectMap;
static map<ULONG, PrescanNGInfo>		m_gstBadCutMap;
static map<ULONG, PrescanNGInfo>		m_gstExtraMap;

static map<ULONG, PrescanAutoFocusData>	m_gstGrabFocusMap;

static map<ULONG, PrescanGood>			m_gstGoodMap;
static ULONG	m_ulGoodDieCounter;

static CCriticalSection localCS;
static ULONG	m_ulMapTotalDieCounter;

static ULONG CreateIndex(int row, int col)
{
	return MAKELONG(row, col);
}

static void GetCoordFromIndex(ULONG index, int &row, int &col)
{
	row = LOWORD(index);
	col = HIWORD(index);
}

void ClearPrescanInfo()
{
	localCS.Lock();
	DelScanInfo();
	DelPrescanFrameInfo();

	m_gstEmptyMap.clear();
	m_gstDefectMap.clear();
	m_gstBadCutMap.clear();
	m_gstExtraMap.clear();
	m_ulMapTotalDieCounter = 0;

	localCS.Unlock();
}

void DelScanInfo()
{
	m_gstPrescanMap.clear();
}

void SetPrescanPosition(int row, int col, LONG encX, LONG encY)
{
	SetScanInfo(row, col, encX, encY, 0, "0", 0);
}

void SetScanInfo(int row, int col, LONG encX, LONG encY, double dDieAngle, CString szDieBin, USHORT usDieState)
{
	ULONG idx = CreateIndex(row, col);
	PrescanMap info;
	info.m_nDieEncX		= encX;
	info.m_nDieEncY		= encY;
	info.m_dDieAngle	= dDieAngle;
	info.m_szDieBin		= szDieBin;
	info.m_usDieState	= usDieState;

	localCS.Lock();
	m_gstPrescanMap[idx] = info;
	localCS.Unlock();
}

void SetPrescanFrameInfo(int row, int col, ULONG ulFrameID)
{
	ULONG idx = CreateIndex(row, col);
	PrescanMapFrame info;
	info.m_ulPrFrameID = ulFrameID;

	localCS.Lock();
	m_gstPrescanMapFrame[idx] = info;
	localCS.Unlock();
}

bool GetPrescanFrameInfo(int row, int col, ULONG &ulFrameID)
{
	bool ret = false;

	ULONG idx = CreateIndex(row, col);

	localCS.Lock();
	map<ULONG, PrescanMapFrame>::iterator iter = m_gstPrescanMapFrame.find(idx);
	if (m_gstPrescanMapFrame.end() != iter)
	{
		ulFrameID = iter->second.m_ulPrFrameID;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

void DelPrescanFrameInfo()
{
	m_gstPrescanMapFrame.clear();
}



ULONG WSGetPrescanTotalDie()
{
	return m_ulWsDieCounter;
}

void WSSetPrescanPosition(LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin, USHORT uwDiePrID)
{
	WSSetScanPosition(nDieX, nDieY, dDieAngle, szDieBin, 0, uwDiePrID, 0);
}

void WSSetScanPosition(LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin, USHORT usDieState, USHORT uwDiePrID, ULONG ulImageID)
{
	localCS.Lock();
	m_ulWsDieCounter++;
	PrescanPrInfo info;
	if (m_gstPrescanInfo.end() == m_gstPrescanInfo.find(m_ulWsDieCounter))
	{
		m_gstPrescanInfo[m_ulWsDieCounter] = info;
	}

	info.m_uwDiePrID	= uwDiePrID;
	info.m_nDieEncX		= nDieX;
	info.m_nDieEncY		= nDieY;
	info.m_dDieAngle	= dDieAngle;
	info.m_szDieBin		= szDieBin;
	info.m_usDieStatus	= usDieState;
	info.m_ulFrameID	= ulImageID;

	m_gstPrescanInfo[m_ulWsDieCounter] = info;
	localCS.Unlock();
}

BOOL WSGetScanAngleDefectBadcut(ULONG ulIndex, double &dDieAngle,  BOOL &bIsDefect, BOOL &isBadCut)
{
	USHORT usDieState = 0,uwDiePrID ;
	BOOL bIsFakeEmpty = FALSE;
	CString szDieBin;
	LONG encX, encY;
	ULONG ulFrameID;

	BOOL ret = WSGetScanPosition(ulIndex, encX, encY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
	DecodeDieState(usDieState, bIsDefect, isBadCut, bIsFakeEmpty);

	return ret;
}

bool WSGetScanPosition(ULONG ulIndex,  LONG &encX, LONG &encY, DOUBLE &dDieAngle, CString &szDieBin, USHORT &usDieState, USHORT &uwDiePrID, ULONG &ulFrameID)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanPrInfo>::iterator iter = m_gstPrescanInfo.find(ulIndex);
	if (m_gstPrescanInfo.end() != iter)
	{
		szDieBin = iter->second.m_szDieBin;
		dDieAngle = iter->second.m_dDieAngle;
		usDieState = iter->second.m_usDieStatus;
		uwDiePrID = iter->second.m_uwDiePrID;
		encX = iter->second.m_nDieEncX;
		encY = iter->second.m_nDieEncY;
		ulFrameID = iter->second.m_ulFrameID;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

bool WSGetScanPosnAngleFrame(ULONG ulIndex,  LONG &encX, LONG &encY, DOUBLE &dDieAngle, ULONG &ulFrameID)
{
	CString szDieBin = "0";
	USHORT uwDiePrID;
	USHORT usDieState = 0;
	return WSGetScanPosition(ulIndex, encX, encY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
}	//	get frame info

bool WSGetPrescanPosn(ULONG ulIndex,  LONG &encX, LONG &encY)
{
	double dDieAngle;
	CString szDieBin = "0";
	USHORT uwDiePrID;
	ULONG ulFrameID;
	USHORT usDieState = 0;

	return WSGetScanPosition(ulIndex, encX, encY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
}

BOOL WSGetPrescanPosnPr(ULONG ulIndex, LONG &encX, LONG &encY, BOOL &bIsDefect, BOOL &isBadCut, USHORT &uwDiePrID)
{
	double dDieAngle;
	ULONG ulFrameID;

	USHORT usDieState = 0;
	BOOL bIsFakeEmpty = FALSE;
	CString szDieBin;
	BOOL ret = WSGetScanPosition(ulIndex, encX, encY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
	DecodeDieState(usDieState, bIsDefect, isBadCut, bIsFakeEmpty);

	return ret;
}

bool WSGetPosnPrID(ULONG ulIndex, LONG &encX, LONG &encY, USHORT &uwDiePrID)
{
	double dDieAngle;
	CString szDieBin = "0";
	ULONG ulFrameID;
	USHORT usDieState = 0;

	return WSGetScanPosition(ulIndex, encX, encY, dDieAngle, szDieBin, usDieState, uwDiePrID, ulFrameID);
}

void ClearWSPrescanInfo()
{
	m_gstPrescanInfo.clear();
	m_ulWsDieCounter = 0;
	WSDelScanReferInfo();
}

// for dies that has no orientation, can not detect tip up or down
ULONG WSGetScanRemainDieTotal()
{
	return m_ulScanRemainDieTotal;
}

void WSAddScanRemainDieIntoList(LONG nDieX, LONG nDieY, ULONG ulWSIndex)
{
	localCS.Lock();

	PrescanRemainInfo info;

	m_ulScanRemainDieTotal++;

	info.m_nDieEncX		= nDieX;
	info.m_nDieEncY		= nDieY;
	info.m_ulWSIndex	= ulWSIndex;

	m_gstScanRemainDieList[m_ulScanRemainDieTotal] = info;

	localCS.Unlock();
}

void	WSUpdateScanRemainDieInList(ULONG ulIndex, LONG nDieX, LONG nDieY, ULONG ulWSIndex)
{
	localCS.Lock();


	if( ulIndex>0 && ulIndex<=m_ulScanRemainDieTotal )
	{
		PrescanRemainInfo info;
		info.m_nDieEncX		= nDieX;
		info.m_nDieEncY		= nDieY;
		info.m_ulWSIndex	= ulWSIndex;

		m_gstScanRemainDieList[ulIndex] = info;
	}

	localCS.Unlock();
}

bool WSGetScanRemainDieFromList(ULONG ulIndex, LONG &encX, LONG &encY, ULONG &ulWSIndex)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanRemainInfo>::iterator iter = m_gstScanRemainDieList.find(ulIndex);
	if (m_gstScanRemainDieList.end() != iter)
	{
		encX		= iter->second.m_nDieEncX;
		encY		= iter->second.m_nDieEncY;
		ulWSIndex	= iter->second.m_ulWSIndex;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

void WSClearScanRemainDieList()
{
	m_gstScanRemainDieList.clear();
	m_ulScanRemainDieTotal = 0;
}


BOOL GetPrescanInfo(int row, int col, LONG &encX, LONG &encY, double &dDieAngle, CString &szDieBin, BOOL &bIsDefect, BOOL &isBadCut)
{
	BOOL bIsFakeEmpty = FALSE;
	USHORT usDieState = 0;
	BOOL bRet = GetScanInfo(row, col, encX, encY, dDieAngle, szDieBin, usDieState);
	DecodeDieState(usDieState, bIsDefect, isBadCut, bIsFakeEmpty);
	return bRet;
}

bool GetScanInfo(int row, int col, LONG &encX, LONG &encY, double &dDieAngle, CString &szDieBin, USHORT &usDieState)
{
	bool ret = false;

	ULONG idx = CreateIndex(row, col);

	localCS.Lock();
	map<ULONG, PrescanMap>::iterator iter = m_gstPrescanMap.find(idx);
	if (m_gstPrescanMap.end() != iter)
	{
		szDieBin	= iter->second.m_szDieBin;
		usDieState = iter->second.m_usDieState;
		dDieAngle	= iter->second.m_dDieAngle;
		encX = iter->second.m_nDieEncX;
		encY = iter->second.m_nDieEncY;
		ret = true;
	}
	localCS.Unlock();

	if( usDieState==9889 )
	{
		ret = FALSE;
	}
	return ret;
}

bool GetPrescanPosition(int row, int col, LONG &encX, LONG &encY)
{
	double dDieAngle = 0;
	CString szDieBin = "";
	USHORT usDieState = 0;
	return GetScanInfo(row, col, encX, encY, dDieAngle, szDieBin, usDieState);
}

void SavePrescanInfo(ULONG ulState, ULONG ulDieCount)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( pUtl->GetPrescanDebug() && (m_gstPrescanMap.size() > 0) )
	{
		localCS.Lock();
		FILE	*fp = NULL;
		CString szFileName, szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);

		if( ulDieCount==0 || ulState==0 )
			szFileName = szLogPath + PRESCAN_PSN_SCNWFT;
		else
		{
			if( ulState==1 )
				szFileName.Format("%s_%07lu%s", szLogPath, ulDieCount, RESCAN_OLD_SCN_FILE);
			else
				szFileName.Format("%s_%07lu%s", szLogPath, ulDieCount, RESCAN_NEW_SCN_FILE);
		}

		//change to absolute time scale
		errno_t nErr = fopen_s(&fp, szFileName, "w");
		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "row, col, wtx, wty");
			fprintf(fp, ", bin, rot, sta\n");
			fprintf(fp, "\n");
			map<ULONG, PrescanMap>::iterator iter = m_gstPrescanMap.begin();
			int nWtX, nWtY;
			int row, col;
			while (iter != m_gstPrescanMap.end())
			{
				nWtX  = iter->second.m_nDieEncX;
				nWtY  = iter->second.m_nDieEncY;
				GetCoordFromIndex(iter->first, row, col);
				fprintf(fp, "%i, %i, %i, %i", row, col, nWtX, nWtY);
				fprintf(fp, ", %s, %.2f, %i", (LPCTSTR)iter->second.m_szDieBin,
					iter->second.m_dDieAngle, iter->second.m_usDieState);
				fprintf(fp, "\n");
				++iter;
			}
			fclose(fp);
		}
		localCS.Unlock();
	}
}

void SavePrescanInfoPr(ULONG ulDieCount)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDebug() && (m_gstPrescanInfo.size() > 0) )
	{
		localCS.Lock();
		FILE	*fp = NULL;
		CString szFileName, szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		if( ulDieCount==0 )
			szFileName = szLogPath + PRESCAN_PSN_RAWDIE;
		else
			szFileName.Format("%s_%07lu%s", szLogPath, ulDieCount, RESCAN_RAW_DIE_FILE);

		errno_t nErr = fopen_s(&fp, szFileName, "w");
		if ((nErr == 0) && (fp != NULL))
		{
			int nWtX, nWtY;
			fprintf(fp, "index, prid, frame, DieBin, Angle, state, wtx, wty\n");
			map<ULONG, PrescanPrInfo>::iterator iter = m_gstPrescanInfo.begin();
			while (iter != m_gstPrescanInfo.end())
			{
				nWtX  = iter->second.m_nDieEncX;
				nWtY  = iter->second.m_nDieEncY;

				fprintf(fp, "%ld, %i, %lu, ", iter->first, iter->second.m_uwDiePrID, iter->second.m_ulFrameID);

				fprintf(fp, "%s, %.2f, %i, ",
					(LPCTSTR)iter->second.m_szDieBin,	iter->second.m_dDieAngle,
					iter->second.m_usDieStatus);

				fprintf(fp, "%i, %i\n", nWtX, nWtY);
				++iter;
			}
			if( m_gstScanReferList.size() > 0 )
			{
				map<ULONG, PrescanRunPosition>::iterator iter = m_gstScanReferList.begin();
				while (iter != m_gstScanReferList.end())
				{
					nWtX  = iter->second.m_nDieEncX;
					nWtY  = iter->second.m_nDieEncY;
					fprintf(fp, "%ld, %i, %i\n", iter->first, nWtX, nWtY);
					++iter;
				}
			}
			fclose(fp);
		}
		localCS.Unlock();
	}
}





void SetExtraInfo(ULONG ulIndex, ULONG ulRow, ULONG ulCol)
{
	localCS.Lock();
	PrescanNGInfo NgInfo;
	NgInfo.m_ulMapRow = ulRow;
	NgInfo.m_ulMapCol = ulCol;

	m_gstExtraMap[ulIndex] = NgInfo;
	localCS.Unlock();
}

bool GetEmptyInfo(ULONG ulIndex, ULONG &ulRow, ULONG &ulCol)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanNGInfo>::iterator iter = m_gstEmptyMap.find(ulIndex);
	if (m_gstEmptyMap.end() != iter)
	{
		ulRow = iter->second.m_ulMapRow;
		ulCol = iter->second.m_ulMapCol;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

bool GetDefectInfo(ULONG ulIndex, ULONG &ulRow, ULONG &ulCol)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanNGInfo>::iterator iter = m_gstDefectMap.find(ulIndex);
	if (m_gstDefectMap.end() != iter)
	{
		ulRow = iter->second.m_ulMapRow;
		ulCol = iter->second.m_ulMapCol;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

bool GetBadCutInfo(ULONG ulIndex, ULONG &ulRow, ULONG &ulCol)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanNGInfo>::iterator iter = m_gstBadCutMap.find(ulIndex);
	if (m_gstBadCutMap.end() != iter)
	{
		ulRow = iter->second.m_ulMapRow;
		ulCol = iter->second.m_ulMapCol;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

bool GetExtraInfo(ULONG ulIndex, ULONG &ulRow, ULONG &ulCol)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanNGInfo>::iterator iter = m_gstExtraMap.find(ulIndex);
	if (m_gstExtraMap.end() != iter)
	{
		ulRow = iter->second.m_ulMapRow;
		ulCol = iter->second.m_ulMapCol;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

ULONG GetEmptySize()
{
	return (ULONG)m_gstEmptyMap.size();
}

ULONG GetDefectSize()
{
	return (ULONG)m_gstDefectMap.size();
}

ULONG GetBadCutSize()
{
	return (ULONG)m_gstBadCutMap.size();
}

ULONG GetExtraSize()
{
	return (ULONG)m_gstExtraMap.size();
}

void SetMapTotalDie(ULONG ulCounter)
{
	m_ulMapTotalDieCounter = ulCounter;
}

ULONG GetMapTotalDie()
{
	return m_ulMapTotalDieCounter;
}

void SetGoodPosnOnly(LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin, LONG nRow, LONG nCol)
{
	localCS.Lock();
	m_ulGoodDieCounter++;
	PrescanGood stGoodInfo;
	if (m_gstGoodMap.end() == m_gstGoodMap.find(m_ulGoodDieCounter))
	{
		m_gstGoodMap[m_ulGoodDieCounter] = stGoodInfo;
	}

	stGoodInfo.m_nDieEncX		= nDieX;
	stGoodInfo.m_nDieEncY		= nDieY;
	stGoodInfo.m_dDGoodieAngle	= dDieAngle;
	stGoodInfo.m_szDieBinGood	= szDieBin;
	stGoodInfo.m_nRow			= nRow;
	stGoodInfo.m_nCol			= nCol;

	m_gstGoodMap[m_ulGoodDieCounter] = stGoodInfo;
	localCS.Unlock();
}

ULONG GetGoodTotalDie()
{
	return (ULONG)m_gstGoodMap.size();
}

void ClearGoodInfo()
{
	m_gstGoodMap.clear();
	m_ulGoodDieCounter = 0;
}

bool GetGoodPosnOnly(ULONG ulIndex, LONG &lPosX, LONG &lPosY, DOUBLE &dDieAngle)
{
	CString szDieBin = "0";
	return GetGoodPosnBin(ulIndex, lPosX, lPosY, dDieAngle, szDieBin);
}

bool GetGoodPosnBin(ULONG ulIndex,LONG &lPosX, LONG &lPosY, DOUBLE &dDieAngle, CString &szDieBin)
{
	LONG lRow = 0, lCol = 0;
	return GetGoodPosnAll(ulIndex, lPosX, lPosY, dDieAngle, szDieBin, lRow, lCol);
}

bool GetGoodPosnAll(ULONG ulIndex, LONG &lPosX, LONG &lPosY, DOUBLE &dDieAngle, CString &szDieBin, LONG &lRow, LONG &lCol)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanGood>::iterator iter = m_gstGoodMap.find(ulIndex);
	if (m_gstGoodMap.end() != iter)
	{
		lPosX		= iter->second.m_nDieEncX;
		lPosY		= iter->second.m_nDieEncY;
		dDieAngle	= iter->second.m_dDGoodieAngle;
		szDieBin	= iter->second.m_szDieBinGood;
		lRow		= iter->second.m_nRow;
		lCol		= iter->second.m_nCol;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}

void SetPrescanRunPosn(int row, int col, LONG encX, LONG encY)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	PrescanRunPosition info;
	info.m_nDieEncX = encX;
	info.m_nDieEncY = encY;

	m_gstPrescanRunMap[idx] = info;

	localCS.Unlock();
}

bool GetPrescanRunPosn(int row, int col, LONG &encX, LONG &encY)
{
	localCS.Lock();

	bool ret = false;
	ULONG idx = CreateIndex(row, col);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstPrescanRunMap.find(idx);
	if (m_gstPrescanRunMap.end() != iter)
	{
		encX = iter->second.m_nDieEncX;
		encY = iter->second.m_nDieEncY;
		ret = true;
	}

	localCS.Unlock();

	return ret;
}

void DelPrescanRunPosn()
{
	m_gstPrescanRunMap.clear();
}

void SetPrescanTmpPosn(int row, int col, LONG encX, LONG encY)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	PrescanRunPosition info;
	info.m_nDieEncX = encX;
	info.m_nDieEncY = encY;

	m_gstPrescanTmpMap[idx] = info;

	localCS.Unlock();
}

bool GetPrescanTmpPosn(int row, int col, LONG &encX, LONG &encY)
{
	localCS.Lock();

	bool ret = false;
	ULONG idx = CreateIndex(row, col);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstPrescanTmpMap.find(idx);
	if (m_gstPrescanTmpMap.end() != iter)
	{
		encX = iter->second.m_nDieEncX;
		encY = iter->second.m_nDieEncY;
		ret = true;
	}

	localCS.Unlock();

	return ret;
}

void CutPrescanTmpPosn(int row, int col)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstPrescanTmpMap.find(idx);
	if (m_gstPrescanTmpMap.end() != iter)
	{
		m_gstPrescanTmpMap.erase(idx);
	}

	localCS.Unlock();
}

void DelPrescanTmpPosn()
{
	m_gstPrescanTmpMap.clear();
}

ULONG SetFrameRescanBasePoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG  lMapRow, LONG  lMapCol, LONG  lWftX, LONG  lWftY)
{
	localCS.Lock();

	INT nRowCol = (INT) MAKEWORD(ucRow, ucCol);
	ULONG ulIndex = CreateIndex(nIndex, nRowCol);
	RescanBasePoint BasePointInfo;
	BasePointInfo.m_nRow		= lMapRow;
	BasePointInfo.m_nCol		= lMapCol;
	BasePointInfo.m_nDieEncX	= lWftX;
	BasePointInfo.m_nDieEncY	= lWftY;

	m_gstFrameRescanBasePointMap[ulIndex] = BasePointInfo;

	localCS.Unlock();
	return ulIndex;
}

bool GetFrameRescanBasePoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG &lMapRow, LONG &lMapCol, LONG &lWftX, LONG &lWftY)
{
	localCS.Lock();

	bool ret = false;
	INT nRowCol = (INT) MAKEWORD(ucRow, ucCol);
	ULONG ulIndex = CreateIndex(nIndex, nRowCol);

	map<ULONG, RescanBasePoint>::iterator iter = m_gstFrameRescanBasePointMap.find(ulIndex);
	if (m_gstFrameRescanBasePointMap.end() != iter)
	{
		lMapRow	= iter->second.m_nRow;
		lMapCol	= iter->second.m_nCol;
		lWftX	= iter->second.m_nDieEncX;
		lWftY	= iter->second.m_nDieEncY;
		ret = true;
	}

	localCS.Unlock();

	return ret;
}

void CutFrameRescanBasePoint(UCHAR ucRow, UCHAR ucCol, INT nIndex)
{
	localCS.Lock();

	INT nRowCol = (INT) MAKEWORD(ucRow, ucCol);
	ULONG ulIndex = CreateIndex(nIndex, nRowCol);

	map<ULONG, RescanBasePoint>::iterator iter = m_gstFrameRescanBasePointMap.find(ulIndex);
	if (m_gstFrameRescanBasePointMap.end() != iter)
	{
		m_gstFrameRescanBasePointMap.erase(ulIndex);
	}
	localCS.Unlock();
}

void DelFrameRescanBasePoints()
{
	m_gstFrameRescanBasePointMap.clear();
}	//	erase all data.

void SetEmptyFull(ULONG ulIndex, ULONG ulRow, ULONG ulCol, UCHAR ucGrd)
{
	localCS.Lock();
	PrescanNGInfo NgInfo;
	NgInfo.m_ulMapRow = ulRow;
	NgInfo.m_ulMapCol = ulCol;

	m_gstEmptyMap[ulIndex] = NgInfo;
	localCS.Unlock();
}

void SetDefectFull(ULONG ulIndex, ULONG ulRow, ULONG ulCol, UCHAR ucGrd)
{
	localCS.Lock();
	PrescanNGInfo NgInfo;
	NgInfo.m_ulMapRow = ulRow;
	NgInfo.m_ulMapCol = ulCol;

	m_gstDefectMap[ulIndex] = NgInfo;
	localCS.Unlock();
}

void SetBadCutFull(ULONG ulIndex, ULONG ulRow, ULONG ulCol, UCHAR ucGrd)
{
	localCS.Lock();
	PrescanNGInfo NgInfo;
	NgInfo.m_ulMapRow = ulRow;
	NgInfo.m_ulMapCol = ulCol;

	m_gstBadCutMap[ulIndex] = NgInfo;
	localCS.Unlock();
}






// grab focust data relative handling: clear set get size
VOID DelGrabFocusData()
{
	m_gstGrabFocusMap.clear();
}

ULONG GetGrabFocusDataSize()
{
	return (ULONG)m_gstGrabFocusMap.size();
}

VOID SetGrabFocusData(ULONG ulRow, ULONG ulCol, LONG lGrabX, LONG lGrabY, ULONG ulFrameID, DOUBLE dFocusScore)
{
	localCS.Lock();

	PrescanAutoFocusData stFocusData;

	ULONG ulIndex = CreateIndex(ulRow, ulCol);

	stFocusData.m_lGrabX		= lGrabX;
	stFocusData.m_lGrabY		= lGrabY;
	stFocusData.m_ulFrameID		= ulFrameID;
	stFocusData.m_dGrabScore	= dFocusScore;

	m_gstGrabFocusMap[ulIndex] = stFocusData;

	localCS.Unlock();
}

BOOL GetGrabFocusData(ULONG ulRow, ULONG ulCol, LONG &lGrabX, LONG &lGrabY, ULONG &ulFrameID, DOUBLE &dFocusScore)
{
	BOOL ret = FALSE;

	localCS.Lock();

	ULONG ulIndex = CreateIndex(ulRow, ulCol);
	map<ULONG, PrescanAutoFocusData>::iterator iter = m_gstGrabFocusMap.find(ulIndex);
	if (m_gstGrabFocusMap.end() != iter)
	{
		lGrabX		= iter->second.m_lGrabX;
		lGrabY		= iter->second.m_lGrabY;
		ulFrameID	= iter->second.m_ulFrameID;
		dFocusScore	= iter->second.m_dGrabScore;
		ret = TRUE;
	}
	localCS.Unlock();

	return ret;
}

DOUBLE GetGrabFocusScore(ULONG ulRow, ULONG ulCol)
{
	DOUBLE dFocusScore = -1.0;

	localCS.Lock();

	ULONG ulIndex = CreateIndex(ulRow, ulCol);
	map<ULONG, PrescanAutoFocusData>::iterator iter = m_gstGrabFocusMap.find(ulIndex);
	if (m_gstGrabFocusMap.end() != iter)
	{
		dFocusScore	= iter->second.m_dGrabScore;
	}
	localCS.Unlock();

	return dFocusScore;
}
// grab focust data relative handling: clear set get size

unsigned long WSGetScanReferTotalDie()
{
	return m_ulWsReferDieTotal;
}

void WSSetScanReferPosition(LONG nDieX, LONG nDieY)
{
	localCS.Lock();
	m_ulWsReferDieTotal++;
	PrescanRunPosition info;
	if (m_gstScanReferList.end() == m_gstScanReferList.find(m_ulWsReferDieTotal))
	{
		m_gstScanReferList[m_ulWsReferDieTotal] = info;
	}

	info.m_nDieEncX		= nDieX;
	info.m_nDieEncY		= nDieY;

	m_gstScanReferList[m_ulWsReferDieTotal] = info;
	localCS.Unlock();
}

bool WSGetScanReferPosition(ULONG ulIndex,  LONG &encX, LONG &encY)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstScanReferList.find(ulIndex);
	if (m_gstScanReferList.end() != iter)
	{
		encX		= iter->second.m_nDieEncX;
		encY		= iter->second.m_nDieEncY;
		ret			= true;
	}
	localCS.Unlock();

	return ret;
}

void WSDelScanReferInfo()
{
	m_gstScanReferList.clear();
	m_ulWsReferDieTotal = 0;
}


USHORT	EncodeDieState(BOOL bIsDefect, BOOL bIsBadCut, BOOL bIsFakeEmpty)
{
	USHORT usDieState = 0;
	if (bIsDefect)
	{
		usDieState	+= 2;
	}

	if (bIsBadCut)
	{
		usDieState	+= 4;
	}

	if (bIsFakeEmpty)
	{
		usDieState	+= 8;
	}

	return usDieState;
}

VOID	DecodeDieState(USHORT usInDieState, BOOL &bIsDefect, BOOL &bIsBadCut, BOOL &bIsFakeEmpty)
{
	USHORT usDieState = usInDieState;
	bIsFakeEmpty = FALSE;
	bIsDefect = FALSE;
	bIsBadCut = FALSE;

	if( usDieState>=8 && usDieState<100)
	{
		bIsFakeEmpty = TRUE;
		usDieState -= 8;
	}
	if( usDieState>=4 )
	{
		bIsBadCut = TRUE;
		usDieState -= 4;
	}
	if( usDieState>=2 )
	{
		bIsDefect = TRUE;
		usDieState -=2;
	}
}

void SetScanMapRecordID(int row, int col, ULONG ulRecordID)
{
	localCS.Lock();
	ULONG idx = CreateIndex(row, col);
	ScanDiePrRecordID info;
	info.m_ulPrRecordID = ulRecordID;
	m_gstScanMapRecordID[idx] = info;
	localCS.Unlock();
}

ULONG GetScanMapRecordID(int row, int col)
{
	ULONG ulRecordID = 0;

	localCS.Lock();
	ULONG idx = CreateIndex(row, col);
	map<ULONG, ScanDiePrRecordID>::iterator iter = m_gstScanMapRecordID.find(idx);
	if (m_gstScanMapRecordID.end() != iter)
	{
		ulRecordID = iter->second.m_ulPrRecordID;
	}
	localCS.Unlock();

	return ulRecordID;
}

void DelScanMapRecordID()
{
	m_gstScanMapRecordID.clear();
}

ULONG SetRescanRemainPoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG  lScanIndex, LONG  lWftX, LONG  lWftY)
{
	localCS.Lock();

	INT nRowCol = (INT) MAKEWORD(ucRow, ucCol);
	ULONG ulIndex = CreateIndex(nIndex, nRowCol);
	RescanRemainPoint BasePointInfo;
	BasePointInfo.m_nScanIndex	= lScanIndex;
	BasePointInfo.m_nDieEncX	= lWftX;
	BasePointInfo.m_nDieEncY	= lWftY;

	m_gstRescanRemainPointMap[ulIndex] = BasePointInfo;

	localCS.Unlock();
	return ulIndex;
}

bool GetRescanRemainPoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG &lScanIndex, LONG &lWftX, LONG &lWftY)
{
	localCS.Lock();

	bool ret = false;
	INT nRowCol = (INT) MAKEWORD(ucRow, ucCol);
	ULONG ulIndex = CreateIndex(nIndex, nRowCol);

	map<ULONG, RescanRemainPoint>::iterator iter = m_gstRescanRemainPointMap.find(ulIndex);
	if (m_gstRescanRemainPointMap.end() != iter)
	{
		lScanIndex	= iter->second.m_nScanIndex;
		lWftX		= iter->second.m_nDieEncX;
		lWftY		= iter->second.m_nDieEncY;
		ret = true;
	}

	localCS.Unlock();

	return ret;
}

void CutRescanRemainPoint(UCHAR ucRow, UCHAR ucCol, INT nIndex)
{
	localCS.Lock();

	INT nRowCol = (INT) MAKEWORD(ucRow, ucCol);
	ULONG ulIndex = CreateIndex(nIndex, nRowCol);

	map<ULONG, RescanRemainPoint>::iterator iter = m_gstRescanRemainPointMap.find(ulIndex);
	if (m_gstRescanRemainPointMap.end() != iter)
	{
		m_gstRescanRemainPointMap.erase(ulIndex);
	}
	localCS.Unlock();
}

void DelRescanRemainPoint()
{
	m_gstRescanRemainPointMap.clear();
}	//	erase all data.

VOID DelRescanRefillData()
{
	m_gstRescanRefillMap.clear();
}

VOID SetRescanRefillData(ULONG ulRow, ULONG ulCol, LONG lWftX, LONG lWftY)
{
	localCS.Lock();

	PrescanRunPosition stDieData;

	ULONG ulIndex = CreateIndex(ulRow, ulCol);

	stDieData.m_nDieEncX		= lWftX;
	stDieData.m_nDieEncY		= lWftY;

	m_gstRescanRefillMap[ulIndex] = stDieData;

	localCS.Unlock();
}

BOOL GetRescanRefillData(ULONG ulRow, ULONG ulCol, LONG &lWftX, LONG &lWftY)
{
	BOOL ret = FALSE;

	localCS.Lock();

	ULONG ulIndex = CreateIndex(ulRow, ulCol);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstRescanRefillMap.find(ulIndex);
	if (m_gstRescanRefillMap.end() != iter)
	{
		lWftX		= iter->second.m_nDieEncX;
		lWftY		= iter->second.m_nDieEncY;
		ret = TRUE;
	}
	localCS.Unlock();

	return ret;
}

void SetRescanLostData(int row, int col, LONG encX, LONG encY)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	PrescanRunPosition info;
	info.m_nDieEncX = encX;
	info.m_nDieEncY = encY;

	m_gstRescanLostDataMap[idx] = info;

	localCS.Unlock();
}

bool GetRescanLostData(int row, int col, LONG &encX, LONG &encY)
{
	localCS.Lock();

	bool ret = false;
	ULONG idx = CreateIndex(row, col);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstRescanLostDataMap.find(idx);
	if (m_gstRescanLostDataMap.end() != iter)
	{
		encX = iter->second.m_nDieEncX;
		encY = iter->second.m_nDieEncY;
		ret = true;
	}

	localCS.Unlock();

	return ret;
}

void CutRescanLostData(int row, int col)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstRescanLostDataMap.find(idx);
	if (m_gstRescanLostDataMap.end() != iter)
	{
		m_gstRescanLostDataMap.erase(idx);
	}

	localCS.Unlock();
}

void DelRescanLostData()
{
	m_gstRescanLostDataMap.clear();
}

void SetRescanFrameBaseNum(int row, int col, ULONG ulBaseNum)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	ScanDiePrRecordID info;
	info.m_ulPrRecordID = ulBaseNum;

	m_gstRescanFrameBaseNumMap[idx] = info;

	localCS.Unlock();
}

bool GetRescanFrameBaseNum(int row, int col, ULONG &ulBaseNum)
{
	localCS.Lock();

	bool ret = false;
	ULONG idx = CreateIndex(row, col);
	map<ULONG, ScanDiePrRecordID>::iterator iter = m_gstRescanFrameBaseNumMap.find(idx);
	if (m_gstRescanFrameBaseNumMap.end() != iter)
	{
		ulBaseNum = iter->second.m_ulPrRecordID;
		ret = true;
	}

	localCS.Unlock();

	return ret;
}

void CutRescanFrameBaseNum(int row, int col)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	map<ULONG, ScanDiePrRecordID>::iterator iter = m_gstRescanFrameBaseNumMap.find(idx);
	if (m_gstRescanFrameBaseNumMap.end() != iter)
	{
		m_gstRescanFrameBaseNumMap.erase(idx);
	}

	localCS.Unlock();
}

void DelRescanFrameBaseNum()
{
	m_gstRescanFrameBaseNumMap.clear();
}

//	DEB key die sample with prescan FOV, one time several key dice
void SetScanKeyDie(int row, int col, LONG encX, LONG encY)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	PrescanRunPosition info;
	info.m_nDieEncX = encX;
	info.m_nDieEncY = encY;

	m_gstScanKeyDieMap[idx] = info;

	localCS.Unlock();
}

bool GetScanKeyDie(int row, int col, LONG &encX, LONG &encY)
{
	localCS.Lock();

	bool ret = false;
	ULONG idx = CreateIndex(row, col);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstScanKeyDieMap.find(idx);
	if (m_gstScanKeyDieMap.end() != iter)
	{
		encX = iter->second.m_nDieEncX;
		encY = iter->second.m_nDieEncY;
		ret = true;
	}

	localCS.Unlock();

	return ret;
}

void CutScanKeyDie(int row, int col)
{
	localCS.Lock();

	ULONG idx = CreateIndex(row, col);
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstScanKeyDieMap.find(idx);
	if (m_gstScanKeyDieMap.end() != iter)
	{
		m_gstScanKeyDieMap.erase(idx);
	}

	localCS.Unlock();
}

void DelScanKeyDie()
{
	m_gstScanKeyDieMap.clear();
}

static map<ULONG, PrescanRunPosition>	m_gstRescanRetryMap;
static ULONG	m_ulRescanRetryCounter;

void	SetRescanRetry(LONG nDieX, LONG nDieY)
{
	localCS.Lock();
	m_ulRescanRetryCounter++;
	PrescanRunPosition stRescan;
	if (m_gstRescanRetryMap.end() == m_gstRescanRetryMap.find(m_ulRescanRetryCounter))
	{
		m_gstRescanRetryMap[m_ulRescanRetryCounter] = stRescan;
	}

	stRescan.m_nDieEncX		= nDieX;
	stRescan.m_nDieEncY		= nDieY;

	m_gstRescanRetryMap[m_ulRescanRetryCounter] = stRescan;
	localCS.Unlock();
}

ULONG	GetRescanRetryTotal()
{
	return (ULONG)m_gstRescanRetryMap.size();
}

void	DelRescanRetry()
{
	m_gstRescanRetryMap.clear();
	m_ulRescanRetryCounter = 0;
}

bool	GetRescanRetry(ULONG ulIndex, LONG &lPosX, LONG &lPosY)
{
	bool ret = false;

	localCS.Lock();
	map<ULONG, PrescanRunPosition>::iterator iter = m_gstRescanRetryMap.find(ulIndex);
	if (m_gstRescanRetryMap.end() != iter)
	{
		lPosX		= iter->second.m_nDieEncX;
		lPosY		= iter->second.m_nDieEncY;
		ret = true;
	}
	localCS.Unlock();

	return ret;
}	//	rescan retry

