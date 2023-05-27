/////////////////////////////////////////////////////////////////
// BH_Prober.cpp : State for Auto-cycle of the CBondHead class
//
//	Description:
//		MS899/MS100 Mapping Die Sorter
//
//	Date:		Friday, March 08, 2013
//	Revision:	1.00
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2013.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
//#include "TesterCommChannel.h"
#include "WT_SubRegion.h"

#include "MS896A.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Prober Fcns
////////////////////////////////////////////

INT CBondHead::PBTZ_Profile(INT nProfile, LONG lUpProbeLevel)
{
	INT nResult			= gnNOTOK;

	LONG lUpPosn = m_lContactLevel_PBTZ;
	if( lUpProbeLevel==0 )
		lUpPosn = m_lContactLevel_PBTZ;
	else
		lUpPosn = lUpProbeLevel;

	LONG lMoveDist = lUpPosn - m_lStandByLevel_PBTZ;
	if (m_fHardware && m_bSel_PBTZ && !m_bDisableBH)
	{
		try
		{
			if (m_lPBTZ_ProfileType == MS896A_OBW_PROFILE)
			{
				CString szProfile;
				if (abs(lMoveDist) <= 1500)
				{
					szProfile = BH_MP_PROBERZ_OBW_SLOW;
				}
				else
				{
					szProfile = BH_MP_PROBERZ_OBW_DEFAULT;
				}
				m_lPBTZDnTime_Z	= CMS896AStn::MotionGetObwProfileTime(BH_AXIS_PROBER_Z, szProfile, lMoveDist, lMoveDist, 
								  HIPEC_SAMPLE_RATE, &m_stBHAxis_ProberZ);

				nResult = gnOK;
			}
			else
			{
				CString szProfile	= "";

				switch (nProfile)
				{
					case NORMAL_PROF:
					default:
						szProfile = BH_MP_PROBERZ_NORMAL;
						break;
				}
				if (szProfile != "")
				{
					CMS896AStn::MotionSelectProfile(BH_AXIS_PROBER_Z, szProfile, &m_stBHAxis_ProberZ);

					m_lPBTZDnTime_Z = CMS896AStn::MotionGetProfileTime(BH_AXIS_PROBER_Z, szProfile, lMoveDist, lMoveDist, 
									HIPEC_SAMPLE_RATE, &m_stBHAxis_ProberZ);
					nResult = gnOK;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ);
			nResult = gnNOTOK;
		}	
	}
	else
	{
#ifdef NU_MOTION			//v3.67T3
		if (IsProber())		//v4.39T11
		{
			m_lPBTZDnTime_Z		= 30;
			m_lPBTZUpTime_Z		= 30;
		}
#endif
		nResult = gnOK;
	}

	if( lUpProbeLevel==0 )
	{
		m_lPBTZUpTime_Z		= m_lPBTZDnTime_Z;
	}
	
	return nResult;
}


VOID CBondHead::InitCP100Variables()
{
	m_lPBTZUpTime_Z		= 30;
	m_lPBTZDnTime_Z		= 30;
	//Prober attributes
	m_bUseLearnProbeTableLevel	= FALSE;
	m_bPbContactAdcRead			= FALSE;

	for ( int i = 0 ; i < 5 ; i++ )
	{
		m_ulTotalProbeCount[i]	= 0;
	}

	m_bReProbeViaVF			= FALSE;
	m_bReProbeViaIR			= FALSE;
	m_ulReProbeLimit		= 0;
	m_uiaCpNgGradeList.RemoveAll();
	m_ulReProbeCount		= 0;
	m_ulContinueProbeLimit	= 0;
	m_ulContinueProbeCount	= 0;

	m_ulProbePinCount		= 0;
	m_ulProbePinCleanCount	= 0;
	m_ulMaxProbePinCount	= 0;
	m_ulMaxProbePinCleanCount = 0;

	m_ulPrbContactFailAcuCount	= 0;
	m_ulPrbContactFailConCount	= 0;
	m_ulProbeContactFailAcuLimit	= 0;
	m_ulProbeContactFailConLimit	= 0;
	m_ulProbeFailCount		= 0;
	m_ulProbeFailCountLimit	= 0;
	m_lProbeMarkCheckLimit	= 0;

	m_bTesterTimeOut		= FALSE;
	m_bCheckProbeHeadContactSnr = FALSE;
	m_bIsCheckFirstProbePos = FALSE;
	m_bCycleFirstProbe		= TRUE;

	m_bIsBlockProbe			= FALSE;
	m_dProbeTestStartTime	= 0;
	m_dProbeTestDuration	= 0;
	m_ucProbeTestPinNo		= 0;
	m_bIsProbeTestStarted	= FALSE;
	m_bStartProbeTest		= FALSE;
	m_ulProbeDelay			= 0;
	m_ulProbeTestTimeout	= 0;
	m_ulProbeTestTime		= 0;
	m_lTestGrade			= 0;
	m_lPPPOpenDelay			= 0;
	m_lPPPCloseDelay		= 0;

	m_bMotionApplyProcessBlock		= FALSE;		// PBTZ motion add process block
	m_bProcessBlockAbortPBTZ		= FALSE;
	m_bProcessBlockSavingEncorder	= FALSE;
	m_ulProcessBlockFailCount		= 0;
	m_ulProcessBlockStopLimit		= 0;
	m_ulProcessBlockAbortLevel_LE	= 20000;
	m_lProcessBlockAbortDiff		= 0;
	m_ulProcessBlockAbortLevel_LE	= (ULONG)(m_lContactLevel_PBTZ + m_lProcessBlockAbortDiff);
	m_bProcessBlockCycleCheckContactDiff	= TRUE;
	m_bProcessBlockCycleCheckAbortLevel		= TRUE;
	m_lProcessBlockSoftwarePortValue	= 00000;
	m_ulProcessBlockDetectDebounce	= 0+0;
	for ( int j = 0 ; j < 16 ; j++ )
	{
		m_lProcessBlockESValue[j] = 00000;
	}
	m_bES1On = FALSE, m_bES2On = FALSE;

	m_bIsCheckContactInRange		= FALSE;	
	m_lContactLevelWithinRange		= 0; // Between Contact Level : |Pin_Level - C_Level| < m_lContactLevelWithinRange
	m_lSensorMaxTorlance			= 0; // Difference Between Pin: |P1_Level - P2_Level| < m_lSensorMaxTorlance
	m_lReCheckCycleNumber			= 0;	


	m_bPBTZUseDynamicLevel		= FALSE;
	m_bPBTZUseCommendInDynamicLevel	= FALSE;	// Use CMD instead of ENC
	m_lStandByDynamicLevel_PBTZ	= 0;	// Variable for dynamic update only
	m_lContactDynamicLevel_PBTZ	= 0;	// Variable for dynamic update only

	m_lCurLevel_ProberZ = 0;
	m_lStandByLevel_PBTZ = 0;
	m_lStandByLevelOffset_PBTZ = 500;
	m_lContactLevel_PBTZ = 0;
	m_lWaferFocusLevelZ		= 0;
	m_lProbeFocusLevelZ		= 0;
	m_lProbingLevelOffset	= 0;
	m_lPbContactLevelOffset = 0;
	m_lPbContactSearchDist	= 0;
	m_dPbSearchVelocity		= 2;
	m_lPbDriveInDist		= 0;
	m_dPbDriveInVelocity	= 0;
	m_lPbSearchDebounce		= 0;

	m_bMultiTestersProbing	= FALSE;

	// CP100 ACP init
	m_bACPEnable			= FALSE;
	m_bACP_BeltType			= FALSE;
	m_bACPAskFillLiquid		= FALSE;
	m_lACPPointLifeLimit	= 1;
	m_lACPPointCycle		= 1;
	m_lACPUsePoints			= 1;
	m_lACPDriveInDist		= 0;
	m_lACPBrushDistX		= 0;
	m_lACPBrushDistY		= 0;
	m_lACPUpDelay			= 0;
	m_lACPStandbyDelay		= 0;
	m_lACPEvaporateDelay	= 0;
	m_lACPPointIndex		= 1;
	m_lACPTotalPoints		= 1;
	m_lACPPointCount		= 0;
	m_bACPLifeExpired		= FALSE;
	m_bUseMultiProbeLevel		= FALSE;
	m_lPrbMultiLevelGridRows	= 2;
	m_lPrbMultiLevelGridCols	= 2;
	m_lPrbLearnLevelOffset		= 0;
	m_stM_P_L.InitPLPoints();

	if ( IsProber() )
	{
		m_lContactLevelWithinRange	= 500;
		m_bIsCheckContactInRange	= FALSE;
	}

	m_lAutoOpenDacRow	= 5;
	m_lAutoOpenDacCol	= 5;
	m_lAutoDacDataIndex	= -1;
	m_lOverShootLimit	= 50;
	m_lDriveInLimit		= 20;
	m_bAutoDacTuned		= FALSE; // init
	m_fScfOpenDacKd		= 0.0;
	m_fTuneOpenDacKd		= 0.0;
}

VOID CBondHead::LoadCP100Options(CStringMapFile  *psmf)
{
	m_ulMaxProbePinCleanCount		= (*psmf)[CP_DATA][PB_COUNT][PB_MAX_CLEAN_PIN_COUNT];
	m_ulProbeFailCountLimit			= (*psmf)[CP_DATA][PB_COUNT][PB_MAX_PROBE_FAIL_LIMIT];
	m_ulProbeContactFailAcuLimit	= (*psmf)[CP_DATA][PB_COUNT][PB_MAX_CONTACT_FAIL_LIMIT];
	m_ulProbeContactFailConLimit	= (*psmf)[CP_DATA][PB_COUNT][PB_MAX_CTCT_CON_FAIL_LIMIT];
	m_lProbeMarkCheckLimit			= (*psmf)[CP_DATA][PB_COUNT][PB_MAX_PRB_MARK_CHECK_LIMIT];

	m_bIsCheckFirstProbePos		= (BOOL)(LONG)((*psmf)[CP_DATA][PB_CHECK_FIRST_PROBE_POSN]);
	m_bCheckProbeHeadContactSnr = (BOOL)(LONG)((*psmf)[CP_DATA][PB_CHECK_PROBE_CONTACT_SNR]);

	m_lProberContactSensors		= (BOOL)(LONG)((*psmf)[CP_DATA][PB_NUMBER_OF_PIN]);
	if ( m_lProberContactSensors <= 0 || m_lProberContactSensors > 8 )	// safety check
	{
		m_lProberContactSensors = 2;	// if it invilid change to 2
	}

	m_bMultiTestersProbing	= (BOOL)(LONG)((*psmf)[CP_DATA][CP_MULTI_TESTERS_PROBING]);
	// CP100 ACP load
	m_bACPEnable			= (BOOL)(LONG)((*psmf)[CP_DATA][CP_ACPP][CP_ENABLE]);
	m_bACPAskFillLiquid		= (BOOL)(LONG)((*psmf)[CP_DATA][CP_ACPP][CP_ASK_ADD_LIQUID]);
	m_lACPPointLifeLimit	= (*psmf)[CP_DATA][CP_ACPP][CP_POINT_LIFE_LIMIT];
	m_lACPPointCycle		= (*psmf)[CP_DATA][CP_ACPP][CP_POINT_CYCLE];
	m_lACPUsePoints			= (*psmf)[CP_DATA][CP_ACPP][CP_USE_POINTS];
	m_lACPDriveInDist		= (*psmf)[CP_DATA][CP_ACPP][CP_DRIVE_IN_DIST];
	m_lACPBrushDistX		= (*psmf)[CP_DATA][CP_ACPP][CP_BRUSH_DIST_X];
	m_lACPBrushDistY		= (*psmf)[CP_DATA][CP_ACPP][CP_BRUSH_DIST_Y];
	m_lACPUpDelay			= (*psmf)[CP_DATA][CP_ACPP][CP_PIN_UP_DELAY];
	m_lACPStandbyDelay		= (*psmf)[CP_DATA][CP_ACPP][CP_PIN_DOWN_DELAY];
	m_lACPEvaporateDelay	= (*psmf)[CP_DATA][CP_ACPP][CP_EVAPORATE_DELAY];
	m_lACPTotalPoints		= (*psmf)[CP_DATA][CP_ACPP][CP_TOTAL_POINTS];

	if( m_lACPTotalPoints>m_lACPTotalPoints )
		m_lACPTotalPoints = m_lACPTotalPoints;

	m_lReCheckCycleNumber	= (*psmf)[CP_DATA][PB_RECHECK_CYCLE_NUMBER];	// Recheck the diff after x count
	m_lSensorMaxTorlance	= (*psmf)[CP_DATA][PB_SENSOR_CHECK_TORLANCE];		
	m_bIsEnableRegularCheck	= (BOOL)(LONG)((*psmf)[CP_DATA][PB_IS_ENABLE_CHECKING]);

	m_lContactLevelWithinRange	= (*psmf)[CP_DATA][PB_CONTACT_LEVEL_RANGE];
	m_bIsCheckContactInRange	= (BOOL)(LONG)((*psmf)[CP_DATA][PB_IS_CHECK_CONTACT_RANGE]);

	m_ulReProbeLimit		= (ULONG)(LONG)(*psmf)[CP_DATA][PB_COUNT][PB_REPROBE_LIMIT];
	m_bReProbeViaVF			= (BOOL)(LONG)(*psmf)[CP_DATA][PB_COUNT][PB_REPROBE_VIA_VF];
	m_bReProbeViaIR			= (BOOL)(LONG)(*psmf)[CP_DATA][PB_COUNT][PB_REPROBE_VIA_IR];


	if( m_lACPPointLifeLimit<=0 )
		m_lACPPointLifeLimit	= 1;
	if( m_lACPPointCycle<=0 )
		m_lACPPointCycle		= 1;
	if( m_lACPUsePoints<=0 )
		m_lACPUsePoints			= 1;
	if( m_lACPTotalPoints<=0 )
		m_lACPTotalPoints = 1;
}

VOID CBondHead::LoadCP100Data(CStringMapFile *psmf)
{
	m_ulProbeDelay				= (*psmf)[BP_DATA][BP_PROBER][PB_PROBE_DELAY];
	m_lContactLevel_PBTZ		= (*psmf)[BP_DATA][BP_PROBER][BP_PROBER_CONTACT_LEVEL_Z];
	m_lContactDynamicLevel_PBTZ = m_lContactLevel_PBTZ;
	m_lStandByLevelOffset_PBTZ	= (*psmf)[BP_DATA][BP_PROBER][PB_STANDBY_LEVEL_OFFSET_Z];
	m_lWaferFocusLevelZ			= (*psmf)[BP_DATA][BP_PROBER][BP_WAFER_FOCUS_LEVEL_Z];
	m_lProbeFocusLevelZ			= (*psmf)[PB_PROCESS][PB_PROBE_FOCUS_LEVEL_Z];
	m_lProbingLevelOffset		= (*psmf)[PB_PROCESS][PB_PROBING_LEVEL_OFFSET_Z];
	m_lStandByLevel_PBTZ		= (*psmf)[BP_DATA][BP_PROBER][BP_PROBER_STANDBY_LEVEL_Z];
	m_lStandByDynamicLevel_PBTZ = m_lStandByLevel_PBTZ;
	m_lPPPOpenDelay			= (*psmf)[BP_DATA][BP_PROBER][PB_PPP_OPEN_DELAY];
	m_lPPPCloseDelay		= (*psmf)[BP_DATA][BP_PROBER][PB_PPP_CLOSE_DELAY];
	if( m_lPPPOpenDelay<500 )
		m_lPPPOpenDelay = 500;
	if( m_lPPPCloseDelay<500 )
		m_lPPPCloseDelay = 500;

	if (m_lStandByLevelOffset_PBTZ == 0)
	{
		m_lStandByLevel_PBTZ = 0;
	}
	else
	{
		m_lStandByLevel_PBTZ = m_lContactLevel_PBTZ + m_lStandByLevelOffset_PBTZ;
		if (m_lStandByLevel_PBTZ < 0)
		{
			m_lStandByLevel_PBTZ = 0;
		}

	}
	if( m_lWaferFocusLevelZ>=m_lStandByLevel_PBTZ )
	{
		m_lWaferFocusLevelZ = 0;
	}
	if( m_lProbeFocusLevelZ>(m_lContactLevel_PBTZ-120) )
	{
		m_lProbeFocusLevelZ = 0;
	}

	m_bEnableSearchFixDist		= (BOOL)(LONG)((*psmf)[PB_PROCESS][PB_PROBER_SRCH_FIX_DIST]);
	m_lPbContactLevelOffset		= (*psmf)[PB_PROCESS][PB_CONTACT_LEVEL_OFFSET];
	m_lPbContactSearchDist		= (*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DISTANCE];
	m_dPbSearchVelocity			= (DOUBLE)(*psmf)[PB_PROCESS][PB_CONTACT_SRCH_VELOCITY];
	m_dPbDriveInVelocity		= (DOUBLE)(*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DRIVEIN_VELOCITY];
	if( m_dPbDriveInVelocity>m_dPbSearchVelocity )
		m_dPbDriveInVelocity = m_dPbSearchVelocity;
	m_lPbSearchDebounce			= (*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DEBOUNCE];
	m_lPbDriveInDist			= (*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DRIVEIN_DISTANCE];
	m_bUseLearnProbeTableLevel	= (BOOL)(LONG)(*psmf)[PB_PROCESS][PB_USE_LEARN_PROBE_TABLE_LEVEL];
	m_bUseMultiProbeLevel		= (BOOL)(LONG)(*psmf)[PB_PROCESS][PB_USE_MULTI_PROBE_LEVEL];
	m_lPrbMultiLevelGridRows	= (*psmf)[PB_PROCESS][PB_MULTI_LEVEL_GRID_ROWS];
	m_lPrbMultiLevelGridCols	= (*psmf)[PB_PROCESS][PB_MULTI_LEVEL_GRID_COLS];
	m_lPrbLearnLevelOffset		= (*psmf)[PB_PROCESS][PB_LEARN_LEVEL_OFFSET];
	BOOL	bPLState = (BOOL)(LONG)(*psmf)[PB_PROCESS][PB_MULTI_PL_STATE];
	m_stM_P_L.SetPLState(bPLState);
	m_stM_B_L1.SetPLState(bPLState);
	CString szData;
	for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			szData.Format("R%02dC%02dX", ulRow, ulCol);
			LONG lScanX = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			szData.Format("R%02dC%02dY", ulRow, ulCol);
			LONG lScanY = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			szData.Format("R%02dC%02dZ", ulRow, ulCol);
			LONG lPL_Z = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			szData.Format("R%02dC%02dR", ulRow, ulCol);
			LONG lState = (*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData];
			m_stM_P_L.SetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z, lState);
			m_stM_B_L1.SetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z, lState);
		}
	}

	if( m_lPrbMultiLevelGridRows<2 )
	{
		m_lPrbMultiLevelGridRows = 2;
	}
	if( m_lPrbMultiLevelGridCols<2 )
	{
		m_lPrbMultiLevelGridCols = 2;
	}

	for (INT i = 0; i < PROBE_TABLE_LEARN_POINT_NUM; i++)
	{
		szData.Format("X%d", i + 1);
		m_lProbeTablePosX[i] = (*psmf)[PB_PROCESS][PB_REGION_LEVEL_TABLE][szData];
		szData.Format("Y%d", i + 1);
		m_lProbeTablePosY[i] = (*psmf)[PB_PROCESS][PB_REGION_LEVEL_TABLE][szData];
		szData.Format("Z%d", i + 1);
		m_lProbeTablePosZ[i] = (*psmf)[PB_PROCESS][PB_REGION_LEVEL_TABLE][szData];

		m_oProbeTable.CalPlaneEqn(m_lProbeTablePosX, m_lProbeTablePosY, m_lProbeTablePosZ);
	}

	m_lACPPointIndex		= (*psmf)[CP_DATA][CP_ACPP][CP_POINT_INDEX];
	m_lACPPointCount		= (*psmf)[CP_DATA][CP_ACPP][CP_POINT_COUNT];
	m_bACPLifeExpired		= (BOOL)(LONG)((*psmf)[CP_DATA][CP_ACPP][CP_CLOTH_EXPIRED]);

	m_bPBTZUseDynamicLevel	= (BOOL)(LONG)((*psmf)[BP_DATA][BP_PROBER][PB_PROBER_USE_DYNAMIC_LEVEL]);

	m_lProcessBlockAbortDiff		= (*psmf)[BP_DATA][BP_PROBER]["Process Block Abort Difference"];
	m_ulProcessBlockStopLimit		= (ULONG)(*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_STOP_LIMIT];
	m_ulProcessBlockAbortLevel_LE	= (ULONG)(*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_ABOUT_LEVEL];
	m_bMotionApplyProcessBlock				= (BOOL)(LONG)((*psmf)[BP_DATA][BP_PROBER][PB_PROBER_USE_PROCESS_BLOCK]);
	m_bProcessBlockSavingEncorder			= (BOOL)(LONG)((*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_SAVE_ENCORDER]);
	if( m_bMotionApplyProcessBlock==FALSE )
	{
		m_bProcessBlockCycleCheckContactDiff	= FALSE;
		m_bProcessBlockCycleCheckAbortLevel		= FALSE;
	}

	m_ulProcessBlockAbortLevel_LE	= m_lContactLevel_PBTZ + m_lProcessBlockAbortDiff;

	if( m_lACPPointIndex<=0 )
		m_lACPPointIndex = 1;
	// search distance must be larger or the same as contact level offset
	if (m_lPbContactLevelOffset > m_lPbContactSearchDist)
	{
		m_lPbContactSearchDist = m_lPbContactLevelOffset;
	}
}

VOID CBondHead::SaveCP100Data(CStringMapFile *psmf)
{
	(*psmf)[CP_DATA][PB_COUNT][PB_MAX_CLEAN_PIN_COUNT]		= m_ulMaxProbePinCleanCount;
	(*psmf)[CP_DATA][PB_COUNT][PB_MAX_PROBE_FAIL_LIMIT]		= m_ulProbeFailCountLimit;
	(*psmf)[CP_DATA][PB_COUNT][PB_MAX_CONTACT_FAIL_LIMIT]	= m_ulProbeContactFailAcuLimit;
	(*psmf)[CP_DATA][PB_COUNT][PB_MAX_CTCT_CON_FAIL_LIMIT]	= m_ulProbeContactFailConLimit;
	(*psmf)[CP_DATA][PB_COUNT][PB_MAX_PRB_MARK_CHECK_LIMIT]	= m_lProbeMarkCheckLimit;

	(*psmf)[CP_DATA][PB_CHECK_FIRST_PROBE_POSN]		= m_bIsCheckFirstProbePos;
	(*psmf)[CP_DATA][PB_CHECK_PROBE_CONTACT_SNR]	= m_bCheckProbeHeadContactSnr;

	(*psmf)[CP_DATA][PB_RECHECK_CYCLE_NUMBER]		= m_lReCheckCycleNumber;	// Recheck the diff after x count
	(*psmf)[CP_DATA][PB_SENSOR_CHECK_TORLANCE]		= m_lSensorMaxTorlance;		
	(*psmf)[CP_DATA][PB_IS_ENABLE_CHECKING]			= m_bIsEnableRegularCheck;

	(*psmf)[CP_DATA][PB_CONTACT_LEVEL_RANGE]		= m_lContactLevelWithinRange;
	(*psmf)[CP_DATA][PB_IS_CHECK_CONTACT_RANGE]		= m_bIsCheckContactInRange;

	(*psmf)[CP_DATA][PB_NUMBER_OF_PIN]				= m_lProberContactSensors;

	(*psmf)[CP_DATA][PB_COUNT][PB_REPROBE_LIMIT]	= m_ulReProbeLimit;
	(*psmf)[CP_DATA][PB_COUNT][PB_REPROBE_VIA_VF]	= m_bReProbeViaVF;
	(*psmf)[CP_DATA][PB_COUNT][PB_REPROBE_VIA_IR]	= m_bReProbeViaIR;

	if (m_lStandByLevelOffset_PBTZ == 0)
	{
		m_lStandByLevel_PBTZ = 0;
	}
	else
	{
		m_lStandByLevel_PBTZ = m_lContactLevel_PBTZ + m_lStandByLevelOffset_PBTZ;

		if (m_lStandByLevel_PBTZ < 0)
		{
			m_lStandByLevel_PBTZ = 0;
		}
	}

	if( m_lWaferFocusLevelZ>=m_lStandByLevel_PBTZ )
	{
		m_lWaferFocusLevelZ = 0;
	}
	if( m_lProbeFocusLevelZ>(m_lContactLevel_PBTZ-120) )
	{
		m_lProbeFocusLevelZ = 0;
	}

	(*psmf)[BP_DATA][BP_PROBER][PB_PROBE_DELAY]				= m_ulProbeDelay;
	(*psmf)[BP_DATA][BP_PROBER][BP_PROBER_CONTACT_LEVEL_Z]	= m_lContactLevel_PBTZ;
	(*psmf)[BP_DATA][BP_PROBER][PB_STANDBY_LEVEL_OFFSET_Z]	= m_lStandByLevelOffset_PBTZ;

	(*psmf)[BP_DATA][BP_PROBER][BP_PROBER_STANDBY_LEVEL_Z]	= m_lStandByLevel_PBTZ;
	(*psmf)[BP_DATA][BP_PROBER][BP_WAFER_FOCUS_LEVEL_Z]		= m_lWaferFocusLevelZ;
	(*psmf)[PB_PROCESS][PB_PROBE_FOCUS_LEVEL_Z]				= m_lProbeFocusLevelZ;
	(*psmf)[PB_PROCESS][PB_PROBING_LEVEL_OFFSET_Z]			= m_lProbingLevelOffset;
	if( m_lPPPOpenDelay<500 )
		m_lPPPOpenDelay = 500;
	if( m_lPPPCloseDelay<500 )
		m_lPPPCloseDelay = 500;
	(*psmf)[BP_DATA][BP_PROBER][PB_PPP_OPEN_DELAY]			= m_lPPPOpenDelay;
	(*psmf)[BP_DATA][BP_PROBER][PB_PPP_CLOSE_DELAY]			= m_lPPPCloseDelay;

	// search distance must be larger or the same as contact level offset
	if (m_lPbContactLevelOffset > m_lPbContactSearchDist)
	{
		m_lPbContactSearchDist = m_lPbContactLevelOffset;
	}
	if( m_dPbDriveInVelocity>m_dPbSearchVelocity )
		m_dPbDriveInVelocity = m_dPbSearchVelocity;
	(*psmf)[PB_PROCESS][PB_PROBER_SRCH_FIX_DIST]			= m_bEnableSearchFixDist;
	(*psmf)[PB_PROCESS][PB_CONTACT_LEVEL_OFFSET]			= m_lPbContactLevelOffset;
	(*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DISTANCE]			= m_lPbContactSearchDist;
	(*psmf)[PB_PROCESS][PB_CONTACT_SRCH_VELOCITY]			= m_dPbSearchVelocity;
	(*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DRIVEIN_VELOCITY]	= m_dPbDriveInVelocity;
	(*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DEBOUNCE]			= m_lPbSearchDebounce;
	(*psmf)[PB_PROCESS][PB_CONTACT_SRCH_DRIVEIN_DISTANCE]	= m_lPbDriveInDist;
	(*psmf)[PB_PROCESS][PB_USE_LEARN_PROBE_TABLE_LEVEL]		= m_bUseLearnProbeTableLevel ;
	(*psmf)[PB_PROCESS][PB_USE_MULTI_PROBE_LEVEL]			= m_bUseMultiProbeLevel;
	(*psmf)[PB_PROCESS][PB_MULTI_LEVEL_GRID_ROWS]			= m_lPrbMultiLevelGridRows;
	(*psmf)[PB_PROCESS][PB_MULTI_LEVEL_GRID_COLS]			= m_lPrbMultiLevelGridCols;
	(*psmf)[PB_PROCESS][PB_LEARN_LEVEL_OFFSET]				= m_lPrbLearnLevelOffset;

	(*psmf)[PB_PROCESS][PB_MULTI_PL_STATE]	= m_stM_P_L.GetPLState();
	CString szData;
	for(LONG ulRow=0; ulRow<PB_MPL_GRID_ROWS; ulRow++)
	{
		for(LONG ulCol=0; ulCol<PB_MPL_GRID_COLS; ulCol++)
		{
			LONG lScanX, lScanY, lPL_Z;
			LONG lState = m_stM_P_L.GetPointPL(ulRow, ulCol, lScanX, lScanY, lPL_Z);
			szData.Format("R%02dC%02dX", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lScanX;
			szData.Format("R%02dC%02dY", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lScanY;
			szData.Format("R%02dC%02dZ", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lPL_Z;
			szData.Format("R%02dC%02dR", ulRow, ulCol);
			(*psmf)[PB_PROCESS][PB_MULTI_PL_TABLE][szData]	= lState;
		}
	}

	for (INT i = 0; i < PROBE_TABLE_LEARN_POINT_NUM; i++)
	{
		szData.Format("X%d", i + 1);
		(*psmf)[PB_PROCESS][PB_REGION_LEVEL_TABLE][szData]	= m_lProbeTablePosX[i];
		szData.Format("Y%d", i + 1);
		(*psmf)[PB_PROCESS][PB_REGION_LEVEL_TABLE][szData]	= m_lProbeTablePosY[i];
		szData.Format("Z%d", i + 1);
		(*psmf)[PB_PROCESS][PB_REGION_LEVEL_TABLE][szData]	= m_lProbeTablePosZ[i];
	}

	// CP100 ACP save
	if( m_lACPTotalPoints>m_lACPTotalPoints )
		m_lACPTotalPoints = m_lACPTotalPoints;

	if( m_lACPPointLifeLimit<=0 )
		m_lACPPointLifeLimit	= 1;
	if( m_lACPPointCycle<=0 )
		m_lACPPointCycle		= 1;
	if( m_lACPUsePoints<=0 )
		m_lACPUsePoints			= 1;
	if( m_lACPTotalPoints<=0 )
		m_lACPTotalPoints = 1;
	if( m_lACPPointIndex<=0 )
		m_lACPPointIndex = 1;
	(*psmf)[CP_DATA][CP_ACPP][CP_ENABLE]			= m_bACPEnable;
	(*psmf)[CP_DATA][CP_ACPP][CP_ASK_ADD_LIQUID]	= m_bACPAskFillLiquid;
	(*psmf)[CP_DATA][CP_ACPP][CP_POINT_LIFE_LIMIT]	= m_lACPPointLifeLimit;
	(*psmf)[CP_DATA][CP_ACPP][CP_POINT_CYCLE]		= m_lACPPointCycle;
	(*psmf)[CP_DATA][CP_ACPP][CP_USE_POINTS]		= m_lACPUsePoints;
	(*psmf)[CP_DATA][CP_ACPP][CP_DRIVE_IN_DIST]		= m_lACPDriveInDist;
	(*psmf)[CP_DATA][CP_ACPP][CP_BRUSH_DIST_X]		= m_lACPBrushDistX;
	(*psmf)[CP_DATA][CP_ACPP][CP_BRUSH_DIST_Y]		= m_lACPBrushDistY;
	(*psmf)[CP_DATA][CP_ACPP][CP_PIN_UP_DELAY]		= m_lACPUpDelay;
	(*psmf)[CP_DATA][CP_ACPP][CP_PIN_DOWN_DELAY]	= m_lACPStandbyDelay;
	(*psmf)[CP_DATA][CP_ACPP][CP_EVAPORATE_DELAY]	= m_lACPEvaporateDelay;
	(*psmf)[CP_DATA][CP_ACPP][CP_POINT_INDEX]		= m_lACPPointIndex;
	(*psmf)[CP_DATA][CP_ACPP][CP_TOTAL_POINTS]		= m_lACPTotalPoints;
	(*psmf)[CP_DATA][CP_ACPP][CP_POINT_COUNT]		= m_lACPPointCount;
	(*psmf)[CP_DATA][CP_ACPP][CP_CLOTH_EXPIRED]		= m_bACPLifeExpired;

	(*psmf)[CP_DATA][CP_MULTI_TESTERS_PROBING]		= m_bMultiTestersProbing;

	if( m_bMotionApplyProcessBlock==FALSE )
	{
		m_bProcessBlockCycleCheckContactDiff	= FALSE;
		m_bProcessBlockCycleCheckAbortLevel		= FALSE;
	}
	(*psmf)[BP_DATA][BP_PROBER][PB_PROBER_USE_DYNAMIC_LEVEL]	= m_bPBTZUseDynamicLevel;

	(*psmf)[BP_DATA][BP_PROBER][PB_PROBER_USE_PROCESS_BLOCK]	= m_bMotionApplyProcessBlock;
	(*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_SAVE_ENCORDER]	= m_bProcessBlockSavingEncorder;
	(*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_STOP_LIMIT]	= m_ulProcessBlockStopLimit;
	(*psmf)[BP_DATA][BP_PROBER]["Process Block Abort Difference"]	= m_lProcessBlockAbortDiff;
	m_ulProcessBlockAbortLevel_LE = m_lContactLevel_PBTZ + m_lProcessBlockAbortDiff;
	(*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_ABOUT_LEVEL]	= m_ulProcessBlockAbortLevel_LE;
	(*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_CHECK_CONTACT] = m_bProcessBlockCycleCheckContactDiff;
	(*psmf)[BP_DATA][BP_PROBER][PB_PROCESS_BLOCK_CHECK_ABORT]	= m_bProcessBlockCycleCheckAbortLevel;
} 

BOOL CBondHead::MoveWaferTableNoCheck(LONG lXAxis, LONG lYAxis)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;
 	RELPOS stPos;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lUnload;
	} RELPOS2;
 	RELPOS2 stPos2;

	stPos.lX	= lXAxis;
	stPos.lY	= lYAxis;
	stPos2.lX	= lXAxis;
	stPos2.lY	= lYAxis;

	stPos2.lUnload = 0;		//to LOAD
	
	if (CMS896AApp::m_bMS100Plus9InchOption)		//v4.16T3	//MS100 9Inch
	{
		stMsg.InitMessage(sizeof(RELPOS2), &stPos2);
		nConvID = m_comClient.SendRequest("WaferTableStn", "XY_MoveToUnloadCmd",	stMsg);
	}
	else
	{
		stMsg.InitMessage(sizeof(RELPOS), &stPos);
		nConvID = m_comClient.SendRequest("WaferTableStn", "XY_MoveToCmd",			stMsg);
	}

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}	
	}

	stMsg.GetMsg(sizeof(BOOL), &bResult);

	return bResult;
}

VOID CBondHead::RegisterProberVariables()
{
	//Prober	//v4.37T10
	RegVariable(_T("PBT_lEnc_PBTZ"),		&m_lEnc_PBTZ);
	
	RegVariable(_T("PBT_bSel_PBTZ"),		&m_bSel_PBTZ);
	RegVariable(_T("PBT_bIsPowerOn_PBTZ"),	&m_bIsPowerOn_PBTZ);
	RegVariable(_T("PBT_bIsPPPClose"),		&m_bIsPPPClose);
	RegVariable(_T("PBT_lPPPOpenDelay"),	&m_lPPPOpenDelay);
	RegVariable(_T("PBT_lPPPCloseDelay"),	&m_lPPPCloseDelay);

	RegVariable(_T("PBT_ulProbeDelay"),				&m_ulProbeDelay);
	RegVariable(_T("PBT_ulProbeTestTimeout"),		&m_ulProbeTestTimeout);
	RegVariable(_T("PBT_ulProbeTestTime"),			&m_ulProbeTestTime);
	RegVariable(_T("PBT_lTestGrade"),				&m_lTestGrade);
	RegVariable(_T("PBT_szProbeTestResult"),		&m_szProbeTestResult);

	// Prober level
	RegVariable(_T("PBT_bEnableSearchFixDist"),		&m_bEnableSearchFixDist);
	RegVariable(_T("PBT_lPbContactLevelOffset"),	&m_lPbContactLevelOffset);
	RegVariable(_T("PBT_lPbContactSearchDist"),		&m_lPbContactSearchDist);
	RegVariable(_T("PBT_lPbDriveInDist"),			&m_lPbDriveInDist);
	RegVariable(_T("PBT_lStandByLevelOffset_PBTZ"),	&m_lStandByLevelOffset_PBTZ);
	RegVariable(_T("PBT_dPbSearchVelocity"),		&m_dPbSearchVelocity);
	RegVariable(_T("PBT_dPbDriveInVelocity"),		&m_dPbDriveInVelocity);
	RegVariable(_T("PBT_lPbSearchDebounce"),		&m_lPbSearchDebounce);
	
	RegVariable(_T("PBT_lFocusLevel_PBTZ"),			&m_lWaferFocusLevelZ);
	RegVariable(_T("PBT_lProbeFocusLevelZ"),		&m_lProbeFocusLevelZ);
	RegVariable(_T("PBT_lProbingLevelOffset"),		&m_lProbingLevelOffset);

	RegVariable(_T("PBT_bUseLearnProbeTableLevel"),	&m_bUseLearnProbeTableLevel);

	for ( int a = 0 ; a < 5 ; a++ )
	{
		CString szTempString;
		if ( a == 0 )
		{
			szTempString = "PBT_ulTotalProbeCount";
		}
		else
		{
			szTempString.Format("PBT_ulTotalProbeCount%d", a);
		}
		RegVariable(szTempString, &(m_ulTotalProbeCount[a]) );
	}


	RegVariable(_T("PBT_ulProbePinCount"),			&m_ulProbePinCount);
	RegVariable(_T("PBT_ulProbePinCleanCount"),		&m_ulProbePinCleanCount);
	RegVariable(_T("PBT_ulMaxProbePinCount"),		&m_ulMaxProbePinCount);
	RegVariable(_T("PBT_ulMaxProbePinCleanCount"),	&m_ulMaxProbePinCleanCount);
	RegVariable(_T("PBT_ulContinueProbeLimit"),		&m_ulContinueProbeLimit);
	RegVariable(_T("PBT_ulReProbeLimit"),			&m_ulReProbeLimit);
	RegVariable(_T("PBT_bReProbeViaVF"),			&m_bReProbeViaVF);
	RegVariable(_T("PBT_bReProbeViaIR"),			&m_bReProbeViaIR);

	RegVariable(_T("PBT_bUseMultiProbeLevel"),		&m_bUseMultiProbeLevel);
	RegVariable(_T("PBT_lPrbMultiLevelGridRows"),	&m_lPrbMultiLevelGridRows);
	RegVariable(_T("PBT_lPrbMultiLevelGridCols"),	&m_lPrbMultiLevelGridCols);
	RegVariable(_T("PBT_lPrbLearnLevelOffset"),		&m_lPrbLearnLevelOffset);

	// CP100 ACP reg var
	RegVariable(_T("PBT_bACPEnable"),				&m_bACPEnable);
	RegVariable(_T("PBT_bACPAskFillLiquid"),		&m_bACPAskFillLiquid);
	RegVariable(_T("PBT_lACPPointLifeLimit"),		&m_lACPPointLifeLimit);
	RegVariable(_T("PBT_lACPPointCycle"),			&m_lACPPointCycle);
	RegVariable(_T("PBT_lACPUsePoints"),			&m_lACPUsePoints);
	RegVariable(_T("PBT_lACPDriveInDist"),			&m_lACPDriveInDist);
	RegVariable(_T("PBT_lACPBrushDistX"),			&m_lACPBrushDistX);
	RegVariable(_T("PBT_lACPBrushDistY"),			&m_lACPBrushDistY);
	RegVariable(_T("PBT_lACPUpDelay"),				&m_lACPUpDelay);
	RegVariable(_T("PBT_lACPStandbyDelay"),			&m_lACPStandbyDelay);
	RegVariable(_T("PBT_lACPEvaporateDelay"),		&m_lACPEvaporateDelay);
	RegVariable(_T("PBT_lACPPointIndex"),			&m_lACPPointIndex);
	RegVariable(_T("PBT_lACPPointCount"),			&m_lACPPointCount);

	RegVariable(_T("PBT_ulAcuContactFailLimit"),	&m_ulProbeContactFailAcuLimit);
	RegVariable(_T("PBT_ulConContactFailLimit"),	&m_ulProbeContactFailConLimit);
	RegVariable(_T("PBT_ulProbeFailCountLimit"),	&m_ulProbeFailCountLimit);
	RegVariable(_T("PBT_ulProbeMarkCheckLimit"),	&m_lProbeMarkCheckLimit);
	RegVariable(_T("PBT_bIsCheckFirstProbePos"),	&m_bIsCheckFirstProbePos);
	RegVariable(_T("PBT_bCheckProbeHeadContactSnr"), &m_bCheckProbeHeadContactSnr);


	// sensor
	RegVariable(_T("PBT_bPbContactSensor1"),		&m_bPbContactSensor1);
	RegVariable(_T("PBT_bPbContactSensor2"),		&m_bPbContactSensor2);
	RegVariable(_T("PBT_bPbContactSensor3"),		&m_bPbContactSensor3);
	RegVariable(_T("PBT_bPbContactSensor4"),		&m_bPbContactSensor4);
	RegVariable(_T("PBT_bPbContactSensor5"),		&m_bPbContactSensor5);
	RegVariable(_T("PBT_bPbContactSensor6"),		&m_bPbContactSensor6);
	RegVariable(_T("PBT_lPbAdcValueContact1"),		&m_lPbAdcValueContact1);
	RegVariable(_T("PBT_lPbAdcValueContact2"),		&m_lPbAdcValueContact2);
	RegVariable(_T("PBT_lPbAdcValueContact3"),		&m_lPbAdcValueContact3);
	RegVariable(_T("PBT_lPbAdcValueContact4"),		&m_lPbAdcValueContact4);
	RegVariable(_T("PBT_lPbAdcValueContact5"),		&m_lPbAdcValueContact5);
	RegVariable(_T("PBT_lPbAdcValueContact6"),		&m_lPbAdcValueContact6);
	RegVariable(_T("PBT_bPbContactAdcRead"),		&m_bPbContactAdcRead);

	// Contact Sensor Check
	RegVariable(_T("PBT_lReCheckCycleNumber"),		&m_lReCheckCycleNumber);	// Recheck the diff after x count
	RegVariable(_T("PBT_lSensorMaxTorlance"),		&m_lSensorMaxTorlance);		
	RegVariable(_T("PBT_bIsEnableRegularCheck"),	&m_bIsEnableRegularCheck);

	// Contact Sensor Range
	RegVariable(_T("PBT_lContactLevelWithinRange"),	&m_lContactLevelWithinRange);
	RegVariable(_T("PBT_bIsCheckContactInRange"),	&m_bIsCheckContactInRange);

	// Real time check
	RegVariable(_T("PBT_bPBTZUseDynamicLevel"),		&m_bPBTZUseDynamicLevel);
	RegVariable(_T("PBT_bPBTZUseCommendInDynamicLevel"),&m_bPBTZUseCommendInDynamicLevel);

	// Flag of using process block
	RegVariable(_T("PBT_bMotionApplyProcessBlock"),		&m_bMotionApplyProcessBlock);
	RegVariable(_T("PBT_bProcessBlockSavingEncorder"),	&m_bProcessBlockSavingEncorder);
	RegVariable(_T("PBT_ulProcessBlockFailCount"),		&m_ulProcessBlockFailCount);
	RegVariable(_T("PBT_ulProcessBlockStopLimit"),		&m_ulProcessBlockStopLimit);
	RegVariable(_T("PBT_lProcessBlockAbortDiff"),		&m_lProcessBlockAbortDiff);
	RegVariable(_T("PBT_ulProcessBlockAbortLevel_LE"),	&m_ulProcessBlockAbortLevel_LE);
	RegVariable(_T("PBT_bProcessBlockCycleCheckContactDiff"),	&m_bProcessBlockCycleCheckContactDiff);
	RegVariable(_T("PBT_bProcessBlockCycleCheckAbortLevel"),	&m_bProcessBlockCycleCheckAbortLevel);
	RegVariable(_T("PBT_lProcessBlockSoftwarePortValue"),		&m_lProcessBlockSoftwarePortValue);
	RegVariable(_T("PBT_ulProcessBlockDetectDebounce"),			&m_ulProcessBlockDetectDebounce);

	// WHAT NUMBER OF PIN IS USING
	RegVariable(_T("PBT_lProberContactSensors"),		&m_lProberContactSensors);

	RegVariable(_T("PBT_bMultiTestersProbing"),			&m_bMultiTestersProbing);
	RegVariable(_T("PBT_bIsEnableOpenDac"),				&m_bIsEnableOpenDac);
	RegVariable(_T("PBT_lPBTSetupSearchingSpeed"),		&m_lPBTSetupSearchingSpeed);

	RegVariable(_T("PBT_lAutoOpenDacRow"),			&m_lAutoOpenDacRow);
	RegVariable(_T("PBT_lAutoOpenDacCol"),			&m_lAutoOpenDacCol);
	RegVariable(_T("PBT_lAutoDacDataIndex"),		&m_lAutoDacDataIndex);
	RegVariable(_T("PBT_lProbeOverShootLimit"),		&m_lOverShootLimit);
	RegVariable(_T("PBT_lProbeDriveInLimit"),		&m_lDriveInLimit);

	RegVariable(_T("BH_lBPGeneral_BHZ5"),	&m_lBPGeneral_BHZ5);
	RegVariable(_T("BH_lBPGeneral_BHZ6"),	&m_lBPGeneral_BHZ6);
}

BOOL CBondHead::StartProbeTest()
{
	return FALSE;
}


