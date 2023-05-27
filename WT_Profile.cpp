/////////////////////////////////////////////////////////////////
// WT_Profile.cpp : Profile function of the CWaferTable class
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	X Axis 
////////////////////////////////////////////

INT CWaferTable::X1_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableWT)
	{
		CString szProfile = X_ProfName(nProfile);
		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(WT_AXIS_X, szProfile, &m_stWTAxis_X);
				m_nProfile_X = nProfile;
				X_SelectControl(PL_DYNAMIC);
				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(WT_AXIS_X, &m_stWTAxis_X);
				nResult = gnNOTOK;
			}	
		}
	}
	else
	{
		nResult = gnOK;
	}

	//CheckResult(nResult, _T("X Axis - Select Profile"));
	return nResult;
}

#define	CP_733_TABLE	0
// prober rod motors
INT CWaferTable::X1_SelectControlByDistance(LONG lX)
{
	INT nResult	= gnOK;

//	if (IsMS60())		//v4.48A4
//	{
///*
//		if (lX < 700)
//		{
//			X_SelectControl(PL_DYNAMIC2);
//			//MotionSelectStaticControl(WT_AXIS_X, &m_stWTAxis_X, PL_STATIC);
//			//MotionSelectSettlingControl(WT_AXIS_X, &m_stWTAxis_X, PL_SETTLING);
//		}
//		else
//		{
//			X_SelectControl(PL_DYNAMIC);
//			//MotionSelectStaticControl(WT_AXIS_X, &m_stWTAxis_X, PL_STATIC1);
//			//MotionSelectSettlingControl(WT_AXIS_X, &m_stWTAxis_X, PL_SETTLING1);
//		}
//*/
//	}
//	else
//	{

		MotionSelectStaticControl(WT_AXIS_X, &m_stWTAxis_X, PL_STATIC);
		MotionSelectSettlingControl(WT_AXIS_X, &m_stWTAxis_X, PL_SETTLING);
		if (lX < 1000)
		{
			X_SelectControl(PL_DYNAMIC);
			//MotionSelectStaticControl(WT_AXIS_X, &m_stWTAxis_X, PL_STATIC);
			//MotionSelectSettlingControl(WT_AXIS_X, &m_stWTAxis_X, PL_SETTLING);
		}
		else
		{
			/*if( lX >= 4500 )
			{
				X_SelectControl(PL_DYNAMIC2);
				MotionSelectStaticControl(WT_AXIS_X, &m_stWTAxis_X, PL_STATIC1);
				MotionSelectSettlingControl(WT_AXIS_X, &m_stWTAxis_X, PL_SETTLING2);
			}
			else*/
			{
				X_SelectControl(PL_DYNAMIC1);
				//MotionSelectStaticControl(WT_AXIS_X, &m_stWTAxis_X, PL_STATIC1);
				//MotionSelectSettlingControl(WT_AXIS_X, &m_stWTAxis_X, PL_SETTLING1);
			}
		}
	//}

	return nResult;
}

INT CWaferTable::X2_Profile(INT nProfile)
{
	INT nResult = gnNOTOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_X2 )
	{
		CString szProfile = X_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(WT_AXIS_X2, szProfile, &m_stWTAxis_X2);
				m_nProfile_X2 = nProfile;

				//v4.20
				if ( (CMS896AApp::m_bMS100Plus9InchOption) || (CMS896AApp::m_bES100v2DualWftOption) )		//v4.24T8
				{
					if (m_nProfile_X2 == LOW_PROF1)
						X2_SelectControl(PL_DYNAMIC1);
					else
						X2_SelectControl(PL_DYNAMIC);
				}

				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
				nResult = gnNOTOK;
			}	
		}
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

CString CWaferTable::X_ProfName(INT nProfile)
{
	CString szProfile	= "mpfWaferXNormal";

	switch (nProfile)
	{
		case FAST_PROF:
			szProfile = "mpfWaferXFast";
			break;

		case NORMAL_PROF:
			szProfile = "mpfWaferXNormal";
			break;

		case NORMAL_PROF1:
			szProfile = "mpfWaferXNormal";
			break;

		case LOW_PROF:
			szProfile = "mpfWaferXSlow";
			break;
		case LOW_PROF1:
			szProfile = "mpfWaferXSlow";
			break;
		case CP_VLOW_PROFILE:
			szProfile = "mpfWaferXVSlow";
			break;
	}

	return szProfile;
}

////////////////////////////////////////////
//	Y Axis 
////////////////////////////////////////////

INT CWaferTable::Y1_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableWT)
	{
		CString szProfile = Y_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(WT_AXIS_Y, szProfile, &m_stWTAxis_Y);
				m_nProfile_Y = nProfile;
				Y_SelectControl(PL_DYNAMIC);
				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(WT_AXIS_Y, &m_stWTAxis_Y);
				nResult = gnNOTOK;
			}	
		}
	}
	else
	{
		nResult = gnOK;
	}

	//CheckResult(nResult, _T("Y Axis - Select Profile"));
	return nResult;
}


// prober rod motors
INT CWaferTable::Y1_SelectControlByDistance(LONG lY)
{
	INT nResult	= gnOK;
//
//	if (IsMS60())	//v4.48A4
//	{
///*		if (lY < 700)
//		{
//			Y_SelectControl(PL_DYNAMIC2);
//		}
//		else
//		{
//			Y_SelectControl(PL_DYNAMIC);
//		}*/
//	}

	
	if (lY < 1000)
	{
		Y_SelectControl(PL_DYNAMIC);
		//MotionSelectStaticControl(WT_AXIS_Y, &m_stWTAxis_Y, PL_STATIC);
		//MotionSelectSettlingControl(WT_AXIS_Y, &m_stWTAxis_Y, PL_SETTLING);
	}
	else //if (lY <= 4500)
	{
		Y_SelectControl(PL_DYNAMIC1);
		//MotionSelectStaticControl(WT_AXIS_Y, &m_stWTAxis_Y, PL_STATIC1);
		//MotionSelectSettlingControl(WT_AXIS_Y, &m_stWTAxis_Y, PL_SETTLING1);
	}
	/*else
	{
		Y_SelectControl(PL_DYNAMIC2);
		MotionSelectStaticControl(WT_AXIS_Y, &m_stWTAxis_Y, PL_STATIC1);
		MotionSelectSettlingControl(WT_AXIS_Y, &m_stWTAxis_Y, PL_SETTLING2);
	}*/
	return nResult;
}


INT CWaferTable::Y2_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableWT && m_bSel_Y2 )
	{
		CString szProfile = Y_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(WT_AXIS_Y2, szProfile, &m_stWTAxis_Y2);
				m_nProfile_Y2 = nProfile;
				
#ifndef ALLDIESORT
				if ( (CMS896AApp::m_bMS100Plus9InchOption) ||
					 (CMS896AApp::m_bES100v2DualWftOption) )		//v4.24T8
				{
					if (m_nProfile_Y2 == LOW_PROF1)
						Y2_SelectControl(PL_DYNAMIC1);	//In MS100 9Inch, DYNAMIC1 control only for very long-dist motion
					else
						Y2_SelectControl(PL_DYNAMIC);	//LOW_PROF also uses DYNAMIC profile
				}
				else
				{
					if ((m_nProfile_Y2 == LOW_PROF) || (m_nProfile_Y2 == LOW_PROF1))
						Y2_SelectControl(PL_DYNAMIC1);
					else
						Y2_SelectControl(PL_DYNAMIC);
				}
#endif

				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
				nResult = gnNOTOK;
			}	
		}
	}
	else
	{
		nResult = gnOK;
	}
	return nResult;
}

CString CWaferTable::Y_ProfName(INT nProfile)
{
	CString szProfile	= "mpfWaferYNormal";
	
	switch (nProfile)
	{
		case FAST_PROF:
			//szProfile = "mpfWaferYFast";
			break;

		case NORMAL_PROF:
			//szProfile = "mpfWaferYNormal";
			break;

		// for prescan
		case NORMAL_PROF1:
			//szProfile = "mpfWaferYNormal";
			break;

		case LOW_PROF:	
			szProfile = "mpfWaferYSlow";	//andrewng //2020-0902
			break;

		case LOW_PROF1:	
			szProfile = "mpfWaferYSlow";	//andrewng //2020-0902
			break;

		case CP_VLOW_PROFILE:
			//szProfile = "mpfWaferYSlow";
			break;
	}

	return szProfile;
}

////////////////////////////////////////////
//	Theta
////////////////////////////////////////////

INT CWaferTable::T1_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableWT && m_bEnableWTTheta)
	{
		CString szProfile = T_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(WT_AXIS_T, szProfile, &m_stWTAxis_T);
				m_nProfile_T = nProfile;
				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(WT_AXIS_T, &m_stWTAxis_T);
				nResult = gnNOTOK;
			}	
		}
	}
	else
	{
		nResult = gnOK;
	}

	//CheckResult(nResult, _T("T Axis - Select Profile"));
	return nResult;
}

INT CWaferTable::T2_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableWT)
	{
		CString szProfile = T_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(WT_AXIS_T2, szProfile, &m_stWTAxis_T2);
				m_nProfile_T2 = nProfile;
				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(WT_AXIS_T2, &m_stWTAxis_T2);
				nResult = gnNOTOK;
			}	
		}
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

CString CWaferTable::T_ProfName(INT nProfile)
{
	CString szProfile	= "";

	switch (nProfile)
	{
		case LOW_PROF:
			szProfile = "mpfWaferTSlow";
			break;

		case NORMAL_PROF:
			szProfile = "mpfWaferTNormal";
			break;
	}

	return szProfile;
}

INT CWaferTable::X_Profile(INT nProfile)
{
	return X1_Profile(nProfile);
}

INT CWaferTable::Y_Profile(INT nProfile)
{
	return Y1_Profile(nProfile);
}

INT CWaferTable::T_Profile(INT nProfile)
{
	return T1_Profile(nProfile);
}

LONG CWaferTable::X1_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	LONG lTime_X = 25;
	if (m_bSel_X)
	{
		if (m_lX_ProfileType == MS896A_OBW_PROFILE)
		{
			lTime_X = CMS896AStn::MotionGetObwProfileTime(WT_AXIS_X,  X_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_X);
		}
		else
		{
			lTime_X = CMS896AStn::MotionGetProfileTime(WT_AXIS_X,  X_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_X);
		}
	}
	return lTime_X;
}
LONG CWaferTable::X2_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	LONG lTime_X = 25;
	if (m_bSel_X2)
		lTime_X = CMS896AStn::MotionGetProfileTime(WT_AXIS_X2, X_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_X2);
	return lTime_X;
}

LONG CWaferTable::X_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	if( IsWT2InUse() )
	{
		return X2_ProfileTime(nProfile, lMaxTravelDist, lQueryDist);
	}
	else
	{
		return X1_ProfileTime(nProfile, lMaxTravelDist, lQueryDist);
	}
}

LONG CWaferTable::Y1_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	LONG lTime_Y = 25;
	if (m_bSel_Y)
	{
		if (m_lX_ProfileType == MS896A_OBW_PROFILE)
		{
			lTime_Y = CMS896AStn::MotionGetObwProfileTime(WT_AXIS_Y,  Y_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_Y);
		}
		else
		{
			lTime_Y = CMS896AStn::MotionGetProfileTime(WT_AXIS_Y,  Y_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_Y);
		}
	}
	return lTime_Y;
}

LONG CWaferTable::Y2_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	LONG lTime_Y = 25;
	if (m_bSel_Y2)
		lTime_Y = CMS896AStn::MotionGetProfileTime(WT_AXIS_Y2, Y_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_Y2);
	return lTime_Y;
}

LONG CWaferTable::Y_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	if( IsWT2InUse() )
	{
		return Y2_ProfileTime(nProfile, lMaxTravelDist, lQueryDist);
	}
	else
	{
		return Y1_ProfileTime(nProfile, lMaxTravelDist, lQueryDist);
	}
}

LONG CWaferTable::Y_ScfProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist) 
{
	LONG lTime_Y = 25;
	if( IsWT2InUse() )
	{
		if (m_bSel_Y2)
		{
			lTime_Y = MotionGetSCFProfileTime(WT_AXIS_Y2, Y_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_Y2);
		}
	}
	else
	{
		if (m_bSel_Y)
		{
			lTime_Y = MotionGetSCFProfileTime(WT_AXIS_Y,  Y_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_Y);
		}
	}

	return lTime_Y;
}

LONG CWaferTable::T1_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	LONG lTime_T = 0;
	if (m_bSel_T)
		lTime_T = CMS896AStn::MotionGetProfileTime(WT_AXIS_T,  T_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_T);
	return lTime_T;
}

LONG CWaferTable::T2_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	LONG lTime_T = 0;
	if (m_bSel_T2)
		lTime_T = CMS896AStn::MotionGetProfileTime(WT_AXIS_T2, T_ProfName(nProfile), lMaxTravelDist, lQueryDist, HIPEC_SAMPLE_RATE, &m_stWTAxis_T2);
	return lTime_T;
}

LONG CWaferTable::T_ProfileTime(INT nProfile, LONG lMaxTravelDist, LONG lQueryDist)
{
	if( IsWT2InUse() )
	{
		return T2_ProfileTime(nProfile, lMaxTravelDist, lQueryDist);
	}
	else
	{
		return T1_ProfileTime(nProfile, lMaxTravelDist, lQueryDist);
	}
}
