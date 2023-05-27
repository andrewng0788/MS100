/////////////////////////////////////////////////////////////////
// BT_SubState.cpp : SubState for Auto-cycle of the CBinTable class
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
#include "BT_Log.h"
#include "PrescanUtility.h"
#include "WT_SubRegion.h"
#include "MS_SecCommConstant.h"
#include "BinLoader.h"
#include "WaferMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Sequence Operations
////////////////////////////////////////////

INT CBinTable::OpInitialize()
{
	INT nResult					= gnOK;

//	if ((nResult = XY_Home()) == gnOK)
	{
		m_qPreviousSubOperation	= -1;
		m_qNextSubOperation = -1;
		m_qSubOperation			= 0;

		NeedReset(FALSE);
	}

	DisplaySequence("BT - Operation Initialize");
	return nResult;
}

INT CBinTable::OpPreStart()
{
	CString szStr;
	ULONG ulPhyBlkInUse;
	INT nResult				= gnOK;

	SetUseBinMapBondArea();

	m_qNextSubOperation = -1;
	m_ulNewBondedCounter	= 0;	// HuaMao2

	if ((m_fHardware == FALSE) || (m_bDisableBT))	//v3.60
	{
		m_bHome_X = TRUE;
		m_bHome_Y = TRUE;
		m_bComm_X = TRUE;
		m_bComm_Y = TRUE;
	}

	SetJoystickOn(FALSE);

	if ( IsMotionHardwareReady() == TRUE )
	{
		m_qSubOperation	= WAIT_DIE_READY_Q;
	}
	else
	{
		DisplaySequence("BT - Hardware not ready ==> house keeping.");
		m_qSubOperation	= HOUSE_KEEPING_Q;
	}

	CMS896AApp* pAppM = (CMS896AApp*) AfxGetApp();
	if (IsMS90HalfSortMode() && IsBLEnable() && (pAppM->GetCustomerName()=="WolfSpeed" || pAppM->GetCustomerName()=="Finisar)"))	//	MS90, prestart, do bin realignment anyway.
	{
		IPC_CServiceMessage rReqMsg;
		BOOL bFrameExist = FALSE;
		int nConvID2 = m_comClient.SendRequest(BIN_LOADER_STN, "CheckFrameExistOnBinTable", rReqMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID2, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID2, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bFrameExist);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

	//	if( bFrameExist==FALSE )
		{
			if (IsMS90Sorting2ndPart())
			{
				BT_SetFrameRotation(0, TRUE, TRUE);
			}
			else
			{
				BT_SetFrameRotation(0, FALSE, TRUE);
			}
		}
	}

	DecodeSubBinSetting();
	SetDieReadyForBT(FALSE);
	SetBTStartMoveWithoutDelay(FALSE);
	SetBTStartMove(FALSE);
	SetBTReady(FALSE);
	SetBTStable(FALSE);
	SetBLPreChangeGrade(FALSE);				//v2.71
	SetBTCompensate(FALSE);					//v4.52A16
	m_lBTCompCounter = 0;
	m_bPreLoadNextMapGradeFrame = FALSE;	//v2.78T1
	//m_szCycleMessageBT		= "";
	m_stStartChangeTime	= CTime::GetCurrentTime();
	m_unChangeStage		= 0;
	m_ulBondedBlk = 0;						
	m_ulBpBondedBlk	= 0;					//v2.78T1
	SetBLReady(TRUE);						//v2.93T2
	m_dBTStartTime = GetTime();
	m_lBTMoveDelayTime = 0;

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	if (m_bUseDualTablesOption)				//v4.20
	{
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);
	}

	//MS100 9Inch DUal-Table settings 
	if (!m_bUseDualTablesOption)					//v4.24
		m_nBTInUse		= 0;	//0=DEFAULT=BT1		//v4.20
	(*m_psmfSRam)["BinTable"]["BTInUse"] =	(LONG) m_nBTInUse;
	m_lWftAdvSamplingStage	= 0;
	//v4.21
	if (m_bUseDualTablesOption)
	{
		//if (m_nBTInUse == 1)	//If BT2 is in use
		//	HomeTable1();
		//else
		HomeTable2();
	}

	//Reset state value
	m_bIsUnload = FALSE;
	m_ulBinBlkFull	= 0;
	(*m_psmfSRam)["BinTable"]["SPC"]["MoveBack"]	= FALSE;
	(*m_psmfSRam)["BinTable"]["BlkInUse"]			= 0;
	(*m_psmfSRam)["BinTable"]["LastBlkInUse"]		= 0;
	(*m_psmfSRam)["BinTable"]["BondingGrade"]		= 0;
	(*m_psmfSRam)["BinTable"]["SortDir"]			= 0;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN		//CSP
	(*m_psmfSRam)["BinTable"]["BinFull"]			= FALSE;
	(*m_psmfSRam)["BinTable"]["NextBinFull"]		= FALSE;	//v4.18T1	//Cree HuiZhou
	(*m_psmfSRam)["BinTable"]["GradeChanged"]		= FALSE;
	//(*m_psmfSRam)["BinTable"]["BinChangedForBH"]	= FALSE;	//v4.49A11	//GelPad
	(*m_psmfSRam)["BinTable"]["BinChanged"]			= FALSE;
	(*m_psmfSRam)["BinTable"]["Die Bonded X"]		= -1;
	(*m_psmfSRam)["BinTable"]["Die Bonded Y"]		= -1;
	(*m_psmfSRam)["BinTable"]["MissingDie"]			= FALSE;
	(*m_psmfSRam)["BinTable"]["AtSafePos"]			= 0;		//v3.67T3	//DBH
	(*m_psmfSRam)["BinTable"]["PreBondDone"]		= FALSE;	//v3.83
	(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"]	= FALSE;	//v3.66
	(*m_psmfSRam)["BinTable"]["Change Grade"]		= FALSE;	//v3.98T3		
	(*m_psmfSRam)["BinTable"]["BinMapLastRowDie"]	= FALSE;	//v4.57A13
	(*m_psmfSRam)["BondPr"]["BinTable At BH1"]		= TRUE;		//v4.48A19
	(*m_psmfSRam)["BinTable"]["ThetaCorrection"]	= 0;
	
	//v4.55A10
	(*m_psmfSRam)["BinTable"]["AddPrePickOffsetTo2ndDie"]	= FALSE;
	(*m_psmfSRam)["BinTable"]["Collet1PrePickOffsetX"]		= 0;
	(*m_psmfSRam)["BinTable"]["Collet2PrePickOffsetX"]		= 0;
	m_lBHZ1CurrPrePickOffsetX	= 0;
	m_lBHZ2CurrPrePickOffsetX	= 0;

	//v4.57A12
	SaveBTCollet1Offset(m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY, m_dXResolution_UM_CNT);
	SaveBTCollet2Offset(m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY, m_dXResolution_UM_CNT);
	SaveBTBond180DegOffset(m_dBond180DegOffsetX_um, m_dBond180DegOffsetY_um);
	m_bFirstCycleBT = TRUE;		//v3.66
	(*m_psmfSRam)["WaferTable"]["WaferEndSmooth"]	= TRUE;
	(*m_psmfSRam)["BondHead"]["AtPrePickAndWftReady"] = 0;
	m_lLoopCount = 0;

	m_bNVRamUpdatedInIfBondOK			= FALSE;		//v4.59A4
	CMS896AStn::m_lBinTableStopped		= 0;
	CMS896AStn::m_bBTAskBLChangeGrade	= FALSE;		//v4.43T9
	CMS896AStn::m_bBTAskBLBinFull		= FALSE;		//v4.43T9
	m_bIfNeedUpdateBinFrameAngle		= FALSE;		//v4.44T3	//Semitek
	m_lPreBondTableOffsetX				= 0;
	m_lPreBondTableOffsetY				= 0;

	//Update Bin & wafer statistic
	LoadWaferStatistics();

	// Check all Bin Blocks for Bin Full
	LONG	lRow=0, lCol=0;
	DOUBLE	dX=0, dY=0;
	ULONG	ulBinBlk = 0;
	ULONG	ulBlkStatus=0;
	LONG	lSortDir = 0;

	//v4.52A6	//CYOptics, Inari
	BOOL bCheckEmptyRowFcn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW)) ||
		(pApp->GetCustomerName() == "CyOptics")		|| 
		(pApp->GetCustomerName() == "FiberOptics")	||		// = Dicon
		(pApp->GetCustomerName() == "Inari")	)	
	{
		bCheckEmptyRowFcn = TRUE;
	}


	//v4.23T1	//Re-enable for Cree HuiZhou again
	if( m_bDisableBT==FALSE )
	{
		UCHAR ucGrade = 0;
		for ( ulBinBlk=1; ulBinBlk<=MS_MAX_BIN; ulBinBlk++ )
		{
			//ulBlkStatus = GrabBondXYPosn(GetBinBlkGrade(ulBinBlk), lX, lY, lRow, lCol);
			//v4.52A6
			if (bCheckEmptyRowFcn)		
			{
				ucGrade = m_oBinBlkMain.GrabGrade(ulBinBlk);
				if (m_oBinBlkMain.GrabInputCount(ucGrade) <= m_oBinBlkMain.GrabNVCurrBondIndex(ulBinBlk))
				{
					ulBlkStatus = 0;	//FULL
				}
				else
				{
					ulBlkStatus = ulBinBlk;
				}
			}
			else
			{
				ulBlkStatus = GrabBondXYPosn(GetBinBlkGrade(ulBinBlk), dX, dY, lRow, lCol, lSortDir);
			}
			
			if (ulBlkStatus == 0)
			{
				(*m_psmfSRam)["BinTable"]["BinFull"] = TRUE;
				m_ucWTGrade = GetBinBlkGrade(ulBinBlk);	
				ulPhyBlkInUse = (*m_psmfSRam)["BinTable"]["PhyBlkInUse"];
				szStr.Format("PreStart bin full Blk:%d, PhyBlk%d Grade:%d", ulBinBlk, ulPhyBlkInUse, GetWTGrade());
				CMSLogFileUtility::Instance()->BL_LogStatus(szStr);
				break;
			}
		}
	}

	GetWafflePadSettings();

	//Save Start map time
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	if ( pAppMod != NULL )
	{
		pAppMod->MapIsStarted();
	}

	//if ( (m_ulMachineType == 2) || (m_ulMachineType == 3) )		//v3.71T4
	if ( m_ulMachineType >= BT_MACHTYPE_DL_DLA )					//v4.16T1
		CheckSlotAssignment();

	//Check to enable NV RunTime data protection or not
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if ( ( (pUtl->GetPrescanDummyMap() || IsEnablePNP()) && m_bEnablePickNPlaceOutputFile==FALSE) || (IsBurnIn()) )
	{
		m_oBinBlkMain.EnableNVRunTimeData(FALSE);
	}
	else
	{
		m_oBinBlkMain.EnableNVRunTimeData(TRUE);
	}

	UINT unToGenTemp = pApp->GetProfileInt(gszPROFILE_SETTING, _T("Gen Temp File Bond Die Counter"), 500);
	if (unToGenTemp < 1 || unToGenTemp > 500)
	{
		unToGenTemp = 500;
	}
	pApp->WriteProfileInt(gszPROFILE_SETTING, _T("Gen Temp File Bond Die Counter"), unToGenTemp);
	m_oBinBlkMain.m_ulBondCountToGenTempFile = unToGenTemp;

	//v3.86
	CMSLogFileUtility::Instance()->BT_BackupLogStatus();
	CMSLogFileUtility::Instance()->BT_BackupReAlignBinFrameLog();
	//OPENCLOSE_BT_LOG(TRUE);		//v3.86T5

	DisplaySequence("BT - Operation Prestart");
	return nResult;
}

////////////////////////////////////////////
//	Index State 
////////////////////////////////////////////

INT CBinTable::OpIndex(BOOL bLogMsg)
{
	CString	szTemp;
	CString szLog;	
	LONG lMapNextRow = (*m_psmfSRam)["WaferMap"]["Y"];
	LONG lMapNextCol = (*m_psmfSRam)["WaferMap"]["X"];

	unsigned long	ulLastBlk	= (*m_psmfSRam)["BinTable"]["LastBlkInUse"];
	unsigned long	ulBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];


	//if( lMapNextRow==-1 && lMapNextCol==-1 )
	//{
	//	return -1;
	//}
	//(*m_psmfSRam)["WaferMap"]["X"] = -1;
	//(*m_psmfSRam)["WaferMap"]["Y"] = -1;

	// Get the current encoder count
	GetEncoderValue();
	if (m_nBTInUse == 1)	//If BT2	//v4.20
	{
		m_lLastX2 = m_lEnc_X2;
		m_lLastY2 = m_lEnc_Y2;
	}
	else
	{
		m_lLastX = m_lEnc_X;
		m_lLastY = m_lEnc_Y;
	}

	// Get the data to SRAM String Map File
	CString szMsg;
	m_lWT_Col		= lMapNextCol;
	m_lWT_Row		= lMapNextRow;
	UCHAR ucGrade = (*m_psmfSRam)["WaferMap"]["Grade"];
	m_ucWTGrade		= ucGrade - m_WaferMapWrapper.GetGradeOffset();
	//CString szMsg;
	szMsg.Format("BT - map (%ld,%ld)%ld Index, BLK = %lu;BT(%d,%d)", m_lWT_Row, m_lWT_Col, GetWTGrade(), GetBTBlock(),m_lRow,m_lCol); 
	DisplaySequence(szMsg);	//v4.52A6

	if ( ucGrade < m_WaferMapWrapper.GetGradeOffset() ) 
	{
		szLog.Format("OpIndex2: LastBlkInUse = %d, BlkInUse = %d", (*m_psmfSRam)["BinTable"]["LastBlkInUse"], (*m_psmfSRam)["BinTable"]["BlkInUse"]);
		CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
		return 0;
	}

	// Get the X, Y position
	LONG lBpX = m_lX;
	LONG lBpY = m_lY;
	LONG lSortDir = 0;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
	DOUBLE dPosX=0, dPosY=0;	//v4.57A7
	m_ulBondedBlk = GrabBondXYPosn(GetWTGrade(), dPosX, dPosY, m_lRow, m_lCol, lSortDir);	//v4.57A7	//CSP

	if ((GetBTBlock() == 0) || (GetBTBlock() == Err_BinBlockNotAssign))
	{
		szLog.Format("OpIndex3: LastBlkInUse = %d, BlkInUse = %d", (*m_psmfSRam)["BinTable"]["LastBlkInUse"], (*m_psmfSRam)["BinTable"]["BlkInUse"]);
		CMSLogFileUtility::Instance()->BL_LogStatus(szLog);

		DisplaySequence("BT - OpIndex fail");	//v4.52A6
		return 0;
	}

	//v4.52A6
	
	if (CMS896AApp::m_bEnableSubBin == TRUE)
	{
		LONG ulPhyBlkInUse = 0;
		for (int p = 1; p <= m_nNoOfSubBlk; p ++)
		{
			for(int q = 1; q <= m_nNoOfSubGrade; q ++)
			{
				if(GetBTBlock() == m_nSubGrade[p][q])
				{
					ulPhyBlkInUse = m_nSubBlk[p];
					CString szLog;
					szLog.Format("GrabBondXYPosn,%d,m_nSubBlk[p],%d",ulPhyBlkInUse,m_nSubBlk[p]);
					CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				}
			}
		}
		(*m_psmfSRam)["BinTable"]["PhyBlkInUse"] = ulPhyBlkInUse;
		szMsg.Format("BT - map(%ld, %ld), Grade=%ld, BLK = %lu (AFTER GrabBondXYPosn),PhyBlk,%d", 
					m_lWT_Row, m_lWT_Col, GetWTGrade(), GetBTBlock(),ulPhyBlkInUse); 
		DisplaySequence(szMsg);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	}

	szMsg.Format("BT - map(%ld, %ld), Grade=%ld, BLK = %lu (AFTER GrabBondXYPosn)", 
					m_lWT_Row, m_lWT_Col, GetWTGrade(), GetBTBlock()); 
	DisplaySequence(szMsg);

	ULONG ulTemp = (*m_psmfSRam)["BinTable"]["BlkInUse"];
	(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= ulTemp;
	(*m_psmfSRam)["BinTable"]["BlkInUse"]		= GetBTBlock();
	(*m_psmfSRam)["BinTable"]["BondingGrade"]	= GetWTGrade();
	(*m_psmfSRam)["BinTable"]["SortDir"]		= lSortDir;		//CSP
	
	ulLastBlk	= (*m_psmfSRam)["BinTable"]["LastBlkInUse"];
	ulBlkInUse	= (*m_psmfSRam)["BinTable"]["BlkInUse"];
	ULONG ulPhyBlkInUse = (*m_psmfSRam)["BinTable"]["PhyBlkInUse"];

	//Update Sorting Grade SVID#3065 2017.11.9
	if (IsSecsGemInit()) 
	{
		(*m_pGemStation)[MS_SECS_SV_WAFER_MAP_SORTING_GRADE] = ucGrade;
	}

//	if (ulTemp != GetBTBlock())
//	{
//		//Send the current Grade Event AutoLine when Change Grade 2017.11.9	
//		SendEvent(SG_CEID_WaferMapCurPickingGrade_AUTOLINE, TRUE);
//	}

	m_lX = ConvertFileUnitToXEncoderValue(dPosX);
	m_lY = ConvertFileUnitToYEncoderValue(dPosY);

	//generate the random hole die index when the binblk in used is empty			//v3.33T3
	if (m_bEnable2DBarcodeOutput)										
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock()) == 0)
		{
			m_oBinBlkMain.GenRandomHole(GetBTBlock());
		}
	}

	//v4.44A5
	if( IsAOIOnlyMachine()==FALSE )
	{
		BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];

		if (!bBHZ2TowardsPick)	//BH2 to BOND	//v4.47T7
		{
			szLog.Format("Op Index - (Bin #%ld, Index: %lu): Enc(%ld, %ld), RowCol(%ld, %ld)", 
				GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, m_lRow, m_lCol);		//v4.59A22
		}
		else
		{
			szLog.Format("Op Index - (Bin #%ld, Index: %lu): Enc(%ld, %ld), RowCol(%ld, %ld)", 
				GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, m_lRow, m_lCol);		//v4.59A22
		}
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}

	return 1;
}

VOID CBinTable::UpdateWTGradefromBinLoader()
{
	//2018.1.18
	if (m_ucWTGrade > BT_MAX_BINBLK_NO)
	{
		UCHAR ucGrade = (*m_psmfSRam)["WaferMap"]["Grade"];
		if (ucGrade > m_WaferMapWrapper.GetGradeOffset())
		{
			m_ucWTGrade		= ucGrade - m_WaferMapWrapper.GetGradeOffset();
		}
		else
		{
			//Get it from bin block
			ULONG ulBinBlk = GetBTCurrentBlock();
			m_ucWTGrade		= GetBinBlkGrade(ulBinBlk);
		}
	}
}

INT CBinTable::OpIndex_MS90()
{

	CString szLog;
	LONG lSortDir = 0;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
	DOUBLE dPosX=0, dPosY=0;	//v4.57A7

	//2018.1.18
	UpdateWTGradefromBinLoader();

	m_ulBondedBlk = GrabBondXYPosn(GetWTGrade(), dPosX, dPosY, m_lRow, m_lCol, lSortDir);	//v4.57A7	//CSP
	if ((GetBTBlock() == 0) || (GetBTBlock() == Err_BinBlockNotAssign))
	{
		DisplaySequence("BT - OpIndex fail");	//v4.52A6
		return 0;
	}

	//v4.52A6
	//szMsg.Format("BT - map(%ld, %ld), Grade=%ld, BLK = %lu (AFTER GrabBondXYPosn)", 
	//				m_lWT_Row, m_lWT_Col, GetWTGrade(), GetBTBlock()); 
	//DisplaySequence(szMsg);

	szLog.Format("Op Index MS90 - (Bin #%ld, Index: %lu): Enc(%ld, %ld), RowCol(%ld, %ld)", 
				GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, m_lRow, m_lCol);	
	CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	//ULONG ulTemp = (*m_psmfSRam)["BinTable"]["BlkInUse"];
	//(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= ulTemp;
	//(*m_psmfSRam)["BinTable"]["BlkInUse"]		= GetBTBlock();
	//(*m_psmfSRam)["BinTable"]["BondingGrade"]	= GetWTGrade();
	(*m_psmfSRam)["BinTable"]["SortDir"]		= lSortDir;
	m_lX = ConvertFileUnitToXEncoderValue(dPosX);
	m_lY = ConvertFileUnitToYEncoderValue(dPosY);
	return 1;
}
////////////////////////////////////////////
//	Move Table State 
////////////////////////////////////////////

INT CBinTable::OpMoveTable()
{
	INT		nResult					= gnOK;
	INT		nProf_X, nProf_Y;
	LONG	lTime_X, lTime_Y, lTime_T;
	LONG	lDiff_X, lDiff_Y;
	BOOL	bBinChanged = FALSE;
	
	CString szMsg;
	szMsg.Format("BT - OpMoveTable() map (%ld,%ld)%ld (%d,%d)", m_lWT_Row, m_lWT_Col, GetWTGrade(), m_lX, m_lY);
	DisplaySequence(szMsg);
	//Check given position is within table limit or not
	if (CheckOutTableLimit(m_lX, m_lY) != 0)
	{
		SetAlert_Red_Yellow(IDS_BT_POS_OUT_LIMIT);

		//SetErrorMessage("Bintable is outside limit");
		//v4.52A6
		CString szErr;
		szErr.Format("Bintable is outside limit : (%ld, %ld) Grade=%ld MoveTable to (%d, %d)", 
			m_lWT_Row, m_lWT_Col, GetWTGrade(), m_lX, m_lY);
		SetErrorMessage(szErr);
		return gnNOTOK;
	}

	if ( m_bUseDualTablesOption && IsTable2InBondRegion() )
	{
		//SetAlert_Red_Yellow(IDS_BT_POS_OUT_LIMIT);
		HmiMessage("ERROR (Op Move Table): BT2 in BOND region; BT1 cannot move");
		SetErrorMessage("ERROR (Op Move Table): BT2 in BOND region; BT1 cannot move");
		return gnNOTOK;
	}

	bBinChanged = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinChanged"];
	if (bBinChanged == TRUE)
	{
		(*m_psmfSRam)["BinTable"]["BinChanged"] = FALSE;
		GetEncoderValue();
		m_lLastX = m_lEnc_X;
		m_lLastY = m_lEnc_Y;

		//	here to save grade/bin frame/slot/barcode ...
		if( TRUE )
		{
			//CMSLogFileUtility::Instance()->BL_LogStatus("BT - save new barcode for BYD");
			CStringMapFile pSmfFile;
			CString szCurrBC, szPrevBC;
			LONG lBCSerialNo = 0;
			LONG lCurrGrade = GetWTGrade();
			ULONG ulBondBlk = (*m_psmfSRam)["BinTable"]["BlkInUse"];
			szCurrBC = m_szLoadReadBarcode;
			if( szCurrBC.IsEmpty() )
			{
				//v4.51A17
				szCurrBC = GetBLBarcodeData(ulBondBlk);
				CMSLogFileUtility::Instance()->BL_LogStatus("BT -  bc by file in file  " + szCurrBC + "\n");
			}
			else
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("BT -  bc by load on frame " + szCurrBC + "\n");
			}
			if( pSmfFile.Open(MSD_WAFER_SORTED_BIN_BC_FILE, FALSE, TRUE)==1 )	// this would be read in wafer end DLL
			{
				lBCSerialNo = (pSmfFile)["GradeBCSerialNo"][lCurrGrade];
				if( lBCSerialNo==0 )
				{
					lBCSerialNo++;
				}
				else
				{
					szPrevBC = (pSmfFile)["GradeBarCode"][lCurrGrade][lBCSerialNo];
					if( szPrevBC!=szCurrBC )
						lBCSerialNo++;
				}
				(pSmfFile)["GradeBCSerialNo"][lCurrGrade]	= lBCSerialNo;
				(pSmfFile)["GradeBarCode"][lCurrGrade][lBCSerialNo]	= szCurrBC;

				pSmfFile.Update();
				pSmfFile.Close();
			}
			CMSLogFileUtility::Instance()->BL_LogStatus("BT -  bc by load bin table 1");
			if ( m_bEnableClearBinCopyTempFile )
			{
				(*m_psmfSRam)["BinTable"]["Copy Temp File Only"] = m_bClearBinInCopyTempFileMode;
			}
		}
	}

	lDiff_X = labs(m_lX - m_lLastX);
	lDiff_Y = labs(m_lY - m_lLastY);

	//v4.46T9	//WH SanAn PB FOV checking fcn
	if (lDiff_X > 0)	
	{
		//BT moves to right -> bonding dir to LEFT -> Lookforward RIGHT die 
		(*m_psmfSRam)["BinTable"]["IndexDirection"] = 0;
	}
	else
	{
		//BT moves to left -> bonding dir to RIGHT -> Lookforward LEFT die 
		(*m_psmfSRam)["BinTable"]["IndexDirection"] = 1;
	}

	// 400 count on X is 0.5 mm and plus some tolerance (5%) for standard X axis
	if (lDiff_X <= m_lMinTravel_X)
		nProf_X = FAST_PROF;
	else
		nProf_X = NORMAL_PROF;

	// 1000 count on Y is 0.5 mm and plus some tolerance (5%) for standard X axis
	if (lDiff_Y <= m_lMinTravel_Y)
		nProf_Y = FAST_PROF;
	else
		nProf_Y = NORMAL_PROF;
	
	X_Profile(nProf_X);
	Y_Profile(nProf_Y);

	LONG lOldX = m_lX;
	LONG lOldY = m_lY;


	OpAddPostBondOffset(m_lX, m_lY, 1);		//v3.86T5
	OpAddBHAtPrePickOffset(m_lX, m_lY);		//v4.55A10
	 

	//v4.44A5
	if( IsAOIOnlyMachine()==FALSE )
	{
		CString szLog;
		BOOL bBHZ2TowardsPick		 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
		BOOL bChangeGradeResetToBH1	 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"];		//v4.47T10
		BOOL bChangeGradeNoResetToBH1= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"];	//v4.47T17	//v4.47A3
		
		if (bChangeGradeResetToBH1)
		{
			//BH always uses BH1 to pick 1st die, so BT should stay at BH1 pos
			szLog.Format("BT MoveTable - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH1 due to ChangeGrade", 
				GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
			(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"] = FALSE;
		}
		//else if (m_bMS100EjtXY && m_bEnableMS100EjtXY && bChangeGradeNoResetToBH1)	//v4.47T17	//v4.47A3
		else if (m_bEnableMS100EjtXY && bChangeGradeNoResetToBH1)	//v4.47T17	//v4.47A3
		{
			if (!bBHZ2TowardsPick)	// -> BHZ2 towards BOND!
			{
				szLog.Format("BT MoveTable - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH1 due to ChangeGrade (EjtXY)", 
					GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
				(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = TRUE;		//v4.48A19
			}
			else
			{
				szLog.Format("BT MoveTable - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH2 due to ChangeGrade (EjtXY)", 
					GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
				(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = FALSE;		//v4.48A19
			}
			(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"] = FALSE;
		}
		else
		{
			if (m_bFirstCycleBT)	//v4.47A1
			{
				szLog.Format("BT MoveTable (1st) - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH1", 
					GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
			}
			else
			{
				if (!bBHZ2TowardsPick)	// -> BHZ2 towards BOND!	//v4.47T7
				{
					szLog.Format("BT MoveTable - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH2", 
						GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
				}
				else
				{
					szLog.Format("BT MoveTable - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH1", 
						GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
				}
			}
		}
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}

	//CSP
	if (m_bPrPreBondAlignment)	// && CMS896AStn::m_bUsePostBond)
	{
		BOOL bIsLFPadFound = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"];
		LONG lSortDir = (LONG) (*m_psmfSRam)["BinTable"]["SortDir"];	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN		//andrewng

		CString szLog;

		if (bIsLFPadFound)
		{
			LONG lOldX = m_lX;
			LONG lOldY = m_lY;

			//v4.59A22	//David Ma
			DOUBLE dThermalDX = m_oBinBlkMain.FindThermalDeltaPitchX(GetBTBlock(), m_lCol);
			DOUBLE dThermalDY = m_oBinBlkMain.FindThermalDeltaPitchX(GetBTBlock(), m_lRow);
			LONG lThermalDX = ConvertFileUnitToXEncoderValue(dThermalDX);
			LONG lThermalDY = ConvertFileUnitToYEncoderValue(dThermalDY);
			
			GetEncoderValue();

			//andrewng2
			m_lX = m_lCmd_X + m_lPreBondTableOffsetX + lThermalDX;
			m_lY = m_lCmd_Y + m_lPreBondTableOffsetY + lThermalDY;
			//LONG lX = m_lCmd_X + m_lPreBondTableOffsetX;
			//LONG lY = m_lCmd_Y + m_lPreBondTableOffsetY;

			szLog.Format("BT - OpMoveTable using LF-PAD: Orig-Final(%ld, %ld), New-Pad-Final(%ld, %ld), Cmd(%ld, %ld), Enc(%ld, %ld), PadOffset(%ld, %ld), ThermalD(Enc = %ld, %ld)(um = %f, %f)",
						lOldX, lOldY, m_lX, m_lY,
						m_lCmd_X, m_lCmd_Y, m_lEnc_X, m_lEnc_Y,
						m_lPreBondTableOffsetX, m_lPreBondTableOffsetY,
						lThermalDX, lThermalDY, dThermalDX, dThermalDY);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		}
	}

	
	BOOL bBinTableThetaCorrection	= (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"];
	DOUBLE dDegree					= (*m_psmfSRam)["BinTable"]["ThetaCorrection"];

	LONG lX = m_lX, lY = m_lY, lTheta = 0;
	dDegree = dDegree * -1;

	if (bBinTableThetaCorrection)
	{
		CString szLog;
		GetEncoderValue();
		szLog.Format("BT Original XYTD,%d,%d,%d,%f", lX, lY, m_lEnc_T, dDegree);
		CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);

		(*m_psmfSRam)["BinTable"]["ThetaCorrection"] = 0;
		if (dDegree != 0)
		{	
			CalculateRotateBinTheta(&lX, &lY, &lTheta, dDegree, m_lBinCalibX, m_lBinCalibY);
			m_lX = lX;
			m_lY = lY;
			//lTheta = lTheta * -1;
			szMsg.Format("BT - OpMoveTable() Theta Correction lX = %d, lY = %d, lTheta = %d, dDegree = %f", lX, lY, lTheta, dDegree);
			DisplaySequence(szMsg);
		}
	
		if (dDegree == 0 || lTheta == 0)
		{
			if (m_oBinBlkMain.GrabNVIsAligned((ULONG)GetWTGrade()))
			{
				ULONG ulBondBlk			= (*m_psmfSRam)["BinTable"]["BlkInUse"];
				DOUBLE dAngle			= m_oBinBlkMain.GrabNVRotateAngleX(ulBondBlk);//Radian
				DOUBLE dAngleInDegree	= fabs(dAngle * 180.0 / PI);
				LONG lXOffset			= m_oBinBlkMain.GrabNVXOffset(ulBondBlk);
				LONG lYOffset			= m_oBinBlkMain.GrabNVYOffset(ulBondBlk);
				m_lX += lXOffset;
				m_lY += lYOffset;
				lTheta = _round(dAngleInDegree / m_dThetaRes);
			}
			else
			{
				lTheta = 0;
			}
		}
	}

#ifdef NU_MOTION
	/*SelectXYProfile(m_lX - m_lCurXPosn, m_lY - m_lCurYPosn);
	if (m_bFirstCycleBT)
	{
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
	}*/
	nProf_X = m_nProfile_X;
	nProf_Y = m_nProfile_Y;
	if (IsMS90() && m_bNGPick)		//v4.51A19	//Silan MS90
	{
		m_lX = -505380;		//m_lNGPickPocketX
		m_lY = 123170;		//m_lNGPickPocketY
	}
#endif

	TakeTime(BT1);	// Take Time
	
	//andrewng //2020-0807
	CString szErrMap;
	if (IsErrMapInUse())
	{
		LONG lOldXBeforeErrMap = m_lX;
		LONG lOldYBeforeErrMap = m_lY;

		BOOL bErrMap = BM_GetNewXY(m_lX, m_lY);//	bonding move table
		
		szErrMap.Format("; before ErrMap Enc(%ld, %ld), after ErrMap Enc(%ld, %ld), ErrMapStatus = %d",
						lOldXBeforeErrMap, lOldYBeforeErrMap, m_lX, m_lY, bErrMap);
		//LONG lErrX = m_lNextX;
		//LONG lErrY = m_lNextY;
		//m_stErrMapBTMarkComp.CalculateAfflineOffsetXY(m_lNextX, m_lNextY);	//v4.69A12
		//szLog.Format("Final (With ErrMap - %ld, %ld, With AffOffset - %ld, %ld", lErrX, lErrY, m_lNextX, m_lNextY);
		//CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);	//v4.71A10
	}

	SetBTStartMoveWithoutDelay(TRUE);	//andrewng //2020-04-27

	X_MoveTo(m_lX, SFM_NOWAIT);
	Y_MoveTo(m_lY, SFM_NOWAIT);


	if (bBinTableThetaCorrection)
	{
		T_MoveTo(lTheta, SFM_NOWAIT);
		//INT nPos = (INT) _round(dDegree / m_dThetaRes);
		//T_MoveTo(lTheta, SFM_NOWAIT);

		CString szLog;
		szLog.Format("BT New XYT,%d,%d,%d", m_lX, m_lY, lTheta);
		CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);

		szMsg.Format("BT - Theta move to %d", lTheta);
		DisplaySequence(szMsg);
	}

	m_dBTStartTime = GetTime();
	m_lBTMoveDelayTime = 0;

	szMsg.Format("BT - map (%ld, %ld) #%ld move table to (%ld, %ld) after XY_MoveTo", 
						m_lWT_Row, m_lWT_Col, GetWTGrade(), m_lX, m_lY);
	szMsg = szMsg + szErrMap;
	DisplaySequence(szMsg);
	
	m_lAGCX = m_lX;
	m_lAGCY = m_lY;

	// pass the position and moved state to bh for bond level offset calculation. after bh get, reset it.
	//	rotate BT back to zero
	LONG lNewX = m_lX, lNewY = m_lY;
//	if( IsMS90BTRotated() )
//		RotateBinTable180(m_lX, m_lY, lNewX, lNewY);
	(*m_psmfSRam)["BinTable"]["Bond Posn X"]	= lNewX;
	(*m_psmfSRam)["BinTable"]["Bond Posn Y"]	= lNewY;
	(*m_psmfSRam)["BinTable"]["Bond Moved"]		= TRUE;

	//m_bFirstCycleBT = FALSE;	

	// Calculate the Bin Table motion time
	if (m_bDisableBT)	//v4.57A1
	{
		lTime_X = 20;
		lTime_Y = 20;
	}
	else
	{
		if (m_lX_ProfileType == MS896A_OBW_PROFILE)
		{
			lTime_X = CMS896AStn::MotionGetObwProfileTime(BT_AXIS_X, X_ProfName(nProf_X), lDiff_X, lDiff_X, HIPEC_SAMPLE_RATE, &m_stBTAxis_X);
		}
		else
		{
			lTime_X = CMS896AStn::MotionGetProfileTime(BT_AXIS_X, X_ProfName(nProf_X), lDiff_X, lDiff_X, HIPEC_SAMPLE_RATE, &m_stBTAxis_X);
		}

		if (m_lY_ProfileType == MS896A_OBW_PROFILE)
		{
			lTime_Y = CMS896AStn::MotionGetObwProfileTime(BT_AXIS_Y, Y_ProfName(nProf_Y), lDiff_Y, lDiff_Y, HIPEC_SAMPLE_RATE, &m_stBTAxis_Y);
		}
		else
		{
			lTime_Y = CMS896AStn::MotionGetProfileTime(BT_AXIS_Y, Y_ProfName(nProf_Y), lDiff_Y, lDiff_Y, HIPEC_SAMPLE_RATE, &m_stBTAxis_Y);
		}
		
		if (lTheta != 0)
		{
			LONG lDiff_T = _round(dDegree / m_dThetaRes);
			lTime_T = CMS896AStn::MotionGetProfileTime(BT_AXIS_T, T_ProfName(NORMAL_PROF), lDiff_T, lDiff_T, HIPEC_SAMPLE_RATE, &m_stBTAxis_T);
		}
		else
		{
			lTime_T = 0;
		}
	}

	if (lTime_X > lTime_Y)
	{
		if (lTime_X > lTime_T)
		{
			m_lTime_BT = lTime_X;
		}
		else
		{
			m_lTime_BT = lTime_T;
		}
	}
	else
	{
		if (lTime_Y > lTime_T)
		{
			m_lTime_BT = lTime_Y;
		}
		else
		{
			m_lTime_BT = lTime_T;
		}
	}

	// if disable BT XY, use default value
	if ((m_bSel_X == FALSE) && (m_bSel_Y == FALSE))
	{
		m_lTime_BT = BT_DEFAULT_TRAVEL_TIME;
	}

#ifdef NU_MOTION
	if (m_bFirstCycleBT)
	{
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
	}
#endif

	//OpUpdateMS100BinRealignAngle(m_lX, m_lY);					//v4.44T2	//Semitek		//v4.50A29	//HongYi
	(*m_psmfSRam)["BinTable"]["MotionTime"]	= m_lTime_BT;		//v2.97T1
	return nResult;
}

INT CBinTable::OpMoveTable2()
{
	INT		nResult	= gnOK;
	INT		nProf_X, nProf_Y;
	LONG	lTime_X, lTime_Y;
	LONG	lDiff_X, lDiff_Y;
	BOOL	bBinChanged = FALSE;
	

	//Check given position is within table limit or not
	if (CheckOutTable2Limit(m_lX + m_lBT2OffsetX, m_lY + m_lBT2OffsetY) != 0)
	{
		SetAlert_Red_Yellow(IDS_BT_POS_OUT_LIMIT);

		SetErrorMessage("Bintable 2 is outside limit");
		return gnNOTOK;
	}

	if ( IsTable1InBondRegion() )
	{
		//SetAlert_Red_Yellow(IDS_BT_POS_OUT_LIMIT);
		HmiMessage("ERROR (OpMoveTable2): BT1 in BOND region; BT2 cannot move");
		SetErrorMessage("ERROR (OpMoveTable2): BT1 in BOND region; BT2 cannot move");
		return gnNOTOK;
	}

	bBinChanged = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["BinChanged"];
	if (bBinChanged == TRUE)
	{
		(*m_psmfSRam)["BinTable"]["BinChanged"] = FALSE;
		GetEncoderValue();

		//LastX & LastY in BT1 values
		m_lLastX2 = m_lEnc_X2;
		m_lLastY2 = m_lEnc_Y2;

		//	here to save grade/bin frame/slot/barcode ...
		if( TRUE )
		{
			CMSLogFileUtility::Instance()->BL_LogStatus("BT -  table 2 save new barcode for BYD");
			CStringMapFile pSmfFile;
			CString szCurrBC, szPrevBC;
			LONG lBCSerialNo = 0;
			LONG lCurrGrade = GetWTGrade();
			ULONG ulBondBlk = (*m_psmfSRam)["BinTable"]["BlkInUse"];
			szCurrBC = m_szLoadReadBarcode;
			if( szCurrBC.IsEmpty() )
			{
				//v4.51A17
				szCurrBC = GetBLBarcodeData(ulBondBlk);
				CMSLogFileUtility::Instance()->BL_LogStatus("BT - table 2 bc by file " + szCurrBC);
			}
			else
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("BT -  table 2 bc by load " + szCurrBC);
			}
			if( pSmfFile.Open(MSD_WAFER_SORTED_BIN_BC_FILE, FALSE, TRUE)==1 )	// this would be read in wafer end DLL
			{
				lBCSerialNo = (pSmfFile)["GradeBCSerialNo"][lCurrGrade];
				if( lBCSerialNo==0 )
				{
					lBCSerialNo++;
				}
				else
				{
					szPrevBC = (pSmfFile)["GradeBarCode"][lCurrGrade][lBCSerialNo];
					if( szPrevBC!=szCurrBC )
						lBCSerialNo++;
				}
				(pSmfFile)["GradeBCSerialNo"][lCurrGrade]	= lBCSerialNo;
				(pSmfFile)["GradeBarCode"][lCurrGrade][lBCSerialNo]	= szCurrBC;

				pSmfFile.Update();
				pSmfFile.Close();
			}
			CMSLogFileUtility::Instance()->BL_LogStatus("BT -  bc by load table 2");
			if ( m_bEnableClearBinCopyTempFile )
			{
				(*m_psmfSRam)["BinTable"]["Copy Temp File Only"] = m_bClearBinInCopyTempFileMode;
			}
		}
	}

	//Calculation based on BT1 values also
	lDiff_X = labs(m_lX + m_lBT2OffsetX - m_lLastX2);
	lDiff_Y = labs(m_lY + m_lBT2OffsetY - m_lLastY2);

	// 400 count on X is 0.5 mm and plus some tolerance (5%) for standard X axis
	if (lDiff_X <= m_lMinTravel_X)
		nProf_X = FAST_PROF;
	else
		nProf_X = NORMAL_PROF;

	// 1000 count on Y is 0.5 mm and plus some tolerance (5%) for standard X axis
	if (lDiff_Y <= m_lMinTravel_Y)
		nProf_Y = FAST_PROF;
	else
		nProf_Y = NORMAL_PROF;
	
	X2_Profile(nProf_X);
	Y2_Profile(nProf_Y);

	LONG lOldX = m_lX;
	LONG lOldY = m_lY;

	OpAddPostBondOffset(m_lX, m_lY, 2);	//offset inserted is independent to BT1/BT2

	//v4.47T12	//v4.47A3
	BOOL bChangeGradeResetToBH1	 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"];	//v4.47T10
	BOOL bChangeGradeNoResetToBH1= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"];	//v4.47T17	//v4.47A3

	//v4.48A22
	if( IsAOIOnlyMachine()==FALSE )
	{
		CString szLog;
		BOOL bBHZ2TowardsPick		 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
		//BOOL bChangeGradeResetToBH1	 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"];		//v4.47T10
		//BOOL bChangeGradeNoResetToBH1= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"];	//v4.47T17	//v4.47A3
		
		if (bChangeGradeResetToBH1)
		{
			//BH always uses BH1 to pick 1st die, so BT should stay at BH1 pos
			szLog.Format("OpMoveTable2 - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH1 due to ChangeGrade", 
				GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
			(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"] = FALSE;
		}
		else
		{
			if (m_bFirstCycleBT)
			{
				szLog.Format("OpMoveTable2 (1st) - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH1", 
					GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
			}
			else
			{
				if (!bBHZ2TowardsPick)	// -> BHZ2 towards BOND!
				{
					szLog.Format("OpMoveTable2 - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH2", 
						GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
				}
				else
				{
					szLog.Format("OpMoveTable2 - (Bin #%ld, Index: %lu): Enc(%ld, %ld); OldEnc(%ld, %ld); BH1", 
						GetBTBlock(), m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()), m_lX, m_lY, lOldX, lOldY);
				}
			}
		}
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}

	if (bChangeGradeResetToBH1)
	{
		(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"] = FALSE;
	}
	if (bChangeGradeNoResetToBH1)
	{
		(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"] = FALSE;
	}


#ifdef NU_MOTION
	if (m_bFirstCycleBT)
	{
		X2_Profile(LOW_PROF);
		Y2_Profile(LOW_PROF);
	}
#endif

	TakeTime(BT1);	// Take Time
	X2_MoveTo(m_lX + m_lBT2OffsetX, SFM_NOWAIT);
	Y2_MoveTo(m_lY + m_lBT2OffsetY, SFM_NOWAIT);


#ifdef NU_MOTION
	if (m_bFirstCycleBT)
	{
		X2_Profile(NORMAL_PROF);
		Y2_Profile(NORMAL_PROF);
	}
#endif

	m_bFirstCycleBT = FALSE;	

	// Calculate the Bin Table motion time
	lTime_X = CMS896AStn::MotionGetProfileTime(BT_AXIS_X2, X_ProfName(nProf_X), lDiff_X, lDiff_X, HIPEC_SAMPLE_RATE, &m_stBTAxis_X2);
	lTime_Y = CMS896AStn::MotionGetProfileTime(BT_AXIS_Y2, Y_ProfName(nProf_Y), lDiff_Y, lDiff_Y, HIPEC_SAMPLE_RATE, &m_stBTAxis_Y2);

	if ( lTime_X > lTime_Y )
		m_lTime_BT = lTime_X;
	else
		m_lTime_BT = lTime_Y;

	// if disable BT XY, use default value
	if ((m_bSel_X == FALSE) && (m_bSel_Y == FALSE))
	{
		m_lTime_BT = BT_DEFAULT_TRAVEL_TIME;
	}
	
	//v4.59A31
	//OpUpdateMS100BinRealignAngle(m_lX + m_lBT2OffsetX, m_lY + m_lBT2OffsetY);	//v4.44T2	//Semitek
	(*m_psmfSRam)["BinTable"]["MotionTime"]	= m_lTime_BT;
	return nResult;
}


////////////////////////////////////////////
//	Next Die State 
////////////////////////////////////////////

INT CBinTable::OpNextDie()
{
	ULONG	ulFirstRowColSkippedUnit = 0;
	ULONG	ulEmptyHoleCount = 0;
	ULONG	ulWafflePadSktipCount = 0;
	INT		nResult	= TRUE;
	ULONG	ulBlk = 0;

	if( m_bDisableBT )
	{
		return TRUE;
	}


	//andrewng //2020-0619
	LONG lWafEncX = (*m_psmfSRam)["WaferTable"]["WTEncX"];
	LONG lWafEncY = (*m_psmfSRam)["WaferTable"]["WTEncY"];


	// Set and save bond die data
	CString szMsg;
	//andrewng //2020-0729
	szMsg.Format("BT - Next Die #%d - MAP Coord (%ld, %ld), WT-ENC (%ld, %ld), BT-ENC (%ld, %ld)",
					GetBTBlock(), m_lWT_Col, m_lWT_Row, lWafEncX, lWafEncY, m_lX, m_lY);
	DisplaySequence(szMsg);

	if ( IfBondOK(GetBTBlock(), m_lWT_Col, m_lWT_Row, lWafEncX, lWafEncY, m_lX, m_lY) == FALSE )
	{
		//Internal message - no need to translate in 2nd lang.
		CString szMsg;
		szMsg.Format("Fail to save bond data - Bonded Block %d exceed the map array", GetBTBlock());
		HmiMessage_Red_Back(szMsg);

		return -1;
	}

	if (m_bEnableOsramBinMixMap)
	{
		OpOsramIndex();
	}

	UpdateWaferStatistics(m_oBinBlkMain.GrabGrade(GetBTBlock()));
	
	//Display bonding binblk
	if ( m_ulMachineType == BT_MACHTYPE_STD )
	{
		UpdateBinDisplay(GetBTBlock(), 2);
	}

//	SaveBondResult(ucGrade, ulBondedBlk, m_lX, m_lY, ulCol, ulRow);

	(*m_psmfSRam)["BinTable"]["Die Bonded X"] = m_lWT_Col;
	(*m_psmfSRam)["BinTable"]["Die Bonded Y"] = m_lWT_Row;


	// Check whether the bin block is full
	INT nIndex = GetWTGrade();		//User Grade

	(*m_psmfSRam)["BinTable"]["BinFull"] = FALSE;
	 ulEmptyHoleCount = m_oBinBlkMain.TwoDimensionBarcodeGetTotalNoOfHoleOnFrame(nIndex);
	 ulFirstRowColSkippedUnit = m_oBinBlkMain.GetFirstRowColSkipPatTotalSkipUnitCount(GetBTBlock());
	 ulWafflePadSktipCount = m_oBinBlkMain.GetWafflePadSkipUnitTotalCount(GetBTBlock());
	
	//v4.52A6	//CYOptics, Inari
	BOOL bCheckEmptyRowFcn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW)) ||
		(pApp->GetCustomerName() == "CyOptics")		|| 
		(pApp->GetCustomerName() == "FiberOptics")	||		// = Dicon
		(pApp->GetCustomerName() == "Inari")	)	
	{
		bCheckEmptyRowFcn = TRUE;
	}

	if (bCheckEmptyRowFcn)
	{
		//If using EMPTY ROW fcn, use bin INDEX as die count because it also includes EMPTY
		// spaces & rows;
		if (m_oBinBlkMain.GrabInputCount(nIndex) <= m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock()))
		{
			(*m_psmfSRam)["BinTable"]["BinFull"] = TRUE;
			nResult = FALSE;
			DisplaySequence("BT: BINFULL at OpNextDie");
		}
	}
	else
	{
		if (m_oBinBlkMain.GrabInputCount(nIndex) <= m_oBinBlkMain.GrabNVNoOfSortedDie(nIndex) + 
												ulEmptyHoleCount + ulFirstRowColSkippedUnit + ulWafflePadSktipCount)
		{
			(*m_psmfSRam)["BinTable"]["BinFull"] = TRUE;
			nResult = FALSE;

			CString szTemp;
			szTemp.Format("BT NextDie: %d + %d = %d  FULL(%d)  (Grade = %d)", 
							m_oBinBlkMain.GrabNVNoOfSortedDie(nIndex), 
							ulEmptyHoleCount,
							m_oBinBlkMain.GrabInputCount(nIndex),
							m_oBinBlkMain.GrabNVIsFull(GetBTBlock()),
							nIndex);
			//CMSLogFileUtility::Instance()->BT_ExchangeFrameLog(szTemp);		//v3.35
		}
	}


	//v4.18T1	//Cree HuiZhou
	if (m_oBinBlkMain.GrabInputCount(nIndex) == m_oBinBlkMain.GrabNVNoOfSortedDie(nIndex) + 
											ulEmptyHoleCount + ulFirstRowColSkippedUnit + ulWafflePadSktipCount + 1)
	{
		//v4.22T5
		BOOL bPreLoadEmptyFrame		= (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["PreLoadEmpty"];	
		BOOL bDisablePreLoadEmpty	= ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD);

		(*m_psmfSRam)["BinTable"]["NextBinFull"] = TRUE;

		CString szTemp;
		szTemp.Format("BT NextDie Next BinFull: %d + %d + 1 = %d  FULL(%d)  (Grade = %d)", 
						m_oBinBlkMain.GrabNVNoOfSortedDie(nIndex), 
						ulEmptyHoleCount,
						m_oBinBlkMain.GrabInputCount(nIndex),
						m_oBinBlkMain.GrabNVIsFull(GetBTBlock()),
						nIndex);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szTemp);
	}
	if (m_oBinBlkMain.GrabInputCount(nIndex) == m_oBinBlkMain.GrabNVNoOfSortedDie(nIndex) + 
											ulEmptyHoleCount + ulFirstRowColSkippedUnit + ulWafflePadSktipCount + 2)
	{
		if (CMS896AApp::m_bEnableSubBin == TRUE)
		{
			unsigned char aaGrades[256];
			CString szTemp;
			aaGrades[0] = GetWTGrade() + 1 + m_WaferMapWrapper.GetGradeOffset();
			szTemp.Format("BT - Next Die Bin Full,Grade,%d,%d",aaGrades[0],GetWTGrade());
			DisplaySequence(szTemp);
			m_WaferMapWrapper.SelectGrade(aaGrades, 1);
		}
	}
	return nResult;

	//OsramTrip 8/22 not available for MS899 of single-arm sorting
#ifdef NU_MOTION
	//	V450X16
	if(IsBurnIn()==FALSE && SPECIAL_DEBUG_LOG_WS && m_lAtBondDieGrade != -1 && m_lAtBondDieRow != -1 && m_lAtBondDieCol != -1 && !m_bStop)
	{
		if( m_lAtBondDieGrade!=GetWTGrade() || m_lAtBondDieRow!=m_lWT_Row || m_lAtBondDieCol!=m_lWT_Col )
		{
			CString szMsg;
			szMsg.Format("BT - map (%ld,%ld)%ld; bond die (%d,%d)%d mismatch, please stop and check!",
				m_lWT_Row, m_lWT_Col, GetWTGrade(), m_lAtBondDieRow, m_lAtBondDieCol, m_lAtBondDieGrade);
			DisplaySequence(szMsg);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Back(szMsg, "DBH sorting");
			nResult = -1;
		}
	}	//	V450X16	check and make sure the bonded is same to get
#endif

	return nResult;
}

////////////////////////////////////////////
//	Move back the table 
////////////////////////////////////////////

INT CBinTable::OpMoveBack()
{
	INT		nResult					= gnOK;

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	if ((m_lLastX == 0) && (m_lLastY == 0))	
	{
		//No move if first die & no previous posn LastXY
	}
	else if ((m_lLastX == m_lBTUnloadPos_X) && (m_lLastY == m_lBTUnloadPos_Y))		//v2.68
	{
		//No move if first die & no previous posn LastXY
	}
	else
	{
		if (m_bUseDualTablesOption && IsTable2InBondRegion())
		{
			CString szErr = "ERROR (OpMoveBack): BT2 in BOND region; BT1 cannot move!";
			HmiMessage(szErr);
			return gnNOTOK;
		}

		// Remark: If the cycle is just started, Last X and Y must not be the previous die location
		MoveXYTo(m_lLastX, m_lLastY);
	}
	
	(*m_psmfSRam)["BinTable"]["SPC"]["MoveBack"] = TRUE;
	return nResult;
}

INT CBinTable::OpMoveBack2()
{
	INT	nResult	= gnOK;

	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);

	if ((m_lLastX2 == 0) && (m_lLastY2 == 0))	
	{
		//No move if first die & no previous posn LastXY
	}
	else if ( (m_lLastX2 == (m_lBTUnloadPos_X + m_lBT2OffsetX)) && 
			  (m_lLastY2 == (m_lBTUnloadPos_Y + m_lBT2OffsetY)) )	
	{
		//No move if first die & no previous posn LastXY
	}
	else
	{
		if (IsTable1InBondRegion())
		{
			CString szErr = "ERROR (OpMoveBack2): BT1 in BOND region; BT2 cannot move!";
			HmiMessage(szErr);
			return gnNOTOK;
		}

		// Remark: If the cycle is just started, Last X and Y must not be the previous die location
		X2_MoveTo(m_lLastX2, SFM_NOWAIT);
		Y2_MoveTo(m_lLastY2, SFM_WAIT);
		X2_Sync();
	}
	
	(*m_psmfSRam)["BinTable"]["SPC"]["MoveBack"] = TRUE;
	return nResult;
}


VOID CBinTable::OpUpdateDieIndex()		// Update Die Index
{
	ULONG ulIndex = (*m_psmfSRam)["DieInfo"]["BT_Index"];
	ulIndex++;
	(*m_psmfSRam)["DieInfo"]["BT_Index"] = ulIndex;
}


BOOL CBinTable::OpGenAllTempFile(BOOL bBackupTempFile)
{
	CString szLogMsg = "";

	if (IsBurnIn())
	{
		return FALSE;
	}

	if( IsAOIOnlyMachine() )
	{
		return FALSE;
	}

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( (pUtl->GetPrescanDummyMap() || IsEnablePNP()) && m_bEnablePickNPlaceOutputFile==FALSE )
	{
		return FALSE;
	}

	CTime theTime = CTime::GetCurrentTime(); 

	DOUBLE dDuration = GetTime();		//v4.47T8
	LONG lGenStatus = m_oBinBlkMain.GenAllTempFiles(FALSE, bBackupTempFile);
	if( lGenStatus==2 )
	{
		return FALSE;
	}
	else if (lGenStatus == BT_BACKUP_NVRAM_ERROR)
	{
		//SetAlert_Red_Yellow(IDS_BT_GEN_TMP_FILE_ERROR);
		//return FALSE;
	}

	szLogMsg.Format("%d-%2d-%2d %2d:%2d:%2d - Start GenAllTempFile\n", theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);
	dDuration = GetTime() - dDuration;

	szLogMsg.Format("%2d %2d:%2d:%2d - Complete GenAllTempFile (%.1f ms)\n", 
		theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), dDuration);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);

	dDuration = GetTime();				//v4.47T8
	m_oBinBlkMain.ResetNVRunTimeData();
	dDuration = GetTime() - dDuration;

	szLogMsg.Format("%2d %2d:%2d:%2d - Complete Reset NVRunTimeData (%.1f ms)\n", 
		theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), dDuration);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);

	// Backup to NVRam immediately to prevent mis-match cases occur
	dDuration = GetTime();				//v4.47T8
	BackupToNVRAM();
	dDuration = GetTime() - dDuration;

	szLogMsg.Format("%2d %2d:%2d:%2d - Complete Backup NVRunTimeData (%.1f ms)\n", 
		theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), dDuration);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);

	return TRUE;
}

VOID CBinTable::OpGenWaferStopInfo()
{
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	//Save Stop map time
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	if ( pAppMod != NULL )
	{
		pAppMod->MapIsStopped();
	}

	(*m_psmfSRam)["WaferTable"]["WaferEndSmooth"] = TRUE;

	if ( pUtl->GetPrescanDummyMap() || IsEnablePNP() )
	{
		return;
	}

	if( IsWaferEnded()==FALSE )
	{
		return;
	}

	if( IsBurnIn() )
	{
		return ;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "CyOptics"		|| 
		pApp->GetCustomerName() == "Inari"			||		//v4.51A24
		pApp->GetCustomerName() == "FiberOptics")
	{
		LONG ulBinIndex = m_oBinBlkMain.GrabNVCurrBondIndex(GetWTGrade());
		LONG ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1);
		LONG ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(1);

		LONG ulQuotient	= ulBinIndex / ulDiePerRow;
		if (ulQuotient > ulDiePerRow -2)
		{
			(*m_psmfSRam)["WaferTable"]["ResortModeWaferEnd"] = TRUE;
		}
	}

	//Get Wafer End Date and Time
	GenerateWaferEndTime();

	//Generate wafer end file
	BOOL bGenFileOk = FALSE;
	if (m_bIfGenWaferEndFile)
	{
		bGenFileOk = WaferEndFileGenerating("Gen Wafer Stop Info");
		BOOL bReturn = IM_WaferEndChecking();
		BOOL bIsWaferEndYieldOk = WaferEndYieldCheck();

		if (bReturn && bIsWaferEndYieldOk)
		{
			(*m_psmfSRam)["WaferTable"]["WaferEndSmooth"] = TRUE;
		}
		else
		{
			(*m_psmfSRam)["WaferTable"]["WaferEndSmooth"] = FALSE;
		}
	}

	//Send wafer end info to Host
	ScanSummaryGenerating();
	if( m_bRenameMapNameAfterEnd )
	{
		CString szOrgMapFilename, szNewMapFilename;
		szOrgMapFilename = m_szOnlyMapFileFullPath;
		szNewMapFilename = szOrgMapFilename + ".skt";
		RenameFile(szOrgMapFilename, szNewMapFilename);
		SetErrorMessage(szOrgMapFilename + " rename to " + szNewMapFilename);
	}

	SendWaferEndInfoToHost();

	if ((CMS896AStn::m_bForceClearMapAfterWaferEnd == TRUE) && (bGenFileOk))
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gen wafer stop info, Force clear map\n");
		SetErrorMessage("Gen wafer stop info, , reset grade mapping");
		LogWaferEndInformation();
		m_WaferMapWrapper.InitMap();
		m_WaferMapWrapper.ResetGradeMap();
	}
}

BOOL CBinTable::WaitMoveComplete(INT nTimeout)
{
	if (m_nBTInUse == 1)	//If BT2
	{
		X2_Sync();
		Y2_Sync();
	}
	else					//ELse BT1
	{
		X_Sync();
		Y_Sync();
	}
	return TRUE;
}

INT CBinTable::OpMoveToUnload()
{
	INT nResult = gnOK;
	LONG lHmiMsgReply = 0;
	BOOL bIsBinCntClred = FALSE;
	CString szText;
	CString szTitle, szContent;
	BOOL bNeedToClear = FALSE;		//v4.14T1

	UpdateBinDisplay(1, 1);

	if (IsBLEnable())				//v4.21	//In case of problem, for MS100 9Inch dual table config
	{
		return nResult;
	}

	if ( (LONG)(*m_psmfSRam)["WaferTable"]["Hit SEP Grade"] == 1 )
	{
		ULONG i;	
		ULONG ulClearCount = 0;
		LONG lReply;

		szText.LoadString(HMB_BT_CLEAR_ALL_BIN_BLK);

#ifdef NU_MOTION
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
#endif
		// Move to unload
		XY_MoveTo(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, SFM_WAIT);

#ifdef NU_MOTION
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
#endif

		lReply = SetAlert_Msg_Red_Yellow(IDS_BT_HIT_SEPERATION_GRADE, szText, "Yes", "No");
		SetErrorMessage("Hit Seperation Grade");

		if ( lReply == 1 )
		{
			for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
			{
				if ( ClrBinCntCmdSubTasks(i, "All Blocks", m_szBinOutputFileFormat, FALSE, FALSE, bNeedToClear) == TRUE )
				{
					ulClearCount++;
				}
			}

			if (ulClearCount == 0)
			{
				szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);

				HmiMessage(szContent);
			}
			else if (ulClearCount == m_oPhyBlkMain.GetNoOfBlk())
			{
				szContent.LoadString(HMB_BT_ALL_BINS_COUNTER_CLEARED);

				HmiMessage(szContent);
				SetStatusMessage("All Bin Counters are cleared");
			}
			else
			{
				szContent.LoadString(HMB_BT_SOME_BIN_COUNTER_NOT_CLR);

				HmiMessage(szContent);
			}

			//Clear ALL SPC data
			if ( (CMS896AStn::m_bUsePostBond == TRUE) && (ulClearCount == m_oPhyBlkMain.GetNoOfBlk()) )
			{
				IPC_CServiceMessage stMsg;
				int nConvID;

				nConvID = m_comClient.SendRequest(BOND_PR_STN, "DeleteRuntimeAllData", stMsg);
				m_comClient.ScanReplyForConvID(nConvID, 5000);
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
			}
		}

		return nResult;
	}


	if (m_bIsUnload == TRUE)
	{
#ifdef NU_MOTION
		X_Profile(LOW_PROF);
		Y_Profile(LOW_PROF);
#endif

		// Move to unload
		//Do not move in Mega Da
		//XY_MoveTo(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, SFM_WAIT);
		m_bIsUnload = FALSE;

#ifdef NU_MOTION
		X_Profile(NORMAL_PROF);
		Y_Profile(NORMAL_PROF);
#endif
		//Display BinBlk full image on HMI
		if ( m_bShowBinBlkImage == TRUE )
		{
			IPC_CServiceMessage stReq;
			CHAR acPar[200];

			//Update Binblk display
			//BinBlksDrawing();
			UpdateBinDisplay(m_ulBinBlkFull);

			strcpy_s(acPar, sizeof(acPar), "BinBlkDisplay");
			stReq.InitMessage(((INT)strlen(acPar) + 1), acPar);
			m_comClient.SendRequest("HmiUserService", "HmiSetScreen", stReq);
		}

		//Ask user to clear bin or not
		// Case of clear by logical bin blk without auto assign grade
		if (m_bAutoClearBin == TRUE && CMS896AStn::m_bUseClearBinByGrade == FALSE && 
			m_oBinBlkMain.GrabAutoAssignGrade() == FALSE )
		{
			szText.Format("%d\n Count: %d", m_ulBinBlkFull, m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkFull) );
			if ( SetAlert_Msg(IDS_BT_AUTO_CLR_BIN_COUNTER, szText, "Yes", "No") == 1 )
			{
				//v3.94	//PLLM Lumiramic flat file support
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if ( (pApp->GetCustomerName() == "Lumileds") )	
					//&& (CMS896AStn::m_lOTraceabilityFormat > 0) )		//v4.44A4
				{
					//ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
					//if (nPLLM == PLLM_LUMIRAMIC)
					AddPLLMFlatFileEntry(m_ulBinBlkFull);
				}

				bIsBinCntClred = ClrBinCntCmdSubTasks(m_ulBinBlkFull, "Single Block", m_szBinOutputFileFormat, FALSE, FALSE, bNeedToClear);

				szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);

				if ( bIsBinCntClred == TRUE )
				{
					//Show message box for 2sec only
					szContent.LoadString(HMB_BT_PHY_BLK_COUNT_CLEARED);
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 2000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					
					ClearBPRAccEmptyCounter(&m_ulBinBlkFull);
				}
				else
				{	
					szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				}


				//Clear Single SPC data if necessary
				if (m_bUsePostBond && bIsBinCntClred)
				{
					ClearSPCData(m_ulBinBlkFull,FALSE,FALSE);
				}

				if ( bIsBinCntClred == TRUE )
				{
					szText.Format("%d", m_ulBinBlkFull); 
					SetStatusMessage("Bin " + szText + " Counter is cleared");
				}

				//v2.99T1
				CString szTemp;
				szTemp.Format("Auto Clear Bin #%d: Count = %d", m_ulBinBlkFull, m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkFull));
				SetErrorMessage(szTemp);
			}
		}

		//Ask user to clear bin or not
		// Case of clear by logical bin blk with Auto Assign Grade
		else if (m_bAutoClearBin == TRUE && CMS896AStn::m_bUseClearBinByGrade == FALSE &&
			m_oBinBlkMain.GrabAutoAssignGrade() == TRUE )
		{
		
			//szText.Format("Are you sure to\nclear BinBlk %d?", m_ulBinBlkFull);
			//if (HmiMessage(szText, "Auto Clear Bin Counter Message", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
			UCHAR uCurGrade = m_oBinBlkMain.GrabGrade(m_ulBinBlkFull);
			CUIntArray szaFullBlkNo;
			
			for (ULONG k=1; k<=m_oBinBlkMain.GetNoOfBlk(); k++)
			{
				if ((m_oBinBlkMain.GrabGrade(k) != uCurGrade) || (m_oBinBlkMain.GrabNVIsFull(k) == FALSE))
					continue;

				szaFullBlkNo.Add(k);
			}

			for (ULONG i=0; i<(ULONG)szaFullBlkNo.GetSize(); i++)
			{
				ULONG ulBinFullId = i;

				szText.Format("%d\n Count: %d", ulBinFullId, m_oBinBlkMain.GrabNVNoOfBondedDie(ulBinFullId));
				if ( SetAlert_Msg(IDS_BT_AUTO_CLR_BIN_COUNTER, szText, "Yes", "No") == 1 )
				{
					//v3.94	//PLLM Lumiramic flat file support
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					if ( (pApp->GetCustomerName() == "Lumileds") )	
						//&& (CMS896AStn::m_lOTraceabilityFormat > 0) )		//v4.44A4
					{
						//ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
						//if (nPLLM == PLLM_LUMIRAMIC)
						AddPLLMFlatFileEntry(ulBinFullId);
					}

					bIsBinCntClred = ClrBinCntCmdSubTasks(ulBinFullId, "Single Block", m_szBinOutputFileFormat, FALSE, FALSE, bNeedToClear);

					szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);

					if ( bIsBinCntClred == TRUE )
					{
						//Show message box for 2sec only
						szContent.LoadString(HMB_BT_PHY_BLK_COUNT_CLEARED);
						HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 2000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

						// Update the serial no for a grade
						CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();	//v4.52A8
						for (ULONG j=0; j<(ULONG)szaFullBlkNo.GetSize(); j++)
						{
							m_oBinBlkMain.UpdateSerialNoGivenBinBlkId(szaFullBlkNo.GetAt(j), pBTfile);
						}
						CMSFileUtility::Instance()->SaveBTConfig();		//v4.52A8
						
						ClearBPRAccEmptyCounter(&ulBinFullId);
					}
					else
					{
						szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);
						HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
					}


					//Clear Single SPC data if necessary
					if (m_bUsePostBond && bIsBinCntClred)
					{
						ClearSPCData(ulBinFullId,FALSE,FALSE);
					}

					if ( bIsBinCntClred == TRUE )
					{
						szText.Format("%d", ulBinFullId); 
						SetStatusMessage("Bin " + szText + " Counter is cleared");
					}

					//v2.99T1
					CString szTemp;
					szTemp.Format("Auto Clear Bin #%d: Count = %d", ulBinFullId, m_oBinBlkMain.GrabNVNoOfBondedDie(ulBinFullId));
					SetErrorMessage(szTemp);
				}
			}
		}
		// Case of Clear By Grade

		else if (m_bAutoClearBin == TRUE && CMS896AStn::m_bUseClearBinByGrade == TRUE)
		{
			CString szGrade;
			szGrade.Format("%d", GetWTGrade());
			
			if (SetAlert_Msg(IDS_BT_CLR_BIN_BY_GRADE, szGrade, "Yes", "No") == 1)
			{
				CDWordArray dwaBinBlkIds;
				CString szBinBlkId;
				
				// Temp to Disable Clear Bin Counter by grade with generating out the output file

				bIsBinCntClred = ClrBinCntCmdSubTasksByGrade(GetWTGrade(), 
					"Single Grade", m_szBinOutputFileFormat, dwaBinBlkIds, TRUE, FALSE);

				if (bIsBinCntClred == FALSE)
				{
					szContent.LoadString(HMB_BT_CLEAR_BIN_FAILED);
					HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
				}
				else
				{
					ULONG ulBinFullId = GetWTGrade();
					ClearBPRAccEmptyCounter(&ulBinFullId);
				}

				if (m_bUsePostBond && bIsBinCntClred)
				{
				
					ULONG ulBlkId;

					for (INT i=0; i<dwaBinBlkIds.GetSize(); i++)
						ulBlkId = dwaBinBlkIds.GetAt(i);

					ClearSPCData(ulBlkId,FALSE,FALSE);
				}

				if ( bIsBinCntClred == TRUE )
				{
					szText.Format("%d", GetWTGrade()); 
					SetStatusMessage("Grade" + szText + " Counter is cleared");
				}

				//v2.99T1
				CString szTemp;
				szTemp.Format("Auto Clear Bin by Grade #%d", GetWTGrade());
				SetErrorMessage(szTemp);
			}
		}
	}

	return nResult;
}


INT CBinTable::OpBackupMap()
{
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( m_bDisableBT )
		return gnOK;

	if( IsBurnIn() )
		return gnOK;

	if( IsPrescanning() )
		return gnOK;
	if( pUtl->GetPrescanDummyMap() || IsEnablePNP() )
		return gnOK;

	if( IsAOIOnlyMachine() )
	{
		return gnOK;
	}
	// update wafer map header
	UpdateWaferMapHeader();

	CString szBackupMapPath			= (*m_psmfSRam)["MS896A"]["BackupMapFilePath"];
	CString szBackupMapExtension	= (*m_psmfSRam)["MS896A"]["BackupMapFileExt"];
	//Immediately quit fcn if backup map path not set up properly
	if (szBackupMapPath.IsEmpty() || (szBackupMapPath == ""))
	{
		SetErrorMessage("BT: Backup Map fails with NULL backup path");				//v3.89
		return gnNOTOK;
	}
	//2. Check whether the path is existing 
	if ( CreateDirectory(szBackupMapPath, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			SetErrorMessage("BT: Backup Map fails with invalid backup path: " + szBackupMapPath);	//v3.89
			SetAlert_Msg(IDS_BT_INVALID_DIRECTORY, szBackupMapPath);
			return gnNOTOK;
		}
	}

	CMSLogFileUtility::Instance()->BT_BackUpMapLog("Start Backup");

	//1. Get Map file name details
	CString szFile = GetMapFileName();	// Get the original map filename and save a backup
	if ( szFile.IsEmpty() == TRUE )
	{
		SetErrorMessage("BT: Backup Map fails with NULL map file name");	//v3.89
		return gnNOTOK;
	}

	INT nResult = gnOK;
	INT nCol;
	CString szTemp, szMsg, szRptFile;
	CString szSourcePath, szSecFile, szSrcFile;

	nCol = szFile.ReverseFind(';');
	if ( nCol != -1 )
	{
		//Get 2nd file name & extension
		szSecFile = szFile.Mid(nCol+1);
		szFile = szFile.Left(nCol);
	}
	szSrcFile = szFile;

	nCol = szFile.ReverseFind('.');
	if ( nCol == -1 )
		szTemp = szFile;			// No file extension
	else
		szTemp = szFile.Left(nCol);
	nCol = szTemp.ReverseFind('\\');			// Remove the path
	if ( nCol != -1 )
	{
		//Get this map file original path
		szSourcePath = szTemp.Left(nCol);
		szTemp = szTemp.Mid(nCol+1);
	}

	CMSLogFileUtility::Instance()->BT_BackUpMapLog("Map FileName " + szFile);
	CMSLogFileUtility::Instance()->BT_BackUpMapLog("Write File");

	//4. Copy local backup map into CTM map location
	TRY 
	{
		CTime theTime = CTime::GetCurrentTime();

		if( GetPsmEnable() )
		{
			szRptFile = szBackupMapPath + _T("\\") + GetMapNameOnly() + _T(".PSM");
			remove(szRptFile);
			CMSLogFileUtility::Instance()->BT_BackUpMapLog("Write PSM file " + szRptFile);
			// should local to server.
			szRptFile = szBackupMapPath + _T("\\") + GetMapNameOnly() + _T(".") + szBackupMapExtension;
			CopyFileWithQueue(szSrcFile, szRptFile, FALSE);
		}
		else if( m_WaferMapWrapper.IsMapValid() )
		{
			//Nichia//v4.43T7
			//2. Check whether the path is existing 
			if ( CreateDirectory(szBackupMapPath, NULL) == 0 )
			{
				if ( GetLastError() != ERROR_ALREADY_EXISTS )
				{
					SetErrorMessage("BT: Backup Map fails with invalid backup path: " + szBackupMapPath);	//v3.89
					SetAlert_Msg(IDS_BT_INVALID_DIRECTORY, szBackupMapPath);
					return gnNOTOK;
				}
			}

			szRptFile = szBackupMapPath + _T("\\") + GetMapNameOnly() + _T(".") + szBackupMapExtension;

			//v4.40T6
			if ((pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == ""))	//v4.59A34
			{
				CString szLotNo = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];	
				szRptFile = szBackupMapPath + _T("\\") + szLotNo + _T(".") + szBackupMapExtension;
				CMSLogFileUtility::Instance()->BT_BackUpMapLog("Copy File (Nichia) to " + szRptFile);
			
				//Nichia//v4.43T7
				BOOL bGenerateRPTFile = (BOOL)(LONG)(*m_psmfSRam)["BinOutputFile"]["Enable Nichia RPTFile"];
				if (!bGenerateRPTFile)
				{
					CMSLogFileUtility::Instance()->BT_BackUpMapLog("RPT File disabled (Nichia)\n");	//v4.45T1
					
					CString szLog;
					szLog = "Nichia RPT file not created: " + szRptFile;
					CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
					return gnOK;
				}			
			}
			else
			{
				CMSLogFileUtility::Instance()->BT_BackUpMapLog("Copy File to " + szRptFile);
			}

			CString szASMBackupPath	= gszUSER_DIRECTORY + "\\MapFile\\BackupMap.txt";
			//Nichia//v4.43T7
			if ((pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == ""))	//v4.59A34
			{
				CString szLotNo = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];	
				szASMBackupPath	= gszUSER_DIRECTORY + _T("\\MapFile\\") + szLotNo + _T(".") + szBackupMapExtension;
			}

			if ( m_WaferMapWrapper.WriteMap(szASMBackupPath) == FALSE )
			{
				SetErrorMessage("BT: Backup Map fails with Write Map to: " + szASMBackupPath);	//v3.89
				szMsg.Format("1.%s", szASMBackupPath); 
				SetAlert_Msg(IDS_BT_CREATE_MAP_FAILED, szMsg);
				return gnNOTOK;
			}
			else
			{
				//v4.45T1
				CMSLogFileUtility::Instance()->BT_BackUpMapLog("Created ASM Backup RPT File at: " + szASMBackupPath);
			}
			
			if( pApp->GetCustomerName()=="OSRAM" )
			{
				BOOL bCopyMap = TRUE;
				while( 1 )
				{
					bCopyMap = TRUE;
					if ( CopyFile(szASMBackupPath, szRptFile, FALSE) == FALSE )
					{
						SetErrorMessage("BT: Backup Map fails with COPY to: " + szRptFile);		//v3.89
						szMsg.Format("Fail to copy backup file into\n%s\nPlease check network and retry!", szRptFile); 
						CMSLogFileUtility::Instance()->BT_BackUpMapLog(szMsg);
						if( HmiMessage_Red_Back(szMsg, "Backup Map", glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300)==glHMI_CANCEL )
						{
							CMSLogFileUtility::Instance()->BT_BackUpMapLog("User give up the map back up");
							return gnNOTOK;
						}
						bCopyMap = FALSE;
					}
					if( bCopyMap )
						break;
				}
			}
			else
			{
				BOOL bDeleteMap = FALSE;
				if ((pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == ""))	//v4.59A34
				{
					bDeleteMap = TRUE;
					szMsg.Format("ASM Backup RPT file is deleted (Nichia): %s", szASMBackupPath); 
					CMSLogFileUtility::Instance()->BT_BackUpMapLog(szMsg);
				}

				BOOL bToUploadMap = TRUE;
				if( pApp->GetCustomerName()==CTM_NICHIA && pApp->GetProductLine()=="MS90" )
				{
					bToUploadMap = IsWaferEnded() && m_bWaferEndUploadMapMpd;
				}
				if( bToUploadMap )
				{
					if ( CopyFileWithQueue(szASMBackupPath, szRptFile, bDeleteMap) == FALSE )
					{
						szMsg.Format("RPT file copied FAIL to: %s\n", szRptFile); 
						CMSLogFileUtility::Instance()->BT_BackUpMapLog(szMsg);		//v4.45T1

						SetErrorMessage("BT: Backup Map fails with COPY to: " + szRptFile);		//v3.89
						szMsg.Format("2.%s", szRptFile); 
						SetAlert_Msg(IDS_BT_CREATE_MAP_FAILED, szMsg);
						return gnNOTOK;
					}
					else
					{
						szMsg.Format("RPT file copied OK to: %s", szRptFile); 
						CMSLogFileUtility::Instance()->BT_BackUpMapLog(szMsg);		//v4.45T1
					}
				}

				szMsg.Format("WaferLabel-- Open another Bk map path:%d", pApp->m_bWaferLabelFile);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);	

				//4.52D10Name
				if (pApp->GetCustomerName() == "Lumileds" && pApp->GetProductLine() == "Rebel" && pApp->m_bWaferLabelFile)
				{
					pApp->BackUpMapFileToOutputPath(szASMBackupPath);
				}
			}
		}
		if (szSecFile.IsEmpty() != TRUE)
		{
			szSourcePath = szSourcePath + _T("\\") + szSecFile;
			szTemp = szBackupMapPath + _T("\\") + szSecFile;
			CopyFileWithQueue(szSourcePath, szTemp, FALSE);
		}
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("BT: Backup Map fails with COPY Exception");				//v3.89
		szMsg.Format("2.%s", szRptFile); 
		SetAlert_Msg(IDS_BT_CREATE_MAP_FAILED, szMsg);
		return gnNOTOK;
	}
	END_CATCH

	CMSLogFileUtility::Instance()->BT_BackUpMapLog("Map is saved\n");
	return nResult;
}


ULONG CBinTable::OpGetNextGradeBlk()
{
	ULONG ulBinBlk = BT_DLA_NO_ACTION;
	ULONG ulBlkLeft;
	
	UCHAR ucMapGrade;
	UCHAR ucBlkGrade;
	UCHAR ucNextGrade = 0;
	ULONG ulGradeTotal = 0;
	ULONG ulGradeLeft = 0;
	ULONG ulGradePick = 0;
	ULONG ulDieLeft = 0;
	ULONG ulFirstRowColPatternSkipUnitCount = 0;
	ULONG ulWafflePadSkipUnitCount = 0;
	BOOL bNextGrade = FALSE;
	CString szLogText;
	int i;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();


	if( pUtl->GetPrescanAreaPickMode() )
	{
		return BT_DLA_NO_ACTION;
	}

	//v4.50A16
	BOOL bMultiGradeToSingleBin = (BOOL)(LONG)(*m_psmfSRam)["Wafer Table Options"]["MultiGradeSortToSingleBin"];
	if (bMultiGradeToSingleBin)
	{
		return BT_DLA_NO_ACTION;	//MS109	//Osram Germany	
	}

	//Return -- if change limit == 0
	if ( m_ulGradeChangeLimit == 0 )
	{
		return BT_DLA_NO_ACTION;
	}

	if ( GetBTBlock() == 0 )
	{
		return ulBinBlk;
	}
	if (GetBTBlock() >= BT_MAX_BINBLK_SIZE)	//v3.35		//v4.48 Klocwork
	{
		return ulBinBlk;
	}

	//Get this blk left count can be bonded
	ulBlkLeft = m_oBinBlkMain.GrabNoOfDiePerBlk(GetBTBlock()) - m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
	if ( m_oBinBlkMain.GrabMaxUnit(GetBTBlock()) != 0 )
	{
		ulBlkLeft = m_oBinBlkMain.GrabMaxUnit(GetBTBlock())   - m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
	}

	//v3.36T1
	//frame capacity should be determined by  Input Count instead of max count, if Input count is smaller
	ULONG ulInputCount = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(GetBTBlock()));
	if (ulInputCount <= m_oBinBlkMain.GrabNoOfDiePerBlk(GetBTBlock()))
	{
		ulBlkLeft = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(GetBTBlock()))
						- m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
			
		ulFirstRowColPatternSkipUnitCount = m_oBinBlkMain.GetFirstRowColSkipPatTotalSkipUnitCount(GetBTBlock());
		ulWafflePadSkipUnitCount = m_oBinBlkMain.GetWafflePadSkipUnitTotalCount(GetBTBlock());
		
		if (m_bEnable2DBarcodeOutput)	//Should also take into account of Hole number if enabled
		{
			ulBlkLeft = ulBlkLeft - m_ulHoleDieNum;
		}

		ulBlkLeft = ulBlkLeft - ulFirstRowColPatternSkipUnitCount - ulWafflePadSkipUnitCount;
	}

	//Get this grade count can be picked
	ucMapGrade = m_oBinBlkMain.GrabGrade(GetBTBlock()) + m_WaferMapWrapper.GetGradeOffset();

	//Get this blk grade in use
	ucBlkGrade = m_oBinBlkMain.GrabGrade(GetBTBlock());

	//Get this grade statistic
	if( IsPrescanEnable() &&	//v4.50A24
		(pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()) )	//	region get the die in region only
	{
		ulGradeLeft = OpGetRegionGradeDieNum(ucMapGrade);
	}
	else
	{
		m_WaferMapWrapper.GetStatistics(ucMapGrade, ulGradeLeft, ulGradePick, ulGradeTotal);
	}

	//Return -- input grade count <= change limit
	if ( m_oBinBlkMain.GrabInputCount(ucBlkGrade) <= m_ulGradeChangeLimit)
	{
		return BT_DLA_NO_ACTION;
	}

	//v3.34
	//Determine if change in MIX mode; support PRE-LOAD in MIX mode (v3.34)
	if (m_WaferMapWrapper.GetPickMode() == WAF_CDieSelectionAlgorithm::MIXED_ORDER)
	{
		ULONG ulY=0, ulX=0;
		UCHAR ucGrade = 0;
		WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
		BOOL bEndOfWafer = FALSE;
		BOOL bChange = FALSE;
		ulGradeLeft = 0;
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();

		//Check upcoming "m_ulGradeChangeLimit" dice and see if change-grade is needed in MIX mode
		for (int i=1; i<(INT)m_ulGradeChangeLimit; i++)
		{
			PeekMapDie(i, ulY, ulX, ucGrade, eAction, bEndOfWafer);
			//	if die out of region, just stop and not change grade as next is unknow before prescan
			if( pUtl->GetPrescanRegionMode() )
			{
				if( pSRInfo->GetWithinRegionNo(ulY, ulX)!=ulTgtRegion )
				{
					return BT_DLA_NO_ACTION;
				}
			}
			if (bEndOfWafer)
				return BT_DLA_NO_ACTION;
			if ((ucGrade != 0) && (ucGrade != ucMapGrade) && (eAction == WAF_CDieSelectionAlgorithm::PICK))
			{
				bChange = TRUE;
				ulGradeLeft = i;
				break;
			}
		}

		if (!bChange)
			return BT_DLA_NO_ACTION;

		//v3.35
		//No CHANGE-GRADE is triggered if BIN_FULL will be triggered first
		if (ulGradeLeft >= ulBlkLeft)
		{
			return BT_DLA_NO_ACTION;
		}

		if (ucGrade > m_WaferMapWrapper.GetGradeOffset())
			ucNextGrade = ucGrade - m_WaferMapWrapper.GetGradeOffset();
		else
			ucNextGrade = 0;

		if ( ucNextGrade > 0 )
		{
			ulBinBlk = m_oBinBlkMain.GrabNVBlkInUse(ucNextGrade);
			if ( m_oBinBlkMain.GrabNVIsFull(ulBinBlk) == TRUE )
			{
				return BT_DLA_NO_ACTION; 
			}
		}

		szLogText.Format("Change Grade (MIX): Grade %d, Block %d, LEFT %d",ucMapGrade, ulBinBlk, ulGradeLeft);
		CMSLogFileUtility::Instance()->BT_DLALogStatus(szLogText);
	}
	else		//else IN_GRADE mode
	{
		//v3.35
		//In this case BIN_FULL should be triggered before CHANGE-GRADE, so
		//no CHANGE-GRADE action should be taken
		if (ulGradeLeft >= ulBlkLeft)
		{
		//	if( pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode() )	//	region get the die in current region only
		//		return ulBinBlk;
		//	else
				return BT_DLA_NO_ACTION;
		}

		if ( ulGradeLeft > m_ulGradeChangeLimit )		//v2.72a5
		{
			return ulBinBlk;
		}

		szLogText.Format("Block Left %d(%d), Grade Left %d(%d)", 
			ulBlkLeft, GetBTBlock(), ulGradeLeft, ucMapGrade-m_WaferMapWrapper.GetGradeOffset());
		CMSLogFileUtility::Instance()->BT_DLALogStatus(szLogText);

		BOOL bFoundCurrentGrade = FALSE;
		CUIntArray aulNextGradeList;
		CUIntArray aulGradeList;

		m_WaferMapWrapper.GetSelectedGradeList(aulGradeList);

		// filter grades with zero count
		for (i=0; i<aulGradeList.GetSize(); i++)
		{
			// must add current grade
			if ( ucMapGrade == aulGradeList[i] )
			{
				bFoundCurrentGrade = TRUE;
				aulNextGradeList.Add(aulGradeList[i]);
				continue;
			}

			if (bFoundCurrentGrade)
			{
				ulGradeLeft		= 0;
				ulGradePick		= 0;
				ulGradeTotal	= 0;
				if( IsPrescanEnable() &&	//v4.50A24
					(pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()) )	//	region get the die in current region only
				{
					ulGradeLeft = OpGetRegionGradeDieNum((UCHAR)aulGradeList[i]);
				}
				else
				{
					m_WaferMapWrapper.GetStatistics((UCHAR)aulGradeList[i], ulGradeLeft, ulGradePick, ulGradeTotal);
				}

				if (ulGradeLeft > 0)
				{
					aulNextGradeList.Add(aulGradeList[i]);
				}
			}
		}

		if (bFoundCurrentGrade == FALSE)
		{
			CMSLogFileUtility::Instance()->BT_DLALogStatus("No current found, NO ACTION");
			return BT_DLA_NO_ACTION;
		}

		if( aulNextGradeList.GetSize()==1 )
		{
			CMSLogFileUtility::Instance()->BT_DLALogStatus("CRRENT IS LAST, NO ACTION");
			return BT_DLA_NO_ACTION;
		}

		for (i=0; i<aulNextGradeList.GetSize(); i++)
		{
			if ( ucMapGrade == aulNextGradeList[i] )
			{
				if ( i == (aulNextGradeList.GetSize() - 1) )
				{
					//Return -- No more next grade						
					szLogText.Format("Last Grade %d", ucMapGrade - m_WaferMapWrapper.GetGradeOffset());
					CMSLogFileUtility::Instance()->BT_DLALogStatus(szLogText);
					return BT_DLA_NO_ACTION;
				}
				else
				{
					ucNextGrade = aulNextGradeList[i+1] - m_WaferMapWrapper.GetGradeOffset();
					if( IsPrescanEnable() &&	//v4.50A24
						(pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()) )
					{
						if( OpGetRegionNextGrade(i+1, aulNextGradeList, ucNextGrade)==FALSE ) 
						{
							szLogText.Format("Next Grade %d in current region", ucMapGrade - m_WaferMapWrapper.GetGradeOffset());
							pUtl->RegionOrderLog(szLogText);
							CMSLogFileUtility::Instance()->BT_DLALogStatus(szLogText);
							return BT_DLA_NO_ACTION;
						}
					}

					break;
				}
			}
		}

		if ( ucNextGrade > 0 )
		{
			if ( m_oBinBlkMain.GrabIsAssigned(ucNextGrade) == TRUE )
			{
				ulBinBlk = m_oBinBlkMain.GrabNVBlkInUse(ucNextGrade);

				if ( m_oBinBlkMain.GrabNVIsFull(ulBinBlk) == TRUE )
				{
					szLogText.Format("OpGetNextGradeBlk NV is full");
					CMSLogFileUtility::Instance()->BT_DLALogStatus(szLogText);

					return BT_DLA_NO_ACTION; 
				}
			}
		}

		szLogText.Format("Change Grade (IN_GRADE): Next Grade %d, Block %d", ucNextGrade, ulBinBlk);
		CMSLogFileUtility::Instance()->BT_DLALogStatus(szLogText);
	}

	return ulBinBlk;
}

	
ULONG CBinTable::OpGetNextNextGradeBlkInMixOrder(CONST ULONG ulCurrBlk)
{
	ULONG ulNextNextBlk = 0;
	UCHAR ucNextNextGrade = 0;
	ULONG ulBinBlk = BT_DLA_NO_ACTION;
	UCHAR ucMapGrade;
	CUIntArray aulGradeList;

	if ( ulCurrBlk <= 0 )
		return 0;
	if ( ulCurrBlk > BT_MAX_BINBLK_NO )
		return 0;

	ucMapGrade = m_oBinBlkMain.GrabGrade(ulCurrBlk) + m_WaferMapWrapper.GetGradeOffset();

	ULONG ulY=0, ulX=0;
	UCHAR ucGrade = 0;
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction;
	BOOL bEndOfWafer = FALSE;

	ULONG ulBlkLeft = m_oBinBlkMain.GrabNoOfDiePerBlk(ulCurrBlk) - m_oBinBlkMain.GrabNVNoOfBondedDie(ulCurrBlk);
	if ( m_oBinBlkMain.GrabMaxUnit(ulCurrBlk) != 0 )
	{
		ulBlkLeft = m_oBinBlkMain.GrabMaxUnit(ulCurrBlk) - m_oBinBlkMain.GrabNVNoOfBondedDie(ulCurrBlk);
	}

	ULONG ulInputCount = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(ulCurrBlk));
	if (ulInputCount <= m_oBinBlkMain.GrabNoOfDiePerBlk(ulCurrBlk))
	{
		ulBlkLeft = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(ulCurrBlk))
						- m_oBinBlkMain.GrabNVNoOfBondedDie(ulCurrBlk);
	}

	ULONG i=1;
	BOOL bChange = FALSE;

	do
	{
		PeekMapDie(i, ulY, ulX, ucGrade, eAction, bEndOfWafer);
		if (bEndOfWafer)
		{
			ulNextNextBlk = 0;
			return ulNextNextBlk;
		}

		if ((ucGrade != 0) && (ucGrade != ucMapGrade) && (eAction == WAF_CDieSelectionAlgorithm::PICK))
		{
			bChange = TRUE;
			ulNextNextBlk = ucGrade - m_WaferMapWrapper.GetGradeOffset();
			break;
		}

		i++;

	} while (i <= ulBlkLeft  && i <1000);		// Check grade-change before BIN_FULL for current bin frame (v4.25T2 added upper bound to 1000)

	if (!bChange)
	{
		ulNextNextBlk = ulCurrBlk;	// Next frame is new empty frame, not next-grade frame
	}

	return ulNextNextBlk;
}


//================================================================
//   Created-By  : Andrew Ng
//   Date        : 10/6/2007 11:03:13 AM
//   Description : 
//   Remarks     : 
//================================================================
ULONG CBinTable::OpGetNextNextGradeBlk(CONST ULONG ulCurrBlk)
{
	ULONG ulNextNextBlk = 0;
	UCHAR ucNextNextGrade = 0;
	ULONG ulBinBlk = BT_DLA_NO_ACTION;
	UCHAR ucMapGrade;
	CUIntArray aulGradeList;

	ULONG ulGradeLeft, ulGradePick, ulGradeTotal;
	int i;
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if (ulCurrBlk == 0)
		return ulNextNextBlk;
	if ((ulCurrBlk < 1) || (ulCurrBlk > BT_MAX_BINBLK_SIZE-1))	//v4.10
		return ulNextNextBlk;


	ucMapGrade = m_oBinBlkMain.GrabGrade(ulCurrBlk) + m_WaferMapWrapper.GetGradeOffset();
	
	// Check if BIN_FULL triggers first for current blk grade
	if (m_WaferMapWrapper.GetPickMode() == WAF_CDieSelectionAlgorithm::IN_GRADE_ORDER)
	{
		//Get this blk left count can be bonded
		ULONG ulBlkLeft = m_oBinBlkMain.GrabNoOfDiePerBlk(ulCurrBlk) - 
								m_oBinBlkMain.GrabNVNoOfBondedDie(ulCurrBlk);
		if ( m_oBinBlkMain.GrabMaxUnit(ulCurrBlk) != 0 )
		{
			ulBlkLeft = m_oBinBlkMain.GrabMaxUnit(ulCurrBlk) 
		  			  - m_oBinBlkMain.GrabNVNoOfBondedDie(ulCurrBlk);
		}
		
		//v3.45
		//frame capacity should be determined by Input-Count instead of max count, if Input count is smaller
		ULONG ulInputCount = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(ulCurrBlk));
		if (ulInputCount <= m_oBinBlkMain.GrabNoOfDiePerBlk(ulCurrBlk))
		{
			ulBlkLeft = m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(ulCurrBlk))
							- m_oBinBlkMain.GrabNVNoOfBondedDie(ulCurrBlk);
		}

		//Get this grade statistic
		ulGradeLeft = 0;
		ulGradePick = 0;
		ulGradeTotal = 0;
		if( IsPrescanEnable() &&	//v4.50A24
			(pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()) )	// to get grade die left in current region
			ulGradeLeft = OpGetRegionGradeDieNum(ucMapGrade);
		else
			m_WaferMapWrapper.GetStatistics(ucMapGrade, ulGradeLeft, ulGradePick, ulGradeTotal);

		if (ulGradeLeft > ulBlkLeft)		// If BIN_FULL triggers before Change-Grade
		{
			ulNextNextBlk = ulCurrBlk;		// Next frame is new empty frame, not next-grade frame
			return ulNextNextBlk;
		}
	}

	if ( ulCurrBlk > 0 )
	{
		CUIntArray aulAvailableGradeList;
		BOOL bFoundCurrentGrade = FALSE;
		m_WaferMapWrapper.GetSelectedGradeList(aulGradeList);

		// filter grades with zero count
		for (i=0; i<aulGradeList.GetSize(); i++)
		{
			// must add current grade
			if ( ucMapGrade == aulGradeList[i] )
			{
				bFoundCurrentGrade = TRUE;
				aulAvailableGradeList.Add(aulGradeList[i]);
				continue;
			}

			if (bFoundCurrentGrade)
			{
				ulGradeLeft		= 0;
				ulGradePick		= 0;
				ulGradeTotal	= 0;
				if( IsPrescanEnable() &&	//v4.50A24
					(pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()) )	// to get grade die left in current region
				{
					ulGradeLeft = OpGetRegionGradeDieNum((UCHAR)aulGradeList[i]);
				}
				else
					m_WaferMapWrapper.GetStatistics((UCHAR)aulGradeList[i], ulGradeLeft, ulGradePick, ulGradeTotal);

				if (ulGradeLeft > 0)
				{
					aulAvailableGradeList.Add(aulGradeList[i]);
				}
			}
		}

		if (bFoundCurrentGrade == FALSE)
		{
			ulNextNextBlk = Err_BinBlockNotAssign;	//v2.93T2
			return ulNextNextBlk;
		}

		for (i=0; i<aulAvailableGradeList.GetSize(); i++)
		{
			if ( ucMapGrade == aulAvailableGradeList[i] )
			{
				if ( i == (aulAvailableGradeList.GetSize() - 1) )		// curr blk is last grade in list; no need to get next grade
				{
					ulNextNextBlk = Err_BinBlockNotAssign;	//v2.93T2
					return ulNextNextBlk;
				}
				else
				{
					// else get next grade
					ucNextNextGrade = aulAvailableGradeList[i+1] - m_WaferMapWrapper.GetGradeOffset();
					break;
				}
			}
		}

		// Get next grade blk num
		if ( ucNextNextGrade > 0 )
		{
			if ( m_oBinBlkMain.GrabIsAssigned(ucNextNextGrade) == TRUE )
			{
				ulBinBlk = m_oBinBlkMain.GrabNVBlkInUse(ucNextNextGrade);
				if ( m_oBinBlkMain.GrabNVIsFull(ulBinBlk) == TRUE )
				{
					return ulNextNextBlk; 
				}
				ulNextNextBlk = ulBinBlk;
			}
		}
	}

	//(*m_psmfSRam)["BinTable"]["BLA_Mode"]["Next Next Blk"] = ulNextNextBlk;		// Save Next-Next blk num into SRam for BL
	return ulNextNextBlk;
}


BOOL CBinTable::OpPreChangeNextGrade()
{
	//Currently not available for DLA config
	BOOL bBinExArm	= (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["ExChgArmExist"];
	if (bBinExArm)
		return FALSE;


	//Get this blk left count can be bonded
	ULONG ulBlkLeft = m_oBinBlkMain.GrabNoOfDiePerBlk(GetBTBlock()) - m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
	if ( m_oBinBlkMain.GrabMaxUnit(GetBTBlock()) != 0 )
	{
		ulBlkLeft = m_oBinBlkMain.GrabMaxUnit(GetBTBlock()) - m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
	}

	//Get this grade count can be picked
	UCHAR ucMapGrade = m_oBinBlkMain.GrabGrade(GetBTBlock()) + m_WaferMapWrapper.GetGradeOffset();

	//Get this grade statistic
	ULONG ulGradeLeft=0, ulGradePick=0, ulGradeTotal=0;
	m_WaferMapWrapper.GetStatistics(ucMapGrade, ulGradeLeft, ulGradePick, ulGradeTotal);
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( IsPrescanEnable() &&	//v4.50A24
		(pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()) )
	{
		ulGradeLeft = OpGetRegionGradeDieNum(ucMapGrade);
	}

	if ((ulGradeLeft == 0) &&		//Time to change next grade frame
		(ulBlkLeft > 0))			//Current frame is not FULL
	{
CString szTemp;
szTemp.Format("Grade = %d; Left = %d; blk left = %d; total = %d", GetBTBlock(), ulGradeLeft, ulBlkLeft, ulGradeTotal);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		return TRUE;
	}
	else
		return FALSE;
}


BOOL CBinTable::OpGetBlkIsNearFull()
{
	//return FALSE;	//v2.67

	//--Temp. comment this function for future use--
	ULONG ulNearFullLimit = m_ulGradeChangeLimit;
	ULONG ulBlkLeft = 0;
	ULONG ulGradeLeft = 0;
	ULONG ulGradeTotal = 0;
	ULONG ulGradePick = 0;
	UCHAR ucBlkGrade = 0;
	UCHAR ucMapGrade = 0;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanAreaPickMode() )
	{
	//	if ( GetBTBlock() > 0 )
	//	{
	//		//Get this blk left count can be bonded
	//		ulBlkLeft = m_oBinBlkMain.GrabNoOfDiePerBlk(GetBTBlock()) - m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
	//		if( ulBlkLeft<=(ulNearFullLimit*2) )
	//		{
	//			return TRUE;
	//		}
	//	}
	
		return FALSE;
	}

	//Return -- Near end of wafer
	if ( m_WaferMapWrapper.GetSelectedDiceLeft() <= (ulNearFullLimit*2) )
	{
		return FALSE;
	}

	if ( GetBTBlock() == 0 )
	{
		return FALSE;
	}

	//Get this blk left count can be bonded
	ulBlkLeft = m_oBinBlkMain.GrabNoOfDiePerBlk(GetBTBlock())
				- m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());

	if ( m_oBinBlkMain.GrabMaxUnit(GetBTBlock()) != 0 )
	{
		ulBlkLeft = m_oBinBlkMain.GrabMaxUnit(GetBTBlock()) 
		  			- m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
	}

	//Get this blk grade in use
	ucBlkGrade = m_oBinBlkMain.GrabGrade(GetBTBlock());

	//Get this grade count left for pick
	ucMapGrade = m_oBinBlkMain.GrabGrade(GetBTBlock()) 
				+ m_WaferMapWrapper.GetGradeOffset();

		//v3.45
	//frame capacity should be determined by  Input Count instead of max count, if Input count is smaller
	ULONG ulInputCount = m_oBinBlkMain.GrabInputCount(ucBlkGrade);
	if (ulInputCount <= m_oBinBlkMain.GrabNoOfDiePerBlk(GetBTBlock()))
	{
		ulBlkLeft = m_oBinBlkMain.GrabInputCount(ucBlkGrade)
						- m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());
	} 

	//Get this grade statistic
	m_WaferMapWrapper.GetStatistics(ucMapGrade, ulGradeLeft, ulGradePick, ulGradeTotal);
	if( IsPrescanEnable() &&	//v4.50A24
		(pUtl->GetPrescanRegionMode() || pUtl->GetRegionPickMode()) )
	{
		ulGradeLeft = OpGetRegionGradeDieNum(ucMapGrade);
	}

	//Return -- Grade Left < Blk Left
	if ( ulGradeLeft < ulBlkLeft )
	{
		return FALSE;
	}

	//Return -- input grade count <= bonded die count
	if ( m_oBinBlkMain.GrabInputCount(ucBlkGrade) <= m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock()) )
	{
		return FALSE;
	}

	//Get this blk grade input count can be bonded
	ulGradeLeft = m_oBinBlkMain.GrabInputCount(ucBlkGrade)
				- m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock());

	//Return -- input grade count <= change limit
	if ( m_oBinBlkMain.GrabInputCount(ucBlkGrade) <= ulNearFullLimit)
	{
		return FALSE;
	}

	//Return TRUE -- remained blk left <= change limit or remained grade left <= change limit
	if ( (ulBlkLeft <= ulNearFullLimit) || (ulGradeLeft <= ulNearFullLimit) )		//v2.72a5
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CBinTable::OpPrPreBondLFPad()
{
	if (!m_bPrPreBondAlignment)
		return TRUE;

	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		BOOL	bIsBHArm2;
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;
	
	BOOL bIsArm1 = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];
	if (bIsArm1)
	{
		stInfo.bIsBHArm2 = FALSE;
	}
	else
	{
		stInfo.bIsBHArm2 = TRUE;
	}

//CMSLogFileUtility::Instance()->BT_TableIndexLog("BT: OpPrPreBondLFPad start ----");

	CString szLog;
	IPC_CServiceMessage rReqMsg;
	rReqMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);

	INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "LookForwardPreBondPattern", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	if (!stInfo.bResult)
	{
		//m_lPreBondTableOffsetX		= 0;
		//m_lPreBondTableOffsetY		= 0;
		(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"] = FALSE;
		return FALSE;
	}

	m_lPreBondTableOffsetX		= stInfo.siStepX;
	m_lPreBondTableOffsetY		= stInfo.siStepY;
	(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"] = TRUE;

	szLog.Format("BT: OpPrPreBondLFPad OK - Pad-Offset(%ld, %ld)", stInfo.siStepX, stInfo.siStepY);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		
	return TRUE;
}

BOOL CBinTable::OpPrPreBondAlignPad()
{
	if (!m_bPrPreBondAlignment)
		return TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPr = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);

	//andrewng2
	BOOL bIsLFPadFound = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"];
	if (bIsLFPadFound)
	{
		//If PAD is found at LF, then no need to redo PreBondAlignPad
		if (!bEnableBHUplookPr)
		{
			m_lPreBondTableOffsetX = 0;
			m_lPreBondTableOffsetY = 0;
			(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"] = FALSE;	
		}
		return TRUE;
	}


	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		BOOL	bIsBHArm2;	// CSP003c
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;
	
	IPC_CServiceMessage rReqMsg;
	rReqMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);

	INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "SearchPreBondPattern", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	if (!stInfo.bResult)
	{
		m_lPreBondTableOffsetX		= 0;
		m_lPreBondTableOffsetY		= 0;
		SetAlert_Red_Yellow(IDS_BT_PRE_BOND_ALIGN_PAD_FAIL);
	//	return FALSE;
		GetEncoderValue();
		LONG lOldX = m_lEnc_X;
		LONG lOldY = m_lEnc_Y;
		IPC_CServiceMessage stMsg;
		SwitchToBPR();
		int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysLiveMode", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		SetJoystickOn(TRUE);
		DisplaySequence("BT - se PR mouse to locate target position.");
		LONG lHmi = HmiMessageEx("Please use PR mouse to locate target position.", "BT PreBond", glHMI_MBX_OKCANCEL, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysBondMode", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		SetJoystickOn(FALSE);
		GetEncoderValue();
		stInfo.siStepX	= m_lEnc_X - lOldX;
		stInfo.siStepY	= m_lEnc_Y - lOldY;
		SetErrorMessage("BT: Prebond pad alignment failure");
		if( lHmi!=glHMI_OK )
			return FALSE;
	}

	if (m_bUseDualTablesOption && (m_nBTInUse == 1))	//BT2	//v4.20
	{
		m_lPreBondTableOffsetX		= stInfo.siStepX;
		m_lPreBondTableOffsetY		= stInfo.siStepY;

		if (!bEnableBHUplookPr)		//andrewng
		{
			XY2_Move((INT)m_lPreBondTableOffsetX, (INT)m_lPreBondTableOffsetY, SFM_WAIT);		// Move table 2
		}
	}
	else
	{
		m_lPreBondTableOffsetX		= stInfo.siStepX;
		m_lPreBondTableOffsetY		= stInfo.siStepY;

		if (stInfo.bIsBHArm2)	//andrewng12345
		{
			m_lPreBondTableOffsetX = m_lPreBondTableOffsetX + m_lBHZ2BondPosOffsetX;
			m_lPreBondTableOffsetY = m_lPreBondTableOffsetY + m_lBHZ2BondPosOffsetY;
		}

		LONG lRTOffsetX=0, lRTOffsetY=0;
		BOOL bPostBondRTCompXY = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["PB RTComp"];
		if (bPostBondRTCompXY)
		{
			if (stInfo.bIsBHArm2)
			{		
				lRTOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 X"];
				lRTOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 Y"];
			}
			else
			{
				lRTOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 X"];
				lRTOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 Y"];
			}
				
			m_lPreBondTableOffsetX = m_lPreBondTableOffsetX - lRTOffsetX;
			m_lPreBondTableOffsetY = m_lPreBondTableOffsetY - lRTOffsetY;
		}


		if (!bEnableBHUplookPr)		//andrewng
		{
			GetEncoderValue();
			LONG lOldX = m_lEnc_X;
			LONG lOldY = m_lEnc_Y;
			LONG lCmdX = m_lCmd_X;
			LONG lCmdY = m_lCmd_Y;

			//v4.59A22	//David Ma
			DOUBLE dThermalDX = m_oBinBlkMain.FindThermalDeltaPitchX(GetBTBlock(), m_lCol);
			DOUBLE dThermalDY = m_oBinBlkMain.FindThermalDeltaPitchX(GetBTBlock(), m_lRow);
			LONG lThermalDX = ConvertFileUnitToXEncoderValue(dThermalDX);
			LONG lThermalDY = ConvertFileUnitToYEncoderValue(dThermalDY);

			XY_Move((INT) m_lPreBondTableOffsetX + lThermalDX, 
					(INT) m_lPreBondTableOffsetY + lThermalDY, 
					SFM_WAIT);		// Move table 1

			GetEncoderValue();
			
			CString szLog;
			
			if (stInfo.bIsBHArm2)	//andrewng12345
			{
				szLog.Format("BT: PreBond AlignPad (BHZ2) - Old-Enc(%ld, %ld), Old-Cmd(%ld, %ld), New-Enc(%ld, %ld), RTOffset -(%ld, %ld), Arm2Offset +(%ld, %ld), Final-Offset(%ld, %ld), ThermalD(%d, %d)", 
					lOldX, lOldY,  lCmdX, lCmdY, m_lEnc_X, m_lEnc_Y, 
					lRTOffsetX, lRTOffsetY, m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY,
					m_lPreBondTableOffsetX, m_lPreBondTableOffsetY,
					lThermalDX, lThermalDY);	//v4.59A22		//David Ma
			}
			else
			{
				szLog.Format("BT: PreBond AlignPad (BHZ1) - Old-Enc(%ld, %ld), Old-Cmd(%ld, %ld), New-Enc(%ld, %ld), RTOffset -(%ld, %ld), Final-Offset(%ld, %ld), ThermalD(%d, %d)", 
					lOldX, lOldY,  lCmdX, lCmdY, m_lEnc_X, m_lEnc_Y, lRTOffsetX, lRTOffsetY,
					m_lPreBondTableOffsetX, m_lPreBondTableOffsetY,
					lThermalDX, lThermalDY);	//v4.59A22		//David Ma
			}
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		}
	}

/*
	typedef struct 
	{
	    int		siStepX;
	    int		siStepY;
    	BOOL    bResult;
		int		iPrX;
		int		iPrY;
	} BPR_DIEOFFSET;
	BPR_DIEOFFSET stInfo;
	
	IPC_CServiceMessage rReqMsg;
	rReqMsg.InitMessage(sizeof(BPR_DIEOFFSET), &stInfo);

	INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "SearchPreBondPattern", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
			rReqMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	if (!stInfo.bResult)
		return FALSE;

	if (m_bUseDualTablesOption && (m_nBTInUse == 1))	//BT2	//v4.20
	{
		XY2_Move(stInfo.siStepX, stInfo.siStepY, SFM_WAIT);		// Move table 2
	}
	else
	{
		INT nX = stInfo.siStepX;
		INT nY = stInfo.siStepY;

		//v4.55A11
		BOOL bBHZ2TowardsPick = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
		if (!bBHZ2TowardsPick)		//-> BH2 towards BOND
		{
			nX = nX + m_lBHZ2BondPosOffsetX;
			nY = nY + m_lBHZ2BondPosOffsetY;
		}

		//v4.55A11
		LONG lRTOffsetX=0, lRTOffsetY=0;
		BOOL bPostBondRTCompXY = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["PB RTComp"];
		if (bPostBondRTCompXY)
		{
			if (!bBHZ2TowardsPick)		//-> BH2 towards BOND
			{		
				lRTOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 X"];
				lRTOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 Y"];
			}
			else
			{
				lRTOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 X"];
				lRTOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 Y"];
			}
				
			nX = nX - lRTOffsetX;
			nY = nY - lRTOffsetY;
		}

		XY_Move(nX, nY, SFM_WAIT);		// Move table 1

		//v4.55A11
		CString szLog;
		if (!bBHZ2TowardsPick)		//-> BH2 towards BOND
		{
			szLog.Format("BT: PreBond AlignPad (BHZ2) - OrigOffset(%d, %d), C2Offset+(%ld, %ld), RT-Offset-(%d, %ld), Final-Offset(%ld, %ld)", 
					stInfo.siStepX, stInfo.siStepY,
					m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY,
					lRTOffsetX, lRTOffsetY,
					nX, nY);
		}
		else
		{
			szLog.Format("BT: PreBond AlignPad (BHZ1) - OrigOffset(%d, %d), RT-Offset-(%d, %ld), Final-Offset(%ld, %ld)", 
					stInfo.siStepX, stInfo.siStepY,
					lRTOffsetX, lRTOffsetY,
					nX, nY);
		}
		
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}
*/
	return TRUE;
}


BOOL CBinTable::OpAddPostBondOffset(LONG &lX, LONG&lY, LONG lDebug)
{
#ifdef NU_MOTION
	//v3.66		//For DBH only
	BOOL bBHZ2TowardsPick		 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
	BOOL bChangeGradeResetToBH1	 = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["ResetToBH1"];	//v4.47T10
	BOOL bChangeGradeNoResetToBH1= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["ChangeGrade"]["EjtXYNoReset"];	//v4.47T10
	BOOL bPostBondRTCompXY		 = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["PB RTComp"];
	CString szMsg;
	CString szDebugMsg;

	szDebugMsg.Format( "**BOND** %s Original Position to move: (%d,%d)", bBHZ2TowardsPick?"BH2":"BH1",lX,lY );
	CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);

	GetEncoderValue();
	LONG lOriginalX = lX;
	LONG lOriginalY = lY;

	//EJT OFfset XY in motor steps; encoder resolution of EJT is same as 
	//	WFT XY and BT XT of 0.5um/motor-step;
	LONG lCollet1OffsetX = 0, lCollet1OffsetY = 0;
	LONG lCollet2OffsetX = 0, lCollet2OffsetY = 0;
	GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dXResolution_UM_CNT, TRUE);


	//CMSLogFileUtility::Instance()->BT_BinTableMapIndexLogOpen();
	if (!m_bFirstCycleBT && !bChangeGradeResetToBH1 && !bChangeGradeNoResetToBH1)
	{
		//1. BH2 Offset XY
		if (!bBHZ2TowardsPick)	// -> BHZ2 towards BOND!
		{
			//this offset XY only used for BHZ2 in DBH design only
//			lX = lX + m_lBHZ2BondPosOffsetX;
//			lY = lY + m_lBHZ2BondPosOffsetY;

			(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = FALSE;		//v4.48A19

			szDebugMsg.Format( "**BOND**a** BH2 Offset: +(%d, %d)",m_lBHZ2BondPosOffsetX,m_lBHZ2BondPosOffsetY);
			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);
		}
		else
		{	
//			lX = lX + m_lBHZ1BondPosOffsetX;
//			lY = lY + m_lBHZ1BondPosOffsetY;

			(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = TRUE;		//v4.48A19
			szDebugMsg.Format( "**BOND** BH1 Offset: +(%d, %d)",m_lBHZ1BondPosOffsetX,m_lBHZ1BondPosOffsetY);
			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);
			
		}

		//v4.44A3
		//szMsg.Format(": OrigEnc(%ld, %ld) ,BH2=%d, +BH2Offset(%ld, %ld), FinalEnc(%ld, %ld)", 
		//	lOriginalX, lOriginalY, 
		//	!bBHZ2TowardsPick, m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY,
		//	lX, lY);

		//2. PostBOnd RT Offset XY
		LONG lOffsetX=0, lOffsetY=0;
		if (bPostBondRTCompXY)
		{
			if (!bBHZ2TowardsPick)	// -> BHZ2 towards BOND!
			{		
				lOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 X"];
				lOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 Y"];
				szDebugMsg.Format( "**BOND** BHZ2 PB RT Offset XY: -(%d,%d)",lOffsetX, lOffsetY);			
			}
			else
			{
				lOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 X"];
				lOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 Y"];
				szDebugMsg.Format( "**BOND** BHZ1 PB RT Offset XY: -(%d,%d)",lOffsetX, lOffsetY);
			}
				
			lX = lX - lOffsetX;
			lY = lY - lOffsetY;
			//LOG_BT(szMsg);
			//CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);

			//v4.44A3
			//szMsg.Format(": OrigEnc(%ld, %ld) ,BH2=%d, +BH2Offset(%ld, %ld), -RTCompOffset(%ld, %ld), FinalEnc(%ld, %ld)", 
			//	lOriginalX, lOriginalY, 
			//	!bBHZ2TowardsPick, m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY,
			//	lOffsetX, lOffsetY,
			//	lX, lY);
		}

		//v4.42T3
		//3. Ejector Collet Offset XY
		//if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A1	Semitek
		if (m_bEnableMS100EjtXY)	//v4.52A14
		{
			LONG OldX ,OldY;
			OldX = lX;
			OldY = lY;
			// move offset
			m_lBinTableDebugCounter++;

			if (/*m_bBTIsMoveCollet == TRUE &&*/ m_bEnableMS100EjtXY == TRUE)
			{
				CString szCountLog;
				if (!bBHZ2TowardsPick)	// -> BHZ2 towards BOND!
				{
					lX = lX - lCollet2OffsetX;
					lY = lY - lCollet2OffsetY;
					szDebugMsg.Format( "**BOND** BHZ2 Move EJT Collet Offset 1. Old:(%d,%d) New:(%d,%d) Offset:-(%d,%d)",
						OldX ,OldY,lX,lY,lCollet2OffsetX,lCollet2OffsetY);	
					szCountLog.Format( "%d,%d,%d,%d,%d,%d,%d" ,m_lBinTableDebugCounter,2,m_lTempIndex,lX,lY,m_lEnc_X,m_lEnc_Y);
				
					//v4.44A3
					szMsg.Format(": OrigEnc(%ld, %ld) ,BH2=%d, +BH1Offset(%ld, %ld),+BH2Offset(%ld, %ld), -RTCompOffset(%ld, %ld), -EjtC2Offset(%ld, %ld), FinalEnc(%ld, %ld)", 
						lOriginalX, lOriginalY, 
						!bBHZ2TowardsPick, m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY,m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY,
						lOffsetX, lOffsetY, lCollet2OffsetX, lCollet2OffsetY,
						lX, lY);	
				}
				else
				{
					lX = lX - lCollet1OffsetX;
					lY = lY - lCollet1OffsetY;
					szDebugMsg.Format( "**BOND** BHZ1 Move EJT Collet Offset 1. Old:(%d,%d) New:(%d,%d) Offset:-(%d,%d)",
						OldX ,OldY,lX,lY,lCollet1OffsetX,lCollet1OffsetY);	
					szCountLog.Format( "%d,%d,%d,%d,%d,%d,%d" ,m_lBinTableDebugCounter,1,m_lTempIndex,lX,lY,m_lEnc_X,m_lEnc_Y);
				
					//v4.44A3
					szMsg.Format(": OrigEnc(%ld, %ld) ,BH2=%d, +BH1Offset(%ld, %ld),+BH2Offset(%ld, %ld), -RTCompOffset(%ld, %ld), -EjtC1Offset(%ld, %ld), FinalEnc(%ld, %ld)", 
						lOriginalX, lOriginalY, 
						!bBHZ2TowardsPick, m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY,m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY,
						lOffsetX, lOffsetY, lCollet1OffsetX, lCollet1OffsetY,
						lX, lY);					
				} 

				CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);
				//CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);
				//CMSLogFileUtility::Instance()->BT_BinTableMapIndexLog(szCountLog);
			}
		}
	}
	else 
	{
		(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = TRUE;		//v4.48A19	//For BPR PostBond SrchWnd

		//1. BH2 Offset XY		//v4.48A29
		if (bChangeGradeNoResetToBH1)
		{
			if (!bBHZ2TowardsPick)	// -> BHZ2 towards BOND!
			{
/*
				//v4.48A29
				//BH2 towards BOND -> 1st to be picked is at BH1,
				//so BT should move to BH1 offset
				lX = lX + m_lBHZ1BondPosOffsetX;
				lY = lY + m_lBHZ1BondPosOffsetY;
*/
				(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = TRUE;

				szDebugMsg.Format( "**BOND** BH1 Offset: +(%d, %d)", m_lBHZ1BondPosOffsetX,m_lBHZ1BondPosOffsetY);
				//CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);
				CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);
			}
			else
			{
/*
				//this offset XY only used for BHZ2 in DBH design only
				lX = lX + m_lBHZ2BondPosOffsetX;
				lY = lY + m_lBHZ2BondPosOffsetY;
*/
				(*m_psmfSRam)["BondPr"]["BinTable At BH1"] = FALSE;

				szDebugMsg.Format( "**BOND**b** BH2 Offset: +(%d, %d)", m_lBHZ2BondPosOffsetX,m_lBHZ2BondPosOffsetY);
				//CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);
				CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);
			}
		}

		//v4.47T10	//SanAn WH
		if (bPostBondRTCompXY)
		{
			LONG lOffsetX=0, lOffsetY=0;

			//v4.48A19	//WH SanAn sw debugging
			if (m_bEnableMS100EjtXY &&			//v4.52A14
				bChangeGradeNoResetToBH1 &&
				bBHZ2TowardsPick)
			{
				lOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 X"];
				lOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH2 Y"];
				szDebugMsg.Format( "**BOND** BHZ2 PB RT Offset XY: -(%d,%d)",lOffsetX, lOffsetY);			
				lX = lX - lOffsetX;
				lY = lY - lOffsetY;
			}
			else
			{
				lOffsetX = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 X"];
				lOffsetY = (LONG)(*m_psmfSRam)["BondPr"]["AVG BH1 Y"];				
				szDebugMsg.Format( "**BOND** BHZ1 PB RT Offset XY: -(%d,%d)",lOffsetX, lOffsetY);
				lX = lX - lOffsetX;
				lY = lY - lOffsetY;
			}

			CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);	//v4.48A26
		}

		//if (m_bMS100EjtXY && m_bEnableMS100EjtXY)	//v4.44A1	//Semitek
		if (m_bEnableMS100EjtXY)	//v4.52A14
		{
			if (/*m_bBTIsMoveCollet == TRUE &&*/ m_bEnableMS100EjtXY == TRUE)
			{
				LONG OldX ,OldY;
				OldX = lX;
				OldY = lY;

				//Debug Log
				CString szCountLog;
				ULONG ulDieIndex = 0;
				//Get this blk grade in use
				ULONG ulBlkToUse = (*m_psmfSRam)["BinTable"]["BlkInUse"] ; 
				ULONG ulTempRow = 0, ulTempCol = 0;
				m_oBinBlkMain.GetDieIndexInBinMap(ulBlkToUse, ulDieIndex, ulTempRow, ulTempCol);	

				if (bChangeGradeNoResetToBH1)	//v4.47A3	//v4.47T17
				{
					if (!bBHZ2TowardsPick)		// -> BHZ2 towards BOND!
					{
						lX = lX - lCollet1OffsetX;
						lY = lY - lCollet1OffsetY;
						szDebugMsg.Format( "**BOND** BHZ1 Move EJT Collet Offset. Old:(%d,%d) New:(%d,%d) Offset:-(%d,%d)",
							OldX ,OldY,lX,lY,lCollet1OffsetX,lCollet1OffsetY);	
						//szCountLog.Format( "%d,%d,%d,%d,%d,%d,%d" ,m_lBinTableDebugCounter,1,m_lTempIndex,lX,lY,m_lEnc_X,m_lEnc_Y);
					}
					else
					{
						lX = lX - lCollet2OffsetX;
						lY = lY - lCollet2OffsetY;
						szDebugMsg.Format( "**BOND** BHZ2 Move EJT Collet Offset. Old:(%d,%d) New:(%d,%d) Offset:-(%d,%d)",
							OldX ,OldY,lX,lY,lCollet2OffsetX,lCollet2OffsetY);	
						//szCountLog.Format( "%d,%d,%d,%d,%d,%d,%d" ,m_lBinTableDebugCounter,2,m_lTempIndex,lX,lY,m_lEnc_X,m_lEnc_Y);
					} 
				}
				else
				{
					lX = lX - lCollet1OffsetX;
					lY = lY - lCollet1OffsetY;
					szDebugMsg.Format( "**BOND** BHZ1 Move EJT Collet Offset(FIRST TIME). Old:(%d,%d) New:(%d,%d) Offset: -(%d,%d)",
							OldX ,OldY,lX,lY,lCollet1OffsetX,lCollet1OffsetY);	
					//szCountLog.Format( "%d,%d,%d,%d,%d,%d,%d" ,m_lBinTableDebugCounter,0,m_lTempIndex,lX,lY,m_lEnc_X,m_lEnc_Y);
				}

				CMSLogFileUtility::Instance()->BT_TableIndexLog(szDebugMsg);
				//CMSLogFileUtility::Instance()->BPR_Arm1Log(szDebugMsg);
				//CMSLogFileUtility::Instance()->BT_BinTableMapIndexLog(szCountLog);
			}
		}
	}

//v4.44A3
//if (szMsg.GetLength() > 0)
//{
	//CString szHeader;
	//szHeader.Format("INDEX - Bin #%lu (%lu) (%lu)", GetBTBlock(), 
	//					m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock()), lDebug);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szHeader + szMsg);
//}
	//CMSLogFileUtility::Instance()->BT_BinTableMapIndexLogClose();
#endif

	return TRUE;
}

//v4.55A10
BOOL CBinTable::OpAddBHAtPrePickOffset(LONG &lX, LONG &lY)
{
	if ( (m_lBHZ1PrePickToBondOffsetX != 0) ||		//Used as BHZ1 PICK offset
		 (m_lBHZ1PrePickToBondOffsetY != 0) ||		//Used as BHZ1 BOND offset 
		 (m_lBHZ2PrePickToBondOffsetX != 0) || 		//Used as BHZ2 PICK offset
		 (m_lBHZ2PrePickToBondOffsetY != 0) )		//Used as BHZ2 BOND offset
	{
		CString szLog	= "";
		LONG lOldX		= m_lX;
		LONG lOldY		= m_lY;

		BOOL bBHZ2TowardsPick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["BHZ2TowardsPICK"];
		BOOL bBHPrePick2ndDie	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["AddPrePickOffsetTo2ndDie"];
		BOOL bBHAtPrePick		= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
		if ( bBHAtPrePick )
		{
			(*m_psmfSRam)["BinTable"]["AddPrePickOffsetTo2ndDie"] = TRUE;	

			if (bBHZ2TowardsPick)
			{
				//BHZ1 to BOND
				//Add BHZ1 BOND OffsetX
				if (labs(m_lBHZ1PrePickToBondOffsetY) <= 1000)
					m_lBHZ1CurrPrePickOffsetX = m_lBHZ1CurrPrePickOffsetX + m_lBHZ1PrePickToBondOffsetY;
				
				m_lX = m_lX + m_lBHZ1CurrPrePickOffsetX;
				(*m_psmfSRam)["BinTable"]["Collet1PrePickOffsetX"] = m_lBHZ1CurrPrePickOffsetX;

				//BHZ2 at PICK
				//Add BHZ2 PICK OffsetX
				if (labs(m_lBHZ2PrePickToBondOffsetX) <= 1000)
					m_lBHZ2CurrPrePickOffsetX = m_lBHZ2PrePickToBondOffsetX;	

				szLog.Format("BT: PrePick Offset XY (BHZ1) added - OLD(%ld, %ld), PrePickOffsetX(BHZ1)(%ld), NEW(%ld, %ld)",
								lOldX, lOldY, m_lBHZ1CurrPrePickOffsetX, m_lX, m_lY);

				m_lBHZ1CurrPrePickOffsetX = 0;
			}
			else
			{
				//BHZ2 to BOND
				//Add BHZ2 BOND OffsetX
				if (labs(m_lBHZ2PrePickToBondOffsetY) <= 1000)
					m_lBHZ2CurrPrePickOffsetX = m_lBHZ2CurrPrePickOffsetX + m_lBHZ2PrePickToBondOffsetY;
				
				m_lX = m_lX + m_lBHZ2CurrPrePickOffsetX;
				(*m_psmfSRam)["BinTable"]["Collet2PrePickOffsetX"] = m_lBHZ2CurrPrePickOffsetX;

				//BHZ1 at PICK
				if (labs(m_lBHZ1PrePickToBondOffsetX) <= 1000)
					m_lBHZ1CurrPrePickOffsetX = m_lBHZ1PrePickToBondOffsetX;	

				szLog.Format("BT: PrePick Offset XY (BHZ2) added - OLD(%ld, %ld), PrePickOffsetX(BHZ2)(%ld), NEW(%ld, %ld)",
								lOldX, lOldY, m_lBHZ2CurrPrePickOffsetX, m_lX, m_lY);

				m_lBHZ2CurrPrePickOffsetX = 0;
			}

			CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		}
		else if (bBHPrePick2ndDie)
		{
			if (bBHZ2TowardsPick)
			{
				//BHZ1 to BOND
				m_lX = m_lX + m_lBHZ1CurrPrePickOffsetX;
				(*m_psmfSRam)["BinTable"]["Collet1PrePickOffsetX"] = m_lBHZ1CurrPrePickOffsetX;

				szLog.Format("BT: PrePick 2nd Offset XY (BHZ1) added - OLD(%ld, %ld), PrePickOffsetX(%ld), NEW(%ld, %ld)",
								lOldX, lOldY, m_lBHZ1CurrPrePickOffsetX, m_lX, m_lY);
				m_lBHZ1CurrPrePickOffsetX = 0;
			}
			else
			{
				//BHZ2 to BOND
				m_lX = m_lX + m_lBHZ2CurrPrePickOffsetX;
				(*m_psmfSRam)["BinTable"]["Collet2PrePickOffsetX"] = m_lBHZ2CurrPrePickOffsetX;

				szLog.Format("BT: PrePick 2nd Offset XY (BHZ2) added - OLD(%ld, %ld), PrePickOffsetX(%ld), NEW(%ld, %ld)",
								lOldX, lOldY, m_lBHZ2CurrPrePickOffsetX, m_lX, m_lY);
				m_lBHZ2CurrPrePickOffsetX = 0;
			}

			(*m_psmfSRam)["BinTable"]["AddPrePickOffsetTo2ndDie"] = FALSE;	
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		}
		else
		{
			m_lBHZ1CurrPrePickOffsetX	= 0;
			m_lBHZ2CurrPrePickOffsetX	= 0;
			(*m_psmfSRam)["BinTable"]["AddPrePickOffsetTo2ndDie"] = FALSE;	
			(*m_psmfSRam)["BinTable"]["Collet1PrePickOffsetX"] = 0;
			(*m_psmfSRam)["BinTable"]["Collet2PrePickOffsetX"] = 0;
		}
	}

	return TRUE;
}


//v4.44T2
BOOL CBinTable::OpUpdateMS100BinRealignAngle(LONG lEncX, LONG lEncY)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableBT)
		return TRUE;
	BOOL bReAlignFrame = (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["RealignBinFrame"];
	if (!bReAlignFrame)
		return TRUE;
	//Only available for TLH sorting path at the moment
	if (m_oBinBlkMain.GrabBinMapWalkPath(GetBTBlock()) != BT_TLH_PATH)
		return TRUE;
	//Not available for Nichia's binmap fcn!!
	if (!m_oBinBlkMain.IsUseBinMapBondArea(GetBTBlock()) && m_oBinBlkMain.GrabEnableBinMap(GetBTBlock()))
		return TRUE;
	if (!m_bSemitekBLMode)	//IsMS60())		//v4.50A29
		return TRUE;	//v4.47T10	//DO not use this fcn on MS100PIII at this moment

	
	//SemitekMode: include OLD or NEW align method; however, 
	//	this Mode only available for Semitek at the moment;
	if (!m_bSemitekBLMode)			//Only available for Semitek
		return TRUE;	
	BOOL bNewAlignMethod = CMS896AApp::m_bBinFrameNewRealignMethod;	//Semitek	//use 2nd-die offset instead of 1st-die


	//1st die Index
	ULONG ulFirstDieIndex	= m_oBinBlkMain.GrabSkipUnit(GetBTBlock());			//v4.51A5	//zero-based
	if (bNewAlignMethod)
		ulFirstDieIndex		= 1 + m_oBinBlkMain.GrabSkipUnit(GetBTBlock());		//v4.51A5	//zero-based
	if (m_oBinBlkMain.IsUseBinMapBondArea(GetBTBlock()))
	{
		ulFirstDieIndex	= 1 + m_oBinBlkMain.GrabSkipUnit(GetBTBlock());			//v4.47T9
		ULONG ulTempRow = 0, ulTempCol = 0;
		m_oBinBlkMain.GetDieIndexInBinMap(GetBTBlock(), ulFirstDieIndex, ulTempRow, ulTempCol);	
	}

	//Last-Die Index on first row
	ULONG ulNoOf1stRowDices = 0;	
	ULONG ulLastDieIndex	= m_oBinBlkMain.GrabNoOfDiePerRow(GetBTBlock()) - 1;	//v4.51A5	
	if (m_oBinBlkMain.IsUseBinMapBondArea(GetBTBlock()))
	{
		ULONG ulLastDieRow = 0, ulLastDieCol = 0;
		m_oBinBlkMain.GetLastDieIndexIn1stBMapRow(GetBTBlock(), ulNoOf1stRowDices, ulLastDieRow, ulLastDieCol);
		ulLastDieIndex = ulFirstDieIndex + ulNoOf1stRowDices - 2;	// - 1;			//v4.47T10
	}


	// Log Encoder value if Bond Index matched
	CString szLog;
	BOOL bNeedUpdateInNextCycle = FALSE;
	LONG lBinIndex = m_oBinBlkMain.GrabNVCurrBondIndex(GetBTBlock());
	//if (lBinIndex == ulFirstDieIndex - 1)				//v4.44T4
	if (lBinIndex == ulFirstDieIndex)					//v4.45T3
	{
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"] = TRUE;
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"] = lEncX;
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"] = lEncY;
		szLog.Format("AutoCalAngle on Bin #%ld: Enc_A(%ld, %ld), 1st Index = %lu, 2nd Index = %lu (%lu)\n", 
			GetBTBlock(), lEncX, lEncY, ulFirstDieIndex, ulLastDieIndex, ulNoOf1stRowDices);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}
	else if (lBinIndex < (long)ulLastDieIndex)
	{
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Enable"]  = TRUE;		
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["X"] = lEncX;
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Y"] = lEncY;
	}
	//else if ( lBinIndex == (ulLastDieIndex - 2) )		//v4.44T4
	else if ( lBinIndex == (ulLastDieIndex) )		//v4.45T3
	{
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Enable"] = TRUE;
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["X"] = lEncX;
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Y"] = lEncY;
		bNeedUpdateInNextCycle = TRUE;
		szLog.Format("AutoCalAngle on Bin #%ld: Enc_B(%ld, %ld), 1st Index = %lu, 2nd Index = %lu\n", 
			GetBTBlock(), lEncX, lEncY, ulFirstDieIndex, ulLastDieIndex);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}
	else if (lBinIndex > (long)ulLastDieIndex)
	{
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= FALSE;
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Enable"]	= FALSE;
		(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Enable"]		= FALSE;
	}

//szLog.Format("AutoCalAngle on Bin #%ld: Index = %lu (%lu, %lu)", 
//	GetBTBlock(), lBinIndex, ulFirstDieIndex, ulLastDieIndex);
//CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog, "a+");

	if (m_bIfNeedUpdateBinFrameAngle)
	{
		DOUBLE dNewAngle = 0;

		LONG lDieAPhysical_x = (*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"];
		LONG lDieAPhysical_y = (*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"];
		LONG lDieBPhysical_x = (*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["X"];
		LONG lDieBPhysical_y = (*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Y"];

		LONG lXDistance	= ConvertXEncoderValueToFileUnit(lDieBPhysical_x - lDieAPhysical_x);
		LONG lYDistance	= ConvertYEncoderValueToFileUnit(lDieBPhysical_y - lDieAPhysical_y);
		if (lXDistance != 0)
			dNewAngle	= atan(((float) lYDistance) / ((float) lXDistance));

		if ( (lDieAPhysical_x == 0) || (lDieAPhysical_y == 0) || 
			 (lDieBPhysical_x == 0) || (lDieBPhysical_y == 0) )
		{
			szLog.Format("AutoCalAngle on Bin #%ld FAIL: TLH A(%ld, %ld); B(%ld, %ld); NewAngle = %.2f (%.2f deg) (%ld, %ld)\n", 
							GetBTBlock(), lDieAPhysical_x, lDieAPhysical_y, lDieBPhysical_x, lDieBPhysical_y, 
							0, 0, 0, 0);
			CMSLogFileUtility::Instance()->BT_TableIndexLog("");
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

			m_bIfNeedUpdateBinFrameAngle = FALSE;
			(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= FALSE;
			(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Enable"]	= FALSE;
			(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Enable"]		= FALSE;
		}
		else
		{
			m_oBinBlkMain.SetIsAligned(GetBTBlock(), TRUE);					//v4.44A2
			//m_oBinBlkMain.SetXOffset(ulBlkInUse, lNewXOffset);
			//m_oBinBlkMain.SetYOffset(ulBlkInUse, lNewYOffset);
			m_oBinBlkMain.SetRotateAngleX(GetBTBlock(), dNewAngle);
			m_oBinBlkMain.m_lEnableLogCount	= 20;	//log 20 die pos only!!		//v4.44T8
			m_oBinBlkMain.m_bEnableLogging = TRUE;								//v4.44T8

			szLog.Format("AutoCalAngle on Bin #%ld: TLH  A(%ld, %ld); B(%ld, %ld); NewAngle = %.2f (%.2f deg) (%ld, %ld)\n", 
						GetBTBlock(), lDieAPhysical_x, lDieAPhysical_y, lDieBPhysical_x, lDieBPhysical_y, 
						dNewAngle, (dNewAngle * 180.0 / PI), lXDistance, lYDistance);
			CMSLogFileUtility::Instance()->BT_TableIndexLog("");
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

			m_bIfNeedUpdateBinFrameAngle = FALSE;
			(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= FALSE;
			(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Enable"]	= FALSE;
			(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Enable"]		= FALSE;
		}
	}

	if (bNeedUpdateInNextCycle)
	{
		//Next cycle when visiting this fcn again, it will enter above IF loop to
		// calculate the frame angle;
		m_bIfNeedUpdateBinFrameAngle = TRUE;
	}

	return TRUE;
}

BOOL CBinTable::OpPerformUplookBTCompensation()
{
	//m_lBTCompCounter++;

	CString szLog;
	CString szLog2;
	CString szLog3;

	szLog.Format("OpPerformUplookBTComp - %d", m_lBTCompCounter);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	LONG lDx = 0;
	LONG lDy = 0;


	GetEncoderValue();
	LONG lOldEncX = m_lEnc_X;
	LONG lOldEncY = m_lEnc_Y;
	LONG lOldCmdX = m_lCmd_X;
	LONG lOldCmdY = m_lCmd_Y;

	lDx = (*m_psmfSRam)["BinTable"]["UplookCompX"];
	lDy	= (*m_psmfSRam)["BinTable"]["UplookCompY"];

	lDx = 0 - lDx;		//csp005
	lDy = 0 - lDy;

	BOOL bIsLFPadFound = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"];

	//CSP
	if (m_bPrPreBondAlignment)	// && !bIsLFPadFound)
	{
		// These 2 parameters are updated/calculated in OpPrPreBondAlignPad();
		//m_lPreBondTableOffsetX;
		//m_lPreBondTableOffsetY;

		//Runtime Die&Pad Offset XY
		LONG lAvgRTX1 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X1"];
		LONG lAvgRTY1 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y1"];
		LONG lAvgRTX2 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X2"];
		LONG lAvgRTY2 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y2"];

		LONG lTableOffsetX = 0;
		LONG lTableOffsetY = 0;
		LONG lRTOffsetX = 0;
		LONG lRTOffsetY = 0;

		BOOL bBTAtBH1Pos	= (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];
		if (bBTAtBH1Pos)
		{
			lTableOffsetX	= m_lCollet1PadOffsetX;
			lTableOffsetY	= m_lCollet1PadOffsetY;
			lRTOffsetX		= lAvgRTX1;
			lRTOffsetY		= lAvgRTY1;
		}
		else
		{
			lTableOffsetX	= m_lCollet2PadOffsetX;
			lTableOffsetY	= m_lCollet2PadOffsetY;
			lRTOffsetX		= lAvgRTX2;
			lRTOffsetY		= lAvgRTY2;
		}

		LONG lOldDx = lDx;
		LONG lOldDy = lDy;

		if (!bIsLFPadFound)
		{
			//v4.59A22	//David Ma
			DOUBLE dThermalDX = m_oBinBlkMain.FindThermalDeltaPitchX(GetBTBlock(), m_lCol);
			DOUBLE dThermalDY = m_oBinBlkMain.FindThermalDeltaPitchX(GetBTBlock(), m_lRow);
			LONG lThermalDX = ConvertFileUnitToXEncoderValue(dThermalDX);
			LONG lThermalDY = ConvertFileUnitToYEncoderValue(dThermalDY);

			lDx = lDx + m_lPreBondTableOffsetX + lTableOffsetX + lThermalDX;	// - lRTOffsetX;
			lDy = lDy + m_lPreBondTableOffsetY + lTableOffsetY + lThermalDY;	// - lRTOffsetY;

			szLog.Format("BT: UplookComp PreBond - Enc(%ld, %ld); Cmd(%ld, %ld); UplookComp OffsetXY +(%ld, %ld); PreBondPad OffsetXY +(%ld, %ld); TableOffsetXY +(%ld, %ld); RTOffset -(%ld, %ld); ThermalD +(%ld, %ld), Final-MOVE (%ld, %ld)",
							lOldEncX, lOldEncY, lOldCmdX, lOldCmdY,
							lOldDx, lOldDy, 
							m_lPreBondTableOffsetX, m_lPreBondTableOffsetY,
							lTableOffsetX, lTableOffsetY, 
							lRTOffsetX, lRTOffsetY,
							lThermalDX, lThermalDY,		//v4.59A22	//David Ma
							lDx, lDy);
		}
		else
		{
			lDx = lDx + lTableOffsetX;		// - lRTOffsetX;
			lDy = lDy + lTableOffsetY;		// - lRTOffsetY;

			szLog.Format("BT: UplookComp PreBondLF - Enc(%ld, %ld); Cmd(%ld, %ld); UplookComp OffsetXY +(%ld, %ld); PreBondPad OffsetXY +(%ld, %ld); TableOffsetXY +(%ld, %ld); RTOffset -(%ld, %ld); Final-MOVE (%ld, %ld)",
							lOldEncX, lOldEncY, lOldCmdX, lOldCmdY,
							lOldDx, lOldDy, 
							0, 0,
							lTableOffsetX, lTableOffsetY, 
							lRTOffsetX, lRTOffsetY,
							lDx, lDy);
		}

		//andrewng12
		//szLog3.Format("%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,", 
		//					lOldDx, lOldDy, lTableOffsetX, lTableOffsetY, 
		//					lAvgRTX1, lAvgRTY1, lAvgRTX2, lAvgRTY2);

	}
	else
	{
		szLog.Format("BT: Uplook NO-PreBond - Enc(%ld, %ld); UplookComp OffsetXY (%ld, %ld)",
						lOldEncX, lOldEncY, lDx, lDy);

		//andrewng12
		//szLog3.Format("%ld,%ld,0,0,0,0,0,0,", lDx, lDy);		

	}

/*
	//andrewng
//	if (m_bPrPreBondAlignment)	// && !bIsLFPadFound)
//	{
		//// These 2 parameters are updated/calculated in OpPrPreBondAlignPad();
		////m_lPreBondTableOffsetX;
		////m_lPreBondTableOffsetY;

		////Runtime Die&Pad Offset XY
		//LONG lAvgRTX1 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X1"];
		//LONG lAvgRTY1 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y1"];
		//LONG lAvgRTX2 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X2"];
		//LONG lAvgRTY2 = (LONG)(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y2"];

		//LONG lTableOffsetX = 0;
		//LONG lTableOffsetY = 0;
		//LONG lRTOffsetX = 0;
		//LONG lRTOffsetY = 0;

		//BOOL bBTAtBH1Pos	= (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["BinTable At BH1"];
		//if (bBTAtBH1Pos)
		//{
		//	lTableOffsetX	= m_lCollet1PadOffsetX;
		//	lTableOffsetY	= m_lCollet1PadOffsetY;
		//	lRTOffsetX		= lAvgRTX1;
		//	lRTOffsetY		= lAvgRTY1;
		//}
		//else
		//{
		//	lTableOffsetX	= m_lCollet2PadOffsetX;
		//	lTableOffsetY	= m_lCollet2PadOffsetY;
		//	lRTOffsetX		= lAvgRTX2;
		//	lRTOffsetY		= lAvgRTY2;
		//}

		//LONG lOldDx = lDx;
		//LONG lOldDy = lDy;

		//if (!bIsLFPadFound)
		//{
		//	lDx = lDx + m_lPreBondTableOffsetX + lTableOffsetX;	// - lRTOffsetX;
		//	lDy = lDy + m_lPreBondTableOffsetY + lTableOffsetY;	// - lRTOffsetY;

		//	szLog.Format("BT: UplookComp PreBond - Enc(%ld, %ld); Cmd(%ld, %ld); UplookComp OffsetXY +(%ld, %ld); PreBondPad OffsetXY +(%ld, %ld); TableOffsetXY +(%ld, %ld); RTOffset -(%ld, %ld); Final-MOVE (%ld, %ld)",
		//					lOldEncX, lOldEncY, lOldCmdX, lOldCmdY,
		//					lOldDx, lOldDy, 
		//					m_lPreBondTableOffsetX, m_lPreBondTableOffsetY,
		//					lTableOffsetX, lTableOffsetY, 
		//					lRTOffsetX, lRTOffsetY,
		//					lDx, lDy);
		//}
		//else
		//{
		//	lDx = lDx + lTableOffsetX;		// - lRTOffsetX;
		//	lDy = lDy + lTableOffsetY;		// - lRTOffsetY;

		//	szLog.Format("BT: UplookComp PreBondLF - Enc(%ld, %ld); Cmd(%ld, %ld); UplookComp OffsetXY +(%ld, %ld); PreBondPad OffsetXY +(%ld, %ld); TableOffsetXY +(%ld, %ld); RTOffset -(%ld, %ld); Final-MOVE (%ld, %ld)",
		//					lOldEncX, lOldEncY, lOldCmdX, lOldCmdY,
		//					lOldDx, lOldDy, 
		//					0, 0,
		//					lTableOffsetX, lTableOffsetY, 
		//					lRTOffsetX, lRTOffsetY,
		//					lDx, lDy);
		//}

		////andrewng12
		//szLog3.Format("%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,", 
		//					lOldDx, lOldDy, lTableOffsetX, lTableOffsetY, 
		//					lAvgRTX1, lAvgRTY1, lAvgRTX2, lAvgRTY2);

//	}
//	else
//	{
		szLog.Format("BT: Uplook NO-PreBond - Enc(%ld, %ld); UplookComp OffsetXY (%ld, %ld)",
						lOldEncX, lOldEncY, lDx, lDy);

		szLog3.Format("%ld,%ld,0,0,0,0,0,0,", lDx, lDy);		

//	}
		
*/

	//X_Profile(LOW_PROF);
	//Y_Profile(LOW_PROF);
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	X_Move(lDx, SFM_NOWAIT);
	Y_Move(lDy, SFM_WAIT);
	X_Sync();
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);

	Sleep(10);
	GetEncoderValue();
	
	szLog2.Format("; Final-Enc(%ld, %ld); Final-Cmd(%ld, %ld)", 
		m_lEnc_X, m_lEnc_Y, m_lCmd_X, m_lCmd_Y);

	szLog = szLog + szLog2;
	CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

	//CMSLogFileUtility::Instance()->BT_TableIndex2Log(szLog3);		//andrewng12

	//Reset current cycle results
	(*m_psmfSRam)["BinTable"]["LFBondPad"]["IsFound"] = FALSE;		//andrewng1
	m_lPreBondTableOffsetX = 0;
	m_lPreBondTableOffsetY = 0;

	SetBTCompensate(FALSE);
	return TRUE;
}


////////////////////////////////////////////
//	Check hardware ready 
////////////////////////////////////////////
BOOL CBinTable::IsMotorXPowerOn()
{
	if (!X_IsPowerOn())
	{
		CString szText;
		szText = "\nBinTable X is not power ON";
		SetStatusMessage("BinTable X NOT power ON yet");
		SetErrorMessage("BinTable X NOT power ON yet");
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		return FALSE;
	}

	return TRUE;
}

BOOL CBinTable::IsMotorYPowerOn()
{
	if (!Y_IsPowerOn())
	{
		CString szText;
		szText = "\nBinTable Y is not power ON";
		SetStatusMessage("BinTable Y NOT power ON yet");
		SetErrorMessage("BinTable Y NOT power ON yet");
		SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		return FALSE;
	}

	return TRUE;
}


BOOL CBinTable::IsMotorXYReady()
{
	CString szText;
	//Power On/Off
	if (!IsMotorXPowerOn())
	{
		return FALSE;
	}

	if (!IsMotorYPowerOn())
	{
		return FALSE;
	}

	BOOL bXResult = CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
	BOOL bYResult = CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
	
	if( (bXResult == FALSE) || (bYResult == FALSE) || !m_bHome_X || !m_bHome_Y )
	{
		SetMotionCE(TRUE, "Bin Table Not Home(IsMotionHardwareReady)");

		if ( !m_bHome_X || !bXResult )
		{
			szText = "\nBinTableX";
			SetStatusMessage("BinTableX NOT home yet");
			SetErrorMessage("BinTableX NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		}
		if ( !m_bHome_Y || !bYResult )
		{
			szText = "\nBinTableY";
			SetStatusMessage("BinTableY NOT home yet");
			SetErrorMessage("BinTableY NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		}
		return FALSE;
	}

	return TRUE;
}


BOOL CBinTable::IsMotionHardwareReady()	//	HuaMao2
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBT)	//v3.60
		return TRUE;

	CString szText;
	//Power On/Off
	if (!IsMotorXPowerOn())
	{
		return FALSE;
	}

	if (!IsMotorYPowerOn())
	{
		return FALSE;
	}

	BOOL bXResult = CMS896AStn::MotionCheckResult(BT_AXIS_X, &m_stBTAxis_X);
	BOOL bYResult = CMS896AStn::MotionCheckResult(BT_AXIS_Y, &m_stBTAxis_Y);
	
	if( (bXResult == FALSE) || (bYResult == FALSE) || !m_bHome_X || !m_bHome_Y )
	{
		SetMotionCE(TRUE, "Bin Table Not Home(IsMotionHardwareReady)");

		if ( !m_bHome_X || !bXResult )
		{
			szText = "\nBinTableX";
			SetStatusMessage("BinTableX NOT home yet");
			SetErrorMessage("BinTableX NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		}
		if ( !m_bHome_Y || !bYResult )
		{
			szText = "\nBinTableY";
			SetStatusMessage("BinTableY NOT home yet");
			SetErrorMessage("BinTableY NOT home yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
		}
		return FALSE;
	}

	if (m_bEnable_T)	//v4.59A42
	{
		if (!T_IsPowerOn())
		{
			szText = "\nBinTable T is not power ON";
			SetStatusMessage("BinTableT NOT power ON yet");
			SetErrorMessage("BinTableT NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if (CMS896AStn::MotionReportErrorStatus(BT_AXIS_T, &m_stBTAxis_T) != 0)
		{
			szText = "\nBinTable T NUMotion Error !!";
			SetStatusMessage("BinTableT NUMotion Error");
			SetErrorMessage("BinTableT NUMotion Error");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}
	}

	return TRUE;
}


ULONG CBinTable::OpGetRegionGradeDieNum(UCHAR ucNextGrade)
{
/*
	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();

	ULONG ulULRow = 0, ulULCol = 0, ulLRRow = 0, ulLRCol = 0;
	ULONG ulRow, ulCol;
	ULONG ulDieCount = 0;
	CString szMsg;
	if( pSRInfo->GetRegion(ulTgtRegion, ulULRow, ulULCol, ulLRRow, ulLRCol)==FALSE )
	{
		szMsg.Format("REGN peek get target wrong %d", ulTgtRegion);
		pUtl->RegionOrderLog(szMsg);
		return ulDieCount;
	}

	for(ulRow=ulULRow; ulRow<ulLRRow; ulRow++)
	{
		for (ulCol=ulULCol; ulCol<ulLRCol; ulCol++)
		{
			if (!m_WaferMapWrapper.GetReader()->IsMissingDie(ulRow, ulCol))
			{
				if (GetMapGrade(ulRow, ulCol) == ucNextGrade)
				{
					ulDieCount++;
				}
			}
		}
	}
*/
	ULONG ulDieCount = WM_CWaferMap::Instance()->GetRegionStatistics(ucNextGrade);
	return ulDieCount;
}


BOOL CBinTable::OpGetRegionNextGrade(CONST INT iIndex, CUIntArray &aulSortGradeList, UCHAR &ucNextGrade)
{
	ULONG ulGradeLeft;
	INT j;

	for(j=iIndex; j<aulSortGradeList.GetSize(); j++)
	{
		// if this grade has die in current region return TRUE
		ulGradeLeft = OpGetRegionGradeDieNum(aulSortGradeList[j]);
		if( ulGradeLeft>0 )
		{
			ucNextGrade = aulSortGradeList[j] - m_WaferMapWrapper.GetGradeOffset();
			return TRUE;
		}
	}

	return FALSE;
}

//v4.42T17
INT CBinTable::OpIndex_MS109_AfterChangeGrade(BOOL bLogMsg)
{
	INT		nResult	= gnOK;
	CString	szTemp;
	
	// Get the current encoder count
	GetEncoderValue();
	if (m_nBTInUse == 1)	//If BT2	//v4.20
	{
		m_lLastX2 = m_lEnc_X2;
		m_lLastY2 = m_lEnc_Y2;
	}
	else
	{
		m_lLastX = m_lEnc_X;
		m_lLastY = m_lEnc_Y;
	}

	// Get the X, Y position
	LONG lBpX = m_lX;
	LONG lBpY = m_lY;
	LONG lSortDir = 0;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN

	DOUBLE dPosX=0, dPosY=0;

	//2018.1.18
	UpdateWTGradefromBinLoader();

	m_ulBondedBlk = GrabBondXYPosn(GetWTGrade(), dPosX, dPosY, m_lRow, m_lCol, lSortDir);
	if ((GetBTBlock() == 0) || (GetBTBlock() == Err_BinBlockNotAssign))
	{
		return gnNOTOK;
	}

	//generate the random hole die index when the binblk in used is empty			//v3.33T3
	if (m_bEnable2DBarcodeOutput)										
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(GetBTBlock()) == 0)
		{
			m_oBinBlkMain.GenRandomHole(GetBTBlock());
		}
	}


	ULONG ulTemp = (*m_psmfSRam)["BinTable"]["BlkInUse"];
	(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= ulTemp;
	(*m_psmfSRam)["BinTable"]["BlkInUse"]		= GetBTBlock();
	(*m_psmfSRam)["BinTable"]["BondingGrade"]	= GetWTGrade();
	(*m_psmfSRam)["BinTable"]["SortDir"]		= lSortDir;		//CSP

	m_lX = ConvertFileUnitToXEncoderValue(dPosX);
	m_lY = ConvertFileUnitToYEncoderValue(dPosY);

	return nResult;
}

BOOL CBinTable::LetWaferTableDoAdvSampling(LONG lTgtGrade)
{
#ifdef NU_MOTION
	if( m_lWftAdvSamplingStage!=0 )
	{
		return TRUE;
	}

	DisplaySequence("BT - BL CHANGE frame Let WFT adv sample");
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(LONG), &lTgtGrade);

	m_lWftAdvSamplingStage	= 1;
	m_comClient.SendRequest(WAFER_TABLE_STN, _T("WaferTableDoAdvSampling"), stMsg);

#endif

	return TRUE;
}
INT CBinTable::OpOsramIndex()
{
	CString szLog;
	szLog.Format("binmixcount:%d",m_lBinMixCount);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
	m_lBinMixCount ++;
	LONG lPrescanGoodDie = 0; 
	lPrescanGoodDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];

	szLog.Format("LOG,lPrescanGoodDie:%d,TypeAQty:%d,TypeBQty:%d,m_lBinMixCount:%d,currentgrade:%d,order:%d",
		lPrescanGoodDie,m_lBinMixTypeAQty,m_lBinMixTypeBQty,m_lBinMixCount,m_ucBinMixCurrentGrade,m_bBinMixOrder);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);

	if (m_ucBinMixCurrentGrade == m_ucBinMixTypeAGrade)
	{
		if( (m_lBinMixCount == (lPrescanGoodDie - (m_lBinMixTypeAStopPoint + 1))) && (m_lBinMixTypeAStopPoint != 0 )&& (m_bBinMixOrder != 1))
		{
			szLog.Format("first loop stop pointA:index:%d,m_lBinMixSetQty:%d,m_lBinMixTypeAStopPoint:%d,m_lBinMixTypeAQty:%d",m_lBinMixCount,m_lBinMixSetQty,m_lBinMixTypeAStopPoint,m_lBinMixTypeAQty);
			CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
			(*m_psmfSRam)["BinTable"]["NextBinFull"] = TRUE;
			m_lBinMixTypeAQty = lPrescanGoodDie - m_lBinMixCount;
			m_lBinMixSetQty = 1;
			OpBackupMap();
		}

		if ((m_lBinMixSetQty == 1) ||( m_lBinMixSetQty == 2))
		{
			m_lBinMixTypeAQty =	m_lBinMixTypeAStopPoint ;							

			szLog.Format("indexA:%d,m_lBinMixSetQty:%d,m_lBinMixTypeAStopPoint:%d,m_lBinMixTypeAQty:%d",m_lBinMixCount,m_lBinMixSetQty,m_lBinMixTypeAStopPoint,m_lBinMixTypeAQty);
			CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
			if(m_lBinMixSetQty == 1)
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("m_lBinMixSetQty == 1");	
				m_lBinMixSetQty = 2;
			}
			else
			{
				//m_lBinMixCount = m_lBinMixCount - 1;
				m_lBinMixTypeAStopPoint = 0;
				CMSLogFileUtility::Instance()->BL_LogStatus("m_lBinMixSetQty == 2");
				if (m_lBinMixLessCommonType == 1)
				{
					HmiMessage("trigger stop point!Please check the quantity on input wafer!");
				}
				else
				{
					HmiMessage("trigger stop point! Please unload the output bin frame and clear bin couter manually!");
				}
				m_lBinMixSetQty =0;
			}
		}
		else
		{
			m_lBinMixTypeAQty = lPrescanGoodDie - m_lBinMixCount;
		}
	}
	else if( m_ucBinMixCurrentGrade == m_ucBinMixTypeBGrade)
	{
		if(( m_lBinMixCount == (lPrescanGoodDie - (m_lBinMixTypeBStopPoint + 1)))&& (m_lBinMixTypeBStopPoint != 0 ) && (m_bBinMixOrder != 1))
		{
			szLog.Format("first loop stop pointB:index:%d,m_lBinMixSetQty:%d,m_lBinMixTypeBStopPoint:%d,m_lBinMixTypeBQty:%d",m_lBinMixCount,m_lBinMixSetQty,m_lBinMixTypeBStopPoint,m_lBinMixTypeBQty);
			CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
			(*m_psmfSRam)["BinTable"]["NextBinFull"] = TRUE;
			m_lBinMixTypeBQty = lPrescanGoodDie - m_lBinMixCount;
			m_lBinMixSetQty = 1;
			OpBackupMap();
		}

		if ((m_lBinMixSetQty == 1) ||( m_lBinMixSetQty == 2))
		{
			m_lBinMixTypeBQty =	m_lBinMixTypeBStopPoint ;
			szLog.Format("indexB:%d,m_lBinMixSetQty:%d,m_lBinMixTypeBStopPoint:%d,m_lBinMixTypeBQty:%d",m_lBinMixCount,m_lBinMixSetQty,m_lBinMixTypeBStopPoint,m_lBinMixTypeBQty);
			CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
			if(m_lBinMixSetQty == 1)
			{
				CMSLogFileUtility::Instance()->BL_LogStatus("m_lBinMixSetQty == 1");
				m_lBinMixSetQty = 2;
			}
			else
			{
				//m_lBinMixCount = m_lBinMixCount - 1;
				m_lBinMixTypeBStopPoint = 0;
				CMSLogFileUtility::Instance()->BL_LogStatus("m_lBinMixSetQty == 2");
				if (m_lBinMixLessCommonType == 2)
				{
					HmiMessage("trigger stop point!Please check the quantity on input wafer!");
				}
				else
				{
					HmiMessage("trigger stop point! Please unload the output bin frame and clear bin couter manually!");
				}
				m_lBinMixSetQty =0;
			}
		}
		else
		{
			m_lBinMixTypeBQty = lPrescanGoodDie - m_lBinMixCount;
		}
	}
	CString szMsg;
	szMsg.Format("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA,binmixcounttemp:%d,count:%d",m_lBinMixCountTemp,m_lBinMixCount);
	CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);
	m_lBinMixCountTemp = m_lBinMixCount;

	return TRUE;
}

INT CBinTable::OpOsramNotIndex()
{
    CString szMsg;
	szMsg.Format("binfull,binmixcounttemp:%d,count:%d",m_lBinMixCountTemp,m_lBinMixCount);
	CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);
	m_lBinMixCountTemp = m_lBinMixCount;
	m_lBinMixCount = 0;
	BOOL bBinMixNeedUnload = 0;
	BOOL bBinMixWaferUnload = 1;

	if( m_bBinMixOrder == FALSE)
	{
		m_bBinMixOrder = 1;
	}
	else
	{
		m_bBinMixOrder = 0;
		bBinMixNeedUnload = 1;//Unload in Idle state
	}
	(*m_psmfSRam)["BinOutputFile"]["Osram Bin Mix Map Need Unload"] = bBinMixNeedUnload;
	(*m_psmfSRam)["BinOutputFile"]["Osram Wafer Unload"] = bBinMixWaferUnload;
	OpBackupMap();
	HmiMessage("Bin Mix Map: Bin Full, Machine Stop!");
	m_qSubOperation	= HOUSE_KEEPING_Q;
	//(*m_psmfSRam)["BinTable"]["NextBinFull"] = TRUE;
	return TRUE;
}

INT CBinTable::OpUpdateOsramStopPoint()
{
	LONG lCount = m_lBinMixCountTemp;
	LONG lPrescanGoodDie	= (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];
	m_lBinMixCountTemp = 0;

	CString szLog;
	szLog.Format("StopPoint Update,last Count:%d,Current Count:%d,Pattern:%d,LessCommonType:%d,PrescanGoodDie:%d",lCount,m_lBinMixCount,m_ulBinMixPatternType,m_lBinMixLessCommonType,lPrescanGoodDie);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
	if (lCount != 0 && (m_lBinMixCount == 0 )/*|| bOsramLoadReset)*/ /*|| m_lBinMixCount == 1 )*/)
	{
		//(*m_psmfSRam)["WaferTable"]["OsramLoadReset"] = 0;
		m_lBinMixCount = 0;
		switch (m_ulBinMixPatternType)
		{
		case 1:		//ABAB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 1;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount / 1;
			}
			break;
		case 2:		//AABAAB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 2;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount * 2;
			}


			break;
		case 3:		//AAABAAAB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 3;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount / 4;
			}

			break;
		case 4:		//AAAABAAAAB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 4;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount * 4;
			}

			break;
		case 5:		//AABBAABB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 2 * 2;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount / 2 * 2;
			}


			break;
		case 6:		//ABBABB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 1 * 2;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount / 2 * 1;
			}


			break;
		case 7:		//ABBBABBB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 1 * 3;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount / 3 * 1;
			}

			break;
		case 8:		//ABBBBABBBB
			if (m_lBinMixLessCommonType == 1)
			{
				m_lBinMixTypeBStopPoint = lPrescanGoodDie - lCount / 1 * 4;
			}
			else
			{
				m_lBinMixTypeAStopPoint = lPrescanGoodDie - lCount / 4 * 1;
			}

			break;
		default:
			HmiMessage("BinBix Pattern = NONE");

		}
	}
	return TRUE;
}

INT CBinTable::OpSemitekCheckEmptyFrame(CONST ULONG ulBlkInUse, CString szBarcode)
{
	//Semitek Generate Empty File
	//ULONG ulBlkInUseTemp = (*m_psmfSRam)["BinTable"]["BlkInUse"];
	ULONG ulBondedDie    = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBlkInUse);
	ULONG ulBondedIndex  = m_oBinBlkMain.GrabNVCurrBondIndex(ulBlkInUse);
	int nSerial = m_oBinBlkMain.GrabSerialNo(ulBlkInUse);

	CString szLogSemitek;
	szLogSemitek.Format("ulBlkInUseTemp:%d,ulBondedDie:%d,index:%d, Serial: %d",ulBlkInUse,ulBondedDie,ulBondedIndex, nSerial);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLogSemitek);
	//
	//if (ulBondedIndex == 1)
	//{
		m_oSemitekSubSystem.GenerateEmptyFrameFile(ulBlkInUse,ulBondedDie,nSerial, szBarcode);
	//}
	return 1;
}

VOID CBinTable::GetWafflePadSettings()
{
	CString szLogFile = "C:\\MapSorter\\UserData\\History\\WafflePad.txt";
	DeleteFile(szLogFile);
	CMS896AStn::m_bEnableWafflePadBonding = FALSE;
	for(ULONG ulBinBlk=1; ulBinBlk<=MS_MAX_BIN; ulBinBlk++)
	{
		if (m_oBinBlkMain.GetIsWafflePadEnable(ulBinBlk) == TRUE)
		{
			CMS896AStn::m_bEnableWafflePadBonding = TRUE;
			break;		//v4.22T8
		}
	}

	if (!CMS896AStn::m_bEnableWafflePadBonding)
	{
		return;
	}

	CString szLog;
	LONG lPadPitchX = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabWafflePadDistX(1));	//	pad pitch x
	LONG lPadPitchY = ConvertFileUnitToXEncoderValue(m_oBinBlkMain.GrabWafflePadDistY(1));	//	pad pitch y
	szLog.Format("Pad Pitch X and Y (%ld,%ld)", lPadPitchX, lPadPitchY);
	SetAlarmLog("");
	SetAlarmLog(szLog);

	LONG lPadDimCol = m_oBinBlkMain.GrabWafflePadSizeX(1);	//	row in pad
	LONG lPadDimRow = m_oBinBlkMain.GrabWafflePadSizeY(1);	//	col in pad
	LONG lPadTotal  = lPadDimCol * lPadDimRow;
	m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1);
	m_ulDiePerCol = m_oBinBlkMain.GrabNoOfDiePerCol(1);
	szLog.Format("Pad Dimension Col and Row (%ld,%ld)(%d), block (%ld,%ld)",
		lPadDimCol, lPadDimRow, lPadTotal, m_ulDiePerRow, m_ulDiePerCol);
	SetAlarmLog(szLog);

	LONG lPadsNoInX = 1;
	if (lPadDimCol != 0)
	{
		lPadsNoInX =  m_ulDiePerRow / lPadDimCol;
	}
	LONG lPadsNoInY = 1;
	if (lPadDimRow != 0)
	{
		lPadsNoInY = m_ulDiePerCol / lPadDimRow;
	}
	szLog.Format("Block Pads in Row and Col (%ld,%ld)", lPadsNoInY, lPadsNoInX);
	SetAlarmLog(szLog);

	szLogFile = "C:\\MapSorter\\UserData\\History\\WafflePad000.txt";
	DeleteFile(szLogFile);
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szLogFile, "wt");
	DOUBLE dULX = 0, dULY = 0, dLRX = 0, dLRY = 0;
	LONG lULX = 0, lULY = 0, lLRX = 0, lLRY = 0;
	LONG lPadIndexLR = lPadTotal - 1;
	if (lPadDimRow%2 == 0)
	{
		lPadIndexLR = lPadTotal - lPadDimCol;
	}

	for(ULONG ulY = 0; ulY < (ULONG)lPadsNoInY; ulY++)
	{
		for(ULONG ulX=0; ulX < (ULONG)lPadsNoInX; ulX++)
		{
			ULONG ulIndexUL = (ulY*lPadsNoInX + ulX)*lPadTotal + 1;
			m_oBinBlkMain.GetIndexMovePosnXY(1, ulIndexUL, dULX, dULY);
			ULONG ulIndexLR = ulIndexUL + lPadIndexLR;
			m_oBinBlkMain.GetIndexMovePosnXY(1, ulIndexLR, dLRX, dLRY);
			lULX = ConvertFileUnitToXEncoderValue(dULX);
			lULY = ConvertFileUnitToYEncoderValue(dULY);
			lLRX = ConvertFileUnitToXEncoderValue(dLRX);
			lLRY = ConvertFileUnitToYEncoderValue(dLRY);
			szLog.Format("Pads %lu==>%lu area %ld,%ld,%ld,%ld from %f,%f,%f,%f",
				ulIndexUL, ulIndexLR, lULX, lULY, lLRX, lLRY, dULX, dULY, dLRX, dLRY);
			SetAlarmLog(szLog);
			lULX = lULX - lPadPitchX/4;
			lULY = lULY - lPadPitchY/4;
			lLRX = lLRX + lPadPitchX/4;
			lLRY = lLRY + lPadPitchY/4;
			if ((nErr == 0) && (fp != NULL))
			{
				fprintf(fp, "%ld,%ld,%ld,%ld\n", lULX, lULY, lLRX, lLRY);
			}
		}
	}

	if ((nErr == 0) && (fp != NULL))
	{
		fclose(fp);
	}

	if (IsMS90())
	{
		szLogFile = "C:\\MapSorter\\UserData\\History\\WafflePad180.txt";
		DeleteFile(szLogFile);

		errno_t nErr = fopen_s(&fp, szLogFile, "wt");
		//Change from motor steps to um
		LONG lCORX = -1 * ConvertXEncoderValueForDisplay(m_lBinCalibX);
		LONG lCORY = -1 * ConvertYEncoderValueForDisplay(m_lBinCalibY);
		m_oBinBlkMain.SetFrameRotation(1, TRUE, lCORX, lCORY);
		for(ULONG ulY = 0; ulY < (ULONG)lPadsNoInY; ulY++)
		{
			for(ULONG ulX=0; ulX < (ULONG)lPadsNoInX; ulX++)
			{
				ULONG ulIndexUL = (ulY*lPadsNoInX + ulX)*lPadTotal + 1;
				m_oBinBlkMain.GetIndexMovePosnXY(1, ulIndexUL, dLRX, dLRY);
				ULONG ulIndexLR = ulIndexUL + lPadIndexLR;
				m_oBinBlkMain.GetIndexMovePosnXY(1, ulIndexLR, dULX, dULY);
				lULX = ConvertFileUnitToXEncoderValue(dULX);
				lULY = ConvertFileUnitToYEncoderValue(dULY);
				lLRX = ConvertFileUnitToXEncoderValue(dLRX);
				lLRY = ConvertFileUnitToYEncoderValue(dLRY);
				szLog.Format("Pads %lu==>%lu area %ld,%ld,%ld,%ld from %f,%f,%f,%f",
					ulIndexUL, ulIndexLR, lULX, lULY, lLRX, lLRY, dULX, dULY, dLRX, dLRY);
				SetAlarmLog(szLog);
				lULX = lULX - lPadPitchX/4;
				lULY = lULY - lPadPitchY/4;
				lLRX = lLRX + lPadPitchX/4;
				lLRY = lLRY + lPadPitchY/4;
				if ((nErr == 0) && (fp != NULL))
				{
					fprintf(fp, "%ld,%ld,%ld,%ld\n", lULX, lULY, lLRX, lLRY);
				}
			}
		}

		if ((nErr == 0) && (fp != NULL))
		{
			fclose(fp);
		}
		lCORX = -1 * ConvertXEncoderValueForDisplay(m_lBinCalibX);
		lCORY = -1 * ConvertYEncoderValueForDisplay(m_lBinCalibY);
		m_oBinBlkMain.SetFrameRotation(1, FALSE, lCORX, lCORY);
	}

	IPC_CServiceMessage stMsg;
	INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "SetBondPadsArea", stMsg);
	m_comClient.ScanReplyForConvID(nConvID, 3600000);
	m_comClient.ReadReplyForConvID(nConvID, stMsg);
}

VOID CBinTable::DecodeSubBinSetting()
{
	if (CMS896AApp::m_bEnableSubBin == FALSE)
		return;

	if (_access("c:\\mapsorter\\userdata\\SubBin.txt",0) != -1)
	{
		CStdioFile cfRead;
		CString szLog;
		//LONG lFormatCorrect = 1;
//		int nBlk;
//		int nBlkNo[5];
		if (cfRead.Open("c:\\mapsorter\\userdata\\subbin.txt",CFile::modeRead| CFile::typeText )==TRUE)
		{	
			CString szLine;
			int n;
			BOOL bFormatCorrect = TRUE;
			int nNoOfBlk;
			int nNoOfGrade;
			int nBlk[5];
			int nGrade[5][4];

			//Find Blk & Grade No
			cfRead.ReadString(szLine);
			n = szLine.Find(",");
			if (n != -1)
				nNoOfBlk = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
			else
				HmiMessage("SubBin Setting Wrong1");
			CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Blk No," + szLine.Mid(n+1,1));
			
			cfRead.ReadString(szLine);
			n = szLine.Find(",");
			if (n != -1)
				nNoOfGrade = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
			else
				HmiMessage("SubBin Setting Wrong2");
			CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Grade No," + szLine.Mid(n+1,1));
			
			if (nNoOfBlk > 5)
				HmiMessage("SubBin Setting Wrong,Blk > 5");

			if (nNoOfBlk > 4)
				HmiMessage("SubBin Setting Wrong,Blk > 4");
			m_nNoOfSubBlk = nNoOfBlk;
			m_nNoOfSubGrade = nNoOfGrade;
			//Get each grade
			for (int k = 1; k <= nNoOfBlk ; k ++)
			{
				cfRead.ReadString(szLine);
				n = szLine.Find(",");
				if (n != -1)
					nBlk[k] = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
				szLog.Format("SubBin Blk%d,%d",k,nBlk[k]);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				m_nSubBlk[k] = nBlk[k];
				szLine = szLine.Mid(n+1);
				for (int j = 1; j<= nNoOfGrade; j++)
				{
					if (j == 1)
					{
						n = szLine.Find(",");
						if (n != -1)
							szLine = szLine.Mid(n+1);
						n = szLine.Find(",");
						if (n != -1)
							nGrade[k][j] = atoi((LPCTSTR)(szLine.Mid(n+1,1)));
						CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Grade1," + szLine.Mid(n+1,1));
						szLine = szLine.Mid(n+3);
					}
					else
					{
						n = szLine.Find(",");
						if (n != -1)
							nGrade[k][j] = atoi((LPCTSTR)(szLine.Left(n)));
						else
							nGrade[k][j] = atoi((LPCTSTR)(szLine));
						CMSLogFileUtility::Instance()->MS_LogOperation("SubBin Grade," + szLine.Left(n));
						szLine = szLine.Mid(n+1);	
					}
					m_nSubGrade[k][j] = nGrade[k][j];
				}
			}
		}
	}
	else
	{
		HmiMessage("Cannot Find SubBin Setting File from,c:\\mapsorter\\userdata\\subbin.txt");
	}
}

BOOL CBinTable::UpdateBondPosOffsetAtBPR(BOOL bBH2,int siStepX,int siStepY)
{
	if (bBH2)
	{
		m_lBHZ2BondPosOffsetX = siStepX;
		m_lBHZ2BondPosOffsetY = siStepY;
	}
	else
	{
		m_lBHZ1BondPosOffsetX = siStepX;
		m_lBHZ1BondPosOffsetY = siStepY;
	}
	SaveBinTableData();
	return TRUE;
}

BOOL CBinTable::MoveTableToEmptyPosition()
{
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);

	XY_MoveTo(m_lBTNoDiePosX, m_lBTNoDiePosY, SFM_WAIT);

	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	return TRUE;
}


BOOL CBinTable::IsIdle()
{
	return (State() == IDLE_Q);
}