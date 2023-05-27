//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "MS896A.h"
#include "PrescanInfo.h"
#include "WT_Log.h"
#include "LogFileUtil.h"
#include "PrescanUtility.h"
#include "WT_RegionPredication.h"
//#include "Encryption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CWaferTable::ForceToRescanWafer(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	if( IsAutoRescanEnable() && GetNewPickCount()>0 )
	{
		m_bReSampleAsError = TRUE;	//	force to rescan by HMI.
		ChangeNextSampleLimit(0);
		bReturn = TRUE;
		SaveScanTimeEvent("WFT: force to rescan wafer by HMI button");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

INT CWaferTable::RescanSortingWafer()	//	rescan XXX	1	just like align wafer, do preparation
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	SaveScanTimeEvent("    WFT: rescan preparation");

	if( m_lRescanPECounter==0 )	// rescan wafer
	{
		CString szFileName = m_szPrescanLogPath + PRESCAN_MOVE_POSN;
		CString szNewFileName = m_szPrescanLogPath + "_0" + PRESCAN_MOVE_POSN;
		RenameFile(szFileName, szNewFileName);
	}	//	auto rescan sorting wafer	prepare into re-scan when get pos.

	m_stRescanStartTime		= CTime::GetCurrentTime();	// just after realignment.

	if (IsAutoRescanAndSampling() || IsAutoRescanAndDEB())
	{
		m_ulLastRscnPickCount = GetNewPickCount();
		if (m_ulLastRscnPickCount >= (LONG)m_ulRescanStartCount)
		{
			m_ulNextRescanCount	= m_ulLastRscnPickCount + m_ulRescanSampleSpan;		//	rescan
		}
		else
		{
			m_ulNextRescanCount	= m_ulLastRscnPickCount + m_ulRescanStartCount;	//	rescan
			//if( (m_ulLastRscnPickCount+m_ulRescanStartCount)<=m_ulRescanSampleSpan )
			//{
			//}
			//else
			//{
			//	if( m_ulRescanStartCount<=m_ulRescanSampleSpan )
			//	{
			//		m_ulNextRescanCount	= m_ulLastRscnPickCount + m_ulRescanStartCount;	//	rescan
			//	}
			//	else
			//	{
			//		m_ulNextRescanCount	= m_ulLastRscnPickCount + m_ulRescanSampleSpan;	//	rescan
			//	}
			//}
		}
		CString szMsg;
		szMsg.Format("WFT: DEB rescan next limit is %d", GetNextRscnLimit());
		DisplaySequence(szMsg);
	}
	RescanAdvOffsetNextLimit();

	ResetBuffer();

	X_Sync();
	Y_Sync();
	T_Sync();
	SetEjectorVacuum(FALSE);

	SaveScanTimeEvent("    WFT: rescan wait bh at prepick");
	INT nPrePickCount = 0;
	while (IsBHAtPrePick() != 1)
	{
		Sleep(10);
		nPrePickCount++;
		if (nPrePickCount >= 100)
		{
			break;
		}
	}

	(*m_psmfSRam)["WaferTable"]["WTReady"]			= 0;
	(*m_psmfSRam)["BinLoader"]["MoveWftToSafeState"] = 0;
	SetDieReady(FALSE);
	SetWTStable(FALSE);
	m_lRescanRunStage	= 1;	//	MS rescan prepare done
	SaveScanTimeEvent("    WFT: rescan move bh to prepick");
	WftMoveBondArmToSafe(TRUE);		// rescan move away.
	BH_EnableThermalControl(TRUE);	//	rescan begin

	nPrePickCount = 0;
	while (IsBHAtPrePick() != 1)
	{
		Sleep(10);
		nPrePickCount++;
		if (nPrePickCount >= 100)
			break;
	}

	T_MoveTo(GetGlobalT(), SFM_WAIT);
	GetEncoderValue();
	m_lRescanLastDiePosnX		= GetCurrX();
	m_lRescanLastDiePosnY		= GetCurrY();

	ULONG lAlignRow	= m_lRescanStartRow;
	ULONG lAlignCol	= m_lRescanStartCol;
	LONG lStepCol = GetMapIndexStepCol();
	LONG lStepRow = GetMapIndexStepRow();
	LONG lMapIndexLoop = max(lStepCol, lStepRow)*2;
	LONG lAlignX	= m_lRescanStartWftX;
	LONG lAlignY	= m_lRescanStartWftY;
	if( PredictGetDieValidPosn(lAlignRow, lAlignCol, 0, lAlignX, lAlignY, TRUE) )
	{
		m_lRescanStartWftX			= lAlignX;
		m_lRescanStartWftY			= lAlignY;
	}
	else	//	rescan XX
	{
		if( GetValidMapDieInBlock(-1, m_lScannedMapMinRow, m_lScannedMapMinCol, m_lScannedMapMaxRow, m_lScannedMapMaxCol, lAlignRow, lAlignCol) )
		{
			m_lRescanStartRow = lAlignRow;
			m_lRescanStartCol = lAlignCol;
			if( PredictGetDieValidPosn(lAlignRow, lAlignCol, 0, lAlignX, lAlignY, TRUE) )
			{
				m_lRescanStartWftX			= lAlignX;	//
				m_lRescanStartWftY			= lAlignY;	//
			}
		}
	}	//	rescan XX
	pUtl->SetAlignPosition(lAlignRow, lAlignCol, lAlignX, lAlignY);	//	rescan XX

	CString szText;
	szText.Format("WFT - start point: %5ld,%5ld,%8ld,%8ld", lAlignRow, lAlignCol, lAlignX, lAlignY);
	pUtl->PrescanMoveLog(szText, TRUE);


	SaveScanTimeEvent("    WFT: rescan move to start point");
	// Move the table for die compensation
	m_WaferMapWrapper.SetCurrentPosition(lAlignRow, lAlignCol);
	XY_SafeMoveTo(lAlignX, lAlignY);
	Sleep(500);
#ifdef NU_MOTION
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
#endif

	ClearWSPrescanInfo();	//	rescan XX	clear it.
	SaveScanTimeEvent("    WFT: rescan clear prescan raw data");
	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "ClearPrescanData", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	//	rescan XX	ResetMapPhyPosn();

	m_lStart_X		= lAlignX;
	m_lStart_Y		= lAlignY;
	m_lStart_T		= GetGlobalT();
	m_ulAlignRow	= lAlignRow;
	m_ulAlignCol	= lAlignCol;

	m_dScanTime = 0.0;
	m_dCurrTime = 0.0;
	m_dStopTime = GetTime();

	m_lLastPickRow			= lAlignRow;
	m_lLastPickCol			= lAlignCol;

	// sort mode, 2nd part align wafer complete
	m_ulAlignHomeDieRow	= lAlignRow;
	m_ulAlignHomeDieCol	= lAlignCol;
	m_nAlignHomeDieWftX	= lAlignX;
	m_nAlignHomeDieWftY	= lAlignY;

	m_nPrescanAlignPosnX  = lAlignX;
	m_nPrescanAlignPosnY  = lAlignY;

	m_nPrescanNextWftPosnX	= m_nPrescanLastWftPosnX = lAlignX;	// update wafer alignment
	m_nPrescanNextWftPosnY	= m_nPrescanLastWftPosnY = lAlignY;

	m_nPrescanAlignMapRow = m_nPrescanNextMapRow	= m_nPrescanLastMapRow	 = lAlignRow;
	m_nPrescanAlignMapCol = m_nPrescanNextMapCol	= m_nPrescanLastMapCol	 = lAlignCol;

	m_nPrescanDirection		= 1;
	m_nPrescanAlignScanStage = 1;
	m_bIsPrescanning		= TRUE;
	m_bIsPrescanned			= FALSE;
	m_lPrescanHomeDieRow	= -1;
	m_lPrescanHomeDieCol	= -1;

	DefineAreaAndWalkPath_Rescan();

	m_lLastScanGrabPosnX = lAlignX;
	m_lLastScanGrabPosnY = lAlignY;
	XY_MoveToPrescanPosn();

	m_lPrescanLastGrabRow	= m_nPrescanNextMapRow	= m_nPrescanLastMapRow;	// index to prescan
	m_lPrescanLastGrabCol	= m_nPrescanNextMapCol	= m_nPrescanLastMapCol;	// index to prescan
	m_lNextFrameRow			= GetLastFrameRow();
	m_lNextFrameCol			= GetLastFrameCol();

	if (IsPrescanMapIndex())	// update map display when move table
	{
		m_bPrescanLastGrabbed	= TRUE;
		pUtl->SetPrescanMapIndexIdle(TRUE);
	}

	(*m_psmfSRam)["MS896A"]["WaferMapRow"]			= lAlignRow;
	(*m_psmfSRam)["MS896A"]["WaferMapCol"]			= lAlignCol;
	(*m_psmfSRam)["MS896A"]["WaferTableX"]			= lAlignX;
	(*m_psmfSRam)["MS896A"]["WaferTableY"]			= lAlignY;
	(*m_psmfSRam)["WaferTable"]["Current"]["Theta"]	= GetGlobalT();
	(*m_psmfSRam)["WaferTable"]["Current"]["X"]		= lAlignX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"]		= lAlignY;

	m_bPrescanningMode		= TRUE;
	m_bWaferAlignComplete	= FALSE;

	return TRUE;
}	// rescan if AOI defect over limit


BOOL CWaferTable::DefineAreaAndWalkPath_Rescan()	//	rescan XX	1.1
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMsg;
	UCHAR	ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR	ucOffset = m_WaferMapWrapper.GetGradeOffset();
	UCHAR	ucRefGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();
	CUIntArray aulSortGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSortGradeList);

//	get base point list for rescan.
	pUtl->DelRescanBasePoints();	//	rescan begin
#define	RESCAN_MAP_ISLAND_SPAN		1
	LONG lHomeRow = 0, lHomeCol = 0, lHomeWfX = 0, lHomeWfY = 0;
	pUtl->GetAlignPosition(lHomeRow, lHomeCol, lHomeWfX, lHomeWfY);	//	rescan XX
	LONG lEdgeMinRow = m_lScannedMapMinRow;
	LONG lEdgeMaxRow = m_lScannedMapMaxRow;
	LONG lEdgeMinCol = m_lScannedMapMinCol;
	LONG lEdgeMaxCol = m_lScannedMapMaxCol;
	pUtl->AddRescanBasePoint(lHomeRow, lHomeCol, lHomeWfX, lHomeWfY);
	LONG lBaseRow = 0, lBaseCol = 0, lBaseWfX = 0, lBaseWfY = 0;
	LONG lEncX = 0, lEncY = 0;
	//if( IsAutoRescanAndDEB() )
	//{
	//	for(UINT i=0; i<m_unNumOfKeyDie; i++)
	//	{
	//		lBaseRow = m_paDebKeyDie[i].uiRow;
	//		lBaseCol = m_paDebKeyDie[i].uiCol;
	//		if( DEB_GetDiePosnOnly(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY) )
	//		{
	//			pUtl->AddRescanBasePoint(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
	//		}
	//	}
	//}

	for(lBaseRow=lHomeRow; lBaseRow<=lEdgeMaxRow; lBaseRow++)	//	Low Part rescan XX	3.0
	{
		for(lBaseCol=lHomeCol; lBaseCol<=lEdgeMaxCol; lBaseCol++)	// Low Right
		{
			if( lBaseRow==lHomeRow && lBaseCol==lHomeCol )
			{
				continue;
			}
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lBaseRow, lBaseCol);
			if( ucGrade==ucNullBin )
			{
				continue;
			}
			if( m_WaferMapWrapper.IsReferenceDie(lBaseRow, lBaseCol) && ucGrade==ucRefGrade )
			{
				continue;
			}
			if( GetMapPhyPosn(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY)==FALSE )
			{
				continue;
			}
			if( IsScanMapNgGrade(ucGrade-ucOffset) )
			{
				continue;
			}
			if( m_bAutoRescanAllDice==FALSE )	// check in list, not => skip this base point
			{
				BOOL bToSortDie = FALSE;
				for (int i=0; i < aulSortGradeList.GetSize(); i++)
				{
					if ((aulSortGradeList.GetAt(i)) == ucGrade)
					{
						bToSortDie = TRUE;
						break;
					}
				}
				if( bToSortDie==FALSE )
				{
					continue;
				}
			}

			if( (lBaseCol-lHomeCol)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow, lBaseCol-1, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol-1))
				{
					continue;
				}
			}
			if( (lBaseCol-lHomeCol)>=RESCAN_MAP_ISLAND_SPAN )
			{
				if (GetMapPhyPosn(lBaseRow, lBaseCol-RESCAN_MAP_ISLAND_SPAN, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol-RESCAN_MAP_ISLAND_SPAN))
				{
					continue;
				}
			}

			if( (lBaseRow-lHomeRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow-1, lBaseCol, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow-1, lBaseCol))
				{
					continue;
				}
			}
			if( (lBaseRow-lHomeRow)>=RESCAN_MAP_ISLAND_SPAN )
			{
				if (GetMapPhyPosn(lBaseRow-RESCAN_MAP_ISLAND_SPAN, lBaseCol, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow-RESCAN_MAP_ISLAND_SPAN, lBaseCol))
				{
					continue;
				}
			}

			if( (lBaseRow-lHomeRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow-1, lBaseCol+1, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow-1, lBaseCol+1))
				{
					continue;
				}
				if( (lBaseCol-lHomeCol)>=1 )
				{
					if (GetMapPhyPosn(lBaseRow-1, lBaseCol-1, lEncX, lEncY) &&
						m_pWaferMapManager->IsMapHaveBin(lBaseRow-1, lBaseCol-1))
					{
						continue;
					}
				}
			}

			if( IsAutoRescanAndDEB() )
			{
				DEB_GetDiePosnOnly(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
			}
			pUtl->AddRescanBasePoint(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
		}

		for(lBaseCol=lHomeCol; lBaseCol>=lEdgeMinCol; lBaseCol--)	// Low Left
		{
			if( lBaseRow==lHomeRow && lBaseCol==lHomeCol )
			{
				continue;
			}
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lBaseRow, lBaseCol);
			if( ucGrade==ucNullBin )
			{
				continue;
			}
			if( m_WaferMapWrapper.IsReferenceDie(lBaseRow, lBaseCol) && ucGrade==ucRefGrade )
			{
				continue;
			}
			if( GetMapPhyPosn(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY)==FALSE )
			{
				continue;
			}
			if( IsScanMapNgGrade(ucGrade-ucOffset) )
			{
				continue;
			}
			if( m_bAutoRescanAllDice==FALSE )	// check in list, not => skip this base point
			{
				BOOL bToSortDie = FALSE;
				for (int i=0; i < aulSortGradeList.GetSize(); i++)
				{
					if ((aulSortGradeList.GetAt(i)) == ucGrade)
					{
						bToSortDie = TRUE;
						break;
					}
				}
				if( bToSortDie==FALSE )
				{
					continue;
				}
			}

			if (GetMapPhyPosn(lBaseRow, lBaseCol+1, lEncX, lEncY) &&
				m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol+1))
			{
				continue;
			}
			if (GetMapPhyPosn(lBaseRow, lBaseCol+RESCAN_MAP_ISLAND_SPAN, lEncX, lEncY) &&
				m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol+RESCAN_MAP_ISLAND_SPAN))
			{
				continue;
			}

			if( (lBaseRow-lHomeRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow-1, lBaseCol, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow-1, lBaseCol))
				{
					continue;
				}
			}
			if( (lBaseRow-lHomeRow)>=RESCAN_MAP_ISLAND_SPAN )
			{
				if (GetMapPhyPosn(lBaseRow-RESCAN_MAP_ISLAND_SPAN, lBaseCol, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow-RESCAN_MAP_ISLAND_SPAN, lBaseCol))
				{
					continue;
				}
			}

			if( (lBaseRow-lHomeRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow-1, lBaseCol+1, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow-1, lBaseCol+1))
				{
					continue;
				}
				if( (lBaseCol-lEdgeMinCol)>=1 )
				{
					if (GetMapPhyPosn(lBaseRow-1, lBaseCol-1, lEncX, lEncY) &&
						m_pWaferMapManager->IsMapHaveBin(lBaseRow-1, lBaseCol-1))
					{
						continue;
					}
				}
			}

			if( IsAutoRescanAndDEB() )
			{
				DEB_GetDiePosnOnly(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
			}
			pUtl->AddRescanBasePoint(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
		}
	}

	for(lBaseRow=lHomeRow; lBaseRow>=lEdgeMinRow; lBaseRow--)	//	rescan XX Up Part
	{
		for(lBaseCol=lHomeCol; lBaseCol<=lEdgeMaxCol; lBaseCol++)	// Up Right
		{
			if( lBaseRow==lHomeRow && lBaseCol==lHomeCol )
			{
				continue;
			}
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lBaseRow, lBaseCol);
			if( ucGrade==ucNullBin )
			{
				continue;
			}
			if( m_WaferMapWrapper.IsReferenceDie(lBaseRow, lBaseCol) && ucGrade==ucRefGrade )
			{
				continue;
			}
			if( GetMapPhyPosn(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY)==FALSE )
			{
				continue;
			}
			if( IsScanMapNgGrade(ucGrade-ucOffset) )
			{
				continue;
			}
			if( m_bAutoRescanAllDice==FALSE )	// check in list, not => skip this base point
			{
				BOOL bToSortDie = FALSE;
				for (int i=0; i < aulSortGradeList.GetSize(); i++)
				{
					if ((aulSortGradeList.GetAt(i)) == ucGrade)
					{
						bToSortDie = TRUE;
						break;
					}
				}
				if( bToSortDie==FALSE )
				{
					continue;
				}
			}

			if( (lBaseCol-lHomeCol)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow, lBaseCol-1, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol-1))
				{
					continue;
				}
			}
			if( (lBaseCol-lHomeCol)>=RESCAN_MAP_ISLAND_SPAN )
			{
				if (GetMapPhyPosn(lBaseRow, lBaseCol-RESCAN_MAP_ISLAND_SPAN, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol-RESCAN_MAP_ISLAND_SPAN))
				{
					continue;
				}
			}

			if( (lHomeRow-lBaseRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow+1, lBaseCol, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow+1, lBaseCol))
				{
					continue;
				}
			}
			if (GetMapPhyPosn(lBaseRow+RESCAN_MAP_ISLAND_SPAN, lBaseCol, lEncX, lEncY) &&
				m_pWaferMapManager->IsMapHaveBin(lBaseRow+RESCAN_MAP_ISLAND_SPAN, lBaseCol))
			{
				continue;
			}

			if( (lHomeRow-lBaseRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow+1, lBaseCol+1, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow+1, lBaseCol+1))
				{
					continue;
				}
				if( (lBaseCol-lHomeCol)>=1 )
				{
					if (GetMapPhyPosn(lBaseRow+1, lBaseCol-1, lEncX, lEncY) &&
						m_pWaferMapManager->IsMapHaveBin(lBaseRow+1, lBaseCol-1))
					{
						continue;
					}
				}
			}

			if( IsAutoRescanAndDEB() )
			{
				DEB_GetDiePosnOnly(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
			}
			pUtl->AddRescanBasePoint(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
		}

		for(lBaseCol=lHomeCol; lBaseCol>=lEdgeMinCol; lBaseCol--)	//	Up Left
		{
			if( lBaseRow==lHomeRow && lBaseCol==lHomeCol )
			{
				continue;
			}
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lBaseRow, lBaseCol);
			if( ucGrade==ucNullBin )
			{
				continue;
			}
			if( m_WaferMapWrapper.IsReferenceDie(lBaseRow, lBaseCol) && ucGrade==ucRefGrade )
			{
				continue;
			}
			if( GetMapPhyPosn(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY)==FALSE )
			{
				continue;
			}
			if( IsScanMapNgGrade(ucGrade-ucOffset) )
			{
				continue;
			}
			if( m_bAutoRescanAllDice==FALSE )	// check in list, not => skip this base point
			{
				BOOL bToSortDie = FALSE;
				for (int i=0; i < aulSortGradeList.GetSize(); i++)
				{
					if ((aulSortGradeList.GetAt(i)) == ucGrade)
					{
						bToSortDie = TRUE;
						break;
					}
				}
				if( bToSortDie==FALSE )
				{
					continue;
				}
			}

			if (GetMapPhyPosn(lBaseRow, lBaseCol+1, lEncX, lEncY) &&
				m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol+1))
			{
				continue;
			}
			if (GetMapPhyPosn(lBaseRow, lBaseCol+RESCAN_MAP_ISLAND_SPAN, lEncX, lEncY) &&
				m_pWaferMapManager->IsMapHaveBin(lBaseRow, lBaseCol+RESCAN_MAP_ISLAND_SPAN))
			{
				continue;
			}

			if( (lHomeRow-lBaseRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow+1, lBaseCol, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow+1, lBaseCol))
				{
					continue;
				}
			}
			if (GetMapPhyPosn(lBaseRow+RESCAN_MAP_ISLAND_SPAN, lBaseCol, lEncX, lEncY) &&
				m_pWaferMapManager->IsMapHaveBin(lBaseRow+RESCAN_MAP_ISLAND_SPAN, lBaseCol))
			{
				continue;
			}

			if( (lHomeRow-lBaseRow)>=1 )
			{
				if (GetMapPhyPosn(lBaseRow+1, lBaseCol+1, lEncX, lEncY) &&
					m_pWaferMapManager->IsMapHaveBin(lBaseRow+1, lBaseCol+1))
				{
					continue;
				}
				if( (lBaseCol-lEdgeMinCol)>=1 )
				{
					if (GetMapPhyPosn(lBaseRow+1, lBaseCol-1, lEncX, lEncY) &&
						m_pWaferMapManager->IsMapHaveBin(lBaseRow+1, lBaseCol-1))
					{
						continue;
					}
				}
			}

			if( IsAutoRescanAndDEB() )
			{
				DEB_GetDiePosnOnly(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
			}
			pUtl->AddRescanBasePoint(lBaseRow, lBaseCol, lBaseWfX, lBaseWfY);
		}
	}

//	get base point list for rescan.

	DelFrameRescanBasePoints();		DelRescanFrameBaseNum();	//	rescan begin to store frame's map row and column.
	LONG lMapLoopRow	= GetMapIndexStepRow()/2;
	LONG lMapLoopCol	= GetMapIndexStepCol()/2;
	INT lAlnWfX = GetPrescanAlignPosnX();
	INT lAlnWfY = GetPrescanAlignPosnY();

	LONG lUpLX = GetWaferCenterX() + GetWaferDiameter();
	LONG lUpLY = GetWaferCenterY() + GetWaferDiameter();
	LONG	laScanMapRow[SCAN_MAX_MATRIX_ROW+1][SCAN_MAX_MATRIX_COL+1];
	LONG	laScanMapCol[SCAN_MAX_MATRIX_ROW+1][SCAN_MAX_MATRIX_COL+1];

	for(INT nRow=SCAN_MATRIX_MIN_ROW; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
	{
		for(INT nCol=SCAN_MATRIX_MIN_COL; nCol<SCAN_MAX_MATRIX_COL; nCol++)
		{
			m_baScanMatrix[nRow][nCol] = FALSE;
			laScanMapRow[nRow][nCol] = -1;
			laScanMapCol[nRow][nCol] = -1;
		}
	}

	m_lAlignFrameCol = (lUpLX-lAlnWfX)/GetPrescanPitchX();
	m_lAlignFrameRow = (lUpLY-lAlnWfY)/GetPrescanPitchY();

	if( m_lAlignFrameRow>=(SCAN_MAX_MATRIX_ROW-1) )
		m_lAlignFrameRow = SCAN_MAX_MATRIX_ROW-1;
	if( m_lAlignFrameRow<(SCAN_MATRIX_MIN_ROW+1) )
		m_lAlignFrameRow = SCAN_MATRIX_MIN_ROW+1;
	if( m_lAlignFrameCol>=(SCAN_MAX_MATRIX_COL-1) )
		m_lAlignFrameCol = SCAN_MAX_MATRIX_COL-1;
	if( m_lAlignFrameCol<(SCAN_MATRIX_MIN_COL+1) )
		m_lAlignFrameCol = (SCAN_MATRIX_MIN_COL+1);

	m_lScanFrameMaxRow	= -1;
	m_lScanFrameMaxCol	= -1;
	m_lScanFrameMinRow	= 9999;
	m_lScanFrameMinCol	= 9999;

	m_ucRunScanWalkTour = m_ucScanWalkTour;	//	rescan XX

	LONG lFrameRow = GetAlignFrameRow();
	LONG lFrameCol = GetAlignFrameCol();
	LONG lWaferRow = m_nPrescanAlignMapRow;
	LONG lWaferCol = m_nPrescanAlignMapCol;

	szMsg.Format("align frame %ld,%ld (UL %ld,%ld), map %ld,%ld at %ld,%ld",
		lFrameRow, lFrameCol, lUpLX, lUpLY, lWaferRow, lWaferCol, lAlnWfX, lAlnWfY);
	pUtl->PrescanMoveLog(szMsg, TRUE);
	BOOL bHoriIndex = TRUE;
	SHORT sJumpCol = 1;
	SHORT sJumpRow = 0;
	if( m_ucRunScanWalkTour==WT_SCAN_WALK_LEFT_VERT || m_ucRunScanWalkTour==WT_SCAN_WALK_HOME_VERT )
	{
		sJumpCol = 0;
		sJumpRow = 1;
		bHoriIndex = FALSE;
	}
	LONG lStepRow = GetMapIndexStepRow();
	LONG lStepCol = GetMapIndexStepCol();
	LONG lHalfRow = lStepRow/2;
	LONG lHalfCol = lStepCol/2;
	LONG lScanMinRow = m_lScannedMapMinRow - lHalfRow;
	LONG lScanMaxRow = m_lScannedMapMaxRow + lHalfRow;
	LONG lScanMinCol = m_lScannedMapMinCol - lHalfCol;
	LONG lScanMaxCol = m_lScannedMapMaxCol + lHalfCol;
	LONG lMapIndexLoop = max(lStepCol, lStepRow)*2;
	INT nScanDir = 1;
	BOOL bFirstFrame = TRUE;

	BOOL bHasDie = TRUE;
	while( 1 )
	{
		while( 1 )
		{
			if( bFirstFrame==FALSE )
			{
				lFrameCol += nScanDir*sJumpCol;
				lFrameRow += nScanDir*sJumpRow;
				lWaferRow += lStepRow*nScanDir*sJumpRow;
				lWaferCol += lStepCol*nScanDir*sJumpCol;
			}
			bFirstFrame = FALSE;
			if( bHoriIndex )
			{
				if( (lWaferCol>lScanMaxCol || lFrameCol>=SCAN_MAX_MATRIX_COL) )
				{
					nScanDir = -1;
					lWaferRow += lStepRow;
					lFrameRow++;
					continue;
				}

				if( (lWaferCol<lScanMinCol || lFrameCol<SCAN_MATRIX_MIN_COL) )
				{
					nScanDir = 1;
					lWaferRow += lStepRow;
					lFrameRow++;
					continue;
				}

				if( lWaferRow>lScanMaxRow || lWaferRow<lScanMinRow || lFrameRow>=SCAN_MAX_MATRIX_ROW )
				{
					bHasDie = FALSE;
					szMsg.Format("Wafer row over limit or frame row over limit");
					pUtl->PrescanMoveLog(szMsg, TRUE);
					break;
				}
			}
			else
			{
				if( (lWaferRow>lScanMaxRow || lFrameRow>=SCAN_MAX_MATRIX_ROW) )
				{
					nScanDir = -1;
					lWaferCol += lStepCol;
					lFrameCol++;
					continue;
				}

				if( (lWaferRow<lScanMinRow || lFrameRow<SCAN_MATRIX_MIN_ROW) )
				{
					nScanDir = 1;
					lWaferCol += lStepCol;
					lFrameCol++;
					continue;
				}

				if( lWaferCol>lScanMaxCol || lWaferCol<lScanMinCol || lFrameCol>=SCAN_MAX_MATRIX_COL)
				{
					bHasDie = FALSE;
					szMsg.Format("Wafer Col over limit or frame col over limit");
					pUtl->PrescanMoveLog(szMsg, TRUE);
					break;
				}
			}

			LONG lFrameMapRow = lWaferRow;
			LONG lFrameMapCol = lWaferCol;
			if( lFrameMapRow>m_lScannedMapMaxRow )
				lFrameMapRow = m_lScannedMapMaxRow;
			if( lFrameMapCol>m_lScannedMapMaxCol )
				lFrameMapCol = m_lScannedMapMaxCol;
			if( lFrameMapRow<m_lScannedMapMinRow )
				lFrameMapRow = m_lScannedMapMinRow;
			if( lFrameMapCol<m_lScannedMapMinCol )
				lFrameMapCol = m_lScannedMapMinCol;

			BOOL bMapHasSortDie = FALSE;
			for(LONG jRow=(0-lHalfRow); jRow<=(0+lHalfRow); jRow++)
			{
				for(LONG iCol=(0-lHalfCol); iCol<=(0+lHalfCol); iCol++)
				{
					LONG lChkRow = jRow+lFrameMapRow;
					LONG lChkCol = iCol+lFrameMapCol;
					if( lChkRow<m_lScannedMapMinRow || lChkRow>m_lScannedMapMaxRow ||
						lChkCol<m_lScannedMapMinCol || lChkCol>m_lScannedMapMaxCol )
					{
						continue;
					}
					UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lChkRow, lChkCol);
					if( ucGrade==ucNullBin )
					{
						continue;
					}
				//	if( m_WaferMapWrapper.IsReferenceDie(lChkRow, lChkCol) && ucGrade==ucRefGrade )
				//	{
				//		continue;
				//	}
				//	if( IsScanMapNgGrade(ucGrade-ucOffset) )
				//	{
				//		continue;
				//	}
				//	LONG lChkEncX = 0, lChkEncY = 0;
				//	if( GetMapPhyPosn(lChkRow, lChkCol, lChkEncX, lChkEncY)==FALSE )
				//	{
				//		continue;
				//	}
					if( m_bAutoRescanAllDice )	//	rescan all available die
					{
						bMapHasSortDie = TRUE;
					}
					else
					{
						for (int i=0; i < aulSortGradeList.GetSize(); i++)
						{
							if ((aulSortGradeList.GetAt(i)) == ucGrade)
							{
								bMapHasSortDie = TRUE;
								break;
							}
						}
					}
					if( bMapHasSortDie )
					{
						break;
					}
				}
				if( bMapHasSortDie )
				{
					break;
				}
			}

			if( bMapHasSortDie==FALSE )
			{
				continue;
			}

			LONG nPosnX = 0, nPosnY = 0;
			BOOL bWaferHasPosn = PredictGetDieValidPosn(lFrameMapRow, lFrameMapCol, lMapIndexLoop, nPosnX, nPosnY, TRUE);
			if( bWaferHasPosn==FALSE )
			{
				continue;
			}

			if( IsWithinWaferLimit(nPosnX, nPosnY)==FALSE )
			{
				LONG lBlkMinRow = lFrameMapRow;
				LONG lBlkMinCol = lFrameMapCol;
				LONG lBlkMaxRow = lFrameMapRow;
				LONG lBlkMaxCol = lFrameMapCol;
				for(LONG jRow=(0-lHalfRow); jRow<=(0+lHalfRow); jRow++)
				{
					for(LONG iCol=(0-lHalfCol); iCol<=(0+lHalfCol); iCol++)
					{
						LONG lChkRow = jRow+lFrameMapRow;
						LONG lChkCol = iCol+lFrameMapCol;
						if( lChkRow<m_lScannedMapMinRow || lChkRow>m_lScannedMapMaxRow ||
							lChkCol<m_lScannedMapMinCol || lChkCol>m_lScannedMapMaxCol )
						{
							continue;
						}
						LONG lChkEncX = 0, lChkEncY = 0;
						if( GetMapPhyPosn(lChkRow, lChkCol, lChkEncX, lChkEncY) )
						{
							lBlkMinRow = min(lBlkMinRow, lChkRow);
							lBlkMaxRow = max(lBlkMaxRow, lChkRow);
							lBlkMinCol = min(lBlkMinCol, lChkCol);
							lBlkMaxCol = max(lBlkMaxCol, lChkCol);
						}
					}
				}
				lFrameMapRow = (lBlkMinRow + lBlkMaxRow)/2;
				lFrameMapCol = (lBlkMinCol + lBlkMaxCol)/2;

				BOOL bWaferHasPosn = PredictGetDieValidPosn(lFrameMapRow, lFrameMapCol, lMapIndexLoop, nPosnX, nPosnY, TRUE);
				if( bWaferHasPosn==FALSE )
			{
				continue;
			}
			}

			//if( nPosnX>m_lScannedWaferUL_X || nPosnX<m_lScannedWaferLR_X ||
			//	nPosnY>m_lScannedWaferUL_Y || nPosnY<m_lScannedWaferLR_Y )
			//{
			//	continue;
			//}	//	rescan XX

			if( m_lScanFrameMinRow>lFrameRow )
				m_lScanFrameMinRow = lFrameRow;
			if( m_lScanFrameMinCol>lFrameCol )
				m_lScanFrameMinCol = lFrameCol;
			if( m_lScanFrameMaxRow<lFrameRow )
				m_lScanFrameMaxRow = lFrameRow;
			if( m_lScanFrameMaxCol<lFrameCol )
				m_lScanFrameMaxCol = lFrameCol;
			m_baScanMatrix[lFrameRow][lFrameCol]	= TRUE;
			m_laScanPosnX[lFrameRow][lFrameCol]		= nPosnX;
			m_laScanPosnY[lFrameRow][lFrameCol]		= nPosnY;
			laScanMapRow[lFrameRow][lFrameCol] = lFrameMapRow;
			laScanMapCol[lFrameRow][lFrameCol] = lFrameMapCol;
		}

		if( bHasDie==FALSE )
		{
			break;
		}
	}

	lFrameRow = GetAlignFrameRow();
	lFrameCol = GetAlignFrameCol();
	lWaferRow = m_nPrescanAlignMapRow;
	lWaferCol = m_nPrescanAlignMapCol;
	nScanDir = -1;
	bHasDie = TRUE;

	while( 1 )
	{
		while( 1 )
		{
			lFrameRow += sJumpRow*nScanDir;
			lFrameCol += sJumpCol*nScanDir;
			lWaferRow += lStepRow*nScanDir*sJumpRow;
			lWaferCol += lStepCol*nScanDir*sJumpCol;
			if( bHoriIndex )
			{
				if( (lWaferCol>lScanMaxCol || lFrameCol>=SCAN_MAX_MATRIX_COL) )
				{
					nScanDir = -1;
					lWaferRow -= lStepRow;
					lFrameRow--;
					continue;
				}

				if( (lWaferCol<lScanMinCol || lFrameCol<SCAN_MATRIX_MIN_COL) )
				{
					nScanDir = 1;
					lWaferRow -= lStepRow;
					lFrameRow--;
					continue;
				}

				if( lWaferRow>lScanMaxRow || lWaferRow<lScanMinRow || lFrameRow<SCAN_MATRIX_MIN_ROW )
				{
					bHasDie = FALSE;
					szMsg.Format("Wafer Row 2 over limit or frame row 2 over limit");
					pUtl->PrescanMoveLog(szMsg, TRUE);
					break;
				}
			}
			else
			{
				if( (lWaferRow>lScanMaxRow || lFrameRow>=SCAN_MAX_MATRIX_ROW) )
				{
					nScanDir = -1;
					lWaferCol -= lStepCol;
					lFrameCol--;
					continue;
				}

				if( (lWaferRow<lScanMinRow || lFrameRow<SCAN_MATRIX_MIN_ROW) )
				{
					nScanDir = 1;
					lWaferCol -= lStepCol;
					lFrameCol--;
					continue;
				}

				if( lWaferCol>lScanMaxCol || lWaferCol<lScanMinCol || lFrameCol<SCAN_MATRIX_MIN_COL )
				{
					bHasDie = FALSE;
					szMsg.Format("Wafer Col 2 over limit or frame col 2 over limit");
					pUtl->PrescanMoveLog(szMsg, TRUE);
					break;
				}
			}

			LONG lFrameMapRow = lWaferRow;
			LONG lFrameMapCol = lWaferCol;

			if(lFrameMapRow>m_lScannedMapMaxRow )
				lFrameMapRow = m_lScannedMapMaxRow;
			if( lFrameMapCol>m_lScannedMapMaxCol )
				lFrameMapCol = m_lScannedMapMaxCol;
			if( lFrameMapRow<m_lScannedMapMinRow )
				lFrameMapRow = m_lScannedMapMinRow;
			if( lFrameMapCol<m_lScannedMapMinCol )
				lFrameMapCol = m_lScannedMapMinCol;

			BOOL bMapHasSortDie = FALSE;
			for(LONG jRow=(0-lHalfRow); jRow<=(0+lHalfRow); jRow++)
			{
				for(LONG iCol=(0-lHalfCol); iCol<=(0+lHalfCol); iCol++)
				{
					LONG lChkRow = jRow+lFrameMapRow;
					LONG lChkCol = iCol+lFrameMapCol;
					if( lChkRow<m_lScannedMapMinRow || lChkRow>m_lScannedMapMaxRow || lChkCol<m_lScannedMapMinCol || lChkCol>m_lScannedMapMaxCol )
					{
						continue;
					}
					UCHAR ucGrade = m_WaferMapWrapper.GetGrade(lChkRow, lChkCol);
					if( ucGrade==ucNullBin )
					{
						continue;
					}
				//	if( m_WaferMapWrapper.IsReferenceDie(lChkRow, lChkCol) && ucGrade==ucRefGrade )
				//	{
				//		continue;
				//	}
				//	if( IsScanMapNgGrade(ucGrade-ucOffset) )
				//	{
				//		continue;
				//	}
				//	LONG lChkEncX = 0, lChkEncY = 0;
				//	if( GetMapPhyPosn(lChkRow, lChkCol, lChkEncX, lChkEncY)==FALSE )
				//	{
				//		continue;
				//	}
					if( m_bAutoRescanAllDice )	//	rescan all available die
					{
						bMapHasSortDie = TRUE;
					}
					else
					{
						for (int i=0; i < aulSortGradeList.GetSize(); i++)
						{
							if ((aulSortGradeList.GetAt(i)) == ucGrade)
							{
								bMapHasSortDie = TRUE;
								break;
							}
						}
					}
					if( bMapHasSortDie )
					{
						break;
					}
				}
				if( bMapHasSortDie )
				{
					break;
				}
			}

			if( bMapHasSortDie==FALSE )
			{
				continue;
			}

			LONG nPosnX = 0, nPosnY = 0;
			BOOL bWaferHasPosn = PredictGetDieValidPosn(lFrameMapRow, lFrameMapCol, lMapIndexLoop, nPosnX, nPosnY, TRUE);
			if( bWaferHasPosn==FALSE )
			{
				continue;
			}

			if( IsWithinWaferLimit(nPosnX, nPosnY)==FALSE )
			{
				LONG lBlkMinRow = lFrameMapRow;
				LONG lBlkMinCol = lFrameMapCol;
				LONG lBlkMaxRow = lFrameMapRow;
				LONG lBlkMaxCol = lFrameMapCol;
				for(LONG jRow=(0-lHalfRow); jRow<=(0+lHalfRow); jRow++)
				{
					for(LONG iCol=(0-lHalfCol); iCol<=(0+lHalfCol); iCol++)
					{
						LONG lChkRow = jRow+lFrameMapRow;
						LONG lChkCol = iCol+lFrameMapCol;
						if( lChkRow<m_lScannedMapMinRow || lChkRow>m_lScannedMapMaxRow ||
							lChkCol<m_lScannedMapMinCol || lChkCol>m_lScannedMapMaxCol )
						{
							continue;
						}
						LONG lChkEncX = 0, lChkEncY = 0;
						if( GetMapPhyPosn(lChkRow, lChkCol, lChkEncX, lChkEncY) )
						{
							lBlkMinRow = min(lBlkMinRow, lChkRow);
							lBlkMaxRow = max(lBlkMaxRow, lChkRow);
							lBlkMinCol = min(lBlkMinCol, lChkCol);
							lBlkMaxCol = max(lBlkMaxCol, lChkCol);
						}
					}
				}
				lFrameMapRow = (lBlkMinRow + lBlkMaxRow)/2;
				lFrameMapCol = (lBlkMinCol + lBlkMaxCol)/2;

				BOOL bWaferHasPosn = PredictGetDieValidPosn(lFrameMapRow, lFrameMapCol, lMapIndexLoop, nPosnX, nPosnY, TRUE);
				if( bWaferHasPosn==FALSE )
				{
					continue;
				}
			}

			//if( nPosnX>m_lScannedWaferUL_X || nPosnX<m_lScannedWaferLR_X ||
			//	nPosnY>m_lScannedWaferUL_Y || nPosnY<m_lScannedWaferLR_Y )
			//{
			//	continue;
			//}	//	rescan XX

			if( m_lScanFrameMinRow>lFrameRow )
				m_lScanFrameMinRow = lFrameRow;
			if( m_lScanFrameMinCol>lFrameCol )
				m_lScanFrameMinCol = lFrameCol;
			if( m_lScanFrameMaxRow<lFrameRow )
				m_lScanFrameMaxRow = lFrameRow;
			if( m_lScanFrameMaxCol<lFrameCol )
				m_lScanFrameMaxCol = lFrameCol;
			m_baScanMatrix[lFrameRow][lFrameCol]	= TRUE;
			m_laScanPosnX[lFrameRow][lFrameCol]		= nPosnX;
			m_laScanPosnY[lFrameRow][lFrameCol]		= nPosnY;
			laScanMapRow[lFrameRow][lFrameCol] = lFrameMapRow;
			laScanMapCol[lFrameRow][lFrameCol] = lFrameMapCol;
		}

		if( bHasDie==FALSE )
		{
			break;
		}
	}

	for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
	{
		CString szMsg, szTemp;
		for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
		{
			LONG lWaferRow = laScanMapRow[nRow][nCol];
			LONG lWaferCol = laScanMapCol[nRow][nCol];
			szTemp.Format("(%5ld,%5ld),", lWaferRow, lWaferCol);
			szMsg += szTemp;
			if( m_baScanMatrix[nRow][nCol]==FALSE )
			{
				continue;
			}

			ULONG lFrameBasePoints = 0;
			while( 1 )
			{
				BOOL bFindOne = FALSE;
				for(UINT ulIndex=0; ulIndex<pUtl->GetRescanBasePointsNum(); ulIndex++)
				{
					ULONG ulBaseRow = 0, ulBaseCol = 0;
					LONG lBaseWfX = 0, lBaseWfY = 0;
					pUtl->GetRescanBasePoint(ulIndex, ulBaseRow, ulBaseCol, lBaseWfX, lBaseWfY);
					if (labs(lWaferRow - (LONG)ulBaseRow) <= lMapLoopRow && labs(lWaferCol - (LONG)ulBaseCol) <= lMapLoopCol)
					{
						SetFrameRescanBasePoint(nRow, nCol, lFrameBasePoints, (LONG)ulBaseRow, (LONG)ulBaseCol, lBaseWfX, lBaseWfY);
						lFrameBasePoints++;
						pUtl->CutRescanBasePoint(ulIndex);
						bFindOne = TRUE;
						break;
					}
				}
				if (bFindOne == FALSE)
				{
					LONG lULRow = lWaferRow, lULCol = lWaferCol;
					LONG lLRRow = lWaferRow + lMapLoopRow;
					LONG lLRCol = lWaferCol + lMapLoopCol;
					if (lWaferRow >= lMapLoopRow)
						lULRow = lWaferRow - lMapLoopRow;
					if (lWaferCol >= lMapLoopCol)
						lULCol = lWaferCol - lMapLoopCol;

					ULONG ulBaseRow = 0, ulBaseCol = 0;
					LONG lBaseWfX = 0, lBaseWfY = 0;
					if (GetValidMapDieInBlock(-1, lULRow, lULCol, lLRRow, lLRCol, ulBaseRow, ulBaseCol))
					{
						if (PredictGetDieValidPosn(ulBaseRow, ulBaseCol, 0, lBaseWfX, lBaseWfY, TRUE))
						{
							SetFrameRescanBasePoint(nRow, nCol, lFrameBasePoints, ulBaseRow, ulBaseCol, lBaseWfX, lBaseWfY);
							lFrameBasePoints++;
						}
					}
					break;
				}
			}
			if( lFrameBasePoints>0 )
			{
				SetRescanFrameBaseNum(nRow, nCol, lFrameBasePoints);	//	set frame's map base points number.
			}
		}
		pUtl->PrescanMoveLog(szMsg, TRUE);
	}

	m_lLastFrameRow		= m_lNextFrameRow		= GetAlignFrameRow();
	m_lLastFrameCol		= m_lNextFrameCol		= GetAlignFrameCol();

	szMsg.Format("WFT - rescan frame dimension %d,%d==>%d,%d", m_lScanFrameMinRow, m_lScanFrameMinCol, m_lScanFrameMaxRow, m_lScanFrameMaxCol);
	pUtl->PrescanMoveLog(szMsg, TRUE);
	szMsg.Format("rescan table frame move position");
	pUtl->PrescanMoveLog(szMsg, TRUE);

	for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
	{
		CString szMsg, szTemp;
		for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
		{
			if( IsScanFrameInWafer(nRow, nCol) )
			{
				szTemp.Format("(%8ld,%8ld),", m_laScanPosnX[nRow][nCol], m_laScanPosnY[nRow][nCol]);
			}
			else
			{
				szTemp.Format("(        ,        ),");
			}
			szMsg += szTemp;
		}
		pUtl->PrescanMoveLog(szMsg, TRUE);
	}

	FILE *fpBase = NULL;	//Klocwork
	CString szConLogFile;
	szConLogFile.Format("%s_%07lu%s", m_szPrescanLogPath, GetNewPickCount(), RESCAN_BASS_IN_FILE);
	errno_t nErr = fopen_s(&fpBase, szConLogFile, "w");
	if ((nErr == 0) && (fpBase != NULL))
	{
		szMsg.Format("table index in scan FOV map list");
		pUtl->PrescanMoveLog(szMsg, TRUE);
		for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
		{
			for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
			{
				ULONG lBasePoints = 0;
				if( GetRescanFrameBaseNum(nRow, nCol, lBasePoints) )	//	get frame's row and column log to move posn.
				{
					CString szMsg = "", szTemp = "";
					szMsg.Format("Frame %d,%d base %d ", nRow, nCol, lBasePoints);
					for(ULONG ulIndex=0; ulIndex<lBasePoints; ulIndex++)
					{
						LONG lBaseRow = 0, lBaseCol = 0, lBaseWfX = 0, lBaseWfY = 0;
						if( GetFrameRescanBasePoint(nRow, nCol, ulIndex, lBaseRow, lBaseCol, lBaseWfX, lBaseWfY) )	//	log
						{
							szTemp.Format("(%5d,%5d),", lBaseRow, lBaseCol);
							szMsg += szTemp;
							if( fpBase!=NULL )
							{
								LONG lUserRow = 0, lUserCol = 0;
								ConvertAsmToOrgUser(lBaseRow, lBaseCol, lUserRow, lUserCol);
								fprintf(fpBase, "%8ld,%8ld,%4ld,%4ld,%4ld,%4ld,%2ld,%2ld,%4lu\n", 
									lBaseWfX, lBaseWfY, lBaseRow, lBaseCol, lUserRow, lUserCol, nRow, nCol, ulIndex);
							}
						}
					}
					pUtl->PrescanMoveLog(szMsg, TRUE);
				}
			}
		}
		if( fpBase!=NULL )
		{
			fclose(fpBase);
		}
	}

	szMsg.Format("frame move points");
	pUtl->PrescanMoveLog(szMsg, TRUE);
	for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
	{
		CString szMsg, szTemp;
		for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
		{
			if ( (nRow >= SCAN_MATRIX_MIN_ROW) && (nRow <= SCAN_MAX_MATRIX_ROW) && 
					(nCol >= SCAN_MATRIX_MIN_COL) && (nCol <= SCAN_MAX_MATRIX_COL) )	//v4.47 Klocwork
			{
				szTemp.Format("%d", m_baScanMatrix[nRow][nCol]);
				szMsg += szTemp;
			}
		}
		pUtl->PrescanMoveLog(szMsg, TRUE);
	}


	BOOL bFindFirst = FALSE;
	LONG lFirstRow = 0, lFirstCol = 0;
	switch( m_ucRunScanWalkTour )
	{
	case WT_SCAN_WALK_TOP_HORI:
		for(lFirstRow=SCAN_MATRIX_MIN_ROW; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
		{
			for(lFirstCol=SCAN_MATRIX_MIN_COL; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
			{
				if( IsScanFrameInWafer(lFirstRow, lFirstCol) )
				{
					bFindFirst = TRUE;
					break;
				}
			}
			if( bFindFirst )
			{
				break;
			}
		}
		break;
	case WT_SCAN_WALK_LEFT_VERT:
		for(lFirstCol=SCAN_MATRIX_MIN_COL; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
		{
			for(lFirstRow=SCAN_MATRIX_MIN_ROW; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
			{
				if( IsScanFrameInWafer(lFirstRow, lFirstCol) )
				{
					bFindFirst = TRUE;
					break;
				}
			}
			if( bFindFirst )
			{
				break;
			}
		}
		break;
	}

	if( bFindFirst )
	{
		LONG lFirstWfX = m_laScanPosnX[lFirstRow][lFirstCol];
		LONG lFirstWfY = m_laScanPosnY[lFirstRow][lFirstCol];
		LONG lScanRow = m_nPrescanAlignMapRow;
		LONG lScanCol = m_nPrescanAlignMapCol;

		if( GetDiePitchX_X()!=0 && GetDiePitchY_Y()!=0 )
		{
			lScanRow = (GetPrescanAlignPosnY() - lFirstWfX)/GetDiePitchY_Y() + m_nPrescanAlignMapRow;
			lScanCol = (GetPrescanAlignPosnX() - lFirstWfY)/GetDiePitchX_X() + m_nPrescanAlignMapCol;
			if( lScanRow<=0 )
				lScanRow = 0;
			if( lScanCol<0 )
				lScanCol = 0;
			ULONG ulMaxRow = 0, ulMaxCol = 0;
			m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
			if(lScanRow>=(LONG)ulMaxRow )
				lScanRow = ulMaxRow-1;
			if(lScanCol>=(LONG)ulMaxCol )
				lScanCol = ulMaxCol-1;
		}
		m_nPrescanNextMapRow	= m_nPrescanLastMapRow	= lScanRow;
		m_nPrescanNextMapCol	= m_nPrescanLastMapCol	= lScanCol;
		m_nPrescanNextWftPosnX	= m_nPrescanLastWftPosnX = lFirstWfX;
		m_nPrescanNextWftPosnY	= m_nPrescanLastWftPosnY = lFirstWfY;
		m_lLastFrameRow		= m_lNextFrameRow	= lFirstRow;
		m_lLastFrameCol		= m_lNextFrameCol	= lFirstCol;
	}

	return TRUE;
}


LONG CWaferTable::RescanAllDoneOnWft()		//	rescan XX	4
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	pUtl->PrescanMoveLog("WFT - Rescan all done to build up", TRUE);
	SaveScanTimeEvent("    WFT: rescan all done begin");

	UCHAR ucOffset  = m_WaferMapWrapper.GetGradeOffset();
	CString szScnFolder = m_szPrescanLogPath;

	CTime stTime = CTime::GetCurrentTime();
	CString szTime;
	szTime.Format("_%2d%2d%2d%2d", stTime.GetDay(), stTime.GetHour(), stTime.GetMinute(), stTime.GetSecond());

//	SaveScanTimeEvent("    WFT: rescan to save prescan info pr");
//	SavePrescanInfoPr(GetNewPickCount());


	if( IsAutoRescanAndDEB() )	// rescan+deb XXX	update all key die in rescan all done on wft
	{
		SaveScanTimeEvent("    WFT: rescan to backup for deb relative log");
		m_ulPdcRegionSampleCounter++;
		CString szLogPath = m_szPrescanLogPath;
		CString szLogFileName, szBkuFileName;
		szLogFileName = szLogPath + "_KeyDie.csv";
		szBkuFileName.Format("%s_KeyDie_%03d.csv", szLogPath, m_ulPdcRegionSampleCounter);
		RenameFile(szLogFileName, szBkuFileName);

		szLogFileName	= szLogPath + PRESCAN_MSLOG_INIT;
		szBkuFileName.Format("%s_MsLog_%03d.txt", szLogPath, m_ulPdcRegionSampleCounter);
		RenameFile(szLogFileName, szBkuFileName);

		szLogFileName	= szLogPath + PRESCAN_MSLOG_KEYC;
		szBkuFileName.Format("%s_MsLogKeyC_%03d.txt", szLogPath, m_ulPdcRegionSampleCounter);
		RenameFile(szLogFileName, szBkuFileName);

		szLogFileName	= szLogPath + PRESCAN_MSLOG_XY;
		szBkuFileName.Format("%s_MsLogXY_%03d.txt", szLogPath, m_ulPdcRegionSampleCounter);
		RenameFile(szLogFileName, szBkuFileName);

		SaveScanTimeEvent("    WFT: deb relative update and key die");
		DEB_UpdateAllDicePos();
		DEB_CheckKeyDicesOnWafer();
	}

	pUtl->DelRescanBasePoints();	//	rescan all done
	// build prescan map complete
	SaveScanTimeEvent("    WFT: rescan to clear temp. data");
	WSClearScanRemainDieList();

	SaveScanTimeEvent("    WFT: rescan move back to last pick die.");
	LONG lAlignX = m_lRescanLastDiePosnX;
	LONG lAlignY = m_lRescanLastDiePosnY;
	XY_SafeMoveTo(lAlignX, lAlignY);
#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	WftMoveBondArmToSafe(FALSE);	// rescan move away.
	m_lReadyToSampleOnWft = 0;		//	rescan done
	m_lReadyToCleanEjPinOnWft	= 0;
	BH_EnableThermalControl(FALSE);	//	rescan done

	m_bPrescanningMode		= FALSE;
	m_lRescanRunStage		= 0;	//	MS rescan all done and resume to sorting
	m_bIsPrescanning		= FALSE;
	m_bWaferAlignComplete	= TRUE;
	m_lRescanPECounter		= GetNewPickCount();	//	rescan done, remember last counter.
	m_bToDoSampleOncePE		= FALSE;

	if( pUtl->GetPrescanDebug() )
	{
		SaveScanTimeEvent("    WFT: rescan write upd map");
		CString szNewMapPath;
		szNewMapPath.Format("%s_%07lu%s", szScnFolder, GetNewPickCount(), RESCAN_NEW_MAP_FILE);
		m_WaferMapWrapper.WriteMap(szNewMapPath);
	
		SavePrescanInfo(2, GetNewPickCount());
		SavePrescanInfoMap(2);
	}

	CTime stEndTime = CTime::GetCurrentTime();
	CTimeSpan stTimeSpan = stEndTime - m_stRescanStartTime;
	CString szLog;
	szLog.Format("rescan wafer used time %d s", stTimeSpan.GetTotalSeconds());
	pUtl->PrescanMoveLog(szLog, TRUE);
	SaveScanTimeEvent("    WFT: " + szLog);
	CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(m_stRescanStartTime, "RescanWafer");

	BOOL bContinue = TRUE;
	SaveScanTimeEvent("    WFT: rescan to sort again");
	DisplaySequence("WFT: rescan wafer done to sort again");

	BOOL bRescanDebPitchError		= FALSE;
	UINT unDebug = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Asm special debug flag"), 0);
	if( bRescanDebPitchError && unDebug>0  )
	{
		HmiMessage_Red_Yellow("The rescan + DEB has pitch error, please check.", "Rescan");
		bContinue = FALSE;
	}

	if( m_bReSampleAsError )	//	rescan done, reset the flag.
	{
		HmiMessage_Red_Yellow("The rescan done successfully, please check.", "Rescan");
		bContinue = FALSE;
	}
	m_bReSampleAsError = FALSE;	//	rescan done
	m_ulPitchAlarmCount = 0;

//	if( pUtl->GetPrescanDebug() )
	{
		CString szTgtSavePath, szOldSavePath;

		szOldSavePath	= szScnFolder + PRESCAN_MOVE_POSN;
		szTgtSavePath.Format("%s_%07lu%s", szScnFolder, GetNewPickCount(), PRESCAN_MOVE_POSN);
		RenameFile(szOldSavePath, szTgtSavePath);
	}

	m_bFirstInAutoCycle = TRUE;	//	rescan wafer done

	return bContinue;
}

VOID CWaferTable::RescanAdvOffsetNextLimit()
{
	if( m_ulAdvStage3StartCount>0 && m_ulAdvStage3SampleSpan>0 && 
		GetNewPickCount()>=(m_ulAdvStage3StartCount+m_ulLastRscnPickCount) )
	{
		m_ulNextAdvSampleCount = GetNewPickCount() + m_ulAdvStage3SampleSpan;
	}
	else if( m_ulAdvStage2StartCount>0 && m_ulAdvStage2SampleSpan>0 && 
		GetNewPickCount()>=(m_ulAdvStage2StartCount+m_ulLastRscnPickCount) )
	{
		m_ulNextAdvSampleCount = GetNewPickCount() + m_ulAdvStage2SampleSpan;
	}
	else
	{
		m_ulNextAdvSampleCount = GetNewPickCount() + m_ulAdvStage1SampleSpan;
	}
}	// auto increase next sample/rescan counter limit

VOID  CWaferTable::ChangeNextSampleLimit(ULONG ulPostCount)	//	only when fatal error such as PR or PE
{
	if (IsAutoRescanAndSampling() || IsAutoRescanAndDEB())
	{
		m_ulNextRescanCount	= GetNewPickCount() + ulPostCount;	// fatal error such pitch or PR no die
	}
	else
	{
		m_ulNextAdvSampleCount = GetNewPickCount() + ulPostCount;
	}
}

ULONG CWaferTable::GetNextRscnLimit()
{
	if (IsAutoRescanAndSampling() || IsAutoRescanAndDEB())
	{
		if( labs(m_ulNextRescanCount-GetNextXXXCounter())<5 )
		{
			m_ulNextAdvSampleCount = m_ulNextRescanCount + 10;
		}

		if( m_ulNextRescanCount>1 )
		{
			return m_ulNextRescanCount-1;
		}
		return m_ulNextRescanCount;
	}
	return GetNextXXXCounter();
}

