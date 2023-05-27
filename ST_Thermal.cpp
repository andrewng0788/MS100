#include "stdafx.h"
#include "Safety.h"

#include "LblPtr.h"	
#include "PtrFactory.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CSafety::InitBondHeadTPreHeat()
{

#ifdef MS896A_UBH
	byte byReturn = 0;
	
	try
	{
		if (m_bInitBondHeadPreHeat == TRUE)
			return TRUE;

		SFM_CHipecAcServo* pBHServo_T = m_pInitOperation->GetHipecAcServo("BondHeadStn", "srvBondHeadT");
		
		if (pBHServo_T != NULL)
		//if ( CMS896AStn::MotionIsServo(BH_AXIS_T) )
		{
			/*
			if (hp_restart_periodic_currprot(&(pBHServo_T->m_HpCcb)) != HP_SUCCESS)
			{
				DisplayMessage("Init Bond Head T Pre-Heat Failed");
				return FALSE;
			}
			*/

			DisplayMessage("Init Bond Head T Pre-Heat");

			m_bInitBondHeadPreHeat = TRUE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

#endif

	return TRUE;
}

BOOL CSafety::CheckBondHeadTThermal()
{
	//No BHT in MegaDa
	return TRUE;

#ifdef MS896A_UBH
	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBH)	//v3.61
		return TRUE;

	if (CMS896AApp::m_bBondHeadTThermalControl == FALSE)
	{
		m_bInitBondHeadPreHeat = FALSE;
		return TRUE;
	}

	if (InitBondHeadTPreHeat() == FALSE)
		return FALSE;

	CTimeSpan ctTimeDiff;
	
	static CTime ctlastCheckTime = CTime::GetCurrentTime();
	ctTimeDiff = CTime::GetCurrentTime() - ctlastCheckTime;

	LONGLONG lTimeDiff = ctTimeDiff.GetTotalSeconds();

	//static double dLastCheckTime = GetTime();
	//double dTimeEplased = GetTime() - dLastCheckTime;
	try
	{
		// 1 s
		if (ctTimeDiff.GetTotalSeconds() >= 1)
		//if (dTimeEplased >= 1000)
		{
			
			SFM_CHipecAcServo* pBHServo_T = m_pInitOperation->GetHipecAcServo("BondHeadStn", "srvBondHeadT");
			
			if (pBHServo_T != NULL)
			//if ( CMS896AStn::MotionIsServo(BH_AXIS_T) )
			{
				CString szMsg = "";
				LONG lIvrms = 0, lIrrms = 0, lIsrms = 0, lItrms = 0;
				LONG lBHTTermalCompensation = 0;
				short sPortNo = 0;
				short sReturn = 0;
				
				/*
				if (hp_report_periodic_currprot(&(pBHServo_T->m_HpCcb)) != HP_SUCCESS)
				{
					ctlastCheckTime = CTime::GetCurrentTime();
					SetErrorMessage("Check Bond Head T Thermal Error: 1");
					DisplayMessage("Check Bond Head T Thermal Error: 1");
					return FALSE;
				}
				*/

				lIvrms = (LONG) pBHServo_T->m_HpCcb.hp_servo_ch.DACIvRMS;
				lIrrms = (LONG) pBHServo_T->m_HpCcb.hp_servo_ch.DACIrRMS;
				lIsrms = (LONG) pBHServo_T->m_HpCcb.hp_servo_ch.DACIsRMS;
				lItrms = (LONG) pBHServo_T->m_HpCcb.hp_servo_ch.DACItRMS;
				
				//sPortNo = HP_MTR_COMPENSATE_DAC_PORT_0 + pBHServo_T->GetChannelId() -1;

				// average rms
				//lBHTTermalCompensation = m_lBHTThermalSetPoint - lIvrms;

				// average power

				lBHTTermalCompensation = m_lBHTThermalSetPoint * m_lBHTThermalSetPoint - lIvrms * lIvrms;

				if (lBHTTermalCompensation <= 0)
				{
					lBHTTermalCompensation = 0;
				}
				else
				{
					lBHTTermalCompensation = (LONG) sqrt(lBHTTermalCompensation);
				}

				if ((sReturn = WriteSoftwarePort(&pBHServo_T->GetModule()->m_HpMcb, sPortNo , lBHTTermalCompensation, 0xffff))
					!= HP_SUCCESS)
				{
					ctlastCheckTime = CTime::GetCurrentTime();
					CString szMsg;
					szMsg.Format("Check Bond Head T Thermal Error: %d",sReturn);
					SetErrorMessage(szMsg);
					DisplayMessage(szMsg);
					return FALSE;
				}

				szMsg.Format("%d,%d,%d,%d,%d",lIvrms,lIrrms,lIsrms,lItrms,lBHTTermalCompensation);
				CMSLogFileUtility::Instance()->BH_LogBondHeadTThermal(szMsg);
			}
			
			ctlastCheckTime = CTime::GetCurrentTime();
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

#endif

	return TRUE;
}