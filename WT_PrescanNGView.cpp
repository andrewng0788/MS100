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
#include "WAF_CPhysicalInformation.h"
//#include "Encryption.h"
//#include "spswitch.h"		//v4.47T5

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

INT	CWaferTable::OpGetWaferPos_AreaPick()
{
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szMoveMsg;
	BOOL bFindNewDie = FALSE;
	LONG lDiePosX = 0, lDiePosY = 0;
	DOUBLE dDieAngle = 0;
	// search in good list to find a die to pick, need check picked or overlapped

	while( 1 )
	{
		if( m_nPickListIndex<1 )
			break;
		bFindNewDie = GetGoodPosnOnly(m_nPickListIndex, lDiePosX, lDiePosY, dDieAngle);
		if( bFindNewDie )
		{
			break;
		}
		m_nPickListIndex--;
	}

	if( bFindNewDie==FALSE )
	{
		szMoveMsg.Format("Get Area no die left %d, continue prescan", m_nPickListIndex);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);

		SetWTStartMove(FALSE);
		SetWTReady(FALSE, "false2");	//Allow BH Z1/2 to move down earilier because no need to wait for compensation  
		SetDieReadyForBT(TRUE);
		SetPRStart(FALSE);	// trigger bond arm to pick
		T_MoveTo(GetGlobalT());
		Sleep(100);

		return gnNOTOK;
	}

	LONG lMoveWfX = lDiePosX;
	LONG lMoveWfY = lDiePosY;

	X_Sync();
	Y_Sync();
	T_Sync();
	GetEncoderValue();
	LONG lCurrT = GetCurrT();

	LONG lMoveWfT = 0;
	lMoveWfT = GetGlobalT() - lCurrT;
	BOOL bThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableThetaCorrection"];
	if ( bThetaCorrection==FALSE )
	{
		dDieAngle = 0;
	}

	if( fabs(dDieAngle)>0 )
	{
		LONG lRottWfT = 0;
		RotateWaferTheta(&lMoveWfX, &lMoveWfY, &lRottWfT, dDieAngle);
		lMoveWfT = lMoveWfT + lRottWfT;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	LONG lOldWfT = lMoveWfT;
	BOOL bConfirmSearch = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["CFS"]["ConfirmSearch"];
	(*m_psmfSRam)["WaferTable"]["ConfirmSearch"]	= FALSE;
	if( bConfirmSearch )
	{
		LONG lMinConfirmT = labs((LONG)(*m_psmfSRam)["WaferPr"]["ThetaMiniumRotation"] * m_lMC[WTMC_AP_C_MIN_T] / 100);
		if( lMinConfirmT<3 )
			lMinConfirmT = 3;
		if( labs(lMoveWfT)>lMinConfirmT )
			(*m_psmfSRam)["WaferTable"]["ConfirmSearch"]	= TRUE;
	}

	LONG lMinRotateT = labs((LONG)(*m_psmfSRam)["WaferPr"]["ThetaMiniumRotation"] * m_lMC[WTMC_AP_R_MIN_T] / 100);
	if( lMinRotateT<3 )
		lMinRotateT = 3;
	if( labs(lMoveWfT)<=lMinRotateT )
	{
		DOUBLE dNewAngle = 0;
		if( m_lThetaMotorDirection!=0 )
			dNewAngle = (DOUBLE)lMoveWfT*m_dThetaRes/m_lThetaMotorDirection;
		LONG lRottWfT = 0;
		RotateWaferTheta(&lMoveWfX, &lMoveWfY, &lRottWfT, dNewAngle);
		lMoveWfT = 0;
	}

	m_lAreaPickDiePosnX = lMoveWfX;
	m_lAreaPickDiePosnY = lMoveWfY;
	m_lAreaPickDieMoveT = lMoveWfT;

	szMoveMsg.Format("Get Area %d,(%.2f); move(%d,%d),T(%d ==> %d), From(%d) Scan(%d,%d)", 
		m_nPickListIndex, dDieAngle, lMoveWfX, lMoveWfY, lOldWfT, lMoveWfT, lCurrT, lDiePosX, lDiePosY);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);

	return gnOK;
}

INT	CWaferTable::OpMoveTable_AreaPick()
{
	LONG lMoveWfX = m_lAreaPickDiePosnX;
	LONG lMoveWfY = m_lAreaPickDiePosnY;
	LONG lMoveWfT = m_lAreaPickDieMoveT;

	LONG lDistX = lMoveWfX - GetCurrX();
	LONG lDistY = lMoveWfY - GetCurrY();
	LONG lTime_X = X_ProfileTime(NORMAL_PROF, lDistX, lDistX);
	LONG lTime_Y = Y_ProfileTime(NORMAL_PROF, lDistY, lDistY);

	TakeTime(EV1);	// into move, calculate and change profile
	SetEjectorVacuum(FALSE);		// Release the vacuum
	//v4.31T10	//VAC off delay re-added for Excelitas
//	LONG lVacOffDelay = (*m_psmfSRam)["WaferTable"]["VacOffDelay"];
//	if ( (lVacOffDelay > 0) && (lVacOffDelay < 10000) )
//		Sleep(lVacOffDelay);
#ifdef NU_MOTION
	if( labs(lDistX) > GetSlowProfileDist() )
	{
		X_Profile(LOW_PROF);
		lTime_X = X_ProfileTime(LOW_PROF, lDistX, lDistX);
	}
	else
	{
		X_Profile(NORMAL_PROF);
	}	

	if (labs(lDistY) > GetSlowProfileDist())
	{
		Y_Profile(LOW_PROF);
		lTime_Y = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
	}
	else
	{
		Y_Profile(NORMAL_PROF);
	}
#else
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	LONG lTime_T = 0;
	BOOL bRotate = FALSE;
	LONG lTDelay = (*m_psmfSRam)["WaferTable"]["TDelay"];
	LONG lExtraTSleepDelay = (LONG) (fabs((DOUBLE)(lMoveWfT) * m_dThetaRes) * lTDelay);
	if ( (abs(lMoveWfT) >= 3) && m_bSel_T )
	{
		lTime_T = T_ProfileTime(NORMAL_PROF, lMoveWfT, lMoveWfT) + lExtraTSleepDelay;
		bRotate = TRUE;
	}


	LONG lIndexTime = lTime_X;
	if( lTime_Y > lIndexTime )
		lIndexTime = lTime_Y;
	if( (lTime_T>lIndexTime) )
		lIndexTime = lTime_T;

	(*m_psmfSRam)["WaferTable"]["IndexTime"]				= 10;
	(*m_psmfSRam)["WaferTable"]["IndexStart"]				= GetTime();
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"]		= 199;
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"]		= 199;
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"]		= m_WaferMapWrapper.GetGradeOffset() + m_ucDummyPrescanPNPGrade;
	(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"]	= WAF_CDieSelectionAlgorithm::PICK;


	TakeTime(WT1);
	SetWTStartMove(TRUE);
	X_MoveTo(lMoveWfX, SFM_NOWAIT);
	Y_MoveTo(lMoveWfY, SFM_NOWAIT);
	if( bRotate )
	{
		T_Move(lMoveWfT, SFM_NOWAIT);
	}

	CString szMoveMsg;
	szMoveMsg.Format("Get MOVE %d,%d,T%d rotate is %d", lMoveWfX, lMoveWfY, lMoveWfT, bRotate);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);

	X_Sync();
	Y_Sync();
	TakeTime(EV2);	// XY complete

	if( bRotate )
	{
		T_Sync();
		Sleep(lExtraTSleepDelay);	//wait for theta stable (1 degree = 15ms by default)
	}
	TakeTime(CP1);

	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lMoveWfX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lMoveWfY;
	(*m_psmfSRam)["WaferTable"]["PickDie"] = (LONG)TRUE;

#ifdef NU_MOTION
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
#endif

	BOOL bConfirmSrch = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["ConfirmSearch"];
	if( bConfirmSrch )
	{
		LONG lDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];
		if( lDelay>0 )
		{
			Sleep(lDelay);
		}
		LONG lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
		INT nPrePickCount = 0;
		while (lBHAtPrePick != 1)
		{
			Sleep(10);
			lBHAtPrePick = (LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];		
			nPrePickCount++;
			if (nPrePickCount >= 100)
				break;
		}
		WftSearchNormalDie(lMoveWfX, lMoveWfY, TRUE);
		szMoveMsg.Format("AP Confirm Srch %d,%d", lMoveWfX, lMoveWfY);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMoveMsg);
	}
	SetWTStable(TRUE);
	SetWTReady(TRUE, "true3");	//	Allow BH Z1/2 to move down earilier because no need to wait for compensation  

	TakeTime(WT2);		//	Take Time
	SetEjectorVacuum(TRUE);

	//	begin PR delay
	LONG lDelay = (*m_psmfSRam)["WaferTable"]["PRDelay"];
#ifdef NU_MOTION
	if ( (lDelay < 20) && (CMS896AApp::m_lCycleSpeedMode < 4))
	{
		lDelay = lDelay + 5;
	}

	//	Added extra 10ms PR delay for long-travel motion
	if (CMS896AApp::m_lCycleSpeedMode >= 4)	
	{
		if ( (m_lLongJumpMotTime >= 30) && (lIndexTime >= m_lLongJumpMotTime) )
		{
			lDelay = lDelay + m_lLongJumpDelay - 3;
		}
	}
#endif
	if( lDelay>0 )
	{
		Sleep(lDelay);
		DisplaySequence("WFT - scan AP pr delay");
	}
	SetPRStart(TRUE);	// trigger bond arm to pick of 899.
	TakeTime(CP2);

	return gnOK;
}

INT CWaferTable::OpGetWaferPos_PrescanAreaPick(LONG lX, LONG lY)
{
	INT nReturn = 1;
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	BOOL bChangeLine = FALSE;
	while( 1 )
	{
		lX += GetPrescanPitchX()*m_lAreaPickScanDirection;
		if( IsPosnWithinMapRange(lX, lY) && IsWithinWaferLimit(lX, lY) )
		{
			break;
		}
		if( (m_lAreaPickScanDirection==-1 && lX<GetScanCtrX()) ||
			(m_lAreaPickScanDirection==1  && lX>GetScanCtrX()) )
		{
			bChangeLine = TRUE;
			break;
		}
	}

	// Check whether the found position is within wafer limit
	if( bChangeLine )
	{
		while( 1 )
		{
			lY = lY - GetPrescanPitchY();
			lX = m_lAreaPickScanUL_X;
			while( 1 )
			{
				if( IsPosnWithinMapRange(lX, lY) && IsWithinWaferLimit(lX, lY) )
				{
					nReturn = 0;
					break;
				}
				lX -= GetPrescanPitchX();
				if( lX<GetScanCtrX() && lY<GetScanCtrY() )
				{
					nReturn = -1;
					break;
				}
			}
			if( nReturn==0 || nReturn==-1 )
			{
				break;
			}
		}
	}

	if( nReturn!=-1 )
	{
		m_nPrescanNextWftPosnY = lY;
		m_nPrescanNextWftPosnX = lX;
		if( GetDiePitchX_X()!=0 || GetDiePitchY_Y()!=0 )
		{
			LONG lScanRow = (GetPrescanAlignPosnY() - lY)/GetDiePitchY_Y() + m_nPrescanAlignMapRow;
			LONG lScanCol = (GetPrescanAlignPosnX() - lX)/GetDiePitchX_X() + m_nPrescanAlignMapCol;
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
			m_nPrescanNextMapRow = lScanRow;
			m_nPrescanNextMapCol = lScanCol;
			CString szMsg;
			szMsg.Format("wt cal, %5ld,%5ld,%5ld,%5ld,%8ld,%8ld", 
				lScanRow, lScanCol, lScanRow, lScanCol, lX, lY);
			pUtl->PrescanMoveLog(szMsg);
		}
	}

	return nReturn;
}


BOOL CWaferTable::ConstructAreaPickMap()
{
#define	EXTRA_DUMMY_EDGE	20
#define	DUMMY_SCAN2_LOOP	5

	WAF_CPhysicalInformationMap cAPMap;
	BOOL bDieOverLap = FALSE;

	ULONG ulTotalGoodDie = GetGoodTotalDie();
	CString szMsg;
	szMsg.Format("AP scan total die %d", ulTotalGoodDie);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);

	if( ulTotalGoodDie==0 )
	{
		m_nPickListIndex = -1;
		return FALSE;
	}
	if( ulTotalGoodDie==1 )
	{
		m_nPickListIndex = 1;
		szMsg.Format("AP pick total die %d", m_nPickListIndex);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		return TRUE;
	}

	LONG lEncX = 0, lEncY = 0;
	LONG ulDiePitchX = labs(GetDiePitchX_X());
	LONG ulDiePitchY = labs(GetDiePitchY_Y());
	ULONG ulDieSizeXTol = (ULONG) (ulDiePitchX*30/100.0);
	ULONG ulDieSizeYTol = (ULONG) (ulDiePitchY*30/100.0);


	// build physical map
	int nMaxSpan = 10;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	int nMaxIterateCount = -1;
	char *pucFindOut;
	DOUBLE *pdDieAngle;
	CDWordArray dwaDm2WfX, dwaDm2WfY, dwaDm2Row, dwaDm2Col;
	ULONG ulAPPyiRowMax = 0, ulAPPyiColMax = 0;
	LONG lApAlignWfX = 0, lApAlignWfY = 0, lApAlignRow = 30, lApAlignCol = 500;
	DOUBLE dDieAngle = 0;

	cAPMap.RemoveAll();
	cAPMap.KeepIsolatedDice(FALSE);

	GetGoodPosnOnly(ulTotalGoodDie/2, lApAlignWfX, lApAlignWfY, dDieAngle);
	// add all prescan die data list to physical map builder include refer points

	pucFindOut = new char[ulTotalGoodDie+10];
	pdDieAngle = new DOUBLE[ulTotalGoodDie+10];

	for(ULONG ulIndex=1; ulIndex<=ulTotalGoodDie; ulIndex++)
	{
		pucFindOut[ulIndex] = 0;
		pdDieAngle[ulIndex] = 0.0;
		if( GetGoodPosnOnly(ulIndex, lEncX, lEncY, dDieAngle) )
		{
			pdDieAngle[ulIndex] = dDieAngle;
			cAPMap.SortAdd_Tail(lEncX, lEncY, ulIndex, bDieOverLap);
		}
	}

	dwaDm2WfX.RemoveAll();
	dwaDm2WfY.RemoveAll();
	dwaDm2Row.RemoveAll();
	dwaDm2Col.RemoveAll();
	dwaDm2WfX.Add(lApAlignWfX);
	dwaDm2WfY.Add(lApAlignWfY);
	dwaDm2Row.Add(lApAlignRow);
	dwaDm2Col.Add(lApAlignCol);

	cAPMap.ConstructMap(dwaDm2WfX, dwaDm2WfY, dwaDm2Row, dwaDm2Col,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
		bPreSorted, nMaxIterateCount, 10, nMaxSpan);

	ClearPrescanInfo();

	cAPMap.GetDimension(ulAPPyiRowMax, ulAPPyiColMax);

	for(ULONG ulRow=0; ulRow<=ulAPPyiRowMax; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<=ulAPPyiColMax; ulCol++)
		{
			LONG lIndex = 0;
			if( cAPMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap)==FALSE )
			{
				continue;
			}
			if( lIndex>=0 )
			{
				pucFindOut[lIndex] = 1;
				SetPrescanPosition(ulRow, ulCol, lEncX, lEncY);
				SetPrescanFrameInfo(ulRow, ulCol, lIndex);
			}
		}
	}

	for(int i=1; i<4; i++)
	{
		for(ULONG ulIndex=1; ulIndex<=ulTotalGoodDie; ulIndex++)
		{
			if( pucFindOut[ulIndex]>0 )
			{
				continue;
			}

			if( GetGoodPosnOnly(ulIndex, lEncX, lEncY, dDieAngle)==FALSE )
			{
				continue;
			}

			LONG lNewRow, lNewCol, lNewX, lNewY;
			lNewCol = (lApAlignWfX - lEncX)/ulDiePitchX + lApAlignCol;
			lNewRow = (lApAlignWfY - lEncY)/ulDiePitchY + lApAlignRow;
			if( CheckDieAssignedAround(lNewRow, lNewCol, DUMMY_SCAN2_LOOP, lEncX, lEncY, FALSE) )
			{
				pucFindOut[ulIndex] = 2;
				continue;
			}	// already assigned nearby, skip

			if( GetDieValidPrescanPosn(lNewRow, lNewCol, DUMMY_SCAN2_LOOP, lNewX, lNewY, FALSE) )
			{
				lNewCol = (lNewX - lEncX)/ulDiePitchX + lNewCol;
				lNewRow = (lNewY - lEncY)/ulDiePitchY + lNewRow;
				if( CheckDieAssignedAround(lNewRow, lNewCol, DUMMY_SCAN2_LOOP, lEncX, lEncY, FALSE) )
				{
					pucFindOut[ulIndex] = 2;
					continue;
				} // assigned nearby continue
			}

			if( i==1 )
			{
				if( GetDieValidPrescanPosn(lNewRow, lNewCol, 0, lNewX, lNewY, FALSE) )
				{
					continue;
				}
			}
			else
			{
				if( GetDieInvalidNearMapPosn(lNewRow, lNewCol, DUMMY_SCAN2_LOOP*i, lNewRow, lNewCol, FALSE)==FALSE )
				{
					continue;
				}
			}

			SetPrescanPosition(lNewRow, lNewCol, lEncX, lEncY);
			SetPrescanFrameInfo(lNewRow, lNewCol, ulIndex);
			pucFindOut[ulIndex] = 3;
			if( lNewRow>(LONG)ulAPPyiRowMax )
				ulAPPyiRowMax = lNewRow;
			if( lNewCol>(LONG)ulAPPyiColMax )
				ulAPPyiColMax = lNewCol;
		}
	}

	cAPMap.RemoveAll();
	dwaDm2WfX.RemoveAll();
	dwaDm2WfY.RemoveAll();
	dwaDm2Row.RemoveAll();
	dwaDm2Col.RemoveAll();

	// build prescan map complete
	ClearGoodInfo();
	for(ULONG nCol=0; nCol<=ulAPPyiColMax; nCol++)
	{
		ULONG ulIndex = 0;
		INT nRow = 0;
		if( nCol%2==0 )
			nRow = 0;
		else
			nRow = ulAPPyiRowMax;
		while( 1 )
		{
			if( GetPrescanPosition(nRow, nCol, lEncX, lEncY) &&
				GetPrescanFrameInfo(nRow, nCol, ulIndex) )
			{
				dDieAngle = pdDieAngle[ulIndex];
				SetGoodPosnOnly(lEncX, lEncY, dDieAngle);
			}
			if( nCol%2==0 )
			{
				nRow++;
				if( nRow>(LONG)ulAPPyiRowMax )
					break;
			}
			else
			{
				nRow--;
				if( nRow<0 )
					break;
			}
		}
	}

	DelPrescanFrameInfo();
	m_nPickListIndex = GetGoodTotalDie();

	delete [] pucFindOut;
	delete [] pdDieAngle;

	szMsg.Format("AP pick total die %d", m_nPickListIndex);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	for(ULONG ulIndex=1; ulIndex<=(ULONG)m_nPickListIndex; ulIndex++)
	{
		if( GetGoodPosnOnly(ulIndex, lEncX, lEncY, dDieAngle) )
		{
			szMsg.Format("AP list %d %4d,%d(%.2f)", ulIndex, lEncX, lEncY, dDieAngle);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
		}
	}

	return TRUE;
}
