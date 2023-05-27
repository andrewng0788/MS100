#include "stdafx.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "HmiDataManager.h"
#include "WaferStat.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "BL_Constant.h"
#include "WT_SubRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	BH_BH_MBL	"C:\\MapSorter\\Userdata\\History\\BH_BT_MBL.txt"
#define	MS90_BT_LVL_LIMIT	1500
typedef struct
{
	LONG lX;
	LONG lY;
} BT_XY_STRUCT;

BOOL CBondHead::BH_GetLearnLevel_BT(BOOL bBHZ2, LONG &lBondZ)
{
	LONG lBT_LearnX = 0, lBT_LearnY = 0, lLearnZ = 0;
	BOOL bBT_LearnR = FALSE;
	if( bBHZ2 )
	{
		lBT_LearnX = m_lZ2BondLevelBT_X;
		lBT_LearnY = m_lZ2BondLevelBT_Y;
		bBT_LearnR = m_bZ2BondLevelBT_R;
	}
	else
	{
		lBT_LearnX = m_lZ1BondLevelBT_X;
		lBT_LearnY = m_lZ1BondLevelBT_Y;
		bBT_LearnR = m_bZ1BondLevelBT_R;
	}

	LONG lBTX = lBT_LearnX, lBTY = lBT_LearnY;
	if( bBT_LearnR!=IsMS90BTRotated() )	//	180 rotated
	{
//		RotateBinTable180(lBT_LearnX, lBT_LearnY, lBTX, lBTY);	//	rotate 180 degree to get BT XY
	}

	return BH_BT_CalculateLevel(bBT_LearnR, lBTX, lBTY, lBondZ);
}

BOOL CBondHead::BH_GetCurntLevel_BT(LONG lBTX, LONG lBTY, LONG &lBondZ)
{
	return BH_BT_CalculateLevel(IsMS90BTRotated(), lBTX, lBTY, lBondZ);
}

BOOL CBondHead::BH_BT_CalculateLevel(BOOL bBTRotated, LONG lBTX, LONG lBTY, LONG &lBondZ)
{
	PB_CMultiProbeLevelInfo	stM_P_L;
	stM_P_L.InitPLPoints();
	if( bBTRotated )
	{
		stM_P_L.SetPLState(m_stM_B_L2.GetPLState());
		for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
		{
			for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
			{
				LONG lScanX, lScanY, lPL_Z;
				LONG lState = m_stM_B_L2.GetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z);
				stM_P_L.SetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z, lState);
			}
		}
	}
	else
	{
		stM_P_L.SetPLState(m_stM_B_L1.GetPLState());
		for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
		{
			for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
			{
				LONG lScanX, lScanY, lPL_Z;
				LONG lState = m_stM_B_L1.GetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z);
				stM_P_L.SetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z, lState);
			}
		}
	}

	if( stM_P_L.GetPLState()==FALSE )
	{
		return FALSE;
	}

	LONG ulBTGridMaxRow = BH_BT_SAMPLE_POINTS - 1, ulBTGridMaxCol = BH_BT_SAMPLE_POINTS - 1;

	CString szTemp;
	ULONG ulRow11 = 0, ulCol11 = 0, ulRow21 = 0, ulCol21 = 0, ulRow12 = 0, ulCol12 = 0, ulRow22 = 0, ulCol22 = 0;
	LONG lScanX_11, lScanY_11, lScanX_21, lScanY_21, lScanX_12, lScanY_12, lScanX_22, lScanY_22;
	LONG lPL_Z_11 = 0, lPL_Z_21 = 0, lPL_Z_12 = 0, lPL_Z_22 = 0;

	BOOL bFindOne = FALSE;
	//	11-------------21
	//	----------------
	//	----------------
	//	12-------------22
	BOOL bFindUL = stM_P_L.GetPLPointULCorner(lBTX, lBTY, ulRow11, ulCol11);
	BOOL bFindLR = stM_P_L.GetPLPointLRCorner(lBTX, lBTY, ulRow22, ulCol22);
	if( bFindUL || bFindLR )
	{
		if( bFindUL && bFindLR )
		{
			bFindOne = TRUE;
		}
		else
		{
			if( bFindUL )
			{
				if( stM_P_L.GetPointPL(ulRow11, ulCol11+1, lScanX_11, lScanY_11, lPL_Z_11)>0 )
				{
					ulRow11--;
					bFindOne = TRUE;
				}
				else if( stM_P_L.GetPointPL(ulRow11+1, ulCol11, lScanX_21, lScanY_21, lPL_Z_21)>0 )
				{
					ulCol11--;
					bFindOne = TRUE;
				}
				else if( ulRow11>0 && ulCol11>0 )
				{
					ulRow11--;
					ulCol11--;
					bFindOne = TRUE;
				}
			}
			else
			{
				if( ulRow22==0 && ulCol22==0 )
				{
					ulRow11 = 0;
					ulCol11 = 0;
					bFindOne = TRUE;
				}
				else
				{
					if( stM_P_L.GetPointPL(ulRow22, ulCol22-1, lScanX_12, lScanY_12, lPL_Z_12)>0 )
					{
						ulRow11 = ulRow22;
						ulCol11 = ulCol22-1;
						bFindOne = TRUE;
					}
					else if( stM_P_L.GetPointPL(ulRow22-1, ulCol22, lScanX_22, lScanY_22, lPL_Z_22)>0 )
					{
						ulRow11 = ulRow22 - 1;
						ulCol11 = ulCol22;
						bFindOne = TRUE;
					}
				}
			}
		}

	}

	if( bFindOne==FALSE )
	{
		if( stM_P_L.GetPointPL(0, 0, lScanX_11, lScanY_11, lPL_Z_11)>0 && 
			stM_P_L.GetPointPL(ulBTGridMaxRow, ulBTGridMaxCol, lScanX_22, lScanY_22, lPL_Z_21)>0 )
		{
			if( lBTX<=lScanX_22 && lBTY>=lScanY_11 )
			{
				ulRow11 = 0;
				ulCol11 = ulBTGridMaxCol-1;
				bFindOne = TRUE;
			}
			else if( lBTX>=lScanX_11 && lBTY<=lScanY_22 )
			{
				ulRow11 = ulBTGridMaxRow - 1;
				ulCol11 = 0;
				bFindOne = TRUE;
			}
		}
	}

	if( bFindOne )
	{
		ulRow21 = ulRow11;
		ulCol21 = ulCol11+1;
		ulRow12 = ulRow11+1;
		ulCol12 = ulCol11;
		ulRow22 = ulRow11+1;
		ulCol22 = ulCol11+1;
	}

	if( bFindOne )
	{
		BOOL bGood_11 = stM_P_L.GetPointPL(ulRow11, ulCol11, lScanX_11, lScanY_11, lPL_Z_11)>0;
		BOOL bGood_21 = stM_P_L.GetPointPL(ulRow21, ulCol21, lScanX_21, lScanY_21, lPL_Z_21)>0;
		BOOL bGood_12 = stM_P_L.GetPointPL(ulRow12, ulCol12, lScanX_12, lScanY_12, lPL_Z_12)>0;
		BOOL bGood_22 = stM_P_L.GetPointPL(ulRow22, ulCol22, lScanX_22, lScanY_22, lPL_Z_22)>0;

		if( !bGood_11 && !bGood_21 && !bGood_12 && !bGood_22 )
		{
		}
		else
		{
			if( !bGood_11 )
			{
				if( bGood_21 )
				{
					lPL_Z_11 = lPL_Z_21;
				}
				else if( bGood_12 )
				{
					lPL_Z_11 = lPL_Z_12;
				}
				else
				{
					lPL_Z_11 = lPL_Z_22;
				}
			}

			if( !bGood_21 )
			{
				if( bGood_11 )
				{
					lPL_Z_21 = lPL_Z_11;
				}
				else if( bGood_22 )
				{
					lPL_Z_21 = lPL_Z_22;
				}
				else
				{
					lPL_Z_21 = lPL_Z_12;
				}
			}

			if( !bGood_12 )
			{
				if( bGood_11 )
				{
					lPL_Z_12 = lPL_Z_11;
				}
				else if( bGood_22 )
				{
					lPL_Z_12 = lPL_Z_22;
				}
				else
				{
					lPL_Z_12 = lPL_Z_21;
				}
			}

			if( !bGood_22 )
			{
				if( bGood_21 )
				{
					lPL_Z_22 = lPL_Z_21;
				}
				else if( bGood_12 )
				{
					lPL_Z_22 = lPL_Z_12;
				}
				else
				{
					lPL_Z_22 = lPL_Z_11;
				}
			}

			LONG lX1 = max(lScanX_11, lScanX_12);
			LONG lX2 = min(lScanX_21, lScanX_22);
			LONG lY1 = max(lScanY_11, lScanY_21);
			LONG lY2 = min(lScanY_12, lScanY_22);

			WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
			lBondZ = pCPInfo->BiLinearInterpolation(lX1, lY1, lX2, lY2, lPL_Z_11, lPL_Z_21, lPL_Z_12, lPL_Z_22, lBTX, lBTY);

	//		szTemp.Format("UL %d,%d LR %d,%d, Z %d,%d,%d,%d", lX1, lY1, lX2, lY2, lPL_Z_11, lPL_Z_21, lPL_Z_12, lPL_Z_22);
	//		SetAlarmLog(szTemp);

	//		szTemp.Format("MPL: die %8d,%8d; ULcor(%2d,%2d) probe z %5d",	lBTX, lBTY, ulRow11, ulCol11, lBondZ);
	//		SetAlarmLog(szTemp);
			return TRUE;
		}
	}

	if( stM_P_L.GetNearestValidPL(lBTX, lBTY, ulRow11, ulCol11) )
	{
		if( stM_P_L.GetPointPL(ulRow11, ulCol11, lScanX_11, lScanY_11, lPL_Z_11)>0 )
		{
			lBondZ = lPL_Z_11;
	//		szTemp.Format("MPL: nearst die %8d,%8d; UL(%2d,%2d) probe z %5d",	lBTX, lBTY, ulRow11, ulCol11, lBondZ);
	//		SetAlarmLog(szTemp);
			return TRUE;
		}
	}

	return FALSE;
}

LONG CBondHead::GetBTLevelOffset(BOOL bBHZ2, LONG lBT_CurrX, LONG lBT_CurrY)
{
	LONG lBTLevelOffset = 0;

	m_szLearnLevelBT = "";
	if (m_bUseMultiProbeLevel)	//	get bt level offset
	{
		LONG lLearnZ = 0, lCurrtZ = 0;
		LONG lLevelLimit = BH_BT_TILTING_LIMIT;
		if (IsMS90())
		{
			lLevelLimit = MS90_BT_LVL_LIMIT;
		}

		if (BH_GetLearnLevel_BT(bBHZ2, lLearnZ) &&
			BH_GetCurntLevel_BT(lBT_CurrX, lBT_CurrY, lCurrtZ))
		{
			lBTLevelOffset = lCurrtZ - lLearnZ;
			if (labs(lBTLevelOffset) > lLevelLimit)
			{
				CString szErr;
				szErr.Format("Get BT tilting fail because Offset %d > %d steps, force to 0!", lBTLevelOffset, lLevelLimit);
				SetErrorMessage(szErr);
				lBTLevelOffset = 0;
			}
		}
		m_szLearnLevelBT.Format("tilt %d (Learn Lvl %ld, Currt R=%d Lvl %ld)", lBTLevelOffset, lLearnZ, IsMS90BTRotated(), lCurrtZ);
	}

	return lBTLevelOffset;
}

LONG CBondHead::GetZ1BondLevel(BOOL bAutoCycle, BOOL bUseBT)
{
	LONG lBTLevelOffset = 0;
	LONG lBT_X = 0;
	LONG lBT_Y = 0;
	//	get bhz1 tilting
	m_szLearnLevelBT = "";
//	BOOL bMoved = TRUE;
	if( bAutoCycle )
	{
//		bMoved= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["Bond Moved"];
		lBT_X = (*m_psmfSRam)["BinTable"]["Bond Posn X"];
		lBT_Y = (*m_psmfSRam)["BinTable"]["Bond Posn Y"];
		(*m_psmfSRam)["BinTable"]["Bond Moved"]		= FALSE;
	}
	else
	{
		BT_Get_XY_Posn(lBT_X, lBT_Y);
	}
	if (m_bUseMultiProbeLevel/* && bMoved*/)
	{
		lBTLevelOffset = GetBTLevelOffset(FALSE, lBT_X, lBT_Y);	//	get bhz1
	}

	BOOL bReturn = FALSE;
	LONG lBondLevel = GetBondPadLevel(FALSE, lBT_X, lBT_Y, bReturn);
	if( bReturn )
	{
		lBondLevel = lBondLevel + m_lWPadsLevelOffsetZ1;
	}

	LONG lLevel = lBondLevel + lBTLevelOffset;
	if( bAutoCycle )
	{
		lLevel = lLevel - m_lCurrOffsetZ1byPBEmptyCheck;
		LONG lBTInUse		= (*m_psmfSRam)["BinTable"]["BTInUse"];
		if ((lBTInUse == 1) && (m_lBT2OffsetZ != 0) && (labs(m_lBT2OffsetZ) < 200))
		{
			lLevel = lLevel + m_lBT2OffsetZ;
		}
	}

	CString szMsg;
	szMsg.Format("BH - BHZ1 on BT Enc %ld,%ld setup %ld + %s ==> %ld(%d)",
		lBT_X, lBT_Y, lBondLevel,  m_szLearnLevelBT, lLevel, bReturn);
	DisplaySequence(szMsg);

	return lLevel;
}

LONG CBondHead::GetZ2BondLevel(BOOL bAutoCycle, BOOL bUseBT)
{
	LONG lBTLevelOffset = 0;
	LONG lBT_X = 0;
	LONG lBT_Y = 0;
	m_szLearnLevelBT = "";
	//	get bhz1 tilting
//	BOOL bMoved= TRUE;
	if( bAutoCycle )
	{
//		bMoved= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["Bond Moved"];
		lBT_X = (*m_psmfSRam)["BinTable"]["Bond Posn X"];
		lBT_Y = (*m_psmfSRam)["BinTable"]["Bond Posn Y"];
		(*m_psmfSRam)["BinTable"]["Bond Moved"]		= FALSE;
	}
	else
	{
		BT_Get_XY_Posn(lBT_X, lBT_Y);
	}
	if (m_bUseMultiProbeLevel/* && bMoved*/)
	{
		lBTLevelOffset = GetBTLevelOffset(TRUE, lBT_X, lBT_Y);	//	get bhz2
	}

	BOOL bReturn = FALSE;
	LONG lBondLevel = GetBondPadLevel( TRUE, lBT_X, lBT_Y, bReturn);
	if( bReturn )
	{
		lBondLevel = lBondLevel + m_lWPadsLevelOffsetZ2;
	}

	LONG lLevel = lBondLevel + lBTLevelOffset;
	if( bAutoCycle )
	{
		lLevel = lLevel - m_lCurrOffsetZ2byPBEmptyCheck;
		LONG lBTInUse		= (*m_psmfSRam)["BinTable"]["BTInUse"];
		if ((lBTInUse == 1) && (m_lBT2OffsetZ != 0) && (labs(m_lBT2OffsetZ) < 200))
		{
			lLevel = lLevel + m_lBT2OffsetZ;		//v4.48A8
		}
	}

	CString szMsg;
	szMsg.Format("BH - BHZ2 on BT Enc %ld,%ld setup %ld + %s ==> %ld(%d)",
		lBT_X, lBT_Y, lBondLevel, m_szLearnLevelBT, lLevel, bReturn);
	DisplaySequence(szMsg);

	return lLevel;
}

LONG CBondHead::BH_DetectBinTableTilting(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	if( m_bDisableBH || IsWLExpanderOpen() || !m_bUseMultiProbeLevel )	//	share for BH to detect BT leveling.
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	INT nCount = 0;
	while (IsCoverOpen())
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);

		nCount++;
		if (nCount >= 3)
		{
			LONG lHmiStatus = HmiMessage("Cover sensor still open in CHANGE BH setup; continue to move T motor?", 
										 "BondHead Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
			if (lHmiStatus != glHMI_CONTINUE)
			{
				svMsg.InitMessage(sizeof(BOOL),	&bReturn);
				return TRUE;
			}
			else
			{
				break;
			}
		}
	}

	IPC_CServiceMessage stMsg;
	if (IsBLEnable() && !IsBT1FrameLevel())
	{
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
		SetErrorMessage("BT frame level not DOWN");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	m_comClient.SendRequest(WAFER_PR_STN, "SelectBondCamera", stMsg);
	Sleep(100);
	//	Both head to home and arm to prepick.
	Z_MoveToHome();
	if (m_bIsArm2Exist == TRUE)
	{
		Z2_MoveToHome();
	}
	if (m_bMS60EjElevator)
	{
		EjElv_MoveToHome();
	}
	T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF
	T_MoveTo(m_lPrePickPos_T);

	// Turn off bin table joystick firstly.
	BT_Set_JoyStick(FALSE);

	m_stM_B_L1.InitPLPoints();
	m_stM_B_L2.InitPLPoints();

	// Get physical bin block UL and LR corner.
	UCHAR ucLoopLimit = 1;
	if( IsMS90() )
		ucLoopLimit = 2;
	OpenWaitingAlert();
	DeleteFile(BH_BH_MBL);
	LONG ulBTGridRows = BH_BT_SAMPLE_POINTS-1, ulBTGridCols = BH_BT_SAMPLE_POINTS-1;
	LONG lLevelLimit = BH_BT_TILTING_LIMIT;
	if( IsMS90() )
	{
		lLevelLimit = MS90_BT_LVL_LIMIT;
	}

	BOOL bFail = FALSE;
for(UCHAR ucLoop=0; ucLoop<ucLoopLimit; ucLoop++)
{
	if( ucLoop==0 )
		SetAlarmLog("Begin to do sampling on BT");
	else
		SetAlarmLog("Second part to do sampling");
	BT_XY_STRUCT	stPosnUL, stPosnLR, stMoveBT;
	stPosnUL.lX = m_lMBL_UL_X;
	stPosnUL.lY = m_lMBL_UL_Y;
	stPosnLR.lX = m_lMBL_LR_X;
	stPosnLR.lY = m_lMBL_LR_Y;
	if( ucLoop>0 )
	{
		LONG lOldX = m_lMBL_LR_X;	//	LR
		LONG lOldY = m_lMBL_LR_Y;	//	LR
		RotateBinTable180(lOldX, lOldY, stPosnUL.lX, stPosnUL.lY);
		lOldX = m_lMBL_UL_X;
		lOldY = m_lMBL_UL_Y;
		RotateBinTable180(lOldX, lOldY, stPosnLR.lX, stPosnLR.lY);
	}
	SetAlarmLog("after get BT corner");
	LONG lPLSpanX = (stPosnLR.lX - stPosnUL.lX)/ulBTGridCols;
	LONG lPLSpanY = (stPosnLR.lY - stPosnUL.lY)/ulBTGridRows;
	stPosnLR.lX = stPosnUL.lX + lPLSpanX * ulBTGridCols;
	stPosnLR.lY = stPosnUL.lY + lPLSpanY * ulBTGridRows;

	CString szMsg;

	if( IsMS90BTRotated() && ucLoop==0 )
	{
		MS90RotateBT180(FALSE);
		SetAlarmLog("after rotate BT to 0 degree");
	}
	if( ucLoop==1 )
	{
		MS90RotateBT180(TRUE);
		SetAlarmLog("after rotate BT");
	}

	LONG lLowerZ = 0, lUpperZ = 0;
	T_MoveTo(m_lBondPos_T);
	for(LONG ulPLRow=0; ulPLRow<=ulBTGridRows; ulPLRow++)
	{
		for(LONG ulPLCol=0; ulPLCol<=ulBTGridCols; ulPLCol++)
		{
			// bin table move to sampling key point
			stMoveBT.lX = stPosnUL.lX + lPLSpanX*ulPLCol;
			stMoveBT.lY = stPosnUL.lY + lPLSpanY*ulPLRow;
			if (BT_XY_MoveTo(stMoveBT.lX, stMoveBT.lY))
			{
				bFail = TRUE;
				break;
			}

			// bond arm to bond and bond head 1 to auto detect the bond level.
			Sleep(100);
			if (AutoLearnBondLevelUsingCTSensor(3) == FALSE)
			{
				Z_MoveToHome();
				bFail = TRUE;
				break;
			}

			//	BHZ1 detect ok, get the reading.
			Sleep(100);
			GetEncoderValue();

			if( ulPLRow==0 && ulPLCol==0 )
			{
				lLowerZ = m_lEnc_Z;
				lUpperZ = m_lEnc_Z;
			}

			if( lLowerZ>m_lEnc_Z )
				lLowerZ = m_lEnc_Z;
			if( lUpperZ<m_lEnc_Z )
				lUpperZ = m_lEnc_Z;

			if( ucLoop==0 )
				m_stM_B_L1.SetPointPL(ulPLRow, ulPLCol, stMoveBT.lX, stMoveBT.lY, m_lEnc_Z, TRUE);
			else
				m_stM_B_L2.SetPointPL(ulPLRow, ulPLCol, stMoveBT.lX, stMoveBT.lY, m_lEnc_Z, TRUE);
			szMsg.Format("Order %d,%d BT %d,%d, BHZ %d", ulPLRow, ulPLCol, stMoveBT.lX, stMoveBT.lY, m_lEnc_Z);
			SetAlarmLog(szMsg);

			//	BHZ1 Up to swing level (better to home it in case motor error.
			Z_MoveToHome();
		}
		if( bFail )
		{
			break;
		}
	}

	// move bond arm to prepick.
	T_MoveTo(m_lPrePickPos_T);
	T_Profile(NORMAL_PROF);

	// move bin table to physical block center.
	stMoveBT.lX = (stPosnUL.lX + stPosnLR.lX)/2;
	stMoveBT.lY = (stPosnUL.lY + stPosnLR.lY)/2;
	if (BT_XY_MoveTo(stMoveBT.lX, stMoveBT.lY))
	{
		bFail = TRUE;
		break;
	}

	if( labs(lLowerZ-lUpperZ)>(lLevelLimit) )
	{
		szMsg.Format("BT level is not good(>%d).\nLow is %d and up is %d.", lLevelLimit, lLowerZ, lUpperZ);
		HmiMessage(szMsg, "BH Setup");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		bFail = TRUE;
	}

	LONG lPL_X = 0, lPL_Y = 0, lPL_Z = 0;
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, BH_BH_MBL, "a");
	if ((nErr == 0) && (fp != NULL))
	{
		if( ucLoop==1 )
		{
			fprintf(fp, "\n\nBin Table rotated 180 leveling\n");
		}
		CTime stTime = CTime::GetCurrentTime();
		CString szTime = stTime.Format("%Y:%m:%d   %H-%M-%S");
		fprintf(fp, "%s\n", szTime);
		fprintf(fp, "UL %ld,%ld\n", stPosnUL.lX, stPosnUL.lY);
		fprintf(fp, "LR %ld,%ld\n", stPosnLR.lX, stPosnLR.lY);
		fprintf(fp, "Span %ld,%ld\n", lPLSpanX, lPLSpanY);
		fprintf(fp, "\nsample data just after detection\n");
		for(LONG lRow=0; lRow<=ulBTGridRows; lRow++)
		{
			for(LONG lCol=0; lCol<=ulBTGridCols; lCol++)
			{
				LONG lState = m_stM_B_L1.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
				if( ucLoop==1 )
				{
					lState = m_stM_B_L2.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
				}
				fprintf(fp, " %ld", lState);
			}
			fprintf(fp, " \n");
		}
		fprintf(fp, "\n");
		for(LONG lRow=0; lRow<=ulBTGridRows; lRow++)
		{
			for(LONG lCol=0; lCol<=ulBTGridCols; lCol++)
			{
				LONG lState = m_stM_B_L1.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
				if( ucLoop==1 )
					lState = m_stM_B_L2.GetPointPL(lRow, lCol, lPL_X, lPL_Y, lPL_Z);
				fprintf(fp, " %2ld,%2ld,%8ld,%8ld,%8ld\n", lRow, lCol, lPL_X, lPL_Y, lPL_Z);
			}
		}
		fprintf(fp, "BT level tolerance %d\n", lLevelLimit);
		fprintf(fp, "Low is %d, up is %d, difference is %d\n",
			lLowerZ, lUpperZ, lUpperZ-lLowerZ);
		fprintf(fp, "BT tilting sample result is %d\n", !bFail);
		fclose(fp);
	}
	if( bFail )
	{
		break;
	}
}

	if( IsMS90() )
	{
		MS90RotateBT180(IsMS90BTRotated());
	}

	CloseWaitingAlert();

	if( bFail==FALSE )
	{
		HmiMessage_Red_Back("Auto Detect bin table multi levels success!", "BH setup");
		m_stM_B_L1.SetPLState(TRUE);
		if( IsMS90() )
		{
			m_stM_B_L2.SetPLState(TRUE);
		}

		SaveBhData();
		SaveFileHWD();
		BH_MBL_LogLevel();	//	after detect tilting
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

BOOL CBondHead::InitVariablesHWD()
{
	m_stM_B_L1.InitPLPoints();
	m_stM_B_L2.InitPLPoints();
	m_lMBL_UL_X	= 0;
	m_lMBL_UL_Y	= 0;
	m_lMBL_LR_X	= 0;
	m_lMBL_LR_Y	= 0;
	return TRUE;
}

BOOL CBondHead::LoadFileHWD()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	if( pUtl->IsHWDFileExist()==FALSE )
	{
		return FALSE;
	}

	// open config file
	if (pUtl->LoadHWDFile() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetHWDFile();
	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	CString szData;
	BOOL	bPLState = (BOOL)(LONG)(*psmf)[PB_PROCESS][PB_MULTI_PL_STATE];
	m_stM_B_L1.SetPLState(bPLState);
	for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			szData.Format("R%02dC%02dX", ulRow, ulCol);
			LONG lScanX = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			szData.Format("R%02dC%02dY", ulRow, ulCol);
			LONG lScanY = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			szData.Format("R%02dC%02dZ", ulRow, ulCol);
			LONG lPL_Z = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			szData.Format("R%02dC%02dR", ulRow, ulCol);
			LONG lState = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			m_stM_B_L1.SetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z, lState);
		}
	}
	bPLState = (BOOL)(LONG)(*psmf)[PB_PROCESS][BH_MULTI_BL_STATE_2];
	m_stM_B_L2.SetPLState(bPLState);
	for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			szData.Format("R%02dC%02dX", ulRow, ulCol);
			LONG lScanX = (*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData];
			szData.Format("R%02dC%02dY", ulRow, ulCol);
			LONG lScanY = (*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData];
			szData.Format("R%02dC%02dZ", ulRow, ulCol);
			LONG lPL_Z = (*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData];
			szData.Format("R%02dC%02dR", ulRow, ulCol);
			LONG lState = (*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData];
			m_stM_B_L2.SetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z, lState);
		}
	}

	m_lMBL_UL_X = (*psmf)[PB_PROCESS][BH_MULTI_BL_UL_X];
	m_lMBL_UL_Y = (*psmf)[PB_PROCESS][BH_MULTI_BL_UL_Y];
	m_lMBL_LR_X = (*psmf)[PB_PROCESS][BH_MULTI_BL_LR_X];
	m_lMBL_LR_Y = (*psmf)[PB_PROCESS][BH_MULTI_BL_LR_Y];

	m_oEjectorRecord.m_szEjtRemarkOld	= (*psmf)[BH_HW_RECORD][BH_HW_EJECTOR_OLD_REMARK];
	m_oEjectorRecord.m_szEjtTypeOld		= (*psmf)[BH_HW_RECORD][BH_HW_EJECTOR_OLD_TYPE];
	m_oColletRecord.m_szRemarkOld	= (*psmf)[BH_HW_RECORD][BH_HW_COLLET_OLD_REMARK];
	m_oColletRecord.m_szTypeOld		= (*psmf)[BH_HW_RECORD][BH_HW_COLLET_OLD_TYPE];
	m_oColletRecord.m_szCollet1Mode = (*psmf)[BH_HW_RECORD][BH_HW_COLLET_1_OLD_MODE];
	m_oColletRecord.m_szCollet2Mode = (*psmf)[BH_HW_RECORD][BH_HW_COLLET_2_OLD_MODE];
	m_oColletRecord.m_szShiftNo		= (*psmf)[BH_HW_RECORD][BH_HW_COLLET_SHIFT_NO];

	pUtl->ShutHWDFile();
	return TRUE;
}

BOOL CBondHead::SaveFileHWD()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadHWDFile() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetHWDFile();
	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	CString szData;
	(*psmf)[PB_PROCESS][PB_MULTI_PL_STATE]	= m_stM_B_L1.GetPLState();
	for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			LONG lScanX, lScanY, lPL_Z;
			LONG lState = m_stM_B_L1.GetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z);
			szData.Format("R%02dC%02dX", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lScanX;
			szData.Format("R%02dC%02dY", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lScanY;
			szData.Format("R%02dC%02dZ", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lPL_Z;
			szData.Format("R%02dC%02dR", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lState;
		}
	}
	(*psmf)[PB_PROCESS][BH_MULTI_BL_STATE_2]	= m_stM_B_L2.GetPLState();
	for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			LONG lScanX, lScanY, lPL_Z;
			LONG lState = m_stM_B_L2.GetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z);
			szData.Format("R%02dC%02dX", ulRow, ulCol);
			(*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData]	= lScanX;
			szData.Format("R%02dC%02dY", ulRow, ulCol);
			(*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData]	= lScanY;
			szData.Format("R%02dC%02dZ", ulRow, ulCol);
			(*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData]	= lPL_Z;
			szData.Format("R%02dC%02dR", ulRow, ulCol);
			(*psmf)[PB_PROCESS][BH_MULTI_BL_TABLE_2][szData]	= lState;
		}
	}

	(*psmf)[PB_PROCESS][BH_MULTI_BL_UL_X] = m_lMBL_UL_X;
	(*psmf)[PB_PROCESS][BH_MULTI_BL_UL_Y] = m_lMBL_UL_Y;
	(*psmf)[PB_PROCESS][BH_MULTI_BL_LR_X] = m_lMBL_LR_X;
	(*psmf)[PB_PROCESS][BH_MULTI_BL_LR_Y] = m_lMBL_LR_Y;

	(*psmf)[BH_HW_RECORD][BH_HW_EJECTOR_OLD_REMARK]	= m_oEjectorRecord.m_szEjtRemarkOld;
	(*psmf)[BH_HW_RECORD][BH_HW_EJECTOR_OLD_TYPE]	= m_oEjectorRecord.m_szEjtTypeOld;
	(*psmf)[BH_HW_RECORD][BH_HW_COLLET_OLD_REMARK]	= m_oColletRecord.m_szRemarkOld;
	(*psmf)[BH_HW_RECORD][BH_HW_COLLET_OLD_TYPE]	= m_oColletRecord.m_szTypeOld;
	(*psmf)[BH_HW_RECORD][BH_HW_COLLET_1_OLD_MODE]	= m_oColletRecord.m_szCollet1Mode;
	(*psmf)[BH_HW_RECORD][BH_HW_COLLET_2_OLD_MODE]	= m_oColletRecord.m_szCollet2Mode;
	(*psmf)[BH_HW_RECORD][BH_HW_COLLET_SHIFT_NO]	= m_oColletRecord.m_szShiftNo;

	pUtl->SaveHWDFile();
	return TRUE;
}

BOOL CBondHead::BH_MBL_LogLevel()
{
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, BH_BH_MBL, "a");

	LONG lBondZ = 0;
	LONG lBT_LearnX = m_lZ1BondLevelBT_X;
	LONG lBT_LearnY = m_lZ1BondLevelBT_Y;
	BOOL bBT_LearnR = m_bZ1BondLevelBT_R;

	LONG lBTX = lBT_LearnX, lBTY = lBT_LearnY;
	BH_BT_CalculateLevel(bBT_LearnR, lBTX, lBTY, lBondZ);
	if ((nErr == 0) && (fp != NULL))
	{
		CTime stTime = CTime::GetCurrentTime();
		CString szTime = stTime.Format("%Y:%m:%d   %H-%M-%S");
		fprintf(fp, "\n%s\n", szTime);
		fprintf(fp, "BHZ1 at BT %ld,%ld R=%d, get bond level %ld\n", lBTX, lBTY, bBT_LearnR, lBondZ);
	}
	RotateBinTable180(lBT_LearnX, lBT_LearnY, lBTX, lBTY);	//	rotate 180 degree to get BT XY
	BH_BT_CalculateLevel(!bBT_LearnR, lBTX, lBTY, lBondZ);

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "BHZ1 at BT %ld,%ld R=%d, get bond level %ld\n", lBTX, lBTY, !bBT_LearnR, lBondZ);
	}

	lBT_LearnX = m_lZ2BondLevelBT_X;
	lBT_LearnY = m_lZ2BondLevelBT_Y;
	bBT_LearnR = m_bZ2BondLevelBT_R;

	lBTX = lBT_LearnX;
	lBTY = lBT_LearnY;
	BH_BT_CalculateLevel(bBT_LearnR, lBTX, lBTY, lBondZ);

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "BHZ2 at BT %ld,%ld R=%d, get bond level %ld\n", lBTX, lBTY, bBT_LearnR, lBondZ);
	}
	RotateBinTable180(lBT_LearnX, lBT_LearnY, lBTX, lBTY);	//	rotate 180 degree to get BT XY
	BH_BT_CalculateLevel(!bBT_LearnR, lBTX, lBTY, lBondZ);

	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "BHZ2 at BT %ld,%ld R=%d, get bond level %ld\n", lBTX, lBTY, !bBT_LearnR, lBondZ);
		fclose(fp);
	}

	return TRUE;
}

BOOL CBondHead::MS90RotateBT180(BOOL bTo180)
{
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bTo180);
	INT	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MS90RotateBT180", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return TRUE;
}

LONG CBondHead::BH_MBL_GetBondLevel(IPC_CServiceMessage &stMsg)
{
	CString szMsg;
	CStringList szSelection;
	LONG lResult;
	szSelection.AddTail("Bond Head 1 Bond Level");
	szSelection.AddTail("Bond Head 2 Bond Level");
	lResult = HmiSelection("Please select bond head.", "MBL test", szSelection, 0);
	if( lResult==1 )
	{
		szMsg.Format("Bond Head 2 calculated bond level is %d", GetZ2BondLevel(FALSE, TRUE));
		HmiMessage(szMsg, "MBL Test");
	}
	else if( lResult==0 )
	{
		szMsg.Format("Bond Head 1 calculated bond level is %d", GetZ1BondLevel(FALSE, TRUE));
		HmiMessage(szMsg, "MBL Test");
	}

	BOOL bReturn = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ToggleLearnPadsLevel(IPC_CServiceMessage &svMsg)
{
	BOOL bStart = FALSE;
	svMsg.InitMessage(sizeof(BOOL), &bStart);

	if( bStart )
	{
		m_lWPadsLevelOffsetZ1 = 0;
		m_lWPadsLevelOffsetZ2 = 0;
		SaveBhData();
	}

	return 1;
}

LONG CBondHead::SetBTLevelSampleArea(IPC_CServiceMessage& svMsg)
{
	BT_XY_STRUCT	stPosnUL, stPosnLR;
	IPC_CServiceMessage stMsg;

	if( m_lMBL_UL_X==0 && m_lMBL_UL_Y==0 )
	{
		LONG lCorner = 0;
		stMsg.InitMessage(sizeof(LONG), &lCorner);
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BT_GetPhyBinBlockCorner", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPosnUL);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		m_lMBL_UL_X = stPosnUL.lX;
		m_lMBL_UL_Y = stPosnUL.lY;
	}
	if( m_lMBL_LR_X==0 && m_lMBL_LR_Y==0 )
	{
		LONG lCorner = 1;
		stMsg.InitMessage(sizeof(LONG), &lCorner);
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BT_GetPhyBinBlockCorner", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPosnLR);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		m_lMBL_LR_X = stPosnLR.lX;
		m_lMBL_LR_Y = stPosnLR.lY;
	}

	CString szMsg;
	szMsg.Format("Upper-Left(x=%d, y=%d) Right Bottom(x=%d, y=%d)\nReset data?", m_lMBL_UL_X, m_lMBL_UL_Y, m_lMBL_LR_X, m_lMBL_LR_Y);
	LONG lTmp = HmiMessageEx(szMsg, "Confirm",
		glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300,0,0,0,0, 300, 300);

	if (lTmp==glHMI_OK)
	{
		m_lMBL_UL_X = m_lMBL_UL_Y = 0;
		m_lMBL_LR_X = m_lMBL_LR_Y = 0;
	}

	BT_Set_JoyStick(FALSE);
	if (BT_XY_MoveTo(m_lMBL_UL_X, m_lMBL_UL_Y))
	{
		BOOL bReturn = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	BT_Set_JoyStick(TRUE);
	szMsg = "Please move to up-left corner with joystick!";
	lTmp = HmiMessageEx(szMsg, "prescan",
		glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300,0,0,0,0, 300, 300);

	if( lTmp==glHMI_OK )
	{
		BT_Get_XY_Posn(m_lMBL_UL_X, m_lMBL_UL_Y);
		BT_Set_JoyStick(FALSE);
		if (BT_XY_MoveTo(m_lMBL_LR_X, m_lMBL_LR_Y))
		{
			BOOL bReturn = FALSE;
			stMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		BT_Set_JoyStick(TRUE);
		szMsg = "Please move to low-right corner with joystick!";
		lTmp = HmiMessageEx(szMsg, "prescan",
			glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300,0,0,0,0, 300, 300);
		if( lTmp==glHMI_OK )
		{
			BT_Get_XY_Posn(m_lMBL_LR_X, m_lMBL_LR_Y);
			BT_Set_JoyStick(FALSE);
			LONG lCtrX = (m_lMBL_UL_X + m_lMBL_LR_X)/2;
			LONG lCtrY = (m_lMBL_UL_Y + m_lMBL_LR_Y)/2;
			if (BT_XY_MoveTo(lCtrX, lCtrY))
			{
				BOOL bReturn = FALSE;
				stMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}
	}
	SaveFileHWD();
	BOOL bReturn = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BT_XY_MoveTo(LONG lX, LONG lY)
{
	IPC_CServiceMessage stMsg;

	BT_XY_STRUCT	stMoveBT;
	stMoveBT.lX = lX;
	stMoveBT.lY = lY;
	stMsg.InitMessage(sizeof(BT_XY_STRUCT), &stMoveBT);
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "XY_MoveToCmd", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	Sleep(100);

	LONG lResult;
	stMsg.GetMsg(sizeof(LONG), &lResult);
	return lResult;
}

LONG CBondHead::BT_Get_XY_Posn(LONG &lBTX, LONG &lBTY)
{
	BT_XY_STRUCT	stPosnBT;
	stPosnBT.lX = 0;
	stPosnBT.lY = 0;
	//	get bin table current position X and Y.
	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "BT_GetCurrPosnInZero", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BT_XY_STRUCT), &stPosnBT);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	lBTX = stPosnBT.lX;
	lBTY = stPosnBT.lY;
	return TRUE;
}

LONG CBondHead::BT_Set_JoyStick(BOOL bEnable)
{
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bEnable);
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetJoystickCmd", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
	Sleep(500);

	return TRUE;
}

BOOL CBondHead::SetBondPadLevel(CONST BOOL bBHZ2)
{
	if( !CMS896AStn::m_bEnableWafflePadBonding )	//	store bond level & BT position when learn
	{
		return TRUE;
	}

	BOOL bAllDone = TRUE;
	for(INT i=0; i<m_lWPadsNum; i++)
	{
		if( m_stPadLevels[i].m_bZ2Learnt==FALSE || m_stPadLevels[i].m_bZ1Learnt==FALSE )
		{
			bAllDone = FALSE;
			break;
		}
		if( m_stPadLevels[i+MAX_WAFFLE_PADS/2].m_bZ2Learnt==FALSE || 
			m_stPadLevels[i+MAX_WAFFLE_PADS/2].m_bZ1Learnt==FALSE )
		{
			bAllDone = FALSE;
			break;
		}
	}

	LONG lX = 0, lY = 0;
	BT_Get_XY_Posn(lX, lY);
	if( m_bBeginPadsLevel==FALSE && bAllDone )
	{
		LONG lOffset = 0;
		BOOL bReturn = FALSE;
		LONG lBondLevel = GetBondPadLevel(bBHZ2, lX, lY, bReturn);
		if( bBHZ2 )
		{
			if( bReturn )
			{
				m_lWPadsLevelOffsetZ2 = m_lBondLevel_Z2 - lBondLevel;
			}
			lOffset = m_lWPadsLevelOffsetZ2;
		}
		else
		{
			if( bReturn )
			{
                m_lWPadsLevelOffsetZ1 = m_lBondLevel_Z - lBondLevel;
			}
			lOffset = m_lWPadsLevelOffsetZ1;
		}

		FILE *fp = NULL;
		errno_t nErr = fopen_s(&fp, BH_BH_MBL, "a");
		if ((nErr == 0) && (fp != NULL))
		{
			CString szLog;
			szLog.Format("Bond Level BHZ%d new offset %d", bBHZ2+1, lOffset);
			fprintf(fp, "%s\n", szLog);
			fclose(fp);
		}
		return TRUE;
	}

	CString szLog;
	if( IsMS90BTRotated() )
	{
		for(INT i=MAX_WAFFLE_PADS/2; i<MAX_WAFFLE_PADS; i++)	//	set level 180 degree
		{
			if( m_stPadLevels[i].m_bDefined )
			{
				CString szTemp;
				szTemp.Format(",Waffle Pad area UL %ld,%ld, LR %ld,%ld", 
					m_stPadLevels[i].m_lPadULX, m_stPadLevels[i].m_lPadULY,
					m_stPadLevels[i].m_lPadLRX, m_stPadLevels[i].m_lPadLRY);

				if( m_stPadLevels[i].m_lPadULX>=lX && lX>=m_stPadLevels[i].m_lPadLRX &&
					m_stPadLevels[i].m_lPadULY>=lY && lY>=m_stPadLevels[i].m_lPadLRY )
				{
					if( bBHZ2 )
					{
						m_stPadLevels[i].m_bZ2Learnt	= TRUE;
						m_stPadLevels[i].m_lZ2PadBT_X	= lX;
						m_stPadLevels[i].m_lZ2PadBT_Y	= lY;
						m_stPadLevels[i].m_lZ2BondLevel	= m_lBondLevel_Z2;
						szLog.Format("BHZ2 pad %ld BT %ld,%ld, Level %ld", i+1-MAX_WAFFLE_PADS/2, lX, lY, m_lBondLevel_Z2);
					}
					else
					{
						m_stPadLevels[i].m_bZ1Learnt	= TRUE;
						m_stPadLevels[i].m_lZ1PadBT_X	= lX;
						m_stPadLevels[i].m_lZ1PadBT_Y	= lY;
						m_stPadLevels[i].m_lZ1BondLevel = m_lBondLevel_Z;
						szLog.Format("BHZ1 pad %ld BT %ld,%ld, Level %ld", i+1-MAX_WAFFLE_PADS/2, lX, lY, m_lBondLevel_Z);
					}
					szLog += szTemp;
					break;
				}
			}
		}
	}
	else
	{
		for(INT i=0; i<MAX_WAFFLE_PADS/2; i++)	//	set level 0 degree
		{
			if( m_stPadLevels[i].m_bDefined )
			{
				CString szTemp;
				szTemp.Format(",Waffle Pad area UL %ld,%ld, LR %ld,%ld", 
					m_stPadLevels[i].m_lPadULX, m_stPadLevels[i].m_lPadULY,
					m_stPadLevels[i].m_lPadLRX, m_stPadLevels[i].m_lPadLRY);

				if( m_stPadLevels[i].m_lPadULX>=lX && lX>=m_stPadLevels[i].m_lPadLRX &&
					m_stPadLevels[i].m_lPadULY>=lY && lY>=m_stPadLevels[i].m_lPadLRY )
				{
					if( bBHZ2 )
					{
						m_stPadLevels[i].m_bZ2Learnt	= TRUE;
						m_stPadLevels[i].m_lZ2PadBT_X	= lX;
						m_stPadLevels[i].m_lZ2PadBT_Y	= lY;
						m_stPadLevels[i].m_lZ2BondLevel	= m_lBondLevel_Z2;
						szLog.Format("BHZ2 pad %ld BT %ld,%ld, Level %ld", i+1, lX, lY, m_lBondLevel_Z2);
					}
					else
					{
						m_stPadLevels[i].m_bZ1Learnt	= TRUE;
						m_stPadLevels[i].m_lZ1PadBT_X	= lX;
						m_stPadLevels[i].m_lZ1PadBT_Y	= lY;
						m_stPadLevels[i].m_lZ1BondLevel = m_lBondLevel_Z;
						szLog.Format("BHZ1 pad %ld BT %ld,%ld, Level %ld", i+1, lX, lY, m_lBondLevel_Z);
					}
					szLog += szTemp;
					break;
				}
			}
		}
	}

	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, BH_BH_MBL, "a");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%s\n", szLog);
		fclose(fp);
	}

	return TRUE;
}

LONG CBondHead::GetBondPadLevel(CONST BOOL bBHZ2, LONG lBT_X, LONG lBT_Y, BOOL &bReturn)
{
	LONG lBondLevel = m_lBondLevel_Z;
	if( bBHZ2 )
		lBondLevel = m_lBondLevel_Z2;

	bReturn = FALSE;
	if( !m_bEnableWafflePadBonding  )	//	Get bond level
	{
		return lBondLevel;
	}

	LONG lX = lBT_X;
	LONG lY = lBT_Y;

	CString szTemp = " not defined in pad area";
	if( IsMS90BTRotated() )
	{
		m_szLearnLevelBT += " BT 180;";
	}	//	MS90 BT rotated.
	else
	{
		m_szLearnLevelBT += " BT 000;";
	}
	szTemp.Format(" BT %d,%d, ", lX, lY);
	m_szLearnLevelBT += szTemp;

	if( IsMS90BTRotated() )
	{
		for(INT i=MAX_WAFFLE_PADS/2; i<MAX_WAFFLE_PADS; i++)	//	get 180 degree
		{
			if( m_stPadLevels[i].m_bDefined )
			{
				if( m_stPadLevels[i].m_lPadULX>=lX && lX>=m_stPadLevels[i].m_lPadLRX &&
					m_stPadLevels[i].m_lPadULY>=lY && lY>=m_stPadLevels[i].m_lPadLRY )
				{
					if( bBHZ2 )
					{
						if( m_stPadLevels[i].m_bZ2Learnt )
						{
							lBondLevel = m_stPadLevels[i].m_lZ2BondLevel;
							bReturn = TRUE;
						}
					}
					else 
					{
						if( m_stPadLevels[i].m_bZ1Learnt )
						{
							lBondLevel = m_stPadLevels[i].m_lZ1BondLevel;
							bReturn = TRUE;
						}
					}
					szTemp.Format(" get bond level %d", lBondLevel);
					m_szLearnLevelBT += szTemp;
					break;
				}
			}
		}
	}
	else
	{
		for(INT i=0; i<MAX_WAFFLE_PADS/2; i++)	//	get 0 degree
		{
			if( m_stPadLevels[i].m_bDefined )
			{
				if( m_stPadLevels[i].m_lPadULX>=lX && lX>=m_stPadLevels[i].m_lPadLRX &&
					m_stPadLevels[i].m_lPadULY>=lY && lY>=m_stPadLevels[i].m_lPadLRY )
				{
					if( bBHZ2 )
					{
						if( m_stPadLevels[i].m_bZ2Learnt )
						{
							lBondLevel = m_stPadLevels[i].m_lZ2BondLevel;
							bReturn = TRUE;
						}
					}
					else 
					{
						if( m_stPadLevels[i].m_bZ1Learnt )
						{
							lBondLevel = m_stPadLevels[i].m_lZ1BondLevel;
							bReturn = TRUE;
						}
					}
					szTemp.Format(" get bond level %d", lBondLevel);
					m_szLearnLevelBT += szTemp;
					break;
				}
			}
		}
	}

	return lBondLevel;
}

LONG CBondHead::SetBondPadsArea(IPC_CServiceMessage& svMsg)
{
	if (!CMS896AStn::m_bEnableWafflePadBonding)
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	m_lWPadsNum = 0;
	CString szLogFile = "C:\\MapSorter\\UserData\\History\\WafflePad000.txt";
	CString szMsg;
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFile, "rt");
	if ((nErr == 0) && (fp != NULL))
	{
		LONG lULX = 0, lULY = 0, lLRX = 0, lLRY = 0;
		INT i=0;
		while( feof(fp)==0 )	// not end of file
		{
			fscanf(fp, "%ld,%ld,%ld,%ld\n", &lULX, &lULY, &lLRX, &lLRY);
			m_stPadLevels[i].m_bDefined = TRUE;
			m_stPadLevels[i].m_lPadLRX	= lLRX;
			m_stPadLevels[i].m_lPadLRY	= lLRY;
			m_stPadLevels[i].m_lPadULX	= lULX;
			m_stPadLevels[i].m_lPadULY	= lULY;
			i++;
			m_lWPadsNum++;
			if( i>=MAX_WAFFLE_PADS/2 )	//	set area 0 degree
			{
				break;
			}
		}
	}
	fclose(fp);

	if (IsMS90())
	{
		CString szLogFile = "C:\\MapSorter\\UserData\\History\\WafflePad180.txt";
		CString szMsg;
		FILE *fp = NULL;
		errno_t nErr = fopen_s(&fp, szLogFile, "rt");
		if ((nErr == 0) && (fp != NULL))
		{
			LONG lULX = 0, lULY = 0, lLRX = 0, lLRY = 0;
			INT i=MAX_WAFFLE_PADS/2;	//	set area 180 degree
			while( feof(fp)==0 )	// not end of file
			{
				fscanf(fp, "%ld,%ld,%ld,%ld\n", &lULX, &lULY, &lLRX, &lLRY);
				m_stPadLevels[i].m_bDefined = TRUE;
				m_stPadLevels[i].m_lPadLRX	= lLRX;
				m_stPadLevels[i].m_lPadLRY	= lLRY;
				m_stPadLevels[i].m_lPadULX	= lULX;
				m_stPadLevels[i].m_lPadULY	= lULY;
				i++;
				if( i>=MAX_WAFFLE_PADS )	//	set area 180 degree
				{
					break;
				}
			}
		}
		fclose(fp);
	}	//	180 degree
	
	fp = NULL;
	nErr = fopen_s(&fp, BH_BH_MBL, "wt");
	for(INT i=0; i<MAX_WAFFLE_PADS; i++)	//	log whole for checking.
	{
		if( m_stPadLevels[i].m_bDefined )
		{
			INT nPad = i+1;
			if( i>=MAX_WAFFLE_PADS/2 && fp!=NULL )
			{
				nPad = i+1-MAX_WAFFLE_PADS/2;
				szMsg.Format("BT 180 degree rotation area and level");
				if ((nErr == 0) && (fp != NULL))
				{
					fprintf(fp, "%s\n", szMsg);
				}
			}
			CString szTemp;
			szMsg.Format("Waffle Pad %d area UL %ld,%ld, LR %ld,%ld; ", nPad,
				m_stPadLevels[i].m_lPadULX, m_stPadLevels[i].m_lPadULY,
				m_stPadLevels[i].m_lPadLRX, m_stPadLevels[i].m_lPadLRY);
			szTemp.Format("BHZ1 Learn %d, BT %ld,%ld, Level %ld; ", 
				m_stPadLevels[i].m_bZ1Learnt, m_stPadLevels[i].m_lZ1PadBT_X, m_stPadLevels[i].m_lZ1PadBT_Y,
				m_stPadLevels[i].m_lZ1BondLevel);
			szMsg += szTemp;
			szTemp.Format("BHZ2 Learn %d, BT %ld,%ld, Level %ld", 
				m_stPadLevels[i].m_bZ2Learnt, m_stPadLevels[i].m_lZ2PadBT_X, m_stPadLevels[i].m_lZ2PadBT_Y,
				m_stPadLevels[i].m_lZ2BondLevel);
			szMsg += szTemp;
			if ((nErr == 0) && (fp != NULL))
			{
				fprintf(fp, "%s\n", szMsg);
			}
		}
	}
	if ((nErr == 0) && (fp != NULL))
	{
		fclose(fp);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}
