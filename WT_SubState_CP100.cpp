#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "MS896A.h"
#include "PrescanInfo.h"
#include "WT_Log.h"
#include "LogFileUtil.h"
#include "PrescanUtility.h"
#include "WT_RegionPredication.h"
#include "Encryption.h"

#ifndef MS_DEBUG
	#include "spswitch.h"
	using namespace AsmSw;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::PeekProbeNextDie_PSC(ULONG &ulOutRow, ULONG &ulOutCol)
{
	BOOL bEndOfMap = TRUE;

	for(int i=0; i<m_szPscRegnDieListRow.GetSize(); i++)
	{
		CString szRow = m_szPscRegnDieListRow.GetAt(i);
		CString szCol = m_szPscRegnDieListCol.GetAt(i);
		LONG lPrbRow = atoi(szRow);
		LONG lPrbCol = atoi(szCol);
		ULONG ulRow = 0, ulCol = 0;
		ConvertOrgUserToAsm(lPrbRow, lPrbCol, ulRow, ulCol);
		if( ulRow==GetLastDieRow() && ulCol==GetLastDieCol() )
		{
			continue;
		}

		if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_PRB_PROBING )
		{
			ulOutRow = ulRow;
			ulOutCol = ulCol;
			bEndOfMap = FALSE;
			break;
		}
	}

	return bEndOfMap;
}

BOOL CWaferTable::PeekProbeNextDie_Block(ULONG &ulOutRow, ULONG &ulOutCol)
{
	BOOL bFindPrbDie = FALSE;
	LONG lRow = m_lLastProbeRow;	//	block probing
	LONG lCol = m_lLastProbeCol;	//	block probing
	LONG lSpanRow = m_lBlockProbeSpanRow + 1;
	LONG lSpanCol = m_lBlockProbeSpanCol + 1;
	LONG lGridRow = m_lBlockProbeRow;
	LONG lGridCol = m_lBlockProbeCol;
	UCHAR ucPinNo = 0;
	LONG lCoverRow = (lGridRow -1 ) * lSpanRow;
	LONG lCoverCol = (lGridCol - 1) * lSpanCol;

	//	block probing reset.
	m_stCurrDieTestData.m_acuPinNo.RemoveAll();
	m_stCurrDieTestData.m_aucPinCol.RemoveAll();
	m_stCurrDieTestData.m_aucPinRow.RemoveAll();
	m_stCurrDieTestData.m_aszPinData.RemoveAll();
	m_stCurrDieTestData.m_aszPinItemData.RemoveAll();
	m_stCurrDieTestData.m_aucPinGrade.RemoveAll();

	CUIntArray auiGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(auiGradeList);
	CString szMsg, szTemp, szPin;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	szMsg.Format("BlkPrb Peek to get map posn from %d,%d", lRow, lCol);
	if( pUtl->GetPrescanDebug() )
		CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
	while( 1 )
	{
		if( lRow>GetMapValidMaxRow() )
		{
			break;
		}

		if( m_bBlockProbingToRight )
		{
			while( 1 )
			{
				//if( lRow==GetLastDieRow() && lCol==GetLastDieCol() )
				//{
				//	continue;
				//}

				szMsg.Format("BlkPrb right index at UL %d,%d", lRow, lCol);
				if( pUtl->GetPrescanDebug() )
					CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
				if( lCol>GetMapValidMaxCol() )
				{
					lCol = GetMapValidMaxCol() - lCoverCol;
					m_lBlockProbeLoopRow++;
					if( m_lBlockProbeLoopRow >= lSpanRow )
					{
						lRow = lRow + lCoverRow + 1 ;
						m_lBlockProbeLoopRow = 0;
					}
					else
					{
						lRow++;
					}
					m_lBlockProbeLoopCol	= 0;
					m_bBlockProbingToRight = FALSE;
					szMsg.Format("BlkPrb right end, change to %d,%d", lRow, lCol);
					if( pUtl->GetPrescanDebug() )
						CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
					break;
				}

				szMsg = "BlkPrb right check at";
				szPin = "BlkPrb Pin and Offset";
				ucPinNo = 0;
				for(LONG lStepRow=0; lStepRow<lGridRow; lStepRow++)
				{
					for(LONG lStepCol=0; lStepCol<lGridCol; lStepCol++)
					{
						ucPinNo++;
						UCHAR ucOffsetRow = lStepRow*lSpanRow;
						UCHAR ucOffsetCol = lStepCol*lSpanCol;
						LONG lBlockRow = lRow + ucOffsetRow;
						LONG lBlockCol = lCol + ucOffsetCol;
						szTemp.Format(" %d,%d", lBlockRow, lBlockCol);
						szMsg += szTemp;
						if( m_WaferMapWrapper.IsReferenceDie(lBlockRow, lBlockCol) )
						{
							continue;
						}
						if( m_WaferMapWrapper.GetDieState(lBlockRow, lBlockCol)==WT_MAP_DIESTATE_PRB_PROBED )
						{
							continue;
						}
						UCHAR ucBlockGrade = m_WaferMapWrapper.GetGrade(lBlockRow, lBlockCol);
						if( ucBlockGrade==m_WaferMapWrapper.GetNullBin() )
						{
							continue;
						}
						for(INT nDie=0; nDie<auiGradeList.GetSize(); nDie++)
						{
							if( ucBlockGrade==auiGradeList.GetAt(nDie) )
							{
								m_stCurrDieTestData.m_acuPinNo.Add(ucPinNo);
								m_stCurrDieTestData.m_aucPinRow.Add(ucOffsetRow);
								m_stCurrDieTestData.m_aucPinCol.Add(ucOffsetCol);
								szTemp.Format(" pin %d,%d,%d", ucPinNo, ucOffsetRow, ucOffsetCol);
								szPin += szTemp;
								szTemp.Format(" is die");
								szMsg += szTemp;
								bFindPrbDie = TRUE;
								break;
							}
						}
					}
				}
				if( pUtl->GetPrescanDebug() )
					CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
				if( bFindPrbDie )
				{
					CMSLogFileUtility::Instance()->WT_GetDieLog(szPin);
					break;
				}
				m_lBlockProbeLoopCol++;
				if( m_lBlockProbeLoopCol>=lSpanCol )
				{
					lCol = lCol + lCoverCol + 1;
					m_lBlockProbeLoopCol = 0;
				}
				else
				{
					lCol++;
				}
			}
		}
		else	//	to left looping search
		{
			while( 1 )
			{
				szMsg.Format("BlkPrb left index at UL %d,%d", lRow, lCol);
				if( pUtl->GetPrescanDebug() )
					CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
				//if( lRow==GetLastDieRow() && lCol==GetLastDieCol() )
				//{
				//	continue;
				//}

				if( (lCol + lCoverCol) < GetMapValidMinCol() )
				{
					lCol = GetMapValidMinCol();
					m_lBlockProbeLoopRow++;
					if( m_lBlockProbeLoopRow>=lSpanRow )
					{
						lRow = lRow + lCoverRow + 1 ;
						m_lBlockProbeLoopRow = 0;
					}
					else
					{
						lRow++;
					}
					m_lBlockProbeLoopCol	= 0;
					m_bBlockProbingToRight = TRUE;
					szMsg.Format("BlkPrb left end, change to %d,%d", lRow, lCol);
					if( pUtl->GetPrescanDebug() )
						CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
					break;
				}

				szMsg.Format("BlkPrb left check at");
				szPin = "BlkPrb Pin and Offset";
				ucPinNo = 0;
				for(LONG lStepRow=0; lStepRow<lGridRow; lStepRow++)
				{
					for(LONG lStepCol=0; lStepCol<lGridCol; lStepCol++)
					{
						ucPinNo++;
						UCHAR ucOffsetRow = lStepRow*lSpanRow;
						UCHAR ucOffsetCol = lStepCol*lSpanCol;
						LONG lBlockRow = lRow + ucOffsetRow;
						LONG lBlockCol = lCol + ucOffsetCol;
						szTemp.Format(" %d,%d", lBlockRow, lBlockCol);
						szMsg += szTemp;
						if( m_WaferMapWrapper.IsReferenceDie(lBlockRow, lBlockCol) )
						{
							continue;
						}
						if( m_WaferMapWrapper.GetDieState(lBlockRow, lBlockCol)==WT_MAP_DIESTATE_PRB_PROBED )
						{
							continue;
						}
						UCHAR ucBlockGrade = m_WaferMapWrapper.GetGrade(lBlockRow, lBlockCol);
						if( ucBlockGrade==m_WaferMapWrapper.GetNullBin() )
						{
							continue;
						}
						for(INT nDie=0; nDie<auiGradeList.GetSize(); nDie++)
						{
							if( ucBlockGrade==auiGradeList.GetAt(nDie) )
							{
								m_stCurrDieTestData.m_acuPinNo.Add(ucPinNo);
								m_stCurrDieTestData.m_aucPinRow.Add(ucOffsetRow);
								m_stCurrDieTestData.m_aucPinCol.Add(ucOffsetCol);
								szTemp.Format(" pin %d,%d,%d", ucPinNo, ucOffsetRow, ucOffsetCol);
								szPin += szTemp;
								szTemp.Format(" is die");
								szMsg += szTemp;
								bFindPrbDie = TRUE;
								break;
							}
						}
					}
				}

				if( pUtl->GetPrescanDebug() )
					CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
				if( bFindPrbDie )
				{
					CMSLogFileUtility::Instance()->WT_GetDieLog(szPin);
					break;
				}

				m_lBlockProbeLoopCol++;
				if( m_lBlockProbeLoopCol>lSpanCol )
				{
					lCol = lCol - lCoverCol - 1;
					m_lBlockProbeLoopCol = 0;
				}
				else
				{
					lCol--;
				}
			}
		}

		if( bFindPrbDie )
		{
			break;
		}
	}

	if( bFindPrbDie )
	{
		m_lLastProbeRow = ulOutRow = lRow;	//	block probing
		m_lLastProbeCol = ulOutCol = lCol;	//	block probing
		szMsg.Format("BlkPrb Peek get map posn at %d,%d\n", lRow, lCol);
		if( pUtl->GetPrescanDebug() )
			CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
	}

	return bFindPrbDie==FALSE;
}	//	block probe

INT CWaferTable::OpGetWaferPos_CP(CONST BOOL bFastDaily)
{
	ULONG	ulX=0, ulY=0;
	unsigned char   ucGrade;
	BOOL			bEndOfWafer;
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;

	if( bFastDaily )
	{
		bEndOfWafer = PeekNextDie_CP100(ulY, ulX, ucGrade);
		eAction = WAF_CDieSelectionAlgorithm::PICK;
	}
	else
	{
		PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "OpGetPos_CP");
		if (bEndOfWafer==FALSE)
		{
			ULONG ulState = GetMapDieState(ulY, ulX);
			CString szMsg;
			ULONG ulPeekCounter = 1;

			if ( IsDieUnpickScan(ulState) )	// should not in loop. single grade probing (scan grade).
			{
				szMsg.Format("update Pick %d Die (%d,%d) is %d", ucGrade, ulY, ulX, ulState);
				CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
			}

			WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct = eAction;;

			BOOL bLoopFind = FALSE;
			if( IsDieUnpickScan(ulState) )
			{
				LONG lDieRow = GetLastDieRow();
				LONG lDieCol = GetLastDieCol();
				UpdateMap_CP(lDieRow, lDieCol, m_stLastDie.ucGrade, GetLastDieEAct(), m_stLastDie.ulStatus);
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
				bLoopFind = TRUE;
			}

			while (!bEndOfWafer && bLoopFind)
			{
				GetMapNextDie(ulY, ulX, ucGrade, eAction, bEndOfWafer);
				SetMapNextDie(ulY, ulX, ucGrade, eUpAct, ulState);

				ulPeekCounter++;
				if (ulPeekCounter > 1000)
				{
					break;
				}
				//Peek another new die after updating curr die state
				PeekMapNextDie(1, ulY, ulX, ucGrade, eAction, bEndOfWafer, "Loop");	
				eUpAct = eAction;
				ulState = GetMapDieState(ulY, ulX);

				bLoopFind = FALSE;
				if( IsDieUnpickScan(ulState) )
				{
					eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
					bLoopFind = TRUE;
				}
			}
		}
		if (eAction == WAF_CDieSelectionAlgorithm::PICK)
		{	
			// Store data to Die Info
			if (DieIsNoPick(ulY, ulX) == TRUE)
			{
				eAction = WAF_CDieSelectionAlgorithm::ALIGN;
			}
			if( m_bIsAlignWithReferDie && m_WaferMapWrapper.IsReferenceDie(ulY, ulX) )
			{
				LONG lRow, lCol, lX, lY;
				CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
				pUtl->GetAlignPosition(lRow, lCol, lX, lY);
				if ((ulX == lCol) && (ulY == lRow) )
				{
					CString szMsg;
					szMsg.Format("HomeDie (%d,%d) is ReferDie in get pos general", ulY, ulX);
					CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
					eAction = WAF_CDieSelectionAlgorithm::ALIGN;
				}
			}
		}
	}

	if ( bEndOfWafer == TRUE )
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("WAFER-END in CWT");
		CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End in CWT");
		return Err_WTWaferEnd;
	}

	NextBondRecord();
	//Reset wafer end signal
	m_bIsWaferEnded		= FALSE;
	m_bIsWaferProbed	= FALSE;

	m_stCurrentDie.lX				= (LONG)ulX;
	m_stCurrentDie.lY				= (LONG)ulY;
	m_stCurrentDie.ucGrade			= ucGrade;
	m_stCurrentDie.eAction			= eAction;

	return gnOK;
}

BOOL CWaferTable::PeekNextDie_CP100(ULONG &ulOutRow, ULONG &ulOutCol, UCHAR &ucGrade)
{
	BOOL bEndOfWafer = FALSE;
	if( GetPSCMode()!=PSC_NONE )	// peek next probe die
	{
		bEndOfWafer = PeekProbeNextDie_PSC(ulOutRow, ulOutCol);
	}
	else
	{
		bEndOfWafer = PeekProbeNextDie_Block(ulOutRow, ulOutCol);
	}

	CString szLogText;
	if( bEndOfWafer==FALSE )
	{
		ucGrade = m_WaferMapWrapper.GetGrade(ulOutRow, ulOutCol);
		LONG lUserRow = 0, lUserCol = 0;
		ConvertAsmToOrgUser(ulOutRow, ulOutCol, lUserRow, lUserCol);
		szLogText.Format("PND %d,%d(%d,%d),%d,", 
			ulOutRow, ulOutCol, lUserRow, lUserCol, ucGrade - m_WaferMapWrapper.GetGradeOffset());
		if( IsBlockProbe() )
		{
			INT nLoopLimit = min(m_stCurrDieTestData.m_aucPinRow.GetSize(), m_stCurrDieTestData.m_aucPinCol.GetSize());
			for(UCHAR i=1; i<nLoopLimit; i++)
			{
				CString szTemp;
				ULONG ulDieRow = ulOutRow + m_stCurrDieTestData.m_aucPinRow.GetAt(i);
				ULONG ulDieCol = ulOutCol + m_stCurrDieTestData.m_aucPinCol.GetAt(i);
				UCHAR ucDieGrade = m_WaferMapWrapper.GetGrade(ulDieRow, ulDieCol) - m_WaferMapWrapper.GetGradeOffset();
				ConvertAsmToOrgUser(ulDieRow, ulDieCol, lUserRow, lUserCol);
				szTemp.Format(" %d,%d(%d,%d),%d,", 
					ulDieRow, ulDieCol, lUserRow, lUserCol, ucDieGrade);
				szLogText += szTemp;
			}	//	block probe
		}
	}
	else
	{
		szLogText.Format("PPND out of wafer map");
	}
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);

	return bEndOfWafer;
}

INT CWaferTable::OpUpdateDie_CP(BOOL bFastDaily)
{
	if( IsBlockProbe() )
	{
		return 	OpUpdateDie_CP_BP();
	}	//	block probe

	UCHAR ucProbeGrade = m_stCurrDieTestData.m_ucTestGrade;
	m_stCurrDieTestData.m_ucTestGrade = 0;

	if( m_bWaferRepeatTest && m_bWaferNonStopTest )
	{
		if( ucProbeGrade==0 || ucProbeGrade>200 )
			ucProbeGrade = 111;
	}

	CByteArray		aItemGradeList;
	aItemGradeList.RemoveAll();

	CStringArray szaRawData;
	szaRawData.RemoveAll();
	ParseRawData(m_stCurrDieTestData.m_szMapItemData, szaRawData);
	for(int i=0; i<szaRawData.GetSize(); i++)
	{
		UCHAR ucGrade = 0;
		DOUBLE dValue = CP_ITEM_INVALID_VALUE;
		if ( IsItemValid(szaRawData.GetAt(i)) )
		{
			dValue = atof(szaRawData.GetAt(i));
		}

		if (i < CP_ITEM_MAX_LIMIT)		//Klocwork	//v4.46
		{
			if (dValue < m_daBinMapItemMin[i])
			{
				ucGrade = 0;
			}
			else if (dValue > m_daBinMapItemMax[i])
			{
				ucGrade = GetItemTopGrade(i);
			}
			else
			{
				if (m_daBinMapItemStep[i] != 0)
					ucGrade = (UCHAR) ((dValue-m_daBinMapItemMin[i])/m_daBinMapItemStep[i]) + 1;
			}

			aItemGradeList.Add(ucGrade);
		}
	}
	m_stCurrDieTestData.m_szMapItemData = "";

	UCHAR ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();
	BOOL bUpdate = TRUE;
	LONG lDieRow = GetLastDieRow();
	LONG lDieCol = GetLastDieCol();
	// Update the last die pos wafer map
	if( bFastDaily )
	{
		if( ucProbeGrade == 0 )
		{
			srand( (unsigned)time( NULL ) );
			ucProbeGrade = (UCHAR)(rand()%99 + 1);
		}
		m_WaferMapWrapper.SetDieState(lDieRow, lDieCol, WT_MAP_DIESTATE_PRB_PROBED);
	}
	else
	{
		UpdateMap_CP(lDieRow, lDieCol, m_stLastDie.ucGrade, GetLastDieEAct(), m_stLastDie.ulStatus);
		if (GetLastDieEAct() != WAF_CDieSelectionAlgorithm::PICK )
		{
			bUpdate = FALSE;
		}
	}

	return gnOK;
}

BOOL CWaferTable::OpGetBlockProbeCenter(CONST LONG lProbeX, CONST LONG lProbeY, LONG &lCtrMoveX, LONG &lCtrMoveY)
{
	if( IsBlockProbe()==FALSE )
	{
		return TRUE;
	}

	LONG lDiePitchXX = GetDiePitchX_X();
	LONG lDiePitchXY = GetDiePitchX_Y();
	LONG lDiePitchYY = GetDiePitchY_Y();
	LONG lDiePitchYX = GetDiePitchY_X();

	LONG lSpanRow = m_lBlockProbeSpanRow + 1;
	LONG lSpanCol = m_lBlockProbeSpanCol + 1;
	LONG lGridRow = m_lBlockProbeRow;
	LONG lGridCol = m_lBlockProbeCol;

	LONG lCoverRow = (lGridRow - 1) * lSpanRow;
	LONG lCoverCol = (lGridCol - 1) * lSpanCol;

	lCtrMoveX = lProbeX - (lCoverCol * lDiePitchXX + lCoverRow * lDiePitchYX)/2;
	lCtrMoveY = lProbeY - (lCoverRow * lDiePitchYY + lCoverCol * lDiePitchXY)/2;

	return TRUE;
}	//	block probe

BOOL CWaferTable::OpGetNextDieMovePosition_CP(LONG &lMoveToWfX, LONG &lMoveToWfY)
{	//	block probe, it is not motorized probe pin, if small die, the probe mark may be shifted.
	LONG lCurrRow = GetCurrentRow();
	LONG lCurrCol = GetCurrentCol();
	LONG lPrescanX = 0, lPrescanY = 0, lUserRow=0, lUserCol=0;
	CString szLog;

	ConvertAsmToOrgUser(lCurrRow, lCurrCol, lUserRow, lUserCol);

	BOOL bPrescanPosnOK = GetPrescanWftPosn(lCurrRow, lCurrCol, lPrescanX, lPrescanY);
	if( bPrescanPosnOK == FALSE )
	{
		//If Prescan dot not have this die pos, try to use nearby dices to calcualate die position
		bPrescanPosnOK = GetDieValidPrescanPosn(lCurrRow, lCurrCol, 4, lPrescanX, lPrescanY, FALSE);
	}
	if( !bPrescanPosnOK )
	{
		szLog.Format("    WFT:  fail at - %d,%d(%ld, %ld) to get posn)", lCurrRow, lCurrCol, lUserRow, lUserCol);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);
		SaveScanTimeEvent(szLog);
		return FALSE;
	}

	OpGetBlockProbeCenter(lPrescanX, lPrescanY, lPrescanX, lPrescanY);	//	block probe, need to improve, locate die accurately.

	GetProbePosition(lPrescanX, lPrescanY, lPrescanX, lPrescanY);

	m_lGetScnWftPosX = lMoveToWfX = lPrescanX;
	m_lGetScnWftPosY = lMoveToWfY = lPrescanY;

	m_ulGetMveMapRow = lCurrRow;
	m_ulGetMveMapCol = lCurrCol;

	szLog.Format("Get POSN: %d,%d(%ld,%ld) at %ld,%ld",
		lCurrRow, lCurrCol, lUserRow, lUserCol, lPrescanX, lPrescanY);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);

	return TRUE;
}

INT	CWaferTable::OpMoveTable_CP(CONST BOOL bFastDaily)
{
	CString szMoveMsg;
	LONG	lMoveWfX = 0, lMoveWfY = 0;

	X_Sync();
	Y_Sync();
	T_Sync();

	if (m_bJustAlign)
	{
		m_bJustAlign = FALSE;
		// get the wafer start time
		m_stWaferStartCTime = CTime::GetCurrentTime();
	}

	LONG lUserRow = 0, lUserCol = 0;
	LONG lMapRow = GetCurrentRow();
	LONG lMapCol = GetCurrentCol();
	ConvertAsmToOrgUser(lMapRow, lMapCol, lUserRow, lUserCol);
	OpGetNextDieMovePosition_CP(lMoveWfX, lMoveWfY);
	if( bFastDaily )
	{
		m_WaferMapWrapper.SetCurrentPosition(lMapRow, lMapCol);
	}

	szMoveMsg.Format("Get MOVE, %d,%d(%d,%d) to %d,%d", lMapRow, lMapCol, lUserRow, lUserCol, lMoveWfX, lMoveWfY);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);

	// Check whether the found position is within wafer limit
	if ( IsWithinWaferLimit(lMoveWfX, lMoveWfY, 1, TRUE) == FALSE )
	{
		CString szTemp;
		szTemp.Format("WT: Invalid die position; X = %d, Y = %d", lMoveWfX, lMoveWfY);
		SetErrorMessage(szTemp);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);
		szTemp.Format("    Last die info: (r%i,c%i), enc(%i,%i)", GetLastDieRow(), GetLastDieCol(),
						GetLastDieEncX(), GetLastDieEncY());
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szTemp);

		return gnNOTOK;
	}

	TakeTime((TIME_ENUM) WTB);
	LONG lDistX = labs(lMoveWfX - GetCurrX());
	LONG lDistY = labs(lMoveWfY - GetCurrY());

	LONG lExtraDelay = 0;
	INT nProfileX = NORMAL_PROF;
	if( lDistX > GetVerySlowDist() )
	{
		nProfileX = CP_VLOW_PROFILE;
		lExtraDelay = m_lLongJumpDelay*2;
	}
	else if (lDistX > GetSlowProfileDist())
	{
		nProfileX = LOW_PROF;
		lExtraDelay = m_lLongJumpDelay;
	}
	else
	{
		nProfileX = NORMAL_PROF1;
	}	
	X_Profile(nProfileX);
	X_MoveTo(lMoveWfX, SFM_NOWAIT);
	LONG lTime_X = X_ProfileTime(nProfileX, lDistX, lDistX);

	LONG lTime_Y = 0;
	BOOL bUseSCFProfileY = m_stWTAxis_Y.m_bUseSCFProfile;
	if (bUseSCFProfileY == FALSE)
	{
		INT nProfileY = NORMAL_PROF;
		if (labs(lDistY) > GetVerySlowDist())
		{
			nProfileY = CP_VLOW_PROFILE;
			lExtraDelay = m_lLongJumpDelay*2;
		}
		else if (labs(lDistY) > GetSlowProfileDist())
		{
			nProfileY = LOW_PROF;
			lExtraDelay = m_lLongJumpDelay;
		}
		else
		{
			if (m_lFastProfileTrggerY != 0 && m_lFastProfileTrggerY > labs(lDistY))
			{
				nProfileY = FAST_PROF;
			}
			else
			{
				nProfileY = NORMAL_PROF1;
			}
		}
		Y_Profile(nProfileY);
		Y_MoveTo(lMoveWfY, SFM_NOWAIT);
		lTime_Y = Y_ProfileTime(nProfileY, lDistY, lDistY);
	}
	else
	{	
		if (labs(lDistY) > GetVerySlowDist())
		{
			INT nProfileY = CP_VLOW_PROFILE;
			Y_Profile(nProfileY);
			Y_MoveTo(lMoveWfY, SFM_NOWAIT);
			lTime_Y = Y_ProfileTime(nProfileY, lDistY, lDistY);
			lExtraDelay = m_lLongJumpDelay*2;
		}
		else
		{
			GetCmdValue();
			LONG lMovePosY = lMoveWfY - GetCurrCmdY();
			Y_ScfProfileMove(lMovePosY, SFM_NOWAIT);
			lTime_Y = Y_ScfProfileTime(NORMAL_PROF, lMovePosY, lMovePosY);
		}
	}

	LONG lIndexTime = max(lTime_X, lTime_Y) + lExtraDelay;
	DOUBLE dStartMoveTime = GetTime();
	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lMoveWfX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lMoveWfY;

	if (!m_bSel_X && !m_bSel_Y)		//v3.86
	{
		Sleep(25);
	}
	else
	{
		LONG lTemp = (LONG)(GetTime()-dStartMoveTime);
		if( lTemp<lIndexTime )
		{
			Sleep(lIndexTime - lTemp);
		}
	}

	TakeData((TIME_ENUM) WTM, lIndexTime);

	m_stCurrDieTestData.m_lMapRow		= GetCurrentRow();		//	move row
	m_stCurrDieTestData.m_lMapCol		= GetCurrentCol();		//	move col
	m_stCurrDieTestData.m_ucTestGrade	= 0;
	m_stCurrDieTestData.m_szTestData	= "";
	m_stCurrDieTestData.m_szMapItemData = "";
	//	block probe reset if not enable
	if( !IsBlockProbe() )
	{
		m_stCurrDieTestData.m_acuPinNo.RemoveAll();
		m_stCurrDieTestData.m_aucPinCol.RemoveAll();
		m_stCurrDieTestData.m_aucPinRow.RemoveAll();
		m_stCurrDieTestData.m_aucPinGrade.RemoveAll();
		m_stCurrDieTestData.m_aszPinItemData.RemoveAll();
		m_stCurrDieTestData.m_aszPinData.RemoveAll();
	}

	TakeTime((TIME_ENUM) WTE);		// Take Time
#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	DisplaySequence("CWT - OpMove Table complete");

	return gnOK;
}

INT CWaferTable::OpNextDie_CP(CONST bFastDaily)
{
	m_stLastDie = m_stCurrentDie;

	GetEncoderValue();		
	LONG lCmdX = GetCurrX();
	LONG lCmdY = GetCurrY();
//	lCmdX = CMS896AStn::MotionGetCommandPosition(WT_AXIS_X, &m_stWTAxis_X);
//	lCmdY = CMS896AStn::MotionGetCommandPosition(WT_AXIS_Y, &m_stWTAxis_Y);

	m_stLastDie.lX_Enc = lCmdX;
	m_stLastDie.lY_Enc = lCmdY;
	m_stLastDie.lT_Enc = GetCurrT();

	if( bFastDaily==FALSE )
	{
		BOOL bEndOfWafer = FALSE;
		unsigned long ulDiePos_X = 0, ulDiePos_Y = 0;
		unsigned char ucGrade = '0';

		// Get the die info again - Should be the same as before. Otherwise, return error
	    WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
		if (GetMapNextDie(ulDiePos_Y, ulDiePos_X, ucGrade, eAction, bEndOfWafer) == FALSE)
		{
			return gnNOTOK;
		}
	}

	return gnOK;
}

INT CWaferTable::OpDisplayWaferEnd_CP()
{
	CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Display func");

	if( IsBurnIn() == FALSE )
	{
		SaveScanTimeEvent("    WFT: SHOW ALARM AS WAFER END");
		SetStatusMessage("Current wafer is finished");
		SetErrorMessage("Current wafer is finished");
		if ( m_bMapDummyPrescanAlign == FALSE  )
		{
			SetAlert_Red_Yellow(IDS_WT_END); 
		}
	}

	if( IsBurnInCP() )
	{
		HmiMessage("WFT moves to unload position in burn in!", "CP100", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 1000);
	}

	CMSLogFileUtility::Instance()->WT_GetDieLog("WAFER-END Handling");
	CMSLogFileUtility::Instance()->WT_GetIdxLog("Wafer End Handling");

	BOOL bWaferEnd = TRUE;
	if( m_bWaferRepeatTest )
	{
		bWaferEnd = FALSE;
		m_WaferMapWrapper.StartMap();
		m_WaferMapWrapper.RestartMap();

		UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
		UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
		UCHAR ucDefectGrade = GetScanDefectGrade() + ucOffset;
		UCHAR ucBadcutGrade = GetScanBadcutGrade() + ucOffset;
		UCHAR ucProbeGrade = ucOffset + m_ucDummyPrescanPNPGrade;

		m_WaferMapWrapper.StopMap();
		ObtainMapValidRange();
		if( IsBlockProbe() )
		{
			m_lLastProbeRow = GetMapValidMinRow();
			m_lLastProbeCol	= GetMapValidMinCol();
			m_bBlockProbingToRight	= TRUE;			//	block probing
			m_lBlockProbeLoopRow	= 0;
			m_lBlockProbeLoopCol	= 0;
		}	//	block probe
		for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
		{
			for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
			{
				UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				if( ucGrade==ucNullBin || ucGrade==ucDefectGrade || ucGrade==ucBadcutGrade )
				{
					continue;
				}
				if( m_WaferMapWrapper.GetReader()->IsDefectiveDie(ulRow, ulCol) )
				{
					continue;
				}
				ULONG ulDieState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
				if( IsDieUnpickAll(ulDieState) || (ulDieState==WT_MAP_DIESTATE_SCAN_EDGE) )
				{
					continue;
				}

				m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucProbeGrade);
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_DEFAULT);
			}
		}

		UCHAR aaTempGrades[5];
		aaTempGrades[0] = ucProbeGrade;

		m_WaferMapWrapper.SelectGrade(aaTempGrades, 1);
		m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);
		PrescanRealignWafer();
		m_WaferMapWrapper.StartMap();

		SetWTStable(FALSE);
		SetWTReady(FALSE);
		SetWTStartMove(FALSE);		//v3.66	//DBH
		(*m_psmfSRam)["WaferTable"]["LFDieNoCompensate"]	= FALSE;	// prescan finish
		m_bThisDieNotLFed = FALSE;
		m_ulPickingBlockIndex	= 0;

		m_bIsPrescanning = FALSE;
	}

	m_bIsWaferProbed = TRUE;
	m_bIsWaferEnded = bWaferEnd;
	(*m_psmfSRam)["WaferTable"]["WaferEnd"]		= bWaferEnd;

	//Output reference die status file
	SaveScanTimeEvent("    WFT: Op Display Wafer End done");

	return 0;
}

BOOL CWaferTable::IsProbingEnd()
{
	return m_bIsWaferEnded;
}

INT CWaferTable::OpUpdateDie_CP_BP()
{
	LONG lBaseRow = m_stCurrDieTestData.m_lMapRow;
	LONG lBaseCol = m_stCurrDieTestData.m_lMapCol;
	INT nLoopLimit = min(m_stCurrDieTestData.m_aucPinGrade.GetSize(), m_stCurrDieTestData.m_aszPinItemData.GetSize());
	nLoopLimit = min(nLoopLimit, m_stCurrDieTestData.m_aucPinRow.GetSize());
	nLoopLimit = min(nLoopLimit, m_stCurrDieTestData.m_aucPinCol.GetSize());
	CString szMsg, szTemp;
	szMsg = "BlkPrb Mark the Probe Die";
	CString szAddMsg = "ADD";
	if( nLoopLimit>0 )
	{
		m_WaferMapWrapper.SetDieState(lBaseRow, lBaseCol, WT_MAP_DIESTATE_PRB_PROBED);
	}
	UCHAR ucItemMapDivide = GetItemMapDisplayRatio();
	for(int j=0; j<nLoopLimit; j++)
	{
		UCHAR ucProbeGrade = m_stCurrDieTestData.m_aucPinGrade.GetAt(j);
		if( m_bWaferRepeatTest && m_bWaferNonStopTest )
		{
			if( ucProbeGrade==0 || ucProbeGrade>200 )
				ucProbeGrade = 111;
		}

		CByteArray		aItemGradeList;
		aItemGradeList.RemoveAll();

		CStringArray szaRawData;
		szaRawData.RemoveAll();
		CString szItemData = m_stCurrDieTestData.m_aszPinItemData.GetAt(j);
		ParseRawData(szItemData, szaRawData);
		for(int i=0; i<szaRawData.GetSize(); i++)
		{
			UCHAR ucGrade = 0;
			DOUBLE dValue = CP_ITEM_INVALID_VALUE;
			if ( IsItemValid(szaRawData.GetAt(i)) )
			{
				dValue = atof(szaRawData.GetAt(i));
			}

			if (i < CP_ITEM_MAX_LIMIT)		//Klocwork	//v4.46
			{
				if (dValue < m_daBinMapItemMin[i])
				{
					ucGrade = 0;
				}
				else if (dValue > m_daBinMapItemMax[i])
				{
					ucGrade = GetItemTopGrade(i);
				}
				else
				{
					if (m_daBinMapItemStep[i] != 0)
						ucGrade = (UCHAR) ((dValue-m_daBinMapItemMin[i])/m_daBinMapItemStep[i]) + 1;
				}

				aItemGradeList.Add(ucGrade);
			}
		}

		UCHAR ucGradeOffset = m_WaferMapWrapper.GetGradeOffset();
		LONG lDieRow = lBaseRow + m_stCurrDieTestData.m_aucPinRow.GetAt(j);
		LONG lDieCol = lBaseCol + m_stCurrDieTestData.m_aucPinCol.GetAt(j);
		// Update the last die pos wafer map
		if( ucProbeGrade == 0 )
		{
			srand( (unsigned)time( NULL ) );
			ucProbeGrade = (UCHAR)(rand()%99 + 1);
		}

		m_WaferMapWrapper.SetDieState(lDieRow, lDieCol, WT_MAP_DIESTATE_PRB_PROBED);

		szTemp.Format(" %d,%d", lDieRow, lDieCol);
		szMsg += szTemp;
	}

	if( nLoopLimit>0 )
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog(szAddMsg);
		CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg + "\n");
	}
	m_stCurrDieTestData.m_aucPinGrade.RemoveAll();
	m_stCurrDieTestData.m_aszPinItemData.RemoveAll();
	m_stCurrDieTestData.m_szMapItemData = "";

	return gnOK;
}	//	block probe

BOOL CWaferTable::UpdateMap_CP(const ULONG ulRow, const ULONG ulCol, 
							const UCHAR ucGrade, const WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction, 
							const ULONG ulDieState)

{
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct;
	eUpAct = eAction;
	CString szState, szLogMsg;
	switch (ulDieState)
	{
	case WT_MAP_DIESTATE_DEFAULT:
		szState = " - DEFAULT";
		break;
	case WT_MAP_DIESTATE_PICK:
		szState = " - PICK";
		break;
	case WT_MAP_DIESTATE_DEFECT:
		szState = " - DEFECT";
		break;
	case WT_MAP_DIESTATE_INK:
		szState = " - INK";
		break;
	case WT_MAP_DIESTATE_CHIP:
		szState = " - CHIP";
		break;
	case WT_MAP_DIESTATE_BADCUT:
		szState = " - BADCUT";
		break;
	case WT_MAP_DIESTATE_EMPTY:
		szState = " - EMPTY";
		break;
	case WT_MAP_DIESTATE_ROTATE:
		szState = " - ROTATE";
		break;
	case WT_MAP_DIESTATE_UNPICK:
		szState = " - UNPICK";
		break;
	default:
		szState.Format(" - %d", ulDieState);
		break;
	}

	// prevent first die is reference die and used to align using normal die record and then updated to missing
	if (m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol))
	{
		ULONG ulStartRow, ulStartCol;
		m_WaferMapWrapper.GetStartPosition(ulStartRow, ulStartCol);

		if ((ulRow == ulStartRow) && (ulCol == ulStartCol) && (eUpAct == WAF_CDieSelectionAlgorithm::MISSING))
		{
			eUpAct = WAF_CDieSelectionAlgorithm::ALIGN;
		}
	}
	
	szLogMsg.Format("PUD %d,%d,%d, act %d(%d), stt %s", ulRow, ulCol, ucGrade-m_WaferMapWrapper.GetGradeOffset(), eUpAct, eAction, szState);
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
	m_lGetNextDieRow = -1;
	m_lGetNextDieCol = -1;

	return UpdateMapDie(ulRow, ulCol, ucGrade, eUpAct, ulDieState);
}	//	update CP map probing only

