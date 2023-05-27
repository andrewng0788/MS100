/////////////////////////////////////////////////////////////////
// VPR_SubState.cpp : SubState for Auto-cycle of the CBondPr class
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
#include "BondPr.h"
#include "CTmpChange.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Sequence Operations
////////////////////////////////////////////
VOID CBondPr::BackupBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bUsePostBond_Backup	= m_bUsePostBond;
		m_bUsePostBond = TRUE;

		m_bPlacementCheck_Backup = m_bPlacementCheck;
		m_bPlacementCheck = FALSE;

		m_bRelOffsetCheck_Backup = m_bRelOffsetCheck;
		m_bRelOffsetCheck = FALSE;

		m_bDefectCheck_Backup	 = m_bDefectCheck;
		m_bDefectCheck = FALSE;

		m_bEmptyCheck_Backup	 = m_bEmptyCheck;
		m_bEmptyCheck = FALSE;
	}
}

VOID CBondPr::RestoreBurnInStatus()
{
	if (IsBurnIn())
	{
		m_bUsePostBond		= m_bUsePostBond_Backup;
		m_bPlacementCheck	= m_bPlacementCheck_Backup;
		m_bRelOffsetCheck	= m_bRelOffsetCheck_Backup;
		m_bDefectCheck		= m_bDefectCheck_Backup;
		m_bEmptyCheck		= m_bEmptyCheck_Backup;
	}
}


INT CBondPr::OpInitialize()
{
	INT nResult					= gnOK;

	m_qPreviousSubOperation	= -1;
	m_qSubOperation			= 0;

	NeedReset(FALSE);

	DisplaySequence("BPR - Operation Initialize");
	return nResult;
}

INT CBondPr::OpPreStart()
{
	INT nResult	= gnOK;

	//m_szCycleMessageBP		= "";
	DisplaySequence("BPR - prestart ==> BT Stable");
	m_qSubOperation	= WAIT_BT_STABLE_Q;

	if( m_bDisableBT )
		m_bUsePostBond = FALSE;

	SetBPRLatched(TRUE);
	SetWPRGrabImageReady(FALSE, "OpPreStart()");
	//SetBPRPostBondDone(TRUE);	//v4.40T6

	if (m_fHardware == FALSE)
	{
		(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
		//(*m_psmfSRam)["BondPr"]["Stopped"] = 0;
		CMS896AStn::m_lBondPrStopped = 0;
		DisplaySequence("BPR - Operation Prestart");
		return nResult;
	}

	//v3.23T1
	// If no die is learned or postbond is disable in msd file, turn off the Post-Bond inspection
	if ( (m_ssGenPRSrchID[BPR_NORMAL_DIE] == 0) || 
		(CMS896AStn::m_bUsePostBond == FALSE) ||  
		(m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE) || 
		(m_bDieCalibrated == FALSE) )
	{
		m_bUsePostBond = FALSE;
		(*m_psmfSRam)["BondPr"]["UsePostBond"]	= m_bUsePostBond;
	}
	else
	{
		//Delete Display SPC data
		m_pDspBin->Delete(100);
#ifdef NU_MOTION
		m_pDspBin_Arm1->Delete(100);
		m_pDspBin_Arm2->Delete(100);
#endif
	}

	//v4.33T1
	m_bMS100DisplayPbSrchRect	= FALSE;	//TRUE;		//v4.43T7
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ( pApp->GetCustomerName() == CTM_SANAN )
	{
		//m_bReSearchCurrDie	= TRUE;
		m_bMS100DisplayPbSrchRect = FALSE;
	}


	(*m_psmfSRam)["BondPr"]["PostBondResult"] = 1;
	(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit1"] = FALSE;		//v4.48A4 WH SanAn
	(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit2"] = FALSE;		//v4.48A4 WH SanAn
	(*m_psmfSRam)["BondPr"]["PB RTComp"]	= m_bEnableRTCompXY;		//v4.57A13

	//(*m_psmfSRam)["BondPr"]["Stopped"] = 0;
	CMS896AStn::m_lBondPrStopped	= 0;
	m_lOutCamBlockEmptyCounter		= 0;
	m_lLFLastRowDieCounter			= 0;	//v4.47T8	//WH SanAn
	m_lLFLastRowDieErrAccCount		= 0;	//v4.47T10	//WH SanAn
	m_lSpcCpCounter					= 0;	//v4.51A17	//TJ SanAn

#ifdef NU_MOTION	//v4.01
	InitRtBHCompXYArrays(TRUE, TRUE);	//Arm#1
	InitRtBHCompXYArrays(FALSE, TRUE);  //Arm#2
#endif
	ResetPreBondDieNPadXYArrays();		//CSP

	OpPrescanInit(1);					//BPR_NORMAL_DIE	//andrewng //2020-0615
	m_ulStationID = 0;
	m_ulImageID = 0;
	m_unHWGrabState = 0;				//0=Not Trigger, 1=PreGrabDone, 2=HWTriggerDone,
	//m_bUseHWTrigger = (BOOL)(LONG) (*m_psmfSRam)["WaferPr"]["Use Hardware Trigger"];

	DisplaySequence("BPR - Operation Prestart");
	return nResult;
}

VOID CBondPr::OpUpdateDieIndex()		// Update Die Index
{
	ULONG ulIndex = (*m_psmfSRam)["DieInfo"]["BPR_Index"];
	ulIndex++;
	(*m_psmfSRam)["DieInfo"]["BPR_Index"] = ulIndex;

	//CString szMsg;
	//szMsg.Format("BPR - Index = %d", ulIndex);
	//DisplayMessage(szMsg);
}


LONG CBondPr::OpCheckRelOffsetinFOV()		//v4.26T1	//Semitek
{
	if (!m_fHardware)
		return TRUE;
	if (m_bPostBondAtBond)
		return TRUE;
	if (!m_bRelOffsetCheck)				//If relative XY shift checking is not enabled
		return TRUE;
	if( m_dFovSize < BPR_LF_SIZE )		//FOV must see at least 3x3
		return TRUE;

	CString szLog;
	ULONG ulCurrentBin		= (ULONG)((*m_psmfSRam)["BinTable"]["SPC"]["Block"]) - 1;
	ulCurrentBin			= min(ulCurrentBin, BPR_MAX_BINBLOCK-1);
	ULONG ulCurrentIndex	= (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["Index"];
	ULONG ulDiePerRow		= (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["DiePerRow"];
	ULONG ulDiePerCol		= (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["DiePerCol"];
	ULONG ulWalkPath		= (ULONG)(*m_psmfSRam)["BinTable"]["SPC"]["WalkPath"];

	if (IsNGBlock((UCHAR)(ulCurrentBin + 1)))
	{
		return TRUE; //If NG Block, same as disable relative XY shift checking
	}

//szLog.Format("BPR: RelXYCheck = WPath=%lu, Index=%lu, DIePerRow=%lu",  ulWalkPath, ulCurrentIndex, ulDiePerRow);
//SetErrorMessage(szLog);

	if ( (ulWalkPath == 0) || (ulWalkPath == 2) ||			//BT_TLH_PATH or BT_TRH_PATH
		 (ulWalkPath == 4) || (ulWalkPath == 6) )			//BT_BLH_PATH or BT_BRH_PATH
	{
		if (ulCurrentIndex <= ulDiePerRow)
		{
			return TRUE;				//No need to check on first-row dice
		}
	}
	else if ( (ulWalkPath == 1) || (ulWalkPath == 3) ||		//BT_TLV_PATH or BT_TRV_PATH
			  (ulWalkPath == 5) || (ulWalkPath == 7) )		//BT_BLV_PATH or BT_BRV_PATH
	{
		if (ulCurrentIndex <= ulDiePerCol)
		{
			return TRUE;				//No need to check on first-col dice
		}
	}
	else
	{
		return TRUE;
	}


	//Calcualte search area base on direction	
	PR_WIN	stLaWnd;
	BOOL bIsGoodDie = FALSE;
	PR_BOOLEAN bLatch = PR_FALSE;		//Use last PostBond image for center postbond die
	PR_COORD stSrchCenter; 
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL fDieRotate; 
	PR_REAL	fDieScore;
	int siStepX=0, siStepY=0;

	CTmpChange<PR_WIN> stOrgSrch(&m_stSearchArea, m_stSearchArea);
//	PR_COORD stOrgSrchCorner1 = m_stSearchArea.coCorner1;                                                
//	PR_COORD stOrgSrchCorner2 = m_stSearchArea.coCorner2;

	//Use 1.5x search area to search die
	DOUBLE dSearchCodeX = 4;
	DOUBLE dSearchCodeY = 4;


	//===== Search if any TOP die present =====
	stLaWnd.coCorner1 = m_stSearchArea.coCorner1;
	stLaWnd.coCorner2 = m_stSearchArea.coCorner2;

	/*
#define BT_TLH_PATH						0
#define BT_TLV_PATH						1
#define BT_TRH_PATH						2
#define BT_TRV_PATH						3
#define BT_BLH_PATH						4			//v4.40T7
#define BT_BLV_PATH						5			//v4.40T7
#define BT_BRH_PATH						6			//v4.40T7
#define BT_BRV_PATH						7			//v4.40T7
*/

	//v4.59A32
	if ((ulWalkPath == 1) || (ulWalkPath == 5))				//BT_TLV_PATH or BT_BLV_PATH
	{
		CalculateLFSearchArea(BPR_LT_DIE, 1, &stLaWnd);
	}
	else if ((ulWalkPath == 3)	|| (ulWalkPath == 7))		//BT_TRV_PATH or BT_BRV_PATH
	{
		CalculateLFSearchArea(BPR_RT_DIE, 1, &stLaWnd);
	}
	else if ((ulWalkPath == 4)	|| (ulWalkPath == 6))		//BT_BLH_PATH or BT_BRH_PATH
	{
		CalculateLFSearchArea(BPR_DN_DIE, 1, &stLaWnd);
	}
	else													//BT_TLH_PATH or BT_TRH_PATH
	{
		CalculateLFSearchArea(BPR_UP_DIE, 1, &stLaWnd);
	}

	VerifyPRRegion(&stLaWnd);

	bLatch = PR_FALSE;
	stSrchCenter.x = (stLaWnd.coCorner2.x + stLaWnd.coCorner1.x) / 2;
	stSrchCenter.y = (stLaWnd.coCorner2.y + stLaWnd.coCorner1.y) / 2;

	//Update search area
	LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;

	m_stSearchArea.coCorner1.x = stSrchCenter.x - (PR_WORD)((dSearchCodeX / 4 + 0.5) * m_stGenDieSize[lDieNo].x);
	m_stSearchArea.coCorner1.y = stSrchCenter.y - (PR_WORD)((dSearchCodeY / 4 + 0.5) * m_stGenDieSize[lDieNo].y);
	m_stSearchArea.coCorner2.x = stSrchCenter.x + (PR_WORD)((dSearchCodeX / 4 + 0.5) * m_stGenDieSize[lDieNo].x);
	m_stSearchArea.coCorner2.y = stSrchCenter.y + (PR_WORD)((dSearchCodeY / 4 + 0.5) * m_stGenDieSize[lDieNo].y);
	VerifyPRRegion(&stLaWnd);


	//Search Die -- Disable Die inspection on here
	ULONG ulSearchDie = 0;
	ulSearchDie = (ULONG) ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), bLatch, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
												stLaWnd.coCorner1, stLaWnd.coCorner2, 0, 45);

//	m_stSearchArea.coCorner1 = stOrgSrchCorner1;
//	m_stSearchArea.coCorner2 = stOrgSrchCorner2;

	if ( ulSearchDie == -1 )
    {
        bIsGoodDie = FALSE;
    }
	if (DieIsAlignable(usDieType) == TRUE)
	{
        bIsGoodDie = TRUE;
	}

	if (bIsGoodDie)
	{
		DisplaySequence("BPR - FOV relative check offset, good die");
		//Calculate difference with center die in X & Y (in pixel)
		PR_COORD	stPixel;
		stPixel.x = (PR_WORD)labs(stDieOffset.x - m_stAutoDieOffset.x);
		stPixel.y = (PR_WORD)labs(stDieOffset.y - m_stAutoDieOffset.y);

szLog.Format("BPR: RelXYCheck  GD T-(%d, %d)  C-(%d, %d)",  
						stDieOffset.x, stDieOffset.y, m_stAutoDieOffset.x, m_stAutoDieOffset.y);
//SetErrorMessage(szLog);

		DOUBLE dXinMil=0, dYinMil=0;
		ConvertPixelToDUnit(stPixel, dXinMil, dYinMil, IsMS90()); 
		//ConvertUnitToUm(siStepX, siStepY, &dUnitXForDisplay, &dUnitYForDisplay)
		//Covert from motor-step to mil
		BOOL bCheckX=FALSE, bCheckY=FALSE;
		DOUBLE dPitchXInMil=0, dPitchYInMil=0;
		if ( (ulWalkPath == 1) || (ulWalkPath == 5)  ||
			 (ulWalkPath == 3) || (ulWalkPath == 7) )	
		{
			dPitchXInMil	= (DOUBLE)(0.5 + m_dBTXRes * (DOUBLE) m_lDiePitchX * 1000.0 / 25.4);
			if (IsMS90())
			{
				dPitchXInMil	= (DOUBLE)(0.5 + m_dBTXRes * (DOUBLE) m_lDiePitchX * 1000.0);
			}
			dPitchXInMil	= fabs(dPitchXInMil);
			dXinMil			= fabs(fabs(dXinMil) - dPitchXInMil);
			bCheckX = TRUE;
		}
		else
		{
			dPitchYInMil	= (DOUBLE)(0.5 + m_dBTYRes * (DOUBLE) m_lDiePitchY * 1000.0 / 25.4);
			if (IsMS90())
			{
				dPitchYInMil	= (DOUBLE)(0.5 + m_dBTYRes * (DOUBLE) m_lDiePitchY * 1000.0);
			}
			dPitchYInMil	= fabs(dPitchYInMil);
			dYinMil			= fabs(fabs(dYinMil) - dPitchYInMil);
			bCheckY = TRUE;
		}

		//v4.59A33	//LeoLam 20171109	//v4.59A36
		//DOUBLE dTolXinMil = (DOUBLE)(*m_psmfSRam)["BinTable"]["RealignRelOffsetX"];// Long/Ivan Request to use PostBond XY Shift to check relative shift while bonding
		//DOUBLE dTolYinMil = (DOUBLE)(*m_psmfSRam)["BinTable"]["RealignRelOffsetY"];//
		DOUBLE dTolXinMil = m_dGenDieShiftX[ulCurrentBin];
		DOUBLE dTolYinMil = m_dGenDieShiftY[ulCurrentBin];
		//szLog.Format("BPR: RelXYCheck GD (in mil) ->  XOffset=%.2f (Tol=%.1f) YOffset=%.2f (Tol=%.1f, YPitch=%.2f)",  
		//					dXinMil, dTolXinMil, dOffsetY, dTolYinMil, dPitchYInMil);

		//Compare with relative XY tolerence in um
		if (bCheckX && (dXinMil > dTolXinMil))
		{
			//SetErrorMessage(szLog + "  - HITX");
			m_lOutRelShiftCounter[ulCurrentBin]++;
			if (m_lOutRelShiftCounter[ulCurrentBin] >= m_lGenRelDieShiftAllow[ulCurrentBin])
			{
				szLog.Format("ERR: PB XY Rel -shift fails #(%d): X-Shift = %f (%f), Y-Shift = %f (%f)", 
				m_lOutRelShiftCounter[ulCurrentBin], 
				dXinMil, dTolXinMil, dYinMil, dTolYinMil);
				SetErrorMessage(szLog);
				
				szLog.Format("BPR postbond relative-X check fails  %ld  %ld", m_lOutRelShiftCounter[ulCurrentBin], m_lGenRelDieShiftAllow[ulCurrentBin]);
				SetErrorMessage(szLog);
				m_lOutRelShiftCounter[ulCurrentBin] = 0;
				CString szLog;
				szLog.Format("PB Bin #%ld Rel-X Shift counter is reset in AUTOBOND", ulCurrentBin);
				//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.46T4	//Semitek
				return BPR_ERR_SPC_REL_XY_LIMIT;
			}
			else
			{
				szLog.Format("ERR: PB XY Rel -shift over #(%d): X-Shift = %f (%f), Y-Shift = %f (%f)", 
				m_lOutRelShiftCounter[ulCurrentBin], 
				dXinMil, dTolXinMil, dYinMil, dTolYinMil);
				//SetErrorMessage(szLog);

			}
		}
		else if (bCheckX)
		{
			szLog.Format("ERR: PB XY Rel-shift  #(%d): X-Shift = %f (%f), Y-Shift = %f (%f)", 
			ulCurrentBin+1, 
			dXinMil, dTolXinMil, dYinMil, dTolYinMil);
			//SetErrorMessage(szLog);

			m_lOutRelShiftCounter[ulCurrentBin] = 0;
		}
		
		if (bCheckY && (dYinMil > dTolYinMil))
		{
			//SetErrorMessage(szLog + "  - HITY");
			m_lOutRelShiftCounter[ulCurrentBin]++;
			if (m_lOutRelShiftCounter[ulCurrentBin] >= m_lGenRelDieShiftAllow[ulCurrentBin])
			{
				szLog.Format("ERR: PB XY Rel -shift fails #(%d): X-Shift = %f (%f), Y-Shift = %f (%f)", 
				m_lOutRelShiftCounter[ulCurrentBin], 
				dXinMil, dTolXinMil, dYinMil, dTolYinMil);
				SetErrorMessage(szLog);
				
				szLog.Format("BPR postbond relative-Y check fails  %ld  %ld", m_lOutRelShiftCounter[ulCurrentBin], m_lGenRelDieShiftAllow[ulCurrentBin]);
				SetErrorMessage(szLog);
				m_lOutRelShiftCounter[ulCurrentBin] = 0;
				CString szLog;
				szLog.Format("PB Bin #%ld Rel-Y Shift counter is reset in AUTOBOND", ulCurrentBin);
				//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.46T4	//Semitek
				return BPR_ERR_SPC_REL_XY_LIMIT;
			}
			else
			{
				szLog.Format("ERR: PB XY Rel -shift over #(%d): X-Shift = %f (%f), Y-Shift = %f (%f)", 
				m_lOutRelShiftCounter[ulCurrentBin], 
				dXinMil, dTolXinMil, dYinMil, dTolYinMil);
				//SetErrorMessage(szLog);
			}

		}
		else if (bCheckY)
		{
			szLog.Format("ERR: PB XY Rel-shift  #(%d): X-Shift = %f (%f), Y-Shift = %f (%f)", 
			ulCurrentBin+1, 
			dXinMil, dTolXinMil, dYinMil, dTolYinMil);
			//SetErrorMessage(szLog);

			m_lOutRelShiftCounter[ulCurrentBin] = 0;
		}
	}
	else
	{
		/*
		DisplaySequence("BPR - FOV relative check offset, bad die");
		//SetErrorMessage("BPR: RelXYCheck no GD found!");
		m_lOutRelShiftCounter[ulCurrentBin]++;
		if (m_lOutRelShiftCounter[ulCurrentBin] >= m_lGenRelDieShiftAllow[ulCurrentBin])
		{
			szLog.Format("BPR postbond relative-XY check fails due to no GD found in TOP row  %ld  %ld", 
								m_lOutRelShiftCounter[ulCurrentBin], m_lGenRelDieShiftAllow[ulCurrentBin]);
			SetErrorMessage(szLog);
			m_lOutRelShiftCounter[ulCurrentBin] = 0;
			CString szLog;
			szLog.Format("PB Bin #%ld Rel-XY Shift counter is reset in AUTOBOND", ulCurrentBin);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.46T4	//Semitek
			return BPR_ERR_SPC_REL_XY_LIMIT;
		}*/
	}

	return TRUE;
}


//================================================================
// Function Name: 		UpdateBH1BH2SearchRange
// Input arguments:		stCollet1BondCorner1, stCollet1BondCorner2  -- Arm#1 search range
//						stCollet2BondCorner1, stCollet2BondCorner2  -- Arm#2 search range
// Output arguments:	stCollet1BondCorner1, stCollet1BondCorner2  -- new Arm#1 search range
//						stCollet2BondCorner1, stCollet2BondCorner2  -- new Arm#2 search range
// Description:   		Update the search range of bond arm#1 & arm#2
// Return:				None
// Remarks:				None
//================================================================
VOID CBondPr::UpdateBH1BH2SearchRange(PR_COORD &stCollet1BondCorner1, PR_COORD &stCollet1BondCorner2,
									  PR_COORD &stCollet2BondCorner1, PR_COORD &stCollet2BondCorner2,
									  CString &szBH1Log, CString &szBH2Log)
{
	LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
	LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;
	GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, TRUE);

	DOUBLE dCalibX = GetCalibX();
	DOUBLE dCalibY = GetCalibY();

	if ((fabs(dCalibX) > 0.0000001) && (fabs(dCalibY) > 0.0000001))		//v4.48A30
	{
		stCollet1BondCorner1.x = stCollet1BondCorner1.x - (PR_WORD)_round(lCollet1OffsetX / dCalibX);
		stCollet1BondCorner1.y = stCollet1BondCorner1.y - (PR_WORD)_round(lCollet1OffsetY / dCalibY);
		stCollet1BondCorner2.x = stCollet1BondCorner2.x - (PR_WORD)_round(lCollet1OffsetX / dCalibX);
		stCollet1BondCorner2.y = stCollet1BondCorner2.y - (PR_WORD)_round(lCollet1OffsetY / dCalibY);

		stCollet2BondCorner1.x = stCollet2BondCorner1.x - (PR_WORD)_round(lCollet2OffsetX / dCalibX);
		stCollet2BondCorner1.y = stCollet2BondCorner1.y - (PR_WORD)_round(lCollet2OffsetY / dCalibY);
		stCollet2BondCorner2.x = stCollet2BondCorner2.x - (PR_WORD)_round(lCollet2OffsetX / dCalibX);
		stCollet2BondCorner2.y = stCollet2BondCorner2.y - (PR_WORD)_round(lCollet2OffsetY / dCalibY);
	}

	szBH1Log.Format("use BH1 Offset in pixel (%ld, %ld), in motor step (%ld, %ld)",
					_round(lCollet1OffsetX / dCalibX), _round(lCollet1OffsetY / dCalibY), lCollet1OffsetX, lCollet1OffsetY);

	szBH2Log.Format("use BH2 Offset in pixel (%ld, %ld), in motor step (%ld, %ld)",
				 _round(lCollet2OffsetX / dCalibX), _round(lCollet2OffsetY / dCalibY), lCollet2OffsetX, lCollet2OffsetY);
}


//================================================================
// Function Name: 		GetBondAlignSearchRange
// Input arguments:		bIsBHArm2   --    FALSE -- Arm#1, TRUE -- Arm#2   
// Output arguments:	stSearchArea -- Bond Arm#1 or Arm#2 search range
// Description:   		Find the search range of bond arm#1 or arm#2
// Return:				None
// Remarks:				None
//================================================================
VOID CBondPr::GetBondAlignSearchRange(CString szTitle, const BOOL bIsBHArm2, PR_WIN &stSearchArea)
{
	//1. Get REF DIE Search window size (at cursor center)
	GetSearchDieArea(&stSearchArea, BPR_REFERENCE_DIE + BPR_GEN_RDIE_OFFSET);
	VerifyPRRegion(&stSearchArea);

	//2. Calculate & update Srch Window by Collet 1/2 offset
	PR_COORD stCollet1BondCorner1 = stSearchArea.coCorner1;
	PR_COORD stCollet1BondCorner2 = stSearchArea.coCorner2;
	PR_COORD stCollet2BondCorner1 = stSearchArea.coCorner1;
	PR_COORD stCollet2BondCorner2 = stSearchArea.coCorner2;

	CString szBH1Log, szBH2Log;
	UpdateBH1BH2SearchRange(stCollet1BondCorner1, stCollet1BondCorner2, stCollet2BondCorner1, stCollet2BondCorner2, szBH1Log, szBH2Log);

	if (bIsBHArm2)		//Arm 2
	{
		stSearchArea.coCorner1 = stCollet2BondCorner1;
		stSearchArea.coCorner2 = stCollet2BondCorner2;

		szTitle += szBH2Log;
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTitle);	
	}
	else						//Arm1
	{
		stSearchArea.coCorner1 = stCollet1BondCorner1;
		stSearchArea.coCorner2 = stCollet1BondCorner2;

		szTitle += szBH1Log;
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTitle);	
	}
}

BOOL CBondPr::OpAutoSearchBPRColletHoleWithEpoxyPattern(BOOL bBH2)
{
	LONG lOldZoom = BPR_GetRunZoom();
	BPR_LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_4X);

	BOOL bRet = SearchBPRColletHoleWithEpoxyPattern(bBH2);
	
	BPR_LiveViewZoom(lOldZoom);
	TakeTime(MDO);
	return bRet;
}

BOOL CBondPr::SearchBPRColletHoleWithEpoxyPattern(BOOL bBH2)
{
	CStdioFile oLogFile;
	CString szLine;
	CString szMsg;
	BOOL bResult = FALSE;
	int siStepX = 0,siStepY = 0;
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));

	UINT lSrchEjtColletCoorX = 0;
	UINT lSrchEjtColletCoorY = 0;
	ULONG ulEpoxyAreaSize = 0;
	CString szErrMsg = "";
	bResult = BPR_AutoSearchBondEpoxy(TRUE, TRUE, FALSE, 0, 0, lSrchEjtColletCoorX, lSrchEjtColletCoorY, ulEpoxyAreaSize, NULL, szErrMsg);

	if (bResult)
	{
		m_lBondPosOffsetXPixel = lSrchEjtColletCoorX;
		m_lBondPosOffsetYPixel = lSrchEjtColletCoorY;
		PrestartConfirmBHZBondPos(bBH2, bResult, siStepX, siStepY);
		pBinTable->UpdateBondPosOffsetAtBPR(bBH2, siStepX, siStepY);
		ResetRtBHCompXYArrays(!bBH2);
	}
	return bResult;
}

BOOL CBondPr::BPR_AutoSearchBondEpoxy(BOOL bAuto, BOOL bLatch, BOOL bDisplayResult,
							   LONG pcx, LONG pcy, UINT &nX, UINT &nY,
							   ULONG &ulEpoxyAreaSize, CEvent *pevGrabImageReady,CString &szErrMsg)
{
	nX = nY = 0;
	ulEpoxyAreaSize = 0;
	LONG lPRIndex = BPR_GEN_RDIE_OFFSET + BPR_REFERENCE_PR_DIE_INDEX6;
	if (!m_bGenDieLearnt[lPRIndex])	//Use normal Die #2 for this eproxy Search
	{
		HmiMessage_Red_Back("Collet Hole pattern is not learnt !!", "Bond Optics Collet Hole Search");
		return FALSE;
	}
/*
	if (m_dMaxEpoxySizeFactor < 1)
	{
		return TRUE;
	}
*/
	PR_COORD prcenter;
	prcenter.x = (PR_WORD)pcx;
	prcenter.y = (PR_WORD)pcy;
	if ( (pcx == 0) || (pcy == 0) )
	{
		prcenter.x = (PR_WORD)GetPrCenterX();
		prcenter.y = (PR_WORD)GetPrCenterY();
	}

	LONG lPRID = m_ssGenPRSrchID[lPRIndex];
	DOUBLE dLearnEpoxySize	= m_ulEpoxySize;
	DOUBLE dMinEpoxySize	= dLearnEpoxySize * 0.05;
	DOUBLE dMaxEpoxySize	= _round(10 * dLearnEpoxySize); //_round(m_dMaxEpoxySizeFactor * dLearnEpoxySize);
	PR_UBYTE ubSID = MS899_BOND_CAM_SEND_ID;
	PR_UBYTE ubRID = MS899_BOND_CAM_RECV_ID;

	PR_WORD wRet = m_pPrGeneral->SearchEpoxyExt(lPRID, dMinEpoxySize, dMaxEpoxySize, prcenter, TRUE, TRUE, TRUE,
												bAuto ? FALSE : TRUE, bLatch, ubSID, ubRID,
												nX, nY, ulEpoxyAreaSize, pevGrabImageReady, szErrMsg);
	if (wRet == PR_FALSE)
	{
		szErrMsg = "BPR  " + szErrMsg;
		SetErrorMessage(szErrMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);
		HmiMessage_Red_Yellow(szErrMsg);
		return FALSE;
	}
	else if (wRet == 2)
	{
		CString szMsg;
		szMsg.Format("BPR Search Eproxy Cmd fails - EPOXY size = %lu (learned = %.2f, MAX = %.2f",
						ulEpoxyAreaSize, dLearnEpoxySize, dMaxEpoxySize);
		SetErrorMessage(szErrMsg);
		szErrMsg = szMsg;
		CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);
		if ((State() != AUTO_Q)	&& (State() != DEMO_Q))
		{
			HmiMessage_Red_Yellow(szMsg, "Wafer PR Search Epoxy");
		}
		return FALSE;
	}
	else
	{	
		CString szMsg;
		szMsg.Format("BPR Search Eproxy Cmd found at (%ld, %ld); LATCH = %d, EPOXY size = %lu (learned = %.2f) (%.2f)", 
					  nX, nY, bLatch, ulEpoxyAreaSize, dLearnEpoxySize, ulEpoxyAreaSize);
		SetErrorMessage(szErrMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);
		if (bDisplayResult)
		{
			HmiMessage(szMsg, "BOND PR Search Epoxy");
		}
	}

	return TRUE;
}

BOOL CBondPr::AutoLearnBondEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner)
{
	PR_COORD prcenter;
	prcenter.x = (PR_WORD)GetPrCenterX();
	prcenter.y = (PR_WORD)GetPrCenterY();
	PR_UBYTE ubSID = MS899_BOND_CAM_SEND_ID;	
	PR_UBYTE ubRID = MS899_BOND_CAM_RECV_ID;

	LONG lThreshold			=  126;
	PR_PRE_EPOXY_SHOW_CMD	stEpoxyShowCmd;


	CString szContent = "Is this image OK?";
	CString szTitle   = "WPR Show Epoxy";

	CString szErrMsg;

	do 
    {
		if (!m_pPrGeneral->PreEpoxyShow(stULCorner, stLRCorner, prcenter, ubSID, ubRID, MS899_BOND_PB_PURPOSE, lThreshold, stEpoxyShowCmd, szErrMsg))
		{
			SetErrorMessage(szErrMsg);
			HmiMessageEx(szErrMsg);
			return FALSE;
		}

	} while (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_NO );

	PR_UWORD uwRecordID = 0;
	PR_AREA aeEpoxyArea = 0;
	LONG lPRIndex = BPR_GEN_RDIE_OFFSET + BPR_REFERENCE_PR_DIE_INDEX6;
	if (!m_pPrGeneral->LearnEpoxyExt(prcenter, stEpoxyShowCmd, ubSID, ubRID, uwRecordID, aeEpoxyArea, szErrMsg))
	{
		m_ssGenPRSrchID[lPRIndex]		= 0;
		m_bGenDieLearnt[lPRIndex]		= FALSE;
		m_stGenDieSize[lPRIndex].x		= 0;
		m_stGenDieSize[lPRIndex].y		= 0;
		m_lGenSrchDieScore[lPRIndex]	= 0;

		SetErrorMessage(szErrMsg);
		HmiMessageEx(szErrMsg);
		return FALSE;
	}

	m_ssGenPRSrchID[lPRIndex]		= uwRecordID;
	m_bGenDieLearnt[lPRIndex]		= TRUE;
	m_stGenDieSize[lPRIndex].x		= (PR_WORD)aeEpoxyArea;
	m_stGenDieSize[lPRIndex].y		= (PR_WORD)aeEpoxyArea;
	m_ulEpoxySize					= (LONG)aeEpoxyArea;

	CMSLogFileUtility::Instance()->MS_LogOperation(szErrMsg);

	return TRUE;
}
