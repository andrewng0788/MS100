/////////////////////////////////////////////////////////////////
// MS_Command.cpp : HMI Registered Command of the CMS896AApp class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, April 21, 2005
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2005.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "MS896A.h"
#include "HmiDataManager.h"
#include "CycleState.h"
#include "MS_AccessConstant.h"
#include "FileUtil.h"
#include "MS_SecCommConstant.h"
#include "MS_HostCommConstant.h"
#include  "MESConnector.h"
#include "StrInputDlg.h"
//#include "Encryption.h"
#include "SecsComm.h"
#include "WaferPr.h"
#include "BondHead.h"
#include "BondPr.h"
#include "WaferTable.h"
#include "BinTable.h"
#include "WaferLoader.h"
#include "Utility.h"

#include <atltime.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Define the IPC Service Commands
const CString gszHARDWARE_INITIAL	= _T("Initialize");
const CString gszSYSTEM_INITIAL		= _T("SystemInit");
const CString gszDIAG_COMMAND		= _T("Diagnostic");
const CString gszAUTO_COMMAND		= _T("AutoCycle");
const CString gszMANUAL_COMMAND		= _T("Manual");
const CString gszDEMO_COMMAND		= _T("Demonstration");
const CString gszRESET_COMMAND		= _T("Reset");
const CString gszDE_INITIAL_COMMAND	= _T("DeInitialize");


VOID CMS896AApp::SetupIPC()
{
	if (m_fCreated)
	{
		return;
	}

	try
	{
		Create(m_szModuleName, m_szGroupID);
		DisplayMessage("Communication initialized ...");

		// Register Service Commands
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),		&CMS896AApp::UpdateAction);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAppData"),	&CMS896AApp::UpdateAppData);		
		//m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),		LogItems);		
		m_comServer.IPC_REG_SERVICE_COMMAND(gszHARDWARE_INITIAL,	&CMS896AApp::SrvInitialize);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszSYSTEM_INITIAL,		&CMS896AApp::SrvSystemInitialize);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszDIAG_COMMAND,		&CMS896AApp::SrvDiagCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszAUTO_COMMAND,		&CMS896AApp::SrvAutoCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszMANUAL_COMMAND,		&CMS896AApp::SrvManualCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Demonstration"),	&CMS896AApp::SrvDemoCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszSTOP_COMMAND,		&CMS896AApp::SrvStopCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SrvStopAutoBond"),	&CMS896AApp::SrvStopAutoBond);		//andrewng //2020-0817
		m_comServer.IPC_REG_SERVICE_COMMAND(gszRESET_COMMAND,		&CMS896AApp::SrvResetCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(gszDE_INITIAL_COMMAND,	&CMS896AApp::SrvDeInitialCommand);
		//For autocycle
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("BurnIn"),			&CMS896AApp::SrvBurnInCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopBurnIn"),		&CMS896AApp::SrvStopBurnInCommand);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreStartCycle"),	&CMS896AApp::PreStartCycle);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfAllTaskInIdleState"), &CMS896AApp::CheckIfAllTaskInIdleState);	//v4.42T6
		// Exit 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Exit"),				&CMS896AApp::Exit);
		//Global Settings
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMapFilePath"),		&CMS896AApp::SaveMapFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SavePasswordFilePath"),	&CMS896AApp::SavePasswordFilePath);				//v4.17T7	//Sanan
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMapFileExt"),		&CMS896AApp::SaveMapFileExt);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOutputFilePath"),	&CMS896AApp::SaveOutputFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOutputFileFormat"), &CMS896AApp::SaveOutputFileFormat);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveWaferEndFilePath"), &CMS896AApp::SaveWaferEndFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveWaferEndFileFormat"), &CMS896AApp::SaveWaferEndFileFormat);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMachineNo"),		&CMS896AApp::SaveMachineNo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMachineNo2"),		&CMS896AApp::SaveMachineNo2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMachineName"),		&CMS896AApp::SaveMachineName);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveLotNumber"),		&CMS896AApp::SaveLotNumber);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveLotNumberPrefix"),	&CMS896AApp::SaveLotNumberPrefix);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveShiftID"),			&CMS896AApp::SaveShiftID);		//v3.32T3
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveRecipeTableFilePath"),	&CMS896AApp::SaveRecipeTableFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetRecipeTableFilePath"),	&CMS896AApp::SetRecipeTableFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetLotNumber"),		&CMS896AApp::ResetLotNumber);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinLotDirectory"),	&CMS896AApp::SaveBinLotDirectory);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetBinLotDirectory"), &CMS896AApp::ResetBinLotDirectory);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveAssociateFile"),	&CMS896AApp::SaveAssociateFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetAssociateFile"),	&CMS896AApp::ResetAssociateFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetPkgFileListPath"),	&CMS896AApp::GetPkgFileListPath);			//v4.21T7	//Walsin China
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetAmiFilePath"),		&CMS896AApp::GetAmiFilePath);				//v4.21T7	//Walsin China
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetDeviceFileName"),	&CMS896AApp::GetDeviceFileName);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveDeviceFile"),		&CMS896AApp::SaveDeviceFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("RestoreDeviceFile"),	&CMS896AApp::RestoreDeviceFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePackageFileInterface"),	&CMS896AApp::UpdatePackageFileInterface);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PackageFileSetupPreRoutine"),	&CMS896AApp::PackageFileSetupPreRoutine);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveGenericDeviceFile"),		&CMS896AApp::SaveGenericDeviceFile);			//xyz
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckGenericDeviceFile"),		&CMS896AApp::CheckGenericDeviceFile);		//v4.59A15
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadGenericDeviceFile"),		&CMS896AApp::LoadGenericDeviceFile);			//xyz
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("DeletePackageFile"),	&CMS896AApp::DeletePackageFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LoadAmiFile"),			&CMS896AApp::LoadAmiFile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetPackagePath"),		&CMS896AApp::GetPackagePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePackageFilePath"), &CMS896AApp::UpdatePackageFilePath);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAccessMode"),		&CMS896AApp::UpdateAccessMode);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckIfSubmountLot"),	&CMS896AApp::CheckIfSubmountLot);		//v3.27T1	//PLLM 
		// Host Comm (TCP/IP) Service
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHostCommSetting"),	&CMS896AApp::UpdateHostCommSetting);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HostCommConnection"),		&CMS896AApp::HostCommConnection);		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HostCommTest"),				&CMS896AApp::HostCommTest);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_DownloadPPG",				&CMS896AApp::SC_CmdDownloadPPG);
        m_comServer.IPC_REG_SERVICE_COMMAND("SC_RequestPPG",				&CMS896AApp::SC_CmdRequestPPG);
		// Startup
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WarmStart"),				&CMS896AApp::WarmStart);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ColdStart"),				&CMS896AApp::ColdStart);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogOff"),					&CMS896AApp::LogOff);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckAccessRight"),					&CMS896AApp::CheckAccessRight);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckAccessRight_MinimizeHmi"),		&CMS896AApp::CheckAccessRight_MinimizeHmi);		//v4.24T11
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckAccessRight_ColletEjLmts"),	&CMS896AApp::CheckAccessRight_ColletEjLmts);		//v4.24T11
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CreeCheckAccessRight"),				&CMS896AApp::CreeCheckAccessRight);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckAccessRight_FixOne"),			&CMS896AApp::CheckAccessRight_FixOne);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ManualGenerateParametersRecord"),	&CMS896AApp::ManualGenerateParametersRecord);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveOperatorId"),					&CMS896AApp::SaveOperatorId);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveLumiledsOperatorId"),			&CMS896AApp::SaveLumiledsOperatorId);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdatePassword"),					&CMS896AApp::UpdatePassword);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetPassword"),					&CMS896AApp::ResetPassword);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ClearWholeNVRAM"),					&CMS896AApp::ClearWholeNVRAM);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetHiPEC"),						&CMS896AApp::ResetHiPEC);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateHmiAccessPw"),				&CMS896AApp::UpdateHmiAccessPw);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetHmiAccessPw"),					&CMS896AApp::ResetHmiAccessPw);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangeHmiAccessLevel"),				&CMS896AApp::ChangeHmiAccessLevel);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ReloadMachineProfile"),				&CMS896AApp::ReloadMachineProfileCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ExportMachineProfile"),				&CMS896AApp::ExportMachineProfile);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ImportMachineProfile"),				&CMS896AApp::ImportMachineProfile);
		// Bin Lot Info
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinWorkNo"),			&CMS896AApp::SaveBinWorkNo);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveBinFileNamePrefix"),	&CMS896AApp::SaveBinFileNamePrefix);
		//Map sub folder
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMapSubFolderName"),		&CMS896AApp::SaveMapSubFolderName);
		// For machine logging
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableMachineLog"),			&CMS896AApp::EnableMachineLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableWtMachineLog"),		&CMS896AApp::EnableWtMachineLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableAutoRecoverPR"),		&CMS896AApp::EnableAutoRecoverPR);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableMsMachineLog"),		&CMS896AApp::EnableMsMachineLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableMsTableIndexLog"),	&CMS896AApp::EnableMsTableIndexLog);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("EnableMappingLog"),			&CMS896AApp::EnableMappingLog);
		// For new lot start
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CheckStartNewLot"),			&CMS896AApp::CheckStartNewLot);		//v4.16T5
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartNewLot"),				&CMS896AApp::StartNewLot);
		// For machine time
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveAndResetTime"),			&CMS896AApp::SaveAndResetTime);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StartSetupTime"),			&CMS896AApp::StartSetupTime);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("StopSetupTime"),			&CMS896AApp::StopSetupTime);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("WaitingSetupButtom"),		&CMS896AApp::WaitingSetupButtom);  // v4.51D5 SanAn(XA) 6

		//	AWET	engineer/operator recover screen functions
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AWET_EngineerLogIn"),		&CMS896AApp::AWET_EngineerLogIn); 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AWET_OperatorLogIn"),		&CMS896AApp::AWET_OperatorLogIn);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AWET_IntoRecoverScreen"),	&CMS896AApp::AWET_IntoRecoverScreen);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AWET_OperatorAskHelp"),		&CMS896AApp::AWET_OperatorAskHelp);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CompareFaultOperatorID"),	&CMS896AApp::CompareFaultOperatorID);  // v4.51D2 //Electech3E(DL) 6
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("CompareRecoverEngineerID"), &CMS896AApp::CompareRecoverEngineerID); 

		// For machine data
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMachineData"),			&CMS896AApp::SaveMachineData);
		// Additional Service Commands registration here
		m_comServer.IPC_REG_SERVICE_COMMAND("SC_CmdHelpScreenInfo",			&CMS896AApp::SC_CmdHelpScreenInfo);
		m_comServer.IPC_REG_SERVICE_COMMAND("ClearSortSetupFile",			&CMS896AApp::ClearSortSetupFile);
		// Command for Multi-Language Selections
		m_comServer.IPC_REG_SERVICE_COMMAND("MultiLangSelections",			&CMS896AApp::MultiLangSelections);
		m_comServer.IPC_REG_SERVICE_COMMAND("MultiLangSelectionsToggle",	&CMS896AApp::MultiLangSelectionsToggle);
		m_comServer.IPC_REG_SERVICE_COMMAND("SaveSelectedLanguage",			&CMS896AApp::SaveSelectedLanguage);
		// Machine Time Report Functions
		m_comServer.IPC_REG_SERVICE_COMMAND("SaveReportStartEndTime",		&CMS896AApp::SaveReportStartEndTime);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetMachineTimeReportPath",		&CMS896AApp::SetMachineTimeReportPath);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetMachineTimeReportPath2",	&CMS896AApp::SetMachineTimeReportPath2);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetTimePerformancePathCmd",	&CMS896AApp::SetTimePerformancePathCmd);

		m_comServer.IPC_REG_SERVICE_COMMAND("SetMachineReportPath",			&CMS896AApp::SetMachineReportPath);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetMachineReportPath2",		&CMS896AApp::SetMachineReportPath2);
		m_comServer.IPC_REG_SERVICE_COMMAND("SetAlarmLampStatusReportPath",	&CMS896AApp::SetAlarmLampStatusReportPath);

		m_comServer.IPC_REG_SERVICE_COMMAND("HmiPassword",					&CMS896AApp::HmiPassword);
		m_comServer.IPC_REG_SERVICE_COMMAND("CheckPasswordFile",			&CMS896AApp::CheckPasswordFile);
		m_comServer.IPC_REG_SERVICE_COMMAND("CheckUserLoginName",			&CMS896AApp::CheckUserLoginName);
		m_comServer.IPC_REG_SERVICE_COMMAND("CodeSanAnPasswordFile",		&CMS896AApp::CodeSanAnPasswordFile);
		m_comServer.IPC_REG_SERVICE_COMMAND("EncryptPasswordFile",			&CMS896AApp::EncryptPasswordFile);
		m_comServer.IPC_REG_SERVICE_COMMAND("DecryptPasswordFile",			&CMS896AApp::DecryptPasswordFile);
		m_comServer.IPC_REG_SERVICE_COMMAND("OperatorLogOnValidation",		&CMS896AApp::OperatorLogOnValidation);
		m_comServer.IPC_REG_SERVICE_COMMAND("ClearOperatorId",				&CMS896AApp::ClearOperatorId);

		m_comServer.IPC_REG_SERVICE_COMMAND("StopLoadUnloadLoopTest",		&CMS896AApp::StopLoadUnloadLoopTest);
		// For Lumileds only, make two choice for them when the new a lot
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SavePLLMProduct"),				&CMS896AApp::SavePLLMProduct);	
		m_comServer.IPC_REG_SERVICE_COMMAND("OutputFileFormatSelectInNewLot",	&CMS896AApp::OutputFileFormatSelectInNewLot);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SavePLLMDieFabMESLot"),			&CMS896AApp::SavePLLMDieFabMESLot);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetLoadPkgForNewWaferFrame"),	&CMS896AApp::SetLoadPkgForNewWaferFrame);
		m_comServer.IPC_REG_SERVICE_COMMAND("CheckSananPasswordFile_XMChangeLight",	&CMS896AApp::CheckSananPasswordFile_XMChangeLight);
		m_comServer.IPC_REG_SERVICE_COMMAND("CheckOpClearBinCounterAcMode",	&CMS896AApp::CheckOpClearBinCounterAcMode);
		m_comServer.IPC_REG_SERVICE_COMMAND("LoadBinBlockRunTimeData",		&CMS896AApp::LoadBinBlockRunTimeData);
		m_comServer.IPC_REG_SERVICE_COMMAND("SaveBinBlockRunTimeData",		&CMS896AApp::SaveBinBlockRunTimeData);
		m_comServer.IPC_REG_SERVICE_COMMAND("GetBinStoragePath",			&CMS896AApp::GetBinStoragePath);
		m_comServer.IPC_REG_SERVICE_COMMAND("HmiCornerSearchHomeDie",		&CMS896AApp::HmiCornerSearchHomeDie);
		m_comServer.IPC_REG_SERVICE_COMMAND("EnableKeyboard",				&CMS896AApp::EnableKeyboard);		//v4.08
		m_comServer.IPC_REG_SERVICE_COMMAND("SetWideScreenMode",			&CMS896AApp::SetWideScreenMode);		//v4.29
		m_comServer.IPC_REG_SERVICE_COMMAND("IsOPMenuEnabled",				&CMS896AApp::IsOPMenuEnabled);
		m_comServer.IPC_REG_SERVICE_COMMAND("IsUnderAutoCycle",				&CMS896AApp::IsUnderAutoCycle);
		m_comServer.IPC_REG_SERVICE_COMMAND("BurnInSetup",					&CMS896AApp::BurnInSetup);			//v4.29

		m_comServer.IPC_REG_SERVICE_COMMAND("CheckDiscUsage",				&CMS896AApp::CheckDiscUsage);		//v4.46T30

		m_comServer.IPC_REG_SERVICE_COMMAND("LoadPackageMsdData",			&CMS896AApp::LoadPackageMsdData);
		m_comServer.IPC_REG_SERVICE_COMMAND("SavePackageMsdData",			&CMS896AApp::SavePackageMsdData);
		m_comServer.IPC_REG_SERVICE_COMMAND("UpdatePackageList",			&CMS896AApp::UpdatePackageList);
		
		m_comServer.IPC_REG_SERVICE_COMMAND("SetAOTDefaultSetting",			&CMS896AApp::SetAOTDefaultSetting);
		m_comServer.IPC_REG_SERVICE_COMMAND("ZipErrorLog",					&CMS896AApp::ZipErrorLog);
			
		//NVRAM Export/Import fcn	//v4.11T1	
		m_comServer.IPC_REG_SERVICE_COMMAND("ExportNVRAMDataToFile",		&CMS896AApp::ExportNVRAMDataToFile);
		m_comServer.IPC_REG_SERVICE_COMMAND("ImportNVRAMDataFromFile",		&CMS896AApp::ImportNVRAMDataFromFile);
		//License Key Cmds
		m_comServer.IPC_REG_SERVICE_COMMAND("CreateMSLicenseFile",			&CMS896AApp::CreateMSLicenseFile);
		m_comServer.IPC_REG_SERVICE_COMMAND("RegisterMSLicenseFile",		&CMS896AApp::RegisterMSLicenseFile);

		m_comServer.IPC_REG_SERVICE_COMMAND("SetProRata",					&CMS896AApp::SetProRata);

		DisplayMessage("IPC Services registered ...");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//Register Hmi Variables
////////////////////////////////////////////////////////////////////////////////////
VOID CMS896AApp::RegisterVariables()
{
	try
	{
		CString szTemp;

		//Machine and Device File
		RegVariable(_T("MS_lHmiSecurityLevel"),			&m_lHmiAccessLevel);
		//RegVariable(_T("MS_szMachineFile"),			&m_szMachineFile);
		RegVariable(_T("MS_lDeviceFileType"),			&m_lDeviceFileType);
		RegVariable(_T("MS_ucDeviceFileType"),			&m_ucDeviceFileType);
		RegVariable(_T("MS_szDevicePathDisplay"),		&m_szDevicePathDisplay);
		RegVariable(_T("MS_szDevicePath"),				&m_szDevicePath);
		RegVariable(_T("MS_szDeviceFile"),				&m_szDeviceFile);
		RegVariable(_T("MS_szPKGPreviewPath"),			&m_szPKGPreviewPath);
		RegVariable(_T("MS_szPkgFileListPath"),			&m_szPkgFileListPath);
		RegVariable(_T("MS_szAmiFilePath"),				&m_szAmiFilePath);
		RegVariable(_T("MS_bAmiPitchUpdateBlock"),		&m_bAmiPitchUpdateBlock);
		RegVariable(_T("MS_szBinStoragePath"),			&m_szBinStoragePath);
		RegVariable(_T("MS_szBinStorageFile"),			&m_szBinStorageFilename);
		RegVariable(_T("MS_bDialogStorageFile"),		&m_bDialogStorageFile);
		//Map Start date & time
		RegVariable(_T("MS_szMapStartDate"),			&m_szMapStartDate);
		RegVariable(_T("MS_szMapStartTime"),			&m_szMapStartTime);
		//v4.48A4
		RegVariable(_T("MS_bRegisterLicense"),			&m_bIsMSLicenseRegistered);
		RegVariable(_T("MS_bValidateLicense"),			&m_bIsMSLicenseKeyChecked);
		RegVariable(_T("MS_bLicenseFcn1"),				&m_bMSLicenseKeyFcn1);
		RegVariable(_T("MS_bLicenseFcn2"),				&m_bMSLicenseKeyFcn2);
		RegVariable(_T("MS_bLicenseFcn3"),				&m_bMSLicenseKeyFcn3);
		RegVariable(_T("MS_bLicenseFcn4"),				&m_bMSLicenseKeyFcn4);
		RegVariable(_T("MS_bLicenseFcn5"),				&m_bMSLicenseKeyFcn5);
		//Lot Info
		RegVariable(_T("MS_szLotNumber"),				&m_szLotNumber);
		RegVariable(_T("MS_szLotNumberPrefix"),			&m_szLotNumberPrefix);
		//RegVariable(_T("MS_szLotStartTime"),			&m_szLotStartTime);
		//RegVariable(_T("MS_szLotEndTime"),			&m_szLotEndTime);
		RegVariable(_T("MS_szBinLotDirectory"),			&m_szBinLotDirectory);
		RegVariable(_T("MS_szAssociateFile"),			&m_szAssociateFile);
		RegVariable(_T("MS_szRecipeFilePath"),			&m_szRecipeFilePath);		//v4.42T3
		RegVariable(_T("MS_szShiftID"),					&m_szShiftID);				//v3.32T3	//Avago
		
		//Bin Info
		RegVariable(_T("MS_bNewLotStarted"),			&m_bNewLotStarted);
		RegVariable(_T("MS_nPLLMStartLot"),				&m_bStartNewLot);			//v4.16T5
		RegVariable(_T("MS_szBinWorkNo"),				&m_szBinWorkNo);
		RegVariable(_T("MS_szBinFileNamePrefix"),		&m_szBinFileNamePrefix);

		//Uplook PR Fcn Turn ON/OFF		//v4.58A3	//MS90
		RegVariable(_T("MS_bUseBHUplookPrFcn"),			&m_bUseBHUplookPrFcn);

		//Map Info
		RegVariable(_T("MS_szMapSubFolderName"),		&m_szMapSubFolderName);

		//Customer Flags
		//PLLM
		RegVariable(_T("MS_bUsePLLM"),					&m_bUsePLLM);				//v3.94
		RegVariable(_T("MS_bIsPLLMDieFab"),				&m_bIsPLLMDieFab);
		RegVariable(_T("MS_bIsPLLMLumiramic"),			&m_bIsPLLMLumiramic);
		RegVariable(_T("MS_bIsPLLMRebel"),				&m_bIsPLLMRebel);
		RegVariable(_T("MS_szPLLMDieFabMESLot"),		&m_szPLLMDieFabMESLot);
		//Nichia
		RegVariable(_T("MS_bNichia"),					&m_bIsNichia);
		//Electech3E
		RegVariable(_T("MS_bElectech3E"),				&m_bIsElectech3E);
		//Yealy
		RegVariable(_T("MS_bYealy"),					&m_bIsYealy);
		//Yealy
		RegVariable(_T("MS_bAvago"),					&m_bIsAvago);
		//v4.59A12	//Renesas
		RegVariable(_T("MS_bRenesas"),					&m_bIsRenesas);
			
		//Software/ Machine info 
		RegVariable(_T("MS_szOSImageReleaseNo"),		&m_szOSImageReleaseNo);
		RegVariable(_T("MS_szSoftVersion"),				&m_szSoftVersion);
		RegVariable(_T("MS_szMachineModel"),			&m_szMachineModel);
		RegVariable(_T("MS_szReleaseDate"),				&m_szSoftwareReleaseDate);
		RegVariable(_T("MS_szReleaseTime"),				&m_szSoftwareReleaseTime);
		RegVariable(_T("MS_szMachineName"),				&m_szMachineName);
		RegVariable(_T("MS_szMachineNo"),				&m_szMachineNo);
		RegVariable(_T("MS_szSWReleaseNo"),				&m_szSWReleaseNo);
		RegVariable(_T("MS_szHiPECVersion"),			&m_szHiPECVersion);
		RegVariable(_T("MS_szHiPECVersion2"),			&m_szHiPECVersion2);

		//Status message for Title section
		RegVariable(_T("MS_szStatusMessage"),			&m_szStatusMessage);

		//Measurement Unit for display
		RegVariable(_T("MS_szMeasurementUnit"),			&m_szMeasurementUnit);

		// Flag to indicate that the auto-cycle is starting
		RegVariable(_T("MS_bCycleStarted"),				&m_bCycleStarted);
		RegVariable(_T("MS_bCycleNotStarted"),			&m_bCycleNotStarted);

		// Flag to indicate that the auto-cycle is starting
		RegVariable(_T("MS_bCycleFKey"),				&m_bCycleFKey);

		// Flag to indicate short cut key status
		RegVariable(_T("MS_bShortCutKey"),				&m_bShortCutKey);
		RegVariable(_T("MS_bDisableStopButton"),		&m_bDisableStopButton);			//v4.13R1

		RegVariable(_T("MS_bGroupSetupAlarmAssitTimeToIdle"), &m_bGroupSetupAlarmAssitTimeToIdle);

		// Flag to indicate whether the device file is browsing via dialog box
		RegVariable(_T("MS_bDialogDeviceFile"),			&m_bDialogDeviceFile);

		// Flag to indicate whether the device path is fixed for operator
		RegVariable(_T("MS_bFixDevicePath"),			&m_bFixDevicePath);

		// Flag to load bin block or not
		RegVariable(_T("MS_bLoadPkgWithBin"),			&m_bLoadPkgWithBin);

		// Flag to only load bin parameters
		RegVariable(_T("MS_bOnlyLoadBinParam"),			&m_bOnlyLoadBinParam);
		RegVariable(_T("MS_bOnlyLoadWFTParam"),			&m_bOnlyLoadWFTParam);
		RegVariable(_T("MS_bOnlyLoadPRParam"),			&m_bOnlyLoadPRParam);

		// Flag to exclude machine parameters (WaferLoader, BinLoader, BondHead & Ejector  level settings)
		RegVariable(_T("MS_bExcludeMachineParam"),		&m_bExcludeMachineParam);

		RegVariable(_T("MS_bPortablePKGFile"),			&m_bPortablePKGFile);
		RegVariable(_T("MS_bRuntimeTwoPkgInUse"),		&m_bRuntimeTwoPkgInUse);

		RegVariable(_T("MS_bEnableAmiFile"),			&m_bEnableAmiFile);
		RegVariable(_T("MS_bEnableBarcodeLengthCheck"),	&m_bEnableBarcodeLengthCheck);
		RegVariable(_T("MS_bEnableInputCountSetupFile"), &m_bEnableInputCountSetupFile);
		

		// Flag to reset bin block serial no or not
		RegVariable(_T("MS_bResetBinSerialNo"),			&m_bResetBinSerialNo);
		RegVariable(_T("MS_bPpkgIncludeDelay"),				&m_bPpkgIncludeDelay);
		RegVariable(_T("MS_bPpkgIncludePrescanSettings"),	&m_bPpkgIncludePrescanSettings);
		RegVariable(_T("MS_bPpkgIncludeRegionSettings"),	&m_bPpkgIncludeRegionSettings);
		RegVariable(_T("MS_bPpkgIncludePickSettingWODelay"),&m_bPpkgIncludePickSettingWODelay);

		// Flag for user account
		RegVariable(_T("MS_bOperator"),					&m_bOperator);
		RegVariable(_T("MS_bTechnician"),				&m_bTechnician);			//v4.38T5	//PLLM Lumiramic
		RegVariable(_T("MS_bEngineer"),					&m_bEngineer);
		RegVariable(_T("MS_bAdministrator"),			&m_bAdministrator);
		RegVariable(_T("MS_szUsername"),				&m_szUsername);
		RegVariable(_T("MS_szProductID"),				&m_szProductID);
		RegVariable(_T("MS_szEngineerPassword"),		&m_szEngineerPassword);
		RegVariable(_T("MS_szTechnicianPassword"),		&m_szTechnicianPassword);
		RegVariable(_T("MS_szAdminPassword"),			&m_szAdminPassword);
		RegVariable(_T("MS_szPLLMRebelPassword"),		&m_szPLLMRebelPassword);	//v3.70T3
		RegVariable(_T("MS_szPasswordFilePath"),		&m_szPasswordFilePath);		//v4.17T7
		RegVariable(_T("MS_szUserLogOn"),				&m_szUserLogOn);

		// Flag for Application features
		RegVariable(_T("MS_bEnablePreBond"),			&m_bEnablePreBond);			//v3.80
		RegVariable(_T("MS_bEnablePostBond"),			&m_bEnablePostBond);
		RegVariable(_T("MS_bEnableRefDie"),				&m_bEnableRefDie);
		RegVariable(_T("MS_bEnableCharRefDie"),			&m_bEnableCharRefDie);
		RegVariable(_T("MS_bEnableSCNFile"),			&m_bEnableSCNFile);
		RegVariable(_T("MS_bEnableBlockFunc"),			&m_bEnableBlockFunc);
		RegVariable(_T("MS_bEnableSepGrade"),			&m_bEnableSepGrade);
		RegVariable(_T("MS_bEnableEmptyCheck"),			&m_bEnableEmptyCheck);		//Block2
		RegVariable(_T("MS_bEnableWPRBackLight"),		&m_bEnableWPRBackLight);	//v4.46T28

		RegVariable(_T("MS_bEnableOPMenu"),				&m_bEnableOPMenuShortcut);	//v4.19
		RegVariable(_T("MS_bEnableBHShortcut"),			&m_bEnableBHShortcut);
		RegVariable(_T("MS_bEnableWPRShortcut"),		&m_bEnableWPRShortcut);
		RegVariable(_T("MS_bEnableBPRShortcut"),		&m_bEnableBPRShortcut);
		RegVariable(_T("MS_bEnableBinBlkShortcut"),		&m_bEnableBinBlkShortcut);
		RegVariable(_T("MS_bEnableWTShortcut"),			&m_bEnableWTShortcut);
		RegVariable(_T("MS_bEnablePRRecordShortcut"),	&m_bEnablePRRecordShortcut);
		RegVariable(_T("MS_bEnableBinTableShortcut"),	&m_bEnableBinTableShortcut);
		RegVariable(_T("MS_bDisablePKGShortcut"),		&m_bDisablePKGShortcut);
		RegVariable(_T("MS_bEnablePicknPlaceShortcut"),	&m_bEnablePicknPlaceShortcut);		//v4.20	//SanAn
		RegVariable(_T("MS_bDisableOptionsShortcut"),	&m_bDisableOptionsShortcut);		//v4.20	//SanAn
		RegVariable(_T("MS_bEnableOptionsTabShortcut"),	&m_bEnableOptionsTabShortcut);		//v4.20	//SanAn
		RegVariable(_T("MS_bSystemServiceShrotcut"),	&m_bEnableSystemServiceShortcut);
		RegVariable(_T("MS_bEnableStepMoveShortcut"),	&m_bEnableStepMoveShortcut);
		RegVariable(_T("MS_bEnablePhyBlockShortcut"),	&m_bEnablePhyBlockShortcut);
		RegVariable(_T("MS_bEnableOperationModeShortcut"),	&m_bEnableOperationModeShortcut);
		RegVariable(_T("MS_bToggleWaferResortScanMode"),	&m_bToggleWaferResortScanMode);
		RegVariable(_T("MS_bEnableWaferToBinResort"),		&m_bEnableWaferToBinResort);
		
		RegVariable(_T("MS_bEnableSECSComm"),			&m_bEnableSECSComm);
		RegVariable(_T("MS_bEnableTCPComm"),			&m_bEnableTCPComm);
		RegVariable(_T("MS_bEnableBinWorkNo"),			&m_bEnableBinWorkNo);
		RegVariable(_T("MS_bEnableFileNamePrefix"),		&m_bEnableFileNamePrefix);
		RegVariable(_T("MS_bEnablePolygonDie"),			&m_bEnablePolygonDie);
		RegVariable(_T("MS_bChangeBinLotNo"),			&m_bChangeBinLotNo);
		RegVariable(_T("MS_bGenBinLotDirectory"),		&m_bGenBinLotDirectory);
		RegVariable(_T("MS_bEnableChineseMenu"),		&m_bEnableChineseMenu);
		RegVariable(_T("MS_bDisableBinSNRFormat"),		&m_bDisableBinSNRFormat);
		RegVariable(_T("MS_bDisableClearCountFormat"),	&m_bDisableClearCountFormat);	
		RegVariable(_T("MS_bEnablePRDualColor"),		&m_bEnablePRDualColor);
		RegVariable(_T("MS_bEnablePRAdaptWafer"),		&m_bEnablePRAdaptWafer);
		RegVariable(_T("MS_bPrCircleDetection"),		&m_bPrCircleDetection);		//v2.78T2
		RegVariable(_T("MS_bPrAutoLearnRefDie"),		&m_bPrAutoLearnRefDie);		//v2.78T2
		RegVariable(_T("MS_bWtCheckMasterPitch"),		&m_bWtCheckMasterPitch);	//v2.78T2
		RegVariable(_T("MS_bPrIMInterface"),			&m_bPrIMInterface);			//v2.82T4
		RegVariable(_T("MS_bPrLineInspection"),			&m_bPrLineInspection);		//v3.17T1
		//RegVariable(_T("MS_bNewCtEnhancement"),		&m_bNewCtEnhancement);		//v2.83T2	//v4.50A2
		RegVariable(_T("MS_bUseNewStepperEnc"),			&m_bUseNewStepperEnc);		//v2.83T2
		RegVariable(_T("MS_bEnable1stDieFinder"),		&m_bEnable1stDieFinder);	//v3.15T5
		RegVariable(_T("MS_bUseBinMultiMgznSnrs"),		&m_bUseBinMultiMgznSnrs);	//v3.30T2
		RegVariable(_T("MS_bUsePLLMSpecialFcn"),		&m_bUsePLLMSpecialFcn);		//v3.70T2

		RegVariable(_T("MS_bEnablePrPostSealOptics"),	&m_bEnablePrPostSealOptics);	//xyz
		RegVariable(_T("MS_bEnableBHPostSealOptics"),	&m_bEnableBHPostSealOptics);	//v4.39T11
		RegVariable(_T("MS_bEnableESContourCamera"),	&m_bEnableESContourCamera);
		RegVariable(_T("MS_bEnableDualPathCamera"),		&m_bEnableDualPathCamera);
		RegVariable(_T("MS_bEnableBHUplookPrFcn"),		&m_bEnableBHUplookPrFcn);		// 4.52D17

		RegVariable(_T("MS_bOfflinePostbondTest"),		&m_bOfflinePostbondTest);	//v4.11T3	//Lumileds Lumiramic
		RegVariable(_T("MS_ulPLLMProduct"),				&m_ulPLLMProduct);				//xyz
		RegVariable(_T("MS_bLoadPkgForNewWaferFrame"),	&m_bLoadPkgForNewWaferFrame);	

		//RegVariable(_T("MS_bEnableMachineReport"),		&m_bEnableMachineReport);
		RegVariable(_T("MS_bEnableReportPathSettingInterface"),	&m_bEnableReportPathSettingInterface);

		RegVariable(_T("MS_bEnablePkgFileList"),		&m_bEnablePkgFileList);		//v4.21T7

		RegVariable(_T("MS_bPackageFileMapHeaderCheckFunc"), &m_bPackageFileMapHeaderCheckFunc);	//v4.16T3
		
		RegVariable(_T("MS_bWafflePadBondDie"),			&m_bWafflePadBondDie);
		RegVariable(_T("MS_bEnableBinMap"),				&m_bEnableBinMap);			//v4.03	//PLLM/PLSG REBEL 
		RegVariable(_T("MS_bMultiGradeSortToSingleBin"), &m_bMultiGradeSortToSingleBin);
		RegVariable(_T("MS_bEnableCMLTReworkFcn"),		&m_bEnableCMLTReworkFcn);	//v4.15T9
		RegVariable(_T("MS_bEnableOsramResortMode"),	&m_bEnableOsramResortMode);	//v4.21T3
		RegVariable(_T("MS_bEnableMultiCOROffset"),		&m_bEnableMultiCOROffset);	//v4.24T8
		
// prescan relative code
		RegVariable(_T("MS_bEnablePrescanInterface"),	&m_bEnablePrescanInterface);
		RegVariable(_T("MS_bEnableWpr2Lighting"),		&m_bPrEnableWpr2Lighting);	//v3.44T1
		RegVariable(_T("MS_bEnable2DBarcode"),			&m_bEnable2DBarCode);		//v3.60		//Cree China

		RegVariable(_T("MS_bEnable2nd1DBarcode"),		&m_bEnable2nd1DBarCode);		
		RegVariable(_T("MS_bColletCleanPocket"),		&m_bEnableColletCleanPocket);//v3.65	//Lexter
		RegVariable(_T("MS_bEnableWaferClamp"),			&m_bEnableWaferClamp);		//v4.26T1	//Walsin China
		RegVariable(_T("MS_bBinFrameStatusSummary"),	&m_bBinFrameStatusSummary);
		RegVariable(_T("MS_bSupportPortablePKGFile"),	&m_bSupportPortablePKGFile);
		RegVariable(_T("MS_bPortablePKGInfoPage"),		&m_bPortablePKGInfoPage);
		RegVariable(_T("MS_bEnableSummaryPage"),		&m_bEnableSummaryPage);
		RegVariable(_T("MS_bEnableBinBlkTemplate"),			&m_bEnableBinBlkTemplate);
		RegVariable(_T("MS_bEnableAutoLoadNamingFile"),		&m_bEnableAutoLoadNamingFile);
		RegVariable(_T("MS_bEnableLoadRankIDFromMap"),		&m_bEnableLoadRankIDFromMap);
		RegVariable(_T("MS_bEnableOptimizeBinCountFunc"),	&m_bEnableOptimizeBinCountFunc);
		RegVariable(_T("MS_bEnableRestoreBinRunTimeData"),	&m_bEnableRestoreBinRunTimeData);
		RegVariable(_T("MS_bOTraceabilityLot"),				&m_bOutputFormatSelInNewLot);	//v3.68T3	//PLLM only
		RegVariable(_T("MS_bEnableBTPt5DiePitch"),			&m_bEnableBTPt5DiePitch);	
		RegVariable(_T("MS_bEnableBTBondAreaOffset"),		&m_bEnableBTBondAreaOffset);
		RegVariable(_T("MS_bEnablePrAutoEnlargeSrchWnd"),	&m_bEnablePrAutoEnlargeSrchWnd);
		RegVariable(_T("MS_bEnableWaferSizeSelect"),		&m_bEnableWaferSizeSelect);
		RegVariable(_T("MS_bEnableLaserTestOutputPortTrigger"),		&m_bEnableLaserTestOutputPortTrigger);
		RegVariable(_T("MS_bEnableGripperLoadUnloadTest"),			&m_bEnableGripperLoadUnloadTest);
		RegVariable(_T("MS_bEnableNGPick"),					&m_bEnableNGPick);				//v4.51A19	//Silan MS90
		RegVariable(_T("MS_bUseColletOffsetWoEjtXY"),		&m_bUseColletOffsetWoEjtXY);	//v4.52A14
		//	AWET
		RegVariable(_T("MS_bAWET_Enabled"),					&m_bAWET_Enabled);
		RegVariable(_T("MS_bAWET_Triggered"),				&m_bAWET_Triggered);
		RegVariable(_T("MS_szAWET_AlarmCode"),				&m_szAWET_AlarmCode);
		RegVariable(_T("MS_bAWET_CtrlEngContainer"),		&m_bAWET_CtrlEngContainer); 
		RegVariable(_T("MS_bAWET_CtrlOpContainer"),			&m_bAWET_CtrlOpContainer); 
		RegVariable(_T("MS_bAWET_CtrlExitButton"),			&m_bAWET_CtrlExitButton); 

		// Flag for Stop alignment
		RegVariable(_T("MS_bStopAlign"), &m_bStopAlign);

		// Flag for HMI control container accees 
		RegVariable(_T("MS_bAcBHMotorSetup"),			&m_bAcBHMotorSetup);
		RegVariable(_T("MS_bAcBondArmSetup"),			&m_bAcBondArmSetup);
		RegVariable(_T("MS_bAcBondHeadSetup"),			&m_bAcBondHeadSetup);
		RegVariable(_T("MS_bAcDelaySetup"),				&m_bAcDelaySetup);
		RegVariable(_T("MS_bAcEjectorSetup"),			&m_bAcEjectorSetup);
		RegVariable(_T("MS_bAcBinBlkEditSetup"),		&m_bAcBinBlkEditSetup);
		RegVariable(_T("MS_bAcBinBlkClearSetup"),		&m_bAcBinBlkClearSetup);
		RegVariable(_T("MS_bAcClearBinFileSettings"),	&m_bAcClearBinFileSettings);
		RegVariable(_T("MS_bAcClearBinCounter"),		&m_bAcClearBinCounter);
		RegVariable(_T("MS_bAcNoGenOutputFile"),		&m_bAcNoGenOutputFile);

		RegVariable(_T("MS_bAcWLLoaderSetup"),			&m_bAcWLLoaderSetup);
		RegVariable(_T("MS_bAcWLExpanderSetup"),		&m_bAcWLExpanderSetup);
		RegVariable(_T("MS_bAcWLAlignmentSetup"),		&m_bAcWLAlignmentSetup);
		RegVariable(_T("MS_bAcWLOthersSetup"),			&m_bAcWLOthersSetup);
		RegVariable(_T("MS_bAcWLMotorSetup"),			&m_bAcWLMotorSetup);
		RegVariable(_T("MS_bAcBLGripperSetup"),			&m_bAcBLGripperSetup);
		RegVariable(_T("MS_bAcBLMagazineSetup"),		&m_bAcBLMagazineSetup);
		RegVariable(_T("MS_bAcBLOthersSetup"),			&m_bAcBLOthersSetup);
		RegVariable(_T("MS_bAcBLOthersManualOp"),		&m_bAcBLOthersManualOp);
		RegVariable(_T("MS_bAcBLMotorSetup"),			&m_bAcBLMotorSetup);

		RegVariable(_T("MS_bAcMapPathSetting"),			&m_bAcMapPathSetting);
		RegVariable(_T("MS_bAcAlignWafer"),				&m_bAcAlignWafer);
		RegVariable(_T("MS_bAcDisableSCNSettings"),		&m_bAcDisableSCNSettings);
		RegVariable(_T("MS_bAcDisableManualAlign"),		&m_bAcDisableManualAlign);
		
		RegVariable(_T("MS_bAcMapSetting"),				&m_bAcMapSetting);
		RegVariable(_T("MS_bAcMapOptions"),				&m_bAcMapOptions);
		RegVariable(_T("MS_bAcLoadMap"),				&m_bAcLoadMap);
		RegVariable(_T("MS_bAcClearMap"),				&m_bAcClearMap);
		
		RegVariable(_T("MS_bAcWaferTableSetup"),		&m_bAcWaferTableSetup);
		RegVariable(_T("MS_bAcCollectnEjSetup"),		&m_bAcCollectnEjSetup);
		
		RegVariable(_T("MS_bAcDieCheckOptions"),		&m_bAcDieCheckOptions);
		RegVariable(_T("MS_bAcWaferEndOptions"),		&m_bAcWaferEndOptions);
		
		RegVariable(_T("MS_bAcWaferPrAdvSetup"),		&m_bAcWaferPrAdvSetup);
		RegVariable(_T("MS_bAcWaferPrLrnRefProtect"),	&m_bAcWaferPrLrnRefProtect);
		RegVariable(_T("MS_bAcWaferPrLrnAlgrProtect"),	&m_bAcWaferPrLrnAlgrProtect);

		RegVariable(_T("MS_bAcBondPrAdvSetup"),			&m_bAcBondPrAdvSetup);
		RegVariable(_T("MS_bAcBondPrLrnRefProtect"),	&m_bAcBondPrLrnRefProtect);

		RegVariable(_T("MS_bAcBinTableLimit"),			&m_bAcBinTableLimit);
		RegVariable(_T("MS_bAcBinTableColletOffset"),	&m_bAcBinTableColletOffset);

		RegVariable(_T("MS_bAcPKGFileSettings"),		&m_bAcPKGFileSettings);

		RegVariable(_T("MS_bAcWaferLotSettings"),		&m_bAcWaferLotSettings);

		// Flag for HMI Display accees status
		RegVariable(_T("MS_bInBondPage"),				&m_bInBondPage);   // 4.51D1 Secs
		RegVariable(_T("MS_bAsBHMotorSetup"),			&m_bAsBHMotorSetup);
		RegVariable(_T("MS_bAsBondArmSetup"),			&m_bAsBondArmSetup);
		RegVariable(_T("MS_bAsBondHeadSetup"),			&m_bAsBondHeadSetup);
		RegVariable(_T("MS_bAsDelaySetup"),				&m_bAsDelaySetup);
		RegVariable(_T("MS_bAsEjectorSetup"),			&m_bAsEjectorSetup);
		RegVariable(_T("MS_bAsBinBlkEditSetup"),		&m_bAsBinBlkEditSetup);
		RegVariable(_T("MS_bAsBinBlkClearSetup"),		&m_bAsBinBlkClearSetup);
		RegVariable(_T("MS_bAsClearBinFileSettings"),	&m_bAsClearBinFileSettings);
		RegVariable(_T("MS_bAsClearBinCounter"),		&m_bAsClearBinCounter);
		RegVariable(_T("MS_bAsNoGenOutputFile"),		&m_bAsNoGenOutputFile);

		RegVariable(_T("MS_bAsWLLoaderSetup"),			&m_bAsWLLoaderSetup);
		RegVariable(_T("MS_bAsWLExpanderSetup"),		&m_bAsWLExpanderSetup);
		RegVariable(_T("MS_bAsWLAlignmentSetup"),		&m_bAsWLAlignmentSetup);
		RegVariable(_T("MS_bAsWLOthersSetup"),			&m_bAsWLOthersSetup);
		RegVariable(_T("MS_bAsWLMotorSetup"),			&m_bAsWLMotorSetup);
		RegVariable(_T("MS_bAsBLGripperSetup"),			&m_bAsBLGripperSetup);
		RegVariable(_T("MS_bAsBLMagazineSetup"),		&m_bAsBLMagazineSetup);
		RegVariable(_T("MS_bAsBLOthersSetup"),			&m_bAsBLOthersSetup);
		RegVariable(_T("MS_bAsBLOthersManualOp"),		&m_bAsBLOthersManualOp);
		RegVariable(_T("MS_bAsBLMotorSetup"),			&m_bAsBLMotorSetup);

		RegVariable(_T("MS_bAsMapPathSetting"),			&m_bAsMapPathSetting);
		RegVariable(_T("MS_bAsAlignWafer"),				&m_bAsAlignWafer);
		RegVariable(_T("MS_bAsDisableSCNSettings"),		&m_bAsDisableSCNSettings);
		RegVariable(_T("MS_bAsDisableManualAlign"),		&m_bAsDisableManualAlign);
		
		RegVariable(_T("MS_bAsMapSetting"),				&m_bAsMapSetting);
		RegVariable(_T("MS_bAsMapOptions"),				&m_bAsMapOptions);
		RegVariable(_T("MS_bAsLoadMap"),				&m_bAsLoadMap);
		RegVariable(_T("MS_bAsClearMap"),				&m_bAsClearMap);
		RegVariable(_T("MS_bAsWaferTableSetup"),		&m_bAsWaferTableSetup);
		RegVariable(_T("MS_bAsCollectnEjSetup"),		&m_bAsCollectnEjSetup);
		RegVariable(_T("MS_bAsWaferMapSetup"),		&m_bAsWaferMapSetup);
		RegVariable(_T("MS_bAcWaferMapSetup"),		&m_bAcWaferMapSetup);

		RegVariable(_T("MS_bAsDieCheckOptions"),		&m_bAsDieCheckOptions);
		RegVariable(_T("MS_bAsWaferEndOptions"),		&m_bAsWaferEndOptions);	
		RegVariable(_T("MS_bAsWaferPrAdvSetup"),		&m_bAsWaferPrAdvSetup);
		RegVariable(_T("MS_bAsWaferPrLrnRefProtect"),	&m_bAsWaferPrLrnRefProtect);
		RegVariable(_T("MS_bAsWaferPrLrnAlgrProtect"),	&m_bAsWaferPrLrnAlgrProtect);
		RegVariable(_T("MS_bAsBondPrAdvSetup"),			&m_bAsBondPrAdvSetup);
		RegVariable(_T("MS_bAsBondPrLrnRefProtect"),	&m_bAsBondPrLrnRefProtect);
		RegVariable(_T("MS_bAsBinTableLimit"),			&m_bAsBinTableLimit);
		RegVariable(_T("MS_bAsBinTableColletOffset"),	&m_bAsBinTableColletOffset);		
		RegVariable(_T("MS_bAsPKGFileSettings"),		&m_bAsPKGFileSettings);
		RegVariable(_T("MS_bAsWaferLotSettings"),		&m_bAsWaferLotSettings); 
		RegVariable(_T("MS_bEnableCommOption"),			&m_bEnableCommOption);		// Flag for HMI display Communication Option
		RegVariable(_T("MS_bEnableBondTaskBar"),		&m_bEnableBondTaskBar);		//v4.33T5	//SanAn
		RegVariable(_T("MS_bEnableScanTaskBar"),		&m_bEnableScanTaskBar);		//v4.33T5	//SanAn
		RegVariable(_T("MS_bWLBarcodeSelection"),		&m_bWLBarcodeSelection);	//v4.38T2

		// Flag for TCP/IP (Host) Communication parameter
		RegVariable(_T("MS_szHostAddress"),				&m_szHostAddress);
		RegVariable(_T("MS_szHostLotNo"),				&m_szHostLotNo);
		RegVariable(_T("MS_ulHostPortNo"),				&m_ulHostPortNo);
		RegVariable(_T("MS_szHostMachineID"),			&m_szHostMachineID);
		RegVariable(_T("MS_szHostCommandID"),			&m_szHostCommandID);
		RegVariable(_T("MS_bHostAutoConnect"),			&m_bHostAutoConnect);
		RegVariable(_T("MS_bHostCommEnabled"),			&m_bHostCommEnabled);
		RegVariable(_T("MS_bHostCommConnected"),		&m_bHostCommConnected);
		RegVariable(_T("MS_bHostAutoReConnect"),		&m_bHostAutoReConnect);
		RegVariable(_T("MS_bHostReConnectting"),		&m_bHostReConnectting);
		RegVariable(_T("MS_szHostCommStatus"),			&m_szHostCommStatus);

		// For machine time
		RegVariable(_T("MS_szMachineBondedUnit"),		&m_szMachineBondedUnit);
		RegVariable(_T("MS_szMachineUpTime"),			&m_szMachineUpTime);
		RegVariable(_T("MS_szMachineRunTime"),			&m_szMachineRunTime);
		RegVariable(_T("MS_szMachineDownTime"),			&m_szMachineDownTime);
		RegVariable(_T("MS_szMachineAlarmTime"),		&m_szMachineAlarmTime);
		RegVariable(_T("MS_szMachineAssistTime"),		&m_szMachineAssistTime);
		RegVariable(_T("MS_szMachineSetupTime"),		&m_szMachineSetupTime);
		RegVariable(_T("MS_szMachineIdleTime"),			&m_szMachineIdleTime);
		RegVariable(_T("MS_szMachineAlarmCount"),		&m_szMachineAlarmCount);
		RegVariable(_T("MS_szMachineAssistCount"),		&m_szMachineAssistCount);
		RegVariable(_T("MS_ucEqTimeReportMode"),		&m_ucEqTimeReportMode);
		RegVariable(_T("MS_bEqTimeAutoReset"),			&m_bEqTimeAutoReset);
		RegVariable(_T("MS_bEqTimeAutoResetDieCounts"), &m_bEqTimeAutoResetDieCounts);	//v2.83T2
		RegVariable(_T("MS_lNoOfReportPeriod"),			&m_lNoOfReportPeriod);
		RegVariable(_T("MS_szMachineTimeReportPath"),	&m_szMachineTimeReportPath);
		RegVariable(_T("MS_szMachineTimeReportPath2"),	&m_szMachineTimeReportPath2);
		RegVariable(_T("MS_szTimePerformancePath"),		&m_szTimePerformancePath);
		RegVariable(_T("MS_szMachineSerialNo"),			&m_szMachineSerialNo);

		RegVariable(_T("MS_szMachineWaitingSetupTime"),	&m_szMachineWaitingSetupTime);  // v4.51D5 SanAn(XA) 7
		RegVariable(_T("MS_szWaitingSetTimeAlarmMsg"),	&m_szWaitingSetTimeAlarmMsg);
		RegVariable(_T("MS_bWaitingSetTimeClosed"),		&m_bWaitingSetTimeClosed);		
		RegVariable(_T("MS_bWaitingSetTimeContianer"),	&m_bWaitingSetTimeContainer);

		RegVariable(_T("MS_szMachineReportPath"),		&m_szMachineReportPath);
		RegVariable(_T("MS_szMachineReportPath2"),		&m_szMachineReportPath2);
		
		RegVariable(_T("MS_szRecoverEnginnerID"),		&m_szRecoverEngineerID); // v4.51D2 //Electech3E(DL) 7
		RegVariable(_T("MS_szRecoverEnginnerPassword"),	&m_szRecoverEngineerPassword); 
		RegVariable(_T("MS_szRecoverPasswordPath"),		&m_szRecoverPasswordPath);
		RegVariable(_T("MS_szRecoverGeneratedFilePath"),&m_szRecoverGeneratedFilePath);
		RegVariable(_T("MS_szFaultOperatorID"),			&m_szFaultOperatorID);
		RegVariable(_T("MS_szFaultOperatorPassword"),	&m_szFaultOperatorPassword); 
		RegVariable(_T("MS_szFaultFilePath"),			&m_szFaultFilePath); 
		RegVariable(_T("MS_bFaultButton"),			    &m_bFaultButton); 
		RegVariable(_T("MS_bFaultContainer"),			&m_bFaultContainer); 
		RegVariable(_T("MS_bFaultButtonColor"),			&m_bFaultButtonColor); 
		RegVariable(_T("MS_bRecoverButton"),			&m_bRecoverButton); 
		RegVariable(_T("MS_bRecoverContainer"),			&m_bRecoverContainer); 
		RegVariable(_T("MS_bRecoverGoBackScreen"),		&m_bRecoverGoBackScreen); 
		RegVariable(_T("MS_bRecoverFinished"),			&m_bRecoverFinished); 
		
		RegVariable(_T("MS_lBondTabPageSelect"),		&m_lBondTabPageSelect); 
		
		RegVariable(_T("MS_dPreviewImageSizeX"),		&m_dPreviewImageSizeX);
		RegVariable(_T("MS_dPreviewImageSizeY"),		&m_dPreviewImageSizeY);

		// Flag to enable machine logging
		RegVariable(_T("MS_bEnableMachineLog"),			&m_bEnableMachineLog);
		RegVariable(_T("MS_bEnableWtMachineLog"),		&m_bEnableWtMachineLog);	//v4.06
		RegVariable(_T("MS_bEnableTableIndexLog"),		&m_bEnableTableIndexLog);	//v4.59A12
		RegVariable(_T("MS_bEnableAutoRecoverPR"),		&m_bEnableAutoRecoverPR);

		// For Motorized Zoom
		RegVariable(_T("MS_bEnableMotorizedZoom"),		&m_bEnableMotorizedZoom);

		// Multi-Language Selection
		RegVariable(_T("MS_lLangSelected"),				&m_lLangSelected);
		RegVariable(_T("MS_szMultiLanguage"),			&m_szMultiLanguage); // For HMI Only

		// Clear By Grade
		RegVariable(_T("MS_bUseClearByGrade"),			&m_bUseClearBinByGrade);
		
		RegVariable(_T("MS_bAutoGenBinBlkCountSummary"), &m_bAutoGenBinBlkCountSummary);
		RegVariable(_T("MS_bEnableMultipleMapHeaderPage"), &m_bEnableMultipleMapHeaderPage);

		RegVariable(_T("MS_bEnableManualUploadBinSummary"), &m_bEnableManualUploadBinSummary);
		
		RegVariable(_T("MS_bEnableExtraClearBinInfo"),	&m_bEnableExtraClearBinInfo);

		RegVariable(_T("MS_bEnableAssocFile"),			&m_bEnableAssocFile);

		// CP100 new screen
		RegVariable(_T("MS_bIsCP100NewScreen"),			&m_bIsCP100NewScreen);
		RegVariable(_T("MS_bIsRedTowerSignal"),			&m_bIsRedTowerSignal);
		RegVariable(_T("MS_bIsYellowTowerSignal"),		&m_bIsYellowTowerSignal);
		RegVariable(_T("MS_bIsGreenTowerSignal"),		&m_bIsGreenTowerSignal);

		// Connection Type like: MES...
		RegVariable(_T("MS_bOthersConnectionType"),		&m_bOthersConnectionType);
		RegVariable(_T("MS_bChangeMaxCountWithAmi"),	&m_bChangeMaxCountWithAmi);
		for (INT i = 0; i < EQUIP_NO_OF_RECORD_TIME ; i++)
		{
			szTemp.Format("MS_szReportStartTime%d", i + 1);
			RegVariable(szTemp, &m_szReportStartTime[i]);

			szTemp.Format("MS_szReportEndTime%d", i + 1);
			RegVariable(szTemp, &m_szReportEndTime[i]);
		}

		RegVariable(_T("MS_bEnableAutoMapDieTypeCheckFunc"), &m_bEnableAutoMapDieTypeCheckFunc);
		
		for (INT i = 0; i < MS896A_DIE_TYPE_CHECK_LIMIT; i++)
		{
			szTemp.Format("MS_szMapDieTypeCheckString%d", i + 1);
			RegVariable(szTemp, &m_szMapDieTypeCheckString[i]);
		}

		RegVariable(_T("MS_bOnBondMode"),				&m_bOnBondMode);
		RegVariable(_T("MS_bIsKeyboardLocked"),			&m_bIsKeyboardLocked);		//v4.08
		// disable module
		RegVariable(_T("MS_bDisableBHModule"),			&m_bDisableBHModule);
		RegVariable(_T("MS_bDisableBTModule"),			&m_bDisableBTModule);
		RegVariable(_T("MS_bDisableBLModule"),			&m_bDisableBLModule);
		RegVariable(_T("MS_bDisableNLModule"),			&m_bDisableNLModule);		//andrewng //2020-0708
		// Machine options variable
		RegVariable(_T("MS_bMS100Plus"),				&m_bMS100Plus);				//v4.35T1
		RegVariable(_T("MS_bMS100Plus9InchOption"),		&m_bMS100Plus9InchOption);	//v4.16T3
		RegVariable(_T("MS_bES100DualWFTOption"),		&m_bES100v2DualWftOption);	//v4.24T4
		RegVariable(_T("MS_bESAoiDualWFT"),				&m_bESAoiDualWT);
		RegVariable(_T("MS_bESAoiBackLight"),			&m_bESAoiBackLight);

		RegVariable(_T("MS_bMS50"),						&m_bMS50);					//v4.59A43
		RegVariable(_T("MS_bMS60"),						&m_bMS60);					//v4.49A5
		RegVariable(_T("MS_bMS90"),						&m_bMS90);					//v4.49A9
		RegVariable(_T("MS_bAutoLineMode"),				&m_bMSAutoLineMode);
		RegVariable(_T("MS_bSISMode_HMI"),				&m_bMS_SISMode_HMI);
		RegVariable(_T("MS_bSISStandaloneMode_HMI"),	&m_bMS_SISStandaloneMode_HMI);
		RegVariable(_T("MS_bStandByButtonPressed_HMI"),	&m_bMS_StandByButtonPressed_HMI);

		RegVariable(_T("MS_bBinFrameHaveRefCross"),		&m_bBinFrameHaveRefCross);
		RegVariable(_T("MS_bEnableClearBinCopyTempFile"),&m_bEnableClearBinCopyTempFile);
		RegVariable(_T("MS_bEnableBondHeadTolerance"),	&m_bEnableBondHeadTolerance);
		// Check Disk size
		RegVariable(_T("MS_cDiskLabel"),				&m_cDiskLabel);
		RegVariable(_T("MS_dWarningEmptySpacePercent"),	&m_dWarningEmptySpacePercent);
		RegVariable(_T("MS_bEnableSubBin"),				&m_bEnableSubBin);
		RegVariable(_T("MS_bNGWithBinMap"),				&m_bNGWithBinMap);

		RegVariable(_T("MS_bBurnInGrabImage"),			&m_bBurnInGrabImage);

		//AlarmReportPath
		RegVariable(_T("MS_szAlarmLampStatusReportPath"),&m_szAlarmLampStatusReportPath); //1.08S

		DisplayMessage("Hmi Variables Registered...");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}

/////////////////////////////////////////////////////////////////
//	Service Command Functions
/////////////////////////////////////////////////////////////////
LONG CMS896AApp::SrvInitialize(IPC_CServiceMessage &svMsg)
{
	SMotCommand	smCommand;
	CString		szDisplay;
	BOOL		bResult = FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);
		smCommand.bReply = smCommand.lAction = smCommand.lAxis = smCommand.lUnit = 0;
		// TODO: Process the received command and perform the initialization
		//bResult = SetRun();
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	
	return 1;
}

LONG CMS896AApp::SrvSystemInitialize(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	CString			szDisplay;
	SFM_CStation	*pStation;
	CAppStation	*pAppStation;
	POSITION		pos;
	CString			szKey;
	BOOL			bResult = FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(smCommand), &smCommand);

		szDisplay.Format(_T("%s - Receive System Initialize command: %d"),
						 m_szModuleName, smCommand.lAction);
		DisplayMessage(szDisplay);

		if (m_qState == IDLE_Q)
		{
			m_lAction = smCommand.lAction;
			m_nOperation = NULL_OP;

			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);
				pAppStation = dynamic_cast<CAppStation*>(pStation);

				pAppStation->Motion(TRUE);
				pAppStation->Result(gnOK);
				pAppStation->SetAxisAction(smCommand.lAxis,
										   smCommand.lAction,
										   smCommand.lUnit);

				pAppStation->Command(glINITIAL_COMMAND);
			}

			m_qState = SYSTEM_INITIAL_Q;
			bResult = TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - Initialize command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	
	return 1;
}

LONG CMS896AApp::SrvDiagCommand(IPC_CServiceMessage &svMsg)
{
	struct
	{
		LONG			lStation;
		SMotCommand		smCommand;
	} smDiagCommand;

	CString	szDisplay;
	BOOL	bResult = FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(smDiagCommand), &smDiagCommand);

		szDisplay.Format(_T("%s - Receive Diagnostic command"), m_szModuleName);
		DisplayMessage(szDisplay);

		szDisplay.Format(_T("   Station: %d, Axis: %d, Action: %d, Unit: %d"),
						 smDiagCommand.lStation, smDiagCommand.smCommand.lAxis,
						 smDiagCommand.smCommand.lAction, smDiagCommand.smCommand.lUnit);
		DisplayMessage(szDisplay);

		if (m_qState == IDLE_Q)
		{
			CAppStation *pStation		= NULL;
			CString		 szStationName	= m_pInitOperation->GetStationName(smDiagCommand.lStation);

			if ((pStation = dynamic_cast<CAppStation*>(GetStation(szStationName))) != NULL)
			{
				pStation->SetAxisAction(smDiagCommand.smCommand.lAxis,
										smDiagCommand.smCommand.lAction, 
										smDiagCommand.smCommand.lUnit);

				pStation->Motion(TRUE);
				pStation->Command(glDIAGNOSTIC_COMMAND);

				m_qState = DIAGNOSTICS_Q;
				bResult = TRUE;
			}
			else
			{
				szDisplay.Format(_T("%s - Diagnostic command canceled, Station Name not found"), m_szModuleName);
				DisplayMessage(szDisplay);
			}

		}
		else
		{
			szDisplay.Format(_T("%s - Diagnostic command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	
	return 1;
}

LONG CMS896AApp::SrvAutoCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	POSITION		pos;
	CString			szDisplay	= _T("");
	CString			szKey		= _T("");
	SFM_CStation	*pStation	= NULL;
	CAppStation	*pAppStation	= NULL;
	BOOL			bResult		= FALSE;

	ResetBuffer();		// Reset the timing log buffer
	BOOL bStatus = ExportNVRAMDataToTxtFile(_T("c:\\MapSorter\\Exe\\NVRAM.bin"));	//v4.40T15

	//v4.43T12
	CMSLogFileUtility::Instance()->MS_BackupLogOperation();	
	CMSLogFileUtility::Instance()->MS_BackupMDOperation();		//v4.52A10
	CMSLogFileUtility::Instance()->MS_BackupLogCtmOperation();	
	CheckAndBackupErrLogFile();		//v4.46T15
	CheckAndBackupCycleStopLog(1024*1024, TRUE);
	CheckAndBackupCycleStopLog(1024*1024, FALSE);

	try
	{
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); 

		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);
		smCommand.bReply = smCommand.lAction = smCommand.lAxis = smCommand.lUnit = 0;

		szDisplay.Format(_T("%s - Receive Auto Cycle command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		if (m_qState == IDLE_Q)
		{
			// Signal all stations to perform system initialization
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->Command(glINITIAL_COMMAND);
					pAppStation->Result(gnOK);
				}
			}

			m_nOperation	= AUTO_OP;
			m_qState		= SYSTEM_INITIAL_Q;
			bResult			= TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - Auto command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	
	return 1;
}

LONG CMS896AApp::SrvManualCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand			smCommand;
	CString				szDisplay	= _T("");
	BOOL				bResult		= FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);
		smCommand.bReply = smCommand.lAction = smCommand.lAxis = smCommand.lUnit = 0;

		szDisplay.Format(_T("%s - Receive Manual command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		if (m_qState == IDLE_Q)
		{
			// TODO: Process the received command and perform the Manual operation
			bResult = TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - Manual command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);

			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CMS896AApp::SrvDemoCommand(IPC_CServiceMessage &svMsg)
{
	SMotCommand		smCommand;
	POSITION		pos;
	CString			szDisplay	= _T("");
	CString			szKey		= _T("");
	SFM_CStation	*pStation	= NULL;
	CAppStation	*pAppStation	= NULL;
	BOOL			bResult		= FALSE;
	CString			szErr;

	//v4.59A16	//update in PreStartCycle()			
	if (!m_bPreStartCycleOK)	
	{
		szErr.Format("Prestart Failure! Main application not in idle STATE=%d", m_qState);
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
		HmiMessage("MS: Check Prestart Failure in SrvDemoCommand");
		SetErrorMessage("MS: Check Prestart Failure in SrvDemoCommand");
		SetAlarmLamp_Yellow(FALSE, TRUE);

		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (m_bIsFirstBondCycle)
	{
		int i;
		m_bIsFirstBondCycle = FALSE; //reset the flag
		CTime theTime = CTime::GetCurrentTime();
		
		m_szBondStartDate.Format("%d-%d-%d", theTime.GetYear(), theTime.GetMonth(), theTime.GetDay());
		m_szBondStartTime.Format("%02d:%02d:%02d", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());

		(m_smfSRam)["MS896A"]["Software Version"] = m_szSoftVersion;
		(m_smfSRam)["MS896A"]["PKG Filename"] = GetPKGFilename();

		for (i = 1; i <= MS_MAX_BIN; i++) 
		{
			//Software Version
			((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[i])->
			SetSoftwareInfoItems("Software Version", m_szSoftVersion);

			//Package Filename
			((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[i])->
			SetSoftwareInfoItems("Package Filename", m_szDeviceFile);

			//Bond Start Date
			((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[i])->
			SetBinBlockInfoItems("Bond Start Date", GetBondStartDate());

			//Bond Start Time
			((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[i])->
			SetBinBlockInfoItems("Bond Start Time", GetBondStartTime());
		}
	}
		
	//v4.43T12
	CMSLogFileUtility::Instance()->MS_BackupLogOperation();	
	CMSLogFileUtility::Instance()->MS_BackupMDOperation();			//v4.52A10
	CMSLogFileUtility::Instance()->MS_BackupLogCtmOperation();	
	CMSFileUtility::Instance()->CheckAndBackupLogFileError();		//v4.51A14	//Cree HZ
	CheckAndBackupErrLogFile();		//v4.46T15
	CheckAndBackupCycleStopLog(1024*1024, TRUE);
	CheckAndBackupCycleStopLog(1024*1024, FALSE);

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoBond Start - "		+ m_szSoftVersion);		//v4.49A5
	CMSLogFileUtility::Instance()->MS_LogMDOperation("AutoBond Start - "	+ m_szSoftVersion);		//v4.52A10
	CMSLogFileUtility::Instance()->LogCycleStartStatus("AutoBond Start - "	+ m_szSoftVersion);		//v4.49A5

	ResetBuffer();		// Reset the timing log buffer
	BOOL bStatus = ExportNVRAMDataToTxtFile(_T("c:\\MapSorter\\Exe\\NVRAM.bin"));	//v4.40T15

	try
	{
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); 

		// Decipher the receive request command
		svMsg.GetMsg(sizeof(SMotCommand), &smCommand);
		smCommand.bReply = smCommand.lAction = smCommand.lAxis = smCommand.lUnit = 0;

		szDisplay.Format(_T("%s - Receive Demostration command"), m_szModuleName);

		if (m_qState == IDLE_Q) 
		{
			m_lAction		= smCommand.lAction;
			m_nOperation	= DEMO_OP;
			m_qState		= SYSTEM_INITIAL_Q;

			// Signal all stations to perform Demo Cycle operation
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Motion();
					pAppStation->Result(gnOK);
					pAppStation->SetAxisAction(0, m_lAction, 0);
					pAppStation->Command(glINITIAL_COMMAND);
				}
			}

			m_bOnBondMode	= TRUE;		//ANDREW_SC
			bResult			= TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - Demostration command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);
			SetErrorMessage(szDisplay);

			m_bOnBondMode	= FALSE;	//ANDREW_SC
			bResult = FALSE;

			m_bCycleStarted = FALSE;
			m_bCycleNotStarted = TRUE;
			m_bCycleFKey = TRUE;
			m_bShortCutKey = TRUE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		m_bOnBondMode	= FALSE;		//ANDREW_SC
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CMS896AApp::SrvStopCommand(IPC_CServiceMessage &svMsg)
{
	//SMotCommand		smCommand;
	POSITION		pos;
	//CString			szDisplay	= _T("");
	CString			szKey		= _T("");
	SFM_CStation	*pStation	= NULL;
	CAppStation	*pAppStation	= NULL;
	BOOL			bResult		= FALSE;

	CString szMsg;
	szMsg.Format("Stop command is running by stop button or sub station.StopButtonPressed,%d,AlignComplete,%d",m_bMsStopButtonPressed,m_pWaferTable->m_bWaferAlignComplete);
	//OutputDebugString(szMsg);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	//CMSLogFileUtility::Instance()->MS_LogCycleState(szMsg);	//	427TX	1
	LogCycleStopStatus(szMsg);

	if (m_bMsStopButtonPressed || !m_pWaferTable->m_bWaferAlignComplete)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	try
	{
		// Signal all stations to perform Stop operation
		for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
		{
			m_pStationMap.GetNextAssoc(pos, szKey, pStation);

			if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
			{
				if ((m_lAction == glSYSTEM_EXIT) ||
						(m_lAction == glEMERGENCY_STOP))
				{
					pAppStation->SetAxisAction(0, glABORT_STOP, 0);
				}
				else
				{
					pAppStation->SetAxisAction(0, m_lAction, 0);
				}

				pAppStation->Command(glSTOP_COMMAND);
				pAppStation->Result(gnOK);
			}
		}

		if (m_qState != IDLE_Q)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("App not in idle state");
			m_fStopMsg = TRUE;
		}
		else
		{
			m_fStopMsg = FALSE;
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("App into stopping q");

		m_qState = STOPPING_Q;
		bResult	= TRUE;
		m_bOnBondMode	= FALSE;	//ANDREW_SC
		m_bStopAlign = TRUE;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	if (CMS896AStn::m_ulNewPickCounter > 0 || m_bBurnIn)
		SaveTimeToFile();		// Save timing data
	SaveRecordToFile();		// Save bond result to file
	CS_SaveToFile();		// Save cycle state to file

	int i;

	CTime theTime = CTime::GetCurrentTime();

	m_szBondStopDate.Format("%d-%d-%d", theTime.GetYear(), theTime.GetMonth(), theTime.GetDay());
	m_szBondStopTime.Format("%02d:%02d:%02d", theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());

	for (i = 1; i <= MS_MAX_BIN; i++) 
	{
		((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[i])->
		SetBinBlockInfoItems("Bond End Date", GetBondStopDate());

		((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[i])->
		SetBinBlockInfoItems("Bond End Time", GetBondStopTime());
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoBond Stop");		//v3.55
	CMSLogFileUtility::Instance()->MS_LogMDOperation("AutoBond Stop");		//v4.52A10

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CMS896AApp::SrvStopAutoBond(IPC_CServiceMessage &svMsg)
{
	//m_lStopButtonPressed++;	//	press HMI stop button
	CMS896AApp::m_bStopAlign = TRUE;	//	press stop button
	//CMS896AStn::m_bInAutoCycleMode = FALSE;	//	press stop button, but indeed the stop cycle not complete
	//m_bStopAlignItself = FALSE;

	CString szMsg;
	szMsg = "MS: STOP button pressed by operator via HMI. Set StopAlign to TRUE";
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	LogCycleStopStatus(szMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::SrvResetCommand(IPC_CServiceMessage &svMsg)
{
	struct
	{
		LONG			lStation;
		SMotCommand		smCommand;
	} smResetCommand;

	POSITION	  pos;
	CString		  szDisplay	= _T("");
	CString		  szKey		= _T("");
	SFM_CStation *pStation	= NULL;
	CAppStation  *pAppStation = NULL;
	BOOL		  bResult	= FALSE;

	try
	{
		// Decipher the receive request command
		svMsg.GetMsg(sizeof(smResetCommand), &smResetCommand);

		szDisplay.Format(_T("%s - Receive Reset command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		if (smResetCommand.smCommand.lAction == glRESET_SINGLE)
		{
			CString szStationName = m_pInitOperation->GetStationName(smResetCommand.lStation);
			if ((pAppStation = dynamic_cast<CAppStation*>(GetStation(szStationName))) != NULL)
			{
				if (pAppStation->State() == ERROR_Q)
				{
					pAppStation->Command(glRESET_COMMAND);
					bResult = TRUE;
				}
			}
			else
			{
				szDisplay.Format(_T("%s - Single Reset command canceled, Station Name not found"), m_szModuleName);
				DisplayMessage(szDisplay);
			}
		}
		else
		{
			for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);
				pAppStation = dynamic_cast<CAppStation*>(pStation);

				if (pAppStation->State() == ERROR_Q)
				{
					pAppStation->Command(glRESET_COMMAND);
				}

				pAppStation->Abort(FALSE);
			}

			bResult = TRUE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	
	return 1;
}

LONG CMS896AApp::SrvDeInitialCommand(IPC_CServiceMessage &svMsg)
{
	CString			szDisplay, szKey;
	BOOL			bResult;
	SFM_CStation	*pStation;
	CAppStation	*pAppStation;

	try
	{
		szDisplay.Format(_T("%s - Receive hardware de-initialize command"), m_szModuleName);
		DisplayMessage(szDisplay);
		
		// Decipher the receive request command
		//	svMsg.GetMsg(sizeof(SCommand), &smCommand);

		if (m_qState == IDLE_Q)
		{
			//Hipec hardware DeInitialize
			m_pInitOperation->DeleteContents();
			DisplayMessage("Reset Hipec Ring Network ...");

			//Re-Load Mark IDE Data
			LoadData();

			if (!m_fEnableHardware)
			{
				m_pInitOperation->EnableAllHardware(FALSE);
			}

			//Init Hipec hardware
			m_pInitOperation->InitializeHipec();
			DisplayMessage("Hipec Ring Network initialized ...");

			// Signal all stations to ReInitialize
			for (POSITION pos = m_pStationMap.GetStartPosition(); pos != NULL;)
			{
				m_pStationMap.GetNextAssoc(pos, szKey, pStation);

				if ((pAppStation = dynamic_cast<CAppStation*>(pStation)) != NULL)
				{
					pAppStation->Command(glDEINITIAL_COMMAND);
					pAppStation->ReInitialized(FALSE);
					pAppStation->Result(gnOK);
				}
			}

			m_qState = DE_INITIAL_Q;
			bResult = TRUE;
		}
		else
		{
			szDisplay.Format(_T("%s - De-Initialize command canceled, Systems not in IDLE state"), m_szModuleName);
			DisplayMessage(szDisplay);
		
			bResult = FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		bResult = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	
	return 1;
}

// Burn-In Mode
LONG CMS896AApp::SrvBurnInCommand(IPC_CServiceMessage &svMsg)
{
	m_bBurnIn = TRUE;
	m_bPreStartCycleOK	= TRUE;		//v4.59A17	//Renesas MS90
	BackupBurnInStatus();
	return SrvDemoCommand(svMsg);	// press start burn in button
}

// Stop Burn-In Mode
LONG CMS896AApp::SrvStopBurnInCommand(IPC_CServiceMessage &svMsg)
{
	m_bBurnIn = FALSE;
	LONG lReturn = SrvStopCommand(svMsg);
	return lReturn;
}

LONG CMS896AApp::CheckIfAllTaskInIdleState(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;

	SFM_CStation *pStation;
	CAppStation  *pAppStation;
	POSITION	  pos;
	CString		  szKey;

	if (m_qState != IDLE_Q) 
	{
		CString szErr;
		szErr.Format(" Main application not in idle STATE=%d", m_qState);

		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
		HmiMessage(szErr);
		SetErrorMessage(szErr);
		SetAlarmLamp_Yellow(FALSE, TRUE);

		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);
		pAppStation = dynamic_cast<CAppStation*>(pStation);

		if (pAppStation->State() != IDLE_Q)
		{
			CString szErr;
			szErr.Format("  STATE=%d", pAppStation->State());
			szErr = szKey + " station not in IDLE state!  Please re-start program!" + szErr;

			SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
			HmiMessage(szErr);
			SetAlarmLamp_Yellow(FALSE, TRUE);

			bResult = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CMS896AApp::IsPickAndPlaceOn()
{
	LONG lPicknPlaceMode = MS_PICK_N_PLACE_DISABLED;
	IPC_CServiceMessage rReqMsg;

	int nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "IsPickAndPlaceOn", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(LONG), &lPicknPlaceMode);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	return lPicknPlaceMode;
}


BOOL CMS896AApp::RealignBinFrame()
{
	IPC_CServiceMessage rReqMsg;
	int	nConvID2;
	BOOL bResult = FALSE;

	BOOL bToggle = FALSE;	//Not used
	rReqMsg.InitMessage(sizeof(BOOL), &bToggle);
	nConvID2 = m_comClient.SendRequest(BIN_LOADER_STN, "RealignBinFrameCmd1", rReqMsg);	//shiraishi02
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID2, 500000)==TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID2, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bResult;
}


VOID CMS896AApp::PreStartCycleFailHandling()
{
	//Enable screen button
	m_bCycleStarted = FALSE;
	m_bCycleNotStarted = TRUE;
	m_bCycleFKey = TRUE;
	m_bShortCutKey = TRUE;
	m_bPreStartCycleOK = FALSE;		//v4.59A16
}


VOID CMS896AApp::SendStartStopEvent_SIS(const BOOL bStart)
{
    if (IsSecsGemInit())
    {
  		//Notice the SIS to stop bonding, MS_SECS_SV_START_STOP_PRESSED : 1 -- Start bonding, 0 -- Stop Bonding
		(*CMS896AStn::m_pGemStation)[MS_SECS_SV_START_STOP_PRESSED]	= bStart;  //5031
		CMS896AStn::m_pGemStation->SendEvent(SG_CEID_START_STOP, FALSE);
	}
}


VOID CMS896AApp::BackupBurnInStatus()
{
	m_pWaferTable->BackupBurnInStatus();
	m_pWaferPr->BackupBurnInStatus();
	m_pBondHead->BackupBurnInStatus();
	m_pBondPr->BackupBurnInStatus();
}

VOID CMS896AApp::RestoreBurnInStatus()
{
	m_pWaferTable->RestoreBurnInStatus();
	m_pWaferPr->RestoreBurnInStatus();
	m_pBondHead->RestoreBurnInStatus();
	m_pBondPr->RestoreBurnInStatus();
}

// Pre Start Cycle check other station status
LONG CMS896AApp::PreStartCycle(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = TRUE;
	BOOL bIsEnableWaferLoader = FALSE;

//	CMSLogFileUtility::Instance()->MissingDieThreshold_LogOpen();
//	m_pBondHead->RuntimeUpdateColletJamThresholdValue();
//	m_pBondHead->RuntimeUpdateMissingDieThresholdValue();
//	CMSLogFileUtility::Instance()->MissingDieThreshold_LogClose();

	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle start");
	CMSLogFileUtility::Instance()->MS_LogCycleState("start sorting");	//	427TX	1

	m_bMS_StandByButtonPressed_HMI = FALSE;

	SendStartStopEvent_SIS(TRUE);
	m_bPreStartCycleOK = TRUE;	//v4.59A16
	m_bStopAlign = FALSE;

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (pWaferPr && pWaferPr->IsInitPreScanWaferCamFailed())
	{

	}
	CString szErr;
	if (m_qState != IDLE_Q) 
	{
		szErr.Format("Prestart Failure! Main application not in idle STATE=%d", m_qState);
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
		HmiMessage(szErr);
		SetErrorMessage(szErr);
		SetAlarmLamp_Yellow(FALSE, TRUE);

		PreStartCycleFailHandling();
		SendStartStopEvent_SIS(FALSE);
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	CString szStartStopSetupTime = m_smfSRam["MS896A"]["StartStop Setup Time"];  //4.52D16
	if(szStartStopSetupTime == "Start" )
	{
		m_eqMachine.SetTime (FALSE, EQUIP_SETUP_TIME, "", "Stop Set up time"); 
		m_eqMachine2.SetTime(FALSE, EQUIP_SETUP_TIME, "", "Stop Set up time");
	}

	// start run time counter
	m_eqMachine.SetTime (TRUE, EQUIP_RUN_TIME, "", "Machine Start");
	m_eqMachine2.SetTime(TRUE, EQUIP_RUN_TIME, "", "Machine Start");	//WH Sanan	//v4.40T4
	
	if (m_pSecsComm)
	{
		m_pSecsComm->SetStartOperationStatus();
	}
	if (!SubPreStartCycle())
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
		HmiMessage("Prestart Failure!");
		SetErrorMessage("Prestart Failure!");
		SetAlarmLamp_Yellow(FALSE, TRUE);
		PreStartCycleFailHandling();
		m_eqMachine.SetTime (FALSE, EQUIP_RUN_TIME, "", "Machine Stop");
		m_eqMachine2.SetTime(FALSE, EQUIP_RUN_TIME, "", "Machine Stop");		//v4.40T4	//WH Sanan

		bResult = FALSE;
		//SendStartStopEvent_SIS(FALSE);
		m_pSecsComm->SetStopOperationStatus();
	}
	
	if (m_fEnableHardware == FALSE)
	{
		bResult = TRUE;
		m_bMsStopButtonPressed = FALSE;
		CMS896AStn::m_bWaferAlignComplete = TRUE;
		m_eqMachine.SetTime (FALSE, EQUIP_RUN_TIME, "", "Machine Stop");
		m_eqMachine2.SetTime(FALSE, EQUIP_RUN_TIME, "", "Machine Stop");	//WH Sanan //v4.40T4
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}


BOOL CMS896AApp::SubPreStartCycle()
{
	BOOL bInput = FALSE;
	BOOL bResult = TRUE;
	BOOL bIsEnableWaferLoader = FALSE;
	CString szMsg;
	INT nReply = 0;
	int nConvID;
	IPC_CServiceMessage rReqMsg;

	if (GetCustomerName() == CTM_SANAN)
	{
		bResult = FALSE;
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BL::UnLoadBinFrameToFullMagazine");
		int nConvID2 = m_comClient.SendRequest(BIN_LOADER_STN, "UnLoadBinFrameToFullMagazine", rReqMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID2, 500000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID2, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	if (!bResult)
	{
		return FALSE;
	}

	//andrew
	//-- Do not allow to run if any station is not in IDLE state --//
	SFM_CStation *pStation;
	CAppStation  *pAppStation;
	POSITION	  pos;
	CString		  szKey;
	for (pos = m_pStationMap.GetStartPosition(); pos != NULL;)
	{
		m_pStationMap.GetNextAssoc(pos, szKey, pStation);
		pAppStation = dynamic_cast<CAppStation*>(pStation);

		if (pAppStation->State() != IDLE_Q)
		{
			CString szErr;
			szErr.Format("  STATE=%d", pAppStation->State());
			szErr = szKey + " station not in IDLE state!  Please try again! " + szErr;

			SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
			HmiMessage(szErr);
			SetAlarmLamp_Yellow(FALSE, TRUE);

			return FALSE;
		}
	}


	//v4.04		//Klocwork
	CheckExpireDate();
/*
	if (!CheckExpireDate())		//v3.57T2	//andrew	//SanAn testing purpose
	{
		return FALSE;
	}
*/
	if (!CheckPkgKeyParameters())	//	in prestart cycle, check key parameters.
	{
		return FALSE;
	}

	//Disable screen button
	m_bCycleStarted = TRUE;
	m_bCycleNotStarted = FALSE;
	m_bCycleFKey = FALSE;
	m_bShortCutKey = FALSE;

	//Reset CriticalError
	m_smfSRam["MS899"]["CriticalError"] = FALSE;

	if (m_fEnableHardware == FALSE)
	{
		return TRUE;
	}

	if( m_bAWET_Enabled ) // test name first
	{
		if (_access(gszAWET_FILENAME, 0) == -1)
		{
			CString szMsg;
			szMsg = "*[Alarm Wait Engineer Time] Cannot open.\n" + gszAWET_FILENAME;
			HmiMessage(szMsg);
			szMsg = "*[Alarm Wait Engineer Time] Cannot Open " + gszAWET_FILENAME;
			SetErrorMessage(szMsg);
		}
		else
		{
			CStdioFile cfAWET_ListFile;
			if( cfAWET_ListFile.Open(gszAWET_FILENAME, CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText) )
			{
				if( cfAWET_ListFile.GetLength()==0 )
				{
					szMsg = "*[Alarm Wait Engineer Time] File empty!\n" + gszAWET_FILENAME;
					HmiMessage(szMsg);
					szMsg = "*[Alarm Wait Engineer Time] File empty " + gszAWET_FILENAME;
					SetErrorMessage(szMsg);
				}
				cfAWET_ListFile.Close();
			}
		}
	}

	//Klocwork	//v4.04
	nReply = CMESConnector::Instance()->UpdateMachineStatus(PRESTART_Q, szMsg);

	if (m_bEnableAmiFile == TRUE)
	{
		if (m_bLoadAmiFileStatus == FALSE)
		{
			SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);		//RED
			HmiMessage("Load Ami File Fail");
			SetAlarmLamp_Yellow(FALSE, TRUE);	//Yellow
			
			return FALSE;
		}
	}

	//v2.71
	BOOL bExArm = (BOOL)((LONG) m_smfSRam["BinLoaderStn"]["ExChgArmExist"]);	//v2.74
	
	CString szBinOutputFileFormat = (m_smfSRam)["MS896A"]["OutputFileFormat"];
	BOOL bRealignBinFrameOnMS90 = FALSE;
	BOOL bIsBinLoader = (BOOL)(LONG) m_smfSRam["BinLoaderStn"]["Enabled"];	//v4.10T8	//PLLM Lumiramic MS899EL(special)

	if (m_bMS90 && 
		(GetCustomerName()==CTM_WOLFSPEED || GetCustomerName()==CTM_FINISAR) && 
		bIsBinLoader&& !CMS896AApp::m_bEnableSubBin)	//	MS90, prestart, do bin realignment anyway.
	{
		IPC_CServiceMessage rReqMsg;
		BOOL bFrameExist = FALSE;
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BL::CheckFrameExistOnBinTable");	//v4.59A19
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
	
		if (bFrameExist)	//Only perform when 1st-half is just done and is switching to 2nd half
		{
			if (m_bBinFrameUseSoftRealign)	//v4.59A36
			{
				(m_smfSRam)["BinLoader"]["SoftRealign"] = (BOOL) TRUE;		//v4.59A31	//v4.59A35
			}

			CString szLog;
			szLog.Format("MS: RealignBinFrameCmd1 1, SoftAlignOption = %d", m_bBinFrameUseSoftRealign);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			bRealignBinFrameOnMS90 = TRUE;

			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BL::RealignBinFrameCmd1");	//v4.59A19
			bResult = RealignBinFrame();
			(m_smfSRam)["BinLoader"]["SoftRealign"] = (BOOL) FALSE;		//v4.59A31
			if (bResult == FALSE)
			{
				CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
				SetStatusMessage("AUTOBOND(PRE): MS90 bin realign fail at start!");
				SetErrorMessage("AUTOBOND(PRE): MS90 bin realign fail at start!");

				return FALSE;
			}
		}

		//Make sure the die on BH is bonded if any die left on collet	//FOr MS90	//v4.59A19
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BH::BH_BondLeftDie");	//v4.59A19
		nConvID2 = m_comClient.SendRequest(BOND_HEAD_STN, "BH_BondLeftDie", rReqMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID2, 500000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID2, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if( bResult==FALSE )
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

			szMsg = "AUTOBOND(PRE): MS90 start, bond left die fail!";
			SetStatusMessage(szMsg);
			SetErrorMessage(szMsg);

			return FALSE;
		}
	}

	//Check machine pressure sensor
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckPreBondChecking");	//v3.93
	/*nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_CheckPreBondChecking", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bResult == FALSE)
	{
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
		SetStatusMessage("AUTOBOND(PRE): CheckPreBondChecking fail");
		SetErrorMessage("AUTOBOND(PRE): CheckPreBondChecking fail");

		return FALSE;
	}*/
	
	//Check machine pressure sensor
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BH_CheckPreBondStatus");	//v3.93
	/*nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_CheckPreBondStatus", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}*/

	if (GetCustomerName() == "EverVision")
	{
		BOOL bReturn = TRUE;
		nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "GenerateParameterList", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> Log Parameter for EverVision");
	}

	if (bResult == FALSE)
	{
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
		SetStatusMessage("AUTOBOND(PRE): check BondPR records fail!");
		SetErrorMessage("AUTOBOND(PRE): check BondPR records fail!");

		return FALSE;
	}

	//	check wafer loader prestart status
	bIsEnableWaferLoader = (BOOL)((LONG)m_smfSRam["WaferLoaderStn"]["Enabled"]);	
	if (bIsEnableWaferLoader == TRUE)
	{
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> Check Prestart status");
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "WLCheckPrestartStatus", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
			CString szMsg;
			szMsg = "AUTOBOND(PRE): Check WL prestart fail!";
			SetStatusMessage(szMsg);
			SetErrorMessage(szMsg);

			return FALSE;
		}
	}

	// Updated to use WaferTableStn instead of SafetyStn
	//Check Expander is closed
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> IsExpanderClosed");	//v3.93
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "IsExpanderClosed", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	
	if (bResult == FALSE)
	{
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
		SetStatusMessage("AUTOBOND(PRE): check expander status fail!");
		SetErrorMessage("AUTOBOND(PRE): check expander status fail!");

		return FALSE;
	}

	//Check Wafer die is learnt
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> IsDieLearnt");	//v3.93
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "IsDieLearnt", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	
	if (bResult == FALSE)
	{
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
		SetStatusMessage("AUTOBOND(PRE): check waferPR records fail!");
		SetErrorMessage("AUTOBOND(PRE): check WaferPR records fail!");

		return FALSE;
	}

	//v4.48A26	//Avago
	if (CMS896AStn::m_bUseRefDieFaceValueCheck == TRUE)
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "IsFaceValueExists", rReqMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
			SetStatusMessage("AUTOBOND(PRE): check waferPR RefDie Face Value fail (Avago)!");
			SetErrorMessage("AUTOBOND(PRE): check waferPR RefDie Face Value fail (Avago)!");

			return FALSE;
		}
	}


	//Check Wafer die Inspection setting is set
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> IsInspectionSet");	//v3.93
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "IsInspectionSet", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if (bResult == FALSE)
	{
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
		SetStatusMessage("AUTOBOND(PRE): Wafer PR inspection checking fails!");
		SetErrorMessage("AUTOBOND(PRE): Wafer PR inspection checking fails!");

		return FALSE;
	}

	if (IsRunTimeLoadPKGFile() == FALSE)
	{
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> download multi search record");
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "DownloadMultiSearchRecord", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	BOOL bIsBondPrEnabled = (BOOL)(LONG) m_smfSRam["BondPrStn"]["Enabled"];
	if (bIsBondPrEnabled == TRUE)
	{
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckPostBondSetup");	//v3.93
		//Check Wafer die Inspection setting is set
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "CheckPostBondSetup", rReqMsg);

		INT nLoopCount = 0;
		BOOL bCmdReturn = FALSE;
		while (nLoopCount <= 5)			//v4.15T9	TynTek hangup casE; WAIT FOR 25SEC MAX.
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				bCmdReturn = TRUE;
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}

			nLoopCount++;
		}
		
		if ((bResult == FALSE) || (bCmdReturn == FALSE))
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

			SetStatusMessage("AUTOBOND(PRE): Bond PR checking fails!");
			if (!bCmdReturn)
			{
				SetErrorMessage("AUTOBOND(PRE): Bond PR checking fails #1!");
			}
			else
			{
				SetErrorMessage("AUTOBOND(PRE): Bond PR checking fails!");
			}

			return FALSE;
		}
	}

	BOOL bIsBinLoaderEnabled = (BOOL)(LONG) m_smfSRam["BinLoaderStn"]["Enabled"];	//v4.10T8	//PLLM Lumiramic MS899EL(special)
	if (bIsBinLoaderEnabled == TRUE)
	{
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckBinLoaderSetup");	//v3.93
		//Check Wafer die Inspection setting is set
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "CheckBinLoaderSetup", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

			SetStatusMessage("AUTOBOND(PRE): BinLoader checking fails !");
			SetErrorMessage("AUTOBOND(PRE): BinLoader checking fails!");

			return FALSE;
		}

		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BinLoader PreBondChecking");	//v3.93
		//Check Wafer die Inspection setting is set
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "PreBondChecking", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		if (bResult == FALSE)
		{

			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

			SetStatusMessage("AUTOBOND(PRE): BinLoader PreBondChecking fails !");
			SetErrorMessage("AUTOBOND(PRE): BinLoader PreBondChecking fails!");

			return FALSE;
		}
	}


	//Set wafer map sequence check.
	
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> EnableSequenceCheck");	//v3.93
	if (CMS896AStn::m_bBlkFuncEnable)
	{
		CMS896AStn::m_WaferMapWrapper.EnableSequenceCheck(FALSE);
	}
	else
	{
		CMS896AStn::m_WaferMapWrapper.EnableSequenceCheck(TRUE);
	}


	//Check use Pick & Place or not
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> IsPickAndPlaceOn");	//v3.93
	if (IsPickAndPlaceOn() == MS_PICK_N_PLACE_MANUAL_MODE)
	{
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> StartPickAndPlace");	//v3.93
		//Set Pick & Place mode
		bInput = TRUE;
		rReqMsg.InitMessage(sizeof(BOOL), &bInput);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "StartPickAndPlace", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

			return FALSE;
		}


		//v3.66
		//Check Wafer alignment is done or not in PICK&PLACE mode
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "IsWaferAligned", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
			SetStatusMessage("AUTOBOND(PRE): Check wafer align fail in PICK&PLACE mode!");
			SetErrorMessage("AUTOBOND(PRE): Check wafer align fail in PICK&PLACE mode!");

			return FALSE;
		}


		//v3.66
		//Check WL expander lock & motor power if available
		bIsEnableWaferLoader = (BOOL)((LONG)m_smfSRam["WaferLoaderStn"]["Enabled"]);	
		if (bIsEnableWaferLoader == TRUE)
		{
			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckExpanderLock");	//v3.93
			nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "CheckExpanderLock", rReqMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			
			if (bResult == FALSE)
			{
				CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
				SetStatusMessage("AUTOBOND(PRE): Check WL expander lock fail in PICK&PLACE mode!");
				SetErrorMessage("AUTOBOND(PRE): Check WL expander lock fail in PICK&PLACE mode!");

				return FALSE;
			}
		}
	}
	else
	{
		// Auto pick and place mode
		bInput = TRUE;
		rReqMsg.InitMessage(sizeof(BOOL), &bInput);
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "StartPickAndPlace", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		//Check bin counter is cleared
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckIfAllBinCntCleared");	//v3.93
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIfAllBinCntCleared", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		if (bResult == FALSE)
		{
			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckIfAllBinCntCleared fail");	//v4.11T3
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

			return FALSE;
		}

/*
		//v2.78T2
		//Check bin counter is cleared
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckIsAllBinNotFull");	//v3.93
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIsAllBinNotFull", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
			SetStatusMessage("AUTOBOND(PRE): check BT FULL fail!");
			SetErrorMessage("AUTOBOND(PRE): check BT FULL fail!");

			return FALSE;
		}
*/
		do
		{
			//v2.78T2
			//Check bin counter is cleared
			UCHAR ucBin = 0;
			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckIsAllBinNotFull");	//v3.93
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIsAllBinNotFull", rReqMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(UCHAR), &ucBin);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		
			if (ucBin != 0)		//v4.53A6
			{
				bResult = FALSE;
				BOOL bReturn = FALSE;
/*
				if ((GetFeatureStatus(MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW)) ||
					(GetCustomerName() == "CyOptics")		|| 
					(GetCustomerName() == "FiberOptics")	||		// = Dicon
					(GetCustomerName() == "Inari")	)	
				{
					IPC_CServiceMessage rReqMsg2;
					LONG lBinID = ucBin;
					rReqMsg2.InitMessage(sizeof(LONG), &lBinID);
					nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "AutoLoadUnloadFilmFrameToFull", rReqMsg2);
					while (1)
					{
						if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
						{
							m_comClient.ReadReplyForConvID(nConvID, rReqMsg2);
							rReqMsg2.GetMsg(sizeof(BOOL), &bReturn);
							break;
						}
						else
						{
							Sleep(10);
						}
					}
				}
*/
				if (bReturn == FALSE)
				{
					CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
					SetStatusMessage("AUTOBOND(PRE): check BT FULL fail!");
					SetErrorMessage("AUTOBOND(PRE): check BT FULL fail!");

					return FALSE;
				}
			}
			else
			{
				bResult = TRUE;
			}

		} while (bResult != TRUE);


		//Get new wafer frame if waferloader is enable in cycle
		bIsEnableWaferLoader = (BOOL)((LONG)m_smfSRam["WaferLoaderStn"]["Enabled"]);	

		m_bIsChangeWafer = TRUE;
		if (bIsEnableWaferLoader == TRUE)
		{
			CMS896AStn::m_bWaferAlignComplete = FALSE;		//v3.65	//Semitek
			m_bMsStopButtonPressed = TRUE;				//v3.65	//Semitek

			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> AutoLoad WaferFrame");	//v3.93
			bInput = FALSE;
			if( m_bBurnIn && m_bES100v2DualWftOption )
				bInput = TRUE;
			rReqMsg.InitMessage(sizeof(BOOL), &bInput);
			nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "AutoLoadWaferFrame", rReqMsg);
			
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 36000000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					LONG lStatus = 1;
					rReqMsg.GetMsg(sizeof(LONG), &lStatus);
					if (lStatus == FALSE)
					{
						m_bMsStopButtonPressed = FALSE;
						CMS896AStn::m_bWaferAlignComplete = TRUE;	//v3.65	//Semitek
						m_bIsChangeWafer = FALSE;
						
						CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
						SetStatusMessage("AUTOBOND(PRE): auto-loadframe fail!");
						SetErrorMessage("AUTOBOND(PRE): auto-loadframe fail!");

						return FALSE;
					}
					break;
				}
				else
				{
					Sleep(1);			
				}
			}

			m_bMsStopButtonPressed = FALSE;
			CMS896AStn::m_bWaferAlignComplete = TRUE;		//v3.65	//Semitek
		}
		else
		{
			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> AutoAlignStdWafer");
			/*nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoAlignStdWafer", rReqMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}*/
		}
		m_bIsChangeWafer = FALSE;

		//Check Wafer alignment is done or not
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> IsWafer Aligned");	//v3.93
		/*nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "IsWaferAligned", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
			SetStatusMessage("AUTOBOND(PRE): Check wafer align fail!");
			SetErrorMessage("AUTOBOND(PRE): Check wafer align fail!");

			return FALSE;
		}*/

		//v3.48		//andrew
		//Check Wafer alignment is done or not
		if (bIsEnableWaferLoader == TRUE)
		{
			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckExpanderLock");	//v3.93
			/*nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "CheckExpanderLock", rReqMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			
			if (bResult == FALSE)
			{
				CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
				SetStatusMessage("AUTOBOND(PRE): Check WL expander lock fail!");
				SetErrorMessage("AUTOBOND(PRE): Check WL expander lock fail!");

				return FALSE;
			}*/
		}

		//v3.49T1
		//WT T to global-Theta before AUTOBOND
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> PreStartCheckGlobalTheta");	//v3.93
		/*nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "PreStartCheckGlobalTheta", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
			SetStatusMessage("AUTOBOND(PRE): Check WT-T missing-step fail!");
			SetErrorMessage("AUTOBOND(PRE): Check WT-T missing-step fail!");

			return FALSE;
		}*/
	}

	if (m_bMS90 && bIsBinLoaderEnabled && !CMS896AApp::m_bEnableSubBin)	//	MS90, prestart, do bin realignment anyway.
	{
		IPC_CServiceMessage rReqMsg;
		BOOL bFrameExist = FALSE;
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BL::CheckFrameExistOnBinTable");	//v3.93
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
	
		//Only perform when 1st-half is just done and is switching to 2nd half
		if (bFrameExist && !bRealignBinFrameOnMS90)		
		{
			if (m_bBinFrameUseSoftRealign)	//v4.59A36
			{
				(m_smfSRam)["BinLoader"]["SoftRealign"] = (BOOL) TRUE;		//v4.59A31
			}

			CString szLog;
			szLog.Format("MS: RealignBinFrameCmd1 2, SoftAlignOption = %d", m_bBinFrameUseSoftRealign);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");

			CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BL::RealignBinFrameCmd1");	//v4.59A19
			bResult = RealignBinFrame();
			(m_smfSRam)["BinLoader"]["SoftRealign"] = (BOOL) FALSE;		//v4.59A31
			if (bResult == FALSE)
			{
				CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
				SetStatusMessage("AUTOBOND(PRE): MS90 bin realign fail at start!");
				SetErrorMessage("AUTOBOND(PRE): MS90 bin realign fail at start!");

				return FALSE;
			}
		}
	}


	//Check grade is assigned on bintable
	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BT::CheckGradeAssignment");	//v3.93
	/*nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckGradeAssignment", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if (bResult == FALSE)
	{
		CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

		return FALSE;
	}*/

	//Check frame is aligned (not for DL machine)
	if (bIsBinLoaderEnabled == TRUE&& !CMS896AApp::m_bEnableSubBin)
	{
		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> BL::CheckBinFrameIsAligned");	//v3.93
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "CheckBinFrameIsAligned", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

			return FALSE;
		}

		CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle -> CheckDualBufferFrameExist");	//v3.93
		/*nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "CheckDualBufferFrameExist", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult == FALSE)
		{
			CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);
			return FALSE;
		}*/
	}

	if (GetBondHead()->IsEnaAutoChangeCollet() && !m_pBondPr->IsAGCCenterInPixelValidate())
	{
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);		//RED
		HmiMessage("Change Collet Center Offset Not Teached");
		SetAlarmLamp_Yellow(FALSE, TRUE);	//Yellow
	}

	CMS896AApp::m_bBondHeadILCFirstCycle = TRUE;

	CMSLogFileUtility::Instance()->LogCycleStartStatus("MS PreStart Cycle done");

	return TRUE;
}


LONG CMS896AApp::SaveMapFilePath(IPC_CServiceMessage &svMsg)
{
	CString szMapFilePath;

	szMapFilePath = m_smfSRam["MS896A"]["MapFilePath"];

	WriteProfileString(gszPROFILE_SETTING, gszMAPFILE_PATH_SETTING, szMapFilePath);

	return 1;
}

LONG CMS896AApp::SavePasswordFilePath(IPC_CServiceMessage &svMsg)
{
	WriteProfileString(gszPROFILE_SETTING, gszPLLM_PASSWORD_FILE_PATH, m_szPasswordFilePath);
	return 1;
}

LONG CMS896AApp::SaveMapFileExt(IPC_CServiceMessage &svMsg)
{
	CString szMapFileExt;

	szMapFileExt = m_smfSRam["MS896A"]["MapFileExt"];

	WriteProfileString(gszPROFILE_SETTING, gszMAPFILE_EXT_SETTING, szMapFileExt);

	return 1;
}


LONG CMS896AApp::SaveOutputFilePath(IPC_CServiceMessage &svMsg)
{
	CString szOutputFilePath;

	szOutputFilePath = m_smfSRam["MS896A"]["OutputFilePath"];

	WriteProfileString(gszPROFILE_SETTING, gszOUTPUT_PATH_SETTING, szOutputFilePath);

	return 1;
} 


LONG CMS896AApp::SaveOutputFileFormat(IPC_CServiceMessage &svMsg)
{
	CString szOutputFileFormat;

	szOutputFileFormat = m_smfSRam["MS896A"]["OutputFileFormat"];

	WriteProfileString(gszPROFILE_SETTING, gszOUTPUT_FORMAT_SETTING, szOutputFileFormat);

	/* CTM not use this function right now
	if ( (szOutputFileFormat == "TyntekEpistar") || (szOutputFileFormat == "Epistar") )
	{
		m_bChangeBinLotNo = TRUE;
	}
	else
	{
		m_bChangeBinLotNo = FALSE;
	}
	*/

	return 1;
} 


LONG CMS896AApp::SaveWaferEndFilePath(IPC_CServiceMessage &svMsg)
{
	CString szWaferEndFilePath;

	szWaferEndFilePath = m_smfSRam["MS896A"]["WaferEndFilePath"];

	WriteProfileString(gszPROFILE_SETTING, gszWAFEREND_PATH_SETTING, szWaferEndFilePath);

	return 1;
} 


LONG CMS896AApp::SaveWaferEndFileFormat(IPC_CServiceMessage &svMsg)
{
	CString szWaferEndFileFormat;

	szWaferEndFileFormat = m_smfSRam["MS896A"]["WaferEndFileFormat"];

	WriteProfileString(gszPROFILE_SETTING, gszWAFEREND_FORMAT_SETTING, szWaferEndFileFormat);

	return 1;
}


LONG CMS896AApp::SaveMachineNo(IPC_CServiceMessage &svMsg)
{
	// disable loading the value from smfRam to prevent the case that it is not updated during warm start
	//m_szMachineNo = m_smfSRam["MS896A"]["MachineNo"];

	WriteProfileString(gszPROFILE_SETTING, gszMACHINE_NO_SETTING, m_szMachineNo);

	m_oToolsUsageRecord.SetMachineNo(m_szMachineNo);
	m_eqMachine.SetMachineNo (m_szMachineNo);
	m_eqMachine2.SetMachineNo(m_szMachineNo);	//WH Sanan	//v4.40T4
	CMESConnector::Instance()->SetMachineNo(m_szMachineNo);
	CMSLogFileUtility::Instance()->SetMachineNo(m_szMachineNo);	//	427TX	1

	return 1;
}

LONG CMS896AApp::SaveMachineNo2(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);
	CString szMachineNumber = &pBuffer[0];

	m_szMachineNo = szMachineNumber;
	m_smfSRam["MS896A"]["MachineNo"] = m_szMachineNo;

	WriteProfileString(gszPROFILE_SETTING, gszMACHINE_NO_SETTING, m_szMachineNo);

	m_oToolsUsageRecord.SetMachineNo(m_szMachineNo);
	m_eqMachine.SetMachineNo (m_szMachineNo);
	m_eqMachine2.SetMachineNo(m_szMachineNo);	//WH Sanan	//v4.40T4
	CMESConnector::Instance()->SetMachineNo(m_szMachineNo);
	CMSLogFileUtility::Instance()->SetMachineNo(m_szMachineNo);
	CMSLogFileUtility::Instance()->MS_LogOperation("Machine No changed to: " + m_szMachineNo);
	if (m_pSecsComm != NULL)
	{
		m_pSecsComm->SetEquipmentManagementInfo(TRUE);
	}
	delete[] pBuffer;
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SaveMachineName(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);
	CString szMachineName = &pBuffer[0];

	m_szMachineName = szMachineName;
	m_smfSRam["MS896A"]["MachineName"] = szMachineName;
	WriteProfileString(gszPROFILE_SETTING, gszMACHINE_NAME_SETTING, szMachineName);

	delete[] pBuffer;
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SaveLotNumber(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szLotNumber;
	CString szOutputFilePath;
	CString szCurrLotNo;
	BOOL bSerialChanged = FALSE;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szLotNumber = &pBuffer[0];

	delete[] pBuffer;

	szCurrLotNo = m_smfSRam["MS896A"]["LotNumber"];
	if (szCurrLotNo != szLotNumber)
	{
		bSerialChanged = TRUE;
	}	

	//v2.83T44		//v3.70T2
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetCustomerName() != _T("Electech3E")) && 
		 (pApp->GetCustomerName() != _T("Electech3E(DL)")) )	//v4.44A6
	{
		if ((szLotNumber.GetLength() == 0) || (szLotNumber == ""))
		{
			HmiMessage("Empty Lot Number is detected; operation is aborted.");
			SetErrorMessage("EMpty Lot No in StartLot");
			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	//v3.70T3
	//if (GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	if (m_bIsPLLMRebel)
	{
CString szLog;
szLog = "SaveLotNumber Check Lot Prefix: " + m_szLotNumberPrefix;
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
szLog = "Current Lot Number is: " + szLotNumber;
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		UCHAR ucDigit = szLotNumber.GetAt(0);
		CString sz1stDigit;
		sz1stDigit.Format("%c", ucDigit);

		szLog = "Lot Number digit #1 is: " + sz1stDigit;
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		//if (ucDigit != 'M')
		//if (m_szLotNumberPrefix.Find(ucDigit) != -1)		//v4.40T1
		if ( (m_szLotNumberPrefix.GetLength() > 0) &&		//v4.42T3	//MS109 Flash
			 (m_szLotNumberPrefix != sz1stDigit) )
		{
			HmiMessage("ERROR: Lot ID 1st letter not started with " + m_szLotNumberPrefix + "; operation is aborted.");
			SetErrorMessage("ERROR: Lot ID 1st letter not started with " + m_szLotNumberPrefix + "; operation is aborted.");
			BOOL bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	if (pApp->GetCustomerName() == "Epistar")
	{
		CString szLotFilename = "";
		char* pFilename;
		pFilename = new char[svMsg.GetMsgLen()];
		svMsg.GetMsg(svMsg.GetMsgLen(),pFilename);
		szLotFilename = &pFilename[0];
		delete [] pFilename;
		m_szLotNumber = szLotFilename;
		m_smfSRam["MS896A"]["LotNumber"] = szLotNumber;
	}
	else
	{
		WriteProfileString(gszPROFILE_SETTING, gszLOT_NO_SETTING, szLotNumber);
		m_smfSRam["MS896A"]["LotNumber"] = szLotNumber;
		m_szLotNumber = szLotNumber;	//v3.78
	}

	// delete the bin frame statistics file for Epicrystal
	DeleteFile(gszROOT_DIRECTORY + "\\Exe\\BinFrameStatistics.msd");

	//v3.24T3
	//DO not reset serial for traceability projects
	if (CMS896AStn::m_lOTraceabilityFormat != 0)	//1=LED, 2=SUBMOUNT
	{
		bSerialChanged = FALSE;
	}


	//Reset Bin Serial no & Bin Cleared counter
	if (bSerialChanged == TRUE)
	{
		IPC_CServiceMessage stMsg;
		BOOL bShowAlert = TRUE;
		int nConvID = 0;

		stMsg.InitMessage(sizeof(BOOL), &bShowAlert);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinSerialNo", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		
		// Log Reset Bin Serial
		SetErrorMessage("Reset Bin Serial (SaveLotNumber)");

		stMsg.InitMessage(sizeof(BOOL), &bShowAlert);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinClearedCount", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		//HmiMessage("before return to HMI *****");


		if (CMS896AStn::m_bNewLotCreateDirectory == TRUE && szLotNumber != "")
		{
			szOutputFilePath = m_smfSRam["MS896A"]["OutputFilePath"];
			CreateDirectory(szOutputFilePath, NULL);
			CreateDirectory(szOutputFilePath + "\\" + szLotNumber, NULL);
		}
	}
//HmiMessage("before return to HMI");
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
} 

LONG CMS896AApp::SaveShiftID(IPC_CServiceMessage& svMsg)	//v3.32T3
{
	char *pBuffer;
	CString szShiftID;
	//CString szCurrShiftID;
	
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szShiftID = &pBuffer[0];

	delete [] pBuffer;

	//szCurrLotNo = m_smfSRam["MS896A"]["Shift ID"];

	WriteProfileString(gszPROFILE_SETTING, gszSHIFT_ID_SETTING, szShiftID);
	
	m_szShiftID = szShiftID;
	m_smfSRam["MS896A"]["Shift Id"] = szShiftID;
	
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
} 

LONG CMS896AApp::SaveLotNumberPrefix(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szLotNumberPrefix;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szLotNumberPrefix = &pBuffer[0];
	delete[] pBuffer;

	m_szLotNumberPrefix = szLotNumberPrefix;
	WriteProfileString(gszPROFILE_SETTING, gszLOT_NO_PREFIX_SETTING, szLotNumberPrefix);
	//m_smfSRam["MS896A"]["LotNumber"] = szLotNumber;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SaveRecipeTableFilePath(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szLotNumberPrefix;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);
	CString szPath = &pBuffer[0];
	delete[] pBuffer;

	m_szRecipeFilePath = szPath;
	WriteProfileString(gszPROFILE_SETTING, gszRECIPE_TABLE_PATH_SETTING, m_szRecipeFilePath);
	m_smfSRam["MS896A"]["RecipeFilePath"] = m_szRecipeFilePath;		//v4.46T20	//PLSG 

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SetRecipeTableFilePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	GetPath(m_szRecipeFilePath);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ResetLotNumber(IPC_CServiceMessage &svMsg)
{
	m_szLotNumber = m_smfSRam["MS896A"]["LotNumber"];
	return 1; 
} 


LONG CMS896AApp::SaveBinLotDirectory(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szLotNumber;
	CString szCurrLotNumber;
	CString szOutputFilePath;
	CString szWaferEndFilePath;
	BOOL bChangeLotDir = FALSE;


	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szLotNumber = &pBuffer[0];

	delete[] pBuffer;

	//Get current LotDir	
	szCurrLotNumber = m_smfSRam["MS896A"]["Bin Lot Directory"];
	if (szCurrLotNumber != szLotNumber)
	{
		bChangeLotDir = TRUE;
	}

	WriteProfileString(gszPROFILE_SETTING, gszBIN_LOT_DIRECTORY_SETTING, szLotNumber);
	m_smfSRam["MS896A"]["Bin Lot Directory"] = szLotNumber;


	//Create this directory
	if (bChangeLotDir == TRUE)
	{
		szOutputFilePath = m_smfSRam["MS896A"]["OutputFilePath"];
		szWaferEndFilePath = m_smfSRam["MS896A"]["WaferEndFilePath"];

		CString szTitle;
		CString szContent;

		szTitle.LoadString(HMB_MS_CHANGE_LOT_DIR);
		szContent.LoadString(HMB_MS_CREATE_DIRECTORY);
	
		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			CreateDirectory(szOutputFilePath, NULL);
			CreateDirectory(szWaferEndFilePath, NULL);

			CreateDirectory(szOutputFilePath + "\\" + m_szMachineNo, NULL);
			CreateDirectory(szWaferEndFilePath + "\\" + m_szMachineNo , NULL);
			
			CreateDirectory(szOutputFilePath + "\\" + m_szMachineNo + "\\" + szLotNumber, NULL);
			CreateDirectory(szWaferEndFilePath + "\\" + m_szMachineNo + "\\" + szLotNumber + "-1", NULL);

			SetStatusMessage("Bin lot directory changed and created");
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
} 

LONG CMS896AApp::ResetBinLotDirectory(IPC_CServiceMessage &svMsg)
{
	m_szBinLotDirectory = m_smfSRam["MS896A"]["Bin Lot Directory"];
	return 1; 
}

LONG CMS896AApp::SaveAssociateFile(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	BOOL bChangeAssoFile = FALSE;
	CString szCurrAssoFile, szAssoFile, szAssociateFilename;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szAssoFile = &pBuffer[0];

	delete[] pBuffer;

	//Get current LotDir	
	szCurrAssoFile = m_smfSRam["MS896A"]["Associate File"];
	if (szCurrAssoFile != szAssoFile)
	{
		bChangeAssoFile = TRUE;
	}

	WriteProfileString(gszPROFILE_SETTING, gszASSO_FILE_SETTING, szAssoFile);
	m_smfSRam["MS896A"]["Associate File"] = szAssoFile;

	//Create this directory
	if (bChangeAssoFile == TRUE	&&	m_bEnableAssocFile)
	{
		if (szAssoFile.IsEmpty() == FALSE)
		{
			szAssociateFilename = szAssoFile + "." + m_szAssocFileExt;
		}
		else
		{
			szAssociateFilename = szAssoFile;
		}

		//AfxMessageBox("szAssociateFilename: " + szAssociateFilename, MB_SYSTEMMODAL);
		CMS896AStn::m_WaferMapWrapper.SetAssociateFile(szAssociateFilename);	
		SetStatusMessage("Updated Associate File:" + szAssociateFilename);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
} 

LONG CMS896AApp::ResetAssociateFile(IPC_CServiceMessage &svMsg)
{
	m_szAssociateFile = m_smfSRam["MS896A"]["Associate File"];
	return 1; 
}

LONG CMS896AApp::SaveBinWorkNo(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	//v2.83T44	//v3.70T2
	if ((m_szBinWorkNo.GetLength() == 0) || (m_szBinWorkNo == ""))
	{
		HmiMessage("Empty WO is detected!  Operation is aborted.");
		SetErrorMessage("EMpty WO in StartLot");
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v3.70T3
	if (m_bIsPLLMRebel)		//GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	{
		UCHAR ucDigit;

		//v4.43T9
		if (m_szBinWorkNo.GetLength() < 7)
		{
			CString szMsg;
			szMsg = "WO must be at least 7-digit numeric number!  Operation is aborted.";
			HmiMessage(szMsg);
			SetErrorMessage(szMsg);
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		for (INT n = 0; n < 7; n++)
		{
			ucDigit = m_szBinWorkNo.GetAt(n);
			if ((ucDigit < '0') || (ucDigit > '9'))
			{
				CString szMsg;
				szMsg.Format("WO digit #%d is not numeric!  Operation is aborted.", n + 1);
				HmiMessage(szMsg);
				SetErrorMessage(szMsg);
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}

		ucDigit = m_szBinWorkNo.GetAt(m_szBinWorkNo.GetLength() - 1);
		
		if ( ((ucDigit >= '0') && (ucDigit <= '9')) ||
			 ((ucDigit >= 'A') && (ucDigit <= 'Z')) ||
			 ((ucDigit >= 'a') && (ucDigit <= 'z'))  )
		{
		}
		else
		{
			CString szMsg;
			szMsg.Format("WO last digit is not numeric or alphabet (%c)!  Operation is aborted.", ucDigit);
			HmiMessage(szMsg);
			SetErrorMessage(szMsg);
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
/*
		for (INT n = 0; n < 7; n++)
		{
			ucDigit = m_szBinWorkNo.GetAt(n);
			if ((ucDigit < '0') || (ucDigit > '9'))
			{
				CString szMsg;
				szMsg.Format("WO digit #%d is not numeric!  Operation is aborted.", n + 1);
				HmiMessage(szMsg);
				SetErrorMessage(szMsg);
				bReturn = FALSE;
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
		}
*/
	}


	WriteProfileString(gszPROFILE_SETTING, gszBIN_WORK_NO, m_szBinWorkNo);
	m_smfSRam["MS896A"]["Bin WorkNo"] = m_szBinWorkNo;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::SaveBinFileNamePrefix(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	WriteProfileString(gszPROFILE_SETTING, gszBIN_FILENAME_PREFIX, m_szBinFileNamePrefix);
	m_smfSRam["MS896A"]["Bin FileName Prefix"] = m_szBinFileNamePrefix;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::SaveMapSubFolderName(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	WriteProfileString(gszPROFILE_SETTING, gszMAP_SUBFOLDER_NAME, m_szMapSubFolderName);
	m_smfSRam["MS896A"]["MapSubFolderName"]	= m_szMapSubFolderName;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//for Lumileds Singapore Die Fab Production Line
LONG CMS896AApp::SavePLLMDieFabMESLot(IPC_CServiceMessage &svMsg)
{
	//BOOL bReturn = TRUE;
	BOOL bResult = TRUE;
	CString	szMESLot, szHmiTemp;

	//if (ReadLabelConfig() != 1)		//only for Singapore Die Fab
	if (m_bIsPLLMDieFab != TRUE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	szHmiTemp = "szPLLMDieFabInfo";

	bResult = ManualInputHmiInfo(&szMESLot, HMB_MS_PLLM_DIE_FAB_MES_LOT_INPUT, szHmiTemp);

	if (bResult)
	{
		//AfxMessageBox(szMESLot, MB_SYSTEMMODAL);
		m_szPLLMDieFabMESLot = szMESLot;
		WriteProfileString(gszPROFILE_SETTING, gszDIE_FAB_MES_LOT, m_szPLLMDieFabMESLot);
		m_smfSRam["MS896A"]["PLLMDieFabMESLot"]	= m_szPLLMDieFabMESLot;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

LONG CMS896AApp::GetDeviceFileName(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	m_szDeviceFile = GetProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, _T(""));

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::OutputFileFormatSelectInNewLot(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (CMS896AStn::m_bOutputFormatSelInNewLot == TRUE)
	{
		IPC_CServiceMessage stMsg;
		INT nSelection, nConvID;
		CString szTemp;
		CString szContent, szTitle, szMapHeader, szOutputFormat;
		CString szSelection1 = "1";
		CString szSelection2 = "2";
		CString szSelection3 = "3";

		CStringMapFile *psmf;
		CMSFileUtility::Instance()->LoadAppFeatureConfig();
		psmf = CMSFileUtility::Instance()->GetAppFeatureFile();

		if (psmf == NULL)
		{
			return 0;
		}
		
		szTitle.LoadString(HMB_MS_FORMAT_SELECTION);
		szContent.LoadString(HMB_MS_OUTPUT_FILE_SELECT);
			
		szTemp = MS896A_FUNC_OUTPUT_FILE_FORMAT;
		
		//ULONG nPLLM = GetFeatureValue(MS896A_PLLM_PRODUCT);
		//if ( GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) &&			//v3.86T5	//Bug fix for PLLS DieFab
		//	 ((nPLLM == PLLM_REBEL) || (nPLLM == PLLM_FLASH)) )
		if (GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) ||	m_bIsPLLMRebel)												//v4.33T1	//PLLM MS109
		{
			//THis selection only available for PLLM Falsh & REBEL line
			m_szFileFormatSelectInNewLot = "N/A";	//automatically select "3" Others
		}
		else if (m_bIsPLLMLumiramic)				//v4.35T2	//MS109 buyoff for Lumiramic
		{
			szMapHeader = "LW-Header.txt";
			szOutputFormat = "LW-Output.txt";
			m_szFileFormatSelectInNewLot = (*psmf)[MS896A_FUNC_NEW_LOT_OPTIONS][szTemp + " 1"];
		}
		else
		{
			//PLSG Die-Fab, PLLM Die-Fab
			nSelection = HmiMessage(szContent , szTitle, glHMI_MBX_TRIPLEBUTTON , 
									glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, &szSelection1, &szSelection2, &szSelection3);
			
			if (nSelection == 1)			//Originally for Lumiramic
			{
				szMapHeader = "LW-Header.txt";
				szOutputFormat = "LW-Output.txt";
				m_szFileFormatSelectInNewLot = (*psmf)[MS896A_FUNC_NEW_LOT_OPTIONS][szTemp + " 1"];
			}
			else if (nSelection == 5)		//Die Fab
			{
				szMapHeader = "SMT-Header.txt";
				szOutputFormat = "SMT-Output.txt";
				m_szFileFormatSelectInNewLot = (*psmf)[MS896A_FUNC_NEW_LOT_OPTIONS][szTemp + " 2"];
			}
			else
			{
				m_szFileFormatSelectInNewLot = "N/A";
			}
		}

		psmf->Close();


		if (m_szFileFormatSelectInNewLot != "N/A")
		{
			LPTSTR lpsz = new TCHAR[255];

			strcpy_s(lpsz, 255, (LPCTSTR) m_szFileFormatSelectInNewLot);
			stMsg.InitMessage(sizeof(TCHAR) * 255, lpsz);

			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "InputOutputFileFormat", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 3600000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(1);
				}
			}

			strcpy_s(lpsz, 255, (LPCTSTR)szMapHeader);

			stMsg.InitMessage(sizeof(TCHAR) * 255, lpsz);
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "LoadWaferMapHeader", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 3600000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(1);
				}
			}

			strcpy_s(lpsz, 255, (LPCTSTR) szOutputFormat);

			stMsg.InitMessage(sizeof(TCHAR) * 255, lpsz);
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "LoadOutputFileFormat", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 3600000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(1);
				}
			}

			delete[] lpsz;
		}
	}
	else
	{
		m_szFileFormatSelectInNewLot = "N/A";
	}

	SaveFileFormatSelectInNewLot();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::PackageFileSetupPreRoutine(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		m_szDevicePathDisplay = m_szDevicePath;
	}
	else
	{
		m_szDevicePathDisplay = m_szPortablePKGPath;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::UpdatePackageFileInterface(IPC_CServiceMessage &svMsg)
{
	UCHAR ucPackageFileType;
	BOOL bReturn = TRUE;
	
	svMsg.GetMsg(sizeof(UCHAR), &ucPackageFileType);

	m_ucDeviceFileType = ucPackageFileType;

	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		m_szDevicePathDisplay = m_szDevicePath;
	}
	else
	{
		m_szDevicePathDisplay = m_szPortablePKGPath;
	}

	//SavePackageInfo();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::SaveDeviceFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn;
	BOOL bPortablePKG;
	CString szDevicePath, szDeviceFile;
	
	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		bPortablePKG = FALSE;
		szDevicePath = m_szDevicePath;	
		szDeviceFile = m_szDeviceFile;
	}
	else
	{
		bPortablePKG = TRUE;
		szDevicePath = m_szPortablePKGPath;
		szDeviceFile = m_szDeviceFile;

		//Nichia//v4.43T7
		if ((GetCustomerName() == CTM_NICHIA)	&& 
			(GetProductLine() == _T(""))		&&		//v4.59A34 
			(szDeviceFile.Find(".ppkg") == -1))
		{
			SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
			HmiMessage("ERROR: PRM file must have \"ppkg\" extension", "Save Package");
			SetAlarmLamp_Yellow(FALSE, TRUE);
		
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

	}

	if (m_bDialogDeviceFile == TRUE)
	{
		static char szFilters[] =
			"Package File (*.pkg)|*.pkg|All Files (*.*)|*.*||";

		// Create an Open dialog; the default file name extension is ".pkg".
		CFileDialog dlgFile(FALSE, "pkg", "*.pkg", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
							szFilters, m_pMainWnd, 0);
		dlgFile.m_ofn.lpstrInitialDir = szDevicePath;
		dlgFile.m_ofn.lpstrDefExt = "pkg";

		ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
		INT nReturn = (INT)dlgFile.DoModal();		// Show the file dialog box
		ShowHmi();			// Show the HMI on top

		if (nReturn == IDOK)
		{
			m_szDeviceFile	= dlgFile.GetFileName();
			szDeviceFile = m_szDeviceFile;
			SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
			bReturn = SaveToDevice(szDevicePath , szDeviceFile);
		}
		else
		{
			bReturn = FALSE;
		}
	}
	else
	{
		bReturn = SaveToDevice(szDevicePath , szDeviceFile);
	}

	//v4.21T7	//Walsin China only
	GeneratePkgDataFile(m_szPkgFileListPath, m_szDeviceFile);
	GeneratePkgFileList(m_szPkgFileListPath);		
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}



LONG CMS896AApp::RestoreDeviceFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	if( m_bChangeMaxCountWithAmi && GetCustomerName() == "Genesis")
	{
		HmiMessage("Please Use Load Ami File Function");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_bOnBondMode = TRUE;
	CMSLogFileUtility::Instance()->MS_LogOperation("Load device file via HMI." + GetPKGFilename());
	//v4.33T1	//PLSG
	m_smfSRam["MS896A"]["Manual Load Pkg"]	= TRUE;

	BOOL bPopConfirm = TRUE, bPopComplete = TRUE;

	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		bReturn = LoadPKGFile(FALSE, m_szDeviceFile, bPopConfirm, bPopComplete, FALSE);
	}
	else
	{
		bReturn = LoadPKGFile(TRUE,  m_szDeviceFile, bPopConfirm, bPopComplete, FALSE);
		if ( GetCustomerName() == CTM_NICHIA && GetProductLine() == _T("") )
			HmiMessage("PRM file loading complete");
		else
			HmiMessage("PPKG file loading complete");
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("Load device file complete." + GetPKGFilename());

	if( bPopComplete==FALSE )
	{
		CString szMsg, szTitle;
		szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);
		szMsg.LoadString(HMB_MS_WARN_START_OK);
		HmiMessage(szMsg, szTitle);
	}

	m_bOnBondMode = FALSE;	//ANDREW_SC
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::UpdatePackageFilePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		m_szDevicePath = m_szDevicePathDisplay;
	}
	else
	{
		m_szPortablePKGPath = m_szDevicePathDisplay;
	}

	SavePackageInfo(MS_PKG_SAVE_PATH);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::GetPackagePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;
	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		GetPath(m_szDevicePath);
		if(m_szDevicePath.Left(1) == "C" || m_szDevicePath.Left(1) == "c" || m_szDevicePath.Left(1) == "D" || m_szDevicePath.Left(1) == "d" || m_szDevicePath.Left(1) == "E" || m_szDevicePath.Left(1) == "e")
		{
			m_szDevicePathDisplay = m_szDevicePath;
		}
		else
		{
			szMsg = _T("Incorrect PKG Path.\n The path changes to:\n") + gszROOT_DIRECTORY + _T("\\OutputFiles\\Param");
			HmiMessage(szMsg, "Incorrect PKG Path", glHMI_MBX_CLOSE);
			m_szDevicePath = gszROOT_DIRECTORY + _T("\\OutputFiles\\Param");
			m_szDevicePathDisplay = m_szDevicePath;
		}
	}
	else
	{
		GetPath(m_szPortablePKGPath);
		m_szDevicePathDisplay = m_szPortablePKGPath;
	}

	SavePackageInfo(MS_PKG_SAVE_PATH);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::DeletePackageFile(IPC_CServiceMessage &svMsg)
{
	BOOL	bReturn;
	CString szMsg;
	CString	strFullPath;
	//CString strTemp;
	CString szTitle;
	CString szContent;
	INT		iRtn;
	//BOOL	bPortablePKG;
	CString szDevicePath, szDeviceFile;

	//if (m_lDeviceFileType == MS_PACKAGE_FILE_TYPE)
	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		szDevicePath = m_szDevicePath;
		szDeviceFile = m_szDeviceFile;
	}
	else
	{
		szDevicePath = m_szPortablePKGPath;
		szDeviceFile = m_szDeviceFile;
	}

	strFullPath = szDevicePath + "\\" + szDeviceFile;
	
	szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);
	szContent.LoadString(HMB_MS_DELETE_PKG_WARNING);

	iRtn = HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	if (iRtn == glHMI_NO)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	TRY
	{
		CFile::Remove(strFullPath);
		DeletePKGPreviewImage(szDevicePath, szDeviceFile);
	}
	CATCH(CFileException, e)
	{
		;//e->ReportError();
	}
	END_CATCH

	m_szDeviceFile = "PackageFile.Pkg";

	GeneratePkgFileList(m_szPkgFileListPath);		//v4.21T7	//Walsin China

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::LoadAmiFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szFilename;
	CString szDevicePath;
	CString szSummaryFilePath;
	CString szStr, szTitle;
	CAmiFileDecode oAmiDecoder;
	
	static char szFilters[] = "All Files (*.*)|*.*||";

	// Create an Open dialog; the default file name extension is ".pkg".
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , szFilters, m_pMainWnd, 0);
	dlgFile.m_ofn.lpstrInitialDir = m_szAmiFilePath;
	dlgFile.m_ofn.lpstrDefExt = "ami";
	
	ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	
	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	ShowHmi();		// After input, restore the application to background
	
	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");

	if (nReturn != IDOK)
	{
		szStr = "Load AMI file aborted.";
		HmiMessage(szStr, "AMI file");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		CMSLogFileUtility::Instance()->MS_LogOperation(szStr); 
		return 1;
	}

	szFilename = dlgFile.GetPathName();

	m_bLoadAmiFileStatus = FALSE; 

	//v4.43T1
	CMSLogFileUtility::Instance()->MS_LogOperation("AMI: Load AMI file - " + szFilename); 
	if (ReadAmiFile(oAmiDecoder, szFilename) == FALSE)
	{	
		SetErrorMessage("AMI: Read AMI file fail - " + szFilename);
		SetAlarmLamp_Red(FALSE, TRUE);
		HmiMessage("Fails to read AMI file", "Load AMI File");
		SetAlarmLamp_Yellow(FALSE, TRUE);
		SaveAppData();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (oAmiDecoder.IsResetAmiLoadFailStatus() == TRUE)
	{
		SetErrorMessage("AMI: IsResetAmiLoadFailStatus fail - " + szFilename);
		SetAlarmLamp_Red(FALSE, TRUE);
		HmiMessage("Fails to reset AMI file status", "Load AMI File");
		SetAlarmLamp_Yellow(FALSE, TRUE);
		m_bLoadAmiFileStatus = TRUE;
		SaveAppData();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
	
	CMSLogFileUtility::Instance()->MS_LogOperation("AMI: Load PKG file - " + oAmiDecoder.m_szPKGFilename); 
	if (LoadPKGFile(FALSE, oAmiDecoder.m_szPKGFilename, TRUE, FALSE, TRUE) == FALSE)
	{
		SetErrorMessage("AMI: Load PKG file fail");
		SetAlarmLamp_Red(FALSE, TRUE);
		HmiMessage("Fails to load PKG file", "Load AMI File");
		SetAlarmLamp_Yellow(FALSE, TRUE);
		SaveAppData();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	INT lDiePitchX = atoi(oAmiDecoder.m_szDiePitchX);
	INT lDiePitchY = atoi(oAmiDecoder.m_szDiePitchY);
	INT lMaxCount  = atoi(oAmiDecoder.m_szMaxCount);
	INT lNoOfBlock = atoi(oAmiDecoder.m_szBinNo);

	BOOL bUpdateSequenceForGenesis = FALSE;
	if( GetCustomerName()=="Genesis" && m_bAmiPitchUpdateBlock && lDiePitchX!=0 && lDiePitchY!=0 )
	{

		CMSLogFileUtility::Instance()->MS_LogOperation("AMI: Load BINSUMMARY file - " + oAmiDecoder.m_szBinSummaryFilename); 
		if (LoadBinSummaryFile(oAmiDecoder.m_szBinSummaryFilename) == FALSE)
		{
			SetErrorMessage("AMI: Load Bin-Summary file fail - " + oAmiDecoder.m_szBinSummaryFilename);
			SetAlarmLamp_Red(FALSE, TRUE);
			HmiMessage("Fails to load BIN Summary file", "Load AMI File");
			SetAlarmLamp_Yellow(FALSE, TRUE);
			SaveAppData();
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		bUpdateSequenceForGenesis = TRUE;

		szStr.Format("Genesis Load AMI die pitch(X,Y) %d,%d,MaxCount:%d", lDiePitchX, lDiePitchY,lMaxCount);
		CMSLogFileUtility::Instance()->MS_LogOperation(szStr); 
		//here pass the loaded pitch to bin table and bin block, then do block setup;
		m_smfSRam["MS896A"]["AmiFile"]["DiePitchX"] = (DOUBLE)lDiePitchX;	// update from AMI to setup block
		m_smfSRam["MS896A"]["AmiFile"]["DiePitchY"] = (DOUBLE)lDiePitchY;	// update from AMI to setup block
		m_smfSRam["MS896A"]["AmiFile"]["MaxCount"]	= lMaxCount;
		m_smfSRam["MS896A"]["AmiFile"]["ChangeMaxCountWithAmi"] = m_bChangeMaxCountWithAmi;
		m_smfSRam["MS896A"]["AmiFile"]["NoOfBlock"]				= lNoOfBlock;
		IPC_CServiceMessage stMsg;
		int nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("AllBlocksSetup"), stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	if (AmiFileChecking(oAmiDecoder) == FALSE)
	{
		SetErrorMessage("AMI: AMI File checking fail");
		SetAlarmLamp_Red(FALSE, TRUE);
		HmiMessage("Fails to check AMI file attributes", "Load AMI File");
		SetAlarmLamp_Yellow(FALSE, TRUE);
		SaveAppData();

		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("AMI: Load BINSUMMARY file - " + oAmiDecoder.m_szBinSummaryFilename); 
	if ((LoadBinSummaryFile(oAmiDecoder.m_szBinSummaryFilename) == FALSE) && (bUpdateSequenceForGenesis == FALSE))
	{
		SetErrorMessage("AMI: Load Bin-Summary file fail - " + oAmiDecoder.m_szBinSummaryFilename);
		SetAlarmLamp_Red(FALSE, TRUE);
		HmiMessage("Fails to load BIN Summary file", "Load AMI File");
		SetAlarmLamp_Yellow(FALSE, TRUE);
		SaveAppData();
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_bLoadAmiFileStatus = TRUE;

	szTitle.LoadString(HMB_MS_AMI_FILE);
	szStr.LoadString(HMB_MS_AMI_LOAD_COMPLETE);
	HmiMessage(szStr, szTitle);

	SaveAppData();

	CMSLogFileUtility::Instance()->MS_LogOperation("AMI: Load Done");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::LoadBinBlockRunTimeData(IPC_CServiceMessage &svMsg)
{
	CString szContent, szTitle;
	BOOL bReturn = FALSE;
	
	szTitle.LoadString(HMB_MS_RESTORE_BIN_DATA);
	szContent.LoadString(HMB_MS_RESTORE_BIN_DATA_WARNING);

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDialogStorageFile == TRUE)
	{
		static char szFilters[] =
			"Storage File (*.rec)|*.rec|All Files (*.*)|*.*||";

		CFileDialog dlgFile(TRUE, "rec", "*.rec", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , 
							szFilters, m_pMainWnd, 0);

		dlgFile.m_ofn.lpstrInitialDir = m_szBinStoragePath;
		dlgFile.m_ofn.lpstrDefExt = "rec";

		ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
		INT nReturn = (INT)dlgFile.DoModal();		// Show the file dialog box
		ShowHmi();			// Show the HMI on top

		if (nReturn != IDOK)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		m_szBinStorageFilename = dlgFile.GetFileName();
	}

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	bReturn = RestoreBinRunTimeData(m_szBinStoragePath, m_szBinStorageFilename);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

LONG CMS896AApp::SaveBinBlockRunTimeData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	//CString szBinBlockRunTimDataFile;

	if (m_bDialogStorageFile == TRUE)
	{
		static char szFilters[] =
			"Storage File (*.rec)|*.rec|All Files (*.*)|*.*||";

		CFileDialog dlgFile(FALSE, "rec", "*.rec", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
							szFilters, m_pMainWnd, 0);

		dlgFile.m_ofn.lpstrInitialDir = m_szBinStoragePath;
		dlgFile.m_ofn.lpstrDefExt = "rec";

		ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
		INT nReturn = (INT)dlgFile.DoModal();		// Show the file dialog box
		ShowHmi();			// Show the HMI on top

		if (nReturn != IDOK)
		{
			bReturn = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		
		m_szBinStorageFilename = dlgFile.GetFileName();
	}
	
	(m_smfSRam)["MS896A"]["BinBlkSummaryPath"] = m_szBinStoragePath;

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	bReturn = SaveBinRunTimeData(m_szBinStoragePath, m_szBinStorageFilename);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CMS896AApp::GetBinStoragePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	GetPath(m_szBinStoragePath);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CMS896AApp::GetPkgFileListPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	GetPath(m_szPkgFileListPath);
	SaveAppData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CMS896AApp::GetAmiFilePath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	GetPath(m_szAmiFilePath);
	SaveAppData();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::CheckIfSubmountLot(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	if (GetCustomerName() != CTM_LUMILEDS)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (CMS896AStn::m_lOTraceabilityFormat != 2)	//If not SUBMOUNT
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::SavePLLMProduct(IPC_CServiceMessage &svMsg)					//xyz123
{
	BOOL bReturn = FALSE;
	ULONG ulProduct = PLLM_REBEL;
	svMsg.GetMsg(sizeof(ULONG), &ulProduct);

	if (GetCustomerName() != CTM_LUMILEDS)
	{
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_ulPLLMProduct = ulProduct;
	WriteProfileInt(gszPROFILE_SETTING, MS896A_PLLM_PRODUCT, ulProduct);

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SetLoadPkgForNewWaferFrame(IPC_CServiceMessage &svMsg)
{
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);
	m_bLoadPkgForNewWaferFrame = bEnable;
	WriteProfileInt(gszPROFILE_SETTING, gszLoadPkgForNewWafer, m_bLoadPkgForNewWaferFrame);

	CString szMsg;
	if (bEnable)
	{
		szMsg = "Enable load package for new wafer frame";
	}
	else
	{
		szMsg = "Disable load package for new wafer frame";
	}
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	HmiMessage(szMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	// for HPO mask fully auto to load PR records


////////////////////////////////////////////////////////////////////////////////////
// Warm & Cold Start, Password
////////////////////////////////////////////////////////////////////////////////////

LONG CMS896AApp::ColdStart(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CString szMsg;
	CString szTitle;
		
	if (CMS896AStn::m_bDisableBH && CMS896AStn::m_bDisableBT && CMS896AStn::m_bDisableBL)
	{
		m_bEnablePostBond = FALSE;
	}

	if (GetCustomerName() == CTM_SEMITEK)
	{
		int nPass = (int) GetProfileInt(gszPROFILE_SETTING, _T("SemitekPassword"), 201277);
		CString szPass;
		szPass.Format("%d", nPass);
		
		BOOL bEnable = CheckPasswordWithTitle(szPass);
		SetErrorMessage("Semitek check cold start password");

		if (bEnable == FALSE)
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		SetErrorMessage("Semitek check cold start password passed");
	}

	HmiControlAccessMode();
	MachineStarted();			// Do machine just started action

	// Cold start will delete all files for first run only. For other cases, the machine will delete all files and then reboot
	if (m_bFirstExecute == TRUE)
	{
		szMsg.LoadString(HMB_MS_COLDSTART_WARNING1);
	}
	else
	{
		szMsg.LoadString(HMB_MS_COLDSTART_WARNING2);
	}

	szTitle.LoadString(HMB_MS_COLDSTART);

	// Issue warning message
	if (HmiMessage(szMsg, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		// Clear NVRAM content per station
		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			CString	szName;
			SFM_CStation *pStation;
			CMS896AStn *pStn;
			m_pStationMap.GetNextAssoc(pos, szName, pStation);
			pStn = dynamic_cast<CMS896AStn*>(pStation);
			pStn->ClearData();
		}

		//v4.04	//Klocwork
		DeleteAllFiles();
		if (m_bFirstExecute == TRUE)
		{
			MachineWarmStart(FALSE);	// Re-init again
		}
		else
		{
			RebootMachine(svMsg);	// Reboot machine
		}
	}
	else
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		SetStatusMessage("S/W Version " + m_szSoftVersion);
		SetStatusMessage("Machine is cold started");
	}

	return 1;
}

LONG CMS896AApp::LogOff(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;

	if (m_bFirstExecute == FALSE)
	{
		CString szTitle;
		CString szContent;
			
		szTitle.LoadString(HMB_MS_LOGOFF);
		szContent.LoadString(HMB_MS_LOGOFF_WARNING);

		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_YES)
		{
			m_bOperator			= FALSE;
			m_bEngineer			= FALSE;
			m_bTechnician		= FALSE;	//v4.38T5	//PLLM Lumiramic
			m_bAdministrator	= FALSE;
			m_szUsername = "";
			m_szUserLogOn = "";	//clear log on, eg Operator and Engineer
			bReturn = TRUE;
			if( GetCustomerName() == _T("SiLan") )		//	when log off, generate machine time report.
			{
				m_eqMachine.SetOperatorLogOut(TRUE);	//	when log off, generate machine time report.
				m_eqMachine2.SetOperatorLogOut(TRUE);	//	when log off, generate machine time report.
				m_eqMachine.Refresh(TRUE);
				m_eqMachine2.Refresh(TRUE);
			}
			if (m_hKeyboardHook != NULL  && GetCustomerName() == CTM_SANAN && GetProductLine() == "XA")
			{
				UnhookWindowsHookEx(m_hKeyboardHook);
				m_hKeyboardHook = NULL;
				m_bIsKeyboardLocked = FALSE;
			}
		}
	}
	else
	{
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::CheckAccessRight(IPC_CServiceMessage &svMsg)
{
	LONG lAccessLevel = 0;
	BOOL bReturn = FALSE, bCheck = FALSE;
	CHAR acPar[200], *pTemp;
	ULONG ulSize;

	svMsg.GetMsg(sizeof(LONG), &lAccessLevel);

	switch (lAccessLevel)
	{
		case 1:			// Operator Level - simple return
			bCheck = FALSE;
			bReturn = TRUE;
			break;

		case 2:			// Engineer Level
			if (m_bEngineer || m_bAdministrator || m_bTechnician)		//v4.22T1	//SanAn
			{
				bCheck = FALSE;
				bReturn = TRUE;
			}
			else
			{
				bCheck = TRUE;
				strcpy_s(acPar, sizeof(acPar), "Please Enter the Engineer Password");
				ulSize = (ULONG)strlen(acPar) + 1;
				pTemp = acPar + ulSize;
				strcpy_s(pTemp, sizeof(acPar) - ulSize, m_szEngineerPassword);
			}
			break;

		case 3:			// Administrator Level
			if (m_bAdministrator == TRUE)
			{
				bCheck = FALSE;
				bReturn = TRUE;
			}
			else
			{
				bCheck = TRUE;
				strcpy_s(acPar, sizeof(acPar), "Please Enter Administrator Password");
				ulSize = (ULONG)strlen(acPar) + 1;
				pTemp = acPar + ulSize;
				strcpy_s(pTemp, sizeof(acPar) - ulSize, m_szAdminPassword);
			}
			break;

		default:
			bCheck = FALSE;
			bReturn = FALSE;
			break;
	}

	if (bCheck == TRUE)
	{
		if (GetCustomerName() == CTM_SANAN || GetCustomerName() == "ChangeLight(XM)" || GetCustomerName() == "NSS")		//v4.44A6
		{
			BOOL bCancel = FALSE;
			bReturn = CheckSananPasswordFile(TRUE, bCancel, lAccessLevel);			//v4.17T7
		}
		else if (GetCustomerName() == "Electech3E(DL)"	|| 
				 GetCustomerName() == "Epitop"			||
				 GetCustomerName() == CTM_CREE )
		{
			bReturn = CheckSananPasswordFile_old(TRUE, lAccessLevel);		//v4.17T7
		}
		else if( (GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG") )
		{
			bReturn = CheckPasswordFile_SemitekZJG(TRUE, lAccessLevel);
		}
		else
		{
			ulSize += (ULONG)strlen(pTemp) + 1;
			svMsg.InitMessage(ulSize, acPar);
			INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", svMsg);

			// Get the reply
			while (m_comClient.ScanReplyForConvID(nConvID, 500) == 0);

			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			svMsg.GetMsg(sizeof(BOOL), &bReturn);
		}
	} 
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::CheckAccessRight_ColletEjLmts(IPC_CServiceMessage &svMsg)
{
	LONG lAccessLevel = 0;
	svMsg.GetMsg(sizeof(LONG), &lAccessLevel);
	BOOL bReturn = CheckPasswordToGo(lAccessLevel);
	/*
	LONG lAccessLevel = 0;
	BOOL bReturn = FALSE, bCheck = FALSE;
	CHAR acPar[200], *pTemp;
	ULONG ulSize;

	svMsg.GetMsg(sizeof(LONG), &lAccessLevel);


	if ((GetCustomerName() != CTM_SANAN) &&
			(GetCustomerName() != "XinGuangLian") &&(GetProductLine() != "TJ") && (GetCustomerName() != "ChangeLight(XM)"))
	{
		bReturn = TRUE;		//By-pass for all other ctms
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	switch (lAccessLevel)
	{
		case 1:			// Operator Level - simple return
			bCheck = FALSE;
			bReturn = TRUE;
			break;

		case 2:			// Engineer Level

			if (m_bEngineer || m_bAdministrator)		//v4.22T1	//SanAn
			{
				bCheck = FALSE;
				bReturn = TRUE;
			}
			else
			{
				bCheck = TRUE;
				strcpy_s(acPar, sizeof(acPar), "Please Enter the Engineer Password");
				ulSize = (ULONG)strlen(acPar) + 1;
				pTemp = acPar + ulSize;
				strcpy_s(pTemp, sizeof(acPar) - ulSize, m_szEngineerPassword);
			}
			break;

		case 3:			// Administrator Level
			if (m_bAdministrator == TRUE)
			{
				bCheck = FALSE;
				bReturn = TRUE;
			}
			else
			{
				bCheck = TRUE;
				strcpy_s(acPar, sizeof(acPar), "Please Enter Administrator Password");
				ulSize = (ULONG)strlen(acPar) + 1;
				pTemp = acPar + ulSize;
				strcpy_s(pTemp, sizeof(acPar) - ulSize, m_szAdminPassword);
			}
			break;

		default:
			bCheck = FALSE;
			bReturn = FALSE;
			break;
	}

	if (GetProductLine() == "TJ")
	{
		bCheck = FALSE;
		bReturn = TRUE;
	}

	if (bCheck == TRUE)
	{
		if (GetCustomerName() == CTM_SANAN)								//v4.44A6
		{
			BOOL bCancel = FALSE;
			bReturn = CheckSananPasswordFile(TRUE, bCancel, lAccessLevel);		//v4.17T7
		}
		else if (GetCustomerName() == "Electech3E(DL)"	|| 
				 GetCustomerName() == "Epitop"			||							//v4.48A2
				 GetCustomerName() == CTM_CREE	)	//v4.51A6
		{
			bReturn = CheckSananPasswordFile_old(TRUE, lAccessLevel);
		}
		else if( (GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG") )
		{
			bReturn = CheckPasswordFile_SemitekZJG(TRUE, lAccessLevel);
		}
		else if (GetCustomerName() == "ChangeLight(XM)")
		{
			bReturn = CheckSananPasswordFile_XMChangeLight();
		}
		else
		{
			ulSize += (ULONG)strlen(pTemp) + 1;
			svMsg.InitMessage(ulSize, acPar);
			INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", svMsg);

			// Get the reply
			while (m_comClient.ScanReplyForConvID(nConvID, 500) == 0);

			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			svMsg.GetMsg(sizeof(BOOL), &bReturn);
		}
	} 
	
	*/
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::CheckAccessRight_MinimizeHmi(IPC_CServiceMessage &svMsg)
{
	LONG lAccessLevel = 0;
	BOOL bReturn = FALSE, bCheck = FALSE;
	CHAR acPar[200], *pTemp;
	ULONG ulSize;

	svMsg.GetMsg(sizeof(LONG), &lAccessLevel);

	if (m_bAllowMinimizeHmi == TRUE)
	{
		bReturn = TRUE;		//By-pass for all other ctms
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	switch (lAccessLevel)
	{
		case 1:			// Operator Level - simple return
			bCheck = FALSE;
			bReturn = TRUE;
			break;

		case 2:			// Engineer Level

			if (m_bEngineer || m_bAdministrator)		//v4.22T1	//SanAn
			{
				bCheck = FALSE;
				bReturn = TRUE;
			}
			else
			{
				bCheck = TRUE;
				strcpy_s(acPar, sizeof(acPar), "Please Enter the Engineer Password");
				ulSize = (ULONG)strlen(acPar) + 1;
				pTemp = acPar + ulSize;
				strcpy_s(pTemp, sizeof(acPar) - ulSize, m_szEngineerPassword);
			}
			break;

		case 3:			// Administrator Level
			if (m_bAdministrator == TRUE)
			{
				bCheck = FALSE;
				bReturn = TRUE;
			}
			else
			{
				bCheck = TRUE;
				strcpy_s(acPar, sizeof(acPar), "Please Enter Administrator Password");
				ulSize = (ULONG)strlen(acPar) + 1;
				pTemp = acPar + ulSize;
				strcpy_s(pTemp, sizeof(acPar) - ulSize, m_szAdminPassword);
			}
			break;

		default:
			bCheck = FALSE;
			bReturn = FALSE;
			break;
	}

	if (bCheck == TRUE)
	{
		if (GetCustomerName() == CTM_SANAN || GetCustomerName() == "ChangeLight(XM)" || GetCustomerName() == "NSS")								//v4.44A6
		{
			BOOL bCancel = FALSE;
			bReturn = CheckSananPasswordFile(TRUE, bCancel, lAccessLevel);		//v4.17T7
		}
		else if (GetCustomerName() == "Electech3E(DL)"	|| 
				 GetCustomerName() == "Epitop"			||							//v4.48A2
				 GetCustomerName() == CTM_CREE	)	//v4.51A6
		{
			bReturn = CheckSananPasswordFile_old(TRUE, lAccessLevel);		//v4.17T7
		}
		else if( (GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG") )
		{
			bReturn = CheckPasswordFile_SemitekZJG(TRUE, lAccessLevel);		//v4.17T7
		}
		else
		{
			ulSize += (ULONG)strlen(pTemp) + 1;
			svMsg.InitMessage(ulSize, acPar);
			INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", svMsg);

			// Get the reply
			while (m_comClient.ScanReplyForConvID(nConvID, 500) == 0);

			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			svMsg.GetMsg(sizeof(BOOL), &bReturn);
		}
	} 

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::CheckAccessRight_FixOne(IPC_CServiceMessage &svMsg)
{
	BOOL bFixOne = FALSE;
	if( GetCustomerName()==CTM_SANAN && GetProductLine()=="XA" )
		bFixOne = TRUE;

	if( bFixOne==FALSE )
	{
		BOOL bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	LONG lAccessLevel = 0;
	svMsg.GetMsg(sizeof(LONG), &lAccessLevel);


	BOOL	bCheck = TRUE;
	CString szFixOne = "";
	switch (lAccessLevel)
	{
	case 2:			// Engineer Level
		szFixOne = "FixedEngineer";
		break;

	case 3:			// Administrator Level
		szFixOne = "FixedAdministrator";
		break;

	case 1:			// Operator Level - simple return
	default:
		bCheck = FALSE;
		break;
	}

	BOOL	bReturn = TRUE;
	if (bCheck == TRUE)
	{
		CHAR acPar[200], *pTemp;
		ULONG ulSize;
		strcpy_s(acPar, sizeof(acPar), "Please Enter your ID");
		ulSize = (ULONG)strlen(acPar) + 1;
		pTemp = acPar + ulSize;
		strcpy_s(pTemp, sizeof(acPar) - ulSize, szFixOne);
		ulSize += (ULONG)strlen(pTemp) + 1;
		svMsg.InitMessage(ulSize, acPar);
		INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", svMsg);
		// Get the reply
		while (m_comClient.ScanReplyForConvID(nConvID, 500) == 0);
		m_comClient.ReadReplyForConvID(nConvID, svMsg);
		svMsg.GetMsg(sizeof(BOOL), &bReturn);

		if( bReturn )
		{
			m_szUsername = szFixOne;
			if (GetCustomerName() == CTM_SANAN || 
				GetCustomerName() == "ChangeLight(XM)" || 
				GetCustomerName() == "NSS")
			{
				BOOL bCancel = FALSE;
				bReturn = CheckSananPasswordFile(FALSE, bCancel, lAccessLevel);
			}
			else if(GetCustomerName() == "Electech3E(DL)"	|| 
					GetCustomerName() == "Epitop"			||
					GetCustomerName() == CTM_CREE )
			{
				bReturn = CheckSananPasswordFile_old(FALSE, lAccessLevel);
			}
			else if( (GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG") )
			{
				bReturn = CheckPasswordFile_SemitekZJG(FALSE, lAccessLevel);
			}
		}
	} 

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SaveOperatorId(IPC_CServiceMessage &svMsg)
{
	char *pBuffer;
	CString szOperatorId;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szOperatorId = &pBuffer[0];

	delete[] pBuffer;

	m_smfSRam["MS896A"]["Operator Id"] = szOperatorId;

	//v3.24T1
	//m_szProductID = szOperatorId;		//Currently Product ID = Operator ID for PLLM Traceability
	WriteProfileString(gszPROFILE_SETTING, gszMAP_USER_NAME,	szOperatorId);		//Save into registry
	//WriteProfileString(gszPROFILE_SETTING, gszMAP_PRODUCT_ID,	m_szProductID);		//Save into registry

	CMS896AStn::m_GenerateWaferDatabase.SetGeneralInfoItems("Operator Id", szOperatorId);
	
	m_oToolsUsageRecord.SetOperatorId(szOperatorId);
	m_eqMachine.SetOperatorId (szOperatorId);
	m_eqMachine2.SetOperatorId(szOperatorId);		//WH Sanan	//v4.40T4
	m_szUsername = szOperatorId;
	if (GetCustomerName() == "EverVision")
	{
        IPC_CServiceMessage sv;
        char acUser[100];
        strcpy_s(acUser, sizeof(acUser), szOperatorId);
        sv.InitMessage((INT)strlen(acUser) + 1, acUser);
        m_comClient.SendRequest("HmiAlarmStation", "HmiSetCurrentUser", sv, 0);
		m_bEverVisionUser = 1;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);		//v3.78
	return 1;
} //end SaveOperatorId


LONG CMS896AApp::SaveLumiledsOperatorId(IPC_CServiceMessage &svMsg)
{
	//char *pBuffer;
	CString szOperatorId, szHmiTemp;
	BOOL bReturn = TRUE;

	//ULONG nPLLM = GetFeatureValue(MS896A_PLLM_PRODUCT);	//v4.08
	szHmiTemp = "MS_szProductID";	//"MS_szUsername";		//v4.38T5	//PLLM Lumiramic
	CString szCurrUserName = m_szUsername;					//v4.38T5	//Backup of user name

	//v4.35T1	//PLLM Rebel & Lumiramic new Recipe auto-load fcn input
	CString szReceipe;
	if (m_bIsPLLMRebel)	
	{
		bReturn = ManualInputHmiInfo(&szReceipe, HMB_MS_PLLM_PART_NAME_INPUT, szHmiTemp);
	}

	if (m_bIsPLLMDieFab || m_bIsPLLMLumiramic)
	{
		bReturn = ManualInputHmiInfo(&szOperatorId, HMB_MS_PLLM_PRODUCT_ID_INPUT, szHmiTemp);
	}
	else
	{
		szHmiTemp = "MS_szUsername";	//v4.40T2
		bReturn = ManualInputHmiInfo(&szOperatorId, HMB_MS_PLLM_OPERATOR_ID_INPUT, szHmiTemp);
	}

	//m_szUsername = szCurrUserName;	//Restore UserName after ManualInputHmiInfo()	//v4.38T5

	if (bReturn)
	{
		if (m_bIsPLLMRebel)	
		{
			m_szProductID	= szReceipe;
			m_szUsername	= szOperatorId;
			m_smfSRam["MS896A"]["Product Id"]	= m_szProductID;
			WriteProfileString(gszPROFILE_SETTING, gszMAP_USER_NAME,	m_szUsername);		//Save into registry
		}
		else
		{
			m_szProductID	= szOperatorId;    //Currently Product ID = Operator ID for PLLM Traceability
			m_smfSRam["MS896A"]["Product Id"]	= szOperatorId;	
		}		
		
		WriteProfileString(gszPROFILE_SETTING, gszMAP_PRODUCT_ID,	m_szProductID);			//Save into registry

		CMS896AStn::m_GenerateWaferDatabase.SetGeneralInfoItems("Operator Id", m_szUsername);
	
		if (m_bIsPLLMDieFab)				//PLSG DieFab
		{
			bReturn = LoadPKGFileByTraceabilityERPNo_PLSG_DieFab(szOperatorId);
		}
		else if (m_bIsPLLMRebel)			//(GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))	
		{
			bReturn = LoadPKGFileByTraceabilityERPNo_PLLM(m_szProductID);				//v4.33T1	//PLLM Rebel/Flash Auto-Recipe fcn
		}
		else if (m_bIsPLLMLumiramic)
		{
			bReturn = LoadPKGFileByTraceabilityERPNo_PLLM_Lumiramic(m_szProductID);		//v4.35		//PLLM Lumiramic Auto-Recipe fcn
		}
	}
CString szTempMsg;
szTempMsg.Format("WaferLabel -- Enable 2nd 1D BarCode:%d, return:%d",m_bEnable2nd1DBarCode, bReturn);
CMSLogFileUtility::Instance()->WL_LogStatus(szTempMsg);

	if(m_bEnable2nd1DBarCode)
	{
	   AfxMessageBox(szTempMsg,MB_SYSTEMMODAL);
	   //4.52D10 byPass OP ID
		//AfxMessageBox("byPass OP ID",MB_SYSTEMMODAL); 
		//bReturn = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

} //end SaveLumiledsOperatorId

LONG CMS896AApp::UpdatePassword(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bTechnician == TRUE)	//v4.38T5	//PLLM Lumiramic
	{
		WriteProfileString(gszPROFILE_SETTING, gszTECHNICIAN_PASSWORD, CodePassword(m_szTechnicianPassword, gnEngineerShift));
	}
	else if (m_bEngineer == TRUE)
	{
		WriteProfileString(gszPROFILE_SETTING, gszENGINEER_PASSWORD, CodePassword(m_szEngineerPassword, gnEngineerShift));
	}

	if (m_bAdministrator == TRUE)
	{
		WriteProfileString(gszPROFILE_SETTING, gszADMIN_PASSWORD, CodePassword(m_szAdminPassword, gnAdminShift));
	}

	if (bReturn == TRUE)
	{
		CString szTitle;
		CString szContent;
			
		szTitle.LoadString(HMB_MS_CHANGE_PASSWORD);
		szContent.LoadString(HMB_MS_CHANGE_PASSWORD_OK);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 300, 200, NULL, NULL, NULL, NULL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ResetPassword(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE, bChange = FALSE;
	LONG lUser;

	svMsg.GetMsg(sizeof(LONG), &lUser);

	switch (lUser)
	{
		default:
		case 1:			// Operator Level & Technicican - simple return
			m_szTechnicianPassword = "technician";
			WriteProfileString(gszPROFILE_SETTING, gszTECHNICIAN_PASSWORD, CodePassword(m_szTechnicianPassword, gnEngineerShift));
			bChange = TRUE;
			break;

		case 2:			// Engineer Level 
			m_szEngineerPassword = "engineer";
			WriteProfileString(gszPROFILE_SETTING, gszENGINEER_PASSWORD, CodePassword(m_szEngineerPassword, gnEngineerShift));
			bChange = TRUE;
			break;

		case 3:			// Administrator Level
			m_szAdminPassword = "FullControl";
			WriteProfileString(gszPROFILE_SETTING, gszADMIN_PASSWORD, CodePassword(m_szAdminPassword, gnAdminShift));
			bChange = TRUE;
			break;
	}

	if (bChange == TRUE)
	{
		CString szTitle;
		CString szContent;
			
		szTitle.LoadString(HMB_MS_CHANGE_PASSWORD);
		szContent.LoadString(HMB_MS_CHANGE_PASSWORD_OK);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 300, 200, NULL, NULL, NULL, NULL);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::RebootMachine(IPC_CServiceMessage &svMsg)
{
	HANDLE		hToken;
	LUID		luID;
	BOOL		bReturn = FALSE;
	TOKEN_PRIVILEGES	Token;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if (LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &luID))
		{
			Token.PrivilegeCount = 1;
			Token.Privileges[0].Luid = luID;
			
			// Set or revoke privilege depending on flag
			Token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (AdjustTokenPrivileges(hToken, FALSE, &Token, 0, 0, 0))
			{
				// Issue message to shut down the system. Force shutdown those hang-up application only
				// The shut down reason is power failure
				if (ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_FLAG_PLANNED | SHTDN_REASON_MAJOR_SOFTWARE))
				{
					bReturn = TRUE;
				}
			}
		}
	}

	if (bReturn == FALSE)
	{
		CString szContent;
			
		szContent.LoadString(HMB_MS_POWER_OFF_ERROR);

		HmiMessage(szContent);
	}

	CloseHandle(hToken);		//Klocwork	//v4.24T11

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ClearWholeNVRAM(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CString szContent;
		
	//pllm
	if (m_bUsePLLMSpecialFcn)
	{
		CStrInputDlg dlg;
		dlg.m_szData = "";
		if (dlg.DoModal() != IDOK)
		{
			BOOL bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}

		if (!CheckPLLMRebelPassword(dlg.m_szData))
		{
			HmiMessage("ERROR: Invalid PLLM password; Manual-Wafer alignment fails.");
			BOOL bReply = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bReply);
			return 1;
		}
	}


	szContent.LoadString(HMB_MS_CLEAR_NVRAM);
	if (HmiMessage(szContent, "", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		ClearNVRAM();

		szContent.LoadString(HMB_GENERAL_CLEARED);

		HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ResetHiPEC(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	CString szContent;

	szContent.LoadString(HMB_MS_RESET_HIPEC);

	if (HmiMessage(szContent, "", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		//Init Hipec hardware
		if (m_pInitOperation)
		{
			m_pInitOperation->InitializeHipec();
			DisplayMessage("Hipec Ring Network initialized ...");
		}

		szContent.LoadString(HMB_GENERAL_RESET);

		HmiMessage(szContent, "", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL);
		bReturn = TRUE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//Update Data
LONG CMS896AApp::UpdateAction(IPC_CServiceMessage &svMsg)
{
	Sleep(500);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::UpdateAppData(IPC_CServiceMessage &svMsg)
{
	SaveAppData();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

/*
LONG CMS896AApp::LogItems(IPC_CServiceMessage& svMsg)
{
	LONG lItemNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lItemNo);
	
	return 1;
}
*/



//Update Access mode file
LONG CMS896AApp::UpdateAccessMode(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	WriteScreenControlAccessMode();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

//Update Host Comm file
LONG CMS896AApp::UpdateHostCommSetting(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	WriteHostCommConfig();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::HostCommConnection(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bConnect = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bConnect);

	if (m_bHostCommEnabled == FALSE)
	{
		CString szTitle;
		CString szContent;
	
		szTitle.LoadString(HMB_MS_TCPIP_MESSAGE);
		szContent.LoadString(HMB_MS_TCPIP_DISABLED);
		
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL);
	}
	else
	{
		ConnectHostComm(bConnect);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::HostCommTest(IPC_CServiceMessage &svMsg)
{
	LONG lCommandNo = 0;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(LONG), &lCommandNo);
	
	switch (lCommandNo)
	{
		case 1:
			SendHostLotNoAck();
			break;
		
		case 2:
			SendHostWaferEnd("FT:TT:ST:RT:DT:CC:RANK:OK:NG:BIN1-32:BIN1-32_RANK");
			break;
		
		case 3:
			SendHostClearBin("BIN1:RANK:CC:MAX_COUNT");
			break;

		default:
			SendHostMachineIDAck();
			break;
	}


	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::SC_CmdHelpScreenInfo(IPC_CServiceMessage &svMsg)
{
	if (IsSecsGemInit())
	{
		//Display Software Ver, Machine model and Release Date
		// 3602
		(*m_pAppGemStation)["HP_HPFirmwareVersion"] = m_szHiPECVersion;
		// 3603
		(*m_pAppGemStation)["HP_ControlSWVersion"] = m_szSoftVersion;
		(*m_pAppGemStation)["HP_ControlSWPartNo"] = m_szSWReleaseNo;
		// 3604
		(*m_pAppGemStation)["HP_LogOnUser"] = m_szUsername;
		// 3605
		(*m_pAppGemStation)["HP_MachineModelNo"] = m_szMachineModel;
		// 7200
		m_pAppGemStation->SendEvent(SG_CEID_EQ_STATUS, FALSE);
	}
	return 1;
}

LONG CMS896AApp::EnableMappingLog(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(BOOL), &m_bEnableWtMachineLog);
	if (GetCustomerName() == CTM_SEMITEK)
	{
		if (CheckPasswordWithTitle("whsemitek"))
		{
			SetErrorMessage("right password to enable wafer map log");
		}
		else
		{
			m_bEnableWtMachineLog = !m_bEnableWtMachineLog;
		}
	}
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_WT_OPTION, (int)m_bEnableWtMachineLog);
	CMSLogFileUtility::Instance()->SetEnableWtMachineLog(m_bEnableWtMachineLog);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::EnableMsMachineLog(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(BOOL), &m_bEnableMachineLog);
	if (GetCustomerName() == CTM_SEMITEK)
	{
		if (CheckPasswordWithTitle("whsemitek"))
		{
			SetErrorMessage("right password to enable machine log");
		}
		else
		{
			m_bEnableMachineLog = !m_bEnableMachineLog;
		}
	}

	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_OPTION, (int)m_bEnableMachineLog);
	CMSLogFileUtility::Instance()->SetEnableMachineLog(m_bEnableMachineLog);

	if (m_bEnableMachineLog)
		CMSLogFileUtility::Instance()->MS_LogOperation("MS Machine Log option is enabled");
	else
		CMSLogFileUtility::Instance()->MS_LogOperation("MS Machine Log option is disabled");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::EnableMsTableIndexLog(IPC_CServiceMessage &svMsg)
{
	svMsg.GetMsg(sizeof(BOOL), &m_bEnableTableIndexLog);

	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_TABLEINDEX_OPTION, (int)m_bEnableTableIndexLog);
	CMSLogFileUtility::Instance()->SetEnableTableIndexLog(m_bEnableTableIndexLog);

	if (m_bEnableTableIndexLog)
		CMSLogFileUtility::Instance()->MS_LogOperation("MS TableIndex Log option is enabled");
	else
		CMSLogFileUtility::Instance()->MS_LogOperation("MS TableIndex Log option is disabled");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::EnableMachineLog(IPC_CServiceMessage &svMsg)
{
	if (GetCustomerName() == CTM_SEMITEK)
	{
		if (CheckPasswordWithTitle("whsemitek"))
		{
			SetErrorMessage("right password to enable machine log");
		}
		else
		{
			m_bEnableMachineLog = !m_bEnableMachineLog;
		}
	}

	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_OPTION, (int)m_bEnableMachineLog);
	CMSLogFileUtility::Instance()->SetEnableMachineLog(m_bEnableMachineLog);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::EnableAutoRecoverPR(IPC_CServiceMessage &svMsg)
{
	if (CheckPasswordWithTitle("SuperUser"))
	{
		SetErrorMessage("right password to enable machine log");
	}
	else
	{
		m_bEnableAutoRecoverPR = !m_bEnableAutoRecoverPR;
	}

	WriteProfileInt(gszPROFILE_SETTING, _T("Auto Retry for scan PR error"), (int)m_bEnableAutoRecoverPR);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}	//	PR error auto recover during scanning

LONG CMS896AApp::EnableWtMachineLog(IPC_CServiceMessage &svMsg)
{
	if (GetCustomerName() == CTM_SEMITEK)
	{
		if (CheckPasswordWithTitle("whsemitek"))
		{
			SetErrorMessage("right password to enable wafer map log");
		}
		else
		{
			m_bEnableWtMachineLog = !m_bEnableWtMachineLog;
		}
	}
	WriteProfileInt(gszPROFILE_SETTING, gszLOG_ENABLE_WT_OPTION, (int)m_bEnableWtMachineLog);
	CMSLogFileUtility::Instance()->SetEnableWtMachineLog(m_bEnableWtMachineLog);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}



LONG CMS896AApp::CheckStartNewLot(IPC_CServiceMessage &svMsg)	//PLLM StartNewLot checking fcn
{
	BOOL bReturn	= TRUE;
	BOOL bStartLot	= TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bStartLot);

	if (bStartLot)
	{
		m_bNewLotStarted = (BOOL)GetProfileInt(gszPROFILE_SETTING, gszSTART_NEW_LOT, 0);

		//Lot already started detected!!
		if (m_bNewLotStarted ||				//In Windows registry, or
				m_bStartNewLot)					//in package file "AppData"
		{
			CString szErr;
			szErr.Format("StartLot Error: lot already-started detected -  %d  %d", m_bNewLotStarted, m_bStartNewLot);
			SetErrorMessage(szErr);

			HmiMessage("ERROR: Lot is already started!");
	
			if (!m_bNewLotStarted)
			{
				m_bNewLotStarted = TRUE;
				WriteProfileInt(gszPROFILE_SETTING, gszSTART_NEW_LOT, (int)m_bNewLotStarted);
			}

			if (!m_bStartNewLot)
			{
				m_bStartNewLot = TRUE;
				SaveAppData();
			}

			bReturn = FALSE;
		}
	}
	else
	{
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::StartNewLot(IPC_CServiceMessage &svMsg)
{
	CTime theTime = CTime::GetCurrentTime();
	BOOL bResult = FALSE;
	BOOL bLotStarted = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bLotStarted);


	if (bLotStarted == TRUE)
	{
		//LogStatusInfo("Start Lot");		//v2.93T2
		LogStatusInfo("Start Lot" + m_szLotNumber);		//v2.93T2	//pllm

		//v3.42T1
		if (m_bUsePLLM)
		{
			if (m_szBinWorkNo.GetLength() <= 0)
			{
				HmiMessage("Invalid WO number; please restart lot again.");
				LogStatusInfo("Invalid WO number; please restart lot again. - " + m_szBinWorkNo);	//pllm
				svMsg.InitMessage(sizeof(BOOL), &bResult);
				return 1;
			}
			if (m_szLotNumber.GetLength() <= 0)
			{
				HmiMessage("Invalid Lot number; please restart lot again. - " + m_szLotNumber);
				LogStatusInfo("Invalid Lot number; please restart lot again. - " + m_szLotNumber);	//pllm
				svMsg.InitMessage(sizeof(BOOL), &bResult);
				return 1;
			}

			//only for Lumileds Singapore Die Fab
			//if (ReadLabelConfig() == 1)
			if (m_bIsPLLMDieFab == TRUE)	//v3.78
			{
				if (m_szPLLMDieFabMESLot.GetLength() <= 0)
				{
					HmiMessage("Invalid Lot number 2; please restart lot again. - " + m_szPLLMDieFabMESLot);
					LogStatusInfo("Invalid Lot number 2; please restart lot again. - " + m_szPLLMDieFabMESLot);
					svMsg.InitMessage(sizeof(BOOL), &bResult);
					return 1;
				}
			}
		}

		//v2.96T4
		LogStatusInfo("StartLot Check bin count");
		bResult = CheckIsAllBinCleared();
		if (!bResult)
		{
			svMsg.InitMessage(sizeof(BOOL), &bResult);
			return 1;
		}


		m_szLotStartTime.Format("%d-%d-%d;%d:%d:%d", 
								theTime.GetDay(), theTime.GetMonth(), theTime.GetYear(),
								theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());

		WriteProfileString(gszPROFILE_SETTING, gszLOT_START_TIME, m_szLotStartTime);
		m_smfSRam["MS896A"]["LotStartTime"] = m_szLotStartTime;
		m_smfSRam["MS896A"]["StartLotCheckCount"] = TRUE;
		m_bStartNewLot = TRUE;		//v2.71
		SaveAppData();

		//Delete Wafer Record file
		DeleteFile(MSD_WAFER_RECORD_FILE);
		DeleteFile(MSD_WAFER_RECORD_BKF);
		DeleteFile(MSD_O_BARCODE_FILE);				//v2.83T65		//v3.70T2
		DeleteFile(MSD_WAFER_BARCODE_FILE);						//PLLM v3.74T31
		DeleteFile(gszUSER_DIRECTORY + _T("\\OutputFile\\FlatFile.txt"));	//v4.02T3	//PLLM Lumiramic FLAT file


		// Search And Remove Old Backup Output File
		if (GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))	//v3.70T2
		{
			//For PLLM REBEL MS899DLA only
			SearchAndRemoveFiles("c:\\MapSorter\\UserData\\OutputFile\\ClearBin", 14);		//v2.83T41	//v4.42T4	
		}
		else if (m_bRemoveBackupOutputFile == TRUE)
		{
			SearchAndRemoveFiles("C:\\output");
		}

		//-- Ask BinTable to enable ACC bin count storage --//	//v2.56
		LogStatusInfo("Enable ACC bond count");		//v2.93T2
		IPC_CServiceMessage rReqMsg;
		int nConvID = 0;
		BOOL bEnableAccBinCount = TRUE;
		rReqMsg.InitMessage(sizeof(BOOL), &bEnableAccBinCount);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "EnableBinAccBondedCount", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		//-- Reset BL magazine --//	//v2.68
		LogStatusInfo("Reset all magazine");		//v2.93T2
		BOOL bBinLoader = (BOOL)(LONG)(m_smfSRam)["BinLoaderStn"]["Enabled"];
		if (bBinLoader)
		{
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "StartLotResetAllMgznCmd", rReqMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}


		//-- Reset BH Die counts for PLLM --//			//v3.27T1
		LogStatusInfo("Reset all BH die counts");
		BOOL bPrompt = FALSE;
		rReqMsg.InitMessage(sizeof(BOOL), &bPrompt);
		nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_ResetAllDieCount", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}


		SetStatusMessage("Current Lot is started");
		LogStatusInfo("Start Lot Done\n");	
		SetErrorMessage("Start Lot Done - " + m_szLotNumber);	
	}
	else
	{
		IPC_CServiceMessage rReqMsg;
		int nConvID = 0;
		CString szTitle;
		CString szContent;
			
		szTitle.LoadString(HMB_MS_SYS_WARNING);
		szContent.LoadString(HMB_MS_LOT_END);

		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300) != glHMI_YES)
		{
			svMsg.InitMessage(sizeof(BOOL), &bLotStarted);
			return 1;
		}


		//v2.96T4
		LogStatusInfo("Check bin count");
		bResult = CheckIsAllBinCleared();

		if (!bResult)
		{
			svMsg.InitMessage(sizeof(BOOL), &bLotStarted);
			return 1;
		}


		LogStatusInfo("End Lot Start");		//v2.93T2
		m_szLotEndTime.Format("%d-%d-%d;%d:%d:%d", 
							  theTime.GetDay(), theTime.GetMonth(), theTime.GetYear(),
							  theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());

		WriteProfileString(gszPROFILE_SETTING, gszLOT_END_TIME, m_szLotEndTime);
		m_smfSRam["MS896A"]["LotEndTime"] = m_szLotEndTime;
		m_bStartNewLot = FALSE;		//v2.71
		SaveAppData();


		/******/ 
		StartLoadingAlert(); /******/
		
		//-- Ask Binloader to unload last frame if exist --//	//andrew
		LogStatusInfo("Unload last frame");		//v2.93T2
		BOOL bBinLoader = (BOOL)(LONG)(m_smfSRam)["BinLoaderStn"]["Enabled"];
		if (bBinLoader)
		{
			nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "UnloadFilmFrame", rReqMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		bResult = FALSE;
		LogStatusInfo("Output lot summary");		//v2.93T2
		if (GetCustomerName() == "Avago")			//v4.48A21
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "OutputLotSummary_Avago", rReqMsg);
		else
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "OutputLotSummary", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	
		//Delete Wafer Record file & reset counter after Lot summary is generated
		if (bResult == TRUE)
		{
			LogStatusInfo("Reset all wafer die counts");		//v2.93T2
			
			BOOL bPrompt = FALSE;
			rReqMsg.InitMessage(sizeof(BOOL), &bPrompt);

			nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_ResetAllWaferDieCount", rReqMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

		
			szTitle.LoadString(HMB_MS_SYS_MESSAGE);
			szContent.LoadString(HMB_MS_LOT_FILE_GENERATED);

			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250);
		}

		LogStatusInfo("Clear map");		//v2.93T2
		bResult = FALSE;
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "EndLotClearMap", rReqMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		//v2.93T2
/*
		//v2.83T2
		//Reset lot data when end lot
		m_szBinWorkNo	= "";
		m_szLotNumber	= "";
		m_szUsername	= "";
		m_szMapSubFolderName = "";
		WriteProfileString(gszPROFILE_SETTING, gszBIN_WORK_NO, m_szBinWorkNo);
		m_smfSRam["MS896A"]["Bin WorkNo"] = m_szBinWorkNo;
		m_smfSRam["MS896A"]["LotNumber"] = m_szLotNumber;
		WriteProfileString(gszPROFILE_SETTING, gszMAP_SUBFOLDER_NAME, m_szMapSubFolderName);
		m_smfSRam["MS896A"]["MapSubFolderName"]	= m_szMapSubFolderName;
*/
		/******/ CloseLoadingAlert(); /******/
		SetStatusMessage("Current Lot is ended");
		LogStatusInfo("End Lot Done\n");		//v2.93T2
		SetErrorMessage("End Lot Done - " + m_szLotNumber);		//v2.99T1
	}

	 //4.52D10Run Open fnc of gen wft file
	if(m_bEnable2nd1DBarCode)
	{
		LONG lArmSelection=0;
		CStringList szList;
		CString szTempList;
		CString szTitle, szContent;
		CString szMzg;
		LONG lSet = 0;
		szList.AddTail("Yes");
		szList.AddTail("No");
		szContent = "Do you want to generate the Wafer label file?";
		szTitle =  "Wafer label file";
		lArmSelection = HmiSelection(szContent, szTitle, szList, lSet); 
		if( lArmSelection == 0)
		{
			m_bWaferLabelFile = TRUE;
			szMzg = "WaferLabel -- Comfired generating wafer label file";
		}
		else
		{
			m_bWaferLabelFile = FALSE;
			szMzg = "WaferLabel -- Cancel generating wafer label file";
		}
		
		CMSLogFileUtility::Instance()->WL_LogStatus(szMzg);
		HmiMessage(szMzg);
		WriteProfileInt(gszPROFILE_SETTING, gszENABLE_WAFER_LABEL_FILE, (int)m_bWaferLabelFile);


		//CString szSubFolder		= m_smfSRam["MS896A"]["MapSubFolderName"];
		CString szSubFolder			= m_smfSRam["MS896A"]["Bin WorkNo"];

		//Add subfolder if sub-folder exist
		if ( szSubFolder.IsEmpty() == FALSE )	
		{

			CString szMapPath = m_szMapFilePath + "\\" + szSubFolder;
			szMzg = "*WaferLabel -- Summary folder Path: " + szMapPath;
			CMSLogFileUtility::Instance()->WL_LogStatus(szMzg);
			HmiMessage(szMzg);
		}
		else
		{
			szMzg.Format("*WaferLabel -- Summary folder %s, no find target folder", m_szMapFilePath);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMzg);
			SetErrorMessage(szMzg);
			HmiMessage(szMzg);
		}

	}

	//Update lot is started or ended
	m_bNewLotStarted = bLotStarted;
	WriteProfileInt(gszPROFILE_SETTING, gszSTART_NEW_LOT, (int)m_bNewLotStarted);

	bLotStarted = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bLotStarted);
	return 1;
}


LONG CMS896AApp::SaveAndResetTime(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bReset = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bReset);

	if (m_eqMachine.IsEquipmentManager())
	{
		m_eqMachine.SetEMHourlyRecipe(m_szDeviceFile);
	}

	if (bReset == TRUE)
	{
		m_eqMachine.ResetAll();
		m_eqMachine2.ResetAll();	//WH Sanan	//v4.40T4
	}
	else
	{
		m_eqMachine.SaveAs ("C:\\MapSorter\\UserData\\Statistic\\LatestTime.txt");
		m_eqMachine2.SaveAs("C:\\MapSorter\\UserData\\Statistic\\LatestTime2.txt");	//HW Sanan	//v4.40T4
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::WaitingSetupButtom(IPC_CServiceMessage &svMsg)  //v4.51D5 SanAn(XA) 8
{

	CString szCode = m_smfSRam["MS896A"]["AlarmCode"];
	m_eqMachine.SetTime (FALSE, EQUIP_WAITING_SETUP_TIME, szCode);  //Stop WaitingSetTime
	CMSLogFileUtility::Instance()->MS_LogOperation("[WaitingSetupButtom] SetTime EQUIP_WAITING_SETUP_TIME with szCode: " + szCode);
	m_bWaitingSetTimeContainer  = FALSE; // Close Container
	m_szWaitingSetTimeAlarmMsg = szCode;


	CString szStartStopSetupTime = m_smfSRam["MS896A"]["StartStop Setup Time"]; 
	if(szStartStopSetupTime == "Start" )
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("[WaitingSetupButtom] szStartStopSetupTime == Start again  with Open again Setup Time");
		m_eqMachine.SetTime (TRUE, EQUIP_SETUP_TIME, szCode);   // Re-Start SetTime;
	}
	CMSLogFileUtility::Instance()->MS_LogOperation("Used [WaitingSetupButtom]");
	return TRUE;
}

LONG CMS896AApp::CompareFaultOperatorID(IPC_CServiceMessage &svMsg)  //v4.51D2 //Electech3E(DL) 8
{
	
	CStringArray szaOperatorID;
	CStringArray szaOperatorPassword;
	szaOperatorID.RemoveAll();
	szaOperatorPassword.RemoveAll();
	CStdioFile cfFile;
	CString szFilePath;
	CString szRead;
	CString szReadSingle;

	m_bRecoverGoBackScreen = FALSE;
	bool bFindIDResult = FALSE; 
	int i;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = FALSE;
	
	if( pApp -> GetCustomerName() == "Electech3E(DL)") // test name first
	{	
		//m_bFaultContainer   = TRUE; // open Operator Container

		CString szRecoverPasswordPath = m_szPasswordFilePath;
		if (szRecoverPasswordPath == "")
		{
			HmiMessage("ERROR: Password file path is null! Enter the Password file path in regedit");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else
		{

			szFilePath = szRecoverPasswordPath + "\\password.csv" ;
		}

		CreateDirectory(szRecoverPasswordPath, NULL);

		if(cfFile.Open(szFilePath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
		{
			HmiMessage("ERROR: Get password file failure - " + szFilePath);
			SetErrorMessage("ERROR: Get password file failure - " + szFilePath);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else
		{
			
			BOOL bTest = FALSE;		//v4.51A20	//Klocwork
			while (cfFile.ReadString(szRead))
			{
				if( szRead.Find("START") != -1)
				{
					bTest = TRUE;
					break;
				}
				else
				{
					bTest = FALSE;
				}
			}

			if( bTest == TRUE)
			{
				do
				{					
					if ( szRead.Find("Operator") != -1 )
					{
						int t = szRead.Find(",");
						szReadSingle = szRead.Left(t);
						szaOperatorID.Add(szReadSingle); 

						szRead = szRead.Mid(t+1);

						t = szRead.Find(",");
						szReadSingle = szRead.Left(t);
						szaOperatorPassword.Add(szReadSingle);

						cfFile.ReadString(szRead);
					}
					else
					{
						cfFile.ReadString(szRead);

					}


				}
				while(szRead != "");
			}
			else
			{
				HmiMessage("There are not START Header in Password file. Please use the right format!");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
			cfFile.Close();
		}

		UpdateHmiVariableData("MS_szFaultOperatorPassword", "");

		CString szPassword	= _T("");
		CString szTitle     = _T("Please input Password: ");
		BOOL bStatus = HmiStrInputKeyboard(szTitle, "MS_szFaultOperatorPassword", TRUE, szPassword);
		if( bStatus == FALSE)
		{
			HmiMessage("ERROR: password Login failure!");
			SetErrorMessage("ERROR: password Login failure - " + szFilePath);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		//HmiMessage("m_szFaultOperatorID: " + m_szFaultOperatorID);
		//HmiMessage("m_szFaultOperatorPassword: " + m_szFaultOperatorPassword);

		if( m_szFaultOperatorID == "" || szPassword == "")
		{
			HmiMessage("Please input Operator ID and Password");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		for(i = 0; i < szaOperatorID.GetSize(); i++)
		{
			if( szaOperatorID[i] == m_szFaultOperatorID &&  szaOperatorPassword[i] == szPassword) 
			{
				bFindIDResult = TRUE;
				break;
			}
			else
			{
				bFindIDResult = FALSE;

			}
		}
		
		if( bFindIDResult == TRUE )
		{
			m_bFaultContainer = FALSE;  // Close the Contrianer
			m_bRecoverContainer = TRUE; // Open the Engineer Recover Container
			m_bFaultButton = TRUE;
			GenerateFaultRecoverAlarmFile("Operator", m_szFaultOperatorID, ""); 
		}
		else
		{
			m_bFaultContainer = TRUE;
			HmiMessage("Please input the right Operator ID and Password");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}


		

		m_szFaultOperatorID = "";
		m_szFaultOperatorPassword = "";
		
	}
	else
	{ 
		m_bFaultContainer   = FALSE;
		m_bRecoverContainer = FALSE;
		m_bRecoverGoBackScreen = TRUE;
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::CompareRecoverEngineerID(IPC_CServiceMessage &svMsg) //v4.51D2 //Electech3E(DL) 9
{
	CStringArray szaCompareID;
	CStringArray szaEngineerID;
	CStringArray szaEngineerPassword;
	szaEngineerID.RemoveAll();
	szaEngineerPassword.RemoveAll();

	CStdioFile cfFile;
	CString szFilePath;
	CString szRead;
	CString szRemainder;
	CString szReadSingle;

	BOOL bReturn = FALSE;

	m_bRecoverGoBackScreen = FALSE;
	bool bFindIDResult = FALSE; 
	int i;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if( pApp -> GetCustomerName() == "Electech3E(DL)") // test name first
	{
		CString szRecoverPasswordPath = m_szPasswordFilePath;
		if (szRecoverPasswordPath == "")
		{
			HmiMessage("ERROR: password file path is null! Enter the Password file path");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else
		{
			szFilePath = szRecoverPasswordPath + "\\password.csv" ;
		}
		
		CreateDirectory(szRecoverPasswordPath, NULL);

		if(cfFile.Open(szFilePath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
		{
			HmiMessage("ERROR: Get password file failure!");
			SetErrorMessage("ERROR: Get password file failure - " + szFilePath);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
		else
		{
			
			BOOL bTest = FALSE;		//v4.51A20	//Klocwork
			while(cfFile.ReadString(szRead))
			{
				
				if( szRead.Find("START") != -1)
				{
					bTest = TRUE;
					break;
				}
				else
				{
					bTest = FALSE;
				}
			}

			if( bTest == TRUE)
			{
				do
				{					
					if ( szRead.Find("Engineer") != -1 )
					{
						int t = szRead.Find(",");
						szReadSingle = szRead.Left(t);
						szaEngineerID.Add(szReadSingle); 

						szRead = szRead.Mid(t+1);

						t = szRead.Find(",");
						szReadSingle = szRead.Left(t);
						szaEngineerPassword.Add(szReadSingle);

						cfFile.ReadString(szRead);
					}
					else
					{
						cfFile.ReadString(szRead);

					}


				}
				while(szRead != "");
			}
			else
			{
				HmiMessage("There are not START Header in Password file. Please use the right format!");
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;
			}
			cfFile.Close();
		}

		UpdateHmiVariableData("MS_szRecoverEnginnerPassword", "");

		CString szPassword	= _T("");
		CString szTitle     = _T("Please input Password: ");
		BOOL bStatus = HmiStrInputKeyboard(szTitle, "MS_szRecoverEnginnerPassword", TRUE, szPassword);
		if( bStatus == FALSE)
		{
			HmiMessage("ERROR: password Login failure!");
			SetErrorMessage("ERROR: password Login failure - " + szFilePath);
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}


		if( m_szRecoverEngineerID == "" || szPassword == "")
		{
			HmiMessage("Please input Engineer ID and Password");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}


		for(i = 0; i < szaEngineerID.GetSize(); i++)
		{
			if( szaEngineerID[i] == m_szRecoverEngineerID && szaEngineerPassword[i] == szPassword)
			{
				bFindIDResult = TRUE;
				break;
			}
		}
	
		if( bFindIDResult == TRUE)
		{
			m_bRecoverContainer	   = FALSE;  // Close the Contrainer
			m_bFaultButtonColor	   = FALSE;
			m_bRecoverButton	   = TRUE;
			m_bRecoverGoBackScreen = TRUE;

			if(m_bRecoverFinished == FALSE)
			{
				HmiMessage("Engineer Starts to check the alarm issue");
				GenerateFaultRecoverAlarmFile("Engineer", m_szRecoverEngineerID, "Start check the alarm");
			}
			else
			{
				HmiMessage("Engineer has fixed the alarm issue");
				GenerateFaultRecoverAlarmFile("Engineer", m_szRecoverEngineerID, "Alarm fixed by Engineer");

			}
		}
		else
		{
			m_bRecoverContainer = TRUE;
			m_bFaultButtonColor = TRUE;
			HmiMessage("Please input the right Engineer ID and Password");
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}

		

		m_szRecoverEngineerID = "";
		m_szRecoverEngineerPassword ="";
	}

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
LONG CMS896AApp::StartSetupTime(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bCycleStarted == TRUE)
	{
		//Stop log if in cycle
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	char *pBuffer;
	CString szCode;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szCode = &pBuffer[0];
	delete[] pBuffer;

	m_smfSRam["MS896A"]["StartStop Setup Time"] = "Start"; // v4.51D5 SanAn(XA) 9
	CMSLogFileUtility::Instance()->MS_LogOperation("Used [StartSetup Time] with [MS896A][StartStop Setup Time] = Start");


	if( m_bAWET_Triggered==FALSE )
	{
		m_eqMachine.SetTime (TRUE, EQUIP_SETUP_TIME, "", szCode); 
		m_eqMachine2.SetTime(TRUE, EQUIP_SETUP_TIME, "", szCode);	//WH Sanan	//v4.40T4
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::StopSetupTime(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bCycleStarted == TRUE)
	{
		//Stop log if in cycle
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	char *pBuffer;
	CString szCode;

	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	szCode = &pBuffer[0];
	delete[] pBuffer;

	
	m_smfSRam["MS896A"]["StartStop Setup Time"] = "Stop";   // v4.51D5 SanAn(XA) 10 
	CMSLogFileUtility::Instance()->MS_LogOperation("Used [StopSrtup Time] with [MS896A][StartStop Setup Time] = Stop");

	if( m_bAWET_Triggered==FALSE )
	{
		m_eqMachine.SetTime (FALSE, EQUIP_SETUP_TIME, "", szCode); 
		m_eqMachine2.SetTime(FALSE, EQUIP_SETUP_TIME, "", szCode);		//WH Sanan	//v4.40T4
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::SaveMachineData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	SaveAppData();

	m_eqMachine.SetReportFormat (m_ucEqTimeReportMode, m_bEqTimeAutoReset);

	//EquipTime2 must use Pre-Wafer setttings for WH Sanan
	UINT unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Equip Machine Time 2"), 0);
	if (unEnable==1 )		//WH Sanan	//v4.40T4
	{
		m_eqMachine2.SetReportFormat(4, m_bEqTimeAutoReset);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ClearSortSetupFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CStringMapFile  *psmf;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	
	//v4.53A9
	if (pUtl->LoadLastState("MS: ClearSortSetupFile") == FALSE)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	psmf = pUtl->GetLastStateFile("MS: ClearSortSetupFile");
	if (psmf != NULL)
	{
		(*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTSETUPFILE] = "";
	}

	pUtl->UpdateLastState("MS: ClearSortSetupFile");
	pUtl->CloseLastState("MS: ClearSortSetupFile");

	CString szContent;	
	szContent.LoadString(HMB_MS_BIN_NAME_RESET);
	HmiMessage(szContent);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::MultiLangSelections(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = FALSE;
	BOOL bNeedSwitch = TRUE;

	if (m_szMultiLanguage == "English")
	{
		m_lLangSelected = MS896A_ENGLISH;
	}

	if (m_szMultiLanguage == "Chinese Traditional")
	{
		m_lLangSelected = MS896A_CHINESE_TRADITIONAL;
	}

	if (m_szMultiLanguage == "Chinese Simplified")
	{
		m_lLangSelected = MS896A_CHINESE_SIMPLIFIED;
	}

	if (m_lLangPrevSelected == m_lLangSelected)
	{
		bNeedSwitch = FALSE;
	}

	m_lLangPrevSelected = m_lLangSelected;
	
	if (bNeedSwitch == TRUE)
	{
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(LONG), &m_lLangSelected);
		m_comClient.SendRequest("HmiUserService", "HmiLangSwitching", stMsg);
	}
	
	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CMS896AApp::MultiLangSelectionsToggle(IPC_CServiceMessage &svMsg)
{
	BOOL bReply;

	if (m_szMultiLanguage == "English")
	{
		m_lLangSelected = MS896A_CHINESE_TRADITIONAL;	
		m_szMultiLanguage = "Chinese Traditional";
	}
	else if (m_szMultiLanguage == "Chinese Traditional")
	{
		m_lLangSelected = MS896A_CHINESE_SIMPLIFIED;
		m_szMultiLanguage = "Chinese Simplified";
	}
	else if (m_szMultiLanguage == "Chinese Simplified")
	{
		m_lLangSelected = MS896A_ENGLISH;
		m_szMultiLanguage = "English";
	}

	m_lLangPrevSelected = m_lLangSelected;

	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(LONG), &m_lLangSelected);
	m_comClient.SendRequest("HmiUserService", "HmiLangSwitching", stMsg);

	MS_SaveLastState();

	bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CMS896AApp::SaveSelectedLanguage(IPC_CServiceMessage &svMsg)
{
	MS_SaveLastState();

	return 1;
}

LONG CMS896AApp::SaveReportStartEndTime(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	BOOL bSaveResult = TRUE;
	CString szContent, szTitle;

	if (m_lNoOfReportPeriod > EQUIP_NO_OF_RECORD_TIME)
	{
		m_lNoOfReportPeriod = EQUIP_NO_OF_RECORD_TIME;
	}

	SetErrorMessage("Start Save Report Start End Time");

	// to be update
	for (INT i = 0; i < m_lNoOfReportPeriod; i++)
	{
		bSaveResult = bSaveResult & ValidateReportTimeInput(m_szReportStartTime[i]) &
					  ValidateReportTimeInput(m_szReportEndTime[i]);
	}

	SetErrorMessage("After Validate Report Time Input");

	if (bSaveResult == FALSE)
	{
		szContent.LoadString(HMB_MS_REPORT_TIME_SAVE_FAIL);
		szTitle.LoadString(HMB_MS_REPORT_TIME_SETUP);

		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetErrorMessage("Start set the record time");

	UINT unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Equip Machine Time 2"), 0);
	for (INT i = 0; i < m_lNoOfReportPeriod; i++)
	{
		m_eqMachine.SetStartRecordTime(m_szReportStartTime[i], i);
		m_eqMachine.SetEndRecordTime(m_szReportEndTime[i], i);

		if( unEnable==0 )
		{
			m_eqMachine2.SetStartRecordTime(m_szReportStartTime[i], i);	//WH Sanan	//v4.40T4
			m_eqMachine2.SetEndRecordTime(m_szReportEndTime[i], i);		//WH Sanan	//v4.40T4
		}
	}

	SetErrorMessage("End set the record time");

	m_eqMachine.SetNoOfPeriod (m_lNoOfReportPeriod);
	if( unEnable==1 )
		m_eqMachine2.SetNoOfPeriod(0);	//WH Sanan	//v4.40T4
	else
		m_eqMachine2.SetNoOfPeriod(m_lNoOfReportPeriod);	//WH Sanan	//v4.40T4

	SetErrorMessage("Start Save AppData");
	
	SaveAppData();

	SetErrorMessage("Start Reset ALL");

	if (m_eqMachine.IsEquipmentManager())
	{
		m_eqMachine.SetEMHourlyRecipe(m_szDeviceFile);
	}

	if (HmiMessage("Reset machine statistics", "System", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		m_eqMachine.ResetAll();
		m_eqMachine2.ResetAll();	//WH Sanan	//v4.40T4

		SetErrorMessage("End Reset All");
	}
	else
	{
		CTime theTime = CTime::GetCurrentTime();
		m_eqMachine.UpdateUserDefineReportTime(theTime);
	}
	
	szTitle.LoadString(HMB_MS_REPORT_TIME_SETUP);
	szContent.LoadString(HMB_MS_REPORT_TIME_SETUP_SUCCESS);
	HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

// Set Machine Time Report Path Function
LONG CMS896AApp::SetMachineTimeReportPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	GetPath(m_szMachineTimeReportPath);

	SaveAppData();
	m_eqMachine.SetReportPath (m_szMachineTimeReportPath);
	m_eqMachine2.SetReportPath(m_szMachineTimeReportPath2);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SetMachineTimeReportPath2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	GetPath(m_szMachineTimeReportPath2);

	SaveAppData();
	m_eqMachine.SetReportPath (m_szMachineTimeReportPath);
	m_eqMachine2.SetReportPath(m_szMachineTimeReportPath2);		//WH Sanan	//v4.40T4
	m_smfSRam["MS896A"]["MachineTimeReportPath2"] = m_szMachineTimeReportPath2;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SetTimePerformancePathCmd(IPC_CServiceMessage &svMsg)
{
	BOOL bPath = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bPath);
	BOOL bReturn = TRUE;

	if( bPath )
	{
		GetPath(m_szTimePerformancePath);
		SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	}

	SaveAppData();

	m_eqMachine.SetTimePerformancePath(m_szTimePerformancePath, m_szMachineSerialNo);
	m_eqMachine2.SetTimePerformancePath(m_szTimePerformancePath, m_szMachineSerialNo);
	CreateDirectory(m_szTimePerformancePath, NULL);
	CreateDirectory(m_szTimePerformancePath + "\\PF", NULL);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	set machine time and performance path

LONG CMS896AApp::SetMachineReportPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	GetPath(m_szMachineReportPath);
	m_oToolsUsageRecord.SetRecordPath(m_szMachineReportPath);
	SaveAppData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SetMachineReportPath2(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	GetPath(m_szMachineReportPath2);
	m_oToolsUsageRecord.SetRecordPath2(m_szMachineReportPath2);
	SaveAppData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SetAlarmLampStatusReportPath(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	GetPath(m_szAlarmLampStatusReportPath);
	SaveAppData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::HmiPassword(IPC_CServiceMessage &svMsg)		//v4.17T7	//SanAn
{
	BOOL bReturn	= FALSE;

	BOOL bAdmin		= FALSE;			
	svMsg.GetMsg(sizeof(BOOL), &bAdmin);

	//No need to input password for Sanan ENGINEER login; use CheckPasswordFile() cmd in Warm Start instead
	if ( (GetCustomerName() == CTM_SANAN)			|| 
		 (GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG") ||	//v4.51A8
		 (GetCustomerName() == "Electech3E(DL)")	||
		 (GetCustomerName() == CTM_CREE)				||						//v4.52A14
		 (GetCustomerName() == "Epitop")			||
		 (GetCustomerName() == "ChangeLight(XM)")	||
		 (GetCustomerName() == "NSS"))									//v4.52A14
	{
		// THE SANAN PASSWORD NEW VERSION
		/*
		m_szUsername = "";
		*/
		bReturn = TRUE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	bReturn = CheckMSLoginPassword(bAdmin);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CMS896AApp::CheckMSLoginPassword(BOOL bAdmin)
{
	BOOL bReturn = FALSE;

	CString szTitle, szPassword;
	if (bAdmin)
	{
		szTitle	= _T("Please input ADMINISTRATOR password.");
	}
	else if (m_bTechnician)		//v4.38T5	//PLLM Lumiramic
	{
		szTitle	= _T("Please input TECHNICIAN password.");
	}
	else
	{
		szTitle	= _T("Please input ENGINEER password.");
	}


	//v4.28T6	//New SuperUser password
	CTime CurTime = CTime::GetCurrentTime();
	CString szSuperUserPassword;
	szSuperUserPassword.Format("andrew%d", CurTime.GetDay());


	szPassword	= _T("");
	BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginPassword", TRUE, szPassword);
	if (bStatus == TRUE)
	{
		if (bAdmin)	
		{
			if (szPassword == m_szAdminPassword)
			{
				m_bSuperUser = FALSE;	//v4.28T6
				bReturn = TRUE;
			}
			else if (szPassword == szSuperUserPassword)
			{
				m_bSuperUser = TRUE;	//v4.28T6
				bReturn = TRUE;
			}
			else
			{
				HmiMessage("You have entered an INVALID ADMIN password!!!", "INVALID PASSWORD");
			}
		}
		else
		{
			if (m_bTechnician && (szPassword == m_szTechnicianPassword))		//Technician
			{
				//&& (szPassword == 
				m_bSuperUser = FALSE;
				bReturn = TRUE;
			}
			else if (!m_bTechnician && (szPassword == m_szEngineerPassword))	//Engineer
			{
				m_bSuperUser = FALSE;	//v4.28T6
				bReturn = TRUE;
			}
			else if (szPassword == szSuperUserPassword)
			{
				m_bSuperUser = TRUE;	//v4.28T6
				bReturn = TRUE;
			}
			else
			{
				HmiMessage("You have entered an INVALID ENGINEER password!!!", "INVALID PASSWORD");
			}
		}
	}

	return bReturn;
}

LONG CMS896AApp::CheckPasswordFile(IPC_CServiceMessage &svMsg)			//	Warm Start
{
	BOOL bReturn = FALSE;

	if (GetCustomerName() == CTM_SANAN || GetCustomerName() == "ChangeLight(XM)" || GetCustomerName() == "NSS"  )				//v4.44A6
	{
		BOOL bCancel = FALSE;
		bReturn = CheckSananPasswordFile(FALSE, bCancel);
		if (bCancel == TRUE)
		{
			bReturn = FALSE;
		}
	}
	else if (GetCustomerName() == "Electech3E(DL)"	|| 
			 GetCustomerName() == "Epitop"			||							//v4.48A2
			 GetCustomerName() == CTM_CREE	)
	{
		bReturn = CheckSananPasswordFile_old(FALSE);
	}
	else if( (GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG") )
	{
		bReturn = CheckPasswordFile_SemitekZJG(FALSE);
	}
	else
	{
		bReturn = TRUE;
	}

	CheckUserLoginName(); // Matthew 20190125

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::CodeSanAnPasswordFile(IPC_CServiceMessage &svMsg)			//v4.17T7	//SanAn
{
	BOOL bReturn	= TRUE;
	// THE SANAN PASSWORD NEW VERSION
	if (GetCustomerName() == CTM_SANAN || GetCustomerName() == "ChangeLight(XM)" || GetCustomerName() == "NSS"  )
	{
		CString szPasswordFile	= m_szPasswordFilePath + "\\password.csv";
		CStdioFile oPFile;
		bReturn = oPFile.Open(szPasswordFile, CFile::modeRead | CFile::typeText );
		if ( !bReturn ) 
		{
			HmiMessage("ERROR: fail to open password database!", "Log In");
			SetErrorMessage("ERROR: fail to open password database - " + szPasswordFile);
		}
		else 
		{
			CString szLine;
			CString szCryptedPasswordFile		= m_szPasswordFilePath + "\\password_crypt.csv";
			CString szCryptedBackupPasswordFile	= gszUSER_DIRECTORY + "\\password_crypt.csv";
			CStdioFile oPFileCrypt;
			if ( _access( szCryptedBackupPasswordFile, 0) != -1 )
			{
				oPFileCrypt.Remove(szCryptedBackupPasswordFile);
			}
			if ( oPFileCrypt.Open(szCryptedBackupPasswordFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite  | CFile::typeText) == FALSE )
			{
				HmiMessage("ERROR: fail to crypt password database!", "Log In");
				SetErrorMessage("ERROR: fail to crypt password database - " + szCryptedPasswordFile);
			}
			else 
			{
				CString szHeader = "CARSYMPHTK";
				oPFileCrypt.WriteString( CodePassword(szHeader,4) );
				oPFileCrypt.WriteString("\n");
				while (oPFile.ReadString(szLine))
				{
					oPFileCrypt.WriteString( CodePassword(szLine,2) );
					oPFileCrypt.WriteString("\n");
				}
				oPFile.Close();
				oPFileCrypt.Close();
				//oPFile.Remove(szPasswordFile);
				CopyFile(szCryptedBackupPasswordFile, szCryptedPasswordFile, FALSE);
			}
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

//THE SANAN PASSWORD NEW VERSION (CRYPT DATABASE ONLY)

BOOL CMS896AApp::CheckSananPasswordFile(BOOL bInputUserID, BOOL &bCancel, LONG lAccessLevel)
{
	BOOL bReturn = FALSE;

	if ( (GetCustomerName() != CTM_SANAN) && (GetCustomerName() != _T("Epitop")) &&
		 (GetCustomerName() != _T("Electech3E(DL)")) && (GetCustomerName() != "ChangeLight(XM)")&&
		 (GetCustomerName() != "NSS"))		//v4.48A2
	{
		return TRUE;
	}

	if (m_szUsername.IsEmpty())
	{
		HmiMessage("LOGIN ERROR: Please enter user name.", "Log In");
		SetErrorMessage("LOGIN ERROR: EMPTY user name");
		return FALSE;
	}

	//v4.20
	BOOL bSuperUser = FALSE;

	//v4.28T6	//New SuperUser password
	CTime CurTime = CTime::GetCurrentTime();
	CString szSuperUserName	= "andrew";
	szSuperUserName.Format("andrew%d", CurTime.GetDay() + 1);		//v4.31T12

	UpdateHmiVariableData("szLoginID", "");
	UpdateHmiVariableData("szLoginPassword", "");

	CString szSuperUserPassword = "111111";
	if (!bInputUserID && (m_szUsername == szSuperUserName))
	{
		bSuperUser = TRUE;
	}

	CString szTitle			= _T("Sanan User Logon");
	CString szInputUserID	= _T("");
	CString szPassword		= _T("");
	CString szPasswordFile	= m_szPasswordFilePath + "\\password_crypt.csv";
	CString szCryptedBackupPasswordFile	= gszUSER_DIRECTORY + "\\password_crypt.csv";

	if (bInputUserID)
	{
		szTitle = _T("Please input valid user ID");
		BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginID", FALSE, szInputUserID);
		if (!bStatus)
		{
			HmiMessage("ERROR: user ID Login failure!", "Log In");
			SetErrorMessage("ERROR: user ID Login failure - " + szPasswordFile);
			bCancel = TRUE;
			return FALSE;
		}

		//v4.20
		if (szInputUserID == szSuperUserName)
		{
			bSuperUser = TRUE;
		}
	}


	if (((_access(szPasswordFile, 0)) == -1) && !bSuperUser)
	{
		if ( _access(szCryptedBackupPasswordFile, 0) == -1 )
		{
			HmiMessage("ERROR: fail to access password database!", "Log In");
			SetErrorMessage("ERROR: fail to access password database - " + szPasswordFile);
			return FALSE;
		}
		else 
		{
			szPasswordFile == szCryptedBackupPasswordFile;
		}
	}


	szTitle	= _T("Please input password");	
	BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginPassword", TRUE, szPassword);
	if (!bStatus)
	{
		HmiMessage("ERROR: password Login failure!", "Log In");
		SetErrorMessage("ERROR: password Login failure - " + szPasswordFile);
		bCancel = TRUE;
		return FALSE;
	}


	CString szTemp;
	szTemp = "User Logon: " + m_szUsername + " -- (" + "***" + ")";
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
//HmiMessage(szTemp);

	//v4.20
	if (bSuperUser && (szPassword == szSuperUserPassword))
	{
		return TRUE;
	}


	CStdioFile oPFile;
	if (!oPFile.Open(szPasswordFile, CFile::modeRead | CFile::typeText))
	{
		HmiMessage("ERROR: fail to open password database!", "Log In");
		SetErrorMessage("ERROR: fail to open password database - " + szPasswordFile);
		return FALSE;
	}


	CString szLine;
	oPFile.ReadString(szLine);

	szLine = CodePassword(szLine,-4);
	if ( szLine != "CARSYMPHTK" )
	{
		oPFile.Close();
		HmiMessage("ERROR: fail to open password database format ERROR!", "Log In");
		SetErrorMessage("ERROR: fail to open password database format ERROR- " + szPasswordFile);
		return FALSE;
	}


	CString szName, szOPPassword, szMode;
	BOOL bStart = FALSE;

	CString szUserIDToBeCompared = m_szUsername;
	if (bInputUserID)
	{
		szUserIDToBeCompared = szInputUserID;
	}

	BOOL bFileClose = FALSE;
	while (oPFile.ReadString(szLine))
	{
		szLine = CodePassword(szLine,-2);
		if (!bStart)
		{
			if (szLine.Find("[START]") != -1)
			{
				bStart = TRUE;
			}
			continue;
		}

		if (szLine.Find("[END]") != -1)
		{
			CString szLog;
			szLog = "LOGIN ERROR: User name (" + m_szUsername + ") is not found in database!  Please login again.";			
			SetErrorMessage(szLog);
			oPFile.Close();
			bFileClose = TRUE;
			HmiMessage(szLog, "Log In");
			break;
		}

		int nCol = szLine.Find(",");
		if( nCol!=-1 )
		{
			szName = szLine.Left(nCol);
			szLine = szLine.Mid(nCol + 1);
		}

		nCol = szLine.Find(",");
		if( nCol!=-1 )
		{
			szOPPassword = szLine.Left(nCol);
			szLine = szLine.Mid(nCol + 1);
		}

		szMode = szLine;
		szMode.Replace("\n", "");

//CString szTemp1;
//szTemp1 = "Name = " + szName + ", P = " + szOPPassword + ", MODE = " + szMode;
//AfxMessageBox(szTemp1, MB_SYSTEMMODAL);

		if (szUserIDToBeCompared == szName)
		{
			CString szLog;

			szMode = szMode.MakeLower();

			if (lAccessLevel == 3)			//ADMIN
			{
				if (szMode != "admin")
				{
					szLog = "ACCESS failure: (" + szUserIDToBeCompared + ") - incorrect access level (require ADMIN Mode)";
					SetErrorMessage(szLog);
					oPFile.Close(); // Matthew 20181219
					bFileClose = TRUE;
					HmiMessage(szLog, "Log In");
					break;
				}
			}
			else if (lAccessLevel == 2)		//ENGINEER
			{
				if ((szMode != "engineer") && (szMode != "admin"))
				{
					szLog = "ACCESS failure: (" + szUserIDToBeCompared + ") - incorrect access level (require ENGINEER Mode)";
					SetErrorMessage(szLog);
					oPFile.Close(); // Matthew 20181219
					bFileClose = TRUE;
					HmiMessage(szLog, "Log In");
					break;
				}
			}
			else if (lAccessLevel == 1)			//TECHNICIAN
			{
				if ((szMode != "engineer") && (szMode != "admin") && (szMode != "technician"))
				{
					szLog = "ACCESS failure: (" + szUserIDToBeCompared + ") - incorrect access level (require TECHNICIAN Mode)";
					SetErrorMessage(szLog);
					oPFile.Close(); // Matthew 20181219
					bFileClose = TRUE;
					HmiMessage(szLog, "Log In");
					break;
				}
			}
			else							//NONE -> for first-time login
			{
				if ((lAccessLevel == 0) && (szMode == "technician")) //Matthew 20181231
				{
					m_bTechnician = TRUE;
					m_bOperator = FALSE;
					lAccessLevel = 1;
				}
				
				if (m_bEngineer)
				{
					if ((szMode != "engineer") && (szMode != "admin"))
					{
						szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect user rights (Engineer Mode)";
						SetErrorMessage(szLog);
						oPFile.Close(); // Matthew 20181219
						bFileClose = TRUE;
						HmiMessage(szLog, "Log In");
						break;
					}
				}
				else if (m_bAdministrator)
				{
					if (szMode != "admin")
					{
						szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect user rights (ADMIN Mode)";
						SetErrorMessage(szLog);
						oPFile.Close(); // Matthew 20181219
						bFileClose = TRUE;
						HmiMessage(szLog, "Log In");
						break;
					}
				}
				else if (m_bTechnician)
				{
					if ((szMode != "engineer") && (szMode != "admin") && (szMode != "technician"))
					{
						szLog = "ACCESS failure: (" + szUserIDToBeCompared + ") - incorrect access level (require TECHNICIAN Mode)";
						SetErrorMessage(szLog);
						oPFile.Close(); // Matthew 20181219
						bFileClose = TRUE;
						HmiMessage(szLog, "Log In");
						break;
					}
				}
				else
				{
					//XM SanAn	//v4.43T9
					if (szMode != "operator")
					{
						szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect user rights (Operator Mode)";
						SetErrorMessage(szLog);
						oPFile.Close(); // Matthew 20181219
						bFileClose = TRUE;
						HmiMessage(szLog, "Log In");
						break;
					}
				}
			}

			if (szPassword == szOPPassword)
			{
				szLog = "LOGIN Success: (" + szUserIDToBeCompared + ")";
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				bReturn = TRUE;		//LOGIN success !!
				oPFile.Close();
			}
			else
			{
				szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect password!!";
				SetErrorMessage(szLog);
				oPFile.Close();
				HmiMessage(szLog, "Log In");
			}
			bFileClose = TRUE;
			break;
		}
	}

	if (!bFileClose)
	{
		oPFile.Close();
	}
	m_smfSRam["MS896A"]["PasswordID"] = szName;
	return bReturn;
}



// for "Electech3E(DL)"		"Epitop"	"Cree"
BOOL CMS896AApp::CheckSananPasswordFile_old(BOOL bInputUserID, LONG lAccessLevel)
{
	BOOL bReturn = FALSE;

	if ( (GetCustomerName() != _T("Electech3E(DL)"))	&& 
		 (GetCustomerName() != CTM_CREE)				&&		//v4.51A19
		 (GetCustomerName() != _T("Epitop")) )	//v4.44A6
	{
		return TRUE;
	}

	if (m_szUsername.IsEmpty())
	{
		HmiMessage("LOGIN ERROR: Please enter user name.", "Log In");
		SetErrorMessage("LOGIN ERROR: EMPTY user name");
		return FALSE;
	}

	//v4.20
	BOOL bSuperUser = FALSE;

	//v4.28T6	//New SuperUser password
	CTime CurTime = CTime::GetCurrentTime();
	CString szSuperUserName	= "andrew";
	szSuperUserName.Format("andrew%d", CurTime.GetDay() + 1);		//v4.31T12

	UpdateHmiVariableData("szLoginID", "");
	UpdateHmiVariableData("szLoginPassword", "");

	CString szSuperUserPassword = "111111";
	if (!bInputUserID && (m_szUsername == szSuperUserName))
	{
		bSuperUser = TRUE;
	}

	CString szTitle			= _T("Sanan User Logon");
	CString szInputUserID	= _T("");
	CString szPassword		= _T("");
	CString szPasswordFile	= m_szPasswordFilePath + "\\password.csv";

	if (bInputUserID)
	{
		szTitle = _T("Please input valid user ID");
		BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginID", FALSE, szInputUserID);
		if (!bStatus)
		{
			HmiMessage("ERROR: user ID Login failure!", "Log In");
			SetErrorMessage("ERROR: user ID Login failure - " + szPasswordFile);
			return FALSE;
		}

		//v4.20
		if (szInputUserID == szSuperUserName)
		{
			bSuperUser = TRUE;
		}
	}


	if (((_access(szPasswordFile, 0)) == -1) && !bSuperUser)
	{
		HmiMessage("ERROR: fail to access password database!", "Log In");
		SetErrorMessage("ERROR: fail to access password database - " + szPasswordFile);
		return FALSE;
	}

	szTitle	= _T("Please input password");	
	BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginPassword", TRUE, szPassword);
	if (!bStatus)
	{
		HmiMessage("ERROR: password Login failure!", "Log In");
		SetErrorMessage("ERROR: password Login failure - " + szPasswordFile);
		return FALSE;
	}


	CString szTemp;
	szTemp = "User Logon: " + m_szUsername + " -- (" + "***" + ")";
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
	SetStatusMessage("User Logon: " + m_szUsername);			//v4.46T15	//3E
//HmiMessage(szTemp);

	//v4.20
	if (bSuperUser && (szPassword == szSuperUserPassword))
	{
		return TRUE;
	}


	CStdioFile oPFile;
	if (!oPFile.Open(szPasswordFile, CFile::modeRead | CFile::typeText))
	{
		HmiMessage("ERROR: fail to open password database!", "Log In");
		SetErrorMessage("ERROR: fail to open password database - " + szPasswordFile);
		return FALSE;
	}


	CString szLine;
	CString szName, szOPPassword, szMode;
	BOOL bStart = FALSE;
	BOOL bWrongFormat = FALSE;

	CString szUserIDToBeCompared = m_szUsername;
	if (bInputUserID)
	{
		szUserIDToBeCompared = szInputUserID;
	}

	while (oPFile.ReadString(szLine))
	{
		if (!bStart)
		{
			if (szLine.Find("[START]") != -1)
			{
				bStart = TRUE;
			}
			continue;
		}

		if (szLine.Find("[END]") != -1 || szLine == "")
		{
			CString szLog;
			szLog = "LOGIN ERROR: User name (" + m_szUsername + ") is not found in database!  Please login again.";			
			SetErrorMessage(szLog);
			HmiMessage(szLog, "Log In");
			break;
		}

		if (szLine.Find(",") != -1)
			szName = szLine.Left(szLine.Find(","));
		else
			bWrongFormat = TRUE;

		if (szLine.Find(",") != -1)
			szLine = szLine.Mid(szLine.Find(",") + 1);
		else
			bWrongFormat = TRUE;

		if (szLine.Find(",") != -1)
			szOPPassword = szLine.Left(szLine.Find(","));
		else
			bWrongFormat = TRUE;

		if (szLine.Find(",") != -1)
			szLine = szLine.Mid(szLine.Find(",") + 1);
		else
			bWrongFormat = TRUE;

		if (bWrongFormat)
		{
			CString szLog;
			szLog = "The Password format wrong!Please Check!";			
			SetErrorMessage(szLog);
			HmiMessage(szLog, "Log In");
			break;
		}

		szMode = szLine;
		szMode.Replace("\n", "");

//CString szTemp1;
//szTemp1 = "Name = " + szName + ", P = " + szOPPassword + ", MODE = " + szMode;
//AfxMessageBox(szTemp1, MB_SYSTEMMODAL);

		if (szUserIDToBeCompared == szName)
		{
			CString szLog;

			szMode = szMode.MakeLower();

			if (lAccessLevel == 3)			//ADMIN
			{
				if (szMode != "admin")
				{
					szLog = "ACCESS failure: (" + szUserIDToBeCompared + ") - incorrect access level (require ADMIN Mode)";
					SetErrorMessage(szLog);
					HmiMessage(szLog, "Log In");
					break;
				}
			}
			else if (lAccessLevel == 2)		//ENGINEER
			{
				if ((szMode != "engineer") && (szMode != "admin"))
				{
					szLog = "ACCESS failure: (" + szUserIDToBeCompared + ") - incorrect access level (require ENGINEER Mode)";
					SetErrorMessage(szLog);
					HmiMessage(szLog, "Log In");
					break;
				}
			}
			else							//NONE -> for first-time login
			{
				if (m_bEngineer)
				{
					if ((szMode != "engineer") && (szMode != "admin"))
					{
						szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect user rights (Engineer Mode)";
						SetErrorMessage(szLog);
						HmiMessage(szLog, "Log In");
						break;
					}
				}
				else if (m_bAdministrator)
				{
					if (szMode != "admin")
					{
						szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect user rights (ADMIN Mode)";
						SetErrorMessage(szLog);
						HmiMessage(szLog, "Log In");
						break;
					}
				}
				else
				{
					//XM SanAn	//v4.43T9
					if (szMode != "operator")
					{
						szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect user rights (Operator Mode)";
						SetErrorMessage(szLog);
						HmiMessage(szLog, "Log In");
						break;
					}
				}
			}

			if (szPassword == szOPPassword)
			{
				szLog = "LOGIN Success: (" + szUserIDToBeCompared + ")";
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				bReturn = TRUE;		//LOGIN success !!
			}
			else
			{
				szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect password!!";
				SetErrorMessage(szLog);
				HmiMessage(szLog, "Log In");
			}
			break;
		}
	}

	oPFile.Close();
	return bReturn;
}

BOOL CMS896AApp::CheckSananPasswordFile_XMChangeLight()
{
	BOOL bReturn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CMSLogFileUtility::Instance()->MS_LogOperation("CheckPwdFile (XMChangeLight) START");

	BOOL bSuperUser = FALSE;

	//New SuperUser password
	CTime CurTime = CTime::GetCurrentTime();
	CString szSuperUserName	= "andrew";
	szSuperUserName.Format("andrew%d", CurTime.GetDay() + 1);	

	UpdateHmiVariableData("szLoginID", "");
	UpdateHmiVariableData("szLoginPassword", "");

	CString szSuperUserPassword = "111111";

	CString szTitle			= _T("ChangeLight User Logon");
	CString szInputUserID	= _T("");
	CString szPassword		= _T("");
	CString szPasswordFile	= pApp->GetPasswordFilePath() + "\\password_crypt.csv";
	CString szCryptedBackupPasswordFile	= gszUSER_DIRECTORY + "\\password_crypt.csv";

	szTitle = _T("Please input valid user ID");
	BOOL bStatus = HmiStrInputKeyboard_OnlyContent(szTitle, szInputUserID);
	if (!bStatus)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("CheckPwdFile (XMChangeLight) : invalid User ID");
		SetErrorMessage("ERROR: user ID Login failure - " + szInputUserID);
		HmiMessage("ERROR: user ID Login failure!", "Log In");
		return FALSE;
	}

	if (szInputUserID == szSuperUserName)
	{
		bSuperUser = TRUE;
	}

	if (!bSuperUser)	//v4.62A21
	{
		if (_access(szPasswordFile, 0) != -1)
		{
			CopyFile(szPasswordFile, szCryptedBackupPasswordFile, FALSE);
		}
		if (_access(szCryptedBackupPasswordFile, 0) == -1)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("CheckPwdFile (XMChangeLight) : invalid database");
			HmiMessage("ERROR: fail to access password database!", "Log In");
			SetErrorMessage("ERROR: fail to access password database - " + szCryptedBackupPasswordFile);
			return FALSE;
		}
	}

	szTitle	= _T("Please input password");	
	bStatus = HmiStrInputKeyboard_OnlyContent(szTitle, szPassword, TRUE);	//v4.62A23
	if (!bStatus)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("CheckPwdFile (XMChangeLight) : invalid password");
		HmiMessage("ERROR: password Login failure!", "Log In");
		SetErrorMessage("ERROR: password Login failure - " + szPassword);
		return FALSE;
	}


	CString szTemp;
	szTemp = "User Logon: " + szInputUserID + " -- (" + "***" + ")";
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
//HmiMessage(szTemp);

	if (bSuperUser && (szPassword == szSuperUserPassword))
	{
		szTemp = "LOGIN Success (Super User) : (" + szInputUserID + ")";
		CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		return TRUE;
	}


	CStdioFile oPFile;
	if (!oPFile.Open(szCryptedBackupPasswordFile, CFile::modeRead | CFile::typeText))
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("CheckPwdFile (XMChangeLight) : fail to OPEN database");
		HmiMessage("ERROR: fail to open password database!", "Log In");
		SetErrorMessage("ERROR: fail to open password database - " + szCryptedBackupPasswordFile);
		return FALSE;
	}


	CString szLine;
	oPFile.ReadString(szLine);

	szLine = pApp->CodePassword(szLine,-4);
	if ( szLine != "CARSYMPHTK" )
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("CheckPwdFile (XMChangeLight) : fail to OPEN database format");
		HmiMessage("ERROR: fail to open password database format ERROR!", "Log In");
		SetErrorMessage("ERROR: fail to open password database format ERROR- " + szCryptedBackupPasswordFile);
		return FALSE;
	}


	CString szName, szOPPassword, szMode, szLog;
	BOOL bStart = FALSE;
	CString szUserIDToBeCompared = szInputUserID;

	szTemp.Format("User ID to be searched in database : %s (%s)", szUserIDToBeCompared, szInputUserID);
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

	while (oPFile.ReadString(szLine))
	{
		szLine = pApp->CodePassword(szLine, -2);
		if (!bStart)
		{
			if (szLine.Find("[START]") != -1)
			{
				bStart = TRUE;
			}
			continue;
		}

		if (szLine.Find("[END]") != -1)
		{
			CString szLog;
			szLog = "LOGIN ERROR: User name (" + szUserIDToBeCompared + ") is not found in database!  Please login again.";			
			CMSLogFileUtility::Instance()->MS_LogOperation("CheckPwdFile (XMChangeLight) : fail to find user in database");
			SetErrorMessage(szLog);
			HmiMessage(szLog, "Log In");
			bReturn = FALSE;		//LOGIN FAIL !!
			break;
		}

		int nCol = szLine.Find(",");
		if( nCol!=-1 )
		{
			szName = szLine.Left(nCol);
			szLine = szLine.Mid(nCol + 1);
		}

		nCol = szLine.Find(",");
		if( nCol!=-1 )
		{
			szOPPassword = szLine.Left(nCol);
			szLine = szLine.Mid(nCol + 1);
		}

		szMode = szLine;
		szMode.Replace("\n", "");

//CString szTemp1;
//szTemp1 = "Name = " + szName + ", P = " + szOPPassword + ", MODE = " + szMode;
//AfxMessageBox(szTemp1, MB_SYSTEMMODAL);

		if (szUserIDToBeCompared == szName)
		{
			if (szPassword == szOPPassword)
			{
				szLog = "LOGIN Success: (" + szUserIDToBeCompared + ")";
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				bReturn = TRUE;		//LOGIN success !!
			}
			else
			{
				bReturn = FALSE;		//LOGIN FAIL !!
				szLog = "LOGIN failure: (" + szUserIDToBeCompared + ") - incorrect password!!";
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				SetErrorMessage(szLog);
				HmiMessage(szLog, "Log In");
			}
			break;
		}
	}

	//m_smfSRam["MS896A"]["PasswordID"] = szName;
	oPFile.Close();
	szTemp.Format("CheckPwdFile (XMChangeLight) FINISH = %d", bReturn);
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

	//v4.62A22
	if (bReturn)
	{
		szTemp.Format("Parameter is changed, OP intput ID: %s", szInputUserID);
		m_smfSRam["MS896A"]["Operator Id"] = szInputUserID;
		SetStatusMessage(szTemp);
	}

	
	return 1;
}

BOOL CMS896AApp::CheckSananPasswordFile_old_3EDL()
{
	BOOL bReturn = FALSE;

	if (GetCustomerName() != _T("Electech3E(DL)"))
	{
		return TRUE;
	}

	BOOL bSuperUser = FALSE;

	CTime CurTime = CTime::GetCurrentTime();
	CString szSuperUserName	= "andrew";
	szSuperUserName.Format("andrew%d", CurTime.GetDay() + 1);		//v4.31T12

	UpdateHmiVariableData("szLoginID", "");
	UpdateHmiVariableData("szLoginPassword", "");

	CString szSuperUserPassword = "111111";
	if (m_szUsername == szSuperUserName)
	{
		bSuperUser = TRUE;
	}

	CString szTitle			= _T("3E User Logon");
	CString szInputUserID	= _T("");
	CString szPassword		= _T("");
	CString szPasswordFile	= m_szPasswordFilePath + "\\password.csv";

	if (!bSuperUser)
	{
		szTitle = _T("Please input valid user ID");
		BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginID", FALSE, szInputUserID);
		if (!bStatus)
		{
			HmiMessage("ERROR: user ID Login failure!", "Log In");
			SetErrorMessage("ERROR: user ID Login failure - " + szPasswordFile);
			return FALSE;
		}

		if (szInputUserID == szSuperUserName)
		{
			bSuperUser = TRUE;
		}
	}


	if (((_access(szPasswordFile, 0)) == -1) && !bSuperUser)
	{
		HmiMessage("ERROR: fail to access password database!", "Log In");
		SetErrorMessage("ERROR: fail to access password database - " + szPasswordFile);
		return FALSE;
	}


	szTitle	= _T("Please input password");	
	BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginPassword", TRUE, szPassword);
	if (!bStatus)
	{
		HmiMessage("ERROR: password Login failure!", "Log In");
		SetErrorMessage("ERROR: password Login failure - " + szPasswordFile);
		return FALSE;
	}


	CString szTemp;
	szTemp = "User Validation (3EDL): " + szInputUserID + " -- (" + szPassword + ")";
	CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);


	if (bSuperUser && (szPassword == szSuperUserPassword))
	{
		return TRUE;
	}


	CStdioFile oPFile;
	if (!oPFile.Open(szPasswordFile, CFile::modeRead | CFile::typeText))
	{
		HmiMessage("ERROR: fail to open password database!", "Log In");
		SetErrorMessage("ERROR: fail to open password database - " + szPasswordFile);
		return FALSE;
	}


	CString szLine;
	CString szName, szOPPassword, szMode;
	BOOL bStart = FALSE;

	CString szUserIDToBeCompared = m_szUsername;
	szUserIDToBeCompared = szInputUserID;

	while (oPFile.ReadString(szLine))
	{
		if (!bStart)
		{
			if (szLine.Find("[START]") != -1)
			{
				bStart = TRUE;
			}
			continue;
		}

		if (szLine.Find("[END]") != -1)
		{
			CString szLog;
			szLog = "LOGIN ERROR: User name (" + szUserIDToBeCompared + ") is not found in database!  Please login again.";			
			SetErrorMessage(szLog);
			HmiMessage(szLog, "Log In");
			break;
		}

		szName = szLine.Left(szLine.Find(","));
		szLine = szLine.Mid(szLine.Find(",") + 1);

		szOPPassword = szLine.Left(szLine.Find(","));
		szLine = szLine.Mid(szLine.Find(",") + 1);

		szMode = szLine;
		szMode.Replace("\n", "");

		if (szUserIDToBeCompared == szName)
		{
			CString szLog;

			szMode = szMode.MakeLower();

			if ((szMode != "admin") && (szMode != "engineer"))
			{
				szLog = "VALIDATE failure: (" + szUserIDToBeCompared + ") - incorrect access level (require ADMIN or ENGR Mode)";
				SetErrorMessage(szLog);
				HmiMessage(szLog, "Log In");
				break;
			}


			if (szPassword == szOPPassword)
			{
				szLog = "VALIDATE Success: (" + szUserIDToBeCompared + ")";
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
				bReturn = TRUE;		//LOGIN success !!
			}
			else
			{
				szLog = "VALIDATE failure: (" + szUserIDToBeCompared + ") - incorrect password!!";
				SetErrorMessage(szLog);
				HmiMessage(szLog, "Log In");
			}
			break;
		}
	}

	oPFile.Close();
	return bReturn;
}


LONG CMS896AApp::OperatorLogOnValidation(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bOperatorLogOnValidation == TRUE && m_bOperator == TRUE)
	{
		if (m_szUsername.IsEmpty() == TRUE)
		{
			CString szContent, szTitle;
			szContent.LoadString(HMB_MS_OPERATOR_VAILDATION_ERR);
			szTitle.LoadString(HMB_MS_OPERATOR_VAILDATION);

			//AfxMessageBox("Cannot Log On Without Operator ID", MB_SYSTEMMODAL);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, 
					   glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

			bReturn = FALSE;
		}
	}

	if (m_bOperator == TRUE)
	{	
		if (m_szUsername.IsEmpty() == TRUE)
		{
			//SetErrorMessage("Operator ID: N/A Log On");
			CMSLogFileUtility::Instance()->MS_LogOperation("Operator ID: N/A LogOn");
		}
		else
		{
			//SetErrorMessage("Operator ID:" + m_szUsername + " Log On");
			CMSLogFileUtility::Instance()->MS_LogOperation("Operator ID: " + m_szUsername + " LogOn");
		}
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ClearOperatorId(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bOperatorLogOnValidation == TRUE && m_bOperator == TRUE)
	{
		m_szUsername.Empty();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}

LONG CMS896AApp::HmiCornerSearchHomeDie(IPC_CServiceMessage &svMsg)	//	426TX	1	now for auto align button
{
	BOOL bReturn = TRUE;
	BOOL	bIsEnableWaferLoader = (BOOL)((LONG)m_smfSRam["WaferLoaderStn"]["Enabled"]);	
	IPC_CServiceMessage stMsg;
	int nConvID;

	BOOL bNewAuto = FALSE;	//	426TX	2	auto align button improvement
	int nConvIDAsk = m_comClient.SendRequest(WAFER_TABLE_STN, "IsAutoAlignElCornerWafer", stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvIDAsk, 500000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvIDAsk, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bNewAuto);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bNewAuto)
	{

		// going to 2nd part or sorting 2nd part, when press the auto align button, do the fully auto align
		if( CMS896AStn::m_bMS90HalfSortMode && CMS896AStn::m_b2PartsAllDone==FALSE && (CMS896AStn::m_bSortGoingTo2ndPart || CMS896AStn::m_b2Parts1stPartDone) )
		{
				nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoAlign2PhasesSortSecondPart", stMsg);
		}
		else
		{
			if (bIsEnableWaferLoader)	// wafer loader just follow fully cycle auto alignment method
			{
				nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "AutoAlignFrameWafer", stMsg);
			}
			else						// wafer taber just follow fully auto alignment method
			{
				nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoAlignStandardWafer", stMsg);
			}
		}
	}
	else
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoAlignWafer", stMsg);
	}
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	//CMS896AStn *pStn;
	if (pWaferPr != NULL)
	{
		if (pWaferPr->IsWaferRotated180() && !pWaferPr->IsMS90Sorting2ndPart() ||
			!pWaferPr->IsWaferRotated180() && pWaferPr->IsMS90Sorting2ndPart())
		{
			SetErrorMessage("Orientation not match when auto align!");
			HmiMessage("Orientation not match!!!");
			bReturn = FALSE;
		}
	}

	if (bReturn)
	{
		HmiMessage("Align wafer complete.", "Auto Align");
	}
	else
	{
		HmiMessage("Align wafer failure.", "Auto Align");
	}
	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::IsOPMenuEnabled(IPC_CServiceMessage &svMsg)			//v4.50A11
{
	BOOL bReturn = TRUE;

	if (!m_bEnableOPMenuShortcut)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::IsUnderAutoCycle(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	m_bCycleNotStarted = !m_bCycleStarted;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	for Op Menu, exit screen.

// if cree not to access when it is operator mode
LONG CMS896AApp::CreeCheckAccessRight(IPC_CServiceMessage &svMsg)
{	
	// refer to check access right function
	BOOL bReturn = TRUE;

	if (GetCustomerName() == CTM_CREE)
	{
		if (m_bOperator)
		{
			bReturn = CheckPasswordWithTitle(m_szEngineerPassword, "Please Enter the Engineer Password");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ManualGenerateParametersRecord(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	GenerateParametersRecord();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::SaveGenericDeviceFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReply);

	return 1;
}	// end save generic package file

//v4.59A15	//Osram Penang
LONG CMS896AApp::CheckGenericDeviceFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReply = TRUE;
	CString szMsg;
	
	char *pBuffer;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	CString szPRMFileName = &pBuffer[0];
	delete [] pBuffer;

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t\t<<< -- [CheckGenericDeviceFile] PKG file name: " + szPRMFileName); 

	CString szFullPath = m_szDevicePath + "\\" + szPRMFileName;

	if (szPRMFileName.GetLength() == 0)
	{
		bReply = FALSE;
	}
	else if (_access(szFullPath, 0) == -1)
	{
		bReply = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;

}	// end save generic package file


LONG CMS896AApp::LoadGenericDeviceFile(IPC_CServiceMessage &svMsg) //4.51D1 SecsGem
{
	BOOL bReturn = TRUE;
	CString szMsg;
	
	char *pBuffer;
	pBuffer = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pBuffer);

	CString szPRMFileName = &pBuffer[0];
	delete [] pBuffer;

	if (szPRMFileName.Find(".ppkg") == -1)
	{
		//szPRMFileName = szPRMFileName + ".ppkg";
	}


	CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t\t\t<<< -- [LoadGenericDeviceFile] Load PKG file name: " + szPRMFileName); 


	m_bPortablePKGFile	= TRUE;
	m_szDeviceFile		= szPRMFileName;
	bReturn = LoadPKGFile(FALSE, m_szDeviceFile, FALSE, FALSE, TRUE);
	m_smfSRam["MS896A"]["Load PKG File"] = bReturn;  // protect the Start Remote Command
	CString szStatus;
	szStatus.Format("\t\t\t<<< -- [LoadGenericDeviceFile] Load PKG file status = %d", bReturn);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szStatus);
	//HmiMessage(szStatus);

	//OsramTrip 8/22
    if ( (CMS896AStn::m_pGemStation != NULL) && (CMS896AStn::m_bIsSecsGemInit == TRUE) )
	{
		CString szPKGFileName = m_szDeviceFile;
		(*CMS896AStn::m_pGemStation)[MS_SECS_SV_CURR_RECIPE_NAME]	= szPKGFileName;
		CMS896AStn::m_pGemStation->SendEvent(SG_CEID_MC_WARMSTART_COMPLETED);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


HHOOK	CMS896AApp::m_hKeyboardHook	= NULL;		//v4.08
CString CMS896AApp::m_szKeyboard	= _T("");	//v4.38T1
BOOL	CMS896AApp::m_bEnableKeyboardHook	= FALSE;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG *msg;
	msg = (MSG*) lParam;

	if (nCode < 0)
	{
		return CallNextHookEx(CMS896AApp::m_hKeyboardHook, nCode, wParam, lParam);
	}
	else //if (nCode == HC_ACTION)
	{
		return 1;	//do not pass message further 
	}

	return 0;	//pass message to rest of system
}

LONG CMS896AApp::EnableKeyboard(IPC_CServiceMessage &svMsg)		//v4.08
{
	BOOL bEnable = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bEnable);

	if (GetCustomerName() == CTM_SANAN && m_bOperator == TRUE && GetProductLine() == "XA")
	{
		HmiMessage("Block Keyboard in OP Mode!");
		return TRUE;
	}

	if (!bEnable)
	{
		m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) KeyboardProc, AfxGetApp()->m_hInstance, 0);	//GetCurrentThreadId());
		if (m_hKeyboardHook == NULL)
		{
			CString szErr;
			szErr.Format("Andrew: Keyboard HOOK fails! = %d", GetLastError());
			AfxMessageBox(szErr, MB_SYSTEMMODAL);
			m_bIsKeyboardLocked = FALSE;
		}
		else
		{
			m_bIsKeyboardLocked = TRUE;
		}
	}
	else
	{
		if (m_hKeyboardHook != NULL)
		{
			UnhookWindowsHookEx(m_hKeyboardHook);
			m_hKeyboardHook = NULL;
		}

		m_bIsKeyboardLocked = FALSE;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::ExportNVRAMDataToFile(IPC_CServiceMessage &svMsg)
{
	BOOL bStatus = ExportNVRAMDataToTxtFile(_T("c:\\MapSorter\\Exe\\NVRAM.bin"));
	if (bStatus)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM data is exported to NVRAM.bin");		//v3.55
		HmiMessage("NVRAM file is exported to: c:\\MapSorter\\Exe\\NVRAM.bin");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


BOOL CMS896AApp::ExportNVRAMDataToTxtFile(CONST CString szFileName)	//v4.40T15	//Cree HuiZhou
{
	LONG *plSW, i;

	//v4.43T12
	if (_access(szFileName, 0) != -1)
	{
		CopyFile(szFileName, "c:\\MapSorter\\Exe\\NVRAM_backup.bin", FALSE);
	}

	if (m_pvNVRAM != NULL)
	{
		BOOL bStatus = m_VNRAM_SW.Open(szFileName, 
						CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeText);
		if (bStatus)
		{
			plSW = (LONG*) m_pvNVRAM;
			CString szLine;

			for (i = 0; i < NVRAM_8K; i++)
			{
				szLine.Format("%d\n", *(plSW + i));
				m_VNRAM_SW.WriteString(szLine);
			}

			m_VNRAM_SW.Close();
			return TRUE;
		}
	}

	return FALSE;
}


LONG CMS896AApp::ImportNVRAMDataFromFile(IPC_CServiceMessage &svMsg)
{
	BOOL bStatus = ImportNVRAMDataFromTxtFile(_T("c:\\MapSorter\\Exe\\NVRAM.bin"));
	if (bStatus)
	{
		HmiMessage("NVRAM file is imported from: c:\\MapSorter\\Exe\\NVRAM.bin");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CMS896AApp::ImportNVRAMDataFromTxtFile(CONST CString szFileName)
{
	LONG *plSW, i;

	BOOL bStatus = m_VNRAM_SW.Open(szFileName,
		CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText);
	
	if (bStatus)
	{
		plSW = (LONG*)m_pvNVRAM;
		CString szLine;

		for (i = 0; i < NVRAM_8K; i++)
		{
			//*(plSW+i) = *(plHW+i);
			if (!m_VNRAM_SW.ReadString(szLine))
			{
				break;
			}
			*(plSW + i) = atol(szLine);
		}

		m_VNRAM_SW.Close();
		return TRUE;
	}

	return FALSE;
}

LONG CMS896AApp::CheckOpClearBinCounterAcMode(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (m_bAcClearBinCounter)
	{
		HmiMessage("OP Mode can not clear bin counter manually!", "Bond");
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CMS896AApp::SetWideScreenMode(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bUseWideScreen)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	DisplayMessage("Wide Screen Mode is used");

	IPC_CServiceMessage sv;
	BOOL bEnable = TRUE;
	sv.InitMessage(sizeof(BOOL), &bEnable);
	m_comClient.SendRequest("HmiUserService", "HmiAutoShowTopAndFunc", sv, 0);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::BurnInSetup(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::CheckDiscUsage(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if ( m_dWarningEmptySpacePercent != 0 && m_cDiskLabel >= 'C' && m_cDiskLabel <= 'Z' )
	{
		ULONG lDiskSpaceUsed, lDiskSpaceRemain;
		CString szDiskLabel;
		szDiskLabel.Format( "%c:" , m_cDiskLabel );

		if ( CMS896AStn::CheckDiskSize(szDiskLabel, lDiskSpaceUsed, lDiskSpaceRemain) )
		{
			CString szText;
			szText.Format( "%c Drive Used %dMB, remain %dMB.", m_cDiskLabel, lDiskSpaceUsed, lDiskSpaceRemain);
			HmiMessage(szText);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

//	HMI access level
BOOL CMS896AApp::CheckPasswordWithTitle(CString szPassword, CString szTitle)
{
	BOOL bReturn = FALSE;
	CHAR acPar[400], *pTemp;
	ULONG ulSize;
	IPC_CServiceMessage stMsg;

	strcpy_s(acPar, sizeof(acPar), szTitle);
	ulSize = (ULONG)strlen(acPar) + 1;
	pTemp = acPar + ulSize;
	strcpy_s(pTemp, sizeof(acPar) - ulSize, szPassword);
	ulSize += (ULONG)strlen(pTemp) + 1;

	stMsg.InitMessage(ulSize, acPar);
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiPassword", stMsg);

	// Get the reply
	while (m_comClient.ScanReplyForConvID(nConvID, 500) == 0);

	m_comClient.ReadReplyForConvID(nConvID, stMsg);
	stMsg.GetMsg(sizeof(BOOL), &bReturn);
	Sleep(200);

	return bReturn;
}

//	To enable the , you need to have a security file. 
//	You can right click the top page->Menu Tree->Security Setting.
//	It will list out all “input?controls. You can save the file. 
//	support the security file is under c:\mapsorter\config, 
//	named with AdmSecurity.txt; EngSecurity.txt; TecSecurity.txt; OpeSecurity.txt
LONG CMS896AApp::LoadHmiSecuritySettings()
{
	CString szAdmin	= m_szHmiAccessPwAdmin;
	CString szServi	= m_szHmiAccessPwService;
	CString szSuper	= m_szHmiAccessPwSuper;
	CString szEngin	= m_szHmiAccessPwEngineer;
	CString szTechn	= m_szHmiAccessPwTechnician;
	IPC_CServiceMessage svMsg;

	CString szFileName	= HMI_SECURITY_FILE;

	BOOL bEnable = TRUE;
	ULONG ulIndex = 0;
	char pBuffer[1000];

	strcpy_s(&pBuffer[ulIndex], sizeof(pBuffer) - ulIndex, (LPCTSTR)szFileName);
	ulIndex += (szFileName.GetLength() + 1);

	strcpy_s(&pBuffer[ulIndex], sizeof(pBuffer) - ulIndex, (LPCTSTR)szAdmin);
	ulIndex += (szAdmin.GetLength() + 1);
	strcpy_s(&pBuffer[ulIndex], sizeof(pBuffer) - ulIndex, (LPCTSTR)szServi);
	ulIndex += (szServi.GetLength() + 1);
	strcpy_s(&pBuffer[ulIndex], sizeof(pBuffer) - ulIndex, (LPCTSTR)szSuper);
	ulIndex += (szSuper.GetLength() + 1);
	strcpy_s(&pBuffer[ulIndex], sizeof(pBuffer) - ulIndex, (LPCTSTR)szEngin);
	ulIndex += (szEngin.GetLength() + 1);
	strcpy_s(&pBuffer[ulIndex], sizeof(pBuffer) - ulIndex, (LPCTSTR)szTechn);
	ulIndex += (szTechn.GetLength() + 1);

	memcpy(&pBuffer[ulIndex], &bEnable, sizeof(BOOL));
	ulIndex += sizeof(BOOL);

	svMsg.InitMessage(ulIndex, pBuffer);
	int nConvID = m_comClient.SendRequest("HmiUserService", _T("HmiSecurity"), svMsg, 0);
	SetHmiSecurityUpperLowerLimit();
	return TRUE;
}


LONG CMS896AApp::ChangeHmiSecurityLevel(LONG lLogInLevel)
{
//	based on login level, to change the password check security level
	CString szPassword = "@@@@@@@@";
	switch( lLogInLevel )
	{
	case 0:	// admin
		szPassword = m_szHmiAccessPwAdmin;
		break;
	case 1:	// Servi
		szPassword = m_szHmiAccessPwService;
		break;
	case 2:	// Super
		szPassword = m_szHmiAccessPwSuper;
		break;
	case 3:	// Engin
		szPassword = m_szHmiAccessPwEngineer;
		break;
	case 4:	// Techn
		szPassword = m_szHmiAccessPwTechnician;
		break;
	default:
		szPassword = "@@@@@@@@";
		break;
	}
	m_lInUseAccessLevel	= lLogInLevel;
	m_lHmiAccessLevel	= lLogInLevel;

	int nIndex = 0;
	char cTemp[1000];
	strcpy_s(&cTemp[nIndex], sizeof(cTemp) - nIndex, szPassword);
	nIndex += (szPassword.GetLength() + 1);
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(nIndex, cTemp);
	int nConvID = m_comClient.SendRequest("HmiUserService", "HmiSetCurrentSecurityLevel", stMsg, 0);
	return TRUE;
}

//	To change the level role display name.
LONG CMS896AApp::ChangeHmiSecurityRoleName()
{
	CString AdminRoleName	= "Administrator";
	CString ServiceRoleName	= "Technical";
	CString SuperRoleName	= "Adv. Prod";
	CString EngineRoleName	= "Production";
	CString TechRoleName	= "Setup";
	CString OperRoleName	= "Operator";

	int nIndex = 0;
	char cTemp[1000];
	nIndex = 0;
	memset(cTemp, NULL, 1000);
	strcpy_s(&cTemp[nIndex], sizeof(cTemp) - nIndex, AdminRoleName);
	nIndex += (AdminRoleName.GetLength() + 1);
	strcpy_s(&cTemp[nIndex], sizeof(cTemp) - nIndex, ServiceRoleName);
	nIndex += (ServiceRoleName.GetLength() + 1);
	strcpy_s(&cTemp[nIndex], sizeof(cTemp) - nIndex, SuperRoleName);
	nIndex += (SuperRoleName.GetLength() + 1);
	strcpy_s(&cTemp[nIndex], sizeof(cTemp) - nIndex, EngineRoleName);
	nIndex += (EngineRoleName.GetLength() + 1);
	strcpy_s(&cTemp[nIndex], sizeof(cTemp) - nIndex, TechRoleName);
	nIndex += (TechRoleName.GetLength() + 1);
	strcpy_s(&cTemp[nIndex], sizeof(cTemp) - nIndex, OperRoleName);
	nIndex += (OperRoleName.GetLength() + 1);

	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(nIndex, cTemp);
	INT nConvID = m_comClient.SendRequest("HmiUserService", _T("HmiSecurityRoleName"), stMsg, 0);
	return TRUE;
}


//===========================================================================
//   HMI Security Vriable Upper&Lower Limit
//===========================================================================
LONG CMS896AApp::SetHmiSecurityUpperLowerLimit()
{
	CString strSecurityVarUpperLowerLimitFileName = HMI_SECURITY_VAR_UPPER_LOWER_LIMIT_FILE;
	IPC_CServiceMessage SvMsg;
	int nIndex = 0;
	char cTemp[1000];
	strcpy_s(cTemp, 1000, (const char*)strSecurityVarUpperLowerLimitFileName);
	SvMsg.InitMessage(strSecurityVarUpperLowerLimitFileName.GetLength() + 1, cTemp);
	INT nConvID = m_comClient.SendRequest("HmiUserService", _T("HmiSecurityLimit"), SvMsg, 0);
	return TRUE;
}


CString CMS896AApp::EncodePassword(CString &szPass)	// to save and invisible to others
{
	return CodePassword(szPass, 12);
}

CString CMS896AApp::DecodePassword(CString &szPass)	// to use at controll s/w level
{
	return CodePassword(szPass, -12);
}

CString CMS896AApp::GetEngineerPassword()			//v4.51A2
{
	return m_szEngineerPassword;
}

LONG CMS896AApp::ChangeHmiAccessLevel(IPC_CServiceMessage &svMsg)
{
	CString szPassword = "";
	LONG lHmiLevel = 0;
	BOOL bChange = TRUE, bReturn = TRUE;
	svMsg.GetMsg(sizeof(LONG), &lHmiLevel);

	CString szContent = "  ";
	switch( lHmiLevel )
	{
	case 0:	// admin
		szContent	= "Administrator";
		szPassword = m_szHmiAccessPwAdmin;
		break;
	case 1:	// Servi
		szContent	= "Technical";
		szPassword = m_szHmiAccessPwService;
		break;
	case 2:	// Super
		szContent	= "Adv. Prod";
		szPassword = m_szHmiAccessPwSuper;
		break;
	case 3:	// Engin
		szContent	= "Production";
		szPassword = m_szHmiAccessPwEngineer;
		break;
	case 4:	// Techn
		szContent	= "Setup";
		szPassword = m_szHmiAccessPwTechnician;
		break;
	default:
		szContent	= "Operator";
		szPassword = "@@@@@@@@";
		break;
	}

	if( lHmiLevel>=0 && lHmiLevel<=4 )
	{
		CString szTitle = "Please Enter YOUR " + szContent + " Password.";
		bChange = CheckPasswordWithTitle(szPassword, szTitle);
	}

	if( bChange )
	{
		ChangeHmiSecurityLevel(lHmiLevel);
		CString szTitle = "Change Level";
		szContent += " Level in use!";
		HmiMessage(szContent, szTitle);
	}
	else
	{
		m_lHmiAccessLevel	= m_lInUseAccessLevel;
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::UpdateHmiAccessPw(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE, bChange = TRUE;
	LONG lHmiLevel = 0;
	CString szPassword = "";
	CString szContent = "   ";

	svMsg.GetMsg(sizeof(LONG), &lHmiLevel);
	switch( lHmiLevel )
	{
	case 0:	// admin
		szPassword = m_szHmiAccessPwAdmin;
		szContent	= "Administrator";
		break;
	case 1:	// Servi
		szPassword = m_szHmiAccessPwService;
		szContent	= "Technical";
		break;
	case 2:	// Super
		szPassword = m_szHmiAccessPwSuper;
		szContent	= "Adv. Prod";
		break;
	case 3:	// Engin
		szPassword = m_szHmiAccessPwEngineer;
		szContent	= "Production";
		break;
	case 4:	// Techn
		szPassword = m_szHmiAccessPwTechnician;
		szContent	= "Setup";
		break;
	default:
		bChange = FALSE;
		break;
	}

	if( bChange ) 
	{
		CString szTitle = "Please Enter YOUR " + szContent + " Password.";
		bChange = CheckPasswordWithTitle(szPassword,szTitle);
		if( bChange )
		{
			CString szPass1	= _T("");
			CString szTitle = "Please enter new password.";
			bChange = HmiStrInputKeyboard(szTitle, "szLoginPassword", TRUE, szPass1);
			if( szPass1.IsEmpty() )
				bChange = FALSE;
			if( bChange )
			{
				szPassword = szPass1;
			}
		}
	}


	if (bChange == TRUE)
	{
		switch( lHmiLevel )
		{
		case 0:	// admin
			m_szHmiAccessPwAdmin = szPassword;
			WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeAdmin, EncodePassword(m_szHmiAccessPwAdmin));
			break;
		case 1:	// Servi
			m_szHmiAccessPwService = szPassword;
			WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeServi, EncodePassword(m_szHmiAccessPwService));
			break;
		case 2:	// Super
			m_szHmiAccessPwSuper = szPassword;
			WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeSuper, EncodePassword(m_szHmiAccessPwSuper));
			break;
		case 3:	// Engin
			m_szHmiAccessPwEngineer = szPassword;
			WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeEngin, EncodePassword(m_szHmiAccessPwEngineer));
			break;
		case 4:	// Techn
			m_szHmiAccessPwTechnician = szPassword;
			WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeTechn, EncodePassword(m_szHmiAccessPwTechnician));
			break;
		default:
			bChange = FALSE;
			break;
		}
	}

	if( bChange )
	{
		LoadHmiSecuritySettings();
		szContent += " password change : OK";
	}
	else
	{
		if( lHmiLevel==5 )
			szContent	= "Operator has NO password";
		else
			szContent += " password change : FAIL";
	}

	CString szTitle = "Update password";
	HmiMessage(szContent, szTitle);

	m_lHmiAccessLevel	= m_lInUseAccessLevel;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ResetHmiAccessPw(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE, bChange = TRUE;
	LONG lHmiLevel = 0;
	CString szContent = " ", szPassword;

	svMsg.GetMsg(sizeof(LONG), &lHmiLevel);

	switch( lHmiLevel )
	{
	case 0:	// admin
		szPassword = m_szHmiAccessPwAdmin;
		szContent	= "Administrator";
		break;
	case 1:	// Servi
		szPassword = m_szHmiAccessPwService;
		szContent	= "Technical";
		break;
	case 2:	// Super
		szPassword = m_szHmiAccessPwSuper;
		szContent	= "Adv. Prod";
		break;
	case 3:	// Engin
		szPassword = m_szHmiAccessPwEngineer;
		szContent	= "Production";
		break;
	case 4:	// Techn
		szPassword = m_szHmiAccessPwTechnician;
		szContent	= "Setup";
		break;
	default:
		bChange = FALSE;
		break;
	}

	if( bChange ) 
	{
		CString szTitle = "Please Enter YOUR " + szContent + " reset Password.";
		bChange = CheckPasswordWithTitle("SuperUser", szTitle);
		if( bChange==FALSE )
		{
			m_lHmiAccessLevel	= m_lInUseAccessLevel;
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	switch( lHmiLevel )
	{
	case 0:	// admin
		szContent	= "Administrator password reset to default";
		m_szHmiAccessPwAdmin		= HMI_SECURITY_PW_ADM;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeAdmin, EncodePassword(m_szHmiAccessPwAdmin));
		break;
	case 1:	// Servi
		szContent	= "Technical password reset to default";
		m_szHmiAccessPwService		= HMI_SECURITY_PW_SER;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeServi, EncodePassword(m_szHmiAccessPwService));
		break;
	case 2:	// Super
		szContent	= "Adv. Prod password reset to default";
		m_szHmiAccessPwSuper		= HMI_SECURITY_PW_SUP;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeSuper, EncodePassword(m_szHmiAccessPwSuper));
		break;
	case 3:	// Engin
		szContent	= "Production password reset to default";
		m_szHmiAccessPwEngineer		= HMI_SECURITY_PW_ENG;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeEngin, EncodePassword(m_szHmiAccessPwEngineer));
		break;
	case 4:	// Techn
		szContent	= "Setup password reset to default";
		m_szHmiAccessPwTechnician	= HMI_SECURITY_PW_TEC;
		WriteProfileString(gszPROFILE_SETTING, gszHmiAccessCodeTechn, EncodePassword(m_szHmiAccessPwTechnician));
		break;
	default:
		szContent	= "Operator has NO password";
		bChange = FALSE;
		break;
	}

	if (bChange == TRUE)
	{
		LoadHmiSecuritySettings();
	}
	m_lHmiAccessLevel	= m_lInUseAccessLevel;
	CString szTitle = "Reset Password";
	HmiMessage(szContent, szTitle);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
//	HMI access level

#define	WT_szMcExImFileName		"C:\\Mapsorter\\EXE\\McnCon.txt"

LONG CMS896AApp::ReloadMachineProfileCmd(IPC_CServiceMessage &svMsg)
{
	ReloadMachineProfile();
	HmiMessage("Reload file done.", "Export Machine Data");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ExportMachineProfile(IPC_CServiceMessage &svMsg)
{
	DeleteFile(WT_szMcExImFileName);
	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		CString	szName;
		SFM_CStation *pStation;
		CMS896AStn *pStn;

		m_pStationMap.GetNextAssoc(pos, szName, pStation);
		pStn = dynamic_cast<CMS896AStn*>(pStation);
		pStn->ExportMachineConstant();
	}
	HmiMessage("Export file done.", "Export Machine Data");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ImportMachineProfile(IPC_CServiceMessage &svMsg)
{
	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		CString	szName;
		SFM_CStation *pStation;
		CMS896AStn *pStn;

		m_pStationMap.GetNextAssoc(pos, szName, pStation);
		pStn = dynamic_cast<CMS896AStn*>(pStation);
		pStn->ImportMachineConstant();
	}
	DeleteFile(WT_szMcExImFileName);
	HmiMessage("Importing file done.", "Export Machine Data");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::ReloadMachineProfile()
{
	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		CString	szName;
		SFM_CStation *pStation;
		CMS896AStn *pStn;

		m_pStationMap.GetNextAssoc(pos, szName, pStation);
		pStn = dynamic_cast<CMS896AStn*>(pStation);
		pStn->ReloadMachineConstant();
	}

	return 1;
}

LONG CMS896AApp::CreateMSLicenseFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;

	if (CreateMSLicenseStringFile())
	{
		szMsg = _T("Machine License file is created at C:\\MapSorter\\UserData\\MS100.lcs");
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg);
	}
	else
	{
		HmiMessage("Fail to create Machine License file");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CMS896AApp::RegisterMSLicenseFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	BOOL bReset = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bReset);

	CString szFileName = _T("c:\\MapSorter\\UserData\\ms100.lic");
	CString szMsg;

	if (bReset)
	{
		m_bIsMSLicenseRegistered = FALSE;
		WriteProfileInt(gszPROFILE_SETTING, gszLOG_MS_REGISTER_LICENSE, m_bIsMSLicenseRegistered);
		
		szMsg = _T("License file un-registered OK");
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg);
	}
	else
	{
		if (RegisterMSLicenseKey())
		{
			szMsg = _T("License file registered OK");
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			HmiMessage(szMsg);

			m_bIsMSLicenseRegistered = TRUE;
			WriteProfileInt(gszPROFILE_SETTING, gszLOG_MS_REGISTER_LICENSE, m_bIsMSLicenseRegistered);

			if (!ValidateMSLicenseKey())
			{
				HmiMessage("MS License Key validation fails; please check.");
			}
		}
		else
		{
			HmiMessage("MS License file registration fails!");
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::WarmStart(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	bReturn = MachineWarmStart(FALSE);

	if (GetCustomerName() == CTM_SANAN && m_bOperator == TRUE && GetProductLine() == "XA")
	{
		m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) KeyboardProc, AfxGetApp()->m_hInstance, 0);	//GetCurrentThreadId());
		if (m_hKeyboardHook == NULL)
		{
			CString szErr;
			szErr.Format("Andrew: Keyboard HOOK fails! = %d", GetLastError());
			AfxMessageBox(szErr, MB_SYSTEMMODAL);
			m_bIsKeyboardLocked = FALSE;
		}
		else
		{
			m_bIsKeyboardLocked = TRUE;
		}
	}
	else if (GetCustomerName() == CTM_SANAN && (m_bEngineer || m_bAdministrator) && GetProductLine() == "XA")
	{
		if (m_hKeyboardHook != NULL)
		{
			UnhookWindowsHookEx(m_hKeyboardHook);
			m_hKeyboardHook = NULL;
		}
		m_bIsKeyboardLocked = FALSE;
	}
	m_comClient.SendRequest(SECS_COMM_STN, "SC_EnterBondPage", svMsg);

	if ((CMS896AStn::m_pGemStation != NULL) && (CMS896AStn::m_bIsSecsGemInit == TRUE))
	{
		CMS896AStn::m_pGemStation->SendEvent(SG_CEID_MC_WARMSTART_COMPLETED);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::Exit(IPC_CServiceMessage &svMsg)
{
	BOOL bResult = FALSE;

	CString szTitle;
	CString szContent;
		
	szTitle.LoadString(HMB_MS_SYS_WARNING);
	szContent.LoadString(HMB_MS_SYS_EXIT);

	if (m_hKeyboardHook != NULL && m_bIsKeyboardLocked == TRUE && GetCustomerName() == CTM_SANAN && m_bOperator == TRUE && GetProductLine() == "XA")
	{
		UnhookWindowsHookEx(m_hKeyboardHook);
		m_hKeyboardHook = NULL;
		m_bIsKeyboardLocked = FALSE;
	}

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) != glHMI_YES)
	{
		bResult = FALSE;
		m_bAppClosing = FALSE;
	}
	else
	{
		if (m_pSecsComm)
		{
			m_pSecsComm->TurnOffSecsComm();
		}
		m_bAppClosing = TRUE;

		SetAlarmLamp_Status(ALARM_OFF_ALL, FALSE, TRUE);
		//Klocwork
		CWnd *pWnd = AfxGetMainWnd();
		if (pWnd != NULL)
		{
			pWnd->PostMessage(WM_CLOSE, 0, 0);
		}
		bResult = TRUE;
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

VOID CMS896AApp::SetProRata(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	HmiMessage("Set ProRata!");
	m_eqMachine2.SetReportFormat(4, m_bEqTimeAutoReset);
	m_eqMachine2.SetLoadNewWafer(TRUE);
	m_eqMachine2.Refresh(TRUE);
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return;
}

#define	OPERATOR_PASSWORD_FILE	gszUSER_DIRECTORY + "\\password_op.ecp"
#define	ENGINEER_PASSWORD_FILE	gszUSER_DIRECTORY + "\\password.ecp"
BOOL CMS896AApp::CheckPasswordFile_SemitekZJG(BOOL bInputUserID, LONG lInAccessLevel)
{
	if (GetCustomerName() != CTM_SEMITEK || GetProductLine() != "ZJG")
	{
		return TRUE;
	}

	if (m_szUsername.IsEmpty())
	{
		HmiMessage("LOGIN ERROR: Please enter user name.", "Log In");
		SetErrorMessage("LOGIN ERROR: EMPTY user name");
		return FALSE;
	}

	SetStatusMessage("User Logon: " + m_szUsername);
	UpdateHmiVariableData("szLoginID", "");
	UpdateHmiVariableData("szLoginPassword", "");

	CString szInUserID	= _T("");
	if (bInputUserID )
	{
		CString szTitle = _T("Please input valid user ID");
		if( !HmiStrInputKeyboard(szTitle, "szLoginID", FALSE, szInUserID) )
		{
			HmiMessage("ERROR: user ID Login failure!", "Log In");
			SetErrorMessage("ERROR: user ID Login failure - No user ID input.");
			return FALSE;
		}
	}
	else
	{
		szInUserID	= m_szUsername;
	}

	CString szInPassword	= _T("");
	CString szTitle	= _T("Please input password");	
	BOOL bStatus = HmiStrInputKeyboard(szTitle, "szLoginPassword", TRUE, szInPassword);
	if (!bStatus)
	{
		HmiMessage("ERROR: password Login failure!", "Log In");
		SetErrorMessage("ERROR: password Login failure - input password invalid.");
		return FALSE;
	}

	CString szSuperUserName;
	int nPass = (int) GetProfileInt(gszPROFILE_SETTING, _T("SemitekPassword"), 201277);
	szSuperUserName.Format("%d", nPass);

	if ( (szInUserID == szSuperUserName) && (szInPassword == "111111") )
	{
		return TRUE;
	}

	CString szEcpPasswordFile	= OPERATOR_PASSWORD_FILE;
	if( (bInputUserID==FALSE && (m_bEngineer || m_bAdministrator)) ||
		(bInputUserID && (lInAccessLevel==3 || lInAccessLevel==2)) )
	{
		szEcpPasswordFile	= ENGINEER_PASSWORD_FILE;
	}

	if ((_access(szEcpPasswordFile, 0)) == -1)
	{
		HmiMessage("ERROR: fail to access password database!", "Log In");
		SetErrorMessage("ERROR: fail to access password database - " + szEcpPasswordFile);
		return FALSE;
	}

#ifdef MS50_64BIT

	return FALSE;

#else

	CStdioFile oPFile;
	if (!oPFile.Open(szEcpPasswordFile, CFile::modeRead | CFile::typeText))
	{
		HmiMessage("ERROR: fail to open password database!", "Log In");
		SetErrorMessage("ERROR: fail to open password database - " + szEcpPasswordFile);
		return FALSE;
	}

	BOOL bStartPart = FALSE;
	INT nMatchState = 0;	// nothing match
	CString szLineData, szReading;
	CStringArray szaLineList;
	CString szName, szPass, szMode;
	WSDecDll	pDrpt;
	pDrpt.SetKeyword("PASSWORDFILE");
	while (oPFile.ReadString(szReading))
	{
		if( szReading.GetLength()<=3 )
		{
			continue;
		}

		if( pDrpt.DecWS896(szReading, szLineData, 3)==0 )
		{
			continue;
		}

		if (!bStartPart)
		{
			if (szLineData.Find("[START]") != -1)
			{
				bStartPart = TRUE;
			}
			continue;
		}

		if (szLineData.Find("[END]") != -1)
		{
			break;
		}

		szaLineList.RemoveAll();
		ParseRawDataComma(szLineData, szaLineList);
		if ( szaLineList.GetSize()<3 )
		{
			nMatchState = 5;	// format wrong, line data less 3 parts
			break;
		}

		szName = szaLineList.GetAt(0);
		szPass = szaLineList.GetAt(1);
		szMode = szaLineList.GetAt(2);
		szMode.Replace("\n", "");
		szMode = szMode.MakeLower();

		if (szInUserID != szName)
		{
			continue;
		}
		nMatchState = 1;	// name found only
		if (szInPassword != szPass)
		{
			continue;
		}
		nMatchState = 2;	// name and password password found only

		if (lInAccessLevel == 3)			//ADMIN
		{
			if (szMode != "admin")
			{
				nMatchState = 3;	// need admin access
				break;
			}
		}
		else if (lInAccessLevel == 2)		//ENGINEER
		{
			if ((szMode != "engineer") && (szMode != "admin"))
			{
				nMatchState = 4;	//	need engineer/admin access
				break;
			}
		}
		else		//NONE -> for first-time login
		{
			if (m_bAdministrator)
			{
				if (szMode != "admin")
				{
					nMatchState = 33;	// login not admin
					break;
				}
			}
			else if (m_bEngineer)
			{
				if ((szMode != "engineer") && (szMode != "admin"))
				{
					nMatchState = 44;	//	login not engineer or admin
					break;
				}
			}
			else
			{
				if (szMode != "operator")
				{
					nMatchState = 6;	//	login need operator
					break;
				}
			}
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("LOGIN Success: (" + szInUserID + ")");
		nMatchState = -1;
		break;
	}
	oPFile.Close();

	if( nMatchState>=0 )
	{
		CString szLog = "LOGIN failure: (" + szInUserID + ") - incorrect ";
		switch( nMatchState )
		{
		case 0:	//	nothing found
			szLog += "not found in database!  Please login again.";			
			break;
		case 5:
			szLog += "database format wrong! " + szEcpPasswordFile;			
			break;
		case 1:	// found name only
			szLog += "password!!";
			break;
		case 2:
			szLog += "login level!!";
			break;
		case 3:
			szLog += "access level (require ADMIN Mode)";
			break;
		case 4:
			szLog += "access level (require ENGINEER Mode)";
			break;
		case 33:
			szLog += "user right (ADMIN Mode)";
			break;
		case 44:
			szLog += "user right (Engineer Mode)";
			break;
		case 6:
			szLog += "user right (Operator Mode)";
			break;
		default:
			szLog += "- unknown error";
			break;
		}

		SetErrorMessage(szLog);
		HmiMessage(szLog, "Log In");
		return FALSE;
	}

	return TRUE;

#endif
}

LONG CMS896AApp::EncryptPasswordFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn	= TRUE;
	if( GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG" )
	{
/*
		Encryption pErpt;
		pErpt.SetKeyword("PASSWORDFILE");

		CString szMsg;
		CStdioFile oSrcFile, oTgtFile;
		CString szSrcPasswordFile = m_szPasswordFilePath + "\\password_op.csv";
		CString szTgtPasswordFile = OPERATOR_PASSWORD_FILE;
		for(int i=0; i<2; i++)
		{
			if( i==1 )
			{
				szSrcPasswordFile	= m_szPasswordFilePath + "\\password.csv";
				szTgtPasswordFile	= ENGINEER_PASSWORD_FILE;
			}

			bReturn = oSrcFile.Open(szSrcPasswordFile, CFile::modeRead | CFile::typeText );
			if ( !bReturn ) 
			{
				szMsg = "ERROR: fail to open source password - " + szSrcPasswordFile;
				HmiMessage(szMsg, "Log In");
				SetErrorMessage(szMsg);
				continue;
			}

			if ( _access(szTgtPasswordFile, 0) != -1 )
			{
				DeleteFile(szTgtPasswordFile);
			}

			if ( oTgtFile.Open(szTgtPasswordFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite  | CFile::typeText) == FALSE )
			{
				szMsg = "ERROR: fail to open target password - " + szTgtPasswordFile;
				HmiMessage(szMsg, "Log In");
				SetErrorMessage(szMsg);
				oSrcFile.Close();
				continue;
			}

			CString szLine, szCode;
			while (oSrcFile.ReadString(szLine))
			{
				if( szLine.GetLength()<=3 )
				{
					continue;
				}
				szCode = pErpt.EncMakeSequence(szLine, szCode, 3);
				oTgtFile.WriteString( szCode );
				oTgtFile.WriteString("\n");
			}

			oSrcFile.Close();
			oTgtFile.Close();
			if( m_szPasswordFilePath.CompareNoCase(gszUSER_DIRECTORY)!=0 )
			{
				DeleteFile(szSrcPasswordFile);
			}
		}
*/
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// PASSWORD FILE TEXT FORMAT.
//CREATED,[15:13:53 02/12/2011]
//REV,1
//BY,John

//[START]
//S1101,123456,Operator
//S2456,345526,Operator
//S3467,4fvdfs4,Operator
//S5644,65fgf,Operator
//S3333,s9fdm3kl,Engineer
//S5788,fgdgd,Engineer
//Andrew,111111,Engineer
//ASM,happy,Admin
//[END]

LONG CMS896AApp::DecryptPasswordFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

#ifdef MS50_64BIT

#else

	if( GetCustomerName() == CTM_SEMITEK && GetProductLine() == "ZJG" )
	{
		WSDecDll	pDrpt;
		pDrpt.SetKeyword("PASSWORDFILE");

		CString szMsg;
		CStdioFile oSrcFile, oTgtFile;
		CString szSrcPasswordFile	= OPERATOR_PASSWORD_FILE;
		CString szTgtPasswordFile	= m_szPasswordFilePath + "\\password_op.csv";
		for(int i=0; i<2; i++)
		{
			if( i==1 )
			{
				szSrcPasswordFile	= ENGINEER_PASSWORD_FILE;
				szTgtPasswordFile	= m_szPasswordFilePath + "\\password.csv";
			}

			bReturn = oSrcFile.Open(szSrcPasswordFile, CFile::modeRead | CFile::typeText );
			if ( !bReturn ) 
			{
				szMsg = "ERROR: fail to open source password - " + szSrcPasswordFile;
				HmiMessage(szMsg, "Log In");
				SetErrorMessage(szMsg);
				continue;
			}

			if ( _access( szTgtPasswordFile, 0) != -1 )
			{
				DeleteFile(szTgtPasswordFile);
			}

			if ( oTgtFile.Open(szTgtPasswordFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite  | CFile::typeText) == FALSE )
			{
				szMsg = "ERROR: fail to open target password - " + szTgtPasswordFile;
				HmiMessage(szMsg, "Log In");
				SetErrorMessage(szMsg);
				oSrcFile.Close();
				continue;
			}

			CString szLine, szCode;
			while (oSrcFile.ReadString(szLine))
			{
				if( szLine.GetLength()<=3 )
				{
					continue;
				}
				if( pDrpt.DecWS896(szLine, szCode, 3)==0 )
				{
					continue;
				}
				oTgtFile.WriteString( szCode );
				oTgtFile.WriteString("\n");
			}

			oSrcFile.Close();
			oTgtFile.Close();
		}
	}
#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	semitek zjg password checking.

CString CMS896AApp::GetBondStartDate()
{
	return m_szBondStartDate;
}

CString CMS896AApp::GetBondStartTime()
{
	return m_szBondStartTime;
}

CString CMS896AApp::GetBondStopDate()
{
	return m_szBondStopDate;
}

CString CMS896AApp::GetBondStopTime()
{
	return m_szBondStopTime;
}

LONG CMS896AApp::StopLoadUnloadLoopTest(IPC_CServiceMessage &svMsg)
{
	m_bStopLoadUnloadLoopTest = TRUE; //for Wafer/Bin loader
	return 1;
}

LONG CMS896AApp::CheckUserLoginName()
{
	if (m_bOperator)
	{
		m_szUserLogOn = "Operator";
	}
	if (m_bTechnician)
	{
		m_szUserLogOn = "Technician";
	}
	if (m_bEngineer)
	{
		m_szUserLogOn = "Engineer";
	}
	if (m_bAdministrator)
	{
		m_szUserLogOn = "Administrator";
	}
	
	return 1;
}

BOOL CMS896AApp::LoadPackageMsdData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;
	LONG lError = 0;

	if (HmiMessage("Are you sure to load msd data?", "Load msd", glHMI_MBX_YESNO) == glHMI_NO )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead->LoadPackageMsdBHData() == FALSE)
	{
		lError = 1;
	}
	
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (lError == 0 && pWaferTable->LoadPackageMsdWTData() == FALSE )
	{
		lError = 2;
	}

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (lError == 0 && pWaferPr->LoadPackageMsdWPRData() == FALSE)
	{
		lError = 3;
	}

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (lError == 0 && pBondPr->LoadPackageMsdBPRData() == FALSE)
	{
		lError = 4;
	}
	
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (lError == 0 && pBinTable->LoadPackageMsdBTData() == FALSE)
	{
		lError = 5;
	}

	CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
	if (lError == 0 && pWaferLoader->LoadPackageMsdWLData() == FALSE)
	{
		lError = 6;
	}

	if (lError == 0 && LoadPackageMsdMSData() == FALSE)
	{
		lError = 7;
	}

	if (lError != 0)
	{
		switch(lError)
		{
			case 1:
				szMsg = "Fail to load BH data from PackageData.msd";
				break;
			case 2:
				szMsg = "Fail to load WT data from PackageData.msd";
				break;
			case 3:
				szMsg = "Fail to load WPR data from PackageData.msd";
				break;
			case 4:
				szMsg = "Fail to load BPR data from PackageData.msd";
				break;
			case 5:
				szMsg = "Fail to load BT data from PackageData.msd";
				break;
			case 6:
				szMsg = "Fail to load WL data from PackageData.msd";
				break;
			case 7:
				szMsg = "Fail to load MS data from PackageData.msd";
				break;
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg, "Fail to load data");
		SetErrorMessage(szMsg);
	}
	
	szMsg = "load data from PackageData.msd success";
	HmiMessage(szMsg, "Success to load data");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


BOOL CMS896AApp::SavePackageMsdData(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;
	LONG lError = 0;

	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead->SavePackageMsdBHData() == FALSE)
	{
		lError = 1;
	}
	
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (lError == 0 && pWaferTable->SavePackageMsdWTData() == FALSE )
	{
		lError = 2;
	}

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr*>(GetStation(WAFER_PR_STN));
	if (lError == 0 && pWaferPr->SavePackageMsdWPRData() == FALSE)
	{
		lError = 3;
	}

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (lError == 0 && pBondPr->SavePackageMsdBPRData() == FALSE)
	{
		lError = 4;
	}
	
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (lError == 0 && pBinTable->SavePackageMsdBTData() == FALSE)
	{
		lError = 5;
	}

	CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
	if (lError == 0 && pWaferLoader->SavePackageMsdWLData() == FALSE)
	{
		lError = 6;
	}

	if (lError == 0 && SavePackageMsdMSData() == FALSE)
	{
		lError = 7;
	}

	if (lError != 0)
	{
		switch(lError)
		{
			case 1:
				szMsg = "Fail to save BH data from PackageData.msd";
				break;
			case 2:
				szMsg = "Fail to save WT data from PackageData.msd";
				break;
			case 3:
				szMsg = "Fail to save WPR data from PackageData.msd";
				break;
			case 4:
				szMsg = "Fail to save BPR data from PackageData.msd";
				break;
			case 5:
				szMsg = "Fail to save BT data from PackageData.msd";
				break;
			case 6:
				szMsg = "Fail to save WL data from PackageData.msd";
				break;
			case 7:
				szMsg = "Fail to save MS data from PackageData.msd";
				break;
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg, "Fail to save data");
		SetErrorMessage(szMsg);
	}

	szMsg = "save data from PackageData.msd success";
	HmiMessage(szMsg, "Success to save data");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


BOOL CMS896AApp::UpdatePackageList(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szMsg;
	LONG lError = 0;

	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead->UpdateBHPackageList() == FALSE)
	{
		lError = 1;
	}
	
	CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
	if (lError == 0 && pWaferTable->UpdateWTPackageList() == FALSE )
	{
		lError = 2;
	}

	CWaferPr *pWaferPr = dynamic_cast<CWaferPr *>(GetStation(WAFER_PR_STN));
	if (lError == 0 && pWaferPr->UpdateWPRPackageList() == FALSE)
	{
		lError = 3;
	}

	CBondPr *pBondPr = dynamic_cast<CBondPr*>(GetStation(BOND_PR_STN));
	if (lError == 0 && pBondPr->UpdateBPRPackageList() == FALSE)
	{
		lError = 4;
	}

	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (lError == 0 && pBinTable->UpdateBTPackageList() == FALSE)
	{
		lError = 5;
	}
	
	CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
	if (lError == 0 && pWaferLoader->UpdateWLPackageList() == FALSE)
	{
		lError = 6;
	}

	if (lError == 0 && UpdateMSPackageList() == FALSE)
	{
		lError = 7;
	}

	if (lError != 0)
	{
		switch(lError)
		{
			case 1:
				szMsg = "Fail to update BH Parameter List";
				break;
			case 2:
				szMsg = "Fail to update WT Parameter List";
				break;
			case 3:
				szMsg = "Fail to update WPR Parameter List";
				break;
			case 4:
				szMsg = "Fail to update BPR Parameter List";
				break;
			case 5:
				szMsg = "Fail to update BT Parameter List";
				break;
			case 6:
				szMsg = "Fail to update WL Parameter List";
				break;
			case 7:
				szMsg = "Fail to update MS Parameter List";
				break;
		}
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
		HmiMessage(szMsg, "Fail to update Parameter List");
		SetErrorMessage(szMsg);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CMS896AApp::SetAOTDefaultSetting(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (GetCustomerName() == "AOT" || GetCustomerName() == "ABC")	//ABC is used in AOT
	{
		CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
		pBinTable->SetAOTClearBinCounterDefaultSetting();
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CMS896AApp::ZipErrorLog(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szSourcePath, szTargetPath;
	//Get Time For Folder Name
	CTime ctCurTime = CTime::GetCurrentTime();
	CString szTime;
	szTime.Format("%d%02d%02d_%02d%02d%02d",ctCurTime.GetYear(), ctCurTime.GetMonth(), ctCurTime.GetDay(), ctCurTime.GetHour(), ctCurTime.GetMinute(), ctCurTime.GetSecond());
	
	//Final location to save the zip file
	CString szErrorLogPath = "C:\\MapSorter\\UserData\\ErrorLog";
	CreateDirectory(szErrorLogPath, NULL);

	//Temp loaction for saving History, AI Log and Error
	CString szOutputTempPath = "C:\\ErrorLogZip_" + szTime;
	CreateDirectory(szOutputTempPath, NULL);

	//Get 2 newest files in Error folder to Temp loaction
	CString szErrorFolderPath = "C:\\MapSorter\\UserData\\Error";
	CString szErrorFolderDestPath = szOutputTempPath + "\\Error";
	CreateDirectory(szErrorFolderDestPath, NULL);
	CStringArray ErrorFolderFileNameList;
	CUtility::Instance()->GetFileNameListWithCreateTimeOrder(szErrorFolderPath, ErrorFolderFileNameList);
	for(int i = 0; i < 2; i++)
	{
		szSourcePath = szErrorFolderPath + "\\" + ErrorFolderFileNameList.GetAt(i);
		szTargetPath = szErrorFolderDestPath + "\\" + ErrorFolderFileNameList.GetAt(i);
		CopyFile(szSourcePath, szTargetPath, FALSE); 
	}
	
	//Get 10 newest files in AI Log folder to Temp loaction
	CString szAILogFolderPath = "C:\\MapSorter\\UserData\\PrescanResult\\AI Log";
	CString szAILogFolderDestPath = szOutputTempPath + "\\AI Log";
	CreateDirectory(szAILogFolderDestPath, NULL);
	CStringArray AILogFolderFileNameList;
	CUtility::Instance()->GetFileNameListWithCreateTimeOrder(szAILogFolderPath, AILogFolderFileNameList);
	for(int i = 0; i < 10; i++)
	{
		szSourcePath = szAILogFolderPath + "\\" + AILogFolderFileNameList.GetAt(i);
		szTargetPath = szAILogFolderDestPath + "\\" + AILogFolderFileNameList.GetAt(i);
		CopyFile(szSourcePath, szTargetPath, FALSE);
	}

	//Get history folder to temp location
	CString szHistoryFolderPath = "C:\\MapSorter\\UserData\\History";
	CString szHistoryFolderDestPath = szOutputTempPath + "\\History";
	CUtility::Instance()->CopyAllFolderFile(szHistoryFolderPath, szHistoryFolderDestPath);

	//Get StatusRecord
	szSourcePath = "C:\\MapSorter\\Exe\\StatusRecord";
	szTargetPath = szOutputTempPath + "\\StatusRecord";
	CopyFile(szSourcePath, szTargetPath, FALSE); 

	//Create folder GMP Error Log
	CreateDirectory(szOutputTempPath + "\\GMP Error Log", NULL);

	//Get GMP Error Log
	szSourcePath = "C:\\MapSorter\\Exe\\NuData\\gmp_error1.errlog";
	szTargetPath = szOutputTempPath + "\\GMP Error Log\\gmp_error1.errlog";
	CopyFile(szSourcePath, szTargetPath, FALSE); 

	//Get GMP Ext Error Log
	szSourcePath = "C:\\MapSorter\\Exe\\NuData\\gmp_error_ext1.errlog";
	szTargetPath = szOutputTempPath + "\\GMP Error Log\\gmp_error_ext1.errlog";
	CopyFile(szSourcePath, szTargetPath, FALSE); 

	//Zip the temp folder to C-drive
	CString szOutputZipPath = szOutputTempPath + ".zip";
	char *OutputPathptr = (char*)(LPCTSTR)szOutputZipPath;
	char *SourcePathptr = (char*)(LPCTSTR)szOutputTempPath;
	CUtility::Instance()->SYS_ZipCommand(OutputPathptr, SourcePathptr);
	
	//Remove the temp folder in C-drive
	CUtility::Instance()->DeleteAllFolderFiles(szOutputTempPath);
	RemoveDirectory(szOutputTempPath);

	//Copy the zip file in C-drive to Userdata\\ErrorLog, then remove the zip file in C-drive
	CopyFile(szOutputZipPath, szErrorLogPath + "\\ErrorLogZip_" + szTime + ".zip", FALSE);
	DeleteFile(szOutputZipPath);

	CString szText = "Finish to Get Error Log in " + szErrorLogPath + "\\ErrorLogZip_" + szTime + ".zip";
	HmiMessage(szText, "Get Error Log");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}
