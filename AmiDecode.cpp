#include "stdafx.h" 
#include "AmiFileDecode.h"
#include "stdio.h"
#include "StringMapFile.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include "LogFileUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAmiFileDecode::CAmiFileDecode()
{
	CleanUp();	//CodeStandard
}

CAmiFileDecode::~CAmiFileDecode()
{
	CleanUp();
}

VOID CAmiFileDecode::CleanUp()
{
	//CodeStandard
	TRY 
	{
		m_bResetAmiLoadFailStatus = FALSE;
		m_szDiePitchX = "";
		m_szDiePitchY = "";
		m_szMaxCount = "";
		m_szBinNo = "";
		m_szPKGFilename = "";
		m_szBinSummaryFilename = "";

		m_szaHeaderItem.RemoveAll();
		m_szaItemValue.RemoveAll();
	} 
	CATCH(CException, e) 
	{
	} END_CATCH
}

BOOL CAmiFileDecode::ReadCsvFormat(CONST CString szFilename)
{
	CStdioFile cfFile;
	CString szStr;
	CString szTemp;
	CString szItem, szValue;
	INT nCol = -1;

	// clear previous value first
	CleanUp();
	
	BOOL bOpenStatus = cfFile.Open(szFilename, CFile::modeRead | CFile::shareExclusive | CFile::typeText);
	if (bOpenStatus == FALSE)
	{
		return FALSE;
	}

	// Read the header
	if (cfFile.ReadString(szStr) == FALSE)
	{
		cfFile.Close();
		return FALSE;
	}

	if (szStr == HEADER_RESET)
	{
		m_bResetAmiLoadFailStatus = TRUE;
		cfFile.Close();
		return TRUE;
	}
	
	while ((nCol = szStr.Find(",")) != -1)
	{
		szTemp = szStr.Left(nCol);
		szStr = szStr.Right(szStr.GetLength() - nCol - 1);
		m_szaHeaderItem.Add(szTemp);
	}

	// Add back the last item
	m_szaHeaderItem.Add(szStr);

	// Read the content
	if (cfFile.ReadString(szStr) == FALSE)
	{
		cfFile.Close();
		return FALSE;
	}

	cfFile.Close();

	while ((nCol = szStr.Find(",")) != -1)
	{
		szTemp = szStr.Left(nCol);
		szStr = szStr.Right(szStr.GetLength() - nCol - 1);
		m_szaItemValue.Add(szTemp);
	}

	m_szaItemValue.Add(szStr);

	if (m_szaHeaderItem.GetSize() != m_szaItemValue.GetSize())
	{
		return FALSE;
	}

	for (INT i = 0; i < m_szaHeaderItem.GetSize(); i++)
	{
		szItem = m_szaHeaderItem.GetAt(i);
		szValue = m_szaItemValue.GetAt(i);

		if (szItem == HEADER_DIEPITCH)
		{
			nCol = szValue.Find('*');
			
			if (nCol != -1)
			{
				m_szDiePitchX = szValue.Left(nCol);
				m_szDiePitchY = szValue.Right(szValue.GetLength() - nCol - 1);
			}
		}
		else if (szItem == HEADER_MAXCOUNT)
		{
			m_szMaxCount = szValue;
		}
		else if (szItem == HEADER_BIN)
		{
			m_szBinNo = szValue;
		}
		else if (szItem == HEADER_RECIPE)
		{
			m_szPKGFilename = szValue;
		}
		else if (szItem == HEADER_LOTID)
		{
			m_szBinSummaryFilename = szValue;
		}
	}

	if (m_szBinSummaryFilename == "")
	{
		return FALSE;
	}
	
	if (m_szPKGFilename == "")
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAmiFileDecode::ReadFile(CString szFilename)
{
	CStdioFile cfFile;
	CString szStr;
	CString szItem, szValue;
	INT nCol = -1;

	// clear previous value first
	CleanUp();
	
	BOOL bOpenStatus = cfFile.Open(szFilename, CFile::modeRead | CFile::shareExclusive | CFile::typeText);
	if (bOpenStatus == FALSE)
	{
		return FALSE;
	}

	while (cfFile.ReadString(szStr))
	{
		nCol = szStr.Find(',');

		if (nCol != -1)
		{
			szItem = szStr.Left(nCol);
			szValue = szStr.Right(szStr.GetLength() - nCol - 1);

		}

		if (szItem == HEADER_DIEPITCH)
		{
			nCol = szValue.Find('*');
			
			if (nCol != -1)
			{
				m_szDiePitchX = szValue.Left(nCol);
				m_szDiePitchY = szValue.Right(szValue.GetLength() - nCol - 1);
			}
		}
		else if (szItem == HEADER_MAXCOUNT)
		{
			m_szMaxCount = szValue;
		}
		else if (szItem == HEADER_BIN)
		{
			m_szBinNo = szValue;
		}
		else if (szItem == HEADER_RECIPE)
		{
			m_szPKGFilename = szValue;
		}
		else if (szItem == HEADER_LOTID)
		{
			m_szBinSummaryFilename = szValue;
		}
		else if (szItem == HEADER_RESET)
		{
			m_bResetAmiLoadFailStatus = TRUE;
			cfFile.Close();
			return TRUE;
		}
	}
	
	cfFile.Close();

	if (m_szBinSummaryFilename == "")
	{
		return FALSE;
	}
	
	if (m_szPKGFilename == "")
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAmiFileDecode::IsResetAmiLoadFailStatus()
{
	return m_bResetAmiLoadFailStatus;
}
