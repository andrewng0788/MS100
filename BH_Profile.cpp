/////////////////////////////////////////////////////////////////
// BondHead.cpp : Profile function of the CBondHead class
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

////////////////////////////////////////////
//	Bond Arm 
////////////////////////////////////////////

INT CBondHead::T_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;
	return nResult;

	if (m_fHardware && m_bSel_T && !m_bDisableBH)
	{
		try
		{
			if (m_lT_ProfileType == MS896A_OBW_PROFILE)
			{
				
				LONG lSamplingRate;
				// Ultra Bond Head Channel Frequency 8K Hz
				if (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD)
				{
					lSamplingRate = ULTRA_BH_SAMPLE_RATE;
				}
				else	// Org Bond HeadFrequency 2K Hz
				{
					lSamplingRate = HIPEC_SAMPLE_RATE;
				}

				//v3.61
				//m_lPrePickTime_T = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_T, BH_MP_T_OBW_DEFAULT, m_lBHArmMaxTravel, (m_lPrePickPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);		
				//v4.11T5
				m_lPrePickTime_T = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_T, BH_MP_T_OBW_PREPICK, m_lBHArmMaxTravel, (m_lPrePickPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);		

				m_lTime_T = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_T, BH_MP_T_OBW_DEFAULT, m_lBHArmMaxTravel, (m_lBondPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);		
				m_lBondTime_T		= m_lTime_T;
				(*m_psmfSRam)["BondHead"]["T_Time"] = m_lTime_T;
				nResult = gnOK;
			}
			else if (m_lT_ProfileType == MS896A_ILC_PROFILE)
			{
				LONG lSamplingRate;
				// Ultra Bond Head Channel Frequency 8K Hz
				if (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD)
				{
					lSamplingRate = ULTRA_BH_SAMPLE_RATE;
				}
				else	// Org Bond HeadFrequency 2K Hz
				{
					lSamplingRate = HIPEC_SAMPLE_RATE;
				}

				//m_lPrePickTime_T = CMS896AStn::MotionGetFifthOrderProfileTime(BH_AXIS_T, BH_MP_T_ILC_CONTOUR,  FLOAT(m_lPrePickPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);
				m_lPrePickTime_T = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_T, BH_MP_T_OBW_PREPICK, m_lBHArmMaxTravel, (m_lPrePickPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);
				
				if (m_bILCSlowProfile)
				{
					m_lTime_T = CMS896AStn::MotionGetFifthOrderProfileTime(BH_AXIS_T, BH_MP_T_ILC_CONTOUR_SLOW,  FLOAT(m_lBondPos_T - m_lPickPos_T) , lSamplingRate, &m_stBHAxis_T);
				}
				else
				{
					m_lTime_T = CMS896AStn::MotionGetFifthOrderProfileTime(BH_AXIS_T, BH_MP_T_ILC_CONTOUR,  FLOAT(m_lBondPos_T - m_lPickPos_T) , lSamplingRate, &m_stBHAxis_T);
				}
				m_lBondTime_T		= m_lTime_T;
				(*m_psmfSRam)["BondHead"]["T_Time"] = m_lTime_T;
				nResult = gnOK;

				//v4.53A14
				CString szProfile	= "";
				switch (nProfile)
				{
				case PREPICK_PROF:
					szProfile = BH_MP_T_OBW_PREPICK;
					break;
				case CLEAN_COLLET_PROF:
					szProfile = BH_MP_T_OBW_CLEANCOLLET;
					break;
				case SETUP_PROF:
					szProfile = BH_MP_T_OBW_SETUP;
					break;
				case NORMAL_PROF:
				default:
					szProfile = BH_MP_T_OBW_DEFAULT;
					break;
				}
				if (szProfile != "")
				{
					CMS896AStn::MotionSelectProfile(BH_AXIS_T, szProfile, &m_stBHAxis_T);
				}

			}
			else
			{
				CString szProfile	= "";

				switch (nProfile)
				{
					case NORMAL_PROF:
						szProfile = BH_MP_T_NORMAL;
						break;

					case CLEAN_COLLET_PROF:
						szProfile = BH_MP_T_CLEANCOLLET;
						break;
				}

				if (szProfile != "")
				{
					
					CMS896AStn::MotionSelectProfile(BH_AXIS_T, szProfile, &m_stBHAxis_T);

					m_lPrePickTime_T = CMS896AStn::MotionGetProfileTime(BH_AXIS_T, szProfile, (m_lPrePickPos_T - m_lPickPos_T), (m_lPrePickPos_T - m_lPickPos_T), HIPEC_SAMPLE_RATE, &m_stBHAxis_T);
					m_lBondTime_T = CMS896AStn::MotionGetProfileTime(BH_AXIS_T, szProfile, (m_lBondPos_T - m_lPickPos_T), (m_lBondPos_T - m_lPickPos_T), HIPEC_SAMPLE_RATE, &m_stBHAxis_T);

					m_lTime_T = m_lBondTime_T;
					(*m_psmfSRam)["BondHead"]["T_Time"] = m_lTime_T;
					nResult = gnOK;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		m_lPrePickTime_T	= 50;
		m_lBondTime_T		= 50;
		m_lTime_T			= 50;
		nResult = gnOK;
	}

	return nResult;
}


INT CBondHead::T_SelectObwProfile(LONG lRelDistance, CString szUserProfile)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= "";


	if (m_fHardware && m_bSel_T && !m_bDisableBH)
	{
		try
		{
			if (abs(lRelDistance) == abs(m_lPrePickPos_T - m_lPickPos_T))
			{
				//Use Prepick profile
				//szProfile = "mpfBondHeadTObwPrePick";
				szProfile = BH_MP_T_OBW_PREPICK;
			}
			else if (abs(lRelDistance) == abs(m_lPrePickPos_T - m_lBondPos_T))
			{
				szProfile = BH_MP_T_OBW_PREPICK;
			}
			else if (abs(lRelDistance) <= 20000)
			{
				//Use 1000 travel distance to calcualte this profile
				//szProfile = "mpfBondHeadTObwSetup";
				szProfile = BH_MP_T_OBW_SETUP;
			}
			else if ((abs(lRelDistance) > 20000) && (abs(lRelDistance) <= 75000))
			{
				//Use 50000 travel distance to calcualte this profile
				//szProfile = "mpfBondHeadTObwCleanCollet";
				szProfile = BH_MP_T_OBW_CLEANCOLLET;
			}
			else if (abs(lRelDistance) > 75000)
			{
				//szProfile = "mpfBondHeadTObwDefault";
				szProfile = BH_MP_T_OBW_DEFAULT;
			}

			if (!szUserProfile.IsEmpty())
			{
				szProfile = szUserProfile;
			}
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_T, szProfile, &m_stBHAxis_T);
			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}


INT CBondHead::T_UpdatePrePickProfile(LONG lRelDistance)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= "";

	if (m_fHardware && m_bSel_T && !m_bDisableBH)
	{
		try
		{
			if (m_lT_ProfileType == MS896A_OBW_PROFILE)
			{
				
				LONG lSamplingRate;
				// Ultra Bond Head Channel Frequency 8K Hz
				if (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD)
				{
					lSamplingRate = ULTRA_BH_SAMPLE_RATE;
				}
				else	// Org Bond HeadFrequency 2K Hz
				{
					lSamplingRate = HIPEC_SAMPLE_RATE;
				}

				DOUBLE dAccMax;
				DOUBLE dDecMax;

				m_lBondTime_T = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_T, BH_MP_T_OBW_DEFAULT, m_lBHArmMaxTravel, (m_lBondPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);

				//v4.59A25	
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				BOOL bEnableBHUplookPr	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
				if (IsMS90() && bEnableBHUplookPr)
				{
					//v4.59A25
					//Do not update T PREPICK profile when Uplook PR sequence is used
				}
				else
				{
					//Update Prepick profile 
					CMS896AStn::MotionCalculateObwProfileData(BH_AXIS_T, BH_MP_T_OBW_DEFAULT, m_lBondTime_T, lRelDistance, lSamplingRate, dAccMax, dDecMax, &m_stBHAxis_T);
					CMS896AStn::MotionUpdateObwProfile(BH_AXIS_T, BH_MP_T_OBW_PREPICK, 0, dAccMax, dDecMax, 0, &m_stBHAxis_T);
				}

				m_lPrePickTime_T = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_T, BH_MP_T_OBW_PREPICK, lRelDistance, (m_lPrePickPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);

				nResult = gnOK;
			}
			else if (m_lT_ProfileType == MS896A_ILC_PROFILE)
			{
				LONG lSamplingRate;
				// Ultra Bond Head Channel Frequency 8K Hz
				if (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD)
				{
					lSamplingRate = ULTRA_BH_SAMPLE_RATE;
				}
				else	// Org Bond HeadFrequency 2K Hz
				{
					lSamplingRate = HIPEC_SAMPLE_RATE;
				}

				m_lPrePickTime_T = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_T, BH_MP_T_OBW_PREPICK, m_lBHArmMaxTravel, (m_lPrePickPos_T - m_lPickPos_T), lSamplingRate, &m_stBHAxis_T);
				
				if (m_bILCSlowProfile)
				{
					m_lBondTime_T = CMS896AStn::MotionGetFifthOrderProfileTime(BH_AXIS_T, BH_MP_T_ILC_CONTOUR_SLOW, FLOAT (m_lBondPos_T - m_lPickPos_T) , lSamplingRate, &m_stBHAxis_T);
				}
				else
				{
					m_lBondTime_T = CMS896AStn::MotionGetFifthOrderProfileTime(BH_AXIS_T, BH_MP_T_ILC_CONTOUR, FLOAT (m_lBondPos_T - m_lPickPos_T) , lSamplingRate, &m_stBHAxis_T);	
				}
				nResult = gnOK;
			}

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	//CheckResult(nResult, _T("BondArm Axis - Select Profile"));
	return nResult;
}


INT CBondHead::T_SelectControl(LONG lRelDistance, LONG lMoveMode)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

#ifdef NU_MOTION
	if (m_fHardware && m_bSel_T && !m_bDisableBH)
	{
		try
		{
			if (lMoveMode == BH_OBW_MV)
			{
				//v4.53A25	//MS90 Pre-Pick wrong control debugging
				if (lRelDistance >= 0)
				{
					szNuControlParaID = m_stBHAxis_T.m_stControl[PL_DYNAMIC].m_szID;
				}
				else
				{
					szNuControlParaID = m_stBHAxis_T.m_stControl[PL_DYNAMIC1].m_szID;
				}

				if (szNuControlParaID != "")
				{
					CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_DYNAMIC_CONTROL_OBW_T, &m_stBHAxis_T, szNuControlParaID);
				}
/*
				// prepick to pick, pick to prepick, bond to prepick & prepick to bond
				if (abs(lRelDistance) == abs(m_lPickPos_T - m_lPrePickPos_T) || abs(lRelDistance) == abs(m_lBondPos_T - m_lPrePickPos_T))
				{
					szNuControlParaID = m_stBHAxis_T.m_stControl[PL_DYNAMIC2].m_szID;
					
					// make backward compatabile
					if (szNuControlParaID == "")
					{
						//Use different Control Parameter for Forward and Backward Motion
						if (lRelDistance >= 0)		//v4.47T8
						{
							szNuControlParaID = m_stBHAxis_T.m_stControl[PL_DYNAMIC].m_szID;
						}
						else if (lRelDistance < 0)
						{
							szNuControlParaID = m_stBHAxis_T.m_stControl[PL_DYNAMIC1].m_szID;
						}	
					}

					CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_DYNAMIC_CONTROL_OBW_T, &m_stBHAxis_T, szNuControlParaID);
				}
				// pick to bond & bond to pick
				else
				{
					//Use different Control Parameter for Forward and Backward Motion
					if (lRelDistance >= 0)			//v4.47T8
					{
						szNuControlParaID = m_stBHAxis_T.m_stControl[PL_DYNAMIC].m_szID;
						CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_DYNAMIC_CONTROL_OBW_T, &m_stBHAxis_T, szNuControlParaID);
					}
					else if (lRelDistance < 0)
					{
						szNuControlParaID = m_stBHAxis_T.m_stControl[PL_DYNAMIC1].m_szID;
						CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_DYNAMIC_CONTROL_OBW_T, &m_stBHAxis_T, szNuControlParaID);
					}
				}
*/
			}
			else
			{
				szNuControlParaID = m_stBHAxis_T.m_stControl[PL_ILC].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_T, BH_ILC_CONTROL_T, &m_stBHAxis_T, szNuControlParaID);
			}
		
			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_T, &m_stBHAxis_T);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	CheckResult(nResult, _T("BondArm Axis - Select Control"));
#endif
	return nResult;
}


////////////////////////////////////////////
//	Bond Head 
////////////////////////////////////////////

INT CBondHead::Z_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;
	return nResult;

	if (m_fHardware && m_bSel_Z && !m_bDisableBH)	//v3.60
	{
		LONG lSwingPickZ1 = m_lSwingLevel_Z - m_lPickLevel_Z;
		LONG lSwingBondZ1 = m_lSwingLevel_Z - m_lBondLevel_Z;
		LONG lSwingPickZ2 = m_lSwingLevel_Z2 - m_lPickLevel_Z2;
		LONG lSwingBondZ2 = m_lSwingLevel_Z2 - m_lBondLevel_Z2;
		try
		{
			if (m_lZ_ProfileType == MS896A_OBW_PROFILE)
			{
				LONG lSamplingRate;
				// Ultra Bond Head Channel Frequency 8K Hz
				if (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD)
				{
					lSamplingRate = ULTRA_BH_SAMPLE_RATE;
				}
				else	// Org Bond HeadFrequency 2K Hz
				{
					lSamplingRate = HIPEC_SAMPLE_RATE;
				}

				if (abs(lSwingPickZ1) > m_lSwingOffset_Z)
				{		
					m_lPickTime_Z	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_DEFAULT, lSwingPickZ1, lSwingPickZ1, lSamplingRate, &m_stBHAxis_Z);
					if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
						m_lPickUpTime_Z	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_UP,  lSwingPickZ1, lSwingPickZ1, lSamplingRate, &m_stBHAxis_Z);
					else
						m_lPickUpTime_Z = m_lPickTime_Z;
				}
				else
				{
					m_lPickTime_Z	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_DEFAULT, m_lSwingOffset_Z, lSwingPickZ1, lSamplingRate, &m_stBHAxis_Z);
					if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
						m_lPickUpTime_Z = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_UP,  m_lSwingOffset_Z, lSwingPickZ1, lSamplingRate, &m_stBHAxis_Z);
					else
						m_lPickUpTime_Z = m_lPickTime_Z;
				}

				//v4.51A6
				if (m_bMS60EjElevator)
				{

				}

				if (m_bIsArm2Exist == TRUE)
				{
					if (m_bSel_Z2)
					{
						if (abs(lSwingPickZ2) > m_lSwingOffset_Z)
						{		
							m_lPickTime_Z2	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_DEFAULT, lSwingPickZ2, lSwingPickZ2, lSamplingRate, &m_stBHAxis_Z2);
							if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
								m_lPickUpTime_Z2 = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_UP,	lSwingPickZ2, lSwingPickZ2, lSamplingRate, &m_stBHAxis_Z2);
							else
								m_lPickUpTime_Z2 = m_lPickTime_Z2;
						}
						else
						{
							m_lPickTime_Z2	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_DEFAULT, m_lSwingOffset_Z2, lSwingPickZ2, lSamplingRate, &m_stBHAxis_Z2);
							if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
								m_lPickUpTime_Z2 = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_UP, m_lSwingOffset_Z2, lSwingPickZ2, lSamplingRate, &m_stBHAxis_Z2);
							else
								m_lPickUpTime_Z2 = m_lPickTime_Z2;
						}

						//v4.51A6
						if (m_bMS60EjElevator)
						{

						}
					}
					else
					{
						m_lPickUpTime_Z2 = 0;
					}
				}


				if (abs(lSwingBondZ1) > m_lSwingOffset_Z)
				{	
					m_lBondTime_Z	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_BOND_DEFAULT, lSwingBondZ1, lSwingBondZ1, lSamplingRate, &m_stBHAxis_Z);
					if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
						m_lBondUpTime_Z = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_UP,	   lSwingBondZ1, lSwingBondZ1, lSamplingRate, &m_stBHAxis_Z);
					else
						m_lBondUpTime_Z = m_lBondTime_Z;
				}
				else
				{
					m_lBondTime_Z	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_BOND_DEFAULT, m_lSwingOffset_Z, lSwingBondZ1, lSamplingRate, &m_stBHAxis_Z);
					if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
						m_lBondUpTime_Z = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z, BH_MP_Z_OBW_UP,	   m_lSwingOffset_Z, lSwingBondZ1, lSamplingRate, &m_stBHAxis_Z);
					else
						m_lBondUpTime_Z = m_lBondTime_Z;
				}

				if (m_bIsArm2Exist == TRUE)
				{
					if (m_bSel_Z2)
					{
						if (abs(lSwingBondZ2) > m_lSwingOffset_Z2)
						{	
							m_lBondTime_Z2	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_BOND_DEFAULT, lSwingBondZ2, lSwingBondZ2, lSamplingRate, &m_stBHAxis_Z2);
							if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
								m_lBondUpTime_Z2 = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_UP,	     lSwingBondZ2, lSwingBondZ2, lSamplingRate, &m_stBHAxis_Z2);
							else
								m_lBondUpTime_Z2 = m_lBondTime_Z2;
						}
						else
						{
							m_lBondTime_Z2	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_BOND_DEFAULT, m_lSwingOffset_Z2, lSwingBondZ2, lSamplingRate, &m_stBHAxis_Z2);
							if (CMS896AApp::m_lCycleSpeedMode > 0)		//v3.61
								m_lBondUpTime_Z2 = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_Z2, BH_MP_Z2_OBW_UP,	     m_lSwingOffset_Z2, lSwingBondZ2, lSamplingRate, &m_stBHAxis_Z2);
							else
								m_lBondUpTime_Z2 = m_lBondTime_Z2;
						}
					}
					else
					{
						m_lBondUpTime_Z2 = 0;
					}
				}

				(*m_psmfSRam)["BondHead"]["Z_PickTime"]		= m_lPickTime_Z;
				(*m_psmfSRam)["BondHead"]["Z_BondTime"]		= m_lBondTime_Z;
				(*m_psmfSRam)["BondHead"]["Z2_PickTime"]	= m_lPickTime_Z2;
				(*m_psmfSRam)["BondHead"]["Z2_BondTime"]	= m_lBondTime_Z2;
				nResult = gnOK;
			}
			else
			{
				CString szProfile	= "";

				switch (nProfile)
				{
					case NORMAL_PROF:
						szProfile = BH_MP_Z_NORMAL;
						break;
				}

				if (szProfile != "")
				{
					CMS896AStn::MotionSelectProfile(BH_AXIS_Z, szProfile, &m_stBHAxis_Z);

					m_lPickTime_Z = CMS896AStn::MotionGetProfileTime(BH_AXIS_Z, szProfile, lSwingPickZ1, lSwingPickZ1, HIPEC_SAMPLE_RATE, &m_stBHAxis_Z);
					m_lBondTime_Z = CMS896AStn::MotionGetProfileTime(BH_AXIS_Z, szProfile, lSwingBondZ1, lSwingBondZ1, HIPEC_SAMPLE_RATE, &m_stBHAxis_Z);

					(*m_psmfSRam)["BondHead"]["Z_PickTime"] = m_lPickTime_Z;
					(*m_psmfSRam)["BondHead"]["Z_BondTime"] = m_lBondTime_Z;
					nResult = gnOK;
				}
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			nResult = gnNOTOK;
		}	
	}
	else
	{
#ifdef NU_MOTION		//v3.67T3
		m_lPickTime_Z		= 30;
		m_lBondTime_Z		= 30;
		m_lPickUpTime_Z		= 30;
		m_lBondUpTime_Z		= 30;
		m_lPickTime_Z2		= 30;
		m_lBondTime_Z2		= 30;
		m_lPickUpTime_Z2	= 30;
		m_lBondUpTime_Z2	= 30;
#endif
		nResult = gnOK;
	}
	
	CheckResult(nResult, _T("BondHead Axis - Select Profile"));
	return nResult;
}


INT CBondHead::Z_SelectObwProfile(LONG lRelDistance, CONST BOOL bAtBond)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= "";

	//v4.25
	LONG lSwingOffsetZ = 4000;	
	if (CMS896AApp::m_lCycleSpeedMode >= 4)
		lSwingOffsetZ = 3500;

	if (m_fHardware && m_bSel_Z && !m_bDisableBH)
	{
		if ((lRelDistance>0) && abs(lRelDistance) >= lSwingOffsetZ)	
		{
			//** Move UP with +4000 steps **//
			if (CMS896AApp::m_lCycleSpeedMode > 0)	
				szProfile = BH_MP_Z_OBW_UP;
			else
				szProfile = BH_MP_Z_OBW_DEFAULT;
		}
		else if (abs(lRelDistance) < lSwingOffsetZ)
		{
			//** Small dist MOVE Z **//
			szProfile = BH_MP_Z_OBW_SETUP;
		}
		else if (abs(lRelDistance) >= lSwingOffsetZ)
		{
			//** Move DONW with +4000 steps **//
			if (bAtBond)
				szProfile = BH_MP_Z_OBW_BOND_DEFAULT;
			else
				szProfile = BH_MP_Z_OBW_DEFAULT;
		}

		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_Z, szProfile, &m_stBHAxis_Z);
			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	//CheckResult(nResult, _T("BondHead Axis - Select Profile"));
	return nResult;
}


INT CBondHead::Z2_SelectObwProfile(LONG lRelDistance, CONST BOOL bAtBond)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= "";
	
	//v4.25
	LONG lSwingOffsetZ = 4000;	
	if (CMS896AApp::m_lCycleSpeedMode >= 4)
		lSwingOffsetZ = 3500;

	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)
	{
		if ((lRelDistance>0) && abs(lRelDistance) >= lSwingOffsetZ)	
		{
			//** Move UP with +4000 steps **//
			if (CMS896AApp::m_lCycleSpeedMode > 0)		
				szProfile = BH_MP_Z2_OBW_UP;
			else
				szProfile = BH_MP_Z2_OBW_DEFAULT;
		}
		else if (abs(lRelDistance) < lSwingOffsetZ)
		{
			//** Small dist MOVE Z **//
			szProfile = BH_MP_Z2_OBW_SETUP;
		}
		else if (abs(lRelDistance) >= lSwingOffsetZ)
		{
			//** Move DOWN with +4000 steps **//
			if (bAtBond)
				szProfile = BH_MP_Z2_OBW_BOND_DEFAULT;	
			else
				szProfile = BH_MP_Z2_OBW_DEFAULT;
		}

		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_Z2, szProfile, &m_stBHAxis_Z2);
			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}


INT	CBondHead::Z_SelectControl(LONG lRelDistance, CONST BOOL bAtBond)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

#ifdef NU_MOTION
	if (m_fHardware && m_bSel_Z && !m_bDisableBH)
	{
		try
		{
			if (bAtBond)
			{
				szNuControlParaID = m_stBHAxis_Z.m_stControl[PL_DYNAMIC1].m_szID;
				// make backward compatable
				if (szNuControlParaID == "")
				{
					szNuControlParaID = m_stBHAxis_Z.m_stControl[PL_DYNAMIC].m_szID;
					CMSLogFileUtility::Instance()->MS_LogOperation("No Control Z");
				}

				CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_DYNAMIC_CONTROL_OBW_Z, &m_stBHAxis_Z, szNuControlParaID);
			}
			else
			{
				szNuControlParaID = m_stBHAxis_Z.m_stControl[PL_DYNAMIC].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_Z, BH_DYNAMIC_CONTROL_OBW_Z, &m_stBHAxis_Z, szNuControlParaID);
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	CheckResult(nResult, _T("WTY Axis - Select Control"));
#endif
	return nResult;
}

INT	 CBondHead::Z2_SelectControl(LONG lRelDistance, CONST BOOL bAtBond)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

#ifdef NU_MOTION
	if (m_fHardware && m_bSel_Z2 && !m_bDisableBH)
	{
		try
		{
			if (bAtBond)
			{
				szNuControlParaID = m_stBHAxis_Z2.m_stControl[PL_DYNAMIC1].m_szID;
				
				// make backward compatable
				if (szNuControlParaID == "")
				{
					szNuControlParaID = m_stBHAxis_Z2.m_stControl[PL_DYNAMIC].m_szID;
					//AfxMessageBox("No Control Z2", MB_SYSTEMMODAL);
				}

				CMS896AStn::MotionSelectControlParam(BH_AXIS_Z2, BH_DYNAMIC_CONTROL_OBW_Z2, &m_stBHAxis_Z2, szNuControlParaID);
			}
			else
			{
				szNuControlParaID = m_stBHAxis_Z2.m_stControl[PL_DYNAMIC].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_Z2, BH_DYNAMIC_CONTROL_OBW_Z2, &m_stBHAxis_Z2, szNuControlParaID);
			}
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_Z2, &m_stBHAxis_Z2);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	CheckResult(nResult, _T("WTY Axis - Select Control"));
#endif
	return nResult;
}

////////////////////////////////////////////
//	Ejector 
////////////////////////////////////////////

INT CBondHead::Ej_Profile(INT nProfile)
{
	INT nResult	= gnNOTOK;

	//andrewng 2020-0608
	BOOL bEjUseContactLvl = FALSE;
	if ( !m_bEnableSyncMotionViaEjCmd && 
			(m_lContactLevel_Ej > m_lStandbyLevel_Ej) && (m_lContactLevel_Ej <= m_lEjectLevel_Ej) )
	{
		//lEjectorLvl		= m_lContactLevel_Ej;
		bEjUseContactLvl	= TRUE;
	}

	if (m_fHardware && m_bSel_Ej && !m_bDisableBH)
	{
		CString szProfile	= "";

		try
		{
			if (m_lE_ProfileType == MS896A_OBW_PROFILE)
			{
				LONG lSamplingRate = ULTRA_BH_SAMPLE_RATE;

				if (bEjUseContactLvl)	//andrewng 2020-0608
				{
					m_lTime_Ej = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ, BH_MP_E_OBW_DEFAULT, m_lMinEjectOffset, (m_lContactLevel_Ej - m_lStandbyLevel_Ej), lSamplingRate, &m_stBHAxis_Ej);
				}
				else
				{
					m_lTime_Ej = CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ, BH_MP_E_OBW_DEFAULT, m_lMinEjectOffset, (m_lEjectLevel_Ej - m_lStandbyLevel_Ej), lSamplingRate, &m_stBHAxis_Ej);
				}
				m_lSlowUpTime_Ej			= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ, BH_MP_E_OBW_SLOW, m_lMinEjectOffset, (m_lEjectLevel_Ej - m_lStandbyLevel_Ej), lSamplingRate, &m_stBHAxis_Ej);
				m_lDnTime_Ej				= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ, BH_MP_E_OBW_DOWN, m_lMinEjectOffset, (m_lEjectLevel_Ej - m_lStandbyLevel_Ej), lSamplingRate, &m_stBHAxis_Ej);

				//andrewng //2020-0601
				LONG lDnOffset_BIN_EJ		= m_lUpLevel_Bin_EJ_Pin - m_lStandby_Bin_EJ_Pin;
				m_lTime_BinEj				= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJ, BH_MP_BIN_E_OBW_DEFAULT, lDnOffset_BIN_EJ, lDnOffset_BIN_EJ, lSamplingRate, &m_stBHAxis_BinEj);
				//LONG lDnOffset_BIN_EJ		= m_lUpLevel_Bin_EJ_Cap - m_lStandby_Bin_EJ_Cap;
				//m_lTime_BinEj				= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJCAP, BH_MP_BIN_E_CAP_OBW_DEFAULT, lDnOffset_BIN_EJ, lDnOffset_BIN_EJ, lSamplingRate, &m_stBHAxis_BinEjCap);
				
				//andrewng //2020-0601
				lDnOffset_BIN_EJ			= m_lStandby_Bin_EJ_Pin - m_lUpLevel_Bin_EJ_Pin;
				m_lDnTime_BinEj				= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJ, BH_MP_BIN_E_OBW_DOWN, lDnOffset_BIN_EJ, lDnOffset_BIN_EJ, lSamplingRate, &m_stBHAxis_BinEj);
				//lDnOffset_BIN_EJ			=  m_lStandby_Bin_EJ_Cap - m_lUpLevel_Bin_EJ_Cap;
				//m_lDnTime_BinEj			= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJCAP, BH_MP_BIN_E_CAP_OBW_DOWN, lDnOffset_BIN_EJ, lDnOffset_BIN_EJ, lSamplingRate, &m_stBHAxis_BinEjCap);

				m_lUpTime_Ej_Cap			= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJCAP, BH_MP_E_OBW_DEFAULT, m_lDnOffset_EJ_Cap, m_lDnOffset_EJ_Cap, lSamplingRate, &m_stBHAxis_EjCap);
				m_lDnTime_Ej_Cap			= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJCAP, BH_MP_E_OBW_DOWN, m_lDnOffset_EJ_Cap, m_lDnOffset_EJ_Cap, lSamplingRate, &m_stBHAxis_EjCap);
				
				LONG lDnOffset_EJ_Theta		= m_lUpLevel_EJ_Theta - m_lStandby_EJ_Theta;
				m_lUpTime_Ej_Theta			= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ_T, BH_MP_E_T_OBW_DEFAULT, lDnOffset_EJ_Theta, lDnOffset_EJ_Theta, lSamplingRate, &m_stBHAxis_EjT);
				lDnOffset_EJ_Theta			= m_lStandby_EJ_Theta - m_lUpLevel_EJ_Theta;
				m_lDnTime_Ej_Theta			= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ_T, BH_MP_E_T_OBW_DOWN, lDnOffset_EJ_Theta, lDnOffset_EJ_Theta, lSamplingRate, &m_stBHAxis_EjT);

				LONG lDnOffset_BIN_EJ_Theta	= m_lUpLevel_Bin_EJ_Theta - m_lStandby_Bin_EJ_Theta;
				m_lUpTime_BinEj_Theta		= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJ_T, BH_MP_BIN_E_T_OBW_DEFAULT, lDnOffset_BIN_EJ_Theta, lDnOffset_BIN_EJ_Theta, lSamplingRate, &m_stBHAxis_BinEjT);
				lDnOffset_BIN_EJ_Theta		=  m_lStandby_Bin_EJ_Theta - m_lUpLevel_Bin_EJ_Theta;
				m_lDnTime_BinEj_Theta		= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_BIN_EJ_T, BH_MP_BIN_E_T_OBW_DOWN, lDnOffset_BIN_EJ_Theta, lDnOffset_BIN_EJ_Theta, lSamplingRate, &m_stBHAxis_BinEjT);

				(*m_psmfSRam)["BondHead"]["Ej_Time"]	= m_lTime_Ej;
				(*m_psmfSRam)["BondHead"]["Ej_DnTime"]	= m_lDnTime_Ej;		//v4.43T5
				
				nResult = gnOK;
			}
			else
			{
				switch (nProfile)
				{
					case NORMAL_PROF:
						//szProfile = "mpfEjectorNormal";
						szProfile = BH_MP_E_NORMAL;
						break;
				}

				if (szProfile != "")
				{
					CMS896AStn::MotionSelectProfile(BH_AXIS_EJ, szProfile, &m_stBHAxis_Ej);

					m_lTime_Ej = CMS896AStn::MotionGetProfileTime(BH_AXIS_EJ, szProfile, (m_lEjectLevel_Ej - m_lStandbyLevel_Ej), (m_lEjectLevel_Ej - m_lStandbyLevel_Ej), HIPEC_SAMPLE_RATE, &m_stBHAxis_Ej);
					(*m_psmfSRam)["BondHead"]["Ej_Time"] = m_lTime_Ej;
					nResult = gnOK;
				}			
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			nResult = gnNOTOK;
		}
	}
	else
	{
		m_lUpTime_Ej_Cap		= 10;
		m_lDnTime_Ej_Cap		= 10;
		m_lTime_Ej				= 10;
		m_lDnTime_Ej			= 10;
		m_lTime_BinEj			= 10;
		m_lDnTime_BinEj			= 10;
		m_lUpTime_Ej_Theta		= 10;
		m_lDnTime_Ej_Theta		= 10;
		m_lUpTime_BinEj_Theta	= 10;
		m_lDnTime_BinEj_Theta	= 10;
		nResult = gnOK;
	}

	return nResult;
}


LONG CBondHead::CalculateEjQueryTime(const LONG lMaxTravelDist, const LONG lQueryDist)
{
	LONG lResult = 0;
	LONG lSamplingRate = ULTRA_BH_SAMPLE_RATE;
	CString szProfileID = m_stBHAxis_Ej.m_stMoveProfile[m_stBHAxis_Ej.m_ucMoveProfileID].m_szID;
	lResult		= 1 + CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ, szProfileID, lMaxTravelDist, lQueryDist, lSamplingRate, &m_stBHAxis_Ej);
	return lResult;
}


LONG CBondHead::CalculateEjTime(LONG lDistance)
{
	LONG lResult = 0;
	LONG lSamplingRate = ULTRA_BH_SAMPLE_RATE;

	lResult		= 1 + CMS896AStn::MotionGetObwProfileTime(BH_AXIS_EJ, BH_MP_E_OBW_DEFAULT, m_lMinEjectOffset, (lDistance - m_lStandbyLevel_Ej), lSamplingRate, &m_stBHAxis_Ej);
	return lResult;
}

INT CBondHead::Ej_SelectObwProfile(LONG lRelDistance)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= BH_MP_E_OBW_DEFAULT;				//Default Rel-Dist = 2100 (spec)

	if (m_fHardware && !m_bDisableBH)
	{
		//if (labs(lRelDistance) <= 500)
		//{
		//	szProfile = BH_MP_E_OBW_DEFAULT;				//andrewng //2020-0605
		//}
		//else 
		if (lRelDistance < 0)	//Moves back to STANDBY	level
		{
			szProfile = BH_MP_E_OBW_DOWN;
			//if (lRelDistance >= -1600)
			//{
				//szProfile = BH_MP_E_OBW_SLOW_DOWN;		//NVC will use it later
			//}
		}
		else	//Moves to UP level
		{
			szProfile = BH_MP_E_OBW_DEFAULT;
			//if (lRelDistance <= 1600)
			//{
				//szProfile = BH_MP_E_OBW_SLOW;				//NVC will use it later
			//}
		}

		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_EJ, szProfile, &m_stBHAxis_Ej);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			//AfxMessageBox(szProfile);
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

INT CBondHead::BinEj_SelectObwProfile(LONG lRelDistance)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= BH_MP_BIN_E_OBW_DEFAULT;	//BH_MP_BIN_E_OBW_DOWN;		//BH_MP_BIN_E_OBW_DEFAULT;	//andrewng //2020-0602

	if (m_fHardware && !m_bDisableBH)
	{
		//if (labs(lRelDistance) <= 500)
		//{
		//	szProfile = BH_MP_BIN_E_OBW_DEFAULT;	//andrewng //2020-0605
		//}
		//else 
		if (lRelDistance < 0)		//Move Towards PICK/BOND level
		{
			szProfile = BH_MP_BIN_E_OBW_DEFAULT;	//BH_MP_BIN_E_OBW_DOWN;		//andrewng //2020-0602
			//if (lRelDistance >= -1600)
			//{
				//szProfile = BH_MP_BIN_E_OBW_SLOW_DOWN;			//NVC will use it later
			//}
		}
		else	//Move back to STANDBY level
		{
			szProfile = BH_MP_BIN_E_OBW_DOWN;
			//if (lRelDistance <= 1600)
			//{
			//szProfile = BH_MP_BIN_E_OBW_SLOW;			//NVC will use it later
			//}
		}

		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_BIN_EJ, szProfile, &m_stBHAxis_BinEj);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			//AfxMessageBox(szProfile);
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}


INT CBondHead::BinEjT_SelectControl(LONG lRelDistance)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

	if (m_fHardware && !m_bDisableBH)
	{
		try
		{
			//Use different Control Parameter for Up and Down
			if (lRelDistance > 0)
			{
				szNuControlParaID = m_stBHAxis_BinEjT.m_stControl[PL_DYNAMIC].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_BIN_EJ_T, BIN_EJ_THETA_DYNAMIC_CONTROL_OBW , &m_stBHAxis_BinEjT, szNuControlParaID);
			}
			else if (lRelDistance < 0)
			{
				szNuControlParaID = m_stBHAxis_BinEjT.m_stControl[PL_DYNAMIC1].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_BIN_EJ_T, BIN_EJ_THETA_DYNAMIC_CONTROL_OBW , &m_stBHAxis_BinEjT, szNuControlParaID);
			}

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;	
}


INT CBondHead::EjT_SelectControl(LONG lRelDistance)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

	if (m_fHardware && !m_bDisableBH)
	{
		try
		{
			//Use different Control Parameter for Up and Down
			if (lRelDistance > 0)
			{
				szNuControlParaID = m_stBHAxis_EjT.m_stControl[PL_DYNAMIC].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_EJ_T, EJ_THETA_DYNAMIC_CONTROL_OBW , &m_stBHAxis_EjT, szNuControlParaID);
			}
			else if (lRelDistance < 0)
			{
				szNuControlParaID = m_stBHAxis_EjT.m_stControl[PL_DYNAMIC1].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_EJ_T, EJ_THETA_DYNAMIC_CONTROL_OBW , &m_stBHAxis_EjT, szNuControlParaID);
			}

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;	
}


INT CBondHead::BinEj_SelectControl(LONG lRelDistance, BOOL bUseShortTravel)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

	if (m_fHardware && !m_bDisableBH)
	{
		try
		{
			if (bUseShortTravel)	//andrewng //2020-0605
			{
				szNuControlParaID = m_stBHAxis_BinEj.m_stControl[PL_DYNAMIC1].m_szID;
			}
			else
			{
				szNuControlParaID = m_stBHAxis_BinEj.m_stControl[PL_DYNAMIC].m_szID;
			}
			
			CMS896AStn::MotionSelectControlParam(BH_AXIS_BIN_EJ, BIN_EJ_DYNAMIC_CONTROL_OBW , &m_stBHAxis_BinEj, szNuControlParaID);

			/*
			//Use different Control Parameter for Up and Down
			if (lRelDistance < 0)				//andrewng //2020-0602
			{
				szNuControlParaID = m_stBHAxis_BinEj.m_stControl[PL_DYNAMIC].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_BIN_EJ, BIN_EJ_DYNAMIC_CONTROL_OBW , &m_stBHAxis_BinEj, szNuControlParaID);
			}
			else if (lRelDistance > 0)			//andrewng //2020-0602
			{
				szNuControlParaID = m_stBHAxis_BinEj.m_stControl[PL_DYNAMIC1].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_BIN_EJ, BIN_EJ_DYNAMIC_CONTROL_OBW , &m_stBHAxis_BinEj, szNuControlParaID);
			}*/

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ, &m_stBHAxis_BinEj);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;	
}


INT CBondHead::Ej_SelectControl(LONG lRelDistance, BOOL bUseShortTravel)
{
	INT nResult = gnOK;
	CString szNuControlParaID;

	if (m_fHardware && !m_bDisableBH)
	{
		try
		{
			//Do not use in NVC Ejector Pin
			//Use different Control Parameter for Up and Down
			//if (lRelDistance > 0)
			//{
			if (bUseShortTravel)	//andrewng //2020-0605
			{
				szNuControlParaID = m_stBHAxis_Ej.m_stControl[PL_DYNAMIC1].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_EJ, EJ_DYNAMIC_CONTROL_OBW , &m_stBHAxis_Ej, szNuControlParaID);
			}
			else
			{
				szNuControlParaID = m_stBHAxis_Ej.m_stControl[PL_DYNAMIC].m_szID;
				CMS896AStn::MotionSelectControlParam(BH_AXIS_EJ, EJ_DYNAMIC_CONTROL_OBW , &m_stBHAxis_Ej, szNuControlParaID);
			}
			//}
			//else if (lRelDistance < 0)
			//{
			//	szNuControlParaID = m_stBHAxis_Ej.m_stControl[PL_DYNAMIC1].m_szID;
			//	CMS896AStn::MotionSelectControlParam(BH_AXIS_EJ, EJ_DYNAMIC_CONTROL_OBW , &m_stBHAxis_Ej, szNuControlParaID);
			//}

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ, &m_stBHAxis_Ej);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;	
}


INT CBondHead::EjT_SelectObwProfile(LONG lRelDistance)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= BH_MP_E_T_OBW_DEFAULT;

	if (m_fHardware && !m_bDisableBH)
	{
		if (lRelDistance < 0)
			szProfile = BH_MP_E_T_OBW_DOWN;
		
		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_EJ_T, szProfile, &m_stBHAxis_EjT);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			AfxMessageBox(szProfile);
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJ_T, &m_stBHAxis_EjT);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

INT CBondHead::BinEjT_SelectObwProfile(LONG lRelDistance)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= BH_MP_BIN_E_T_OBW_DEFAULT;

	if (m_fHardware && !m_bDisableBH)
	{
		if (lRelDistance < 0)
			szProfile = BH_MP_BIN_E_T_OBW_DOWN;
		
		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_BIN_EJ_T, szProfile, &m_stBHAxis_BinEjT);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			AfxMessageBox(szProfile);
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJ_T, &m_stBHAxis_BinEjT);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

INT CBondHead::EjCap_SelectObwProfile(LONG lRelDistance)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= BH_MP_E_CAP_OBW_DEFAULT;

	if (m_fHardware && !m_bDisableBH)
	{
		if (lRelDistance < 0)
			szProfile = BH_MP_E_CAP_OBW_DOWN;
		
		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_EJCAP, szProfile, &m_stBHAxis_EjCap);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			AfxMessageBox(szProfile);
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_EJCAP, &m_stBHAxis_EjCap);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

INT CBondHead::BinEjCap_SelectObwProfile(LONG lRelDistance)
{
	INT nResult			= gnNOTOK;
	CString szProfile	= BH_MP_BIN_E_CAP_OBW_DEFAULT;

	if (m_fHardware && !m_bDisableBH)
	{
		if (lRelDistance < 0)
			szProfile = BH_MP_BIN_E_CAP_OBW_DOWN;
		
		try
		{
			CMS896AStn::MotionSelectObwProfile(BH_AXIS_BIN_EJCAP, szProfile, &m_stBHAxis_BinEjCap);

			nResult = gnOK;
		}
    	catch (CAsmException e)
		{
			AfxMessageBox(szProfile);
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_BIN_EJCAP, &m_stBHAxis_BinEjCap);
			nResult = gnNOTOK;
		}	
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

VOID CBondHead::LogProfileTime()
{
	if( IsAOIOnlyMachine() )
	{
		return ;
	}

	CString szLogFileName  = gszUSER_DIRECTORY + "\\History\\ProfileTime.txt";

	CStdioFile OLogFile;
	BOOL bStatus = OLogFile.Open(szLogFileName, 
					CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);	
	if (bStatus == FALSE)
		return;

	CString szLine;

	//{
		//TRY {
		szLine.Format("EJ Z UP Time = %ld\n",		m_lTime_Ej);
		OLogFile.WriteString(szLine);
		szLine.Format("EJ Z DN Time = %ld\n",		m_lDnTime_Ej);
		OLogFile.WriteString(szLine);
		szLine.Format("EJ CAP UP Time = %ld\n",		m_lUpTime_Ej_Cap);
		OLogFile.WriteString(szLine);
		szLine.Format("EJ CAP DN Time = %ld\n",		m_lDnTime_Ej_Cap);
		OLogFile.WriteString(szLine);
		szLine.Format("EJ T UP Time = %ld\n",		m_lUpTime_Ej_Theta);
		OLogFile.WriteString(szLine);
		szLine.Format("EJ T DN Time = %ld\n\n",		m_lDnTime_Ej_Theta);
		OLogFile.WriteString(szLine);

		szLine.Format("Bin EJ Z UP Time = %ld\n",		m_lTime_BinEj);
		OLogFile.WriteString(szLine);
		szLine.Format("Bin EJ Z DN Time = %ld\n",		m_lDnTime_BinEj);
		OLogFile.WriteString(szLine);
		szLine.Format("Bin EJ T UP Time = %ld\n",		m_lUpTime_BinEj_Theta);
		OLogFile.WriteString(szLine);
		szLine.Format("Bin EJ T DN Time = %ld\n\n",		m_lDnTime_BinEj_Theta);
		OLogFile.WriteString(szLine);

		//v3.61T1	//BT motion time logging
		LONG lBTTimeX_Normal	= (LONG)(*m_psmfSRam)["BinTable"]["NormalProfile"]["PitchTimeX"];
		LONG lBTTimeY_Normal	= (LONG)(*m_psmfSRam)["BinTable"]["NormalProfile"]["PitchTimeY"];
		LONG lBTTimeX_Fast		= (LONG)(*m_psmfSRam)["BinTable"]["FastProfile"]["PitchTimeX"];
		LONG lBTTimeY_Fast		= (LONG)(*m_psmfSRam)["BinTable"]["FastProfile"]["PitchTimeY"];
		szLine.Format("BT PITCH Time X = %d (%d)\n", lBTTimeX_Normal, lBTTimeX_Fast);
		OLogFile.WriteString(szLine);
		szLine.Format("BT PITCH Time Y = %d (%d)\n\n", lBTTimeY_Normal, lBTTimeY_Fast);
		OLogFile.WriteString(szLine);

	szLine.Format("PICK Delay = %d\n",			m_lPickDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("BOND Delay = %d\n",			m_lBondDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("Arm-PICK = %d\n",			m_lArmPickDelay);
	OLogFile.WriteString(szLine);

	szLine.Format("Arm-BOND = %d\n",			m_lArmBondDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("Head-PICK = %d\n",			m_lHeadPickDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("BlockCamera = %d\n",			m_lBlockCameraDelay);		//v4.52A3
	OLogFile.WriteString(szLine);

	szLine.Format("Head-BOND = %d\n",			m_lHeadBondDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("Head-PREPICK = %d\n",		m_lHeadPrePickDelay);		//v3.79
	OLogFile.WriteString(szLine);
	szLine.Format("Sync-PICK = %d\n\n",			m_lSyncPickDelay);			//v3.83
	OLogFile.WriteString(szLine);

	szLine.Format("EJ UP Delay = %d\n",			m_lEjectorUpDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("EJ DN Delay = %d\n\n",		m_lEjectorDownDelay);
	OLogFile.WriteString(szLine);
	//szLine.Format("EJ READY Delay = %d\n",	m_lEjReadyDelay);
	//OLogFile.WriteString(szLine);
	//szLine.Format("EJ OFF Delay = %d\n\n",	m_lEjVacOffDelay);			//v3.78
	//OLogFile.WriteString(szLine);

	szLine.Format("PR Delay = %ld\n",			m_lPRDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("WT T Delay = %ld\n",			m_lWTTDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("WT SETTLE Delay = %ld\n",	m_lWTSettlingDelay);
	OLogFile.WriteString(szLine);
	szLine.Format("BT Delay = %d\n\n",			m_lBinTableDelay);
	OLogFile.WriteString(szLine);


	//v4.16T5
	LONG lLongJumpDelay		= (*m_psmfSRam)["WaferTable"]["LongJumpDelay"];
	LONG lLongJumpMotTime	= (*m_psmfSRam)["WaferTable"]["LongJumpMotion"];
	szLine.Format("WT LongJump Motion Time = %d\n",	lLongJumpMotTime);
	OLogFile.WriteString(szLine);
	szLine.Format("WT LongJump Delay = %d\n\n",			lLongJumpDelay);
	OLogFile.WriteString(szLine);


	szLine.Format("Ej StandBy Level = %ld\n",		m_lStandbyLevel_Ej);
	OLogFile.WriteString(szLine);
	szLine.Format("Ej PreEject Level = %ld\n",		m_lPreEjectLevel_Ej);
	OLogFile.WriteString(szLine);
	szLine.Format("Ej CONTACT Level = %ld\n",		m_lContactLevel_Ej);
	OLogFile.WriteString(szLine);
	szLine.Format("Ej UP Level = %ld\n\n",			m_lEjectLevel_Ej);
	OLogFile.WriteString(szLine);

	szLine.Format("Ej T StandBy pos = %ld\n",		m_lStandby_EJ_Theta);
	OLogFile.WriteString(szLine);
	szLine.Format("Ej T UP pos = %ld\n\n",			m_lUpLevel_EJ_Theta);
	OLogFile.WriteString(szLine);

	szLine.Format("Ej CAP StandBy Level = %ld\n",	m_lStandby_EJ_Cap);
	OLogFile.WriteString(szLine);
	szLine.Format("Ej CAP UP Level = %ld\n",		m_lUpLevel_EJ_Cap);
	OLogFile.WriteString(szLine);
	szLine.Format("Ej CAP DN Offset = %ld\n\n",		m_lDnOffset_EJ_Cap);
	OLogFile.WriteString(szLine);

	szLine.Format("Bin Ej StandBy Level = %ld\n",	m_lStandby_Bin_EJ_Pin);
	OLogFile.WriteString(szLine);
	szLine.Format("Bin Ej UP Level = %ld\n",		m_lUpLevel_Bin_EJ_Pin);
	OLogFile.WriteString(szLine);
	szLine.Format("Bin Ej T StandBy pos = %ld\n",	m_lStandby_Bin_EJ_Theta);
	OLogFile.WriteString(szLine);
	szLine.Format("Bin Ej T UP pos = %ld\n",		m_lUpLevel_Bin_EJ_Theta);
	OLogFile.WriteString(szLine);


	OLogFile.Close();
}
