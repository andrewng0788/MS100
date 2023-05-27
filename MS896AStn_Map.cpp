#include "stdafx.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include <math.h>
#include <sys/timeb.h>
#include "GenerateDatabase.h"
#include "GenerateWaferDatabase.h"
#include "QueryWaferDatabase.h"
#include "QueryDatabase.h"
#include "OutputFileFactory.h"
#include "FactoryMap.h"
#include "OutputFileInterface.h"
#include "WaferEndFileFactory.h"
#include "WaferEndFactoryMap.h"
#include "WaferEndFileInterface.h"
//#include "Encryption.h"
#include "PrescanInfo.h"
#include "WaferMap.h"

#include <gdiplus.h>
#include <AtlImage.h>
#include "Utility.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CMS896AStn::LoadWaferMap(LPCTSTR strFileName, LPCTSTR strFormatName, const BOOL bMenuLoad)
{
	CString szLogMsg = "    WFT: To load map file ";
	szLogMsg = szLogMsg + strFileName + ";" + strFormatName;
	SaveScanTimeEvent(szLogMsg);
	m_bAutoLoadWaferMap = bMenuLoad ? FALSE : TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	//Song SanAn auto load PPKG
	BOOL bRunTimeLoadPPKG = (pApp->IsRunTimeLoadPKGFile() && pApp->GetCustomerName()==CTM_SANAN);

	CString szLoadMapMsg;
	CMSLogFileUtility::Instance()->WL_LogStatus("");
	szLoadMapMsg.Format("%d",bRunTimeLoadPPKG);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLoadMapMsg);
	if( IsBurnIn()==FALSE && IsEnablePNP()==FALSE &&
		pUtl->GetPrescanDummyMap()==FALSE && bRunTimeLoadPPKG && m_ulTotalSorted == 0)	//	before load check sort type and program/version
	{
		m_WaferMapWrapper.InitMap();

		CStdioFile oFile;
		if( !oFile.Open(strFileName, CFile::modeRead|CFile::shareDenyNone) )
		{
			HmiMessage_Red_Back("Can not open map file.", "Please check.");
			return FALSE;
		}

		CString szMapLotNumber = "";
		CString szTemp = "";
		CString szLineData;
		while( oFile.ReadString(szLineData) )
		{
			if( szLineData.Find("map data")!=-1 )
			{
				break;
			}
			//LotNumber,,"S-35FBMUD-A"
			if( szLineData.Find("LotNumber,")!=-1 )
			{
				int nIndex = szLineData.Find(",\"");
				szTemp = szLineData.Mid(nIndex + 2);
				szMapLotNumber = szTemp.Left(szTemp.GetLength()-1);
				SetErrorMessage("Map:" + szMapLotNumber);
			}
		}
		oFile.Close();

		CString szMsg;
		(*m_psmfSRam)["MS896A"]["PKG RunTime Loading done"]	= FALSE;
		CString	szOldPkgName = pApp->GetPKGFilename();
		CString szNewPkgName = pApp->GetPPKGFilenameByLotNo(szMapLotNumber);

		if( szOldPkgName.IsEmpty() || szOldPkgName.CompareNoCase(szNewPkgName)!=0 )
		{
			if( pApp->CheckIsAllBinCleared()==FALSE )
			{
				szMsg += " Error: clear all bin first ";
				HmiMessage_Red_Yellow(szMsg, "Auto Load PPKG");
				return FALSE;
			}

			if (szNewPkgName.IsEmpty())
			{
				szMsg += " Error: New PPKG name is empty ";
				HmiMessage_Red_Yellow(szMsg, "Auto Load PPKG");
				return FALSE;
			}

			CMSLogFileUtility::Instance()->WL_LogStatus("");
			szMsg = szOldPkgName + " is old; " + szNewPkgName + " is new;";
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			szMsg = "Map load PPKG " + szNewPkgName;
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			DOUBLE dTime = GetTime();
			LONG lAlarmStatus = GetAlarmLamp_Status();
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
			if( pApp->LoadPPKGFile(szNewPkgName)==FALSE )
			{
				szMsg += " Error";
				HmiMessage_Red_Yellow(szMsg, "Auto Load PPKG");
				return FALSE;
			}
			SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
			LONG lTime = (LONG)(GetTime() - dTime)/1000;
			szMsg.Format("Load PKG at run time used %d seconds", lTime);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		}

		(*m_psmfSRam)["MS896A"]["PKG RunTime Loading done"]	= TRUE;
	}

	BOOL bFixSortType = pApp->IsRunTimeLoadPKGFile() && pApp->GetCustomerName()==CTM_LEXTAR;
	if( IsBurnIn()==FALSE && IsEnablePNP()==FALSE &&
		pUtl->GetPrescanDummyMap()==FALSE && bFixSortType )	//	before load check sort type and program/version
	{
		m_WaferMapWrapper.InitMap();
		CStdioFile oFile;
		if( !oFile.Open(strFileName, CFile::modeRead|CFile::shareDenyNone) )
		{
			HmiMessage_Red_Back("Can not open map file.", "Please check.");
			return FALSE;
		}

		CString szMapProgramN = "", szMapProgramV = "", szMapSortType = "", szMapRecipeID = "", szMapRecipeNm = "";
		CString szLineData;
		while( oFile.ReadString(szLineData) )
		{
			//PosX,PosY,Bin,AOIBin,CPTBin,IRa,ESDa,Thya,VFa,VFb,VFd,Wdd,Wpd,D_Wdd,HWd,Xd,Yd,PURd,Pod,Ivd,VRa
			if( szLineData.Find("PosX,PosY,Bin,")!=-1 )
			{
				break;
			}
			CStringArray szLineList;
			szLineList.RemoveAll();
			CUtility::Instance()->ParseRawData(szLineData, szLineList);
			//SortType,Fixed
			if( szLineData.Find("SortType,")!=-1 && szLineList.GetSize()>=2 )
			{
				szMapSortType = szLineList.GetAt(1);
				SetErrorMessage("Map:" + szLineData);
			}
			//Program,CB22N01_V01
			if( szLineData.Find("Program,")!=-1 && szLineList.GetSize()>=2 )
			{
				szMapProgramN = szLineList.GetAt(1);
				SetErrorMessage("Map:" + szLineData);
			}
			//ProgramVersion,1
			if( szLineData.Find("ProgramVersion,")!=-1 && szLineList.GetSize()>=2 )
			{
				szMapProgramV = szLineList.GetAt(1);
				SetErrorMessage("Map:" + szLineData);
			}
			//Recipe_ID,CB22NB7-B4V3-UT-RA
			if( szLineData.Find("Recipe_ID,")!=-1 && szLineList.GetSize()>=2 )
			{
				szMapRecipeID = szLineList.GetAt(1);
				SetErrorMessage("Map:" + szLineData);
			}
			//Recipe_Name,
			if( szLineData.Find("Recipe_Name,")!=-1 && szLineList.GetSize()>=2 )
			{
				szMapRecipeNm = szLineList.GetAt(1);
				SetErrorMessage("Map:" + szLineData);
			}
		}
		oFile.Close();

		CString szMsg;
		if( szMapSortType!="Fixed" )
		{
			szMsg = "Current map Sort Type:" + szMapSortType + " is not Fixed.";
			HmiMessage_Red_Back(szMsg, "Please Check");
			SetErrorMessage(szMsg);
			return FALSE;
		}

		CString szOldProgramN = pApp->GetProfileString(gszPROFILE_SETTING, gszLextarMapProgramN, "");
		CString szOldProgramV = pApp->GetProfileString(gszPROFILE_SETTING, gszLextarMapProgramV, "");
		if( !pApp->CheckIsAllBinClearedNoMsg() )
		{
			szMsg = "Old Program:" + szOldProgramN + "; Map Program:" + szMapProgramN;
			if( szOldProgramN!=szMapProgramN )
			{
				HmiMessage_Red_Back(szMsg, "Please Check");
				SetErrorMessage(szMsg);
				return FALSE;
			}
			szMsg = "Old Program Version:" + szOldProgramV + "; Map Program Version:" + szMapProgramV;
			if( szOldProgramV!=szMapProgramV )
			{
				HmiMessage_Red_Back(szMsg, "Please Check");
				SetErrorMessage(szMsg);
				return FALSE;
			}
		}

		(*m_psmfSRam)["MS896A"]["PKG RunTime Loading done"]	= FALSE;
		CString	szOldPkgName = pApp->GetPKGFilename();
		CString szNewPkgName = szMapRecipeID + ".ppkg";
		if( szOldPkgName.IsEmpty() || szOldPkgName.CompareNoCase(szNewPkgName)!=0 )
		{
			if( pApp->CheckIsAllBinCleared()==FALSE )
			{
				return FALSE;
			}

			CMSLogFileUtility::Instance()->WL_LogStatus("");
			szMsg = szOldPkgName + " is old; " + szNewPkgName + " is new;";
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			szMsg = "Map load PPKG " + szNewPkgName;
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			DOUBLE dTime = GetTime();
			LONG lAlarmStatus = GetAlarmLamp_Status();
			SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);
			if( pApp->LoadPPKGFile(szNewPkgName)==FALSE )
			{
				szMsg += " Error";
				HmiMessage_Red_Yellow(szMsg, "Auto Load PPKG");
				return FALSE;
			}
			SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
			LONG lTime = (LONG)(GetTime() - dTime)/1000;
			szMsg.Format("Load PKG at run time used %d seconds", lTime);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		}
		if( pApp->CheckIsAllBinClearedNoMsg() )
		{
			if( szOldProgramN != szMapProgramN || szOldProgramV != szMapProgramV)
			{
				CString szBinFile = szMapProgramN + "_" + szMapProgramV;	//	ext .csv fixed
				if( pApp->LoadBinFile(szBinFile) )
				{
					return FALSE;
				}
			}
		}
		szOldProgramN = szMapProgramN;
        szOldProgramV = szMapProgramV;
		pApp->WriteProfileString(gszPROFILE_SETTING, gszLextarMapProgramN, szOldProgramN);
		pApp->WriteProfileString(gszPROFILE_SETTING, gszLextarMapProgramV, szOldProgramV);
		(*m_psmfSRam)["MS896A"]["PKG RunTime Loading done"]	= TRUE;
	}

	return m_WaferMapWrapper.LoadMap(strFileName, strFormatName);
}

CString CMS896AStn::GetMapFileName()
{
	return m_WaferMapWrapper.GetFileName();
}

CString CMS896AStn::GetMapNameOnly()
{
	return m_szMapFileName;
}

BOOL CMS896AStn::IsWaferMapValid()
{
	BOOL bValid = m_WaferMapWrapper.IsMapValid();
	if( bValid==FALSE && IsBurnIn()==FALSE && m_bContourPreloadMap )
	{
		bValid = m_WaferMapWrapper.IsMapCreated();
	}

	return bValid;
}

BOOL CMS896AStn::ConvertAsmToHmiUser(ULONG ulAsmRow, ULONG ulAsmCol, LONG &lHmiRow, LONG &lHmiCol)
{
	return m_WaferMapWrapper.ConvertInternalToUser(ulAsmRow, ulAsmCol, lHmiRow, lHmiCol);
}

BOOL CMS896AStn::ConvertHmiUserToAsm(LONG lHmiRow, LONG lHmiCol, ULONG &ulAsmRow, ULONG &ulAsmCol)
{
	return m_WaferMapWrapper.ConvertUserToInternal(lHmiRow, lHmiCol, ulAsmRow, ulAsmCol);
}

// convert from the asm map into customer map original (the dispaly in () on HMI)
BOOL CMS896AStn::ConvertAsmToOrgUser(ULONG ulAsmRow, ULONG ulAsmCol, LONG &lUserRow, LONG &lUserCol)
{
	return m_WaferMapWrapper.ConvertInternalToOriginalUser(ulAsmRow, ulAsmCol, m_bMapOHFlip, m_bMapOVFlip, m_usMapAngle, lUserRow, lUserCol);
}

BOOL CMS896AStn::ConvertOrgUserToAsm(LONG lUserRow, LONG lUserCol, ULONG &ulAsmRow, ULONG &ulAsmCol)
{
	return m_WaferMapWrapper.ConvertOriginalUserToInternal(lUserRow, lUserCol, m_bMapOHFlip, m_bMapOVFlip, m_usMapAngle, ulAsmRow, ulAsmCol);
}

BOOL CMS896AStn::IsInSelectedGrades(ULONG ulRow, ULONG ulCol)
{
	UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
	CUIntArray aulSelectedGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	for(INT i = 0; i < aulSelectedGradeList.GetSize(); i++)
	{
		if( ucGrade==(UCHAR)aulSelectedGradeList.GetAt(i) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

ULONG CMS896AStn::GetMapDieState(ULONG ulRow, ULONG ulCol)
{
	if (IsScanEmptyGrade(ulRow, ulCol))
	{
		return WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY;
	}

	return m_WaferMapWrapper.GetDieState(ulRow, ulCol);
}

/*
BOOL CMS896AStn::GetWaferMapDimension(ULONG &ulMaxRow, ULONG &ulMaxCol)
{
	ulMaxRow = 0;
	ulMaxCol = 0;

	if (m_WaferMapWrapper.GetMapDimension(ulMaxRow, ulMaxCol) != TRUE)
	{
		return FALSE;
	}

	if ((ulMaxRow >= 1001) && (ulMaxCol >= 1001))
	{
		SetErrorMessage("No of Row/Col are out of range!");
		return FALSE;
	}

	return TRUE;
}
*/

VOID CMS896AStn::ResetMapPhyPosn()
{
	ULONG ulMaxRow = 0, ulMaxCol = 0;

	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMaxRow, ulMaxCol);
	for (ULONG i = 0; i < (ulMaxRow+10); i++)
	{
		for (ULONG j = 0; j < (ulMaxCol+10); j++)
		{
			// claer all physical position
			m_WaferMapWrapper.BatchSetPhysicalPosition(i, j , 0x80000000 , 0x80000000);
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
}

BOOL CMS896AStn::GetMapPhyPosn(ULONG ulRow, ULONG ulCol, LONG &lPhyX, LONG &lPhyY)
{
	if( IsInMapValidRange(ulRow, ulCol)==FALSE )
	{
		return FALSE;
	}

	//v4.38T4	//Avoid passing 0x80000000 to outside
	LONG lTempX = 0;
	LONG lTempY = 0;

	BOOL bGet = m_WaferMapWrapper.GetPhysicalPosition(ulRow, ulCol, lTempX, lTempY);
	if( labs(lTempX)==0x80000000 || labs(lTempY)==0x80000000 )
	{
		bGet = FALSE;
	}
	if (labs(lTempX) == 2147483648 || labs(lTempY) == 2147483648)		// 0x80000000
	{
		bGet = FALSE;
	}
	if ((labs(lTempX) > 10000000) || (labs(lTempY) > 10000000))		//v4.38T5
	{
		bGet = FALSE;
	}
	if( lTempX==0 && lTempY==0 )
	{
		bGet = FALSE;
	}

	if (bGet)
	{
		lPhyX = lTempX;
		lPhyY = lTempY;
	}
	return bGet;
}


VOID	CMS896AStn::ObtainMapValidRange()
{
	ULONG ulMapRowMax = 0, ulMapColMax = 0, ulRow, ulCol;
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset	= m_WaferMapWrapper.GetGradeOffset();
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

	m_lMapValidMinRow = 0;
	m_lMapValidMinCol = 0;
	m_lMapValidMaxRow = ulMapRowMax;
	m_lMapValidMaxCol = ulMapColMax;
	BOOL bFindEdge = FALSE;
	for (ulRow = 0; ulRow < ulMapRowMax; ulRow++)
	{
		for (ulCol = 0; ulCol < ulMapColMax; ulCol++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if (ucGrade != ucNullBin)
			{
				if (IsMS90HalfSortMode())
				{
					if( IsScanMapNgGrade(ucGrade-ucOffset) )
					{
						continue;
					}
				}
				m_lMapValidMinRow = ulRow;
				bFindEdge = TRUE;
				break;
			}
		}
		if (bFindEdge)
		{
			break;
		}
	}
	bFindEdge = FALSE;
	for (ulCol = 0; ulCol < ulMapColMax; ulCol++)
	{
		for (ulRow = 0; ulRow < ulMapRowMax; ulRow++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if (ucGrade != ucNullBin)
			{
				if( IsMS90HalfSortMode() )
				{
					if( IsScanMapNgGrade(ucGrade-ucOffset) )
					{
						continue;
					}
				}
				m_lMapValidMinCol = ulCol;
				bFindEdge = TRUE;
				break;
			}
		}
		if (bFindEdge)
		{
			break;
		}
	}
	bFindEdge = FALSE;
	for (ulRow = ulMapRowMax; ulRow > 0; ulRow--)
	{
		for (ulCol = 0; ulCol < ulMapColMax; ulCol++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if (ucGrade != ucNullBin)
			{
				if( IsMS90HalfSortMode() )
				{
					if( IsScanMapNgGrade(ucGrade-ucOffset) )
					{
						continue;
					}
				}
				m_lMapValidMaxRow = ulRow;
				bFindEdge = TRUE;
				break;
			}
		}
		if (bFindEdge)
		{
			break;
		}
	}
	bFindEdge = FALSE;
	for (ulCol = ulMapColMax; ulCol > 0; ulCol--)
	{
		for (ulRow = 0; ulRow < ulMapRowMax; ulRow++)
		{
			UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			if (ucGrade != ucNullBin)
			{
				if( IsMS90HalfSortMode() )
				{
					if( IsScanMapNgGrade(ucGrade-ucOffset) )
					{
						continue;
					}
				}
				m_lMapValidMaxCol = ulCol;
				bFindEdge = TRUE;
				break;
			}
		}
		if (bFindEdge)
		{
			break;
		}
	}
}

BOOL CMS896AStn::IsInMapValidRange(ULONG ulRow, ULONG ulCol)
{
	if (ulRow < (ULONG)GetMapValidMinRow() || ulRow > (ULONG)GetMapValidMaxRow() ||
		ulCol < (ULONG)GetMapValidMinCol() || ulCol > (ULONG)GetMapValidMaxCol())
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CMS896AStn::InitWaferMap()
{
	CString szAssociateFilename;
	CString szAssociateFile;
	unsigned long ulSize;
	CString szMsg;
	void *pvTemp;
	BOOL bReturn = TRUE;

	WAF_CMapConverterFactory::Instance()->AutoInstallLibrary("Converter");
	WAF_CMapMemoryManagement::Instance()->AutoInstallLibrary("MemoryComponent");
	WAF_CDieSelectorFactory::Instance()->AutoInstallLibrary("Algorithm");

	ulSize = m_WaferMapWrapper.GetPersistentSize();
	if ((m_pvNVRAM == NULL) || (ulSize > glNVRAM_WaferMap_Size))
	{
		// Create a 60000 byte file memory
		WAF_CMapMemoryManagement::Instance()->CreateMemory("FileStorage", "Wafermap", NULL, 60000);
	}
	else
	{
		pvTemp = (void*)((unsigned long)m_pvNVRAM + glNVRAM_WaferMap_Start);
		WAF_CMapMemoryManagement::Instance()->CreateMemory("SRAMStorage", "Wafermap", pvTemp, ulSize);
		szMsg.Format("Wafer Map - NVRAM is used (size = %ld)", ulSize);
		DisplayMessage(szMsg);
	}
	m_WaferMapWrapper.Initialize("Wafermap", "WaferMapBuffer", 0x2800000);
	m_WaferMapWrapper.EnableRealTimeUpdate(FALSE);
	m_WaferMapWrapper.SetEvent(&m_WaferMapEvent);
	m_WaferMapWrapper.SuspendAlgorithmPreparation();

	if (m_WaferMapWrapper.WarmStart(TRUE))
	{
		DisplayMessage("Wafer Map - Warm start successfully");
		if (m_WaferMapWrapper.CheckStatistics() == FALSE)
		{
			//v4.59A18
			CString szLog;
			szLog = "MapSorter was not properly shutdown - Wafermap statistic will be reset";
			DisplayMessage(szLog);
			SetErrorMessage(szLog);
			//AfxMessageBox("MapSorter was unproperly shutdown!\n\nWafermap statistic will be reset"); 
			m_WaferMapWrapper.ResetStatistics();
		}

		if (CMS896AStn::m_bNoIgnoreGradeOnInterface == TRUE)
		{
			m_WaferMapWrapper.ShowIgnoreGrade(FALSE);
		}

		//2019.1.16
		//if machine shut down, it need copy origin die information again, other machine will get the wrong grade with GetOriginalDieInfromation function if not re-load wafer map again
		if (m_WaferMapWrapper.GetReader() != NULL) 
		{
			//Preserve a copy of wafermap grades in current memory	
			//SanAn mixedbin sorting
			//Wafermap library v1.67.00
			m_WaferMapWrapper.CopyOriginalDieInformation();
		}
	}
	else 
	{
		DisplayMessage("Wafer Map - Unable to warm start");
		bReturn = FALSE;
		m_WaferMapWrapper.ResetStorage();	//v4.22T2
	}


	CMS896AApp *pApp = dynamic_cast<CMS896AApp*>(m_pModule);
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);

	// Create a 60000 byte file memory
	WAF_CMapMemoryManagement::Instance()->CreateMemory("FileStorage", "binmap", NULL, 60000);
	m_BinMapWrapper.Initialize("Binmap", "BinMapBuffer", 0xa00000);
	m_BinMapWrapper.EnableRealTimeUpdate(FALSE);
	m_BinMapWrapper.SetEvent(&m_BinMapEvent);
	m_BinMapWrapper.EnableAutoRefresh(TRUE);

	// Added for Testar col value add by 1
	m_WaferMapWrapper.SetVerticalZeroSkip(FALSE);
	m_WaferMapWrapper.SetHorizontalZeroSkip(FALSE);
	m_WaferMapWrapper.EnableGradeMap(FALSE);

	if ( IsMapDetectSkipMode() && pApp->GetCustomerName()=="Cree" )
	{
#ifdef NU_MOTION
		m_WaferMapWrapper.SetMaxGradeMapSize(150);		//Only 150 bins available for Cree Dynamic gradeMapping on MS100Plus
#else
		m_WaferMapWrapper.SetMaxGradeMapSize(100);		//Only 100 bins available for Cree Dynamic gradeMapping on MS899
#endif
	}
	else
	{
		m_WaferMapWrapper.SetMaxGradeMapSize(MS_MAX_BIN);
	}

	m_WaferMapWrapper.EnableAutoRefresh(TRUE);
	
	if (m_bEnableAssocFile == TRUE)
	{
		szAssociateFile = (*m_psmfSRam)["MS896A"]["Associate File"];
			
		if (szAssociateFile.IsEmpty() == FALSE)
		{
			szAssociateFilename = szAssociateFile + "." + m_szAssocFileExt;
		}
		else
		{
			szAssociateFilename = szAssociateFile;
		}

		m_WaferMapWrapper.SetAssociateFile(szAssociateFilename);
		DisplayMessage("Wafer Map - Associate File:" + szAssociateFilename);

	}

	//v4.50A5
	if ( IsMapDetectSkipMode() )
	{
		m_WaferMapWrapper.SetDetectSkipMode(m_lSetDetectSkipMode);
		if( pApp->GetCustomerName()=="EverLight" )
		{
			m_WaferMapWrapper.EnableGradeMap(TRUE);
		}
		CString szDetectSkipMode;
		szDetectSkipMode.Format("Wafer Map - SetDetectSkipMode:%d", m_lSetDetectSkipMode);
		DisplayMessage(szDetectSkipMode);
		CMSLogFileUtility::Instance()->MS_LogOperation(szDetectSkipMode);
	}

	//v4.52A11	//XM SanAn
	if ( !pApp->IsToSaveMap() )
	{
		//Fixed long START/STOP time due to temp map generation by wafermap library;
		//However, need to store temp map in ExitInstance instead for next WarmStart;
#ifdef NU_MOTION	//Not available for MS899
		m_WaferMapWrapper.SetStoreTempOnStop(FALSE);
#endif
	}

	if (pApp->GetCustomerName() == "Lumileds")		//v4.53A21
	{
		//Grade-0 dices: 0=INVALID, 1=NULL, 2= NORMAL
//#ifdef NU_MOTION	//Not available for MS899
		m_WaferMapWrapper.SetMapInterpretation(2);
		CString szLog;
		szLog.Format("Wafer Map (Lumileds) - MapInterpretation = %d", 1);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
//#endif
	}

	return bReturn;
}

BOOL CMS896AStn::ObtainEmcoreBarColumnStart()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( pApp->GetCustomerName()=="emcore" && pUtl->GetPrescanBarWafer() && m_WaferMapWrapper.IsMapValid() )
	{
		CStdioFile oFile;
		if( oFile.Open(GetMapFileName(), CFile::modeRead|CFile::shareDenyNone) )
		{	// Get the sample electricial data
			for (ULONG ulRow = (ULONG)GetMapValidMinRow(); ulRow <= (ULONG)GetMapValidMaxRow(); ulRow++)
			{
				CString szPreviousDie = "";
				for (ULONG ulCol = (ULONG)GetMapValidMinCol(); ulCol <= (ULONG)GetMapValidMaxCol(); ulCol++)
				{
					if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow, ulCol))
					{
						continue;
					}
					CString szLineData = "", szOcrValue = "";
					CStringArray szLineList;
					m_WaferMapWrapper.GetExtraInformation(&oFile, ulRow, ulCol, szLineData);
					szLineList.RemoveAll();
					CUtility::Instance()->ParseRawData(szLineData, szLineList);
					if (szLineList.GetSize() >= 1)
					{
						szOcrValue = szLineList.GetAt(0);
						CString szLetter = szOcrValue.Left(2);
						CString szNumber = szOcrValue.Right(szOcrValue.GetLength()-2);
						if (szPreviousDie != "")
						{
							if (szPreviousDie != szLetter)
							{
								m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
								m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_BAR_2ND_START);
								break;
							}
						}
						szPreviousDie = szLetter;
					}
				}
			}
		}
		oFile.Close();
	}

	return TRUE;
}

BOOL CMS896AStn::IsMapDetectSkipMode()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	return pApp->IsMapDetectSkipMode();
}


UCHAR CMS896AStn::GetMapGrade(const ULONG ulRow, const ULONG ulCol)
{
	UCHAR ucGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
	if( DEB_IsUseable() && m_bGiveUpDebKeyDie==FALSE && m_ucDEBKeyDieGrade>0 )
	{
		if( m_WaferMapWrapper.GetReader()!=NULL )
		{
			if( m_WaferMapWrapper.GetReader()->GetSpecialFlag(ulRow, ulCol)==DIE_SPECIAL_KEYDIE )
			{
				CString szMsg;
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				szMsg.Format("    WFT: map key die grade,%4d,%4d,%4d,%4d,%d=>%d",
					ulRow, ulCol, lUserRow, lUserCol, ucGrade - m_WaferMapWrapper.GetGradeOffset(), m_ucDEBKeyDieGrade);
				CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
				ucGrade = m_ucDEBKeyDieGrade + m_WaferMapWrapper.GetGradeOffset();
			}
		}
	}

	return ucGrade;
}

BOOL CMS896AStn::PeekMapDie(ULONG ulSkipDice, ULONG &ulRow, ULONG &ulCol, UCHAR &ucGrade, 
						WAF_CDieSelectionAlgorithm::WAF_EDieAction& eAction, BOOL &bEndOfWafer)
{
	BOOL bReturn = m_WaferMapWrapper.PeekNextDie(ulSkipDice, ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
	if( DEB_IsUseable() && m_bGiveUpDebKeyDie==FALSE && m_ucDEBKeyDieGrade>0 )
	{
		if( m_WaferMapWrapper.GetReader()!=NULL )
		{
			if( m_WaferMapWrapper.GetReader()->GetSpecialFlag(ulRow, ulCol)==DIE_SPECIAL_KEYDIE )
			{
				CString szMsg;
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				szMsg.Format("    WFT: peek key die grade,%4d,%4d,%4d,%4d,%d=>%d",
					ulRow, ulCol, lUserRow, lUserCol, ucGrade - m_WaferMapWrapper.GetGradeOffset(), m_ucDEBKeyDieGrade);
				CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
				ucGrade = m_ucDEBKeyDieGrade + m_WaferMapWrapper.GetGradeOffset();
			}
		}
	}
	return bReturn;
}

BOOL CMS896AStn::GetMapDie(ULONG &ulRow, ULONG &ulCol, UCHAR &ucGrade,
						WAF_CDieSelectionAlgorithm::WAF_EDieAction &eAction, BOOL &bEndOfWafer)
{
	BOOL bReturn = m_WaferMapWrapper.GetNextDie(ulRow, ulCol, ucGrade, eAction, bEndOfWafer);
	if( DEB_IsUseable() && m_bGiveUpDebKeyDie==FALSE && m_ucDEBKeyDieGrade>0 )
	{
		if( m_WaferMapWrapper.GetReader()!=NULL )
		{
			if( m_WaferMapWrapper.GetReader()->GetSpecialFlag(ulRow, ulCol)==DIE_SPECIAL_KEYDIE )
			{
				CString szMsg;
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				szMsg.Format("    WFT: get key die grade,%4d,%4d,%4d,%4d,%d=>%d",
					ulRow, ulCol, lUserRow, lUserCol, ucGrade - m_WaferMapWrapper.GetGradeOffset(), m_ucDEBKeyDieGrade);
				CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
				ucGrade = m_ucDEBKeyDieGrade + m_WaferMapWrapper.GetGradeOffset();
			}
		}
	}
	return bReturn;
}

BOOL CMS896AStn::UpdateMapDie(const ULONG ulRow, const ULONG ulCol, const UCHAR ucGrade,
							  const WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction, const ULONG ulDieState)
{
	UCHAR ucUpGrade = ucGrade;
	if( DEB_IsUseable() && m_bGiveUpDebKeyDie==FALSE && m_ucDEBKeyDieGrade>0 )
	{
		if( m_WaferMapWrapper.GetReader()!=NULL )
		{
			if( m_WaferMapWrapper.GetReader()->GetSpecialFlag(ulRow, ulCol)==DIE_SPECIAL_KEYDIE )
			{
				ucUpGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
				CString szMsg;
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
				szMsg.Format("    WFT: update key die grade,%4d,%4d,%4d,%4d,%d=>%d",
					ulRow, ulCol, lUserRow, lUserCol, ucGrade - m_WaferMapWrapper.GetGradeOffset(), ucUpGrade - m_WaferMapWrapper.GetGradeOffset());
				CMSLogFileUtility::Instance()->WT_WriteKeyDieLog(szMsg);
			}
		}
	}

	 WM_CWaferMap::Instance()->DecreaseRegionStatistics(ucUpGrade);
	return m_WaferMapWrapper.Update(ulRow, ulCol, ucUpGrade, eAction, FALSE, ulDieState);
}
