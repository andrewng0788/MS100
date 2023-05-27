#include "stdafx.h"
#include "MarkConstant.h" 
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_Log.h"
#include "math.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CWaferTable::CPOutputLibraryAboutDialog(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;	
}

VOID CWaferTable::TesterCORGET(CString szIndex, CString szTestName, CString szValue, CString& szResult)
{
	CString szRequest;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (ConnectTesterTerminal())
	{
		if (szTestName == "IR")
			szTestName = "IR(mA)";
		else if (szTestName == "LOP1")
			szTestName = "PWR";

		szRequest = "CORGET," + szIndex + "," + szTestName + "," + szValue; 
		SendRequestToTesterTerminal(szRequest, szResult);
		DisconnectTesterTerminal();
	}
}

VOID CWaferTable::TesterCORSET(CString szIndex, CString szTestName, CString szValue, 
		CString szEnable, CString szMode, CString szGain, CString szOffset, CString& szResult)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (szTestName == "IR")
	{
		szTestName = "IR(mA)";
		//double dTemp;
		//dTemp = atof((LPCTSTR)szOffset);
		//dTemp = dTemp/1000;
		//szOffset.Format("%f",dTemp);
	}
	else if (szTestName == "LOP1")
		szTestName = "PWR";

	CString szRequest;
	if (ConnectTesterTerminal())
	{
		szRequest = "CORSET," + szIndex + "," + szTestName + "," + szValue + "," + szEnable + "," + szMode + "," + szGain +
			/*"," + szGain   +*/ "," + szOffset;
		SendRequestToTesterTerminal(szRequest, szResult);
		DisconnectTesterTerminal();
	}
}

VOID CWaferTable::GetTestItemsList(CStringArray &szaTestItemList)
{
	CString szReply;
	if (ConnectTesterTerminal())
	{
		SendRequestToTesterTerminal("TESTS", szReply);
		szReply.Replace("TESTS,", "");
		szReply.Replace(";", "");
		DisconnectTesterTerminal();
		szReply = szReply + ",INT1";
		ParseRawData(szReply, szaTestItemList);
	}
}


BOOL CWaferTable::GetTestingItemsForNearCheckDummy(CStringArray &szaTestItemList)
{
	GetTestItemsList(szaTestItemList);

	// copy back to the Test Item List
	CString szMsg = "";
	for (INT i = 0; i < szaTestItemList.GetSize(); i++)
	{
		if (i == 0)
		{
			szMsg = szaTestItemList.GetAt(i);
		}
		else
		{
			szMsg = szMsg + "," + szaTestItemList.GetAt(i);
		}
	}
	SetErrorMessage(szMsg);

	return TRUE;
}

BOOL CWaferTable::IsItemValid(CString szItemValue)
{
	CString szCheck = szItemValue.MakeUpper();
	if (szCheck.Find("INFINITY") != -1	||
		szCheck.Find("UNDER") != -1		||
		szCheck.Find("OVER") != -1		||
		szCheck.Find("NAN") != -1)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CWaferTable::CP_RelocateHomeDie()
{
	return FALSE;
}

VOID CWaferTable::ReprobeAtMapToCleanUp(LONG lAsmRow, LONG lAsmCol, CONST BOOL bRestoreMap)
{
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucGrade  = ucNullBin;

	if( IsBlockProbe() )
	{
		m_lLastProbeRow = GetMapValidMinRow();
		m_lLastProbeCol	= GetMapValidMinCol();
		m_bBlockProbingToRight	= TRUE;			//	block probing
		m_lBlockProbeLoopRow	= 0;
		m_lBlockProbeLoopCol	= 0;
	}	//	block probe
	ObtainMapValidRange();
	LONG lRow = lAsmRow, lCol = lAsmCol;
	for (lCol = lAsmCol; lCol <= GetMapValidMaxCol(); lCol++)
	{
		ucGrade = m_WaferMapWrapper.GetGrade(lRow, lCol);
		if (ucGrade == ucNullBin)
		{
			continue;
		}
		if (m_WaferMapWrapper.IsReferenceDie(lRow, lCol))
		{
			continue;
		}
		ucGrade = ucGrade - ucOffset;
		if (ucGrade == m_ucDummyPrescanPNPGrade)
		{
			continue;
		}
		if (ucGrade == m_ucPrescanDefectGrade)
		{
			continue;
		}
		if (ucGrade == m_ucPrescanBadCutGrade)
		{
			continue;
		}
		if (ucGrade == m_ucPrescanEmptyGrade)
		{
			continue;
		}
		if (ucGrade == m_ucPrescanEdgeGrade)
		{
			continue;
		}
		m_WaferMapWrapper.ChangeGrade(lRow, lCol, m_ucDummyPrescanPNPGrade + ucOffset);
		m_WaferMapWrapper.SetDieState(lRow, lCol, WT_MAP_DIESTATE_DEFAULT);
	}
	for (lRow = lAsmRow + 1; lRow <= GetMapValidMaxRow(); lRow++)
	{
		for (lCol = 0; lCol <= GetMapValidMaxCol(); lCol++)
		{
			ucGrade = m_WaferMapWrapper.GetGrade(lRow, lCol);
			if (ucGrade == ucNullBin)
			{
				continue;
			}
			if (m_WaferMapWrapper.IsReferenceDie(lRow, lCol))
			{
				continue;
			}
			ucGrade = ucGrade - ucOffset;
			if (ucGrade == m_ucDummyPrescanPNPGrade)
			{
				continue;
			}
			if (ucGrade == m_ucPrescanDefectGrade)
			{
				continue;
			}
			if (ucGrade == m_ucPrescanBadCutGrade)
			{
				continue;
			}
			if (ucGrade == m_ucPrescanEmptyGrade)
			{
				continue;
			}
			if (ucGrade == m_ucPrescanEdgeGrade)
			{
				continue;
			}
			m_WaferMapWrapper.ChangeGrade(lRow, lCol, m_ucDummyPrescanPNPGrade + ucOffset);
			m_WaferMapWrapper.SetDieState(lRow, lCol, WT_MAP_DIESTATE_DEFAULT);
		}
	}

	CStdioFile cfTempRead, cfTempWrite;
	CString szReadFile = CP_PROBE_TEMP_FILE, szSaveFile = CP_PROBE_BACKUP_FILE;		//	Reprobe at map.
	if (cfTempRead.Open(szReadFile,
						CFile::modeNoTruncate | CFile::modeRead | CFile::shareExclusive | CFile::typeText) == FALSE)
	{
		return ;
	}

	DeleteFile(szSaveFile);
	if (cfTempWrite.Open(szSaveFile, 
						 CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText) == FALSE)
	{
		cfTempRead.Close();
		return ;
	}

	lRow = lAsmRow;
	lCol = lAsmCol;
	LONG lStartRow = 0, lStartCol = 0;
	ConvertAsmToHmiUser(lRow, lCol, lStartRow, lStartCol);

	CString szData;
	CStringArray szaList;
	while (cfTempRead.ReadString(szData))
	{
		szaList.RemoveAll();
		ParseRawData(szData, szaList);
		if (szaList.GetSize() > 3)	//	unprobed die, no need to probe again.
		{
			LONG lMapRow = atoi(szaList.GetAt(0));
			LONG lMapCol = atoi(szaList.GetAt(1));
			if (lMapRow > lStartRow)
			{
				continue;
			}
			if (lMapRow == lStartRow && lMapCol >= lStartCol)
			{
				continue;
			}
			if( bRestoreMap==TRUE )
			{
				ULONG ulAsmRow = 0, ulAsmCol = 0;
				if( ConvertHmiUserToAsm(lMapRow, lMapCol, ulAsmRow, ulAsmCol) )
				{
					UCHAR ucGrade = (UCHAR) atoi(szaList.GetAt(2)) + ucOffset;
					m_WaferMapWrapper.ChangeGrade(ulAsmRow, ulAsmCol, ucGrade);
				}
			}	// recover map
		}
		cfTempWrite.WriteString(szData + "\n");
	}

	cfTempRead.Close();
	cfTempWrite.Close();

	DeleteFile(szReadFile);
	CopyFile(szSaveFile, szReadFile, FALSE);
	DeleteFile(szSaveFile);
}	//	reprobe map at selected position, re-assign probed grade to dummy grade and eras raw data.
