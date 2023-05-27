#pragma once

#include "StringMapFile.h"
#include "io.h"
#include "MS896A_ConStant.h"
#include "PrescanConstant.h"
#include "Math.h"


class CMSPrescanUtility: public CObject
{
public:
	static CMSPrescanUtility* Instance();
	
protected:

	CMSPrescanUtility();
	virtual ~CMSPrescanUtility();

	static CMSPrescanUtility* m_pInstance;

protected:

	BOOL	m_bMultiSrchInIdle;

	ULONG	m_ulSoraaAlign2PrFrameID;
	ULONG	m_ulAlignPrFrameID;
	LONG	m_lAlignMapPosnX;
	LONG	m_lAlignMapPosnY;
	LONG	m_lAlignWftPosnX;
	LONG	m_lAlignWftPosnY;
	LONG	m_lAlignGlobalTheta;

	LONG	m_lPrescanRealignMapCol;
	LONG	m_lPrescanRealignMapRow;
	LONG	m_lPrescanRealignWftPnX;
	LONG	m_lPrescanRealignWftPnY;
	LONG	m_lReAlignGlobalTheta;

	CDWordArray	m_dwaPrescanReRefRow;
	CDWordArray	m_dwaPrescanReRefCol;
	CDWordArray	m_dwaPrescanReRefWfX;
	CDWordArray	m_dwaPrescanReRefWfY;

	CDWordArray	m_dwaReferRow;
	CDWordArray	m_dwaReferCol;
	CDWordArray	m_dwaReferWfX;
	CDWordArray	m_dwaReferWfY;
	UINT		m_unNumOfReferPoints;

	CDWordArray	m_dwaAssistRow;
	CDWordArray	m_dwaAssistCol;
	CDWordArray	m_dwaAssistWfX;
	CDWordArray	m_dwaAssistWfY;
	UINT		m_unAssistPointsNum;

	CDWordArray	m_dwaRescanBaseRow;
	CDWordArray	m_dwaRescanBaseCol;
	CDWordArray	m_dwaRescanBaseWfX;
	CDWordArray	m_dwaRescanBaseWfY;
	UINT		m_unRescanBaseNum;

	CDWordArray	m_dwaReferMapRow;
	CDWordArray	m_dwaReferMapCol;
	CDWordArray	m_dwaReferMapWfX;
	CDWordArray	m_dwaReferMapWfY;
	UINT		m_unReferMapNum;

	BOOL	m_bPrAbnormalError;
	BOOL	m_bBlk2DebugLog;
	BOOL	m_bAutoCycleLog;
	BOOL	m_bVisionCtrlLog;
	BOOL	m_bVisionThrdLog;

	BOOL	m_bPrescanDebug;
	BOOL	m_bPrescanRegionMode;
	BOOL	m_bDummyMapScan;
	BOOL	m_bPrescanAoiWafer;
	BOOL	m_bScanAreaPickMode;
	BOOL	m_bRegionPickMode;
	BOOL	m_bBarWaferScan;

	BOOL		m_bPickDefectDie;
	LONG		m_lSpecailReferDieGrade;
	LONG		m_lNoDieGrade;
	BOOL		m_bMarkDieUnpick;
	LONG		m_lAlignDieRotation;
//	LONG		m_lSearchDieRotation;

	CString		m_szPrescanLogPath;
	CString		m_szPsmMapMcNo;

	BOOL		m_bAoiStopSending;
	BOOL		m_bAoiEndSending;
	BOOL		m_bPrescanEnable;

	BOOL		m_bPickandPlaceMode;

	VOID	InitPrescanVariables();

public:
	VOID  SetRealignPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY);
	VOID  GetRealignPosition(LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY);

	VOID  UpdateReRefPosition(UINT unIndex, LONG lRow, LONG lCol, LONG lEncX, LONG lEncY);
	BOOL  GetReRefPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY);

	ULONG GetAlignPrFrameID();
	VOID  SetAlignPrFrameID(ULONG ulFrameID);

	ULONG GetSoraaAlign2PrFrameID();
	VOID  SetSoraaAlign2PrFrameID(ULONG ulFrameID);

	VOID  SetAlignPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY);
	VOID  GetAlignPosition(LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY);

	VOID  RemoveAllReferPoints();
	VOID  AddAllReferPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY);

	BOOL  GetReferPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY);
	UINT  GetNumOfReferPoints();

	VOID  DelAssistPoints();
	VOID  TrimAssistPoints(ULONG ulNum);
	VOID  AddAssistPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY);
	BOOL  GetAssistPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY);
	UINT  GetAssistPointsNum();

	VOID  DelRescanBasePoints();
	VOID  CutRescanBasePoint(ULONG ulIndex);
	VOID  AddRescanBasePoint(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY);
	BOOL  GetRescanBasePoint(ULONG lIndex, ULONG &lRow, ULONG &lCol, LONG &lEncX, LONG &lEncY);
	UINT  GetRescanBasePointsNum();

	VOID  DelReferMapPoints();
	VOID  AddReferMapPosition(LONG lRow, LONG lCol, LONG lEncX, LONG lEncY);
	BOOL  GetReferMapPosition(ULONG lIndex, LONG &lRow, LONG &lCol, LONG &lEncX, LONG &lEncY);
	UINT  GetReferMapPointsNum();

	VOID  RegionOrderLog(CString szMsg, CONST BOOL bLog=FALSE);
	VOID  PrescanMoveLog(CString szMsg, CONST BOOL bLog=FALSE);
	VOID  ScanImageHistory(CString szMsg, CONST BOOL bNewOne=FALSE);

	VOID  SetPrescanMethod(ULONG ulMethod);
	VOID  SetAoiScanMethod(ULONG ulMethod);
	VOID  SetProberScanMethod(ULONG ulMethod);
	VOID  SetRegionPickMode(CONST BOOL bEnable);

	BOOL  GetPrescanRegionMode();
	BOOL  GetPrescanDummyMap();
	BOOL  GetPrescanAoiWafer();
	BOOL  GetPrescanBarWafer();
	BOOL  GetPrescanAreaPickMode();
	BOOL  GetRegionPickMode();


	VOID  SetPrescanMapIndexIdle(CONST BOOL bIsIdle);
	BOOL  GetPrescanMapIndexIdle();

	VOID  SetPickDefectDie(CONST BOOL bPickDefect);
	BOOL  GetPickDefectDie();
	VOID  SetSpcReferGrade(LONG lGrade);
	LONG  GetSpcReferGrade();
	VOID  SetNoDieGrade(LONG lGrade);
	LONG  GetNoDieGrade();

	VOID SetAlignDieRoation(LONG lRotation);
	LONG GetAlignDieRotation();

//	VOID SetSearchDieRoation(LONG lRotation);
//	LONG GetSearchDieRotation();

	VOID  SetMarkDieUnpick(CONST BOOL bUnpick);
	BOOL  GetMarkDieUnpick();

	VOID  SetPrescanLogPath(CString szLogPath);
	VOID  GetPrescanLogPath(CString &szLogPath);

	VOID SetPrescanGlobalTheta(LONG lTheta);
	LONG GetPrescanGlobalTheta();
	VOID SetRealignGlobalTheta(LONG lTheta);
	LONG GetRealignGlobalTheta();

	VOID SetPrescanDebug(CONST BOOL bEnable);
	BOOL GetPrescanDebug();
	VOID SetBlk2DebugLog(CONST BOOL bEnable);
	VOID SetAutoCycleDebugLog(CONST BOOL bEnable);
	VOID SetVisionCtrlLog(CONST BOOL bEnable);
	VOID SetVisionThrdLog(CONST BOOL bEnable);
	BOOL GetBlk2DebugLog();
	BOOL GetAutoCycleLog();
	BOOL GetVisionCtrlLog();
	BOOL GetVisionThrdLog();
	VOID SetPrAbnormal(CONST BOOL bError, CString szLogMsg = "");
	BOOL GetPrAbnormal();


	VOID SetPickAndPlaceMode(CONST BOOL bEnable);
	VOID SetPrescanEnable(CONST BOOL bEnable);
	BOOL GetPrescanEnable();

	VOID SetAoiStopSending(CONST BOOL bYes);
	BOOL GetAoiStopSending();
	VOID SetAoiEndSending(CONST BOOL bYes);
	BOOL GetAoiEndSending();

	VOID SetPsmMapMcNo(CString szPsmMcNo);
	CString GetPsmMapMachineNo();

	CCriticalSection m_CsMoveLog;
	CCriticalSection m_CsRegionOrderLog;

}; //end class CMSPrescanUtility
