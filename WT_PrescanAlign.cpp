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
#include "WaferPr.h"
//#include "Encryption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CWaferTable::FindAndAlignAllReferDie()
{
	ULONG	i, j, ulDieIndex = 0, ulBaseIndex = 0;
	LONG	lAsmRow, lAsmCol, lBaseRow, lBaseCol, lOrgRow = 0, lOrgCol = 0;
	LONG	lBaseWfX, lBaseWfY, lWfX, lWfY;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
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
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	if( ulMapNumOfReferDice<=0 )
	{
		if( IsScanAlignWafer()==FALSE )
		{
			if (pUtl->GetPrescanRegionMode() && pSRInfo->GetTotalSubRegions() > 1)
			{
				HmiMessage_Red_Back("Region scan has no reference die in map, please manual align region to sort.", "Prescan");
				pSRInfo->SetCurrentRegionState_HMI("M");
			}
			return FALSE;
		}
	}

	BOOL bVirtualRefer = FALSE;
	if( IsScanAlignWafer() && pUtl->GetPrescanRegionMode() && pSRInfo->GetTotalSubRegions()>1 )
	{
		ulMapNumOfReferDice = pSRInfo->GetTotalSubRegions();
		bVirtualRefer = TRUE;
	}

	if( ulMapNumOfReferDice>=MAXFDC2NUM )
	{
		ulMapNumOfReferDice = MAXFDC2NUM-1;
	}

	astReferList  = new WAF_CMapDieInformation[ulMapNumOfReferDice];

	szMsg.Format("Map get num of refer die %d", ulMapNumOfReferDice);
	WT_SpecialLog(szMsg);

	if( bVirtualRefer==FALSE )
		m_WaferMapWrapper.GetReferenceDieList(astReferList, ulMapNumOfReferDice);
	else
	{
		for(int i=0; i < (LONG)pSRInfo->GetTotalSubRegions(); i++)
		{
			ULONG ulRegionNo = pSRInfo->GetSortRegion(i);
			if( pSRInfo->IsWithinThisRegion(ulRegionNo, lHomeRow, lHomeCol) )
			{
				astReferList[i].SetRow(lHomeRow);
				astReferList[i].SetColumn(lHomeCol);
			}
			else
			{
				ULONG lULRow = lHomeRow, lULCol = lHomeCol, lLRRow = lHomeRow, lLRCol = lHomeCol;
				if( pSRInfo->GetRegion(ulRegionNo, lULRow, lULCol, lLRRow, lLRCol) )
				{
					astReferList[i].SetRow( (lULRow+lLRRow)/2 );
					astReferList[i].SetColumn( (lULCol+lLRCol)/2 );
				}
			}
		}
	}

	WT_SpecialLog("All Reference Die list In Map");
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
				if( pUtl->GetPrescanRegionMode() && pApp->GetCustomerName()!="Lumileds" )
				{
					RectWaferAroundDieCheck();
					LONG lTotalFound = GetGoodTotalDie();
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

				if(pApp->GetCustomerName() == "Macom")
				{
					szMsg = szMsg +"\n Do you want to bypasss ? "; // 4.52D7
					if(HmiMessage_Red_Back(szMsg, "Search Refer Die", glHMI_MBX_YESNO)==glHMI_NO)
					{
						SetAlignmentStatus(FALSE);
						break;
					}
					
				}

				if( pApp->GetCustomerName() == "Lumileds" )
				{
					HmiMessage_Red_Back(szMsg, "Wafer Alignment");
					SetAlignmentStatus(FALSE);
					break;
				}
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

//Check region reference die

	}

	delete[] astReferList;

	XY_SafeMoveTo(lHomeWfX, lHomeWfY);
	m_WaferMapWrapper.SetCurrentPosition(lHomeRow, lHomeCol);

	if( pUtl->GetPrescanRegionMode() )
	{
		WT_CSubRegionsInfo* pSRInfo = WT_CSubRegionsInfo::Instance();
		if (ulFoundRefers <= 0)
		{
			pSRInfo->SetCurrentRegionState_HMI("M");
			szMsg = "Region scan has no reference die in wafer, please manual align region to sort.";
			HmiMessage_Red_Back(szMsg, "Prescan");
			WT_SpecialLog(szMsg);
		}
		else
		{
			pSRInfo->SetCurrentRegionState_HMI("A");
			if( IsWLAutoMode()==FALSE && IsMS90HalfSortMode()==FALSE )
			{
				szMsg = "Region scan has reference die, please sort in auto mode";
				HmiMessage_Red_Back(szMsg, "Prescan");
				WT_SpecialLog(szMsg);
			}
		}
	}

	return (ulFoundRefers>0);
}

BOOL CWaferTable::CreeAutoSearchMapLimit()
{
	CString szLogMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( pApp->GetCustomerName()!="Cree" )
	{
		m_bSearchWaferLimitDone = FALSE;
		return TRUE;
	}

	if( m_bSearchWaferLimitDone )
	{
		return TRUE;
	}

	if( m_lFindMapEdgeDieLimit<=0 )
	{
		return TRUE;
	}

	if( m_WaferMapWrapper.GetSelectedTotalDice()<(ULONG)m_lFindMapEdgeDieLimit )
	{
		szLogMsg.Format("Map selected total dice %ld, min limit %ld", 
			m_WaferMapWrapper.GetSelectedTotalDice(), m_lFindMapEdgeDieLimit);
		WT_SpecialLog(szLogMsg);
		return TRUE;
	}

	if( IsMotionCE() )
	{
		WT_SpecialLog("motion critical error");
		return FALSE;
	}

	LONG lCtrX = m_lAutoWaferCenterX;
	LONG lCtrY = m_lAutoWaferCenterY;
	LONG lRadius = m_lAutoWaferDiameter*8/20;
	LONG lPosX, lPosY, lOrigX, lOrigY;
	DOUBLE dAngle;

	MultiSrchInitNmlDie1();

	ChangeCameraToWafer();

	SetJoystickOn(FALSE);

	GetEncoderValue();
	lOrigX = GetCurrX();
	lOrigY = GetCurrY();

	m_lWaferTmpX[0] = 0;
	m_lWaferTmpY[0] = 0;
	m_lWaferTmpX[1] = 0;
	m_lWaferTmpY[1] = 0;
	m_lWaferTmpX[2] = 0;
	m_lWaferTmpY[2] = 0;

	// left side edge
	XY_SafeMoveTo(lCtrX, lCtrY);
	Sleep(50);
	dAngle = PI*2*WT_SRCH_EDGE_LFT/WT_SRCH_EDGE_MAX;
	lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
	lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
	if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
	{
	}
	else
	{
		lPosX = lCtrX + lRadius;
		lPosY = lCtrY;
		XY_SafeMoveTo(lPosX, lPosY);
	}
	Sleep(50);
	if( IsMotionCE() )
	{
		WT_SpecialLog("motion critical error");
		return FALSE;
	}
	SeearchWaferEdge(WT_SRCH_EDGE_LFT, m_lWaferTmpX[0], m_lWaferTmpY[0]);	// left
	WT_SpecialLog("left edge searched");

	// top side edge
	XY_SafeMoveTo(lCtrX, lCtrY);
	Sleep(50);
	dAngle = PI*2*WT_SRCH_EDGE_TOP/WT_SRCH_EDGE_MAX;
	lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
	lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
	if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
	{
	}
	else
	{
		lPosX = lCtrX;
		lPosY = lCtrY + lRadius;
		XY_SafeMoveTo(lPosX, lPosY);
	}
	Sleep(50);
	if( IsMotionCE() )
	{
		WT_SpecialLog("motion critical error");
		return FALSE;
	}
	SeearchWaferEdge(WT_SRCH_EDGE_TOP, m_lWaferTmpX[1], m_lWaferTmpY[1]);	// top
	WT_SpecialLog("top edge searched");

	// right side edge
	XY_SafeMoveTo(lCtrX, lCtrY);
	Sleep(50);
	dAngle = PI*2*WT_SRCH_EDGE_RGT/WT_SRCH_EDGE_MAX;
	lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
	lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
	if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
	{
	}
	else
	{
		lPosX = lCtrX - lRadius ;
		lPosY = lCtrY;
		XY_SafeMoveTo(lPosX, lPosY);
	}
	Sleep(50);
	if( IsMotionCE() )
	{
		WT_SpecialLog("motion critical error");
		return FALSE;
	}
	SeearchWaferEdge(WT_SRCH_EDGE_RGT, m_lWaferTmpX[2], m_lWaferTmpY[2]);	// right
	WT_SpecialLog("right edge searched");

	if( IsMotionCE() )
	{
		WT_SpecialLog("motion critical error");
		return FALSE;
	}
	// calculate the circular center and diameter
	LONG lSetSize = GetWaferDiameter();
	if( CalcWaferCircle() )
	{
		if( labs(lSetSize-m_lAutoWaferDiameter)>50 )
		{
			if( labs(lSetSize-m_lAutoWaferDiameter)>labs(30*lSetSize/100) )
				HmiMessage("auto search wafer limit larger 0.30 than before");
		}
	}
	else
	{
		XY_SafeMoveTo(lOrigX, lOrigY);
		Sleep(50);
		WT_SpecialLog("calculation error");
		return FALSE;
	}

	// verify the all other 5 corner to make sure cover enough
	int i;
	lCtrX = m_lAutoWaferCenterX;
	lCtrY = m_lAutoWaferCenterY;
	for(i=0; i<=WT_SRCH_EDGE_B_R; i++)
	{
		if( i==WT_SRCH_EDGE_RGT || i==WT_SRCH_EDGE_TOP || i==WT_SRCH_EDGE_LFT )
			continue;
		lRadius = m_lAutoWaferDiameter/2;
		XY_SafeMoveTo(lCtrX, lCtrY);
		Sleep(50);
		dAngle = PI*2*i/WT_SRCH_EDGE_MAX;
		lPosX = lCtrX - (LONG)(lRadius*cos(dAngle));
		lPosY = lCtrY + (LONG)(lRadius*sin(dAngle));
		if( XY_SafeMoveTo(lPosX, lPosY, WT_SRCH_EDGE_LMT) )
		{
		}
		else
		{
			continue;
		}
		Sleep(50);
		if( IsMotionCE() )
		{
			WT_SpecialLog("motion critical error");
			return FALSE;
		}
		SeearchWaferEdge(i, lPosX, lPosY);
		DOUBLE	dDistance = 0.0, dX, dY;
		dX = (double)(lPosX - lCtrX);
		dY = (double)(lPosY - lCtrY);
		dDistance = sqrt( dX*dX + dY*dY );
		if ( dDistance > ((DOUBLE)(lRadius) * WT_SRCH_EDGE_LMT) )
		{
			m_lAutoWaferDiameter = (LONG)(GetDistance(lCtrX, lCtrY, lPosX, lPosY)*2);
		}
		WT_SpecialLog("check all other 5 coners");
	}
	m_bSearchWaferLimitDone = TRUE;

	XY_SafeMoveTo(lOrigX, lOrigY);
	Sleep(50);
	WT_SpecialLog("search complete");

	return TRUE;
}


LONG CWaferTable::GetPrescanAlignPosnX()
{
	return m_nPrescanAlignPosnX;
}

LONG CWaferTable::GetPrescanAlignPosnY()
{
	return m_nPrescanAlignPosnY;
}

BOOL CWaferTable::FindCrossReferDieAlignWafer()
{
	LONG	lUpCount = 0, lUpFound = 0;
	LONG	lDnCount = 0, lDnFound = 0;
	LONG	lLtCount = 0, lLtFound = 0;
	LONG	lRtCount = 0, lRtFound = 0;

	m_ulCrossHomeDieRow = 0;
	m_ulCrossHomeDieCol = 0;
	ULONG ulMapNumOfReferDice = m_WaferMapWrapper.GetNumberOfReferenceDice();
	if( ulMapNumOfReferDice<=0 )
	{
		return TRUE;
	}
	ULONG ulNewHomeRow, ulNewHomeCol;
	GetMapAlignHomeDie(ulNewHomeRow, ulNewHomeCol);

	//-----------------------------
	DiePhyPos2	CurrTbl, HomeTbl;
	LONG	lResult;
	CString szMsg;

	MultiSrchInitNmlDie1();
	GetEncoderValue();
	HomeTbl.x = GetCurrX();
	HomeTbl.y = GetCurrY();
    CMS896AApp::m_bStopAlign = FALSE;

	szMsg.Format("Home die (%ld,%ld)", HomeTbl.x, HomeTbl.y);
	WT_SpecialLog(szMsg);
	//Move to UP & PR search on current die
	lUpCount = lUpFound = 0;
	CurrTbl = HomeTbl;
	while(1)
	{
		lResult = IndexAndSearchRefer(&CurrTbl, 0-m_lReferPitchRow, 0);	// up
		if( lResult>=2 )
		{
			break;
		}

		lUpCount++;
		if( lResult==1 )
		{
			lUpFound = lUpCount;
		}
	}		
	szMsg.Format("Up find refer die %ld", lUpFound);
	WT_SpecialLog(szMsg);

	//Move to LEFT & PR search on current die
	lLtCount = lLtFound = 0;
	CurrTbl = HomeTbl;
	while(1)
	{
		lResult = IndexAndSearchRefer(&CurrTbl, 0, 0-m_lReferPitchCol);	// left
		if( lResult>=2 )
		{
			break;
		}

		lLtCount++;
		if( lResult==1 )
		{
			lLtFound = lLtCount;
		}
	}			
	szMsg.Format("Left find refer die %ld", lLtFound);
	WT_SpecialLog(szMsg);

	//Move to DOWN & PR search on current die
	lDnCount = lDnFound = 0;
	CurrTbl = HomeTbl;
	while(1)
	{
		lResult = IndexAndSearchRefer(&CurrTbl, m_lReferPitchRow, 0);	// down
		if( lResult>=2 )
		{
			break;
		}

		lDnCount++;
		if( lResult==1 )
		{
			lDnFound = lDnCount;
		}
	}			
	szMsg.Format("Down find refer die %ld", lDnFound);
	WT_SpecialLog(szMsg);

	//Move to RIGHT & PR search on current die
	lRtCount = lRtFound = 0;
	CurrTbl = HomeTbl;
	while(1)
	{
		lResult = IndexAndSearchRefer(&CurrTbl, 0, m_lReferPitchCol);	// right
		if( lResult>=2 )
		{
			break;
		}

		lRtCount++;
		if( lResult==1 )
		{
			lRtFound = lRtCount;
		}
	}			
	szMsg.Format("Right find refer die %ld", lRtFound);
	WT_SpecialLog(szMsg);

	XY_SafeMoveTo(HomeTbl.x, HomeTbl.y);

	WAF_CMapDieInformation *astReferList;
	astReferList  = new WAF_CMapDieInformation[ulMapNumOfReferDice];
	m_WaferMapWrapper.GetReferenceDieList(astReferList, ulMapNumOfReferDice);

	szMsg.Format("Home die %lu,%lu", ulNewHomeRow, ulNewHomeCol);
	WT_SpecialLog(szMsg);
	ULONG ulTgtRow = 0, ulTgtCol = 0, ulChkRow, ulChkCol, ulPrvRow = 0;
	BOOL bFindMatch = FALSE;
	ulPrvRow = astReferList[0].GetRow();
	CString szTemp;
	szMsg = "";
	for(ULONG i=0; i<ulMapNumOfReferDice; i++)
	{
		ulTgtRow = astReferList[i].GetRow();
		ulTgtCol = astReferList[i].GetColumn();
		szTemp.Format(" %lu,%lu", ulTgtRow, ulTgtCol);
		if( ulPrvRow==ulTgtRow )
		{
			szMsg += szTemp;
		}
		else
		{
			WT_SpecialLog("refer die" + szMsg);
			szMsg = szTemp;
		}
		ulPrvRow = ulTgtRow;
	}
	for(ULONG i=0; i<ulMapNumOfReferDice; i++)
	{
		ulTgtRow = astReferList[i].GetRow();
		ulTgtCol = astReferList[i].GetColumn();
		lUpCount = lDnCount = lLtCount = lRtCount = 0;
		for(ULONG j=0; j<ulMapNumOfReferDice; j++)
		{
			ulChkRow = astReferList[j].GetRow();
			ulChkCol = astReferList[j].GetColumn();
			if( ulChkRow==ulTgtRow )
			{
				if( ulChkCol<ulTgtCol )
					lLtCount++;
				if( ulChkCol>ulTgtCol )
					lRtCount++;
			}
			if( ulChkCol==ulTgtCol )
			{
				if( ulChkRow<ulTgtRow )
					lUpCount++;
				if( ulChkRow>ulTgtRow )
					lDnCount++;
			}
		}
		if( lLtCount==lLtFound && lRtCount==lRtFound && lUpCount==lUpFound && lDnCount==lDnFound )
		{
			szMsg.Format("match die %lu,%lu", ulTgtRow, ulTgtCol);
			WT_SpecialLog(szMsg);
			bFindMatch = TRUE;
			break;
		}
	}
	delete[] astReferList;

	if( bFindMatch )
	{
		m_ulCrossHomeDieRow = ulTgtRow;
		m_ulCrossHomeDieCol = ulTgtCol;
	}
	else
	{
		szMsg = "Cross refer die check.\nFound refer dice can not match to map!";
		HmiMessage_Red_Back(szMsg, "Auto Wafer Align");
		szMsg = "Cross refer die check. Found refer dice can not match to map!";
		WT_SpecialLog(szMsg);
	}

	return bFindMatch;
}

LONG CWaferTable::IndexAndSearchRefer(DiePhyPos2 *InputPos, LONG lPitchRow, LONG lPitchCol, LONG lLoop)
{
	CString szMsg;
	szMsg.Format("search pitch (%ld,%ld) loop %ld", lPitchRow, lPitchCol, lLoop);
	WT_SpecialLog(szMsg);
	//Calculate original no grade die pos from surrounding die position
	LONG lPosX = InputPos->x - lPitchCol * GetDiePitchX_X() - lPitchRow * GetDiePitchY_X();
	LONG lPosY = InputPos->y - lPitchRow * GetDiePitchY_Y() - lPitchCol * GetDiePitchX_Y();

	//Table only index when inside wafer limit
	if (XY_SafeMoveTo(lPosX, lPosY) == FALSE)
	{
		return 3;
	}
	Sleep(20);

	BOOL bResult = SpiralSearchRegionReferDie(lLoop, 1);

	if( bResult )
	{
		Sleep(10);
		GetEncoderValue();
		lPosX = GetCurrX();
		lPosY = GetCurrY();
	}
	else
	{
		if( SearchAndAlignDie(TRUE, TRUE, FALSE)==FALSE )
		{
			return 2;
		}
	}
	InputPos->x = lPosX;
	InputPos->y = lPosY;


	return bResult;
}

//	search this die in FOV window
BOOL CWaferTable::AOISearchOcrDie(LONG &lOffsetX, LONG &lOffsetY, CString &szOcrPrValue, CString &szMsg)
{
	szOcrPrValue = "";
	LONG lDig1 = 0, lDig2 = 0;
	BOOL bReturn = m_pBlkFunc2->Blk2SearchDgtReferDie(lOffsetX, lOffsetY, &lDig1, &lDig2);

	szOcrPrValue = m_szAoiOcrPrValue;

	CMSLogFileUtility::Instance()->MS_LogOperation("Manual align found die OCR:" + szOcrPrValue);
	if( szOcrPrValue.GetLength()<6 )
	{
		szMsg = "Search OCR value is partial:" + szOcrPrValue;
		return FALSE;
	}
 	if( szOcrPrValue.GetAt(2)<'A' && szOcrPrValue.GetAt(2)>'Z' )
	{
		szMsg = "Search OCR value BIT 3 is not a letter:" + szOcrPrValue;
		return FALSE;
	}
	if( szOcrPrValue.GetAt(4)<'A' && szOcrPrValue.GetAt(4)>'Z' )
	{
		szMsg = "Search OCR value BIT 5 is not a letter:" + szOcrPrValue;
		return FALSE;
	}
	if( szOcrPrValue.GetAt(3)<'0' && szOcrPrValue.GetAt(3)>'9' )
	{
		szMsg = "Search OCR value BIT 4 is not a number:" + szOcrPrValue;
		return FALSE;
	}
	if( szOcrPrValue.GetAt(5)<'0' && szOcrPrValue.GetAt(5)>'9' )
	{
		szMsg = "Search OCR value BIT 6 is not a number:" + szOcrPrValue;
		return FALSE;
	}

	return bReturn;
}

BOOL CWaferTable::AOISpiralSearchOcrDie(LONG &lStartX, LONG &lStartY, CString &szOcrValue)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	LONG lDiePitchX_X	= GetDiePitchX_X(); 
	LONG lDiePitchX_Y	= GetDiePitchX_Y();
	LONG lDiePitchY_Y	= GetDiePitchY_Y();
	LONG lDiePitchY_X	= GetDiePitchY_X();
	if( pUtl->GetPrescanBarWafer() && m_nDieSizeX!=0 )
	{
		lDiePitchY_Y = lDiePitchX_X*m_nDieSizeY/m_nDieSizeX;
		lDiePitchY_X = lDiePitchX_Y*m_nDieSizeY/m_nDieSizeX;
	}

	//Get current position
	LONG		lX, lY, lOrigX, lOrigY, lOrigT;
	GetEncoder(&lOrigX, &lOrigY, &lOrigT);
	lX = lOrigX;
	lY = lOrigY;

	WT_SpecialLog("spiral search refer die 1st try");

	LONG lOffsetX = 0, lOffsetY = 0;
	CString szLogMsg;
	//Search from current position	
	if (AOISearchOcrDie(lOffsetX, lOffsetY, szOcrValue, szLogMsg))
	{
		lStartX	= lX + lOffsetX;
		lStartY	= lY + lOffsetY;
		XY_SafeMoveTo(lStartX, lStartY);
		return TRUE;
	}

	LONG lLFSizeX = 1, lLFSizeY = 1;
	if ( IsLFSizeOK() )
	{
		lLFSizeX = 3;
		lLFSizeY = 3;
	}


	//lLFSizeX = GetMapIndexStepCol();
	//lLFSizeY = GetMapIndexStepRow();
	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));//Leo 20181213
	lLFSizeX = pWaferPr->GetSortFovCol();//GetMapIndexStepCol();
	lLFSizeY = pWaferPr->GetSortFovRow();//GetMapIndexStepRow();


	if( lLFSizeX%2==0 )
		lLFSizeX--;
	if( lLFSizeY%2==0 )
		lLFSizeY--;
	if( lLFSizeX<=0 )
		lLFSizeX = 1;
	if( lLFSizeY<=0 )
		lLFSizeY = 1;

	LONG lMaxLoopCount = 5;
	if( GetPrescanPitchY()!=0 )
		lMaxLoopCount = max(GetDiePitchY_Y()/GetPrescanPitchY()/2, 1);
	CString szMsg;
	szMsg.Format("Spiral search region refer die %d,%d loop %d", lLFSizeX, lLFSizeY, lMaxLoopCount);
	WT_SpecialLog(szMsg);

	for(LONG lCurrentLoop=0; lCurrentLoop<=lMaxLoopCount; lCurrentLoop++)
	{
		LONG lRow = (lCurrentLoop*2 + 1);
		LONG lCol = (lCurrentLoop*2 + 1);
		LONG lDiff_X = 0, lDiff_Y = 0;


		//Move to UP & PR search on current die
		LONG lUpIndex = lRow - 2;
		while(1)
		{
			if (lUpIndex <= 0)
			{
				break;
			}
			lUpIndex--;

			WT_SpecialLog("loop find refer to up");
			//Move table to UP
			lDiff_X = 0*lLFSizeX;
			lDiff_Y = -1*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(20);

			if (AOISearchOcrDie(lOffsetX, lOffsetY, szOcrValue, szLogMsg))
			{
				lStartX	= lX + lOffsetX;
				lStartY	= lY + lOffsetY;
				XY_SafeMoveTo(lStartX, lStartY);
				return TRUE;
			}	

			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}			


		//Move to LEFT & PR search on current die
		LONG lLtIndex = lCol - 1;
		while(1)
		{
			if (lLtIndex <= 0)
			{
				break;
			}
			lLtIndex--;

			WT_SpecialLog("loop find refer to left");
			//Move table to LEFT
			lDiff_X = -1*lLFSizeX;
			lDiff_Y = 0*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(20);

			if (AOISearchOcrDie(lOffsetX, lOffsetY, szOcrValue, szLogMsg))
			{
				lStartX	= lX + lOffsetX;
				lStartY	= lY + lOffsetY;
				XY_SafeMoveTo(lStartX, lStartY);
				return TRUE;
			}	
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}


		//Move to DOWN & PR search on current die
		LONG lDnIndex = lRow - 1;
		while(1)
		{
			if (lDnIndex <= 0)
			{
				break;
			}
			lDnIndex--;

			WT_SpecialLog("loop find refer to down");
			//Move table to DN
			lDiff_X = 0*lLFSizeX;
			lDiff_Y = 1*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(100);

			if (AOISearchOcrDie(lOffsetX, lOffsetY, szOcrValue, szLogMsg))
			{
				lStartX	= lX + lOffsetX;
				lStartY	= lY + lOffsetY;
				XY_SafeMoveTo(lStartX, lStartY);
				return TRUE;
			}	
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}


		//Move to right & PR search on current die
		LONG lRtIndex = lCol;
		while(1)
		{
			if (lRtIndex <= 0)
			{
				break;
			}
			lRtIndex--;

			//Move table to RIGHT & PR search on current die
			WT_SpecialLog("loop find refer to right");
			lDiff_X = 1*lLFSizeX;
			lDiff_Y = 0*lLFSizeY;
			lX = lX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
			lY = lY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;
			XY_SafeMoveTo(lX, lY);
			Sleep(20);

			if (AOISearchOcrDie(lOffsetX, lOffsetY, szOcrValue, szLogMsg))
			{
				lStartX	= lX + lOffsetX;
				lStartY	= lY + lOffsetY;
				XY_SafeMoveTo(lStartX, lStartY);
				return TRUE;
			}
			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}
		}
	}

	WT_SpecialLog("loop find refer fail and move back");
	XY_SafeMoveTo(lOrigX, lOrigY);

	return FALSE;
}

//	for manual add scan reference points.
LONG CWaferTable::AdjustReferPoints(IPC_CServiceMessage &svMsg)
{
	ULONG ulNewPoints = 0;
	BOOL bReturn = TRUE;
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	svMsg.GetMsg(sizeof(ULONG), &ulNewPoints);

	if( ulNewPoints<pUtl->GetAssistPointsNum() )
	{
		if( ulNewPoints<=0 )
			pUtl->DelAssistPoints();
		else
			pUtl->TrimAssistPoints(ulNewPoints);
	}

	m_ulPrescanRefPoints = pUtl->GetAssistPointsNum();

	if( !IsPrescanEnable() && !IsScnLoaded() )
	{
		m_ulPrescanRefPoints = 0;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CWaferTable::ManualAddPrescanRefPoints(IPC_CServiceMessage &svMsg)
{
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szTitle;
	BOOL bReply = TRUE;

	if( (IsBlkPickAlign() || IsBlkFuncEnable()) )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	if( !IsPrescanEnable() && !IsScnLoaded() )
	{
		HmiMessage("It is not for your current working mode.");
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	if( m_lScnAlignMethod==3 && IsScnLoaded() )
	{
	}
	else
	{
		if( IsAlignedWafer()==FALSE )	//	427TX	4
		{
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	if ( IsMapLoadingOk() != TRUE || AutoLoadRankIDFile() != TRUE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	if( IsBLInUse() )	//	4.24TX 4
	{
		if( IsWT2InUse() && IsWT2UnderCamera()==FALSE )
		{
			szTitle = "Wafer table 2 in use but table 2 not under camera!";
			HmiMessage(szTitle, "Manaul Align Wafer");
			SetErrorMessage(szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		if( !IsWT2InUse() && IsWT1UnderCamera()==FALSE )
		{
			szTitle = "Wafer table 1 in use but table 1 not under camera!";
			HmiMessage(szTitle, "Manaul Align Wafer");
			SetErrorMessage(szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		BOOL bOK = FALSE;
		BOOL bWft = FALSE;
		CString szMsg;
		if( IsWT1UnderCamera() && !IsWT2InUse() )
		{
			bOK = TRUE;
			bWft = FALSE;
		}
		if( IsWT2UnderCamera() && IsWT2InUse() )
		{
			bOK = TRUE;
			bWft = TRUE;
		}
		if( bOK )
		{
			m_lBackLightZStatus = 0;	//	4.24TX 4
			MoveES101BackLightZUpDn(TRUE);	// in prestart, if align ok, check current is safe, UP BL Z
			MoveFocusToWafer(bWft);
			SetJoystickOn(TRUE, IsWT2InUse());
			HmiMessageEx("Please move PR center to target die to add refer!", "ES101 Align Wafer",
				glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300,0,0,0,0, 400, 400);
			//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("Please move PR center to target die to add refer!");		//v4.43T10
			SetJoystickOn(FALSE, IsWT2InUse());
		}
		else
		{
			szTitle.LoadString(HMB_WT_ALIGN_DIE_FAILED);
			if( IsWT2InUse() )
				szMsg = "WT2 in use, can not manual align WT1";
			else
				szMsg = "WT1 in use, can not manual align WT2";
			HmiMessage(szMsg, szTitle);

			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}

	if( m_lScnAlignMethod==3 && IsScnLoaded() && pUtl->GetAssistPointsNum()>0 )
	{
		// if not first, based on first to locate second die position.
		LONG lUserRow=0, lUserCol=0;
		ULONG		ulAlignRow=0, ulAlignCol=0;
		m_WaferMapWrapper.GetSelectedPosition(ulAlignRow, ulAlignCol);
		ConvertAsmToOrgUser(ulAlignRow, ulAlignCol, lUserRow, lUserCol); 

		LONG lRow = 0, lCol = 0, lWfX = 0, lWfY = 0;
		BOOL bThisOk = TRUE;
		for(ULONG i=0; i<pUtl->GetAssistPointsNum(); i++)
		{
			if( pUtl->GetAssistPosition(i, lRow, lCol, lWfX, lWfY) )
			{
				if( lRow==ulAlignRow && lCol==ulAlignCol )
				{
					bThisOk = FALSE;
					break;
				}
			}
		}
		CString szMsg, szTitle = "Bar waer align";
		if( bThisOk==FALSE )
		{
			szMsg.Format("The map point %d,%d selected already.", lUserRow, lUserCol);
			HmiMessage(szMsg, szTitle);
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		LONG lAWfX = 0, lAWfY = 0, lBWfX = 0, lBWfY = 0;
		pUtl->GetAssistPosition(0, lRow, lCol, lWfX, lWfY);
		if( GetScnData(lRow, lCol, &lAWfX, &lAWfY) &&
			GetScnData(ulAlignRow, ulAlignCol, &lBWfX, &lBWfY) )
		{
			LONG lMsBWfX = lWfX + ConvertUnitToMotorStep(lBWfX-lAWfX);
			LONG lMsBWfY = lWfY + ConvertUnitToMotorStep(lBWfY-lAWfY);
			XY_SafeMoveTo(lMsBWfX, lMsBWfY);
			Sleep(100);
		}

		szMsg.Format("Please locate target die %d,%d with PR mouse.", lUserRow, lUserCol);
		HmiMessageEx(szMsg, szTitle, glHMI_MBX_OK);
	}

	BOOL	bOriginal=FALSE;
	if ( m_bJoystickOn == TRUE ) 
	{
		bOriginal = TRUE;
	}

	LONG lCurrX=0, lCurrY=0;
	CString szMsg;
	X_Sync();
	Y_Sync();
	GetEncoderValue();
	Sleep(50);
	lCurrX = GetCurrX();
	lCurrY = GetCurrY();

	if( WftMoveSearchDie(lCurrX, lCurrY, TRUE) == FALSE)
	{
		szTitle.LoadString(HMB_WT_ALIGN_DIE_FAILED);
		HmiMessage(szTitle, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 300, 150);
		SetErrorMessage("Manual Add Ref point fails.");
		bReply = FALSE;

		if ( bOriginal == TRUE )
			SetJoystickOn(TRUE);

		if( IsBLInUse() )	//	4.24TX 3
		{
			m_lBackLightZStatus = 2;	//	4.24TX 4
			MoveES101BackLightZUpDn(FALSE);	// in prestart, if align ok, to add refer die
		}

		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return 1;
	}

	LONG lUserRow=0, lUserCol=0;
	ULONG		ulAlignRow=0, ulAlignCol=0;

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	BOOL bESDummyBar = pUtl->GetPrescanAoiWafer() && pUtl->GetPrescanBarWafer();
	if( bESDummyBar && lDiePitchX_X!=0 && lDiePitchY_Y!=0 && IsAlignedWafer() )
	{
		LONG lCurRow, lCurCol, lAlnRow, lAlnCol, lAlnWfX, lAlnWfY;
		pUtl->GetAlignPosition(lAlnRow, lAlnCol, lAlnWfX, lAlnWfY);

		lCurCol = (lAlnWfX - lCurrX)/lDiePitchX_X + lAlnCol;
		lCurRow = (lAlnWfY - lCurrY)/lDiePitchY_Y + lAlnRow;

		if( lCurRow<0 )
			lCurRow = 0;
		if( lCurCol<0 )
			lCurCol = 0;

		ulAlignRow = lCurRow;
		ulAlignCol = lCurCol;
		m_WaferMapWrapper.SetSelectedPosition(ulAlignRow, ulAlignCol);
		Sleep(1000);
	}

	m_WaferMapWrapper.GetSelectedPosition(ulAlignRow, ulAlignCol);
	ConvertAsmToOrgUser(ulAlignRow, ulAlignCol, lUserRow, lUserCol); 

	m_bManualAddReferPoints = TRUE;	//	427TX	4
	if( IsPrescanEnded() )
	{
		szTitle = "Manual add align point";
		SetStatusMessage(szTitle);
		szMsg.Format("Realign Die Row: %ld, Die Col: %ld\n Die Pos (X,Y): (%ld, %ld)", lUserRow, lUserCol, lCurrX, lCurrY);
		CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szMsg);		//v4.43T10
		if( HmiMessage(szMsg, szTitle, glHMI_MBX_OKCANCEL)==glHMI_OK )
		{
			LONG lX, lY;
			if( GetPrescanWftPosn(ulAlignRow, ulAlignCol, lX, lY) )
			{
				m_ucPrescanRealignMethod = 1;
				m_ulScnDieB_Row = ulAlignRow;
				m_ulScnDieB_Col = ulAlignCol;
				m_lRlnDieB_X	= lCurrX;
				m_lRlnDieB_Y	= lCurrY;
			}
			else
			{
				HmiMessage("No original physical die position for selected die!\n\n Please select another one", szTitle);
			}
		}
	}
	else
	{
		szTitle = "Manual add refer point";
		SetStatusMessage(szTitle);
		szMsg.Format("Add assist Die Row: %ld, Die Col: %ld\n Die Pos (X,Y): (%ld, %ld)", lUserRow, lUserCol, lCurrX, lCurrY);
		SetErrorMessage(szMsg);
		CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szMsg);		//v4.43T10
		if( HmiMessage(szMsg, szTitle, glHMI_MBX_OKCANCEL)==glHMI_OK )
		{
			if( pApp->GetCustomerName()!="Semitek" || CheckMapWaferMatch_Align(ulAlignRow, ulAlignCol, lCurrX, lCurrY) )
			{
				pUtl->AddAssistPosition(ulAlignRow, ulAlignCol, lCurrX, lCurrY);
				m_ulPrescanRefPoints = pUtl->GetAssistPointsNum();
			}
			else
			{
				HmiMessage("Manual add refer point fail.\nMap--Wafer not match!");
			}
		}
	}

	if ( bOriginal == TRUE )
		SetJoystickOn(TRUE);

	if( IsBLInUse() )
	{
		m_lBackLightZStatus = 2;
		MoveES101BackLightZUpDn(FALSE);	// in prestart, if align ok, to add refer die
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

