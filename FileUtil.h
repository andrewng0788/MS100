#pragma once
#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include "StringMapFile.h"
#include "io.h"
#include "MS896A_ConStant.h"


class CMSFileUtility: public CObject
{
public:
	static CMSFileUtility* Instance();
	
	//alex
	VOID LogFileError(CString szError);
	BOOL CheckAndBackupLogFileError(ULONGLONG nFileSizeLimit = 1024*1024);	//v4.51A14

	BOOL BackupMSDFile(CString szFileName);
	BOOL RestoreMSDFile(CString szFileName, CString szBkfFileName);

	/*********************************/
	/*			Bin Table            */
	/*********************************/
	BOOL LoadBTConfig();
	BOOL SaveBTConfig();
	BOOL CloseBTConfig();
	CStringMapFile* GetBTConfigFile();

	BOOL LoadTwoDimensionBarcodeInfo();
	BOOL SaveTwoDimensionBarcodeFile();	
	BOOL CloseTwoDimensionBarcodeFile();
	CStringMapFile* GetTwoDimensionBarcodeFile();

	BOOL LoadBinBlkTemplateConfig();
	BOOL SaveBinBlkTemplateConfig();
	BOOL CloseBinBlkTemplateConfig();
	CStringMapFile* GetBinBlkTemplateConfigFile();

	BOOL LoadBinBlkRunTimeDataFile();
	BOOL SaveBinBlkRunTimeDataFile();
	BOOL CloseBinBlkRunTimeDataFile();
	CStringMapFile* GetBinBlkRunTimeDataFile();

    // ********************************
    //           Bonding Process 
    // ********************************
    BOOL LoadBPConfig();
    BOOL UpdateBPConfig();
    BOOL CloseBPConfig();
    CStringMapFile* GetBPConfigFile();

    // ********************************
    //           WaferPr Process 
    // ********************************
    BOOL LoadWPRConfig();
    BOOL UpdateWPRConfig();
    BOOL CloseWPRConfig();
    CStringMapFile* GetWPRConfigFile();

    // ********************************
    //           WaferTable Process 
    // ********************************
    BOOL LoadWTConfig();
    BOOL UpdateWTConfig();
    BOOL CloseWTConfig();
    CStringMapFile* GetWTConfigFile();


    // ********************************
    //           Probe Table Probing Compensation Process 
    // ********************************
    BOOL LoadCP_ComConfig();
    BOOL SaveCP_ComConfig();
    BOOL CloseCP_ComConfig();
    CStringMapFile* GetCP_ComConfigFile();

    // ********************************
    //           CP100 Independent Variable
    // ********************************
    BOOL LoadCP_IndependentConfig();
    BOOL SaveCP_IndependentConfig();
    BOOL CloseCP_IndependentConfig();
    CStringMapFile* GetCP_IndependentConfigFile();


    // ********************************
    //   Machine HWD functions	//	HardWareDependent such as BT tilting level, barcode type selection
    // ********************************
	BOOL IsHWDFileExist();
    BOOL LoadHWDFile();
    BOOL SaveHWDFile();
    BOOL ShutHWDFile();
    CStringMapFile* GetHWDFile();


	// ********************************
    //           Lot Information
    // ********************************
	BOOL CheckAndBackupLotInfoFile(ULONGLONG nFileSizeLimit = 1024*1024);		//v4.51A14	//Cree HZ 
    BOOL LoadLotInfoFile();
    BOOL UpdateLotInfoFile();
    BOOL CloseLotInfoFile();
    CStringMapFile* GetLotInfoFile();

	// *******************************
	//		Multiple Bin Serial File
	// *******************************
	
	BOOL LoadMultipleBinSerialFile();
	BOOL UpdateMultipleBinSerialFile();
	BOOL CloseMultipleBinSerialFile();
	CStringMapFile*	GetMultipleBinSerialFile();

    // ********************************
    //           BondPr Process 
    // ********************************
    BOOL LoadBPRConfig();
    BOOL UpdateBPRConfig();
    BOOL CloseBPRConfig();
    CStringMapFile* GetBPRConfigFile();

    // ********************************
    //           WaferLoader Process 
    // ********************************
    BOOL LoadWLConfig();
    BOOL UpdateWLConfig();
    BOOL CloseWLConfig();
    CStringMapFile* GetWLConfigFile();

	BOOL LoadBarcodeInCassetteFile();
	BOOL UpdateBarcodeInCassetteFile();
	CStringMapFile* GetBarcodeInCassetteFile();
	BOOL CloseBarcodeCassetteFile();

    // ********************************
    //		BinLoader Process 
    // ********************************
    BOOL LoadBLConfig();
    BOOL UpdateBLConfig();
    BOOL CloseBLConfig();
    CStringMapFile* GetBLConfigFile();

	BOOL SaveMSDFileNameByMgznOMWithPkg(CString szPKGFilename,CString &szSMF_NameBLOM);
	BOOL LoadMSDFileNameByMgznOMWithPkg(CString szPKGFilename,CString &szSMF_NameBLOM);
    BOOL LoadBLOMConfig();
    BOOL UpdateBLOMConfig();
    BOOL CloseBLOMConfig();
    CStringMapFile* GetBLOMConfigFile();

    BOOL LoadBLRTConfig();
    BOOL UpdateBLRTConfig();
    BOOL CloseBLRTConfig();
    CStringMapFile* GetBLRTConfigFile();

    // ********************************
    //		NVCLoader Process	//andrewng //2020-0707
    // ********************************
    BOOL LoadNLConfig();
    BOOL UpdateNLConfig();
    BOOL CloseNLConfig();
    CStringMapFile* GetNLConfigFile();

    // ********************************
    //		BinLoader barcode Process 
    // ********************************
    BOOL LoadConfigBLBarcode();
    BOOL UpdateConfigBLBarcode();
    BOOL CloseConfigBLBarcode();
    CStringMapFile* GetConfigFileBLBarcode();

	//v4.20	//Reposition from BL/BT classes
    BOOL SaveBLBarcodeData(LONG lBlkNo, CString szBarcode, LONG lMgzNo=0, LONG lSlotNo=0);
    CString GetBLBarcodeData(LONG lBlkNo);

    // ********************************
    //		Secs Gem Process 
    // ********************************
    BOOL LoadSGConfig();
    CStringMapFile* GetSGConfigFile();
    BOOL UpdateSGConfig();
    BOOL CloseSGConfig();

    // ********************************
    //           Machine confiuration 
    // ********************************
    BOOL LoadMachineConfig();
    BOOL UpdateMachineConfig();
    BOOL CloseMachineConfig();
    CStringMapFile* GetMachineConfigFile();

    // ********************************
    //           Last State 
    // ********************************
    BOOL LoadLastState(CString szLog="");
    BOOL UpdateLastState(CString szLog="");
    BOOL CloseLastState(CString szLog="");
    CStringMapFile* GetLastStateFile(CString szLog="");

    // *******************************
    //           Application Features 
    // *******************************
    BOOL LoadAppFeatureConfig();
    BOOL CloseAppFeatureConfig();
    CStringMapFile* GetAppFeatureFile();

    // ********************************
    //           BLK Funcation 
    // ********************************
    BOOL LoadBlkConfig();
    BOOL UpdateBlkConfig();
    BOOL CloseBlkConfig();
    CStringMapFile* GetBlkConfigFile();

	// ********************************
	//			HMI control access
	// ********************************
	BOOL LoadAccessMode();
	BOOL UpdateAccessMode();
	BOOL CloseAccessMode();
	CStringMapFile* GetAccessModeFile();

	// ********************************
	//			Host Comm Parameter
	// ********************************
	BOOL LoadHostCommConfig();
	BOOL UpdateHostCommConfig();
	BOOL CloseHostCommConfig();
	CStringMapFile* GetHostCommFile();

	// *******************************
    //           MS Options 
    // *******************************
    BOOL LoadMSOptionsConfig();
    BOOL CloseMSOptionsConfig();
    CStringMapFile* GetMSOptionsFile();

	CStringMapFile* GetAuthorizedPrinterListFile();
	BOOL LoadAuthorizedPrinterListFile();
	BOOL CloseAuthorizedPrinterListFile();


	// *******************************
    //           AS Wafer PR
    // *******************************
    BOOL AS_LoadWPRConfig(CString szWPRConfigFileName);
    BOOL AS_UpdateWPRConfig();
    BOOL AS_CloseWPRConfig();
    CStringMapFile* AS_GetWPRConfigFile();

    // ********************************
    //           AS WaferTable Process 
    // ********************************
    BOOL AS_LoadWTConfig(CString szWTConfigFileName);
    BOOL AS_UpdateWTConfig();
    BOOL AS_CloseWTConfig();
    CStringMapFile* AS_GetWTConfigFile();

    // ********************************
    //           AS WaferLoader Process 
    // ********************************
    BOOL AS_LoadWLConfig(CString szWLConfigFileName);
    BOOL AS_UpdateWLConfig();
    BOOL AS_CloseWLConfig();
    CStringMapFile* AS_GetWLConfigFile();

    // ********************************
    //           Nichia SubSystem Process 
    // ********************************
    BOOL LoadNichiaConfig();
    BOOL UpdateNichiaConfig();
    BOOL CloseNichiaConfig();
    CStringMapFile* GetNichiaConfigFile();

    BOOL LoadNichiaWafIDListConfig();
    BOOL UpdateNichiaWafIDListConfig();
    BOOL CloseNichiaWafIDListConfig();
    CStringMapFile* GetNichiaWafIDListConfigFile();

	// ********************************
	//			 Wafer End DataLog
	// ********************************
	BOOL LoadWaferEndDataStrMapFile();
	BOOL CloseWaferEndDataStrMapFile();
	BOOL UpdateWaferEndDataStrMapFile();
	BOOL ClearWaferEndDataStrMapFile();
	CStringMapFile* GetWaferEndDataStrMapFile();

	// ********************************
    //          Package Data 
    // ********************************
    BOOL LoadPackageDataConfig();
    BOOL ClosePackageDataConfig();
    BOOL UpdatePackageDataConfig();
    CStringMapFile* GetPackageDataConfig();

protected:

	CMSFileUtility();
	virtual ~CMSFileUtility();

	static CMSFileUtility* m_pInstance;

	/*********************************/
	/*			Bin Table            */
	/*********************************/
	CStringMapFile	m_fBTStrMapFile;
	CString			m_szSMF_NameBT;
	CString			m_szSMF_BkfNameBT;
	BOOL			m_bIsBTConfigOpen;

	CStringMapFile	m_f2DBarcodeMapFile;
	CString			m_szSMF_Name2DBarcode;
	BOOL			m_bIs2DBarcodeFileOpen;

	CStringMapFile	m_fBinBlkTmplStrMapFile;
	CString			m_szSMF_NameBinBlkTmpl;
	BOOL			m_bIsBinBlkTmplConfigOpen;

	CStringMapFile	m_fBinBlkRunTimeDataFile;
	CString			m_szSMF_NameBlkRTData;
	BOOL			m_bIsBinBlkRunTimeDataFileOpen;

	CStringMapFile	m_fBinGradeRunTimeDataFile;
	CString			m_szSMF_NameBinGradeRunTimeData;
	BOOL			m_bIsBinGradeRunTimeDataFileOpen;

	// ********************************
    //           Bonding Process 
    // ********************************
    CStringMapFile  m_fBPStrMapFile;
	CString			m_szSMF_NameBH;
	CString			m_szSMF_BkfNameBH;
    BOOL            m_bIsBPMapFileOpen;

    // ********************************
    //           WaferPr Process 
    // ********************************
    CStringMapFile  m_fWPRStrMapFile;
	CString			m_szSMF_NameWPR;
	CString			m_szSMF_BkfNameWPR;
    BOOL            m_bIsWPRMapFileOpen;

    // ********************************
    //           WaferTable Process 
    // ********************************
    CStringMapFile  m_fWTStrMapFile;
	CString			m_szSMF_NameWT;
	CString			m_szSMF_BkfNameWT;
    BOOL            m_bIsWTMapFileOpen;

    // ********************************
    //           CP100 Compensation Process 
    // ********************************
    CStringMapFile  m_fStrMapFileCPCom;
	CString			m_szSMF_NameCPCom;
	CString			m_szSMF_BkfNameCPCom;
    BOOL            m_bIsMapFileOpenCPCom;

    // ********************************
    //           CP100 Independent variable
    // ********************************
	CStringMapFile  m_fStrMapFileCPInt;
    CString			m_szSMF_NameCPInt;
	CString			m_szSMF_BkfNameCPInt;
    BOOL            m_bIsMapFileOpenCPInt;

    // ********************************
    //    Machine HWD variables
    // ********************************
	CStringMapFile  m_fStrMapFileHWD;
    CString			m_szSMF_NameHWD;
	CString			m_szSMF_NameBkfHWD;
    BOOL            m_bIsMapFileOpenHWD;
	CCriticalSection	m_cCSFileHWD;

	// ********************************
    //           WaferTable Process 
    // ********************************
    CStringMapFile  m_fLotFileMapFile;
	CString			m_szSMF_NameLotFile;
    BOOL			m_bIsLotFileOpen;

	// *******************************
	//		Multiple Bin Serial File
	// *******************************
	CStringMapFile	m_fMultipleBinSerialMapFile;
	CString			m_szSMF_NameMultipleBinSerial;
	BOOL			m_bIsMultipleBinSerialFileOpen;

    // ********************************
    //           BondPr Process 
    // ********************************
    CStringMapFile  m_fBPRStrMapFile;
	CString			m_szSMF_NameBPR;
	CString			m_szSMF_BkfNameBPR;
    BOOL            m_bIsBPRMapFileOpen;

    // ********************************
    //           WaferLoader Process 
    // ********************************
    CStringMapFile  m_fWLStrMapFile;
	CString			m_szSMF_NameWL;
	CString			m_szSMF_BkfNameWL;
    BOOL            m_bIsWLMapFileOpen;

	// ********************************
    //           
    // ********************************
    CStringMapFile  m_fBCInCassetteFile;
	CString			m_szSMF_NameWLBCInCast;
    BOOL            m_bIsBCInCassetteFileOpen;

    // ********************************
    //           BinLoader Process 
    // ********************************
    CStringMapFile  m_fBLStrMapFile;
	CString			m_szSMF_NameBL;
	CString			m_szSMF_BkfNameBL;
    BOOL            m_bIsBLMapFileOpen;

    CStringMapFile  m_fStrMapFileBLBarcode;
	CString			m_szSMF_NameBLBarcode;
    BOOL            m_bIsMapFileOpenBLBarcode;

    CStringMapFile  m_fBLOMStrMapFile;
	CString			m_szSMF_NameBLOM;
	CString			m_szSMF_BkfNameBLOM;
    BOOL            m_bIsBLOMMapFileOpen;

    CStringMapFile  m_fBLRTStrMapFile;
	CString			m_szSMF_NameBLRT;
	CString			m_szSMF_BkfNameBLRT;
    BOOL            m_bIsBLRTMapFileOpen;

    // ********************************
    //		NVC Loader Process 
    // ********************************
    CStringMapFile  m_fNLStrMapFile;
	CString			m_szSMF_NameNL;
	CString			m_szSMF_BkfNameNL;
    BOOL            m_bIsNLMapFileOpen;

    // ********************************
    //           Secs Gem Process 
    // ********************************
    CStringMapFile  m_fSGStrMapFile;
	CString			m_szSMF_NameSG;
    BOOL            m_bIsSGMapFileOpen;

    // ********************************
    //           Machine configuration 
    // ********************************
    CStringMapFile  m_fMachineStrMapFile;
	CString			m_szSMF_NameMachine;
    BOOL            m_bIsMachineMapFileOpen;

    // ********************************
    //           Last State 
    // ********************************
    CStringMapFile  m_fLastStateMapFile;
	CString			m_szSMF_NameLastState;
	CString			m_szSMF_BkfNameLastState;
    BOOL            m_bIsLastStateOpen;

    // *******************************
    //           Application Features 
    // *******************************
    CStringMapFile  m_fAppFeatureStrMapFile;
	CString			m_szSMF_NameAppFeature;
    BOOL            m_bIsAppFeatureFileOpen;

    // ********************************
    //           BLK Function 
    // ********************************
    CStringMapFile  m_fBlkStrMapFile;
	CString			m_szSMF_NameWTBlkPick;
    BOOL            m_bIsBlkMapFileOpen;

	// ********************************
	//			HMI control access
	// ********************************
    CStringMapFile  m_fAccessModeMapFile;
	CString			m_szSMF_NameAccessMode;
    BOOL            m_bIsAccessModeFileOpen;


	// ********************************
	//			Host Comm Parameter
	// ********************************
    CStringMapFile  m_fHostCommConfigFile;
	CString			m_szSMF_NameHostComm;
    BOOL            m_bIsHostCommConfigFileOpen;

	// ********************************
    //           MS Options
    // ********************************
    CStringMapFile  m_fMSOptionsStrMapFile;
	CString			m_szSMF_NameMSOptions;
    BOOL            m_bIsMSOptionsFileOpen;

	// ********************************
    //           MS Options
    // ********************************
    CStringMapFile  m_fAuthorizedPrinterListMapFile;
	CString			m_szSMF_NamePrinterList;
    BOOL            m_bIsAuthorizedPrinterFileOpen;

    // ********************************
    //           AS WaferPr Process 
    // ********************************
    CStringMapFile  m_fAsWPRStrMapFile;
	CString			m_szSMF_NameAsWPR;
    BOOL            m_bIsAsWPRMapFileOpen;

    // ********************************
    //           AS WaferTable Process 
    // ********************************
    CStringMapFile  m_fAsWTStrMapFile;
	CString			m_szSMF_NameAsWT;
    BOOL            m_bIsAsWTMapFileOpen;

    // ********************************
    //           AS WaferLoader Process 
    // ********************************
    CStringMapFile  m_fAsWLStrMapFile;
	CString			m_szSMF_NameAsWL;
    BOOL            m_bIsAsWLMapFileOpen;

	// ********************************
	//			 Wafer End DataLog
	// ********************************
    CStringMapFile  m_fWaferEndDataStrMapFile;
	CString			m_szSMF_NameWaferEndData;
	BOOL			m_bIsWaferEndDataFileOpen;

	// ********************************
	//			 Package Data
	// ********************************
	CStringMapFile  m_fPackageDataFile;
	CString			m_szSMF_PackageData;
    BOOL            m_bIsPackageDataFileOpen;

    // ********************************
    //           Nichia SubSys Process 
    // ********************************
    CStringMapFile  m_fNichiaStrMapFile;
	CString			m_szSMF_NameNichia;
	CString			m_szSMF_BkfNameNichia;
    BOOL            m_bIsNichiaMapFileOpen;

    CStringMapFile  m_fNichiaWafIDListStrMapFile;
	CString			m_szSMF_NameNichiaWafIDList;
	CString			m_szSMF_BkfNameNichiaWafIDList;
    BOOL            m_bIsNichiaWafIDListMapFileOpen;

	CCriticalSection	m_CSect;
	CCriticalSection	m_CSBLBarcode;
	CCriticalSection	m_CSBinTableMSD;
	CCriticalSection	m_CSBlomMSD;			//andrewng //2020-0708
	CCriticalSection	m_CSRTMgznMSD;			//andrewng //2020-0708

	CCriticalSection	m_CSLastState;
	CMutex				m_mxLastState;			//v4.53A9


}; //end class CMSFileUtility

#endif 