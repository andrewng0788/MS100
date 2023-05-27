#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "math.h"
#include "MS_SecCommConstant.h"
#include "FileUtil.h"
#include "WaferPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CWaferTable::MS90SetWaferLimitY(IPC_CServiceMessage& svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	LONG lLimit = 0;
	svMsg.GetMsg(sizeof(LONG), &lLimit);

	UINT unNegLimit = labs(lLimit);
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("MS90 WFT Neg Limit Y"), unNegLimit);
	m_lYNegLimit = lLimit;
	CMS896AStn::m_lWafYNegLimit = m_lYNegLimit;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::MS90SortTo2ndHalf(IPC_CServiceMessage& svMsg)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( IsAllSorted() )
	{
		HmiMessage_Red_Back("Whole wafer sorting complete.", "MS90 Half Sort");
	}
	else
	{
		CMS896AStn::m_bRealignFrameDone = FALSE;	//	rotate wafer and map, need to realign bin frame.
		SetAlignmentStatus(FALSE);  // Need alignment wafer
		m_pWaferMapManager->SuspendSortingPathAlgorithmPreparation();

		//XY_SafeMoveTo(0, 0);
		BOOL bMove = XY_SafeMoveTo(m_lWaferCalibX, m_lWaferCalibY);
		if( bMove==FALSE )
		{
			CString szMsg;
			szMsg.Format("Invalid Wafer COR Value (X = %d, Y = %d), please learn COR again", m_lWaferCalibX, m_lWaferCalibY);
			CString szTitle = "Rotate 180";
			HmiMessageEx_Red_Back(szMsg, szTitle);
			BOOL bReturn = TRUE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		if( IsMS90Sorting2ndPart()==FALSE )
		{
			MS90Set1stDone( TRUE );
			m_bSortGoingTo2ndPart = TRUE;
			USHORT usRotate = 2*90;
			if (m_ucMapRotation >= 2)
			{
				usRotate = (m_ucMapRotation - 2) * 90;
			}
			else
			{
				usRotate = (m_ucMapRotation + 2) * 90;
			}

			if (pUtl->GetPrescanDummyMap())
			{
				usRotate = 2 * 90;
			}

			m_WaferMapWrapper.Rotate(usRotate);
			MS90HalfSortMapAction(FALSE);	// first part done, need to restore second part.
			LONG  lHomeWftT = 0 - m_lThetaMotorDirection * (LONG)(180.0/m_dThetaRes);
			T_Move(lHomeWftT, SFM_WAIT);
		}
		else
		{
			INT nPos = GetAutoWaferT();
			T_MoveTo(nPos, SFM_WAIT);	// Move table

			MS90Set1stDone( FALSE );
			m_bSortGoingTo2ndPart = FALSE;
			USHORT usRotate =m_ucMapRotation*90;
			if (pUtl->GetPrescanDummyMap())
			{
				usRotate = 0;
			}
//=====================2018.1.16=================================================
			m_WaferMapWrapper.Rotate(usRotate);
			MS90HalfSortMapAction(TRUE);	// restore back to first part.
//================================================================================
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// sort mode, align 2nd part wafer; move, rotate, map loading, do alignment.
// -1 - error; 0 - align fail; 1 - the mode align ok; 2 - MS90 auto align.
LONG CWaferTable::AutoAlign2PhasesSortSecondPart(IPC_CServiceMessage& svMsg)
{
	LONG lReturn = -1;

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	CString szMsg;
	USHORT usRotate = 2*90;
	if( pUtl->GetPrescanDummyMap()==FALSE )
	{
		if( m_ucMapRotation>=2 )
			usRotate = (m_ucMapRotation-2)*90;
		else
			usRotate = (m_ucMapRotation+2)*90;
	}
	DWTDSortLog("\n");
	DWTDSortLog("WT: begin auto align 2nd part wafer");
	if( IsSorraSortMode() )
	{
		if( IsSortTo2ndPart()==FALSE )	// auto cycle to auto align second part
		{
			svMsg.InitMessage(sizeof(LONG), &lReturn);
			return 1;
		}

		LONG lRow, lCol, lWfX, lWfY, lWfT;
		LONG lOriRow = 0, lOriCol = 0;
		CString szMsg;

		pUtl->GetAlignPosition(lRow, lCol, lWfX, lWfY);
		ConvertAsmToOrgUser(lRow, lCol, lOriRow, lOriCol);

		CString szFileName = GetMapFileName();
		if ( LoadWaferMap(szFileName, "") == FALSE )
		{
			svMsg.InitMessage(sizeof(LONG), &lReturn);
			return 1;
		}
		Sleep(500);
		while (!m_WaferMapWrapper.IsMapValid())
		{
			Sleep(100);
		}

		m_WaferMapWrapper.Rotate(usRotate);

		RotateWaferTheta(&lWfX, &lWfY, &lWfT, 180);

		//v4.08
		m_WaferMapWrapper.SetAlgorithmParameter("Reverse Shape Dir", FALSE);
		m_WaferMapWrapper.ResumeAlgorithmPreparation(TRUE);		//Force map-lib to re-calculate mapping path based on new settings

		ULONG ulRow = 0, ulCol = 0;
		ConvertOrgUserToAsm(lOriRow, lOriCol, ulRow, ulCol);
		m_WaferMapWrapper.SetSelectedPosition(ulRow, ulCol);
		
		szMsg.Format("WT set SELECT posn at %d %d", lOriRow, lOriCol);
		DWTDSortLog(szMsg);

		T_Move(lWfT, SFM_WAIT);
		XY_SafeMoveTo(lWfX, lWfY);
		Sleep(100);

		// init second part and prepare variable and scan raw data and alignment
		SetAlignmentStatus(FALSE);
		ClearPrescanRawData();	// for semi auto mode when press start button

		if( m_b2PartsSortAutoAlignWay )
		{
			lReturn = FindWaferHalfHomeDie();	// soraa for 2nd prescan alignment during sorting	//v4.08
		}
		else
		{
			HmiMessage_Red_Back("Please manual-align wafer again for 2nd part of wafer.");
			lReturn = 0;
		}
	}
	else if( IsMS90HalfSortMode() )	// auto align second part
	{
		USHORT usCurrMapRot = m_WaferMapWrapper.GetRotatedAngle();
		if( usCurrMapRot!=usRotate )
		{
			szMsg.Format("WT: Begin to rotate map from %d to %d", usCurrMapRot, usRotate);
			DWTDSortLog(szMsg);
			m_pWaferMapManager->SuspendSortingPathAlgorithmPreparation();
			m_WaferMapWrapper.Rotate(usRotate);
		//	m_WaferMapWrapper.ResumeAlgorithmPreparation(FALSE);
		//	m_WaferMapWrapper.ResumeAlgorithmPrepNoLock();
		//	m_bPrescanWaferEnd4ShortPath = TRUE;	//	old map, first time start, not short path
			szMsg.Format("WT: finish rotate map");
			DWTDSortLog(szMsg);
		}
		MS90HalfSortMapAction(FALSE);	// first part done, need to restore second part.

		//	In Map Loaded , set to false, rotate wafer table theta back to global theta./gt -180 degree.
		if( IsSortTo2ndPart() )	// auto cycle to auto align second part
		{
			LONG  lHomeWftT = 0 - m_lThetaMotorDirection * (LONG)(180.0/m_dThetaRes);
			GetEncoderValue();
			LONG lEncT = GetCurrT();
			INT nPos = GetAutoWaferT();
			nPos = nPos + lHomeWftT;
			if( fabs((lEncT-nPos)*m_dThetaRes)>20.0 )
			{
				XY_SafeMoveTo(0, 0);
				T_Move(nPos, SFM_WAIT);
			}
			DWTDSortLog("MS90 rotate wafer table T 180 degree done");
		}
		LONG  lHomeWftX = 0, lHomeWftY = 0;
		GetHomeDiePhyPosn(lHomeWftX, lHomeWftY);
		XY_SafeMoveTo(lHomeWftX, lHomeWftY);
		DWTDSortLog("MS90 move wafer table to home position");

		ULONG ulHomeRow = 0, ulHomeCol = 0;
		LONG lRowOffset=0, lColOffset=0;
		GetMapOffset(lRowOffset, lColOffset);	// to get map align die position
		GetAsmMapHomeDie(ulHomeRow, ulHomeCol, FALSE, TRUE);
		//Check Row & Col must be >= 0
		if ( ((LONG)ulHomeRow + lRowOffset) < 0 )
		{
			lRowOffset = 0;
		}
		if ( ((LONG)ulHomeCol + lColOffset) < 0 )
		{
			lColOffset = 0;
		}
		ulHomeRow = ulHomeRow + lRowOffset;
		ulHomeCol = ulHomeCol + lColOffset;
		m_WaferMapWrapper.SetSelectedPosition(ulHomeRow, ulHomeCol);
		m_WaferMapWrapper.SetCurrentPosition(ulHomeRow, ulHomeCol);	
		LONG lHomeRow = 0, lHomeCol = 0;
		ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lHomeRow, lHomeCol);
		
		szMsg.Format("WT MS90 set SELECT posn at %ld %ld asm map %lu,%lu(%ld,%ld)", 
			lHomeWftX, lHomeWftY, ulHomeRow, ulHomeCol, lHomeRow, lHomeCol);
		DWTDSortLog(szMsg);

		m_ulCurrentRow = m_ulAlignRow = ulHomeRow;
		m_ulCurrentCol = m_ulAlignCol = ulHomeCol;

		// init second part and prepare variable and scan raw data and alignment
		SetAlignmentStatus(FALSE);
		ClearPrescanRawData();	// for semi auto mode when press start button
		if (IsWLAutoMode() || m_b2PartsSortAutoAlignWay )
		{
			lReturn = FindMS90HalfHomeDie();	// ms90 for 2nd prescan alignment during sorting
		}
		else
		{
			HmiMessage_Red_Back("Please manual-align wafer again for 2nd part of wafer(MS90)");
			lReturn = 0;
		}
	}

	szMsg.Format("WT finish auto align 2nd part wafer, result %d.", lReturn);
	DWTDSortLog(szMsg);

	svMsg.InitMessage(sizeof(LONG), &lReturn);
	return 1;
}	// sort mode, align 2nd part wafer

VOID CWaferTable::MS90Set1stDone(BOOL bSet)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_b2Parts1stPartDone = bSet;

	//v4.59A36
	CString szLog;
	szLog.Format("WT MS90Set1stDone to 2ndPart=%d, 180deg=%d", m_b2Parts1stPartDone, 0);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("MS90 Half Sort 1st Done"), m_b2Parts1stPartDone);
}

VOID CWaferTable::CalcMS90HalfMapMaxRowCol(const BOOL bLoadingMap, const BOOL bRowModeSeparateHalfMap)
{
	ULONG ulMaxRow=0, ulMaxCol=0;
	if (!m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
	{
		return ;
	}

	CString szMsg;

	//ULONG ulAsmHalfRow = ulMaxRow/2+1, ulAsmHalfCol = ulMaxCol/2;
	ULONG ulAsmHalfRow = ulMaxRow/2 + 1, ulAsmHalfCol = ulMaxCol/2;
	if (bLoadingMap)
	{
		if (bRowModeSeparateHalfMap)
		{
			//for MS90
			if( IsScanThenDivideMap() )	//	after prescan, should auto update based on scan area.
			{
				ulAsmHalfRow = ulMaxRow;
			}
			m_ulMS90HalfBorderMapRow = ulAsmHalfRow;

			LONG lOrgMapCol = 0;
			ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, m_lMS90HalfDivideMapOrgRow, lOrgMapCol);

			szMsg.Format("MS90 Loading map dim(%d,%d) edge %d and hide second half divide map at %d,%d", 
				ulMaxRow, ulMaxCol, GetMS90HalfMapMaxRow(), m_lMS90HalfDivideMapOrgRow, lOrgMapCol);
		}
		else
		{
			//For MS50
			if( IsScanThenDivideMap() )	//	after prescan, should auto update based on scan area.
			{
				ulAsmHalfCol = 0;//ulMaxCol;
			}
			m_ulMS90HalfBorderMapCol = ulAsmHalfCol;

			LONG lOrgMapRow = 0;
			ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, lOrgMapRow, m_lMS90HalfDivideMapOrgRow);

			szMsg.Format("MS50 Loading map dim(%d,%d) edge(col) %d and hide second half divide map at %d,%d", 
				ulMaxRow, ulMaxCol, GetMS90HalfMapMaxCol(), lOrgMapRow, m_lMS90HalfDivideMapOrgCol);
		}
	}
	else
	{
		LONG lOrgMapRow = 0, lOrgMapCol = 0;
		if (bRowModeSeparateHalfMap)
		{
			//for MS90
			m_ulMS90HalfBorderMapRow = ulMaxRow - ulMaxRow/2;

			ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, lOrgMapRow, lOrgMapCol);
			ConvertOrgUserToAsm(m_lMS90HalfDivideMapOrgRow, lOrgMapCol, ulAsmHalfRow, ulAsmHalfCol);
			//	m_WaferMapWrapper.UnhideAllDie();
			if (IsScanThenDivideMap())
			{
				m_ulMS90HalfBorderMapRow = ulAsmHalfRow+1;
			}
			szMsg.Format("MS90 2ndhalf map dim(%d,%d) edge %d second part half row %d,%d to asm %d,%d", 
						ulMaxRow, ulMaxCol, GetMS90HalfMapMaxRow(), m_lMS90HalfDivideMapOrgRow, lOrgMapCol, ulAsmHalfRow, ulAsmHalfCol);

			CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
			if( pUtl->GetPrescanDummyMap() )
			{
				m_ulMS90HalfBorderMapRow = ulAsmHalfRow+1;
			}
		}
		else
		{
			//for MS50
			m_ulMS90HalfBorderMapCol = ulMaxCol - ulMaxCol/2;

			ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, lOrgMapRow, lOrgMapCol);
			ConvertOrgUserToAsm(lOrgMapRow, m_lMS90HalfDivideMapOrgCol, ulAsmHalfRow, ulAsmHalfCol);
			//	m_WaferMapWrapper.UnhideAllDie();
			if (IsScanThenDivideMap())
			{
				m_ulMS90HalfBorderMapCol = ulAsmHalfCol + 1;
			}
			szMsg.Format("MS50 2ndhalf map dim(%d,%d) edge(col) %d second part half (row, col) %d,%d to asm (row, col) %d,%d", 
						ulMaxRow, ulMaxCol, GetMS90HalfMapMaxCol(), lOrgMapRow, m_lMS90HalfDivideMapOrgCol, ulAsmHalfRow, ulAsmHalfCol);

			CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
			if( pUtl->GetPrescanDummyMap() )
			{
				m_ulMS90HalfBorderMapCol = ulAsmHalfCol + 1;
			}
		}
	}
	DWTDSortLog(szMsg);
}


VOID CWaferTable::SetLoadingRowModeHalfMapValid(ULONG ulMaxRow, ULONG ulMaxCol)
{
	m_pWaferMapManager->BatchClearDieStatus(0, GetMS90HalfMapMaxRow(), 0, ulMaxCol);

 	m_WaferMapWrapper.UnhideAllDie();

	m_pWaferMapManager->BatchSetMissingDie(GetMS90HalfMapMaxRow(), ulMaxRow, 0, ulMaxCol);
}


VOID CWaferTable::SetLoadingColumnModeHalfMapValid(ULONG ulMaxRow, ULONG ulMaxCol)
{
	m_pWaferMapManager->BatchClearDieStatus(0, ulMaxRow, 0, ulMaxCol);

	m_WaferMapWrapper.UnhideAllDie();

	m_pWaferMapManager->BatchSetMissingDie(0, ulMaxRow, 0, GetMS90HalfMapMaxCol());
}


VOID CWaferTable::MS90HalfSortMapAction(BOOL bLoadingMap)
{
	if( IsMS90HalfSortMode()==FALSE )
	{
		return ;
	}

	ULONG ulMaxRow=0, ulMaxCol=0;
	if (!m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol))
	{
		return ;
	}
/*
	CString szMsg;
	LONG lOrgMapCol = 0;

	if( bLoadingMap )
	{
		ULONG ulAsmHalfRow = ulMaxRow/2+1, ulAsmHalfCol = ulMaxCol/2;
		if( IsScanThenDivideMap() )	//	after prescan, should auto update based on scan area.
			ulAsmHalfRow = ulMaxRow;
		m_ulMS90HalfBorderMapRow = ulAsmHalfRow;
		ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, m_lMS90HalfDivideMapOrgRow, lOrgMapCol);
		szMsg.Format("MS90 Loading map dim(%d,%d) edge %d and hide second half divide map at %d,%d", 
			ulMaxRow, ulMaxCol, GetMS90HalfMapMaxRow(), m_lMS90HalfDivideMapOrgRow, lOrgMapCol);
		DWTDSortLog(szMsg);
	}
	else
	{
		LONG lOrgMapRow = 0;
		ULONG ulAsmHalfRow = ulMaxRow/2+1, ulAsmHalfCol = ulMaxCol/2;
		m_ulMS90HalfBorderMapRow = ulMaxRow - ulMaxRow/2;
		ConvertAsmToOrgUser(ulAsmHalfRow, ulAsmHalfCol, lOrgMapRow, lOrgMapCol);
		ConvertOrgUserToAsm(m_lMS90HalfDivideMapOrgRow, lOrgMapCol, ulAsmHalfRow, ulAsmHalfCol);
	//	m_WaferMapWrapper.UnhideAllDie();
		if( IsScanThenDivideMap() )
			m_ulMS90HalfBorderMapRow = ulAsmHalfRow+1;
		szMsg.Format("MS90 2ndhalf map dim(%d,%d) edge %d second part half row %d,%d to asm %d,%d", 
			ulMaxRow, ulMaxCol, GetMS90HalfMapMaxRow(), m_lMS90HalfDivideMapOrgRow, lOrgMapCol, ulAsmHalfRow, ulAsmHalfCol);
		DWTDSortLog(szMsg);
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		if( pUtl->GetPrescanDummyMap() )
		{
			m_ulMS90HalfBorderMapRow = ulAsmHalfRow+1;
		}
	}
*/
//AfxMessageBox("ClearAllSelection");

	m_WaferMapWrapper.ClearAllSelection();
	BOOL bRowModeSeparateHalfMap = IsRowModeSeparateHalfMap();

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	//ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
	//pSRInfo->SetRegionState(ulTgtRegion, WT_SUBREGION_STATE_INIT);
	if (!bLoadingMap && (pSRInfo->GetSubRows() == 1) && (pSRInfo->GetSubCols() == 1))
	{
//AfxMessageBox("SetupSubRegionMode");
		SetupSubRegionMode(TRUE, TRUE);
	}
	else if (!bLoadingMap && ((pSRInfo->GetSubRows() > 1) || (pSRInfo->GetSubCols() > 1)))
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		if (bRowModeSeparateHalfMap)
		{
			pSRInfo->ResetRegionState(0, GetMS90HalfMapMaxRow(), 0, ulMaxCol);
		}
		else
		{
			pSRInfo->ResetRegionState(0, ulMaxRow, 0, GetMS90HalfMapMaxCol());
		}
		//pSRInfo->Rotate180RegionState();
	}
//AfxMessageBox("CalcMS90HalfMapMaxRowCol");
	CalcMS90HalfMapMaxRowCol(bLoadingMap, bRowModeSeparateHalfMap);
	if (bRowModeSeparateHalfMap)
	{
		SetLoadingRowModeHalfMapValid(ulMaxRow, ulMaxCol);
	}
	else
	{
		SetLoadingColumnModeHalfMapValid(ulMaxRow, ulMaxCol);
	}
	m_bSortingDefectDie = FALSE;
//	m_WaferMapWrapper.Redraw();
	DWTDSortLog("MS90 half sort map, show up part, dim low part.");
}

BOOL CWaferTable::FindWaferHalfHomeDie()	// second part to do auto alignment
{
	//Request switch camera to wafer side
	ChangeCameraToWafer();

	if( m_bEnableGlobalTheta )
	{
		DWTDSortLog("Find Global Theta start");
		//Move to HomeDie Position
		if (FindGlobalAngle() == FALSE)
		{
			DWTDSortLog("Cannot correct wafer angle");
			SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
			return FALSE;
		}
	}

	GetEncoderValue();
	SetGlobalTheta();
	//Move to HomeDie Position
	DWTDSortLog("Find home die begin");
	if( SearchHomeDie()==FALSE )
	{
		DWTDSortLog("Search Home die failed");
		SetAlert_Red_Yellow(IDS_WL_SRCH_HOMEDIE_FAIL);
		return FALSE;
	}

	//Perform auto wafer alignment
	DWTDSortLog("Find home die finish, Align wafer start");
	if( FullAutoAlignWafer(m_bSearchHomeOption, m_ulCornerSearchOption)==FALSE )
	{
		DWTDSortLog("Align wafer failed in auto button\n");
		SetAlert_Red_Yellow(IDS_WL_ALIGNWAF_FAIL);
		return FALSE;
	}

	//  auto align wafer, by auto align button or start button
	PrescanUpdateWaferAlignment(m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol, 4);	// half sort mode auto cycle

	DWTDSortLog("Align wafer finish");

	return TRUE;
}

LONG CWaferTable::SetSoraaModeOption(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetCustomerName() != "Soraa") && (pApp->GetCustomerName() != "Epistar") )
	{
		m_bSoraaSortMode = FALSE;
		SaveData();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//CString szColumnFilename, szContent, szTitle;
	CString szAlgorithm, szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);

	if (IsSorraSortMode())
	{
		szAlgorithm		= _T("TLH-Tri");
		szPathFinder	= _T("LocalMinDistanceTri");
	}
	else
	{
		szAlgorithm		= _T("TLH (Reference Cross)");
		szPathFinder	= _T("LocalMinDistance (Reference Cross)");
	}
		
	m_WaferMapWrapper.SelectAlgorithm(szAlgorithm, szPathFinder);
	m_WaferMapWrapper.Redraw();

	m_szMapTour = szAlgorithm;
	m_szMapPath = szPathFinder;
	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::RotateWaferCmd(IPC_CServiceMessage& svMsg)
{
	UCHAR ucRotate = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucRotate);

	LONG lEncT = GetGlobalT();

	LONG lEnableEngageOffset = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_ENGAGE_OFFSET);

	INT nPos = (INT)((90 * ucRotate) / m_dThetaRes);
	if (CMS896AApp::m_bMS100Plus9InchOption && lEnableEngageOffset != 0)
	{
		LONG lMinPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MIN_DISTANCE);								
		LONG lMaxPos = GetChannelInformation(MS896A_CFG_CH_WAFTABLE_T, MS896A_CFG_CH_MAX_DISTANCE);	

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		int nPosnT = (int) pApp->GetProfileInt(gszPROFILE_SETTING, _T("WTEngagePositionT"), 0);

		if (nPosnT > lMaxPos)
			nPosnT = 0;
		if (nPosnT < lMinPos)
			nPosnT = 0;

		if (ucRotate == 3)				//270 degree = -90 degree
		{
			nPos = (INT)((90 * -1) / m_dThetaRes);
		}

		lEncT += (nPosnT + nPos);
	}
	else if (CMS896AApp::m_bMS100Plus9InchOption)
	{
		if (ucRotate == 3)				//270 degree = -90 degree
		{
			nPos = (INT)((90 * -1) / m_dThetaRes);
		}
		lEncT += nPos;
	}
	else
	{
		lEncT += nPos;
	}

	T1_MoveTo(lEncT, SFM_WAIT);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

// set by UI use map original coordinate
BOOL CWaferTable::DefineAreaAndWalkPath_HalfSortScan()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	LONG ulUlRow = 0, ulLrRow = 0;
	LONG ulUlCol = 0, ulLrCol = 0;
	GetMapValidSize(ulLrRow, ulUlRow, ulLrCol, ulUlCol);

	UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();
	UINT nEdgeX = abs(GetPrescanPitchX())*(unWafEdgeNum + unWafEdgeNum);
	UINT nEdgeY = abs(GetPrescanPitchY())*(unWafEdgeNum + unWafEdgeNum);

	LONG lAlnRow = m_nPrescanAlignMapRow;
	LONG lAlnCol = m_nPrescanAlignMapCol;
	INT lAlnWfX = GetPrescanAlignPosnX();
	INT lAlnWfY = GetPrescanAlignPosnY();

	GetEncoderValue();
	LONG lUserRow=0, lUserCol=0;
	ConvertAsmToOrgUser(lAlnRow, lAlnCol, lUserRow, lUserCol); 
	CString szMsg;
	szMsg.Format("half sort scan corner %d,%d, %d,%d, align map %d,%d(%d,%d), wft %d,%d,%d(%d)", 
		ulUlRow, ulUlCol, ulLrRow, ulLrCol, lAlnRow, lAlnCol, lUserRow, lUserCol, 
		lAlnWfX, lAlnWfY, GetCurrT(), GetGlobalT());
	DWTDSortLog(szMsg);

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	pSRInfo->CalcPrescanRegionSize(ulUlRow, ulUlCol, ulLrRow, ulLrCol,	
							lAlnRow, lAlnCol, lAlnWfX, lAlnWfY, nEdgeX, nEdgeY,
							lDiePitchX_X, lDiePitchX_Y, lDiePitchY_Y, lDiePitchY_X);

	szMsg = "area,";
	pUtl->PrescanMoveLog(szMsg);
	szMsg.Format(",%d,%d", pSRInfo->GetRegionWftULX(), pSRInfo->GetRegionWftULY());
	pUtl->PrescanMoveLog(szMsg);
	szMsg.Format(",%d,%d", pSRInfo->GetRegionWftLRX(), pSRInfo->GetRegionWftLRY());
	pUtl->PrescanMoveLog(szMsg);

	for(INT nRow=0; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
	{
		for(INT nCol=0; nCol<SCAN_MAX_MATRIX_COL; nCol++)
		{
			m_baScanMatrix[nRow][nCol] = FALSE;
		}
	}

	m_lAlignFrameCol = (pSRInfo->GetRegionWftULX() - lAlnWfX) / GetPrescanPitchX();
	m_lAlignFrameRow = (pSRInfo->GetRegionWftULY() - lAlnWfY) / GetPrescanPitchY();

	if( m_lAlignFrameRow>=(SCAN_MAX_MATRIX_ROW-1) )
		m_lAlignFrameRow = SCAN_MAX_MATRIX_ROW-1;
	if( m_lAlignFrameRow<1 )
		m_lAlignFrameRow = 1;
	if( m_lAlignFrameCol>=(SCAN_MAX_MATRIX_COL-1) )
		m_lAlignFrameCol = SCAN_MAX_MATRIX_COL-1;
	if( m_lAlignFrameCol<1 )
		m_lAlignFrameCol = 1;

	m_lLastFrameRow		= m_lNextFrameRow		= GetAlignFrameRow();
	m_lLastFrameCol		= m_lNextFrameCol		= GetAlignFrameCol();

	m_lScanFrameMaxRow	= -1;
	m_lScanFrameMaxCol	= -1;
	m_lScanFrameMinRow	= 9999;
	m_lScanFrameMinCol	= 9999;

	szMsg.Format("align frame %d,%d", GetAlignFrameRow(), GetAlignFrameCol());
	DWTDSortLog(szMsg);
	for(INT nRow=0; nRow<SCAN_MAX_MATRIX_ROW; nRow++)
	{
		for(INT nCol=0; nCol<SCAN_MAX_MATRIX_COL; nCol++)
		{
			LONG lDistRow = nRow - GetAlignFrameRow();
			LONG lDistCol = nCol - GetAlignFrameCol();
			INT nPosnX = lAlnWfX - lDistCol * GetPrescanPitchX();
			INT nPosnY = lAlnWfY - lDistRow * GetPrescanPitchY();

			if( IsPosnWithinMapRange(nPosnX, nPosnY)==FALSE )
			{
				continue;
			}
			if (pSRInfo->IsScanWithinMapRegionRange(nPosnX, nPosnY)==FALSE )
			{
				continue;
			}
			if( IsWithinWaferLimit(nPosnX, nPosnY)==FALSE )
			{	//	not in wafer limit, but over half scan and less than full scan.
				if ( (nPosnY > m_lYNegLimit) )
				{
					continue;
				}
				nPosnY = nPosnY + GetPrescanPitchY()/2;
				if( IsWithinWaferLimit(nPosnX, nPosnY)==FALSE )
				{
					continue;
				}
			}	//	not in wafer limit, but over half scan and less than full scan.

			if( m_lScanFrameMinRow>nRow )
				m_lScanFrameMinRow = nRow;
			if( m_lScanFrameMinCol>nCol )
				m_lScanFrameMinCol = nCol;
			if( m_lScanFrameMaxRow<nRow )
				m_lScanFrameMaxRow = nRow;
			if( m_lScanFrameMaxCol<nCol )
				m_lScanFrameMaxCol = nCol;
			m_baScanMatrix[nRow][nCol]	= TRUE;
			m_laScanPosnX[nRow][nCol]	= nPosnX;
			m_laScanPosnY[nRow][nCol]	= nPosnY;
			szMsg.Format(",%d,%d", nPosnX, nPosnY);
			pUtl->PrescanMoveLog(szMsg);
		}
	}

	szMsg.Format("region frame dimension %d,%d==>%d,%d", m_lScanFrameMinRow, m_lScanFrameMinCol, m_lScanFrameMaxRow, m_lScanFrameMaxCol);
	DWTDSortLog(szMsg);

	for(INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
	{
		nRow = min(nRow, 200);		//Klocwork	//v4.51A20

		CString szMsg, szTemp;
		for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
		{
			nCol = min(nCol, 200);		//Klocwork	//v4.51A20

			szTemp.Format("%d", m_baScanMatrix[nRow][nCol]);
			szMsg += szTemp;
		}
		DWTDSortLog(szMsg);
	}

	for (INT nRow=m_lScanFrameMinRow; nRow<=m_lScanFrameMaxRow; nRow++)
	{
		nRow = min(nRow, SCAN_MAX_MATRIX_ROW);			//v4.50 Klocwork

		CString szMsg = "", szTemp;
		for(INT nCol=m_lScanFrameMinCol; nCol<=m_lScanFrameMaxCol; nCol++)
		{
			nCol = min(nCol, SCAN_MAX_MATRIX_COL);		//v4.50 Klocwork

			if( IsScanFrameInWafer(nRow, nCol) )
			{
				szTemp.Format("%6d,%6d,", m_laScanPosnX[nRow][nCol], m_laScanPosnY[nRow][nCol]);
				szMsg += szTemp;
			}
		}
		DWTDSortLog(szMsg);
	}

	m_ucRunScanWalkTour = m_ucScanWalkTour;
	BOOL bFindFirst = FALSE;
	LONG lFirstRow = 0, lFirstCol = 0;
	switch( m_ucRunScanWalkTour )
	{
	case WT_SCAN_WALK_TOP_HORI:
		for(lFirstRow=0; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
		{
			for(lFirstCol=0; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
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
		for(lFirstCol=0; lFirstCol<SCAN_MAX_MATRIX_COL; lFirstCol++)
		{
			for(lFirstRow=0; lFirstRow<SCAN_MAX_MATRIX_ROW; lFirstRow++)
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
		LONG lScanRow = lAlnRow;
		LONG lScanCol = lAlnCol;

		if( GetDiePitchX_X()!=0 && GetDiePitchY_Y()!=0 )
		{
			lScanRow = (GetPrescanAlignPosnY() - lFirstWfX)/GetDiePitchY_Y() + lScanRow;
			lScanCol = (GetPrescanAlignPosnX() - lFirstWfY)/GetDiePitchX_X() + lScanCol;
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


BOOL CWaferTable::GetMS90HalfSortMapScanArea()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMoveMsg;
	DOUBLE	dCounter = 0.0;			
	LONG lXmove = GetWft1CenterX();
	LONG lYmove = GetWft1CenterY();

	szMoveMsg = "wafer limit,";
	pUtl->PrescanMoveLog(szMoveMsg);
	szMoveMsg.Format(",%d,%d", m_lXPosLimit, m_lYPosLimit);
	pUtl->PrescanMoveLog(szMoveMsg);
	szMoveMsg.Format(",%d,%d", m_lXNegLimit, m_lYNegLimit);
	pUtl->PrescanMoveLog(szMoveMsg);
	szMoveMsg.Format(",%d,%d,%d", lXmove, lYmove, GetWaferDiameter());
	pUtl->PrescanMoveLog(szMoveMsg);

	while(dCounter < 6.28)	
	{			
		lXmove = (LONG)(sin(dCounter) * (DOUBLE)GetWaferDiameter() / 2 + (DOUBLE)GetWft1CenterX());
		lYmove = (LONG)(cos(dCounter) * (DOUBLE)GetWaferDiameter() / 2 + (DOUBLE)GetWft1CenterY());
		dCounter += 0.04;
		szMoveMsg.Format(",%d,%d", lXmove, lYmove);
		pUtl->PrescanMoveLog(szMoveMsg);
	}

	UINT unWafEdgeNum = pApp->GetPrescanWaferEdgeNum();

	INT lAlnWfX = GetPrescanAlignPosnX();
	INT lAlnWfY = GetPrescanAlignPosnY();

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	LONG lMinRow = 0, lMaxRow = 0;
	LONG lMinCol = 0, lMaxCol = 0;
	GetMapValidSize(lMaxRow, lMinRow, lMaxCol, lMinCol);

	LONG lColCtr = (lMinCol + lMaxCol)/2;
	LONG lRowCtr = lMaxRow;

	LONG lDistCol = lColCtr - m_nPrescanAlignMapCol;
	LONG lDistRow = lRowCtr - m_nPrescanAlignMapRow;

	m_nPrescanMapCtrX = lAlnWfX - lDistCol * lDiePitchX_X - lDistRow * lDiePitchY_X;
	m_nPrescanMapCtrY = lAlnWfY - lDistRow * lDiePitchY_Y - lDistCol * lDiePitchX_Y;

	INT  nXWidth = (INT) labs((lMaxCol - lMinCol) * lDiePitchX_X);
	INT  nYHeight= (INT) labs((lMaxRow - lMinRow) * lDiePitchY_Y);
	if( IsScanAlignWafer() )
	{
		nXWidth  = nXWidth * 2;
		nYHeight = nYHeight * 2;
	}

	m_nPrescanMapWidth  = abs(nXWidth)  + abs(GetPrescanPitchX())*unWafEdgeNum*2;
	m_nPrescanMapHeight = abs(nYHeight) + abs(GetPrescanPitchY())*unWafEdgeNum*2;
	m_nPrescanMapRadius = max(m_nPrescanMapWidth/2, m_nPrescanMapHeight/2);

	double dOffsetX = 0.0, dOffsetY = 0.0;
	LONG nMaxRadius = 0, nCrnRadius = 0;

	// below to calculate the scan area to compare with above
	LONG i, j;	// MUST KEEP LONG, IF UNLONG, WOULD DEAD LOOP
	for(i=lMinRow; i<=lMaxRow; i++)	// MUST KEEP LONG, IF UNLONG, WOULD DEAD LOOP
	{
		// first valid die in this row
		for(j=lMinCol; j<=lMaxCol; j++)	// MUST KEEP LONG, IF UNLONG, WOULD DEAD LOOP
		{
			if (m_pWaferMapManager->IsMapHaveBin(i, j))
			{
				lDistCol = j - lColCtr;
				lDistRow = i - lRowCtr;
				dOffsetX =  lDistCol * lDiePitchX_X + lDistRow * lDiePitchY_X;
				dOffsetY =  lDistRow * lDiePitchY_Y + lDistCol * lDiePitchX_Y;
				nCrnRadius = abs((int) sqrt(pow(dOffsetX,2.0) + pow(dOffsetY,2.0)));
				if( nCrnRadius>nMaxRadius )
					nMaxRadius = nCrnRadius;
				break;
			}
		}

		// last valid die in this row
		for(j=lMaxCol; j>=lMinCol; j--)
		{
			if (m_pWaferMapManager->IsMapHaveBin(i, j))
			{
				lDistCol = j - lColCtr;
				lDistRow = i - lRowCtr;
				dOffsetX =  lDistCol * lDiePitchX_X + lDistRow * lDiePitchY_X;
				dOffsetY =  lDistRow * lDiePitchY_Y + lDistCol * lDiePitchX_Y;
				nCrnRadius = abs((int) sqrt(pow(dOffsetX,2.0) + pow(dOffsetY,2.0)));
				if( nCrnRadius>nMaxRadius )
					nMaxRadius = nCrnRadius;
				break;
			}
		}
	}

	dOffsetX = GetPrescanPitchX()*unWafEdgeNum*2;
	dOffsetY = GetPrescanPitchY()*unWafEdgeNum*2;
	int nEdgeCheck = (int) max(dOffsetX, dOffsetY);
	nCrnRadius = abs(nMaxRadius) + nEdgeCheck;
	if( (nCrnRadius)>(m_nPrescanMapRadius) )
		m_nPrescanMapRadius = nCrnRadius;

	m_nPrescanMapWidth  = m_nPrescanMapRadius*2;
	m_nPrescanMapHeight = m_nPrescanMapRadius*2;

	szMoveMsg = "align limit,";
	pUtl->PrescanMoveLog(szMoveMsg);
	szMoveMsg.Format(",%d,%d", lAlnWfX, lAlnWfY);
	pUtl->PrescanMoveLog(szMoveMsg);
	szMoveMsg.Format(",%d,%d", GetScanUL_X(), GetScanUL_Y());
	pUtl->PrescanMoveLog(szMoveMsg);
	szMoveMsg.Format(",%d,%d", GetScanLR_X(), GetScanLR_Y());
	pUtl->PrescanMoveLog(szMoveMsg);
	szMoveMsg.Format(",%d,%d", GetScanCtrX(), GetScanCtrY());
	pUtl->PrescanMoveLog(szMoveMsg);

	dCounter = 0.0;
	while(dCounter < 6.28)	
	{			
		lXmove = (LONG)(sin(dCounter) * (DOUBLE)GetScanRadius()) + GetScanCtrX();
		lYmove = (LONG)(cos(dCounter) * (DOUBLE)GetScanRadius()) + GetScanCtrY();
		dCounter += 0.04;
		szMoveMsg.Format(",%d,%d", lXmove, lYmove);
		pUtl->PrescanMoveLog(szMoveMsg);
	}

	return TRUE;
}

BOOL CWaferTable::FindMS90HalfHomeDie()	// MS90 second part to do auto alignment
{
	//Request switch camera to wafer side
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	ChangeCameraToWafer();//1115

	DWTDSortLog("Ms90 Find Global Theta start");

	//Move to HomeDie Position
	if (FindGlobalAngle() == FALSE)
	{
		DWTDSortLog("Ms90 Cannot correct wafer angle");
		SetAlert_Red_Yellow(IDS_WL_CORRECT_ANGLE_FAIL);
		return FALSE;
	}

	GetEncoderValue();
	SetGlobalTheta();
	//Move to HomeDie Position
	DWTDSortLog("Ms90 Find home die begin");

	//2019.1.18 it should classified as two case with reference die and no reference die
	//for chnagelight, it need search home die
	if (IsScanAlignWafer() != SCAN_ALIGN_WAFER_WITHOUT_REF || pUtl->GetPrescanRegionMode())
	{
		if (m_bSearchHomeOption == 0)
		{
			//Do sprial search
			DWTDSortLog("Ms90 Sprial Search second Start");
			BOOL bReturn = SpiralSearchHomeDie();
			if( IsBlkPickAlign() && bReturn )
			{
				DWTDSortLog("Ms90 Block pick align Search second Start");
				bReturn = BlockPickAlignHalfWafer();
			}
			DWTDSortLog("Ms90 Sprial Search second end");
			if( bReturn==FALSE )
			{
				DWTDSortLog("Search second part Home die failed");
				SetAlert_Red_Yellow(IDS_WL_SRCH_HOMEDIE_FAIL);
				return FALSE;
			}
		}
		else
		{
			if( CornerSearchHalfHomeDie()==FALSE )	//	to find corner of the rect wafer.
			{
				DWTDSortLog("Align wafer failed in auto button\n");
				SetAlert_Red_Yellow(IDS_WL_ALIGNWAF_FAIL);
				return FALSE;
			}
		}
	}
	//Perform auto wafer alignment
	DWTDSortLog("Ms90 Find home die finish, Align wafer start");
	LONG	lX, lY, lT;
	GetEncoder(&lX, &lY, &lT);
	m_lCurrent_X = m_lAlignPosX = lX;
	m_lCurrent_Y = m_lAlignPosY = lY;

	SetAlignStartPoint(FALSE, lX, lY, m_ulAlignRow, m_ulAlignCol);
	DWTDSortLog("Ms90 auto align map");
//	SetPrescanAutoAlignMap();

	m_lStart_X	= lX;
	m_lStart_Y	= lY;

	DWTDSortLog("Ms90 set map start position");
//	m_WaferMapWrapper.SetStartPosition(m_ulAlignRow, m_ulAlignCol); 

	SetAlignmentStatus(TRUE);

	DWTDSortLog("Ms90 Fully-AutoAlign second Wafer home completed");

	(*m_psmfSRam)["MS896A"]["WaferMapRow"] = m_ulAlignRow;
	(*m_psmfSRam)["MS896A"]["WaferMapCol"] = m_ulAlignCol;
	(*m_psmfSRam)["MS896A"]["WaferTableX"] = m_lCurrent_X;
	(*m_psmfSRam)["MS896A"]["WaferTableY"] = m_lCurrent_Y;
	GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_MAN);

	//CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanRegionMode() )
		m_bFirstRegionInit	= TRUE;
	if (m_bFullRefBlock == FALSE || pUtl->GetPrescanRegionMode() )	//Block2
	{
		DWTDSortLog("Ms90 2nd part setup sub region mode");
		SetupSubRegionMode(TRUE);		// MS90 halft sort second part.
	}

	//  auto align wafer, by auto align button or start button
	DWTDSortLog("Ms90 2nd part prescan update wafer alignment");
	PrescanUpdateWaferAlignment(m_lAlignPosX, m_lAlignPosY, m_ulAlignRow, m_ulAlignCol, 7);	// half sort mode auto cycle

	DWTDSortLog("Ms90 Align wafer finish");

	return TRUE;
}

LONG CWaferTable::CornerSearchHalfHomeDie()	//	
{
	LONG siOrigX, siOrigY, siOrigT;
	LONG siTempPhyX, siTempPhyY;
	
	LONG lConfirmResult = 0;
	LONG lRetryCount = 0;
	BOOL bDieFound = FALSE;

	ULONG ulTargetPosition = m_ulCornerSearchOption;	//0 = TL; 1 = TR; 2 = BL; 3 = BR

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	// -------- Start of Search Home Die Step by Step -------//

	//Get current position
	GetEncoder(&siOrigX, &siOrigY, &siOrigT);

	BOOL bNewCornerHome = FALSE;
	ULONG ulTargetCorner = 0;	//0 = TL; 1 = TR; 2 = BL; 3 = BR
	CString szMsg;
	if (IsOsramResortMode() && m_ucMapRotation != 0)
	{
		bNewCornerHome = TRUE;
		switch (m_ucMapRotation)
		{
			//case 0:		//Klocwork	//v4.27
			//	ulTargetCorner = 0;
			//	break;
		case 1:
			ulTargetCorner = 1;
			break;
		case 2:
			ulTargetCorner = 3;
			break;
		case 3:
			ulTargetCorner = 2;
			break;
		}
		szMsg.Format("begin corner search home die at map rotation %d", m_ucMapRotation * 90);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}
	else if (IsLFSizeOK())
	{
		bNewCornerHome = TRUE;
		ulTargetCorner = ulTargetPosition;
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("begin corner search home die with ShareGrab");
	}
	else
	{
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog("begin corner search home die");
	}


	if (bNewCornerHome)
	{
		BOOL bNeedUp = TRUE, bNeedDown = TRUE, bNeedLeft = TRUE, bNeedRight = TRUE;
		LONG lFindResult = 0;
		lConfirmResult = TRUE;
		lRetryCount = 0;
		LONG lIdxDir = WPR_UR_DIE;

//		MultiSrchInitNmlDie1(FALSE);	// for  to speed up time

		while (lRetryCount < 5)
		{
			bNeedUp = FALSE;		
			bNeedDown = FALSE;
			bNeedLeft = FALSE;		
			bNeedRight = FALSE;
			(*m_psmfSRam)["WaferTable"]["Current"]["X"] = siOrigX;
			(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = siOrigY;
			lFindResult = RectWaferAroundDieCheck();

			szMsg.Format("current die around state %ld", lFindResult);
			CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);

			if (lFindResult >= 10000)	// center die remove
			{
				lFindResult -= 10000;
			}
			if (lFindResult >= 1000)		// bottom die remove
			{
				bNeedDown = TRUE;
				lFindResult -= 1000;
			}
			if (lFindResult >= 100)		// right die remove
			{
				bNeedRight = TRUE;
				lFindResult -= 100;
			}
			if (lFindResult >= 10)		// up die remove
			{
				bNeedUp = TRUE;
				lFindResult -= 10;
			}
			if (lFindResult >= 1)		// left die remove
			{
				bNeedLeft = TRUE;
				lFindResult -= 1;
			}

			if (ulTargetCorner == 0)
			{
				if (bNeedUp == FALSE && bNeedLeft == FALSE)	// no any die found at TL
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_UL_DIE;
				if (bNeedUp && bNeedLeft)
				{
					lIdxDir = WPR_UL_DIE;
				}
				else if (bNeedLeft)
				{
					lIdxDir = WPR_LT_DIE;
				}
				else if (bNeedUp)
				{
					lIdxDir = WPR_UP_DIE;
				}
			}
			if (ulTargetCorner == 1)
			{
				if (bNeedUp == FALSE && bNeedRight == FALSE)	// no any die found at TR
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_UR_DIE;
				if (bNeedUp && bNeedRight)
				{
					lIdxDir = WPR_UR_DIE;
				}
				else if (bNeedRight)
				{
					lIdxDir = WPR_RT_DIE;
				}
				else if (bNeedUp)
				{
					lIdxDir = WPR_UP_DIE;
				}
			}
			if (ulTargetCorner == 3)
			{
				if (bNeedDown == FALSE && bNeedRight == FALSE)	// no die at BR
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_DR_DIE;
				if (bNeedDown && bNeedRight)
				{
					lIdxDir = WPR_DR_DIE;
				}
				else if (bNeedDown)
				{
					lIdxDir = WPR_DN_DIE;
				}
				else if (bNeedRight)
				{
					lIdxDir = WPR_RT_DIE;
				}
			}
			if (ulTargetCorner == 2)
			{
				if (bNeedDown == FALSE && bNeedLeft == FALSE)	// no die at BL
				{
					lConfirmResult = FALSE;
					break;
				}
				lIdxDir = WPR_DL_DIE;
				if (bNeedDown && bNeedLeft)
				{
					lIdxDir = WPR_DL_DIE;
				}
				else if (bNeedDown)
				{
					lIdxDir = WPR_DN_DIE;
				}
				else if (bNeedLeft)
				{
					lIdxDir = WPR_LT_DIE;
				}
			}

			siTempPhyX = siOrigX;
			siTempPhyY = siOrigY;

			bDieFound = WftIndexAndSearchDie(&siTempPhyX, &siTempPhyY, lIdxDir, WPR_CT_DIE, TRUE, TRUE, PR_TRUE);

			siOrigX = siTempPhyX;
			siOrigY = siTempPhyY;
			if (bDieFound == TRUE)
			{
				lRetryCount = 0;
			}
			else
			{
				CMSLogFileUtility::Instance()->WPR_AlignWaferLog("here to do something outer check");
				lRetryCount++;
			}
		}
	}
	else	// not bNewCornerHome
	{
		//LONG lSrchPos = WPR_CT_DIE;
		//LONG lIndexPos[WPR_MAX_CORNER_DIE_CHECK];
		//LONG lConfirmPos;
		//switch (ulTargetPosition)
		//{
		//default:	//TL
		//	lIndexPos[0] = WPR_UL_DIE;
		//	lIndexPos[1] = WPR_UP_DIE;
		//	lIndexPos[2] = WPR_LT_DIE;
		//	lIndexPos[3] = WPR_DL_DIE;
		//	lConfirmPos	= WPR_UR_DIE;
		//	break;

		//case 1:		//TR
		//	lIndexPos[0] = WPR_UR_DIE;
		//	lIndexPos[1] = WPR_UP_DIE;
		//	lIndexPos[2] = WPR_RT_DIE;
		//	lIndexPos[3] = WPR_DR_DIE;
		//	lConfirmPos = WPR_UL_DIE;
		//	break;

		//case 2:		//BL
		//	lIndexPos[0] = WPR_DL_DIE;
		//	lIndexPos[1] = WPR_DN_DIE;
		//	lIndexPos[2] = WPR_LT_DIE;
		//	lIndexPos[3] = WPR_UL_DIE;
		//	lConfirmPos = WPR_DR_DIE;
		//	break;

		//case 3:		//BR
		//	lIndexPos[0] = WPR_DR_DIE;
		//	lIndexPos[1] = WPR_DN_DIE;
		//	lIndexPos[2] = WPR_RT_DIE;
		//	lIndexPos[3] = WPR_UR_DIE;
		//	lConfirmPos = WPR_DL_DIE;
		//	break;
		//}

		//BOOL bMoveTable = TRUE;
		//BOOL bUseIndex = TRUE;
		//PR_BOOLEAN bPRLatch = PR_TRUE;

		//if (IsLFSizeOK())
		//{
		//	bUseIndex	= FALSE;
		//	bMoveTable	= FALSE;
		//	bPRLatch	= PR_TRUE;
		//}
		//else
		//{
		//	//Index 1 by 1
		//	bUseIndex	= TRUE;
		//	bMoveTable	= TRUE;
		//	bPRLatch	= PR_TRUE;
		//}

		//int i;
		//while (lRetryCount < 2)
		//{
		//	//Search IndexPos die (For LF, 1 cycle only no need to move table & die compensation)
		//	for (i = 0; i < WPR_MAX_CORNER_DIE_CHECK; i++)
		//	{
		//		if (bUseIndex == FALSE)
		//		{
		//			lSrchPos = lIndexPos[i];
		//		}
	
		//		siTempPhyX = siOrigX;
		//		siTempPhyY = siOrigY;
	
		//		bDieFound = (BOOL)WftIndexAndSearchDie(&siTempPhyX, &siTempPhyY, lIndexPos[i], lSrchPos, bMoveTable, TRUE, bPRLatch);	//v2.78T1
	
		//		bMoveTable = bUseIndex;
		//		bPRLatch = (PR_BOOLEAN)bUseIndex;
	
		//		if (bDieFound == TRUE)
		//		{
		//			break;
		//		}
		//	}
	
		//	if (bDieFound == TRUE)
		//	{
		//		siOrigX = siTempPhyX;
		//		siOrigY = siTempPhyY;
	
		//		//If LF use, need to grab PR image again after die is found
		//		if (bUseIndex == FALSE)
		//		{
		//			bPRLatch = PR_TRUE;
		//		}
	
		//		lRetryCount = 0;
		//	}
		//	else
		//	{
		//		if (!IsLFSizeOK())
		//		{
		//			lSrchPos = WPR_CT_DIE;
	
		//			if (bUseIndex == FALSE)
		//			{
		//				lSrchPos = lConfirmPos;
		//			}
	
		//			if (lRetryCount == 0)
		//			{
		//				//Search ConfirmPos die
		//				siTempPhyX = siOrigX;
		//				siTempPhyY = siOrigY;
		//				lConfirmResult = WftIndexAndSearchDie(&siTempPhyX, &siTempPhyY, lConfirmPos, lSrchPos, bMoveTable, FALSE, bPRLatch);		//v2.78T1
		//			}
	
		//			//Move 1 more IndexPos
		//			siTempPhyX = siOrigX;
		//			siTempPhyY = siOrigY;
		//			WftIndexAndSearchDie(&siTempPhyX, &siTempPhyY, lIndexPos[lRetryCount], WPR_CT_DIE, TRUE, TRUE, PR_TRUE);		//v2.78T1
	
		//			lRetryCount++;
		//		}
		//		else
		//		{
		//			typedef struct 
		//			{
		//				ULONG	ulCornerOption;
		//				BOOL	bLfResult;
		//				BOOL	bFovSize;
		//			} SRCH_LR_TYPE;
		//			SRCH_LR_TYPE	stSrchLrCornerInfo;
		//			IPC_CServiceMessage stMsg;
		//			stMsg.InitMessage(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

		//			INT nDiePos = 0;
		//			if (ulTargetPosition == 1)	//If UR		//v4.40T11
		//			{
		//				AlignRectWaferURCorner(1);
		//			}
		//			else
		//			{
		//				nDiePos = LookAheadULeftRectWaferCorner(stMsg);
		//				stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);

		//				//pllm	//Retry!!
		//				//v4.01		//Re-implement for Cree China
		//				if (nDiePos == WPR_UR_DIE)
		//				{
		//					int nX = 0, nY = 0, nT = 0;
		//					GetWaferTableEncoder(&nX, &nY, &nT);
		//					WftIndexAndSearchDie(&nX, &nY, WPR_UP_DIE, WPR_CT_DIE, TRUE, FALSE, PR_TRUE);
		//
		//					siOrigX = nX;
		//					siOrigY = nY;
		//
		//					Sleep(200);
		//					nDiePos = LookAheadULeftRectWaferCorner(stMsg);
		//					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		//
		//				}
		//				else if (nDiePos == WPR_DL_DIE)
		//				{
		//					int nX = 0, nY = 0, nT = 0;
		//					GetWaferTableEncoder(&nX, &nY, &nT);
		//					WftIndexAndSearchDie(&nX, &nY, WPR_LT_DIE, WPR_CT_DIE, TRUE, FALSE, PR_TRUE);
		//
		//					siOrigX = nX;
		//					siOrigY = nY;
		//
		//					Sleep(200);
		//					nDiePos = LookAheadULeftRectWaferCorner(stMsg);
		//					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		//				}
		//				else if (nDiePos != 1)	//!=0 -> not OK!
		//				{
		//					Sleep(200);
		//					INT nDiePos = LookAheadULeftRectWaferCorner(stMsg);
		//					stMsg.GetMsg(sizeof(SRCH_LR_TYPE), &stSrchLrCornerInfo);
		//				}
		//			}

		//			if (stSrchLrCornerInfo.bLfResult)
		//			{
		//				lConfirmResult = FALSE;
		//			}
		//			else
		//			{	
		//				lConfirmResult = TRUE;
		//			}
	
		//			break;
		//		}
		//	}
		//}
	}

	XY_SafeMoveTo(siOrigX, siOrigY);

	BOOL bReturn = FALSE;
	if (lConfirmResult == FALSE)
	{
		bReturn = TRUE;
		szMsg.Format("cofnirm false and return true at %d,%d", siOrigX, siOrigY);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
	}
	else
	{
		szMsg.Format("cofnirm true at %d,%d", siOrigX, siOrigY);
		CMSLogFileUtility::Instance()->WPR_AlignWaferLog(szMsg);
		//v3.70T3	//PLLM special feature
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		//Allow user to manaul align
		LONG lReply = 0;
		CString szContent = " ";

		lReply = SetAlert_Msg_Red_Yellow(IDS_WPR_CORNER_SRH_FAILED, szContent, "Yes", "No");
		
		//if ( lReply == glHMI_YES )
		if (lReply == 1)
		{
			SetJoystickOn(TRUE);

			CString szContent;
			szContent.LoadString(HMB_WPR_MANUAL_ALIGN_HDIE);
			HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			SetJoystickOn(FALSE);
			bReturn = TRUE;
		}
	}
	// -------- End of Search Home Die Step by Step -------//

	GetEncoder(&siOrigX, &siOrigY, &siOrigT);

	return bReturn;
}

BOOL CWaferTable::WftIndexAndSearchDie(LONG *siPosX, LONG *siPosY, LONG lIdxPos, LONG lSrchPos, BOOL bMoveTable, BOOL bDoComp, PR_BOOLEAN bLatch, PR_BOOLEAN bCheckDefect)
{
	LONG lDiff_X = 0;
	LONG lDiff_Y = 0;

	switch (lIdxPos)
	{
	case WPR_LT_DIE:		
		lDiff_X = -1;
		break;

	case WPR_RT_DIE:
		lDiff_X = 1;
		break;

	case WPR_UP_DIE:
		lDiff_Y = -1;
		break;

	case WPR_DN_DIE:		
		lDiff_Y = 1;
		break;

	case WPR_UL_DIE:		
		lDiff_X = -1;
		lDiff_Y = -1;
		break;

	case WPR_DL_DIE:
		lDiff_X = -1;
		lDiff_Y = 1;
		break;

	case WPR_UR_DIE:
		lDiff_X = 1;
		lDiff_Y = -1;
		break;

	case WPR_DR_DIE:		
		lDiff_X = 1;
		lDiff_Y = 1;
		break;

	default:	
		lDiff_X = 0;
		lDiff_Y = 0;
		break;
	}

	*siPosX = *siPosX - lDiff_X * GetDiePitchX_X() - lDiff_Y * GetDiePitchY_X();
	*siPosY = *siPosY - lDiff_Y * GetDiePitchY_Y() - lDiff_X * GetDiePitchX_Y();

	LONG lNewX = *siPosX;
	LONG lNewY = *siPosY;
	BOOL bReturn = WftMoveSearchDie(lNewX, lNewY, bMoveTable);
	*siPosX = lNewX;
	*siPosY = lNewY;
	return bReturn;
}

BOOL CWaferTable::BlockPickAlignHalfWafer()
{
//	m_ulAlignRow, m_ulAlignCol
	CMS896AApp::m_bStopAlign = FALSE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
#if	1
	m_WaferMapWrapper.EnableSequenceCheck(FALSE);
	SaveBlkData();
	BOOL bResult = Blk2FindAllReferDiePosn();
	if( bResult==FALSE )
	{
		m_WaferMapWrapper.DeleteAllBoundary();
	}

	SetAlignmentStatus(bResult);
	if( bResult==TRUE )
	{
		SetPrescanAutoAlignMap();

		m_pBlkFunc2->Blk2GetStartDie(&m_lBlkHomeRow, &m_lBlkHomeCol, &m_lBlkHomeX, &m_lBlkHomeY);
		//Return to Align position
		SetAlignStartPoint(FALSE, m_lBlkHomeX, m_lBlkHomeY, m_lBlkHomeRow, m_lBlkHomeCol);
		XY_SafeMoveTo(m_lBlkHomeX, m_lBlkHomeY);
		//Update Wafertable & wafer map start position
		m_ulAlignRow	= m_lBlkHomeRow;
		m_ulAlignCol	= m_lBlkHomeCol;
		m_lStart_X		= m_lBlkHomeX;
		m_lStart_Y		= m_lBlkHomeY;
	}

	return bResult;
#else
	ULONG	i, j, ulDieIndex = 0, ulBaseIndex = 0;
	LONG	lAsmRow, lAsmCol, lBaseRow, lBaseCol, lOrgRow = 0, lOrgCol = 0;
	LONG	lBaseWfX, lBaseWfY, lWfX, lWfY;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	BOOL bCheckAllReferDie = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CheckAllReferDie"];
	ChangeCameraToWafer();
    CMS896AApp::m_bStopAlign = FALSE;

	ReferDieMapWaferData	stReferDieInfo[MAXFDC2NUM];
	for(i=0; i<MAXFDC2NUM; i++)
	{
		stReferDieInfo[i].m_bOnMap = FALSE;
		stReferDieInfo[i].m_bOnWaf = FALSE;
		stReferDieInfo[i].m_lAsmCol = 0;
		stReferDieInfo[i].m_lAsmRow = 0;
		stReferDieInfo[i].m_lWftPosX = 0;
		stReferDieInfo[i].m_lWftPosY = 0;
	}

	ULONG	ulMaxRow = 0, ulMaxCol = 0, ulMinDist = 0, ulDist = 0;

	m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
	LONG lHomeRow = m_nPrescanAlignMapRow;
	LONG lHomeCol = m_nPrescanAlignMapCol;
	LONG lHomeWfX = GetPrescanAlignPosnX();
	LONG lHomeWfY = GetPrescanAlignPosnY();

	CString szMsg;
	szMsg.Format("Home %ld,%ld   %ld,%ld", lHomeRow, lHomeCol, lHomeWfX, lHomeWfY);
	WT_SpecialLog(szMsg);
	pUtl->GetAlignPosition(lHomeRow, lHomeCol, lHomeWfX, lHomeWfY);
	szMsg.Format("Home %ld,%ld   %ld,%ld", lHomeRow, lHomeCol, lHomeWfX, lHomeWfY);
	WT_SpecialLog(szMsg);
	WAF_CMapDieInformation *astReferList;
	ULONG ulMapNumOfReferDice = m_WaferMapWrapper.GetNumberOfReferenceDice();
	if( ulMapNumOfReferDice<=0 )
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		if( pUtl->GetPrescanRegionMode() && pSRInfo->GetTotalSubRegions()>1 )
		{
			HmiMessage_Red_Back("Region scan has no reference die in map, please manual align region to sort.", "Prescan");
			pSRInfo->SetCurrentRegionState_HMI("M");
		}
		return FALSE;
	}

	if( ulMapNumOfReferDice>=MAXFDC2NUM )
	{
		ulMapNumOfReferDice = MAXFDC2NUM-1;
	}

	astReferList  = new WAF_CMapDieInformation[ulMapNumOfReferDice];

	szMsg.Format("Map get num of refer die %d", ulMapNumOfReferDice);
	WT_SpecialLog(szMsg);

	m_WaferMapWrapper.GetReferenceDieList(astReferList, ulMapNumOfReferDice);

	WT_SpecialLog("All 2nd Half Reference Die list In Map");
	BOOL bFindHomeDie = FALSE;
	for (i=0; i<ulMapNumOfReferDice; i++)
	{
		lAsmRow = (LONG)astReferList[i].GetRow();
		lAsmCol = (LONG)astReferList[i].GetColumn();

		if( IsOutMS90SortingPart(lAsmRow, lAsmCol) )
		{
			continue;
		}

		stReferDieInfo[i].m_bOnMap = TRUE;
		stReferDieInfo[i].m_lAsmRow = lAsmRow;
		stReferDieInfo[i].m_lAsmCol = lAsmCol;

		if( lAsmRow==lHomeRow && lAsmCol==lHomeCol )
		{
			bFindHomeDie = TRUE;
			stReferDieInfo[i].m_bOnWaf = TRUE;
			stReferDieInfo[i].m_lWftPosX = lHomeWfX;
			stReferDieInfo[i].m_lWftPosY = lHomeWfY;
			szMsg.Format("Home %2ld %4ld,%4ld, %ld, %ld", i+1, lHomeRow, lHomeCol, lHomeWfX, lHomeWfY);
			WT_SpecialLog(szMsg);
		}

		LONG lUserRow = 0, lUserCol = 0;
		ConvertAsmToOrgUser(lAsmRow, lAsmCol, lUserRow, lUserCol);
		szMsg.Format("Map  %2ld %4ld,%4ld (%4ld,%4ld)", i+1, lAsmRow, lAsmCol, lUserRow, lUserCol);
		WT_SpecialLog(szMsg);
	}

	if( bFindHomeDie==FALSE )
	{
		i = ulMapNumOfReferDice;
		ulMapNumOfReferDice += 1;
		stReferDieInfo[i].m_bOnMap = TRUE;
		stReferDieInfo[i].m_bOnWaf = TRUE;
		stReferDieInfo[i].m_lAsmRow = lHomeRow;
		stReferDieInfo[i].m_lAsmCol = lHomeCol;
		stReferDieInfo[i].m_lWftPosX = lHomeWfX;
		stReferDieInfo[i].m_lWftPosY = lHomeWfY;
		LONG lUserRow = 0, lUserCol = 0;
		ConvertAsmToOrgUser(lHomeRow, lHomeCol, lUserRow, lUserCol);
		szMsg.Format("Home %2ld,%4ld,%4ld (%4ld,%4ld)", i+1, lHomeRow, lHomeCol, lUserRow, lUserCol);
		WT_SpecialLog(szMsg);
	}

	szMsg.Format("Map refer die num at last %d\n", ulMapNumOfReferDice);
	WT_SpecialLog(szMsg);

	BOOL bFindOne = FALSE;
	BOOL bLowerDone = FALSE;
	LONG lDiffX, lDiffY;
	LONG lDiePitchX_X	= GetDiePitchX_X();
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();

	ULONG ulHomeDieRow = 0, ulHomeDieCol = 0;
	GetMapAlignHomeDie(ulHomeDieRow, ulHomeDieCol, FALSE);

	while( 1 )
	{
		ulMinDist = (ulMaxRow*ulMaxRow) + (ulMaxCol*ulMaxCol);
		bFindOne = FALSE;
		for(i=0; i<ulMapNumOfReferDice; i++)
		{
			if( stReferDieInfo[i].m_bOnMap && stReferDieInfo[i].m_bOnWaf )
			{
				lBaseRow = stReferDieInfo[i].m_lAsmRow;
				lBaseCol = stReferDieInfo[i].m_lAsmCol;
				for(j=0; j<ulMapNumOfReferDice; j++)
				{
					lAsmRow = stReferDieInfo[j].m_lAsmRow;
					lAsmCol = stReferDieInfo[j].m_lAsmCol;
					if( bLowerDone==FALSE )
					{
						if( lAsmRow<lHomeRow )
						{
							continue;
						}
					}
					else
					{
						if( lAsmRow>=lHomeRow )
						{
							continue;
						}
					}
					if( stReferDieInfo[j].m_bOnMap && stReferDieInfo[j].m_bOnWaf==FALSE )
					{
						ulDist = (lBaseRow-lAsmRow)*(lBaseRow-lAsmRow) + (lBaseCol-lAsmCol)*(lBaseCol-lAsmCol);
						if( ulDist<ulMinDist )
						{
							ulMinDist = ulDist;
							ulBaseIndex = i;
							ulDieIndex = j;
							bFindOne = TRUE;
						}
					}
				}
			}
		}

		if( pApp->IsStopAlign() )
		{
			SetAlignmentStatus(FALSE);
			HmiMessage_Red_Back("Find all refer die operation stopped, please align wafer again!");
			break;
		}

		if( bFindOne )
		{
			// move to target by base and do pr check
			lBaseRow = stReferDieInfo[ulBaseIndex].m_lAsmRow;
			lBaseCol = stReferDieInfo[ulBaseIndex].m_lAsmCol;
			lBaseWfX = stReferDieInfo[ulBaseIndex].m_lWftPosX;
			lBaseWfY = stReferDieInfo[ulBaseIndex].m_lWftPosY;
			lAsmRow  = stReferDieInfo[ulDieIndex].m_lAsmRow;
			lAsmCol  = stReferDieInfo[ulDieIndex].m_lAsmCol;
			LONG lHmiRow = 0, lHmiCol = 0;
			ConvertAsmToOrgUser(lAsmRow, lAsmCol, lHmiRow, lHmiCol);
			lDiffX = lAsmCol - lBaseCol;
			lDiffY = lAsmRow - lBaseRow;
			lWfX = lBaseWfX - (lDiffX) * lDiePitchX_X - (lDiffY) * lDiePitchY_X;
			lWfY = lBaseWfY - (lDiffY) * lDiePitchY_Y - (lDiffX) * lDiePitchX_Y;
			m_WaferMapWrapper.SetCurrentPosition(lAsmRow, lAsmCol);
			if( XY_SafeMoveTo(lWfX, lWfY)==FALSE )
			{
				stReferDieInfo[ulDieIndex].m_bOnMap = FALSE;
				szMsg.Format("Index %d, move to error and die skipped, Wft(%d,%d)",
					ulDieIndex+1, lWfX, lWfY);
				WT_SpecialLog(szMsg);
				szMsg.Format("Refer die move to do PR check MOTION error, please check!");
				HmiMessage_Red_Back(szMsg, "Wafer Alignment");
				WT_SpecialLog(szMsg);
				szMsg.Format("Refer die (%ld,%ld) move to do PR check MOTION error, please check!", lHmiRow, lHmiCol);
				WT_SpecialLog(szMsg);
				SetErrorMessage(szMsg);
				SetAlignmentStatus(FALSE);
				break;
			}
			X1_Sync();
			Y1_Sync();
			Sleep(100);
			//	sprial search die
			LONG lReferPRID = 0;
			if( bCheckAllReferDie )
				lReferPRID = 0;
			else
				lReferPRID = 1;
			if( pApp->GetCustomerName()=="Lumileds" )
			{
				if( lAsmRow==ulHomeDieRow && lAsmCol==ulHomeDieCol )
					lReferPRID = 1;
				else
					lReferPRID = 0;
			}

			BOOL bFindReferInWafer = SpiralSearchRegionReferDie(m_lAllReferSpiralLoop, lReferPRID);
			if( bFindReferInWafer )
			{
				if( pUtl->GetPrescanRegionMode() )
				{
					RectWaferAroundDieCheck();
					LONG lTotalFound = 0;	//	GetGoodTotalDie();
					LONG lTotalFOV = (LONG)(m_dPrescanLFSizeX)*(LONG)(m_dPrescanLFSizeY);
					if( (lTotalFound*4)<lTotalFOV )
					{
						szMsg.Format("Index %04d, not enough neighbour die", ulDieIndex+1);
						WT_SpecialLog(szMsg);
						szMsg.Format("Refer die (%ld,%ld) NOT FOUND, please check!", lHmiRow, lHmiCol);
						HmiMessage_Red_Back(szMsg, "Wafer Alignment");
						WT_SpecialLog(szMsg);
						SetAlignmentStatus(FALSE);
						break;
					}
				}

				LONG	lT;
				GetEncoder(&lWfX, &lWfY, &lT);
				stReferDieInfo[ulDieIndex].m_bOnWaf = TRUE;
				stReferDieInfo[ulDieIndex].m_lWftPosX = lWfX;
				stReferDieInfo[ulDieIndex].m_lWftPosY = lWfY;
				szMsg.Format("find %02lu, %4ld,%4ld (%4ld,%4ld) %ld,%ld\n",
					ulDieIndex+1, lAsmRow, lAsmCol, lHmiRow, lHmiCol, lWfX, lWfY);
				WT_SpecialLog(szMsg);
			}
			else
			{
				stReferDieInfo[ulDieIndex].m_bOnMap = FALSE;
				szMsg.Format("Index %04d, sprial search die error and skipped", ulDieIndex+1);
				WT_SpecialLog(szMsg);
				szMsg.Format("Refer die (%ld,%ld) NOT FOUND, please check!\n", lHmiRow, lHmiCol);
				SetErrorMessage(szMsg);
				WT_SpecialLog(szMsg);
			}
		}
		else
		{
			if( bLowerDone==FALSE )
			{
				bLowerDone = TRUE;
			}
			else
			{
				break;
			}
		}
	}

	WT_SpecialLog("\n");

	ULONG ulFoundRefers = 0;
	pUtl->RemoveAllReferPoints();	// after wafer alignment to update values
	for(i=0; i<ulMapNumOfReferDice; i++)
	{
		if( stReferDieInfo[i].m_bOnMap && stReferDieInfo[i].m_bOnWaf )
		{
			lAsmRow = stReferDieInfo[i].m_lAsmRow;
			lAsmCol = stReferDieInfo[i].m_lAsmCol;
			if( (ULONG)lAsmRow >= ulMaxRow || (ULONG)lAsmCol >= ulMaxCol)
			{
				continue;
			}

			lWfX = stReferDieInfo[i].m_lWftPosX;
			lWfY = stReferDieInfo[i].m_lWftPosY;
			pUtl->AddAllReferPosition(lAsmRow, lAsmCol, lWfX, lWfY);

			ulFoundRefers++;
			ConvertAsmToOrgUser(lAsmRow, lAsmCol, lOrgRow, lOrgCol);
			szMsg.Format("list %2lu(%2ld), %4ld,%4ld (%4ld,%4ld), Wft(%d,%d)",
				ulFoundRefers, i+1, lAsmRow, lAsmCol, lOrgRow, lOrgCol, lWfX, lWfY);
			WT_SpecialLog(szMsg);
		}
	}

	delete[] astReferList;

	XY_SafeMoveTo(lHomeWfX, lHomeWfY);
	m_WaferMapWrapper.SetCurrentPosition(lHomeRow, lHomeCol);

	if( pUtl->GetPrescanRegionMode() )
	{
		if( ulFoundRefers<=0 )
		{
			pSRInfo->SetCurrentRegionState_HMI("M");
			szMsg = "Region scan has no reference die in wafer, please manual align region to sort.";
			HmiMessage_Red_Back(szMsg, "Prescan");
			WT_SpecialLog(szMsg);
		}
		else
		{
			pSRInfo->SetCurrentRegionState_HMI("A");
			if( IsWLAutoMode()==FALSE )
			{
				szMsg = "Region scan has reference die, please sort in auto mode";
				WT_SpecialLog(szMsg);
			}
		}
	}

	return (ulFoundRefers>0);
#endif
}

LONG CWaferTable::MS90Set2ndHomePosition(IPC_CServiceMessage& svMsg)
{
	BOOL bToSet = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bToSet);

	LONG  lHomeWftT = 0 - m_lThetaMotorDirection * (LONG)(180.0/m_dThetaRes);
	INT nPos = GetAutoWaferT();
	GetEncoderValue();
	LONG lEncT = GetCurrT();
	if( bToSet==FALSE )
	{
		nPos = nPos + lHomeWftT;
	}
	else
	{
		if( IsMS90Sorting2ndPart() )
		{
			nPos = nPos + lHomeWftT;
		}
	}
	if( fabs((lEncT-nPos)*m_dThetaRes)>20.0 )
	{
		XY_SafeMoveTo(0, 0);
		T_MoveTo(nPos, SFM_WAIT);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

INT  CWaferTable::GetAutoWaferT()
{
	INT nPos = (INT)((90.0 * m_ucAutoWaferRotation) / m_dThetaRes);
	//270 degree = -90 degree
	if ( CMS896AApp::m_bMS100Plus9InchOption && (m_ucAutoWaferRotation == 3) )	
	{
		nPos = (INT)((90 * -1) / m_dThetaRes);
	}
	else if (IsMS60() && (m_ucAutoWaferRotation == 3))		//v4.53A24	//EverVision MS60
	{
		nPos = (INT)((90 * -1) / m_dThetaRes);
	}

	return nPos;
}

//	by check current WFT Theta encoder to ideal wafer orientation in 1st or 2nd part.
LONG CWaferTable::GetWaferSortPart()
{
	INT nReturn = 0;
	INT nPos = GetAutoWaferT();

	X_Sync();
	Y_Sync();
	T_Sync();
	GetEncoderValue();
	LONG lEncT = GetCurrT();
	if( fabs((lEncT-nPos)*m_dThetaRes)<30.0 )
	{
		nReturn = 1;
	}

	LONG  lHomeWftT = 0 - m_lThetaMotorDirection * (LONG)(180.0/m_dThetaRes);
	nPos = nPos + lHomeWftT;
	if (fabs((lEncT - nPos) * m_dThetaRes) < 30)
	{
		nReturn = 2;
	}

	//v4.59A36
	CString szLog;
	szLog.Format("WT GetWaferSortPart: Rotate=%d, Res=%.5f, EncT = %ld (%ld), RESULT=%d",
			m_ucAutoWaferRotation, m_dThetaRes, lEncT, nPos, nReturn);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);			

	(*m_psmfSRam)["WaferTable"]["MS9oWaferSortPart"] = (LONG) nReturn;	//0=FAIL, 1=North, 2=South

	return nReturn;
}
