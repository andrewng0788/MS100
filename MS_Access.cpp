/////////////////////////////////////////////////////////////////
// MS_Access.cpp : Function to enable/disable HMI control access
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, Nov 04, 2005
//	Revision:	1.00
//
//	By:			Barry Chu
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MS896A.h"
#include "MS_AccessConstant.h"
#include "FileUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CMS896AApp::ResetScreenControlAccessMode()
{
	m_bAcBHMotorSetup		= FALSE;
	m_bAcBondArmSetup		= FALSE;
	m_bAcBondHeadSetup		= FALSE;
	m_bAcDelaySetup			= FALSE;
	m_bAcEjectorSetup		= FALSE;
	m_bAcBinBlkEditSetup	= FALSE;
	m_bAcBinBlkClearSetup	= FALSE;
	m_bAcClearBinFileSettings = FALSE;
	m_bAcClearBinCounter	= FALSE;
	m_bAcNoGenOutputFile	= FALSE;
	m_bAcWLLoaderSetup		= FALSE;
	m_bAcWLExpanderSetup	= FALSE;
	m_bAcWLAlignmentSetup	= FALSE;
	m_bAcWLOthersSetup		= FALSE;
	m_bAcWLMotorSetup		= FALSE;
	m_bAcBLGripperSetup		= FALSE;
	m_bAcBLMagazineSetup	= FALSE;
	m_bAcBLOthersSetup		= FALSE;
	m_bAcBLOthersManualOp	= FALSE;
	m_bAcBLMotorSetup		= FALSE;
	m_bAcMapPathSetting		= FALSE;
	m_bAcAlignWafer			= FALSE;
	m_bAcDisableSCNSettings = FALSE;
	m_bAcDisableManualAlign = FALSE;
	m_bAcMapSetting			= FALSE;
	m_bAcMapOptions			= FALSE;
	m_bAcLoadMap			= FALSE;
	m_bAcClearMap			= FALSE;
	m_bAcWaferTableSetup	= FALSE;
	m_bAcCollectnEjSetup	= FALSE;
	m_bAcWaferMapSetup		= FALSE;
	// huga
	m_bAcWaferPrAdvSetup	= FALSE;
	m_bAcWaferPrLrnRefProtect = FALSE;
	m_bAcWaferPrLrnAlgrProtect = FALSE;

	m_bAcBondPrAdvSetup		= FALSE;
	m_bAcBondPrLrnRefProtect = FALSE;

	m_bAcBinTableLimit = FALSE;
	m_bAcBinTableColletOffset = FALSE;

	m_bAcPKGFileSettings	= FALSE;

	m_bAcWaferLotSettings = FALSE;

	m_bAcDieCheckOptions	= FALSE;
	m_bAcWaferEndOptions	= FALSE;

	BOOL bEnableOpenButton = TRUE;
	if( GetCustomerName()=="SanAn" && GetProductLine()=="WH" )	//	not allow to load map manually
	{
		bEnableOpenButton = FALSE;
	}
	CMS896AStn::m_WaferMapWrapper.EnableOpenButton(bEnableOpenButton);
	m_smfSRam["MSOperator"]["DisableLoadMap"] = !bEnableOpenButton;

	CMS896AStn::m_WaferMapWrapper.EnableInitButton(TRUE);

	return TRUE;
}


BOOL CMS896AApp::ReadScreenControlAccessMode()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	BOOL bFileExist = FALSE;
	short	i = 0;	

	// open config file
    if (pUtl->LoadAccessMode() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetAccessModeFile();

	if ( psmf == NULL )
		return FALSE;

	//Read content
	m_bAcBHMotorSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_MOTOR];
	m_bAcBondArmSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_ARM];
	m_bAcBondHeadSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_HEAD];
	m_bAcDelaySetup			= (BOOL)(LONG)(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_DELAY];
	m_bAcEjectorSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_EJECTOR];

	m_bAcBinBlkEditSetup	= (BOOL)(LONG)(*psmf)[MS_ACC_BINBLOCK][MS_ACC_BK_SETUP_EDIT];
	m_bAcBinBlkClearSetup	= (BOOL)(LONG)(*psmf)[MS_ACC_BINBLOCK][MS_ACC_BK_SETUP_CLEAR];
	m_bAcClearBinFileSettings = (BOOL)(LONG)(*psmf)[MS_ACC_BINBLOCK][MS_ACC_CLEAR_BIN_FILE_SETTINGS];
	m_bAcClearBinCounter	= (BOOL)(LONG)(*psmf)[MS_ACC_BINBLOCK][MS_ACC_CLEAR_BIN_COUNTER];
	m_bAcNoGenOutputFile	= (BOOL)(LONG)(*psmf)[MS_ACC_BINBLOCK][MS_ACC_NOGEN_OUTPUT_FILE];

	m_bAcWLLoaderSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_LOADER];
	m_bAcWLExpanderSetup	= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_EXPANDER];
	m_bAcWLAlignmentSetup	= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_ALIGNMENT];
	m_bAcWLOthersSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_OTHERS];
	m_bAcWLMotorSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_MOTOR];

	m_bAcBLGripperSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_GRIPPER];
	m_bAcBLMagazineSetup	= (BOOL)(LONG)(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_MAGAZINE];
	m_bAcBLOthersSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_OTHERS];
	// Manual Operation (Load/Unload part) in Others Tab
	m_bAcBLOthersManualOp	= (BOOL)(LONG)(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_OTHERS_MANUAL_OPERATION];
	m_bAcBLMotorSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_MOTOR];

	m_bAcMapPathSetting		= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MAP_PATH_SETTING];
	m_bAcAlignWafer			= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_ALIGN_WAFER];
	m_bAcDisableManualAlign = (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MANUAL_ALIGN];
	m_bAcDisableSCNSettings = (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_SCN_SETTINGS];
	m_bAcMapSetting			= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MAP_SETTING];
	m_bAcMapOptions			= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MAP_OPTIONS];
	m_bAcLoadMap			= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_LOAD_MAP];
	m_bAcClearMap			= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP][MS_ACC_CLEAR_MAP];

	m_bAcWaferTableSetup	= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERTABLE][MS_ACC_WT_SETUP];
	m_bAcCollectnEjSetup	= (BOOL)(LONG)(*psmf)[MS_ACC_BOND_PAGE][MS_ACC_BOND_COLLECT_EJ_SETUP];
	m_bAcWaferMapSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BOND_PAGE][MS_ACC_BOND_WAFER_MAP_SETUP];

	m_bAcDieCheckOptions	= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP_SUBPAGE][MS_ACC_DIE_CHECK_OPTIONS];
	m_bAcWaferEndOptions	= (BOOL)(LONG)(*psmf)[MS_ACC_WAFERMAP_SUBPAGE][MS_ACC_WAFER_END_OPTIONS];

	// huga
	m_bAcWaferPrAdvSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_WPR][MS_ACC_WPR_SETUP_ADV];
	m_bAcWaferPrLrnRefProtect	= (BOOL)(LONG)(*psmf)[MS_ACC_WPR][MS_ACC_WPR_LRN_DIE_PROTECT];
	m_bAcWaferPrLrnAlgrProtect	= (BOOL)(LONG)(*psmf)[MS_ACC_WPR][MS_ACC_WPR_LRN_DIE_ALGR_PRTECT];

	m_bAcBondPrAdvSetup		= (BOOL)(LONG)(*psmf)[MS_ACC_BPR][MS_ACC_BPR_SETUP_ADV];
	m_bAcBondPrLrnRefProtect = (BOOL)(LONG)(*psmf)[MS_ACC_BPR][MS_ACC_BPR_LRN_DIE_PROTECT];

	m_bAcBinTableLimit = (BOOL)(LONG)(*psmf)[MS_ACC_BINTABLE][MS_ACC_BINTABLE_LIMIT];
	m_bAcBinTableColletOffset = (BOOL)(LONG)(*psmf)[MS_ACC_BINTABLE][MS_ACC_COLLET_OFFSET];

	m_bAcPKGFileSettings	= (BOOL)(LONG)(*psmf)[MS_ACC_GENERAL][MS_ACC_PKG_FILE_SETTINGS];

	m_bAcWaferLotSettings = (BOOL)(LONG)(*psmf)[MS_ACC_GENERAL][MS_ACC_WAFER_LOT_SETTINGS];
	

	//Update HMI screen variable
	m_bAsBHMotorSetup		= m_bAcBHMotorSetup;
	m_bAsBondArmSetup		= m_bAcBondArmSetup;
	m_bAsBondHeadSetup		= m_bAcBondHeadSetup;
	m_bAsDelaySetup			= m_bAcDelaySetup;
	m_bAsEjectorSetup		= m_bAcEjectorSetup;

	m_bAsBinBlkEditSetup	= m_bAcBinBlkEditSetup;
	m_bAsBinBlkClearSetup	= m_bAcBinBlkClearSetup;
	m_bAsClearBinFileSettings = m_bAcClearBinFileSettings;
	m_bAsClearBinCounter	= m_bAcClearBinCounter;
	m_bAsNoGenOutputFile	= m_bAcNoGenOutputFile;
	m_bAsWLLoaderSetup		= m_bAcWLLoaderSetup;	
	m_bAsWLExpanderSetup	= m_bAcWLExpanderSetup;
	m_bAsWLAlignmentSetup	= m_bAcWLAlignmentSetup;
	m_bAsWLOthersSetup		= m_bAcWLOthersSetup;
	m_bAsWLMotorSetup		= m_bAcWLMotorSetup;

	m_bAsBLGripperSetup		= m_bAcBLGripperSetup;
	m_bAsBLMagazineSetup	= m_bAcBLMagazineSetup;
	m_bAsBLOthersSetup		= m_bAcBLOthersSetup;
	m_bAsBLOthersManualOp	= m_bAcBLOthersManualOp;
	m_bAsBLMotorSetup		= m_bAcBLMotorSetup;

	m_bAsMapPathSetting		= m_bAcMapPathSetting;
	m_bAsAlignWafer			= m_bAcAlignWafer;
	m_bAsDisableManualAlign	= m_bAcDisableManualAlign;
	m_bAsDisableSCNSettings	= m_bAcDisableSCNSettings;
	m_bAsMapSetting			= m_bAcMapSetting;
	m_bAsMapOptions			= m_bAcMapOptions;
	m_bAsLoadMap			= m_bAcLoadMap;
	m_bAsClearMap			= m_bAcClearMap;

	m_bAsWaferTableSetup	= m_bAcWaferTableSetup;
	
	m_bAsCollectnEjSetup	= m_bAcCollectnEjSetup;
	m_bAsWaferMapSetup		= m_bAcWaferMapSetup;

	m_bAsDieCheckOptions	= m_bAcDieCheckOptions;
	m_bAsWaferEndOptions	= m_bAcWaferEndOptions;

	// huga
	m_bAsWaferPrAdvSetup	= m_bAcWaferPrAdvSetup;
	m_bAsWaferPrLrnRefProtect	= m_bAcWaferPrLrnRefProtect;
	m_bAsWaferPrLrnAlgrProtect	= m_bAcWaferPrLrnAlgrProtect;

	m_bAsBondPrAdvSetup		= m_bAcBondPrAdvSetup;
	m_bAsBondPrLrnRefProtect = m_bAcBondPrLrnRefProtect;

	m_bAsBinTableLimit = m_bAcBinTableLimit;
	m_bAsBinTableColletOffset = m_bAcBinTableColletOffset;

	m_bAsPKGFileSettings	= m_bAcPKGFileSettings;

	m_bAsWaferLotSettings = m_bAcWaferLotSettings;
    
    // close config file
    pUtl->CloseAccessMode();

	// disable wafer map buttons
	BOOL bEnableOpenButton = !m_bAsLoadMap;
	if( GetCustomerName()=="SanAn" && GetProductLine()=="WH" )	//	not allow to load map manually
	{
		bEnableOpenButton = FALSE;
	}
	CMS896AStn::m_WaferMapWrapper.EnableOpenButton(bEnableOpenButton);

	m_smfSRam["MSOperator"]["DisableLoadMap"] = !bEnableOpenButton;

	if (m_bAsClearMap)
	{
		CMS896AStn::m_WaferMapWrapper.EnableInitButton(FALSE);
	}
	else
	{	
		CMS896AStn::m_WaferMapWrapper.EnableInitButton(TRUE);
	}

	return TRUE;
}


BOOL CMS896AApp::WriteScreenControlAccessMode()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	// open config file
    if (pUtl->LoadAccessMode() == FALSE)
		return FALSE;

    // get file pointer
    psmf = pUtl->GetAccessModeFile();

	if ( psmf == NULL )
		return FALSE;

	//Write content from HMI display variable
	(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_MOTOR]			= m_bAsBHMotorSetup;
	(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_ARM]			= m_bAsBondArmSetup;
	(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_HEAD]			= m_bAsBondHeadSetup;
	(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_DELAY]			= m_bAsDelaySetup;
	(*psmf)[MS_ACC_BONDHEAD][MS_ACC_BH_SETUP_EJECTOR]		= m_bAsEjectorSetup;

	(*psmf)[MS_ACC_BINBLOCK][MS_ACC_BK_SETUP_EDIT]			= m_bAsBinBlkEditSetup;
	(*psmf)[MS_ACC_BINBLOCK][MS_ACC_BK_SETUP_CLEAR]			= m_bAsBinBlkClearSetup;
	(*psmf)[MS_ACC_BINBLOCK][MS_ACC_CLEAR_BIN_FILE_SETTINGS] = m_bAsClearBinFileSettings;
	(*psmf)[MS_ACC_BINBLOCK][MS_ACC_CLEAR_BIN_COUNTER]		= m_bAsClearBinCounter;
	(*psmf)[MS_ACC_BINBLOCK][MS_ACC_NOGEN_OUTPUT_FILE]		= m_bAsNoGenOutputFile;

	(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_LOADER]		= m_bAsWLLoaderSetup;
	(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_EXPANDER]	= m_bAsWLExpanderSetup;
	(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_ALIGNMENT]	= m_bAsWLAlignmentSetup;
	(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_OTHERS]		= m_bAsWLOthersSetup;
	(*psmf)[MS_ACC_WAFERLOADER][MS_ACC_WL_SETUP_MOTOR]		= m_bAsWLMotorSetup;

	(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_GRIPPER]		= m_bAsBLGripperSetup;
	(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_MAGAZINE]		= m_bAsBLMagazineSetup;
	(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_OTHERS]		= m_bAsBLOthersSetup;
	// Manual Operation (Load/Unload part) in Others Tab
	(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_OTHERS_MANUAL_OPERATION] = m_bAsBLOthersManualOp;
	(*psmf)[MS_ACC_BINLOADER][MS_ACC_BL_SETUP_MOTOR]		= m_bAsBLMotorSetup;

	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MAP_PATH_SETTING] = m_bAsMapPathSetting;
	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_ALIGN_WAFER] = m_bAsAlignWafer;
	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_SCN_SETTINGS] = m_bAsDisableSCNSettings;
	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MANUAL_ALIGN] = m_bAsDisableManualAlign;
	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MAP_SETTING] = m_bAsMapSetting;
	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_MAP_OPTIONS] = m_bAsMapOptions;
	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_LOAD_MAP]	 = m_bAsLoadMap;
	(*psmf)[MS_ACC_WAFERMAP][MS_ACC_CLEAR_MAP]	 = m_bAsClearMap;

	(*psmf)[MS_ACC_WAFERTABLE][MS_ACC_WT_SETUP] = m_bAsWaferTableSetup;

	(*psmf)[MS_ACC_BOND_PAGE][MS_ACC_BOND_COLLECT_EJ_SETUP] = m_bAsCollectnEjSetup;
	(*psmf)[MS_ACC_BOND_PAGE][MS_ACC_BOND_WAFER_MAP_SETUP] = m_bAsWaferMapSetup;

	(*psmf)[MS_ACC_WAFERMAP_SUBPAGE][MS_ACC_DIE_CHECK_OPTIONS] = m_bAsDieCheckOptions;
	(*psmf)[MS_ACC_WAFERMAP_SUBPAGE][MS_ACC_WAFER_END_OPTIONS] = m_bAsWaferEndOptions;

	// huga
	(*psmf)[MS_ACC_WPR][MS_ACC_WPR_SETUP_ADV]		= m_bAsWaferPrAdvSetup;
	(*psmf)[MS_ACC_WPR][MS_ACC_WPR_LRN_DIE_PROTECT] = m_bAsWaferPrLrnRefProtect;
	(*psmf)[MS_ACC_WPR][MS_ACC_WPR_LRN_DIE_ALGR_PRTECT]  = m_bAsWaferPrLrnAlgrProtect;

	(*psmf)[MS_ACC_BPR][MS_ACC_BPR_SETUP_ADV] = m_bAsBondPrAdvSetup;
	(*psmf)[MS_ACC_BPR][MS_ACC_BPR_LRN_DIE_PROTECT] = m_bAsBondPrLrnRefProtect;
	
	(*psmf)[MS_ACC_BINTABLE][MS_ACC_BINTABLE_LIMIT] = m_bAsBinTableLimit;
	(*psmf)[MS_ACC_BINTABLE][MS_ACC_COLLET_OFFSET] = m_bAsBinTableColletOffset;

	(*psmf)[MS_ACC_GENERAL][MS_ACC_PKG_FILE_SETTINGS] = m_bAsPKGFileSettings;

	(*psmf)[MS_ACC_GENERAL][MS_ACC_WAFER_LOT_SETTINGS] = m_bAsWaferLotSettings;


    // close config file
    pUtl->UpdateAccessMode();
    pUtl->CloseAccessMode();

	return TRUE;
}
