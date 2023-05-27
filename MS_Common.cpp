/////////////////////////////////////////////////////////////////
// MS_Common.cpp : Common functions of the CMS896AApp class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, April 21, 2005
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
#include "atlimage.h"
#include "MS896A_Constant.h"
#include "MS896A.h"
#include "MainFrm.h"
#include "MS896ADoc.h"
#include "MS896AView.h"
#include "FileUtil.h"
#include "sys\timeb.h"
#include "tlhelp32.h"
//#include "NVRAM.h"
#include "WaferPr.h"
#include "WaferTable.h"
#include "BondPr.h"
#include "MS_SecCommConstant.h"		//v4.59A33
#include "MathFunc.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LONG CMS896AApp::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}

// Temporary folder for PR Records
/////////////////////////////////////////////////////////////////
// Display Msg & Exception
/////////////////////////////////////////////////////////////////
VOID CMS896AApp::DisplayException(CAsmException& e)
{
	CString	szException, szMsg, szCode;
	CString	szMessage = "	EXCEPTION ENCOUNTERED\n";

	e.What(szException);

	szMsg = m_szModuleName + _T(" - EXCEPTION ENCOUNTER!");
	DisplayMessage(szMsg);

	e.What(szException);
	szMsg.Format("   FUNCTION: %s", e.Function());
	DisplayMessage(szMsg);

	szMsg.Format("   PARAMETER: %s", e.Parameter());
	DisplayMessage(szMsg);
	
	szMsg.Format("   ERROR: %s", szException);
	DisplayMessage(szMsg);
}

VOID CMS896AApp::DisplayMessage(const CString& szMessage)
{
	CSingleLock slLock(&m_csDispMsg);

	slLock.Lock();

	if (m_fLogMessage && m_fCanDisplay)
	{
		if (m_pMainWnd != NULL)
		{
			CMS896AView* pView = (CMS896AView*) 
				((CMainFrame*) m_pMainWnd)->GetActiveView();
		
			if (pView != NULL)
			{
				pView->DisplayMessage(szMessage);

				if (m_fLogFileCreated)
				{
					CString szMsg = szMessage + "\r\n";	//v3.94
					m_pInitOperation->WriteLogFile(szMsg);
				}
			}
		}
	}

	slLock.Unlock();
}

/*
////////////////////////////////////////////////////////////////////////////////////
//String Map File Save As
////////////////////////////////////////////////////////////////////////////////////
BOOL CMS896AApp::SaveAsStrMapFile(const CString& szSourcePath, const CString& szSourceFileName,
									   CCriticalSection& csSourceFile, const CString& szDestPath, 
									   const CString& szDestFileName, INT nFormat)
{
	CStringMapFile	smpFile;
	CFileFind		fileFind;
	CString			szSource = szSourcePath + "\\"+ szSourceFileName;
	CString			szDest	 = szDestPath + "\\"+ szDestFileName;

	if (!fileFind.FindFile(szSourcePath))
	{
		HmiMessage("Path not found \"" + szSourcePath + "\"", "SAVE AS", glHMI_MBX_OK);
		return FALSE;
	}

	fileFind.Close();
	if (!fileFind.FindFile(szSource))
	{
		HmiMessage("File not found \"" + szSource + "\"", "SAVE AS", glHMI_MBX_OK);
		return FALSE;
	}

	fileFind.Close();
	if (!fileFind.FindFile(szDestPath))
	{
		HmiMessage("Path not found \"" + szDestPath + "\"", "SAVE AS", glHMI_MBX_OK);
		return FALSE;
	}

	CSingleLock sllock(&csSourceFile);
	sllock.Lock();

	if (!smpFile.Open(szSource, FALSE, FALSE))
	{
		sllock.Unlock();
		return FALSE;
	}

	switch(nFormat)
	{
	case CStringMapFile::frmTEXT:
	case CStringMapFile::frmCSV:
		smpFile.SaveAs(szDest, nFormat);
		break;
	default:
		smpFile.SaveAs(szDest, CStringMapFile::frmBINARY);
		break;
	}

	sllock.Unlock();

	return TRUE;
}
*/

VOID CMS896AApp::SetStatusMessage(const CString& szStatus)
{
	if (szStatus.IsEmpty())
	{
		return;
	}
	CString szText, szExtraStatus;
	CTime CurTime = CTime::GetCurrentTime();

	szText = CurTime.Format("[%H:%M:%S %d/%m/%y] ");
	szExtraStatus = GetExtraStatus();
	m_szStatusMessage = szText + szExtraStatus + szStatus;

	//v4.48A2	//WH SanAn
	CString szLogFile		= gszROOT_DIRECTORY + "\\Exe\\StatusRecord";
	CString szSanAnFile		= gszUSER_DIRECTORY + "\\Error\\" + GetMachineNo() + "_StatusRecord";	//v4.48A4

	FILE *fpRecord = NULL;
	errno_t nErr = fopen_s(&fpRecord, szLogFile, "a+");
	if ((nErr == 0) && (fpRecord != NULL))
	{
		fprintf(fpRecord, m_szStatusMessage + "\n");
		fclose(fpRecord);

		if (GetCustomerName() == "SanAn")
		{
			CopyFile(szLogFile, szSanAnFile, FALSE);	//v4.48A2
		}
	}	
	else
	{
		DisplayMessage("Cannot record status");
	}
}

VOID CMS896AApp::SetLogItems(CString szStatus)
{
	m_oToolsUsageRecord.SetLogItems(szStatus);
}

CString CMS896AApp::GetExtraStatus()
{
	if ( GetCustomerName() == "Yealy" && m_bES100v2DualWftOption == TRUE )//IsES101() )
	{
		LONG lLampColour = GetAlarmLamp_Status();
		CString szLampStatus;
		switch(lLampColour)
		{
			case ALARM_OFF_ALL:
			case ALARM_OFF_LIGHT: // off all light
			default:
				szLampStatus = "[ ]";
				break;

			case ALARM_GREEN:		// Green
				szLampStatus = "[G]";
				break;

			case ALARM_YELLOW:		// Yellow
			case ALARM_YELLOW_BUZZER: // yellow with buzzer
				szLampStatus = "[O]";
				break;

			case ALARM_RED:		// Red
			case ALARM_RED_NO_BUZZER:		// red but no buzzer
			case ALARM_REDYELLOW_BUZZER:	// red yellow with buzzer
			case ALARM_REDYELLOW_NO_BUZZER:	// red yellow no buzzer
			case ALARM_GREEN_YELLOW_RED_ON:	//	green yellow red no buzzer
				szLampStatus = "[R]";
				break;
		}
		szLampStatus = szLampStatus + " ";

		CString szArrTemp[4];
		szArrTemp[3] = (m_smfSRam)["WaferTable"]["YealyFilePath_1Layer"];		// Top == First (series id)
		szArrTemp[2] = (m_smfSRam)["WaferTable"]["YealyFilePath_2Layer"];		// (date)
		szArrTemp[1] = (m_smfSRam)["WaferTable"]["YealyFilePath_3Layer"];		// (lotnumber name)
		szArrTemp[0] = (m_smfSRam)["WaferTable"]["YealyFilePath_LowestLayer"];	// waferid

		for ( int i = 0 ; i < 4 ; i++ )
		{
			if ( szArrTemp[i].GetLength() == 0 )
			{
				szArrTemp[i] = " - ";
			}
		}

		CString szLotStatus;
		szLotStatus.Format("[%s,%s,%s,%s] ", szArrTemp[3], szArrTemp[2], szArrTemp[1], szArrTemp[0]);

		return szLampStatus + szLotStatus;
	}
	else
	{
		return "";
	}
}

VOID CMS896AApp::SetErrorMessage(const CString& szStatus)
{
	if (szStatus.GetLength() == 0)
		return;

	CString szText;
	CString szPath;	
	CString szYear, szMonth, szDay10;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);
	szMonth.Format("%02d", CurTime.GetMonth());
	szDay10.Format("%02d", (CurTime.GetDay()/10+1));

	szText = CurTime.Format("[%d %H:%M:%S] ");
	szPath = gszUSER_DIRECTORY + "\\Error\\ErrorLog_" + szYear + szMonth + szDay10 + ".txt";

	if (GetCustomerName() == "SanAn")		//v4.48A9
	{
		CString szMachineNo = GetMachineNo();
		szPath = gszUSER_DIRECTORY + "\\Error\\" + szMachineNo + "_ErrorLog_" + szYear + szMonth + szDay10 + ".txt";
	}

	FILE *fpRecord = NULL;
	errno_t nErr = fopen_s(&fpRecord, szPath, "a+");
	if ((nErr == 0) && (fpRecord != NULL))
	{
		fprintf(fpRecord,  szText + szStatus + "\n");
		fclose(fpRecord);
	}	
}

BOOL CMS896AApp::CheckAndBackupErrLogFile(ULONGLONG nFileSizeLimit)	//v4.46T15
{
	INT nCol =0;
	ULONGLONG nFileSize =0;
	CString szErrLogFileName;
	CString szBpErrLogFileName;
	CFile fLogFile;

	CString szYear, szMonth, szDay10, szDay, szHour;
		
	CTime CurTime = CTime::GetCurrentTime();

	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);
	szMonth.Format("%02d",	CurTime.GetMonth());
	szDay10.Format("%02d",	(CurTime.GetDay()/10+1));
	szDay.Format("%02d",	CurTime.GetDay());
	szHour.Format("%02d",	CurTime.GetHour());

	szErrLogFileName = gszUSER_DIRECTORY + "\\Error\\ErrorLog_" + szYear + szMonth + szDay10 + ".txt";

	if (GetCustomerName() == "SanAn")		//v4.48A9
	{
		CString szMachineNo = GetMachineNo();
		szErrLogFileName = gszUSER_DIRECTORY + "\\Error\\" + szMachineNo + "_ErrorLog_" + szYear + szMonth + szDay10 + ".txt";
	}

	if (_access(szErrLogFileName, NULL) == -1)	//Not exist!
	{
		return FALSE;
	}

	if  (fLogFile.Open(szErrLogFileName, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	nFileSize = fLogFile.GetLength();
	fLogFile.Close();

	// Case of no need to backup the log file
	if (nFileSize < nFileSizeLimit*2 )
	{
		return TRUE;
	}

	// Start backup log file
	nCol = szErrLogFileName.ReverseFind('.');

	if (nCol != -1)
	{
		szBpErrLogFileName = szErrLogFileName.Left(nCol) + "_" + szDay + szHour + ".bak";
	}
	else
	{
		szBpErrLogFileName = szErrLogFileName + "_" + szDay + szHour + ".bak";
	}

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

VOID CMS896AApp::SetCtmLogMessage(const CString& szStatus)
{

	CString szText;
	CString szPath;
	CTime CurTime = CTime::GetCurrentTime();

	szText = CurTime.Format("[%H:%M:%S %d/%m/%y] ");

	CString szYear, szMonth;
	szYear.Format("%d", CurTime.GetYear()); 
	szYear = szYear.Right(2);
	szMonth.Format("%d", CurTime.GetMonth());
	if ( CurTime.GetMonth() < 10 )
	{
		szMonth = "0" + szMonth;
	}
	szPath = gszUSER_DIRECTORY + "\\CtmLog\\MachineStatus_" + szMonth + szYear + ".txt";

	m_szStatusMessage = szText + szStatus;

	FILE *fpRecord = NULL;
	errno_t nErr = fopen_s(&fpRecord, szPath, "a+");
	if ((nErr == 0) && (fpRecord != NULL))
	{
		fprintf(fpRecord, m_szStatusMessage + "\n");
		fclose(fpRecord);
	}
	else
	{
		DisplayMessage("Cannot save custom logfile.");
	}
}

VOID CMS896AApp::HmiControlAccessMode()
{
	if ( m_bOperator == TRUE )
	{
		ReadScreenControlAccessMode();
	}
	else
	{
		ResetScreenControlAccessMode();
	}

	if( GetCustomerName() == "SanAn" && GetProductLine() == "XA" )
	{
		if( m_bAdministrator )
		{
			CMS896AStn::m_WaferMapWrapper.EnableInitButton(TRUE);
		}
		else
		{	
			CMS896AStn::m_WaferMapWrapper.EnableInitButton(FALSE);
		}
	}

	LoadHmiSecuritySettings();
	ChangeHmiSecurityRoleName();
	LONG lLogInLevel = 0;
	switch( CheckLoginLevel() )
	{
	case 0:
		lLogInLevel = 5;
		break;
	case 1:
		lLogInLevel = 4;
		break;
	case 2:
		lLogInLevel = 3;
		break;
	case 3:
		lLogInLevel = 0;
		break;
	}		// 0 OP; 1 Tech; 2 Engr 3: Admin
	ChangeHmiSecurityLevel(lLogInLevel);

/*
HMI				Level	LogIn
Administrator 	0		Administrator
Technical		1	
Adv. Prod		2
Production		3		Engineer
Setup			4		Technician
Operator		5		Operator
*/

}

VOID CMS896AApp::MachineStarted(void)
{
	if( m_bMachineStarted==FALSE )
	{
		ReloadMachineProfile();
	}
	if ( m_bMachineStarted == TRUE )
	{
		return;
	}

	CString szText;
	CTime CurTime = CTime::GetCurrentTime();
	
	CStdioFile cfStatusFile;
	CStdioFile cfBackupStatusFile;
	BOOL bStatusFileExist = FALSE;
	BOOL bBackuupStatusFileExist = FALSE;
	CString szContent;


	// Delete DummyWaferMap File
	DeleteFile("DummyWaferMap");
	DeleteFile("WaferMapTemp\\DummyWaferMap.asm.tmp");

	// Make a copy before delete Machine Status file (as shown in HMI)
	//CopyFile("StatusRecord", "StatusRecord.bak", FALSE);
	//Append StatusRecord file content into BAK file
	bStatusFileExist = cfStatusFile.Open("StatusRecord", 
								CFile::modeRead|CFile::shareExclusive|CFile::typeText);

	bBackuupStatusFileExist = cfBackupStatusFile.Open("StatusRecord.bak", 
									CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone|CFile::typeText);

	if ( (bStatusFileExist == TRUE) && (bBackuupStatusFileExist == TRUE) )
	{
		cfStatusFile.SeekToBegin();
		cfBackupStatusFile.SeekToEnd();
		
		//Start in next line
		cfBackupStatusFile.WriteString("\n");

		while(cfStatusFile.ReadString(szContent))
		{
			szContent = szContent + "\n";
			cfBackupStatusFile.WriteString(szContent);
		}
	}


	if ( bStatusFileExist == TRUE )
	{
		cfStatusFile.Close();
	}

	if ( bBackuupStatusFileExist == TRUE )
	{
		cfBackupStatusFile.Close();
	}

	DeleteFile("StatusRecord");

	InitMachineTime();
	InitMachineTime2();			//WH Sanan	//v4.40T4
	InitToolsUsageRecord();

	m_bMachineStarted = TRUE;
}


VOID CMS896AApp::SetMeasurementUnit(const CString& szMeasurementUnit)
{
	m_szMeasurementUnit = szMeasurementUnit;
} //end SetMeasurementUnit


CString CMS896AApp::GetMeasurementUnit()
{
	return m_szMeasurementUnit;
} //end GetMeasurementUnit


LONG CMS896AApp::ConvertFileUnitToDisplayUnit(DOUBLE dValue)
{
	LONG lConvertedValue = 0;	//Klocwork

	//convert value in file (in um) into the selected display unit (mil or um)
	if (m_szMeasurementUnit == "um") //um -> um
		lConvertedValue = _round(dValue);
	else if (m_szMeasurementUnit == "mil") //um ->mil		//Klocwork
	{
		lConvertedValue = _round(dValue / 25.4);
	}

	return lConvertedValue;
} //end ConvertFileUnitToDisplayUnit


LONG CMS896AApp::ConvertDisplayUnitToFileUnit(DOUBLE dValue)
{
	LONG lConvertedValue = 0;	//Klocwork

	//convert value on screen (in mil or um) into value for file saving (in um)
	if (m_szMeasurementUnit == "um") //um -> um
		lConvertedValue = _round(dValue);
	else if (m_szMeasurementUnit == "mil") //mil -> um
	{
		lConvertedValue = _round(dValue * 25.4);
	}

	return lConvertedValue;
} //end ConvertDisplayUnitToFileUnit

////////////////////////////////////////////////////////////////////////////////////
//Get Machine config file information
////////////////////////////////////////////////////////////////////////////////////
CString CMS896AApp::GetMachineInformation(const CString& szTitle, const CString& szGroup)
{
	CString	szValue = NULL; 
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	//load & get config file ptr
	pUtl->LoadMachineConfig(); 
	psmf = pUtl->GetMachineConfigFile();

	if ( psmf == NULL )
		return "";
/*
	//Get target value
	szValue	= (*psmf)[szTitle];
*/	
	//Get target value
	if ( szGroup.IsEmpty() == TRUE )
	{
		szValue	= (*psmf)[szTitle];
	}
	else
	{
		szValue	= (*psmf)[szGroup][szTitle];
	}

	//close config file ptr
	pUtl->CloseMachineConfig();

	return szValue;
}


LONG CMS896AApp::GetMachineHardwareConfig(const CString& szTitle)
{
	LONG lValue = 0; 
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	//load & get config file ptr
	pUtl->LoadMachineConfig(); 
	psmf = pUtl->GetMachineConfigFile();

	if ( psmf == NULL )
		return 0;

	//Get target value
	lValue	= (*psmf)[MS896A_CFG_HARDWARD_CONFIG][szTitle];

	//close config file ptr
	pUtl->CloseMachineConfig();

	return lValue;
}


VOID CMS896AApp::SavePackageInfo(LONG lSaveMode)
{
	if (lSaveMode == MS_PKG_SAVE_PATH)
	{
		WriteProfileInt(gszPROFILE_SETTING, gszDEVICE_FILE_TYPE, (INT)m_ucDeviceFileType);
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, m_szDevicePath);
		WriteProfileString(gszPROFILE_SETTING, gszPORTABLE_DEVICE_PATH_SETTING, m_szPortablePKGPath);
	}
	else if (lSaveMode == MS_PKG_SAVE_DEVICE)
	{
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, GetPKGFilename());
		//v4.04
		(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
		(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();

		//v4.59A33
		//SetGemValue(MS_SECS_SV_CURR_RECIPE_NAME, szPKGFilename);
		if (IsSecsGemInit())
		{
			(*m_pAppGemStation)[MS_SECS_SV_CURR_RECIPE_NAME] = GetPKGFilename();
			m_pAppGemStation->SendEvent(SG_CEID_PROCESS_PROGRAM_SELECTED);
		}
	}
	else
	{
		WriteProfileInt(gszPROFILE_SETTING, gszDEVICE_FILE_TYPE, (INT)m_ucDeviceFileType);
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_PATH_SETTING, m_szDevicePath);
		WriteProfileString(gszPROFILE_SETTING, gszPORTABLE_DEVICE_PATH_SETTING, m_szPortablePKGPath);
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_FILE_SETTING, GetPKGFilename());
		//v4.04
		(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
		(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();

		//v4.59A33
		//SetGemValue(MS_SECS_SV_CURR_RECIPE_NAME, szPKGFilename);
		if (IsSecsGemInit())
		{
			(*m_pAppGemStation)[MS_SECS_SV_CURR_RECIPE_NAME] = GetPKGFilename();
			m_pAppGemStation->SendEvent(SG_CEID_PROCESS_PROGRAM_SELECTED);
		}
	}
}


VOID CMS896AApp::ClearNVRAM()
{
	LONG *plTemp, i;


	//Init wafermap
	CMS896AStn::m_WaferMapWrapper.InitMap();

	if ( m_pvNVRAM != NULL )
	{
		plTemp = (LONG*)m_pvNVRAM;
		// Assume 32KB NVRAM (here is 8K as it is LONG)
		for ( i=0; i < NVRAM_8K; i++ )
		{
			*(plTemp+i) = 0;
		}
	}

	if ( m_pvNVRAM_HW != NULL )
	{
		plTemp = (LONG*)m_pvNVRAM_HW;
		// Assume 32KB NVRAM (here is 8K as it is LONG)
		for ( i=0; i < NVRAM_8K; i++ )
		{
			*(plTemp+i) = 0;
		}
	}
	else	//v3.66
	{
		TRY {
			DeleteFile(_T("c:\\MapSorter\\Exe\\NVRAM.bin"));
		} CATCH (CFileException, e)
		{
		}
		END_CATCH
	}
	

	// Re-create wafermap memory
	void *pvTemp;
	unsigned long ulSize;
		
	ulSize = CMS896AStn::m_WaferMapWrapper.GetPersistentSize();
	pvTemp = (void*)((unsigned long)m_pvNVRAM + glNVRAM_WaferMap_Start);

	if ( (m_pvNVRAM == NULL) || (ulSize > glNVRAM_WaferMap_Size) )
	{
		// Create a 60000 byte file memory
		WAF_CMapMemoryManagement::Instance()->CreateMemory("FileStorage", "Wafermap", NULL, 60000);
	}
	else
	{
		pvTemp = (void*)((unsigned long)m_pvNVRAM + glNVRAM_WaferMap_Start);
		WAF_CMapMemoryManagement::Instance()->CreateMemory("SRAMStorage", "Wafermap", pvTemp, ulSize);
	}
}


BOOL CMS896AApp::InitMS899NVRAM()
{
	//BOOL bInitNuMotionNVRam = TRUE;
	m_pvNVRAM = malloc(NVRAM_8K * sizeof(LONG));	// Assume 32KB NVRAM (here is 8K as it is LONG)
	
	//v4.59A41
	/*if ( NVRAM_Init() == 0 )		//Init MFB4 onboard SRAM
	{
		DisplayMessage("NVRAM: Init MFB4 SRAM Done");
		m_nNVRAMOption = 1;			//MFB4 option
		m_pvNVRAM_HW = NVRAM_GetAddr();
		CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM: Init MFB4 SRAM Done");
	}
	else*/ 
	if (InitNuMotionNVRAM())	//Init NuMotion built-in SRAM
	{
		//bInitNuMotionNVRam = TRUE;
		DisplayMessage("NVRAM: Init NuMotion SRAM Done");
		m_nNVRAMOption = 2;			//NuMotion option
		CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM: Init NuMotion SRAM Done");
	}
	else
	{
		m_pvNVRAM_HW	= NULL;
		m_nNVRAMOption	= 0;		//DRAM from TXT file;

		//v3.87T1	//Make DRAM option available for MS899/810 also
		DisplayMessage("NVRAM: Init DRAM Done");
		CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM: Init DRAM Done");
	}

	//if (bInitNuMotionNVRam == FALSE)
	//{
	//	CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM: Init NuMotion SRam Fail");
	//	return FALSE;
	//}

	LoadFromNVRAM();	
	CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM: load from nvram");
	return TRUE;
}


BOOL CMS896AApp::InitNuMotionNVRAM()
{
#ifdef NU_MOTION	
	GMP_S16 s16RetVal;
	GMP_UINT_PTR u64NVRamBaseAddr = 0;
	GMP_U32 u32NVRamSize = 0;
	//GMP_UINT_PTR* p;
	GMP_UINT_PTR p = 0;
	CString szErr;

	// Get the base address and the size of Non-Volatile Random Access Memory
	s16RetVal = gmp_get_nvram_base_addr(&u64NVRamBaseAddr, &u32NVRamSize);
	//s16RetVal = gmp_get_nvram_base_addr(&p, &u32NVRamSize);
	if (s16RetVal != GMP_SUCCESS)
	{
		CString szErr;
		szErr.Format("NVRAM: Init NuMotion SRAM fails ERR=%d", (SHORT) s16RetVal);
		DisplayMessage(szErr);
		return FALSE;
	}

	m_pvNVRAM_HW = (VOID*) u64NVRamBaseAddr;

	ULONG ulRamSize = (ULONG) u32NVRamSize;
	CString szLog;
	szLog.Format("NVRAM: NuMotion OnBoard RAM size = %d", ulRamSize);
	DisplayMessage(szLog);

	if (ulRamSize < NVRAM_8K)
	{
		szErr.Format("NVRAM: NuMotion SRAM size %lu bytes (too small %d)", ulRamSize, NVRAM_8K);
		DisplayMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);		//v4.65A3
		return FALSE;
	}
	else
	{
		szErr.Format("NVRAM: NuMotion SRAM size = %lu bytes (%d)", ulRamSize, NVRAM_8K);
		DisplayMessage(szErr);
		CMSLogFileUtility::Instance()->MS_LogOperation(szErr);		//v4.65A3
	}
	return TRUE;
#else
	return FALSE;
#endif
}


// Load from and backup to NVRAM (Hardware)
VOID CMS896AApp::LoadFromNVRAM()
{
	LONG *plSW, *plHW, i;

	if ( (m_nNVRAMOption == 1) &&			//MFB4
		 (m_pvNVRAM != NULL) && (m_pvNVRAM_HW != NULL) )
	{
		plSW = (LONG*)m_pvNVRAM;
		plHW = (LONG*)m_pvNVRAM_HW;
		// Assume 32KB NVRAM (here is 8K as it is LONG)
		for ( i=0; i < NVRAM_8K; i++ )
		{
			*(plSW+i) = *(plHW+i);
		}
	}
	else if ( (m_nNVRAMOption == 2) &&		//NuMotion Built-in
			  (m_pvNVRAM != NULL) && (m_pvNVRAM_HW != NULL) )
	{
		plSW = (LONG*)m_pvNVRAM;
		plHW = (LONG*)m_pvNVRAM_HW;
		// Assume 32KB NVRAM (here is 8K as it is LONG)
		for ( i=0; i < NVRAM_8K; i++ )
		{
			*(plSW+i) = *(plHW+i);
		}
	}
	else if (m_nNVRAMOption == 0)			//DRAM
	{
		BOOL bStatus = m_VNRAM_SW.Open(_T("c:\\MapSorter\\Exe\\NVRAM.bin"), CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
		if (bStatus)
		{
			plSW = (LONG*)m_pvNVRAM;
			CString szLine;

			for ( i=0; i < NVRAM_8K; i++ )
			{
				//*(plSW+i) = *(plHW+i);
				if (!m_VNRAM_SW.ReadString(szLine))
					break;
				*(plSW+i) = atol(szLine);
			}

			m_VNRAM_SW.Close();
		}
	}
}


LONGLONG CMS896AApp_GetTime(VOID)
{
	LARGE_INTEGER		liTime, liFreq;

	QueryPerformanceCounter(&liTime);
	QueryPerformanceFrequency(&liFreq);

	return(liTime.QuadPart * 1000000 / liFreq.QuadPart);
}


VOID CMS896AApp::BackupToNVRAM()
{
	LONG *plSW, *plHW, i;
	if ( (m_nNVRAMOption == 1) &&								//v3.74
		 (m_pvNVRAM != NULL) && (m_pvNVRAM_HW != NULL) )
	{
		plSW = (LONG*)m_pvNVRAM;
		plHW = (LONG*)m_pvNVRAM_HW;
		// Assume 32KB NVRAM (here is 8K as it is LONG)
		for ( i=0; i < NVRAM_8K; i++ )
		{
			*(plHW+i) = *(plSW+i);

			LONGLONG	lwCur = CMS896AApp_GetTime();
			
			while(1)
			{
				LONGLONG lwNow = CMS896AApp_GetTime();
				
				//if Now is Negative AND Cur is Positive, then break the loop
				if ( (lwNow < 0) && (lwCur > 0) )
				{
					break;
				}

				//if ( (lwNow - lwCur) > 5 )
				if ( (lwNow - lwCur) >= 2 )
				{
					break;
				}
			}

			//while (CMS896AApp_GetTime() - lwCur < 5 );
		}
	}
	else if ( (m_nNVRAMOption == 2) &&								//v3.74
			  (m_pvNVRAM != NULL) && (m_pvNVRAM_HW != NULL))
	{
		plSW = (LONG*)m_pvNVRAM;
		plHW = (LONG*)m_pvNVRAM_HW;
		// Assume 32KB NVRAM (here is 8K as it is LONG)
		for ( i=0; i < NVRAM_8K; i++ )
		{
			*(plHW+i) = *(plSW+i);

			LONGLONG lwCur = CMS896AApp_GetTime();
			
			while(1)
			{
				LONGLONG lwNow = CMS896AApp_GetTime();
				
				//if Now is Negative AND Cur is Positive, then break the loop
				if ( (lwNow < 0) && (lwCur > 0) )
				{
					break;
				}

				//if ( (lwNow - lwCur) > 5 )
				if ( (lwNow - lwCur) >= 2 )
				{
					break;
				}
			}
		}
	}

	ExportNVRAMDataToTxtFile(_T("c:\\MapSorter\\Exe\\NVRAM.bin"));	//v4.40T15
}


BOOL CMS896AApp::DeleteAllFiles()
{
	DeleteFile("BondHead.msd");
	DeleteFile("WaferTable.msd");
	DeleteFile("BinTable.msd");
	DeleteFile("WaferPr.msd");
	DeleteFile("BondPr.msd");
	DeleteFile("WaferLoader.msd");
	DeleteFile("BinLoader.msd");

	DeleteFile("BondHeadbkf.msd");
	DeleteFile("WaferTablebkf.msd");
	DeleteFile("BinTablebkf.msd");
	DeleteFile("WaferPrbkf.msd");
	DeleteFile("BondPrbkf.msd");
	DeleteFile("WaferLoaderbkf.msd");
	DeleteFile("BinLoaderbkf.msd");

	DeleteFile("LastState.msd");
	DeleteFile("LastStatebkf.msd");

	DeleteFile(MSD_APP_DATA_FILE);
	DeleteFile("AppDatabkf.msd");

	DeleteFile("Wafermap");
	DeleteFile("BinTable");
	DeleteFile("WaferMapTemp\\*.*");
	return TRUE;
}

BOOL CMS896AApp::CheckPKGFileExtension(BOOL bIsPortablePKGFile, CString szFileExt)
{
	if (bIsPortablePKGFile == FALSE)
	{
		szFileExt = szFileExt.MakeUpper();
		
		if (szFileExt == "PPKG")
		{
			return FALSE;
		}
	}
	else
	{
		szFileExt = szFileExt.MakeUpper();

		if (szFileExt != "PPKG")
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

// Internal Function for SaveDeviceFile()
BOOL CMS896AApp::SaveToDevice(CString szDevicePath, CString szDeviceFile)
{			
	int nConvID = 0;
	IPC_CServiceMessage svMsg;
	CString szTitle;
	CString szMsg, szPrRecordPath, szFullPath;
	ULONG ulNumOfPrRecords=0, ulNumOfBPrRecords = 0;
	BOOL bReturn, bBprRecordExist=FALSE;

	if (szDeviceFile.Find(".") == -1) // for no .ppkg or .pkg cases
	{
		if(m_ucDeviceFileType == 1)
		{	
			szDeviceFile = szDeviceFile + _T(".ppkg");
		} 
		else if(m_ucDeviceFileType == 0)
		{
			szDeviceFile = szDeviceFile + _T(".pkg");
		}
	}
	
	szTitle.LoadString(HMB_MS_SAVE_PKG_FILE);

	StartLoadingAlert();		// Show the waiting message
	// Check whether the path is existing
	if ( CreateDirectory(szDevicePath, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			CloseLoadingAlert();		// Close the waiting message

			szMsg.LoadString(HMB_MS_CREATE_DRIECTORY_FAILED);

			HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			return FALSE;
		}
	}

	szFullPath = szDevicePath + "\\" + szDeviceFile;

	//SeoulSemi: Backup all of DATABASE file
//	if ( GetCustomerName() == "SeoulSemi" ) 
	if ( m_bEnableSaveTempFileWithPKG )
	{
		// COPY DATABASE
		CStringList szSelection;
		CONST CString szBackupPath = m_szDevicePathDisplay + "\\" + GetPKGFilename() + "_TEMPDATA\\"; 
		CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";

		CreateDirectory(szBackupPath, NULL);

		WIN32_FIND_DATA FileData;
		HANDLE hSearch;
		hSearch = FindFirstFile( szExePath + "Blk*tempfile.csv", &FileData);
		if ( hSearch == INVALID_HANDLE_VALUE ) 
		{ 
			HmiMessage("No Temp Database file backup.");
		}
		else 
		{
			CopyFile( szExePath + FileData.cFileName, szBackupPath + FileData.cFileName, FALSE);
			Sleep(1);
			int counter = 1;
			while (FindNextFile(hSearch, &FileData) == TRUE)
			{
				CopyFile( szExePath + FileData.cFileName, szBackupPath + FileData.cFileName, FALSE);
				Sleep(1);
				counter++;
			}
			CString szRecoverResult;
			szRecoverResult.Format("%d Temp Database file has backuped.", counter);
			HmiMessage(szRecoverResult);
		}

		// EXPORT NVRAM.bin
		BOOL bStatus = ExportNVRAMDataToTxtFile( szBackupPath + _T("NVRAM.bin"));
		if (bStatus)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM data is exported to NVRAM.bin");		//v3.55
			CString szTemp;
			szTemp.Format( "NVRAM file is exported to: %s", szBackupPath + _T("NVRAM.bin") );
			HmiMessage(szTemp);
		}
		else 
		{
			HmiMessage("NVRAM file export error");
		}
	}

	// Check whether the file is existing
	FILE *pFile = NULL;
	errno_t nErr = fopen_s(&pFile, szFullPath, "r");
	if ((nErr == 0) && (pFile != NULL))
	{
		CloseLoadingAlert();		// Close the waiting message

		szMsg.LoadString(HMB_MS_SAVE_PKG_FILE_WARNING);

		BOOL bStop = FALSE;
		CString szFileName;
		szFileName = szDeviceFile;
		szFileName = szFileName.MakeLower();

		// check the extension V4.10 and cannot overwrite the file if it is ppkg
		if( szFileName.Find(".ppkg")!=-1 )
		{
			if ( (GetCustomerName() == CTM_NICHIA) && (GetProductLine() == _T("")) )	//v4.42T12
			{
				bStop = (HmiMessage(szMsg, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_NO);
			}
			else
			{
				szMsg.Format(" %s\n Already exist and can NOT replace!", szFullPath);
				HmiMessage(szMsg, szTitle, glHMI_MBX_CLOSE);
				bStop = TRUE;
			}
		}
		else
		{
			bStop = (HmiMessage(szMsg, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_NO);
		}

		fclose(pFile);

		if( bStop )
		{
			return FALSE;
		}

		StartLoadingAlert();		// Show the waiting message
	}

	// Set the record path
	(m_smfSRam)["PR"]["RecordPath"] = gszPR_RECORD_PATH;
	szPrRecordPath = gszPR_RECORD_PATH;
	if ( CreateDirectory(gszPR_RECORD_PATH, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			CloseLoadingAlert();		// Close the waiting message

			szMsg.LoadString(HMB_MS_CREATE_DRIECTORY_FAILED);

			HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			return FALSE;
		}
	}

	// Upload the WaferPR Records			
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UploadPrRecords", svMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	svMsg.GetMsg(sizeof(BOOL), &bReturn);
	if ( bReturn == TRUE )
	{
		ulNumOfPrRecords = (m_smfSRam)["WaferPr"]["NumOfRecords"];

		// Upload the BondPR Records		
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "UploadPrRecords", svMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 100000) == TRUE )
			{	
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				svMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if ( bReturn == TRUE )
		{
			bBprRecordExist = (BOOL)(LONG)(m_smfSRam)["BPR"]["RecordExist"];
			ulNumOfPrRecords = WPR_MAX_DIE;
			ulNumOfBPrRecords = BPR_MAX_DIE;

			if ( m_DeviceFile.Save(szFullPath, szPrRecordPath, ulNumOfPrRecords, bBprRecordExist, ulNumOfBPrRecords) )
			{
				// store preview image
				StorePKGPreviewImage(szDevicePath , szDeviceFile);
				// update the preview image
				ChangePkgFilePreviewDiagram(szDevicePath , szDeviceFile);
				// Delete the WaferPR Records	

				DeletePrRecordFiles(TRUE);
				DeletePrRecordFiles(FALSE);

				szMsg.LoadString(HMB_MS_SAVE_PKG_FILE_OK);
				SavePackageInfo();		// Save package info to String Map file
			}
			else
			{
				szMsg.LoadString(HMB_MS_CREATE_FILE_FAILED);
			}
		}
		else
			szMsg.LoadString(HMB_MS_UPLOAD_BPR_FAILED);
	}
	else
		szMsg.LoadString(HMB_MS_UPLOAD_WPR_FAILED);

	SteerPkgKeyParameters(0);	//	save pkg file

	//v4.31T9	//SECSGEM to upload PKG to host
	CString szPackage = szDeviceFile;
	//char* pFilename;
	INT nMsgLength = (szPackage.GetLength() + 1) * sizeof(CHAR);
	char* pFilename = new char[nMsgLength];
	strcpy_s(pFilename, nMsgLength, szPackage);
	svMsg.InitMessage(nMsgLength, pFilename);

	nConvID = m_comClient.SendRequest(SECS_COMM_STN, "UploadPackage", svMsg);	//Upload package to Host
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}


	CloseLoadingAlert();		// Close the waiting message

	szTitle.LoadString(HMB_MS_SAVE_PKG_FILE);

	HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	SetStatusMessage("Package file is saved");

	m_eqMachine.SetPackage (szDeviceFile);
	m_eqMachine2.SetPackage(szDeviceFile);		//WH Sanan	//v4.40T4

	delete[] pFilename;
	return bReturn;
}

//Nichia//v4.43T7
BOOL CMS896AApp::RestoreNichiaPRM(CString szDeviceFile, BOOL bNeedWarmStart)
{
	int nConvID = 0;
	IPC_CServiceMessage svMsg;
	CString szTitle;
	CString szMsg, szFullPath, szLog;
	BOOL bReturn = FALSE, bAlwaysFalse = FALSE;
	BOOL bShowAlert = FALSE;
	BOOL bResetSerialNo = FALSE;
	//FILE *pFile;
		
	szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);

	//m_szaPortablePackageFileFileList
	if (szDeviceFile.Find("ppkg") == -1)
		szDeviceFile = szDeviceFile + ".ppkg";
	
	m_DeviceFile.SetLoadPkgOption(m_bLoadPkgWithBin);
	m_DeviceFile.SetOnlyLoadBinParameters(m_bOnlyLoadBinParam);
	m_DeviceFile.SetOnlyLoadWFTParameters(m_bOnlyLoadWFTParam);
	m_DeviceFile.SetExcludeMachineParameters(TRUE);	//m_bExcludeMachineParam);
	m_DeviceFile.SetPortablePKGFile(TRUE);			//m_bPortablePKGFile);
	m_DeviceFile.SetOnlyLoadPRParameters(FALSE);	//m_bOnlyLoadPRParam);
	m_DeviceFile.SetPortablePackageFileFileList(m_szaPortablePackageFileFileList);
	m_DeviceFile.SetIsPackageFileIgnoreList(m_bIsPortablePackageFileIgnoreList);
	
	//OpenWaitAlert();

	szFullPath = m_szPortablePKGPath + "\\" + szDeviceFile;


	if ( m_DeviceFile.Restore(szFullPath) )
	{
		szMsg.LoadString(HMB_MS_LOAD_PKG_OK);	
		if (bNeedWarmStart)
			szLog = "Machine PRM file is restored (manual)  - " + szDeviceFile;
		else
		{
			szLog = "Machine PRM file is restored (auto)  - " + szDeviceFile;
			m_szDeviceFile = szDeviceFile;
		}
		SetStatusMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		SavePackageInfo();

		if (bNeedWarmStart)
		{
			HmiMessage(szMsg, szTitle);
			OpenWaitAlert();
			MachineWarmStart(TRUE);
			CloseWaitAlert();
		
			szMsg.LoadString(HMB_MS_WARN_START_OK);
			HmiMessage(szMsg, szTitle);
		}
		else
		{
			CMS896AStn::m_oNichiaSubSystem.LoadData(TRUE);	
			//HmiMessage(szMsg, szTitle);
		}

	}
	else
	{
		szMsg.LoadString(HMB_MS_LOAD_PKG_FAILED);

		szLog = "Machine PRM file is restored fails - " + szDeviceFile;
		SetErrorMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogCtmOperation(szLog);
		return FALSE;
	}

	m_eqMachine.SetPackage (szDeviceFile);
	m_eqMachine2.SetPackage(szDeviceFile);
	return TRUE;
}

// Internal Function for Restore DeviceFile()
BOOL CMS896AApp::RestoreFromDevice(CString szDevicePath, CString szDeviceFile, 
								   BOOL bPopUpConfirmDialog, BOOL bPopupCompleteDialog,
								   BOOL bEnableSECSGEM)
{
	int nConvID = 0;
	IPC_CServiceMessage svMsg;
	CString szTitle;
	CString szMsg, szFullPath;
	BOOL bReturn = FALSE, bAlwaysFalse = FALSE;
	BOOL bShowAlert = FALSE;
	BOOL bResetSerialNo = FALSE;

	//Nichia//v4.43T7
	if (m_bPortablePKGFile && (GetCustomerName() == CTM_NICHIA) && (GetProductLine() == ""))	//v4.59A34
	{
		return RestoreNichiaPRM(szDeviceFile, FALSE);
	}

	m_DeviceFile.SetLoadPkgOption(m_bLoadPkgWithBin);
	m_DeviceFile.SetOnlyLoadBinParameters(m_bOnlyLoadBinParam);
	m_DeviceFile.SetOnlyLoadWFTParameters(m_bOnlyLoadWFTParam);
	m_DeviceFile.SetExcludeMachineParameters(m_bExcludeMachineParam);
	m_DeviceFile.SetPortablePKGFile(m_bPortablePKGFile);
	m_DeviceFile.SetOnlyLoadPRParameters(m_bOnlyLoadPRParam);
	m_DeviceFile.SetPortablePackageFileFileList(m_szaPortablePackageFileFileList);
	m_DeviceFile.SetIsPackageFileIgnoreList(m_bIsPortablePackageFileIgnoreList);

	szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);
	if (bPopUpConfirmDialog == TRUE)
	{
		//Prompt user restore PKG file will clear bin counter
		szMsg.LoadString(HMB_MS_LOAD_PKG_FILE_CONFIRM);
		if (HmiMessage(szMsg, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL) == glHMI_NO)
		{
			return FALSE;
		}
	}

	CStringMapFile smfList;
	if (smfList.Open(gszROOT_DIRECTORY + _T("\\Exe\\DF_FileList.msd")) == FALSE)
	{
		HmiMessage("DF_FileList.msd Missing! Restore Fails!");
		return FALSE;
	}
	smfList.Close();

	szFullPath = szDevicePath + "\\" + szDeviceFile;
	CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestoreFromDevice - " + szFullPath);	//v4.52A10

	//StartLoadingAlert();		// Show the waiting message
	OpenWaitAlert();			//v4.33T1	//PLLM


	//SeoulSemi: recover all of tempdatabase file
//	if ( GetCustomerName() == "SeoulSemi" ) 

	if ( m_bEnableSaveTempFileWithPKG )
	{
		// recover tempfile
		CStringList szSelection;
		CONST CString szBackupPath = m_szDevicePathDisplay + "\\" + GetPKGFilename() + "_TEMPDATA\\"; 
		CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";

		WIN32_FIND_DATA FileData;
		HANDLE hSearch;
		hSearch = FindFirstFile( szBackupPath + "Blk*tempfile.csv", &FileData);
		if ( hSearch == INVALID_HANDLE_VALUE ) 
		{ 
			//HmiMessage("No Temp Database file recover.");
			SetErrorMessage("No Temp Database file recover.");
		}
		else 
		{
			CopyFile( szBackupPath + FileData.cFileName, szExePath + FileData.cFileName, FALSE);
			Sleep(1);
			int counter = 1;
			while (FindNextFile(hSearch, &FileData) == TRUE)
			{
				CopyFile( szBackupPath + FileData.cFileName, szExePath + FileData.cFileName, FALSE);
				Sleep(1);
				counter++;
			}
			CString szRecoverResult;
			szRecoverResult.Format("%d Temp Database file recovered.", counter);
			SetErrorMessage(szRecoverResult);
			//HmiMessage( szRecoverResult );
		}

		// recover NVRAM.bin
		BOOL bStatus = ImportNVRAMDataFromTxtFile( szBackupPath + _T("NVRAM.bin") );
		if (bStatus)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM data is imported from NVRAM.bin"); //v3.55
			CString szTemp = szBackupPath + _T("NVRAM.bin");
			szTemp.Format( "NVRAM file is imported from: %s", szTemp );
			//HmiMessage(szTemp);
			SetErrorMessage(szTemp);
			// copy NVRAM.bin
			CopyFile( szBackupPath + _T("NVRAM.bin"), szExePath + _T("NVRAM.bin"), FALSE);
		}
		else 
		{
			//HmiMessage("NVRAM file import error");
			SetErrorMessage("NVRAM file import error");
		}
	}

	// Check whether the file is existing
	FILE *pFile = NULL;
	errno_t nErr = fopen_s(&pFile, szFullPath, "r");
	if ((nErr != 0) || (pFile == NULL))
	{
		//CloseLoadingAlert();		// Close the waiting message
		CloseWaitAlert();			//v4.33T1	//PLLM

		if (bPopupCompleteDialog)
		{
			szMsg.LoadString(HMB_MS_FILE_NOT_EXIST);
			HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			HmiMessage("DEVICE file " + szDeviceFile + " cannot be opened for reading; please check this path at: " + szFullPath);	//v4.15T9
		}

		SetErrorMessage("DEVICE file " + szDeviceFile + " cannot be opened for reading; please check this path at: " + szFullPath);
		return FALSE;
	}
	fclose(pFile);

	CMSLogFileUtility::Instance()->MS_LogOperation("LDF full name " + szFullPath);
	if ( CreateDirectory(gszPR_RECORD_PATH, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			//CloseLoadingAlert();		// Close the waiting message
			CloseWaitAlert();			//v4.33T1	//PLLM

			if (bPopupCompleteDialog)
			{
				szMsg.LoadString(HMB_MS_CREATE_DRIECTORY_FAILED);
				HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			}
			return FALSE;
		}
	}
	
	//BOOL bNeedDownloadPRRecord = FALSE;		//v4.53A22

	// Set the record path
	(m_smfSRam)["PR"]["RecordPath"] = gszPR_RECORD_PATH;

	CMSLogFileUtility::Instance()->MS_LogOperation("LDF delete wafer PR record files");
	// Delete the WaferPR Records In the Folder First		
	DeletePrRecordFiles(TRUE);
	DeletePrRecordFiles(FALSE);

	CMSLogFileUtility::Instance()->MS_LogOperation("LDF restore file (break PKG to its own files)");
	if ( m_DeviceFile.Restore(szFullPath) )
	{
		if( m_bOnlyLoadBinParam )
		{
			bReturn = TRUE;
			szMsg.LoadString(HMB_MS_LOAD_PKG_OK);
		}
		else
		{	
			if ((m_bDownloadPortablePackageFileImage == FALSE) && IsPortablePKGFile() == TRUE)
			{
				bReturn = TRUE;
				szMsg.LoadString(HMB_MS_LOAD_PKG_OK);	
				SavePackageInfo();
			}
			else
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("LDF to download wafer PR records");
				// Download PR Records
				svMsg.InitMessage(sizeof(BOOL), &bAlwaysFalse);	// Init the message
				// Download the WaferPR Records			
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "DownloadPrRecords", svMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID, svMsg);
						svMsg.GetMsg(sizeof(BOOL), &bReturn);
						break;
					}
					else
					{
						Sleep(10);
					}
				}

				if ( bReturn == TRUE )
				{
					szMsg.LoadString(HMB_MS_LOAD_PKG_OK);

					svMsg.InitMessage(sizeof(BOOL), &bAlwaysFalse); // Init the message
					// Download the BondPR Records			
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "DownloadPrRecords", svMsg);
					while (1)
					{
						if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
						{
							m_comClient.ReadReplyForConvID(nConvID, svMsg);
							svMsg.GetMsg(sizeof(BOOL), &bReturn);
							break;
						}
						else
						{
							Sleep(10);
						}
					}

					if ( bReturn == TRUE )
					{
						// Delete the BondrPR Records			
						DeletePrRecordFiles(FALSE);

						// Delete the WaferPR Records			
						DeletePrRecordFiles(TRUE);
						
						SavePackageInfo();		// Save package info to String Map file
					}
					else
					{
						szMsg.LoadString(HMB_MS_DOWNLOAD_BPR_FAILED);
					}
				}
				else
				{
					szMsg.LoadString(HMB_MS_DOWNLOAD_WPR_FAILED);
				}
			}

		}//end of OnlyLoadBinParam is false case

		CMSLogFileUtility::Instance()->MS_LogOperation("LDF restore file done");
	}
	else
	{
		szMsg.LoadString(HMB_MS_LOAD_PKG_FAILED);
		bReturn = FALSE;	//v4.52A10
	}

	//CloseLoadingAlert();		// Close the waiting message

	if (bPopupCompleteDialog == TRUE)
	{
		HmiMessage(szMsg, szTitle);
	}

	SetStatusMessage("Package file is restored");

	m_eqMachine.SetPackage (szDeviceFile);
	m_eqMachine2.SetPackage(szDeviceFile);	//WH Sanan	//v4.40T4

/*
	if ( bReturn == FALSE )
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestoreFromDevice fails");	//v4.52A10
		CloseWaitAlert();			//v4.33T1	//PLLM
		return FALSE;
	}
*/
	m_bManualLoadPkgFile = TRUE;	//v4.53A22
	m_smfSRam["MS896A"]["Reset Bin Serial No"]	= m_bResetBinSerialNo;
	CMSLogFileUtility::Instance()->MS_LogOperation("LDF machine warm start");
	// Warm Start the machine
	MachineWarmStart(TRUE);
	CloseWaitAlert();			//v4.33T1	//PLLM
	LoadFileFormatSelectInNewLot();
	CMSLogFileUtility::Instance()->MS_LogOperation("LDF machien warn start done");

	m_bManualLoadPkgFile = FALSE;	//v4.53A22

	if ( m_bEnableSaveTempFileWithPKG )
	{
	}
	else if ( (GetCustomerName() == CTM_NICHIA) && (GetProductLine() == "") && m_bPortablePKGFile)	//v4.59A34
	{
		//Do not clear map if Nichia manual load PRM file	//v4.42T12
	}
	else
	{
		//Clear All Bin Counters
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ClearAllBinCounters", svMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		// Copy Reset Serial No Flag
		bResetSerialNo = m_bResetBinSerialNo;
		// If is Traceability fomrat, Not to Reset Bin Serial
		if (CMS896AStn::m_lOTraceabilityFormat == 1 || CMS896AStn::m_lOTraceabilityFormat == 2)
			bResetSerialNo = FALSE;

		if ( ((m_bLoadPkgWithBin == TRUE) || m_bOnlyLoadBinParam == TRUE) && (bResetSerialNo == TRUE))
		{
			//Clear Bin's serial no
			bShowAlert = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bShowAlert);
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinSerialNo", svMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, svMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//Clear Bin Table clear bin time
			bShowAlert = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bShowAlert);
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinClearedCount", svMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,svMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("LDF init wafer map");
		CMS896AStn::m_WaferMapWrapper.InitMap();
	}


	//Update Load PKG file count	
	int nCount, nCol = 0;
	CString szLastDate, szCurrDate, szPKGCount;

	CTime theTime = CTime::GetCurrentTime();	

	if ( (nCol = m_szDeviceCount.Find(":")) != -1 )
	{
		nCount = atoi((LPCTSTR)m_szDeviceCount.Left(nCol));
		szLastDate = m_szDeviceCount.Right(m_szDeviceCount.GetLength() - nCol -1);
		szCurrDate.Format("%d-%d-%d", theTime.GetDay(), theTime.GetMonth(), theTime.GetYear());

		if ( szLastDate != szCurrDate )
		{
			if ( nCount == 0 )
			{
				nCount = 1;
			}
			else
			{
				nCount = 0;
			}
		}
		else
		{
			nCount++;
		}

		if (nCount >= 36)
		{
			nCount = 0;
		}

		szPKGCount.Format("%d", nCount);

		m_szDeviceCount = szPKGCount + ":" + szCurrDate;
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_COUNT_SETTING, m_szDeviceCount);

		m_smfSRam["MS896A"]["DevFileCount"] = szPKGCount;
	}

	if (bPopupCompleteDialog == TRUE)
	{
		szMsg.LoadString(HMB_MS_WARN_START_OK);
		HmiMessage(szMsg, szTitle);
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestoreFromDevice completed");	//v4.52A10
	return TRUE;
}

BOOL CMS896AApp::StorePKGPreviewImage(CString szDevicePath, CString szDeviceFile)
{

	LPTSTR lpsz = new TCHAR[255];
	IPC_CServiceMessage stMsg;

	// call to pr to get the cliped images
	CString szPreviewImagePath;
	CString szTempDeviceFile;
	INT nCol = -1;
	
	szDevicePath = szDevicePath;

	szTempDeviceFile = szDeviceFile;
	//nCol = szTempDeviceFile.Find(".");

	//if (nCol != -1)
	//{
	//	szTempDeviceFile = szTempDeviceFile.Left(nCol);
	//}

	szPreviewImagePath = szDevicePath + "\\" + WPR_PREVIEW_IMAGES;
	CreateDirectory(szPreviewImagePath, NULL);

	szPreviewImagePath = szPreviewImagePath + "\\" + szTempDeviceFile + ".jpg";
	
	strcpy_s(lpsz, 255, (LPCTSTR)szPreviewImagePath);

	stMsg.InitMessage(sizeof(TCHAR)*255, lpsz);

	int nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UploadPrPkgPreviewImage", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	delete [] lpsz;
	
	return TRUE;
}

BOOL CMS896AApp::DeletePKGPreviewImage(CString szDevicePath , CString szDeviceFile)
{
	CString szPreviewImagePath;
	CString szTempDeviceFile;
	
	szDevicePath = szDevicePath;
	szTempDeviceFile = szDeviceFile;

	szPreviewImagePath = szDevicePath + "\\" + WPR_PREVIEW_IMAGES;
	szPreviewImagePath = szPreviewImagePath + "\\" + szTempDeviceFile + ".jpg";

	DeleteFile(szPreviewImagePath);
	m_szPKGPreviewPath = "";

	return TRUE;
}


VOID CMS896AApp::ChangePkgFilePreviewDiagram(CString szDevicePath, CString szDeviceFile)
{
	DOUBLE dRatio = 1.0;
	LONG lPreviewImageSizeX, lPreviewImageSizeY;
	CImage ImPreviewIamge;
	CTime curTime = CTime::GetCurrentTime();
	INT nCol = -1;
	//CString szDevicePath = m_szDevicePath;

	CString szFilename = szDeviceFile;

	CString szPreviewImagePath = "";
	CString szTempFilePath = "";
	CString szCurDateTime = curTime.Format("%Y%m%d%H%M%S");

	szPreviewImagePath = szDevicePath + "\\" + WPR_PREVIEW_IMAGES;
	szPreviewImagePath = szPreviewImagePath + "\\" + szFilename + ".jpg";

	// cannot find fhe image
	if (_access(szPreviewImagePath, 0) == -1)
	{
		m_szPKGPreviewPath = "";
		return;
	}

	szTempFilePath = gszUSER_DIRECTORY + "\\" + WPR_PKG_PREVIEW_TEMP ;

	CreateDirectory(szTempFilePath, NULL);
	
	// Clean up the image show before
	SearchAndRemoveFiles(szTempFilePath, 0, TRUE);

	if (_access(szTempFilePath, 0) == -1)
	{
		CreateDirectory(szTempFilePath, NULL);
	}

	// Set the filename of the image
	szTempFilePath = szTempFilePath + "\\" + szFilename + szCurDateTime + ".jpg";

	// copy it to local so that the control will not lock the image 
	CopyFile(szPreviewImagePath, szTempFilePath ,FALSE);
	
	m_szPKGPreviewPath = szTempFilePath;

	ImPreviewIamge.Load(szTempFilePath);
	lPreviewImageSizeY = ImPreviewIamge.GetHeight();
	lPreviewImageSizeX = ImPreviewIamge.GetWidth();

	m_dPreviewImageSizeX = 1;
	m_dPreviewImageSizeY = 1;

	if ((lPreviewImageSizeY > WPR_PRVIEW_IMAGE_SIZE) || (lPreviewImageSizeX > WPR_PRVIEW_IMAGE_SIZE))
	{
		if (lPreviewImageSizeY > WPR_PRVIEW_IMAGE_SIZE)
		{
			m_dPreviewImageSizeY = (DOUBLE)WPR_PRVIEW_IMAGE_SIZE/(DOUBLE)lPreviewImageSizeY;
		}

		if (lPreviewImageSizeX > WPR_PRVIEW_IMAGE_SIZE)
		{
			m_dPreviewImageSizeX = (DOUBLE)WPR_PRVIEW_IMAGE_SIZE/(DOUBLE)lPreviewImageSizeX;
		}

		if (lPreviewImageSizeY > lPreviewImageSizeX)
		{
			dRatio = (DOUBLE)lPreviewImageSizeX / (DOUBLE)lPreviewImageSizeY;
			m_dPreviewImageSizeX = m_dPreviewImageSizeX * dRatio;
		}
		else
		{
			dRatio = (DOUBLE)lPreviewImageSizeY / (DOUBLE)lPreviewImageSizeX;
			m_dPreviewImageSizeY = m_dPreviewImageSizeY * dRatio;
		}
	}
}


BOOL CMS896AApp::GeneratePkgDataFile(CONST CString szFilePath, CONST CString szPkgName)
{
	if (GetCustomerName() != "Walsin")
		return TRUE;
	if (szFilePath.GetLength() == 0)
		return TRUE;

	if (_access(szFilePath, 0) == -1)
	{
		HmiMessage("ERROR: package-file-list destination path is not available - " + szFilePath);
		return FALSE;
	}

	CString szDevicePath	= szFilePath;
	//if ( (szDevicePath.GetLength() == 0) || (_access(szDevicePath, 0) == -1) )
	//{
	//	szDevicePath = m_szDevicePath;	
	//}

	CTime theTime = CTime::GetCurrentTime();
	CString szCurrTime	= theTime.Format("%Y%m%d%H%M%S");
	CString szCurrDate	= theTime.Format("%Y/%m/%d");
	CString szTime		= theTime.Format("%H:%M:%S");

	CString szPRVersion	= (m_smfSRam)["MS896A"]["Vision Software Version"];

	//v4.24T5
	CString szPkgDataFileName		= m_szMachineNo + " " + szPkgName + ".CSV";
	
	CString szLocalPkgDataFileName	= gszUSER_DIRECTORY + "\\" + szPkgDataFileName;
	szPkgDataFileName				= szDevicePath + "\\" + szPkgDataFileName;
	(m_smfSRam)["MS896A"]["PKG Local Data File Name"]	= szLocalPkgDataFileName;

	
	CMSLogFileUtility::Instance()->MS_LogOperation("MS: create PKG data file (Walsin) - " + szPkgDataFileName);
	CStdioFile oFile;
	if (oFile.Open(szLocalPkgDataFileName, 
					CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) != TRUE)
	{
		HmiMessage("ERROR: unable to open package data file - " + szLocalPkgDataFileName);
		return FALSE;
	}


	//Write data file header into file
	oFile.WriteString("Machine NO:," + m_szMachineNo + "\n");
	oFile.WriteString("Updata Date:," + szCurrDate + "\n");
	oFile.WriteString("Updata Time:," + szTime + "\n");
	oFile.WriteString("Package file name:," + szPkgName + "\n\n");
	oFile.Close();


	//Write wafertable data into file
	INT nConvID = 0;
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;

	//Write WaferTable data into file
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GeneratePkgDataFile", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if (bReturn == FALSE)
	{
		HmiMessage("ERROR: Fail to write WaferTable data into PKG data file!");
		SetErrorMessage("ERROR: Fail to write WaferTable data into PKG data file!");
		return FALSE;
	}

	//Write BinTable data into file
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GeneratePkgDataFile", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if (bReturn == FALSE)
	{
		HmiMessage("ERROR: Fail to write BinTable data into PKG data file!");
		SetErrorMessage("ERROR: Fail to write BinTable data into PKG data file!");
		return FALSE;
	}


	//Write BOndHead data into file
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "GeneratePkgDataFile", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if (bReturn == FALSE)
	{
		HmiMessage("ERROR: Fail to write BondHead data into PKG data file!");
		SetErrorMessage("ERROR: Fail to write BondHead data into PKG data file!");
		return FALSE;
	}


	//FInally, copy local PKG data file to network share
	TRY 
	{
		if (CopyFile(szLocalPkgDataFileName, szPkgDataFileName, FALSE))
		{
			DeleteFile(szLocalPkgDataFileName);
		}

	} CATCH (CFileException, e) 
	{
		SetErrorMessage("ERR: Exception in GeneratePkgDataFile: " + szPkgDataFileName);
	}
	END_CATCH

	return TRUE;
}


BOOL CMS896AApp::GeneratePkgFileList(CONST CString szFilePath)
{
	if (GetCustomerName() != "Walsin")
		return TRUE;
	if (szFilePath.GetLength() == 0)
		return TRUE;

	if (_access(szFilePath, 0) == -1)
	{
		HmiMessage("ERROR: package-file-list destination path is not available - " + szFilePath);
		return FALSE;
	}

	CString szDevicePath;
	if (m_ucDeviceFileType == MS_PACKAGE_FILE_TYPE)
	{
		szDevicePath = m_szDevicePath;	
	}
	else
	{
		szDevicePath = m_szPortablePKGPath;
	}

	
	CString szFilename, szFolderPath;
	WIN32_FIND_DATA FileData;
	CString szFileName, szFileTime, szFileSize;
	CString szLine;
	CString szLineNo;
	LONG lFileCount = 0;
	CFileStatus status;

	CTime theTime = CTime::GetCurrentTime();
	CString szCurrTime	= theTime.Format("%Y%m%d%H%M%S");
	CString szCurrDate	= theTime.Format("%Y/%m/%d");
	CString szTime		= theTime.Format("%H:%M:%S");

	CString szPRVersion	= (m_smfSRam)["MS896A"]["Vision Software Version"];

	//v4.24T5
	//CString szPkgFileListName		= m_szMachineNo + " Package file list-" + szCurrTime + ".CSV";
	CString szPkgFileListName		= m_szMachineNo + " Package file list.CSV";
	
	CString szLocalPkgFileListName	= gszUSER_DIRECTORY + "\\" + szPkgFileListName;
	szPkgFileListName				= szFilePath + "\\" + szPkgFileListName;


	CMSLogFileUtility::Instance()->MS_LogOperation("MS: create PKG file list (Walsin) - " + szPkgFileListName);
	CStdioFile oFile;
	if (oFile.Open(szLocalPkgFileListName, 
					CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) != TRUE)
	{
		HmiMessage("ERROR: unable to open package file list - " + szLocalPkgFileListName);
		return FALSE;
	}


	HANDLE hSearch = FindFirstFile(szDevicePath + "\\" + "*", &FileData);
	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		oFile.Close();
		HmiMessage("ERROR: invalid file path - " + szDevicePath);
		return FALSE;
	}


	//File List Header fields
	oFile.WriteString("Machine NO:," + m_szMachineNo + "\n");
	oFile.WriteString("Updata Date:," + szCurrDate + "\n");
	oFile.WriteString("Updata Time:," + szTime + "\n");
	oFile.WriteString("Control SW Version:," + m_szSoftVersion + "\n");
	oFile.WriteString("Vision SW Version:," + szPRVersion + "\n");
	oFile.WriteString(",name,size,date\n");


	do 
	{
		// if it is a folder;
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		lFileCount++;
		szLineNo.Format("%ld,", lFileCount); 

		szFileName = FileData.cFileName;

		//CTime ctFileTime(FileData.ftLastWriteTime);
		CFile::GetStatus(szDevicePath + "\\" + szFileName, status);
		szFileTime = status.m_mtime.Format("%d/%m/%y %I:%M:%S");
		if (status.m_mtime.GetHour() > 12)
			szFileTime = szFileTime + " PM";
		else
			szFileTime = szFileTime + " AM";

		DOUBLE dFileSize = ((double)FileData.nFileSizeHigh * (1 + (double)MAXDWORD) + (double)FileData.nFileSizeLow) / 1024;
		szFileSize.Format("%.0fK", dFileSize);

		szLine = szLineNo + szFileName + "," + szFileSize + "," + szFileTime;
		oFile.WriteString(szLine + "\n");
	}while (FindNextFile(hSearch, &FileData) == TRUE);
 
	// Close the search handle. 
	FindClose(hSearch);
	oFile.Close();


	TRY 
	{
		if (CopyFile(szLocalPkgFileListName, szPkgFileListName, FALSE))
		{
			DeleteFile(szLocalPkgFileListName);
		}

	} CATCH (CFileException, e) 
	{
		HmiMessage("ERR: Exception in GeneratePkgFileList: " + szPkgFileListName);
		SetErrorMessage("ERR: Exception in GeneratePkgFileList: " + szPkgFileListName);
	}
	END_CATCH

	return TRUE;
}


BOOL CMS896AApp::SaveBinRunTimeData(CString szFilePath, CString szFilename)
{
	CString szTitle, szMsg;
	BOOL bReturn = TRUE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;

	szTitle.LoadString(HMB_MS_SAVE_BIN_DATA);
	
	CString szFile = szFilePath + "\\" + szFilename ;

	SetStatusMessage("Start save bin data");

	StartLoadingAlert();		// Show the waiting message
	// Check whether the path is existing
	if ( CreateDirectory(szFilePath, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			CloseLoadingAlert();		// Close the waiting message

			szMsg.LoadString(HMB_MS_CREATE_DRIECTORY_FAILED);

			HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			return FALSE;
		}
	}

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "UploadBinBlkRunTimeData", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn == FALSE)
	{
		CloseLoadingAlert();
		SetErrorMessage("Fail to upload bin runtime data");
		szMsg.LoadString(HMB_MS_SAVE_BIN_DATA_FAILED);
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	//m_DeviceFile.RemoveTempBinRunTimeFile();

	if (m_DeviceFile.SaveBinRunTimeFile(szFile) == FALSE)
	{
		CloseLoadingAlert();
		SetErrorMessage("Fail to save bin runtime data");
		szMsg.LoadString(HMB_MS_SAVE_BIN_DATA_FAILED);
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "GenBinBlkRunTimeSummary", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn == FALSE)
	{
		CloseLoadingAlert();
		SetErrorMessage("Fail to generate bin block summary");
		szMsg.LoadString(HMB_MS_SAVE_BIN_DATA_FAILED);
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	CloseLoadingAlert();
	szMsg.LoadString(HMB_MS_SAVE_BIN_DATA_COMPLETE);
	HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	SaveAppData();

	SetStatusMessage("Save bin data complete");

	return TRUE;
}

BOOL CMS896AApp::RestoreBinRunTimeData(CString szFilePath, CString szFilename)
{
	BOOL bIsBinLoader = (BOOL)(LONG)(m_smfSRam)["BinLoaderStn"]["Enabled"];
	
	BOOL bReturn = TRUE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;
	CString szFile, szTitle, szMsg;

	szTitle.LoadString(HMB_MS_RESTORE_BIN_DATA);
	szMsg.LoadString(HMB_MS_RESTORE_BIN_CONFIRM);

	if (HmiMessage(szMsg, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, 
		NULL, NULL, NULL, NULL) == glHMI_NO)
	{
		return FALSE;
	}

	SetStatusMessage("Start restore bin data");

	StartLoadingAlert();
	szFile = szFilePath + "\\" + szFilename ;

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "RemoveAllTempFile", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn == FALSE)
	{
		CloseLoadingAlert();
		SetErrorMessage("Fail to remove temp files");
		szMsg.LoadString(HMB_MS_REMOVE_RUNTIME_FILES_FAILED);
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ClearNVRunTimeData", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn == FALSE)
	{
		CloseLoadingAlert();
		SetErrorMessage("Fail to clear NVRam data");
		szMsg.LoadString(HMB_MS_REMOVE_RUNTIME_FILES_FAILED);
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	
	if (m_DeviceFile.RestoreBinRunTimeFile(szFile) == FALSE)
	{
		CloseLoadingAlert();
		SetErrorMessage("Fail to restore bin run time data");
		szMsg.LoadString(HMB_RESTORE_BIN_DATA_FAILED);
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}
		
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "DownloadBinBlkRunTimeData", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (bReturn == FALSE)
	{
		CloseLoadingAlert();
		SetErrorMessage("Fail to download bin run time data");
		szMsg.LoadString(HMB_RESTORE_BIN_DATA_FAILED);
		HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		return FALSE;
	}

	// Store Bin Blk Data to SRAM
	BackupToNVRAM();

	SetStatusMessage("Warm Start start");
	MachineWarmStart(TRUE);
	SetStatusMessage("Warm Start complete");

	//Clear Wafermap 
	CMS896AStn::m_WaferMapWrapper.InitMap();

	// reset all magazine
	if (bIsBinLoader == TRUE)
	{
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "RestoreMagazineRunTimeStatus", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bReturn == FALSE)
		{
			CloseLoadingAlert();
			SetStatusMessage("Fail to restore magazine status");
			szMsg.LoadString(HMB_RESTORE_BIN_DATA_FAILED);
			HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
			return FALSE;
		}
	}

	CloseLoadingAlert();
	
	szMsg.LoadString(HMB_RESTORE_BIN_DATA_COMPLETED);
	HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		
	SaveAppData();
	SetStatusMessage("Restore bin data complete");
	
	return TRUE;
}

#include "HmiFolderDialog.h"

BOOL CMS896AApp::GetColorWithDialog(COLORREF &ulReturnColour)
{
	CColorDialog dlg(ulReturnColour, CC_FULLOPEN, m_pMainWnd); // new colour dialog

	ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
//	dlg.SetCurrentColor(ulReturnColour);
	INT nReturn = (INT)dlg.DoModal();		// Show the file dialog box
	ShowHmi();			// Show the HMI on top

	if ( nReturn == IDOK)
	{
		ulReturnColour = dlg.GetColor(); 
		return TRUE;
	}

	return FALSE;
}

// Get the folder path via pop-up dialog
// REMARK: Since it is failed to pass string from HMI, use this approach instead
BOOL CMS896AApp::GetPath(CString &szPath)
{
	CString szTitle		= "Select The Folder Path";
	CHmiFolderDialog	dlg(szTitle, szPath, BIF_RETURNONLYFSDIRS | BIF_USENEWUI, m_pMainWnd);

	ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	INT nReturn = (INT)dlg.DoModal();		// Show the file dialog box
	ShowHmi();			// Show the HMI on top

	if ( nReturn == IDOK)
	{
		szPath = dlg.GetFolderPath();
		return TRUE;
	}

	return FALSE;
}

// Show the application on top with mininized size
VOID CMS896AApp::ShowApp(BOOL bMinimized)
{
	// Set the application to TopMost, otherwise the dialog box will only show at background
	m_pMainWnd->CenterWindow();
	if ( bMinimized == TRUE )
	{
		m_pMainWnd->SetWindowPos(&CWnd::wndTopMost, 
								(m_stWinRect.right + m_stWinRect.left) / 2, 
								(m_stWinRect.bottom + m_stWinRect.top) / 2,
								0, 0,
								SWP_HIDEWINDOW);
	}
	else
	{
		m_pMainWnd->SetWindowPos(&CWnd::wndBottom, m_stWinRect.left, m_stWinRect.top, 
								(m_stWinRect.right - m_stWinRect.left), (m_stWinRect.bottom - m_stWinRect.top),
								SWP_SHOWWINDOW);
	}
}

// Show HMI and Set the application to bottom with original size
VOID CMS896AApp::ShowHmi()
{
	// Restore the application to background
	m_pMainWnd->SetWindowPos(&CWnd::wndBottom, m_stWinRect.left, m_stWinRect.top, 
							(m_stWinRect.right - m_stWinRect.left), (m_stWinRect.bottom - m_stWinRect.top),
							SWP_SHOWWINDOW | SWP_NOACTIVATE);

	CWnd *pHMI = CWnd::FindWindow(NULL, _T("ASM Human Machine Interface"));	// Get the HMI handle
	if ( pHMI != NULL )
	{
		pHMI->SetForegroundWindow();
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Loading Dialog
////////////////////////////////////////////////////////////////////////////////////

VOID CMS896AApp::CreateLoadingAlert(BOOL bCreate)
{
	return;
/*
	RECT stRect;

	if (bCreate)
	{
		if ( m_pLoadingDialog == NULL )
		{
			m_pLoadingDialog = new CDialog();
			
			// If fail to create dialog, simply return
			if ( m_pLoadingDialog == NULL )
				return;
			
			m_pLoadingDialog->Create(IDD_LOADING);
			m_pLoadingDialog->CenterWindow();
			m_pLoadingDialog->GetWindowRect(&stRect);
			m_pLoadingDialog->SetWindowPos(&CWnd::wndTopMost, stRect.left, stRect.top,
											(stRect.right-stRect.left+1), (stRect.bottom-stRect.top+1), SWP_HIDEWINDOW);

			//m_pLoadingDialog->ShowWindow(SW_HIDE);
			m_pLoadingDialog->UpdateWindow();
		}
	}
	else
	{
		if ( m_pLoadingDialog != NULL )
		{
			INT nResult = IDOK;
			m_pLoadingDialog->EndDialog(nResult);
			m_pLoadingDialog->DestroyWindow();
			delete m_pLoadingDialog;
			m_pLoadingDialog = NULL;
		}
	}
*/
}

// Show and close the loading alert box
VOID CMS896AApp::StartLoadingAlert()
{
	return;
/*
	RECT stRect;
	if ( m_pLoadingDialog != NULL )
	{
		m_pLoadingDialog->ShowWindow(SW_SHOW);
		m_pLoadingDialog->UpdateWindow();
	}
*/
}

VOID CMS896AApp::CloseLoadingAlert()
{
	return;
/*
	if ( m_pLoadingDialog != NULL )
	{
		m_pLoadingDialog->ShowWindow(SW_HIDE);
		m_pLoadingDialog->UpdateWindow();

		CWnd *pHMI = CWnd::FindWindow(NULL, _T("ASM Human Machine Interface"));	// Get the HMI handle
		if ( pHMI != NULL )
		{
			pHMI->SetForegroundWindow();
		}
	}

	/*
	INT nResult=IDOK;
	if ( m_pLoadingDialog != NULL )
	{
		m_pLoadingDialog->EndDialog(nResult);
		m_pLoadingDialog->DestroyWindow();
//		ShowHmi();		// Set HMI to Top Most
		delete m_pLoadingDialog;
		m_pLoadingDialog = NULL;
		// Remark: Not call ShowHmi() of MS896A since it will show the application window
		CWnd *pHMI = CWnd::FindWindow(NULL, _T("ASM Human Machine Interface"));	// Get the HMI handle
		if ( pHMI != NULL )
		{
			pHMI->SetForegroundWindow();
		}
	}
	*/
}

VOID CMS896AApp::OpenWaitAlert()
{
	if ( m_pLoadingDialog == NULL )
	{
		RECT stRect;
		m_pLoadingDialog = new CDialog();

		// If fail to create dialog, simply return
		if ( m_pLoadingDialog == NULL )
			return;
		
		m_pLoadingDialog->Create(IDD_LOADING);
		m_pLoadingDialog->CenterWindow();
		m_pLoadingDialog->GetWindowRect(&stRect);
		m_pLoadingDialog->SetWindowPos(&CWnd::wndTopMost, stRect.left, stRect.top,
										(stRect.right-stRect.left+1), (stRect.bottom-stRect.top+1), SWP_HIDEWINDOW);

		m_pLoadingDialog->ShowWindow(SW_SHOW);
		m_pLoadingDialog->UpdateWindow();
	}
}

VOID CMS896AApp::CloseWaitAlert()
{
	if ( m_pLoadingDialog != NULL )
	{
		INT nResult=IDOK;
		m_pLoadingDialog->EndDialog(nResult);
		m_pLoadingDialog->DestroyWindow();
		delete m_pLoadingDialog;

		m_pLoadingDialog = NULL;
		// Remark: Not call ShowHmi() of MS896A since it will show the application window
		CWnd *pHMI = CWnd::FindWindow(NULL, _T("ASM Human Machine Interface"));	// Get the HMI handle
		if ( pHMI != NULL )
		{
			pHMI->SetForegroundWindow();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Code & Decode the password
////////////////////////////////////////////////////////////////////////////////////
CString CMS896AApp::CodePassword(CString &szPass, INT nShift)
{
	CString szResult;
	INT nSize, i, nTemp;
	CHAR *pcData;
	
	nSize = szPass.GetLength();
	pcData = new CHAR[nSize + 1];
	if (pcData == NULL)
	{
		return szPass;
	}
	memset(pcData, 0x00, nSize + 1);
	strcpy_s(pcData, nSize + 1, (const char*)szPass);

	for (i = 0; i < nSize; i++)
	{
		nTemp = *(pcData + i) + nShift;

		//v4.31T9
		// Remark: Start from '!' (33) to '~' (126)
		//if ( nTemp > 126 )
		//	nTemp -= 93;		// nTemp - '~' + '!'
		//if ( nTemp < 33 )
		//	nTemp += 93;		// nTemp + '~' - '!'

		*(pcData + i) = (CHAR)nTemp;
	}
	szResult.Format("%s", pcData);
	delete [] pcData;
	return szResult;
}

////////////////////////////////////////////////////////////////////////////////////
// Check whether all hardware initiated
////////////////////////////////////////////////////////////////////////////////////
BOOL CMS896AApp::CheckAllHardwareReady(CString &szFailStnName)
{
	SFM_CStation*	pStn;
	CString			szStn;
	CMS896AStn*		pTemp;

	if (m_fEnableHardware == FALSE)
	{
		return TRUE;
	}

	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		m_pStationMap.GetNextAssoc(pos, szStn, pStn);
		pTemp = dynamic_cast<CMS896AStn*> (pStn);
		if (pTemp)
		{
			if ( pTemp->IsHardwareReady() == FALSE )
			{
				szFailStnName = pTemp->GetName();
				return FALSE;
			}
		}
	}
	return TRUE;
}



LONG CMS896AApp::ChangeWaferFrame(BOOL bBurnInApp, BOOL bStarted)
{
	IPC_CServiceMessage rReqMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	BOOL bReturn = FALSE;

	LONG lStatus = 0;	

	BOOL bHalfDone		= (BOOL)((LONG)m_smfSRam["WaferTable"]["WaferFirstPartDone"]);

	BOOL bRegionEnd		= (BOOL)((LONG)m_smfSRam["WaferTable"]["WaferRegionEnd"]);
	BOOL bIsWaferEnd	= (BOOL)((LONG)m_smfSRam["WaferTable"]["WaferEnd"]);
	BOOL bIsEnableWL	= (BOOL)((LONG)m_smfSRam["WaferLoaderStn"]["Enabled"]);	

	CString szMsg;
	szMsg.Format("APP: cycle stop to change wafer frame. burnin %d, cyclestart %d, region end %d, wafer end %d, half done %d",
		bBurnInApp, bStarted, bRegionEnd, bIsWaferEnd, bHalfDone);
	LogCycleStopStatus(szMsg);

	if( ((bIsEnableWL == FALSE) || (bStarted == FALSE)) && (bRegionEnd==FALSE) )
	{
		//Waferloader not created or Disable!
		CString szText;
		
		szText.Format("No WL Module %d,%d", bIsEnableWL, bStarted);
		LogCycleStopStatus(szText);
		return -1;
	}

	BOOL bDimStopBtn = (bRegionEnd || bIsWaferEnd || bHalfDone);

	if( bDimStopBtn )
	{
		CMS896AStn::m_bWaferAlignComplete = FALSE;
		m_bIsChangeWafer = TRUE;
	}

	BOOL bEndOk = (BOOL)((LONG)m_smfSRam["WaferTable"]["WaferEndSmooth"]);	//v4.46T1	//Re-enable for Genesis

	if( ((bIsEnableWL == FALSE) || (bStarted == FALSE)) )
	{
		if( bRegionEnd && pUtl->GetPrescanRegionMode() )
		{
			//Waferloader not created or Disable!
			// should send command to wafer table
			BOOL bResult = FALSE;
			LogCycleStopStatus("Start region align wafer");

			int nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoRegionAlignStdWafer", rReqMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 500000)==TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if( bResult )
				lStatus = 1;
		}
	}
	else
	{
		int	nBLConvID	= 0;	//v4.42T18
		BOOL bResult = TRUE;
		BOOL bCheckBL = FALSE;
		//BOOL bEndOk = (BOOL)((LONG)m_smfSRam["WaferTable"]["WaferEndSmooth"]);	//v4.46T1	//Re-enable for Genesis
		if( bBurnInApp==FALSE && bIsWaferEnd==TRUE && bEndOk )
		{
			BOOL bBinLoader = (BOOL)(LONG)(m_smfSRam)["BinLoaderStn"]["Enabled"];
		}

		if (bIsWaferEnd == TRUE && bEndOk )
		{
			LogCycleStopStatus("APP: Start Change FilmFrame");
			if( GetCustomerName()==CTM_NICHIA)
				CMS896AApp::m_bStopAlign = TRUE;	//v4.50A22	//Osram Germany

			rReqMsg.InitMessage(sizeof(BOOL), &bBurnInApp);
			int nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "App_AutoChangeFilmFrame", rReqMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID,36000000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
					rReqMsg.GetMsg(sizeof(LONG), &lStatus);
					LogCycleStopStatus("End Change FilmFrame");
					break;
				}
				else
				{
					Sleep(1);			
				}
			}
		}

		//v4.42T18		//Cree HuiZhou
		if (bCheckBL)
		{
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nBLConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nBLConvID, rReqMsg);
					rReqMsg.GetMsg(sizeof(BOOL), &bResult);
					LogCycleStopStatus("End Unload BL Frame");
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			if( bResult == FALSE )
			{
				lStatus = 0;		//ABORT AUTOBOND mode !!
				LogCycleStopStatus("Fail to auto unload bin frame to full after wafer end");
			}
		}


		if( bRegionEnd && bEndOk && pUtl->GetPrescanRegionMode() )
		{
			LogCycleStopStatus("Region Start region align wafer");

			rReqMsg.InitMessage(sizeof(BOOL), &bBurnInApp);
			int nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "AutoRegionAlignWafer", rReqMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID,36000000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
					rReqMsg.GetMsg(sizeof(LONG), &lStatus);
					break;
				}
				else
				{
					Sleep(1);			
				}
			}
		}
	}

	bHalfDone = FALSE;
	if( CMS896AStn::m_bMS90HalfSortMode && CMS896AStn::m_b2PartsAllDone==FALSE && (CMS896AStn::m_bSortGoingTo2ndPart) )
	{
		bHalfDone = TRUE;
	}

	if( bHalfDone )	// sort mode, to align 2nd part wafer
	{
		LONG lResult = FALSE;
		LogCycleStopStatus("Start 2nd part align wafer");
		int nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoAlign2PhasesSortSecondPart", rReqMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 500000)==TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
				rReqMsg.GetMsg(sizeof(LONG), &lResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		LogCycleStopStatus("Stop 2nd part align wafer");

		// -1 - error; 0 - align fail; 1 - the mode align ok; 2 - MS90 auto align.
		if( lResult>1 )
		{
			BOOL bReturn = TRUE;
			IPC_CServiceMessage stMsg;
			int nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "AutoAlignFrameWafer", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bReturn);
					break;
				}
				else
				{
					Sleep(10);
				}
			}	// full auto alignment.

			lResult = bReturn;
		}

		if( lResult==1 )
			lStatus = 1;
	}		// sort mode, to align 2nd part wafer

	BOOL bBinRealignOK = TRUE;//2019.04.11 rotate bt after rotate wt done to make sure bt and wt in same orientation when realign
	//v4.50A1	//MS90
	if (m_bMS90)
	{
		BOOL bBinLoader = (BOOL)(LONG)(m_smfSRam)["BinLoaderStn"]["Enabled"];
		if (bBinLoader)
		{
			UCHAR ucWaferRotation = 0;		//No transform

			BOOL b1stDone	= CMS896AStn::m_b2Parts1stPartDone;		//v4.59A21
			if( b1stDone )	// sort mode, to align 2nd part wafer
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("MS90: Realign bin frame for 2nd half ...");
				//HmiMessage("Realign bin frame (MS90) by 180 degree ...");
				ucWaferRotation = 2;		//180 degree transform
				CMS896AStn::m_b2Parts2ndPartStart = TRUE;	//v4.59A17
			}
			else
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("MS90: Realign bin frame for 1st half ...");
				//HmiMessage("Realign bin frame (MS90) by 0 degree ...");

				if (bIsEnableWL && (bIsWaferEnd == TRUE) && bEndOk )
					ucWaferRotation = 4;	//0 degree transform
				CMS896AStn::m_b2Parts2ndPartStart = FALSE;	//v4.59A17
			}
			
			if( (bIsEnableWL && bIsWaferEnd && bEndOk ) || (bHalfDone) )	//v4.54A3	//Only perform when 1st-half is just done and is switching to 2nd half
			{
				//v4.50A24
				//BPR RT comp offset need to perform transformation for wafer&bin rotation
				BOOL bBprResult = TRUE;
				IPC_CServiceMessage stBprMsg;
				stBprMsg.InitMessage(sizeof(UCHAR), &ucWaferRotation);
				INT nBprConvID = m_comClient.SendRequest(BOND_PR_STN, "TransformBHRTCompXY", stBprMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nBprConvID, 500000)==TRUE )
					{
						m_comClient.ReadReplyForConvID(nBprConvID, stBprMsg);
						stBprMsg.GetMsg(sizeof(BOOL), &bBprResult);
						break;
					}
					else
					{
						Sleep(10);
					}
				}

				if (!RealignBinFrame())
				{
					bBinRealignOK = FALSE;
				}
			}
		}
	}

	if (bHalfDone && CMS896AApp::m_bEnableSubBin == TRUE)
	{
		//HmiMessage("Unload");
		BOOL bReturn = TRUE;
		IPC_CServiceMessage stMsg;
		int nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "AutoUnloadFilmFrameNotToFull", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		//HmiMessage("load");
		LONG lBlk = 1;
		stMsg.InitMessage(sizeof(LONG), &lBlk);
		nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "ManualLoadFilmFrame", stMsg);
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE)
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}
	if( bDimStopBtn )
	{
		CMS896AStn::m_bWaferAlignComplete = TRUE;
		m_bIsChangeWafer = FALSE;
	}

	LogCycleStopStatus("APP: cycle stop to change wafer frame done");

	if( bBinRealignOK == FALSE )
	{
		lStatus = 0;		//ABORT AUTOBOND mode !!
	}
	return lStatus;
}


BOOL CMS896AApp::AutoStopBinLoader(BOOL bBurnIn)
{
	BOOL bBinLoader = (BOOL)(LONG)(m_smfSRam)["BinLoaderStn"]["Enabled"];
	if (!bBinLoader)
		return TRUE;
	if (!m_bUsePLLM)
		return TRUE;

	IPC_CServiceMessage rReqMsg;
	int	nConvID;
	BOOL bResult = TRUE;
	
	CMSLogFileUtility::Instance()->MS_LogOperation("Auto-Stop BinLoader (PLLM)");

	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, "HomeToReady", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 500000)==TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}


	return TRUE;
}


//Debug Log
BOOL CMS896AApp::CheckAndBackupCycleStopLog(ULONGLONG nFileSizeLimit, BOOL bStopLog)
{
	INT nCol =0;
	ULONGLONG nFileSize =0;
	CString szLogFileName;
	
	if (bStopLog)
		szLogFileName = gszUSER_DIRECTORY + "\\History\\Stopcycle.log";
	else
		szLogFileName = gszUSER_DIRECTORY + "\\History\\Startcycle.log";

	CString szBpLogFileName;
	CFile fLogFile;

	if  (fLogFile.Open(szLogFileName,CFile::modeRead) == FALSE)
	{
		return FALSE;
	}

	nFileSize = fLogFile.GetLength();
	fLogFile.Close();

	// Case of no need to backup the log file
	if (nFileSize < nFileSizeLimit )
	{
		return TRUE;
	}

	// Start backup log file
	nCol = szLogFileName.ReverseFind('.');
	if (nCol != -1)
		szBpLogFileName = szLogFileName.Left(nCol) + ".bak";
	else
		szBpLogFileName = szLogFileName + ".bak";

	if (_access(szBpLogFileName, 0) != -1)
	{
		DeleteFile(szBpLogFileName);
	}

	try
	{
		CopyFile(szLogFileName, szBpLogFileName, FALSE);
		DeleteFile(szLogFileName);
	}
	catch (CFileException e)
	{
		return FALSE;
	}
	
	return TRUE;
}

LONG CMS896AApp::LogCycleStopStatus(CString szMessage)
{
	if ( (m_bEnableMachineLog == FALSE) && (m_bBurnIn == FALSE) )	//v3.87		//Need to enable StopCycle log for BURNIN
	{
		return 1;
	}

	static LONG lCount = 0;

	FILE *fp = NULL;
	CTime theTime = CTime::GetCurrentTime();


	CSingleLock slLock(&m_csDebugLog);
	slLock.Lock();

	CString szLogFileName = gszUSER_DIRECTORY + "\\History\\Stopcycle.log";

	errno_t nErr = fopen_s(&fp, szLogFileName, "a+");
	if ((nErr == 0) && (fp != NULL))
	{
		lCount++;	

		fprintf(fp, "%d-%2d-%2d (%2d:%2d:%2d) - %2ld. %s\n",  
				theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(),
				theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(),
				lCount, (LPCTSTR) szMessage);

		if( szMessage == "MC in idle state" )
		{
			fprintf(fp, "\n");
			lCount = 0;
		}

		fclose(fp);
	}

	slLock.Unlock();

	return 1;
}


BOOL CMS896AApp::DisplaySortFileContent(LONG lTitleID, CONST CString szOldFile, CONST CString szNewFile)
{
	if (m_bDisplaySortBinItemMsg == FALSE)
	{
		return TRUE;
	}

	if (szOldFile == "")
	{
		return TRUE;
	}

	if (szOldFile == szNewFile)
	{
		return TRUE;
	}

	CString szMsg;
	szMsg = "Old: (" + szOldFile + ")\nNew: (" + szNewFile + ")\n";

	SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);		// Red

	CString szTitle;

	szTitle.LoadString(lTitleID);

	if ( AfxMessageBox(szTitle + "\n" + szMsg + "Reset name?", MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL) == IDYES )
	{
		SetAlarmLamp_Yellow(FALSE, TRUE);		// Yellow
		return TRUE;
	}

	SetAlarmLamp_Yellow(FALSE, TRUE);

	return FALSE;
}


BOOL CMS896AApp::IsApplicationRunning(CString szAppName, UCHAR ucLimit, CONST BOOL bShowMsg)
{
	HANDLE hProcessSnap = NULL; 
	PROCESSENTRY32 pe32      = {0}; 
	int nMCUXPCount = 0;
	UCHAR ucExistCount = 0;
	CString szText;
	BOOL bAppExist = FALSE;


	//Set upper case
	szAppName.MakeUpper();

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap != INVALID_HANDLE_VALUE) 
	{
		//  Fill in the size of the structure before using it. 
		pe32.dwSize = sizeof(PROCESSENTRY32); 
	 
		//  Walk the snapshot of the processes, and for each process, 
		//  display information. 

		if (Process32First(hProcessSnap, &pe32)) 
		{ 
			do 
			{ 
				CString strName = pe32.szExeFile;
				strName = strName.MakeUpper();
				if (strName.Find(szAppName) >= 0)
				{
					ucExistCount++;
				}

				if ( ucExistCount >= ucLimit )
				{
					szText = szAppName + " is already exist!\nPlease remove this before start MapSorter!";
					if( bShowMsg )
					{
						AfxMessageBox(szText, MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
					}
					bAppExist = TRUE;
					break;
				}

			} while (Process32Next(hProcessSnap, &pe32)); 
		} 
	} 
	CloseHandle (hProcessSnap); 

	return bAppExist;
}


VOID CMS896AApp::LogStatusInfo(CString szText)
{
	if ( CMS896AApp::m_bEnableMachineLog == TRUE )
	{
		//Log file
		FILE *fp = NULL;
		CString szLogFileName = gszUSER_DIRECTORY + "\\History\\MS_LOT.log";
		CTime theTime = CTime::GetCurrentTime(); 

		errno_t nErr = fopen_s(&fp, szLogFileName, "a+");
		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "%d-%2d-%2d (%2d:%2d:%2d) - %s\n", 
								theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(),
								(LPCTSTR) szText);
			fclose(fp);
		}
	}

}

BOOL CMS896AApp::InitMachineTime()
{
	//Machine Time
	for (INT i=0; i<m_lNoOfReportPeriod; i++)
	{
		m_eqMachine.SetStartRecordTime(m_szReportStartTime[i],i);
		m_eqMachine.SetEndRecordTime(m_szReportEndTime[i],i);
	}

	m_eqMachine.SetNoOfPeriod(m_lNoOfReportPeriod);
	m_eqMachine.SetReportPath(m_szMachineTimeReportPath);
	m_eqMachine.SetTimePerformancePath(m_szTimePerformancePath, m_szMachineSerialNo);
	CreateDirectory(m_szTimePerformancePath, NULL);
	CreateDirectory(m_szTimePerformancePath + "\\PF", NULL);
	//v4.15T2	//HighPowerOpto
	if( (GetCustomerName() == "HPO") || 
		(GetCustomerName() == CTM_LEXTAR)	||
		(GetCustomerName() == _T("Electech3E(DL)")) )		//v4.44A7
	{
		m_eqMachine.SetParameter("Use Machine No As Filename", 1);
	}

	if( (GetCustomerName() == CTM_SANAN) ||
		(GetCustomerName() == CTM_LEXTAR) )
	{
		m_eqMachine.SetParameter(EQUIP_PARA_GENERATE_HOURLY_REPORT, 1);		//v4.49A7	//WH SanAn	//LeoLam
	}

	m_eqMachine.SetParameter(EQUIP_PARA_WAIT_ENGINEER_TIME, m_bAWET_Enabled);
	m_eqMachine.SetParameter("Customer Name", GetCustomerName()); //v4.51A8 v4.51D5 SanAn(XA) 11
	m_eqMachine.SetReportFormat(m_ucEqTimeReportMode, TRUE);
	m_eqMachine.SetErrorCodeMapFilePath("C:\\MapSorter\\Config\\ErrorCodeInfo.txt");
	m_eqMachine.SetGroupSetupAlarmAssitTimeToIdle(m_bGroupSetupAlarmAssitTimeToIdle);
	m_eqMachine.SetDetailsReport(m_bEnableMachineTimeDetailsReport);
	m_eqMachine.SetDetailsReportFormat((UCHAR)m_lMachineTimeReportFormat);
	m_eqMachine.SetTimeReportFileExtension(m_szMachineTimeReportExt);

	BOOL bEquipmentManager = GetFeatureStatus(MS896A_FUNC_HOST_COMM_EQUIP_MANAGER);
	m_eqMachine.SetEquipmentManager(bEquipmentManager);
	if (m_eqMachine.IsEquipmentManager())
	{
		m_eqMachine.SetEMHourlyRecipe(m_szDeviceFile);
		m_eqMachine.SetEMHourlyStatisticsLogExpiredDays(10);
	}
	m_eqMachine.Initial("C:\\MapSorter\\UserData\\Statistic\\EquipTime.msd", "C:\\MapSorter\\UserData\\Statistic\\EventLog.txt");
	
	return TRUE;
}

BOOL CMS896AApp::InitMachineTime2()
{
	//Machine Time
	UINT unEnable = GetProfileInt(gszPROFILE_SETTING, _T("Equip Machine Time 2"), 0);
	if( unEnable==0 )
	{
		for (INT i=0; i<m_lNoOfReportPeriod; i++)
		{
			m_eqMachine2.SetStartRecordTime(m_szReportStartTime[i],i);
			m_eqMachine2.SetEndRecordTime(m_szReportEndTime[i],i);
		}
		m_eqMachine2.SetNoOfPeriod(m_lNoOfReportPeriod);
		m_eqMachine2.SetReportFormat(0, TRUE);	//Not Used!
	}
	else
	{
		m_eqMachine2.SetNoOfPeriod(0);
		m_eqMachine2.SetReportFormat(4, TRUE);						//Must force to use Per-Wafer Mode
	}
	m_eqMachine2.SetReportPath(m_szMachineTimeReportPath2);
	m_eqMachine2.SetTimePerformancePath(m_szTimePerformancePath, m_szMachineSerialNo);

	m_eqMachine2.SetParameter(EQUIP_PARA_WAIT_ENGINEER_TIME, m_bAWET_Enabled);
	m_eqMachine2.SetParameter("Customer Name", GetCustomerName()); //v4.51A8 v4.51D5 SanAn(XA) 12/
	m_eqMachine2.SetErrorCodeMapFilePath("C:\\MapSorter\\Config\\ErrorCodeInfo.txt");
	m_eqMachine2.SetGroupSetupAlarmAssitTimeToIdle(m_bGroupSetupAlarmAssitTimeToIdle);
	m_eqMachine2.SetDetailsReport(m_bEnableMachineTimeDetailsReport);
	m_eqMachine2.SetDetailsReportFormat((UCHAR)m_lMachineTimeReportFormat);
	m_eqMachine2.SetTimeReportFileExtension(m_szMachineTimeReportExt);
	//v4.40T8
	m_eqMachine2.Initial("C:\\MapSorter\\UserData\\Statistic\\EquipTime2.msd", "C:\\MapSorter\\UserData\\Statistic\\EventLog2.txt");
	return TRUE;
}

BOOL CMS896AApp::InitToolsUsageRecord()
{
	m_oToolsUsageRecord.SetRecordPath(m_szMachineReportPath);
	m_oToolsUsageRecord.SetRecordPath2(m_szMachineReportPath2);		//v4.48A4	//3E DL
	return TRUE;
}

BOOL CMS896AApp::ValidateReportTimeInput(CString szDate)
{
	INT nCol =0, nHour, nMin;

	nCol = szDate.Find(':');

	if (nCol == -1)
		return FALSE;

	if (nCol+1 >= szDate.GetLength())
		return FALSE;

	if ((szDate.Left(nCol).GetLength() != 2)|| (szDate.Mid(nCol+1).GetLength() != 2))
		return FALSE;

	nHour = atoi(szDate.Left(nCol));
	nMin = atoi(szDate.Mid(nCol+1));

	if ((nHour >= 24) || (nHour<0) || (nMin >=60) || (nMin<0))
		return FALSE;

	return TRUE;
}

CString CMS896AApp::GetPKGFilename()
{
	if(m_szDeviceFile.Find(".ppkg") > 0)
	{
		return m_szDeviceFile.Left(m_szDeviceFile.GetLength() - 5);
	}
	else if(m_szDeviceFile.Find(".pkg") > 0)
	{
		return m_szDeviceFile.Left(m_szDeviceFile.GetLength() - 4);
	}
	else
	{
		return m_szDeviceFile;
	}
	//return m_szDeviceFile;
}

CString CMS896AApp::GetPPKGFilenameByLotNo(CString szLotNo)
{
	CString szPPKGFileName = "";
	CString szFileFullName = gszUSER_DIRECTORY + "\\" + "PPKGNameList.txt";
	SetErrorMessage(szFileFullName);

	CStdioFile cfPPKGListFile;
	CFileException e;

	if (! cfPPKGListFile.Open(szFileFullName, CFile::modeCreate|CFile::modeRead|CFile::modeNoTruncate, &e))
	{
		SetErrorMessage("Cannot open PPKGNameList.txt");
		return _T("");
	}
	else
	{
		CString szData;
		while(cfPPKGListFile.ReadString(szData) != NULL )
		{
			if (szData.Find(szLotNo) != -1)
			{
				int nIndex = szData.Find(",");
				szPPKGFileName = szData.Mid(nIndex + 1);
				break;
			}
		}	
	}
	cfPPKGListFile.Close();
	SetErrorMessage("Return PPKG file name" + szPPKGFileName);
	return szPPKGFileName;
}

BOOL CMS896AApp::LoadPKGFile(BOOL bIsPortablePKGFile , CString szFilename, 
							 BOOL bPopupConfirmDialog, BOOL bPopupCompleteDialog, 
							 BOOL bIsAutoMode, BOOL bEnableSECSGEM)
{
	CString szFileExt = "";
	CString szTitle = "", szContent = "";
	CString szFilePath;
	CString szFileDialogFilter;
	INT nCol = -1;
	INT nReturn = 0;
	BOOL bReturn = FALSE;

	szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);

	if (bPopupConfirmDialog == TRUE)
	{
		szContent.LoadString(HMB_MS_LOAD_PKG_WARNING);
		if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO) != glHMI_YES)
		{
			return FALSE;
		}
	}

	m_bIsLoadingPKGFile = TRUE;

	m_bPortablePKGFile = bIsPortablePKGFile;

	if (bIsPortablePKGFile == FALSE)
	{
		szFilePath = m_szDevicePath;
	}
	else
	{
		szFilePath = m_szPortablePKGPath;
	}
	m_szDeviceFile = szFilename;

	if (m_bDialogDeviceFile == TRUE && bIsAutoMode == FALSE)
	{
		if (bIsPortablePKGFile == FALSE)
		{	
			static char szFilters[]=
				"Device File (*.pkg)|*.pkg|All Files (*.*)|*.*||";

			// Create an Open dialog; the default file name extension is ".pkg".
			CFileDialog dlgFile(TRUE, "pkg", "*.pkg", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
								szFilters, m_pMainWnd, 0);
			dlgFile.m_ofn.lpstrInitialDir = szFilePath;
			dlgFile.m_ofn.lpstrDefExt = "pkg";

			ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
			nReturn = (INT)dlgFile.DoModal();		// Show the file dialog box
			ShowHmi();			// Show the HMI on top

			if ( nReturn == IDOK )
			{
				m_szDeviceFile = dlgFile.GetFileName();
				szFileExt = dlgFile.GetFileExt();
				szFileExt = szFileExt.MakeUpper();
				// as a protection
				if (CheckPKGFileExtension(bIsPortablePKGFile, szFileExt) == FALSE)
				{
					szContent.LoadString(HMB_MS_FILE_EXT_NOT_SUPPORTED);
					HmiMessage(szContent, szTitle);
					bReturn = FALSE;
					m_bIsLoadingPKGFile = FALSE;
					m_bPortablePKGFile = FALSE;
					m_bOnBondMode = FALSE;
					return FALSE;
				}
			
				(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
				(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04
				
				SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
				bReturn = RestoreFromDevice(szFilePath, m_szDeviceFile/*GetPKGFilename()*/, bPopupConfirmDialog, bPopupCompleteDialog);
			}
			else
			{
				bReturn = FALSE;
			}
		}
		else
		{
			static char szFilters[]=
				"Device File (*.ppkg)|*.ppkg|All Files (*.*)|*.*||";

			// Create an Open dialog; the default file name extension is ".ppkg".
			CFileDialog dlgFile(TRUE, "ppkg", "*.ppkg", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY , 
								szFilters, m_pMainWnd, 0);
			dlgFile.m_ofn.lpstrInitialDir = szFilePath;
			dlgFile.m_ofn.lpstrDefExt = "ppkg";

			ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
			nReturn = (INT)dlgFile.DoModal();		// Show the file dialog box
			ShowHmi();			// Show the HMI on top

			if ( nReturn == IDOK )
			{				
				m_szDeviceFile = dlgFile.GetFileName();
				szFileExt = dlgFile.GetFileExt();

				if (CheckPKGFileExtension(bIsPortablePKGFile, szFileExt) == FALSE)
				{
					szContent.LoadString(HMB_MS_FILE_EXT_NOT_SUPPORTED);
					HmiMessage(szContent, szTitle);

					bReturn = FALSE;
					m_bIsLoadingPKGFile = FALSE;
					m_bPortablePKGFile = FALSE;
					m_bOnBondMode = FALSE;
					return FALSE;
				}

				(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
				(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04
				
				SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
				bReturn = RestoreFromDevice(szFilePath, m_szDeviceFile/*GetPKGFilename()*/, bPopupConfirmDialog, bPopupCompleteDialog);
			}
			else
			{
				bReturn = FALSE;
			}
		}
	}
	else
	{ 
		nCol = m_szDeviceFile.Find(".");
		
		if (nCol != -1)
		{
			szFileExt = m_szDeviceFile.Right(m_szDeviceFile.GetLength()- nCol -1);
		}

		if (CheckPKGFileExtension(bIsPortablePKGFile, szFileExt) == FALSE)
		{
			szContent.LoadString(HMB_MS_FILE_EXT_NOT_SUPPORTED);
			HmiMessage(szContent, szTitle);
			bReturn = FALSE;
			m_bIsLoadingPKGFile = FALSE;
			m_bPortablePKGFile = FALSE;
			m_bOnBondMode = FALSE;
			return FALSE;
		}
		(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
		(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04

		SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
		bReturn = RestoreFromDevice(szFilePath, m_szDeviceFile/*GetPKGFilename()*/, bPopupConfirmDialog, bPopupCompleteDialog);
	}

	m_bIsLoadingPKGFile = FALSE;
	m_bPortablePKGFile = FALSE;

	return bReturn;
}


BOOL CMS896AApp::LoadPKGFileByTraceabilityERPNo_PLSG_DieFab(CString szERPPartNo) 		//PLSG traceability
{
	BOOL bReturn			= TRUE;
	CString szDeviceFile	= _T("");
	CString szFileExt		= _T("");

	CStdioFile oFile;
	CString szLine = _T("");
	CString szCurrERPNo = _T("");
	CString szCurrPkg	= _T("");
	BOOL bERPFound = FALSE;
	BOOL bStart = FALSE;
	INT nCol = 0;

	//v4.42T6
	//BOOL bExist = oFile.Open(gszROOT_DIRECTORY + "\\Exe\\erp_part_no.txt", 
	//								CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	BOOL bExist = oFile.Open(m_szRecipeFilePath + "\\erp_part_no.txt", 
									CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	if (!bExist)
	{
		HmiMessage("LOAD PKG: ERP file (erp_part_no.txt) is not found in \\Exe sub-folder!");
		SetErrorMessage("LOAD PKG: ERP file (erp_part_no.txt) is not found in \\Exe sub-folder!");
		return FALSE;
	}

	while (oFile.ReadString(szLine))
	{
		//Search [START] of entry in file
		if (!bStart)
		{
			if (szLine.Find("[START]") == -1)
				continue;
			else
			{
				bStart = TRUE;
				continue;
			}
		}

		nCol = szLine.Find(",");
		if (nCol == -1)
			continue;
		szCurrERPNo = szLine.Left(nCol);
		szCurrPkg	= szLine.Mid(nCol + 1);

		if (szCurrERPNo == szERPPartNo)
		{
			bERPFound = TRUE;
			szDeviceFile = szCurrPkg;
			break;
		}

	}
	oFile.Close();

	if (!bERPFound)
	{
		HmiMessage("MS: PLSG ERP No = " + szERPPartNo + " is not found!");
		SetErrorMessage("MS: PLSG ERP No = " + szERPPartNo + " is not found!");
		return FALSE;
	}

//CString szMsg;
//szMsg = "PKG file found = " + szDeviceFile;
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

	//v4.04		//PLSG auto-load device file for Lumileds3 format
	if (m_szDeviceFile != szDeviceFile)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("MS: auto-load PLSG ERP DEVICE file - " + szDeviceFile);
		
		BOOL bManualLoad = TRUE;
		m_smfSRam["MS896A"]["Manual Load Pkg"]	= bManualLoad;		//v4.33T1	//PLSG

		bReturn = RestoreFromDevice(m_szDevicePath, szDeviceFile, FALSE, FALSE);
		if (bReturn)
		{
			m_szDeviceFile = szDeviceFile;
			(m_smfSRam)["MS896A"]["PKG Filename Display"] = GetPKGFilename();
			(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04
			SavePackageInfo();
		}
		else
		{
			SetErrorMessage("MS: auto-load PLSG ERP DEVICE file fails - " + szDeviceFile);
		}
	}

	return bReturn;
}


BOOL CMS896AApp::LoadPKGFileByTraceabilityERPNo_PLLM(CString szERPPartNo)
{
	BOOL bReturn			= TRUE;
	CString szDeviceFile	= _T("");
	CString szFileExt		= _T("");

	CStdioFile oFile;
	CString szLine = _T("");
	CString szCurrERPNo = _T("");
	CString szCurrPkg	= _T("");
	BOOL bERPFound = FALSE;
	BOOL bStart = FALSE;
	INT nCol = 0;

	//BOOL bExist = oFile.Open(gszROOT_DIRECTORY + "\\Exe\\RecipeTable.txt", 
	//								CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	BOOL bExist = oFile.Open(m_szRecipeFilePath + "\\RecipeTable.txt", 
									CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	if (!bExist)
	{
		HmiMessage("LOAD PKG: ERP file (RecipeTable.txt) is not found in "		+ m_szRecipeFilePath);
		SetErrorMessage("LOAD PKG: ERP file (erp_part_no.txt) is not found in " + m_szRecipeFilePath);
		return FALSE;
	}

	while (oFile.ReadString(szLine))
	{
		//Search [START] of entry in file
		if (!bStart)
		{
			if (szLine.Find("Product: ") == -1)
				continue;
			else
			{
				bStart = TRUE;
				continue;
			}
		}

		//Find new RECIPE entry
		if (szLine.Find("PnP_Recipe = ") != -1)
		{
			nCol = szLine.Find("= ");

			//Retrieve RECIPE file name on same line
			if( nCol!=-1 )
				szCurrPkg = szLine.Mid(nCol + 2);
//AfxMessageBox(szCurrPkg, MB_SYSTEMMODAL);

			//THen check for available PART numbers to see if any one is matched with current ERP number
			while (oFile.ReadString(szLine))
			{
				if (szLine.Find("{") != -1)
					continue;					//Skip OPEN bracket
				if (szLine.Find("Part Number") != -1)
					continue;					//Skip Part Number
				if (szLine.Find("}") != -1)
					break;						//If END bracket reached, that means end of current RECIPE list

				szCurrERPNo = szLine;
				szCurrERPNo.Replace("\t", "");
				szCurrERPNo.Replace(" ", "");

				if (szCurrERPNo == szERPPartNo)	//If current ERP found in this list -> FOUND!!
				{
					bERPFound = TRUE;
					break;
				}
			}
		}

		if (bERPFound == TRUE)
		{
			szDeviceFile = szCurrPkg;
			break;
		}

		bStart = FALSE;
		//if (szLine.Find("[END]") != -1)
		//	break;
	}

	oFile.Close();

	if (!bERPFound)
	{
		SetErrorMessage("ERROR: Recipe = " + szERPPartNo + " is not found on machine (PLLM StartLot)");
		HmiMessage("ERROR: Recipe = " + szERPPartNo + " is not found on machine!");
		return FALSE;
	}

CString szMsg;
szMsg = "PKG file found = " + szDeviceFile;
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

	//v4.04		//PLSG auto-load device file for Lumileds3 format
	if (m_szDeviceFile != szDeviceFile)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("MS: auto-load PLLM ERP DEVICE file - " + szDeviceFile);
		
		BOOL bManualLoad = TRUE;
		m_smfSRam["MS896A"]["Manual Load Pkg"]	= bManualLoad;		//v4.33T1	//PLSG

		bReturn = RestoreFromDevice(m_szDevicePath, szDeviceFile, FALSE, FALSE);
		if (bReturn)
		{
			m_szDeviceFile = szDeviceFile;
			(m_smfSRam)["MS896A"]["PKG Filename Display"] = GetPKGFilename();
			(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04
			SavePackageInfo();
		}
		else
		{
			SetErrorMessage("MS: auto-load PLLM ERP DEVICE file fails - " + szDeviceFile);
		}
	}

	return bReturn;
}


BOOL CMS896AApp::LoadPKGFileByTraceabilityERPNo_PLLM_Lumiramic(CString szERPPartNo)
{
	BOOL bReturn			= TRUE;
	CString szDeviceFile	= _T("");
	CString szFileExt		= _T("");

	CStdioFile oFile;
	CString szLine = _T("");
	CString szCurrERPNo = _T("");
	CString szCurrPkg	= _T("");
	BOOL bERPFound = FALSE;
	BOOL bStart = FALSE;
	INT nCol = 0;

//HmiMessage("PLLM Lumiramic Auto-Load Recipe starts ...");

	BOOL bExist = oFile.Open(gszROOT_DIRECTORY + "\\Exe\\RecipeTable.txt", 
									CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	if (!bExist)
	{
		HmiMessage("LOAD PKG: ERP file (RecipeTable.txt) is not found in \\Exe sub-folder!");
		SetErrorMessage("LOAD PKG: ERP file (erp_part_no.txt) is not found in \\Exe sub-folder!");
		return FALSE;
	}

	while (oFile.ReadString(szLine))
	{
		//Search [START] of entry in file
		if (!bStart)
		{
			if (szLine.Find("Product: ") == -1)
				continue;
			else
			{
				bStart = TRUE;
				continue;
			}
		}

		//Find new RECIPE entry
		if (szLine.Find("PnP_Recipe = ") != -1)
		{
			nCol = szLine.Find("= ");

			//Retrieve RECIPE file name on same line
			if( nCol!=-1 )
				szCurrPkg = szLine.Mid(nCol + 2);
//AfxMessageBox(szCurrPkg, MB_SYSTEMMODAL);

			//THen check for available PART numbers to see if any one is matched with current ERP number
			while (oFile.ReadString(szLine))
			{
				if (szLine.Find("{") != -1)
					continue;					//Skip OPEN bracket
				if (szLine.Find("Part Number") != -1)
					continue;					//Skip Part Number
				if (szLine.Find("}") != -1)
					break;						//If END bracket reached, that means end of current RECIPE list

				szCurrERPNo = szLine;
				szCurrERPNo.Replace("\t", "");
				szCurrERPNo.Replace(" ", "");

				if (szCurrERPNo == szERPPartNo)	//If current ERP found in this list -> FOUND!!
				{
					bERPFound = TRUE;
					break;
				}
			}
		}

		if (bERPFound == TRUE)
		{
			szDeviceFile = szCurrPkg;
			break;
		}

		bStart = FALSE;
		//if (szLine.Find("[END]") != -1)
		//	break;
	}

	oFile.Close();

	if (!bERPFound)
	{
		SetErrorMessage("ERROR: Recipe = " + szERPPartNo + " is not found on machine (PLLM StartLot)");
		HmiMessage("ERROR: Recipe = " + szERPPartNo + " is not found on machine!");
		return FALSE;
	}

CString szMsg;
szMsg = "PKG file found = " + szDeviceFile;
//AfxMessageBox(szMsg, MB_SYSTEMMODAL);

	//v4.04		//PLSG auto-load device file for Lumileds3 format
	if (m_szDeviceFile != szDeviceFile)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("MS: auto-load PLLM ERP DEVICE file - " + szDeviceFile);
		
		BOOL bManualLoad = TRUE;
		m_smfSRam["MS896A"]["Manual Load Pkg"]	= bManualLoad;		//v4.33T1	//PLSG

		bReturn = RestoreFromDevice(m_szDevicePath, szDeviceFile, FALSE, FALSE);
		if (bReturn)
		{
			m_szDeviceFile = szDeviceFile;
			(m_smfSRam)["MS896A"]["PKG Filename Display"] = GetPKGFilename();
			(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04
			SavePackageInfo();
		}
		else
		{
			SetErrorMessage("MS: auto-load PLLM ERP DEVICE file fails - " + szDeviceFile);
		}
	}

	return bReturn;
}


// Remove Backup Output File if the file exist for 30 days as default
BOOL CMS896AApp::SearchAndRemoveFiles(CString szPath, USHORT usDiffDays, BOOL bClearNow, 
									  CString szFileExtension, BOOL bSearchSubFolder)
{
	CString szFilename, szFolderPath;
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;

	//Klocwork		//v4.04
	//if( usDiffDays<0 )
	//	return FALSE;

	hSearch = FindFirstFile(szPath + "\\" + "*." + szFileExtension, &FileData);
	
	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		return FALSE;
	}

	do 
	{
		// if it is a folder recurive call to remove file
		if ((FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) && bSearchSubFolder)	//v4.46T13
		{
			szFolderPath = szPath + "\\" + FileData.cFileName;
			
			if ((szFolderPath != szPath + "\\" + ".") && (szFolderPath != szPath + "\\" + ".."))
			{
				//LogFile("Folder:" + szFolderPath);
				SearchAndRemoveFiles(szFolderPath, usDiffDays, bClearNow, szFileExtension, bSearchSubFolder);
			}

			if( szFolderPath.Find("AI Log")==-1 && szFolderPath.Find("SaveImages")==-1 )
			{
				CTime curTime = CTime::GetCurrentTime();
				CTime ctFileTime(FileData.ftCreationTime);
				CTimeSpan timeDiff = curTime - ctFileTime;
			
				// keep file for a month(certain time)
				if ((timeDiff.GetDays() > usDiffDays) || (bClearNow == TRUE))
				{
					RemoveDirectory(szFolderPath);
				}
			}
		}
		// if it is a file delete the file
		else
		{
			//LogFile("File:" + szPath + "\\" + FileData.cFileName);
			CTime curTime = CTime::GetCurrentTime();
			CTime ctFileTime(FileData.ftCreationTime);
			CTimeSpan timeDiff = curTime - ctFileTime;
		
			// keep file for a month(certain time)
			if ((timeDiff.GetDays() > usDiffDays) || (bClearNow == TRUE))
			{
				szFilename = szPath + "\\" + FileData.cFileName;
				DeleteFile(szFilename);
			}
		}

	}while (FindNextFile(hSearch, &FileData) == TRUE);
 
	// Close the search handle. 
	if ( FindClose(hSearch) == FALSE) 
	{ 
		return FALSE;
	} 

	return TRUE;
}

LONG CMS896AApp::SaveFileFormatSelectInNewLot()
{
	if( MS_SaveLastState()==FALSE )
		return FALSE;

	//v3.19T1
	//PLLM Traceability output format selection
	if (m_szFileFormatSelectInNewLot == "Lumileds (SMT)")			//SUBMOUNT
		CMS896AStn::m_lOTraceabilityFormat = 2;
	else if (m_szFileFormatSelectInNewLot == "Lumileds (LW)")		//LED
		CMS896AStn::m_lOTraceabilityFormat = 1;
	else
		CMS896AStn::m_lOTraceabilityFormat = 0;

	return TRUE;
}

LONG CMS896AApp::LoadFileFormatSelectInNewLot()
{
	if( MS_LoadLastState()==FALSE )
		return FALSE;
		//v3.19T1
		//PLLM Traceability output format selection
	if (m_szFileFormatSelectInNewLot == "Lumileds (SMT)")			//SUBMOUNT
		CMS896AStn::m_lOTraceabilityFormat = 2;
	else if (m_szFileFormatSelectInNewLot == "Lumileds (LW)")		//LED
		CMS896AStn::m_lOTraceabilityFormat = 1;
	else
		CMS896AStn::m_lOTraceabilityFormat = 0;

	return TRUE;
}

BOOL CMS896AApp::GetMachineTimes(CString& szMacUpTime, CString& szMacIdleTime, CString& szMacSetupTime, 
								 CString& szMacRunTime, CString& szMacAlarmTime, CString& szMacAssitTime,
								 CString& szMacDownTime)
{
	szMacUpTime = m_szMachineUpTime;
	szMacIdleTime = m_szMachineIdleTime;
	szMacSetupTime = m_szMachineSetupTime; 
	szMacRunTime = m_szMachineRunTime; 
	szMacAlarmTime = m_szMachineAlarmTime; 
	szMacAssitTime = m_szMachineAssistTime; 
	szMacDownTime = m_szMachineDownTime;

	return TRUE;
}

BOOL CMS896AApp::GenerateFaultRecoverAlarmFile(CString szName, CString szNameID, CString szStation)  //v4.51D2 //Electech3E(DL) 10
{

	CString szMachineNo		= m_smfSRam["MS896A"]["MachineNo"];
	CString szLog			= m_smfSRam["MS896A"]["AlarmCode"];
	//CString gszUSER_DIRECTORY  = "c:\\MapSorter\\UserData";

	CTime curTime = CTime::GetCurrentTime();
	CString szCurrentTime = curTime.Format( "%Y%m%d%H%M" );


	CStdioFile cfFile; 
	CString szPath = m_szRecoverGeneratedFilePath;
	CString szRead = "";
	if( szPath == "")
	{
		szPath = "c:\\MapSorter\\UserData\\FaultRecoverAlarm";
	}
	CreateDirectory(szPath, NULL);
	CString szPathFile = szPath + "\\" + szMachineNo + "_" + szName + "_" + szCurrentTime  + ".csv";
	if( cfFile.Open(szPathFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) == FALSE)
	{
		HmiMessage(" [GenarateFaultRecoverAlarmFile] Create/Open file Wrong!");
		SetErrorMessage(" [GenarateFaultRecoverAlarmFile] Create/Open file Wrong!");
		return FALSE;
	}
	else
	{

		cfFile.ReadString(szRead);
		
		if(szRead.Find("Date") != -1)
		{
			cfFile.SeekToEnd();
			CTime curTime = CTime::GetCurrentTime();
			CString szCurrentTime = curTime.Format( "%Y/%m/%d %H:%M" );
			cfFile.WriteString(szCurrentTime + "," + szMachineNo + ","+ szNameID  + "," + szLog + "," + szStation + ",\n" );
		}
		else
		{
			CString szHeader = "Date,Machine NO," + szName + " ID," + "Code,Station\n";
			cfFile.WriteString(szHeader); 
			cfFile.SeekToEnd();
			CString szCurrentTime = curTime.Format( "%Y/%m/%d %H:%M" );
			cfFile.WriteString(szCurrentTime + "," + szMachineNo + ","+ szNameID + "," + szLog + "," + szStation + ",\n" );
		}

		 //m_smfSRam["MS896A"]["AlarmCode"] = "";
	}

	cfFile.Close();		//v4.53A29	//Klocwork
	return TRUE;
}

VOID CMS896AApp::SetMachineTimeLoadNewMap(BOOL bLoadNewMap, CString szWaferId)
{
	// set new map is loaded
	m_eqMachine.SetLoadNewWafer(bLoadNewMap);
	// update the wafer id
	m_eqMachine.SetCurWaferId(szWaferId);
	// generate the file
	m_eqMachine.Refresh(TRUE);

	if (CMS896AApp::m_bProRataData == FALSE)
	{
	//WH Sanan	//v4.40T4
	// set new map is loaded
	m_eqMachine2.SetLoadNewWafer(bLoadNewMap);
	// update the wafer id
	m_eqMachine2.SetCurWaferId(szWaferId);
	// generate the file
	m_eqMachine2.Refresh(TRUE);
	}
}

BOOL CMS896AApp::GenerateColletUsageRecordFile(CColletRecord& oColletRecord)
{
	UCHAR ucFormatType = (UCHAR)m_lToolsUsageRecordFormat;
	return m_oToolsUsageRecord.GenerateColletUsageRecordFile(oColletRecord, ucFormatType, GetCustomerName());
}

BOOL CMS896AApp::GenerateEjectorUsageRecordFile(CEjectorRecord& oEjectorRecord)
{
	UCHAR ucFormatType = (UCHAR)m_lToolsUsageRecordFormat;
	return m_oToolsUsageRecord.GenerateEjectorUsageRecordFile(oEjectorRecord, ucFormatType, GetCustomerName());
}

VOID CMS896AApp::SetMS896ADataBlock(CMS896ADataBlock& oMS896ADataBlk)
{
	m_oToolsUsageRecord.SetMS896ADataBlock(oMS896ADataBlk);
}

VOID CMS896AApp::SetBinTableDataBlock(CBinTableDataBlock& oBinTableDataBlk)
{
	m_oToolsUsageRecord.SetBinTableDataBlock(oBinTableDataBlk);
}

VOID CMS896AApp::SetBondHeadDataBlock(CBondHeadDataBlock& oBondHeadDataBlk)
{
	m_oToolsUsageRecord.SetBondHeadDataBlock(oBondHeadDataBlk);
}

VOID CMS896AApp::SetWaferTableDataBlock(CWaferTableDataBlock& oWaferTableDataBlk)
{
	m_oToolsUsageRecord.SetWaferTableDataBlock(oWaferTableDataBlk);
}

VOID CMS896AApp::SetWaferPrDataBlock(CWaferPrDataBlock& oWaferPrDataBlk)
{
	m_oToolsUsageRecord.SetWaferPrDataBlock(oWaferPrDataBlk);
}

VOID CMS896AApp::SetBondPrDataBlock(CBondPrDataBlock& oBondPrDataBlk)
{
	m_oToolsUsageRecord.SetBondPrDataBlock(oBondPrDataBlk);
}

VOID CMS896AApp::SetBinLoaderDatatBlock(CBinLoaderDataBlock& oBinLoaderDataBlk)
{
	m_oToolsUsageRecord.SetBinLoaderDatatBlock(oBinLoaderDataBlk);
}

VOID CMS896AApp::SetSafetyDataBlock(CSafetyDataBlock& oSafetyDataBlk)
{
	m_oToolsUsageRecord.SetSafetyDataBlock(oSafetyDataBlk);
}

CString CMS896AApp::GetUsername()
{
	return m_szUsername;
}
//1.08S
CString CMS896AApp::GetPasswordFilePath()
{
	return m_szPasswordFilePath;
}

BOOL CMS896AApp::CheckIsAllBinCleared()
{
	IPC_CServiceMessage rReqMsg;
	INT nConvID = 0;
	BOOL bResult = FALSE;

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIsAllBinCleared", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bResult;
}

BOOL CMS896AApp::CheckIsAllBinClearedNoMsg()
{
	IPC_CServiceMessage rReqMsg;
	INT nConvID = 0;
	BOOL bResult = FALSE;

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIsAllBinClearedNoMsg", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bResult;
}

BOOL CMS896AApp::CheckIsAllBinClearedWithDieCountConstraint(ULONG ulDieCount)
{
	IPC_CServiceMessage rReqMsg;
	INT nConvID = 0;
	BOOL bResult = FALSE;
	
	rReqMsg.InitMessage(sizeof(ULONG), &ulDieCount);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIsAllBinClearedWithDieCountConstraint", rReqMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
			rReqMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return bResult;
}


BOOL CMS896AApp::ManualInputHmiInfo(CString *szInfo, ULONG ulAppMsgCode, CString szHmiTemp)
{
	IPC_CServiceMessage srvMsg;
	CString szTemp;
	CString szResult;
	CHAR acPar[200];
	BOOL bResult;
	BOOL bMask = FALSE;
	BOOL bReturn = TRUE;
	int nIndex = 0;

	//Assign 1 HMI variable to be used
	//szTemp = "szPLLMDieFabInfo";
	szTemp = szHmiTemp;
	strcpy_s(acPar, sizeof(acPar), (LPCTSTR)szTemp);
	nIndex += szTemp.GetLength() + 1;
	
	//Append input box title
	szTemp.LoadString(ulAppMsgCode);
	strcpy_s(&acPar[nIndex], sizeof(acPar) - nIndex, (LPCTSTR)szTemp);
	nIndex += szTemp.GetLength() + 1;

	memcpy(&acPar[nIndex], &bMask, sizeof(BOOL));
	nIndex += sizeof(BOOL);
	memcpy(&acPar[nIndex], &bReturn, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("ManualInputHmiInfo");		//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.MLog_LogDownTime(TRUE);							//anichia001

	srvMsg.InitMessage(nIndex, acPar);
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiAlphaKeys", srvMsg);

	while (!m_comClient.ScanReplyForConvID(nConvID, 10))
	{
		Sleep(10);
	}

	m_comClient.ReadReplyForConvID(nConvID, srvMsg);
	CMS896AStn::m_oNichiaSubSystem.MLog_LogDownTime(FALSE);		//anichia001

	//Get reply 
	char* pBuffer = new char[srvMsg.GetMsgLen()];
	srvMsg.GetMsg(srvMsg.GetMsgLen(), pBuffer);
	memcpy(&bResult, pBuffer, sizeof(BOOL));

	if ( bResult == TRUE )
	{
		*szInfo = &pBuffer[sizeof(BOOL)]; 
	}
	else
	{
		szInfo->IsEmpty();
	}

	delete[] pBuffer;
	return bResult;
}

BOOL CMS896AApp::HmiStrInputKeyboard(CString szTitle, CString szVarname, BOOL bPasswordMode, CString& szContent)
{
	BOOL bResult	= TRUE;
	BOOL bReturn	= TRUE;	
	BOOL bMask		= FALSE;
	if (bPasswordMode)
		bMask		= TRUE;	//Use password mode
	int nIndex		= 0;

	CString szOutput = _T("");
	char* pBuffer;	// = new char[rReqMsg2.GetMsgLen()];

	IPC_CServiceMessage rReqMsg;
	CHAR acPar[200];

	CString szTemp = szVarname;	//"szLoginPassword";	//This STRING HMI var has to be defined in HMI data list
	strcpy_s(acPar, sizeof(acPar), (LPCTSTR) szTemp);
	nIndex += szTemp.GetLength() + 1;

	//Append input box title
	szTemp = szTitle;
	strcpy_s(&acPar[nIndex], sizeof(acPar) - nIndex, (LPCTSTR) szTemp);
	nIndex += szTemp.GetLength() + 1;

	memcpy(&acPar[nIndex], &bMask, sizeof(BOOL));
	nIndex += sizeof(BOOL);
	memcpy(&acPar[nIndex], &bReturn, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("HmiStrInputKeyboard");		//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.MLog_LogDownTime(TRUE);							//anichia001

	rReqMsg.InitMessage(nIndex, acPar);			
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiAlphaKeys", rReqMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
	
			pBuffer = new char[rReqMsg.GetMsgLen()];
			rReqMsg.GetMsg(rReqMsg.GetMsgLen(), pBuffer);
			memcpy(&bResult, pBuffer, sizeof(BOOL));
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	CMS896AStn::m_oNichiaSubSystem.MLog_LogDownTime(FALSE);		//anichia001

	if ( bResult == TRUE )
	{
		//Get input string
		szContent = &pBuffer[sizeof(BOOL)]; 
	}

	delete[] pBuffer;
	return bResult;
}

BOOL CMS896AApp::HmiStrInputKeyboard_OnlyContent(CString szTitle, CString &szContent, BOOL bPasswordMode)
{
	BOOL bResult	= TRUE;
	BOOL bReturn	= TRUE;
	BOOL bMask		= FALSE;
	if (bPasswordMode)
		bMask		= TRUE;		//Use password mode		//v4.62A23

	int nIndex		= 0;

	CString szOutput = _T("");
	char *pBuffer;	// = new char[rReqMsg2.GetMsgLen()];

	IPC_CServiceMessage rReqMsg;
	CHAR acPar[200];

	CString szTemp = "szWaferIdInput";		//This STRING HMI var has to be defined in HMI data list
	strcpy(acPar, (LPCTSTR) szTemp);
	nIndex += szTemp.GetLength() + 1;

	//Append input box title
	szTemp = szTitle;
	strcpy(&acPar[nIndex], (LPCTSTR) szTemp);
	nIndex += szTemp.GetLength() + 1;

	memcpy(&acPar[nIndex], &bMask, sizeof(BOOL));
	nIndex += sizeof(BOOL);
	memcpy(&acPar[nIndex], &bReturn, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);			//nichia001

	rReqMsg.InitMessage(nIndex, acPar);				
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiAlphaKeys", rReqMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID, rReqMsg);
	
			pBuffer = new char[rReqMsg.GetMsgLen()];
			rReqMsg.GetMsg(rReqMsg.GetMsgLen(), pBuffer);
			memcpy(&bResult, pBuffer, sizeof(BOOL));
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);		//anichia001

	if (bResult == TRUE)
	{
		//Get input string
		szContent = &pBuffer[sizeof(BOOL)]; 
	}

	delete[] pBuffer;
	return bResult;
}



BOOL CMS896AApp::UpdateHmiVariableData(CString szVariableName, CString szValue)
{
	INT nIndex = 0;
	IPC_CServiceMessage svMsg;
	unsigned char acBuffer[200];

	memcpy(acBuffer, (LPCTSTR)szVariableName, szVariableName.GetLength()+1);
	nIndex += szVariableName.GetLength()+1;
	memcpy(&acBuffer[nIndex], (LPCTSTR)szValue, szValue.GetLength()+1);
	nIndex += szValue.GetLength()+1;
	svMsg.InitMessage(nIndex, acBuffer);
	m_comClient.SendRequest("HmiUserService", "HmiUpdateData", svMsg, 0);

	return TRUE;
}

//for Lumileds Singapore
LONG CMS896AApp::ReadLabelConfig()
{
	CStdioFile cfLabelConfig;
	CString szData, szLabelFormatSelection;

	if (cfLabelConfig.Open("C:\\MapSorter\\UserData\\OutputFile\\LabelConfig.txt",CFile::modeRead) == FALSE)
		return FALSE;

	cfLabelConfig.SeekToBegin();

	// Find the Flag [Label Type]
	while(cfLabelConfig.ReadString(szData) == TRUE)
	{
		INT nCol = szData.Find('=');
		
		if (nCol != -1)
		{
			if (szData.Left(nCol) == "[Label Format]")
			{
				szLabelFormatSelection = szData.Mid(nCol+1);
				break;
			}
		}

	}

	if (szLabelFormatSelection == "Die Fabrication")
	{
		//Singapore Die Fab
		return 1;
	}

	return 0;
}

BOOL CMS896AApp::MS_LoadLastState()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	if (pUtl->LoadLastState() == FALSE) 		// open Last State file
	{
		return FALSE;
	}

	psmf = pUtl->GetLastStateFile();	// get file pointer

	if ( psmf == NULL )
	{
		return FALSE;
	}

	m_szFileFormatSelectInNewLot = (*psmf)["New Lot"]["Output Format In New Lot"];
	m_szMultiLanguage = (*psmf)[MS896A_FUNC_GENERAL][MS896A_LANGUAGE];

	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPEC]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SPEC];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINPARA]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BINPARA];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TYPE]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_TYPE];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBIN]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTBIN];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFER]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WAFER];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_DEVICE];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFERBIN]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WAFERBIN];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODELNO]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_MODELNO];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LOTID];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PRODUCT]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PRODUCT];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LOTNUMBER];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD_VOLTAGE] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_ESD_VOLTAGE];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_MACHINE_NO]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PROBER_MACHINE_NO];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_DATE_TIME]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PROBER_DATE_TIME];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_TABLE_SERIAL_NO]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BIN_TABLE_SERIAL_NO];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_INTERNAL_PRODUCT_NO]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_INTERNAL_PRODUCT_NO];	
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODE]					= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_MODE];					
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPECIFICATION]			= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SPECIFICATION];			
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBINFNAME]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SORTBINFNAME];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINTABLE]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BINTABLE];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK1]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_REMARK1];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK2]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_REMARK2];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TEST_TIME] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_TEST_TIME];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WO] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WO];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SUBSTRATE_ID] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SUBSTRATE_ID];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CST_ID] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CST_ID];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_ID] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RECIPE_ID];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_NAME] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RECIPE_NAME];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_OPERATOR] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_OPERATOR];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PART_NO] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PART_NO];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_SERIAL_NUMBER] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_SERIAL_NUMBER];	

	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DN]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_DN];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WN]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WN];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM1]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CM1];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM2]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CM2];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM3]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CM3];

	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_EPIID]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_EPIID];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RESORTINGBIN] = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RESORTINGBIN];
	
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK3]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_REMARK3];

	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UN]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_UN];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COND]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COND];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LO]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LO];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UP]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_UP];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_FT]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_FT];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIM]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LIM];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF1]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COEF1];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF2]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COEF2];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF3]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_COEF3];

	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CENTERPOINT]= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CENTERPOINT];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIFETIME]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_LIFETIME];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_ESD] ;
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PAD_COLOR]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_PAD_COLOR];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_CODE]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BIN_CODE];

	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CONTROL_RANK]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_CONTROL_RANK];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RANK_NO]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_RANK_NO];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_AT]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_AT];

	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_DATE_IN_MAP_FILE]		= (*psmf)[WT_MAP_HEADER_INFO][WT_DATE_IN_MAP_FILE];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MAP_OTHER]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_MAP_OTHER];

	//v4.13T1	//DengCheng -> APlusEpi
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFTYPE]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_WAFTYPE];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINNAME]	= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_BINNAME];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U1]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_U1];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U2]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_U2];
	m_smfSRam["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U3]		= (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_U3];

	//v4.44		//SeoulSemi
	m_smfSRam["MS896A"][WT_MAP_HEADER]["Project"]				= (*psmf)[WT_MAP_HEADER_INFO]["Project"];
	m_smfSRam["MS896A"][WT_MAP_HEADER]["Product Code"]			= (*psmf)[WT_MAP_HEADER_INFO]["Product Code"];

	m_szBinSpecVersion = (*psmf)[WT_MAP_HEADER_INFO][WT_MAP_HEADER_SPEC];

	pUtl->CloseLastState();

	return TRUE;
}

BOOL CMS896AApp::MS_SaveLastState()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	if (pUtl->LoadLastState() == FALSE)
	{
		return FALSE;
	}

	psmf = pUtl->GetLastStateFile();	// get file pointer
	if ( psmf == NULL )
	{
		return FALSE;
	}

	(*psmf)["New Lot"]["Output Format In New Lot"] = m_szFileFormatSelectInNewLot;
	(*psmf)[MS896A_FUNC_GENERAL][MS896A_LANGUAGE] = m_szMultiLanguage;

	pUtl->UpdateLastState();
	pUtl->CloseLastState();

	return TRUE;
}

VOID CMS896AApp::SaveStopBondTsToRegistry(const BOOL bBHZ1)
{
	CTime theTime = CTime::GetCurrentTime();

	INT nYear	= theTime.GetYear();
	INT nMonth	= theTime.GetMonth();
	INT nDay	= theTime.GetDay();
	INT nHour	= theTime.GetHour();
	INT nMin	= theTime.GetMinute();

	if (bBHZ1)
	{
		WriteProfileInt(_T("PostBond"), _T("PB RT Year"),	nYear);
		WriteProfileInt(_T("PostBond"), _T("PB RT Month"),	nMonth);
		WriteProfileInt(_T("PostBond"), _T("PB RT Day"),	nDay);
		WriteProfileInt(_T("PostBond"), _T("PB RT Hour"),	nHour);
		WriteProfileInt(_T("PostBond"), _T("PB RT Min"),	nMin);
	}
	else
	{
		WriteProfileInt(_T("PostBond"), _T("Arm2 PB RT Year"),	nYear);
		WriteProfileInt(_T("PostBond"), _T("Arm2 PB RT Month"),	nMonth);
		WriteProfileInt(_T("PostBond"), _T("Arm2 PB RT Day"),	nDay);
		WriteProfileInt(_T("PostBond"), _T("Arm2 PB RT Hour"),	nHour);
		WriteProfileInt(_T("PostBond"), _T("Arm2 PB RT Min"),	nMin);
	}
}

INT CMS896AApp::CheckRegTsForPostBond(const BOOL bBHZ1)
{
	INT nYear	= GetProfileInt(_T("PostBond"), _T("PB RT Year"),	0);
	INT nMonth	= GetProfileInt(_T("PostBond"), _T("PB RT Month"),	0);
	INT nDay	= GetProfileInt(_T("PostBond"), _T("PB RT Day"),	0);
	INT nHour	= GetProfileInt(_T("PostBond"), _T("PB RT Hour"),	0);
	INT nMin	= GetProfileInt(_T("PostBond"), _T("PB RT Min"),	0);
	
	if (!bBHZ1)
	{
		nYear	= GetProfileInt(_T("PostBond"), _T("Arm2 PB RT Year"),	0);
		nMonth	= GetProfileInt(_T("PostBond"), _T("Arm2 PB RT Month"),	0);
		nDay	= GetProfileInt(_T("PostBond"), _T("Arm2 PB RT Day"),	0);
		nHour	= GetProfileInt(_T("PostBond"), _T("Arm2 PB RT Hour"),	0);
		nMin	= GetProfileInt(_T("PostBond"), _T("Arm2 PB RT Min"),	0);
	}


	if ((nYear == 0) || (nMonth == 0))
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("MS: invalid TS!");	
		return 0;		// Invalid registry data ->do not keep RT PB data!
	}

	CTime oldTime = CTime::CTime(nYear, nMonth, nDay, nHour, nMin, 0);
	CTime theTime = CTime::GetCurrentTime();
	CTimeSpan ts = theTime - oldTime;

//CString szTemp;
//szTemp.Format("MS: CheckRegTsForPostBond  m = %d;  h = %d", ts.GetTotalMinutes(), ts.GetTotalHours());
//CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);	

	if (ts.GetTotalMinutes() < 24*60)	//leo TJ //Finisar change to <24hours not reset
	{
//CMSLogFileUtility::Instance()->MS_LogOperation("MS: CheckRegTsForPostBond return 2");	
		return 2;	//Use FULL PB RT data
	}
	else if (ts.GetTotalMinutes() < 24*60)	//leo TJ
	{
//CMSLogFileUtility::Instance()->MS_LogOperation("MS: CheckRegTsForPostBond return 1");	
		return 1;	//Use 1/2 PB RT data
	}

	return 0;	//Do not keep/use old RT PB data by default
}

BOOL CMS896AApp::CheckDiskSize(LPCSTR lpDiskName, ULONG &lDiskSpaceUsed, ULONG &lDiskSpaceRemain)
{
	CString szMsg;
	ULARGE_INTEGER	FreeForCaller,	// Free bytes available to caller
					TotalBytes,		// Total number of bytes
					FreeBytes;		// Total number of free bytes


	DOUBLE dStartTime = GetTime();
	BOOL bReturn = GetDiskFreeSpaceEx(lpDiskName, &FreeForCaller, &TotalBytes, &FreeBytes);

	lDiskSpaceRemain = 0;
	ULONG lDiskSpaceTotal = 0, lDiskSpaceFree4Caller = 0;
	if (bReturn)
	{
		lDiskSpaceTotal			= (ULONG) ((ULONGLONG) TotalBytes.QuadPart / (1024 * 1024));
		lDiskSpaceFree4Caller	= (ULONG) ((ULONGLONG) FreeForCaller.QuadPart / (1024 * 1024));
		lDiskSpaceRemain		= (ULONG) ((ULONGLONG) FreeBytes.QuadPart / (1024 * 1024));
	}

	lDiskSpaceUsed = lDiskSpaceTotal - lDiskSpaceRemain;
	
	if (!bReturn)
	{
		szMsg.Format("C: Harddisk free space check fail.");
		HmiMessage(szMsg, "C: checking");
		return FALSE;
	}
	
	if ( lDiskSpaceRemain < 512 )
	{
		szMsg.Format("C: Harddisk free space is %luMB.", lDiskSpaceRemain);
		if ( lDiskSpaceRemain < 200 )
		{
			szMsg += "\nPlease clean up at once!";
		}

		LONG lUsedTime = (LONG)(GetTime() - dStartTime);
		CString szTest;
		szTest.Format("Used Time %d FreeBytes:%lu, used %lu", lUsedTime, lDiskSpaceRemain, lDiskSpaceUsed);
		SetErrorMessage(szTest);
		HmiMessage(szMsg, "C: checking");
		bReturn = FALSE;
	}

	return bReturn; 
}


BOOL CMS896AApp::RestorePrDataFromDevice(CString szDeviceFile, BOOL bLoadWPR, BOOL bLoadWFT)
{
	int nConvID = 0;
	IPC_CServiceMessage svMsg;
	CString szTitle;
	CString szMsg, szFullPath;
	BOOL bReturn = FALSE;
	FILE *pFile = NULL;

	szFullPath = m_szDevicePath + "\\" + szDeviceFile + "";
	// Check whether the file is existing
	errno_t nErr = fopen_s(&pFile, szFullPath, "r");
	if ((nErr != 0) || (pFile == NULL))
	{
		szFullPath = m_szDevicePath + "\\" + szDeviceFile + ".pkg";
		errno_t nErr = fopen_s(&pFile, szFullPath, "r");
		if ((nErr != 0) || (pFile == NULL))
		{
			szMsg.LoadString(HMB_MS_FILE_NOT_EXIST);
			HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			szMsg = "DEVICE file " + szDeviceFile + " cannot be opened for reading.";
			SetErrorMessage(szMsg);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			return FALSE;
		}
	}
	fclose(pFile);

	m_szDeviceFile = szDeviceFile;
	(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
	(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04
	szMsg = "begin load wpr & wft only devcie file:" + szFullPath;
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	OpenWaitAlert();		// Show the waiting message
	if ( CreateDirectory(gszPR_RECORD_PATH, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			CloseWaitAlert();		// Close the waiting message
			szMsg.LoadString(HMB_MS_CREATE_DRIECTORY_FAILED);
			HmiMessage(szMsg, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			return FALSE;
		}
	}

	// Set the record path
	(m_smfSRam)["PR"]["RecordPath"] = gszPR_RECORD_PATH;
	CMSLogFileUtility::Instance()->WL_LogStatus("delete PR record files.");
	// Delete the WaferPR Records In the Folder First		
	DeletePrRecordFiles(TRUE);
	DeletePrRecordFiles(FALSE);

	(m_smfSRam)["WPR"]["Package File Full Path"] = szFullPath;
	if( IsRuntime2PkgInUse()==FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("get waferpr msd, wafer table msd and PR record files begin.");
		bReturn = m_DeviceFile.RestorePrData(szFullPath, bLoadWPR, bLoadWFT);
		CMSLogFileUtility::Instance()->WL_LogStatus("get waferpr msd, wafer table msd and PR record files done.");

		if ( bReturn != TRUE )
		{
			CloseWaitAlert();		// Close the waiting message
			szMsg = szFullPath + " Package file is not restored!";
			HmiMessage(szMsg);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			return FALSE;
		}
	}
	else
	{
		bReturn = TRUE;
	}

	m_eqMachine.SetPackage (szDeviceFile);
	m_eqMachine2.SetPackage(szDeviceFile);		//WH Sanan	//v4.40T4

	// Download PR Records
	CMSLogFileUtility::Instance()->WL_LogStatus("Download the WaferPR Records.");
	svMsg.InitMessage(sizeof(BOOL), &bLoadWFT);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "RunTimeDownloadPrRecords", svMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			svMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	CloseWaitAlert();		// Close the waiting message

	if ( bReturn != TRUE )
	{
		szMsg = "down load pr record fail";
		HmiMessage(szMsg);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		return FALSE;
	}

	SavePackageInfo();		// Save package info to String Map file

	// load wafer pr and wafer table data if in need.
	if( bLoadWFT )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("to load wafer table msd file");
		for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
		{
			CString	szName;
			SFM_CStation* pStation;
			CMS896AStn* pStn;
			m_pStationMap.GetNextAssoc(pos, szName, pStation);
			pStn = dynamic_cast<CMS896AStn*> (pStation);
			if (szName == WAFER_TABLE_STN)
			{
				CWaferTable *pWft;
				pWft = dynamic_cast<CWaferTable*> (pStn);
				pWft->LoadWftData();
			}
		}
	}

	return TRUE;
}

LONG CMS896AApp::LoadBinSummaryFile(CString szFilename)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT nMsgLength;
	char* pFilename;

	nMsgLength = (szFilename.GetLength() + 1) * sizeof(CHAR);
	pFilename = new char[nMsgLength];
	strcpy_s(pFilename, nMsgLength, szFilename);
	
	stMsg.InitMessage(nMsgLength, pFilename);

	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "AutoLoadBinLotSummaryFile", stMsg);
	
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	delete[] pFilename;
	return bReturn;
}

BOOL CMS896AApp::ReadAmiFile(CAmiFileDecode& oAmiFileDecoder, CString szFilename)
{
	CString szStr, szTitle;

	if (oAmiFileDecoder.ReadCsvFormat(szFilename) == FALSE)
	{
		szStr.LoadString(HMB_MS_LOAD_AMI_FILE_FAIL);
		szTitle.LoadString(HMB_MS_AMI_FILE);
		HmiMessage(szStr, szTitle);
		return FALSE;
	}

	return TRUE;
}

BOOL CMS896AApp::AmiFileChecking(CAmiFileDecode& oAmiFileDecoder)
{
	BOOL bReturn = TRUE;
	LONG lMaxDieCount, lTotalBinNo;
	DOUBLE dDiePitchX, dDiePitchY;

	dDiePitchX = (DOUBLE)(LONG)m_smfSRam["MS896A"]["AmiFile"]["DiePitchX"];
	dDiePitchY = (DOUBLE)(LONG)m_smfSRam["MS896A"]["AmiFile"]["DiePitchY"];
	lMaxDieCount = (LONG)m_smfSRam["MS896A"]["AmiFile"]["MaxCount"];
	lTotalBinNo	 = (LONG)m_smfSRam["MS896A"]["AmiFile"]["TotalBinNo"];

	if (atoi(oAmiFileDecoder.m_szDiePitchX) != _round(dDiePitchX))
	{
		//v4.43T1
		SetErrorMessage("AmiFileChecking fails in m_szDiePitchX = " + oAmiFileDecoder.m_szDiePitchX);
		bReturn = FALSE;
	}

	if (atoi(oAmiFileDecoder.m_szDiePitchY) !=  _round(dDiePitchY))
	{
		//v4.43T1
		SetErrorMessage("AmiFileChecking fails in m_szDiePitchY = " + oAmiFileDecoder.m_szDiePitchY);
		bReturn = FALSE;
	}

	if (atoi(oAmiFileDecoder.m_szMaxCount) != lMaxDieCount)
	{
		//v4.43T1
		SetErrorMessage("AmiFileChecking fails in m_szMaxCount = "	+ oAmiFileDecoder.m_szMaxCount);
		bReturn = FALSE;
	}

	if (atoi(oAmiFileDecoder.m_szBinNo) != lTotalBinNo)
	{
		//v4.43T1
		SetErrorMessage("AmiFileChecking fails in m_szBinNo = "		+ oAmiFileDecoder.m_szBinNo);
		bReturn = FALSE;
	}

	if (bReturn == FALSE)
	{
		CString szStr, szTitle;
		szStr.LoadString(HMB_MS_AMI_CONTENT_CHECK_FAIL);
		szTitle.LoadString(HMB_MS_AMI_FILE);
		HmiMessage(szStr, szTitle);
	}
	
	return bReturn;
}


BOOL CMS896AApp::MachineWarmStart(CONST BOOL bLoadMSD, BOOL bNoResetNVRAM)
{
	HmiControlAccessMode();
	if( !m_bFirstExecute && bLoadMSD==FALSE )
	{
		//v4.46T20
		BOOL bPLSG = FALSE;
		if (GetCustomerName() == _T("Lumileds"))
		{
			CString szPLine = GetProductLine();
			if (szPLine == "DieFab")
				bPLSG = TRUE;
		}

		//	registry, warm log in, no alignment again.
		UINT unWarmLogin = GetProfileInt(gszPROFILE_SETTING, _T("Warm Re-log in no need align"), 0);

		if( (GetCustomerName() == _T("SanAn"))			|| 
			(GetCustomerName() == _T("Genesis"))		|| 
			(GetCustomerName() == CTM_CREE)				||		//v4.48A33
			(GetCustomerName() == _T("NSS"))			|| 
			(GetCustomerName() == _T("TongFang"))		||
			(GetCustomerName() == _T("EpiCrystal"))		|| 
			(GetCustomerName() == _T("XinGuangLian"))	||		//v4.28T5
			(GetCustomerName() == _T("AoYang"))			||		//v4.46T23	
			(GetCustomerName() == _T("Electech3E(DL)")) ||		//v4.48A2
			(GetCustomerName() == _T("Electech3E(DL)")) ||
			(GetCustomerName() == _T("Primaopto"))		||
			(GetCustomerName() == _T("ChangeLight(XM)")) ||
			(unWarmLogin == 1)							||
			bPLSG )												//v4.46T20
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("WARM-START (re-LOG-IN) completed.");
			SetStatusMessage("Machine is logged in but not reset align status");
			return TRUE;
		}
	}

	if (m_bIsMSLicenseKeyChecked)	//v4.48A2
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("LDF warm start, check License key");
		ValidateMSLicenseKeyFeature();
	}

	StartLoadingAlert();		// Show the waiting message
	MachineStarted();			// Do machine just started action

	CMSLogFileUtility::Instance()->MS_LogOperation("LDF load station msd file");
	if( CMS896AStn::m_bDisableBH && CMS896AStn::m_bDisableBT && CMS896AStn::m_bDisableBL )
		m_bEnablePostBond = FALSE;
	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		CString	szName;
		SFM_CStation* pStation;
		CMS896AStn* pStn;
		m_pStationMap.GetNextAssoc(pos, szName, pStation);
		pStn = dynamic_cast<CMS896AStn*> (pStation);

		if ( pStn->InitData() == FALSE )
		{
			CloseLoadingAlert();		// Close the waiting message
			HmiMessage("Fail to initiate data. Can't start: " + szName);
			return FALSE;
		}
	}

	CloseLoadingAlert();		// Close the waiting message
	CMSLogFileUtility::Instance()->MS_LogOperation("LDF save App data and switch language.");
	IPC_CServiceMessage svMsg;
	//Update Registry after MSD files are loaded
	SaveMapFilePath(svMsg);
	SaveMapFileExt(svMsg);
	SaveOutputFilePath(svMsg);
	SaveOutputFileFormat(svMsg);
	SaveWaferEndFilePath(svMsg);
	SaveWaferEndFileFormat(svMsg);
	SaveMachineNo(svMsg);
	LoadSelectedLanguage();
	MultiLangSelections(svMsg);

	//Restore NV RunTime data
	IPC_CServiceMessage rReqMsg;
	int nConvID = 0;
	if (bNoResetNVRAM == FALSE)		//v4.60A11
	{
		BOOL bReturn = FALSE;
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "RestoreNVRunTimeData", rReqMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID,5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,rReqMsg);
				rReqMsg.GetMsg(sizeof(BOOL), &bReturn);
				if ( bReturn == TRUE )
				{
					DisplayMessage("NVRAM data is restored");
					SetStatusMessage("NVRAM data is restored");
				}
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}

	CMSLogFileUtility::Instance()->MS_BackupLogOperation();				//v3.54T4
	CMSLogFileUtility::Instance()->MS_BackupMDOperation();				//v4.52A10
	CMSLogFileUtility::Instance()->MS_BackupLogCtmOperation();			//v4.42T11
	CMSFileUtility::Instance()->CheckAndBackupLogFileError();			//v4.51A14	//Cree HZ
	CheckAndBackupErrLogFile();											//v4.46T15

	m_bFirstExecute = FALSE;
	SetStatusMessage("S/W Version " + m_szSoftVersion);
	SetStatusMessage("Machine is warm started");

	CMSLogFileUtility::Instance()->MS_BackupSECSGEMLog();				//v4.11
	CMSLogFileUtility::Instance()->MS_BackupNVRAMHistory();				//v4.22T2
	CMSLogFileUtility::Instance()->SetCustomerName(GetCustomerName());
	CString szMsg;
	CMESConnector::Instance()->UpdateMachineStatus(IDLE_Q, szMsg);

	CMSLogFileUtility::Instance()->MS_LogOperation("WARM-START completed.");
	CMSLogFileUtility::Instance()->MS_LogMDOperation("WARM-START completed");	//v4.52A10
	return TRUE;
}

BOOL CMS896AApp::IsRunTimeLoadPKGFile()	// to load packgae file after scan barcode and a new frame(map) loaded
{
	return	m_bLoadPkgForNewWaferFrame;
}

UINT CMS896AApp::GetEnableBMapBondAreaFromRegistry()
{
	CStdioFile oFile;
	BOOL bStatus = oFile.Open(_T("c:\\MapSorter\\Exe\\binmap_config.dat"), 
								CFile::modeRead|CFile::shareExclusive|CFile::typeText);
	if (!bStatus)
		return 0;	//none
	if (GetCustomerName() != "SanAn")
		return 0;

	CString szLine;
	oFile.ReadString(szLine);
	oFile.Close();

	UINT nOption = atoi((LPCTSTR) szLine);
	if (nOption > 2)
		return 0;	//none
	return nOption;
}

VOID CMS896AApp::SetEnableBMapBondAreaToRegistry(UINT nOption)
{
	if (nOption > 2)
		return;
	if (GetCustomerName() != "SanAn")
		return;

	CStdioFile oFile;
	BOOL bStatus = oFile.Open(_T("c:\\MapSorter\\Exe\\binmap_config.dat"), 
		CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText);
	if (bStatus)
	{
		CString szLine;
		szLine.Format("%d", nOption);
		oFile.WriteString(szLine);
		oFile.Close();
	}
	return;
}

LONG CMS896AApp::GetNoOfTimeReportPeriod()
{
	return m_lNoOfReportPeriod;
}

LONG CMS896AApp::CopyPerformanceToServer(CString szSrcFile, CString szFileName)
{
	if (_access(m_szTimePerformancePath,0) != -1)
	{
		if( m_szTimePerformancePath.IsEmpty()==FALSE && m_szMachineSerialNo.IsEmpty()==FALSE )
		{
			//CreateDirectory(m_szTimePerformancePath, NULL);
			//CreateDirectory(m_szTimePerformancePath + "\\PF", NULL);
			CString szNewPath = m_szTimePerformancePath + "\\PF\\" + m_szMachineSerialNo + "_" + szFileName;
			if( szSrcFile.CompareNoCase(szNewPath)!=0 )
			{
				CopyFile(szSrcFile, szNewPath, FALSE);
			}
		}
	}

	return 1;
}	//	copy performance file to server

LONG CMS896AApp::GetReportPeriodStartTime(LONG lIndex, INT &nHour, INT &nMinute)	// time in HHMM
{
	LONG lTimeIndex = min(lIndex, EQUIP_NO_OF_RECORD_TIME-1);
	CString szDate = m_szReportStartTime[lTimeIndex];

	INT nCol = szDate.Find(':');

	nHour = 0;
	nMinute = 0;
	if (nCol == -1)
	{
		return 0;
	}

	if (nCol+1 >= szDate.GetLength())
	{
		return 0;
	}

	if ((szDate.Left(nCol).GetLength() != 2)|| (szDate.Mid(nCol+1).GetLength() != 2))
	{
		return 0;
	}

	nHour = atoi(szDate.Left(nCol));
	nMinute = atoi(szDate.Mid(nCol+1));

	nHour = min(nHour, 24);
	nMinute = min(nMinute, 59);

	return nHour*60 + nMinute;
}

BOOL CMS896AApp::EnableScreenButton(BOOL bTest)  // 4.51D1 secs
{
	if(bTest == TRUE)
	{
		m_bCycleFKey = TRUE;
		m_bCycleStarted = FALSE;
		m_bCycleNotStarted = TRUE;
		m_bShortCutKey = TRUE;
		CMS896AStn::m_bWaferAlignComplete = TRUE; //For Enable Stop Button
	}
	else
	{
		//Disable screen button
		m_bCycleNotStarted = FALSE;
		m_bCycleStarted = TRUE;
		m_bCycleFKey = FALSE;
		m_bShortCutKey = FALSE;
		CMS896AStn::m_bWaferAlignComplete = FALSE;	//For Disable Stop Button
	}
	return	TRUE;
}

BOOL CMS896AApp::DeletePrRecordFiles(CONST BOOL bWaferPr)
{
	CString szTemp, szPath;
	szPath = gszPR_RECORD_PATH;
	if( bWaferPr==TRUE )
	{
		// Delete the uploaded records
		for (INT i = 0; i <= WPR_MAX_DIE; i++)
		{
			szTemp.Format("%s\\WPR%d.rec", szPath, i);
			DeleteFile(szTemp);
		}
	}
	else
	{
		// Delete the uploaded records
		for (INT i=0; i <= BPR_MAX_DIE; i++ )
		{
			szTemp.Format("%s\\BPR%d.rec", szPath, i);
			DeleteFile(szTemp);
		}
		szTemp.Format("%s\\PEI0.rec", szPath);
		DeleteFile(szTemp);
	}

	return TRUE;
}

BOOL CMS896AApp::IsMapDetectSkipMode()
{
	if( (m_lSetDetectSkipMode > 0) )
	{
		return TRUE;
	}

	return FALSE;
}

//	Create a sub folder to contain the key parameter check list value when save PKG.
BOOL CMS896AApp::SteerPkgKeyParameters(LONG lAction)	//	Pkg Key Parameters check
{
	if( !(m_bCheckPKGKeyParameters) )
	{
		return TRUE;
	}

	CString szDevicePath = m_szDevicePath;
	if (m_ucDeviceFileType != MS_PACKAGE_FILE_TYPE)
	{
		szDevicePath = m_szPortablePKGPath;
	}

	CString	szCheckFullPath = szDevicePath + "\\KeyParamPath";
	CreateDirectory(szCheckFullPath, NULL);//	Create sub folder for the check list file
	szCheckFullPath = szCheckFullPath + "\\" + GetPKGFilename();

	//	Use a fixed filename to generate list in local HD, copy to target full path.
	DeleteFile(gszLocalPkgCheckListFile);
	CStdioFile kFile;
	if( lAction==2 )	//	server compare with memory
	{
		CopyFile(szCheckFullPath, gszLocalPkgCheckListFile, FALSE);
		if( kFile.Open(gszLocalPkgCheckListFile, CFile::modeRead|CFile::typeText) )
		{
			CString szTemp;
			while( kFile.ReadString(szTemp) )
			{
				if( szTemp.Find("App list begin")!=-1 )
				{
					break;
				}
			}
			BOOL bReturn = TRUE;
			if( kFile.ReadString(szTemp) )
			{
				if( szTemp!=m_szLotNumber )
				{
					bReturn = FALSE;
				}
			}
			else
			{
				bReturn = FALSE;
				szTemp = "";
			}
			kFile.Close();
			if( bReturn==FALSE )
			{
				CString szMsg;
				szMsg.Format("Lot Number changed to %s(%s)", szTemp, m_szLotNumber);
				SetErrorMessage(szMsg);
				SetAlarmLamp_Red(m_bEnableAlarmLampBlink, TRUE);
				HmiMessage(szMsg, "App Parameter Check");
				SetAlarmLamp_Yellow(FALSE, TRUE);
				return FALSE;
			}
		}
	}
	else
	{
		if( kFile.Open(gszLocalPkgCheckListFile, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText) )
		{
			kFile.WriteString("App list begin\n");
			kFile.WriteString(m_szLotNumber + "\n");
			kFile.Close();
		}
	}

	BOOL bReturn = TRUE;
	INT nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(LONG), &lAction);
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN,	"PkgKeyParametersTask", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if( bReturn==FALSE )
	{
		return FALSE;
	}

	stMsg.InitMessage(sizeof(LONG), &lAction);
	nConvID = m_comClient.SendRequest(WAFER_PR_STN,		"PkgKeyParametersTask", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if( bReturn==FALSE )
	{
		return FALSE;
	}

	stMsg.InitMessage(sizeof(LONG), &lAction);
	nConvID = m_comClient.SendRequest(BOND_PR_STN,		"PkgKeyParametersTask", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if( bReturn==FALSE )
	{
		return FALSE;
	}

	stMsg.InitMessage(sizeof(LONG), &lAction);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN,	"PkgKeyParametersTask", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}
	if( bReturn==FALSE )
	{
		return FALSE;
	}

	if( lAction!=2 )
	{
		CopyFile(gszLocalPkgCheckListFile, szCheckFullPath, FALSE);
	}

	return TRUE;
}

//	in prestart cycle, if exist in server, check value with current machine
//	if not, load current PKG and generate it, upload to server, then execute above checking.
BOOL CMS896AApp::CheckPkgKeyParameters()	//	Pkg Key Parameters check
{
	if( !(m_bCheckPKGKeyParameters) )
	{
		return TRUE;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("begin check pkg key parameters");
	CString szDevicePath = m_szDevicePath;
	if (m_ucDeviceFileType != MS_PACKAGE_FILE_TYPE)
	{
		szDevicePath = m_szPortablePKGPath;
	}

	CString	szCheckFullPath = szDevicePath + "\\KeyParamPath\\" + GetPKGFilename();
	if (_access(szCheckFullPath, 0)==-1 )
	{
		CString szMsg;
		//	To obtain target msd file to userdata folder (not in exe), open and get the required variable.
		CString szFullPath = m_szDevicePath + "\\" + GetPKGFilename();
		if( m_DeviceFile.GainPkgKeyParameters(szFullPath)==FALSE )
		{
			szMsg = "PKG file " + szFullPath + " is not exist.";
			HmiMessage(szMsg);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			return FALSE;
		}

		if( SteerPkgKeyParameters(1)==FALSE )	//	prestart cycle, no file, extract from PKG.
		{
			szMsg = "PKG file " + szFullPath + " parameter error.";
			HmiMessage(szMsg);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			return FALSE;
		}
	}

	BOOL bReturn = SteerPkgKeyParameters(2);	//	prestart cycle, check difference for key parameters.
	CMSLogFileUtility::Instance()->MS_LogOperation("stop check pkg key parameters");
	return bReturn;
}	//	for key parameters check.

BOOL CMS896AApp::LoadPPKGFileByWaferTypeID(CString szTypeID, CString &szErrorMsg)
{
	CString szFilePath;
//	if (bIsPortablePKGFile == FALSE)
//		szFilePath = m_szDevicePath;
//	else
		szFilePath = m_szPortablePKGPath;
	CString szServeFile = szFilePath		+ "\\WaferToPpkgTable.txt";
	CString szLocalFile = gszUSER_DIRECTORY	+ "\\WaferToPpkgTable.txt";
	if (_access(szServeFile, 0)==-1 )
	{
		szErrorMsg = szServeFile + " is missing.";
		return FALSE;
	}

	CopyFile(szServeFile, szLocalFile, FALSE);

	CStdioFile oFile;
	CString szOldPkgName = GetPKGFilename(), szNewPkgName = "";
	if( oFile.Open(szLocalFile, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szLineData = "";
		while( oFile.ReadString(szLineData)!=NULL )
		{
			CStringArray szLineList;
			szLineList.RemoveAll();
			ParseRawDataComma(szLineData, szLineList);
			if( szLineList.GetSize()>=2 )
			{
				if( szTypeID.CompareNoCase(szLineList.GetAt(0))==0 )
				{
					szNewPkgName = szLineList.GetAt(1);
					break;
				}
			}
		}
		oFile.Close();
	}
	else
	{
		szErrorMsg = szLocalFile + " can not be open.";
		return FALSE;
	}

	if( szNewPkgName=="" )
	{
		szErrorMsg = szTypeID + " is not defined in " + szServeFile + ".";
		return FALSE;
	}

	if( szOldPkgName.CompareNoCase(szNewPkgName)==0 )
	{
		return TRUE;
	}

	if( CheckIsAllBinCleared()==FALSE )
	{
		return FALSE;
	}

	CString szTitle = "", szContent = "";
	szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);
	//INT nCol = szNewPkgName.Find(".");
	//CString szFileExt = "";
	//if (nCol != -1)
	//{
	//	szFileExt = szNewPkgName.Right(szNewPkgName.GetLength()- nCol -1);
	//}
	//if (CheckPKGFileExtension(bIsPortablePKGFile, szFileExt) == FALSE)
	//{
	//	szContent.LoadString(HMB_MS_FILE_EXT_NOT_SUPPORTED);
	//	HmiMessage(szContent, szTitle);
	//	return FALSE;
	//}

	CString szFullPath = szFilePath + "\\" + szNewPkgName;
	FILE *pFile = NULL;
	errno_t nErr = fopen_s(&pFile, szFullPath, "r");
	if ((nErr != 0) || (pFile == NULL))
	{
		szContent.LoadString(HMB_MS_FILE_NOT_EXIST);
		HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		szContent = "DEVICE file " + szNewPkgName + " cannot be opened for reading; please check this path at: " + szFullPath;
		HmiMessage(		szContent);
		SetErrorMessage(szContent);
		return FALSE;
	}
	fclose(pFile);

	if ( CreateDirectory(gszPR_RECORD_PATH, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			szContent.LoadString(HMB_MS_CREATE_DRIECTORY_FAILED);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300);
			CMSLogFileUtility::Instance()->WL_LogStatus(szContent);
			return FALSE;
		}
	}

	m_bIsLoadingPKGFile = TRUE;
	m_szDeviceFile = szNewPkgName;
	(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
	(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();
	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	(m_smfSRam)["PR"]["RecordPath"] = gszPR_RECORD_PATH;

	OpenWaitAlert();

	//	WaferTable.msd		WaferPr.msd		WaferLoader.msd		BondHead.msd		BinTable.msd
	//	BondPr.msd			BinLoader.msd	LastState.msd		BLMgzSetup.msd
	CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestoreFromDevice - " + szFullPath);
	CMSLogFileUtility::Instance()->MS_LogOperation("LDF restore file (break PKG to its own files)");
	CStringArray	objFileList;
	objFileList.RemoveAll();
	switch( 0 )
	{
	case 0:
	default:
		objFileList.Add("WaferPr.msd");
		objFileList.Add("BondPr.msd");
	//	objFileList.Add("WaferTable.msd");
	//	objFileList.Add("WaferLoader.msd");
	//	objFileList.Add("BinLoader.msd");
		objFileList.Add("LastState.msd");
		objFileList.Add("BLMgzSetup.msd");
		objFileList.Add("BinTable.msd");
	//	objFileList.Add("BondHead.msd");
		break;
	}

	// Wafer PR Records
	INT i = 0;
	CString szIndex;
	CString szPrRecordPath = gszPR_RECORD_PATH;
	for ( i=0; i <= WPR_MAX_DIE; i++ )
	{
		szIndex.Format("%s\\WPR%d.rec", szPrRecordPath, i);		//v3.13T1
		objFileList.Add(szIndex);
	}
	// Bond PR Records
	for ( i=0; i <= BPR_MAX_DIE; i++ )
	{
		szIndex.Format("%s\\BPR%d.rec", szPrRecordPath, i);
		objFileList.Add(szIndex);
	}
	szIndex.Format("%s\\PEI0.rec", szPrRecordPath);
	objFileList.Add(szIndex);

	BOOL bReturn = TRUE;
	if ( m_DeviceFile.RestoreFiles(szFullPath, objFileList) )
	{
		SavePackageInfo();
		CMSLogFileUtility::Instance()->MS_LogOperation("LDF restore file done");
	}
	else
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestoreFromDevice fails");	//v4.52A10
		bReturn = FALSE;
	}

	SetStatusMessage("Package file is restored");

	m_eqMachine.SetPackage (szNewPkgName);
	m_eqMachine2.SetPackage(szNewPkgName);

	if ( bReturn )
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("LDF to download wafer PR records");
		BOOL bAlwaysFalse = FALSE;
		IPC_CServiceMessage svMsg;
		// Download PR Records
		// Download the WaferPR Records			
		svMsg.InitMessage(sizeof(BOOL), &bAlwaysFalse);	// Init the message
		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "DownloadPrRecords", svMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				svMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if ( bReturn == TRUE )
		{
			// Download the BondPR Records			
			svMsg.InitMessage(sizeof(BOOL), &bAlwaysFalse); // Init the message
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "DownloadPrRecords", svMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, svMsg);
					svMsg.GetMsg(sizeof(BOOL), &bReturn);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if ( bReturn == TRUE )
			{
				// Delete the BondrPR Records			
				DeletePrRecordFiles(FALSE);
				// Delete the WaferPR Records			
				DeletePrRecordFiles(TRUE);
				SavePackageInfo();		// Save package info to String Map file
			}
		}

		m_bManualLoadPkgFile = TRUE;
		m_smfSRam["MS896A"]["Reset Bin Serial No"]	= m_bResetBinSerialNo;

		CMSLogFileUtility::Instance()->MS_LogOperation("LDF machine warm start");
		// Warm Start the machine
		m_bRuntimeLoadPPKG = TRUE;
		MachineWarmStart(TRUE, TRUE);
		LoadFileFormatSelectInNewLot();
		CMSLogFileUtility::Instance()->MS_LogOperation("LDF machien warm start done");
		m_bManualLoadPkgFile = FALSE;
		m_bRuntimeLoadPPKG = FALSE;

		// Copy Reset Serial No Flag
		if ( FALSE && (m_bLoadPkgWithBin || m_bOnlyLoadBinParam) && m_bResetBinSerialNo)
		{
			//Clear Bin's serial no
			IPC_CServiceMessage svMsg;
			BOOL bShowAlert = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bShowAlert);
			INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinSerialNo", svMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, svMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//Clear Bin Table clear bin time
			bShowAlert = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bShowAlert);
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinClearedCount", svMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,svMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}

		//	CMSLogFileUtility::Instance()->MS_LogOperation("LDF init wafer map");
		//	CMS896AStn::m_WaferMapWrapper.InitMap();

		//Update Load PKG file count	
		int nCount, nCol = 0;
		CString szLastDate, szCurrDate, szPKGCount;
		CTime theTime = CTime::GetCurrentTime();	
		if ( (nCol = m_szDeviceCount.Find(":")) != -1 )
		{
			nCount = atoi((LPCTSTR)m_szDeviceCount.Left(nCol));
			szLastDate = m_szDeviceCount.Right(m_szDeviceCount.GetLength() - nCol -1);
			szCurrDate.Format("%d-%d-%d", theTime.GetDay(), theTime.GetMonth(), theTime.GetYear());

			if ( szLastDate != szCurrDate )
			{
				if ( nCount == 0 )
				{
					nCount = 1;
				}
				else
				{
					nCount = 0;
				}
			}
			else
			{
				nCount++;
			}

			if (nCount >= 36)
			{
				nCount = 0;
			}

			szPKGCount.Format("%d", nCount);
			m_szDeviceCount = szPKGCount + ":" + szCurrDate;
			WriteProfileString(gszPROFILE_SETTING, gszDEVICE_COUNT_SETTING, m_szDeviceCount);
			m_smfSRam["MS896A"]["DevFileCount"] = szPKGCount;
		}

		CMSLogFileUtility::Instance()->MS_LogOperation("MS: Load PPKG RestoreFromDevice completed");	//v4.52A10
	}

	CloseWaitAlert();
	m_bIsLoadingPKGFile = FALSE;
	m_bOnBondMode = FALSE;

	if( !bReturn )
	{
		m_szDeviceFile = szOldPkgName;
	}

	return bReturn;
}

BOOL CMS896AApp::LoadBinFile(CString szFilename)
{
	if( CheckIsAllBinCleared()==FALSE )
	{
		return FALSE;
	}
	
	//	WftLoadBinFile
	LONG lStatus = LoadBinSummaryFile(szFilename);
	//v4.59A35
	if (lStatus == FALSE)
		return FALSE;
	return TRUE;
}

BOOL CMS896AApp::LoadPPKGFile(CString szFilename, BOOL bNoResetNVRam)
{
	if( CheckIsAllBinCleared()==FALSE )
	{
		return FALSE;
	}

	m_smfSRam["MS896A"]["Manual Load Pkg"]	= TRUE;
	CString szFileExt = "";
	CString szTitle = "", szContent = "";
	CString szFilePath;
	CString szFileDialogFilter;
	INT nCol = -1;
	INT nReturn = 0;
	BOOL bReturn = FALSE;

	szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);

	m_bIsLoadingPKGFile = TRUE;

	m_bPortablePKGFile = TRUE;

	szFilePath = m_szPortablePKGPath;
	m_szDeviceFile = szFilename;

	nCol = m_szDeviceFile.ReverseFind('.');
	if (nCol != -1)
	{
		szFileExt = m_szDeviceFile.Right(m_szDeviceFile.GetLength()- nCol -1);
	}

	if (CheckPKGFileExtension(TRUE, szFileExt) == FALSE)
	{
		szContent.LoadString(HMB_MS_FILE_EXT_NOT_SUPPORTED);
		HmiMessage(szContent, szTitle);
		bReturn = FALSE;
		m_bIsLoadingPKGFile = FALSE;
		m_bPortablePKGFile = FALSE;
		return FALSE;
	}

	(m_smfSRam)["MS896A"]["PKG Filename Display"]	= GetPKGFilename();
	(m_smfSRam)["MS896A"]["PKG Filename"]			= GetPKGFilename();	//v4.04

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	bReturn = RestorePPKG(szFilePath, GetPKGFilename(), bNoResetNVRam);

	m_bIsLoadingPKGFile = FALSE;
	m_bPortablePKGFile = FALSE;

	return bReturn;
}

BOOL CMS896AApp::RestorePPKG(CString szDevicePath, CString szDeviceFile, BOOL bNoResetNVRam)
{
	int nConvID = 0;
	IPC_CServiceMessage svMsg;
	CString szTitle;
	CString szMsg, szFullPath;
	BOOL bReturn = FALSE, bAlwaysFalse = FALSE;
	BOOL bShowAlert = FALSE;
	BOOL bResetSerialNo = FALSE;

	m_DeviceFile.SetLoadPkgOption(m_bLoadPkgWithBin);
	m_DeviceFile.SetOnlyLoadBinParameters(m_bOnlyLoadBinParam);
	m_DeviceFile.SetOnlyLoadWFTParameters(m_bOnlyLoadWFTParam);
	m_DeviceFile.SetExcludeMachineParameters(m_bExcludeMachineParam);
	m_DeviceFile.SetPortablePKGFile(m_bPortablePKGFile);
	m_DeviceFile.SetOnlyLoadPRParameters(m_bOnlyLoadPRParam);
	m_DeviceFile.SetPortablePackageFileFileList(m_szaPortablePackageFileFileList);
	m_DeviceFile.SetIsPackageFileIgnoreList(m_bIsPortablePackageFileIgnoreList);

	szTitle.LoadString(HMB_MS_LOAD_PKG_FILE);

	szFullPath = szDevicePath + "\\" + szDeviceFile;
	CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestorePPKG - " + szFullPath);	//v4.52A10

	//StartLoadingAlert();		// Show the waiting message
	OpenWaitAlert();			//v4.33T1	//PLLM


	//SeoulSemi: recover all of tempdatabase file
	if ( m_bEnableSaveTempFileWithPKG )
	{
		// recover tempfile
		CStringList szSelection;
		CONST CString szBackupPath = m_szDevicePathDisplay + "\\" + GetPKGFilename() + "_TEMPDATA\\"; 
		CONST CString szExePath = gszROOT_DIRECTORY + "\\EXE\\";

		WIN32_FIND_DATA FileData;
		HANDLE hSearch;
		hSearch = FindFirstFile( szBackupPath + "Blk*tempfile.csv", &FileData);
		if ( hSearch == INVALID_HANDLE_VALUE ) 
		{ 
			//HmiMessage("No Temp Database file recover.");
			SetErrorMessage("No Temp Database file recover.");
		}
		else 
		{
			CopyFile( szBackupPath + FileData.cFileName, szExePath + FileData.cFileName, FALSE);
			Sleep(1);
			int counter = 1;
			while (FindNextFile(hSearch, &FileData) == TRUE)
			{
				CopyFile( szBackupPath + FileData.cFileName, szExePath + FileData.cFileName, FALSE);
				Sleep(1);
				counter++;
			}
			CString szRecoverResult;
			szRecoverResult.Format("%d Temp Database file recovered.", counter);
			SetErrorMessage(szRecoverResult);
			//HmiMessage( szRecoverResult );
		}

		// recover NVRAM.bin
		BOOL bStatus = ImportNVRAMDataFromTxtFile( szBackupPath + _T("NVRAM.bin") );
		if (bStatus)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("NVRAM data is imported from NVRAM.bin"); //v3.55
			CString szTemp = szBackupPath + _T("NVRAM.bin");
			szTemp.Format( "NVRAM file is imported from: %s", szTemp );
			//HmiMessage(szTemp);
			SetErrorMessage(szTemp);
			// copy NVRAM.bin
			CopyFile( szBackupPath + _T("NVRAM.bin"), szExePath + _T("NVRAM.bin"), FALSE);
		}
		else 
		{
			//HmiMessage("NVRAM file import error");
			SetErrorMessage("NVRAM file import error");
		}
	}

	//v4.31T9	//SECSGEM to upload PKG to host
	CString szPackage = szDeviceFile;

	// Check whether the file is existing
	FILE *pFile = NULL;
	errno_t nErr = fopen_s(&pFile, szFullPath, "r");
	if ((nErr != 0) || (pFile == NULL))
	{
		//CloseLoadingAlert();		// Close the waiting message
		CloseWaitAlert();			//v4.33T1	//PLLM

		SetErrorMessage("DEVICE file " + szDeviceFile + " cannot be opened for reading; please check this path at: " + szFullPath);
		return FALSE;
	}
	fclose(pFile);

	CMSLogFileUtility::Instance()->MS_LogOperation("LDF full name " + szFullPath);
	if ( CreateDirectory(gszPR_RECORD_PATH, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			//CloseLoadingAlert();		// Close the waiting message
			CloseWaitAlert();			//v4.33T1	//PLLM

			return FALSE;
		}
	}
	
	//BOOL bNeedDownloadPRRecord = FALSE;		//v4.53A22

	// Set the record path
	(m_smfSRam)["PR"]["RecordPath"] = gszPR_RECORD_PATH;

	CMSLogFileUtility::Instance()->MS_LogOperation("LDF delete wafer PR record files");
	// Delete the WaferPR Records In the Folder First		
	DeletePrRecordFiles(TRUE);
	DeletePrRecordFiles(FALSE);

	CMSLogFileUtility::Instance()->MS_LogOperation("LDF restore file (break PKG to its own files)");
	if ( m_DeviceFile.Restore(szFullPath) )
	{
		if( m_bOnlyLoadBinParam )
		{
			bReturn = TRUE;
			szMsg.LoadString(HMB_MS_LOAD_PKG_OK);
		}
		else
		{	
			if ((m_bDownloadPortablePackageFileImage == FALSE) && IsPortablePKGFile() == TRUE)
			{
				bReturn = TRUE;
				szMsg.LoadString(HMB_MS_LOAD_PKG_OK);	
				SavePackageInfo();
			}
			else
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("LDF to download wafer PR records");
				// Download PR Records
				svMsg.InitMessage(sizeof(BOOL), &bAlwaysFalse);	// Init the message
				// Download the WaferPR Records			
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "DownloadPrRecords", svMsg);
				while (1)
				{
					if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID, svMsg);
						svMsg.GetMsg(sizeof(BOOL), &bReturn);
						break;
					}
					else
					{
						Sleep(10);
					}
				}

				if ( bReturn == TRUE )
				{
					szMsg.LoadString(HMB_MS_LOAD_PKG_OK);
					svMsg.InitMessage(sizeof(BOOL), &bAlwaysFalse); // Init the message
					// Download the BondPR Records			
					nConvID = m_comClient.SendRequest(BOND_PR_STN, "DownloadPrRecords", svMsg);
					while (1)
					{
						if( m_comClient.ScanReplyForConvID(nConvID, 10000) == TRUE )
						{
							m_comClient.ReadReplyForConvID(nConvID, svMsg);
							svMsg.GetMsg(sizeof(BOOL), &bReturn);
							break;
						}
						else
						{
							Sleep(10);
						}
					}

					if ( bReturn == TRUE )
					{
						// Delete the BondrPR Records			
						DeletePrRecordFiles(FALSE);

						// Delete the WaferPR Records			
						DeletePrRecordFiles(TRUE);
						
						SavePackageInfo();		// Save package info to String Map file
					}
					else
					{
						szMsg.LoadString(HMB_MS_DOWNLOAD_BPR_FAILED);
					}
				}
				else
				{
					szMsg.LoadString(HMB_MS_DOWNLOAD_WPR_FAILED);
				}
			}

		}//end of OnlyLoadBinParam is false case

		CMSLogFileUtility::Instance()->MS_LogOperation("LDF restore file done");
	}
	else
	{
		szMsg.LoadString(HMB_MS_LOAD_PKG_FAILED);
		bReturn = FALSE;	//v4.52A10
	}

	//CloseLoadingAlert();		// Close the waiting message

	SetStatusMessage("Package file is restored");

	m_eqMachine.SetPackage (szDeviceFile);
	m_eqMachine2.SetPackage(szDeviceFile);	//WH Sanan	//v4.40T4

	if ( bReturn == FALSE )
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestorePPKG fails");	//v4.52A10
		CloseWaitAlert();			//v4.33T1	//PLLM
		return FALSE;
	}

	m_bManualLoadPkgFile = TRUE;	//v4.53A22
	m_smfSRam["MS896A"]["Reset Bin Serial No"]	= m_bResetBinSerialNo;
	CMSLogFileUtility::Instance()->MS_LogOperation("LDF machine warm start");
	// Warm Start the machine
	MachineWarmStart(TRUE, bNoResetNVRam);
	CloseWaitAlert();			//v4.33T1	//PLLM
	LoadFileFormatSelectInNewLot();
	CMSLogFileUtility::Instance()->MS_LogOperation("LDF machien warn start done");

	m_bManualLoadPkgFile = FALSE;	//v4.53A22

	if ( !m_bEnableSaveTempFileWithPKG )
	{
		//Clear All Bin Counters
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ClearAllBinCounters", svMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		// Copy Reset Serial No Flag
		bResetSerialNo = m_bResetBinSerialNo;
		// If is Traceability fomrat, Not to Reset Bin Serial
		if (CMS896AStn::m_lOTraceabilityFormat == 1 || CMS896AStn::m_lOTraceabilityFormat == 2)
			bResetSerialNo = FALSE;

		if ( ((m_bLoadPkgWithBin == TRUE) || m_bOnlyLoadBinParam == TRUE) && (bResetSerialNo == TRUE))
		{
			//Clear Bin's serial no
			bShowAlert = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bShowAlert);
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinSerialNo", svMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, svMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			//Clear Bin Table clear bin time
			bShowAlert = FALSE;
			svMsg.InitMessage(sizeof(BOOL), &bShowAlert);
			nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "ResetBinClearedCount", svMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,svMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
		}
		CMSLogFileUtility::Instance()->MS_LogOperation("LDF init wafer map");
		CMS896AStn::m_WaferMapWrapper.InitMap();
	}


	//Update Load PKG file count	
	int nCount, nCol = 0;
	CString szLastDate, szCurrDate, szPKGCount;

	CTime theTime = CTime::GetCurrentTime();	

	if ( (nCol = m_szDeviceCount.Find(":")) != -1 )
	{
		nCount = atoi((LPCTSTR)m_szDeviceCount.Left(nCol));
		szLastDate = m_szDeviceCount.Right(m_szDeviceCount.GetLength() - nCol -1);
		szCurrDate.Format("%d-%d-%d", theTime.GetDay(), theTime.GetMonth(), theTime.GetYear());

		if ( szLastDate != szCurrDate )
		{
			if ( nCount == 0 )
			{
				nCount = 1;
			}
			else
			{
				nCount = 0;
			}
		}
		else
		{
			nCount++;
		}

		if (nCount >= 36)
		{
			nCount = 0;
		}

		szPKGCount.Format("%d", nCount);

		m_szDeviceCount = szPKGCount + ":" + szCurrDate;
		WriteProfileString(gszPROFILE_SETTING, gszDEVICE_COUNT_SETTING, m_szDeviceCount);

		m_smfSRam["MS896A"]["DevFileCount"] = szPKGCount;
	}


	CMSLogFileUtility::Instance()->MS_LogOperation("MS: RestorePPKG completed");	//v4.52A10
	return TRUE;
}


BOOL CMS896AApp::GetGmpErrorLogToDrive()
{
	CString szTime, szDrivePath, szGMPErrorLogPath, szGMPErrorExt1Path, szTargetPath;
	CTime theTime;
	LONG lTime;

	theTime = CTime::GetCurrentTime();
	lTime = theTime.GetHour();

	if (lTime == 8 && m_bIsCopyGmpErrorLog)
	{
		if (m_szMachineReportPath.IsEmpty())
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Machine Report Path is empty.");
			m_bIsCopyGmpErrorLog = FALSE;
			return FALSE;
		}

		szDrivePath = m_szMachineReportPath + "\\GMPErrorLog";
		if (CreateDirectory(szDrivePath, NULL) == 0)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				return FALSE;
			}
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Success to Create folder (" + szDrivePath + ")");
		}

		szDrivePath = m_szMachineReportPath + "\\GMPErrorLog\\" + m_szMachineNo;
		if (CreateDirectory(szDrivePath, NULL) == 0)
		{
			if ( GetLastError() != ERROR_ALREADY_EXISTS )
			{
				return FALSE;
			}
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Success to Create folder (" + szDrivePath + ")");
		}

		//Get Time for file name
		szTime.Format("%d%02d%02d_%02d%02d%02d", theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());

		//Copy gmp_error1.errlog
		szGMPErrorLogPath = gszEXE_DIRECTORY + "\\NuData\\gmp_error1.errlog";
		szTargetPath = szDrivePath + "\\gmp_error1_" + szTime + ".errlog";
		if (CopyFile(szGMPErrorLogPath, szTargetPath, FALSE) == FALSE)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Fail to Copy gmp_error1.errlog to " + szTargetPath);
			return FALSE;
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Success to Copy gmp_error1.errlog to " + szTargetPath);
		}

		//Copy gmp_error1_ext1.errlog
		szGMPErrorExt1Path = gszEXE_DIRECTORY + "\\NuData\\gmp_error_ext1.errlog";
		szTargetPath = szDrivePath + "\\gmp_error1_ext1_" + szTime + ".errlog";
		if (CopyFile(szGMPErrorExt1Path, szTargetPath, FALSE) == FALSE)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Fail to Copy gmp_error_ext1.errlog to " + szTargetPath);
			return FALSE;
		}
		else
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Success to Copy gmp_error_ext1.errlog to " + szTargetPath);
		}

		m_bIsCopyGmpErrorLog = FALSE;
	}
	else if ((lTime == 9 || lTime == 7) && !m_bIsCopyGmpErrorLog)
	{
		m_bIsCopyGmpErrorLog = TRUE;
	}

	return TRUE;
}


BOOL CMS896AApp::LoadPackageMsdMSData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	IPC_CServiceMessage stMsg;

	// open config file
	if (pUtl->LoadPackageDataConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetPackageDataConfig();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}
//content
	CMS896AApp::m_bEnableMachineLog	= (BOOL)(LONG)(*psmf)["MapSorter Data"]["Log Enable Option"]["Enable Machine Log"];
	EnableMachineLog(stMsg);

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to load MS data from PackageData.msd");

	// close config file
    pUtl->ClosePackageDataConfig();

	return TRUE;
}


BOOL CMS896AApp::SavePackageMsdMSData(VOID)
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;

	// open config file
	if (pUtl->LoadPackageDataConfig() == FALSE)
	{
		return FALSE;
	}

	// get file pointer
	psmf = pUtl->GetPackageDataConfig();

	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

	//content
	(*psmf)["MapSorter Data"]["Log Enable Option"]["Enable Machine Log"] = CMS896AApp::m_bEnableMachineLog;

	pUtl->UpdatePackageDataConfig();	
	// close config file
    pUtl->ClosePackageDataConfig();

	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to save MS data from PackageData.msd");

	return TRUE;
}


BOOL CMS896AApp::UpdateMSPackageList(VOID)
{
	CStdioFile fTemp;
	CString szContent, szText, szTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\MSPackageList.csv"), CFile::modeCreate| CFile::modeReadWrite))
	{
		return FALSE;
	}

	szText = "Enable Machine Log";
	szText = szText + ",";
	szTemp.Format("%d,", CMS896AApp::m_bEnableMachineLog);
	szContent = szContent + szText + szTemp;

	fTemp.WriteString(szContent);
	CMSLogFileUtility::Instance()->MS_LogOperation("Finish to update MS package data to MSPackageList.csv");

	fTemp.Close();

	return TRUE;
}
