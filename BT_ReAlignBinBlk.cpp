/////////////////////////////////////////////////////////////////////
// BT_ReAlignBinBlk.cpp : Realign functions of the CBinTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Barry Chu
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_Constant.h"
#include "BondPr.h"
#include <iostream>
#include "PRFailureCaseLog.h"
#include "BinLoader.h"
#include "WaferPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// NEW ADDED REALIGN BIN BLOCK
LONG CBinTable::GetBinLoadTestState(IPC_CServiceMessage &svMsg)
{
	svMsg.InitMessage(sizeof(BOOL), &m_bBinLoadState);

	return 1;
}

LONG CBinTable::SetBinLoadTestState(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(BOOL), &m_bBinLoadState);

	return 1;
}


LONG CBinTable::MoveToNextLastDiePosn()
{
	ULONG ulBlkInUse = GetBTCurrentBlock();

	if (ulBlkInUse == 0)
	{
		return FALSE;
	}
	LONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ULONG ulDieIndex = 0;
	LONG lPosX = 0, lPosY = 0;

	if (ulNoOfSortedDie == 0)
	{
		LONG lPosX = 0, lPosY = 0;
		ulDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
	}
	else
	{
		//================================the position of next bonding die======================================
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + 1;
		//======================================================================================================
	}
	GetDieLogicalEncoderValue(ulBlkInUse, ulDieIndex, lPosX, lPosY);
	if (BT_MoveTo(lPosX, lPosY, TRUE) == FALSE)
	{

		return FALSE;
	}

	return TRUE;
}


LONG CBinTable::BT_MoveTo(LONG lX, LONG lY, CONST BOOL bWait, BOOL bUseBT2)
{
	CString str;
	INT nProf_X, nProf_Y;
	LONG lDiff_X, lDiff_Y;

//	if (bUseBT2)	//v4.17T5
//	{
//		return BT2_MoveTo(lX, lY, bWait);
//	}

	if (lX > m_lTableXPosLimit || lX < m_lTableXNegLimit)
	{
		str.Format("BT_MoveTo X : %d would hit position limit(%d--%d)", lX, m_lTableXPosLimit, m_lTableXNegLimit);
		SetErrorMessage(str);	//v4.57A5
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}
	if (lY > m_lTableYPosLimit || lY < m_lTableYNegLimit)
	{
		str.Format("BT_MoveTo Y : %d would hit position limit(%d--%d)", lY, m_lTableYPosLimit, m_lTableYNegLimit);
		SetErrorMessage(str);	//v4.57A5
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	if (m_bUseDualTablesOption && IsTable2InBondRegion())
	{
		str = "ERROR (BT_MoveTo): BT2 in BOND region; BT1 cannot move!";
		SetErrorMessage(str);
		HmiMessage(str);
		return FALSE;
	}


	GetEncoderValue();

	lDiff_X = labs(m_lEnc_X - lX);
	lDiff_Y = labs(m_lEnc_Y - lY);

	//v3.71T5		//PLLM REBEL
	if (lDiff_X <= m_lMinTravel_X)
	{
		nProf_X = FAST_PROF;
	}
	else
	{
		nProf_X = NORMAL_PROF;
	}

	//if (lDiff_Y > 10000)
	//	nProf_Y = LOW_PROF;
	if (lDiff_Y <= m_lMinTravel_Y)
	{
		nProf_Y = FAST_PROF;
	}
	else
	{
		nProf_Y = NORMAL_PROF;
	}

//#ifdef NU_MOTION
	//v3.83
	nProf_X = LOW_PROF;
	nProf_Y = LOW_PROF;
//#endif

	X_Profile(nProf_X);
	Y_Profile(nProf_Y);

	//v2.93T2
	if (!bWait)
	{
		XY_MoveTo(lX, lY, SFM_NOWAIT);
	}
	else
	{
		XY_MoveTo(lX, lY);
	}

	//Y_SelectControl(BT_DYN_CTRL);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	return TRUE;
}

VOID CBinTable::GetXYEncoderValue(LONG &lX, LONG &lY, BOOL bUseBT2)
{
	Sleep(20);

	lX = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_X, 1, &m_stBTAxis_X);
	lY = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_Y, 1, &m_stBTAxis_Y);
}

LONG CBinTable::ConvertXEncoderValueToFileUnit(LONG lXEncoderValue)
{
	LONG lConvertedValue;

	if (lXEncoderValue > 0)
	{
		lConvertedValue = (LONG)(((DOUBLE)lXEncoderValue / m_dXResolution) + 0.5);
	}
	else if (lXEncoderValue < 0)
	{
		lConvertedValue = (LONG)(((DOUBLE)lXEncoderValue / m_dXResolution) - 0.5);
	}
	else 
	{
		lConvertedValue = 0;
	}

	return lConvertedValue;
} //end ConvertXEncoderValueToFileUnit


LONG CBinTable::ConvertYEncoderValueToFileUnit(LONG lYEncoderValue)
{
	LONG lConvertedValue;

	if (lYEncoderValue > 0)
	{
		lConvertedValue = (LONG)(((DOUBLE)lYEncoderValue / m_dYResolution) + 0.5);
	}
	else if (lYEncoderValue < 0)
	{
		lConvertedValue = (LONG)(((DOUBLE)lYEncoderValue / m_dYResolution) - 0.5);
	}
	else
	{
		lConvertedValue = 0;
	}

	return lConvertedValue;
} //end ConvertYEncoderValueToFileUnit

VOID CBinTable::SwitchToBPR(VOID)
{
	IPC_CServiceMessage stMsg;
	BOOL bCamera = TRUE;
	int nConvID;
	
	// Get the reply
	stMsg.InitMessage(sizeof(BOOL), &bCamera);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SwitchToTarget", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
}


VOID CBinTable::SwitchToPbBPR(VOID)
{
	IPC_CServiceMessage stMsg;
	BOOL bCamera = TRUE;
	int nConvID;


	BOOL bPostBond = (BOOL)((LONG)(*m_psmfSRam)["BondPr"]["UsePostBond"]);
	if (!bPostBond)
	{
		(*m_psmfSRam)["BondPr"]["UsePostBond"] = TRUE;
	}

	// Get the reply
	stMsg.InitMessage(sizeof(BOOL), &bCamera);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SwitchToTarget", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	(*m_psmfSRam)["BondPr"]["UsePostBond"]	= bPostBond;
}


LONG CBinTable::CheckBPRStatus(VOID)
{
	BOOL bUsePr;
	int nConvID;
	IPC_CServiceMessage stMsg;

	// if want to get the state, scan reply, otherwise, just go
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_GetPRStatus", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bUsePr);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	if (bUsePr != FALSE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


DOUBLE CBinTable::GetBPRFOVSize(ULONG ulBlkInUse)
{
	DOUBLE dFOVSize = 0;
	int nConvID;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		LONG lPitchX;
		LONG lPitchY;
	} BT_PITCH;
	BT_PITCH stInfo;

	stInfo.lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	stInfo.lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));

	stMsg.InitMessage(sizeof(BT_PITCH), &stInfo);
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_GetFOVSize", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(DOUBLE), &dFOVSize);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	return dFOVSize;
}


//================================================================
// IsLargeDieSize()
//   Created-By  : Andrew Ng
//   Date        : 5/18/2007 9:54:34 AM
//   Description : 
//   Remarks     : 
//================================================================
BOOL CBinTable::IsLargeDieSize()
{
	int nConvID;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		LONG lX;
		LONG lY;
	} BT_DIESIZE;

	BT_DIESIZE stInfo;
	stInfo.lX = 0;
	stInfo.lY = 0;

	BOOL bStatus = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bStatus);
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_GetDieSize", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BT_DIESIZE), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	//Convert motor into mil	
	//double dXinMil=0, dYinMil=0;
	//dXinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * (DOUBLE)stInfo.lX * 1000 / 25.4);
	//dYinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y) * (DOUBLE)stInfo.lY * 1000 / 25.4);

	LONG lXinMil = stInfo.lX;
	LONG lYinMil = stInfo.lY;

//CString szTemp;
//szTemp.Format("Check LargeDie : X=%d, Y=%d", lXinMil, lYinMil);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);


	if ((lXinMil > 40) || (lYinMil > 40))
	{
		return TRUE;
	}
	return FALSE;
}

UCHAR CBinTable::GetRealignWithFirstCorner(const ULONG ulWalkPath, const ULONG ulDieRow, const ULONG ulDieCol)
{
	UCHAR ucCorner = REALIGN_LOWER_RIGHT_CORNER;
	switch(ulWalkPath)
	{
	case BT_TLH_PATH:  //BT_TLH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_LOWER_RIGHT_CORNER : REALIGN_LOWER_LEFT_CORNER;
		break;
	case BT_TRH_PATH:  //BT_TRH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_LOWER_LEFT_CORNER : REALIGN_LOWER_RIGHT_CORNER;
		break;
	case BT_BLH_PATH:  //BT_BLH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_UPPER_RIGHT_CORNER : REALIGN_UPPER_LEFT_CORNER;
		break;
	case BT_BRH_PATH:  //BT_BRH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_UPPER_LEFT_CORNER : REALIGN_UPPER_RIGHT_CORNER;
		break;
	
	case BT_TLV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_LOWER_RIGHT_CORNER : REALIGN_UPPER_RIGHT_CORNER;
		break;
	case BT_TRV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_UPPER_RIGHT_CORNER : REALIGN_LOWER_RIGHT_CORNER;
		break;
	case BT_BLV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_LOWER_LEFT_CORNER : REALIGN_UPPER_LEFT_CORNER;
		break;
	case BT_BRV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_UPPER_LEFT_CORNER : REALIGN_LOWER_LEFT_CORNER;
		break;
	}

	CString strTemp;
	if (ucCorner == REALIGN_UPPER_LEFT_CORNER)
	{
		strTemp.Format("Search --- UL Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}
	if (ucCorner ==  REALIGN_UPPER_RIGHT_CORNER)
	{
		strTemp.Format("Search --- UR Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}
	if (ucCorner == REALIGN_LOWER_LEFT_CORNER)
	{
		strTemp.Format("Search --- LL Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}
	if (ucCorner == REALIGN_LOWER_RIGHT_CORNER)
	{
		strTemp.Format("Search --- LR Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(strTemp, "a+");
	return ucCorner;
}

UCHAR CBinTable::GetRealignWithSecondCorner(const ULONG ulWalkPath, const ULONG ulDieRow, const ULONG ulDieCol)
{
	UCHAR ucCorner = REALIGN_LOWER_RIGHT_CORNER;
	switch(ulWalkPath)
	{
	case BT_TLH_PATH:  //BT_TLH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_LOWER_LEFT_CORNER : REALIGN_LOWER_RIGHT_CORNER;
		break;
	case BT_TRH_PATH:  //BT_TRH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_LOWER_RIGHT_CORNER : REALIGN_LOWER_LEFT_CORNER;
		break;
	case BT_BLH_PATH:  //BT_BLH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_UPPER_LEFT_CORNER : REALIGN_UPPER_RIGHT_CORNER;
		break;
	case BT_BRH_PATH:  //BT_BRH_PATH
		ucCorner = (ulDieRow %2 == 0) ? REALIGN_UPPER_RIGHT_CORNER : REALIGN_UPPER_LEFT_CORNER;
		break;
	
	case BT_TLV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_UPPER_RIGHT_CORNER : REALIGN_LOWER_RIGHT_CORNER;
		break;
	case BT_TRV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_LOWER_RIGHT_CORNER : REALIGN_UPPER_RIGHT_CORNER;
		break;
	case BT_BLV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_UPPER_LEFT_CORNER : REALIGN_LOWER_LEFT_CORNER;
		break;
	case BT_BRV_PATH:
		ucCorner = (ulDieCol %2 == 0) ? REALIGN_LOWER_LEFT_CORNER : REALIGN_UPPER_LEFT_CORNER;
		break;
	}

	CString strTemp;
	if (ucCorner == REALIGN_UPPER_LEFT_CORNER)
	{
		strTemp.Format("Search --- UL Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}
	if (ucCorner ==  REALIGN_UPPER_RIGHT_CORNER)
	{
		strTemp.Format("Search --- UR Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}
	if (ucCorner == REALIGN_LOWER_LEFT_CORNER)
	{
		strTemp.Format("Search --- LL Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}
	if (ucCorner == REALIGN_LOWER_RIGHT_CORNER)
	{
		strTemp.Format("Search --- LR Corner verify (%d,%d), ", ulDieRow, ulDieCol);
	}

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(strTemp, "a+");
	return ucCorner;
}

LONG CBinTable::MultiSearchFirstGoodDie(const ULONG ulBlkInUse, const ULONG ulDieRow, const ULONG ulDieCol, const BOOL bDoGlobalTheta) //4.51D20
{
	typedef struct 
	{
		UCHAR		ucCorner;
	} BPR_MULTI_INPUT;

	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
		INT		nDieStateRight;
		INT		nDieStateLeft; 
		INT		nDieStateUp;
		INT		nDieStateDown;
		DOUBLE	dRowFovSize;  //4.53D01
		DOUBLE	dColFovSize;  

	} BPR_MULTI_DIEOFFSET;

	
	BPR_MULTI_DIEOFFSET stInfo;
	CString szMsg;
	m_nMultiSeachCounter++;
	
	
	int nConvID = 0;
	LONG lX = 0, lY = 0, lPitchX = 0, lPitchY = 0;
	BOOL bEmptyDieInFov = FALSE;
	BOOL bIs1stDie		= FALSE;
	BOOL bDoubleCheckIs1stDie = FALSE; //20170824 Leo utilze the checking feature of 1st die
	LONG lCurX			= 0;
	LONG lCurY			= 0;

	BPR_MULTI_INPUT stInfoInput;
	IPC_CServiceMessage stMsg;

	Sleep(m_lAlignBinDelay);

	stInfo.bResult = FALSE;
	//20170824 Leo added checking for bRotate180 and walkpath
	BOOL bRotate180 = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"];
	ULONG ulWalkPath = (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"];

	GetEncoderValue();		// Get the encoder value from hardware
	(*m_psmfSRam)["BinTable"]["Current"]["X"] = m_lEnc_X;
	(*m_psmfSRam)["BinTable"]["Current"]["Y"] = m_lEnc_Y;
	
	UCHAR ucCorner = GetRealignWithFirstCorner(ulWalkPath, ulDieRow, ulDieCol);

	szMsg.Format("\n\n\n\t\t\tSearch --- Original EncCur(%d,%d)", m_lEnc_X, m_lEnc_Y); 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	stInfoInput.ucCorner = ucCorner;
	stMsg.InitMessage(sizeof(stInfoInput), &stInfoInput);

/*
	//Search any die on current BT position
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_MultiSearchFirstDie1", stMsg);

	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_MULTI_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
*/
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		pBondPr->BT_MultiSearchFirstDie1(stMsg);
	}
	stMsg.GetMsg(sizeof(BPR_MULTI_DIEOFFSET), &stInfo);

	//PRID is not learnt
	if (stInfo.bResult == IDS_BPR_DIENOTLEARNT)
	{
		return IDS_BPR_DIENOTLEARNT;
	}

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));

	LONG AllowedPitchX = 70; //50 20170824 Leo
	LONG AllowedPitchY = 70; //50 20170824 Leo

	szMsg.Format("Search(Step2) -- Temp 1st Die --- bResult:%d, Pitch(%d,%d), Fovsize(%f,%f), Step(%d,%d) vs AllowedPitch(%d,%d)", stInfo.bResult, lPitchX, lPitchY, stInfo.dRowFovSize, stInfo.dColFovSize, stInfo.siStepX, stInfo.siStepY, AllowedPitchX, AllowedPitchY ); 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	Sleep(100);
	GetXYEncoderValue(lX, lY);
	lCurX = lX + stInfo.siStepX; // For Comparing two final searched die moving whether same or not
	lCurY = lY + stInfo.siStepY;
	
	// Comparing the PR centre offset with Allowed pitch --- here is main idea that decide the 1st Die
	if (BPR_IsSensorZoomFFMode() && stInfo.bResult || ((m_lDoubleCheckFirstDie == 0) || (m_lDoubleCheckFirstDie == 2)) && !BPR_IsSensorZoomFFMode() &&
		abs(stInfo.siStepX) < AllowedPitchX && abs(stInfo.siStepY) < AllowedPitchY)
	{
		bIs1stDie = TRUE;

		//Save Final posn then moving to check outside FOV
		GetXYEncoderValue(lX, lY);
		m_lTempMultiSearchX = lX + stInfo.siStepX;
		m_lTempMultiSearchY = lY + stInfo.siStepY;
	}
	else if (m_lDoubleCheckFirstDie == 1)
	{
		if ((stInfo.nDieStateRight == 0 && stInfo.nDieStateLeft == 0 && stInfo.nDieStateUp == 0 && stInfo.nDieStateDown == 0) ||
			stInfo.bResult)
		{
			bIs1stDie = TRUE;
		}
		else
		{
			bIs1stDie = FALSE;
		}
	}
	else
	{
		bIs1stDie = FALSE;
	}

	//Only check empty die in FOV
	if (stInfo.nDieStateRight == 0 && stInfo.nDieStateLeft == 0	&& stInfo.nDieStateUp == 0 && stInfo.nDieStateDown == 0 )
	{
		bEmptyDieInFov = TRUE;
	}
	else
	{
		bEmptyDieInFov = FALSE;
	}

	//moving to check the outside FOV die
	if (!BPR_IsSensorZoomFFMode() && (m_lDoubleCheckFirstDie == 0) && bIs1stDie)
	{
		LONG lTempX = 0, lTempY = 0;
		DOUBLE dRowFov = 0, dColFov = 0;

		GetXYEncoderValue(lX, lY);
		//20170829 Leo to make sure jumping to the select corner could have die found by PR 
		if (stInfo.dRowFovSize < BPR_LF_SIZE)
		{
			dRowFov = stInfo.dRowFovSize/3; 
		}
		else
		{
			dRowFov = stInfo.dRowFovSize/2.5; 
		}
		if (stInfo.dColFovSize < BPR_LF_SIZE)
		{
			dColFov = stInfo.dColFovSize/3; 
		}
		else
		{
			dColFov = stInfo.dColFovSize/2.5; 
		}

		switch (ucCorner)
		{
			case REALIGN_UPPER_LEFT_CORNER:
				lTempX = lX + lPitchX*(1)*((LONG)dRowFov);
				lTempY = lY + lPitchY*(1)*((LONG)dColFov);
				 break;
			case REALIGN_UPPER_RIGHT_CORNER:
				lTempX = lX + lPitchX*(-1)*((LONG)dRowFov);
				lTempY = lY + lPitchY*(1)*((LONG)dColFov);
				 break;
			case REALIGN_LOWER_LEFT_CORNER:
				lTempX = lX + lPitchX*(1)*((LONG)dRowFov);
				lTempY = lY + lPitchY*(-1)*((LONG)dColFov);
				 break;
			case REALIGN_LOWER_RIGHT_CORNER:
				lTempX = lX + lPitchX*(-1)*((LONG)dRowFov);
				lTempY = lY + lPitchY*(-1)*((LONG)dColFov);
				break;
		}

		BT_MoveTo(lTempX ,lTempY );

		szMsg.Format("Rotate180?= (%d), Search(Check) --Enc(%d,%d), Fov(%d,%d), Move(%d,%d), Diff(%d,%d) over3",bRotate180,lX,lY,(LONG)dRowFov,(LONG)dColFov, lTempX, lTempY,lTempX -lX, lTempY - lY); 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

		m_lDoubleCheckFirstDie++;

		return MultiSearchFirstGoodDie(ulBlkInUse, ulDieRow, ulDieCol, bDoGlobalTheta);

	}
	else if ( m_lDoubleCheckFirstDie == 1 && !bIs1stDie)
	{
/*
		m_lDoubleCheckFirstDie = 0;
		szMsg.Format("Search --- After Cheacking,Have Dies, lastMove(%d,%d), CurMove(%d,%d)",m_lTempMultiSearchX,m_lTempMultiSearchY,lCurX,lCurY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

		if( abs(m_lTempMultiSearchX - lCurX) < 50 && abs(m_lTempMultiSearchY - lCurY) < 50)
		{
			bIs1stDie = TRUE;

			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search --- After Comparing movement, last and current MovePosn  is the same", "a+");

		}
		bDoubleCheckIs1stDie = TRUE;

		bIs1stDie = bDoubleCheckIs1stDie;
		szMsg.Format("Double Confirm 1st Die?, %d",bDoubleCheckIs1stDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
*/
	}
		

	if (bIs1stDie)
	{
		//the First die is founded
		BT_MoveTo(m_lTempMultiSearchX, m_lTempMultiSearchY);
		Sleep(m_lAlignBinDelay);
		szMsg.Format("Search(Final) -- Result of 1st Die --- Enc(%d,%d), Step(%d,%d), Move(%d,%d)",lX,lY,stInfo.siStepX,stInfo.siStepY, m_lTempMultiSearchX, m_lTempMultiSearchY); 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		m_nMultiSeachCounter = 0;
		return TRUE;
	}
	else
	{
		// find Die if there are not any die in the FOV
		if (bEmptyDieInFov )
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search(Step3) -- FOV not any die", "a+");
			if(m_nMultiSeachCounter == 1) // for start search but No die in the FOV by Spiral Search 
			{
				LONG lMoveCountX = 5000;
				LONG lMoveCountY = 5000;
				switch (ucCorner)
				{
					case REALIGN_UPPER_LEFT_CORNER:
						lMoveCountX = -5000;
						lMoveCountY = -5000;
						 break;
					case REALIGN_UPPER_RIGHT_CORNER:
						lMoveCountX = -5000;
						lMoveCountY = 5000;
						 break;
					case REALIGN_LOWER_LEFT_CORNER:
						lMoveCountX = 5000;
						lMoveCountY = -5000;
						break;
					case REALIGN_LOWER_RIGHT_CORNER:
						lMoveCountX = 5000;
						lMoveCountY = 5000;
						break;
				}		

				DOUBLE dRowFov = 0 , dColFov = 0;
				dRowFov = stInfo.dRowFovSize;  
				dColFov	= stInfo.dColFovSize;
				lMoveCountX = abs(lPitchX *((LONG)dRowFov)); // (1) : move to left
				lMoveCountY = abs(lPitchY *((LONG)dColFov));

				if (SpiralSearchMove(m_nMultiUnfondCounter, lX, lY, lMoveCountX, lMoveCountY) == FALSE )
				{
					return FALSE;
				}

				m_nMultiUnfondCounter++;
				m_nMultiSeachCounter = 0;

				szMsg.Format("Search(Step4)-- Spiral again: %d th, moveC(%d,%d)", m_nMultiUnfondCounter,lMoveCountX,lMoveCountY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				
				return MultiSearchFirstGoodDie(ulBlkInUse, ulDieRow, ulDieCol, bDoGlobalTheta);
			}
			else
			{

				szMsg.Format("Search(Step4) -- after %dth NO Die"); 
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				m_nMultiSeachCounter = 0;
				return FALSE;
			}
		}
		else
		{
			LONG lTempX = 0, lTempY = 0;
			DOUBLE dRowFov = 0, dColFov = 0;

			GetXYEncoderValue(lX, lY);

			m_lTempMultiSearchX = lX + stInfo.siStepX;
			m_lTempMultiSearchY = lY + stInfo.siStepY;

			lTempX = lX + stInfo.siStepX ;
			lTempY = lY + stInfo.siStepY;

			BT_MoveTo(lTempX ,lTempY );
			
			szMsg.Format("Search(Step3) -- FOV have Dies again %d th -- Move(%d,%d),Fov(%d,%d),Diff(%d,%d)",m_nMultiSeachCounter, lTempX,lTempY, (LONG)dRowFov,(LONG)dColFov, lTempX -lX,  lTempY -lY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

			m_lDoubleCheckFirstDie = 0;
			if( m_nMultiSeachCounter <= 15)
			{
				return MultiSearchFirstGoodDie(ulBlkInUse, ulDieRow, ulDieCol, bDoGlobalTheta);
			}
			else
			{
				szMsg.Format("Search(Final) -- Multi search more then %d -- Fail Search!", m_nMultiSeachCounter);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
				//HmiMessage(szMsg);	
				CString szTitle = "Bin Multi Seaerch ";
				HmiMessageEx_Red_Back(szMsg, szTitle);
				m_nMultiSeachCounter = 0;
				return FALSE;
			}
		}		
	}

	return FALSE;	//v4.52 Klocwork
}

LONG CBinTable::SpiralSearchMove(INT nMultiUnfondCounter,LONG lX, LONG lY, LONG lMoveCountX, LONG lMoveCountY)
{

	CString szMsg;
		//1. search right direction
	if( nMultiUnfondCounter == 1)
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX + lMoveCountX*(-1) , lY);
			szMsg.Format("search right direction -- X:%d , Y:%d", lX + lMoveCountX*(-1) , lY);
		}
		//2. search up direction
	else if( nMultiUnfondCounter == 2 )
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX , lY + lMoveCountY*(1));
			szMsg.Format("search up direction -- X:%d , Y:%d", lX , lY + lMoveCountY*(1));
		}
		//3. search left direction
	else if( nMultiUnfondCounter == 3)
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX + lMoveCountX*(1) , lY);
			szMsg.Format("search left direction -- X:%d , Y:%d", lX + lMoveCountX*(1) , lY);
		}
		//3.1 search left direction
	else if( nMultiUnfondCounter == 4)
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX + lMoveCountX*(1) , lY);
			szMsg.Format("search left direction -- X:%d , Y:%d", lX + lMoveCountX*(1) , lY);
		}
		//4. search down direction
	else if ( nMultiUnfondCounter == 5 )
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX , lY + lMoveCountY*(-1));
			szMsg.Format("search down direction -- X:%d , Y:%d",lX , lY + lMoveCountY*(-1));
		}
		//4.1 search down direction
	else if ( nMultiUnfondCounter == 6 )
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX , lY + lMoveCountY*(-1));
			szMsg.Format("search down direction -- X:%d , Y:%d",lX , lY + lMoveCountY*(-1));
		}
		//5. search right direction
	else if( nMultiUnfondCounter == 7)
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX + lMoveCountX*(-1) , lY);
				szMsg.Format("Search right direction -- X:%d , Y:%d", lX + lMoveCountX*(-1) , lY);
		}
		//5. search right direction
	else if( nMultiUnfondCounter == 8)
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX + lMoveCountX*(-1) , lY);
				szMsg.Format("Search right direction -- X:%d , Y:%d", lX + lMoveCountX*(-1) , lY);
		}
		else
		{
			//m_nMultiSeachCounter = 0;
				szMsg.Format("No any dies after sprial search");
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			HmiMessage(szMsg);
				m_nMultiUnfondCounter = 1;
				return FALSE;
			}

			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			return TRUE;
}


BOOL CBinTable::BT_SearchFirstDie(BOOL &bResult, LONG &lSiStepX, LONG &lSiStepY)
{
	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;

	try
	{
/*
		//Search any die on current BT position
		int nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
				break;
			}
			else
			{
				Sleep(1);
			}
		}
*/
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			pBondPr->BT_SearchFirstDie(stMsg);
		}

		stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
		//PR not learnt
		if (stInfo.bResult == IDS_BPR_DIENOTLEARNT)
		{
			return FALSE;
		}

	} 
	catch (CAsmException e)
	{
		SetErrorMessage("Exception in SearchFirstGoodDie");
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Exception in SearchFirstGoodDie", "a+");
		return FALSE;
	}

	bResult = stInfo.bResult;
	lSiStepX = stInfo.siStepX;
	lSiStepY = stInfo.siStepY;
	return TRUE;
}


// capture die image at bin table current position, then judge the result.
LONG CBinTable::SearchFirstGoodDie(ULONG ulBlkInUse, BOOL bDoGlobalTheta)
{

	int nConvID = 0;
	LONG lX, lY;
	IPC_CServiceMessage stMsg;
	BOOL bResult = FALSE;
	LONG siStepX = 0, siStepY = 0;

	if (!BT_SearchFirstDie(bResult, siStepX, siStepY))
	{
		return FALSE;
	}

	if (bResult)
	{
		//Return if 1st die is located on 1st try
		GetXYEncoderValue(lX, lY);
		BT_MoveTo(lX + siStepX, lY + siStepY);
		if (bDoGlobalTheta)
		{
			return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
		}	
		else
		{
			return TRUE;
		}
	}
	else
	{
		//No need to search around if in BURNIN mode
		if (IsBurnIn())		//v3.34
		{
			return FALSE;
		}

		//Otherwise, perform spiral search 1st die with Max PR search region
		short i, j, k;
		LONG lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
		LONG lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));

		for (i = 1; i < 5; i++)
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR);
			Sleep(50);

			if (!BT_SearchFirstDie(bResult, siStepX, siStepY))
			{
				return FALSE;
			}

			if (bResult)
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX + siStepX, lY + siStepY);
				if (bDoGlobalTheta)
				{
					return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
				}	
				else
				{
					return TRUE;
				}
			}

			k = i * 2;

			for (j = 0; j < k; j++) // a
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR);
				Sleep(50);

				if (!BT_SearchFirstDie(bResult, siStepX, siStepY))
				{
					return FALSE;
				}

				if (bResult)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX + siStepX, lY + siStepY);
					if (bDoGlobalTheta)
					{
						return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
					}	
					else
					{
						return TRUE;
					}
				}
			}

			for (j = 0; j < k; j++) // b
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX + lPitchX * BT_X_DIR, lY);
				Sleep(50);

				if (!BT_SearchFirstDie(bResult, siStepX, siStepY))
				{
					return FALSE;
				}

				if (bResult)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX + siStepX, lY + siStepY);
					if (bDoGlobalTheta)
					{
						return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
					}	
					else
					{
						return TRUE;
					}
				}
			}

			for (j = 0; j < k; j++) // c
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR);
				Sleep(50);

				if (!BT_SearchFirstDie(bResult, siStepX, siStepY))
				{
					return FALSE;
				}

				if (bResult)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX + siStepX, lY + siStepY);
					if (bDoGlobalTheta)
					{
						return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
					}	
					else
					{
						return TRUE;
					}
				}
			}

			for (j = 0; j < k; j++) // d
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX - lPitchX * BT_X_DIR, lY);
				Sleep(50);

				if (!BT_SearchFirstDie(bResult, siStepX, siStepY))
				{
					return FALSE;
				}

				if (bResult)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX + siStepX, lY + siStepY);
					if (bDoGlobalTheta)
					{
						return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
					}	
					else
					{
						return TRUE;
					}
				}
			}	//END if (m_bSemitekBLMode)
		}	//END for (i = 1; i < 5; i++)
	}

	return FALSE;
}

LONG CBinTable::SearchFirstGoodDie_Semitek(ULONG ulBlkInUse, BOOL bDoGlobalTheta, BOOL bCCWSearch)
{
	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	int nConvID = 0;
	LONG lX, lY;
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;

	CString szLog;
	szLog.Format("Search 2nd-die (Bin #%lu): SearchFirstGoodDie_Semitek", ulBlkInUse);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");


	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	if (stInfo.bResult)
	{
		//Return if 1st die is located on 1st try
		GetXYEncoderValue(lX, lY);
		BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);

		szLog.Format("1: %ld, %ld", lX + stInfo.siStepX, lY + stInfo.siStepY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		if (bDoGlobalTheta)
		{
			return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
		}	
		else
		{
			return TRUE;
		}
	}
	else
	{
		//No need to search around if in BURNIN mode
		if (IsBurnIn())		//v3.34
		{
			return FALSE;
		}

		//Otherwise, perform spiral search 1st die with Max PR search region
		short i, j, k;
		LONG lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
		LONG lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));
		LONG lStepX = 0, lStepY = 0;

		for (i = 1; i < 5; i++)
		{
			GetXYEncoderValue(lX, lY);
			
			//v4.50A21
			//BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR);
			lStepX = _round(lX - 0.5 * lPitchX * BT_X_DIR);
			lStepY = _round(lY - 0.5 * lPitchY * BT_Y_DIR);
			BT_MoveTo(lStepX, lStepY);

			//v4.51A14
			szLog.Format("Srch1stDie: BT START pos #%d = (%ld, %ld)", i, lStepX, lStepY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

			//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo Search Area Larger
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
					break;
				}
				else
				{
					Sleep(1);
				}
			}

			szLog.Format("START: %d", stInfo.bResult);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			if (stInfo.bResult)
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
				if (bDoGlobalTheta)
				{
					return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
				}	
				else
				{
					return TRUE;
				}
			}

			k = i * 2;

			if (bCCWSearch)		//Counter-Clockwise from upper-right corner die		//v4.50A15
			{
				//v4.50A10	//Use anti-clockwise instead of clockwise	//Semitek
					
				//1. Search LEFT direction
				for (j = 0; j < k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX + lPitchX * BT_X_DIR, lY);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CCW: BT LEFT pos #%d = (%ld, %ld)", 
									j, lX + lPitchX * BT_X_DIR, lY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo Search Area Larger
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("LEFT: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}

				//2. Search DOWN direction
				for (j=0; j<k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CCW: BT DOWN pos #%d = (%ld, %ld)", 
									j, lX, lY + lPitchY * BT_Y_DIR);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("DOWN: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}

				//3. Search RIGHT direction
				for (j = 0; j < k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX - lPitchX * BT_X_DIR, lY);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CCW: BT RIGHT pos #%d = (%ld, %ld)", 
									j, lX - lPitchX * BT_X_DIR, lY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("RIGHT: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}

				//4. Search UP direction
				for (j=0; j<k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CCW: BT UP pos #%d = (%ld, %ld)", 
									j, lX, lY - lPitchY * BT_Y_DIR);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("UP: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}
			}	
			else	//if (bCCWSearch)
			{
				//v4.50A15	//Use clockwise instead of anti-clockwise	//3E DL

				//1. Search DOWN direction
				for (j=0; j<k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CW: BT DOWN pos #%d = (%ld, %ld)", 
									j, lX, lY + lPitchY * BT_Y_DIR);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("DOWN: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}

				//2. Search LEFT direction
				for (j = 0; j < k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX + lPitchX * BT_X_DIR, lY);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CW: BT LEFT pos #%d = (%ld, %ld)", 
									j, lX + lPitchX * BT_X_DIR, lY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("LEFT: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}

				//4. Search UP direction
				for (j=0; j<k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CW: BT UP pos #%d = (%ld, %ld)", 
									j, lX, lY - lPitchY * BT_Y_DIR);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("UP: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}

				//4. Search RIGHT direction
				for (j = 0; j < k; j++)
				{
					GetXYEncoderValue(lX, lY);
					BT_MoveTo(lX - lPitchX * BT_X_DIR, lY);
					Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

					//v4.51A14
					szLog.Format("Srch1stDie CW: BT RIGHT pos #%d = (%ld, %ld)", 
									j, lX - lPitchX * BT_X_DIR, lY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, stMsg);
							stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
							break;
						}
						else
						{
							Sleep(1);
						}
					}

					szLog.Format("RIGHT: %d", stInfo.bResult);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (stInfo.bResult)
					{
						GetXYEncoderValue(lX, lY);
						BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
						if (bDoGlobalTheta)
						{
							return BT_FindGlobalTheta(ulBlkInUse);    //v4.39T7
						}	
						else
						{
							return TRUE;
						}
					}
				}
			}

		}	//END for (i = 1; i < 5; i++)
	}

	return FALSE;
}


BOOL CBinTable::BT_FindContinue8Die(const LONG lPitchX, const LONG lPitchY, const ULONG ulWalkPath, const BOOL bUseWafflePad,
									LONG &lLHSDieX2, LONG &lLHSDieY2, LONG &lRHSDieX, LONG &lRHSDieY, LONG &lFoundCounter)
{
	LONG lX = 0, lY = 0;
	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	BOOL bRHSDieFound = FALSE;
	INT nPrFailCount = 0;

	//init
	lLHSDieX2 = 0, lLHSDieY2 = 0; 
	lRHSDieX = 0, lRHSDieY = 0;
	lFoundCounter = 0;

	for (LONG i = 0; i <= 7; i++)
	{
		GetXYEncoderValue(lX, lY);

		//5432123
		if ((ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH))
		{
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR);
		}
		else if ((ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_BRV_PATH))
		{
			BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR);
		}
		else if ((ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH))
		{
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY);
		}
		else
		{
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY);
		}

		Sleep(m_lAlignBinDelay);

		int nConvID = 0;
		IPC_CServiceMessage stMsg;
		stInfo.bResult = FALSE;

		if (bUseWafflePad)	//v4.58A5
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchRefDie2", stMsg);
		else
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDie", stMsg);
		
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
				break;
			}
			else
			{
				Sleep(1);
			}
		}

		if (stInfo.bResult)
		{
			lFoundCounter++;
			nPrFailCount = 0;
			bRHSDieFound = TRUE;
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
			lRHSDieX = lX + stInfo.siStepX;
			lRHSDieY = lY + stInfo.siStepY;

			if (i == 1)		//Use 2nd die for LHS die if available
			{
				lLHSDieX2 = lX + stInfo.siStepX;
				lLHSDieY2 = lY + stInfo.siStepY;
			}
		}
		else
		{
			nPrFailCount++;
			if (bUseWafflePad)			//v4.59A35	//WafflePad must align all PADs to continue !!
			{
				if (nPrFailCount >= 1)	//v4.59A1	//MS90 wafflepad //Wolfspeed
				{
					break;
				}
			}
		}
	}

	return bRHSDieFound;
}


LONG CBinTable::BT_FindGlobalTheta(ULONG ulBlkInUse)
{
	if (!m_bEnable_T)
	{
		return TRUE;
	}
	if (!m_bSel_T)
	{
		return TRUE;
	}
	if (!T_IsPowerOn())
	{
		return FALSE;
	}


	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

//	INT i = 0;
	int nConvID = 0;
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;
	LONG lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	LONG lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));
	ULONG ulWalkPath		= m_oBinBlkMain.GrabWalkPath(ulBlkInUse);			//5432123
	BOOL bUseWafflePad		= m_oBinBlkMain.GrabIsEnableWafflePad(ulBlkInUse);	//v4.58A5	//WolfSpeed
	ULONG ulNoOfSortedDie	= m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);	//v4.59A26


	//v4.59A21
	CString szTemp;
	szTemp.Format("BT_FindGlobalTheta-T: Path=%d, UseWafflePad=%d, PITCH=(%ld, %ld), Count=%lu", 
		ulWalkPath, bUseWafflePad, lPitchX, lPitchY, ulNoOfSortedDie);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szTemp, "a+");

	//v4.59A26
	if (ulNoOfSortedDie <= 1)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("BT_FindGlobalTheta aborted", "a+");
		return TRUE;
	}

	INT nPrFailCount = 0;
	LONG lX = 0,		lY = 0;
	LONG lLHSDieX = 0, lLHSDieY = 0; 
	LONG lLHSDieX2 = 0, lLHSDieY2 = 0; 
	LONG lRHSDieX = 0, lRHSDieY = 0; 
	GetXYEncoderValue(lLHSDieX, lLHSDieY);

	//5432123
	LONG lOrigX = lLHSDieX;
	LONG lOrigY = lLHSDieY;

	LONG lFoundCounter = 0;
	BOOL bRHSDieFound = BT_FindContinue8Die(lPitchX, lPitchY, ulWalkPath, bUseWafflePad,
									lLHSDieX2, lLHSDieY2, lRHSDieX, lRHSDieY, lFoundCounter);

	//Find Die at the Left Direction
	if (!bRHSDieFound)
	{
		BT_MoveTo(lOrigX, lOrigY);
		bRHSDieFound = BT_FindContinue8Die(-lPitchX, -lPitchY, ulWalkPath, bUseWafflePad,
											lLHSDieX2, lLHSDieY2, lRHSDieX, lRHSDieY, lFoundCounter);
		if  ((fabs((double)lRHSDieX - (double)lLHSDieX) < fabs((double)lPitchX / 2)) && 
			 (fabs((double)lRHSDieY - (double)lLHSDieY) < fabs((double)lPitchY / 2)))
		{
			bRHSDieFound = FALSE;
		}

		if (bRHSDieFound)
		{
			//Exchange Left&Right die coordinate
			lLHSDieX2 = lRHSDieX;
			lLHSDieY2 = lRHSDieY;
			lRHSDieX = lLHSDieX;
			lRHSDieY = lLHSDieY;
			lLHSDieX = lLHSDieX2;
			lLHSDieY = lLHSDieY2;
		}
	}
	else
	{
		//Use 2nd die as LHS die if more than 2 dice on the row
		if ((lFoundCounter > 4) && (lLHSDieX2 != 0) && (lLHSDieY2 != 0))
		{
			lLHSDieX = lLHSDieX2;
			lLHSDieY = lLHSDieY2;
		}
	}

	if (!bRHSDieFound)
	{
		SetErrorMessage("ERR: BT_FindGlobalTheta - fail to find any RHS die");
		HmiMessage_Red_Yellow("RHS die not found!", "Align Global BT Angle");
		return FALSE;
	}

	//Calcuate wafer angle	
	DOUBLE dThetaAngle = 0;
	DOUBLE dDiffY = (DOUBLE)(lLHSDieY - lRHSDieY);
	DOUBLE dDiffX = (DOUBLE)(lLHSDieX - lRHSDieX);
	if ((ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH))
		dDiffX = (DOUBLE)(lRHSDieX - lLHSDieX);		//v4.49A10	//MS90
	
	//5432123	//v4.59A33	//LeoLam 20171109
	if ((ulWalkPath == BT_BRV_PATH) ||    //leo please check that BLV and BRV should put together  
		(ulWalkPath == BT_BLV_PATH) )
		//(ulWalkPath == BT_TRV_PATH) ||
		//(ulWalkPath == BT_BRV_PATH) )
	{
		dThetaAngle = atan2(fabs(dDiffX) , fabs(dDiffY) ) * 180 / PI;
		if ((lRHSDieX - lLHSDieX) < 0)
		{
			dThetaAngle = dThetaAngle * -1.00;
		}
	}
	else if ((ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH)) //leo please check that TLV and TRV should put together
	{
		dThetaAngle = atan2(fabs(dDiffX) , fabs(dDiffY) ) * 180 / PI;
		if ((lRHSDieX - lLHSDieX) > 0)
		{
			dThetaAngle = dThetaAngle * -1.00;
		}
	}
	else
	{
		dThetaAngle = atan2(dDiffY , dDiffX) * (180 / PI) * -1.00;
		if ((ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH))
			dThetaAngle = dThetaAngle * -1.00;		//v4.49A10	//MS90
	}

	szTemp.Format("Global-T: Angle = %.6f degree\n", dThetaAngle);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szTemp, "a+");

	if (IsMS90())
		T_MoveByDegree(-1 * dThetaAngle);	//v4.50A23
	else
		T_MoveByDegree(dThetaAngle);
	Sleep(100);
	BT_MoveTo(lOrigX, lOrigY);
	Sleep(100);


	//Perform XY alignment on LHS die once more after theta correction (ConfirmSearch)
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDie", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
	if (stInfo.bResult)
	{
		GetXYEncoderValue(lX, lY);
		BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
	}
	else
	{
		return 2;	//need to re-do SearchFirstGoodDie()
	}

	return TRUE;
}


LONG CBinTable::ScanningBarcode(ULONG ulBlkID, BOOL bScanWithTheta)
{
	BOOL bReadBarcode = (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["UseBracode"];
	if (!bReadBarcode)
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("BT: ScanningBarcode disabled");
		return TRUE;
	}                
	if (IsBurnIn())
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("BT: ScanningBarcode disabled by BURNIN");
		return TRUE;
	}

		//Tell BT that current LAOD frame is for BINFULL	//FOr Nichia MS100+ with BT T only
	BOOL bDBBinFull	= (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["DB"]["BinFull"];

	CString szMsg;
	CString szBarcode;
	LONG lStatus = ScanBySymbol(szBarcode, bScanWithTheta);
	
	LONG lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
	LONG lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
	CString szSlotBarcode = m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot];
	
	CMSLogFileUtility::Instance()->BL_LogStatus("BT: Start barcode scan on BT ...");
	szMsg.Format("Curr Mgzn = %ld, Slot = %ld, Slot BC = " + szSlotBarcode, lCurrMgzn, lCurrSlot);
	CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);

	//Use Manual Input BC if cannot scan
	if ((lStatus == FALSE) || szBarcode.IsEmpty())
	{
		T_MoveTo(0);	//v4.40T7

		CMSLogFileUtility::Instance()->BL_LogStatus("No barcode read (BT) -> use Manual-Input");

		if (ManualInputBarcode(&szBarcode) != TRUE)
		{
			lStatus = FALSE;
		}
		else
		{
			if (!szBarcode.IsEmpty())
			{
				lStatus = TRUE;
			}
			else
			{
				lStatus = FALSE;
			}
		}

		szMsg.Format("Manual Input BC: status = %d; BC = " + szBarcode, lStatus);
		CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);
	}

	//v4.40T7
	BOOL bBinSheetCheck = CMS896AStn::m_oNichiaSubSystem.CheckBinSheetType(szBarcode);
	if (!bBinSheetCheck)
	{
		lStatus = FALSE;
		CString szMsg;
		szMsg.Format("Nichia bin sheet check fail! barcode = %s, Sheet-type=%s, status = %d", szBarcode, 
						CMS896AStn::m_oNichiaSubSystem.m_szBinSheetType, lStatus);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage_Red_Yellow(szMsg);
		szMsg.Format("Nichia bin sheet check fail! barcode = %s, \nSheet-type=%s, \nstatus = %d", szBarcode, 
						CMS896AStn::m_oNichiaSubSystem.m_szBinSheetType, lStatus);
		SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szMsg, "CLOSE");	//anichia003
	}

	if (bDBBinFull)
	{
		//Let BL to update this barcode AFTER UNLOAD FULL Frame;
		m_szBtTBarCode	= szBarcode;
		CMSLogFileUtility::Instance()->BL_LogStatus("BT: BinFull table T save barcode to m_szBtTBarCode");
		(*m_psmfSRam)["BinLoader"]["DB"]["BinFull"] = FALSE;
	}
	else
	{
		//CHeck if BC is available
		if ((lStatus != FALSE) && !szBarcode.IsEmpty())
		{
			if (szSlotBarcode.IsEmpty())
			{
				m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = szBarcode;
				SaveBLBarcodeData(ulBlkID, szBarcode, lCurrMgzn, lCurrSlot);
			}
			else if (szSlotBarcode != szBarcode)
			{
				CString szText;
				szText = "#1 Unload Bin Frame\n";
				szText += "Old: [@@" + szSlotBarcode + "@@]\nNew: [@@" + szBarcode + "@@]";
									
				szMsg = szText;
				szMsg.Replace('\n', ' ');
				SetStatusMessage(szMsg);

				LONG lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Old", "New", NULL, glHMI_ALIGN_LEFT);

				switch (lOption)
				{
					case -1:
						CMSLogFileUtility::Instance()->BL_LogStatus("LOAD: barcode comparison fails -> abort");
						SetStatusMessage("BT Compare BL barcode fail: ABORT");
						CMSLogFileUtility::Instance()->BL_LogStatus("BT Compare BL barcode fail: ABORT");
						//SaveBarcodeData(m_lBTCurrentBlock, m_szBCName, lCurrMgzn, lCurrSlot);
						return FALSE;	//ABORT

					case 1:		//OLD
						CMSLogFileUtility::Instance()->BL_LogStatus("LOAD: barcode comparison fails -> use old " + szSlotBarcode);	
						SetStatusMessage("BT Compare BL barcode fail: Use old barcode -> " + szSlotBarcode);
						CMSLogFileUtility::Instance()->BL_LogStatus("BT Compare BL barcode fail: Use old barcode -> " + szSlotBarcode);
						SaveBLBarcodeData(ulBlkID, szSlotBarcode, lCurrMgzn, lCurrSlot);
						break;

					case 2:		//NEW
						CMSLogFileUtility::Instance()->BL_LogStatus("LOAD: barcode comparison fails ABORT -> use new ");	
						m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = szBarcode;
						SetStatusMessage("BT Compare BL barcode fail: Use new barcode -> " + szBarcode);
						CMSLogFileUtility::Instance()->BL_LogStatus("BT Compare BL barcode fail: Use new barcode -> " + szBarcode);
						SaveBLBarcodeData(ulBlkID, szBarcode, lCurrMgzn, lCurrSlot);
						break;
				}
			}
			else
			{
				m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = szBarcode;
				SetStatusMessage("BT Barcode updated -> " + szBarcode);
				CMSLogFileUtility::Instance()->BL_LogStatus("BT Barcode updated -> " + szBarcode);	//5432123
				SaveBLBarcodeData(ulBlkID, szBarcode, lCurrMgzn, lCurrSlot);
			}
		}
	}

	return lStatus;
}

LONG CBinTable::ScanBySymbol(CString &szBarcode, BOOL bScanWithTheta)
{
	short i = 0;
	BOOL bResult = TRUE;

	LONG lScanRange		= (*m_psmfSRam)["BinLoader"]["BCScanRange"];
	LONG lRetryLimit	= (*m_psmfSRam)["BinLoader"]["BCRetryLimit"];

	if (lRetryLimit < 1)
	{
		lRetryLimit = 1;
	}
	if (lRetryLimit > 5)
	{
		lRetryLimit = 5;
	}
	if (lScanRange < 10000)
	{
		lScanRange = 10000;
	}
	if (lScanRange > 50000)
	{
		lScanRange = 50000;
	}

	GetEncoderValue();
	LONG lX = m_lEnc_X;
	LONG lY = m_lEnc_Y;

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	//XY_MoveTo(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, SFM_WAIT);
	XY_MoveTo(m_lBTBarcodePos_X, m_lBTBarcodePos_Y, SFM_WAIT);
	CString szBC = "";

	for (INT i = 0; i < lRetryLimit; i++)
	{
		Sleep(500);
		X_SearchBarcode(1, lScanRange, SFM_NOWAIT);
		if (bScanWithTheta)
		{
			T_SearchBarcode(1, 800, SFM_NOWAIT);
		}
		ReadBarcode(&szBC);
		if (szBC.GetLength() > 0)
		{
			if (bScanWithTheta)
			{
				CMS896AStn::MotionStop(BT_AXIS_T, &m_stBTAxis_T);
			}
			X_Sync();
			break;
		}
		X_Sync();
		T_Sync();

		Sleep(500);
		X_SearchBarcode(0, lScanRange, SFM_NOWAIT);
		if (bScanWithTheta)
		{
			T_SearchBarcode(1, -800, SFM_NOWAIT);
		}
		ReadBarcode(&szBC);
		if (szBC.GetLength() > 0)
		{
			if (bScanWithTheta)
			{
				CMS896AStn::MotionStop(BT_AXIS_T, &m_stBTAxis_T);
			}
			X_Sync();
			break;
		}
		X_Sync();
		T_Sync();
	}

	XY_MoveTo(lX, lY, SFM_WAIT);
	Sleep(500);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if (szBC.GetLength() > 0)
	{
		szBarcode	= szBC;
		bResult		= TRUE;
	}
	else
	{
		bResult		= FALSE;
	}

	return bResult;
}

BOOL CBinTable::ReadBarcode(CString *szData, int nTimeOut)
{
	char scTurnOnLaser[] = { 0x02, 0x53, 0x54, 0x49, 0x45, 0x03, 0x08 };
	char scReadData[50];
	char scOutputData[50];
	char scTempData[50];
	short i = 0;
	short j = 0;

	//Init variable
	szData->Empty();
	memset(scReadData, NULL, sizeof(scReadData));
	memset(scOutputData, NULL, sizeof(scOutputData));
	memset(scTempData, NULL, sizeof(scTempData));

	//Check COM is created
	if (m_oCommPort.IsCreated() != TRUE)
	{
		return FALSE;
	}

	//Open COM & Check 
	m_oCommPort.SetReadConstant((DWORD)nTimeOut);
	m_oCommPort.Open();
	if (m_oCommPort.IsOpen() == TRUE)
	{
		m_oCommPort.Purge();
	}
	else
	{
		return FALSE;
	}

	try
	{
		//Request Barcode On laser
		m_oCommPort.Write(scTurnOnLaser, 7);
		m_oCommPort.Purge();
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}


	INT nCount = 0;
	INT nLen = 0;

	while (1)
	{
		try
		{
			//Read data
			m_oCommPort.Read(scReadData, 50);

			//Append data into temp.
			strcat_s(scTempData, sizeof(scTempData), scReadData);

			//check data end character
			nLen = (INT)strlen(scReadData) - 1;
			nLen = max(nLen, 0);	//Klocwork		//v4.02T5
			
			if (scReadData[nLen] == '\n')
			{
				//Copy temp data into ReadData
				strcpy_s(scReadData, sizeof(scReadData), scTempData);
				break;
			}
			else
			{
				memset(scReadData, NULL, sizeof(scReadData));
			}
		}
		catch (CAsmException e)
		{
			//No need to display exception
		}

		nCount++;
		if (nCount >= 5)	//10 )	//v4.37
		{
			break;
		}
	}

	m_oCommPort.Close();

	//Decode data
	for (i = 0; i < 50; i++)
	{
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}

	szData->Format("%s", scOutputData);
	if (szData->IsEmpty())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CBinTable::ManualInputBarcode(CString *szBarcode)
{
	IPC_CServiceMessage srvMsg;
	CString szTemp;
	CString szResult;
	CHAR acPar[200];
	BOOL bResult;
	BOOL bMask = FALSE;
	BOOL bReturn = TRUE;
	int nIndex = 0;

	//Assign 1 HMI variable to be used
	szTemp = "szBLBarcodeValue";
	strcpy_s(acPar, sizeof(acPar), (LPCTSTR)szTemp);
	nIndex += szTemp.GetLength() + 1;
	
	//Append input box title
	szTemp.LoadString(HMB_BL_KEYIN_BARCODE);
	strcpy_s(&acPar[nIndex], sizeof(acPar) - nIndex, (LPCTSTR)szTemp);
	nIndex += szTemp.GetLength() + 1;

	memcpy(&acPar[nIndex], &bMask, sizeof(BOOL));
	nIndex += sizeof(BOOL);
	memcpy(&acPar[nIndex], &bReturn, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	//anichia003
	CString szContent	= "Please input barcode manually for bin frame";
	CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);
	//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szContent);		//v4.43T10
	SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_WARNING, szContent, "CLOSE");

	srvMsg.InitMessage(nIndex, acPar);
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiAlphaKeys", srvMsg);
	while (!m_comClient.ScanReplyForConvID(nConvID, 10))
	{
		Sleep(10);
	}

	m_comClient.ReadReplyForConvID(nConvID, srvMsg);
	CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);		//anichia001

	//Get reply 
	char *pBuffer = new char[srvMsg.GetMsgLen()];	
	srvMsg.GetMsg(srvMsg.GetMsgLen(), pBuffer);

	//Get result
	memcpy(&bResult, pBuffer, sizeof(BOOL));

	//if (bResult == TRUE)
	//{
		//Get input string
		*szBarcode = &pBuffer[sizeof(BOOL)]; 
	//}
	//else
	//{
	//	szBarcode->IsEmpty();
	//}

	delete[] pBuffer;
	return bResult;
}

BOOL CBinTable::LookAround1stDie(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol)		//v4.50A23
{
	if (dFOVSize < BPR_LF_SIZE) 
	{
		return TRUE;
	}

	if (!m_bSemitekBLMode)
	{
		return TRUE;
	}

	LONG lCX=0, lCY=0;
	LONG lX=0, lY=0;

	BOOL bCDie = SearchGoodDieXYinFOV(lCX, lCY, BT_PR_CENTER);		//Latch or first image that has to be grabbed

	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBinBlockID) || m_oBinBlkMain.GrabEnableBinMap(ulBinBlockID))
	{
		BOOL bTDie = FALSE;
		BOOL bRDie = FALSE;
		ULONG ulWalkPath = (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"];
		UCHAR ucCorner = GetRealignWithFirstCorner(ulWalkPath, ulDieRow, ulDieCol);
		switch (ucCorner)
		{
			case REALIGN_UPPER_LEFT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_BOTTOM);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_RIGHT);
					break;
			case REALIGN_UPPER_RIGHT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_BOTTOM);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_LEFT);
					break;
			case REALIGN_LOWER_LEFT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_RIGHT);
					break;
			case REALIGN_LOWER_RIGHT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_LEFT);
					break;
		}
		if (!bCDie || bTDie || bRDie)
		{
			return FALSE;	//Wrong alignment!
		}
	}
	else if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TLH_PATH) 
	{
		BOOL bTDie  = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);		
		BOOL bLDie  = SearchGoodDieXYinFOV(lX, lY, BT_PR_LEFT);		
		BOOL bBLDie = FALSE;	//SearchGoodDieXYinFOV(lX, lY, BT_PR_BOTTOM_LEFT);		//v4.50A30	//LeoLam
		BOOL bTRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP_RIGHT);	

		CString szLog;
		szLog.Format("LookAround1stDie: C=%d, T=%d, L=%d, BL=%d, TR=%d", 
			bCDie, bTDie, bLDie, bBLDie, bTRDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		if (bTDie || bLDie || bBLDie || bTRDie)
		{
			return FALSE;	//Wrong alignment!
		}
	}
	else if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TRH_PATH) 
	{
		BOOL bTDie  = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);		
		BOOL bRDie  = SearchGoodDieXYinFOV(lX, lY, BT_PR_RIGHT);		
		BOOL bBRDie = FALSE;	//SearchGoodDieXYinFOV(lX, lY, BT_PR_BOTTOM_RIGHT);			//v4.50A30	//LeoLam
		BOOL bTLDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP_LEFT);	

		CString szLog;
		szLog.Format("LookAround1stDie: C=%d, T=%d, R=%d, BR=%d, TL=%d", 
				bCDie, bTDie, bRDie, bBRDie, bTLDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		if (bTDie || bRDie || bBRDie || bTLDie)
		{
			return FALSE;	//Wrong alignment!
		}
	}

	return TRUE;
}

BOOL CBinTable::LookAround2ndDie(const ULONG ulBinBlockID, const DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol)		//v4.44T3
{
	if (dFOVSize < BPR_LF_SIZE) 
	{
		return TRUE;
	}

	LONG lCX=0, lCY=0;
	LONG lX=0, lY=0;

	BOOL bCDie = SearchGoodDieXYinFOV(lCX, lCY, BT_PR_CENTER);		//Latch or first image that has to be grabbed
	
	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBinBlockID) || m_oBinBlkMain.GrabEnableBinMap(ulBinBlockID))
	{
		BOOL bTDie = FALSE;
		BOOL bRDie = FALSE;
		ULONG ulWalkPath = (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"];
		UCHAR ucCorner = GetRealignWithSecondCorner(ulWalkPath, ulDieRow, ulDieCol);
		switch (ucCorner)
		{
			case REALIGN_UPPER_LEFT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_BOTTOM);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_RIGHT);
					break;
			case REALIGN_UPPER_RIGHT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_BOTTOM);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_LEFT);
					break;
			case REALIGN_LOWER_LEFT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_RIGHT);
					break;
			case REALIGN_LOWER_RIGHT_CORNER:
					bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);
					bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_LEFT);
					break;
		}
		if (!bCDie || bTDie || bRDie)
		{
			return FALSE;	//Wrong alignment!
		}
	}
	else if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TLH_PATH) 
	{
		BOOL bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);		//Latch or first image that has to be grabbed
		BOOL bRDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_RIGHT);		//Latch or first image that has to be grabbed

		CString szLog;
		szLog.Format("LookAround2ndDie: C=%d, T=%d, R=%d", bCDie, bTDie, bRDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		if (!bCDie || bTDie || bRDie)
		{
			return FALSE;	//Wrong alignment!
		}
	}
	else if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TRH_PATH) 
	{
		BOOL bTDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_TOP);		//Latch or first image that has to be grabbed
		BOOL bLDie = SearchGoodDieXYinFOV(lX, lY, BT_PR_LEFT);		//Latch or first image that has to be grabbed

		CString szLog;
		szLog.Format("LookAround2ndDie: C=%d, T=%d, L=%d", bCDie, bTDie, bLDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		if (!bCDie || bTDie || bLDie)
		{
			return FALSE;	//Wrong alignment!
		}
	}

	return TRUE;
}


BOOL CBinTable::LookAhead3rdGoodDie(CONST ULONG ulBinBlockID, CONST BOOL bCheckLeftDie, DOUBLE dFOVSize, CString &szError)
{
	if (dFOVSize < BPR_LF_SIZE) 
	{
		szError = "OK";
		return TRUE;
	}

	ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBinBlockID);
	ULONG ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBinBlockID); 
	
	BOOL bCheckTopDie = FALSE;
	if (ulNoOfSortedDie > ulDiePerRow)				// If 1st row
	{
		bCheckTopDie = TRUE;
	}
	
	BOOL bIsFirstDieInRow = FALSE;
	if ((ulNoOfSortedDie % ulDiePerRow) == 1)		// If 1st die on the row
	{
		bIsFirstDieInRow = TRUE;
	}

	BOOL bLeftDie = bCheckLeftDie;
	if ((ulNoOfSortedDie % ulDiePerRow) == 0)		// If last die on the row
	{
		INT nCol = ulNoOfSortedDie / ulDiePerRow;
		if ((nCol % 2) == 0)	
		{
			bLeftDie = FALSE;	//EVEN row 0,2,4,....
		}
		else
		{
			bLeftDie = TRUE;	//ODD row 1,3,5,....
		}
	}

	// Display Search window on screen for last-die LF searching
	if (bIsFirstDieInRow)		// Do not check LEFT/RIGHT die for every 1st die on the new row
	{
		DisplayBondPrSearchWindow(TRUE, TRUE, 5);	// UP & DN window

		DisplayBondPrSearchWindow(TRUE, TRUE, 4);	// RT window (To show user this is next bond pos
	}
	else if (bLeftDie)
	{
		DisplayBondPrSearchWindow(TRUE, TRUE, 3);	// Left & DN & UP

		DisplayBondPrSearchWindow(TRUE, TRUE, 4);	// RT window (To show user this is next bond pos
	}
	else
	{
		DisplayBondPrSearchWindow(TRUE, TRUE, 4);	// Right & DN & UP

		DisplayBondPrSearchWindow(TRUE, TRUE, 3);	// LT window (To show user this is next bond pos
	}


	// Check TOP die window
	BOOL bTopDieOK	= TRUE;
	if (bCheckTopDie)
	{
		bTopDieOK = SearchGoodDieinFOV(BT_PR_TOP);
		if (!bTopDieOK)
		{
			szError += "TOP die not existed!  ";
		}
	}
	else
	{
		bTopDieOK = !SearchGoodDieinFOV(BT_PR_TOP);
		if (!bTopDieOK)
		{
			szError += "TOP die is detected!  ";
		}
	}


	// Check DOWN window
	BOOL bDnDieOK = SearchGoodDieinFOV(BT_PR_BOTTOM);
	bDnDieOK = !bDnDieOK;
	if (!bDnDieOK)
	{
		szError += "DOWN die exists!  ";
	}


	// Check LEFT or RIGHT window
	BOOL bLeftRightDieOK = TRUE;
	if (!bIsFirstDieInRow)
	{
		if (bLeftDie)
		{
			bLeftRightDieOK = SearchGoodDieinFOV(BT_PR_LEFT);
		}
		else
		{
			bLeftRightDieOK = SearchGoodDieinFOV(BT_PR_RIGHT);
		}

		if (!bLeftRightDieOK)
			if (bLeftDie)
			{
				szError += "LEFT die not existed!  ";
			}
			else
			{
				szError += "RIGHT die not existed!  ";
			}
	}


	// Return result based on ctrieria
	if (bTopDieOK && bDnDieOK && bLeftRightDieOK)
	{
		szError = "PR Look-Forward result of Last-Die OK.  ";
		return TRUE;
	}

	return FALSE;
}


BOOL CBinTable::LookAhead2ndDieOffset(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, 
									  BOOL &bUse2ndDieOffset, LONG &lOffsetX, LONG &lOffsetY)
{
	CString szLog;
	szLog = "START 1st+1 Die XY";
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

	if (dFOVSize < BPR_LF_SIZE) 
	{
		szLog.Format("1st+1 Die XY - disabled due to small FOV size (%.2f)", dFOVSize);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		return FALSE;
	}

	ULONG ulWalkingPath = m_oBinBlkMain.GrabWalkPath(ulBinBlockID);

	BOOL bCheckRightDie = TRUE;
	if ((ulWalkingPath == BT_TRH_PATH) || (ulWalkingPath == BT_BRH_PATH))
	{
		bCheckRightDie = FALSE;
	}

	BOOL bCheckDownDie = TRUE;		//For BT_TLV_PATH & BT_TRV_PATH
	if ((ulWalkingPath == BT_BLV_PATH) || (ulWalkingPath == BT_BRV_PATH))
	{
		bCheckDownDie = FALSE;
	}



	LONG lCX = 0, lCY = 0;
	LONG lTX = 0, lTY = 0;
	BOOL bCDie = FALSE, b2ndDie = FALSE;


	bCDie = SearchGoodDieXYinFOV(lCX, lCY, BT_PR_CENTER);		//Latch or first image that has to be grabbed

	if ( (ulWalkingPath == BT_TLH_PATH) || (ulWalkingPath == BT_TRH_PATH) ||
		 (ulWalkingPath == BT_BLH_PATH) || (ulWalkingPath == BT_BRH_PATH) )
	{
		if (bCheckRightDie)		//2nd die on right side
		{
			b2ndDie = SearchGoodDieXYinFOV(lTX, lTY, BT_PR_RIGHT);
		}
		else
		{
			b2ndDie = SearchGoodDieXYinFOV(lTX, lTY, BT_PR_LEFT);
		}

		if (!bCDie || !b2ndDie)
		{
			bUse2ndDieOffset = FALSE;
			lOffsetX = 0;
			lOffsetY = 0;

			szLog = "1st+1 Die XY (HORIZ) - fail because 2nd die not found";
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			return FALSE;
		}


		LONG lPitchXinStep = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBinBlockID)));
		LONG lPitchYinStep = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBinBlockID));

		bUse2ndDieOffset = TRUE;

		LONG lOffsetXinStep = lCX - lTX;
		if (!bCheckRightDie)
			lOffsetXinStep	= lTX - lCX;		//v4.46T16		
		LONG lOffsetYinStep = lCY - lTY;

		lOffsetX = ConvertXEncoderValueToFileUnit(lOffsetXinStep - lPitchXinStep);		//steps -> um
		lOffsetY = ConvertXEncoderValueToFileUnit(lOffsetYinStep);						//steps -> um

		if (bCheckRightDie)		//2nd die on right side
		{
			szLog.Format("1st+1 RIGHT Die XY - X Offset = %ld um (X-Dist = %ld, PitchX = %ld), Y Offset = %ld um (Y-Dist = %ld, PitchX = %ld)", 
						lOffsetX, lOffsetXinStep, lPitchXinStep, lOffsetY, lOffsetYinStep, lPitchYinStep);
		}
		else
		{
			szLog.Format("1st+1 LEFT Die XY - X Offset = %ld um (X-Dist = %ld, PitchX = %ld), Y Offset = %ld um (Y-Dist = %ld, PitchX = %ld)", 
						lOffsetX, lOffsetXinStep, lPitchXinStep, lOffsetY, lOffsetYinStep, lPitchYinStep);
		}
		
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
	}
	else
	{
		if (bCheckDownDie)		//2nd die on right side
		{
			b2ndDie = SearchGoodDieXYinFOV(lTX, lTY, BT_PR_BOTTOM);
		}
		else
		{
			b2ndDie = SearchGoodDieXYinFOV(lTX, lTY, BT_PR_TOP);
		}

		if (!bCDie || !b2ndDie)
		{
			bUse2ndDieOffset = FALSE;
			lOffsetX = 0;
			lOffsetY = 0;

			szLog = "1st+1 Die XY (VERTICAL) - fail because 2nd die not found"; 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			return FALSE;
		}


		LONG lPitchXinStep = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBinBlockID)));
		LONG lPitchYinStep = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBinBlockID));

		bUse2ndDieOffset = TRUE;

		LONG lOffsetXinStep = lCX - lTX;
		LONG lOffsetYinStep = lCY - lTY;
		if (!bCheckDownDie)
			lOffsetXinStep	= lTY - lCY;	


		lOffsetX = ConvertXEncoderValueToFileUnit(lOffsetXinStep - lPitchXinStep);		//steps -> um
		lOffsetY = ConvertXEncoderValueToFileUnit(lOffsetYinStep);						//steps -> um

		if (bCheckDownDie)		//2nd die on right side
		{
			szLog.Format("1st+1 DOWN Die XY - X Offset = %ld um (X-Dist = %ld, PitchX = %ld), Y Offset = %ld um (Y-Dist = %ld, PitchX = %ld)", 
						lOffsetX, lOffsetXinStep, lPitchXinStep, lOffsetY, lOffsetYinStep, lPitchYinStep);
		}
		else
		{
			szLog.Format("1st+1 UP Die XY - X Offset = %ld um (X-Dist = %ld, PitchX = %ld), Y Offset = %ld um (Y-Dist = %ld, PitchX = %ld)", 
						lOffsetX, lOffsetXinStep, lPitchXinStep, lOffsetY, lOffsetYinStep, lPitchYinStep);
		}
		
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
	}
	
	return TRUE;
}


BOOL CBinTable::Check3rdGoodDieRelOffsetXYinFOV(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, 
		CString &szError, BOOL &bTopDieOK, BOOL &bSideDieOK)
{
	CString szLog;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	szLog.Format("START RelOffset XY Check (Semitek) - BLK=%lu, FOV=%.2f", ulBinBlockID, dFOVSize);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

	if (dFOVSize < BPR_LF_SIZE) 
	{
		szLog.Format("RelOffset XY Check - disabled due to small FOV size (%.2f)", dFOVSize);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		szError = "OK";
		return FALSE;
	}

	ULONG ulWalkingPath = m_oBinBlkMain.GrabWalkPath(ulBinBlockID);

	//ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBinBlockID);
	ULONG ulNoOfSortedDieInNV = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBinBlockID);
	ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVCurrBondIndex(ulBinBlockID);
	ULONG ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBinBlockID); 
	
	BOOL bCheckTopDie = FALSE;
	BOOL bCheckDownDie = FALSE;

	//v4.59A31
	if ( (ulWalkingPath == BT_TRV_PATH) || (ulWalkingPath == BT_BRV_PATH) ||
		 (ulWalkingPath == BT_TLV_PATH) || (ulWalkingPath == BT_BLV_PATH) )
	{
		bCheckTopDie	= TRUE;
		bCheckDownDie	= TRUE;
	}
	else
	{
		if (ulNoOfSortedDie > ulDiePerRow)				// If 1st row
		{
			bCheckTopDie = TRUE;
		}
	}
	

	BOOL bIsFirstDieInRow = FALSE;
	if ((ulNoOfSortedDie % ulDiePerRow) == 1)			// If 1st die on the row
	{
		bIsFirstDieInRow = TRUE;
	}

	BOOL bCheckLeftDie = TRUE;

	if ( (ulWalkingPath == BT_TRV_PATH) || (ulWalkingPath == BT_BRV_PATH) )			//v4.59A31
	{
		bCheckLeftDie = FALSE;
	}
	else if ( (ulWalkingPath == BT_TLV_PATH) || (ulWalkingPath == BT_BLV_PATH) )	//v4.59A31
	{
		bCheckLeftDie = TRUE;
	}
	else
	{
		INT nCol = ulNoOfSortedDie / ulDiePerRow;
		if ((ulNoOfSortedDie % ulDiePerRow) == 0)		// If last die on the row
		{
			if ((nCol % 2) == 0)	
			{
				bCheckLeftDie = FALSE;	//ODD row 2,4,6,....
			}
			else
			{
				bCheckLeftDie = TRUE;	//EVEN row 1,3,5,....
			}
		}
		else
		{
			if (nCol == 0)	
			{
				bCheckLeftDie = TRUE;	//Row 1
			}
			if ((nCol % 2) == 0)	
			{
				bCheckLeftDie = TRUE;	//LastDie on row 3,5,7,....
			}
			else
			{
				bCheckLeftDie = FALSE;	//LastDie on row 2,4,5,....
			}
		}
	}

	szLog.Format("RelOffset XY - Check Criteria : NVDices=%lu, IndexDices=%lu, DiePreRow=%lu, T=%d, DN=%d, 1DRow=%d, L=%d", 
				 ulNoOfSortedDieInNV, ulNoOfSortedDie, ulDiePerRow, 
				 bCheckTopDie, bCheckDownDie, bIsFirstDieInRow, bCheckLeftDie);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");


	LONG lCX = 0, lCY = 0;
	LONG lTX = 0, lTY = 0;
	LONG lDX = 0, lDY = 0;
	LONG lSX = 0, lSY = 0;
	BOOL bCDie = FALSE, bTopDie = FALSE, bDownDie = FALSE, bLeftDie = FALSE, bRightDie = FALSE;

	bCDie = SearchGoodDieXYinFOV(lCX, lCY, BT_PR_CENTER);		//Latch or first image that has to be grabbed
	if (bCheckTopDie)
	{
		bTopDie = SearchGoodDieXYinFOV(lTX, lTY, BT_PR_TOP);
	}
	if (bCheckDownDie)
	{
		bDownDie = SearchGoodDieXYinFOV(lDX, lDY, BT_PR_BOTTOM);	//v4.59A33
	}

	//v4.59A31
	if ( (ulWalkingPath == BT_TRV_PATH) || (ulWalkingPath == BT_BRV_PATH) ||
		 (ulWalkingPath == BT_TLV_PATH) || (ulWalkingPath == BT_BLV_PATH) )
	{
		if (bCheckLeftDie)
		{
			bLeftDie = SearchGoodDieXYinFOV(lSX, lSY, BT_PR_LEFT);
		}
		else
		{
			bRightDie = SearchGoodDieXYinFOV(lSX, lSY, BT_PR_RIGHT);
		}
	}
	else
	{
		if (!bIsFirstDieInRow)
		{
			if (bCheckLeftDie)
			{
				bLeftDie = SearchGoodDieXYinFOV(lSX, lSY, BT_PR_LEFT);
			}
			else
			{
				bRightDie = SearchGoodDieXYinFOV(lSX, lSY, BT_PR_RIGHT);
			}
		}
	}

	szLog.Format("RelOffset XY - SrchDie: C=%d, T=%d, D=%d, L=%d, R=%d", bCDie, bTopDie, bDownDie, bLeftDie, bRightDie);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	//if (!bCDie)
	//{
	//	szLog = "RelOffset XY Check - fails because Center Die is not found for comparison";
	//	szError = szLog;
	//	return FALSE;
	//}


	DOUBLE dX = 0, dSideX =0;
	DOUBLE dY = 0, dTopY = 0;

	//lCX = (*m_psmfSRam)["BondPr"]["CursorCenter"]["X"];		
	//lCY = (*m_psmfSRam)["BondPr"]["CursorCenter"]["Y"];	
	//GetEncoderValue();
	//lCX = m_lEnc_X;
	//lCY = m_lEnc_Y;

	//v4.51A14
	ULONG ulPitchXInEnc = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBinBlockID)));	//in ENC
	ULONG ulPitchYInEnc = labs(ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBinBlockID)));	//in ENC
/*
	if (bTopDie)
	{
		dX		= labs(lTX);
		dTopY	= labs(labs(lTY));// - ulPitchYInEnc); TY is relative to centre
	}
	else if (bDownDie)	//v4.59A31
	{
		dX		= labs(lDX);
		dTopY	= labs(labs(lDY));// - ulPitchYInEnc);	
	}

	if (bLeftDie || bRightDie)
	{
		dSideX	= labs(lSX);		//v4.59A33	//LeoLam 20171109
		dY		= labs(lSY);
	}
*/

	//v4.59A34
	if (bTopDie)
	{
		dX		= labs(lTX);
		//dTopY	= labs(labs(lTY));		// - ulPitchYInEnc); TY is relative to centre
		dTopY	= labs(labs(lTY) - labs(ulPitchYInEnc));
	}
	else if (bDownDie)	//v4.59A31
	{
		dX		= labs(lDX);
		dTopY	= labs(labs(lDY) - labs(ulPitchYInEnc));	// - ulPitchYInEnc);	
	}

	if (bLeftDie || bRightDie)
	{
		dSideX	= labs(labs(lSX) - labs(ulPitchXInEnc));	//v4.59A33	//LeoLam 20171109
		dY		= labs(lSY);
	}


	DOUBLE dXinMil = 0, dSideXinMil = 0, dTopYinMil = 0, dYinMil = 0;
	dXinMil		= (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * dX * 1000 / 25.4);
	dSideXinMil	= (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * dSideX * 1000 / 25.4);
	dTopYinMil	= (GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y) * dTopY * 1000 / 25.4);
	dYinMil		= (GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y) * dY * 1000 / 25.4);

	if (bTopDie)
	{
		szLog.Format("RelOffset XY (TOP Die) - X Offset = %.5f mil (TX = %ld steps); Tol = %.2f mil", dXinMil, lTX, m_dAlignLastDieRelOffsetX);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		
		szLog.Format("RelOffset XY (TOP Die) - Y Offset = %.5f mil (TY = %ld steps, Y-Pitch = %ld steps, dTopY = %.5f); Tol = %.2f mil", 
							dTopYinMil, lTY, ulPitchYInEnc, dTopY, m_dAlignLastDieRelOffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	}
	else if (bDownDie)
	{
		szLog.Format("RelOffset XY (DOWN Die) - X Offset = %.2f mil (DX = %ld steps); Tol = %.2f mil", dXinMil, lDX, m_dAlignLastDieRelOffsetX);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		
		szLog.Format("RelOffset XY (DOWN Die) - Y Offset = %.2f mil (DY = %ld steps, Y-Pitch = %ld steps, dTopY = %.2f); Tol = %.2f mil", 
							dTopYinMil, lDY, ulPitchYInEnc, dTopY, m_dAlignLastDieRelOffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	}
	
	if (bLeftDie)
	{
		szLog.Format("RelOffset XY (LEFT Die) - Y Offset = %.2f mil (SY = %ld steps); Tol = %.2f mil", dYinMil, lSY, m_dAlignLastDieRelOffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		szLog.Format("RelOffset XY (LEFT Die) - X Offset = %.2f mil (SideX = %.2f steps); Tol = %.2f mil", dSideXinMil, dSideX, m_dAlignLastDieRelOffsetX);
	}
	else
	{
		szLog.Format("RelOffset XY (RIGHT Die) - Y Offset = %.2f mil (SY = %ld steps); Tol = %.2f mil", dYinMil, lSY, m_dAlignLastDieRelOffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		szLog.Format("RelOffset XY (RIGHT Die) - X Offset = %.2f mil (SideX = %.2f steps); Tol = %.2f mil", dSideXinMil, dSideX, m_dAlignLastDieRelOffsetX);

	}
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	if (m_dAlignLastDieRelOffsetX > 0)
	{
		if ((dXinMil > m_dAlignLastDieRelOffsetX) || (dSideXinMil > m_dAlignLastDieRelOffsetX))
		{
			szLog = "RelOffset XY - X Check fails";
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			return FALSE;
		}
		else
		{
			if (bTopDie)
			{
				bTopDieOK = TRUE;
			}
		}
	}

	if (m_dAlignLastDieRelOffsetY > 0)
	{
		if (bTopDie)
		{
			if (dTopYinMil > m_dAlignLastDieRelOffsetY)
			{
				szLog = "RelOffset XY - Y Check (TOP Die) fails";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
				return FALSE;
			}
		}

		if (dYinMil > m_dAlignLastDieRelOffsetY)
		{
			szLog = "RelOffset XY - Y Check fails";
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			return FALSE;
		}
		else
		{
			if (bLeftDie || bRightDie)
			{
				bSideDieOK = TRUE;
			}
		}
	}

	szLog = "RelOffset XY - XY Check completed & OK";
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	return TRUE;
}



LONG CBinTable::Search3rdGoodDie(BOOL &bPlacement, DOUBLE &dX, DOUBLE &dY, BOOL &bDXFail, BOOL &bDYFail)
{
	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	int nConvID = 0;
	LONG lX, lY;
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;
	bPlacement = TRUE;
/*
	// Get the reply
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDie", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
*/
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		pBondPr->BT_SearchDie(stMsg);
	}

	stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
	if (!stInfo.bResult)
	{
		return FALSE;
	}


	//Convert motor into mil	
	double dXinMil = 0, dYinMil = 0;
	dXinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * (DOUBLE)stInfo.siStepX * 1000 / 25.4);
	dYinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y) * (DOUBLE)stInfo.siStepY * 1000 / 25.4);
	dX = stInfo.siStepX;	//Count;
	dY = stInfo.siStepY;	//Count;

	//v4.52A9
	DOUBLE dLastDieOffsetX = m_dAlignLastDieOffsetX;
	DOUBLE dLastDieOffsetY = m_dAlignLastDieOffsetY;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetCustomerName() == "Lumileds") && (pApp->GetProductLine() == "DieFab") )	//v4.52A9	//PLSG
	{
		//v4.52A9	//PLSG
		dLastDieOffsetX = 0;
	}

	if ((m_dAlignLastDieOffsetX == 0.0) && (m_dAlignLastDieOffsetY == 0.0))
	{
		bPlacement = TRUE;
	}
	else if ((dLastDieOffsetX > 0.0) && (fabs(dXinMil) > (dLastDieOffsetX)) )	//v4.52A9
	{
		bPlacement = FALSE;
		bDXFail = TRUE;
	}	
	else if ((dLastDieOffsetY > 0.0) && (fabs(dYinMil) > (dLastDieOffsetY)) )	//v4.52A9
	{
		bPlacement = FALSE;
		bDYFail = TRUE;
	}


	CString szLog = "";
	szLog.Format("Search last die dX = %.2f mil (step=%d tol=%.2f), dY = %.2f mil (step=%d, tol=%.2f)",	//v3.62
				 dXinMil, stInfo.siStepX, m_dAlignLastDieOffsetX, dYinMil, stInfo.siStepY, m_dAlignLastDieOffsetY);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
	
	//v2.93T2
	GetXYEncoderValue(lX, lY);
	BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);

	//v3.71T1
	szLog.Format("REALIGN LastDie ENC POS XY = ,%d,%d,  Offset = ,%d,%d", 
				 lX + stInfo.siStepX, lY + stInfo.siStepY, stInfo.siStepX, stInfo.siStepY);
	CMSLogFileUtility::Instance()->BT_LogStatus(szLog);

	return TRUE;
}


LONG CBinTable::SearchGoodDie(BOOL bMove, BOOL bUseBT2)
{
	typedef struct 
	{
		int		siStepX;
		int		siStepY;
		BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	int nConvID = 0;
	LONG lX, lY;
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;


	// Get the reply
//	if (bUseBT2)
//	{
//		nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchDie", stMsg);    //v4.35T4
//	}		
//	else
	{
		//nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDie", stMsg);
		CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
		if (pBondPr != NULL)
		{
			pBondPr->BT_SearchDie(stMsg);
		}
	}
/*
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
*/
	stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
	if (stInfo.bResult)
	{
		if (bMove)
		{
//			if (bUseBT2)
//			{
//				GetXYEncoderValue(lX, lY, TRUE);					//Returned XY encoder values are based on BT1, not BT2
//				BT2_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);	//BT1 values are passed into this fcn for further transformation inside it
//			}
//			else
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
			}
			Sleep(50);
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CBinTable::SearchGoodDieinFOV(LONG lDirection, LONG lLookAheadDieNum)
{
	typedef struct 
	{
		LONG	lDirection;
		LONG	lLookAheadDieNum;
		BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_SrchDie;
	BPR_SrchDie stInfo;
	BPR_SrchDie stResult;

	BOOL bIsGoodDie = TRUE;

	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stInfo.lDirection = lDirection;
	stInfo.lLookAheadDieNum = lLookAheadDieNum;
	stInfo.bResult = FALSE;
	stInfo.iPrX = 0;//nPrOffsetX;
	stInfo.iPrY = 0;//nPrOffsetY;
	stMsg.InitMessage(sizeof(BPR_SrchDie), &stInfo);
/*
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDieInFOV", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_SrchDie), &stResult);		//v4.26T1	//Semitek
			//stMsg.GetMsg(sizeof(BOOL), &bIsGoodDie);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
*/
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		pBondPr->BT_SearchDieInFOV(stMsg);
	}
	stMsg.GetMsg(sizeof(BPR_SrchDie), &stResult);

	bIsGoodDie = stResult.bResult;			//v4.26T1	//Semitek
	return bIsGoodDie;
}


BOOL CBinTable::SearchGoodDieXYinFOV(LONG &lX, LONG &lY, LONG lDirection, LONG lLookAheadDieNum, LONG lSearchRangeRatio, BOOL bLookAheadDieOffset)
{
	typedef struct 
	{
		LONG	lDirection;
		LONG	lLookAheadDieNum;
		BOOL    bResult;
		int		iPrX;
		int		iPrY;
		int		lSearchRangeRatio;
		BOOL	bLookAheadDieOffset;
	} BPR_SrchDie;
	BPR_SrchDie stInfo;
	BPR_SrchDie stResult;

	BOOL bIsGoodDie = TRUE;

	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stInfo.lDirection = lDirection;
	stInfo.lLookAheadDieNum = lLookAheadDieNum;
	stInfo.bResult = FALSE;
	stInfo.iPrX = 0;//nPrOffsetX;
	stInfo.iPrY = 0;//nPrOffsetY;
	stInfo.lSearchRangeRatio = lSearchRangeRatio;
	stInfo.bLookAheadDieOffset = bLookAheadDieOffset;
	stMsg.InitMessage(sizeof(BPR_SrchDie), &stInfo);
/*
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT_SearchDieXYInFOV", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BPR_SrchDie), &stResult);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
*/
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		pBondPr->BT_SearchDieXYInFOV(stMsg);
	}
	stMsg.GetMsg(sizeof(BPR_SrchDie), &stResult);

	lX = stResult.iPrX;		//X in motor steps
	lY = stResult.iPrY;		//Y in motor steps
	bIsGoodDie = stResult.bResult;
	return bIsGoodDie;
}

BOOL CBinTable::Calculate1PtRealignAngleInFOV(DOUBLE& dAngle, ULONG ulBinBlockID, DOUBLE dFOVSize)
{
	CString szLog;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		dAngle = 0;
			
		szLog.Format("1st Realign Angle Cal: FOV size (%.1f) too small", dFOVSize);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		return TRUE;
	}
/*
#define		BPR_CT_DIE					0	//CENTER
#define		BPR_LT_DIE					1	//LHS
#define		BPR_RT_DIE					5	//RHS	
#define		BPR_UP_DIE					3	//UP
#define		BPR_DN_DIE					7	//DOWN
#define		BPR_UL_DIE					2	//UP+LHS
#define		BPR_DL_DIE					8	//DOWN+LHS	
#define		BPR_UR_DIE					4	//UP+RHS
#define		BPR_DR_DIE					6	//DOWN+RHS
*/
	LONG lEncX=0, lEncY=0;
	GetXYEncoderValue(lEncX, lEncY);

	BOOL bDie1=FALSE, bDie2=FALSE;
	LONG lCX=0, lCY=0;
	LONG lX1=0, lY1=0;
	LONG lX2=0, lY2=0;
	BOOL bCDie = SearchGoodDieXYinFOV(lCX, lCY, BT_PR_CENTER);		//Latch or first image that has to be grabbed
	
	if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TLH_PATH) 
	{
		bDie1 = SearchGoodDieXYinFOV(lX1, lY1, BT_PR_TOP_LEFT);	
		bDie2 = SearchGoodDieXYinFOV(lX2, lY2, BT_PR_TOP_RIGHT);	
	}
	else if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TRH_PATH) 
	{
		bDie1 = SearchGoodDieXYinFOV(lX1, lY1, BT_PR_TOP);	
		bDie2 = SearchGoodDieXYinFOV(lX2, lY2, BT_PR_LEFT);	
	}

	return TRUE;
}	

VOID CBinTable::DisplayBondPrSearchWindow(CONST BOOL bDisplay, CONST BOOL bLF, CONST LONG lDirection)
{
	BOOL bReturn = FALSE;
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
		BOOL bDisplay;
		BOOL bLF;
		LONG lDirection;
	} BPR_DISPLAY_SA;

	BPR_DISPLAY_SA stInfo;
	stInfo.bDisplay = bDisplay;
	stInfo.bLF = bLF;
	stInfo.lDirection = lDirection;
	stMsg.InitMessage(sizeof(BPR_DISPLAY_SA), &stInfo);

/*
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "DrawSearchWindow", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
*/

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		pBondPr->DrawSearchWindow(stMsg);
	}
}

VOID CBinTable::DisplayBondPrDieSizeWindow(BOOL bDisplayHomeCursor)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BOOL), &bDisplayHomeCursor);
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "DrawDieSizeWindow", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bDisplayHomeCursor);
			break;
		}
		else
		{
			Sleep(1);
		}
	}
}


BOOL CBinTable::RetryFindGoodDie(const LONG lPitchX, const LONG lPitchY, const LONG lDirX, const LONG lDirY, 
								 const LONG lDirection1, const LONG lDirection2, const LONG lDirection3, const BOOL bUseBT2, const BOOL bRetry)
{
	if (!bRetry)
	{
		return FALSE;
	}
	//Retry Continue 3 Die in Left
	LONG lX, lY, lCount = 0;
	LONG lRetryCount = 3;
	LONG lOrgX = 0, lOrgY = 0;

	GetXYEncoderValue(lOrgX, lOrgY, bUseBT2);

	while  (lCount++ < lRetryCount)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);

		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(lDirection1) == TRUE) ||
			(SearchGoodDieinFOV(lDirection2) == TRUE) ||
			(SearchGoodDieinFOV(lDirection3) == TRUE))
		{
			return TRUE;
			break;
		}
		BT_MoveTo(lX + lPitchX * lDirX, lY + lPitchX * lDirY, TRUE, bUseBT2);
	}

	BT_MoveTo(lOrgX, lOrgY, TRUE, bUseBT2);
	return FALSE;
}

LONG CBinTable::FindTopLeftDie(const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize, const BOOL bUseBT2, BOOL bRetry)
{
	LONG lX, lY, lCount = 0;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(BT_PR_TOP_LEFT) == FALSE) && 
				(SearchGoodDieinFOV(BT_PR_TOP) == FALSE) && 
				(SearchGoodDieinFOV(BT_PR_LEFT) == FALSE))
		{
			//Bingo (Already on TL)
			//Retry Continue 3 Die in Left
			if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
								 BT_PR_TOP_LEFT, BT_PR_TOP, BT_PR_LEFT, bUseBT2, bRetry))
			{
				return TRUE;
			}
		}

		while (1)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);

			if (SearchGoodDieinFOV(BT_PR_TOP_LEFT) == TRUE)		//Move to TL if die exist (1st choice)
			{
				BT_MoveTo(lX - lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else if (SearchGoodDieinFOV(BT_PR_TOP) == TRUE)	//Move to UP if die exist (2nd choice)
			{
				BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else if (SearchGoodDieinFOV(BT_PR_LEFT) == TRUE)	//Move to LT if die exist (3rd choice)
			{
				BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			}
			else
			{
				if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
									 BT_PR_TOP_LEFT, BT_PR_TOP, BT_PR_LEFT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}

			SearchGoodDie(TRUE, bUseBT2);

			lCount++;
			if (lCount > 20)
			{
				return FALSE;
			}
		}
		return TRUE;
	}


	// search to top first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);

		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);

			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX, lY - 2 * lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
									 BT_PR_TOP_LEFT, BT_PR_TOP, BT_PR_LEFT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	// search to left first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);

		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);

			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX + lPitchX * BT_X_DIR * 2, lY, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
									 BT_PR_TOP_LEFT, BT_PR_TOP, BT_PR_LEFT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	return TRUE;
}

LONG CBinTable::FindTopRightDie(const ULONG ulWalkPath, const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize, const BOOL bUseBT2, const BOOL bRetry)
{
	LONG lX, lY, lCount = 0;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(BT_PR_TOP_RIGHT) == FALSE) && (SearchGoodDieinFOV(BT_PR_TOP) == FALSE) && (SearchGoodDieinFOV(BT_PR_RIGHT) == FALSE))
		{
			//Bingo (Already on TR)
			//Retry Continue 3 Die in Left
			if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
								 BT_PR_TOP_RIGHT, BT_PR_TOP, BT_PR_RIGHT, bUseBT2, bRetry))
			{
				return TRUE;
			}
		}

		while (1)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);

			if (ulWalkPath == BT_TRV_PATH)
			{
				if (SearchGoodDieinFOV(BT_PR_RIGHT) == TRUE)	//Move to RT if die exist (3rd choice)
				{
					BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_TOP_RIGHT) == TRUE)		//Move to TR if die exist (1st choice)
				{
					BT_MoveTo(lX + lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_TOP) == TRUE)	//Move to UP if die exist (2nd choice)
				{
					BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else
				{
					if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
										 BT_PR_TOP_RIGHT, BT_PR_TOP, BT_PR_RIGHT, bUseBT2, bRetry))
					{
						break;	//Bingo
					}
				}
			}
			else
			{
				if (SearchGoodDieinFOV(BT_PR_TOP_RIGHT) == TRUE)		//Move to TR if die exist (1st choice)
				{
					BT_MoveTo(lX + lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_TOP) == TRUE)	//Move to UP if die exist (2nd choice)
				{
					BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_RIGHT) == TRUE)	//Move to RT if die exist (3rd choice)
				{
					BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
				}
				else
				{
					if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
										 BT_PR_TOP_RIGHT, BT_PR_TOP, BT_PR_RIGHT, bUseBT2, bRetry))
					{
						break;	//Bingo
					}
				}
			}

			SearchGoodDie(TRUE, bUseBT2);

			lCount++;
			if (lCount > 20)
			{
				return FALSE;
			}
		}
		return TRUE;
	}


	// search to top first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX, lY - 2 * lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
									 BT_PR_TOP_RIGHT, BT_PR_TOP, BT_PR_RIGHT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	// search to right first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX - lPitchX * BT_X_DIR * 2, lY, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
									 BT_PR_TOP_RIGHT, BT_PR_TOP, BT_PR_RIGHT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	return TRUE;
}


LONG CBinTable::FindBottomLeftDie(const ULONG ulWalkPath, const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize, const BOOL bUseBT2, const BOOL bRetry)
{
	LONG lX, lY, lCount = 0;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT) == FALSE) && (SearchGoodDieinFOV(BT_PR_BOTTOM) == FALSE) && (SearchGoodDieinFOV(BT_PR_LEFT) == FALSE))
		{
			//Bingo (Already on TL)
			//Retry Continue 3 Die in Left
			if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
								 BT_PR_BOTTOM_LEFT, BT_PR_BOTTOM, BT_PR_LEFT, bUseBT2, bRetry))
			{
				return TRUE;
			}
		}

		while (1)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);

			//v4.59A31
			if (ulWalkPath == BT_BLV_PATH)
			{
				if (SearchGoodDieinFOV(BT_PR_LEFT) == TRUE)					//Move to LT if die exist (3rd choice)
				{
					BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_TOP_LEFT) == TRUE)		//Move to Top-LT if die exist (1st choice)
				{
					BT_MoveTo(lX - lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT) == TRUE)		//Move to BL-LT if die exist (1st choice)
				{
					BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_BOTTOM) == TRUE)			//Move to DN if die exist (2nd choice)
				{
					BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else
				{
					if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
										 BT_PR_BOTTOM_LEFT, BT_PR_BOTTOM, BT_PR_LEFT, bUseBT2, bRetry))
					{
						break;	//Bingo
					}
				}
			}
			else
			{
				if (SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT) == TRUE)		//Move to BL if die exist (1st choice)
				{
					BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_BOTTOM) == TRUE)	//Move to DN if die exist (2nd choice)
				{
					BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else if (SearchGoodDieinFOV(BT_PR_LEFT) == TRUE)	//Move to LT if die exist (3rd choice)
				{
					BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
				}
				else
				{
					if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
										 BT_PR_BOTTOM_LEFT, BT_PR_BOTTOM, BT_PR_LEFT, bUseBT2, bRetry))
					{
						break;	//Bingo
					}
				}
			}

			Sleep(100);
			SearchGoodDie(TRUE, bUseBT2);
			Sleep(100);

			lCount++;
			if (lCount > 20)
			{
				return FALSE;
			}
		}
		return TRUE;
	}


	// search to down first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX, lY + 2 * lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
									 BT_PR_BOTTOM_LEFT, BT_PR_BOTTOM, BT_PR_LEFT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	// search to left first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX + lPitchX * BT_X_DIR * 2, lY, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, -BT_X_DIR, 0, 
									 BT_PR_BOTTOM_LEFT, BT_PR_BOTTOM, BT_PR_LEFT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	return TRUE;
}


LONG CBinTable::FindBottomRightDie(const LONG lPitchX, const LONG lPitchY, const DOUBLE dFOVSize, const BOOL bUseBT2, const BOOL bRetry)
{
	LONG lX, lY, lCount = 0;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT) == FALSE) && (SearchGoodDieinFOV(BT_PR_BOTTOM) == FALSE) && (SearchGoodDieinFOV(BT_PR_RIGHT) == FALSE))
		{
			//Bingo (Already on TR)
			//Bingo (Already on TL)
			//Retry Continue 3 Die in Left
			if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
								 BT_PR_BOTTOM_RIGHT, BT_PR_BOTTOM, BT_PR_RIGHT, bUseBT2, bRetry))
			{
				return TRUE;
			}
		}

		while (1)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			if (SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT) == TRUE)		//Move to BR if die exist (1st choice)
			{
				BT_MoveTo(lX + lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else if (SearchGoodDieinFOV(BT_PR_BOTTOM) == TRUE)	//Move to DN if die exist (2nd choice)
			{
				BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else if (SearchGoodDieinFOV(BT_PR_RIGHT) == TRUE)	//Move to RT if die exist (3rd choice)
			{
				BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			}
			else
			{
				if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
									 BT_PR_BOTTOM_RIGHT, BT_PR_BOTTOM, BT_PR_RIGHT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
			SearchGoodDie(TRUE, bUseBT2);

			lCount++;
			if (lCount > 20)
			{
				return FALSE;
			}
		}
		return TRUE;
	}


	// search to down first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX, lY + 2 * lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
									 BT_PR_BOTTOM_RIGHT, BT_PR_BOTTOM, BT_PR_RIGHT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	// search to right first
	while (1)
	{
		GetXYEncoderValue(lX, lY, bUseBT2);
		BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
		if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
		{
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			if (SearchGoodDie(TRUE, bUseBT2) == FALSE)
			{
				GetXYEncoderValue(lX, lY, bUseBT2);
				BT_MoveTo(lX - lPitchX * BT_X_DIR * 2, lY, TRUE, bUseBT2);
				if (!RetryFindGoodDie(lPitchX, lPitchY, BT_X_DIR, 0, 
									 BT_PR_BOTTOM_RIGHT, BT_PR_BOTTOM, BT_PR_RIGHT, bUseBT2, bRetry))
				{
					break;	//Bingo
				}
			}
		}
	}

	return TRUE;
}


LONG CBinTable::Find9MatrixDieState(LONG lMethod, LONG lPitchX, LONG lPitchY, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lX, lY;

	for (int i = 0; i < 9; i++)
	{
		m_bMatrix_9[i] = FALSE;
	}


	if (dFOVSize >= BPR_LF_SIZE)
	{
		//use LF
		switch (lMethod)
		{
			case BT_PR_ALL_SIDE:
				m_bMatrix_9[BT_PR_CENTER]		= SearchGoodDieinFOV(BT_PR_CENTER);
				m_bMatrix_9[BT_PR_LEFT]			= SearchGoodDieinFOV(BT_PR_LEFT);
				m_bMatrix_9[BT_PR_TOP_LEFT]		= SearchGoodDieinFOV(BT_PR_TOP_LEFT);
				m_bMatrix_9[BT_PR_TOP]			= SearchGoodDieinFOV(BT_PR_TOP);
				m_bMatrix_9[BT_PR_TOP_RIGHT]	= SearchGoodDieinFOV(BT_PR_TOP_RIGHT);
				m_bMatrix_9[BT_PR_RIGHT]		= SearchGoodDieinFOV(BT_PR_RIGHT);
				m_bMatrix_9[BT_PR_BOTTOM_RIGHT] = SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT);
				m_bMatrix_9[BT_PR_BOTTOM]		= SearchGoodDieinFOV(BT_PR_BOTTOM);
				m_bMatrix_9[BT_PR_BOTTOM_LEFT]	= SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT);
				break;

			case BT_PR_TOP_LEFT:
				m_bMatrix_9[BT_PR_CENTER]		= SearchGoodDieinFOV(BT_PR_CENTER);
				m_bMatrix_9[BT_PR_LEFT]			= SearchGoodDieinFOV(BT_PR_LEFT);
				m_bMatrix_9[BT_PR_TOP_LEFT]		= SearchGoodDieinFOV(BT_PR_TOP_LEFT);
				m_bMatrix_9[BT_PR_TOP]			= SearchGoodDieinFOV(BT_PR_TOP);
				m_bMatrix_9[BT_PR_TOP_RIGHT]	= SearchGoodDieinFOV(BT_PR_TOP_RIGHT);			//v2.58
				m_bMatrix_9[BT_PR_BOTTOM_LEFT]	= SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT);			//v2.58
				break;

			case BT_PR_TOP_RIGHT:
				m_bMatrix_9[BT_PR_CENTER]		= SearchGoodDieinFOV(BT_PR_CENTER);
				m_bMatrix_9[BT_PR_TOP_LEFT]		= SearchGoodDieinFOV(BT_PR_TOP_LEFT);			//v2.58
				m_bMatrix_9[BT_PR_TOP]			= SearchGoodDieinFOV(BT_PR_TOP);
				m_bMatrix_9[BT_PR_TOP_RIGHT]	= SearchGoodDieinFOV(BT_PR_TOP_RIGHT);
				m_bMatrix_9[BT_PR_RIGHT]		= SearchGoodDieinFOV(BT_PR_RIGHT);
				m_bMatrix_9[BT_PR_BOTTOM_RIGHT] = SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT);			//v2.58
				break;

			case BT_PR_BOTTOM_LEFT:
				m_bMatrix_9[BT_PR_CENTER]		= SearchGoodDieinFOV(BT_PR_CENTER);
				m_bMatrix_9[BT_PR_LEFT]			= SearchGoodDieinFOV(BT_PR_LEFT);
				m_bMatrix_9[BT_PR_TOP_LEFT]		= SearchGoodDieinFOV(BT_PR_TOP_LEFT);
				m_bMatrix_9[BT_PR_BOTTOM_RIGHT] = SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT);
				m_bMatrix_9[BT_PR_BOTTOM]		= SearchGoodDieinFOV(BT_PR_BOTTOM);			
				m_bMatrix_9[BT_PR_BOTTOM_LEFT]	= SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT);			
				break;

			case BT_PR_BOTTOM_RIGHT:
				m_bMatrix_9[BT_PR_CENTER]		= SearchGoodDieinFOV(BT_PR_CENTER);
				m_bMatrix_9[BT_PR_TOP_RIGHT]	= SearchGoodDieinFOV(BT_PR_TOP_RIGHT);			
				m_bMatrix_9[BT_PR_RIGHT]		= SearchGoodDieinFOV(BT_PR_RIGHT);
				m_bMatrix_9[BT_PR_BOTTOM_RIGHT] = SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT);
				m_bMatrix_9[BT_PR_BOTTOM]		= SearchGoodDieinFOV(BT_PR_BOTTOM);
				m_bMatrix_9[BT_PR_BOTTOM_LEFT]	= SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT);			
				break;
		}

		return TRUE;
	}

	//   234
	//   105
	//   876
	switch (lMethod)
	{
		case BT_PR_ALL_SIDE:
			// center
			m_bMatrix_9[0] = SearchGoodDie(TRUE, bUseBT2);
			// left
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[1] = SearchGoodDie(TRUE, bUseBT2);
			// top-left
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[2] = SearchGoodDie(TRUE, bUseBT2);
			// top
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[3] = SearchGoodDie(TRUE, bUseBT2);
			// top-right
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[4] = SearchGoodDie(TRUE, bUseBT2);
			// right
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[5] = SearchGoodDie(TRUE, bUseBT2);
			// right-bottom
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[6] = SearchGoodDie(TRUE, bUseBT2);
			// bottom
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[7] = SearchGoodDie(TRUE, bUseBT2);
			// bottom-left
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[8] = SearchGoodDie(TRUE, bUseBT2);
			break;
		case BT_PR_TOP_LEFT:
			// center
			m_bMatrix_9[0] = SearchGoodDie(TRUE, bUseBT2);
			// left
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[1] = SearchGoodDie(TRUE, bUseBT2);
			// top-left
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[2] = SearchGoodDie(TRUE, bUseBT2);
			// top
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[3] = SearchGoodDie(TRUE, bUseBT2);
			break;
		case BT_PR_TOP_RIGHT:
			// center
			m_bMatrix_9[0] = SearchGoodDie(TRUE, bUseBT2);
			// top
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[3] = SearchGoodDie(TRUE, bUseBT2);
			// top-right
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[4] = SearchGoodDie(TRUE, bUseBT2);
			// right
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[5] = SearchGoodDie(TRUE, bUseBT2);
			break;
		case BT_PR_BOTTOM_LEFT:
			// center
			m_bMatrix_9[0] = SearchGoodDie(TRUE, bUseBT2);
			// left
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[1] = SearchGoodDie(TRUE, bUseBT2);
			// down-left
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[8] = SearchGoodDie(TRUE, bUseBT2);
			// down
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[7] = SearchGoodDie(TRUE, bUseBT2);
			break;
		case BT_PR_BOTTOM_RIGHT:
			// center
			m_bMatrix_9[0] = SearchGoodDie(TRUE, bUseBT2);
			// down
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[7] = SearchGoodDie(TRUE, bUseBT2);
			// down-right
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			m_bMatrix_9[6] = SearchGoodDie(TRUE, bUseBT2);
			// right
			GetXYEncoderValue(lX, lY, bUseBT2);
			BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			m_bMatrix_9[5] = SearchGoodDie(TRUE, bUseBT2);
			break;
	}
	// done 

	return TRUE;
}

// based on current postion and die pitch to search top-left die as the 1st die on bin block
// if found, return TRUE, else return FALSE;
// during searching, auto align the die
// in this function to locate top-left die
LONG CBinTable::SearchDieInTopLeft(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const BOOL bUseBT2, const BOOL bRetry)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));

	ULONG ulWalkPath = m_oBinBlkMain.GrabWalkPath(ulBlkInUse);

	// Search for top-left die with Look-forward mode
	lCounter = 0;
	lSrchStatus = FindTopLeftDie(lPitchX, lPitchY, dFOVSize, bUseBT2, bRetry);

	if (dFOVSize >= BPR_LF_SIZE)
	{
		DisplayBondPrSearchWindow(TRUE, TRUE, BT_PR_TOP_LEFT);	//v2.58
		return lSrchStatus;
	}

	//m_bMatrix_9[9]
	//2	3 4
	//1	0 5
	//8 7 6

	/*case BT_PR_BOTTOM_LEFT:
		m_bMatrix_9[BT_PR_CENTER]		= SearchGoodDieinFOV(BT_PR_CENTER);
		m_bMatrix_9[BT_PR_LEFT]			= SearchGoodDieinFOV(BT_PR_LEFT);
		m_bMatrix_9[BT_PR_TOP_LEFT]		= SearchGoodDieinFOV(BT_PR_TOP_LEFT);
		m_bMatrix_9[BT_PR_BOTTOM_RIGHT] = SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT);
		m_bMatrix_9[BT_PR_BOTTOM]		= SearchGoodDieinFOV(BT_PR_BOTTOM);			
		m_bMatrix_9[BT_PR_BOTTOM_LEFT]	= SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT);			
		break;
	*/

	while (1)
	{
		lCounter++;
		GetXYEncoderValue(lX, lY, bUseBT2);
		Find9MatrixDieState(BT_PR_TOP_LEFT, lPitchX, lPitchY, dFOVSize, bUseBT2);	// Update PR LF 3x3 matrix status
		if (m_bMatrix_9[2] == TRUE)
		{
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
		}
		else
		{
			if (m_bMatrix_9[1] == FALSE && m_bMatrix_9[3] == FALSE && m_bMatrix_9[4] == FALSE && m_bMatrix_9[8] == FALSE)
			{
				BT_MoveTo(lX, lY, TRUE, bUseBT2);
				return TRUE;
			}

			if (m_bMatrix_9[1] == FALSE && m_bMatrix_9[3] == FALSE && m_bMatrix_9[4] == TRUE)		//v2.58
			{
				BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			if (m_bMatrix_9[1] == FALSE && m_bMatrix_9[3] == FALSE && m_bMatrix_9[8] == TRUE)		//v2.58
			{
				BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			}
			else if (m_bMatrix_9[1] == TRUE && m_bMatrix_9[3] == TRUE)
			{
				BT_MoveTo(lX - lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else
			{
				if (m_bMatrix_9[3] == TRUE)
				{
					BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else
				{
					if (m_bMatrix_9[1] == TRUE)
					{
						BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
					}
				}
			}
		}

		if (lCounter > 10)
		{
			break;
		}
	}

	return FALSE;
}


LONG CBinTable::SearchDieInTopRight(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const BOOL bUseBT2, const BOOL bRetry)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));		//v2.56
	ULONG ulWalkPath = m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse);

	// Search for top-right die with Look-forward mode
	lCounter = 0;

	lSrchStatus = FindTopRightDie(ulWalkPath, lPitchX, lPitchY, dFOVSize, bUseBT2, bRetry);

	if (dFOVSize >= BPR_LF_SIZE)
	{
		DisplayBondPrSearchWindow(TRUE, TRUE, BT_PR_TOP_RIGHT);		//v2.58
		return lSrchStatus;
	}

	while (1)
	{
		lCounter++;
		GetXYEncoderValue(lX, lY, bUseBT2);
		Find9MatrixDieState(BT_PR_TOP_RIGHT, lPitchX, lPitchY, dFOVSize, bUseBT2);
		if (m_bMatrix_9[4] == TRUE)
		{
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
		}
		else
		{
			if (m_bMatrix_9[3] == FALSE && m_bMatrix_9[5] == FALSE && m_bMatrix_9[2] == FALSE && m_bMatrix_9[6] == FALSE)
			{
				BT_MoveTo(lX, lY, TRUE, bUseBT2);
				return TRUE;
			}

			if (m_bMatrix_9[3] == FALSE && m_bMatrix_9[5] == FALSE && m_bMatrix_9[2] == TRUE)		//v2.58
			{
				BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else if (m_bMatrix_9[3] == FALSE && m_bMatrix_9[5] == FALSE && m_bMatrix_9[6] == TRUE)	//v2.58
			{
				BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			}
			else if (m_bMatrix_9[3] == TRUE && m_bMatrix_9[5] == TRUE)
			{
				BT_MoveTo(lX + lPitchX * BT_X_DIR, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else
			{
				if (m_bMatrix_9[3] == TRUE)
				{
					BT_MoveTo(lX, lY + lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else
				{
					if (m_bMatrix_9[5] == TRUE)
					{
						BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
					}
				}
			}
		}
		if (lCounter > 10)
		{
			break;
		}
	}

	return FALSE;
}


LONG CBinTable::SearchDieInBottomLeft(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const BOOL bUseBT2, const BOOL bRetry)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));

	ULONG ulWalkPath = m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse);

	// Search for down-left die with Look-forward mode
	lCounter = 0;
	lSrchStatus = FindBottomLeftDie(ulWalkPath, lPitchX, lPitchY, dFOVSize, bUseBT2, bRetry);

	if (dFOVSize >= BPR_LF_SIZE)
	{
		DisplayBondPrSearchWindow(TRUE, TRUE, BT_PR_BOTTOM_LEFT);	//v2.58
		return lSrchStatus;
	}

	//m_bMatrix_9[9]
	//2	3 4
	//1	0 5
	//8 7 6

	/*case BT_PR_BOTTOM_LEFT:
		m_bMatrix_9[BT_PR_CENTER]		= SearchGoodDieinFOV(BT_PR_CENTER);
		m_bMatrix_9[BT_PR_LEFT]			= SearchGoodDieinFOV(BT_PR_LEFT);
		m_bMatrix_9[BT_PR_TOP_LEFT]		= SearchGoodDieinFOV(BT_PR_TOP_LEFT);
		m_bMatrix_9[BT_PR_BOTTOM_RIGHT] = SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT);
		m_bMatrix_9[BT_PR_BOTTOM]		= SearchGoodDieinFOV(BT_PR_BOTTOM);			
		m_bMatrix_9[BT_PR_BOTTOM_LEFT]	= SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT);			
		break;
*/
	CString szLog;

	while (1)
	{
		lCounter++;
		GetXYEncoderValue(lX, lY, bUseBT2);
		Find9MatrixDieState(BT_PR_BOTTOM_LEFT, lPitchX, lPitchY, dFOVSize, bUseBT2);	// Update PR LF 3x3 matrix status
	
		szLog.Format("SearchDieInBottomLeft: LEFT=%d, BLEFT=%d, BOTTOM=%d", 
					m_bMatrix_9[1], m_bMatrix_9[8], m_bMatrix_9[7]);

		if (m_bMatrix_9[8] == TRUE)
		{
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
		}
		else
		{
			if (m_bMatrix_9[1] == FALSE && m_bMatrix_9[2] == FALSE && m_bMatrix_9[7] == FALSE && m_bMatrix_9[6] == FALSE)
			{
				BT_MoveTo(lX, lY, TRUE, bUseBT2);
				return TRUE;
			}

			if (m_bMatrix_9[1] == FALSE && m_bMatrix_9[7] == FALSE && m_bMatrix_9[6] == TRUE)
			{
				BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			if (m_bMatrix_9[1] == FALSE && m_bMatrix_9[7] == FALSE && m_bMatrix_9[2] == TRUE)
			{
				BT_MoveTo(lX - lPitchX * BT_X_DIR, lY), TRUE, bUseBT2;
			}
			else if (m_bMatrix_9[1] == TRUE && m_bMatrix_9[7] == TRUE)
			{
				BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else
			{
				if (m_bMatrix_9[7] == TRUE)
				{
					BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else
				{
					if (m_bMatrix_9[1] == TRUE)
					{
						BT_MoveTo(lX - lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
					}
				}
			}
		}

		if (lCounter > 10)
		{
			break;
		}
	}

	return FALSE;
}


LONG CBinTable::SearchDieInBottomRight(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const BOOL bUseBT2, const BOOL bRetry)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));		//v2.56

	// Search for down-right die with Look-forward mode
	lCounter = 0;
	lSrchStatus = FindBottomRightDie(lPitchX, lPitchY, dFOVSize, bUseBT2, bRetry);

	if (dFOVSize >= BPR_LF_SIZE)
	{
		DisplayBondPrSearchWindow(TRUE, TRUE, BT_PR_BOTTOM_RIGHT);		//v2.58
		return lSrchStatus;
	}

	while (1)
	{
		lCounter++;
		GetXYEncoderValue(lX, lY, bUseBT2);
		Find9MatrixDieState(BT_PR_BOTTOM_RIGHT, lPitchX, lPitchY, dFOVSize, bUseBT2);
		if (m_bMatrix_9[6] == TRUE)
		{
			BT_MoveTo(lX + lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
		}
		else
		{
			if (m_bMatrix_9[7] == FALSE && m_bMatrix_9[5] == FALSE && m_bMatrix_9[4] == FALSE && m_bMatrix_9[8] == FALSE)
			{
				BT_MoveTo(lX, lY, TRUE, bUseBT2);
				return TRUE;
			}

			if (m_bMatrix_9[7] == FALSE && m_bMatrix_9[5] == FALSE && m_bMatrix_9[8] == TRUE)		//v2.58
			{
				BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else if (m_bMatrix_9[7] == FALSE && m_bMatrix_9[5] == FALSE && m_bMatrix_9[4] == TRUE)	//v2.58
			{
				BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
			}
			else if (m_bMatrix_9[7] == TRUE && m_bMatrix_9[5] == TRUE)
			{
				BT_MoveTo(lX + lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
			}
			else
			{
				if (m_bMatrix_9[7] == TRUE)
				{
					BT_MoveTo(lX, lY - lPitchY * BT_Y_DIR, TRUE, bUseBT2);
				}
				else
				{
					if (m_bMatrix_9[5] == TRUE)
					{
						BT_MoveTo(lX + lPitchX * BT_X_DIR, lY, TRUE, bUseBT2);
					}
				}
			}
		}
		if (lCounter > 10)
		{
			break;
		}
	}

	return FALSE;
}


LONG CBinTable::AlignEmptyFrame(ULONG ulBlkID)
{
	ULONG ulFirstDieIndex = 0;
	LONG lX = 0, lY = 0;
	LONG lDieFound = 0;
	CString str = "";
	BOOL bPostbond = FALSE;

	bPostbond = (BOOL)((LONG)(*m_psmfSRam)["BondPr"]["UsePostBond"]);

	CMS896AStn::m_bRealignFrameDone = FALSE;

	if (bPostbond == FALSE)
	{
		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkID);
		GetDieLogicalEncoderValue(ulBlkID, ulFirstDieIndex, lX, lY);
		BT_MoveTo(lX, lY);
		CMS896AStn::m_bRealignFrameDone = TRUE;
		return TRUE;
	}

	ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkID);
	GetDieLogicalEncoderValue(ulBlkID, ulFirstDieIndex, lX, lY);
	BT_MoveTo(lX, lY);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	//v2.58

	if (m_lAlignBinDelay > 0)
	{
		Sleep(m_lAlignBinDelay);
	}

	if ((SearchGoodDie(FALSE) == TRUE) && (!IsBurnIn()))
	{
		str.Format("%d", ulBlkID);
		SetAlert_Msg_Red_Yellow(IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN, str);
		return IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN;
	}

	CMS896AStn::m_bRealignFrameDone = TRUE;

	return TRUE;
}


LONG CBinTable::FirstSearchDieInBinMapWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const UCHAR ucCorner)
{
	LONG lDieFound = 0;

	switch (ucCorner)
	{
		case REALIGN_UPPER_LEFT_CORNER:
			 lDieFound = SearchDieInBottomRight(ulBlkInUse, dFOVSize);
			 break;
		case REALIGN_UPPER_RIGHT_CORNER:
			 lDieFound = SearchDieInBottomLeft(ulBlkInUse, dFOVSize);
			 break;
		case REALIGN_LOWER_LEFT_CORNER:
			 lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize);
			 break;
		case REALIGN_LOWER_RIGHT_CORNER:
			 lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize);
			 break;
	}

	return lDieFound;
}


LONG CBinTable::FirstSearchDieNormalWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize)
{
	LONG lDieFound = 0;

	switch (m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
	{
		case BT_TRH_PATH:
		case BT_TRV_PATH:
			lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize);
			break;

		case BT_BLH_PATH:
		case BT_BLV_PATH:
			lDieFound = SearchDieInBottomLeft(ulBlkInUse, dFOVSize);
			break;

		case BT_BRH_PATH:
		case BT_BRV_PATH:
			lDieFound = SearchDieInBottomRight(ulBlkInUse, dFOVSize);
			break;

		case BT_TLV_PATH:
		default:
			lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize);
			break;
	}

	return lDieFound;
}


LONG CBinTable::FirstSearchDieWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol)
{
	LONG lDieFound = 0;
	CString szLog;

	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))	//v4.42T7
	{
		szLog.Format("Finding die, BinMap WalkPath = %d", m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse));
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		ULONG ulWalkPath = (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"];
		UCHAR ucCorner = GetRealignWithFirstCorner(ulWalkPath, ulDieRow, ulDieCol);
		lDieFound = FirstSearchDieInBinMapWalkPath(ulBlkInUse, dFOVSize, ucCorner);
	}
	else
	{
		szLog.Format("Finding die, WalkPath = %d", m_oBinBlkMain.GrabWalkPath(ulBlkInUse));
		/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

		lDieFound = FirstSearchDieNormalWalkPath(ulBlkInUse, dFOVSize);
	}	//End ELSE

	return lDieFound;
}


LONG CBinTable::SecondSearchDieInBinMapWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const UCHAR ucCorner)
{
	LONG lDieFound = 0;

	switch (ucCorner)
	{
		case REALIGN_UPPER_LEFT_CORNER:
			 lDieFound = SearchDieInBottomRight(ulBlkInUse, dFOVSize, FALSE, FALSE);
			 break;
		case REALIGN_UPPER_RIGHT_CORNER:
			 lDieFound = SearchDieInBottomLeft(ulBlkInUse, dFOVSize, FALSE, FALSE);
			 break;
		case REALIGN_LOWER_LEFT_CORNER:
			 lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize, FALSE, FALSE);
			 break;
		case REALIGN_LOWER_RIGHT_CORNER:
			 lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize, FALSE, FALSE);
			 break;
	}

	return lDieFound;
}


LONG CBinTable::SecondSearchDieNormalWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize)
{
	LONG lDieFound = 0;

	switch (m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
	{
		case BT_TRH_PATH:
		case BT_BLV_PATH:	//Nichia//v4.43T7
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Finding 2nd die: TRH or BLV", "a+");
			lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize);
			break;

		case BT_TLV_PATH:
		case BT_BRH_PATH:	//Nichia//v4.43T7
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Finding 2nd die: TLV or BRH", "a+");
			lDieFound = SearchDieInBottomLeft(ulBlkInUse, dFOVSize);
			break;

		case BT_TRV_PATH:
		case BT_BLH_PATH:	//Nichia//v4.43T7
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Finding 2nd die: TRV or BLH", "a+");
			lDieFound = SearchDieInBottomRight(ulBlkInUse, dFOVSize);
			break;

		case BT_TLH_PATH:
		case BT_BRV_PATH:	//Nichia//v4.43T7
		default:
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Finding 2nd die: TLH or BRV", "a+");
			lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize);
			break;
	}

	return lDieFound;
}


LONG CBinTable::SecondSearchDieWalkPath(const ULONG ulBlkInUse, const DOUBLE dFOVSize, const ULONG ulDieRow, const ULONG ulDieCol)
{
	LONG lDieFound = 0;

	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))	//v4.42T7
	{
		ULONG ulWalkPath = (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"];
		UCHAR ucCorner = GetRealignWithSecondCorner(ulWalkPath, ulDieRow, ulDieCol);
		lDieFound = SecondSearchDieInBinMapWalkPath(ulBlkInUse, dFOVSize, ucCorner);
	}
	else
	{
		lDieFound = SecondSearchDieNormalWalkPath(ulBlkInUse, dFOVSize);
	}

	return lDieFound;
}

VOID CBinTable::GetBinBlockXYDistance(const ULONG ulBlkInUse, LONG &lLogicalXBinBlockDistance, LONG &lLogicalYBinBlockDistance)
{
	lLogicalXBinBlockDistance = m_oBinBlkMain.GrabBlkUpperLeftX(ulBlkInUse) - m_oBinBlkMain.GrabBlkLowerRightX(ulBlkInUse);
	lLogicalYBinBlockDistance = m_oBinBlkMain.GrabBlkUpperLeftY(ulBlkInUse) - m_oBinBlkMain.GrabBlkLowerRightY(ulBlkInUse);
}


DOUBLE CBinTable::CalcDieABAngle(ULONG ulBlkInUse, 
							   const LONG DieALogical_x, const LONG DieALogical_y, const LONG DieAPhysical_x, const LONG DieAPhysical_y, 
							   const LONG DieBLogical_x, const LONG DieBLogical_y, const LONG DieBPhysical_x, const LONG DieBPhysical_y,
							   const LONG lDiePitchX, const LONG lDiePitchY,
							   LONG &lXDistance, LONG &lYDistance, LONG &lLogicalXDistance, LONG &lLogicalYDistance,
							   double &dCheckRealignmentAngle)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	DOUBLE  dNewAngle = 0;
	DOUBLE lLogicalAngle = 0.00;

	BOOL bEnableBHUplookPr = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR); //leo lam check relignment angle criteria
	CString szLog;

	lXDistance = lYDistance = 0;
	lLogicalXDistance = lLogicalYDistance = 0;

	LONG lLogicalXBinBlockDistance = 0, lLogicalYBinBlockDistance = 0;

	GetBinBlockXYDistance(ulBlkInUse, lLogicalXBinBlockDistance, lLogicalYBinBlockDistance);
	// after two points confirmed, Calculate New Angle
	// convert to standard distance to avoid problem because of diffrent resolution
	switch (m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
	{
		case BT_TLV_PATH:
		case BT_BLV_PATH:	//v4.42T1
			if (abs(DieBPhysical_y - DieAPhysical_y) < (lDiePitchY / 2))
			{
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("BLV: angle reset to 0", "a+");
				dNewAngle = 0; // less than 0.5 y pitch, only one die
			}
			else
			{
				lXDistance = ConvertXEncoderValueToFileUnit(DieAPhysical_x - DieBPhysical_x);
				lYDistance = ConvertYEncoderValueToFileUnit(DieBPhysical_y - DieAPhysical_y);
				dNewAngle = atan(((float)lXDistance) / ((float)lYDistance));

				//v4.50A16
				lLogicalXDistance = ConvertXEncoderValueToFileUnit(DieALogical_x - DieBLogical_x);
				lLogicalYDistance = ConvertYEncoderValueToFileUnit(DieBLogical_y - DieALogical_y);
				lLogicalAngle	  = atan(((float)lLogicalXDistance) / ((float)lLogicalYDistance));

				//fabs(dNewAngle * 180.0 /PI);
				if (bEnableBHUplookPr)
				{
					dCheckRealignmentAngle = fabs(atan(40 / ((float)lLogicalYBinBlockDistance)) * 0.61) * 180.0 /PI; //61% is left for margin
				}
				else
				{
					dCheckRealignmentAngle = fabs(atan(76.2 / ((float)lLogicalYBinBlockDistance)) * 0.61) * 180.0 /PI;
				}

			}

			//aeeyhng1030
			szLog.Format("TLV/BLV: Die ENC-A(%ld, %ld), ENC-B(%ld, %ld), lX=%ld (res=%.6f), lY=%ld (res=%.6f), Angle(radian)=%.6f, CheckAngle(deg)=%.6f, Uplook = %ld", 
							DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y,
					lXDistance, m_dXResolution, lYDistance, m_dYResolution, dNewAngle, dCheckRealignmentAngle, bEnableBHUplookPr);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			break;

		case BT_TRV_PATH:
		case BT_BRV_PATH:	//v4.42T1
			if (abs(DieBPhysical_y - DieAPhysical_y) < (lDiePitchY / 2))
			{
			//aeeyhng1030
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("TRV: angle reset to 0", "a+");
				dNewAngle = 0; // less than 0.5 y pitch, only one die
			}
			else
			{
				lXDistance = -ConvertXEncoderValueToFileUnit(DieAPhysical_x - DieBPhysical_x);
				lYDistance = -ConvertYEncoderValueToFileUnit(DieBPhysical_y - DieAPhysical_y);
				dNewAngle = atan(((float)lXDistance) / ((float)lYDistance));
				//v4.50A16
				lLogicalXDistance = ConvertXEncoderValueToFileUnit(DieALogical_x - DieBLogical_x);
				lLogicalYDistance = ConvertYEncoderValueToFileUnit(DieBLogical_y - DieALogical_y);
				lLogicalAngle	  = atan(((float)lLogicalXDistance) / ((float)lLogicalYDistance));

				if (bEnableBHUplookPr)
				{
					dCheckRealignmentAngle = fabs(atan(40 / ((float)lLogicalYBinBlockDistance)) * 0.61) * 180.0 /PI; //61% is left for margin
				}
				else
				{
					dCheckRealignmentAngle = fabs(atan(76.2 / ((float)lLogicalYBinBlockDistance)) * 0.61) * 180.0 /PI;
				}
			}
			//aeeyhng1030
			szLog.Format("TRV/BRV: Die ENC-A(%ld, %ld), ENC-B(%ld, %ld), lX=%ld (res=%.6f), lY=%ld (res=%.6f), Angle(radian)=%.6f, CheckAngle(deg)=%.6f, Uplook = %ld", 
							DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y,
					lXDistance, m_dXResolution, lYDistance, m_dYResolution, dNewAngle, dCheckRealignmentAngle, bEnableBHUplookPr);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			break;

		case BT_TRH_PATH:
		case BT_BRH_PATH:		//v4.42T1
			if (abs(DieBPhysical_x - DieAPhysical_x) < (lDiePitchX / 2))
			{
				dNewAngle = 0; // less than 0.5 x pitch, only one die
			}
			else
			{
				lXDistance = -ConvertXEncoderValueToFileUnit(DieBPhysical_x - DieAPhysical_x);
				lYDistance = -ConvertYEncoderValueToFileUnit(DieBPhysical_y - DieAPhysical_y);
				dNewAngle = atan(((float)lYDistance) / ((float)lXDistance));
				//v4.50A16
				lLogicalXDistance = ConvertXEncoderValueToFileUnit(DieBLogical_x - DieALogical_x);
				lLogicalYDistance = ConvertYEncoderValueToFileUnit(DieBLogical_y - DieALogical_y);
				lLogicalAngle	  = atan(((float)lLogicalYDistance) / ((float)lLogicalXDistance));

				if (bEnableBHUplookPr)
				{
					dCheckRealignmentAngle = fabs(atan(40 / ((float)lLogicalXBinBlockDistance)) * 0.61) * 180.0 /PI; //61% is left for margin
				}
				else
				{
					dCheckRealignmentAngle = fabs(atan(76.2 / ((float)lLogicalXBinBlockDistance)) * 0.61) * 180.0 /PI;
				}
			}
			szLog.Format("TRH/BRH: Die ENC-A(%ld, %ld), ENC-B(%ld, %ld), lX=%ld (res=%.6f), lY=%ld (res=%.6f), Angle(radian)=%.6f, CheckAngle(deg)=%.6f, Uplook = %ld", 
					DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y,
					lXDistance, m_dXResolution, lYDistance, m_dYResolution, dNewAngle, dCheckRealignmentAngle, bEnableBHUplookPr);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			break;

		case BT_BLH_PATH:		//v4.42T1
		default:
			if (abs(DieBPhysical_x - DieAPhysical_x) < (lDiePitchX / 2))
			{
				dNewAngle = 0; // less than 0.5 x pitch, only one die
				
				szLog = "Andrew: Angle = 0 degree!";
				/******/ 
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
			}
			else
			{
				lXDistance	= ConvertXEncoderValueToFileUnit(DieBPhysical_x - DieAPhysical_x);
				lYDistance	= ConvertYEncoderValueToFileUnit(DieBPhysical_y - DieAPhysical_y);
				dNewAngle	= atan(((float)lYDistance) / ((float)lXDistance));

				lLogicalXDistance = ConvertXEncoderValueToFileUnit(DieBLogical_x - DieALogical_x);
				lLogicalYDistance = ConvertYEncoderValueToFileUnit(DieBLogical_y - DieALogical_y);
				lLogicalAngle = atan(((float)lLogicalYDistance) / ((float)lLogicalXDistance));

				if (bEnableBHUplookPr)
				{
					dCheckRealignmentAngle = fabs(atan(40 / ((float)lLogicalXBinBlockDistance)) * 0.61) * 180.0 /PI; //61% is left for margin
				}
				else
				{
					dCheckRealignmentAngle = fabs(atan(76.2 / ((float)lLogicalXBinBlockDistance)) * 0.61) * 180.0 /PI;
				}
			
				//szLog.Format("TLH  A = (%ld, %ld); B = (%ld, %ld); Angle = %.2f (%ld, %ld); Angle (logical) = %.2f", 
				//			 DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y, 
				//			 dNewAngle, lXDistance, lYDistance, lLogicalAngle);
				//szLog.Format("TLH  A = (%ld, %ld); B = (%ld, %ld); Angle = %.2f (%.2f deg); Angle (logical) = %.2f", 
				//			 DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y, 
				//			 dNewAngle, (dNewAngle * 180.0 / PI), lLogicalAngle);		//v4.44A3
				szLog.Format("TLH/BLH: Die ENC-A(%ld, %ld), ENC-B(%ld, %ld), lX=%ld (res=%.6f), lY=%ld (res=%.6f), Angle(radian)=%.6f, CheckAngle(deg)=%.6f, Uplook = %ld", 
							 DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y, 
							lXDistance, m_dXResolution, lYDistance, m_dYResolution, dNewAngle, dCheckRealignmentAngle, bEnableBHUplookPr);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

				
				//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

				//**Note: only TLH mode can support LSBondPattern	//v4.35T4
				if (m_oBinBlkMain.GrabLSBondPattern())
				{
					dNewAngle = dNewAngle - lLogicalAngle;			//v4.35T4	//MS109
				}
			}
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("BLH and DEFAULT", "a+");
			break;
	}

	return dNewAngle;
}


VOID CBinTable::FindNextDieOfFirstDie(const ULONG ulBlkInUse, const DOUBLE dFOVSize,
									  const BOOL bNewAlignMethod, const BOOL bUsePR, 
									  const ULONG ulNoOfSortedDie, const ULONG ulFirstDieIndex, const ULONG ulDiePerRow,
									  BOOL &bUse2ndOffsetXY, BOOL &bUse3rdOffsetXY, 
									  LONG &lNewXOffset, LONG &lNewYOffset,
									  LONG &DieALogical_x, LONG &DieALogical_y, LONG &DieAPhysical_x, LONG &DieAPhysical_y)
{
	CString szLog;
	DOUBLE dUmX = 0, dUmY = 0;
	//******************************************************//
	// find 1st+1 die (using PR LookForward) of first row in the grade
	LONG lNewXOffset2 = lNewXOffset;
	LONG lNewYOffset2 = lNewYOffset;
	bUse2ndOffsetXY = FALSE;
	bUse3rdOffsetXY = FALSE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//Added with Nichia //leo lam
	if ( (pApp->GetCustomerName()==CTM_FINISAR || pApp->GetCustomerName()==CTM_NICHIA) && bUsePR && (ulNoOfSortedDie > 3) )
	{
		//Find 1st + 3
		DOUBLE d3rdX=0, d3rdY = 0;
		LONG lDie3rdLogical_x=0,	lDie3rdLogical_y=0;
		LONG lDie3rdNewLogical_x=0, lDie3rdNewLogical_y=0;
		LONG lDie3rdPhysical_x=0,	lDie3rdPhysical_y=0;
		LONG l3rdDieOffsetX=0,		l3rdDieOffsetY=0;

		m_oBinBlkMain.StepDMove(ulBlkInUse, 3, d3rdX, d3rdY);
		//For Offset calculation
		lDie3rdLogical_x = ConvertFileUnitToXEncoderValue(d3rdX);
		lDie3rdLogical_y = ConvertFileUnitToYEncoderValue(d3rdY);
		//For indexing to 3rd-Die using 1st-die offset
		lDie3rdNewLogical_x = ConvertFileUnitToXEncoderValue(d3rdX + lNewXOffset2);
		lDie3rdNewLogical_y = ConvertFileUnitToYEncoderValue(d3rdY + lNewYOffset2);
		
		szLog.Format("NewAlgor (Finisar) Search 1st+2 die (blk #%d) in BINMAP: 3rdIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulFirstDieIndex+2, ulNoOfSortedDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		
		BT_MoveTo(lDie3rdNewLogical_x, lDie3rdNewLogical_y);
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(m_lAlignBinDelay);	

		if (SearchGoodDie() == TRUE)
		{
			GetXYEncoderValue(lDie3rdPhysical_x, lDie3rdPhysical_y);


			//lNewXOffset = -ConvertXEncoderValueToFileUnit(DieAPhysical_x - DieALogical_x);
			//lNewYOffset = -ConvertYEncoderValueToFileUnit(DieAPhysical_y - DieALogical_y);
			l3rdDieOffsetX = -ConvertXEncoderValueToFileUnit(lDie3rdPhysical_x - lDie3rdLogical_x);
			l3rdDieOffsetY = -ConvertYEncoderValueToFileUnit(lDie3rdPhysical_y - lDie3rdLogical_y);

			szLog.Format("Use 1st+2 die offset XY: orig 1stDieOffset (%ld, %ld); new 3rdDieOffset (%ld, %ld)", 
							lNewXOffset, lNewYOffset, l3rdDieOffsetX, l3rdDieOffsetY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			lNewXOffset = l3rdDieOffsetX;
			lNewYOffset = l3rdDieOffsetY;

			DieALogical_x	= lDie3rdLogical_x;
			DieALogical_y	= lDie3rdLogical_y;
			DieAPhysical_x	= lDie3rdPhysical_x;
			DieAPhysical_y	= lDie3rdPhysical_y;

			bUse3rdOffsetXY = TRUE;		//v4.59A36
			CString szMsg;
			//szMsg.Format("Search 3rd Die result OK, new Offset (%ld, %ld)",  l3rdDieOffsetX, l3rdDieOffsetY);
			//HmiMessage(szMsg);
		}

	}
	else if (bNewAlignMethod && bUsePR && (ulNoOfSortedDie > 2))		//Semitek Mode with NEW method
	{
		LONG l2ndDieOffsetX = 0, l2ndDieOffsetY = 0;

		if (LookAhead2ndDieOffset(ulBlkInUse, dFOVSize, bUse2ndOffsetXY, l2ndDieOffsetX, l2ndDieOffsetY))
		{
			szLog.Format("NewAlgor: 1st+1 Offset - Result=%d; 1stDie Offset (%ld, %ld) in um; 2ndDie dOffset (%ld, %ld) in um", 
						bUse2ndOffsetXY, lNewXOffset, lNewYOffset, l2ndDieOffsetX, l2ndDieOffsetY);	
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
			
			CString szNewLog;
			//If 2nd is found under LookForward, and fcn advises to use 2nd die offset instead ...
			if (bUse2ndOffsetXY)
			{
				lNewXOffset = lNewXOffset + l2ndDieOffsetX;
				lNewYOffset = lNewYOffset + l2ndDieOffsetY;

				szLog.Format("Use 1st+1 die offset XY, NewOffset (%ld, %ld); 2nd(%ld, %ld)", 
								lNewXOffset, lNewYOffset, l2ndDieOffsetX, l2ndDieOffsetY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Use 1st+1 die offset XY ...", "a+");

				//v4.43T13
				//Use 1st+1 die pos for offset and angle calculation
				m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex + 1, dUmX, dUmY);	//1st+1 Die logicial pos
				DieALogical_x	= ConvertFileUnitToXEncoderValue(dUmX);
				DieALogical_y	= ConvertFileUnitToXEncoderValue(dUmY);
				DieAPhysical_x	= ConvertFileUnitToXEncoderValue(dUmX + lNewXOffset);
				DieAPhysical_y	= ConvertFileUnitToXEncoderValue(dUmY + lNewYOffset);
				szNewLog.Format("AutoCalAngle on Bin #%ld (Realign 2ndDie): Enc_A(%ld, %ld)\n", 
					ulBlkInUse, DieAPhysical_x, DieAPhysical_y);
			}
			else
			{
				//Use 1st die offset for calculation instead
				szNewLog.Format("AutoCalAngle on Bin #%ld (Realign 1stDie): Enc_A(%ld, %ld)\n", 
					ulBlkInUse, DieAPhysical_x, DieAPhysical_y);
			}

			//Use 1st die offset for calculation instead
			//v4.44T3
			if (m_oBinBlkMain.GrabWalkPath(ulBlkInUse) == BT_TLH_PATH)
			{
				if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) )
				{
					ULONG ulLastDieRow = 0, ulLastDieCol = 0;
					ULONG ulNoOf1stRowDices = ulDiePerRow;		//v4.48A2
					m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices, ulLastDieRow, ulLastDieCol);
					if (ulNoOfSortedDie < ulNoOf1stRowDices)
					{
						//(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= TRUE;
						(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"]		= DieAPhysical_x;
						(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"]		= DieAPhysical_y;
						CMSLogFileUtility::Instance()->BT_TableIndexLog(szNewLog);
					}
				}
				else if (ulNoOfSortedDie <ulDiePerRow)
				{
					//(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= TRUE;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"]		= DieAPhysical_x;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"]		= DieAPhysical_y;
					CMSLogFileUtility::Instance()->BT_TableIndexLog(szNewLog);
				}
			}

			szLog.Format("NewAlgor: REALIGN 1st+1 Die OFFSET-XY (%ld, %ld)", lNewXOffset, lNewYOffset);	
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

		}
	}
	else if (m_bSemitekBLMode && bUsePR)	//Semitek Mode with OLD method
	{
		CString szNewLog;
		//v4.44A3
		//Save PhysicalA XY pos on first-Die-A for AutoCalAngle frame alignment
		if (m_oBinBlkMain.GrabWalkPath(ulBlkInUse) == BT_TLH_PATH)
		{
			if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) )
			{
				ULONG ulLastDieRow = 0, ulLastDieCol = 0;
				ULONG ulNoOf1stRowDices = ulDiePerRow;
				m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices, ulLastDieRow, ulLastDieCol);
				if (ulNoOfSortedDie < ulNoOf1stRowDices)
				{
					//(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= TRUE;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"]		= DieAPhysical_x;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"]		= DieAPhysical_y;
					szNewLog.Format("AutoCalAngle on Bin #%ld (Realign 1stDie): Enc_A(%ld, %ld)\n", 
						ulBlkInUse, DieAPhysical_x, DieAPhysical_y);
					CMSLogFileUtility::Instance()->BT_TableIndexLog(szNewLog);
				}
			}
			else if (ulNoOfSortedDie < ulDiePerRow)
			{
				//(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= TRUE;
				(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"]		= DieAPhysical_x;
				(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"]		= DieAPhysical_y;
				szNewLog.Format("AutoCalAngle on Bin #%ld (Realign 1stDie): Enc_A(%ld, %ld)\n", 
					ulBlkInUse, DieAPhysical_x, DieAPhysical_y);
				CMSLogFileUtility::Instance()->BT_TableIndexLog(szNewLog);
			}
		}
	}
}


ULONG CBinTable::FindFirstDieOfGrade(const ULONG ulBlkInUse, const ULONG ulNoOfSortedDie, ULONG &ulRow, ULONG &ulCol)
{
	ULONG ulFirstDieIndex = 0;
	CString szLog;

	ulRow = ulCol = 0;
	//******************************************************//
	// find first die of the grade
	// move bin table to first die of this block
	// get the the logically position of first die
	ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);

	//v4.35T3
	if (m_oBinBlkMain.GrabLSBondPattern())
	{
		//** e.g. LS Bond sequence with TLH path, DiePerRow = 10 **//
		//
		//     1           
		// 11 10  9  8  7  6  5  4  3  2
		//    12 13 14 15 16 17 18 19 20
		//    29 28 27 26 25 24 23 22 21
		//    31 32 33 .......
		//

		ulFirstDieIndex = 2;

		szLog.Format("Search 1st die (blk #%d) with LS pattern: 1stIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulFirstDieIndex, ulNoOfSortedDie);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) )	//v4.36
	{
		//ulFirstDieIndex = m_oBinBlkMain.Get1stDieIndexIn1stBMapRow();	//v4.36
		//v4.42T7	//Update BinMap Walk path only; does not affect 1st Die index
		m_oBinBlkMain.Get1stDieIndexIn1stBMapRow(ulBlkInUse, ulRow, ulCol);	

		szLog.Format("Search 1st die (blk #%d) in BINMAP: 1stIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulFirstDieIndex, ulNoOfSortedDie);

		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))		//Nichia//v4.43T7
	{
		ulFirstDieIndex = m_oBinBlkMain.Get1stDieIndexIn1stBMapRow(ulBlkInUse, ulRow, ulCol);	

		szLog.Format("Search 1st die (blk #%d) in BINMAP2: 1stIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulFirstDieIndex, ulNoOfSortedDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}

	return ulFirstDieIndex;
}


ULONG CBinTable::FindLastDieOfFirstRowInGrade(const ULONG ulBlkInUse, const ULONG ulNoOfSortedDie, const ULONG ulFirstDieIndex,
											 const ULONG ulDiePerRow, const ULONG ulDiePerCol, ULONG &ulLastDieRow, ULONG &ulLastDieCol)
{
	ULONG ulLastDieIndex = 0;
	CString szLog;

	//******************************************************//
	// find last die of first row in the grade
	// off bin table joy stick
	//SetJoystickOn(FALSE);		//v3.71T9

	// move bin table to last die/first row of this block, use the offset get from 1st die
	szLog.Format("No of sorted die: %d", ulNoOfSortedDie);
	CMSLogFileUtility::Instance()->BT_LogStatus(szLog);

	ulLastDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);

	szLog.Format("LastDieIndex After Get Hole:%d", ulLastDieIndex);
	CMSLogFileUtility::Instance()->BT_LogStatus(szLog);

	ulLastDieIndex = ulLastDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
	ulLastDieIndex = ulLastDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

	szLog.Format("LastDieIndex After Get First Row Col Skip Pattern:%d", ulLastDieIndex);
	CMSLogFileUtility::Instance()->BT_LogStatus(szLog);

	switch (m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
	{
	case BT_TLV_PATH:
	case BT_TRV_PATH:
	case BT_BLV_PATH:		//v4.42T1
	case BT_BRV_PATH:		//v4.42T1
		if (ulLastDieIndex > ulDiePerCol)
		{
			ulLastDieIndex = ulDiePerCol;
		}
		break;

	case BT_TLH_PATH:
	case BT_TRH_PATH:
	case BT_BLH_PATH:		//v4.42T1
	case BT_BRH_PATH:		//v4.42T1
	default:
		if (ulLastDieIndex > ulDiePerRow)
		{
			ulLastDieIndex = ulDiePerRow;
		}
		break;
	}

	//v4.35T3
	if (m_oBinBlkMain.GrabLSBondPattern())
	{
		//** e.g. LS Bond sequence with TLH path, DiePerRow = 10 **//
		//
		//     1           
		// 11 10  9  8  7  6  5  4  3  2
		//    12 13 14 15 16 17 18 19 20
		//    29 28 27 26 25 24 23 22 21
		//    31 32 33 .......
		//
		
		//ULONG ul2ndDieIndex		= ulLastDieIndex;
		ulLastDieIndex = ulDiePerRow + 1;	//Use Die #2 on 2nd row

		szLog.Format("Search 2nd die (blk #%d) with LS pattern: 2ndIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulLastDieIndex, ulNoOfSortedDie);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))		//v4.36
	{
		ULONG ulNoOf1stRowDices = ulDiePerRow;

		m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices, ulLastDieRow, ulLastDieCol);

		if (ulNoOfSortedDie >= ulNoOf1stRowDices)
		{
			ulLastDieIndex = ulFirstDieIndex + ulNoOf1stRowDices - 1;
		}
		else
		{
			ulLastDieIndex = ulFirstDieIndex + ulNoOfSortedDie - 1;
		}

		szLog.Format("Search 2nd die (blk #%d) in BINMAP: 2ndIndex=%lu, NoOfDieOn1stRow=%lu", 
					 ulBlkInUse, ulLastDieIndex, ulNoOf1stRowDices);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "CyOptics"	|| 
		pApp->GetCustomerName() == "Inari"		||		//v4.51A24
		pApp->GetCustomerName() == "FiberOptics")		//v4.40T2
	{
		ULONG ul1stRowDieIndex = m_oBinBlkMain.GetEmptyRow1stRowIndex(ulBlkInUse);

		szLog.Format("CyOptics - 1stRowIndex = %lu (Orig = %lu; DiePerRow = %lu)", 
					ul1stRowDieIndex, ulLastDieIndex, ulDiePerRow);
		/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 

		if ((ul1stRowDieIndex != 0) && (ul1stRowDieIndex < ulDiePerRow))
		{
			szLog.Format("CyOptics uses 1stRowIndex = %lu", ul1stRowDieIndex);
			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
			ulLastDieIndex = ul1stRowDieIndex;
		}
	}
	else if (!m_bUseDualTablesOption)	//Not available for MS109; only for MS100/MS100+
	{
		//v4.40T9	//LeoLam
		ULONG ulMinDicesToMeasureAngle = (ULONG) (0.33 * ulDiePerRow);
	}

	return ulLastDieIndex;
}


VOID CBinTable::GetDieLogicalEncoderValue(const ULONG ulBlkInUse, const ULONG ulDieIndex, 
										  LONG &lDieLogical_x, LONG &lDieLogical_y, BOOL bLog)
{
	DOUBLE dUmX = 0, dUmY = 0;

	m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dUmX, dUmY, bLog);
	lDieLogical_x = ConvertFileUnitToXEncoderValue(dUmX);
	lDieLogical_y = ConvertFileUnitToYEncoderValue(dUmY);
}

 // suppose one grade only put on one block and one block contains only one grade die
// just like auto bin loader
// not handle the empty pattern( top left no die bonded)
LONG CBinTable::RealignBinBlock(ULONG ulBlkInUse, BOOL bAuto, BOOL bAutoTeach, 
								BOOL bDisableAlarm, BOOL bDummyRun, USHORT sRetry,
								BOOL bSecondRealign)	//v4.59A31
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	BOOL bOldAligned, bUsePR, bManualLocate;
	BOOL bRealignFailed = FALSE;
	BOOL bMoveTable = TRUE;
	CString str;
	CString szTitle, szContent;	
	DOUBLE dUmX=0, dUmY=0;		//v4.59A19
	//LONG lX=0, lY=0;
	LONG lPhyDist = 0, lLogDist = 0, lDiePitchX, lDiePitchY, lXDistance = 0, lYDistance = 0;		//Klocwoek
	ULONG ulNoOfSortedDie, ulDiePerRow, ulDiePerCol, ulDieIndex, ulFirstDieIndex, ulLastDieIndex;
	LONG lDieFound, lDieFound1, lDieFound2;
	LONG DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y;
	DOUBLE  dNewAngle = 0, dFOVSize = 0;
	DOUBLE	dCheckRealignmentAngle = 0.0; //leo lam check relignment angle criteria
	BOOL bEnableBHUplookPr = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR); //leo lam check relignment angle criteria
	LONG lNewXOffset, lNewYOffset;
	CString szLog;
	CString szMzg;
	
	m_oBinBlkMain.SetEnableBinMap(ulBlkInUse);

	CMS896AStn::m_bRealignFrameDone = FALSE;
	m_nMultiSeachCounter = 0; //4.51D20 init
	m_lDoubleCheckFirstDie = 0;
	m_nMultiUnfondCounter = 1;
	//(*m_psmfSRam)["BinTable"]["MultiSearchUpdate"] = -1;
	(*m_psmfSRam)["BinTable"]["StepX"]["UpdateX"]= 0;
	(*m_psmfSRam)["BinTable"]["StepY"]["UpdateY"]= 0;
	// 20170824 leo update the current walking path as MS90 will change when bonding south
	(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"]	= m_oBinBlkMain.GrabWalkPath(ulBlkInUse);

	BOOL bNewAlignMethod = CMS896AApp::m_bBinFrameNewRealignMethod;		//v4.26T1	//Semitek
	if (m_bSemitekBLMode)	//pApp->GetCustomerName() == "Semitek")		//v4.22T9
	{
		bNewAlignMethod = TRUE;
	}

	ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 
	ulDiePerCol		= m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkInUse); 
	lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));
	bOldAligned		= m_oBinBlkMain.GrabNVIsAligned(ulBlkInUse);

	szTitle.LoadString(HMB_BT_REALIGN_BIN_BLOCK);			

	if (ulNoOfSortedDie == 0)
	{
		SetJoystickOn(FALSE);	//v3.71T5

		LONG lPosX = 0, lPosY = 0;
		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
		GetDieLogicalEncoderValue(ulBlkInUse, ulFirstDieIndex, lPosX, lPosY);

		if (CMS896AStn::m_bEnableEmptyBinFrameCheck == FALSE)
		{
			bMoveTable = BT_MoveTo(lPosX, lPosY, FALSE);		//v4.46T13	//Re-enable for Genesis MS899DLA

			if (bAuto == FALSE && !IsBurnIn())
			{
				str.Format("%d", ulBlkInUse);
				SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_ON_BLOCK, str);
			}
		}
		else
		{
			bMoveTable = BT_MoveTo(lPosX, lPosY, TRUE);

			//DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	//v2.58

			if (m_lAlignBinDelay > 0)
			{
				Sleep(m_lAlignBinDelay);
			}
			//DisplayBondPrSearchWindow(FALSE, FALSE, BT_PR_CENTER);

			// make sure no die in bin block if no of sorted die = 0
			if ((lDieFound = SearchGoodDie()) == TRUE && (!IsBurnIn()))
			{
				str.Format("%d", ulBlkInUse);
				SetAlert_Msg_Red_Yellow(IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN, str);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()BT Die is found on Empty","a+");
				return IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN;
			}	

		}

		//v4.57A5
		if (!bMoveTable)
		{
			CString szError;
			szError = "BT MOVE is out of table limit";
			SetErrorMessage("BT_MoveTo Error 1");
			HmiMessage_Red_Back(szError, "Bin Table Error");

			m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);
			CMS896AStn::m_bRealignFrameDone = TRUE;
			SwitchToBPR();
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()BT is out of Table Limit1","a+");
			return FALSE;
		}

		szLog.Format("Re-align start with 0 die on bin #%lu - " + pApp->GetSoftVersion(), ulBlkInUse);	//v4.34T2
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	
		m_dFrameAlignAngleForTesting = m_oBinBlkMain.GrabNVRotateAngleX(1);	//v4.57A9

		m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);		//v2.93T2
		CMS896AStn::m_bRealignFrameDone = TRUE;
		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= TRUE;
		return TRUE;
	}


	//check FOV size for this binblk
	dFOVSize = GetBPRFOVSize(ulBlkInUse);
	BOOL bLargeDie = IsLargeDieSize();		//v2.67
	BOOL bFrameRotation = m_oBinBlkMain.GetFrameRotation(ulBlkInUse);		//v4.59A32
	
	szLog.Format("Re-align start [%3.2f] count=%d, DiePerRow=%d, DiePerCol=%d, NewAlgor=%d, DummyRun=%d, 180deg=%d", 
				 dFOVSize, ulNoOfSortedDie, ulDiePerRow, ulDiePerCol, bNewAlignMethod, bDummyRun, bFrameRotation);	
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	
	//v4.49A1
	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))
	{
		szLog.Format("BMAP settings: Bin #%d, Grade %d, InputCnt=%lu, BMapCnt=%lu",
				ulBlkInUse, m_oBinBlkMain.GrabGrade(ulBlkInUse),
				m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(ulBlkInUse)),
				GetBinMapCount(ulBlkInUse));
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}

	m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);

	//Check Postbond pr status
	bUsePR = CheckBPRStatus();

	//v4.53A21	//Osram Germany
	if (bUsePR)
	{
		BOOL bIsBT1Calibrated = (BOOL)(LONG)((*m_psmfSRam)["BondPr"]["Calibration"]["BT1"]);
		if (!bIsBT1Calibrated)
		{
			szLog = "BT1 Realignment fails due to PR calibration";
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
			SetAlert(IDS_BPR_CALIBRATEFACTORERROR);
			m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);
			CMS896AStn::m_bRealignFrameDone = TRUE;
			SwitchToBPR();
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()BT1 Calibrate not done","a+");
			return IDS_BPR_CALIBRATEFACTORERROR;
		}

	}

	// change camera to bond side
	//SwitchToPbBPR();						//v2.93T2

	// Display Search window	//v2.58
	BOOL bLF = FALSE;
	if (dFOVSize >= BPR_LF_SIZE)
	{
		bLF = TRUE;
		bLargeDie = FALSE;		//v2.67	//prefer to use LA even if Large-Die
	}

	// off bin table joy stick
	SetJoystickOn(FALSE);

	ULONG ulFirstDieRow = 0, ulFirstDieCol = 0;
	// find first die of the grade
	ulFirstDieIndex = FindFirstDieOfGrade(ulBlkInUse, ulNoOfSortedDie, ulFirstDieRow, ulFirstDieCol);
	GetDieLogicalEncoderValue(ulBlkInUse, ulFirstDieIndex, DieALogical_x, DieALogical_y);

	bMoveTable = BT_MoveTo(DieALogical_x, DieALogical_y);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
	Sleep(m_lAlignBinDelay + 100);		//v4.40T5				

	if (!bMoveTable)
	{
		CString szError;
		szError = "BT MOVE is out of table limit";
		SetErrorMessage("BT_MoveTo Error 2");
		HmiMessage_Red_Back(szError, "Bin Table Error");

		m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);
		CMS896AStn::m_bRealignFrameDone = TRUE;
		SwitchToBPR();
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()BT is out of Table Limit2","a+");
		return FALSE;
	}


	if (bUsePR == TRUE)
	{
		if (bAutoTeach == TRUE)
		{
			szLog.Format("Search 1st die start at blk #%d", ulBlkInUse);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 

			if (bLargeDie || dFOVSize<BPR_LF_SIZE)	//Large die; use single mode //20170829 Leo Prevent multisearch fail
			{
				if (bDummyRun)		//v4.31T10
				{
					lDieFound = TRUE;
				}
				else
				{
					lDieFound = SearchGoodDie();
				}
			}
			else			//Small die; use LA or move-search mode
			{
				/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search first die", "a+"); /******/ 

				szMzg.Format("Search --- bDummyRun: %d, bMultiSearch:%d", bDummyRun, CMS896AApp::m_bBinMultiSearchFirstDie );
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMzg, "a+");
				
				if (bDummyRun)							//v4.31T10
				{
					lDieFound = TRUE;
				}
				else if (m_oBinBlkMain.GrabLSBondPattern() || m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))
				{
					if(CMS896AApp::m_bBinMultiSearchFirstDie == FALSE) 
					{
					   lDieFound = SearchGoodDie();    //v4.35T4
					}
					else
					{
						LONG lOldZoomSensorMode = BPR_ZoomSensor(CPrZoomSensorMode::PR_ZOOM_MODE_FF); //FF Mode
						lDieFound = MultiSearchFirstGoodDie(ulBlkInUse, ulFirstDieRow, ulFirstDieCol); //4.51D20search
						BPR_ZoomSensor(lOldZoomSensorMode); //4X Mode
						if( lDieFound == TRUE) // final search again after multi search
						{
							lDieFound = SearchFirstGoodDie(ulBlkInUse); 
							szMzg.Format("Search --- After Multi DieFound1: %d  ", lDieFound );
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMzg, "a+");
						}

					}

					szMzg.Format("Search --- DieFound: %d ", lDieFound );
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMzg, "a+");

					//v4.42T7
					if (lDieFound == FALSE)
					{
						if (m_bSemitekBLMode && pApp->GetCustomerName() != "Electech3E(DL)")	//v4.50A15
							lDieFound = SearchFirstGoodDie_Semitek(ulBlkInUse, FALSE, TRUE);
						else if (pApp->GetCustomerName() == "Electech3E(DL)")
							lDieFound = SearchFirstGoodDie_Semitek(ulBlkInUse, FALSE, FALSE);
						else
							lDieFound = SearchFirstGoodDie(ulBlkInUse);
					}
				}		
				else
				{

					szMzg = "Search --- Last Condition";
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMzg, "a+");

					if(  CMS896AApp::m_bBinMultiSearchFirstDie == FALSE)
					{
						if (m_bSemitekBLMode && pApp->GetCustomerName() != "Electech3E(DL)")	//v4.50A15
							lDieFound = SearchFirstGoodDie_Semitek(ulBlkInUse, FALSE, TRUE);	
						else if (pApp->GetCustomerName() == "Electech3E(DL)")
							lDieFound = SearchFirstGoodDie_Semitek(ulBlkInUse, FALSE, FALSE);
						else
							lDieFound = SearchFirstGoodDie(ulBlkInUse);
					}
					else
					{
						LONG lOldZoomSensorMode = BPR_ZoomSensor(CPrZoomSensorMode::PR_ZOOM_MODE_FF); //FF Mode
						lDieFound = MultiSearchFirstGoodDie(ulBlkInUse, ulFirstDieRow, ulFirstDieCol); //4.51D20search
						BPR_ZoomSensor(lOldZoomSensorMode); //4X Mode
						if( lDieFound == TRUE)
						{
							lDieFound = SearchFirstGoodDie(ulBlkInUse);  
							szMzg.Format("Search --- After Multi DieFound2: %d  ", lDieFound );
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMzg, "a+");
						}
					}

				}

				if (lDieFound == TRUE)
				{
					if (bDummyRun)						//v4.31T10
					{
						lDieFound = TRUE;
					}
					else if (m_oBinBlkMain.GrabLSBondPattern() ||
						    (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse)))
					{
						//2018.5.8
						if (bSecondRealign)
						{
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Find 1st die through walk path in second align (BINMAP)", "a+");
							lDieFound = FirstSearchDieWalkPath(ulBlkInUse, dFOVSize, ulFirstDieRow, ulFirstDieCol);//2019.05.14
						}
						else
						{
							lDieFound = TRUE;
						}
					}
					else
					{
						lDieFound = FirstSearchDieWalkPath(ulBlkInUse, dFOVSize, ulFirstDieRow, ulFirstDieCol);
					}
				}
			}
		}
		else
		{
			lDieFound = FALSE;
		}

		if (lDieFound == IDS_BPR_DIENOTLEARNT)
		{
			SwitchToBPR();
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()Die is not Learnt","a+");
			return IDS_BPR_DIENOTLEARNT;
		}

		if (lDieFound == FALSE)
		{
			if (!IsBurnIn() && m_bAlignBinInAlive == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UL_DIE);
				SwitchToBPR();
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No UL Die","a+");
				return IDS_BT_REALIGN_NO_UL_DIE;
			}
			
			bManualLocate = 1;
		}
		else
		{
			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("1st die is found", "a+"); /******/
			bManualLocate = 0;
		}
	}
	else
	{
		bManualLocate = 1;
	}

	if (bManualLocate == 1)
	{
		if (!IsBurnIn())
		{
			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Manual locate 1st die", "a+"); /******/

			//Ask user to locate die pos, first die in first row
			SetJoystickOn(TRUE);

			szContent.LoadString(HMB_BT_REALIGN_STEP1);			
			if (HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			{
				SwitchToBPR();
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No UL Die to Stop","a+");
				return HMB_BT_REALIGN_STEP1;
			}

			// if operation confirm the location
			SetJoystickOn(FALSE);
		}

		// to align the die by pr
		if (bUsePR == TRUE)
		{
			SearchGoodDie();
		}
	}

		
	/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-confirm TL die", "a+"); /******/
/*
	//Get 1st die physical position
	if (bManualLocate == 0)
	{
		Sleep(m_lAlignBinDelay);		
		SearchGoodDie();
		Sleep(100);
	}
*/

	GetXYEncoderValue(DieAPhysical_x, DieAPhysical_y);

	// convert die offset in um unit
	lNewXOffset = -ConvertXEncoderValueToFileUnit(DieAPhysical_x - DieALogical_x);
	lNewYOffset = -ConvertYEncoderValueToFileUnit(DieAPhysical_y - DieALogical_y);

	//v3.71T1
	szLog.Format("REALIGN 1stDie ENC-A POS XY,%d,%d,OFFSET-XY,%d,%d", DieAPhysical_x, DieAPhysical_y, lNewXOffset, lNewYOffset);
	CMSLogFileUtility::Instance()->BT_LogStatus(szLog);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 


	// only one die bonded, no need to find last
	if (ulNoOfSortedDie == 1)
	{
		//v4.36
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse) + 1;
		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dUmX, dUmY);	//v4.59A19
		ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, 0, lNewXOffset, lNewYOffset);
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
		
		//4.52D1 //only one die bonded, but need to find last if Inari
		if (bAuto && (pApp->GetCustomerName() == "CyOptics" || pApp->GetCustomerName() == "Inari"))
		{
			ULONG ulNextDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse) + 1;
			ULONG ulOrigIndex = ulNextDieIndex;
			if (m_oBinBlkMain.CheckIfEmptyRow(ulBlkInUse))
			{
				m_oBinBlkMain.CheckIfNeedToLeaveEmptyRow(ulBlkInUse, ulNextDieIndex);
				m_oBinBlkMain.StepDMove(ulBlkInUse, ulNextDieIndex, dUmX, dUmY);	//v4.59A19
				ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
				BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
				Sleep(100);

				//Used in OpMoveTable() in CycleState
				m_lX = ConvertFileUnitToXEncoderValue(dUmX);
				m_lY = ConvertFileUnitToXEncoderValue(dUmY);

				//v4.44A5
				CString szMyLog;
				szMyLog.Format("RealignFrame 2: m_lX = %ld, m_lY = %ld", m_lX, m_lY);
				CMSLogFileUtility::Instance()->BT_TableIndexLog(szMyLog);

				szLog.Format("Re-align (Inari) - EmptyRow triggered - CurrIndex = %lu; NewIndex = %lu; Encoder(%ld, %ld)", 
							ulOrigIndex, ulNextDieIndex, ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
				/******/ 
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
			}
		}

		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Only 1 die on frame1\n", "a+"); /******/ 

		m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
		m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);
		m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, 0);
		m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
		if (bAuto == FALSE && !IsBurnIn())
		{
			SetAlert_Red_Yellow(IDS_BT_ONLY_1_DIE_ON_BLOCK);
		}

		SwitchToBPR();
		CMS896AStn::m_bRealignFrameDone = TRUE;
		return (TRUE);
	}

	//v4.50A23	//WH SanAn
	//******************************************************//
	// Check 1st die position (using PR LookForward) of first row in the grade
	//2018.5.8 not check LookAround1stDie for BinMap
	if (!(m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse)))
	{
		BOOL bLA1stDie = LookAround1stDie(ulBlkInUse, dFOVSize, ulFirstDieRow, ulFirstDieCol);
		if (!bLA1stDie)
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("LookAround 1st Die fail\n", "a+");

			SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UL_DIE);
			//BPR_ZoomSensor(lOldZoomSensorMode); //4X Mode
			SwitchToBPR();
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No UL Die(Look Around)","a+");
			return IDS_BT_REALIGN_NO_UL_DIE;
		}
	}

	//******************************************************//
	// find 1st+1 die (using PR LookForward) of first row in the grade
	BOOL bUse2ndOffsetXY = FALSE;
	BOOL bUse3rdOffsetXY = FALSE;
	FindNextDieOfFirstDie(ulBlkInUse, dFOVSize,	bNewAlignMethod, bUsePR, ulNoOfSortedDie, ulFirstDieIndex, ulDiePerRow,
						  bUse2ndOffsetXY, bUse3rdOffsetXY, lNewXOffset, lNewYOffset, DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y);

	//******************************************************//
	// find last die of first row in the grade
	// off bin table joy stick
	//SetJoystickOn(FALSE);		//v3.71T9
	BOOL bBinMapResetAngle = FALSE;		//v4.39T8
	ULONG ulLastDieRow = 0, ulLastDieCol = 0;
	ulLastDieIndex = FindLastDieOfFirstRowInGrade(ulBlkInUse, ulNoOfSortedDie, ulFirstDieIndex, ulDiePerRow, ulDiePerCol, ulLastDieRow, ulLastDieCol);

	LONG DieBLogical_x = 0, DieBLogical_y = 0;
	GetDieLogicalEncoderValue(ulBlkInUse, ulLastDieIndex, DieBLogical_x, DieBLogical_y);

	m_oBinBlkMain.StepDMove(ulBlkInUse, ulLastDieIndex, dUmX, dUmY);		//v4.59A19
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX + lNewXOffset), ConvertFileUnitToYEncoderValue(dUmY + lNewYOffset));

	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	//v2.58
	Sleep(m_lAlignBinDelay);					//v2.93T2


	// use pr to confirm is the last one in the row and no die above this row or at right hand of this die
	if (bUsePR == TRUE)
	{
		if (bAutoTeach == TRUE)
		{

			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search 2nd die", "a+"); /******/
			
			if (bLargeDie)	//Large die; use single mode	//v2.67
			{
				if (bDummyRun)		//v4.31T10
				{
					lDieFound = TRUE;
				}
				else
				{
					lDieFound = SearchGoodDie();
				}
			}
			else			//Small die; use LA or move-search mode
			{

				/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search first die with 2nd die", "a+"); /******/
				
				if (bDummyRun)		//v4.31T10
				{
					lDieFound = TRUE;
				}
				else
				{
					lDieFound = SearchGoodDie();
					szLog.Format("Search 2nd-die: lDieFound=%d, bMulti=%d", lDieFound, CMS896AApp::m_bBinMultiSearchFirstDie);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					if (lDieFound != TRUE)
					{
						if (m_bSemitekBLMode)
							lDieFound = SearchFirstGoodDie_Semitek(ulBlkInUse);		//v4.50A10
						else
							lDieFound = SearchFirstGoodDie(ulBlkInUse);				//Re-enabled in //v4.38T2
						szLog.Format("Re-search 2nd-die in FOV: lDieFound=%d", lDieFound);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
					}
				}		

				if (lDieFound == TRUE)
				{
					if (bDummyRun)		//v4.31T10
					{
						lDieFound = TRUE;
					}
					else if (m_oBinBlkMain.GrabLSBondPattern())
					{
						lDieFound = TRUE;
					}
					else
					{
						lDieFound = SecondSearchDieWalkPath(ulBlkInUse, dFOVSize, ulLastDieRow, ulLastDieCol);
					}
/*
					//2018.5.8 not check LookAround2ndDie for BinMap
					if (bNewAlignMethod && bUsePR)
					{
						lDieFound = LookAround2ndDie(ulBlkInUse, dFOVSize, ulLastDieRow, ulLastDieCol);
						szLog.Format("Look Around 2nd-die in FOV: lDieFound=%d", lDieFound);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
					}
*/
				}
			}
		}
		else
		{
			lDieFound = FALSE;
		}

		if (lDieFound == FALSE)
		{
			if (!IsBurnIn() && (m_bAlignBinInAlive == FALSE))
			{
				if (!bDisableAlarm)		//v4.24T6
				{
					SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UR_DIE);
				}
//				BPR_ZoomSensor(lOldZoomSensorMode); //4X Mode
				SwitchToBPR();
				if (!bDisableAlarm)
				{
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No UR Die(Disable Alarm)","a+");
					return IDS_BT_REALIGN_NO_UR_DIE;
				}
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No UR Die","a+");
				return FALSE;
			}

			bManualLocate = 1;
		}
		else
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("2nd die is found", "a+");
			bManualLocate = 0;
		}
	}
	else
	{
		bManualLocate = 1;
	}

	// manually confirm the last die
	if (bManualLocate == 1)
	{
		if (!IsBurnIn())
		{
			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Manual locate 2nd die", "a+"); /******/

			//Ask user to  die pos
			SetJoystickOn(TRUE);

//			BPR_ZoomSensor(lOldZoomSensorMode); //4X Mode
			szContent.LoadString(HMB_BT_REALIGN_STEP2);			
			if (HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			{
				SwitchToBPR();
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No UR die to stop","a+");
				return HMB_BT_REALIGN_STEP2;
			}

			// confirm the die
			SetJoystickOn(FALSE);
		}

		if (bUsePR == TRUE)
		{
			SearchGoodDie();
		}
	}

//	BPR_ZoomSensor(lOldZoomSensorMode); //4X Mode
	/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-confirm TR die", "a+"); /******/

	//Sleep(m_lAlignBinDelay);	//v3.71T9
	GetXYEncoderValue(DieBPhysical_x, DieBPhysical_y);
	szLog.Format("REALIGN 2ndDie ENC-B POS XY, (%ld, %ld)", DieBPhysical_x, DieBPhysical_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	LONG lLogicalXDistance = 0, lLogicalYDistance = 0;
	dNewAngle = CalcDieABAngle(ulBlkInUse, 
							   DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y, 
							   DieBLogical_x, DieBLogical_y, DieBPhysical_x, DieBPhysical_y,
							   lDiePitchX, lDiePitchY, 
							   lXDistance, lYDistance, lLogicalXDistance, lLogicalYDistance,
							   dCheckRealignmentAngle);
/*  2018.9.26 it does not check the angle of AB die
	// bin block rotate too much
	if (m_bCheckAlignBinResult && !bBinMapResetAngle && fabs(dNewAngle) >= 0.05236)// 3 degrees	//v4.44A7
	{
		BOOL bFailure = TRUE;
		//Search Top Left again
		//Move to 1St Die Position accroding to the right die position
		lNewXOffset = -ConvertXEncoderValueToFileUnit(DieBPhysical_x - DieBLogical_x);
		lNewYOffset = -ConvertYEncoderValueToFileUnit(DieBPhysical_y - DieBLogical_y);

		m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex, dUmX, dUmY);
		DieALogical_x = ConvertFileUnitToXEncoderValue(dUmX);
		DieALogical_y = ConvertFileUnitToYEncoderValue(dUmY);

		DieAPhysical_x	= ConvertFileUnitToXEncoderValue(dUmX + lNewXOffset);
		DieAPhysical_y	= ConvertFileUnitToXEncoderValue(dUmY + lNewYOffset);

		bMoveTable = BT_MoveTo(DieAPhysical_x, DieAPhysical_y);

		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
		Sleep(m_lAlignBinDelay);
		//==========================================================================

		LONG lDieFound = FirstSearchDieWalkPath(ulBlkInUse, dFOVSize, ulFirstDieRow, ulFirstDieCol);
		if (lDieFound)
		{
			bFailure = FALSE;
			//Get 1st Die Phisical & Logical Position
			GetXYEncoderValue(DieAPhysical_x, DieAPhysical_y);

			lNewXOffset = -ConvertXEncoderValueToFileUnit(DieAPhysical_x - DieALogical_x);
			lNewYOffset = -ConvertYEncoderValueToFileUnit(DieAPhysical_y - DieALogical_y);

			FindNextDieOfFirstDie(ulBlkInUse, dFOVSize,	bNewAlignMethod, bUsePR, ulNoOfSortedDie, ulFirstDieIndex, ulDiePerRow,
								  bUse2ndOffsetXY, bUse3rdOffsetXY, lNewXOffset, lNewYOffset, DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y);
			dNewAngle = CalcDieABAngle(ulBlkInUse, 
									   DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y, 
									   DieBLogical_x, DieBLogical_y, DieBPhysical_x, DieBPhysical_y,
									   lDiePitchX, lDiePitchY, 
									   lXDistance, lYDistance, lLogicalXDistance, lLogicalYDistance,
									   dCheckRealignmentAngle);

			if (m_bCheckAlignBinResult && !bBinMapResetAngle && fabs(dNewAngle) >= 0.05236)// 3 degrees	//v4.44A7
			{
				bFailure = TRUE;
			}
		}

		if (bFailure)
		{
			//m_oBinBlkMain.SetIsAligned(ulBlkInUse, bOldAligned);		//v3.44
			if (!IsBurnIn())
			{
				szLog.Format("Bin block rotate too much %d, %d, %.6f", lXDistance, lYDistance, fabs(dNewAngle));
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "\n", "a+");
				if (!bDisableAlarm)		//v4.24T6
				{
					SetAlert_Red_Yellow(IDS_BT_REALIGN_ANGLE_TOO_LARGE);
				}
				SwitchToBPR();
				if (!bDisableAlarm)
				{
					return IDS_BT_REALIGN_ANGLE_TOO_LARGE;
				}
			}
			return FALSE;
		}
	}
*/
	// lPhysicalDistance = (physical b -- physical a);
	DOUBLE dXDistance = (DOUBLE)lXDistance;
	DOUBLE dYDistance = (DOUBLE)lYDistance;
	lPhyDist = (LONG)sqrt(pow(dXDistance, 2.0) + pow(dYDistance, 2.0));

	//Nichia//v4.43T7
	if (m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse) == TRUE)
	{
		DOUBLE dLogicalXDistance = (DOUBLE)lLogicalXDistance;
		DOUBLE dLogicalYDistance = (DOUBLE)lLogicalYDistance;
		lLogDist = (LONG) sqrt( pow(dLogicalXDistance, 2.0) + pow(dLogicalYDistance, 2.0) );
		szLog.Format("LogDist = %ld (%ld, %ld)", lLogDist, lLogicalXDistance, lLogicalYDistance);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	}
	else
	{
		DOUBLE dX1, dY1, dX2, dY2;
		if (bUse3rdOffsetXY)	//v4.59A36
		{
			m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex+2,	dX1, dY1);
			szLog.Format("Use 1st+2 Die for Logical-dist calculation at (%f, %f)", dX1, dY1);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
		else if (bNewAlignMethod && bUse2ndOffsetXY)		//v4.43T13	//Use 1st+1 die for calculation
		{
			m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex+1,	dX1, dY1);
			szLog.Format("Use 1st+1 Die for Logical-dist calculation at (%f, %f)", dX1, dY1);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
		else
		{
			m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex,	dX1, dY1);
			szLog.Format("Use 1st Die for Logical-dist calculation at (%f, %f)", dX1, dY1);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}

		m_oBinBlkMain.StepDMove(ulBlkInUse, ulLastDieIndex, dX2, dY2);
		lLogDist = (LONG)sqrt(pow((dX1 - dX2), 2.0) + pow((dY1 - dY2), 2.0));
	}


	szLog.Format("2 Die distance (P/L) = %d,%d (XPitch = %.1f x 3/4), angle = %.6f degree (radian = %.6f)", 
				 lPhyDist, lLogDist, m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse), dNewAngle * 180.0 / PI, dNewAngle);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

	//2018.6.16 it remove the over picth checking becasue it is no used for last die
/*	
	//v4.58A5	//Finisar	//Need to consider X & Y with different pitch size for rectangular die
	DOUBLE dPitchToBeCompared = m_oBinBlkMain.GrabDiePitchX(ulBlkInUse) * 3.0 / 4.0;
	if ((m_oBinBlkMain.GrabWalkPath(ulBlkInUse) == BT_TLV_PATH) ||
		(m_oBinBlkMain.GrabWalkPath(ulBlkInUse) == BT_TRV_PATH) ||
		(m_oBinBlkMain.GrabWalkPath(ulBlkInUse) == BT_BLV_PATH) ||
		(m_oBinBlkMain.GrabWalkPath(ulBlkInUse) == BT_BRV_PATH) )
	{
		dPitchToBeCompared = m_oBinBlkMain.GrabDiePitchY(ulBlkInUse) * 3.0 / 4.0;
	}
	LONG lPitchToBeCompared = (LONG) dPitchToBeCompared;

	//if (m_bCheckAlignBinResult && abs(lPhyDist - lLogDist) > m_oBinBlkMain.GrabDiePitchX(ulBlkInUse) * 3 / 4)
	if (m_bCheckAlignBinResult && abs(lPhyDist - lLogDist) > lPitchToBeCompared)
	{
		// if physical dist between 1st and 2nd die and logical row length diff by larger than 1-pitch
		if (!IsBurnIn())
		{
			str.Format("Diff. of REALIGN distance %d and LOGICAL distance %d between 1st and 2nd die is over one-pitch!", 
				lPhyDist, lLogDist);
	
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(str + "\n", "a+"); 
			if (!bDisableAlarm)		//v4.24T6
			{
				SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_FAILED, str);
			}
		}
			
		SwitchToBPR();
		return FALSE;
	}
*/
	if (bBinMapResetAngle)									//v4.40T2	//EverVision
	{
		dNewAngle = 0;
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Andrew: reset angle to 0", "a+"); /******/ 
	}

	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))
	{
		ULONG ulNoOf1stRowDices = ulDiePerRow;
		m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices, ulLastDieRow, ulLastDieCol);
		if (ulNoOfSortedDie < ulNoOf1stRowDices)
		{
			dNewAngle = 0;
			str.Format("(BINMAP) Reset angle to 0 - NoOfBOndDices=%ld, NoOfDicesOn1stRow=%ld", 
						ulNoOfSortedDie, ulNoOf1stRowDices);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(str, "a+");
		}
	}


	//******************************************************//
	// check last die here
	LONG l3rdDieX = 0, l3rdDieY = 0;
	LONG l3rdDieOrigX = 0, l3rdDieOrigY = 0;
	LONG lLastDieOffsetX = 0;	
	LONG lLastDieOffsetY = 0;

	double dRealignmentLatestAngle = dNewAngle;
	if (bUsePR == FALSE)
	{
		// just move bin table to last die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie;

		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		ULONG ulLastIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);		
		for (INT i = 1; i < (INT) ulLastIndex; i++)		
		{
			for (INT j = 0; j < (INT) m_ulHoleDieNum; j++)									
			{
				if	(m_oBinBlkMain.GrabRandomHoleDieIndex(ulBlkInUse, j) == i)
				{
					ulDieIndex++;
				}
			}
		}

		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dUmX, dUmY);	//v4.59A19
		ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
	}
	else
	{
		//SetJoystickOn(FALSE);		//v3.71T9

		// move bin table to last die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		szLog.Format("Confirm last die (3pt), INDEX = #%lu", ulDieIndex);
		/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/


		if (pApp->GetCustomerName() == "CyOptics"	||
			pApp->GetCustomerName() == "Inari"		||		//v4.51A24
			pApp->GetCustomerName() == "FiberOptics")		//v4.31T8
		{
			ulDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse);
			szLog.Format("Confirm last die (3pt EMPTYROW), INDEX = #%lu", ulDieIndex);
			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}

		//v4.35T3	//PLLM MS109 Lumiramic
		if (m_oBinBlkMain.GrabLSBondPattern())
		{
			//** e.g. LS Bond sequence with TLH path, DiePerRow = 10 **//
			//
			//     1           
			// 11 10  9  8  7  6  5  4  3  2
			//    12 13 14 15 16 17 18 19 20
			//    29 28 27 26 25 24 23 22 21
			//    31 32 33 .......
			//
			
			ULONG ulOrigDieIndex = ulDieIndex;
			ULONG ulLastDieIndex = ulDieIndex;
			ULONG ulNextIndex = 0;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulLastDieIndex, ulNextIndex))
			{
				ulDieIndex = ulNextIndex;
			}

			szLog.Format("Search Last die (blk #%d) with LS pattern: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
		}
		else if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse))	//v4.36
		{
			ULONG ulOrigDieIndex = ulDieIndex;

			szLog.Format("Search Last die (blk #%d) in BINMAP: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

			//v4.59A31
			//******************************************************//
			// check last row angle
			if (IsMS90())		//v4.59A41
			{
				if (bSecondRealign)
				{
					dNewAngle = 0;
				}
				BOOL bTStatus = AdjustGlobalThetaUsingLastRowCol(ulBlkInUse, ulDieIndex, dCheckRealignmentAngle, bSecondRealign,
																	dNewAngle, dRealignmentLatestAngle, lNewXOffset, lNewYOffset);
				if (!bTStatus)
				{
					SwitchToBPR();
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()Adjust Global Theta Using Last Row Col Fails (BM Bond Area)","a+");
					return FALSE;
				}
			}
		}
		else if (m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))	//Nichia//v4.43T7
		{
			ULONG ulOrigDieIndex = ulDieIndex;

			ULONG lCurrIndex = 1;
			for (INT i = 1; i < (LONG)ulOrigDieIndex; i++)
			{
				ULONG ulTempRow = 0, ulTempCol = 0;
				m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, lCurrIndex, ulTempRow, ulTempCol);
				lCurrIndex = lCurrIndex + 1;
			}
			ulDieIndex = lCurrIndex;

			szLog.Format("Search Last die (blk #%d) in BINMAP2: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, lCurrIndex, ulOrigDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
		
		}
		else
		{
			ULONG ulOrigDieIndex = ulDieIndex;
			//ulDieIndex = m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, ulDieIndex);	

			//szLog.Format("Search Last die (blk #%d) in BINMAP: NewIndex=%lu, OrigIndex=%lu", 
			//			 ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

			//v4.59A31
			//******************************************************//
			// check last row angle
			if (IsMS90())
			{
				if (bSecondRealign)
				{
					dNewAngle = 0;
				}
				BOOL bTStatus = AdjustGlobalThetaUsingLastRowCol(ulBlkInUse, ulDieIndex, dCheckRealignmentAngle, bSecondRealign,
																	dNewAngle, dRealignmentLatestAngle, lNewXOffset, lNewYOffset);
				if (!bTStatus)
				{
					SwitchToBPR();
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()Adjust Global Theta Using Last Row Col Fails ","a+");
					return FALSE;
				}
			}
		}

		//==================================================================================================
		//  Move to the position of the last die
		//==================================================================================================
		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		//ULONG ulLastIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dUmX, dUmY, TRUE);	//v4.59A19
		l3rdDieOrigX = ConvertFileUnitToXEncoderValue(dUmX);
		l3rdDieOrigY = ConvertFileUnitToYEncoderValue(dUmY);
		
		//v4.52A8
		szLog.Format("BLK #%lu LastDie (Before comp) - XEnc=%ld, YEnc=%ld (%.2f, %ld, %ld)", 
			ulBlkInUse, l3rdDieOrigX, l3rdDieOrigY, dNewAngle, lNewXOffset, lNewYOffset);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		LONG lX1 = _round(dUmX);
		ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
		l3rdDieX = ConvertFileUnitToXEncoderValue(dUmX);
		l3rdDieY = ConvertFileUnitToYEncoderValue(dUmY);
		szLog.Format("BLK #%lu LastDie (Before PR) - XEnc=%ld, YEnc=%ld", ulBlkInUse, l3rdDieX, l3rdDieY);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

		//================================the position of next bonding die======================================
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + 1;
		//======================================================================================================

		//v4.48A26	//Cree Huizhou
		if (m_oBinBlkMain.TwoDimensionBarcodeFindNextBondIndex(ulBlkInUse, ulDieIndex))
		{
			//LONG lLast1X=0, lLast1Y=0;
			DOUBLE dLast1X=0, dLast1Y=0;
			m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dLast1X, dLast1Y, TRUE);	//Last+1+1(HOLE)

			CString szMsg;
			szMsg.Format("REALIGN-BIN #%d at Last+1 - HOLE detected at INDEX = %ld, New = %ld, Enc (%f, %f)",	//v4.48A21
				ulBlkInUse, ulDieIndex-1, ulDieIndex, dLast1X, dLast1Y);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

			szLog.Format("BLK #%lu LastDie+1 encounter 2D HOLE - New INDEX = %ld", ulBlkInUse, ulDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}

		if (pApp->GetCustomerName() == "CyOptics"	|| 
			pApp->GetCustomerName() == "Inari"		||		//v4.51A24
			pApp->GetCustomerName() == "FiberOptics")		//v4.31T8
		{
			ulDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse) + 1;
		}

		//v4.35T3	//PLLM MS109 Lumiramic
		if (m_oBinBlkMain.GrabLSBondPattern())
		{
			ULONG ulOrigDieIndex = ulDieIndex;
			ULONG ulLastDieIndex = ulDieIndex;
			ULONG ulNextIndex = 0;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulLastDieIndex, ulNextIndex))
			{
				ulDieIndex = ulNextIndex;
			}

			szLog.Format("Search Last+1 die (blk #%d) with LS pattern: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

			ULONG ulLSIndex = 0;
			m_oBinBlkMain.RecalculateLSBondPatternIndex(ulBlkInUse, ulNoOfSortedDie, ulLSIndex);
			m_oBinBlkMain.SaveEmptyUnitsToMSD();
		}
		else if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse))	//v4.36
		{
			ULONG ulOrigDieIndex = ulDieIndex;
			//ulDieIndex = m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, ulDieIndex);	//v4.36

			szLog.Format("Search Last+1 die (blk #%d) in BINMAP: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}
		else if (m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))		//Nichia//v4.43T7
		{
			ULONG ulOrigDieIndex = ulDieIndex;
			//ulDieIndex = m_oBinBlkMain.GetDieIndexInBinMap(ulDieIndex);

			ULONG lCurrIndex = 1;
			for (INT i = 1; i < (LONG)ulOrigDieIndex; i++)
			{
				ULONG ulTempRow = 0, ulTempCol = 0;
				m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, lCurrIndex, ulTempRow, ulTempCol);
				lCurrIndex = lCurrIndex + 1;
			}
			ulDieIndex = lCurrIndex;

			szLog.Format("Search Last+1 die (blk #%d) in BINMAP2: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}

		// Align Last-die and return its XY placement
		//lDieFound1 = SearchGoodDie();	
		BOOL bPlacementOk = TRUE;
		DOUBLE dX = 0, dY = 0;
		BOOL bDXFail=FALSE, bDYFail=FALSE;
		Sleep(m_lAlignBinDelay);
		lDieFound1 = Search3rdGoodDie(bPlacementOk, dX, dY, bDXFail, bDYFail);
		
		//find and search the upper die of bonding die 
		if (!lDieFound1)
		{
			//===================================================================================================================================
			//Get the map row/column for bonding die
			ULONG ulNewIndex = ulDieIndex;
			ULONG ulBinMapRow = 0, ulBinMapCol = 0;
			//Get Current die map position(ulBinMapRow, ulBinMapCol)
			m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, ulNewIndex, ulBinMapRow, ulBinMapCol);

			//Get the last map row/col of bonded die 
			ULONG ulCurBinMapPath = m_oBinBlkMain.GetCurPathInBinMap(ulBinMapRow, ulBinMapCol);
			//Y Direction
			LONG lRowDir = 0, lColDir = 0;
			BOOL bFrameRotate180 = m_oBinBlkMain.GetFrameRotation(ulBlkInUse);
			ULONG ulPRLookAheadYDirection = GetLookAheadYDirectionWtihPreviousDie(ulCurBinMapPath, bFrameRotate180, lRowDir, lColDir);

			//the row/column of last bonded die
			ulBinMapRow += lRowDir;
			ulBinMapCol += lColDir;
			if (m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, ulBinMapRow, ulBinMapCol))
			{
				ULONG ulIndexCount = 0;
				m_oBinBlkMain.GetDieIndexInBinMapWithRowCol(ulBlkInUse, ulBinMapRow, ulBinMapCol, ulIndexCount);
				m_oBinBlkMain.SubStepDMove(ulBlkInUse, ulIndexCount, dUmX, dUmY);

				ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
				BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
				Sleep(m_lAlignBinDelay);
				lDieFound1 = Search3rdGoodDie(bPlacementOk, dX, dY, bDXFail, bDYFail);
			}
			//===================================================================================================================================
		}


		if (!lDieFound1)
		{
			CPRFailureCaseLog *pPRFailureCaseLog = CPRFailureCaseLog::Instance();
			if (pPRFailureCaseLog->IsEnableFailureCaselog())
			{
				CloseAlarm();
				CString szMsg;
				szMsg = "Log PR Failure Case...";
				SetAlert_WarningMsg(IDS_MS_WARNING_LOG_PR_FAILURE_CASE, szMsg);
				pPRFailureCaseLog->LogFailureCaseDumpFile(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, 5);
				if (State() == IDLE_Q)
				{
					SetAlarmLamp_Yellow(FALSE, TRUE);
				}
				else
				{
					SetAlarmLamp_Green(FALSE, TRUE);
				}
				CloseAlarm();
			}
		}

		if (bDummyRun)
		{
			bPlacementOk = TRUE;
			lDieFound1 = TRUE;
		}

		//v4.36	//PLLM MS109
		//If under MS109 AUTO mode the same BT1 has confirm & bypass LastDie failure in setup, then let it go
		BOOL bDisableMS109LastDieOffsetX = FALSE;
		if (!m_bAlignBinInAlive && m_bUseDualTablesOption && m_bManualConfirmLastDieResult1)
		{
			if (lDieFound1 && !bPlacementOk)	
			{
				bPlacementOk = TRUE;
				if (m_lManualConfirmXOffset != 0)
				{
					lNewXOffset = m_lManualConfirmXOffset;
				}
				if (m_lManualConfirmYOffset != 0)
				{
					lNewYOffset = m_lManualConfirmYOffset;
				}
				m_lManualConfirmXOffset = 0;
				m_lManualConfirmYOffset = 0;
				bDisableMS109LastDieOffsetX = TRUE;
			}

			m_bManualConfirmLastDieResult1 = FALSE;
			lDieFound1 = TRUE;
		}
		else if (m_bManualConfirmLastDieResult1)
		{
			m_bManualConfirmLastDieResult1 = FALSE;
		}

		// Calculate LastDie + 1 posn
		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dUmX, dUmY, TRUE);	//v4.59A19		//v4.59A23
		LONG lX2 = _round(dUmX);


		if (m_bUseDualTablesOption && bPlacementOk && !bDisableMS109LastDieOffsetX)		//v4.38T7
		{
			if (m_dXResolution != 0)
			{
				lNewXOffset = _round(lNewXOffset - dX / m_dXResolution);
			}
			if (m_dYResolution != 0)
			{
				lNewYOffset = _round(lNewYOffset - dY / m_dYResolution);    //v4.39T6
			}		
			
			//OsramTrip 8/22
			ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);	//LastDie + 1 //leo 20170824
		}
		else
		{
			//OsramTrip 8/22
			ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);	//LastDie + 1 //leo 20170824
		}		

		//v3.65
		// Look-ahead other PR sub-windows at Last-Die position
		CString szLfError = "Warning: ";
		LONG lLfDieOK = TRUE;	//Always disable lookahead function on 3rd die

		//v4.26T1	//Semitek
		BOOL bRelOffsetXYinFOV = TRUE;
		BOOL bLaTopDieOK = FALSE, bLaSideDieOK = FALSE;
		if (bNewAlignMethod)
		{
			bRelOffsetXYinFOV = Check3rdGoodDieRelOffsetXYinFOV(ulBlkInUse, dFOVSize, szLfError, bLaTopDieOK, bLaSideDieOK);
		}

		// move bin table to last+1 die of this block, use realign result
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));	//v4.59A23
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(m_lAlignBinDelay);			//v2.93T2		

		lDieFound2 = SearchGoodDie();
		if (bDummyRun)		//v4.31T10
		{
			lDieFound2 = FALSE;
		}

		//leo 20170822 add logging
		szLog.Format("BLK #%lu LastDie+1 - RelXEnc=%ld, RelYEnc=%ld, dX=%lf, dY=%lf, Xb4convertUmX=%lf, Xb4convertUmY=%lf, AfterConvertXUmX=%ld, AfterConvertXUmY=%ld", 
					 ulBlkInUse, ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmX), dX, dY, dUmX, dUmY, ConvertFileUnitToXEncoderValue(dUmX),ConvertFileUnitToYEncoderValue(dUmY));
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		
		szLog.Format("PR Reault: Last = %d; Last+1 = %d; PlacementOK = %d; Alive = %d,SecondAlign,%d", lDieFound1, lDieFound2, bPlacementOk, m_bAlignBinInAlive,bSecondRealign );
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		// Determine last-die result
		if (lDieFound1 == TRUE && lDieFound2 != TRUE)
		{
			CString szMsg;
			if (!bPlacementOk && !IsBurnIn() && m_bAlignBinInAlive)		//v3.62	
			{
				LONG lPosX1 = 0, lPosX2 = 0;
				LONG lPosY1 = 0, lPosY2 = 0;
				GetXYEncoderValue(lPosX1, lPosY1);

				SwitchToBPR();
				DisplayBondPrDieSizeWindow(TRUE);

				if (!bNewAlignMethod)	// || bRelOffsetXYinFOV)	//v4.43T12		//v4.26T1	//Semitek
				{
					//** Case #1 **
					//		LA dies OK; only Last-Die is shift;
					//		-> use original calculated XY pos for NEXT die
					szLog = "CASE #1: follow 1st-Die";
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
				}
				else
				{
					//if (bLaTopDieOK || bLaSideDieOK)	//v4.43T12
					//{
						//** Case #2b **
						//		NEITHER LA-dies nor Last-Die are OK -> the frame is shifted!
						//		-> move BT by Last-Die offset XY for OP to confirm instead
					//	szLog = "CASE #2b: follow 1st-Die";
					//	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
					//}
					//else
					//{
					//** Case #2a **
					//		NEITHER LA-dies nor Last-Die are OK -> the frame is shifted!
					//		-> move BT by Last-Die offset XY for OP to confirm instead
					// leo 20170824 move with the correct relative x y encoder value
					LONG lXInStep = (LONG) dX;
					LONG lYInStep = (LONG) dY;

					//leo 20170824 add logging
					szLog.Format("BLK #%lu Relative Move X =%ld, Move YE=%ld", 
					 ulBlkInUse, lXInStep, lYInStep);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

					szLog.Format("CASE #2a (MANUAL): follow Last-Die offset XY, PR Offset (%d, %d)", lXInStep, lYInStep);	//v4.59A23
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
					
					X_Move(lXInStep, FALSE);
					Y_Move(lYInStep);
					X_Sync();
					Sleep(50);
				}

				//Ask user to die pos
				if (bNewAlignMethod && m_bSemitekBLMode)	//v4.42T9
				{
					if ( (pApp->GetCustomerName() == "SanAn") && (!pApp->IsUsingEngineerMode()))	//v4.51A7	//LeoLam
					{
						szMsg = _T("3rd die placement fail; operation is aborted (SanAn)");		
						HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user (SanAn)\n", "a+");
						SwitchToBPR();
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
					else if( pApp->GetCustomerName() == "Electech3E(DL)")
					{
						// 4.53D26 manual checking is the same as the auto checking
						if (!bRelOffsetXYinFOV)
						{
							//BOOL bOnJoystick = m_bJoystickOn; 
							//if(!bOnJoystick)
								SetJoystickOn(TRUE);
							//AfxMessageBox("Please click !", MB_SYSTEMMODAL);
							szMsg = _T("Please confirm NEXT die position*; if OK press CONTINUE.");		
							SetStatusMessage(szMsg);
							if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
							{
								szMsg = "**Confirm Next Die pos aborted by user";
								SetStatusMessage(szMsg);
								CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg + "\n", "a+");
								SwitchToBPR();
								//SetAlarmLamp_Yellow();
								return FALSE;
							}
							else
							{
								szMsg = "**Confirm Next Die pos CONTINUE";
								SetStatusMessage(szMsg);
							}

							//if(!bOnJoystick) 
								SetJoystickOn(FALSE);
						}
						else
						{
							szLog = "*CASE #2a (MANUAL): follow Last-Die offset XY";
							SetStatusMessage(szLog);
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
						}

					}
					else
					{
						szMsg = _T("Please confirm NEXT die position; if OK press CONTINUE.");		
						SetStatusMessage(szMsg);
						if (!bSecondRealign)
						{
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()1st Align Abort1","a+");
							return FALSE;
						}

						if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
						{
							szMsg = "Confirm Next Die pos (Semitek) aborted by user";
							SetStatusMessage(szMsg);
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg + "\n", "a+");
							SwitchToBPR();
							//SetAlarmLamp_Yellow();
							return FALSE;
						}
						else
						{
							szMsg = "*Confirm Next Die pos (Semitek) CONTINUE";
							SetStatusMessage(szMsg);
						}
				    }
				}
				else
				{
					CMS896AStn::MotionSelectIncrementParam(BT_AXIS_X, "BT_JoySlow", &m_stBTAxis_X);
					CMS896AStn::MotionSelectIncrementParam(BT_AXIS_Y, "BT_JoySlow", &m_stBTAxis_Y);
					SetJoystickOn(TRUE);
					m_lJoystickLevel = 0;

					//v4.43T1	//andrewng
					if ((pApp->GetCustomerName() == "SanAn") && (!pApp->IsUsingEngineerMode()))
					{
						szMsg = _T("3rd die placement fail; operation is aborted (SanAn)");		
						HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user (SanAn)\n", "a+");
						SwitchToBPR();
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
					else if ((pApp->GetCustomerName()=="ChangeLight(XM)") && (!pApp->IsUsingEngineerMode()))
					{
						szMsg.Format("Realign(%d): 3rd die placement fail; operation is aborted (ChangeLight)", bSecondRealign ? 1 : 2);		
						HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
						szMsg += "\n";
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
						SwitchToBPR();
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
					else
					{
						if (!bSecondRealign)
						{
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()1st Align Abort2","a+");
							return FALSE;
						}

						szMsg.Format("Realign(%d): 3rd die placement fail; please specify last-die position with joystick, then press CONTINUE.", bSecondRealign ? 1 : 2);		
						if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
						{
							szMsg.Format("Realign(%d): 3rd die placement fail; Confirm Next Die pos aborted by user\n", bSecondRealign ? 1 : 2);	
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
							SwitchToBPR();
							//SetAlarmLamp_Yellow();
							return FALSE;
						}
					}

					SetJoystickOn(FALSE);
				}

				Sleep(100);
				GetXYEncoderValue(lPosX2, lPosY2);
				lNewXOffset = lNewXOffset - ConvertXEncoderValueToFileUnit(lPosX2 - lPosX1);
				lNewYOffset = lNewYOffset - ConvertYEncoderValueToFileUnit(lPosY2 - lPosY1);

				//v4.38T7		//Bypass LastDie failure in manual RealignFrame fcn in MS109
				if (m_bUseDualTablesOption)	
				{
					m_bManualConfirmLastDieResult1 = TRUE;
					m_lManualConfirmXOffset = lNewXOffset;
					m_lManualConfirmYOffset = lNewYOffset;
				}
			}
			else if ((bPlacementOk == FALSE) && (IsBurnIn() == FALSE))
			{
				if (bNewAlignMethod)	//v4.43T14	//Semitek
				{
					LONG lPosX1 = 0, lPosX2 = 0;
					LONG lPosY1 = 0, lPosY2 = 0;
					GetXYEncoderValue(lPosX1, lPosY1);

					szLog = "CASE #2a (AUTO): follow Last-Die offset XY";
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
					
					LONG lXInStep = (LONG) dX;
					LONG lYInStep = (LONG) dY;
					X_Move(lXInStep, FALSE);
					Y_Move(lYInStep);
					X_Sync();

					Sleep(100);
					GetXYEncoderValue(lPosX2, lPosY2);
					lNewXOffset = lNewXOffset - ConvertXEncoderValueToFileUnit(lPosX2 - lPosX1);
					lNewYOffset = lNewYOffset - ConvertYEncoderValueToFileUnit(lPosY2 - lPosY1);

					//v4.38T7		//Bypass LastDie failure in manual RealignFrame fcn in MS109
					if (m_bUseDualTablesOption)	
					{
						m_bManualConfirmLastDieResult1 = TRUE;
						m_lManualConfirmXOffset = lNewXOffset;
						m_lManualConfirmYOffset = lNewYOffset;
					}
					

					if (!bRelOffsetXYinFOV)
					{
						if (!bSecondRealign)
						{
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()1st Align Abort3","a+");
							return FALSE;
						}
						szLog.Format("Realign(%d): CASE #2b (AUTO): Last-Die placement failure to machine stop", bSecondRealign ? 1 : 2);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

						szMsg = _T("Bin Last-Die placement failure; machine is aborted.");		
						HmiMessageEx_Red_Back(szMsg, szTitle);
						SetStatusMessage(szMsg);
						SwitchToBPR();
						return FALSE;
					}
					else
					{
						szLog.Format("Realign(%d): CASE #2a (AUTO): follow Last-Die offset XY", bSecondRealign ? 1 : 2);
						SetStatusMessage(szLog);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
					}
				}
				else
				{
					if (!bSecondRealign)
					{
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()1st Align Abort4","a+");
						return FALSE;
					}
					szMsg.Format("Realign(%d): dX = %.2f steps, dY = %.2f steps.", bSecondRealign ? 1 : 2, dX, dY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Last Die pos placement fails\n", "a+");

					if (!bDisableAlarm)		//v4.24T6
					{
						SetAlert_Red_Yellow(IDS_BT_REALIGN_LAST_DIE_SHIFT);
					}
					SwitchToBPR();
					if (!bDisableAlarm)
					{
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()Realign Last Die fails (!Disable Alarm)","a+");
						return IDS_BT_REALIGN_LAST_DIE_SHIFT;
					}
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()Realign Last Die fails","a+");
					return FALSE;
				}
			}

			LONG PosX, PosY;
			GetXYEncoderValue(PosX, PosY);
			m_lX = PosX;
			m_lY = PosY;

			//v4.44A5
			CString szMyLog;
			szMyLog.Format("RealignFrame 1: m_lX = %ld, m_lY = %ld", m_lX, m_lY);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szMyLog);

			//lNewXOffset = lNewXOffset + lLastDieOffsetX;
			//lNewYOffset = lNewYOffset + lLastDieOffsetY;
		}
		else if (lDieFound1 != TRUE && lDieFound2 != TRUE)		//v2.56
		{
			if (!bSecondRealign)
			{
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()Die 1 2 not found return False","a+");
				return FALSE;
			}
			if (IsBurnIn())		//v3.34
			{
				bRealignFailed = FALSE;
			}
			else if (bNewAlignMethod && bRelOffsetXYinFOV && m_bAlignBinInAlive)
			{
				//** Case #3 **
				//		If Last-Die is not found, but LA dices are all OK
				//		-> still use original calculated XY pos for NEXT die
				szLog = "CASE #3";
				/******/ 
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

				LONG lPosX1 = 0, lPosX2 = 0;
				LONG lPosY1 = 0, lPosY2 = 0;
				GetXYEncoderValue(lPosX1, lPosY1);

				SwitchToBPR();
				DisplayBondPrDieSizeWindow(TRUE);

				//Ask user to die pos
				CString szMsg;
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				if (bNewAlignMethod && m_bSemitekBLMode)	//(pApp->GetCustomerName() == "Semitek"))	//v4.42T9
				{
					if ((pApp->GetCustomerName() == "SanAn" || pApp->GetCustomerName() == "ChangeLight(XM)") && (!pApp->IsUsingEngineerMode()))		//v4.51A7	//LeoLam
					{
						szMsg = _T("Last die is not found; operation is aborted ");		
						SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Conirm Last Die pos without die aborted by user 2 (XXX)\n", "a+");
						SwitchToBPR();
						return FALSE;
					}
					else
					{
						szMsg = _T("Please confirm NEXT die position; if OK press CONTINUE.");		
						SetStatusMessage(szMsg);
						if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
						{
							szMsg.Format("Realign(%d): Conirm Last Die pos without die (Semitek) aborted by user", bSecondRealign ? 1 : 2);
							SetStatusMessage(szMsg);
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg + "\n", "a+");
							SwitchToBPR();
							return FALSE;
						}
						else
						{
							szMsg = "Confirm Next Die pos (Semitek) CONTINUE";
							SetStatusMessage(szMsg);
						}
					}
				}
				else
				{
					CMS896AStn::MotionSelectIncrementParam(BT_AXIS_X, "BT_JoySlow", &m_stBTAxis_X);
					CMS896AStn::MotionSelectIncrementParam(BT_AXIS_Y, "BT_JoySlow", &m_stBTAxis_Y);
					SetJoystickOn(TRUE);
					m_lJoystickLevel = 0;

					if ( (pApp->GetCustomerName()=="ChangeLight(XM)") && (!pApp->IsUsingEngineerMode()))
					{
						szMsg = _T("3rd die placement fail; operation is aborted (ChangeLight)");		
						HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
						szMsg.Format("Realign(%d): Confirm Next Die pos aborted by user (ChangeLight)\n", bSecondRealign ? 1 : 2);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
						SwitchToBPR();
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
					else
					{
						if (IsMS90() && sRetry == 0)	//v4.59A31
						{
							szMsg.Format("Realign(%d): Confirm LastDie fail AUTO abort and retry\n", bSecondRealign ? 1 : 2);
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
							return FALSE;
						}
						else
						{
							//szMsg = _T("3rd die placement fail; please specify last-die position with joystick, then press CONTINUE.");
							szMsg = _T("Can not detect last die by PR; please specify last-die position with joystick, then press CONTINUE.");
							if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
							{
								/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Conirm Last Die pos without die aborted by user\n", "a+"); /******/
								SwitchToBPR();
								//SetAlarmLamp_Yellow();
								return FALSE;
							}
						}
					}

					SetJoystickOn(FALSE);
				}

				Sleep(100);
				GetXYEncoderValue(lPosX2, lPosY2);
				lNewXOffset = lNewXOffset - ConvertXEncoderValueToFileUnit(lPosX2 - lPosX1);
				lNewYOffset = lNewYOffset - ConvertYEncoderValueToFileUnit(lPosY2 - lPosY1);
			}
			else
			{
				CString szMsg = " ";
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && !m_bAlignBinInAlive)	//xyz
				{
					SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Conirm Last Die pos without die aborted by user (PLLM)\n", "a+");
					SwitchToBPR();
					return IDS_BT_REALIGN_NO_LAST_DIE;
				}
				//andrewng
				else if ((pApp->GetCustomerName() == "SanAn" || pApp->GetCustomerName() == "ChangeLight(XM)") && (!pApp->IsUsingEngineerMode()))			//v4.43T3
				{
					szMsg = _T("Last die is not found; operation is aborted");		
					SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Conirm Last Die pos without die aborted by user (QQQ)\n", "a+");
					SwitchToBPR();
					return IDS_BT_REALIGN_NO_LAST_DIE;
				}
				else
				{
					if (SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Continue", "Stop") != 1)
					{
						SwitchToBPR();
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No Last Die To Stop","a+");
						return IDS_BT_REALIGN_NO_LAST_DIE;
					}
				}

				//v4.36		//Bypass LastDie failure in manual RealignFrame fcn in MS109
				if (m_bAlignBinInAlive && m_bUseDualTablesOption)		
				{
					m_bManualConfirmLastDieResult1 = TRUE;
					m_lManualConfirmXOffset = lNewXOffset;
					m_lManualConfirmYOffset = lNewYOffset;
				}
			}
		}
		else
		{
			bRealignFailed = TRUE;
		}


		if (bRealignFailed == TRUE)
		{
			if (!bSecondRealign)
			{
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()1st Align Abort5","a+");
				return FALSE;
			}
			if (!IsBurnIn())
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE);
			}

			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-align failed\n", "a+"); /******/

			SwitchToBPR();
			if (!IsBurnIn())
			{
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No Last Auto Stop","a+");
				return IDS_BT_REALIGN_NO_LAST_DIE;
			}
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("!@#$%^&*()No Last burnin Stop","a+");
			return FALSE;
		}
	}

	//v4.31T10
	if (bAuto && 
		(pApp->GetCustomerName() == "CyOptics"	|| 
		 pApp->GetCustomerName() == "Inari"		||
		 pApp->GetCustomerName() == "FiberOptics") )	//v4.52A3
	{
		ULONG ulNextDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse) + 1;
		ULONG ulOrigIndex = ulNextDieIndex;
		if (m_oBinBlkMain.CheckIfEmptyRow(ulBlkInUse))
		{
			m_oBinBlkMain.CheckIfNeedToLeaveEmptyRow(ulBlkInUse, ulNextDieIndex, TRUE);
			m_oBinBlkMain.StepDMove(ulBlkInUse, ulNextDieIndex, dUmX, dUmY);	//v4.59A19
			ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
			BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));

			//Used in OpMoveTable() in CycleState
			m_lX = ConvertFileUnitToXEncoderValue(dUmX);
			m_lY = ConvertFileUnitToXEncoderValue(dUmY);
			Sleep(100);

			//v4.44A5
			CString szMyLog;
			szMyLog.Format("RealignFrame 2: m_lX = %ld, m_lY = %ld", m_lX, m_lY);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szMyLog);

			szLog.Format("Re-align (CyOptics) - EmptyRow triggered - CurrIndex = %lu; NewIndex = %lu; Encoder(%ld, %ld)", 
						 ulOrigIndex, ulNextDieIndex, ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}
	}
	//leo 20171105 Final Check if Angle larger than 0.01 degree, false to stop;

	DOUBLE dNewAngleInDegree2 = fabs(dRealignmentLatestAngle * 180.0/ PI);
	if ((dNewAngleInDegree2 > dCheckRealignmentAngle) && (pApp->GetCustomerName()==CTM_FINISAR || pApp->GetCustomerName() == CTM_NICHIA) && !CMS896AApp::m_bEnableSubBin && (ulNoOfSortedDie >= ulDiePerRow))	//0.01 degree
	{
		CString szMsg;
		CString szTitle;
		szTitle = "Global Angle Alignment";
		szMsg.Format("Global Angle exceeds (%.6f) degree (%.6f); operation is aborted. Please Unload the frame", dCheckRealignmentAngle, dNewAngleInDegree2);		
		HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg + "Confirm Next Die pos aborted by user\n", "a+");
		SwitchToBPR();
		//SetAlarmLamp_Yellow();
		return FALSE;
	}
	else
	{
		CString szMsg;
		szMsg.Format("Realign(%d): Global Angle (%.6f) < degree (%.6f)", bSecondRealign ? 1 : 2, dCheckRealignmentAngle, dNewAngleInDegree2);		
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg + "Confirm Angle OK", "a+");
	}

	szLog.Format("1Re-align completed = %ld,%ld,%f,%f,%f,%f,%ld,%ld\n", lNewXOffset, lNewYOffset, dNewAngle, fabs(dNewAngle * 180.0/ PI),
				 dRealignmentLatestAngle, dNewAngleInDegree2,
				 lLastDieOffsetX, lLastDieOffsetY);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

	if (m_bAlignBinInAlive)		//pllm
	{
		SwitchToBPR();
	}

	m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
	m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, dNewAngle);
	m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
	m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);

	//check the angle of bin table(0 degree or 180 degree) is matched with that of wafer table
	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (pWaferPr != NULL)
	{
		if (pWaferPr->IsWaferRotated180() && !IsBinFrameRotated180() ||
			!pWaferPr->IsWaferRotated180() && IsBinFrameRotated180())
		{
			SetErrorMessage("Orientation not match while realigning bin block!");
			HmiMessage_Red_Yellow("Orientation not match!");
			return IDS_BT_DIR_NOT_MATCH_WAFER;
		}
	}

	CMS896AStn::m_bRealignFrameDone = TRUE;
	(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= TRUE;
	return (TRUE);
}


// pass in and out in um unit
// convert lXPosn and lYPosn using realign result - (long)siColletOffsetX - (long)siColletOffsetY
LONG CBinTable::ConvertBinTablePosn(LONG &lX, LONG &lY, ULONG ulBlkInUse, DOUBLE dAngle, 
									LONG lXOffset, LONG lYOffset, BOOL bLog)
{
	LONG lOrgX, lOrgY;
	DOUBLE fX, fY, dDistX, dDistY;

	LONG l1stDieIndex = 1;
/*
	//v4.35T4
	if ( m_oBinBlkMain.GrabLSBondPattern() )
	{
		//** e.g. LS Bond sequence with TLH path, DiePerRow = 10
		//
		//     1           
		// 11 10  9  8  7  6  5  4  3  2
		//    12 13 14 15 16 17 18 19 20
		//    29 28 27 26 25 24 23 22 21
		//    31 32 33 .......
		//
		
		ULONG ulNextIndex		= 0;
		ULONG ulNoOfSortedDie	= m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
		ULONG ulDiePerRow		= m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 

		if ( ulNoOfSortedDie == 1 )
		{
			//Use die #1 only
			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, l1stDieIndex, ulNextIndex))
				l1stDieIndex = ulNextIndex;
		}
		else
		{
			//Use left-most die on second row
			if (ulNoOfSortedDie <= ulDiePerRow+1)
				l1stDieIndex = ulDiePerRow + (ulNoOfSortedDie - 1);
			else
				l1stDieIndex = ulDiePerRow * 2;
		}

		ULONG ulLSIndex = 0;
		m_oBinBlkMain.RecalculateLSBondPatternIndex(ulBlkInUse, ulNoOfSortedDie, ulLSIndex);
	}
*/

	m_oBinBlkMain.StepMove(ulBlkInUse, l1stDieIndex, lOrgX, lOrgY);

	dDistX = (DOUBLE)(lX - lOrgX);
	dDistY = (DOUBLE)(lY - lOrgY);

	fX = dDistX * cos(dAngle) - dDistY * sin(dAngle) + lXOffset;
	fY = dDistY * cos(dAngle) + dDistX * sin(dAngle) + lYOffset;

	lX = (LONG)fX + lOrgX;
	lY = (LONG)fY + lOrgY;

	return (TRUE);
}

LONG CBinTable::ConvertBinTableDPosn(DOUBLE &dX, DOUBLE &dY, ULONG ulBlkInUse, DOUBLE dAngle, 
									LONG lXOffset, LONG lYOffset, BOOL bLog)
{
	DOUBLE dOrgX, dOrgY;
	LONG l1stDieIndex = 1;

	//the 1st index unit is the original of BT as rotation transformation becasue 
	//the translate matrix is at 1st index unit
	m_oBinBlkMain.StepDMove(ulBlkInUse, l1stDieIndex, dOrgX, dOrgY);

	Rotate(dOrgX, dOrgY, dAngle, dX, dY);

	dX += lXOffset;
	dY += lYOffset;
	return TRUE;
}

//For Um Unit
VOID CBinTable::Rotate(const DOUBLE dOrgX, const DOUBLE dOrgY, DOUBLE dAngle, DOUBLE &dX, DOUBLE &dY)
{
	DOUBLE fX, fY, dDistX, dDistY;

	dDistX = dX - dOrgX;
	dDistY = dY - dOrgY;

	double dDegree = dAngle * 180 / PI;
	INT nPos = _round(dDegree / m_dThetaRes);
	if (nPos == 0)
	{
		dAngle = 0;
	}

	fX = dDistX * cos(dAngle) - dDistY * sin(dAngle);
	fY = dDistY * cos(dAngle) + dDistX * sin(dAngle);

	dX = fX + dOrgX;
	dY = fY + dOrgY;
}

//for Encoder'count
VOID CBinTable::Rotate(const LONG lOrgX, const LONG lOrgY, DOUBLE dAngle, LONG &lX, LONG &lY)
{
	DOUBLE fX, fY, dDistX, dDistY;

	dDistX = lX - lOrgX;
	dDistY = lY - lOrgY;

	double dDegree = dAngle * 180 / PI;
	INT nPos = _round(dDegree / m_dThetaRes);
	if (nPos == 0)
	{
		dAngle = 0;
	}

	fX = dDistX * cos(dAngle) - dDistY * sin(dAngle);
	fY = dDistY * cos(dAngle) + dDistX * sin(dAngle);

	lX = _round(fX + lOrgX);
	lY = _round(fY + lOrgY);
}

//================================================================
// Realign2PtBinBlock()
//   Created-By  : Andrew Ng
//   Date        : 9/7/2007 4:56:24 PM
//   Description : 
//   Remarks     : 
//================================================================
LONG CBinTable::Realign2PtBinBlock(ULONG ulBlkInUse, BOOL bAuto, BOOL bAutoTeach)
{
	BOOL bOldAligned, bUsePR, bManualLocate;
	BOOL bRealignFailed = FALSE;
	CString str;
	CString szTitle, szContent;	
	LONG lX, lY, lDiePitchX, lDiePitchY;
	ULONG ulNoOfSortedDie, ulDiePerRow, ulDiePerCol, ulDieIndex, ulFirstDieIndex;
	LONG lDieFound, lDieFound1, lDieFound2;
	LONG DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y;
	DOUBLE  dNewAngle = 0, dFOVSize;
	LONG lNewXOffset, lNewYOffset;
	CString szLog;

	CMS896AStn::m_bRealignFrameDone = FALSE;

	ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 
	ulDiePerCol		= m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkInUse); 
	lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));
	bOldAligned		= m_oBinBlkMain.GrabNVIsAligned(ulBlkInUse);

	szTitle.LoadString(HMB_BT_REALIGN_BIN_BLOCK);			

	if (ulNoOfSortedDie == 0)
	{
		if (bAuto == FALSE && !IsBurnIn())
		{
			str.Format("%d", ulBlkInUse);
			SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_ON_BLOCK, str);
		}

		CMS896AStn::m_bRealignFrameDone = TRUE;
		return TRUE;
	}


	//check FOV size for this binblk
	dFOVSize = GetBPRFOVSize(ulBlkInUse);
	BOOL bLargeDie = IsLargeDieSize();		//v2.67

	szLog.Format("2-point Re-align start [%3.2f]", dFOVSize);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
	
	m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);

	//Check Postbond pr status
	bUsePR = CheckBPRStatus();

	// change camera to bond side
	SwitchToPbBPR();

	// Display Search window	//v2.58
	BOOL bLF = FALSE;
	if (dFOVSize >= BPR_LF_SIZE)
	{
		bLF = TRUE;
		bLargeDie = FALSE;		//v2.67	//prefer to use LA even if Large-Die
	}

	// off bin table joy stick
	SetJoystickOn(FALSE);


	//******************************************************//
	// find first die of the grade
	// move bin table to first die of this block
	// get the the logically position of first die
	ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
	m_oBinBlkMain.StepMove(ulBlkInUse, ulFirstDieIndex, lX, lY);
	DieALogical_x = ConvertFileUnitToXEncoderValue(lX);
	DieALogical_y = ConvertFileUnitToYEncoderValue(lY);
	BT_MoveTo(DieALogical_x, DieALogical_y);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	//v2.58
	Sleep(100);					//v2.56

	if (bUsePR == TRUE)
	{
		if (bAutoTeach == TRUE)
		{
			szLog.Format("Search 1st die start at blk #%d,largedie,%d", ulBlkInUse,bLargeDie);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
			
			if (bLargeDie)	//Large die; use single mode	//v2.67
			{
				lDieFound = SearchGoodDie();
			}
			else			//Small die; use LA or move-search mode
			{

				/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("2pt Search first die", "a+"); /******/

				if ((lDieFound = SearchFirstGoodDie(ulBlkInUse)) == TRUE)
				{

					/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Finding die", "a+"); /******/
					
					switch (m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
					{
						default:
							lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize);
							break;

						case BT_TRH_PATH:
							lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize);
							break;

						case BT_TLV_PATH:
							lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize);
							break;

						case BT_TRV_PATH:
							lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize);
							break;
					}
				}
			}
		}
		else
		{
			lDieFound = FALSE;
		}

		if (lDieFound == FALSE)
		{
			if (m_bAlignBinInAlive == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UL_DIE);
				SwitchToBPR();
				return FALSE;
			}
			
			bManualLocate = 1;
		}
		else
		{

			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("1st die is found", "a+"); /******/
			bManualLocate = 0;
		}
	}
	else
	{
		bManualLocate = 1;
	}

	if (bManualLocate == 1)
	{
		//Ask user to locate die pos, first die in first row
		SetJoystickOn(TRUE);

		if (!IsBurnIn())
		{
			szContent.LoadString(HMB_BT_REALIGN_STEP1);			
			//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szContent);		//v4.43T10
			
			//if (HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			if (HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			{
				SwitchToBPR();
				if (bAuto == TRUE)
				{
					return FALSE;
				}
				else
				{
					CMS896AStn::m_bRealignFrameDone = TRUE;
					return (TRUE);
				}
			}
		}

		// if operation confirm the location
		SetJoystickOn(FALSE);

		// to align the die by pr
		if (bUsePR == TRUE)
		{
			SearchGoodDie();
		}
	}

	/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-confirm TL die(2pt)", "a+"); /******/
	
	//Get 1st die physical position
	if (bLF == FALSE)
	{
		SearchGoodDie();
	}
	Sleep(m_lAlignBinDelay);
	GetXYEncoderValue(DieAPhysical_x, DieAPhysical_y);


	// convert die offset in um unit
	lNewXOffset = -ConvertXEncoderValueToFileUnit(DieAPhysical_x - DieALogical_x);
	lNewYOffset = -ConvertYEncoderValueToFileUnit(DieAPhysical_y - DieALogical_y);

	// only one die bonded, no need to find last
	if (ulNoOfSortedDie == 1)
	{
		m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
		m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);
		m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, 0);
		m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
		if (bAuto == FALSE && !IsBurnIn())
		{
			SetAlert_Red_Yellow(IDS_BT_ONLY_1_DIE_ON_BLOCK);
		}

		SwitchToBPR();
		CMS896AStn::m_bRealignFrameDone = TRUE;
		return (TRUE);
	}



	//******************************************************//
	// check last die here
	LONG l3rdDieX = 0, l3rdDieY = 0;
	LONG l3rdDiePrX = 0, l3rdDiePrY = 0;

	if (bUsePR == FALSE)
	{
		// just move bin table to last die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie;
		m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
		ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
	}
	else
	{
		SetJoystickOn(FALSE);

		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm last die", "a+"); /******/
		
		// move bin table to last die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie;
		m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
		LONG lX1 = lX;	//v2.58
		ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
		l3rdDieX = ConvertFileUnitToXEncoderValue(lX);
		l3rdDieY = ConvertFileUnitToYEncoderValue(lY);


		// Calculate bin table to last+1 die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie + 1;
		m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
		LONG lX2 = lX;
		ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);


		// Align Last-die and return its XY placement
		//lDieFound1 = SearchGoodDie();	
		BOOL bPlacementOk = TRUE;
		DOUBLE dX = 0, dY = 0;
		BOOL bDXFail=FALSE, bDYFail=FALSE;
		Sleep(100);					//v2.56
		lDieFound1 = Search3rdGoodDie(bPlacementOk, dX, dY, bDXFail, bDYFail);
		if (lDieFound1) 
		{
			lDieFound1 = SearchDieInBottomRight(ulBlkInUse, dFOVSize);
		}
		GetXYEncoderValue(l3rdDiePrX, l3rdDiePrY);


		// Look-ahead other PR sub-windows at Last-Die position
		CString szLfError = "Warning: ";
		LONG lLfDieOK = TRUE;	//Always disable lookahead function on 3rd die


		// move bin table to last+1 die of this block, use realign result
		BT_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(100);					
		lDieFound2 = SearchGoodDie();

		szLog.Format("Search next index %d", lDieFound2);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

		// Determine last-die result
		if (lDieFound1 == TRUE && lDieFound2 != TRUE)
		{
			if ((bPlacementOk == FALSE) && (IsBurnIn() == FALSE))
			{
				CString szMsg;
				szMsg.Format("dX = %.2f mil, dY = %.2f mil.", dX, dY);
				if (SetAlert_Msg_Red_Back(IDS_BT_REALIGN_LAST_DIE_SHIFT, szMsg, "Continue", "Stop") != 1)
				{
					SwitchToBPR();
					//SetAlarmLamp_Yellow();
					return FALSE;
				}
			}


			LONG PosX, PosY;
			GetXYEncoderValue(PosX, PosY);
			m_lX = PosX;
			m_lY = PosY;

			//v4.44A5
			CString szMyLog;
			szMyLog.Format("RealignFrame 3: m_lX = %ld, m_lY = %ld", m_lX, m_lY);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szMyLog);

		}
		else if (lDieFound1 != TRUE && lDieFound2 != TRUE)		//v2.56
		{
			//if ( lLfDieOK == TRUE )
			//{
			//Found die on their previous pos
			CString szMsg = " ";
			if (SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE_2, szMsg, "Yes", "No") != 1)
			{
				//SetAlarmLamp_Yellow();
				SwitchToBPR();
				return FALSE;
			}
/*
			}
			else
			{
				//v2.63		//andrew: will change to ALARM type later!!
				CString szMsg = " ";
				if ( SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Continue", "Stop") != 1 )
				{
					SetAlarmLamp_Yellow();
					SwitchToBPR();
					return FALSE;
				}
			}
*/
		}
		else
		{
			bRealignFailed = TRUE;
		}


		if (bRealignFailed == TRUE)
		{
			m_oBinBlkMain.SetIsAligned(ulBlkInUse, bOldAligned);

			if (!IsBurnIn())
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE);
			}

			szLog.Format("Re-align failed\n", lDieFound2);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		
			SwitchToBPR();
			return FALSE;
		}


		//Calculate dNewAngle
		DOUBLE dA2 = pow((DieAPhysical_x - l3rdDieX), 2.0) + pow((DieAPhysical_y - l3rdDieY), 2.0);
		DOUBLE dB2 = pow((DieAPhysical_x - l3rdDiePrX), 2.0) + pow((DieAPhysical_y - l3rdDiePrY), 2.0);
		DOUBLE dC2 = pow((l3rdDieX - l3rdDiePrX), 2.0) + pow((l3rdDieY - l3rdDiePrY), 2.0);
		DOUBLE dDivident = 2.00 * sqrt(dA2) * sqrt(dB2);

		if (dDivident != 0)
		{
			//dNewAngle = acos((dA2 + dB2 - dC2) / dDivident); temporary by leo
		}
		else
		{
			//dNewAngle = 0.00; temporary by leo
		}
	}


	szLog.Format("2Re-align completed = %d,%d,%f\n", lNewXOffset, lNewYOffset, dNewAngle);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

	SwitchToBPR();
	m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
	m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, dNewAngle);
	m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
	m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);

	m_dFrameAlignAngleForTesting = m_oBinBlkMain.GrabNVRotateAngleX(1);	//v4.57A9
	CMS896AStn::m_bRealignFrameDone = TRUE;
	return (TRUE);
}


LONG CBinTable::Realign1PtBinBlock(ULONG ulBlkInUse, BOOL bAuto, BOOL bAutoTeach)
{
	BOOL bOldAligned, bUsePR;
	BOOL bRealignFailed = FALSE;
	CString str;
	CString szTitle, szContent;	
	DOUBLE dStepX=0, dStepY=0;
	LONG lX = 0, lY = 0, lDiePitchX = 0, lDiePitchY = 0;
	ULONG ulNoOfSortedDie, ulDiePerRow, ulDiePerCol, ulDieIndex, ulFirstDieIndex = 0;
	LONG lDieFound1, lDieFound2;
	LONG DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y;
	DOUBLE  dNewAngle = 0, dFOVSize;
	LONG lNewXOffset = 0, lNewYOffset = 0;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szLog;

	CMS896AStn::m_bRealignFrameDone = FALSE;

	ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 
	ulDiePerCol		= m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkInUse); 
	lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));
	bOldAligned		= m_oBinBlkMain.GrabNVIsAligned(ulBlkInUse);

	szTitle.LoadString(HMB_BT_REALIGN_BIN_BLOCK);			

	if (ulNoOfSortedDie == 0)
	{
		SetJoystickOn(FALSE);	//v3.71T5

		LONG lStepX = 0, lStepY = 0;
		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
		GetDieLogicalEncoderValue(ulBlkInUse, ulFirstDieIndex, lStepX, lStepY);
		BT_MoveTo(lStepX, lStepY, FALSE);

		if (bAuto == FALSE && !IsBurnIn())
		{
			str.Format("%d", ulBlkInUse);
			SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_ON_BLOCK, str);
		}

		szLog.Format("1-point Re-align: blk%d, no of sorted die = 0\n", ulBlkInUse);		
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	
		m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
		CMS896AStn::m_bRealignFrameDone = TRUE;
		return TRUE;
	}

	//check FOV size for this binblk
	dFOVSize = GetBPRFOVSize(ulBlkInUse);
	BOOL bLargeDie = IsLargeDieSize();
	
	szLog.Format("1-point Re-align start FOV=[%3.2f]; Count=%d; Alive=%d", dFOVSize, ulNoOfSortedDie, m_bAlignBinInAlive);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 

	m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);

	//Check Postbond pr status
	bUsePR = CheckBPRStatus();

	// Display Search window
	BOOL bLF = FALSE;
	if (dFOVSize >= BPR_LF_SIZE)
	{
		bLF = TRUE;
		bLargeDie = FALSE;		//v2.67	//prefer to use LA even if Large-Die
	}

	// off bin table joy stick
	if (m_bAlignBinInAlive)			//xyz
	{
		SwitchToBPR();
	}
	SetJoystickOn(FALSE);


	//******************************************************//
	//Move table to last die directly
	LONG l3rdDieX = 0, l3rdDieY = 0;

	if (bUsePR == FALSE)
	{
		// just move bin table to last die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie;

		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		ULONG ulLastIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);		
		for (INT i = 1; i < (INT) ulLastIndex; i++)		
		{
			for (INT j = 0; j < (INT) m_ulHoleDieNum; j++)									
			{
				if	(m_oBinBlkMain.GrabRandomHoleDieIndex(ulBlkInUse, j) == i)
				{
					ulDieIndex++;
				}
			}
		}

		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		szLog.Format("Directly Confirm last die (no PR), INDEX = #%d", ulDieIndex);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

		//v4.01
		if (m_oBinBlkMain.GrabLSBondPattern())
		{
			ULONG ulNextIndex = 0;
			ULONG ulOriginalIndex = ulDieIndex;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulDieIndex, ulNextIndex))
			{
				ulDieIndex = ulNextIndex;
			}
			
			szLog.Format("Last die uses LS pattern; orig-INDEX = %#d; new-INDEX = #%d", ulOriginalIndex, ulDieIndex);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}

		if (pApp->GetCustomerName() == "CyOptics"	||
			pApp->GetCustomerName() == "Inari"		||		//v4.51A24
			pApp->GetCustomerName() == "FiberOptics")		//v4.31T8
		{
			ulDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse);
		}

		LONG lStepX = 0, lStepY = 0;
		GetDieLogicalEncoderValue(ulBlkInUse, ulDieIndex, lStepX, lStepY);
		BT_MoveTo(lStepX, lStepY);
	}
	else
	{
		SetJoystickOn(FALSE);

		// move bin table to last die of this block, use realign result
		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		if (pApp->GetCustomerName() == "CyOptics"	||
			pApp->GetCustomerName() == "Inari"		||		//v4.51A24
			pApp->GetCustomerName() == "FiberOptics")		//v4.31T8
		{
			ulDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse);
		}

		//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Directly Confirm last die", "a+");
		szLog.Format("Directly Confirm last die (PR), INDEX = #%d", ulDieIndex);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

		//v4.01
		if (m_oBinBlkMain.GrabLSBondPattern())
		{
			ULONG ulNextIndex = 0;
			ULONG ulOriginalIndex = ulDieIndex;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulDieIndex, ulNextIndex))
			{
				ulDieIndex = ulNextIndex;
			}

			szLog.Format("Last die uses LS pattern; orig-INDEX = #%d; new-INDEX = #%d", ulOriginalIndex, ulDieIndex);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}

		GetDieLogicalEncoderValue(ulBlkInUse, ulDieIndex, DieALogical_x, DieALogical_y);
		BT_MoveTo(DieALogical_x, DieALogical_y);
		
		// Calculate bin table to last+1 die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + 1;
		
		//v4.51A14		//Cree Huizhou
		if (m_oBinBlkMain.TwoDimensionBarcodeFindNextBondIndex(ulBlkInUse, ulDieIndex))
		{
			//LONG lLast1X=0, lLast1Y=0;
			DOUBLE dLast1X=0, dLast1Y=0;
			m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dLast1X, dLast1Y, TRUE);	//Last+1+1(HOLE)

			CString szMsg;
			szMsg.Format("REALIGN-BIN #%d at Last+1 - HOLE detected at INDEX = %ld, New = %ld, Enc (%f, %f)",	//v4.48A21
				ulBlkInUse, ulDieIndex-1, ulDieIndex, dLast1X, dLast1Y);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

			szLog.Format("BLK #%lu LastDie+1 encounter 2D HOLE - New INDEX = %ld", ulBlkInUse, ulDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}

		if (pApp->GetCustomerName() == "CyOptics"	|| 
			pApp->GetCustomerName() == "Inari"		||		//v4.51A24 
			pApp->GetCustomerName() == "FiberOptics")		//v4.31T8
		{
			ulDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse) + 1;
		}

		//v4.01
		if (m_oBinBlkMain.GrabLSBondPattern())
		{
			ULONG ulNextIndex = 0;
			ULONG ulOriginalIndex = ulDieIndex;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulDieIndex, ulNextIndex))
			{
				ulDieIndex = ulNextIndex;
			}

			szLog.Format("NEXT die uses LS pattern; orig-INDEX = #%d; new-INDEX = #%d", ulOriginalIndex, ulDieIndex);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}

		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dStepX, dStepY);	//v4.59A19
		//ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);


		// Align Last-die and return its XY placement
		BOOL bPlacementOk = TRUE;
		DOUBLE dX = 0, dY = 0;
		BOOL bDXFail=FALSE, bDYFail=FALSE;
		Sleep(m_lAlignBinDelay);
		lDieFound1 = Search3rdGoodDie(bPlacementOk, dX, dY, bDXFail, bDYFail);	

		if (m_bTestRealign || IsBurnIn())		//v3.70T4	//v3.94		//Test-Mode only
		{
			lDieFound1 = TRUE;
			bPlacementOk = TRUE;
			dX = 0;
			dY = 0;
		}

		//Sleep(m_lAlignBinDelay);		//v3.80
		GetXYEncoderValue(DieAPhysical_x, DieAPhysical_y);

		// convert die offset in um unit
		lNewXOffset = -ConvertXEncoderValueToFileUnit(DieAPhysical_x - DieALogical_x);
		lNewYOffset = -ConvertYEncoderValueToFileUnit(DieAPhysical_y - DieALogical_y);

		//xyz
		if (!IsMS90())		//v4.59A10
		{
			if (!m_b1ptRealignComp)		//v3.86	//Add Offset compensation option
			{
				lNewXOffset = 0;
				lNewYOffset = 0;
				dNewAngle = 0.00;
			}
		}

		LONG lLfDieOK = TRUE;	//Always disable lookahead function on 3rd die

		//v4.59A20	//fix bug after precision changed to 0.1um
		ConvertBinTableDPosn(dStepX, dStepY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);

		// move bin table to last+1 die of this block, use realign result
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dStepX), ConvertFileUnitToYEncoderValue(dStepY));
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(m_lAlignBinDelay);	
		lDieFound2 = SearchGoodDie();

		if (m_bTestRealign || IsBurnIn())		//v3.70T4	//v3.94		//Test-Mode only
		{
			lDieFound2 = FALSE;
		}

		szLog.Format("Last-Die status = %d; NEXT-die status = %d", lDieFound1, lDieFound2);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

		// Determine last-die result
		if (lDieFound1 == TRUE && lDieFound2 != TRUE)
		{
			CString szMsg;
			if (!bPlacementOk && !IsBurnIn() && m_b1ptRealignComp && m_bAlignBinInAlive)		//v3.62		//v3.86	
			{
				szLog.Format("dX = %.2f steps, dY = %.2f steps.", dX, dY);
				/******/ 
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

				LONG lPosX1 = 0, lPosX2 = 0;
				LONG lPosY1 = 0, lPosY2 = 0;
				GetXYEncoderValue(lPosX1, lPosY1);

				SwitchToBPR();
				DisplayBondPrDieSizeWindow(TRUE);

				//Ask user to die pos
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_X, "BT_JoySlow", &m_stBTAxis_X);
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_Y, "BT_JoySlow", &m_stBTAxis_Y);
				SetJoystickOn(TRUE);

				szMsg = _T("last-die placement fail; please specify last-die position with joystick, then press CONTINUE.");		
				if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
				{
					SwitchToBPR();
					//SetAlarmLamp_Yellow();
					return FALSE;
				}

				SetJoystickOn(FALSE);
				Sleep(100);

				GetXYEncoderValue(lPosX2, lPosY2);

				lNewXOffset = lNewXOffset - ConvertXEncoderValueToFileUnit(lPosX2 - lPosX1);
				lNewYOffset = lNewYOffset - ConvertYEncoderValueToFileUnit(lPosY2 - lPosY1);

			}
			else if ((bPlacementOk == FALSE) && (IsBurnIn() == FALSE))
			{
				szMsg.Format("Placement fails - dX = %.2f steps, dY = %.2f steps.\n", dX, dY);
				/******/ 
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+"); /******/ 

				SwitchToBPR();
				SetAlert_Red_Yellow(IDS_BT_REALIGN_LAST_DIE_SHIFT);
				SwitchToBPR();
				return FALSE;
			}

			LONG PosX, PosY;
			GetXYEncoderValue(PosX, PosY);
			m_lX = PosX;
			m_lY = PosY;

			//v4.44A5
			CString szMyLog;
			szMyLog.Format("RealignFrame 4: m_lX = %ld, m_lY = %ld", m_lX, m_lY);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szMyLog);

		}
		else if (lDieFound1 != TRUE && lDieFound2 != TRUE)
		{
			if (IsBurnIn())
			{
				bRealignFailed = FALSE;
			}
			else
			{
				//pllm
				CString szMsg = "  ";
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && !m_bAlignBinInAlive)	//xyz
				{
					szMsg = "Re-align fails due to last-die (PLLM REBEL)\n";
					/******/ 
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+"); /******/ 

					SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
					SwitchToBPR();
					return FALSE;
				}
				else
				{
					CString szMsg = " ";
					SwitchToBPR();
					DisplayBondPrDieSizeWindow(TRUE);

					szMsg = "Re-align fails due to last-die\n";
					/******/ 
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+"); /******/ 

					if (pApp->IsUsingEngineerMode())
					{
						if (SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Continue", "Stop") != 1)
						{
							//SetAlarmLamp_Yellow();
							return FALSE;
						}
					}
					else
					{
						szMsg = _T("Last die is not found; operation is aborted");		
						SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
						return FALSE;
					}
				}
			}
		}
		else
		{
			bRealignFailed = TRUE;
		}

		if (bRealignFailed == TRUE)
		{
			SwitchToBPR();
			if (!IsBurnIn())
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE);
			}

			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-align failed\n", "a+"); /******/
			return FALSE;
		}
	}

	//v4.31T10
	if (bAuto && 
		(pApp->GetCustomerName() == "CyOptics"	|| 
		 pApp->GetCustomerName() == "Inari"		||
		 pApp->GetCustomerName() == "FiberOptics") )	//v4.52A3
	{
		ULONG ulNextDieIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse) + 1;
		ULONG ulOrigIndex = ulNextDieIndex;
		if (m_oBinBlkMain.CheckIfEmptyRow(ulBlkInUse))
		{
			m_oBinBlkMain.CheckIfNeedToLeaveEmptyRow(ulBlkInUse, ulNextDieIndex, TRUE);
			m_oBinBlkMain.StepDMove(ulBlkInUse, ulNextDieIndex, dStepX, dStepY);	//v4.59A19
			ConvertBinTableDPosn(dStepX, dStepY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
			BT_MoveTo(ConvertFileUnitToXEncoderValue(dStepX), ConvertFileUnitToYEncoderValue(dStepY));

			//Used in OpMoveTable() in CycleState
			m_lX = ConvertFileUnitToXEncoderValue(dStepX);
			m_lY = ConvertFileUnitToXEncoderValue(dStepY);
			Sleep(100);

			//v4.44A5
			CString szMyLog;
			szMyLog.Format("RealignFrame 2: m_lX = %ld, m_lY = %ld", m_lX, m_lY);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szMyLog);

			szLog.Format("Re-align (CyOptics) - EmptyRow triggered - CurrIndex = %lu; NewIndex = %lu; Encoder(%ld, %ld)", 
						 ulOrigIndex, ulNextDieIndex, ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
		}
	}

	if (!IsMS90())		//v4.59A10
	{
		if (!m_b1ptRealignComp)		//v3.86		//Added compensation option for 1pt realignment
		{
			lNewXOffset = 0;
			lNewYOffset = 0;
			dNewAngle = 0.00;
		}
	}
	DOUBLE dNewAngleInDegree1 = fabs(dNewAngle * 180.0 / PI);
	szLog.Format("3Re-align completed: Offset(XYT) = %d, %d, %f, %f\n", lNewXOffset, lNewYOffset, dNewAngle, dNewAngleInDegree1);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	//SwitchToBPR();
	m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
	m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, dNewAngle);
	m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
	m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);

	CMS896AStn::m_bRealignFrameDone = TRUE;
	return (TRUE);
}


VOID CBinTable::CreteBondedDieInfoInBinMap(ULONG ulBlkInUse) 
{
	ULONG ulNoOfSortedDie	= m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	LONG ulTotalDieIndex	= ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
	ulTotalDieIndex			+= m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
	ulTotalDieIndex			+= m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);
	m_oBinBlkMain.CreteBondedDieInBinMap(ulBlkInUse, ulTotalDieIndex);
}

//v4.59A41
BOOL CBinTable::AdjustGlobalThetaUsingLastRowCol(const ULONG ulBlkInUse, const ULONG ulDieIndex,	 
												 const DOUBLE dCheckRealignmentAngle, const BOOL bSecondRealign,
												 DOUBLE& dNewAngle, DOUBLE& dRealignmentNewAngle, 
												 LONG& lNewXOffset, LONG& lNewYOffset)	
{
	DOUBLE dNewLastRowColAngle1 = 0.0;
	DOUBLE dNewLastRowColAngle2 = 0.0;
	LONG lXOffset = lNewXOffset;
	LONG lYOffset = lNewYOffset;
	DOUBLE dNewAngleInDegree = 0.0;
	BOOL b1stCompensateGobalAngle = TRUE;
	DOUBLE dNewCheckRealignmentAngle = dCheckRealignmentAngle;
	CString szLog;

	ULONG ulNoOfSortedDie	= m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ULONG ulDiePerRow		= m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 

	CreteBondedDieInfoInBinMap(ulBlkInUse);

	dRealignmentNewAngle = dNewAngle;

	if (FindLastRowColumnFrameAngle(FALSE, b1stCompensateGobalAngle, ulBlkInUse, ulDieIndex, 
					dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle1, dNewCheckRealignmentAngle))
	{

//		Sleep(500);
//		dNewAngle += -1 * dNewLastRowColAngle1;
		dNewAngle = 0; //Assume
		b1stCompensateGobalAngle = !b1stCompensateGobalAngle;
		if (FindLastRowColumnFrameAngle(TRUE, b1stCompensateGobalAngle, ulBlkInUse, ulDieIndex, 
										dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle2, dNewCheckRealignmentAngle))
//		dNewLastRowColAngle2 = dNewLastRowColAngle1;
		{

			szLog.Format("FindLastRowColumnFrameAngle Done (in radian): OrigAngle = %.6f, 1stAngle = %.6f, 2ndAngle = %.6f", 
						dNewAngle, dNewLastRowColAngle1, dNewLastRowColAngle2);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

			//dNewAngle = -1* (dNewLastRowColAngle1 + dNewLastRowColAngle2)/2;//leo 20171105 need the Angle2 only
			//dNewAngle += -1 * (dNewLastRowColAngle2);

			dRealignmentNewAngle = -1 * (dNewLastRowColAngle2);

			szLog.Format("Final FRAME Angle = %.6f (rad) or %.6f (degree)", 
						dNewAngle, dNewAngle * 180.0 / PI);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

			szLog.Format("Final diff FRAME Angle = %.6f (rad) or %.6f (degree)", 
						dRealignmentNewAngle, dRealignmentNewAngle * 180.0 / PI);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 


			szLog.Format("Final FRAME XY Offset: Orig (%ld, %ld); New (%ld, %ld)", 
						lNewXOffset, lNewYOffset, lXOffset, lYOffset);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

			lNewXOffset = lXOffset;
			lNewYOffset = lYOffset;

			LONG lLoopFindLastColAngle = 0;
			dNewAngleInDegree = fabs(dRealignmentNewAngle * 180.0 /PI);
/*
			//for first align, if the rotate angle exceed the limit, machine need do alignment becasue
			//the translate matrix is changed.
			if ((fabs(dNewAngle) > 0.000001) && (dNewAngleInDegree > dNewCheckRealignmentAngle))
			{
				return FALSE; //do realign again
			}
*/
			while ((dNewAngleInDegree > dNewCheckRealignmentAngle) && (lLoopFindLastColAngle <3))	//Loop twice for Better Alignment Angle //leo lam check with the angle according to the length of the distance
			{
				lLoopFindLastColAngle++;
				b1stCompensateGobalAngle = !b1stCompensateGobalAngle;
				if (FindLastRowColumnFrameAngle(TRUE, b1stCompensateGobalAngle, ulBlkInUse, ulDieIndex, 
								dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle1, dNewCheckRealignmentAngle))
				{
					//dNewAngle += -1 * dNewLastRowColAngle1;
					//if (FindLastRowColumnFrameAngle(ulBlkInUse, ulDieIndex, 
					//				dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle2))
					dNewLastRowColAngle2 = dNewLastRowColAngle1;
					{
						szLog.Format("FindLastRowColumnFrameAngle Done (in radian): OrigAngle = %.6f, 1stAngle = %.6f, 2ndAngle = %.6f", 
									dNewAngle, dNewLastRowColAngle1, dNewLastRowColAngle2);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

						dRealignmentNewAngle = -1 * (dNewLastRowColAngle2);

						szLog.Format("Final FRAME Angle = %.6f (rad) or %.6f (degree)", 
									dNewAngle, dNewAngle * 180.0 / PI);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

						szLog.Format("Final Diff FRAME Angle = %.6f (rad) or %.6f (degree)", 
									dRealignmentNewAngle, dRealignmentNewAngle * 180.0 / PI);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

						szLog.Format("Final FRAME XY Offset: Orig (%ld, %ld); New (%ld, %ld)", 
									lNewXOffset, lNewYOffset, lXOffset, lYOffset);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

						lNewXOffset = lXOffset;
						lNewYOffset = lYOffset;
						dNewAngleInDegree = fabs(dRealignmentNewAngle * 180.0 /PI);
						
					}
				}
			}
		}
//		else
//		{
//			//do realign again
//			return FALSE;
//			//No handling needed at this moment
//		}
	}
	else	//v4.59A33	//LeoLam 20171109
	{
		dNewAngleInDegree = dNewLastRowColAngle1; //if enter the rotate angle function return false, no update before on the angle even rotated good
		
		DOUBLE dOldAngle = dNewAngle;
		dNewAngle = -1 * (dNewLastRowColAngle1);

		szLog.Format("Realign(%d): FindLastRowColumnFrameAngle FAIL1 but Final FRAME Angle updated = %.6f (rad) or %.6f (degree), orig = %.6f (rad)", 
					bSecondRealign ? 1 : 2, dNewAngle, dNewAngle * 180.0 / PI, dOldAngle);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		return FALSE;
	}

	if (dNewAngleInDegree > dNewCheckRealignmentAngle && !CMS896AApp::m_bEnableSubBin && (ulNoOfSortedDie >= ulDiePerRow))	//0.01 degree
	{
		CString szTitle;
		szTitle = "Global Angle Alignment";
		szLog.Format("Realign(%d): Global Angle exceeds (%.6f) degree (%.6f); operation is aborted", bSecondRealign ? 1 : 2, dNewCheckRealignmentAngle, dNewAngleInDegree);		
		if (bSecondRealign)
		{
			HmiMessageEx_Red_Back(szLog, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		}
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "Confirm Next Die pos aborted by user", "a+");
		return FALSE;
	}

	szLog.Format("Global Angle (%.6f) < degree (%.6f)", dNewCheckRealignmentAngle, dNewAngleInDegree);		
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "Confirm Angle OK", "a+");
	
	return TRUE;
}


LONG CBinTable::MovePRSearchIndexPosn(ULONG ulBlkInUse, ULONG ulIndex, 
									  DOUBLE dNewAngle, LONG lXOffset, LONG lYOffset, BOOL bDisplayBondPrSearchWindow,
									  const LONG lRow, const LONG lCol, 
									  LONG &lIndexPhysical_x, LONG &lIndexPhysical_y)
{
	DOUBLE dIndexUmX = 0, dIndexUmY = 0;
	LONG lIndexFlag = 0;

	/////////////////////////////////////////
	// Move & Find INDEX1 - 123 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex, dIndexUmX, dIndexUmY, FALSE);
	ConvertBinTableDPosn(dIndexUmX, dIndexUmY, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX), ConvertFileUnitToYEncoderValue(dIndexUmY));
	if (bDisplayBondPrSearchWindow)
	{
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
	}
	Sleep(m_lAlignBinDelay);	
	
	CString szLog;
	if (SearchGoodDie() == FALSE)
	{
		szLog.Format("FindLastRowColAngle(Move PR Search) FAIL: no die found at INDEX %d-%d position", lRow, lCol);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		lIndexFlag = 1;
	}
		
	lIndexPhysical_x = 0;
	lIndexPhysical_y = 0;
	GetXYEncoderValue(lIndexPhysical_x, lIndexPhysical_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX %d-%d Enc (%ld, %ld)", lRow, lCol, lIndexPhysical_x, lIndexPhysical_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	return lIndexFlag;
}


LONG CBinTable::FindCurrentDieCenter(const LONG lX, const LONG lY, const LONG lRow, const LONG lCol, const BOOL bMovetoDieCenter, const BOOL bRetry,
									 LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4])
{
	CString szLog;
	szLog.Format("FindCurrentDieCenter-BT_MoveTo");
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	BT_MoveTo(lX, lY);
	szLog.Format("FindCurrentDieCenter-DisplayBondPrSearchWindow");
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
	Sleep(m_lAlignBinDelay);

	szLog.Format("FindCurrentDieCenter-SearchGoodDie = %d", bMovetoDieCenter);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	
	BOOL bSrchResult = TRUE;
	if (bMovetoDieCenter)
	{
		bSrchResult = SearchGoodDie();
	}
	GetXYEncoderValue(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol]);

	LONG lPROffsetX_Count = 0, lPROffsetY_Count = 0;
	if (bSrchResult)
	{
		szLog.Format("FindCurrentDieCenter-SearchGoodDieXYinFOV");
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		bPRResult[lCol] = SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, BT_PR_CENTER);
		if (bPRResult[lCol])
		{
			alIndexPhysical_x[lCol] += lPROffsetX_Count;
			alIndexPhysical_y[lCol] += lPROffsetY_Count;
		}
		bSrchResult = bPRResult[lCol];
	}
	szLog.Format("FindCurrentDieCenter-SearchGoodDieXYinFOV finished");
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	if (!bSrchResult)
	{
		BOOL bFailure = TRUE;
		if (bRetry)
		{
			bPRResult[lCol] = SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, BT_PR_CENTER, 1, 5);
			if (bPRResult[lCol])
			{
				alIndexPhysical_x[lCol] += lPROffsetX_Count;
				alIndexPhysical_y[lCol] += lPROffsetY_Count;
				BT_MoveTo(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol]);
				DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
				Sleep(m_lAlignBinDelay);	
				bPRResult[lCol] = SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, BT_PR_CENTER);
				if (bPRResult[lCol])
				{
					alIndexPhysical_x[lCol] += lPROffsetX_Count;
					alIndexPhysical_y[lCol] += lPROffsetY_Count;
					bFailure = FALSE;
				}
			}
		}

		if (bFailure)
		{
			szLog.Format("FindLastRowColAngle(Current Die Center) FAIL: no die found at INDEX %d-%d position", lRow, lCol + 1);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			return lCol + 1;
		}
	}
	return 0;
}


ULONG CBinTable::GetCurBinPath(const ULONG ulWalkPath, const ULONG ulIndex, const ULONG ulDividend)
{
	ULONG ulCurBinMapPath = BT_TLH_PATH;
	if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ||  (ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH))
	{
		ulCurBinMapPath = m_oBinBlkMain.GetCurRowWalkPath(ulWalkPath, _round((double)ulIndex / ulDividend));
	}
	else if ((ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_TRV_PATH) || (ulWalkPath == BT_BRV_PATH))
	{
		ulCurBinMapPath = m_oBinBlkMain.GetCurColumnWalkPath(ulWalkPath, _round((double)ulIndex / ulDividend));
	}

	return ulCurBinMapPath;
}


VOID CBinTable::GetLookAheadYDirection(const ULONG ulWalkPath, const BOOL bRotate180, const ULONG ulRow, const ULONG ulCol, 
									   const LONG lDiePitchX, const LONG lDiePitchY, CLookAheadMap acLookAheadDie[9])
{
	//ulWalkPath -- is a origin Path
	switch (ulWalkPath)
	{
		case BT_TLH_PATH:
		case BT_TRH_PATH:
			if (!bRotate180)
			{
				//	  Left/cur/Right
				//     6    5        4
				//     7    8(die)   3
				//     0    1        2
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[0].m_ulRow = ulRow + 1;
				acLookAheadDie[0].m_ulCol = ulCol - 1;
				acLookAheadDie[0].m_lPitchX = -lDiePitchX;
				acLookAheadDie[0].m_lPitchY = lDiePitchY;

				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[1].m_ulRow = ulRow + 1;
				acLookAheadDie[1].m_ulCol = ulCol;
				acLookAheadDie[1].m_lPitchX = 0;
				acLookAheadDie[1].m_lPitchY = lDiePitchY;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[2].m_ulRow = ulRow + 1;
				acLookAheadDie[2].m_ulCol = ulCol + 1;
				acLookAheadDie[2].m_lPitchX = lDiePitchX;
				acLookAheadDie[2].m_lPitchY = lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[3].m_ulRow = ulRow;
				acLookAheadDie[3].m_ulCol = ulCol + 1;
				acLookAheadDie[3].m_lPitchX = lDiePitchX;
				acLookAheadDie[3].m_lPitchY = 0;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[4].m_ulRow = ulRow - 1;
				acLookAheadDie[4].m_ulCol = ulCol + 1;
				acLookAheadDie[4].m_lPitchX = lDiePitchX;
				acLookAheadDie[4].m_lPitchY = -lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[5].m_ulRow = ulRow - 1;
				acLookAheadDie[5].m_ulCol = ulCol;
				acLookAheadDie[5].m_lPitchX = 0;
				acLookAheadDie[5].m_lPitchY = -lDiePitchY;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[6].m_ulRow = ulRow - 1;
				acLookAheadDie[6].m_ulCol = ulCol - 1;
				acLookAheadDie[6].m_lPitchX = -lDiePitchX;
				acLookAheadDie[6].m_lPitchY = -lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[7].m_ulRow = ulRow;
				acLookAheadDie[7].m_ulCol = ulCol - 1;
				acLookAheadDie[7].m_lPitchX = -lDiePitchX;
				acLookAheadDie[7].m_lPitchY = 0;
			}
			else
			{
				//Origin Path is TLH or TRH --map coodinate not rotate 180 degree
				//	  Left/cur/Right
				//     2    1        0
				//     3    8(die)   7
				//     4    5        6
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[0].m_ulRow = ulRow + 1;
				acLookAheadDie[0].m_ulCol = ulCol - 1;
				acLookAheadDie[0].m_lPitchX = lDiePitchX;
				acLookAheadDie[0].m_lPitchY = -lDiePitchY;


				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[1].m_ulRow = ulRow + 1;
				acLookAheadDie[1].m_ulCol = ulCol;
				acLookAheadDie[1].m_lPitchX = 0;
				acLookAheadDie[1].m_lPitchY = -lDiePitchY;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[2].m_ulRow = ulRow + 1;
				acLookAheadDie[2].m_ulCol = ulCol + 1;
				acLookAheadDie[2].m_lPitchX = -lDiePitchX;
				acLookAheadDie[2].m_lPitchY = -lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[3].m_ulRow = ulRow;
				acLookAheadDie[3].m_ulCol = ulCol + 1;
				acLookAheadDie[3].m_lPitchX = -lDiePitchX;
				acLookAheadDie[3].m_lPitchY = 0;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[4].m_ulRow = ulRow - 1;
				acLookAheadDie[4].m_ulCol = ulCol + 1;
				acLookAheadDie[4].m_lPitchX = -lDiePitchX;
				acLookAheadDie[4].m_lPitchY = lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[5].m_ulRow = ulRow - 1;
				acLookAheadDie[5].m_ulCol = ulCol;
				acLookAheadDie[5].m_lPitchX = 0;
				acLookAheadDie[5].m_lPitchY = lDiePitchY;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[6].m_ulRow = ulRow - 1;
				acLookAheadDie[6].m_ulCol = ulCol - 1;
				acLookAheadDie[6].m_lPitchX = lDiePitchX;
				acLookAheadDie[6].m_lPitchY = lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[7].m_ulRow = ulRow;
				acLookAheadDie[7].m_ulCol = ulCol - 1;
				acLookAheadDie[7].m_lPitchX = lDiePitchX;
				acLookAheadDie[7].m_lPitchY = 0;
			}
			break;

		case BT_BLH_PATH:
		case BT_BRH_PATH:
			if (!bRotate180)
			{
				//	  Left/cur/Right
				//     0    1        2
				//     7    8(die)   3
				//     6    5        4
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[0].m_ulRow = ulRow - 1;
				acLookAheadDie[0].m_ulCol = ulCol - 1;
				acLookAheadDie[0].m_lPitchX = -lDiePitchX;
				acLookAheadDie[0].m_lPitchY = -lDiePitchY;

				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[1].m_ulRow = ulRow - 1;
				acLookAheadDie[1].m_ulCol = ulCol;
				acLookAheadDie[1].m_lPitchX = 0;
				acLookAheadDie[1].m_lPitchY = -lDiePitchY;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[2].m_ulRow = ulRow - 1;
				acLookAheadDie[2].m_ulCol = ulCol + 1;
				acLookAheadDie[2].m_lPitchX = lDiePitchX;
				acLookAheadDie[2].m_lPitchY = -lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[3].m_ulRow = ulRow;
				acLookAheadDie[3].m_ulCol = ulCol + 1;
				acLookAheadDie[3].m_lPitchX = lDiePitchX;
				acLookAheadDie[3].m_lPitchY = 0;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[4].m_ulRow = ulRow + 1;
				acLookAheadDie[4].m_ulCol = ulCol + 1;
				acLookAheadDie[4].m_lPitchX = lDiePitchX;
				acLookAheadDie[4].m_lPitchY = lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[5].m_ulRow = ulRow + 1;
				acLookAheadDie[5].m_ulCol = ulCol;
				acLookAheadDie[5].m_lPitchX = 0;
				acLookAheadDie[5].m_lPitchY = lDiePitchY;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[6].m_ulRow = ulRow + 1;
				acLookAheadDie[6].m_ulCol = ulCol - 1;
				acLookAheadDie[6].m_lPitchX = -lDiePitchX;
				acLookAheadDie[6].m_lPitchY = lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[7].m_ulRow = ulRow;
				acLookAheadDie[7].m_ulCol = ulCol - 1;
				acLookAheadDie[7].m_lPitchX = -lDiePitchX;
				acLookAheadDie[7].m_lPitchY = 0;
			}
			else
			{
				//	  Left/cur/Right
				//     4    5        6
				//     3    8(die)   7
				//     2    1        0
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[0].m_ulRow = ulRow - 1;
				acLookAheadDie[0].m_ulCol = ulCol - 1;
				acLookAheadDie[0].m_lPitchX = lDiePitchX;
				acLookAheadDie[0].m_lPitchY = lDiePitchY;

				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[1].m_ulRow = ulRow - 1;
				acLookAheadDie[1].m_ulCol = ulCol;
				acLookAheadDie[1].m_lPitchX = 0;
				acLookAheadDie[1].m_lPitchY = lDiePitchY;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[2].m_ulRow = ulRow - 1;
				acLookAheadDie[2].m_ulCol = ulCol + 1;
				acLookAheadDie[2].m_lPitchX = -lDiePitchX;
				acLookAheadDie[2].m_lPitchY = lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[3].m_ulRow = ulRow;
				acLookAheadDie[3].m_ulCol = ulCol + 1;
				acLookAheadDie[3].m_lPitchX = -lDiePitchX;
				acLookAheadDie[3].m_lPitchY = 0;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[4].m_ulRow = ulRow + 1;
				acLookAheadDie[4].m_ulCol = ulCol + 1;
				acLookAheadDie[4].m_lPitchX = -lDiePitchX;
				acLookAheadDie[4].m_lPitchY = -lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[5].m_ulRow = ulRow + 1;
				acLookAheadDie[5].m_ulCol = ulCol;
				acLookAheadDie[5].m_lPitchX = 0;
				acLookAheadDie[5].m_lPitchY = -lDiePitchY;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[6].m_ulRow = ulRow + 1;
				acLookAheadDie[6].m_ulCol = ulCol - 1;
				acLookAheadDie[6].m_lPitchX = lDiePitchX;
				acLookAheadDie[6].m_lPitchY = -lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[7].m_ulRow = ulRow;
				acLookAheadDie[7].m_ulCol = ulCol - 1;
				acLookAheadDie[7].m_lPitchX = lDiePitchX;
				acLookAheadDie[7].m_lPitchY = 0;
			}
			break;
		case BT_TLV_PATH:
		case BT_BLV_PATH:
			if (!bRotate180)
			{
				//     6    7        0    LEFT
				//     5    8(die)   1    Cur
				//     4    3        2    RIGHT
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[0].m_ulRow = ulRow - 1;
				acLookAheadDie[0].m_ulCol = ulCol + 1;
				acLookAheadDie[0].m_lPitchX = lDiePitchX;
				acLookAheadDie[0].m_lPitchY = -lDiePitchY;

				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[1].m_ulRow = ulRow;
				acLookAheadDie[1].m_ulCol = ulCol + 1;
				acLookAheadDie[1].m_lPitchX = lDiePitchX;
				acLookAheadDie[1].m_lPitchY =0;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[2].m_ulRow = ulRow + 1;
				acLookAheadDie[2].m_ulCol = ulCol + 1;
				acLookAheadDie[2].m_lPitchX = lDiePitchX;
				acLookAheadDie[2].m_lPitchY = lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[3].m_ulRow = ulRow + 1;
				acLookAheadDie[3].m_ulCol = ulCol;
				acLookAheadDie[3].m_lPitchX = 0;
				acLookAheadDie[3].m_lPitchY = lDiePitchY;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[4].m_ulRow = ulRow + 1;
				acLookAheadDie[4].m_ulCol = ulCol - 1;
				acLookAheadDie[4].m_lPitchX = -lDiePitchX;
				acLookAheadDie[4].m_lPitchY = lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[5].m_ulRow = ulRow;
				acLookAheadDie[5].m_ulCol = ulCol - 1;
				acLookAheadDie[5].m_lPitchX = -lDiePitchX;
				acLookAheadDie[5].m_lPitchY = 0;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[6].m_ulRow = ulRow - 1;
				acLookAheadDie[6].m_ulCol = ulCol - 1;
				acLookAheadDie[6].m_lPitchX = -lDiePitchX;
				acLookAheadDie[6].m_lPitchY = -lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[7].m_ulRow = ulRow - 1;
				acLookAheadDie[7].m_ulCol = ulCol;
				acLookAheadDie[7].m_lPitchX = 0;
				acLookAheadDie[7].m_lPitchY = -lDiePitchY;
			}
			else
			{
				//     2    3        4    LEFT
				//     1    8(die)   5    Cur
				//     0    7        6    RIGHT
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[0].m_ulRow = ulRow - 1;
				acLookAheadDie[0].m_ulCol = ulCol + 1;
				acLookAheadDie[0].m_lPitchX = -lDiePitchX;
				acLookAheadDie[0].m_lPitchY = lDiePitchY;

				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[1].m_ulRow = ulRow;
				acLookAheadDie[1].m_ulCol = ulCol + 1;
				acLookAheadDie[1].m_lPitchX = -lDiePitchX;
				acLookAheadDie[1].m_lPitchY = 0;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[2].m_ulRow = ulRow + 1;
				acLookAheadDie[2].m_ulCol = ulCol + 1;
				acLookAheadDie[2].m_lPitchX = -lDiePitchX;
				acLookAheadDie[2].m_lPitchY = -lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[3].m_ulRow = ulRow + 1;
				acLookAheadDie[3].m_ulCol = ulCol;
				acLookAheadDie[3].m_lPitchX = 0;
				acLookAheadDie[3].m_lPitchY = -lDiePitchY;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[4].m_ulRow = ulRow + 1;
				acLookAheadDie[4].m_ulCol = ulCol - 1;
				acLookAheadDie[4].m_lPitchX = lDiePitchX;
				acLookAheadDie[4].m_lPitchY = -lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[5].m_ulRow = ulRow;
				acLookAheadDie[5].m_ulCol = ulCol - 1;
				acLookAheadDie[5].m_lPitchX = lDiePitchX;
				acLookAheadDie[5].m_lPitchY = 0;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[6].m_ulRow = ulRow - 1;
				acLookAheadDie[6].m_ulCol = ulCol - 1;
				acLookAheadDie[6].m_lPitchX = lDiePitchX;
				acLookAheadDie[6].m_lPitchY = lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[7].m_ulRow = ulRow - 1;
				acLookAheadDie[7].m_ulCol = ulCol;
				acLookAheadDie[7].m_lPitchX = 0;
				acLookAheadDie[7].m_lPitchY = lDiePitchY;
			}
			break;
		case BT_TRV_PATH:
		case BT_BRV_PATH:
			if (!bRotate180)
			{
				// LEFT    0    7        6
				// CUR     1    8(die)   5
				// RIGHT   2    3        4
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[0].m_ulRow = ulRow - 1;
				acLookAheadDie[0].m_ulCol = ulCol - 1;
				acLookAheadDie[0].m_lPitchX = -lDiePitchX;
				acLookAheadDie[0].m_lPitchY = -lDiePitchY;

				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[1].m_ulRow = ulRow;
				acLookAheadDie[1].m_ulCol = ulCol - 1;
				acLookAheadDie[1].m_lPitchX = -lDiePitchX;
				acLookAheadDie[1].m_lPitchY = 0;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[2].m_ulRow = ulRow + 1;
				acLookAheadDie[2].m_ulCol = ulCol - 1;
				acLookAheadDie[2].m_lPitchX = -lDiePitchX;
				acLookAheadDie[2].m_lPitchY = lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[3].m_ulRow = ulRow + 1;
				acLookAheadDie[3].m_ulCol = ulCol;
				acLookAheadDie[3].m_lPitchX = 0;
				acLookAheadDie[3].m_lPitchY = lDiePitchY;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[4].m_ulRow = ulRow + 1;
				acLookAheadDie[4].m_ulCol = ulCol + 1;
				acLookAheadDie[4].m_lPitchX = lDiePitchX;
				acLookAheadDie[4].m_lPitchY = lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[5].m_ulRow = ulRow;
				acLookAheadDie[5].m_ulCol = ulCol + 1;
				acLookAheadDie[5].m_lPitchX = lDiePitchX;
				acLookAheadDie[5].m_lPitchY = 0;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[6].m_ulRow = ulRow - 1;
				acLookAheadDie[6].m_ulCol = ulCol + 1;
				acLookAheadDie[6].m_lPitchX = lDiePitchX;
				acLookAheadDie[6].m_lPitchY = -lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[7].m_ulRow = ulRow - 1;
				acLookAheadDie[7].m_ulCol = ulCol;
				acLookAheadDie[7].m_lPitchX = 0;
				acLookAheadDie[7].m_lPitchY = -lDiePitchY;
			}
			else
			{
				// LEFT    4    3        2
				// CUR     5    8(die)   1
				// RIGHT   6    7        0
				acLookAheadDie[0].m_ulLookAheadDirection = BT_PR_BOTTOM_RIGHT;
				acLookAheadDie[0].m_ulRow = ulRow - 1;
				acLookAheadDie[0].m_ulCol = ulCol - 1;
				acLookAheadDie[0].m_lPitchX = lDiePitchX;
				acLookAheadDie[0].m_lPitchY = lDiePitchY;

				acLookAheadDie[1].m_ulLookAheadDirection = BT_PR_RIGHT;
				acLookAheadDie[1].m_ulRow = ulRow;
				acLookAheadDie[1].m_ulCol = ulCol - 1;
				acLookAheadDie[1].m_lPitchX = lDiePitchX;
				acLookAheadDie[1].m_lPitchY = 0;

				acLookAheadDie[2].m_ulLookAheadDirection = BT_PR_TOP_RIGHT;
				acLookAheadDie[2].m_ulRow = ulRow + 1;
				acLookAheadDie[2].m_ulCol = ulCol - 1;
				acLookAheadDie[2].m_lPitchX = lDiePitchX;
				acLookAheadDie[2].m_lPitchY = -lDiePitchY;

				acLookAheadDie[3].m_ulLookAheadDirection = BT_PR_TOP;
				acLookAheadDie[3].m_ulRow = ulRow + 1;
				acLookAheadDie[3].m_ulCol = ulCol;
				acLookAheadDie[3].m_lPitchX = 0;
				acLookAheadDie[3].m_lPitchY = -lDiePitchY;

				acLookAheadDie[4].m_ulLookAheadDirection = BT_PR_TOP_LEFT;
				acLookAheadDie[4].m_ulRow = ulRow + 1;
				acLookAheadDie[4].m_ulCol = ulCol + 1;
				acLookAheadDie[4].m_lPitchX = -lDiePitchX;
				acLookAheadDie[4].m_lPitchY = -lDiePitchY;

				acLookAheadDie[5].m_ulLookAheadDirection = BT_PR_LEFT;
				acLookAheadDie[5].m_ulRow = ulRow;
				acLookAheadDie[5].m_ulCol = ulCol + 1;
				acLookAheadDie[5].m_lPitchX = -lDiePitchX;
				acLookAheadDie[5].m_lPitchY = 0;

				acLookAheadDie[6].m_ulLookAheadDirection = BT_PR_BOTTOM_LEFT;
				acLookAheadDie[6].m_ulRow = ulRow - 1;
				acLookAheadDie[6].m_ulCol = ulCol + 1;
				acLookAheadDie[6].m_lPitchX = -lDiePitchX;
				acLookAheadDie[6].m_lPitchY = lDiePitchY;

				acLookAheadDie[7].m_ulLookAheadDirection = BT_PR_BOTTOM;
				acLookAheadDie[7].m_ulRow = ulRow - 1;
				acLookAheadDie[7].m_ulCol = ulCol;
				acLookAheadDie[7].m_lPitchX = 0;
				acLookAheadDie[7].m_lPitchY = lDiePitchY;
			}
			break;
	}

	acLookAheadDie[8].m_ulLookAheadDirection = BT_PR_CENTER;
	acLookAheadDie[8].m_ulRow = ulRow;
	acLookAheadDie[8].m_ulCol = ulCol;
	acLookAheadDie[8].m_lPitchX = 0;
	acLookAheadDie[8].m_lPitchY = 0;
}

ULONG CBinTable::GetLookAheadYDirectionWtihPreviousDie(const ULONG ulWalkPath, const BOOL bRotate180, LONG &lRowDir, LONG &lColDir)
{
	LONG lLookAheadYDirection = BT_PR_TOP;

	switch (ulWalkPath)
	{
		case BT_TLH_PATH:
		case BT_TRH_PATH:
			lLookAheadYDirection = BT_PR_TOP;
			if (!bRotate180)
			{
				lRowDir = -1;
				lColDir = 0;
			}
			else
			{
				lRowDir = 1;
				lColDir = 0;
			}
			break;

		case BT_BLH_PATH:
		case BT_BRH_PATH:
			lLookAheadYDirection = BT_PR_BOTTOM;
			if (!bRotate180)
			{
				lRowDir = 1;
				lColDir = 0;
			}
			else
			{
				lRowDir = -1;
				lColDir = 0;
			}
			break;
		case BT_TLV_PATH:
		case BT_BLV_PATH:
			lLookAheadYDirection = BT_PR_LEFT;
			if (!bRotate180)
			{
				lRowDir = 0;
				lColDir = 1;
			}
			else
			{
				lRowDir = 0;
				lColDir = -1;
			}

			break;
		case BT_TRV_PATH:
		case BT_BRV_PATH:
			lLookAheadYDirection = BT_PR_RIGHT;
			if (!bRotate180)
			{
				lRowDir = 0;
				lColDir = -1;
			}
			else
			{
				lRowDir = 0;
				lColDir = 1;
			}
			break;
	}

	return lLookAheadYDirection;
}



VOID CBinTable::GetLookAheadDirection(const ULONG ulWalkPath, const LONG lDiePitchX, const LONG lDiePitchY, CLookAheadMap &clLookAheadDirection)
{
	clLookAheadDirection.m_ulLookAheadDirection = BT_PR_CENTER;
	clLookAheadDirection.m_ulRow = 0;
	clLookAheadDirection.m_ulCol = 0;
	clLookAheadDirection.m_lPitchX = 0;
	clLookAheadDirection.m_lPitchY = 0;

	if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_BLH_PATH))
	{
		clLookAheadDirection.m_ulLookAheadDirection = BT_PR_RIGHT;
		clLookAheadDirection.m_ulRow = 0;
		clLookAheadDirection.m_ulCol = 0;
		clLookAheadDirection.m_lPitchX = lDiePitchX;
		clLookAheadDirection.m_lPitchY = 0;
	}
	else if ((ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH))
	{
		clLookAheadDirection.m_ulLookAheadDirection = BT_PR_LEFT;
		clLookAheadDirection.m_ulRow = 0;
		clLookAheadDirection.m_ulCol = 0;
		clLookAheadDirection.m_lPitchX = -lDiePitchX;
		clLookAheadDirection.m_lPitchY = 0;
	}
	else if ((ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_BLV_PATH))
	{
		clLookAheadDirection.m_ulLookAheadDirection = BT_PR_BOTTOM;
		clLookAheadDirection.m_ulRow = 0;
		clLookAheadDirection.m_ulCol = 0;
		clLookAheadDirection.m_lPitchX = 0;
		clLookAheadDirection.m_lPitchY = lDiePitchY;
	}
	else if ((ulWalkPath == BT_TRV_PATH) || (ulWalkPath == BT_BRV_PATH))
	{
		clLookAheadDirection.m_ulLookAheadDirection = BT_PR_TOP;
		clLookAheadDirection.m_ulRow = 0;
		clLookAheadDirection.m_ulCol = 0;
		clLookAheadDirection.m_lPitchX = 0;
		clLookAheadDirection.m_lPitchY = -lDiePitchY;
	}
}


BOOL CBinTable::ConfirmLastRow(const ULONG ulBlkInUse, const CLookAheadMap acLookAheadDie[9], const LONG lRow, const LONG lCol,
							   LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4],BOOL bNoRetry)
{
	// the next die (aclookAheadDie[1].m_ulRow, aclookAheadDie[1].m_ulCol) is existed
	BOOL bPRLookAheadYDirectionExisted = FALSE;
	if (m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[1].m_ulRow, acLookAheadDie[1].m_ulCol))
	{
		bPRLookAheadYDirectionExisted = TRUE;
	}

	//======================================================================================================
	//check the die'PR result of the lookahead Y direction is matched with the bin map information
	LONG lPROffsetX_Count = 0, lPROffsetY_Count = 0;
	LONG lRet = 0;
	LONG lRetryCount = 20;
	LONG lRowRetryCount = 0, lColRetryCount = 0;

	if (!bPRLookAheadYDirectionExisted)
	{
		while (TRUE)
		{
			//it should be no die in the next row
			if (SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, acLookAheadDie[1].m_ulLookAheadDirection, 1))
			{
				if (bNoRetry)
				{
					return FALSE;
				}
				alIndexPhysical_x[lCol] += lPROffsetX_Count;
				alIndexPhysical_y[lCol] += lPROffsetY_Count;
				if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
												alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
				{
					return FALSE; //Stop
				}
				if (++lRowRetryCount >= lRetryCount)
				{
					return FALSE; //Stop
				}
			}
			else
			{
				//Last Row is found(No Die) and confirm the column position again
				BOOL bLeftDieInMap = m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[0].m_ulRow, acLookAheadDie[0].m_ulCol);
				BOOL bRightDieInMap = m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[2].m_ulRow, acLookAheadDie[2].m_ulCol);
				LONG lLeft_PROffsetX_Count = 0, lLeft_PROffsetY_Count = 0;
				LONG lRight_PROffsetX_Count = 0, lRight_PROffsetY_Count = 0;
				BOOL bLAPRLeftDieExisted = SearchGoodDieXYinFOV(lLeft_PROffsetX_Count, lLeft_PROffsetY_Count, acLookAheadDie[0].m_ulLookAheadDirection, 1);
				BOOL bLAPRRightDieExisted = SearchGoodDieXYinFOV(lRight_PROffsetX_Count, lRight_PROffsetY_Count, acLookAheadDie[2].m_ulLookAheadDirection, 1);
				if ((bLeftDieInMap == bLAPRLeftDieExisted) && (bRightDieInMap == bLAPRRightDieExisted))
				{
					break;
				}
				else
				{
					if ((bLeftDieInMap != bLAPRLeftDieExisted) && (bRightDieInMap == bLAPRRightDieExisted))
					{
						alIndexPhysical_x[lCol] += acLookAheadDie[0].m_lPitchX - acLookAheadDie[1].m_lPitchX;
						alIndexPhysical_y[lCol] += acLookAheadDie[0].m_lPitchY - acLookAheadDie[1].m_lPitchY;
						if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
														alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
						{
							return FALSE; //Stop
						}
						if (++lColRetryCount >= lRetryCount)
						{
							return FALSE; //Stop
						}
					}
					else if ((bLeftDieInMap == bLAPRLeftDieExisted) && (bRightDieInMap != bLAPRRightDieExisted))
					{
						alIndexPhysical_x[lCol] += acLookAheadDie[2].m_lPitchX - acLookAheadDie[1].m_lPitchX;
						alIndexPhysical_y[lCol] += acLookAheadDie[2].m_lPitchY - acLookAheadDie[1].m_lPitchY;
						if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
														alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
						{
							return FALSE; //Stop
						}
						if (++lColRetryCount >= lRetryCount)
						{
							return FALSE; //Stop
						}
					}
					else
					{
						return FALSE; //Stop
					}
				}
			}
		}
	}
	else
	{
		while (TRUE)
		{
			//it should be die in the last(n-1) row
			if (!SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, acLookAheadDie[1].m_ulLookAheadDirection, 1))
			{
				//Backward
				if (bNoRetry)
				{
					return FALSE;
				}
				alIndexPhysical_x[lCol] += acLookAheadDie[5].m_lPitchX;
				alIndexPhysical_y[lCol] += acLookAheadDie[5].m_lPitchY;
				if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
												alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
				{
					return FALSE;
				}

				if (++lRowRetryCount >= lRetryCount)
				{
					return FALSE; //Stop
				}
			}
			else
			{
				//No Die and confirm again
				BOOL bLeftDieInMap = m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[0].m_ulRow, acLookAheadDie[0].m_ulCol);
				BOOL bRightDieInMap = m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[2].m_ulRow, acLookAheadDie[2].m_ulCol);
				LONG lLeft_PROffsetX_Count = 0, lLeft_PROffsetY_Count = 0;
				LONG lRight_PROffsetX_Count = 0, lRight_PROffsetY_Count = 0;
				BOOL bLAPRLeftDieExisted = SearchGoodDieXYinFOV(lLeft_PROffsetX_Count, lLeft_PROffsetY_Count, acLookAheadDie[0].m_ulLookAheadDirection, 1);
				BOOL bLAPRRightDieExisted = SearchGoodDieXYinFOV(lRight_PROffsetX_Count, lRight_PROffsetY_Count, acLookAheadDie[2].m_ulLookAheadDirection, 1);
				if ((bLeftDieInMap == bLAPRLeftDieExisted) && (bRightDieInMap == bLAPRRightDieExisted))
				{
					break;
				}
				else
				{
					if ((bLeftDieInMap != bLAPRLeftDieExisted) && (bRightDieInMap == bLAPRRightDieExisted))
					{
						alIndexPhysical_x[lCol] += acLookAheadDie[0].m_lPitchX - acLookAheadDie[1].m_lPitchX;
						alIndexPhysical_y[lCol] += acLookAheadDie[0].m_lPitchY - acLookAheadDie[1].m_lPitchY;
						if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
														alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
						{
							return FALSE; //Stop
						}
						if (++lColRetryCount >= lRetryCount)
						{
							return FALSE; //Stop
						}
					}
					else if ((bLeftDieInMap == bLAPRLeftDieExisted) && (bRightDieInMap != bLAPRRightDieExisted))
					{
						alIndexPhysical_x[lCol] += acLookAheadDie[2].m_lPitchX - acLookAheadDie[1].m_lPitchX;
						alIndexPhysical_y[lCol] += acLookAheadDie[2].m_lPitchY - acLookAheadDie[1].m_lPitchY;
						if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
														alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
						{
							return FALSE; //Stop
						}
						if (++lColRetryCount >= lRetryCount)
						{
							return FALSE; //Stop
						}
					}
					else
					{
						return FALSE; //Stop
					}
				}
			}
		}

		lRowRetryCount = 0;
		while (TRUE)
		{
			//it should be no die in the next-next row
			if (SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, acLookAheadDie[1].m_ulLookAheadDirection, 2))
			{
				alIndexPhysical_x[lCol] += lPROffsetX_Count;
				alIndexPhysical_y[lCol] += _round(lPROffsetY_Count / 2.0);
				if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
												alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
				{
					return FALSE;
				}
				if (++lRowRetryCount >= lRetryCount)
				{
					return FALSE; //Stop
				}
			}
			else
			{
				break;
			}
		}
	}
	return TRUE;
}


BOOL CBinTable::ConfirmFirstLastColOnRow(const ULONG ulBlkInUse, const CLookAheadMap acLookAheadDie[9], const LONG lLookAheadIndex, const LONG lRow, const LONG lCol,
										 LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4])
{
	if (!((lLookAheadIndex == 3) || (lLookAheadIndex == 7)))
	{
		return TRUE;
	}
	// the next die (aclookAheadDie[lLookAheadIndex].m_ulRow, aclookAheadDie[lLookAheadIndex].m_ulCol) is existed  (lLookAheadIndex = 7 or lLookAheadIndex = 3)
	BOOL bPRLookAheadDirectionExisted = FALSE;
	if (m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[lLookAheadIndex].m_ulRow, acLookAheadDie[lLookAheadIndex].m_ulCol))
	{
		bPRLookAheadDirectionExisted = TRUE;
	}

	//======================================================================================================
	//check the die'PR result of the lookahead X direction is matched with the bin map information
	LONG lPROffsetX_Count = 0, lPROffsetY_Count = 0;
	LONG lRet = 0;
	LONG lRetryCount = 3;
	LONG lRowRetryCount = 0, lColRetryCount = 0;

	if (!bPRLookAheadDirectionExisted)
	{
		while (TRUE)
		{
			//it should be no die in the next row
			if (SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, acLookAheadDie[lLookAheadIndex].m_ulLookAheadDirection, 1))
			{
				alIndexPhysical_x[lCol] += lPROffsetX_Count;
				alIndexPhysical_y[lCol] += lPROffsetY_Count;
				if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
												alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
				{
					return FALSE; //Stop
				}
				if (++lRowRetryCount >= lRetryCount)
				{
					return FALSE; //Stop
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		LONG lLookAheadCount = 0;
		if ((acLookAheadDie[lLookAheadIndex].m_ulLookAheadDirection == BT_PR_LEFT) || (acLookAheadDie[lLookAheadIndex].m_ulLookAheadDirection == BT_PR_RIGHT))
		{
			LONG lDir = acLookAheadDie[lLookAheadIndex].m_ulCol - acLookAheadDie[8].m_ulCol;
			while (TRUE)
			{
				if (!m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[8].m_ulRow, acLookAheadDie[8].m_ulCol + lDir))
				{
					break;
				}
				lLookAheadCount++;
				if (lLookAheadCount > 3)
				{
					return TRUE; //not need to check in this direction
				}
			}
		}
		else if ((acLookAheadDie[lLookAheadIndex].m_ulLookAheadDirection == BT_PR_TOP) || (acLookAheadDie[lLookAheadIndex].m_ulLookAheadDirection == BT_PR_BOTTOM))
		{
			LONG lDir = acLookAheadDie[lLookAheadIndex].m_ulRow - acLookAheadDie[8].m_ulRow;
			while (TRUE)
			{
				if (!m_oBinBlkMain.IsBondedDieInBinMap(ulBlkInUse, acLookAheadDie[8].m_ulRow + lDir, acLookAheadDie[8].m_ulCol))
				{
					break;
				}
				lLookAheadCount++;
				if (lLookAheadCount > 3)
				{
					return TRUE; //not need to check in this direction
				}
			}
		}


		//Check die exist?
		while (TRUE)
		{
			//it should be no die in the next row
			if (!SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, acLookAheadDie[lLookAheadIndex].m_ulLookAheadDirection, lLookAheadCount))
			{
				//backward
				alIndexPhysical_x[lCol] -= acLookAheadDie[lLookAheadIndex].m_lPitchX;
				alIndexPhysical_y[lCol] -= acLookAheadDie[lLookAheadIndex].m_lPitchY;
				if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
												alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
				{
					return FALSE; //Stop
				}
				if (++lRowRetryCount >= lRetryCount)
				{
					return FALSE; //Stop
				}
			}
			else
			{
				break;
			}
		}

		//Check no-die exist?
		while (TRUE)
		{
			//it should be no die in the next row
			if (SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, acLookAheadDie[lLookAheadIndex].m_ulLookAheadDirection, lLookAheadCount + 1))
			{
				//forward
				alIndexPhysical_x[lCol] += acLookAheadDie[lLookAheadIndex].m_lPitchX;
				alIndexPhysical_y[lCol] += acLookAheadDie[lLookAheadIndex].m_lPitchY;
				if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, FALSE, FALSE,
												alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
				{
					return FALSE; //Stop
				}
				if (++lRowRetryCount >= lRetryCount)
				{
					return FALSE; //Stop
				}
			}
			else
			{
				break;
			}
		}
	}
		
	return TRUE;
}


LONG CBinTable::ConfirmMovetoLastRow(ULONG ulBlkInUse, const ULONG ulIndex, 
									 const DOUBLE dNewAngle, const LONG lXOffset, const LONG lYOffset,
									 const LONG lRow, const LONG lCol, const ULONG ulCurBinMapPath, 
									 LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4],BOOL bNoRetry)
{
	LONG lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	LONG lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));

	DOUBLE dIndexUmX = 0, dIndexUmY = 0;

	//===================================================================================================================================
	//check whether the die of the next row/col is existed or not
	ULONG ulNewIndex = ulIndex;
	ULONG ulBinMapRow = 0, ulBinMapCol = 0, ulBinMapNextRow = 0, ulBinMapNextCol = 0;
	//Get Current die map position(ulBinMapRow, ulBinMapCol)
	m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, ulNewIndex, ulBinMapRow, ulBinMapCol);

	CLookAheadMap acLookAheadDie[9]; //Left/cur/right of the next row
	BOOL bFrameRotate180 = m_oBinBlkMain.GetFrameRotation(ulBlkInUse);

	//Get the origin path
	ULONG ulOrgBinMapPath = m_oBinBlkMain.GrabWalkPath(ulBlkInUse, ulCurBinMapPath);
	GetLookAheadYDirection(ulOrgBinMapPath, bFrameRotate180, ulBinMapRow, ulBinMapCol, lDiePitchX, lDiePitchY, acLookAheadDie);

	//===================================================================================================================================

	/////////////////////////////////////////
	// Move & Find INDEX1 - 123 position 
	ulNewIndex = ulIndex;
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulNewIndex, dIndexUmX, dIndexUmY, FALSE);
	ConvertBinTableDPosn(dIndexUmX, dIndexUmY, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
CString szLog;
	szLog.Format("ConfirmMovetoLastRow-FindCurrentDieCenter");
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	LONG lCurEncX = ConvertFileUnitToXEncoderValue(dIndexUmX);
	LONG lCurEncY = ConvertFileUnitToYEncoderValue(dIndexUmY);
	LONG lRet = 0;
	if ((lRet = FindCurrentDieCenter(lCurEncX, lCurEncY, lRow, lCol, TRUE, TRUE,
							 alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
	{
		return lRet;
	}

	szLog.Format("ConfirmMovetoLastRow-ConfirmLastRow");
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	if (!ConfirmLastRow(ulBlkInUse, acLookAheadDie, lRow, lCol, alIndexPhysical_x, alIndexPhysical_y, bPRResult,bNoRetry))
	{
		if (bNoRetry)
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("No Retry Triggered in Confirm Move to Last Row", "a+");
			lRet = 1;
			return 1;
		}
		//move to origin position if confirm failure
		if ((lRet = FindCurrentDieCenter(lCurEncX,lCurEncY, lRow, lCol, TRUE, TRUE,
										 alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
		{
			return lRet;
		}
	}

	if (lCol == 0)
	{
		szLog.Format("ConfirmMovetoLastRow-ConfirmFirstLastColOnRow1");
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		lCurEncX = alIndexPhysical_x[lCol];
		lCurEncY = alIndexPhysical_y[lCol];
		//Is the edge die
		if (!ConfirmFirstLastColOnRow(ulBlkInUse, acLookAheadDie, 7, lRow, lCol, alIndexPhysical_x, alIndexPhysical_y, bPRResult))
		{
			//move to origin position if confirm failure
			if ((lRet = FindCurrentDieCenter(lCurEncX,lCurEncY, lRow, lCol, TRUE, TRUE,
											 alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
			{
				return lRet;
			}
		}

		szLog.Format("ConfirmMovetoLastRow-ConfirmFirstLastColOnRow2");
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		//Is the edge die
		if (!ConfirmFirstLastColOnRow(ulBlkInUse, acLookAheadDie, 3, lRow, lCol, alIndexPhysical_x, alIndexPhysical_y, bPRResult))
		{
			//move to origin position if confirm failure
			if ((lRet = FindCurrentDieCenter(lCurEncX,lCurEncY, lRow, lCol, TRUE, TRUE,
											 alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
			{
				return lRet;
			}
		}
	}


	return 0;
}



LONG CBinTable::LookAhead4DiePRSearchIndexPosn(ULONG ulBlkInUse, const ULONG ulIndex, 
											   const DOUBLE dNewAngle, const LONG lXOffset, const LONG lYOffset,
											   const LONG lRow, const LONG lCol, const ULONG ulCurBinMapPath, BOOL &bConfirmLastRow,
											   LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4])
{
	LONG lRet = 0;
	if (bConfirmLastRow)
	{
		if ((lRet = ConfirmMovetoLastRow(ulBlkInUse, ulIndex, dNewAngle, lXOffset, lYOffset, lRow, lCol, ulCurBinMapPath, 
								 alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
		{
			return lRet;
		}
	}
	else
	{
		if ((lRet = FindCurrentDieCenter(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol], lRow, lCol, TRUE, FALSE,
								 alIndexPhysical_x, alIndexPhysical_y, bPRResult)) > 0)
		{
			return lRet;
		}
	}

	bConfirmLastRow = FALSE;
	//======================================================================================================
	//the path direction of current die
	LONG lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
	LONG lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));
	CLookAheadMap clPRLookAheadDirection;
	GetLookAheadDirection(ulCurBinMapPath, lDiePitchX, lDiePitchY, clPRLookAheadDirection);
	LONG lPROffsetX_Count = 0, lPROffsetY_Count = 0;

	LONG i = lCol + 1;
	for (i = lCol + 1; i < 4; i++)
	{
		bPRResult[i] = SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, clPRLookAheadDirection.m_ulLookAheadDirection, i - lCol);
		if (bPRResult[i])
		{
			alIndexPhysical_x[i] = alIndexPhysical_x[lCol] + lPROffsetX_Count;
			alIndexPhysical_y[i] = alIndexPhysical_y[lCol] + lPROffsetY_Count;
			CString szLog;
			szLog.Format("FindLastRowColAngle: INDEX %d-%d Enc (%ld, %ld)", lRow, i + 1, alIndexPhysical_x[i], alIndexPhysical_y[i]);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
		else
		{
			alIndexPhysical_x[i] = alIndexPhysical_x[i - 1] + clPRLookAheadDirection.m_lPitchX;
			alIndexPhysical_y[i] = alIndexPhysical_y[i - 1] + clPRLookAheadDirection.m_lPitchY;
			return i;
		}
	}

	return i;
}


LONG CBinTable::MoveToGoodDieIndexPosn(const ULONG ulCurIndex, const ULONG ulStartIndex,
									   LONG alIndexPhysical_x[4], LONG alIndexPhysical_y[4], BOOL bPRResult[4])
{
	//Move to good die
	if ((ulCurIndex != ulStartIndex) && (!bPRResult[3]))
	{
		for (LONG lCol = 0; lCol < 3; lCol++)
		{
			 if (bPRResult[lCol])
			 {
				 BT_MoveTo(alIndexPhysical_x[lCol], alIndexPhysical_y[lCol]);
				 return ulStartIndex + lCol;
			 }
		}
	}
	return ulCurIndex;
}


VOID CBinTable::GetLookAheadPRAverageOffset(ULONG ulBlkInUse, ULONG ulCurIndex, ULONG ulPRLookAheadDirection, const LONG lCurEncX, const LONG lCurEncY,  const LONG lOffsetX, const LONG lOffsetY, 
											const LONG lRowDir, const LONG lColDir, const LONG lLookAheadNum, const BOOL bLookAheadYDirection, LONG &lAvgOffsetX, LONG &lAvgOffsetY)
{
	LONG alOffsetX[10], alOffsetY[10];
	BOOL bPRResult[10];

	for (LONG i = 0; i < 10; i++)
	{
		alOffsetX[i] = alOffsetY[i] = 0;
		bPRResult[i] = FALSE;
	}
	//======================================================================================================
	LONG lPROffsetX_Count = 0, lPROffsetY_Count = 0;

	bPRResult[0] = TRUE;
	alOffsetX[0] = lOffsetX;
	alOffsetY[0] = lOffsetY;

	ULONG ulNewIndex = ulCurIndex;
	ULONG ulBinMapRow = 0, ulBinMapCol = 0;
	m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, ulNewIndex, ulBinMapRow, ulBinMapCol);

	for (LONG i = 0; i <= lLookAheadNum; i++)
	{
		bPRResult[i] = SearchGoodDieXYinFOV(lPROffsetX_Count, lPROffsetY_Count, (i == 0) ? BT_PR_CENTER : ulPRLookAheadDirection, i, 1, FALSE);
		if (bPRResult[i])
		{
			double dTempLogicalEncX, dTempLogicalEncY;
			ULONG ulIndex = ulCurIndex + i;
			if (bLookAheadYDirection)
			{
				m_oBinBlkMain.GetDieIndexInBinMapWithRowCol(ulBlkInUse, ulBinMapRow + lRowDir * i, ulBinMapCol + lColDir * i, ulIndex);
				m_oBinBlkMain.SubStepDMove(ulBlkInUse, ulIndex, dTempLogicalEncX, dTempLogicalEncY);
			}
			else
			{
				m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex, dTempLogicalEncX, dTempLogicalEncY);
			}

			LONG lTempLogicalEncX = ConvertFileUnitToXEncoderValue(dTempLogicalEncX);
			LONG lTempLogicalEncY = ConvertFileUnitToXEncoderValue(dTempLogicalEncY);

			LONG lTempXOffset = -ConvertXEncoderValueToFileUnit(lCurEncX + lPROffsetX_Count - lTempLogicalEncX);
			LONG lTempYOffset = -ConvertYEncoderValueToFileUnit(lCurEncY + lPROffsetY_Count - lTempLogicalEncY);

			alOffsetX[i] = lTempXOffset;
			alOffsetY[i] = lTempYOffset;
			CString szLog;
			szLog.Format("GetLookAheadPRAverageOffset: INDEX %d OFFSET (%ld, %ld)", i, alOffsetX[i], alOffsetY[i]);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
	}

	//Find the point with maximum offset 
	LONG lMaxIndexPosn = 0;
	LONG lTotalCount = 0;
	for (LONG i = 1; i <= lLookAheadNum; i++)
	{
		if (bPRResult[i])
		{
			double dMaxSD = alOffsetX[lMaxIndexPosn] * alOffsetX[lMaxIndexPosn] + alOffsetY[lMaxIndexPosn] * alOffsetY[lMaxIndexPosn];
			double dCurSD = alOffsetX[i] * alOffsetX[i] + alOffsetY[i] * alOffsetY[i];

			if (dCurSD > dMaxSD)
			{
				lMaxIndexPosn = i;
			}
			lTotalCount++;
		}
	}

	//Remove point with maximum offset in array
	if (lTotalCount > 3)
	{
		bPRResult[lMaxIndexPosn] = FALSE;
	}

	//Calculate the average
	DOUBLE dTotalOffsetX = 0;
	DOUBLE dTotalOffsetY = 0;
	lTotalCount = 0;
	for (LONG i = 0; i <= lLookAheadNum; i++)
	{
		if (bPRResult[i])
		{
			dTotalOffsetX += alOffsetX[i];
			dTotalOffsetY += alOffsetY[i];
			lTotalCount++;
		}
	}

	if (lTotalCount > 0)
	{
		lAvgOffsetX = _round(dTotalOffsetX / lTotalCount);
		lAvgOffsetY = _round(dTotalOffsetY / lTotalCount);
	}
	else
	{
		lAvgOffsetX = lOffsetX;
		lAvgOffsetY = lOffsetY;
	}
}


VOID CBinTable::GetLookAheadValidData(const ULONG ulPRLookAheadDirection, LONG lOffsetX, LONG lOffsetY, LONG &lNewOffsetX, LONG &lNewOffsetY)
{
	switch (ulPRLookAheadDirection)
	{
	case BT_PR_LEFT:
	case BT_PR_RIGHT:
		 lNewOffsetX = lOffsetX;
		 break;
	case BT_PR_TOP:
	case BT_PR_BOTTOM:
		 lNewOffsetY = lOffsetY;
		 break;
	case BT_PR_CENTER:
		 lNewOffsetX = lOffsetX;
		 lNewOffsetY = lOffsetY;
		 break;
	}
}



VOID CBinTable::CalcAngle(const double dDiffY, const double dDiffX, double &dThetaAngle, double &dThetaAngleInRadian)
{
	dThetaAngle			= atan2( fabs(dDiffY) , fabs(dDiffX) ) * 180 / PI;
	dThetaAngleInRadian	= atan2( fabs(dDiffY) , fabs(dDiffX) );
}


VOID CBinTable::ReverseAngle(double &dThetaAngle, double &dThetaAngleInRadian)
{
	dThetaAngle			= dThetaAngle * -1.00;
	dThetaAngleInRadian = dThetaAngleInRadian * -1.00;
}




BOOL CBinTable::FindLastRowColumnFrameAngle(const BOOL bDoCenterLineCompensate, const BOOL b1stCompensateGobalAngle, const ULONG ulBlkInUse, const ULONG ulDieIndex, 
											const DOUBLE dNewAngle, 
											const BOOL bUpdateOffset, LONG& lNewXOffset, LONG& lNewYOffset,
											DOUBLE& dNewLastRowColAngle, DOUBLE& dNewCheckRealignmentAngle)
{
	CString szLog;
	ULONG ulIndex1 = 0;
	ULONG ulIndex2 = 0;

	ULONG ulMapIndex	= ulDieIndex;
	LONG lXOffset		= lNewXOffset;
	LONG lYOffset		= lNewYOffset;

	if (!m_bEnable_T)
	{
		dNewLastRowColAngle = -1.00 * dNewAngle;
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColumnFrameAngle disabled: wo T", "a+");
		return FALSE;
	}
	if (m_bUseFrameCrossAlignment)
	{
		dNewLastRowColAngle = -1.00 * dNewAngle;
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColumnFrameAngle disabled: CrossChk is used", "a+");
		return FALSE;
	}

	ULONG ulWalkPath = m_oBinBlkMain.GrabWalkPath(ulBlkInUse);
	
	///////////////////////////////////////
	// Find INDEX1 and INDEX2 
	ULONG ulCurBinMapPath = BT_TLH_PATH;
	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse))
	{
		m_oBinBlkMain.GetDieIndexInBinMap2(ulBlkInUse, ulMapIndex, ulIndex1, ulIndex2, ulCurBinMapPath);
		szLog.Format("FindLastRowColAngle (Binmap): WalkPath=%lu; DieIndex: curr=%lu, Last1=%lu, Last2=%lu", 
							ulWalkPath, ulDieIndex, ulIndex1, ulIndex2);

		ulCurBinMapPath = m_oBinBlkMain.GrabWalkPath(ulBlkInUse, ulCurBinMapPath);
	}
	else
	{
		ULONG ulNoOfSortedDie	= m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
		ULONG ulDiePerRow		= m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 
		ULONG ulDiePerCol		= m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkInUse); 

		ULONG ulDividend = ulDiePerCol;		//default for TLV, BLV, TRV, BRV

		if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ||
			 (ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH) )
		{
			ulDividend = ulDiePerRow;
		}

		ULONG ulQuotient	= ulNoOfSortedDie / ulDividend;
		ULONG ulRemainder	= ulNoOfSortedDie % ulDividend;

		if (ulQuotient == 0)
		{
			ulIndex2 = ulNoOfSortedDie;
			ulIndex1 = 1;
		}
		else
		{
			//find the second last row/col or a full last row/col one.
			ulIndex2 = ulNoOfSortedDie - ulRemainder;
			if (ulRemainder == 0)
			{
				ulIndex2 = ulNoOfSortedDie - ulDividend;
			}
			ulIndex1 = ulIndex2 - ulDividend + 1;
		}

		ulCurBinMapPath = GetCurBinPath(ulWalkPath,  ulIndex1, ulDividend);

		szLog.Format("FindLastRowColAngle (Normal): WalkPath=%lu; DieIndex: curr=%lu, Last1=%lu, Last2=%lu", 
					ulWalkPath, ulDieIndex, ulIndex1, ulIndex2);
	}
	
	
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage(szLog);

	if (ulIndex2 <= 8)	//last row/col is 1st row/col, so do not use !!!
	{
		szLog.Format("FindLastRowColAngle NOT-USE due to 1st row or col (%ld, %ld)",  ulIndex1, ulIndex2);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		dNewLastRowColAngle = 0;
		return TRUE;
	}


	BOOL bIndex1PRResult[4];
	LONG alIndex1Physical_x[4], alIndex1Physical_y[4];
	for (LONG i = 0; i < 4; i++)
	{
		bIndex1PRResult[i] = FALSE;
		alIndex1Physical_x[i] = 0;
		alIndex1Physical_y[i] = 0;
	}

	BOOL bIndex2PRResult[4];
	LONG alIndex2Physical_x[4], alIndex2Physical_y[4];
	for (LONG i = 0; i < 4; i++)
	{
		bIndex2PRResult[i] = FALSE;
		alIndex2Physical_x[i] = 0;
		alIndex2Physical_y[i] = 0;
	}

	ULONG ulCurIndex = 0;

	BOOL bConfirmLastRow = TRUE;
	if (b1stCompensateGobalAngle)
	{
		//do lookahead from ulIndex1 to ulIndex1+3 without moving table
		LONG lCol = 0;
		while (lCol < 4)
		{
			lCol = LookAhead4DiePRSearchIndexPosn(ulBlkInUse, ulIndex1 + lCol, dNewAngle, lXOffset, lYOffset,
												  1, lCol, ulCurBinMapPath, bConfirmLastRow,
												  alIndex1Physical_x, alIndex1Physical_y, bIndex1PRResult);
		}

		//do lookahead from ulIndex2 -3 to ulIndex2 without moving table
		ulCurIndex = ulIndex2 - 3;
		lCol = 0;
		bConfirmLastRow = TRUE;
		while (lCol < 4)
		{
			ulCurIndex = ulIndex2 - 3 + lCol;
			lCol = LookAhead4DiePRSearchIndexPosn(ulBlkInUse, ulIndex2 - 3 + lCol, dNewAngle, lXOffset, lYOffset,
												  2, lCol, ulCurBinMapPath, bConfirmLastRow,
												  alIndex2Physical_x, alIndex2Physical_y, bIndex2PRResult);
		}

		//Move to good die
		ulCurIndex = MoveToGoodDieIndexPosn(ulCurIndex, ulIndex2 - 3, alIndex2Physical_x, alIndex2Physical_y, bIndex2PRResult);
	}
	else
	{
		LONG lCol = 0;
		while (lCol < 4)
		{
			ulCurIndex = ulIndex2 - 3 + lCol;
			lCol = LookAhead4DiePRSearchIndexPosn(ulBlkInUse, ulIndex2 - 3 + lCol, dNewAngle, lXOffset, lYOffset,
												  2, lCol, ulCurBinMapPath, bConfirmLastRow,
												  alIndex2Physical_x, alIndex2Physical_y, bIndex2PRResult);
		}

		//do lookahead from ulIndex1 to ulIndex1+3 without moving table
		lCol = 0;
		ulCurIndex = ulIndex1;
		bConfirmLastRow = TRUE;
		while (lCol < 4)
		{
			ulCurIndex = ulIndex1 + lCol;
			lCol = LookAhead4DiePRSearchIndexPosn(ulBlkInUse, ulIndex1 + lCol, dNewAngle, lXOffset, lYOffset,
												  1, lCol, ulCurBinMapPath, bConfirmLastRow,
												  alIndex1Physical_x, alIndex1Physical_y, bIndex1PRResult);
		}

		//Move to good die
		ulCurIndex = MoveToGoodDieIndexPosn(ulCurIndex, ulIndex1, alIndex1Physical_x, alIndex1Physical_y, bIndex1PRResult);
	}

	/////////////////////////////////////////
	// Code copied from BT_FindGlobalTheta()
	//Calcuate wafer angle	
	DOUBLE dThetaAngle1 = 0;
	DOUBLE dThetaAngle2 = 0;
	DOUBLE dThetaAngle3 = 0;
	DOUBLE dThetaAngle4 = 0;
	DOUBLE dThetaAngleInRadian1 = 0;
	DOUBLE dThetaAngleInRadian2 = 0;
	DOUBLE dThetaAngleInRadian3 = 0;
	DOUBLE dThetaAngleInRadian4 = 0;

	//Angle1 between INDEX1 & INDEX2
	LONG lLHS1DieX = alIndex1Physical_x[0];
	LONG lLHS1DieY = alIndex1Physical_y[0];
	LONG lRHS1DieX = alIndex2Physical_x[3];
	LONG lRHS1DieY = alIndex2Physical_y[3];

	//Angle2 between INDEX1-1 & INDEX2-1
	LONG lLHS2DieX = alIndex1Physical_x[1];
	LONG lLHS2DieY = alIndex1Physical_y[1];
	LONG lRHS2DieX = alIndex2Physical_x[2];
	LONG lRHS2DieY = alIndex2Physical_y[2];

	//Angle3 between INDEX1-2 & INDEX2-2
	LONG lLHS3DieX = alIndex1Physical_x[2];
	LONG lLHS3DieY = alIndex1Physical_y[2];
	LONG lRHS3DieX = alIndex2Physical_x[1];
	LONG lRHS3DieY = alIndex2Physical_y[1];

	//Angle4 between INDEX1-3 & INDEX2-3
	LONG lLHS4DieX = alIndex1Physical_x[3];
	LONG lLHS4DieY = alIndex1Physical_y[3];
	LONG lRHS4DieX = alIndex2Physical_x[0];
	LONG lRHS4DieY = alIndex2Physical_y[0];

	BOOL bExchange = FALSE;
	//leo lam //If Horizontal sorting, arrange LHS & RHS by Y encoder pos of INDEX1 & INDEX2
	if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_BLH_PATH) ) //First Row Go Left
	{
		//if current zigzag to the other direction, reverse positions
		if (alIndex2Physical_x[0] > alIndex1Physical_x[0])
		{
			bExchange = TRUE;
		}
	}
	else if ( (ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH) )//First Row Go Right
	{
		//if current zigzag to the other direction, reverse positions
		if (alIndex2Physical_x[0] < alIndex1Physical_x[0])
		{
			bExchange = TRUE;
		}
	}


	//If Vertical sorting, arrange LHS & RHS by Y encoder pos of INDEX1 & INDEX2
	if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) )
	{
		//if current zigzag to the other direction, reverse positions
		if (alIndex2Physical_y[0] > alIndex1Physical_y[0])
		{
			bExchange = TRUE;
		}
	}
	else if ( (ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_BRV_PATH) )
	{
		//if current zigzag to the other direction, reverse positions
		if (alIndex2Physical_y[0] < alIndex1Physical_y[0])
		{
			bExchange = TRUE;
		}
	}

	if (bExchange)
	{
		lLHS1DieX = alIndex2Physical_x[3];
		lLHS1DieY = alIndex2Physical_y[3];
		lRHS1DieX = alIndex1Physical_x[0];
		lRHS1DieY = alIndex1Physical_y[0];

		lLHS2DieX = alIndex2Physical_x[2];
		lLHS2DieY = alIndex2Physical_y[2];
		lRHS2DieX = alIndex1Physical_x[1];
		lRHS2DieY = alIndex1Physical_y[1];

		lLHS3DieX = alIndex2Physical_x[1];
		lLHS3DieY = alIndex2Physical_y[1];
		lRHS3DieX = alIndex1Physical_x[2];
		lRHS3DieY = alIndex1Physical_y[2];

		lLHS4DieX = alIndex2Physical_x[0];
		lLHS4DieY = alIndex2Physical_y[0];
		lRHS4DieX = alIndex1Physical_x[3];
		lRHS4DieY = alIndex1Physical_y[3];
	}

	DOUBLE dDiffY1 = (DOUBLE)(lLHS1DieY - lRHS1DieY);
	DOUBLE dDiffX1 = (DOUBLE)(lLHS1DieX - lRHS1DieX);
	DOUBLE dDiffY2 = (DOUBLE)(lLHS2DieY - lRHS2DieY);
	DOUBLE dDiffX2 = (DOUBLE)(lLHS2DieX - lRHS2DieX);
	DOUBLE dDiffY3 = (DOUBLE)(lLHS3DieY - lRHS3DieY);
	DOUBLE dDiffX3 = (DOUBLE)(lLHS3DieX - lRHS3DieX);
	DOUBLE dDiffY4 = (DOUBLE)(lLHS4DieY - lRHS4DieY);
	DOUBLE dDiffX4 = (DOUBLE)(lLHS4DieX - lRHS4DieX);

	//if ((ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH))
	//	dDiffX = (DOUBLE)(lRHSDieX - lLHSDieX);	
	if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_BLH_PATH) ) //leo please check that TLH and BLH should put together
	{
		dNewCheckRealignmentAngle = fabs(atan(76.2 / ConvertXEncoderValueToFileUnit(_round(dDiffX1))) * 0.61) * 180.0 /PI;
		CalcAngle(dDiffY1, dDiffX1, dThetaAngle1, dThetaAngleInRadian1);
		CalcAngle(dDiffY2, dDiffX2, dThetaAngle2, dThetaAngleInRadian2);
		CalcAngle(dDiffY3, dDiffX3, dThetaAngle3, dThetaAngleInRadian3);
		CalcAngle(dDiffY4, dDiffX4, dThetaAngle4, dThetaAngleInRadian4);

		if ( (lRHS1DieY - lLHS1DieY) < 0 )
		{
			ReverseAngle(dThetaAngle1, dThetaAngleInRadian1);
		}
		if ( (lRHS2DieY - lLHS2DieY) < 0 )
		{
			ReverseAngle(dThetaAngle2, dThetaAngleInRadian2);
		}
		if ( (lRHS3DieY - lLHS3DieY) < 0 )
		{
			ReverseAngle(dThetaAngle3, dThetaAngleInRadian3);
		}
		if ( (lRHS4DieY - lLHS4DieY) < 0 )
		{
			ReverseAngle(dThetaAngle4, dThetaAngleInRadian4);
		}
	}
	else if ( (ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH))//leo please check that TRH and BRH should put together
	{
		dNewCheckRealignmentAngle = fabs(atan(76.2 / ConvertXEncoderValueToFileUnit(_round(dDiffX1))) * 0.61) * 180.0 /PI;
		CalcAngle(dDiffY1, dDiffX1, dThetaAngle1, dThetaAngleInRadian1);
		CalcAngle(dDiffY2, dDiffX2, dThetaAngle2, dThetaAngleInRadian2);
		CalcAngle(dDiffY3, dDiffX3, dThetaAngle3, dThetaAngleInRadian3);
		CalcAngle(dDiffY4, dDiffX4, dThetaAngle4, dThetaAngleInRadian4);

		if ( (lRHS1DieY - lLHS1DieY) > 0 )
		{
			ReverseAngle(dThetaAngle1, dThetaAngleInRadian1);
		}
		if ( (lRHS2DieY - lLHS2DieY) > 0 )
		{
			ReverseAngle(dThetaAngle2, dThetaAngleInRadian2);
		}
		if ( (lRHS3DieY - lLHS3DieY) > 0 )
		{
			ReverseAngle(dThetaAngle3, dThetaAngleInRadian3);
		}
		if ( (lRHS4DieY - lLHS4DieY) > 0 )
		{
			ReverseAngle(dThetaAngle4, dThetaAngleInRadian4);
		}
	}

	
	if ( (ulWalkPath == BT_BRV_PATH) || (ulWalkPath == BT_BLV_PATH) ) //leo please check that BLV and BRV should put together
	{
		dNewCheckRealignmentAngle = fabs(atan(76.2 / ConvertYEncoderValueToFileUnit(_round(dDiffY1))) * 0.61) * 180.0 /PI;
		CalcAngle(dDiffX1, dDiffY1, dThetaAngle1, dThetaAngleInRadian1);
		CalcAngle(dDiffX2, dDiffY2, dThetaAngle2, dThetaAngleInRadian2);
		CalcAngle(dDiffX3, dDiffY3, dThetaAngle3, dThetaAngleInRadian3);
		CalcAngle(dDiffX4, dDiffY4, dThetaAngle4, dThetaAngleInRadian4);

		if ( (lRHS1DieX - lLHS1DieX) < 0 )
		{
			ReverseAngle(dThetaAngle1, dThetaAngleInRadian1);
		}
		if ( (lRHS2DieX - lLHS2DieX) < 0 )
		{
			ReverseAngle(dThetaAngle2, dThetaAngleInRadian2);
		}
		if ( (lRHS3DieX - lLHS3DieX) < 0 )
		{
			ReverseAngle(dThetaAngle3, dThetaAngleInRadian3);
		}
		if ( (lRHS4DieX - lLHS4DieX) < 0 )
		{
			ReverseAngle(dThetaAngle4, dThetaAngleInRadian4);
		}
	}
	else if ( (ulWalkPath == BT_TRV_PATH) || (ulWalkPath == BT_TLV_PATH))//leo please check that TLV and TRV should put together
	{
		dNewCheckRealignmentAngle = fabs(atan(76.2 / ConvertYEncoderValueToFileUnit(_round(dDiffY1))) * 0.61) * 180.0 /PI;
		CalcAngle(dDiffX1, dDiffY1, dThetaAngle1, dThetaAngleInRadian1);
		CalcAngle(dDiffX2, dDiffY2, dThetaAngle2, dThetaAngleInRadian2);
		CalcAngle(dDiffX3, dDiffY3, dThetaAngle3, dThetaAngleInRadian3);
		CalcAngle(dDiffX4, dDiffY4, dThetaAngle4, dThetaAngleInRadian4);

		if ( (lRHS1DieX - lLHS1DieX) > 0 )
		{
			ReverseAngle(dThetaAngle1, dThetaAngleInRadian1);
		}
		if ( (lRHS2DieX - lLHS2DieX) > 0 )
		{
			ReverseAngle(dThetaAngle2, dThetaAngleInRadian2);
		}
		if ( (lRHS3DieX - lLHS3DieX) > 0 )
		{
			ReverseAngle(dThetaAngle3, dThetaAngleInRadian3);
		}
		if ( (lRHS4DieX - lLHS4DieX) > 0 )
		{
			ReverseAngle(dThetaAngle4, dThetaAngleInRadian4);
		}
	}


	//Check if pairs dies could not be PR //leo 20171105
	LONG lIndexFlag = 0;
	if (!bIndex1PRResult[0] || !bIndex2PRResult[3])
	{
		dThetaAngle1 = 0.0;
		dThetaAngleInRadian1 = 0.0;
		lIndexFlag++;
	}
	if (!bIndex1PRResult[1] || !bIndex2PRResult[2])
	{
		dThetaAngle2 = 0.0;
		dThetaAngleInRadian2 = 0.0;
		lIndexFlag++;
	}
	if (!bIndex1PRResult[2] || !bIndex2PRResult[1])
	{
		dThetaAngle3 = 0.0;
		dThetaAngleInRadian3 = 0.0;
		lIndexFlag++;
	}
	if (!bIndex1PRResult[3] || !bIndex2PRResult[0])
	{
		dThetaAngle4 = 0.0;
		dThetaAngleInRadian4 = 0.0;
		lIndexFlag++;
	}
	
	DOUBLE dThetaAngle = 0, dThetaAngleInRadian = 0;

	if (lIndexFlag <= 2)
	{
		DOUBLE dLargestAngle = dThetaAngle1;
		DOUBLE dLargestAngleInRadian = dThetaAngleInRadian1;
		if (fabs(dThetaAngle2) > fabs(dLargestAngle))
		{
			dLargestAngle = dThetaAngle2;
			dLargestAngleInRadian = dThetaAngleInRadian2;
		}
		if (fabs(dThetaAngle3) > fabs(dLargestAngle))
		{
			dLargestAngle = dThetaAngle3;
			dLargestAngleInRadian = dThetaAngleInRadian3;
		}
		if (fabs(dThetaAngle4) > fabs(dLargestAngle))
		{
			dLargestAngle = dThetaAngle4;
			dLargestAngleInRadian = dThetaAngleInRadian4;
		}

		dThetaAngle = (dThetaAngle1 + dThetaAngle2 + dThetaAngle3 + dThetaAngle4 - dLargestAngle) / (3.0-lIndexFlag);
		dThetaAngleInRadian = (dThetaAngleInRadian1 + dThetaAngleInRadian2 + dThetaAngleInRadian3 + dThetaAngleInRadian4 - dLargestAngleInRadian) / (3.0-lIndexFlag);

	}
	else if (lIndexFlag == 3)
	{
		dThetaAngle = (dThetaAngle1 + dThetaAngle2 + dThetaAngle3 + dThetaAngle4) / (1.0);
		dThetaAngleInRadian = (dThetaAngleInRadian1 + dThetaAngleInRadian2 + dThetaAngleInRadian3 + dThetaAngleInRadian4) / (1.0);
	}
	else
	{
		szLog.Format("FindLastRowColAngle is fail due to no pairs = %d", lIndexFlag);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		return FALSE;
	}

	szLog.Format("FindLastRowColAngle (radian): Angle1=%.6f, Angle2=%.6f, Angle3=%.6f, Angle4=%.6f : AVG = %.6f", 
	dThetaAngleInRadian1, dThetaAngleInRadian2, dThetaAngleInRadian3, dThetaAngleInRadian4, dThetaAngleInRadian);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	szLog.Format("FindLastRowColAngle (degree): Angle1=%.6f, Angle2=%.6f, Angle3=%.6f, Angle4=%.6f - AVG = %.6f", 
	dThetaAngle1, dThetaAngle2, dThetaAngle3, dThetaAngle4, dThetaAngle);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");


	//==========================================================================================
	//Get the coordinate of current die after rotate
	LONG lTempEncX1 = 0, lTempEncY1 = 0;
	GetXYEncoderValue(lTempEncX1, lTempEncY1);

	dNewLastRowColAngle = -1 * dThetaAngleInRadian;	//return angle (in rad) to main fcn

	Rotate(m_lBinCalibX, m_lBinCalibY, -1 * dNewLastRowColAngle, lTempEncX1, lTempEncY1);
	//==========================================================================================

	/////////////////////////////////////////
	// Rotate global angle by its Average
	T_MoveByDegree(-1 * dThetaAngle);
	dNewLastRowColAngle = -1 * dThetaAngleInRadian;	//return angle (in rad) to main fcn

	//==========================================================================================
	//move to new coordinate and search PR,
	//then recalculate the translate matrix(lXOffset, lYOffset) after rotate dNewLastRowColAngle for 1st die
	//==========================================================================================
	BT_MoveTo(lTempEncX1, lTempEncY1);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
	Sleep(m_lAlignBinDelay);

	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX1 position 3", "a+");
		return FALSE;
	}
		
	LONG lTempEncX2 = 0, lTempEncY2 = 0;
	GetXYEncoderValue(lTempEncX2, lTempEncY2);

	double dTempLogicalEncX2, dTempLogicalEncY2;
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulCurIndex, dTempLogicalEncX2, dTempLogicalEncY2);

	LONG lTempLogicalEncX2 = ConvertFileUnitToXEncoderValue(dTempLogicalEncX2);
	LONG lTempLogicalEncY2 = ConvertFileUnitToXEncoderValue(dTempLogicalEncY2);

	lXOffset = -ConvertXEncoderValueToFileUnit(lTempEncX2 - lTempLogicalEncX2);
	lYOffset = -ConvertYEncoderValueToFileUnit(lTempEncY2 - lTempLogicalEncY2);

	lNewXOffset = lXOffset;
	lNewYOffset = lYOffset;
	//==========================================================================================

	szLog.Format("FindLastRowColAngle (Finisar): New Global-T Angle = %.6f radian (%.6f deg), XY offset = (%d, %d) um\n", 
		dNewLastRowColAngle, -1*dThetaAngle, lNewXOffset, lNewYOffset);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	if (!bDoCenterLineCompensate)
	{
		return TRUE;
	}

	//Move to Center die and get the average offset in X direction and Y direction
	ulCurIndex = _round((ulIndex1 + ulIndex2) / 2.0);

//	double dIndexUmX = 0, dIndexUmY = 0;
//	m_oBinBlkMain.StepDMove(ulBlkInUse, ulCurIndex, dIndexUmX, dIndexUmY);
//	LONG lLogicalIndexX = ConvertFileUnitToXEncoderValue(dIndexUmX);
//	LONG lLogicalIndexY = ConvertFileUnitToXEncoderValue(dIndexUmY);

//	double dNewAngle1 = dNewAngle - dNewLastRowColAngle;
//	ConvertBinTableDPosn(dIndexUmX, dIndexUmY, ulBlkInUse, dNewAngle1, lXOffset, lYOffset);
//	LONG lPhysicalIndexX = ConvertFileUnitToXEncoderValue(dIndexUmX);
//	LONG lPhysicalIndexY = ConvertFileUnitToYEncoderValue(dIndexUmY);

//	BT_MoveTo(lPhysicalIndexX, lPhysicalIndexY);
//	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
//	Sleep(m_lAlignBinDelay);	

//	if (SearchGoodDie() == FALSE)
//	{
//		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at Update INDEX ", "a+");
//		return FALSE;
//	}

	double dNewAngle1 = dNewAngle - dNewLastRowColAngle;
	LONG lRet = 0;
	if ((lRet = ConfirmMovetoLastRow(ulBlkInUse, ulCurIndex, dNewAngle1, lXOffset, lYOffset, 0, 0, ulCurBinMapPath, 
									 alIndex1Physical_x, alIndex1Physical_y, bIndex1PRResult,TRUE)) > 0)
	 {
		 if (lRet == 1)//No Retry
		 {
			 ulCurIndex = ulCurIndex - 1;
			 CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Current Index - 1 to find last row", "a+");
			 if ((lRet = ConfirmMovetoLastRow(ulBlkInUse, ulCurIndex, dNewAngle1, lXOffset, lYOffset, 0, 0, ulCurBinMapPath, 
									 alIndex1Physical_x, alIndex1Physical_y, bIndex1PRResult)) > 0)
			 {
				 CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Find Center Die Fails(No Retry)", "a+");
				 return lRet;
			 }
		 }
		 else
		 {
			 CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Find Center Die Fails", "a+");
			 return lRet;
		 }
	 }

	GetXYEncoderValue(lTempEncX2, lTempEncY2);
	//the get the average offsetXY at X&Y direction
	//X Direction
	LONG lRowDir = 0, lColDir = 0;
	CLookAheadMap clPRLookAheadDirection;
	GetLookAheadDirection(ulCurBinMapPath, 0, 0, clPRLookAheadDirection);
	LONG lAvgOffsetX = 0, lAvgOffsetY = 0;
	GetLookAheadPRAverageOffset(ulBlkInUse, ulCurIndex, clPRLookAheadDirection.m_ulLookAheadDirection, lTempEncX2, lTempEncY2, lXOffset, lYOffset, lRowDir, lColDir, 3, FALSE, lAvgOffsetX, lAvgOffsetY);
	GetLookAheadValidData(clPRLookAheadDirection.m_ulLookAheadDirection, lAvgOffsetX, lAvgOffsetY, lNewXOffset, lNewYOffset);


	//Y Direction
	BOOL bFrameRotate180 = m_oBinBlkMain.GetFrameRotation(ulBlkInUse);
	ULONG ulPRLookAheadYDirection = GetLookAheadYDirectionWtihPreviousDie(ulCurBinMapPath, bFrameRotate180, lRowDir, lColDir);
	GetLookAheadPRAverageOffset(ulBlkInUse, ulCurIndex, ulPRLookAheadYDirection, lTempEncX2, lTempEncY2, lXOffset, lYOffset, lRowDir, lColDir, 3, TRUE, lAvgOffsetX, lAvgOffsetY);
	GetLookAheadValidData(ulPRLookAheadYDirection, lAvgOffsetX, lAvgOffsetY, lNewXOffset, lNewYOffset);

/*
	if (bUpdateOffset)
	{
		LONG lDieIndexToUpdate = ulIndex1 + 2;		//INDEX1+2
		CString szIndex = "INDEX1+3";

		if (!bIndex1PRResult[2])
		{	
			if (bIndex1PRResult[3] == 0)
			{
				lDieIndexToUpdate = ulIndex1 + 3;		//INDEX1+3
				szIndex = "INDEX1+3";
			}
			else if (!bIndex2PRResult[0] == 0)
			{
				lDieIndexToUpdate = ulIndex2 - 3;		//INDEX2-3
				szIndex = "INDEX2-3";
			}
			else if (bIndex2PRResult[1] == 0)
			{
				lDieIndexToUpdate = ulIndex2-2;		//INDEX2-2
				szIndex = "INDEX2-2";
			}
		}

		szLog.Format("FindLastRowColAngle (UpdateXYOffset): use %s = %ld (%ld, %ld, %ld, %ld)", 
			szIndex, lDieIndexToUpdate, bIndex1PRResult[2], bIndex1PRResult[3], bIndex2PRResult[0], bIndex2PRResult[1]);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		double dIndexUmX = 0, dIndexUmY = 0;
		m_oBinBlkMain.StepDMove(ulBlkInUse, lDieIndexToUpdate, dIndexUmX, dIndexUmY);
		LONG lLogicalIndexX = ConvertFileUnitToXEncoderValue(dIndexUmX);
		LONG lLogicalIndexY = ConvertFileUnitToXEncoderValue(dIndexUmY);

		dNewAngle += -1 * dNewLastRowColAngle;
		ConvertBinTableDPosn(dIndexUmX, dIndexUmY, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
		LONG lPhysicalIndexX = ConvertFileUnitToXEncoderValue(dIndexUmX);
		LONG lPhysicalIndexY = ConvertFileUnitToYEncoderValue(dIndexUmY);

//		LONG lPhysicalIndexX = ConvertFileUnitToXEncoderValue(dIndexUmX);
//		LONG lPhysicalIndexY = ConvertFileUnitToYEncoderValue(dIndexUmY);

//		Rotate(m_lBinCalibX, m_lBinCalibY, dNewAngle, lPhysicalIndexX, lPhysicalIndexY);
//		lPhysicalIndexX += ConvertFileUnitToXEncoderValue(lXOffset);
//		lPhysicalIndexY += ConvertFileUnitToYEncoderValue(lYOffset);

		BT_MoveTo(lPhysicalIndexX, lPhysicalIndexY);
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(m_lAlignBinDelay);	

		if (SearchGoodDie() == FALSE)
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX1 position 3", "a+");
			return FALSE;
		}

		//calculate the New Translate Matrix after rotate dNewLastRowColAngle again
		LONG lEncX=0, lEncY=0;
		GetXYEncoderValue(lEncX, lEncY);

		LONG lIndexOffsetX = -ConvertXEncoderValueToFileUnit(lEncX - lLogicalIndexX);
		LONG lIndexOffsetY = -ConvertYEncoderValueToFileUnit(lEncY - lLogicalIndexY);

		szLog.Format("Use %s DieOffset XY: orig XYOffset (%ld, %ld); new XYOffset (%ld, %ld)", 
							szIndex, lXOffset, lYOffset, lIndex1OffsetX, lIndex1OffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		lNewXOffset = lIndexOffsetX;
		lNewYOffset = lIndexOffsetY;
	}
*/

	return TRUE;
}


//==========================================================================================================================================
//								Called by Bin Loader during autobond
//==========================================================================================================================================

//========================================================================================
// Function Name: 		SetRealignBinFrameReq
// Input arguments:		bRealignBinFrameReq -- do realign or not after BL ready
//						ulRealignBinBlkID ---- realign bin block ID
//						bUseEmpty		  ---- realign frame is empty or not
//						bUseBT2			  ---- realign frame is in BT1 or BT2
// Output arguments:	None
// Description:   		set the parameters for realign after bin loader ready
// Return:				None
// Remarks:				None
//========================================================================================
VOID CBinTable::SetRealignBinFrameReq(const BOOL bRealignBinFrameReq, const ULONG ulRealignBinBlkID, const BOOL bUseEmpty, const BOOL bUseBT2)
{
	m_bRealignBinFrameReq	= bRealignBinFrameReq;
	m_ulRealignBinBlkID		= ulRealignBinBlkID;
	m_bRealignEmptyFrame	= bUseEmpty;
	m_bRealignUseBT2		= bUseBT2;
}


//========================================================================================
// Function Name: 		DoRealignBinFrame
// Input arguments:		None
// Output arguments:	None
// Description:   		do realign frame after bin loader ready
// Return:				None
// Remarks:				None
//========================================================================================
LONG CBinTable::DoRealignBinFrame()
{
	LONG lReturn = TRUE;

	if (!m_bRealignBinFrameReq)
	{
		return lReturn;
	}

	if (m_bRealignEmptyFrame)
	{
		if (CMS896AStn::m_bEnableEmptyBinFrameCheck == FALSE)
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("start RealignEmptyFrameCmd", "a+");
			lReturn = SubRealignEmptyFrameCmd(m_ulRealignBinBlkID);
		}
		else
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("start ExArmRealignEmptyFrameWithPRCmd", "a+");
			lReturn = SubExArmRealignEmptyFrameWithPRCmd(m_ulRealignBinBlkID);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("complete ExArmRealignEmptyFrameWithPRCmd", "a+");
		}
	}
	else
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("start RealignPhysicalBlockCmd", "a+");
//		if (m_bRealignUseBT2)
//		{
//			lReturn = SubRealignPhysicalBlock2Cmd(m_ulRealignBinBlkID);
//		}
//		else 
		{
			lReturn = SubRealignPhysicalBlockCmd(m_ulRealignBinBlkID);
		}
	}

	m_bRealignBinFrameReq	= FALSE;
	m_ulRealignBinBlkID		= 0;
	m_bRealignEmptyFrame	= FALSE;
	m_bRealignUseBT2		= FALSE;

	return lReturn;
}


LONG CBinTable::BPR_ZoomSensor(LONG lZoomMode)
{
	LONG lOldZoomMode = -1;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(MS896A_FUNC_VISION_BT_REALIGN_FF_MODE))
	{
		return lOldZoomMode;
	}

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		lOldZoomMode = pBondPr->BPR_GetNmlZoom();
		pBondPr->BPR_LiveViewZoom(lZoomMode);

		if (lZoomMode == CPrZoomSensorMode::PR_ZOOM_MODE_FF)
		{
			pBondPr->BPR_ZoomScreen(-2);
		}
		else
		{
			pBondPr->BPR_ZoomScreen(0);
		}
	}

	return lOldZoomMode;
}

BOOL CBinTable::BPR_IsSensorZoomFFMode()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->GetFeatureStatus(MS896A_FUNC_VISION_BT_REALIGN_FF_MODE))
	{
		return FALSE;
	}

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (pBondPr != NULL)
	{
		return pBondPr->IsSensorZoomFFMode();
	}

	return FALSE;
}