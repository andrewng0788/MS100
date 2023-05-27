/////////////////////////////////////////////////////////////////
// BH_SubState.cpp : State for Auto-cycle of the CBondHead class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "MS896A.h"		// For Timing

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBondHead::MoveBHZToHome(const BOOL bBHZ2)
{
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		Z2_MoveToHome();
	}
	else
	{
		Z_MoveToHome();
	}
}


VOID CBondHead::MoveBHZToSwingLevel(const BOOL bBHZ2)
{
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
	}
	else
	{
		Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	}
}


VOID CBondHead::BHZStrongBlowOn(const BOOL bBHZ2, const LONG lAirStrongBlowCount, const LONG lStrongBlowOnDelay, const LONG lStrongBlowOffDelay)
{
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		//Clean BHZ2
		SetPickVacuumZ2(FALSE);
		Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7

		if (lAirStrongBlowCount > 0)
		{
			INT nCount = 0;
			do 
			{
				SetStrongBlowZ2(TRUE);
				Sleep(lStrongBlowOnDelay);			// Cleaning 
				SetStrongBlowZ2(FALSE);
				Sleep(lStrongBlowOffDelay);
				nCount++;

			} 
			while (nCount < lAirStrongBlowCount);
		}
		
		Sleep(100);
		SetPickVacuumZ2(TRUE);
	}
	else
	{
		//Clean BHZ1
		SetPickVacuum(FALSE);
		Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7

		if (lAirStrongBlowCount > 0)
		{
			INT nCount = 0;
			do 
			{
				SetStrongBlow(TRUE);
				Sleep(lStrongBlowOnDelay);			// Cleaning 
				SetStrongBlow(FALSE);
				Sleep(lStrongBlowOffDelay);
				nCount++;

			} 
			while (nCount < lAirStrongBlowCount);
		}
		
		Sleep(100);
		SetPickVacuum(TRUE);
	}
	Sleep(500);
}


BOOL CBondHead::MoveToHolderStrongBlow(const BOOL bBHZ2, const BOOL bStrongBlowOn, const LONG lAirStrongBlowCount, const LONG lStrongBlowOnDelay, const LONG lStrongBlowOffDelay)
{
	//3. Move BT to Holder Install position
	if (!BTMoveToAGCHolderInstallPos(bBHZ2))
	{
		return FALSE;
	}

	//4. Move T to Bond Position
	LONG nErr = T_SafeMoveToInAuto(bBHZ2 ? m_lPickPos_T : m_lBondPos_T, "MoveToHolderStrongBlow Move To Bond");

	//5. Move to Blow die Z level=====================
	if (!nErr)
	{
		nErr = MS50ChgColletZ_MoveTo(m_lAGCHolderBlowDieZ);
	}
	//=============================================

	if (nErr)
	{
		return FALSE;
	}

	SetChgColletHolderVacuum(TRUE);
	if (bStrongBlowOn)
	{
		Sleep(500);
		SetChgColletHolderVacuum(FALSE);
		SetColletHolderStrongBlow(TRUE);
	}
	BHZStrongBlowOn(bBHZ2, lAirStrongBlowCount, lStrongBlowOnDelay, lStrongBlowOffDelay);
	SetColletHolderStrongBlow(FALSE);
	SetChgColletHolderVacuum(FALSE);
	nErr = MS50ChgColletZ_MoveTo(0, SFM_NOWAIT);
	MoveBHZToSwingLevel(bBHZ2);
	return (nErr == 0) ? TRUE : FALSE;
}


BOOL CBondHead::MoveToPrepick(const BOOL bBHZ2)
{
	//1. Move to swing level
	MoveBHZToHome(bBHZ2);
	MoveBHZToSwingLevel(bBHZ2);

	LONG nErr = T_SafeMoveToInAuto(m_lPrePickPos_T, "MoveToPrepick Move To PrePick");	//	m_lCleanColletPos_T

	return (nErr == 0) ? TRUE : FALSE;
}


BOOL CBondHead::MoveToPrepickStrongBlow(const BOOL bBHZ2, const LONG lAirStrongBlowCount, const LONG lStrongBlowOnDelay, const LONG lStrongBlowOffDelay)
{
	BHZStrongBlowOn(bBHZ2, lAirStrongBlowCount, lStrongBlowOnDelay, lStrongBlowOffDelay);

	return TRUE;
}


BOOL CBondHead::BHStrongBlowColletDieAtPrepick(const BOOL bBHZ2)
{
	BOOL bRet = FALSE;

	//2.  Strong Blow
	if (IsNewAutoCleanColletSystem())
	{
		bRet = MoveToHolderStrongBlow(bBHZ2, FALSE, 1, 2000, 500);
		if (!bRet)
		{
			MoveBHZToHome(bBHZ2);
			T_SafeMoveToInAuto(m_lPrePickPos_T, "BHStrongBlowColletDieAtPrepick  Move To PrePick");
		}
	}
	else
	{
		bRet = MoveToPrepickStrongBlow(bBHZ2, 1, 2000, 500);
	}

	return bRet;
}


VOID CBondHead::BHZMoveToCleanColletZLevel(const BOOL bBHZ2)
{
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		if( m_ulCleanColletTimeOut > 0 )	//	Clean BHZ2	turn on blow 
		{
			SetPickVacuumZ2(FALSE);
			SetStrongBlowZ2(TRUE);
		}

		LONG lCleanLevel = m_lAutoCleanCollet_Z + m_lBondLevel_Z2 - m_lBondLevel_Z;
		if (m_lAutoCleanOffsetZ != 0)
		{
			lCleanLevel = m_lBondLevel_Z2 + m_lAutoCleanOffsetZ;
		}
		Z2_MoveTo(lCleanLevel, SFM_WAIT);
	}
	else
	{
		if( m_ulCleanColletTimeOut>0 )	//Clean BHZ1	turn on blow 
		{
			SetPickVacuum(FALSE);
			SetStrongBlow(TRUE);
		}

		LONG lCleanLevel = m_lAutoCleanCollet_Z;
		if (m_lAutoCleanOffsetZ != 0)
		{
			lCleanLevel = m_lBondLevel_Z + m_lAutoCleanOffsetZ;
		}
		Z_MoveTo(lCleanLevel, SFM_WAIT);
	}
	Sleep(50);
}



VOID CBondHead::BHZMoveToRemoveDirtZLevel(const BOOL bBHZ2)
{
	if (bBHZ2)
	{
		Z2_MoveTo(m_lBondLevel_Z2 + 100, SFM_WAIT);
	}
	else
	{
		Z_MoveTo(m_lBondLevel_Z + 100, SFM_WAIT);
	}
}

//================================================================
// Function Name: 		MoveBinTableToBondPosn
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to bond position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::MoveBinTableToBondPosn(const BOOL bToBond)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("MoveBinTableToBondPosn"), bToBond))
	{
		CString szErr;

		szErr = "MoveBinTableToBondPosn: Bin Table is not able to move to bond position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

//================================================================
// Function Name: 		BTMoveToACCLiquid
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to ACCLiquid position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToACCLiquid()
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToACCLiquid"), FALSE))
	{
		CString szErr;

		szErr = "BTMoveToACCLiquid: Bin Table is not able to move to ACCLiquid position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

//================================================================
// Function Name: 		BTMoveToACCClean
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to ACCClean position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToACCClean()
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToACCClean"), FALSE))
	{
		CString szErr;

		szErr = "BTMoveToACCClean: Bin Table is not able to move to ACCClean position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

//================================================================
// Function Name: 		BTCycleACCOperation
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to ACCOperation position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTCycleACCOperation(const BOOL bBHZ2)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTCycleACCOperation"), bBHZ2))
	{
		CString szErr;

		szErr = "BTCycleACCOperation: Bin Table is not able to move to ACCOperation position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}


//================================================================
// Function Name: 		BTMoveToRemoveDirt
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to Remove-Dirt position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTMoveToRemoveDirt()
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToRemoveDirt"), FALSE))
	{
		CString szErr;

		szErr = "BTMoveToRemoveDirt: Bin Table is not able to move to Remove-Dirt position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

//================================================================
// Function Name: 		BTCycleRemoveDirtOperation
// Input arguments:     bBHZ2
// Output arguments:	None
// Description:   		BT will move to Remove-Dirt position
// Return:				None
// Remarks:				called by other function
//================================================================
BOOL CBondHead::BTCycleRemoveDirtOperation(const BOOL bBHZ2)
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTCycleRemoveDirtOperation"), bBHZ2))
	{
		CString szErr;

		szErr = "BTCycleRemoveDirtOperation: Bin Table is not able to move to Remove-Dirt position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}


BOOL CBondHead::OpBTAutoCleanCollet(const BOOL bBHZ2, BOOL &bReturn)
{
	// let bin table move to clean position
	if (!BTMoveToACCClean())
	{
		MoveBHZToHome(bBHZ2);
		T_SafeMoveToInAuto(m_lPrePickPos_T, "OpBTAutoCleanCollet Move To PrePick");
		return FALSE;
	}

	//	??	auto learn level before clean	??
	// bond head down to level
	BHZMoveToCleanColletZLevel(bBHZ2);

	SetBHPickVacuum(bBHZ2, FALSE);
	Sleep(500);
	// let the bin table move brush the collet, with a pattern and cycle and time delay
	bReturn = BTCycleACCOperation(bBHZ2);


	//protect not to drip alcohol
	MoveBHZToHome(bBHZ2);
	SetBHPickVacuum(bBHZ2, FALSE);
	Sleep(500);
	SetBHStrongBlow(bBHZ2, TRUE);
	Sleep(1000);
	SetBHStrongBlow(bBHZ2, FALSE);
	Sleep(500);
	SetBHPickVacuum(bBHZ2, TRUE);
	Sleep(500);

	//1. Move to swing level
	if (!MoveToPrepick(bBHZ2))
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	//Remove exceed alcohol
	if (IsNewAutoCleanColletSystem())
	{
		bRet = MoveToHolderStrongBlow(bBHZ2, TRUE, m_lAutoCleanAirBlowLimit, m_lHighBlowTime, m_lHighBlowTime);
	}
	else
	{
		bRet = MoveToPrepickStrongBlow(bBHZ2, m_lAutoCleanAirBlowLimit, m_lHighBlowTime, m_lHighBlowTime);
	}

	//Remove dirt on tip
	if (IsNewAutoCleanColletSystem() && bReturn && bRet)
	{
		for (LONG i = 0; i < 2; i++)
		{
			// let bin table move to remove-dirt position
			if (!BTMoveToRemoveDirt())
			{
				MoveBHZToHome(bBHZ2);
				T_SafeMoveToInAuto(m_lPrePickPos_T, "OpBTAutoCleanCollet Move To PrePick");
				return FALSE;
			}

			BHZMoveToRemoveDirtZLevel(bBHZ2);

			SetBHPickVacuum(bBHZ2, FALSE);
			Sleep(500);
			// let the bin table move brush the collet, with a pattern and cycle and time delay
			BTCycleRemoveDirtOperation(bBHZ2);
			if (i == 0)
			{
				MoveBHZToSwingLevel(bBHZ2);
			}
		}
		MoveBHZToHome(bBHZ2);
	}

	return TRUE;
}



BOOL CBondHead::IsDoCleanCollet(const BOOL bBHZ2)
{
	//Log file===============================================================================
	LogAutoCleanColletThresholdValue(!bBHZ2);
	//====================================================================================

	//==================Check Airflow================================================
	if (m_bMS100DigitalAirFlowSnr)
	{
		if (bBHZ2)
		{
			if (ReadBH2AirFlowValue())
			{
				//if the current value is more than 1.3 * the threshold of missing die, not do collet clean
				if (m_ulBHZ2DAirFlowValue > m_ulBHZ2DAirFlowCleanColletThreshold) //(unsigned long )_round(m_ulBHZ2DAirFlowThreshold * 1.3))
				{
					return FALSE;  //no do collet-clean
				}
			}
		}
		else
		{
			if (ReadBH1AirFlowValue())
			{
				//if the current value is more than 1.3 * the threshold of missing die, not do collet clean
				if (m_ulBHZ1DAirFlowValue > m_ulBHZ1DAirFlowCleanColletThreshold) //(unsigned long )_round(m_ulBHZ1DAirFlowThreshold * 1.3))
				{
					return FALSE;  //no do collet-clean
				}
			}
		}
	}

	return TRUE;
}


BOOL CBondHead::OpMS100AutoCleanCollet(const BOOL bBHZ2)
{
	BOOL bReturn = TRUE;

	CMSLogFileUtility::Instance()->MS_LogOperation("Auto-Clean-Collet start");	//v4.52A2
	CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet (normal) start");

	if (m_bMS100DigitalAirFlowSnr)	
	{
		BH_Z_AirFlowLog(bBHZ2, "AutoCleanCollet");
	}

	// make sure bin table move back as it is indexed already or postbond not done yet
	if (!MoveBinTableToBondPosn(FALSE))
	{
		return FALSE;
	}

	//m_bSearchColletNow1 = TRUE;
	//m_bSearchColletNow2 = TRUE;//20190610, trigger search collet hole

	if (!m_bBPRErrorToCleanCollet)
	{
		SaveAutoErrorHandlingLog("Clean Collet,Hit Collet Life Time", m_psmfSRam);
		//1. Move to swing level
		if (!MoveToPrepick(bBHZ2))
		{
			return FALSE;
		}

		//Bondhead strong blow die on the collet at pre-pick or holder position
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		LONG lCount = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Clean Collet Strong Blow Count"), 1);
		MoveToPrepickStrongBlow(bBHZ2, lCount, 2000, 500);
		//Strong blow should do before check threshold
		if (!IsDoCleanCollet(bBHZ2))
		{
			return TRUE;  //no do collet-clean
		}

		if (!BHStrongBlowColletDieAtPrepick(bBHZ2))
		{
			MoveBinTableToBondPosn(TRUE);
			return FALSE;
		}
	}
	else
	{
		SaveAutoErrorHandlingLog("Clean Collet,BPR Error", m_psmfSRam);
	}

	// should the missing/jam die check complete
	// bond arm move to clean position and bond head up to swing level
	MoveBHZToSwingLevel(bBHZ2);
	Sleep(50);
	T_SafeMoveToInAuto(bBHZ2 ? m_lPickPos_T : m_lBondPos_T, "Op MS100 ACC");	// always arm 1 base, must at bond positon to clean collet

	// let bin table move to alcohol dropping position
	CTime stTime = CTime::GetCurrentTime();
	CTimeSpan stTimeDiff = stTime - m_tSqueezeLiquidTime;
	if( (m_lLiquidExpireTime==0) || (stTimeDiff.GetTotalSeconds()>=m_lLiquidExpireTime) )
	{
		if (!BTMoveToACCLiquid())
		{
			MoveBHZToHome(bBHZ2);
			T_SafeMoveToInAuto(m_lPrePickPos_T, "OpMS100AutoCleanCollet Move To PrePick");
			MoveBinTableToBondPosn(TRUE);
			return FALSE;
		}

		// press the alcohol to the cloth
		CleanColletPocketAlcoholOnce();
		m_tSqueezeLiquidTime = CTime::GetCurrentTime();
	}

	//Clean Collet with Bin Table
	if (!OpBTAutoCleanCollet(bBHZ2, bReturn))
	{
		MoveBHZToHome(bBHZ2);
		MoveBinTableToBondPosn(TRUE);
		return FALSE;
	}

	// make sure bin table move back as it is indexed already or postbond not done yet
	if (!MoveBinTableToBondPosn(TRUE))
	{
		return FALSE;
	}

	//v4.53A24
	if (CMS896AStn::m_bAutoChangeCollet && m_bACCSearchColletHole)	//v4.53A23
	{
		if (bBHZ2)		//If BHZ2 towards PICK	
		{
			//Search BHZ2 Collet Hole
			CMSLogFileUtility::Instance()->MS_LogOperation("SearchCollet BHZ2 ...");
			SearchCollet(TRUE);
		}
		else
		{
			//Search BHZ1 Collet Hole
			CMSLogFileUtility::Instance()->MS_LogOperation("SearchCollet BHZ1 ...");
			SearchCollet(FALSE);
		}

		// make sure bin table move back as it is indexed already or postbond not done yet
		if (!MoveBinTableToBondPosn(TRUE))
		{
			return FALSE;
		}
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("OpMS100AutoCleanCollet: Auto Learn Z Level ...");
	if (!OpAutoLearnBHZPickBondLevel(bBHZ2))
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("OpMS100AutoCleanCollet: Auto Learn Z Level fails");
		SetErrorMessage("BH: autoLearn Z levels fail in OpAutoCleanCollet");
		return FALSE;	//ABORT AUTOBOND mode if any motor is dead
	}

	if (bBHZ2)
	{
		OpPrestartColletHoleCheck(FALSE, FALSE);
	}
	else
	{
		OpPrestartColletHoleCheck(TRUE, FALSE);
	}

	if (bReturn == FALSE)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("OpMS100AutoCleanCollet fails");
		return FALSE;
	}

	SetAlarmLamp_Green(FALSE, TRUE); //4.53D21 after Acc and learn Z level
	CMSLogFileUtility::Instance()->MS_LogOperation("*OpMS100AutoCleanCollet done");
	return TRUE;
}


BOOL CBondHead::OpMS100AutoCleanCollet_MDCJ(BOOL bMissingDie)	//v4.44A1	//SEmitek
{
	BOOL bReturn = TRUE;

	//m_bSearchColletNow1 = TRUE;
	//m_bSearchColletNow2 = TRUE;//20190610, trigger search collet hole

	CloseAlarm();
	SetAlarmLamp_Green(FALSE,TRUE);
	if (bMissingDie)
	{
		SaveAutoErrorHandlingLog("Clean Collet,Missing Die", m_psmfSRam);
		SetAlert_WarningMsg(IDS_MS_WARNING_CLEAN_COLLET, "Clean Collet(Missing Die)...Please Wait");
	}
	else
	{
		SaveAutoErrorHandlingLog("Clean Collet,Collet Jam", m_psmfSRam);
		SetAlert_WarningMsg(IDS_MS_WARNING_CLEAN_COLLET, "Clean Collet(Collet jam)...Please Wait");
	}

	Sleep(100);

	CTime stStartTime = CTime::GetCurrentTime();
	CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet MDCJ start");

	if (m_bMS100DigitalAirFlowSnr)	
	{
		BH_Z_AirFlowLog(IsBHZ2ToPick(), "AutoCleanCollet");
	}

	// make sure bin table move back as it is indexed already or postbond not done yet
	if (!MoveBinTableToBondPosn(FALSE))
	{
		SetAlarmLamp_Green(FALSE, TRUE);
		CloseAlarm();
		return FALSE;
	}

	// should the missing/jam die check complete
	// bond arm move to clean position and bond head up to swing level
	Z_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
	Sleep(50);

	BOOL bBHZ2 = bMissingDie ? IsBHZ1ToPick() : !IsBHZ1ToPick();
	if (!bMissingDie)
	{
		//1. Move to swing level
		if (!MoveToPrepick(bBHZ2))
		{
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			return FALSE;
		}

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		LONG lCount = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Clean Collet Strong Blow Count"), 1);
		MoveToPrepickStrongBlow(bBHZ2, lCount, 2000, 500);
		//Strong blow should do before check threshold
		if (!IsDoCleanCollet(bBHZ2))
		{
			SetAlarmLamp_Green(FALSE,TRUE);
			CloseAlarm();
			if (IsBHZ1ToPick())		//If BHZ1	
			{
				m_lCJContinueCounter1--;
			}
			else
			{
				m_lCJContinueCounter2--;
			}

			return TRUE;  //no do collet-clean
		}
		if (!BHStrongBlowColletDieAtPrepick(bBHZ2))
		{
			MoveBinTableToBondPosn(TRUE);
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			return FALSE;
		}
	}


	// should the missing/jam die check complete
	// bond arm move to clean position and bond head up to swing level
	MoveBHZToSwingLevel(bBHZ2);
	Sleep(50);
	T_SafeMoveToInAuto(bBHZ2 ? m_lPickPos_T : m_lBondPos_T, "Op MS100 ACC");	// always arm 1 base, must at bond positon to clean collet

	// let bin table move to alcohol dropping position
	if (!BTMoveToACCLiquid())
	{
		MoveBHZToHome(bBHZ2);
		MoveBinTableToBondPosn(TRUE);
		T_SafeMoveToInAuto(m_lPrePickPos_T, "OpMS100AutoCleanCollet Move To PrePick");
		SetAlarmLamp_Green(FALSE, TRUE);
		CloseAlarm();
		return FALSE;
	}

	// press the alcohol to the cloth
	CleanColletPocketAlcoholOnce();

	//Clean Collet with Bin Table
	if (!OpBTAutoCleanCollet(bBHZ2, bReturn))
	{
		MoveBinTableToBondPosn(TRUE);
		SetAlarmLamp_Green(FALSE, TRUE);
		CloseAlarm();
		return FALSE;
	}

	IsAllMotorsEnable();

	if (/*CMS896AStn::m_bAutoChangeCollet &&*/ m_bACCSearchColletHole)
	{
		CMSLogFileUtility::Instance()->BH_LogStatus(bBHZ2? "SearchCollet BHZ2 ..." : "SearchCollet BHZ1 ...");

		if (!SearchCollet(bBHZ2))
		{
			CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet_MDCJ: SearchCollet failure");
			SetErrorMessage("OpMS100AutoCleanCollet_MDCJ: SearchCollet failure");
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			return FALSE;	//ABORT AUTOBOND mode if any motor is dead
		}
	}

	// make sure bin table move back as it is indexed already or postbond not done yet
	if (!MoveBinTableToBondPosn(TRUE))
	{
		SetAlarmLamp_Green(FALSE, TRUE);
		CloseAlarm();
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet_MDCJ: Auto Learn Z Level ...");
	if( !OpAutoLearnBHZPickBondLevel(bBHZ2))	
	{
		CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet_MDCJ: Auto Learn Z Level fails");
		SetErrorMessage("BH: autoLearn Z levels fail in OpAutoCleanCollet");
		SetAlarmLamp_Green(FALSE, TRUE);
		CloseAlarm();
		return FALSE;	//ABORT AUTOBOND mode if any motor is dead
	}

	if (bBHZ2)
	{
		OpPrestartColletHoleCheck(FALSE, FALSE);
	}
	else
	{
		OpPrestartColletHoleCheck(TRUE, FALSE);
	}
	//if (1)
	//{
	//	if (bBHZ2)
	//	{
	//		m_bAutoLearnPickLevelZ2 = TRUE;
	//		DisplaySequence("BH - Auto Learn Pick LevelZ2 Next cycle");
	//	}
	//	else
	//	{
	//		m_bAutoLearnPickLevelZ1 = TRUE;
	//		DisplaySequence("BH - Auto Learn Pick LevelZ1 Next cycle");
	//	}
	//}

	CMSLogFileUtility::Instance()->BH_LogStatus("OpMS100AutoCleanCollet_MDCJ done");

	//v4.54A7
	if (bMissingDie && (m_ulMaxCleanCount > 0) && (m_ulCleanCount > m_ulMaxCleanCount))	
	{
		m_ulCleanCount = 0;
		m_dLastPickTime = GetTime();	// ms100 after second head done
		if (m_pulStorage != NULL)
		{
			*(m_pulStorage + BH_CLEANCOUNT_OFFSET) = 0;
		}

		SetErrorMessage("auto clean collet reset (MDJ)");
		SetStatusMessage("auto clean collet reset (MDJ)");
	}


	//if( bMissingDie )
	//{
		CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AutoCleanColletbyMD");//Requested by Harry 2019.04.03
	//}
	//else
	//{
	//	CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AutoCleanColletbyJC");
	//}


	// should the missing/jam die check complete
	// bond arm move to clean position and bond head up to swing level
	SetBHPickVacuum(bBHZ2, TRUE);

	if (bMissingDie)
	{
/*
		if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
		{
			T_SafeMoveToInAuto(m_lBondPos_T, "Op MS100 ACC");	// always arm 1 base, must at bond positon to clean collet
		}
		else
		{
			T_SafeMoveToInAuto(m_lPickPos_T, "Op MS100 ACC");	// always arm 1 base, must at bond positon to clean collet
		}
*/
	}
	else
	{
		if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
		{
			T_SafeMoveToInAuto(m_lPickPos_T, "Op m_lPickPos_T");	// always arm 1 base, must at bond positon to clean collet
		}
		else
		{
			T_SafeMoveToInAuto(m_lBondPos_T, "Op m_lBondPos_T");	// always arm 1 base, must at bond positon to clean collet
		}
	}
	Sleep(50);

	SetAlarmLamp_Green(FALSE, TRUE);
	CloseAlarm();
	if (bReturn == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}



BOOL CBondHead::IsReachACCMaxTimeOut()
{
	if( IsBurnIn() )
	{
		return FALSE;
	}
	if( m_ulCleanColletTimeOut>0 )
	{
		ULONG ulTimeUsed = (ULONG)fabs(GetTime()-m_dLastPickTime);
		if( ulTimeUsed>(m_ulCleanColletTimeOut*60*1000) )
		{
			CString szMsg;
			szMsg.Format("ACC used time %lu(%lu), curr time %f, last time %f", 
				m_ulCleanColletTimeOut, ulTimeUsed, GetTime(), m_dLastPickTime);
			CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
			return TRUE;
		}
	}
	return FALSE;
}	// Is Reach to ACC max time out

BOOL CBondHead::IsReachACCMaxCount()
{
	if( IsBurnIn() )
	{
		return FALSE;
	}

	if ((m_ulMaxCleanCount > 0) && (m_ulCleanCount >= m_ulMaxCleanCount))
	{
		//v3.93
		SetStatusMessage("BH Clean count is reached");
		SetErrorMessage("BH Clean count is reached");
		return TRUE;
	}

	return FALSE;
}


