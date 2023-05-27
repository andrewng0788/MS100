
#include "stdafx.h"
#include "AlarmCodeTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CIgnoreAlarmCodeTable *CIgnoreAlarmCodeTable::m_pInstance = NULL;
//======================================================================================
//			Module Error Code
//======================================================================================
CAlarmCode::CAlarmCode()
{
};

VOID CAlarmCode::SetAlarmCode(const CString szAlarmCode, const CString szAlarmDescription)
{
	m_szAlarmCode			= szAlarmCode;
	m_szAlarmDescription	= szAlarmDescription;
}

BOOL CAlarmCode::IsSame(const CString szAlarmCode)
{
	if (this == NULL)
	{
		return FALSE;
	}
	return (m_szAlarmCode == szAlarmCode);
}

//================================================================
//	Ignore Alarm Code Table
//================================================================
CIgnoreAlarmCodeTable::CIgnoreAlarmCodeTable()
{
	m_IgnoreAlarmCodeTables.RemoveAll();
}


CIgnoreAlarmCodeTable::~CIgnoreAlarmCodeTable()
{
	CAlarmCode *pAlarmCode;
	LONG lSize = (LONG)m_IgnoreAlarmCodeTables.GetSize();
	for (LONG i = 0; i < lSize; i++)
	{
		pAlarmCode = (CAlarmCode *)m_IgnoreAlarmCodeTables[i];
		delete pAlarmCode;
		pAlarmCode = NULL;
	}
	m_IgnoreAlarmCodeTables.RemoveAll();
}

//================================================================
// Function Name: 		Instance
// Input arguments:     None
// Output arguments:	None
// Description:   		Get singleton instance
// Return:				None
// Remarks:				None
//================================================================
CIgnoreAlarmCodeTable *CIgnoreAlarmCodeTable::Instance()
{
	if (m_pInstance == NULL) 
	{
		m_pInstance = new CIgnoreAlarmCodeTable;
	}
	return m_pInstance;
}


//=================================================================================
//  Ignore error Code
//=================================================================================
VOID CIgnoreAlarmCodeTable::CreateIgnoreAlarmCodeTable()
{
	CStdioFile FileCSV;
	CString strLine, szAlarmCode, szAlarmDescription;
	LONG n = 0, lCount = 0;
	CAlarmCode *pAlarmCode;

	if (!FileCSV.Open(IGNORE_ALARM_CODE_DATABASE, CFile::modeRead)) 
	{
//		AfxMessageBox("Open IgnoreAlarmCodeTable.csv failure");
		return;
	}
	while (FileCSV.ReadString(strLine))
	{
		if (lCount > 0)
		{
			n	= strLine.Find(",");
			szAlarmCode			= strLine.Left(n);
			szAlarmDescription	= strLine.Right(strLine.GetLength() - n - 1);
			pAlarmCode			= new CAlarmCode;
			pAlarmCode->SetAlarmCode(szAlarmCode, szAlarmDescription);
			m_IgnoreAlarmCodeTables.Add(pAlarmCode);
		}
		lCount++;
	}
	FileCSV.Close();
}


BOOL CIgnoreAlarmCodeTable::IsInIgnoreAlarmCodeTable(CString szAlarmCode)
{
	CAlarmCode *pAlarmCode;

	LONG lSize = (LONG)m_IgnoreAlarmCodeTables.GetSize();
	for (LONG i = 0; i < lSize; i++)
	{
		pAlarmCode = (CAlarmCode *)m_IgnoreAlarmCodeTables[i];

		if (pAlarmCode->IsSame(szAlarmCode))
		{
			return TRUE;
		}
	}
	return FALSE;
}

VOID CIgnoreAlarmCodeTable::ClearIgnoreAlarmCodeTable()
{
	CAlarmCode *pAlarmCode;
	LONG lSize = (LONG)m_IgnoreAlarmCodeTables.GetSize();
	for (LONG i = 0; i < lSize; i++)
	{
		pAlarmCode = (CAlarmCode *)m_IgnoreAlarmCodeTables[i];
		delete pAlarmCode;
	}
	m_IgnoreAlarmCodeTables.RemoveAll();
}




