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
#include "WaferTable.h"

#include "MS896A.h"		// For Timing

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



////////////////////////////////////////////
//	Supporting Functions
////////////////////////////////////////////
BOOL CBondHead::ReadBH1AirFlowValue()
{
	if (m_bMS100DigitalAirFlowSnr)
	{
		LONG lValue = CMS896AStn::MotionReadInputADCPort(BHZ1_ADC_MD);
		lValue = lValue - m_ulBHZ1DAirFlowSetZeroOffset;
		if (lValue < 0)
		{
			lValue = 0;
		}
		m_ulBHZ1DAirFlowValue = (ULONG) lValue;
	}

	return TRUE;
}


BOOL CBondHead::ReadBH2AirFlowValue()
{
	if (m_bMS100DigitalAirFlowSnr)
	{
		LONG lValue = CMS896AStn::MotionReadInputADCPort(BHZ2_ADC_MD);
		lValue = lValue - m_ulBHZ2DAirFlowSetZeroOffset;
		if (lValue < 0)
		{
			lValue = 0;
		}
		m_ulBHZ2DAirFlowValue = (ULONG) lValue;
	}

	return TRUE;
}

BOOL CBondHead::IsMS60EjtHome()
{
	if (!IsMS60())
	{
		return TRUE;
	}

	//v4.59A15
	CMS896AApp* pApp	= (CMS896AApp*) AfxGetApp();
	LONG lEjOffset	= pApp->GetProfileInt(gszPROFILE_SETTING, _T("EJ HOME Offset"), 0);
	if ( (lEjOffset != 0) && labs(lEjOffset) <= 500 )
	{
		return TRUE;
	}

	return !CMS896AStn::MotionReadInputBit(BH_SI_MS60_EJT_HOME);	//v4.55A8
}

BOOL CBondHead::IsMissingDie()
{
	//No BHZ in Mega Da
	return FALSE;

	CString szTemp;
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));

#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return FALSE;
	}
#endif

	// Check the missing die
	//return m_piMissingDie->IsLow();
	if (m_bMS100DigitalAirFlowSnr)		//v4.43T8
	{
		ReadBH1AirFlowValue();
		LONG lBH1KOffsetValue = 0;
		//if ((m_lDAFlowBH1KCount > 0) && (m_lDAFlowBH1KOffset > 0))
		//{
		//	lBH1KOffsetValue = m_lDAFlowBH1KOffsetTotal;
		//}

		//v4.48A33
		//szTemp.Format("BH1 digital MD: Value=%lu, (Thres=%lu, Offset=%lu)", 
		//			m_ulBHZ1DAirFlowValue, m_ulBHZ1DAirFlowThreshold, lBH1KOffsetValue);

		if ( m_ulBHZ1DAirFlowValue > (m_ulBHZ1DAirFlowThreshold - lBH1KOffsetValue) )
		{
//			pWaferTable->m_nMS60CycleCount = 0;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if (!CMS896AStn::MotionReadInputBit(BH_SI_MISSING_DIE) == TRUE)
		{
//			pWaferTable->m_nMS60CycleCount = 0;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		//return !CMS896AStn::MotionReadInputBit(BH_SI_MISSING_DIE); //Matthew 20181219 by Harry Ho
	}
}

BOOL CBondHead::IsMissingDieZ2()
{
	//No BHZ in Mega Da
	return FALSE;

	CString szTemp;
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));

#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return FALSE;
	}
#endif

	// Check the missing die
	if (m_bMS100DigitalAirFlowSnr)		//v4.43T8
	{
		ReadBH2AirFlowValue();
		LONG lBH2KOffsetValue = 0;
		//if ((m_lDAFlowBH2KCount > 0) && (m_lDAFlowBH2KOffset > 0))
		//{
		//	lBH2KOffsetValue = m_lDAFlowBH2KOffsetTotal;
		//}

		//v4.48A33
		//szTemp.Format("BH2 digital MD: Value=%lu, (Thres=%lu, Offset=%lu)", 
		//			m_ulBHZ2DAirFlowValue, m_ulBHZ2DAirFlowThreshold, lBH2KOffsetValue);


		if ( m_ulBHZ2DAirFlowValue > (m_ulBHZ2DAirFlowThreshold - lBH2KOffsetValue) )
		{
//			pWaferTable->m_nMS60CycleCount = 0;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if(!CMS896AStn::MotionReadInputBit(BH_SI_MISSING_DIE_Z2) == TRUE)
		{
//			pWaferTable->m_nMS60CycleCount = 0;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
		//return !CMS896AStn::MotionReadInputBit(BH_SI_MISSING_DIE_Z2);
}

BOOL CBondHead::IsColletJam()
{
	//No BHZ in Mega Da
	return FALSE;
	
	//if (m_piMissingDie == NULL)		//v3.60
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return FALSE;
	}
#endif
	// Check the collet jam

	if (m_bMS100DigitalAirFlowSnr)		//v4.43T13
	{
		ReadBH1AirFlowValue();

		LONG lBH1KOffsetValue = 0;
		if ((m_lDAFlowBH1KCount > 0) && (m_lDAFlowBH1KOffset > 0))
		{
			lBH1KOffsetValue = m_lDAFlowBH1KOffsetTotal;
		}

		if (m_ulBHZ1DAirFlowThresholdCJ > 0)	//v4.50A7
		{
			if ( m_ulBHZ1DAirFlowValue <= (m_ulBHZ1DAirFlowThresholdCJ - lBH1KOffsetValue) )
			{
				return TRUE;	//Collet is jammed!!!
			}
			else
			{
				return FALSE;	//No collet jam, OK.
			}
		}
		else
		{
			if ( m_ulBHZ1DAirFlowValue <= (m_ulBHZ1DAirFlowThreshold - lBH1KOffsetValue) )
			{
				return TRUE;	//Collet is jammed!!!
			}
			else
			{
				return FALSE;	//No collet jam, OK.
			}
		}
	}
	else
	{
		//return m_piMissingDie->IsHigh();
		return CMS896AStn::MotionReadInputBit(BH_SI_MISSING_DIE);
	}
}

BOOL CBondHead::IsColletJamZ2()
{
	//No BHZ in Mega Da
	return FALSE;

#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return FALSE;
	}
#endif
	// Check the collet jam
	if (m_bMS100DigitalAirFlowSnr)		//v4.43T13
	{
		ReadBH2AirFlowValue();

		LONG lBH2KOffsetValue = 0;
		if ((m_lDAFlowBH2KCount > 0) && (m_lDAFlowBH2KOffset > 0))
		{
			lBH2KOffsetValue = m_lDAFlowBH2KOffsetTotal;
		}

		if (m_ulBHZ2DAirFlowThresholdCJ > 0)	//v4.50A7
		{
			if ( m_ulBHZ2DAirFlowValue <= (m_ulBHZ2DAirFlowThresholdCJ - lBH2KOffsetValue) )
			{
				return TRUE;	//Collet is jammed !!
			}
			else
			{
				return FALSE;	//No collet jam, OK
			}
		}
		else
		{
			if ( m_ulBHZ2DAirFlowValue <= (m_ulBHZ2DAirFlowThreshold - lBH2KOffsetValue) )
			{
				return TRUE;	//Collet is jammed !!
			}
			else
			{
				return FALSE;	//No collet jam, OK
			}
		}
	}
	else
	{
		return CMS896AStn::MotionReadInputBit(BH_SI_MISSING_DIE_Z2);
	}
}

BOOL CBondHead::IsBHContact()
{
	return TRUE;

	//if (m_piBHContact == NULL)		//v3.60
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return TRUE;
	}
#endif
	//return m_piBHContact->IsHigh();
	return CMS896AStn::MotionReadInputBit(BH_SI_CONTACT);
}

BOOL CBondHead::IsBHContactZ2()
{
	return TRUE;

#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return TRUE;
	}
#endif

	return CMS896AStn::MotionReadInputBit(BH_SI_CONTACT_Z2);
}

BOOL CBondHead::IsLowPressure()
{
	return FALSE;	
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	//TRUE = ON = Error, FALSE = OK
	return CMS896AStn::MotionReadInputBit(BH_SI_PRESSURE);
}

BOOL CBondHead::IsLowVacuumFlow()
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	//TRUE = ON = Error, FALSE = OK
	return FALSE;
}

BOOL CBondHead::IsLowPressure1()		//v4.59A12	//Renesas MS90
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (!m_bCheckBH1PressureSnr)			//v4.59A26
	{
		return FALSE;
	}
	//TRUE = ON = Error, FALSE = OK
	return CMS896AStn::MotionReadInputBit(BH_SI_PRESSURE1);
}

BOOL CBondHead::IsLowPressure2()		//v4.59A12	//Renesas MS90
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (!m_bCheckBH2PressureSnr)			//v4.59A26
	{
		return FALSE;
	}
	//TRUE = ON = Error, FALSE = OK
	return CMS896AStn::MotionReadInputBit(BH_SI_PRESSURE2);
}

BOOL CBondHead::IsLowPressureEj()		//v4.59A12	//Renesas MS90
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (!m_bCheckEjPressureSnr)			//v4.59A17
	{
		return FALSE;
	}

	//TRUE = ON = Error, FALSE = OK
	return CMS896AStn::MotionReadInputBit(BH_SI_PRESSURE_EJ);
}

BOOL CBondHead::IsInputIonizerErrOn()	//v4.59A12	//Renesas MS90
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	//TRUE = ON = Error, FALSE = OK
	return CMS896AStn::MotionReadInputBit(BH_SI_INPUT_IONIZER);
}

BOOL CBondHead::IsOutputIonizerErrOn()	//v4.59A12	//Renesas MS90
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	//TRUE = ON = Error, FALSE = OK
	return CMS896AStn::MotionReadInputBit(BH_SI_OUTPUT_IONIZER);
}

BOOL CBondHead::IsCatchAndBlowYOn()			//v4.59A12
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	//TRUE = at SAFE pos, FALSE = at PICK pos
	return !CMS896AStn::MotionReadInputBit(BH_SI_CATCHANDBLOW_Y);
}

BOOL CBondHead::IsScopeDown(VOID)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	//TRUE = at SAFE pos, FALSE = at PICK pos
	return !CMS896AStn::MotionReadInputBit(BH_SI_ScopeLevel);
}


BOOL CBondHead::IsCleanColletSafetySensor()
{
	m_bCleanColletSafetySensor_HMI = CMS896AStn::MotionReadInputBit(BH_SI_CleanColletSafetySensor);
	return m_bCleanColletSafetySensor_HMI;
}

//======================================================================================================
//   Main Cover Sensor indluding :
//	Front Cover Sensor, Front Left Cover Sensor, Front Middle Cover Sensor
//	Rear Cover Sensor,  Rear Right Cover Sensor, Left Cover Sensor
//  for SIS, not check Rear Left Cover sensor and Rear Right Cover Sensor
//======================================================================================================
BOOL CBondHead::IsCoverOpen()
{
	return FALSE;
	if (IsFrontCoverOpen() || IsFrontLeftCoverOpen() || IsFrontMiddleCoverOpen() ||
		!IsMSAutoLineMode() && (IsRearLeftCoverOpen() || IsRearRightCoverOpen()) ||
		IsLeftCoverOpen())
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CBondHead::IsMainCoverOpen(const CString& szBitId, const CString& szBitId2)
{
	if (!m_fHardware)
	{
		return FALSE; 
	}	

	if (CMS896AStn::m_bCEMark)	
	{
		BOOL bBit1 = CMS896AStn::MotionReadInputBit(szBitId);
		BOOL bBit2 = CMS896AStn::MotionReadInputBit(szBitId2);
		
		if ((bBit1 == TRUE) && (bBit2 == TRUE))
		{
			return TRUE;    //Assume OPEN if TURE-FALSE reverse state is wrong
		}	
		else if ((bBit1 == FALSE) && (bBit2 == FALSE))
		{
			return TRUE;    //Assume OPEN if TURE-FALSE reverse state is wrong
		}	
		else
		{
			return (bBit1 && (!bBit2));
		}
	}

	if ((CMS896AApp::m_lCheckCoverSnr == 0) || (m_bCheckCoverSensor == FALSE))
	{
		return FALSE;
	}

	return CMS896AStn::MotionReadInputBit(szBitId);
}


BOOL CBondHead::IsFrontCoverOpen()
{
	m_bMainFrontCover_HMI = IsMainCoverOpen(MAIN_SI_FRONT_COVER, MAIN_SI_FRONT_COVER2);

	return m_bMainFrontCover_HMI;
}

BOOL CBondHead::IsFrontLeftCoverOpen()
{
	m_bMainFrontLeftCover_HMI = IsMainCoverOpen(MAIN_SI_FRONT_LEFT_DOOR_COVER, MAIN_SI_FRONT_LEFT_DOOR_COVER2);

    return m_bMainFrontLeftCover_HMI;
}


BOOL CBondHead::IsFrontMiddleCoverOpen()
{
	m_bMainFrontMiddleCover_HMI = IsMainCoverOpen(MAIN_SI_FRONT_MIDDLE_DOOR_COVER, MAIN_SI_FRONT_MIDDLE_DOOR_COVER2);

	return m_bMainFrontMiddleCover_HMI;
}

BOOL CBondHead::IsRearLeftCoverOpen()
{
	m_bMainRearLeftCover_HMI = IsMainCoverOpen(MAIN_SI_REAR_LEFT_DOOR_COVER, MAIN_SI_REAR_LEFT_DOOR_COVER2);

    return m_bMainRearLeftCover_HMI;
}

BOOL CBondHead::IsRearRightCoverOpen()
{
	m_bMainRearRightCover_HMI = IsMainCoverOpen(MAIN_SI_REAR_RIGHT_DOOR_COVER, MAIN_SI_REAR_RIGHT_DOOR_COVER2);

    return m_bMainRearRightCover_HMI;
}

BOOL CBondHead::IsLeftCoverOpen()
{
	m_bMainLeftCover_HMI = IsMainCoverOpen(MAIN_SI_LEFT_DOOR_COVER, MAIN_SI_LEFT_DOOR_COVER2);

    return m_bMainLeftCover_HMI;
}

//=====================================================================================================
VOID CBondHead::SetBHPickVacuum(const BOOL bBHZ2, const BOOL bSet)
{
	if (bBHZ2)			//If BHZ2 towards PICK	
	{
		SetPickVacuumZ2(bSet);	//BHZ2
	}
	else
	{
		SetPickVacuum(bSet);	//BHZ1
	}
}

VOID CBondHead::SetPickVacuum(BOOL bSet)
{
	return;

	if (!m_fHardware)
	{
		return;
	}
	//if (m_poPickVacuum == NULL)		//v3.60
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	//CMS896AStn::MotionSetOutputBit(BH_SO_VACUUM, bSet);
	m_bPickVacuumOn = bSet;
	return;
}

VOID CBondHead::SetPickVacuumZ2(BOOL bSet)
{
	return;

	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	//CMS896AStn::MotionSetOutputBit(BH_SO_VACUUM_Z2, bSet);
	m_bPickVacuumOnZ2 = bSet;
	return;
}


VOID CBondHead::SetBHStrongBlow(const BOOL bBHZ2, const BOOL bSet)
{
	if (bBHZ2)
	{
		SetStrongBlowZ2(bSet);  //BHZ2
	}
	else
	{
		SetStrongBlow(bSet);  //BHZ1
	}
}


VOID CBondHead::SetStrongBlow(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	//CMS896AStn::MotionSetOutputBit(BH_SO_STRONG_BLOW, bSet);
	return;
}

VOID CBondHead::SetStrongBlowZ2(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	//CMS896AStn::MotionSetOutputBit(BH_SO_STRONG_BLOW_Z2, bSet);
	return;
}

VOID CBondHead::SetBlowAndCatchSolY(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(MS90_SO_CATCH_Y_SOL, bSet);
	return;
}

VOID CBondHead::SetBlowAndCatchSolZ(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(MS90_SO_CATCH_Z_SOL, bSet);
	return;
}


VOID CBondHead::SetBlowAndCatchSolBlow(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(MS90_SO_BLOW_SOL, bSet);
	return;
}


VOID CBondHead::SetEjectorCapVacuum(BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableBH)
	{
		return;
	}
#endif
	CMS896AStn::MotionSetOutputBit(BH_SO_EJR_CAP_VACUUM, bSet);
	m_bEjectorCapVacuumOn = bSet;
	return;
}