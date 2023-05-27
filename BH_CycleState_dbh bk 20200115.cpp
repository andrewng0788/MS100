/////////////////////////////////////////////////////////////////
// BH_CycleState.cpp : Cycle Operation State of the CBondHead class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Saturday, December 4, 2004
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "CycleState.h"
#include "math.h"
#include "BondPr.h"
#include "Mmsystem.h"
#include "WT_SubRegion.h"
#include "BH_AirFlowCompensation.h"
#include "WaferTable.h"
#include "WaferPr.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBondHead::WaitForDelayToEjectorUpMove(const LONG lTime_Ej, const LONG lEjectorUpDelay, const DOUBLE s_dbStartZ2PK)
{
	LONG lBHZPickTime = m_lPickTime_Z;
	if (!IsBHZ1ToPick())
	{
		lBHZPickTime = m_lPickTime_Z2;
	}

	LONG lExtraEjectorUpDelay = 0;
	if ((m_lHeadPickDelay + m_lEjectorUpDelay + m_lPickDelay) < m_lHeadBondDelay + m_lBondDelay)
	{
		lExtraEjectorUpDelay = (m_lHeadBondDelay + m_lBondDelay) - (m_lHeadPickDelay + m_lEjectorUpDelay + m_lPickDelay);
	}

	LONG lDelay = lTime_Ej - lEjectorUpDelay - lExtraEjectorUpDelay;			//Time of BHZ travel to this moment

	if (m_bEnableSyncMotionViaEjCmd && (lDelay > 0))
	{
		//Ejector Up delay must be more than ej moving time.
		lDelay = 0;
	}

	LONG lTemp = _round(GetTime() - s_dbStartZ2PK);		//Time before BHZ motion complete that Ej moves UP

	LONG lNewDelay = lBHZPickTime - lTemp - lDelay - 1;

	if (lNewDelay > 0)
	{
		Sleep(lNewDelay);
	}
}


LONG CBondHead::GetActualPickDelay(const DOUBLE dTime_Ej, const LONG lEjectorUpDelay, const LONG lPickDelay, const DOUBLE s_dbStartZ2PK, const DOUBLE dCurTime)
{
	LONG lBHZPickTime = m_lPickTime_Z;
	if (!IsBHZ1ToPick())
	{
		lBHZPickTime = m_lPickTime_Z2;
	}

	DOUBLE dActualEjectorTime = dTime_Ej;
	if (dActualEjectorTime < lEjectorUpDelay)
	{
		//Ejector Up delay is more than ej moving time and will wait for (lEjectorUpDelay - lTime_Ej) and move ejector.
		dActualEjectorTime = lEjectorUpDelay;
	}

	DOUBLE dTemp = dCurTime - s_dbStartZ2PK;		//Time before BHZ motion complete that Ej moves UP

	DOUBLE dActualPickDelay = lBHZPickTime + dActualEjectorTime + lPickDelay - dTemp;
	if (lPickDelay <= 0)
	{
		dActualPickDelay = lBHZPickTime + dActualEjectorTime - dTemp;
	}

	LONG lActualPickDelay = _round(dActualPickDelay);
	if (m_lSyncTriggerValue <= 30)
	{
		//becasue m_lSyncTriggerValue will extra the calculating time of sync pick move if m_lSyncTriggerValue is less than 10
		lActualPickDelay = _round(dActualPickDelay - 1);
	}

	return lActualPickDelay;
}


VOID CBondHead::WaitForPickDelay(const DOUBLE dTime_Ej, const LONG lEjectorUpDelay, const LONG lPickDelay, const DOUBLE s_dbStartZ2PK)
{
	LONG lActualPickDelay = GetActualPickDelay(dTime_Ej, lEjectorUpDelay, lPickDelay, s_dbStartZ2PK, GetTime());
	if (lActualPickDelay > 0)
	{
		Sleep(lActualPickDelay);
	}
}


LONG CBondHead::GetActualBondDelay(const DOUBLE dBondZDownStart, const DOUBLE dCurTime)
{
	//Wait for Bond delay
	LONG lBHZBondTime = m_lBondTime_Z;
	if (IsBHZ1ToPick())			//If BHZ1 at PICK	
	{
		lBHZBondTime = m_lBondTime_Z2;
	}

	LONG lActualBondDelay = lBHZBondTime + m_lBondDelay - _round(dCurTime - dBondZDownStart);

	return lActualBondDelay;
}


VOID CBondHead::WaitForBondDelay(const DOUBLE dBondZDownStart)
{
	//Wait for Bond delay
	LONG lBHZBondTime = m_lBondTime_Z;
	if (IsBHZ1ToPick())			//If BHZ1 at PICK	
	{
		lBHZBondTime = m_lBondTime_Z2;
	}

	WaitForDelay(lBHZBondTime + m_lBondDelay, dBondZDownStart);
}


VOID CBondHead::WaitTurnOffStrongBlow(const LONG lActualNeutralDelay, const DOUBLE dBondZDownStart)
{
	if (IsBHZ1ToPick())			//If BHZ1 at PICK	
	{
		WaitForDelay(m_lBondTime_Z2 + lActualNeutralDelay, dBondZDownStart);
		SetStrongBlowZ2(FALSE);			//BHZ2 to WEAKBLOW
	}
	else
	{
		WaitForDelay(m_lBondTime_Z + lActualNeutralDelay, dBondZDownStart);
		SetStrongBlow(FALSE);			//BHZ1 to WEAKBLOW
	}
}


VOID CBondHead::WaitForDelay(const LONG lDelayTime, const DOUBLE dStartTime)
{
	LONG lDelay = lDelayTime - _round(GetTime() - dStartTime);
	if (lDelay > 0)
	{
		Sleep(lDelay);
	}
}




BOOL CBondHead::CheckColletJamCount(BOOL &bIsColletJam, BOOL &bReachedColletJamCount)
{
	bReachedColletJamCount = FALSE;
	if (m_ulColletJam_Retry > 0)
	{
		bIsColletJam = OpCheckColletJam();
		if (bIsColletJam)
		{
			if (m_nBHAlarmCode == 6)
			{
				//collet jam should be cleared 
				OpSetMS100AlarmCode(0);
				CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
				if (pBondPr != NULL)
				{
					pBondPr->DecreaseDieEmptyCount(IsBHZ1ToPick());
				}
			}
		}
		BOOL bDoAuto = TRUE;
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "ChangeLight(XM)" && !pApp->m_bOperator)
		{
			bDoAuto = FALSE;
		}
		if (m_bAutoCleanCollet && bIsColletJam && bDoAuto)
		{
			LONG lCJContinueCounter = 0;
			if (IsBHZ1ToPick())		//If BHZ1	
			{
				m_lCJContinueCounter1++;
				lCJContinueCounter = m_lCJContinueCounter1;
			}
			else
			{
				m_lCJContinueCounter2++;
				lCJContinueCounter = m_lCJContinueCounter2;
			}

			if (lCJContinueCounter <= (LONG)m_ulColletJam_Retry)
			{
				if (!OpMS100AutoCleanCollet_MDCJ(FALSE))
				{
					return FALSE;
				}
			}
			else
			{
				bReachedColletJamCount = TRUE;
			}
		}
	}

	return TRUE;
}


VOID CBondHead::MissingDieThreshold_Log(const BOOL bBHZ2, CString szText)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	ULONG lRTUpdateMissingDie = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD);
	if ((lRTUpdateMissingDie == 1) || (lRTUpdateMissingDie == 2))
	{
		CMSLogFileUtility::Instance()->MissingDieThreshold_Log(bBHZ2, szText);
	}
}


VOID CBondHead::LogMissingDieThresholdValue(const BOOL bBHZ1)
{
	CString szMess;
	ULONG ulBHZAirFlowValue[3];

	if (bBHZ1)
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH1AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ1DAirFlowValue;
			}
		}

		szMess.Format(",,MissingDie,BHZ1,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulColletCount, m_dBHTThermostatReading);
		MissingDieThreshold_Log(FALSE, szMess);
	}
	else
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH2AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ2DAirFlowValue;
			}
		}

		szMess.Format(",,MissingDie,BHZ2,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulCollet2Count, m_dBHTThermostatReading);
		MissingDieThreshold_Log(TRUE, szMess);
	}
}

VOID CBondHead::LogAutoCleanColletThresholdValue(const BOOL bBHZ1)
{
	CString szMess;
	ULONG ulBHZAirFlowValue[3];

	if (bBHZ1)
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH1AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ1DAirFlowValue;
			}
		}

		szMess.Format(",,AutoCleanCollet,BHZ1,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulColletCount, m_dBHTThermostatReading);
		MissingDieThreshold_Log(FALSE, szMess);
	}
	else
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH2AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ2DAirFlowValue;
			}
		}

		szMess.Format(",,AutoCleanCollet,BHZ2,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulCollet2Count, m_dBHTThermostatReading);
		MissingDieThreshold_Log(TRUE, szMess);
	}
}

VOID CBondHead::LogColletJamThresholdValue(const BOOL bBHZ1)
{
	CString szMess;
	ULONG ulBHZAirFlowValue[3];

	if (bBHZ1)
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH1AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ1DAirFlowValue;
			}
		}

		szMess.Format(",,ColletJam,BHZ1,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulColletCount, m_dBHTThermostatReading);
		MissingDieThreshold_Log(FALSE, szMess);
	}
	else
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH2AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ2DAirFlowValue;
			}
		}

		szMess.Format(",,ColletJam,BHZ2,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulCollet2Count, m_dBHTThermostatReading);
		MissingDieThreshold_Log(TRUE, szMess);
	}
}



VOID CBondHead::RuntimeUpdateColletJamThresholdValue()
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	ULONG lRTUpdateMissingDie = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD);
	if (lRTUpdateMissingDie == 0)
	{
		return;
	}

	if (!m_bMS100DigitalAirFlowSnr)
	{
		return;
	}

	unsigned long  ulBHZAirFlowValue[3];
	if (IsBHZ1ToPick())
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH1AirFlowValue())
			{
				m_pBHZ1AirFlowCompensation->SetEmptyAirFlowValue(m_ulBHZ1DAirFlowValue);
				ulBHZAirFlowValue[i] = m_ulBHZ1DAirFlowValue;
			}
		}
		CString szMess;
		szMess.Format("Read,BHZ1,ColletJam,Threshold,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulColletCount, m_dBHTThermostatReading);
		MissingDieThreshold_Log(FALSE, szMess);
	}
	else
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH2AirFlowValue())
			{
				m_pBHZ2AirFlowCompensation->SetEmptyAirFlowValue(m_ulBHZ2DAirFlowValue);
				ulBHZAirFlowValue[i] = m_ulBHZ2DAirFlowValue;
			}
		}
		CString szMess;
		szMess.Format("Read,BHZ2,ColletJam,Threshold,%d,%d,%d,%d,%.1f", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulCollet2Count, m_dBHTThermostatReading);
		MissingDieThreshold_Log(TRUE, szMess);
	}
}

VOID CBondHead::RuntimeUpdateMissingDieThresholdValue(BOOL bBHZ1ToPick)
{
	if (bBHZ1ToPick/*IsBHZ1ToPick()*/)
	{
		m_pBHZ2AirFlowCompensation->ResetMissingDieCount();
	}
	else
	{
		m_pBHZ1AirFlowCompensation->ResetMissingDieCount();
	}

	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	ULONG lRTUpdateMissingDie = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD);
	if (lRTUpdateMissingDie == 0)
	{
		return;
	}

	if (!m_bMS100DigitalAirFlowSnr)
	{
		return;
	}

	unsigned long  ulBHZAirFlowValue[3];
	BOOL bValid = TRUE;
	if (bBHZ1ToPick/*IsBHZ1ToPick()*/)
	{
		//Get 3 times's value
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH2AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ2DAirFlowValue;
				ULONG ulBHZ2DAirFlowMissingDieThreshold =  GetBHZ2MissingDieThresholdValue(m_ulBHZ2DAirFlowUnBlockValue, m_ulBHZ2DAirFlowValue);
				if ((ulBHZ2DAirFlowMissingDieThreshold <= m_lBHZ1ThresholdLimit) || (ulBHZ2DAirFlowMissingDieThreshold >= m_lBHZ1ThresholdUpperLimit))
				{
					bValid = FALSE;
				}
			}
			else
			{
				bValid = FALSE;
			}
		}
		CString szMess;
		if (bValid)
		{
			for (short i = 0; i < 3; i++)
			{
				m_pBHZ2AirFlowCompensation->SetDieAirFlowValue(ulBHZAirFlowValue[i]);
			}
			szMess.Format("Read,BHZ2,MissingDie,Threshold,%d,%d,%d,%d,%.1f,%d,%d", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulCollet2Count, m_dBHTThermostatReading, m_ulBHZ2DAirFlowThreshold, m_ulBHZ2DAirFlowThresholdCJ);
		}
		else
		{
			szMess.Format("Read,BHZ2,MissingDie,Threshold(Invalid),%d,%d,%d,%d,%.1f,%d,%d", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulCollet2Count, m_dBHTThermostatReading, m_ulBHZ2DAirFlowThreshold, m_ulBHZ2DAirFlowThresholdCJ);
		}
		MissingDieThreshold_Log(TRUE, szMess);

		if (m_pBHZ2AirFlowCompensation->CalcAirFlowThreshold())
		{
			ULONG ulBHZ2DAirFlowThreshold = m_pBHZ2AirFlowCompensation->GetBHZMissingDieThreshold();
			if ((lRTUpdateMissingDie == 2) && 
				(ulBHZ2DAirFlowThreshold > m_lBHZ1ThresholdLimit) && (ulBHZ2DAirFlowThreshold < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ2DAirFlowThreshold = ulBHZ2DAirFlowThreshold;
			}

			//Threshold value for CJ
			ULONG ulBHZ2DAirFlowThresholdCJ = m_pBHZ2AirFlowCompensation->GetBHZColletJamThreshold();
			if ((lRTUpdateMissingDie == 2) && 
				(ulBHZ2DAirFlowThresholdCJ > m_lBHZ1ThresholdLimit) && (ulBHZ2DAirFlowThresholdCJ < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ2DAirFlowThresholdCJ = ulBHZ2DAirFlowThresholdCJ;
			}

			if (lRTUpdateMissingDie == 2)
			{
				szMess.Format("Update,BHZ2,MissingDie&ColletJam,Threshold,%d,%d", m_ulBHZ2DAirFlowThreshold, m_ulBHZ2DAirFlowThresholdCJ);
			}
			else
			{
				szMess.Format("Log,BHZ2,MissingDie&ColletJam,Threshold,%d,%d", ulBHZ2DAirFlowThreshold, ulBHZ2DAirFlowThresholdCJ);
			}
			MissingDieThreshold_Log(TRUE, szMess);
		}
	}
	else
	{
		for (short i = 0; i < 3; i++)
		{
			ulBHZAirFlowValue[i] = 0;
			if (ReadBH1AirFlowValue())
			{
				ulBHZAirFlowValue[i] = m_ulBHZ1DAirFlowValue;
				ULONG ulBHZ1DAirFlowMissingDieThreshold =  GetBHZ1MissingDieThresholdValue(m_ulBHZ1DAirFlowUnBlockValue, m_ulBHZ1DAirFlowValue);
				if ((ulBHZ1DAirFlowMissingDieThreshold <= m_lBHZ1ThresholdLimit) || (ulBHZ1DAirFlowMissingDieThreshold >= m_lBHZ1ThresholdUpperLimit))
				{
					bValid = FALSE;
				}
			}
			else
			{
				bValid = FALSE;
			}
		}
		CString szMess;
		if (bValid)
		{
			for (short i = 0; i < 3; i++)
			{
				m_pBHZ1AirFlowCompensation->SetDieAirFlowValue(ulBHZAirFlowValue[i]);
			}
			szMess.Format("Read,BHZ1,MissingDie,Threshold,%d,%d,%d,%d,%.1f,%d,%d", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulColletCount, m_dBHTThermostatReading, m_ulBHZ1DAirFlowThreshold, m_ulBHZ1DAirFlowThresholdCJ);
		}
		else
		{
			szMess.Format("Read,BHZ1,MissingDie,Threshold(Invalid),%d,%d,%d,%d,%.1f,%d,%d", ulBHZAirFlowValue[0], ulBHZAirFlowValue[1], ulBHZAirFlowValue[2], m_ulColletCount, m_dBHTThermostatReading, m_ulBHZ1DAirFlowThreshold, m_ulBHZ1DAirFlowThresholdCJ);
		}
		MissingDieThreshold_Log(FALSE, szMess);

		if (m_pBHZ1AirFlowCompensation->CalcAirFlowThreshold())
		{
			ULONG ulBHZ1DAirFlowThreshold = m_pBHZ1AirFlowCompensation->GetBHZMissingDieThreshold();
			if ((lRTUpdateMissingDie == 2) && 
				(ulBHZ1DAirFlowThreshold > m_lBHZ1ThresholdLimit) && (ulBHZ1DAirFlowThreshold < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ1DAirFlowThreshold = ulBHZ1DAirFlowThreshold;
			}

			//Threshold value for CJ
			ULONG ulBHZ1DAirFlowThresholdCJ = m_pBHZ1AirFlowCompensation->GetBHZColletJamThreshold();
			if ((lRTUpdateMissingDie == 2) && 
				(ulBHZ1DAirFlowThresholdCJ > m_lBHZ1ThresholdLimit) && (ulBHZ1DAirFlowThresholdCJ < m_lBHZ1ThresholdUpperLimit))
			{
				m_ulBHZ1DAirFlowThresholdCJ = ulBHZ1DAirFlowThresholdCJ;
			}

			if (lRTUpdateMissingDie == 2)
			{
				szMess.Format("Update,BHZ1,MissingDie&ColletJam,Threshold,%d,%d", m_ulBHZ1DAirFlowThreshold, m_ulBHZ1DAirFlowThresholdCJ);
			}
			else
			{
				szMess.Format("Log,BHZ1,MissingDie&ColletJam,Threshold,%d,%d", ulBHZ1DAirFlowThreshold, ulBHZ1DAirFlowThresholdCJ);
			}
			MissingDieThreshold_Log(FALSE, szMess);
		}
	}
}


LONG CBondHead::GetCheckBHMarkCycle()
{
	LONG lCheckBHMarkCycle = m_lCheckBHMarkCycle;
	if ((GetTime() - m_dStartTime) < 10 * 60 * 1000)
	{
		lCheckBHMarkCycle  = m_lCheckBHMarkCycle; 
	}
	else if ((GetTime() - m_dStartTime) < 20 * 60 * 1000)
	{
		lCheckBHMarkCycle  = m_lCheckBHMarkCycle * 2; 
	}
	else if ((GetTime() - m_dStartTime) < 30 * 60 * 1000)
	{
		lCheckBHMarkCycle  = m_lCheckBHMarkCycle * 4; 
	}
	else
	{
		lCheckBHMarkCycle  = m_lCheckBHMarkCycle * 10; 
	}

	return lCheckBHMarkCycle;
}


VOID CBondHead::RunOperation_dbh()
{
	CString szMsg, szTemp;
	LONG	lTemp = 0, lDelay = 0, lSPCResult = 0;	//v4.47T12
	LONG	lTotal = 0;
	LONG	lEjectorLvl = 0;
	LONG	lTime_Ej	= m_lTime_Ej;
	LONG	lDnTime_Ej	= m_lDnTime_Ej;	
	BOOL	bCriticalError=FALSE;
	BOOL	bWaferEnd, bColletFull, bLFDie;
	BOOL	bRegionEnd = FALSE;
	double	dCurrentTime = 0;
	static  LONG	lArmPicBondDelay = 0;
	static	BOOL	s_bUpdateNVRAM = TRUE;
	static	BOOL	s_bDownToPick = TRUE;
	static	DOUBLE	s_dbStartZ2EjUp = 0;
	static	DOUBLE	s_dbStartZ2PK = 0, s_dbStartT2PB = 0, s_dbSettling = 100;
	static	DOUBLE  s_dArmPickUpStart = 0, s_dBondZDownStart = 0;
	static  DOUBLE  s_dEjectorCapStartTime = 0;
	static  DOUBLE  s_dTotalTime_Ej = 0;
	static  DOUBLE  s_dBHEJSyncTime = 0;
	LONG lMotionTime_BT = 0;
	LONG lTMoveDelay = 0, lTMaxUnblockTime = 0, lTMinPrUnblockTime = 0;
	DOUBLE dWTIndexStart = 0, dWTIndexDuration = 0;
	LONG lEjVacOffDelay = 0;
	LONG lWTIndexDuration	= 0;
	LONG lEjCompleteTime	= 0;				//v4.04
	LONG lEjDnDuration		= 0;				//v4.22T1
	DOUBLE dEjDuration		= 0;				//v4.04
	LONG lWTLongJumpExDelay = 0;
	LONG lWTLongJumpMotionTime = 0;
	BOOL bIsNeedRestartUpdate = FALSE;
	BOOL bPostBondNextCycleStop = FALSE;	//v4.21T8	//Cree HuiZhou
	BOOL bWTNeedRotate = FALSE;
	INT nResult = gnOK;
	BOOL bMS60NGPickNoLFCycle = FALSE;			//v4.54A5
	BOOL bUpLookFailAtMD	= FALSE;
	LONG lLastPickRow		= 0;
	LONG lLastPickCol		= 0;
	BOOL bStatus			= TRUE;
	LONG lEnableSyncMotionViaEjCmdExtraTime = 0; //if enable SyncMotionViaEjCmd, it need more than 2 ms for ej up move
	BOOL bBHMarkStop = FALSE;
	LONG EmptyCounter, EmptyCounter2, AccEmptyCounter, AccEmptyCounter2, AccEmptyCounterTotal, GenMaxEmptyAllow, GenMaxAccEmptyAllow;
	static BOOL bSwitchPR			= FALSE;
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();

	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		DisplaySequence("BH - STOP");
		State(STOPPING_Q);
		return ;
	}

	m_bStep = FALSE;

	//Support of default HeadPick/HeadBond delay = -1
	LONG lHeadPickDelay = m_lHeadPickDelay;
	if (m_lHeadPickDelay <= -1)
	{
		lHeadPickDelay = m_lPickTime_Z;
	}
	
	LONG lHeadBondDelay = m_lHeadBondDelay;
	if (m_lHeadBondDelay <= -1)
	{
		lHeadBondDelay = m_lBondTime_Z;
	}
	
	LONG lArmPickDelay = m_lArmPickDelay;
	if (m_lArmPickDelay <= -1)
	{
		lArmPickDelay = m_lPickTime_Z;
	}

	LONG lArmBondDelay = m_lArmBondDelay;
	if (m_lArmBondDelay <= -1)
	{
		lArmBondDelay = m_lBondTime_Z;
	}

	LONG lEjectorUpDelay	= m_lEjectorUpDelay;
	LONG lEjectorDownDelay	= m_lEjectorDownDelay;

	if (m_bUseDefaultDelays)		//v2.97T1
	{
		lHeadPickDelay	= m_lPickTime_Z;
		lHeadBondDelay	= m_lBondTime_Z;
		lArmPickDelay	= m_lPickTime_Z;
		lArmBondDelay	= m_lBondTime_Z;
	}

	if (m_bEnableSyncMotionViaEjCmd == TRUE)
	{
		lTime_Ej = lTime_Ej + lEnableSyncMotionViaEjCmdExtraTime;
	}

	LONG lExtraPrGrabDelay = (*m_psmfSRam)["WaferPr"]["ExtraGrabDelay"];	//v.49A3

	LONG lAutobondCurrTime = 0;
	LONG lWTStop		= (*m_psmfSRam)["WaferTable"]["AtSafePos"];
	LONG lBTStop		= (*m_psmfSRam)["BinTable"]["AtSafePos"];
	BOOL bEnableBHUplookPr	= pAppMod->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);	//v4.52A16

	//v3.91
	BOOL bEMOStop = FALSE;
	if (CMS896AStn::m_bEMOChecking)
	{
		m_lEMOCheckCounter++;
		if (m_lEMOCheckCounter > 20)
		{
			m_lEMOCheckCounter = 0;
			bEMOStop = IsEMOTriggered();
			if (bEMOStop)
			{
				bCriticalError = TRUE;
				//(*m_psmfSRam)["MS899"]["CriticalError"] = bCriticalError;
				SetMotionCE(TRUE, "EMO Check fail");	//v4.59A19
			}
		}
	}

	//v4.50A6
	if (IsMS60() && CMS896AStn::m_bDBHThermostat)
	{
		m_lMS60TempCheckCounter++;

		if (m_lMS60TempCheckCounter > MS60_BH_THERMALCHECK_CYCLE)
		{
			m_lMS60TempCheckCounter = 0;
			if (IsMS60TempertureOverLoaded(TRUE, TRUE))		//v4.53A19
			{
				SetBondHeadFan(TRUE);	//v4.59A13

				HmiMessage_Red_Yellow("BonHead Temperature is overheat!  Machine is aborted!");
				bCriticalError = TRUE;
				//(*m_psmfSRam)["MS899"]["CriticalError"] = bCriticalError;
				SetMotionCE(TRUE, "MS60 Temperature is over-heat");	//v4.59A19
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
		}
	}

	try
	{
		if (IsMotionCE() == TRUE)
		{
			DisplaySequence("BH - critical error ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		if (IsBLBCRepeatCheckFail())
		{
			DisplaySequence("BH - |BL BC repeat fail| ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
		}

		m_nLastError = gnOK;
		switch (m_qSubOperation)
		{
		case WAIT_WT_Q:
			DisplaySequence("BH - dbh wait WT Q");
			bWaferEnd	= IsWaferEnded();

			if( IsAutoRescanWafer() && (GetWftSampleState()==1 || GetWftCleanEjPinState()==1) && m_bFirstCycle )	// wait wt, first cycle
			{
				if (IsEnableBHMark() || IsEnableColletHole())
				{
					if (!WaitSearchMarkDone(200))
					{
						break;
					}
				}

				CString szMsg;
				if (GetWftSampleState()==1)
				{
					m_lReadyToSampleOnWft = 2;	//	wait wt, first cycle
					szMsg = "BH - DBH at wait wt first cycle, to sample from 1 to 2";
				}
				else
				{
					m_lReadyToCleanEjPinOnWft = 2;	//	wait wt, first cycle
					szMsg = "BH - DBH at wait wt first cycle, to Clean Ej Pin from 1 to 2";
				}
				
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				pUtl->PrescanMoveLog(szMsg);
				DisplaySequence(szMsg);
			}

			// Check whether the collet & ejector count is larger than the user-defined maximum
			if (lWTStop)
			{
				if (m_bStop)
				{
					m_bDBHStop = TRUE;
					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					DisplaySequence("BH - Wait WT lWTstop ==> T Prepick to pick");
				}
				else
				{
					DisplaySequence("BH - WT stop ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			else if (IsMaxColletEjector() && !m_bEjPinNeedReplacementAtUnloadWafer)
			{
				m_bStop = TRUE;
				m_bDBHStop = TRUE;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait WT reach life limit ==> T Prepick to pick");
			}
			else if (m_bCheckPr2DCode && !m_bIs2DCodeDone)	//v4.40T1	//PLLM MS109 with BH 2D BC
			{
				CString szCode = _T("");
				BOOL b2DStatus = Search2DBarCodePattern(szCode, IsBHZ1ToPick(), TRUE);
				m_bIs2DCodeDone = TRUE;
				DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy2 Done");

				if (!b2DStatus)		//v4.41T3
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			//Only receive RPY2 here when BIN_FULL to avoid waiting for too long
			else if (IsBinFull() && bEnableBHUplookPr && m_lUpLookPrStage==2)
			{
				BOOL b2DStatus = TRUE;
				b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				m_lUpLookPrStage = 0;
				DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy2 Bin Full Done");
			}
			else if (bEnableBHUplookPr && m_lUpLookPrStage==1)		//v4.52A16	//v4.57A11
			{
				BOOL b2DStatus = TRUE;
				if (!m_bFirstCycle && !m_bChangeGrade)
				{
					T_Sync();
					TakeTime(T2);

					if (m_lUplookDelay > 0)
						Sleep(m_lUplookDelay);

					b2DStatus = UplookPrSearchDie_Rpy1(!m_bBHZ2TowardsPick);
					//TakeTime(LC3);	//v4.57A13	//v4.59A3
					if(b2DStatus)
					{
						m_lUpLookPrStage = 2;
						DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy1 Done");
					}
					else
					{
						DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy1 Fail");
					}
				}

				if (!b2DStatus)	
				{
					//SetErrorMessage("BHZ Uplook PR Search Failure to machine stop");
					//m_qSubOperation = HOUSE_KEEPING_Q;
				}
			}
			else if (WaitWTReady() || bWaferEnd || IsSortTo2ndPart() || IsOnlyRegionEnd())	//v3.98T1 add WaferEnd checking
			{	
				//TakeTime(LC4);	//v4.57A13	//v4.59A2
				SetWTReady(FALSE, "SetWTReady FALSE WAIT_WT_Q");
				SetPRStart(FALSE);
				SetPRLatched(FALSE);
				SetEjectorReady(FALSE, "1");	//	Wait WT Q; WT moved or wafer/region end
				SetDieBondedForWT(FALSE);
				SetConfirmSrch(FALSE);
				SetWTStartMove(FALSE);			//v3.89		//Clear it once again as die-ready means WT not yet indexed for sure!
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait WT ready ==> T Prepick to pick");
			}
			else if (m_bStop)	// && 	//v3.68T5	//v3.84	
			{
				//Allow BH to bond last-die on collet if bondarm is waiting at PRE-PICK
				if (m_bFirstCycle || m_bChangeGrade)
				{
					szMsg.Format("BH - Wait WT |firstcycle=%d or changegrade=%d| ==> HK", m_bFirstCycle, m_bChangeGrade);
					DisplaySequence(szMsg);
					m_qSubOperation = HOUSE_KEEPING_Q;	
				}
				else
				{
					if (IsMS90() && bEnableBHUplookPr&& (IsSortTo2ndPart() || m_b2PartsAllDone))	//andrewng
						;
					else
						m_bDBHStop = TRUE;
					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					DisplaySequence("BH - Wait WT stop ==> T Prepick to pick");
				}
			}
			else
			{
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

				if (s_bUpdateNVRAM == TRUE)
				{
					SetBackupNVRam(TRUE);
					s_bUpdateNVRAM = FALSE;
				}
			}

			break;


		case MOVE_T_PREPICK_TO_PICK_Q:
			if (IsAutoSampleSort())	// rescan block camera
			{
				if( IsAutoRescanWafer() )
				{
					if (m_bStop)
					{
						DisplaySequence("BH - |Wait rescanning| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					szMsg = "BH - DBH at T prepick to pick, rescanning";
					DisplaySequence(szMsg);
					break;
				}

				if( GetWftSampleState()>1 && m_bFirstCycle==FALSE && //	OR LET THE ADV SAMPLE COUNTER + 2 TO DELAY IT
					!IsBTChngGrd())	// if changing grade, wt at wait ej ready (not sampling), here should go on and sampling would do at opmovetable
				{
					if (m_bStop)
					{
						DisplaySequence("BH - |Wait sampling| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					szMsg = "BH - DBH at T prepick to pick, sampling waiting";
					DisplaySequence(szMsg);
					break;
				}
			}

			if (GetWftSampleState() == 1)
			{
				CString szMsg;
				m_lReadyToSampleOnWft = 2;
				szMsg = "BH - DBH at T prepick to pick, to sample from 1 to 2";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				DisplaySequence(szMsg);
			}
			else if (GetWftCleanEjPinState() == 1)
			{
				CString szMsg;
				m_lReadyToCleanEjPinOnWft = 2;
				szMsg = "BH - DBH at T prepick to pick, to Clean Ej Pin from 1 to 2";
				CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
				DisplaySequence(szMsg);
			}

			if (IsOnlyRegionEnd())
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
				pUtl->RegionOrderLog("T Prepick to pick q, region end, to house keeping");
			}
			else if (AllowMove() == TRUE)
			{
				bWaferEnd		= IsWaferEnded();
				LONG lRefDieCheck	= (*m_psmfSRam)["DieInfo"]["RefDie"]["Check"];
				//Check BHT motion is completed before start other action
				if (T_IsComplete() == FALSE)
				{
					T_Sync();				
				}

				//** Wait for BT-Start to avoid extra head-PICK delay at PICK side **//
				if (m_bStop && (m_bFirstCycle || m_bChangeGrade || CMS896AStn::m_bBTAskBLBinFull==2))
				{
					szMsg.Format("BH - T Prepick to pick bstop |firstcycle=%d or changegrade=%d| ==> HK",
						m_bFirstCycle, m_bChangeGrade);
					DisplaySequence(szMsg);
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}

				if( IsMS90() && !WaitBTStartMove(200) )
				{
					if (bEnableBHUplookPr && m_lUpLookPrStage==2)
					{
						BOOL b2DStatus = TRUE;
						b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
						m_lUpLookPrStage = 0;
						DisplaySequence("BH - MS90 |WAIT WT | Uplook Rpy2 wait bt start move too long");
					}
					if (m_bStop && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE)
					{
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
					if (m_bStop )
					{
						if( CMS896AStn::m_bBTAskBLBinFull==2 || m_bDisableBL)
						{
							szMsg.Format("BH - 90 T Prepick to pick |bin loader error| ==> HK");
							DisplaySequence(szMsg);
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
					}
					DisplaySequence("BH - MS90 T Prepick to pick wait BT start move");
					break;
				}	//	for MS90 only
				else if (!IsMS90() && !WaitBTStartMove(200))	/*MS90 feature*/
				{
					if (bEnableBHUplookPr && m_lUpLookPrStage==2)
					{
						BOOL b2DStatus = TRUE;
						b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
						m_lUpLookPrStage = 0;
						DisplaySequence("BH - DBH |WAIT WT | Uplook Rpy2 wait bt start move too long");
					}
					if (m_bStop)
					{
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
					else if (IsBTChngGrd() && CMS896AStn::m_bMS100OriginalBHSequence == FALSE)	//v4.15T7
					{
						//Normal CHANGE-GRADE triggering position in MS100 sequence when BH at PREPICK
						(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
						CMSLogFileUtility::Instance()->BL_LogStatus("BH: reset change grade state");

						if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
						{
							CString szLog;
							szLog.Format("BH: EjtXY ChangeGrade 1; BH2 = %d", IsBHZ1ToPick()); 
							CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

							(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"] = TRUE;	//v4.47A3
							m_bChangeGrade	= TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
							szMsg = "BH - Prepick To Pick changegrade=1 with EjtXY";
						}
						else
						{
							CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 1");	//v4.47T10
							CMSLogFileUtility::Instance()->BT_BackupTableIndexLog();

							m_bFirstCycle		= TRUE;		
							m_bBHZ2TowardsPick	= FALSE;			//BH1 at PICK = FALSE
							(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
							(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"] = TRUE;	//v4.47T9
							szMsg = "BH - Prepick To Pick firstcycle=1 without EjtXY";
						}
						DisplaySequence(szMsg);

						m_lBHStateCode		= 0;
						m_nBHAlarmCode		= 0;	
						m_nBHLastAlarmCode	= 0;
						break;
					}
					DisplaySequence("BH - T Prepick to pick wait BT start move");
					break;
				}

				if ((CMS896AApp::m_bBondHeadILCFirstCycle) && (IsEnableILC() == TRUE) && m_bFirstCycle)
				{	
					if( WaitBTStartMove() )
					{
						if (IsMS60())	//v4.49A2
						{
							EnableBHTThermalControl(FALSE, TRUE);	//v4.49A5
						}

						CMS896AApp::m_bBondHeadILCFirstCycle = FALSE;
						m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;//ILC_RESTART_UPDATE_Q;

						DisplaySequence("BH - T Prepick to pick ==> ILC Restart update q");
					}
					break;
				}
	
				if (bWaferEnd && lRefDieCheck == 1)
				{
					DisplaySequence("BH - |Wafer End, at prepick, stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}

				if( m_bCycleFirstProbe )
				{
					if( IsReachACCMaxTimeOut() )
					{
						DisplaySequence("BH - start first cycle, ACC time out, do auto clean");
						BOOL bOldState = m_bBHZ2TowardsPick;
						m_ulCleanCount = m_ulMaxCleanCount;
						INT nColletCleanStatus = IsCleanCollet();
						if( nColletCleanStatus==1 )
						{
							DisplaySequence("BH - Prestart ACC error 1");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
						m_ulCleanCount++;
						if( bOldState )
							m_bBHZ2TowardsPick = FALSE;
						else
							m_bBHZ2TowardsPick = TRUE;
						DisplaySequence("BH - start first cycle, one head done, switch to another");
						nColletCleanStatus = IsCleanCollet();
						m_bBHZ2TowardsPick = bOldState;
						if( nColletCleanStatus==1 )
						{
							DisplaySequence("BH - Prestart ACC error 2");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
						DisplaySequence("BH - start first cycle, both done");
					}

					if( m_lAutoLearnLevelsIdleTime>0 )	//	do auto learn p/b levels after start sort. WT and BT should be OK.
					{
						DisplaySequence("BH - start first cycle, Auto learn pickbond levels begin");
						if( OpAutoLearnPickBondLevels(TRUE)==FALSE )
						{
							DisplaySequence("BH - start first cycle, Auto learn pickbond levels fail, house keeping");
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
						DisplaySequence("BH - start first cycle, Auto learn pickbond levels done");
					}

					m_bCycleFirstProbe = FALSE;
				}

				if( IsBTChngGrd() )	// HuaMao green light idle, prepick to pick, reset
				{
					SetEjectorReady(FALSE, "2");
					(*m_psmfSRam)["BinTable"]["Change Grade"]  = FALSE;
					DisplaySequence("BH - T Prepick to Pick (BT start move, ejector not ready, change grade to false)");
				}

				if( !m_bOnVacuumAtPick )	//	prepick
				{
					TakeTime(PV1);	
					if (IsBHZ1ToPick())			//If BHZ1 towards PICK
					{
						SetPickVacuum(TRUE); 
					}	
					else
					{
						SetPickVacuumZ2(TRUE);
					}
				}

				//v4.49A11
				BOOL bBinChanged	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinChangedForBH"];			//v4.49A11
				if (bBinChanged)
				{
					(*m_psmfSRam)["BinTable"]["BinChangedForBH"] = FALSE;
					if (pAppMod->GetFeatureStatus(MS896A_FUNC_BINBLK_GELPAD))
					{
						OpAutoLearnBHZPickBondLevel(IsBHZ1ToPick(), TRUE);
					}
				}

				//v4.59A39
				if (m_bFirstCycle /*|| m_bChangeColletDone*/)
				{
					bStatus = OpMoveEjectorTableXY(TRUE, SFM_WAIT);
//					m_bChangeColletDone = FALSE;
				}
				else
				{
					if (m_bWaferPrMS60LFSequence)
						bStatus = TRUE;
					else
						bStatus = OpMoveEjectorTableXY(FALSE, SFM_WAIT);
				}
				if (!bStatus)		//v4.42T3
				{
					SetErrorMessage("Ejector XY module is off power!");
					SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("BH - move ejt table error -> house keeping");
					break;
				}

				m_bDetectDAirFlowAtPrePick = TRUE;		//v4.52A12
				if (m_bMS100DigitalAirFlowSnr)	
				{
				//	DisplaySequence("BH - DBH at T prepick to pick, Digi air flow snr");
					BH_Z_AirFlowLog(IsBHZ1ToPick(), "PrePick to PICK,");
				}

				// Wait for Pick Vacuum settling
				lDelay = (LONG)(s_dbSettling + m_lPickTime_Z - lHeadPickDelay - m_lPrePickTime_T);
				if (lDelay > 0)
				{
					//if (!IsEjtUpAndPreheat())	//v4.47T8
					//	Sleep(lDelay);
				}

				//CSP
				BOOL bPreBondAlignPad	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];	//CSP
				if (bPreBondAlignPad)
				{
					//Need to add extra delay for BT to perform Pad alignment motion
					if ((m_lHeadPrePickDelay > 0) && (m_lHeadPrePickDelay <= 250))
					{
						Sleep(m_lHeadPrePickDelay + 20);
						//Sleep(m_lHeadPrePickDelay ); //DavidTest
					}
					else
					{
						Sleep(50);
					}
				}
	
				if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
				{
					m_lUpLookPrStage = 0;	//reset
					BOOL b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
					DisplaySequence("BH - get uplook rpy2");
					if(!b2DStatus)
					{
						SetErrorMessage("BHZ *Uplook PR Search Rpy2 Failure to machine stop");
						m_bUplookResultFail = TRUE;		//v4.57A11
					}
				}

				if (bPreBondAlignPad && !m_bFirstCycle)	
				{
					
					LONG lPreBondDone	= (LONG)(*m_psmfSRam)["BinTable"]["PreBondDone"];
					if (lPreBondDone==0)	//Must check PreAlign result before BH down to PICK level
					{
						if ( m_bComplete_T == FALSE )
						{
							T_Sync();
						}
						Sleep(10);
						DisplaySequence("BH - waiting prebond PR result");
						break;
					}
					if( lPreBondDone==2 )
					{
						Sleep(10);
						DisplaySequence("BH - prebond PR fail, into house keeping q");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					(*m_psmfSRam)["BinTable"]["PreBondDone"] = FALSE;
				}

				// Make sure the T is at Pick before Z down
				if (IsZMotorsEnable() == FALSE)
				{
					//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetMotionCE(TRUE, "BH motion error in MOVE_T_PREPICK_TO_PICK_Q state");	//v4.59A19
					SetErrorMessage("BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					break;
				}

				if (!OpIsBinTableLevelAtDnLevel())				//v4.22T8	//Walsin China
				{
					//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetMotionCE(TRUE, "BT level UP detected in MOVE_T_PREPICK_TO_PICK_Q state");	//v4.59A19
					SetErrorMessage("BH: BT platform at UP level in MOVE_T_PREPICK_TO_PICK_Q !!");
					SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

		//		DisplaySequence("BH - DBH at T prepick to pick, check cover sen");
				CheckCoverOpenInAuto("at BH PrePick state");	//v4.31T9

		//		DisplaySequence("BH - DBH at T prepick to pick, enable thermal control");
				EnableBHTThermalControl(FALSE, TRUE);			//v4.49A5

		//		DisplaySequence("BH - DBH at T prepick to pick, move BH T");
				if (IsBHZ1ToPick())						//If BHZ1 towards PICK	
				{
					TakeTime(T3);
					BA_MoveTo(m_lPickPos_T, SFM_NOWAIT);
				}
				else
				{
					if (bEnableBHUplookPr)	//v4.57A14
						TakeTime(T3);
					else
						TakeTime(T1);			
					BA_MoveTo(m_lBondPos_T, SFM_NOWAIT);
				}

				m_bComplete_T = FALSE;
				m_bMoveFromPrePick = TRUE;		//v4.40T11	//Sanan	//Enable Head-PrePick delay for Sanan MS100+
				m_lTime_T = m_lPrePickTime_T;
				s_dbStartT2PB = GetTime();
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 0;
				(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
				m_dWaitBTStartTime	= GetTime();
	
				//2018.6.1 for trigger to bond arm down
//				SetBTStartMove(TRUE);

				m_qSubOperation		= PICK_Z_DOWN_Q;
				DisplaySequence("BH - T Prepick to Pick ==> Pick Z Down");
			}
			break;


		case WAIT_WT_READY_Q:	// must after die picked
			DisplaySequence("BH - wait WT ready q");
			// Check whether Z is completed
			if (m_bComplete_T == FALSE)
			{
				if (T_IsComplete() == TRUE)
				{
					if (IsBHZ2ToPick())		//If BHZ1 towards BOND
					{
						if (bEnableBHUplookPr)	//v4.57A13
							TakeTime(T4);
						else
							TakeTime(T2);
					}
					else
					{
						TakeTime(T4);
					}
					m_bComplete_T = TRUE;
				}
			}

			bWaferEnd	= IsWaferEnded();
			bRegionEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"];	// xuzhijin_region_end 

			if (m_bStop)
			{
				TakeTime(BD);		//v3.99T1
				SetEjectorReady(FALSE, "3");
				SetDieBondedForWT(FALSE);
				m_dWaitBTStartTime = GetTime();			//v2.93T2
				m_qSubOperation = WAIT_BT_READY_Q;	
				DisplaySequence("BH - Wait WT ready stop ==> Wait BT Ready");

				//**** Normal STOP triggering position when user presses STOP button on menu ****//
				if (IsMS90() && (IsSortTo2ndPart() || m_b2PartsAllDone))
				{
					DisplaySequence("BH - detect wafer end, do 1 more cycle in wait wt ready");
				}
				else
				{
					m_bDBHStop = TRUE;
				}
			}
			else if (WaitWTReady() || bWaferEnd || bRegionEnd || IsOnlyRegionEnd() )
			{	
				TakeTime(BD);
				SetConfirmSrch(FALSE);		
				SetWTReady(FALSE, "SetWTReady FALSE WAIT_WT_READY_Q");
				SetEjectorReady(FALSE, "4");	//	Wait WT ready q, WT moved or wafer/region end
				SetDieBondedForWT(FALSE);
				SetWTStartMove(FALSE);					//v3.89		//Clear it once again as die-ready means WT not yet indexed for sure!
				m_dWaitBTStartTime = GetTime();			//v2.93T2
				m_qSubOperation = WAIT_BT_READY_Q;	
				DisplaySequence("BH - wait WT ready ok ==> wait BT ready");
			}
			else if (WaitBadDieForT())
			{
				SetBadDieForT(FALSE); 
	//	V450X16	SetEjectorReady(FALSE, "5");
				OpBadDie(TRUE);				//v3.93T3
	//	V450X16	SetEjectorReady(TRUE, "5");
				StartTime(GetRecordNumber());	// Restart the time
				OpUpdateDieIndex();				// Update the die index
				m_qSubOperation = WAIT_WT_Q;
				DisplaySequence("BH - Bad die to Wait WT");
				s_bUpdateNVRAM = TRUE;
			}
			else if (WaitConfirmSrch())
			{
				SetConfirmSrch(FALSE);
				//	V450X16	SetEjectorReady(FALSE, "6");	
				OpBadDie(TRUE);					// Treat as bad die case	//BH-T to PRE-PICK
				//	V450X16	SetEjectorReady(TRUE, "6");
				OpUpdateDieIndex();				// Update the die index
				SetBhToPrePick(TRUE);			// Tell WPR that BH is now on PRE-PICK & go for CONFIRM-SEARCH
				m_qSubOperation = WAIT_WT_Q;
				s_bUpdateNVRAM = TRUE;
				DisplaySequence("BH - Confirm search to Wait WT");
			}
			else
			{
/*
				//v4.22T7	//Re-implemented for Cree HuiZhou for "NextBinFull NoPickDie" fcn only
				if ((pAppMod->GetCustomerName() == "Cree") && IsBinFull())		
				{
					SetErrorMessage("BH: BIN_FULL detected at WAIT WT_READY_Q");

					//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();	//Must wait T complete for ILC
					Sleep(100);	
				
					EnableBHTThermalControl(TRUE, TRUE);		//v4.49A5
					//SetBondHeadFan(FALSE);					//v4.54A7
					CheckCoverOpenInAuto("MS100 bin full");
					T_MoveTo(m_lPrePickPos_T);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					m_qSubOperation = WAIT_BIN_FULL_Q;
				}
				else 
*/
				if (m_nBHLastAlarmCode == -2)			//ChangeGrade NoPickDIe	//v4.15T1
				{
					m_qSubOperation = WAIT_BT_READY_Q;	//Go directly to WAIT_BT_READY_Q to change bin!
					DisplaySequence("BH - Wait WT ready alarm code -2 ==> wait BT ready");
				}

				if ( IsBTChngGrd() || IsBinFull() )	// HuaMao green light idle, at pick position, move away.
				{
					CString szMsg;
					szMsg.Format("BH - Wait WT ready (Next change %d change GRADE %d, BinFull %d) ==> wait BT Ready Q",
							OpIsNextDieChangeGrade(), IsBTChngGrd(), IsBinFull());
					DisplaySequence(szMsg);
					m_qSubOperation = WAIT_BT_READY_Q;	//Go directly to WAIT_BT_READY_Q to change bin!
					break;
				}
			}
			break;

		case WAIT_BT_READY_Q:
			DisplaySequence("BH - Wait BT Ready");
			bWaferEnd		= IsWaferEnded();		//v3.99T1

			if (lBTStop)
			{
				if( (IsMS90() && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE) )
				{
					DisplaySequence("BH - |BT stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			if (IsBinFull())
			{
				//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
				T_Sync();	//Must wait T complete for ILC
				Sleep(100);	
			
				EnableBHTThermalControl(TRUE, TRUE);	//v4.49A5
				//SetBondHeadFan(FALSE);			//v4.54A7

				CheckCoverOpenInAuto("MS100 bin full");
				T_MoveTo(m_lPrePickPos_T);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
				m_qSubOperation = WAIT_BIN_FULL_Q;
			}
/*
			else if (WaitBTStartMove(200))		//v3.99T1	//v4.01
			{
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				TakeTime(RD);
				SetBTStartMove(FALSE);
				m_qSubOperation = PICK_Z_DOWN_Q;
			}
*/
//=============================================================================
//  PICK Z down do not wait for WaitBTStartMove event
//=============================================================================
			else if (WaitBTStartMoveWithoutDelay(200))
			{
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				SetBTStartMoveWithoutDelay(FALSE);
				DisplaySequence("SetBTStartMoveWithoutDelay==>FALSE");
				m_qSubOperation = PICK_Z_DOWN_Q;
			}
//=============================================================================

			else if ( IsBTChngGrd() || IsOnlyRegionEnd() )
			{
				//Normal CHANGE-GRADE triggering position in MS100 sequence
			
				//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
				T_Sync();	//Must wait T complete for ILC
				Sleep(100);	

				CheckCoverOpenInAuto("MS100 change grade");
				if( !(IsBTChngGrd() && pUtl->GetPrescanRegionMode()) )
				{
					(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				}
				T_MoveTo(m_lPrePickPos_T);

				szMsg = "BH - Wait BT ready ==> T Prepick to pick ";
				//v4.15T1
				if (CMS896AStn::m_bMS100OriginalBHSequence == FALSE)
				{
					if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
					{
						szMsg += "Andrew: EjtXY ChangeGrade 2";
						SetErrorMessage("Andrew: EjtXY ChangeGrade 2");
						m_bChangeGrade = TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
					}
					else
					{
						CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 2");	//v4.47T10
						szMsg += "BH: Change Grade No Pick Die 2";
						m_bFirstCycle		= TRUE;		
						m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
					}

					//Need to reset NoPickDie warning here
					m_lBHStateCode		= 0;
					m_nBHAlarmCode		= 0;		//v4.15T6	
					m_nBHLastAlarmCode	= 0;		//v4.15T6
				}

				EnableBHTThermalControl(TRUE, TRUE);	//v4.49A5
				//SetBondHeadFan(FALSE);				//v4.54A7
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;

				if( pUtl->GetPrescanRegionMode() )	// xu_semitek
				{
					m_lBondDieCount++;
					SetBTReady(FALSE); //2018.3.28 
					SetDieBonded(TRUE);
					m_qSubOperation =  WAIT_BT_CHANGE_GRADE;
				}

				CMSLogFileUtility::Instance()->BL_LogStatus("BH: change grade to prepick");
				DisplaySequence(szMsg);
				if( IsBTChngGrd() )	// HuaMao green light idle, reset ejector ready state
				{
					SetEjectorReady(FALSE, "8");
				}
				break;
			}
			else if ((bWaferEnd && !IsMS90()) )// || (IsMS90() && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE ) )	//	!m_bMS90UplookPrDo1MoreCycle)			//v3.99T1
			{
				//SetErrorMessage("BH: WaferEnd encountered in WAIT_BT_READY state -> STOP");
				DisplaySequence("BH - |WaferEnd encountered in WAIT_BT_READY state| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if ( bWaferEnd && IsMS90() && m_bBHZ1HasDie==FALSE && m_bBHZ2HasDie==FALSE )	//	!m_bMS90UplookPrDo1MoreCycle)			//v3.99T1
			{
				//SetErrorMessage("BH: WaferEnd encountered in WAIT_BT_READY state -> STOP");
				DisplaySequence("BH - MS90 |WaferEnd encountered in WAIT_BT_READY state| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			else if (WaitBTReSrchDie())		//v4.xx
			{
				SetBTReSrchDie(FALSE);
				SetWTReady(TRUE, "SetWTReady TRUE WaitBTReSrchDie");
				T_Sync();			//Must wait T complete for ILC
				Sleep(20);	

				SetErrorMessage("BH: BPR ReSrchDie triggers BH to PrePICK");
				CheckCoverOpenInAuto("MS100 ReSrchDie");
				T_MoveTo(m_lPrePickPos_T);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = WAIT_WT_Q;
				break;
			}
			else	//v3.99T6
			{
				//Emergency STOP loop to avoid sw hangup here !! //v3.99 version
				if (m_bStop)
				{
					//if (!WaitBTStartMove(1000))
					if (!WaitBTStartMove(200))		//v4.59A31
					{
						SetErrorMessage("BH: abnormal STOP encountered in WAIT_BT_READY state -> STOP");
						DisplaySequence("BH - |BT not start and stop| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
					}
				}
			}
			break;


		case PICK_Z_DOWN_Q:

			if (IsBHZ1ToPick() && m_bAutoLearnPickLevelZ1)
			{
				DisplaySequence("BH - Learn Pick Level Z1 before Z Down");
				if (m_bComplete_T == FALSE)
				{
					T_Sync();
					m_bComplete_T = TRUE;
					TakeTime(T2);
				}
				//if (!OpPrestartColletHoleCheck(TRUE,FALSE))
				//{
				//	m_qSubOperation = HOUSE_KEEPING_Q;	
				//	break;
				//}
				//T_MoveTo(m_lPickPos_T);
				//Sleep(100);
				if (!OpAutoLearnBHZPickLevel(FALSE))
				{
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}
				DisplaySequence("BH - Learn Pick Level Z1 Completed");

			}
			else if (IsBHZ2ToPick() && m_bAutoLearnPickLevelZ2)
			{
				DisplaySequence("BH - Learn Pick Level Z2 ");
				if (m_bComplete_T == FALSE)
				{
					T_Sync();
					m_bComplete_T = TRUE;
					TakeTime(T4);
				}
				//if (!OpPrestartColletHoleCheck(FALSE,FALSE))
				//{
				//	m_qSubOperation = HOUSE_KEEPING_Q;	
				//	break;
				//}
				//T_MoveTo(m_lBondPos_T);
				//Sleep(100);
				if (!OpAutoLearnBHZPickLevel(TRUE))
				{
					m_qSubOperation = HOUSE_KEEPING_Q;	
					break;
				}
				DisplaySequence("BH - Learn Pick Level Z2 Completed");
			}

			if( IsBHZ1ToPick() )	// HuaMao green light idle
				DisplaySequence("BH - BHZ1 pick down q");
			else
				DisplaySequence("BH - BHZ2 pick down q");

			//if (IsMS90() && (IsSortTo2ndPart() || m_b2PartsAllDone))
			//{
			//	DisplaySequence("BH - detect wafer end, do i more cycle2");
			//	m_bMS90UplookPrDo1MoreCycle = TRUE;
			//}

			m_lSPCResult = (*m_psmfSRam)["BondPr"]["PostBondResult"];
			if (m_bDisableBT)
			{
				m_lSPCResult = 1;
			}
			if (m_lSPCResult != 1)
			{
				if (!CheckBPRErrorNeedAlarm(0))
				{
					m_lSPCResult = 1;
					(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
				}
			}

			//if ((lSPCResult != 1) && (pAppMod->GetCustomerName().Find(CTM_SANAN) != -1))
			if (m_lSPCResult != 1)
			{
				OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "1");	//v4.52A7	//Sanan & Semitek
			}

			if (m_lSPCResult != 1)		//v3.82
			{
				m_lPrevDieSPCResult = m_lSPCResult;
				OpSetMS100AlarmCode(6);			//BPR PostBond-fail
			}

			s_bDownToPick = FALSE;
			if (AllowMove() == TRUE)
			{
				//Check machine pressure
				if (IsLowPressure() == TRUE)
				{
					SetStatusMessage("Machine low pressure");
					SetErrorMessage("Machine low pressure");
					OpSetMS100AlarmCode(1);			//LowPressure
				}

				//Check machine pressure
				if (IsLowVacuumFlow())
				{
					SetStatusMessage("Machine low vacuum flow in auto");
					SetErrorMessage("Machine low vacuum flow in auto");
					OpSetMS100AlarmCode(16);			//LowPressure
				}

				// Check whether it is suitable time for Collet Jam check and Move Z
				LONG	lPickDelay = 0, lBondDelay = 0;
				LONG lTMvTm = (LONG)(GetTime() - s_dbStartT2PB);
				if (IsBHZ1ToPick())		//If BHZ1
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )		//v4.40T11
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z + m_lHeadPrePickDelay;
					}
					else
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z/* - 1*/;	//v4.43T7
					}
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )	//v4.49A9
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z2 + m_lHeadPrePickDelay;	
					else
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z2/* - 1*/;		//v4.43T7
				}	
				else
				{
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )		//v4.40T11
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z2 + m_lHeadPrePickDelay;
					}
					else
					{
						lPickDelay = m_lTime_T - lTMvTm + lHeadPickDelay - m_lPickTime_Z2/* - 1*/;	//v4.43T7
					}
					if ( m_bMoveFromPrePick && (m_lHeadPrePickDelay > 0) )	//v4.49A9
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z + m_lHeadPrePickDelay;	
					else
						lBondDelay = m_lTime_T - lTMvTm + lHeadBondDelay - m_lBondTime_Z/* - 1*/;		//v4.43T7
				}

				//m_bMoveFromPrePick = FALSE;		//v4.49A9

				//v3.86
				//****************************************//
				if (lPickDelay <= lBondDelay)
				{
					m_bPickDownFirst = TRUE;
					m_nBHZ1BHZ2Delay = labs(lBondDelay - lPickDelay); // - 2;	//v4.47T12
				}
				else
				{
					m_bPickDownFirst = FALSE;
					m_nBHZ1BHZ2Delay = labs(lPickDelay - lBondDelay); // - 2;	//v4.47T12
				}
				//****************************************//

				if (!WaitBTStartMoveWithoutDelay(0) && !m_bFirstCycle && !m_bPickDownFirst)
				{
					//====================================================================================
					//  Wait for BT table move event
					//====================================================================================
					if  (WaitBTStartMove(200))
					{
						TakeTime(RD);
						DisplaySequence("SetBTStartMove1==>FALSE");
						SetBTStartMove(FALSE);
					}
					else
					{
						break;
					}
				}
				//====================================================================================
/*
				//Sync the pick&bond delay
				LONG lEjectorUpTime = max(lTime_Ej, lEjectorUpDelay);
				LONG lToPickDelayProcess = lPickDelay + lEjectorUpTime;
				LONG lToBondDelayProcess = 0;
				CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
				if (pBinTable != NULL)
				{
					lToBondDelayProcess = pBinTable->GetBTRemainMoveDelay();
				}
				
				if (lToBondDelayProcess > lToPickDelayProcess + 1)
				{
					Sleep(lToBondDelayProcess - lToPickDelayProcess - 1);
				}
*/

				//4.52D17Auto rpy2 before bonding (steal time)	(PICK_Z_DOWN_Q)
				if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
				{
					s_dbStartZ2PK = GetTime();	

					BOOL b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				
					m_lUpLookPrStage = 0;	//reset

					if(!b2DStatus)
					{
						SetErrorMessage("BHZ *Uplook PR Search Rpy2 Failure to machine stop");
						m_bUplookResultFail = TRUE;		//v4.57A11
						//m_qSubOperation = HOUSE_KEEPING_Q;
						//break;
					}

					lTemp = (LONG)(GetTime() - s_dbStartZ2PK);
					//m_nBHZ1BHZ2Delay = m_nBHZ1BHZ2Delay - lTemp;

					if (m_bPickDownFirst)
					{
						lPickDelay = lPickDelay - lTemp;
					}
					else
					{
						lBondDelay = lBondDelay - lTemp;
					}
				}
				//MegaDa No BHT
				m_bComplete_T = TRUE;
				if (m_bComplete_T == FALSE)
				{
					if (m_bPickDownFirst)
					{
						if (lPickDelay - 1 > 0)
						{
							Sleep(lPickDelay - 1);
						}
					}
					else
					{
						if (lBondDelay- 1 > 0)
						{
							Sleep(lBondDelay - 1);
						}
					}

					if ( (lDelay > 0) && !IsES101() )	
					{
						Sleep(lDelay);
					}
				}

				// check machine cover state
				if (IsCoverOpen() == TRUE)
				{
					SetStatusMessage("Machine Cover Open");
					SetErrorMessage("Machine Cover Open");
					OpSetMS100AlarmCode(2);			//CoverOpen
				}
				
				//20171025 Leo Protection of hit hand
				CheckCoverOpenInAuto("During Bonding Cycle");
				//v4.59A19	//Not used anymore
				//Check Thermal Sensor if present	//4.26T1
				/*if (OpIsThermalAlarmTriggered() == TRUE)
				{
					SetStatusMessage("Machine DBH Thermal sensor is triggered");
					SetErrorMessage("Machine DBH Thermal sensor is triggered");
					OpSetMS100AlarmCode(7);			//Thermal Alarm
				}*/


				if (!OpCheckValidAccessMode(TRUE))		//v4.34T4	//SanAn
				{
					SetErrorMessage("OpCheckValidAccessMode fails");	//v4.53A23
					m_bDBHStop = TRUE;
				}

				if(!OpCheckStartLotCount())
				{
					//SetStatusMessage("OpCheckStartLotCount fails");
					SetErrorMessage("OpCheckStartLotCount fails");		//v4.53A23
					m_bDBHStop = TRUE;
				}

				if ( IsMS60() == TRUE )		//Check BH Fan sensor
				{
					if ( m_lCheckFanCount >= 50 )	// 0.06x50 = 3sec
					{
						if (OpCheckMS60BHFanOK() != TRUE)	//v4.48A1
						{
							OpSetMS100AlarmCode(8);	
						}
						m_lCheckFanCount = 0;
					}
					else
					{
						m_lCheckFanCount++;
					}
				}


				if (m_bPickDownFirst)	//	pick side down firstly
				{
					TakeTime(CJ);		// Take Time

					BOOL bIsColletJam = TRUE;
					BOOL bReachedColletJamCount = FALSE;
					if (!CheckColletJamCount(bIsColletJam, bReachedColletJamCount))
					{
						SetErrorMessage("OpMS100AutoCleanCollet_MDCJ fails");
						m_bDBHStop = TRUE;
					}
					if (bIsColletJam && (bReachedColletJamCount || (OpCheckColletJam() == TRUE)))	//v2.58
					{
						LogColletJamThresholdValue(IsBHZ1ToPick());
						//Disable NuMotion data log earlier here	//LeoLam	//v4.50A24
						CycleEnableDataLog(FALSE, TRUE, TRUE);

						SetErrorMessage("6# Collet is Jammed");
						(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
					
						if (IsBHZ1ToPick())		//If BHZ1	
						{
							m_lCJTotalCount1++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ1);
						}
						else
						{
							m_lCJTotalCount2++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"] = m_lCJTotalCount2;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ2);
						}

						CheckCoverOpenInAuto("CJ1 alarm code 3");
						OpSetMS100AlarmCode(3);		//CJ
					}
					else
					{
						RuntimeUpdateColletJamThresholdValue();
						//v4.52A11	//XM SanAn
						if (!m_bDetectDAirFlowAtPrePick && 
							!OpUpdateDAirFlowThresholdValue(IsBHZ2ToPick()))
						{
							//OpSetMS100AlarmCode(11);	// 11: new alarm, not yet implemented !!!
							//		or
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}
						//v4.15T1	//CMLT
						if (OpIsNextDieChangeGrade())
						{
							OpSetMS100AlarmCode(-2);	//ChangeGrade NoPickDie
						}
						else if (OpIsNextDieBinFull())	//v4.21T2
						{
							OpSetMS100AlarmCode(-3);	//NextBinFull NoPickDie
						}

						BOOL bPickDie = TRUE;
						LONG eAction = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"];
						if( (WAF_CDieSelectionAlgorithm::WAF_EDieAction)eAction!=WAF_CDieSelectionAlgorithm::PICK )
						{
							bPickDie = FALSE;
						}
						LONG lNextPickRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
						LONG lNextPickCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
						lLastPickRow	= m_ulAtPickDieRow;
						lLastPickCol	= m_ulAtPickDieCol;
						if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop)
						{
							s_bDownToPick = TRUE;
							if( (lLastPickRow==lNextPickRow) && (lLastPickCol==lNextPickCol) )
							{
								s_bDownToPick = FALSE;
							}
							if( bPickDie==FALSE )
							{
								s_bDownToPick = FALSE;
							}
						}
						
						szMsg.Format("BH - PICKDOWN %d 1ST pick(%ld,%ld),bond(%ld,%ld); WPR %d BHZ%d",
							s_bDownToPick, lNextPickRow, lNextPickCol, lLastPickRow, lLastPickCol, bPickDie, IsBHZ1ToPick());
						DisplaySequence(szMsg);
						
						if (s_bDownToPick)	
						{
							CString szMsg = "";
							LONG lCompen_Z = OpCalCompen_Z(IsBHZ2ToPick());		//v4.53A25
							
							if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
							{
								TakeTime(Z3);
								if( IsBHZOnVacuumOnLayer() )
								{
									SetPickVacuum(FALSE);
								}
								if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
								{
									AC_Z1P_MoveToOrSearch(m_lPickLevel_Z + lCompen_Z, SFM_NOWAIT);
								}
								
								m_bComplete_Z = FALSE;
								szMsg.Format("BH - BHZ1 to pick %ld (COMP=%ld) in PICK_Z_DOWN_Q", m_lPickLevel_Z, lCompen_Z);
							}
							else
							{
								TakeTime(Z7);
								if( IsBHZOnVacuumOnLayer() )
								{
									SetPickVacuum(FALSE);
								}
								if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
								{
									AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2 + lCompen_Z, SFM_NOWAIT);
								}
								
								m_bComplete_Z2 = FALSE;
								szMsg.Format("BH - BHZ2 to pick %ld (COMP=%ld) in PICK_Z_DOWN_Q", m_lPickLevel_Z2, lCompen_Z);
							}

							m_dBHZPickStartTime = GetTime();	//shiraishi03
							DisplaySequence(szMsg);
							CMSLogFileUtility::Instance()->BPR_Arm1Log(szMsg);
						}
					}
					s_dbStartZ2PK = GetTime();	//	pick down first
				}
				else
				{
					TakeTime(MD);	
					lLastPickRow	= m_ulAtPickDieRow;
					lLastPickCol	= m_ulAtPickDieCol;
					if (m_bFirstCycle || m_bChangeGrade)
					{
						m_lMD_Count		= GetMissingDieRetryCount();
						m_lMD_Count2	= GetMissingDieRetryCount();
						szMsg.Format("BH - Bond Down first firstcycle=%d, changegrade=%d", m_bFirstCycle, m_bChangeGrade);
						DisplaySequence(szMsg);
					}
					else if (m_bUplookResultFail	|| 		//v4.57A11
							 OpCheckMissingDie())
					{
						TakeTime(MDO);
						LONG lMissingDieStop = 0;

						if (!m_bUplookResultFail)
						{
							m_ulMissingDieCount++;
							WriteUnpickInfo(0, lLastPickRow, lLastPickCol);
							if (m_pulStorage != NULL)
							{
								*(m_pulStorage + BH_MISSINGDIE_OFFSET) = m_ulMissingDieCount;
								*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = m_ulCurrWaferMissingDieCount;
							}

							OpIncSKOffsetForBHZMD(IsBHZ1ToPick());

							// If retry is 0, simply stop
							if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
							{
								m_lMD_Count--;//for retry
								m_lMDCycleCount1++;
								m_lMDTotalCount1++;
								m_lNOMDCount1 = 0;
								(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"] = m_lMDTotalCount1; //4.54T15

								if (GetMDCycleLimit() > 0 && (m_lMDCycleCount1 >= GetMDCycleLimit()))
								{
									lMissingDieStop = 3;
								}

								if (m_lMD_Count <= 0)
								{
									lMissingDieStop = 1;
								}
							}
							else
							{
								m_lMD_Count2--;//for retry
								m_lMDCycleCount2++;
								m_lMDTotalCount2++;
								m_lNOMDCount2 = 0;
								(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"] = m_lMDTotalCount2; //4.54T15

								if (GetMDCycleLimit() > 0 && (m_lMDCycleCount2 >= GetMDCycleLimit()))
								{
									lMissingDieStop = 4;
								}

								if (m_lMD_Count2 <= 0)
								{
									lMissingDieStop = 2;
								}
							}
						}
						//v4.55		//v4.57A11
						//Uplook PR fail count as 1 MD count
						//if (m_bUplookResultFail)
						else
						{
							m_bUplookResultFail = FALSE;
							bUpLookFailAtMD = TRUE;

							//v4.59A5
							if (IsBHZ1ToPick())
							{
								if (m_lBHUplookPrFailLimit > 0)
								{
									m_ulBH2UplookPrFailCount++;
									if ((long)m_ulBH2UplookPrFailCount >= m_lBHUplookPrFailLimit) 
									{
										lMissingDieStop = 6;	//BHZ2 Uplook PR failure
										m_bUplookResultFail = TRUE;
									}
								}
							}
							else
							{
								if (m_lBHUplookPrFailLimit > 0)
								{
									m_ulBH1UplookPrFailCount++;
									if ((long)m_ulBH1UplookPrFailCount >= m_lBHUplookPrFailLimit) 
									{
										lMissingDieStop = 5;	//BHZ1 Uplook PR failure
										m_bUplookResultFail = TRUE;
									}
								}
							}
						}

						LogMissingDieThresholdValue(IsBHZ2ToPick());

						if (lMissingDieStop > 0)
						{
							BOOL bMissingDieStop = ReachMissingDieCountLimit();
							BOOL bReturn = TRUE;
							if (!bMissingDieStop)
							{
								bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, TRUE);
							}
	
							if (!bReturn || bMissingDieStop)
							{
								if (!bReturn)
								{
									SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z_DOWN_Q state");	//v4.59A19
									SetErrorMessage("BondHead module is off power");
									SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
								}
								else
								{
									OpMissingDieUplookPrFailToStop(lMissingDieStop);	//v4.59A5
								}

								if (lMissingDieStop == 5 || lMissingDieStop == 6)
								{
									WriteUnpickInfo(8 ,lLastPickRow, lLastPickCol);
								}

								if( bUpLookFailAtMD )
									(*m_psmfSRam)["BondHead"]["MissingDie"] = 2;
								else
									(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
								m_qSubOperation = HOUSE_KEEPING_Q;	
								DisplaySequence("BH - |missing die| ==> house keeping");
								break;
							}
							// reset retry-count
							if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
							{
								m_lMD_Count		= GetMissingDieRetryCount();
								m_lMDCycleCount1 = 0;
							}
							else
							{
								m_lMD_Count2	= GetMissingDieRetryCount();
								m_lMDCycleCount2 = 0;
							}
							//Reset for BT table
							DisplaySequence("BH - PICK_Z_DOWN_Q, Missing die up look");
							OpSetMS100AlarmCode(-1);	//MD-Retry warning
						}
						else
						{
							BOOL bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, FALSE);		//v4.59A5

							if (!bReturn)
							{
								SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z_DOWN_Q state");	//v4.59A19
								SetErrorMessage("BondHead module is off power");
								SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
								m_qSubOperation = HOUSE_KEEPING_Q;
								WriteUnpickInfo(IDS_BH_MODULE_NO_POWER,lLastPickRow,lLastPickCol);
								break;
							}
							else
							{
								//Reset for BT table
								DisplaySequence("BH - PICK_Z_DOWN_Q, Missing die up look");
								OpSetMS100AlarmCode(-1);	//MD-Retry warning
							}
						}
					}
					else
					{
						//RuntimeUpdateMissingDieThresholdValue();
						if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						{
							m_lNOMDCount2++;
							m_lMD_Count2	= GetMissingDieRetryCount();    // Reset the local count
							if (m_lNOMDCount2 > 200)
							{
								DisplaySequence("BH - NO MD Count2 > 200 A");
								RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
							}
						}	
						else
						{
							m_lNOMDCount1++;
							m_lMD_Count		= GetMissingDieRetryCount();    // Reset the local count
							if (m_lNOMDCount1 > 200)
							{
								DisplaySequence("BH - NO MD Count2 > 200 B");
								RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
							}
						}

						//v4.35T2
						lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];
						if (lMotionTime_BT > 150)
						{
							Sleep(m_lBinTableDelay);
						}

						if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						{
							TakeTime(Z7);

							LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);		//v4.48A8

							AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
							//AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT);

							m_bComplete_Z2 = FALSE;
							szMsg.Format("BH - BHZ2 to bond %d false PICK_Z_DOWN_Q", lBondZ2);
						}
						else
						{
							TakeTime(Z3);

							LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);	//v4.48A8

							AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
							//AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT);

							m_bComplete_Z = FALSE;
							szMsg.Format("BH - BHZ1 to bond %d false in PICK_Z_DOWN_Q", lBondZ1);
						}
						s_dBondZDownStart = GetTime();
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						DisplaySequence(szMsg);
						CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);
					}
				}
				m_qSubOperation = BOND_Z_WAIT_BT_READY_Q; //PICK_Z2_DOWN_Q;	// -> normal flow
			}
			break;

		case BOND_Z_WAIT_BT_READY_Q:
			if (!m_bFirstCycle && m_bPickDownFirst && (m_nBHZ1BHZ2Delay > 0))
			{
				//waiting the remain headbond delay and allow Z to move bond position
				Sleep(m_nBHZ1BHZ2Delay);
				m_nBHZ1BHZ2Delay = 0;
			}
			
			if (!WaitBTStartMoveWithoutDelay(0) && !m_bFirstCycle && m_bPickDownFirst)
			{
				//====================================================================================
				//  Wait for BT table move event
				//====================================================================================
				if  (WaitBTStartMove(200))
				{
					m_nBHZ1BHZ2Delay = 0;
					TakeTime(RD);
					DisplaySequence("SetBTStartMove2==>FALSE");
					SetBTStartMove(FALSE);
					m_qSubOperation = PICK_Z2_DOWN_Q;	// -> normal flow
				}
				break;
				//====================================================================================
			 }
			 //go to PICK_Z2_DOWN_Q
		case PICK_Z2_DOWN_Q:
			if( IsBHZ1ToPick() )	// HuaMao green light idle
				DisplaySequence("BH - PICK_Z2_DOWN_Q pick z1 down q");
			else
				DisplaySequence("BH - PICK_Z2_DOWN_Q pick z2 down q");

			if (AllowMove() == TRUE)
			{
				if (!m_bFirstCycle)	//ashiraishi03
				{
					if (m_nBHZ1BHZ2Delay > 0)	//v4.47T7
					{
						Sleep(m_nBHZ1BHZ2Delay);
					}
				}

				if (!m_bPickDownFirst)
				{
					if (IsBHZ1ToPick())		//If BHZ1
					{
						if (m_lPickTime_Z <= lHeadPickDelay && (m_bComplete_T == FALSE))
						{
							T_Sync();
							m_bComplete_T = TRUE;	//v4.43T1
							TakeTime(T4);
						}
					}
					else
					{
						if (m_lPickTime_Z2 <= lHeadPickDelay && (m_bComplete_T == FALSE))
						{
							T_Sync();
							m_bComplete_T = TRUE;	//v4.43T1
							
							if (bEnableBHUplookPr)	//v4.57A13
								TakeTime(T4);
							else
								TakeTime(T2);
						}
					}

					TakeTime(CJ);		// Take Time

					BOOL bIsColletJam = TRUE;
					BOOL bReachedColletJamCount = FALSE;
					if (!CheckColletJamCount(bIsColletJam, bReachedColletJamCount))
					{
						SetErrorMessage("OpMS100AutoCleanCollet_MDCJ fails");
						m_bDBHStop = TRUE;
					}

					if (bIsColletJam && (bReachedColletJamCount || (OpCheckColletJam() == TRUE)))	//v2.58
					{
						LogColletJamThresholdValue(IsBHZ1ToPick());

						//Disable NuMotion data log earlier here	//LeoLam	//v4.50A24
						CycleEnableDataLog(FALSE, TRUE, TRUE);

						SetErrorMessage("7# Collet is Jammed");
						(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
						
						if (IsBHZ1ToPick())		//If BHZ1	
						{
							m_lCJTotalCount1++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ1"] = m_lCJTotalCount1;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ1);
						}
						else
						{
							m_lCJTotalCount2++;
							(*m_psmfSRam)["BondHead"]["ColletJamBHZ2"] = m_lCJTotalCount2;
							SetAlert_Red_Yellow(IDS_BH_COLLET_JAM_BHZ2);
						}

						CheckCoverOpenInAuto("CJ2 alarm code 3");

						OpSetMS100AlarmCode(3);		//CJ
					}
					else
					{
						RuntimeUpdateColletJamThresholdValue();
						//v4.52A11	//XM SanAn
						if (!m_bDetectDAirFlowAtPrePick && 
							!OpUpdateDAirFlowThresholdValue(IsBHZ2ToPick()))
						{
							//OpSetMS100AlarmCode(11);	// 11: new alarm, not yet implemented !!!
							//		or
							m_qSubOperation = HOUSE_KEEPING_Q;	
							break;
						}

						//v4.15T1	//CMLT
						if (OpIsNextDieChangeGrade())
						{
							OpSetMS100AlarmCode(-2);	//ChangeGrade NoPickDie
						}
						else if (OpIsNextDieBinFull())	//v4.21T2
						{
							OpSetMS100AlarmCode(-3);	//NextBinFull NoPickDie
						}

						BOOL bPickDie = TRUE;
						LONG eAction = (LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"];
						if( (WAF_CDieSelectionAlgorithm::WAF_EDieAction)eAction!=WAF_CDieSelectionAlgorithm::PICK )
						{
							bPickDie = FALSE;
						}

						LONG lNextPickRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
						LONG lNextPickCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
						lLastPickRow	= m_ulAtPickDieRow;
						lLastPickCol	= m_ulAtPickDieCol;
						if (OpCheckAlarmCodeOKToPick() && !m_bDBHStop)		//v4.15T1
						{
							s_bDownToPick = TRUE;
							if( (lLastPickRow==lNextPickRow) && (lLastPickCol==lNextPickCol) )
							{
								s_bDownToPick = FALSE;
							}
							if( bPickDie==FALSE )
							{
								s_bDownToPick = FALSE;
							}
						}
						szMsg.Format("BH - PICKDOWN %d 2ND pick(%ld,%ld),bond(%ld,%ld); WPR %d BHZ%d",
							s_bDownToPick, lNextPickRow, lNextPickCol, lLastPickRow, lLastPickCol, bPickDie, IsBHZ1ToPick());
						DisplaySequence(szMsg);
						if( s_bDownToPick )
						{
							CString szMsg;
							if (IsBHZ1ToPick())	//If BHZ1	
							{
								TakeTime(Z3);
								if( IsBHZOnVacuumOnLayer() )
								{
									SetPickVacuum(FALSE);
								}
								if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
								{	
									AC_Z1P_MoveToOrSearch(m_lPickLevel_Z, SFM_NOWAIT);
								}
								m_bComplete_Z = FALSE;
								szMsg.Format("BH - BHZ1 to pick %d in PICK_Z2_DOWN_Q", m_lPickLevel_Z);
							}
							else
							{
								TakeTime(Z7);
								//v4.47T9
								//
								if( IsBHZOnVacuumOnLayer() )
								{
									SetPickVacuum(FALSE);
								}
								if( !IsLayerPicking() || (m_lTCUpHeatingTime<0) )
								{
									AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2, SFM_NOWAIT);
								}
								//
								m_bComplete_Z2 = FALSE;
								szMsg.Format("BH - BHZ2 to pick %d in PICK_Z2_DOWN_Q", m_lPickLevel_Z2);
							}

							m_dBHZPickStartTime = GetTime();	//shiraishi03

							CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
							DisplaySequence(szMsg);
							CMSLogFileUtility::Instance()->BPR_Arm1Log(szMsg);
						}

						s_dbStartZ2PK = GetTime();	//	pick down secondly
					}

					//m_qSubOperation = EJ_UP_Q;
					m_qSubOperation = SEARCH_MARK_Q;
					DisplaySequence("BH - Pick Z2 Down bd1st ==> EJ UP");
				}
				else
				{
					if (IsBHZ1ToPick())		//If BHZ1
					{
						if( (m_lBondTime_Z2 <= lHeadBondDelay) && (m_bComplete_T == FALSE))
						{
							T_Sync();
							m_bComplete_T = TRUE;	//v4.43T1
							TakeTime(T4);
						}
					}
					else
					{
						if( (m_lBondTime_Z <= lHeadBondDelay) && (m_bComplete_T == FALSE))
						{
							T_Sync();
							m_bComplete_T = TRUE;	//v4.43T1

							if (bEnableBHUplookPr)	//v4.57A13
								TakeTime(T4);
							else
								TakeTime(T2);
						}
					}

					TakeTime(MD);	

					if (m_bFirstCycle || m_bChangeGrade)
					{
						//m_qSubOperation = EJ_UP_Q;				// <- Normal flow 
						m_lMD_Count		= GetMissingDieRetryCount();
						m_lMD_Count2	= GetMissingDieRetryCount();
						m_qSubOperation = SEARCH_MARK_Q;
						szMsg.Format("BH - Pick_Z2_Down pick firstcycle=%d, changegrade=%d ==> EJ UP", m_bFirstCycle, m_bChangeGrade);
						DisplaySequence(szMsg);
					}
					else if (m_bUplookResultFail ||	(OpCheckMissingDie() == TRUE))
					{
						TakeTime(MDO);
						LONG lMissingDieStop = 0;

						lLastPickRow	= m_ulAtPickDieRow;
						lLastPickCol	= m_ulAtPickDieCol;
						if (!m_bUplookResultFail)	//v4.59A7
						{
							IncreaseMissingDieCounter();
							WriteUnpickInfo(0, lLastPickRow, lLastPickCol);
							if (m_pulStorage != NULL)
							{
								*(m_pulStorage + BH_MISSINGDIE_OFFSET) = m_ulMissingDieCount;
								*(m_pulStorage + BH_CURWAF_MISSINGDIE_OFFSET) = m_ulCurrWaferMissingDieCount;
							}

							//v4.53A25	//Semitek
							OpIncSKOffsetForBHZMD(IsBHZ1ToPick());

							// If retry is 0, simply stop
							if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
							{
								m_lMD_Count--;//for retry
								m_lMDCycleCount1++;
								m_lMDTotalCount1++;
								m_lNOMDCount1 = 0;
								(*m_psmfSRam)["BondHead"]["MissingDieBHZ1"] = m_lMDTotalCount1; //4.54T15
								if( GetMDCycleLimit() > 0 && (m_lMDCycleCount1 >= GetMDCycleLimit()) )
								{
									lMissingDieStop = 3;
								}
								if (m_lMD_Count <= 0)
								{
									lMissingDieStop = 1;
								}
							}
							else
							{
								m_lMD_Count2--;//for retry
								m_lMDCycleCount2++;
								m_lMDTotalCount2++;
								m_lNOMDCount2 = 0;
								(*m_psmfSRam)["BondHead"]["MissingDieBHZ2"] = m_lMDTotalCount2; //4.54T15

								if( GetMDCycleLimit() > 0 && (m_lMDCycleCount2 >= GetMDCycleLimit()) )
								{
									lMissingDieStop = 4;
								}
								if (m_lMD_Count2 <= 0)
								{
									lMissingDieStop = 2;
								}
							}
						}
						//v4.55		//v4.57A11
						//Uplook PR fail count as 1 MD count
						//if (m_bUplookResultFail)
						else
						{
							m_bUplookResultFail = FALSE;
							bUpLookFailAtMD = TRUE;

							//v4.59A5
							if (IsBHZ1ToPick())
							{
								m_ulBH2UplookPrFailCount++;
								if ((long)m_ulBH2UplookPrFailCount >= m_lBHUplookPrFailLimit) 
								{
									m_bUplookResultFail = TRUE;
									lMissingDieStop = 6;	//BHZ2 Uplook PR failure
								}
							}
							else
							{
								m_ulBH1UplookPrFailCount++;
								if ((long)m_ulBH1UplookPrFailCount >= m_lBHUplookPrFailLimit) 
								{
									m_bUplookResultFail = TRUE;
									lMissingDieStop = 5;	//BHZ1 Uplook PR failure
								}
							}
						}

						LogMissingDieThresholdValue(IsBHZ2ToPick());

						if (lMissingDieStop > 0)
						{
							BOOL bMissingDieStop = ReachMissingDieCountLimit();
							BOOL bReturn = TRUE;
							if (!bMissingDieStop)
							{
								bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, TRUE);
							}
	
							if (!bReturn || bMissingDieStop)
							{
								if (!bReturn)
								{
									SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z_DOWN_Q state");	//v4.59A19
									SetErrorMessage("BondHead module is off power");
									SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
								}
								else
								{
									OpMissingDieUplookPrFailToStop(lMissingDieStop);	//v4.59A5
								}

								if (lMissingDieStop == 5 || lMissingDieStop == 6 )
								{
									WriteUnpickInfo(8 ,lLastPickRow, lLastPickCol);
								}

								if( bUpLookFailAtMD )
									(*m_psmfSRam)["BondHead"]["MissingDie"] = 2;
								else
									(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
								DisplaySequence("BH - |missing| ==> house keeping");
								m_qSubOperation = HOUSE_KEEPING_Q;	
								break;		//v3.86T3
							}
							// reset retry-count
							if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
							{
								m_lMD_Count		= GetMissingDieRetryCount();
								m_lMDCycleCount1 = 0;
							}
							else
							{
								m_lMD_Count2	= GetMissingDieRetryCount();
								m_lMDCycleCount2 = 0;
							}
							//Reset for BT table
							//m_qSubOperation = EJ_UP_Q;				// <- Normal flow	//v4.08
							m_qSubOperation = SEARCH_MARK_Q;
							DisplaySequence(szMsg + " slow ==> EJ UP");
							OpSetMS100AlarmCode(-1);		//MD-Retry warning	
							DisplaySequence("BH - PICK_Z2_DOWN_Q, Missing die up look ==> EJ UP");
						}
						else
						{
							BOOL bReturn = OpMissingDieUplookPrFailHandling(bUpLookFailAtMD, FALSE);		//v4.59A5
							
							if (!bReturn)
							{
								//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
								SetMotionCE(TRUE, "OpMissingDieUplookPrFailHandling fail in PICK_Z2_DOWN_Q state");	//v4.59A19
								
								SetErrorMessage("BondHead module is off power");
								SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
								m_qSubOperation = HOUSE_KEEPING_Q;
								WriteUnpickInfo(IDS_BH_MODULE_NO_POWER,lLastPickRow,lLastPickCol);
								break;
							}
							else
							{
								//Reset for BT table
								//m_qSubOperation = EJ_UP_Q;				// <- Normal flow	//v4.08
								m_qSubOperation = SEARCH_MARK_Q;
								DisplaySequence(szMsg + " slow ==> EJ UP");
								OpSetMS100AlarmCode(-1);		//MD-Retry warning	
								DisplaySequence("BH - PICK_Z2_DOWN_Q, Missing die up look ==> EJ UP");
							}
						}
					}
					else
					{
												//RuntimeUpdateMissingDieThresholdValue();
						if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
						{
							m_lNOMDCount2++;
							m_lMD_Count2	= GetMissingDieRetryCount();    // Reset the local count
							if (m_lNOMDCount2 > 200)
							{
								DisplaySequence("BH - NO MD Count2 > 200 C");
								RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
							}
						}	
						else
						{
							m_lNOMDCount1++;
							m_lMD_Count		= GetMissingDieRetryCount();    // Reset the local count
							if (m_lNOMDCount1 > 200)
							{
								DisplaySequence("BH - NO MD Count1 > 200 D");
								RuntimeUpdateMissingDieThresholdValue(IsBHZ1ToPick());
							}
						}	

						//v4.35T2
						lMotionTime_BT = (*m_psmfSRam)["BinTable"]["MotionTime"];
						if (!(IsEjtUpAndPreheat()) && (lMotionTime_BT > 150))	//v4.47T8
						{
							Sleep(m_lBinTableDelay);
						}

						if (IsBHZ1ToPick())		//If BHZ1
						{
							TakeTime(Z7);

							LONG lBondZ2 = GetZ2BondLevel(TRUE, TRUE);	//v4.48A8
							AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
							//AC_Z2B_MoveToOrSearch(lBondZ2, SFM_NOWAIT);
							m_bComplete_Z2 = FALSE;
							szMsg.Format("BH - BHZ2 to bond %d in PICK_Z2_DOWN_Q", lBondZ2);
						}
						else
						{
							TakeTime(Z3);
							
							LONG lBondZ1 = GetZ1BondLevel(TRUE, TRUE);	//v4.48A8
							AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT, COLLET_VAC_OFF_PROCESS_BLK);
							//AC_Z1B_MoveToOrSearch(lBondZ1, SFM_NOWAIT);
							m_bComplete_Z = FALSE;
							szMsg.Format("BH - BHZ1 to bond %d in PICK_Z2_DOWN_Q", lBondZ1);
						}
						DisplaySequence(szMsg + " ==> EJ UP");
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);
						s_dBondZDownStart = GetTime();
						m_qSubOperation = EJ_UP_Q;			//v4.08	
						//m_qSubOperation = SEARCH_MARK_Q;
					}
				}
			}
			break;

		case SEARCH_MARK_Q:
			if (IsEnableBHMark()/* || IsEnableColletHole()*/)
			{
				//DisplaySequence("Search Mark Q");
				if (IsBHZ1ToPick())		//If BHZ1 at PICK		
				{
					m_lBH1MarkCount++;
					if (m_lBH1MarkCount > m_lCheckBHMarkCycle || m_lBH1MarkCount == 0 /*|| m_bSearchColletNow1*/)
					{
						szMsg.Format("BH - BH1 Mark, Count,%d,Cycle,%d",m_lBH1MarkCount,m_lCheckBHMarkCycle);
						DisplaySequence(szMsg);
						if (m_bComplete_T == FALSE)
						{
							T_Sync();
							m_bComplete_T = TRUE;		//v4.43T1

							TakeTime(T4);
						}

						if (m_bComplete_Z == FALSE)
						{	
							Z_Sync();
							m_bComplete_Z = TRUE;
							TakeTime(Z4);
						}

						SetSearchMarkReady(FALSE);
						SetSearchMarkDone(FALSE);
						m_lBH1MarkCount = 0;
						Sleep(m_lCheckBHMarkDelay);
						GetEncoderValue();
						m_lColletHoleEnc_T = m_lEnc_T;
						m_lColletHoleEnc_Z = m_lEnc_Z;

						SetBhTReadyForWPR(TRUE);
						(*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"] = FALSE;
					}
				}
				else
				{
					m_lBH2MarkCount++;
					if (m_lBH2MarkCount > m_lCheckBHMarkCycle ||m_lBH2MarkCount == 0/* || m_bSearchColletNow2*/)
					{
						szMsg.Format("BH - BH2 Mark, Count,%d,Cycle,%d",m_lBH2MarkCount,m_lCheckBHMarkCycle);
						DisplaySequence(szMsg);
						if (m_bComplete_T == FALSE)
						{
							T_Sync();
							m_bComplete_T = TRUE;		//v4.43T1

							TakeTime(T2);
						}

						if (m_bComplete_Z2 == FALSE)
						{
							Z2_Sync();
							m_bComplete_Z2 = TRUE;
							TakeTime(Z8);
						}

						SetSearchMarkReady(FALSE);
						SetSearchMarkDone(FALSE);
						m_lBH2MarkCount = 0;
						Sleep(m_lCheckBHMarkDelay);
						GetEncoderValue();
						m_lColletHoleEnc_T = m_lEnc_T;
						m_lColletHoleEnc_Z = m_lEnc_Z2;

						SetBhTReadyForWPR(TRUE);
						(*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"] = TRUE;
					}
				}
			}
			m_qSubOperation = EJ_UP_Q;
			break;

		case EJ_UP_Q:
			DisplaySequence("BH - EJ up q");
			if (IsEnableBHMark()/* || IsEnableColletHole()*/)
			{
				if (!WaitSearchMarkReady())
				{
					break;
				}
				DisplaySequence("BH - SearchBHMark Done!");
			}

			if (AllowMove() == TRUE)
			{
				if (m_bEnableBHZOpenDac)
				{
					Z_SetOpenDACSwPort(TRUE);
					Z2_SetOpenDACSwPort(TRUE);
				}
				
				if (m_bComplete_T == FALSE)
				{
					T_Sync();
					m_bComplete_T = TRUE;		//v4.43T1

					if (IsBHZ2ToPick())		//If BHZ1 towards BOND
					{
						TakeTime(T2);
					}
					else
					{
						TakeTime(T4);
					}
				}
				bWaferEnd = IsWaferEnded();
				BOOL bOnlyRegionEnd	= IsOnlyRegionEnd(); // xuzhijin_region_end

				OpSyncEjectorTableXY();	//v4.42T3
				//v4.46T10	//SEmitek
				if (s_bDownToPick && !bWaferEnd && !bOnlyRegionEnd)
				{
					if (!OpCheckEjtXYBHPos(IsBHZ1ToPick()))
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}
				if (m_bEnableSyncMotionViaEjCmd || (lEjectorUpDelay >= lTime_Ej))
				{
					//Syn BH at PICK side BEFORE EJ UP
					if (IsBHZ1ToPick())		//If BHZ1 at PICK		
					{
						if (m_bComplete_Z == FALSE)
						{	
							if (!m_bEnableSyncMotionViaEjCmd)
							{
								Z_Sync();
							}
							m_bComplete_Z = TRUE;
							TakeTime(Z4);
						}
						//Sync BH at BOND side if motion completed before EJ UP		//v3.86
					}
					else
					{
						if (m_bComplete_Z2 == FALSE)
						{
							if (!m_bEnableSyncMotionViaEjCmd)
							{
								Z2_Sync();
							}
							m_bComplete_Z2 = TRUE;
							TakeTime(Z8);
						}
					}
				}
				
				lEjectorLvl = m_lEjectLevel_Ej;
				// Calculate the compensation for wear out of ejector pin
				LONG lCompen_Ej = OpCalCompen_Ej();
				//Because BHZ not yet motion completed, so need to wait for delay to move the ejector up
				//WaitForDelayToEjectorUpMove(lTime_Ej, lEjectorUpDelay, s_dbStartZ2PK);
				if (m_bEnableSyncMotionViaEjCmd)
				{
					if (IsBHZ1ToPick())
					{
						TakeTime(Z4);
					}
					else
					{
						TakeTime(Z8);
					}
				}
				//v4.51A16	//SanAn
				if (m_lSPCResult != 1)
				{
					OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "2");	//v4.52A7	//Sanan & Semitek
				}
				BOOL bEjtToUp = FALSE;
				//v4.26T1	//Fixed EJ Up delay bug when Sync-Pick fcn is used
				if (s_bDownToPick && /*!m_bDisableBH &&*/ (IsMS90() || !bWaferEnd) && (IsMS90() || !bOnlyRegionEnd))		//v4.15T1
				{	//	XU_WOLFSPEED
					bEjtToUp = TRUE;
				}
				s_dBHEJSyncTime = 0;
				if (bEjtToUp)
				{
					if (m_bEnableSyncMotionViaEjCmd)
					{
						CString szMsg;
						szMsg.Format("BH - EJ sync up begin to %d, Compen %d", lEjectorLvl, lCompen_Ej);
						DisplaySequence(szMsg);
						TakeTime(EJ1);	

						//v4.53A10
						Ej_SelectObwProfile((LONG)(lEjectorLvl - m_lCurLevel_Ej));
						Ej_SelectControl((LONG)(lEjectorLvl - m_lCurLevel_Ej));

						szMsg = "";
						//v4.46T2	//Semitek for SyncPick with KOffset
						if ((m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0))
						{
							MotionSetupSyncViaEjCmdDistance(BH_AXIS_EJ, &m_stBHAxis_Ej, (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej));
							szMsg.Format("BH - Ej SyncCmd, standby %d ", m_lStandbyLevel_Ej);
						}
TakeTime(LC1);//Matt:to check why Ej2-Ej1 over Ej motion time.
						if (IsBHZ1ToPick())
						{
							MotionSyncViaEjCmdDistance(BH_AXIS_Z, &m_stBHAxis_Z, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);
							szMsg = szMsg + "BH - Z1 SyncCmd";
						}
						else
						{
							MotionSyncViaEjCmdDistance(BH_AXIS_Z2, &m_stBHAxis_Z2, &m_stBHAxis_Ej, m_lSyncTriggerValue, m_lSyncZOffset);
							szMsg = szMsg + "BH - Z2 SyncCmd";
						}
						
//						LONG lSamplingRate = ULTRA_BH_SAMPLE_RATE;
//						CString szProfileID = m_stBHAxis_Ej.m_stMoveProfile[m_stBHAxis_Ej.m_ucMoveProfileID].m_szID;
//						s_dBHEJSyncTime = MotionGetFifthOrderProfileTime(BH_AXIS_EJ, szProfileID, lSamplingRate, (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej), &m_stBHAxis_Ej);
						if (m_lSyncTriggerValue > 0)
						{
							s_dBHEJSyncTime = CalculateEjQueryTime((lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej), m_lSyncTriggerValue);
						}
						else
						{
							s_dBHEJSyncTime = 0;
						}
						//s_dBHEJSyncTime += CalculateEjQueryTime((lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej) - m_lSyncZOffset, (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej) - m_lSyncZOffset);
						s_dBHEJSyncTime += lTime_Ej;
						if( szMsg!="" && SPECIAL_DEBUG_LOG_BH )
							DisplaySequence(szMsg);
						//Ej_Sync();	//v4.54A3
					}
					else
					{
						TakeTime(EJ1);	
					}
				}

				s_dbStartZ2EjUp = GetTime();
				s_dTotalTime_Ej = lTime_Ej;
				if ( bEjtToUp )		//v4.15T1
				{
					CString szMsg;
					szMsg.Format("BH - EJ up final to %d", lEjectorLvl + lCompen_Ej);
					DisplaySequence(szMsg);
					Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);
					m_bComplete_Ej = FALSE;

					OpUpdateMS100EjectorCounter(2);	
					OpUpdateMS100ColletCounter();
				}
TakeTime(LC2);

				if ( m_bComplete_Ej == FALSE )
				{
					BOOL bSyncEj = FALSE;
					if ( m_bEnableSyncMotionViaEjCmd && (CMS896AApp::m_lCycleSpeedMode >= 4) )
					{
						if ((m_lEjectorKCount > 0) && (m_lEjectorKOffset > 0))
						{
							//v4.46T2
							//Because Ej motion time is not the same for each cycle due to KOffset,
							//So need to wait for motion completion instead of using Sleep();
							//LONG lEjKTime = CalculateEjTime(lEjectorLvl + lCompen_Ej) + lEnableSyncMotionViaEjCmdExtraTime;
							LONG lEjKTime = CalculateEjQueryTime((lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej) , (lEjectorLvl + lCompen_Ej - m_lStandbyLevel_Ej));

							//Wait a delay for sysnc with both bondhead Z and Ejector
							WaitForDelay(lEjKTime, s_dbStartZ2EjUp);
							s_dTotalTime_Ej = lEjKTime;
						}
						else
						{
							//Wait a delay for sysnc with both bondhead Z and Ejector
							WaitForDelay(lTime_Ej, s_dbStartZ2EjUp);
						}
					}
					else
					{
						bSyncEj = TRUE;
						//Ej_Sync();
					}
					//TakeTime(EJ2);
					m_bComplete_Ej = TRUE;

					if (bSyncEj)
					{
						Ej_Sync();
					}
					TakeTime(EJ2);
				}

				//Ejector Cap Start to move after Ej Pin arrive at Up level
				if (!m_bDBHStop && m_bMS60EjElevator && (m_lDnOffset_EJ_Cap > 0))
				{
					//EjElevator_MoveTo(m_lStandby_EJ_Cap - m_lDnOffset_EJ_Cap, SFM_NOWAIT);
					//s_dEjectorCapStartTime = GetTime();
				}

				if (IsBHZ1ToPick())		//If BHZ1 at PICK		
				{
					if (m_bComplete_Z2 == FALSE)
					{
//						Z2_Sync();
						//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
						//WaitForDelay(m_lBondTime_Z2, s_dBondZDownStart);
						m_bComplete_Z2 = TRUE;
						TakeTime(Z8);
					}
				}
				else
				{
					if (m_bComplete_Z == FALSE)
					{
//						Z_Sync();
						//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
						//WaitForDelay(m_lBondTime_Z, s_dBondZDownStart);
						m_bComplete_Z = TRUE;
						TakeTime(Z4);
					}
				}

				if (s_dTotalTime_Ej < s_dBHEJSyncTime)
				{
					s_dTotalTime_Ej = s_dBHEJSyncTime;
				}

				//v4.59A41	//re-position to AFTER Z motion complete
				//Collet-Hole shift test for dual arm
				//Matt test begin
				//v4.59A30
				OpUpdateColletHoleShiftTestCounters();

				m_qSubOperation = WAIT_PICK_DELAY_Q;	// -> normal flow		//v4.08
			}
			break;

		case WAIT_PICK_DELAY_Q:
			DisplaySequence("BH - Wait Pick Delay");

			//Syn BH at BOND side
			if (IsBHZ1ToPick())			//If BHZ1 at PICK	
			{
				if (s_bDownToPick)
				{
					m_bBHZ1HasDie = TRUE;
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ1 Pick Die");
				}
				if (m_bComplete_Z2 == FALSE)
				{
//					Z2_Sync();
					//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
					//WaitForDelay(m_lBondTime_Z2, s_dBondZDownStart);
					TakeTime(Z8);
				}
				SetPickVacuumZ2(FALSE);				//Turn off BHZ2 vac on BOND side
				CMSLogFileUtility::Instance()->BT_TableIndexLog("BH2 at BOND");

				if ( (m_bNeutralVacState && !m_bFirstCycle && (m_lNeutralDelay > 0)) || m_bNGPick )
				{
					SetStrongBlowZ2(TRUE);    //NEUTRAL state
				}
			}
			else
			{
				if( !m_bFirstCycle && s_bDownToPick )
				{
					m_bBHZ2HasDie = TRUE;
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ2 Pick Die");
				}
				if (m_bComplete_Z == FALSE)
				{
//					Z_Sync();
					//wait for Bond Z to the bond level of Bin table to sync the pickZ and bondZ when they move up
					//WaitForDelay(m_lBondTime_Z, s_dBondZDownStart);
					TakeTime(Z4);
				}
				SetPickVacuum(FALSE);				//Else Turn off BHZ1 Vac on BOND side
				CMSLogFileUtility::Instance()->BT_TableIndexLog("BH1 at BOND");

				if ( (m_bNeutralVacState && (m_lNeutralDelay > 0)) || m_bNGPick )
				{
					SetStrongBlow(TRUE);    //NEUTRAL state
				}		
			}



			//v4.34T10		//Chck CP2 completion before Bh to continue next cycle
			if (s_bDownToPick && !m_bStop && !m_bIsWaferEnded)	
			{
				if (!WaitCompDone())
				{
					break;
				}
				SetCompDone(FALSE);
			}



			if (!m_bFirstCycle && !m_bChangeGrade)
			{
				DisplaySequence("SetBTStartMoveWithoutDelay3==>FALSE");
				SetWTStartMove(FALSE);
				SetBTStartMoveWithoutDelay(FALSE);
				SetBTStartMove(FALSE);

				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;

				if ( (m_nBHAlarmCode != -1)	&& m_lBHStateCode!=-1 )	//If not MD-RETRY, index BT by 1	
				{
					//	V450X16	at bond, update bond grade and check BT block
					m_lAtBondDieGrade	= m_ucAtPickDieGrade;
					m_lAtBondDieRow		= m_ulAtPickDieRow;
					m_lAtBondDieCol		= m_ulAtPickDieCol;
					LONG lBTBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];	//	["BinTable"]["LastBlkInUse"]
					LONG lBTGrade		= (*m_psmfSRam)["BinTable"]["BondingGrade"];	//	wrong sort bug fix
					szMsg.Format("BH - map (%ld,%ld)%ld BHZ%d At BOND, Bin grade %ld, Blk %ld", m_lAtBondDieRow, m_lAtBondDieCol, m_lAtBondDieGrade,
							IsBHZ1ToPick()+1, lBTGrade, lBTBlkInUse);
					if( IsAutoSampleSort() )
					{
						GetEncoderValue();
						szTemp.Format(", T %ld(%ld), Z1 %d, Z2 %d, EJ %ld",
								m_lEnc_T, m_lCurPos_T, m_lEnc_Z, m_lEnc_Z2, m_lEnc_Ej);
						szMsg = szMsg + szTemp;
					}
					DisplaySequence(szMsg);
					if(pAppMod->GetCustomerName() != "Primaopto")  //4.51D12 
					{
						if( IsBurnIn()==FALSE && SPECIAL_DEBUG_LOG_WS && (m_lAtBondDieGrade > 0) && (lBTGrade > 0) && (m_lAtBondDieGrade != lBTGrade) )
						{
							CString szErr;
							szErr.Format("ERROR: wrong die is bonded!  Current Bin = #%ld but die-grade = #%ld", 
							lBTGrade, m_lAtBondDieGrade);
							SetErrorMessage(szErr);
							HmiMessage_Red_Yellow(szErr);
							m_qSubOperation = HOUSE_KEEPING_Q;
							break;
						}
					}

					//	V450X16	at bond, update bond grade and check BT block
					SetBTReady(FALSE); //2018.3.28 
					m_lBondDieCount++;
					SetDieBonded(TRUE);    //Let BT task to NEXT DIE_Q
				}

				SetDieBondedForWT(TRUE);		//Let WT task to continue
			}
			else	//v4.31T12
			{
				//FOr Yealy MS100Plus Single Loader UNLOAD buffer sequence only
				if (!CMS896AStn::m_bWLReadyToUnloadBufferFrame)
				{
					CMS896AStn::m_bWLReadyToUnloadBufferFrame = TRUE; 
				}	
			}

			if (m_bPreBondAtPick || m_bPostBondAtBond)		//v4.48A8
			{					
			}
			else
			{
				if (m_bOnVacuumAtPick)	//	at pick delay
				{
					if( m_lTCDnOnVacDelay>0 )
						Sleep(m_lTCDnOnVacDelay);

					TakeTime(PV1);	
					if (IsBHZ1ToPick())			//If BHZ1 towards PICK
					{
						DisplaySequence("BH - on BHZ1 vacuum just before pick delay");
						SetPickVacuum(TRUE); 
					}	
					else
					{
						DisplaySequence("BH - on BHZ2 vacuum just before pick delay");
						SetPickVacuumZ2(TRUE);
					}
				}	//	turn on bond head vaccum at pick level
			}

			//Wait for weak blow delay
			if (m_bNeutralVacState && !m_bFirstCycle && (m_lNeutralDelay > 0))
			{
				if (m_lBondDelay > m_lNeutralDelay - COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL)
				{
					WaitTurnOffStrongBlow(m_lNeutralDelay - COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL, s_dBondZDownStart);
					//Wait for Bond delay
					WaitForBondDelay(s_dBondZDownStart);
				}
				else
				{
					//Wait for Bond delay
					WaitForBondDelay(s_dBondZDownStart);
					WaitTurnOffStrongBlow(m_lNeutralDelay - COLLET_VAC_OFF_TIME_BEFOR_Z_TO_BOND_LEVEL, s_dBondZDownStart);
				}
			}
			else
			{
				//Wait for Bond delay
				//WaitForBondDelay(s_dBondZDownStart);
				//TakeTime(LC9);
			}



			//Wait for pick delay
			//WaitForPickDelay(s_dTotalTime_Ej, lEjectorUpDelay, m_lPickDelay, s_dbStartZ2PK);
			//TakeTime(LC10);

			if (m_bMS60EjElevator)
			{
				//EjElevator_MoveTo(m_lStandby_EJ_Cap, SFM_NOWAIT);
			}

			m_bBhInBondState	= FALSE;	
			//m_bIs2DCodeDone		= FALSE;			//v4.40T1
			m_bDetectDAirFlowAtPrePick = FALSE;

			//v4.55A8
			m_bMoveFromPrePick = FALSE;


			//MD response time measurement option in SERVICE page
			if (m_bEnableMDResponseChecking)	//v3.94
			{
				OpCheckMissingDieResponseTime();
			}

			////Collet-Hole shift test for dual arm
			//Update Ejector & Collet counter
			bWaferEnd	= IsWaferEnded();
			if (s_bDownToPick && (IsMS90() ||!bWaferEnd))		//v4.43T12
			{
				OpUpdateMachineCounters();
			}

			if( !m_bFirstCycle )
			{
				if (IsBHZ1ToPick())
				{
					m_bBHZ2HasDie = FALSE;	//
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ2 Bond Die");
				}
				else
				{
					m_bBHZ1HasDie = FALSE;	//
					if( IsMS90() )
						DisplaySequence("BH - Wait Pick Delay, BHZ1 Bond Die");
				}
			}	//	for die left
			
			if (!m_bFirstCycle && !m_bChangeGrade)
			{
				// Find the cycle time
				dCurrentTime = GetTime();
				if (m_dLastTime > 0.0)
				{
					m_dCycleTime = fabs(dCurrentTime - m_dLastTime);

					if (m_dCycleTime > 99999)
					{
						m_dCycleTime = 10000;
					}

					// Calculate the average cycle time
					m_dAvgCycleTime = (m_dAvgCycleTime * m_ulCycleCount + m_dCycleTime) / (m_ulCycleCount + 1);
					m_ulCycleCount++;

					CMSLogFileUtility::Instance()->WT_GetCTLog_CT_NoUpdate(m_dCycleTime);		//v4.06		//MS100+ CT study
				
					// 3501
					SetGemValue("AB_CurrentCycleTime", m_dCycleTime);
					SetGemValue("AB_AverageCycleTime", m_dAvgCycleTime);
					SetGemValue("AB_CycleCounter", m_ulCycleCount);
					// 7704
					// SendEvent(SG_CEID_AB_RUNTIME, FALSE);
				}
				m_dLastTime = dCurrentTime;

				if ( (m_nBHAlarmCode != -1) && m_lBHStateCode!=-1 )		//If not MD-RETRY	//v4.43T12
				{
					OpUpdateBondedUnit(m_dCycleTime);		//Update bonded unit
				}
				OpUpdateDieIndex();				// Update the die index
			}

			//	V450X16	store pick grade
			m_bSetEjectReadyDone = FALSE;
			if (s_bDownToPick)
			{
				//Record wafer map coordinate&Bin on bondhead
				m_ulAtPickDieRow	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
				m_ulAtPickDieCol	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
				m_ucAtPickDieGrade	= (UCHAR)(LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"] - m_WaferMapWrapper.GetGradeOffset();
				szMsg.Format("BH - map (%ld,%ld)%ld BHZ%d At PICK",
					m_ulAtPickDieRow, m_ulAtPickDieCol, m_ucAtPickDieGrade, IsBHZ2ToPick()+1);
			}
			if( IsAutoSampleSort() )
			{
				GetEncoderValue();
				szTemp.Format(", T %ld(%ld), Z1 %d, Z2 %d, EJ %ld",
						m_lEnc_T, m_lCurPos_T, m_lEnc_Z, m_lEnc_Z2, m_lEnc_Ej);
				szMsg = szMsg + szTemp;
			}
			if (s_bDownToPick)
			{
				DisplaySequence(szMsg);
			}
			//	V450X16	store pick grade

			OpCheckPostBondEmptyCountToIncBHZ();	//v4.48A4
			(*m_psmfSRam)["BondHead"]["First Pick Die"] = TRUE;		//v4.54A5

			//m_qSubOperation = MOVE_Z_PICK_UP_Q;
			m_qSubOperation = MOVE_EJ_THEN_T_Q;
			break;

		case MOVE_EJ_THEN_T_Q:
			DisplaySequence("BH - Move Ej then T");
			lArmPicBondDelay = 0;
			if (AllowMove() == TRUE)
			{
				if (lArmPickDelay > lArmBondDelay)
				{
					lArmPicBondDelay = lArmPickDelay; // - lEjectorDownDelay;
				}
				else
				{
					lArmPicBondDelay = lArmBondDelay; // - lEjectorDownDelay;
				}

				TakeTime(CycleTime);
				CString szTemp;
				szTemp.Format("MegaDa Cycle #%d", GetRecordNumber());
				DisplaySequence(szTemp);
				StartTime(GetRecordNumber());	// Restart the time	

				// Move Ej
				if (lEjectorDownDelay > 0)
				{
					Sleep(lEjectorDownDelay);		//Linear Ej
				}		
				if (lEjectorDownDelay >= 0)
				{
					TakeTime(EJ3);	
					Ej_MoveTo(m_lStandbyLevel_Ej, SFM_NOWAIT);
					szMsg.Format("BH - in EJ THEN T, EJT down to standby %d", m_lStandbyLevel_Ej);
					DisplaySequence(szMsg);

					m_dEjDnStartTime = GetTime();
					m_bComplete_Ej = FALSE;
				}

				//** Special Speed-up mode for MS100 100ms testing **//
				bLFDie	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];
				bSwitchPR = m_bPrAoiSwitchCamera;
				if ((lArmPicBondDelay >= lDnTime_Ej) && !bSwitchPR/*m_bPrAoiSwitchCamera*/)	//v4.05		// || !bLFDie)	//v3.94
				{
					//v4.59A42
					//Ej_Sync();
					if ( (lDnTime_Ej > 0) && (lDnTime_Ej < 10))
					{
						Sleep(lDnTime_Ej);
					}
					else
					{
						Ej_Sync();
						if (m_bMS60EjElevator)	
						{
							EjElevator_Sync();	
						}
					}

					TakeTime(EJ4);
					m_bComplete_Ej = TRUE;
					DisplaySequence("BH - TakeTimeEj4, PickBondDelay > EJDownTime & not switch pr");
					//v4.15T2	//Correct bug with big ArmPick delay values to resolve camera blocking problem
					LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"] + m_lPRDelay;
					LONG lBHTMotion		= (LONG)(lArmPicBondDelay - lDnTime_Ej + (1.0 / 3.0 * m_lBondTime_T));
					LONG lExtraWTDelay	= 0;

					if (lBHTMotion > lWTIndexTime)
					{
						lExtraWTDelay = lBHTMotion - lWTIndexTime; 
/*
						BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];	//v4.47T5
						if (bMS60LFCycle)
						{
							lExtraWTDelay = 0;
						}
*/
						if (lExtraWTDelay > 0)
						{
							Sleep(lExtraWTDelay);
						}
					}

					//v4.54A7
					if (s_bDownToPick)
					{
						if (!OpLumiledsSpecialMDCheck())
						{
							if (IsBHZ2ToPick())	//If BHZ2 towards PICK	
							{
								SetErrorMessage("BH: MD2 Check fails at MOVE_T_Q (Lumileds)");
								//CMSLogFileUtility::Instance()->MS_LogMDOperation("BH2 MD alarm start (PLLM)");
								SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ2);
							}
							else
							{
								SetErrorMessage("BH: MD1 Check fails at MOVE_T_Q (Lumileds)");
								//CMSLogFileUtility::Instance()->MS_LogMDOperation("BH1 MD alarm start (PLLM)");
								SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ1);
							}

							OpDisplayAlarmPage(TRUE);
							m_qSubOperation = HOUSE_KEEPING_Q;
							break;
						}
					}
				}

				m_qSubOperation = MOVE_T_Q;
			}
			break;

		case MOVE_T_Q:		//v2.99T1
			DisplaySequence("BH - T Move DBH");
			if (IsEnableColletHole() && m_bDoColletSearch)
			{
				if(!WaitSearchMarkReady())
				{
					break;
				}

				m_bDoColletSearch = FALSE;

				Ej_MoveTo(m_lStandbyLevel_Ej, SFM_WAIT);
				CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
				if (!IsBHZ1ToPick())		//If BHZ1 at Bond	
				{
					Z_MoveTo(m_lSwingLevel_Z);
					pWaferTable->SwitchColletOffset(FALSE, FALSE);
					OpSwitchEjectorTableXY(FALSE, SFM_WAIT);
					T_MoveTo(m_lBondPos_T);
				}
				else
				{
					Z2_MoveTo(m_lSwingLevel_Z2);
					pWaferTable->SwitchColletOffset(TRUE, FALSE);
					OpSwitchEjectorTableXY(TRUE, SFM_WAIT);
					T_MoveTo(m_lPickPos_T);
				}
				SetEjectorVacuum(TRUE);
				Sleep(20);
				DisplaySequence("BH - Search Collet Hole Done");
			}
			
			//Trigger EJ ready to let table move==2019.1.31 after search collet hole======

			if (s_bDownToPick)
			{
				if ((lArmPicBondDelay >= lDnTime_Ej) && !bSwitchPR/*m_bPrAoiSwitchCamera*/)
				{
					if (!IsReachACCMaxCount())
					{
						m_bSetEjectReadyDone = TRUE;
						SetEjectorReady(TRUE, "Move EJ then T");	//Let WT to INDEX
						TakeTime(LC1);
					}
				}
			}
			//for Armpick or Amrbond delay, it will trigger to move theta
			WaitForDelay(lArmPicBondDelay, s_dArmPickUpStart);
			//============================================================================

			lMotionTime_BT	= (*m_psmfSRam)["BinTable"]["MotionTime"];
			m_lSPCResult	= (*m_psmfSRam)["BondPr"]["PostBondResult"];
			bWaferEnd		= IsWaferEnded();
			bLFDie			= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LFDie"];
			if (m_lSPCResult != 1)
			{
				if (!CheckBPRErrorNeedAlarm(1))
				{
					m_lSPCResult = 1;
					(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
				}
			}

			if (!m_bFirstCycle && ((m_nBHAlarmCode != -1) && m_lBHStateCode!=-1) && (m_lBondDieCount > 2))
			{
				if (WaitBPRPostBondDone(5000)) 
				{
//					SetBPRPostBondDone(FALSE);
				}
				else
				{
					CString szErr;
					szErr = _T("ERROR: Postbond Inspection Timeout");
					SetErrorMessage(szErr);
					HmiMessage_Red_Yellow(szErr);

					//TO resolve postbond error hangup problem	//TW
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						m_bComplete_Ej = TRUE;
					}

					Sleep(100);	
					T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			if (IsOnlyRegionEnd())
			{
				T_Sync();	//Must wait T complete for ILC
				Sleep(100);	

				CheckCoverOpenInAuto("Alarm Code Hanler stop");
				T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				break;
			}
		
			if (m_bDisableBT)
			{
				m_lSPCResult = 1;
			}

			//v4.51A16	//SanAn
			if (m_lSPCResult != 1)
			{
				OpCheckSPCAlarmTwice(IsBHZ2ToPick(), "3");	//v4.52A7	//Sanan & Semitek
			}

			if (IsEnableBHMark() || IsEnableColletHole())
			{
				if (((LONG)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej1_CurValueValid"] == 0) ||
					((LONG)(*m_psmfSRam)["WaferPr"]["BondHeadCompentate"]["Ej2_CurValueValid"] == 0))
				{
					bBHMarkStop = TRUE;
					CString szErr;
					if (IsBHZ1ToPick())
					{
						szErr = _T("ERROR: BH Arm1 mark Search Result exceed limit(change 'Update Cycle' value)");
					}
					else
					{
						szErr = _T("ERROR: BH Arm2 mark Search Result exceed limit(change 'Update Cycle' value)");
					}
					SetErrorMessage(szErr);
					HmiMessage_Red_Yellow(szErr);
				}
			}

			if (bBHMarkStop || (m_lSPCResult != 1) && (m_nBHAlarmCode != -4))
			{
				CString szLog;
				szLog.Format("BH: bPostBondNextCycleStop SET - %ld, %d, %lu", 
					m_lSPCResult, m_bSPCAlarmTwice, m_ulCycleCount);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

				//Need to stop at "Next Cycle" instead because curr-die already picked at this moment!
				m_lPrevDieSPCResult = m_lSPCResult;	//v4.24T7
				bPostBondNextCycleStop = TRUE;
			}
			else
			{
				bPostBondNextCycleStop = FALSE;
			}

			if ((m_nBHAlarmCode == -4) ||	//postbond already triggered in "last" cycle	//v4.21T8
				(m_nBHAlarmCode == 6))		//postbond triggerd before BH down to PICK		//v4.21T8	
			{
				//TO resolve postbond error hangup problem	//TW
				if (m_bComplete_Ej == FALSE)
				{
					Ej_Sync();
					TakeTime(EJ4);
					m_bComplete_Ej = TRUE;
				}

				//Sleep(100);
				Sleep(100);		//Extra delay to avoid BH lose-power problem due to ILC		//v4.22T4
				T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
				(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
				m_qSubOperation = POSTBOND_FAIL_Q;
				break;
			}
			else if (m_bDBHStop)
			{
				OpAlarmCodeHandler();
				DisplaySequence("BH - |DBH stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
				break;
			}
			else if (1)	//AllowMove() == TRUE)	//v4.47T7
			{
				BOOL bMS60LFCycle = (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["MS60NewLFCycle"];	//v4.47T5
				if (bMS60LFCycle)	//v4.47T5
				{
					(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;		//v4.10T1
				}
				else
				{
					if (!m_bDBHStop && !bWaferEnd && !m_bStop && !WaitWTStartMove(0) && !IsReachACCMaxCount())
					{
						if ((CMS896AApp::m_lCycleSpeedMode >= 3))		//&& bLFDie)	//v3.89		//v3.94	
						{
							//MS100 BH-T speedup for smaller Arm-PICK delay, if current dei is LF and no-compensate needed
							//No need to wait for WT start-move event
							(*m_psmfSRam)["WaferTable"]["LFDie"] = FALSE;		//v4.10T1
						}
						else
						{
							Sleep(1);
							break;
						}
					}
				}

				SetWTStartMove(FALSE);

				if (IsZMotorsEnable() == FALSE)
				{
					SetMotionCE(TRUE, "IsZMotorsEnable fail in MOVE_T_Q");	//v4.59A19
					SetErrorMessage("BondHead module is off power");
					SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (!OpIsBinTableLevelAtDnLevel())		//v4.44T1	//Semitek
				{
					SetMotionCE(TRUE, "BT level at UP detected in MOVE_T_Q");	//v4.59A19
					SetErrorMessage("BH: BT platform at UP level in MOVE_T_Q !!");
					SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (!OpAlarmCodeHandler())
				{
					//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();	//Must wait T complete for ILC
					Sleep(100);	

					CheckCoverOpenInAuto("Alarm Code Hanler stop");
					T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					DisplaySequence("BH - |Alarm code stop| ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (bPostBondNextCycleStop)
				{
					OpSetMS100AlarmCode(-4);		//postbond NextCycle stop	//v4.21T8	
				}

				if (IsReachACCMaxTimeOut())
				{
					if( m_ulCleanCount < m_ulMaxCleanCount )
					{
						m_ulCleanCount = m_ulMaxCleanCount;
					}
				}
				if (m_bBPRErrorToCleanCollet)
				{
					if( m_ulCleanCount < m_ulMaxCleanCount )
					{
						m_ulCleanCount = m_ulMaxCleanCount;
					}
				}

				INT nColletCleanStatus = 0;		// rescan block camera
				if ((GetWftSampleState() == 0) && (GetWftCleanEjPinState() == 0))	//	block camera when sampling
				{
					nColletCleanStatus = IsCleanCollet();	//	return 0,1,2
				}
				m_bBPRErrorToCleanCollet = FALSE;

				if (nColletCleanStatus != 0)	// do auto clean collet
				{
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						m_bComplete_Ej = TRUE;
					}

					if( m_bSetEjectReadyDone==FALSE )
					{
						m_bSetEjectReadyDone = TRUE;
						SetEjectorReady(TRUE, "Move T Clean Collet != 0");
					}
					TakeTime(LC1);	//v4.42T6
					//v4.22T4		//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();		//Must wait T complete for ILC
					Sleep(10);	
					CheckCoverOpenInAuto("CC2");
					T_MoveTo(m_lPrePickPos_T);
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;

					if (nColletCleanStatus == 2)		//Cleaned
					{
						if (pAppMod->GetCustomerName() == "Testar")
						{
							SetErrorMessage("BH: Clean Collet is reset & continue");		//v4.25
						}

						m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
						if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
						{
							SetBhTReady(TRUE);
							SetBhTReadyForWT(TRUE, "1");		//Let WT to perform WPR	//MS60	//v4.46T22
						}
						m_bFirstCycle = FALSE;	//v3.67
						m_bChangeGrade	= FALSE;
						m_bBhInBondState = FALSE;
						m_qSubOperation = WAIT_WT_Q;
						DisplaySequence("BH - Clean Status 2 to Wait WT");
						break;
					}
					else if (nColletCleanStatus == 1)	//ABORT
					{
						SetErrorMessage("BH: CleanCollet is aborted");		//v4.25
						DisplaySequence("BH - |clean collet aborted| ==> house keeping");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				if (IsEnableILC() == TRUE)
				{
					ILC_AutoUpdateApplyingCycleCountInc();
				}

				//v4.15T8
				BOOL bEnableOsramBinMixMap = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Enable Osram Bin Mix Map"];
				if ((m_nBHLastAlarmCode == -3) && bEnableOsramBinMixMap == TRUE)
				{
					T_MoveTo(m_lPrePickPos_T);
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if ((m_nBHLastAlarmCode == -2) ||	//If curr-pick-die is ChangeGrade NoPickDie
					(m_nBHLastAlarmCode == -3) )	//BIN-FULL //v4.43T10
				{
					//v4.62A12
					//** BIN-FULL (-3) only applicable in this loop for CREE only, when 
					//** 	NextBinFull fcn is enabled;

					//v4.22T4		//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();		//Must wait T complete for ILC
					Sleep(10);	

					CString szLog;
					if (IsBHZ1ToPick())	//If BH! towards PICK
						szLog = "BH: ChangeGrade; last die bonded by BH2"; 
					else
						szLog = "BH: ChangeGrade; last die bonded by BH1"; 

					T_MoveTo(m_lPrePickPos_T);

					if (!m_bFirstCycle && !m_bChangeGrade)	
					{
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE, "2");		//Let WT to perform WPR	//MS60	//v4.46T22
					}

					if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A3
					{
						m_bChangeGrade = TRUE;	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
						szLog.Format("BH: EjtXY ChangeGrade 3; BH2 = %d", IsBHZ1ToPick()); 
						CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
						szLog.Format("BH - BH last alarm code %d with EjtXY ==> Prepick, -2=next die change grade; -3=next die bin full",
							m_nBHLastAlarmCode);

						//v4.62A12		//MS90 in general
						//If Last Grade last die is bonded by BH1, then Next Grade
						// 1st die will be picked & bonded by BH2; however, BT moves earlier after 
						// ChangeBin, so it uses wrong INDEX BH1.  Here, we reverse the BH sequence
						// for BT only, so it moves to correct BH offset (BH2 in this case)
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) !m_bBHZ2TowardsPick;

					}
					else
					{
						CMSLogFileUtility::Instance()->BT_TableIndexLog("BH: Change Grade No Pick Die 3");	//v4.47T10
						DisplaySequence("SetBTStartMoveWithoutDelay4==>FALSE");
						SetBTStartMoveWithoutDelay(FALSE);
						SetBTStartMove(FALSE);	// suggest by Andrew, HuaMao green light idle
						m_bFirstCycle		= TRUE;		
						m_bBHZ2TowardsPick	= FALSE;	//BH1 at PICK = FALSE
						(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v4.47T10
						szLog.Format("BH - BH last alarm code %d ==> Prepick, -2=next die change grade; -3=next die bin full",
							m_nBHLastAlarmCode);
					}

					DisplaySequence(szLog);
					//For BT to move to correct BH1/2 offset XY, in this case 1st die to bond must be BH1
					m_nBHAlarmCode		= 0;			
					m_nBHLastAlarmCode	= 0;	
					m_lBHStateCode		= 0;

					DisplaySequence("SetBTStartMoveWithoutDelay5==>FALSE");
					SetBTStartMoveWithoutDelay(FALSE);
					SetBTStartMove(FALSE);

					EnableBHTThermalControl(TRUE, TRUE);	//v4.49A5
					//SetBondHeadFan(FALSE);				//v4.54A7

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					Sleep(500);
					m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
					break;
				}	//	change grade or bin full

				//v4.54A5
				bMS60NGPickNoLFCycle = FALSE;
				if (CMS896AStn::m_bMS60NGPick && !bLFDie)
				{
					//In NGPick Cycle, if no LF, then BH must move to PREPICK for 
					// WaferPR station to perform 2 PRs, 1 for EMPTY pos and one for next-Die;
					bMS60NGPickNoLFCycle = TRUE;
				}

				BOOL bPickDie = (BOOL)((LONG)(*m_psmfSRam)["WaferTable"]["PickDie"]);
				if (!bPickDie || bMS60NGPickNoLFCycle)
				{
					DisplaySequence("BH - T move, to prepick, Unpick die to Wait WT");
					//	CheckCoverOpenInAuto("Unpick to prepick");
					// The die is only for alignment, move BH to PrePick
					SetWTReady(FALSE, "SetWTReady FALSE !bPickDie || bMS60NGPickNoLFCycle");		//v3.80
					//v4.22T4				//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();				//Must wait T complete for ILC
					Sleep(10);	

					T_MoveTo(m_lPrePickPos_T, SFM_NOWAIT);		//v4.54A10	//MS60 NGPick
					Sleep(10);

					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					m_bChangeGrade	= FALSE;	//	wrong sort bug fix
					m_bFirstCycle	= FALSE;	//v3.67
					m_bBhInBondState = FALSE;
					m_qSubOperation	= WAIT_WT_Q;

					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);
						SetWTReady(FALSE, "SetWTReady FALSE m_bComplete_Ej = FALSE 1");				//REset WT flag again before letting it to move
						m_bComplete_Ej = TRUE;
						if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE )
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move T WT Die not pick, let ej ready");    //Let WT to INDEX
							TakeTime(LC1);	//v4.42T6
						}			
					}

					if (!m_bFirstCycle && !m_bChangeGrade)
					{
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE, "3");		//Let WT to perform WPR	//MS60	//v4.46T22
					}

					break;
				}

				if (bSwitchPR/*m_bPrAoiSwitchCamera*/	|| 
					IsCheckPr2DCode()		|| 
					IsEjtUpAndPreheat()		||
					bEnableBHUplookPr		||						//v4.52A16
					IsMaxColletEjector_CheckOnly(bWaferEnd) )		//v4.50A29
				{
					szMsg.Format("BH - DBH move T q, maybe PR switch camera. 3,switchpr%d,2DCode,%d,EjtUpPreheat,%d,UpLook,%d,CheckCollet,%d,CompleteEj,%d,Pick,%d,Ejready,%d",
bSwitchPR,IsCheckPr2DCode(),IsEjtUpAndPreheat(),bEnableBHUplookPr,IsMaxColletEjector_CheckOnly(bWaferEnd),m_bComplete_Ej,OpCheckAlarmCodeOKToPick(TRUE),m_bSetEjectReadyDone);
					DisplaySequence(szMsg);
					if (m_bComplete_Ej == FALSE)
					{
						Ej_Sync();
						TakeTime(EJ4);

						//2019.3.28 remove SetWTReady becasue it will lead into confuse
//						SetWTReady(FALSE, "SetWTReady FALSE (m_bComplete_Ej == FALSE) 3");				//REset WT flag again before letting it to move
						m_bComplete_Ej = TRUE;
						if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE)		//v4.15T1
						{
							m_bSetEjectReadyDone = TRUE;
							SetEjectorReady(TRUE, "Move T Pr2DCode or EjPreheat, let ej ready");    //Let WT to INDEX
							TakeTime(LC1);	//v4.42T6
						}			
					}

					//v4.22T4	//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();	//Must wait T complete for ILC
					Sleep(10);	

					T_MoveTo(m_lPrePickPos_T, SFM_NOWAIT);

					//v4.57A13
					if (bEnableBHUplookPr)
					{
						TakeTime(T1);
					}

					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					if (bSwitchPR/*m_bPrAoiSwitchCamera*/)
					{
						DOUBLE dTime = GetTime();
						DisplaySequence("BH - DBH move T q, T to prepick, PR switch camera. 4");
						SwitchPrAOICamera();
						CString szMsg;
						szMsg.Format("BH - DBH move T q, PR switch camera. 5 used time %d", LONG(GetTime() - dTime));
						DisplaySequence(szMsg);
					}

					if (IsEjtUpAndPreheat())	//v4.47T8
					{
						Sleep(30);
					}
					else if (bEnableBHUplookPr)	//v4.57A13
					{
						Sleep(10);				
					}
					else
					{
						T_Sync();
					}

					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
					{
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE, "4");		//Let WT to perform WPR	//MS60	//v4.46T22
					}
				
					m_bFirstCycle	= FALSE;	//v3.67
					m_bChangeGrade	= FALSE;	
					m_bBhInBondState = FALSE;
					if (IsCheckPr2DCode())
					{
						m_bIs2DCodeDone	= FALSE;	//v4.52A16
					}
					if (bEnableBHUplookPr)
					{
						m_lUpLookPrStage	= 1;	//v4.52A16
					}

					if (((m_bStop && !m_bBHZ1HasDie && !m_bBHZ2HasDie) || IsSortTo2ndPart() || m_b2PartsAllDone))	//andrewng
					{
						DisplaySequence("BH - MS90 Uplook PR Do 1More Cycle ==> Wait WT");
						m_bDBHStop = TRUE;
					}
					
					DisplaySequence("BH - AOI PR switch ==> Wait WT");
					m_qSubOperation = WAIT_WT_Q;
					break;
				}

				if ((m_bStop && !m_bBHZ1HasDie && !m_bBHZ2HasDie))
				{
					DisplaySequence("BH - MS90 Do 1More Cycle ==> Wait WT NO UPLOOK");
					m_bDBHStop = TRUE;
				}

				//v4.43T8
				bWTNeedRotate	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["LookForward"]["NeedRotate"];
				LONG lWTIndexTime	= (LONG)(*m_psmfSRam)["WaferTable"]["IndexTime"];

				if (lWTIndexTime > BH_WT_MAX_MOTION_TIME || m_bReadyToRealignOnWft || (GetWftSampleState() >= 1) || (GetWftCleanEjPinState() >= 1) || bWTNeedRotate)
				{
					Ej_Sync();
					TakeTime(EJ4);
					if (m_bReadyToRealignOnWft || (GetWftSampleState() >= 1) || (GetWftCleanEjPinState() >= 1) || bWTNeedRotate)
					{
						SetWTReady(FALSE, "m_bReadyToRealignOnWft || (GetWftSampleState() >= 1) || (GetWftCleanEjPinState() >= 1) || bWTNeedRotate");	//REset WT flag again before letting it to move
					}
					m_bComplete_Ej = TRUE;

					if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE )	//	V450X16
					{
						//	should check it is set already after pick die
						m_bSetEjectReadyDone = TRUE;
						SetEjectorReady(TRUE, "Move T WTIndexTime>80 or WTNeedRotate");    //Let WT to INDEX
						TakeTime(LC1);	//v4.42T6
					}				

					//v4.22T4				//Extra delay to avoid BH lose-power problem due to ILC		
					T_Sync();				//Must wait T complete for ILC
					Sleep(10);	

					//v4.35T1
					if (m_bComplete_Z == FALSE)
					{
						if (m_lPickUpTime_Z2 > m_lPickUpTime_Z)
						{
							Z_Sync();
							TakeTime(Z2);
							Z2_Sync();
							TakeTime(Z6);
						}
						else
						{
							Z2_Sync();
							TakeTime(Z6);
							Z_Sync();
							TakeTime(Z2);
						}
					}				
				
					T_MoveTo(m_lPrePickPos_T);

					m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;		//Toggle BHZ1 & BHZ2
					(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;	//v3.67T2
					(*m_psmfSRam)["BondHead"]["AtPrePick"] = 1;
					if (!m_bFirstCycle && !m_bChangeGrade)		//v3.67
					{
						SetBhTReady(TRUE);
						SetBhTReadyForWT(TRUE, "5");		//Let WT to perform WPR	//MS60	//v4.46T22
					}

					CString szMsg;
					szMsg.Format("BH - DBH move to prepick because WTIndexTime %ld, RealignOnWft %d, Sample On Wft %d, %d, WT Rotate %d",
						lWTIndexTime, m_bReadyToRealignOnWft, GetWftSampleState(), GetWftCleanEjPinState(), bWTNeedRotate);
					CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
					DisplaySequence(szMsg);

					m_bFirstCycle	= FALSE;	//v3.67
					m_bChangeGrade	= FALSE;
					m_bBhInBondState = FALSE;
					m_bReadyToRealignOnWft = FALSE;
					if (GetWftSampleState() == 1) 
					{
						szMsg = "BH - move T to sample adv or rescan set 2";
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						DisplaySequence(szMsg);
						m_lReadyToSampleOnWft = 2;
					}
					else if (GetWftCleanEjPinState() == 1)
					{
						szMsg = "BH - move T to Clean Ej Pin set 2";
						CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
						DisplaySequence(szMsg);
						m_lReadyToCleanEjPinOnWft = 2;
					}
					m_qSubOperation = WAIT_WT_Q;
					break;
				}

				//v3.94	
				//No need to count WT duration because WT not yet start move not yet called);
				//Only enter this loop if EJ motion not yet completed -> Arm-PICK/BOND delay < EJ motion time (e.g. 6ms < 8ms)
				if (m_bComplete_Ej == FALSE)
				{
					lWTIndexDuration	= 0;
				
					//v4.37T6	//SpeedMode=5 70ms testing
					if (CMS896AApp::m_lCycleSpeedMode >= 5)		//v4.47T5
					{
						LONG lArmDelay = lArmBondDelay;
						if (lArmPickDelay > lArmBondDelay)
						{
							lArmDelay = lArmPickDelay;
						}
						LONG lEjDnDelay = lEjectorDownDelay;
						if (lEjectorDownDelay < 0)
						{
							lEjDnDelay = 0;
						}
						lEjDnDuration	= lDnTime_Ej - lArmDelay + lEjDnDelay;
					}
					else
					{
						lEjDnDuration	= (LONG)(lDnTime_Ej - (GetTime() - m_dEjDnStartTime));
					}

					if (lEjDnDuration < 0)
					{
						lEjDnDuration = 0;
					}
				}
				else
				{
					dWTIndexStart		= (DOUBLE)(*m_psmfSRam)["WaferTable"]["IndexStart"];
					dWTIndexDuration	= GetTime() - dWTIndexStart;
					lWTIndexDuration	= (LONG) dWTIndexDuration;
					if (dWTIndexStart == 0)
						lWTIndexDuration = 0;		//v4.41T4
					if (lWTIndexDuration > lWTIndexTime)
						lWTIndexDuration = 0;		//v4.41T4
					lEjDnDuration		= 0;
					(*m_psmfSRam)["WaferTable"]["IndexStart"] = 0;			//v4.41T4
				}
			
				lTMoveDelay	= 0;

				//v4.41T4
				//Currently set to zero due to camera blocking bug found in MS100;
				lWTIndexDuration = 0;	

				//v4.46T12	//SeoulSemi
				lEjVacOffDelay = 0;
				if (m_lEjVacOffDelay > 0)
				{
					lEjVacOffDelay = m_lEjVacOffDelay + 1;
				}
				
				if (!bMS60LFCycle)
				{
					CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
					ULONG ulDelay = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_DELAY_BEFORE_TURN_OFF_EJ_VAC);
					if ((ulDelay > 0) && (ulDelay < 200))
					{
						lEjVacOffDelay += ulDelay;
					}
				}

				//v4.52A1
				if (CMS896AApp::m_lCycleSpeedMode >= 6)			//v4.48A22
				{
					//Add extra 2ms to calculate camera blocking for 40ms profile
					lTMoveDelay = lEjDnDuration + lWTIndexTime - lWTIndexDuration + m_lPRDelay + lEjVacOffDelay + 2;	
				
					//v4.49A3	//Extra PR delay due to longer exposure time for WPR normal die
					if (bPickDie && (lExtraPrGrabDelay > 0))
					{
						lTMoveDelay = lTMoveDelay + lExtraPrGrabDelay;
					}
				}
				else if (CMS896AApp::m_lCycleSpeedMode >= 3)			//v3.93T1
				{
					lTMoveDelay = lEjDnDuration + lWTIndexTime - lWTIndexDuration + m_lPRDelay + lEjVacOffDelay;
				}
				else
				{
					lTMoveDelay = lWTIndexTime - lWTIndexDuration + m_lPRDelay + lEjVacOffDelay + 4;
				}

				lTMaxUnblockTime = (LONG)(2.0 / 3.0 * m_lBondTime_T) + 2;			//v4.24T5	//WanYiMing		
				CString szMsg;

				//v4.10T1	//Add extra PR delay for long-travel motion
				if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS60/MS100P3/MS90		//v4.50A24
				{
					lWTLongJumpExDelay		= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpDelay"];
					lWTLongJumpMotionTime	= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpMotion"];

					if ((lWTLongJumpMotionTime >= 25) && (lWTIndexTime >= lWTLongJumpMotionTime))
					{
						lTMoveDelay = lTMoveDelay + lWTLongJumpExDelay + 1;		
					}
				}
				else if (CMS896AApp::m_lCycleSpeedMode >= 4)	
				{
					lWTLongJumpExDelay		= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpDelay"];
					lWTLongJumpMotionTime	= (LONG)(*m_psmfSRam)["WaferTable"]["LongJumpMotion"];

					if ((lWTLongJumpMotionTime >= 30) && (lWTIndexTime >= lWTLongJumpMotionTime))
					{
						lTMoveDelay = lTMoveDelay + lWTLongJumpExDelay + 1;		//v4.46T12	//SeoulSemi
					}
				}

				//v4.54A6
				if (bMS60LFCycle)
				{
					lTMoveDelay = 0;
				}

				if (lTMoveDelay > lTMaxUnblockTime)
				{
					if (CMS896AApp::m_lCycleSpeedMode >= 3)		//v3.93T1
					{
						//dEjDuration = GetTime();
						LONG lTDelay		= lTMoveDelay - lTMaxUnblockTime;
						LONG lEjTime		= (LONG)(GetTime() - m_dEjDnStartTime);
						LONG lEjRemainTime	= lDnTime_Ej - lEjTime;
						LONG lEjExtraMotionCompleteTime = 0;
						if (lEjRemainTime < 0)
						{
							lEjRemainTime = 0;
						}

						if (lTDelay >= lEjRemainTime)
						{
							//Let WT to proceed if still enough time for T to wait
							//Only enter this loop if EJ motion not yet completed -> Arm-PICK/BOND delay < EJ motion time (e.g. 6ms < 8ms)
							if (m_bComplete_Ej == FALSE)
							{
								if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS60 & above only
								{
									if (lEjRemainTime > 0)
										Sleep(lEjRemainTime);
								}
								else
								{
									Ej_Sync();
								}

								TakeTime(EJ4);

								if( m_bSetEjectReadyDone==FALSE )
								{
									m_bSetEjectReadyDone = TRUE;
									SetEjectorReady(TRUE, "Move T CS>=3");			//Let WT to INDEX
								}
								m_bComplete_Ej = TRUE;

								//v4.53A8	//XM SanAn
								lEjExtraMotionCompleteTime = (LONG)(GetTime() - m_dEjDnStartTime);
								if ( lEjExtraMotionCompleteTime > (lDnTime_Ej+1) )
								{
									lEjExtraMotionCompleteTime = lEjExtraMotionCompleteTime - lDnTime_Ej - 1;
								}
								else
								{
									lEjExtraMotionCompleteTime = 0;
								}
							}

							//lEjCompleteTime = m_lTime_Ej - lEjTime;	
							lTDelay = lTDelay - lEjRemainTime + lEjExtraMotionCompleteTime;	//Remaing Delay after waiting for Ej Motion Completion
						}
						else
						{
							//Else no need to wait Ej completion here and let T move first!
						}

						lTMoveDelay = lTDelay;

						if (m_lBlockCameraDelay > 0)
						{
							lTMoveDelay = lTMoveDelay + m_lBlockCameraDelay;	//v4.52A3
						}
					}
					else
					{
						lTMoveDelay = lTMoveDelay - lTMaxUnblockTime + 2;
					}

					if (m_bDBHStop || m_bStop)
					{
						if (lTMoveDelay <= 0)
						{
							lTMoveDelay = 10;
						}
						else
						{
							lTMoveDelay = lTMoveDelay + 10;    //v4.21T3	//75ms cycle testing
						}		
					}

					if (lTMoveDelay > 0)
					{
						//Sleep(lTMoveDelay);
					}
				}
				else
				{
					if (m_bDBHStop || m_bStop)
					{
						Sleep(10);    //v4.21T3	//75ms cycle testing
					}							
				}

				m_ctBHTAutoCycleLastMoveTime = CTime::GetCurrentTime();
				if (IsBHZ1ToPick())		//If BHZ1 currently at PICK 	
				{
					TakeTime(T1);
					nResult = BA_MoveTo(m_lBondPos_T, SFM_NOWAIT);
				}
				else
				{
					TakeTime(T3);
					nResult = BA_MoveTo(m_lPickPos_T, SFM_NOWAIT);
				}

				if (nResult != gnOK)	//v4.47A1
				{
					HmiMessage_Red_Yellow("Bond Arm channel error");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				m_bComplete_T = FALSE;
				m_bBHZ2TowardsPick = !m_bBHZ2TowardsPick;	//Toggle BHZ1 & BHZ2
				(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"] = (BOOL) m_bBHZ2TowardsPick;
				m_lTime_T = m_lBondTime_T;		//v3.61
				s_dbStartT2PB = GetTime();			//v2.83T7
				m_qSubOperation = WAIT_Z_EJ_COMPLETE_Q;
			}
			break;

		case WAIT_Z_EJ_COMPLETE_Q:
			DisplaySequence("BH - wait Z EJ complete q");
			//Re-enabled for MS100 100ms speedup //No need to be disabled
			// Allow WT to move earlier than Ej completed
			if (m_bComplete_Ej == FALSE)
			{
				lDelay = m_lDnTime_Ej - _round(GetTime() - m_dEjDnStartTime);
				if (CMS896AApp::m_lCycleSpeedMode >= 6)		//MS60 & above only		//v4.53A23
				{
					if (lDelay > 0)
						Sleep(lDelay);
				}
				else
				{
					Ej_Sync();
				}

				TakeTime(EJ4);

				//2019.03.27 remove SetWTReady option becasue it will lead into confuse.
//				SetWTReady(FALSE, "SetWTReady FALSE WAIT_Z_EJ_COMPLETE_Q");						//REset WT flag again before letting it to move
				
				if (OpCheckAlarmCodeOKToPick(TRUE) && m_bSetEjectReadyDone==FALSE )		//v4.15T1
				{
					m_bSetEjectReadyDone = TRUE;
					SetEjectorReady(TRUE, "Wait Z EJ complete");    //Let WT to INDEX
					TakeTime(LC1);	//v4.42T6
				}		

				//Ej_Sync();
				m_bComplete_Ej = TRUE;
			}

			if (CMS896AApp::m_lCycleSpeedMode < 4)		//v4.02T5
			{
				//Original Checking sequence for >=100ms cycle mode
				if (m_bComplete_Z == FALSE)
				{
					if (m_lPickUpTime_Z2 > m_lPickUpTime_Z)
					{
						Z_Sync();
						TakeTime(Z2);
						Z2_Sync();
						TakeTime(Z6);
					}
					else
					{
						Z2_Sync();
						TakeTime(Z6);
						Z_Sync();
						TakeTime(Z2);
					}
				}				
			}

			if (IsEnableILC() == TRUE)
			{
				BOOL bILCResult = TRUE;
				if (IsBHZ2ToPick())	
				{
					bILCResult = ILC_AutoUpdateInCycle(BH_T_PICK_TO_BOND);
				}			
				else
				{
					bILCResult = ILC_AutoUpdateInCycle(BH_T_BOND_TO_PICK);
				}

				if (!bILCResult)	//v4.47T9
				{
					SetErrorMessage("DBH: ILC Update fails");
					CycleEnableDataLog(FALSE, TRUE, TRUE);
					HmiMessage_Red_Yellow("ILC Update fails; machine will be stopped");
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			//MS60 LF sequence will call this fcn in WT::OpMoveTable to move EJT 
			//together with WT XY;
			if (!m_bWaferPrMS60LFSequence)		//v4.59A39
			{
				if (!OpMoveEjectorTableXY(FALSE))	//v4.42T3
				{
					SetErrorMessage("Ejector XY module is off power");
					SetAlert_Red_Yellow(HMB_BH_EJTXY_NO_POWER) ;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
			}

			lTemp = (LONG)(GetTime() - s_dbStartT2PB);

			if (m_lTime_T >= 60)							//MS100
				lTMinPrUnblockTime = 20;
			else if (IsMS50())								//MS50		//v4.59A41
			{
				lTMinPrUnblockTime = 5;
				//ULONG ulCameraBlockDelay = pAppMod->GetFeatureValue(MS896A_FUNC_GENERAL_CAMERA_BLOCK_DELAY);
				//if ((ulCameraBlockDelay > 0) && (ulCameraBlockDelay < 10))
				//{
				//	lTMinPrUnblockTime += ulCameraBlockDelay;
				//}
			}
			else if (CMS896AApp::m_lCycleSpeedMode >= 6)	//Plus3/MS60/MS90
				lTMinPrUnblockTime = 8;
			else if (CMS896AApp::m_lCycleSpeedMode >= 4)	//MS100Plus/Plus2
				lTMinPrUnblockTime = 12;
			else
				lTMinPrUnblockTime = 15;

			if (m_lDnOffset_EJ_Cap > 0)
			{
				LONG lEJCapUpDelay = 5;
				DOUBLE dRemainEJCapTime = (m_lUpTime_Ej_Cap + m_lDnTime_Ej_Cap) - (GetTime() - s_dEjectorCapStartTime) + lEJCapUpDelay;
				if (dRemainEJCapTime < 0)
				{
					dRemainEJCapTime = 0;
				}
				lTMinPrUnblockTime = max(_round(dRemainEJCapTime), lTMinPrUnblockTime);
			}

			if (lTemp < lTMinPrUnblockTime)
			{
				//Sleep(lTMinPrUnblockTime - lTemp);
			}

			if (!m_bFirstCycle && !m_bChangeGrade)	//	HuaMao 3 ChangeGrade, 1 die full, green light no production.
			{
				DisplaySequence("BH - let BPR to grab, set BHT ready");
//				TakeTime(LC3);
				SetBhTReady(TRUE);						//Let BPR Grade image
				SetBhTReadyForWT(TRUE, "6");		//Let WT to perform WPR	//MS60
			}
		
			//v4.02T5
			if (m_bComplete_Z == FALSE)
			{
				if (!IsMS60())	//v4.50A4
				{
					if (m_lPickUpTime_Z2 > m_lPickUpTime_Z)
					{
						Z_Sync();
						TakeTime(Z2);
						Z2_Sync();
						TakeTime(Z6);
					}
					else
					{
						Z2_Sync();
						TakeTime(Z6);
						Z_Sync();
						TakeTime(Z2);
					}
				}				
			}				

			if( m_bSetEjectReadyDone == FALSE )
			{
				m_bSetEjectReadyDone = TRUE;
				SetEjectorReady(TRUE, "Wait Z EJ Complete at last");
			}

			m_bFirstCycle	= FALSE;
			m_bChangeGrade	= FALSE;
			m_qSubOperation = WAIT_WT_READY_Q;	
			break;

		//**** Currently not used ****//
		case WAIT_BT_CHANGE_GRADE:
			DisplaySequence("BH - Wait BT Change Grade");
			if (m_bStop)
			{
				DisplaySequence("BH - |Wait BT change grade stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if (WaitBTStartMove())
			{
				//Assum wafer table die is ready
				SetWTReady(FALSE, "FALSE WAIT_BT_CHANGE_GRADE");
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				(*m_psmfSRam)["BinTable"]["Change Grade"] = FALSE;
				DisplaySequence("BH - Wait BT ChangeGrade done ==> T Prepick to pick");
CMSLogFileUtility::Instance()->BL_LogStatus("BH - Wait BT ChangeGrade done ==> T Prepick to pick\n");	//v4.48A26	//By Andrew
			}
			else
			{
				Sleep(1);
			}
			break;


		case WAIT_BIN_FULL_Q:
			DisplaySequence("BH - Wait Bin Full");

			// Wait for bin full alert issued by Bin Table Station
			if (m_bStop)
			{
				SetErrorMessage("BH: BIN_FULL -> stop");
				DisplaySequence("BH - |Wait Bin full stop| ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			//v4.57A11
			if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
			{
				BOOL b2DStatus = TRUE;
				b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				m_lUpLookPrStage = 0;	//reset
			}

			if (WaitBTStartMove())
			{
				//Assum wafer table die is ready
				SetWTReady(FALSE, "FALSE WAIT_BIN_FULL_Q");
				m_qSubOperation = MOVE_T_PREPICK_TO_PICK_Q;
				DisplaySequence("BH - Wait Bin Full ==> T Prepick to pick");
				//	WPR confirm search done, but this cycle will not reset, next move to wait and loop time out.
				SetConfirmSrch(FALSE);
			}
			else
			{
				Sleep(1);
			}
			break;


		case POSTBOND_FAIL_Q:
			DisplaySequence("BH - SPC failed");
			lSPCResult = m_lPrevDieSPCResult;	//(*m_psmfSRam)["BondPr"]["PostBondResult"];	//v4.24T7
			EmptyCounter = (*m_psmfSRam)["BondPr"]["EmptyCounter"];
			EmptyCounter2 = (*m_psmfSRam)["BondPr"]["EmptyCounter2"];
			AccEmptyCounter = (*m_psmfSRam)["BondPr"]["AccEmptyCounter"];
			AccEmptyCounter2 = (*m_psmfSRam)["BondPr"]["AccEmptyCounter2"];
			AccEmptyCounterTotal = (*m_psmfSRam)["BondPr"]["AccEmptyCounterTotal"];
			GenMaxEmptyAllow = (*m_psmfSRam)["BondPr"]["MaxEmptyAllow"];
			GenMaxAccEmptyAllow = (*m_psmfSRam)["BondPr"]["MaxAccEmptyAllow"];

			if (m_bDisableBT)
			{
				lSPCResult = 1;
			}
			switch (lSPCResult)
			{
			case BPR_ERR_SPC_XY_LIMIT:
				OpDisplayAlarmPage(TRUE);	//v4.53A13
				SetAlert_Red_Yellow(IDS_BH_SPC_XY_LIMIT);		// Set Alarm
				szMsg = _T("Postbond hit XY Limit Count");
				break;

			case BPR_ERR_SPC_XY_LIMIT2:		//v4.57A14
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_XY_LIMIT);		// Set Alarm
				szMsg = _T("Postbond hit XY Limit (BH2) Count");
				break;

			case BPR_ERR_SPC_MAX_ANGLE:
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_MAX_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Max Angle (BH1) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ANGLE2:		//v4.57A14
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_MAX_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Max Angle (BH2) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ACC_ANGLE:		//v4.58A5
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_MAX_ACC_ANGLE);	// Set Alarm
				szMsg = _T("Postbond hit Max ACC Angle (BH1) Limit Count");
				break;

			case BPR_ERR_SPC_MAX_ACC_ANGLE2:	//v4.58A5
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_MAX_ACC_ANGLE);	// Set Alarm
				szMsg = _T("Postbond hit Max ACC Angle (BH2) Limit Count");
				break;

			case BPR_ERR_SPC_AVG_ANGLE:
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH_SPC_AVG_ANGLE);		// Set Alarm
				szMsg = _T("Postbond hit Average Angle Limit");
				break;

			case BPR_ERR_SPC_DEFECT:
				SetAlert_Red_Yellow(IDS_BH_SPC_DEFECT);			// Set Alarm
				szMsg = _T("Postbond hit Defect Limit Count");
				break;

			case BPR_ERR_SPC_EMPTY:
				LogPBFailureCaseDumpFile();
				OpDisplayAlarmPage(TRUE);	//v4.53A13
				szMsg.Format("Postbond hits EMPTY Limit Count.\nBH1: Continuation=%ld(%ld), Accumlation=%ld\nBH2: Continuation=%ld(%ld), Accumlation=%ld\nTotal Accumlation=%ld(%ld)\n", EmptyCounter, GenMaxEmptyAllow, AccEmptyCounter, EmptyCounter2, GenMaxEmptyAllow, AccEmptyCounter2, AccEmptyCounterTotal, GenMaxAccEmptyAllow);
				SetAlert_Msg_Red_Yellow(IDS_BH_SPC_EMPTY, szMsg);
				SetErrorMessage(szMsg);
				break;

			case BPR_ERR_SPC_EMPTY2:		//v4.57A14
				LogPBFailureCaseDumpFile();
				OpDisplayAlarmPage(TRUE);
				SetAlert_Red_Yellow(IDS_BH2_SPC_EMPTY);			// Set Alarm
				szMsg = _T("Postbond hit EMPTY Limit (BH2) Count");
				break;

			case BPR_ERR_SPC_REL_XY_LIMIT:						//v4.26T1	//Semitek	//Relative XY shift check
				SetAlert_Red_Yellow(IDS_BH_SPC_REL_XY_LIMIT);	// Set Alarm
				szMsg = _T("Postbond hit relative-XY shift Limit Count");
				break;

			default:
				SetAlert_Red_Yellow(IDS_BH_SPC_FAIL);			// Set Alarm
				szMsg.Format("Postbond Detect Error; code=%ld", lSPCResult);	//v4.23T2
				break;
			}
			szTemp.Format(" die picked %d", GetNewPickCount());
			SetErrorMessage(szMsg + szTemp);
			DisplaySequence("BH - |post bond fail| ==> house keeping");
			m_qSubOperation = HOUSE_KEEPING_Q;
			break;

		case HOUSE_KEEPING_Q:
			DisplaySequence("BH - DBH House Keeping Q");
			SetAutoCycleLog(FALSE);
			if (GetHouseKeepingToken("BondHeadStn") == TRUE)
			{
				SetHouseKeepingToken("BondHeadStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}


			if( IsMS90() && (pAppMod->GetCustomerName()=="WolfSpeed" || pAppMod->GetCustomerName()=="Finisar"))
			{
				if( m_bBHZ2HasDie )
				{
					SetPickVacuumZ2(TRUE);
					Sleep(500);
					if( (m_bCheckMissingDie == TRUE) &&	(m_bPickVacuumOnZ2 == TRUE) && (IsMissingDieZ2() == FALSE) )
					{
						HmiMessage_Red_Back("Bond head 2 has die un-bonded", "Auto Sort");
					}
					else
					{
						DisplaySequence("BH - BHZ2 logic die left, but lost");
						SetPickVacuumZ2(FALSE);
						m_bBHZ2HasDie = FALSE;
					}
				}
				else if( m_bBHZ1HasDie )
				{
					SetPickVacuum(TRUE);
					Sleep(500);
					if ((m_bCheckMissingDie == TRUE) && (m_bPickVacuumOn == TRUE) && (IsMissingDie() == FALSE))
					{
						HmiMessage_Red_Back("Bond head 1 has die un-bonded", "Auto Sort");
					}
					else
					{
						DisplaySequence("BH - BHZ1 logic die left, but lost");
						SetPickVacuum(FALSE);
						m_bBHZ1HasDie = FALSE;
					}
				}
			}
			else
			{
				SetPickVacuum(FALSE); // Matthew by Harry Ho
				Sleep(500);
				SetBHStrongBlow(0, TRUE);
				Sleep(500);
				SetPickVacuumZ2(FALSE);
				Sleep(500);
				SetBHStrongBlow(1, TRUE);
				Sleep(500);
							
				m_bBHZ2HasDie = FALSE;
				m_bBHZ1HasDie = FALSE;
			}

			// Make sure the motion is complete first
			// Wait Z complete
			if (m_bComplete_Z == FALSE)
			{
				Z_Sync();
				Z2_Sync();
			}					
			// Wait Ej complete
			if (m_bComplete_Ej == FALSE)
			{
				Ej_Sync();
				m_bComplete_Ej = TRUE;
			}					
			// Wait T complete
			if (m_bComplete_T == FALSE)
			{
				T_Sync();
			}

			//Disable NuMotion data log earlier here	//v4.17T7
			CycleEnableDataLog(FALSE, TRUE, TRUE);

			//SetPickVacuum(FALSE);			// Turn-off the pick vacuum		//v2.77
			if (Z_IsPowerOn())				//v3.13T3
			{
				AC_Z1_MoveTo(0, SFM_WAIT);		// Move Z to Zero
			}		

			if (Z2_IsPowerOn())
			{
				AC_Z2_MoveTo(0, SFM_WAIT);
			}

			SetEjectorVacuum(FALSE);
			Sleep(100);
			Ej_MoveTo(0, SFM_WAIT);			// Move Ej to Zero

			if (m_bMS60EjElevator)	
			{
				if (EjElevator_IsPowerOn())	
				{
					//EjElevator_MoveTo(m_lStandby_EJ_Cap, SFM_WAIT);	

				}
				else
				{
					EjElevator_Home();
				}
			}

			//v3.98T3
			TakeTime(CycleTime);
			StartTime(GetRecordNumber());	// Restart the time	
			//andrew
			if (bEnableBHUplookPr && m_lUpLookPrStage==2)	
			{
				BOOL b2DStatus = TRUE;
				b2DStatus = UplookPrSearchDie_Rpy2(!m_bBHZ2TowardsPick);
				m_lUpLookPrStage = 0; //reset
			}

			//if ((m_ulMaxColletCount > 0) && (m_ulColletCount > 0))	//v4.40T7
			//{
			//	m_ulColletCount = m_ulColletCount - 1;
			//}
			//20171025 Leo Protection of hit hand
			CheckCoverOpenInAuto("During Bonding Cycle");
			
			if (IsBHZ1ToPick())		//If BHZ1 at PICK	
				LogCycleStopState("BH - MoveArm; BH2 to prepick(clean collet) at last cycle");
			else
				LogCycleStopState("BH - MoveArm; BH1 to prepick(clean collet) at last cycle");

			bRegionEnd	= (BOOL)(LONG)(*m_psmfSRam)["WaferTable"]["WaferRegionEnd"]; // xuzhijin_region_end
			bWaferEnd	= IsWaferEnded();
			bColletFull = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ColletFull"];

			if (Z_IsPowerOn() && T_IsPowerOn() && Z2_IsPowerOn())			//v3.13T3
			{
				if( IsPrescanning()==FALSE )
					CheckCoverOpenInAuto("House Keeping");
				if ((bWaferEnd == TRUE) || (bColletFull == TRUE) || (bRegionEnd == TRUE))
				{
					T_MoveTo(m_lCleanColletPos_T, SFM_WAIT);	// Move T to Blow position
				}
				else
				{
					T_MoveTo(m_lPrePickPos_T, SFM_WAIT);		// Move T to Pre-Pick
				}
			}

			if (!m_bUplookResultFail)		//v4.59A7	//MS90
			{
				if( IsMS90() && (pAppMod->GetCustomerName()=="WolfSpeed" ||pAppMod->GetCustomerName()=="Finisar" )&& m_bBHZ1HasDie )
				{
				}
				else
					SetPickVacuum(FALSE);		// Turn-off the pick vacuum after T-Move done		//v2.77
				if( IsMS90() && (pAppMod->GetCustomerName()=="WolfSpeed" ||pAppMod->GetCustomerName()=="Finisar" ) && m_bBHZ2HasDie )
				{
				}
				else
					SetPickVacuumZ2(FALSE);		// Turn-off the pick vacuum after T-Move done		//v3.86
			}

			if (bWaferEnd)				//v4.40T2	//Nichia MS100+
			{	
				LogCycleStopState("BH - WaferEnd");	//v4.44A1	//SEmitek
				OpUpdateNichiaRawMaterialRecordsAtWaferEnd();
				OpResetEjKOffsetAtWaferEnd();	//v4.43T8	//Semitek
			}

			if (pSRInfo->IsRegionEnding())
			{
				CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
				if (pUtl->GetPrescanRegionMode())
				{
					m_ulEjCountForSubRegionKOffset = 0;
					m_ulEjCountForSubRegionSKOffset = 0;
					m_lEjSubRegionSmartValue = 0;
					pSRInfo->SetRegionEnding(FALSE);
					SaveScanTimeEvent("BH: REGION END reset k offset house keeping");
				}
			}

			CMS896AStn::m_lBondHeadAtSafePos = 1;
			(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;

			LogCycleStopState("BH - UpdateDieCounter");
			UpdateDieCounter();			//Update die counter in HMI

			LogCycleStopState("BH - LogWaferInfo");
			LogWaferInformation(FALSE);	//Update current wafer counter

			LogCycleStopState("BH - MachineStat");
			SaveMachineStatistic();		//update machine statistic	

			ILC_SaveAllResult();

		//	CMS896AStn::m_lBondHeadAtSafePos = 1;
			// Acknowledge other stations to stop
			StopCycle("BondHeadStn");
			LogCycleStopState("BH - dbh stop completely");
			State(STOPPING_Q);

			SetHouseKeepingToken("BondHeadStn", TRUE);

			if( IsPrescanning() && IsCoverOpen() )
			{
				SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
				SetStatusMessage("Machine Cover Open");
				SetErrorMessage("Machine Cover Open when prescanning");
			}
			break;

		default:
			DisplaySequence("BH - Unknown");
			m_nLastError = glINVALID_STATE;
			break;
		}

		if (m_nLastError != gnAMS_OK)
		{
			if (!IsAbort())
			{
				SetError(m_nLastError);
			}

			Result(gnNOTOK);
			State(ERROR_Q);
		}
		else
		{
			m_qPreviousSubOperation = m_qSubOperation;
		}
		if (m_bHeapCheck == TRUE)
		{
			// Check the heap
			INT nHeapStatus = _heapchk();
			if (nHeapStatus != _HEAPOK)
			{				
				CString szMsg;
				szMsg.Format("BH - Heap corrupted (%d) before BH", nHeapStatus);
				DisplayMessage(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		//Handle normal stop cycle task
		LogCycleStopState("BH ex - UpdateDieCounter");
		UpdateDieCounter();			

		LogCycleStopState("BH ex - LogWaferInfo");
		LogWaferInformation(FALSE);	

		LogCycleStopState("BH ex - MachineStat");
		SaveMachineStatistic();		

		CMS896AStn::m_lBondHeadAtSafePos = 1;

		DisplayException(e);
		Result(gnNOTOK);

		//State(ERROR_Q);
		State(STOPPING_Q);

		NeedReset(TRUE);
		StopCycle("BondHeadStn");

		//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
		CString szErr;
		szErr.Format("Exception detected in BH Cycle = %d, %d", m_qPreviousSubOperation, m_qSubOperation);
		SetMotionCE(TRUE, szErr);	//v4.59A19
	}
}



BOOL CBondHead::OpAlarmCodeHandler()
{
	BOOL bReset = TRUE;
//	LONG lSPCResult = 0;
	CString szErr = _T("none");

	switch (m_nBHAlarmCode)
	{
//OK
	case 0:	
		bReset = TRUE;
		break;

//WARNING
	case -1:		//MD Retry
		szErr	= _T("BH Warning: MD Retry warning");
		bReset = TRUE;
		break;
	case -2:		//ChangeGrade NoPickDie on pick side		//v4.15T1
		szErr	= _T("BH Warning: ChangeGrade NoPickDie");
		bReset = TRUE;
		break;
	case -3:		//NextBinFull NoPickDie on pick side		//v4.21T2
		szErr	= _T("BH Warning: NextBinFull NoPickDie");
		bReset = TRUE;
		break;
	case -4:		//BPR post-bond failure wanring for Next-Cycle Stop		//v4.21T8
		szErr.Format("BH Warning: postbond PR failure code=(%ld) - Next Cycle Stop", m_lPrevDieSPCResult);
		bReset = TRUE;
		break;

//ERROR
	case 1:			//Low pressure
		szErr	= _T("BH Alarm: Machine low in pressure");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);
		break;
	case 2:			//Machine cover open
		szErr	= _T("BH Alarm: Machine Cover Open");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		break;
	case 3:			//CJ
		szErr	= _T("BH Alarm: CJ Alarm triggered");
		bReset = FALSE;
		break;
	case 5:			//Wafer-END
		szErr	= _T("BH Alarm: WAFER-END triggered");
		bReset = FALSE;
		break;
	case 6:			//BPR post-bond failure
		szErr.Format("BH Alarm: postbond PR failure code=(%d) - ", m_lPrevDieSPCResult); // lSPCResult);
		//v4.15T1
		if (m_lPrevDieSPCResult == 10)
		{
			szErr = szErr + "XY-Limit count is hit";
		}
		else if (m_lPrevDieSPCResult == 11)
		{
			szErr = szErr + "Max. Angle rotation exceeds limit";
		}
		else if (m_lPrevDieSPCResult == 12)
		{
			szErr = szErr + "Average Angle rotation exceeds limit";
		}
		else if (m_lPrevDieSPCResult == 13)
		{
			szErr = szErr + "DEFECT dice exceeds limit";
		}
		else if (m_lPrevDieSPCResult == 14)
		{
			szErr = szErr + "EMPTY dice exceeds limit";
		}
		bReset = FALSE;
		HmiMessage_Red_Yellow(szErr);
		break;
	case 7:			//Theraml Alarm
		szErr	= _T("BH Alarm: Machine Thermal alarm is triggered");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH_THERMAL_CHECK);
		break;
	case 8:			//MS60 BH Fan Alarm		//v4.48A1
		szErr	= _T("BH Alarm: Machine BH Fan alarm is triggered");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH_THERMAL_CHECK);
		break;

	case 11:			//BH2 Low pressure
		szErr	= _T("BH Alarm: Machine BH2 low in pressure");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH2_LOW_PRESSURE);
		break;
	case 12:			//EJ Low pressure
		szErr	= _T("BH Alarm: Machine EJ low in pressure");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_EJ_LOW_PRESSURE);
		break;
	case 13:			//Input Ionizer Error Bit ON
		szErr	= _T("BH Alarm: Machine Input Ionizer Error Bit ON");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH_INPUT_IONIZER_ERR);
		break;
	case 14:			//Output Ionizer Error Bit ON
		szErr	= _T("BH Alarm: Machine Output Ionizer Error Bit ON");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH_OUTPUT_IONIZER_ERR);
		break;

	//v4.59A12
	case 15:			//BH1 Low pressure
		szErr	= _T("BH Alarm: Machine BH1 low in pressure");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH1_LOW_PRESSURE);
		break;
	case 16:	//	low vacuum flow
		szErr	= _T("BH Alarm: Machine low in vacuum flow");
		bReset = FALSE;
		SetAlert_Red_Yellow(IDS_BH_LOW_VACUUM_FLOW);
		break;
	default:
		szErr.Format("BH Alarm Error: Alarm Code = %d", m_nBHAlarmCode);
		bReset = FALSE;
		HmiMessage_Red_Yellow(szErr);
		break;
	}

	if (m_nBHAlarmCode > 0)		//!= 0)		//v4.37T13
	{
		SetErrorMessage(szErr);
	}
	
	m_nBHLastAlarmCode = m_nBHAlarmCode;
	
	if (bReset == TRUE)
	{
		m_nBHAlarmCode = 0;						//Reset after CODE checking
		m_lBHStateCode	= 0;
	}
		
	return bReset;
}

BOOL CBondHead::OpSetMS100AlarmCode(CONST INT nCode)
{
	CString szErr;

	CString szMsg;
	szMsg.Format("BH - Alarm code %d to %d ", m_nBHAlarmCode, nCode);
	if( nCode==0 )
	{
		m_lBHStateCode = 0;
	}
	else
	{
		m_lBHStateCode = nCode;	//	pow(2, );
	}
	switch (nCode)
	{
		case 0:			//OK
			if (m_nBHAlarmCode != 0)
			{
				szErr.Format("BH Alarm: RESET from (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			m_nBHAlarmCode = nCode;
			return TRUE;

		case -1:		//MD Retry warning
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to MD Retry warning");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to MD Retry warning fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case -2:		//ChangeGrade No Pick Die on PICK side
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to ChangeGrade NoPickDie warning");
				//SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("ERROR: BH Alarm code changed to ChangeGrade NoPickDie warning (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case -3:		//NextBinFull No Pick Die on PICK side
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to NextBinFull NoPickDie warning");
				//SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("ERROR: BH Alarm code changed to NextBinFull NoPickDie warning (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case -4:		//postbond warning Next Cycle Stop
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				//szErr	= _T("BH Alarm code changed to postbond failure NextCycle stop warning");
				szErr.Format("BH Alarm code changed to postbond failure NextCycle stop warning (CycleCnt = %lu)", m_ulCycleCount);
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("ERROR: BH Alarm code changed to postbond failure NextCycle stop warning (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;


			//ERROR
		case 1:			//Low pressure
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine low in pressure");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine low in pressure fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 2:			//Machine cover open
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine Cover Error");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine Cover Error fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 3:			//CJ
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to CJ");
			}
			else
			{
				szErr.Format("BH Alarm code changed to CJ fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 5:			//Wafer-END
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to WAFER-END");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to WAFER-END fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 6:			//BPR post-bond failure
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr.Format("BH Alarm code changed to BPR postbond failure = %d", m_lPrevDieSPCResult); // lSPCResult);
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to BPR postbond failure = %d fail (%d)", m_lPrevDieSPCResult, m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;
	
		case 7:			//Thermal Alarm
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine thermal Check failure");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine thermal Check failure fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 8:			//BHZ FAN stop Alarm
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BHZ FAN ERROR");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BHZ FAN ERROR (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 9:			//BHT UP FAN stop Alarm
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BHT UP FAN ERROR");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BHT UP FAN ERROR (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 10:		//BHT LOW FAN stop Alarm
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BHT LOW FAN ERROR");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BHT LOW FAN ERROR (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 11:			//BH2 Low pressure
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine BH2 low in pressure");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine BH2 low in pressure fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 12:			//EJ Low pressure
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine EJ low in pressure");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine EJ low in pressure fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 13:			//Input Ionizer
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine Input Ionizer Error Bit");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine Input Ionizer Error Bit fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 14:			//Output Ionizer
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine Output Ionizer Error Bit");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine Output Ionizer Error Bit fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 15:			//BH2 Low pressure
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine BH1 low in pressure");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine BH1 low in pressure fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;

		case 16:	//	vacuum flow low
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr	= _T("BH Alarm code changed to Machine low in vacuum flow");
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to Machine low in vacuum flow fail (%d)", m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}

		default:
			if (m_nBHAlarmCode == 0)
			{
				m_nBHAlarmCode = nCode;
				szErr.Format("BH Alarm code changed to: Alarm Code = %d", nCode);
				SetErrorMessage(szErr);
			}
			else
			{
				szErr.Format("BH Alarm code changed to: Alarm Code = %d fail (%d)", nCode, m_nBHAlarmCode);
				SetErrorMessage(szErr);
			}
			break;
	}

	szMsg = szMsg + szErr;
	DisplaySequence(szMsg);

	return TRUE;
}

BOOL CBondHead::OpCheckAlarmCodeOKToPick(BOOL bUseLastCode)
{
	BOOL bPick = TRUE;

	if (bUseLastCode)
	{
		switch (m_nBHLastAlarmCode)
		{
			case 0:		//OK
			case -1:	//MD Warning
				bPick = TRUE;
				break;
			default:
				bPick = FALSE;
				break;
		}
	}
	else
	{
		switch (m_nBHAlarmCode)
		{
			case 0:		//OK
			case -1:	//MD Warning
				bPick = TRUE;
				break;
			default:
				bPick = FALSE;
				break;
		}
	}

	return bPick;
}
	
BOOL CBondHead::OpIsNextDieChangeGrade()	//	wrong sort bug fix
{
	BOOL bMultiGradeSortToSingleBin = (BOOL)(LONG)(*m_psmfSRam)["Wafer Table Options"]["MultiGradeSortToSingleBin"];
	if (bMultiGradeSortToSingleBin)		//No need to chagne grade if this option is enabled, because all grades to Grade-1 bin
	{
		return FALSE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (pUtl->GetPrescanAreaPickMode())
	{
		return FALSE;
	}

	//current may be same grade as it is check or align, not pick such as reference die or step stone.
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction = (WAF_CDieSelectionAlgorithm::WAF_EDieAction)(LONG)(*m_psmfSRam)["WaferTable"]["Current"]["MoveAction"];
	if( eAction!=WAF_CDieSelectionAlgorithm::PICK )
	{
		return FALSE;
	}

	ULONG ulRow		= (*m_psmfSRam)["WaferTable"]["Current"]["MoveRow"];
	ULONG ulCol		= (*m_psmfSRam)["WaferTable"]["Current"]["MoveCol"];
	UCHAR ucGrade	= (*m_psmfSRam)["WaferTable"]["Current"]["MoveGrade"];
	//if (CMS896AStn::m_bMS100OriginalBHSequence == TRUE)
	//{
	//	return FALSE;
	//}	// HuaMao green light idle

	BOOL bIsChangeGrade = FALSE;

	if( m_bLastMapChecked )
	{
		if ((ulRow != m_ulLastMapRow) || (ulCol != m_ulLastMapCol))
		{
			if (ucGrade != m_ucLastMapGrade)
			{
				bIsChangeGrade = TRUE;		//Change-Grade for current PICK die !!!!!
			}
		}
	}

	//Update current die info into BH sequence
	m_ulLastMapRow		= ulRow;
	m_ulLastMapCol		= ulCol;
	m_ucLastMapGrade	= ucGrade;
	m_bLastMapChecked	= TRUE;

	return bIsChangeGrade;
}


BOOL CBondHead::OpIsNextDieBinFull()		//v4.21T2	//Cree HuiZhou
{
	BOOL bNextBinFUll = (BOOL)(LONG)((*m_psmfSRam)["BinTable"]["NextBinFull"]);
	if (!bNextBinFUll)
	{
		return FALSE;
	}
	BOOL bEnableOsramBinMixMap = FALSE;
	bEnableOsramBinMixMap = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Enable Osram Bin Mix Map"];
	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);
	if (bEnableOsramBinMixMap == FALSE && CMS896AApp::m_bEnableSubBin == FALSE)	//v4.41T10
	{
		if (bNextBinFUll)
		{
			(*m_psmfSRam)["BinTable"]["NextBinFull"] = FALSE;
		}
		return FALSE;
	}
	if (CMS896AApp::m_bEnableSubBin)
	{
		m_lSubBinGrade ++;
		m_bSubBinFull = TRUE;
		CString szMsg;
		szMsg.Format("SubBin Change to,%d",m_lSubBinGrade);
		DisplaySequence(szMsg);
	}
	//SetErrorMessage("BH: NextBinFull NoPickDie (andrew)");
	//CMSLogFileUtility::Instance()->MS_LogCtmOperation("BH: NextBinFull NoPickDie");
	(*m_psmfSRam)["BinTable"]["NextBinFull"] = FALSE;
	return TRUE;
}


BOOL CBondHead::OpIsBinTableLevelAtDnLevel()	//v4.22T8	//Walsin China 
{
	if (!IsBLEnable())
	{
		return TRUE;
	}
	if (m_bDisableBL)
	{
		return TRUE;
	}

	BOOL bBT1FrameLevel = TRUE;
	BOOL bBT2FrameLevel = TRUE;						
	LONG lNewBTInUse = (*m_psmfSRam)["BinTable"]["BTInUse"];

	if ((CMS896AApp::m_bMS100Plus9InchOption) && (lNewBTInUse != 0))
	{
		if (!IsBT2FrameLevel())		//BT2 At UP level!!!
		{
			Sleep(500);
			if (!IsBT2FrameLevel())	//BT2 still At UP level!!!
			{
				SetErrorMessage("DBH: BT2 platform detected at UP level when BH moves from PREPICK");
				SetAlert_Red_Yellow(IDS_BH_BT2_LEVEL_AT_UP);
				return FALSE;
			}
		}
	}
	else
	{
		if (!IsBT1FrameLevel())		//BT1 At UP level!!!
		{
			Sleep(500);
			if (!IsBT1FrameLevel())	//BT1 still At UP level!!!
			{
				SetErrorMessage("DBH: BT1 platform detected at UP level when BH moves from PREPICK");
				SetAlert_Red_Yellow(IDS_BH_BT1_LEVEL_AT_UP);
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CBondHead::OpIsLumiledsSpecialMDCheck()		//v4.54A7
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Lumileds")
		return FALSE;
	if (m_lArmPickDelay < 80)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CBondHead::OpLumiledsSpecialMDCheck()
{
	CString szMsg;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != "Lumileds")
		return TRUE;

	szMsg.Format("MD check at PICK (PLLM) start - BH2 = %d, ArmPickDelay = %ld",  
						IsBHZ2ToPick(), m_lArmPickDelay);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	if (m_lArmPickDelay < 80)
	{
		return TRUE;
	}

	//v4.54A8
	if (IsBHZ1ToPick())
	{
		Z_Sync();
	}
	else
	{
		Z2_Sync();
	}
	Sleep(200);

	INT nRetryCount = 0;
	LONG lRetryLimit = 5;//m_lMDCycleLimit;

	while (OpCheckMissingDie_PLLM())
	{
		nRetryCount++;
		if (nRetryCount > lRetryLimit)
		{
			szMsg.Format("MD (PLLM) check timeout; BH = %d", IsBHZ2ToPick()); 
CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			return FALSE;
		}


		szMsg.Format("MD Retry #%d (PLLM) ....", nRetryCount);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
//HmiMessage(szMsg);


		//1. Move BHZ down and try again
		LONG lCompen_Z = OpCalCompen_Z(IsBHZ2ToPick());	
		if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
		{
			AC_Z1P_MoveToOrSearch(m_lPickLevel_Z + lCompen_Z, SFM_WAIT);
		}
		else
		{
			AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2 + lCompen_Z, SFM_WAIT);
		}

		//2. Move EJ UP
		LONG lEjectorLvl = m_lEjectLevel_Ej;
		LONG lCompen_Ej = OpCalCompen_Ej();
		Ej_MoveTo(lEjectorLvl + lCompen_Ej, SFM_NOWAIT);

		//3. Wait for PICK-DELAY)
		Sleep(200);

		//4. MOVE EJ DOWN & BHZ UP
		if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
		{
			AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_NOWAIT);
		}
		else
		{
			AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_NOWAIT);	
		}

		Ej_MoveTo(m_lStandbyLevel_Ej, SFM_WAIT);

		if (IsBHZ1ToPick())			//If BHZ1 towards PICK	
		{
			Z_Sync();
		}
		else
		{
			Z2_Sync();
		}

		Sleep(200);
	}

	szMsg.Format("MD (PLLM) check OK; BH = %d", IsBHZ2ToPick()); 
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

	return TRUE;	// means OK -> no MD
}

BOOL CBondHead::OpUpdateColletHoleShiftTestCounters() 	//v4.59A30
{
	//Collet-Hole shift test for dual arm
	//Matt test begin
	if (m_bPreBondAtPick || m_bPostBondAtBond)	
	{
		m_lPreBondAtPickCount++;
	
		if (m_lPreBondAtPickCount == 9)
		{
			if ( m_lPickDelay > 0 )
			{
				Sleep(m_lPickDelay);
			}

			if (m_bMoveFromPrePick)
				(*m_psmfSRam)["BondHead"]["BondFromPrePick"] = TRUE;
			else
				(*m_psmfSRam)["BondHead"]["BondFromPrePick"] = FALSE;

			if (m_bPreBondAtPick)		//v4.47A7
			{
				GetEncoderValue();
				m_lColletHoleEnc_T = m_lEnc_T;
				m_lColletHoleEnc_Z = m_lEnc_Z;
				m_lColletHoleEnc_Z2 = m_lEnc_Z2;

				if (IsBHZ1ToPick())		//If BHZ1 at PICK	
					(*m_psmfSRam)["BondHead"]["ColletTest"]["UseRecord1"] = FALSE;
				else
					(*m_psmfSRam)["BondHead"]["ColletTest"]["UseRecord1"] = TRUE;
				SetBhTReadyForWPR();
			}

			if (m_lBondDelay > 0)
			{
				Sleep(m_lBondDelay);
			}

			if (m_bPostBondAtBond)
			{
				GetEncoderValue();
				m_lColletHoleEnc_T = m_lEnc_T;
				m_lColletHoleEnc_Z = m_lEnc_Z;
				m_lColletHoleEnc_Z2 = m_lEnc_Z2;

				if (IsBHZ1ToPick())		//If BHZ1 at PICK	
					(*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"] = TRUE;
				else
					(*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"] = FALSE;

				SetBhTReadyForBPR();
			}
		}

		if (m_lPreBondAtPickCount == 10)	//BH2 at PICK
		{
			if ( m_lPickDelay > 0 )
			{
				Sleep(m_lPickDelay);
			}

			if (m_bMoveFromPrePick)
				(*m_psmfSRam)["BondHead"]["BondFromPrePick"] = TRUE;
			else
				(*m_psmfSRam)["BondHead"]["BondFromPrePick"] = FALSE;

			if (m_bPreBondAtPick)		//v4.47A7
			{
				GetEncoderValue();
				m_lColletHoleEnc_T = m_lEnc_T;
				m_lColletHoleEnc_Z = m_lEnc_Z;
				m_lColletHoleEnc_Z2 = m_lEnc_Z2;

				if (IsBHZ1ToPick())		//If BHZ1 at PICK	
					(*m_psmfSRam)["BondHead"]["ColletTest"]["UseRecord1"] = FALSE;
				else
					(*m_psmfSRam)["BondHead"]["ColletTest"]["UseRecord1"] = TRUE;
				SetBhTReadyForWPR();
				//Sleep(200);
			}

			if (m_lBondDelay > 0)
			{
				Sleep(m_lBondDelay);
			}

			if (m_bPostBondAtBond)
			{
				GetEncoderValue();
				m_lColletHoleEnc_T = m_lEnc_T;
				m_lColletHoleEnc_Z = m_lEnc_Z;
				m_lColletHoleEnc_Z2 = m_lEnc_Z2;

				if (IsBHZ1ToPick())		//If BHZ1 at PICK	
					(*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"] = TRUE;
				else
					(*m_psmfSRam)["BondHead"]["ColletTest2"]["UseRecord1"] = FALSE;
				SetBhTReadyForBPR();
				//Sleep(500);
			}
		}


		if (m_lPreBondAtPickCount >= 10)
		{
			m_lPreBondAtPickCount = 0;
		}
	}

	return TRUE;
}


BOOL CBondHead::OpCheckValidAccessMode(BOOL bCheckInBondCycle)	//v4.33T1	//SanAn
{
	return TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (bCheckInBondCycle)
	{
		LONG lLowLimit = 100;
		if( pApp->GetCustomerName()=="Semitek" && pApp->GetProductLine()=="ZJG" )
		{
			lLowLimit = 200;
		}
		else if (pApp->GetCustomerName() == "Electech3E(DL)") // v4.51D2 //Electech3E(DL) 11
		{
			lLowLimit = 300;
		}
		else if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			lLowLimit = 2000; //1.08S
		}
		LONG lUpLimit = lLowLimit + 10;
		if (((long)m_ulCycleCount >= lLowLimit) && ((long)m_ulCycleCount < lUpLimit))		//v4.41T6	
		{
			if(pApp->GetCustomerName() == "Electech3E(DL)")
			{
				pApp->GenerateFaultRecoverAlarmFile("Engineer", "In last Engineer file", "Operation is ok"); // v4.51D2 //Electech3E(DL) 12
			}

			return pApp->CheckSanAnProductionMode();				//TRUE=OK; FALSE="must abort AUTOBOND because non-OP not allowed to run machine"
		}
	}
	else
	{
		return pApp->CheckSanAnProductionMode();					//TRUE=OK; FALSE="must abort AUTOBOND because non-OP not allowed to run machine"
	}

	return TRUE;
}
	
BOOL CBondHead::OpCheckStartLotCount()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCheckStartLotAtFirstTime = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["StartLotCheckCount"];
	
	CString szMsg;
	szMsg.Format("Current Count:%d,first:%d, customized count:%d",m_ulCycleCount,bCheckStartLotAtFirstTime, m_ulStartLotCheckCount);
	//CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);	//v4.53A23


	if (/*pApp->m_bStartNewLot == TRUE && */bCheckStartLotAtFirstTime)
	{
		//ULONG ulLowLimit = 200;
		//ULONG ulUpLimit = 210;
		if ( m_ulCycleCount == 0 || m_ulCycleCount == 1 || m_ulCycleCount == 2 || m_ulStartLotCheckCount < 3)
		{
			return TRUE;
		}
		ULONG ulDownLimit = m_ulStartLotCheckCount - 1;

#ifdef NU_MOTION
		ulDownLimit = m_ulStartLotCheckCount - 2;
#endif

		if ((m_ulCycleCount >= ulDownLimit)&& (m_ulCycleCount < (m_ulStartLotCheckCount + 10)))		//v4.41T6	
		{
			SaveBhData();
			(*m_psmfSRam)["MS896A"]["StartLotCheckCount"] = FALSE;
			HmiMessage_Red_Back("Start Lot Check! (Lumileds)", "AUTOBOND");		//v4.59A1
			return FALSE;
		}
	}
	return TRUE;
}
	
BOOL CBondHead::OpCheckNichiaRawMaterialRegistration()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;
	}
	if (pApp->GetProductLine() != "")	//v4.59A34
	{
		return TRUE;
	}

	CString szLog;
	BOOL bStatus = TRUE;
	if (CMS896AStn::m_oNichiaSubSystem.m_szColletType.GetLength() == 0)
	{
		szLog = "Collet Type is not valid: " + CMS896AStn::m_oNichiaSubSystem.m_szColletType;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		bStatus = FALSE;
		//return FALSE;
	}
	if (CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type.GetLength() == 0)
	{
		szLog = "Collet2 Type is not valid: " + CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		bStatus = FALSE;
		//return FALSE;
	}
	if (CMS896AStn::m_oNichiaSubSystem.m_szNeedleType.GetLength() == 0)
	{
		szLog = "Needle Type is not valid: " + CMS896AStn::m_oNichiaSubSystem.m_szNeedleType;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		bStatus = FALSE;
		//return FALSE;
	}

	return bStatus;
}

BOOL CBondHead::CheckNichiaCriteriaInLoadMap()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;
	}
	if (pApp->GetProductLine() != "")	//v4.59A34
	{
		return TRUE;
	}

	BOOL bStatus = IsMaxColletEjector(TRUE);
	if (!bStatus)
	{
		if (m_WaferMapWrapper.IsMapValid())
			m_WaferMapWrapper.InitMap();
	}
	return bStatus;
}

BOOL CBondHead::OpUpdateNichiaRawMaterialRecordsAtWaferEnd()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;
	}
	if (pApp->GetProductLine() != "")	//v4.59A34
	{
		return TRUE;
	}

	CString szLog;

	BOOL bTool1 = CMS896AStn::m_oNichiaSubSystem.AddRawMaterialUsageRecord(CMS896AStn::m_oNichiaSubSystem.m_szColletType,	m_ulColletCount);
	BOOL bTool2	= CMS896AStn::m_oNichiaSubSystem.AddRawMaterialUsageRecord(CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type,	m_ulCollet2Count);
	BOOL bTool3 = CMS896AStn::m_oNichiaSubSystem.AddRawMaterialUsageRecord(CMS896AStn::m_oNichiaSubSystem.m_szNeedleType,	m_ulEjectorCount);

	//v4.40T5	//Nichia Machine Log generation
	CalculateNichiaWaferEndStat();
	CMS896AStn::m_oNichiaSubSystem.UpdateMachineLog_Time(9);	//SORT_END	//v4.40T5
	//BOOL bMachineLog = CMS896AStn::m_oNichiaSubSystem.WriteMachineLog(TRUE);	//v4.40T6
	//if (!bMachineLog)
	//{
	//	SetErrorMessage("Nichia Machine Log: " + CMS896AStn::m_oNichiaSubSystem.GetLastError());
	//	HmiMessage_Red_Yellow("Nichia Machine Log: " + CMS896AStn::m_oNichiaSubSystem.GetLastError());
	//}

	BOOL bTool = IsMaxColletEjector(TRUE);

	BOOL bCollet1 = CMS896AStn::m_oNichiaSubSystem.CheckRawMaterialExpireDate(CMS896AStn::m_oNichiaSubSystem.m_szColletType);
	if (!bCollet1)
	{
		szLog = "Collet Type has expired: ID = " + CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		return FALSE;
	}

	BOOL bCollet2 = CMS896AStn::m_oNichiaSubSystem.CheckRawMaterialExpireDate(CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type);
	if (!bCollet2)
	{
		szLog = "Collet2 Type has expired: ID = " + CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		return FALSE;
	}

	BOOL bNeedle  = CMS896AStn::m_oNichiaSubSystem.CheckRawMaterialExpireDate(CMS896AStn::m_oNichiaSubSystem.m_szNeedleType);
	if (!bNeedle)
	{
		szLog = "Needle Type has expired: ID = " + CMS896AStn::m_oNichiaSubSystem.m_szNeedleType;
		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::CalculateNichiaWaferEndStat()
{
	ULONG ulTotalMapDices=0, ulTotalDicesForSort=0, ulTotalSortedDices=0;
	CString szTotalMapDices, szTotalDicesForSort, szTotalSortedDices;
	ULONG ulLeft=0, ulPicked=0, ulTotal=0;
	
	if (!m_WaferMapWrapper.IsMapValid())
	{
		return FALSE;
	}

	CUIntArray unaAvaGradeList;
	m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);

	for (INT i = 0; i < unaAvaGradeList.GetSize(); i++)
	{
		m_WaferMapWrapper.GetStatistics(unaAvaGradeList.GetAt(i), ulLeft, ulPicked, ulTotal);
		
		ulTotalMapDices			+= ulTotal;
		//ulTotalDicesForSort		+= ulTotal;
		ulTotalSortedDices		+= ulPicked;
	}

	szTotalMapDices.Format	  ("%d", ulTotalMapDices);
	szTotalDicesForSort.Format("%d", ulTotalMapDices);
	szTotalSortedDices.Format ("%d", ulTotalSortedDices);

	//Nichia Machine Log statisitics Data retrieved here
	(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdMapDices"]		= szTotalMapDices;
	(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGdDicesForSort"]	= szTotalDicesForSort;
	(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfSortedDices"]		= szTotalSortedDices;
	CString szNoOfGrades;
	szNoOfGrades.Format("%d", unaAvaGradeList.GetSize());
	(*m_psmfSRam)["Nichia"]["MC Log"]["ulNoOfGrades"]			= szNoOfGrades;

	return TRUE;
}

BOOL CBondHead::ValidateNichiaMapInLoadingPPKG()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_NICHIA)
	{
		return TRUE;		//this fcn only available for Nichia
	}
	if (pApp->GetProductLine() != "")	//v4.59A34
	{
		return TRUE;
	}

	if (!m_WaferMapWrapper.IsMapValid())
	{
		return TRUE;		//No need to check if map not yet loaded
	}

//AfxMessageBox("BH: ValidateNichiaMapInLoadingPPKG ...", MB_SYSTEMMODAL);
	BOOL bStatus = TRUE;
	LONG lCurrCount = 0, lLimit = 0, lExpDays = 0;
	CString szLog;

	bStatus = CMS896AStn::m_oNichiaSubSystem.RegisterRawMaterial(0, 
								CMS896AStn::m_oNichiaSubSystem.m_szColletType, 
								lCurrCount, lLimit, lExpDays); 
	if (!bStatus)
	{
		szLog = "ERROR: ID=" + CMS896AStn::m_oNichiaSubSystem.m_szColletType + " - " + CMS896AStn::m_oNichiaSubSystem.GetLastError();
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		CMS896AStn::m_oNichiaSubSystem.m_szColletType = _T("");
	}

	bStatus = CMS896AStn::m_oNichiaSubSystem.RegisterRawMaterial(1, 
								CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type, 
								lCurrCount, lLimit, lExpDays); 
	if (!bStatus)
	{
		szLog = "ERROR: ID=" + CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type + " - " + CMS896AStn::m_oNichiaSubSystem.GetLastError();
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		CMS896AStn::m_oNichiaSubSystem.m_szCollet2Type = _T("");
	}

	bStatus = CMS896AStn::m_oNichiaSubSystem.RegisterRawMaterial(2, 
								CMS896AStn::m_oNichiaSubSystem.m_szNeedleType, 
								lCurrCount, lLimit, lExpDays); 
	if (!bStatus)
	{
		szLog = "ERROR: ID=" + CMS896AStn::m_oNichiaSubSystem.m_szNeedleType + " - " + CMS896AStn::m_oNichiaSubSystem.GetLastError();
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		CMS896AStn::m_oNichiaSubSystem.m_szNeedleType = _T("");
	}


	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bStatus);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ValidateNichiaArrCodeInMap", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);	
		}
	}

	return TRUE;
}

BOOL CBondHead::OpMoveEjectorTableXY(BOOL bStart, INT nMode)
{
	if (!m_bMS100EjtXY)
	{
		return TRUE;
	}

	//v4.46T10	//Semitek
	if (!EjX_IsPowerOn())
	{
		SetErrorMessage("Ejt X loses power 1, HOME");
		EjX_Home();
		Sleep(200);
		if (!EjX_IsPowerOn())
		{
			SetErrorMessage("Ejt X loses power 2; fail");
			return FALSE;
		}
	}

	//v4.46T10	//Semitek
	if (!EjY_IsPowerOn())
	{
		SetErrorMessage("Ejt Y loses power 1, HOME");
		EjY_Home();
		Sleep(200);
		if (!EjY_IsPowerOn())
		{
			SetErrorMessage("Ejt Y loses power 2; fail");
			return FALSE;
		}
	}

	LONG lEjtX = m_lCDiePos_EjX;
	LONG lEjtY = m_lCDiePos_EjY;

	if (!m_bEnableMS100EjtXY)
	{
		if (bStart)
		{
			EjX_MoveTo(lEjtX, SFM_NOWAIT);
			EjY_MoveTo(lEjtY, SFM_WAIT);
			EjX_Sync();
		}
		
		if (!EjX_IsPowerOn() || !EjY_IsPowerOn())	//v4.46T10
		{
			return FALSE;
		}
		return TRUE;
	}

	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetEjtColletOffsetCount(lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dEjtXYRes, TRUE, IsBHZ2ToPick() ? TRUE : FALSE, 8);

	CString szLog, szTemp;
	if (bStart)
	{
		szLog.Format("BH - MoveEJTT start: XY (%ld, %ld); C1Offset (%ld, %ld)",
			lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY);

		lEjtX = lEjtX + lCollet1OffsetX;
		lEjtY = lEjtY + lCollet1OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_WAIT);
		EjX_Sync();
	}
	else if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
	{
		szLog.Format("BH - MoveEJTT to BHZ1: XY (%ld, %ld); C1Offset (%ld, %ld)",
			lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY);

		lEjtX = lEjtX + lCollet1OffsetX;
		lEjtY = lEjtY + lCollet1OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_NOWAIT);
		SaveBHMark1(szLog);
	}
	else
	{
		szLog.Format("BH - MoveEJTT to BHZ2: XY (%ld, %ld); C2Offset (%ld, %ld)",
			lEjtX, lEjtY, lCollet2OffsetX, lCollet2OffsetY);
		lEjtX = lEjtX + lCollet2OffsetX;
		lEjtY = lEjtY + lCollet2OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_NOWAIT);
		SaveBHMark2(szLog);
	}
	szTemp.Format(" at last (%ld, %ld)", lEjtX, lEjtY);
	szLog += szTemp;
	DisplaySequence(szLog);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);

	if (nMode == SFM_WAIT)
	{
		EjX_Sync();
		EjY_Sync();

		if (!EjX_IsPowerOn() || !EjY_IsPowerOn())	//v4.46T10
		{
			SetErrorMessage("Ejt XY loses power after MOVE; fail");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBondHead::OpSwitchEjectorTableXY(BOOL bBH1, INT nMode)
{
	if (!EjX_IsPowerOn())
	{
		SetErrorMessage("Ejt X loses power 1, HOME");
		EjX_Home();
		Sleep(200);
		if (!EjX_IsPowerOn())
		{
			SetErrorMessage("Ejt X loses power 2; fail");
			return FALSE;
		}
	}

	if (!EjY_IsPowerOn())
	{
		SetErrorMessage("Ejt Y loses power 1, HOME");
		EjY_Home();
		Sleep(200);
		if (!EjY_IsPowerOn())
		{
			SetErrorMessage("Ejt Y loses power 2; fail");
			return FALSE;
		}
	}

	LONG lEjtX = m_lCDiePos_EjX;
	LONG lEjtY = m_lCDiePos_EjY;

	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetEjtColletOffsetCount(lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dEjtXYRes, TRUE, IsBHZ2ToPick() ? TRUE : FALSE, 8);

	CString szLog, szTemp;
	if (bBH1)		//If BHZ1 towards PICK	
	{
		szLog.Format("BH - SwitchEJTT to BHZ1: XY (%ld, %ld); C1Offset (%ld, %ld)",
			lEjtX, lEjtY, lCollet1OffsetX, lCollet1OffsetY);

		lEjtX = lEjtX + lCollet1OffsetX;
		lEjtY = lEjtY + lCollet1OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_NOWAIT);
		SaveBHMark1(szLog);
	}
	else
	{
		szLog.Format("BH - SwitchEJTT to BHZ2: XY (%ld, %ld); C2Offset (%ld, %ld)",
			lEjtX, lEjtY, lCollet2OffsetX, lCollet2OffsetY);
		lEjtX = lEjtX + lCollet2OffsetX;
		lEjtY = lEjtY + lCollet2OffsetY;
		EjX_MoveTo(lEjtX, SFM_NOWAIT);
		EjY_MoveTo(lEjtY, SFM_NOWAIT);
		SaveBHMark2(szLog);
	}
	szTemp.Format(" at last (%ld, %ld)", lEjtX, lEjtY);
	szLog += szTemp;
	DisplaySequence(szLog);
	//CMSLogFileUtility::Instance()->WT_GetIdxLog(szLog);

	if (nMode == SFM_WAIT)
	{
		EjX_Sync();
		EjY_Sync();

		if (!EjX_IsPowerOn() || !EjY_IsPowerOn())	//v4.46T10
		{
			SetErrorMessage("Ejt XY loses power after MOVE; fail");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CBondHead::OpSyncEjectorTableXY()
{
	if (!m_bMS100EjtXY)
		return TRUE;
	if (!EjX_IsPowerOn() || !EjY_IsPowerOn())
	{
		return FALSE;
	}

	EjX_Sync();
	EjY_Sync();
	return TRUE;
}

BOOL CBondHead::OpCheckEjtXYBHPos(BOOL bIsBH1ToPick)	//v4.46T10	//SEmitek
{
	if (!m_bMS100EjtXY)
		return TRUE;
	if (!m_bEnableMS100EjtXY)
		return TRUE;
	if (IsBurnIn())			//v4.47T5
		return TRUE;

	CString szErr;
	if (bIsBH1ToPick && (m_nWTAtColletPos != 1))
	{
		szErr.Format("DBH: ejector table Collet position error!  BH_to_PICK = 1, EJT_CPos = %d", m_nWTAtColletPos);
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->BPR_Arm1Log(szErr);	
		DisplaySequence(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	if (!bIsBH1ToPick && (m_nWTAtColletPos != 2))
	{
		szErr.Format("DBH: ejector table Collet position error!  BH_to_PICK = 2, EJT_CPos = %d", m_nWTAtColletPos);
		SetErrorMessage(szErr);
		CMSLogFileUtility::Instance()->BPR_Arm1Log(szErr);	
		DisplaySequence(szErr);
		HmiMessage_Red_Yellow(szErr);
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::OpCheckMS60BHFanOK()
{
	if (m_bCoolingFanWillTurnOff)
		return TRUE;

	if (!m_bMS60DetectBHFan)	//v4.48A33
		return TRUE;			//v4.48A16		//Temporarily disabled for T version; available for v4.48

	LONG lBit = CMS896AStn::MotionReadInputBit("BHZ_FAN_111");

	if (lBit != 0)	//0 = OK, 1=Fan OFF!!
	{
		CString szErr;
		szErr.Format("MS60 BH Fan signal error - %d", lBit);
		SetErrorMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::OpCheckPostBondEmptyCountToIncBHZ()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName().Find(CTM_SANAN) == -1)
		return TRUE;

	BOOL bEmpty1 = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit1"];
	BOOL bEmpty2 = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit2"];
	CString szMsg;

	LONG lOffsetZ = m_lBHZOffsetbyPBEmptyCheck;


	//v4.50A4	//SanAn	//Smart PB EMPTY limit reset method after AutoCleanColletNow
	if (m_bEnableAutoCCNowCounter1 || m_bEnableAutoCCNowCounter2)
	{
		m_lAutoCCNowCounter++;

		szMsg.Format("PB EMPTY-DIE Reset count (AUTO-CLEAN) - %ld",  m_lAutoCCNowCounter);
		CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);

		if (m_lAutoCCNowCounter > 100)
		{
			if (m_bEnableAutoCCNowCounter1)
			{
				(*m_psmfSRam)["BondPr"]["PostBond"]["ResetEmptyLimit1"]	= TRUE;
				m_bEnableAutoCCNowCounter1 = FALSE;
				szMsg = "PB EMPTY-Die limit 1 reset triggered after AUTO-CLEAN";
			}
			else
			{
				(*m_psmfSRam)["BondPr"]["PostBond"]["ResetEmptyLimit2"] = TRUE;
				m_bEnableAutoCCNowCounter2 = FALSE;
				szMsg = "PB EMPTY-Die limit 2 reset triggered after AUTO-CLEAN";
			}

			m_lAutoCCNowCounter = 0;
			CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);
		}
	}


	if (bEmpty1 && (lOffsetZ > 0))
	{
		(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit1"] = FALSE;

		m_bAutoCleanColletNow1 = TRUE;	//v4.50A4
		m_bEnableAutoCCNowCounter1 = FALSE;
		m_lAutoCCNowCounter	= 0;
		szMsg = "PB EMPTY-Die BHZ1 near limit triggers AUTO-CLEAN-Collet NOW";
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);
		
		//SaveBhData();
	}

	if (bEmpty2 && (lOffsetZ > 0))
	{
		(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit2"] = FALSE;
		
		m_bAutoCleanColletNow2 = TRUE;	//v4.50A4
		m_bEnableAutoCCNowCounter2 = FALSE;
		m_lAutoCCNowCounter	= 0;

		szMsg = "PB EMPTY-Die BHZ2 near limit triggers AUTO-CLEAN-Collet NOW";
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->BH_LogStatus(szMsg);

		//SaveBhData();
	}

	return TRUE;
}


BOOL CBondHead::OpAutoLearnBHZPickBondLevel(BOOL bBHZ2, BOOL bLearnGelPad)	//After AutoCleanCollet	//v4.49A6
{
	if (!m_bAutoCleanAutoLearnZLevels/* && !bLearnGelPad*/)
	{
		SetErrorMessage("Op AutoLearnBHZPickBondLevel: fcn is disabled");
		return TRUE;
	}

	if (!IsAllMotorsEnable())
	{
		SetErrorMessage("Op AutoLearnBHZPickBondLevel: fcn is disabled because of Motor OFF");
		return FALSE;
	}

	if (m_bDisableBH)
	{
		SetErrorMessage("Op AutoLearnBHZPickBondLevel: fcn is disabled because m_bDisableBH");
		return TRUE;
	}

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Op AutoLearnBHZPickBondLevel: Wafer Expander not closed");
		return FALSE;
	}

	//Extra protection to protect against BH collision/safety
	INT nCount = 0;
	while (IsCoverOpen())
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		SetErrorMessage("Machine cover open detected in Op AutoLearnBHZPickBondLevel");

		nCount++;
		if (nCount >= 3)
		{
			LONG lHmiStatus = HmiMessage_Red_Yellow("Cover sensor still open in CHANGE BH setup; continue to move T motor?", 
										 "BondHead Setup", glHMI_MBX_CONTINUESTOP | 0x80000000);
			if (lHmiStatus != glHMI_CONTINUE)
			{
				return FALSE;
			}
		}
	}

	BOOL bStatus = TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (IsBLEnable() && !IsBT1FrameLevel())
	{
		SetAlert_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN);
		SetErrorMessage("BT frame level not DOWN");
		return FALSE;
	}

	//HmiMessage_Red_Back("BH: AutoLearn Z levels after AutoCleanCollet starts ...");
	CMSLogFileUtility::Instance()->MS_LogOperation("BH: AutoLearn BOND Z after AUTO_CLEAN_COLLET start ...");

	LONG lValue = 0;
	if (m_bMS100DigitalAirFlowSnr/* && !bLearnGelPad*/)		//v4.50A3
	{
		if (bBHZ2)
		{
			SetPickVacuumZ2(FALSE);
			SetStrongBlowZ2(TRUE);
		}
		else
		{
			SetPickVacuum(FALSE);
			SetStrongBlow(TRUE);
		}
	}

	MoveToChangeColletSafePosn();

	if (m_bMS100DigitalAirFlowSnr/* && !bLearnGelPad*/)		//v4.50A3
	{
		Sleep(500);	
		if (bBHZ2)
		{
			SetStrongBlowZ2(FALSE);
			Sleep(500);
			m_ulBHZ2DAirFlowSetZeroOffset = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		}
		else
		{
			SetStrongBlow(FALSE);
			Sleep(500);
			m_ulBHZ1DAirFlowSetZeroOffset = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		}
	}

	if (bBHZ2)
	{
		SetPickVacuumZ2(TRUE);
	}
	else
	{
		SetPickVacuum(TRUE);
	}

	if (m_bMS100DigitalAirFlowSnr/* && !bLearnGelPad*/)		//v4.50A3
	{
		Sleep(100);
#define DT_LOOP_LMT		100
		//Take the un-clogged reading
		ULONG ulInitValue[41];
		INT iLoopBreak = 0;
		//3.18 leo request
		ULONG ulMinInitValue = 0;
		ULONG ulMinValueTemp = 0;

		for (int i = 1; i < 40; i ++)
		{
			Sleep(10);
			if (bBHZ2)
			{
				ulInitValue[i] = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
				ulInitValue[i] = ulInitValue[i] - m_ulBHZ2DAirFlowSetZeroOffset;
				if (ulInitValue[i] < 0)
					ulInitValue[i] = 0;

				ulMinValueTemp = ulInitValue[i];
				if (i != 1)
				{
					if (ulInitValue[i] < ulMinInitValue)
					{
						ulMinInitValue = ulInitValue[i];
					}
				}
				else
				{
					ulMinInitValue = ulInitValue[1];
				}
			}
			else
			{
				ulInitValue[i] = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
				ulInitValue[i] = ulInitValue[i] - m_ulBHZ1DAirFlowSetZeroOffset;
				if (ulInitValue[i] < 0)
					ulInitValue[i] = 0;

				ulMinValueTemp = ulInitValue[i];
				if (i != 1)
				{
					if (ulInitValue[i] < ulMinInitValue)
					{
						ulMinInitValue = ulInitValue[i];
					}
				}
				else
				{
					ulMinInitValue = ulInitValue[1];
				}
			}
		}

		BOOL bFindInitValue = FALSE;
		ULONG ulUnBlockValue = 0;
		if (ulInitValue[1] != ulMinInitValue && ulInitValue [39] != ulMinInitValue)
		{
			ulUnBlockValue = ulMinInitValue;
			bFindInitValue = TRUE;
		}

		CString szInitMsg, szInitTemp;
		szInitMsg.Format("Op AutoLearnBHZPickBondLevel - min:%d,DAirFlow UNBLOCK Initvalues = %lu (",
						ulMinInitValue,ulUnBlockValue);
		for(INT i=1; i<40; i++)
		{
			szInitTemp.Format("%lu, ", ulInitValue[i]);
			szInitMsg += szInitTemp;
		}

		CMSLogFileUtility::Instance()->MS_LogOperation(szInitMsg);
		ULONG ulValue[DT_LOOP_LMT];

		for (INT i=0; i<DT_LOOP_LMT; i++)
		{
			ulValue[i] = 0;
		}

		if (bFindInitValue == FALSE && ulUnBlockValue == 0)
		{
			for (INT i=0; i<DT_LOOP_LMT; i++)
			{
				Sleep(200);

				if (bBHZ2)
				{
					ulValue[i] = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
					ulValue[i] = ulValue[i] - m_ulBHZ2DAirFlowSetZeroOffset;
					if (ulValue[i] < 0)
						ulValue[i] = 0;
				}
				else
				{
					ulValue[i] = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
					ulValue[i] = ulValue[i] - m_ulBHZ1DAirFlowSetZeroOffset;
					if (ulValue[i] < 0)
						ulValue[i] = 0;
				}

				if( i>0 )
				{
					if( labs(ulValue[i]-ulValue[i-1])<=40 )
					{
						iLoopBreak = i;
						break;
					}
				}
			}

			ulUnBlockValue = ulValue[0];
			for (INT i=1; i<=iLoopBreak; i++)
			{
				if (ulValue[i] < ulUnBlockValue)
				{
					ulUnBlockValue = ulValue[i];
				}
			}
		}

		if (bBHZ2)
		{
			m_ulBHZ2DAirFlowUnBlockValue	= (ULONG) ulUnBlockValue;
		}
		else
		{
			m_ulBHZ1DAirFlowUnBlockValue	= (ULONG) ulUnBlockValue;
		}

		CString szMsg, szTemp;
		szMsg.Format("Op AutoLearnBHZPickBondLevel - DAirFlow UNBLOCK values = %lu (",
						ulUnBlockValue);
		for(INT i=0; i<iLoopBreak; i++)
		{
			szTemp.Format("%lu, ", ulValue[i]);
			szMsg += szTemp;
		}
		szTemp.Format("%lu)", ulValue[iLoopBreak]);
		szMsg += szTemp;
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	}
	else
	{
		Sleep(200);
	}

//	if (!bLearnGelPad)		//v4.49A11
	{
		if (!MoveBTForAutoCColletAutoLearnZ(TRUE))
		{
			CString szErr;
			szErr.Format("Fail to move to the position of BT Table's Auto Learn Z");
			HmiMessage_Red_Back(szErr, "Move BT table");
			SetErrorMessage(szErr);
			return FALSE;
		}
	}

	LONG lNewEncZ1	= 0;
	LONG lNewEncZ2	= 0;
	LONG lEncZ		= 0;

	// BT Level tilting, to adjust and change pick level.
	LONG lBTLevelOffset = 0;
	if (m_bUseMultiProbeLevel)	//	auto learn clean collet
	{
		LONG lX = 0;
		LONG lY = 0;
		BT_Get_XY_Posn(lX, lY);
		//	get bin table current position X and Y.
		//lBTLevelOffset = GetBTLevelOffset(IsBHZ1ToPick(), lX, lY);
		lBTLevelOffset = GetBTLevelOffset(bBHZ2, lX, lY);
		//if( IsBHZ1ToPick() )
		if (bBHZ2)
		{
			m_lZ2BondLevelBT_X = lX;
			m_lZ2BondLevelBT_Y = lY;
			m_bZ2BondLevelBT_R = IsMS90BTRotated();
		}
		else
		{
			m_lZ1BondLevelBT_X = lX;
			m_lZ1BondLevelBT_Y = lY;
			m_bZ1BondLevelBT_R = IsMS90BTRotated();
		}
		BH_MBL_LogLevel();
		CString szMsg;
		szMsg.Format("Auto Learn BHZ%d new BT %d,%d offset %d, BT Rotate %d",
			bBHZ2+1, lX, lY, lBTLevelOffset, IsMS90BTRotated());
		SetErrorMessage(szMsg);
	}

	if (bBHZ2)
	{
		//HmiMessage("BHZ2 to BOND ....");

		T_SafeMoveToInAuto(m_lPickPos_T, "Auto Learn BHZ Level");
		lEncZ = m_lBondLevel_Z2 + 1000;	//m_lSwingLevel_Z2;		//v4.49A9	//v4.50A3

		//HmiMessage("BHZ2 auto-learn Z starts ....");

		if (!AutoLearnBHZ2BondLevel(lEncZ) || !Z2_IsPowerOn())
		{
			//Z2_Home();
			Z2_MoveToHome();	//v4.52A11
			SetPickVacuum(FALSE);
			SetPickVacuumZ2(FALSE);
			T_SafeMoveToInAuto(m_lPrePickPos_T, "Auto Learn BHZ Level");
			SetErrorMessage("Op AutoLearnBHZPickBondLevel: Auto Learn Z2 fails");
			if (!Z2_IsPowerOn())	//v4.52A11
			{
				CString szErr = "\nBHZ2 is not power on!";
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			}
			return FALSE;
		}
		
		lNewEncZ2 = lEncZ + m_lBondDriveIn;

		if (m_bMS100DigitalAirFlowSnr/* && !bLearnGelPad*/)		//v4.50A3
		{
			//Take the clogged reading
			Sleep(500);
			lValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
			lValue = lValue - m_ulBHZ2DAirFlowSetZeroOffset;
			if (lValue < 0)
				lValue = 0;
			m_ulBHZ2DAirFlowBlockValue	= (ULONG) lValue;
		}

		Z2_MoveTo(m_lSwingLevel_Z2);
		if ( (pApp->GetBHZ2HomeOffset() > 0) && (pApp->GetBHZ2HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z2_Move(pApp->GetBHZ2HomeOffset());
		}

		Sleep(100);
	}
	else
	{
		//HmiMessage("BHZ1 to BOND ....");

		T_SafeMoveToInAuto(m_lBondPos_T, "Auto Learn BHZ Level");
		lEncZ = m_lBondLevel_Z + 1000;		//m_lSwingLevel_Z;	//v4.50A3

		//HmiMessage("BHZ1 auto-learn Z starts ....");

		if (!AutoLearnBHZ1BondLevel(lEncZ) || !Z_IsPowerOn())
		{
			//Z_Home();
			Z_MoveToHome();		//v4.52A11
			SetPickVacuum(FALSE);
			SetPickVacuumZ2(FALSE);
			T_SafeMoveToInAuto(m_lPrePickPos_T, "Auto Learn BHZ Level");
			SetErrorMessage("Op AutoLearnBHZPickBondLevel: Auto Learn Z fails");
			if (!Z_IsPowerOn())		//v4.52A11
			{
				CString szErr = "\nBHZ is not power on!";
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, szErr);
			}
			return FALSE;
		}
		
		lNewEncZ1 = lEncZ + m_lBondDriveIn;

		if (m_bMS100DigitalAirFlowSnr/* && !bLearnGelPad*/)		//v4.50A3
		{
			//Take the clogged reading
			Sleep(500);
			lValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
			lValue = lValue - m_ulBHZ1DAirFlowSetZeroOffset;
			if (lValue < 0)
				lValue = 0;
			m_ulBHZ1DAirFlowBlockValue	= (ULONG) lValue;
		}

		Z_MoveTo(m_lSwingLevel_Z);
		if ( (pApp->GetBHZ1HomeOffset() > 0) && (pApp->GetBHZ1HomeOffset() <= BH_Z_MAX_HOMEOFFSET) )
		{
			Sleep(100);
			Z_Move(pApp->GetBHZ1HomeOffset());
		}

		Sleep(100);
	}

	T_SafeMoveToInAuto(m_lPrePickPos_T, "Auto Learn BHZ Level");

//	if (!bLearnGelPad)		//v4.49A11
	{
		if (!MoveBTForAutoCColletAutoLearnZ(FALSE))
		{
			CString szErr;
			szErr.Format("Fail to move to the position of BT Table's Auto Learn Z");
			HmiMessage_Red_Back(szErr, "Move BT table");
			SetErrorMessage(szErr);
			return FALSE;
		}
	}

	LONG lCurrPickZ  = m_lPickLevel_Z;
	LONG lCurrPickZ2 = m_lPickLevel_Z2;
	LONG lCurrBondZ  = m_lBondLevel_Z;
	LONG lCurrBondZ2 = m_lBondLevel_Z2;
	LONG lOffsetZ	 = 0;

	CString szMsg;
	ULONG ulThresholdValue = 0;

	if (bBHZ2)
	{
		lOffsetZ = lNewEncZ2 - m_lBondLevel_Z2 - lBTLevelOffset;

		szMsg.Format("Op AutoLearnBHZPickBondLevel - new BONDZ2 = %ld (%ld); BT tilting %d; ZOffset = %ld steps",
						lNewEncZ2, lCurrBondZ2, lBTLevelOffset, lOffsetZ);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);

		//xx check new/old level difference.
		if( /*CheckNewOldLevelOffset(lOffsetZ)==FALSE*/lOffsetZ > 500 )
		{
			CString szErr;
			szErr.Format("Auto-Learn1 Z2 fails because Offset %d > 500 steps!", lOffsetZ);
			HmiMessage_Red_Back(szErr, "Auto Learn 1");
			SetErrorMessage(szErr);
			return FALSE;
		}

		m_lBondLevel_Z2		= lNewEncZ2;
		m_lBPGeneral_C		= m_lBondLevel_Z2;

//		if (!bLearnGelPad)		//v4.49A11
		{
			m_lPickLevel_Z2	= m_lPickLevel_Z2 + lOffsetZ;
			m_lBPGeneral_B	= m_lPickLevel_Z2;
		}
/*
		else
		{
			m_lBondLevel_Z		= m_lBondLevel_Z + lOffsetZ;
			m_lBPGeneral_7		= m_lBondLevel_Z;
		}
*/
		if (m_bMS100DigitalAirFlowSnr /*&& !bLearnGelPad*/)		//v4.50A3
		{
			m_ulBHZ2DAirFlowThreshold = GetBHZ2MissingDieThresholdValue(m_ulBHZ2DAirFlowUnBlockValue);
			//v4.50A7	//Threshold value for CJ
			m_ulBHZ2DAirFlowThresholdCJ = GetBHZ2ColletJamThresholdValue(m_ulBHZ2DAirFlowUnBlockValue);

			bStatus = CheckDigitalAirFlowThresholdLimit(TRUE);

			szMsg.Format("AUTO-Learn DAirFlow BHZ2: Unclog = %lu, Clog = %lu, Pct = %.1f, Thres = %lu, THres(CJ) = %lu, Status = %d",
							m_ulBHZ2DAirFlowUnBlockValue, m_ulBHZ2DAirFlowBlockValue, 
							m_dBHZ1ThresholdPercent, m_ulBHZ2DAirFlowThreshold, m_ulBHZ2DAirFlowThresholdCJ, 
							bStatus);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			//HmiMessage(szMsg);	//v4.50A4
		}

		//xx check new/old and pick/bond level difference.
		CheckZ2PickBondLevelOffset();

		szMsg.Format("Op AutoLearnBHZPickBondLevel BHZ2 - new PICKZ2=%ld, BONDZ2=%ld, BONDZ=%ld",
						m_lPickLevel_Z2, m_lBondLevel_Z2, m_lBondLevel_Z);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage(szMsg);
		//HmiMessage_Red_Back("BH: AutoLearn Z2 levels done.");
	}
	else
	{
		lOffsetZ = lNewEncZ1 - m_lBondLevel_Z - lBTLevelOffset;

		szMsg.Format("Op AutoLearnBHZPickBondLevel - new BONDZ = %ld (%ld); BT tilting %d; ZOffset = %ld steps",
						lNewEncZ1, lCurrBondZ, lBTLevelOffset, lOffsetZ);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//xx check new/old level difference.
		if( /*CheckNewOldLevelOffset(lOffsetZ)==FALSE*/lOffsetZ > 500 )
		{
			CString szErr;
			szErr.Format("Auto-Learn1 Z1 fails because Offset %d > 500 steps!", lOffsetZ);
			HmiMessage_Red_Back(szErr, "Auto Learn 1");
			SetErrorMessage(szErr);
			return FALSE;
		}

		m_lBondLevel_Z		= lNewEncZ1;
		m_lBPGeneral_7		= m_lBondLevel_Z;

//		if (!bLearnGelPad)		//v4.49A11
		{
			m_lPickLevel_Z	= m_lPickLevel_Z + lOffsetZ;
			m_lBPGeneral_6	= m_lPickLevel_Z;
		}
/*
		else
		{
			m_lBondLevel_Z2		= m_lBondLevel_Z2 + lOffsetZ;
			m_lBPGeneral_C		= m_lBondLevel_Z2;
		}
*/
		if (m_bMS100DigitalAirFlowSnr /*&& !bLearnGelPad*/)		//v4.50A3
		{
			m_ulBHZ1DAirFlowThreshold =  GetBHZ1MissingDieThresholdValue(m_ulBHZ1DAirFlowUnBlockValue);
			//v4.50A7	//THreshold value for CJ
			m_ulBHZ1DAirFlowThresholdCJ = GetBHZ1ColletJamThresholdValue(m_ulBHZ1DAirFlowUnBlockValue);

			bStatus = CheckDigitalAirFlowThresholdLimit(FALSE);

			szMsg.Format("AUTO-Learn DAirFlow BHZ1: Unclog = %lu, Clog = %lu, Pct = %.1f, Thres = %lu, Thres(CJ) = %lu, Status = %d",
							m_ulBHZ1DAirFlowUnBlockValue, m_ulBHZ1DAirFlowBlockValue, 
							m_dBHZ1ThresholdPercent, m_ulBHZ1DAirFlowThreshold, m_ulBHZ1DAirFlowThresholdCJ, 
							bStatus);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			//HmiMessage(szMsg);	//v4.50A4
		}

		//xx check new/old and pick/bond level difference.
		CheckZ1PickBondLevelOffset();

		szMsg.Format("Op AutoLearnBHZPickBondLevel BHZ1 - new PICKZ=%ld, BONDZ=%ld, BONDZ2=%ld",
						m_lPickLevel_Z, m_lBondLevel_Z, m_lBondLevel_Z2);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		//HmiMessage(szMsg);
		//HmiMessage_Red_Back("BH: AutoLearn Z1 levels done.");
	}

	//SetBondPadLevel(IsBHZ1ToPick());	//	auto learn level
	SetBondPadLevel(bBHZ2);	//	auto learn level

	SaveBhData();

	if (bBHZ2)
	{
		m_bAutoLearnPickLevelZ2 = TRUE;
		DisplaySequence("BH - **Auto Learn Pick LevelZ2 Next cycle");
		//OpPrestartColletHoleCheck(FALSE, FALSE);
	}
	else
	{
		m_bAutoLearnPickLevelZ1 = TRUE;
		DisplaySequence("BH - **Auto Learn Pick LevelZ1 Next cycle");
		//OpPrestartColletHoleCheck(TRUE, FALSE);
	}
	T_Sync();
	return bStatus;
}

BOOL CBondHead::OpBurnInPreStartBHTThermalValues()
{
	if (!IsMS60())
		return TRUE;
	if (!m_bMS60ThermalCtrl)
		return TRUE;
	if (!m_bEnableMS60ThermalCheck)
		return TRUE;

	//v4.48A26	//Requested by Donald
	m_dID = 0;
	m_nRmsCount = 0;
	m_dRmsRSum = m_dRmsSSum = m_dRmsTSum = 0;
	
	//v4.48A30
	INT nThermalDAC = 0;
	CMS896AStn::MotionSetMotorThermalCompDAC(BH_AXIS_T, nThermalDAC, &m_stBHAxis_T);

	return TRUE;
}

BOOL CBondHead::OpBurnInCalculateBHTThermalValues()
{
	CStdioFile oFile;
	CString szMsg; 
	CString szFileName = gszUSER_DIRECTORY + BPR_ARM2_LOG;

	if (!IsMS60())
		return TRUE;
	if (!m_bMS60ThermalCtrl)
	{
		return TRUE;
	}
	if (!m_bEnableMS60ThermalCheck)
	{
		return TRUE;
	}
	//if (!m_bStop)
	//{
	//	return TRUE;
	//}
	if (!CMSLogFileUtility::Instance()->BPR_IsArm2LogOpen())
	{
		return TRUE;
	}

	CMSLogFileUtility::Instance()->BPR_Arm2LogClose();	//v4.48A10

	BOOL bFileOpen = oFile.Open(szFileName, CFile::modeRead|CFile::typeText);

	if (!bFileOpen)
	{
		HmiMessage("OpBurnInCalculateBHTThermalValues: cannot open file - " + szFileName);
		return FALSE;
	}

	INT nCount = 0;
	CString szLine, szLastLine;
	while (oFile.ReadString(szLine))
	{
		if (szLine.GetLength() > 0)
			szLastLine = szLine;
		nCount++;
	}

	oFile.Close();
	if (nCount < 20)
	{
		szMsg.Format("BHT Thermal Log file with not enough samples = %d (min = 20)", nCount);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg);
		return FALSE;
	}

	szLine = szLastLine;

	CString szIQ = "";
	INT nPos = szLine.Find(",");
	if (nPos != -1)
	{
		szLine = szLine.Mid(nPos+1);
		nPos = szLine.Find(",");
		if (nPos != -1)
		{
			szLine = szLine.Mid(nPos+1);
			nPos = szLine.Find(",");
			if (nPos != -1)
			{
				szIQ = szLine.Left(nPos);
			}
		}
	}

	if (szIQ.GetLength() == 0)
	{
		szMsg = "BHT Thermal Log file cannot find Iq value";
		HmiMessage(szMsg);
		return FALSE;
	}

	DOUBLE dIQ = atof((LPCTSTR) szIQ);

	if ( (dIQ <= 0) || (dIQ > 1) )
	{
		szMsg.Format("BHT Thermal Log: Invalid Iq value logged = %.6f - " + szIQ, dIQ);
		HmiMessage(szMsg);
		return FALSE;
	}

	//v4.49A2
	//m_dIS = dIQ * 1.025;	//? this factor may not be correct as mentioned by Gan

	szMsg.Format("MS60 BHT Thermal Fcn: IS updated to %.6f (Iq found = %.6f)", m_dIS, dIQ);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
//HmiMessage(szMsg);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szValue;
	//szValue.Format("%.6f", m_dIS);
	//pApp->WriteProfileString(gszPROFILE_HW_CONFIG, _T("MS60 Thermal IS"), (LPCTSTR) szValue);

	return TRUE;
}


BOOL CBondHead::OpMissingDieUplookPrFailToStop(CONST LONG lCode)	//v4.59A5
{
	CString szLogMsg;
	if (lCode == 1)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH1 MD alarm start");	//v4.52A10
		SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ1);
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH1 MD alarm end");	//v4.52A10
		szLogMsg = "BH 1 Missing Die Detected";
	}
	else if (lCode == 2)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH2 MD alarm start");	//v4.52A10
		SetAlert_Red_Yellow(IDS_BH_MISSINGDIEDETECTED_BHZ2);
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH2 MD alarm end");	//v4.52A10
		szLogMsg = "BH 2 Missing Die Detected";
	}
	else if (lCode == 3)
	{
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH1 ACCUM MD alarm start");	//v4.52A10
		SetAlert_Red_Yellow(IDS_BH_MD_ACCUM_BHZ1);
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH1 ACCUM MD alarm end");		//v4.52A10
		szLogMsg = "BH 1 Accumulated Missing Die Detected";
	}
	else if (lCode == 4)
	{
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH2 ACCUM MD alarm start");	//v4.52A10
		OpDisplayAlarmPage(TRUE);	//v4.53A13
		SetAlert_Red_Yellow(IDS_BH_MD_ACCUM_BHZ2);
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH2 ACCUM MD alarm end");		//v4.52A10
		szLogMsg = "BH 2 Accumulated Missing Die Detected";
	}
	else if (lCode == 5)	//v4.59A7
	{
		OpDisplayAlarmPage(TRUE);
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH1 Uplook PR FAIL start");
		szLogMsg.Format("BH 1 Lookup PR failure exceeds limit (%ld)", m_ulBH1UplookPrFailCount);
		HmiMessage_Red_Yellow(szLogMsg, "AUTOBOND");
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH1 Uplook PR FAIL end");
	}
	else if (lCode == 6)	//v4.59A7
	{
		OpDisplayAlarmPage(TRUE);
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH2 Uplook PR FAIL start");
		szLogMsg.Format("BH 2 Lookup PR failure exceeds limit (%ld)", m_ulBH2UplookPrFailCount);
		HmiMessage_Red_Yellow(szLogMsg, "AUTOBOND");
		CMSLogFileUtility::Instance()->MS_LogMDOperation("BH2 Uplook PR FAIL end");
	}

	SetErrorMessage(szLogMsg);

	//BH back to SAFE position & abort PICK action 
	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
	{
		AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	}
	else
	{
		AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
	}

	T_SafeMoveToInAuto(m_lCleanColletPos_T, "MD1 stop");		// Move T to Blow position

	// Treat as no die picked
	(*m_psmfSRam)["BondHead"]["DiePickedForBT"] = FALSE;
	//SetDiePicked(FALSE);
	TakeTime(PV2);		

	if ((lCode == 5) || (lCode == 6))
	{
		//v4.59A6
		//do not turn off vac if PR uplook fail
	}
	else
	{
		if (IsBHZ1ToPick())		//If BHZ1 towawrds PICK
		{
			SetPickVacuumZ2(FALSE); 
		}			
		else
		{
			SetPickVacuum(FALSE); 
		}
	}

	Sleep(1000);
	return TRUE;
}


BOOL CBondHead::ReachMissingDieCountLimit()
{
	if (IsBHZ1ToPick())
	{
		m_pBHZ2AirFlowCompensation->IncreaseMissingDieCount();
		if (m_pBHZ2AirFlowCompensation->ReachMissingDieCountLimit())
		{
			return TRUE;
		}
	}
	else
	{
		m_pBHZ1AirFlowCompensation->IncreaseMissingDieCount();
		if (m_pBHZ1AirFlowCompensation->ReachMissingDieCountLimit())
		{
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CBondHead::OpMissingDieUplookPrFailHandling(BOOL bUpLookFailAtMD, BOOL bRetryHandling)		//v4.59A5
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPr	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	CString szMsg;
	
	// Wait T complete
	if (m_bComplete_T == FALSE)
	{
		T_Sync();
		m_bComplete_T = TRUE;
		
		//v4.43T1
		if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
		{
			TakeTime(T4);
		}
		else
		{
			if (bEnableBHUplookPr)	//v4.57A13
				TakeTime(T4);
			else
				TakeTime(T2);
		}
	}
/*
	if (!bUpLookFailAtMD)		//v4.59A7
	{
		if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
		{
			m_lMD_Count2--;
		}
		else
		{
			m_lMD_Count--;
		}
	}
*/

	//SetDiePicked(FALSE);
	if( bUpLookFailAtMD )
		(*m_psmfSRam)["BondHead"]["MissingDie"] = 2;
	else
		(*m_psmfSRam)["BondHead"]["MissingDie"] = TRUE;
	DisplaySequence("BH - |missing die| ==> UpLook PR Fail Handling");

	m_bDetectDAirFlowAtPrePick = TRUE;		//v4.52A12
	if (m_bMS100DigitalAirFlowSnr)	
	{
		if (IsBHZ1ToPick())			//If BHZ2 towards PICK	
			BH_Z_AirFlowLog(FALSE, "MD Retry,");
		else
			BH_Z_AirFlowLog(TRUE, "MD Retry,");
	}

	//First move Z/Z2 up at PICK side before moving BH to ColletClean pos
	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
	{
		if (m_bComplete_Z == FALSE)
		{
			Z_Sync();
		}
		AC_Z1_MoveTo(m_lSwingLevel_Z, SFM_WAIT);
	}
	else
	{
		if (m_bComplete_Z2 == FALSE)
		{
			Z2_Sync();
		}
		AC_Z2_MoveTo(m_lSwingLevel_Z2, SFM_WAIT);
	}


	szMsg.Format("BH - MD Retry %d %d %d", m_lMDCycleCount1, m_lMD_Count, m_lMD_Count2);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);							
	OpIncEjKOffsetForBHZMD();		//v4.52A10
	DisplaySequence(szMsg);

	BOOL bDoAuto = TRUE;
	if (pApp->GetCustomerName() == "ChangeLight(XM)" && !pApp->m_bOperator)
	{
		bDoAuto = FALSE;
	}
	//v4.52A13	//XA SanAn
	if (bRetryHandling || pApp->GetCustomerName() == CTM_SANAN)
	{
		if (IsBHZ2ToPick() && (bRetryHandling || (m_lMD_Count == 1)) && m_bAutoCleanCollet && bDoAuto)		//If BHZ2 towards PICK	
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BH: MD Retry AutoCleanCollet 1");							
			T_Profile(CLEAN_COLLET_PROF);		// Change T profile
			OpMS100AutoCleanCollet_MDCJ(TRUE);
			T_Profile(CLEAN_COLLET_PROF);		// Change T profile
			T_SafeMoveToInAuto(m_lCleanColletPos_T, "UpLook PR Fail");		// Move to clean collet position
		}
		else if (!IsBHZ2ToPick() && (bRetryHandling || (m_lMD_Count2 == 1)) && m_bAutoCleanCollet && bDoAuto)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BH: MD Retry AutoCleanCollet 2");							
			T_Profile(CLEAN_COLLET_PROF);		// Change T profile //make sure using clean profile when moving in autoclean collet
			OpMS100AutoCleanCollet_MDCJ(TRUE);
			T_Profile(CLEAN_COLLET_PROF);		// Change T profile
			T_SafeMoveToInAuto(m_lCleanColletPos_T, "UpLook PR Fail");		// Move to clean collet position
		}
		else
		{
			CTime stStartTime = CTime::GetCurrentTime();
			CheckCoverOpenInAuto("MD1 blow");
			T_Profile(CLEAN_COLLET_PROF);		// Change T profile
			T_SafeMoveToInAuto(m_lCleanColletPos_T, "UpLook PR Fail");		// Move to clean collet position

			TakeTime(PV2);	
			if (IsBHZ1ToPick())		//If BHZ1 towawrds PICK		
			{
				SetPickVacuumZ2(FALSE);		
				Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
				SetStrongBlowZ2(TRUE);
				Sleep(m_lHighBlowTime);	 
				SetStrongBlowZ2(FALSE);
			}
			else
			{
				SetPickVacuum(FALSE);	
				Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
				SetStrongBlow(TRUE);
				Sleep(m_lHighBlowTime);	
				SetStrongBlow(FALSE);
			}

			CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AirFlowByMD");
		}
	}
	else
	{
		if (m_bSemitekBLMode && 			//v4.44A1	//SEmitek
			m_bAutoCleanCollet && 
			m_bMDRetryUseAutoCleanCollet)	//v4.49A3	//WH SanAn
		{
			OpMS100AutoCleanCollet_MDCJ(TRUE);
			T_Profile(CLEAN_COLLET_PROF);		// Change T profile
			T_SafeMoveToInAuto(m_lCleanColletPos_T, "UpLook PR Fail");		// Move to clean collet position
		}
		else
		{
			CTime stStartTime = CTime::GetCurrentTime();

			//v4.59A7	//v4.59A16	//disabled
			/*if (IsMS90())		//WolfSpeed & Finisar MS90
			{
				if (!OpManualCleanCollet_MDCJ_MS90())
				{
					(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
					SetErrorMessage("OpManualCleanCollet_MDCJ_MS90 fail");
					return FALSE;
				}
			}
			else*/
			{
				CheckCoverOpenInAuto("MD1 blow");
				T_Profile(CLEAN_COLLET_PROF);		// Change T profile
				T_SafeMoveToInAuto(m_lCleanColletPos_T, "UpLook PR Fail");		// Move to clean collet position

				TakeTime(PV2);	
				if (IsBHZ1ToPick())		//If BHZ1 towawrds PICK		
				{
					SetPickVacuumZ2(FALSE);		
					Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
					SetStrongBlowZ2(TRUE);
					Sleep(m_lHighBlowTime);	 
					SetStrongBlowZ2(FALSE);
				}
				else
				{
					SetPickVacuum(FALSE);	
					Sleep(500);						//Extra 500ms NEUTRAL delay before StrongBlow	//v4.17T7
					SetStrongBlow(TRUE);
					Sleep(m_lHighBlowTime);	
					SetStrongBlow(FALSE);
				}
			}

			CMSLogFileUtility::Instance()->AC_NoProductionTimeLog(stStartTime, "AirFlowByMD");
		}
	}

	Sleep(1000);

	if (IsZMotorsEnable() == FALSE)
	{
		SetMotionCE(TRUE, "IsZMotorsEnable fail in OpMissingDieUplookPrFailHandling");	//v4.59A19
		
		//SetErrorMessage("BondHead module is off power");
		//SetAlert_Red_Yellow(IDS_BH_MODULE_NO_POWER);
		//m_qSubOperation = HOUSE_KEEPING_Q;
		return FALSE;
	}

	CheckCoverOpenInAuto("MD Bond To Pick");

	if (IsBHZ1ToPick())	//If BHZ1 towards PICK
	{
		//BA_MoveTo(m_lPickPos_T, SFM_WAIT);
		T_MoveTo(m_lPickPos_T, SFM_WAIT);		//v4.46T14
		DisplaySequence("BH - BHZ2 On missing, T to Pick Posn");
	}
	else
	{
		T_MoveTo(m_lBondPos_T, SFM_WAIT);		//v4.46T14
		DisplaySequence("BH - BHZ1 On missing, T to Bond Posn");
		//BA_MoveTo(m_lBondPos_T, SFM_WAIT);
	}

	T_Profile(NORMAL_PROF);

	//Finally move Z/Z2 down to PICK level when done
	if (IsBHZ1ToPick())		//If BHZ1 towards PICK	
	{
		szMsg.Format("BH - BHZ1 down to pick %d after BHZ2 missing die cleaned", m_lPickLevel_Z);
		AC_Z1P_MoveToOrSearch(m_lPickLevel_Z, SFM_WAIT);
		//m_nWTAtColletPos = 1;//Matt:2019.04.08
	}
	else
	{
		szMsg.Format("BH - BHZ2 down to pick %d after BHZ1 missing die cleaned", m_lPickLevel_Z2);
		AC_Z2P_MoveToOrSearch(m_lPickLevel_Z2, SFM_WAIT);
	}


	CMSLogFileUtility::Instance()->BPR_Arm1Log(szMsg);
	CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	DisplaySequence(szMsg);

	return TRUE;
}

BOOL CBondHead::TurnOnBlowAndCatchModule(BOOL bOn)	//v4.59A12
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_RENESAS)
	{
		return TRUE;
	}

	if (bOn)
	{
		SetBlowAndCatchSolZ(TRUE);
		Sleep(1000);

		//shiraishi01
		INT nCount = 0;
		while (!IsScopeDown())
		{
			Sleep(200);
			nCount++;
			if (nCount > 5)
				break;
		}

		SetBlowAndCatchSolY(TRUE);
		Sleep(2500);
		SetBlowAndCatchSolBlow(TRUE);
		Sleep(500);
	}
	else
	{
		SetBlowAndCatchSolBlow(FALSE);
		
		SetBlowAndCatchSolY(FALSE);
		Sleep(1000);
		if (!IsCatchAndBlowYOn())
		{
			Sleep(1000);
		}
		SetBlowAndCatchSolZ(FALSE);
		//}
	}

	return TRUE;
}

//================================================================
// Function Name: 		GetBHZ1MissingDieThresholdValue
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Missing die on BHZ1
// Return:				None
// Remarks:				None
//================================================================
ULONG CBondHead::GetBHZ1MissingDieThresholdValue(const ULONG ulBHZ1DAirFlowUnBlockValue)
{
	return GetBHZ1MissingDieThresholdValue(ulBHZ1DAirFlowUnBlockValue, m_ulBHZ1DAirFlowBlockValue);
}


//================================================================
// Function Name: 		GetBHZ1MissingDieThresholdValue
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Missing die on BHZ1
// Return:				None
// Remarks:				None
//================================================================
ULONG CBondHead::GetBHZ1MissingDieThresholdValue(const ULONG ulBHZ1DAirFlowUnBlockValue, const ULONG ulBHZ1DAirFlowBlockValue)
{
	ULONG ulThresholdValue = 0;

	if ( (m_dBHZ1ThresholdPercent >= 10) && (m_dBHZ1ThresholdPercent <= 90) )
	{
		ulThresholdValue = (ULONG)_round(m_dBHZ1ThresholdPercent / 100.0 * (ulBHZ1DAirFlowUnBlockValue - ulBHZ1DAirFlowBlockValue)) + ulBHZ1DAirFlowBlockValue;
	}
	else
	{
		ulThresholdValue = (ULONG)_round((ulBHZ1DAirFlowUnBlockValue - ulBHZ1DAirFlowBlockValue) / 2.0) + ulBHZ1DAirFlowBlockValue;
	}
	
	return ulThresholdValue;
}

//================================================================
// Function Name: 		GetBHZ2MissingDieThresholdValue
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Missing die on BHZ2
// Return:				None
// Remarks:				None
//================================================================
ULONG CBondHead::GetBHZ2MissingDieThresholdValue(const ULONG ulBHZ2DAirFlowUnBlockValue)
{
	return GetBHZ2MissingDieThresholdValue(ulBHZ2DAirFlowUnBlockValue, m_ulBHZ2DAirFlowBlockValue);
}


//================================================================
// Function Name: 		GetBHZ2MissingDieThresholdValue
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Missing die on BHZ2
// Return:				None
// Remarks:				None
//================================================================
ULONG CBondHead::GetBHZ2MissingDieThresholdValue(const ULONG ulBHZ2DAirFlowUnBlockValue, const ULONG ulBHZ2DAirFlowBlockValue)
{
	ULONG ulThresholdValue = 0;

	if ( (m_dBHZ1ThresholdPercent >= 10) && (m_dBHZ1ThresholdPercent <= 90) )
	{
		ulThresholdValue = (ULONG)_round(m_dBHZ1ThresholdPercent / 100.0 * (ulBHZ2DAirFlowUnBlockValue - ulBHZ2DAirFlowBlockValue)) + ulBHZ2DAirFlowBlockValue;
	}
	else
	{
		ulThresholdValue = (ULONG)_round((ulBHZ2DAirFlowUnBlockValue - ulBHZ2DAirFlowBlockValue) / 2.0) + ulBHZ2DAirFlowBlockValue;
	}

	return ulThresholdValue;
}

//================================================================
// Function Name: 		GetBHZ1ColletJamThresholdValue
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Collet jam on BHZ1
// Return:				None
// Remarks:				None
//================================================================
ULONG CBondHead::GetBHZ1ColletJamThresholdValue(const ULONG ulBHZ1DAirFlowUnBlockValue)
{
	ULONG ulThresholdValue = 0;

	if ( (m_dBHZ2ThresholdPercent >= 10) && (m_dBHZ2ThresholdPercent <= 90) )
	{
		ulThresholdValue = (ULONG)_round(m_dBHZ2ThresholdPercent / 100.0 * (ulBHZ1DAirFlowUnBlockValue - m_ulBHZ1DAirFlowBlockValue)) + m_ulBHZ1DAirFlowBlockValue;
	}
	else
	{
		ulThresholdValue = (ULONG)_round((ulBHZ1DAirFlowUnBlockValue - m_ulBHZ1DAirFlowBlockValue) / 2.0) + m_ulBHZ1DAirFlowBlockValue;
	}

	return ulThresholdValue;
}
	

//================================================================
// Function Name: 		GetBHZ2ColletJamThresholdValue
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Collet jam on BHZ2
// Return:				None
// Remarks:				None
//================================================================
ULONG CBondHead::GetBHZ2ColletJamThresholdValue(const ULONG ulBHZ2DAirFlowUnBlockValue)
{
	ULONG ulThresholdValue = 0;

	if ( (m_dBHZ2ThresholdPercent >= 10) && (m_dBHZ2ThresholdPercent <= 90) )
	{
		ulThresholdValue = (ULONG)_round(m_dBHZ2ThresholdPercent / 100.0 * (ulBHZ2DAirFlowUnBlockValue - m_ulBHZ2DAirFlowBlockValue)) + m_ulBHZ2DAirFlowBlockValue;
	}
	else
	{
		ulThresholdValue = (ULONG)_round((ulBHZ2DAirFlowUnBlockValue - m_ulBHZ2DAirFlowBlockValue) / 2.0) + m_ulBHZ2DAirFlowBlockValue;
	}

	return ulThresholdValue;
}

BOOL CBondHead::OpPrestartColletHoleCheck(BOOL bBH1, BOOL bPreStart)
{

	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed in prestart check collet hole");
		return TRUE;
	}
	if ( IsMotionCE() == TRUE )
	{
		SetErrorMessage("Check Critical Error in Check Collet Hole(Wafer Side)!");
		return FALSE;
	}
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));

	SetBhTReadyForWPR(FALSE);
	SetEjectorVacuum(FALSE);
	//SetPickVacuum(FALSE);
	//SetPickVacuumZ2(FALSE);
	//SetStrongBlow(TRUE);
	//SetStrongBlowZ2(TRUE);
	//Sleep(100);
	//SetStrongBlow(FALSE);
	//SetStrongBlowZ2(FALSE);
	//Sleep(100);
	//SetPickVacuum(TRUE);
	//SetPickVacuumZ2(TRUE);
	//Sleep(100);
	//if (IsColletJam() || IsColletJamZ2())
	LONG lWTX = 0, lWTY = 0, lWTT = 0;

	if (!bPreStart)
	{
		GetWaferTableEncoder(&lWTX, &lWTY, &lWTT);
	}
	else
	{
		if (IsColletJamForBothBH() != 0) //Matthew 20190415
		{
			CString szError;
			szError.Format("Collet Jam in prestart");
			HmiMessage_Red_Yellow(szError, "Collet Jam!");
			SetErrorMessage("Collet Jam in prestart check collet hole function!");
			return FALSE;
		}
	}
	Z_MoveTo(m_lSwingLevel_Z,SFM_NOWAIT);
	Z2_MoveTo(m_lSwingLevel_Z2,SFM_NOWAIT);
	Ej_MoveTo(0, SFM_NOWAIT);

	T_Sync();
	Z_Sync();
	Z2_Sync();
	pWaferTable->GoToNoDiePos(SFM_WAIT);

	if (bBH1)//If BHZ1 at Bond	
	{
		OpSwitchEjectorTableXY(TRUE,SFM_WAIT);
		T_MoveTo(m_lPickPos_T);
		Z_MoveTo(m_lPickLevel_Z);

		m_lBH1MarkCount = 0;
		m_bDoColletSearch = TRUE;
		Sleep(m_lCheckBHMarkDelay);
		GetEncoderValue();
		m_lColletHoleEnc_T = m_lEnc_T;
		m_lColletHoleEnc_Z = m_lEnc_Z;

		(*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"] = FALSE;
		DisplaySequence("BH - BH1 Collet Prestart");
	}
	else
	{
		OpSwitchEjectorTableXY(FALSE,SFM_WAIT);
		T_MoveTo(m_lBondPos_T);
		Z2_MoveTo(m_lPickLevel_Z2);

		m_lBH2MarkCount = 0;
		m_bDoColletSearch = TRUE;
		Sleep(m_lCheckBHMarkDelay);
		GetEncoderValue();
		m_lColletHoleEnc_T = m_lEnc_T;
		m_lColletHoleEnc_Z = m_lEnc_Z2;
			
		(*m_psmfSRam)["BondHead"]["ColletTest"]["BH Mark 2"] = TRUE;
		DisplaySequence("BH - BH2 Collet Prestart");
	}

	while (!pWaferPr->IsPrestartFinished())
	{
		Sleep(100);
	}

	pWaferPr->OpAutoSearchColletHoleWithEpoxyPattern();

	while(1)
	{
		if(WaitSearchMarkReady())
		{
			break;
		}
	}

	Ej_MoveTo(m_lStandbyLevel_Ej, SFM_WAIT);
	if (bBH1)		//If BHZ1 at Bond	
	{
		Z_MoveTo(m_lSwingLevel_Z);
	}
	else
	{
		Z2_MoveTo(m_lSwingLevel_Z2);
	}

	Sleep(20);
	T_MoveTo(m_lPrePickPos_T);

	if (!bPreStart)
	{
		pWaferTable->GoTo(lWTX, lWTY, SFM_WAIT);
		Sleep(30);
		SetEjectorVacuum(TRUE);
		Sleep(300);
	}

	return TRUE;
}

BOOL CBondHead::OpPrestartBPRColletHoleCheck(BOOL bBH1)
{
	if (IsWLExpanderOpen() == TRUE)
	{
		SetErrorMessage("Expander not closed in prestart check BPR collet hole");
		return TRUE;
	}
	if ( IsMotionCE() == TRUE )
	{
		SetErrorMessage("Check Critical Error in Check Collet Hole(Bond Side)!");
		return FALSE;
	}
	BOOL bResult = FALSE;
	Z_MoveTo(m_lSwingLevel_Z,SFM_NOWAIT);
	Z2_MoveTo(m_lSwingLevel_Z2,SFM_NOWAIT);

	T_Sync();
	Z_Sync();
	Z2_Sync();
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	pBinTable->MoveTableToEmptyPosition();
	Sleep(30);
	if (bBH1)
	{
		T_Profile(SETUP_PROF); 
		T_SMoveTo(m_lBondPos_T);
		T_Profile(NORMAL_PROF);
		Z_MoveTo(m_lBondLevel_Z);
		Sleep(30);
		bResult = pBondPr->OpAutoSearchBPRColletHoleWithEpoxyPattern(FALSE);
	}
	else
	{
		T_Profile(SETUP_PROF); 
		T_SMoveTo(m_lPickPos_T);
		T_Profile(NORMAL_PROF);
		Z2_MoveTo(m_lBondLevel_Z2);
		Sleep(30);
		bResult = pBondPr->OpAutoSearchBPRColletHoleWithEpoxyPattern(TRUE);
	}

	Z_MoveTo(m_lSwingLevel_Z,SFM_NOWAIT);
	Z2_MoveTo(m_lSwingLevel_Z2,SFM_NOWAIT);

	Z_Sync();
	Z2_Sync();
	Sleep(10);
	T_MoveTo(m_lPrePickPos_T);
	return bResult;
}


BOOL CBondHead::OpAutoLearnBHZPickLevel(BOOL bBHZ2)//After WT2
{
	BOOL bStatus = TRUE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg;
	//Asume T is ready @ pick/bond posn after wt2
	if (bBHZ2)
	{
		LONG lOldPickLevelZ2 = m_lPickLevel_Z2;
		LONG lNewPickLevelZ2 = lOldPickLevelZ2 + 1000;

		bStatus = AutoLearnBHZ2BondLevel(lNewPickLevelZ2 ,lOldPickLevelZ2, 200);

		if (bStatus)
		{
			lNewPickLevelZ2 += m_lPickDriveIn;
			m_bAutoLearnPickLevelZ2 = FALSE;

			if (labs(lNewPickLevelZ2 - lOldPickLevelZ2) > 500)
			{
				bStatus = FALSE;
				HmiMessage_Red_Back("New Old Z2 Offset diffence ove 500 counts!", "Auto Pick Level");
				CMSLogFileUtility::Instance()->MS_LogOperation("Auto Learn PICK Level Z2 Fails!");	
			}
			else
			{
				m_lBPGeneral_B	= m_lPickLevel_Z2	= lNewPickLevelZ2;
				SetPickVacuumZ2(TRUE);
				Z2_MoveTo(m_lSwingLevel_Z2);		
				szMsg.Format("Auto Learn PICK Level Z2,New,%d,Old,%d",lNewPickLevelZ2,lOldPickLevelZ2);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			}
		}
	}
	else
	{
		LONG lOldPickLevelZ1 = m_lPickLevel_Z;
		LONG lNewPickLevelZ1 = lOldPickLevelZ1 + 1000;

		bStatus = AutoLearnBHZ1BondLevel(lNewPickLevelZ1,FALSE, lOldPickLevelZ1, 200);

		if (bStatus)
		{
			lNewPickLevelZ1 += m_lPickDriveIn;
			m_bAutoLearnPickLevelZ1 = FALSE;

			if (labs(lNewPickLevelZ1 - lOldPickLevelZ1) > 500)
			{
				bStatus = FALSE;
				HmiMessage_Red_Back("New Old Z1 Offset diffence ove 500 counts!", "Auto Pick Level");
				CMSLogFileUtility::Instance()->MS_LogOperation("Auto Learn PICK Level Z1 Fails!");
			}
			else
			{
				m_lBPGeneral_6	= m_lPickLevel_Z	= lNewPickLevelZ1;
				SetPickVacuum(TRUE);
				Z_MoveTo(m_lSwingLevel_Z);
				szMsg.Format("Auto Learn PICK Level Z1,New,%d,Old,%d",lNewPickLevelZ1,lOldPickLevelZ1);
				CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			}
		}
	}
	return bStatus;
}

BOOL CBondHead::CheckBPRErrorNeedAlarm(LONG lMark)
{
	m_bBPRErrorToCleanCollet = FALSE;
	//if ((m_nBHAlarmCode == -4) ||	//postbond already triggered in "last" cycle	//v4.21T8
	//	(m_nBHAlarmCode == 6))		//postbond triggerd before BH down to PICK		//v4.21T8
	//{
		CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
		BOOL bNoCleanCollet = pAppMod->GetFeatureStatus(MS896A_FUNC_BPRERROR_NOCLEANCOLLET);
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		
		LONG lBPRErrorToCleanColletResetCount = 200;//pApp->GetProfileInt(gszPROFILE_SETTING, _T("BPR Error To Clean Collet Reset Count"), 10000);
		LONG lNewPickCount = GetNewPickCount();
		CString szMsg;
		szMsg.Format("BH - Check BPR Error Need Alarm,NewPickCount,%d,ErrorCount,%d,Mark,%d,reset count,%d,OP,%d,NoCleanCollet,%d,Auto,%d",
			lNewPickCount,m_lBPRErrorToCleanColletCount,lMark,lBPRErrorToCleanColletResetCount,pApp->m_bOperator,bNoCleanCollet,m_bAutoCleanCollet);
		DisplaySequence(szMsg);
		if (lNewPickCount - m_lBPRErrorToCleanColletCount < lBPRErrorToCleanColletResetCount && 
			m_lBPRErrorToCleanColletCount != 0 &&
			lNewPickCount != m_lBPRErrorToCleanColletCount)
		{
			return TRUE;
		}

		if (pApp->GetCustomerName() != "ChangeLight(XM)")
		{
			return TRUE;
		}
		if (!pApp->m_bOperator)//OP mode go to clean collet
		{
			return TRUE;
		}
		if (!m_bAutoCleanCollet)
		{
			return TRUE;
		}
		if (bNoCleanCollet)
		{
			return TRUE;
		}

		m_lBPRErrorToCleanColletCount = lNewPickCount;
	
		m_bBPRErrorToCleanCollet = TRUE;
	//}
	return FALSE;
}

LONG CBondHead::GetMissingDieRetryCount()
{
	LONG lCount			= m_lMissingDie_Retry;
	LONG lBTGrade		= (*m_psmfSRam)["BinTable"]["BondingGrade"];//Get from BT Stn OpIndex
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));

	if (pBondPr->IsNGBlock(lBTGrade))
	{
		lCount = lCount *50;
	}
	return lCount;
}

LONG CBondHead::GetMDCycleLimit()
{
	LONG lCount			= m_lMDCycleLimit;
	LONG lBTGrade		= (*m_psmfSRam)["BinTable"]["BondingGrade"];//Get from BT Stn OpIndex
	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));

	if (pBondPr->IsNGBlock(lBTGrade))
	{
		lCount = lCount *50;
	}
	return lCount;
}
