/////////////////////////////////////////////////////////////////
// DevFile.h : Header file for Device File Class
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

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "StringMapFile.h"

#define BT_BIN_BLOCK_NO		175

class CDeviceFile: public CObject
{
	public:
		CDeviceFile();
		~CDeviceFile();

		// Save from Device File
		BOOL Save(CString& szDevName, CString& szPrRecordPath, ULONG ulNumOfPrRecords, BOOL bBprRecordExist, ULONG ulNumOfBPrRecords);
		BOOL Restore(CString& szDevName);			// Restore from Device File
		BOOL GainPkgKeyParameters(CString &szDevName);	// Extract some for Pkg Key parameters check.
		VOID SetFileList(CString& szFileList);		// Set filename of FileList
		BOOL RestorePrData(CString& szDevName, BOOL bLoadWPR, BOOL bLoadWFT);			// Restore PR record from Device File
		BOOL RestoreFiles(CString& szDevName, CStringArray	&szaFilesList);			// Restore specifi data files

		BOOL SetLoadPkgOption(BOOL bWithBin);
		BOOL SetOnlyLoadBinParameters(BOOL bOnlyLoadBinParam);
		BOOL SetOnlyLoadWFTParameters(BOOL bOnlyLoadBinParam);
		BOOL SetOnlyLoadPRParameters(BOOL bOnlyLoadPRParam);
		// Function to set the exclude Machine Parameters
		// i.e Do not load WaferLoader, BinLoader, BondHead & Ejector Setttings
		BOOL SetExcludeMachineParameters(BOOL bExcludeMachineParam);
		BOOL SetPortablePKGFile(BOOL bPortablePKGFile);

		// Bin block run time data backup fucntions
		BOOL ReadBinRunTimeFileFileList(CStringArray& szaFileList);
		BOOL SaveBinRunTimeFile(CString szDevName);
		BOOL RestoreBinRunTimeFile(CString szDevName);
		BOOL RestoreBackupMSD();	//v4.40T6

		VOID SetIsPackageFileIgnoreList(BOOL bIsIgnoreList);
		VOID SetPortablePackageFileFileList(CStringArray& szaFileList);

		VOID ReadDFFileList();
		
	protected:
		// Save to Device File (Primitive Function)
		BOOL SaveToFile(CStringArray& objFileArray, CString& szDevName);

	protected:
        CStringMapFile  m_smfFileList;		// String Map for file list
		CString			m_szFileList;		// Filename of file list

		BOOL			m_bOnlyLoadPRParam;
		BOOL			m_bOnlyLoadBinParam;
		BOOL			m_bOnlyLoadWFTParam;
		BOOL			m_bLoadPkgWithBin;			// Flag for loading with Bin
		BOOL			m_bExcludeMachineParam;		// Flag for loading the package file with WaferLoader, 
													// BinLoader, BondHead & Ejector Settings
		BOOL			m_bPortablePKGFile;

		CString			m_szBinRunTimeFileList;
		
		BOOL			m_bIsPortablePackageFileIgnoreList;
		
		CStringArray	m_szPortablePackageFileFileList;
		CStringArray	m_szaDFFileList;
}; 
