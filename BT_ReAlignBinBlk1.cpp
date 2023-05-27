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


LONG CBinTable::BT_MoveTo(LONG lX, LONG lY, CONST BOOL bWait, BOOL bUseBT2)
{
	CString str;
	INT nProf_X, nProf_Y;
	LONG lDiff_X, lDiff_Y;

	if (bUseBT2)	//v4.17T5
	{
		return BT2_MoveTo(lX, lY, bWait);
	}

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

	//Sleep(20);	//v2.93T2
	return TRUE;
}

VOID CBinTable::GetXYEncoderValue(LONG &lX, LONG &lY, BOOL bUseBT2)
{
	Sleep(10);
	//lX	= GetHipecAcServo("srvBinTableX")->GetEncoderPosition();
	//lY	= GetHipecAcServo("srvBinTableY")->GetEncoderPosition();

	if (bUseBT2)
	{
		lX = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_X2, 1, &m_stBTAxis_X2) - m_lBT2OffsetX;
		lY = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_Y2, 1, &m_stBTAxis_Y2) - m_lBT2OffsetY;
	}
	else
	{
		lX = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_X, 1, &m_stBTAxis_X);
		lY = CMS896AStn::MotionGetEncoderPosition(BT_AXIS_Y, 1, &m_stBTAxis_Y);
	}
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
	nConvID = m_comClient.SendRequest("WaferPrStn", "SwitchToTarget", stMsg);
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
	nConvID = m_comClient.SendRequest("WaferPrStn", "SwitchToTarget", stMsg);
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
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_GetPRStatus", stMsg);
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

	stInfo.lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	stInfo.lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));

	stMsg.InitMessage(sizeof(BT_PITCH), &stInfo);
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_GetFOVSize", stMsg);
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
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_GetDieSize", stMsg);
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


LONG CBinTable::MultiSearchFirstGoodDie(ULONG ulBlkInUse, BOOL bDoGlobalTheta) //4.51D20
{
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
	LONG lX, lY,lPitchX, lPitchY;
	BOOL bEmptyDieInFov = FALSE;
	BOOL bIs1stDie		= FALSE;
	BOOL bDoubleCheckIs1stDie = FALSE; //20170824 Leo utilze the checking feature of 1st die
	LONG lCurX			= 0;
	LONG lCurY			= 0;
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;
	//20170824 Leo added checking for bRotate180 and walkpath
	BOOL bRotate180 = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"];
	ULONG ulWalkPath = (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"];

	GetEncoderValue();		// Get the encoder value from hardware
	(*m_psmfSRam)["BinTable"]["Current"]["X"] = m_lEnc_X;
	(*m_psmfSRam)["BinTable"]["Current"]["Y"] = m_lEnc_Y;
	


	szMsg.Format("\n\n\n\t\t\tSearch --- Original EncCur(%d,%d)", m_lEnc_X, m_lEnc_Y); 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	//Search any die on current BT position
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_MultiSearchFirstDie", stMsg);

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

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));


	LONG AllowedPitchX = 70; //50 20170824 Leo
	LONG AllowedPitchY = 70; //50 20170824 Leo

	szMsg.Format("Search(Step2) -- Temp 1st Die --- bResult:%d, Pitch(%d,%d), Fovsize(%f,%f), Step(%d,%d) vs AllowedPitch(%d,%d)", stInfo.bResult, lPitchX, lPitchY, stInfo.dRowFovSize, stInfo.dColFovSize, stInfo.siStepX, stInfo.siStepY, AllowedPitchX, AllowedPitchY ); 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
	Sleep(150);
	GetXYEncoderValue(lX, lY);
	lCurX = lX + stInfo.siStepX; // For Comparing two final searched die moving whether same or not
	lCurY = lY + stInfo.siStepY;
	
	// Comparing the PR centre offset with Allowed pitch --- here is main idea that decide the 1st Die
	if( abs(stInfo.siStepX) < AllowedPitchX && abs(stInfo.siStepY) < AllowedPitchY)
	{
		bIs1stDie = TRUE;
		
		//Save Final posn then moving to check outside FOV
		if( m_lDoubleCheckFirstDie != 1)
		{
			Sleep(100);
			GetXYEncoderValue(lX, lY);
			m_lTempMultiSearchX = lX + stInfo.siStepX;
			m_lTempMultiSearchY = lY + stInfo.siStepY;
		}
	}
	else
	{
		bIs1stDie = FALSE;
		
	}


	if (stInfo.nDieStateRight == 0 && stInfo.nDieStateLeft == 0	&& stInfo.nDieStateUp == 0 && stInfo.nDieStateDown == 0 )
	{
		bIs1stDie = FALSE;
		bEmptyDieInFov = TRUE;
	}
	else
	{
		bEmptyDieInFov = FALSE;
	}
	//moving to check the outside FOV die
	if(m_lDoubleCheckFirstDie == 0 && bIs1stDie)
	{
		LONG lTempX, lTempY;
		DOUBLE dRowFov , dColFov;

		Sleep(100);
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

		//20170822 add multisearch for ms90
		/*if (!IsMS90()) // just keep the original code from David Choi for not MS90 machine
		{
			lTempX = lX + +lPitchX*(1)*((LONG)dRowFov); // (1) : move to left
			lTempY = lY + lPitchY*(-1)*((LONG)dColFov);
		}*/
		if (ulWalkPath==0)
		{
			lTempX = lX + lPitchX*(-1)*((LONG)dRowFov);
			lTempY = lY + lPitchY*(-1)*((LONG)dColFov);
		}
		else if (ulWalkPath==6)
		{
			lTempX = lX + lPitchX*(1)*((LONG)dRowFov);
			lTempY = lY + lPitchY*(1)*((LONG)dColFov);
		}
		else
		{
			lTempX = lX + lPitchX*(1)*((LONG)dRowFov);
			lTempY = lY + lPitchY*(1)*((LONG)dColFov);
		}

		BT_MoveTo(lTempX ,lTempY );

		szMsg.Format("Rotate180?= (%d), Search(Check) --Enc(%d,%d), Fov(%d,%d), Move(%d,%d), Diff(%d,%d) over3",bRotate180,lX,lY,(LONG)dRowFov,(LONG)dColFov, lTempX, lTempY,lTempX -lX, lTempY - lY); 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

		m_lDoubleCheckFirstDie++;

		return MultiSearchFirstGoodDie(ulBlkInUse,bDoGlobalTheta);

	}
	else if( m_lDoubleCheckFirstDie == 1 && bEmptyDieInFov)
	{
		bIs1stDie = TRUE;
		m_lDoubleCheckFirstDie++;
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search --- After Cheacking, No any Die", "a+");
	}
	else if( m_lDoubleCheckFirstDie == 1 && !bEmptyDieInFov)
	{
		m_lDoubleCheckFirstDie = 0;
		szMsg.Format("Search --- After Cheacking,Have Dies, lastMove(%d,%d), CurMove(%d,%d)",m_lTempMultiSearchX,m_lTempMultiSearchY,lCurX,lCurY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

		if( abs(m_lTempMultiSearchX - lCurX) < 50 && abs(m_lTempMultiSearchY - lCurY) < 50)
		{
			bIs1stDie = TRUE;

			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search --- After Comparing movement, last and current MovePosn  is the same", "a+");

		}
		//20170824 Leo Enhance DoubleCheckFirst Die 
		if (ulWalkPath == 0 && stInfo.nDieStateLeft  == 0 && stInfo.nDieStateUp == 0 && stInfo.nDieStateRight == 1 && stInfo.nDieStateDown == 1)
		{	bDoubleCheckIs1stDie = TRUE;
		}
		else if (ulWalkPath == 6 && stInfo.nDieStateLeft  == 1 && stInfo.nDieStateUp == 1 && stInfo.nDieStateRight == 0 && stInfo.nDieStateDown == 0)
		{	bDoubleCheckIs1stDie = TRUE;
		}
		else
		{	bDoubleCheckIs1stDie = FALSE;
		}
		bIs1stDie = bDoubleCheckIs1stDie;
		szMsg.Format("Double Confirm 1st Die?, %d",bDoubleCheckIs1stDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

	}
		

	if (bIs1stDie)
	{
		//GetXYEncoderValue(lX, lY);
		//BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
		BT_MoveTo(m_lTempMultiSearchX, m_lTempMultiSearchY);
		szMsg.Format("Search(Final) -- Result of 1st Die --- Enc(%d,%d), Step(%d,%d), Move(%d,%d)",lX,lY,stInfo.siStepX,stInfo.siStepY, m_lTempMultiSearchX, m_lTempMultiSearchY); 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		Sleep(100);
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
		
				DOUBLE dRowFov , dColFov;
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
				
				return MultiSearchFirstGoodDie(ulBlkInUse,bDoGlobalTheta);
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
			LONG lTempX, lTempY;
			DOUBLE dRowFov , dColFov;
			Sleep(100);
			GetXYEncoderValue(lX, lY);

			m_lTempMultiSearchX = lX + stInfo.siStepX;
			m_lTempMultiSearchY = lY + stInfo.siStepY;


			//dRowFov = stInfo.dRowFovSize/2;  
			//dColFov	= stInfo.dColFovSize/2;
			//lTempX = lX + stInfo.siStepX +lPitchX*(-1)*((LONG)dRowFov); 
			//lTempY = lY + stInfo.siStepY+ lPitchY*(-1)*((LONG)dColFov);

			lTempX = lX + stInfo.siStepX ;
			lTempY = lY + stInfo.siStepY;

			BT_MoveTo(lTempX ,lTempY );
			
			szMsg.Format("Search(Step3) -- FOV have Dies again %d th -- Move(%d,%d),Fov(%d,%d),Diff(%d,%d)",m_nMultiSeachCounter, lTempX,lTempY, (LONG)dRowFov,(LONG)dColFov, lTempX -lX,  lTempY -lY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
			
			if( m_nMultiSeachCounter <= 15)
			{
				return MultiSearchFirstGoodDie(ulBlkInUse,bDoGlobalTheta);
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


// capture die image at bin table current position, then judge the result.
LONG CBinTable::SearchFirstGoodDie(ULONG ulBlkInUse, BOOL bDoGlobalTheta)
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

	int nConvID = 0;
	LONG lX, lY;
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;


	//Search any die on current BT position
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
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
		LONG lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
		LONG lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));

		for (i = 1; i < 5; i++)
		{
			GetXYEncoderValue(lX, lY);
			BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR);
			Sleep(50);

			nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
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

				nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
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

				nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
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

				nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
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

				nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
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


	//Search any die on current BT position
	//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
	//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDie", stMsg); //20170824 Leo
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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
		LONG lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
		LONG lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));
		LONG lStepX = 0, lStepY = 0;

		for (i = 1; i < 5; i++)
		{
			GetXYEncoderValue(lX, lY);
			
			//v4.50A21
			//BT_MoveTo(lX - lPitchX * BT_X_DIR, lY - lPitchY * BT_Y_DIR);
			lStepX = lX - 0.5 * lPitchX * BT_X_DIR;
			lStepY = lY - 0.5 * lPitchY * BT_Y_DIR;
			BT_MoveTo(lStepX, lStepY);

			//v4.51A14
			szLog.Format("Srch1stDie: BT START pos #%d = (%ld, %ld)", i, lStepX, lStepY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			Sleep(m_lAlignBinDelay/2); //20170824 Leo original sleep(50)

			//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
			nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo Search Area Larger
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo Search Area Larger
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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

					//nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchFirstDie", stMsg);
					nConvID = m_comClient.SendRequest("BondPrStn", "BT_LargeAreaSearchDie", stMsg); //20170824 Leo
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

	INT i = 0;
	int nConvID = 0;
	IPC_CServiceMessage stMsg;
	stInfo.bResult = FALSE;
	LONG lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	LONG lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));
	ULONG ulWalkPath	= m_oBinBlkMain.GrabWalkPath(ulBlkInUse);			//5432123
	BOOL bUseWafflePad	= m_oBinBlkMain.GrabIsEnableWafflePad(ulBlkInUse);	//v4.58A5	//WolfSpeed
	//v4.59A26
	ULONG ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);

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

	BOOL bRHSDieFound = FALSE;
	INT nPrFailCount = 0;
	LONG lX = 0,		lY = 0;
	LONG lLHSDieX = 0, lLHSDieY = 0; 
	LONG lLHSDieX2 = 0, lLHSDieY2 = 0; 
	LONG lRHSDieX = 0, lRHSDieY = 0; 
	GetXYEncoderValue(lLHSDieX, lLHSDieY);

	//5432123
	LONG lOrigX = lLHSDieX;
	LONG lOrigY = lLHSDieY;


	for (i = 0; i <= 7; i++)
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

		Sleep(50);

		if (bUseWafflePad)	//v4.58A5
			nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchRefDie2", stMsg);
		else
			nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDie", stMsg);
		
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
			if (nPrFailCount >= 1)	//v4.59A1	//MS90 wafflepad //Wolfspeed
			{
				break;
			}
		}
	}

	if (!bRHSDieFound)
	{
		SetErrorMessage("ERR: BT_FindGlobalTheta - fail to find any RHS die");
		HmiMessage_Red_Yellow("RHS die not found!", "Align Global BT Angle");
		return FALSE;
	}

	//Use 2nd die as LHS die if more than 2 dice on the row
	if ((i > 4) && (lLHSDieX2 != 0) && (lLHSDieY2 != 0))
	{
		lLHSDieX = lLHSDieX2;
		lLHSDieY = lLHSDieY2;
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
		dThetaAngle = atan2( labs(dDiffX) , labs(dDiffY) ) * 180 / PI;
		if ( (lRHSDieX - lLHSDieX) < 0 )
			dThetaAngle = dThetaAngle * -1.00;
	}
	else if ((ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH)) //leo please check that TLV and TRV should put together
	{
		dThetaAngle = atan2( labs(dDiffX) , labs(dDiffY) ) * 180 / PI;
		if ( (lRHSDieX - lLHSDieX) > 0 )
			dThetaAngle = dThetaAngle * -1.00;
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
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDie", stMsg);
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

	X_Profile(LOW_PROF1);
	Y_Profile(LOW_PROF1);

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
			strcat(scTempData, scReadData);

			//check data end character
			nLen = strlen(scReadData) - 1;
			nLen = max(nLen, 0);	//Klocwork		//v4.02T5
			
			if (scReadData[nLen] == '\n')
			{
				//Copy temp data into ReadData
				strcpy(scReadData, scTempData);
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
	strcpy(acPar, (LPCTSTR)szTemp);
	nIndex += szTemp.GetLength() + 1;
	
	//Append input box title
	szTemp.LoadString(HMB_BL_KEYIN_BARCODE);
	strcpy(&acPar[nIndex], (LPCTSTR)szTemp);
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

BOOL CBinTable::LookAround1stDie(CONST ULONG ulBinBlockID, DOUBLE dFOVSize)		//v4.50A23
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
	
	if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TLH_PATH) 
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

BOOL CBinTable::LookAround2ndDie(CONST ULONG ulBinBlockID, DOUBLE dFOVSize)		//v4.44T3
{
	if (dFOVSize < BPR_LF_SIZE) 
	{
		return TRUE;
	}

	LONG lCX=0, lCY=0;
	LONG lX=0, lY=0;

	BOOL bCDie = SearchGoodDieXYinFOV(lCX, lCY, BT_PR_CENTER);		//Latch or first image that has to be grabbed
	
	if (m_oBinBlkMain.GrabWalkPath(ulBinBlockID) == BT_TLH_PATH) 
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
	//if ((ulWalkingPath == BT_TLV_PATH) || (ulWalkingPath == BT_TRV_PATH))
	//{
	//	szLog.Format("1st+1 Die XY - disabled due to invalid walking path (%lu)", ulWalkingPath);
	//	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	//	return FALSE;
	//}
	

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


		LONG lPitchXinStep = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBinBlockID)));
		LONG lPitchYinStep = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBinBlockID));

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


		LONG lPitchXinStep = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBinBlockID)));
		LONG lPitchYinStep = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBinBlockID));

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
	
	//szLog = "END 1st+1 Die XY - XY Check completed & OK";
	//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	return TRUE;
}


BOOL CBinTable::Check3rdGoodDieRelOffsetXYinFOV(CONST ULONG ulBinBlockID, DOUBLE dFOVSize, 
		CString &szError, BOOL &bTopDieOK, BOOL &bSideDieOK)
{
	CString szLog;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//if (pApp->GetCustomerName() != "Semitek")
	//{
	//	szError = "OK";
	//	return TRUE;
	//}


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
	//if ((ulWalkingPath == BT_TLV_PATH) || (ulWalkingPath == BT_TRV_PATH))
	//{
	//	szLog.Format("RelOffset XY Check - disabled due to invalid walking path (%lu)", ulWalkingPath);
	//	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
	//	szError = "OK";
	//	return FALSE;
	//}


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
	ULONG ulPitchXInEnc = labs(ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBinBlockID)));	//in ENC
	ULONG ulPitchYInEnc = labs(ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBinBlockID)));	//in ENC


	if (bTopDie)
	{
		dX		= labs(lTX);
		//dTopY	= labs(labs(lTY));// - ulPitchYInEnc); TY is relative to centre
		dTopY	= labs(labs(lTY) - labs(ulPitchYInEnc));
	}
	else if (bDownDie)	//v4.59A31
	{
		dX		= labs(lDX);
		dTopY	= labs(labs(lDY) - labs(ulPitchYInEnc));// - ulPitchYInEnc);	
	}

	if (bLeftDie || bRightDie)
	{
		dSideX	= labs(labs(lSX) - labs(ulPitchXInEnc));		//v4.59A33	//LeoLam 20171109
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
		szLog.Format("RelOffset XY (LEFT Die) - X Offset = %.2f mil (SX = %ld steps); Tol = %.2f mil", dSideXinMil, lSX, m_dAlignLastDieRelOffsetX);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	}
	else
	{
		szLog.Format("RelOffset XY (RIGHT Die) - Y Offset = %.2f mil (SY = %ld steps); Tol = %.2f mil", dYinMil, lSY, m_dAlignLastDieRelOffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		
		szLog.Format("RelOffset XY (TOP Die) - X Offset = %.5f mil (TX = %ld steps); Tol = %.2f mil", dXinMil, lTX, m_dAlignLastDieRelOffsetX);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		szLog.Format("RelOffset XY (RIGHT Die) - X Offset = %.5f mil (SX = %ld steps); Tol = %.2f mil", dSideXinMil, lSX, m_dAlignLastDieRelOffsetX);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

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


//Nichia//v4.43T7
//LONG CBinTable::Search3rdGoodDie(BOOL &bPlacement, DOUBLE &dX, DOUBLE &dY)
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

	// Get the reply
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDie", stMsg);
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

	if (!stInfo.bResult)
	{
		return FALSE;
	}


	//Convert motor into mil	
	double dXinMil = 0, dYinMil = 0;
	dXinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * (DOUBLE)stInfo.siStepX * 1000 / 25.4);
	dYinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y) * (DOUBLE)stInfo.siStepY * 1000 / 25.4);
	dX = stInfo.siStepX;	//dXinMil;
	dY = stInfo.siStepY;	//dYinMil;

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
	if (bUseBT2)
	{
		nConvID = m_comClient.SendRequest("BondPrStn", "BT2_SearchDie", stMsg);    //v4.35T4
	}		
	else
	{
		nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDie", stMsg);
	}
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
		if (bMove)
		{
			if (bUseBT2)
			{
				GetXYEncoderValue(lX, lY, TRUE);					//Returned XY encoder values are based on BT1, not BT2
				BT2_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);	//BT1 values are passed into this fcn for further transformation inside it
			}
			else
			{
				GetXYEncoderValue(lX, lY);
				BT_MoveTo(lX + stInfo.siStepX, lY + stInfo.siStepY);
			}
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CBinTable::SearchGoodDieinFOV(LONG lDirection)
{
	typedef struct 
	{
		LONG	lDirection;
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
	stInfo.bResult = FALSE;
	stInfo.iPrX = 0;//nPrOffsetX;
	stInfo.iPrY = 0;//nPrOffsetY;
	stMsg.InitMessage(sizeof(BPR_SrchDie), &stInfo);

	nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDieInFOV", stMsg);
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

	bIsGoodDie = stResult.bResult;			//v4.26T1	//Semitek
	return bIsGoodDie;
}


BOOL CBinTable::SearchGoodDieXYinFOV(LONG &lX, LONG &lY, LONG lDirection)
{
	typedef struct 
	{
		LONG	lDirection;
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
	stInfo.bResult = FALSE;
	stInfo.iPrX = 0;//nPrOffsetX;
	stInfo.iPrY = 0;//nPrOffsetY;
	stMsg.InitMessage(sizeof(BPR_SrchDie), &stInfo);

	nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchDieXYInFOV", stMsg);
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

/*
	BOOL bBprDisplay = bDisplay;
	stMsg.InitMessage(sizeof(BOOL), &bBprDisplay);
*/

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


	nConvID = m_comClient.SendRequest("BondPrStn", "DrawSearchWindow", stMsg);
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
}

VOID CBinTable::DisplayBondPrDieSizeWindow(BOOL bDisplayHomeCursor)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BOOL), &bDisplayHomeCursor);
	nConvID = m_comClient.SendRequest("BondPrStn", "DrawDieSizeWindow", stMsg);
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

LONG CBinTable::FindTopLeftDie(LONG lPitchX, LONG lPitchY, DOUBLE dFOVSize, BOOL bUseBT2)
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
			return TRUE;
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
				break;	//Bingo
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
				break;
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
				break;
			}
		}
	}

	return TRUE;
}

LONG CBinTable::FindTopRightDie(ULONG ulWalkPath, LONG lPitchX, LONG lPitchY, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lX, lY, lCount = 0;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(BT_PR_TOP_RIGHT) == FALSE) && (SearchGoodDieinFOV(BT_PR_TOP) == FALSE) && (SearchGoodDieinFOV(BT_PR_RIGHT) == FALSE))
		{
			//Bingo (Already on TR)
			return TRUE;
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
					break;	//Bingo
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
				break;
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
				break;
			}
		}
	}

	return TRUE;
}


LONG CBinTable::FindBottomLeftDie(ULONG ulWalkPath, LONG lPitchX, LONG lPitchY, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lX, lY, lCount = 0;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(BT_PR_BOTTOM_LEFT) == FALSE) && (SearchGoodDieinFOV(BT_PR_BOTTOM) == FALSE) && (SearchGoodDieinFOV(BT_PR_LEFT) == FALSE))
		{
			//Bingo (Already on TL)
			return TRUE;
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
					break;	//Bingo
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
					break;	//Bingo
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
				break;
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
				break;
			}
		}
	}

	return TRUE;
}


LONG CBinTable::FindBottomRightDie(LONG lPitchX, LONG lPitchY, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lX, lY, lCount = 0;

	if (dFOVSize >= BPR_LF_SIZE)
	{
		SearchGoodDieinFOV(BT_PR_CENTER);
		if ((SearchGoodDieinFOV(BT_PR_BOTTOM_RIGHT) == FALSE) && (SearchGoodDieinFOV(BT_PR_BOTTOM) == FALSE) && (SearchGoodDieinFOV(BT_PR_RIGHT) == FALSE))
		{
			//Bingo (Already on TR)
			return TRUE;
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
				break;	//Bingo
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
				break;
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
				break;
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
LONG CBinTable::SearchDieInTopLeft(ULONG ulBlkInUse, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));

	ULONG ulWalkPath = m_oBinBlkMain.GrabWalkPath(ulBlkInUse);

	// Search for top-left die with Look-forward mode
	lCounter = 0;
	lSrchStatus = FindTopLeftDie(lPitchX, lPitchY, dFOVSize, bUseBT2);

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

LONG CBinTable::SearchDieInTopRight(ULONG ulBlkInUse, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));		//v2.56
	ULONG ulWalkPath = m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse);

	// Search for top-right die with Look-forward mode
	lCounter = 0;

	lSrchStatus = FindTopRightDie(ulWalkPath, lPitchX, lPitchY, dFOVSize, bUseBT2);

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


LONG CBinTable::SearchDieInBottomLeft(ULONG ulBlkInUse, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));

	ULONG ulWalkPath = m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse);

	// Search for down-left die with Look-forward mode
	lCounter = 0;
	lSrchStatus = FindBottomLeftDie(ulWalkPath, lPitchX, lPitchY, dFOVSize, bUseBT2);

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


LONG CBinTable::SearchDieInBottomRight(ULONG ulBlkInUse, DOUBLE dFOVSize, BOOL bUseBT2)
{
	LONG lCounter;
	LONG lPitchX, lPitchY, lX, lY;
	LONG lSrchStatus = FALSE;

	lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));		//v2.56

	// Search for down-right die with Look-forward mode
	lCounter = 0;
	lSrchStatus = FindBottomRightDie(lPitchX, lPitchY, dFOVSize, bUseBT2);

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


// huga
BOOL CBinTable::AlignEmptyFrame(ULONG ulBlkID)
{
	ULONG ulFirstDieIndex = 0;
	//LONG lX = 0, lY = 0;
	DOUBLE dX=0, dY=0;
	LONG lDieFound = 0;
	CString str = "";
	BOOL bPostbond = FALSE;

	bPostbond = (BOOL)((LONG)(*m_psmfSRam)["BondPr"]["UsePostBond"]);

	CMS896AStn::m_bRealignFrameDone = FALSE;

	if (bPostbond == FALSE)
	{
		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkID);
		m_oBinBlkMain.StepDMove(ulBlkID, ulFirstDieIndex, dX, dY);	//v4.59A19

		BT_MoveTo(ConvertFileUnitToXEncoderValue(dX), ConvertFileUnitToYEncoderValue(dY));
		CMS896AStn::m_bRealignFrameDone = TRUE;
		return TRUE;
	}

	ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkID);
	m_oBinBlkMain.StepDMove(ulBlkID, ulFirstDieIndex, dX, dY);

	BT_MoveTo(ConvertFileUnitToXEncoderValue(dX), ConvertFileUnitToYEncoderValue(dY));
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	//v2.58

	if (m_lAlignBinDelay > 0)
	{
		Sleep(m_lAlignBinDelay);
	}

	if ((SearchGoodDie(FALSE) == TRUE) && (!IsBurnIn()))
	{
		str.Format("%d", ulBlkID);
		SetAlert_Msg_Red_Yellow(IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN, str);
		return FALSE;
	}

	CMS896AStn::m_bRealignFrameDone = TRUE;

	return TRUE;
}

// suppose one grade only put on one block and one block contains only one grade die
// just like auto bin loader
// not handle the empty pattern( top left no die bonded)
LONG CBinTable::RealignBinBlock(ULONG ulBlkInUse, BOOL bAuto, BOOL bAutoTeach, 
								BOOL bDisableAlarm, BOOL bDummyRun, USHORT sRetry)	//v4.59A31
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
	DOUBLE  dNewAngle, dFOVSize;
	LONG lNewXOffset, lNewYOffset;
	CString szLog;
	CString szMzg;

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
	lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));
	bOldAligned		= m_oBinBlkMain.GrabNVIsAligned(ulBlkInUse);

	szTitle.LoadString(HMB_BT_REALIGN_BIN_BLOCK);			

	if (ulNoOfSortedDie == 0)
	{
		SetJoystickOn(FALSE);	//v3.71T5

		//v2.93T2
		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
		m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex, dUmX, dUmY);	//v4.59A19
		//BT_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY), FALSE);

		//CMS896AStn::m_bEnableEmptyBinFrameCheck = TRUE;
		if (CMS896AStn::m_bEnableEmptyBinFrameCheck == FALSE)
		{
			//v4.44T5		//Semitek	//LeoLam
			LONG lPosX = ConvertFileUnitToXEncoderValue(dUmX);
			LONG lPosY = ConvertFileUnitToYEncoderValue(dUmY);
			LONG lOldX = lPosX;
			LONG lOldY = lPosY;
			//OpAddPostBondOffset(lPosX, lPosY, 3);		//v4.42T17	//Silan	//LeoLam

			//CString szLog;
			//szLog.Format("MS100(BH2-OFFSET Realign 0): BLK #%lu 1stDie - XEnc=%ld: New(%ld), YEnc=%ld: New(%ld)", 
			//			ulBlkInUse, lOldX, lPosX, lOldY, lPosY);
			//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/

			bMoveTable = BT_MoveTo(lPosX, lPosY, FALSE);		//v4.46T13	//Re-enable for Genesis MS899DLA

			if (bAuto == FALSE && !IsBurnIn())
			{
				str.Format("%d", ulBlkInUse);
				SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_ON_BLOCK, str);
			}
		}
		else
		{
			bMoveTable = BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY), TRUE);

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
				return FALSE;
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
			return FALSE;
		}

		szLog.Format("Re-align start with 0 die on bin #%lu - " + pApp->GetSoftVersion(), ulBlkInUse);	//v4.34T2
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	
		m_dFrameAlignAngleForTesting = m_oBinBlkMain.GrabNVRotateAngleX(1);	//v4.57A9

		m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);		//v2.93T2
		CMS896AStn::m_bRealignFrameDone = TRUE;
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
	if (m_bEnableBinMapBondArea || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))
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
			return FALSE;
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


	//******************************************************//
	// find first die of the grade
	// move bin table to first die of this block
	// get the the logically position of first die
	if (bNewAlignMethod && (ulNoOfSortedDie > 1))
		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);		//v4.43T13
	else
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
	else if (m_bEnableBinMapBondArea)	//v4.36
	{
		//ulFirstDieIndex = m_oBinBlkMain.Get1stDieIndexIn1stBMapRow();	//v4.36
		//v4.42T7	//Update BinMap Walk path only; does not affect 1st Die index
		m_oBinBlkMain.Get1stDieIndexIn1stBMapRow(ulBlkInUse);	

		szLog.Format("Search 1st die (blk #%d) in BINMAP: 1stIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulFirstDieIndex, ulNoOfSortedDie);

		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))		//Nichia//v4.43T7
	{
		ulFirstDieIndex = m_oBinBlkMain.Get1stDieIndexIn1stBMapRow(ulBlkInUse);	

		szLog.Format("Search 1st die (blk #%d) in BINMAP2: 1stIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulFirstDieIndex, ulNoOfSortedDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}


	m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex, dUmX, dUmY);
	DieALogical_x = ConvertFileUnitToXEncoderValue(dUmX);
	DieALogical_y = ConvertFileUnitToYEncoderValue(dUmY);
	bMoveTable = BT_MoveTo(DieALogical_x, DieALogical_y);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
	Sleep(m_lAlignBinDelay + 100);		//v4.40T5				

	//v4.58A5	//Finisar
	/*if (m_bEnableBinMapBondArea && 
		(pApp->GetCustomerName() == CTM_FINISAR) && 
		(ulNoOfSortedDie > 10))
	{
		//LONG lNewX=0, lNewY=0;
		DOUBLE dNewX=0, dNewY=0;
		m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex+5, dNewX, dNewY);	//v4.59A19

		szLog.Format("Search 1st die (blk #%d) in BINMAP (Finisar): 1stIndex=%lu, NoOfDie=%lu", 
					 ulBlkInUse, ulFirstDieIndex+5, ulNoOfSortedDie);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		LONG NewDieALogical_x = ConvertFileUnitToXEncoderValue(dNewX);
		LONG NewDieALogical_y = ConvertFileUnitToYEncoderValue(dNewY);
		bMoveTable = BT_MoveTo(NewDieALogical_x, NewDieALogical_y);
		Sleep(m_lAlignBinDelay);				
	}*/

	//v4.57A5
	if (!bMoveTable)
	{
		CString szError;
		szError = "BT MOVE is out of table limit";
		SetErrorMessage("BT_MoveTo Error 2");
		HmiMessage_Red_Back(szError, "Bin Table Error");

		m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);
		CMS896AStn::m_bRealignFrameDone = TRUE;
		SwitchToBPR();
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
				else if (m_oBinBlkMain.GrabLSBondPattern() || m_bEnableBinMapBondArea || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))
				{
					if(CMS896AApp::m_bBinMultiSearchFirstDie == FALSE) 
					{
					   lDieFound = SearchGoodDie();    //v4.35T4
					}
					else
					{
						lDieFound = MultiSearchFirstGoodDie(ulBlkInUse); //4.51D20search
						if( lDieFound == TRUE) // final search again after multi search
						{
							lDieFound = SearchFirstGoodDie(ulBlkInUse); 
							szMzg.Format("Search --- After Multi DieFound: %d  ", lDieFound );
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMzg, "a+");
						}

					}

					szMzg.Format("Search --- DieFound: %d ", lDieFound );
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMzg, "a+");

					//v4.42T7
					if (lDieFound != TRUE)
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
						lDieFound = MultiSearchFirstGoodDie(ulBlkInUse); //4.51D20search
						if( lDieFound == TRUE)
						{
							lDieFound = SearchFirstGoodDie(ulBlkInUse);  
							szMzg.Format("Search --- After Multi DieFound: %d  ", lDieFound );
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
					else if (m_oBinBlkMain.GrabLSBondPattern())
					{
						lDieFound = TRUE;
					}
					else if (m_bEnableBinMapBondArea || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))	//v4.42T7
					{
						szLog.Format("Finding die, BinMap WalkPath = %d", m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse));
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
						//lDieFound = TRUE;
						switch (m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse))
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
					}
					else
					{
						//if (!bNewAlignMethod)	//v4.43T12	//v4.48A3
						//{
							szLog.Format("Finding die, WalkPath = %d", m_oBinBlkMain.GrabWalkPath(ulBlkInUse));
							/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
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
						//}
					}	//End ELSE
				}
			}
		}
		else
		{
			lDieFound = FALSE;
		}

		if (lDieFound == FALSE)
		{
			if (!IsBurnIn() && m_bAlignBinInAlive == FALSE)
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
		if (!IsBurnIn())
		{
			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Manual locate 1st die", "a+"); /******/

			//Ask user to locate die pos, first die in first row
			SetJoystickOn(TRUE);

			szContent.LoadString(HMB_BT_REALIGN_STEP1);			
			//if (HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szContent);		//v4.43T10
			if (HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
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
	//Get 1st die physical position
	//v4.40T5
	if (bManualLocate == 0)
	{
		Sleep(m_lAlignBinDelay);		
		SearchGoodDie();	
		Sleep(100);
	}

	//Sleep(m_lAlignBinDelay);
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

				//Used in OpMoveTable() in CycleState
				m_lX = ConvertFileUnitToXEncoderValue(dUmX);
				m_lY = ConvertFileUnitToXEncoderValue(dUmY);
				Sleep(100);

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
	BOOL bLA1stDie = LookAround1stDie(ulBlkInUse, dFOVSize);
	if (!bLA1stDie)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("LookAround 1st Die fail\n", "a+");

		SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UL_DIE);
		SwitchToBPR();
		return FALSE;
	}


	//******************************************************//
	// find 1st+1 die (using PR LookForward) of first row in the grade
	LONG lNewXOffset2 = lNewXOffset;
	LONG lNewYOffset2 = lNewYOffset;
	BOOL bUse2ndOffsetXY = FALSE;

	if ( (pApp->GetCustomerName()==CTM_FINISAR) && bUsePR && (ulNoOfSortedDie > 3) )
	{
//HmiMessage("Start Searching 3rd Die .....");
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
				if (m_bEnableBinMapBondArea)
				{
					ULONG ulNoOf1stRowDices = ulDiePerRow;		//v4.48A2
					m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices);
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
			if (m_bEnableBinMapBondArea)
			{
				ULONG ulNoOf1stRowDices = ulDiePerRow;
				m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices);
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
	BOOL bBinMapResetAngle = FALSE;		//v4.39T8
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
	else if (m_bEnableBinMapBondArea || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))		//v4.36
	{
		ULONG ulNoOf1stRowDices = ulDiePerRow;

		//ulLastDieIndex = m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulNoOf1stRowDices);	//v4.36
		m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices);

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

		//Finisar1028
		//if (ulNoOfSortedDie <= ulNoOf1stRowDices)	//v4.39T8
		//{
		//	bBinMapResetAngle = TRUE;
		//}
	}

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
		//Finisar1028
		/*if (ulNoOfSortedDie < ulMinDicesToMeasureAngle)
		{
			CString szLog;
			szLog.Format("Andrew: RESET angle: min=%lu, currDie=%lu; DiePreRow=%lu", 
				ulMinDicesToMeasureAngle, ulNoOfSortedDie, ulDiePerRow);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			bBinMapResetAngle = TRUE;
		}*/
	}


	m_oBinBlkMain.StepDMove(ulBlkInUse, ulLastDieIndex, dUmX, dUmY);		//v4.59A19
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX + lNewXOffset), ConvertFileUnitToYEncoderValue(dUmY + lNewYOffset));
	LONG DieBLogical_x = ConvertFileUnitToXEncoderValue(dUmX);
	LONG DieBLogical_y = ConvertFileUnitToYEncoderValue(dUmY);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	//v2.58
	Sleep(m_lAlignBinDelay);					//v2.93T2


//if (bUse2ndOffsetXY)
//{
//AfxMessageBox("2nd-Die: before using 1st+1 Die Offset XY ...", MB_SYSTEMMODAL);
//BT_MoveTo(ConvertFileUnitToXEncoderValue(lX+lNewXOffset2), ConvertFileUnitToYEncoderValue(lY+lNewYOffset2));
//AfxMessageBox("2nd-Die: after using 1st+1 Die Offset XY", MB_SYSTEMMODAL);
//	lNewXOffset = lNewXOffset2;
//	lNewYOffset = lNewYOffset2;
//}

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
				//else
				//	lDieFound = SearchGoodDie();				//v4.35T4
				else
				{
					//v4.44T3
					//if(CMS896AApp::m_bBinMultiSearchFirstDie == FALSE)
					//{
					lDieFound = SearchGoodDie();
					//}
					//else
					//{
					//	lDieFound = MultiSearchFirstGoodDie(ulBlkInUse); //4.51D20
					//}
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
						// || m_bEnableBinMapBondArea || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))	//v4.36	//v4.49A7
					{
						lDieFound = TRUE;
					}
					else
					{
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
					}

					//v4.44T3	//SEmitek
					if (bNewAlignMethod && bUsePR)
					{
						lDieFound = LookAround2ndDie(ulBlkInUse, dFOVSize);
						szLog.Format("Look Around 2nd-die in FOV: lDieFound=%d", lDieFound);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
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
			if (!IsBurnIn() && (m_bAlignBinInAlive == FALSE))
			{
				if (!bDisableAlarm)		//v4.24T6
				{
					SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UR_DIE);
				}
				SwitchToBPR();
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

			szContent.LoadString(HMB_BT_REALIGN_STEP2);			
			//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount(szContent);		//v4.43T10
			//if (HmiMessage(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			if (HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
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

			// confirm the die
			SetJoystickOn(FALSE);
		}

		if (bUsePR == TRUE)
		{
			SearchGoodDie();
		}
	}

	/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-confirm TR die", "a+"); /******/

	//Get 2nd die physical position
	if (bLF == FALSE)
	{
		//SearchGoodDie();		//Disabled in //v3.71T9
	}

	//Sleep(m_lAlignBinDelay);	//v3.71T9
	GetXYEncoderValue(DieBPhysical_x, DieBPhysical_y);
	szLog.Format("REALIGN 2ndDie ENC-B POS XY, (%ld, %ld)", DieBPhysical_x, DieBPhysical_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	LONG lLogicalXDistance = 0;
	LONG lLogicalYDistance = 0;
	DOUBLE lLogicalAngle = 0.00;

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
			}

			//aeeyhng1030
			szLog.Format("TLV-BLV: Die ENC-A(%ld, %ld), ENC-B(%ld, %ld), lX=%ld (res=%.6f), lY=%ld (res=%.6f), Angle(radian)=%.6f", 
							DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y,
							lXDistance, m_dXResolution, lYDistance, m_dYResolution, dNewAngle);
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
			}

			//aeeyhng1030
			szLog.Format("TRV: Die ENC-A(%ld, %ld), ENC-B(%ld, %ld), lX=%ld (res=%.6f), lY=%ld (res=%.6f), Angle(radian)=%.6f", 
							DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y,
							lXDistance, m_dXResolution, lYDistance, m_dYResolution, dNewAngle);
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
			}
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("TRH", "a+");
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

				//szLog.Format("TLH  A = (%ld, %ld); B = (%ld, %ld); Angle = %.2f (%ld, %ld); Angle (logical) = %.2f", 
				//			 DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y, 
				//			 dNewAngle, lXDistance, lYDistance, lLogicalAngle);
				szLog.Format("TLH  A = (%ld, %ld); B = (%ld, %ld); Angle = %.2f (%.2f deg); Angle (logical) = %.2f", 
							 DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y, 
							 dNewAngle, (dNewAngle * 180.0 / PI), lLogicalAngle);		//v4.44A3
				
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

				//**Note: only TLH mode can support LSBondPattern	//v4.35T4
				if (m_oBinBlkMain.GrabLSBondPattern())	//|| m_bEnableBinMapBondArea)
				{
					dNewAngle = dNewAngle - lLogicalAngle;			//v4.35T4	//MS109
				}
			}
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("BLH and DEFAULT", "a+");
			break;
	}

	// bin block rotate too much
	if (m_bCheckAlignBinResult && !bBinMapResetAngle && fabs(dNewAngle) >= 0.05236)// 3 degrees	//v4.44A7
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
		}

		return FALSE;
	}

	// lPhysicalDistance = (physical b -- physical a);
	//LONG lX1, lY1, lX2, lY2;
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
		//LONG lX1, lY1, lX2, lY2;
		DOUBLE dX1, dY1, dX2, dY2;
		if (bNewAlignMethod && bUse2ndOffsetXY)		//v4.43T13	//Use 1st+1 die for calculation
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


	szLog.Format("2 Die distance (P/L) = %d,%d (XPitch = %ld x 3/4), angle = %.6f degree (radian = %.6f)", 
				 lPhyDist, lLogDist, m_oBinBlkMain.GrabDiePitchX(ulBlkInUse), dNewAngle * 180.0 / PI, dNewAngle);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
	
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
		//m_oBinBlkMain.SetIsAligned(ulBlkInUse, bOldAligned);		//v3.44

		// if physical dist between 1st and 2nd die and logical row length diff by larger than 1-pitch
		if (!IsBurnIn())
		{
			str.Format("Diff. of REALIGN distance %d and LOGICAL distance %d between 1st and 2nd die is over one-pitch!", 
				lPhyDist, lLogDist);
			/******/ 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(str + "\n", "a+"); /******/
			if (!bDisableAlarm)		//v4.24T6
			{
				SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_FAILED, str);
			}
		}
			
		SwitchToBPR();
		return FALSE;
	}

	//if (m_bEnableBinMapBondArea && bBinMapResetAngle)		//v4.39T8
	if (bBinMapResetAngle)									//v4.40T2	//EverVision
	{
		dNewAngle = 0;
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Andrew: reset angle to 0", "a+"); /******/ 
	}

	if (m_bEnableBinMapBondArea || m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))
	{
		ULONG ulNoOf1stRowDices = ulDiePerRow;
		m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(ulBlkInUse, ulNoOf1stRowDices);
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
		else if (m_bEnableBinMapBondArea)	//v4.36
		{
			ULONG ulOrigDieIndex = ulDieIndex;
			//ulDieIndex = m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, ulDieIndex);	//v4.36

			szLog.Format("Search Last die (blk #%d) in BINMAP: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

			//v4.59A31
			//******************************************************//
			// check last row angle
			if (pApp->GetCustomerName() == CTM_FINISAR)
			{
				DOUBLE dNewLastRowColAngle1 = 0.0;
				DOUBLE dNewLastRowColAngle2 = 0.0;
				LONG lXOffset = lNewXOffset;
				LONG lYOffset = lNewYOffset;
				DOUBLE dNewAngleInDegree = 0.0;

				if (FindLastRowColumnFrameAngle(ulBlkInUse, ulDieIndex, 
								dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle1))
				{
					Sleep(500);
					if (FindLastRowColumnFrameAngle(ulBlkInUse, ulDieIndex, 
									dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle2))
					{

						szLog.Format("FindLastRowColumnFrameAngle Done (in radian): OrigAngle = %.6f, 1stAngle = %.6f, 2ndAngle = %.6f", 
									dNewAngle, dNewLastRowColAngle1, dNewLastRowColAngle2);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 


						//dNewAngle = -1* (dNewLastRowColAngle1 + dNewLastRowColAngle2)/2;//leo 20171105 need the Angle2 only
						dNewAngle = -1* (dNewLastRowColAngle2);

						szLog.Format("Final FRAME Angle = %.6f (rad) or %.6f (degree)", 
									dNewAngle, dNewAngle * 180.0 / PI);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 


						szLog.Format("Final FRAME XY Offset: Orig (%ld, %ld); New (%ld, %ld)", 
									lNewXOffset, lNewYOffset, lXOffset, lYOffset);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

						lNewXOffset = lXOffset;
						lNewYOffset = lYOffset;

						LONG lLoopFindLastColAngle = 0;
						dNewAngleInDegree = fabs(dNewAngle * 180.0 /PI);
						while((dNewAngleInDegree > 0.01) && (lLoopFindLastColAngle <3))	//Loop twice for Better Alignment Angle
						{
							lLoopFindLastColAngle++;
							if (FindLastRowColumnFrameAngle(ulBlkInUse, ulDieIndex, 
											dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle1))
							{
								Sleep(500);
								if (FindLastRowColumnFrameAngle(ulBlkInUse, ulDieIndex, 
												dNewAngle, TRUE, lXOffset, lYOffset, dNewLastRowColAngle2))
								{

									szLog.Format("FindLastRowColumnFrameAngle Done (in radian): OrigAngle = %.6f, 1stAngle = %.6f, 2ndAngle = %.6f", 
												dNewAngle, dNewLastRowColAngle1, dNewLastRowColAngle2);
									CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 


									//dNewAngle = -1* (dNewLastRowColAngle1 + dNewLastRowColAngle2)/2;//leo 20171105 need the Angle2 only
									dNewAngle = -1* (dNewLastRowColAngle2);

									szLog.Format("Final FRAME Angle = %.6f (rad) or %.6f (degree)", 
												dNewAngle, dNewAngle * 180.0 / PI);
									CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 


									szLog.Format("Final FRAME XY Offset: Orig (%ld, %ld); New (%ld, %ld)", 
												lNewXOffset, lNewYOffset, lXOffset, lYOffset);
									CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
		
									lNewXOffset = lXOffset;
									lNewYOffset = lYOffset;
									dNewAngleInDegree = fabs(dNewAngle * 180.0 /PI);
									
								}
							}
						}						
					}
					else
					{
						//No handling needed at this moment
					}
				}
				else	//v4.59A33	//LeoLam 20171109
				{
					dNewAngleInDegree = dNewLastRowColAngle1; //if enter the rotate angle function return false, no update before on the angle even rotated good
					
					DOUBLE dOldAngle = dNewAngle;
					dNewAngle = -1 * (dNewLastRowColAngle1);

					szLog.Format("FindLastRowColumnFrameAngle FAIL1 but Final FRAME Angle updated = %.6f (rad) or %.6f (degree), orig = %.6f (rad)", 
								dNewAngle, dNewAngle * 180.0 / PI, dOldAngle);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
				}

				if (dNewAngleInDegree > 0.01)	//0.01 degree
				{
					CString szMsg;
					CString szTitle;
					szTitle = "Global Angle Alignment";
					szMsg.Format("Global Angle exceeds 0.01 degree (%.6f); operation is aborted", dNewAngleInDegree);		
					HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user (SanAn)\n", "a+");
					SwitchToBPR();
					//SetAlarmLamp_Yellow();
					return FALSE;
				}

			}
		}
		else if (m_oBinBlkMain.GrabEnableBinMap(ulBlkInUse))	//Nichia//v4.43T7
		{
			ULONG ulOrigDieIndex = ulDieIndex;

			ULONG lCurrIndex = 1;
			for (INT i=1; i<ulOrigDieIndex; i++)
			{
				m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, lCurrIndex);
				lCurrIndex = lCurrIndex + 1;
			}
			ulDieIndex = lCurrIndex;

			szLog.Format("Search Last die (blk #%d) in BINMAP2: NewIndex=%lu, OrigIndex=%lu", 
						 ulBlkInUse, lCurrIndex, ulOrigDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
		}


		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		//ULONG ulLastIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dUmX, dUmY, TRUE);	//v4.59A19
		l3rdDieOrigX = ConvertFileUnitToXEncoderValue(dUmX);
		l3rdDieOrigY = ConvertFileUnitToYEncoderValue(dUmY);
		
		//v4.52A8
		szLog.Format("BLK #%lu LastDie (Before comp) - XEnc=%ld, YEnc=%ld (%.2f, %ld, %ld)", 
			ulBlkInUse, l3rdDieOrigX, l3rdDieOrigY, dNewAngle, lNewXOffset, lNewYOffset);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		LONG lX1 = dUmX;
		ConvertBinTableDPosn(dUmX, dUmY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dUmX), ConvertFileUnitToYEncoderValue(dUmY));
		l3rdDieX = ConvertFileUnitToXEncoderValue(dUmX);
		l3rdDieY = ConvertFileUnitToYEncoderValue(dUmY);
		szLog.Format("BLK #%lu LastDie (Before PR) - XEnc=%ld, YEnc=%ld", ulBlkInUse, l3rdDieX, l3rdDieY);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/


		//ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse) + 1;
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + 1;

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
		else if (m_bEnableBinMapBondArea)	//v4.36
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
			for (INT i=1; i<ulOrigDieIndex; i++)
			{
				m_oBinBlkMain.GetDieIndexInBinMap(ulBlkInUse, lCurrIndex);
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
		if (bDummyRun)		//v4.31T10
		{
			bPlacementOk = TRUE;
			lDieFound1 = TRUE;
		}

//CString szLog;
//szLog.Format("Realign: Die1=%d, Place=%d, MConfirm=%d", lDieFound1, bPlacementOk, m_bManualConfirmLastDieResult1);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

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
		LONG lX2 = dUmX;

		//ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);	//LastDie + 1
		if (m_bUseDualTablesOption && bPlacementOk && !bDisableMS109LastDieOffsetX)		//v4.38T7
		{
			if (m_dXResolution != 0)
			{
				lNewXOffset = lNewXOffset - dX / m_dXResolution;
			}
			if (m_dYResolution != 0)
			{
				lNewYOffset = lNewYOffset - dY / m_dYResolution;    //v4.39T6
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
		//if (lDieFound1)
		//	lLfDieOK = LookAhead3rdGoodDie(ulBlkInUse, TRUE, dFOVSize, szLfError);

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
		
		szLog.Format("PR Reault: Last = %d; Last+1 = %d; PlacementOK = %d; Alive = %d", lDieFound1, lDieFound2, bPlacementOk, m_bAlignBinInAlive );
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
					//}
				}
				if (bNewAlignMethod && pApp->GetCustomerName() == CTM_FINISAR)
				{
					if (!bRelOffsetXYinFOV)
						{
							szLog = "CASE #2b (AUTO): Last-Die placement failure to machine stop";
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

							szMsg = _T("Bin Last-Die placement failure; machine is aborted.");		
							HmiMessageEx_Red_Back(szMsg, szTitle);
							SetStatusMessage(szMsg);
							SwitchToBPR();
							return FALSE;
						}
					else
						{
							szLog = "CASE #2a (AUTO) FINISAR: follow Last-Die offset XY";
							SetStatusMessage(szLog);
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
						}
				}
				//Ask user to die pos
				if (bNewAlignMethod && m_bSemitekBLMode)	//(pApp->GetCustomerName() == "Semitek"))	//v4.42T9
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
					if ( (pApp->GetCustomerName() == "SanAn") && (!pApp->IsUsingEngineerMode()))
					{
						szMsg = _T("3rd die placement fail; operation is aborted (SanAn)");		
						HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user (SanAn)\n", "a+");
						SwitchToBPR();
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
					else if ( (pApp->GetCustomerName()=="ChangeLight(XM)") && (!pApp->IsUsingEngineerMode()))
					{
						szMsg = _T("3rd die placement fail; operation is aborted (ChangeLight)");		
						HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user (ChangeLight)\n", "a+");
						SwitchToBPR();
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
					else
					{
						if (IsMS90() && sRetry == 0)	//v4.59A31
						{
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm LastDie fail AUTO abort 2 and retry\n", "a+");
							return FALSE;
						}
						else
						{
							szMsg = _T("3rd die placement fail; please specify last-die position with joystick, then press CONTINUE.");		
							if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
							{
								CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user\n", "a+");
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
					
					//v4.50A3	//3E DL	//LeoLam
					//if ( (pApp->GetCustomerName() == "Electech3E(DL)") ||
					//	 (pApp->GetCustomerName() == "SanAn") )				//v4.51A5	//LeoLam & Douglas
					
						// the same as manual function // 4.53D26
					//if (bNewAlignMethod && m_bSemitekBLMode)
					//{
					//	
					//	if(pApp->GetCustomerName() == "Electech3E(DL)")
					//	{
					//		szMsg = _T("Please confirm NEXT die position; if OK press CONTINUE.");		
					//		SetStatusMessage(szMsg);
					//		if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
					//		{
					//			szMsg = "**Confirm Next Die pos aborted by user";
					//			SetStatusMessage(szMsg);
					//			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg + "\n", "a+");
					//			SwitchToBPR();
					//			//SetAlarmLamp_Yellow();
					//			return FALSE;
					//		}
					//		else
					//		{
					//			szMsg = "Confirm Next Die pos (Semitek) CONTINUE";
					//			SetStatusMessage(szMsg);
					//		}
					//	}
					//}

					if (!bRelOffsetXYinFOV)
					{
						szLog = "CASE #2b (AUTO): Last-Die placement failure to machine stop";
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

						szMsg = _T("Bin Last-Die placement failure; machine is aborted.");		
						HmiMessageEx_Red_Back(szMsg, szTitle);
						SetStatusMessage(szMsg);
						SwitchToBPR();
						return FALSE;
					}
					else
					{
						szLog = "CASE #2a (AUTO): follow Last-Die offset XY";
						SetStatusMessage(szLog);
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
					}
				}
				else
				{
					szMsg.Format("dX = %.2f steps, dY = %.2f steps.", dX, dY);
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Last Die pos placement fails\n", "a+");

					if (!bDisableAlarm)		//v4.24T6
					{
						SetAlert_Red_Yellow(IDS_BT_REALIGN_LAST_DIE_SHIFT);
					}
					SwitchToBPR();
					return FALSE;
				}
			}
			else	//v4.43T12
			{
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
					if ((pApp->GetCustomerName() == "SanAn") && (!pApp->IsUsingEngineerMode()))		//v4.51A7	//LeoLam
					{
						szMsg = _T("Last die is not found; operation is aborted (SanAn)");		
						SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Conirm Last Die pos without die aborted by user 2 (SanAn)\n", "a+");
						SwitchToBPR();
						return FALSE;
					}
					else
					{
						szMsg = _T("Please confirm NEXT die position; if OK press CONTINUE.");		
						SetStatusMessage(szMsg);
						if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
						{
							szMsg = "Conirm Last Die pos without die (Semitek) aborted by user";
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
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user (ChangeLight)\n", "a+");
						SwitchToBPR();
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
					else
					{
						if (IsMS90() && sRetry == 0)	//v4.59A31
						{
							CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm LastDie fail AUTO abort and retry\n", "a+");
							return FALSE;
						}
						else
						{
							szMsg = _T("3rd die placement fail; please specify last-die position with joystick, then press CONTINUE.");		
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
					return FALSE;
				}
				//andrewng
				else if ((pApp->GetCustomerName() == "SanAn") && (!pApp->IsUsingEngineerMode()))		//v4.43T3
				{
					szMsg = _T("Last die is not found; operation is aborted (SanAn)");		
					SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Conirm Last Die pos without die aborted by user (SanAn)\n", "a+");
					SwitchToBPR();
					return FALSE;
				}
				else
				{
					if (SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Continue", "Stop") != 1)
					{
						SwitchToBPR();
						return FALSE;
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
			//m_oBinBlkMain.SetIsAligned(ulBlkInUse, bOldAligned);		//v3.44

			if (!IsBurnIn())
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE);
			}

			/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-align failed\n", "a+"); /******/

			SwitchToBPR();
			//SetJoystickOn(TRUE);
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
	DOUBLE dNewAngleInDegree2 = fabs(dNewAngle * 180.0/ PI);
	if ((dNewAngleInDegree2 > 0.01) && (pApp->GetCustomerName()==CTM_FINISAR))	//0.01 degree
	{
		CString szMsg;
		CString szTitle;
		szTitle = "Global Angle Alignment";
		szMsg.Format("Global Angle exceeds 0.01 degree (%.6f); operation is aborted. Please Unload the frame", dNewAngleInDegree2);		
		HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm Next Die pos aborted by user (SanAn)\n", "a+");
		SwitchToBPR();
		//SetAlarmLamp_Yellow();
		return FALSE;
	}
	szLog.Format("Re-align completed = %ld,%ld,%f,%f,%ld,%ld\n", lNewXOffset, lNewYOffset, dNewAngle, dNewAngleInDegree2,
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

	CMS896AStn::m_bRealignFrameDone = TRUE;
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
	DOUBLE fX, fY, dDistX, dDistY;

	LONG l1stDieIndex = 1;

	m_oBinBlkMain.StepDMove(ulBlkInUse, l1stDieIndex, dOrgX, dOrgY);

	dDistX = dX - dOrgX;
	dDistY = dY - dOrgY;

	fX = dDistX * cos(dAngle) - dDistY * sin(dAngle) + lXOffset;
	fY = dDistY * cos(dAngle) + dDistX * sin(dAngle) + lYOffset;

	dX = fX + dOrgX;
	dY = fY + dOrgY;

	return (TRUE);
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
	lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));
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
			szLog.Format("Search 1st die start at blk #%d", ulBlkInUse);
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
			if (HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
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

	/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-confirm TL die", "a+"); /******/
	
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


	szLog.Format("Re-align completed = %d,%d,%f\n", lNewXOffset, lNewYOffset, dNewAngle);
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
	LONG lX, lY, lDiePitchX, lDiePitchY;
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
	lDiePitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDiePitchX(ulBlkInUse));
	lDiePitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDiePitchY(ulBlkInUse));
	bOldAligned		= m_oBinBlkMain.GrabNVIsAligned(ulBlkInUse);

	szTitle.LoadString(HMB_BT_REALIGN_BIN_BLOCK);			

	if (ulNoOfSortedDie == 0)
	{
		SetJoystickOn(FALSE);	//v3.71T5

		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
		m_oBinBlkMain.StepDMove(ulBlkInUse, ulFirstDieIndex, dStepX, dStepY);	//v4.59A19
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dStepX), ConvertFileUnitToYEncoderValue(dStepY), FALSE);

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

		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dStepY, dStepY);	//v4.59A19
		//ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT_MoveTo(ConvertFileUnitToXEncoderValue(dStepY), ConvertFileUnitToYEncoderValue(dStepY));
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

		m_oBinBlkMain.StepDMove(ulBlkInUse, ulDieIndex, dStepX, dStepY);	//v4.59A19
		
		DieALogical_x = ConvertFileUnitToXEncoderValue(dStepX);
		DieALogical_y = ConvertFileUnitToYEncoderValue(dStepY);
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

					if (SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Continue", "Stop") != 1)
					{
						//SetAlarmLamp_Yellow();
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
	szLog.Format("Re-align completed: Offset(XYT) = %d, %d, %f, %f\n", lNewXOffset, lNewYOffset, dNewAngle, dNewAngleInDegree1);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	//SwitchToBPR();
	m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
	m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, dNewAngle);
	m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
	m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);

	CMS896AStn::m_bRealignFrameDone = TRUE;
	return (TRUE);
}


BOOL CBinTable::FindLastRowColumnFrameAngle(ULONG ulBlkInUse, ULONG ulDieIndex, 
											DOUBLE dNewAngle, 
											BOOL bUpdateOffset, LONG& lNewXOffset, LONG& lNewYOffset,
											DOUBLE& dNewLastRowColAngle)
{
	CString szLog;
	ULONG ulIndex1=0;
	ULONG ulIndex2=0;

	ULONG ulMapIndex	= ulDieIndex;
	LONG lXOffset		= lNewXOffset;
	LONG lYOffset		= lNewYOffset;

	if (!m_bEnable_T)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColumnFrameAngle disabled: wo T", "a+");
		return FALSE;
	}
	if (m_bUseFrameCrossAlignment)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColumnFrameAngle disabled: CrossChk is used", "a+");
		return FALSE;
	}

	ULONG ulWalkPath = m_oBinBlkMain.GrabWalkPath(ulBlkInUse);
	if ( (ulWalkPath != BT_TLV_PATH) && (ulWalkPath != BT_TRV_PATH) &&
		 (ulWalkPath != BT_BLV_PATH) && (ulWalkPath != BT_BRV_PATH) )
	{
		szLog.Format("FindLastRowColumnFrameAngle: NOT-USE because sortpath (%lu) not supported", ulWalkPath);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		return FALSE;
	}

	///////////////////////////////////////
	// Find INDEX1 and INDEX2 
	if (m_bEnableBinMapBondArea)
	{
		m_oBinBlkMain.GetDieIndexInBinMap2(ulBlkInUse, ulMapIndex, ulIndex1, ulIndex2);
		szLog.Format("FindLastRowColAngle (Binmap): WalkPath=%lu; DieIndex: curr=%lu, Last1=%lu, Last2=%lu", 
							ulWalkPath, ulDieIndex, ulIndex1, ulIndex2);
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

		ulIndex2 = ulNoOfSortedDie - ulRemainder;
		if (ulRemainder == 0)
			ulIndex2 = ulNoOfSortedDie - ulDiePerCol;
		ulIndex1 = ulIndex2 - ulDiePerCol + 1;

		szLog.Format("FindLastRowColAngle (Normal): WalkPath=%lu; DieIndex: curr=%lu, Last1=%lu, Last2=%lu", 
					ulWalkPath, ulDieIndex, ulIndex1, ulIndex2);
	}

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage(szLog);

	if (ulIndex2 <= 8)	//last row/col is 1st row/col, so do not use !!!
	{
		szLog.Format("FindLastRowColAngle NOT-USE due to 1st row or col (%ld, %ld)",  ulIndex1, ulIndex2);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		return FALSE;
	}


	DOUBLE dIndexUmX1=0, dIndexUmY1=0;
	DOUBLE dIndexUmX2=0, dIndexUmY2=0;
	LONG	lIndex1Physical_x=0, lIndex1Physical_y=0,lIndex2Physical_x=0, lIndex2Physical_y=0;
	LONG	lIndex1OffsetX=0, lIndex1OffsetY=0,lIndex2OffsetX=0, lIndex2OffsetY=0;
	LONG	lIndexFlag11 =0,lIndexFlag12 =0,lIndexFlag13 =0,lIndexFlag14 =0; //Check if Index Die cannot find could omit
	LONG	lIndexFlag21 =0,lIndexFlag22 =0,lIndexFlag23 =0,lIndexFlag24 =0; //Check if Index Die cannot find could omit



	/////////////////////////////////////////
	// Move & Find INDEX1 - 123 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex1, dIndexUmX1, dIndexUmY1, FALSE);
	ConvertBinTableDPosn(dIndexUmX1, dIndexUmY1, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX1), ConvertFileUnitToYEncoderValue(dIndexUmY1));
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
	Sleep(m_lAlignBinDelay);	
	
	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX1-1 position", "a+");
		lIndexFlag11 = 1;
		//return FALSE;
	}
		
	Sleep(100);
	GetXYEncoderValue(lIndex1Physical_x, lIndex1Physical_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 1-1 Enc (%ld, %ld)", 
		lIndex1Physical_x, lIndex1Physical_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX1-1 position ......");


	// Move & Find INDEX1 - 2 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex1+1, dIndexUmX1, dIndexUmY1, FALSE);
	ConvertBinTableDPosn(dIndexUmX1, dIndexUmY1, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX1), ConvertFileUnitToYEncoderValue(dIndexUmY1));
	//DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
	Sleep(m_lAlignBinDelay);	
		
	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX1-2 position", "a+");
		lIndexFlag12 = 1;
		//return FALSE;
	}
		
	Sleep(100);
	LONG lIndex1Physical2_x=0, lIndex1Physical2_y=0;
	GetXYEncoderValue(lIndex1Physical2_x, lIndex1Physical2_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 1-2 Enc (%ld, %ld)", 
		lIndex1Physical2_x, lIndex1Physical2_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX1-2 position ......");


	// Move & Find INDEX1 - 3 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex1+2, dIndexUmX1, dIndexUmY1, FALSE);
	ConvertBinTableDPosn(dIndexUmX1, dIndexUmY1, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX1), ConvertFileUnitToYEncoderValue(dIndexUmY1));
	Sleep(m_lAlignBinDelay);	
		
	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX1-3 position", "a+");
		lIndexFlag13 = 1;
		//return FALSE;
	}

	Sleep(100);
	LONG lIndex1Physical3_x=0, lIndex1Physical3_y=0;
	GetXYEncoderValue(lIndex1Physical3_x, lIndex1Physical3_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 1-3 Enc (%ld, %ld)", 
		lIndex1Physical3_x, lIndex1Physical3_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX1-3 position ......");


	// Move & Find INDEX1 - 4 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex1+3, dIndexUmX1, dIndexUmY1, FALSE);
	ConvertBinTableDPosn(dIndexUmX1, dIndexUmY1, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX1), ConvertFileUnitToYEncoderValue(dIndexUmY1));
	Sleep(m_lAlignBinDelay);	
		
	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX1-4 position", "a+");
		lIndexFlag14 =1;
		//return FALSE;
	}

	Sleep(100);
	LONG lIndex1Physical4_x=0, lIndex1Physical4_y=0;
	GetXYEncoderValue(lIndex1Physical4_x, lIndex1Physical4_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 1-4 Enc (%ld, %ld)", 
		lIndex1Physical4_x, lIndex1Physical4_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX1-4 position ......");


	/////////////////////////////////////////
	// Move & Find INDEX2-1 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex2-3, dIndexUmX2, dIndexUmY2, FALSE);
	ConvertBinTableDPosn(dIndexUmX2, dIndexUmY2, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX2), ConvertFileUnitToYEncoderValue(dIndexUmY2));
	//DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
	Sleep(m_lAlignBinDelay);	

	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX2 position", "a+");
		lIndexFlag21=1;
		//return FALSE;
	}

	Sleep(100);
	GetXYEncoderValue(lIndex2Physical_x, lIndex2Physical_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 2 Enc (%ld, %ld)", 
		lIndex2Physical_x, lIndex2Physical_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX2-1 position ......");


	// Move & Find INDEX2-2 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex2-2, dIndexUmX2, dIndexUmY2, FALSE);
	ConvertBinTableDPosn(dIndexUmX2, dIndexUmY2, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX2), ConvertFileUnitToYEncoderValue(dIndexUmY2));
	//DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
	Sleep(m_lAlignBinDelay);	

	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX2 position", "a+");
		lIndexFlag22=1;
		//return FALSE;
	}

	Sleep(100);
	LONG lIndex2Physical2_x=0, lIndex2Physical2_y=0;
	GetXYEncoderValue(lIndex2Physical2_x, lIndex2Physical2_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 2-2 Enc (%ld, %ld)", 
		lIndex2Physical2_x, lIndex2Physical2_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX2-1 position ......");

	
	// Move & Find INDEX2-3 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex2-1, dIndexUmX2, dIndexUmY2, FALSE);
	ConvertBinTableDPosn(dIndexUmX2, dIndexUmY2, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX2), ConvertFileUnitToYEncoderValue(dIndexUmY2));
	//DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
	Sleep(m_lAlignBinDelay);	

	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX2-2 position", "a+");
		lIndexFlag23=1;
		//return FALSE;
	}

	Sleep(100);
	LONG lIndex2Physical3_x=0, lIndex2Physical3_y=0;
	GetXYEncoderValue(lIndex2Physical3_x, lIndex2Physical3_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 2-3 Enc (%ld, %ld)", 
		lIndex2Physical3_x, lIndex2Physical3_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX2-2 position ......");


	// Move & Find INDEX2-4 position 
	m_oBinBlkMain.StepDMove(ulBlkInUse, ulIndex2, dIndexUmX2, dIndexUmY2, FALSE);
	ConvertBinTableDPosn(dIndexUmX2, dIndexUmY2, ulBlkInUse, dNewAngle, lXOffset, lYOffset);
	BT_MoveTo(ConvertFileUnitToXEncoderValue(dIndexUmX2), ConvertFileUnitToYEncoderValue(dIndexUmY2));
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
	Sleep(m_lAlignBinDelay);	

	if (SearchGoodDie() == FALSE)
	{
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX2-2 position", "a+");
		lIndexFlag24=1;
		//return FALSE;
	}

	Sleep(100);
	LONG lIndex2Physical4_x=0, lIndex2Physical4_y=0;
	GetXYEncoderValue(lIndex2Physical4_x, lIndex2Physical4_y);

	szLog.Format("FindLastRowColAngle (Finisar): INDEX 2-4 Enc (%ld, %ld)", 
		lIndex2Physical4_x, lIndex2Physical4_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage("This is INDEX2-3 position ......");


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

	//Angle1 between INDEX1-1 & INDEX2-4
	LONG lLHS1DieX = lIndex1Physical_x;
	LONG lLHS1DieY = lIndex1Physical_y;
	LONG lRHS1DieX = lIndex2Physical4_x;
	LONG lRHS1DieY = lIndex2Physical4_y;

	//Angle2 between INDEX1-2 & INDEX2-2
	LONG lLHS2DieX = lIndex1Physical2_x;
	LONG lLHS2DieY = lIndex1Physical2_y;
	LONG lRHS2DieX = lIndex2Physical3_x;
	LONG lRHS2DieY = lIndex2Physical3_y;

	//Angle3 between INDEX1-3 & INDEX2-1
	LONG lLHS3DieX = lIndex1Physical3_x;
	LONG lLHS3DieY = lIndex1Physical3_y;
	LONG lRHS3DieX = lIndex2Physical2_x;
	LONG lRHS3DieY = lIndex2Physical2_y;

	//Angle4 between INDEX1-3 & INDEX2-1
	LONG lLHS4DieX = lIndex1Physical4_x;
	LONG lLHS4DieY = lIndex1Physical4_y;
	LONG lRHS4DieX = lIndex2Physical_x;
	LONG lRHS4DieY = lIndex2Physical_y;

	//If Vertical sorting, arrange LHS & RHS by Y encoder pos of INDEX1 & INDEX2
	if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) )
	{
		//if current zigzag to the other direction, reverse positions
		if (lIndex2Physical_y > lIndex1Physical_y)
		{
			lLHS1DieX = lIndex2Physical4_x;
			lLHS1DieY = lIndex2Physical4_y;
			lRHS1DieX = lIndex1Physical_x;
			lRHS1DieY = lIndex1Physical_y;

			lLHS2DieX = lIndex2Physical3_x;
			lLHS2DieY = lIndex2Physical3_y;
			lRHS2DieX = lIndex1Physical2_x;
			lRHS2DieY = lIndex1Physical2_y;

			lLHS3DieX = lIndex2Physical2_x;
			lLHS3DieY = lIndex2Physical2_y;
			lRHS3DieX = lIndex1Physical3_x;
			lRHS3DieY = lIndex1Physical3_y;

			lLHS4DieX = lIndex2Physical_x;
			lLHS4DieY = lIndex2Physical_y;
			lRHS4DieX = lIndex1Physical4_x;
			lRHS4DieY = lIndex1Physical4_y;
		}
	}
	else if ( (ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_BRV_PATH) )
	{
		//if current zigzag to the other direction, reverse positions
		if (lIndex2Physical_y < lIndex1Physical_y)
		{
			lLHS1DieX = lIndex2Physical4_x;
			lLHS1DieY = lIndex2Physical4_y;
			lRHS1DieX = lIndex1Physical_x;
			lRHS1DieY = lIndex1Physical_y;

			lLHS2DieX = lIndex2Physical3_x;
			lLHS2DieY = lIndex2Physical3_y;
			lRHS2DieX = lIndex1Physical2_x;
			lRHS2DieY = lIndex1Physical2_y;

			lLHS3DieX = lIndex2Physical2_x;
			lLHS3DieY = lIndex2Physical2_y;
			lRHS3DieX = lIndex1Physical3_x;
			lRHS3DieY = lIndex1Physical3_y;

			lLHS4DieX = lIndex2Physical_x;
			lLHS4DieY = lIndex2Physical_y;
			lRHS4DieX = lIndex1Physical4_x;
			lRHS4DieY = lIndex1Physical4_y;
		}
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
	
	if ( (ulWalkPath == BT_BRV_PATH) || (ulWalkPath == BT_BLV_PATH) ) //leo please check that BLV and BRV should put together
	{
		dThetaAngle1		= atan2( labs(dDiffX1) , labs(dDiffY1) ) * 180 / PI;
		dThetaAngleInRadian1 = atan2( labs(dDiffX1) , labs(dDiffY1) );
		dThetaAngle2		= atan2( labs(dDiffX2) , labs(dDiffY2) ) * 180 / PI;
		dThetaAngleInRadian2 = atan2( labs(dDiffX2) , labs(dDiffY2) );
		dThetaAngle3		= atan2( labs(dDiffX3) , labs(dDiffY3) ) * 180 / PI;
		dThetaAngleInRadian3 = atan2( labs(dDiffX3) , labs(dDiffY3) );
		dThetaAngle4		= atan2( labs(dDiffX4) , labs(dDiffY4) ) * 180 / PI;
		dThetaAngleInRadian4 = atan2( labs(dDiffX4) , labs(dDiffY4) );

		if ( (lRHS1DieX - lLHS1DieX) < 0 )
		{
			dThetaAngle1 = dThetaAngle1 * -1.00;
			dThetaAngleInRadian1 = dThetaAngleInRadian1 * -1.00;
			dThetaAngle2 = dThetaAngle2 * -1.00;
			dThetaAngleInRadian2 = dThetaAngleInRadian2 * -1.00;
			dThetaAngle3 = dThetaAngle3 * -1.00;
			dThetaAngleInRadian3 = dThetaAngleInRadian3 * -1.00;
			dThetaAngle4 = dThetaAngle4 * -1.00;
			dThetaAngleInRadian4 = dThetaAngleInRadian4 * -1.00;
		}
	}
	else if ( (ulWalkPath == BT_TRV_PATH) || (ulWalkPath == BT_TLV_PATH))//leo please check that TLV and TRV should put together
	{
		dThetaAngle1			= atan2( labs(dDiffX1) , labs(dDiffY1) ) * 180 / PI;
		dThetaAngleInRadian1	= atan2( labs(dDiffX1) , labs(dDiffY1) );
		dThetaAngle2			= atan2( labs(dDiffX2) , labs(dDiffY2) ) * 180 / PI;
		dThetaAngleInRadian2	= atan2( labs(dDiffX2) , labs(dDiffY2) );
		dThetaAngle3			= atan2( labs(dDiffX3) , labs(dDiffY3) ) * 180 / PI;
		dThetaAngleInRadian3	= atan2( labs(dDiffX3) , labs(dDiffY3) );
		dThetaAngle4			= atan2( labs(dDiffX4) , labs(dDiffY4) ) * 180 / PI;
		dThetaAngleInRadian4	= atan2( labs(dDiffX4) , labs(dDiffY4) );

		if ( (lRHS1DieX - lLHS1DieX) > 0 )
		{
			dThetaAngle1 = dThetaAngle1 * -1.00;
			dThetaAngleInRadian1 = dThetaAngleInRadian1 * -1.00;
			dThetaAngle2 = dThetaAngle2 * -1.00;
			dThetaAngleInRadian2 = dThetaAngleInRadian2 * -1.00;
			dThetaAngle3 = dThetaAngle3 * -1.00;
			dThetaAngleInRadian3 = dThetaAngleInRadian3 * -1.00;
			dThetaAngle4 = dThetaAngle4 * -1.00;
			dThetaAngleInRadian4 = dThetaAngleInRadian4 * -1.00;
		}
	}
	/*else
	{
		dThetaAngle = atan2(dDiffY , dDiffX) * (180 / PI) * -1.00;
		if ((ulWalkPath == BT_TRH_PATH) || (ulWalkPath == BT_BRH_PATH))
		{
			dThetaAngle = dThetaAngle * -1.00;		//v4.49A10	//MS90
			dThetaAngleInRadian = dThetaAngleInRadian * -1.00;
		}
	}*/

	//Check if pairs dies could not be PR //leo 20171105
	LONG lIndexFlag = 0;
	if (lIndexFlag11 == 1 || lIndexFlag24 == 1)
	{
		dThetaAngle1 = 0.0;
		dThetaAngleInRadian1 = 0.0;
		lIndexFlag++;
	}
	if (lIndexFlag12 == 1 || lIndexFlag23 == 1)
	{
		dThetaAngle2 = 0.0;
		dThetaAngleInRadian2 = 0.0;
		lIndexFlag++;
	}
	if (lIndexFlag13 == 1 || lIndexFlag22 == 1)
	{
		dThetaAngle3 = 0.0;
		dThetaAngleInRadian3 = 0.0;
		lIndexFlag++;
	}
	if (lIndexFlag14 == 1 || lIndexFlag21 == 1)
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

//HmiMessage(szLog);


	/////////////////////////////////////////
	// Rotate global angle by its Average
	T_MoveByDegree(-1 * dThetaAngle);
	dNewLastRowColAngle = -1 * dThetaAngleInRadian;	//return angle (in rad) to main fcn
	Sleep(100);

	szLog.Format("FindLastRowColAngle (Finisar): New Global-T Angle = %.6f radian (%.6 deg)\n", 
		dNewLastRowColAngle, -1*dThetaAngle);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage(szLog);


	if (bUpdateOffset)
	{
		LONG lDieIndexToUpdate = ulIndex1+2;		//INDEX1-3
		CString szIndex = "INDEX1-3";

		if (lIndexFlag13 == 1)
		{	
			if (lIndexFlag14 == 0)
			{
				lDieIndexToUpdate = ulIndex1+3;		//INDEX1-4
				szIndex = "INDEX1-4";
			}
			else if (lIndexFlag21 == 0)
			{
				lDieIndexToUpdate = ulIndex2-3;		//INDEX2-1
				szIndex = "INDEX2-1";
			}
			else if (lIndexFlag22 == 0)
			{
				lDieIndexToUpdate = ulIndex2-2;		//INDEX2-2
				szIndex = "INDEX2-2";
			}
		}

		szLog.Format("FindLastRowColAngle (UpdateXYOffset): use %s = %ld (%ld, %ld, %ld, %ld)", 
			szIndex, lDieIndexToUpdate, lIndexFlag13, lIndexFlag14, lIndexFlag21, lIndexFlag22);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		m_oBinBlkMain.StepDMove(ulBlkInUse, lDieIndexToUpdate, dIndexUmX1, dIndexUmY1);
		//ConvertBinTableDPosn(dIndexUmX1, dIndexUmY1, ulBlkInUse, dNewAngle, lXOffset, lYOffset);

		LONG lLogicalIndex1X = ConvertFileUnitToXEncoderValue(dIndexUmX1);
		LONG lLogicalIndex1Y = ConvertFileUnitToXEncoderValue(dIndexUmY1);
		LONG lNewLogicalIndex1X = ConvertFileUnitToXEncoderValue(dIndexUmX1 + lXOffset);
		LONG lNewLogicalIndex1Y = ConvertFileUnitToYEncoderValue(dIndexUmY1 + lYOffset);

		BT_MoveTo(lNewLogicalIndex1X, lNewLogicalIndex1Y);
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(m_lAlignBinDelay);	
//HmiMessage("Jump back to INDEX 1 die to update XY Offset .....");

		if (SearchGoodDie() == FALSE)
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("FindLastRowColAngle FAIL: no die found at INDEX1 position 3", "a+");
			return FALSE;
		}
			
		LONG lEncX=0, lEncY=0;
		GetXYEncoderValue(lEncX, lEncY);

		LONG lIndex1OffsetX = -ConvertXEncoderValueToFileUnit(lEncX - lLogicalIndex1X);
		LONG lIndex1OffsetY = -ConvertYEncoderValueToFileUnit(lEncY - lLogicalIndex1Y);

		szLog.Format("Use %s DieOffset XY: orig XYOffset (%ld, %ld); new XYOffset (%ld, %ld)", 
							szIndex, lXOffset, lYOffset, lIndex1OffsetX, lIndex1OffsetY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
//HmiMessage(szLog);

		lNewXOffset = lIndex1OffsetX;
		lNewYOffset = lIndex1OffsetY;
	}

	return TRUE;
}


