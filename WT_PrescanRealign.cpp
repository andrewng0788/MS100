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
//#include "Encryption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// after realign, should update map physical position based on refer die and theta,
// also need to update predict algorithm data
VOID CWaferTable::UpdatePrescanPosnAfterBlkPkRealign()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG lRlnRow, lRlnCol, lRlnX, lRlnY;
	LONG lAlnRow, lAlnCol, lAlnX, lAlnY;

	ULONG ulNumRow = 0, ulNumCol = 0, ulRow, ulCol;
	DieMapPos2 SrcDie;
	DieMapPos2 RefDie;
	DiePhyPos2 RefPhy;
	LONG lPhyX = 0, lPhyY = 0, lOffsetX, lOffsetY, lAlnOffX, lAlnOffY;

	m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

	DOUBLE dTheta = (DOUBLE)(pUtl->GetRealignGlobalTheta() - pUtl->GetPrescanGlobalTheta());
	DOUBLE dDegree = -(DOUBLE) (m_lThetaMotorDirection* dTheta * m_dThetaRes);
	LONG lTheta = 0;

	ScnAlignLog("BlkPk realign, get alignment position");
	lAlnOffX = 0;
	lAlnOffY = 0;
	pUtl->GetRealignPosition(lRlnRow, lRlnCol, lRlnX, lRlnY);
	pUtl->GetAlignPosition(lAlnRow, lAlnCol, lAlnX, lAlnY);
	if( lAlnRow==lRlnRow && lAlnCol==lRlnCol )
	{
		lAlnOffX = lRlnX - lAlnX;
		lAlnOffY = lRlnY - lAlnY;
	}

	BOOL bWSOK = FALSE;
	if( m_bEnableGlobalTheta==FALSE )
	{
		ScnAlignLog("BlkPk realign, no GT, find angle in virtual");
		if( IsPrescanBlkPick() || IsBlkFuncEnable() )
		{
			bWSOK = GetBlockRealignDiePosn(lAlnRow, lAlnCol);
		}

		if( bWSOK )
		{
			CalculateOrgAngle();
			CalculateRlnAngle();
		}
	}

	ScnAlignLog("BlkPk realign, update all die map position");
	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			if( GetPrescanWftPosn(ulRow, ulCol, lPhyX, lPhyY) )
			{
				lOffsetX = lAlnOffX;
				lOffsetY = lAlnOffY;
				if( m_bEnableGlobalTheta )
				{
					dDegree = -(DOUBLE) (m_lThetaMotorDirection* dTheta * m_dThetaRes);
					if( IsPrescanBlkPick() || IsBlkFuncEnable() )
					{
						SrcDie.Row = ulRow;
						SrcDie.Col = ulCol;
						if( m_pBlkFunc2->Blk2GetNearRefDie(SrcDie, &RefDie, &RefPhy)!=FALSE )
						{
							FindBlkReferDieAngle(RefDie.Row, RefDie.Col, lOffsetX, lOffsetY, dDegree);
						}
					}

					RotateWaferTheta(&lPhyX, &lPhyY, &lTheta, dDegree);

					lPhyX = lPhyX + lOffsetX;
					lPhyY = lPhyY + lOffsetY;
				}
				else
				{
					dDegree = -(DOUBLE) (m_lThetaMotorDirection* dTheta * m_dThetaRes);

					if( bWSOK==FALSE )
					{
						RotateWaferTheta(&lPhyX, &lPhyY, &lTheta, dDegree);
						lPhyX = lPhyX + lOffsetX;
						lPhyY = lPhyY + lOffsetY;
					}
					else
					{
						ConvertPrescanPosn(ulRow, ulCol, lPhyX, lPhyY);
					}
				}
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();

	// update run time after physical map updated
	ScnAlignLog("BlkPk realign, update all die prescan position");
	DelScanInfo();
	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			if( GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY) )
			{
				SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}

	ScnAlignLog("BlkPk realign, done and return");
}

BOOL CWaferTable::RealignBlkFunc2RefDiePos(CONST BOOL bAutoMode)
{
	UINT	i;
	LONG	lX, lY, lT, lRow, lCol, lWfX, lWfY;
	LONG	lX0, lY0, lT0;
	BOOL	bResult;
	BOOL	bReturn = TRUE;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	// Joystick off
	if( bAutoMode==FALSE )
		SetJoystickOn(FALSE);

	X_Sync();
	Y_Sync();
	T_Sync();
	Sleep(50);
	GetEncoder(&lX0, &lY0, &lT0);

	// Move theta home
	T_MoveTo(GetGlobalT(), SFM_WAIT);
	Sleep(50);

	bReturn = TRUE;
	//Search RefDie
	for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		if( bAutoMode==FALSE )
		{
			if (pApp->IsStopAlign() )
			{
				bReturn = FALSE;
				break;
			}
		}
		else
		{
			if (m_bStop)
			{
				bReturn = FALSE;
				break;
			}
		}

		pUtl->GetReRefPosition(i, lRow, lCol, lWfX, lWfY);

		if (XY_SafeMoveTo(lWfX, lWfY) == FALSE)
		{
			continue;
		}

		Sleep(50);

		GetEncoder(&lX,&lY,&lT);

		bResult = BlkFunc2SearchCurrDie(&lX, &lY);

		SetMapPhyPosn(lRow, lCol, lX, lY);
		if ( bResult != TRUE )
			continue;

		pUtl->UpdateReRefPosition(i, lRow, lCol, lX, lY);
	}

	XY_SafeMoveTo(lX0, lY0);
	T_MoveTo(lT0, SFM_WAIT);

	Sleep(200);

	return bReturn;
}

// after realign, should update map physical position based on refer die and theta,
// also need to update predict algorithm data
VOID CWaferTable::UpdatePrescanPosnAfterBlockRealign()
{
	if( !IsPrescanEnded() )
		return ;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG lRlnRow, lRlnCol, lRlnX, lRlnY;
	LONG lAlnRow, lAlnCol, lAlnX, lAlnY;

	ULONG ulNumRow = 0, ulNumCol = 0, ulRow, ulCol;
	DieMapPos2 SrcDie;
	DieMapPos2 RefDie;
	DiePhyPos2 RefPhy;
	LONG lPhyX = 0, lPhyY = 0, lOffsetX, lOffsetY;

	m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

	DelPrescanTmpPosn();
	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			if( GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY) )
			{
				SetPrescanTmpPosn(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}

	pUtl->GetRealignPosition(lRlnRow, lRlnCol, lRlnX, lRlnY);
	pUtl->GetAlignPosition(lAlnRow, lAlnCol, lAlnX, lAlnY);
	GetPrescanRunWftPosn(lAlnRow, lAlnCol, lAlnX, lAlnY);
	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			if( GetPrescanRunWftPosn(ulRow, ulCol, lPhyX, lPhyY) )
			{
				lOffsetX = 0;
				lOffsetY = 0;
				if( lAlnRow==lRlnRow && lAlnCol==lRlnCol )
				{
					lOffsetX = lRlnX - lAlnX;
					lOffsetY = lRlnY - lAlnY;
				}
				if( IsPrescanBlkPick() || IsBlkFuncEnable() )
				{
					SrcDie.Row = ulRow;
					SrcDie.Col = ulCol;
					if( m_pBlkFunc2->Blk2GetNearRefDie(SrcDie, &RefDie, &RefPhy)!=FALSE )
					{
						LONG lAlnX, lAlnY;
						LONG lRlnRow, lRlnCol, lRlnX, lRlnY;
						if( GetPrescanRunWftPosn(RefDie.Row, RefDie.Col, lAlnX, lAlnY) )
						{
							// find the offset firstly
							for(UINT i=0; i<pUtl->GetNumOfReferPoints(); i++)
							{
								pUtl->GetReRefPosition(i, lRlnRow, lRlnCol, lRlnX, lRlnY);
								if( lRlnRow==RefDie.Row && lRlnCol==RefDie.Col )
								{
									lOffsetX = lRlnX - lAlnX;
									lOffsetY = lRlnY - lAlnY;
									break;
								}
							}
						}
					}
				}

				lPhyX = lPhyX + lOffsetX;
				lPhyY = lPhyY + lOffsetY;
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();

	DelScanInfo();
	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			if( GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY) )
			{
				SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}
	m_bScnModeWaferAlignTwice	= TRUE;

	DEB_UpdateAllDicePos();
	DEB_CheckKeyDicesOnWafer();

	//Use 5-pt-check fcn from scanner mode for prescan result checking 
	if( IsEnableFPC() )
	{
		FivePointCheckDiePosition();
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( (pApp->GetCustomerName()!="Cree") )
	{
		EmptyDieGradeCheck();
	}

	RectWaferCornerCheck();

	DelPrescanTmpPosn();

	m_WaferMapWrapper.Redraw();
}

BOOL CWaferTable::GetBlockRealignDiePosn(ULONG ulRow, ULONG ulCol)
{
	bool bFindInList = false;
	UINT i = 0;
	LONG lAlnRow, lAlnCol, lAlnX=0, lAlnY=0, lAlnX1=0, lAlnY1=0, lAlnX2=0, lAlnY2=0;
	LONG lRlnRow, lRlnCol, lRlnX=0, lRlnY=0, lRlnX1=0, lRlnY1=0, lRlnX2=0, lRlnY2=0;
	LONG lLeft, lRight, lUp, lDown;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	LONG lReferRow = ulRow;
	LONG lReferCol = ulCol;
	// find the offset firstly
	for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReferPosition(i, lAlnRow, lAlnCol, lAlnX, lAlnY);
		if( lAlnRow==lReferRow && lAlnCol==lReferCol )
		{
			pUtl->GetReRefPosition(i, lRlnRow, lRlnCol, lRlnX, lRlnY);
			m_lOrgDieA_X = lAlnX;
			m_lOrgDieA_Y = lAlnY;
			m_lOrgDieB_X = lAlnX;
			m_lOrgDieB_Y = lAlnY;

			m_lRlnDieA_X = lRlnX;
			m_lRlnDieA_Y = lRlnY;
			m_lRlnDieB_X = lRlnX;
			m_lRlnDieB_Y = lRlnY;
			bFindInList = true;
			break;
		}
	}

	if( bFindInList==false )
	{
		return FALSE;
	}

	// find the left nearest and right nearest in horizontal
	lLeft = lReferCol;
	lRight = lReferCol;
	for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReferPosition(i, lAlnRow, lAlnCol, lAlnX, lAlnY);
		pUtl->GetReRefPosition(i, lRlnRow, lRlnCol, lRlnX, lRlnY);
		if( lAlnRow==lReferRow )
		{
			if( lAlnCol<lReferCol )
			{
				lLeft = lAlnCol;
				lAlnX1 = lAlnX;
				lAlnY1 = lAlnY;
				lRlnX1 = lRlnX;
				lRlnY1 = lRlnY;
			}
			else if( lAlnCol>lReferCol )
			{
				lRight = lAlnCol;
				lAlnX2 = lAlnX;
				lAlnY2 = lAlnY;
				lRlnX2 = lRlnX;
				lRlnY2 = lRlnY;
				break;
			}
		}
	}
	if( lLeft==lRight )
	{
		return FALSE;
	}

	m_lOrgDieB_X = lAlnX2;
	m_lOrgDieB_Y = lAlnY2;

	m_lRlnDieB_X = lRlnX2;
	m_lRlnDieB_Y = lRlnY2;

	// find the up nearest and down nearest in vertical
	lUp = lReferRow;
	lDown = lReferRow;
	for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReferPosition(i, lAlnRow, lAlnCol, lAlnX, lAlnY);
		pUtl->GetReRefPosition(i, lRlnRow, lRlnCol, lRlnX, lRlnY);
		if( lAlnCol==lReferCol )
		{
			if( lAlnRow<lReferRow )
			{
				lUp = lAlnRow;
				lAlnX1 = lAlnX;
				lAlnY1 = lAlnY;
				lRlnX1 = lRlnX;
				lRlnY1 = lRlnY;
			}
			else if( lAlnRow>lReferRow )
			{
				lDown = lAlnRow;
				lAlnX2 = lAlnX;
				lAlnY2 = lAlnY;
				lRlnX2 = lRlnX;
				lRlnY2 = lRlnY;
				break;
			}
		}
	}

	if( lUp==lDown )
	{
		return FALSE;
	}

	m_lOrgDieA_X = lAlnX1;
	m_lOrgDieA_Y = lAlnY1;

	m_lRlnDieA_X = lRlnX1;
	m_lRlnDieA_Y = lRlnY1;

	return TRUE;
}

BOOL CWaferTable::IsRealignScnnedWafer()
{
	if( IsPrescanEnded()==FALSE )
	{
		return FALSE;
	}

	if( m_bAlignNeedRescan )
	{
		return FALSE;
	}

	if( IsAOIOnlyMachine() )
	{
		return FALSE;
	}

	if( IsScnLoaded() )
	{
		return FALSE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( IsPrescanBlkPick() && pApp->GetCustomerName()!="OSRAM" )
	{
		return FALSE;
	}

	return TRUE;
}

// after realign, should update map physical position based on refer die and theta,
// also need to update predict algorithm data
VOID CWaferTable::UpdatePrescanPosnAfterWaferRealign()	// for no block pick algorithm only
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	LONG lRlnRow, lRlnCol, lRlnX, lRlnY, lAlnX, lAlnY;
	LONG lAlnOffX = 0, lAlnOffY = 0;
	LONG lTheta = 0;
	CString szMsg;

	ULONG ulNumRow = 0, ulNumCol = 0, ulRow, ulCol;
	LONG lPhyX = 0, lPhyY = 0;

	m_pWaferMapManager->GetWaferMapDimension(ulNumRow, ulNumCol);

	DOUBLE dTheta = (DOUBLE)(pUtl->GetRealignGlobalTheta() - pUtl->GetPrescanGlobalTheta());
	DOUBLE dDegree = -(DOUBLE) (m_lThetaMotorDirection* dTheta * m_dThetaRes);
	pUtl->GetRealignPosition(lRlnRow, lRlnCol, lRlnX, lRlnY);
	if( GetPrescanWftPosn(lRlnRow, lRlnCol, lAlnX, lAlnY)!=TRUE )
	{
		szMsg = "Realign die has no original physical position, please manual align!";
		HmiMessage(szMsg, "Realign");
		ScnAlignLog(szMsg);
		return ;
	}
	lAlnOffX = lRlnX - lAlnX;
	lAlnOffY = lRlnY - lAlnY;

	BOOL bWsModeOK = FALSE;
	if( m_ucPrescanRealignMethod==2 )	// by auto check L shape die
	{
		//Get SCN data from wafermap function 
		if( GetPrescanWftPosn(m_ulScnDieA_Row, m_ulScnDieA_Col, m_lOrgDieA_X, m_lOrgDieA_Y)!=TRUE )
		{
			szMsg = "Scan A die has no original physical position, please manual align!";
			HmiMessage(szMsg, "Realign");
			ScnAlignLog(szMsg);
			return ;
		}
		m_lRlnDieA_X = m_lMsDieA_X;
		m_lRlnDieA_Y = m_lMsDieA_Y;
		if( GetPrescanWftPosn(m_ulScnDieB_Row, m_ulScnDieB_Col, m_lOrgDieB_X, m_lOrgDieB_Y)!=TRUE )
		{
			szMsg = "Scan B die has no original physical position, please manual align!";
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if( IsPrescanEnable() && pApp->GetCustomerName()!="OSRAM" )
				HmiMessage(szMsg, "Realign");
			ScnAlignLog(szMsg);
			return ;
		}
		m_lRlnDieB_X = m_lMsDieB_X;
		m_lRlnDieB_Y = m_lMsDieB_Y;
		bWsModeOK = TRUE;
		ScnAlignLog("No BLK and scanner realignment method is 2");
	}
	else
	{
		if( m_ucPrescanRealignMethod==1 ) // manual add refer base point before align executed
		{
			m_lRlnDieA_X = lRlnX;
			m_lRlnDieA_Y = lRlnY;
			if( GetPrescanWftPosn(lRlnRow, lRlnCol, m_lOrgDieA_X, m_lOrgDieA_Y)!=TRUE )
			{
				szMsg = "Realign die has no original physical position, please manual align!";
				HmiMessage(szMsg, "Realign");
				ScnAlignLog(szMsg);
				return ;
			}
			if( GetPrescanWftPosn(m_ulScnDieB_Row, m_ulScnDieB_Col, m_lOrgDieB_X, m_lOrgDieB_Y)!=TRUE )
			{
				szMsg = "Refer align die has no original physical position, please manual align!";
				HmiMessage(szMsg, "Realign");
				ScnAlignLog(szMsg);
				return ;
			}
			ScnAlignLog("scanner method realignment is 1");
			bWsModeOK = TRUE;
		}
		else
		{
			ScnAlignLog("scanner realignment unkwown");
		}
	}

	if( bWsModeOK==TRUE )
	{
		ScnAlignLog("calculate scanner angle");
		CalculateOrgAngle();
		CalculateRlnAngle();
	}

	ScnAlignLog("calcualte all scanned die position");
	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			if( GetPrescanWftPosn(ulRow, ulCol, lPhyX, lPhyY) )
			{
				if( bWsModeOK )
				{
					ConvertPrescanPosn(ulRow, ulCol, lPhyX, lPhyY);
				}
				else
				{
					RotateWaferTheta(&lPhyX, &lPhyY, &lTheta, dDegree);
					lPhyX = lPhyX + lAlnOffX;
					lPhyY = lPhyY + lAlnOffY;
				}
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();

	// update run time after physical map updated
	ScnAlignLog("del run prescan position");
	DelScanInfo();
	ScnAlignLog("set run prescan position");
	for(ulRow=0; ulRow<=ulNumRow; ulRow++)
	{
		for(ulCol=0; ulCol<=ulNumCol; ulCol++)
		{
			if( GetMapPhyPosn(ulRow, ulCol, lPhyX, lPhyY) )
			{
				SetPrescanPosition(ulRow, ulCol, lPhyX, lPhyY);
			}
		}
	}
}

VOID CWaferTable::FindBlkReferDieAngle(LONG lReferRow, LONG lReferCol, LONG &lX, LONG &lY, DOUBLE &dDegree)
{
	bool bFindInList = false;
	UINT i = 0;
	LONG lAlnRow, lAlnCol, lAlnX, lAlnY, lAlnX1, lAlnY1, lAlnX2, lAlnY2;
	LONG lRlnRow, lRlnCol, lRlnX, lRlnY, lRlnX1, lRlnY1, lRlnX2, lRlnY2;
	LONG lLeft, lRight, lUp, lDown;
	DOUBLE dDiffX, dDiffY, dAlnT, dRlnT;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	// find the offset firstly
	for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReferPosition(i, lAlnRow, lAlnCol, lAlnX, lAlnY);
		if( lAlnRow==lReferRow && lAlnCol==lReferCol )
		{
			pUtl->GetReRefPosition(i, lRlnRow, lRlnCol, lRlnX, lRlnY);
			lX = lRlnX - lAlnX;
			lY = lRlnY - lAlnY;
			bFindInList = true;
			lAlnX2 = lAlnX1 = lAlnX;
			lAlnY2 = lAlnY1 = lAlnY;
			lRlnX2 = lRlnX1 = lRlnX;
			lRlnY2 = lRlnY1 = lRlnY;
			break;
		}
	}

	if( bFindInList==false )
	{
		return ;
	}

	// find the left nearest and right nearest in horizontal
	lLeft = lReferCol;
	lRight = lReferCol;
	for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReferPosition(i, lAlnRow, lAlnCol, lAlnX, lAlnY);
		pUtl->GetReRefPosition(i, lRlnRow, lRlnCol, lRlnX, lRlnY);
		if( lAlnRow==lReferRow )
		{
			if( lAlnCol<lReferCol )
			{
				lLeft = lAlnCol;
				lAlnX1 = lAlnX;
				lAlnY1 = lAlnY;
				lRlnX1 = lRlnX;
				lRlnY1 = lRlnY;
			}
			else if( lAlnCol>lReferCol )
			{
				lRight = lAlnCol;
				lAlnX2 = lAlnX;
				lAlnY2 = lAlnY;
				lRlnX2 = lRlnX;
				lRlnY2 = lRlnY;
				break;
			}
		}
	}
	if( lLeft!=lRight )
	{
		dDiffY = (DOUBLE)(lAlnY1 - lAlnY2);
		dDiffX = (DOUBLE)(lAlnX1 - lAlnX2);
		dAlnT = atan2(dDiffY, dDiffX) * (180/PI);

		dDiffY = (DOUBLE)(lRlnY1 - lRlnY2);
		dDiffX = (DOUBLE)(lRlnX1 - lRlnX2);
		dRlnT = atan2(dDiffY, dDiffX) * (180/PI);
		dDegree = dRlnT - dAlnT;
		return ;
	}

	// find the up nearest and down nearest in vertical
	lUp = lReferRow;
	lDown = lReferRow;
	for(i=0; i<pUtl->GetNumOfReferPoints(); i++)
	{
		pUtl->GetReferPosition(i, lAlnRow, lAlnCol, lAlnX, lAlnY);
		pUtl->GetReRefPosition(i, lRlnRow, lRlnCol, lRlnX, lRlnY);
		if( lAlnCol==lReferCol )
		{
			if( lAlnRow<lReferRow )
			{
				lUp = lAlnRow;
				lAlnX1 = lAlnX;
				lAlnY1 = lAlnY;
				lRlnX1 = lRlnX;
				lRlnY1 = lRlnY;
			}
			else if( lAlnRow>lReferRow )
			{
				lDown = lAlnRow;
				lAlnX2 = lAlnX;
				lAlnY2 = lAlnY;
				lRlnX2 = lRlnX;
				lRlnY2 = lRlnY;
				break;
			}
		}
	}

	if( lUp!=lDown )
	{
		dDiffY = (DOUBLE)(lAlnY1 - lAlnY2);
		dDiffX = (DOUBLE)(lAlnX1 - lAlnX2);
		dAlnT = atan2(dDiffX , dDiffY * -1.00) * (180 / PI);

		dDiffY = (DOUBLE)(lRlnY1 - lRlnY2);
		dDiffX = (DOUBLE)(lRlnX1 - lRlnX2);
		dRlnT = atan2(dDiffX , dDiffY * -1.00) * (180 / PI);

		dDegree = dRlnT - dAlnT;
	}
}

VOID CWaferTable::CalculateOrgAngle(VOID)
{
	DOUBLE	dDistX	= 0.0;		
	DOUBLE	dDistY	= 0.0;		
	DOUBLE	dDistXY = 0.0;		

	//Calculate angle
	dDistX	= m_lOrgDieA_X - m_lOrgDieB_X;
	dDistY	= m_lOrgDieA_Y - m_lOrgDieB_Y;
	dDistXY = sqrt((dDistX*dDistX) + (dDistY*dDistY));

	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	lDiePitchX_X = labs(lDiePitchX_X)/2;
	lDiePitchY_Y = labs(lDiePitchY_Y)/2;
	if( dDistXY<lDiePitchX_X && dDistXY<lDiePitchY_Y )
	{
		HmiMessage_Red_Back("Original two alignment die is same die", "OrgAngle");
	}

	if (dDistXY < 1.0)
	{
		dDistXY = 1.0;
	}

	m_dOrgAngleCos_X = dDistX / dDistXY;
	m_dOrgAngleSin_Y = dDistY / dDistXY;
}

VOID CWaferTable::CalculateRlnAngle(VOID)
{
	DOUBLE	dDistX	= 0.0;		
	DOUBLE	dDistY	= 0.0;		
	DOUBLE	dDistXY = 0.0;		


	//Calculate angle
	dDistX	= (DOUBLE)(m_lRlnDieB_X - m_lRlnDieA_X);
	dDistY	= (DOUBLE)(m_lRlnDieB_Y - m_lRlnDieA_Y);
	dDistXY = sqrt((dDistX*dDistX) + (dDistY*dDistY));
	LONG lDiePitchX_X = GetDiePitchX_X(); 
	LONG lDiePitchY_Y = GetDiePitchY_Y();
	lDiePitchX_X = labs(lDiePitchX_X)/2;
	lDiePitchY_Y = labs(lDiePitchY_Y)/2;
	if( dDistXY<lDiePitchX_X && dDistXY<lDiePitchY_Y )
	{
		HmiMessage_Red_Back("Realign two alignment die is same die", "RlnAngle");
	}

	if (dDistXY < 1.0)
	{
		dDistXY = 1.0;
	}

	m_dRlnAngleCos_X = dDistX / dDistXY;
	m_dRlnAngleSin_Y = dDistY / dDistXY;
}

BOOL CWaferTable::ConvertPrescanPosn(ULONG ulCurrRow, ULONG ulCurrCol, LONG &lPhyPosX, LONG &lPhyPosY)
{
	DOUBLE	dTemp1X = 0;
	DOUBLE	dTemp1Y = 0;
	DOUBLE	dTemp2X = 0;
	DOUBLE	dTemp2Y = 0;
	LONG	lScnPosX = 0;
	LONG	lScnPosY = 0;

	GetPrescanWftPosn(ulCurrRow, ulCurrCol, lScnPosX, lScnPosY);
	
	//Tansform SCN position related to angle	
	dTemp1X		= (DOUBLE)(lScnPosX - m_lOrgDieA_X);
	dTemp1Y		= (DOUBLE)(lScnPosY - m_lOrgDieA_Y);
	dTemp2X		= (dTemp1X * m_dOrgAngleCos_X) + (dTemp1Y * m_dOrgAngleSin_Y);
	dTemp2Y		= (dTemp1Y * m_dOrgAngleCos_X) - (dTemp1X * m_dOrgAngleSin_Y);

	//Re-Transform back to MS896 position
	dTemp1X		= (dTemp2X * m_dRlnAngleCos_X) - (dTemp2Y * m_dRlnAngleSin_Y);
	dTemp1Y		= (dTemp2X * m_dRlnAngleSin_Y) + (dTemp2Y * m_dRlnAngleCos_X);

	lPhyPosX	= m_lRlnDieA_X - (LONG)(dTemp1X);
	lPhyPosY	= m_lRlnDieA_Y - (LONG)(dTemp1Y);

	return TRUE;
}
