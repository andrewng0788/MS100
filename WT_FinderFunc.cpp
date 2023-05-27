#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "PrescanInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// update all phy positions in the image for corresponding die(row/col)
BOOL CWaferTable::LookAroundPosn(ULONG ulTgtRow, ULONG ulTgtCol, ULONG ulCtrRow, ULONG ulCtrCol, CONST BOOL bLatch)
{
	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchX_Y = GetDiePitchX_Y();
	LONG lDiePitchY_X = GetDiePitchY_X();
	LONG lDiePitchY_Y = GetDiePitchY_Y();

	m_WaferMapWrapper.SetCurrentPosition(ulTgtRow, ulTgtCol);

	LONG lCtrX, lCtrY, lCtrT;
	GetEncoder(&lCtrX, &lCtrY, &lCtrT);

	(*m_psmfSRam)["WaferTable"]["Current"]["X"] = lCtrX;
	(*m_psmfSRam)["WaferTable"]["Current"]["Y"] = lCtrY;

	IPC_CServiceMessage stMsg;
	BOOL bDrawDie = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bDrawDie);
	int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MultiSearchNmlDie1", stMsg);
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

	WftCheckCurrentGoodDie();

	LONG lOffsetX = 0, lOffsetY = 0;
	LONG lDieX = 0, lDieY = 0;
	DOUBLE dDieAngle;
	CString szDieBin = "0";

	ULONG ulRowIdx=0, ulColIdx=0, ulStartRow=0, ulStartCol=0, ulIdx=0;		//Klocwork
	LONG  lDiff_X, lDiff_Y;
	BOOL bFound = FALSE;

	if( ulCtrRow>20 )
		ulStartRow = ulCtrRow-20;
	else
		ulStartRow = 0;
	if( ulCtrCol>20 )
		ulStartCol = ulCtrCol-20;
	else
		ulStartCol = 0;

	for(ulIdx=1; ulIdx<=GetGoodTotalDie(); ulIdx++)
	{
		if( GetGoodPosnBin(ulIdx, lOffsetX, lOffsetY, dDieAngle, szDieBin) )
		{
			bFound = FALSE;
			for(ulRowIdx=ulStartRow; ulRowIdx<=ulCtrRow+20; ulRowIdx++)
			{
				for(ulColIdx=ulStartCol; ulColIdx<=ulCtrCol+20; ulColIdx++)
				{
					lDiff_X = ulColIdx - ulCtrCol;
					lDiff_Y = ulRowIdx - ulCtrRow;
					if( abs(  lDiff_X * lDiePitchX_X + lDiff_Y * lDiePitchY_X + lOffsetX)<abs(lDiePitchX_X/2) &&
						abs(  lDiff_Y * lDiePitchY_Y + lDiff_X * lDiePitchX_Y + lOffsetY)<abs(lDiePitchY_Y/2) )
					{
						bFound = TRUE;
						break;
					}
				}
				if( bFound )
					break;
			}
			if( bFound == TRUE )	//Klocwork
			{
				lDieX	= lCtrX + lOffsetX;	
				lDieY	= lCtrY + lOffsetY;
				if (m_pWaferMapManager->IsMapHaveBin(ulRowIdx, ulColIdx))
				{
					m_WaferMapWrapper.SetPhysicalPosition(ulRowIdx, ulColIdx, lDieX, lDieY);
					WSSetPrescanPosition(lDieX, lDieY, dDieAngle, szDieBin, 1);
				}
			}
		}
	}

	//If OK, then perform PR compensation if necessary
	LONG lCurrX, lCurrY;
	if( GetDieValidPrescanPosn(ulTgtRow, ulTgtCol, 3, lCurrX, lCurrY) )
	{
	}
	else
	{
		LONG	lDiff_X = 0, lDiff_Y = 0;
	
		lDiff_X	= ulTgtCol - ulCtrCol;
		lDiff_Y	= ulTgtRow - ulCtrRow;

		lCurrX = lCtrX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lCurrY = lCtrY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
	}

	BOOL bDieResult = FALSE;
	if (XY_SafeMoveTo(lCurrX, lCurrY))
	{
		bDieResult = TRUE;
	}
	m_WaferMapWrapper.SetPhysicalPosition(ulTgtRow, ulTgtCol, lCurrX, lCurrY);
	WftCheckCurrentGoodDie();

	return bDieResult;
}

//================================================================
// AUTOBOND Support Functions for 1st Die Finder algorithm to locate UL die position
//================================================================
// before call this, make sure the table and map is at the same and should at home die position
INT CWaferTable::OpStart1stDieFinderToPick()
{
	CString szAlgorithm;
	CString szPathFinder;
	m_WaferMapWrapper.GetAlgorithm(szAlgorithm, szPathFinder);

	if( (szAlgorithm == "Block Algorithm (without Ref Die)")	||
		(szAlgorithm.Find("TLH") != -1)						||
		(szAlgorithm.Find("TLV") != -1)						)
	{
		ULONG ulMaxRow = 0, ulMaxCol = 0;
		m_pWaferMapManager->GetWaferMapDimension(ulMaxRow, ulMaxCol);
		ULONG lULRow, lULCol, lLRRow, lLRCol;
		lULRow = 0;
		lULCol = 0;
		lLRRow = ulMaxRow;
		lLRCol = ulMaxCol;

		m_p1stDieFinder->SetTargetSubRegion(lULRow, lULCol, lLRRow, lLRCol);
	}
	else
	{
		return gnNOTOK;
	}

	ULONG ulRow = 0, ulCol = 0;
	m_WaferMapWrapper.GetStartPosition(ulRow, ulCol);
	GetEncoderValue();
	m_WaferMapWrapper.SetPhysicalPosition(ulRow, ulCol, GetCurrX(), GetCurrY());

	//Init 1st-Die Finder object
	if (!m_p1stDieFinder->StartFinder(ulRow, ulCol, m_dPrescanLFSizeX, m_dPrescanLFSizeY))
	{
		return gnNOTOK;
	}
	m_WaferMapWrapper.ResetGrade();

	m_WaferMapWrapper.StartMap();

	return gnOK;
}


INT CWaferTable::OpCycle1stDieFinderToPick()
{
	CString szMsg;
	BOOL bConfirmToPick = FALSE;

	INT nFinderResult = m_p1stDieFinder->FindNextDie();

	switch (nFinderResult)
	{
	//Continue
	case ERR_1DF__OK:
		return gnOK;


	//Target found and END
	case ERR_1DF__DONE:	
		bConfirmToPick = TRUE;
		break;


	case ERR_1DF__WAFEREND:
	default:
		szMsg.Format("WT::1stDieFinder: Wafer-End; can not find starting point!");
		AfxMessageBox(szMsg, MB_SYSTEMMODAL);
		break;
	}

	INT nResult = gnNOTOK;
	if( bConfirmToPick )
	{
		Sleep(500);
		ULONG ulLastRow=0, ulLastCol=0;
		LONG lPosnX=0, lPosnY=0;

		m_p1stDieFinder->GetLastPosn(ulLastRow, ulLastCol);	// auto mode
		if (GetMapPhyPosn(ulLastRow, ulLastCol, lPosnX, lPosnY))
		{
			OpAbort1stDieFinderToPick();

			XY_SafeMoveTo(lPosnX, lPosnY);
			Sleep(20);
			WftCheckCurrentGoodDie();

			OpFinish1stDieFinder(ulLastRow, ulLastCol);

			m_qSubOperation	= WAIT_DIE_READY_Q;
			SetWTStable(TRUE);
			m_bEnableSWalk1stDieFinder = FALSE;
			nResult = gnOK;
		}
		else
		{
			m_WaferMapWrapper.StopMap();
		}
	}

	return nResult;
}

INT	CWaferTable::OpFinish1stDieFinder(ULONG ulMapRow, ULONG ulMapCol)
{
	m_WaferMapWrapper.SetCurrentPosition(ulMapRow, ulMapCol);
	m_WaferMapWrapper.SetStartPosition(ulMapRow, ulMapCol);
	m_WaferMapWrapper.EnableAutoAlign(TRUE);	//Start bonding at current START position
	GetEncoderValue();
	m_lStart_X	= GetCurrX();
	m_lStart_Y	= GetCurrY();
	m_lStart_T	= GetCurrT();

	return gnOK;
}

INT CWaferTable::OpAbort1stDieFinderToPick()
{
	if( m_p1stDieFinder->IsInit() )
	{
		m_WaferMapWrapper.StopMap();
		m_p1stDieFinder->StopFinder();
		WftCheckCurrentGoodDie();
	}
	return gnOK;
}
