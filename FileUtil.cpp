#include "stdafx.h"
#include "FileUtil.h"
#include "LogFileUtil.h"
#include "Mmsystem.h"


CMSFileUtility* CMSFileUtility::m_pInstance = NULL;	

CMSFileUtility::CMSFileUtility()
{
	m_szSMF_NameMultipleBinSerial = "C:\\MapSorter\\UserData\\OutputFile\\BinSerial.msd";

	m_szSMF_NameBT			= "BinTable.msd";
	m_szSMF_NameBH			= "BondHead.msd";
	m_szSMF_NameWPR			= "WaferPr.msd";
	m_szSMF_NameWT			= "WaferTable.msd";
	m_szSMF_NameCPCom		= "Cp100Com.msd";
	m_szSMF_NameCPInt		= "Cp100IntVar.msd";
	m_szSMF_NameBPR			= "BondPr.msd";
	m_szSMF_NameWL			= "WaferLoader.msd";
	m_szSMF_NameBL			= "BinLoader.msd";
	m_szSMF_NameNL			= "NVCLoader.msd";
	m_szSMF_NameLastState	= "LastState.msd";
	m_szSMF_NameBLOM		= "BLMgzSetup.msd";
	m_szSMF_NameBLRT		= "BLMgzCurrent.msd";
	m_szSMF_NameNichia				= "Nichia.msd";
	m_szSMF_NameNichiaWafIDList		= "NichiaWafIDList.msd";
	m_szSMF_NameHWD			= "MachineHWD.msd";
	m_szSMF_NameBkfHWD		= "MachineHWDBkf.msd";

	m_szSMF_BkfNameBT		= "BinTablebkf.msd";
	m_szSMF_BkfNameBH		= "BondHeadbkf.msd";
	m_szSMF_BkfNameWPR		= "WaferPrbkf.msd";
	m_szSMF_BkfNameWT		= "WaferTablebkf.msd";
	m_szSMF_BkfNameCPCom	= "Cp100Combkf.msd";
	m_szSMF_BkfNameCPInt	= "Cp100IntVarbkf.msd";
	m_szSMF_BkfNameBPR		= "BondPrbkf.msd";
	m_szSMF_BkfNameWL		= "WaferLoaderbkf.msd";
	m_szSMF_BkfNameBL		= "BinLoaderbkf.msd";
	m_szSMF_BkfNameNL		= "NVCLoaderbkf.msd";
	m_szSMF_BkfNameLastState= "LastStatebkf.msd";
	m_szSMF_BkfNameBLOM		= "BLMgzSetupbkf.msd";
	m_szSMF_BkfNameBLRT		= "BLMgzCurrentbkf.msd";
	m_szSMF_BkfNameNichia			= "Nichiabkf.msd";
	m_szSMF_BkfNameNichiaWafIDList	= "NichiaWafIDListbkf.msd";

	m_szSMF_NameBLBarcode	= gszROOT_DIRECTORY + _T("\\Exe\\BLBarcode.msd");				//v4.48A10
	m_szSMF_Name2DBarcode	= gszROOT_DIRECTORY + _T("\\Exe\\BinTwoDimensionBarcode.msd");	//v4.48A10
	m_szSMF_NameBinBlkTmpl	= gszROOT_DIRECTORY + _T("\\Exe\\BT_BinBlkTemplate.msd");
	m_szSMF_NameBlkRTData	= gszROOT_DIRECTORY + _T("\\Exe\\BinBlkRunTime.msd");
	m_szSMF_NameLotFile		= gszROOT_DIRECTORY + _T("\\Exe\\LotInfo.msd");
	m_szSMF_NameWLBCInCast	= gszROOT_DIRECTORY + _T("\\Exe\\BarcodeInCassette.msd");
	m_szSMF_NameMachine		= gszROOT_DIRECTORY + _T("\\Exe\\Machine.msd");
	m_szSMF_NameAppFeature	= gszROOT_DIRECTORY + _T("\\Exe\\Feature.msd");
	m_szSMF_NameWTBlkPick	= gszROOT_DIRECTORY + _T("\\Exe\\WTBlock.msd");
	m_szSMF_NameAccessMode	= gszROOT_DIRECTORY + _T("\\Exe\\AccessMode.msd");
	m_szSMF_NameHostComm	= gszROOT_DIRECTORY + _T("\\Exe\\HostComm.msd");
	m_szSMF_NameMSOptions	= gszROOT_DIRECTORY + _T("\\Exe\\MSOptions.msd");
	m_szSMF_NamePrinterList	= gszROOT_DIRECTORY + _T("\\Exe\\PrinterList.msd");
	m_szSMF_NameSG			= gszROOT_DIRECTORY + _T("\\Exe\\secscomm.msd");
	m_szSMF_NameWaferEndData= gszROOT_DIRECTORY + _T("\\Exe\\WaferEndData.msd");
	m_szSMF_PackageData		= gszROOT_DIRECTORY + _T("\\sys\\PackageData.msd");

	// need to init to false so that it will not skip the first time reading
	m_bIsBTConfigOpen	= FALSE;
	//v4.53A25
	m_bIsLastStateOpen		= FALSE;
	m_bIsAppFeatureFileOpen = FALSE;
	m_bIsBlkMapFileOpen		= FALSE;
	m_bIsAccessModeFileOpen = FALSE;
	m_bIsHostCommConfigFileOpen = FALSE;
	m_bIsMSOptionsFileOpen	= FALSE;
	m_bIsAuthorizedPrinterFileOpen = FALSE;
	m_bIsAsWPRMapFileOpen	= FALSE;
	m_bIsAsWTMapFileOpen	= FALSE;
	m_bIsAsWLMapFileOpen	= FALSE;
	m_bIsWaferEndDataFileOpen = FALSE;
	m_bIsNichiaMapFileOpen	= FALSE;
	m_bIsNichiaWafIDListMapFileOpen = FALSE;
	m_bIsPackageDataFileOpen = FALSE;
	m_bIsNLMapFileOpen		= FALSE;			//andrewng //2020-0707

} //end constructor


CMSFileUtility::~CMSFileUtility()
{
} //end destructor


CMSFileUtility* CMSFileUtility::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CMSFileUtility();
	return m_pInstance;

} //end Instance


/*********************************/
/*			Bin Table            */
/*********************************/
BOOL CMSFileUtility::LoadBTConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameBT, m_szSMF_BkfNameBT))
		{
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameBT;
		//if (m_fBTStrMapFile.Open(m_szSMF_NameBT, FALSE, TRUE))
		if (m_fBTStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsBTConfigOpen = TRUE;
		}
		else
		{
			m_bIsBTConfigOpen = FALSE;
		}
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBT);
		m_bIsBTConfigOpen = FALSE;
		return FALSE;
	}

	return TRUE;
}


CStringMapFile* CMSFileUtility::GetBTConfigFile()
{
	m_CSBinTableMSD.Lock();

	if( m_bIsBTConfigOpen==FALSE )
	{
		if( LoadBTConfig()==FALSE )
		{
			return NULL;
		}
	}

	if (m_bIsBTConfigOpen)
		return &m_fBTStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBT);
		return NULL;
	}
}


BOOL CMSFileUtility::SaveBTConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		bUpdate = m_fBTStrMapFile.Update();
		CloseBTConfig();
	}
	catch (...)
	{
		bUpdate = FALSE;
	}
	
	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_NameBT);
	
	return bUpdate;
}


BOOL CMSFileUtility::CloseBTConfig()
{
	try
	{
		if( m_bIsBTConfigOpen )
		{
			m_fBTStrMapFile.Close();
			m_bIsBTConfigOpen = FALSE;
			BackupMSDFile(m_szSMF_NameBT);
		}

		m_CSBinTableMSD.Unlock();
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBT);
		m_CSBinTableMSD.Unlock();
		return FALSE;
	}

	return TRUE;
} //end 

BOOL CMSFileUtility::LoadTwoDimensionBarcodeInfo()
{
	try
	{
		if (m_f2DBarcodeMapFile.Open(m_szSMF_Name2DBarcode, FALSE, TRUE))
		{
			m_bIs2DBarcodeFileOpen = TRUE;
		}
		else
			m_bIs2DBarcodeFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_Name2DBarcode);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetTwoDimensionBarcodeFile()
{
	if (m_bIs2DBarcodeFileOpen)
		return &m_f2DBarcodeMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_Name2DBarcode);
		return NULL;
	}
}

BOOL CMSFileUtility::SaveTwoDimensionBarcodeFile()	
{
	BOOL bUpdate = FALSE;

	try
	{
		bUpdate = m_f2DBarcodeMapFile.Update();
	}
	catch (...)
	{
		bUpdate = FALSE;
	}
	
	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_Name2DBarcode);
	
	return bUpdate;
}


BOOL CMSFileUtility::CloseTwoDimensionBarcodeFile()
{
	try
	{
		m_f2DBarcodeMapFile.Close();
		m_bIs2DBarcodeFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_Name2DBarcode);
		return FALSE;
	}

	return TRUE;
} //end 


BOOL CMSFileUtility::LoadBinBlkTemplateConfig()
{
	try
	{
		if (m_fBinBlkTmplStrMapFile.Open(m_szSMF_NameBinBlkTmpl, FALSE, TRUE))
		{
			m_bIsBinBlkTmplConfigOpen = TRUE;
		}
		else
			m_bIsBinBlkTmplConfigOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBinBlkTmpl);
		return FALSE;
	}

	return TRUE;
} //end 


CStringMapFile* CMSFileUtility::GetBinBlkTemplateConfigFile()
{
	if (m_bIsBinBlkTmplConfigOpen)
		return &m_fBinBlkTmplStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBinBlkTmpl);
		return NULL;
	}
}


BOOL CMSFileUtility::SaveBinBlkTemplateConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		bUpdate = m_fBinBlkTmplStrMapFile.Update();
	}
	catch (...)
	{
		bUpdate = FALSE;
	}
	
	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_NameBinBlkTmpl);
	
	return bUpdate;
}


BOOL CMSFileUtility::CloseBinBlkTemplateConfig()
{
	try
	{
		m_fBinBlkTmplStrMapFile.Close();
		m_bIsBinBlkTmplConfigOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBinBlkTmpl);
		return FALSE;
	}

	return TRUE;
} //end 

BOOL CMSFileUtility::LoadBinBlkRunTimeDataFile()
{
	try
	{
		if (m_fBinBlkRunTimeDataFile.Open(m_szSMF_NameBlkRTData, FALSE, TRUE))
		{
			m_bIsBinBlkRunTimeDataFileOpen = TRUE;
		}
		else
			m_bIsBinBlkRunTimeDataFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBlkRTData);
		return FALSE;
	}

	return TRUE;
}


CStringMapFile* CMSFileUtility::GetBinBlkRunTimeDataFile()
{
	if (m_bIsBinBlkRunTimeDataFileOpen)
		return &m_fBinBlkRunTimeDataFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBlkRTData);
		return NULL;
	}
}



BOOL CMSFileUtility::SaveBinBlkRunTimeDataFile()
{
	BOOL bUpdate = FALSE;

	try
	{
		bUpdate = m_fBinBlkRunTimeDataFile.Update();
	}
	catch (...)
	{
		bUpdate = FALSE;
	}
	
	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_NameBlkRTData);
	
	return bUpdate;

}

BOOL CMSFileUtility::CloseBinBlkRunTimeDataFile()
{
	try
	{
		m_fBinBlkRunTimeDataFile.Close();
		m_bIsBinBlkRunTimeDataFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBlkRTData);
		return FALSE;
	}

	return TRUE;
}


//----------------------------------//
//		Multiple Bin Serial File	//
//---------------------------------//

BOOL CMSFileUtility::LoadMultipleBinSerialFile()
{
	try
	{
		if (m_fMultipleBinSerialMapFile.Open(m_szSMF_NameMultipleBinSerial, FALSE, TRUE))
		{
			m_bIsMultipleBinSerialFileOpen = TRUE;
		}
		else
			m_bIsMultipleBinSerialFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameMultipleBinSerial);
		return FALSE;
	}
	
	return TRUE;
}

CStringMapFile* CMSFileUtility::GetMultipleBinSerialFile()
{
	if (m_bIsMultipleBinSerialFileOpen)
		return &m_fMultipleBinSerialMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameMultipleBinSerial);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateMultipleBinSerialFile()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsMultipleBinSerialFileOpen )
		{
			bUpdate = m_fMultipleBinSerialMapFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameMultipleBinSerial);
	}
	return bUpdate;
}


BOOL CMSFileUtility::CloseMultipleBinSerialFile()
{
	try
	{
		if( m_bIsMultipleBinSerialFileOpen )
			m_fMultipleBinSerialMapFile.Close();

		 m_bIsMultipleBinSerialFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameMultipleBinSerial);
		return FALSE;
	}

	return TRUE;
}

// ********************************
//           Bonding Process 
// ********************************
BOOL CMSFileUtility::LoadBPConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameBH, m_szSMF_BkfNameBH))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameBH;
		//if (m_fBPStrMapFile.Open(m_szSMF_NameBH, FALSE, TRUE))
		if (m_fBPStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsBPMapFileOpen = TRUE;
		}
		else
			m_bIsBPMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBH);
		return FALSE;
	}
	
	return TRUE;
}

CStringMapFile* CMSFileUtility::GetBPConfigFile()
{
	if (m_bIsBPMapFileOpen)
		return &m_fBPStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBH);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateBPConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsBPMapFileOpen )
		{
			bUpdate = m_fBPStrMapFile.Update();
		}
		CloseBPConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameBH);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameBH);
	}
	return bUpdate;
}


BOOL CMSFileUtility::CloseBPConfig()
{
	try
	{
		if( m_bIsBPMapFileOpen )
			m_fBPStrMapFile.Close();

		 m_bIsBPMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBH);
		return FALSE;
	}

	return TRUE;
}

// ********************************
//           WaferPr Process 
// ********************************
BOOL CMSFileUtility::LoadWPRConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameWPR, m_szSMF_BkfNameWPR))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameWPR;
		//if (m_fWPRStrMapFile.Open(m_szSMF_NameWPR, FALSE, TRUE))
		if (m_fWPRStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsWPRMapFileOpen = TRUE;
		}
		else
			m_bIsWPRMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameWPR);
		return FALSE;
	}
	
	return TRUE;
}

CStringMapFile* CMSFileUtility::GetWPRConfigFile()
{
	if (m_bIsWPRMapFileOpen)
		return &m_fWPRStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameWPR);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateWPRConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsWPRMapFileOpen )
		{
			bUpdate = m_fWPRStrMapFile.Update();
		}
		CloseWPRConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameWPR);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameWPR);
	}
	
	return bUpdate;
}


BOOL CMSFileUtility::CloseWPRConfig()
{
	try
	{
		if( m_bIsWPRMapFileOpen )
			m_fWPRStrMapFile.Close();

		m_bIsWPRMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameWPR);
		return FALSE;
	}

	return TRUE;
}


// ********************************
//           WaferTable Process 
// ********************************
BOOL CMSFileUtility::LoadWTConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameWT, m_szSMF_BkfNameWT))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameWT;
		//if (m_fWTStrMapFile.Open(m_szSMF_NameWT, FALSE, TRUE))
		if (m_fWTStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsWTMapFileOpen = TRUE;
		}
		else
			m_bIsWTMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameWT);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetWTConfigFile()
{
	if (m_bIsWTMapFileOpen)
		return &m_fWTStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameWT);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateWTConfig()	
{
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsWTMapFileOpen )
		{
			bUpdate =  m_fWTStrMapFile.Update();
		}
		CloseWTConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameWT);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameWT);
	}
	  
	return bUpdate;
}


BOOL CMSFileUtility::CloseWTConfig()
{
	try
	{
		if( m_bIsWTMapFileOpen )
			m_fWTStrMapFile.Close();

		m_bIsWTMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameWT);
		return FALSE;
	}

	return TRUE;
}

BOOL CMSFileUtility::CheckAndBackupLotInfoFile(ULONGLONG nFileSizeLimit)	//v4.51A14	//Cree HZ 
{
	CFile fLogFile;
	CString szBpLogFileName = gszROOT_DIRECTORY + _T("\\Exe\\LotInfo_bk.msd");

	if  (fLogFile.Open(m_szSMF_NameLotFile, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	LONG nFileSize = (LONG)fLogFile.GetLength();
	fLogFile.Close();

	// Case of no need to backup the log file
	if (nFileSize < nFileSizeLimit)
	{
		return TRUE;
	}

	try
	{
		CopyFile(m_szSMF_NameLotFile, szBpLogFileName, FALSE);
		DeleteFile(m_szSMF_NameLotFile);
	}
	catch (CFileException e)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CMSFileUtility::LoadLotInfoFile()
{
	try
	{
		if (m_fLotFileMapFile.Open(m_szSMF_NameLotFile, FALSE, TRUE))
		{
			m_bIsLotFileOpen = TRUE;
		}
		else
			m_bIsLotFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameLotFile);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetLotInfoFile()
{
	if (m_bIsLotFileOpen)
		return &m_fLotFileMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameLotFile);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateLotInfoFile()	
{
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsLotFileOpen )
		{
			bUpdate =  m_fLotFileMapFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameLotFile);
	}
	  
	return bUpdate;
}


BOOL CMSFileUtility::CloseLotInfoFile()
{
	try
	{
		if( m_bIsLotFileOpen )
			m_fLotFileMapFile.Close();

		m_bIsLotFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameLotFile);
		return FALSE;
	}

	return TRUE;
}

// ********************************
//           BondPr Process 
// ********************************
BOOL CMSFileUtility::LoadBPRConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameBPR, m_szSMF_BkfNameBPR))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameBPR;
		//if (m_fBPRStrMapFile.Open(m_szSMF_NameBPR, FALSE, TRUE))
		if (m_fBPRStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsBPRMapFileOpen = TRUE;
		}
		else
			m_bIsBPRMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBPR);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetBPRConfigFile()
{
	if (m_bIsBPRMapFileOpen)
		return &m_fBPRStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBPR);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateBPRConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsBPRMapFileOpen )
		{
			bUpdate = m_fBPRStrMapFile.Update();
		}
		CloseBPRConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameBPR);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameBPR);
	}

	return bUpdate;
}


BOOL CMSFileUtility::CloseBPRConfig()
{
	try
	{
		if( m_bIsBPRMapFileOpen )
			m_fBPRStrMapFile.Close();

		m_bIsBPRMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBPR);
		return FALSE;
	}

	return TRUE;
}


// ********************************
//           WaferLoader Process 
// ********************************
BOOL CMSFileUtility::LoadWLConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameWL, m_szSMF_BkfNameWL))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameWL;
		//if (m_fWLStrMapFile.Open(m_szSMF_NameWL, FALSE, TRUE))
		if (m_fWLStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsWLMapFileOpen = TRUE;
		}
		else
			m_bIsWLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameWL);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetWLConfigFile()
{
	if (m_bIsWLMapFileOpen)
		return &m_fWLStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameWL);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateWLConfig()	
{
	BOOL bUpdate = FALSE;


	try
	{
		if( m_bIsWLMapFileOpen )
		{
			bUpdate = m_fWLStrMapFile.Update();
		}
		CloseWLConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameWL);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameWL);
	}
	return bUpdate;
}


BOOL CMSFileUtility::CloseWLConfig()
{
	try
	{
		if( m_bIsWLMapFileOpen )
			m_fWLStrMapFile.Close();

		m_bIsWLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameWL);
		return FALSE;
	}

	return TRUE;
}

BOOL CMSFileUtility::LoadBarcodeInCassetteFile()
{
	try
	{
		if (m_fBCInCassetteFile.Open(m_szSMF_NameWLBCInCast, FALSE, TRUE))
		{
			m_bIsBCInCassetteFileOpen = TRUE;
		}
		else
			m_bIsBCInCassetteFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameWLBCInCast);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetBarcodeInCassetteFile()
{
	if (m_bIsBCInCassetteFileOpen)
		return &m_fBCInCassetteFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameWLBCInCast);
		return NULL;
	}
}


BOOL CMSFileUtility::UpdateBarcodeInCassetteFile()
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsBCInCassetteFileOpen )
		{
			bUpdate = m_fBCInCassetteFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameWLBCInCast);
	}
	return bUpdate;

}

BOOL CMSFileUtility::CloseBarcodeCassetteFile()
{
	try
	{
		if( m_bIsBCInCassetteFileOpen )
			m_fBCInCassetteFile.Close();

		m_bIsBCInCassetteFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameWLBCInCast);
		return FALSE;
	}

	return TRUE;

}

// ********************************
//           WaferEnd Process 
// ********************************

BOOL CMSFileUtility::LoadWaferEndDataStrMapFile()
{
	try
	{
		if (m_fWaferEndDataStrMapFile.Open(m_szSMF_NameWaferEndData, FALSE, TRUE))
		{
			m_bIsWaferEndDataFileOpen = TRUE;
		}
		else
		{
			m_bIsWaferEndDataFileOpen = FALSE;
			return FALSE;
		}
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameWaferEndData);
		return FALSE;
	}
	return TRUE;
}

CStringMapFile* CMSFileUtility::GetWaferEndDataStrMapFile()
{
	if (m_bIsWaferEndDataFileOpen)
	{
		return &m_fWaferEndDataStrMapFile;
	}
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameWaferEndData);
		return NULL;
	}
}

BOOL CMSFileUtility::CloseWaferEndDataStrMapFile()
{
	try
	{
		if (m_bIsWaferEndDataFileOpen)
		{
			m_fWaferEndDataStrMapFile.Close();
		}

		m_bIsWaferEndDataFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameWaferEndData);
		return FALSE;
	}

	return TRUE;
}

BOOL CMSFileUtility::UpdateWaferEndDataStrMapFile()
{
	BOOL bUpdate = FALSE;

	try
	{
		if (m_bIsWaferEndDataFileOpen)
		{
			bUpdate = m_fWaferEndDataStrMapFile.Update();
		}
		CloseWaferEndDataStrMapFile();
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if ( bUpdate == FALSE )
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameWaferEndData);
	}

	return bUpdate;
}

BOOL CMSFileUtility::ClearWaferEndDataStrMapFile()
{
	try
	{
		if (m_bIsWaferEndDataFileOpen)
		{
			CloseWaferEndDataStrMapFile();
		}
		m_fWaferEndDataStrMapFile.Remove(m_szSMF_NameWaferEndData);
		DeleteFile("C:\\MapSorter\\Exe\\WaferEndData.msd");
	}
	catch (...)
	{
		LogFileError("Cannot Delete File: " + m_szSMF_NameWaferEndData);
		return FALSE;
	}

	return TRUE;
}

// ********************************
//           BinLoader Process 
// ********************************
BOOL CMSFileUtility::LoadBLConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameBL, m_szSMF_BkfNameBL))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameBL;
		//if (m_fBLStrMapFile.Open(m_szSMF_NameBL, FALSE, TRUE))
		if (m_fBLStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsBLMapFileOpen = TRUE;
		}
		else
			m_bIsBLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBL);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetBLConfigFile()
{
	if (m_bIsBLMapFileOpen)
		return &m_fBLStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBL);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateBLConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsBLMapFileOpen )
		{
			bUpdate = m_fBLStrMapFile.Update();
		}
		CloseBLConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameBL);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_NameBL);

	return bUpdate;
}


BOOL CMSFileUtility::CloseBLConfig()
{
	try
	{
		if( m_bIsBLMapFileOpen )
			m_fBLStrMapFile.Close();

		m_bIsBLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBL);
		return FALSE;
	}

	return TRUE;
}

//4.55T19 change file name by pkg name
BOOL CMSFileUtility::SaveMSDFileNameByMgznOMWithPkg(CString szPKGFilename, CString &szSMF_NameBLOM )
{
	CString szMsg;


	if(szPKGFilename.IsEmpty() == TRUE)
	{
		szMsg = "PKG File name not existed";
		LogFileError(szMsg);	
		//AfxMessageBox(szMsg, MB_SYSTEMMODAL);
		return FALSE;
	}
	szSMF_NameBLOM = "BLMgzSetup" + szPKGFilename + ".msd";
	m_szSMF_NameBLOM = szSMF_NameBLOM;
	
	szMsg.Format("Save MSD file (Bin Loader Operation with Pkg name %s): %s",szPKGFilename, m_szSMF_NameBLOM);
	//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

	return TRUE;
}

BOOL CMSFileUtility::LoadMSDFileNameByMgznOMWithPkg(CString szPKGFilename, CString &szSMF_NameBLOM)
{
	CString szMsg ;

	if(szPKGFilename.IsEmpty() == TRUE)
	{
		szMsg = "*PKG File name not existed";
		LogFileError(szMsg);	
		AfxMessageBox(szMsg, MB_SYSTEMMODAL);
		return FALSE;
	}

	szSMF_NameBLOM = "BLMgzSetup" + szPKGFilename + ".msd";

	CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + szSMF_NameBLOM;

	if (_access(szTgtPath, 0) == -1)
	{
		szMsg.Format("Load OM file (empty grade with pkg:%s) not existed:%s ", szPKGFilename,szTgtPath);
		LogFileError(szMsg);
		AfxMessageBox(szMsg, MB_SYSTEMMODAL);
		return FALSE;
	}

	m_szSMF_NameBLOM = szSMF_NameBLOM;
	szMsg.Format("Load MSD file (Bin Loader Operation with Pkg:%s): %s",szPKGFilename, m_szSMF_NameBLOM);
	//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

	return TRUE;
}

BOOL CMSFileUtility::LoadBLOMConfig()
{
	m_CSBlomMSD.Lock();		//andrewng //2020-0708

	try
	{
		if (!RestoreMSDFile(m_szSMF_NameBLOM, m_szSMF_BkfNameBLOM))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameBLOM;
		//if (m_fBLOMStrMapFile.Open(m_szSMF_NameBLOM, FALSE, TRUE))
		if (m_fBLOMStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsBLOMMapFileOpen = TRUE;
		}
		else
			m_bIsBLOMMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBLOM);
		m_CSBlomMSD.Unlock();
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetBLOMConfigFile()
{
	if (m_bIsBLOMMapFileOpen)
	{
		return &m_fBLOMStrMapFile;
	}
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBLOM);
		m_CSBlomMSD.Unlock();		//andrewng //2020-0708
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateBLOMConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsBLOMMapFileOpen )
		{
			bUpdate = m_fBLOMStrMapFile.Update();
		}
		CloseBLOMConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameBLOM);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameBLOM);
	}
	return bUpdate;
}


BOOL CMSFileUtility::CloseBLOMConfig()
{
	try
	{
		if (m_bIsBLOMMapFileOpen)
		{
			m_fBLOMStrMapFile.Close();
		}
		m_bIsBLOMMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBLOM);
		m_CSBlomMSD.Unlock();
		return FALSE;
	}

	m_CSBlomMSD.Unlock();
	return TRUE;
}


BOOL CMSFileUtility::LoadBLRTConfig()
{
	m_CSRTMgznMSD.Lock();

	try
	{
		if (!RestoreMSDFile(m_szSMF_NameBLRT, m_szSMF_BkfNameBLRT))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameBLRT;
		//if (m_fBLRTStrMapFile.Open(m_szSMF_NameBLRT, FALSE, TRUE))
		if (m_fBLRTStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsBLRTMapFileOpen = TRUE;
		}
		else
			m_bIsBLRTMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBLRT);
		m_CSRTMgznMSD.Unlock();
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetBLRTConfigFile()
{
	if (m_bIsBLRTMapFileOpen)
	{
		return &m_fBLRTStrMapFile;
	}
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBLRT);
		m_CSRTMgznMSD.Unlock();
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateBLRTConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsBLRTMapFileOpen )
		{
			bUpdate = m_fBLRTStrMapFile.Update();
		}
		CloseBLRTConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameBLRT);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_NameBLRT);

	return bUpdate;
}


BOOL CMSFileUtility::CloseBLRTConfig()
{
	try
	{
		if (m_bIsBLRTMapFileOpen)
		{
			m_fBLRTStrMapFile.Close();
		}
		m_bIsBLRTMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBLRT);
		m_CSRTMgznMSD.Unlock();
		return FALSE;
	}

	m_CSRTMgznMSD.Unlock();
	return TRUE;
}

// ********************************
//		NVC Loader Process 
// ********************************
BOOL CMSFileUtility::LoadNLConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameNL, m_szSMF_BkfNameNL))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameNL;
		//if (m_fBLStrMapFile.Open(m_szSMF_NameBL, FALSE, TRUE))
		if (m_fNLStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsNLMapFileOpen = TRUE;
		}
		else
			m_bIsNLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameNL);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetNLConfigFile()
{
	if (m_bIsNLMapFileOpen)
		return &m_fNLStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameNL);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateNLConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsNLMapFileOpen )
		{
			bUpdate = m_fNLStrMapFile.Update();
		}
		CloseNLConfig();
		if (bUpdate)
			BackupMSDFile(m_szSMF_NameNL);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_NameNL);

	return bUpdate;
}


BOOL CMSFileUtility::CloseNLConfig()
{
	try
	{
		if( m_bIsNLMapFileOpen )
			m_fNLStrMapFile.Close();

		m_bIsNLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameNL);
		return FALSE;
	}

	return TRUE;
}


// ********************************
//           Secs Gem Process 
// ********************************
BOOL CMSFileUtility::LoadSGConfig()
{
	try
	{
		if (m_fSGStrMapFile.Open(m_szSMF_NameSG, FALSE, TRUE))
		{
			m_bIsSGMapFileOpen = TRUE;
		}
		else
			m_bIsSGMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameSG);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetSGConfigFile()
{
	if (m_bIsSGMapFileOpen)
		return &m_fSGStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameSG);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateSGConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsSGMapFileOpen )
		{
			bUpdate = m_fSGStrMapFile.Update();	
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameSG);
	}

	return bUpdate;
}


BOOL CMSFileUtility::CloseSGConfig()
{
	try 
	{
		if( m_bIsSGMapFileOpen )
			m_fSGStrMapFile.Close();

		m_bIsSGMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameSG);
		return FALSE;
	}

	return TRUE;
}


// *****************************************
//           Machine Hardware configuration 
// *****************************************
BOOL CMSFileUtility::LoadMachineConfig()
{
	try
	{
		if (m_fMachineStrMapFile.Open(m_szSMF_NameMachine, FALSE, TRUE))
		{
			m_bIsMachineMapFileOpen = TRUE;
		}
		else
			m_bIsMachineMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameMachine);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetMachineConfigFile()
{
	if (m_bIsMachineMapFileOpen)
		return &m_fMachineStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameMachine);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateMachineConfig()	
{
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsMachineMapFileOpen )
		{
			bUpdate = m_fMachineStrMapFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}
	
	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameMachine);
	}

	return bUpdate;
}


BOOL CMSFileUtility::CloseMachineConfig()
{
	try
	{
		if( m_bIsMachineMapFileOpen )
			m_fMachineStrMapFile.Close();

		m_bIsMachineMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameMachine);
		return FALSE;
	}

	return TRUE;
}


// *****************************************
//				Last State
// *****************************************
BOOL CMSFileUtility::LoadLastState(CString szLog)
{
	//v4.53A9
	if (m_bIsLastStateOpen)
	{
		LogFileError("Fail to open LastState MSD because it is already in-use"); 
		return FALSE;
	}

	if (szLog != "")
	{
		//LogFileError("LastState MSD file opened by :" + szLog); 
	}

	m_CSLastState.Lock();

	try
	{
		if (!RestoreMSDFile(m_szSMF_NameLastState, m_szSMF_BkfNameLastState))
		{
		}

		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameLastState;
		if (m_fLastStateMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsLastStateOpen = TRUE;
		}
		else
		{
			m_bIsLastStateOpen = FALSE;
			LogFileError("Cannot OPEN File: " + m_szSMF_NameLastState);
		}
	}
	catch (...)
	{
		m_bIsLastStateOpen = FALSE;
		LogFileError("Cannot Load File: " + m_szSMF_NameLastState);
	}

	if ( m_bIsLastStateOpen == FALSE )
	{
		m_fLastStateMapFile.Close();
		m_CSLastState.Unlock();
	}

	return m_bIsLastStateOpen;
}

CStringMapFile* CMSFileUtility::GetLastStateFile(CString szLog)
{
	//v4.53A9
	if (szLog != "")
	{
		//LogFileError("Get LastState File by: " + szLog);
	}

	if( (m_bIsLastStateOpen) && (&m_fLastStateMapFile!=NULL) )
	{
		return &m_fLastStateMapFile;
	}
	else
	{
		m_fLastStateMapFile.Close();
		m_bIsLastStateOpen = FALSE;
		m_CSLastState.Unlock();
		LogFileError("Cannot GET File: " + m_szSMF_NameLastState);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateLastState(CString szLog)	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsLastStateOpen )
		{
			bUpdate = m_fLastStateMapFile.Update();
		}

		m_fLastStateMapFile.Close();

		if (bUpdate)
		{
			BackupMSDFile(m_szSMF_NameLastState);
		}

		m_bIsLastStateOpen = FALSE;
		m_CSLastState.Unlock();
	}
	catch (...)
	{
		m_fLastStateMapFile.Close();
		m_bIsLastStateOpen = FALSE;		//v4.53A9
		m_CSLastState.Unlock();
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameLastState);
	}

	return bUpdate;
}


BOOL CMSFileUtility::CloseLastState(CString szLog)
{
	//v4.53A9
	//if (szLog != "")
	//{
	//LogFileError("Close LastState File by: " + szLog);
	//}

	try
	{
		if ( m_bIsLastStateOpen )
		{
			m_fLastStateMapFile.Close();
			m_bIsLastStateOpen = FALSE;
			m_CSLastState.Unlock();
		}
	}
	catch (...)
	{
		m_bIsLastStateOpen = FALSE;
		m_fLastStateMapFile.Close();
		m_CSLastState.Unlock();
		LogFileError("Cannot Close File: " + m_szSMF_NameLastState);
		return FALSE;
	}

	return TRUE;
}


// *****************************************
//		Application Feature configuration 
// *****************************************
BOOL CMSFileUtility::LoadAppFeatureConfig()
{
	try
	{
		if (m_fAppFeatureStrMapFile.Open(m_szSMF_NameAppFeature, FALSE, TRUE))
		{
			m_bIsAppFeatureFileOpen = TRUE;
		}
		else
			m_bIsAppFeatureFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameAppFeature);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetAppFeatureFile()
{
	if (m_bIsAppFeatureFileOpen)
		return &m_fAppFeatureStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameAppFeature);
		return NULL;
	}
}


BOOL CMSFileUtility::CloseAppFeatureConfig()
{
	try
	{
		if( m_bIsAppFeatureFileOpen )
			m_fAppFeatureStrMapFile.Close();

		m_bIsAppFeatureFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameAppFeature);
		return FALSE;
	}

	return TRUE;
}

// **********************************
//		BLK Funcation Parameter
// **********************************
BOOL CMSFileUtility::LoadBlkConfig()
{
	try
	{
		if (m_fBlkStrMapFile.Open(m_szSMF_NameWTBlkPick, FALSE, TRUE))
		{
			m_bIsBlkMapFileOpen = TRUE;
		}
		else
			m_bIsBlkMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameWTBlkPick);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetBlkConfigFile()
{
	if (m_bIsBlkMapFileOpen)
		return &m_fBlkStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameWTBlkPick);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateBlkConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsBlkMapFileOpen )
		{
			bUpdate = m_fBlkStrMapFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}
  
	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameWTBlkPick);
	}
	
	return bUpdate;
}


BOOL CMSFileUtility::CloseBlkConfig()
{
	try
	{
		if( m_bIsBlkMapFileOpen )
			m_fBlkStrMapFile.Close();

		m_bIsBlkMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameWTBlkPick);
	}

	return TRUE;
}


// *****************************************
//			HMI control access
// *****************************************
BOOL CMSFileUtility::LoadAccessMode()
{
	try
	{
		if (m_fAccessModeMapFile.Open(m_szSMF_NameAccessMode, FALSE, TRUE))
		{
			m_bIsAccessModeFileOpen = TRUE;
		}
		else
			m_bIsAccessModeFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameAccessMode);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetAccessModeFile()
{
	if (m_bIsAccessModeFileOpen)
		return &m_fAccessModeMapFile;
	else
	{
		 LogFileError("Cannot Open File: " + m_szSMF_NameAccessMode);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateAccessMode()	
{
	
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsAccessModeFileOpen )
		{
			bUpdate = m_fAccessModeMapFile.Update();
		}
	}
	catch(...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		 LogFileError("Cannot Save File: " + m_szSMF_NameAccessMode);
	}
	
	return bUpdate;
}


BOOL CMSFileUtility::CloseAccessMode()
{
	try
	{
		if( m_bIsAccessModeFileOpen )
			m_fAccessModeMapFile.Close();

		m_bIsAccessModeFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameAccessMode);
		return FALSE;
	}

	return TRUE;
}


// *****************************************
//			Host Comm Parameter
// *****************************************
BOOL CMSFileUtility::LoadHostCommConfig()
{
	try
	{
		if (m_fHostCommConfigFile.Open(m_szSMF_NameHostComm, FALSE, TRUE))
		{
			m_bIsHostCommConfigFileOpen = TRUE;
		}
		else
			m_bIsHostCommConfigFileOpen = FALSE;
	}
	catch(...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameHostComm);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetHostCommFile()
{
	if (m_bIsHostCommConfigFileOpen)
		return &m_fHostCommConfigFile;
	else
	{
		 LogFileError("Cannot Open File: " + m_szSMF_NameHostComm);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateHostCommConfig()	
{
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsHostCommConfigFileOpen )
		{
			bUpdate = m_fHostCommConfigFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameHostComm);
	}

	return bUpdate;
}


BOOL CMSFileUtility::CloseHostCommConfig()
{
	try
	{
		if( m_bIsHostCommConfigFileOpen )
			m_fHostCommConfigFile.Close();

		m_bIsHostCommConfigFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameHostComm);
		return FALSE;
	}

	return TRUE;
}


VOID CMSFileUtility::LogFileError(CString szErr)
{
	m_CSect.Lock();
	CStdioFile cfLogFileErr;
	CString szText;
	CString szPath;	
	CString szYear, szMonth;
		
	//AfxMessageBox(szErr, MB_SYSTEMMODAL);		//v4.53A23
	
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());
	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szText = CurTime.Format("[%H:%M:%S %d/%m/%y] ");
	szPath = gszUSER_DIRECTORY + "\\Error\\FileErrorLog_" + szMonth + szYear + ".txt";

	if (cfLogFileErr.Open(szPath, 
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		cfLogFileErr.SeekToEnd();
		cfLogFileErr.WriteString(szText + szErr + "\n");
		cfLogFileErr.Close();
	}

	m_CSect.Unlock();
}

BOOL CMSFileUtility::CheckAndBackupLogFileError(ULONGLONG nFileSizeLimit)
{
	INT nCol =0;
	ULONGLONG nFileSize =0;
	CStdioFile cfLogFileErr;
	CString szText;
	CString szErrLogFileName, szBpErrLogFileName;	
	CString szYear, szMonth;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);

	szMonth.Format("%d", CurTime.GetMonth());
	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}

	szErrLogFileName = gszUSER_DIRECTORY + "\\Error\\FileErrorLog_" + szMonth + szYear + ".txt";

	if (_access(szErrLogFileName, NULL) == -1)	//Not exist!
	{
		return FALSE;
	}

	if  (cfLogFileErr.Open(szErrLogFileName, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	nFileSize = cfLogFileErr.GetLength();
	cfLogFileErr.Close();

	// Case of no need to backup the log file
	if (nFileSize < nFileSizeLimit )
	{
		return TRUE;
	}

	// Start backup log file
	nCol = szErrLogFileName.ReverseFind('.');

	if (nCol != -1)
		szBpErrLogFileName = szErrLogFileName.Left(nCol) + ".bak";
	else
		szBpErrLogFileName = szErrLogFileName + ".bak";

	try
	{
		CopyFile(szErrLogFileName, szBpErrLogFileName, FALSE);
		DeleteFile(szErrLogFileName);
	}
	catch (CFileException e)
	{
		return FALSE;
	}
	
	return TRUE;
}



// *****************************************
//		MS Options
// *****************************************
BOOL CMSFileUtility::LoadMSOptionsConfig()
{
	try
	{
		if (m_fMSOptionsStrMapFile.Open(m_szSMF_NameMSOptions, FALSE, TRUE))
		{
			m_bIsMSOptionsFileOpen = TRUE;
		}
		else
			m_bIsMSOptionsFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameMSOptions);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetMSOptionsFile()
{
	if (m_bIsMSOptionsFileOpen)
		return &m_fMSOptionsStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameMSOptions);
		return NULL;
	}
}


BOOL CMSFileUtility::CloseMSOptionsConfig()
{
	try
	{
		if( m_bIsMSOptionsFileOpen )
			m_fMSOptionsStrMapFile.Close();

		m_bIsMSOptionsFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameMSOptions);
		return FALSE;
	}

	return TRUE;
}



// *****************************************
//		Authorized Printer
// *****************************************
BOOL CMSFileUtility::LoadAuthorizedPrinterListFile()
{
	try
	{
		if (m_fAuthorizedPrinterListMapFile.Open(m_szSMF_NamePrinterList, FALSE, TRUE))
		{
			m_bIsAuthorizedPrinterFileOpen = TRUE;
		}
		else
		{
			m_bIsAuthorizedPrinterFileOpen = FALSE;
		}
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NamePrinterList);
		return FALSE;
	}

	return TRUE;
}


CStringMapFile* CMSFileUtility::GetAuthorizedPrinterListFile()
{
	if (m_bIsAuthorizedPrinterFileOpen)
		return &m_fAuthorizedPrinterListMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NamePrinterList);
		return NULL;
	}
}

BOOL CMSFileUtility::CloseAuthorizedPrinterListFile()
{
	try
	{
		if( m_bIsAuthorizedPrinterFileOpen )
		{
			m_fAuthorizedPrinterListMapFile.Close();
		}

		m_bIsAuthorizedPrinterFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NamePrinterList);
		return FALSE;
	}

	return TRUE;
}

// ********************************
//           AS WaferPr Process 
// ********************************
BOOL CMSFileUtility::AS_LoadWPRConfig(CString szWPRConfigFileName)
{
	m_szSMF_NameAsWPR = szWPRConfigFileName;

	try
	{
		if (m_fAsWPRStrMapFile.Open(szWPRConfigFileName, FALSE, TRUE))
		{
			m_bIsAsWPRMapFileOpen = TRUE;
		}
		else
			m_bIsAsWPRMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameAsWPR);
		return FALSE;
	}
	
	return TRUE;
}

CStringMapFile* CMSFileUtility::AS_GetWPRConfigFile()
{
	if (m_bIsAsWPRMapFileOpen)
		return &m_fAsWPRStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameAsWPR);
		return NULL;
	}
}

BOOL CMSFileUtility::AS_UpdateWPRConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsAsWPRMapFileOpen )
		{
			bUpdate = m_fAsWPRStrMapFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameAsWPR);
	}
	
	return bUpdate;
}


BOOL CMSFileUtility::AS_CloseWPRConfig()
{
	try
	{
		if( m_bIsAsWPRMapFileOpen )
			m_fAsWPRStrMapFile.Close();

		m_bIsAsWPRMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameAsWPR);
		return FALSE;
	}

	return TRUE;
}


// ********************************
//           AS WaferTable Process 
// ********************************
BOOL CMSFileUtility::AS_LoadWTConfig(CString szWTConfigFileName)
{
	m_szSMF_NameAsWT = szWTConfigFileName;

	try
	{
		if (m_fAsWTStrMapFile.Open(szWTConfigFileName, FALSE, TRUE))
		{
			m_bIsAsWTMapFileOpen = TRUE;
		}
		else
			m_bIsAsWTMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameAsWT);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::AS_GetWTConfigFile()
{
	if (m_bIsAsWTMapFileOpen)
		return &m_fAsWTStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameAsWT);
		return NULL;
	}
}

BOOL CMSFileUtility::AS_UpdateWTConfig()	
{
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsAsWTMapFileOpen )
		{
			bUpdate =  m_fAsWTStrMapFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameAsWT);
	}
	return bUpdate;
}

BOOL CMSFileUtility::AS_CloseWTConfig()
{
	try
	{
		if( m_bIsAsWTMapFileOpen )
			m_fAsWTStrMapFile.Close();
		m_bIsAsWTMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameAsWT);
		return FALSE;
	}
	return TRUE;
}


// ********************************
//           AS WaferLoader Process 
// ********************************
BOOL CMSFileUtility::AS_LoadWLConfig(CString szWLConfigFileName)
{
	m_szSMF_NameAsWL = szWLConfigFileName;
	
	try
	{
		if (m_fAsWLStrMapFile.Open(szWLConfigFileName, FALSE, TRUE))
		{
			m_bIsAsWLMapFileOpen = TRUE;
		}
		else
			m_bIsAsWLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameAsWL);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::AS_GetWLConfigFile()
{
	if (m_bIsAsWLMapFileOpen)
		return &m_fAsWLStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameAsWL);
		return NULL;
	}
}

BOOL CMSFileUtility::AS_UpdateWLConfig()	
{
	BOOL bUpdate = FALSE;


	try
	{
		if( m_bIsAsWLMapFileOpen )
		{
			bUpdate = m_fAsWLStrMapFile.Update();
		}
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameAsWL);
	}
	return bUpdate;
}

BOOL CMSFileUtility::AS_CloseWLConfig()
{
	try
	{
		if( m_bIsAsWLMapFileOpen )
			m_fAsWLStrMapFile.Close();

		m_bIsAsWLMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameAsWL);
		return FALSE;
	}

	return TRUE;
}

BOOL CMSFileUtility::BackupMSDFile(CString szFileName)
{
	CString szSrcPath = gszROOT_DIRECTORY + "\\Exe\\" + szFileName;
	CString szTgtPath = gszUSER_DIRECTORY + "\\BackupMSD\\" + szFileName;
	CopyFile(szSrcPath, szTgtPath, FALSE);

	CString szBackupFilename;
	CString szIndex;
	CString szFilename;
	CString szFilenamePrefix, szFileExtension;

	CTime curTime = CTime::GetCurrentTime();
	CString szFolderName;
	szFolderName.Format("Day%d", curTime.GetDayOfWeek());
	CString szTgtHistoryPath = gszUSER_DIRECTORY + "\\MSDHistory";
	CreateDirectory(szTgtHistoryPath, NULL);
	szTgtHistoryPath = szTgtHistoryPath + "\\" + szFolderName;
	CreateDirectory(szTgtHistoryPath, NULL);

	INT nCol = szFileName.Find(".");
	if (nCol != -1)
	{
		szFilenamePrefix = szFileName.Left(nCol);
		szFileExtension = szFileName.Right(szFileName.GetLength() - nCol -1);
	}

	BOOL bFileFound = TRUE;
	for (INT i=1; i<=5; i++)
	{
		szIndex.Format("%d", i);
		szBackupFilename = szTgtHistoryPath + "\\" + szFilenamePrefix + szIndex + "." + szFileExtension;

		if (_access(szBackupFilename, 0) == -1)
		{
			bFileFound = FALSE;
			break;
		}
	}

	if (bFileFound == TRUE)
	{
		szIndex = "1";
		szBackupFilename = szTgtHistoryPath + "\\" + szFilenamePrefix + szIndex + "." + szFileExtension;
	}

	return CopyFile(szSrcPath, szBackupFilename, FALSE);
}

BOOL CMSFileUtility::RestoreMSDFile(CString szFileName, CString szBkfFileName)
{
	CString szSrcPath		= gszUSER_DIRECTORY + "\\BackupMSD\\" + szFileName;
	CString szTgtPath		= gszROOT_DIRECTORY + "\\Exe\\" + szFileName;
	CString szBkfTgtPath	= gszROOT_DIRECTORY + "\\Exe\\" + szBkfFileName;

	if (_access(szSrcPath, 0) == -1)
	{
		LogFileError("Src MSD file not existed: " + szSrcPath);		//v4.28T6
		return FALSE;
	}

	DeleteFile(szTgtPath);		//v4.27
	DeleteFile(szBkfTgtPath);	//v4.27

	BOOL bReturn = CopyFile(szSrcPath, szTgtPath, FALSE);
	if( bReturn==FALSE )
	{
		LogFileError("Cannot restore File: " + szFileName);
	}

	return bReturn;
}


// ********************************
//           BinLoader barcode Process 
// ********************************
BOOL CMSFileUtility::LoadConfigBLBarcode()
{
	m_CSBLBarcode.Lock();
	try
	{
		if (m_fStrMapFileBLBarcode.Open(m_szSMF_NameBLBarcode, FALSE, TRUE))
		{
			m_bIsMapFileOpenBLBarcode = TRUE;
		}
		else
			m_bIsMapFileOpenBLBarcode = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameBLBarcode);
		m_CSBLBarcode.Unlock();
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetConfigFileBLBarcode()
{
	if (m_bIsMapFileOpenBLBarcode)
		return &m_fStrMapFileBLBarcode;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameBLBarcode);
		m_CSBLBarcode.Unlock();
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateConfigBLBarcode()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsMapFileOpenBLBarcode )
		{
			bUpdate = m_fStrMapFileBLBarcode.Update();
		}
		//below comment will make the SW freeze
		//CloseConfigBLBarcode();
		//if (bUpdate)
		//	BackupMSDFile(m_szSMF_NameBLBarcode);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
		LogFileError("Cannot Save File: " + m_szSMF_NameBLBarcode);

	return bUpdate;
}


BOOL CMSFileUtility::CloseConfigBLBarcode()
{
	try
	{
		if( m_bIsMapFileOpenBLBarcode )
			m_fStrMapFileBLBarcode.Close();

		m_bIsMapFileOpenBLBarcode = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameBLBarcode);
		m_CSBLBarcode.Unlock();
		return FALSE;
	}

	m_CSBLBarcode.Unlock();
	return TRUE;
}


// ********************************
//           Probe Table Probing Compensation Process 
// ********************************
BOOL CMSFileUtility::LoadCP_ComConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameCPCom, m_szSMF_BkfNameCPCom))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameCPCom;
		//if (m_fStrMapFileCPCom.Open(m_szSMF_NameCPCom, FALSE, TRUE))
		if (m_fStrMapFileCPCom.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsMapFileOpenCPCom = TRUE;
		}
		else
			m_bIsMapFileOpenCPCom = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameCPCom);
		return FALSE;
	}

	return TRUE;
}

BOOL CMSFileUtility::SaveCP_ComConfig()	
{
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsMapFileOpenCPCom )
		{
			bUpdate =  m_fStrMapFileCPCom.Update();
		}
		CloseCP_ComConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameCPCom);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameCPCom);
	}
	  
	return bUpdate;
}


CStringMapFile* CMSFileUtility::GetCP_ComConfigFile()
{
	if (m_bIsMapFileOpenCPCom)
		return &m_fStrMapFileCPCom;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameCPCom);
		return NULL;
	}
}

BOOL CMSFileUtility::CloseCP_ComConfig()
{
	try
	{
		if( m_bIsMapFileOpenCPCom )
			m_fStrMapFileCPCom.Close();

		m_bIsMapFileOpenCPCom = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameCPCom);
		return FALSE;
	}

	return TRUE;
}
// ********************************
//           Probe Table Probing Compensation Process 
// ********************************

// ********************************
//           Probe Table Independent Variable 
// ********************************
BOOL CMSFileUtility::LoadCP_IndependentConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameCPInt, m_szSMF_BkfNameCPInt))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameCPInt;
		//if (m_fStrMapFileCPCom.Open(m_szSMF_NameCPCom, FALSE, TRUE))
		if (m_fStrMapFileCPInt.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsMapFileOpenCPInt = TRUE;
		}
		else
			m_bIsMapFileOpenCPInt = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameCPInt);
		return FALSE;
	}

	return TRUE;
}

BOOL CMSFileUtility::SaveCP_IndependentConfig()	
{
	BOOL bUpdate = FALSE;
	
	try
	{
		if( m_bIsMapFileOpenCPInt )
		{
			bUpdate =  m_fStrMapFileCPInt.Update();
		}
		CloseCP_IndependentConfig();
		if (bUpdate)		//v4.28T1
			BackupMSDFile(m_szSMF_NameCPInt);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameCPInt);
	}
	  
	return bUpdate;
}


CStringMapFile* CMSFileUtility::GetCP_IndependentConfigFile()
{
	if (m_bIsMapFileOpenCPInt)
		return &m_fStrMapFileCPInt;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameCPInt);
		return NULL;
	}
}

BOOL CMSFileUtility::CloseCP_IndependentConfig()
{
	try
	{
		if( m_bIsMapFileOpenCPInt )
			m_fStrMapFileCPInt.Close();

		m_bIsMapFileOpenCPInt = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameCPInt);
		return FALSE;
	}

	return TRUE;
}
// ********************************
//           Probe Table Independent Variable 
// ********************************


// ********************************
//   Machine HWD functions
// ********************************
BOOL CMSFileUtility::IsHWDFileExist()
{
	if	(_access(m_szSMF_NameHWD, 0) != -1)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CMSFileUtility::LoadHWDFile()
{
	try
	{
		m_cCSFileHWD.Lock();
		if (!RestoreMSDFile(m_szSMF_NameHWD, m_szSMF_NameBkfHWD))
		{
			//return FALSE;
		}

		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameHWD;
		if (m_fStrMapFileHWD.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsMapFileOpenHWD = TRUE;
		}
		else
			m_bIsMapFileOpenHWD = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameHWD);
		m_cCSFileHWD.Unlock();
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetHWDFile()
{
	if (m_bIsMapFileOpenHWD)
		return &m_fStrMapFileHWD;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameHWD);
		m_cCSFileHWD.Unlock();
		return NULL;
	}
}

BOOL CMSFileUtility::SaveHWDFile()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsMapFileOpenHWD )
		{
			bUpdate =  m_fStrMapFileHWD.Update();
		}
		ShutHWDFile();
		if (bUpdate)
			BackupMSDFile(m_szSMF_NameHWD);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameHWD);
	}
	  
	return bUpdate;
}

BOOL CMSFileUtility::ShutHWDFile()
{
	try
	{
		if( m_bIsMapFileOpenHWD )
		{
			m_fStrMapFileHWD.Close();
		}

		m_bIsMapFileOpenHWD = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_NameHWD);
	}
	m_cCSFileHWD.Unlock();

	return TRUE;
}
// ********************************
//   Machine HWD functions
// ********************************


// ********************************
//           Nichia SubSys Process 
// ********************************
BOOL CMSFileUtility::LoadNichiaConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameNichia, m_szSMF_BkfNameNichia))
		{
			//return FALSE;
		}

		//v4.47T5
		CString szTgtPath = gszROOT_DIRECTORY + "\\Exe\\" + m_szSMF_NameNichia;
		//if (m_fNichiaStrMapFile.Open(m_szSMF_NameNichia, FALSE, TRUE))
		if (m_fNichiaStrMapFile.Open(szTgtPath, FALSE, TRUE))
		{
			m_bIsNichiaMapFileOpen = TRUE;
		}
		else
			m_bIsNichiaMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameNichia);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetNichiaConfigFile()
{
	if (m_bIsNichiaMapFileOpen)
		return &m_fNichiaStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameNichia);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateNichiaConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsNichiaMapFileOpen )
		{
			bUpdate = m_fNichiaStrMapFile.Update();
		}
		CloseNichiaConfig();
		if (bUpdate)
			BackupMSDFile(m_szSMF_NameNichia);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameNichia);
	}

	return bUpdate;
}

BOOL CMSFileUtility::CloseNichiaConfig()
{
	try
	{
		if( m_bIsNichiaMapFileOpen )
			m_fNichiaStrMapFile.Close();

		m_bIsNichiaMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Close File: " + m_szSMF_NameNichia);
		return FALSE;
	}

	return TRUE;
}


// ********************************
//           Nichia Wafer ID list checking Process 
// ********************************

BOOL CMSFileUtility::LoadNichiaWafIDListConfig()
{
	try
	{
		if (!RestoreMSDFile(m_szSMF_NameNichiaWafIDList, m_szSMF_BkfNameNichiaWafIDList))
		{
			//return FALSE;
		}

		if (m_fNichiaWafIDListStrMapFile.Open(m_szSMF_NameNichiaWafIDList, FALSE, TRUE))
		{
			m_bIsNichiaWafIDListMapFileOpen = TRUE;
		}
		else
			m_bIsNichiaWafIDListMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_NameNichiaWafIDList);
		return FALSE;
	}

	return TRUE;
}

CStringMapFile* CMSFileUtility::GetNichiaWafIDListConfigFile()
{
	if (m_bIsNichiaWafIDListMapFileOpen)
		return &m_fNichiaWafIDListStrMapFile;
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_NameNichiaWafIDList);
		return NULL;
	}
}

BOOL CMSFileUtility::UpdateNichiaWafIDListConfig()	
{
	BOOL bUpdate = FALSE;

	try
	{
		if( m_bIsNichiaWafIDListMapFileOpen )
		{
			bUpdate = m_fNichiaWafIDListStrMapFile.Update();
		}
		CloseNichiaWafIDListConfig();
		if (bUpdate)
			BackupMSDFile(m_szSMF_NameNichiaWafIDList);
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_NameNichiaWafIDList);
	}

	return bUpdate;
}

BOOL CMSFileUtility::CloseNichiaWafIDListConfig()
{
	try
	{
		if( m_bIsNichiaWafIDListMapFileOpen )
			m_fNichiaWafIDListStrMapFile.Close();
		m_bIsNichiaWafIDListMapFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Close File: " + m_szSMF_NameNichiaWafIDList);
		return FALSE;
	}

	return TRUE;
}

// ********************************
//            Package Data
// ********************************
BOOL CMSFileUtility::LoadPackageDataConfig()
{
	try
	{
		if (m_fPackageDataFile.Open(m_szSMF_PackageData, FALSE, TRUE))
		{
			m_bIsPackageDataFileOpen = TRUE;
		}
		else
		{
			m_bIsPackageDataFileOpen = FALSE;
			return FALSE;
		}
	}
	catch (...)
	{
		LogFileError("Cannot Load File: " + m_szSMF_PackageData);
		return FALSE;
	}
	return TRUE;
}

BOOL CMSFileUtility::UpdatePackageDataConfig()	
{
	BOOL bUpdate = FALSE;
	try
	{
		if (m_bIsPackageDataFileOpen)
		{
			bUpdate = m_fPackageDataFile.Update();
		}
		ClosePackageDataConfig();
	}
	catch (...)
	{
		bUpdate = FALSE;
	}

	if (bUpdate == FALSE)
	{
		LogFileError("Cannot Save File: " + m_szSMF_PackageData);
	}

	return bUpdate;
}


CStringMapFile* CMSFileUtility::GetPackageDataConfig()
{
	if (m_bIsPackageDataFileOpen)
	{
		return &m_fPackageDataFile;
	}
	else
	{
		LogFileError("Cannot Open File: " + m_szSMF_PackageData);
		return NULL;
	}
}

BOOL CMSFileUtility::ClosePackageDataConfig()
{
	try
	{
		if (m_bIsPackageDataFileOpen)
		{
			m_fPackageDataFile.Close();
		}

		m_bIsPackageDataFileOpen = FALSE;
	}
	catch (...)
	{
		LogFileError("Cannot Shut File: " + m_szSMF_PackageData);
		return FALSE;
	}

	return TRUE;
}