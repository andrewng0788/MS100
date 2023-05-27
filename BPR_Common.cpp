/////////////////////////////////////////////////////////////////
// BPR_Common.cpp : Common functions of the CBondPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
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
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondPr.h"
#include "FileUtil.h"
#include "BinLoader.h"
#include "NGGrade.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CBondPr::InitVariable(VOID)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	short	i = 0;

	m_dBTResolution_UM_CNT	= GetChannelResolution(MS896A_CFG_CH_BINTABLE_X) * 1000;

	m_bDisplayUnitInUm			= FALSE;		//v4.40T8
	if (pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR_ONLY))	//v4.58A3
	{
		m_bDisplayUnitInUm		= TRUE;			//v4.58A2
	}


	m_lRT_Count1 = 0;
	m_lRT_Count2 = 0;

	//Learn die parameter
	m_ucLearnDieCornerNo		= 0;
	memset(&m_stLearnDieCornerPos, 0, sizeof(m_stLearnDieCornerPos));
	memset(&m_stLearnNormalDieCornerPos, 0, sizeof(m_stLearnNormalDieCornerPos));
	memset(&m_stLearnFFModeDieCornerPos, 0, sizeof(m_stLearnFFModeDieCornerPos));

	m_bIsPowerOn_Tooling_T		= FALSE;
	m_bSelTooling_T				= FALSE;
	m_ucPbtToolingStageInUse	= 0;	//0=Optics; 1=ISP; 2=Detector
	m_bSkipTheCheckToolingHome	= FALSE;

	m_bSelectDieType			= BPR_NORMAL_DIE;
	m_szLrnDieType				= BPR_HMI_NORM_DIE;
	m_ucGenDieShape				= BPR_RECTANGLE_DIE;
	m_ucDieShape				= BPR_RECTANGLE_DIE;
	m_lCurRefDieNo				= 1;
	m_ssPRSrchID				= 0;
	m_stDieSize.x				= 0;
	m_stDieSize.y				= 0;
	m_stSearchArea.coCorner1.x	= PR_MAX_WIN_ULC_X;
	m_stSearchArea.coCorner1.y	= PR_MAX_WIN_ULC_Y;
	m_stSearchArea.coCorner2.x	= PR_MAX_WIN_LRC_X;
	m_stSearchArea.coCorner2.y	= PR_MAX_WIN_LRC_Y;

	m_dFovSize					= 0.0;

	m_lLrnAlignAlgo			= 0;
	m_lLrnBackground		= 0;
	m_lLrnFineSrch			= 0;
	m_lLrnInspMethod		= 0;
	m_lLrnInspRes			= 0;

	//Search Die parameter
	m_lSrchAlignRes			= PR_LOW_DIE_ALIGN_ACCURACY;
	m_lSrchDefectThres		= 12;
	m_lSrchDieAreaX			= 4;
	m_lSrchDieAreaY			= 4;
	m_lSrchDieScore			= 70;
	m_lSrchGreyLevelDefect	= 0;
	m_lCurNormDieID			= 0;
	m_lCurNormDieSizeX		= 0;
	m_lCurNormDieSizeY		= 0;
	m_dCurNormDieSizeX		= 0;
	m_dCurNormDieSizeY		= 0;
	m_dSrchSingleDefectArea	= 0.0;
	m_dSrchTotalDefectArea	= 0.0;
	m_dSrchChipArea			= 0.0;
	m_dSrchDieAreaX			= m_lSrchDieAreaX * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
	m_dSrchDieAreaY			= m_lSrchDieAreaY * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60

	m_bMoveEjrZAndPostBond	= FALSE;

	//v2.96T4
	m_dSpSingleDefectScore	= 0;
	m_dSpTotalDefectScore	= 0;
	m_dSpChipAreaScore		= 0;

	m_bUseHWTrigger			= FALSE;

	m_lPrCenterX			= PR_DEF_CENTRE_X;
	m_lPrCenterY			= PR_DEF_CENTRE_Y;
	m_lPrCenterOffsetXInPixel	= 0;
	m_lPrCenterOffsetYInPixel	= 0;
	m_dCalibX				= 0.0; 
	m_dCalibY				= 0.0;		
	m_dCalibXY				= 0.0; 
	m_dCalibYX				= 0.0;		
	m_dFFModeCalibX			= 0.0; 
	m_dFFModeCalibY			= 0.0;		
	m_dFFModeCalibXY		= 0.0; 
	m_dFFModeCalibYX		= 0.0;		
	m_dCalibX2				= 0.0; 
	m_dCalibY2				= 0.0;		
	m_dCalibXY2				= 0.0; 
	m_dCalibYX2				= 0.0;
	m_lPrCal1stStepSize		= 100;		//in motor steps
	m_bDieLearnt			= FALSE;
	m_bDieCalibrated		= FALSE;
	m_bDieCalibrated2		= FALSE;
	m_bSrchEnableDefectCheck= FALSE;
	m_bSrchEnableChipCheck	= FALSE;
	m_bUseMouse				= FALSE;

	m_lBondPosOffsetXPixel1		= PR_DEF_CENTRE_X;
	m_lBondPosOffsetYPixel1		= PR_DEF_CENTRE_Y;
	m_lBondPosOffsetXPixel2		= PR_DEF_CENTRE_X;
	m_lBondPosOffsetYPixel2		= PR_DEF_CENTRE_Y;

	m_lBondPosOffsetXPixel		= PR_DEF_CENTRE_X;
	m_lBondPosOffsetYPixel		= PR_DEF_CENTRE_Y;
	m_lTmpBondPosOffsetXPixel	= PR_DEF_CENTRE_X;
	m_lTmpBondPosOffsetYPixel	= PR_DEF_CENTRE_Y;

	m_lOfflineCount			= 0;
	m_bStartOffline			= FALSE;

	m_bUsePostBond_Backup	= FALSE;
	m_bPlacementCheck_Backup = FALSE;
	m_bRelOffsetCheck_Backup = FALSE;
	m_bDefectCheck_Backup	 = FALSE;	
	m_bEmptyCheck_Backup	 = FALSE;	

	//Postbond data
	m_bUsePostBond			= FALSE;
	m_bPostBondAtBond		= FALSE;
	m_bAverageAngleCheck	= FALSE;	
	m_bBinTableAngleCheck	= FALSE;	
	m_bPlacementCheck		= FALSE;
	m_bRelOffsetCheck		= FALSE;
	m_bDefectCheck			= FALSE;	
	m_bEmptyCheck			= FALSE;	
	m_bMS100DisplayPbSrchRect	= FALSE;

	m_bReSearchCurrDie		= FALSE;

	m_lCurrentBinBlock		= 1;	
	m_lMaxAngleAllow		= 0;	
	m_lMaxAccAngleAllow		= 0;	
	m_lMaxDefectAllow		= 0;	
	m_lMaxEmptyAllow		= 0;	
	m_lMaxAccEmptyAllow		= 0;	
	m_lMaxShiftAllow		= 0;	
	m_lMaxRelShiftAllow		= 0;	
	m_dAverageAngle			= 0.0;	
	m_dMaxAngle				= 0.0;	
	m_dDieShiftX			= 0.0;	
	m_dDieShiftY			= 0.0;	

	//v4.51A17	//SanAn TJ
	m_dMinCpXAllow			= 0;
	m_dMinCpYAllow			= 0;
	m_dMinCpTAllow			= 0;
	m_dSpcCpX				= 0;
	m_dSpcCpY				= 0;
	m_dSpcCpT				= 0;
	m_dSpcCpkX				= 0;
	m_dSpcCpkY				= 0;
	m_dSpcCpkT				= 0;
	m_lSpcCpCounter			= 0;

	m_ulLastBin				= 999;
	m_ulLastIndex			= 999;

	m_bEnableTotalDefect	= FALSE;
	m_bEnableLineDefect		= FALSE;
	m_bEnableProberBondPR	= FALSE;
	m_dMinLineDefectLength	= 0;
	m_lMinNumOfLineDefects	= 1;
	m_lMaxNumOfDefects		= 32767;

	m_lCurRefDieNo			= 1;

	m_bPreBondAlignUseEpoxySearch	= FALSE;	//v4.58A4
	m_bCheckFrameAngleByHwMark		= FALSE;	//v4.59A1

	m_bEnableRTCompXY		= FALSE;
	InitRtBHCompXYArrays(TRUE);  //Arm#1
	InitRtBHCompXYArrays(FALSE); //Arm#2

	m_bLastBH1DieIsEmpty	= FALSE;
	m_bLastBH2DieIsEmpty	= FALSE;

	m_lOutCamBlockEmptyCounter = 0;

//	m_lPBIShiftCounter = 0;
//	m_lPBIAngleCounter = 0;
	m_ulEpoxySize			= 0;
	for (i=0; i<BPR_MAX_BINBLOCK; i++)
	{
		m_lGenMaxAngleAllow[i]	= 0;
		m_lGenMaxAccAngleAllow[i]	= 0;
		m_lGenMaxDefectAllow[i]	= 0;
		m_lGenMaxEmptyAllow[i]	= 0;
		m_lGenMaxAccEmptyAllow[i]	= 0;
		m_lGenMaxShiftAllow[i]	= 0;
		m_lGenRelDieShiftAllow[i] = 0;
		m_dGenAverageAngle[i]	= 0.0;
		m_dGenMaxAngle[i]		= 0.0;
		m_dGenDieShiftX[i]		= 0.0;
		m_dGenDieShiftY[i]		= 0.0;

		m_lOutAngleCounter[i]		= 0;	
		m_lOutAngleCounter2[i]		= 0;	
		m_lAccOutAngleCounter[i]	= 0;
		m_lAccOutAngleCounter2[i]	= 0;
		m_lOutDefectCounter[i]		= 0;	
		m_lOutEmptyCounter[i]		= 0;	
		m_lOutEmptyCounter2[i]		= 0;	
		m_lOutAccEmptyCounter[i]	= 0;	
		m_lOutAccEmptyCounter2[i]	= 0;
		m_lOutAccEmptyCounterTotal[i]	= 0;
		//m_lOutCamBlockEmptyCounter[i] = 0;
		m_lOutShiftCounter[i]		= 0;	
		m_lAccOutShiftCounter[i]	= 0;	
		m_lAccOutShiftCounter2[i]	= 0;	
		m_lOutRelShiftCounter[i]	= 0;	
		m_lTotalDieBonded[i]		= 0;	
		m_dOutAverageAngle[i]		= 0.0;	
		m_dTotalAngle[i]			= 0.0;	
	}

	for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
	{
		m_lBPRGenCoaxLightLevel[i] = 0;
		m_lBPRGenRingLightLevel[i] = 0;
		m_lBPRGenSideLightLevel[i] = 0;

		m_lBPRLrnCoaxLightLevel[i] = 0;
		m_lBPRLrnRingLightLevel[i] = 0;
		m_lBPRLrnSideLightLevel[i] = 0;

		m_lBPRTmpCoaxLightLevel[i] = 0;
		m_lBPRTmpRingLightLevel[i] = 0;
		m_lBPRTmpSideLightLevel[i] = 0;
	}

	m_lBPRLrnCoaxLightHmi		= 0;
	m_lBPRLrnRingLightHmi		= 0;
	m_lBPRLrnSideLightHmi		= 0;

	m_lNoLastStateFile = 0;
	for (i=0; i<BPR_MAX_DIE; i++)
	{
		m_ssGenPRSrchID[i]			= 0;
		m_stGenDieSize[i].x			= 0;
		m_stGenDieSize[i].y			= 0;
		m_bGenDieLearnt[i]			= FALSE;
		m_bGenSrchEnableDefectCheck[i]	= FALSE;
		m_lGenLrnAlignAlgo[i]		= 0;
		m_lGenLrnBackgroud[i]		= 0;
		m_lGenLrnFineSrch[i]		= 2; //Default value by Matthew 20190410
		m_lGenLrnInspMethod[i]		= 0;
		m_lGenLrnInspRes[i]			= 0;
		m_lGenSrchAlignRes[i]		= 0;
		m_lGenSrchDefectThres[i]	= 0;
		m_lGenSrchDieScore[i]		= 0;
		m_lGenSrchGreyLevelDefect[i] = 0;
		m_dGenSrchChipArea[i]		= 0;
		m_dGenSrchSingleDefectArea[i] = 0;
		m_dGenSrchTotalDefectArea[i] = 0;
		m_bGenSrchEnableChipCheck[i] = FALSE;
	}

	m_bLrnAdvanceOption	= FALSE;
	if( pApp->GetCustomerName()==CTM_SEMITEK )
	{
		m_bLrnAdvanceOption	= TRUE;
	}
	m_lInspDetectWinNo = 0;
	m_lInspIgnoreWinNo = 0;
	memset(&m_stInspDetectWin, 0, sizeof(m_stInspDetectWin));
	memset(&m_stInspIgnoreWin, 0, sizeof(m_stInspIgnoreWin));

	m_lBPRExposureTimeLevel=0;		//v3.30T1
	m_lBPRGExposureTimeLevel=0;		//v3.30T1

	m_szShowImagePath		= "";
	m_szShowImageType		= BPR_HMI_NORM_DIE;
	m_lShowImageNo			= 1;

	m_szPostbondDataSource	= gszROOT_DIRECTORY + "\\OutputFiles\\Postbond";

	m_szPostbondDataX		= "MapSorter RunTime>Bin1+X Placement";
	m_szPostbondDataY		= "MapSorter RunTime>Bin1+Y Placement";
	m_szPostbondDataTheta	= "MapSorter RunTime>Bin1+Die Rotation";

	m_szPostbondDispX		= "MapSorter Display>Display+X Placement";
	m_szPostbondDispY		= "MapSorter Display>Display+Y Placement";
	m_szPostbondDispTheta	= "MapSorter Display>Display+Die Rotation";

	/*** Arm1 ***/
	m_szPostbondDataSource_Arm1	= gszROOT_DIRECTORY + "\\OutputFiles\\Postbond Arm1";
	m_szPostbondDataX_Arm1		= "MapSorter RunTime>Bin1+X Placement Arm1";
	m_szPostbondDataY_Arm1		= "MapSorter RunTime>Bin1+Y Placement Arm1";
	m_szPostbondDataTheta_Arm1	= "MapSorter RunTime>Bin1+Die Rotation Arm1";

	m_szPostbondDispX_Arm1		= "MapSorter Display>Display+X Placement Arm1";
	m_szPostbondDispY_Arm1		= "MapSorter Display>Display+Y Placement Arm1";
	m_szPostbondDispTheta_Arm1	= "MapSorter Display>Display+Die Rotation Arm1";
	/*** end Arm1 ***/
	
	/*** Arm2 ***/
	m_szPostbondDataSource_Arm2	= gszROOT_DIRECTORY + "\\OutputFiles\\Postbond Arm2";
	m_szPostbondDataX_Arm2		= "MapSorter RunTime>Bin1+X Placement Arm2";
	m_szPostbondDataY_Arm2		= "MapSorter RunTime>Bin1+Y Placement Arm2";
	m_szPostbondDataTheta_Arm2	= "MapSorter RunTime>Bin1+Die Rotation Arm2";

	m_szPostbondDispX_Arm2		= "MapSorter Display>Display+X Placement Arm2";
	m_szPostbondDispY_Arm2		= "MapSorter Display>Display+Y Placement Arm2";
	m_szPostbondDispTheta_Arm2	= "MapSorter Display>Display+Die Rotation Arm2";
	/*** end Arm2 ***/

	m_bDisableTurnOnOffPrLighting = FALSE;

	//Temp use data
	m_lDiePitchX			= 0;
	m_lDiePitchY			= 0;
	m_bMissingDie			= FALSE;

	m_nArmSelection			= 0;
	m_lLearnUPLPRIndex		= 0;

	m_bUpdateOutput			= FALSE; 

	m_bHMIReferenceDie		= FALSE;

	m_lCleanProbeLightLevel = 50;
	m_bEnableElectech3EDLCheck150Bin = FALSE;	

	m_lAGCC1CenterXInPixel	= 0;
	m_lAGCC1CenterYInPixel	= 0;
	m_lAGCC2CenterXInPixel	= 0;
	m_lAGCC2CenterYInPixel	= 0;
	m_dAGCCalibX			= 0;	
	m_dAGCCalibY			= 0;	

	m_lBTRow				= 0;
	m_lBTCol				= 0;
	m_szSpcYDisplayTitle	= "";

	InitZoomView();
}


VOID CBondPr::InitSPCControl(VOID)
{
	CString		szTemp;
	short		ssBin;

	// Load SPC library
	SPC_CDataManagement::Instance()->AutoInstallLibrary("SPCStdDataAccess");


	//Create Database
	m_pDatabase			= SPC_CDataManagement::Instance()->CreateDatabase("Native", m_szPostbondDataSource);
	m_pRuntimeProduct   = m_pDatabase->CreateProduct("MapSorter RunTime");


	//Set Database properties
	for (ssBin=0; ssBin < BPR_MAX_BINBLOCK; ssBin++)
	{
		szTemp.Format("Bin%d",ssBin+1);

		m_pBin[ssBin] = m_pRuntimeProduct->GetProductPart()->CreatePart(szTemp);

		m_pDieX[ssBin]			= m_pBin[ssBin]->CreateDouble("X Placement");
		m_pDieY[ssBin]			= m_pBin[ssBin]->CreateDouble("Y Placement");
		m_pDieTheta[ssBin]		= m_pBin[ssBin]->CreateDouble("Die Rotation");
		m_pDieAvgAngle[ssBin]	= m_pBin[ssBin]->CreateDouble("Average Die Rotation");

		m_pDieIsGood[ssBin]		= m_pBin[ssBin]->CreateBool("Good Die");
		m_pDieType[ssBin]		= m_pBin[ssBin]->CreateLong("Die Type");
		m_pDieIndex[ssBin]		= m_pBin[ssBin]->CreateULong("Bin Index");

		if (m_bDisplayUnitInUm)		//v4.58A3
		{
			m_pBin[ssBin]->SetLimits("X Placement", 0.0, -40, 40);
			m_pBin[ssBin]->SetLimits("Y Placement", 0.0, -40, 40);
		}
		else
		{
			m_pBin[ssBin]->SetLimits("X Placement", 0.0, -1.5, 1.5);
			m_pBin[ssBin]->SetLimits("Y Placement", 0.0, -1.5, 1.5);
		}
		m_pBin[ssBin]->SetLimits("Die Rotation", 0.0, -3.0, 3.0);
		m_pBin[ssBin]->SaveSchema();
		m_pBin[ssBin]->SaveLimits();
	}

	//Set display database
	m_pDisplayProduct	= m_pDatabase->CreateProduct("MapSorter Display");	
	m_pDspBin			= m_pDisplayProduct->GetProductPart()->CreatePart("Display");
	m_pDspDieX			= m_pDspBin->CreateDouble("X Placement");
	m_pDspDieY			= m_pDspBin->CreateDouble("Y Placement");
	m_pDspDieTheta		= m_pDspBin->CreateDouble("Die Rotation");
	m_pAvgDieAngle		= m_pDspBin->CreateDouble("Average Die Rotation");
	
	if (m_bDisplayUnitInUm)		//v4.40T7
	{
		m_pDspBin->SetLimits("X Placement", 0.0, -40, 40);
		m_pDspBin->SetLimits("Y Placement", 0.0, -40, 40);
		m_szSpcYDisplayTitle = "Y unit in um";
	}
	else
	{
		m_pDspBin->SetLimits("X Placement", 0.0, -1.5, 1.5);
		m_pDspBin->SetLimits("Y Placement", 0.0, -1.5, 1.5);
		m_szSpcYDisplayTitle = "Y unit in mil";
	}

	m_pDspBin->SetLimits("Die Rotation", 0.0, -3.0, 3.0);
	m_pDspBin->SaveSchema();
	m_pDspBin->SaveLimits();

	m_szPostbondDataX		= "MapSorter RunTime>Bin1+X Placement";
	m_szPostbondDataY		= "MapSorter RunTime>Bin1+Y Placement";
	m_szPostbondDataTheta	= "MapSorter RunTime>Bin1+Die Rotation";

	m_szPostbondDispX		= "MapSorter Display>Display+X Placement";
	m_szPostbondDispY		= "MapSorter Display>Display+Y Placement";
	m_szPostbondDispTheta	= "MapSorter Display>Display+Die Rotation";


	m_pRuntimeProduct->GetProductPart()->SaveSchema();
	m_pRuntimeProduct->GetProductPart()->SaveLimits();
	m_pDisplayProduct->GetProductPart()->SaveSchema();
	m_pDisplayProduct->GetProductPart()->SaveLimits();

	if (m_bDisplayUnitInUm)		//v4.40T7
	{
		//m_szPostbondDispX		= "MapSorter Display>Display+X Placement (um)";
		//m_szPostbondDispY		= "MapSorter Display>Display+Y Placement (um)";
		
		//v4.58A3
		//m_szSPCCmdDspDieX	= _T("YMinimum=-50;YMaximum=50;Title=Autobond X(um);Store[Cp]=BPR_dSpcCpX");
		//m_szSPCCmdDspDieY	= _T("YMinimum=-50;YMaximum=50;Title=Autobond Y(um);Store[Cp]=BPR_dSpcCpY");
		//m_szSPCCmdDspDieT	= _T("TMinimum=-15;TMaximum=15;Title=Autobond T;Store[Cp]=BPR_dSpcCpT");
		m_szSPCCmdDspDieX	= _T("Store[Cp]=BPR_dSpcCpX;Store[Cpk]=BPR_dSpcCpkX");
		m_szSPCCmdDspDieY	= _T("Store[Cp]=BPR_dSpcCpY;Store[Cpk]=BPR_dSpcCpkY");
		m_szSPCCmdDspDieT	= _T("Store[Cp]=BPR_dSpcCpT;Store[Cpk]=BPR_dSpcCpkT");
	}
	else	//v4.51A17
	{
		m_szSPCCmdDspDieX	= _T("Store[Cp]=BPR_dSpcCpX;Store[Cpk]=BPR_dSpcCpkX");
		m_szSPCCmdDspDieY	= _T("Store[Cp]=BPR_dSpcCpY;Store[Cpk]=BPR_dSpcCpkY");
		m_szSPCCmdDspDieT	= _T("Store[Cp]=BPR_dSpcCpT;Store[Cpk]=BPR_dSpcCpkT");
	}

	//Temp directory for temp SPC data
	CreateDirectory(m_szPostbondDataSource + "\\Temp", NULL);

	ResetPostBondSPCFlags();	//v3.93
}

VOID CBondPr::InitSPCControl_Arm1(VOID)
{
	CString		szTemp;
	short		ssBin;

	// Load SPC library
	SPC_CDataManagement::Instance()->AutoInstallLibrary("SPCStdDataAccess");


	//Create Database
	m_pDatabase_Arm1			= SPC_CDataManagement::Instance()->CreateDatabase("Native", m_szPostbondDataSource_Arm1);
	m_pRuntimeProduct_Arm1		= m_pDatabase_Arm1->CreateProduct("MapSorter RunTime");


	//Set Database properties
	for (ssBin=0; ssBin < BPR_MAX_BINBLOCK; ssBin++)
	{
		szTemp.Format("Bin%d",ssBin+1);

		m_pBin_Arm1[ssBin] = m_pRuntimeProduct_Arm1->GetProductPart()->CreatePart(szTemp);

		m_pDieX_Arm1[ssBin]			= m_pBin_Arm1[ssBin]->CreateDouble("X Placement Arm1");
		m_pDieY_Arm1[ssBin]			= m_pBin_Arm1[ssBin]->CreateDouble("Y Placement Arm1");
		m_pDieTheta_Arm1[ssBin]		= m_pBin_Arm1[ssBin]->CreateDouble("Die Rotation Arm1");
		m_pDieAvgAngle_Arm1[ssBin]	= m_pBin_Arm1[ssBin]->CreateDouble("Average Die Rotation Arm1");

		m_pDieIsGood_Arm1[ssBin]	= m_pBin_Arm1[ssBin]->CreateBool("Good Die Arm1");
		m_pDieType_Arm1[ssBin]		= m_pBin_Arm1[ssBin]->CreateLong("Die Type Arm1");
		m_pDieIndex_Arm1[ssBin]		= m_pBin_Arm1[ssBin]->CreateULong("Bin Index Arm1");

		m_pBin_Arm1[ssBin]->SetLimits("X Placement Arm1", 0.0, -1.5, 1.5);
		m_pBin_Arm1[ssBin]->SetLimits("Y Placement Arm1", 0.0, -1.5, 1.5);
		m_pBin_Arm1[ssBin]->SetLimits("Die Rotation Arm1", 0.0, -3.0, 3.0);
		m_pBin_Arm1[ssBin]->SaveSchema();
		m_pBin_Arm1[ssBin]->SaveLimits();
	}

	//Set display database
	m_pDisplayProduct_Arm1	= m_pDatabase_Arm1->CreateProduct("MapSorter Display");	
	m_pDspBin_Arm1			= m_pDisplayProduct_Arm1->GetProductPart()->CreatePart("Display");
	m_pDspDieX_Arm1			= m_pDspBin_Arm1->CreateDouble("X Placement Arm1");
	m_pDspDieY_Arm1			= m_pDspBin_Arm1->CreateDouble("Y Placement Arm1");
	m_pDspDieTheta_Arm1		= m_pDspBin_Arm1->CreateDouble("Die Rotation Arm1");
	
	if (m_bDisplayUnitInUm)		//v4.40T7
	{
		m_pDspBin_Arm1->SetLimits("X Placement Arm1", 0.0, -40, 40);
		m_pDspBin_Arm1->SetLimits("Y Placement Arm1", 0.0, -40, 40);
	}
	else
	{
		m_pDspBin_Arm1->SetLimits("X Placement Arm1", 0.0, -1.5, 1.5);
		m_pDspBin_Arm1->SetLimits("Y Placement Arm1", 0.0, -1.5, 1.5);
	}
	m_pDspBin_Arm1->SetLimits("Die Rotation Arm1", 0.0, -3.0, 3.0);
	m_pDspBin_Arm1->SaveSchema();
	m_pDspBin_Arm1->SaveLimits();

	m_szPostbondDataX_Arm1		= "MapSorter RunTime>Bin1+X Placement Arm1";
	m_szPostbondDataY_Arm1		= "MapSorter RunTime>Bin1+Y Placement Arm1";
	m_szPostbondDataTheta_Arm1	= "MapSorter RunTime>Bin1+Die Rotation Arm1";

	m_szPostbondDispX_Arm1		= "MapSorter Display>Display+X Placement Arm1";
	m_szPostbondDispY_Arm1		= "MapSorter Display>Display+Y Placement Arm1";
	m_szPostbondDispTheta_Arm1	= "MapSorter Display>Display+Die Rotation Arm1";


	m_pRuntimeProduct_Arm1->GetProductPart()->SaveSchema();
	m_pRuntimeProduct_Arm1->GetProductPart()->SaveLimits();
	m_pDisplayProduct_Arm1->GetProductPart()->SaveSchema();
	m_pDisplayProduct_Arm1->GetProductPart()->SaveLimits();

	if (m_bDisplayUnitInUm)		//v4.40T7
	{
		//v4.58A3
		//m_szPostbondDispX_Arm1		= "MapSorter Display>Display+X Placement Arm1 (um)";
		//m_szPostbondDispY_Arm1		= "MapSorter Display>Display+Y Placement Arm1 (um)";
		//m_szSPCCmdDspDieX_Arm1 = _T("YMinimum=-50;YMaximum=50;Title=Autobond X1(um)");
		//m_szSPCCmdDspDieY_Arm1 = _T("YMinimum=-50;YMaximum=50;Title=Autobond Y1(um)");
	}

	//Temp directory for temp SPC data
	CreateDirectory(m_szPostbondDataSource_Arm1 + "\\Temp", NULL);
}

VOID CBondPr::InitSPCControl_Arm2(VOID)
{
	CString		szTemp;
	short		ssBin;

	// Load SPC library
	SPC_CDataManagement::Instance()->AutoInstallLibrary("SPCStdDataAccess");


	//Create Database
	m_pDatabase_Arm2			= SPC_CDataManagement::Instance()->CreateDatabase("Native", m_szPostbondDataSource_Arm2);
	m_pRuntimeProduct_Arm2		= m_pDatabase_Arm2->CreateProduct("MapSorter RunTime");


	//Set Database properties
	for (ssBin=0; ssBin < BPR_MAX_BINBLOCK; ssBin++)
	{
		szTemp.Format("Bin%d",ssBin+1);

		m_pBin_Arm2[ssBin] = m_pRuntimeProduct_Arm2->GetProductPart()->CreatePart(szTemp);

		m_pDieX_Arm2[ssBin]			= m_pBin_Arm2[ssBin]->CreateDouble("X Placement Arm2");
		m_pDieY_Arm2[ssBin]			= m_pBin_Arm2[ssBin]->CreateDouble("Y Placement Arm2");
		m_pDieTheta_Arm2[ssBin]		= m_pBin_Arm2[ssBin]->CreateDouble("Die Rotation Arm2");
		m_pDieAvgAngle_Arm2[ssBin]	= m_pBin_Arm2[ssBin]->CreateDouble("Average Die Rotation Arm2");

		m_pDieIsGood_Arm2[ssBin]	= m_pBin_Arm2[ssBin]->CreateBool("Good Die Arm2");
		m_pDieType_Arm2[ssBin]		= m_pBin_Arm2[ssBin]->CreateLong("Die Type Arm2");
		m_pDieIndex_Arm2[ssBin]		= m_pBin_Arm2[ssBin]->CreateULong("Bin Index Arm2");

		m_pBin_Arm2[ssBin]->SetLimits("X Placement Arm2", 0.0, -1.5, 1.5);
		m_pBin_Arm2[ssBin]->SetLimits("Y Placement Arm2", 0.0, -1.5, 1.5);
		m_pBin_Arm2[ssBin]->SetLimits("Die Rotation Arm2", 0.0, -3.0, 3.0);
		m_pBin_Arm2[ssBin]->SaveSchema();
		m_pBin_Arm2[ssBin]->SaveLimits();
	}

	//Set display database
	m_pDisplayProduct_Arm2	= m_pDatabase_Arm2->CreateProduct("MapSorter Display");	
	m_pDspBin_Arm2			= m_pDisplayProduct_Arm2->GetProductPart()->CreatePart("Display");
	m_pDspDieX_Arm2			= m_pDspBin_Arm2->CreateDouble("X Placement Arm2");
	m_pDspDieY_Arm2			= m_pDspBin_Arm2->CreateDouble("Y Placement Arm2");
	m_pDspDieTheta_Arm2		= m_pDspBin_Arm2->CreateDouble("Die Rotation Arm2");
	
	if (m_bDisplayUnitInUm)		//v4.40T7
	{
		m_pDspBin_Arm2->SetLimits("X Placement Arm2", 0.0, -40, 40);
		m_pDspBin_Arm2->SetLimits("Y Placement Arm2", 0.0, -40, 40);
	}
	else
	{
		m_pDspBin_Arm2->SetLimits("X Placement Arm2", 0.0, -1.5, 1.5);
		m_pDspBin_Arm2->SetLimits("Y Placement Arm2", 0.0, -1.5, 1.5);
	}
	m_pDspBin_Arm2->SetLimits("Die Rotation Arm2", 0.0, -3.0, 3.0);
	m_pDspBin_Arm2->SaveSchema();
	m_pDspBin_Arm2->SaveLimits();

	m_szPostbondDataX_Arm2		= "MapSorter RunTime>Bin1+X Placement Arm2";
	m_szPostbondDataY_Arm2		= "MapSorter RunTime>Bin1+Y Placement Arm2";
	m_szPostbondDataTheta_Arm2	= "MapSorter RunTime>Bin1+Die Rotation Arm2";

	m_szPostbondDispX_Arm2		= "MapSorter Display>Display+X Placement Arm2";
	m_szPostbondDispY_Arm2		= "MapSorter Display>Display+Y Placement Arm2";
	m_szPostbondDispTheta_Arm2	= "MapSorter Display>Display+Die Rotation Arm2";


	m_pRuntimeProduct_Arm2->GetProductPart()->SaveSchema();
	m_pRuntimeProduct_Arm2->GetProductPart()->SaveLimits();
	m_pDisplayProduct_Arm2->GetProductPart()->SaveSchema();
	m_pDisplayProduct_Arm2->GetProductPart()->SaveLimits();

	if (m_bDisplayUnitInUm)		//v4.40T7
	{
		//v4.58A3
		//m_szPostbondDispX_Arm2		= "MapSorter Display>Display+X Placement Arm2 (um)";
		//m_szPostbondDispY_Arm2		= "MapSorter Display>Display+Y Placement Arm2 (um)";
		//m_szSPCCmdDspDieX_Arm2 = _T("YMinimum=-50;YMaximum=50;Title=Autobond X2(um)");
		//m_szSPCCmdDspDieY_Arm2 = _T("YMinimum=-50;YMaximum=50;Title=Autobond Y2(um)");
	}

	//Temp directory for temp SPC data
	CreateDirectory(m_szPostbondDataSource_Arm2 + "\\Temp", NULL);
}

/*
VOID CBondPr::InitSPCControl_OfflinePBTest(VOID)	//v4.11T3
{
	CString		szTemp;

	// Load SPC library
	//SPC_CDataManagement::Instance()->AutoInstallLibrary("SPCStdDataAccess");

	//Create Database
	m_pDatabase_OPBTest			= SPC_CDataManagement::Instance()->CreateDatabase("Native", m_szPostbondDataSource_Arm2);

	//Set display database
	m_pDisplayProduct_OPBTest	= m_pDatabase_OPBTest->CreateProduct("MS OPBTest Display");	
	m_pDspBin_OPBTest			= m_pDisplayProduct_OPBTest->GetProductPart()->CreatePart("OPB Display");
	m_pDspDieX_OPBTest			= m_pDspBin_OPBTest->CreateDouble("X Placement OPB");
	m_pDspDieY_OPBTest			= m_pDspBin_OPBTest->CreateDouble("Y Placement OPB");
	m_pDspDieTheta_OPBTest		= m_pDspBin_OPBTest->CreateDouble("Die Rotation OPB");
	
	m_pDspBin_OPBTest->SetLimits("X Placement OPB", 0.0, -1.5, 1.5);
	m_pDspBin_OPBTest->SetLimits("Y Placement OPB", 0.0, -1.5, 1.5);
	m_pDspBin_OPBTest->SetLimits("Die Rotation OPB", 0.0, -3.0, 3.0);
	m_pDspBin_OPBTest->SaveSchema();
	m_pDspBin_OPBTest->SaveLimits();

	m_szPostbondDataX_OPBTest		= "MS RunTime>Bin1+X Placement OPB";
	m_szPostbondDataY_OPBTest		= "MS RunTime>Bin1+Y Placement OPB";
	m_szPostbondDataTheta_OPBTest	= "MS RunTime>Bin1+Die Rotation OPB";

	m_pDisplayProduct_OPBTest->GetProductPart()->SaveSchema();
	m_pDisplayProduct_OPBTest->GetProductPart()->SaveLimits();

	//Temp directory for temp SPC data
	CreateDirectory(gszROOT_DIRECTORY + "\\OutputFiles\\OPB", NULL);
}
*/

VOID CBondPr::UpdateHMIVariable(VOID)
{
	m_lMaxAngleAllow		= m_lGenMaxAngleAllow[m_lCurrentBinBlock - 1];	
	m_lMaxAccAngleAllow		= m_lGenMaxAccAngleAllow[m_lCurrentBinBlock - 1];	
	m_lMaxDefectAllow		= m_lGenMaxDefectAllow[m_lCurrentBinBlock - 1];	
	m_lMaxEmptyAllow		= m_lGenMaxEmptyAllow[m_lCurrentBinBlock - 1];	
	m_lMaxAccEmptyAllow		= m_lGenMaxAccEmptyAllow[m_lCurrentBinBlock - 1];		//v4.40T9	//Sanan
	m_lMaxShiftAllow		= m_lGenMaxShiftAllow[m_lCurrentBinBlock - 1];	
	m_lMaxRelShiftAllow		= m_lGenRelDieShiftAllow[m_lCurrentBinBlock - 1];	
	m_dAverageAngle			= m_dGenAverageAngle[m_lCurrentBinBlock - 1];	
	m_dMaxAngle				= m_dGenMaxAngle[m_lCurrentBinBlock - 1];	
	m_dDieShiftX			= m_dGenDieShiftX[m_lCurrentBinBlock - 1];	
	m_dDieShiftY			= m_dGenDieShiftY[m_lCurrentBinBlock - 1];	
}


VOID CBondPr::GetHMIVariable(VOID)
{
	m_lGenMaxAngleAllow[m_lCurrentBinBlock - 1]		= m_lMaxAngleAllow;
	m_lGenMaxAccAngleAllow[m_lCurrentBinBlock - 1]	= m_lMaxAccAngleAllow;		//v4.58A5
	m_lGenMaxDefectAllow[m_lCurrentBinBlock - 1]	= m_lMaxDefectAllow;
	m_lGenMaxEmptyAllow[m_lCurrentBinBlock - 1]		= m_lMaxEmptyAllow;
	m_lGenMaxAccEmptyAllow[m_lCurrentBinBlock - 1]	= m_lMaxAccEmptyAllow;
	m_lGenMaxShiftAllow[m_lCurrentBinBlock - 1]		= m_lMaxShiftAllow;
	m_lGenRelDieShiftAllow[m_lCurrentBinBlock - 1]	= m_lMaxRelShiftAllow;
	m_dGenAverageAngle[m_lCurrentBinBlock - 1]		= m_dAverageAngle;
	m_dGenMaxAngle[m_lCurrentBinBlock - 1]			= m_dMaxAngle;
	m_dGenDieShiftX[m_lCurrentBinBlock - 1]			= m_dDieShiftX;
	m_dGenDieShiftY[m_lCurrentBinBlock - 1]			= m_dDieShiftY;	
}


VOID CBondPr::CopyBinBlockVariable(const LONG lSoureBlock, const LONG lTargetBlock)
{
	m_lGenMaxAngleAllow[lTargetBlock - 1]		= m_lGenMaxAngleAllow[lSoureBlock - 1];
	m_lGenMaxAccAngleAllow[lTargetBlock - 1]	= m_lGenMaxAccAngleAllow[lSoureBlock - 1];
	m_lGenMaxDefectAllow[lTargetBlock - 1]		= m_lGenMaxDefectAllow[lSoureBlock - 1];
	m_lGenMaxEmptyAllow[lTargetBlock - 1]		= m_lGenMaxEmptyAllow[lSoureBlock - 1];
	m_lGenMaxAccEmptyAllow[lTargetBlock - 1]	= m_lGenMaxAccEmptyAllow[lSoureBlock - 1];
	m_lGenMaxShiftAllow[lTargetBlock - 1]		= m_lGenMaxShiftAllow[lSoureBlock - 1];
	m_lGenRelDieShiftAllow[lTargetBlock - 1]	= m_lGenRelDieShiftAllow[lSoureBlock - 1];
	m_dGenAverageAngle[lTargetBlock - 1]		= m_dGenAverageAngle[lSoureBlock - 1];
	m_dGenMaxAngle[lTargetBlock - 1]			= m_dGenMaxAngle[lSoureBlock - 1];
	m_dGenDieShiftX[lTargetBlock - 1]			= m_dGenDieShiftX[lSoureBlock - 1];
	m_dGenDieShiftY[lTargetBlock - 1]			= m_dGenDieShiftY[lSoureBlock - 1];	
}

BOOL CBondPr::SaveRecordID(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    if (pUtl->LoadBPRConfig() == FALSE)
	{
		return FALSE;
	}
    psmf = pUtl->GetBPRConfigFile();
	if ( psmf == NULL )
	{
		return FALSE;
	}

	CString szLog = _T("BPR: SaveRecordID - ");
	CString szValue;

	for (INT i=0; i<BPR_MAX_DIE; i++)
	{
		(*psmf)[BPR_DIE_PRID][i+1]								= (LONG) m_ssGenPRSrchID[i];
		(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_LEARNDIE][i+1]	= m_bGenDieLearnt[i];

		(*m_psmfSRam)["BondPr"]["Die Record"][i] = (LONG)m_ssGenPRSrchID[i];	
		szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
		szLog = szLog + szValue;
	}

	pUtl->UpdateBPRConfig();
    pUtl->CloseBPRConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return TRUE;
}

BOOL CBondPr::SavePrData(BOOL bUpdateHMIVariable)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
    CStringMapFile  *psmf;
	PR_COORD		stPixel;
	short	i = 0;	
	CString szLog;


	// open config file
    if (pUtl->LoadBPRConfig() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetBPRConfigFile();

	//Check Load/Save Data
	if ( psmf == NULL )
	{
		return FALSE;
	}

	if ( pApp->GetCustomerName() == "Cree" )		//v4.46T27
	{
		m_bUsePostBond	= TRUE;
		m_bEmptyCheck	= TRUE;
	}

	UpdateFovSize();
	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_FOV]		= m_dFovSize;		//v2.58
	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_ROW_FOV]		= m_dRowFovSize;	//4.5301	
	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_COL_FOV]		= m_dColFovSize;		


	//search die data
	if (m_dSrchSingleDefectArea > m_dSrchTotalDefectArea)
	{
		m_dSrchSingleDefectArea = m_dSrchTotalDefectArea;
	}


	(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHULCX]		= (LONG)m_stSearchArea.coCorner1.x;
	(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHULCY]		= (LONG)m_stSearchArea.coCorner1.y;
	(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHLRCX]		= (LONG)m_stSearchArea.coCorner2.x;
	(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHLRCY]		= (LONG)m_stSearchArea.coCorner2.y;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_X]		= m_lSrchDieAreaX;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_Y]		= m_lSrchDieAreaY;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_PASSSCORE]		= m_lSrchDieScore;

	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_TOTAL_DEFECTS]			= m_bEnableTotalDefect;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_LINE_DEFECT]			= m_bEnableLineDefect;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MIN_LINE_DEFECT_LENGTH]	= m_dMinLineDefectLength;
	if( m_lMinNumOfLineDefects==0 )
		m_lMinNumOfLineDefects = 32767;
	if( m_lMaxNumOfDefects==0 )
		m_lMaxNumOfDefects = 32767;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MIN_NUM_OF_LINE_DEFECT]	= m_lMinNumOfLineDefects;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MAX_NUM_OF_DEFECTS]		= m_lMaxNumOfDefects;
	(*psmf)[BPR_SRH_NDIE_DATA]["DisableLightingSetting"]			= m_bDisableLightingSetting;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_PREBOND_USE_EPOXYSEARCH]			= m_bPreBondAlignUseEpoxySearch;
	(*psmf)[BPR_SRH_NDIE_DATA][BPR_FRAMEALIGN_HW_MARK]				= m_bCheckFrameAngleByHwMark;

	for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
	{
		(*psmf)[BPR_LIGHT_SETTING][i+1][BPR_LRN_COAX_LIGHT_LEVEL] = m_lBPRLrnCoaxLightLevel[i];
		(*psmf)[BPR_LIGHT_SETTING][i+1][BPR_LRN_RING_LIGHT_LEVEL] = m_lBPRLrnRingLightLevel[i];
		(*psmf)[BPR_LIGHT_SETTING][i+1][BPR_LRN_SIDE_LIGHT_LEVEL] = m_lBPRLrnSideLightLevel[i];
	}

	(*psmf)[BPR_LIGHT_SETTING][BPR_LRN_EXPOSURE_TIME_LEVEL]		= m_lBPRExposureTimeLevel;	//ching
	(*psmf)[BPR_LIGHT_SETTING][BPR_LRN_G_EXPOSURE_TIME_LEVEL]	= m_lBPRExposureTimeLevel;	//v3.30T1

	//v4.44A3
	m_lPrCenterOffsetXInPixel = (m_lPrCenterX - PR_DEF_CENTRE_X) / PR_SCALE_FACTOR;
	m_lPrCenterOffsetYInPixel = (m_lPrCenterY - PR_DEF_CENTRE_Y) / PR_SCALE_FACTOR;

	//andrewng //2020-0615
	(*psmf)[BPR_GEN_NDIE_DATA]["WPR Use Hardware Trigger"] = m_bUseHWTrigger;

	//General die data
	// Save Record ID to LastState file instead
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_X]		= m_lPrCenterX;			//v3.80
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_Y]		= m_lPrCenterY;			//v3.80

	UpdateSearchDieArea(GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE, FALSE);				//v4.47T2
	m_ucDieShape			= CheckDieShape(m_ucGenDieShape);	// save msd

	(*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_X_PIXEL]	= m_lPrCenterOffsetXInPixel;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_Y_PIXEL]	= m_lPrCenterOffsetYInPixel;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_SHAPE]			= m_ucGenDieShape;

	(*psmf)[BPR_GEN_NDIE_DATA][MOVE_EJR_Z_AND_POSTBOND]		= m_bMoveEjrZAndPostBond;

	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_LEARNCALIB]	= m_bDieCalibrated;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBX]		= m_dCalibX;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBY]		= m_dCalibY;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBXY]	= m_dCalibXY;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBYX]	= m_dCalibYX;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBX]	= m_dFFModeCalibX;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBY]	= m_dFFModeCalibY;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBXY]	= m_dFFModeCalibXY;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBYX]	= m_dFFModeCalibYX;

	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_LEARNCALIB2]= m_bDieCalibrated2;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBX2]	= m_dCalibX2;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBY2]	= m_dCalibY2;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBXY2]	= m_dCalibXY2;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBYX2]	= m_dCalibYX2;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIB_1STSTEPSIZE]	= m_lPrCal1stStepSize;

	(*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_X_PIXEL1]		= m_lBondPosOffsetXPixel1;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_Y_PIXEL1]		= m_lBondPosOffsetYPixel1;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_X_PIXEL2]		= m_lBondPosOffsetXPixel2;
	(*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_Y_PIXEL2]		= m_lBondPosOffsetYPixel2;

	//v4.26T1
	if (m_bRelOffsetCheck)		
	{
		if (m_dFovSize < BPR_LF_SIZE)	//FOV must see at least 3x3
			m_bRelOffsetCheck = FALSE;
		//if (pApp->GetCustomerName() != CTM_SEMITEK )
		//	m_bRelOffsetCheck = FALSE;
	}

	
	//For NG Grade 
	CNGGrade::Instance()->WriteData(&(*psmf)[BPR_POSTBOND_DATA]);

	//Postbond data
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_USE_POSTBOND]		= m_bUsePostBond;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_AVERAGE_CHECK]	= m_bAverageAngleCheck;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE_CHECK]	= m_bBinTableAngleCheck;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_PLACEMENT_CHECK]	= m_bPlacementCheck;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_RELOFFSET_CHECK]	= m_bRelOffsetCheck;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_INSPECTION_TYPE]	= m_bDefectCheck;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_EMPTY_CHECK]		= m_bEmptyCheck;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_RT_XYCOMP]		= m_bEnableRTCompXY;	//v3.86
	(*psmf)[BPR_POSTBOND_DATA]["ACG: Learn Epoxy Size"]	= m_lLearnEpoxySize[0];

	if (bUpdateHMIVariable)
	{
		GetHMIVariable();
		GetHMIVisionVariable();		//Update current menu data into BPR structure before saving into MSD file
	}
	
	for (i=0; i<BPR_MAX_BINBLOCK; i++)
	{
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE_ALLOW][i+1]		= m_lGenMaxAngleAllow[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ACC_ANGLE_ALLOW][i+1]	= m_lGenMaxAccAngleAllow[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE][i+1]			= m_dGenMaxAngle[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_AVERAGE][i+1]			= m_dGenAverageAngle[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_SHIFT_ALLOW][i+1]		= m_lGenMaxShiftAllow[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_REL_SHIFT_ALLOW][i+1]		= m_lGenRelDieShiftAllow[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_X_SHIFT][i+1]			= m_dGenDieShiftX[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_Y_SHIFT][i+1]			= m_dGenDieShiftY[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_DEFECT_ALLOW][i+1]	= m_lGenMaxDefectAllow[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_EMPTY_ALLOW][i+1]		= m_lGenMaxEmptyAllow[i];
		(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ACC_EMPTY_ALLOW][i+1]	= m_lGenMaxAccEmptyAllow[i];	//v4.40T9	//Sanan
	}

	//v4.51A16	//SanAn XA
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_CP_X_ALLOW][1]		= m_dMinCpXAllow;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_CP_Y_ALLOW][1]		= m_dMinCpYAllow;
	(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_CP_T_ALLOW][1]		= m_dMinCpTAllow;

	(*psmf)["SaveFromLastState"] = 1;	//New sw uses BondPr.msd to store record IDs instead


	szLog = _T("BPR: SavePrData - ");
	CString szValue;

	for (i=0; i<BPR_MAX_DIE; i++)
	{
		(*psmf)[BPR_DIE_PRID][i+1]	= (LONG)m_ssGenPRSrchID[i];

		//v4.49A7	//Osram
		(*m_psmfSRam)["BondPr"]["Die Record"][i] = (LONG)m_ssGenPRSrchID[i];	
		szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
		szLog = szLog + szValue;

		//v3.74T12	//PLLM special feature
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		//if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
		//{
		//	if (m_lGenSrchDieScore[i] < 70) //Matthew 20181213
		//		m_lGenSrchDieScore[i] = 70;
		//}

		m_lGenLrnFineSrch[i] = min(m_lGenLrnFineSrch[i], 2);
		m_lGenLrnFineSrch[i] = max(m_lGenLrnFineSrch[i], 0);
		(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_PRID][i+1]				= (LONG)m_ssGenPRSrchID[i];
		(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_DIESIZEX][i+1]			= (LONG)m_stGenDieSize[i].x;
		(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_DIESIZEY][i+1]			= (LONG)m_stGenDieSize[i].y;
		(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_LEARNDIE][i+1]			= m_bGenDieLearnt[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRN_NDIE_DEFECT_CHECK][i+1]		= m_bGenSrchEnableDefectCheck[i];
		(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_ALNALGO][i+1]			= m_lGenLrnAlignAlgo[i];
		(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_BKGROUND][i+1]			= m_lGenLrnBackgroud[i];
		//(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_FINESCH][i+1]			= m_lGenLrnFineSrch[i]; //Default value by Matthew 20190410
		//(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_INSPMETHOD][i+1]		= m_lGenLrnInspMethod[i];	//Default value by Matthew 20190410
		(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_INSPRES][i+1]			= m_lGenLrnInspRes[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_ALGNRES][i+1]			= m_lGenSrchAlignRes[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_DEFECTTHRE][i+1]		= m_lGenSrchDefectThres[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_X][i+1]		= m_lGenSrchDieAreaX[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_Y][i+1]		= m_lGenSrchDieAreaY[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_PASSSCORE][i+1]			= m_lGenSrchDieScore[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_GREYLEVEL][i+1]			= m_lGenSrchGreyLevelDefect[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MINCHIPAREA][i+1]		= m_dGenSrchChipArea[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SIGNLEDEFECT][i+1]		= m_dGenSrchSingleDefectArea[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_TOTALDEFECT][i+1]		= m_dGenSrchTotalDefectArea[i];
		(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRN_NDIE_CHIP_CHECK][i+1]		= m_bGenSrchEnableChipCheck[i];
		(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_ZOOM_MODE][i+1]			= m_szGenDieZoomMode[i];
		(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_CALIBRATION][i+1]		= m_bGenDieCalibration[i];
	}

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.49A7

	(*psmf)[BPR_POSTBOND_DATA][BPR_LIGHTING_ONOFF]			= m_bDisableTurnOnOffPrLighting;
	(*psmf)[BPR_POSTBOND_DATA][BPR_TOOLING_STAGE_TO_USE]	= m_ucToolingStageToUse;
	if( m_lISPDownTimeout<1000 )
		m_lISPDownTimeout = 5000;
	(*psmf)[BPR_POSTBOND_DATA][BPR_ISP_DOWN_TIMEOUT]		= m_lISPDownTimeout;
	(*psmf)[BPR_POSTBOND_DATA][BPR_ISP_DOWN_SETTLE_DELAY]	= m_lISPDownSettleDelay;
	(*psmf)[BPR_POSTBOND_DATA]["Bond Epoxy Size"]			= m_ulEpoxySize;

#ifdef NU_MOTION
	if (CMS896AStn::m_bAutoChangeCollet)	//v4.50A15
	{
		(*psmf)[PSPR_DIE_PRID][1]			= (LONG) m_ssPSPRSrchID[0];
		(*psmf)[PSPR_DIE_PRID][2]			= (LONG) m_ssPSPRSrchID[1];
		(*psmf)["AGC"]["C1"]["X"]			= (LONG) m_lAGCC1CenterXInPixel;
		(*psmf)["AGC"]["C1"]["Y"]			= (LONG) m_lAGCC1CenterYInPixel;
		(*psmf)["AGC"]["C2"]["X"]			= (LONG) m_lAGCC2CenterXInPixel;
		(*psmf)["AGC"]["C2"]["Y"]			= (LONG) m_lAGCC2CenterYInPixel;

		(*psmf)["AGC"]["Calibrate"]["X"]	= (DOUBLE) m_dAGCCalibX;	
		(*psmf)["AGC"]["Calibrate"]["Y"]	= (DOUBLE) m_dAGCCalibY;	
	}

#endif

	//Zoom Sensor Data
	(*psmf)[BPR_CAMERA_DATA][BPR_ZOOM_SENSOR][BPR_CONTINUE_ZOOM_FACTOR]		= m_lBprContZoomFactor;
	(*psmf)[BPR_CAMERA_DATA][BPR_ZOOM_SENSOR][BPR_CONTINUOUS_ZOOM_MODE]		= m_bBprContinuousZoom;
	(*psmf)[BPR_CAMERA_DATA][BPR_ZOOM_SENSOR][BPR_NORMAL_ZOOM_FACTOR]		= m_lBprNmlZoomFactor;
    //Check Load/Save Data
	pUtl->UpdateBPRConfig();

	// close config file
    pUtl->CloseBPRConfig();

	//Update Sram string map file
	(*m_psmfSRam)["BondPr"]["UsePostBond"]			= m_bUsePostBond;
	(*m_psmfSRam)["BondPr"]["Die Size X"]			= (LONG)m_stGenDieSize[BPR_NORMAL_DIE].x;
	(*m_psmfSRam)["BondPr"]["Die Size Y"]			= (LONG)m_stGenDieSize[BPR_NORMAL_DIE].y;
	(*m_psmfSRam)["BondPr"]["CursorCenter"]["X"]	= m_lPrCenterX;			//v3.80
	(*m_psmfSRam)["BondPr"]["CursorCenter"]["Y"]	= m_lPrCenterY;			//v3.80
	(*m_psmfSRam)["BondPr"]["PB RTComp"]			= m_bEnableRTCompXY;	//v3.86
	(*m_psmfSRam)["BondPr"]["HWTrigger"]			= m_bUseHWTrigger;		//andrewng //2020-0618

	//Update HIM variable
	m_bDieLearnt			= m_bGenDieLearnt[BPR_NORMAL_DIE];		
	m_lCurNormDieID			= (LONG)m_ssGenPRSrchID[BPR_NORMAL_DIE];

	stPixel.x = m_stGenDieSize[BPR_NORMAL_DIE].x;
	stPixel.y = m_stGenDieSize[BPR_NORMAL_DIE].y;
	ConvertPixelToUnit(stPixel, &m_lCurNormDieSizeX, &m_lCurNormDieSizeY);
	ConvertPixelToDUnit(stPixel, m_dCurNormDieSizeX, m_dCurNormDieSizeY);

	//v4.43T6
	(*m_psmfSRam)["BondPr"]["SearchArea"]["ULX"]	= (LONG) m_stSearchArea.coCorner1.x;
	(*m_psmfSRam)["BondPr"]["SearchArea"]["ULY"]	= (LONG) m_stSearchArea.coCorner1.y;
	(*m_psmfSRam)["BondPr"]["SearchArea"]["LRX"]	= (LONG) m_stSearchArea.coCorner2.x;
	(*m_psmfSRam)["BondPr"]["SearchArea"]["LRY"]	= (LONG) m_stSearchArea.coCorner2.y;

	(*m_psmfSRam)["BondPr"]["Calibration"]["X"]		= m_dCalibX;
	(*m_psmfSRam)["BondPr"]["Calibration"]["Y"]		= m_dCalibY;
	(*m_psmfSRam)["BondPr"]["Calibration"]["XY"] 	= m_dCalibXY;
	(*m_psmfSRam)["BondPr"]["Calibration"]["YX"]	= m_dCalibYX;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["X"]	= m_dFFModeCalibX;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["Y"]	= m_dFFModeCalibY;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["XY"] 	= m_dFFModeCalibXY;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["YX"]	= m_dFFModeCalibYX;
	//For BT2 in MS109
	(*m_psmfSRam)["BondPr"]["Calibration"]["X2"]	= m_dCalibX2;
	(*m_psmfSRam)["BondPr"]["Calibration"]["Y2"]	= m_dCalibY2;
	(*m_psmfSRam)["BondPr"]["Calibration"]["XY2"] 	= m_dCalibXY2;
	(*m_psmfSRam)["BondPr"]["Calibration"]["YX2"]	= m_dCalibYX2;


	//v4.53A21
	(*m_psmfSRam)["BondPr"]["Calibration"]["BT1"]	= m_bDieCalibrated;
	(*m_psmfSRam)["BondPr"]["Calibration"]["BT2"]	= m_bDieCalibrated2;

#ifdef NU_MOTION
	if ( m_bUsePostBond == FALSE )
	{
		ResetRtBHCompXYArrays(FALSE);
		ResetRtBHCompXYArrays(TRUE);
	}
#endif
	return TRUE;
}

BOOL CBondPr::LoadBondPrOption()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
    CStringMapFile  *psmf;
	short	i = 0;	
	
	// open config file
    if (pUtl->LoadBPRConfig() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetBPRConfigFile();

	//Check Load/Save Data
	if ( psmf == NULL )
	{
		return FALSE;
	}

	m_bEnableTotalDefect		= (BOOL)((LONG)(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_TOTAL_DEFECTS]);
	m_bEnableLineDefect			= (BOOL)((LONG)(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_LINE_DEFECT]);
	m_dMinLineDefectLength		= (DOUBLE)(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MIN_LINE_DEFECT_LENGTH];
	m_lMinNumOfLineDefects		= (*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MIN_NUM_OF_LINE_DEFECT];
	m_lMaxNumOfDefects			= (*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MAX_NUM_OF_DEFECTS];
	m_bDisableLightingSetting      = (BOOL)((LONG)(*psmf)[BPR_SRH_NDIE_DATA]["DisableLightingSetting"]);
	if( m_lMinNumOfLineDefects==0 )
		m_lMinNumOfLineDefects = 32767;
	if( m_lMaxNumOfDefects==0 )
		m_lMaxNumOfDefects = 32767;

	//For NG Grade 
	CNGGrade::Instance()->ReadData(&(*psmf)[BPR_POSTBOND_DATA]);

	//Postbond data
	m_bUsePostBond				= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_USE_POSTBOND]);	
	m_bAverageAngleCheck		= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_AVERAGE_CHECK]);
	m_bBinTableAngleCheck		= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE_CHECK]);
	m_bPlacementCheck			= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_PLACEMENT_CHECK]);
	m_bRelOffsetCheck			= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_RELOFFSET_CHECK]);	
	m_bDefectCheck				= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_INSPECTION_TYPE]);
	m_bEmptyCheck				= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_EMPTY_CHECK]);
	m_bEnableRTCompXY			= (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_PB_RT_XYCOMP]);		//v3.86

#ifdef NU_MOTION

	if (!IsMS90())				//v4.59A3	//MS90 may hv Uplook/WafflePad config
	{
		m_bEnableRTCompXY		= TRUE;			//v3.89		//DEFAULT is ENABLE for MS100
	}
	
	if ( (pApp->GetCustomerName() == "Lumileds")	||		//v4.35T4	//MS109
		 //(pApp->GetCustomerName() == "Nichia")		||		//v4.59A34
		 //(pApp->GetCustomerName() == "Finisar")		||		//v4.57A11
		 (pApp->GetCustomerName() == "SeoulSemi") )			//v4.47A1
	{
		m_bEnableRTCompXY = FALSE;
	}

	//v4.58A4	//MS90 PreBond Fcn
	m_bPreBondAlignUseEpoxySearch	= (BOOL)((LONG)(*psmf)[BPR_SRH_NDIE_DATA][BPR_PREBOND_USE_EPOXYSEARCH]);
	m_bCheckFrameAngleByHwMark		= (BOOL)((LONG)(*psmf)[BPR_SRH_NDIE_DATA][BPR_FRAMEALIGN_HW_MARK]);

	//v4.57A11	//Finisar MS90
	BOOL bEnableBHUplookPrFcn	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);
	if (bEnableBHUplookPrFcn)
	{
		//m_bEnableRTCompXY = FALSE;
	}


	//v4.44A2
	//if (m_bMS100EjtXY && m_bEnableMS100EjtXY) 	// bin table offset		//v4.43T8	//v4.44A1
	//{
	//	m_bEnableRTCompXY = FALSE;
	//}
#else
	m_bEnableRTCompXY			= FALSE;		//v3.89		//DEFAULT is DISABLE for MS899
#endif

	for (i=0; i<BPR_MAX_BINBLOCK; i++)
	{
		m_lGenMaxAngleAllow[i]	= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE_ALLOW][i+1];
		m_lGenMaxAccAngleAllow[i]	= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ACC_ANGLE_ALLOW][i+1];
		m_dGenMaxAngle[i]		= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ANGLE][i+1];
		m_dGenAverageAngle[i]	= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_AVERAGE][i+1];
		m_lGenMaxShiftAllow[i]	= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_SHIFT_ALLOW][i+1];
		m_lGenRelDieShiftAllow[i]	= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_REL_SHIFT_ALLOW][i+1];
		m_dGenDieShiftX[i]		= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_X_SHIFT][i+1];
		m_dGenDieShiftY[i]		= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_Y_SHIFT][i+1];
		m_lGenMaxDefectAllow[i]	= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_DEFECT_ALLOW][i+1];
		m_lGenMaxEmptyAllow[i]	= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_EMPTY_ALLOW][i+1];
		m_lGenMaxAccEmptyAllow[i]= (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_ACC_EMPTY_ALLOW][i+1];		//v4.40T9	//Sanan
	}

	//v4.51A16	//SanAn XA
	m_dMinCpXAllow = (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_CP_X_ALLOW][1];
	m_dMinCpYAllow = (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_CP_Y_ALLOW][1];
	m_dMinCpTAllow = (*psmf)[BPR_POSTBOND_DATA][BPR_PB_MAX_CP_T_ALLOW][1];
	
	for (i=0; i<BPR_MAX_DIE; i++)
	{
		m_lGenSrchDieScore[i]			=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_PASSSCORE][i+1];
		m_bGenSrchEnableDefectCheck[i]	=	(BOOL)(LONG)(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRN_NDIE_DEFECT_CHECK][i+1];
		m_lGenSrchDefectThres[i]		=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_DEFECTTHRE][i+1];
		m_lGenSrchGreyLevelDefect[i]	=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_GREYLEVEL][i+1];
		m_dGenSrchChipArea[i]			=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_MINCHIPAREA][i+1];
		m_dGenSrchSingleDefectArea[i]	=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SIGNLEDEFECT][i+1];
		m_dGenSrchTotalDefectArea[i]	=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_TOTALDEFECT][i+1];
		m_bGenSrchEnableChipCheck[i]	=	(BOOL)(LONG)(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRN_NDIE_CHIP_CHECK][i+1];
		m_lGenSrchDieAreaX[i]			=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_X][i+1];
		m_lGenSrchDieAreaY[i]			=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_Y][i+1];

		// Make sure the search area multiplier is at least 2
		if ( m_lGenSrchDieAreaX[i] == 0 )
		{
			m_lGenSrchDieAreaX[i] = 4;
		}

		if ( m_lGenSrchDieAreaY[i] == 0 )
		{
			m_lGenSrchDieAreaY[i] = 4;
		}
		

		//v3.74T12	//PLLM special feature
	//	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
		{
			//if (m_lGenSrchDieScore[i] < 70) //Matthew 20181213
				//m_lGenSrchDieScore[i] = 70;
		}
	}

	if (CNGGrade::Instance()->m_bHaveNGGrade)
	{
		m_lGenSrchDieScore[BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET]	= m_lGenSrchDieScore[BPR_NORMAL_DIE];//CNGGrade::Instance()->m_lSrchNGDieScore;
	}

	// prober
	m_ucToolingStageToUse = (UCHAR)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_TOOLING_STAGE_TO_USE]);
	m_bDisableTurnOnOffPrLighting = (BOOL)((LONG)(*psmf)[BPR_POSTBOND_DATA][BPR_LIGHTING_ONOFF]);
	m_lISPDownTimeout		= (*psmf)[BPR_POSTBOND_DATA][BPR_ISP_DOWN_TIMEOUT];
	m_lISPDownSettleDelay	= (*psmf)[BPR_POSTBOND_DATA][BPR_ISP_DOWN_SETTLE_DELAY];
	m_ulEpoxySize			= (*psmf)[BPR_POSTBOND_DATA]["Bond Epoxy Size"];
	if( m_lISPDownTimeout<1000 )
		m_lISPDownTimeout = 5000;

	pUtl->CloseBPRConfig();

	if (pApp->GetCustomerName() == "Cree")		//v4.46T27
	{
		m_bUsePostBond	= TRUE;
		m_bEmptyCheck	= TRUE;
	}
	if( m_bDisableBT )
		m_bUsePostBond = FALSE;


	(*m_psmfSRam)["BondPr"]["UsePostBond"]			= m_bUsePostBond;
	(*m_psmfSRam)["BondPr"]["PB RTComp"]			= m_bEnableRTCompXY;	//v3.86


	m_lSrchDieScore		= m_lGenSrchDieScore[BPR_NORMAL_DIE];
	m_lSrchDefectThres = m_lGenSrchDefectThres[BPR_NORMAL_DIE];
	m_lSrchGreyLevelDefect = m_lGenSrchGreyLevelDefect[BPR_NORMAL_DIE];
	m_dSrchSingleDefectArea = m_dGenSrchSingleDefectArea[BPR_NORMAL_DIE];
	m_dSrchTotalDefectArea		= m_dGenSrchTotalDefectArea[BPR_NORMAL_DIE];
	m_dSrchChipArea				= m_dGenSrchChipArea[BPR_NORMAL_DIE];
	m_bSrchEnableDefectCheck	= m_bGenSrchEnableDefectCheck[BPR_NORMAL_DIE];
	m_bSrchEnableChipCheck		= m_bGenSrchEnableChipCheck[BPR_NORMAL_DIE];
	
	CNGGrade::Instance()->m_lSrchNGDieScore		= m_lGenSrchDieScore[BPR_NORMAL_DIE];//m_lGenSrchDieScore[BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET]; // BPR Reference 9

	m_lMaxAngleAllow	= m_lGenMaxAngleAllow[0];	
	m_lMaxAccAngleAllow	= m_lGenMaxAccAngleAllow[0];	
	m_lMaxDefectAllow	= m_lGenMaxDefectAllow[0];	
	m_lMaxEmptyAllow	= m_lGenMaxEmptyAllow[0];	
	m_lMaxAccEmptyAllow	= m_lGenMaxAccEmptyAllow[0];	
	m_lMaxShiftAllow	= m_lGenMaxShiftAllow[0];	
	m_lMaxRelShiftAllow	= m_lGenRelDieShiftAllow[0];	
	m_dAverageAngle		= m_dGenAverageAngle[0];	
	m_dMaxAngle			= m_dGenMaxAngle[0];	
	m_dDieShiftX		= m_dGenDieShiftX[0];	
	m_dDieShiftY		= m_dGenDieShiftY[0];	

	//v4.32T1
	if ( m_bRelOffsetCheck )		
	{
		//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		//if (pApp->GetCustomerName() != CTM_SEMITEK )
		//	m_bRelOffsetCheck = FALSE;
	}

	return TRUE;
}


BOOL CBondPr::LoadPrData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	PR_COORD		stPixel;
	short			i = 0;	
	CString			szLog;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bIsManualLoadPkgFile = pApp->IsManualLoadPkgFile();

	// open config file
    if (pUtl->LoadBPRConfig() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetBPRConfigFile();

	//Check Load/Save Data
	if ( psmf == NULL )
	{
		return FALSE;
	}

	m_dFovSize					= (*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_FOV];		//v2.58
	m_dRowFovSize					= (*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_ROW_FOV];		//4.53D01 Save
	m_dColFovSize					= (*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_COL_FOV];		

	m_stSearchArea.coCorner1.x	= (PR_WORD)((LONG)(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHULCX]);
	m_stSearchArea.coCorner1.y	= (PR_WORD)((LONG)(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHULCY]);
	m_stSearchArea.coCorner2.x	= (PR_WORD)((LONG)(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHLRCX]);
	m_stSearchArea.coCorner2.y	= (PR_WORD)((LONG)(*psmf)[BPR_LRN_NDIE_DATA][BPR_SRH_NDIE_SRCHLRCY]);
	m_lSrchDieAreaX				= (*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_X];
	m_lSrchDieAreaY				= (*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_SCRHAREA_Y];

	for (i=0; i<BPR_MAX_LIGHT_GROUP; i++)
	{
		m_lBPRLrnCoaxLightLevel[i] = (*psmf)[BPR_LIGHT_SETTING][i+1][BPR_LRN_COAX_LIGHT_LEVEL];
		m_lBPRLrnRingLightLevel[i] = (*psmf)[BPR_LIGHT_SETTING][i+1][BPR_LRN_RING_LIGHT_LEVEL];
		m_lBPRLrnSideLightLevel[i] = (*psmf)[BPR_LIGHT_SETTING][i+1][BPR_LRN_SIDE_LIGHT_LEVEL];
	}

	m_lBPRExposureTimeLevel		= (*psmf)[BPR_LIGHT_SETTING][BPR_LRN_EXPOSURE_TIME_LEVEL];
	
	//andrewng //2020-0615
	m_bUseHWTrigger				= (BOOL)(LONG) (*psmf)[BPR_GEN_NDIE_DATA]["WPR Use Hardware Trigger"];

	m_lPrCenterX				= (*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_X];		//v3.80
	m_lPrCenterY				= (*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_Y];		//v3.80
	//m_lPrCenterX				= 4096; //hardcode 2019.03.05
	//m_lPrCenterY				= 4096;

	m_lPrCenterOffsetXInPixel	= (*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_X_PIXEL];
	m_lPrCenterOffsetYInPixel	= (*psmf)[BPR_GEN_NDIE_DATA][BPR_CURSOR_CENTER_Y_PIXEL];
	//m_lPrCenterOffsetXInPixel	= 0; //hardcode 2019.03.05
	//m_lPrCenterOffsetYInPixel	= 0;

	m_bMoveEjrZAndPostBond		= (BOOL)(LONG)(*psmf)[BPR_GEN_NDIE_DATA][MOVE_EJR_Z_AND_POSTBOND];

	/*if ( labs(m_lPrCenterX - PR_DEF_CENTRE_X) > 1000 )
	{
		m_lPrCenterX = PR_DEF_CENTRE_X;
	}
	if ( labs(m_lPrCenterY - PR_DEF_CENTRE_Y) > 1000 )
	{
		m_lPrCenterY = PR_DEF_CENTRE_Y;
	}*/

	m_bDieCalibrated			= (BOOL)((LONG)(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_LEARNCALIB]);
	m_dCalibX					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBX];
	m_dCalibY					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBY];
	m_dCalibXY					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBXY];
	m_dCalibYX					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBYX];
	m_dFFModeCalibX				= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBX];
	m_dFFModeCalibY				= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBY];
	m_dFFModeCalibXY			= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBXY];
	m_dFFModeCalibYX			= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_FFMODE_CALIBYX];
	m_bDieCalibrated2			= (BOOL)((LONG)(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_LEARNCALIB2]);
	m_dCalibX2					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBX2];
	m_dCalibY2					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBY2];
	m_dCalibXY2					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBXY2];
	m_dCalibYX2					= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIBYX2];
	m_ucGenDieShape				= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_SHAPE];
	m_ucDieShape				= CheckDieShape(m_ucGenDieShape);	// load msd

	//v4.57A2
	m_lPrCal1stStepSize			= (*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_CALIB_1STSTEPSIZE];
	if (m_lPrCal1stStepSize < 100)
	{
		//normal BT XY with 0.5um encoder:	motor step size = 0.05 / 0.0005 = 100 steps
		//New BT XY with 0.1um encoder:		motor step size = 0.05 / 0.0001 = 500 steps
		m_lPrCal1stStepSize = (LONG) (0.05 / m_dBTXRes);
	}

	m_lBondPosOffsetXPixel1		= (*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_X_PIXEL1];
	m_lBondPosOffsetYPixel1		= (*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_Y_PIXEL1];
	m_lBondPosOffsetXPixel2		= (*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_X_PIXEL2];
	m_lBondPosOffsetYPixel2		= (*psmf)[BPR_GEN_NDIE_DATA][BPR_BOND_POS_OFFSET_Y_PIXEL2];

	m_lLearnEpoxySize[0]		= (*psmf)[BPR_POSTBOND_DATA]["ACG: Learn Epoxy Size"];
	
	m_lNoLastStateFile = (*psmf)["SaveFromLastState"];
	szLog = _T("BPR: LoadPrData - ");
	CString szValue;

	for (i=0; i<BPR_MAX_DIE; i++)
	{
		if (!bIsManualLoadPkgFile)	//v4.53A22
		{
			//andrew: do not overwrite record IDs as updated by auto-assigned by 
			//	PR_DownloadRecordProcessCmd();
			m_ssGenPRSrchID[i]			= (PR_WORD) ((LONG)(*psmf)[BPR_DIE_PRID][i+1]);
			//m_ssGenPRSrchID[i]		= (PR_UWORD)((LONG)(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_PRID][i+1]);
			m_bGenDieLearnt[i]			= (BOOL)(LONG)(*psmf)[BPR_GEN_NDIE_DATA][BPR_GEN_NDIE_LEARNDIE][i+1];
		}

		//v4.49A7	//Osram
		(*m_psmfSRam)["BondPr"]["Die Record"][i] = (LONG)m_ssGenPRSrchID[i];
		szValue.Format("%ld(ID=%ld) ", i, m_ssGenPRSrchID[i]);
		szLog = szLog + szValue;

		m_stGenDieSize[i].x				=	(PR_WORD)((LONG)(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_DIESIZEX][i+1]);
		m_stGenDieSize[i].y				=	(PR_WORD)((LONG)(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_DIESIZEY][i+1]);
		m_lGenLrnAlignAlgo[i]			=	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_ALNALGO][i+1];
		m_lGenLrnBackgroud[i]			=	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_BKGROUND][i+1];
		//m_lGenLrnFineSrch[i]			=	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_FINESCH][i+1];	//Default value by Matthew 20190410
		//m_lGenLrnInspMethod[i]		=	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_INSPMETHOD][i+1];	//Default value by Matthew 20190410
		m_lGenLrnInspRes[i]				=	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_INSPRES][i+1];
		m_lGenSrchAlignRes[i]			=	(*psmf)[BPR_SRH_NDIE_DATA][BPR_SRH_NDIE_ALGNRES][i+1];
		m_szGenDieZoomMode[i]			=	(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_ZOOM_MODE][i+1];
		m_bGenDieCalibration[i]			=	(BOOL)((LONG)(*psmf)[BPR_LRN_NDIE_DATA][BPR_LRN_NDIE_CALIBRATION][i+1]);
		m_lGenLrnFineSrch[i] = min(m_lGenLrnFineSrch[i], 2);
		m_lGenLrnFineSrch[i] = max(m_lGenLrnFineSrch[i], 0);
	}

	if (CMS896AStn::m_bAutoChangeCollet)	//v4.50A15
	{
		m_ssPSPRSrchID[0]		= (PR_WORD)(LONG)(*psmf)[PSPR_DIE_PRID][1];
		m_ssPSPRSrchID[1]		= (PR_WORD)(LONG)(*psmf)[PSPR_DIE_PRID][2];
		m_lAGCC1CenterXInPixel	= (LONG)(*psmf)["AGC"]["C1"]["X"];
		m_lAGCC1CenterYInPixel	= (LONG)(*psmf)["AGC"]["C1"]["Y"];
		m_lAGCC2CenterXInPixel	= (LONG)(*psmf)["AGC"]["C2"]["X"];
		m_lAGCC2CenterYInPixel	= (LONG)(*psmf)["AGC"]["C2"]["Y"];
		m_dAGCCalibX			= (DOUBLE)(*psmf)["AGC"]["Calibrate"]["X"];	
		m_dAGCCalibY			= (DOUBLE)(*psmf)["AGC"]["Calibrate"]["Y"];	
	}

	m_lBprContZoomFactor	= (*psmf)[BPR_CAMERA_DATA][BPR_ZOOM_SENSOR][BPR_CONTINUE_ZOOM_FACTOR];
	m_bBprContinuousZoom	= (BOOL)(LONG)(*psmf)[BPR_CAMERA_DATA][BPR_ZOOM_SENSOR][BPR_CONTINUOUS_ZOOM_MODE];
	m_lBprNmlZoomFactor		= (LONG)(*psmf)[BPR_CAMERA_DATA][BPR_ZOOM_SENSOR][BPR_NORMAL_ZOOM_FACTOR];

    pUtl->CloseBPRConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.49A7

	if (m_lNoLastStateFile <= 0)
	{
		//v4.53A22
		//For compatability only; very old sw uses LastState.MSD to store
		//PR records; current sw uses BondPr.msd instead
		LoadRecordID();
	}

	//Update Sram string map file
	(*m_psmfSRam)["BondPr"]["Die Size X"]			= (LONG) m_stGenDieSize[BPR_NORMAL_DIE].x;
	(*m_psmfSRam)["BondPr"]["Die Size Y"]			= (LONG) m_stGenDieSize[BPR_NORMAL_DIE].y;
	(*m_psmfSRam)["BondPr"]["CursorCenter"]["X"]	= m_lPrCenterX;
	(*m_psmfSRam)["BondPr"]["CursorCenter"]["Y"]	= m_lPrCenterY;

	//Update HMI variable
	m_lLrnAlignAlgo		= m_lGenLrnAlignAlgo[BPR_NORMAL_DIE];
	m_lLrnBackground	= m_lGenLrnBackgroud[BPR_NORMAL_DIE];
	m_lLrnFineSrch		= m_lGenLrnFineSrch[BPR_NORMAL_DIE];
	m_lLrnInspMethod	= m_lGenLrnInspMethod[BPR_NORMAL_DIE];
	m_lLrnInspRes		= m_lGenLrnInspRes[BPR_NORMAL_DIE];
	m_lSrchAlignRes		= m_lGenSrchAlignRes[BPR_NORMAL_DIE];
	m_bDieLearnt		= m_bGenDieLearnt[BPR_NORMAL_DIE];		
	m_lCurNormDieID		= (LONG)m_ssGenPRSrchID[BPR_NORMAL_DIE];

	//m_dSrchDieAreaX		= m_lSrchDieAreaX * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
	//m_dSrchDieAreaY		= m_lSrchDieAreaY * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60	
	m_lSrchDieAreaX		= m_lGenSrchDieAreaX[0];
	m_lSrchDieAreaY		= m_lGenSrchDieAreaY[0];
	m_dSrchDieAreaX		= m_lSrchDieAreaX * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
	m_dSrchDieAreaY		= m_lSrchDieAreaY * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
			
	stPixel.x = m_stGenDieSize[BPR_NORMAL_DIE].x;
	stPixel.y = m_stGenDieSize[BPR_NORMAL_DIE].y;

	ConvertPixelToUnit(stPixel, &m_lCurNormDieSizeX, &m_lCurNormDieSizeY);
	ConvertPixelToDUnit(stPixel, m_dCurNormDieSizeX, m_dCurNormDieSizeY);

	//v4.43T6
	(*m_psmfSRam)["BondPr"]["SearchArea"]["ULX"]	= (LONG) m_stSearchArea.coCorner1.x;
	(*m_psmfSRam)["BondPr"]["SearchArea"]["ULY"]	= (LONG) m_stSearchArea.coCorner1.y;
	(*m_psmfSRam)["BondPr"]["SearchArea"]["LRX"]	= (LONG) m_stSearchArea.coCorner2.x;
	(*m_psmfSRam)["BondPr"]["SearchArea"]["LRY"]	= (LONG) m_stSearchArea.coCorner2.y;

	(*m_psmfSRam)["BondPr"]["Calibration"]["X"]		= m_dCalibX;
	(*m_psmfSRam)["BondPr"]["Calibration"]["Y"]		= m_dCalibY;
	(*m_psmfSRam)["BondPr"]["Calibration"]["XY"] 	= m_dCalibXY;
	(*m_psmfSRam)["BondPr"]["Calibration"]["YX"]	= m_dCalibYX;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["X"]	= m_dFFModeCalibX;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["Y"]	= m_dFFModeCalibY;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["XY"] 	= m_dFFModeCalibXY;
	(*m_psmfSRam)["BondPr"]["FFModeCalibration"]["YX"]	= m_dFFModeCalibYX;

	(*m_psmfSRam)["BondPr"]["Calibration"]["X2"]	= m_dCalibX2;
	(*m_psmfSRam)["BondPr"]["Calibration"]["Y2"]	= m_dCalibY2;
	(*m_psmfSRam)["BondPr"]["Calibration"]["XY2"] 	= m_dCalibXY2;
	(*m_psmfSRam)["BondPr"]["Calibration"]["YX2"]	= m_dCalibYX2;
	//v4.53A21
	(*m_psmfSRam)["BondPr"]["Calibration"]["BT1"]	= m_bDieCalibrated;
	(*m_psmfSRam)["BondPr"]["Calibration"]["BT2"]	= m_bDieCalibrated2;

	(*m_psmfSRam)["BondPr"]["HWTrigger"]	= m_bUseHWTrigger;		//andrewng //2020-0618

	PR_RSIZE		stFov;
	PR_OPTIC		stOptic;
	PR_COMMON_RPY	stComRpy;
	stFov.x = (PR_REAL)(m_dBTXRes * m_dCalibX * 8192);	// mm
	stFov.y = (PR_REAL)(m_dBTYRes * m_dCalibY * 8192);	// mm
	// Set FOV to Wafer PR
	PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);
	PR_SetFov(&stFov, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);

	//v4.32T1
	if (m_bRelOffsetCheck)		
	{
		if (m_dFovSize < BPR_LF_SIZE)	//FOV must see at least 3x3
			m_bRelOffsetCheck = FALSE;
	}

	return TRUE;
}

BOOL CBondPr::LoadRecordID(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	
	//v4.53A9
	if (pUtl->LoadLastState("BPR: LoadRecordID") == FALSE) 	// open LastState file
		return FALSE;

    psmf = pUtl->GetLastStateFile("BPR: LoadRecordID");		// get file pointer
	if ( psmf != NULL )
	{
		for (INT i=0; i<BPR_MAX_DIE; i++)
		{
			m_ssGenPRSrchID[i] = (PR_WORD)((LONG)(*psmf)[BPR_DIE_PRID][i+1]);
			(*m_psmfSRam)["BondPr"]["Die Record"][i] = (LONG)m_ssGenPRSrchID[i];	//v4.49A7	//Osram
		}
		
	}

	// close config file
	pUtl->CloseLastState("BPR: LoadRecordID");
	return TRUE;
}


LONG CBondPr::GetDieNo(const LONG lSelectDieType, const LONG lRefDieNo)
{
	LONG	lDieNo = 0;

	if (lSelectDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + lRefDieNo;
	}
	else if (lSelectDieType == BPR_REFERENCE_DIE)
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lRefDieNo;
	}

	return lDieNo;
}


VOID CBondPr::UpdateHMIVisionVariable(VOID)
{
	LONG	lDieNo = 0;
	
	if (m_szLrnDieType == BPR_HMI_NORM_DIE)
	{
		m_bSelectDieType = BPR_NORMAL_DIE;
	}
	else
	{
		m_bSelectDieType = BPR_REFERENCE_DIE;
	}


	if ( m_bSelectDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + m_lCurRefDieNo;
	}
	else if ( m_bSelectDieType == BPR_REFERENCE_DIE)
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + m_lCurRefDieNo;
	}
	//Klocwork	//v4.04
	//else
	//{
	//	lDieNo = 0;
	//}

	m_ucDieShape				= CheckDieShape(m_ucGenDieShape);	// update hmi
	m_lLrnAlignAlgo				= m_lGenLrnAlignAlgo[lDieNo];
	m_lLrnBackground			= m_lGenLrnBackgroud[lDieNo];
	m_lLrnFineSrch				= m_lGenLrnFineSrch[lDieNo];
	m_lLrnInspMethod			= m_lGenLrnInspMethod[lDieNo];
	m_lLrnInspRes				= m_lGenLrnInspRes[lDieNo];
	m_lSrchAlignRes				= m_lGenSrchAlignRes[lDieNo];
	m_lSrchDefectThres			= m_lGenSrchDefectThres[lDieNo];
	m_lSrchDieAreaX				= m_lGenSrchDieAreaX[lDieNo];
	m_lSrchDieAreaY				= m_lGenSrchDieAreaY[lDieNo];
	m_lSrchDieScore				= m_lGenSrchDieScore[lDieNo];
	CNGGrade::Instance()->m_lSrchNGDieScore	= m_lGenSrchDieScore[BPR_NORMAL_DIE];//m_lGenSrchDieScore[BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET]; // BPR Reference 9
	m_lSrchGreyLevelDefect		= m_lGenSrchGreyLevelDefect[lDieNo];
	m_dSrchSingleDefectArea		= m_dGenSrchSingleDefectArea[lDieNo];
	m_dSrchTotalDefectArea		= m_dGenSrchTotalDefectArea[lDieNo];
	m_dSrchChipArea				= m_dGenSrchChipArea[lDieNo];
	m_bSrchEnableDefectCheck	= m_bGenSrchEnableDefectCheck[lDieNo];
	m_bSrchEnableChipCheck		= m_bGenSrchEnableChipCheck[lDieNo];
	m_dSrchDieAreaX				= m_lSrchDieAreaX * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60
	m_dSrchDieAreaY				= m_lSrchDieAreaY * BPR_SEARCHAREA_SHIFT_FACTOR;	//v2.60

	m_lBPRLrnCoaxLightHmi		= m_lBPRLrnCoaxLightLevel[lDieNo];
	m_lBPRLrnRingLightHmi		= m_lBPRLrnRingLightLevel[lDieNo];
	m_lBPRLrnSideLightHmi		= m_lBPRLrnSideLightLevel[lDieNo];
}


VOID CBondPr::GetHMIVisionVariable(VOID)
{
	LONG	lDieNo = 0;

	if (m_szLrnDieType == BPR_HMI_NORM_DIE)
	{
		m_bSelectDieType = BPR_NORMAL_DIE;
	}
	else
	{
		m_bSelectDieType = BPR_REFERENCE_DIE;
	}


	if ( m_bSelectDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + m_lCurRefDieNo;
	}
	else if ( m_bSelectDieType == BPR_REFERENCE_DIE)
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + m_lCurRefDieNo;
	}
	//Klocwork	//v4.04
	//else
	//{
	//	lDieNo = 0;
	//}

	if (m_dSrchSingleDefectArea > m_dSrchTotalDefectArea)
	{
		m_dSrchSingleDefectArea = m_dSrchTotalDefectArea;
	}

	m_ucGenDieShape						= CheckDieShape(m_ucDieShape);	// get from hmi
	m_lGenLrnAlignAlgo[lDieNo]			= m_lLrnAlignAlgo; 
	m_lGenLrnBackgroud[lDieNo]			= m_lLrnBackground;
	m_lGenLrnFineSrch[lDieNo]			= m_lLrnFineSrch;
	m_lGenLrnInspMethod[lDieNo]			= m_lLrnInspMethod;
	m_lGenLrnInspRes[lDieNo]			= m_lLrnInspRes;
	m_lGenSrchAlignRes[lDieNo]			= m_lSrchAlignRes;
	m_lGenSrchDefectThres[lDieNo]		= m_lSrchDefectThres;
	m_lGenSrchDieAreaX[lDieNo]			= m_lSrchDieAreaX;
	m_lGenSrchDieAreaY[lDieNo]			= m_lSrchDieAreaY;
	m_lGenSrchDieScore[lDieNo]			= m_lSrchDieScore;
	if (CNGGrade::Instance()->m_bHaveNGGrade)
	{
		m_lGenSrchDieScore[BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET]	= m_lGenSrchDieScore[BPR_NORMAL_DIE];//CNGGrade::Instance()->m_lSrchNGDieScore; // BPR Reference 9
	}
	m_lGenSrchGreyLevelDefect[lDieNo]	= m_lSrchGreyLevelDefect;
	m_dGenSrchSingleDefectArea[lDieNo]	= m_dSrchSingleDefectArea;
	m_dGenSrchTotalDefectArea[lDieNo]	= m_dSrchTotalDefectArea;
	m_dGenSrchChipArea[lDieNo]			= m_dSrchChipArea;
	m_bGenSrchEnableDefectCheck[lDieNo]	= m_bSrchEnableDefectCheck;
	m_bGenSrchEnableChipCheck[lDieNo]	= m_bSrchEnableChipCheck;

	m_lBPRLrnCoaxLightLevel[lDieNo]		= m_lBPRLrnCoaxLightHmi;
	m_lBPRLrnRingLightLevel[lDieNo]		= m_lBPRLrnRingLightHmi;
	m_lBPRLrnSideLightLevel[lDieNo]		= m_lBPRLrnSideLightHmi;
}

VOID CBondPr::ConvertUnit(int siStepX, int siStepY, double *dUnitX, double *dUnitY)
{
	double	dStep_2_XMM		= 0.0005;
	double	dStep_2_YMM		= 0.0005;
	double	dStep_2_XMIL	= 0.0197;
	double	dStep_2_YMIL	= 0.0197;


	dStep_2_XMM		= m_dBTXRes;	//0.00125;   
	dStep_2_YMM		= m_dBTYRes;	//0.0005;
	dStep_2_XMIL	= (dStep_2_XMM * 1000.0) / 25.4;					//0.0492;    
	dStep_2_YMIL	= (dStep_2_YMM * 1000.0) / 25.4;					//0.0197;


	//convert to mil 
	*dUnitX = (double)(siStepX) * dStep_2_XMIL;
	*dUnitY = (double)(siStepY) * dStep_2_YMIL;
}

VOID CBondPr::ConvertUnitToUm(int siStepX, int siStepY, double *dumX, double *dumY)
{
	double	dStep_2_XMM		= 0.0005;
	double	dStep_2_YMM		= 0.0005;
	double	dStep_2_XMIL	= 0.5;
	double	dStep_2_YMIL	= 0.5;


	dStep_2_XMM		= m_dBTXRes;	//0.00125;   
	dStep_2_YMM		= m_dBTYRes;	//0.0005;
	dStep_2_XMIL	= (dStep_2_XMM * 1000.0); 
	dStep_2_YMIL	= (dStep_2_YMM * 1000.0);

	//convert to um 
	*dumX = (double)(siStepX) * dStep_2_XMIL;
	*dumY = (double)(siStepY) * dStep_2_YMIL;
}


VOID CBondPr::SelectBondCamera(VOID)
{
	IPC_CServiceMessage stMsg;

	int nConvID = 0;

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectBondCamera", stMsg);

	// Get the reply
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			GetGeneralLighting(stMsg);		// Get the general light
			GetGExposureTime(stMsg);		//v3.30T1
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

VOID CBondPr::SelectPostSealCamera(VOID)		//v4.50A13
{
	IPC_CServiceMessage stMsg;
	//4.53D19

	int nConvID = 0;
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectPostSealCamera", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			//GetGeneralLighting(stMsg);		// Get the general light
			//GetGExposureTime(stMsg);		//v3.30T1
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

VOID CBondPr::SelectLiveVideoMode(VOID)		//v4.50A13
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysLiveMode", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			//GetGeneralLighting(stMsg);		// Get the general light
			//GetGExposureTime(stMsg);		//v3.30T1
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

VOID CBondPr::SelectAutoBondMode(VOID)		//v4.50A13
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetPRSysBondMode", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			GetGeneralLighting(stMsg);		// Get the general light
			GetGExposureTime(stMsg);		//v3.30T1
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

BOOL CBondPr::BTMoveToAGCUPLUploadPos()
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCUPLUploadPos"), FALSE))
	{
		CString szErr = "BPR: Bin Table is not able to move to Upload UPLOOK position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}

BOOL CBondPr::BTMoveToUpLookPos(BOOL bBHZ2)		//v4.50A13
{
	if (!SendGeneralRequest(BIN_TABLE_STN, _T("BTMoveToAGCUpLookPos"), bBHZ2))
	{
		CString szErr = "BPR: Bin Table is not able to move to UPLOOK position";
		HmiMessage_Red_Yellow(szErr);
		SetErrorMessage(szErr);
		return FALSE;
	}
	return TRUE;
}

VOID CBondPr::OutText(char *pMessage, PR_COORD stTxtCoord, PR_COLOR eTxtColor)
{
	// Display a text
	PR_DISPLAY_CHAR_CMD	stDispCmd;
	PR_DISPLAY_CHAR_RPY	stDispRpy;

	stDispCmd.tcoPosition.x = stTxtCoord.x;
	stDispCmd.tcoPosition.y = stTxtCoord.y;
	stDispCmd.emColor = eTxtColor;
	stDispCmd.uwNChar = (PR_UWORD)strlen(pMessage);
	stDispCmd.pubChar = (PR_UBYTE*)pMessage;
	PR_DisplayCharCmd(&stDispCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stDispRpy);
}


VOID CBondPr::DrawRectangleBox(PR_COORD stCorner1, PR_COORD stCorner2, PR_COLOR ssColor)
{
	PR_DRAW_RECT_CMD		stRectCmd;
	PR_DRAW_RECT_RPY		stRectRpy;

	stRectCmd.emColor = (PR_COLOR)(ssColor);
	stRectCmd.stWin.coCorner1.x = (PR_WORD)(stCorner1.x);
	stRectCmd.stWin.coCorner1.y = (PR_WORD)(stCorner1.y);
	stRectCmd.stWin.coCorner2.x = (PR_WORD)(stCorner2.x);
	stRectCmd.stWin.coCorner2.y = (PR_WORD)(stCorner2.y);

	PR_DrawRectCmd(&stRectCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stRectRpy);
}


VOID CBondPr::DrawRectangleDieSize(PR_COLOR ssColor, BOOL bDieType, LONG lInputDieNo)
{
	LONG lDieNo = 0;
	PR_COORD stCorner1; 
	PR_COORD stCorner2;

	if (lInputDieNo <= 0)
	{
		lInputDieNo = 1;
	}
	
	if (bDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	}
	

	if (bDieType == BPR_REFERENCE_DIE)
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	}

	stCorner1.x = (PR_WORD)m_lPrCenterX - (m_stGenDieSize[lDieNo].x / 2);
    stCorner1.y = (PR_WORD)m_lPrCenterY - (m_stGenDieSize[lDieNo].y / 2);
    stCorner2.x = (PR_WORD)m_lPrCenterX + (m_stGenDieSize[lDieNo].x / 2);
    stCorner2.y = (PR_WORD)m_lPrCenterY + (m_stGenDieSize[lDieNo].y / 2);

	DrawRectangleBox(stCorner1, stCorner2, ssColor);
}


VOID CBondPr::DrawAndEraseCursor(PR_COORD stStartPos, unsigned char ucCorner, BOOL bDraw)
{
	PR_COORD				stLineLength;
	PR_COORD				coPoint1, coPoint2;

	stLineLength.x = (PR_WORD)_round(50 * PR_SCALE_FACTOR);
	stLineLength.y = (PR_WORD)_round(50 * PR_SCALE_FACTOR); 

	if (ucCorner == PR_LOWER_RIGHT) 	
	{
		stLineLength.x = -stLineLength.x;
		stLineLength.y = -stLineLength.y; 
	}

	coPoint1 = stStartPos;
	coPoint2 = stStartPos; 

	for (short i = 0; i < 2; i++)
	{
		coPoint2 = stStartPos; 
		if (i == 0)	//Draw Horz Line
		{
			coPoint2.x += stLineLength.x;
		}	
		else		//Draw Vert Line
		{
			coPoint2.y += stLineLength.y;
		}
		m_pPrGeneral->PRDrawLine(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, coPoint1, coPoint2, (bDraw == 1) ? PR_COLOR_GREEN : PR_COLOR_TRANSPARENT);
	}
}


VOID CBondPr::DrawAndEraseLine(PR_COORD stStartPos, PR_COORD stEndPos, BOOL bDraw)
{
	m_pPrGeneral->PRDrawLine(MS899_GEN_SEND_ID, MS899_GEN_RECV_ID, stStartPos, stEndPos, (bDraw == 1) ? PR_COLOR_GREEN : PR_COLOR_TRANSPARENT);
}


VOID CBondPr::CheckFFModeCalib()
{
	double dRatio = 4;
	if (m_stGenDieSize[BPR_NORMAL_PR_FF_MODE_INDEX3 - 1].x > 10)
	{
		dRatio = (double)m_stGenDieSize[BPR_NORMAL_PR_DIE_INDEX1 - 1].x / (double)m_stGenDieSize[BPR_NORMAL_PR_FF_MODE_INDEX3 - 1].x;
	}
	if ((dRatio > 5) || (dRatio < 0.5))
	{
		dRatio = 4;
	}

	if ((fabs(m_dFFModeCalibX) > fabs(m_dCalibX * (dRatio + 1))) ||
		(fabs(m_dFFModeCalibX) < 0.0000001))
	{
		m_dFFModeCalibX = m_dCalibX * dRatio;
		m_dFFModeCalibXY = m_dCalibXY * dRatio;
	}

	if ((fabs(m_dFFModeCalibY) > fabs(m_dCalibY * (dRatio + 1))) ||
		(fabs(m_dFFModeCalibY) < 0.0000001))
	{
		m_dFFModeCalibY = m_dCalibY * dRatio;
		m_dFFModeCalibYX = m_dCalibYX * dRatio;
	}
}


DOUBLE CBondPr::GetCalibX()
{
	if (IsSensorZoomFFMode())
	{
		//CheckFFModeCalib();//2019.05.01 Matt:as calib already learn in learn die ff mode. no need to calculate here
		return m_dFFModeCalibX;
	}

	return m_dCalibX;
}


DOUBLE CBondPr::GetCalibXY()
{
	if (IsSensorZoomFFMode())
	{
		return m_dFFModeCalibXY;
	}

	return m_dCalibXY;
}

DOUBLE CBondPr::GetCalibY()
{
	if (IsSensorZoomFFMode())
	{
		//CheckFFModeCalib();//2019.05.01 Matt:as calib already learn in learn die ff mode. no need to calculate here
		return m_dFFModeCalibY;
	}

	return m_dCalibY;
}

DOUBLE CBondPr::GetCalibYX()
{
	if (IsSensorZoomFFMode())
	{
		return m_dFFModeCalibYX;
	}

	return m_dCalibYX;
}


DOUBLE CBondPr::GetScanCalibX() //4.51D20
{
	DOUBLE dCalib = GetCalibX();
	//if( IsEnableZoom() )
	//{
	//	dCalib = m_stZoomView.m_dScanCalibXX;
	//}

	return dCalib;
}

DOUBLE CBondPr::GetScanCalibY() //4.51D20
{
	DOUBLE dCalib = GetCalibY();
	//if( IsEnableZoom() )
	//{
	//	dCalib = m_stZoomView.m_dScanCalibYY;
	//}

	return dCalib;
}

DOUBLE CBondPr::GetScanCalibXY() //4.51D20
{
	DOUBLE dCalib = GetCalibXY();
	//if( IsEnableZoom() )
	//{
	//	dCalib = m_stZoomView.m_dScanCalibXY;
	//}

	return dCalib;
}

DOUBLE CBondPr::GetScanCalibYX() //4.51D20
{
	DOUBLE dCalib = GetCalibYX();
	//if( IsEnableZoom() )
	//{
	//	dCalib = m_stZoomView.m_dScanCalibYX;
	//}

	return dCalib;
}

VOID CBondPr::ConvertMotorStepToPixel(int siStepX, int siStepY, PR_COORD &stPixel)
{
	DOUBLE dStepX = siStepX;
	DOUBLE dStepY = siStepY;
	if (GetCalibX() != 0)
	{
		stPixel.x = (PR_WORD)(dStepX / GetCalibX());
	}
	if (GetCalibY() != 0)
	{
		stPixel.y = (PR_WORD)(dStepY / GetCalibY());
	}
}

VOID CBondPr::ConvertPixelToUM(const LONG lPixelX, const LONG lPixelY, double &dXum, double &dYum)
{
	PR_COORD stPixel;

	stPixel.x = (PR_WORD)lPixelX;
	stPixel.y = (PR_WORD)lPixelY;
	ConvertPixelToUM(stPixel, dXum, dYum);
}

VOID CBondPr::ConvertPixelToUM(PR_COORD stPixel, double &dXum, double &dYum)
{
	int siStepX, siStepY;
	ConvertPixelToMotorStep(stPixel, &siStepX, &siStepY);

	dXum = m_dBTResolution_UM_CNT * siStepX;
	dYum = m_dBTResolution_UM_CNT * siStepY;
}

VOID CBondPr::ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY)
{
	*siStepX = (int)((DOUBLE)stPixel.x * GetCalibX() + (DOUBLE)stPixel.y * GetCalibXY());
	*siStepY = (int)((DOUBLE)stPixel.y * GetCalibY() + (DOUBLE)stPixel.x * GetCalibYX());
}

VOID CBondPr::ConvertPixelToMotorStep2(PR_COORD stPixel, int *siStepX, int *siStepY)
{
	*siStepX = (int)((DOUBLE)stPixel.x * m_dCalibX2 + (DOUBLE)stPixel.y * m_dCalibXY2);
    *siStepY = (int)((DOUBLE)stPixel.y * m_dCalibY2 + (DOUBLE)stPixel.x * m_dCalibYX2);
}


VOID CBondPr::ConvertPixelToUnit(PR_COORD stPixel, LONG *lUnitX, LONG *lUnitY, BOOL bUseUm)
{
	int		siStepX;
	int		siStepY;


	//1st Convert into motor step 
	ConvertPixelToMotorStep(stPixel, &siStepX, &siStepY);

	if (bUseUm)		//v4.59A16	//Renesas MS90
	{
		//Convert motor into um	
		*lUnitX = (LONG)(0.5 + m_dBTXRes * (DOUBLE)siStepX );
		*lUnitY = (LONG)(0.5 + m_dBTYRes * (DOUBLE)siStepY );
	}
	else
	{
		//Convert motor into mil	
		*lUnitX = (LONG)(0.5 + m_dBTXRes * (DOUBLE)siStepX * 1000 / 25.4);
		*lUnitY = (LONG)(0.5 + m_dBTYRes * (DOUBLE)siStepY * 1000 / 25.4);
	}
}

VOID CBondPr::ConvertPixelToDUnit(PR_COORD stPixel, DOUBLE& dUnitX, DOUBLE& dUnitY, BOOL bUseUm)
{
	int		siStepX;
	int		siStepY;

	//1st Convert into motor step 
	ConvertPixelToMotorStep(stPixel, &siStepX, &siStepY);

	if (bUseUm)		//v4.59A16	//Renesas MS90
	{
		//Convert motor into um	
		dUnitX = (DOUBLE)(0.5 + m_dBTXRes * (DOUBLE)siStepX * 1000.0 );
		dUnitY = (DOUBLE)(0.5 + m_dBTYRes * (DOUBLE)siStepY * 1000.0 );
	}
	else
	{
		//Convert motor into mil	
		dUnitX = (DOUBLE)(0.5 + m_dBTXRes * (DOUBLE)siStepX * 1000.0 / 25.4);
		dUnitY = (DOUBLE)(0.5 + m_dBTYRes * (DOUBLE)siStepY * 1000.0 / 25.4);
	}
}

DOUBLE CBondPr::ConvertDUnitToPixel(CONST DOUBLE dUnit)
{
	DOUBLE dMotorStep	= dUnit / m_dBTXRes / 1000.0 * 25.4;
	DOUBLE dCalibX = GetCalibX();
	if (fabs(dCalibX) > 0.0000001)	// divide by zero
	{
		return  dMotorStep / dCalibX;
	}

	return 0;
}

BOOL CBondPr::VerifyPRRegion(PR_WIN *stInputArea)
{
	return m_pPrGeneral->VerifyPRRegion(stInputArea, PR_MAX_WIN_ULC_X - 200, PR_MAX_WIN_ULC_Y - 200, PR_MAX_WIN_LRC_X + 200, PR_MAX_WIN_LRC_Y + 200);
}

VOID CBondPr::CalculateDieCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY)
{
	PR_COORD	stRelMove;

	stRelMove.x = (PR_WORD) m_lPrCenterX - stDieOffset.x;
	stRelMove.y = (PR_WORD) m_lPrCenterY - stDieOffset.y;
	ConvertPixelToMotorStep(stRelMove, siStepX, siStepY);
}


VOID CBondPr::MoveBinTable(int siXAxis, int siYAxis, BOOL bUseBT2)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} RELPOS;

	RELPOS stPos;

	int nConvID = 0;

	stPos.lX = siXAxis;
	stPos.lY = siYAxis;

	stMsg.InitMessage(sizeof(RELPOS), &stPos);

	if (bUseBT2)
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "XY2_MoveToCmd", stMsg);
	else
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "XY_MoveToCmd", stMsg);

	// Get the reply
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID,stMsg);
}


VOID CBondPr::IndexBinTable(ULONG ulIndexNo)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;


	stMsg.InitMessage(sizeof(ULONG), &ulIndexNo);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "MoveToNewIndex", stMsg);

	// Get the reply
	m_comClient.ScanReplyForConvID(nConvID, 5000);
	m_comClient.ReadReplyForConvID(nConvID,stMsg);
}


VOID CBondPr::GetBinTableEncoder(int *siXAxis, int *siYAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} ENCVAL;

	ENCVAL stEnc;

	int nConvID = 0;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GetEncoderCmd", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
			*siXAxis = stEnc.lX;
			*siYAxis = stEnc.lY;
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

VOID CBondPr::GetBinTable2Encoder(int *siXAxis, int *siYAxis)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} ENCVAL;

 	ENCVAL stEnc;

	int nConvID = 0;

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GetEncoderCmd2", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
			*siXAxis = stEnc.lX;
			*siYAxis = stEnc.lY;
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

VOID CBondPr::SetBinTableJoystick(BOOL bState)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stMsg.InitMessage(sizeof(BOOL), &bState);

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetMouseControlCmd", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bState);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}
	else
	{
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetJoystickCmd", stMsg);
	
		// Get the reply
		m_comClient.ScanReplyForConvID(nConvID, 5000);
		m_comClient.ReadReplyForConvID(nConvID,stMsg);
	}
}


VOID CBondPr::ManualDieCompenate(PR_COORD stDieOffset, PR_REAL fDieRotate)
{
	int			siStepX;
	int			siStepY;
	int			siOrigX;
	int			siOrigY;

	CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	CalculateNDieOffsetXY(siStepX, siStepY);
	GetBinTableEncoder(&siOrigX, &siOrigY);

	LONG lPrX = 0;
	LONG lPrY = 0;

	BOOL bBinTableThetaCorrection = (BOOL)(LONG)(*m_psmfSRam)["WaferPr"]["EnableBinTableThetaCorrection"];
	if (bBinTableThetaCorrection)
	{
		BprCompensateBinRotateDie(siOrigX + siStepX, siOrigY + siStepY, 0, (DOUBLE) fDieRotate);
	}
	else
	{
		MoveBinTable(siOrigX + siStepX, siOrigY + siStepY);
	}
}


BOOL CBondPr::BprCompensateBinRotateDie(LONG lDieEncX, LONG lDieEncY, LONG lDieEncT, DOUBLE dDieRotate)
{
	IPC_CServiceMessage stMsg;

	typedef struct {
		LONG	lX;
		LONG	lY;
		LONG	lT;
		DOUBLE	dAngle;
	} ABSPOS;
	ABSPOS	stPos;

	stPos.lX = lDieEncX;//Die Enc Position X
	stPos.lY = lDieEncY;//Die Enc Position Y
	stPos.lT = lDieEncT;//Die Enc Position T
	stPos.dAngle = dDieRotate; //Degree

	stMsg.InitMessage(sizeof(ABSPOS), &stPos);

	BOOL bResult = TRUE;
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "RotateBinAngularDie", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return TRUE;
}


LONG CBondPr::FindDieCalibration(BOOL bNormalDie)
{
	PR_UWORD		usDieType;
	PR_COORD		stDiePosition[2]; 
	PR_COORD		stDieShiftX,stDieShiftY; 
	PR_COORD		stXPixelMove; 
	PR_COORD		stYPixelMove; 
	PR_WIN			stOrgSearchArea = m_stSearchArea;
	PR_WIN			stDieSizeArea;
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;

	int				siSrchMargin = (32 * PR_SCALE_FACTOR);
	int				siTempData1 = 0;
	int				siTempData2 = 0;
	short			ssCycle;			
	short			ssSign = 1;
	short			ssCount = 0;
	float			fTemp = 0.0;
	BOOL			bUpdateSearchArea = FALSE;

	LONG lRefDieNo = BPR_GEN_RDIE_OFFSET + 1;
	
	typedef struct 
	{
		int XPos;
		int YPos;
	} MOTORDATA;

	MOTORDATA	stDestX,stDestY;
	MOTORDATA	stLastPos, stTempPos;
	MOTORDATA	stTablePos[2];
	MOTORDATA	stXMotorMove, stYMotorMove;


	//Update Search area
	if (bNormalDie)
	{
		LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
		m_stSearchArea.coCorner1.x = (PR_WORD)m_lPrCenterX - (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x);
		m_stSearchArea.coCorner1.y = (PR_WORD)m_lPrCenterY - (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y);
		m_stSearchArea.coCorner2.x = (PR_WORD)m_lPrCenterX + (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x);
		m_stSearchArea.coCorner2.y = (PR_WORD)m_lPrCenterY + (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y);
		VerifyPRRegion(&m_stSearchArea);
		stDieSizeArea = m_stSearchArea;
		DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_GREEN);
	}
	else
	{
		PR_WIN stWnd;
		//stWnd.coCorner1.x	= PR_MAX_WIN_ULC_X + 100;
		//stWnd.coCorner1.y	= PR_MAX_WIN_ULC_Y + 100;
		//stWnd.coCorner2.x	= PR_MAX_WIN_LRC_X - 100;
		//stWnd.coCorner2.y	= PR_MAX_WIN_LRC_Y - 100;
		//v3.85
		stWnd.coCorner1.x	= (PR_WORD)m_lPrCenterX - (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x);
		stWnd.coCorner1.y	= (PR_WORD)m_lPrCenterY - (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y);
		stWnd.coCorner2.x	= (PR_WORD)m_lPrCenterX + (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x);
		stWnd.coCorner2.y	= (PR_WORD)m_lPrCenterY + (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y);

		VerifyPRRegion(&stWnd);
		stDieSizeArea = stWnd;
		DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, PR_COLOR_GREEN);
	}

	//Store Current position	
	GetBinTableEncoder(&stLastPos.XPos, &stLastPos.YPos);

	//Start learning
	ssCycle	= 0;
	while(1)
	{
		//Search Die on current position
		m_stSearchArea = stDieSizeArea;
		if (bNormalDie)		//v3.82
			ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 19);	//v3.85
		else
		{
			ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE ,PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, 0, 20);		//v3.85
		}

		if (DieIsAlignable(usDieType) == FALSE)
		{
			m_stSearchArea = stOrgSearchArea;
			return BPR_ERR_CALIB_NO_1ST_DIE;
		}

		if (ssCycle == 0)
		{
			//if (bNormalDie)		//v3.85
			//	stDestX.XPos = 30;    
			//else
			stDestX.XPos = m_lPrCal1stStepSize;		//100;		//v4.57A2  
			stDestX.YPos = 0;     

			stDestY.XPos = 0;     
			//if (bNormalDie)		//v3.85
			//	stDestY.YPos = 30;     
			//else
			stDestY.YPos = m_lPrCal1stStepSize;    //100;		//v4.57A2 
			
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if( pApp->IsPLLMRebel() )
			{
				//stDestX.XPos = 500;
				//stDestY.YPos = 500;
			}
			stDieShiftX.x = (PR_WORD)m_lPrCenterX;
			stDieShiftX.y = (PR_WORD)m_lPrCenterY;
			stDieShiftY.x = (PR_WORD)m_lPrCenterX;
			stDieShiftY.y = (PR_WORD)m_lPrCenterY;
			bUpdateSearchArea = FALSE;
		}
		else
		{	//Only update search area & calculate motor step on cycle 1 & 2 
			if (bNormalDie)		//v3.85
			{
				LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
				siTempData1 = (int)(stDiePosition[0].x - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x) / 2) - PR_MAX_WIN_ULC_X);
				siTempData2 = (int)(PR_MAX_WIN_LRC_X - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x) / 2) - stDiePosition[0].x);
			}
			else
			{
				siTempData1 = (int)(stDiePosition[0].x - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x) / 2) - PR_MAX_WIN_ULC_X);
				siTempData2 = (int)(PR_MAX_WIN_LRC_X - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x) / 2) - stDiePosition[0].x);
			}

			if (ssCycle != 3)	// divide by zero
			{
				stDieShiftX.x = abs((min(siTempData1, siTempData2) - siSrchMargin) / (3 - ssCycle));
			}

			stDieShiftX.y = 0;
			ConvertPixelToMotorStep(stDieShiftX, &stDestX.XPos, &stDestX.YPos);

			if (bNormalDie)		//v3.85
			{
				LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
				siTempData1 = (int)(stDiePosition[0].y - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y) / 2) - PR_MAX_WIN_ULC_Y);
				siTempData2 = (int)(PR_MAX_WIN_LRC_Y - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y) / 2) - stDiePosition[0].y);
			}
			else
			{
				siTempData1 = (int)(stDiePosition[0].y - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y) / 2) - PR_MAX_WIN_ULC_Y);
				siTempData2 = (int)(PR_MAX_WIN_LRC_Y - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y) / 2) - stDiePosition[0].y);
			}
			if (ssCycle != 3)	// divide by zero
			{
				stDieShiftY.y = abs((min(siTempData1, siTempData2) - siSrchMargin) / (3 - ssCycle));
			}
			stDieShiftY.x = 0;
			ConvertPixelToMotorStep(stDieShiftY, &stDestY.XPos, &stDestY.YPos);

			bUpdateSearchArea = TRUE;
		}

		//Get current motor pos to calculate next postion
		GetBinTableEncoder(&stTempPos.XPos, &stTempPos.YPos);

		//Move Table X & search PR
		ssSign = 1;
		for (ssCount=0; ssCount<2; ssCount++)
		{
			if (ssCount > 0)
			{
				ssSign = -1;
			}

			if (bUpdateSearchArea == TRUE)
			{
				DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_TRANSPARENT);
				DrawRectangleBox(m_stSearchArea.coCorner1,m_stSearchArea.coCorner2,PR_COLOR_TRANSPARENT);

				//Update Search die area
				if (bNormalDie)		//v3.85
				{
					LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
					m_stSearchArea.coCorner1.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner1.y = (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
					m_stSearchArea.coCorner2.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner2.y = (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
				}
				else
				{
					m_stSearchArea.coCorner1.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner1.y = (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
					m_stSearchArea.coCorner2.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner2.y = (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
				}
				VerifyPRRegion(&m_stSearchArea);
				DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);
			}

			//Move table to + and then - direction
			MoveBinTable((stTempPos.XPos + (ssSign*stDestX.XPos)), (stTempPos.YPos + (ssSign*stDestX.YPos)));
			Sleep(500);

			//Search PR		
			GetBinTableEncoder(&stTablePos[ssCount].XPos, &stTablePos[ssCount].YPos);
			
			if (bNormalDie)		//v3.82
				ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 21);	//v3.85
			else
			{
				ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 22);	//v3.85
			}

			if (DieIsAlignable(usDieType) == FALSE)
			{
				m_stSearchArea = stOrgSearchArea;
				if (ssSign == 1)
				{
					return BPR_ERR_CALIB_NO_LT_DIE;
				}
				return BPR_ERR_CALIB_NO_RT_DIE;
			}
		}

		//Move table back to start poistion
		DrawRectangleBox(m_stSearchArea.coCorner1,m_stSearchArea.coCorner2,PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_GREEN);
		MoveBinTable(stLastPos.XPos, stLastPos.YPos);
		Sleep(500);


		//Get Both X Value
		stXMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stXMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stXPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stXPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;


		//Move Table Y & search PR
		ssSign = 1;
		for (ssCount=0; ssCount<2; ssCount++)
		{
			if (ssCount > 0)
			{
				ssSign = -1;
			}

			if (bUpdateSearchArea == TRUE)
			{
				DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_TRANSPARENT);
				DrawRectangleBox(m_stSearchArea.coCorner1,m_stSearchArea.coCorner2,PR_COLOR_TRANSPARENT);

				//Update Search die area
				if (bNormalDie)		//v3.85
				{
					LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
					m_stSearchArea.coCorner1.x	= (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner1.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
					m_stSearchArea.coCorner2.x	= (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner2.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
				}
				else
				{
					m_stSearchArea.coCorner1.x	= (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner1.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
					m_stSearchArea.coCorner2.x	= (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner2.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
				}
				VerifyPRRegion(&m_stSearchArea);
				DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);
			}

			//Move table to + and then - direction
			MoveBinTable((stTempPos.XPos + (ssSign*stDestY.XPos)), (stTempPos.YPos + (ssSign*stDestY.YPos)));
			Sleep(500);

			//Search PR		
			GetBinTableEncoder(&stTablePos[ssCount].XPos, &stTablePos[ssCount].YPos);
			
			if (bNormalDie)		//v3.82
				ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 23);	//v3.85
			else
			{
				ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 24);	//v3.85
			}
			
			if (DieIsAlignable(usDieType) == FALSE)
			{
				m_stSearchArea = stOrgSearchArea;
				if (ssSign == 1)
				{
					return BPR_ERR_CALIB_NO_UP_DIE;
				}
				return BPR_ERR_CALIB_NO_DN_DIE;
			}
		}


		//Get Both Y Value
		stYMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stYMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stYPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stYPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;

		
		// Check for zero error!
		fTemp = (float)stXPixelMove.x * (float)stYPixelMove.y - (float)stYPixelMove.x * (float)stXPixelMove.y;
		if (fabs(fTemp) < 0.000001)
		{
			m_stSearchArea = stOrgSearchArea;
			return BPR_ERR_CALIB_ZERO_VALUE;
		}

		if (IsSensorZoomFFMode())
		{
			m_dFFModeCalibX	= ((DOUBLE)stXMotorMove.XPos * (DOUBLE)stYPixelMove.y - (DOUBLE)stYMotorMove.XPos*(DOUBLE)stXPixelMove.y) / fTemp;
			m_dFFModeCalibXY	= ((DOUBLE)stYMotorMove.XPos - (DOUBLE)m_dFFModeCalibX * (DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;

			m_dFFModeCalibY	= ((DOUBLE)stYMotorMove.YPos * (DOUBLE)stXPixelMove.x-(DOUBLE)stXMotorMove.YPos * (DOUBLE)stYPixelMove.x) / fTemp;
			m_dFFModeCalibYX	= ((DOUBLE)stXMotorMove.YPos - (DOUBLE)m_dFFModeCalibY * (DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;
		}
		else
		{
			m_dCalibX	= ((DOUBLE)stXMotorMove.XPos * (DOUBLE)stYPixelMove.y - (DOUBLE)stYMotorMove.XPos*(DOUBLE)stXPixelMove.y) / fTemp;
			m_dCalibXY	= ((DOUBLE)stYMotorMove.XPos - (DOUBLE)m_dCalibX * (DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;

			m_dCalibY	= ((DOUBLE)stYMotorMove.YPos * (DOUBLE)stXPixelMove.x-(DOUBLE)stXMotorMove.YPos * (DOUBLE)stYPixelMove.x) / fTemp;
			m_dCalibYX	= ((DOUBLE)stXMotorMove.YPos - (DOUBLE)m_dCalibY * (DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;
		}

		PR_RSIZE		stFov;
		PR_OPTIC		stOptic;
		PR_COMMON_RPY	stComRpy;
		stFov.x = (PR_REAL)(m_dBTXRes * m_dCalibX * 8192);	// mm
		stFov.y = (PR_REAL)(m_dBTYRes * m_dCalibY * 8192);	// mm
		// Set FOV to Wafer PR
		PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);
		PR_SetFov(&stFov, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);

		//Move table back to start poistion & do PR & update start position
		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

		m_stSearchArea = stDieSizeArea;
		MoveBinTable(stLastPos.XPos, stLastPos.YPos);
		Sleep(500);

		if (bNormalDie)		//v3.82
			ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 25);	//v3.85
		else
		{
			ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 26);	//v3.85
		}
		
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDiePosition[0]);
			GetBinTableEncoder(&stLastPos.XPos, &stLastPos.YPos);
		}

		//Start next cycle	
		ssCycle++;
		if (ssCycle > 2)
		{
			break;
		}
	}

	m_bDieCalibrated = TRUE;
	m_stSearchArea = stOrgSearchArea;
	return 0;
}

LONG CBondPr::FindDieCalibration2(BOOL bNormalDie)
{
	PR_UWORD		usDieType;
	PR_COORD		stDiePosition[2]; 
	PR_COORD		stDieShiftX,stDieShiftY; 
	PR_COORD		stXPixelMove; 
	PR_COORD		stYPixelMove; 
	PR_WIN			stOrgSearchArea = m_stSearchArea;
	PR_WIN			stDieSizeArea;
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;

	int				siSrchMargin = (32 * PR_SCALE_FACTOR);
	int				siTempData1 = 0;
	int				siTempData2 = 0;
	short			ssCycle;			
	short			ssSign = 1;
	short			ssCount = 0;
	float			fTemp = 0.0;
	BOOL			bUpdateSearchArea = FALSE;

	LONG lRefDieNo = BPR_GEN_RDIE_OFFSET + 1;
	
	typedef struct 
	{
		int XPos;
		int YPos;
	} MOTORDATA;

	MOTORDATA	stDestX,stDestY;
	MOTORDATA	stLastPos, stTempPos;
	MOTORDATA	stTablePos[2];
	MOTORDATA	stXMotorMove, stYMotorMove;


	//Update Search area
	if (bNormalDie)
	{
		LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
		m_stSearchArea.coCorner1.x = (PR_WORD)m_lPrCenterX - (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x);
		m_stSearchArea.coCorner1.y = (PR_WORD)m_lPrCenterY - (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y);
		m_stSearchArea.coCorner2.x = (PR_WORD)m_lPrCenterX + (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x);
		m_stSearchArea.coCorner2.y = (PR_WORD)m_lPrCenterY + (PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y);
		VerifyPRRegion(&m_stSearchArea);
		stDieSizeArea = m_stSearchArea;
		DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_GREEN);
	}
	else
	{
		PR_WIN stWnd;
		stWnd.coCorner1.x	= (PR_WORD)m_lPrCenterX - (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x);
		stWnd.coCorner1.y	= (PR_WORD)m_lPrCenterY - (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y);
		stWnd.coCorner2.x	= (PR_WORD)m_lPrCenterX + (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x);
		stWnd.coCorner2.y	= (PR_WORD)m_lPrCenterY + (PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y);

		VerifyPRRegion(&stWnd);
		stDieSizeArea = stWnd;
		DrawRectangleBox(stWnd.coCorner1, stWnd.coCorner2, PR_COLOR_GREEN);
	}

	//Store Current position	
	GetBinTable2Encoder(&stLastPos.XPos, &stLastPos.YPos);

	//Start learning
	ssCycle	= 0;
	while(1)
	{
		//Search Die on current position
		m_stSearchArea = stDieSizeArea;
		if (bNormalDie)		//v3.82
			ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo() ,PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 27);	//v3.85
		else
		{
			ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE ,PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, 0, 28);		//v3.85
		}

		if (DieIsAlignable(usDieType) == FALSE)
		{
			m_stSearchArea = stOrgSearchArea;
			return BPR_ERR_CALIB_NO_1ST_DIE;
		}

		if (ssCycle == 0)
		{
			if (bNormalDie)		//v3.85
				stDestX.XPos = 30;    
			else
				stDestX.XPos = 100;    
			stDestX.YPos = 0;     
			stDestY.XPos = 0;     
			if (bNormalDie)		//v3.85
				stDestY.YPos = 30;     
			else
				stDestY.YPos = 100;     
			stDieShiftX.x = (PR_WORD)m_lPrCenterX;
			stDieShiftX.y = (PR_WORD)m_lPrCenterY;
			stDieShiftY.x = (PR_WORD)m_lPrCenterX;
			stDieShiftY.y = (PR_WORD)m_lPrCenterY;
			bUpdateSearchArea = FALSE;
		}
		else
		{	//Only update search area & calculate motor step on cycle 1 & 2 
			if (bNormalDie)		//v3.85
			{
				LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
				siTempData1 = (int)(stDiePosition[0].x - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x) / 2) - PR_MAX_WIN_ULC_X);
				siTempData2 = (int)(PR_MAX_WIN_LRC_X - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x) / 2) - stDiePosition[0].x);
			}
			else
			{
				siTempData1 = (int)(stDiePosition[0].x - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x) / 2) - PR_MAX_WIN_ULC_X);
				siTempData2 = (int)(PR_MAX_WIN_LRC_X - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x) / 2) - stDiePosition[0].x);
			}
			if( ssCycle!=3 )	// divide by zero
				stDieShiftX.x = abs((min(siTempData1,siTempData2) - siSrchMargin) / (3-ssCycle));
			stDieShiftX.y = 0;
			ConvertPixelToMotorStep2(stDieShiftX, &stDestX.XPos, &stDestX.YPos);

			if (bNormalDie)		//v3.85
			{
				LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
				siTempData1 = (int)(stDiePosition[0].y - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y) / 2) - PR_MAX_WIN_ULC_Y);
				siTempData2 = (int)(PR_MAX_WIN_LRC_Y - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y) / 2) - stDiePosition[0].y);
			}
			else
			{
				siTempData1 = (int)(stDiePosition[0].y - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y) / 2) - PR_MAX_WIN_ULC_Y);
				siTempData2 = (int)(PR_MAX_WIN_LRC_Y - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y) / 2) - stDiePosition[0].y);
			}
			if( ssCycle!=3 )	// divide by zero
				stDieShiftY.y = abs((min(siTempData1,siTempData2) - siSrchMargin) / (3-ssCycle));
			stDieShiftY.x = 0;
			ConvertPixelToMotorStep2(stDieShiftY, &stDestY.XPos, &stDestY.YPos);

			bUpdateSearchArea = TRUE;
		}

		//Get current motor pos to calculate next postion
		GetBinTable2Encoder(&stTempPos.XPos, &stTempPos.YPos);

		//Move Table X & search PR
		ssSign = 1;
		for (ssCount=0; ssCount<2; ssCount++)
		{
			if (ssCount > 0)
			{
				ssSign = -1;
			}

			if (bUpdateSearchArea == TRUE)
			{
				DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_TRANSPARENT);
				DrawRectangleBox(m_stSearchArea.coCorner1,m_stSearchArea.coCorner2,PR_COLOR_TRANSPARENT);

				//Update Search die area
				if (bNormalDie)		//v3.85
				{
					LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
					m_stSearchArea.coCorner1.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner1.y = (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
					m_stSearchArea.coCorner2.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner2.y = (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
				}
				else
				{
					m_stSearchArea.coCorner1.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner1.y = (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
					m_stSearchArea.coCorner2.x = (ssSign*stDieShiftX.x) + (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner2.y = (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
				}
				VerifyPRRegion(&m_stSearchArea);
				DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);
			}

			//Move table to + and then - direction
			MoveBinTable((stTempPos.XPos + (ssSign*stDestX.XPos)), (stTempPos.YPos + (ssSign*stDestX.YPos)), TRUE);
			Sleep(500);

			//Search PR		
			GetBinTable2Encoder(&stTablePos[ssCount].XPos, &stTablePos[ssCount].YPos);
			
			if (bNormalDie)		//v3.82
				ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 29);	//v3.85
			else
			{
				ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 30);	//v3.85
			}

			if (DieIsAlignable(usDieType) == FALSE)
			{
				m_stSearchArea = stOrgSearchArea;
				if (ssSign == 1)
				{
					return BPR_ERR_CALIB_NO_LT_DIE;
				}
				return BPR_ERR_CALIB_NO_RT_DIE;
			}
		}

		//Move table back to start poistion
		DrawRectangleBox(m_stSearchArea.coCorner1,m_stSearchArea.coCorner2,PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_GREEN);
		MoveBinTable(stLastPos.XPos, stLastPos.YPos, TRUE);
		Sleep(500);


		//Get Both X Value
		stXMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stXMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stXPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stXPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;


		//Move Table Y & search PR
		ssSign = 1;
		for (ssCount=0; ssCount<2; ssCount++)
		{
			if (ssCount > 0)
			{
				ssSign = -1;
			}

			if (bUpdateSearchArea == TRUE)
			{
				DrawRectangleBox(stDieSizeArea.coCorner1,stDieSizeArea.coCorner2,PR_COLOR_TRANSPARENT);
				DrawRectangleBox(m_stSearchArea.coCorner1,m_stSearchArea.coCorner2,PR_COLOR_TRANSPARENT);

				//Update Search die area
				if (bNormalDie)		//v3.85
				{
					LONG lDieNo = IsSensorZoomFFMode() ? BPR_NORMAL_PR_FF_MODE_INDEX3 - 1 : BPR_NORMAL_DIE;
					m_stSearchArea.coCorner1.x	= (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner1.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
					m_stSearchArea.coCorner2.x	= (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].x));
					m_stSearchArea.coCorner2.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lDieNo].y));
				}
				else
				{
					m_stSearchArea.coCorner1.x	= (PR_WORD)m_lPrCenterX - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner1.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY - ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
					m_stSearchArea.coCorner2.x	= (PR_WORD)m_lPrCenterX + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].x));
					m_stSearchArea.coCorner2.y	= (ssSign*stDieShiftY.y) + (PR_WORD)m_lPrCenterY + ((PR_WORD)(1.3 * m_stGenDieSize[lRefDieNo].y));
				}
				VerifyPRRegion(&m_stSearchArea);
				DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);
			}

			//Move table to + and then - direction
			MoveBinTable((stTempPos.XPos + (ssSign*stDestY.XPos)), (stTempPos.YPos + (ssSign*stDestY.YPos)), TRUE);
			Sleep(500);

			//Search PR		
			GetBinTable2Encoder(&stTablePos[ssCount].XPos, &stTablePos[ssCount].YPos);
			
			if (bNormalDie)		//v3.82
				ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 31);	//v3.85
			else
			{
				ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[ssCount], &fDieScore, 
									m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 32);	//v3.85
			}
			
			if (DieIsAlignable(usDieType) == FALSE)
			{
				m_stSearchArea = stOrgSearchArea;
				if (ssSign == 1)
				{
					return BPR_ERR_CALIB_NO_UP_DIE;
				}
				return BPR_ERR_CALIB_NO_DN_DIE;
			}
		}


		//Get Both Y Value
		stYMotorMove.XPos	= stTablePos[1].XPos - stTablePos[0].XPos;
		stYMotorMove.YPos	= stTablePos[1].YPos - stTablePos[0].YPos;
		stYPixelMove.x		= stDiePosition[1].x - stDiePosition[0].x;
		stYPixelMove.y		= stDiePosition[1].y - stDiePosition[0].y;

		
		// Check for zero error!
		fTemp = (float)stXPixelMove.x * (float)stYPixelMove.y - (float)stYPixelMove.x * (float)stXPixelMove.y;
		if (fabs(fTemp) < 0.000001 )
		{
			m_stSearchArea = stOrgSearchArea;
			return BPR_ERR_CALIB_ZERO_VALUE;
		}

		m_dCalibX2	= ((DOUBLE)stXMotorMove.XPos * (DOUBLE)stYPixelMove.y - (DOUBLE)stYMotorMove.XPos*(DOUBLE)stXPixelMove.y) / fTemp;
		m_dCalibXY2	= ((DOUBLE)stYMotorMove.XPos - (DOUBLE)m_dCalibX2 * (DOUBLE)stYPixelMove.x) / (DOUBLE)stYPixelMove.y;

		m_dCalibY2	= ((DOUBLE)stYMotorMove.YPos * (DOUBLE)stXPixelMove.x-(DOUBLE)stXMotorMove.YPos * (DOUBLE)stYPixelMove.x) / fTemp;
		m_dCalibYX2	= ((DOUBLE)stXMotorMove.YPos - (DOUBLE)m_dCalibY2 * (DOUBLE)stXPixelMove.y) / (DOUBLE)stXPixelMove.x;

		PR_RSIZE		stFov;
//		PR_OPTIC		stOptic;
//		PR_COMMON_RPY	stComRpy;
		stFov.x = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_BINTABLE2_X) * m_dCalibX2 * 8192);	// mm
		stFov.y = (PR_REAL)(GetChannelResolution(MS896A_CFG_CH_BINTABLE2_Y) * m_dCalibY2 * 8192);	// mm
		// Set FOV to Wafer PR
		//PR_GetOptic(MS899_BOND_GEN_PURPOSE, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);
		//PR_SetFov(&stFov, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stOptic, &stComRpy);

		//Move table back to start poistion & do PR & update start position
		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleBox(stDieSizeArea.coCorner1, stDieSizeArea.coCorner2, PR_COLOR_GREEN);

		m_stSearchArea = stDieSizeArea;
		MoveBinTable(stLastPos.XPos, stLastPos.YPos, TRUE);
		Sleep(500);

		if (bNormalDie)		//v3.82
			ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(), PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 33);	//v3.85
		else
		{
			ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_DIE, PR_TRUE, PR_TRUE, PR_FALSE, &usDieType, &fDieRotate, &stDiePosition[0], &fDieScore, 
								m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 34);	//v3.85
		}
		
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDiePosition[0], TRUE);
			GetBinTable2Encoder(&stLastPos.XPos, &stLastPos.YPos);
		}

		//Start next cycle	
		ssCycle++;
		if (ssCycle > 2)
		{
			break;
		}
	}

	m_bDieCalibrated2 = TRUE;
	m_stSearchArea = stOrgSearchArea;
	return 0;

}	//FindDieCalibration2()


//Postbond function
BOOL CBondPr::CheckMaxRotation(ULONG ulBin, DOUBLE dAngle, BOOL bIsArm1)
{
	if (m_bBinTableAngleCheck == FALSE || IsNGBlock((UCHAR)(ulBin + 1)))
	{
		return TRUE;	
	}

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK-1);
	ulBin	= max(ulBin, 0);

	//Start checking

	if (bIsArm1)
	{
		if (fabs(dAngle) > m_dGenMaxAngle[ulBin])
		{
			m_lOutAngleCounter[ulBin]++;
		}
		else	
		{
			//Semitek	//Jay/Leo	//11-09-2016	//v4.54A10
			if (m_lOutAngleCounter[ulBin] > 0)
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetCustomerName() == CTM_SEMITEK)
				{
					//Change from accumulative counter to continous counter only;
					//so reset counter if any die rotate is OK;
					m_lOutAngleCounter[ulBin] = 0;
				}
			}
		}
	
		if (m_lOutAngleCounter[ulBin] > m_lGenMaxAngleAllow[ulBin])
		{
			m_lOutAngleCounter[ulBin] = 0;

			CString szLog;
			szLog.Format("PB Bin #%ld Max Rotation counter (BH1) is reset in AUTOBOND", ulBin);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.46T4	//Semitek

			return FALSE;
		}
	}
	else	//v4.57A14	//SEmitek
	{
		if (fabs(dAngle) > m_dGenMaxAngle[ulBin])
		{
			m_lOutAngleCounter2[ulBin]++;
		}
		else	
		{
			if (m_lOutAngleCounter2[ulBin] > 0)
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetCustomerName() == CTM_SEMITEK)
				{
					//Change from accumulative counter to continous counter only;
					//so reset counter if any die rotate is OK;
					m_lOutAngleCounter2[ulBin] = 0;
				}
			}
		}
	
		if (m_lOutAngleCounter2[ulBin] > m_lGenMaxAngleAllow[ulBin])
		{
			m_lOutAngleCounter2[ulBin] = 0;

			CString szLog;
			szLog.Format("PB Bin #%ld Max Rotation counter (BH2) is reset in AUTOBOND", ulBin);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBondPr::CheckMaxAccRotation(ULONG ulBin, DOUBLE dAngle, BOOL bIsArm1)
{
	if (m_bBinTableAngleCheck == FALSE || IsNGBlock((UCHAR)(ulBin + 1)))
	{
		return TRUE;	
	}

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK-1);
	ulBin	= max(ulBin, 0);

	//Start checking

	if (bIsArm1)
	{
		if (fabs(dAngle) > m_dGenMaxAngle[ulBin])
		{
			m_lAccOutAngleCounter[ulBin]++;
			IncreasePBIAngleCounter();
		}
	
		if (m_lAccOutAngleCounter[ulBin] > m_lGenMaxAccAngleAllow[ulBin])
		{
			m_lAccOutAngleCounter[ulBin] = 0;
			//m_lPBIAngleCounter = 0;
			CString szLog;
			szLog.Format("PB Bin #%ld Max ACC Rotation counter (BH1) is reset in AUTOBOND", ulBin);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			return FALSE;
		}
	}
	else
	{
		if (fabs(dAngle) > m_dGenMaxAngle[ulBin])
		{
			m_lAccOutAngleCounter2[ulBin]++;
			IncreasePBIAngleCounter();
		}
	
		if (m_lAccOutAngleCounter2[ulBin] > m_lGenMaxAccAngleAllow[ulBin])
		{
			m_lAccOutAngleCounter2[ulBin] = 0;
			//m_lPBIAngleCounter = 0;
			CString szLog;
			szLog.Format("PB Bin #%ld Max ACC Rotation counter (BH2) is reset in AUTOBOND", ulBin);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBondPr::CheckDieShifted(ULONG ulBin, DOUBLE dShiftX, DOUBLE dShiftY, BOOL bIsArm1)
{
	if (m_bPlacementCheck == FALSE || IsNGBlock((UCHAR)(ulBin + 1)))
	{
		return TRUE;
	}

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK-1);
	ulBin	= max(ulBin, 0);
		
	CString szLog;


	//Start checking
	if ((fabs(dShiftX) > m_dGenDieShiftX[ulBin]) || (fabs(dShiftY) > m_dGenDieShiftY[ulBin]))
	{
		m_lOutShiftCounter[ulBin]++;
		
		IncreasePBIShiftCounter();
		//v4.59A4
		szLog.Format("ERR: PB XY-shift fails #(%d): X-Shift = %f (%f), Y-Shift = %f (%f)", 
			m_lOutShiftCounter[ulBin], 
			dShiftX, m_dGenDieShiftX[ulBin], dShiftY, m_dGenDieShiftY[ulBin]);
		SetErrorMessage(szLog);
	}

	if (m_lOutShiftCounter[ulBin] > m_lGenMaxShiftAllow[ulBin])
	{
		/*
		CString	csText;

		csText.Format("BinBlock %d out of XY Shift (%f, %f)", ulBin+1, dShiftX, dShiftY);
		DisplayMessage(csText);
		*/
		m_lOutShiftCounter[ulBin] = 0;
		//m_lPBIShiftCounter = 0;
		szLog.Format("PB Bin #%ld XY Shift counter is reset in AUTOBOND", ulBin);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);		//v4.46T4	//Semitek

		return FALSE;
	}

	return TRUE;
}


BOOL CBondPr::CheckAverageRotation(ULONG ulBin)
{
	if (m_bAverageAngleCheck == FALSE || IsNGBlock((UCHAR)(ulBin + 1)))
	{
		return TRUE;
	}

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK-1);
	ulBin	= max(ulBin, 0);

	//Start checking
	if (fabs(m_dOutAverageAngle[ulBin]) > m_dGenAverageAngle[ulBin])
	{
		/*
		CString	csText;
		csText.Format("BinBlock %d out of Avg Rotation (%f)", ulBin+1, fabs(m_dOutAverageAngle[ulBin]));
		DisplayMessage(csText);
		*/
		return FALSE;
	}

	return TRUE;
}


BOOL CBondPr::CheckAverageCp(ULONG ulBin, BOOL bIsArm1)	//v4.51A17
{
	if (m_bPlacementCheck == FALSE || IsNGBlock((UCHAR)(ulBin + 1)))
	{
		return TRUE;
	}

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK-1);
	ulBin	= max(ulBin, 0);

	m_lSpcCpCounter++;

	if (m_lSpcCpCounter < 50)
	{
		return TRUE;
	}
	m_lSpcCpCounter = 0;

	//4.53D105 open back the log
	CString szLog;
	if (m_dMinCpXAllow > 0)
	{
		if ((m_dSpcCpX > 0) && (m_dSpcCpX < m_dMinCpXAllow))
		{
			szLog.Format("BPR PB ERROR: CpX (%.3f) below limit (%.3f); AUTOBOND is aborted.",
							m_dSpcCpX, m_dMinCpXAllow);
			SetErrorMessage(szLog);

			szLog.Format("PostBond ERROR: CpX below limit (%.3f).", m_dMinCpXAllow);
			HmiMessage_Red_Yellow(szLog, "Hit  CP  Limit");
			//SetAlert_Red_Yellow(IDS_BH_SPC_CP_XY_LIMIT);
			return FALSE;
		}
	}

	if (m_dMinCpYAllow > 0)
	{
		if ((m_dSpcCpY > 0) && (m_dSpcCpY < m_dMinCpYAllow))
		{
			szLog.Format("BPR PB ERROR: CpY (%.3f) below limit (%.3f); AUTOBOND is aborted.",
							m_dSpcCpY, m_dMinCpYAllow);
			SetErrorMessage(szLog);

			szLog.Format("PostBond ERROR: CpY below limit (%.3f).", m_dMinCpYAllow);
			HmiMessage_Red_Yellow(szLog, "Hit  CP  Limit");
			//SetAlert_Red_Yellow(IDS_BH_SPC_CP_XY_LIMIT);
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CBondPr::CheckDieDefect(ULONG ulBin, LONG lDieType)
{
	if ((m_bDefectCheck == FALSE) || (lDieType == 0) || IsNGBlock((UCHAR)(ulBin + 1)))
	{
		return TRUE;
	}

	if (lDieType != 1)						// 1=Defective die		//v2.96T4
		return TRUE;

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK-1);
	ulBin	= max(ulBin, 0);

	//Start checking
	m_lOutDefectCounter[ulBin]++;
	if (m_lOutDefectCounter[ulBin] > m_lGenMaxDefectAllow[ulBin])
	{
		/*
		CString	csText;
		csText.Format("BinBlock %d out of Max Defect", ulBin+1);
		DisplayMessage(csText);
		*/
		m_lOutDefectCounter[ulBin] = 0;
		return FALSE;
	}

	return TRUE;
}


BOOL CBondPr::CheckDieEmpty(ULONG ulBin, LONG lDieType, BOOL bIsArm1)
{
	if (m_bEmptyCheck == FALSE)
	{
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCriticalErr = FALSE;
	CString szLog;

	//v4.59A17
	//szLog.Format("customername,%s,empty allow,%d",pApp->GetCustomerName(),m_lGenMaxEmptyAllow[ulBin]);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	if (pApp->GetCustomerName() == "Cree" && m_lGenMaxEmptyAllow[ulBin] == 0 && lDieType == 2)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("Cree check die empty");
		return FALSE;
	}
#ifdef NU_MOTION

	//v4.45T2	//SanAn
	if (lDieType == 2 && !IsNGBlock((UCHAR)(ulBin + 1)))		//If current PB die is EMPTY
	{
		//Try to look around to find any neighbor dices, only for SanAn
		BOOL bFOVHasDie = AutoLookAroundDieInFOV(bCriticalErr, ulBin, bIsArm1);
		if (!bFOVHasDie)	//If no neighbor die found
		{
			if (bCriticalErr)
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("BPR: critical error by 4 camera blocking");
				m_lOutCamBlockEmptyCounter = 0;	//Reset Camera-Block counter after alarm
				return FALSE;
			}

			//Propoerly camera blocking in MS100 sequence because no other neighbor dices found, 
			// so do not count this EMPTY die !!
			if (bIsArm1)
				szLog.Format("BPR: skip EMPTY die because of camera blocking BH1; count = %ld", m_lOutCamBlockEmptyCounter);
			else
				szLog.Format("BPR: skip EMPTY die because of camera blocking BH2; count = %ld", m_lOutCamBlockEmptyCounter);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			return TRUE;
		}
		else
		{
			m_lOutCamBlockEmptyCounter = 0;	//Reset Camera-Block counter if no EMPTY die
			if (bIsArm1)
				szLog.Format("BPR: FOV has die in EMPTY die check at BH1; count = %ld", m_lOutCamBlockEmptyCounter);
			else
				szLog.Format("BPR: FOV has die in EMPTY die check at BH2; count = %ld", m_lOutCamBlockEmptyCounter);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
	else
	{
		szLog.Format("BPR: PB has Die - %d", bIsArm1);
		//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		m_lOutCamBlockEmptyCounter = 0;	//Reset Camera-Block counter if no EMPTY die
	}

#endif

	if (lDieType != 2)			// 2=Empty die, 1=Defective die, 0=GOOD die
	{
		if (bIsArm1)
			m_lOutEmptyCounter[ulBin] = 0;		//Reset consessive EMPTY count if curr die is OK	//v4.39T6	//SanAn
		else
			m_lOutEmptyCounter2[ulBin] = 0;		//Reset consessive EMPTY count if curr die is OK	//v4.39T6	//SanAn
		return TRUE;
	}

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK-1);
	ulBin	= max(ulBin, 0);

	//Start checking
	if (bIsArm1)
	{
		szLog.Format("BH1 PB EMPTY Total=%ld, ACC=%ld (%ld)", 
			m_lOutEmptyCounter[ulBin], m_lOutAccEmptyCounter[ulBin], m_lGenMaxAccEmptyAllow[ulBin]);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		m_lOutEmptyCounter[ulBin]++;

		if (m_lGenMaxAccEmptyAllow[ulBin] > 0)
		{
			m_lOutAccEmptyCounter[ulBin]++;
			m_lOutAccEmptyCounterTotal[ulBin]++;
		}

		(*m_psmfSRam)["BondPr"]["EmptyCounter"] = m_lOutEmptyCounter[ulBin];
		(*m_psmfSRam)["BondPr"]["EmptyCounter2"] = m_lOutEmptyCounter2[ulBin];
		(*m_psmfSRam)["BondPr"]["AccEmptyCounter"] = m_lOutAccEmptyCounter[ulBin];
		(*m_psmfSRam)["BondPr"]["AccEmptyCounter2"] = m_lOutAccEmptyCounter2[ulBin];
		(*m_psmfSRam)["BondPr"]["AccEmptyCounterTotal"] = m_lOutAccEmptyCounterTotal[ulBin];

		if (m_lOutEmptyCounter[ulBin] >= m_lGenMaxEmptyAllow[ulBin])
		{
			m_lOutEmptyCounter[ulBin] = 0;
			return FALSE;
		}

		//v4.40T9	//Sanan
		if (m_lGenMaxAccEmptyAllow[ulBin] > 0)
		{
			/*
			if (m_lOutAccEmptyCounter[ulBin] >= m_lGenMaxAccEmptyAllow[ulBin])
			{
				m_lOutAccEmptyCounter[ulBin] = 0;
				return FALSE;
			}
			*/

			if (m_lOutAccEmptyCounterTotal[ulBin] >= m_lGenMaxAccEmptyAllow[ulBin])
			{
				/*m_lOutAccEmptyCounter[ulBin] = 0;
				m_lOutAccEmptyCounter2[ulBin] = 0;
				m_lOutAccEmptyCounterTotal[ulBin] = 0;*/
				return FALSE;
			}

			//v4.48A4
			if (pApp->GetCustomerName().Find(CTM_SANAN) != -1)
			{
				if (m_lOutAccEmptyCounter[ulBin] >= m_lGenMaxAccEmptyAllow[ulBin] - 1)
				{
					(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit1"] = TRUE;

				}
			}
		}
	}
	else
	{
		szLog.Format("BH2 PB EMPTY Total=%ld, ACC=%ld (%ld)", 
			m_lOutEmptyCounter2[ulBin], m_lOutAccEmptyCounter2[ulBin], m_lGenMaxAccEmptyAllow[ulBin]);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		m_lOutEmptyCounter2[ulBin]++;

		if (m_lGenMaxAccEmptyAllow[ulBin] > 0)
		{
			m_lOutAccEmptyCounter2[ulBin]++;
			m_lOutAccEmptyCounterTotal[ulBin]++;
		}

		(*m_psmfSRam)["BondPr"]["EmptyCounter"] = m_lOutEmptyCounter[ulBin];
		(*m_psmfSRam)["BondPr"]["EmptyCounter2"] = m_lOutEmptyCounter2[ulBin];
		(*m_psmfSRam)["BondPr"]["AccEmptyCounter"] = m_lOutAccEmptyCounter[ulBin];
		(*m_psmfSRam)["BondPr"]["AccEmptyCounter2"] = m_lOutAccEmptyCounter2[ulBin];
		(*m_psmfSRam)["BondPr"]["AccEmptyCounterTotal"] = m_lOutAccEmptyCounterTotal[ulBin];

		if (m_lOutEmptyCounter2[ulBin] >= m_lGenMaxEmptyAllow[ulBin])
		{
			m_lOutEmptyCounter2[ulBin] = 0;
			return FALSE;
		}

		//v4.40T9	//Sanan
		if (m_lGenMaxAccEmptyAllow[ulBin] > 0)
		{
			/*
			if (m_lOutAccEmptyCounter2[ulBin] >= m_lGenMaxAccEmptyAllow[ulBin])
			{
				m_lOutAccEmptyCounter2[ulBin] = 0;
				return FALSE;
			}
			*/

			if (m_lOutAccEmptyCounterTotal[ulBin] >= m_lGenMaxAccEmptyAllow[ulBin])
			{
				/*m_lOutAccEmptyCounter[ulBin] = 0;
				m_lOutAccEmptyCounter2[ulBin] = 0;
				m_lOutAccEmptyCounterTotal[ulBin] = 0;*/
				return FALSE;
			}

			//v4.48A4
			if (pApp->GetCustomerName().Find(CTM_SANAN) != -1)
			{
				if (m_lOutAccEmptyCounter2[ulBin] >= m_lGenMaxAccEmptyAllow[ulBin] - 1)
				{
					(*m_psmfSRam)["BondPr"]["PostBond"]["NearEmptyLimit2"] = TRUE;
				}
			}
		}
	}

	return TRUE;
}


VOID CBondPr::DecreaseDieEmptyCount(BOOL bIsArm1)
{
	if (m_bEmptyCheck == FALSE)
	{
		return;
	}
	CString szLog;

	ULONG ulBin	= (ULONG)((*m_psmfSRam)["BinTable"]["SPC"]["Block"]) - 1;

	if (ulBin > BPR_MAX_BINBLOCK)
	{
		return;
	}

	//Klocwork	//v4.02T5
	ulBin	= min(ulBin, BPR_MAX_BINBLOCK - 1);
	ulBin	= max(ulBin, 0);

	//Start checking
	if (bIsArm1)
	{
		if (m_lOutEmptyCounter[ulBin] > 0)
		{
			m_lOutEmptyCounter[ulBin]--;
		}

		if (m_lGenMaxAccEmptyAllow[ulBin] > 0)
		{
			if (m_lOutAccEmptyCounter[ulBin] > 0)
			{
				m_lOutAccEmptyCounter[ulBin]--;
			}
			if (m_lOutAccEmptyCounterTotal[ulBin] > 0)
			{
				m_lOutAccEmptyCounterTotal[ulBin]--;
			}
		}
		szLog.Format("(DecreaseDieEmpty)BH1 PB EMPTY Total=%ld, ACC=%ld (%ld)", 
			m_lOutEmptyCounter[ulBin], m_lOutAccEmptyCounter[ulBin], m_lGenMaxAccEmptyAllow[ulBin]);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
	else
	{
		if (m_lOutEmptyCounter2[ulBin] > 0)
		{
			m_lOutEmptyCounter2[ulBin]--;
		}

		if (m_lGenMaxAccEmptyAllow[ulBin] > 0)
		{
			if (m_lOutAccEmptyCounter2[ulBin] > 0)
			{
				m_lOutAccEmptyCounter2[ulBin]--;
			}
			if (m_lOutAccEmptyCounterTotal[ulBin] > 0)
			{
				m_lOutAccEmptyCounterTotal[ulBin]--;
			}
		}
		szLog.Format("(DecreaseDieEmpty)BH2 PB EMPTY Total=%ld, ACC=%ld (%ld)", 
			m_lOutEmptyCounter2[ulBin], m_lOutAccEmptyCounter2[ulBin], m_lGenMaxAccEmptyAllow[ulBin]);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
}

/*
LONG CBondPr::DrawThePrWindow()
{
	if (m_bEnableMS100EjtXY)	//v4.52A14
	{
		PR_COORD stBH1BondCorner1, stBH1BondCorner2;
		PR_COORD stBH2BondCorner1, stBH2BondCorner2;
		CString szBH1Log, szBH2Log;

		stBH1BondCorner1 = m_stSearchArea.coCorner1;
		stBH1BondCorner2 = m_stSearchArea.coCorner2;
		stBH2BondCorner1 = m_stSearchArea.coCorner1;
		stBH2BondCorner2 = m_stSearchArea.coCorner2;
		UpdateBH1BH2SearchRange(stBH1BondCorner1, stBH1BondCorner2, stBH2BondCorner1, stBH2BondCorner2, szBH1Log, szBH2Log);

		DrawRectangleBox(stBH1BondCorner1, stBH1BondCorner2, PR_COLOR_RED);	
		DrawRectangleBox(stBH2BondCorner1, stBH2BondCorner2, PR_COLOR_YELLOW);
	}

	return 1;
}
*/

LONG CBondPr::LogPostBondData(LONG lPostBondType, PR_UWORD usDieType, PR_REAL fDieRotate, PR_COORD stDieOffset)
{
	int		siStepX, siStepY;
	double	dUnitX, dUnitY;
	double	dUnitXForDisplay, dUnitYForDisplay;
	LONG	lDieClass = -1;
	BOOL	bDieIsGood = FALSE;
	BOOL	bDieIsBonded = FALSE;
	ULONG	ulCurrentBin = 1;
	ULONG	ulCurrentIndex = 0;
	BOOL	bIsArm1 = TRUE;
	CString szStr;
	CString szLog;

	bIsArm1 = (BOOL)(LONG)(*m_psmfSRam)["BondPr"]["IsArm1"];
	siStepX	= 0; 
	siStepY = 0;
	dUnitX	= 0;
	dUnitY	= 0;
	DOUBLE dAvgUnitOffsetX = 0;
	DOUBLE dAvgUnitOffsetY = 0;
	LONG lAvgOffsetX = 0;
	LONG lAvgOffsetY = 0;
	LONG lPBEmptyStatus = TRUE;

	CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
	ConvertUnit(siStepX, siStepY, &dUnitX, &dUnitY);	// motor steps to mil

	//v4.44A3	//relative motor steps of die to BOND PR center
	LONG lPrCenterStepX = siStepX;
	LONG lPrCenterStepY = siStepY;
	//DrawThePrWindow();

	if (DieIsGood(usDieType) == TRUE)
	{
		lDieClass = 0;
		bDieIsGood = TRUE;
	}
	else if (DieIsDefective(usDieType) == TRUE)
	{
		lDieClass = 1;
		bDieIsGood = FALSE;
	}
	else if (DieIsEmpty(usDieType) == TRUE)
	{
		lDieClass = 2;
		bDieIsGood = FALSE;
	}

	//v4.59A38	//Nichia MS90
	if (!bDieIsGood && IsMS90() && CMS896AStn::m_b2Parts2ndPartStart)	//If wafer is rotated 180 deg
	{
		if ((siStepX == 0) && (siStepY == 0))	// 0 = no SrchDie result
		{
			fDieRotate = 0.00;					// Use 0 instead of 180.0 if die is rotated
		}
	}

	//ULONG ulCurrBin	= (ULONG)((*m_psmfSRam)["BinTable"]["SPC"]["Block"]) - 1;
	//LogPBDataWithRowCol(m_lBTRow, m_lBTCol, bDieIsGood,		//v4.59A35
	//					dUnitX, 
	//					dUnitY, 
	//					ulCurrBin, (double) fDieRotate);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bEnableBHUplookPr = pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR);

	DOUBLE dPrCentreWithOffsetX = 0;
	DOUBLE dPrCentreWithOffsetY = 0;
	DOUBLE dCalibX = GetCalibX();
	DOUBLE dCalibY = GetCalibY();


	if (bDieIsGood)	//if the die is not good, do not add the offset to avoid abnormal postbond data
	{
		LONG lCollet1OffsetX	= 0, lCollet1OffsetY	= 0;
		LONG lCollet2OffsetX	= 0, lCollet2OffsetY	= 0;

		/*
		if (bIsArm1 == FALSE)	// -> BHZ2 towards PICK!
		{
			//calculate PR Centre with offset
			LONG lBHZ2BondPosOffsetX = 0; //(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetX"];
			LONG lBHZ2BondPosOffsetY = 0; //(*m_psmfSRam)["BinTable"]["BHZ2BondPosOffsetY"];

			DOUBLE dOrgPrCentreWithOffsetX = 0;
			DOUBLE dOrgPrCentreWithOffsetY = 0;
			DOUBLE dtestCol2X = 0;
			DOUBLE dtestCol2Y = 0;
			LONG lBH2UplookOffSetX = 0;
			LONG lBH2UplookOffSetY = 0;
			
			if (m_bEnableMS100EjtXY)
			{
				GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, TRUE);
			}
			
			if (fabs(dCalibX) > 0.0000001)
			{
				dPrCentreWithOffsetX = m_lPrCenterX + ((lBHZ2BondPosOffsetX - lCollet2OffsetX) / dCalibX);
				dtestCol2X			 = (lBHZ2BondPosOffsetX - lCollet2OffsetX) / dCalibX;
			}
			if (fabs(dCalibY) > 0.0000001)
			{
				dPrCentreWithOffsetY = m_lPrCenterY + ((lBHZ2BondPosOffsetY - lCollet2OffsetY) / dCalibY);
				dtestCol2Y			 = (lBHZ2BondPosOffsetY - lCollet2OffsetY) / dCalibY;

			}
			//4.52D17cal PostBond (add uplook offset) 
			if(bEnableBHUplookPr)
			{
				lBH2UplookOffSetX = (LONG)(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetX"];
				lBH2UplookOffSetY = (LONG)(*m_psmfSRam)["BinTable"]["BH2 UplookOffsetY"];

				//lBH2UplookOffSetX = 0 - lBH2UplookOffSetX;
				//lBH2UplookOffSetY = 0 - lBH2UplookOffSetY;

//				DOUBLE dOffsetInPixel;
				if (fabs(dCalibX) > 0.0000001)
				{
					dOrgPrCentreWithOffsetX = dPrCentreWithOffsetX;
					dPrCentreWithOffsetX	= dPrCentreWithOffsetX - (lBH2UplookOffSetX / dCalibX) * 10;		// - (lBH2UplookOffSetY / m_dCalibYX)*2;
					//dPrCentreWithOffsetX	= dPrCentreWithOffsetX - (lBH2UplookOffSetX / 0.64); //CSP006

				}

				if (fabs(dCalibY) > 0.0000001)
				{
					dOrgPrCentreWithOffsetY = dPrCentreWithOffsetY;
					dPrCentreWithOffsetY	= dPrCentreWithOffsetY - (lBH2UplookOffSetY / dCalibY) * 10;		// - (lBH2UplookOffSetX / m_dCalibXY)*2;
					//dPrCentreWithOffsetY	= dPrCentreWithOffsetY - (lBH2UplookOffSetY / 0.64); //CSP006
				}
			}

			//calculate die compensation with Centre Offset
			PR_COORD	stRelMove;
			stRelMove.x = (PR_WORD)dPrCentreWithOffsetX - stDieOffset.x;
			stRelMove.y = (PR_WORD)dPrCentreWithOffsetY - stDieOffset.y;

			ConvertPixelToMotorStep(stRelMove, &siStepX, &siStepY);
			ConvertUnit(siStepX, siStepY, &dUnitX, &dUnitY);

			if (!m_bPostBondAtBond)		//RT Record added in AutoLogResult() instead
				AddRTBHCompXY(2, siStepX, siStepY);		//v3.86
			UpdateAvgRTCompOffsetXY(2, lAvgOffsetX, lAvgOffsetY, dAvgUnitOffsetX, dAvgUnitOffsetY);

			//v4.57A11
			if(bEnableBHUplookPr)
			{
				szLog.Format("BPR: PB (all in Pixels) - PR-Center(%ld, %ld); BH2-Offset(%ld, %ld); EJTC2Offset(%ld, %ld), UplookXY(%ld, %ld), CalibXY(%.2f, %.2f), new-PR-Center(%.2f, %.2f), DieOffset(%ld, %ld); FinalOffset(%ld, %ld), FinalOffsetInMil(%.2f, %.2f), RTOffset(%.3f, %.3f)",
							m_lPrCenterX, m_lPrCenterY, 
							lBHZ2BondPosOffsetX, lBHZ2BondPosOffsetY,
							lCollet2OffsetX, lCollet2OffsetY,
							lBH2UplookOffSetX, lBH2UplookOffSetY,
							dCalibX, dCalibY,
							dPrCentreWithOffsetX, dPrCentreWithOffsetY,
							stDieOffset.x, stDieOffset.y,
							stRelMove.x, stRelMove.y,
							dUnitX, dUnitY,
							dAvgUnitOffsetX, dAvgUnitOffsetY);
			}
			else
			{
				szLog.Format("BPR: PB (all in Pixels) - PR-Center(%ld, %ld); BH2-Offset(%ld, %ld); EJTC2Offset(%ld, %ld), CalibXY(%.2f, %.2f), new-PR-Center(%.2f, %.2f), DieOffset(%ld, %ld); FinalOffset(%ld, %ld), FinalOffsetInMil(%.2f, %.2f), RTOffset(%.3f, %.3f)",
							m_lPrCenterX, m_lPrCenterY, 
							lBHZ2BondPosOffsetX, lBHZ2BondPosOffsetY,
							lCollet2OffsetX, lCollet2OffsetY,
							dCalibX, dCalibY,
							dPrCentreWithOffsetX, dPrCentreWithOffsetY,
							stDieOffset.x, stDieOffset.y,
							stRelMove.x, stRelMove.y,
							dUnitX, dUnitY,
							dAvgUnitOffsetX, dAvgUnitOffsetY);
			}
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

		}
		else*/
		{	
			//calculate PR Centre with offset

			LONG lBHZ1BondPosOffsetX = 0; //(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetX"];
			LONG lBHZ1BondPosOffsetY = 0; //(*m_psmfSRam)["BinTable"]["BHZ1BondPosOffsetY"];

			DOUBLE dOrgPrCentreWithOffsetX = 0;
			DOUBLE dOrgPrCentreWithOffsetY = 0;
			DOUBLE dtestCol1X = 0;
			DOUBLE dtestCol1Y = 0;
			LONG lBH1UplookOffSetX = 0;
			LONG lBH1UplookOffSetY = 0;
			PR_COORD	stRelMove;
			stRelMove.x	= 0;
			stRelMove.y = 0;
			
			if (m_bEnableMS100EjtXY /*&& m_bBTIsMoveCollet*/)	//v4.52A14
			{
				GetBTColletOffsetCount(lCollet1OffsetX, lCollet1OffsetY, lCollet2OffsetX, lCollet2OffsetY, m_dBTResolution_UM_CNT, TRUE);
		
				if (fabs(dCalibX) > 0.0000001)
				{
					dPrCentreWithOffsetX = m_lPrCenterX + ((lBHZ1BondPosOffsetX - lCollet1OffsetX) / dCalibX);
					dtestCol1X = (lBHZ1BondPosOffsetX - lCollet1OffsetX) / dCalibX;
				}

				if (fabs(dCalibY) > 0.0000001)
				{
					dPrCentreWithOffsetY = m_lPrCenterY + ((lBHZ1BondPosOffsetY - lCollet1OffsetY) / dCalibY);
					dtestCol1Y = (lBHZ1BondPosOffsetY - lCollet1OffsetY) / dCalibY;
				}
				//4.52D17cal PostBond (add uplook offset) 
				if(bEnableBHUplookPr)
				{
					//in um
					lBH1UplookOffSetX = (LONG)(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetX"];
					lBH1UplookOffSetY = (LONG)(*m_psmfSRam)["BinTable"]["BH1 UplookOffsetY"];

					if (fabs(dCalibX) > 0.0000001)
					{
						dOrgPrCentreWithOffsetX = dPrCentreWithOffsetX;
						dPrCentreWithOffsetX	= dPrCentreWithOffsetX - (lBH1UplookOffSetX / dCalibX) * 10;		// - (lBH1UplookOffSetY / m_dCalibYX)*2;
						//dPrCentreWithOffsetX	= dPrCentreWithOffsetX - (lBH1UplookOffSetX / 0.64);  //CSP006

					}

					if (fabs(dCalibY) > 0.0000001)
					{
						dOrgPrCentreWithOffsetY = dPrCentreWithOffsetY;
						dPrCentreWithOffsetY	= dPrCentreWithOffsetY - (lBH1UplookOffSetY / dCalibY) * 10;		// - (lBH1UplookOffSetX / m_dCalibXY)*2;
						//dPrCentreWithOffsetY	= dPrCentreWithOffsetY - (lBH1UplookOffSetY /0.64);  //CSP006
					}

				}
				//calculate die compensation with Centre Offset
				stRelMove.x = (PR_WORD)dPrCentreWithOffsetX - stDieOffset.x;
				stRelMove.y = (PR_WORD)dPrCentreWithOffsetY - stDieOffset.y;
				ConvertPixelToMotorStep(stRelMove, &siStepX, &siStepY);
				ConvertUnit(siStepX, siStepY, &dUnitX, &dUnitY);
			}
			
			if (!m_bPostBondAtBond)		//RT Record added in AutoLogResult() instead
				AddRTBHCompXY(1, siStepX, siStepY);		//v3.86
			UpdateAvgRTCompOffsetXY(1, lAvgOffsetX, lAvgOffsetY, dAvgUnitOffsetX, dAvgUnitOffsetY);

			//v4.57A11
			/*if (bEnableBHUplookPr)
			{
				szLog.Format("BPR: PB (all in Pixels) - PR-Center(%ld, %ld); BH1, EJTC1Offset(%ld, %ld), UplookXY(%ld, %ld), CalibXY(%.2f, %.2f), new-PR-Center(%.2f, %.2f), DieOffset(%ld, %ld), FinalOffset(%ld, %ld), FinalOffsetInMil(%.2f, %.2f), RTOffset(%.3f, %.3f)",
							m_lPrCenterX, m_lPrCenterY, 
							lCollet1OffsetX, lCollet1OffsetY,
							lBH1UplookOffSetX, lBH1UplookOffSetY,
							dCalibX, dCalibY,
							dPrCentreWithOffsetX, dPrCentreWithOffsetY,
							stDieOffset.x, stDieOffset.y,
							stRelMove.x, stRelMove.y,
							dUnitX, dUnitY,
							dAvgUnitOffsetX, dAvgUnitOffsetY);
			}
			else
			{*/
			szLog.Format("BPR: PB (all in Pixels) - PR-Center(%ld, %ld); CalibXY(%.2f, %.2f), new-PR-Center(%.2f, %.2f), DieOffset(%ld, %ld), FinalOffset(%ld, %ld), FinalOffsetInMil(%.2f, %.2f), RTOffset(%.3f, %.3f)",
						m_lPrCenterX, m_lPrCenterY, 
						dCalibX, dCalibY,
						dPrCentreWithOffsetX, dPrCentreWithOffsetY,
						stDieOffset.x, stDieOffset.y,
						stRelMove.x, stRelMove.y,
						dUnitX, dUnitY,
						dAvgUnitOffsetX, dAvgUnitOffsetY);
			//}
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		}

		//v4.44T3	//Semitek
		//Calculate frame angle for bin realignment based on postbond die offset XY
		/*if (CMS896AApp::m_bBinFrameNewRealignMethod || m_bSemitekBLMode)	
		{
			BOOL bEnable1stDie	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"];
			BOOL bEnable2ndDie	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Enable"];
			BOOL bEnableMidDice = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Enable"];

			if (bEnable1stDie || bEnable2ndDie || bEnableMidDice)
			{
				LONG lEncX=0, lEncY=0;
				LONG lNewEncX=0, lNewEncY=0;
				ULONG ulBlockID	= (ULONG)((*m_psmfSRam)["BinTable"]["SPC"]["Block"]);
				CString szLog;

				if (bEnable1stDie)
				{
					lEncX = (LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"];
					lEncY = (LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"];
					//v4.44A3	//New Enc relative to BOND PR center instead of collet offset
					lNewEncX = lEncX + lPrCenterStepX;	//siStepX;		//v4.45T3
					lNewEncY = lEncY + lPrCenterStepY;	//siStepY;

					szLog.Format("AutoCalAngle on Bin #%ld: Enc_A(%ld, %ld), PB NewEnc_A(%ld, %ld), PBOffset(%ld, %ld)\n", 
							ulBlockID, lEncX, lEncY, lNewEncX, lNewEncY, lPrCenterStepX, lPrCenterStepY);
					//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
					CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

					(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"]		= lNewEncX;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"]		= lNewEncY;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= FALSE;
					bEnableMidDice = FALSE;
				}
				else if (bEnable2ndDie)
				{
					lEncX = (LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["X"];
					lEncY = (LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Y"];
					//v4.44A3	//New Enc relative to BOND PR center instead of collet offset
					lNewEncX = lEncX + lPrCenterStepX;	//siStepX;		//v4.45T3
					lNewEncY = lEncY + lPrCenterStepY;	//siStepY;

					szLog.Format("AutoCalAngle on Bin #%ld: Enc_B(%ld, %ld), PB NewEnc_B(%ld, %ld), PBOffset(%ld, %ld)\n", 
							ulBlockID, lEncX, lEncY, lNewEncX, lNewEncY, lPrCenterStepX, lPrCenterStepY);
					//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
					CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

					(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["X"]			= lNewEncX;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Y"]			= lNewEncY;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Enable"]	= FALSE;
					(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Enable"]		= FALSE;
					bEnableMidDice = FALSE;
				}

				//v4.46T11		//For logging purpose only
				if (bEnableMidDice)
				{
					lEncX = (LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["X"];
					lEncY = (LONG)(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Y"];
					lNewEncX = lEncX + lPrCenterStepX;
					lNewEncY = lEncY + lPrCenterStepY;

					szLog.Format("PB Die (Bin #%ld), Enc(%ld, %ld), PB NewEnc(%ld, %ld), PBOffset(%ld, %ld)", 
							ulBlockID, lEncX, lEncY, lNewEncX, lNewEncY, lPrCenterStepX, lPrCenterStepY);
					CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
				}
			}
		}*/

	}	//if (bDieIsGood)
	

	//TakeTime(Z5);//4.52D17Time Testing 
	CString szLog2;

	//Log Result
	if (lPostBondType == BPR_POSTBOND_RUNTIME)		
	{
		//Get Current BinBlock no & Index
		ulCurrentBin	= (ULONG)((*m_psmfSRam)["BinTable"]["SPC"]["Block"]) - 1;
		ulCurrentIndex	= (ULONG)(*m_psmfSRam)["BinTable"]["CurrIndex"];
		bDieIsBonded	= (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["SPC"]["Bonded"];

		if ( ulCurrentBin > BPR_MAX_BINBLOCK )
		{
			return FALSE;
		}

		if ( (m_ulLastBin == ulCurrentBin) && (m_ulLastIndex == ulCurrentIndex) )
		{
			return TRUE;
		}

		//Update Last value
		m_ulLastBin		= ulCurrentBin;
		m_ulLastIndex	= ulCurrentIndex;

		//Klocwork	//v4.02T5
		ulCurrentBin	= min(ulCurrentBin, BPR_MAX_BINBLOCK-1);
		ulCurrentBin	= max(ulCurrentBin, 0);

		//RunTime result
		m_lTotalDieBonded[ulCurrentBin]++;
		m_dTotalAngle[ulCurrentBin] += (double)fDieRotate;
		m_dOutAverageAngle[ulCurrentBin] = m_dTotalAngle[ulCurrentBin] / ((DOUBLE)(m_lTotalDieBonded[ulCurrentBin]) + 1);

		//v4.40T7	//Nichia
		dUnitXForDisplay = dUnitX;
		dUnitYForDisplay = dUnitY;
		if (m_bDisplayUnitInUm)
		{
			ConvertUnitToUm(siStepX, siStepY, &dUnitXForDisplay, &dUnitYForDisplay);
		}


		//v4.42T12	//Nichia to detect/protect against die crack on PICK side 
					//	due to die-pick-back problem;
		if (CheckDieEmpty(ulCurrentBin, lDieClass, bIsArm1) != TRUE)
		{
			lPBEmptyStatus = BPR_ERR_SPC_EMPTY;
			(*m_psmfSRam)["BondPr"]["PostBondResult"] = lPBEmptyStatus;
			(*m_psmfSRam)["BondPr"]["MaxEmptyAllow"] = m_lGenMaxEmptyAllow[ulCurrentBin];
			(*m_psmfSRam)["BondPr"]["MaxAccEmptyAllow"] = m_lGenMaxAccEmptyAllow[ulCurrentBin];
		}


		//****** Update SPC for individual bin ********//	//v4.47A7	//Re-enabled
		*m_pDieIndex[ulCurrentBin]		= ulCurrentIndex;
		if (m_bEnableRTCompXY)		//v3.86
		{
			*m_pDieX[ulCurrentBin]		= dUnitXForDisplay - dAvgUnitOffsetX;
			*m_pDieY[ulCurrentBin]		= dUnitYForDisplay - dAvgUnitOffsetY;

			szLog2.Format("A,%d,%d,%d,%d,", siStepX, siStepY, dUnitXForDisplay - dAvgUnitOffsetX, dUnitYForDisplay - dAvgUnitOffsetY);
		}
		else
		{
			*m_pDieX[ulCurrentBin]		= dUnitXForDisplay;
			*m_pDieY[ulCurrentBin]		= dUnitYForDisplay;
			szLog2.Format("B,%d,%d,%d,%d,", siStepX, siStepY, dUnitXForDisplay, dUnitYForDisplay);
		}

		*m_pDieTheta[ulCurrentBin]		= (double)(fDieRotate);
		*m_pDieType[ulCurrentBin]		= lDieClass;
		*m_pDieIsGood[ulCurrentBin]		= bDieIsGood;
		*m_pDieAvgAngle[ulCurrentBin]	= m_dOutAverageAngle[ulCurrentBin];

		m_pBin[ulCurrentBin]->Log();

		
		//****** Update SPC for Display ********//
		if (m_bEnableRTCompXY)		//v3.86
		{
			*m_pDspDieX	= dUnitXForDisplay - dAvgUnitOffsetX;
			*m_pDspDieY	= dUnitYForDisplay - dAvgUnitOffsetY;

			LogPBDataWithRowCol(m_lBTRow, m_lBTCol, bDieIsGood,		//v4.59A35
								dUnitXForDisplay - dAvgUnitOffsetX, 
								dUnitYForDisplay - dAvgUnitOffsetY, 
								ulCurrentBin, (double) fDieRotate);
		}
		else
		{
			*m_pDspDieX	= dUnitXForDisplay;
			*m_pDspDieY	= dUnitYForDisplay;

			LogPBDataWithRowCol(m_lBTRow, m_lBTCol, bDieIsGood,		//v4.59A35
								dUnitXForDisplay, 
								dUnitYForDisplay, 
								ulCurrentBin, (double) fDieRotate);
		}

		*m_pDspDieTheta	= (double)(fDieRotate);
		m_pDspBin->Log();

		// set to TRUE such that postbond data need to be exported for this bin grade at StopOperation() or AutoClrCount()
		m_arbIsPostBond[ulCurrentBin] = TRUE;


		//****** Update SPC for BHZ1 & BHZ2 Display ********//
		//if (bIsArm1)	//Arm1
		//{
		//Display result
		if (m_bEnableRTCompXY)		//v3.86
		{
			*m_pDspDieX_Arm1				= dUnitXForDisplay - dAvgUnitOffsetX;
			*m_pDspDieY_Arm1				= dUnitYForDisplay - dAvgUnitOffsetY;
		}
		else
		{
			*m_pDspDieX_Arm1				= dUnitXForDisplay;
			*m_pDspDieY_Arm1				= dUnitYForDisplay;
		}
		*m_pDspDieTheta_Arm1				= (double)(fDieRotate);
		m_pDspBin_Arm1->Log();

		//}
		/*else	//Arm2
		{
			//Display result
			if (m_bEnableRTCompXY)		//v3.86
			{
				*m_pDspDieX_Arm2				= dUnitXForDisplay - dAvgUnitOffsetX;
				*m_pDspDieY_Arm2				= dUnitYForDisplay - dAvgUnitOffsetY;
			}
			else
			{
				*m_pDspDieX_Arm2				= dUnitXForDisplay;
				*m_pDspDieY_Arm2				= dUnitYForDisplay;
			}
			*m_pDspDieTheta_Arm2				= (double)(fDieRotate);
			m_pDspBin_Arm2->Log();
		}*/

		//Check Current data is out of the following setting			
		if (m_bEnableRTCompXY)		//v3.86
		{
			if (CheckDieShifted(ulCurrentBin, 
									dUnitXForDisplay - dAvgUnitOffsetX,				//v4.59A4
									dUnitYForDisplay - dAvgUnitOffsetY) != TRUE)	//v4.59A4
			{
				OpEnableElectech3EDLPostBondChecking(ulCurrentBin+1);		//v4.50A31	//v4.51A3
				return BPR_ERR_SPC_XY_LIMIT;
			}
		}
		else
		{
			if (CheckDieShifted(ulCurrentBin, dUnitXForDisplay, dUnitYForDisplay) != TRUE)	
			{
				return BPR_ERR_SPC_XY_LIMIT;
			}
		}


		//andrewng
		//Grab another image for PAD PostBond & LookForward alignment
		//AutoSearchRTPadOffsetXY(bIsArm1, (INT) stDieOffset.x, (INT) stDieOffset.y, 
		//				siStepX, siStepY, dPrCentreWithOffsetX, dPrCentreWithOffsetY);
		//AutoLookForwardNextPad(ulCurrentBin, bIsArm1, FALSE);
		//CMSLogFileUtility::Instance()->BT_TableIndex2Log(szLog2);	//andrewng12

		//TakeTime(Z6);//4.52D17Time Testing 

		BOOL bError = FALSE;
		//andrewng //2020-0616	//Not used for HW Trigger Mode
		//if (!AutoLFLastRowDieInFOV(bError, ulCurrentBin, stDieOffset, FALSE))	//v4.47T8	//SanAn WH
		//{
		//	return BPR_ERR_SPC_XY_LIMIT;	//v4.47T10
		//}

		if (CheckMaxRotation(ulCurrentBin, (double)(fDieRotate), bIsArm1) != TRUE)
		{
			if (bIsArm1)
			{
				return BPR_ERR_SPC_MAX_ANGLE;
			}
			else
			{
				return BPR_ERR_SPC_MAX_ANGLE2;		//v4.57A14
			}
		}

		//v4.58A5
		if (CheckMaxAccRotation(ulCurrentBin, (double)(fDieRotate), bIsArm1) != TRUE)
		{
			if (bIsArm1)
			{
				return BPR_ERR_SPC_MAX_ACC_ANGLE;
			}
			else
			{
				return BPR_ERR_SPC_MAX_ACC_ANGLE2;	
			}
		}

		if (CheckAverageRotation(ulCurrentBin) != TRUE)
		{
			return BPR_ERR_SPC_AVG_ANGLE;
		}

		if (CheckAverageCp(ulCurrentBin) != TRUE)	//v4.51A17
		{
			return BPR_ERR_SPC_XY_LIMIT;
		}

		if (CheckDieDefect(ulCurrentBin, lDieClass) != TRUE)
		{
			return BPR_ERR_SPC_DEFECT;
		}

		//v4.42T12	//detect EMPTY die before SPC above to speed up the checking	//Nichia
		if (lPBEmptyStatus == BPR_ERR_SPC_EMPTY)
		{
			return BPR_ERR_SPC_EMPTY;
		}
	}

	return TRUE;
}


LONG CBondPr::ExportPostBondData(ULONG ulCurrentBin)
{
	SPC_CPart::SPC_ERetrievalResult		eResult;
	SPC_CProperty::SPC_EPropertyType	eType1;
	SPC_CProperty::SPC_EPropertyType	eType3;
	SPC_CProperty::SPC_EPropertyType	eType4;
	SPC_CProperty::SPC_EPropertyType	eType5;
	SPC_CProperty::SPC_EPropertyType	eType6;
	SPC_CProperty::SPC_EPropertyType	eType7;
	SPC_CProperty::SPC_EPropertyType	eType8;
	SPC_CProperty::SPC_EPropertyType	eType9;
	SPC_CProperty::SPC_EPropertyType	eType10;

	unsigned long ulTempDataID;
	unsigned long ulParentID;
	unsigned long ulOwnID;

	void	*pBinIndex;
	void	*pDieRow;
	void	*pDieCol;
	void	*pDieX;
	void	*pDieY;
	void	*pDieAngle;
	void	*pDieAvgAngle;
	void	*pDieIsGood;
	void	*pDieType;
	ULONG	ulIndex;	
	BOOL	bEndOfRecord = FALSE;	

	ULONG	ulSPCRow = 0;
	ULONG	ulSPCCol = 0;
	DOUBLE	dSPCDieX = 0;
	DOUBLE	dSPCDieY = 0;
	DOUBLE	dSPCDieAngle = 0;
	BOOL	bSPCDIeIsGood = FALSE;
	UCHAR	ucSPCDieType = 0;

	LONG	lSPCTotalDieCount = 0;
	LONG	lSPCGoodDieCount = 0;
	DOUBLE	dSPCAverageAngle = 0.0; 
	CString	szFilePath;

	CTime	TimeStamp; 

	//v3.93		//No need to access if no postbond data being logged for this bin grade
	if (!m_arbIsPostBond[ulCurrentBin])
		return 0;

	szFilePath.Format(m_szPostbondDataSource + "\\Temp\\" + "%d", ulCurrentBin+1);

	FILE	*fp = NULL;
	errno_t nErr = fopen_s(&fp, (LPCTSTR)szFilePath, "w+");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%ld\n", ulCurrentBin+1);

		//Ready get data from SPC
		m_pBin[ulCurrentBin]->Flush();
		m_pBin[ulCurrentBin]->BeginRetrieveFromStart(0,1);

		pBinIndex		= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Bin Index", eType1);
		pDieRow			= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Bin Row", eType9);
		pDieCol			= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Bin Col", eType10);
		pDieX			= m_pBin[ulCurrentBin]->GetVariableInFileProperty("X Placement", eType3);
		pDieY			= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Y Placement", eType4);
		pDieAngle		= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Die Rotation", eType5);
		pDieAvgAngle	= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Average Die Rotation", eType6);
		pDieIsGood		= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Good Die", eType7);
		pDieType		= m_pBin[ulCurrentBin]->GetVariableInFileProperty("Die Type", eType8);

		while(!bEndOfRecord)
		{
			eResult = m_pBin[ulCurrentBin]->RetrieveAdvance(ulTempDataID, TimeStamp, ulParentID, ulOwnID);
			bEndOfRecord = eResult == SPC_CPart::SPC_RETRIEVE_END;

			if (!bEndOfRecord)
			{
				//Get raw data....	
				ulIndex			= SPC_CProperty::ConvertToULong(pBinIndex, eType1);
				ulSPCRow		= SPC_CProperty::ConvertToULong(pDieRow, eType9);
				ulSPCCol		= SPC_CProperty::ConvertToULong(pDieCol, eType10);
				dSPCDieX		= SPC_CProperty::ConvertToDouble(pDieX, eType3);
				dSPCDieY		= SPC_CProperty::ConvertToDouble(pDieY, eType4);
				dSPCDieAngle	= SPC_CProperty::ConvertToDouble(pDieAngle, eType5);
				bSPCDIeIsGood	= (BOOL)SPC_CProperty::ConvertToUChar(pDieIsGood, eType7);
				ucSPCDieType	= SPC_CProperty::ConvertToUChar(pDieType, eType8);
				
				//Update SPC Summay variable
				if (lSPCTotalDieCount == 0)
				{
					//Get average angle from last unit
					dSPCAverageAngle = SPC_CProperty::ConvertToDouble(pDieAvgAngle, eType6);
				}

				if (bSPCDIeIsGood == TRUE)
				{
					lSPCGoodDieCount++;
				}

				lSPCTotalDieCount++;

				//ouput each data
				fprintf(fp, "%lu,%f,%f,%f,%d,%d\n", ulIndex+1, dSPCDieX, dSPCDieY, dSPCDieAngle, bSPCDIeIsGood, ucSPCDieType);
			}

			//Prevent infinite loop
			if ( lSPCTotalDieCount > 99999 )
			{
				break;
			}
		}
		m_pBin[ulCurrentBin]->EndRetrieve();

		fprintf(fp, "End of SPC Data Details\n");

		//Output summay
		fprintf(fp, "%ld\n", lSPCTotalDieCount);
		fprintf(fp, "%ld\n", lSPCGoodDieCount);
		fprintf(fp, "%f\n", dSPCAverageAngle);
		fprintf(fp, "End\n");

		fclose(fp);
	}

	if (lSPCTotalDieCount > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}


BOOL CBondPr::ExportPostBondDisplayData()
{
	SPC_CPart::SPC_ERetrievalResult		eResult;
	SPC_CProperty::SPC_EPropertyType	eType1;
	SPC_CProperty::SPC_EPropertyType	eType3;
	SPC_CProperty::SPC_EPropertyType	eType4;
	SPC_CProperty::SPC_EPropertyType	eType5;

	unsigned long ulTempDataID;
	unsigned long ulParentID;
	unsigned long ulOwnID;

	void	*pBinIndex;
	void	*pDieX;
	void	*pDieY;
	void	*pDieAngle;

	ULONG	ulIndex = 0;	
	BOOL	bEndOfRecord = FALSE;	

	ULONG	ulSPCRow = 0;
	ULONG	ulSPCCol = 0;
	DOUBLE	dSPCDieX = 0;
	DOUBLE	dSPCDieY = 0;
	DOUBLE	dSPCDieAngle = 0;
	BOOL	bSPCDIeIsGood = FALSE;
	UCHAR	ucSPCDieType = 0;

	LONG	lSPCTotalDieCount = 0;
	LONG	lSPCGoodDieCount = 0;
	DOUBLE	dSPCAverageAngle = 0.0; 
	CString	szFilePath;

	CTime	TimeStamp; 

	szFilePath = gszUSER_DIRECTORY + "\\OutputFile\\Postbond\\pb_RT.txt";	//v4.12

	FILE	*fp = NULL;
	errno_t nErr = fopen_s(&fp, (LPCTSTR) szFilePath, "w+");
	if ((nErr != 0) || (fp == NULL))
	{
		return FALSE;
	}

	fprintf(fp, "DieNo,IsGoodDie,X_mil,Y_mil,T_Degree\n");

	//Ready get data from SPC
	m_pDspBin->Flush();
	m_pDspBin->BeginRetrieveFromStart(0, 1);

	pBinIndex		= m_pDspBin->GetVariableInFileProperty("Bin Index", eType1);
	pDieX			= m_pDspBin->GetVariableInFileProperty("X Placement", eType3);
	pDieY			= m_pDspBin->GetVariableInFileProperty("Y Placement", eType4);
	pDieAngle		= m_pDspBin->GetVariableInFileProperty("Die Rotation", eType5);

	while (!bEndOfRecord)
	{
		eResult = m_pDspBin->RetrieveAdvance(ulTempDataID, TimeStamp, ulParentID, ulOwnID);
		bEndOfRecord = eResult == SPC_CPart::SPC_RETRIEVE_END;

		if (!bEndOfRecord)
		{
			//Get raw data....	
			ulIndex++;	//			= SPC_CProperty::ConvertToULong(pBinIndex, eType1);
			dSPCDieX		= SPC_CProperty::ConvertToDouble(pDieX, eType3);
			dSPCDieY		= SPC_CProperty::ConvertToDouble(pDieY, eType4);
			dSPCDieAngle	= SPC_CProperty::ConvertToDouble(pDieAngle, eType5);
			
			//ouput each data
			fprintf(fp, "%lu,1,%.1f,%.1f,%.2f\n", ulIndex, dSPCDieX, dSPCDieY, dSPCDieAngle);
		}
	}

	m_pDspBin->EndRetrieve();
	fclose(fp);

	return TRUE;
}


LONG CBondPr::ExportPostBondData_Arm1(ULONG ulCurrentBin)
{
	SPC_CPart::SPC_ERetrievalResult		eResult;
	SPC_CProperty::SPC_EPropertyType	eType1;
	SPC_CProperty::SPC_EPropertyType	eType3;
	SPC_CProperty::SPC_EPropertyType	eType4;
	SPC_CProperty::SPC_EPropertyType	eType5;
	SPC_CProperty::SPC_EPropertyType	eType6;
	SPC_CProperty::SPC_EPropertyType	eType7;
	SPC_CProperty::SPC_EPropertyType	eType8;
	SPC_CProperty::SPC_EPropertyType	eType9;
	SPC_CProperty::SPC_EPropertyType	eType10;

	unsigned long ulTempDataID;
	unsigned long ulParentID;
	unsigned long ulOwnID;

	void	*pBinIndex;
	void	*pDieRow;
	void	*pDieCol;
	void	*pDieX;
	void	*pDieY;
	void	*pDieAngle;
	void	*pDieAvgAngle;
	void	*pDieIsGood;
	void	*pDieType;
	ULONG	ulIndex;	
	BOOL	bEndOfRecord = FALSE;	

	ULONG	ulSPCRow = 0;
	ULONG	ulSPCCol = 0;
	DOUBLE	dSPCDieX = 0;
	DOUBLE	dSPCDieY = 0;
	DOUBLE	dSPCDieAngle = 0;
	BOOL	bSPCDIeIsGood = FALSE;
	UCHAR	ucSPCDieType = 0;

	LONG	lSPCTotalDieCount = 0;
	LONG	lSPCGoodDieCount = 0;
	DOUBLE	dSPCAverageAngle = 0.0; 
	CString	szFilePath;

	CTime	TimeStamp; 

	//v3.93		//No need to access if no postbond data being logged for this bin grade
	if (!m_arbIsPostBond[ulCurrentBin])
		return 0;

	szFilePath.Format(m_szPostbondDataSource_Arm1 + "\\Temp\\" + "%d", ulCurrentBin+1);

	FILE	*fp = NULL;
	errno_t nErr = fopen_s(&fp, (LPCTSTR)szFilePath, "w+");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%ld\n", ulCurrentBin+1);

		//Ready get data from SPC
		m_pBin_Arm1[ulCurrentBin]->Flush();
		m_pBin_Arm1[ulCurrentBin]->BeginRetrieveFromStart(0,1);

		pBinIndex		= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Bin Index Arm1", eType1);
		pDieRow			= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Bin Row Arm1", eType9);
		pDieCol			= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Bin Col Arm1", eType10);
		pDieX			= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("X Placement Arm1", eType3);
		pDieY			= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Y Placement Arm1", eType4);
		pDieAngle		= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Die Rotation Arm1", eType5);
		pDieAvgAngle	= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Average Die Rotation Arm1", eType6);
		pDieIsGood		= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Good Die Arm1", eType7);
		pDieType		= m_pBin_Arm1[ulCurrentBin]->GetVariableInFileProperty("Die Type Arm1", eType8);

		while(!bEndOfRecord)
		{
			eResult = m_pBin_Arm1[ulCurrentBin]->RetrieveAdvance(ulTempDataID, TimeStamp, ulParentID, ulOwnID);
			bEndOfRecord = eResult == SPC_CPart::SPC_RETRIEVE_END;

			if (!bEndOfRecord)
			{
				//Get raw data....	
				ulIndex			= SPC_CProperty::ConvertToULong(pBinIndex, eType1);
				ulSPCRow		= SPC_CProperty::ConvertToULong(pDieRow, eType9);
				ulSPCCol		= SPC_CProperty::ConvertToULong(pDieCol, eType10);
				dSPCDieX		= SPC_CProperty::ConvertToDouble(pDieX, eType3);
				dSPCDieY		= SPC_CProperty::ConvertToDouble(pDieY, eType4);
				dSPCDieAngle	= SPC_CProperty::ConvertToDouble(pDieAngle, eType5);
				bSPCDIeIsGood	= (BOOL)SPC_CProperty::ConvertToUChar(pDieIsGood, eType7);
				ucSPCDieType	= SPC_CProperty::ConvertToUChar(pDieType, eType8);
				
				//Update SPC Summay variable
				if (lSPCTotalDieCount == 0)
				{
					//Get average angle from last unit
					dSPCAverageAngle = SPC_CProperty::ConvertToDouble(pDieAvgAngle, eType6);
				}

				if (bSPCDIeIsGood == TRUE)
				{
					lSPCGoodDieCount++;
				}

				lSPCTotalDieCount++;

				//ouput each data
				fprintf(fp, "%lu,%f,%f,%f,%d,%d\n", ulIndex+1, dSPCDieX, dSPCDieY, dSPCDieAngle, bSPCDIeIsGood, ucSPCDieType);
			}

			//Prevent infinite loop
			if ( lSPCTotalDieCount > 99999 )
			{
				break;
			}
		}
		m_pBin_Arm1[ulCurrentBin]->EndRetrieve();

		fprintf(fp, "End of SPC Data Details\n");

		//Output summay
		fprintf(fp, "%ld\n", lSPCTotalDieCount);
		fprintf(fp, "%ld\n", lSPCGoodDieCount);
		fprintf(fp, "%f\n", dSPCAverageAngle);
		fprintf(fp, "End\n");

		fclose(fp);
	}

	if (lSPCTotalDieCount > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

LONG CBondPr::ExportPostBondData_Arm2(ULONG ulCurrentBin)
{
	SPC_CPart::SPC_ERetrievalResult		eResult;
	SPC_CProperty::SPC_EPropertyType	eType1;
	SPC_CProperty::SPC_EPropertyType	eType3;
	SPC_CProperty::SPC_EPropertyType	eType4;
	SPC_CProperty::SPC_EPropertyType	eType5;
	SPC_CProperty::SPC_EPropertyType	eType6;
	SPC_CProperty::SPC_EPropertyType	eType7;
	SPC_CProperty::SPC_EPropertyType	eType8;
	SPC_CProperty::SPC_EPropertyType	eType9;
	SPC_CProperty::SPC_EPropertyType	eType10;

	unsigned long ulTempDataID;
	unsigned long ulParentID;
	unsigned long ulOwnID;

	void	*pBinIndex;
	void	*pDieRow;
	void	*pDieCol;
	void	*pDieX;
	void	*pDieY;
	void	*pDieAngle;
	void	*pDieAvgAngle;
	void	*pDieIsGood;
	void	*pDieType;
	ULONG	ulIndex;	
	BOOL	bEndOfRecord = FALSE;	

	ULONG	ulSPCRow = 0;
	ULONG	ulSPCCol = 0;
	DOUBLE	dSPCDieX = 0;
	DOUBLE	dSPCDieY = 0;
	DOUBLE	dSPCDieAngle = 0;
	BOOL	bSPCDIeIsGood = FALSE;
	UCHAR	ucSPCDieType = 0;

	LONG	lSPCTotalDieCount = 0;
	LONG	lSPCGoodDieCount = 0;
	DOUBLE	dSPCAverageAngle = 0.0; 
	CString	szFilePath;

	CTime	TimeStamp; 

	//v3.93		//No need to access if no postbond data being logged for this bin grade
	if (!m_arbIsPostBond[ulCurrentBin])
		return 0;

	szFilePath.Format(m_szPostbondDataSource_Arm2 + "\\Temp\\" + "%d", ulCurrentBin+1);

	FILE	*fp = NULL;
	errno_t nErr = fopen_s(&fp, (LPCTSTR)szFilePath, "w+");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "%ld\n", ulCurrentBin+1);

		//Ready get data from SPC
		m_pBin_Arm2[ulCurrentBin]->Flush();
		m_pBin_Arm2[ulCurrentBin]->BeginRetrieveFromStart(0,1);

		pBinIndex		= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Bin Index Arm2", eType1);
		pDieRow			= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Bin Row Arm2", eType9);
		pDieCol			= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Bin Col Arm2", eType10);
		pDieX			= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("X Placement Arm2", eType3);
		pDieY			= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Y Placement Arm2", eType4);
		pDieAngle		= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Die Rotation Arm2", eType5);
		pDieAvgAngle	= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Average Die Rotation Arm2", eType6);
		pDieIsGood		= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Good Die Arm2", eType7);
		pDieType		= m_pBin_Arm2[ulCurrentBin]->GetVariableInFileProperty("Die Type Arm2", eType8);

		while(!bEndOfRecord)
		{
			eResult = m_pBin_Arm2[ulCurrentBin]->RetrieveAdvance(ulTempDataID, TimeStamp, ulParentID, ulOwnID);
			bEndOfRecord = eResult == SPC_CPart::SPC_RETRIEVE_END;

			if (!bEndOfRecord)
			{
				//Get raw data....	
				ulIndex			= SPC_CProperty::ConvertToULong(pBinIndex, eType1);
				ulSPCRow		= SPC_CProperty::ConvertToULong(pDieRow, eType9);
				ulSPCCol		= SPC_CProperty::ConvertToULong(pDieCol, eType10);
				dSPCDieX		= SPC_CProperty::ConvertToDouble(pDieX, eType3);
				dSPCDieY		= SPC_CProperty::ConvertToDouble(pDieY, eType4);
				dSPCDieAngle	= SPC_CProperty::ConvertToDouble(pDieAngle, eType5);
				bSPCDIeIsGood	= (BOOL)SPC_CProperty::ConvertToUChar(pDieIsGood, eType7);
				ucSPCDieType	= SPC_CProperty::ConvertToUChar(pDieType, eType8);
				
				//Update SPC Summay variable
				if (lSPCTotalDieCount == 0)
				{
					//Get average angle from last unit
					dSPCAverageAngle = SPC_CProperty::ConvertToDouble(pDieAvgAngle, eType6);
				}

				if (bSPCDIeIsGood == TRUE)
				{
					lSPCGoodDieCount++;
				}

				lSPCTotalDieCount++;

				//ouput each data
				fprintf(fp, "%lu,%f,%f,%f,%d,%d\n", ulIndex+1, dSPCDieX, dSPCDieY, dSPCDieAngle, bSPCDIeIsGood, ucSPCDieType);
			}

			//Prevent infinite loop
			if ( lSPCTotalDieCount > 99999 )
			{
				break;
			}
		}
		m_pBin_Arm2[ulCurrentBin]->EndRetrieve();

		fprintf(fp, "End of SPC Data Details\n");

		//Output summay
		fprintf(fp, "%ld\n", lSPCTotalDieCount);
		fprintf(fp, "%ld\n", lSPCGoodDieCount);
		fprintf(fp, "%f\n", dSPCAverageAngle);
		fprintf(fp, "End\n");

		fclose(fp);
	}

	if (lSPCTotalDieCount > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

VOID CBondPr::ResetPostBondSPCFlags()		//v3.93
{
	for (INT i=0; i<BPR_MAX_BINBLOCK; i++)
	{
		m_arbIsPostBond[i] = FALSE;
	}
}

VOID CBondPr::DoOfflinePostBond(VOID)
{
	if ((m_bStartOffline == FALSE) || (m_fHardware == FALSE))
	{
		return;
	}


	//PR_UWORD		usDieType;
	//PR_COORD		stDieOffset; 
	//PR_REAL			fDieRotate; 
	//PR_REAL			fDieScore;
	//LONG			lCount = 1;
	//ULONG			ulCurrentBin = 1;

	//ulCurrentBin = m_lCurrentBinBlock - 1;

	//for (lCount = 1; lCount <= m_lOfflineCount; lCount++)
	//{
	//	IndexBinTable((ULONG)(lCount));
	//	Sleep(100);
	//	ManualSearchDie(m_ssPRSrchID, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore);
	//	LogPostBondData(BPR_POSTBOND_OFFLINE, ulCurrentBin, usDieType, fDieRotate, stDieOffset);
	//}

	m_bStartOffline = FALSE;
}


//================================================================
// DisplayLFSearchArea()
//   Created-By  : Andrew Ng
//   Date        : 3/7/2007 9:36:36 AM
//   Description : 
//   Remarks     : 
//================================================================
VOID CBondPr::DisplayLFSearchArea(CONST INT nDirection, PR_COLOR ssColor)
{
	PR_WIN stArea = m_stSearchArea;

	CalculateLFSearchArea(nDirection, 1, &stArea);
	DrawRectangleBox(stArea.coCorner1, stArea.coCorner2, ssColor);
}


VOID CBondPr::UpdateFovSize()
{
    DOUBLE dFOVSize = 0;
    int nConvID;
	IPC_CServiceMessage stMsg;

	typedef struct 
	{
	    LONG lPitchX;
	    LONG lPitchY;
	} BT_PITCH;
	BT_PITCH stInfo;

	//BOOL bResult = TRUE;
	stInfo.lPitchX = 0;
	stInfo.lPitchY = 0;
	stMsg.InitMessage(sizeof(BT_PITCH), &stInfo);
    nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GetBinDiePitch", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BT_PITCH), &stInfo);
			break;
		}
		else
		{
			Sleep(1);
		}
	}


	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if ( m_bGenDieLearnt[BPR_NORMAL_DIE] == TRUE && m_bDieCalibrated==TRUE && lInitPR==1 )
    {
		//Calculate this blk FOV
		PR_COORD stTmpDiePixel;
		int	siMarginPixel = 20;
		int	siDieInField = 0;
		int	siXPos;
		int	siYPos;

		DOUBLE	dRowDieCount = 0;
		DOUBLE	dColDieCount = 0;
		DOUBLE	dDieInField = 0;

		//Convert Pixel into Motor step
		stTmpDiePixel.x = PR_MAX_WIN_LRC_X - PR_MAX_WIN_ULC_X - (m_stGenDieSize[BPR_NORMAL_DIE].x / 2) - siMarginPixel;
		stTmpDiePixel.y = PR_MAX_WIN_LRC_Y - PR_MAX_WIN_ULC_Y - (m_stGenDieSize[BPR_NORMAL_DIE].y / 2) - siMarginPixel;
		ConvertPixelToMotorStep(stTmpDiePixel, &siXPos, &siYPos);

		m_lDiePitchX	= stInfo.lPitchX;
		m_lDiePitchY	= stInfo.lPitchY;

		dRowDieCount	= ((DOUBLE)abs(siXPos) / (DOUBLE)abs((int)m_lDiePitchX)) + 1;
		dColDieCount	= ((DOUBLE)abs(siYPos) / (DOUBLE)abs((int)m_lDiePitchY)) + 1;

		//4.53D01 Save Row&Col Fov
		m_dRowFovSize	= dRowDieCount;
		m_dColFovSize	= dColDieCount;
		CString szMsg;
szMsg.Format("Search --- FovSize(%f,%f)", m_dRowFovSize, m_dColFovSize); 
//AfxMessageBox(szMsg,MB_SYSTEMMODAL);
CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");
		dDieInField		= min(dRowDieCount, dColDieCount);
		m_dFovSize		= dDieInField;
	}
	else
		m_dFovSize = 0.0;
}


VOID CBondPr::FreeAllPrRecords(VOID)
{
	for (INT i=0; i <BPR_MAX_DIE ; i++)
	{
		m_ssGenPRSrchID[i] = 0;
		m_bGenDieLearnt[i] = FALSE;
		(*m_psmfSRam)["BondPr"]["Die Record"][i] = (LONG)m_ssGenPRSrchID[i];	//v4.49A7	//Osram
	}

	m_bDieCalibrated	= FALSE;	//For BT1
	m_bDieCalibrated2	= FALSE;	//For BT2 in MS109
	m_bDieLearnt		= FALSE;
	m_lCurNormDieID		= 0;
}


VOID CBondPr::CalculateLFSearchArea(LONG lDirection, LONG lNumDie, PR_WIN *stInputArea)
{
	int siDiePixelX = 0;	//Klocwork
	int siDiePixelY = 0;	//Klocwork

	DOUBLE dCalibX = GetCalibX();
	DOUBLE dCalibY = GetCalibY();
	if (fabs(dCalibX) > 0.0000001)
	{
		siDiePixelX = abs((int)((DOUBLE)m_lDiePitchX / dCalibX));
	}

	if (fabs(dCalibY) > 0.0000001)
	{
		siDiePixelY = abs((int)((DOUBLE)m_lDiePitchY / dCalibY));
	}

	siDiePixelX = siDiePixelX * lNumDie;
	siDiePixelY = siDiePixelY * lNumDie;


	switch (lDirection)
	{
		case BPR_LT_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x - siDiePixelX;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x - siDiePixelX;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y;
			break;

		case BPR_RT_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x + siDiePixelX;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x + siDiePixelX;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y;
			break;

		case BPR_UP_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y - siDiePixelY;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y - siDiePixelY;
			break;

		case BPR_DN_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y + siDiePixelY;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y + siDiePixelY;
			break;

		case BPR_UL_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x - siDiePixelX;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y - siDiePixelY;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x - siDiePixelX;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y - siDiePixelY;
			break;

		case BPR_DL_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x - siDiePixelX;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y + siDiePixelY;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x - siDiePixelX;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y + siDiePixelY;
			break;

		case BPR_UR_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x + siDiePixelX;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y - siDiePixelY;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x + siDiePixelX;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y - siDiePixelY;
			break;

		case BPR_DR_DIE:
			stInputArea->coCorner1.x = stInputArea->coCorner1.x + siDiePixelX;
			stInputArea->coCorner1.y = stInputArea->coCorner1.y + siDiePixelY;
			stInputArea->coCorner2.x = stInputArea->coCorner2.x + siDiePixelX;
			stInputArea->coCorner2.y = stInputArea->coCorner2.y + siDiePixelY;
			break;

		default:	
			break;
	}
}

VOID CBondPr::CalculateLFDiePosn(const LONG lDirection, const LONG lNumDie, LONG &lDiePosnOffsetX, LONG &lDiePosnOffsetY)
{
	int siDiePixelX = 0;
	int siDiePixelY = 0;
	DOUBLE dCalibX = GetCalibX();
	DOUBLE dCalibY = GetCalibY();

	if (fabs(dCalibX) > 0.0000001)	// divide by zero
	{
		siDiePixelX = abs((int)((DOUBLE)m_lDiePitchX / dCalibX));
	}
	if (fabs(dCalibY) > 0.0000001)	// divide by zero
	{
		siDiePixelY = abs((int)((DOUBLE)m_lDiePitchY / dCalibY));
	}

	switch (lDirection)
	{
		case BPR_LT_DIE:
			siDiePixelX = -siDiePixelX * lNumDie;
			siDiePixelY = 0;
			break;

		case BPR_RT_DIE:
			siDiePixelX = siDiePixelX * lNumDie;
			siDiePixelY = 0;
			break;

		case BPR_UP_DIE:
			siDiePixelX = 0;
			siDiePixelY = -siDiePixelY * lNumDie;
			break;

		case BPR_DN_DIE:
			siDiePixelX = 0;
			siDiePixelY = siDiePixelY * lNumDie;
			break;

		case BPR_UL_DIE:
			siDiePixelX = -siDiePixelY * lNumDie;
			siDiePixelY = -siDiePixelY * lNumDie;
			break;

		case BPR_DL_DIE:
			siDiePixelX = -siDiePixelY * lNumDie;
			siDiePixelY = siDiePixelY * lNumDie;
			break;

		case BPR_UR_DIE:
			siDiePixelX = siDiePixelY * lNumDie;
			siDiePixelY = -siDiePixelY * lNumDie;
			break;

		case BPR_DR_DIE:
			siDiePixelX = siDiePixelY * lNumDie;
			siDiePixelY = siDiePixelY * lNumDie;
			break;

		default:
			siDiePixelX = 0;
			siDiePixelY = 0;
			break;
	}

	lDiePosnOffsetX = siDiePixelX;
	lDiePosnOffsetY = siDiePixelY;
}


VOID CBondPr::FlushMessage()
{
	if (m_comServer.ScanRequest(10))
		m_comServer.ReadRequest();
}

VOID CBondPr::UpdateStationData()
{
	//AfxMessageBox("CBondPr", MB_SYSTEMMODAL);
	m_oBondPrDataBlk.m_szEnableDieRotationAverageAngleCheck = m_oBondPrDataBlk.ConvertBoolToOnOff(m_bAverageAngleCheck);
	m_oBondPrDataBlk.m_szDieRotationAverageAngle.Format("%.2f", m_dGenAverageAngle[0]);
	m_oBondPrDataBlk.m_szEnableDieRotationMaxAngleCheck = m_oBondPrDataBlk.ConvertBoolToOnOff(m_bBinTableAngleCheck);
	m_oBondPrDataBlk.m_szDieRotationMaxAngle.Format("%.2f", m_dGenMaxAngle[0]);
	m_oBondPrDataBlk.m_szDieRotationMaxAngleMaxDieAllow.Format("%d",  m_lGenMaxAngleAllow[0]);
	m_oBondPrDataBlk.m_szBondDiePassScore.Format("%d",m_lGenSrchDieScore[BPR_NORMAL_DIE]);
	m_oBondPrDataBlk.m_szEnablePostBondInspection = m_oBondPrDataBlk.ConvertBoolToOnOff(m_bUsePostBond);
	m_oBondPrDataBlk.m_szEnableDiePlacementCheck = m_oBondPrDataBlk.ConvertBoolToOnOff(m_bPlacementCheck);
	m_oBondPrDataBlk.m_szPostBondDiePlacementMaxDieAllow.Format("%d", m_lGenMaxShiftAllow[0]);
	m_oBondPrDataBlk.m_szPostBondDiePlacementMaxXShift.Format("%.2f", m_dGenDieShiftX[0]);
	m_oBondPrDataBlk.m_szPostBondDiePlacementMaxYShift.Format("%.2f", m_dGenDieShiftY[0]);
	m_oBondPrDataBlk.m_szEnableEmptyDieCheck = m_oBondPrDataBlk.ConvertBoolToOnOff(m_bEmptyCheck);
	m_oBondPrDataBlk.m_szEmptyDieCheckMaxDieAllow.Format("%d", m_lGenMaxEmptyAllow[0]);

	m_oBondPrDataBlk.m_szPRCenterX.Format("%d", m_lPrCenterX);
	m_oBondPrDataBlk.m_szPRCenterX.Format("%d", m_lPrCenterX);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetBondPrDataBlock(m_oBondPrDataBlk);
}

//v2.60
VOID CBondPr::MoveBhToBond(CONST BOOL bBond)
{
	if (BPR_GetFrameLevel() == TRUE)
	{
		SetAlert(IDS_BL_PLATFORM_NOT_DOWN);
		return;
	}	

	// Move BH to BON for learning collet hole pattern
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;
	BOOL bBhToBond = bBond;

	stMsg.InitMessage(sizeof(BOOL), &bBhToBond);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToBond", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

//dual arm learn collet
VOID CBondPr::MoveBhToBond_Z2(CONST BOOL bBond)
{
	if (BPR_GetFrameLevel() == TRUE)
	{
		SetAlert(IDS_BL_PLATFORM_NOT_DOWN);
		return;
	}	

	// Move BH to BON for learning collet hole pattern
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;
	BOOL bBhToBond = bBond;

	stMsg.InitMessage(sizeof(BOOL), &bBhToBond);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveToBond_Z2", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}


BOOL CBondPr::BPR_GetFrameLevel()
{
	BOOL bIsUp;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "GetFrameLevel" , stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bIsUp);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bIsUp;
}

VOID CBondPr::GetLightingConfig(int nGroupID, PR_SOURCE &emCoaxID, PR_SOURCE &emRingID, PR_SOURCE &emSideID)
{
	if ( CMS896AStn::m_bEnablePRDualColor == FALSE )
	{
		emCoaxID = PR_COAXIAL_LIGHT;
		emRingID = PR_RING_LIGHT;
		emSideID = PR_SIDE_LIGHT;
	}
	else
	{
		switch(nGroupID)
		{
			default:
				emCoaxID = PR_COAXIAL_LIGHT;
				emRingID = PR_RING_LIGHT;
				emSideID = PR_SIDE_LIGHT_1;
				break;
				
			case 1:
				emCoaxID = PR_COAXIAL_LIGHT_1;
				emRingID = PR_RING_LIGHT;
				emSideID = PR_SIDE_LIGHT_2;
				break;
		}
	}
}

BOOL CBondPr::ShowPrImage(CString& szImagePath, LONG& lDieNo, BOOL bDieType, LONG lRefDieNo)
{
	BOOL bFoundImage = FALSE;
	
	CTime curTime = CTime::GetCurrentTime();
	CString szDateTime = curTime.Format("%y%m%d%H%M%S");

	if ( bDieType == BPR_NORMAL_DIE )
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + lRefDieNo;
	}
	else
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lRefDieNo;
	}

	if (lDieNo > BPR_MAX_DIE)
	{
		//RemoveFilesInFolder(BPR_SHOW_IMAGE_PATH);
		//m_szShowImagePath			= "";
		//m_szGenShowImagePath		= "";
		szImagePath = "";
		return FALSE;
	}

	//Klocwork	//v4.02T5
	lDieNo	= min(lDieNo, BPR_MAX_DIE-1);
	//lDieNo= max(lDieNo, 0);

	PR_UWORD ssID = m_ssGenPRSrchID[lDieNo];

	if (ssID == 0)
	{
		//RemoveFilesInFolder(BPR_SHOW_IMAGE_PATH);
		//m_szGenShowImagePath	= "";
		//m_szShowImagePath		= "";
		szImagePath = "";
		return TRUE;
	}

	CString szIndex = "";
	CString szDestPath = "";
	INT nIndex = 0;

	LONG lID = (LONG) ssID;
	CString szID = "";

	szID.Format("%d", lID);
	if (lID < 10)
	{
		szID = "00" + szID;
	}
	else if (lID < 100)
	{
		szID = "0" + szID;
	}

	//Source path
	CString szSourcePath = BPR_PR_RECORD_PATH;
	szSourcePath = szSourcePath + szID;


	if ((_access(szSourcePath + "\\log\\buffer00.bmp", 0 )) != -1)
	{	
		szSourcePath += "\\log\\buffer00.bmp";
		bFoundImage = TRUE;
	}
	else if ((_access(szSourcePath + "\\log\\buffer01.bmp", 0)) != -1)
	{
		szSourcePath += "\\log\\buffer01.bmp";
		bFoundImage = TRUE;
	}
	
	szDestPath = BPR_SHOW_IMAGE_PATH;
	CreateDirectory(BPR_SHOW_IMAGE_PATH, NULL);

	// remove no use file in the folder first
	//RemoveFilesInFolder(BPR_SHOW_IMAGE_PATH);

	if (bFoundImage == FALSE)
	{
		//m_szGenShowImagePath		= "";
		//m_szShowImagePath			= "";
		szImagePath = "";
		return TRUE;
	}

	szDestPath = szDestPath + "\\BPR" + szDateTime + szID + ".bmp";
	
	szImagePath = szDestPath;
	//m_szGenShowImagePath	= szDestPath;
	//m_szShowImagePath		= szDestPath;	// Update HMI display path in 

	// Copy PR record bitmap to Mapsorter sub-folder for display
	CopyFile(szSourcePath, szDestPath, FALSE);

	return TRUE;
}

VOID CBondPr::ResetRtBHCompXYArrays(const BOOL bBHZ1)
{
	CString szMess;
	if (bBHZ1)
	{
		szMess = _T("BPR: Arm#1 PB RT Data is RESET");
	}
	else
	{
		szMess = _T("BPR: Arm#2 PB RT Data is RESET");
	}
	CMSLogFileUtility::Instance()->MS_LogOperation(szMess);	

	for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
	{
		if (bBHZ1)
		{
			m_lBH1_RTX[i] = 0;
			m_lBH1_RTY[i] = 0;
		}
		else
		{
			m_lBH2_RTX[i] = 0;
			m_lBH2_RTY[i] = 0;
		}
	}

	if (m_psmfSRam != NULL)
	{
		if (bBHZ1)
		{
			m_lRT_Count1 = 0;
			(*m_psmfSRam)["BondPr"]["AVG BH1 X"]	= 0;
			(*m_psmfSRam)["BondPr"]["AVG BH1 Y"]	= 0;
		}
		else
		{
			m_lRT_Count2 = 0;
			(*m_psmfSRam)["BondPr"]["AVG BH2 X"]	= 0;
			(*m_psmfSRam)["BondPr"]["AVG BH2 Y"]	= 0;
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SaveStopBondTsToRegistry(bBHZ1);	//Reset timestamp
}


VOID CBondPr::InitRtBHCompXYArrays(const BOOL bBHZ1, BOOL bUpdateSRAM)
{
	//v3.98T5
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nStatus = pApp->CheckRegTsForPostBond(bBHZ1);

	if (bBHZ1)
	{
		if ((nStatus == 2) && (m_lRT_Count1 > 0))
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Amr#1 PB RT Data use mode #2");	
		}
		else if ((nStatus == 1) && (m_lRT_Count1 > 0))
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Amr#1 PB RT Data use mode #1");	

			//All data values by "half"

			DOUBLE dBH1X, dBH1Y;
			for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
			{
				dBH1X = m_lBH1_RTX[i];
				dBH1Y = m_lBH1_RTY[i];

				m_lBH1_RTX[i] = _round(dBH1X / 2.0);
				m_lBH1_RTY[i] = _round(dBH1Y / 2.0);
			}

			DOUBLE dAvgX1 = (*m_psmfSRam)["BondPr"]["AVG BH1 X"];
			DOUBLE dAvgY1 = (*m_psmfSRam)["BondPr"]["AVG BH1 Y"];
			(*m_psmfSRam)["BondPr"]["AVG BH1 X"]	= (LONG) dAvgX1 / 2.0;
			(*m_psmfSRam)["BondPr"]["AVG BH1 Y"]	= (LONG) dAvgY1 / 2.0;
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR: PB RT Data use mode #0");	

			m_lRT_Count1 = 0;
			for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
			{
				m_lBH1_RTX[i] = 0;
				m_lBH1_RTY[i] = 0;
			}

			if (bUpdateSRAM && (m_psmfSRam != NULL))
			{
				(*m_psmfSRam)["BondPr"]["AVG BH1 X"]	= 0;
				(*m_psmfSRam)["BondPr"]["AVG BH1 Y"]	= 0;
			}
		}
	}
	else
	{
		if ((nStatus == 2) && (m_lRT_Count2 > 0))
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Amr#2 PB RT Data use mode #2");	
		}
		else if ((nStatus == 1) && (m_lRT_Count2 > 0))
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Amr#2 PB RT Data use mode #1");	

			//All data values by "half"

			DOUBLE dBH2X, dBH2Y;
			for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
			{
				dBH2X = m_lBH2_RTX[i];
				dBH2Y = m_lBH2_RTY[i];

				m_lBH2_RTX[i] = _round(dBH2X / 2.0);
				m_lBH2_RTY[i] = _round(dBH2Y / 2.0);
			}

			DOUBLE dAvgX2 = (*m_psmfSRam)["BondPr"]["AVG BH2 X"];
			DOUBLE dAvgY2 = (*m_psmfSRam)["BondPr"]["AVG BH2 Y"];
			(*m_psmfSRam)["BondPr"]["AVG BH2 X"]	= (LONG) dAvgX2 / 2.0;
			(*m_psmfSRam)["BondPr"]["AVG BH2 Y"]	= (LONG) dAvgY2 / 2.0;
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Arm#2 PB RT Data use mode #0");	

			m_lRT_Count2 = 0;
			for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
			{
				m_lBH2_RTX[i] = 0;
				m_lBH2_RTY[i] = 0;
			}

			if (bUpdateSRAM && (m_psmfSRam != NULL))
			{
				(*m_psmfSRam)["BondPr"]["AVG BH2 X"]	= 0;
				(*m_psmfSRam)["BondPr"]["AVG BH2 Y"]	= 0;
			}
		}
	}
}

//v4.50A9	//MS90
BOOL CBondPr::TransformBHCompXYArrays(CONST UCHAR ucRotation)
{
	if (!m_bEnableRTCompXY)
		return TRUE;

	CString szMsg;

	//enum {BBX_FRAME_ROTATE_0 = 0, BBX_FRAME_ROTATE_90 = 1, BBX_FRAME_ROTATE_180 = 2, BBX_FRAME_ROTATE_270 = 3};
	switch (ucRotation)
	{
	case 1:				//90  degree transform
	case 3:				//270 degree transform
		szMsg = "BPR - TransformBHCompXYArrays: perform INVALID Transform";
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg + "\n");
		return FALSE;		//** Not yet supported

	case 2:				//180 degree rotation
		szMsg = "BPR - TransformBHCompXYArrays: perform 180 degree Transform";
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg + "\n");
		break;

	case 4:				//0 degree transform from previous
		szMsg = "BPR - TransformBHCompXYArrays: perform 0 degree Transform";
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg + "\n");
		break;

	case 0:				//NO transform
	default:
		szMsg = "BPR - TransformBHCompXYArrays: perform NO Transform";
		//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg + "\n");
		return TRUE;
	}

	INT i=0;
	LONG lRTX=0, lRTY=0;

	if ( (ucRotation == 2) || (ucRotation == 4) )	//transform 180 / 0 degree
	{
		//Disabled for MS90 testing by HarryHo	//v4.50a25
/*
		for (i=0; i<m_lRT_Count1; i++)
		{
			if (i >= BPR_RT_XYCOMP_ARRAY_SIZE)
				break;
			lRTX = m_lBH1_RTX[i];
			lRTY = m_lBH1_RTY[i];
			m_lBH1_RTX[i] = -1 * lRTX;
			m_lBH1_RTY[i] = -1 * lRTY;
		}

		for (i=0; i<m_lRT_Count2; i++)
		{
			if (i >= BPR_RT_XYCOMP_ARRAY_SIZE)
				break;
			lRTX = m_lBH2_RTX[i];
			lRTY = m_lBH2_RTY[i];
			m_lBH2_RTX[i] = -1 * lRTX;
			m_lBH2_RTY[i] = -1 * lRTY;
		}
*/
	}
/*
	else if (ucRotation == 0)		//v4.50A24
	{
		for (i=0; i<m_lRT_Count1; i++)
		{
			if (i >= BPR_RT_XYCOMP_ARRAY_SIZE)
				break;
			m_lBH1_RTX[i] = 0;
			m_lBH1_RTY[i] = 0;
		}

		for (i=0; i<m_lRT_Count2; i++)
		{
			if (i >= BPR_RT_XYCOMP_ARRAY_SIZE)
				break;
			m_lBH2_RTX[i] = 0;
			m_lBH2_RTY[i] = 0;
		}
	}
*/
	return TRUE;
}

BOOL CBondPr::AddRTBHCompXY(CONST INT nBHIndex, CONST LONG lStepX, CONST LONG lStepY)
{
	INT nArrIndex = 0;

	if (nBHIndex == 2)		//BH2
	{
		nArrIndex = m_lRT_Count2 % BPR_RT_XYCOMP_ARRAY_SIZE;
		m_lBH2_RTX[nArrIndex] = lStepX;
		m_lBH2_RTY[nArrIndex] = lStepY;
		m_lRT_Count2++;
	}
	else					//BH1
	{
		nArrIndex = m_lRT_Count1 % BPR_RT_XYCOMP_ARRAY_SIZE;
		m_lBH1_RTX[nArrIndex] = lStepX;
		m_lBH1_RTY[nArrIndex] = lStepY;
		m_lRT_Count1++;
	}

	return TRUE;
}


BOOL CBondPr::UpdateAvgRTCompOffsetXY(CONST INT nBHIndex, LONG &lStepX, LONG &lStepY, DOUBLE &dUnitX, DOUBLE &dUnitY)
{
	LONG lAvgBH1X=0, lAvgBH2X=0;
	LONG lAvgBH1Y=0, lAvgBH2Y=0;
	DOUBLE dTotalBH1X=0, dTotalBH2X=0;
	DOUBLE dTotalBH1Y=0, dTotalBH2Y=0;

	DOUBLE dBH1MinX=0, dBH1MaxX=0;
	DOUBLE dBH2MinX=0, dBH2MaxX=0;
	DOUBLE dBH1MinY=0, dBH1MaxY=0;
	DOUBLE dBH2MinY=0, dBH2MaxY=0;

	if (m_lRT_Count2 >= BPR_RT_XYCOMP_ARRAY_SIZE)
	{
		for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
		{
			if (i == 0)
			{
				dBH2MaxX = m_lBH2_RTX[i];
				dBH2MinX = m_lBH2_RTX[i];
				dBH2MaxY = m_lBH2_RTY[i];
				dBH2MinY = m_lBH2_RTY[i];
			}

			if (m_lBH2_RTX[i] > dBH2MaxX)
				dBH2MaxX = m_lBH2_RTX[i];
			if (m_lBH2_RTX[i] < dBH2MinX)
				dBH2MinX = m_lBH2_RTX[i];
			if (m_lBH2_RTY[i] > dBH2MaxY)
				dBH2MaxY = m_lBH2_RTY[i];
			if (m_lBH2_RTY[i] < dBH2MinY)
				dBH2MinY = m_lBH2_RTY[i];

			dTotalBH2X = dTotalBH2X + m_lBH2_RTX[i];
			dTotalBH2Y = dTotalBH2Y + m_lBH2_RTY[i];
		}

		lAvgBH2X = (LONG) (dTotalBH2X - dBH2MaxX - dBH2MinX) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);
		lAvgBH2Y = (LONG) (dTotalBH2Y - dBH2MaxY - dBH2MinY) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);

	}
	else if (m_lRT_Count2 > 1)		//v4.59A44
	{
		for (INT i=0; i<m_lRT_Count2; i++)
		{
			dTotalBH2X = dTotalBH2X + m_lBH2_RTX[i];
			dTotalBH2Y = dTotalBH2Y + m_lBH2_RTY[i];
		}

		lAvgBH2X = (LONG) dTotalBH2X / m_lRT_Count2;
		lAvgBH2Y = (LONG) dTotalBH2Y / m_lRT_Count2;
	}
	else
	{
		lAvgBH2X = 0;
		lAvgBH2Y = 0;
	}

	if (m_lRT_Count1 >= BPR_RT_XYCOMP_ARRAY_SIZE)
	{
		for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
		{
			if (i == 0)
			{
				dBH1MaxX = m_lBH1_RTX[i];
				dBH1MinX = m_lBH1_RTX[i];
				dBH1MaxY = m_lBH1_RTY[i];
				dBH1MinY = m_lBH1_RTY[i];
			}

			if (m_lBH1_RTX[i] > dBH1MaxX)
				dBH1MaxX = m_lBH1_RTX[i];
			if (m_lBH1_RTX[i] < dBH1MinX)
				dBH1MinX = m_lBH1_RTX[i];
			if (m_lBH1_RTY[i] > dBH1MaxY)
				dBH1MaxY = m_lBH1_RTY[i];
			if (m_lBH1_RTY[i] < dBH1MinY)
				dBH1MinY = m_lBH1_RTY[i];

			dTotalBH1X = dTotalBH1X + m_lBH1_RTX[i];
			dTotalBH1Y = dTotalBH1Y + m_lBH1_RTY[i];
		}

		lAvgBH1X = (LONG) (dTotalBH1X - dBH1MaxX - dBH1MinX) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);
		lAvgBH1Y = (LONG) (dTotalBH1Y - dBH1MaxY - dBH1MinY) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);
	}
	else if (m_lRT_Count1 > 1)		//v4.59A44
	{
		for (INT i=0; i<m_lRT_Count1; i++)
		{
			dTotalBH1X = dTotalBH1X + m_lBH1_RTX[i];
			dTotalBH1Y = dTotalBH1Y + m_lBH1_RTY[i];
		}

		lAvgBH1X = (LONG) dTotalBH1X / m_lRT_Count1;
		lAvgBH1Y = (LONG) dTotalBH1Y / m_lRT_Count1;
	}
	else
	{
		lAvgBH1X = 0;
		lAvgBH1Y = 0;
	}

	//v4.57A11
	CString szLog1, szLog2;
	szLog1.Format("RTY1 Array (%ld):,", lAvgBH1Y);
	szLog2.Format("RTY2 Array (%ld):,", lAvgBH2Y);
	CString szItem1, szItem2;
	for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
	{
		szItem1.Format("%ld,", m_lBH1_RTY[i]);
		szLog1 = szLog1 + szItem1;
		szItem2.Format("%ld,", m_lBH2_RTY[i]);
		szLog2 = szLog2 + szItem2;
	}
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog1);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog2);

	(*m_psmfSRam)["BondPr"]["AVG BH1 X"]	= lAvgBH1X;
	(*m_psmfSRam)["BondPr"]["AVG BH1 Y"]	= lAvgBH1Y;
	(*m_psmfSRam)["BondPr"]["AVG BH2 X"]	= lAvgBH2X;
	(*m_psmfSRam)["BondPr"]["AVG BH2 Y"]	= lAvgBH2Y;

	DOUBLE dX=0, dY=0;
	if (nBHIndex == 2)
	{
		if (m_bDisplayUnitInUm)		//v4.40T7
			ConvertUnitToUm(lAvgBH2X, lAvgBH2Y, &dX, &dY);
		else
			ConvertUnit(lAvgBH2X, lAvgBH2Y, &dX, &dY);
		lStepX = lAvgBH2X;
		lStepY = lAvgBH2Y;
		dUnitX = dX;
		dUnitY = dY;
	}
	else
	{
		if (m_bDisplayUnitInUm)		//v4.40T7
			ConvertUnitToUm(lAvgBH1X, lAvgBH1Y, &dX, &dY);
		else
			ConvertUnit(lAvgBH1X, lAvgBH1Y, &dX, &dY);
		lStepX = lAvgBH1X;
		lStepY = lAvgBH1Y;
		dUnitX = dX;
		dUnitY = dY;
	}

	return TRUE;
}

BOOL CBondPr::PrintRtBHXYArray()		//v4.48A18
{
	CStdioFile oFile;

	if (!m_bEnableRTCompXY)
		return TRUE;
	if (!m_bUsePostBond)
		return TRUE;
	if (!m_bSemitekBLMode)	//!IsMS60())	//v4.50A29
		return TRUE;

	BOOL bStatus = oFile.Open(gszUSER_DIRECTORY + "\\pb_rt_data.log", 
						CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
	if (!bStatus)
	{
		return FALSE;
	}

	CString szLine;
	szLine.Format("BH1 Count,%ld\n", m_lRT_Count1);
	oFile.WriteString(szLine);
	szLine.Format("BH2 Count,%ld\n\n", m_lRT_Count2);
	oFile.WriteString(szLine);

	szLine = "- If COUNT >= 20, Offset = (Offset1 + Offset2 + ..... + Offset20 - Max - Min) / 18\n";
	oFile.WriteString(szLine);
	szLine = "- If 0>COUNT>20,  Offset = (Offset1 + Offset2 + ..... + Offset_m) / m\n";
	oFile.WriteString(szLine);
	szLine = "- If COUNT = 0,   Offset = 0\n\n";
	oFile.WriteString(szLine);


	INT nMaxBH1 = 20;
	if (m_lRT_Count1 < 20)
		nMaxBH1 = m_lRT_Count1;

	szLine = "BH1 Offset (XY) in motor steps:\n";
	oFile.WriteString(szLine);

	for (INT i=0; i<nMaxBH1; i++)
	{
		szLine.Format("%ld,%ld\n", m_lBH1_RTX[i], m_lBH1_RTY[i]);
		oFile.WriteString(szLine);
	}

	oFile.WriteString("\n\n");
	szLine = "BH2 Offset (XY) in motor steps:\n";
	oFile.WriteString(szLine);

	INT nMaxBH2 = 20;
	if (m_lRT_Count2 < 20)
		nMaxBH2 = m_lRT_Count2;

	for (INT i=0; i<nMaxBH2; i++)
	{
		szLine.Format("%ld,%ld\n", m_lBH2_RTX[i], m_lBH2_RTY[i]);
		oFile.WriteString(szLine);
	}

	oFile.WriteString("\n\n");

	LONG lStepX=0, lStepY=0;
	DOUBLE dUnitX=0, dUnitY=0;
	UpdateAvgRTCompOffsetXY(1, lStepX, lStepY, dUnitX, dUnitY);
	szLine.Format("BH1 AVG Offset (XY in motor step),%ld,%ld\n", lStepX, lStepY);
	oFile.WriteString(szLine);
	UpdateAvgRTCompOffsetXY(2, lStepX, lStepY, dUnitX, dUnitY);
	szLine.Format("BH2 AVG Offset (XY in motor step),%ld,%ld\n", lStepX, lStepY);
	oFile.WriteString(szLine);

	oFile.Close();
	return TRUE;
}

LONG CBondPr::LogItems(LONG lEventNo)
{
	if (m_bEnableItemLog == FALSE)
	{
		return 1;
	}

	CString szMsg;
	
	GetLogItemsString(lEventNo, szMsg);
	SetLogItems(szMsg);

	return 1;
}

LONG CBondPr::GetLogItemsString(LONG lEventNo, CString& szMsg)
{
	CString szBinBlkId;
	CString szTemp;
	CString szDieType;
	CString szDieNo;

	if (m_lCurrentBinBlock > 0)
	{
		szBinBlkId.Format("(Bin Blk %d)", m_lCurrentBinBlock);
	}
	else 
	{
		szBinBlkId = "(All Bin Blocks)";
	}

	if (m_szLrnDieType == BPR_HMI_NORM_DIE)
	{
		szDieType = "Normal Die";
	}
	else
	{
		szDieType = "Reference Die";
	}

	szDieNo.Format("%d", m_lCurRefDieNo);
	


	switch(lEventNo)
	{
	case BPR_PLACEMENT_CHECK:
		
		if (m_bPlacementCheck)
		{
			szMsg = "PostBond Die Placement Check " + szBinBlkId + ",On" ;
		}
		else 
		{
			szMsg = "PostBond Die Placement Check " + szBinBlkId + ",Off";
		}
		break;
	
	case BPR_PLACEMENT_MAX_SHIFT_X:
		szTemp.Format("%.2f", m_dDieShiftX);
		szMsg = "PostBond Die Placement Check Max Shift X (mil) " + szBinBlkId + "," + szTemp;
		break;

	case BPR_PLACEMENT_MAX_SHIFT_Y:
		szTemp.Format("%.2f", m_dDieShiftY);
		szMsg = "PostBond Die Placement Check Max Shift Y (mil) " + szBinBlkId + "," + szTemp;
		break;

	case BPR_PLACEMENT_MAX_ALLOWED:
		szTemp.Format("%d", m_lMaxShiftAllow);
		szMsg = "PostBond Die Placement Check Max Die Allowed " + szBinBlkId + "," + szTemp;
		break;

	case BPR_DIE_PASS_SCORE:
		szTemp.Format("%d", m_lSrchDieScore);
		szMsg = szDieType + " Pass Score (Die No: " + szDieNo + "),"  + szTemp;
		break;

	case BPR_DIE_ROTATION_AVG_ANGLE:
		
		if (m_bAverageAngleCheck)
		{
			szMsg = "PostBond Die Rotation Avg Angle Check " + szBinBlkId + ",On" ;
		}
		else
		{
			szMsg = "PostBond Die Rotation Avg Angle Check " + szBinBlkId + ",Off" ;
		}	
		
		break;

	case BPR_DIE_ROTATION_AVG_ANGLE_ALLOW:
		szTemp.Format("%.2f", m_dAverageAngle);
		szMsg = "PostBond Die Rotation Avg Angle Allow " + szBinBlkId + "," + szTemp;
		break;

	case BPR_DIE_ROTATION_CHECK_MAX_ANGLE:
		if (m_bBinTableAngleCheck)
		{
			szMsg = "PostBond Die Rotation Max Angle Check " + szBinBlkId + ",On" ;
		}
		else
		{
			szMsg = "PostBond Die Rotation Max Angle Check " + szBinBlkId + ",Off" ;
		}
		break;

	case BPR_DIE_ROTATION_MAX_ANGLE_DIE_ALLOW:
		szTemp.Format("%d", m_lMaxAngleAllow);
		szMsg = "PostBond Die Rotation Max Angle Max No Of Die Allow " + szBinBlkId + "," + szTemp;
		break;

	case BPR_DIE_EMPTY_DIE_CHECK:
		if (m_bEmptyCheck)
		{
			szMsg = "PostBond Die Empty Check " + szBinBlkId + ",On" ;
		}
		else
		{
			szMsg = "PostBond Die Empty Check " + szBinBlkId + ",Off" ;
		}
		break;

	case BPR_DIE_EMPTY_DIE_CHECK_MAX_ALLOWED:
		szTemp.Format("%d", m_lMaxEmptyAllow);
		szMsg = "PostBond Die Empty Check " + szBinBlkId + "," + szTemp;
		break;

	case BPR_DIE_ROTATION_MAX_ANGLE:
		szTemp.Format("%.2f", m_dMaxAngle);
		szMsg = "PostBond Die Rotation Max Angle " + szBinBlkId + "," + szTemp;
		break;

	case BPR_SEARCH_AREA_X:
		szTemp.Format("%.1f", m_dSrchDieAreaX);
		szMsg = szDieType + "Bond Pr Search Area X (Die No: " + szDieNo + "),"  + szTemp;
		break;

	case BPR_SEARCH_AREA_Y:
		szTemp.Format("%.1f", m_dSrchDieAreaY);
		szMsg = szDieType + "Bond Pr Search Area Y (Die No: " + szDieNo + "),"  + szTemp;
		break;
	}
	return 1;
}

BOOL CBondPr::TurnOnOffPrLighting(BOOL bOn)
{
	return TRUE;
}

VOID CBondPr::MoveBhToPick(CONST BOOL bPick)
{
	// Move BH to BON for learning collet hole pattern
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;
	BOOL bBhToPick = bPick;

	stMsg.InitMessage(sizeof(BOOL), &bBhToPick);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveBHToPick", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

//dual arm learn collet
VOID CBondPr::MoveBhToPick_Z2(CONST BOOL bPick)
{
	// Move BH to BON for learning collet hole pattern
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bResult = TRUE;
	BOOL bBhToPick = bPick;

	stMsg.InitMessage(sizeof(BOOL), &bBhToPick);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_MoveBHToPick_Z2", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
}

UCHAR CBondPr::GetDieShape()
{
	return m_ucGenDieShape;
}

UCHAR CBondPr::CheckDieShape(UCHAR &ucDieShape)
{
	if (CMS896AStn::m_bEnablePolyonDie == FALSE)
	{
		ucDieShape = BPR_RECTANGLE_DIE;
	}

	if( ucDieShape==BPR_RHOMBUS_DIE && m_bEnableRhombusDie==FALSE )
	{
		ucDieShape = BPR_RECTANGLE_DIE;
	}

	return ucDieShape;
}

BOOL CBondPr::OpEnableElectech3EDLPostBondChecking(ULONG ulBin)		//v4.50A31
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Electech3E(DL)")
	{
		CString szLog;
		szLog.Format("BPR: postbond XY-shift check fails (3EDL); BIN = %ld", ulBin);
		SetErrorMessage(szLog);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		if (ulBin == 150)
		{
			m_bEnableElectech3EDLCheck150Bin = TRUE;
			szLog.Format("BPR: Bin #%ld - m_bEnableElectech3EDLCheck150Bin = TRUE", ulBin);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
	}
	return TRUE;
}

BOOL CBondPr::CheckElectech3EDLPostBondChecking()
{
	BOOL bEnable = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetCustomerName() == "Electech3E(DL)") && m_bEnableElectech3EDLCheck150Bin)
	{
		bEnable = pApp->CheckSananPasswordFile_old_3EDL();
/*
		pApp->UpdateHmiVariableData("szLoginID",		"");
		pApp->UpdateHmiVariableData("szLoginPassword",	"");

		CString szPassword = "engineer";	//pApp->GetEngineerPassword();

		CString szTitle			= _T("User Logon");
		CString szInputUserID	= _T("");

		szTitle = _T("Please Enter YOUR Password");
		bEnable = pApp->HmiStrInputKeyboard(szTitle, "szLoginID", TRUE, szInputUserID);

		CString szLog;
		szLog.Format("BPR (PreStart): ENGR pwd checking - %s (%s)", szInputUserID, szPassword);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
*/
		if (bEnable)
		{
			//Checking OK
			m_bEnableElectech3EDLCheck150Bin = FALSE;
		}
		else
		{
			bEnable = FALSE;	//Checking FAIL !!!
		}
	}

	return bEnable;
} 

LONG CBondPr::GetPrCenterX() //4.51D20
{
	return m_lPrCenterX;
}

LONG CBondPr::GetPrCenterY()//4.51D20
{
	return m_lPrCenterY;
}

VOID CBondPr::ResetPreBondDieNPadXYArrays()
{
	CMSLogFileUtility::Instance()->MS_LogOperation("BPR: PB RT PREBOND Data is RESET");	

	m_lRT_PreBondDieNPadCount1 = 0;
	m_lRT_PreBondDieNPadCount2 = 0;
	for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
	{
		m_lBH1_PreBondDieNPadX[i] = 0;
		m_lBH1_PreBondDieNPadY[i] = 0;
		m_lBH2_PreBondDieNPadX[i] = 0;
		m_lBH2_PreBondDieNPadY[i] = 0;
	}

	//if (m_psmfSRam != NULL)
	//{
	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X1"]	= 0;
	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y1"]	= 0;
	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X2"]	= 0;
	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y2"]	= 0;
	//}
	/*CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SaveStopBondTsToRegistry();	//Reset timestamp*/
}

BOOL CBondPr::AddRTPreBondDieNPadXY(CONST INT nBHIndex, CONST LONG lStepX, CONST LONG lStepY)
{
	INT nArrIndex = 0;

	if (nBHIndex == 2)		//BH2
	{
		nArrIndex = m_lRT_PreBondDieNPadCount2 % BPR_RT_XYCOMP_ARRAY_SIZE;
		m_lBH2_PreBondDieNPadX[nArrIndex] = lStepX;
		m_lBH2_PreBondDieNPadY[nArrIndex] = lStepY;
		m_lRT_PreBondDieNPadCount2++;
	}
	else					//BH1
	{
		nArrIndex = m_lRT_PreBondDieNPadCount1 % BPR_RT_XYCOMP_ARRAY_SIZE;
		m_lBH1_PreBondDieNPadX[nArrIndex] = lStepX;
		m_lBH1_PreBondDieNPadY[nArrIndex] = lStepY;
		m_lRT_PreBondDieNPadCount1++;
	}

	return TRUE;
}

BOOL CBondPr::UpdateAvgRTPreBondDieNPadXY(CONST INT nBHIndex)
{
	LONG lAvgBH1X=0, lAvgBH2X=0;
	LONG lAvgBH1Y=0, lAvgBH2Y=0;
	DOUBLE dTotalBH1X=0, dTotalBH2X=0;
	DOUBLE dTotalBH1Y=0, dTotalBH2Y=0;

	DOUBLE dBH1MinX=0, dBH1MaxX=0;
	DOUBLE dBH2MinX=0, dBH2MaxX=0;
	DOUBLE dBH1MinY=0, dBH1MaxY=0;
	DOUBLE dBH2MinY=0, dBH2MaxY=0;

	if (m_lRT_PreBondDieNPadCount2 >= BPR_RT_XYCOMP_ARRAY_SIZE)
	{
		for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
		{
			if (i == 0)
			{
				dBH2MaxX = m_lBH2_PreBondDieNPadX[i];
				dBH2MinX = m_lBH2_PreBondDieNPadX[i];
				dBH2MaxY = m_lBH2_PreBondDieNPadY[i];
				dBH2MinY = m_lBH2_PreBondDieNPadY[i];
			}

			if (m_lBH2_PreBondDieNPadX[i] > dBH2MaxX)
				dBH2MaxX = m_lBH2_PreBondDieNPadX[i];
			if (m_lBH2_PreBondDieNPadX[i] < dBH2MinX)
				dBH2MinX = m_lBH2_PreBondDieNPadX[i];
			if (m_lBH2_PreBondDieNPadY[i] > dBH2MaxY)
				dBH2MaxY = m_lBH2_PreBondDieNPadY[i];
			if (m_lBH2_PreBondDieNPadY[i] < dBH2MinY)
				dBH2MinY = m_lBH2_PreBondDieNPadY[i];

			dTotalBH2X = dTotalBH2X + m_lBH2_PreBondDieNPadX[i];
			dTotalBH2Y = dTotalBH2Y + m_lBH2_PreBondDieNPadY[i];
		}

		lAvgBH2X = (LONG) (dTotalBH2X - dBH2MaxX - dBH2MinX) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);
		lAvgBH2Y = (LONG) (dTotalBH2Y - dBH2MaxY - dBH2MinY) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);

	}
	else if ( m_lRT_PreBondDieNPadCount2 > (BPR_RT_XYCOMP_ARRAY_SIZE / 2) )
	{
		for (INT i=0; i<m_lRT_PreBondDieNPadCount2; i++)
		{
			dTotalBH2X = dTotalBH2X + m_lBH2_PreBondDieNPadX[i];
			dTotalBH2Y = dTotalBH2Y + m_lBH2_PreBondDieNPadY[i];
		}

		lAvgBH2X = (LONG) dTotalBH2X / m_lRT_PreBondDieNPadCount2;
		lAvgBH2Y = (LONG) dTotalBH2Y / m_lRT_PreBondDieNPadCount2;
	}
	else
	{
		lAvgBH2X = 0;
		lAvgBH2Y = 0;
	}

	if (m_lRT_PreBondDieNPadCount1 >= BPR_RT_XYCOMP_ARRAY_SIZE)
	{
		for (INT i=0; i<BPR_RT_XYCOMP_ARRAY_SIZE; i++)
		{
			if (i == 0)
			{
				dBH1MaxX = m_lBH1_PreBondDieNPadX[i];
				dBH1MinX = m_lBH1_PreBondDieNPadX[i];
				dBH1MaxY = m_lBH1_PreBondDieNPadY[i];
				dBH1MinY = m_lBH1_PreBondDieNPadY[i];
			}

			if (m_lBH1_PreBondDieNPadX[i] > dBH1MaxX)
				dBH1MaxX = m_lBH1_PreBondDieNPadX[i];
			if (m_lBH1_PreBondDieNPadX[i] < dBH1MinX)
				dBH1MinX = m_lBH1_PreBondDieNPadX[i];
			if (m_lBH1_PreBondDieNPadY[i] > dBH1MaxY)
				dBH1MaxY = m_lBH1_PreBondDieNPadY[i];
			if (m_lBH1_PreBondDieNPadY[i] < dBH1MinY)
				dBH1MinY = m_lBH1_PreBondDieNPadY[i];

			dTotalBH1X = dTotalBH1X + m_lBH1_PreBondDieNPadX[i];
			dTotalBH1Y = dTotalBH1Y + m_lBH1_PreBondDieNPadY[i];
		}

		lAvgBH1X = (LONG) (dTotalBH1X - dBH1MaxX - dBH1MinX) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);
		lAvgBH1Y = (LONG) (dTotalBH1Y - dBH1MaxY - dBH1MinY) / (BPR_RT_XYCOMP_ARRAY_SIZE - 2);
	}
	else if ( m_lRT_PreBondDieNPadCount1 > (BPR_RT_XYCOMP_ARRAY_SIZE / 2) )
	{
		for (INT i=0; i<m_lRT_PreBondDieNPadCount1; i++)
		{
			dTotalBH1X = dTotalBH1X + m_lBH1_PreBondDieNPadX[i];
			dTotalBH1Y = dTotalBH1Y + m_lBH1_PreBondDieNPadY[i];
		}

		lAvgBH1X = (LONG) dTotalBH1X / m_lRT_PreBondDieNPadCount1;
		lAvgBH1Y = (LONG) dTotalBH1Y / m_lRT_PreBondDieNPadCount1;
	}
	else
	{
		lAvgBH1X = 0;
		lAvgBH1Y = 0;
	}

	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X1"]	= lAvgBH1X;
	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y1"]	= lAvgBH1Y;
	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET X2"]	= lAvgBH2X;
	(*m_psmfSRam)["BondPr"]["PREBOND RT DIENPAD OFFSET Y2"]	= lAvgBH2Y;
/*
	(*m_psmfSRam)["BondPr"]["AVG BH1 X"]	= lAvgBH1X;
	(*m_psmfSRam)["BondPr"]["AVG BH1 Y"]	= lAvgBH1Y;
	(*m_psmfSRam)["BondPr"]["AVG BH2 X"]	= lAvgBH2X;
	(*m_psmfSRam)["BondPr"]["AVG BH2 Y"]	= lAvgBH2Y;
*/
	return TRUE;
}

VOID CBondPr::LogPBDataWithRowCol(LONG lRow, LONG lCol, USHORT usDieType, DOUBLE dX, DOUBLE dY, ULONG ulCurrentBin, DOUBLE dRotate)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_FINISAR)
		return;

	CStdioFile Log;
	CString szPBPath;
	szPBPath.Format("c://MapSorter//UserData//PB%d.csv", ulCurrentBin + 1);
	
	BOOL bExist = FALSE;
	if (_access(szPBPath,0) == -1)
		bExist = FALSE;
	else
		bExist = TRUE;

	if (!Log.Open(szPBPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))
	{
		return;
	}
	Log.SeekToEnd();
	CString szLog;

	BOOL bIs180 = FALSE;
	if (IsMS90() && CMS896AStn::m_b2Parts2ndPartStart)
	{
		bIs180 = TRUE;
	}

	if(bExist == FALSE)
	{
		Log.WriteString("Col,Row,DieType,180,X,Y,Rotation");
		Log.WriteString("\n");
	}

	szLog.Format("%d,%d,0x%x,%d,%f,%f,%f", lCol, lRow, usDieType, bIs180, dX, dY, dRotate);		//v4.59A35

	if (m_qSubOperation == INSPECT_LAST_DIE_Q)
	{
	szLog.Format("%d,%d,0x%x,%d,%f,%f,%f, LASTSTATE", lCol, lRow, usDieType, bIs180, dX, dY, dRotate);	//v4.59A35
	}

	//DisplaySequence(szLog);
	Log.WriteString(szLog);
	Log.WriteString("\n");
	Log.Close();
	return;
}

DOUBLE CBondPr::GetSrchDieAreaX()
{
	DOUBLE dSrchAreaX = m_lSrchDieAreaX;

	return dSrchAreaX;
}


DOUBLE CBondPr::GetSrchDieAreaY()
{
	DOUBLE dSrchAreaY = m_lSrchDieAreaY;

	return dSrchAreaY;
}


VOID CBondPr::SetUseBinMapBondArea()
{
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable != NULL)
	{
		pBinTable->SetUseBinMapBondArea();
	}
}


BOOL CBondPr::IsNGBlockNow()
{
	LONG	 ulBlkInUse = GetBTCurrentBlock();
	CNGGrade *pNGGrade = CNGGrade::Instance();

	return pNGGrade->IsNGBlock(ulBlkInUse);
}


BOOL CBondPr::IsNGBlock(UCHAR ucGrade)
{
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable != NULL)
	{
		LONG ulBlkInUse = pBinTable->GetBlock(ucGrade);
		CNGGrade *pNGGrade = CNGGrade::Instance();
		return pNGGrade->IsNGBlock(ulBlkInUse);
	}

	return FALSE;
}


LONG CBondPr::GetBTCurrentBlock()
{
	LONG lBinBlk = 0;
	CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
	if (pBinLoader != NULL)
	{
		lBinBlk = pBinLoader->GetBTCurrentBlock();
	}

	return lBinBlk;
}

BOOL CBondPr::UpdateBPRInfoList() //Matthew 20190128
{
	CStdioFile fTemp;
	CString szText, szRight, szLeft;
	LONG lLeftCount = 0;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\BPR_Info.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	for(int i = 0; i < 13; i++)
	{
		//PR Record Type
		if (i < 3)
		{
			szText = "Normal,";
		}
		else
		{
			szText = "Reference,";
		}
		fTemp.WriteString(szText);

		//PRID
		if (i < 3)
		{
			szText.Format("%d,", i + 1);
		}
		else
		{
			szText.Format("%d,", i - 2);
		}
		fTemp.WriteString(szText);

		//PR Record Description
		if (i == 0)	
		{
			szText = "NORMAL_PR_ARM1_INDEX";
		}
		else if (i == 1)
		{
			szText = "NORMAL_PR_ARM2_INDEX";
		}
		else if (i == 2)
		{
			szText = "realignment FF mode";
		}
		else if (i == 3) //Reference 1
		{
			szText = "Prebond";
		}
		else if (i == 4 || i == 5 || i == 9 || i == 10 || i == 12)
		{
			szText = "no use";
		}
		else if (i == 7)
		{
			szText = "Ejector Pin";
		}
		else if (i == 6)
		{
			szText = "BT uplook collet hole";
		}
		else if (i == 8)
		{
			szText = "Collet Hole (Expoxy)";
		}
		else if (i == 11)
		{
			szText = "NG grade PR";
		}
		szText = szText + ",";
		fTemp.WriteString(szText);

		//Algorithm Type
		switch (m_lGenLrnAlignAlgo[i])
		{
		case 0:
			szText = "PGE";
			break;
		case 1:
			szText = "PM";
			break;
		case 2:
			szText = "EM";
			break;
		case 3:
			szText = "P+E";
			break;
		case 4:
			szText = "ESM";
			break;
		case 5:
			szText = "CGLF";
			break;
		}
		szText = szText + ",";
		fTemp.WriteString(szText);

		//Learn Status
		if(m_bGenDieLearnt[i] == TRUE)
		{
			szText = "OK,";
		}
		else
		{
			szText = "No,";
		}
		fTemp.WriteString(szText);
		
		//Zoom Mode
		szText = m_szGenDieZoomMode[i];
		szText = szText + ",";
		fTemp.WriteString(szText);

		//Calibration Status
		if (m_bGenDieCalibration[i])
		{
			szText = "Yes,";
		}
		else
		{
			szText = "No,";
		}
		fTemp.WriteString(szText);
		
		//Learn PR Record ID
		LONG lPrID = m_ssGenPRSrchID[i];
		if (lPrID != 0)
		{
			szText.Format("%d,", lPrID);
		}
		else
		{
			szText = "N/A,";
		}
		fTemp.WriteString(szText);
	}

	fTemp.Close();
	return TRUE;
}

BOOL CBondPr::SetbGenDieCalibration(BOOL bCalibration)
{
	if (bCalibration == FALSE)
	{
		if (m_bSelectDieType == BPR_REFERENCE_DIE)
		{
			m_bGenDieCalibration[m_lCurRefDieNo + BPR_GEN_RDIE_OFFSET] = FALSE;
		}
		else
		{
			m_bGenDieCalibration[m_lCurRefDieNo + BPR_GEN_NDIE_OFFSET] = FALSE;
		}
	}
	else
	{
		if (m_bSelectDieType == BPR_REFERENCE_DIE)
		{
			m_bGenDieCalibration[m_lCurRefDieNo + BPR_GEN_RDIE_OFFSET] = TRUE;
		}
		else
		{
			m_bGenDieCalibration[m_lCurRefDieNo + BPR_GEN_NDIE_OFFSET] = TRUE;
		}
	}

	return TRUE;
}

BOOL CBondPr::SetszGenDieZoomMode(BOOL bEmpty)
{
	if (bEmpty == FALSE)
	{
		if (m_bSelectDieType == BPR_REFERENCE_DIE)
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + BPR_GEN_RDIE_OFFSET] = m_szBprZoomFactor;
		}
		else
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + BPR_GEN_NDIE_OFFSET] = m_szBprZoomFactor;
		}
	}
	else
	{
		if (m_bSelectDieType == BPR_REFERENCE_DIE)
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + BPR_GEN_RDIE_OFFSET] = "";
		}
		else
		{
			m_szGenDieZoomMode[m_lCurRefDieNo + BPR_GEN_NDIE_OFFSET] = "";
		}
	}

	return TRUE;
}


BOOL CBondPr::LoadPackageMsdBPRData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadPackageDataConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetPackageDataConfig();

	//	Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	// PB alarm
	m_bEmptyCheck					= (BOOL)((LONG)(*psmf)[BPR_DATA][PB_ALARM][BPR_ENABLE_PR_EMPTY_ALARM]);
	m_bPlacementCheck				= (BOOL)((LONG)(*psmf)[BPR_DATA][PB_ALARM][BPR_ENABLE_PLACEMENT_CHECK]);
	m_bBinTableAngleCheck			= (BOOL)((LONG)(*psmf)[BPR_DATA][PB_ALARM][BPR_ENABLE_BT_ANGLE_CHECK]);

	for (int i = 0; i < BPR_MAX_BINBLOCK; i++)
	{
		m_lGenMaxEmptyAllow[i]		= (*psmf)[BPR_DATA][PB_ALARM][BPR_EMPTY_CONT_LIMIT][i+1];
		m_lGenMaxAccEmptyAllow[i]	= (*psmf)[BPR_DATA][PB_ALARM][BPR_EMPTY_ACC_LIMIT][i+1];
		m_dGenDieShiftX[i]			= (*psmf)[BPR_DATA][PB_ALARM][BPR_DIE_SHIFT_X][i+1];
		m_dGenDieShiftY[i]			= (*psmf)[BPR_DATA][PB_ALARM][BPR_DIE_SHIFT_Y][i+1];
		m_lGenMaxShiftAllow[i]		= (*psmf)[BPR_DATA][PB_ALARM][BPR_MAX_SHIFT_ALLOW][i+1];
		m_dGenMaxAngle[i]			= (*psmf)[BPR_DATA][PB_ALARM][BPR_MAX_ANGLE][i+1];
		m_lGenMaxAngleAllow[i]		= (*psmf)[BPR_DATA][PB_ALARM][BPR_MAX_ANGLE_ALLOW][i+1];
		m_lGenMaxAccAngleAllow[i]	= (*psmf)[BPR_DATA][PB_ALARM][BPR_ACC_ANGLE_ALLOW][i+1];
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to load BPR data from PackageData.msd");

	// close config file
    pUtl->ClosePackageDataConfig();

	return TRUE;
}


BOOL CBondPr::SavePackageMsdBPRData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadPackageDataConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetPackageDataConfig();

	//	Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	// PB alarm
	(*psmf)[BPR_DATA][PB_ALARM][BPR_ENABLE_PR_EMPTY_ALARM]		= m_bEmptyCheck;
	(*psmf)[BPR_DATA][PB_ALARM][BPR_ENABLE_PLACEMENT_CHECK]		= m_bPlacementCheck;
	(*psmf)[BPR_DATA][PB_ALARM][BPR_ENABLE_BT_ANGLE_CHECK]		= m_bBinTableAngleCheck;

	for (int i = 0; i < BPR_MAX_BINBLOCK; i++)
	{
		(*psmf)[BPR_DATA][PB_ALARM][BPR_EMPTY_CONT_LIMIT][i+1]	= m_lGenMaxEmptyAllow[i];
		(*psmf)[BPR_DATA][PB_ALARM][BPR_EMPTY_ACC_LIMIT][i+1]	= m_lGenMaxAccEmptyAllow[i];
		(*psmf)[BPR_DATA][PB_ALARM][BPR_DIE_SHIFT_X][i+1]		= m_dGenDieShiftX[i];
		(*psmf)[BPR_DATA][PB_ALARM][BPR_DIE_SHIFT_Y][i+1]		= m_dGenDieShiftY[i];
		(*psmf)[BPR_DATA][PB_ALARM][BPR_MAX_SHIFT_ALLOW][i+1]	= m_lGenMaxShiftAllow[i];
		(*psmf)[BPR_DATA][PB_ALARM][BPR_MAX_ANGLE][i+1]			= m_dGenMaxAngle[i];
		(*psmf)[BPR_DATA][PB_ALARM][BPR_MAX_ANGLE_ALLOW][i+1]	= m_lGenMaxAngleAllow[i];
		(*psmf)[BPR_DATA][PB_ALARM][BPR_ACC_ANGLE_ALLOW][i+1]	= m_lGenMaxAccAngleAllow[i];
	}

	pUtl->UpdatePackageDataConfig();	
	// close config file
    pUtl->ClosePackageDataConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to save BPR data from PackageData.msd");

	return TRUE;
}


BOOL CBondPr::UpdateBPRPackageList(VOID)
{
	CStdioFile fTemp;
	CString szContent, szText, szTemp;
	DOUBLE dTemp;
	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\BPRPackageList.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	// PB alarm
	szText = BPR_ENABLE_PR_EMPTY_ALARM;
	szText = szText + ",";
	szTemp.Format("%d,", m_bEmptyCheck);
	szContent = szContent + szText + szTemp;

	szText = BPR_ENABLE_PLACEMENT_CHECK;
	szText = szText + ",";
	szTemp.Format("%d,", m_bPlacementCheck);
	szContent = szContent + szText + szTemp;

	szText = BPR_ENABLE_BT_ANGLE_CHECK;
	szText = szText + ",";
	szTemp.Format("%d,", m_bBinTableAngleCheck);
	szContent = szContent + szText + szTemp;

	szText = BPR_EMPTY_CONT_LIMIT;
	szText = szText + ",";
	szTemp.Format("%d,", m_lGenMaxEmptyAllow[0]);
	szContent = szContent + szText + szTemp;

	szText = BPR_EMPTY_ACC_LIMIT;
	szText = szText + ",";
	szTemp.Format("%d,", m_lGenMaxAccEmptyAllow[0]);
	szContent = szContent + szText + szTemp;

	szText = BPR_DIE_SHIFT_X;
	szText = szText + ",";
	szTemp.Format("%d,", m_dGenDieShiftX[0]);
	szContent = szContent + szText + szTemp;

	szText = BPR_DIE_SHIFT_Y;
	szText = szText + ",";
	szTemp.Format("%d,", m_dGenDieShiftY[0]);
	szContent = szContent + szText + szTemp;

	szText = BPR_MAX_SHIFT_ALLOW;
	szText = szText + ",";
	szTemp.Format("%d,", m_lGenMaxShiftAllow[0]);
	szContent = szContent + szText + szTemp;

	szText = BPR_MAX_ANGLE;
	szText = szText + "Grade 1,";
	dTemp = m_dGenMaxAngle[0];
	szTemp.Format("%4.2f,", dTemp);
	szContent = szContent + szText + szTemp;

	szText = BPR_MAX_ANGLE_ALLOW;
	szText = szText + "Grade 1,";
	dTemp = m_lGenMaxAngleAllow[0];
	szTemp.Format("%4.2f,", dTemp);
	szContent = szContent + szText + szTemp;

	szText = BPR_ACC_ANGLE_ALLOW;
	szText = szText + "Grade 1,";
	dTemp = m_lGenMaxAccAngleAllow[0];
	szTemp.Format("%4.2f,", dTemp);
	szContent = szContent + szText + szTemp;
	
	fTemp.WriteString(szContent);
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to update BPR package data to BPRPackageList.csv");

	fTemp.Close();

	return TRUE;
}


LONG CBondPr::AutoLearnBondFFModeDie()
{
	PR_WIN	 stSrchWin;
	PR_UWORD usDieType;
	PR_COORD stDieOffset; 
	PR_REAL	 fDieRotate; 
	PR_REAL	 fDieScore;

	stSrchWin.coCorner1.x = PR_MIN_COORD + 256;
	stSrchWin.coCorner1.y = PR_MIN_COORD + 256;
	stSrchWin.coCorner2.x = PR_MAX_COORD - 256;
	stSrchWin.coCorner2.y = PR_MAX_COORD - 256;

	PR_UWORD lLearnStatus = 0;

	//Search reference die with user defined search region
	PR_WORD wResult = ManualSearchDie(BPR_NORMAL_DIE, BPR_NORMAL_DIE, PR_TRUE, PR_TRUE, PR_TRUE, 
							&usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
							stSrchWin.coCorner1, stSrchWin.coCorner2, 0, 35);

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			ManualDieCompenate(stDieOffset, fDieRotate);
			Sleep(25);

			LONG lWidth = m_stLearnDieCornerPos[PR_LOWER_RIGHT].x - m_stLearnDieCornerPos[PR_UPPER_LEFT].x;
			LONG lLength = m_stLearnDieCornerPos[PR_LOWER_RIGHT].y - m_stLearnDieCornerPos[PR_UPPER_LEFT].y;

			DOUBLE dRatio = 1;
			if (m_szBprZoomFactor == PR_ZOOM_VALUE_1X)
			{
				dRatio = 1;
			}
			else if (m_szBprZoomFactor == PR_ZOOM_VALUE_2X)
			{
				dRatio = 0.5;
			}
			else if (m_szBprZoomFactor == PR_ZOOM_VALUE_4X)
			{
				dRatio = 0.25;
			}

			lWidth = _round(lWidth * dRatio);
			lLength = _round(lLength * dRatio);


			m_stLearnDieCornerPos[PR_UPPER_LEFT].x	= (PR_WORD)(PR_DEF_CENTRE_X - lWidth /2);
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x = (PR_WORD)(PR_DEF_CENTRE_X + lWidth /2);
			m_stLearnDieCornerPos[PR_UPPER_LEFT].y	= (PR_WORD)(PR_DEF_CENTRE_Y - lLength /2);
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].y = (PR_WORD)(PR_DEF_CENTRE_Y + lLength /2);

			//Move Die to center
			//convert to 4X mode
			LONG lOldZoom = BPR_GetRunZoom();
			BPR_LiveViewZoom(CPrZoomSensorMode::PR_ZOOM_MODE_FF);
			//Auto learn FF Mode die
			lLearnStatus =  AutoManualLearnDie(BPR_NORMAL_DIE, 3);
			BPR_LiveViewZoom(lOldZoom);
		}
	}

	return lLearnStatus;
}


BOOL CBondPr::SearchBPRDie(BOOL bMoveToCenter, BOOL bTakeLog)
{
	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	LONG			ulBlkInUse = GetBTCurrentBlock();

	if (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		SetStatusMessage("F1: Bond PR no die record");
		SetErrorMessage("BPR no die record in UserSearchDie");
		return FALSE;
	}

	if (m_bDieCalibrated == FALSE)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not calibrated");
		SetErrorMessage("Bond PR calibration factor missing in UserSearchDie");
		return FALSE;
	}

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if (lInitPR != 1)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not initialized");
		SetErrorMessage("Bond PR is not initialized in UserSearchDie");
		return FALSE;
	}


	//Clear PR screen & Off joystick
	SelectBondCamera();
	SetBinTableJoystick(FALSE);

	DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);

	LONG lDieNo = BPR_NORMAL_DIE;
	ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(lDieNo), PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
						m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 36);

	if (DieIsAlignable(usDieType) == TRUE)
	{
		if (bMoveToCenter)
		{
			ManualDieCompenate(stDieOffset);
		}
		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
	}

	if (bTakeLog)
	{
		CString szLog;
		int siStepX = 0, siStepY = 0;

		if (DieIsAlignable(usDieType) == TRUE)
		{
			CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);
		}

		szLog.Format("DieOffset(ENC),%d,%d,DieOffset(Pixel),%d,%d,DieRotate,%f,DieScore,%f", 
			(LONG)siStepX, (LONG)siStepY, (LONG)stDieOffset.x, (LONG)stDieOffset.y, (DOUBLE)fDieRotate, (DOUBLE)fDieScore);
		CMSLogFileUtility::Instance()->BT_PostBondLog(szLog);
	}

	//On Joystick & Clear PR screen
	SetBinTableJoystick(TRUE);
	SelectBondCamera();

	return TRUE;
}

BOOL CBondPr::BprSearchDie(LONG &lStepX, LONG &lStepY, CString &szDieResult)
{
	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	LONG			ulBlkInUse = GetBTCurrentBlock();
	int siStepX = 0, siStepY = 0;

	LONG lBTENcX = lStepX;
	LONG lBTENcY = lStepY;
	lStepX = 0;
	lStepY = 0;


	if (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		SetStatusMessage("F1: Bond PR no die record");
		SetErrorMessage("BPR no die record in UserSearchDie");
		return FALSE;
	}

	if (m_bDieCalibrated == FALSE)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not calibrated");
		SetErrorMessage("Bond PR calibration factor missing in UserSearchDie");
		return FALSE;
	}

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if (lInitPR != 1)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not initialized");
		SetErrorMessage("Bond PR is not initialized in UserSearchDie");
		return FALSE;
	}


	//Clear PR screen & Off joystick
	//SelectBondCamera();
	//SetBinTableJoystick(FALSE);

	//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);

	LONG lDieNo = BPR_NORMAL_DIE;
	ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(lDieNo), PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
						m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 36);

	if (DieIsAlignable(usDieType) == TRUE)
	{
		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

		lStepX = siStepX;
		lStepY = siStepY;

		DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
	}

	//if (bTakeLog)
	//{
	CString szLog;
	szLog.Format("DieOffset(ENC),%d,%d,DieOffset(Pixel),%d,%d,DieRotate,%f,DieScore,%f,", 
					(LONG) siStepX, (LONG) siStepY, 
					(LONG) stDieOffset.x, (LONG) stDieOffset.y, 
					(DOUBLE) fDieRotate, (DOUBLE) fDieScore);
	szDieResult = szDieResult + szLog;
	//CMSLogFileUtility::Instance()->BT_PostBondLog(szLog);
	//}

	//On Joystick & Clear PR screen
	//SetBinTableJoystick(TRUE);
	//SelectBondCamera();
	return TRUE;
}

BOOL CBondPr::BprSearchLFDie(BOOL bTakeLog, UCHAR ucLFDir, BOOL bGrab, CString szDieLog, LONG &lStepX, LONG &lStepY)
{
	PR_UWORD		usDieType;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	LONG			ulBlkInUse = GetBTCurrentBlock();
	int siStepX = 0, siStepY = 0;

	PR_BOOLEAN bPRLatch = PR_TRUE;
	if (bGrab)
	{
		bPRLatch = PR_TRUE;
	}

	LONG lBTENcX = lStepX;
	LONG lBTENcY = lStepY;
	lStepX = 0;
	lStepY = 0;


	if (m_bGenDieLearnt[BPR_NORMAL_DIE] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		SetStatusMessage("F1: Bond PR no die record");
		SetErrorMessage("BPR no die record in UserSearchDie");
		return FALSE;
	}

	if (m_bDieCalibrated == FALSE)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not calibrated");
		SetErrorMessage("Bond PR calibration factor missing in UserSearchDie");
		return FALSE;
	}

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if (lInitPR != 1)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not initialized");
		SetErrorMessage("Bond PR is not initialized in UserSearchDie");
		return FALSE;
	}

	if ( (ucLFDir == 0) || (ucLFDir > BPR_DL_DIE) )
	{
		return FALSE;	//Do not use LF
	}

	//Clear PR screen & Off joystick
	//SelectBondCamera();
	//SetBinTableJoystick(FALSE);

	//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_RED);
	PR_WIN stSrchArea;
	stSrchArea = m_stSearchArea;

	/*
	#define		BPR_CT_DIE					0	//CENTER
	#define		BPR_LT_DIE					1	//LHS
	#define		BPR_RT_DIE					5	//RHS	
	#define		BPR_UP_DIE					3	//UP
	#define		BPR_DN_DIE					7	//DOWN
	#define		BPR_UL_DIE					2	//UP+LHS
	#define		BPR_DL_DIE					8	//DOWN+LHS	
	#define		BPR_UR_DIE					4	//UP+RHS
	#define		BPR_DR_DIE					6	//DOWN+RHS
	*/
	CalculateLFSearchArea((LONG) ucLFDir, 1, &stSrchArea);
	VerifyPRRegion(&stSrchArea);

	//DrawRectangleBox(stSrchArea.coCorner1, stSrchArea.coCorner2, PR_COLOR_RED);

	CString szLog;
	szLog.Format("BPR: showing LF #%d Window ....", ucLFDir);
//HmiMessage(szLog);

	LONG lDieNo = BPR_NORMAL_DIE;
	ManualSearchDie(BPR_NORMAL_DIE, GetBondPRDieNo(lDieNo), bPRLatch, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, 
						stSrchArea.coCorner1, stSrchArea.coCorner2);

	if (DieIsAlignable(usDieType) == TRUE)
	{
		CalculateDieCompenate(stDieOffset, &siStepX, &siStepY);

		lStepX = siStepX;
		lStepY = siStepY;

		bReturn = TRUE;
		//DrawRectangleBox(m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, PR_COLOR_TRANSPARENT);
		//DrawRectangleDieSize(PR_COLOR_GREEN, BPR_NORMAL_DIE, BPR_NORMAL_DIE);
	}
	else
	{
		bReturn = FALSE;
	}


	if (bTakeLog)
	{
		CString szLog;
		szLog.Format("LFDieOffset(ENC),%d,%d,LFDieOffset(Pixel),%d,%d,LFDieRotate,%f,LFDieScore,%f,LFWnd,%d", 
						(LONG) siStepX, (LONG) siStepY, 
						(LONG) stDieOffset.x, (LONG) stDieOffset.y, 
						(DOUBLE) fDieRotate, (DOUBLE) fDieScore, ucLFDir);
		CMSLogFileUtility::Instance()->BT_PostBondLog(szDieLog + szLog);
	}

	//On Joystick & Clear PR screen
	//SetBinTableJoystick(TRUE);
	//SelectBondCamera();
	return bReturn;
}

BOOL CBondPr::BprSerachWaferEjectorPin(LONG lEjTEnc, LONG lEjTCmd)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	BOOL		bStatus = TRUE;
	CString		szLog;


	if (m_bGenDieLearnt[BPR_REFERENCE_PR_DIE_INDEX5 + BPR_GEN_RDIE_OFFSET] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		SetStatusMessage("F1: Bond PR no die record");
		SetErrorMessage("BPR no die record in UserSearchDie");
		return FALSE;
	}

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if (lInitPR != 1)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not initialized");
		SetErrorMessage("Bond PR is not initialized in UserSearchDie");
		return FALSE;
	}

	PR_WORD wResult = ManualSearchDie(BPR_REFERENCE_DIE, BPR_REFERENCE_PR_DIE_INDEX5, PR_TRUE, PR_TRUE, PR_TRUE, 
		&usDieType, &fDieRotate, &stDieOffset, &fDieScore, m_stSearchArea.coCorner1, m_stSearchArea.coCorner2, 0, 37);

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			int nOffsetX = 0, nOffsetY = 0;
			DOUBLE dXum = 0, dYum = 0;

			CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);
			CalculateNDieOffsetXY(nOffsetX, nOffsetY);
			ConvertPixelToUM(stDieOffset, dXum, dYum);

			szLog.Format("Bpr Search Wafer Ejector Pin,Pixel Offset,%d,%d,ENC Offset,%d,%d,um offset,%d,%d,Rotate,%f,EjT ENC,%d,EjT CMD,%d", 
				(int)stDieOffset.x, (int)stDieOffset.y, nOffsetX, nOffsetY, dXum, dYum, (DOUBLE)fDieRotate, lEjTEnc, lEjTCmd);

			CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog, TRUE);
		}
		else
		{
			bStatus = FALSE;

			szLog.Format("Bpr Search Wafer Ejector Pin,ErrorDieType,%d", (UINT) usDieType);
			CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog, TRUE);
		}
	}
	else
	{
		bStatus = FALSE;

		szLog.Format("Bpr Search Wafer Ejector Pin,SearchDieError,%d", (INT) wResult);
		CMSLogFileUtility::Instance()->WPR_WaferDieOffsetLog(szLog, TRUE);
	}
	

	return bStatus;
}

/*
PR_UWORD CBondPr::GrabShareImage(BOOL bDieType, LONG lInputDieNo, PR_UBYTE ubSID, PR_UBYTE ubRID)
{
	LONG		lDieNo = 0;
	PR_UWORD	lPrSrchID;
	CString		szMsg;

	if (lInputDieNo <= 0)
		lInputDieNo = 1;
	if (bDieType == BPR_NORMAL_DIE)
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	if (bDieType == BPR_REFERENCE_DIE)
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	lPrSrchID	= m_ssGenPRSrchID[lDieNo];

	if (lPrSrchID == 0)
	{
		return IDS_BPR_DIENOTLEARNT;
	}

	PR_GRAB_SHARE_IMAGE_CMD		stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);

	stGrbCmd.emPurpose			= MS899_BOND_PB_PURPOSE;//MS899_BOND_GEN_PURPOSE; //PR_PURPOSE_CAM_A1;
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= lPrSrchID;
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_HW_TRIGGER;
	stGrbCmd.emDisplayAutoBond	= PR_FALSE; 

	PR_GRAB_SHARE_IMAGE_RPY		stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSID, ubRID, &stGrbRpy);

	if (stGrbRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		szMsg.Format("BPR - ManualSearchDie_HWTri: grab share image rpy1 fail com %d", stGrbRpy.uwCommunStatus);
		HmiMessage_Red_Back(szMsg);
		return stGrbRpy.uwCommunStatus;
	}

	return PR_ERR_NOERR;
}


BOOL CBondPr::GrabHwTrigger()
{
	MotionSetOutputBit("EJ_TABLE_BIN_OUT_BIT_BIN_CAMERA_TRIGGER", TRUE);
	DOUBLE dSetBitStartTime = GetTime();

	//Exposure time
	DOUBLE dDelay	= 3 - _round(GetTime() - dSetBitStartTime);
	LONG lDelay		= _round(dDelay);
	if (lDelay > 0)
	{
		Sleep(lDelay);
	}

	MotionSetOutputBit("EJ_TABLE_BIN_OUT_BIT_BIN_CAMERA_TRIGGER", FALSE);

	return TRUE;
}


PR_UWORD CBondPr::GrabShareImageRpy2(PR_UBYTE ubSID, PR_ULWORD *ulImageID, PR_ULWORD *ulStationID)
{
	CString szMsg;
	
	//Grab Done
	PR_GRAB_SHARE_IMAGE_RPY2 stGrbRpy2;
	PR_GrabShareImgRpy(ubSID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("BPR - GrabHWTrigger: grab share image rpy2 fail com %d, status %d",
							stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		if (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus)
		{
			szMsg.Format("BPR - GrabHwTrigger: PR_GrabShareImgRpy fail PRERR =0x%d", stGrbRpy2.stStatus.uwPRStatus);
			HmiMessage_Red_Back(szMsg);
			return stGrbRpy2.stStatus.uwPRStatus;
		}
		else
		{
			szMsg.Format("BPR - GrabHwTrigger: PR_GrabShareImgRpy fail COMERR =0x%d", stGrbRpy2.stStatus.uwCommunStatus);
			HmiMessage_Red_Back(szMsg);
			return stGrbRpy2.stStatus.uwCommunStatus;
		}
	}

	// now add the image to queue
	*ulStationID = stGrbRpy2.ulStationID;
	*ulImageID	 = stGrbRpy2.ulImageID;

	return PR_ERR_NOERR;
}
*/

BOOL CBondPr::BprSerachDieOnCenterOfRotation(LONG lBTThetaEnc, DOUBLE dRotateDegree)
{
	PR_UWORD	usDieType = PR_ERR_NO_OBJECT_FOUND;
	PR_COORD	stDieOffset; 
	PR_REAL		fDieRotate; 
	PR_REAL		fDieScore;
	BOOL		bStatus = TRUE;
	CString		szLog;


	if (m_bGenDieLearnt[BPR_NORMAL_PR_DIE_INDEX1 + BPR_GEN_NDIE_OFFSET] == FALSE)
	{
		SetAlert(IDS_BPR_DIENOTLEARNT);
		SetStatusMessage("F1: Bond PR no die record");
		SetErrorMessage("BPR no die record in UserSearchDie");
		return FALSE;
	}

	LONG lInitPR = (*m_psmfSRam)["WaferPr"]["InitPR"];
	if (lInitPR != 1)
	{
		SetAlert(IDS_BPR_DIE_NOT_READY);
		SetStatusMessage("F1: Bond PR is not initialized");
		SetErrorMessage("Bond PR is not initialized in UserSearchDie");
		return FALSE;
	}

	PR_WIN stSearchArea;
	stSearchArea.coCorner1.x	= PR_MAX_WIN_ULC_X;
	stSearchArea.coCorner1.y	= PR_MAX_WIN_ULC_Y;
	stSearchArea.coCorner2.x	= PR_MAX_WIN_LRC_X;
	stSearchArea.coCorner2.y	= PR_MAX_WIN_LRC_Y;

	PR_WORD wResult = ManualSearchDie(BPR_NORMAL_DIE, BPR_NORMAL_PR_DIE_INDEX1, PR_TRUE, PR_TRUE, PR_TRUE, 
		&usDieType, &fDieRotate, &stDieOffset, &fDieScore, stSearchArea.coCorner1, stSearchArea.coCorner2, -1 * dRotateDegree, 38);

	if (wResult != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			int nOffsetX = 0, nOffsetY = 0;
			DOUBLE dXum = 0, dYum = 0;

			CalculateDieCompenate(stDieOffset, &nOffsetX, &nOffsetY);
			CalculateNDieOffsetXY(nOffsetX, nOffsetY);
			ConvertPixelToUM(stDieOffset, dXum, dYum);

			szLog.Format("Bpr Serach Rotate Die OK,Pixel Offset,%d,%d,ENC Offset,%d,%d,um offset,%d,%d,Die Rotate,%f,Theta Enc,%d,Theta Degree,%f", 
				(int)stDieOffset.x, (int)stDieOffset.y, nOffsetX, nOffsetY, dXum, dYum, (DOUBLE)fDieRotate, lBTThetaEnc, dRotateDegree);
			CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);
		}
		else
		{
			bStatus = FALSE;

			szLog.Format("Bpr Serach Rotate Die FAIL,Pixel Offset,%d,%d,ENC Offset,%d,%d,um offset,%d,%d,Die Rotate,%f,Theta Enc,%d,Theta Degree,%f", 
				(int)stDieOffset.x, (int)stDieOffset.y, 0, 0, 0, 0, (DOUBLE)fDieRotate, lBTThetaEnc, dRotateDegree);
			CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);
		}
	}
	else
	{
		bStatus = FALSE;

		szLog.Format("Bpr Serach Rotate Die FAIL2,Pixel Offset,%d,%d,ENC Offset,%d,%d,um offset,%d,%d,Die Rotate,%f,Theta Enc,%d,Theta Degree,%f", 
			(int)stDieOffset.x, (int)stDieOffset.y, 0, 0, 0, 0, (DOUBLE)fDieRotate, lBTThetaEnc, dRotateDegree);
		CMSLogFileUtility::Instance()->BT_ThetaCorrectionLog(szLog);
	}
	
	return bStatus;
}
