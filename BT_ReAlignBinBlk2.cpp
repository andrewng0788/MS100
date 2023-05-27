/////////////////////////////////////////////////////////////////////
// BT_ReAlignBinBlk2.cpp : Realign functions of the CBinTable class
//
//	Description:
//		MS100 9Inch Mapping Die Sorter
//
//	Date:		Thur, December 08, 2011
//	Revision:	1.00
//
//	By:			Andrew Ng
//				
//
//	Copyright @ ASM Technology HK Ltd., 2011.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*
LONG CBinTable::RealignBinBlock2(ULONG ulBlkInUse, BOOL bAuto, BOOL bAutoTeach, BOOL bDisableAlarm, BOOL bDummyRun)
{
    BOOL bOldAligned, bUsePR, bManualLocate;
	BOOL bRealignFailed = FALSE;
	CString str;
	CString szTitle, szContent;	
    LONG lX, lY, lPhyDist=0, lLogDist=0, lDiePitchX, lDiePitchY, lXDistance=0, lYDistance=0;
    ULONG ulNoOfSortedDie, ulDiePerRow, ulDiePerCol, ulDieIndex, ulFirstDieIndex, ulLastDieIndex;
	LONG lDieFound, lDieFound1, lDieFound2;
	LONG DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y;
	DOUBLE  dNewAngle, dFOVSize;
    LONG lNewXOffset, lNewYOffset;
	CString szLog;

	CMS896AStn::m_bRealignFrame2Done = FALSE;

	//v4.52A9
	BOOL bNewAlignMethod = CMS896AApp::m_bBinFrameNewRealignMethod;	//Semitek
	if (m_bSemitekBLMode)	//pApp->GetCustomerName() == "Semitek")	
	{
		bNewAlignMethod = TRUE;
	}

    ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ulDiePerRow     = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 
	ulDiePerCol		= m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkInUse); 
    lDiePitchX      = ConvertFileUnitToXEncoderValue( m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse) );
    lDiePitchY      = ConvertFileUnitToYEncoderValue( m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse) );
    bOldAligned		= m_oBinBlkMain.GrabNVIsAligned(ulBlkInUse);

	szTitle.LoadString(HMB_BT_REALIGN_BIN_BLOCK);			

    if( ulNoOfSortedDie==0 )
    {
		SetJoystickOn(FALSE);

		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
		m_oBinBlkMain.StepMove(ulBlkInUse, ulFirstDieIndex, lX, lY);

		//CMS896AStn::m_bEnableEmptyBinFrameCheck = TRUE;
		if (CMS896AStn::m_bEnableEmptyBinFrameCheck	== FALSE)
		{
			BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY), TRUE);

			if( bAuto==FALSE && !IsBurnIn())
			{
				str.Format("%d", ulBlkInUse);
				SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_ON_BLOCK, str);
			}
		}
		else
		{
			BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY), TRUE);
			if (m_lAlignBinDelay > 0)
				Sleep(m_lAlignBinDelay);

			// make sure no die in bin block if no of sorted die = 0
			if ((lDieFound = SearchGoodDie(TRUE, TRUE)) == TRUE && (!IsBurnIn()))
			{
				str.Format("%d", ulBlkInUse);
				SetAlert_Msg_Red_Yellow(IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN, str);
				return FALSE;
			}	
		}

		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
		m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
		CMS896AStn::m_bRealignFrame2Done = TRUE;
        return TRUE;
    }


	//check FOV size for this binblk
	dFOVSize = GetBPRFOVSize(ulBlkInUse);
	BOOL bLargeDie = IsLargeDieSize();
	
	szLog.Format("Re-align BT2 start [%3.2f] %d; DummyRun=%d", dFOVSize, ulNoOfSortedDie, bDummyRun);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
	
    m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);

    //Check Postbond pr status
    bUsePR = CheckBPRStatus();

	//v4.53A21	//Osram Germany
	if (bUsePR)
	{
		BOOL bIsBT2Calibrated = (BOOL)(LONG)((*m_psmfSRam)["BondPr"]["Calibration"]["BT2"]);
		if (!bIsBT2Calibrated)
		{
			szLog = "BT2 Realignment fails due to PR calibration";
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
			SetAlert(IDS_BPR_CALIBRATEFACTORERROR);
			m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);
			CMS896AStn::m_bRealignFrame2Done = TRUE;
			SwitchToBPR();
			return FALSE;
		}

	}

    // change camera to bond side
	//SwitchToPbBPR();	

	// Display Search window	//v2.58
	BOOL bLF = FALSE;
	if (dFOVSize >= BPR_LF_SIZE)
	{
		bLF = TRUE;
		bLargeDie = FALSE;		//v2.67	//prefer to use LA even if Large-Die
	}

    // off bin table joy stick
	SetJoystickOn(FALSE);

	// find first die of the grade
	ULONG ulFirstDieRow = 0, ulFirstDieCol = 0;
	ulFirstDieIndex = FindFirstDieOfGrade(ulBlkInUse, ulNoOfSortedDie, ulFirstDieRow, ulFirstDieCol);

    m_oBinBlkMain.StepMove(ulBlkInUse, ulFirstDieIndex, lX, lY);
	DieALogical_x = ConvertFileUnitToXEncoderValue(lX);
    DieALogical_y = ConvertFileUnitToYEncoderValue(lY);
    BT2_MoveTo(DieALogical_x, DieALogical_y);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
	Sleep(m_lAlignBinDelay + 100);		//v4.40T5

	if( bUsePR==TRUE )
	{
		if (bAutoTeach == TRUE)
		{

			szLog.Format("Search 1st die start at blk #%d", ulBlkInUse);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

			if (bLargeDie)	//Large die; use single mode	//v2.67
			{
				if (bDummyRun)		//v4.31T10
					lDieFound = TRUE;
				else
					lDieFound = SearchGoodDie(TRUE, TRUE);
			}
			else			//Small die; use LA or move-search mode
			{
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search first die", "a+"); 
				if (bDummyRun)		//v4.31T10
					lDieFound = TRUE;
				else if ( m_oBinBlkMain.GrabLSBondPattern() || m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse))
				{
					lDieFound = SearchGoodDie(TRUE, TRUE);		//v4.35T4
					//v4.42T7
					if ( lDieFound != TRUE )
						lDieFound = SearchBT2FirstGoodDie(ulBlkInUse);
				}
				else
					lDieFound = SearchBT2FirstGoodDie(ulBlkInUse);

				if ( lDieFound == TRUE )
				{
					if (bDummyRun)		//v4.31T10
						lDieFound = TRUE;
					else if ( m_oBinBlkMain.GrabLSBondPattern() )
					{
						lDieFound = TRUE;
					}
					else if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse))	//v4.42T7
					{
						szLog.Format("Finding die, BinMap WalkPath = %d", m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse));
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
						//lDieFound = TRUE;
						switch(m_oBinBlkMain.GrabBinMapWalkPath(ulBlkInUse))
						{
						case BT_TRH_PATH:
							lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize, TRUE);
							break;
						case BT_TLV_PATH:
							lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize, TRUE);
							break;
						case BT_TRV_PATH:
							lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize, TRUE);
							break;
						default:
							lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize, TRUE);
							break;
						}
					}
					else
					{
						CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Finding die", "a+");
						switch(m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
						{
						case BT_TRH_PATH:
							lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize, TRUE);
							break;
						case BT_TLV_PATH:
							lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize, TRUE);
							break;
						case BT_TRV_PATH:
							lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize, TRUE);
							break;
						default:
							lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize, TRUE);
							break;
						}
					}
				}//IF

			}//IF ELSE
		}
		else
		{
			lDieFound = FALSE;
		}

		if( lDieFound == FALSE )
		{
			if ( !IsBurnIn() && m_bAlignBinInAlive == FALSE )
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UL_DIE);
				SwitchToBPR();
				return FALSE;
			}
			
			bManualLocate = 1;
		}
		else
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("1st die is found", "a+");
			bManualLocate = 0;
		}
	}
	else
    {
        bManualLocate = 1;
    }

	if (bManualLocate == 1)
	{
		if (! IsBurnIn())
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Manual locate 1st die", "a+");

			//Ask user to locate die pos, first die in first row
			SetJoystickOn(TRUE, TRUE, TRUE);
			//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001
			szContent.LoadString(HMB_BT_REALIGN_STEP1);			
			if (HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			{
				SwitchToBPR();
				if( bAuto==TRUE )
				{
					return FALSE;
				}
				else
				{
					CMS896AStn::m_bRealignFrame2Done = TRUE;
					return (TRUE);
				}
			}

			SetJoystickOn(FALSE, TRUE, TRUE);
		}

        // to align the die by pr
		if (bUsePR==TRUE)
		{
			SearchGoodDie(TRUE, TRUE);
		}
	}

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-confirm TL die", "a+");
	//Get 1st die physical position
	//v4.40T5
	if (bManualLocate == 0)
	{
		Sleep(m_lAlignBinDelay);		
		SearchGoodDie(TRUE, TRUE);	
		Sleep(100);
	}

	//Sleep(m_lAlignBinDelay);
	GetXYEncoderValue(DieAPhysical_x, DieAPhysical_y, TRUE);	//Returned encoder values is transformed back to BT1 XY encoder values

    // convert die offset in um unit
	lNewXOffset = -ConvertXEncoderValueToFileUnit(DieAPhysical_x-DieALogical_x);
	lNewYOffset = -ConvertYEncoderValueToFileUnit(DieAPhysical_y-DieALogical_y);

	szLog.Format("REALIGN 1stDie ENC POS XY,%d,%d,OFFSET-XY,%d,%d", DieAPhysical_x, DieAPhysical_y, lNewXOffset, lNewYOffset);
	CMSLogFileUtility::Instance()->BT_LogStatus(szLog);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");	//v4.52A9


    // only one die bonded, no need to find last
    if( ulNoOfSortedDie==1 )
	{
		//v4.36
        ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse) + 1;
		m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
        ConvertBinTablePosn(lX, lY, ulBlkInUse, 0, lNewXOffset, lNewYOffset);
        BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));

		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Only 1 die on frame2\n", "a+"); 

        m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
        m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);
        m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, 0);
        m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
        if( bAuto==FALSE && !IsBurnIn())
		{
			SetAlert_Red_Yellow(IDS_BT_ONLY_1_DIE_ON_BLOCK);
		}

		SwitchToBPR();
		CMS896AStn::m_bRealignFrame2Done = TRUE;
		return (TRUE);
	}


 	//=================================================//
	// find last die of first row in the grade
    // off bin table joy stick
	//SetJoystickOn(FALSE);		//v3.71T9
	BOOL bBinMapResetAngle = FALSE;		//v4.39T8
	ULONG ulLastDieRow = 0, ulLastDieCol = 0;
	ulLastDieIndex = FindLastDieOfFirstRowInGrade(ulBlkInUse, ulNoOfSortedDie, ulFirstDieIndex, ulDiePerRow, ulDiePerCol, ulLastDieRow, ulLastDieCol);

    m_oBinBlkMain.StepMove(ulBlkInUse, ulLastDieIndex, lX, lY);
    BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX+lNewXOffset), ConvertFileUnitToYEncoderValue(lY+lNewYOffset));
	LONG DieBLogical_x = ConvertFileUnitToXEncoderValue(lX);
    LONG DieBLogical_y = ConvertFileUnitToYEncoderValue(lY);
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);
	Sleep(m_lAlignBinDelay);

    // use pr to confirm is the last one in the row and no die above this row or at right hand of this die
	if (bUsePR==TRUE)
	{
		if (bAutoTeach == TRUE)
		{

			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search 2nd die", "a+");
			
			if (bLargeDie)	//Large die; use single mode	//v2.67
			{
				if (bDummyRun)		//v4.31T10
					lDieFound = TRUE;
				else
					lDieFound = SearchGoodDie(TRUE, TRUE);
			}
			else			//Small die; use LA or move-search mode
			{

				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Search first die", "a+");
				if (bDummyRun)		//v4.31T10
					lDieFound = TRUE;
				//else
				//	lDieFound = SearchGoodDie(TRUE, TRUE);		//v4.35T4
				else
					lDieFound = SearchBT2FirstGoodDie(ulBlkInUse);	//Re-enabled in //v4.38T2

				if ( lDieFound == TRUE )
				{

					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Finding die", "a+");
					if (bDummyRun)		//v4.31T10
					{
						lDieFound = TRUE;
					}
					else if (m_oBinBlkMain.GrabLSBondPattern())		
					{
						lDieFound = TRUE;
					}
					else
					{
						switch(m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
						{
							default:
								lDieFound = SearchDieInTopRight(ulBlkInUse, dFOVSize, TRUE);
								break;

							case BT_TRH_PATH:
								lDieFound = SearchDieInTopLeft(ulBlkInUse, dFOVSize, TRUE);
								break;

							case BT_TLV_PATH:
								lDieFound = SearchDieInBottomLeft(ulBlkInUse, dFOVSize, TRUE);
								break;

							case BT_TRV_PATH:
								lDieFound = SearchDieInBottomRight(ulBlkInUse, dFOVSize, TRUE);
								break;
						}
					}
				}
			}
		}
		else
		{
			lDieFound = FALSE;
		}

		if (lDieFound == FALSE)
		{
			if ( !IsBurnIn() && (m_bAlignBinInAlive == FALSE) )
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_UR_DIE);
				SwitchToBPR();
				return FALSE;
			}

			bManualLocate = 1;
		}
		else
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("2nd die is found", "a+");
			bManualLocate = 0;
		}
	}
	else
    {
        bManualLocate = 1;
    }

    // manually confirm the last die
	if (bManualLocate == 1)
	{
		if (! IsBurnIn())
		{
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Manual locate 2nd die", "a+");

			//Ask user to  die pos
			SetJoystickOn(TRUE, TRUE, TRUE);
			//CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount();		//anichia001	//v4.43T10

			szContent.LoadString(HMB_BT_REALIGN_STEP2);			
			if (HmiMessageEx_Red_Back(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
			{
				SwitchToBPR();
				if( bAuto==TRUE )
				{
					return FALSE;
				}
				else
				{
					CMS896AStn::m_bRealignFrame2Done = TRUE;
					return (TRUE);
				}
			}

			// confirm the die
			SetJoystickOn(FALSE);
		}

		if (bUsePR==TRUE)
		{
			SearchGoodDie(TRUE, TRUE);
		}
	}

	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-confirm TR die", "a+"); 

	//Get 2nd die physical position
	if ( bLF == FALSE )
	{
		//SearchGoodDie();		//Disabled in //v3.71T9
	}

	//Sleep(m_lAlignBinDelay);	//v3.71T9
	GetXYEncoderValue(DieBPhysical_x, DieBPhysical_y, TRUE);
	szLog.Format("REALIGN 2ndDie ENC-B POS XY, (%ld, %ld)", DieBPhysical_x, DieBPhysical_y);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");		//v4.52A8


	LONG lLogicalXDistance = 0;
	LONG lLogicalYDistance = 0;
	DOUBLE lLogicalAngle = 0.00;

    // after two points confirmed, Calculate New Angle
	// convert to standard distance to avoid problem because of diffrent resolution
	switch(m_oBinBlkMain.GrabWalkPath(ulBlkInUse))
	{
		case BT_TLV_PATH:
			if( abs(DieBPhysical_y-DieAPhysical_y) < (lDiePitchY/2) )
			{
				dNewAngle = 0; // less than 0.5 y pitch, only one die
			}
			else
			{
				lXDistance = ConvertXEncoderValueToFileUnit( DieAPhysical_x-DieBPhysical_x );
				lYDistance = ConvertYEncoderValueToFileUnit( DieBPhysical_y-DieAPhysical_y );
				dNewAngle = atan( ((float)lXDistance)/((float)lYDistance) );
			}
			break;

		case BT_TRV_PATH:
			if( abs(DieBPhysical_y-DieAPhysical_y) < (lDiePitchY/2) )
			{
				dNewAngle = 0; // less than 0.5 y pitch, only one die
			}
			else
			{
				lXDistance = -ConvertXEncoderValueToFileUnit( DieAPhysical_x-DieBPhysical_x );
				lYDistance = -ConvertYEncoderValueToFileUnit( DieBPhysical_y-DieAPhysical_y );
				dNewAngle = atan( ((float)lXDistance)/((float)lYDistance) );
			}
			break;

		case BT_TRH_PATH:
			if( abs(DieBPhysical_x-DieAPhysical_x) < (lDiePitchX/2) )
			{
				dNewAngle = 0; // less than 0.5 x pitch, only one die
			}
			else
			{
				lXDistance = -ConvertXEncoderValueToFileUnit( DieBPhysical_x-DieAPhysical_x );
				lYDistance = -ConvertYEncoderValueToFileUnit( DieBPhysical_y-DieAPhysical_y );
				dNewAngle = atan( ((float)lYDistance)/((float)lXDistance) );
			}
			break;

		default:
			if( abs(DieBPhysical_x-DieAPhysical_x) < (lDiePitchX/2) )
			{
				dNewAngle = 0; // less than 0.5 x pitch, only one die

				szLog = "BT: Angle = 0 degree!";
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
			}
			else
			{
				lLogicalXDistance = ConvertXEncoderValueToFileUnit( DieBLogical_x-DieALogical_x );
				lLogicalYDistance = ConvertYEncoderValueToFileUnit( DieBLogical_y-DieALogical_y );
				lLogicalAngle = atan( ((float)lLogicalYDistance)/((float)lLogicalXDistance) );

				lXDistance = ConvertXEncoderValueToFileUnit( DieBPhysical_x-DieAPhysical_x );
				lYDistance = ConvertYEncoderValueToFileUnit( DieBPhysical_y-DieAPhysical_y );
				dNewAngle = atan( ((float)lYDistance)/((float)lXDistance) );

				szLog.Format("TLH  A = (%ld, %ld); B = (%ld, %ld); Angle = %.2f (%.2f deg); Angle (logical) = %.2f", 
							 DieAPhysical_x, DieAPhysical_y, DieBPhysical_x, DieBPhysical_y, 
							 dNewAngle, (dNewAngle * 180.0 / PI), lLogicalAngle);		//v4.52A9
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

				if (m_oBinBlkMain.GrabLSBondPattern())
				{
					dNewAngle = dNewAngle - lLogicalAngle;	//v4.35T4	//MS109
				}
			}
			break;
	}

    // bin block rotate too much
	if (m_bCheckAlignBinResult && fabs(dNewAngle) >= 0.05236)// 3 degrees
	{
        //m_oBinBlkMain.SetIsAligned(ulBlkInUse, bOldAligned);		//v3.44

		if (! IsBurnIn())
		{
			szLog.Format("Bin block rotate too much %d, %d, %.5f", lXDistance, lYDistance, fabs(dNewAngle));
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog + "\n", "a+");
			SetAlert_Red_Yellow(IDS_BT_REALIGN_ANGLE_TOO_LARGE);
			SwitchToBPR();
		}

		return (FALSE);
	}

	// lPhysicalDistance = (physical b -- physical a);
	LONG lX1, lY1, lX2, lY2;
	DOUBLE dXDistance = (DOUBLE)lXDistance;
	DOUBLE dYDistance = (DOUBLE)lYDistance;
	lPhyDist = (LONG)sqrt( pow(dXDistance, 2.0) + pow(dYDistance, 2.0) );
	m_oBinBlkMain.StepMove(ulBlkInUse, ulFirstDieIndex, lX1, lY1);
	m_oBinBlkMain.StepMove(ulBlkInUse, ulLastDieIndex,  lX2, lY2);
	dXDistance = (DOUBLE)(lX1-lX2);
	dYDistance = (DOUBLE)(lY1-lY2);
	lLogDist = (LONG)sqrt( pow(dXDistance, 2.0) + pow(dYDistance, 2.0) );

	szLog.Format("2 Die distance (P/L) = %d,%d; angle = %.2f degree", lPhyDist, lLogDist, dNewAngle * 180.0 / PI);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	
	if(m_bCheckAlignBinResult && fabs((double)lPhyDist - lLogDist) > m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse))
	{
		//m_oBinBlkMain.SetIsAligned(ulBlkInUse, bOldAligned);		//v3.44

		// if physical dist between 1st and 2nd die and logical row length diff by larger than 1-pitch
		str.Format("Diff. of RELAIGN distance %d and LOGICAL distance %d between 1st and 2nd die is over one-pitch!", lPhyDist, lLogDist);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(str + "\n", "a+");
		if (! IsBurnIn())
		{
			SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_FAILED, str);
		}
			
		SwitchToBPR();
		return FALSE;
	}

	if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse) && bBinMapResetAngle)		//v4.39T8
	{
		dNewAngle = 0;
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Andrew: reset angle to 0", "a+"); 
	}


	//==============================================================//
	// check last die here
	LONG l3rdDieX = 0, l3rdDieY = 0;
	LONG l3rdDieOrigX = 0, l3rdDieOrigY = 0;
	LONG lLastDieOffsetX = 0;	
	LONG lLastDieOffsetY = 0;

	if (bUsePR == FALSE)
	{
		// just move bin table to last die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie;

		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		ULONG ulLastIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);		
		for (INT i = 1; i < (INT) ulLastIndex; i++)		
		{
			for (INT j = 0; j < (INT) m_ulHoleDieNum; j++)									
			{
				if	(m_oBinBlkMain.GrabRandomHoleDieIndex(ulBlkInUse, j) == i)
					ulDieIndex++;
			}
		}

		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
		ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
	}
	else
	{
		//SetJoystickOn(FALSE);		//v3.71T9

		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Confirm last die", "a+");

		// move bin table to last die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie +  m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		//v4.35T3	//PLLM MS109 Lumiramic
		if ( m_oBinBlkMain.GrabLSBondPattern() )
		{
			// e.g. LS Bond sequence with TLH path, DiePerRow = 10 //
			//
			//     1           
			// 11 10  9  8  7  6  5  4  3  2
			//    12 13 14 15 16 17 18 19 20
			//    29 28 27 26 25 24 23 22 21
			//    31 32 33 .......
			//
			
			ULONG ulOrigDieIndex = ulDieIndex;
			ULONG ulLastDieIndex = ulDieIndex;
			ULONG ulNextIndex = 0;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulLastDieIndex, ulNextIndex))
				ulDieIndex = ulNextIndex;

			szLog.Format("Search Last die (blk #%d) with LS pattern: NewIndex=%lu, OrigIndex=%lu", 
				ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
		}
		else if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse))	//v4.36
		{
			ULONG ulOrigDieIndex = ulDieIndex;

			szLog.Format("Search Last die (blk #%d) in BINMAP: NewIndex=%lu, OrigIndex=%lu", 
				ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
		}


		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		//ULONG ulLastIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
		l3rdDieOrigX = ConvertFileUnitToXEncoderValue(lX);
		l3rdDieOrigY = ConvertFileUnitToYEncoderValue(lY);
		
		//v4.52A8
		szLog.Format("BLK #%lu LastDie (Before comp) - XEnc=%ld, YEnc=%ld (%.2f, %ld, %ld)", 
			ulBlkInUse, l3rdDieOrigX, l3rdDieOrigY, dNewAngle, lNewXOffset, lNewYOffset);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		LONG lX1 = lX;	
		ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
		l3rdDieX = ConvertFileUnitToXEncoderValue(lX);
		l3rdDieY = ConvertFileUnitToYEncoderValue(lY);
		szLog.Format("BLK #%lu BT2 LastDie (Before PR) - XEnc=%ld, YEnc=%ld", ulBlkInUse, l3rdDieX, l3rdDieY);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		//v2.58
		// Calculate bin table to last+1 die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + 1;

		//v4.35T3	//PLLM MS109 Lumiramic
		if ( m_oBinBlkMain.GrabLSBondPattern() )
		{
			ULONG ulOrigDieIndex = ulDieIndex;
			ULONG ulLastDieIndex = ulDieIndex;
			ULONG ulNextIndex = 0;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulLastDieIndex, ulNextIndex))
				ulDieIndex = ulNextIndex;

			szLog.Format("Search Last+1 die (blk #%d) with LS pattern: NewIndex=%lu, OrigIndex=%lu", 
				ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			ULONG ulLSIndex = 0;
			m_oBinBlkMain.RecalculateLSBondPatternIndex(ulBlkInUse, ulNoOfSortedDie, ulLSIndex);
			m_oBinBlkMain.SaveEmptyUnitsToMSD();
		}
		else if (m_oBinBlkMain.IsUseBinMapBondArea(ulBlkInUse))	//v4.36
		{
			ULONG ulOrigDieIndex = ulDieIndex;

			szLog.Format("Search Last+1 die (blk #%d) in BINMAP: NewIndex=%lu, OrigIndex=%lu", 
				ulBlkInUse, ulDieIndex, ulOrigDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}


		// Align Last-die and return its XY placement
	    //lDieFound1 = SearchGoodDie();	
		BOOL bPlacementOk = TRUE;
		DOUBLE dX=0, dY=0;
		Sleep(m_lAlignBinDelay);
		lDieFound1 = Search3rdGoodDie2(bPlacementOk, dX, dY);	
		if (bDummyRun)		//v4.31T10
		{
			lDieFound1 = TRUE;
			bPlacementOk = TRUE;
		}

		//v4.36	//PLLM MS109
		//If under MS109 AUTO mode the same BT1 has confirm & bypass LastDie failure in setup, then let it go
		BOOL bDisableMS109LastDieOffsetX = FALSE;
		if (!m_bAlignBinInAlive && m_bUseDualTablesOption && m_bManualConfirmLastDieResult2)
		{
			if (lDieFound1 && !bPlacementOk)
			{
				bPlacementOk = TRUE;
				if (m_lManualConfirmXOffset != 0)
					lNewXOffset = m_lManualConfirmXOffset;
				if (m_lManualConfirmYOffset != 0)
					lNewYOffset = m_lManualConfirmYOffset;
				m_lManualConfirmXOffset = 0;
				m_lManualConfirmYOffset = 0;
				bDisableMS109LastDieOffsetX = TRUE;
			}
							
			lDieFound1 = TRUE;
			m_bManualConfirmLastDieResult2 = FALSE;
		}
		else if (m_bManualConfirmLastDieResult2)
		{
			m_bManualConfirmLastDieResult2 = FALSE;
		}


		//Calculate LastDie + 1 posn
        m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
		LONG lX2 = lX;
        //ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
		if (bPlacementOk && m_bUseDualTablesOption && !bDisableMS109LastDieOffsetX)		//v4.38T7
		{
			if (fabs(m_dXResolution - 0.000000001) > 0)
			{
				lNewXOffset = _round(lNewXOffset - dX / m_dXResolution);
			}

			if (fabs(m_dYResolution - 0.000000001) > 0)
			{
				lNewYOffset = _round(lNewYOffset - dY / m_dYResolution);		//v4.39T6
			}
			ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset, TRUE);
		}
		else
			ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset, TRUE);


		// Look-ahead other PR sub-windows at Last-Die position
		CString szLfError = "Warning: ";
		LONG lLfDieOK = TRUE;	//Always disable lookahead function on 3rd die
		//if (lDieFound1)
		//	lLfDieOK = LookAhead3rdGoodDie(ulBlkInUse, TRUE, dFOVSize, szLfError);


		// move bin table to last+1 die of this block, use realign result
        BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(m_lAlignBinDelay);		
		lDieFound2 = SearchGoodDie(TRUE, TRUE);
		if (bDummyRun)		//v4.31T10
		{
			lDieFound2 = TRUE;
		}


		szLog.Format("BLK #%lu BT2 LastDie+1 - XEnc=%ld, YEnc=%ld", 
						ulBlkInUse, ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		szLog.Format("PR LastDie Result - %d %d", lDieFound1, lDieFound2);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		// Determine last-die result
        if( lDieFound1==TRUE && lDieFound2!=TRUE )
		{
			CString szMsg;
			if ( !bPlacementOk && !IsBurnIn() && m_bAlignBinInAlive )		//v3.62	
			{
				LONG lPosX1=0, lPosX2=0;
				LONG lPosY1=0, lPosY2=0;
				GetXYEncoderValue(lPosX1, lPosY1, TRUE);

				SwitchToBPR();
				DisplayBondPrDieSizeWindow(TRUE);

				//Ask user to die pos
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_X, "BT_JoySlow", &m_stBTAxis_X);
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_Y, "BT_JoySlow", &m_stBTAxis_Y);
				SetJoystickOn(TRUE, TRUE, TRUE);
				m_lJoystickLevel = 0;

				szMsg = _T("3rd die placement fail; please specify last-die position with joystick, then press CONTINUE.");		
				if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
				{
					SwitchToBPR();
					//SetAlarmLamp_Yellow();
					return FALSE;
				}

				SetJoystickOn(FALSE);
				Sleep(100);

				GetXYEncoderValue(lPosX2, lPosY2, TRUE);

				lNewXOffset = lNewXOffset - ConvertXEncoderValueToFileUnit(lPosX2 - lPosX1);
				lNewYOffset = lNewYOffset - ConvertYEncoderValueToFileUnit(lPosY2 - lPosY1);

				//v4.38T6		//Bypass LastDie failure in manual RealignFrame fcn in MS109
				if (m_bAlignBinInAlive && m_bUseDualTablesOption)		
				{
					m_bManualConfirmLastDieResult2 = TRUE;
					m_lManualConfirmXOffset = lNewXOffset;
					m_lManualConfirmYOffset = lNewYOffset;
				}
			}
			else if ( (bPlacementOk == FALSE) && (IsBurnIn() == FALSE) )
			{
				if (bNewAlignMethod)	//v4.52A9	//Semitek & PLSG
				{
					LONG lPosX1 = 0, lPosX2 = 0;
					LONG lPosY1 = 0, lPosY2 = 0;
					GetXYEncoderValue(lPosX1, lPosY1, TRUE);

					szLog = "CASE #2a (AUTO): follow Last-Die offset XY";
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
					
					LONG lXInStep = (LONG) dX;
					LONG lYInStep = (LONG) dY;
					X2_Move(lXInStep, FALSE);
					Y2_Move(lYInStep);
					X2_Sync();

					Sleep(100);
					GetXYEncoderValue(lPosX2, lPosY2, TRUE);
					lNewXOffset = lNewXOffset - ConvertXEncoderValueToFileUnit(lPosX2 - lPosX1);
					lNewYOffset = lNewYOffset - ConvertYEncoderValueToFileUnit(lPosY2 - lPosY1);

					//v4.38T7		//Bypass LastDie failure in manual RealignFrame fcn in MS109
					if (m_bUseDualTablesOption)	
					{
						m_bManualConfirmLastDieResult1 = TRUE;
						m_lManualConfirmXOffset = lNewXOffset;
						m_lManualConfirmYOffset = lNewYOffset;
					}					
				}
				else
				{
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Last Die pos placement fails\n", "a+");
					
					szMsg.Format("dX = %.2f steps, dY = %.2f steps.", dX, dY);
					SetAlert_Red_Yellow(IDS_BT_REALIGN_LAST_DIE_SHIFT);
					SwitchToBPR();
					return FALSE;
				}
			}

			LONG PosX,PosY;
		    GetXYEncoderValue(PosX, PosY, TRUE);
			m_lX = PosX;
			m_lY = PosY;

			//v4.39T9
			szLog.Format("BT2 3pt Realign Result: EncX = %ld, EncY = %ld",  m_lX, m_lY);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
		else if( lDieFound1!=TRUE && lDieFound2!=TRUE )		//v2.56
		{
			if (IsBurnIn())		//v3.34
			{
				bRealignFailed = FALSE;
			}
			else
			{
				//v2.63		//andrew: will change to ALARM type later!!
				CString szMsg = " ";
				//pllm
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && !m_bAlignBinInAlive)	//xyz
				{
					SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
					SwitchToBPR();
					return FALSE;
				}
				else
				{
					if ( SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Continue", "Stop") != 1 )
					{
						//SetAlarmLamp_Yellow();
						SwitchToBPR();
						return FALSE;
					}
				}

				//v4.36		//Bypass LastDie failure in manual RealignFrame fcn in MS109
				if (m_bAlignBinInAlive && m_bUseDualTablesOption)		
				{
					m_bManualConfirmLastDieResult2 = TRUE;
					m_lManualConfirmXOffset = lNewXOffset;
					m_lManualConfirmYOffset = lNewYOffset;
				}
			}
		}
		else
		{
			bRealignFailed = TRUE;
		}


		if ( bRealignFailed == TRUE )
		{
            //m_oBinBlkMain.SetIsAligned(ulBlkInUse, bOldAligned);		//v3.44
			if (! IsBurnIn())
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE);
			}

			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-align failed\n", "a+"); 

			SwitchToBPR();
			//SetJoystickOn(TRUE);
            return FALSE;
        }
    }

	szLog.Format("Re-align 2 completed = %d,%d,%f\n", lNewXOffset, lNewYOffset, dNewAngle);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	if (m_bAlignBinInAlive)		//pllm
		SwitchToBPR();
    m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
    m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, dNewAngle);
    m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
    m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);

	CMS896AStn::m_bRealignFrame2Done = TRUE;
	return (TRUE);
}


LONG CBinTable::Realign1PtBinBlock2(ULONG ulBlkInUse, BOOL bAuto, BOOL bAutoTeach)
{
	BOOL bOldAligned, bUsePR;
	BOOL bRealignFailed = FALSE;
	CString str;
	CString szTitle, szContent;	
    LONG lX, lY, lDiePitchX, lDiePitchY;
    ULONG ulNoOfSortedDie, ulDiePerRow, ulDiePerCol, ulDieIndex, ulFirstDieIndex=0;
	LONG lDieFound1, lDieFound2;
	LONG DieALogical_x, DieALogical_y, DieAPhysical_x, DieAPhysical_y;
	DOUBLE  dNewAngle=0, dFOVSize;
    LONG lNewXOffset = 0, lNewYOffset = 0;
	CString szLog;

	CMS896AStn::m_bRealignFrame2Done = FALSE;

    ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ulDiePerRow     = m_oBinBlkMain.GrabNoOfDiePerRow(ulBlkInUse); 
	ulDiePerCol		= m_oBinBlkMain.GrabNoOfDiePerCol(ulBlkInUse); 
    lDiePitchX      = ConvertFileUnitToXEncoderValue( m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse) );
    lDiePitchY      = ConvertFileUnitToYEncoderValue( m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse) );
    bOldAligned		= m_oBinBlkMain.GrabNVIsAligned(ulBlkInUse);

	szTitle.LoadString(HMB_BT_REALIGN_BIN_BLOCK);			

    if( ulNoOfSortedDie==0 )
    {
		SetJoystickOn(FALSE);

		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkInUse);
		m_oBinBlkMain.StepMove(ulBlkInUse, ulFirstDieIndex, lX, lY);
		BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY), FALSE);

		if( bAuto==FALSE && !IsBurnIn())
        {
			str.Format("%d", ulBlkInUse);
			SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_ON_BLOCK, str);
        }

		szLog.Format("1-point Re-align 2: blk%d, no of sorted die = 0\n", ulBlkInUse);		
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
	
		m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
		CMS896AStn::m_bRealignFrame2Done = TRUE;
        return TRUE;
    }

	//check FOV size for this binblk
	dFOVSize = GetBPRFOVSize(ulBlkInUse);
	BOOL bLargeDie = IsLargeDieSize();
	
	szLog.Format("1-point Re-align 2 start FOV=[%3.2f]; Count=%d; Alive=%d", dFOVSize, ulNoOfSortedDie, m_bAlignBinInAlive);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

    m_oBinBlkMain.SetIsAligned(ulBlkInUse, FALSE);

    //Check Postbond pr status
    bUsePR = CheckBPRStatus();

	// Display Search window
	BOOL bLF = FALSE;
	if (dFOVSize >= BPR_LF_SIZE)
	{
		bLF = TRUE;
		bLargeDie = FALSE;		//v2.67	//prefer to use LA even if Large-Die
	}

    // off bin table joy stick
	if (m_bAlignBinInAlive)			//xyz
		SwitchToBPR();
	SetJoystickOn(FALSE);


	//Move table to last die directly
	LONG l3rdDieX = 0, l3rdDieY = 0;

	if (bUsePR == FALSE)
	{
        // just move bin table to last die of this block, use realign result
        ulDieIndex = ulNoOfSortedDie;

		//update the die index when 2D barcode is used for outout bin					//v3.34T1
		ULONG ulLastIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);		
		for (INT i = 1; i < (INT) ulLastIndex; i++)		
		{
			for (INT j = 0; j < (INT) m_ulHoleDieNum; j++)									
			{
				if	(m_oBinBlkMain.GrabRandomHoleDieIndex(ulBlkInUse, j) == i)
					ulDieIndex++;
			}
		}

		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		szLog.Format("Directly Confirm last die (no PR), INDEX = #%d", ulDieIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

		//v4.01
		if ( m_oBinBlkMain.GrabLSBondPattern() )
		{
			ULONG ulNextIndex = 0;
			ULONG ulOriginalIndex = ulDieIndex;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulDieIndex, ulNextIndex))
				ulDieIndex = ulNextIndex;
			
			szLog.Format("Last die uses LS pattern; orig-INDEX = %#d; new-INDEX = #%d", ulOriginalIndex, ulDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}

        m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
        //ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);
        BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
	}
	else
    {
		SetJoystickOn(FALSE);

        // move bin table to last die of this block, use realign result
        //update the die index when 2D barcode is used for outout bin					//v3.34T1
		ulDieIndex = ulNoOfSortedDie +  m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);

		//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Directly Confirm last die", "a+");
		szLog.Format("Directly Confirm last die (PR), INDEX = #%d", ulDieIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

		//v4.01
		if ( m_oBinBlkMain.GrabLSBondPattern() )
		{
			ULONG ulNextIndex = 0;
			ULONG ulOriginalIndex = ulDieIndex;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulDieIndex, ulNextIndex))
				ulDieIndex = ulNextIndex;

			szLog.Format("Last die uses LS pattern; orig-INDEX = #%d; new-INDEX = #%d", ulOriginalIndex, ulDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
		}

		m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
		
        DieALogical_x = ConvertFileUnitToXEncoderValue(lX);
		DieALogical_y = ConvertFileUnitToYEncoderValue(lY);
		BT2_MoveTo(DieALogical_x, DieALogical_y);
		
		// Calculate bin table to last+1 die of this block, use realign result
		ulDieIndex = ulNoOfSortedDie + m_oBinBlkMain.GetCurrHoleDieNum(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetFirstRowColSkipPatternSkippedUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + m_oBinBlkMain.GetWafflePadSkipUnitCount(ulBlkInUse);
		ulDieIndex = ulDieIndex + 1;

		//v4.01
		if ( m_oBinBlkMain.GrabLSBondPattern() )
		{
			ULONG ulNextIndex = 0;
			ULONG ulOriginalIndex = ulDieIndex;

			if (m_oBinBlkMain.GrabLSBondPatternIndex(ulBlkInUse, ulDieIndex, ulNextIndex))
				ulDieIndex = ulNextIndex;

			szLog.Format("NEXT die uses LS pattern; orig-INDEX = #%d; new-INDEX = #%d", ulOriginalIndex, ulDieIndex);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 
		}

        m_oBinBlkMain.StepMove(ulBlkInUse, ulDieIndex, lX, lY);
        //ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);


		// Align Last-die and return its XY placement
		BOOL bPlacementOk = TRUE;
		DOUBLE dX=0, dY=0;
		Sleep(m_lAlignBinDelay);
		lDieFound1 = Search3rdGoodDie2(bPlacementOk, dX, dY);	

if (m_bTestRealign || IsBurnIn())	//Test-Mode only
{
	lDieFound1 = TRUE;
	bPlacementOk = TRUE;
	dX = 0;
	dY = 0;
}

		GetXYEncoderValue(DieAPhysical_x, DieAPhysical_y, TRUE);

	    // convert die offset in um unit
		lNewXOffset = -ConvertXEncoderValueToFileUnit(DieAPhysical_x-DieALogical_x);
		lNewYOffset = -ConvertYEncoderValueToFileUnit(DieAPhysical_y-DieALogical_y);

		//xyz
		if (!m_b1ptRealignComp)		//v3.86	//Add Offset compensation option
		{
			lNewXOffset = 0;
			lNewYOffset = 0;
			dNewAngle = 0.00;
		}

		LONG lLfDieOK = TRUE;	//Always disable lookahead function on 3rd die

		ConvertBinTablePosn(lX, lY, ulBlkInUse, dNewAngle, lNewXOffset, lNewYOffset);

		// move bin table to last+1 die of this block, use realign result
        BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
		DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	
		Sleep(m_lAlignBinDelay);	
		lDieFound2 = SearchGoodDie(TRUE, TRUE);

if (m_bTestRealign || IsBurnIn())		//v3.70T4	//v3.94		//Test-Mode only
{
	lDieFound2 = FALSE;
}

		szLog.Format("Last-Die status = %d; NEXT-die status = %d", lDieFound1, lDieFound2);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); 

		// Determine last-die result
        if( lDieFound1==TRUE && lDieFound2!=TRUE )
		{
			CString szMsg;
			if ( !bPlacementOk && !IsBurnIn() && m_b1ptRealignComp && m_bAlignBinInAlive )		//v3.62		//v3.86	
			{
				szLog.Format("dX = %.2f steps, dY = %.2f steps.", dX, dY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

				LONG lPosX1=0, lPosX2=0;
				LONG lPosY1=0, lPosY2=0;
				GetXYEncoderValue(lPosX1, lPosY1, TRUE);

				SwitchToBPR();
				DisplayBondPrDieSizeWindow(TRUE);

				//Ask user to die pos
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_X, "BT_JoySlow", &m_stBTAxis_X);
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_Y, "BT_JoySlow", &m_stBTAxis_Y);
				SetJoystickOn(TRUE, TRUE, TRUE);

				szMsg = _T("last-die placement fail; please specify last-die position with joystick, then press CONTINUE.");		
				if (HmiMessageEx_Red_Back(szMsg, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_CONTINUE)
				{
					SwitchToBPR();
					//SetAlarmLamp_Yellow();
					return FALSE;
				}

				SetJoystickOn(FALSE);
				Sleep(100);

				GetXYEncoderValue(lPosX2, lPosY2, TRUE);

				lNewXOffset = lNewXOffset - ConvertXEncoderValueToFileUnit(lPosX2 - lPosX1);
				lNewYOffset = lNewYOffset - ConvertYEncoderValueToFileUnit(lPosY2 - lPosY1);

			}
			else if ( (bPlacementOk == FALSE) && (IsBurnIn() == FALSE) )
			{
				szMsg.Format("Placement fails - dX = %.2f steps, dY = %.2f steps.\n", dX, dY);
				CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");  

				SwitchToBPR();
				SetAlert_Red_Yellow(IDS_BT_REALIGN_LAST_DIE_SHIFT);
				SwitchToBPR();
				return FALSE;
			}

			LONG PosX,PosY;
		    GetXYEncoderValue(PosX, PosY, TRUE);
			m_lX = PosX;
			m_lY = PosY;
		}
		else if( lDieFound1!=TRUE && lDieFound2!=TRUE )
		{
			if (IsBurnIn())
			{
				bRealignFailed = FALSE;
			}
			else
			{
				//pllm
				CString szMsg = "  ";
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && !m_bAlignBinInAlive)	//xyz
				{
					szMsg = "Re-align fails due to last-die (PLLM REBEL)\n";
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

					SetAlert_Msg_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Stop");
					SwitchToBPR();
					return FALSE;
				}
				else
				{
					CString szMsg = " ";
					SwitchToBPR();
					DisplayBondPrDieSizeWindow(TRUE);

					szMsg = "Re-align fails due to last-die\n";
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");

					if ( SetAlert_Msg_Red_Back(IDS_BT_REALIGN_NO_LAST_DIE, szMsg, "Continue", "Stop") != 1 )
					{
						//SetAlarmLamp_Yellow();
						return FALSE;
					}
				}
			}
		}
		else
		{
			bRealignFailed = TRUE;
		}

		if ( bRealignFailed == TRUE )
		{
			SwitchToBPR();
			if (! IsBurnIn())
			{
				SetAlert_Red_Yellow(IDS_BT_REALIGN_NO_LAST_DIE);
			}

			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog("Re-align failed\n", "a+"); 
            return FALSE;
        }
    }

	//xyz
	if (!m_b1ptRealignComp)		//v3.86		//Added compensation option for 1pt realignment
	{
		lNewXOffset = 0;
		lNewYOffset = 0;
		dNewAngle = 0.00;
	}

	szLog.Format("4Re-align completed = %d,%d,%f\n", lNewXOffset, lNewYOffset, dNewAngle);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

	//SwitchToBPR();
    m_oBinBlkMain.SetIsAligned(ulBlkInUse, TRUE);
    m_oBinBlkMain.SetRotateAngleX(ulBlkInUse, dNewAngle);
    m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
    m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);

	CMS896AStn::m_bRealignFrame2Done = TRUE;
	return (TRUE);
}


LONG CBinTable::BT2_MoveTo(LONG lX, LONG lY, CONST BOOL bWait)
{
    CString str;
	INT nProf_X, nProf_Y;
	LONG lDiff_X, lDiff_Y;

	//Added BT2 Offset for correct table positions relative to BT1
	LONG lBT2X = lX + m_lBT2OffsetX;
	LONG lBT2Y = lY + m_lBT2OffsetY;

    if( lBT2X>m_lTableX2PosLimit || lBT2X<m_lTableX2NegLimit )
    {
        str.Format("BT2 X : %d would hit position limit(%d--%d)", lBT2X, m_lTableX2PosLimit, m_lTableX2NegLimit);
		SetErrorMessage(str);
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
        return FALSE;
    }
    if( lBT2Y>m_lTableY2PosLimit || lBT2Y<m_lTableY2NegLimit )
    {
        str.Format("BT2 Y : %d would hit position limit(%d--%d)", lBT2Y, m_lTableY2PosLimit, m_lTableY2NegLimit);
		SetErrorMessage(str);
		//HmiMessage(str, "Re-align Binblock Message", glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
        return FALSE;
    }

	if (IsTable1InBondRegion())
	{
		str = "ERROR (BT2_MoveTo): BT1 in BOND region; BT2 cannot move!";
		HmiMessage(str);
		return FALSE;
	}


	GetEncoderValue();
	lDiff_X = labs(m_lEnc_X2 - lBT2X);
	lDiff_Y = labs(m_lEnc_Y2 - lBT2Y);

	if (lDiff_X <= m_lMinTravel_X)
		nProf_X = FAST_PROF;
	else
		nProf_X = NORMAL_PROF;
	if (lDiff_Y <= m_lMinTravel_Y)
		nProf_Y = FAST_PROF;
	else
		nProf_Y = NORMAL_PROF;


	X2_Profile(nProf_X);
	Y2_Profile(nProf_Y);

	if (!bWait)
	   XY2_MoveTo(lBT2X, lBT2Y, SFM_NOWAIT);
	else
	   XY2_MoveTo(lBT2X, lBT2Y);

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);
    return TRUE;
}

LONG CBinTable::SearchBT2FirstGoodDie(ULONG ulBlkInUse)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	int nConvID = 0;
    LONG lX, lY;
	IPC_CServiceMessage stMsg;
    stInfo.bResult = FALSE;


	//Search any die on current BT position
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchFirstDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

    if( stInfo.bResult )
    {
		//Return if 1st die is located on 1st try
		GetXYEncoderValue(lX, lY, TRUE);
		BT2_MoveTo(lX+stInfo.siStepX, lY+stInfo.siStepY);
		return TRUE;
    }
	else
    {
		//No need to search around if in BURNIN mode
		if (IsBurnIn())
			return FALSE;

		//Otherwise, perform spiral search 1st die with Max PR search region
		short i, j, k;
		LONG lPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabDDiePitchX(ulBlkInUse));
		LONG lPitchY = ConvertFileUnitToYEncoderValue(m_oBinBlkMain.GrabDDiePitchY(ulBlkInUse));

		for(i=1; i<5; i++)
		{
			GetXYEncoderValue(lX, lY, TRUE);
			BT2_MoveTo(lX-lPitchX*BT_X_DIR, lY-lPitchY*BT_Y_DIR);

			nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchFirstDie", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
					break;
				}
				else
				{
					Sleep(1);
				}
			}

			if( stInfo.bResult )
			{
				GetXYEncoderValue(lX, lY, TRUE);
				BT2_MoveTo(lX+stInfo.siStepX, lY+stInfo.siStepY);
				return TRUE;
			}


			k = i*2;
			for(j=0; j<k; j++) // a
			{
				GetXYEncoderValue(lX, lY, TRUE);
				BT2_MoveTo(lX, lY+lPitchY*BT_Y_DIR);

				nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchFirstDie", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID,stMsg);
						stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
						break;
					}
					else
					{
						Sleep(1);
					}
				}

				if( stInfo.bResult )
				{
					GetXYEncoderValue(lX, lY, TRUE);
					BT2_MoveTo(lX+stInfo.siStepX, lY+stInfo.siStepY);
					return TRUE;
				}
			}

			for(j=0; j<k; j++) // b
			{
				GetXYEncoderValue(lX, lY, TRUE);
				BT2_MoveTo(lX+lPitchX*BT_X_DIR, lY);

				nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchFirstDie", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID,stMsg);
						stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
						break;
					}
					else
					{
						Sleep(1);
					}
				}

				if( stInfo.bResult )
				{
					GetXYEncoderValue(lX, lY, TRUE);
					BT2_MoveTo(lX+stInfo.siStepX, lY+stInfo.siStepY);
					return TRUE;
				}
			}

			for(j=0; j<k; j++) // c
			{
				GetXYEncoderValue(lX, lY, TRUE);
				BT2_MoveTo(lX, lY-lPitchY*BT_Y_DIR);

				nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchFirstDie", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID,stMsg);
						stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
						break;
					}
					else
					{
						Sleep(1);
					}
				}

				if( stInfo.bResult )
				{
					GetXYEncoderValue(lX, lY, TRUE);
					BT2_MoveTo(lX+stInfo.siStepX, lY+stInfo.siStepY);
					return TRUE;
				}
			}

			for(j=0; j<k; j++) // d
			{
				GetXYEncoderValue(lX, lY, TRUE);
				BT2_MoveTo(lX-lPitchX*BT_X_DIR, lY);

				nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchFirstDie", stMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID,stMsg);
						stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
						break;
					}
					else
					{
						Sleep(1);
					}
				}

				if( stInfo.bResult )
				{
					GetXYEncoderValue(lX, lY, TRUE);
					BT2_MoveTo(lX+stInfo.siStepX, lY+stInfo.siStepY);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

LONG CBinTable::Search3rdGoodDie2(BOOL& bPlacement, DOUBLE& dX, DOUBLE& dY)
{
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;

	int nConvID = 0;
    LONG lX, lY;
	IPC_CServiceMessage stMsg;
    stInfo.bResult = FALSE;
	bPlacement = TRUE;

	// Get the reply
    nConvID = m_comClient.SendRequest(BOND_PR_STN, "BT2_SearchDie", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

    if (!stInfo.bResult){
		return FALSE;
	}


	//Convert motor into mil	
	double dXinMil=0, dYinMil=0;
	dXinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE2_X) * (DOUBLE)stInfo.siStepX * 1000 / 25.4);
	dYinMil = (GetChannelResolution(MS896A_CFG_CH_BINTABLE2_Y) * (DOUBLE)stInfo.siStepY * 1000 / 25.4);
	dX = stInfo.siStepX;	//dXinMil;
	dY = stInfo.siStepY;	//dYinMil;

	DOUBLE dLastDieOffsetX = m_dAlignLastDieOffsetX;
	DOUBLE dLastDieOffsetY = m_dAlignLastDieOffsetY;

//v4.52A9	//PLSG
dLastDieOffsetX = 0;
//dLastDieOffsetY = 0;

	if ( (m_dAlignLastDieOffsetX == 0.0) && (m_dAlignLastDieOffsetY == 0.0) )
	{
	    bPlacement = TRUE;
	}
	else if ((dLastDieOffsetX > 0.0) && (fabs(dXinMil) > (dLastDieOffsetX)) )		//v4.52A9
	{
		bPlacement = FALSE;
	}	
	else if ((dLastDieOffsetY > 0.0) && (fabs(dYinMil) > (dLastDieOffsetY)) )		//v4.52A9
	{
		bPlacement = FALSE;
	}


	CString szLog = "";
	szLog.Format("Search BT2 last die dX = %.2f mil (%.2f), dY = %.2f mil (%.2f)",
		dXinMil, m_dAlignLastDieOffsetX,	//dLastDieOffsetX,
		dYinMil, m_dAlignLastDieOffsetY);	//, dLastDieOffsetY);
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	
    GetXYEncoderValue(lX, lY, TRUE);
    BT2_MoveTo(lX+stInfo.siStepX, lY+stInfo.siStepY);

szLog.Format("REALIGN LastDie ENC POS XY = ,%d,%d,  Offset = ,%d,%d", 
					lX+stInfo.siStepX, lY+stInfo.siStepY, stInfo.siStepX, stInfo.siStepY);
CMSLogFileUtility::Instance()->BT_LogStatus(szLog);

    return TRUE;
}

// huga
LONG CBinTable::AlignEmptyFrame2(ULONG ulBlkID)
{
	ULONG ulFirstDieIndex = 0;
	LONG lX = 0, lY = 0;
	LONG lDieFound = 0;
	CString str = "";
	BOOL bPostbond = FALSE;

	bPostbond = (BOOL)((LONG)(*m_psmfSRam)["BondPr"]["UsePostBond"]);

	CMS896AStn::m_bRealignFrameDone = FALSE;

	if (bPostbond == FALSE)
	{
		ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkID);
		m_oBinBlkMain.StepMove(ulBlkID, ulFirstDieIndex, lX, lY);

		BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
		CMS896AStn::m_bRealignFrameDone = TRUE;
		return TRUE;
	}

	ulFirstDieIndex = 1 + m_oBinBlkMain.GrabSkipUnit(ulBlkID);
	m_oBinBlkMain.StepMove(ulBlkID, ulFirstDieIndex, lX, lY);

	BT2_MoveTo(ConvertFileUnitToXEncoderValue(lX), ConvertFileUnitToYEncoderValue(lY));
	DisplayBondPrSearchWindow(TRUE, FALSE, BT_PR_CENTER);	//v2.58

	if (m_lAlignBinDelay > 0)
	{
		Sleep(m_lAlignBinDelay);
	}

	if ((SearchGoodDie(FALSE) == TRUE) && (!IsBurnIn()))
	{
		str.Format("%d", ulBlkID);
		SetAlert_Msg_Red_Yellow(IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN, str);
		return IDS_BT_DIE_IS_FOUND_ON_EMPTY_BIN;
	}

	CMS896AStn::m_bRealignFrameDone = TRUE;
	return TRUE;
}
*/