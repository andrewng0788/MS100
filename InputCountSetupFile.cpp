#include "stdafx.h" 
#include "InputCountSetupFile.h"
#include "stdio.h"
#include "StringMapFile.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include "LogFileUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInputCountSetupFile::CInputCountSetupFile()
{

}

CInputCountSetupFile::~CInputCountSetupFile()
{
	CleanUp();
}

VOID CInputCountSetupFile::CleanUp()
{
	m_szErrorCode = _T("");
	m_szaHeaderItem.RemoveAll();
	m_aData.RemoveAll();
}

BOOL CInputCountSetupFile::ReadFile(CString szFilename)
{
	CStdioFile cfFile;
	CString szStr;
	CString szTemp;
	CString szItem, szValue;
	CString szGrade, szInputCount;
	INPUT_COUNT_INFO stTemp;
	INT nCol = -1;

	// clear previous value first
	CleanUp();
	
	if (cfFile.Open(szFilename, CFile::modeRead|CFile::shareExclusive|CFile::typeText)== FALSE)
	{
		m_szErrorCode = "Fail to open file: " + szFilename;
		return FALSE;
	}

	// Read the header
	if (cfFile.ReadString(szStr) == FALSE)
	{
		cfFile.Close();
		m_szErrorCode = "Fail to read file content at: " + szFilename;
		return FALSE;
	}

	while ((nCol = szStr.Find(",")) != -1)
	{
		szTemp = szStr.Left(nCol);
		szStr = szStr.Right(szStr.GetLength()- nCol -1);
		m_szaHeaderItem.Add(szTemp);
	}

	// Add back the last item
	m_szaHeaderItem.Add(szStr);

	//for (INT i=0; i<m_szaHeaderItem.GetSize(); i++)
	//{
	//	CMSLogFileUtility::Instance()->MS_LogOperation(m_szaHeaderItem.GetAt(i));
	//}

	// Read the content
	while (cfFile.ReadString(szStr) != FALSE)
	{
		if (ExtractData(szStr, szGrade, szInputCount) == FALSE)
		{
			cfFile.Close();
			m_szErrorCode = "Fail to ExtractData";
			return FALSE;
		}
		
		if (atoi(szInputCount) == 0)
		{
			continue;
		}

		stTemp.ucGrade = atoi(szGrade);
		stTemp.lInputCount = atoi(szInputCount);
		CString szData;
		szData.Format("ucGrade:%d InputCount:%d", stTemp.ucGrade, stTemp.lInputCount);
		CMSLogFileUtility::Instance()->MS_LogOperation(szData);
		m_aData.Add(stTemp);
	}

	cfFile.Close();
	return TRUE;
}

BOOL CInputCountSetupFile::ExtractData(CString szStr, CString& szBin, CString& szInputCount)
{
	CString szTemp;
	CString szHeader;
	INT nCol = 0;
	INT nIndex = 0;

	while ((nCol = szStr.Find(",")) != -1)
	{
		szTemp = szStr.Left(nCol);
		szStr = szStr.Right(szStr.GetLength()- nCol -1);
	
		if (nIndex >= m_szaHeaderItem.GetSize())
		{
			return FALSE;
		}

		szHeader = m_szaHeaderItem.GetAt(nIndex);

		if (szHeader == HEADER_NEW_BIN)
		{
			szBin = szTemp;
		}
		else if (szHeader == HEADER_FULL_BIN_DIE)
		{
			szInputCount = szTemp;
			return TRUE;
		}

		nIndex =  nIndex + 1;
	}
	
	return FALSE;
}

BOOL CInputCountSetupFile::GetInputCount(unsigned char ucGrade, LONG& InputCount)
{
	INPUT_COUNT_INFO stInputCountInfo;

	for (INT i=0; i<m_aData.GetSize(); i++)
	{
		stInputCountInfo = m_aData.GetAt(i);
		if (stInputCountInfo.ucGrade == ucGrade)
		{
			InputCount = stInputCountInfo.lInputCount;
			return TRUE;
		}
	}

	return FALSE;
}
		
CString CInputCountSetupFile::GetLastError()
{
	return m_szErrorCode;
}

