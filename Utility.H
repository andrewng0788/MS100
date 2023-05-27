// 
// =====================================================================
//================================================================
// #include and #define
//================================================================
#ifndef __UTILITY_H
#define __UTILITY_H
#pragma once
/*
#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_DECLSPEC __declspec(dllexport)
#else
#define UTILITY_LIB_DECLSPEC __declspec(dllimport)
#endif
*/
//================================================================
// Class definition
//================================================================
class CUtility : public CObject
{
#define F_OK 0
public:
	CUtility();
	~CUtility();
	static CUtility *Instance();
private:

public:
	BOOL ParseRawData(CString szRawData, CStringArray& szaRawDataByCol);

	BOOL IsFileExist(CString szFileName);
	BOOL SYS_ZipCommand(char *acOutputZipFile, char *acSourceFolder);
	BOOL SYS_ZipCommandWithoutPath(LPCTSTR szCurrentDirectory, char *acOutputZipFile, char *acSourcefile1, char *acSourcefile2);
	BOOL SYS_UnZipCommand(char *acInputZipFile);
	BOOL SYS_UnZipCommandWithoutPath(LPCTSTR szCurrentDirectory, char *acInputZipFile);
	BOOL RunApplication(LPCTSTR szCurrentDirectory, char const *pcCommandLine, bool bWaitClose = true);
	LONG FindFileInFloder(CString szPath, CString szFileName);
	LONG FindFileInAllFloder(CString szPath, CString szFileName, CString szCompareHeader, CString szCompareTail, CString &szPathFullFileName);
	LONG CountFileNumInFloder(CString szPath);
	LONG CountFileNumInFloder(CString szPath, CString szPartFileName);
	BOOL CopyFile(const char *dest, const char *src, BOOL bOverwrite = TRUE, BOOL bCheckUploadSuccess = FALSE);		// [514V9-27-09#3 20160630] Akito
	BOOL CopyOneFile(WIN32_FIND_DATA  FindData, const CString szSrcPath, const CString szDestPath, BOOL bOverwrite = TRUE, BOOL bCheckUploadSuccess = FALSE);		// [514V9-27-09#3 20160630] Akito
	LONG CopyAllFileInFloder(CString szSrcPath, CString szDestPath, BOOL bOverwrite = TRUE, BOOL bCheckUploadSuccess = FALSE);		// [514V9-27-09#3 20160630] Akito
	LONG CopyAllFolderFile(CString szSrcPath, CString szDestPath, BOOL bCheckUploadSuccess = FALSE, BOOL bAutoDelete = FALSE);

	LONG GetOneFileCreatedTime(WIN32_FIND_DATA  FindData, CString szPath);
	LONG DeleteOneFiles(WIN32_FIND_DATA  FindData, CString szPath, const CString szExt, short hLifeDayTime);
	LONG DeleteAllFiles(CString szProcessProgramPath, short hLifeDayTime = 0);
	LONG DeleteAllFolderFiles(CString szProcessProgramPath, const CString szExt = "", short hLifeDayTime = 0);
	LONG DeleteExpiredFile(char *szFullPathName, short hLifeDayTime);
	
	VOID GetFileNameListWithCreateTimeOrder(CString szFolderPath, CStringArray &FileNameList);

	LONG GetAllFilesCreatedTime(CString szFullPathName, CString szPartFileName);
	CString FilterFileName(LPCSTR lpcFullFilePath);

	BOOL SaveScreenBMPFile(CString szFileName);
private:


public:
	static CUtility *m_pInstance;
};
#endif