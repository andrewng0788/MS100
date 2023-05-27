/////////////////////////////////////////////////////////////////
// WT_AlignWafer.cpp : Align Wafermap function of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, Dec 21, 2004
//	Revision:	1.00
//
//	By:			???
//				AAA MS896 Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "math.h"
#include "MS_SecCommConstant.h"
#include "FileUtil.h"
#include "WaferPr.h"
#include "PrescanInfo.h"
#include "WL_Constant.h"
#include "WaferLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LONG CWaferTable::ConvertUnitToMotorStep(DOUBLE dUnit)
{
	if (dUnit > 0.000001)
	{
		dUnit += 0.5;
	}
	else
	{
		dUnit -= 0.5;
	}

	return((LONG)(dUnit / (m_dXYRes * 1000)));
}

// to micron
DOUBLE CWaferTable::ConvertMotorStepToUnit(LONG lStep)
{
	return (DOUBLE)lStep * m_dXYRes * 1000.0;
}


LONG CWaferTable::SetAlignStartPoint(BOOL bDoPR, LONG lCurrX, LONG lCurrY, unsigned long ulCurrRow, unsigned long ulCurrCol)
{
	//Move table to Start position
	XY_SafeMoveTo(lCurrX, lCurrY);

	if (bDoPR == TRUE)
	{
		IPC_CServiceMessage stMsg;
		int nConvID = 0;

		REF_TYPE	stInfo;

		SRCH_TYPE	stSrchInfo;

		//Init Message
		stSrchInfo.bShowPRStatus = FALSE;
		stSrchInfo.bNormalDie	= TRUE;
		stSrchInfo.lRefDieNo	= 1;
		stSrchInfo.bDisableBackupAlign = FALSE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (stInfo.bStatus == FALSE)
		{
			return WT_ALN_SEARCH_DIE_ERROR;
		}

		lCurrX	+= stInfo.lX;	
		lCurrY	+= stInfo.lY;
	}
	

	//Klocwork
	//Set Input row & col as current position
	//if ((ulCurrRow < 0) || (ulCurrCol < 0))
	//{
	//	return WT_ALN_OUT_MAPLIMIT;		//Return out map limit
	//}

	m_WaferMapWrapper.SetCurrentPosition(ulCurrRow, ulCurrCol);
	m_ulCurrentRow	= ulCurrRow;
	m_ulCurrentCol	= ulCurrCol;

	//Store postion into temp variable
	m_lCurrent_X = lCurrX;	
	m_lCurrent_Y = lCurrY;

	//Move table to updated position
	XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);

	//Store start point 
	m_ulAlignRow = m_ulCurrentRow;
	m_ulAlignCol = m_ulCurrentCol;
	m_lAlignPosX = m_lCurrent_X;
	m_lAlignPosY = m_lCurrent_Y;

	return WT_ALN_OK;
}


BOOL CWaferTable::GetAlignStartPoint(LONG *lCurrX, LONG *lCurrY, unsigned long *ulCurrRow, unsigned long *ulCurrCol)
{
	*ulCurrRow	= m_ulAlignRow;
	*ulCurrCol	= m_ulAlignCol;
	*lCurrX		= m_lAlignPosX;
	*lCurrY		= m_lAlignPosY;

	return TRUE;
}


LONG CWaferTable::SetAlignIndexPoint(BOOL bDoPR, LONG lDirection, LONG lStepSize, BOOL bCheckMapAndGrade, BOOL bCheckNullGrade)
{
	LONG	lDiePitchX_X = 0;
	LONG	lDiePitchX_Y = 0;
	LONG	lDiePitchY_X = 0;
	LONG	lDiePitchY_Y = 0;
	LONG	lDiff_X = 0;
	LONG	lDiff_Y = 0;
	LONG	lTempRow = 0;
	LONG	lTempCol = 0;
	LONG	lUserRow = 0;
	LONG	lUserCol = 0;
	BOOL	bMapPosNoGrade=FALSE;
	BOOL	bCheckRefDie = FALSE;

	lDiePitchX_X	= GetDiePitchX_X(); 
	lDiePitchX_Y	= GetDiePitchX_Y();
	lDiePitchY_X	= GetDiePitchY_X();
	lDiePitchY_Y	= GetDiePitchY_Y();
	lTempRow		= (LONG)(m_ulCurrentRow);
	lTempCol		= (LONG)(m_ulCurrentCol);

	switch(lDirection)
	{
		case 1:		//Right
			lDiff_X = lStepSize;
			break;

		case 2:		//Up
			lDiff_Y = -lStepSize;
			break;

		case 3:		//Down
			lDiff_Y = lStepSize;
			break;

		default:	//Left
			lDiff_X = -lStepSize;
			break;
	}

	//Update wafermap current position
	lTempRow += lDiff_Y;
	lTempCol += lDiff_X;

	ULONG ulNumberOfRows=0, ulNumberOfCols=0;
	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		return WT_ALN_OUT_MAPLIMIT;
	}

	if ((lTempRow < 0) || (lTempCol < 0))
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT_ALN_OUT_MAPLIMIT 1");	//v3.70T3
		return WT_ALN_OUT_MAPLIMIT;		//Return out map limit
	}
	else if ((lTempRow > (LONG)ulNumberOfRows) || (lTempCol > (LONG)ulNumberOfCols))			//v4.21T3
	{
		return WT_ALN_OUT_MAPLIMIT;		//Return out map limit
	}
	else
	{
		m_ulCurrentRow = (ULONG)(lTempRow); 
		m_ulCurrentCol = (ULONG)(lTempCol);
	}


	if( !(IsBlkFunc2Enable()) )	//Block2		//v3.53
	{
		//v4.21T3	//Osram RESORT
		m_WaferMapWrapper.SetCurrentPosition(m_ulCurrentRow, m_ulCurrentCol);	//v4.15T9	//Put inside IF loop
		
		//If CheckMap&Grade is enabled and is using normal circular wafer (not rectangular wafer)
		//** Note: for rectangular wafer, AutoAlign will search til the end of map dimension 
		//			(not stop/abort	when EMPTY die is reached on map);	//v4.21T3
		if ( bCheckMapAndGrade && !m_bSearchHomeOption)		
		{
			if (m_WaferMapWrapper.ConvertInternalToUser(m_ulCurrentRow, m_ulCurrentCol, lUserRow, lUserCol) == FALSE)
			{
				CString szErr;
				szErr.Format("WT_ALN_OUT_MAPLIMIT - Interal (%d, %d); User (%d, %d)", m_ulCurrentRow, m_ulCurrentCol, lUserRow, lUserCol);
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szErr);	//v3.70T3
				return WT_ALN_OUT_MAPLIMIT;		//Return out map limit
			}

		
			if (m_WaferMapWrapper.IsDieInMap_User(lUserRow, lUserCol) == FALSE)
			{
				CString szTemp;
				szTemp.Format("WT_ALN_OUT_MAPLIMIT 3  (%ld, %ld)", lUserRow, lUserCol);
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);	//v3.70T3
				return WT_ALN_OUT_MAPLIMIT;		//Return out map limit
			}

			//v4.15T3	//Cree China
			if (m_WaferMapWrapper.IsReferenceDie(m_ulCurrentRow, m_ulCurrentCol) == TRUE)
			{
				bCheckRefDie = TRUE;
			}
		}
	}


	//Move table to updated position
	m_lCurrent_X = m_lCurrent_X - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
	m_lCurrent_Y = m_lCurrent_Y - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

	if (XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y) == FALSE)
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT_ALN_OUT_MAPLIMIT 4");	//v3.70T3
		return WT_ALN_OUT_WAFLIMIT;		//return out wafer limit
	}

	if( !(IsBlkFunc2Enable()) )	//Block2		//v3.53
	{
		if ( bCheckNullGrade == TRUE )
		{
			if (IsMapNullBin(m_ulCurrentRow, m_ulCurrentCol))
			{
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT_ALN_IS_NOGRADE");	//v3.70T3
				return WT_ALN_IS_NOGRADE;		//Return no grade
			}
		}
		else
		{
			if (IsMapNullBin(m_ulCurrentRow, m_ulCurrentCol))
			{
				bMapPosNoGrade = bCheckMapAndGrade;
			}
		}
	}
	
	if (bDoPR == TRUE)
	{
		IPC_CServiceMessage stMsg;
		int nConvID = 0;

		REF_TYPE	stInfo;

		SRCH_TYPE	stSrchInfo;

		Sleep(25);

		//Init Message
		stSrchInfo.bShowPRStatus = FALSE;
		if (bCheckRefDie)
			stSrchInfo.bNormalDie	= FALSE;			//v4.15T2	//Cree China
		else
			stSrchInfo.bNormalDie	= TRUE;
		stSrchInfo.lRefDieNo	= 1;
		stSrchInfo.bDisableBackupAlign = FALSE;

		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		m_lCurrent_X += stInfo.lX;	
		m_lCurrent_Y += stInfo.lY;

		//Move table to updated position
		XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);

		if (stInfo.bStatus == FALSE)
		{
			if (bMapPosNoGrade)			
				return WT_ALN_IS_GOOD;							//If no-grade & no-die, OK	//v4.21T3
			else
				return WT_ALN_SEARCH_DIE_ERROR;
		}
		else
		{
			if ( (bMapPosNoGrade == TRUE) && stInfo.bGoodDie)	//If no-grade but real-die found	//v3.57T3
			{
				//return WT_ALN_IS_GOOD;			//Good die found
				return WT_ALN_IS_NOGRADE;			//v3.57T1
			}
			else if (stInfo.bFullDie == TRUE)
			{
				if (stInfo.bGoodDie == FALSE)
				{
					//CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT_ALN_IS_DEFECT");	//v3.70T3
					return WT_ALN_IS_DEFECT;	//Defect die found
				}
			}
			else
			{
				//CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT_ALN_IS_EMPTY");	//v3.70T3
				return WT_ALN_IS_EMPTY;			//Empty die found
			}
		}
	}

	return WT_ALN_IS_GOOD;	//Good die found
}


LONG CWaferTable::AlignNormalWafer(VOID)
{
	LONG	lAlignDieResult	= 1;
	LONG	lUserConfirm = glHMI_CONTINUE;
	LONG	lStepSize = 1;
	ULONG	ulIndexCount=0;
	ULONG	ulJumpCount=0, ulLeftCount=0;	//Klocwork 
	unsigned long ulNumberOfRows = 0; 
	unsigned long ulNumberOfCols = 0;
	CString szTitle, szContent;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	szTitle.LoadString(HMB_WT_ALIGN_WAFER);

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return WT_ALN_ERROR;
	}

	//v4.36		//PLLM MS109
	ULONG ulRightEmptyCount = 0;
	for (INT m=ulNumberOfCols-1; m>0; m--)
	{
		if (IsMapNullBin(m_ulCurrentRow, m))
			ulRightEmptyCount++;
		else
		{
			break;
		}
	}

	//	xu_resort manual auto	to set search index counter alignment,
	LONG lIndexDir = 1;
	ulIndexCount = ulNumberOfCols - m_ulAlignCol - 1 - ulRightEmptyCount;	//v4.36


	//Change index step size = 2 if polygon die is selected
	if ( m_ucDieShape == WPR_HEXAGON_DIE )
	{
		lStepSize = 2;
		ulIndexCount = ((ulNumberOfCols - m_ulAlignCol)/2) - 1;

		ulJumpCount = ulIndexCount;
		ulLeftCount = 0;
	}
	else
	{
		lStepSize = (LONG)m_ucAlignStepSize;
		if (lStepSize <= 0)
			lStepSize = 1;

		if( lStepSize!=0 )	// divide by zero
		{
			ulJumpCount = ulIndexCount / lStepSize;
			ulLeftCount = ulIndexCount % lStepSize;
		}
	}


	if ( ulJumpCount < 1 )
	{
		// step size > indexcount
		lAlignDieResult = SetAlignIndexPoint(TRUE, lIndexDir, (LONG)ulIndexCount);
	}
	else
	{
		//Index jump
		for (INT i=0; i<(LONG)ulJumpCount; i++)
		{
			lAlignDieResult = SetAlignIndexPoint(TRUE, lIndexDir, lStepSize);
			if (ulJumpCount < 30)
				Sleep(200);
			else
				Sleep(100);

			//Display Message if need user to confirm align wafer process
			switch(lAlignDieResult)
			{
				case WT_ALN_IS_EMPTY:
					if (pApp->GetCustomerName() == "OSRAM")		//Osram Penang RESORT mode		//v4.21T3
					{
						szContent = "Map not match!\nPhysical die not found!";
						if ((i==ulJumpCount-1) && (ulLeftCount == 0))	//If last-die in row
							lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
						else
							lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					}
					else
					{
						szContent.LoadString(HMB_WT_ALN_IS_EMPTY);
						lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					}
					break;

				case WT_ALN_IS_NOGRADE:
					szContent.LoadString(HMB_WT_ALN_IS_NO_GRADE);
					lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					break;

				case WT_ALN_OUT_WAFLIMIT:
					szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);
					lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					return WT_ALN_ERROR;

				case WT_ALN_OUT_MAPLIMIT:
					//lUserConfirm = HmiMessage("Map not match\n Hit map limit", "Align Wafer Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					return WT_ALN_OK;

				case WT_ALN_SEARCH_DIE_ERROR:
					return WT_ALN_ERROR;

				default:	//WT_ALN_IS_GOOD
					lUserConfirm = glHMI_CONTINUE;
					break;
			}

			if (lUserConfirm != glHMI_CONTINUE)
			{
				//Stop align prcoss & return -1
				return WT_ALN_ERROR;		
			}
		}
			
		if ( ulLeftCount > 0 )
		{
			lAlignDieResult = SetAlignIndexPoint(TRUE, lIndexDir, ulLeftCount);
			Sleep(500);

			//Display Message if need user to confirm align wafer process
			switch(lAlignDieResult)
			{
				case WT_ALN_IS_EMPTY:
					if (pApp->GetCustomerName() == "OSRAM")		//Osram Penang RESORT mode		//v4.21T3
					{
						szContent = "Map not match!\nPhysical die not found!";
						lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					}
					else
					{
						szContent.LoadString(HMB_WT_ALN_IS_EMPTY);
						lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					}
					break;

				case WT_ALN_IS_NOGRADE:
					szContent.LoadString(HMB_WT_ALN_IS_NO_GRADE);
					lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					break;

				case WT_ALN_OUT_WAFLIMIT:
					szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);
					lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					return WT_ALN_ERROR;

				case WT_ALN_OUT_MAPLIMIT:
					lUserConfirm = glHMI_CONTINUE;
					break;

				case WT_ALN_SEARCH_DIE_ERROR:
					return WT_ALN_ERROR;

				default:	//WT_ALN_IS_GOOD
					lUserConfirm = glHMI_CONTINUE;
					break;
			}

			if (lUserConfirm != glHMI_CONTINUE)
			{
				return WT_ALN_ERROR;	//Stop align prcoss & return -1	
			}

			if (m_bSearchHomeOption)	//If using Corner-Search for rectangular wafer, check last-die
			{
				BOOL bStatus = TRUE;
				bStatus = AlignRectWaferURCorner(0);		//Check RT die only

				if (!bStatus)
				{
					SetStatusMessage("Auto align wafer: fail to LookAround Upper-Right corner dices!");	
					HmiMessage_Red_Yellow("Auto-align fails when checking Upper-Right corner dices!");
					return WT_ALN_ERROR;	//Stop align prcoss & return -1	
				}
			}
		}
		else
		{
			Sleep(500);
			if (m_bSearchHomeOption)	//If using Corner-Search for rectangular wafer, check last-die
			{
				BOOL bStatus = TRUE;
				bStatus = AlignRectWaferURCorner(0);		//Check RT die only

				if (!bStatus)
				{
					SetStatusMessage("Auto align wafer: fail to LookAround Upper-Right corner dices 2!");	
					HmiMessage_Red_Yellow("Auto-align fails when checking Upper-Right corner dices!");
					return WT_ALN_ERROR;	//Stop align prcoss & return -1	
				}
			}
		}
	}

	return WT_ALN_OK;		//Completed align prcoss & return 1
}


LONG CWaferTable::AlignBlockWafer(VOID)
{
	//BLOCKPICK

	LONG	lOldX, lOldY,lT;

	GetEncoder(&lOldX, &lOldY, &lT);

	ResetMapStauts();

	SetJoystickOn(FALSE);

	if ( IsBlkFunc2Enable() || IsPrescanBlkPick() )	//Block2	427TX	5
	{
		LONG	lResult = TRUE;
		m_WaferMapWrapper.EnableSequenceCheck(FALSE);

		lResult = Blk2FindAllReferDiePosn();
		if( lResult==FALSE )
		{
			m_WaferMapWrapper.DeleteAllBoundary();
			XY_SafeMoveTo(lOldX, lOldY);
	
			Sleep(200);
			return WT_ALN_ERROR;
		}
		m_pBlkFunc2->Blk2GetStartDie(&m_lBlkHomeRow, &m_lBlkHomeCol, &m_lBlkHomeX, &m_lBlkHomeY);
	}
	else
	{
		ULONG ulRow, ulMaxRow=0;
		ULONG ulCol, ulMaxCol=0;
	 	if (m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
		{
			for (ulRow=0; ulRow<ulMaxRow; ulRow++)
			{
				for (ulCol=0; ulCol<ulMaxCol; ulCol++)
				{
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_DEFAULT);
				}
			}
		}
	
		m_WaferMapWrapper.EnableSequenceCheck(FALSE);
		Blk1SetAlignParameter();
	
		LONG lX,lY;
		GetAlignStartPoint(&lX, &lY, &ulRow, &ulCol);
		m_pBlkFunc->SetHomeDiePos(ulRow,ulCol);

		//v3.33
		//LOG result
		CString szLog;
		szLog.Format("Auto-Align Blk OK: (%d, %d) at x=%d, y=%d", ulRow, ulCol, lX, lY);
		SetStatusMessage(szLog);
		SetErrorMessage(szLog);
	
		//v3.33
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		if (m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
		{
			LONG lSubRows = ((LONG)ulMaxRow + (pSRInfo->GetSubRows() - 1)) / pSRInfo->GetSubRows();
			LONG lSubCols = ((LONG)ulMaxCol + (pSRInfo->GetSubCols() - 1)) / pSRInfo->GetSubCols();
			m_pBlkFunc->SetRefDieArray(lSubRows, lSubCols);
		}

		m_pBlkFunc->GetStartDie(&m_lBlkHomeRow, &m_lBlkHomeCol, &m_lBlkHomeX, &m_lBlkHomeY);
	}	//	427TX	5
	XY_SafeMoveTo(lOldX, lOldY);

	Sleep(200);
	SetAlignStartPoint(FALSE, m_lBlkHomeX, m_lBlkHomeY, m_lBlkHomeRow, m_lBlkHomeCol);

	//Completed align prcoss & return 1
	return WT_ALN_BLK_OK;
}


LONG CWaferTable::AlignRectWafer(ULONG ulCornerSearchOption)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG	lUserConfirm = glHMI_CONTINUE;
	LONG	lAlignDieResult	= 1;
	LONG	lVertDirection = 3; //0 = Left; 1 = Right; 2 = Up; 3 = Down;
	LONG	lHorzDirection = 1; //0 = Left; 1 = Right; 2 = Up; 3 = Down;
	LONG	lCheckPos[4];		//0 & 1 = 1st pos; 2 & 3 = 2nd pos
	LONG	lX, lY, lT;
	LONG	lStepSize = 1;
	ULONG	i = 0;
	ULONG	ulVertIndexCount = m_ulAlignRow;
	ULONG	ulHorzIndexCount = m_ulAlignCol;
	//ULONG	ulJumpCount, ulLeftCount; 
	unsigned long ulNumberOfRows = 0; 
	unsigned long ulNumberOfCols = 0;
	//CString szTemp;
	CString szTitle, szContent;
	BOOL bUpperLeftCornerIsIncomplete	= FALSE;
	BOOL bLowerRightCornerIsIncomplete	= FALSE;
	BOOL bLowerLeftCornerIsIncomplete	= FALSE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bCheckRefDie = FALSE;

// prescan relative code
	if( IsPrescanEnable() )
		return WT_ALN_OK;

	CString szTemp;
	szTemp.Format("Align RectWafer PITCH CHECK %d %d", m_lPitchX_Tolerance, m_lPitchY_Tolerance);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	if( IsOsramResortMode() && m_ucMapRotation==1 )
	{
		return AlignRectWafer_Resort_090();
	}
	if( IsOsramResortMode() && m_ucMapRotation==2 )
	{
		return AlignRectWafer_Resort_180();	//	426TX	2
	}
	if( IsOsramResortMode() && m_ucMapRotation==3 )
	{
		return AlignRectWafer_Resort_270();
	}
	REF_TYPE	stInfo;

	SRCH_TYPE	stSrchInfo;

	typedef struct {
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;

	LONG lPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);	//v3.70T4

	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return WT_ALN_ERROR;
	}


	//v4.21T8
	szTemp.Format("***** WT: Align RectWafer start (fully-auto): map dimension (%lu, %lu)", ulNumberOfRows, ulNumberOfCols);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);


	ULONG lLLRows = ulNumberOfRows;		//v3.61

	//v4.01T3	//Re-enable for Cree China
	if (IsMapNullBin(ulNumberOfRows-1, 0))
	{
		bLowerLeftCornerIsIncomplete = TRUE;
		lLLRows = lLLRows - 1;
		//v4.13T5
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]	= lLLRows;
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"]	= 0;
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWafer LL corner is a null bin die");
	}

	if (IsMapNullBin(ulNumberOfRows-1, ulNumberOfCols-1))
	{
		bLowerRightCornerIsIncomplete = TRUE;
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWafer LR corner is a null bin die");
		ulNumberOfRows = ulNumberOfRows - 1;
		//v4.13T5
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]	= ulNumberOfRows-2;
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"]	= ulNumberOfCols-1;
	}


	//v3.70T3	//PLLM map checking
	if (bPLLMSpecialFcn)
	{
		if (IsMapNullBin(ulNumberOfRows, m_ulCurrentCol))
		{
			//CString szTemp1;
			//szTemp1.Format("ROW= %d, COL= %d", ulNumberOfRows-1, m_ulCurrentCol);
			//AfxMessageBox(szTemp1, MB_SYSTEMMODAL);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWafer special lumileds");
			//Need to minus one row because PLLM REBEL map has one EMPTY row at bottom of the map
			lLLRows = lLLRows - 1;
			ulNumberOfRows = ulNumberOfRows - 1;
			//bLastRowIsIncomplete = TRUE;
		}
	}

	//Check Move direction
	if ( m_ulAlignRow > (ulNumberOfRows/2) )
	{
		lVertDirection = 2;
		ulVertIndexCount = ulNumberOfRows - m_ulAlignRow;
	}

	if ( m_ulAlignCol > (ulNumberOfCols/2) )
	{
		lHorzDirection = 0;
		ulHorzIndexCount = ulNumberOfCols - m_ulAlignCol;
	}

	lCheckPos[0] = lVertDirection;
	if ( lHorzDirection == 0 )
	{
		lCheckPos[1] = 1;
	}
	else
	{
		lCheckPos[1] = 0;
	}
	
	if (ulCornerSearchOption == 1){
		//check left for lower-left corner
		lHorzDirection = 0;
	}

	lCheckPos[2] = lVertDirection;
	lCheckPos[3] = lHorzDirection;

	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lBpCurrent_X	= m_lCurrent_X;
	LONG lBpCurrent_Y	= m_lCurrent_Y;


	if (ulCornerSearchOption == 1)	//TR -> LL	//anichia004
		szTemp.Format("WT: Align RectWafer UR posn at (%d, %d) with pitch (%d, %d)",	m_lCurrent_X, m_lCurrent_Y, lDiePitchX_X, lDiePitchY_Y);
	else
		szTemp.Format("WT: Align RectWafer UL posn at (%d, %d) with pitch (%d, %d)",	m_lCurrent_X, m_lCurrent_Y, lDiePitchX_X, lDiePitchY_Y);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//v3.70T3
	if (pApp->GetCustomerName() != "Lumileds")	
	{
		//********************************************//
		//** 1. Check number of columns   ***********//
		//********************************************//
		BOOL bRetryLLCorner = FALSE;

		//v4.15T2	//Cree China
		ULONG ulDieStep = m_ucAlignStepSize;	//m_WaferMapWrapper.GetDieStep();
		if (ulDieStep < 1)
			ulDieStep = 1;
		ULONG ulNumOfSteps	= (lLLRows-1) / ulDieStep;
		//If only one row of dices for this wafer, not need to check LL corner -> OSRAM RESORT mode	//v4.21T8
		//if (ulNumOfSteps < 1)
		//	ulNumOfSteps = 1;	//v4.17T2
		ULONG ulLastStep	= (lLLRows-1) % ulDieStep;
		if (ulLastStep > 0)
			ulNumOfSteps = ulNumOfSteps + 1;

		CString szLog;
		szLog.Format("WT: Align RectWafer LL-corner (%ld); NoOfSteps=%lu; LastStep=%lu", lLLRows, ulNumOfSteps, ulLastStep);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szLog);


		for (i=0; i<ulNumOfSteps; i++)			//v4.15T2
		{
			Sleep(lPRDelay);

			LONG lAlignDieResult = 0;
			//lAlignDieResult = SetAlignIndexPoint(TRUE, 3, 1);					//DN
			if ( (i == (ulNumOfSteps-1)) && (ulLastStep > 0) )
				lAlignDieResult = SetAlignIndexPoint(TRUE, 3, ulLastStep);		//DN
			else
				lAlignDieResult = SetAlignIndexPoint(TRUE, 3, ulDieStep);		//DN


			//Display Message if need user to confirm align wafer process
			switch (lAlignDieResult)
			{
			case WT_ALN_OUT_MAPLIMIT:
				//lUserConfirm = HmiMessage("Map not match\n Hit map limit", "Align Wafer Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				//return WT_ALN_OK;
				Sleep(1000);
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWafer err -> out map limit\n");
				return WT_ALN_ERROR;

			case WT_ALN_SEARCH_DIE_ERROR:
				//return WT_ALN_ERROR;
				Sleep(1000);
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWafer err -> search die error\n");
				return WT_ALN_ERROR;

			case WT_ALN_OUT_WAFLIMIT:
				//szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);
				//lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				Sleep(1000);
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWafer err -> out limit\n");
				return WT_ALN_ERROR;

			case WT_ALN_IS_EMPTY:
				//szContent.LoadString(HMB_WT_ALN_IS_EMPTY);
				//lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				//break;
			case WT_ALN_IS_NOGRADE:
				//szContent.LoadString(HMB_WT_ALN_IS_NO_GRADE);
				//lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				//break;

			default:	//WT_ALN_IS_GOOD
				lUserConfirm = glHMI_CONTINUE;
				if (lAlignDieResult != WT_ALN_IS_GOOD)
				{
					szTemp.Format("WT: Align step #%d fail; ERR=%ld", i, lAlignDieResult);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
				}	
				break;
			}

			if (lUserConfirm != glHMI_CONTINUE)
			{
				//Stop align prcoss & return -1
				SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
				szTemp = "WT: AlignRect wafer -> Check no of columns fails";
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
				return WT_ALN_ERROR;		
			}

			//if (i >= (lLLRows-2))
			if (i >= (ulNumOfSteps-1))		//v4.15T2
			{
				Sleep(1000);

				if (lAlignDieResult == WT_ALN_IS_EMPTY)
				{
					if (pApp->GetCustomerName() == "OSRAM")		//Osram Penang RESORT mode		//v4.21T3
						szContent = "Map not match!\nPhysical die not found!";
					else
						szContent.LoadString(HMB_WT_ALN_IS_EMPTY);
					lUserConfirm = HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					if (lUserConfirm != glHMI_CONTINUE)
					{
						//Stop align prcoss & return -1
						SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails 2");
						szTemp = "WT: Align Rect wafer -> Check no of columns fails 2";
						CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
						return WT_ALN_ERROR;		
					}
					bRetryLLCorner = TRUE;
				}
			}
		}

		if (bRetryLLCorner)
		{
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWafer LL Retry");

			m_lCurrent_X = lBpCurrent_X;
			m_lCurrent_Y = lBpCurrent_Y;

			XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);
			Sleep(100);
			Sleep(lPRDelay);


			//********************************************//
			//** 2. Check lower (Left) corners ***********//
			//********************************************//

			m_lCurrent_X = m_lCurrent_X + (lLLRows-1) * lDiePitchY_X;
			m_lCurrent_Y = m_lCurrent_Y - (lLLRows-1) * lDiePitchY_Y;

			/*
			if (bLastRowIsIncomplete)	//v3.55T1	//Fix Cree Rect wafer problem
			{
				m_lCurrent_X = m_lCurrent_X + (ulNumberOfRows) * lDiePitchY_X;
				m_lCurrent_Y = m_lCurrent_Y - (ulNumberOfRows) * lDiePitchY_Y;
			}
			else
			{
				m_lCurrent_X = m_lCurrent_X + (ulNumberOfRows-1) * lDiePitchY_X;
				m_lCurrent_Y = m_lCurrent_Y - (ulNumberOfRows-1) * lDiePitchY_Y;
			}
			*/

			if (XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y) == FALSE)
			{
				SetErrorMessage("WT: Align Rect wafer -> LL corner out of wafer limit!");
				HmiMessage_Red_Yellow("RECT wafer lower-left corner out of wafer limit!");
				szTemp = "WT: Align Rect wafer -> LL corner out of wafer limit!";
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
				return WT_ALN_OUT_WAFLIMIT;		//return out wafer limit
			}

			Sleep(100);
			Sleep(lPRDelay);

			//v4.15T3	//Cree China
			bCheckRefDie = FALSE;
			if (m_WaferMapWrapper.IsReferenceDie(m_ulCurrentRow, m_ulCurrentCol) == TRUE)
			{
				bCheckRefDie = TRUE;
			}

			//Init Message
			stSrchInfo.bShowPRStatus		= FALSE;
			if (bCheckRefDie)		//v4.15T2
				stSrchInfo.bNormalDie		= FALSE;
			else
				stSrchInfo.bNormalDie		= TRUE;
			stSrchInfo.lRefDieNo			= 1;
			stSrchInfo.bDisableBackupAlign	= FALSE;

			stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (!stInfo.bFullDie)
			{
				SetErrorMessage("WT: Align Rect wafer -> Check LL corner fails");
				szTemp = "WT: Align Rect wafer -> Check LL corner fails";
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
				return WT_ALN_ERROR;
			}

			
			//v3.53T1
			if ( IsDiePitchCheck() )
			{
				if ( CheckAlignPitchX(labs(stInfo.lX), 75)==FALSE )
				{
					CString szTemp;
					szTemp.Format("Align RectWafer LL X fail  %d", labs(stInfo.lX));
					SetErrorMessage(szTemp);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
					return WT_ALN_ERROR;
				}
				if ( CheckAlignPitchY(labs(stInfo.lY), 75)==FALSE ) 
				{
					CString szTemp;
					szTemp.Format("Align RectWafer LL Y fail  %d", labs(stInfo.lY));
					SetErrorMessage(szTemp);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
					return WT_ALN_ERROR;
				}
			}
		}

		//v4.15T1
		ULONG ulRow=0, ulCol=0;
		m_WaferMapWrapper.GetCurrentPosition(ulRow, ulCol);
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]	= ulRow;
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"]	= ulCol;

		szLog.Format("WT: LookAround LL corner IsComplete = %d", !bLowerLeftCornerIsIncomplete);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szLog);	//v4.17T2

		if (bLowerLeftCornerIsIncomplete == FALSE)
		{
			szTemp = "WT: Align Rect wafer -> LookAround LL corner to confirm";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			// Use Look-Ahead to check lower-left rect-wafer corners
			//Init Message
			stSrchLrCornerInfo.ulCornerOption = ulCornerSearchOption;
			stMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			
			Sleep(lPRDelay);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLLeftRectWaferCorner", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (stSrchLrCornerInfo.bFovSize != TRUE)		
			{
				SetErrorMessage("WT: Align Rect wafer -> LookAround LL corner FOV checking fails");
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align Rect wafer -> LookAround LL corner FOV checking fails");
				return WT_ALN_ERROR;
			}
			else if (!stSrchLrCornerInfo.bLfResult)
			{
				SetErrorMessage("WT: Align Rect wafer -> LookAround LL corner fails");
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align Rect wafer -> LookAround LL corner fails");
				return WT_ALN_ERROR;
			}
		}
		else 
		{
			szTemp = "WT: Align Rect wafer -> LookAround InComplete LL corner to confirm";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			// Use Look-Ahead to check lower-left rect-wafer corners
			//Init Message
			stSrchLrCornerInfo.ulCornerOption = ulCornerSearchOption;
			stMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			
			Sleep(lPRDelay);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLLeftRectWaferWithInCompleteCorner", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (stSrchLrCornerInfo.bFovSize != TRUE)		
			{
				SetErrorMessage("WT: Align Rect wafer -> LookAround InComplete LL corner FOV checking fails");
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align Rect wafer -> LookAround InComplete LL corner FOV checking fails");
				return WT_ALN_ERROR;
			}
			else if (!stSrchLrCornerInfo.bLfResult)
			{
				SetErrorMessage("WT: Align Rect wafer -> LookAround InComplete LL corner fails");
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align Rect wafer -> LookAround InComplete LL corner fails");
				return WT_ALN_ERROR;
			}
		}
	}


	//********************************************//
	//** 3. Check lower opposite corners *********//
	//********************************************//
	m_lCurrent_X = lBpCurrent_X;
	m_lCurrent_Y = lBpCurrent_Y;

	// Special Handle to Lumileds
	if (pApp->GetCustomerName() == "Lumileds")	
	{
		//v2.83T1
		if (ulCornerSearchOption == 1){
			//Move to lower-left corner
			m_lCurrent_X = m_lCurrent_X + (ulNumberOfCols-2) * lDiePitchX_X + (ulNumberOfRows-2) * lDiePitchY_X;
		}
		else {
		//Move to lower-right corner
			m_lCurrent_X = _round(m_lCurrent_X - (ulNumberOfCols - 2) * lDiePitchX_X - (ulNumberOfRows - 2) * lDiePitchY_X + 0.1 * lDiePitchX_X);
		}

		m_lCurrent_Y = _round(m_lCurrent_Y - (ulNumberOfRows-1) * lDiePitchY_Y - (ulNumberOfCols-2) * lDiePitchX_Y + 0.1 * lDiePitchY_Y);
	}
	else
	{
		if (ulCornerSearchOption == 1){
			//Move to lower-left corner
			m_lCurrent_X = m_lCurrent_X + (ulNumberOfCols-1) * lDiePitchX_X + (ulNumberOfRows-1) * lDiePitchY_X;
		}
		else {
		//Move to lower-right corner
			m_lCurrent_X = m_lCurrent_X - (ulNumberOfCols-1) * lDiePitchX_X - (ulNumberOfRows-1) * lDiePitchY_X;
		}

		m_lCurrent_Y = m_lCurrent_Y - (ulNumberOfRows-1) * lDiePitchY_Y - (ulNumberOfCols-1) * lDiePitchX_Y;

	}

	if (IsWithinWaferLimit(m_lCurrent_X, m_lCurrent_Y) == FALSE)
	{
		SetErrorMessage("WT: Align Rect wafer -> LR corner out of wafer limit!");
		szTemp = "WT: Align Rect wafer -> LR corner out of wafer limit!";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		HmiMessage_Red_Yellow("RECT wafer lower-right corner out of wafer limit!");

		return WT_ALN_OUT_WAFLIMIT;		//return out wafer limit
	}


	//axyz1234
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	ULONG ulCurrRow1=0, ulCurrCol1=0;
	if (1)
	{
		ULONG ulRows=0, ulCols=0;
		m_pWaferMapManager->GetWaferMapDimension(ulRows, ulCols);

		ULONG ulCurrRow, ulCurrCol;
		//Default REBEL map without rotation
		if (pApp->GetCustomerName() == "Lumileds")	
		{
			if (ulCornerSearchOption == 1)	//Start at UR corner?
			{
				ulCurrRow = 1;
				ulCurrCol = ulCols - 1;
				if (IsMapNullBin(ulCurrRow, ulCurrCol))
				{
					ulCurrCol = ulNumberOfCols - 2;
				}
			}
			else	//DEFAULT: start at UL corner and move to LR corner
			{
				//** Start at UL corner **
				ulCurrRow = 1;
				ulCurrCol = 1;
			}

			if (m_WaferMapWrapper.GetRotatedAngle() == 180)
			{
				//DEFAULT FALSH map with 180 deg rotation
				ulCurrRow = 0;
				ulCurrCol = 0;
			}
		}
		else	//v4.01
		{
			//** Start at current position -> LL corner **
			m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
		}


		ULONG ulLastRow = ulCurrRow;
		ULONG ulLastCol = ulCurrCol;
		ULONG ulDieStep = m_ucAlignStepSize;	//m_WaferMapWrapper.GetDieStep();
		if (ulDieStep < 1)
			ulDieStep = 1;
		ULONG ulTargetRow = ulRows - 1;
		ULONG ulTargetCol = ulCols - 1;

		//PLLM Flash map rotated by 180 degree, so ref-die at LR corner instead of REBEL map
		if ( (pApp->GetCustomerName() == "Lumileds") && 	
			 m_WaferMapWrapper.IsReferenceDie(ulRows-1, ulCols-1) )	//Flash		//xyz
		{
			ulTargetRow = ulRows - 2;
			ulTargetCol = ulCols - 2;
		}

		if (ulCornerSearchOption == 1)		//UR corner target to LL corner
		{
			ulTargetRow = ulRows - 1;
			ulTargetCol = 0;
			if (IsMapNullBin(ulTargetRow, ulTargetCol))
			{
				ulTargetCol = 1;
			}
		}
		else
		{
			//Check existence of LR corner-die
			if (IsMapNullBin(ulRows-1, ulCols-1))
			{
				if (m_pWaferMapManager->IsMapHaveBin(ulRows-1, ulCols-2))
				{
					ulTargetCol = ulCols - 2;
				}
				else if (m_pWaferMapManager->IsMapHaveBin(ulRows-2, ulCols-1))
				{
					ulTargetRow = ulRows - 2;
				}
				else
				{
					ulTargetRow = ulRows - 2;
					ulTargetCol = ulCols - 2;
				}
			}
		}


//v4.40T11
CString szLog;
LONG lUserStartRow=0, lUserStartCol=0, lUserTargetRow=0, lUserTargetCol=0;
ConvertAsmToOrgUser(ulCurrRow, ulCurrCol, lUserStartRow, lUserStartCol);
ConvertAsmToOrgUser(ulTargetRow, ulTargetCol, lUserTargetRow, lUserTargetCol);
szLog.Format("Align RectWafer: Start(%ld, %ld); Target(%ld, %ld)",
						lUserStartRow, lUserStartCol, lUserTargetRow, lUserTargetCol);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);


		//v4.14T1
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]	= ulCurrRow; 
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"] = ulTargetCol;

		LONG lRowOffset=0, lColOffset = 0;
		LONG lX, lY, lT;

		LONG lUserRow=0, lUserCol=0;
		ConvertAsmToOrgUser(ulTargetRow, ulTargetCol, lUserRow, lUserCol);
		if (ulCornerSearchOption == 1)	//v4.40T11
		{
			szTemp.Format("WT: Align Rect wafer LL Corner Target=(%ld, %ld); die-step=%lu --Will Not Go To LL Corner", lUserRow, lUserCol, ulDieStep);	
return 1;//Matt
		}
		else
		{
			szTemp.Format("WT: Align Rect wafer LR Corner Target=(%ld, %ld); die-step=%lu", lUserRow, lUserCol, ulDieStep);		
		}
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		//v4.17T2

		ulCurrRow1 = ulTargetRow;	//v4.36
		ulCurrCol1 = ulTargetCol;	//v4.36

		while ((ulCurrRow != ulTargetRow) || (ulCurrCol != ulTargetCol))
		{
			ulLastRow = ulCurrRow;
			ulLastCol = ulCurrCol;

			ulCurrRow = ulCurrRow + ulDieStep;
			if (ulCurrRow > ulTargetRow)
				ulCurrRow = ulTargetRow;
			if (ulCornerSearchOption == 1)		//v4.40T11	//UR -> LL
			{
				if (ulDieStep > ulCurrCol)		//v4.42T8	//PLLM FLASH
				{
					ulCurrCol = ulTargetCol;
				}
				else
				{
					ulCurrCol = ulCurrCol - ulDieStep;
					if (ulCurrCol < ulTargetCol)
						ulCurrCol = ulTargetCol;
				}
			}
			else	//UL -> LR
			{
				ulCurrCol = ulCurrCol + ulDieStep;
				if (ulCurrCol > ulTargetCol)
					ulCurrCol = ulTargetCol;
			}

			lRowOffset = ulCurrRow - ulLastRow;
			lColOffset = ulCurrCol - ulLastCol;

			GetEncoder(&lX, &lY, &lT);
			lX = lX - (lColOffset) * lDiePitchX_X - (lRowOffset) * lDiePitchY_X;
			lY = lY - lRowOffset * lDiePitchY_Y - lColOffset * lDiePitchX_Y;

			//v4.42T8
			ConvertAsmToOrgUser(ulCurrRow, ulCurrCol, lUserRow, lUserCol);
			szTemp.Format("NextDie: I(%lu, %lu); U(%ld, %ld); pos (%ld, %ld)", 
				ulCurrRow, ulCurrCol, lUserRow, lUserCol, lX, lY);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

			if (XY_SafeMoveTo(lX, lY) == FALSE)
			{
				//anichia004
				if (ulCornerSearchOption == 1)	//TR -> LL
				{
					szTemp.Format("ERROR: LL corner indexing out of wafer limit at: X=%ld, Y=%ld", lX, lY);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
					HmiMessage_Red_Yellow("RECT wafer lower-left corner out of wafer limit!");
				}
				else
				{
					szTemp.Format("ERROR: LR corner indexing out of wafer limit at: X=%ld, Y=%ld", lX, lY);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
					HmiMessage_Red_Yellow("RECT wafer lower-right corner out of wafer limit!");
				}
				return WT_ALN_OUT_WAFLIMIT;		//return out wafer limit
			}

			m_WaferMapWrapper.SetCurrentPosition(ulCurrRow, ulCurrCol);
			Sleep(100);

			//v4.15T2	//Cree China
			bCheckRefDie = FALSE;
			if (m_WaferMapWrapper.IsReferenceDie(ulCurrRow, ulCurrCol))
			{
				bCheckRefDie = TRUE;
			}

			//Search die at current posn
			stSrchInfo.bShowPRStatus		= FALSE;
			if (bCheckRefDie)
				stSrchInfo.bNormalDie		= FALSE;	//v4.15T2	//Cree China
			else
				stSrchInfo.bNormalDie		= TRUE;
			stSrchInfo.lRefDieNo			= 1;
			stSrchInfo.bDisableBackupAlign	= FALSE;
			stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

			if (bPLLMSpecialFcn)
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie_PLLM_REBEL", stMsg);	//v4.41T1
			else
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			GetEncoder(&lX, &lY, &lT);
			lX += stInfo.lX;	
			lY += stInfo.lY;
			XY_SafeMoveTo(lX, lY);
		}

		GetEncoder(&lX, &lY, &lT);
		m_lCurrent_X = lX;
		m_lCurrent_Y = lY;
	}

	XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);
	//m_WaferMapWrapper.SetCurrentPosition(ulNumberOfRows-1, m_ulCurrentCol-1);	//v4.01
	Sleep(100);
	Sleep(lPRDelay);

	//v4.15T2	//Cree China
	bCheckRefDie = FALSE;
	m_WaferMapWrapper.SetCurrentPosition(ulCurrRow1, ulCurrCol1);
	if (m_WaferMapWrapper.IsReferenceDie(ulCurrRow1, ulCurrCol1))
	{
		bCheckRefDie = TRUE;
		szTemp.Format("WT: Align Rect wafer -> Jump LR corner last REF die to confirm %d,%d", m_lCurrent_X, m_lCurrent_Y);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}
	else
	{
		if (ulCornerSearchOption == 1)	//v4.40T11
			szTemp.Format("WT: Align Rect wafer -> Jump LL corner last die to confirm %d,%d", m_lCurrent_X, m_lCurrent_Y);
		else
			szTemp.Format("WT: Align Rect wafer -> Jump LR corner last die to confirm %d,%d", m_lCurrent_X, m_lCurrent_Y);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}

	stSrchInfo.bShowPRStatus		= FALSE;
	if (bCheckRefDie)
		stSrchInfo.bNormalDie		= FALSE;	//v4.15T2	//Cree China
	else
		stSrchInfo.bNormalDie		= TRUE;
	stSrchInfo.lRefDieNo			= 1;
	stSrchInfo.bDisableBackupAlign	= FALSE;
	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

	if (bPLLMSpecialFcn)
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie_PLLM_REBEL", stMsg);	//v4.41T1
	else
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	//v3.53T1
	if (stInfo.bFullDie && IsDiePitchCheck() )
	{
		if ( CheckAlignPitchX(labs(stInfo.lX), 75)==FALSE ) 
		{
			CString szTemp;
			szTemp.Format("Align RectWafer LR X fail  %d", labs(stInfo.lX));
			SetErrorMessage(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;
		}
		if ( CheckAlignPitchY(labs(stInfo.lY), 75)==FALSE ) 
		{
			CString szTemp;
			szTemp.Format("Align RectWafer LR Y fail  %d", labs(stInfo.lY));
			SetErrorMessage(szTemp);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;
		}
	}

	if ((stInfo.bStatus == FALSE) || (stInfo.bFullDie == FALSE))		
	{
		//Spiral search for the LR Corner Die, Fully Auto mode		//v3.33T2
		BOOL bFound = FALSE;
		LONG lTempLRightX, lTempLRightY;

		LONG lTempX, lTempY, lTempT;
		GetEncoder(&lTempX, &lTempY, &lTempT);

		//v4.44T1
		szTemp.Format("Spiral-Search corner die at x=%ld, y=%ld", lTempX, lTempY); 
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		for (int i=0; i<8; i++)
		{
			LONG lOffsetX = lDiePitchX_X / 3;
			LONG lOffsetY = lDiePitchY_Y / 3;

			switch (i)
			{
			case 0:
                lTempLRightX = m_lCurrent_X + lOffsetX;
				lTempLRightY = m_lCurrent_Y + lOffsetY;
				break;

			case 1:
				lTempLRightX = m_lCurrent_X + lOffsetX;
				lTempLRightY = m_lCurrent_Y;
				break;

			case 2:
				lTempLRightX = m_lCurrent_X + lOffsetX;
				lTempLRightY = m_lCurrent_Y - lOffsetY;
				break;

			case 3:
				lTempLRightX = m_lCurrent_X;
				lTempLRightY = m_lCurrent_Y - lOffsetY;
				break;

			case 4:
				lTempLRightX = m_lCurrent_X - lOffsetX;
				lTempLRightY = m_lCurrent_Y - lOffsetY;
				break;

			case 5:
				lTempLRightX = m_lCurrent_X - lOffsetX;
				lTempLRightY = m_lCurrent_Y;
				break;

			case 6:
				lTempLRightX = m_lCurrent_X - lOffsetX;
				lTempLRightY = m_lCurrent_Y + lOffsetY;
				break;

			case 7:
				lTempLRightX = m_lCurrent_X;
				lTempLRightY = m_lCurrent_Y + lOffsetY;
				break;
			}
			XY_SafeMoveTo(lTempLRightX, lTempLRightY);
			Sleep(100);
			Sleep(lPRDelay);

			stSrchInfo.bShowPRStatus	= FALSE;
			if (bCheckRefDie)
				stSrchInfo.bNormalDie	= FALSE;	//v4.15T2	//Cree China
			else
				stSrchInfo.bNormalDie	= TRUE;
			stSrchInfo.lRefDieNo		= 1;
			stSrchInfo.bDisableBackupAlign = FALSE;

			stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
			if (bPLLMSpecialFcn)
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie_PLLM_REBEL", stMsg);	//v4.41T1
			else
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if ((stInfo.bStatus == TRUE) && (stInfo.bFullDie == TRUE))
			{
				bFound = TRUE;
				m_lCurrent_X = lTempLRightX;
				m_lCurrent_Y = lTempLRightY;

				//v4.44T1
				szTemp.Format("Spiral-Search corner die found at x=%ld, y=%ld", m_lCurrent_X, m_lCurrent_Y); 
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

				//v3.53T1
				if ( IsDiePitchCheck() )
				{
					LONG lXDiff = lTempX - lTempLRightX - stInfo.lX;
					LONG lYDiff = lTempY - lTempLRightY - stInfo.lY;

					//v4.44T1
					szTemp.Format("Die-Pitch-Check at corner die with x-diff = %ld (%ld), y-diff = %ld (%ld)", 
						lXDiff, GetDiePitchX_X(), lYDiff, GetDiePitchY_Y()); 
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

					if ( CheckAlignPitchX(labs(lXDiff), 75) == FALSE ) 
					{
						m_lCurrent_X += stInfo.lX;	
						m_lCurrent_Y += stInfo.lY;
						XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);

						CString szTemp;
						szTemp.Format("Align RectWafer LR X2 fail  %d", labs(lXDiff));
						SetErrorMessage(szTemp);
						CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
						return WT_ALN_ERROR;
					}
					if ( CheckAlignPitchY(labs(lYDiff), 75) == FALSE )
					{
						m_lCurrent_X += stInfo.lX;	
						m_lCurrent_Y += stInfo.lY;
						XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);

						CString szTemp;
						szTemp.Format("Align RectWafer LR Y2 fail  %d", labs(lYDiff));
						SetErrorMessage(szTemp);
						CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
						return WT_ALN_ERROR;
					}
				}

				break;
			}
		}

		if (!bFound)
		{
			SetErrorMessage("WT: Align Rect wafer -> LR corner no die found!");
			szTemp = "WT: Align Rect wafer -> LR corner no die found!";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;
		}
	}

	m_lCurrent_X += stInfo.lX;	
	m_lCurrent_Y += stInfo.lY;

	//Move table to updated position for PR compensation
	XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);
	Sleep(lPRDelay);

	//Get corner pos
	Sleep(100);
	GetEncoder(&lX, &lY, &lT);
	if (ulCornerSearchOption == 1)	//v4.40T11
		szTemp.Format("WT: Align Rect wafer -> Jump to LL corner Last die after correction %d,%d", lX, lY);
	else
		szTemp.Format("WT: Align Rect wafer -> Jump to LR corner Last die after correction %d,%d", lX, lY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	//v2.82
	// Use Look-Ahead to check lower-right rect-wafer corners
	stSrchLrCornerInfo.ulCornerOption = ulCornerSearchOption;
	stMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
	Sleep(500);

		
	if (ulCornerSearchOption == 1)	//v4.40T11
	{
//AfxMessageBox("LookAheadLLeftRectWaferCorner ...", MB_SYSTEMMODAL);
		szTemp.Format("WT: Align Rect wafer -> LL corner Look Forward check full wafer");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLLeftRectWaferCorner", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		//pllm
		//Retry Look-Around LR Corner
		if (stSrchLrCornerInfo.bFovSize && !stSrchLrCornerInfo.bLfResult) 
		{
			szTemp.Format("WT: Align Rect wafer -> LL corner Look Forward check full wafer (RETRY)");
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			Sleep(500);

			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLLeftRectWaferCorner", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}
//stSrchLrCornerInfo.bLfResult = TRUE;
	}
	else
	{
		if (bLowerRightCornerIsIncomplete == FALSE)
		{
			szTemp.Format("WT: Align Rect wafer -> LR corner Look Forward check full wafer");
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLRightRectWaferCorner", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//pllm
			//Retry Look-Around LR Corner
			if (stSrchLrCornerInfo.bFovSize && !stSrchLrCornerInfo.bLfResult) 
			{
				szTemp.Format("WT: Align Rect wafer -> LR corner Look Forward check full wafer (RETRY)");
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
				Sleep(500);

				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLRightRectWaferCorner", stMsg);
				while(1)
				{
					if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID, stMsg);
						stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
						break;
					}
					else
					{
						Sleep(10);
					}
				}
			}
		}
		else
		{
			szTemp.Format("WT: Align Rect wafer -> LR corner Look Forward check partial wafer");
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);		//v4.14T8
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLRightRectWaferCornerWithIncompleteLastRow", stMsg);
			
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}
	}


	// Check PR LF result
	// If FOV smaller than 3.5, use original table-move method
	if (stSrchLrCornerInfo.bLfResult)
	{
	}
	else if (stSrchLrCornerInfo.bFovSize != TRUE)		
	{
		SetErrorMessage("WT: Align Rect wafer -> Check LR corner FOV fails; try auto-search");

		//Index 2 more die to check PR status
		//for (i=2; i<4; i++)
		for (i=1; i<4; i++)
		{
			if (i == 2)
				continue;	//No need to check UP die

			//i=1 -> RIGHT
			//i=2 -> UP
			//i=3 -> DOWN
			//else -> Left

			Sleep(lPRDelay);
			//lAlignDieResult = SetAlignIndexPoint(TRUE, lCheckPos[i], 1, FALSE);
			lAlignDieResult = SetAlignIndexPoint(TRUE, i, 1, FALSE);

			//Display Message if need user to confirm align wafer process
			switch(lAlignDieResult)
			{
				case WT_ALN_IS_GOOD:
				case WT_ALN_IS_DEFECT:
					szContent.LoadString(HMB_WT_WAFER_HAS_DIE);

					lUserConfirm = HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					break;

				case WT_ALN_OUT_WAFLIMIT:
					szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);

					HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szContent);
					return WT_ALN_ERROR;

				case WT_ALN_SEARCH_DIE_ERROR:
					szTemp = "WT: Align RectWafer search die error";
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
					return WT_ALN_ERROR;

				case WT_ALN_IS_NOGRADE:
					szContent.LoadString(HMB_WT_ALN_IS_NO_GRADE);

					HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szContent);
					return WT_ALN_ERROR;

				case WT_ALN_IS_EMPTY:
					lUserConfirm = glHMI_CONTINUE;
					break;
			}

			if (lUserConfirm != glHMI_CONTINUE)
			{
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Align RectWaferr user quit");
				return WT_ALN_ERROR;
			}

			//Return to last corner position
			m_lCurrent_X = lX;
			m_lCurrent_Y = lY;
			XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);
		}
	}
	else
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Alignment error");	//v3.70T3
		HmiMessage_Red_Yellow("Input wafer alignment error; AUTOBOND is aborted.");
		return WT_ALN_ERROR;
	}

	ULONG ulCheckCorner = WL_CORNER_TL;
	switch( ulCornerSearchOption )
	{
	case WL_CORNER_TL:
	default:	//TL
		ulCheckCorner = WL_CORNER_BR;
		break;

	case WL_CORNER_TR:		//TR
		ulCheckCorner = WL_CORNER_BL;
		break;

	case WL_CORNER_BL:		//BL
		ulCheckCorner = WL_CORNER_TR;
		break;

	case WL_CORNER_BR:		//BR
		ulCheckCorner = WL_CORNER_TL;
		break;
	}

//	CheckRebel_N2R4_Relation(ulCheckCorner);

	//2D Barcode Checking, Fully Auto		//v3.33T1
	if (m_bEnableTwoDimensionsBarcode)
	{
		if (TwoDimensionsBarcodeChecking() == FALSE)	
		{
			//HmiMessage("Auto Align Failed!\nTwo Dimensions Barcode Pattern does not match!");
			CString szErr;
			szErr.Format("WT: 2D barcode checking (auto) fail (%d)\n", m_b2DBarCodeCheckGrade99);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szErr);

			SetAlert_Red_Yellow(IDS_WL_2DBARCODE_FAIL);

			return WT_ALN_NOTOK;
		}
	} //End 2D Barcode Checking

	return WT_ALN_OK;		
}


BOOL CWaferTable::AligningWafer(VOID)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG	lStartAlignX		= 0; 
	LONG	lStartAlignY		= 0; 
	LONG	lThetaStep			= 0;
	DOUBLE	dThetaAngle			= 0.0;
	DOUBLE	dDiffX				= 0.0;
	DOUBLE	dDiffY				= 0.0;
	LONG	lCount				= 0;
	LONG	i = 0;
	LONG	lLHSPhyX			= 0;
	LONG	lLHSPhyY			= 0;
	LONG	lRHSPhyX			= 0;
	LONG	lRHSPhyY			= 0;
	unsigned long ulStartAlignRow = 0; 
	unsigned long ulStartAlignCol = 0;
	//	427TX	5
	LONG	lAlignResult		= WT_ALN_OK;
	CString szTitle, szContent;


	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	//Get user select die position
	m_WaferMapWrapper.GetSelectedPosition(m_ulAlignRow, m_ulAlignCol);
	m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);	
	SetAlignStartPoint(TRUE, m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol);
	// prescan relative code
	SetPrescanAutoAlignMap();

	//v4.13T5
	CString szTemp;
	LONG lUserRow=0, lUserCol=0;
	ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, lUserRow, lUserCol);
	szTemp.Format("Auto-ALIGN Wafer at (%lu, %lu) - User(%ld, %ld)", m_ulAlignRow, m_ulAlignCol, lUserRow, lUserCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//Reset global angle	
	Sleep(50);
	GetEncoderValue();
	SetGlobalTheta();

	try
	{
		//Set SECSGEM Value
		SetGemValue(MS_SECS_SV_WAFER_ID, m_szWaferId);		//3003
		SetGemValue(MS_SECS_SV_SLOT_NO, 1);					//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");		//3005
		SendEvent(SG_CEID_StartAlignment, TRUE);
	}
	catch(...)
	{
		DisplaySequence("SG_CEID_StartAlignment stop failure");
	}

	//Correct Wafer angle if user enable
	if ( m_bEnableGlobalTheta )
	{
		if( IsWT2InUse() )	//	427TX	5
			T2_Home();
		else
			T_Home();
		// 4.24TX
		INT nPos;
		LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

		if (CMS896AApp::m_bMS100Plus9InchOption && lEnableEngageOffset!=0)	//MS100 9Inch	//v4.20		//LOAD/UNLOAD TY pos is not at ENC=0
		{
			LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
			LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			int nPosnT = (int) pApp->GetProfileInt(_T("Settings"), _T("WTEngagePositionT"), 0);
			
			if (nPosnT > lMaxPos)
				nPosnT = 0;
			if (nPosnT < lMinPos)
				nPosnT = 0;

			nPos = GetAutoWaferT();	//include Wafer rotation angle
			
			T1_MoveTo(nPosnT + nPos);
		}
		else
		{
			nPos = GetAutoWaferT();

			T_MoveTo(nPos, SFM_WAIT);	// Move table
		}

		Sleep(200);
		
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName()!="OSRAM" )
		{
			//	427TX	5
			if ( IsBlkFunc2Enable() || IsPrescanBlkPick() )		//Block2
			{
				ULONG ulRowNum = 0, ulColNum = 0;
				m_pWaferMapManager->GetWaferMapDimension(ulRowNum,ulColNum);
				m_ulCurrentRow = ulRowNum/2;
				m_ulCurrentCol = ulColNum/2;
			}

			//v3.03
			LONG lX, lY, lT;
			GetEncoder(&lX, &lY, &lT);
			lRHSPhyX = lLHSPhyX = lX;		
			lRHSPhyY = lLHSPhyY = lY;

			//Go to LHS
			CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
			LONG lLHSIndexCount = pWaferLoader->GetAngleLHSCount();
			for (i = 0; i < lLHSIndexCount; i++)
			{
				if ( (lAlignResult = SetAlignIndexPoint(TRUE, 0, 1)) == WT_ALN_SEARCH_DIE_ERROR)	
				{
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN fails at Global Angle LHS\n");	//v4.13T5
					szContent.LoadString(HMB_WT_AUTO_ALIGN_FAILED);
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					return FALSE;
				}

				lLHSPhyX = m_lCurrent_X;		
				lLHSPhyY = m_lCurrent_Y;
			}

			GetAlignStartPoint(&lStartAlignX, &lStartAlignY, &ulStartAlignRow, &ulStartAlignCol);
			SetAlignStartPoint(TRUE, lStartAlignX, lStartAlignY, ulStartAlignRow, ulStartAlignCol);
			SetAlignIndexPoint(TRUE, 1, 1);
	
			//Go to RHS
			LONG lRHSIndexCount = pWaferLoader->GetAngleRHSCount();
			for (i=0; i<lRHSIndexCount; i++)
			{
				if ( (lAlignResult = SetAlignIndexPoint(TRUE, 1, 1)) == WT_ALN_SEARCH_DIE_ERROR )	
				{
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN fails at Global Angle RHS\n");	//v4.13T5
					szContent.LoadString(HMB_WT_AUTO_ALIGN_FAILED);
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					return FALSE;
				}

				lRHSPhyX = m_lCurrent_X;		
				lRHSPhyY = m_lCurrent_Y;
			}

			//Get start point again		
			GetAlignStartPoint(&lStartAlignX, &lStartAlignY, &ulStartAlignRow, &ulStartAlignCol);

			//Calculate global angle
			dDiffY = (double)(lLHSPhyY - lRHSPhyY);
			dDiffX = (double)(lLHSPhyX - lRHSPhyX);
			dThetaAngle = atan2(dDiffY , dDiffX) * (180 / PI);
			
			RotateWaferTheta(&lStartAlignX, &lStartAlignY, &lThetaStep, dThetaAngle);

			T_Move(lThetaStep, SFM_WAIT);

			XY_SafeMoveTo(lStartAlignX, lStartAlignY);

			//Set AlignStartPoint again	
			SetAlignStartPoint(TRUE, lStartAlignX, lStartAlignY, ulStartAlignRow, ulStartAlignCol);
		}
		else
		{
			if( FindGlobalAngle()==FALSE )
			{
				CMSLogFileUtility::Instance()->WT_LogStatus("Cannot correct wafer angle");
				SetErrorMessage("Cannot correct wafer angle");

				SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
				return FALSE;
			}
		}
	}

	//Update Last Die status & global theta position7
	Sleep(50);
	GetEncoderValue();
	SetGlobalTheta();
	GetPrescanDiePitch();	// align wafer

	//	426TX	2
	//	427TX	5
	if ( IsBlkFunc2Enable() || IsPrescanBlkPick() )		//Block2
	{
		SaveBlkData();
		Blk2SetAlignParameter();
	}

	if( IsScnLoaded() || IsRealignScnnedWafer() )
	{
	}
	else if ( IsBlkFuncEnable() || IsPrescanBlkPick() )
	{
		if (m_bFullRefBlock == FALSE)	//Block2
		{
			SaveBlkData();
		}

		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN BLOCK");	//v4.13T5
		lAlignResult = AlignBlockWafer();
	}
	else
	{
		
		if (( m_bSearchHomeOption == WT_SPIRAL_SEARCH) && (pApp->GetCustomerName() == "Lumileds") 
			&&( pApp->GetProductLine() == "Rebel"))
		{
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U");	//v4.46T7
			lAlignResult = ManualAlignU2UReferenceDie();
		}
		else if ( (pApp->GetCustomerName() == "Lumileds")	&& 
				  (pApp->GetProductLine() == "Rebel")		&&
				  (m_ucPLLMWaferAlignOption == 2) )		//REBEL_U2U
		{
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U 2");//v4.46T25
			lAlignResult = ManualAlignU2UReferenceDie();
		}
		else
		{
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN NORMAL");	//v4.13T5
			lAlignResult = AlignNormalWafer();
		}
	}

	if ( (lAlignResult == WT_ALN_OK) || (lAlignResult == WT_ALN_SCN_OK) || (lAlignResult == WT_ALN_SCN_WARNING) )
	{
		//Return to Align position	
		SetAlignStartPoint(FALSE, m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol);

		//Update Wafertable & wafer map start position
		m_lStart_X	= m_lAlignPosX;
		m_lStart_Y	= m_lAlignPosY;
		m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol); 
	}

	if ( lAlignResult == WT_ALN_BLK_OK )
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN BLK OK");	//v4.13T5

		if ( IsBlkFunc2Enable() || IsPrescanBlkPick() )	//Block2	//	427TX	5
		{
			//Return to Align position
			SetAlignStartPoint(FALSE, m_lBlkHomeX, m_lBlkHomeY, m_lBlkHomeRow, m_lBlkHomeCol);

			//Update Wafertable & wafer map start position
			m_lStart_X	= m_lBlkHomeX;
			m_lStart_Y	= m_lBlkHomeY;
			m_WaferMapWrapper.SetCurrentPosition(m_lBlkHomeRow, m_lBlkHomeCol);
		}
		else
		{
			SetAlignStartPoint(FALSE, m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol);
			m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol); 
			m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
			m_pBlkFunc->SetWaferAlignment(1, (LONG)m_ulAlignRow, (LONG)m_ulAlignCol, m_lAlignPosX, m_lAlignPosY);
		}	//	427TX	5
	}

	//v2.82
	// If using rect wafer, confirm & look-around lower-right corner dice
	BOOL bWaferLoader = (BOOL) ((LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"]);
	if ( bWaferLoader && (lAlignResult == WT_ALN_OK) )
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN OK");	//v4.13T5

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

		if (m_bSearchHomeOption || ( (pApp->GetCustomerName() == "Cree") && (m_bEnableTwoDimensionsBarcode) ) )	//v4.10T5
		{
			if (pApp->GetCustomerName() == "Cree")		//v4.13T5
			{
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN AlignRectWaferLowerCorner LEFT");	//v4.13T5
				if (!AlignRectWaferLowerCorner(FALSE, m_ulAlignRow, m_ulAlignCol))	//LL Corner
				{
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN AlignRectWaferLowerCorner LEFT fail\n");	//v4.13T5
					SetStatusMessage("Auto align wafer is failed in LF of lower-left corner dice");
					return FALSE;
				}
			}

			//v4.46T25
			if ( (pApp->GetCustomerName() == "Lumileds")	&& 
				 (pApp->GetProductLine() == "Rebel")		&&
				 (m_ucPLLMWaferAlignOption == 2) )			//REBEL_U2U
			{
			}
			else
			{
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN AlignRectWaferLowerCorner RIGHT");	//v4.13T5
				if (!AlignRectWaferLowerCorner(TRUE, m_ulAlignRow, m_ulAlignCol))		//LR Corner
				{
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN AlignRectWaferLowerCorner RIGHT fail\n");	//v4.13T5
					SetStatusMessage("Auto align wafer is failed in LF of lower-right corner dice");
					return FALSE;
				}
			}
		}

		//v4.08
		//2D Barcode Checking, Manual Auto Align	
		if (m_bEnableTwoDimensionsBarcode)
		{
			m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);	//v4.15T1	//need to update row/col dor 2D barcode checking

			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN TwoDimensionsBarcodeChecking");	//v4.13T5
			if (TwoDimensionsBarcodeChecking(FALSE) == FALSE)	//v4.15T1
			{
				CString szErr;
				szErr.Format("Auto-ALIGN: 2D barcode checking (manual)  fail (%d)\n", m_b2DBarCodeCheckGrade99);
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szErr);

				HmiMessage("Auto Align Failed!\nTwo Dimensions Barcode Pattern does not match!");
				return FALSE;
			}
		}
	}

	switch(lAlignResult)
	{
	case WT_ALN_OK:
	case WT_ALN_SCN_OK:
	case WT_ALN_BLK_OK:
		break;

	case WT_ALN_XCHECK_ERROR:
		SetStatusMessage("Auto align wafer is finished (with XCheck error)");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN: XCheck fail\n");		//v4.13T5
		szContent.LoadString(HMB_WT_AUTO_ALIGN_WARN_2);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;

	default:
		SetStatusMessage("Auto align wafer is failed");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN: fail\n");				//v4.13T5
		szContent.LoadString(HMB_WT_AUTO_ALIGN_FAILED);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	return TRUE;
}

	
BOOL CWaferTable::ManualAlignGlobalTheta(ULONG ulStartMapRow, ULONG ulStartMapCol, BOOL bHomeT)	//v4.42T12	//PLLM
{
	if( m_bEnableGlobalTheta==FALSE )
	{
		return FALSE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	LONG	lLHSPhyX			= 0;
	LONG	lLHSPhyY			= 0;
	LONG	lRHSPhyX			= 0;
	LONG	lRHSPhyY			= 0;
	LONG	lAlignResult		= 0;
	LONG	lStartAlignX		= 0; 
	LONG	lStartAlignY		= 0; 
	DOUBLE	dThetaAngle			= 0.0;
	DOUBLE	dDiffX				= 0.0;
	DOUBLE	dDiffY				= 0.0;
	LONG lThetaStep				= 0;
	
	LONG lX, lY, lT;
	GetEncoder(&lX, &lY, &lT);
	lRHSPhyX = lLHSPhyX = lX;		
	lRHSPhyY = lLHSPhyY = lY;

	m_ulCurrentRow	= ulStartMapRow;
	m_ulCurrentCol	= ulStartMapCol;
	m_lCurrent_X	= lX;
	m_lCurrent_Y	= lY;

	CString szContent, szTitle;
	szTitle.LoadString(HMB_WT_ALIGN_WAFER);

		if (bHomeT)
		{
			if( IsWT2InUse() )	//	427TX	5
				T2_Home();
			else
				T_Home();

			INT nPos;
			LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

			if (CMS896AApp::m_bMS100Plus9InchOption && lEnableEngageOffset!=0)	//MS100 9Inch	//v4.20		//LOAD/UNLOAD TY pos is not at ENC=0
			{
				LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
				LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

				int nPosnT = (int) pApp->GetProfileInt(_T("Settings"), _T("WTEngagePositionT"), 0);
				if (nPosnT > lMaxPos)
					nPosnT = 0;
				if (nPosnT < lMinPos)
					nPosnT = 0;
				nPos = GetAutoWaferT();	//include Wafer rotation angle
			
				T1_MoveTo(nPosnT + nPos);
			}
			else
			{
				nPos = GetAutoWaferT();
				T_MoveTo(nPos, SFM_WAIT);	// Move table
			}
		}

		Sleep(200);
		
		if ( pApp->GetCustomerName() == "OSRAM" )
		{
			if( FindGlobalAngle()==FALSE )
			{
				CMSLogFileUtility::Instance()->WT_LogStatus("Cannot correct wafer angle");
				SetErrorMessage("Cannot correct wafer angle");

				SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
				return FALSE;
			}
		}
		else
		{
			if ( IsBlkFunc2Enable() || IsPrescanBlkPick() )		//Block2
			{
				ULONG ulRowNum = 0, ulColNum = 0;
				m_pWaferMapManager->GetWaferMapDimension(ulRowNum,ulColNum);
				m_ulCurrentRow = ulRowNum/2;
				m_ulCurrentCol = ulColNum/2;
			}

			//Go to RHS
			CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
			LONG lRHSIndexCount = pWaferLoader->GetAngleRHSCount();
			for (INT i = 0; i < lRHSIndexCount; i++)
			{
				if ( (lAlignResult = SetAlignIndexPoint(TRUE, 1, 1, FALSE, FALSE)) == WT_ALN_SEARCH_DIE_ERROR )	
				{
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN fails at Global Angle RHS\n");	//v4.13T5
					szContent.LoadString(HMB_WT_AUTO_ALIGN_FAILED);
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					return FALSE;
				}

				lRHSPhyX = m_lCurrent_X;		
				lRHSPhyY = m_lCurrent_Y;
			}

			Sleep(500);

			//Calculate global angle
			dDiffY = (double)(lLHSPhyY - lRHSPhyY);
			dDiffX = (double)(lLHSPhyX - lRHSPhyX);
			dThetaAngle = atan2(dDiffY , dDiffX) * (180 / PI);
			RotateWaferTheta(&lX, &lY, &lThetaStep, dThetaAngle);
			XY_SafeMoveTo(lX, lY);
			
			T_Move(lThetaStep, SFM_WAIT);
			SetAlignStartPoint(TRUE, lX, lY, ulStartMapRow, ulStartMapRow);
		}
	Sleep(1000);
	GetEncoderValue();
	m_lStart_X		= GetCurrX();
	m_lStart_Y		= GetCurrY();
	m_lStart_T		= GetCurrT();
	SetGlobalTheta();

	return TRUE;
}


BOOL CWaferTable::AlignRectWaferURCorner(CONST LONG lOption)
{
	INT nConvID = 0;
	CString szTemp;

	typedef struct {
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;

	IPC_CServiceMessage stMsg;
	stSrchLrCornerInfo.ulCornerOption = 0;

	//Option:
	// 1 -> check DR, RT, UP, UL, UR
	// 0 -> check RT
	if (lOption > 0)
		stSrchLrCornerInfo.ulCornerOption = 1;
	else
		stSrchLrCornerInfo.ulCornerOption = 0;
	stMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: LookAhead URightRectWaferCorner start");

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadURightRectWaferCorner", stMsg);
	
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	//Retry Look-Around LR Corner
	if (stSrchLrCornerInfo.bFovSize && !stSrchLrCornerInfo.bLfResult) 
	{
		//szTemp.Format("WT: Align Rect wafer -> LR corner Look Forward check full wafer (RETRY)");
		//CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		return FALSE;
	}

	return TRUE;
}


BOOL CWaferTable::AlignRectWaferLowerCorner(CONST BOOL bIsLRCorner, CONST ULONG ulAlignRow, CONST ULONG ulAlignCol)
{
	//Pre-condition: m_lStart X/Y are updated; m_ulAlign Row/Col are updated

	unsigned long ulNumberOfRows=0, ulNumberOfCols=0;
	BOOL bIsLastRowIncomplete = FALSE;
	m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols);

	//v3.30T1
	//If last row is in-complete row
	if (bIsLRCorner && IsMapNullBin(ulNumberOfRows-1, ulNumberOfCols-1))
	{
		ulNumberOfRows = ulNumberOfRows - 1;
		bIsLastRowIncomplete = TRUE;

		//v4.15T1
		if (IsMapNullBin(ulNumberOfRows-1, ulNumberOfCols-1))
		{
			m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols);
			ulNumberOfCols = ulNumberOfCols - 1;
		}
	}
	else if (!bIsLRCorner && IsMapNullBin(ulNumberOfRows-1, 0))	//v4.15T1
	{
		ulNumberOfRows = ulNumberOfRows - 1;
		bIsLastRowIncomplete = TRUE;
	}

	//xyz
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
	if (bPLLMSpecialFcn && m_WaferMapWrapper.IsReferenceDie(ulNumberOfRows-1, ulNumberOfCols-1))	//PLLM FLASH map with 180 deg rotation
	{
		ulNumberOfRows = ulNumberOfRows - 1;
		ulNumberOfCols = ulNumberOfCols - 1;
	}

	LONG lRowDiff=0, lColDiff=0;
	if (bIsLRCorner)	//LR corner
	{
		lRowDiff = ulNumberOfRows - ulAlignRow - 1;
		lColDiff = ulNumberOfCols - ulAlignCol - 1;
	}
	else				//LL corner
	{
		lRowDiff = ulNumberOfRows - ulAlignRow - 1;
		lColDiff = -1 * ulAlignCol;
	}

CString szTemp3;
szTemp3.Format("AlignRectWaferLowerCorner: RowDiff = %ld, ColDiff = %ld", lRowDiff, lColDiff);
//AfxMessageBox(szTemp3, MB_SYSTEMMODAL);
CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp3);	//v4.15T1


	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();

//CString szTemp;
//szTemp.Format("Andrew: ROW = %d, COL = %d", ulRowDiff, ulColDiff);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;
	IPC_CServiceMessage stMsg;
	BOOL bCheckRefDie = FALSE;		//v4.15T2


	LONG lLowerX=0, lLowerY=0;
	if (bIsLRCorner)
	{
		//Move to lower-right corner
		lLowerX = m_lStart_X - (lColDiff) * lDiePitchX_X - (lRowDiff) * lDiePitchY_X;
		lLowerY = m_lStart_Y - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;
	}
	else
	{
		//Move to lower-left corner
		lLowerX = m_lStart_X + (lColDiff) * lDiePitchX_X + (lRowDiff) * lDiePitchY_X;
		lLowerY = m_lStart_Y - (lRowDiff) * lDiePitchY_Y - (lColDiff) * lDiePitchX_Y;
	}

	//xyz
	//Implement die-step jumping from upperleft to lower-right corner die; for PLLM 
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	//if (nPLLM == PLLM_FLASH)
	if (1)	//bPLLMSpecialFcn)
	{
		LONG ulCurrRow, ulCurrCol;
		//m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
		ulCurrRow = ulAlignRow;
		ulCurrCol = ulAlignCol;

		LONG ulLastRow = ulCurrRow;
		LONG ulLastCol = ulCurrCol;
		ULONG ulDieStep = m_ucAlignStepSize;	//m_WaferMapWrapper.GetDieStep();
		if (ulDieStep < 1)
			ulDieStep = 1;
		LONG ulTargetRow = ulCurrRow + lRowDiff;
		LONG ulTargetCol = ulCurrCol + lColDiff;

		//v4.15T2
		//if (m_WaferMapWrapper.IsReferenceDie(ulTargetRow, ulTargetCol))		//PLLM Flash	
		//{
		//	bIsLastRowIncomplete = TRUE;
		//	ulTargetRow = ulCurrRow + lRowDiff - 1;
		//	ulTargetCol = ulCurrCol + lColDiff;
		//}
		//else 
		if (IsMapNullBin(ulTargetRow, ulTargetCol))
		{
			bIsLastRowIncomplete = TRUE;
			ulTargetRow = ulCurrRow + lRowDiff - 1;
			ulTargetCol = ulCurrCol + lColDiff;
			if (IsMapNullBin(ulTargetRow, ulTargetCol))
			{
				ulTargetRow = ulCurrRow + lRowDiff;
				if (bIsLRCorner)
					ulTargetCol = ulCurrCol + lColDiff - 1;
				else
					ulTargetCol = ulCurrCol + lColDiff + 1;
			}
		}

		//v4.13T5
		CString szTemp;
		LONG lUserRow=0, lUserCol=0;
		ConvertAsmToOrgUser(ulTargetRow, ulTargetCol, lUserRow, lUserCol);
		szTemp.Format("AlignRectWaferLowerCorner #%d at (%lu, %lu) - User(%ld, %ld); step = %lu", 
						bIsLRCorner, ulTargetRow, ulTargetCol, lUserRow, lUserCol, ulDieStep);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

//CString szMsg;
szTemp3.Format("Start CURR (%d, %d);  TARGET User(%d, %d)", ulCurrRow, ulCurrCol, lUserRow, lUserCol);
//AfxMessageBox(szTemp3, MB_SYSTEMMODAL);

		LONG lRowOffset=0, lColOffset = 0;
		LONG lX, lY, lT;

		while ((ulCurrRow != ulTargetRow) || (ulCurrCol != ulTargetCol))
		{
			ulLastRow = ulCurrRow;
			ulLastCol = ulCurrCol;

			ulCurrRow = ulCurrRow + ulDieStep;
			if (ulCurrRow > ulTargetRow)
				ulCurrRow = ulTargetRow;
			if (bIsLRCorner)
			{
				ulCurrCol = ulCurrCol + ulDieStep;
				if (ulCurrCol > ulTargetCol)
					ulCurrCol = ulTargetCol;
			}
			else	//v4.13T5
			{
				ulCurrCol = ulCurrCol - ulDieStep;
				if (ulCurrCol <= ulTargetCol)
					ulCurrCol = ulTargetCol;
			}

			lRowOffset = ulCurrRow - ulLastRow;
			lColOffset = ulCurrCol - ulLastCol;

			GetEncoder(&lX, &lY, &lT);

			lX = lX - (lColOffset) * lDiePitchX_X - (lRowOffset) * lDiePitchY_X;
			lY = lY - (lRowOffset) * lDiePitchY_Y - (lColOffset) * lDiePitchX_Y;

			if (XY_SafeMoveTo(lX, lY) == FALSE)
			{
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("AlignRectWaferLowerCorner #%d out table limit");	//v4.13T5

				if (bIsLRCorner)
					HmiMessage_Red_Yellow("RECT wafer lower-right corner out of wafer limit!");
				else
					HmiMessage_Red_Yellow("RECT wafer lower-left corner out of wafer limit!");
				return FALSE;		//return out wafer limit
			}

			m_WaferMapWrapper.SetCurrentPosition(ulCurrRow, ulCurrCol);
			Sleep(100);

//szTemp3.Format("Current pos (%ld, %ld),  Target = (%ld, %ld)", ulCurrRow, ulCurrCol, ulTargetRow, ulTargetCol);
//AfxMessageBox(szTemp3, MB_SYSTEMMODAL);

			bCheckRefDie = FALSE;
			if (m_WaferMapWrapper.IsReferenceDie(ulCurrRow, ulCurrCol))
				bCheckRefDie = TRUE;

			//Search die at current posn
			stSrchInfo.bShowPRStatus		= FALSE;
			if (bCheckRefDie)
				stSrchInfo.bNormalDie		= FALSE;	//v4.15T2
			else
				stSrchInfo.bNormalDie		= TRUE;
			stSrchInfo.lRefDieNo			= 1;
			stSrchInfo.bDisableBackupAlign	= FALSE;
			stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

			INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			GetEncoder(&lX, &lY, &lT);
			lX += stInfo.lX;	
			lY += stInfo.lY;
			XY_SafeMoveTo(lX, lY);
		}

		//v4.13T5
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapRow"]		= ulTargetRow; 
		(*m_psmfSRam)["WaferPr"]["AutoAlign"]["CurrMapCol"]		= ulTargetCol;

		GetEncoder(&lX, &lY, &lT);
		lLowerX = lX;
		lLowerY = lY;
	}

	XY_SafeMoveTo(lLowerX, lLowerY);
	Sleep(500);

	//xyz
	LONG lLowerRightPosX = lLowerX;
	LONG lLowerRightPosY = lLowerY;

	//v4.15T2
	ULONG ulCurrRow1=0, ulCurrCol1=0;
	m_WaferMapWrapper.GetCurrentPosition(ulCurrRow1, ulCurrCol1);
	bCheckRefDie = FALSE;
	if (m_WaferMapWrapper.IsReferenceDie(ulCurrRow1, ulCurrCol1))
		bCheckRefDie = TRUE;

	//Search current die at lower-right corner position
	stSrchInfo.bShowPRStatus	= FALSE;
	if (bCheckRefDie)
		stSrchInfo.bNormalDie	= FALSE;
	else
		stSrchInfo.bNormalDie	= TRUE;
	stSrchInfo.lRefDieNo		= 1;
	stSrchInfo.bDisableBackupAlign = FALSE;
	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) {
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
			Sleep(10);
	}

	//v3.53T1
	if (stInfo.bFullDie && IsDiePitchCheck() )
	{
		if (  CheckAlignPitchX(labs(stInfo.lX))==FALSE ) 
		{
			CString szTemp;
			if (bIsLRCorner)
				szTemp.Format("AlignRectWaferLRCorner X fail  %d", labs(stInfo.lX));
			else
				szTemp.Format("AlignRectWaferLLCorner X fail  %d", labs(stInfo.lX));
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("AlignRectWaferLowerCorner #%d: X fail");	//v4.13T5
			SetErrorMessage(szTemp);
			HmiMessage_Red_Yellow(szTemp);
			return FALSE;
		}
		if ( CheckAlignPitchY(labs(stInfo.lY))==FALSE )
		{
			CString szTemp;
			if (bIsLRCorner)
				szTemp.Format("AlignRectWaferLRCorner Y fail  %d", labs(stInfo.lY));
			else
				szTemp.Format("AlignRectWaferLLCorner Y fail  %d", labs(stInfo.lY));
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("AlignRectWaferLowerCorner #%d: Y fail");	//v4.13T5
			SetErrorMessage(szTemp);
			HmiMessage_Red_Yellow(szTemp);
			return FALSE;
		}
	}


	if ((stInfo.bStatus == FALSE) || (stInfo.bFullDie == FALSE))		
	{
		szTemp3.Format("AlignRectWaferLowerCorner #%d: corner-die not found", bIsLRCorner);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp3);	//v4.13T5
	}


	// Do PR compensation
	if (stInfo.bStatus)
	{
		lLowerX += stInfo.lX;	
		lLowerY += stInfo.lY;
	}
	XY_SafeMoveTo(lLowerX, lLowerY);
	Sleep(500);

	// Use Look-Ahead to check lower-right rect-wafer corners
	typedef struct {
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;

	stSrchLrCornerInfo.ulCornerOption = 0;
	stMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

	if (bIsLastRowIncomplete == FALSE)
	{
		if (bIsLRCorner)
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLRightRectWaferCorner", stMsg);
		else
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLLeftRectWaferCorner", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}
	else
	{
		if (bIsLRCorner)
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLRightRectWaferCornerWithIncompleteLastRow", stMsg);
		else
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLLeftRectWaferWithInCompleteCorner", stMsg);

		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

	}


	//xyz
	//Implement lower-right corner checking of RT & DN die (absense) only when FOV < 3.3 
	if (stSrchLrCornerInfo.bFovSize != TRUE)
	{
		LONG lX = m_lCurrent_X;
		LONG lY = m_lCurrent_Y;
		m_lCurrent_X = lLowerX;
		m_lCurrent_Y = lLowerY;

		SetErrorMessage("PR Look-around result disabled becasue of small FOV size -> use table move SEARCH");
		for (INT i=1; i<4; i++)
		{
			if (i == 2)
				continue;	//No need to check UP die

			//i=1 -> RIGHT
			//i=2 -> UP
			//i=3 -> DOWN
			//else -> Left

			CString szContent, szTemp;
			CString szTitle = _T("Align LL Corner die");
			LONG lUserConfirm = glHMI_CONTINUE;

			//lAlignDieResult = SetAlignIndexPoint(TRUE, lCheckPos[i], 1, FALSE);
			LONG lAlignDieResult = SetAlignIndexPoint(TRUE, i, 1, FALSE);

			//Display Message if need user to confirm align wafer process
			switch(lAlignDieResult)
			{
				case WT_ALN_IS_GOOD:
				case WT_ALN_IS_DEFECT:
					szContent.LoadString(HMB_WT_WAFER_HAS_DIE);
					HmiMessage_Red_Yellow(szContent, szTitle);
					return FALSE;

				case WT_ALN_OUT_WAFLIMIT:
					szContent.LoadString(HMB_WT_OUT_WAFER_LIMIT);
					HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szContent);
					return FALSE;

				case WT_ALN_SEARCH_DIE_ERROR:
					szTemp = "WT: Align RectWafer search die error";
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
					return FALSE;

				case WT_ALN_IS_NOGRADE:
					szContent.LoadString(HMB_WT_ALN_IS_NO_GRADE);
					HmiMessage_Red_Yellow(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szContent);
					return FALSE;

				case WT_ALN_IS_EMPTY:
					//lUserConfirm = glHMI_CONTINUE;
					break;
			}

			//if (lUserConfirm != glHMI_CONTINUE)
			//{
			//	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: Manual Align RectWafer LR corner with small FOV -> FAIL and quit");
			//	return FALSE;
			//}

			m_lCurrent_X = lLowerX;
			m_lCurrent_Y = lLowerY;
			XY_SafeMoveTo(m_lCurrent_X, m_lCurrent_Y);
			Sleep(100);
		}
		
		m_lCurrent_X = lX;
		m_lCurrent_Y = lY;
	}


	// Move back to original WT position 
	XY_SafeMoveTo(m_lStart_X, m_lStart_Y);

	// Check PR LA result
	if (stSrchLrCornerInfo.bFovSize != TRUE)
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("AlignRectWaferLowerCorner #%d: small FOV");	//v4.13T5
		SetErrorMessage("PR Look-around result disabled becasue of small FOV size");
		return TRUE;
	}
	else if (stSrchLrCornerInfo.bLfResult != TRUE)
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("AlignRectWaferLowerCorner PR LookAround fail");	//v4.13T5

		if (bIsLRCorner)
		{
			SetErrorMessage("PR Look-around result fails at lower-right corner die!  Auto align wafer aborted");		
			HmiMessage("PR Look-around result fails at lower-right corner die!  Auto align wafer aborted");
		}
		else
		{
			SetErrorMessage("PR Look-around result fails at lower-Left corner die!  Auto align wafer aborted");		
			HmiMessage("PR Look-around result fails at lower-Left corner die!  Auto align wafer aborted");
		}
		return FALSE;
	}

	szTemp3.Format("AlignRectWaferLowerCorner #%d: Done & OK", bIsLRCorner);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp3);	//v4.13T5
	return TRUE;
}


//check whether the 2D Barcode patterns in map match those in wafer		//v3.33T1
BOOL CWaferTable::TwoDimensionsBarcodeChecking(BOOL bAuto)	
{
	ULONG ulNumOfRows = 0, ulNumOfCols = 0;
	LONG lStartX, lStartY , lStartT;

	BOOL bPatternCheckOK = FALSE;
	if (!m_b2DBarCodeCheckGrade99)		//Check grade-99	//v3.59	//Cree China
		bPatternCheckOK = TRUE;

	unsigned long ulRowDiff;
	unsigned long ulColDiff;

	LONG lTargetX;
	LONG lTargetY;
	
	REF_TYPE	stInfo;

	SRCH_TYPE	stSrchInfo;

	//Init Message
	IPC_CServiceMessage stMsg;
	stSrchInfo.bShowPRStatus	= FALSE;
	stSrchInfo.bNormalDie		= TRUE;
	stSrchInfo.lRefDieNo		= 1;
	stSrchInfo.bDisableBackupAlign = FALSE;

	GetEncoder(&lStartX, &lStartY, &lStartT);

	m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols);
	
	INT nLastDieRow, nLastDieCol;

	for (INT i = 0; i < (INT)ulNumOfRows; i++)
	{
		for (INT j = 0; j < (INT)ulNumOfCols; j++)
		{
			if  (m_pWaferMapManager->IsMapHaveBin(i, j))
			{
				nLastDieRow = i;
				nLastDieCol = j;
			}
		}
	}

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	INT nNoDieCount = 0;		//v3.59
	INT nNoOfGrade99Die = 0;

	//v4.15T1
	CString szTemp;
	ULONG ulCurrRow=0, ulCurrCol=0;
	LONG lUserRow=0, lUserCol=0;
	m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);
	if (!bAuto)
	{
		ConvertAsmToOrgUser(ulCurrRow, ulCurrCol, lUserRow, lUserCol);
		szTemp.Format("WT: 2D barcode checking start at MAP I(%lu, %lu)  U(%lu, %lu)", ulCurrRow, ulCurrCol, lUserRow, lUserCol);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	}


	for (INT i = 0; i < (INT)ulNumOfRows; i++)
	{
		for (INT j = 0; j < (INT)ulNumOfCols; j++)
		{
			//if ((i == nLastDieRow) && (j == nLastDieCol))
			//	break;

			if (m_b2DBarCodeCheckGrade99)		//Check grade-99	//v3.59	//Cree China
			{
				if (m_WaferMapWrapper.GetGrade(i, j) == (m_WaferMapWrapper.GetGradeOffset() + 98))
				{
					//Do not check the 4 corners
					if ((i==0) && (j==0))
						continue;
					else if ((i==0) && (j==ulNumOfCols-1))
						continue;
					else if ((i==ulNumOfRows-1) && (j==0))
						continue;
					else if ((i==ulNumOfRows-1) && (j==ulNumOfCols-1))
						continue;

					nNoOfGrade99Die++;

					if (bAuto)					//v4.15T1
					{
						ulRowDiff = i - 0;
						ulColDiff = j - 0;
					}
					else
					{
						ulRowDiff = i - ulCurrRow;
						ulColDiff = j - ulCurrCol;
					}

					lTargetX = m_lStart_X - (ulColDiff) * lDiePitchX_X - (ulRowDiff) * lDiePitchY_X;
					lTargetY = m_lStart_Y - (ulRowDiff) * lDiePitchY_Y - (ulColDiff) * lDiePitchX_Y;

					ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
					szTemp.Format("WT: 2D barcode checking Grade99 at MAP I(%lu, %lu)  U(%lu, %lu) - Diff(%ld, %ld)", 
									i, j, lUserRow, lUserCol, ulRowDiff, ulColDiff);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

					if (!XY_SafeMoveTo(lTargetX, lTargetY))
					{
						CMSLogFileUtility::Instance()->WPR_AlignWaferLog("WT: 2D barcode checking out wafer limit");
						bPatternCheckOK = FALSE;
						XY_SafeMoveTo(m_lStart_X, m_lStart_Y);
						Sleep(100);
						return bPatternCheckOK;
					}

					m_WaferMapWrapper.SetCurrentPosition(i, j);		//v3.59
					Sleep(500);

					stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
					INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
					while(1)
					{
						if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
							break;
						}
						else
						{
							Sleep(10);
						}
					}

					if (!stInfo.bFullDie)
					{
						CString szErr;
						LONG lUserRow=0, lUserCol=0;
						ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
						szErr.Format("WT: 2D barcode-check 98 no-die found at (%d, %d)", lUserCol, lUserRow);
						CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szErr);

						nNoDieCount++;
						if (nNoDieCount >= 2)		//From Cree requirement, if 2 empty die posn found -> alignment OK
						{
							bPatternCheckOK = TRUE;
							break;
						}
					}
					else
					{
						bPatternCheckOK = FALSE;
					}
				}	
			}
			else								//else Check EMPTY grade
			{
				if (IsMapNullBin(i, j))
				{
					if (bAuto)					//v4.15T1
					{
						ulRowDiff = i - 0;
						ulColDiff = j - 0;
					}
					else
					{
						ulRowDiff = i - ulCurrRow;
						ulColDiff = j - ulCurrCol;
					}

					lTargetX = m_lStart_X - (ulColDiff) * lDiePitchX_X - (ulRowDiff) * lDiePitchY_X;
					lTargetY = m_lStart_Y - (ulRowDiff) * lDiePitchY_Y - (ulColDiff) * lDiePitchX_Y;

					//v4.15T1
					ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
					szTemp.Format("WT: 2D barcode checking at MAP I(%lu, %lu)  U(%lu, %lu) - Diff(%ld, %ld)", 
									i, j, lUserRow, lUserCol, ulRowDiff, ulColDiff);
					CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

					if (!XY_SafeMoveTo(lTargetX, lTargetY))
					{
						bPatternCheckOK = FALSE;
						XY_SafeMoveTo(m_lStart_X, m_lStart_Y);
						Sleep(100);
						return bPatternCheckOK;
					}

					m_WaferMapWrapper.SetCurrentPosition(i, j);	
					Sleep(500);

					stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
					INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
					while(1)
					{
						if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
							break;
						}
						else
						{
							Sleep(10);
						}
					}

					if (!stInfo.bFullDie)
					{
						//bPatternCheckOK = TRUE;
					}
					else
					{
						CString szErr;
						LONG lUserRow=0, lUserCol=0;
						ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
						szErr.Format("WT: 2D barcode-check die found at (%d, %d)", lUserCol, lUserRow);
						CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szErr);

						bPatternCheckOK = FALSE;
					}
				}	
			}
		}

		if (m_b2DBarCodeCheckGrade99 && bPatternCheckOK)		//Check grade-99	//v3.59	//Cree China
		{
			break;	//return OK if any 2 Grade-98 posns found no-die
		}
		else if (!m_b2DBarCodeCheckGrade99 && !bPatternCheckOK)
		{
			break;	//Return FALSE immediately if any no-die map posn found a die through PR
		}
	}


	//Still OK if smaller than 2 Grade98 dices on wafer
	if (m_b2DBarCodeCheckGrade99 && !bPatternCheckOK)		//Check grade-99	//v3.59	//Cree China
	{
		if ((nNoOfGrade99Die <= 2) && (nNoOfGrade99Die == nNoDieCount))
			bPatternCheckOK = TRUE;		
	}

	XY_SafeMoveTo(m_lStart_X, m_lStart_Y);
	Sleep(100);
	return bPatternCheckOK;
}


//================================================================
// AlignAllRefDiePositions()
//   Created-By  : Andrew Ng
//   Date        : 3/3/2008 11:43:57 AM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferTable::AlignAllRefDiePositions()
{
	//Get curr map position
	ULONG ulCurrRow=0, ulCurrCol=0;
	GetMapAlignHomeDie(ulCurrRow, ulCurrCol);

	//Get current encoder position
	LONG lCurrX, lCurrY, lCurrT;
	GetEncoder(&lCurrX, &lCurrY, &lCurrT);

	//Get die pitch from WaferPr
	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();

	REF_TYPE	stInfo;

	SRCH_TYPE	stSrchInfo;

	//Init Message
	IPC_CServiceMessage stMsg;
	stSrchInfo.bShowPRStatus	= FALSE;
	stSrchInfo.bNormalDie		= FALSE;
	stSrchInfo.lRefDieNo		= 1;
	stSrchInfo.bDisableBackupAlign = FALSE;

	ULONG ulNumOfRows = 0, ulNumOfCols = 0;
	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
		return FALSE;

	LONG lTargetX=0, lTargetY=0;
	ULONG ulRowDiff, ulColDiff;

	//Walk through map to search for any ref-die points
	for (ULONG i=0; i<ulNumOfRows; i++)
	{
		for (ULONG j=0; j<ulNumOfCols; j++)
		{
			if (m_WaferMapWrapper.IsReferenceDie(i, j))
			{
				if ( (i == ulCurrRow) && (j == ulCurrCol) )
					continue;

				ulRowDiff	= i - ulCurrRow;
				ulColDiff	= j - ulCurrCol;
				lTargetX	= lCurrX - ulColDiff * lDiePitchX_X - ulRowDiff * lDiePitchY_X;
				lTargetY	= lCurrY - ulRowDiff * lDiePitchY_Y - ulColDiff * lDiePitchX_Y;

				if (XY_SafeMoveTo(lTargetX, lTargetY) == FALSE)
				{
					HmiMessage_Red_Yellow("REF die position out of wafer limit!!");
					return FALSE;		//return out wafer limit
				}

				Sleep(500);

				//Search ref die
				stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
				INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
				while(1)
				{
					if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID, stMsg);
						stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
						break;
					}
					else
					{
						Sleep(10);
					}
				}

				//Fail if SrchDie result no good or no ref die found!!
				if (!stInfo.bStatus || !stInfo.bFullDie)
				{
					LONG lUserRow=0, lUserCol=0;
					ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
					CString szErr;
					szErr.Format("Wafermap ALL-REF-DIE checking fails at REF(%d, %d): Ref-die is not found", 
						lUserRow, lUserCol);
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);
					return FALSE;
				}

				//Update ref-die physical position in map memory
				LONG lRefX = lTargetX + stInfo.lX;	
				LONG lRefY = lTargetY + stInfo.lY;
				m_WaferMapWrapper.SetPhysicalPosition(i, j, lRefX, lRefY);
			}
		}
	}

	XY_SafeMoveTo(lCurrX, lCurrY);
	Sleep(1000);
	return TRUE;
}

BOOL CWaferTable::FindLeftmostCreeHorz4RefDice(LONG& lEncX, LONG& lEncY)	//v3.92		//Cree US Waffle Pack machine buyoff
{
	//Get current encoder position
	LONG lCurrX, lCurrY, lCurrT;
	GetEncoder(&lCurrX, &lCurrY, &lCurrT);

	//Get die pitch from WaferPr
	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();

	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;

	//Init Message
	IPC_CServiceMessage stMsg;
	stSrchInfo.bShowPRStatus	= FALSE;
	stSrchInfo.bNormalDie		= FALSE;
	stSrchInfo.lRefDieNo		= 1;
	stSrchInfo.bDisableBackupAlign = FALSE;


	LONG lTargetX, lTargetY;
	LONG lLastX = 0, lLastY = 0;

	//Walk through map to left to search for any ref-die points
	for (int j=1; j<4; j++)
	{
		GetEncoder(&lLastX, &lLastY, &lCurrT);

		lTargetX	= lCurrX + j * lDiePitchX_X;
		lTargetY	= lCurrY + j * lDiePitchX_Y;

		if (XY_SafeMoveTo(lTargetX, lTargetY) == FALSE)
		{
			HmiMessage_Red_Yellow("REF die position out of wafer limit!!");
			return FALSE;		//return out wafer limit
		}

		Sleep(500);

		//Search ref die
		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		//Fail if SrchDie result no good or no ref die found!!
		if (!stInfo.bStatus || !stInfo.bFullDie)
		{
			if (j > 1)
			{
				XY_SafeMoveTo(lLastX, lLastY);	//Move back to last position of ref-die found
				Sleep(1000);
				lEncX = lLastX;
				lEncY = lLastY;
				return TRUE;		//Update HOME die position beause more ref-die are found on left side!
			}
			else
			{
				XY_SafeMoveTo(lCurrX, lCurrY);
				Sleep(1000);
				return FALSE;		//Use original Spiral-Search HOME position
			}
		}
	}

	XY_SafeMoveTo(lCurrX, lCurrY);
	Sleep(1000);
	return FALSE;		//Use original Spiral-Search HOME position
}


//xyz
LONG CWaferTable::CheckRebel_N2RC_Position(ULONG ulCornerType)
{
//	if( PLLM_REBEL_EXTRA_N2RC==0 )
//	{
//		return 1;
//	}
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel()==FALSE )
	{
		return 1;
	}

	if( ulCornerType!=m_ulCornerSearchOption )
	{
		return 0;
	}

	GrabAndSaveImage(WPR_NORMAL_DIE, 1, WPR_GRAB_SAVE_IMG_N2RC);

	LONG lReturn = 1;
	GetEncoderValue();
	LONG siOrigX = GetCurrX();
	LONG siOrigY = GetCurrY();
	LONG lRSizeX = 0, lRSizeY = 0;
	typedef struct 
	{
		LONG	x;
		LONG	y;
	} RDIE_SIZE;

	IPC_CServiceMessage stMsg;
	RDIE_SIZE stDieSize;
	UCHAR ucPrID = 1;
	stMsg.InitMessage(sizeof(UCHAR), &ucPrID);
	INT	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GetReferDieSize", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(RDIE_SIZE), &stDieSize);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	lRSizeX = stDieSize.x;
	lRSizeY = stDieSize.y;

	LONG lPitchX = GetDiePitchX_X() + lRSizeX/2 - GetDieSizeX()/2;
	LONG lPitchY = GetDiePitchY_Y() + lRSizeY/2 - GetDieSizeY()/2;
	LONG lMoveX = siOrigX;
	LONG lMoveY = siOrigY;
	CString szMsg;
	switch (ulCornerType)
	{
	case WL_CORNER_TL:
	default:	//TL
		lMoveX += lPitchX;
		lMoveY += lPitchY;
		szMsg = "TL ";
		break;

	case WL_CORNER_TR:		//TR
		lMoveX -= lPitchX;
		lMoveY += lPitchY;
		szMsg = "TR ";
		break;

	case WL_CORNER_BL:		//BL
		lMoveX += lPitchX;
		lMoveY -= lPitchY;
		szMsg = "BL ";
		break;

	case WL_CORNER_BR:		//BR
		lMoveX -= lPitchX;
		lMoveY -= lPitchY;
		szMsg = "BR ";
		break;
	}

	XY_SafeMoveTo(lMoveX, lMoveY);
	Sleep(100);
	LONG lOffsetX = lMoveX, lOffsetY = lMoveY;
	lReturn = WftMoveSearchReferDie(lMoveX, lMoveY, FALSE, ucPrID);
	if( lReturn==FALSE )
	{
		Sleep(200);
		lReturn = WftMoveSearchReferDie(lMoveX, lMoveY, FALSE, ucPrID);
	}
	if( lReturn )
	{
		lOffsetX = labs(lOffsetX - lMoveX);
		lOffsetY = labs(lOffsetY - lMoveY);
		if( labs(lOffsetX)>labs(GetDieSizeX()*2/3) ||
			labs(lOffsetY)>labs(GetDieSizeY()*2/3) )
		{
			szMsg += "Corner Refer die offset over limit.\nWafer alignment fail!";
			HmiMessage_Red_Back(szMsg, "PLLM");
			lReturn = -1;
		}
	}
	else
	{
	//	if( HmiMessage_Red_Back("Refer die missing.\nContinue align wafer?", "PLLM", glHMI_MBX_YESNO)==glHMI_NO )
		szMsg += "Corner Refer die missing.\nWafer alignment fail!";
		HmiMessage_Red_Back(szMsg, "PLLM");
		{
			lReturn = 0;
		}
	}
	Sleep(100);
	XY_SafeMoveTo(siOrigX, siOrigY);
	Sleep(100);
	if( lReturn<1 )
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}

	return lReturn;
}

LONG CWaferTable::CheckRebel_N2R4_Relation(ULONG ulCornerType)
{
//	if( PLLM_REBEL_EXTRA_N2R4==0 )
//	{
//		return 1;
//	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->IsPLLMRebel()==FALSE )
	{
		return 1;
	}

	GrabAndSaveImage(WPR_NORMAL_DIE, 1, WPR_GRAB_SAVE_IMG_N2RX);

	LONG lReturn = 1;
	GetEncoderValue();
	LONG siOrigX = GetCurrX();
	LONG siOrigY = GetCurrY();
	LONG lRSizeX = 0, lRSizeY = 0;
	typedef struct 
	{
		LONG	x;
		LONG	y;
	} RDIE_SIZE;

	IPC_CServiceMessage stMsg;
	UCHAR ucPrID = 2;
	if( ulCornerType==WL_CORNER_TR || ulCornerType==WL_CORNER_BR )
		ucPrID = 3;
	stMsg.InitMessage(sizeof(UCHAR), &ucPrID);
	RDIE_SIZE stDieSize;
	INT	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GetReferDieSize", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(RDIE_SIZE), &stDieSize);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	lRSizeX = stDieSize.x;
	lRSizeY = stDieSize.y;

	LONG lPitchX = GetDiePitchX_X() + lRSizeX/2 - GetDieSizeX()/2;
	LONG lPitchY = lRSizeY/2 - GetDieSizeY()/2;
	LONG lMoveX = siOrigX;
	LONG lMoveY = siOrigY;
	CString szMsg;
	switch (ulCornerType)
	{
	case WL_CORNER_TL:
	default:	//TL
		lMoveX += lPitchX;
		lMoveY -= lPitchY;
		szMsg = "TL ";
		break;

	case WL_CORNER_TR:		//TR
		lMoveX -= lPitchX;
		lMoveY -= lPitchY;
		szMsg = "TR ";
		break;

	case WL_CORNER_BL:		//BL
		lMoveX += lPitchX;
		lMoveY += lPitchY;
		szMsg = "BL ";
		break;

	case WL_CORNER_BR:		//BR
		lMoveX -= lPitchX;
		lMoveY += lPitchY;
		szMsg = "BR ";
		break;
	}

	XY_SafeMoveTo(lMoveX, lMoveY);
	Sleep(100);
	LONG lOffsetX = lMoveX, lOffsetY = lMoveY;
	lReturn = WftMoveSearchReferDie(lMoveX, lMoveY, FALSE, ucPrID);
	if( lReturn==FALSE )
	{
		lReturn = WftMoveSearchReferDie(lMoveX, lMoveY, FALSE, ucPrID);
	}
	if( lReturn )
	{
		lOffsetX = labs(lOffsetX - lMoveX);
		lOffsetY = labs(lOffsetY - lMoveY);
		if( labs(lOffsetX)>labs(GetDieSizeX()*2/3) ||
			labs(lOffsetY)>labs(GetDieSizeY()*2/3) )
		{
			szMsg += "Side Refer die offset over limit.\nWafer alignment fail!";
			HmiMessage_Red_Back(szMsg, "PLLM");
			lReturn = -1;
		}
	}
	else
	{
	//	if( HmiMessage_Red_Back("Refer die missing.\nContinue align wafer?", "PLLM", glHMI_MBX_YESNO)==glHMI_NO )
		szMsg += "Side Refer die missing.\nWafer alignment fail!";
		HmiMessage_Red_Back(szMsg, "PLLM");
		{
			lReturn = 0;
		}
	}
	Sleep(100);
	XY_SafeMoveTo(siOrigX, siOrigY);
	Sleep(100);
	if( lReturn<1 )
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}

	return lReturn;
}

BOOL CWaferTable::CheckRebel_NormalOut(ULONG ulCornerType)
{
//	if( PLLM_REBEL_EXTRA_EDGE==0 )
//	{
		return TRUE;
//	}
/*
	GrabAndSaveImage(WPR_NORMAL_DIE, 1, WPR_GRAB_SAVE_IMG_NOUT);

	BOOL bCtr = FALSE, bBtm = FALSE, bRight = FALSE, bTop = FALSE, bLeft = FALSE;
	LONG lDieState = RectWaferAroundDieCheck();
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>0 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}

	BOOL bReturn = TRUE;
	CString szMsg;
	switch (ulCornerType)
	{
	case WL_CORNER_TL:
	default:	//TL
		if( bLeft || bTop )
		{
			szMsg = "Find die outer around upper-left corner.\nPlease learn normal 1 with Pattern!";
			bReturn = FALSE;
		}
		break;

	case WL_CORNER_TR:		//TR
		if( bRight || bTop )
		{
			szMsg = "Find die outer around upper-right corner.\nPlease re-locate die!";
			bReturn = FALSE;
		}
		break;

	case WL_CORNER_BL:		//BL
		if( bLeft || bBtm )
		{
			szMsg = "Find die outer around lower-left corner.\nPlease re-locate die!";
			bReturn = FALSE;
		}
		break;

	case WL_CORNER_BR:		//BR
		if( bRight || bBtm )
		{
			szMsg = "Find die outer around lower-right corner.\nPlease re-locate die!";
			bReturn = FALSE;
		}
		break;
	}

	if( bReturn==FALSE )
	{
		szMsg += "\nDo you want to continue align?";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		if( HmiMessage_Red_Back(szMsg, "PLLM", glHMI_MBX_YESNO)==glHMI_YES )
		{
			bReturn = TRUE;
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog("OP select continue");
		}
	}

	return bReturn;
*/
}

BOOL CWaferTable::RebelManualAskPasswordToGo()
{
	BOOL bReturn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	UINT unEnable = pApp->GetProfileInt(gszPROFILE_SETTING, _T("ScanPassScorePassWord"), 0);
	if( unEnable !=0 )
	{
		bReturn = Check201277PasswordToGo();
	}

	return bReturn;
}

BOOL CWaferTable::ManualLocate3Corners()
{
	BOOL bFindGT = ManualAlignGlobalTheta(m_ulAlignRow, m_ulAlignCol, FALSE);	//v4.42T12
	CString szMsg;

	ULONG ulNumberOfRows = 0, ulNumberOfCols = 0;
	if ( m_WaferMapWrapper.GetMapDimension(ulNumberOfRows, ulNumberOfCols) != TRUE )
	{
		szMsg = "Map dimension error";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		return FALSE;
	}

	//Get current encoder position
	SetJoystickOn(FALSE);
	LONG lCurrX, lCurrY;
	LONG lNewX, lNewY, lNewT;
	GetEncoder(&lNewX, &lNewY, &lNewT);
	LONG lULRow = m_ulAlignRow;
	LONG lULCol = m_ulAlignCol;
	LONG lURRow = lULRow + 0;
	LONG lURCol = lULCol + ulNumberOfCols-2;
	LONG lLRRow = lULRow + ulNumberOfRows-2;
	LONG lLRCol = lULCol + ulNumberOfCols-2;
	LONG lLLRow = lULRow + ulNumberOfRows-2;
	LONG lLLCol = lULCol + 0;

	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();

	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Manual Locate 3 Corners Start ....");
	szMsg.Format("Manual-Align 3-corner-die: UL Die at Map(%d, %d)", lULRow, lULCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	szMsg.Format("UL Enc(%ld, %ld); PitchX(%ld, %ld) PitchY(%ld, %ld)", 
		lNewX, lNewY, lDiePitchX_X, lDiePitchX_Y, lDiePitchY_Y, lDiePitchY_X);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	LONG lDiff_X, lDiff_Y;

	SRCH_TYPE	stSrchInfo;
	REF_TYPE	stInfo;
	IPC_CServiceMessage stMsg;

	CMS896AApp* pApp = dynamic_cast<CMS896AApp*> (m_pModule);
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);	//v3.74T49

	CMSLogFileUtility::Instance()->MS_LogOperation("Manual-Align: Locate 3 Corner-Die");

	CheckRebel_NormalOut(WL_CORNER_TL);		//Klocwork	//v4.46
	//if( CheckRebel_NormalOut(WL_CORNER_TL)==FALSE )
	//{
	//	return FALSE;
	//}

	LONG lTLRC = CheckRebel_N2RC_Position(WL_CORNER_TL);
	if( lTLRC<0 )
	{
		return FALSE;
	}
	LONG lTLR4 = CheckRebel_N2R4_Relation(WL_CORNER_TL);
	if( lTLR4<0 )
	{
		return FALSE;
	}

	if( lTLRC<1 && lTLR4<1 )
	{
		if( RebelManualAskPasswordToGo()==FALSE )
		{
			return FALSE;
		}
	}

	//********************************//
	//Get Upper-Right die
	//v4.42T12	//No rotation!!
/*
	if( PLLM_REBEL_EXTRA_FIND )
	{
		if ( (IsMapNullBin(lULRow, lULCol)) ||
			(m_WaferMapWrapper.IsReferenceDie(lULRow, lULCol) == TRUE) )
		{
			lURRow = lURRow + 1;
		}

		for (INT i=ulNumberOfCols-1; i>=0; i--)
		{
			if (m_pWaferMapManager->IsMapHaveBin(lURRow, i))
			{
				lURCol = i;
				break;
			}
		}
	}
*/
	lDiff_X = lURCol - lULCol;
	lDiff_Y = lURRow - lULRow;
	lCurrX = lNewX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
	lCurrY = lNewY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
	szMsg.Format("move distance %d,%d", labs(lCurrX-lNewX), labs(lCurrY-lNewY));
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	if (!IsWithinWaferLimit(lCurrX, lCurrY))
	{
		//SetAlarmLamp_Red(FALSE);
		szMsg = "UR corner position out of wafer limit!";
		HmiMessage_Red_Yellow(szMsg);
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		return FALSE;
	}
	m_WaferMapWrapper.SetCurrentPosition(lURRow, lURCol);
	
	//v3.74T50
#ifdef NU_MOTION
	XY_SMoveTo(lCurrX, lCurrY);
#else
	XY_MoveTo(lCurrX, lCurrY);
#endif
	Sleep(200);

	//Search current die
	LONG lReturn = 0;
	while( 1 )
	{
		SetJoystickOn(TRUE);
		lReturn = HmiMessageEx("Please locate upper-right corner-die center with joystick, then press CONTINUE", 
							"Manual-Align", glHMI_MBX_CONTINUESTOP);
		SetJoystickOn(FALSE);
		if (lReturn != glHMI_CONTINUE)
		{
			Sleep(100);
			szMsg = "OP stop";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			return FALSE;
		}

		stSrchInfo.bShowPRStatus		= FALSE;
		stSrchInfo.bNormalDie			= TRUE;
		stSrchInfo.lRefDieNo			= 1;
		stSrchInfo.bDisableBackupAlign	= FALSE;
		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

		INT	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie_PLLM_REBEL", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		LONG lX, lY, lT;
		GetEncoder(&lX, &lY, &lT);
		if( stInfo.bGoodDie )
		{
			lX += stInfo.lX;	
			lY += stInfo.lY;
			XY_SafeMoveTo(lX, lY);
		}

		if( CheckRebel_NormalOut(WL_CORNER_TR) )
		{
			break;
		}
	}

	SetJoystickOn(FALSE);
	Sleep(100);
	if ( bFindGT==FALSE )
	{
	//	if( PLLM_REBEL_EXTRA_WFGT )
		{
			LONG	lLHSPhyX			= m_lStart_X;
			LONG	lLHSPhyY			= m_lStart_Y;
			LONG lX, lY, lT;
			GetEncoder(&lX, &lY, &lT);
			LONG	lRHSPhyX			= lX;
			LONG	lRHSPhyY			= lY;
			LONG	lThetaStep;
				//Calculate global angle
			DOUBLE	dDiffY = (double)(lLHSPhyY - lRHSPhyY);
			DOUBLE	dDiffX = (double)(lLHSPhyX - lRHSPhyX);
			DOUBLE	dThetaAngle = atan2(dDiffY , dDiffX) * (180 / PI);
			RotateWaferTheta(&lX, &lY, &lThetaStep, dThetaAngle);
			XY_SafeMoveTo(lX, lY);
				
			T_Move(lThetaStep, SFM_WAIT);
			Sleep(1000);
			GetEncoderValue();
			lX			= m_lStart_X;
			lY			= m_lStart_Y;
			RotateWaferTheta(&lX, &lY, &lThetaStep, dThetaAngle);
			m_lStart_X		= lX;
			m_lStart_Y		= lY;
			m_lStart_T		= GetCurrT();
			SetGlobalTheta();
			szMsg.Format("UL Enc(%ld, %ld); T: %d", lX, lY, m_lStart_T);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		}
	}

	Sleep(200);
	GetEncoder(&lNewX, &lNewY, &lNewT);
	szMsg.Format("Manual-Align 3-corner-die: UR Die at (%d, %d)", lURRow, lURCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	LONG lURX = lNewX;
	LONG lURY = lNewY;

	LONG lDeltaX = (lNewX - lCurrX);
	szMsg.Format("UR: EncX = %ld, MovX = %ld, Delta = %ld", (lNewX), (lCurrX), lDeltaX);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	LONG lDeltaY = (lNewY - lCurrY);
	szMsg.Format("UR: EncY = %ld, MovY = %ld, Delta = %ld", (lNewY), (lCurrY), lDeltaY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	BOOL bTRPitch = FALSE;
	if( bFindGT )	//	&& PLLM_REBEL_EXTRA_NO_PITCH==0 )
	{
		bTRPitch = TRUE;
		if ( CheckLongJumpPitchX(lDeltaX, labs(lURCol - lULCol))==FALSE )
		{
			szMsg.Format("Manual-Align 3-corner-die fail -> UR corner-die exceeds X-die-pitch");
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			HmiMessage_Red_Yellow(szMsg);
			bTRPitch = FALSE;
		}

		if ( CheckLongJumpPitchY(lDeltaY, labs(lURRow - lULRow))==FALSE ) 
		{
			szMsg.Format("Manual-Align 3-corner-die fail -> UR corner-die exceeds Y-die-pitch");
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			HmiMessage_Red_Yellow(szMsg);
			bTRPitch = FALSE;
		}
	}

	LONG lTRRC = CheckRebel_N2RC_Position(WL_CORNER_TR);
	if( lTRRC<0 )
	{
		return FALSE;
	}
	LONG lTRR4 = CheckRebel_N2R4_Relation(WL_CORNER_TR);
	if( lTRR4<0 )
	{
		return FALSE;
	}

	if( lTRRC<1 && lTRR4<1 && bTRPitch==FALSE)
	{
		if( RebelManualAskPasswordToGo()==FALSE )
		{
			return FALSE;
		}
	}

	//********************************//
	//Get Lower-Right die
/*
	if( PLLM_REBEL_EXTRA_FIND )
	{
		for (INT i=ulNumberOfRows-1; i>=0; i--)
		{
			if (m_pWaferMapManager->IsMapHaveBin(i, lLRCol))
			{
				lLRRow = i;
				break;
			}
		}
	}
*/
	//v4.42T12	//Jump based on confirmed UR pos
	lDiff_X = lLRCol - lURCol;
	lDiff_Y = lLRRow - lURRow;
	lCurrX = lNewX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
	lCurrY = lNewY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
	szMsg.Format("move distance %d,%d", labs(lCurrX-lNewX), labs(lCurrY-lNewY));
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
/*
	///////////////////////////////////////////////////
	//v4.42T13
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	SRCH_TYPE	stSrchInfo;
	LONG lIndexX = lNewX;
	LONG lIndexY = lNewY;

	for (INT y=lURRow; y<lLRRow; y++)
	{
		lIndexY = lIndexY - lDiePitchY_Y;
#ifdef NU_MOTION	
		XY_SMoveTo(lCurrX, lIndexY);
#else
		XY_MoveTo(lCurrX, lIndexY);
#endif

		Sleep(50);

		//Init Message
		stSrchInfo.bShowPRStatus	= FALSE;
		stSrchInfo.bNormalDie		= TRUE;
		stSrchInfo.lRefDieNo		= 1;
		stSrchInfo.bDisableBackupAlign = FALSE;
		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		lCurrX  += stInfo.lX;	
		lIndexY += stInfo.lY;

		//Move table to updated position
#ifdef NU_MOTION	
		XY_SMoveTo(lCurrX, lIndexY);
#else
		XY_MoveTo(lCurrX, lIndexY);
#endif
		Sleep(50);
	}
	///////////////////////////////////////////////////
*/

	if (!IsWithinWaferLimit(lCurrX, lCurrY))
	{
		//SetAlarmLamp_Red(FALSE);
		szMsg = "LR corner position out of wafer limit!";
		HmiMessage_Red_Yellow(szMsg);
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		return FALSE;
	}
	m_WaferMapWrapper.SetCurrentPosition(lLRRow, lLRCol);

#ifdef NU_MOTION	
	XY_SMoveTo(lCurrX, lCurrY);
#else
	XY_MoveTo(lCurrX, lCurrY);
#endif
	Sleep(200);

	//Search current die
	while( 1 )
	{
		SetJoystickOn(TRUE);
		lReturn = HmiMessageEx("Please locate lower-right corner-die center with joystick, then press CONTINUE", 
							"Manual-Align", glHMI_MBX_CONTINUESTOP);
		SetJoystickOn(FALSE);
		if (lReturn != glHMI_CONTINUE)
		{
			Sleep(100);
			szMsg = "OP stop";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			return FALSE;
		}

		stSrchInfo.bShowPRStatus		= FALSE;
		stSrchInfo.bNormalDie			= TRUE;
		stSrchInfo.lRefDieNo			= 1;
		stSrchInfo.bDisableBackupAlign	= FALSE;
		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		INT	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie_PLLM_REBEL", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		LONG lX, lY, lT;
		GetEncoder(&lX, &lY, &lT);
		if( stInfo.bGoodDie )
		{
			lX += stInfo.lX;	
			lY += stInfo.lY;
			XY_SafeMoveTo(lX, lY);
		}

		if( CheckRebel_NormalOut(WL_CORNER_BR) )
		{
			break;
		}
	}

	SetJoystickOn(FALSE);
	Sleep(100);
	GetEncoder(&lNewX, &lNewY, &lNewT);
	LONG lBRX = lNewX;
	LONG lBRY = lNewY;

	szMsg.Format("Manual-Align 3-corner-die: LR Die at (%d, %d)", lLRRow, lLRCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	lDeltaX = (lNewX-lCurrX);
	szMsg.Format("LR: EncX = %ld, MovX = %ld, Delta = %ld", (lNewX), (lCurrX), lDeltaX);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	lDeltaY = (lNewY-lCurrY);
	szMsg.Format("LR: EncY = %ld, MovY = %ld, Delta = %ld", (lNewY), (lCurrY), lDeltaY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	BOOL bBRPitch = FALSE;
//	if( PLLM_REBEL_EXTRA_NO_PITCH==0 )
	{
		bBRPitch = TRUE;
		if ( CheckLongJumpPitchX(lDeltaX, labs(lLRCol - lURCol))==FALSE )
		{
			szMsg.Format("Manual-Align 3-corner-die fail -> LR corner-die exceeds X-die-pitch");
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			HmiMessage_Red_Yellow(szMsg);
			bBRPitch = FALSE;
		}

		if ( CheckLongJumpPitchY(lDeltaY, labs(lLRRow - lURRow))==FALSE ) 
		{
			szMsg.Format("Manual-Align 3-corner-die fail -> LR corner-die exceeds Y-die-pitch");
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			HmiMessage_Red_Yellow(szMsg);
			bBRPitch = FALSE;
		}
	}

	LONG lBRRC = CheckRebel_N2RC_Position(WL_CORNER_BR);
	if( lBRRC<0 )
	{
		return FALSE;
	}

	LONG lBRR4 = CheckRebel_N2R4_Relation(WL_CORNER_BR);
	if( lBRR4<0 )
	{
		return FALSE;
	}

	if( lBRRC<1 && lBRR4<1 && bBRPitch==FALSE )
	{
		if( RebelManualAskPasswordToGo()==FALSE )
		{
			return FALSE;
		}
	}

	//********************************//
	//Look aorund at lower-right corner
	typedef struct {
		ULONG	ulCornerOption;
		BOOL	bLfResult;
		BOOL	bFovSize;
	} SRCH_LR_TYPE;
	SRCH_LR_TYPE	stSrchLrCornerInfo;

	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("LookAheadLRightRectWaferCorner");
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "LookAheadLRightRectWaferCorner", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!stSrchLrCornerInfo.bFovSize)
	{
		szMsg = "Manual-Align 3-corner-die: LookAround LR corner die FOV fail";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}
	else if (stSrchLrCornerInfo.bFovSize && stSrchLrCornerInfo.bLfResult)
	{
		szMsg = "Manual-Align 3-corner-die: LookAround LR corner die OK";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}
	else if (stSrchLrCornerInfo.bFovSize && !stSrchLrCornerInfo.bLfResult)
	{
		szMsg = "Manual-Align 3-corner-die: LookAround LR corner die fail";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

		//SetAlarmLamp_Red(FALSE);
	//	szMsg = "Warning: PR Look-Around mode fails at Lower-right corner die; CONTINUE?";
	//	LONG lReturn = HmiMessage_Red_Yellow(szMsg, "Manual Align LR Corner Die", glHMI_MBX_CONTINUESTOP);
		szMsg = "Warning: PR Look-Around mode fails at Lower-right corner die!";
		HmiMessage_Red_Yellow(szMsg, "Manual Align LR Corner Die");
	//	if (lReturn != glHMI_CONTINUE)
		{
			szMsg = "Manual-Align 3-corner-die: LookAround LR corner die fail.";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			return FALSE;
		}

		//Klocwork	//v4.46
		//szMsg = "Manual-Align 3-corner-die: LookAround LR corner die fail but CONTINUE";
		//CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}

	//********************************//
	//Get Lower-Left die
/*
	if( PLLM_REBEL_EXTRA_FIND )
	{
		for (ULONG i=0; i<ulNumberOfCols; i++)
		{
			if (m_pWaferMapManager->IsMapHaveBin(lLLRow, i))
			{
				lLLCol = i;
				break;
			}
		}
	}
*/

	//v4.42T12
	lNewX = lBRX;
	lNewY = lBRY;

	lDiff_X = lLLCol - lLRCol;
	lDiff_Y = lLLRow - lLRRow;
	lCurrX = lNewX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
	lCurrY = lNewY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
	szMsg.Format("move distance %d,%d", labs(lCurrX-lNewX), labs(lCurrY-lNewY));
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	if (!IsWithinWaferLimit(lCurrX, lCurrY))
	{
		szMsg = "LL corner position is out of wafer limit!";
		HmiMessage_Red_Yellow(szMsg);
		SetErrorMessage(szMsg);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		return FALSE;
	}
	m_WaferMapWrapper.SetCurrentPosition(lLLRow, lLLCol);
		
#ifdef NU_MOTION	
	XY_SMoveTo(lCurrX, lCurrY);
#else
	XY_MoveTo(lCurrX, lCurrY);
#endif
	Sleep(200);

	//Search current die
	while( 1 )
	{
		SetJoystickOn(TRUE);
		lReturn = HmiMessageEx("Please locate lower-left corner-die center with joystick, then press CONTINUE", 
							"Manual-Align", glHMI_MBX_CONTINUESTOP);
		SetJoystickOn(FALSE);
		if (lReturn != glHMI_CONTINUE)
		{
			Sleep(100);
			szMsg = "OP stop";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			return FALSE;
		}

		stSrchInfo.bShowPRStatus		= FALSE;
		stSrchInfo.bNormalDie			= TRUE;
		stSrchInfo.lRefDieNo			= 1;
		stSrchInfo.bDisableBackupAlign	= FALSE;
		stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
		INT	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie_PLLM_REBEL", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		LONG lX, lY, lT;
		GetEncoder(&lX, &lY, &lT);
		if( stInfo.bGoodDie )
		{
			lX += stInfo.lX;	
			lY += stInfo.lY;
			XY_SafeMoveTo(lX, lY);
		}

		if( CheckRebel_NormalOut(WL_CORNER_BL) )
		{
			break;
		}
	}

	SetJoystickOn(FALSE);

	Sleep(100);
	GetEncoder(&lNewX, &lNewY, &lNewT);
	LONG lBLX = lNewX;
	LONG lBLY = lNewY;

	szMsg.Format("Manual-Align 3-corner-die: LL Die at (%d, %d)", lLLRow, lLLCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	lDeltaX = (lNewX-lCurrX);
	szMsg.Format("LL: EncX = %ld, MovX = %ld, Delta = %ld", lNewX, lCurrX, lDeltaX);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	lDeltaY = (lNewY-lCurrY);
	szMsg.Format("LL: EncY = %ld, MovY = %ld, Delta = %ld", lNewY, lCurrY, lDeltaY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	BOOL bBLPitch = FALSE;
//	if( PLLM_REBEL_EXTRA_NO_PITCH==0 )
	{
		bBLPitch = TRUE;
		if ( CheckLongJumpPitchX(lDeltaX, labs(lLLCol - lLRCol)) == FALSE )
		{
			szMsg.Format("Manual-Align 3-corner-die fail -> LL corner-die exceeds X-die-pitch");
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			HmiMessage_Red_Yellow(szMsg);
			bBLPitch = FALSE;
		}

		if ( CheckLongJumpPitchY(lDeltaY, labs(lLLRow - lLRRow)) == FALSE ) 
		{
			szMsg.Format("Manual-Align 3-corner-die fail -> LL corner-die exceeds Y-die-pitch");
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
			HmiMessage_Red_Yellow(szMsg);
			bBLPitch = FALSE;
		}
	}

	LONG lBLRC = CheckRebel_N2RC_Position(WL_CORNER_BL);
	if( lBLRC<0 )
	{
		return FALSE;
	}
	LONG lBLR4 = CheckRebel_N2R4_Relation(WL_CORNER_BL);
	if( lBLR4<0 )
	{
		return FALSE;
	}
	if( lBLRC<1 && lBLR4<1 && bBLPitch==FALSE )
	{
		if( RebelManualAskPasswordToGo()==FALSE )
		{
			return FALSE;
		}
	}

	LONG lY_X = GetDiePitchY_X();
	LONG lY_Y = GetDiePitchY_Y();
	LONG lX_X = GetDiePitchX_X();
	LONG lX_Y = GetDiePitchX_Y();
	if( bBRPitch==FALSE )
	{
		LONG lB2TX = lBRX - lURX;
		LONG lB2TY = lBRY - lURY;
		lDiff_Y = lLRRow - lURRow;
		if( lDiff_Y!=0 )
		{
			lY_X = lB2TX/lDiff_Y;
			lY_Y = labs(lB2TY/lDiff_Y);
		}
	}

	if( bBLPitch==FALSE )
	{
		LONG lL2RX = lBRX - lBLX;
		LONG lL2RY = lBRY - lBLY;
		lDiff_X = lLLCol - lLRCol;
		if( lDiff_X!=0 )
		{
			lX_X = labs(lL2RX/lDiff_X);
			lX_Y = lL2RY/lDiff_X;
		}
	}

	(*m_psmfSRam)["WaferPr"]["NewDiePitchY"]["Y"] = lY_Y;
	(*m_psmfSRam)["WaferPr"]["NewDiePitchY"]["X"] = lY_X;
	(*m_psmfSRam)["WaferPr"]["NewDiePitchX"]["X"] = lX_X;
	(*m_psmfSRam)["WaferPr"]["NewDiePitchX"]["Y"] = lX_Y;
	szMsg.Format("New learnt pitch X(%d,%d), Y(%d,%d)", lX_X, lX_Y, lY_Y, lY_X);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

	if( bBLPitch && bBRPitch )
	{
		m_bRebelManualAlign = FALSE;
	}
	else
	{
		m_bRebelManualAlign = TRUE;
	}

	//********************************//
	//Move back to original position
	LONG lOrigX = m_lStart_X;
	LONG lOrigY = m_lStart_Y;
#ifdef NU_MOTION
	XY_SMoveTo(lOrigX, lOrigY);
#else
	XY_MoveTo(lOrigX, lOrigY);
#endif
	Sleep(1000);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Manual Locate 3 Corners Done.\n");

	return TRUE;
}


//================================================================
// SearchAndAlignPattern()
//   Created-By  : Andrew Ng
//   Date        : 5/7/2009 3:35:42 PM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferTable::SearchAndAlignPattern()
{
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;

	typedef struct {
		BOOL bStatus;
		BOOL bLFStatus;
		BOOL bCGoodDie;
		LONG lCX;
		LONG lCY;
	} ALN_REF_TYPE;
	ALN_REF_TYPE	stInfo;


	UCHAR ucPattern = 1;	//NeoNeon
	stMsg.InitMessage(sizeof(UCHAR), &ucPattern);


	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchAlignPattern", stMsg);			//Use default Src Wnd
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(ALN_REF_TYPE), &stInfo);
			break;
		}
		else{
			Sleep(10);
		}
	}


	if (!stInfo.bLFStatus)
	{
HmiMessage("LF window smaller than 5x5!");
		return FALSE;
	}

	if (stInfo.bStatus)		//Pattern found!
	{
		return TRUE;
	}

	if (stInfo.bCGoodDie)	
	{
		//Do PR compensation
		LONG lCurrX, lCurrY, lCurrT;
		GetEncoder(&lCurrX, &lCurrY, &lCurrT);
		lCurrX	+= stInfo.lCX;	
		lCurrY	+= stInfo.lCY;

		XY_SafeMoveTo(lCurrX, lCurrY);
	}

	return FALSE;
}


BOOL CWaferTable::CheckCreeHorz4RefDice()				//v3.82		//Cree US Waffle Pack machine buyoff
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != _T("Cree"))
		return FALSE;

	m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
	
	ULONG ulRefDieRow = m_ulAlignRow;
	ULONG ulRefDieCol = m_ulAlignCol + 1;
	if (!m_WaferMapWrapper.IsReferenceDie(m_ulAlignRow, ulRefDieCol))
		return FALSE;

	ulRefDieCol = ulRefDieCol + 1;
	if (!m_WaferMapWrapper.IsReferenceDie(m_ulAlignRow, ulRefDieCol))
		return FALSE;

	ulRefDieCol = ulRefDieCol + 1;
	if (!m_WaferMapWrapper.IsReferenceDie(m_ulAlignRow, ulRefDieCol))
		return FALSE;

	return TRUE;
}


BOOL CWaferTable::AlignU2UReferenceDie()
{
	CString szTitle, szContent;
	ULONG ulNumberOfRows, ulNumberOfCols;
	LONG lColDiff, lRowDiff;
	LONG lHomeDiePos_X, lHomeDiePos_Y;
	LONG lCur_X, lCur_Y, lCur_T;
	LONG lTargetX, lTargetY;
	BOOL bFoundStepStone = FALSE;
	IPC_CServiceMessage stMsg;
	REF_TYPE stInfo;
	LONG lTolerance = 35;
	BOOL bStatus = TRUE;


	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		szTitle.LoadString(HMB_WT_ALIGN_WAFER);
		szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	lHomeDiePos_X = m_lCurrent_X;
	lHomeDiePos_Y = m_lCurrent_Y;

	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();

	lColDiff = ulNumberOfCols;
	lRowDiff = ulNumberOfRows;
	GetEncoder(&lCur_X, &lCur_Y, &lCur_T);
	
	//v4.46T19
	CString szTemp;
	szTemp.Format("***** WT: Align Sprial-Wafer (U2U) start (fully-auto): map dimension (%lu, %lu), CornerOption (%d)", 
		ulNumberOfRows, ulNumberOfCols, m_ulCornerSearchOption);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//v4.46T25
	BOOL bTRCornerRefDie = FALSE;
	if (m_ulCornerSearchOption == WL_CORNER_TR)
	{
		//Handle new U2U map with ref die at upper-right corner instead of UL corner
		bTRCornerRefDie = TRUE;
	}

	//v4.46T25
	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, lUserRow, lUserCol);
	szTemp.Format("AlignU2UReferenceDie: Curr-Map-U (%ld, %ld), Curr-EncXY (%ld, %ld)", 
					lUserRow, lUserCol, lCur_X, lCur_Y);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);


	//v4.46T19	//Support of U2U with TR Ref Die
	if (m_ulCornerSearchOption == WL_CORNER_TR)
	{
		//New U2U wafer with Ref-Die at TR corner
		lTargetX	= lCur_X + lColDiff * lDiePitchX_X + lRowDiff * lDiePitchY_X;
		lTargetY	= lCur_Y - lRowDiff * lDiePitchY_Y - lColDiff * lDiePitchX_Y;
	}
	else
	{
		//Default is Ref-Die at TL corner in Rebel Tile Wafer
		// move form Home Die Position and search the lower right corner (hex pattern)
		lTargetX	= lCur_X - lColDiff * lDiePitchX_X - lRowDiff * lDiePitchY_X;
		lTargetY	= lCur_Y - lRowDiff * lDiePitchY_Y - lColDiff * lDiePitchX_Y;
	}

	if (XY_SafeMoveTo(lTargetX, lTargetY) == FALSE)
	{
		HmiMessage_Red_Yellow("REF die position out of wafer limit!!");
		szTemp.Format("REF die position out of wafer limit (%ld, %ld)\n", lTargetX, lTargetY);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		return FALSE;		//return out wafer limit
	}

	bStatus = SearchDieInSearchWindow(FALSE, FALSE, HEX_PATTERN, FALSE, stInfo);
	if (bStatus == FALSE)
	{
		HmiMessage_Red_Yellow("Search Ref pattern fail");
		SetErrorMessage("Search Ref pattern fail");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Search PR Ref-Die patten (HEX) fails\n");
		return FALSE;
	}

	//GetEncoder(&lCur_X, &lCur_Y, &lCur_T);
	//m_WaferMapWrapper.SetCurrentPosition(m_ulAlignRow, m_ulAlignCol);
	//SetAlignStartPoint(FALSE, lCur_X, lCur_Y, m_ulAlignRow, m_ulAlignCol);

	// move back to home die position
	szTemp.Format("AlignU2UReferenceDie to Enc(%ld, %ld)", lTargetX, lTargetY);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
	XY_SafeMoveTo(lHomeDiePos_X, lHomeDiePos_Y);
	Sleep(1000);

	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("AlignU2UReferenceDie (PLLM U2U) is Done\n");
	return TRUE;	
}

LONG CWaferTable::ManualAlignU2UReferenceDie()
{
	IPC_CServiceMessage stMsg;
	REF_TYPE stInfo;
	CString szTitle, szContent;
	ULONG ulNumberOfRows, ulNumberOfCols;
	LONG lColDiff, lRowDiff;
	LONG lCur_X, lCur_Y, lCur_T;
	LONG lTargetX, lTargetY;
	LONG lTolerance = 100;
	ULONG ulRow, ulCol;
	CString szLog;
	BOOL bStatus = TRUE;


//HmiMessage("Andrew: Start PLLM U2U alignment ....");	//v4.46T7

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		szTitle.LoadString(HMB_WT_ALIGN_WAFER);
		szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 1");	//v4.46T7
		return WT_ALN_ERROR;
	}

	//v4.46T10
	ulNumberOfRows	= m_lPLLMWaferRow;/*52;*/
	ulNumberOfCols	= m_lPLLMWaferCol;/*52;*/


	CString temp;
	temp.Format("Manual input U2U reference die position : %d,%d",m_lPLLMWaferRow,m_lPLLMWaferCol);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(temp);	
	//v4.46T10
	BOOL bShift1Col = FALSE;
	if (m_ulCornerSearchOption == WL_CORNER_TR)
	{
		//Handle new U2U map with ref die at upper-right corner instead of UL corner
		bShift1Col = TRUE;
	}

	//v4.46T25
	//For TL option, TL die uses RefDie #1, TR -> RefDie #2, LR -> RefDie #3, LL -> RefDie #4
	//For TR option, TL die uses RefDie #2, TR -> RefDie #1, LR -> RefDie #4, LL -> RedDie #3


	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();

	m_WaferMapWrapper.GetSelectedPosition(ulRow, ulCol);
	GetEncoder(&lCur_X, &lCur_Y, &lCur_T);

	ulRow = m_ulAlignRow;
	ulCol = m_ulAlignCol;

	LONG lUserRow=0, lUserCol=0;
	ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol); 

szLog.Format("ManualAlignU2URefDie: at (%ld, %ld), Enc(%ld, %ld), Dimension(%ld, %ld); LeftShift(%d)",  
			 lUserRow, lUserCol, lCur_X, lCur_Y, ulNumberOfRows, ulNumberOfCols, bShift1Col);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
HmiMessage(szLog);

	//v4.46T11
	//lColDiff = 0 - ulCol;
	//lRowDiff = 0 - ulRow;
	lColDiff = -1 * lUserCol;
	lRowDiff = lUserRow;

	if (bShift1Col)		//v4.46T10
	{
		lTargetX = lCur_X - lColDiff * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	else
	{
		lTargetX = lCur_X - lColDiff * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	lTargetY	= lCur_Y - lRowDiff * lDiePitchY_Y - lColDiff * lDiePitchX_Y;

szLog.Format("ManualAlignU2UReferenceDie: move to 1st pos at (%ld, %ld), Diff(%ld, %ld), Pitch(%ld, %ld)",  
			 lTargetX, lTargetY, lRowDiff, lColDiff,
			 lDiePitchX_X, lDiePitchY_Y);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (XY_SafeMoveTo(lTargetX, lTargetY) == FALSE)
	{
		HmiMessage_Red_Yellow("REF die position out of wafer limit!!");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 2");	//v4.46T7
		return WT_ALN_ERROR;		//return out wafer limit
	}

//AfxMessageBox("DIAMOND_PATTERN", MB_SYSTEMMODAL);

	Sleep(500);
	if (bShift1Col)		//if TR Corner option
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, RECT_PATTERN, FALSE, stInfo);		//TL uses RefDie #2
	else
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, DIAMOND_PATTERN, FALSE, stInfo);	//TL uses RefDie #1
	if (bStatus == FALSE)
	{
		HmiMessage_Red_Yellow("Search TL Ref pattern fail");
		SetErrorMessage("Search TL Ref pattern fail");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 3 at TL");	//v4.46T7
		return WT_ALN_ERROR;
	}

	//v4.46T11
	//lColDiff = ulNumberOfCols - ulCol;
	//lRowDiff = 0 - ulRow;
	lColDiff = ulNumberOfCols + 1 - lUserCol;//lColDiff = 53 - lUserCol;
	lRowDiff = lUserRow;
	
	if (bShift1Col)		//v4.46T10
	{
		lTargetX	= lCur_X - (lColDiff - 1) * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	else
	{
		lTargetX	= lCur_X - lColDiff * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	lTargetY	= lCur_Y - lRowDiff * lDiePitchY_Y - lColDiff * lDiePitchX_Y;

szLog.Format("ManualAlignU2UReferenceDie: move to 2nd pos at (%ld, %ld)",  lTargetX, lTargetY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	
	if (XY_SafeMoveTo(lTargetX, lTargetY) == FALSE)
	{
		HmiMessage_Red_Yellow("REF die position out of wafer limit!!");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 4");	//v4.46T7
		return WT_ALN_ERROR;		//return out wafer limit
	}

	Sleep(500);
	if (bShift1Col)		//if TR Corner option
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, DIAMOND_PATTERN, FALSE, stInfo);	//TR uses RefDie #1
	else
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, RECT_PATTERN, FALSE, stInfo);		//TR uses RefDie #2
	if (bStatus == FALSE)
	{
		HmiMessage_Red_Yellow("Search TR Ref pattern fail");
		SetErrorMessage("Search TR Ref pattern fail");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 5 at TR");	//v4.46T7
		return WT_ALN_ERROR;
	}

	//v4.46T11
	lColDiff = ulNumberOfCols + 1 - ulCol;
	lRowDiff = ulNumberOfRows + 1 - ulRow;
	//lColDiff = 53 - lUserCol;
	//lRowDiff = 53 - labs(lUserRow);

	if (bShift1Col)		//v4.46T10
	{
		lTargetX = lCur_X - (lColDiff - 1) * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	else
	{
		lTargetX = lCur_X - lColDiff * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	lTargetY	= lCur_Y - lRowDiff * lDiePitchY_Y - lColDiff * lDiePitchX_Y;
	
szLog.Format("ManualAlignU2UReferenceDie: move to 3th pos at (%ld, %ld)",  lTargetX, lTargetY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (XY_SafeMoveTo(lTargetX, lTargetY) == FALSE)
	{
		HmiMessage_Red_Yellow("REF die position out of wafer limit!!");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 6");	//v4.46T7
		return WT_ALN_ERROR;		//return out wafer limit
	}

	Sleep(500);
	if (bShift1Col)		//if TR Corner option
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, TRI_PATTERN, FALSE, stInfo);	//LR uses RefDie #4
	else
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, HEX_PATTERN, FALSE, stInfo);	//LR uses RefDie #3
	if (bStatus == FALSE)
	{
		HmiMessage_Red_Yellow("Search LR Ref pattern fail");
		SetErrorMessage("Search LR Ref pattern fail");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 7 at LL");	//v4.46T7
		return WT_ALN_ERROR;
	}

	//v4.46T11
	//lColDiff = 0 - ulCol;
	lRowDiff = ulNumberOfRows + 1 - ulRow;
	lColDiff = -1 * lUserCol;
	//lRowDiff = 53 - labs(lUserRow);

	if (bShift1Col)		//v4.46T10
	{
		lTargetX	= lCur_X - lColDiff * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	else
	{
		lTargetX	= lCur_X - lColDiff * lDiePitchX_X - lRowDiff * lDiePitchY_X;
	}
	lTargetY	= lCur_Y - lRowDiff * lDiePitchY_Y - lColDiff * lDiePitchX_Y;

szLog.Format("ManualAlignU2UReferenceDie: move to 4th pos at (%ld, %ld)",  lTargetX, lTargetY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (XY_SafeMoveTo(lTargetX, lTargetY) == FALSE)
	{
		HmiMessage_Red_Yellow("REF die position out of wafer limit!!");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 8");	//v4.46T7
		return WT_ALN_ERROR;		//return out wafer limit
	}

//AfxMessageBox("TRI_PATTERN", MB_SYSTEMMODAL);

	Sleep(500);
	if (bShift1Col)		//if TR Corner option
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, HEX_PATTERN, FALSE, stInfo);	//LL uses RefDie #3
	else
		bStatus = SearchDieInSearchWindow(FALSE, FALSE, TRI_PATTERN, FALSE, stInfo);	//LL uses RefDie #4
	if (bStatus == FALSE)
	{
		HmiMessage_Red_Yellow("Search LL Ref pattern fail");
		SetErrorMessage("Search LL Ref pattern fail");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U FAIL 9 at LL");	//v4.46T7
		return WT_ALN_ERROR;
	}

	XY_SafeMoveTo(lCur_X, lCur_Y);
	Sleep(500);

	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("Auto-ALIGN PLLM Rebel U2U OK");	//v4.46T7
	return WT_ALN_OK;
}

BOOL CWaferTable::SearchRectPatternFromMap(ULONG& ulULRow, ULONG& ulULCol, 
										   eWT_Pattern ePattern,
										   CONST UCHAR ucPatternNum,
										   CONST BOOL bIsPartialWafer)		//v4.41T4
{
	LONG lStartRow=0, lStartCol=0;
	LONG lRow=0, lCol=0;
	BOOL bResult = TRUE;
	UCHAR ucNichiaTEGType = 0;
	
	//nichia001
	BOOL bIsTypeCMap = FALSE;
	CString szBasePt = CMS896AStn::m_oNichiaSubSystem.GetMapBasePt();
	if (szBasePt == "C")
		bIsTypeCMap = TRUE;

	//v4.40T13
	//1st TEG pattern must be the left TEG pattern on full wafer
	//2nd TEG pattern is at full wafer center region;
	//3rd TEG pattern must locate at right part of FULL wafer
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;
	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		return FALSE;
	}
	BOOL b1stTEGPatternFound = FALSE;
	ULONG ul1stTEGRegionRow = ulNumOfRows / 3;
	ULONG ul1stTEGRegionCol = ulNumOfCols / 3;
	INT n1stRegionOffset = 0;
	INT nTEGPattern = 0;
	INT nMaxLoop = 0;

	for (int i=1; i<=ucPatternNum; i++)
	{
		nTEGPattern = i + n1stRegionOffset;

		switch (ePattern)
		{
		case P1x3:
			bResult = Get1x3HomeDieAlignRowColFromMap(lRow, lCol, lStartRow, lStartCol);
			break;

		case P3x4:
			bResult = Get3x4HomeDieAlignRowColFromMap(lRow, lCol, lStartRow, lStartCol);
			break;

		case P4x3:			//v3.65		//NeoNeon
			bResult = Get4x3HomeDieAlignRowColFromMap(lRow, lCol, lStartRow, lStartCol);
			break;

		case NICHIA8x3:		//v4.40T10	//Nichia
			ucNichiaTEGType	= CMS896AStn::m_oNichiaSubSystem.m_ucWaferTEGType;	//v4.40T13
			if (i == 1)
			{
				nMaxLoop = 0;
				do 
				{
					bResult = GetNichia8030HomeDieAlignRowColFromMap(lRow, lCol, ucNichiaTEGType, lStartRow, lStartCol);
					if (!bResult)
						return FALSE;
					if (bIsPartialWafer)	// && (lRow > ul1stTEGRegionRow))
					{
						b1stTEGPatternFound = TRUE;
						break;
					}
					else if ((lRow > (LONG)ul1stTEGRegionRow) && (lCol < (LONG)ul1stTEGRegionCol))
					{
						b1stTEGPatternFound = TRUE;
						break;
					}
					else
					{
						n1stRegionOffset = n1stRegionOffset + 1;
						lStartRow = lRow;
						lStartCol = lCol + 1;
						nMaxLoop++;
					}

				} while (nMaxLoop < 3);

				if (!b1stTEGPatternFound)
				{
//AfxMessageBox("1st 800x300 TEG pattern not found!", MB_SYSTEMMODAL);
					return FALSE;
				}
			}
			else
			{
				bResult = GetNichia8030HomeDieAlignRowColFromMap(lRow, lCol, ucNichiaTEGType, lStartRow, lStartCol);
			}
			break;

		case NICHIA8x6:		//v4.40T10	//Nichia
			ucNichiaTEGType	= CMS896AStn::m_oNichiaSubSystem.m_ucWaferTEGType;	//v4.40T13
			if (i == 1)
			{
				CString szLog;
				nMaxLoop = 0;
				do 
				{
					bResult = GetNichia8060HomeDieAlignRowColFromMap(lRow, lCol, ucNichiaTEGType, lStartRow, lStartCol);
					if (!bResult)
						return FALSE;
					if (bIsPartialWafer)	// && (lRow > ul1stTEGRegionRow))
					{
						b1stTEGPatternFound = TRUE;
						break;
					}
					else if ((lRow > (LONG)ul1stTEGRegionRow) && (lCol < (LONG)ul1stTEGRegionCol))
					{
						b1stTEGPatternFound = TRUE;
						break;
					}
					else
					{
						n1stRegionOffset = n1stRegionOffset + 1;
						lStartRow = lRow;
						lStartCol = lCol + 1;
						nMaxLoop++;
					}

				} while (nMaxLoop < 3);

				if (!b1stTEGPatternFound)
				{
//AfxMessageBox("1st 800x600 TEG pattern not found!", MB_SYSTEMMODAL);
					return FALSE;
				}
			}
			else
			{
				bResult	= GetNichia8060HomeDieAlignRowColFromMap(lRow, lCol, ucNichiaTEGType, lStartRow, lStartCol);
			}
			break;

		case NICHIA6x6:		//v4.40T10	//Nichia
			bResult = FALSE;
			break;

		case P3x3:
		default:
			bResult = Get3x3HomeDieAlignRowColFromMap(lRow, lCol, lStartRow, lStartCol);
			break;
		}

		if (!bResult)
			return FALSE;

		lStartRow = lRow;
		lStartCol = lCol + 1;
	}


	//Select a die inside the pattern as map position of this pattern
	switch (ePattern)
	{
	case NICHIA8x3:
	case NICHIA8x6:
	case NICHIA6x6:
		ulULRow = lRow;
		ulULCol = lCol;
		break;

	case P1x3:
		//Return TOP die in 1x3 pattern
		ulULRow = lRow;
		ulULCol = lCol + 1;
		break;

	case P3x4:
		//Return UPPER-LEFT die in 3x4 pattern
		ulULRow = lRow;
		ulULCol = lCol + 1;
		break;

	case P3x3:
	default:
		//ulULRow = lRow + 1;	//v3.64
		//ulULCol = lCol + 2;
		//v3.64	//Return upper-left die pos within the pattern
		ulULRow = lRow;
		ulULCol = lCol + 1;
		break;
	}

	return TRUE;
}


//================================================================
// Get3x3HomeDieAlignRowColFromMap()
//   Created-By  : Andrew Ng
//   Date        : 5/8/2009 3:44:11 PM
//   Description : 
//   Remarks     : Return UL corner die before the 3x3 pattern
//================================================================
BOOL CWaferTable::Get3x3HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow, CONST LONG lStartCol)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		return FALSE;
	}
	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
	{
		return FALSE;
	}


	//5x5 region
	//===============
	// 1  2  3  4  5
	// 6  7  8  9  10
	// 11 12 13 14 15
	// 16 17 18 19 20
	// 21 22 23 24 25
	//===============
	
	//5x5 region (NeoNeon)
	//===============
	// -  o  o  o  -
	// o  x  x  x  o
	// o  x  x  x  o
	// o  x  x  x  o
	// -  o  o  o  -
	//===============


	BOOL bDie1, bDie2, bDie3, bDie4, bDie5;

	for (int i=lStartRow; i<(LONG)ulNumOfRows; i++)
	{
		int nCurrStartCol = 0;
		if (i == lStartRow)
			nCurrStartCol = lStartCol;
					
		if (i > ((LONG)ulNumOfRows - 3))
			break;

		for (int j=nCurrStartCol; j<(LONG)ulNumOfCols; j++)
		{
			if (j > ((LONG)ulNumOfCols-5))
				continue;

			bDie1 = !(IsMapNullBin(i, j));
			bDie2 = !(IsMapNullBin(i, j+1));
			bDie3 = !(IsMapNullBin(i, j+2));
			bDie4 = !(IsMapNullBin(i, j+3));
			bDie5 = !(IsMapNullBin(i, j+4));

			if (bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)
			{
				int nNextRow = i-1;
				if (nNextRow < 0)
					continue;

				bDie2 = !(IsMapNullBin(nNextRow, j+1));
				bDie3 = !(IsMapNullBin(nNextRow, j+2));
				bDie4 = !(IsMapNullBin(nNextRow, j+3));
			
				if (bDie2 && bDie3 && bDie4)
				{
					nNextRow = i+1;
					bDie1 = !(IsMapNullBin(nNextRow, j));
					bDie2 = !(IsMapNullBin(nNextRow, j+1));
					bDie3 = !(IsMapNullBin(nNextRow, j+2));
					bDie4 = !(IsMapNullBin(nNextRow, j+3));
					bDie5 = !(IsMapNullBin(nNextRow, j+4));

					if (bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)
					{
						nNextRow = i+2;
						bDie1 = !(IsMapNullBin(nNextRow, j));
						bDie2 = !(IsMapNullBin(nNextRow, j+1));
						bDie3 = !(IsMapNullBin(nNextRow, j+2));
						bDie4 = !(IsMapNullBin(nNextRow, j+3));
						bDie5 = !(IsMapNullBin(nNextRow, j+4));

						if (bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)
						{
							nNextRow = i+3;
							//bDie1 = (IsMapNullBin(nNextRow, j));
							bDie2 = !(IsMapNullBin(nNextRow, j+1));
							bDie3 = !(IsMapNullBin(nNextRow, j+2));
							bDie4 = !(IsMapNullBin(nNextRow, j+3));
							//bDie5 = (IsMapNullBin(nNextRow, j+4));

							if (bDie2 && bDie3 && bDie4)
							{
								lULRow = i;
								lULCol = j;
								return TRUE;	//Return Pos-#6
							}
						}
					}
				}
			}
		}
	}

	return FALSE;
}


//================================================================
// Get3x4HomeDieAlignRowColFromMap()
//   Created-By  : Andrew Ng
//   Date        : 11/30/2009 2:55:05 PM
//   Description : 
//   Remarks     : For Sunrich 1213 wafer type
//================================================================
BOOL CWaferTable::Get3x4HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow, CONST LONG lStartCol)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		return FALSE;
	}
	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
	{
		return FALSE;
	}


	//5x5 region
	//===============
	// 1  2  3  4  5
	// 6  7  8  9  10
	// 11 12 13 14 15
	// 16 17 18 19 20
	// 21 22 23 24 25
	// 26 27 28 29 30
	//===============
	
	//5x5 region (NeoNeon)
	//===============
	// -  o  o  o  -		//ROw #0
	// o  x  x  x  o		//Row #1
	// o  x  x  x  o		//Row #2
	// o  x  x  x  o		//Row #3
	// o  x  x  x  o		//Row #4
	// -  o  o  o  -		//Row #5
	//===============


	BOOL bDie1, bDie2, bDie3, bDie4, bDie5;

	for (int i=lStartRow; i<(LONG)ulNumOfRows; i++)
	{
		int nCurrStartCol = 0;
		if (i == lStartRow)
			nCurrStartCol = lStartCol;
					
		if (i > ((LONG)ulNumOfRows - 3))
			break;

		for (int j=nCurrStartCol; j<(LONG)ulNumOfCols; j++)
		{
			if (j > ((LONG)ulNumOfCols-5))
				continue;

			bDie1 = !(IsMapNullBin(i, j));
			bDie2 = !(IsMapNullBin(i, j+1));
			bDie3 = !(IsMapNullBin(i, j+2));
			bDie4 = !(IsMapNullBin(i, j+3));
			bDie5 = !(IsMapNullBin(i, j+4));

			if (bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)	//Check Row #1
			{
				int nNextRow = i-1;
				if (nNextRow < 0)
					return FALSE;	//pattern must not be found on 1st row

				bDie2 = !(IsMapNullBin(nNextRow, j+1));
				bDie3 = !(IsMapNullBin(nNextRow, j+2));
				bDie4 = !(IsMapNullBin(nNextRow, j+3));
			
				if (bDie2 && bDie3 && bDie4)	//Check Row #0
				{
					nNextRow = i+1;
					bDie1 = !(IsMapNullBin(nNextRow, j));
					bDie2 = !(IsMapNullBin(nNextRow, j+1));
					bDie3 = !(IsMapNullBin(nNextRow, j+2));
					bDie4 = !(IsMapNullBin(nNextRow, j+3));
					bDie5 = !(IsMapNullBin(nNextRow, j+4));

					if (bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)	//Check Row #2
					{
						nNextRow = i+2;
						bDie1 = !(IsMapNullBin(nNextRow, j));
						bDie2 = !(IsMapNullBin(nNextRow, j+1));
						bDie3 = !(IsMapNullBin(nNextRow, j+2));
						bDie4 = !(IsMapNullBin(nNextRow, j+3));
						bDie5 = !(IsMapNullBin(nNextRow, j+4));

						if (bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)	//Check Row #3
						{
							nNextRow = i+3;
							bDie1 = !(IsMapNullBin(nNextRow, j));
							bDie2 = !(IsMapNullBin(nNextRow, j+1));
							bDie3 = !(IsMapNullBin(nNextRow, j+2));
							bDie4 = !(IsMapNullBin(nNextRow, j+3));
							bDie5 = !(IsMapNullBin(nNextRow, j+4));

							if (bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)	//Check Row #4
							{
								nNextRow = i+4;
								//bDie1 = (IsMapNullBin(nNextRow, j));
								bDie2 = !(IsMapNullBin(nNextRow, j+1));
								bDie3 = !(IsMapNullBin(nNextRow, j+2));
								bDie4 = !(IsMapNullBin(nNextRow, j+3));
								//bDie5 = (IsMapNullBin(nNextRow, j+4));

								if (bDie2 && bDie3 && bDie4)	//Check Row #5
								{
									lULRow = i;
									lULCol = j;
									return TRUE;	//Return Pos-#6
								}
							}
						}
					}
				}
			}
		}
	}

	return FALSE;
}


//================================================================
// Get4x3HomeDieAlignRowColFromMap()
//   Created-By  : Andrew Ng
//   Date        : 12/9/2009 11:37:52 AM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferTable::Get4x3HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow, CONST LONG lStartCol)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		return FALSE;
	}
	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
	{
		return FALSE;
	}


	//5x5 region
	//==================
	// 1  2  3  4  5  6
	// 7  8  9  10 11 12
	// 13 14 15 16 17 18
	// 19 20 21 22 23 24
	// 25 26 27 28 29 30
	// 31 32 33 34 35 36
	//==================
	
	//5x5 region (NeoNeon)
	//=================
	// -  o  o  o  o  -		//ROw #0
	// o  x  x  x  x  o		//Row #1
	// o  x  x  x  x  o		//Row #2
	// o  x  x  x  x  o		//Row #3
	// -  o  o  o  0  -		//Row #5
	//=================


	BOOL bDie1, bDie2, bDie3, bDie4, bDie5, bDie6;

	for (int i=lStartRow; i<(LONG)ulNumOfRows; i++)
	{
		int nCurrStartCol = 0;
		if (i == lStartRow)
			nCurrStartCol = lStartCol;
					
		if (i > ((LONG)ulNumOfRows - 3))
			break;

		for (int j=nCurrStartCol; j<(LONG)ulNumOfCols; j++)
		{
			if (j > ((LONG)ulNumOfCols-5))
				continue;

			bDie1 = !(IsMapNullBin(i, j));
			bDie2 = !(IsMapNullBin(i, j+1));
			bDie3 = !(IsMapNullBin(i, j+2));
			bDie4 = !(IsMapNullBin(i, j+3));
			bDie5 = !(IsMapNullBin(i, j+4));
			bDie6 = !(IsMapNullBin(i, j+5));

			if (bDie1 && !bDie2 && !bDie3 && !bDie4 && !bDie5 && bDie6)	//Check Row #1
			{
				int nNextRow = i-1;
				if (nNextRow < 0)
					return FALSE;	//pattern must not be found on 1st row

				bDie2 = !(IsMapNullBin(nNextRow, j+1));
				bDie3 = !(IsMapNullBin(nNextRow, j+2));
				bDie4 = !(IsMapNullBin(nNextRow, j+3));
				bDie5 = !(IsMapNullBin(nNextRow, j+4));
			
				if (bDie2 && bDie3 && bDie4 && bDie5)	//Check Row #0
				{
					nNextRow = i+1;
					bDie1 = !(IsMapNullBin(nNextRow, j));
					bDie2 = !(IsMapNullBin(nNextRow, j+1));
					bDie3 = !(IsMapNullBin(nNextRow, j+2));
					bDie4 = !(IsMapNullBin(nNextRow, j+3));
					bDie5 = !(IsMapNullBin(nNextRow, j+4));
					bDie6 = !(IsMapNullBin(nNextRow, j+5));

					if (bDie1 && !bDie2 && !bDie3 && !bDie4 && !bDie5 && bDie6)	//Check Row #2
					{
						nNextRow = i+2;
						bDie1 = !(IsMapNullBin(nNextRow, j));
						bDie2 = !(IsMapNullBin(nNextRow, j+1));
						bDie3 = !(IsMapNullBin(nNextRow, j+2));
						bDie4 = !(IsMapNullBin(nNextRow, j+3));
						bDie5 = !(IsMapNullBin(nNextRow, j+4));
						bDie6 = !(IsMapNullBin(nNextRow, j+5));

						if (bDie1 && !bDie2 && !bDie3 && !bDie4 && !bDie5 && bDie6)	//Check Row #3
						{
							nNextRow = i+3;
							//bDie1 = (IsMapNullBin(nNextRow, j));
							bDie2 = !(IsMapNullBin(nNextRow, j+1));
							bDie3 = !(IsMapNullBin(nNextRow, j+2));
							bDie4 = !(IsMapNullBin(nNextRow, j+3));
							bDie5 = !(IsMapNullBin(nNextRow, j+4));
							//bDie6 = !(IsMapNullBin(nNextRow, j+5));

							if (bDie2 && bDie3 && bDie4 && bDie5)	//Check Row #4
							{
								lULRow = i;
								lULCol = j;
								return TRUE;	//Return Pos-#6
							}
						}
					}
				}
			}
		}
	}

	return FALSE;
}


//================================================================
// Get1x3HomeDieAlignRowColFromMap()
//   Created-By  : Andrew Ng
//   Date        : 11/30/2009 11:56:59 AM
//   Description : 
//   Remarks     : For Sunrich #1023 wafer type
//================================================================
BOOL CWaferTable::Get1x3HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, CONST LONG lStartRow, CONST LONG lStartCol)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		return FALSE;
	}
	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
	{
		return FALSE;
	}


	//5x5 region
	//===============
	// 1  2  3  
	// 4  5  6  
	// 7  8  9
	// 10 11 12
	// 13 14 15
	//===============
	
	//5x5 region (NeoNeon)
	//===============
	// -  o  -		//ROw #0
	// o  x  o		//Row #1
	// o  x  o		//Row #2
	// o  x  o		//Row #3
	// -  o  -		//Row #4
	//===============


	BOOL bDie1, bDie2, bDie3; //, bDie4, bDie5;

	for (int i=lStartRow; i<(LONG)ulNumOfRows; i++)
	{
		int nCurrStartCol = 0;
		if (i == lStartRow)
			nCurrStartCol = lStartCol;
					
		if (i > ((LONG)ulNumOfRows - 3))
			break;

		for (int j=nCurrStartCol; j<(LONG)ulNumOfCols; j++)
		{
			if (j > ((LONG)ulNumOfCols-5))
				continue;

			bDie1 = !(IsMapNullBin(i, j));
			bDie2 = !(IsMapNullBin(i, j+1));
			bDie3 = !(IsMapNullBin(i, j+2));

			if (bDie1 && !bDie2 && bDie3)	//If Row #1 matches
			{
				int nNextRow = i-1;
				if (nNextRow < 0)
					return FALSE;			//#cannot be 1st row in map!

				bDie2 = !(IsMapNullBin(nNextRow, j+1));
			
				if (bDie2)						//Then if Row #0 matches as well
				{
					nNextRow = i+1;
					bDie1 = !(IsMapNullBin(nNextRow, j));
					bDie2 = !(IsMapNullBin(nNextRow, j+1));
					bDie3 = !(IsMapNullBin(nNextRow, j+2));

					if (bDie1 && !bDie2 && bDie3)	//Then if Row #3 matches
					{
						nNextRow = i+2;
						bDie1 = !(IsMapNullBin(nNextRow, j));
						bDie2 = !(IsMapNullBin(nNextRow, j+1));
						bDie3 = !(IsMapNullBin(nNextRow, j+2));

						if (bDie1 && !bDie2 && bDie3)	//Then if Row #4 matches
						{
							nNextRow = i+3;
							bDie2 = !(IsMapNullBin(nNextRow, j+1));

							if (bDie2)						//All patterns on 5 rows matches; return OK (Found)
							{
								lULRow = i;
								lULCol = j;
								return TRUE;				//Return Pos-#4
							}
						}
					}
				}
			}
		}
	}

	return FALSE;
}


//================================================================
// FindSunrich1x3AlignPatternFromMap()
//   Created-By  : Andrew Ng
//   Date        : 11/30/2009 12:13:16 PM
//   Description : 
//   Remarks     : For Sunrich #1023 wafer type
//================================================================
BOOL CWaferTable::FindSunrich1x3AlignPatternFromMap()	//Type #1023 
{
	//Check if all 5 patterns exist on map
	ULONG ulLastRow=0, ulLastCol=0;
	LONG lUserRow = 0, lUserCol = 0;
	ULONG ulCurrAlignRow = m_ulAlignRow;
	ULONG ulCurrAlignCol = m_ulAlignCol;


	if (!SearchRectPatternFromMap(ulLastRow, ulLastCol, P1x3, 3))	//Must be able to find 3 1x3 patterns on map
	{
		CString szErr = _T("Error: not all 3 1x3 EMPTY-DIE patterns are found; AUTOBOND is aborted.");
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Check if more than 5 patterns exist on map
	if (SearchRectPatternFromMap(ulLastRow, ulLastCol, P1x3, 4))
	{
		CString szErr = _T("Error: more than 3 1x3 EMPTY-DIE patterns are found; AUTOBOND is aborted.");
		HmiMessage_Red_Yellow(szErr);
		SetStatusMessage(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Use pattern #3 -> center 3x3 pattern on NeoNeon wafer for alignment
	if (!SearchRectPatternFromMap(ulCurrAlignRow, ulCurrAlignCol, P1x3, 2))
	{
		CString szErr = _T("Error: center 1x3 pattern not found; AUTOBOND is aborted.");
		HmiMessage_Red_Yellow(szErr);
		SetStatusMessage(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Return 3x3 center-die position
	// Use OFFSET-XY to adjust final ref-die position around this 3x3 pattern
	m_ulAlignRow = ulCurrAlignRow + m_lOrgMapRowOffset;
	m_ulAlignCol = ulCurrAlignCol + m_lOrgMapColOffset;

	//CString szTemp;
	ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, lUserRow, lUserCol);

	//Log status on screen
	CString szMsg;
	szMsg.Format("Sunrich 2nd 1x3 pattern found at (%d, %d); offset = (%d %d)", 
		lUserRow, lUserCol, m_lOrgMapRowOffset, m_lOrgMapColOffset);
	SetStatusMessage(szMsg);
	SetErrorMessage(szMsg);

	return TRUE;
}


//================================================================
// FindSunrich3x3AlignPatternFromMap()
//   Created-By  : Andrew Ng
//   Date        : 11/30/2009 12:13:21 PM
//   Description : 
//   Remarks     : For Sunrich #1212 wafer type
//================================================================
BOOL CWaferTable::FindSunrich3x3AlignPatternFromMap()	//Type #1212
{
	//Check if all 5 patterns exist on map
	ULONG ulLastRow=0, ulLastCol=0;
	LONG lUserRow = 0, lUserCol = 0;
	ULONG ulCurrAlignRow = m_ulAlignRow;
	ULONG ulCurrAlignCol = m_ulAlignCol;


	if (!SearchRectPatternFromMap(ulLastRow, ulLastCol, P3x3, 3))
	{
		CString szErr = _T("Error: not all 3 3x3 EMPTY-DIE patterns are found; AUTOBOND is aborted.");
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Check if more than 5 patterns exist on map
	if (SearchRectPatternFromMap(ulLastRow, ulLastCol, P3x3, 4))
	{
		CString szErr = _T("Error: more than 3 3x3 EMPTY-DIE patterns are found; AUTOBOND is aborted.");
		HmiMessage_Red_Yellow(szErr);
		SetStatusMessage(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Use pattern #3 -> center 3x3 pattern on NeoNeon wafer for alignment
	if (!SearchRectPatternFromMap(ulCurrAlignRow, ulCurrAlignCol, P3x3, 2))
	{
		CString szErr = _T("Error: center 3x3 pattern not found; AUTOBOND is aborted.");
		HmiMessage_Red_Yellow(szErr);
		SetStatusMessage(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Return 3x3 center-die position
	// Use OFFSET-XY to adjust final ref-die position around this 3x3 pattern
	m_ulAlignRow = ulCurrAlignRow + m_lOrgMapRowOffset;
	m_ulAlignCol = ulCurrAlignCol + m_lOrgMapColOffset;

	//CString szTemp;
	ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, lUserRow, lUserCol);

	//Log status on screen
	CString szMsg;
	szMsg.Format("Sunrich 2nd 3x3 pattern found at (%d, %d); offset = (%d %d)", 
		lUserRow, lUserCol, m_lOrgMapRowOffset, m_lOrgMapColOffset);
	SetStatusMessage(szMsg);
	SetErrorMessage(szMsg);

	return TRUE;
}

	
//================================================================
// FindSunrich3x4AlignPatternFromMap()
//   Created-By  : Andrew Ng
//   Date        : 11/30/2009 12:13:25 PM
//   Description : 
//   Remarks     : For Sunrich #1213 wafer type
//================================================================
BOOL CWaferTable::FindSunrich3x4AlignPatternFromMap()	//Type #1213
{
	//Check if all 5 patterns exist on map
	ULONG ulLastRow=0, ulLastCol=0;
	LONG lUserRow = 0, lUserCol = 0;
	ULONG ulCurrAlignRow = m_ulAlignRow;
	ULONG ulCurrAlignCol = m_ulAlignCol;


	if (!SearchRectPatternFromMap(ulLastRow, ulLastCol, P3x4, 3))	//Must be able to find 3 1x3 patterns on map
	{
		CString szErr = _T("Error: not all 3 3x4 EMPTY-DIE patterns are found; AUTOBOND is aborted.");
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Use pattern #3 -> center 3x3 pattern on NeoNeon wafer for alignment
	if (!SearchRectPatternFromMap(ulCurrAlignRow, ulCurrAlignCol, P3x4, 2))
	{
		CString szErr = _T("Error: 2nd 3x4 pattern not found; AUTOBOND is aborted.");
		HmiMessage_Red_Yellow(szErr);
		SetStatusMessage(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	//Return 3x3 center-die position
	// Use OFFSET-XY to adjust final ref-die position around this 3x3 pattern
	m_ulAlignRow = ulCurrAlignRow + m_lOrgMapRowOffset;
	m_ulAlignCol = ulCurrAlignCol + m_lOrgMapColOffset;

	//CString szTemp;
	ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, lUserRow, lUserCol);

	//Log status on screen
	CString szMsg;
	szMsg.Format("Sunrich 2nd 3x4 pattern found at (%d, %d); offset = (%d %d)", 
		lUserRow, lUserCol, m_lOrgMapRowOffset, m_lOrgMapColOffset);
	SetStatusMessage(szMsg);
	SetErrorMessage(szMsg);

	return TRUE;
}


BOOL CWaferTable::FindNichiaTEGPatternFromMap(ULONG& ulAsmRow, ULONG& ulAsmCol, CONST UCHAR ucIndex, 
											  CONST BOOL bCreate, CONST BOOL bPartialWafer)
{
	//0=800x300, 1=800x600, 2=600x600
	UCHAR ucWaferType = CMS896AStn::m_oNichiaSubSystem.m_ucWaferType;
	eWT_Pattern ePattern;

	CString szMsg;
	szMsg.Format("FindNichiaTEGPatternFromMap with Index=%d ...", ucIndex);
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

//	CString szFileName = gszROOT_DIRECTORY + "\\Exe\\WaferHole.txt";
//	DeleteFile(szFileName);

	switch (ucWaferType)
	{
	case 1:
		ePattern = NICHIA8x6;
		break;
	case 2:
		ePattern = NICHIA6x6;
		break;
	case 0:
	default:
		ePattern = NICHIA8x3;
		break;
	}


	ULONG ulCurrAlignRow = 0;	//m_ulAlignRow;
	ULONG ulCurrAlignCol = 0;	//m_ulAlignCol;

	//Use pattern #1 -> TEG pattern in Nichia wafer for alignment
	if (!SearchRectPatternFromMap(ulCurrAlignRow, ulCurrAlignCol, ePattern, ucIndex, bPartialWafer))
	{
		CString szErr;
		switch (ucWaferType)	//v4.41T4
		{
		case 1:
			//ePattern = NICHIA8x6;
			szErr.Format("Nichia: TEG #%d pattern not found (WType = %d (800x600)).", ucIndex, ucWaferType);
			break;
		case 2:
			//ePattern = NICHIA6x6;
			szErr.Format("Nichia: TEG #%d pattern not found (WType = %d (600x600)).", ucIndex, ucWaferType);
			break;
		case 0:
		default:
			//ePattern = NICHIA8x3;
			szErr.Format("Nichia: TEG #%d pattern not found (WType = %d (800x300)).", ucIndex, ucWaferType);
			break;
		}

		SetStatusMessage(szErr);
		SetErrorMessage(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	if (bCreate)
		WriteNichiaTEGPatternToAlignFile(ulCurrAlignRow, ulCurrAlignCol, FALSE, bPartialWafer);	//v4.41T5
	else
		WriteNichiaTEGPatternToAlignFile(ulCurrAlignRow, ulCurrAlignCol, TRUE);

	LONG lUserRow, lUserCol;
	ConvertAsmToOrgUser(ulCurrAlignRow, ulCurrAlignCol, lUserRow, lUserCol);	
	szMsg.Format("FindNichiaTEGPatternFromMap: pattern found at U(%ld, %ld)", lUserRow, lUserCol);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
//HmiMessage(szMsg);

	ulAsmRow = ulCurrAlignRow;
	ulAsmCol = ulCurrAlignCol;
	return TRUE;
}


BOOL CWaferTable::GetNichia8030HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol,  
														 CONST UCHAR ucTEGType,			//v4.40T13
														 CONST LONG lStartRow, CONST LONG lStartCol)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;

	if (!m_WaferMapWrapper.IsMapValid())
	{
		//AfxMessageBox("Map is not valid", MB_SYSTEMMODAL);
		return FALSE;
	}

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		//AfxMessageBox("Map dimension is failed!", MB_SYSTEMMODAL);
		return FALSE;
	}

	CString szLog;
	szLog.Format("Searching Nichia TEG pattern for 800x300 wafer ... %ld  %ld", ulNumOfRows, ulNumOfCols);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
	{
		return FALSE;
	}

	//TEG #0 region for Nichia 800x300 wafer
	//===========================================
	// -  o  o  o  o  o  o  o  o  -		//Row #0
	// o  o  o  x  x  x  x  x  x  o		//Row #1
	// o  x  x	x  x  x  x  x  x  o		//Row #2
	// o  x  x	x  x  x  x  x  x  o		//Row #2
	// -  o  o  o  o  o  o  o  o  -		//Row #4
	//===========================================
	// 0  1  2  3  4  5  6  7  8  9		//Index

	//TEG #1 region for Nichia 800x300 wafer
	//===========================================
	// -  o  o  o  o  o  o  o  o  -		//Row #0
	// o  o  o  x  x  x  x  x  x  o		//Row #1
	// o  x  x	x  x  x  x  x  x  o		//Row #2
	// -  o  o  o  o  o  o  o  o  -		//Row #3
	//===========================================
	// 0  1  2  3  4  5  6  7  8  9		//Index

	BOOL bDie0, bDie1, bDie2, bDie3, bDie4, bDie5, bDie6, bDie7, bDie8, bDie9;
	INT nNextRow = 0;
	UCHAR ucTEGGrade = 206 + m_WaferMapWrapper.GetGradeOffset();

	for (int i=lStartRow; i<(LONG)ulNumOfRows; i++)
	{
		int nCurrStartCol = 0;
		if (i == lStartRow)
			nCurrStartCol = lStartCol;
					
		if (i > ((LONG)ulNumOfRows - 3))
			break;

		for (int j=nCurrStartCol; j<(LONG)ulNumOfCols; j++)
		{
			if (j > ((LONG)ulNumOfCols-8))
				continue;
			if (j < 3)
				continue;

			bDie3 = !(m_WaferMapWrapper.GetGrade(i, j)	 == ucTEGGrade);
			bDie4 = !(m_WaferMapWrapper.GetGrade(i, j+1) == ucTEGGrade);
			bDie5 = !(m_WaferMapWrapper.GetGrade(i, j+2) == ucTEGGrade);
			bDie6 = !(m_WaferMapWrapper.GetGrade(i, j+3) == ucTEGGrade);
			bDie7 = !(m_WaferMapWrapper.GetGrade(i, j+4) == ucTEGGrade);
			bDie8 = !(m_WaferMapWrapper.GetGrade(i, j+5) == ucTEGGrade);

			bDie1 = !(m_WaferMapWrapper.GetGrade(i, j-2) == ucTEGGrade);
			bDie2 = !(m_WaferMapWrapper.GetGrade(i, j-1) == ucTEGGrade);

			if (bDie1 && bDie2 && !bDie3 && !bDie4 && !bDie5 && !bDie6 && !bDie7 && !bDie8)		//If Row #1 matches
			{
				nNextRow = i+1;

				bDie0 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-3)	== ucTEGGrade);
				bDie1 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-2) == ucTEGGrade);
				bDie2 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-1) == ucTEGGrade);
				bDie3 = !(m_WaferMapWrapper.GetGrade(nNextRow, j)	== ucTEGGrade);
				bDie4 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+1) == ucTEGGrade);
				bDie5 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+2) == ucTEGGrade);
				bDie6 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+3) == ucTEGGrade);
				bDie7 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+4) == ucTEGGrade);
				bDie8 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+5) == ucTEGGrade);
				bDie9 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+6) == ucTEGGrade);

				if (bDie0 && !bDie1 && !bDie2 && !bDie3 && !bDie4 && !bDie5 && !bDie6 && !bDie7 && !bDie8 && bDie9)			//If Row #2 matches
				{
					nNextRow = i+2;

					if (ucTEGType == 1)				//v4.40T13
					{
						lULRow = i;
						lULCol = j;
						return TRUE;				//Return Pos-#2
					}

					bDie0 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-3)	== ucTEGGrade);
					bDie1 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-2) == ucTEGGrade);
					bDie2 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-1) == ucTEGGrade);
					bDie3 = !(m_WaferMapWrapper.GetGrade(nNextRow, j)	== ucTEGGrade);
					bDie4 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+1) == ucTEGGrade);
					bDie5 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+2) == ucTEGGrade);
					bDie6 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+3) == ucTEGGrade);
					bDie7 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+4) == ucTEGGrade);
					bDie8 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+5) == ucTEGGrade);
					bDie9 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+6) == ucTEGGrade);

					if (bDie0 && !bDie1 && !bDie2 && !bDie3 && !bDie4 && !bDie5 && !bDie6 && !bDie7 && !bDie8 && bDie9)		//If Row #3 matches
					{
						lULRow = i;
						lULCol = j;
						return TRUE;				//Return Pos-#4
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL CWaferTable::GetNichia8060HomeDieAlignRowColFromMap(LONG& lULRow, LONG& lULCol, 
														 CONST UCHAR ucTEGType,			//v4.40T13
														 CONST LONG lStartRow, CONST LONG lStartCol)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;

	if (!m_WaferMapWrapper.IsMapValid())
	{
		//AfxMessageBox("Map is not valid", MB_SYSTEMMODAL);
		return FALSE;
	}

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		//AfxMessageBox("Map dimension is failed!", MB_SYSTEMMODAL);
		return FALSE;
	}

	CString szLog;
	szLog.Format("Searching Nichia TEG pattern for 800x600 wafer ... %ld  %ld", ulNumOfRows, ulNumOfCols);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);


	if ( (ulNumOfRows <= 0) || (ulNumOfCols <= 0) )
	{
		return FALSE;
	}

	//TEG Type #0 region for Nichia 800x600 wafer
	//================================
	// -  o  o  o  o  o  -	//Row #0
	// o  o  x  x  x  o		//Row #1
	// o  x  x  x  x  o		//Row #2
	// o  x  x  x  x  o		//Row #2
	// -  o  o  o  o  o  -	//Row #4
	//================================
	// 0  1  2  3  4  5  	//Index

	//TEG Type #1 region for Nichia 800x600 wafer
	//================================
	// -  o  o  o  o  o  -	//Row #0
	// o  o  x  x  x  o		//Row #1
	// o  x  x  x  x  o		//Row #2
	// -  o  o  o  o  o  -	//Row #3
	//================================
	// 0  1  2  3  4  5  	//Index

	BOOL bDie0, bDie1, bDie2, bDie3, bDie4, bDie5;
	INT nNextRow = 0;
	UCHAR ucTEGGrade = 206 + m_WaferMapWrapper.GetGradeOffset();

	for (int i = lStartRow; i < (int)ulNumOfRows; i++)
	{
		int nCurrStartCol = 0;
		if (i == lStartRow)
			nCurrStartCol = lStartCol;
					
		if (i > (int)(ulNumOfRows - 3))
			break;

		for (int j = nCurrStartCol; j < (int)ulNumOfCols; j++)
		{
			if (j > (int)(ulNumOfCols-3))
			{
				continue;
			}

			if (j < 2)
			{
				continue;
			}

			bDie0 = !(m_WaferMapWrapper.GetGrade(i, j-2) == ucTEGGrade);
			bDie1 = !(m_WaferMapWrapper.GetGrade(i, j-1) == ucTEGGrade);
			bDie2 = !(m_WaferMapWrapper.GetGrade(i, j)	 == ucTEGGrade);
			bDie3 = !(m_WaferMapWrapper.GetGrade(i, j+1) == ucTEGGrade);
			bDie4 = !(m_WaferMapWrapper.GetGrade(i, j+2) == ucTEGGrade);
			bDie5 = !(m_WaferMapWrapper.GetGrade(i, j+3) == ucTEGGrade);

			if (bDie0 && bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)			//If Row #1 matches
			{
				nNextRow = i+1;

				bDie0 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-2)	== ucTEGGrade);
				bDie1 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-1) == ucTEGGrade);
				bDie2 = !(m_WaferMapWrapper.GetGrade(nNextRow, j)   == ucTEGGrade);
				bDie3 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+1)	== ucTEGGrade);
				bDie4 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+2) == ucTEGGrade);
				bDie5 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+3) == ucTEGGrade);

				if (bDie0 && !bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)		//If Row #2 matches
				{
					nNextRow = i+2;

					if (ucTEGType == 1)				//v4.40T13
					{
						lULRow = i;
						lULCol = j;
						return TRUE;				//Return Pos-#2
					}

					bDie0 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-2)	== ucTEGGrade);
					bDie1 = !(m_WaferMapWrapper.GetGrade(nNextRow, j-1) == ucTEGGrade);
					bDie2 = !(m_WaferMapWrapper.GetGrade(nNextRow, j)   == ucTEGGrade);
					bDie3 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+1)	== ucTEGGrade);
					bDie4 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+2) == ucTEGGrade);
					bDie5 = !(m_WaferMapWrapper.GetGrade(nNextRow, j+3) == ucTEGGrade);

					if (bDie0 && !bDie1 && !bDie2 && !bDie3 && !bDie4 && bDie5)		//If Row #3 matches
					{
						lULRow = i;
						lULCol = j;
						return TRUE;				//Return Pos-#2
					}
				}
			}
		}
	}

	return FALSE;
}
	

BOOL CWaferTable::WriteNichiaTEGPatternToAlignFile(CONST ULONG ulStartRow, CONST ULONG ulStartCol, 
												   CONST BOOL bAppendFile, CONST BOOL bPartialWafer)
{
	if (!m_WaferMapWrapper.IsMapValid())
		return FALSE;

	//0=800x300, 1=800x600, 2=600x600
	UCHAR ucWaferType	= CMS896AStn::m_oNichiaSubSystem.m_ucWaferType;
	UCHAR ucTEGType		= CMS896AStn::m_oNichiaSubSystem.m_ucWaferTEGType;	//v4.40T13
	
	ULONG ulMaxNoOfRows = 0;
	ULONG ulMaxNoOfCols = 0;
	m_WaferMapWrapper.GetMapDimension(ulMaxNoOfRows, ulMaxNoOfCols);

	LONG lUserRow=0, lUserCol=0;
	LONG lPartialUserRow = 0;
	LONG lPartialUserCol = 0;
	UCHAR ucTEGGrade = 206 + m_WaferMapWrapper.GetGradeOffset();
	CString szFileName = gszROOT_DIRECTORY + "\\Exe\\WaferHole.txt";
	CString szLine;

	CStdioFile oAlignFile;
	BOOL bOpen = TRUE;
	if (bAppendFile)
	{
		bOpen = oAlignFile.Open(szFileName, CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
		if (bOpen)
			oAlignFile.SeekToEnd();
	}
	else
	{
		bOpen = oAlignFile.Open(szFileName, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
	}

	if (bOpen)
	{
		if (ucWaferType == 0)	//800x300 wafer
		{
			//TEG region for Nichia 800x300 wafer
			//===========================================
			// -  o  o  o  o  o  o  o  o  -		//Row #0
			// o  o  o  A  x  x  x  x  x  o		//Row #1
			// o  x  x	x  x  x  x  x  x  o		//Row #2
			// o  x  x	x  x  x  x  x  x  o		//Row #2
			// -  o  o  o  o  o  o  o  o  -		//Row #4
			//===========================================
			// 0  1  2  3  4  5  6  7  8  9		//Index

			//ConvertAsmToOrgUser(ulStartRow, ulStartCol-2, lUserRow, lUserCol);
			ULONG ulRow = ulStartRow;
			ULONG ulCol = ulStartCol - 2;

			for (INT i = (INT)ulRow; i <= (INT)ulRow + 2; i++)
			{
				for (INT j = (INT)ulCol; j <= (INT)ulCol + 8; j++)
				{
					if ( (m_WaferMapWrapper.GetGrade(i, j) == ucTEGGrade) && 
						 (m_WaferMapWrapper.GetReader()->IsInkedDie(i, j)) )		//anichia004
					{
						ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
						szLine.Format("%ld,%ld\n", lUserRow, lUserCol);
						oAlignFile.WriteString(szLine);
					}
				}
			}

			if (bPartialWafer)	//v4.41T5
			{
				ulRow = ulStartRow + 1;
				ulCol = ulStartCol + 10;

				for (INT j = (INT)ulCol; j < (INT)ulMaxNoOfCols; j++)
				{
					if (j == ulMaxNoOfCols)
						break;
					if (IsMapNullBin(ulRow, j))
						continue;
					if ( (m_WaferMapWrapper.GetGrade(ulRow, j) == ucTEGGrade) &&
						 (m_WaferMapWrapper.GetReader()->IsInkedDie(ulRow, j)) )		//anichia004
					{
						ConvertAsmToOrgUser(ulRow, j, lPartialUserRow, lPartialUserCol);
						szLine.Format("%ld,%ld\n", lPartialUserRow, lPartialUserCol);
						oAlignFile.WriteString(szLine);
						break;
					}
				}
			}
		}
		else if (ucWaferType == 1)	//800x600 wafer
		{
			//TEG region for Nichia 800x600 wafer
			//================================
			// -  o  o  o  o  o  -	//Row #0
			// o  o  A  x  x  o		//Row #1
			// o  x  x  x  x  o		//Row #2
			// o  x  x  x  x  o		//Row #2
			// -  o  o  o  o  o  -	//Row #4
			//================================
			// 0  1  2  3  4  5  	//Index

			ULONG ulRow = ulStartRow;
			ULONG ulCol = ulStartCol - 1;

			for (INT i = (INT)ulRow; i <= (INT)ulRow + 2; i++)
			{
				for (INT j = (INT)ulCol; j <= (INT)ulCol + 3; j++)
				{
					if ( (m_WaferMapWrapper.GetGrade(i, j) == ucTEGGrade) &&
						 (m_WaferMapWrapper.GetReader()->IsInkedDie(i, j)) )		//anichia004
					{
						ConvertAsmToOrgUser(i, j, lUserRow, lUserCol);
						szLine.Format("%ld,%ld\n", lUserRow, lUserCol);
						oAlignFile.WriteString(szLine);
					}
				}
			}

			if (bPartialWafer)	//v4.41T5
			{
				ulRow = ulStartRow + 1;
				ulCol = ulStartCol + 5;

				for (INT j = (INT)ulCol; j < (INT)ulMaxNoOfCols; j++)
				{
					if (j == ulMaxNoOfCols)
						break;
					if (IsMapNullBin(ulRow, j))
						continue;
					if ( (m_WaferMapWrapper.GetGrade(ulRow, j) == ucTEGGrade) &&
						 (m_WaferMapWrapper.GetReader()->IsInkedDie(ulRow, j)) )		//anichia004
					{
						ConvertAsmToOrgUser(ulRow, j,	lPartialUserRow, lPartialUserCol);
						szLine.Format("%ld,%ld\n",		lPartialUserRow, lPartialUserCol);
						oAlignFile.WriteString(szLine);
					}
				}
			}
		}
		else
		{
			bOpen = FALSE;
		}

		oAlignFile.Close();
	}

	return bOpen;
}


BOOL CWaferTable::FindNichiaTEGRefDieOnMap(CONST UCHAR ucIndex, ULONG& ulRow, ULONG& ulCol)
{
	if (!m_WaferMapWrapper.IsMapValid())
		return FALSE;

	ULONG ulNoOfRows=0, ulNoOfCols=0;
	m_WaferMapWrapper.GetMapDimension(ulNoOfRows, ulNoOfCols);
	if ((ulNoOfRows == 0) || (ulNoOfCols == 0))
		return FALSE;

	ULONG ulRow1 = (ULONG) (1.0 * ulNoOfRows / 3.0);
	ULONG ulRow2 = (ULONG) (2.0 * ulNoOfRows / 3.0);
	ULONG ulCol1 = (ULONG) (1.0 * ulNoOfCols / 3.0);
	ULONG ulCol2 = (ULONG) (2.0 * ulNoOfCols / 3.0);

	ULONG ulAsmRow=0, ulAsmCol=0;
	ULONG ulMapNumOfReferDice = m_WaferMapWrapper.GetNumberOfReferenceDice();
	UCHAR ucTEGRefGrade = 205 + m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucTEGGrade	= 206 + m_WaferMapWrapper.GetGradeOffset();

CString szLog;
//szLog.Format("No Of Ref Dice = %lu; %ld %ld", ulMapNumOfReferDice, ulNoOfRows, ulNoOfCols);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	for (INT i = 0; i < (INT)ulNoOfRows; i++)
	{
		for (INT j = 0; j < (INT)ulNoOfCols; j++)
		{
			if (m_WaferMapWrapper.GetGrade(i, j) == ucTEGRefGrade)
			{
				if ((i > (INT)ulRow1) && (i < (INT)ulRow2))
				{
					if ((j > (INT)ulCol1) && (j < (INT)ulCol2))
					{
						if ( (m_WaferMapWrapper.GetGrade(i, j+1) == ucTEGGrade) )
						{
							ulRow = i;
							ulCol = j;
							return TRUE;
						}
					}
				}
			}
		}
	}

/*
	if (ulMapNumOfReferDice <= 0)
		return FALSE;
	astReferList  = new WAF_CMapDieInformation[ulMapNumOfReferDice];
	m_WaferMapWrapper.GetReferenceDieList(astReferList, ulMapNumOfReferDice);

	for (INT i=0; i<m_ulMapNumOfReferDice; i++)
	{
		ulAsmRow = (LONG)astReferList[i].GetRow();
		ulAsmCol = (LONG)astReferList[i].GetColumn();
	
		if ( (ulAsmRow > ulRow1) && (ulAsmRow < ulRow2) )
		{
			if ( (ulAsmCol > ulCol1) && (ulAsmCol < ulCol2) )
			{
				ulRow = ulAsmRow;
				ulCol = ulAsmCol;
				return TRUE;
			}
		}
	}
*/
	return FALSE;
}

BOOL CWaferTable::FindAutoAlignHomeDieFromMapBYD(BOOL bAlarm)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0, ulRow, ulCol;
	unsigned long ulStartRow, ulEndRow, ulStartCol, ulEndCol;
	BOOL bDieH, bDie1, bDie2, bDie3, bDie4, bDie5, bDie6;
	ULONG ulHomeRow = 0, ulHomeCol = 0, ulExtraRow, ulExtraCol;
	BOOL bFindHome = FALSE, bExtraHome = FALSE;
	CString szMsg;

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		szMsg = "BYD Get Map Dimension error!";
		if( bAlarm )
			HmiMessage_Red_Back(szMsg, "Auto Align Wafer");
		SetErrorMessage(szMsg);
		return FALSE;
	}
	if( (ulNumOfRows<=3) || (ulNumOfCols<=3) )
	{
		szMsg = "BYD Map Dimension less than 3 row/columns!";
		if( bAlarm )
			HmiMessage_Red_Back(szMsg, "Auto Align Wafer");
		SetErrorMessage(szMsg);
		return FALSE;
	}

	if( ulNumOfRows<40 )
	{
		ulStartRow = 0;
		ulEndRow = ulNumOfRows;
	}
	else
	{
		ulStartRow = ulNumOfRows/2 - 20;
		ulEndRow = ulNumOfRows/2 + 20;
	}

	if( ulNumOfCols<40 )
	{
		ulStartCol = 0;
		ulEndCol = ulNumOfCols;
	}
	else
	{
		ulStartCol = ulNumOfCols/2 - 20;
		ulEndCol = ulNumOfCols/2 + 20;
	}

	// 1+3x2 MODE, x is null bin and wafer is refer die, left x is home die
	//===============
	// 1  1  1  1  
	// 1  x  x  1				bDie1	bDie2
	// x  x  x  1		bDieH	bDie3	bDie4
	// 1  x  x  1				bDie5	bDie6
	// 1  1  1  1
	//===============

	bFindHome = FALSE;
	bExtraHome = FALSE;
	for(ulRow=ulStartRow+1; ulRow<ulEndRow-1; ulRow++)
	{
		for(ulCol=ulStartCol+1; ulCol<ulEndCol-1; ulCol++)
		{
			bDieH = IsMapNullBin(ulRow,	ulCol);
			bDie1 = IsMapNullBin(ulRow-1,	ulCol+1);
			bDie2 = IsMapNullBin(ulRow-1,	ulCol+2);
			bDie3 = IsMapNullBin(ulRow,	ulCol+1);
			bDie4 = IsMapNullBin(ulRow,	ulCol+2);
			bDie5 = IsMapNullBin(ulRow+1,	ulCol+1);
			bDie6 = IsMapNullBin(ulRow+1,	ulCol+2);

			if( bDieH && bDie1 && bDie2 && bDie3 && bDie4 && bDie5 && bDie6 )
			{
				if( bFindHome==FALSE )
				{
					bFindHome = TRUE;
					ulHomeRow = ulRow;
					ulHomeCol = ulCol;
				}
				else
				{
					bExtraHome = TRUE;
					ulExtraRow = ulRow;
					ulExtraCol = ulCol;
					break;
				}
			}
		}
		if( bExtraHome )
			break;
	}

	if( bFindHome==FALSE )
	{
		szMsg = " Auto find home die from amp failure!\n Please align wafer manually.";
		if( bAlarm )
			HmiMessage_Red_Back(szMsg, "Auto Align Wafer");
		SetErrorMessage(szMsg);
	}

	if( bExtraHome )
	{
		bFindHome = FALSE;
		szMsg.Format(" Auto find home die from map failure!\n More than one pattern found at below:\n (%d,%d) and (%d,%d) matched.\n Please align wafer manually.", 
			ulHomeRow, ulHomeCol, ulExtraRow, ulExtraCol);
		if( bAlarm )
			HmiMessage_Red_Back(szMsg, "Auto Align Wafer");
		SetErrorMessage(szMsg);
	}

	if( bFindHome )
	{
		m_ulAlignRow = ulHomeRow;
		m_ulAlignCol = ulHomeCol;
		UCHAR ucReferBin = m_WaferMapWrapper.GetNullBin();
		if( m_WaferMapWrapper.GetReader() != NULL )
		{
			ucReferBin = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
		}
		if( IsMapNullBin(ulHomeRow, ulHomeCol) )
		{
			m_WaferMapWrapper.ChangeGrade(ulHomeRow, ulHomeCol, ucReferBin);
		}
		m_WaferMapWrapper.SetReferenceDie(ulHomeRow, ulHomeCol, TRUE);
		LONG lUserRefRow = 0, lUserRefCol = 0;
		ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRefRow, lUserRefCol);
		szMsg.Format("BYD find auto home die %d,%d, map %d,%d", ulHomeRow, ulHomeCol, lUserRefRow, lUserRefCol);
		SetErrorMessage(szMsg);
		ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, m_lRefHomeDieRowOffset, m_lRefHomeDieColOffset);
	}

	return bFindHome;
}

BOOL CWaferTable::MoveSoraaMapByOneDieUpDnFromHomeDie(BOOL& bIsDnDie)		//v4.08
{
	//** Used for triangular-die wafer at 0-degree T posn under fully-auto mode only **//

	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();

	LONG lX, lY, lT;
	GetEncoder(&lX, &lY, &lT);
	
	LONG lOrigX		= lX;
	LONG lOrigY		= lY;
	LONG lDiff_X	= 0;
	LONG lDiff_Y	= 1;	//Use the Good-die below Ref(0,0) (i.e.GD at (-1,0)) to align "0-degree" map

	//1. Move to DN die 
	lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
	lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
	XY_SafeMoveTo(lX, lY);	//Move to DN die below Ref(0,0) -> GD(-1,0)
	Sleep(200);

	if (SearchAndAlignDie(TRUE, TRUE, TRUE))
	{
		//OK if DN die is found from HOME posn
		bIsDnDie = TRUE;
		return TRUE;
	}

	Sleep(200);

	//2. Try to find UP die if DN die not available
	lX		= lOrigX;
	lY		= lOrigY;
	lDiff_X	= 0;
	lDiff_Y	= -1;		//Use the Good-die above Ref(0,0) (i.e.GD at (1,0)) to align "0-degree" map

	lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
	lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
	XY_SafeMoveTo(lX, lY);	//Move to UP die above Ref(0,0) -> GD(-1,0)
	Sleep(200);

	if (SearchAndAlignDie(TRUE, TRUE, TRUE))
	{
		//OK if DN die is found from HOME posn
		bIsDnDie = FALSE;
		return TRUE;
	}

//AfxMessageBox("Andrew: OK?", MB_SYSTEMMODAL);
	return FALSE;
}


//================================================================
//   Created-By  : Andrew Ng
//   Date        : 3/11/2008 3:04:32 PM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferTable::SpiralSearchHomeDie()
{
	BOOL bReturn = FALSE;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(LONG), &m_lSprialSize);
	LONG nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SpiralSearchHomeDie", stMsg);

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 36000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(2);	
		}
	}

	return bReturn;
}


//================================================================
// SearchAndAlignDie()
//   Created-By  : Andrew Ng
//   Date        : 3/4/2008 4:34:16 PM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CWaferTable::SearchAndAlignDie(CONST BOOL bNormalDie, CONST BOOL bSrch1DieWnd, CONST BOOL bDoComp, CONST UCHAR ucDieID)
{
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;

	REF_TYPE	stInfo;

	SRCH_TYPE	stSrchInfo;

	if( IsBurnIn() )
		return TRUE;

	if( bNormalDie==FALSE && IsOsramResortMode()==FALSE && IsCharDieInUse() )
	{
		LONG lCurrX, lCurrY, lCurrT;
		GetEncoder(&lCurrX, &lCurrY, &lCurrT);
		return BlkFunc2SearchCurrDie(&lCurrX, &lCurrY, !bSrch1DieWnd);
	}

	// 1.  Find any GOOD die under FOV
	stSrchInfo.bShowPRStatus	= FALSE;
	stSrchInfo.bNormalDie		= bNormalDie;		//Normal or REF die
	stSrchInfo.lRefDieNo		= ucDieID;

	if( bNormalDie==FALSE && IsSorraSortMode() && m_b2Parts1stPartDone )	// sort mode
		stSrchInfo.lRefDieNo = 2;
	stSrchInfo.bDisableBackupAlign = FALSE;

	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

	if (bSrch1DieWnd)
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);			//Use default Src Wnd
	else
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDieInFOV", stMsg);	//Use whole FOV for searching

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if ((!stInfo.bStatus) || (!stInfo.bFullDie))		//If no GOOD/REF die is found
	{
		return FALSE;
	}

	if ( stInfo.bFullDie && bDoComp)					//Return if no COMPENSATE is needed
	{
		//Do PR compensation
		LONG lCurrX, lCurrY, lCurrT;
		GetEncoder(&lCurrX, &lCurrY, &lCurrT);
		lCurrX	+= stInfo.lX;	
		lCurrY	+= stInfo.lY;

		XY_SafeMoveTo(lCurrX, lCurrY);
	}

	(*m_psmfSRam)["WaferTable"]["Die Orientation Good Die"] = stInfo.bGoodDie;

	return TRUE;
}

VOID CWaferTable::Blk1SetAlignParameter()
{
	m_pBlkFunc->SetBlockParameter(m_lMnSrchRefGrid,m_lMnSrchHomeGrid,m_lMnMaxJumpCtr,m_lMnMaxJumpEdge,m_lMnNoDieGrade,m_lMnAlignReset,m_lMnMinDieForPick);
}

VOID CWaferTable::Blk2SetAlignParameter()
{
	LONG lRefDieLrn = (*m_psmfSRam)["WaferPr"]["RefDie"]["Count"];

	// bond -> wafer map setup -> Align Wafer -> Block Function
	m_pBlkFunc2->Blk2SetSearch(m_lMnSrchRefGrid, m_lMnMaxJumpCtr, m_lMnMaxJumpEdge);
	m_pBlkFunc2->Blk2SetEmptyGradeMinCount(m_lEmptyGradeMinCount);	// for cree
	m_pBlkFunc2->Blk2SetAlignment(m_bMnEdgeAlign, m_lMnPassPercent, m_bOCRDigitalF);	// OSRAM setup -> wafer pr page
	m_pBlkFunc2->Blk2SetReferMode(IsCharDieInUse(), lRefDieLrn);
	m_pBlkFunc2->Blk2SetBlock4ReferCheckScore(m_lBlk4ReferCheckScore);
}

BOOL CWaferTable::Blk2FindAllReferDiePosn()
{
	m_lRealignDieNum = 0;
	Blk2SetAlignParameter();
    CMS896AApp::m_bStopAlign = FALSE;
	BOOL bAlign = m_pBlkFunc2->Blk2FoundAllReferDiePos();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( bAlign && pApp->GetCustomerName()=="Cree" )
	{
		m_pBlkFunc2->RemoveMissingReferDieInMap();
	}

	return bAlign;
}

BOOL CWaferTable::MoveSoraaOneDieUpDnFromAlign(BOOL& bIsDnDie)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	//** Used for triangular-die wafer at 0-degree T posn under fully-auto mode only **//
	LONG lRow, lCol, lX, lY;
	LONG lOrigX, lOrigY, lOrigT;
	LONG lDiff_X, lDiff_Y;

	pUtl->GetAlignPosition(lRow, lCol, lX, lY);

	BOOL bDieUp = TRUE;
	XY_SafeMoveTo(lX, lY);
	Sleep(200);

	BOOL bMapDieTipUp = TRUE, bWftDieTipUp = TRUE;
	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);
	if( m_ucMapRotation==2 )
	{
		if( labs(lUserRow+lUserCol)%2==0 )	// odd
			bMapDieTipUp = FALSE;
		else
			bMapDieTipUp = TRUE;
	}
	else	// suppose 0 degree
	{
		if( labs(lUserRow+lUserCol)%2==0 )	// odd
			bMapDieTipUp = TRUE;
		else
			bMapDieTipUp = FALSE;
	}
	// should search die and get die tip direction. then check map/wafer map or not

	// check it is normal die or reference die
	BOOL bNormalDie = TRUE;
	if( m_WaferMapWrapper.IsReferenceDie(lRow, lCol) )
		bNormalDie = FALSE;
	if( SearchAlignDieSoraaDie(bNormalDie, TRUE, FALSE, FALSE) )
	{
		LONG lDieRotation = (*m_psmfSRam)["WaferTable"]["AOI Die Rotation"];
		if( lDieRotation==0 )
		{
			pUtl->SetAlignDieRoation(0);
		}
		else
		{
			pUtl->SetAlignDieRoation(180);
			bDieUp = FALSE;
		}
		if( bNormalDie )
		{
			LONG lPrDieUp = (*m_psmfSRam)["WaferTable"]["Soraa AOI Die Tip Up"];
			if( lPrDieUp )
				bWftDieTipUp = TRUE;
			else
				bWftDieTipUp = FALSE;

			if( bWftDieTipUp!=bMapDieTipUp )
			{
				HmiMessage_Red_Back("Map selected die tip not match to wafer die, please check!", "AOI");
				return FALSE;
			}
		}
	}
	else
	{
		HmiMessage_Red_Back("can not find alignment die.", "AOI");
		return FALSE;
	}

	GetEncoder(&lOrigX, &lOrigY, &lOrigT);
	
	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= 0; // GetDiePitchX_Y();
	LONG lDiePitchY_X	= 0; // GetDiePitchY_X();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	if( bDieUp )
		lDiePitchY_Y = GetDiePitchY_Y()*2/3;
	else
		lDiePitchY_Y = GetDiePitchY_Y()*4/3;
	//1. Move to DN die 
	if (m_pWaferMapManager->IsMapHaveBin(lRow+1, lCol))
	{
		lX		= lOrigX;
		lY		= lOrigY;
		lDiff_X	= 0;
		lDiff_Y	= 1;	//Use the Good-die below Ref(0,0) (i.e.GD at (-1,0)) to align "0-degree" map
		lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		XY_SafeMoveTo(lX, lY);	//Move to DN die below Ref(0,0) -> GD(-1,0)
		Sleep(200);

		bNormalDie = TRUE;
		if( m_WaferMapWrapper.IsReferenceDie(lRow+1, lCol) )
			bNormalDie = FALSE;
		if (SearchAlignDieSoraaDie(bNormalDie, TRUE, TRUE, TRUE))
		{
			//OK if DN die is found from HOME posn
			bIsDnDie = TRUE;
			return TRUE;
		}
	}

	//2. Try to find UP die if DN die not available
	if( bDieUp )
		lDiePitchY_Y = GetDiePitchY_Y()*4/3;
	else
		lDiePitchY_Y = GetDiePitchY_Y()*2/3;
	if (m_pWaferMapManager->IsMapHaveBin(lRow-1, lCol))
	{
		lX		= lOrigX;
		lY		= lOrigY;
		lDiff_X	= 0;
		lDiff_Y	= -1;		//Use the Good-die above Ref(0,0) (i.e.GD at (1,0)) to align "0-degree" map

		lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
		XY_SafeMoveTo(lX, lY);	//Move to UP die above Ref(0,0) -> GD(-1,0)
		Sleep(200);

		bNormalDie = TRUE;
		if( m_WaferMapWrapper.IsReferenceDie(lRow-1, lCol) )
			bNormalDie = FALSE;
		if (SearchAlignDieSoraaDie(bNormalDie, TRUE, TRUE, TRUE))
		{
			//OK if DN die is found from HOME posn
			bIsDnDie = FALSE;
			return TRUE;
		}
	}

	HmiMessage_Red_Back("Please re-align wafer as no up/down die at home die.", "AOI");
	return FALSE;
}


BOOL CWaferTable::SearchAlignDieSoraaDie(CONST BOOL bNormalDie, CONST BOOL bSrchWnd, CONST BOOL bDoComp, CONST BOOL bRotateSearch)
{
	CString szMsg;
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;

	REF_TYPE	stInfo;

	SRCH_TYPE	stSrchInfo;

	// 1.  Find any GOOD die under FOV
	stSrchInfo.bShowPRStatus	= FALSE;
	stSrchInfo.bNormalDie		= bNormalDie;		//Normal or REF die
	stSrchInfo.lRefDieNo		= 1;
	stSrchInfo.bDisableBackupAlign = FALSE;

	szMsg.Format("pr Nml Die %d, SrchCurr %d, do comp %d, rotate %d", bNormalDie, bSrchWnd, bDoComp, bRotateSearch);
	SetErrorMessage(szMsg);

	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
//	if( bRotateSearch )
//		pUtl->SetSearchDieRoation(180);

	if (bSrchWnd)
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);			//Use default Src Wnd
	else
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDieInFOV", stMsg);	//Use whole FOV for searching

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

//	pUtl->SetSearchDieRoation(0);

	if ((!stInfo.bStatus) || (!stInfo.bFullDie))	//If no GOOD/REF die is found
	{
		return FALSE;
	}

	if (stInfo.bGoodDie && !bDoComp)				//Return if no COMPENSATE is needed
	{
		return TRUE;
	}


	//Do PR compensation
	LONG lCurrX, lCurrY, lCurrT;
	GetEncoder(&lCurrX, &lCurrY, &lCurrT);
	lCurrX	+= stInfo.lX;	
	lCurrY	+= stInfo.lY;

	XY_SafeMoveTo(lCurrX, lCurrY);
	Sleep(200);

	return TRUE;
}

LONG CWaferTable::AlignRectWafer_Resort_000()
{
	LONG	lUserConfirm = glHMI_CONTINUE;
	LONG	lIndexDieResult	= 1;
	ULONG	ulNumberOfRows = 0; 
	ULONG	ulNumberOfCols = 0;
	//CString szTemp;
	CString szTitle, szContent;
	BOOL bVertCheckFromHome	= FALSE;

	LONG lPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];

	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return WT_ALN_ERROR;
	}

	if( ulNumberOfRows==1 && ulNumberOfCols==1 )
	{
		return WT_ALN_OK;
	}

	ULONG ulVertTgtRow = ulNumberOfRows-1;
	ULONG ulHoriTgtCol = ulNumberOfCols-1;
	ULONG ulHomeRow = 0, ulHomeCol = 0;

	GetMapAlignHomeDie(ulHomeRow, ulHomeCol);
	m_ulAlignRow = ulHomeRow;
	m_ulAlignCol = ulHomeCol;
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_WaferMapWrapper.SetSelectedPosition(ulHomeRow, ulHomeCol);


	LONG lUserRow=0, lUserCol=0;
	CString szTemp;
	ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer Start Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	ConvertAsmToOrgUser(ulHomeRow, ulHoriTgtCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer hori Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	if( IsMapNullBin(ulVertTgtRow, ulHoriTgtCol) )
	{
		bVertCheckFromHome = TRUE;
		ConvertAsmToOrgUser(ulVertTgtRow, 0, lUserRow, lUserCol);
	}
	else
	{
		ConvertAsmToOrgUser(ulVertTgtRow, ulHoriTgtCol, lUserRow, lUserCol);
	}
	szTemp.Format("WT: Align Rect wafer vert Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	szTemp.Format("***** WT: Align RectWafer (fully-auto): map dimension (%lu, %lu)", ulNumberOfRows, ulNumberOfCols);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	LONG lBpCurrent_X	= m_lCurrent_X;
	LONG lBpCurrent_Y	= m_lCurrent_Y;

	szTemp.Format("WT: Align RectWafer posn at (%d, %d)",	m_lCurrent_X, m_lCurrent_Y);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//********************************************//
	//** check first bonded line *********//
	//********************************************//
	LONG lIndexDir = 1;
	ULONG ulStart = 0;
	for(ulStart=1; ulStart<(ulHoriTgtCol); ulStart++)
	{
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//RT

		Sleep(100);
		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error\n";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}
	Sleep(20);

	BOOL bLastTwo = FALSE, bLastOne = FALSE;
	BOOL bCtr = FALSE, bLeft = FALSE, bTop = FALSE, bRight = FALSE, bBtm = FALSE;

	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	LONG lDieState = RectWaferAroundDieCheck();
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>0 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}

	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//LT
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
		bLastOne = TRUE;

	if( bCtr || bRight )
		bLastTwo = TRUE;
	if( bTop )
		bLastTwo = FALSE;
	if( bRight )
		bLastOne = TRUE;
	if( RectWaferCornerDieCheck(1)==FALSE )
		bLastOne = FALSE;

	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		szTemp = "Up Right Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}

	if( bVertCheckFromHome )
	{
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		Sleep(100);
	}

	//********************************************//
	//** check vertical complete edge die*********//
	//********************************************//

	if( ulVertTgtRow==0 )	//	only one row
	{
		Sleep(lPRDelay);

		//Get corner pos
		szTemp.Format("WT: Align Rect wafer -> Jump to Home, only one row");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;

		return WT_ALN_OK;		
	}

	lIndexDir = 3;
	for(ulStart=1; ulStart<(ulVertTgtRow); ulStart++)
	{
		Sleep(lPRDelay);
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//DN

		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}

	Sleep(20);
	bLastTwo = FALSE;
	bLastOne = FALSE;
	bCtr = FALSE;
	bLeft = FALSE;
	bTop = FALSE;
	bRight = FALSE;
	bBtm = FALSE;
	lDieState = RectWaferAroundDieCheck();

	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>=10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>=1 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}
	//	426TX	2
	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//DN
	Sleep(20);
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
		bLastOne = TRUE;

	if( bVertCheckFromHome )
	{
		if( bCtr || bBtm )
			bLastTwo = TRUE;
		if( bLeft )
			bLastTwo = FALSE;
		if( bBtm )
			bLastOne = TRUE;

		if( RectWaferCornerDieCheck(2)==FALSE )
			bLastOne = FALSE;
	}
	else
	{
		if( bCtr || bBtm )
			bLastTwo = TRUE;
		if( bRight )
			bLastTwo = FALSE;
		if( bBtm )
			bLastOne = TRUE;

		if( RectWaferCornerDieCheck(3)==FALSE )
			bLastOne = FALSE;
	}

	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		if( bVertCheckFromHome )
			szTemp = "Lower Left Corner error?\nContinue";
		else
			szTemp = "Lower Right Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}

	Sleep(lPRDelay);

	//Get corner pos
	szTemp.Format("WT: Align Rect wafer -> Jump to Home after Counter corner check done");
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_ulCurrentRow = ulHomeRow;
	m_ulCurrentCol = ulHomeCol;
	m_lCurrent_X = lBpCurrent_X;
	m_lCurrent_Y = lBpCurrent_Y;

	return WT_ALN_OK;		
}

LONG CWaferTable::AlignRectWafer_Resort_090()
{
	LONG	lUserConfirm = glHMI_CONTINUE;
	LONG	lIndexDieResult	= 1;
	ULONG	ulNumberOfRows = 0; 
	ULONG	ulNumberOfCols = 0;
	CString szTitle, szContent;
	BOOL bHoriCheckFromHome	= FALSE;

	LONG lPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];

	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return WT_ALN_ERROR;
	}

	if( ulNumberOfRows==1 && ulNumberOfCols==1 )
	{
		return WT_ALN_OK;
	}

	ULONG ulVertTgtRow = ulNumberOfRows-1;
	ULONG ulHoriTgtCol = ulNumberOfCols-1;
	ULONG ulHomeRow = 0, ulHomeCol = 0;

	GetMapAlignHomeDie(ulHomeRow, ulHomeCol);
	m_ulAlignRow = ulHomeRow;
	m_ulAlignCol = ulHomeCol;
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_WaferMapWrapper.SetSelectedPosition(ulHomeRow, ulHomeCol);

	LONG lUserRow=0, lUserCol=0;
	CString szTemp;
	ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer Start Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	if( IsMapNullBin(ulVertTgtRow, 0) )
	{
		bHoriCheckFromHome = TRUE;
		ConvertAsmToOrgUser(0, 0, lUserRow, lUserCol);
	}
	else
	{
		ConvertAsmToOrgUser(ulVertTgtRow, 0, lUserRow, lUserCol);
	}
	szTemp.Format("WT: Align Rect wafer hori Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	ConvertAsmToOrgUser(ulVertTgtRow, ulHomeCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer vert Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//v4.21T8
	szTemp.Format("***** WT: Align RectWafer start (fully-auto): map dimension (%lu, %lu)", ulNumberOfRows, ulNumberOfCols);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	LONG lBpCurrent_X	= m_lCurrent_X;
	LONG lBpCurrent_Y	= m_lCurrent_Y;

	szTemp.Format("WT: Align RectWafer posn at (%d, %d)",	m_lCurrent_X, m_lCurrent_Y);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//********************************************//
	//** check first bonded line *********//
	//********************************************//
	ULONG ulStart = 0;
	LONG lIndexDir = 3;
	for(ulStart=1; ulStart<(ulVertTgtRow); ulStart++)
	{
		Sleep(lPRDelay);
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//DN

		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}

	Sleep(20);
	BOOL bLastTwo = FALSE, bLastOne = FALSE;
	BOOL bCtr = FALSE, bLeft = FALSE, bTop = FALSE, bRight = FALSE, bBtm = FALSE;
	LONG lDieState = RectWaferAroundDieCheck();

	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>=10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>=1 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}
	if( bCtr || bBtm )
		bLastTwo = TRUE;
	if( bRight )
		bLastTwo = FALSE;
	if( bBtm )
		bLastOne = TRUE;

	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//DN
	Sleep(20);
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
		bLastOne = TRUE;
	if( RectWaferCornerDieCheck(3)==FALSE )
		bLastOne = FALSE;
	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		szTemp = "Lower Right Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("090 vertical check done");

	if( bHoriCheckFromHome )
	{
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;
		Sleep(100);
	}

	//********************************************//
	//** check vertical complete edge die*********//
	//********************************************//

	if( ulHoriTgtCol==0 )	//	only one row
	{
		Sleep(lPRDelay);

		//Get corner pos
		szTemp.Format("WT: Align Rect wafer -> Jump to Home, only one row");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;

		return WT_ALN_OK;		
	}

	lIndexDir = 0;
	for(ulStart=1; ulStart<(ulHoriTgtCol); ulStart++)
	{
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//LT

		Sleep(100);
		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error\n";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}
	Sleep(20);
	bLastTwo = FALSE;
	bLastOne = FALSE;
	bCtr = FALSE;
	bLeft = FALSE;
	bTop = FALSE;
	bRight = FALSE;
	bBtm = FALSE;
	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	lDieState = RectWaferAroundDieCheck();
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>0 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}

	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//LT
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
		bLastOne = TRUE;
	if( bHoriCheckFromHome )
	{
		if( bCtr || bLeft )
			bLastTwo = TRUE;
		if( bTop )
			bLastTwo = FALSE;
		if( bLeft )
			bLastOne = TRUE;
		if( RectWaferCornerDieCheck(0)==FALSE )
			bLastOne = FALSE;
	}
	else
	{
		if( bCtr || bLeft )
			bLastTwo = TRUE;
		if( bBtm )
			bLastTwo = FALSE;
		if( bLeft )
			bLastOne = TRUE;
		if( RectWaferCornerDieCheck(2)==FALSE )
			bLastOne = FALSE;

	}
	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		if( bHoriCheckFromHome )
			szTemp = "Top Left Corner error?\nContinue";
		else
			szTemp = "Bottom Left Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}

	Sleep(lPRDelay);

	//Get corner pos
	szTemp.Format("WT: Align Rect wafer -> Jump to Home after Counter corner check done");
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_ulCurrentRow = ulHomeRow;
	m_ulCurrentCol = ulHomeCol;
	m_lCurrent_X = lBpCurrent_X;
	m_lCurrent_Y = lBpCurrent_Y;

	return WT_ALN_OK;		
}

LONG CWaferTable::AlignRectWafer_Resort_180()
{	//	426TX	2
	LONG	lUserConfirm = glHMI_CONTINUE;
	LONG	lIndexDieResult	= 1;
	ULONG	ulNumberOfRows = 0; 
	ULONG	ulNumberOfCols = 0;
	//CString szTemp;
	CString szTitle, szContent;
	BOOL bVertCheckFromHome	= FALSE;

	LONG lPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];

	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return WT_ALN_ERROR;
	}

	if( ulNumberOfRows==1 && ulNumberOfCols==1 )
	{
		return WT_ALN_OK;
	}

	ULONG ulVertTgtRow = ulNumberOfRows-1;
	ULONG ulHoriTgtCol = ulNumberOfCols-1;
	ULONG ulHomeRow = 0, ulHomeCol = 0;

	GetMapAlignHomeDie(ulHomeRow, ulHomeCol);
	m_ulAlignRow = ulHomeRow;
	m_ulAlignCol = ulHomeCol;
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_WaferMapWrapper.SetSelectedPosition(ulHomeRow, ulHomeCol);


	LONG lUserRow=0, lUserCol=0;
	CString szTemp;
	ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer Start Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	ConvertAsmToOrgUser(ulHomeRow, ulHoriTgtCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer hori Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	if( IsMapNullBin(0, 0) )
	{
		bVertCheckFromHome = TRUE;
		ConvertAsmToOrgUser(0, ulHoriTgtCol, lUserRow, lUserCol);
	}
	else
	{
		ConvertAsmToOrgUser(0, 0, lUserRow, lUserCol);
	}
	szTemp.Format("WT: Align Rect wafer vert Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	szTemp.Format("***** WT: Align RectWafer (fully-auto): map dimension (%lu, %lu)", ulNumberOfRows, ulNumberOfCols);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	LONG lBpCurrent_X	= m_lCurrent_X;
	LONG lBpCurrent_Y	= m_lCurrent_Y;

	szTemp.Format("WT: Align RectWafer posn at (%d, %d)",	m_lCurrent_X, m_lCurrent_Y);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//********************************************//
	//** check first bonded line *********//
	//********************************************//
	LONG lIndexDir = 0;
	ULONG ulStart = 0;
	for(ulStart=1; ulStart<(ulHoriTgtCol); ulStart++)
	{
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//LT

		Sleep(100);
		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error\n";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}
	Sleep(20);

	BOOL bLastTwo = FALSE, bLastOne = FALSE;
	BOOL bCtr = FALSE, bLeft = FALSE, bTop = FALSE, bRight = FALSE, bBtm = FALSE;

	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	LONG lDieState = RectWaferAroundDieCheck();
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>0 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}

	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//LT
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
		bLastOne = TRUE;

	if( bCtr || bLeft )
		bLastTwo = TRUE;
	if( bBtm )
		bLastTwo = FALSE;
	if( bLeft )
		bLastOne = TRUE;
	if( RectWaferCornerDieCheck(2)==FALSE )
		bLastOne = FALSE;

	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		szTemp = "Lower Left Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("180 horizontal check done");

	if( bVertCheckFromHome )
	{
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		Sleep(100);
	}

	//********************************************//
	//** check vertical complete edge die*********//
	//********************************************//

	if( ulVertTgtRow==0 )	//	only one row
	{
		Sleep(lPRDelay);

		//Get corner pos
		szTemp.Format("WT: Align Rect wafer -> Jump to Home, only one row");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;

		return WT_ALN_OK;		
	}

	lIndexDir = 2;
	for(ulStart=1; ulStart<(ulVertTgtRow); ulStart++)
	{
		Sleep(lPRDelay);
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//UP

		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}

	Sleep(20);
	bLastTwo = FALSE;
	bLastOne = FALSE;
	bCtr = FALSE;
	bLeft = FALSE;
	bTop = FALSE;
	bRight = FALSE;
	bBtm = FALSE;
	lDieState = RectWaferAroundDieCheck();

	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>=10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>=1 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}

	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//UP
	Sleep(20);
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
		bLastOne = TRUE;

	if( bVertCheckFromHome )
	{
		if( bCtr || bTop )
			bLastTwo = TRUE;
		if( bRight )
			bLastTwo = FALSE;
		if( bTop )
			bLastOne = TRUE;
		if( RectWaferCornerDieCheck(1)==FALSE )
			bLastOne = FALSE;
	}
	else
	{
		if( bCtr || bTop )
			bLastTwo = TRUE;
		if( bLeft )
			bLastTwo = FALSE;
		if( bTop )
			bLastOne = TRUE;
		if( RectWaferCornerDieCheck(0)==FALSE )
			bLastOne = FALSE;
	}

	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		if( bVertCheckFromHome )
			szTemp = "Top Right Corner error?\nContinue";
		else
			szTemp = "Top Left Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}

	Sleep(lPRDelay);

	//Get corner pos
	szTemp.Format("WT: Align Rect wafer -> Jump to Home after Counter corner check done");
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_ulCurrentRow = ulHomeRow;
	m_ulCurrentCol = ulHomeCol;
	m_lCurrent_X = lBpCurrent_X;
	m_lCurrent_Y = lBpCurrent_Y;

	return WT_ALN_OK;		
}

LONG CWaferTable::AlignRectWafer_Resort_270()
{
	LONG	lUserConfirm = glHMI_CONTINUE;
	LONG	lIndexDieResult	= 1;
	ULONG	ulNumberOfRows = 0; 
	ULONG	ulNumberOfCols = 0;
	//CString szTemp;
	CString szTitle, szContent;
	BOOL bHoriCheckFromHome	= FALSE;

	LONG lPRDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];

	szTitle.LoadString(HMB_WT_ALIGN_WAFER);
	szContent.LoadString(HMB_WT_INVALID_MAP_SIZE);

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumberOfRows, ulNumberOfCols))
	{
		HmiMessage_Red_Back(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return WT_ALN_ERROR;
	}

	if( ulNumberOfRows==1 && ulNumberOfCols==1 )
	{
		return WT_ALN_OK;
	}

	ULONG ulVertTgtRow = ulNumberOfRows-1;
	ULONG ulHoriTgtCol = ulNumberOfCols-1;
	ULONG ulHomeRow = 0, ulHomeCol = 0;

	GetMapAlignHomeDie(ulHomeRow, ulHomeCol);
	m_ulAlignRow = ulHomeRow;
	m_ulAlignCol = ulHomeCol;
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_WaferMapWrapper.SetSelectedPosition(ulHomeRow, ulHomeCol);

	LONG lUserRow=0, lUserCol=0;
	CString szTemp;
	ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer Start Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	if( IsMapNullBin(0, ulHoriTgtCol) )
	{
		bHoriCheckFromHome = TRUE;
		ConvertAsmToOrgUser(ulVertTgtRow, ulHoriTgtCol, lUserRow, lUserCol);
	}
	else
	{
		ConvertAsmToOrgUser(0, ulHoriTgtCol, lUserRow, lUserCol);
	}
	szTemp.Format("WT: Align Rect wafer hori Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	ConvertAsmToOrgUser(0, ulHomeCol, lUserRow, lUserCol);
	szTemp.Format("WT: Align Rect wafer vert Corner (%ld, %ld)", lUserRow, lUserCol);		
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//v4.21T8
	szTemp.Format("***** WT: Align RectWafer start (fully-auto): map dimension (%lu, %lu)", ulNumberOfRows, ulNumberOfCols);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	LONG lBpCurrent_X	= m_lCurrent_X;
	LONG lBpCurrent_Y	= m_lCurrent_Y;

	szTemp.Format("WT: Align RectWafer posn at (%d, %d)",	m_lCurrent_X, m_lCurrent_Y);
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	//********************************************//
	//** check first bonded line *********//
	//********************************************//
	ULONG ulStart = 0;
	LONG lIndexDir = 2;
	for(ulStart=1; ulStart<(ulVertTgtRow); ulStart++)
	{
		Sleep(lPRDelay);
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//UP

		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}

	Sleep(20);
	BOOL bLastTwo = FALSE, bLastOne = FALSE;
	BOOL bCtr = FALSE, bLeft = FALSE, bTop = FALSE, bRight = FALSE, bBtm = FALSE;
	LONG lDieState = RectWaferAroundDieCheck();

	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>=10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>=1 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}
	if( bCtr || bTop )
		bLastTwo = TRUE;
	if( bLeft )
		bLastTwo = FALSE;
	if( bTop )
		bLastOne = TRUE;

	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//up
	Sleep(20);
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
		bLastOne = TRUE;
	if( RectWaferCornerDieCheck(0)==FALSE )
		bLastOne = FALSE;
	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		szTemp = "Up Left Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog("270 vertical check done");

	if( bHoriCheckFromHome )
	{
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;
		Sleep(100);
	}

	//********************************************//
	//** check vertical complete edge die*********//
	//********************************************//

	if( ulHoriTgtCol==0 )	//	only one row
	{
		Sleep(lPRDelay);

		//Get corner pos
		szTemp.Format("WT: Align Rect wafer -> Jump to Home, only one row");
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

		XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
		m_ulCurrentRow = ulHomeRow;
		m_ulCurrentCol = ulHomeCol;
		m_lCurrent_X = lBpCurrent_X;
		m_lCurrent_Y = lBpCurrent_Y;

		return WT_ALN_OK;		
	}

	lIndexDir = 1;
	for(ulStart=1; ulStart<(ulHoriTgtCol); ulStart++)
	{
		lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//RT

		Sleep(100);
		//Display Message if need user to confirm align wafer process
		switch (lIndexDieResult)
		{
		case WT_ALN_OUT_MAPLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out map limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_SEARCH_DIE_ERROR:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> search die error\n";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_OUT_WAFLIMIT:
			Sleep(1000);
			szTemp = "WT: Align RectWafer err -> out limit";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			HmiMessage_Red_Back(szTemp, szTitle);
			return WT_ALN_ERROR;

		case WT_ALN_IS_NOGRADE:
		case WT_ALN_IS_GOOD:
		case WT_ALN_IS_DEFECT:
			break;

		case WT_ALN_IS_EMPTY:
		default:	//WT_ALN_IS_GOOD
			lUserConfirm = HmiMessage_Red_Back("No die found at current position!\nContinue?", szTitle, glHMI_MBX_CONTINUESTOP);
			szTemp.Format("WT: Align step fail; ERR=%ld", lIndexDieResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			break;
		}

		if (lUserConfirm != glHMI_CONTINUE)
		{
			SetErrorMessage("WT: Align Rect wafer -> Check no of columns fails");
			szTemp = "WT: AlignRect wafer -> Check no of columns fails";
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
			return WT_ALN_ERROR;		
		}
	}
	Sleep(20);
	bLastTwo = FALSE;
	bLastOne = FALSE;
	bCtr = FALSE;
	bLeft = FALSE;
	bTop = FALSE;
	bRight = FALSE;
	bBtm = FALSE;
	bCtr = (lIndexDieResult==WT_ALN_IS_NOGRADE) || (lIndexDieResult==WT_ALN_IS_GOOD) || (lIndexDieResult==WT_ALN_IS_DEFECT);
	lDieState = RectWaferAroundDieCheck();
	if( lDieState>=10000 )
	{
		bCtr = TRUE;
		lDieState -= 10000;
	}
	if( lDieState>=1000 )
	{
		bBtm = TRUE;
		lDieState -= 1000;
	}
	if( lDieState>=100 )
	{
		bRight = TRUE;
		lDieState -= 100;
	}
	if( lDieState>10 )
	{
		bTop = TRUE;
		lDieState -= 10;
	}
	if( lDieState>0 )
	{
		bLeft = TRUE;
		lDieState -= 1;
	}

	lIndexDieResult = SetAlignIndexPoint(TRUE, lIndexDir, 1);		//LT
	if( (lIndexDieResult==WT_ALN_IS_NOGRADE) || 
		(lIndexDieResult==WT_ALN_IS_GOOD) || 
		(lIndexDieResult==WT_ALN_IS_DEFECT) )
	{
		bLastOne = TRUE;
	}
	if( bHoriCheckFromHome )
	{
		if( bCtr || bRight )
			bLastTwo = TRUE;
		if( bBtm )
			bLastTwo = FALSE;
		if( bRight )
			bLastOne = TRUE;
		if( RectWaferCornerDieCheck(3)==FALSE )
			bLastOne = FALSE;
	}
	else
	{
		if( bCtr || bRight )
			bLastTwo = TRUE;
		if( bTop )
			bLastTwo = FALSE;
		if( bRight )
			bLastOne = TRUE;
		if( RectWaferCornerDieCheck(1)==FALSE )
			bLastOne = FALSE;

	}
	if( bLastTwo==FALSE || bLastOne==FALSE )
	{
		if( bHoriCheckFromHome )
			szTemp = "Bottom Right Corner error?\nContinue";
		else
			szTemp = "Top Right Corner error?\nContinue";
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);
		lUserConfirm = HmiMessage_Red_Back(szTemp, szTitle, glHMI_MBX_CONTINUESTOP);
		if( lUserConfirm!=glHMI_CONTINUE )
			return WT_ALN_ERROR;
	}

	Sleep(lPRDelay);

	//Get corner pos
	szTemp.Format("WT: Align Rect wafer -> Jump to Home after Counter corner check done");
	CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szTemp);

	XY_SafeMoveTo(lBpCurrent_X, lBpCurrent_Y);
	m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);
	m_ulCurrentRow = ulHomeRow;
	m_ulCurrentCol = ulHomeCol;
	m_lCurrent_X = lBpCurrent_X;
	m_lCurrent_Y = lBpCurrent_Y;

	return WT_ALN_OK;		
}

BOOL CWaferTable::FindAutoAlignHomeDieFromMap(BOOL bAlarm)
{
	unsigned long ulNumOfRows = 0, ulNumOfCols = 0;
	unsigned long ulStartRow = 0, ulEndRow = 0, ulStartCol = 0, ulEndCol = 0;
	ULONG ulHomeRow = 0, ulHomeCol = 0;
	CString szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szTitle = "Auto Find Home Die";
	CString szCtmName = pApp->GetCustomerName();

	if (!m_pWaferMapManager->GetWaferMapDimension(ulNumOfRows, ulNumOfCols))
	{
		szMsg = szCtmName + " Get Map Dimension error!";
		if( bAlarm )
			HmiMessage_Red_Back(szMsg, szTitle);
		SetErrorMessage(szMsg);
		return FALSE;
	}

	if( (ulNumOfRows<=(m_ulHomePitchRow*2+m_ulHomePatternRow)) || 
		(ulNumOfCols<=(m_ulHomePitchCol*2+m_ulHomePatternCol)) )
	{
		szMsg = szCtmName + " Map Dimension less than required rows/columns!";
		if( bAlarm )
			HmiMessage_Red_Back(szMsg, szTitle);
		SetErrorMessage(szMsg);
		return FALSE;
	}

	ulStartRow	= ulNumOfRows/2 - m_ulHomePitchRow/2;
	ulEndRow	= ulNumOfRows/2 + m_ulHomePitchRow/2;
	ulStartCol	= ulNumOfCols/2 - m_ulHomePitchCol/2;
	ulEndCol	= ulNumOfCols/2 + m_ulHomePitchCol/2;

	//===============
	// 1  1  1  1  1  
	// 1  x  x  x  1
	// 1  x  x  x  1
	// 1  x  x  x  1
	// 1  1  1  1  1
	//===============

	LONG lUserRow = 0, lUserCol = 0;
	SaveScanTimeEvent("    WFT: " + szCtmName + " is customer name");
	szMsg.Format("auto home setting: Pattern (%lu,%lu), Pitch (%lu,%lu), Key Die (%lu,%lu)", 
		m_ulHomePatternRow, m_ulHomePatternCol, m_ulHomePitchRow, m_ulHomePitchCol, m_ulHomeKeyDieRow, m_ulHomeKeyDieCol);
	SaveScanTimeEvent("    WFT: " + szMsg);
	ConvertAsmToOrgUser(ulStartRow, ulStartCol, lUserRow, lUserCol);
	szMsg.Format("auto home check UL %4d,%4d, map %4d,%4d", ulStartRow, ulStartCol, lUserRow, lUserCol);
	SaveScanTimeEvent("    WFT: " + szMsg);
	ConvertAsmToOrgUser(ulEndRow, ulEndCol, lUserRow, lUserCol);
	szMsg.Format("auto home check LR %4d,%4d, map %4d,%4d", ulEndRow, ulEndCol, lUserRow, lUserCol);
	SaveScanTimeEvent("    WFT: " + szMsg);

	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGrade = 0;
	BOOL bChkHomeOK = FALSE;
	ULONG ulRow = 0, ulCol = 0, ulChkRow = 0, ulChkCol = 0, ulULBaseRow = 0, ulULBaseCol = 0;
	for(ulRow=ulStartRow; ulRow<=ulEndRow; ulRow++)
	{
		for(ulCol=ulStartCol; ulCol<=ulEndCol; ulCol++)
		{
			ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade!=ucNullBin && IsScanMapNgGrade(ucGrade-ucOffset)==FALSE )
				continue;

			bChkHomeOK = TRUE;
			ulULBaseRow = ulRow;
			ulULBaseCol = ulCol;
			// to find center and check the pattern
			for(ulChkRow=0; ulChkRow<m_ulHomePatternRow; ulChkRow++)
			{
				for(ulChkCol=0; ulChkCol<m_ulHomePatternCol; ulChkCol++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulULBaseRow+ulChkRow, ulULBaseCol+ulChkCol);
					if( ucGrade!=ucNullBin && IsScanMapNgGrade(ucGrade-ucOffset)==FALSE )
					{
						bChkHomeOK = FALSE;
						break;
					}
				}
				if( bChkHomeOK==FALSE )
				{
					break;
				}
			}
			if( bChkHomeOK==FALSE )
			{
				continue;
			}

			ConvertAsmToOrgUser(ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			szMsg.Format("Find center ok at UL %d,%d, map %d,%d", ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			SaveScanTimeEvent("    WFT: " + szMsg);
			// to check the up part ok or not
			if ((LONG)ulRow < m_ulHomePitchRow)
			{
				bChkHomeOK = FALSE;
				continue;
			}
			ulULBaseRow = ulRow - m_ulHomePitchRow;
			ulULBaseCol = ulCol;
			ConvertAsmToOrgUser(ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			szMsg.Format("To check up at UL %d,%d, map %d,%d", ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			SaveScanTimeEvent("    WFT: " + szMsg);
			for(ulChkRow=0; ulChkRow<m_ulHomePatternRow; ulChkRow++)
			{
				for(ulChkCol=0; ulChkCol<m_ulHomePatternCol; ulChkCol++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulULBaseRow+ulChkRow, ulULBaseCol+ulChkCol);
					if( ucGrade!=ucNullBin && IsScanMapNgGrade(ucGrade-ucOffset)==FALSE )
					{
						bChkHomeOK = FALSE;
						break;
					}
				}
				if( bChkHomeOK==FALSE )
				{
					break;
				}
			}
			if( bChkHomeOK==FALSE )
			{
				continue;
			}

			// to check the low part ok or not
			if( (ulRow+m_ulHomePitchRow+m_ulHomePatternRow)>ulNumOfRows )
			{
				bChkHomeOK = FALSE;
				continue;
			}
			ulULBaseRow = ulRow + m_ulHomePitchRow;
			ulULBaseCol = ulCol;
			ConvertAsmToOrgUser(ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			szMsg.Format("To check low at UL %d,%d, map %d,%d", ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			SaveScanTimeEvent("    WFT: " + szMsg);
			for(ulChkRow=0; ulChkRow<m_ulHomePatternRow; ulChkRow++)
			{
				for(ulChkCol=0; ulChkCol<m_ulHomePatternCol; ulChkCol++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulULBaseRow+ulChkRow, ulULBaseCol+ulChkCol);
					if( ucGrade!=ucNullBin && IsScanMapNgGrade(ucGrade-ucOffset)==FALSE )
					{
						bChkHomeOK = FALSE;
						break;
					}
				}
				if( bChkHomeOK==FALSE )
				{
					break;
				}
			}
			if( bChkHomeOK==FALSE )
			{
				continue;
			}

			// to check the left part ok or not
			if ((LONG)ulCol < m_ulHomePitchCol)
			{
				bChkHomeOK = FALSE;
				continue;
			}
			ulULBaseRow = ulRow;
			ulULBaseCol = ulCol - m_ulHomePitchCol;
			ConvertAsmToOrgUser(ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			szMsg.Format("To check left at UL %d,%d, map %d,%d", ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			SaveScanTimeEvent("    WFT: " + szMsg);
			for(ulChkRow=0; ulChkRow<m_ulHomePatternRow; ulChkRow++)
			{
				for(ulChkCol=0; ulChkCol<m_ulHomePatternCol; ulChkCol++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulULBaseRow+ulChkRow, ulULBaseCol+ulChkCol);
					if( ucGrade!=ucNullBin && IsScanMapNgGrade(ucGrade-ucOffset)==FALSE )
					{
						bChkHomeOK = FALSE;
						break;
					}
				}
				if( bChkHomeOK==FALSE )
				{
					break;
				}
			}
			if( bChkHomeOK==FALSE )
			{
				continue;
			}

			// to check the right part ok or not
			if( (ulCol+m_ulHomePitchCol+m_ulHomePatternCol)>ulNumOfCols )
			{
				bChkHomeOK = FALSE;
				continue;
			}
			ulULBaseRow = ulRow;
			ulULBaseCol = ulCol + m_ulHomePitchCol;
			ConvertAsmToOrgUser(ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			szMsg.Format("To check right at UL %d,%d, map %d,%d", ulULBaseRow, ulULBaseCol, lUserRow, lUserCol);
			SaveScanTimeEvent("    WFT: " + szMsg);
			for(ulChkRow=0; ulChkRow<m_ulHomePatternRow; ulChkRow++)
			{
				for(ulChkCol=0; ulChkCol<m_ulHomePatternCol; ulChkCol++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulULBaseRow+ulChkRow, ulULBaseCol+ulChkCol);
					if( ucGrade!=ucNullBin && IsScanMapNgGrade(ucGrade-ucOffset)==FALSE )
					{
						bChkHomeOK = FALSE;
						break;
					}
				}
				if( bChkHomeOK==FALSE )
				{
					break;
				}
			}
			if( bChkHomeOK==FALSE )
			{
				continue;
			}

			if( bChkHomeOK )
			{
				ulHomeRow = ulRow + m_ulHomeKeyDieRow - 1;
				ulHomeCol = ulCol + m_ulHomeKeyDieCol - 1;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				szMsg.Format("Find home pattern UL base at %d,%d, map %d,%d", ulRow, ulCol, lUserRow, lUserCol);
				SaveScanTimeEvent("    WFT: " + szMsg);
				break;
			}
		}
		if( bChkHomeOK )
		{
			break;
		}
	}

	if( bChkHomeOK )
	{
		m_ulAlignRow = ulHomeRow;
		m_ulAlignCol = ulHomeCol;
		UCHAR ucReferBin = ucNullBin;
		if( m_WaferMapWrapper.GetReader() != NULL )
		{
			ucReferBin = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
		}
		if( IsMapNullBin(ulHomeRow, ulHomeCol) )
		{
			m_WaferMapWrapper.ChangeGrade(ulHomeRow, ulHomeCol, ucReferBin);
		}
		m_WaferMapWrapper.SetReferenceDie(ulHomeRow, ulHomeCol, TRUE);
		ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRow, lUserCol);
		szMsg.Format("autp find home key die at %d,%d, map %d,%d", ulHomeRow, ulHomeCol, lUserRow, lUserCol);
		SetErrorMessage(szMsg);
		SaveScanTimeEvent("    WFT: " + szMsg);
		ConvertAsmToOrgUser(m_ulAlignRow, m_ulAlignCol, m_lRefHomeDieRowOffset, m_lRefHomeDieColOffset);
	}
	else
	{
		szMsg = "Auto find home die failure!\n Please align wafer manually.";
		if( bAlarm )
			HmiMessage_Red_Back(szMsg, szTitle);
		szMsg = "Auto find home die failure! Please align wafer manually.";
		SetErrorMessage(szMsg);
		SaveScanTimeEvent("    WFT: " + szMsg);
	}

	return bChkHomeOK;
}

BOOL CWaferTable::AutoFindHomeDieFromMap(BOOL bAlarm)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() == _T("Sunrich"))			
	{
		//v3.64		//Added fully-auto special ALIGN pattern alignment for Sunrich
		if (!FindSunrich3x4AlignPatternFromMap())
		{
			if (!FindSunrich3x3AlignPatternFromMap())
			{
				if (!FindSunrich1x3AlignPatternFromMap())
				{
					return FALSE;
				}
			}
		}
	}

	if( pApp->GetCustomerName()==_T("BYD") )
	{
		return FindAutoAlignHomeDieFromMapBYD(bAlarm);
	}

	if( pApp->GetCustomerName()==_T("SanAn") )
	{
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		if( pUtl->GetPrescanRegionMode() )
			return FindReferDieFromMapSanAn(bAlarm);
		else
			return FindAutoAlignHomeDieFromMap(bAlarm);
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if( pApp->GetCustomerName()==_T("Semitek") && pUtl->GetPrescanRegionMode() && pSRInfo->GetTotalSubRegions()>1 )
	{
		return FindReferDieFromMapSemitek(bAlarm);
	}

	if( pApp->GetCustomerName()==_T("Renesas") )
	{
		return FindReferDieFromMapRenesas();
	}

	return TRUE;
}

BOOL CWaferTable::SearchDieInSearchWindow(BOOL bShowPrStatus, BOOL bNormalDie, LONG lRefDieNo, BOOL bDisableBackupAlign, REF_TYPE& stInfo)
{
	IPC_CServiceMessage stMsg;
	SRCH_TYPE	stSrchInfo;

	stSrchInfo.bShowPRStatus	= bShowPrStatus;
	stSrchInfo.bNormalDie		= bNormalDie;
	stSrchInfo.lRefDieNo		= lRefDieNo;
	stSrchInfo.bDisableBackupAlign = bDisableBackupAlign;

	//Search ref die
	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchDieInSearchWindow", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!stInfo.bStatus || !stInfo.bFullDie)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::SearchCurrentDie(BOOL bShowPrStatus, BOOL bNormalDie, LONG lRefDieNo, BOOL bDisableBackupAlign, REF_TYPE& stInfo)
{
	IPC_CServiceMessage stMsg;
	SRCH_TYPE	stSrchInfo;

	stSrchInfo.bShowPRStatus	= bShowPrStatus;
	stSrchInfo.bNormalDie		= bNormalDie;
	stSrchInfo.lRefDieNo		= lRefDieNo;
	stSrchInfo.bDisableBackupAlign = bDisableBackupAlign;

	//Search ref die
	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!stInfo.bStatus || !stInfo.bFullDie)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CWaferTable::CheckLongJumpPitchX(LONG lDeltaX, LONG lJumpSpanCol)
{
	LONG lDiePitchX_X = GetDiePitchX_X();
	LONG lTolerance = 40 + lJumpSpanCol/2;

	if( (lTolerance>0) && (labs(lDeltaX)>labs(lDiePitchX_X*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Check Long Jump X pitch %ld, tolerance %ld, delta %ld", lDiePitchX_X, lTolerance, lDeltaX);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);

		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CheckLongJumpPitchY(LONG lDeltaY, LONG lJumpSpanRow)
{
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	LONG lTolerance = 40 + lJumpSpanRow/2;

	if( (lTolerance>0) && (labs(lDeltaY)>labs(lDiePitchY_Y*lTolerance/100)) )
	{
		CString szLog;
		szLog.Format("Check Long Jump Y pitch %ld, tolerance %ld, delta %ld", lDiePitchY_Y, lTolerance, lDeltaY);
		CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szLog);
		return FALSE;
	}

	return TRUE;
}	// check manual align wafer pitch

BOOL CWaferTable::FindReferDieFromMapSemitek(BOOL bAlarm)
{
	//===============
	// 1  1  1  1  1  
	// 1  x  x  x  1
	// 1  x  H  x  1
	// 1  x  x  x  1
	// 1  1  1  1  1
	//===============

	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucReferBin = ucNullBin;
	if( m_WaferMapWrapper.GetReader() != NULL )
	{
		ucReferBin = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	}
	UCHAR ucGrade = 0;
	for (ULONG ulRow = (ULONG)GetMapValidMinRow(); ulRow <= (ULONG)GetMapValidMaxRow(); ulRow++)
	{
		BOOL bFindDieBefore = FALSE;
		for(ULONG ulCol = (ULONG)GetMapValidMinCol(); ulCol <= (ULONG)GetMapValidMaxCol(); ulCol++)
		{
			ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade!=ucNullBin )
			{
				bFindDieBefore = TRUE;
				continue;
			}
			// must a none empty die before in this row.
			if( bFindDieBefore==FALSE )
			{
				continue;
			}

			// check any die after the empty, if no any die after it, then no need to check.
			BOOL bFindDieAfter = FALSE;
			for(ULONG j = ulCol + 1; j <= (ULONG)GetMapValidMaxCol(); j++)
			{
				ucGrade = m_WaferMapWrapper.GetGrade(ulRow, j);
				if( ucGrade!=ucNullBin )
				{
					bFindDieAfter = TRUE;
					break;
				}
			}
			if( bFindDieAfter==FALSE )
			{
				break;
			}

			BOOL bFindMatchHole = TRUE;
			ULONG ulHoleMaxRow = ulRow + m_ulHomePatternRow;
			ULONG ulHoleMaxCol = ulCol + m_ulHomePatternCol;
			if (ulHoleMaxRow > (ULONG)GetMapValidMaxRow() || ulHoleMaxCol > (ULONG)GetMapValidMaxCol() )
			{
				continue;
			}

			for(ULONG ulHoleRow = ulRow; ulHoleRow < ulHoleMaxRow; ulHoleRow++)
			{
				for(ULONG ulHoleCol = ulCol; ulHoleCol < ulHoleMaxCol; ulHoleCol++)
				{
					BOOL bHasDieBefore = FALSE, bHasDieAfter = FALSE;
					// must a none empty die before in this row.
					for(LONG jCol = 0; jCol < (LONG)ulCol; jCol++)
					{
						ucGrade = m_WaferMapWrapper.GetGrade(ulHoleRow, jCol);
						if( ucGrade!=ucNullBin )
						{
							bHasDieBefore = TRUE;
							break;
						}
					}
					if( bHasDieBefore==FALSE )
					{
						bFindMatchHole = FALSE;
						break;
					}
					// check any die after the empty, if no any die after it, then no need to check.
					for(ULONG jCol = ulCol + 1; jCol <= (ULONG)GetMapValidMaxCol(); jCol++)
					{
						ucGrade = m_WaferMapWrapper.GetGrade(ulHoleRow, jCol);
						if( ucGrade!=ucNullBin )
						{
							bHasDieAfter = TRUE;
							break;
						}
					}
					if( bHasDieAfter==FALSE )
					{
						bFindMatchHole = FALSE;
						break;
					}

					// has die on it, fail
					ucGrade = m_WaferMapWrapper.GetGrade(ulHoleRow, ulHoleCol);
					if( ucGrade!=ucNullBin )
					{
						bFindMatchHole = FALSE;
						break;
					}
				}	// end hole points checking
				if( bFindMatchHole==FALSE )
				{
					break;
				}
			}	// end hole list checking

			if( bFindMatchHole )
			{
				ULONG ulLeftGood = 0, ulRightGood = 0, ulTopGood = 0, ulBottomGood = 0;
				for(ULONG ulChkRow=ulRow; ulChkRow<ulHoleMaxRow; ulChkRow++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulChkRow, ulCol-1);
					if( ucGrade!=ucNullBin )
						ulLeftGood++;
					ucGrade = m_WaferMapWrapper.GetGrade(ulChkRow, ulHoleMaxCol);
					if( ucGrade!=ucNullBin )
						ulRightGood++;
				}
				for(ULONG ulChkCol=ulCol; ulChkCol<ulHoleMaxCol; ulChkCol++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulRow-1, ulChkCol);
					if( ucGrade!=ucNullBin )
						ulTopGood++;
					ucGrade = m_WaferMapWrapper.GetGrade(ulHoleMaxRow, ulChkCol);
					if( ucGrade!=ucNullBin )
						ulBottomGood++;
				}

				if( ulTopGood>=m_ulHomePatternCol/2 && ulBottomGood>=m_ulHomePatternCol/2 &&
					ulLeftGood>=m_ulHomePatternRow/2 && ulBottomGood>=m_ulHomePatternRow/2 )
				{
					ULONG ulHomeRow = ulRow + m_ulHomeKeyDieRow-1;
					ULONG ulHomeCol = ulCol + m_ulHomeKeyDieCol-1;

					if( IsMapNullBin(ulHomeRow, ulHomeCol) )
					{
						m_WaferMapWrapper.ChangeGrade(ulHomeRow, ulHomeCol, ucReferBin);
					}
					m_WaferMapWrapper.SetReferenceDie(ulHomeRow, ulHomeCol, TRUE);
					LONG lUserRow, lUserCol;
					ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRow, lUserCol);
					CString szMsg;
					szMsg.Format("Found refer die at %d,%d", lUserRow, lUserCol);
					CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
				}
			}
		}
	}

	return TRUE;
}

BOOL CWaferTable::FindReferDieFromMapSanAn(BOOL bAlarm)
{
	for (ULONG ulRow = (ULONG)GetMapValidMinRow(); ulRow <= (ULONG)GetMapValidMaxRow(); ulRow++)
	{
		for(ULONG ulCol = (ULONG)GetMapValidMinCol(); ulCol <= (ULONG)GetMapValidMaxCol(); ulCol++)
		{
			if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) )
			{
				m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, FALSE);
			}
		}
	}

	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if ((pSRInfo->GetSubRows()<=1 && pSRInfo->GetSubCols() <= 1))
	{
		ULONG ulRow, ulCol;
		m_WaferMapWrapper.GetFirstDiePosition(ulRow, ulCol);
		if( m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) )
		{
			m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, FALSE);
		}
		ULONG ulHomeRow = 0, ulHomeCol = 0;
		GetMapAlignHomeDie(ulHomeRow, ulHomeCol);
		UCHAR ucReferGrade = ucNullBin;
		if( m_WaferMapWrapper.GetReader() != NULL )
		{
			ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
		}
		m_WaferMapWrapper.ChangeGrade(ulHomeRow, ulHomeCol, ucReferGrade);
		m_WaferMapWrapper.SetReferenceDie(ulHomeRow, ulHomeCol, TRUE);
		return TRUE;
	}

	//===============
	// 1  1  1  1  1  
	// 1  x  x  x  1
	// 1  x  H  x  1
	// 1  x  x  x  1
	// 1  1  1  1  1
	//===============

	UCHAR ucReferBin = ucNullBin;
	if( m_WaferMapWrapper.GetReader() != NULL )
	{
		ucReferBin = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	}
	UCHAR ucGrade = 0;
	for(ULONG ulRow = (ULONG)GetMapValidMinRow(); ulRow <= (ULONG)GetMapValidMaxRow(); ulRow++)
	{
		BOOL bFindDieBefore = FALSE;
		for(ULONG ulCol = (ULONG)GetMapValidMinCol(); ulCol <= (ULONG)GetMapValidMaxCol(); ulCol++)
		{
			ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade!=ucNullBin )
			{
				bFindDieBefore = TRUE;
				continue;
			}
			// must a none empty die before in this row.
			if( bFindDieBefore==FALSE )
			{
				continue;
			}

			// check any die after the empty, if no any die after it, then no need to check.
			BOOL bFindDieAfter = FALSE;
			for(ULONG j = ulCol + 1; j <= (ULONG)GetMapValidMaxCol(); j++)
			{
				ucGrade = m_WaferMapWrapper.GetGrade(ulRow, j);
				if( ucGrade!=ucNullBin )
				{
					bFindDieAfter = TRUE;
					break;
				}
			}
			if( bFindDieAfter==FALSE )
			{
				break;
			}

			BOOL bFindMatchHole = TRUE;
			ULONG ulHoleMaxRow = ulRow + m_ulHomePatternRow;
			ULONG ulHoleMaxCol = ulCol + m_ulHomePatternCol;
			if (ulHoleMaxRow > (ULONG)GetMapValidMaxRow() || ulHoleMaxCol > (ULONG)GetMapValidMaxCol() )
			{
				continue;
			}

			for(ULONG ulHoleRow=ulRow; ulHoleRow<ulHoleMaxRow; ulHoleRow++)
			{
				for (ULONG ulHoleCol = ulCol; ulHoleCol < ulHoleMaxCol; ulHoleCol++)
				{
					BOOL bHasDieBefore = FALSE, bHasDieAfter = FALSE;
					// must a none empty die before in this row.
					for (LONG jCol = 0; jCol < (LONG)ulCol; jCol++)
					{
						ucGrade = m_WaferMapWrapper.GetGrade(ulHoleRow, jCol);
						if( ucGrade!=ucNullBin )
						{
							bHasDieBefore = TRUE;
							break;
						}
					}
					if( bHasDieBefore==FALSE )
					{
						bFindMatchHole = FALSE;
						break;
					}
					// check any die after the empty, if no any die after it, then no need to check.
					for(ULONG jCol = ulCol + 1; jCol <= (ULONG)GetMapValidMaxCol(); jCol++)
					{
						ucGrade = m_WaferMapWrapper.GetGrade(ulHoleRow, jCol);
						if( ucGrade!=ucNullBin )
						{
							bHasDieAfter = TRUE;
							break;
						}
					}
					if( bHasDieAfter==FALSE )
					{
						bFindMatchHole = FALSE;
						break;
					}

					// has die on it, fail
					ucGrade = m_WaferMapWrapper.GetGrade(ulHoleRow, ulHoleCol);
					if( ucGrade!=ucNullBin )
					{
						bFindMatchHole = FALSE;
						break;
					}
				}	// end hole points checking
				if( bFindMatchHole==FALSE )
				{
					break;
				}
			}	// end hole list checking

			if( bFindMatchHole )
			{
				ULONG ulLeftGood = 0, ulRightGood = 0, ulTopGood = 0, ulBottomGood = 0;
				for(ULONG ulChkRow=ulRow; ulChkRow<ulHoleMaxRow; ulChkRow++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulChkRow, ulCol-1);
					if( ucGrade!=ucNullBin )
						ulLeftGood++;
					ucGrade = m_WaferMapWrapper.GetGrade(ulChkRow, ulHoleMaxCol);
					if( ucGrade!=ucNullBin )
						ulRightGood++;
				}
				for(ULONG ulChkCol=ulCol; ulChkCol<ulHoleMaxCol; ulChkCol++)
				{
					ucGrade = m_WaferMapWrapper.GetGrade(ulRow-1, ulChkCol);
					if( ucGrade!=ucNullBin )
						ulTopGood++;
					ucGrade = m_WaferMapWrapper.GetGrade(ulHoleMaxRow, ulChkCol);
					if( ucGrade!=ucNullBin )
						ulBottomGood++;
				}

				if( ulTopGood>=m_ulHomePatternCol/2 && ulBottomGood>=m_ulHomePatternCol/2 &&
					ulLeftGood>=m_ulHomePatternRow/2 && ulBottomGood>=m_ulHomePatternRow/2 )
				{
					ULONG ulHomeRow = ulRow + m_ulHomeKeyDieRow-1;
					ULONG ulHomeCol = ulCol + m_ulHomeKeyDieCol-1;

					if( IsMapNullBin(ulHomeRow, ulHomeCol) )
					{
						m_WaferMapWrapper.ChangeGrade(ulHomeRow, ulHomeCol, ucReferBin);
					}
					m_WaferMapWrapper.SetReferenceDie(ulHomeRow, ulHomeCol, TRUE);
					LONG lUserRow, lUserCol;
					ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lUserRow, lUserCol);
					CString szMsg;
					szMsg.Format("Found refer die at %d,%d", lUserRow, lUserCol);
					CMSLogFileUtility::Instance()->WT_PitchAlarmLog(szMsg);
				}
			}
		}
	}

	return TRUE;
}	// San An region scan to find refer die for auto alignment.

BOOL CWaferTable::FindReferDieFromMapRenesas()
{
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucReferGrade = ucNullBin;
	if( m_WaferMapWrapper.GetReader() != NULL )
	{
		ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	}
	for (ULONG ulRow = (ULONG)GetMapValidMinRow(); ulRow <= (ULONG)GetMapValidMaxRow(); ulRow++)
	{
		for(ULONG ulCol = (ULONG)GetMapValidMinCol(); ulCol <= (ULONG)GetMapValidMaxCol(); ulCol++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if( ucGrade==ucNullBin )
			{
				continue;
			}
			ucGrade = ucGrade - m_WaferMapWrapper.GetGradeOffset();
		//	if( ucGrade>=97 && ucGrade<=105 )
			if( ucGrade==98 )
			{
			//	m_WaferMapWrapper.ChangeGrade(ulHomeRow, ulHomeCol, ucReferGrade);
				m_WaferMapWrapper.SetReferenceDie(ulRow, ulCol, TRUE);
			}
		}
	}

	return TRUE;
}	// Renesas region scan to find refer die for auto alignment.

