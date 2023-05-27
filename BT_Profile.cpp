/////////////////////////////////////////////////////////////////
// BT_Profile.cpp : Profile function of the CBinTable class
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
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////
//	X Axis 
////////////////////////////////////////////

INT CBinTable::X_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableBT)
	{
		CString szProfile = X_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(BT_AXIS_X, szProfile, &m_stBTAxis_X);
				m_nProfile_X = nProfile;			// Store the profile selected

#ifndef ALLDIESORT
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					//if (m_nProfile_X == LOW_PROF)
					if ((m_nProfile_X == LOW_PROF) || (m_nProfile_X == LOW_PROF1))	//v4.17T4
						X_SelectControl(PL_DYNAMIC1);
					else
						X_SelectControl(PL_DYNAMIC);
				}
#endif
				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
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

INT CBinTable::X2_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableBT)
	{
		CString szProfile = X_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(BT_AXIS_X2, szProfile, &m_stBTAxis_X2);
				m_nProfile_X2 = nProfile;			// Store the profile selected

#ifndef ALLDIESORT
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					//if (m_nProfile_X2 == LOW_PROF)
					if ((m_nProfile_X2 == LOW_PROF) || (m_nProfile_X2 == LOW_PROF1))	//v4.17T4
						X2_SelectControl(PL_DYNAMIC1);
					else
						X2_SelectControl(PL_DYNAMIC);
				}
#endif

				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
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

CString CBinTable::X_ProfName(INT nProfile)
{
	CString szProfile	= "";

	// MS100 9Inch dual-table config only	//v4.17T4
	/////////////////////////////////////////////////
	//	mpfBinTableXFast	: < 1mm travel
	//	mpfBinTableXNormal	: 1 - 1.5 mm travel
	//	mpfBinTableXLow		: 1.5 - 5mm travel		(fastest!!)
	//	mpfBinTableXVLow	: Very long-dist travel
	/////////////////////////////////////////////////

	switch (nProfile)
	{
		case NORMAL_PROF:
			szProfile = "mpfBinTableXNormal";
			break;

		case FAST_PROF:			
			szProfile = "mpfBinTableXFast";
			break;

		case LOW_PROF:
			szProfile = "mpfBinTableXLow";
			break;

		case LOW_PROF1:			//For very-long motion travel
			if (m_bUseDualTablesOption)
				szProfile = "mpfBinTableXVLow";		//Only available for MS100 9Inch dual-table config
			else
				szProfile = "mpfBinTableXLow";
			break;
	}
	return szProfile;
}


////////////////////////////////////////////
//	Y Axis 
////////////////////////////////////////////

INT CBinTable::Y_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableBT)	//v3.60
	{
		CString szProfile = Y_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				//m_pServo_Y->SelectProfile(szProfile);
				CMS896AStn::MotionSelectProfile(BT_AXIS_Y, szProfile, &m_stBTAxis_Y);
				m_nProfile_Y = nProfile;			// Store the profile selected

#ifndef ALLDIESORT
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					if ((m_nProfile_Y == LOW_PROF) || (m_nProfile_Y == LOW_PROF1))	//v4.17T4
						Y_SelectControl(PL_DYNAMIC1);
					else
						Y_SelectControl(PL_DYNAMIC);
				}
#endif

				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
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

INT CBinTable::Y2_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableBT)
	{
		CString szProfile = Y_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				CMS896AStn::MotionSelectProfile(BT_AXIS_Y2, szProfile, &m_stBTAxis_Y2);
				m_nProfile_Y2 = nProfile;			// Store the profile selected

#ifndef ALLDIESORT
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					if ((m_nProfile_Y2 == LOW_PROF) || (m_nProfile_Y2 == LOW_PROF1))	//v4.17T4
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
				CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
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

CString CBinTable::Y_ProfName(INT nProfile)
{
	CString szProfile	= "";

	// MS100 9Inch dual-table config only	//v4.17T4
	/////////////////////////////////////////////////
	//	mpfBinTableYFast	: < 1mm travel
	//	mpfBinTableYNormal	: 1 - 1.5 mm travel
	//	mpfBinTableYLow		: 1.5 - 5mm travel		(fastest!!)
	//	mpfBinTableYVLow	: Very long-dist travel
	/////////////////////////////////////////////////

	switch (nProfile)
	{
		case NORMAL_PROF:
			szProfile = "mpfBinTableYNormal";
			break;

		case FAST_PROF:
			szProfile = "mpfBinTableYFast";
			break;

		case LOW_PROF:
			szProfile = "mpfBinTableYLow";
			break;

		case LOW_PROF1:		//For very-long motion travel
			if (m_bUseDualTablesOption)
				szProfile = "mpfBinTableYVLow";		//Only available for MS100 9Inch dual-table config
			else
				szProfile = "mpfBinTableYLow";
			break;
	}
		
	return szProfile;
}


VOID CBinTable::LogProfileTime()
{
	LONG lDiff_X = m_oBinBlkMain.GetBlkPitchX();
	LONG lDiff_Y = m_oBinBlkMain.GetBlkPitchY();

	LONG lTime_X1=0, lTime_Y1=0, lTime_X2=0, lTime_Y2=0;
	//CalculateProfileTime(X_ProfName(NORMAL_PROF),	lDiff_X, lTime_X1);
	//CalculateProfileTime(Y_ProfName(NORMAL_PROF),	lDiff_Y, lTime_Y1);
	//CalculateProfileTime(X_ProfName(FAST_PROF), lDiff_X, lTime_X2);
	//CalculateProfileTime(Y_ProfName(FAST_PROF), lDiff_Y, lTime_Y2);
	
	lTime_X1 = CMS896AStn::MotionGetProfileTime(BT_AXIS_X, X_ProfName(NORMAL_PROF), lDiff_X, lDiff_X, HIPEC_SAMPLE_RATE, &m_stBTAxis_X);
	lTime_Y1 = CMS896AStn::MotionGetProfileTime(BT_AXIS_Y, Y_ProfName(NORMAL_PROF), lDiff_Y, lDiff_Y, HIPEC_SAMPLE_RATE, &m_stBTAxis_Y);
	lTime_X2 = CMS896AStn::MotionGetProfileTime(BT_AXIS_X, X_ProfName(FAST_PROF), lDiff_X, lDiff_X, HIPEC_SAMPLE_RATE, &m_stBTAxis_X);
	lTime_Y2 = CMS896AStn::MotionGetProfileTime(BT_AXIS_Y, Y_ProfName(FAST_PROF), lDiff_Y, lDiff_Y, HIPEC_SAMPLE_RATE, &m_stBTAxis_Y);

	(*m_psmfSRam)["BinTable"]["NormalProfile"]["PitchTimeX"]	= lTime_X1;
	(*m_psmfSRam)["BinTable"]["NormalProfile"]["PitchTimeY"]	= lTime_Y1;
	(*m_psmfSRam)["BinTable"]["FastProfile"]["PitchTimeX"]		= lTime_X2;
	(*m_psmfSRam)["BinTable"]["FastProfile"]["PitchTimeY"]		= lTime_Y2;
}


BOOL CBinTable::X_SelectControl(CONST INT nCtrl)
{
#ifndef NU_MOTION

	if (!CMS896AApp::m_bUseSlowBTControlProfile)
		return TRUE;

	LONG lDynCtrlTypeX	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_DYNA_CONTROL);				
	LONG lStaCtrlTypeX	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_STAT_CONTROL);				
	CObject* pObject;
	SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(BT_AXIS_X));

	try
	{
		switch (nCtrl)
		{
		case BT_STA_CTRL:
			CMS896AStn::MotionSelectControlParam(BT_AXIS_X, BT_STATIC_CONTROL_X, &m_stBTAxis_X);
			CMS896AStn::MotionSelectControlType(BT_AXIS_X, lStaCtrlTypeX, 1, &m_stBTAxis_X);
			break;

		case BT_SLOW_DYN_CTRL:
			if (m_pInitOperation->GetControlParameterMap()->Lookup(BT_SLOW_DYNAMIC_CONTROL_X, pObject))
			{
				SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
				pServo->SetControlParam(BT_SLOW_DYNAMIC_CONTROL_X, *pCtr);
				pServo->SelectControlParam("ctrBondHeadTObwSettling");
			}
			CMS896AStn::MotionSelectControlType(BT_AXIS_X, lDynCtrlTypeX, 2, &m_stBTAxis_X);
			break;

		case BT_DYN_CTRL:
		default:
			CMS896AStn::MotionSelectControlParam(BT_AXIS_X, BT_DYNAMIC_CONTROL_X, &m_stBTAxis_X);
			CMS896AStn::MotionSelectControlType(BT_AXIS_X, lDynCtrlTypeX, 2, &m_stBTAxis_X);
			break;
		}
	}	
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
#else
	if (m_fHardware && !m_bDisableBT)
	{
		CString szNuControlParaID = "";

		try
		{
			switch (nCtrl)
			{
				case PL_STATIC:
					szNuControlParaID = m_stBTAxis_X.m_stControl[PL_STATIC].m_szID;
					break;

				case PL_DYNAMIC:
					szNuControlParaID = m_stBTAxis_X.m_stControl[PL_DYNAMIC].m_szID;
					break;

				case PL_DYNAMIC1:
					szNuControlParaID = m_stBTAxis_X.m_stControl[PL_DYNAMIC1].m_szID;
					break;
				
				default:
					szNuControlParaID = m_stBTAxis_X.m_stControl[PL_DYNAMIC].m_szID;
			}

			CMS896AStn::MotionSelectControlParam(BT_AXIS_X, "", &m_stBTAxis_X, szNuControlParaID);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
			return FALSE;
		}	
	}
#endif

	return TRUE;
}


BOOL CBinTable::X2_SelectControl(CONST INT nCtrl)
{
#ifdef NU_MOTION

	if (m_fHardware && !m_bDisableBT && CMS896AApp::m_bMS100Plus9InchOption)
	{
		CString szNuControlParaID = "";

		try
		{
			switch (nCtrl)
			{
				case PL_STATIC:
					szNuControlParaID = m_stBTAxis_X2.m_stControl[PL_STATIC].m_szID;
					break;

				case PL_DYNAMIC:
					szNuControlParaID = m_stBTAxis_X2.m_stControl[PL_DYNAMIC].m_szID;
					break;

				case PL_DYNAMIC1:
					szNuControlParaID = m_stBTAxis_X2.m_stControl[PL_DYNAMIC1].m_szID;
					break;
				
				default:
					szNuControlParaID = m_stBTAxis_X2.m_stControl[PL_DYNAMIC].m_szID;
					break;
			}

			CMS896AStn::MotionSelectControlParam(BT_AXIS_X2, "", &m_stBTAxis_X2, szNuControlParaID);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_X2, &m_stBTAxis_X2);
			return FALSE;
		}	
	}

#else

#endif

	return TRUE;
}


BOOL CBinTable::Y_SelectControl(CONST INT nCtrl)
{
#ifndef NU_MOTION

	if (!CMS896AApp::m_bUseSlowBTControlProfile)
		return TRUE;

	LONG lDynCtrlTypeY	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_DYNA_CONTROL);				
	LONG lStaCtrlTypeY	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_STAT_CONTROL);				
	CObject* pObject;
	SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(BT_AXIS_Y));

	try
	{
		switch (nCtrl)
		{
		case BT_STA_CTRL:
			CMS896AStn::MotionSelectControlParam(BT_AXIS_Y, BT_STATIC_CONTROL_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionSelectControlType(BT_AXIS_Y, lStaCtrlTypeY, 1, &m_stBTAxis_Y);
			break;

		case BT_SLOW_DYN_CTRL:
			if (m_pInitOperation->GetControlParameterMap()->Lookup(BT_SLOW_DYNAMIC_CONTROL_Y, pObject))
			{
				SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
				pServo->SetControlParam(BT_SLOW_DYNAMIC_CONTROL_Y, *pCtr);
				pServo->SelectControlParam(BT_SLOW_DYNAMIC_CONTROL_Y);
			}
			CMS896AStn::MotionSelectControlType(BT_AXIS_Y, lDynCtrlTypeY, 2, &m_stBTAxis_Y);
			break;

		case BT_DYN_CTRL:
		default:
			CMS896AStn::MotionSelectControlParam(BT_AXIS_Y, BT_DYNAMIC_CONTROL_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionSelectControlType(BT_AXIS_Y, lDynCtrlTypeY, 2, &m_stBTAxis_Y);
			break;
		}
	}	
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
#else
	if (m_fHardware && !m_bDisableBT)
	{
		CString szNuControlParaID = "";

		try
		{
			switch (nCtrl)
			{
				case PL_STATIC:
					szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_STATIC].m_szID;
					break;

				case PL_DYNAMIC:
					szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
					break;

				case PL_DYNAMIC1:
					szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC1].m_szID;
					break;
				
				default:
					szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
			}

			CMS896AStn::MotionSelectControlParam(BT_AXIS_Y, "", &m_stBTAxis_Y, szNuControlParaID);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
			return FALSE;
		}	
	}
#endif

	return TRUE;
}


BOOL CBinTable::Y2_SelectControl(CONST INT nCtrl)
{
#ifdef NU_MOTION

	if (m_fHardware && !m_bDisableBT && CMS896AApp::m_bMS100Plus9InchOption)
	{
		CString szNuControlParaID = "";

		try
		{
			switch (nCtrl)
			{
				case PL_STATIC:
					szNuControlParaID = m_stBTAxis_Y2.m_stControl[PL_STATIC].m_szID;
					break;

				case PL_DYNAMIC:
					szNuControlParaID = m_stBTAxis_Y2.m_stControl[PL_DYNAMIC].m_szID;
					break;

				case PL_DYNAMIC1:
					szNuControlParaID = m_stBTAxis_Y2.m_stControl[PL_DYNAMIC1].m_szID;
					break;
				
				default:
					szNuControlParaID = m_stBTAxis_Y2.m_stControl[PL_DYNAMIC].m_szID;
			}

			CMS896AStn::MotionSelectControlParam(BT_AXIS_Y2, "", &m_stBTAxis_Y2, szNuControlParaID);
		}
    	catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BT_AXIS_Y2, &m_stBTAxis_Y2);
			return FALSE;
		}	
	}

#else

#endif

	return TRUE;
}

INT CBinTable::T_Profile(INT nProfile)
{
	INT nResult			= gnNOTOK;

	if (m_fHardware && !m_bDisableBT && m_bEnable_T)
	{
		CString szProfile = T_ProfName(nProfile);

		if (szProfile != "")
		{
			try
			{
				//v4.49A10
//CMSLogFileUtility::Instance()->MS_LogOperation("BT T Profile selected - " + szProfile);	//v4.49A9

				CMS896AStn::MotionSelectProfile(BT_AXIS_T, szProfile, &m_stBTAxis_T);
				//m_nProfile_X = nProfile;			// Store the profile selected
				nResult = gnOK;
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				CMS896AStn::MotionCheckResult(BT_AXIS_T, &m_stBTAxis_T);
				nResult = gnNOTOK;
			}	
		}
		else
		{
			HmiMessage_Red_Yellow("BT T: NULL T Profile name!");
		}
	}
	else
	{
		nResult = gnOK;
	}

	return nResult;
}

CString CBinTable::T_ProfName(INT nProfile)
{
	CString szProfile	= "";

	switch (nProfile)
	{
	case LOW_PROF:
		szProfile = "mpfBinTableTSlow";
		break;

	case NORMAL_PROF:
	case FAST_PROF:	
	default:
		szProfile = "mpfBinTableTNormal";
		break;
	}
	return szProfile;
}



