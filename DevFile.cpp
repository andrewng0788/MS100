/////////////////////////////////////////////////////////////////
// DevFile.cpp : Device File Class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, January 21, 2005
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
#include "DevFile.h"
#include "stdio.h"
#include "StringMapFile.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include "LogFileUtil.h"
#include "MS896a.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Version number
static const LONG g_lVersion = 1;

//  Bin Storage Version number
static const LONG g_lBinStorage_Ver = 30000;

CDeviceFile::CDeviceFile()
{
	m_szFileList = gszROOT_DIRECTORY + _T("\\Exe\\DF_FileList.msd");		// Default FileList File
	ReadDFFileList();
}

CDeviceFile::~CDeviceFile()
{
}

VOID CDeviceFile::SetFileList(CString& szFileList)
{
	m_szFileList= szFileList;
}


VOID CDeviceFile::ReadDFFileList()
{
	CString szIndex;
	ULONG ulNumOfFiles = 0;

	m_szaDFFileList.RemoveAll();

	if (m_smfFileList.Open(m_szFileList) == FALSE)
	{
		return;
	}

	// Read file list from the String Map File
	ulNumOfFiles = m_smfFileList["NumberOfFiles"];
	
	for (ULONG i=1; i <= ulNumOfFiles; i++ )
	{
		szIndex.Format("%lu", i);
		m_szaDFFileList.Add(m_smfFileList[szIndex]);
		//AfxMessageBox("DFFile List:" + m_szaDFFileList.GetAt(m_szaDFFileList.GetSize() -1));
	}

	m_smfFileList.Close();	
}

// Save Device File
BOOL CDeviceFile::Save(CString& szDevName, CString& szPrRecordPath, ULONG ulNumOfPrRecords, BOOL bBprRecordExist, ULONG ulNumOfBPrRecords)
{
	ULONG			i;
	BOOL			bReturn;
	CStringArray	objFileList;
	CString			szIndex;
	
	objFileList.Copy(m_szaDFFileList);
	
	CString szAccessPath = gszEXE_DIRECTORY + "\\CurBinMap.dat";
	if (_access(szAccessPath, 0) != -1)
	{
		szIndex = "CurBinMap.dat"; //Save the current bin map in pkg/ppkg file
		objFileList.Add(szIndex);
	}
	else
	{
		CMSFileUtility  *pUtl = CMSFileUtility::Instance();
		pUtl->LogFileError("CurBinMap.dat does not exist.");
	}
	
	// Wafer PR Records
	for ( i=0; i <= ulNumOfPrRecords; i++ )
	{
		szIndex.Format("%s\\WPR%d.rec", szPrRecordPath, i);		//v3.13T1
		objFileList.Add(szIndex);
	}

	if ( bBprRecordExist == TRUE )
	{
		// Bond PR Records
		for ( i=0; i <= ulNumOfBPrRecords; i++ )
		{
			szIndex.Format("%s\\BPR%d.rec", szPrRecordPath, i);
			objFileList.Add(szIndex);
		}

		szIndex.Format("%s\\PEI0.rec", szPrRecordPath);
		objFileList.Add(szIndex);
	}

	// Save to Device File
	bReturn = SaveToFile(objFileList, szDevName);

	objFileList.RemoveAll();	// Clean up the memory
	return bReturn;
}

// Restore from Device File
BOOL CDeviceFile::Restore(CString& szDevName)
{
	ULONG		i, ulNumOfFiles;
	CFile		objDevFile, objSrcFile; 
	BYTE		*pbData;
	LONG		lVersion;
	ULONG		ulLength;
	CHAR		acFilename[256];
	CString		strSmfName, strSmfNameTemp;
			
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	if ( ! objDevFile.Open(szDevName, CFile::modeRead) )
		return FALSE;

	// Read version number
	objDevFile.Read(&lVersion, sizeof(LONG));

	if( lVersion==999 )
	{
		objDevFile.Close();
		return FALSE;
	}

	// Read the number of files saved
	objDevFile.Read(&ulNumOfFiles, sizeof(ULONG));

	for ( i=0; i < ulNumOfFiles; i++ )
	{
		// Read filename
		objDevFile.Read(acFilename, 256 * sizeof(CHAR));
		// Read file size
		objDevFile.Read(&ulLength, sizeof(ULONG));

		strSmfName.Format("%s",acFilename);

		BOOL bDummyRead = FALSE;

		if (m_bPortablePKGFile == TRUE)
		{
			// Huga type
			if (m_bIsPortablePackageFileIgnoreList == TRUE)
			{
				//AfxMessageBox("Ignore List", MB_SYSTEMMODAL);
				bDummyRead = FALSE;

				for (INT k=0; k<m_szPortablePackageFileFileList.GetSize(); k++)
				{
					strSmfNameTemp = m_szPortablePackageFileFileList.GetAt(k);

					if (strSmfName == strSmfNameTemp)
					{
						//AfxMessageBox("Dummy read file" + strSmfName, MB_SYSTEMMODAL);
						bDummyRead = TRUE;
						break;
					}
				}
			}
			else
			{
				// general type
				bDummyRead = TRUE;
				for (INT k=0; k<m_szaDFFileList.GetSize(); k++)
				{
					strSmfNameTemp = m_szaDFFileList.GetAt(k);

					if (strSmfName == strSmfNameTemp)
					{
						//AfxMessageBox("Dummy Read File: " + strSmfName , MB_SYSTEMMODAL);
						bDummyRead = FALSE;
						break;
					}
				}

				if (strSmfName.Find(".dat") != -1)
				{
					bDummyRead = FALSE;
				}
			}
		}
		// Case of only load bin table parameters
		else if (m_bOnlyLoadBinParam == TRUE)
		{
			if (strSmfName != "BinTable.msd")
				bDummyRead = TRUE;
		}
		else if( m_bOnlyLoadWFTParam )
		{
			if( strSmfName != "WaferTable.msd" && strSmfName != "WaferLoader.msd" )
				bDummyRead = TRUE;
		}
		else if (m_bOnlyLoadPRParam == TRUE)
		{
			bDummyRead = FALSE;

			if (strSmfName == "WaferLoader.msd" || strSmfName == "BinLoader.msd" || strSmfName == "BondHead.msd" 
				|| strSmfName == "WaferTable.msd" || strSmfName == "LastState.msd" || strSmfName == "BinTable.msd") 
			{
				bDummyRead = TRUE;
			}
		}
		// Case of loading whole package file
		else
		{
			// case of include loading bin table parameter for loading
			// the whole package file
			if ( m_bLoadPkgWithBin == FALSE)
			{
				if ( strSmfName == "BinTable.msd" )
					bDummyRead = TRUE;
			}

			// case of include loading machine parameters
			// (e.g WaferLoader, BinLoader, BondHeade & Ejector)for loading
			// the whole package file
			if ( m_bExcludeMachineParam == TRUE )
			{
				if ( (pApp->GetCustomerName() == "Lumileds") && (pApp->GetProductLine() == "DieFab") )	//v4.33T1	//PLSG
				{
					if (strSmfName == "BinLoader.msd")
					{
						bDummyRead = TRUE;
					}
				}
				//v4.46T15	//Need to load WaferLoader.msd file for Rebel request
				else if ( (pApp->GetCustomerName() == "Lumileds") && (pApp->GetProductLine() == "Rebel") )	//v4.33T1	//PLSG
				{
					if (strSmfName == "BinLoader.msd" || strSmfName == "BondHead.msd")
					{
						bDummyRead = TRUE;
					}
					
				}
				else
				{
					if (strSmfName == "WaferLoader.msd" || strSmfName == "BinLoader.msd" || strSmfName == "BondHead.msd")
					{
						bDummyRead = TRUE;
					}
				}
			}
		}

		if (bDummyRead == TRUE)
		{	
			pbData = new BYTE[ulLength];
			objDevFile.Read(pbData, ulLength * sizeof(BYTE));
			delete [] pbData;
			continue;
		}

		if ( ! objSrcFile.Open(acFilename,  CFile::modeCreate | CFile::modeWrite) )
		{
			objDevFile.Close();
			return FALSE;
		}

		pbData = new BYTE[ulLength];
		objDevFile.Read(pbData, ulLength * sizeof(BYTE));
		objSrcFile.Write(pbData, ulLength * sizeof(BYTE));
		delete [] pbData;
		objSrcFile.Close();
	    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
		strSmfName.MakeLower();
		pUtl->LogFileError("restore file " + strSmfName);
		if (strSmfName.Find(".msd") != -1)
			pUtl->BackupMSDFile(strSmfName);
	}
	objDevFile.Close();
	return TRUE;
}


BOOL CDeviceFile::SetLoadPkgOption(BOOL bWithBin)
{
	m_bLoadPkgWithBin = bWithBin;

	return TRUE;
}

BOOL CDeviceFile::SetOnlyLoadBinParameters(BOOL bOnlyLoadBinParam)
{
	m_bOnlyLoadBinParam = bOnlyLoadBinParam;

	return TRUE;
}

BOOL CDeviceFile::SetOnlyLoadWFTParameters(BOOL bOnlyLoadWFTParam)
{
	m_bOnlyLoadWFTParam = bOnlyLoadWFTParam;
	return TRUE;
}

BOOL CDeviceFile::SetOnlyLoadPRParameters(BOOL bOnlyLoadPRParam)
{
	m_bOnlyLoadPRParam = bOnlyLoadPRParam;
	return TRUE;
}

BOOL CDeviceFile::SetExcludeMachineParameters(BOOL bExcludeMachineParam)
{
	m_bExcludeMachineParam = bExcludeMachineParam;

	return TRUE;
}

BOOL CDeviceFile::SetPortablePKGFile(BOOL bPortablePKGFile)
{
	m_bPortablePKGFile	= bPortablePKGFile;
	return TRUE;
}


BOOL CDeviceFile::ReadBinRunTimeFileFileList(CStringArray& szaFileList)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	pUtl->LoadBinBlkRunTimeDataFile();

	psmf = pUtl->GetBinBlkRunTimeDataFile();
	if (psmf==NULL)
	{
		return FALSE;
	}

	LONG lNoOfFiles = (*psmf)["FileList"]["No Of File"];
	CString szFileName;
	for (INT i=1; i <= lNoOfFiles; i++ )
	{
		szFileName = (*psmf)["FileList"][i];
		szaFileList.Add(szFileName);
	}
	pUtl->CloseBinBlkRunTimeDataFile();

	// store bin table setup
	szaFileList.Add("BinTable.msd");
	szaFileList.Add("BinBlkRunTime.msd");
	szaFileList.Add("BLMgzSetup.msd");
	szaFileList.Add("BLMgzCurrent.msd");
	szaFileList.Add("BLBarcode.msd");
	szaFileList.Add("BinTwoDimensionBarcode.msd");

	return TRUE;
}

BOOL CDeviceFile::SaveBinRunTimeFile(CString szDevName)
{
	CStringArray	objFileArray;
	
	if (ReadBinRunTimeFileFileList(objFileArray) == FALSE)
	{
		return FALSE;
	}

	if (SaveToFile(objFileArray, szDevName) == FALSE)
	{
		objFileArray.RemoveAll();
		return FALSE;
	}

	objFileArray.RemoveAll();

	return TRUE;
}

BOOL CDeviceFile::RestoreBinRunTimeFile(CString szDevName)
{
	ULONG		i, ulNumOfFiles;
	CFile		cfDstFile, cfSrcFile; 
	BYTE		*pbData;
	LONG		lVersion;
	ULONG		ulLength;
	CHAR		acFilename[256];
	CString		strSmfName;

	if (cfSrcFile.Open(szDevName, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	// Read version number
	cfSrcFile.Read(&lVersion, sizeof(LONG));

	// Read the number of files saved
	cfSrcFile.Read(&ulNumOfFiles, sizeof(ULONG));

	for ( i=0; i < ulNumOfFiles; i++ )
	{
		// Read filename
		cfSrcFile.Read(acFilename, 256 * sizeof(CHAR));
		// Read file size
		cfSrcFile.Read(&ulLength, sizeof(ULONG));

		strSmfName.Format("%s",acFilename);

		if (ulLength == 0)
		{
			continue;
		}

		if (cfDstFile.Open(acFilename,  CFile::modeCreate | CFile::modeWrite) == FALSE)
		{
			cfDstFile.Close();
			return FALSE;
		}

		pbData = new BYTE[ulLength];
		cfSrcFile.Read(pbData, ulLength * sizeof(BYTE));
		cfDstFile.Write(pbData, ulLength * sizeof(BYTE));
		delete [] pbData;
		cfDstFile.Close();
		if( strSmfName=="BinTable.msd" || 
			strSmfName=="BLMgzSetup.msd" || 
			strSmfName=="BLMgzCurrent.msd" )
		{
		    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
			pUtl->BackupMSDFile(strSmfName);
		}
	}

	cfSrcFile.Close();
	return TRUE;
}


// Save to Device File
// Limitation: 1) File size must be < 4G bytes and 2) Filename is < 256 characters
BOOL CDeviceFile::SaveToFile(CStringArray& objFileArray, CString& szDevName)
{
	ULONG		i, j, ulNumOfFiles;
	CFile		objDevFile, objSrcFile; 
	CString		szFilename;
	ULONG		ulLength;
	BYTE		*pbData;
	BOOL		bReturn;
	CHAR		acName[256];

	// Create Device File
	if ( ! objDevFile.Open(szDevName,  CFile::modeCreate | CFile::modeWrite) )
		return FALSE;
	
	// Store version number 
	objDevFile.Write(&g_lVersion, sizeof(LONG));	
	
	// Store number of files to be saved	
	ulNumOfFiles = (ULONG)objFileArray.GetSize();
	objDevFile.Write(&ulNumOfFiles, sizeof(ULONG));

	for ( i=0; i < ulNumOfFiles; i++ )
	{
		szFilename = objFileArray.GetAt(i);

		if ( szFilename.GetLength() >= 256 )
		{
			objDevFile.Close();
			return FALSE;
		}

		bReturn = objSrcFile.Open(szFilename, CFile::modeRead);

		for ( j=0; j < 256; j++ ) 		// Clear the temporary array
			acName[j] = 0;

		sprintf_s(acName, sizeof(acName), szFilename.GetBuffer());

		// Write filename (limit to 256-bytes)
		objDevFile.Write(acName, 256 * sizeof(CHAR)); 
	
		// Write file size
		if ( bReturn )
			ulLength = (ULONG)objSrcFile.GetLength();	// Limited the file size
		else
			ulLength = 0;
		objDevFile.Write(&ulLength, sizeof(ULONG));

		if ( bReturn )
		{
			// Write raw data
			pbData = new BYTE[ulLength];
			objSrcFile.Read(pbData, ulLength * sizeof(BYTE));
			objDevFile.Write(pbData, ulLength * sizeof(BYTE));

			delete [] pbData;
			objSrcFile.Close();
		}
	}
	objDevFile.Close();
	return TRUE;
}

VOID CDeviceFile::SetPortablePackageFileFileList(CStringArray& szaFileList)
{
	m_szPortablePackageFileFileList.Copy(szaFileList);
}

VOID CDeviceFile::SetIsPackageFileIgnoreList(BOOL bIsIgnoreList)
{
	m_bIsPortablePackageFileIgnoreList = bIsIgnoreList;
}

// Restore PR record from Device File
BOOL CDeviceFile::RestorePrData(CString& szDevName, BOOL bLoadWPR, BOOL bLoadWFT)
{
	CFile		objDevFile, objSrcFile; 
	LONG		lVersion;
	ULONG		i, ulNumOfFiles, ulDataLength;
	BYTE		*pbRawData;
	CHAR		acFilename[256];
	CString		szSmfName;

	if( !objDevFile.Open(szDevName, CFile::modeRead) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("device file can not open " + szDevName);
		return FALSE;
	}

	// Read version number
	objDevFile.Read(&lVersion, sizeof(LONG));
	if( lVersion==999 )
	{
		objDevFile.Close();
		CMSLogFileUtility::Instance()->WL_LogStatus("device file version is 999");
		return FALSE;
	}

	// Read the number of files saved
	objDevFile.Read(&ulNumOfFiles, sizeof(ULONG));

	for( i=0; i < ulNumOfFiles; i++ )
	{
		// Read filename
		objDevFile.Read(acFilename, 256 * sizeof(CHAR));
		szSmfName.Format("%s",acFilename);

		// Read file size
		objDevFile.Read(&ulDataLength, sizeof(ULONG));
		pbRawData = new BYTE[ulDataLength];
		objDevFile.Read(pbRawData, ulDataLength * sizeof(BYTE));

		if( szSmfName=="WaferLoader.msd" ||	szSmfName=="BinLoader.msd" || szSmfName=="BondHead.msd" ||
			szSmfName=="LastState.msd"	 || szSmfName=="BinTable.msd" )
		{
		}
		else if( (bLoadWPR==FALSE && szSmfName=="WaferPr.msd") ||
				 (bLoadWFT==FALSE && szSmfName=="WaferTable.msd") )
		{
		}
		else
		{
			if( !objSrcFile.Open(acFilename, CFile::modeCreate | CFile::modeWrite) )
			{
				objDevFile.Close();
				delete [] pbRawData;
				CMSLogFileUtility::Instance()->WL_LogStatus("can not write file " + szSmfName);
				return FALSE;
			}

			objSrcFile.Write(pbRawData, ulDataLength * sizeof(BYTE));
			objSrcFile.Close();

			szSmfName.MakeLower();
			if( szSmfName.Find(".msd")!=-1 )
			{
				CMSFileUtility::Instance()->BackupMSDFile(szSmfName);
			}
		}

		delete [] pbRawData;
	}

	objDevFile.Close();

	return TRUE;
}	// resort PR data from device file

// Extract key parameters from Device File if not done when save pkg, done when press start button, once only.
BOOL CDeviceFile::GainPkgKeyParameters(CString& szDevName)
{
	CFile	objPkgFile; 
	if (objPkgFile.Open(szDevName, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	// Read version number
	LONG	lVersion;
	objPkgFile.Read(&lVersion, sizeof(LONG));
	if( lVersion==999 )
	{
		objPkgFile.Close();
		return FALSE;
	}

	LONG lCounter = 0;
	// Read the number of files saved
	ULONG	ulNumOfFiles;
	objPkgFile.Read(&ulNumOfFiles, sizeof(ULONG));
	for(ULONG i=0; i < ulNumOfFiles; i++)
	{
		BYTE		*pbData;
		CHAR	acFilename[256];
		ULONG		ulLength;
		// Read filename
		objPkgFile.Read(acFilename, 256 * sizeof(CHAR));
		// Read file size
		objPkgFile.Read(&ulLength, sizeof(ULONG));
		pbData = new BYTE[ulLength];
		objPkgFile.Read(pbData, ulLength * sizeof(BYTE));

		CString		strSmfName;
		strSmfName.Format("%s", acFilename);

		if( strSmfName.CompareNoCase("LastState.msd")==0	||
			strSmfName.CompareNoCase("BinTable.msd")==0		||
			strSmfName.CompareNoCase("BondPr.msd")==0 		||
			strSmfName.CompareNoCase("WaferPr.msd")==0 )
		{
			CString szNewMsdName = "C:\\MapSorter\\UserData\\" + strSmfName;
			DeleteFile(szNewMsdName);
			CFile	objTgtFile; 
			if( objTgtFile.Open(szNewMsdName,  CFile::modeCreate | CFile::modeWrite) )
			{
				objTgtFile.Write(pbData, ulLength * sizeof(BYTE));
				objTgtFile.Close();
			}
			lCounter++;
		}
	
		delete [] pbData;
		if( lCounter>=4 )
		{
			break;
		}
	}
	objPkgFile.Close();

	return TRUE;
}

BOOL CDeviceFile::RestoreFiles(CString& szDevName, CStringArray	&szaFilesList)	// Restore specifi data files
{
	CFile	objDevFile; 
	if( !objDevFile.Open(szDevName, CFile::modeRead) )
	{
		return FALSE;
	}

	// Read version number
	LONG		lVersion;
	objDevFile.Read(&lVersion, sizeof(LONG));
	if( lVersion==999 )
	{
		objDevFile.Close();
		return FALSE;
	}

	// Read the number of files saved
	ULONG		ulNumOfFiles;
	objDevFile.Read(&ulNumOfFiles, sizeof(ULONG));

	for (ULONG i=0; i < ulNumOfFiles; i++ )
	{
		// Read filename
		CHAR		acFilename[256];
		objDevFile.Read(acFilename, 256 * sizeof(CHAR));
		// Read file size
		ULONG		ulLength;
		objDevFile.Read(&ulLength, sizeof(ULONG));

		CString		strSmfName;
		strSmfName.Format("%s", acFilename);

		BOOL bDummyRead = TRUE;
		for (INT k=0; k<szaFilesList.GetSize(); k++)
		{
			CString strSmfNameTemp = szaFilesList.GetAt(k);
			if (strSmfName.CompareNoCase(strSmfNameTemp)==0)
			{
				bDummyRead = FALSE;
				break;
			}
		}

		BYTE		*pbData;
		pbData = new BYTE[ulLength];
		objDevFile.Read(pbData, ulLength * sizeof(BYTE));
		if (bDummyRead == FALSE)
		{
			CFile	objSrcFile; 
			if ( !objSrcFile.Open(acFilename,  CFile::modeCreate | CFile::modeWrite) )
			{
				delete [] pbData;
				objDevFile.Close();
				return FALSE;
			}
			objSrcFile.Write(pbData, ulLength * sizeof(BYTE));
			objSrcFile.Close();

			CMSFileUtility  *pUtl = CMSFileUtility::Instance();
			strSmfName.MakeLower();
			pUtl->LogFileError("restore file " + strSmfName);
			if( strSmfName.Find(".msd")!=-1 )
				pUtl->BackupMSDFile(strSmfName);
		}
		delete [] pbData;
	}
	objDevFile.Close();

	return TRUE;
}
