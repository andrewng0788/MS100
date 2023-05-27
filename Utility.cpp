#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <dos.h>
#include <conio.h>
#include <io.h>
#include <direct.h>
#include <errno.h>
#include <sys\stat.h>
#include "Utility.h"
#include <atlimage.h>
#include <gdiplus.h>
#include <vector>
using namespace Gdiplus;


CUtility *CUtility::m_pInstance = NULL;

CUtility::CUtility()
{
}


CUtility::~CUtility()
{

}

//================================================================
// Function Name: 		Instance
// Input arguments:     None
// Output arguments:	None
// Description:   		Get singleton instance
// Return:				None
// Remarks:				None
//================================================================
CUtility *CUtility::Instance()
{
	if (m_pInstance == NULL) 
	{
		m_pInstance = new CUtility;
	}
	return m_pInstance;
}


//================================================================
// Function Name: 		ParseRawData
// Input arguments:     szRawData
// Output arguments:	szaRawDataByCol
// Description:   		separate the string into mult-single string
// Return:				None
// Remarks:				None
//================================================================
BOOL CUtility::ParseRawData(CString szRawData, CStringArray& szaRawDataByCol)	//	data with , into string array;
{
	CString szData;
	INT nCol = -1;

	while((nCol = szRawData.Find(",")) != -1)
	{
		szData = szRawData.Left(nCol);
		szRawData = szRawData.Right(szRawData.GetLength() - nCol -1);
		szaRawDataByCol.Add(szData);
	}

	// add the last item
	if (szRawData != "")
	{
		szaRawDataByCol.Add(szRawData);
	}

	return TRUE;
}


BOOL CUtility::IsFileExist(CString szFileName)
{
//	CFileStatus status;
//	return (CFile::GetStatus(szFileName, status) == TRUE);

	if (_access((PCSTR)szFileName, 0) != 0)
	{
		//if can not access the backup folder in network
		return false;
	}
	return true;
}

//===============================================================================
//   Zip/Unzip Related Function
//==============================================================================
BOOL CUtility::SYS_ZipCommand(char *acOutputZipFile, char *acSourceFolder)
{
	char acCmdStr[_MAX_PATH];
//	struct __stat64 fs;

//	sprintf( acCmdStr, "c:\\zip -q -r -1 %s %s\\*.*", acOutputZipFile, acSourceFolder);
	sprintf(acCmdStr, "zip -q -r -1 \"%s\" %s", acOutputZipFile, acSourceFolder);
//	if (_stat64("c:\\zip.exe", &fs) == 0)
	{
//		remove(acOutputZipFile);
		RunApplication(NULL, acCmdStr);
		return TRUE;
	}
	return FALSE;
}


BOOL CUtility::SYS_ZipCommandWithoutPath(LPCTSTR szCurrentDirectory, char *acOutputZipFile, char *acSourcefile1, char *acSourcefile2)
{
	char acCmdStr[_MAX_PATH];

	sprintf(acCmdStr, "zip -q -1 \"%s\" %s  %s", acOutputZipFile, acSourcefile1, acSourcefile2);
	RunApplication(szCurrentDirectory, acCmdStr);
	return TRUE;
}

BOOL CUtility::SYS_UnZipCommand(char *acInputZipFile)
{
	char acCmdStr[_MAX_PATH];
//	struct __stat64 fs;

	sprintf(acCmdStr, "unzip -q -o %s -d c:\\", acInputZipFile); //Overwrite if file existed
//	sprintf( acCmdStr, "unzip -q -uo %s -d c:\\", acInputZipFile);
//	if (_stat64("c:\\unzip.exe", &fs) == 0)
	{
		RunApplication(NULL, acCmdStr);
//		remove(acInputZipFile);
		return TRUE;
	}
//	remove(acInputZipFile);
	return FALSE;
}

BOOL CUtility::SYS_UnZipCommandWithoutPath(LPCTSTR szCurrentDirectory, char *acInputZipFile)
{
	char acCmdStr[_MAX_PATH];

	sprintf(acCmdStr, "unzip -q -o %s", acInputZipFile); //Overwrite if file existed
	RunApplication(szCurrentDirectory, acCmdStr);
	return TRUE;
}

BOOL CUtility::RunApplication(LPCTSTR szCurrentDirectory, char const *pcCommandLine, bool bWaitClose)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO)); //set memory to 0
	si.cb = sizeof(si);

	memset(&pi, 0, sizeof(pi)); //set memory to 0
	// Start the child process. 

	if (!CreateProcess(NULL, // No module name (use command line). 
					   (char*)pcCommandLine, // Command line. 
					   NULL, // Process handle not inheritable. 
					   NULL, // Thread handle not inheritable. 
					   TRUE, // Set handle inheritance to FALSE. 
					   CREATE_NO_WINDOW, // No creation flags.
					   NULL, // Use parent's environment block. 
					   //NULL, // Use parent's starting directory. 
					   szCurrentDirectory,
					   &si, // Pointer to STARTUPINFO structure.
					   &pi)  // Pointer to PROCESS_INFORMATION structure.
	   )
	{
		return FALSE;
	}

	// Wait until child process exits.
	if (bWaitClose)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	return TRUE;
}


//===============================================================================================
LONG CUtility::FindFileInFloder(CString szPath, CString szFileName)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	long count = 0;

	hHandle = ::FindFirstFile(szPath + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Select all items that are not directory or hidden
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
			{
				CString strName(FindData.cFileName, sizeof(FindData.cFileName));
				if (strcmp(szFileName, FindData.cFileName) == 0)
				{
					count++;
					break;
				}
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}


//===============================================================================================
//		Find File 
//===============================================================================================
LONG CUtility::FindFileInAllFloder(CString szPath, CString szFileName, CString szCompareHeader, CString szCompareTail, CString &szPathFullFileName)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	long count = 0;

	hHandle = ::FindFirstFile(szPath + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Select all items that are not directory or hidden
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0) &&
				(strcmp(FindData.cFileName, ".") != 0) && 
				(strcmp(FindData.cFileName, "..") != 0))
			{
				char szSubPath[_MAX_PATH];

				strcpy(szSubPath, (const char*)szPath);
				if (szSubPath[strlen(szSubPath) - 1] != '\\')
				{
					strcat(szSubPath, "\\");
				}
				strcat(szSubPath, (const char*)FindData.cFileName);

				count = FindFileInAllFloder(szSubPath, szFileName, szCompareHeader, szCompareTail, szPathFullFileName);
				if (count == 1)
				{
					break;
				}
			}
			else if ((strcmp(FindData.cFileName, ".") != 0) && 
					(strcmp(FindData.cFileName, "..") != 0))
			{
				CString strName(FindData.cFileName, sizeof(FindData.cFileName));
				bool bFound = false;
				if (strcmp(szFileName, FindData.cFileName) == 0)
				{
					bFound  = true;
				}

				if (!bFound && !szCompareHeader.IsEmpty())
				{
					if (strName.Find(szCompareHeader) == 0)
					{
						if (!szCompareTail.IsEmpty())
						{
							long n = strName.Find(szCompareTail);
							if (n == strlen(FindData.cFileName) - szCompareTail.GetLength())
							{
								bFound = true;
							}
						}
						else
						{
							bFound = true;
						}
					}
				}

				if (bFound)
				{
					char szSubPath[_MAX_PATH];

					strcpy(szSubPath, (const char*)szPath);
					if (szSubPath[strlen(szSubPath) - 1] != '\\')
					{
						strcat(szSubPath, "\\");
					}
					strcat(szSubPath, (const char*)FindData.cFileName);

					szPathFullFileName = szSubPath;
					count++;
					break;
				}
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}


LONG CUtility::CountFileNumInFloder(CString szPath)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	CFileStatus rStatus;
	long count = 0;
	hHandle = ::FindFirstFile(szPath + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Select all items that are not directory or hidden
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
			{
				CString strName(FindData.cFileName, sizeof(FindData.cFileName));
				CString strName1 = szPath + "\\" + strName;
				if (CFile::GetStatus(strName1, rStatus))
				{
					count++;
				}
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}

//==============================================================================================
//  Count the number of the part same filename 
//==============================================================================================
LONG CUtility::CountFileNumInFloder(CString szPath, CString szPartFileName)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	CFileStatus rStatus;
	long count = 0;
	hHandle = ::FindFirstFile(szPath + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Select all items that are not directory or hidden
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
			{
				CString strName(FindData.cFileName, sizeof(FindData.cFileName));
				if (strName.Find(szPartFileName) == 0)
				{
					count++;
				}
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}


BOOL CUtility::CopyFile(const char *dest, const char *src, BOOL bOverwrite, BOOL bCheckUploadSuccess)
{
	int size;
	FILE *fd, *fs;
	char *buf, *buf2;		// [514V9-27-09#3 20160630] Akito: buf2 for compare copied content with source content

	if (strlen(dest) == 0)
	{
		return FALSE;
	}
	if (strlen(src) == 0)
	{
		return FALSE;
	}
	if ((fs = fopen(src, "rb")) == NULL)
	{
		return FALSE;
	}
	size = _filelength(_fileno(fs));
	buf = (char*)malloc(size);
	if (buf == NULL)
	{
		fclose(fs);
		return FALSE;
	}
	if (fread(buf, 1, size, fs) != size)
	{
		fclose(fs);
		free(buf);
		return FALSE;
	}
	fclose(fs);

	if (!bOverwrite)
	{
		if ((fd = fopen(dest, "a+b")) == NULL)
		{
			free(buf);
			return FALSE;
		}
	}
	else
	{
		if ((fd = fopen(dest, "wb")) == NULL)
		{
			free(buf);
			return FALSE;
		}
	}
	fwrite(buf, 1, size, fd);
	if (bCheckUploadSuccess)		// [514V9-27-09#3 20160630] Akito
	{
		fclose(fd);
		if ((fd = fopen(dest, "rb")) == NULL)
		{
			free(buf);
			return FALSE;
		}
		size = _filelength(_fileno(fd));
		buf2 = (char*)malloc(size);
		if (buf2 != 0)
		{
			if (fread(buf2, 1, size, fd) != size)		// Open destination file
			{
				fclose(fd);
				free(buf);
				free(buf2);
				return FALSE;
			}
			if (memcmp(buf, buf2, size) != 0)		// Compare source and destination content
			{
				fclose(fd);
				free(buf);
				free(buf2);
				return FALSE;
			}
			free(buf2);		// [514V9-27-29#2 20160826] Akito: Free buf2 without init before -> load package file crash in Debug mode
		}
	}
	fclose(fd);
	free(buf);
	return TRUE;
}


//================================================================
// Function Name: 		Copy One Files
// Remarks:				None
//================================================================
BOOL CUtility::CopyOneFile(WIN32_FIND_DATA  FindData, const CString szSrcPath, const CString szDestPath, BOOL bOverwrite, BOOL bCheckUploadSuccess)
{
	CFileStatus rStatus;
	// Select all items that are not directory or hidden
	if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
		((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
	{
		CString strSrcName(FindData.cFileName, sizeof(FindData.cFileName));
		strSrcName = szSrcPath + "\\" + strSrcName;
		if (CFile::GetStatus(strSrcName, rStatus))
		{
			CString strDestName = szDestPath + "\\" + FindData.cFileName;
			if (!bOverwrite && (_access((const char*)strDestName, F_OK) == F_OK))
			{
				char szSrcName[_MAX_PATH], szDestName[_MAX_PATH];
				strcpy(szSrcName, (const char*)strSrcName);
				strcpy(szDestName, (const char*)strDestName);
				return CopyFile(szDestName, szSrcName, bOverwrite, bCheckUploadSuccess);
			}
			else
			{
				return CopyFile(strDestName, strSrcName, TRUE, bCheckUploadSuccess);
			}
		}
	}
	return TRUE;
}

//================================================================
// Function Name: 		Copy All Files in Folder
//						bCopyOption --  0-->Overite
//						bCopyOption --  1-->Add
// Remarks:				None
//================================================================
LONG CUtility::CopyAllFileInFloder(const CString szSrcPath, const CString szDestPath, BOOL bOverwrite, BOOL bCheckUploadSuccess)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	long count = 0;
	char szTemp[_MAX_PATH];
	hHandle = ::FindFirstFile(szSrcPath + "\\*.*", &FindData);

	strcpy(szTemp, (const char*)szDestPath);
	if (_access(szTemp, F_OK) != F_OK)
	{
		_mkdir(szTemp);
	}

	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!CopyOneFile(FindData, szSrcPath, szDestPath, bOverwrite, bCheckUploadSuccess))
			{
				return -1;
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}

//================================================================
// Function Name: 		Copy All Files in Folder
// Remarks:				None
//================================================================
LONG CUtility::CopyAllFolderFile(CString szSrcPath, CString szDestPath, BOOL bCheckUploadSuccess, BOOL bAutoDelete)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	long count = 0;
	char szTemp[_MAX_PATH];
	hHandle = ::FindFirstFile(szSrcPath + "\\*.*", &FindData);

	strcpy(szTemp, (const char*)szDestPath);
	if (_access(szTemp, F_OK) != F_OK)
	{
		_mkdir(szTemp);
	}

	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0) &&
				(strcmp(FindData.cFileName, ".") != 0) && 
				(strcmp(FindData.cFileName, "..") != 0))
			{
				char szSubSrcPath[_MAX_PATH];
				char szSubDestPath[_MAX_PATH];

				strcpy(szSubSrcPath, (const char*)szSrcPath);
				if (szSubSrcPath[strlen(szSubSrcPath) - 1] != '\\')
				{
					strcat(szSubSrcPath, "\\");
				}
				strcat(szSubSrcPath, (const char*)FindData.cFileName);

				strcpy(szSubDestPath, (const char*)szDestPath);
				if (szSubDestPath[strlen(szSubDestPath) - 1] != '\\')
				{
					strcat(szSubDestPath, "\\");
				}
				strcat(szSubDestPath, (const char*)FindData.cFileName);
				if (_access(szSubDestPath, F_OK) != F_OK)
				{
					_mkdir(szSubDestPath);
				}

				if (CopyAllFolderFile(szSubSrcPath, szSubDestPath, bCheckUploadSuccess, bAutoDelete) == -1)		// [514V9-27-09#3 20160630] Akito: To skip copy strip map and do not delete back up strip map file
				{
					return -1;
				}
			}
			else
			{
				if (!CopyOneFile(FindData, szSrcPath, szDestPath, bCheckUploadSuccess))
				{
					return -1;		// if copy file fail, return -1 to indicate do not delete folder
				}
				if (bAutoDelete)
				{
					DeleteOneFiles(FindData, szSrcPath, "", 0);
				}
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}


//================================================================
// Function Name: 		Get One File Data/Time
// Remarks:				None
//================================================================
LONG CUtility::GetOneFileCreatedTime(WIN32_FIND_DATA  FindData, CString szPath)
{
	LONG lCount = 0;
	CFileStatus rStatus;
	LONG lCreatedTime_sec;

	lCreatedTime_sec = -1;
	// Select all items that are not directory or hidden
	if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
		((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
	{
		CString strName(FindData.cFileName, sizeof(FindData.cFileName));
		CString strName1 = szPath + "\\" + strName;

		struct __stat64 fs;
		time_t t;
		char szFileName[_MAX_PATH];
		strcpy(szFileName, (const char*)strName1);
		if (_stat64(szFileName, &fs) == 0)
		{
			t = time(NULL);
			lCreatedTime_sec = (int)(difftime(t, (time_t)fs.st_mtime));
		}
	}
	return lCreatedTime_sec;
}

//================================================================
// Function Name: 		Delete One File
// Remarks:				None
//================================================================
LONG CUtility::DeleteOneFiles(WIN32_FIND_DATA  FindData, CString szPath, const CString szExt, short hLifeDayTime)
{
	LONG lCount = 0;
	CFileStatus rStatus;

	// Select all items that are not directory or hidden
	if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
		((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
	{
		CString strName(FindData.cFileName, sizeof(FindData.cFileName));
		CString strName1 = szPath + "\\" + strName;

		//Not file extension
		if (!szExt.IsEmpty() && (strName.Find(szExt) <= 0))
		{
			return 0;
		}
		
		if (hLifeDayTime > 0)
		{
			LONG lCreatedTime_sec = 0;
			if ((lCreatedTime_sec = GetOneFileCreatedTime(FindData, szPath)) >= 0)
			{
				int nDays = (int)(lCreatedTime_sec / (3600 * 24));
				if (nDays >= hLifeDayTime)
				{
					if (_access((const char*)strName1, F_OK) == 0)
					{
						remove((const char*)strName1);
						lCount++;
					}
				}
			}
			return lCount;
		}

		if (CFile::GetStatus(strName1, rStatus))
		{
			CFile::Remove(strName1);
		}
		if (CFile::GetStatus(strName1, rStatus))
		{
			lCount++;
			try
			{
				CFile::Remove(strName1);
			}
			catch (CFileException *pEx)
			{
#ifdef _DEBUG
				afxDump << "File " << strName1 << " cannot be removed\n";
#endif
				pEx->Delete();
			}
		}
	}
	return lCount;
}


//================================================================
// Function Name: 		SG_DeleteAllFiles
// Remarks:				None
//================================================================
LONG CUtility::DeleteAllFiles(CString szFullPathName, short hLifeDayTime)
{
//		char szFileName[_MAX_PATH];
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	long count = 0;
	hHandle = ::FindFirstFile(szFullPathName + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			count += DeleteOneFiles(FindData, szFullPathName, "", hLifeDayTime);
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}


//================================================================
// Function Name: 		SG_DeleteAllFiles
// Remarks:				None
//================================================================
LONG CUtility::DeleteAllFolderFiles(CString szFullPathName, const CString szExt, short hLifeDayTime)
{
//		char szFileName[_MAX_PATH];
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	CFileStatus rStatus;
	long count = 0;
	hHandle = ::FindFirstFile(szFullPathName + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0) &&
				(strcmp(FindData.cFileName, ".") != 0) && 
				(strcmp(FindData.cFileName, "..") != 0))
			{
				char szFilePath[_MAX_PATH];

				strcpy(szFilePath, (const char*)szFullPathName);
				if (szFilePath[strlen(szFilePath) - 1] != '\\')
				{
					strcat(szFilePath, "\\");
				}
				strcat(szFilePath, (const char*)FindData.cFileName);
				count += DeleteAllFolderFiles(szFilePath, szExt, hLifeDayTime);
				if (hLifeDayTime > 0)
				{
					if (CountFileNumInFloder(szFilePath) == 0)
					{
						RemoveDirectory(szFilePath);
					}
				}
				else
				{
					if (szExt.IsEmpty() || (CountFileNumInFloder(szFilePath) == 0))
					{
						RemoveDirectory(szFilePath);
					}
				}
			}
			else
			{
				count += DeleteOneFiles(FindData, szFullPathName, szExt, hLifeDayTime);
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}

//================================================================
// Function Name: 		DeleteExpiredFile
// Input arguments:     Path, Lifetime
// Output arguments:	None
// Description:   	    Delete Expired File in a given folder
// Return:				None
// Remarks:				None
//================================================================
LONG CUtility::DeleteExpiredFile(char *szFullPathName, short hLifeDayTime)
{
/*
	char szFileName[_MAX_PATH];
	struct __stat64 fs;
	time_t t;
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	CFileStatus rStatus;
	int nDays;
	LONG lCount = 0;

	strcpy(szFileName, szFullPathName);
	if (szFileName[strlen(szFileName)-1] == '\\')
		strcat( szFileName, "*.*" );
	else
		strcat( szFileName, "\\*.*" );

	hHandle = ::FindFirstFile(szFileName, &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Delete the file
			strcpy( szFileName, szFullPathName);
			if (szFileName[strlen(szFileName)-1] != '\\')
				strcat( szFileName, "\\" );

			strcat( szFileName, FindData.cFileName);
			if (_stat64(szFileName, &fs) == 0)
			{
				t = time(NULL);
				nDays = (int)(difftime(t, (time_t)fs.st_mtime) /(3600*24));
				if (nDays >= hLifeDayTime)
					if (_access(szFileName, F_OK) == 0)
					{
						remove(szFileName);
						lCount++;
					}
			}
			// Get the next file in the directory
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return lCount;
*/
	return DeleteAllFolderFiles(szFullPathName, "", hLifeDayTime);
}


//======================================================================================
//   Get File Data/Time
//=======================================================================================
//================================================================
// Function Name: 		Get all File Data/Time
// Remarks:				None
//================================================================
LONG CUtility::GetAllFilesCreatedTime(CString szFullPathName, CString szPartFileName)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	long count = 0;
	hHandle = ::FindFirstFile(szFullPathName + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Select all items that are not directory or hidden
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
			{
				CString strName(FindData.cFileName, sizeof(FindData.cFileName));
				if (strName.Find(szPartFileName) == 0)
				{
					LONG lCreatedFileTime = 0;
					lCreatedFileTime = GetOneFileCreatedTime(FindData, szFullPathName);
					count++;
				}
			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}
	return count;
}


CString CUtility::FilterFileName(LPCSTR lpcFullFilePath)
{
	CString strFile;
	if (lpcFullFilePath)
	{
		char szName[_MAX_PATH] = {0};
		char szExt[_MAX_EXT] = {0};
		_splitpath(lpcFullFilePath, NULL, NULL, szName, szExt);
		strFile = szName;
		strFile += szExt;
	}
	return strFile;
}


BOOL CUtility::SaveScreenBMPFile(CString szFileName)
{
	HWND hWnd = ::GetDesktopWindow();
	if (!hWnd)
	{
		return FALSE;
	}

	HDC hDC = ::GetDC(hWnd);
	if (!hDC)
	{
		return FALSE;
	}

	RECT rect;
	::GetClientRect(hWnd, &rect);

	HDC memDc;
	memDc = ::CreateCompatibleDC(hDC);
	if (!memDc)
	{
		return FALSE;
	}

	HBITMAP hBitMap;
	hBitMap = ::CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	if (!hBitMap)
	{
		return FALSE;
	}
	HBITMAP hOldMap;
	hOldMap = (HBITMAP)::SelectObject(memDc, hBitMap);
	
	::BitBlt(memDc, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);

	//convert bmp file into png file
	CImage TempImageObj;
	TempImageObj.Attach(hBitMap);
	TempImageObj.Save(szFileName, ImageFormatJPEG);
	TempImageObj.Detach();
	
	::SelectObject(memDc, hOldMap);
	::DeleteObject(hBitMap);
	::DeleteDC(memDc);
	::DeleteDC(hDC);
	return TRUE;
}


/*
void CAD8312FCApp::CheckMemoryUsage(CString szTitle)
{
	if (!IsEnableMemoryUsageLog())
	{
	}
	else if (m_pMemoryUsageLogFile != NULL)
	{
		unsigned long ulUsedSize = 0, ulFreeSize = 0;
		DBG_HeapStatus(TRUE, &ulUsedSize);
		DBG_HeapStatus(FALSE, &ulFreeSize);

		CString strWrite = "";
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);
		strWrite.Format(CTime::GetCurrentTime().Format("%B %d,  %H:%M:%S") + "\t%ld\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%Ld\t", 
						statex.dwMemoryLoad,
						statex.ullTotalPhys / 1024,
						statex.ullAvailPhys / 1024,
						statex.ullTotalPageFile / 1024,
						statex.ullAvailPageFile / 1024,
						statex.ullTotalVirtual / 1024,
						statex.ullAvailVirtual / 1024,
						statex.ullAvailExtendedVirtual / 1024,
						_round(ulUsedSize / 1024), _round(ulFreeSize / 1024));
		strWrite.Append(szTitle);
		strWrite.Append("\n");
		m_pMemoryUsageLogFile->log(strWrite);

//		MEMORYSTATUS s;
//		GlobalMemoryStatus(&s);
//		m_pMemoryUsageLogFile->log(szTitle);
//		m_pMemoryUsageLogFile->log("Memory Usage = %u, Total Physical Memeory = %u, Available Physical Memeory = %u, Total Page File = %u, Avaliable Page File = %u,  Total Virtual = %u, Avliable Virtual = %u",
//			s.dwMemoryLoad, s.dwTotalPhys, s.dwAvailPhys, s.dwTotalPageFile, s.dwAvailPageFile, s.dwTotalVirtual, s.dwAvailVirtual);

		CString szMess;
		szMess.Format("Memory Usage = %u", statex.dwMemoryLoad);
		DisplayMessage(szMess);
		szMess.Format("Total Physical Memeory = %u", statex.ullTotalPhys / 1024);
		DisplayMessage(szMess);
		szMess.Format("Available Physical Memeory = %u", statex.ullAvailPhys / 1024);
		DisplayMessage(szMess);
		szMess.Format("Total Page File = %u", statex.ullTotalPageFile / 1024);
		DisplayMessage(szMess);
		szMess.Format("Avaliable Page File = %u", statex.ullAvailPageFile / 1024);
		DisplayMessage(szMess);
		szMess.Format("Total Virtual = %u", statex.ullTotalVirtual / 1024);
		DisplayMessage(szMess);
		szMess.Format("Avliable Virtual = %u", statex.ullAvailVirtual / 1024);
		DisplayMessage(szMess);
		szMess.Format("Avliable Extend Virtual = %u", statex.ullAvailExtendedVirtual / 1024);
		DisplayMessage(szMess);
		szMess.Format("Heap Used Size = %u", _round(ulUsedSize / 1024));
		DisplayMessage(szMess);
		szMess.Format("Heap Avalible Size = %u", _round(ulFreeSize / 1024));
		DisplayMessage(szMess);
	}
}

*/


VOID CUtility::GetFileNameListWithCreateTimeOrder(CString szFolderPath, CStringArray &FileNameList)
{
	WIN32_FIND_DATA  FindData;
	HANDLE hHandle;
	LONG lTempCreatedFileTime = 0;
	vector<WIN32_FIND_DATA> FindDataArray;

	hHandle = ::FindFirstFile(szFolderPath + "\\*.*", &FindData);
	if (hHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Select all items that are not directory or hidden
			if (((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) &&
				((FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
			{
				//For First Case, FindDataArray is empty
				if (lTempCreatedFileTime == 0)
				{
					FindDataArray.push_back(FindData);
					lTempCreatedFileTime = 1;
					continue;
				}

				lTempCreatedFileTime = GetOneFileCreatedTime(FindData, szFolderPath);
				for (int i = 0; i < FindDataArray.size(); i++)
				{
					if (lTempCreatedFileTime < GetOneFileCreatedTime(FindDataArray[i], szFolderPath))
					{
						vector<WIN32_FIND_DATA> TempFindDataArray;
						TempFindDataArray.push_back(FindData);

						FindDataArray.insert(FindDataArray.begin() + i, TempFindDataArray.begin(), TempFindDataArray.end());
						break;
					}

					//For Oldest file, don't insert to array
					if (i == FindDataArray.size() - 1) 
					{
						FindDataArray.push_back(FindData);
					}
				}

			}
		} while (FindNextFile(hHandle, &FindData));
		FindClose(hHandle);
	}

	//Set all file name with create time order to FileNameList
	for(int i = 0; i < FindDataArray.size(); i++)
	{
		FileNameList.Add(FindDataArray[i].cFileName);
	}
}