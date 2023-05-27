//================================================================
// #include and #define
//================================================================
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_BlkFunc2.h"
#include "math.h"
#include "MS896A.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CWT_BlkFunc2::Blk2ManAlignWafer(DiePhyPos2 &HomeNewPhy)
{
	int i,j;
	ULONG		ulIndex, ulIndex1, ulIndex2;
	LONG	CtrRowIdx, CtrColIdx;
	DiePhyPos2	CurrPhy, TgtPhy;
	int			iTopLeftCol;
	LONG		lX, lY, lT;
	LONG		lDig1, lDig2;

	LONG		sResult;
	DieMapPos2	RefDieMap;
	DieMapPos2  CtrMap;
	DiePhyPos2	RefDiePhy;
	LONG		lTmp;
	CString		strTemp;

	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			m_stReferWftInfo[ulIndex].m_cInWaf = 0;
		}
	}

	//Move to center , first find refer die position
 	CurrPhy.x = HomeNewPhy.x;
	CurrPhy.y = HomeNewPhy.y;
	if (CheckWaferLimit(CurrPhy.x, CurrPhy.y) == TRUE)
	{
		MoveXYTo(CurrPhy.x, CurrPhy.y);
		Sleep(m_lPrSrchDelay);
	}
	else
	{
		return FALSE;
	}

	//Adjust Joystick
	IPC_CServiceMessage stMsg;
	LONG lRtn = 0;
	LONG lLevel = 1;
	LONG lDigital = 0;

	stMsg.InitMessage(sizeof(LONG), &lLevel);
	lRtn = m_stnWaferTable->SetJoystickSpeedCmd(stMsg);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Cree")	
		GetRealMapCenterDie(&CtrMap);
	else
		GetUserMapCenterDie(&CtrMap, &CurrPhy, &lDigital);

	Blk2GetWTPosition(&lX,&lY,&lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;

	BOOL	bAutoManualAlignOK = FALSE;
	if ( IsAutoManualAlign() )
	{
		LONG bResult = Blk2SearchReferDieInFOV( &lX, &lY, &lDig1, &lDig2);

		if( bResult==FALSE )
		{
			strTemp = "No ref die found in current position!";
			SetDisplayErrorMessage(strTemp);
		}
		else
		{
			for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
			{
				for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
				{
					ulIndex = i*MAXREFERWIDTH + j;

					ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

					if( Blk2IsSrchCharDie() )
					{
						if ( GetReferMapPosValue(ulIndex)==(lDig1*10+lDig2) )
						{
							CtrMap.Row = GetReferSrcMapRow(ulIndex);
							CtrMap.Col = GetReferSrcMapCol(ulIndex);

							//if Ref Die found does not exist in map, AutoManual Align fails
							if (GetReferInMap(ulIndex))
							{
								bAutoManualAlignOK = TRUE;
								break;
							}
							else
							{
								bAutoManualAlignOK = FALSE;
								break;
							}
						}
					}
					else
					{
						bAutoManualAlignOK = TRUE;
						break;
					}
				}

				if (bAutoManualAlignOK == TRUE)
				{
					break;
				}
			}
		}

		if( (bAutoManualAlignOK == FALSE) )
		{
			//if Auto Manual Align fails, ask user to manual align
			lTmp = SetHmiMessage("Ref Die Found does not exist in map!\nDo you want to continue by\n\n    Manual-Alignment", "Auto-Alignment Fail", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			if (lTmp == glHMI_NO)
			{
				return FALSE;
			}
		}
	}

	if ( (bAutoManualAlignOK == FALSE) )
	{
		//Ask for moving table to center, should also display the char value from wafer map
		SetJoystick(TRUE);
		if( Blk2IsSrchCharDie() )
			strTemp.Format("Please move to ref.die(r%d,c%d)(%d)...\n\nAnd press ENTER", CtrMap.Row, CtrMap.Col, lDigital);
		else
			strTemp.Format("Please move to ref.die(r%d,c%d)...\n\nAnd press ENTER", CtrMap.Row, CtrMap.Col);
		SetHmiMessageEx(strTemp, "Manual Align Wafer", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 500, 300);
		SetJoystick(FALSE);
	}

	Blk2GetWTPosition(&lX, &lY, &lT);
	CurrPhy.x = lX;
	CurrPhy.y = lY;

	BOOL bFindCtr = FALSE;
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;

			ulIndex = min(ulIndex, MAXFDC2NUM-1);		//Klocwork	//v4.02T5

			if( GetReferSrcMapRow(ulIndex) == CtrMap.Row && GetReferSrcMapCol(ulIndex) == CtrMap.Col )
			{
				sResult = Blk2SearchReferOnlyInFOV( &lX, &lY);
				if ( sResult == TRUE )
				{
					CurrPhy.x = lX;
					CurrPhy.y = lY;
					m_stReferWftInfo[ulIndex].m_cInWaf = 1;
				}
				else
				{
					m_stReferWftInfo[ulIndex].m_cInWaf = 2;
				}

				SetReferWftInfo(ulIndex, CurrPhy.x, CurrPhy.y);
				SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);

				CtrRowIdx = i;
				CtrColIdx = j;
				bFindCtr = TRUE;
				break;
			}
		}
		if( bFindCtr )
		{
			break;
		}
	}


	HomeNewPhy.x = lX;
	HomeNewPhy.y = lY;

	if (bFindCtr == FALSE)
	{
		SetHmiMessage("Manual Align can not find center die", "Block Alignment");
		return FALSE;
	}

	i = CtrRowIdx;
	j = CtrColIdx;

	// look up to Top
	while (i>GetBlkMinRowIdx())
	{
		i--;
		ulIndex = i*MAXREFERWIDTH + j;
		sResult = NextReferDiePosition(CurrPhy, BLK_REFER_UP, &TgtPhy);
		if (sResult == TRUE)
		{
			SetReferWftInfo(ulIndex, TgtPhy.x, TgtPhy.y);
			m_stReferWftInfo[ulIndex].m_cInWaf = 1;
			SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);

			CurrPhy = TgtPhy;
		}
		else if (sResult == FALSE)
		{
			if ( GetReferInMap(ulIndex)==FALSE ) 
			{
				Blk2GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				SetReferWftInfo(ulIndex, CurrPhy.x, CurrPhy.y);
				continue;
			}

			lTmp = Blk2ManualLocateReferDie(ulIndex, lX, lY);

			if ( lTmp == 1 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 5 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 8 )
			{
				return FALSE;
			}
		}
		else	// motor error
		{
			return FALSE;
		}
	}

	// top row, column min and max
	iTopLeftCol = -1;
	i = GetBlkMinRowIdx();
	for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
	{
		ulIndex = i*MAXREFERWIDTH + j;
		ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

		if (GetReferInMap(ulIndex))
		{
			iTopLeftCol = j;
			break;
		}
	}
	for (j=GetBlkMaxColIdx(); j>=GetBlkMinColIdx(); j--)
	{
		ulIndex = i*MAXREFERWIDTH + j;
		ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

		if (GetReferInMap(ulIndex))
		{
			break;
		}
	}

	// search top left part from top center to top left
	j = CtrColIdx;
	while (j>iTopLeftCol)
	{
		j--;
		ulIndex = i*MAXREFERWIDTH + j;
		ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

		sResult = NextReferDiePosition(CurrPhy, BLK_REFER_LEFT, &TgtPhy);
		if (sResult == TRUE)
		{
			SetReferWftInfo(ulIndex, TgtPhy.x, TgtPhy.y);
			m_stReferWftInfo[ulIndex].m_cInWaf = 1;
			SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);

			CurrPhy = TgtPhy;
		}
		else if (sResult == FALSE)
		{
			if ( GetReferInMap(ulIndex)==FALSE ) 
			{
				Blk2GetWTPosition(&lX,&lY,&lT);
				CurrPhy.x = lX;
				CurrPhy.y = lY;
				SetReferWftInfo(ulIndex, CurrPhy.x, CurrPhy.y);
				continue;
			}

			lTmp = Blk2ManualLocateReferDie(ulIndex, lX, lY);

			if ( lTmp == 1 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 5 )
			{
				CurrPhy.x = lX;
				CurrPhy.y = lY;
			}
			else if ( lTmp == 8 )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	// update hori FDC pitch
	ulIndex1 = GetBlkMinRowIdx()*MAXREFERWIDTH + iTopLeftCol;
	ulIndex2 = GetBlkMinRowIdx()*MAXREFERWIDTH + CtrColIdx;
	LONG lBlkPitchXX = GetDiePitchHX() * GetBlkPitchCol();
	LONG lBlkPitchXY = GetDiePitchHY() * GetBlkPitchCol();

	ulIndex1 = min(ulIndex1, MAXFDC2NUM-1);		//Klocwork

	if (iTopLeftCol != CtrColIdx && GetReferInWfa(ulIndex1) == 1)
	{
		LONG lNewPitchXX = (GetReferDiePhyX(ulIndex2) - GetReferDiePhyX(ulIndex1)) / (CtrColIdx-iTopLeftCol);
		LONG lNewPitchXY = (GetReferDiePhyY(ulIndex2) - GetReferDiePhyY(ulIndex1)) / (CtrColIdx-iTopLeftCol);
		ULONG ulBlkDist = (ULONG) GetDistIn2Points(lBlkPitchXX, lBlkPitchXY, 0, 0);
		ULONG ulNewDist = (ULONG) GetDistIn2Points(lNewPitchXX, lNewPitchXY, 0, 0);

		if( labs(ulNewDist-ulBlkDist)>(ulBlkDist*0.5) )
		{
			lBlkPitchXX = lNewPitchXX;
			lBlkPitchXY = lNewPitchXY;
		}
	}
	m_stFDCBasic.m_lFDCPitchX_X = lBlkPitchXX;
	m_stFDCBasic.m_lFDCPitchX_Y = lBlkPitchXY;

	// update vert FDC pitch
	ulIndex1 = GetBlkMinRowIdx()*MAXREFERWIDTH + CtrColIdx;
	ulIndex2 = CtrRowIdx*MAXREFERWIDTH + CtrColIdx;
	LONG lBlkPitchYY = GetDiePitchVY() * GetBlkPitchRow();
	LONG lBlkPitchYX = GetDiePitchVX() * GetBlkPitchRow();

	ulIndex1 = min(ulIndex1, MAXFDC2NUM-1);		//Klocwork

	if (GetBlkMinRowIdx() != CtrRowIdx && GetReferInWfa(ulIndex1) == 1)
	{
		LONG lNewPitchYX = (GetReferDiePhyX(ulIndex2) - GetReferDiePhyX(ulIndex1)) / (CtrRowIdx-GetBlkMinRowIdx());
		LONG lNewPitchYY = (GetReferDiePhyY(ulIndex2) - GetReferDiePhyY(ulIndex1)) / (CtrRowIdx-GetBlkMinRowIdx());
		ULONG ulNewDist = (ULONG) GetDistIn2Points(lNewPitchYX, lNewPitchYY, 0, 0);
		ULONG ulBlkDist = (ULONG) GetDistIn2Points(lBlkPitchYX, lBlkPitchYY, 0, 0);
		if( labs(ulNewDist-ulBlkDist)>(ulBlkDist*0.5) )
		{
			lBlkPitchYY = lNewPitchYY;
			lBlkPitchYX = lNewPitchYX;
		}
	}
	m_stFDCBasic.m_lFDCPitchY_Y = lBlkPitchYY;
	m_stFDCBasic.m_lFDCPitchY_X = lBlkPitchYX;

	//Begin Index
	// from top left to search, horizontal, then vertical; then next row from left to right
	LONG lDiffRow, lDiffCol;
	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;
			ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

			if ( GetReferInMap(ulIndex)==FALSE )
			{
				continue;
			}
			if (GetReferInWfa(ulIndex) != 0)
			{
				continue;
			}

			Blk2GetNearRefDie(m_stReferWftInfo[ulIndex].m_MapPos, &RefDieMap, &RefDiePhy);

			lDiffRow = GetReferMapPosRow(ulIndex) - RefDieMap.Row;
			lDiffCol = GetReferMapPosCol(ulIndex) - RefDieMap.Col;
			CurrPhy.x = RefDiePhy.x + lDiffCol*GetDiePitchHX() + lDiffRow*GetDiePitchVX();
			CurrPhy.y = RefDiePhy.y + lDiffCol*GetDiePitchHY() + lDiffRow*GetDiePitchVY();

			if (CheckWaferLimit(CurrPhy.x, CurrPhy.y) == FALSE)
			{
				continue;
			}

			//v4.04		//Klocwork
			MoveXYTo(CurrPhy.x, CurrPhy.y);

			Sleep(m_lPrSrchDelay);

			// Align RefDie
			Blk2GetWTPosition(&lX,&lY,&lT);
			sResult = Blk2SearchReferOnlyInFOV( &lX, &lY);
			if ( sResult == TRUE )
			{
				SetReferWftInfo(ulIndex, lX, lY);
				m_stReferWftInfo[ulIndex].m_cInWaf = 1;
				SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);
			}
			else
			{
				lTmp = Blk2ManualLocateReferDie(ulIndex, lX, lY);

				if ( lTmp == 8 )	// stop
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}


BOOL CWT_BlkFunc2::GetRealMapCenterDie(DieMapPos2 *MapCtrDie)	// for cree only
{
	INT		i,j;
	ULONG	ulIndex;

	MapCtrDie->Row = 500;
	MapCtrDie->Col = 500;

	CString	strTemp;
	strTemp.Format("Manual Align User Map Center die %d,%d", MapCtrDie->Row, MapCtrDie->Col);
	SetFDCRegionLog(strTemp);

	i = (GetBlkMinRowIdx() + GetBlkMaxRowIdx())/2;
	j = (GetBlkMinColIdx() + GetBlkMaxColIdx())/2;
	ulIndex = i*MAXREFERWIDTH + j;
	if ( GetReferInMap(ulIndex) )
	{
		if( GetReferSrcMapRow(ulIndex)==MapCtrDie->Row &&
			GetReferSrcMapCol(ulIndex)==MapCtrDie->Col )
		{
			return TRUE;
		}
	}

	for (i=GetBlkMinRowIdx(); i<=GetBlkMaxRowIdx(); i++)
	{
		for (j=GetBlkMinColIdx(); j<=GetBlkMaxColIdx(); j++)
		{
			ulIndex = i*MAXREFERWIDTH + j;

			ulIndex = min(ulIndex, MAXFDC2NUM-1);	//Klocwork	//v4.02T5

			if ( GetReferInMap(ulIndex) )
			{
				if( GetReferSrcMapRow(ulIndex)==MapCtrDie->Row &&
					GetReferSrcMapCol(ulIndex)==MapCtrDie->Col )
				{
					return TRUE;
				}
			}
		}
	}

	return TRUE;
}

LONG CWT_BlkFunc2::Blk2ManualLocateReferDie(ULONG ulIndex, LONG &lX, LONG &lY)
{
	CString		szText1 = "OK";		// == 1
	CString		szText2 = "Ignore";	// == 5
	CString		szText3 = "STOP";	// == 8
	CString strTemp;
	BOOL sResult;
	LONG lX0, lY0, lT;
	lX0 = lX;
	lY0 = lY;
	SetJoystick(TRUE);
	strTemp.Format("Please move to ref.die(r%d,c%d) Status.(%d)...", 
		GetReferSrcMapRow(ulIndex), GetReferSrcMapCol(ulIndex), GetReferMapPosValue(ulIndex));
	LONG lTmp = SetHmiMessageEx(strTemp, "Manual Align Wafer", glHMI_MBX_TRIPLEBUTTON,glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, &szText1, &szText2, &szText3);
	SetJoystick(FALSE);

	if ( lTmp == 1 )
	{
		// Align RefDie
		Blk2GetWTPosition(&lX,&lY,&lT);
		if (CheckWaferLimit(lX,lY) == FALSE)
		{
			return 8;
		}

		sResult = Blk2SearchReferOnlyInFOV( &lX, &lY);
		if ( sResult == TRUE )
		{
			m_stReferWftInfo[ulIndex].m_cInWaf = 1;
		}
		else
		{
			m_stReferWftInfo[ulIndex].m_cInWaf = 2;
		}
		SetReferWftInfo(ulIndex, lX, lY);
		SetReferDiePrepStatus(ulIndex, REGN_ALIGN2);
	}
	else if ( lTmp == 5 )
	{
		lX = lX0;
		lY = lY0;
	}
	else if ( lTmp == 8 )
	{
		return 8;
	}

	return lTmp;
}
