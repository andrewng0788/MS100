/////////////////////////////////////////////////////////////////
// BPR_Event.cpp : Event functions of the CBondPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wed, March 13, 2013
//	Revision:	1.00
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2013.
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MS896A_Constant.h"
#include "BondPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////
//	Prober Tooling T fcns
////////////////////////////////////////////
VOID CBondPr::RegisterToolingTVariables()
{
	// for prober
	RegVariable(_T("BPR_bDisableTurnOnOffPrLighting"),	&m_bDisableTurnOnOffPrLighting);
	RegVariable(_T("BPR_bIsPowerOn_Tooling_T"),		&m_bIsPowerOn_Tooling_T);
	RegVariable(_T("BPR_bSelTooling_T"),			&m_bSelTooling_T);
	RegVariable(_T("BPR_bIsOpticsPosOn"),			&m_bIsOpticsPosOn);
	RegVariable(_T("BPR_bIsISPPosOn"),				&m_bIsISPPosOn);
	RegVariable(_T("BPR_bIsPDPosOn"),				&m_bIsPDPosOn);
	RegVariable(_T("BPR_bIsISPUp"),					&m_bIsISPUp);
	RegVariable(_T("BPR_bIsISPDown"),				&m_bIsISPDown);
	RegVariable(_T("BPR_ucToolingStageToUse"),		&m_ucToolingStageToUse);
	RegVariable(_T("BPR_lISPDownTimeout"),			&m_lISPDownTimeout);
	RegVariable(_T("BPR_lISPDownSettleDelay"),		&m_lISPDownSettleDelay);
	RegVariable(_T("BPR_szPrevItemName"),			&m_szPrevItemName);
	RegVariable(_T("BPR_szCurrItemName"),			&m_szCurrItemName);
	RegVariable(_T("BPR_szNextItemName"),			&m_szNextItemName);
	RegVariable(_T("BPR_szPSCDisplayCmd"),			&m_szPSCDisplayCmd);
	RegVariable(_T("BPR_szPSCChartTitle"),			&m_szPSCChartTitle);
	RegVariable(_T("BPR_lPSCDataSpinIndex"),		&m_lPSCDataSpinIndex);
	RegVariable(_T("BPR_lPSCDataColumns"),			&m_lPSCDataColumns);
}

VOID CBondPr::RegisterToolingTCommand()
{
	//Prober tooling station
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsAtIspPosition"),				IsAtIspPosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("TurnOnOffPrLightingCmd"),		TurnOnOffPrLightingCmd);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("Diag_PowerOn_ToolingStage"),	Diag_PowerOn_ToolingStage);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectToolingStage"),			SelectToolingStage);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectToolingStageHmi"),		SelectToolingStageHmi);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveToToolingStageToUse"),		MoveToToolingStageToUse);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("MovePlusIspDown"),				MovePlusIspDown);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectToolingStageShortcut"),	SelectToolingStageShortcut);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("IsToolingStageReady"),			IsToolingStageReady);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetISPDownCmd"),				SetISPDownCmd);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetISPDownUpForCleanProbePin"),	SetISPDownUpForCleanProbePin);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadSourceAndProbeData"),	LoadSourceAndProbeData);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrevSPCDataDisplay"),		PrevSPCDataDisplay);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("NextSPCDataDisplay"),		NextSPCDataDisplay);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("IntoPSCDataDisplay"),		IntoPSCDataDisplay);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SpinPSCDataDisplay"),		SpinPSCDataDisplay);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SavePSCDisplayData"),		SavePSCDisplayData);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetSkipToolingStageCheck"),	SetSkipToolingStageCheck);
}

VOID CBondPr::GetAxisInformation()
{
	if (!IsProber())
		return;

	if( IsProberPlus() )
		return ;

	InitAxisData(m_stRotarytTooling_T);
	m_stRotarytTooling_T.m_szName				= BPR_TOOLING_T;			//"ToolingTAxis";
	m_stRotarytTooling_T.m_szTag				= MS_CFG_CH_TOOLING_T;
	m_stRotarytTooling_T.m_ucControlID			= PL_DYNAMIC;				//Use Dynamic as default
	GetAxisData(m_stRotarytTooling_T);

	PrintAxisData(m_stRotarytTooling_T);
}

BOOL CBondPr::ToolingT_IsPowerOn()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (!IsProber())
	{
		return TRUE;
	}

	if( IsProberPlus() )
	{
		return TRUE;
	}

	try
	{
		return CMS896AStn::MotionIsPowerOn(BPR_TOOLING_T, &m_stRotarytTooling_T);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT  CBondPr::ToolingT_PowerOn(BOOL bOn)
{
	INT nResult = gnOK;
	if (m_fHardware && IsProber() && m_bSelTooling_T)
	{
		try
		{
			if ( bOn == TRUE )
			{
				if ( CMS896AStn::MotionIsPowerOn(BPR_TOOLING_T, &m_stRotarytTooling_T) == FALSE )
				{
					CMS896AStn::MotionPowerOn(BPR_TOOLING_T,	&m_stRotarytTooling_T);
					m_bIsPowerOn_Tooling_T = TRUE;
				}
			}
			else
			{
				if ( CMS896AStn::MotionIsPowerOn(BPR_TOOLING_T, &m_stRotarytTooling_T) == TRUE )
				{
					CMS896AStn::MotionPowerOff(BPR_TOOLING_T,	&m_stRotarytTooling_T);
					m_bIsPowerOn_Tooling_T = FALSE;
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BPR_TOOLING_T, &m_stRotarytTooling_T);
			nResult = gnNOTOK;
		}	
	}

	return nResult;
}

BOOL CBondPr::ToolingT_Sync()
{
	BOOL bResult = TRUE;
	if (m_fHardware && IsProber() && m_bSelTooling_T)
	{
		try
		{
			CMS896AStn::MotionSync(BPR_TOOLING_T, 5000, &m_stRotarytTooling_T);		
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			CMS896AStn::MotionCheckResult(BPR_TOOLING_T, &m_stRotarytTooling_T);
			bResult = FALSE;
		}
	}
	return bResult;
}

INT CBondPr::ToolingT_Home()
{
	INT nResult = gnOK;
	
	if (m_fHardware && IsProber() )
	{
		if( m_bSelTooling_T )
			CMS896AStn::MotionPowerOn(BPR_TOOLING_T, &m_stRotarytTooling_T);
		MoveToolingStage(MS_TOOLINGSTAGE_CAM, FALSE);	// home it, to optics
	}

	return nResult;
}

BOOL CBondPr::ToolingT_SearchToOptics(BOOL bWait)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (!IsProber())	
		return FALSE;

	if (m_bSelTooling_T == FALSE)
		return TRUE;

	try
	{
		CString szAwayProfile = BPR_SP_TOOLING_T_OPTICS;
		CString szNearProfile = BPR_SP_TOOLING_T_OPTICS_NEG;
		LONG lDist = -100000, lDDist = 20;
		DOUBLE dSpeed = 1.0;
		if (!IsOpticsPositionOn())
		{
			CMS896AStn::MotionUpdateSearchProfile(BPR_TOOLING_T, szNearProfile, dSpeed, lDist, dSpeed, lDDist, &m_stRotarytTooling_T);
			CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szNearProfile, &m_stRotarytTooling_T);
			CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_WAIT, &m_stRotarytTooling_T, szNearProfile);
			UpdateOutput();
			Sleep(500);
		}
		
//	if (IsOpticsPositionOn())
	{
		CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szAwayProfile, &m_stRotarytTooling_T);
		CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_WAIT, &m_stRotarytTooling_T, szAwayProfile);
		UpdateOutput();
		Sleep(500);
	}
		dSpeed = 0.05;
		CMS896AStn::MotionUpdateSearchProfile(BPR_TOOLING_T, szNearProfile, dSpeed, lDist, dSpeed, lDDist, &m_stRotarytTooling_T);
		CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szNearProfile, &m_stRotarytTooling_T);
		CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_NOWAIT, &m_stRotarytTooling_T, szNearProfile);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	if (bWait == SFM_WAIT)
	{
		ToolingT_Sync();
	}

	m_ucPbtToolingStageInUse = 0;
	return TRUE;
}

BOOL CBondPr::ToolingT_SearchToDetector(BOOL bWait)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (!IsProber())	
		return FALSE;

	if (m_bSelTooling_T == FALSE)
		return TRUE;

	try
	{
		CString szNearProfile = BPR_SP_TOOLING_T_DETECTOR_NEG;
		CString szAwayProfile = BPR_SP_TOOLING_T_DETECTOR;
		LONG lDist = -100000, lDDist = 20;
		DOUBLE dSpeed = 1.0;
		if (!IsPDPositionOn())
		{
			CMS896AStn::MotionUpdateSearchProfile(BPR_TOOLING_T, szNearProfile, dSpeed, lDist, dSpeed, lDDist, &m_stRotarytTooling_T);
			CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szNearProfile, &m_stRotarytTooling_T);
			CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_WAIT, &m_stRotarytTooling_T, szNearProfile);
			UpdateOutput();
			Sleep(500);
		}
//	if (IsPDPositionOn())
	{
		CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szAwayProfile, &m_stRotarytTooling_T);
		CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_WAIT, &m_stRotarytTooling_T, szAwayProfile);
		UpdateOutput();
		Sleep(500);
	}
		dSpeed = 0.05;
		CMS896AStn::MotionUpdateSearchProfile(BPR_TOOLING_T, szNearProfile, dSpeed, lDist, dSpeed, lDDist, &m_stRotarytTooling_T);
		CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szNearProfile, &m_stRotarytTooling_T);
		CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_NOWAIT, &m_stRotarytTooling_T, szNearProfile);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	if (bWait == SFM_WAIT)
	{
		ToolingT_Sync();
	}

	m_ucPbtToolingStageInUse = 2;
	return TRUE;
}

BOOL CBondPr::ToolingT_SearchToISP(BOOL bWait)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if (!IsProber())	
		return TRUE;

	if (m_bSelTooling_T == FALSE)
		return TRUE;

	try
	{
		CString szAwayProfile = BPR_SP_TOOLING_T_ISP;
		CString szNearProfile = BPR_SP_TOOLING_T_ISP_NEG;
		LONG lDist = -100000, lDDist = 20;
		DOUBLE dSpeed = 1.0;
		if (!IsISPPositionOn())
		{
			CMS896AStn::MotionUpdateSearchProfile(BPR_TOOLING_T, szNearProfile, dSpeed, lDist, dSpeed, lDDist, &m_stRotarytTooling_T);
			CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szNearProfile, &m_stRotarytTooling_T);
			CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_WAIT, &m_stRotarytTooling_T, szNearProfile);
			UpdateOutput();
			Sleep(500);
		}
//	if (IsISPPositionOn())
	{
		CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szAwayProfile, &m_stRotarytTooling_T);
		CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_WAIT, &m_stRotarytTooling_T, szAwayProfile);
		UpdateOutput();
		Sleep(500);
	}
		dSpeed = 0.05;
		CMS896AStn::MotionUpdateSearchProfile(BPR_TOOLING_T, szNearProfile, dSpeed, lDist, dSpeed, lDDist, &m_stRotarytTooling_T);
		CMS896AStn::MotionSelectSearchProfile(BPR_TOOLING_T, szNearProfile, &m_stRotarytTooling_T);
		CMS896AStn::MotionSearch(BPR_TOOLING_T, 1, SFM_NOWAIT, &m_stRotarytTooling_T, szNearProfile);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	m_ucPbtToolingStageInUse = 1;
	if (bWait == SFM_WAIT)
	{
		return ToolingT_Sync();
	}

	return TRUE;
}

BOOL CBondPr::IsOpticsPositionOn()
{
	return !CMS896AStn::MotionReadInputBit(BPR_SI_OPTICS_POS);
}

BOOL CBondPr::IsISPPositionOn()
{
	return !CMS896AStn::MotionReadInputBit(BPR_SI_ISP_POS);
}
			
BOOL CBondPr::IsPDPositionOn()
{
	return !CMS896AStn::MotionReadInputBit(BPR_SI_PD_POS);
}

BOOL CBondPr::IsISPUp()
{
	return !CMS896AStn::MotionReadInputBit(BPR_SI_ISP_UP);
}

BOOL CBondPr::IsISPDown()
{
	return !CMS896AStn::MotionReadInputBit(BPR_SI_ISP_DOWN);
}

BOOL CBondPr::SetISPDown(BOOL bOn)
{
	return CMS896AStn::MotionSetOutputBit(BPR_SO_ISP_DOWN, bOn);
}

BOOL CBondPr::MoveToolingStage(UCHAR ucStage, BOOL  bSelectBondCam, BOOL bDownISP)
{
	if( IsProberPlus() )
	{
		return MovePlusISP(ucStage);
	}

	BOOL bLightingOn = FALSE;

	BOOL bReturn = TRUE;

	SetISPDown(FALSE);
	INT nCount = 0;
	while(1)
	{
		Sleep(100);

		if (IsISPUp() == TRUE)
		{
			break;
		}

		nCount++;
		if (nCount >= 50)
		{
			break;
		}
	}

	if (IsISPUp() == FALSE)
	{
		SetAlert(IDS_BPR_ISP_IS_NOT_IN_SAFE_POSITION);
		return FALSE;
	}

	switch (ucStage)
	{
	case MS_TOOLINGSTAGE_PD:
		ToolingT_SearchToDetector();
		bLightingOn = FALSE;
		if( m_lISPDownSettleDelay>0 )	// Add ISP delap to PD Head
		{
			Sleep(m_lISPDownSettleDelay);
		}
		break;
	case MS_TOOLINGSTAGE_ISP:		//ISP
		bReturn = ToolingT_SearchToISP();
		
		if (IsISPPositionOn() == TRUE)
		{
			if( bDownISP )
			{
				SetISPDown(TRUE);
				DOUBLE dTime = GetTime();
				while(1)
				{
					Sleep(100);

					if (IsISPDown() == TRUE)
					{
						break;
					}

					if( labs(GetTime()-dTime)>m_lISPDownTimeout )
					{
						bReturn = FALSE;
						break;
					}
				}
				if( m_lISPDownSettleDelay>0 )
				{
					Sleep(m_lISPDownSettleDelay);
				}
			}
		}
		else
		{
			bReturn = FALSE;
		}
		bLightingOn = FALSE;
		break;
	default:
		if ( m_ucPbtToolingStageInUse == 0 && m_bSkipTheCheckToolingHome == TRUE)
		{
			// Do Nothing
		}
		else
		{
			ToolingT_SearchToOptics();
		}

		if ( bSelectBondCam == TRUE )
		{
			SelectBondCamera();
		}

		bLightingOn = TRUE;
		break;
	}

	TurnOnOffPrLighting(bLightingOn);

	return bReturn;
}

LONG CBondPr::SetSkipToolingStageCheck(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	m_bSkipTheCheckToolingHome	= bOn;

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::Diag_PowerOn_ToolingStage(IPC_CServiceMessage& svMsg)
{
	BOOL bOn;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	StartLoadingAlert();
	ToolingT_PowerOn(bOn);
	CloseLoadingAlert();
	
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::MoveToToolingStageToUse(IPC_CServiceMessage& svMsg)
{
	UCHAR ucToolingStageToUse = m_ucToolingStageToUse + 1;
	//0=Optics; 1=ISP; 2=Detector
	BOOL bDownISP,bCheckDownISP = FALSE;	
	svMsg.GetMsg(sizeof(BOOL), &bCheckDownISP);

	bDownISP = FALSE;
	if ( bCheckDownISP == TRUE && ucToolingStageToUse == 1 )
	{
		bDownISP = TRUE;
	}

	BOOL bReturn = MoveToolingStage(ucToolingStageToUse, TRUE, bDownISP);	// switch to user defined.
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CBondPr::MovePlusIspDown(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if( IsProberPlus() )
	{
		BOOL bIsp = FALSE;	//0=Optics; 1=ISP; 2=Detector
		svMsg.GetMsg(sizeof(BOOL), &bIsp);
		if( bIsp )
			MoveToolingStage(1);
		else
			MoveToolingStage(0);
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SelectToolingStage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	UCHAR ucStage = 0;	//0=Optics; 1=ISP; 2=Detector
	svMsg.GetMsg(sizeof(UCHAR), &ucStage);

	bReturn = MoveToolingStage(ucStage);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::IsAtIspPosition(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn =  IsISPPositionOn();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SelectToolingStageHmi(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	UCHAR ucStage = 0;	//0=Optics; 1=ISP; 2=Detector
	svMsg.GetMsg(sizeof(UCHAR), &ucStage);

	MoveToolingStage(ucStage, TRUE, FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SelectToolingStageShortcut(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	LONG lResult = 0;
	LONG lSelection = 0;
	
	CStringList szSelection;
	szSelection.AddTail("Move To Optics");
	szSelection.AddTail("Move To ISP");
	if( IsProberPlus()==FALSE )
		szSelection.AddTail("Move To PD");

	lResult = HmiSelection("Please select item", "Tooling Stage", szSelection, lSelection);

	if (lResult != -1)
	{
		bReturn = MoveToolingStage((UCHAR)lResult);	//	, TRUE, FALSE);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

LONG CBondPr::IsToolingStageReady(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	if (!IsProber())
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	if (m_ucPbtToolingStageInUse == 0)		//0=Optics; 1=ISP; 2=Detector
	{
		bReturn = FALSE;
		SetErrorMessage("Prober Error: Tooling stage optics in use");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SetISPDownCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = TRUE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (bOn == TRUE)
	{
		// only allow ISP down if it is at its position
		if (IsISPPositionOn() == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}
	
	SetISPDown(bOn);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::SetISPDownUpForCleanProbePin(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = TRUE;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bOn);

	if (m_ucToolingStageToUse != MS_TOOLINGSTAGE_ISP)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;	
	}

	SetISPDown(bOn);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::TurnOnOffPrLightingCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bOn = TRUE;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bOn);

	TurnOnOffPrLighting(bOn);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBondPr::MovePlusISP(UCHAR ucStage)
{
	BOOL bLightingOn = FALSE;

	BOOL bReturn = TRUE;

	switch (ucStage)
	{
	case MS_TOOLINGSTAGE_ISP:		//ISP
		if (!IsISPPositionOn())
		{
			SetISPDown(TRUE);
			if( m_lISPDownSettleDelay>0 )
			{
				Sleep(m_lISPDownSettleDelay);
			}
			DOUBLE dTime = GetTime();
			while(1)
			{
				Sleep(100);

				if (IsISPPositionOn())
				{
					break;
				}

				if( labs(GetTime()-dTime)>m_lISPDownTimeout )
				{
					break;
				}
			}
		}
		UpdateOutput();
		m_ucPbtToolingStageInUse = 1;
		bReturn =  IsISPPositionOn();
		bLightingOn = FALSE;
		break;
	case MS_TOOLINGSTAGE_PD:
	default:
		if (IsISPPositionOn())
		{
			SetISPDown(FALSE);
			if( m_lISPDownSettleDelay>0 )
			{
				Sleep(m_lISPDownSettleDelay);
			}
		}
		UpdateOutput();
		m_ucPbtToolingStageInUse = 0;
		IsISPPositionOn();

		bLightingOn = TRUE;
		break;
	}

	TurnOnOffPrLighting(bLightingOn);

	return bReturn;
}

