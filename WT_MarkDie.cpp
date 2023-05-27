/////////////////////////////////////////////////////////////////
// WT_Common.cpp : Common functions of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
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
#include "WaferTable.h"
#include "math.h"
#include "FileUtil.h"
#include "MS896A.h"
#include "PrescanUtility.h"
#include "StrInputDlg.h"
#include "WL_Constant.h"
#include "WaferPr.h"
#include "PrescanInfo.h"
#include "MarkDieRegion.h"
#include "MarkDieRegionManager.h"
#include "WaferPr.h"

//#ifndef MS_DEBUG
//	#include "mxml.h"	
//#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CWaferTable::ToggleScanRegionMarkDieCaseLog(IPC_CServiceMessage &svMsg)
{
	WM_CMarkDieRegionManager::Instance()->ToggleScanRegionMarkDieCaseLog();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CWaferTable::MD_SearchNormalDie(LONG &lPosX, LONG &lPosY, LONG &lOffsetX_Count, LONG &lOffsetY_Count, BOOL bDoComp)
{
	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;

	//Init Message
	stSrchInfo.bNormalDie	= TRUE;
	stSrchInfo.lRefDieNo	= 1;
	stSrchInfo.bShowPRStatus = TRUE;
	stSrchInfo.bDisableBackupAlign = FALSE;

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (pWaferPr)
	{
		Sleep(50);	
		pWaferPr->SubSearchCurrentDie(stSrchInfo, stInfo);
	}

	if( (stInfo.bStatus == FALSE) )	// PR error
	{
		return FALSE;
	}

	if( (stInfo.bFullDie == FALSE) )	// empty die
	{
		return FALSE;
	}

	lPosX += stInfo.lX;	
	lPosY += stInfo.lY;
	lOffsetX_Count = stInfo.lX;
	lOffsetY_Count = stInfo.lY;
	if( bDoComp )
	{
		//Move table to updated position
		if (IsWithinWaferLimit(lPosX, lPosY) == FALSE)	// OUT OF WAFER LIMIT
		{
			return FALSE;
		}

		XY_SafeMoveTo(lPosX, lPosY);
		Sleep(20);
	}

	if( (stInfo.bGoodDie==FALSE) )
	{
		return TRUE;	//Defect die found
	}

	return TRUE;
}


BOOL CWaferTable::ScanRegionMarkDie()
{
	X_Sync();
	Y_Sync();
	T_Sync();
	SetEjectorVacuum(FALSE);
	Sleep(100);
	GetEncoderValue();

	LONG lOrigX = GetCurrX();
	LONG lOrigY = GetCurrY();
	LONG lOrigT = GetCurrT();


	WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
	pMarkDieRegionManager->AutoUpdateMarkDieCoord();

	ULONG ulMarkDieRegionRowNum = pMarkDieRegionManager->GetMarkDieRegionRowNum();
	ULONG ulMarkDieRegionColNum = pMarkDieRegionManager->GetMarkDieRegionColNum();
	ULONG ulRegionNum = 0;

	for (ULONG ulRegionRow = 0; ulRegionRow < ulMarkDieRegionRowNum; ulRegionRow++)
	{
		for (ULONG ulRegionCol = 0; ulRegionCol < ulMarkDieRegionColNum; ulRegionCol++)
		{
			LONG ulRegionCol1 = ulRegionCol;
			if (ulRegionRow % 2 == 1)
			{
				ulRegionCol1 = ulMarkDieRegionColNum - ulRegionCol1 - 1;
			}

			ulRegionNum =  ulRegionRow * ulMarkDieRegionColNum + ulRegionCol1;
			WM_CMarkDieRegion *pMarkDieRegion = pMarkDieRegionManager->GetMarkDieRegion(ulRegionNum);
			if ((pMarkDieRegion != NULL) && !pMarkDieRegion->IsEmptyRegion())
			{
				ULONG ulMarkDieRow = pMarkDieRegion->GetMarkDieRow();
				ULONG ulMarkDieCol = pMarkDieRegion->GetMarkDieCol();

				LONG encX = 0, encY = 0;
				double dDieAngle = 0;
				CString szDieScore = "0";
				USHORT usDieState = 0;
			
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

				CString szMsg;
				if (m_pWaferMapManager->IsMapValidDie(ulMarkDieRow, ulMarkDieCol) && GetScanInfo(ulMarkDieRow, ulMarkDieCol, encX, encY, dDieAngle, szDieScore, usDieState))
				{
					if (!IsWithinWaferLimit(encX, encY))
					{
						szMsg = "Mark Die get out of position or return false";
						CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
						continue;
					}

					if (!XY_SafeMoveTo(encX, encY))
					{
						return FALSE;
					}

					//Sleep(30);
					LONG lMakeDieOffsetX = 0, lMakeDieOffsetY = 0;
					if (!MD_SearchNormalDie(encX, encY, lMakeDieOffsetX, lMakeDieOffsetY, FALSE))
					{
						szMsg = "PR search die is empty";
						CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
						continue;
					}

					LONG lUserMarkDieRow = 0, lUserMarkDieCol = 0;
					ConvertAsmToOrgUser(ulMarkDieRow, ulMarkDieCol, lUserMarkDieRow, lUserMarkDieCol);
					CString szLogMsg;
					szLogMsg.Format("RegionRow = %d, RegionCol = %d, row = %d, col = %d, UserRow = %d, UserCol = %d offsetX = %d, offsetY = %d",
									 ulRegionRow, ulRegionCol, ulMarkDieRow, ulMarkDieCol, lUserMarkDieRow, lUserMarkDieCol, lMakeDieOffsetX, lMakeDieOffsetY);
					SaveMarkDieOffset(szLogMsg);
					pMarkDieRegion->SetMarkDieOffset(lMakeDieOffsetX, lMakeDieOffsetY);
				}
				else
				{
					szMsg = "GetScanInfo failure";
					CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
				}
			}
		}
	}

	T_MoveTo(lOrigT, SFM_NOWAIT);
	XY_SafeMoveTo(lOrigX, lOrigY);
	T_Sync();
	Sleep(50);

	return TRUE;
}


VOID CWaferTable::UpdateAllDiePosnByMarkDiePosn()
{
	WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
	ULONG ulMarkDieRegionRowNum = pMarkDieRegionManager->GetMarkDieRegionRowNum();
	ULONG ulMarkDieRegionColNum = pMarkDieRegionManager->GetMarkDieRegionColNum();
	ULONG ulRegionNum = 0;

	for (ULONG ulRegionRow = 0; ulRegionRow < ulMarkDieRegionRowNum; ulRegionRow++)
	{
		for (ULONG ulRegionCol = 0; ulRegionCol < ulMarkDieRegionColNum; ulRegionCol++)
		{
			ulRegionNum =  ulRegionRow * ulMarkDieRegionColNum + ulRegionCol;

			double dDieRow[4], dDieCol[4];
			double dDiePROffsetX[4], dDiePROffsetY[4];

			pMarkDieRegionManager->GetUpperRightCornerPRInfoOfMarkDieRegion(ulRegionNum, dDieRow[UPPER_RIGHT], dDieCol[UPPER_RIGHT], dDiePROffsetX[UPPER_RIGHT], dDiePROffsetY[UPPER_RIGHT]);
			pMarkDieRegionManager->GetUpperLeftCornerPRInfoOfMarkDieRegion(ulRegionNum, dDieRow[UPPER_LEFT], dDieCol[UPPER_LEFT], dDiePROffsetX[UPPER_LEFT], dDiePROffsetY[UPPER_LEFT]);
			pMarkDieRegionManager->GetLowerLeftCornerPRInfoOfMarkDieRegion(ulRegionNum, dDieRow[LOWER_LEFT], dDieCol[LOWER_LEFT], dDiePROffsetX[LOWER_LEFT], dDiePROffsetY[LOWER_LEFT]);
			pMarkDieRegionManager->GetLowerRightCornerPRInfoOfMarkDieRegion(ulRegionNum, dDieRow[LOWER_RIGHT], dDieCol[LOWER_RIGHT], dDiePROffsetX[LOWER_RIGHT], dDiePROffsetY[LOWER_RIGHT]);

			WM_CMarkDieRegion *pMarkDieRegion = pMarkDieRegionManager->GetMarkDieRegion(ulRegionNum);
			for (ULONG i = pMarkDieRegion->GetRegionTopRow(); i < pMarkDieRegion->GetRegionBottomRow(); i++)
			{
				for (ULONG j = pMarkDieRegion->GetRegionLeftCol(); j < pMarkDieRegion->GetRegionRightCol(); j++)
				{
					if (m_pWaferMapManager->IsMapValidDie(i, j))
					{
						double dResultOffsetX = 0, dResultOffsetY = 0;
						pMarkDieRegionManager->CalFourPointInterpolation(dDieRow, dDieCol, dDiePROffsetX, dDiePROffsetY, i, j, dResultOffsetX, dResultOffsetY);

						LONG encX = 0, encY = 0;
						double dDieAngle = 0;
						CString szDieScore = "0";
						USHORT usDieState = 0;

						if (GetScanInfo(i, j, encX, encY, dDieAngle, szDieScore, usDieState))
						{
							SetScanInfo(i, j, encX + _round(dResultOffsetX), encY + _round(dResultOffsetY), dDieAngle, szDieScore, usDieState);
							m_WaferMapWrapper.BatchSetPhysicalPosition(i, j, encX + _round(dResultOffsetX), encY + _round(dResultOffsetY));
						}
					}
				}
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
}