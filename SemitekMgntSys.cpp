/////////////////////////////////////////////////////////////////
// NichiaMgntSys.cpp : 
//
//	Description:
//
//	Date:		
//	Revision:	
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2013.
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SemitekMgntSys.h"
#include "MS896a.h"
#include "LogFileUtil.h"
#include "MS896A_Constant.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//================================================================
// Constructor / destructor implementation section
//================================================================

CMSSemitekMgntSubSystem::CMSSemitekMgntSubSystem()
{
	m_bIsEnabled = FALSE;
	CleanUp();
}

CMSSemitekMgntSubSystem::~CMSSemitekMgntSubSystem()
{
	CleanUp();
}

VOID CMSSemitekMgntSubSystem::CleanUp()
{


}

BOOL CMSSemitekMgntSubSystem::LoadData(BOOL bLoadPRMFile)
{
	if (!m_bIsEnabled)
		return FALSE;

	CMSLogFileUtility::Instance()->MS_LogCtmOperation("Semitek SYS: LoadData");


	return TRUE;
}

BOOL CMSSemitekMgntSubSystem::SaveData(VOID)
{
	if (!m_bIsEnabled)
		return FALSE;

	CMSLogFileUtility::Instance()->MS_LogCtmOperation("NICHIA SYS: SaveData");


	return TRUE;
}


//================================================================
// Public GET/SET function implementation section
//================================================================

VOID CMSSemitekMgntSubSystem::SetEnabled(CONST BOOL bEnable)
{
	m_bIsEnabled = bEnable;
}

BOOL CMSSemitekMgntSubSystem::IsEnabled()
{
	return m_bIsEnabled;
}

CString CMSSemitekMgntSubSystem::GetLastError()
{
	return m_szErrMsg;
}

BOOL CMSSemitekMgntSubSystem::GenerateEmptyFrameFile(ULONG ulBlk,ULONG ulBondedDie,int nSerial, CString szBarcode)
{
	//read path from a .txt file start
	CString szPathFile = "c:\\mapsorter\\pathfile.txt";
	CString szTemp;
	CStdioFile cfCheck;
	CString szReadPath;
	if( _access(szPathFile,0) != -1)
	{
		if( cfCheck.Open(szPathFile,CFile::modeRead))
		{
			cfCheck.ReadString(szTemp);
			if (szTemp == "")
			{
				AfxMessageBox("Path 2 Empty");
			}
			else
			{
				szReadPath = szTemp;
				if (_access(szReadPath,0) == -1)
				{
					AfxMessageBox("Could Not Access Parameter List Path:" + szReadPath + ",Save Empty Frame File Fails");
				}
			}
			cfCheck.Close();		
		}
	}
	//read path from a .txt file end
	if (!m_bIsEnabled)
		return FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	m_psmfSRam = &(pApp->m_smfSRam);
	CMSLogFileUtility::Instance()->BL_LogStatus("GenerateEmptyFrameFile(Semitek) Begins!");
	CTime theTime = CTime::GetCurrentTime();
	//CString szBarcode	= szBarcode;//(*m_psmfSRam)["BinLoader"]["ScanBarcodeName"];
	CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
	//CString szOutputFilePath = (*m_psmfSRam)["MS896A"]["OutputFilePath"];
	CString szBlk,szSerial,szYear,szMonth,szDay,szTime;
	CString szPath,szLine;
	CString szWaferID;
	CStdioFile cfWrite;

	szYear.Format("%d",theTime.GetYear());
	szYear = szYear.Right(1);

	szMonth.Format("%d",theTime.GetMonth());
	if (szMonth == "10")
		szMonth = "A";
	if (szMonth == "11")
		szMonth = "B";
	if (szMonth == "12")
		szMonth = "C";

	szDay.Format("%d",theTime.GetDay());
	if (szDay.GetLength()<2)
		szDay = "0" + szDay;

	if (ulBlk < 10)
	{
		szBlk.Format("00%d",ulBlk);
	}
	else if(ulBlk < 100)
	{
		szBlk.Format("0%d",ulBlk);
	}
	else
	{
		szBlk.Format("%d",ulBlk);
	}

	szSerial.Format("%d",nSerial);
	szTime = theTime.Format("%Y/%m/%d %H:%M");
	
	szWaferID = CMS896AStn::m_WaferMapWrapper.GetFileName();
	CMSLogFileUtility::Instance()->BL_LogStatus("GenerateEmptyFrameFile(Semitek) WaferID:" + szWaferID + ",Path," + szReadPath);
	int a = szWaferID.ReverseFind('\\');
	szWaferID = szWaferID.Mid(a + 1);
	szWaferID.Replace(".csv","");
	szWaferID.Replace(".CSV","");
	
	//m_szTapeName = szLotNo + szMachineNo + szYear + szMonth + szDay + szGrade + szSerialNo;
	CString szTapeName;
	CString szLotNo =  (*m_psmfSRam)["MS896A"]["LotNumber"];
	CString szSerial2 = szSerial;
	if ( szSerial2.GetLength() < 2)
		szSerial2 = "0" + szSerial2;
	szTapeName = szLotNo + szMachineNo + szYear + szMonth + szDay + szBlk + szSerial2 + ".csv";

	szPath = szReadPath + "\\" + szWaferID + ".txt";
	szYear.Format("%d",theTime.GetYear());
	szLine = szTime + "," + szBarcode + szYear + szMonth + szDay + szBlk + /*szSerial +*/ "," + szTapeName + "," + szBarcode + "," + szMachineNo + "\n";
	CMSLogFileUtility::Instance()->BL_LogStatus("GenerateEmptyFrameFile(Semitek) Path:" + szPath);
	CMSLogFileUtility::Instance()->BL_LogStatus("GenerateEmptyFrameFile(Semitek) Content:"+ szLine);
	if(_access(szPath,0) != -1)
	{
		if (cfWrite.Open(szPath,CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) != FALSE)
		{
			cfWrite.SeekToEnd();
			cfWrite.WriteString(szLine);
			cfWrite.Close();
		}
	}
	else
	{
		if (cfWrite.Open(szPath,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) != FALSE)
		{
			cfWrite.SeekToEnd();
			cfWrite.WriteString("Time,Original Name,TADE Name,Bin Frame Barcode,Machine ID\n");
			cfWrite.WriteString(szLine);
			cfWrite.Close();
		}
	}

	return TRUE;
}
//================================================================
// End of NichiaMgntSys.cpp
//================================================================



