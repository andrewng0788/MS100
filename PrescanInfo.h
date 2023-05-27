#pragma once

#include <map>
using std::map;

// Prescan Result

void ClearPrescanInfo();

USHORT	EncodeDieState(BOOL bIsDefect, BOOL bIsBadCut, BOOL bIsFakeEmpty);
VOID	DecodeDieState(USHORT usDieState, BOOL &bIsDefect, BOOL &bIsBadCut, BOOL &bIsFakeEmpty);

void WSSetPrescanPosition(			   LONG nDieX, LONG nDieY, DOUBLE  dDieAngle, CString  szDieBin, USHORT  uwDiePrID);
void WSSetScanPosition(				   LONG nDieX, LONG nDieY, DOUBLE  dDieAngle, CString  szDieBin, USHORT  usDieState, USHORT  uwDiePrID, ULONG  ulImageID);
bool WSGetScanPosition(ULONG ulIndex,  LONG &encX, LONG &encY, DOUBLE &dDieAngle, CString &szDieBin, USHORT &usDieState, USHORT &uwDiePrID, ULONG &ulFrameID);
bool WSGetScanPosnAngleFrame(ULONG ulIndex,  LONG &encX, LONG &encY, DOUBLE &dDieAngle, ULONG &ulFrameID);
bool WSGetPrescanPosn(ULONG ulIndex, LONG &encX, LONG &encY);
bool WSGetPosnPrID(   ULONG ulIndex, LONG &encX, LONG &encY, USHORT &uwDiePrID);
BOOL WSGetScanAngleDefectBadcut(ULONG ulIndex, DOUBLE &dDieAngle, BOOL &bIsDefect, BOOL &isBadCut);
BOOL WSGetPrescanPosnPr(ULONG ulIndex,  LONG &encX, LONG &encY, BOOL &bIsDefect, BOOL &isBadCut, USHORT &uwDiePrID);

ULONG WSGetPrescanTotalDie();
void ClearWSPrescanInfo();

unsigned long WSGetScanReferTotalDie();
void WSSetScanReferPosition(LONG nDieX, LONG nDieY);
bool WSGetScanReferPosition(ULONG ulIndex,  LONG &encX, LONG &encY);
void WSDelScanReferInfo();

ULONG	WSGetScanRemainDieTotal();
void	WSAddScanRemainDieIntoList(LONG nDieX, LONG nDieY, ULONG ulWSIndex);
void	WSUpdateScanRemainDieInList(ULONG ulIndex, LONG nDieX, LONG nDieY, ULONG ulWSIndex);
bool	WSGetScanRemainDieFromList(ULONG ulIndex,  LONG &encX, LONG &encY, ULONG &ulWSIndex);
void	WSClearScanRemainDieList();

void SetPrescanPosition(int row, int col, LONG encX, LONG encY);
void SetScanInfo(int row, int col, LONG encX, LONG encY, double dDieAngle, CString szDieBin, USHORT usDieState);
bool GetScanInfo(int row, int col, LONG &encX, LONG &encY, double &dDieAngle, CString &szDieBin, USHORT &usDieState);
BOOL GetPrescanInfo(int row, int col, LONG &encX, LONG &encY, double &dDieAngle, CString &szDieBin, BOOL &bIsDefect, BOOL &isBadCut);
bool GetPrescanPosition(int row, int col, LONG &encX, LONG &encY);
void DelScanInfo();

void SetPrescanFrameInfo(int row, int col, ULONG ulFrameID);
bool GetPrescanFrameInfo(int row, int col, ULONG &ulFrameID);
void DelPrescanFrameInfo();

void  SetScanMapRecordID(int row, int col, ULONG ulRecordID);
ULONG GetScanMapRecordID(int row, int col);
void  DelScanMapRecordID();

void SetPrescanRunPosn(int row, int col, LONG encX, LONG encY);
bool GetPrescanRunPosn(int row, int col, LONG &encX, LONG &encY);
void DelPrescanRunPosn();

void SavePrescanInfo(ULONG ulState = 0, ULONG ulDieCount=0);
void SavePrescanInfoPr(ULONG ulDieCount=0);

void SetExtraInfo( ULONG ulIndex, ULONG ulRow, ULONG ulCol);
void SetEmptyFull( ULONG ulIndex, ULONG ulRow, ULONG ulCol, UCHAR ucGrd=0);
void SetDefectFull(ULONG ulIndex, ULONG ulRow, ULONG ulCol, UCHAR ucGrd=0);
void SetBadCutFull(ULONG ulIndex, ULONG ulRow, ULONG ulCol, UCHAR ucGrd=0);

bool GetExtraInfo( ULONG ulIndex, ULONG &ulRow, ULONG &ulCol);
bool GetEmptyInfo( ULONG ulIndex, ULONG &ulRow, ULONG &ulCol);
bool GetDefectInfo(ULONG ulIndex, ULONG &ulRow, ULONG &ulCol);
bool GetBadCutInfo(ULONG ulIndex, ULONG &ulRow, ULONG &ulCol);

ULONG GetExtraSize();
ULONG GetEmptySize();
ULONG GetDefectSize();
ULONG GetBadCutSize();

void SetMapTotalDie(ULONG ulCounter);
ULONG GetMapTotalDie();

void SetGoodPosnOnly(LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin = "", LONG nRow=0, LONG nCol=0);
bool GetGoodPosnOnly(ULONG ulIndex, LONG &lPosX, LONG &lPosY, DOUBLE &dDieAngle);
bool GetGoodPosnBin(ULONG ulIndex, LONG &lPosX, LONG &lPosY, DOUBLE &dDieAngle, CString &szDieBin);
bool GetGoodPosnAll(ULONG ulIndex, LONG &lPosX, LONG &lPosY, DOUBLE &dDieAngle, CString &szDieBin, LONG &lRow, LONG &lCol);
ULONG GetGoodTotalDie();
void ClearGoodInfo();

void	SetRescanRetry(LONG nDieX, LONG nDieY);
bool	GetRescanRetry(ULONG ulIndex, LONG &lPosX, LONG &lPosY);
ULONG	GetRescanRetryTotal();
void	DelRescanRetry();

void SetPrescanTmpPosn(int row, int col, LONG encX, LONG encY);
bool GetPrescanTmpPosn(int row, int col, LONG &encX, LONG &encY);
void CutPrescanTmpPosn(int row, int col);
void DelPrescanTmpPosn();

ULONG SetFrameRescanBasePoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG  lMapRow, LONG  lMapCol, LONG  lWftX, LONG  lWftY);
bool  GetFrameRescanBasePoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG &lMapRow, LONG &lMapCol, LONG &lWftX, LONG &lWftY);
void  CutFrameRescanBasePoint(UCHAR ucRow, UCHAR ucCol, INT nIndex);
void  DelFrameRescanBasePoints();

VOID	DelGrabFocusData();
ULONG	GetGrabFocusDataSize();
VOID	SetGrabFocusData(ULONG ulRow, ULONG ulCol, LONG lGrabX, LONG lGrabY, ULONG ulFrameID, DOUBLE dFocusScore);
BOOL	GetGrabFocusData(ULONG ulRow, ULONG ulCol, LONG &lGrabX, LONG &lGrabY, ULONG &ulFrameID, DOUBLE &dFocusScore);
DOUBLE	GetGrabFocusScore(ULONG ulRow, ULONG ulCol);

//	for easy reading, one function for one purpose only. remember to delete before and after.
void SetRescanFrameBaseNum(int row, int col, ULONG  ulBaseNum);
bool GetRescanFrameBaseNum(int row, int col, ULONG &ulBaseNum);
void CutRescanFrameBaseNum(int row, int col);
void DelRescanFrameBaseNum();

ULONG SetRescanRemainPoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG  lScanIndex, LONG  lWftX, LONG  lWftY);
bool  GetRescanRemainPoint(UCHAR ucRow, UCHAR ucCol, INT nIndex, LONG &lScanIndex, LONG &lWftX, LONG &lWftY);
void  CutRescanRemainPoint(UCHAR ucRow, UCHAR ucCol, INT nIndex);
void  DelRescanRemainPoint();

VOID	DelRescanRefillData();
VOID	SetRescanRefillData(ULONG ulRow, ULONG ulCol, LONG  lWftX, LONG  lWftY);
BOOL	GetRescanRefillData(ULONG ulRow, ULONG ulCol, LONG &lWftX, LONG &lWftY);

void SetRescanLostData(int row, int col, LONG encX, LONG encY);
bool GetRescanLostData(int row, int col, LONG &encX, LONG &encY);
void CutRescanLostData(int row, int col);
void DelRescanLostData();

//	DEB key die sample with prescan FOV, one time several key dice
void SetScanKeyDie(int row, int col, LONG encX, LONG encY);
bool GetScanKeyDie(int row, int col, LONG &encX, LONG &encY);
void CutScanKeyDie(int row, int col);
void DelScanKeyDie();

