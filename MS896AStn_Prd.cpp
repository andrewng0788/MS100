#include "stdafx.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include <math.h>
#include <sys/timeb.h>
#include "GenerateDatabase.h"
#include "GenerateWaferDatabase.h"
#include "QueryWaferDatabase.h"
#include "QueryDatabase.h"
#include "OutputFileFactory.h"
#include "FactoryMap.h"
#include "OutputFileInterface.h"
#include "WaferEndFileFactory.h"
#include "WaferEndFactoryMap.h"
#include "WaferEndFileInterface.h"
//#include "Encryption.h"
#include "PrescanInfo.h"
#include "BH_Constant.h"

#include <gdiplus.h>
#include <AtlImage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ULONG	CMS896AStn::m_ulNewPickCounter	= 0;
ULONG	CMS896AStn::m_ulNextAdvSampleCount	= 0;
LONG	CMS896AStn::m_ulLastRealignDieCount	= 0;

BOOL	CMS896AStn::m_bAutoSampleAfterPE	= FALSE;
BOOL	CMS896AStn::m_bAutoSampleAfterPR	= FALSE;

LONG	CMS896AStn::m_lPredictMethod	= 0;

BOOL	CMS896AStn::m_bAdvRegionOffset		= FALSE;
BOOL	CMS896AStn::m_bPitchCheckMapWafer	= FALSE;

BOOL	CMS896AStn::m_bEnableAdaptPredict	= FALSE;
BOOL	CMS896AStn::m_bEnableDEBSort	= FALSE;
BOOL	CMS896AStn::m_bDEB_INIT_OK		= FALSE;
//PstnModel	CMS896AStn::m_oPstModel;
LONG	CMS896AStn::m_lGetSetLogic		= 0;
BOOL	CMS896AStn::m_bGiveUpDebKeyDie	= FALSE;
UCHAR	CMS896AStn::m_ucDEBKeyDieGrade	= 0;

BOOL	CMS896AStn::m_bAutoRescanWafer		= FALSE;
LONG	CMS896AStn::m_lRescanRunStage		= 0;	//	0, idle; 1, ready to rescan; 2, PR init, scanning; 3, construct done;
LONG	CMS896AStn::m_lReadyToSampleOnWft	= 0;
LONG	CMS896AStn::m_lReadyToCleanEjPinOnWft = 0;

//	Thermal Control Variables
BOOL	CMS896AStn::m_bDBHThermalCheck			= FALSE;
BOOL	CMS896AStn::m_bEnableMS60ThermalCheck	= FALSE;
BOOL	CMS896AStn::m_bMS60ThermalCtrl			= FALSE;
DOUBLE	CMS896AStn::m_dID		= 0.0;
DOUBLE	CMS896AStn::m_dIS		= 0.0;
DOUBLE	CMS896AStn::m_dIQ		= 0.0;
DOUBLE	CMS896AStn::m_dRmsRSum	= 0.0;
DOUBLE	CMS896AStn::m_dRmsSSum	= 0.0;
DOUBLE	CMS896AStn::m_dRmsTSum	= 0.0;
DOUBLE	CMS896AStn::m_dMS60ThermalUpdateTime	= 0.0;
INT		CMS896AStn::m_nRmsCount	= 0;
ULONG	CMS896AStn::m_ulBHTThermostatCounter	= 0;
ULONG	CMS896AStn::m_ulBHTThermostatReading	= 0;
DOUBLE	CMS896AStn::m_dBHTThermostatReading		= 0;
ULONG	CMS896AStn::m_ulBHTThermostatADC		= 0;
UINT	CMS896AStn::m_unMS60TargetIdleTemperature = 0;
CMSNmAxisInfo	CMS896AStn::m_stBHAxis_T;


ULONG CMS896AStn::GetNewPickCount()
{
	return m_ulNewPickCounter;
}

ULONG CMS896AStn::GetNextXXXCounter()	// next realign key die/adv.sample/rescan wafer.
{
	if( m_ulNextAdvSampleCount>1 )
	{
		return m_ulNextAdvSampleCount-1;
	}

	return m_ulNextAdvSampleCount;
}

LONG CMS896AStn::GetWftSampleState()
{
	return	m_lReadyToSampleOnWft;
}


LONG CMS896AStn::GetWftCleanEjPinState()
{
	return	m_lReadyToCleanEjPinOnWft;
}

BOOL CMS896AStn::IsAdvRegionOffset()
{
	return m_bAdvRegionOffset && (IsBurnIn()==FALSE) && (IsPrescanEnable() || IsScnLoaded());
}

BOOL CMS896AStn::IsVerifyMapWaferEnable()
{
	return IsPrescanEnable() && IsAdvRegionOffset() && m_bPitchCheckMapWafer;
}

BOOL CMS896AStn::IsSamplingRescanEnable()
{
	return (m_lPredictMethod == 5) && (IsBurnIn()==FALSE) && IsPrescanEnable();
}

BOOL CMS896AStn::DEB_IsEnable()
{
	return m_bEnableAdaptPredict && (IsBurnIn()==FALSE) && (IsPrescanEnable() || IsScnLoaded());
}

BOOL CMS896AStn::DEB_IsUseable()
{
	return m_bDEB_INIT_OK && DEB_IsEnable() && (IsBurnIn()==FALSE);
}

BOOL CMS896AStn::DEB_GetDiePosnOnly(CONST LONG lRow, CONST LONG lCol, LONG& lPhyX, LONG& lPhyY)
{
	//if( !IsAutoRescanAndDEB() )
	//{
		return FALSE;
	//}
	//INT nStatus = MS_Init;	//	rescan+deb XXX
	//BOOL bRtn = DEB_GetDiePos(lRow, lCol, lPhyX, lPhyY, nStatus);
	//DEB_UpdateDiePos(lRow, lCol, lPhyX, lPhyY, MS_Align);
	//return bRtn;

}

BOOL CMS896AStn::DEB_GetDiePos(CONST LONG lRow, CONST LONG lCol, LONG& lPhyX, LONG& lPhyY, INT& nStatus)
{
	//if( !DEB_IsUseable() )
	//{
		return FALSE;
	//}
	//BOOL bReturn = m_oPstModel.GetDieEncPosition(lRow, lCol, lPhyX, lPhyY, nStatus);

	//return bReturn;
}

BOOL CMS896AStn::DEB_UpdateDiePos(CONST LONG lRow, CONST LONG lCol, 
								  CONST LONG lPhyX, CONST LONG lPhyY,
								  CONST INT nStatus)
{	// can be defined in wafer table only
	//if( !DEB_IsUseable() )
	//{
		return FALSE;
	//}
	//BOOL bReturn = m_oPstModel.SetDieEncPosition(lRow, lCol, lPhyX, lPhyY, nStatus);
	//if( bReturn==FALSE )
	//{
	//	CString szMsg;
	//	szMsg.Format("Set Die error %ld,%ld Enc Position %ld,%ld, %d", lRow, lCol, lPhyX, lPhyY, nStatus);
	//	SetErrorMessage(szMsg);
	//	return FALSE;
	//}

	//return TRUE;
}


BOOL CMS896AStn::IsAutoRescanEnable()
{
	return IsPrescanEnable() && (IsBurnIn()==FALSE) && m_bAutoRescanWafer;
}

BOOL CMS896AStn::IsAutoRescanWafer()
{
	return IsAutoRescanEnable() && m_lRescanRunStage>0;
}

BOOL CMS896AStn::IsAutoRescanAndDEB()
{
	return IsAutoRescanEnable() && DEB_IsUseable();
}

BOOL CMS896AStn::IsAutoRescanAndSampling()
{
	return IsAutoRescanEnable() && IsSamplingRescanEnable();
}

BOOL CMS896AStn::IsAutoRescanOnly()
{
	return IsAutoRescanEnable() && DEB_IsEnable()==FALSE;
}

BOOL CMS896AStn::IsAutoSampleSort()
{
	return IsAutoRescanEnable() || IsAdvRegionOffset() || DEB_IsUseable();
}

BOOL CMS896AStn::PredictGetDieValidPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap)
{	//	rescan+deb XXX	predict to get die valid position.
	LONG lTmpWfX = 0, lTmpWfY = 0;
	BOOL	bFindNearValid = FALSE;
	LONG	lTmpRow, lTmpCol;
	ULONG ulCurrentLoop = 1;
	LONG  lRow, lCol, lCurrentIndex = 0;
	LONG lDiff_X, lDiff_Y;
	LONG lUpIndex = 0;
	LONG lDnIndex = 0;
	LONG lLtIndex = 0;
	LONG lRtIndex = 0;

	lTmpRow = ulIntRow;
	lTmpCol = ulIntCol;
	if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
	{
		if( IsAutoRescanAndDEB() )
		{
			DEB_GetDiePosnOnly(lTmpRow, lTmpCol,lTmpWfX, lTmpWfY);
		}
		lPhyX = lTmpWfX;
		lPhyY = lTmpWfY;
		return TRUE;
	}

	for (ulCurrentLoop = 1; ulCurrentLoop <= ulLoop; ulCurrentLoop++)
	{
		lRow = (ulCurrentLoop * 2 + 1);
		lCol = (ulCurrentLoop * 2 + 1);

		//Move to RIGHT & PR search on current die
		lCurrentIndex = 1;

		//Move table to RIGHT
		lDiff_X = 1;
		lDiff_Y = 0;
		lTmpRow += lDiff_Y;
		lTmpCol += lDiff_X;

		if (lTmpRow >= 0 && lTmpCol >= 0)
		{
			if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
			{
				if( IsAutoRescanAndDEB() )
				{
					DEB_GetDiePosnOnly(lTmpRow, lTmpCol,lTmpWfX, lTmpWfY);
				}
				bFindNearValid = TRUE;
				break;
			}
		}

		//Move to UP & PR search on current die
		lUpIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lUpIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lUpIndex--;

			//Move table to UP
			lDiff_X = 0;
			lDiff_Y = -1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					if( IsAutoRescanAndDEB() )
					{
						DEB_GetDiePosnOnly(lTmpRow, lTmpCol,lTmpWfX, lTmpWfY);
					}
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}

		//Move to LEFT & PR search on current die
		lLtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lLtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lLtIndex--;

			//Move table to LEFT
			lDiff_X = -1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					if( IsAutoRescanAndDEB() )
					{
						DEB_GetDiePosnOnly(lTmpRow, lTmpCol,lTmpWfX, lTmpWfY);
					}
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lDnIndex = (lRow - 1) - lCurrentIndex;
		while (1)
		{
			if (lDnIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lDnIndex--;

			//Move table to DN
			lDiff_X = 0;
			lDiff_Y = 1;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					if( IsAutoRescanAndDEB() )
					{
						DEB_GetDiePosnOnly(lTmpRow, lTmpCol,lTmpWfX, lTmpWfY);
					}
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}

		//Move to DOWN & PR search on current die
		lRtIndex = (lCol - 1) - lCurrentIndex;
		while (1)
		{
			if (lRtIndex == 0)
			{
				lCurrentIndex = 0;
				break;
			}
			lRtIndex--;

			//Move table to RIGHT
			lDiff_X = 1;
			lDiff_Y = 0;
			lTmpRow += lDiff_Y;
			lTmpCol += lDiff_X;

			if (lTmpRow >= 0 && lTmpCol >= 0)
			{
				if (GetWftPosn(bByMap, lTmpRow, lTmpCol, lTmpWfX, lTmpWfY))
				{
					if( IsAutoRescanAndDEB() )
					{
						DEB_GetDiePosnOnly(lTmpRow, lTmpCol,lTmpWfX, lTmpWfY);
					}
					bFindNearValid = TRUE;
					break;
				}
			}
		}

		if (bFindNearValid)
		{
			break;
		}
	}

	if (bFindNearValid)
	{
		LONG lDiePitchX_X = GetDiePitchX_X(); 
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_Y = GetDiePitchY_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();

		LONG lDiff_X = ulIntCol - lTmpCol;
		LONG lDiff_Y = ulIntRow - lTmpRow;

		//Calculate original no grade die pos from surrounding die position
		lPhyX = lTmpWfX - lDiff_X * lDiePitchX_X - lDiff_Y * lDiePitchY_X;
		lPhyY = lTmpWfY - lDiff_Y * lDiePitchY_Y - lDiff_X * lDiePitchX_Y;

	}

	return bFindNearValid;
}	//	Get valid by preddict

//	Thermal Control Functions
BOOL CMS896AStn::BH_EnableThermalControl(BOOL bEnable)	//	control vai IPC command for auto cycle only
{
	if (IsMS60() && m_bMS60ThermalCtrl)
	{
		CString szMsg;
		if( bEnable==FALSE )
		{
			m_nRmsCount = 10;
			OpUpdateBHTThermalRMSValues(0, TRUE);
			szMsg.Format("BH - Disable control thermal heating in auto for prescan or sampling");
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		}

		//v4.59A18
		if (bEnable)
		{
			BOOL bCriticalError = (BOOL)(LONG)(*m_psmfSRam)["MS"]["ThermalCriticalError"];
			if (bCriticalError)
			{
				return FALSE;
			}
		}

		EnableBHTThermalControl(bEnable, TRUE);	//	for prescan end or sampling start/stop
		if( bEnable )
		{
			m_nRmsCount = 10;
			szMsg.Format("BH - Enable control thermal heating in auto for prescan or sampling");
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			OpUpdateBHTThermalRMSValues(0, TRUE);
		}
		DisplaySequence(szMsg);
	}

	return TRUE;
}

VOID CMS896AStn::SetBondHeadFan(BOOL bOn)	//v4.09
{
	if (!m_fHardware)
	{
		return;
	}
	if (m_bDisableBH)	
	{
		return;
	}
	if (!CMS896AStn::m_bDBHThermalCheck)	//v4.26T1		//FOr MS100Plus v2.2 new thermal control
	{
		return;
	}

	CMS896AStn::MotionSetOutputBit(BH_SO_BH_FAN, bOn);

	if (IsMS60())	//v4.48A16
	{
		//Need to reset latch status on sensor board for MS100P3 & MS60
		if (bOn)
		{
			//pulse _|---|_
			Sleep(100);
			CMS896AStn::MotionSetOutputBit(MS60_SO_BH_FAN_RESET, TRUE);	
			Sleep(500);
			CMS896AStn::MotionSetOutputBit(MS60_SO_BH_FAN_RESET, FALSE);
		}
		else
		{
		}
	}

	return;
}

BOOL CMS896AStn::EnableBHTThermalControl(BOOL bEnable, BOOL bUseBHFan)
{
	if (!IsMS60())
		return TRUE;
	if (!m_bMS60ThermalCtrl)
		return TRUE;

	if (bEnable && !m_bEnableMS60ThermalCheck)
	{
		//v4.59A18
		BOOL bCriticalError = (BOOL)(LONG)(*m_psmfSRam)["MS"]["ThermalCriticalError"];
		if (bCriticalError)
		{
			return FALSE;
		}

		BOOL bAuto = FALSE;
		if (State() != IDLE_Q)
			bAuto = TRUE;
		if (IsMS60TempertureOverLoaded(bAuto, TRUE))
		{
			return FALSE;
		}


		m_bEnableMS60ThermalCheck = TRUE;

		if (bUseBHFan)		//v4.54A10
		{
			SetBondHeadFan(FALSE);
		}


#ifdef NU_MOTION_MS60	//v4.54A3
		m_pThermalCtrl->ResetThermalCtrl();
#endif

		CMSLogFileUtility::Instance()->MS_LogOperation("BHT Thermal Ctrl = ON");	//v4.54A7
	}
	else if (!bEnable && m_bEnableMS60ThermalCheck)
	{
		m_bEnableMS60ThermalCheck = FALSE;
		
		m_dID = 0;
		m_nRmsCount = 0;
		m_dRmsRSum = m_dRmsSSum = m_dRmsTSum = 0;
	
		INT nThermalDAC = 0;
		CMS896AStn::MotionSetMotorThermalCompDAC(BH_AXIS_T, nThermalDAC, &m_stBHAxis_T);

		if (bUseBHFan)		//v4.54A10
		{
			SetBondHeadFan(TRUE);
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("BHT Thermal Ctrl = OFF");	//v4.54A7
	}
	else	//v4.54A10
	{
		if (bEnable)
			m_bEnableMS60ThermalCheck = TRUE;
		else
			m_bEnableMS60ThermalCheck = FALSE;
	}

	return TRUE;
}

BOOL CMS896AStn::IsMS60TempertureOverLoaded(BOOL bAuto, BOOL bCheckNow)			//v4.49A5
{
	CString szLog;
	BOOL bStatus = FALSE;

	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (!IsMS60())
	{
		return FALSE;
	}
	if (!CMS896AStn::m_bDBHThermostat)
	{
		return FALSE;
	}

	if (!bCheckNow)	
	{
		m_ulBHTThermostatCounter++;
		if (m_ulBHTThermostatCounter < 10)
		{
			return FALSE;
		}
		//m_ulBHTThermostatCounter = 0;
	}

	LONG lValue = CMS896AStn::MotionReadInputADCPort(BHT_THERMOSTAT);
	m_ulBHTThermostatADC		= lValue;
	m_dBHTThermostatReading		= (DOUBLE) (250.0 * lValue / 32767.0);
	m_ulBHTThermostatReading	= (ULONG) m_dBHTThermostatReading;

	if (bAuto)
	{
		if (m_ulBHTThermostatReading > MS60_BH_MAX_TEMPERATURE_AUTOBOND || m_ulBHTThermostatReading == 0)		// 70 degree max. for MS60 T motor	//v4.49A6
		{
			(*m_psmfSRam)["MS"]["ThermalCriticalError"] = TRUE;		//v4.59A18
			bStatus = TRUE;
		}
	}
	else	//v4.53A19
	{
		//CString szLog;
		//szLog.Format("BH Temp (OverHeat) = %lu deg (limit=51), ADC = %ld", m_ulBHTThermostatReading, lValue);
		//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);							

		if (m_ulBHTThermostatReading > MS60_BH_MAX_TEMPERATURE_IDLE || m_ulBHTThermostatReading == 0)		// 70 degree max. for MS60 T motor	//v4.49A6
		{
			(*m_psmfSRam)["MS"]["ThermalCriticalError"] = TRUE;		//v4.59A18
			bStatus = TRUE;
		}
	}

	if (bStatus)
	{
		if (bAuto)		//v4.50A9	//Log if overheat or AUTOBOND
		{
			szLog.Format("BH Temp (OverHeat) = %lu deg (limit=%d), ADC = %ld",
							m_ulBHTThermostatReading, MS60_BH_MAX_TEMPERATURE_AUTOBOND, lValue);
			CMSLogFileUtility::Instance()->BH_LogStatus(szLog);
		}
		else
		{
			szLog.Format("BH Temp (OverHeat-IDLE) = %lu deg (limit=%d), ADC = %ld", 
							m_ulBHTThermostatReading, MS60_BH_MAX_TEMPERATURE_IDLE, lValue);
			CMSLogFileUtility::Instance()->BH_LogStatus(szLog);
		}
		
		SetErrorMessage(szLog);
	}

	return bStatus;
}

BOOL CMS896AStn::IsMS60TempertureOverHeated(CONST ULONG ulTemperature)		//v4.49A18
{
	CString szLog;
	BOOL bStatus = FALSE;

	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (!IsMS60())
	{
		return FALSE;
	}
	if (!CMS896AStn::m_bDBHThermostat)
	{
		return FALSE;
	}


	LONG lValue = CMS896AStn::MotionReadInputADCPort(BHT_THERMOSTAT);
	m_ulBHTThermostatADC		= lValue;
	m_dBHTThermostatReading		= (DOUBLE) (250.0 * lValue / 32767.0);
	m_ulBHTThermostatReading	= (ULONG) m_dBHTThermostatReading;

	if (m_ulBHTThermostatReading > ulTemperature || m_ulBHTThermostatReading == 0)	
	{
		//(*m_psmfSRam)["MS"]["ThermalCriticalError"] = TRUE;
		bStatus = TRUE;
	}

	if (bStatus)
	{
		szLog.Format("BH Temp (OverHeat 2) = %lu deg (limit=%d), ADC = %ld",
						m_ulBHTThermostatReading, ulTemperature, lValue);
		CMSLogFileUtility::Instance()->BH_LogStatus(szLog);
		SetErrorMessage(szLog);
	}

	return bStatus;
}

BOOL CMS896AStn::OpUpdateBHTThermalRMSValues(INT nFrequency, BOOL bLog)
{
	INT nThermalDAC = 0;
	DOUBLE dRMSTime = 0;
	DOUBLE dRmsR = 0;
	DOUBLE dRmsS = 0;
	DOUBLE dRmsT = 0;

	if (!IsMS60())
	{
		return TRUE;
	}
	if (!m_bMS60ThermalCtrl)
	{
		return TRUE;
	}
	if (!m_bEnableMS60ThermalCheck)		//Onlybenabled in IDLE state; disabled in AUTOBOND mode
	{
		return FALSE;
	}

#ifdef NU_MOTION

	if (nFrequency > 0)
	{
		if( (LONG)(GetTime()-m_dMS60ThermalUpdateTime)<(nFrequency*10) )
		{
			Sleep(10);	//v4.54A7
			return TRUE;
		}
	}

	m_dMS60ThermalUpdateTime = GetTime();

//	if(!T_IsPowerOn())
//	{
//		return FALSE;
//	}

	BOOL bStatus = CMS896AStn::MotionGetMotorRMSValues(BH_AXIS_T, 3, dRMSTime, dRmsR, dRmsS, dRmsT, &m_stBHAxis_T);
	if (!bStatus)
	{
HmiMessage("MotionGetMotorRMSValues fails !");
		m_dID = 0;
		m_dIQ = 0;
		nThermalDAC = 0;
		m_nRmsCount = 0;
		m_dRmsRSum = m_dRmsSSum = m_dRmsTSum = 0;
		CMS896AStn::MotionSetMotorThermalCompDAC(BH_AXIS_T, nThermalDAC, &m_stBHAxis_T);
		return FALSE;
	}

	m_dRmsRSum += (dRmsR * dRmsR);
	m_dRmsSSum += (dRmsS * dRmsS);
	m_dRmsTSum += (dRmsT * dRmsT);
	//double Id = 0, Is = 0.05, Iq = 0;
	m_nRmsCount++;

	if (m_nRmsCount >= 5)
	{
		DOUBLE dRrms = sqrt (m_dRmsRSum / 5.0);
		DOUBLE dSrms = sqrt (m_dRmsSSum / 5.0);
		DOUBLE dTrms = sqrt (m_dRmsTSum / 5.0);

	#ifdef NU_MOTION_MS60	//v4.54A3
		
		//v4.53A20	//New CTRL fcn
		IsMS60TempertureOverLoaded(FALSE, TRUE);
		DOUBLE dCurrTemp = (DOUBLE) m_ulBHTThermostatReading;

//		GMP_S32 sDAC = ThermalCompensateCtrl( (GMP_FLOAT) dRrms, 
//											  (GMP_FLOAT) dSrms, 
//											  (GMP_FLOAT) dTrms, 
//											  (GMP_FLOAT) m_unMS60TargetIdleTemperature,	//v4.54A5
//											  (GMP_FLOAT) dCurrTemp);

		GMP_S16 sDAC = 0;
		if (m_pThermalCtrl != NULL)
		{
			m_pThermalCtrl->CalcThermalDACOutput((GMP_FLOAT) dRrms, 
												 (GMP_FLOAT) dSrms, 
												 (GMP_FLOAT) dTrms,
												 (GMP_FLOAT) dCurrTemp,
												 &sDAC,
												 GMP_TRUE);
		}

		nThermalDAC = (INT) sDAC;
		
		if (bLog)
		{
			CString szLog;
			szLog.Format("Rrms,Srms,Trms,  DAC,  Temp,(Max-IDLE) : %.6f,%.6f,%.6f,  %d,  %.2f,(%d)", 
				dRrms, dSrms, dTrms, nThermalDAC, dCurrTemp, m_unMS60TargetIdleTemperature);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);	//v4.54A6
		}
		
	#else
		
		//Calculate Comp DAC
		//if (taskPara->channel->bThermalControl)
		//{
		//m_dIS = 0.226;		//m_dThermalSetValue;
		DOUBLE dIS = m_dIS;		//0.21 //0.23;		//m_dIS;	//0.32;	//m_dThermalSetValue;
		
		if (2.0 / 3.0 * (pow(dRrms, 2) + pow(dSrms, 2) + pow(dTrms, 2)) - m_dID * m_dID < 0)
		{
			m_dIQ = 0;
		}
		else
		{
			m_dIQ = pow((2.0 / 3.0 * (pow (dRrms, 2) + pow(dSrms, 2) + pow(dTrms, 2)) - m_dID*m_dID), 0.5);
		}

		if (dIS * dIS - m_dIQ * m_dIQ < 0)
		{
			m_dID = 0;
		}
		else
		{
			m_dID = pow ((dIS * dIS - m_dIQ * m_dIQ), 0.5);
		}

		if (bLog)
		{
			CString szLog;
			szLog.Format("%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f", 
				dIS, m_dID, m_dIQ, 
				m_dRmsRSum, m_dRmsSSum, m_dRmsTSum, 
				dRrms, dSrms, dTrms); 

			//if (CMSLogFileUtility::Instance()->BPR_IsArm2LogOpen())
			//	CMSLogFileUtility::Instance()->BPR_Arm2Log(szLog);	
			//else
			//CMSLogFileUtility::Instance()->MS_LogOperation("Is-Id-Iq," + szLog);	//v4.49A10
			//CMSLogFileUtility::Instance()->BH_LogStatus("Is-Id-Iq," + szLog);	//v4.53A23
		}

		nThermalDAC = (int)(m_dID * 32767);

	#endif
		
		CMS896AStn::MotionSetMotorThermalCompDAC(BH_AXIS_T, nThermalDAC, &m_stBHAxis_T);
		m_nRmsCount = 0;
		m_dRmsRSum = m_dRmsSSum = m_dRmsTSum = 0;
	}

#endif

	return TRUE;
}
