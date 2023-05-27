#include "stdafx.h"
#include <Math.h>
#include <stdio.h>
#include <string.h>
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_BinBlk.h"
#include "BT_BinBlkMain.h"
#include "FileUtil.h"
#include "GenerateDatabase.h"
#include "FactoryMap.h"
#include "OutputFileInterface.h"
#include "OutputFileFactory.h"
#include "LogFileUtil.h"
#include <crtdbg.h>
#include "io.h"
#include "NGGrade.h"
#include "MathFunc.h"
#pragma once

/*************************** Class CBinBlkMain Start ***************************/

/***********************************/
/*     Constructor/Destructor      */
/***********************************/
CBinBlkMain::CBinBlkMain()
{
	m_pBinTable = NULL;
	m_pstNvTempFileData		= NULL;
	m_pstNvTempFileData_HW	= NULL;
	m_bUseHWNVRam			= TRUE;

	m_ulBondCountToGenTempFile	= MAX_RT_LIMIT;
	m_bEnableLogging = FALSE;	//v4.39T9	//PLLM MS109
	m_lEnableLogCount = 0;

	m_ulNoOfBlk = 1;
	m_lBlkPitchX = 0;
	m_lBlkPitchY = 0;
	m_ulMsgCode = 0;
	m_szLifeTime = "";
	m_szESD = "";
	m_szIF = "";
	m_szTapeID = "";
	m_szBinTableFilename = "";

	m_ulSerialNoFormat = 1;
	m_ulSerialNoMaxLimit = 9999;

	m_szBinClearedDate = "";
	m_ulBinClearedCount = 1;
	m_ulBinClearFormat = 0;
	m_ulBinClearInitCount = 1;
	m_ulBinClearMaxLimit = 99;

	m_bFilenameASWaferID = FALSE;
	m_bEnableNVRunTimeData = FALSE;

	m_bStoreBinBondedCount = FALSE;	
	m_bEnableAutoAssignGrade = FALSE;	
	m_bEnableLSBondPattern = FALSE;	

	m_bFirstTimeInitOutputFile = TRUE;

	m_bEnable2DBarcodeOutput = FALSE;	
	m_ulHoleDieNum = 3;	

	//CyOptics	
	for (INT i = 0; i < BT_MAX_BINBLK_SIZE; i++)
	{
		m_ulCurrHoleDieNum[i]	= 0;
		m_bLeaveEmptyRow[i]		= FALSE;		//v4.28T4	
		m_ul1stRowIndex[i]		= 0;			//v4.40T2
	}

	m_lGenSummaryPeriodNum = 0;
	m_szLastGenSummaryTime = "";
	m_szBackupTempFilePath = "";				//v4.48A10

	m_bEnableBinMapBondArea	= FALSE;			//v4.03

	for (INT i = 0; i < BT_SUMMARY_TIME_NO; i++)
	{
		m_szBinSummaryGenTime[i] = "";
	}

	m_bIsBinMapLastRowDieExist = FALSE;

} //end constructor

CBinBlkMain::~CBinBlkMain()
{
}

VOID CBinBlkMain::SetBinTable(CBinTable* pBinTable)
{
	m_pBinTable = pBinTable;
}

LONG CBinBlkMain::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}

BOOL CBinBlkMain::IsUseBinMapBondArea(ULONG ulBlkToUse)
{
	CNGGrade *pNGGrade = CNGGrade::Instance();
	if (pNGGrade->IsNGBlock(ulBlkToUse) && !CMS896AApp::m_bNGWithBinMap)
	{
		return FALSE;
	}//1.1777
	return m_bEnableBinMapBondArea;
}

VOID CBinBlkMain::SetBinSerialDefault(BOOL bDisableBinSNRFormat, ULONG ulDefaultBinSNRFormat)
{	
	m_bDisableBinSNRFormat = bDisableBinSNRFormat;
	m_ulDefaultBinSNRFormat = ulDefaultBinSNRFormat;
}

VOID CBinBlkMain::SetClearCountDefault(BOOL bDisableClearCountFormat, ULONG ulDefaultClearCountFormat)
{
	m_bDisableClearCountFormat = bDisableClearCountFormat;
	m_ulDefaultClearCountFormat = ulDefaultClearCountFormat;
}

BOOL CBinBlkMain::IsDisableBinSNRFormat()
{
	return m_bDisableBinSNRFormat;
}

ULONG CBinBlkMain::GetDefaultBinSNRFormat()
{
	return m_ulDefaultBinSNRFormat;
}

BOOL CBinBlkMain::IsDisableClearCountFormat()
{
	return m_bDisableClearCountFormat;
}

ULONG CBinBlkMain::GetDefaultClearCountFormat()
{
	return m_ulDefaultClearCountFormat;
}

/***********************************/
/*        Get/Set functions        */
/***********************************/
ULONG CBinBlkMain::GetNoOfBlk()
{
	return m_ulNoOfBlk;
} //end GetNoOfBlk


BOOL CBinBlkMain::SetNoOfBlk(ULONG ulNoOfBlk)
{
	m_ulNoOfBlk = ulNoOfBlk;
	return TRUE;
} //end SetNoOfBlk


ULONG CBinBlkMain::GetBlkPitchX()
{
	return m_lBlkPitchX;
} //end GetBlkPitchX

BOOL CBinBlkMain::SetBlkPitchX(LONG lBlkPitchX)
{
	m_lBlkPitchX = lBlkPitchX;
	return TRUE;
} //end SetBlkPitchX


ULONG CBinBlkMain::GetBlkPitchY()
{
	return m_lBlkPitchY;
} //end GetBlkPitchY

BOOL CBinBlkMain::SetBlkPitchY(LONG lBlkPitchY)
{
	m_lBlkPitchY = lBlkPitchY;
	return TRUE;
} //end SetBlkPitchY

ULONG CBinBlkMain::GetMsgCode()
{
	return m_ulMsgCode;
} //end GetMsgCode

BOOL CBinBlkMain::SetMsgCode(ULONG ulMsgCode)
{
	m_ulMsgCode = ulMsgCode;
	return TRUE;
} //end SetMsgCode


VOID CBinBlkMain::SetNVRamPtr(void *pvNVRAM)
{
	if (pvNVRAM != NULL)
	{
		m_pvNVRAM = pvNVRAM;
		m_bUseHWNVRam	= TRUE;		//v4.65A1
	}
	else
	{
		m_bUseHWNVRam = FALSE;		//v4.65A1

		m_pvNVRAM = malloc((5 * sizeof(ULONG) + 2 * sizeof(LONG) + 3 * sizeof(BOOL)
							+ 1 * sizeof(DOUBLE)) * (BT_MAX_BINBLK_SIZE - 1));
	
		if (m_pvNVRAM != NULL)	//Klocwork
		{
			ULONG i;
			char *pTemp;
			pTemp = (char*) m_pvNVRAM;
			for (i = 0; i < (5 * sizeof(ULONG) + 2 * sizeof(LONG) + 3 * sizeof(BOOL)
							 + 1 * sizeof(DOUBLE)) * (BT_MAX_BINBLK_SIZE - 1); i++)
			{
				*(pTemp + i) = 0;
			}
		}
	}
} //end SetNVRamPtr


VOID CBinBlkMain::SetNVRamBinTableStart(LONG lNVRAM_BinTable_Start)
{
	m_lNVRAM_BinTable_Start = lNVRAM_BinTable_Start;
} //end SetNVRamBinTableStart

//v4.65A1
VOID CBinBlkMain::SetNVRamTempDataStart(void *pvNVRAM_HW, LONG lNVRAM_TEMPDATA_Start)	//v4.65A1
{
	if (pvNVRAM_HW == NULL)
	{
		m_pstNvTempFileData		= NULL;
		m_pstNvTempFileData_HW	= NULL;
		return;
	}

	m_pvNVRAM_HW			= pvNVRAM_HW;
	m_lNVRAM_TempData_Start = lNVRAM_TEMPDATA_Start;

	if (m_bUseHWNVRam)
	{
		//HW SRAM pointer
		BT_NVTEMPDATA *pTemp;
		pTemp = (BT_NVTEMPDATA*)((ULONG) m_pvNVRAM_HW + m_lNVRAM_TempData_Start);
		m_pstNvTempFileData_HW = pTemp;

		//SW SRAM pointer
		BT_NVTEMPDATA *pTemp2;
		pTemp2 = (BT_NVTEMPDATA*)((ULONG) m_pvNVRAM + m_lNVRAM_TempData_Start);
		m_pstNvTempFileData = pTemp2;
	}
}

BOOL CBinBlkMain::ResetNVTempFileData()
{
	if (m_pstNvTempFileData == NULL)
	{
		return FALSE;
	}
	if (!m_bUseHWNVRam)
	{
		return FALSE;
	}

	CString szLog;
	szLog.Format("BT GenTempFile RESET NV TEMPDATA: InUse=%ld, lBlock=%ld, INDEX=%ld", 
		m_pstNvTempFileData->lInUse, m_pstNvTempFileData->lBlock, m_pstNvTempFileData->lIndex);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	m_pstNvTempFileData->lInUse				= 0;
	m_pstNvTempFileData->lBlock				= 0;
	m_pstNvTempFileData->lIndex				= 0;
	
	m_pstNvTempFileData_HW->lInUse			= 0;
	m_pstNvTempFileData_HW->lBlock			= 0;
	m_pstNvTempFileData_HW->lIndex			= 0;

	for (INT i = 0; i < 500; i++)
	{
		m_pstNvTempFileData->lWaferMapRow[i]	= 0;
		m_pstNvTempFileData->lWaferMapCol[i]	= 0;
		m_pstNvTempFileData->lBinIndex[i]		= 0;

		m_pstNvTempFileData_HW->lWaferMapRow[i]	= 0;
		m_pstNvTempFileData_HW->lWaferMapCol[i]	= 0;
		m_pstNvTempFileData_HW->lBinIndex[i]	= 0;
	}
	return TRUE;
}

BOOL CBinBlkMain::SetNVTempFileData(LONG lBlock, LONG lMapRow, LONG lMapCol, LONG lBinIndex)
{
	if (m_pstNvTempFileData == NULL)
	{
		return FALSE;
	}
	if (!m_bUseHWNVRam)
	{
		return FALSE;
	}

	if (m_ulBondCountToGenTempFile > 500)
	{
		return FALSE;
	}

	//typedef struct
	//{
	//	LONG lInUse;
	//	LONG lGrade;
	//	LONG lIndex;
	//	LONG lWaferMapRow[500];
	//	LONG lWaferMapCol[500];
	//} BT_NVTEMPDATA;

	LONG lCurrIndex = m_pstNvTempFileData->lIndex;		//after RESET this starts from ZERO
	lCurrIndex = lCurrIndex + 1;						//1-based Die Index from 1 to 500;
	
	if (lCurrIndex > (LONG)m_ulBondCountToGenTempFile)
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("BT SetNVTempFileData: FAIL 1");
		return FALSE;
	}
	if (lCurrIndex > 500)
	{
		//CMSLogFileUtility::Instance()->MS_LogOperation("BT SetNVTempFileData: FAIL 2");
		return FALSE;
	}


	LONG lUserCol = 0, lUserRow = 0;
	USHORT usMapAngle = CMS896AStn::m_usMapAngle;
	BOOL bHFlip = CMS896AStn::m_bMapOHFlip;
	BOOL bVFlip = CMS896AStn::m_bMapOVFlip;
	CMS896AStn::m_WaferMapWrapper.ConvertInternalToOriginalUser((ULONG)lMapRow, (ULONG)lMapCol, bHFlip, bVFlip, usMapAngle, lUserRow,lUserCol);

	m_pstNvTempFileData_HW->lInUse							= 1;
	m_pstNvTempFileData_HW->lBlock							= lBlock;
	m_pstNvTempFileData_HW->lIndex							= lCurrIndex;	//from 1-based Index to 0-based array, so need to minus 1
	m_pstNvTempFileData_HW->lWaferMapRow[lCurrIndex - 1]	= lUserRow;		//lMapRow;
	m_pstNvTempFileData_HW->lWaferMapCol[lCurrIndex - 1]	= lUserCol;		//lMapCol;
	m_pstNvTempFileData_HW->lBinIndex[lCurrIndex - 1]		= lBinIndex;

	m_pstNvTempFileData->lInUse								= 1;
	m_pstNvTempFileData->lBlock								= lBlock;
	m_pstNvTempFileData->lIndex								= lCurrIndex;	//from 1-based Index to 0-based array, so need to minus 1
	m_pstNvTempFileData->lWaferMapRow[lCurrIndex - 1]		= lUserRow;		//lMapRow;
	m_pstNvTempFileData->lWaferMapCol[lCurrIndex - 1]		= lUserCol;		//lMapCol;
	m_pstNvTempFileData->lBinIndex[lCurrIndex - 1]			= lBinIndex;

	CString szLog;
	szLog.Format("BT SetNVTempFileData: InUse=%ld, lBlock=%ld, INDEX=%ld, MAP(%ld, %ld), BinIndex=%ld", 
		m_pstNvTempFileData->lInUse, m_pstNvTempFileData->lBlock, m_pstNvTempFileData->lIndex,
		m_pstNvTempFileData->lWaferMapRow[lCurrIndex - 1], m_pstNvTempFileData->lWaferMapCol[lCurrIndex - 1],
		lBinIndex);
	//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	return TRUE;
} 

		
BOOL CBinBlkMain::IsNVTempDataNeedToRestoreAtWarmStart(LONG &lBlock, LONG &lDieCount)		//v4.65A1
{
	if (m_pstNvTempFileData == NULL)
	{
		return FALSE;
	}
	if (!m_bUseHWNVRam)
	{
		return FALSE;
	}

	LONG lIsNVRamInUse = m_pstNvTempFileData->lInUse;
	LONG lIsNVRamBlock = m_pstNvTempFileData->lBlock;

	if ( (lIsNVRamInUse == 1) && (lIsNVRamBlock > 0) && (lIsNVRamBlock <= BT_MAX_BINBLK_NO) )
	{
		lBlock		= lIsNVRamBlock;
		lDieCount	= m_pstNvTempFileData->lIndex;
		return TRUE;
	}

	return FALSE;
}

BOOL CBinBlkMain::RestoreNVTempFileDataAtWarmStart()		//v4.65A1
{
	if (m_pstNvTempFileData == NULL)
	{
		return FALSE;
	}
	if (!m_bUseHWNVRam)
	{
		return FALSE;
	}

	BOOL bStatus = TRUE;

	LONG lIsNVRamInUse = m_pstNvTempFileData->lInUse;
	LONG lIsNVRamBlock = m_pstNvTempFileData->lBlock;

	if ( (lIsNVRamInUse == 1) && (lIsNVRamBlock > 0) && (lIsNVRamBlock <= BT_MAX_BINBLK_NO) )
	{
		//bStatus = m_oBinBlk[i].GenTempFile(i, m_bFilenameASWaferID, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bAddSortingSequence, 
		//								bHasUpdatedTempFile , bUseNVData, CMS896AStn::m_bForceDisableHaveOtherFile);
	
		bStatus = m_oBinBlk[lIsNVRamBlock].GenTempFileFromNVRAM(m_pstNvTempFileData, lIsNVRamBlock, m_bFilenameASWaferID, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bAddSortingSequence);
	}

	return bStatus;
}

/***********************************/
/*    Init from file functions     */
/***********************************/

BOOL CBinBlkMain::InitBinBlkMainData(BOOL bLoadClearCount)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	//Save data values from config file into structure
	
	if ((!((*pBTfile)["BinBlock"]["NoOfBlk"])) == FALSE)
	{
		m_ulNoOfBlk = (*pBTfile)["BinBlock"]["NoOfBlk"];
	}
	else
	{
		m_ulNoOfBlk = 0;
	}

	if ((!((*pBTfile)["BinBlock"]["BlkPitchX"])) == FALSE)
	{
		m_lBlkPitchX = (*pBTfile)["BinBlock"]["BlkPitchX"];
	}
	else
	{
		m_lBlkPitchX = 0;
	}

	if ((!((*pBTfile)["BinBlock"]["BlkPitchY"])) == FALSE)
	{
		m_lBlkPitchY = (*pBTfile)["BinBlock"]["BlkPitchY"];
	}
	else
	{
		m_lBlkPitchY = 0;
	}

	if ((!((*pBTfile)["BinBlock"]["LifeTime"])) == FALSE)
	{
		m_szLifeTime = (*pBTfile)["BinBlock"]["LifeTime"];
	}
	else
	{
		m_szLifeTime = "";
	}

	if ((!((*pBTfile)["BinBlock"]["ESD"])) == FALSE)
	{
		m_szESD = (*pBTfile)["BinBlock"]["ESD"];
	}
	else
	{
		m_szESD = "";
	}

	if ((!((*pBTfile)["BinBlock"]["IF"])) == FALSE)
	{
		m_szIF = (*pBTfile)["BinBlock"]["IF"];
	}
	else
	{
		m_szIF = "";
	}

	if ((!((*pBTfile)["BinBlock"]["TapeID"])) == FALSE)
	{
		m_szTapeID = (*pBTfile)["BinBlock"]["TapeID"];
	}
	else
	{
		m_szTapeID = "";
	}

	if ((!((*pBTfile)["BinBlock"]["BinTableFilename"])) == FALSE)
	{
		m_szBinTableFilename = (*pBTfile)["BinBlock"]["BinTableFilename"];
	}
	else
	{
		m_szBinTableFilename = "";
	}
	
	if (bLoadClearCount == TRUE)
	{
		m_ulSerialNoFormat = (*pBTfile)["BinBlock"]["SerialNoFormat"];
		
		if (m_ulSerialNoFormat == 0)
		{
			m_ulSerialNoFormat = 1;
		}

		m_ulSerialNoMaxLimit = (*pBTfile)["BinBlock"]["SerialNoMaxLimit"];
		
		if (m_ulSerialNoMaxLimit == 0)
		{
			m_ulSerialNoMaxLimit = 9999;
		}

		m_ulBinClearFormat = (*pBTfile)["BinBlock"]["Bin Clear Format"];

		m_ulBinClearedCount = (*pBTfile)["BinBlock"]["Bin Clear Count"];

		if (m_ulBinClearedCount == 0)
		{
			m_ulBinClearedCount = 1;
		}

		m_szBinClearedDate = (*pBTfile)["BinBlock"]["Bin Clear Date"];

		m_ulBinClearInitCount = (*pBTfile)["BinBlock"]["Bin Clear InitCount"];

		if (m_ulBinClearInitCount == 0)
		{
			m_ulBinClearInitCount = 1;
		}

		m_ulBinClearMaxLimit = (*pBTfile)["BinBlock"]["Bin Clear MaxLimit"];
		
		if (m_ulBinClearMaxLimit == 0)
		{
			m_ulBinClearMaxLimit = 99;
		}
	}

	m_bStoreBinBondedCount = (BOOL)((LONG)(*pBTfile)["BinBlock"]["Store Acc. Bonded Count"]);
	m_bEnableAutoAssignGrade = (BOOL)((LONG)(*pBTfile)["BinBlock"]["Enable Auto Ass. Grade"]);
	
	m_bEnableLSBondPattern = (BOOL)((LONG)(*pBTfile)["BinBlock"]["Enable LS Pattern Bond"]);
	m_bEnableNVRunTimeData = (BOOL)((LONG)(*pBTfile)["BinBlock"]["Enable NVProtect"]);

	CMSFileUtility::Instance()->CloseBTConfig();

	return TRUE;
} //end InitBinBlkMainData


BOOL CBinBlkMain::InitBinBlkData(ULONG ulBlkId, BOOL bLoadSNR, BOOL bCloseFile)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	LONG lUpperLeftX, lUpperLeftY, lLowerRightX, lLowerRightY;
	LONG lUpperLeftXFromSetup, lUpperLeftYFromSetup, lLowerRightXFromSetup, lLowerRightYFromSetup;
	UCHAR ucGrade, ucOrgGrade, ucStatus;
//	LONG lDiePitchX, lDiePitchY;
	DOUBLE dDiePitchX, dDiePitchY;
	ULONG ulWalkPath, ulSkipUnit, ulAccCount, ulMaxUnit, ulEmptyUnit;
	ULONG ulDiePerBlk = 0, ulDiePerRow = 0, ulDiePerCol = 0;		//Klocwork
	BOOL bIsDisableFromSameGradeMerge, bIsSetup;
	ULONG ulIsDisableFromSameGradeMerge, ulIsSetup, ulPhyBlkId, ulSerialNo;
	CString szLastResetSerialNoDate;
	CString szStartDate;
	BOOL	bEnableFirstRowColSkipPattern = FALSE;
	ULONG	ulFirstRowColSkipUnit = 0;
	BOOL	bCentralizedBondArea;
	LONG	lCentralizedOffsetX, lCentralizedOffsetY;
	BOOL	bEnableWafflePad;
	LONG	lWafflePadDistX, lWafflePadDistY, lWafflePadSizeX, lWafflePadSizeY;
	BOOL	bUseBlockCornerAsFirstDiePos;

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	if (ulBlkId >= BT_MAX_BINBLK_SIZE)	//v4.46T20	//Klocwork
	{
		CMSFileUtility::Instance()->CloseBTConfig();	//v4.47T1
		return FALSE;
	}


	//Save data values from smf to structure
	if (((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftX"])) == FALSE)
			&&
			((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightX"])) == FALSE))
	{
		lUpperLeftX = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftX"];
		lLowerRightX = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightX"];
	}
	else
	{
		lUpperLeftX = 0;
		lLowerRightX = 0;
	}

	if (((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftY"])) == FALSE)
			&&
			((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightY"])) == FALSE))
	{
		lUpperLeftY = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftY"];
		lLowerRightY = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightY"];
	}
	else
	{
		lUpperLeftY = 0;
		lLowerRightY = 0;
	}

	m_oBinBlk[ulBlkId].SetUpperLeftX(lUpperLeftX);
	m_oBinBlk[ulBlkId].SetLowerRightX(lLowerRightX);
	m_oBinBlk[ulBlkId].SetWidth();
	m_oBinBlk[ulBlkId].SetUpperLeftY(lUpperLeftY);
	m_oBinBlk[ulBlkId].SetLowerRightY(lLowerRightY);
	m_oBinBlk[ulBlkId].SetHeight();

	if	(((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftXFromSetup"])) == FALSE)
			&&
			((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightXFromSetup"])) == FALSE))
	{
		lUpperLeftXFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftXFromSetup"];
		lLowerRightXFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightXFromSetup"];
	}
	else
	{
		lUpperLeftXFromSetup = 0;
		lLowerRightXFromSetup = 0;
	}

	if	(((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftYFromSetup"])) == FALSE)
			&&
			((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightYFromSetup"])) == FALSE))
	{
		lUpperLeftYFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftYFromSetup"];
		lLowerRightYFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightYFromSetup"];
	}
	else
	{
		lUpperLeftYFromSetup = 0;
		lLowerRightYFromSetup = 0;
	}

	m_oBinBlk[ulBlkId].SetUpperLeftXFromSetup(lUpperLeftXFromSetup);
	m_oBinBlk[ulBlkId].SetLowerRightXFromSetup(lLowerRightXFromSetup);
	m_oBinBlk[ulBlkId].SetWidthFromSetup();
	m_oBinBlk[ulBlkId].SetUpperLeftYFromSetup(lUpperLeftYFromSetup);
	m_oBinBlk[ulBlkId].SetLowerRightYFromSetup(lLowerRightYFromSetup);
	m_oBinBlk[ulBlkId].SetHeightFromSetup();

	if	((!((*pBTfile)["BinBlock"][ulBlkId]["Use Block Corner As First Die Pos"])) == FALSE)
	{
		bUseBlockCornerAsFirstDiePos = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Use Block Corner As First Die Pos"];
	}
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		bUseBlockCornerAsFirstDiePos = FALSE;
	}
	m_oBinBlk[ulBlkId].SetIsUseBlockCornerAsFirstDiePos(bUseBlockCornerAsFirstDiePos);

	if ((!((*pBTfile)["BinBlock"][ulBlkId]["Grade"])) == FALSE)
	{
		ucGrade = (*pBTfile)["BinBlock"][ulBlkId]["Grade"];
	}
	else
	{
		ucGrade = 1;
	}
	m_oBinBlk[ulBlkId].SetGrade(ucGrade);

	if ((!((*pBTfile)["BinBlock"][ulBlkId]["Original Grade"])) == FALSE)
	{
		ucOrgGrade = (*pBTfile)["BinBlock"][ulBlkId]["Original Grade"];
	}
	else
	{	
		ucOrgGrade = ucGrade;
	}
	m_oBinBlk[ulBlkId].SetOriginalGrade(ucOrgGrade);


	if ((!((*pBTfile)["BinBlock"][ulBlkId]["DiePitchX"])) == FALSE)
	{
		dDiePitchX = (*pBTfile)["BinBlock"][ulBlkId]["DiePitchX"];
	}
	else
	{
		dDiePitchX = 500;	//default
	}

	m_oBinBlk[ulBlkId].SetDDiePitchX(dDiePitchX);

	if ((!((*pBTfile)["BinBlock"][ulBlkId]["DiePitchY"])) == FALSE)
	{
		dDiePitchY = (*pBTfile)["BinBlock"][ulBlkId]["DiePitchY"];
	}
	else
	{
		dDiePitchY = 500;	//default
	}

	m_oBinBlk[ulBlkId].SetDDiePitchY(dDiePitchY);			//v4.59A19

	//v4.42T5
	BOOL bPt5DiePitchX = FALSE;
	if ((!((*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchX"])) == FALSE)
	{
		bPt5DiePitchX = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchX"];
	}
	else
	{
		bPt5DiePitchX = FALSE;	//default
	}
	m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchX(bPt5DiePitchX);

	//v4.42T5
	BOOL bPt5DiePitchY = FALSE;
	if ((!((*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchY"])) == FALSE)
	{
		bPt5DiePitchY = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchY"];
	}
	else
	{
		bPt5DiePitchY = FALSE;	//default
	}
	m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchY(bPt5DiePitchY);

	//v4.48A11	//SEmitek, 3E DL
	BOOL bByPassBinMap = FALSE;
	bByPassBinMap = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["ByPassBinMap"];
	m_oBinBlk[ulBlkId].SetByPassBinMap(bByPassBinMap);

	//v4.42T9
	DOUBLE dOffsetX = 0, dOffsetY=0;
	if ((!((*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetX"])) == FALSE)
	{
		dOffsetX = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetX"];
	}
	else
	{
		dOffsetX = 0;	//default
	}
	if ((!((*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetY"])) == FALSE)
	{
		dOffsetY = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetY"];
	}
	else
	{
		dOffsetY = 0;	//default
	}	
	m_oBinBlk[ulBlkId].SetBondAreaOffset(dOffsetX, dOffsetY);

	//v4.59A22
	dOffsetX = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["ThermalDeltaPitchX"];
	dOffsetY = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["ThermalDeltaPitchY"];
	m_oBinBlk[ulBlkId].SetThermalDeltaPitch(dOffsetX, dOffsetY);


	if ((!((*pBTfile)["BinBlock"][ulBlkId]["WalkPath"])) == FALSE)
	{
		ulWalkPath = (*pBTfile)["BinBlock"][ulBlkId]["WalkPath"];
	}
	else
	{
		ulWalkPath = 0;
	}
	m_oBinBlk[ulBlkId].SetWalkPath(ulWalkPath);

	if (((!((*pBTfile)["BinBlock"][ulBlkId]["DiePerBlk"])) == FALSE)
			&&
			((!((*pBTfile)["BinBlock"][ulBlkId]["DiePerRow"])) == FALSE))
	{
		ulDiePerBlk = (*pBTfile)["BinBlock"][ulBlkId]["DiePerBlk"];
		ulDiePerRow = (*pBTfile)["BinBlock"][ulBlkId]["DiePerRow"];
		ulDiePerCol = (*pBTfile)["BinBlock"][ulBlkId]["DiePerCol"];		//pllm
	}
	else
	{
		ulDiePerBlk = 0;
		ulDiePerRow = 0;
	}

	//v3.70T3		//PLLM
	BOOL bUseCircularBinArea	= (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Use Circular Bin Area"];
	LONG lRadius				= (LONG)(*pBTfile)["BinBlock"][ulBlkId]["Circular Bin Radius"];
	LONG lCenterX				= (LONG)(*pBTfile)["BinBlock"][ulBlkId]["Circular Bin CenterX"];
	LONG lCenterY				= (LONG)(*pBTfile)["BinBlock"][ulBlkId]["Circular Bin CenterY"];
	m_oBinBlk[ulBlkId].SetUseCircularArea(bUseCircularBinArea, lRadius);
	m_oBinBlk[ulBlkId].SetCirBinCenterXY(lCenterX, lCenterY);

	//v4.42T6
	m_oBinBlk[ulBlkId].m_bEnableBinMap = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Enable"];
	BOOL bEnable = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Enable Offset"];
	LONG lBinMapRowOffset = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Row Offset"];
	LONG lBinMapColOffset = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Col Offset"];
	m_oBinBlk[ulBlkId].SetBinMapOffset(bEnable, lBinMapRowOffset, lBinMapColOffset);
	//v4.42T11
	ULONG ulBinMapWalkPath	= (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Walk Path"];
	m_oBinBlk[ulBlkId].SetBinMapWalkPath(ulBinMapWalkPath);
	DOUBLE dCirRadius		= (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["CirRadius"];
	m_oBinBlk[ulBlkId].SetBinMapCircleRadius(dCirRadius);
	LONG lTEdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Top Edge"];
	LONG lBEdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Btm Edge"];
	LONG lLEdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Left Edge"];
	LONG lREdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Right Edge"];
	m_oBinBlk[ulBlkId].SetBinMapEdgeSize(lTEdge, lBEdge, lLEdge, lREdge);

	//v4.30T4		//CyOptics
	m_bLeaveEmptyRow[ulBlkId]	= (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["LeaveEmptyRow"];
	m_ul1stRowIndex[ulBlkId]	= (*pBTfile)["BinBlock"][ulBlkId]["1stRowIndex"];	//v4.40T2

	if (ulDiePerRow == 0)
	{
		if (ulDiePerCol != 0)
		{
			ulDiePerRow = ulDiePerBlk / ulDiePerCol;
		}
	}
	else if (ulDiePerCol == 0)
	{
		if (ulDiePerRow != 0)
		{
			ulDiePerCol = ulDiePerBlk / ulDiePerRow;
		}
	}

	m_oBinBlk[ulBlkId].SetDiePerBlk(ulDiePerBlk);
	m_oBinBlk[ulBlkId].SetDiePerRow(ulDiePerRow);
	m_oBinBlk[ulBlkId].SetDiePerCol(ulDiePerCol);

	m_oBinBlk[ulBlkId].CalculateDieInfoDbCapacity(ulDiePerBlk, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime);
	
	if ((!((*pBTfile)["BinBlock"][ulBlkId]["IsDisableFromSameGradeMerge"])) == FALSE)
	{
		ulIsDisableFromSameGradeMerge = (*pBTfile)["BinBlock"][ulBlkId]["IsDisableFromSameGradeMerge"];
	}
	else
	{
		ulIsDisableFromSameGradeMerge = 0;
	}
	bIsDisableFromSameGradeMerge = BOOL(ulIsDisableFromSameGradeMerge);
	m_oBinBlk[ulBlkId].SetIsDisableFromSameGradeMerge(bIsDisableFromSameGradeMerge);

	if ((!((*pBTfile)["BinBlock"][ulBlkId]["PhyBlkId"])) == FALSE)
	{
		ulPhyBlkId = (*pBTfile)["BinBlock"][ulBlkId]["PhyBlkId"];
	}
	else
	{
		ulPhyBlkId = 0;
	}
	m_oBinBlk[ulBlkId].SetPhyBlkId(ulPhyBlkId);

	if ((!((*pBTfile)["BinBlock"][ulBlkId]["IsSetup"])) == FALSE)
	{
		ulIsSetup = (*pBTfile)["BinBlock"][ulBlkId]["IsSetup"];
	}
	else
	{
		ulIsSetup = 0;
	}
	bIsSetup = (BOOL)ulIsSetup;
	m_oBinBlk[ulBlkId].SetIsSetup(bIsSetup);

	if ((!((*pBTfile)["BinBlock"][ulBlkId]["Status"])) == FALSE)
	{
		ucStatus = (*pBTfile)["BinBlock"][ulBlkId]["Status"];
	}
	else
	{
		ucStatus = BT_BLK_NORMAL;
	}
	m_oBinBlk[ulBlkId].SetStatus(ucStatus);

	if (bLoadSNR == TRUE)
	{
		ulSerialNo = (*pBTfile)["BinBlock"][ulBlkId]["SerialNo"];
		m_oBinBlk[ulBlkId].SetSerialNo(ulSerialNo);

		szLastResetSerialNoDate = (*pBTfile)["BinBlock"][ulBlkId]["LastResetSerialNoDate"];
		m_oBinBlk[ulBlkId].SetLastResetSerialNoDate(szLastResetSerialNoDate);

		if (ulBlkId == 1)
		{
			CMSLogFileUtility::Instance()->BT_BinSerialLog("Start Load SNR");
		}

		CString szMsg;
		szMsg.Format("Init Load SNR - Blk%d date:%s  serial:%d", ulBlkId , szLastResetSerialNoDate, ulSerialNo);
		CMSLogFileUtility::Instance()->BT_BinSerialLog(szMsg);
	}

	if ((!((*pBTfile)["BinBlock"][ulBlkId]["SkipUnit"])) == FALSE)
	{
		ulSkipUnit = (*pBTfile)["BinBlock"][ulBlkId]["SkipUnit"];
	}
	else
	{
		ulSkipUnit = 0;
	}
	m_oBinBlk[ulBlkId].SetSkipUnit(ulSkipUnit);


	if ((!((*pBTfile)["BinBlock"][ulBlkId]["MaxUnit"])) == FALSE)
	{
		ulMaxUnit = (*pBTfile)["BinBlock"][ulBlkId]["MaxUnit"];
	}
	else
	{
		ulMaxUnit = 0;
	}
	m_oBinBlk[ulBlkId].SetMaxUnit(ulMaxUnit);


	if ((!((*pBTfile)["BinBlock"][ulBlkId]["StartDate"])) == FALSE)
	{
		szStartDate = (*pBTfile)["BinBlock"][ulBlkId]["StartDate"];
	}
	else
	{
		szStartDate = "";
	}
	m_oBinBlk[ulBlkId].SetStartDate(szStartDate);


	if ((!((*pBTfile)["BinBlock"][ulBlkId]["Accumulated"])) == FALSE)
	{
		ulAccCount = (*pBTfile)["BinBlock"][ulBlkId]["Accumulated"];
	}
	else
	{
		ulAccCount = 0;
	}
	m_oBinBlk[ulBlkId].SetAccmulatedCount(ulAccCount);


	if ((!((*pBTfile)["BinBlock"][ulBlkId]["EmptyUnit"])) == FALSE)
	{
		ulEmptyUnit = (*pBTfile)["BinBlock"][ulBlkId]["EmptyUnit"];
	}
	else
	{
		ulEmptyUnit = 0;
	}
	m_oBinBlk[ulBlkId].SetEmptyUnit(ulEmptyUnit);

	bEnableFirstRowColSkipPattern = FALSE;
	if (!((*pBTfile)["BinBlock"][ulBlkId]["Enbale FRSP"]) == FALSE)
	{
		bEnableFirstRowColSkipPattern = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Enbale FRSP"];
	}
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		bEnableFirstRowColSkipPattern = FALSE;
	}
	m_oBinBlk[ulBlkId].SetFirstRowColSkipPattern(bEnableFirstRowColSkipPattern);

	ulFirstRowColSkipUnit = 0;
	if (!(((*pBTfile)["BinBlock"][ulBlkId]["FRSP Skip Unit"])) == FALSE)
	{
		ulFirstRowColSkipUnit = (ULONG)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["FRSP Skip Unit"];
	}
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		ulFirstRowColSkipUnit = 0;
	}
	m_oBinBlk[ulBlkId].SetFirstRowColSkipUnit(ulFirstRowColSkipUnit);


	//Centralized Bond Area
	bCentralizedBondArea = FALSE;
	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Enbale Centralized"])) == FALSE)
	{
		bCentralizedBondArea = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Enbale Centralized"];
	}
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		bCentralizedBondArea = TRUE;
	}
	m_oBinBlk[ulBlkId].SetIsCentralizedBondArea(bCentralizedBondArea);
	
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		//m_oBinBlk[ulBlkId].SetTeachWithPhysicalBlk(TRUE);
	}

	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset X"])) == FALSE)
	{
		lCentralizedOffsetX = (ULONG)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset X"];
	}
	else
	{
		lCentralizedOffsetX = 0;
	}
	m_oBinBlk[ulBlkId].SetCentralizedOffsetX(lCentralizedOffsetX);


	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset Y"])) == FALSE)
	{
		lCentralizedOffsetY = (ULONG)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset Y"];
	}
	else
	{
		lCentralizedOffsetY = 0;
	}
	m_oBinBlk[ulBlkId].SetCentralizedOffsetY(lCentralizedOffsetY);


	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Enbale Waffle Pad"])) == FALSE)
	{
		bEnableWafflePad = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Enbale Waffle Pad"];
	}
	else
	{
		bEnableWafflePad = FALSE;
	}

	m_oBinBlk[ulBlkId].SetEnableWafflePad(bEnableWafflePad);

	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist X"])) == FALSE)
	{
		lWafflePadDistX = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist X"];
	}
	else
	{
		lWafflePadDistX = 0;
	}

	m_oBinBlk[ulBlkId].SetWafflePadDistX(lWafflePadDistX);

	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist Y"])) == FALSE)
	{
		lWafflePadDistY = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist Y"];
	}
	else
	{
		lWafflePadDistY = 0;
	}

	m_oBinBlk[ulBlkId].SetWafflePadDistY(lWafflePadDistY);

	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size X"])) == FALSE)
	{
		lWafflePadSizeX = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size X"];
	}
	else
	{
		lWafflePadSizeX = 0;
	}

	m_oBinBlk[ulBlkId].SetWafflePadSizeX(lWafflePadSizeX);

	if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size Y"])) == FALSE)
	{
		lWafflePadSizeY = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size Y"];
	}
	else
	{
		lWafflePadSizeY = 0;
	}
	m_oBinBlk[ulBlkId].SetWafflePadSizeY(lWafflePadSizeY);

	//v4.42T1	//Nichia
	BOOL bNoReturnTravel = FALSE;
	if (!(((*pBTfile)["BinBlock"][ulBlkId]["No Return Travel"])) == FALSE)
	{
		bNoReturnTravel = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["No Return Travel"];
	}
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		//bNoReturnTravel = FALSE;
	}
	m_oBinBlk[ulBlkId].SetNoReturnTravel(bNoReturnTravel);

	//if (bCloseFile)		//v4.46T15	//v4.47T1
	//{
	CMSFileUtility::Instance()->CloseBTConfig();
	//}
	return TRUE;  
} //end InitBinBlkData

BOOL CBinBlkMain::InitAllBinBlkData(BOOL bLoadSNR)		//v4.46T15
{
	//v4.47T2
/*
	ULONG ulBlkID = 1;
	for (ulBlkID=1; ulBlkID<=GetNoOfBlk(); ulBlkID++)
	{
		if (ulBlkID == GetNoOfBlk())
			InitBinBlkData(ulBlkID, bLoadSNR);
		else
			InitBinBlkData(ulBlkID, bLoadSNR, FALSE);
	}
*/
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	LONG lUpperLeftX, lUpperLeftY, lLowerRightX, lLowerRightY;
	LONG lUpperLeftXFromSetup, lUpperLeftYFromSetup, lLowerRightXFromSetup, lLowerRightYFromSetup;
	UCHAR ucGrade, ucOrgGrade, ucStatus;
//	LONG lDiePitchX, lDiePitchY;
	DOUBLE dDiePitchX, dDiePitchY;
	ULONG ulWalkPath, ulSkipUnit, ulAccCount, ulMaxUnit, ulEmptyUnit;
	ULONG ulDiePerBlk = 0, ulDiePerRow = 0, ulDiePerCol = 0;		//Klocwork
	BOOL bIsDisableFromSameGradeMerge, bIsSetup;
	ULONG ulIsDisableFromSameGradeMerge, ulIsSetup, ulPhyBlkId, ulSerialNo;
	CString szLastResetSerialNoDate;
	CString szStartDate;
	BOOL	bEnableFirstRowColSkipPattern = FALSE;
	ULONG	ulFirstRowColSkipUnit = 0;
	BOOL	bCentralizedBondArea;
	LONG	lCentralizedOffsetX, lCentralizedOffsetY;
	BOOL	bEnableWafflePad;
	LONG	lWafflePadDistX, lWafflePadDistY, lWafflePadSizeX, lWafflePadSizeY;
	BOOL	bUseBlockCornerAsFirstDiePos;


	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}


	ULONG ulBlkId = 1;
	for (ulBlkId=1; ulBlkId<=GetNoOfBlk(); ulBlkId++)
	{
		//Save data values from smf to structure
		if (((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftX"])) == FALSE)
				&&
				((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightX"])) == FALSE))
		{
			lUpperLeftX = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftX"];
			lLowerRightX = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightX"];
		}
		else
		{
			lUpperLeftX = 0;
			lLowerRightX = 0;
		}

		if (((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftY"])) == FALSE)
				&&
				((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightY"])) == FALSE))
		{
			lUpperLeftY = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftY"];
			lLowerRightY = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightY"];
		}
		else
		{
			lUpperLeftY = 0;
			lLowerRightY = 0;
		}

		m_oBinBlk[ulBlkId].SetUpperLeftX(lUpperLeftX);
		m_oBinBlk[ulBlkId].SetLowerRightX(lLowerRightX);
		m_oBinBlk[ulBlkId].SetWidth();
		m_oBinBlk[ulBlkId].SetUpperLeftY(lUpperLeftY);
		m_oBinBlk[ulBlkId].SetLowerRightY(lLowerRightY);
		m_oBinBlk[ulBlkId].SetHeight();

		if	(((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftXFromSetup"])) == FALSE)
				&&
				((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightXFromSetup"])) == FALSE))
		{
			lUpperLeftXFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftXFromSetup"];
			lLowerRightXFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightXFromSetup"];
		}
		else
		{
			lUpperLeftXFromSetup = 0;
			lLowerRightXFromSetup = 0;
		}

		if	(((!((*pBTfile)["BinBlock"][ulBlkId]["UpperLeftYFromSetup"])) == FALSE)
				&&
				((!((*pBTfile)["BinBlock"][ulBlkId]["LowerRightYFromSetup"])) == FALSE))
		{
			lUpperLeftYFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["UpperLeftYFromSetup"];
			lLowerRightYFromSetup = (*pBTfile)["BinBlock"][ulBlkId]["LowerRightYFromSetup"];
		}
		else
		{
			lUpperLeftYFromSetup = 0;
			lLowerRightYFromSetup = 0;
		}

		m_oBinBlk[ulBlkId].SetUpperLeftXFromSetup(lUpperLeftXFromSetup);
		m_oBinBlk[ulBlkId].SetLowerRightXFromSetup(lLowerRightXFromSetup);
		m_oBinBlk[ulBlkId].SetWidthFromSetup();
		m_oBinBlk[ulBlkId].SetUpperLeftYFromSetup(lUpperLeftYFromSetup);
		m_oBinBlk[ulBlkId].SetLowerRightYFromSetup(lLowerRightYFromSetup);
		m_oBinBlk[ulBlkId].SetHeightFromSetup();

		bUseBlockCornerAsFirstDiePos = FALSE;
		if	((!((*pBTfile)["BinBlock"][ulBlkId]["Use Block Corner As First Die Pos"])) == FALSE)
		{
			bUseBlockCornerAsFirstDiePos = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Use Block Corner As First Die Pos"];
		}
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			bUseBlockCornerAsFirstDiePos = FALSE;
		}
		m_oBinBlk[ulBlkId].SetIsUseBlockCornerAsFirstDiePos(bUseBlockCornerAsFirstDiePos);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["Grade"])) == FALSE)
		{
			ucGrade = (*pBTfile)["BinBlock"][ulBlkId]["Grade"];
		}
		else
		{
			ucGrade = 1;
		}
		m_oBinBlk[ulBlkId].SetGrade(ucGrade);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["Original Grade"])) == FALSE)
		{
			ucOrgGrade = (*pBTfile)["BinBlock"][ulBlkId]["Original Grade"];
		}
		else
		{	
			ucOrgGrade = ucGrade;
		}
		m_oBinBlk[ulBlkId].SetOriginalGrade(ucOrgGrade);


		if ((!((*pBTfile)["BinBlock"][ulBlkId]["DiePitchX"])) == FALSE)
		{
			dDiePitchX = (*pBTfile)["BinBlock"][ulBlkId]["DiePitchX"];
		}
		else
		{
			dDiePitchX = 500;	//default
		}

		m_oBinBlk[ulBlkId].SetDDiePitchX(dDiePitchX);			//v4.59A19

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["DiePitchY"])) == FALSE)
		{
			dDiePitchY = (*pBTfile)["BinBlock"][ulBlkId]["DiePitchY"];
		}
		else
		{
			dDiePitchY = 500;	//default
		}

		m_oBinBlk[ulBlkId].SetDDiePitchY(dDiePitchY);

		//v4.42T5
		BOOL bPt5DiePitchX = FALSE;
		if ((!((*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchX"])) == FALSE)
		{
			bPt5DiePitchX = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchX"];
		}
		else
		{
			bPt5DiePitchX = FALSE;	//default
		}
		m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchX(bPt5DiePitchX);

		//v4.42T5
		BOOL bPt5DiePitchY = FALSE;
		if ((!((*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchY"])) == FALSE)
		{
			bPt5DiePitchY = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Pt5DiePitchY"];
		}
		else
		{
			bPt5DiePitchY = FALSE;	//default
		}
		m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchY(bPt5DiePitchY);

		//v4.48A11	//Semitek, 3E DL
		BOOL bByPassBinMap = FALSE;
		bByPassBinMap = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["ByPassBinMap"];
		m_oBinBlk[ulBlkId].SetByPassBinMap(bByPassBinMap);

		//v4.42T9
		DOUBLE dOffsetX = 0, dOffsetY=0;
		if ((!((*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetX"])) == FALSE)
		{
			dOffsetX = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetX"];
		}
		else
		{
			dOffsetX = 0;	//default
		}
		if ((!((*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetY"])) == FALSE)
		{
			dOffsetY = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["BondAreaOffsetY"];
		}
		else
		{
			dOffsetY = 0;	//default
		}	
		m_oBinBlk[ulBlkId].SetBondAreaOffset(dOffsetX, dOffsetY);

		//v4.59A22
		dOffsetX = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["ThermalDeltaPitchX"];
		dOffsetY = (DOUBLE)(*pBTfile)["BinBlock"][ulBlkId]["ThermalDeltaPitchY"];
		m_oBinBlk[ulBlkId].SetThermalDeltaPitch(dOffsetX, dOffsetY);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["WalkPath"])) == FALSE)
		{
			ulWalkPath = (*pBTfile)["BinBlock"][ulBlkId]["WalkPath"];
		}
		else
		{
			ulWalkPath = 0;
		}
		m_oBinBlk[ulBlkId].SetWalkPath(ulWalkPath);

		if (((!((*pBTfile)["BinBlock"][ulBlkId]["DiePerBlk"])) == FALSE)
				&&
				((!((*pBTfile)["BinBlock"][ulBlkId]["DiePerRow"])) == FALSE))
		{
			ulDiePerBlk = (*pBTfile)["BinBlock"][ulBlkId]["DiePerBlk"];
			ulDiePerRow = (*pBTfile)["BinBlock"][ulBlkId]["DiePerRow"];
			ulDiePerCol = (*pBTfile)["BinBlock"][ulBlkId]["DiePerCol"];		//pllm
		}
		else
		{
			ulDiePerBlk = 0;
			ulDiePerRow = 0;
		}

		//v3.70T3		//PLLM
		BOOL bUseCircularBinArea	= (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Use Circular Bin Area"];
		LONG lRadius				= (LONG)(*pBTfile)["BinBlock"][ulBlkId]["Circular Bin Radius"];
		LONG lCenterX				= (LONG)(*pBTfile)["BinBlock"][ulBlkId]["Circular Bin CenterX"];
		LONG lCenterY				= (LONG)(*pBTfile)["BinBlock"][ulBlkId]["Circular Bin CenterY"];
		m_oBinBlk[ulBlkId].SetUseCircularArea(bUseCircularBinArea, lRadius);
		m_oBinBlk[ulBlkId].SetCirBinCenterXY(lCenterX, lCenterY);

		//v4.42T6
		m_oBinBlk[ulBlkId].m_bEnableBinMap = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Enable"];
		BOOL bEnable = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Enable Offset"];
		LONG lBinMapRowOffset = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Row Offset"];
		LONG lBinMapColOffset = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Col Offset"];
		m_oBinBlk[ulBlkId].SetBinMapOffset(bEnable, lBinMapRowOffset, lBinMapColOffset);
		//v4.42T11
		ULONG ulBinMapWalkPath	= (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Walk Path"];
		m_oBinBlk[ulBlkId].SetBinMapWalkPath(ulBinMapWalkPath);
		DOUBLE dCirRadius		= (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["CirRadius"];
		m_oBinBlk[ulBlkId].SetBinMapCircleRadius(dCirRadius);
		LONG lTEdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Top Edge"];
		LONG lBEdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Btm Edge"];
		LONG lLEdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Left Edge"];
		LONG lREdge = (*pBTfile)["BinBlock"][ulBlkId]["Bin Map"]["Right Edge"];
		m_oBinBlk[ulBlkId].SetBinMapEdgeSize(lTEdge, lBEdge, lLEdge, lREdge);

		//v4.30T4		//CyOptics
		m_bLeaveEmptyRow[ulBlkId]	= (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["LeaveEmptyRow"];
		m_ul1stRowIndex[ulBlkId]	= (*pBTfile)["BinBlock"][ulBlkId]["1stRowIndex"];	//v4.40T2

		if (ulDiePerRow == 0)
		{
			if (ulDiePerCol != 0)
			{
				ulDiePerRow = ulDiePerBlk / ulDiePerCol;
			}
		}
		else if (ulDiePerCol == 0)
		{
			if (ulDiePerRow != 0)
			{
				ulDiePerCol = ulDiePerBlk / ulDiePerRow;
			}
		}

		m_oBinBlk[ulBlkId].SetDiePerBlk(ulDiePerBlk);
		m_oBinBlk[ulBlkId].SetDiePerRow(ulDiePerRow);
		m_oBinBlk[ulBlkId].SetDiePerCol(ulDiePerCol);

		m_oBinBlk[ulBlkId].CalculateDieInfoDbCapacity(ulDiePerBlk, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime);
		
		if ((!((*pBTfile)["BinBlock"][ulBlkId]["IsDisableFromSameGradeMerge"])) == FALSE)
		{
			ulIsDisableFromSameGradeMerge = (*pBTfile)["BinBlock"][ulBlkId]["IsDisableFromSameGradeMerge"];
		}
		else
		{
			ulIsDisableFromSameGradeMerge = 0;
		}
		bIsDisableFromSameGradeMerge = BOOL(ulIsDisableFromSameGradeMerge);
		m_oBinBlk[ulBlkId].SetIsDisableFromSameGradeMerge(bIsDisableFromSameGradeMerge);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["PhyBlkId"])) == FALSE)
		{
			ulPhyBlkId = (*pBTfile)["BinBlock"][ulBlkId]["PhyBlkId"];
		}
		else
		{
			ulPhyBlkId = 0;
		}
		m_oBinBlk[ulBlkId].SetPhyBlkId(ulPhyBlkId);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["IsSetup"])) == FALSE)
		{
			ulIsSetup = (*pBTfile)["BinBlock"][ulBlkId]["IsSetup"];
		}
		else
		{
			ulIsSetup = 0;
		}
		bIsSetup = (BOOL)ulIsSetup;
		m_oBinBlk[ulBlkId].SetIsSetup(bIsSetup);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["Status"])) == FALSE)
		{
			ucStatus = (*pBTfile)["BinBlock"][ulBlkId]["Status"];
		}
		else
		{
			ucStatus = BT_BLK_NORMAL;
		}
		m_oBinBlk[ulBlkId].SetStatus(ucStatus);

		if (bLoadSNR == TRUE)
		{
			ulSerialNo = (*pBTfile)["BinBlock"][ulBlkId]["SerialNo"];
			m_oBinBlk[ulBlkId].SetSerialNo(ulSerialNo);

			szLastResetSerialNoDate = (*pBTfile)["BinBlock"][ulBlkId]["LastResetSerialNoDate"];
			m_oBinBlk[ulBlkId].SetLastResetSerialNoDate(szLastResetSerialNoDate);

			if (ulBlkId == 1)
			{
				CMSLogFileUtility::Instance()->BT_BinSerialLog("Start Load SNR");
			}

			CString szMsg;
			szMsg.Format("Init Load SNR - Blk%d date:%s  serial:%d", ulBlkId , szLastResetSerialNoDate, ulSerialNo);
			CMSLogFileUtility::Instance()->BT_BinSerialLog(szMsg);
		}

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["SkipUnit"])) == FALSE)
		{
			ulSkipUnit = (*pBTfile)["BinBlock"][ulBlkId]["SkipUnit"];
		}
		else
		{
			ulSkipUnit = 0;
		}
		m_oBinBlk[ulBlkId].SetSkipUnit(ulSkipUnit);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["MaxUnit"])) == FALSE)
		{
			ulMaxUnit = (*pBTfile)["BinBlock"][ulBlkId]["MaxUnit"];
		}
		else
		{
			ulMaxUnit = 0;
		}
		m_oBinBlk[ulBlkId].SetMaxUnit(ulMaxUnit);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["StartDate"])) == FALSE)
		{
			szStartDate = (*pBTfile)["BinBlock"][ulBlkId]["StartDate"];
		}
		else
		{
			szStartDate = "";
		}
		m_oBinBlk[ulBlkId].SetStartDate(szStartDate);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["Accumulated"])) == FALSE)
		{
			ulAccCount = (*pBTfile)["BinBlock"][ulBlkId]["Accumulated"];
		}
		else
		{
			ulAccCount = 0;
		}
		m_oBinBlk[ulBlkId].SetAccmulatedCount(ulAccCount);

		if ((!((*pBTfile)["BinBlock"][ulBlkId]["EmptyUnit"])) == FALSE)
		{
			ulEmptyUnit = (*pBTfile)["BinBlock"][ulBlkId]["EmptyUnit"];
		}
		else
		{
			ulEmptyUnit = 0;
		}
		m_oBinBlk[ulBlkId].SetEmptyUnit(ulEmptyUnit);
		
		bEnableFirstRowColSkipPattern = FALSE;
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Enbale FRSP"])) == FALSE)
		{
			bEnableFirstRowColSkipPattern = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Enbale FRSP"];
		}
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			bEnableFirstRowColSkipPattern = FALSE;
		}
		m_oBinBlk[ulBlkId].SetFirstRowColSkipPattern(bEnableFirstRowColSkipPattern);

		ulFirstRowColSkipUnit = 0;
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["FRSP Skip Unit"])) == FALSE)
		{
			ulFirstRowColSkipUnit = (ULONG)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["FRSP Skip Unit"];
		}
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			ulFirstRowColSkipUnit = 0;
		}
		m_oBinBlk[ulBlkId].SetFirstRowColSkipUnit(ulFirstRowColSkipUnit);


		//Centralized Bond Area
		bCentralizedBondArea = FALSE;
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Enbale Centralized"])) == FALSE)
		{
			bCentralizedBondArea = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Enbale Centralized"];
		}
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			bCentralizedBondArea = TRUE;
		}
		m_oBinBlk[ulBlkId].SetIsCentralizedBondArea(bCentralizedBondArea);

		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			//m_oBinBlk[ulBlkId].SetTeachWithPhysicalBlk(TRUE);
		}

		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset X"])) == FALSE)
		{
			lCentralizedOffsetX = (ULONG)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset X"];
		}
		else
		{
			lCentralizedOffsetX = 0;
		}
		m_oBinBlk[ulBlkId].SetCentralizedOffsetX(lCentralizedOffsetX);

		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset Y"])) == FALSE)
		{
			lCentralizedOffsetY = (ULONG)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Centralized Offset Y"];
		}
		else
		{
			lCentralizedOffsetY = 0;
		}
		m_oBinBlk[ulBlkId].SetCentralizedOffsetY(lCentralizedOffsetY);

		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Enbale Waffle Pad"])) == FALSE)
		{
			bEnableWafflePad = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["Enbale Waffle Pad"];
		}
		else
		{
			bEnableWafflePad = FALSE;
		}

		m_oBinBlk[ulBlkId].SetEnableWafflePad(bEnableWafflePad);
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist X"])) == FALSE)
		{
			lWafflePadDistX = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist X"];
		}
		else
		{
			lWafflePadDistX = 0;
		}

		m_oBinBlk[ulBlkId].SetWafflePadDistX(lWafflePadDistX);
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist Y"])) == FALSE)
		{
			lWafflePadDistY = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Dist Y"];
		}
		else
		{
			lWafflePadDistY = 0;
		}

		m_oBinBlk[ulBlkId].SetWafflePadDistY(lWafflePadDistY);
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size X"])) == FALSE)
		{
			lWafflePadSizeX = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size X"];
		}
		else
		{
			lWafflePadSizeX = 0;
		}

		m_oBinBlk[ulBlkId].SetWafflePadSizeX(lWafflePadSizeX);
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size Y"])) == FALSE)
		{
			lWafflePadSizeY = (*pBTfile)["BinBlock"][ulBlkId]["Waffle Pad Size Y"];
		}
		else
		{
			lWafflePadSizeY = 0;
		}
		m_oBinBlk[ulBlkId].SetWafflePadSizeY(lWafflePadSizeY);

		//v4.42T1	//Nichia
		BOOL bNoReturnTravel = FALSE;
		if (!(((*pBTfile)["BinBlock"][ulBlkId]["No Return Travel"])) == FALSE)
		{
			bNoReturnTravel = (BOOL)(LONG)(*pBTfile)["BinBlock"][ulBlkId]["No Return Travel"];
		}
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			//bNoReturnTravel = FALSE;
		}
		m_oBinBlk[ulBlkId].SetNoReturnTravel(bNoReturnTravel);
	}

	CMSFileUtility::Instance()->CloseBTConfig();
	return TRUE;  
}

BOOL CBinBlkMain::InitRandomHoleData(ULONG ulBlkId)
{
	CString szIndex;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	//CString szTemp;

	if (pUtl->LoadTwoDimensionBarcodeInfo() == FALSE)	// open Last State file 
	{
		return FALSE;
	}

	psmf = pUtl->GetTwoDimensionBarcodeFile();	// get file pointer

	// Max index = 10
	if (psmf != NULL)
	{
		// Max index = 10
		for (INT i = 0; i < 10; i++)
		{
			szIndex.Format("%d", i + 1);

			ULONG ulRandomNum = (ULONG)(*psmf)["BinBlock"][ulBlkId]["RandomHole"][szIndex];
			m_oBinBlk[ulBlkId].SetRandomHoleDieIndex(ulBlkId, i, ulRandomNum);
		}

		pUtl->CloseTwoDimensionBarcodeFile();
	}

	return TRUE;
}

BOOL CBinBlkMain::InitAndLoadAllRandomHoleData()
{
	CString szIndex;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	//CString szTemp;

	if (pUtl->LoadTwoDimensionBarcodeInfo() == FALSE)	// open Last State file 
	{
		return FALSE;
	}

	psmf = pUtl->GetTwoDimensionBarcodeFile();	// get file pointer
	if (psmf != NULL)
	{
		ULONG ulBlkId = 1;

		for (ulBlkId=1; ulBlkId<=GetNoOfBlk(); ulBlkId++)
		{
			// Max index = 10
			for (INT i = 0; i < 10; i++)
			{
				szIndex.Format("%d", i + 1);

				ULONG ulRandomNum = (ULONG)(*psmf)["BinBlock"][ulBlkId]["RandomHole"][szIndex];
				m_oBinBlk[ulBlkId].SetRandomHoleDieIndex(ulBlkId, i, ulRandomNum);
			}

			m_ulCurrHoleDieNum[ulBlkId] = (ULONG)(*psmf)["BinBlock"][ulBlkId]["RandomHole"]["CurHoleDieNum"];
		}

		pUtl->CloseTwoDimensionBarcodeFile();
	}

	return TRUE;
}

BOOL CBinBlkMain::SaveRandomHoleData(ULONG ulBlkId)
{
	CString szIndex, szData="";
	ULONG ulData = 0;
	CString szValue;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	//CString szTemp;

	if (!m_bEnable2DBarcodeOutput)	//v4.48A29
	{
		return TRUE;
	}

	if (pUtl->LoadTwoDimensionBarcodeInfo() == FALSE)	// open Last State file 
	{
		return FALSE;
	}

	psmf = pUtl->GetTwoDimensionBarcodeFile();	// get file pointer

	// Max index = 10
	if (psmf != NULL)
	{
		for (INT i = 0; i < 10; i++)
		{
			szIndex.Format("%d", i + 1);
			(*psmf)["BinBlock"][ulBlkId]["RandomHole"][szIndex] = (ULONG)m_oBinBlk[ulBlkId].GetRandomHoleDieIndex(ulBlkId, i);

			//v4.48A18
			ulData = (ULONG) m_oBinBlk[ulBlkId].GetRandomHoleDieIndex(ulBlkId, i);
			szValue.Format("%lu", ulData);
			szData = szData + szValue + ",";	//v4.48A10
		}

		pUtl->SaveTwoDimensionBarcodeFile();
		pUtl->CloseTwoDimensionBarcodeFile();
	}

	//v4.48A10	//Cree HuiZhou	//v4.50A28
	CString szMsg;
	szMsg.Format("BIN #%d Random HOLE index created (SaveRandomHoleData): ", ulBlkId);
	//if (CMSLogFileUtility::Instance()->GetEnableMachineLog())
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	return TRUE;
}

BOOL CBinBlkMain::LoadCurHoleDieNum(ULONG ulBlkId)
{	
	CString szIndex;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	//CString szTemp;

	if (pUtl->LoadTwoDimensionBarcodeInfo() == FALSE)	// open Last State file 
	{
		return FALSE;
	}

	psmf = pUtl->GetTwoDimensionBarcodeFile();	// get file pointer

	// Max index = 10
	if (psmf != NULL)
	{
		m_ulCurrHoleDieNum[ulBlkId] = (ULONG)(*psmf)["BinBlock"][ulBlkId]["RandomHole"]["CurHoleDieNum"];
		pUtl->CloseTwoDimensionBarcodeFile();
	}

	return TRUE;
}

BOOL CBinBlkMain::SaveCurHoleDieNum(ULONG ulBlkId)
{
	CString szIndex;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf;
	//CString szTemp;

	if (pUtl->LoadTwoDimensionBarcodeInfo() == FALSE)	// open Last State file 
	{
		return FALSE;
	}

	psmf = pUtl->GetTwoDimensionBarcodeFile();	// get file pointer

	if (psmf != NULL)
	{
		(*psmf)["BinBlock"][ulBlkId]["RandomHole"]["CurHoleDieNum"] = (ULONG)m_ulCurrHoleDieNum[ulBlkId];
		pUtl->SaveTwoDimensionBarcodeFile();
		pUtl->CloseTwoDimensionBarcodeFile();
	}

	return TRUE;
}


BOOL CBinBlkMain::InitBinGradeData()
{
	ULONG ulGradeCapacity, ulInputCount;
	ULONG ulTempIsAssigned;
	UCHAR ucAliasGrade;
	BOOL bIsAssigned;
	CString szRankID, szBlockSize; 
	ULONG i, j;

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	
	if (pBTfile == NULL)
	{
		return FALSE;
	}
	
	for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)		//Grade No
	{
		if ((!((*pBTfile)["BinGradeData"][i]["GradeCapacity"])) == FALSE)
		{
			ulGradeCapacity = (*pBTfile)["BinGradeData"][i]["GradeCapacity"];
			m_oBinGradeData[i].SetGradeCapacity(ulGradeCapacity);
		}

		if ((!((*pBTfile)["BinGradeData"][i]["InputCount"])) == FALSE)
		{
			ulInputCount = (*pBTfile)["BinGradeData"][i]["InputCount"];
			m_oBinGradeData[i].SetInputCount(ulInputCount);
		}

		if ((!((*pBTfile)["BinGradeData"][i]["AliasGrade"])) == FALSE)
		{
			ucAliasGrade = (*pBTfile)["BinGradeData"][i]["AliasGrade"];
			m_oBinGradeData[i].SetAliasGrade(ucAliasGrade);
		}

		if ((!((*pBTfile)["BinGradeData"][i]["IsAssigned"])) == FALSE)
		{
			ulTempIsAssigned = (*pBTfile)["BinGradeData"][i]["IsAssigned"];
			bIsAssigned = BOOL(ulTempIsAssigned);
			m_oBinGradeData[i].SetIsAssigned(bIsAssigned);
		}

		//if (m_oBinGradeData[i].GetIsAssigned() && m_oBinGradeData[i].GetNVBlkInUse() == 0)
		if (m_oBinGradeData[i].GetIsAssigned() == TRUE)
		{
			UCHAR *ucBlkStatus;
			
			bIsAssigned = FALSE;
			ucBlkStatus = new UCHAR[m_ulNoOfBlk + 1];

			//Check any binblk have same grade & which binblk is fulled, bonded (not full) or empty
			for (j = 1; j <= GetNoOfBlk(); j++)
			{
				ucBlkStatus[j] = BT_OZ_BLK_NOT_ASSIGN;

				if (m_oBinBlk[j].GetGrade() == UCHAR(i))
				{		
					ucBlkStatus[j] = BT_OZ_BLK_IS_ASSIGN;

					if (m_oBinBlk[j].GetNVIsFull() == TRUE)
					{
						ucBlkStatus[j] = BT_OZ_BLK_IS_FULL;
					}
					else if (m_oBinBlk[j].GetNVNoOfBondedDie() > 0)
					{
						ucBlkStatus[j] = BT_OZ_BLK_NOT_FULL;
					}
				}
			}

			//Assign current grade's blk to bonded blk only
			for (j = 1; j <= GetNoOfBlk(); j++)
			{
				if ((m_oBinBlk[j].GetGrade() == UCHAR(i)) && (ucBlkStatus[j] == BT_OZ_BLK_NOT_FULL))
				{
					m_oBinGradeData[i].SetNVBlkInUse(j);
					bIsAssigned = TRUE;
					break;
				}
			}
			delete[] ucBlkStatus;

			//Assign current grade's blk base on binblk order
			if (bIsAssigned == FALSE)
			{
				for (j = 1; j <= GetNoOfBlk(); j++)
				{
					if (m_oBinBlk[j].GetGrade() == UCHAR(i))
					{
						m_oBinGradeData[i].SetNVBlkInUse(j);
						break;
					}
				}
			}
		}

		if ((!((*pBTfile)["BinGradeData"][i]["RankID"])) == FALSE)
		{
			szRankID = (*pBTfile)["BinGradeData"][i]["RankID"];
			m_oBinGradeData[i].SetRankID(szRankID);
		}

		if ((!((*pBTfile)["BinGradeData"][i]["BlockSize"])) == FALSE)
		{
			szBlockSize = (*pBTfile)["BinGradeData"][i]["BlockSize"];
			m_oBinGradeData[i].SetBlockSize(szBlockSize);
		}
	}

	CMSFileUtility::Instance()->CloseBTConfig();
	return TRUE;
} //end InitBinGradeData

BOOL CBinBlkMain::InitOptimizeBinCountData()
{
	ULONG ulMinFrameDieCount = 0, ulMaxFrameDieCount = 0;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}
	
	for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++) //Grade No
	{

		if ((!((*pBTfile)["BinGradeData"][i]["Min Frame Die Count"])) == FALSE)
		{
			ulMinFrameDieCount = (ULONG)(LONG)(*pBTfile)["BinGradeData"][i]["Min Frame Die Count"];
			m_oBinGradeData[i].SetMinFrameDieCount(ulMinFrameDieCount);
		}

		if ((!((*pBTfile)["BinGradeData"][i]["Max Frame Die Count"])) == FALSE)
		{
			ulMaxFrameDieCount = (ULONG)(LONG)(*pBTfile)["BinGradeData"][i]["Max Frame Die Count"];
			m_oBinGradeData[i].SetMaxFrameDieCount(ulMaxFrameDieCount);
		}
	}

	CMSFileUtility::Instance()->CloseBTConfig();
	return TRUE;
}

BOOL CBinBlkMain::InitOptimizeBinCountRunTimeData()
{
	CStringMapFile  *psmf;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	ULONG ulRemainCount = 0;

	if (pUtl->LoadLastState("BBX: InitOptimizeBinCountRunTimeData") == FALSE) 	// open LastState file
	{
		return FALSE;
	}

	psmf = pUtl->GetLastStateFile("BBX: InitOptimizeBinCountRunTimeData");	

	if (psmf != NULL)	//v4.53A9
	{	
		for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++) //Grade No
		{
			if (psmf != NULL)	//Klocwork	//v4.02T5
			{
				ulRemainCount = (ULONG)(*psmf)["BinGradeData"][i]["Lot Remain Count"];
				m_oBinGradeData[i].SetLotRemainingCount(ulRemainCount);
			}
		}
	}

	pUtl->CloseLastState("BBX: InitOptimizeBinCountRunTimeData");
	return TRUE;
}

/***********************************/
/*  Init Non-volatile RAM pointer  */
/***********************************/
VOID CBinBlkMain::InitNVBTDataPtr()
{
	ULONG i;

	//1. Bin Block Data 56*176 = 9856
	//2. Grade Data 
	for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		m_oBinBlk[i].SetPSTNVBinBlkData(m_pvNVRAM, m_lNVRAM_BinTable_Start, i);
		m_oBinBlk[i].SetPSTNVBinBlkData_HW(m_pvNVRAM_HW, m_lNVRAM_BinTable_Start, i);			//v4.65A3

		m_oBinGradeData[i].SetPSTNVBinGradeData(m_pvNVRAM, m_lNVRAM_BinTable_Start,
												glNVRAM_BinTable_Size , (UCHAR)i);
		m_oBinGradeData[i].SetPSTNVBinGradeData_HW(	m_pvNVRAM_HW,	m_lNVRAM_BinTable_Start,	//v4.65A3
												glNVRAM_BinTable_Size, (UCHAR)i);
	}
}

/***********************************/
/*  Clear NVRAM for Cold-start	   */
/***********************************/
VOID CBinBlkMain::ClearNVRAM(VOID *pvNVRAM, LONG lOffset)
{
	ULONG i;

	for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		m_oBinBlk[i].SetPSTNVBinBlkData(pvNVRAM, lOffset, i);
		m_oBinGradeData[i].SetPSTNVBinGradeData(pvNVRAM, lOffset, glNVRAM_BinTable_Size, (UCHAR)i);
		ClearBinCounter(i);		// Call clear bin counter to init the NVRAM

		SetBinAccBondedCount(i, 0);
	}

	CheckIfGradesAreStillValid();

	//Set & Clear RunTime Data
	SetPSTNVRunTimeData(pvNVRAM, lOffset, glNVRAM_BinTable_Size);
	ResetNVRunTimeData();
}


/***********************************/
/*  Bin map functions			   */
/***********************************/
BOOL CBinBlkMain::IsDieInBinMap(CONST LONG lRow, CONST LONG lCol)
{
	LONG lCurrRow = lRow;
	LONG lCurrCol = lCol;

	if ((lCurrRow < 0) || (lCurrRow >= MAX_BINMAP_SIZE_X))
	{
		return FALSE;
	}
	if ((lCurrCol < 0) || (lCurrCol >= MAX_BINMAP_SIZE_Y))
	{
		return FALSE;
	}

	if (m_nBinMap[lCurrRow][lCurrCol] == 1)
	{
		return TRUE;
	}
	return FALSE;
}

LONG CBinBlkMain::IsDieInOSRAMBinMap(CONST LONG lRow, CONST LONG lCol, UCHAR ucGrade)
{
	
	if ((lRow < 0) || (lRow >= MAX_BINMAP_SIZE_X))
	{
		return FALSE;
	}
	if ((lCol < 0) || (lCol >= MAX_BINMAP_SIZE_Y))
	{
		return FALSE;
	}

	if (m_nBinMap[lRow][lCol] == ucGrade)
	{
		return 1;
	}

	return FALSE;
}

ULONG CBinBlkMain::Get1stDieIndexIn1stBMapRow(ULONG ulBlkToUse, ULONG &ulDieRow, ULONG &ulDieCol)		
{
	//UPdate Walk Path for bin realignment purpose
	ULONG ulIndex = 1;

	ulDieRow = ulDieCol = 0;
	if (IsUseBinMapBondArea(ulBlkToUse))							//v4.43T13	//put to first of the loop!
	{
		GetDieIndexInBinMap(ulBlkToUse, ulIndex, ulDieRow, ulDieCol, TRUE);	
	}
	else if (CMS896AStn::m_oNichiaSubSystem.IsEnabled() && m_oBinBlk[ulBlkToUse].m_bEnableBinMap)	//v4.43T13	//Nichia//v4.43T7
	{
		LONG lBRow=0, lBCol=0;
		m_oBinBlk[ulBlkToUse].GetNextDieIndexInBinMap(m_oBinBlk[ulBlkToUse].GetDiePerBlk(), 
					ulIndex, lBRow, lBCol);
		ulDieRow = lBRow;
		ulDieCol = lBCol;
	}
	else
	{
		ulIndex = 0;
	}

	return ulIndex;	
}


ULONG CBinBlkMain::GetLastDieIndexIn1stBMapRow(ULONG ulBlkToUse, ULONG &ulNoOfDiesIn1stRow, ULONG &ulDieRow, ULONG &ulDieCol)
{
	if (!IsUseBinMapBondArea(ulBlkToUse))	
	{
		return m_oBinBlk[ulBlkToUse].GetLastDieIndexIn1stBMapRow(ulNoOfDiesIn1stRow);
	}

	//v4.48A4
	//if ( (m_ucSpGrade1ToByPassBinMap > 0) && (m_ucSpGrade1ToByPassBinMap == ulBlkToUse) )
	if (m_oBinBlk[ulBlkToUse].GetByPassBinMap())	//v4.48A11	//Semitek, 3E DL
	{
		return 0;
	}

	ULONG ulIndex = 0;
	ULONG ulDieCount = 0;
	INT i = 0;
	ULONG ulWalkPath = m_oBinBlk[ulBlkToUse].GetOriginalWalkPath();	//GrabWalkPath(1);	//v4.50A24
	CString szLog;

	INT n1stRow = 0;
	INT n1stCol = 0;
	BOOL b1stRowColFound = FALSE;

	//v4.36T10
	//Find first ROW or COLUMN before counting
	if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH))
	{
		for (INT m = 0; m < MAX_BINMAP_SIZE_Y; m++)		//ROW
		{
			for (INT n = 0; n < MAX_BINMAP_SIZE_X; n++)	//COL
			{
				if (m_nBinMap[m][n] == 1)
				{
					n1stRow = m;
					b1stRowColFound = TRUE;
					break;
				}
			}

			if (b1stRowColFound)	
			{
				break;    //v4.37T3
			}		
		}
	}
	else if ((ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH))
	{
		for (INT m = MAX_BINMAP_SIZE_Y - 1; m >= 0; m--)	//ROW
		{
			for (INT n = 0; n < MAX_BINMAP_SIZE_X; n++)		//COL
			{
				if (m_nBinMap[m][n] == 1)
				{
					n1stRow = m;
					b1stRowColFound = TRUE;
					break;
				}
			}

			if (b1stRowColFound)	
			{
				break;    //v4.37T3
			}		
		}
	}
	else if ((ulWalkPath == BT_TRV_PATH) ||					//TR-Vertical
			 (ulWalkPath == BT_BRV_PATH))					//BR-VErtical
	{
		for (INT n = MAX_BINMAP_SIZE_X - 1; n >= 0; n--)	//COL
		{
			for (INT m = 0; m < MAX_BINMAP_SIZE_Y; m++)		//ROW
			{
				if (m_nBinMap[m][n] == 1)
				{
					n1stCol = n;
					b1stRowColFound = TRUE;
					break;
				}
			}

			if (b1stRowColFound)
			{
				break;    //v4.37T3
			}		
		}
	}
	//else	//TL-Vertical
	else if ((ulWalkPath == BT_TLV_PATH) ||				//TL-Vertical
			 (ulWalkPath == BT_BLV_PATH))				//BL-VErtical
	{
		for (INT n = 0; n < MAX_BINMAP_SIZE_X; n++)		//COL
		{
			for (INT m = 0; m < MAX_BINMAP_SIZE_Y; m++)	//ROW
			{
				if (m_nBinMap[m][n] == 1)
				{
					n1stCol = n;
					b1stRowColFound = TRUE;
					break;
				}
			}
			
			if (b1stRowColFound)
			{
				break;    //v4.37T3
			}		
		}
	}


	//Then cotun how many dices on this first ROW/COL, as well as the lastDieIndex
	if (ulWalkPath == BT_TLH_PATH)		//TL-Horizontal
	{
		FindLastColInRow(n1stRow, (n1stRow % 2 == 0) ? 1 : -1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}
	else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
	{
		FindLastColInRow(n1stRow, (n1stRow % 2 == 0) ? -1 : 1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}
	else if (ulWalkPath == BT_BLH_PATH)	// BL-Horizontal
	{
		FindLastColInRow(n1stRow, ((MAX_BINMAP_SIZE_Y - 1 - n1stRow) % 2 == 0) ? 1 : -1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}
	else if (ulWalkPath == BT_BRH_PATH)	// BR-Horizontal
	{
		FindLastColInRow(n1stRow, ((MAX_BINMAP_SIZE_Y - 1 - n1stRow) % 2 == 0) ? -1 : 1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}

	else if (ulWalkPath == BT_TLV_PATH)		//TL-Vertical
	{
		FindLastRowInColumn(n1stCol, (n1stCol % 2 == 0) ? 1 : -1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}
	else if (ulWalkPath == BT_BLV_PATH)		//BL-Vertical
	{
		FindLastRowInColumn(n1stCol, (n1stCol % 2 == 0) ? -1 : 1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}
	else if (ulWalkPath == BT_TRV_PATH)		//TR-Vertical
	{
		FindLastRowInColumn(n1stCol, ((MAX_BINMAP_SIZE_X - 1 - n1stCol) % 2 == 0) ? -1 : 1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}
	else if (ulWalkPath == BT_BRV_PATH)		//BR-Vertical	//v4.50A16
	{
		FindLastRowInColumn(n1stCol, ((MAX_BINMAP_SIZE_X - 1 - n1stCol) % 2 == 0) ? 1 : -1, ulDieCount, ulIndex, ulDieRow, ulDieCol);
	}

	ulNoOfDiesIn1stRow = ulDieCount;
	return ulIndex;
}


VOID CBinBlkMain::FindLastColInRow(const LONG lRow, const LONG lDirection,
								   ULONG &ulDieCount, ULONG &ulIndex, ULONG &ulDieRow, ULONG &ulDieCol)
{
	if (lDirection == 1)
	{
		for (LONG i = 0; i < MAX_BINMAP_SIZE_X; i++)
		{
			if (m_nBinMap[lRow][i] == 1)
			{
				ulDieCount++;
				ulIndex = i + 1;
				ulDieRow = lRow;
				ulDieCol = i;
			}
		}
	}
	else
	{
		for (LONG i = MAX_BINMAP_SIZE_X - 1; i >= 0; i--)
		{
			if (m_nBinMap[lRow][i] == 1)
			{
				ulDieCount++;
				ulIndex = i + 1;
				ulDieRow = lRow;
				ulDieCol = i;
			}
		}
	}
}


VOID CBinBlkMain::FindLastRowInColumn(const LONG lCol, const LONG lDirection,
									  ULONG &ulDieCount, ULONG &ulIndex, ULONG &ulDieRow, ULONG &ulDieCol)
{
	if (lDirection == 1)
	{
		for (LONG i = 0; i < MAX_BINMAP_SIZE_Y; i++)
		{
			if (m_nBinMap[i][lCol] == 1)
			{
				ulDieCount++;
				ulIndex = i + 1;
				ulDieRow = i;
				ulDieCol = lCol;
			}
		}
	}
	else
	{
		for (LONG i = MAX_BINMAP_SIZE_Y - 1; i >= 0; i--)
		{
			if (m_nBinMap[i][lCol] == 1)
			{
				ulDieCount++;
				ulIndex = i + 1;
				ulDieRow = i;
				ulDieCol = lCol;
			}
		}
	}
}

LONG CBinBlkMain::GetNextDieIndexInOSRAMBinMixMap(ULONG ulBlkToUse, ULONG &ulNextIndex)
{
	LONG lBinRow = 0, lBinCol = 0;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulBinIndex	= ulNextIndex;

	ULONG ulDiePerRow	= GrabNoOfDiePerRow(1);
	ULONG ulDiePerCol	= GrabNoOfDiePerCol(1);
	ULONG ulWalkPath	= GrabWalkPath(1);

	if(pApp->GetCustomerName() != CTM_OSRAM || pApp->GetProductLine() != "Germany" || !m_bEnableOsramBinMixMap)
	{
		return 3;
	}
	BOOL bNoReturnTravel = m_oBinBlk[ulBlkToUse].GetNoReturnTravel();	//v4.42T1
	BOOL bDieFound = FALSE;
	ULONG ulBinMapPath = BT_TLH_PATH;
	do
	{
		if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
		{
			GetRowColWithTLH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
		}
		else
		{
			return FALSE;
		}
		UCHAR ucGrade = m_oBinBlk[ulBlkToUse].GetGrade();
		m_ucBinMixCurrentGrade = ucGrade;

		if (IsDieInOSRAMBinMap(lBinRow - 1, lBinCol - 1, ucGrade))
		{

			bDieFound = TRUE;			//done; target index found
			break;
		}
	ulBinIndex++;

	} 
	while (ulBinIndex <= (GrabGradeCapacity(1)+ 1));

	if (!bDieFound)
	{
		return FALSE;
	}
	ulNextIndex	= ulBinIndex;

	return TRUE;
}


VOID CBinBlkMain::GetRowColWithTLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient  = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulDiePerRow;
			ulBinMapPath = BT_TLH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1)	//last in row
			{
				lBinCol = ulDiePerRow;
				ulBinMapPath = BT_TLH_PATH;	
			}
			else						//first in row
			{
				lBinCol = 1;
				ulBinMapPath = BT_TRH_PATH;	
			}
		}
		lBinRow = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulRemainder;
			ulBinMapPath = BT_TLH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = ulRemainder;
				ulBinMapPath = BT_TLH_PATH;	

			}
			else //on even row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
				ulBinMapPath = BT_TRH_PATH;	
			}
		}
		lBinRow = (ulQuotient + 1);
	}
}


VOID CBinBlkMain::GetRowColWithTRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = 1;
			ulBinMapPath = BT_TRH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lBinCol = 1;
				ulBinMapPath = BT_TRH_PATH;	
			}
			else //first in row
			{
				lBinCol = ulDiePerRow;
				ulBinMapPath = BT_TLH_PATH;	
			}
		}
		lBinRow = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = (ulDiePerRow - ulRemainder + 1);
			ulBinMapPath = BT_TRH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
				ulBinMapPath = BT_TRH_PATH;	
			}
			else //on even row
			{
				lBinCol = ulRemainder;
				ulBinMapPath = BT_TLH_PATH;	
			}
		}
		lBinRow = (ulQuotient + 1);
	}
}


VOID CBinBlkMain::GetRowColWithBLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient  = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulDiePerRow;
			ulBinMapPath = BT_BLH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1)	//last in row
			{
				lBinCol = ulDiePerRow;
				ulBinMapPath = BT_BLH_PATH;	
			}
			else						//first in row
			{
				lBinCol = 1;
				ulBinMapPath = BT_BRH_PATH;	
			}
		}
		lBinRow = ulDiePerCol - ulQuotient + 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulRemainder;
			ulBinMapPath = BT_BLH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = ulRemainder;
				ulBinMapPath = BT_BLH_PATH;	
			}
			else //on even row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
				ulBinMapPath = BT_BRH_PATH;	
			}
		}
		lBinRow = ulDiePerCol - ulQuotient;
	}
}


VOID CBinBlkMain::GetRowColWithBRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = 1;
			ulBinMapPath = BT_BRH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lBinCol = 1;
				ulBinMapPath = BT_BRH_PATH;	
			}
			else //first in row
			{
				lBinCol = ulDiePerRow;
				ulBinMapPath = BT_BLH_PATH;	
			}
		}
		lBinRow = ulDiePerCol - ulQuotient + 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = (ulDiePerRow - ulRemainder + 1);
			ulBinMapPath = BT_BRH_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
				ulBinMapPath = BT_BRH_PATH;	
			}
			else //on even row
			{
				lBinCol = ulRemainder;
				ulBinMapPath = BT_BLH_PATH;	
			}
		}
		lBinRow = ulDiePerCol - ulQuotient;
	}
}

VOID CBinBlkMain::GetRowColWithTLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerCol;
	ULONG ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulDiePerCol;
			ulBinMapPath = BT_TLV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = ulDiePerCol;
				ulBinMapPath = BT_TLV_PATH;	
			}
			else //first in column
			{
				lBinRow = 1;
				ulBinMapPath = BT_BLV_PATH;	
			}
		}
		lBinCol = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulRemainder;
			ulBinMapPath = BT_TLV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinRow = ulRemainder;
				ulBinMapPath = BT_TLV_PATH;	
			}
			else //on even row
			{
				lBinRow = (ulDiePerCol - ulRemainder + 1);
				ulBinMapPath = BT_BLV_PATH;	
			}
		}
		lBinCol = ulQuotient + 1;
	}
}



VOID CBinBlkMain::GetRowColWithTRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerCol;
	ULONG ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulDiePerCol;
			ulBinMapPath = BT_TRV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = ulDiePerCol;
				ulBinMapPath = BT_TRV_PATH;	
			}
			else //first in column
			{
				lBinRow = 1;
				ulBinMapPath = BT_BRV_PATH;	
			}
		}
		lBinCol = ulDiePerRow - ulQuotient + 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulRemainder;
			ulBinMapPath = BT_TRV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinRow = ulRemainder;
				ulBinMapPath = BT_TRV_PATH;	
			}
			else //on even row
			{
				lBinRow = (ulDiePerCol - ulRemainder + 1);
				ulBinMapPath = BT_BRV_PATH;	
			}
		}
		lBinCol = ulDiePerRow - ulQuotient;
	}
}


VOID CBinBlkMain::GetRowColWithBLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerCol;
	ULONG ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)	
		{
			lBinRow = 1;
			ulBinMapPath = BT_BLV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = 1;
				ulBinMapPath = BT_BLV_PATH;	
			}
			else //first in column
			{
				lBinRow = ulDiePerCol;
				ulBinMapPath = BT_TLV_PATH;	
			}
		}
		lBinCol = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulDiePerCol - ulRemainder + 1;
			ulBinMapPath = BT_BLV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinRow = (ulDiePerCol - ulRemainder + 1);
				ulBinMapPath = BT_BLV_PATH;	
			}
			else //on even row
			{
				lBinRow = ulRemainder;
				ulBinMapPath = BT_TLV_PATH;	
			}
		}
		lBinCol = ulQuotient + 1;
	}
}


VOID CBinBlkMain::GetRowColWithBRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerCol;
	ULONG ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = 1;
			ulBinMapPath = BT_BRV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = 1;
				ulBinMapPath = BT_BRV_PATH;	
			}
			else //first in column
			{
				lBinRow = ulDiePerCol;
				ulBinMapPath = BT_TRV_PATH;	
			}
		}
		lBinCol = ulDiePerRow - ulQuotient + 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulDiePerCol - ulRemainder + 1;
			ulBinMapPath = BT_BRV_PATH;	
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinRow = (ulDiePerCol - ulRemainder + 1);
				ulBinMapPath = BT_BRV_PATH;	
			}
			else //on even row
			{
				lBinRow = ulRemainder;
				ulBinMapPath = BT_TRV_PATH;	
			}
		}
		lBinCol = ulDiePerRow - ulQuotient;
	}
}


VOID CBinBlkMain::GetRowColWithIndexInBinMap(const ULONG ulBinIndex, const ULONG ulWalkPath, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
											 LONG &lBinRow, LONG &lBinCol, ULONG &ulBinMapPath)
{
	if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
	{
		GetRowColWithTLH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
	else if (ulWalkPath == BT_TRH_PATH) //TR-Horizontal
	{
		GetRowColWithTRH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
	else if (ulWalkPath == BT_BLH_PATH)		//BL-Horizontal 2018.5.18
	{
		GetRowColWithBLH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
	else if (ulWalkPath == BT_BRH_PATH)		//BR-Horizontal	2018.5.18
	{
		GetRowColWithBRH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
	else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
	{
		GetRowColWithTLV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
	else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
	{
		GetRowColWithTRV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
	else if (ulWalkPath == BT_BLV_PATH)		//BL-Vertical	//v4.50A16
	{
		GetRowColWithBLV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
	else if (ulWalkPath == BT_BRV_PATH)		//BR-Vertical	//v4.50A16
	{
		GetRowColWithBRV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);
	}
}

BOOL CBinBlkMain::CreteBondedDieInBinMap(const ULONG ulBlkToUse, const ULONG ulLastIndex)
{
	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulDieCount	= 0;	//ulNextIndex;
	ULONG ulIndexCount	= 1;
	ULONG ulBinMapPath	= 0;
	ULONG ulDiePerRow	= GrabNoOfDiePerRow(1);
	ULONG ulDiePerCol	= GrabNoOfDiePerCol(1);
	ULONG ulWalkPath	= m_oBinBlk[1].GetOriginalWalkPath();	//GrabWalkPath(1);	//v4.50A24

	if (!IsUseBinMapBondArea(ulBlkToUse))
	{
		BOOL bStatus = m_oBinBlk[ulBlkToUse].CreateBondedDieInBinMap(m_oBinBlk[ulBlkToUse].GetDiePerBlk(), ulLastIndex);
		return bStatus;
	}

	if (m_oBinBlk[ulBlkToUse].GetByPassBinMap())	//v4.48A11	//Semitek, 3E DL
	{
		return TRUE;
	}

	BOOL bNoReturnTravel = m_oBinBlk[ulBlkToUse].GetNoReturnTravel();	//v4.42T1

	m_oBinBlk[ulBlkToUse].InitBondedDieInBinMap();
	do
	{
		GetRowColWithIndexInBinMap(ulIndexCount, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);

		if (IsDieInBinMap(lBinRow - 1, lBinCol - 1))
		{
			ulDieCount++;					//add by 1 if curr index hits inside binmap
			if (ulDieCount <= ulLastIndex)
			{
				m_oBinBlk[ulBlkToUse].m_cBondedDieMap[lBinRow - 1][lBinCol - 1] = 1; //Bonded
			}
		}

		ulIndexCount++;		//actual index in rectangular bin area
	} 
	while (ulIndexCount <= GrabGradeCapacity(1));

	return TRUE;
}

BOOL CBinBlkMain::IsBondedDieInBinMap(const ULONG ulBlkToUse, const ULONG ulRow, const ULONG ulCol)
{
	return m_oBinBlk[ulBlkToUse].IsBondedDieInBinMap(ulRow, ulCol);
}


ULONG CBinBlkMain::GetCurPathInBinMap(const ULONG ulRow, const ULONG ulCol)
{
	ULONG ulCurBinMapPath = BT_TLH_PATH;
	ULONG ulWalkPath = m_oBinBlk[1].GetOriginalWalkPath();

	if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ||
		(ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH))
	{
		ulCurBinMapPath = GetCurRowWalkPath(ulWalkPath, ulRow);
	}
	else
	{
		ulCurBinMapPath = GetCurColumnWalkPath(ulWalkPath, ulCol);
	}

	return ulCurBinMapPath;
}

BOOL CBinBlkMain::GetNextDieIndexInBinMap(ULONG ulBlkToUse, ULONG &ulNextIndex)
{
	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulBinIndex	= ulNextIndex;

	ULONG ulDiePerRow	= GrabNoOfDiePerRow(1);
	ULONG ulDiePerCol	= GrabNoOfDiePerCol(1);

	//v4.50A24		//Use original path for binmap INDEX calculation
	ULONG ulWalkPath	= m_oBinBlk[ulBlkToUse].GetOriginalWalkPath();		//GrabWalkPath(1);


	if (!IsUseBinMapBondArea(ulBlkToUse))
	{
		LONG lBinRow=0, lBinCol=0;
		return m_oBinBlk[ulBlkToUse].GetNextDieIndexInBinMap(GrabGradeCapacity((UCHAR)ulBlkToUse), 
															 ulNextIndex, lBinRow, lBinCol);
	}
	if (m_oBinBlk[ulBlkToUse].GetByPassBinMap())	//v4.48A13
	{
		return TRUE;
	}

	//if ( (m_ucSpGrade1ToByPassBinMap > 0) && (m_ucSpGrade1ToByPassBinMap == ulBlkToUse) )
	if (m_oBinBlk[ulBlkToUse].GetByPassBinMap())	//v4.48A11	//Semitek, 3E DL
	{
		return TRUE;
	}

	BOOL bNoReturnTravel = m_oBinBlk[ulBlkToUse].GetNoReturnTravel();	//v4.42T1

	BOOL bDieFound = FALSE;

	ULONG ulBinMapPath = BT_TLH_PATH;
	do
	{
		GetRowColWithIndexInBinMap(ulBinIndex, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);

		if (IsDieInBinMap(lBinRow - 1, lBinCol - 1))
		{
			bDieFound = TRUE;			//done; target index found

			//v4.57A13		//Optional: for ChangeLight only
			//Use BPR PostBond LF to search last-row die for mlyer-residue problem
			if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH))
			{
				if (IsDieInBinMap(lBinRow - 2, lBinCol - 1))
					m_bIsBinMapLastRowDieExist = TRUE;
				else
					m_bIsBinMapLastRowDieExist = FALSE;
			}
			else if ((ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH))
			{
				if (IsDieInBinMap(lBinRow, lBinCol - 1))
					m_bIsBinMapLastRowDieExist = TRUE;
				else
					m_bIsBinMapLastRowDieExist = FALSE;
			}
			else if ((ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_BLV_PATH))
			{
				if (IsDieInBinMap(lBinRow - 1, lBinCol - 2))
					m_bIsBinMapLastRowDieExist = TRUE;
				else
					m_bIsBinMapLastRowDieExist = FALSE;
			}
			else if ((ulWalkPath == BT_BRV_PATH) || (ulWalkPath == BT_BRV_PATH))
			{
				if (IsDieInBinMap(lBinRow - 1, lBinCol))
					m_bIsBinMapLastRowDieExist = TRUE;
				else
					m_bIsBinMapLastRowDieExist = FALSE;
			}

			break;
		}

		ulBinIndex++;
	} 
	while (ulBinIndex <= GrabGradeCapacity(1));

	if (!bDieFound)
	{
		return FALSE;
	}

	ulNextIndex	= ulBinIndex;
	return TRUE;
}



BOOL CBinBlkMain::GetDieIndexInBinMap(ULONG ulBlkToUse, ULONG &ulIndex, ULONG &ulRow, ULONG &ulCol, BOOL bUpdateWalkPath)
{
	ulRow = ulCol = 0;

	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulDieCount	= 0;	//ulNextIndex;
	ULONG ulIndexCount	= 1;
	ULONG ulBinMapPath	= 0;
	ULONG ulDiePerRow	= GrabNoOfDiePerRow(1);
	ULONG ulDiePerCol	= GrabNoOfDiePerCol(1);
	ULONG ulWalkPath	= m_oBinBlk[1].GetOriginalWalkPath();	//GrabWalkPath(1);	//v4.50A24

	if (!IsUseBinMapBondArea(ulBlkToUse))
	{
		LONG lBRow=0, lBCol=0;
		BOOL bStatus = m_oBinBlk[ulBlkToUse].GetNextDieIndexInBinMap(m_oBinBlk[ulBlkToUse].GetDiePerBlk(), 
						ulIndex, lBRow, lBCol);

		ulRow = lBRow - 1;
		ulCol = lBCol - 1;
		return bStatus;
	}

	if (m_oBinBlk[ulBlkToUse].GetByPassBinMap())	//v4.48A11	//Semitek, 3E DL
	{
		return TRUE;
	}

	if (ulIndex > GrabInputCount((UCHAR)ulBlkToUse))	//v4.49A7	//3E DL
	{
		return FALSE;
	}

	BOOL bNoReturnTravel = m_oBinBlk[ulBlkToUse].GetNoReturnTravel();	//v4.42T1
	BOOL bDieFound = FALSE;

	do
	{
		GetRowColWithIndexInBinMap(ulIndexCount, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);

		if (IsDieInBinMap(lBinRow - 1, lBinCol - 1))
		{
			ulDieCount++;					//add by 1 if curr index hits inside binmap
			if (ulDieCount >= ulIndex)
			{
				if (bUpdateWalkPath)		//v4.42T7
				{
					m_oBinBlk[ulBlkToUse].SetBinMapWalkPath(ulBinMapPath);
				}

				bDieFound = TRUE;			//done; target index found
				break;
			}
		}

		ulIndexCount++;		//actual index in rectangular bin area
	} 
	while (ulIndexCount <= GrabGradeCapacity(1));

	if (!bDieFound)
	{
		return FALSE;
	}

	ulRow = lBinRow - 1;
	ulCol = lBinCol - 1;
	ulIndex	= ulIndexCount;				//actual index replaces logical index
	return TRUE;
}

BOOL CBinBlkMain::GetDieIndexInBinMapWithRowCol(const ULONG ulBlkToUse, const ULONG ulRow, const ULONG ulCol, ULONG &ulIndex)
{
	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulDieCount	= 0;	//ulNextIndex;
	ULONG ulIndexCount	= 1;
	ULONG ulBinMapPath	= 0;
	ULONG ulDiePerRow	= GrabNoOfDiePerRow(1);
	ULONG ulDiePerCol	= GrabNoOfDiePerCol(1);
	ULONG ulWalkPath	= m_oBinBlk[1].GetOriginalWalkPath();	//GrabWalkPath(1);	//v4.50A24

	if (!IsUseBinMapBondArea(ulBlkToUse))
	{
		LONG lBRow=0, lBCol=0;
		BOOL bStatus = m_oBinBlk[ulBlkToUse].GetDieIndexInBinMapWithRowCol(m_oBinBlk[ulBlkToUse].GetDiePerBlk(), 
						ulRow, ulCol, ulIndex);
		return bStatus;
	}

	if (m_oBinBlk[ulBlkToUse].GetByPassBinMap())	//v4.48A11	//Semitek, 3E DL
	{
		return TRUE;
	}

	if (ulIndex > GrabInputCount((UCHAR)ulBlkToUse))	//v4.49A7	//3E DL
	{
		return FALSE;
	}

	BOOL bNoReturnTravel = m_oBinBlk[ulBlkToUse].GetNoReturnTravel();	//v4.42T1
	BOOL bDieFound = FALSE;

	do
	{
		GetRowColWithIndexInBinMap(ulIndexCount, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);

		if ((lBinRow - 1 == (LONG)ulRow) && (lBinCol - 1 == (LONG)ulCol))
		{
			bDieFound = TRUE;			//done; target index found
			break;
		}

		ulIndexCount++;		//actual index in rectangular bin area
	} 
	while (ulIndexCount <= GrabGradeCapacity(1));

	if (!bDieFound)
	{
		return FALSE;
	}

	ulIndex	= ulIndexCount;				//actual index replaces logical index
	return TRUE;
}



BOOL CBinBlkMain::GetDieIndexInBinMap2(ULONG ulBlkToUse, ULONG &ulIndex,
									   ULONG &ulLastIndex1, ULONG &ulLastIndex2,
									   ULONG &ulCurBinMapPath,
									   BOOL bUpdateWalkPath)
{
	LONG lBinRow = 0, lBinCol = 0;

	ULONG ulDieCount	= 0;	//ulNextIndex;
	ULONG ulIndexCount	= 1;
	ULONG ulBinMapPath	= 0;

	ULONG ulDiePerRow	= GrabNoOfDiePerRow(1);
	ULONG ulDiePerCol	= GrabNoOfDiePerCol(1);

	//v4.50A24
	//Use original WalkPath for this INDEX calculation because
	//BINMAP is NOT rotated;
	ULONG ulWalkPath	= m_oBinBlk[1].GetOriginalWalkPath();	//GrabWalkPath(1);	//v4.50A24
	ulCurBinMapPath		 = ulWalkPath;

	if (!IsUseBinMapBondArea(ulBlkToUse))
	{
		LONG lBRow=0, lBCol=0;
		BOOL bStatus = m_oBinBlk[ulBlkToUse].GetNextDieIndexInBinMap(m_oBinBlk[ulBlkToUse].GetDiePerBlk(), 
																	 ulIndex, lBRow, lBCol);
		return bStatus;
	}

	if (m_oBinBlk[ulBlkToUse].GetByPassBinMap())	//v4.48A11	//Semitek, 3E DL
	{
		return TRUE;
	}

	if (ulIndex > GrabInputCount((UCHAR)ulBlkToUse))	//v4.49A7	//3E DL
	{
		return FALSE;
	}

	BOOL bNoReturnTravel = m_oBinBlk[ulBlkToUse].GetNoReturnTravel();	//v4.42T1
	BOOL bDieFound = FALSE;

	//v4.59A31
	LONG lLastLastRow = 0;
	LONG lLastRow = 0;
	LONG lLastLastCol = 0;
	LONG lLastCol = 0;
	if (ulWalkPath == BT_TRV_PATH)	//TR-Vertical
		lLastCol = ulDiePerRow;

	LONG lLastRowDieCount = 1;
	LONG lLastLastRowDieCount = 1;
	LONG lLastColDieCount = 1;
	LONG lLastLastColDieCount = 1;

	do
	{
		GetRowColWithIndexInBinMap(ulIndexCount, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol, ulBinMapPath);

		if (IsDieInBinMap(lBinRow - 1, lBinCol - 1))
		{
			CString szLog;

			if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH))
			{
				if (lBinRow > lLastRow)
				{
					lLastLastRow = lLastRow; 
					lLastRow = lBinRow;
					lLastLastRowDieCount = lLastRowDieCount;
					lLastRowDieCount = ulDieCount;
				}
			}
			else if ((ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH))
			{
				if (lBinRow < lLastRow)
				{
					lLastLastRow = lLastRow;
					lLastRow = lBinRow;
					lLastLastRowDieCount = lLastRowDieCount;
					lLastRowDieCount = ulDieCount;
				}
			}
			else if ( (ulWalkPath == BT_TRV_PATH) || (ulWalkPath == BT_BRV_PATH)  )
			{
				if (lBinCol < lLastCol)
				{
					szLog.Format("TRV: DieCount = %ld, lBinCol = %ld, lLastCol=%ld", 
						ulDieCount, lBinCol, lLastCol);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
					lLastLastCol = lLastCol;
					lLastCol = lBinCol;
					lLastLastColDieCount = lLastColDieCount + 1;
					lLastColDieCount = ulDieCount;
				}
			}
			else if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_BLV_PATH)  )
			{
				if (lBinCol > lLastCol)
				{
					szLog.Format("BLV: DieCount = %ld, lBinCol = %ld, lLastCol=%ld", 
						ulDieCount, lBinCol, lLastCol);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
					lLastLastCol = lLastCol;
					lLastCol = lBinCol;
					lLastLastColDieCount = lLastColDieCount + 1;
					lLastColDieCount = ulDieCount;
				}
			}

			ulDieCount++;					//add by 1 if curr index hits inside binmap
		
			if (ulDieCount >= ulIndex)
			{
				if (bUpdateWalkPath)
				{
					m_oBinBlk[ulBlkToUse].SetBinMapWalkPath(ulBinMapPath);
				}

				bDieFound = TRUE;			//done; target index found
				break;
			}
		}

		ulIndexCount++;		//actual index in rectangular bin area
	} 
	while (ulIndexCount <= GrabGradeCapacity(1));

	if (!bDieFound)
	{
		return FALSE;
	}

	ulIndex	= ulIndexCount;				//actual index replaces logical index
	
	if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ||
		(ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH))
	{
		if ((lLastRowDieCount == 0) && (lLastLastRowDieCount == 1))
		{
			//only one row
			ulLastIndex1 = 1;
			//Remove the 1st - 4th die if the number of total die more than 8 die
			while ((ulLastIndex1 <= 4) && ((ulLastIndex1 + 8) < ulDieCount))
			{
				ulLastIndex1++;
			}
			ulLastIndex2 = ulDieCount;
		}
		else
		{
			ulLastIndex1 = lLastLastRowDieCount + 1;
			if (lLastLastRowDieCount == 0)
			{
				//Only two rows
				//Remove the 1st - 4th die if the number of total die more than 8 die
				while ((ulLastIndex1 <= 4) && ((ulLastIndex1 + 8) < (ULONG)lLastRowDieCount))
				{
					ulLastIndex1++;
				}
			}
			ulLastIndex2 = lLastRowDieCount;
		}

		//only one row
		if (lLastRow == 0)
		{
			lLastRow = lBinRow;
		}
		if (lLastLastRow == 0)
		{
			lLastLastRow = lLastRow;
		}

		ulCurBinMapPath = GetCurRowWalkPath(ulWalkPath, (lLastLastRow > 0) ? lLastLastRow - 1 : 0);
	}
	else if ( (ulWalkPath == BT_TRV_PATH) || (ulWalkPath == BT_BRV_PATH) ||
			  (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_BLV_PATH) )
	{
		if ((lLastRowDieCount == 0) && (lLastLastRowDieCount == 1))
		{
			//only one column
			ulLastIndex1 = 1;
			//Remove the 1st - 4th die if the number of total die more than 8 die
			while ((ulLastIndex1 <= 4) && ((ulLastIndex1 + 8) < ulDieCount))
			{
				ulLastIndex1++;
			}
			ulLastIndex2 = ulDieCount;
		}
		else
		{
			ulLastIndex1 = lLastLastColDieCount + 1;
			if (lLastLastColDieCount == 0)
			{
				//only two column
				//Remove the 1st - 4th die if the number of total die more than 8 die
				while ((ulLastIndex1 <= 4) && ((ulLastIndex1 + 8) < (ULONG)lLastColDieCount))
				{
					ulLastIndex1++;
				}
			}
			ulLastIndex2 = lLastColDieCount;
		}

		//only one column
		if (lLastCol == 0)
		{
			lLastCol = lBinCol;
		}
		if (lLastLastCol == 0)
		{
			lLastLastCol = lLastCol;
		}

		ulCurBinMapPath = GetCurColumnWalkPath(ulWalkPath, (lLastLastCol > 0) ? lLastLastCol - 1 : 0);
	}
	
	return TRUE;
}


ULONG CBinBlkMain::GetCurRowWalkPath(const ULONG ulWalkPath, const ULONG ulRow)
{
	ULONG ulCurBinMapPath = BT_TLV_PATH;

	if (ulWalkPath == BT_TLH_PATH)
	{
		ulCurBinMapPath	= (ulRow % 2 == 0) ? BT_TLH_PATH : BT_TRH_PATH;
	}
	else if (ulWalkPath == BT_TRH_PATH)
	{
		ulCurBinMapPath	= (ulRow % 2 == 0) ? BT_TRH_PATH : BT_TLH_PATH;
	}
	else if (ulWalkPath == BT_BLH_PATH)
	{
		ulCurBinMapPath	= (ulRow % 2 == 0) ? BT_BLH_PATH : BT_BRH_PATH;
	}
	else if (ulWalkPath == BT_BRH_PATH)
	{
		ulCurBinMapPath	= (ulRow % 2 == 0) ? BT_BRH_PATH : BT_BLH_PATH;
	}

	return ulCurBinMapPath;
}


ULONG CBinBlkMain::GetCurColumnWalkPath(const ULONG ulWalkPath, const ULONG ulCol)
{
	ULONG ulCurBinMapPath = BT_TLV_PATH;

	if (ulWalkPath == BT_TLV_PATH)
	{
		ulCurBinMapPath	= (ulCol % 2 == 0) ? BT_TLV_PATH : BT_BLV_PATH;
	}
	else if (ulWalkPath == BT_BLV_PATH)
	{
		ulCurBinMapPath	= (ulCol % 2 == 0) ? BT_BLV_PATH : BT_TLV_PATH;
	}
	else if (ulWalkPath == BT_TRV_PATH)
	{
		ulCurBinMapPath	= (ulCol % 2 == 0) ? BT_TRV_PATH : BT_BRV_PATH;
	}
	else if (ulWalkPath == BT_BRV_PATH)
	{
		ulCurBinMapPath	= (ulCol % 2 == 0) ? BT_BRV_PATH : BT_TRV_PATH;
	}

	return ulCurBinMapPath;
}

BOOL CBinBlkMain::GrabEnableBinMap(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		CNGGrade *pNGGrade = CNGGrade::Instance();
		if (pNGGrade->IsNGBlock(ulBlkId)&& !CMS896AApp::m_bNGWithBinMap)
		{
			return FALSE;
		}//1.1777
		return m_oBinBlk[ulBlkId].m_bEnableBinMap;
	}
	return FALSE;
}

VOID CBinBlkMain::SetEnableBinMap(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		CNGGrade *pNGGrade = CNGGrade::Instance();
		if (pNGGrade->IsNGBlock(ulBlkId) && !CMS896AApp::m_bNGWithBinMap)
		{
			m_oBinBlk[ulBlkId].m_bEnableBinMap = FALSE;//1.1777
		}
		else
		{
			m_oBinBlk[ulBlkId].m_bEnableBinMap = m_bEnableBinMapBondArea;
		}
	}
}


BOOL CBinBlkMain::SetUseBinMapBondArea(BOOL bEnable)		//v3.74T45	//PLLM REBEL
{
	m_bEnableBinMapBondArea = bEnable;

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++) 
	{
		SetEnableBinMap(i);
		if (pBTfile != NULL)
		{
			(*pBTfile)["BinBlock"][i]["Bin Map"]["Enable"]	= m_oBinBlk[i].m_bEnableBinMap;
		}
	}

	if (pBTfile != NULL)
	{
		CMSFileUtility::Instance()->SaveBTConfig();
	}

	return TRUE;
}

//v4.42T6
BOOL CBinBlkMain::SetUseBinMapOffset(ULONG ulBlkId, BOOL bEnable, LONG lRowOffset, LONG lColOffset)
{
	if (ulBlkId > (BT_MAX_BINBLK_SIZE-1))
		return FALSE;

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
		return FALSE;

	for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++) 
	{
		m_oBinBlk[i].SetBinMapOffset(bEnable, lRowOffset, lColOffset);

		if (pBTfile != NULL)
		{
			(*pBTfile)["BinBlock"][i]["Bin Map"]["Enable Offset"]	= m_oBinBlk[i].GetEnableBinMapOffset();
			(*pBTfile)["BinBlock"][i]["Bin Map"]["Row Offset"]		= m_oBinBlk[i].GetBinMapRowOffset();
			(*pBTfile)["BinBlock"][i]["Bin Map"]["Col Offset"]		= m_oBinBlk[i].GetBinMapColOffset();
		}
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
}

BOOL CBinBlkMain::SetSpGradeToByPassBinMap(CONST INT ulBlkToUse, CONST UCHAR ucSpGrade)		//v3.74T45	//PLLM REBEL
{
	if (!IsUseBinMapBondArea(ulBlkToUse))
	{
		return FALSE;
	}
	m_ucSpGrade1ToByPassBinMap	= ucSpGrade;
	return TRUE;
}

UCHAR CBinBlkMain::GetSpGradeToByPassBinMap(CONST INT ulBlkToUse)	//v4.48A4
{
	if (!IsUseBinMapBondArea(ulBlkToUse))
	{
		return 0;
	}
	return m_ucSpGrade1ToByPassBinMap;
}

BOOL CBinBlkMain::SetOSRAMBinMixA(CONST INT nIndex, CONST UCHAR ucBinMixA)		//v3.74T45	//PLLM REBEL
{
	m_ucBinMixTypeAGrade	= ucBinMixA;
	return TRUE;
}

BOOL CBinBlkMain::SetOSRAMBinMixB(CONST INT nIndex, CONST UCHAR ucBinMixB)		//v3.74T45	//PLLM REBEL
{
	m_ucBinMixTypeBGrade	= ucBinMixB;
	return TRUE;
}

BOOL CBinBlkMain::EnableOSRAMBinMixMap(CONST INT nIndex, CONST BOOL bEnableOSRAMBinMixMap)		//v4.48
{
	m_bEnableOsramBinMixMap	= bEnableOSRAMBinMixMap;
	return TRUE;
}

//BOOL CBinBlkMain::SetBinMixOtherValue(CONST INT nIndex, CONST LONG lBinMixTypeAQty,CONST LONG lBinMixTypeBQty,
//			CONST LONG lBinMixTypeAStopPoint,CONST LONG lBinMixTypeBStopPoint)		//v4.48
//{
//	m_lBinMixTypeAQty			= lBinMixTypeAQty;
//	m_lBinMixTypeBQty			= lBinMixTypeBQty;
//	m_lBinMixTypeAStopPoint		= lBinMixTypeAStopPoint;
//	m_lBinMixTypeBStopPoint		= lBinMixTypeBStopPoint;
//	return TRUE;
//}
	

BOOL CBinBlkMain::SetBinMixPatternType(CONST INT nIndex, CONST LONG ulBinMixPatternType)		
{
	m_ulBinMixPatternType	= ulBinMixPatternType;
	return TRUE;
}

BOOL CBinBlkMain::SaveAllBinMapData()
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++) 
	{
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Enable"]			= m_oBinBlk[i].m_bEnableBinMap;
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Enable Offset"]	= m_oBinBlk[i].GetEnableBinMapOffset();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Row Offset"]		= m_oBinBlk[i].GetBinMapRowOffset();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Col Offset"]		= m_oBinBlk[i].GetBinMapColOffset();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Walk Path"]		= m_oBinBlk[i].GetBinMapWalkPath();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["CirRadius"]		= m_oBinBlk[i].GetBinMapCircleRadius();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Top Edge"]		= m_oBinBlk[i].GetBinMapTopEdge();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Btm Edge"]		= m_oBinBlk[i].GetBinMapBtmEdge();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Left Edge"]		= m_oBinBlk[i].GetBinMapLeftEdge();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Right Edge"]		= m_oBinBlk[i].GetBinMapRightEdge();
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
}

BOOL CBinBlkMain::GetBinMapOffset(ULONG ulBlkId, LONG& lRowOffset, LONG& lColOffset)
{
	if (ulBlkId > (BT_MAX_BINBLK_SIZE-1))
		return FALSE;
	lRowOffset = m_oBinBlk[ulBlkId].GetBinMapRowOffset();
	lColOffset = m_oBinBlk[ulBlkId].GetBinMapColOffset();
	return m_oBinBlk[ulBlkId].GetEnableBinMapOffset();
}

VOID CBinBlkMain::UpdateBinMapFile()
{
	//print out binmap array only when the BINMAP fcn is enabled (PLSG)
	if (!m_bEnableBinMapBondArea)
	{
		return;
	}

	CStdioFile oFile;
	if (oFile.Open("c:\\MapSorter\\Binmap.xxx", CFile::modeCreate | CFile::modeWrite | CFile::typeText) != TRUE)
	{
		return;
	}

	for (INT m = 0; m < MAX_BINMAP_SIZE_X; m++)
	{
		CString szLine;
		for (INT n = 0; n < MAX_BINMAP_SIZE_Y; n++)
		{
			if (CBinBlk::m_nBinMap[m][n] == 1)
			{
				szLine = szLine + "1";
			}
			else
			{
				szLine = szLine + "0";
			}
		}

		oFile.WriteString(szLine + "\n");
	}

	oFile.Close();
}

ULONG CBinBlkMain::GrabBinMapWalkPath(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		//Nichia//v4.43T7
		//return m_oBinBlk[ulBlkId].GetBinMapWalkPath();
		return m_oBinBlk[ulBlkId].GetWalkPath();

	}
	else
	{
		return 0;
	}
}

VOID CBinBlkMain::SetBinMapCircleRadius(ULONG ulBlkId, DOUBLE dRadiusInUm)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].SetBinMapCircleRadius(dRadiusInUm);
	}
	else if (ulBlkId == 0)
	{
		for (INT i=1; i<=(BT_MAX_BINBLK_SIZE - 1); i++)
		{
			m_oBinBlk[i].SetBinMapCircleRadius(dRadiusInUm);
		}
	}
	else
	{
		return;
	}
}

VOID CBinBlkMain::SetBinMapEdgeSize(ULONG ulBlkId, LONG lTEdgeSize, LONG lBEdgeSize, 
									LONG lLEdgeSize, LONG lREdgeSize)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].SetBinMapEdgeSize(lTEdgeSize, lBEdgeSize, lLEdgeSize, lREdgeSize);
	}
	else if (ulBlkId == 0)
	{
		for (INT i=1; i<=(BT_MAX_BINBLK_SIZE - 1); i++)
		{
			m_oBinBlk[i].SetBinMapEdgeSize(lTEdgeSize, lBEdgeSize, lLEdgeSize, lREdgeSize);
		}
	}
	else
	{
		return;
	}
}

BOOL CBinBlkMain::DrawBinMap(ULONG ulBlkId)	//Nichia//v4.43T7
{
	CString szAlgorithm;
	CString szDummyWaferMapName;
	CString szErr;

	unsigned char aucAvailableGrades[1] = {'1'};
	unsigned char aaTempGrades[1];

	aucAvailableGrades[0] = 48 + (UCHAR) ulBlkId;
	aaTempGrades[0] = 48 + (UCHAR) ulBlkId;

	unsigned char* pGradeBuffer;
	unsigned char** aaGrades;
	int i, j;


	szErr.Format("CBinBlkMain: DrawBinMap for BLK #%d ...", ulBlkId);
//AfxMessageBox(szErr, MB_SYSTEMMODAL);

	if ((ulBlkId < 1) || (ulBlkId > BT_MAX_BINBLK_SIZE - 1))	//v4.46T20	//Klocwork
	{
		return FALSE;
	}

	LONG lNoOfRows = GrabNoOfDiePerCol(ulBlkId);
	LONG lNoOfCols = GrabNoOfDiePerRow(ulBlkId);
	if ( (lNoOfRows == 0) || (lNoOfCols == 0) )
	{
		szErr.Format("ERROR: DrawBinMap: Invalid die row & col (%ld, %ld)", lNoOfRows, lNoOfCols);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
		return FALSE;
	}

	if ((lNoOfRows > 300) || (lNoOfCols > 300))
	{
		szErr = "ERROR: DrawBinMap: no or rows & cols exceeds 300";
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
		return FALSE;
	}

	szErr.Format("CBinBlkMain: BLK #%d no of rows = %ld, cols = %ld", ulBlkId, lNoOfRows, lNoOfCols);
//AfxMessageBox(szErr, MB_SYSTEMMODAL);

	DOUBLE dRadiusInUm = m_oBinBlk[ulBlkId].GetBinMapCircleRadius();
	szErr.Format("BLK #%d BinMap radius = %.1f um", ulBlkId, dRadiusInUm);
//AfxMessageBox(szErr, MB_SYSTEMMODAL);

	//LONG lCRow = lNoOfRows / 2;
	//LONG lCCol = lNoOfCols / 2;
	//m_oBinBlk[ulBlkId].SetBinMapOffset(TRUE, lCRow, lCCol);

	LONG lCRow = m_oBinBlk[ulBlkId].GetBinMapRowOffset();
	LONG lCCol = m_oBinBlk[ulBlkId].GetBinMapColOffset();
	szErr.Format("BLK #%d BinMap center at (%ld, %ld)", ulBlkId, lCRow, lCCol);
//AfxMessageBox(szErr, MB_SYSTEMMODAL);


	// Reduce memory fragmentation
	pGradeBuffer = new unsigned char[lNoOfRows*lNoOfCols];
	aaGrades = new unsigned char*[lNoOfRows];
	for (i=0; i<lNoOfRows; i++) 
	{
		aaGrades[i] = &pGradeBuffer[lNoOfCols*i]; //new unsigned char[lNoOfCols];
	}


	// Set the grades
	for (i=0; i<lNoOfRows; i++) 
	{
		for (j=0; j<lNoOfCols; j++) 
		{
			aaGrades[i][j] = WAF_CMapConfiguration::DEFAULT_NULL_BIN;
		}
	}


	//Clear Wafermap & reset grade map
	CMS896AStn::m_BinMapWrapper.InitMap();
	szDummyWaferMapName = "BinMap";	
	CMS896AStn::m_BinMapWrapper.CreateMap(szDummyWaferMapName, "Raw", lNoOfRows, lNoOfCols, aaGrades, aucAvailableGrades, 1);


	if (!m_oBinBlk[ulBlkId].m_bEnableBinMap)
	{
		szErr.Format("CBinBlkMain: BLK #%d BinMap is not enabled!", ulBlkId);
		AfxMessageBox(szErr, MB_SYSTEMMODAL);
		
		delete [] aaGrades;
		delete [] pGradeBuffer;
		return FALSE;
	}


	UCHAR ucGradeOffset = CMS896AStn::m_BinMapWrapper.GetGradeOffset();
	UCHAR ucGrade		= ucGradeOffset + (UCHAR) ulBlkId;

	LONG lMapRow=0, lMapCol=0;
	for (lMapRow=0; lMapRow<lNoOfRows; lMapRow++)
	{
		for (lMapCol=0; lMapCol<lNoOfCols; lMapCol++)
		{			
			if (m_oBinBlk[ulBlkId].IsDieInBinMap(lMapRow, lMapCol))
			{
				CMS896AStn::m_BinMapWrapper.AddDie(lMapRow, lMapCol, ucGrade);
			}
		}
	}

	delete [] aaGrades;
	delete [] pGradeBuffer;
	return TRUE;
}

ULONG CBinBlkMain::CalclateBinMapCapacity(ULONG ulBlkId)
{
	ULONG ulCount = 0;
	LONG lMapRow=0, lMapCol=0;
	LONG lNoOfRows = GrabNoOfDiePerCol(ulBlkId);
	LONG lNoOfCols = GrabNoOfDiePerRow(ulBlkId);

	//v4.50A3	//WH SanAn	//LeoLam
	if (IsUseBinMapBondArea(ulBlkId))	
	{
		for (lMapRow = 0; lMapRow < MAX_BINMAP_SIZE_Y; lMapRow++)
		{
			for (lMapCol = 0; lMapCol < MAX_BINMAP_SIZE_X; lMapCol++)
			{			
				if (m_nBinMap[lMapRow][lMapCol] == 1)
				{
					ulCount++;
				}
			}
		}
	}
	else if (CMS896AStn::m_oNichiaSubSystem.IsEnabled() && m_oBinBlk[ulBlkId].m_bEnableBinMap)	
	{
		for (lMapRow=0; lMapRow<lNoOfRows; lMapRow++)
		{
			for (lMapCol=0; lMapCol<lNoOfCols; lMapCol++)
			{			
				if (m_oBinBlk[ulBlkId].IsDieInBinMap(lMapRow, lMapCol))
				{
					ulCount++;
				}
			}
		}
	}

	return ulCount;
}


/***********************************/
/*     Save to file functions      */
/***********************************/

BOOL CBinBlkMain::SaveAllBinBlkSetupData(ULONG ulNoOfBlk, LONG lBlkPitchX, LONG lBlkPitchY, BOOL bUpdateGradeData)
{
	ULONG i;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	//Write new values
	(*pBTfile)["BinBlock"]["NoOfBlk"] = ulNoOfBlk;
	(*pBTfile)["BinBlock"]["BlkPitchX"] = lBlkPitchX;
	(*pBTfile)["BinBlock"]["BlkPitchY"] = lBlkPitchY;

	for (i = 1; i <= ulNoOfBlk; i++)
	{
		(*pBTfile)["BinBlock"][i]["UpperLeftX"] = m_oBinBlk[i].GetUpperLeftX();
		(*pBTfile)["BinBlock"][i]["UpperLeftXFromSetup"] = m_oBinBlk[i].GetUpperLeftXFromSetup();
		
		(*pBTfile)["BinBlock"][i]["UpperLeftY"] = m_oBinBlk[i].GetUpperLeftY();
		(*pBTfile)["BinBlock"][i]["UpperLeftYFromSetup"] = m_oBinBlk[i].GetUpperLeftYFromSetup();

		(*pBTfile)["BinBlock"][i]["LowerRightX"] = m_oBinBlk[i].GetLowerRightX();
		(*pBTfile)["BinBlock"][i]["LowerRightXFromSetup"] = m_oBinBlk[i].GetLowerRightXFromSetup();

		(*pBTfile)["BinBlock"][i]["LowerRightY"] = m_oBinBlk[i].GetLowerRightY();
		(*pBTfile)["BinBlock"][i]["LowerRightYFromSetup"] = m_oBinBlk[i].GetLowerRightYFromSetup();

		//(*pBTfile)["BinBlock"][i]["SerialNo"] = m_oBinBlk[i].GetSerialNo();
		//(*pBTfile)["BinBlock"][i]["LastResetSerialNoDate"] = m_oBinBlk[i].GetSerialNo();

		(*pBTfile)["BinBlock"][i]["Grade"]		= m_oBinBlk[i].GetGrade();
		(*pBTfile)["BinBlock"][i]["DiePitchX"]	= m_oBinBlk[i].GetDDiePitchX();				//v4.59A19
		(*pBTfile)["BinBlock"][i]["DiePitchY"]	= m_oBinBlk[i].GetDDiePitchY();				//v4.59A19
		(*pBTfile)["BinBlock"][i]["Pt5DiePitchX"]= m_oBinBlk[i].GetPoint5UmInDiePitchX();	//v4.42T5
		(*pBTfile)["BinBlock"][i]["Pt5DiePitchY"]= m_oBinBlk[i].GetPoint5UmInDiePitchY();	//v4.42T5
		(*pBTfile)["BinBlock"][i]["BondAreaOffsetX"]= (DOUBLE) m_oBinBlk[i].GetBondAreaOffsetX();	//v4.42T9
		(*pBTfile)["BinBlock"][i]["BondAreaOffsetY"]= (DOUBLE) m_oBinBlk[i].GetBondAreaOffsetY();	//v4.42T9
		(*pBTfile)["BinBlock"][i]["ThermalDeltaPitchX"]= (DOUBLE) m_oBinBlk[i].GetThermalDeltaPitchX();	//v4.59A22
		(*pBTfile)["BinBlock"][i]["ThermalDeltaPitchY"]= (DOUBLE) m_oBinBlk[i].GetThermalDeltaPitchY();	//v4.59A22
		(*pBTfile)["BinBlock"][i]["WalkPath"]	= m_oBinBlk[i].GetWalkPath();
		(*pBTfile)["BinBlock"][i]["SkipUnit"]	= m_oBinBlk[i].GetSkipUnit();
		(*pBTfile)["BinBlock"][i]["MaxUnit"]	= m_oBinBlk[i].GetMaxUnit();
		(*pBTfile)["BinBlock"][i]["DiePerBlk"]	= m_oBinBlk[i].GetDiePerBlk();
		(*pBTfile)["BinBlock"][i]["DiePerRow"]	= m_oBinBlk[i].GetDiePerRow();
		(*pBTfile)["BinBlock"][i]["DiePerCol"]	= m_oBinBlk[i].GetDiePerCol();		//pllm
		(*pBTfile)["BinBlock"][i]["IsDisableFromSameGradeMerge"] = 
			m_oBinBlk[i].GetIsDisableFromSameGradeMerge();
		(*pBTfile)["BinBlock"][i]["PhyBlkId"]	= m_oBinBlk[i].GetPhyBlkId();
		(*pBTfile)["BinBlock"][i]["IsSetup"]	= m_oBinBlk[i].GetIsSetup();

		(*pBTfile)["BinBlock"][i]["Status"]	= m_oBinBlk[i].GetStatus();
		(*pBTfile)["BinBlock"][i]["Original Grade"] = m_oBinBlk[i].GetOriginalGrade();
		(*pBTfile)["BinBlock"][i]["EmptyUnit"]	= m_oBinBlk[i].GetEmptyUnit();

		(*pBTfile)["BinBlock"][i]["Enbale FRSP"]	= m_oBinBlk[i].GetFirstRowColSkipPattern();
		(*pBTfile)["BinBlock"][i]["FRSP Skip Unit"] = m_oBinBlk[i].GetFirstRowColSkipUnit();

		(*pBTfile)["BinBlock"][i]["Set With Physical Blk"]	= m_oBinBlk[i].GetIsTeachWithPhysicalBlk();

		(*pBTfile)["BinBlock"][i]["Enbale Centralized"]	= m_oBinBlk[i].GetIsCentralizedBondArea();
		(*pBTfile)["BinBlock"][i]["Centralized Offset X"] = m_oBinBlk[i].GetCentralizedOffsetX();
		(*pBTfile)["BinBlock"][i]["Centralized Offset Y"] = m_oBinBlk[i].GetCentralizedOffsetY();

		(*pBTfile)["BinBlock"][i]["Enbale Waffle Pad"] = m_oBinBlk[i].GetIsEnableWafflePad();
		(*pBTfile)["BinBlock"][i]["Waffle Pad Dist X"] = m_oBinBlk[i].GetWafflePadDistX();
		(*pBTfile)["BinBlock"][i]["Waffle Pad Dist Y"] = m_oBinBlk[i].GetWafflePadDistY();
		(*pBTfile)["BinBlock"][i]["Waffle Pad Size X"] = m_oBinBlk[i].GetWafflePadSizeX();
		(*pBTfile)["BinBlock"][i]["Waffle Pad Size Y"] = m_oBinBlk[i].GetWafflePadSizeY();

		//v3.70T3	//Circular bin sorting are fcn support
		(*pBTfile)["BinBlock"][i]["Use Circular Bin Area"]	= m_oBinBlk[i].GetUseCircularArea();
		(*pBTfile)["BinBlock"][i]["Circular Bin Radius"]	= m_oBinBlk[i].GetRadius();
		(*pBTfile)["BinBlock"][i]["Circular Bin CenterX"]	= m_oBinBlk[i].GetCirBinCenterX();
		(*pBTfile)["BinBlock"][i]["Circular Bin CenterY"]	= m_oBinBlk[i].GetCirBinCenterY();
		//v4.42T6
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Enable"]			= m_oBinBlk[i].m_bEnableBinMap;
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Enable Offset"]	= m_oBinBlk[i].GetEnableBinMapOffset();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Row Offset"]		= m_oBinBlk[i].GetBinMapRowOffset();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Col Offset"]		= m_oBinBlk[i].GetBinMapColOffset();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Walk Path"]		= m_oBinBlk[i].GetBinMapWalkPath();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["CirRadius"]		= m_oBinBlk[i].GetBinMapCircleRadius();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Top Edge"]		= m_oBinBlk[i].GetBinMapTopEdge();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Btm Edge"]		= m_oBinBlk[i].GetBinMapBtmEdge();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Left Edge"]		= m_oBinBlk[i].GetBinMapLeftEdge();
		(*pBTfile)["BinBlock"][i]["Bin Map"]["Right Edge"]		= m_oBinBlk[i].GetBinMapRightEdge();

		(*pBTfile)["BinBlock"][i]["Use Block Corner As First Die Pos"] = m_oBinBlk[i].GetIsUseBlockCornerAsFirstDiePos();
		(*pBTfile)["BinBlock"][i]["No Return Travel"]			= m_oBinBlk[i].GetNoReturnTravel();		//v4.42T1

		(*pBTfile)["BinBlock"][i]["ByPassBinMap"]				= m_oBinBlk[i].GetByPassBinMap();	//v4.48A11
	}

	if (bUpdateGradeData == TRUE)
	{
		for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
		{
			(*pBTfile)["BinGradeData"][i]["GradeCapacity"] = 
				m_oBinGradeData[i].GetGradeCapacity();
			(*pBTfile)["BinGradeData"][i]["InputCount"] =
				m_oBinGradeData[i].GetInputCount();
			
			(*pBTfile)["BinGradeData"][i]["Max Frame Die Count"] = m_oBinGradeData[i].GetMaxFrameDieCount();

			(*pBTfile)["BinGradeData"][i]["Min Frame Die Count"] = m_oBinGradeData[i].GetMinFrameDieCount();

			(*pBTfile)["BinGradeData"][i]["AliasGrade"] = 
				m_oBinGradeData[i].GetAliasGrade();
			(*pBTfile)["BinGradeData"][i]["IsAssigned"] = 
				m_oBinGradeData[i].GetIsAssigned();
		}
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
} //end SaveAllBinBlkSetupData


BOOL CBinBlkMain::SaveSingleBinBlkSetupData(ULONG ulNoOfBlk, ULONG ulBlkToSetup, UCHAR ucBlkOrgGrade, BOOL bUpdateGradeData)
{
	ULONG i;
	UCHAR ucGrade;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}
	if (ulNoOfBlk >= BT_MAX_BINBLK_SIZE)		//v4.46T20	//Klocwork
	{
		CMSFileUtility::Instance()->CloseBTConfig();	//v4.47T1
		return FALSE;
	}
	if (ulBlkToSetup >= BT_MAX_BINBLK_SIZE)		//Klocwork	//v4.46
	{
		CMSFileUtility::Instance()->CloseBTConfig();	//v4.47T1
		return FALSE;
	}

	//Remove old values
	for (i = ulNoOfBlk + 1; i <= (*pBTfile)["BinBlock"]["NoOfBlk"]; i++)
	{
		(*pBTfile)["BinBlock"].Remove(i);
	}

	//Write new values
	(*pBTfile)["BinBlock"]["NoOfBlk"] = ulNoOfBlk;

	(*pBTfile)["BinBlock"][ulBlkToSetup]["UpperLeftX"] = m_oBinBlk[ulBlkToSetup].GetUpperLeftX();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["UpperLeftXFromSetup"] = 
		m_oBinBlk[ulBlkToSetup].GetUpperLeftXFromSetup();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["UpperLeftY"] = m_oBinBlk[ulBlkToSetup].GetUpperLeftY();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["UpperLeftYFromSetup"] = 
		m_oBinBlk[ulBlkToSetup].GetUpperLeftYFromSetup();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["LowerRightX"] = m_oBinBlk[ulBlkToSetup].GetLowerRightX();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["LowerRightXFromSetup"] = 
		m_oBinBlk[ulBlkToSetup].GetLowerRightXFromSetup();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["LowerRightY"] = m_oBinBlk[ulBlkToSetup].GetLowerRightY();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["LowerRightYFromSetup"] = 
		m_oBinBlk[ulBlkToSetup].GetLowerRightYFromSetup();

	if (bUpdateGradeData == TRUE)
	{
		//save BinGradeData values
		ucGrade = m_oBinBlk[ulBlkToSetup].GetGrade();
		if ( (ucGrade > BT_MAX_BINBLK_SIZE - 1) || 
			 (ucBlkOrgGrade > BT_MAX_BINBLK_SIZE - 1) )		//Klocwork	//v4.46
		{
			CMSFileUtility::Instance()->SaveBTConfig();
			return FALSE;
		}

		(*pBTfile)["BinBlock"][ulBlkToSetup]["Grade"] = ucGrade;
		(*pBTfile)["BinGradeData"][ucGrade]["GradeCapacity"] = m_oBinGradeData[ucGrade].GetGradeCapacity();
		
		//Disable AliasGrade function
		(*pBTfile)["BinGradeData"][ucGrade]["AliasGrade"] = 0;

		(*pBTfile)["BinGradeData"][ucGrade]["InputCount"] = m_oBinGradeData[ucGrade].GetInputCount();
		(*pBTfile)["BinGradeData"][ucGrade]["IsAssigned"] = m_oBinGradeData[ucGrade].GetIsAssigned();

		(*pBTfile)["BinGradeData"][ucBlkOrgGrade]["GradeCapacity"] = 
			m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity();
		(*pBTfile)["BinGradeData"][ucBlkOrgGrade]["AliasGrade"] = 
			m_oBinGradeData[ucBlkOrgGrade].GetAliasGrade();
		(*pBTfile)["BinGradeData"][ucBlkOrgGrade]["InputCount"] =
			m_oBinGradeData[ucBlkOrgGrade].GetInputCount();

		(*pBTfile)["BinGradeData"][ucBlkOrgGrade]["Max Frame Die Count"] = 
			m_oBinGradeData[ucBlkOrgGrade].GetMaxFrameDieCount();

		(*pBTfile)["BinGradeData"][ucBlkOrgGrade]["Min Frame Die Count"] = 
			m_oBinGradeData[ucBlkOrgGrade].GetMinFrameDieCount();

		(*pBTfile)["BinGradeData"][ucBlkOrgGrade]["IsAssigned"] =
			m_oBinGradeData[ucBlkOrgGrade].GetIsAssigned();

	}

	//Continue with BinBlock value
	(*pBTfile)["BinBlock"][ulBlkToSetup]["DiePitchX"]	= m_oBinBlk[ulBlkToSetup].GetDDiePitchX();			//v4.59A19
	(*pBTfile)["BinBlock"][ulBlkToSetup]["DiePitchY"]	= m_oBinBlk[ulBlkToSetup].GetDDiePitchY();			//v4.59A19
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Pt5DiePitchX"]= m_oBinBlk[ulBlkToSetup].GetPoint5UmInDiePitchX();	//v4.42T5
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Pt5DiePitchY"]= m_oBinBlk[ulBlkToSetup].GetPoint5UmInDiePitchY();	//v4.42T5
	(*pBTfile)["BinBlock"][ulBlkToSetup]["BondAreaOffsetX"]= (DOUBLE) m_oBinBlk[ulBlkToSetup].GetBondAreaOffsetX();	//v4.42T9
	(*pBTfile)["BinBlock"][ulBlkToSetup]["BondAreaOffsetY"]= (DOUBLE) m_oBinBlk[ulBlkToSetup].GetBondAreaOffsetY();	//v4.42T9
	(*pBTfile)["BinBlock"][ulBlkToSetup]["ThermalDeltaPitchX"]= (DOUBLE) m_oBinBlk[ulBlkToSetup].GetThermalDeltaPitchX();	//v4.59A22
	(*pBTfile)["BinBlock"][ulBlkToSetup]["ThermalDeltaPitchY"]= (DOUBLE) m_oBinBlk[ulBlkToSetup].GetThermalDeltaPitchY();	//v4.59A22

CString szLog;
szLog.Format("SaveSingleBinBlkSetupData[%d]: %f %f", ulBlkToSetup,
			 m_oBinBlk[ulBlkToSetup].GetThermalDeltaPitchX(),
			 m_oBinBlk[ulBlkToSetup].GetThermalDeltaPitchY());
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	(*pBTfile)["BinBlock"][ulBlkToSetup]["WalkPath"]	= m_oBinBlk[ulBlkToSetup].GetWalkPath();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["SkipUnit"]	= m_oBinBlk[ulBlkToSetup].GetSkipUnit();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["MaxUnit"]		= m_oBinBlk[ulBlkToSetup].GetMaxUnit();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["DiePerBlk"]	= m_oBinBlk[ulBlkToSetup].GetDiePerBlk();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["DiePerRow"]	= m_oBinBlk[ulBlkToSetup].GetDiePerRow();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["DiePerCol"]	= m_oBinBlk[ulBlkToSetup].GetDiePerCol();		//pllm
	(*pBTfile)["BinBlock"][ulBlkToSetup]["IsDisableFromSameGradeMerge"] = 
		m_oBinBlk[ulBlkToSetup].GetIsDisableFromSameGradeMerge();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["PhyBlkId"]	= m_oBinBlk[ulBlkToSetup].GetPhyBlkId();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["IsSetup"]		= m_oBinBlk[ulBlkToSetup].GetIsSetup();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["Status"]	= m_oBinBlk[ulBlkToSetup].GetStatus();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Original Grade"] = m_oBinBlk[ulBlkToSetup].GetOriginalGrade();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["EmptyUnit"]	= m_oBinBlk[ulBlkToSetup].GetEmptyUnit();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["Enbale FRSP"]	= m_oBinBlk[ulBlkToSetup].GetFirstRowColSkipPattern();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["FRSP Skip Unit"] = m_oBinBlk[ulBlkToSetup].GetFirstRowColSkipUnit();

	//Klocwork	//v4.46
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Set With Physical Blk"]	= m_oBinBlk[ulBlkToSetup].GetIsTeachWithPhysicalBlk();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["Enbale Centralized"]	= m_oBinBlk[ulBlkToSetup].GetIsCentralizedBondArea();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Centralized Offset X"] = m_oBinBlk[ulBlkToSetup].GetCentralizedOffsetX();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Centralized Offset Y"] = m_oBinBlk[ulBlkToSetup].GetCentralizedOffsetY();
	//v4.42T6
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Enable"]		= m_oBinBlk[ulBlkToSetup].m_bEnableBinMap;
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Enable Offset"]= m_oBinBlk[ulBlkToSetup].GetEnableBinMapOffset();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Row Offset"]	= m_oBinBlk[ulBlkToSetup].GetBinMapRowOffset();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Col Offset"]	= m_oBinBlk[ulBlkToSetup].GetBinMapColOffset();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Walk Path"]	= m_oBinBlk[ulBlkToSetup].GetBinMapWalkPath();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["CirRadius"]	= m_oBinBlk[ulBlkToSetup].GetBinMapCircleRadius();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Top Edge"]		= m_oBinBlk[ulBlkToSetup].GetBinMapTopEdge();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Btm Edge"]		= m_oBinBlk[ulBlkToSetup].GetBinMapBtmEdge();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Left Edge"]	= m_oBinBlk[ulBlkToSetup].GetBinMapLeftEdge();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Bin Map"]["Right Edge"]	= m_oBinBlk[ulBlkToSetup].GetBinMapRightEdge();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["Enbale Waffle Pad"]	= m_oBinBlk[ulBlkToSetup].GetIsEnableWafflePad();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Waffle Pad Dist X"]	= m_oBinBlk[ulBlkToSetup].GetWafflePadDistX();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Waffle Pad Dist Y"]	= m_oBinBlk[ulBlkToSetup].GetWafflePadDistY();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Waffle Pad Size X"]	= m_oBinBlk[ulBlkToSetup].GetWafflePadSizeX();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["Waffle Pad Size Y"]	= m_oBinBlk[ulBlkToSetup].GetWafflePadSizeY();

	(*pBTfile)["BinBlock"][ulBlkToSetup]["Use Block Corner As First Die Pos"] = m_oBinBlk[ulBlkToSetup].GetIsUseBlockCornerAsFirstDiePos();
	(*pBTfile)["BinBlock"][ulBlkToSetup]["No Return Travel"]	= m_oBinBlk[ulBlkToSetup].GetNoReturnTravel();		//v4.42T1

	(*pBTfile)["BinBlock"][ulBlkToSetup]["ByPassBinMap"]		= m_oBinBlk[ulBlkToSetup].GetByPassBinMap();	//v4.48A11

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
} //end SaveSingleBinBlkSetupData

LONG CBinBlkMain::SaveBinBlkDataAsTemplate(CString szTemplateName, LONG lBinBlkULX, LONG lBinBlkULY,
		LONG lBinBlkLRX, LONG lBinBlkLRY, CString szWalkPath,
		ULONG ulDiePerRow, ULONG ulDiePerCol,
		DOUBLE dDiePitchX, DOUBLE dDiePitchY,
		ULONG ulSkipUnit, ULONG ulMaxUnit, ULONG ulInputCount, 
		BOOL bEnableFirstRowColSkipPattern, ULONG ulFirstRowColSkipUnit, 
		BOOL bCentralizedBondArea, BOOL bTeachWithPhysicalBlk, 
		BOOL bUseBlockCornerAsFirstDiePos, BOOL bEnableWafflePad,
		LONG lWafflePadSizeX, LONG lWafflePadSizeY, LONG	lWafflePadDistX,
		LONG	lWafflePadDistY)
{
	CStringArray szaTemplateList;
	BOOL bTemplateSelectionExist = FALSE;
	CStdioFile cfBinBlkTemplateFile;
	LONG lTotalNoOfTemplate = 0;
	CString szTemplateIndex, szTemplateNameFromFile;
	BOOL bFoundTemplate = FALSE;

	if (szTemplateName.IsEmpty())
	{
		return FALSE;
	}

	CMSFileUtility::Instance()->LoadBinBlkTemplateConfig();
	CStringMapFile *pBinBlkTemplate = CMSFileUtility::Instance()->GetBinBlkTemplateConfigFile();

	if (pBinBlkTemplate == NULL)
	{
		return FALSE;
	}

	// limit the no of template to 10
	lTotalNoOfTemplate = (*pBinBlkTemplate)["TemplateInfo"]["Total No Of Template"];

	if (lTotalNoOfTemplate >= 10)
	{
		CMSFileUtility::Instance()->CloseBinBlkTemplateConfig();
		return -1;
	}

	for (INT i = 1; i <= (INT) lTotalNoOfTemplate; i++)
	{
		szTemplateIndex.Format("%d", i);
		szTemplateNameFromFile = (*pBinBlkTemplate)["TemplateInfo"]["TemplateName"][szTemplateIndex];
		
		szaTemplateList.Add(szTemplateNameFromFile);

		if (szTemplateNameFromFile == szTemplateName)
		{
			bFoundTemplate = TRUE;
			break;
		}
	}

	// If not found update template list
	if (bFoundTemplate == FALSE)
	{
		szTemplateIndex.Format("%d", lTotalNoOfTemplate + 1);
		(*pBinBlkTemplate)["TemplateInfo"]["Total No Of Template"] = lTotalNoOfTemplate + 1;
		(*pBinBlkTemplate)["TemplateInfo"]["TemplateName"][szTemplateIndex] = szTemplateName;
		szaTemplateList.Add(szTemplateName);
	}
	
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Template Name"] = szTemplateName;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftX"] = lBinBlkULX;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftXFromSetup"] = lBinBlkULX; 

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftY"] = lBinBlkULY;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftYFromSetup"] = lBinBlkULY;
										
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightX"] = lBinBlkLRX;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightXFromSetup"] = lBinBlkLRX;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightY"] = lBinBlkLRY;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightYFromSetup"] = lBinBlkLRY;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePerRow"] = ulDiePerRow;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePerCol"] = ulDiePerCol;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePitchX"]	= dDiePitchX;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePitchY"]	= dDiePitchY;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["WalkPath"]	= szWalkPath;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["SkipUnit"]	= ulSkipUnit;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["MaxUnit"]	= ulMaxUnit;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["InputCount"]	= ulInputCount;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Enbale FRSP"]	= bEnableFirstRowColSkipPattern;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["FRSP Skip Unit"] = ulFirstRowColSkipUnit;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Enbale Centralized"] = bCentralizedBondArea;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Set With Physical Block"] = bTeachWithPhysicalBlk;
	
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Use Block Corner As First Die Pos"] = bUseBlockCornerAsFirstDiePos;

	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Enable Waffle Pad"] = bEnableWafflePad;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Size X"] = lWafflePadSizeX;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Size Y"] = lWafflePadSizeY;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Dist X"] = lWafflePadDistX;
	(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Dist Y"] = lWafflePadDistY;


	CMSFileUtility::Instance()->SaveBinBlkTemplateConfig();
	CMSFileUtility::Instance()->CloseBinBlkTemplateConfig();



	return TRUE;
} 

BOOL CBinBlkMain::LoadBinBlkTemplate(CString szTemplateName, LONG &lBinBlkULX, LONG &lBinBlkULY,
									 LONG &lBinBlkLRX, LONG &lBinBlkLRY, CString &szWalkPath,
									 ULONG &ulDiePerRow, ULONG &ulDiePerCol,
									 DOUBLE &dDiePitchX, DOUBLE &dDiePitchY,
									 ULONG &ulSkipUnit, ULONG &ulMaxUnit,
									 ULONG &ulInputCount, BOOL &bEnableFirstRowColSkipPattern,
									 ULONG &ulFirstRowColSkipUnit, BOOL &bCentralizedBondArea, 
									 BOOL &bTeachWithPhysicalBlk, BOOL &bUseBlockCornerAsFirstDiePos, BOOL &bEnableWafflePad,
									 LONG &lWafflePadSizeX, LONG &lWafflePadSizeY, LONG &lWafflePadDistX,
									 LONG &lWafflePadDistY)
{
	
	LONG lTotalNoOfTemplate = 0;
	BOOL bFoundTemplate = FALSE;
	CString szTemplateNameFromFile, szTemplateIndex;
	CMSFileUtility::Instance()->LoadBinBlkTemplateConfig();
	CStringMapFile *pBinBlkTemplate = CMSFileUtility::Instance()->GetBinBlkTemplateConfigFile();

	if (pBinBlkTemplate == NULL)
	{
		return FALSE;
	}

	lTotalNoOfTemplate = (*pBinBlkTemplate)["TemplateInfo"]["Total No Of Template"];
	
	for (INT i = 1; i <= (INT) lTotalNoOfTemplate; i++)
	{
		szTemplateIndex.Format("%d", i);
		szTemplateNameFromFile = (*pBinBlkTemplate)["TemplateInfo"]["TemplateName"][szTemplateIndex];

	
		if (szTemplateNameFromFile == szTemplateName)
		{
			bFoundTemplate = TRUE;
			break;
		}
	
	}
	
	if (bFoundTemplate == FALSE)
	{
		CMSFileUtility::Instance()->CloseBinBlkTemplateConfig();
		return FALSE;
	}

	if ((*pBinBlkTemplate)["BinBlock"][szTemplateName] != "")
	{
		szTemplateName = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["Template Name"];
		lBinBlkULX = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftX"];
		lBinBlkULX = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftXFromSetup"]; 

		lBinBlkULY = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftY"];
		lBinBlkULY = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["UpperLeftYFromSetup"];
											
		lBinBlkLRX = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightX"];
		lBinBlkLRX = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightXFromSetup"];

		lBinBlkLRY = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightY"];
		lBinBlkLRY = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["LowerRightYFromSetup"];

		ulDiePerRow = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePerRow"];
		ulDiePerCol = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePerCol"];

		dDiePitchX = (DOUBLE)(LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePitchX"];
		dDiePitchY = (DOUBLE)(LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["DiePitchY"];
		szWalkPath = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["WalkPath"];
		ulSkipUnit = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["SkipUnit"];
		ulMaxUnit = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["MaxUnit"];

		ulInputCount = (*pBinBlkTemplate)["BinBlock"][szTemplateName]["InputCount"];

		bEnableFirstRowColSkipPattern = (BOOL)(LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Enbale FRSP"];
		ulFirstRowColSkipUnit			= (*pBinBlkTemplate)["BinBlock"][szTemplateName]["FRSP Skip Unit"];
		
		bCentralizedBondArea			= (BOOL)(LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Enbale Centralized"];

		bTeachWithPhysicalBlk			= (BOOL)(LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Set With Physical Block"];

		bUseBlockCornerAsFirstDiePos	= (BOOL)(LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Use Block Corner As First Die Pos"];
		
		bEnableWafflePad				= (BOOL)(LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Enable Waffle Pad"];
		lWafflePadSizeX					= (LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Size X"];
		lWafflePadSizeY					= (LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Size Y"];
		lWafflePadDistX					= (LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Dist X"];
		lWafflePadDistY					= (LONG)(*pBinBlkTemplate)["BinBlock"][szTemplateName]["Waffle Pad Dist Y"];
	}
		
	CMSFileUtility::Instance()->CloseBinBlkTemplateConfig();

	return TRUE;
}

BOOL CBinBlkMain::RemoveBinBlkTemplate(CString szTemplateName)
{
	CStdioFile cfBinBlkTemplateFile;
	CStringArray szaTemplateList;
	LONG lTotalNoOfTemplate = 0;
	BOOL bFoundTemplate = FALSE;
	CString szTemplateNameFromFile, szTemplateIndex;
	CMSFileUtility::Instance()->LoadBinBlkTemplateConfig();
	CStringMapFile *pBinBlkTemplate = CMSFileUtility::Instance()->GetBinBlkTemplateConfigFile();

	if (pBinBlkTemplate == NULL)
	{
		return FALSE;
	}

	if (szTemplateName.IsEmpty())
	{
		return FALSE;
	}

	lTotalNoOfTemplate = (*pBinBlkTemplate)["TemplateInfo"]["Total No Of Template"];
	
	for (INT i = 1; i <= (INT) lTotalNoOfTemplate; i++)
	{
		szTemplateIndex.Format("%d", i);
		szTemplateNameFromFile = (*pBinBlkTemplate)["TemplateInfo"]["TemplateName"][szTemplateIndex];

		if (szTemplateNameFromFile == szTemplateName)
		{
			bFoundTemplate = TRUE;
			
		}
		else
		{
			szaTemplateList.Add(szTemplateNameFromFile);
		}
		
	}
	
	if (bFoundTemplate == FALSE)
	{
		CMSFileUtility::Instance()->CloseBinBlkTemplateConfig();
		
		/*
		if (cfBinBlkTemplateFile.Open("BinBlkTemplateSelection.txt", CFile::modeCreate|
			CFile::modeReadWrite))
		{
						
			for (INT i=0; i< (INT) szaTemplateList.GetSize(); i++)
			{
				if (i != szaTemplateList.GetSize()-1)
					cfBinBlkTemplateFile.WriteString(szaTemplateList.GetAt(i) + ",");
				else
					cfBinBlkTemplateFile.WriteString(szaTemplateList.GetAt(i));
			}
							
			cfBinBlkTemplateFile.Close();
		}
		*/
		return FALSE;
	}

	
	(*pBinBlkTemplate)["BinBlock"].Remove(szTemplateName);

	(*pBinBlkTemplate).Remove("TemplateInfo");
	(*pBinBlkTemplate)["TemplateInfo"]["Total No Of Template"] = (LONG) szaTemplateList.GetSize();

	for (INT i = 0; i < (INT) szaTemplateList.GetSize(); i++)
	{
		szTemplateIndex.Format("%d", i + 1);
		(*pBinBlkTemplate)["TemplateInfo"]["TemplateName"][szTemplateIndex] = szaTemplateList.GetAt(i);
	}

	CMSFileUtility::Instance()->SaveBinBlkTemplateConfig();
	CMSFileUtility::Instance()->CloseBinBlkTemplateConfig();
	
	return TRUE;
}




/***********************************/
/*      BinTable Map Display       */
/***********************************/
ULONG CBinBlkMain::ConvertUpperLeftX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
									 ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedULX;
	LONG lUpperLeftX;

	lUpperLeftX = m_oBinBlk[ulBlkId].GetUpperLeftX();

	ulConvertedULX = (ULONG)(((lUpperLeftX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) + 15 
					 + ulBTXOffset;	//15 is the offset added for better looking
	ulConvertedULX -= 40; //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedULX = (ULONG)(((lUpperLeftX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) - 100
//					 + ulBTXOffset;	//BT Home pos not at center, -100 to adjust the drawing
#endif

	return ulConvertedULX;
} //end ConvertUpperLeftX


ULONG CBinBlkMain::ConvertUpperLeftY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
									 ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedULY;
	LONG lUpperLeftY;

	lUpperLeftY = m_oBinBlk[ulBlkId].GetUpperLeftY();

	ulConvertedULY = (ULONG)(((lUpperLeftY + abs(lBTYNegLimit)) / lDisplayResol) + 0.5) + 15 
					 + ulBTYOffset;	//15 is the offset added for better looking
	ulConvertedULY -= 170; //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedULY = (ULONG)(((lUpperLeftY + abs(lBTYNegLimit)) / lDisplayResol) + 0.5) + 100 
//					 + ulBTYOffset;	//BT Home pos not at center, +100 to adjust the drawing
#endif

	return ulConvertedULY;
} //end ConvertUpperLeftY


ULONG CBinBlkMain::ConvertLowerRightX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
									  ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedLRX;
	LONG lLowerRightX;

	lLowerRightX = m_oBinBlk[ulBlkId].GetLowerRightX();

	ulConvertedLRX = (ULONG)(((lLowerRightX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) + 15 
					 + ulBTXOffset;	//15 is the offset added for better looking
	ulConvertedLRX -= 40; //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedLRX = (ULONG)(((lLowerRightX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) - 100 
//					 + ulBTXOffset;	//BT Home pos not at center, -100 to adjust the drawing
#endif

	return ulConvertedLRX;
} //end ConvertLowerRightX


ULONG CBinBlkMain::ConvertLowerRightY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
									  ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedLRY;
	LONG lLowerRightY;

	lLowerRightY = m_oBinBlk[ulBlkId].GetLowerRightY();

	ulConvertedLRY = (ULONG)(((lLowerRightY + abs(lBTYNegLimit)) / lDisplayResol) + 0.5) + 15 
					 + ulBTYOffset;	//15 is the offset added for better looking
	ulConvertedLRY -= 170; //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedLRY = (ULONG)(((lLowerRightY + abs(lBTYNegLimit)) / lDisplayResol) + 0.5) + 100
//					 + ulBTYOffset;	//BT Home pos not at center, +100 to adjust the drawing
#endif

	return ulConvertedLRY;
} //end ConvertLowerRightY


/********************************/
/*    All Bin Blocks Setup      */
/********************************/

BOOL CBinBlkMain::ValidateAllSetupUL(LONG lPhyBlk1ULX, LONG lPhyBlk1ULY, LONG lPhyBlk1LRX, 
									 LONG lPhyBlk1LRY, LONG lBinBlkULX, LONG lBinBlkULY)
{
	BOOL bIsValidUL;

	//Check if upper left of "bin block to setup" is on physical block 1
	if (((lPhyBlk1ULX <= lBinBlkULX) && (lBinBlkULX <= lPhyBlk1LRX))
			&&
			((lPhyBlk1ULY <= lBinBlkULY) && (lBinBlkULY <= lPhyBlk1LRY)))
	{
		bIsValidUL = TRUE;
	}
	else 
	{
		bIsValidUL = FALSE;
	}

	return bIsValidUL;
} //end ValidateAllSetupUL


LONG CBinBlkMain::ValidateAllSetupLR(LONG lPhyBlk1ULX, LONG lPhyBlk1ULY, LONG lPhyBlk1LRX,
									 LONG lPhyBlk1LRY, LONG lBinBlkULX, LONG lBinBlkULY,
									 LONG lBinBlkLRX, LONG lBinBlkLRY)
{
	LONG lFuncRtnValue;

	//Check if lower right is on physical block 1
	if (((lPhyBlk1ULX <= lBinBlkLRX) && (lBinBlkLRX <= lPhyBlk1LRX))
			&&
			((lPhyBlk1ULY <= lBinBlkLRY) && (lBinBlkLRY <= lPhyBlk1LRY)))
	{
		//Check if lower right is really at lower right corner
		if ((lBinBlkLRX > lBinBlkULX) && (lBinBlkLRY > lBinBlkULY))
		{
			lFuncRtnValue = 3;    //error code = 3
		} 
		else
		{
			lFuncRtnValue = 2;    //error code = 2
		} 
	}
	else
	{
		lFuncRtnValue = 1;    //error code = 1
	} 

	return lFuncRtnValue;
} //end ValidateAllSetupLR


BOOL CBinBlkMain::SetAllULsAndLRs(CPhyBlkMain *cpPhyBlkMain, ULONG ulNoOfBinBlk, 
								  LONG lBinBlk1ULX, LONG lBinBlk1ULY,
								  LONG lBinBlk1LRX, LONG lBinBlk1LRY,
								  LONG lBinBlkPitchX, LONG lBinBlkPitchY)
{
	LONG lBinBlkWidth, lBinBlkHeight;
	LONG lPhyBlk1ULX, lPhyBlk1ULY;
	LONG lXOffset, lYOffset;
	ULONG i, j, k, l;

	lBinBlkWidth = lBinBlk1LRX - lBinBlk1ULX;
	lBinBlkHeight = lBinBlk1LRY - lBinBlk1ULY;
	
	lPhyBlk1ULX = cpPhyBlkMain->GrabBlkUpperLeftX(1);
	lPhyBlk1ULY = cpPhyBlkMain->GrabBlkUpperLeftY(1);
	lXOffset = lBinBlk1ULX - lPhyBlk1ULX;
	lYOffset = lBinBlk1ULY - lPhyBlk1ULY;

	//for (i = 1; i <= ulNoOfBinBlk; i++)    
	//{
	i = 1;
	
	for (j = 1; j <= cpPhyBlkMain->GetNoOfBlk(); j++)
	{   
		if (cpPhyBlkMain->GrabIsSetup(j))          
		{
			for (k = 1; k <= cpPhyBlkMain->GrabNoOfBinBlkPerRow(j); k++)
			{
				for (l = 1; l <= cpPhyBlkMain->GrabNoOfBinBlkPerCol(j); l++)
				{
					if (i <= ulNoOfBinBlk)
					{
						if ((k == 1) && (l == 1)) //1st row, 1st col
						{
							m_oBinBlk[i].SetUpperLeftX(cpPhyBlkMain->GrabBlkUpperLeftX(j) + 
													   lXOffset);
							m_oBinBlk[i].SetUpperLeftXFromSetup
							(m_oBinBlk[i].GetUpperLeftX());

							m_oBinBlk[i].SetUpperLeftY(cpPhyBlkMain->GrabBlkUpperLeftY(j) +
													   lYOffset);
							m_oBinBlk[i].SetUpperLeftYFromSetup
							(m_oBinBlk[i].GetUpperLeftY());
						}
						else if (l == 1) //1st col, but not 1st row
						{
							m_oBinBlk[i].SetUpperLeftX
							(m_oBinBlk[i - cpPhyBlkMain->GrabNoOfBinBlkPerCol(j)].GetLowerRightX()
							 + lBinBlkPitchX);
							m_oBinBlk[i].SetUpperLeftXFromSetup
							(m_oBinBlk[i].GetUpperLeftX());

							m_oBinBlk[i].SetUpperLeftY
							(m_oBinBlk[i - cpPhyBlkMain->GrabNoOfBinBlkPerCol(j)].GetUpperLeftY());
							m_oBinBlk[i].SetUpperLeftYFromSetup
							(m_oBinBlk[i].GetUpperLeftY());
						} 
						else //from 2nd row on
						{
							m_oBinBlk[i].SetUpperLeftX(m_oBinBlk[i - 1].GetUpperLeftX());
							m_oBinBlk[i].SetUpperLeftXFromSetup(m_oBinBlk[i].GetUpperLeftX());

							m_oBinBlk[i].SetUpperLeftY(m_oBinBlk[i - 1].GetLowerRightY() + lBinBlkPitchY);
							m_oBinBlk[i].SetUpperLeftYFromSetup(m_oBinBlk[i].GetUpperLeftY());
						}
						m_oBinBlk[i].SetLowerRightX(m_oBinBlk[i].GetUpperLeftX() + lBinBlkWidth);
						m_oBinBlk[i].SetLowerRightXFromSetup(m_oBinBlk[i].GetLowerRightX());

						m_oBinBlk[i].SetLowerRightY(m_oBinBlk[i].GetUpperLeftY() + lBinBlkHeight);
						m_oBinBlk[i].SetLowerRightYFromSetup(m_oBinBlk[i].GetLowerRightY());

						m_oBinBlk[i].SetWidth();
						m_oBinBlk[i].SetWidthFromSetup();

						m_oBinBlk[i].SetHeight();
						m_oBinBlk[i].SetHeightFromSetup();

						m_oBinBlk[i].SetPhyBlkId(j);
							
						m_oBinBlk[i].SetIsSetup(TRUE);
						m_oBinBlk[i].SetStatus(BT_BLK_NORMAL);
						m_oBinBlk[i].SetEmptyUnit(0);

						i++;
					}
					else
					{
						l = cpPhyBlkMain->GrabNoOfBinBlkPerCol(j) + 2;
						k = cpPhyBlkMain->GrabNoOfBinBlkPerRow(j) + 2;
						j = cpPhyBlkMain->GetNoOfBlk() + 2;

					}
						
				}
			}
		}
	}
	//}

	return TRUE;
} //end SetAllULsAndLRs


BOOL CBinBlkMain::SetupAllBlks(CPhyBlkMain *cpPhyBlkMain, ULONG ulNoOfBinBlk, 
							   LONG lBinBlk1ULX, LONG lBinBlk1ULY,
							   LONG lBinBlk1LRX, LONG lBinBlk1LRY,
							   LONG lBinBlkPitchX, LONG lBinBlkPitchY, 
							   DOUBLE dDiePitchX, DOUBLE dDiePitchY,
							   ULONG ulWalkPath, BOOL bAutoAssignGrade,
							   BOOL bAssignSameGrade, UCHAR ucStartingGrade, 
							   ULONG ulSkipUnit, ULONG ulMaxUnit, 
							   BOOL bEnableFirstRowColSkipPattern, ULONG ulFirstRowColSkipUnit, 
							   BOOL bCentralizedBondArea, BOOL bTeachWithPhysicalBlk, BOOL bUpdateGradeData,
							   BOOL bEnableWafflePad, LONG lWafflePadDistX, 
							   LONG lWafflePadDistY, LONG lWafflePadSizeX, LONG lWafflePadSizeY,
							   BOOL bEnableCircularBinArea, LONG lRadius, 
							   BOOL bUseBlockCornerAsFirstDiePos, BOOL bNoReturnTravel,
							   BOOL bUsePt5DiePitchX, BOOL bUsePt5DiePitchY,
							   DOUBLE dBondAreaOffsetX, DOUBLE dBondAreaOffsetY)
{
	ULONG i, ulDiePerRow, ulDiePerCol, ulDiePerBlk;
	LONG lBondXOffset, lBondYOffset;
	UCHAR ucGradeOffset;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	ClrAllBlksSettings();

	SetNoOfBlk(ulNoOfBinBlk);
	SetBlkPitchX(lBinBlkPitchX);
	SetBlkPitchY(lBinBlkPitchY);
	
	SetAllULsAndLRs(cpPhyBlkMain, ulNoOfBinBlk, lBinBlk1ULX, lBinBlk1ULY,
					lBinBlk1LRX, lBinBlk1LRY, lBinBlkPitchX, lBinBlkPitchY);

	lBondXOffset = (LONG) dDiePitchX;
	lBondYOffset = (LONG) dDiePitchY;

	for (i = 1; i <= ulNoOfBinBlk; i++)
	{
		m_oBinBlk[i].SetDDiePitchX(dDiePitchX);					//v4.59A19
		m_oBinBlk[i].SetDDiePitchY(dDiePitchY);					//v4.59A19
		m_oBinBlk[i].SetPoint5UmInDiePitchX(bUsePt5DiePitchX);				//v4.42T8	//Citizen
		m_oBinBlk[i].SetPoint5UmInDiePitchY(bUsePt5DiePitchY);				//v4.42T8	//Citizen	
		m_oBinBlk[i].SetBondAreaOffset(dBondAreaOffsetX, dBondAreaOffsetY);	//v4.42T9	//Citizen
		m_oBinBlk[i].SetWalkPath(ulWalkPath);
		m_oBinBlk[i].SetSkipUnit(ulSkipUnit);
		m_oBinBlk[i].SetMaxUnit(ulMaxUnit);

		//Update Use Block Corner As First Die Pos
		m_oBinBlk[i].SetIsUseBlockCornerAsFirstDiePos(bUseBlockCornerAsFirstDiePos);
		m_oBinBlk[i].SetNoReturnTravel(bNoReturnTravel);	//v4.42T1	//nichia

		ulDiePerRow = m_oBinBlk[i].CalculateDiePerRow((DOUBLE)lBondXOffset);
		if (ulDiePerRow == 0)
		{
			SetMsgCode(IDS_BT_INVALID_DIE_PITCH);
			return FALSE;
		}
		m_oBinBlk[i].SetDiePerRow(ulDiePerRow);

		ulDiePerCol = m_oBinBlk[i].CalculateDiePerCol((DOUBLE)lBondYOffset);
		if (ulDiePerCol == 0)
		{
			SetMsgCode(IDS_BT_INVALID_DIE_PITCH);
			return FALSE;
		}
		m_oBinBlk[i].SetDiePerCol(ulDiePerCol);

		m_oBinBlk[i].SetFirstRowColSkipPattern(bEnableFirstRowColSkipPattern);
		m_oBinBlk[i].SetFirstRowColSkipUnit(ulFirstRowColSkipUnit);

		if (m_oBinBlk[i].IsFirstRowLastDieSkipDie() == TRUE)
		{
			SetMsgCode(IDS_BT_INVALID_FIRST_ROW_COL_SKIP_DIE_PATTERN);
			return FALSE;
		}

		if (bEnableCircularBinArea)
		{
			m_oBinBlk[i].SetUseCircularArea(TRUE, lRadius);		//v3.71T10	//pllm
		}
			
		//Nichia//v4.43T7
		m_oBinBlk[i].m_bEnableBinMap = FALSE;	//Disable binmap if ALL BLOCKS are updated

		ulDiePerBlk = m_oBinBlk[i].CalculateDiePerBlk();
		m_oBinBlk[i].SetDiePerBlk(ulDiePerBlk);

		m_oBinBlk[i].SetNVCurrBondIndex(0);
		m_oBinBlk[i].SetNVIsFull(FALSE);
		m_oBinBlk[i].CalculateDieInfoDbCapacity(ulDiePerBlk, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime);

		m_oBinBlk[i].SetNVNoOfBondedDie(0);
		m_oBinBlk[i].SetNVLastFileSaveIndex(0);

		m_oBinBlk[i].SetNVIsAligned(FALSE);
		m_oBinBlk[i].SetNVXOffset(0);
		m_oBinBlk[i].SetNVYOffset(0);
		m_oBinBlk[i].SetNVRotateAngleX(0.0);
		
		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			//bTeachWithPhysicalBlk = TRUE;
		}
		m_oBinBlk[i].SetTeachWithPhysicalBlk(bTeachWithPhysicalBlk);

		if (pApp->GetCustomerName() == "ChangeLight(XM)")
		{
			//Default setting by Matthew 20190415
			bCentralizedBondArea = TRUE;
		}
		//Update the Centralized Bond Area
		m_oBinBlk[i].SetIsCentralizedBondArea(bCentralizedBondArea);
		if (bCentralizedBondArea)
		{
			m_oBinBlk[i].CalculateCentralizedBondArea();
		}
		else
		{
			m_oBinBlk[i].SetCentralizedOffsetX(0);
			m_oBinBlk[i].SetCentralizedOffsetY(0);
		}

		m_oBinBlk[i].SetEnableWafflePad(bEnableWafflePad);
		if (bEnableWafflePad)
		{
			m_oBinBlk[i].SetWafflePadDistX(lWafflePadDistX);
			m_oBinBlk[i].SetWafflePadDistY(lWafflePadDistY);
			m_oBinBlk[i].SetWafflePadSizeX(lWafflePadSizeX);
			m_oBinBlk[i].SetWafflePadSizeY(lWafflePadSizeY);
		}
		else
		{
			m_oBinBlk[i].SetWafflePadDistX(0);
			m_oBinBlk[i].SetWafflePadDistY(0);
			m_oBinBlk[i].SetWafflePadSizeX(0);
			m_oBinBlk[i].SetWafflePadSizeY(0);
		}

		//Remove Temp file
		RemoveTempFileAndResetIndex(i, '0', CLEAR_BIN_BY_PHY_BLK);

		//------ Case of assign grades in accending order to the bin block ------// 
		
		if (bUpdateGradeData == TRUE)
		{
			if (bAutoAssignGrade == TRUE)  
			{
				ucGradeOffset = ucStartingGrade - (UCHAR)1;

				//Klocwork	//v4.02T5
				i = min(i, BT_MAX_BINBLK_SIZE - 1);
				INT nIndex = i + ULONG(ucGradeOffset);
				nIndex = min(nIndex, BT_MAX_BINBLK_SIZE - 1);

				UCHAR ucGrade = UCHAR(i) + ucGradeOffset;
/*
				//=====================================================
				//set the grade for special 149 slot 2019.1.1
				//=====================================================
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetFeatureStatus(MS896A_FUNC_BIN_LOADER_ASSIGN_SPECIAL_GRADE) && 
					(ucOriginSpecialGrade > 0) && (ucOriginSpecialGrade <= BT_MAX_BINBLK_NO) &&
					(ucAssignSpecialGrade > 0) && (ucAssignSpecialGrade <= BT_MAX_BINBLK_NO))
				{
					if (i == ucAssignSpecialGrade)  //149
					{
						ucGrade = ucOriginSpecialGrade; //154
					}
				}
*/
				m_oBinBlk[i].SetGrade(ucGrade);
				m_oBinBlk[i].SetOriginalGrade(ucGrade);

				m_oBinGradeData[nIndex].SetNVBlkInUse(i);
				m_oBinGradeData[nIndex].SetGradeCapacity(m_oBinBlk[i].GetDiePerBlk());
				m_oBinGradeData[nIndex].SetInputCount(m_oBinBlk[i].GetDiePerBlk());
				//m_oBinGradeData[i + ULONG(ucGradeOffset)].SetMaxFrameDieCount(m_oBinGradeData[i + ULONG(ucGradeOffset)].GetInputCount());
				m_oBinGradeData[nIndex].SetNVNoOfSortedDie(0);
				m_oBinGradeData[nIndex].SetAliasGrade(0);
				m_oBinGradeData[nIndex].SetIsAssigned(TRUE);
				m_oBinGradeData[nIndex].SetNVIsFull(FALSE);
			}
		}

		//------ Case of assign same grade for all the bin block ------// 
		if (bAssignSameGrade == TRUE)
		{
			
			m_oBinBlk[i].SetGrade(ucStartingGrade);
			m_oBinBlk[i].SetOriginalGrade(ucStartingGrade);

			if (bUpdateGradeData == TRUE)
			{

				m_oBinGradeData[ucStartingGrade].SetNVBlkInUse(1);
				
				m_oBinGradeData[ucStartingGrade].SetGradeCapacity(m_oBinGradeData[ucStartingGrade].GetGradeCapacity()
						+ m_oBinBlk[i].GetDiePerBlk());

				m_oBinGradeData[ucStartingGrade].SetInputCount(m_oBinGradeData[ucStartingGrade].GetInputCount() + 
						m_oBinBlk[i].GetDiePerBlk());

				//m_oBinGradeData[ucStartingGrade].SetMaxFrameDieCount(m_oBinGradeData[ucStartingGrade].GetInputCount());

				m_oBinGradeData[ucStartingGrade].SetNVNoOfSortedDie(0);
				m_oBinGradeData[ucStartingGrade].SetAliasGrade(0);
				m_oBinGradeData[ucStartingGrade].SetIsAssigned(TRUE);
				m_oBinGradeData[ucStartingGrade].SetNVIsFull(FALSE);
			}
			// Temp to disable the merge two blk for the same grade
			//LONG lSourcePhyBlkULY = cpPhyBlkMain->GrabBlkUpperLeftY(m_oBinBlk[i].GetPhyBlkId());
			//LONG lSourcePhyBlkLRY = cpPhyBlkMain->GrabBlkLowerRightY(m_oBinBlk[i].GetPhyBlkId());

			//IfSameGradeMerge(i, lSourcePhyBlkULY, lSourcePhyBlkLRY);

		}
	}


	SaveAllBinBlkSetupData(ulNoOfBinBlk, lBinBlkPitchX, lBinBlkPitchY, bUpdateGradeData);

	return TRUE;
} //end SetupAllBlks


/***********************************/
/*     Single Bin Block Setup      */
/***********************************/

LONG CBinBlkMain::ValidateSingleSetupUL(CPhyBlkMain *cpPhyBlkMain, ULONG ulBinBlkToSetup,
										LONG lBinBlkULX, LONG lBinBlkULY)
{
	ULONG i, j;
	LONG lRtnCode = 0;
	ULONG ulPhyBlkId;
	if (CMS896AApp::m_bEnableSubBin == TRUE)
	{
		return TRUE;
	}

	//Look for source physical block
	for (i = 1; i <= cpPhyBlkMain->GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_PHYBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (((cpPhyBlkMain->GrabBlkUpperLeftX(i) <= lBinBlkULX) &&
				(lBinBlkULX <= cpPhyBlkMain->GrabBlkLowerRightX(i)))
				&&
				((cpPhyBlkMain->GrabBlkUpperLeftY(i) <= lBinBlkULY) &&
				 (lBinBlkULY <= cpPhyBlkMain->GrabBlkLowerRightY(i))))
			//inputted upper left is on physical block i
		{
			lRtnCode = i;
			ulPhyBlkId = i;

			break;
		}
	}

	if (lRtnCode > 0) //the physical block that the inputted upper left belongs to is found
		//-> now check if it's within any existing bin block
	{
		for (j = 1; j <= GetNoOfBlk(); j++)
		{
			if (m_oBinBlk[j].GetPhyBlkId() == ulPhyBlkId)
			{
				if (((m_oBinBlk[j].GetUpperLeftX() <= lBinBlkULX) &&
						(lBinBlkULX <= m_oBinBlk[j].GetLowerRightX()))
						&&
						((m_oBinBlk[j].GetUpperLeftY() <= lBinBlkULY) &&
						 (lBinBlkULY <= m_oBinBlk[j].GetLowerRightY())))
					//inputted upper left overlaps with existing bin block => ERROR
				{
					if (j != ulBinBlkToSetup)
					{
						lRtnCode = -2; //ERROR: inputted upper left overlaps with existing bin block
						break;
					}
				}
			}
		}
	}
	else
	{
		lRtnCode = -1; //ERROR: no physical block is found that the inputted upper left is belongs to
	}
	
	return lRtnCode;
} //end ValidateSingleSetupUL


LONG CBinBlkMain::ValidateSingleSetupUL_BL(CPhyBlkMain *cpPhyBlkMain, ULONG ulBinBlkToSetup, 
		LONG lBinBlkULX, LONG lBinBlkULY, ULONG ulSourcePhyBlk)
{
	ULONG i;
	if (CMS896AApp::m_bEnableSubBin == TRUE)
	{
		return TRUE;
	}
	if (!(((cpPhyBlkMain->GrabBlkUpperLeftX(ulSourcePhyBlk) <= lBinBlkULX) &&
			(lBinBlkULX <= cpPhyBlkMain->GrabBlkLowerRightX(ulSourcePhyBlk))) 
			&&
			((cpPhyBlkMain->GrabBlkUpperLeftY(ulSourcePhyBlk) <= lBinBlkULY) &&
			 (lBinBlkULY <= cpPhyBlkMain->GrabBlkLowerRightY(ulSourcePhyBlk)))))
		//inputted upper left is not on the indicated source physical block
		// => ERROR
	{
		return -1;
	}

	for (i = 1; i <= GetNoOfBlk(); i++)
	{
		if ((i != ulBinBlkToSetup) && (m_oBinBlk[i].GetPhyBlkId() == ulSourcePhyBlk))
		{
			if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkULX) &&
					(lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()))
					&&
					((m_oBinBlk[i].GetUpperLeftY() <= lBinBlkULY) &&
					 (lBinBlkULY <= m_oBinBlk[i].GetLowerRightY())))
				//inputted upper left overlaps with existing bin block => ERROR
			{
				return -2;
			}
		}
	}

	return 1;
} //end ValidateSingleSetupUL_BL


LONG CBinBlkMain::ValidateSingleSetupLR(CPhyBlkMain *cpPhyBlkMain, ULONG ulBinBlkToSetup,
										ULONG ulPhyBlkId, LONG lBinBlkULX, LONG lBinBlkULY, 
										LONG lBinBlkLRX, LONG lBinBlkLRY)
{
	LONG lRtnCode = 0;
	ULONG i;
	if (CMS896AApp::m_bEnableSubBin == TRUE)
	{
		return TRUE;
	}

	if (((cpPhyBlkMain->GrabBlkUpperLeftX(ulPhyBlkId) <= lBinBlkLRX) &&
			(lBinBlkLRX <= cpPhyBlkMain->GrabBlkLowerRightX(ulPhyBlkId)))
			&&
			((cpPhyBlkMain->GrabBlkUpperLeftY(ulPhyBlkId) <= lBinBlkLRY) &&
			 (lBinBlkLRY <= cpPhyBlkMain->GrabBlkLowerRightY(ulPhyBlkId))))
		//inputted lower right is on the same physical block as upper left
	{
		for (i = 1; i <= GetNoOfBlk(); i++)
		{
			if (m_oBinBlk[i].GetPhyBlkId() == ulPhyBlkId)
				//check only those physical block id is the same as that of 
				//the inputted LR
			{
				if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX) &&
						(lBinBlkLRX <= m_oBinBlk[i].GetLowerRightX()))
						&&
						(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY) &&
						(lBinBlkLRY <= m_oBinBlk[i].GetLowerRightY()))
					//inputted lower right is within existing bin block
				{
					if (i != ulBinBlkToSetup)
					{
						//lRtnCode = -2; 
						lRtnCode = -1;
						//SetMsgCode("Error: Inputted lower right is within existing bin block!\nPlease re-teach.");
						SetMsgCode(IDS_BT_LRCHECK1);
						break;
					}
				}

				if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX) &&
						(lBinBlkLRX <= m_oBinBlk[i].GetLowerRightX()))
						&&
						(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkULY) &&
						(lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()))
					//inputted upper right is within existing bin block
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Inputted upper right is within existing bin block!\nPlease re-teach.");
						SetMsgCode(IDS_BT_URCHECK1);
						break;
					}
				}

				if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkULX) &&
						(lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()))
						&&
						(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY) &&
						(lBinBlkLRY <= m_oBinBlk[i].GetLowerRightY()))
					//inputted lower left is within existing bin block
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Inputted lower left is within existing bin block!\nPlease re-teach.");
						SetMsgCode(IDS_BT_LLCHECK1);
						break;
					}
				}

				if (((lBinBlkULX <= m_oBinBlk[i].GetUpperLeftX()) &&
						(m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX))
						&&
						(lBinBlkULY <= m_oBinBlk[i].GetUpperLeftY()) &&
						(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY))
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Left boundary of existing bin block is within blk to teach!\nPlease re-teach.");
						SetMsgCode(IDS_BT_LEFTBOUNDARYCHK);
						break;
					}
				}

				if (((lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()) &&
						(m_oBinBlk[i].GetLowerRightX() <= lBinBlkLRX))
						&&
						(lBinBlkULY <= m_oBinBlk[i].GetUpperLeftY()) &&
						(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY))
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Right boundary of existing bin block is within blk to teach!\nPlease re-teach.");
						SetMsgCode(IDS_BT_RIGHTBOUNDARYCHK);
						break;
					}
				}
				
				if (((lBinBlkULX <= m_oBinBlk[i].GetUpperLeftX()) &&
						(m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX))
						&&
						(lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()) &&
						(m_oBinBlk[i].GetLowerRightY() <= lBinBlkLRY))
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Upper boundary of existing bin block is within blk to teach!\nPlease re-teach.");
						SetMsgCode(IDS_BT_UPPERBOUNDARYCHK);
						break;
					}
				}

				if (((lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()) && 
						(m_oBinBlk[i].GetLowerRightX() <= lBinBlkLRX))
						&&
						(lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()) &&
						(m_oBinBlk[i].GetLowerRightY() <= lBinBlkLRY))
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Lower boundary of existing bin block is within blk to teach!\nPlease re-teach.");
						SetMsgCode(IDS_BT_LOWERBOUNDARYCHK);
						break;
					}
				}

				if (((lBinBlkULX <= m_oBinBlk[i].GetUpperLeftX()) &&
						(m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX))
						&&
						((lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()) &&
						 (m_oBinBlk[i].GetLowerRightX() <= lBinBlkLRX))
						&&
						((m_oBinBlk[i].GetUpperLeftY() <= lBinBlkULY) &&
						 (lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()))
						&&
						(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY) &&
						(lBinBlkLRY <= m_oBinBlk[i].GetLowerRightY()))
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Block to teach overlaps within existing block!\nPlease re-teach."); //form a cross
						SetMsgCode(IDS_BT_OVERLAPPINGCHK1);
						break;
					}
				}

				if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkULX) &&
						(lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()))
						&&
						((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX) &&
						 (lBinBlkLRX <= m_oBinBlk[i].GetLowerRightX()))
						&&
						((lBinBlkULY <= m_oBinBlk[i].GetUpperLeftY()) &&
						 (m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY))
						&&
						((lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()) &&
						 (m_oBinBlk[i].GetLowerRightY() <= lBinBlkLRY)))
				{
					if (i != ulBinBlkToSetup)
					{
						lRtnCode = -1;
						//SetMsgCode("Error: Block to teach overlaps within existing block!\nPlease re-teach."); //form a cross
						SetMsgCode(IDS_BT_OVERLAPPINGCHK1);
						break;
					}
				}
			}
		}
		if (lRtnCode == 0)
		{
			if ((lBinBlkULX < lBinBlkLRX) && (lBinBlkULY < lBinBlkLRY))
			{
				lRtnCode = ulPhyBlkId;
			}
			else
			{
				lRtnCode = -1;
				//SetMsgCode("Error: Lower right is not at lower-right corner!\nPlease re-teach.");
				SetMsgCode(IDS_BT_LRCHECK2);
			}
		}
	}
	else
	{
		lRtnCode = -1; //lower right is not on the same physical block as upper left
		//SetMsgCode("Error: Lower right is not on the same physical block as that of upper left!\nPlease re-teach.");
		SetMsgCode(IDS_BT_LRCHECK4);
	}

	return lRtnCode;
} //end ValidateSingleSetupLR


LONG CBinBlkMain::ValidateSingleSetupLR_BL(CPhyBlkMain *cpPhyBlkMain, ULONG ulBinBlkToSetup, 
		LONG lBinBlkULX, LONG lBinBlkULY, LONG lBinBlkLRX, 
		LONG lBinBlkLRY, ULONG ulSourcePhyBlk)
{
	ULONG i;
	if (CMS896AApp::m_bEnableSubBin == TRUE)
	{
		return TRUE;
	}
	if (!(((cpPhyBlkMain->GrabBlkUpperLeftX(ulSourcePhyBlk) <= lBinBlkLRX) &&
			(lBinBlkLRX <= cpPhyBlkMain->GrabBlkLowerRightX(ulSourcePhyBlk))) 
			&&
			((cpPhyBlkMain->GrabBlkUpperLeftY(ulSourcePhyBlk) <= lBinBlkLRY) &&
			 (lBinBlkLRY <= cpPhyBlkMain->GrabBlkLowerRightY(ulSourcePhyBlk)))))
		//inputted lower right is not on the indicated source physical block
		// => ERROR
	{
		return -1;
	}

	for (i = 1; i <= GetNoOfBlk(); i++)
	{
		if ((i != ulBinBlkToSetup) && (m_oBinBlk[i].GetPhyBlkId() == ulSourcePhyBlk))
		{
			if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX) &&
					(lBinBlkLRX <= m_oBinBlk[i].GetLowerRightX()))
					&&
					((m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY) &&
					 (lBinBlkLRY <= m_oBinBlk[i].GetLowerRightY())))
				//inputted lower right overlaps with existing bin block => ERROR
			{
				return -2;
			}

			if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX) &&
					(lBinBlkLRX <= m_oBinBlk[i].GetLowerRightX()))
					&&
					(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkULY) &&
					(lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()))
				//inputted upper right overlaps with existing bin block => ERROR
			{
				return -3;
			}

			if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkULX) &&
					(lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()))
					&&
					(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY) &&
					(lBinBlkLRY <= m_oBinBlk[i].GetLowerRightY()))
				//inputted lower left is within existing bin block => ERROR
			{
				return -4;
			}

			if (((lBinBlkULX <= m_oBinBlk[i].GetUpperLeftX()) &&
					(m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX))
					&&
					(lBinBlkULY <= m_oBinBlk[i].GetUpperLeftY()) &&
					(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY))
				//Left boundary of existing bin block is within blk to teach => ERROR
			{
				return -5;
			}

			if (((lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()) &&
					(m_oBinBlk[i].GetLowerRightX() <= lBinBlkLRX))
					&&
					(lBinBlkULY <= m_oBinBlk[i].GetUpperLeftY()) &&
					(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY))
				//Right boundary of existing bin block is within blk to teach => ERROR
			{
				return -6;
			}

			if (((lBinBlkULX <= m_oBinBlk[i].GetUpperLeftX()) &&
					(m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX))
					&&
					(lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()) &&
					(m_oBinBlk[i].GetLowerRightY() <= lBinBlkLRY))
				//Upper boundary of existing bin block is within blk to teach => ERROR
			{
				return -7;
			}

			if (((lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()) && 
					(m_oBinBlk[i].GetLowerRightX() <= lBinBlkLRX))
					&&
					(lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()) &&
					(m_oBinBlk[i].GetLowerRightY() <= lBinBlkLRY))
				//Lower boundary of existing bin block is within blk to teach => ERROR
			{
				return -8;
			}

			if (((lBinBlkULX <= m_oBinBlk[i].GetUpperLeftX()) &&
					(m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX))
					&&
					((lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()) &&
					 (m_oBinBlk[i].GetLowerRightX() <= lBinBlkLRX))
					&&
					((m_oBinBlk[i].GetUpperLeftY() <= lBinBlkULY) &&
					 (lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()))
					&&
					(m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY) &&
					(lBinBlkLRY <= m_oBinBlk[i].GetLowerRightY()))
				//Block to teach overlaps within existing block => ERROR
			{
				return -9;
			}

			if (((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkULX) &&
					(lBinBlkULX <= m_oBinBlk[i].GetLowerRightX()))
					&&
					((m_oBinBlk[i].GetUpperLeftX() <= lBinBlkLRX) &&
					 (lBinBlkLRX <= m_oBinBlk[i].GetLowerRightX()))
					&&
					((lBinBlkULY <= m_oBinBlk[i].GetUpperLeftY()) &&
					 (m_oBinBlk[i].GetUpperLeftY() <= lBinBlkLRY))
					&&
					((lBinBlkULY <= m_oBinBlk[i].GetLowerRightY()) &&
					 (m_oBinBlk[i].GetLowerRightY() <= lBinBlkLRY)))
				//Block to teach overlaps within existing block => ERROR
			{
				return -10;
			}

			if (!((lBinBlkULX < lBinBlkLRX) && (lBinBlkULY < lBinBlkLRY)))
				//Lower right is not at lower-right corner => ERROR
			{
				return -11;
			}
		}
	} 
	return 1;
} //end ValidateSingleSetupLR_BL


LONG CBinBlkMain::IfSameGradeMerge(ULONG ulBinBlkId, LONG lPhyBlkULY, LONG lPhyBlkLRY)
{
	ULONG ulPhyBlkId;
	UCHAR ucGrade;
	LONG lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY;
	LONG lNewBinBlkULX, lNewBinBlkULY, lNewBinBlkLRX, lNewBinBlkLRY;
	ULONG ulUpperCutOffBlk = ulBinBlkId;
	ULONG ulLowerCutOffBlk = ulBinBlkId;
	ULONG ulDiePerRow, ulDiePerCol, ulDiePerBlk;
	ULONG i;

	ulPhyBlkId = m_oBinBlk[ulBinBlkId].GetPhyBlkId();
	ucGrade = m_oBinBlk[ulBinBlkId].GetGrade();

	ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

	lBinBlkULX = m_oBinBlk[ulBinBlkId].GetUpperLeftX();
	lBinBlkULY = m_oBinBlk[ulBinBlkId].GetUpperLeftY();
	lBinBlkLRX = m_oBinBlk[ulBinBlkId].GetLowerRightX();
	lBinBlkLRY = m_oBinBlk[ulBinBlkId].GetLowerRightY();


	//Assumption: Bin Blk Ids are assigned from top to bottom, left to right
	for (i = ulBinBlkId - 1; i >= 1; i--)
	{
		if (m_oBinBlk[i].GetIsSetup())
		{
			//to search the block(s) which has/have the same phy blk id and grade (in upper direction)		
			if ((m_oBinBlk[i].GetPhyBlkId() == ulPhyBlkId) && (m_oBinBlk[i].GetGrade() == ucGrade)
					&& 
					(lPhyBlkULY < m_oBinBlk[i].GetUpperLeftY()) && (m_oBinBlk[i].GetUpperLeftY() < lBinBlkULY))
				//check the one which is qualified and only check those which are
				//on the same column
			{
				if (m_oBinBlk[i].GetNVNoOfBondedDie() == 0)
				{
					ulUpperCutOffBlk = i;
				}
				else
				{
					return i;
				}
			}
			else
			{
				break;
			}
		}
	}

	for (i = ulBinBlkId + 1; i <= GetNoOfBlk(); i++)
	{
		if (m_oBinBlk[i].GetIsSetup())
		{
			//to search the block(s) which has/have the same phy blk id and grade (in lower direction)
			if ((m_oBinBlk[i].GetPhyBlkId() == ulPhyBlkId) && (m_oBinBlk[i].GetGrade() == ucGrade)
					&&
					(lBinBlkULY < m_oBinBlk[i].GetLowerRightY()) && (m_oBinBlk[i].GetLowerRightY() < lPhyBlkLRY))
				//check the one which is qualified and only check those which are 
				//on the same column
			{
				if (m_oBinBlk[i].GetNVNoOfBondedDie() == 0)
				{
					ulLowerCutOffBlk = i;
				}
				else
				{
					return i;
				}
			}
			else
			{
				break;
			}
		}
	}


	//consecutive block on the same phy blk with the same grade is found
	if (ulUpperCutOffBlk != ulLowerCutOffBlk) 
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		if (pBTfile == NULL)
		{
			return 0;
		}
		lNewBinBlkULX = m_oBinBlk[ulUpperCutOffBlk].GetUpperLeftX();
		lNewBinBlkULY = m_oBinBlk[ulUpperCutOffBlk].GetUpperLeftY();
		lNewBinBlkLRX = m_oBinBlk[ulUpperCutOffBlk].GetLowerRightX();
		lNewBinBlkLRY = m_oBinBlk[ulUpperCutOffBlk].GetLowerRightY();

		for (i = ulUpperCutOffBlk + 1; i <= ulLowerCutOffBlk; i++)
		{
			if (m_oBinBlk[i].GetIsSetup())
			{
				if (m_oBinBlk[i].GetUpperLeftX() < lNewBinBlkULX)
				{
					lNewBinBlkULX = m_oBinBlk[i].GetUpperLeftX();
				}
				if (m_oBinBlk[i].GetUpperLeftY() < lNewBinBlkULY)
				{
					lNewBinBlkULY = m_oBinBlk[i].GetUpperLeftY();
				}
				if (m_oBinBlk[i].GetLowerRightX() > lNewBinBlkLRX)
				{
					lNewBinBlkLRX = m_oBinBlk[i].GetLowerRightX();
				}
				if (m_oBinBlk[i].GetLowerRightY() > lNewBinBlkLRY)
				{
					lNewBinBlkLRY = m_oBinBlk[i].GetLowerRightY();
				}
			}
		}

		//reset attributes for the "top" block (in both data structures and file)
		m_oBinBlk[ulUpperCutOffBlk].SetIsDisableFromSameGradeMerge(FALSE);

		m_oBinBlk[ulUpperCutOffBlk].SetUpperLeftX(lNewBinBlkULX);
		(*pBTfile)["BinBlock"][ulUpperCutOffBlk]["UpperLeftX"] = 
			m_oBinBlk[ulUpperCutOffBlk].GetUpperLeftX();

		m_oBinBlk[ulUpperCutOffBlk].SetUpperLeftY(lNewBinBlkULY);
		(*pBTfile)["BinBlock"][ulUpperCutOffBlk]["UpperLeftY"] = 
			m_oBinBlk[ulUpperCutOffBlk].GetUpperLeftY();

		m_oBinBlk[ulUpperCutOffBlk].SetLowerRightX(lNewBinBlkLRX);
		(*pBTfile)["BinBlock"][ulUpperCutOffBlk]["LowerRightX"] = 
			m_oBinBlk[ulUpperCutOffBlk].GetLowerRightX();

		m_oBinBlk[ulUpperCutOffBlk].SetLowerRightY(lNewBinBlkLRY);
		(*pBTfile)["BinBlock"][ulUpperCutOffBlk]["LowerRightY"] = 
			m_oBinBlk[ulUpperCutOffBlk].GetLowerRightY();

		m_oBinBlk[ulUpperCutOffBlk].SetWidth();
		m_oBinBlk[ulUpperCutOffBlk].SetHeight();

		ulDiePerRow = m_oBinBlk[ulUpperCutOffBlk].CalculateDiePerRow
					  (m_oBinBlk[ulUpperCutOffBlk].GetDDiePitchX());
		m_oBinBlk[ulUpperCutOffBlk].SetDiePerRow(ulDiePerRow);
		(*pBTfile)["BinBlock"][ulUpperCutOffBlk]["DiePerRow"] = 
			m_oBinBlk[ulUpperCutOffBlk].GetDiePerRow();

		ulDiePerCol = m_oBinBlk[ulUpperCutOffBlk].CalculateDiePerCol
					  (m_oBinBlk[ulUpperCutOffBlk].GetDDiePitchY());	//v4.59A19
		m_oBinBlk[ulUpperCutOffBlk].SetDiePerCol(ulDiePerCol);

		//delete old values first
		for (i = ulUpperCutOffBlk; i <= ulLowerCutOffBlk; i++)
		{
			if (!m_oBinBlk[i].GetIsDisableFromSameGradeMerge())
			{
				m_oBinGradeData[ucGrade].SetGradeCapacity(m_oBinGradeData[ucGrade].GetGradeCapacity() - 
						m_oBinBlk[i].GetDiePerBlk());
				m_oBinGradeData[ucGrade].SetInputCount
				(m_oBinGradeData[ucGrade].GetGradeCapacity());
			}
		}

		ulDiePerBlk = m_oBinBlk[ulUpperCutOffBlk].CalculateDiePerBlk();
		m_oBinBlk[ulUpperCutOffBlk].SetDiePerBlk(ulDiePerBlk);
		(*pBTfile)["BinBlock"][ulUpperCutOffBlk]["DiePerBlk"] = 
			m_oBinBlk[ulUpperCutOffBlk].GetDiePerBlk();	
		
		// V2.96
		// update the Die Info Database Capacity
		m_oBinBlk[ulUpperCutOffBlk].CalculateDieInfoDbCapacity(m_oBinBlk[ulUpperCutOffBlk].GetDiePerBlk(), 
				CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime);

		//update after new "die per blk" for top block is calculated
		m_oBinGradeData[ucGrade].SetGradeCapacity(m_oBinGradeData[ucGrade].GetGradeCapacity() +
				m_oBinBlk[ulUpperCutOffBlk].GetDiePerBlk());
		(*pBTfile)["BinGradeData"][ucGrade]["GradeCapacity"] = 
			m_oBinGradeData[ucGrade].GetGradeCapacity();
		m_oBinGradeData[ucGrade].SetInputCount(m_oBinGradeData[ucGrade].GetGradeCapacity());
	
		(*pBTfile)["BinGradeData"][ucGrade]["InputCount"] = 
			m_oBinGradeData[ucGrade].GetInputCount();

		//disable the block(s) below
		for (i = ulUpperCutOffBlk + 1; i <= ulLowerCutOffBlk; i++)  
		{
			if (m_oBinBlk[i].GetIsSetup())
			{
				m_oBinBlk[i].SetIsDisableFromSameGradeMerge(TRUE);
				(*pBTfile)["BinBlock"][i]["IsDisableFromSameGradeMerge"] = 
					m_oBinBlk[i].GetIsDisableFromSameGradeMerge();
			}
		}  
		CMSFileUtility::Instance()->SaveBTConfig();
	}  

	return 0;
} //end IfSameGradeMerge

BOOL CBinBlkMain::CheckAllBlockSize(ULONG ulBinBlkId)
{
	LONG lDiePitchX = 0, lDiePitchY = 0;

	lDiePitchX = (LONG) m_oBinBlk[ulBinBlkId].GetDDiePitchX();		//v4.59A19
	lDiePitchY = (LONG) m_oBinBlk[ulBinBlkId].GetDDiePitchY();		//v4.59A19

	if (m_oBinBlk[ulBinBlkId].CheckBlockSize(lDiePitchX, lDiePitchY) == FALSE)
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL CBinBlkMain::SetupSingleBlk(ULONG ulNoOfBlk, ULONG ulBlkToSetup, LONG lUpperLeftX,
								 LONG lUpperLeftY, LONG lLowerRightX, LONG lLowerRightY,
								 UCHAR ucGrade, ULONG ulWalkPath, 
								 DOUBLE dDiePitchX, DOUBLE dDiePitchY, 
								 ULONG ulDiePerRow, ULONG ulPhyBlkId,
								 LONG lPhyBlkULY, LONG lPhyBlkLRY, ULONG ulSkipUnit,
								 ULONG ulMaxUnit, BOOL bResetBinSerial,
								 BOOL bEnableFirstRowColSkipPattern, ULONG ulFirstRowColSkipUnit,
								 BOOL bCentralizedBondArea, BOOL bTeachWithPhysicalBlk, BOOL bUpdateGradeData,
								 BOOL bUseBlockCornerAsFirstDiePos, BOOL	bEnableWafflePad, LONG lWafflePadDistX, 
								 LONG lWafflePadDistY, LONG lWafflePadSizeX, LONG lWafflePadSizeY,
								 BOOL bNoReturnTravel, BOOL bUsePt5PitchX, BOOL bUsePt5PitchY,
								 DOUBLE dBondAreaOffsetX, DOUBLE dBondAreaOffsetY)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulDiePerCol, ulDiePerBlk, i;
	BOOL bIsDisableFromSameGradeMerge = FALSE;
	LONG lBinToClearCount;
	UCHAR ucBlkOrgGrade = 0;
	CString szTemp;
	BOOL bRtnValue = TRUE;


	SetNoOfBlk(ulNoOfBlk);

	//if (bResetBinSerial == TRUE)
	//ResetSerialNo(ulBlkToSetup);
	
	m_oBinBlk[ulBlkToSetup].SetUpperLeftX(lUpperLeftX);
	m_oBinBlk[ulBlkToSetup].SetUpperLeftXFromSetup(lUpperLeftX);

	m_oBinBlk[ulBlkToSetup].SetUpperLeftY(lUpperLeftY);
	m_oBinBlk[ulBlkToSetup].SetUpperLeftYFromSetup(lUpperLeftY);

	m_oBinBlk[ulBlkToSetup].SetLowerRightX(lLowerRightX);
	m_oBinBlk[ulBlkToSetup].SetLowerRightXFromSetup(lLowerRightX);

	m_oBinBlk[ulBlkToSetup].SetLowerRightY(lLowerRightY);
	m_oBinBlk[ulBlkToSetup].SetLowerRightYFromSetup(lLowerRightY);

	m_oBinBlk[ulBlkToSetup].SetWidth();
	m_oBinBlk[ulBlkToSetup].SetWidthFromSetup();

	m_oBinBlk[ulBlkToSetup].SetHeight();
	m_oBinBlk[ulBlkToSetup].SetHeightFromSetup();

	
	//if (m_oBinBlk[ulBlkToSetup].GetDiePerBlk() < m_oBinBlk[ulBlkToSetup].CalculateDiePerBlk())

	//{
	//	return FALSE;
	//}

	//Update BinGradeData structure before new grade and die per blk are set
	if (bUpdateGradeData == TRUE)
	{
		ucBlkOrgGrade = m_oBinBlk[ulBlkToSetup].GetGrade();

		//Klocwork	//v4.02T5
		ucBlkOrgGrade = min(ucBlkOrgGrade, BT_MAX_BINBLK_SIZE - 1);

		if ((m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity() > m_oBinBlk[ulBlkToSetup].GetDiePerBlk()))
		{
			m_oBinGradeData[ucBlkOrgGrade].SetGradeCapacity(m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity() - m_oBinBlk[ulBlkToSetup].GetDiePerBlk());
		}
		else
		{
			m_oBinGradeData[ucBlkOrgGrade].SetGradeCapacity(0);
		}


		if (m_oBinGradeData[ucBlkOrgGrade].GetInputCount() > 
				m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity())
		{
			m_oBinGradeData[ucBlkOrgGrade].SetInputCount
			(m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity());
		}

		//m_oBinGradeData[ucBlkOrgGrade].SetMaxFrameDieCount(m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity());

		if (m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity() == 0) 
			//original grade is no longer assigned with bin block
		{
			m_oBinGradeData[ucBlkOrgGrade].SetAliasGrade(0);
			m_oBinGradeData[ucBlkOrgGrade].SetNVNoOfSortedDie(0);
			m_oBinGradeData[ucBlkOrgGrade].SetNVIsFull(FALSE);
			m_oBinGradeData[ucBlkOrgGrade].SetNVBlkInUse(0);
			m_oBinGradeData[ucBlkOrgGrade].SetInputCount(0);
			//m_oBinGradeData[ucBlkOrgGrade].SetMaxFrameDieCount(0);
			m_oBinGradeData[ucBlkOrgGrade].SetIsAssigned(FALSE);
		}
	}


	m_oBinBlk[ulBlkToSetup].SetGrade(ucGrade);
	m_oBinBlk[ulBlkToSetup].SetOriginalGrade(ucGrade);

	if (bUpdateGradeData == TRUE)
	{
		if (m_oBinGradeData[ucBlkOrgGrade].GetNVBlkInUse() == ulBlkToSetup)
		{
			for (i = 1; i <= GetNoOfBlk(); i++)
			{
				if (m_oBinBlk[i].GetGrade() == ucBlkOrgGrade)
				{
					m_oBinGradeData[ucBlkOrgGrade].SetNVBlkInUse(i);
					break;
				}
			}
		}
	}
	
	m_oBinBlk[ulBlkToSetup].SetDDiePitchX(dDiePitchX);					//v4.59A19
	m_oBinBlk[ulBlkToSetup].SetDDiePitchY(dDiePitchY);					//v4.59A19
	//m_oBinBlk[ulBlkToSetup].SetPoint5UmInDiePitchX(bUsePt5PitchX);	//v4.42T8
	//m_oBinBlk[ulBlkToSetup].SetPoint5UmInDiePitchY(bUsePt5PitchY);	//v4.42T8
	//m_oBinBlk[ulBlkToSetup].SetBondAreaOffset(dBondAreaOffsetX, dBondAreaOffsetY);	//v4.42T9
	m_oBinBlk[ulBlkToSetup].SetWalkPath(ulWalkPath);
	m_oBinBlk[ulBlkToSetup].SetSkipUnit(ulSkipUnit);
	m_oBinBlk[ulBlkToSetup].SetMaxUnit(ulMaxUnit);

	//Update Use Block Corner As First Die Pos
	m_oBinBlk[ulBlkToSetup].SetIsUseBlockCornerAsFirstDiePos(bUseBlockCornerAsFirstDiePos);
	m_oBinBlk[ulBlkToSetup].SetNoReturnTravel(bNoReturnTravel);		//v4.42T1	//Nichia

	//v4.59A19
	LONG lBondXOffset = (LONG) dDiePitchX;
	LONG lBondYOffset = (LONG) dDiePitchY;

	ulDiePerRow = m_oBinBlk[ulBlkToSetup].CalculateDiePerRow((DOUBLE)lBondXOffset);
	m_oBinBlk[ulBlkToSetup].SetDiePerRow(ulDiePerRow);

	ulDiePerCol = m_oBinBlk[ulBlkToSetup].CalculateDiePerCol((DOUBLE)lBondYOffset);
	m_oBinBlk[ulBlkToSetup].SetDiePerCol(ulDiePerCol);

	ulDiePerBlk = m_oBinBlk[ulBlkToSetup].CalculateDiePerBlk();
	m_oBinBlk[ulBlkToSetup].SetDiePerBlk(ulDiePerBlk);

	/*
	if (m_oBinBlk[ulBlkToSetup].CalulateAcutalDiePerBlk(lDiePitchX, lDiePitchY) < m_oBinBlk[ulBlkToSetup].GetDiePerBlk())
	{
		AfxMessageBox("User Row Col too large", MB_SYSTEMMODAL);
		return FALSE;
	}
	*/

	m_oBinBlk[ulBlkToSetup].SetFirstRowColSkipPattern(bEnableFirstRowColSkipPattern);
	m_oBinBlk[ulBlkToSetup].SetFirstRowColSkipUnit(ulFirstRowColSkipUnit);

	if (m_oBinBlk[ulBlkToSetup].IsFirstRowLastDieSkipDie() == TRUE)
	{
		SetMsgCode(IDS_BT_INVALID_FIRST_ROW_COL_SKIP_DIE_PATTERN);
		return FALSE;
	}

	m_oBinBlk[ulBlkToSetup].SetNVCurrBondIndex(0);
	m_oBinBlk[ulBlkToSetup].SetNVIsFull(FALSE);
	m_oBinBlk[ulBlkToSetup].CalculateDieInfoDbCapacity(ulDiePerBlk, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime);

	m_oBinBlk[ulBlkToSetup].SetNVNoOfBondedDie(0);
	m_oBinBlk[ulBlkToSetup].SetNVLastFileSaveIndex(0);

	m_oBinBlk[ulBlkToSetup].SetNVIsAligned(FALSE);
	m_oBinBlk[ulBlkToSetup].SetNVXOffset(0);
	m_oBinBlk[ulBlkToSetup].SetNVYOffset(0);
	m_oBinBlk[ulBlkToSetup].SetNVRotateAngleX(0.0);

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		//bTeachWithPhysicalBlk = TRUE;
	}
	m_oBinBlk[ulBlkToSetup].SetTeachWithPhysicalBlk(bTeachWithPhysicalBlk);
		
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		bCentralizedBondArea = TRUE;
	}
	//Update the Centralized Bond Area
	m_oBinBlk[ulBlkToSetup].SetIsCentralizedBondArea(bCentralizedBondArea);
	if (bCentralizedBondArea)
	{
		m_oBinBlk[ulBlkToSetup].CalculateCentralizedBondArea();
	}
	else
	{
		m_oBinBlk[ulBlkToSetup].SetCentralizedOffsetX(0);
		m_oBinBlk[ulBlkToSetup].SetCentralizedOffsetY(0);
	}

	m_oBinBlk[ulBlkToSetup].SetEnableWafflePad(bEnableWafflePad);
	if (bEnableWafflePad)
	{
		
		m_oBinBlk[ulBlkToSetup].SetWafflePadDistX(lWafflePadDistX);
		m_oBinBlk[ulBlkToSetup].SetWafflePadDistY(lWafflePadDistY);
		m_oBinBlk[ulBlkToSetup].SetWafflePadSizeX(lWafflePadSizeX);
		m_oBinBlk[ulBlkToSetup].SetWafflePadSizeY(lWafflePadSizeY);
	}
	else
	{
		m_oBinBlk[ulBlkToSetup].SetWafflePadDistX(0);
		m_oBinBlk[ulBlkToSetup].SetWafflePadDistY(0);
		m_oBinBlk[ulBlkToSetup].SetWafflePadSizeX(0);
		m_oBinBlk[ulBlkToSetup].SetWafflePadSizeY(0);
	}

	//Remove Temp file
	RemoveTempFileAndResetIndex(ulBlkToSetup, '0', CLEAR_BIN_BY_PHY_BLK);

	if (bUpdateGradeData == TRUE)
	{
		//Input Count
		if (m_oBinGradeData[ucGrade].GetInputCount() == m_oBinGradeData[ucGrade].GetGradeCapacity())
		{
			m_oBinGradeData[ucGrade].SetInputCount(m_oBinGradeData[ucGrade].GetInputCount() +
												   m_oBinBlk[ulBlkToSetup].GetDiePerBlk());
		}

		//m_oBinGradeData[ucGrade].SetMaxFrameDieCount(m_oBinGradeData[ucGrade].GetInputCount());

		//GradeCapacity
		m_oBinGradeData[ucGrade].SetGradeCapacity(m_oBinGradeData[ucGrade].GetGradeCapacity() +
				m_oBinBlk[ulBlkToSetup].GetDiePerBlk()); //add newly calculated
		//"die per blk"

		//Update BinGradeData structure
		if (!m_oBinGradeData[ucGrade].GetIsAssigned())  //this is a newly-assigned grade
		{
			//IsAssigned
			m_oBinGradeData[ucGrade].SetIsAssigned(TRUE);

			//InputCount
			m_oBinGradeData[ucGrade].SetInputCount(m_oBinGradeData[ucGrade].GetGradeCapacity());

			//m_oBinGradeData[ucGrade].SetMaxFrameDieCount(m_oBinGradeData[ucGrade].GetInputCount());

			//IsFull
			m_oBinGradeData[ucGrade].SetNVIsFull(FALSE);

			//AliasGrade
			m_oBinGradeData[ucGrade].SetAliasGrade(0);

			//NoOfSortedDie
			m_oBinGradeData[ucGrade].SetNVNoOfSortedDie(0);

			//BlkInUse
			m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBlkToSetup);
		}

		ULONG ulBlkInUse = m_oBinGradeData[ucGrade].GetNVBlkInUse();

		if ((ulBlkInUse > ulBlkToSetup) && (m_oBinBlk[ulBlkInUse].GetNVNoOfBondedDie() == 0))
		{
			m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBlkToSetup);
		}

		if (m_oBinGradeData[ucGrade].GetNVBlkInUse() == 0)
		{
			m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBlkToSetup);
		}
	}

	m_oBinBlk[ulBlkToSetup].SetPhyBlkId(ulPhyBlkId);
	
	m_oBinBlk[ulBlkToSetup].SetIsSetup(TRUE);

	m_oBinBlk[ulBlkToSetup].SetStatus(BT_BLK_NORMAL);

	m_oBinBlk[ulBlkToSetup].SetEmptyUnit(0);

	lBinToClearCount = IfSameGradeMerge(ulBlkToSetup, lPhyBlkULY, lPhyBlkLRY);
	szTemp.Format("%d", lBinToClearCount);

	if (lBinToClearCount > 0)
	{
		bRtnValue = FALSE;
		AfxMessageBox("Please clear bin " + szTemp + "counter for same grade merge!"); 
	}
	
	SaveSingleBinBlkSetupData(GetNoOfBlk(), ulBlkToSetup, ucBlkOrgGrade, bUpdateGradeData);

	return bRtnValue;
} //end SetupSingleBlk


BOOL CBinBlkMain::SetupCircularBlkByArrCode(ULONG ulBinBlkID, LONG lULX, LONG lULY, LONG lLRX, LONG lLRY,
											DOUBLE dAreaXInUm, DOUBLE dAreaYInUm,
											UCHAR ucStartPos, ULONG ulMaxLoad, LONG lBinPitchX, LONG lBinPitchY,
											BOOL bReturnTravel)
{
	CString szTemp;
	BOOL bUpdateGradeData = TRUE;

	UCHAR ucBlkOrgGrade = m_oBinBlk[ulBinBlkID].GetGrade();

	if ( (dAreaYInUm<=0) )
	{
		return FALSE;
	}
	if (ucStartPos > BT_BRV_PATH)	//0-7
	{
		return FALSE;
	}

	//szTemp.Format("SetupCircularBlkByArrCode: UL - %ld %ld; LR - %ld %ld", lULX, lULY, lLRX, lLRY);

	m_oBinBlk[ulBinBlkID].SetUpperLeftX(lULX);
	m_oBinBlk[ulBinBlkID].SetUpperLeftY(lULY);
	m_oBinBlk[ulBinBlkID].SetLowerRightX(lLRX);
	m_oBinBlk[ulBinBlkID].SetLowerRightY(lLRY);

	m_oBinBlk[ulBinBlkID].SetUpperLeftXFromSetup(lULX);
	m_oBinBlk[ulBinBlkID].SetUpperLeftYFromSetup(lULY);
	m_oBinBlk[ulBinBlkID].SetLowerRightXFromSetup(lLRX);
	m_oBinBlk[ulBinBlkID].SetLowerRightYFromSetup(lLRY);
	m_oBinBlk[ulBinBlkID].SetWidth();
	m_oBinBlk[ulBinBlkID].SetWidthFromSetup();
	m_oBinBlk[ulBinBlkID].SetHeight();
	m_oBinBlk[ulBinBlkID].SetHeightFromSetup();

	m_oBinBlk[ulBinBlkID].SetIsUseBlockCornerAsFirstDiePos(TRUE);	//v4.43T10

	if (lBinPitchX > 0)
		m_oBinBlk[ulBinBlkID].SetDDiePitchX(lBinPitchX);
	if (lBinPitchY > 0)
		m_oBinBlk[ulBinBlkID].SetDDiePitchY(lBinPitchY);
	m_oBinBlk[ulBinBlkID].SetNoReturnTravel(!bReturnTravel);	//543212

	ULONG ulDiePerRow = m_oBinBlk[ulBinBlkID].CalculateDiePerRow((DOUBLE)lBinPitchX);
	m_oBinBlk[ulBinBlkID].SetDiePerRow(ulDiePerRow);
	ULONG ulDiePerCol = m_oBinBlk[ulBinBlkID].CalculateDiePerCol((DOUBLE)lBinPitchY);
	m_oBinBlk[ulBinBlkID].SetDiePerCol(ulDiePerCol);
	ULONG ulDiePerBlk = m_oBinBlk[ulBinBlkID].CalculateDiePerBlk();
	m_oBinBlk[ulBinBlkID].SetDiePerBlk(ulDiePerBlk);

	//m_oBinBlk[ulBinBlkID].SetUserRowCol(ulDiePerRow, ulDiePerCol);

	//#define BT_TLH_PATH				0
	//#define BT_TLV_PATH				1
	//#define BT_TRH_PATH				2
	//#define BT_TRV_PATH				3
	//#define BT_BLH_PATH				4	
	//#define BT_BLV_PATH				5	
	//#define BT_BRH_PATH				6	
	//#define BT_BRV_PATH				7	
	ULONG ulWalkPath = ucStartPos;
	if ( (ulWalkPath != BT_TLV_PATH) && (ulWalkPath != BT_BLV_PATH) )
		ulWalkPath = BT_TLV_PATH;
	m_oBinBlk[ulBinBlkID].SetWalkPath(ulWalkPath);

	m_oBinBlk[ulBinBlkID].m_bEnableBinMap = TRUE;
	m_oBinBlk[ulBinBlkID].SetBinMapCircleRadius(dAreaYInUm);
	
	//54321234
	LONG lCCol=0, lCRow=0;
	ULONG ulQuotient	= m_oBinBlk[ulBinBlkID].GetDiePerRow()	/ 2;
	ULONG ulRemainder = m_oBinBlk[ulBinBlkID].GetDiePerRow()	% 2;
	if (ulRemainder > 0)
		lCCol = (LONG) (m_oBinBlk[ulBinBlkID].GetDiePerRow() / 2 + 1);
	else
		lCCol = (LONG) (m_oBinBlk[ulBinBlkID].GetDiePerRow() / 2);
	
	ulQuotient	= m_oBinBlk[ulBinBlkID].GetDiePerCol()	/ 2;
	ulRemainder = m_oBinBlk[ulBinBlkID].GetDiePerCol()	% 2;
	if (ulRemainder > 0)
		lCRow = (LONG) (m_oBinBlk[ulBinBlkID].GetDiePerCol() / 2 + 1);
	else
		lCRow = (LONG) (m_oBinBlk[ulBinBlkID].GetDiePerCol() / 2);

	m_oBinBlk[ulBinBlkID].SetBinMapOffset(TRUE, lCRow, lCCol);
	szTemp.Format("SetupCircularBlkByArrCode: CDie (%ld, %ld)", lCRow, lCCol);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);


	//54321
	LONG lDiesOnCol = (LONG) dAreaXInUm / lBinPitchY;
	LONG lLEdge = 0;
	szTemp.Format("SetupCircularBlkByArrCode: TEdge dices on row = %ld", lDiesOnCol);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	m_oBinBlk[ulBinBlkID].FindBinMapLEdge1stCol(lDiesOnCol, lLEdge);
	m_oBinBlk[ulBinBlkID].SetBinMapEdgeSize(0, 0, lLEdge, 0);

	BOOL bNoReturnTravel = m_oBinBlk[ulBinBlkID].GetNoReturnTravel();
	if (!bNoReturnTravel)
	{
		INT nReminder = lLEdge%2;
		if (ulWalkPath == BT_TLV_PATH)
		{
			if (nReminder == 1)
			{
//AfxMessageBox("Walking path changed to TRH ...", MB_SYSTEMMODAL);
				ulWalkPath = BT_BLV_PATH;
				m_oBinBlk[ulBinBlkID].SetWalkPath(ulWalkPath);
			}
		}
		else
		{
			if (nReminder == 1)
			{
//AfxMessageBox("Walking path changed to TLH ...", MB_SYSTEMMODAL);
				ulWalkPath = BT_TLV_PATH;
				m_oBinBlk[ulBinBlkID].SetWalkPath(ulWalkPath);
			}
		}
	}

	m_oBinBlk[ulBinBlkID].SetNVCurrBondIndex(0);
	m_oBinBlk[ulBinBlkID].SetNVIsFull(FALSE);
	m_oBinBlk[ulBinBlkID].CalculateDieInfoDbCapacity(ulDiePerBlk, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime);
	m_oBinBlk[ulBinBlkID].SetNVNoOfBondedDie(0);
	m_oBinBlk[ulBinBlkID].SetNVLastFileSaveIndex(0);
	m_oBinBlk[ulBinBlkID].SetNVIsAligned(FALSE);
	m_oBinBlk[ulBinBlkID].SetNVXOffset(0);
	m_oBinBlk[ulBinBlkID].SetNVYOffset(0);
	m_oBinBlk[ulBinBlkID].SetNVRotateAngleX(0.0);
	m_oBinBlk[ulBinBlkID].SetIsSetup(TRUE);
	m_oBinBlk[ulBinBlkID].SetStatus(BT_BLK_NORMAL);
	m_oBinBlk[ulBinBlkID].SetEmptyUnit(0);

	//Update the Centralized Bond Area if necessary
	BOOL bCentralizedBondArea = m_oBinBlk[ulBinBlkID].GetIsCentralizedBondArea();
	if (bCentralizedBondArea)
	{
		m_oBinBlk[ulBinBlkID].CalculateCentralizedBondArea();
	}
	else
	{
		m_oBinBlk[ulBinBlkID].SetCentralizedOffsetX(0);
		m_oBinBlk[ulBinBlkID].SetCentralizedOffsetY(0);
	}

	//Remove Temp file
	RemoveTempFileAndResetIndex(ulBinBlkID, '0', CLEAR_BIN_BY_PHY_BLK);


	if (bUpdateGradeData == TRUE)
	{
		UCHAR ucGrade = GrabGrade(ulBinBlkID);
		ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE-1);	//v4.46T20	//Klocwork

		if (ulMaxLoad > 0)
		{
			m_oBinGradeData[ucGrade].SetInputCount(ulMaxLoad);
		}
		if (m_oBinBlk[ulBinBlkID].GetDiePerBlk() < m_oBinGradeData[ucGrade].GetInputCount())
		{
			m_oBinGradeData[ucGrade].SetInputCount(m_oBinBlk[ulBinBlkID].GetDiePerBlk());
		}

		ULONG ulBinMapCount = CalclateBinMapCapacity(ulBinBlkID);
		if (ulBinMapCount < m_oBinGradeData[ucGrade].GetInputCount())
		{
			m_oBinGradeData[ucGrade].SetInputCount(ulBinMapCount);
		}

		m_oBinGradeData[ucGrade].SetNVIsFull(FALSE);
		m_oBinGradeData[ucGrade].SetAliasGrade(0);
		m_oBinGradeData[ucGrade].SetNVNoOfSortedDie(0);
		m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBinBlkID);
	}

	m_oBinBlk[ulBinBlkID].SetIsSetup(TRUE);
	m_oBinBlk[ulBinBlkID].SetStatus(BT_BLK_NORMAL);
	m_oBinBlk[ulBinBlkID].SetEmptyUnit(0);
	SaveSingleBinBlkSetupData(GetNoOfBlk(), ulBinBlkID, ucBlkOrgGrade, bUpdateGradeData);
	
	return TRUE;
}


BOOL CBinBlkMain::SetupSingleBlkByArrCode(ULONG ulBinBlkID,  
										  LONG lPhyULX, LONG lPhyULY, LONG lPhyLRX, LONG lPhyLRY,
										  UCHAR ucType,
										  DOUBLE dAreaXInUm, DOUBLE dAreaYInUm,
										  UCHAR ucStartPos, ULONG ulMaxLoad, 
										  LONG lBinPitchX, LONG lBinPitchY,
										  BOOL bReturnTravel)
{
	CString szTemp;
	//szTemp.Format("Index = %lu, %d, %.1f, %.1f, %d, %lu, %ld, %ld, %d",  
	//	ulBinBlkID, ucType, dAreaXInUm, dAreaYInUm, ucStartPos, ulMaxLoad, 
	//	lBinPitchX, lBinPitchY, bReturnTravel);

	BOOL bUpdateGradeData = TRUE;

	UCHAR ucBlkOrgGrade = m_oBinBlk[ulBinBlkID].GetGrade();

	if ( (dAreaXInUm<=0) || (dAreaYInUm<=0) )
	{
		return FALSE;
	}
	if (ucStartPos > BT_BRV_PATH)	//0-7
	{
		return FALSE;
	}
	if (ulBinBlkID >= BT_MAX_BINBLK_SIZE)		//v4.46T20	//Klocwork
	{
		return FALSE;
	}

 	LONG lULX = m_oBinBlk[ulBinBlkID].GetUpperLeftX();
	LONG lLRX = lULX + (LONG) dAreaXInUm;
	LONG lULY = m_oBinBlk[ulBinBlkID].GetUpperLeftY();
	LONG lLRY = lULY + (LONG) dAreaYInUm;

	m_oBinBlk[ulBinBlkID].SetUpperLeftXFromSetup(lULX);
	m_oBinBlk[ulBinBlkID].SetUpperLeftYFromSetup(lULY);
	m_oBinBlk[ulBinBlkID].SetLowerRightX(lLRX);
	m_oBinBlk[ulBinBlkID].SetLowerRightXFromSetup(lLRX);
	m_oBinBlk[ulBinBlkID].SetLowerRightY(lLRY);
	m_oBinBlk[ulBinBlkID].SetLowerRightYFromSetup(lLRY);
	m_oBinBlk[ulBinBlkID].SetWidth();
	m_oBinBlk[ulBinBlkID].SetWidthFromSetup();
	m_oBinBlk[ulBinBlkID].SetHeight();
	m_oBinBlk[ulBinBlkID].SetHeightFromSetup();

	m_oBinBlk[ulBinBlkID].SetIsUseBlockCornerAsFirstDiePos(TRUE);	//v4.43T10

	if (lBinPitchX > 0)
		m_oBinBlk[ulBinBlkID].SetDDiePitchX(lBinPitchX);
	if (lBinPitchY > 0)
		m_oBinBlk[ulBinBlkID].SetDDiePitchY(lBinPitchY);
	m_oBinBlk[ulBinBlkID].SetNoReturnTravel(!bReturnTravel);	//543212

	ULONG ulDiePerRow = m_oBinBlk[ulBinBlkID].CalculateDiePerRow((DOUBLE)lBinPitchX);
	m_oBinBlk[ulBinBlkID].SetDiePerRow(ulDiePerRow);
	ULONG ulDiePerCol = m_oBinBlk[ulBinBlkID].CalculateDiePerCol((DOUBLE)lBinPitchY);
	m_oBinBlk[ulBinBlkID].SetDiePerCol(ulDiePerCol);
	ULONG ulDiePerBlk = m_oBinBlk[ulBinBlkID].CalculateDiePerBlk();
	m_oBinBlk[ulBinBlkID].SetDiePerBlk(ulDiePerBlk);

	//m_oBinBlk[ulBinBlkID].SetUserRowCol(ulDiePerRow, ulDiePerCol);

	//#define BT_TLH_PATH				0
	//#define BT_TLV_PATH				1
	//#define BT_TRH_PATH				2
	//#define BT_TRV_PATH				3
	//#define BT_BLH_PATH				4	
	//#define BT_BLV_PATH				5	
	//#define BT_BRH_PATH				6	
	//#define BT_BRV_PATH				7	
	ULONG ulWalkPath = ucStartPos;
	m_oBinBlk[ulBinBlkID].SetWalkPath(ulWalkPath);

	m_oBinBlk[ulBinBlkID].m_bEnableBinMap = FALSE;

	m_oBinBlk[ulBinBlkID].SetNVCurrBondIndex(0);
	m_oBinBlk[ulBinBlkID].SetNVIsFull(FALSE);
	m_oBinBlk[ulBinBlkID].CalculateDieInfoDbCapacity(ulDiePerBlk, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime);
	m_oBinBlk[ulBinBlkID].SetNVNoOfBondedDie(0);
	m_oBinBlk[ulBinBlkID].SetNVLastFileSaveIndex(0);
	m_oBinBlk[ulBinBlkID].SetNVIsAligned(FALSE);
	m_oBinBlk[ulBinBlkID].SetNVXOffset(0);
	m_oBinBlk[ulBinBlkID].SetNVYOffset(0);
	m_oBinBlk[ulBinBlkID].SetNVRotateAngleX(0.0);
	m_oBinBlk[ulBinBlkID].SetIsSetup(TRUE);
	m_oBinBlk[ulBinBlkID].SetStatus(BT_BLK_NORMAL);
	m_oBinBlk[ulBinBlkID].SetEmptyUnit(0);

	//Update the Centralized Bond Area if necessary
	m_oBinBlk[ulBinBlkID].SetIsCentralizedBondArea(TRUE);
	if (m_oBinBlk[ulBinBlkID].GetIsCentralizedBondArea())
	{
CString szTemp;
szTemp.Format("SetupSingleBlkByArrCode - Blk #%ld CalculateCentralizedBondAreaFromPhyLimit ....", ulBinBlkID);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
szTemp.Format("%ld, %ld,   %ld, %ld.", lPhyULX, lPhyULY, lPhyLRX, lPhyLRY);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		m_oBinBlk[ulBinBlkID].CalculateCentralizedBondAreaFromPhyLimit(lPhyULX, lPhyULY, lPhyLRX, lPhyLRY);
	}
	else
	{
		m_oBinBlk[ulBinBlkID].SetCentralizedOffsetX(0);
		m_oBinBlk[ulBinBlkID].SetCentralizedOffsetY(0);
	}

	//Remove Temp file
	RemoveTempFileAndResetIndex(ulBinBlkID, '0', CLEAR_BIN_BY_PHY_BLK);


	if (bUpdateGradeData == TRUE)
	{
		UCHAR ucGrade = GrabGrade(ulBinBlkID);

		//CString szTemp2;
		//szTemp2.Format("SetupSingleBlkByArrCode: DiePerBlock = %ld, InputCount = %ld",
		//m_oBinBlk[ulBinBlkID].GetDiePerBlk(), m_oBinGradeData[ucGrade].GetInputCount());
		//AfxMessageBox(szTemp2, MB_SYSTEMMODAL);

		if (ucGrade < BT_MAX_BINBLK_SIZE)	//Klocwork	//v4.46
		{
			if (ulMaxLoad > 0)
			{
				m_oBinGradeData[ucGrade].SetInputCount(ulMaxLoad);
			}
			if (m_oBinBlk[ulBinBlkID].GetDiePerBlk() < m_oBinGradeData[ucGrade].GetInputCount())
			{
				m_oBinGradeData[ucGrade].SetInputCount(m_oBinBlk[ulBinBlkID].GetDiePerBlk());
			}

			m_oBinGradeData[ucGrade].SetNVIsFull(FALSE);
			m_oBinGradeData[ucGrade].SetAliasGrade(0);
			m_oBinGradeData[ucGrade].SetNVNoOfSortedDie(0);
			m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBinBlkID);
		}
/*
		//Update BinGradeData structure
		if (!m_oBinGradeData[ucGrade].GetIsAssigned())  //this is a newly-assigned grade
		{
			//IsAssigned
			m_oBinGradeData[ucGrade].SetIsAssigned(TRUE);
			//InputCount
			m_oBinGradeData[ucGrade].SetInputCount(m_oBinGradeData[ucGrade].GetGradeCapacity());
			//IsFull
			m_oBinGradeData[ucGrade].SetNVIsFull(FALSE);
			//AliasGrade
			m_oBinGradeData[ucGrade].SetAliasGrade(0);
			//NoOfSortedDie
			m_oBinGradeData[ucGrade].SetNVNoOfSortedDie(0);
			//BlkInUse
			m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBinBlkID);
		}

		ULONG ulBlkInUse = m_oBinGradeData[ucGrade].GetNVBlkInUse();
		if ((ulBlkInUse > ulBinBlkID) && (m_oBinBlk[ulBinBlkID].GetNVNoOfBondedDie() == 0))
		{
			m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBinBlkID);
		}
		if (m_oBinGradeData[ucGrade].GetNVBlkInUse() == 0)
		{
			m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBinBlkID);
		}
*/
	}

	m_oBinBlk[ulBinBlkID].SetIsSetup(TRUE);
	m_oBinBlk[ulBinBlkID].SetStatus(BT_BLK_NORMAL);
	m_oBinBlk[ulBinBlkID].SetEmptyUnit(0);
	SaveSingleBinBlkSetupData(GetNoOfBlk(), ulBinBlkID, ucBlkOrgGrade, bUpdateGradeData);
	return TRUE;
}


BOOL CBinBlkMain::EditBlkInMerge(ULONG ulBinBlkToEdit, UCHAR ucInputGrade, LONG lInputDiePitchX,
								 LONG lInputDiePitchY, ULONG ulInputWalkPath, ULONG ulSkipUnit,
								 ULONG ulMaxUnit)
{
	ULONG ulNewDiePerRow, ulNewDiePerCol, ulNewDiePerBlk;
	ULONG ulUpperStartBlk, ulUpperEndBlk, ulLowerStartBlk, ulLowerEndBlk;
	ULONG i;
	UCHAR ucOriginalGrade;
	CString szTemp;
	BOOL bRtnValue = TRUE;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	
	if (pBTfile == NULL)
	{
		return FALSE;
	}
							
	ucOriginalGrade = m_oBinBlk[ulBinBlkToEdit].GetGrade();

	//Klocwork	//v4.02T5
	ucOriginalGrade = min(ucOriginalGrade, BT_MAX_BINBLK_SIZE - 1);

	//reset attributes for "Blk To Edit" 

	//Die Pitch X
	m_oBinBlk[ulBinBlkToEdit].SetDDiePitchX(lInputDiePitchX);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["DiePitchX"] = 
		m_oBinBlk[ulBinBlkToEdit].GetDDiePitchX();		//v4.59A19

	//Die Per Row
	ulNewDiePerRow = 
		m_oBinBlk[ulBinBlkToEdit].CalculateDiePerRow((DOUBLE)lInputDiePitchX);
	m_oBinBlk[ulBinBlkToEdit].SetDiePerRow(ulNewDiePerRow);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["DiePerRow"] = 
		m_oBinBlk[ulBinBlkToEdit].GetDiePerRow();
	
	//Die Pitch Y
	m_oBinBlk[ulBinBlkToEdit].SetDDiePitchY(lInputDiePitchY);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["DiePitchY"] =
		m_oBinBlk[ulBinBlkToEdit].GetDDiePitchY();		//v4.59A19

	//Die Per Col
	ulNewDiePerCol =
		m_oBinBlk[ulBinBlkToEdit].CalculateDiePerCol((DOUBLE)lInputDiePitchY);
	m_oBinBlk[ulBinBlkToEdit].SetDiePerCol(ulNewDiePerCol);

	//Die Per Blk
	ulNewDiePerBlk = 
		m_oBinBlk[ulBinBlkToEdit].CalculateDiePerBlk();
	m_oBinBlk[ulBinBlkToEdit].SetDiePerBlk(ulNewDiePerBlk);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["DiePerBlk"] = 
		m_oBinBlk[ulBinBlkToEdit].GetDiePerBlk();

	//Walking Path
	m_oBinBlk[ulBinBlkToEdit].SetWalkPath(ulInputWalkPath);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["WalkPath"] =
		m_oBinBlk[ulBinBlkToEdit].GetWalkPath();

	//SkipUnit
	m_oBinBlk[ulBinBlkToEdit].SetSkipUnit(ulSkipUnit);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["SkipUnit"] =
		m_oBinBlk[ulBinBlkToEdit].GetSkipUnit();

	//MaxUnit
	m_oBinBlk[ulBinBlkToEdit].SetMaxUnit(ulMaxUnit);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["MaxUnit"] =
		m_oBinBlk[ulBinBlkToEdit].GetMaxUnit();

	if (m_oBinBlk[ulBinBlkToEdit].GetGrade() != ucInputGrade) //grade being changed
	{
		//separate the merged blocks
		ulUpperStartBlk = ulBinBlkToEdit - 1;
		ulUpperEndBlk = ulBinBlkToEdit - 1;
		ulLowerStartBlk = ulBinBlkToEdit + 1;
		ulLowerEndBlk = ulBinBlkToEdit + 1;

		for (i = ulBinBlkToEdit - 1; i >= 1; i--)
		{
			//find the upper edge of merge 
			if ((!m_oBinBlk[i].GetIsDisableFromSameGradeMerge()) &&
					(m_oBinBlk[i].GetGrade() == m_oBinBlk[ulBinBlkToEdit].GetGrade()))
			{
				ulUpperStartBlk = i;
				break;
			}
		}

		//if (m_oBinBlk[ulUpperStartBlk].GetNoOfBondedDie() > 0) //cannot separate if bonding in process
		if (m_oBinBlk[ulUpperStartBlk].GetNVNoOfBondedDie() > 0) //cannot separate if bonding in process		
		{
			szTemp.Format("%d", ulUpperStartBlk);
			AfxMessageBox("Please clear bin " + szTemp + " counter first!");
			bRtnValue = FALSE;
			CMSFileUtility::Instance()->SaveBTConfig();
			return bRtnValue;
		}
		else
		{
			//subtract "existing" grade capacity, then add new one(s) later
			m_oBinGradeData[ucOriginalGrade].SetGradeCapacity(
				m_oBinGradeData[ucOriginalGrade].GetGradeCapacity() - 
				m_oBinBlk[ulUpperStartBlk].GetDiePerBlk());
			(*pBTfile)["BinGradeData"][ucOriginalGrade]["GradeCapacity"] =
				m_oBinGradeData[ucOriginalGrade].GetGradeCapacity();

			//InputCount
			m_oBinGradeData[ucOriginalGrade].SetInputCount(
				m_oBinGradeData[ucOriginalGrade].GetGradeCapacity());
			(*pBTfile)["BinGradeData"][ucOriginalGrade]["InputCount"] = 
				m_oBinGradeData[ucOriginalGrade].GetInputCount();

			//m_oBinGradeData[ucOriginalGrade].SetMaxFrameDieCount(
			//m_oBinGradeData[ucOriginalGrade].GetInputCount());

			(*pBTfile)["BinGradeData"][ucOriginalGrade]["Max Frame Die Count"] =
				m_oBinGradeData[ucOriginalGrade].GetMaxFrameDieCount();
	
		}

		for (i = ulBinBlkToEdit + 1; i <= GetNoOfBlk(); i++)
		{
			//find the lower edge of merge
			if ((m_oBinBlk[i].GetIsDisableFromSameGradeMerge()) &&
					(m_oBinBlk[i].GetGrade() == m_oBinBlk[ulBinBlkToEdit].GetGrade()))
			{
				ulLowerEndBlk = i;
			}
			else if (m_oBinBlk[i].GetGrade() != m_oBinBlk[ulBinBlkToEdit].GetGrade())
			{
				ulLowerStartBlk = i - 1;
				ulLowerEndBlk = i - 1;
				break;
			}
		}

		//reset attributes for "upper block" (in both file and structure)

		//Lower Right Y
		m_oBinBlk[ulUpperStartBlk].SetLowerRightY
		(m_oBinBlk[ulUpperEndBlk].GetLowerRightYFromSetup());
		(*pBTfile)["BinBlock"][ulUpperStartBlk]["LowerRightY"] = 
			m_oBinBlk[ulUpperStartBlk].GetLowerRightY();

		//Height
		m_oBinBlk[ulUpperStartBlk].SetHeight();
		
		//Die Per Col
		ulNewDiePerCol = 
			m_oBinBlk[ulUpperStartBlk].CalculateDiePerCol(m_oBinBlk[ulUpperStartBlk].GetDDiePitchY());
		m_oBinBlk[ulUpperStartBlk].SetDiePerCol(ulNewDiePerCol);

		//Die Per Blk
		ulNewDiePerBlk =
			m_oBinBlk[ulUpperStartBlk].CalculateDiePerBlk();
		m_oBinBlk[ulUpperStartBlk].SetDiePerBlk(ulNewDiePerBlk);
		(*pBTfile)["BinBlock"][ulUpperStartBlk]["DiePerBlk"] = 
			m_oBinBlk[ulUpperStartBlk].GetDiePerBlk();

		//Grade Capacity
		m_oBinGradeData[ucOriginalGrade].SetGradeCapacity
		(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity() +
		 m_oBinBlk[ulUpperStartBlk].GetDiePerBlk());
		(*pBTfile)["BinGradeData"][ucOriginalGrade]["GradeCapacity"] =
			m_oBinGradeData[ucOriginalGrade].GetGradeCapacity();

		//InputCount
		m_oBinGradeData[ucOriginalGrade].SetInputCount
		(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity());
		(*pBTfile)["BinGradeData"][ucOriginalGrade]["InputCount"] =
			m_oBinGradeData[ucOriginalGrade].GetInputCount();

		//reset attributes for "lower block" (in both file and structure)
		if (ulBinBlkToEdit != ulLowerStartBlk)
		{
			//IsDisableFromSameGradeMerge
			m_oBinBlk[ulLowerStartBlk].SetIsDisableFromSameGradeMerge(FALSE);
			(*pBTfile)["BinBlock"][ulLowerStartBlk]["IsDisableFromSameGradeMerge"] =
				m_oBinBlk[ulLowerStartBlk].GetIsDisableFromSameGradeMerge();

			//Lower Right X
			m_oBinBlk[ulLowerStartBlk].SetLowerRightX(m_oBinBlk[ulUpperStartBlk].GetLowerRightX());
			(*pBTfile)["BinBlock"][ulLowerStartBlk]["LowerRightX"] =
				m_oBinBlk[ulLowerStartBlk].GetLowerRightX();

			//Lower Right Y
			m_oBinBlk[ulLowerStartBlk].SetLowerRightY(m_oBinBlk[ulLowerEndBlk].GetLowerRightYFromSetup());
			(*pBTfile)["BinBlock"][ulLowerStartBlk]["LowerRightY"] = 
				m_oBinBlk[ulLowerStartBlk].GetLowerRightY();

			//Height
			m_oBinBlk[ulLowerStartBlk].SetHeight();

			//Die Per Col
			ulNewDiePerCol = m_oBinBlk[ulLowerStartBlk].CalculateDiePerCol
							 (m_oBinBlk[ulLowerStartBlk].GetDDiePitchY());		//v4.59A19
			m_oBinBlk[ulLowerStartBlk].SetDiePerCol(ulNewDiePerCol);

			//Die Per Blk
			ulNewDiePerBlk = m_oBinBlk[ulLowerStartBlk].CalculateDiePerBlk();
			m_oBinBlk[ulLowerStartBlk].SetDiePerBlk(ulNewDiePerBlk);
			(*pBTfile)["BinBlock"][ulLowerStartBlk]["DiePerBlk"] = 
				m_oBinBlk[ulLowerStartBlk].GetDiePerBlk();

			//Grade Capacity
			m_oBinGradeData[ucOriginalGrade].SetGradeCapacity
			(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity() +
			 m_oBinBlk[ulLowerStartBlk].GetDiePerBlk());
			(*pBTfile)["BinGradeData"][ucOriginalGrade]["GradeCapacity"] =
				m_oBinGradeData[ucOriginalGrade].GetGradeCapacity();

			//InputCount
			m_oBinGradeData[ucOriginalGrade].SetInputCount
			(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity());
			(*pBTfile)["BinGradeData"][ucOriginalGrade]["InputCount"] =
				m_oBinGradeData[ucOriginalGrade].GetInputCount();
		}



		//reset IsDisableFromSameGradeMerge attribute for "Blk to edit"
		m_oBinBlk[ulBinBlkToEdit].SetIsDisableFromSameGradeMerge(FALSE);
		(*pBTfile)["BinBlock"][ulBinBlkToEdit]["IsDisableFromSameGradeMerge"] =
			m_oBinBlk[ulBinBlkToEdit].GetIsDisableFromSameGradeMerge();

		// new grade setup start
		//reset grade attribute for "Blk to edit"
		m_oBinBlk[ulBinBlkToEdit].SetGrade(ucInputGrade);
		(*pBTfile)["BinBlock"][ulBinBlkToEdit]["Grade"] = 
			m_oBinBlk[ulBinBlkToEdit].GetGrade();

		//Original Grade
		m_oBinBlk[ulBinBlkToEdit].SetOriginalGrade(ucInputGrade);
		(*pBTfile)["BinBlock"][ulBinBlkToEdit]["Original Grade"] = 
			m_oBinBlk[ulBinBlkToEdit].GetOriginalGrade();

		//Grade Capacity for "new" grade
		m_oBinGradeData[ucInputGrade].SetGradeCapacity
		(m_oBinGradeData[ucInputGrade].GetGradeCapacity() +
		 m_oBinBlk[ulBinBlkToEdit].GetDiePerBlk());
		(*pBTfile)["BinGradeData"][ucInputGrade]["GradeCapacity"] = 
			m_oBinGradeData[ucInputGrade].GetGradeCapacity();

		//InputCount
		m_oBinGradeData[ucInputGrade].SetInputCount
		(m_oBinGradeData[ucInputGrade].GetGradeCapacity());
		(*pBTfile)["BinGradeData"][ucInputGrade]["InputCount"] =
			m_oBinGradeData[ucInputGrade].GetInputCount();

		// Check if the Grade is Newly assigned
		// Update the required Flag Respectively
		if (!m_oBinGradeData[ucInputGrade].GetIsAssigned())  //this is a newly-assigned grade
		{
			//IsAssigned
			m_oBinGradeData[ucInputGrade].SetIsAssigned(TRUE);
			(*pBTfile)["BinGradeData"][ucInputGrade]["IsAssigned"] = m_oBinGradeData[ucInputGrade].GetIsAssigned();

			//InputCount
			//m_oBinGradeData[ucInputGrade].SetInputCount(m_oBinGradeData[ucInputGrade].GetGradeCapacity());
			//Grade Capacity for "new" grade
			//IsFull
			m_oBinGradeData[ucInputGrade].SetNVIsFull(FALSE);

			//AliasGrade
			m_oBinGradeData[ucInputGrade].SetAliasGrade(0);
			(*pBTfile)["BinGradeData"][ucInputGrade]["AliasGrade"] = m_oBinGradeData[ucInputGrade].GetAliasGrade();

			//NoOfSortedDie
			m_oBinGradeData[ucInputGrade].SetNVNoOfSortedDie(0);

			//BlkInUse
			m_oBinGradeData[ucInputGrade].SetNVBlkInUse(ulBinBlkToEdit);
		}

		if (m_oBinGradeData[ucInputGrade].GetNVBlkInUse() == 0)
		{
			m_oBinGradeData[ucInputGrade].SetNVBlkInUse(ulBinBlkToEdit);
		}
	}

	CMSFileUtility::Instance()->SaveBTConfig();

	return bRtnValue;
} //end EditBlkInMerge


BOOL CBinBlkMain::Edit1stBlkInMerge(ULONG ulBinBlkToEdit, UCHAR ucInputGrade, LONG lInputDiePitchX,
									LONG lInputDiePitchY, ULONG ulInputWalkPath)
{
	UCHAR ucOriginalGrade;
	BOOL bRtnValue = TRUE;
	ULONG ulNewDiePerCol, ulNewDiePerBlk, ulNewLowerRightYFor1stBlkInMerge;

	ucOriginalGrade = m_oBinBlk[ulBinBlkToEdit].GetGrade();	
	if (ucOriginalGrade > BT_MAX_BINBLK_SIZE - 1)		//Klocwork
	{
		return FALSE;
	}

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}
	//reset attributes for "1st Blk In Merge"

	//save LowerRightY for new 1st blk in merge b4 edit
	ulNewLowerRightYFor1stBlkInMerge = m_oBinBlk[ulBinBlkToEdit].GetLowerRightY();

	//LowerRightY
	m_oBinBlk[ulBinBlkToEdit].SetLowerRightY(m_oBinBlk[ulBinBlkToEdit].GetLowerRightYFromSetup());
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["LowerRightY"] = 
		m_oBinBlk[ulBinBlkToEdit].GetLowerRightY();

	//Height
	m_oBinBlk[ulBinBlkToEdit].SetHeight();

	//DiePerCol
	ulNewDiePerCol = m_oBinBlk[ulBinBlkToEdit].CalculateDiePerCol
					 (m_oBinBlk[ulBinBlkToEdit].GetDDiePitchY());	//v4.59A19
	m_oBinBlk[ulBinBlkToEdit].SetDiePerCol(ulNewDiePerCol);

	//GradeCapacity (for original grade - edit before DiePerBlk is updated)
	m_oBinGradeData[ucOriginalGrade].SetGradeCapacity
	(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity() - 
	 m_oBinBlk[ulBinBlkToEdit].GetDiePerBlk());
	(*pBTfile)["BinGradeData"][ucOriginalGrade]["GradeCapacity"] =
		m_oBinGradeData[ucOriginalGrade].GetGradeCapacity();

	//InputCount
	m_oBinGradeData[ucOriginalGrade].SetInputCount
	(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity());
	(*pBTfile)["BinGradeData"][ucOriginalGrade]["InputCount"] =
		m_oBinGradeData[ucOriginalGrade].GetInputCount();

	//DiePerBlk
	ulNewDiePerBlk = m_oBinBlk[ulBinBlkToEdit].CalculateDiePerBlk();
	m_oBinBlk[ulBinBlkToEdit].SetDiePerBlk(ulNewDiePerBlk);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["DiePerBlk"] = 
		m_oBinBlk[ulBinBlkToEdit].GetDiePerBlk();

	//Grade
	m_oBinBlk[ulBinBlkToEdit].SetGrade(ucInputGrade);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["Grade"] = 
		m_oBinBlk[ulBinBlkToEdit].GetGrade();

	//Original Grade
	m_oBinBlk[ulBinBlkToEdit].SetOriginalGrade(ucInputGrade);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit]["Original Grade"] = 
		m_oBinBlk[ulBinBlkToEdit].GetOriginalGrade();

	//GradeCapacity (for input grade)
	m_oBinGradeData[ucInputGrade].SetGradeCapacity(m_oBinGradeData[ucInputGrade].GetGradeCapacity() +
			m_oBinBlk[ulBinBlkToEdit].GetDiePerBlk());
	(*pBTfile)["BinGradeData"][ucInputGrade]["GradeCapacity"] = 
		m_oBinGradeData[ucInputGrade].GetGradeCapacity();

	//InputCount
	m_oBinGradeData[ucInputGrade].SetInputCount
	(m_oBinGradeData[ucInputGrade].GetGradeCapacity());
	(*pBTfile)["BinGradeData"][ucInputGrade]["InputCount"] =
		m_oBinGradeData[ucInputGrade].GetInputCount();

	//reset attributes for new "1st blk in merge"

	//IsDisableFromSameGradeMerge
	m_oBinBlk[ulBinBlkToEdit + 1].SetIsDisableFromSameGradeMerge(FALSE);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit + 1]["IsDisableFromSameGradeMerge"] =
		m_oBinBlk[ulBinBlkToEdit + 1].GetIsDisableFromSameGradeMerge();

	//UpperLeftY
	m_oBinBlk[ulBinBlkToEdit + 1].SetUpperLeftY(m_oBinBlk[ulBinBlkToEdit + 1].GetUpperLeftYFromSetup());
	(*pBTfile)["BinBlock"][ulBinBlkToEdit + 1]["UpperLeftY"] = 
		m_oBinBlk[ulBinBlkToEdit + 1].GetUpperLeftY();

	//LowerRightY
	m_oBinBlk[ulBinBlkToEdit + 1].SetLowerRightY(ulNewLowerRightYFor1stBlkInMerge);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit + 1]["LowerRightY"] =
		m_oBinBlk[ulBinBlkToEdit + 1].GetLowerRightY();

	//Height
	m_oBinBlk[ulBinBlkToEdit + 1].SetHeight();
	
	//DiePerCol
	ulNewDiePerCol = m_oBinBlk[ulBinBlkToEdit + 1].CalculateDiePerCol
					 (m_oBinBlk[ulBinBlkToEdit + 1].GetDDiePitchY());		//v4.59A19
	m_oBinBlk[ulBinBlkToEdit + 1].SetDiePerCol(ulNewDiePerCol);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit + 1]["DiePerCol"] =
		m_oBinBlk[ulBinBlkToEdit + 1].GetDiePerCol();

	//DiePerBlk
	ulNewDiePerBlk = m_oBinBlk[ulBinBlkToEdit + 1].CalculateDiePerBlk();
	m_oBinBlk[ulBinBlkToEdit + 1].SetDiePerBlk(ulNewDiePerBlk);
	(*pBTfile)["BinBlock"][ulBinBlkToEdit + 1]["DiePerBlk"] = 
		m_oBinBlk[ulBinBlkToEdit + 1].GetDiePerBlk();

	//GradeCapacity (for original grade)
	m_oBinGradeData[ucOriginalGrade].SetGradeCapacity(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity() +
			m_oBinBlk[ulBinBlkToEdit + 1].GetDiePerBlk());
	(*pBTfile)["BinGradeData"][ucOriginalGrade]["GradeCapacity"] = 
		m_oBinGradeData[ucOriginalGrade].GetGradeCapacity();
	
	// Alex
	// Current Blk In Use 

	//udpate the blk in use for the bin below merged grade
	m_oBinGradeData[ucOriginalGrade].SetNVBlkInUse(ulBinBlkToEdit + 1);
	
	//InputCount
	m_oBinGradeData[ucOriginalGrade].SetInputCount
	(m_oBinGradeData[ucOriginalGrade].GetGradeCapacity());

	(*pBTfile)["BinGradeData"][ucOriginalGrade]["InputCount"] =
		m_oBinGradeData[ucOriginalGrade].GetInputCount();

	//Update BinGradeData structure
	if (m_oBinGradeData[ucInputGrade].GetIsAssigned() == FALSE)  //this is a newly-assigned grade
	{
		//IsAssigned
		m_oBinGradeData[ucInputGrade].SetIsAssigned(TRUE);

		//InputCount
		m_oBinGradeData[ucInputGrade].SetInputCount(m_oBinGradeData[ucInputGrade].GetGradeCapacity());

		//IsFull
		m_oBinGradeData[ucInputGrade].SetNVIsFull(FALSE);

		//AliasGrade
		m_oBinGradeData[ucInputGrade].SetAliasGrade(0);

		//NoOfSortedDie
		m_oBinGradeData[ucInputGrade].SetNVNoOfSortedDie(0);

		//BlkInUse
		m_oBinGradeData[ucInputGrade].SetNVBlkInUse(ulBinBlkToEdit);
	}
	if (m_oBinGradeData[ucInputGrade].GetNVBlkInUse() == 0)
	{
		m_oBinGradeData[ucInputGrade].SetNVBlkInUse(ulBinBlkToEdit);
	}

	CMSFileUtility::Instance()->SaveBTConfig();

	return bRtnValue;
} //end Edit1stBlkInMerge


BOOL CBinBlkMain::ReassignBlkGrade(ULONG ulBlkToAssign, UCHAR ucGrade, BOOL bInitial, BOOL bUpdateGradeData)
{
	BOOL bReturn = TRUE;
	ULONG i;
	UCHAR ucBlkOrgGrade;


	//Update BinGradeData structure before new grade and die per blk are set
	ucBlkOrgGrade = m_oBinBlk[ulBlkToAssign].GetGrade();
	
	if (ucBlkOrgGrade > BT_MAX_BINBLK_SIZE - 1)	//Klocwork
	{
		return FALSE;
	}

	if (ucGrade > BT_MAX_BINBLK_SIZE - 1)			//Klocwork
	{
		return FALSE;
	}

	m_oBinGradeData[ucBlkOrgGrade].SetGradeCapacity
	(m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity() - 
	 m_oBinBlk[ulBlkToAssign].GetDiePerBlk());
	if (m_oBinGradeData[ucBlkOrgGrade].GetInputCount() > 
			m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity())
	{
		m_oBinGradeData[ucBlkOrgGrade].SetInputCount
		(m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity());
	}

	//m_oBinGradeData[ucBlkOrgGrade].SetMaxFrameDieCount(m_oBinGradeData[ucBlkOrgGrade].GetInputCount());

	if (m_oBinGradeData[ucBlkOrgGrade].GetGradeCapacity() == 0) 
		//original grade is no longer assigned with bin block
	{
		m_oBinGradeData[ucBlkOrgGrade].SetAliasGrade(0);
		m_oBinGradeData[ucBlkOrgGrade].SetNVNoOfSortedDie(0);
		m_oBinGradeData[ucBlkOrgGrade].SetNVIsFull(FALSE);
		m_oBinGradeData[ucBlkOrgGrade].SetNVBlkInUse(0);
		m_oBinGradeData[ucBlkOrgGrade].SetInputCount(0);
		//m_oBinGradeData[ucBlkOrgGrade].SetMaxFrameDieCount(m_oBinGradeData[ucBlkOrgGrade].GetInputCount());
		m_oBinGradeData[ucBlkOrgGrade].SetIsAssigned(FALSE);
	}

	m_oBinBlk[ulBlkToAssign].SetGrade(ucGrade);
	m_oBinBlk[ulBlkToAssign].SetStatus(BT_BLK_NORMAL);
	m_oBinBlk[ulBlkToAssign].SetEmptyUnit(0);

	if (m_oBinGradeData[ucBlkOrgGrade].GetNVBlkInUse() == ulBlkToAssign)
	{
		for (i = 1; i <= GetNoOfBlk(); i++)
		{
			if (m_oBinBlk[i].GetGrade() == ucBlkOrgGrade)
			{
				m_oBinGradeData[ucBlkOrgGrade].SetNVBlkInUse(i);
				break;
			}
		}
	}


	//Input Count
	if (m_oBinGradeData[ucGrade].GetInputCount() == m_oBinGradeData[ucGrade].GetGradeCapacity())
	{
		m_oBinGradeData[ucGrade].SetInputCount(m_oBinGradeData[ucGrade].GetInputCount() +
											   m_oBinBlk[ulBlkToAssign].GetDiePerBlk());
	}

	//m_oBinGradeData[ucGrade].SetMaxFrameDieCount(m_oBinGradeData[ucGrade].GetInputCount());

	//GradeCapacity
	m_oBinGradeData[ucGrade].SetGradeCapacity(m_oBinGradeData[ucGrade].GetGradeCapacity() +
			m_oBinBlk[ulBlkToAssign].GetDiePerBlk()); //add newly calculated
	//"die per blk"

	//Update BinGradeData structure
	if (!m_oBinGradeData[ucGrade].GetIsAssigned())  //this is a newly-assigned grade
	{
		//IsAssigned
		m_oBinGradeData[ucGrade].SetIsAssigned(TRUE);

		//InputCount
		m_oBinGradeData[ucGrade].SetInputCount(m_oBinGradeData[ucGrade].GetGradeCapacity());

		//m_oBinGradeData[ucGrade].SetMaxFrameDieCount(m_oBinGradeData[ucGrade].GetInputCount());

		//IsFull
		m_oBinGradeData[ucGrade].SetNVIsFull(FALSE);

		//AliasGrade
		m_oBinGradeData[ucGrade].SetAliasGrade(0);

		//NoOfSortedDie
		m_oBinGradeData[ucGrade].SetNVNoOfSortedDie(0);

		//BlkInUse
		m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBlkToAssign);
	}

	ULONG ulBlkInUse = m_oBinGradeData[ucGrade].GetNVBlkInUse();

	if ((ulBlkInUse > ulBlkToAssign) && (m_oBinBlk[ulBlkInUse].GetNVNoOfBondedDie() == 0))
	{
		m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBlkToAssign);
	}

	if (m_oBinGradeData[ucGrade].GetNVBlkInUse() == 0)
	{
		m_oBinGradeData[ucGrade].SetNVBlkInUse(ulBlkToAssign);
	}


	//Check any binblk have same grade and update grade's blk use
	if (bInitial == FALSE)
	{
		for (i = 1; i <= GetNoOfBlk(); i++)
		{
			if ((m_oBinBlk[i].GetGrade() == ucGrade) && (m_oBinBlk[i].GetNVNoOfBondedDie() > 0))
			{
				m_oBinGradeData[ucBlkOrgGrade].SetNVBlkInUse(i);
			}
		}
	}


	SaveSingleBinBlkSetupData(GetNoOfBlk(), ulBlkToAssign, ucBlkOrgGrade, bUpdateGradeData);

	return bReturn;
}


BOOL CBinBlkMain::SetUserRowCol(ULONG ulBlkId, ULONG ulDiePerUserRow, ULONG ulDiePerUserCol)
{
	INT i;

	if (ulBlkId == 0)
	{
		for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++) 
		{
			m_oBinBlk[i].SetUserRowCol(ulDiePerUserRow, ulDiePerUserCol);
		}
	}
	else
	{
		m_oBinBlk[ulBlkId].SetUserRowCol(ulDiePerUserRow, ulDiePerUserCol);
	}

	return TRUE;
}


BOOL CBinBlkMain::SwapBinBlksGrade(ULONG ulBlkId, UCHAR ucTargetedGrade)
{
	BOOL bIsNeedSwap = FALSE;
	UCHAR ucCurrentBlkGrade = 0, ucTempGrade = 0;
	ULONG ulSwapBlockId = 0;

	if (ulBlkId > GetNoOfBlk())
	{
		return FALSE;
	}
	
	ucCurrentBlkGrade = GrabGrade(ulBlkId);

	if (ucCurrentBlkGrade != ucTargetedGrade)
	{
		for (INT i = 1; i <= (INT)GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (i != ulBlkId)
			{
				ucTempGrade = m_oBinBlk[i].GetGrade();

				if (ucTempGrade == ucTargetedGrade)
				{
					bIsNeedSwap = TRUE;
					ulSwapBlockId = i;
					break;
				}
			}
		}

		if (bIsNeedSwap == TRUE)
		{
			ReassignBlkGrade(ulBlkId, ucTargetedGrade, FALSE, TRUE);
			ReassignBlkGrade(ulSwapBlockId, ucCurrentBlkGrade, FALSE, TRUE);
		}
		// Grade not exist in current block settings
		else
		{
			ReassignBlkGrade(ulBlkId, ucTargetedGrade, TRUE, TRUE);
		}
	}
		
	return TRUE;

}


/***********************************/
/*			For Bonding            */
/***********************************/
ULONG CBinBlkMain::FindBondXYPosn(UCHAR ucGrade, DOUBLE &dXPosn, DOUBLE &dYPosn, 
								  LONG &lRow, LONG &lCol, LONG& lSortDir)
{
	//LONG lX = 0, lY = 0, lBinRow = 0, lBinCol = 0;
	LONG lBinRow = 0, lBinCol = 0;
	DOUBLE dPosX=0, dPosY=0;	//v4.57A7
	//LONG lOrigX = 0; LONG lOrigY = 0;
	DOUBLE dOrigX = 0; DOUBLE dOrigY = 0;
	ULONG ulBlkToUse, ulNextIndex;
	ULONG i;
	ULONG ulCurrentIndex;


	if (m_oBinGradeData[ucGrade].GetAliasGrade() != 0)
	{
		ucGrade = m_oBinGradeData[ucGrade].GetAliasGrade();
	} 

	if (m_oBinGradeData[ucGrade].GetIsAssigned())
	{																	
		if (!m_oBinGradeData[ucGrade].GetNVIsFull())  //grade is not full		
		{  
			ulBlkToUse = m_oBinGradeData[ucGrade].GetNVBlkInUse();  
		
			ulBlkToUse = min(ulBlkToUse, BT_MAX_BINBLK_SIZE - 1);		//Klocwork	//v4.02T5

			if (m_oBinBlk[ulBlkToUse].GetNVIsFull() || 
					m_oBinBlk[ulBlkToUse].GetIsDisableFromSameGradeMerge() ||
					!m_oBinBlk[ulBlkToUse].GetIsSetup() ||
					!m_oBinBlk[ulBlkToUse].GetIsAssignedWithSlot()) 
				//current block with given grade is full or is disabled or is not setup or
				//is not assigned with slot
				//--> look for next available block
			{
				ulBlkToUse = 0; 

				for (i = 1; i <= GetNoOfBlk(); i++)
				{
					i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

					if (m_oBinBlk[i].GetGrade() == ucGrade)
					{
						if ((m_oBinBlk[i].GetIsSetup()) && 
								(!m_oBinBlk[i].GetIsDisableFromSameGradeMerge()) &&
								(!m_oBinBlk[i].GetNVIsFull()))
						{
							ulBlkToUse = i;
							
							m_oBinGradeData[ucGrade].SetNVBlkInUse(i);

							break;
						}
						
					}
				}
			}
			
			if (ulBlkToUse > 0) //there is a block to use
			{		
				ulNextIndex = m_oBinBlk[ulBlkToUse].GetNVCurrBondIndex() + 1;
				ulNextIndex = ulNextIndex + m_oBinBlk[ulBlkToUse].GetSkipUnit();

				ULONG ulOldIndex = ulNextIndex;

				BOOL b2DStatus = TwoDimensionBarcodeFindNextBondIndex(ulBlkToUse, ulNextIndex);	
				BOOL bLog2DEnc = IsNear2DBarcodeBondIndex(ulBlkToUse, ulNextIndex);		//v4.48A20
				FirstRowColSkipPatternFindNextBondIndex(ulBlkToUse, ulNextIndex);
				WafflePadSkipPosFindNextBondIndex(ulBlkToUse, ulNextIndex);

				BOOL bBinMapOK = GetNextDieIndexInBinMap(ulBlkToUse, ulNextIndex);			//v4.03		//PLLM/PLSG bin map fcn
				if (!bBinMapOK)
				{
					return 0;	//No NEXT-INDEX can be assigned/allocated	//v4.59A37	//Finisar TX
				}
				
				BOOL bBinMix = GetNextDieIndexInOSRAMBinMixMap(ulBlkToUse, ulNextIndex);
				if (bBinMix == FALSE || bBinMix == 4)
				{
					/*HmiMessage("Bin Mix Map: Bin Full, Please Clear Bin!");*/
					return Err_BinBlockNotAssign;
				}


				ULONG ulOriginalIndex = ulNextIndex;
				if (!CheckIfNeedToLeaveEmptyRow(ulBlkToUse, ulNextIndex))	//v4.30T3	//CyOptics US
				{
					ulBlkToUse = Err_BinBlockNotAssign;
					return  ulBlkToUse;
				}


				// Skip Bond Pattern Index
				if (GrabLSBondPattern() == TRUE)
				{
					ulCurrentIndex = ulNextIndex;
					if (m_oBinBlk[ulBlkToUse].GetLSBondIndex(ulCurrentIndex, ulNextIndex) == TRUE)
					{
						CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
						if (pBTfile != NULL)
						{
							(*pBTfile)["BinBlock"][ulBlkToUse]["EmptyUnit"] = m_oBinBlk[ulBlkToUse].GetEmptyUnit();
							CMSFileUtility::Instance()->SaveBTConfig();
						}
					}
				}

				//v4.48A4	//v4.50A28	//v4.51A12
				CString szLog;
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

				//Calculations next position
				m_oBinBlk[ulBlkToUse].FindBinTableXYPosnGivenIndex(ulNextIndex, dPosX, dPosY);
				lSortDir = m_oBinBlk[ulBlkToUse].GetSortDirection();	//CSP

				m_oBinBlk[ulBlkToUse].FindBondRowColGivenIndex(ulNextIndex, lBinRow, lBinCol);
			}
		}
		else //grade full
		{
			SetMsgCode(IDS_BT_GRADEFULL);
			ulBlkToUse = 0;
		}

		if (GrabNVIsAligned(ulBlkToUse) == TRUE) //  conver value
		{
			//LONG lX1=0, lY1=0;
			DOUBLE dX1=0, dY1=0;
			DOUBLE dX=0, dY=0, dDistX=0, dDistY=0, dAngle=0;
			ULONG ul1stDieIndex = 1;

			//v4.39T9	//PLLM MS109 bin realignment bug-fix 
			//If binmap is enabled, need to get the "real" 1st die XY for calculation, 
			// not UL die XY pos;
			ULONG ulTempRow = 0, ulTempCol = 0;
			GetDieIndexInBinMap(ulBlkToUse, ul1stDieIndex, ulTempRow, ulTempCol);	
			m_oBinBlk[ulBlkToUse].FindBinTableXYPosnGivenIndex(ul1stDieIndex, dX1, dY1);

			dOrigX = dX1;
			dOrigY = dY1;

			dDistX = (DOUBLE)(dPosX - dX1);
			dDistY = (DOUBLE)(dPosY - dY1);
			dAngle = GrabNVRotateAngleX(ulBlkToUse);
			dX = dDistX * cos(dAngle) - dDistY * sin(dAngle) + GrabNVXOffset(ulBlkToUse);
			dY = dDistY * cos(dAngle) + dDistX * sin(dAngle) + GrabNVYOffset(ulBlkToUse);
			dPosX = dX + dX1;
			dPosY = dY + dY1;

			if (m_bEnableLogging)				//v4.39T9	//PLLM MS109
			{
				if (m_lEnableLogCount <= 0)		//v4.44A2
				{
					m_bEnableLogging = FALSE;
					m_lEnableLogCount = 0;
				}
				else
				{
					m_lEnableLogCount--;

					CString szLog;
					//szLog.Format("FindBondXYPosn: 1stDie (%ld, %ld), Last+1 (%ld, %ld), Angle=%.2f, Offset (%ld, %ld)",
					//			lX1, lY1, lX, lY, dAngle, GrabNVXOffset(ulBlkToUse), GrabNVYOffset(ulBlkToUse));
					szLog.Format("FindBondXYPosn: INDEX = %lu, um(%f, %f), New_um(%f, %f), Angle = %.2f deg, Offset(%ld, %ld)",
								ulNextIndex, dOrigX, dOrigY, dPosX, dPosY, 
								(dAngle * 180.0 / PI), GrabNVXOffset(ulBlkToUse), GrabNVYOffset(ulBlkToUse));
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
				}
			}
		}
		else
		{
			LONG lX1=0, lY1=0;
			DOUBLE dX=0, dY=0, dDistX=0, dDistY=0, dAngle=0;
			ULONG ul1stDieIndex = 1;

			if (m_bEnableLogging)				//v4.39T9	//PLLM MS109
			{
				if (m_lEnableLogCount <= 0)		//v4.44A2
				{
					m_bEnableLogging = FALSE;
					m_lEnableLogCount = 0;
				}
				else
				{
					m_lEnableLogCount--;

					CString szLog;
					//szLog.Format("FindBondXYPosn: 1stDie (%ld, %ld), Last+1 (%ld, %ld), Angle=%.2f, Offset (%ld, %ld)",
					//			lX1, lY1, lX, lY, dAngle, GrabNVXOffset(ulBlkToUse), GrabNVYOffset(ulBlkToUse));
					szLog.Format("FindBondXYPosn (not aligned): INDEX = %lu, um(%f, %f), New_um(%f, %f), Angle = %.2f deg, Offset(%ld, %ld)",
								ulNextIndex, dOrigX, dOrigY, dX, dY, 
								(dAngle * 180.0 / PI), GrabNVXOffset(ulBlkToUse), GrabNVYOffset(ulBlkToUse));
					CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
				}
			}
		}

		dXPosn = dPosX;
		dYPosn = dPosY;
		lRow = lBinRow;
		lCol = lBinCol;
	}
	else
	{
		//"No Bin Block has been assigned with the indicated grade yet!"
		SetMsgCode(IDS_BT_NOBINBLKASSIGN);
		//ulBlkToUse = 0;
		ulBlkToUse = Err_BinBlockNotAssign;
	}

	return  ulBlkToUse;
} //end FindBondXYPosn


INT CBinBlkMain::BondOK(ULONG ulBondedBlkId, LONG lWaferMapX, LONG lWaferMapY, LONG lWafEncX, LONG lWafEncY,
							LONG lBtEncX, LONG lBtEncY)
{
	UCHAR ucGrade;
	BOOL bSaveStatus;
	ULONG ulTwoDBarocdeEmptyHoleCount = 0;
	ULONG ulFirstRowColSkippedUnit = 0;
	ULONG ulWafflePadSkippedUnit = 0;
	BOOL bHole = FALSE;	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();


	//Record BinBlk 1st bonded die time
	if (m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() == 0)
	{
		CTime theTime = CTime::GetCurrentTime();
		CString szDate;

		int nYear	= theTime.GetYear();
		int nMonth	= theTime.GetMonth();
		int nDay	= theTime.GetDay();
		int nHour	= theTime.GetHour();
		int nMinute = theTime.GetMinute();
		int nSecond = theTime.GetSecond();

		szDate.Format("%d-%d-%d,%d:%d:%d", nYear, nMonth, nDay, nHour, nMinute, nSecond);

		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		if (pBTfile != NULL)
		{
			CString szWaferID;
			CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferID);
			int nCol = szWaferID.Find(".");
			if (nCol != -1)
			{
				szWaferID = szWaferID.Left(nCol);
			}
			(*pBTfile)["BinBlock"][ulBondedBlkId]["StartDate"] = szDate;
			(*pBTfile)["BinBlock"][ulBondedBlkId]["StartWaferID"] = szWaferID;
			CMSFileUtility::Instance()->SaveBTConfig();
		}

		m_oBinBlk[ulBondedBlkId].SetStartDate(szDate);
	}

	
	/*if (IsTwoDimensionBarcodeHole(ulBondedBlkId) == TRUE)
	{
		ULONG ulEmptyHoleUnit = 0;

		//Increase the index when hole is generated on bin for 2D Barcode (for Cree)	//v3.33T3
		m_oBinBlk[ulBondedBlkId].SetNVCurrBondIndex(m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 2);

		for (INT i = 0; i < (INT) m_ulHoleDieNum; i++)
		{
			if (m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() > GrabRandomHoleDieIndex(ulBondedBlkId, i))
			{
				ulEmptyHoleUnit++;
			}
		}

		m_ulCurrHoleDieNum[ulBondedBlkId] = ulEmptyHoleUnit;
		SaveCurHoleDieNum(ulBondedBlkId);
	}
	else if (IsFirstRowColSkipPatternSkipUnit(ulBondedBlkId) == TRUE)
	{	
		m_oBinBlk[ulBondedBlkId].SetNVCurrBondIndex(m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 2);	
	}
	else if (IsWafflePadSkipUnit(ulBondedBlkId) == TRUE)
	{
		m_oBinBlk[ulBondedBlkId].SetNVCurrBondIndex(m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 2);
	}
	else*/
	if (IsUseBinMapBondArea(ulBondedBlkId) || GrabEnableBinMap(ulBondedBlkId))
	{
		ULONG ulCurrIndex = m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 1;
		GetNextDieIndexInBinMap(ulBondedBlkId, ulCurrIndex);
		m_oBinBlk[ulBondedBlkId].SetNVCurrBondIndex(ulCurrIndex);

		//v4.48A29
		CString szLog;
		szLog.Format("BT: BondOK (bmap) - Index: %lu", ulCurrIndex);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}
	/*else if (CheckIfEmptyRow(ulBondedBlkId) == TRUE)		//v4.30T4	//CyOptics
	{
		//v4.31T10
		ULONG ulNextIndex = m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 1;		
		ULONG ulOrigIndex = ulNextIndex;
		CheckIfNeedToLeaveEmptyRow(ulBondedBlkId, ulNextIndex);
		//ulNextIndex = ulNextIndex + 1;
		m_oBinBlk[ulBondedBlkId].SetNVCurrBondIndex(ulNextIndex);
		UpdateLastFileSaveIndex(ulBondedBlkId, ulNextIndex - 1);			//v4.31T9	//FIxed bug of updating the NVRam LastDieIndex

		CString szLog;
		szLog.Format("BondOK: CheckIfEmptyRow (Bin #%lu) - Orig-Next = %lu; New-Next = %lu", ulBondedBlkId, ulOrigIndex, ulNextIndex);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLog);
		SetLeaveEmptyRow(ulBondedBlkId, FALSE);				//Reset the flag after update!
	}*/
	else 
	{
		ULONG ulCurrIndex = m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 1;
		m_oBinBlk[ulBondedBlkId].SetNVCurrBondIndex(ulCurrIndex);

		//v4.48A29
		CString szLog;
		szLog.Format("BT: BondOK - Index: %lu", ulCurrIndex);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
	}


	//NoOfBondedDie (CBinBlk)
	m_oBinBlk[ulBondedBlkId].SetNVNoOfBondedDie(m_oBinBlk[ulBondedBlkId].GetNVNoOfBondedDie() + 1);

	//IsFull (CBinBlk)
	if (m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() == m_oBinBlk[ulBondedBlkId].GetDiePerBlk())
	{
		m_oBinBlk[ulBondedBlkId].SetNVIsFull(TRUE);
	}

	//IsFull (CBinBlk -- Hit BinBlk Max Unit)
	if (m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() == m_oBinBlk[ulBondedBlkId].GetMaxUnit())
	{
		CMSLogFileUtility::Instance()->BT_ExchangeFrameLog("BondOK 1");		//v3.35
		m_oBinBlk[ulBondedBlkId].SetNVIsFull(TRUE);
	}

	//IsFull (CBinBlk -- Hit Empty Unit by enable LSBondPattern)
	/*if (GrabLSBondPattern() == TRUE)
	{
		if (m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() ==
				m_oBinBlk[ulBondedBlkId].GetDiePerBlk() 
				- (m_oBinBlk[ulBondedBlkId].GetDiePerRow() - 1)
				- (m_oBinBlk[ulBondedBlkId].GetDiePerCol() - 1)
				+ 1)
		{
			CMSLogFileUtility::Instance()->BT_ExchangeFrameLog("BondOK 2");	
			m_oBinBlk[ulBondedBlkId].SetNVIsFull(TRUE);
		}
	}*/


	//Get the blk grade
	ucGrade = m_oBinBlk[ulBondedBlkId].GetGrade();
	ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	

	//NoOfSortedDie (CBinGradeData) 
	m_oBinGradeData[ucGrade].SetNVNoOfSortedDie(m_oBinGradeData[ucGrade].GetNVNoOfSortedDie() + 1);

	ulTwoDBarocdeEmptyHoleCount = TwoDimensionBarcodeGetTotalNoOfHoleOnFrame(ucGrade);
	ulFirstRowColSkippedUnit = GetFirstRowColSkipPatTotalSkipUnitCount(ulBondedBlkId);
	ulWafflePadSkippedUnit = GetWafflePadSkipUnitTotalCount(ulBondedBlkId);

	//IsFull (CBinGradeData)
	if (m_oBinGradeData[ucGrade].GetInputCount() <= (m_oBinGradeData[ucGrade].GetNVNoOfSortedDie() + 
			ulTwoDBarocdeEmptyHoleCount + ulFirstRowColSkippedUnit + ulWafflePadSkippedUnit))
	{
		CMSLogFileUtility::Instance()->BT_ExchangeFrameLog("BondOK 3");		//v3.35
		m_oBinGradeData[ucGrade].SetNVIsFull(TRUE);
	}


	//Save die info to output file
	bSaveStatus = m_oBinBlk[ulBondedBlkId].SaveDieInfo(m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex(), 
				  lWaferMapX, lWaferMapY, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime,
				  lWafEncX, lWafEncY, lBtEncX, lBtEncY);	//andrewng //2020-0619


	//v4.65A1	//TJ/WH/XM SanAn
	// Save the last 500 die's ROW/COL into back of NVRAM, for sw crash recovery purpose;
	// RESET this data whenever GenAllTempFile() succeeds to pull data into TEMP file; 
	// lWaferMapY = ROW, lWaferMapX = COL
	SetNVTempFileData((LONG) ulBondedBlkId, lWaferMapY, lWaferMapX, m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex());	


	//CString szMsg;
	//szMsg.Format("XMoutput debug4 (%d,%d)", lWaferMapX, lWaferMapY);
	//CMSLogFileUtility::Instance()->WT_GetIdxLog(szMsg);
	if ((bSaveStatus == TRUE) && (m_bEnableNVRunTimeData == TRUE))
	{
		//Save RunTime data into NVRAM
		LONG lRet = SaveNVRunTimeData(ulBondedBlkId, ucGrade, lWaferMapX, lWaferMapY);
		if ((lRet == BT_BACKUP_NVRAM_ERROR) || (lRet == BT_BACKUP_NVRAM))
		{
			return lRet;
		}
	}

	return bSaveStatus;
} //end BondOK


BOOL CBinBlkMain::SaveDieInfoToFile(UCHAR ucGrade, ULONG ulBondedBlk, 
									LONG lBTXInDisplayUnit, LONG lBTYInDisplayUnit, 
									LONG lBinTableX, LONG lBinTableY,
									LONG lWaferX, LONG lWaferY)
{
	CStdioFile cfDieInfo;
	CString szFileContent;

	if (!cfDieInfo.Open("BondResult.txt", 
				   CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
	{
		return FALSE;
	}
	
	szFileContent.Format("%d, %d, %d, %d, %d, %d, %d, %d", ucGrade, ulBondedBlk, 
						 lBTXInDisplayUnit, lBTYInDisplayUnit,
						 lBinTableX, lBinTableY, lWaferX, lWaferY);
	
	cfDieInfo.SeekToEnd();
	cfDieInfo.WriteString(szFileContent);
	cfDieInfo.WriteString("\n");   
	cfDieInfo.Close();

	return TRUE;
} //end SaveDieInfoToFile


/******************************/
/*          Postbond          */
/******************************/
BOOL CBinBlkMain::PostBondOK(ULONG ulBinBlkId, ULONG ulIndex, DOUBLE dDieAngle, BOOL bIsGoodDie,
							 UCHAR ucDieType)
{
	return TRUE;
} //end PostBondOK

/*******************************/
/*      Clear Block Setting    */
/*******************************/

VOID CBinBlkMain::ClrSingleBlkSettings(ULONG ulBlkId)
{
	UCHAR ucBlkGrade;
	ULONG i;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	if (pBTfile == NULL)
	{
		return;
	}

	ucBlkGrade = GrabGrade(ulBlkId);

	ucBlkGrade = min(ucBlkGrade, BT_MAX_BINBLK_SIZE - 1);		//Klocwork	//v4.02T5
	//CBinBlk: SerialNo

	//CBinGradeData: GradeCapacity
	m_oBinGradeData[ucBlkGrade].SetGradeCapacity
	(m_oBinGradeData[ucBlkGrade].GetGradeCapacity() - m_oBinBlk[ulBlkId].GetDiePerBlk());
	(*pBTfile)["BinGradeData"][ucBlkGrade]["GradeCapacity"] =
		m_oBinGradeData[ucBlkGrade].GetGradeCapacity();

	//CBinGradeData: InputCount
	if (m_oBinGradeData[ucBlkGrade].GetInputCount() > m_oBinGradeData[ucBlkGrade].GetGradeCapacity())
	{
		m_oBinGradeData[ucBlkGrade].SetInputCount(m_oBinGradeData[ucBlkGrade].GetGradeCapacity());
		(*pBTfile)["BinGradeData"][ucBlkGrade]["InputCount"] =
			m_oBinGradeData[ucBlkGrade].GetInputCount();
	}

	//CBinGradeData: BlkInUse
	if (m_oBinGradeData[ucBlkGrade].GetNVBlkInUse() == ulBlkId)
	{
		m_oBinGradeData[ucBlkGrade].SetNVBlkInUse(0);

		for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
		{
			if ((i != ulBlkId) && (m_oBinBlk[i].GetGrade() == ucBlkGrade))
			{
				m_oBinGradeData[ucBlkGrade].SetNVBlkInUse(i);
				break;
			}
		}
	}
	
	//CBinGradeData: IsAssigned 
	if (m_oBinGradeData[ucBlkGrade].GetGradeCapacity() == 0)
	{
		m_oBinGradeData[ucBlkGrade].SetIsAssigned(FALSE);
	}
	else
	{
		m_oBinGradeData[ucBlkGrade].SetIsAssigned(TRUE);
	}

	(*pBTfile)["BinGradeData"][ucBlkGrade]["IsAssigned"] =
		m_oBinGradeData[ucBlkGrade].GetIsAssigned();

	//CBinBlk: UpperLeftX
	m_oBinBlk[ulBlkId].SetUpperLeftX(0);

	//CBinBlk: UpperLeftXFromSetup
	m_oBinBlk[ulBlkId].SetUpperLeftXFromSetup(0);

	//CBinBlk: UpperLeftY
	m_oBinBlk[ulBlkId].SetUpperLeftY(0);

	//CBinBlk: UpperLeftYFromSetup
	m_oBinBlk[ulBlkId].SetUpperLeftYFromSetup(0);

	//CBinBlk: LowerRightX
	m_oBinBlk[ulBlkId].SetLowerRightX(0);

	//CBinBlk: LowerRightXFromSetup
	m_oBinBlk[ulBlkId].SetLowerRightXFromSetup(0);

	//CBinBlk: LowerRightY
	m_oBinBlk[ulBlkId].SetLowerRightY(0);

	//CBinBlk: LowerRightYFromSetup
	m_oBinBlk[ulBlkId].SetLowerRightYFromSetup(0);

	//CBinBlk: Width
	m_oBinBlk[ulBlkId].SetWidth();

	//CBinBlk: WidthFromSetup
	m_oBinBlk[ulBlkId].SetWidthFromSetup();

	//CBinBlk: Height
	m_oBinBlk[ulBlkId].SetHeight();

	//CBinBlk: HeightFromSetup
	m_oBinBlk[ulBlkId].SetHeightFromSetup();

	//CBinBlk: DiePitch XY
	m_oBinBlk[ulBlkId].SetDDiePitchX(500);
	m_oBinBlk[ulBlkId].SetDDiePitchY(500);
	m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchX(FALSE);
	m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchY(FALSE);
	m_oBinBlk[ulBlkId].SetBondAreaOffset(0, 0);

	m_oBinBlk[ulBlkId].SetByPassBinMap(FALSE);	//v4.48A11	//Semitek, 3E DL

	//CBinBlk: WalkPath
	m_oBinBlk[ulBlkId].SetWalkPath(0);

	//CBinBlk: SkipUnit
	m_oBinBlk[ulBlkId].SetSkipUnit(0);

	//CBinBlk: MaxUnit
	m_oBinBlk[ulBlkId].SetMaxUnit(0);

	//CBinBlk: StartDate
	m_oBinBlk[ulBlkId].SetStartDate("");

	//CBinBlk: DiePerRow
	m_oBinBlk[ulBlkId].SetDiePerRow(0);

	//CBinBlk: DiePerCol
	m_oBinBlk[ulBlkId].SetDiePerCol(0);

	//CBinBlk: DiePerBlk
	m_oBinBlk[ulBlkId].SetDiePerBlk(0);

	//CBinBlk: Grade
	m_oBinBlk[ulBlkId].SetGrade(1);

	//CBinBlk: Original Grade
	m_oBinBlk[ulBlkId].SetOriginalGrade(1);

	//CBinBlk: IsDisableFromSameGradeMerge
	m_oBinBlk[ulBlkId].SetIsDisableFromSameGradeMerge(FALSE);

	//CBinBlk: PhyBlkId
	m_oBinBlk[ulBlkId].SetPhyBlkId(0);

	//CBinBlk: NoOfBondedDie
	m_oBinBlk[ulBlkId].SetNVNoOfBondedDie(0);

	//CBinBlk: CurrBondIndex
	m_oBinBlk[ulBlkId].SetNVCurrBondIndex(0);

	//CBinBlk: LastFileSaveIndex
	m_oBinBlk[ulBlkId].SetNVLastFileSaveIndex(0);

	//CBinBlk: IsFull
	m_oBinBlk[ulBlkId].SetNVIsFull(FALSE);

	//CBinBlk: IsSetup
	m_oBinBlk[ulBlkId].SetIsSetup(FALSE);

	//CBinBlk: Status
	m_oBinBlk[ulBlkId].SetStatus(BT_BLK_NORMAL);

	//CBinBlk: Empty unit
	m_oBinBlk[ulBlkId].SetEmptyUnit(0);

	//Remove Temp file
	RemoveTempFileAndResetIndex(ulBlkId, '0', CLEAR_BIN_BY_PHY_BLK);

	//Update No of binblock if this block exist in file
	UCHAR ucGrade = (UCHAR)(*pBTfile)["BinBlock"][ulBlkId]["Grade"];
	if (ucGrade != 0)
	{
		if (m_ulNoOfBlk >= 1)
		{	
			m_ulNoOfBlk = m_ulNoOfBlk - 1;
		}

		(*pBTfile)["BinBlock"]["NoOfBlk"] = m_ulNoOfBlk;
	}

	//Remove values from string map file
	(*pBTfile)["BinBlock"].Remove(ulBlkId);

	CMSFileUtility::Instance()->SaveBTConfig();

} //end ClrSingleBlkSettings


VOID CBinBlkMain::ClrAllBlksSettings(BOOL bResetSerial)
{
	ULONG i;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	if (pBTfile == NULL)
	{
		return;
	}

	//Clear data structures
	m_ulNoOfBlk = 0;
	m_lBlkPitchX = 0;
	m_lBlkPitchY = 0;

	for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		//Clear All grade setting

		//CBinGradeData: GradeCapacity
		m_oBinGradeData[i].SetGradeCapacity(0);

		//CBinGradeData: AliasGrade
		m_oBinGradeData[i].SetAliasGrade(0);

		//CBinGradeData: InputCount
		m_oBinGradeData[i].SetInputCount(0);
		
		//m_oBinGradeData[i].SetMaxFrameDieCount(0);

		//CBinGradeData: IsAssigned
		m_oBinGradeData[i].SetIsAssigned(FALSE);

		//CBinGradeData: NoOfSortedDie
		m_oBinGradeData[i].SetNVNoOfSortedDie(0);

		//CBinGradeData: BlkInUse
		m_oBinGradeData[i].SetNVBlkInUse(0);

		//CBinGradeData: IsFull
		m_oBinGradeData[i].SetNVIsFull(FALSE);

		//CBinBlk: SerialNo
		//if (bResetSerial == TRUE)
		//ResetSerialNo(i);

		//CBinBlk: UpperLeftX
		m_oBinBlk[i].SetUpperLeftX(0);

		//CBinBlk: UpperLeftXFromSetup
		m_oBinBlk[i].SetUpperLeftXFromSetup(0);

		//CBinBlk: UpperLeftY
		m_oBinBlk[i].SetUpperLeftY(0);

		//CBinBlk: UpperLeftYFromSetup
		m_oBinBlk[i].SetUpperLeftYFromSetup(0);

		//CBinBlk: LowerRightX
		m_oBinBlk[i].SetLowerRightX(0);

		//CBinBlk: LowerRightXFromSetup
		m_oBinBlk[i].SetLowerRightXFromSetup(0);

		//CBinBlk: LowerRightY
		m_oBinBlk[i].SetLowerRightY(0);

		//CBinBlk: LowerRightYFromSetup
		m_oBinBlk[i].SetLowerRightYFromSetup(0);

		//CBinBlk: Width
		m_oBinBlk[i].SetWidth();

		//CBinBlk: WidthFromSetup
		m_oBinBlk[i].SetWidthFromSetup();

		//CBinBlk: Height
		m_oBinBlk[i].SetHeight();

		//CBinBlk: HeightFromSetup
		m_oBinBlk[i].SetHeightFromSetup();

		//CBinBlk: DiePitchX
		m_oBinBlk[i].SetDDiePitchX(500);
		m_oBinBlk[i].SetDDiePitchY(500);
		m_oBinBlk[i].SetPoint5UmInDiePitchX(FALSE);
		m_oBinBlk[i].SetPoint5UmInDiePitchY(FALSE);
		m_oBinBlk[i].SetBondAreaOffset(0, 0);

		m_oBinBlk[i].SetByPassBinMap(FALSE);	//v4.48A11	//Semitek, 3E DL

		//CBinBlk: WalkPath
		m_oBinBlk[i].SetWalkPath(0);

		//CBinBlk: SkipUnit
		m_oBinBlk[i].SetSkipUnit(0);

		//CBinBlk: MaxUnit
		m_oBinBlk[i].SetMaxUnit(0);

		//CBinBlk: StartDate
		m_oBinBlk[i].SetStartDate("");

		//CBinBlk: DiePerRow
		m_oBinBlk[i].SetDiePerRow(0);

		//CBinBlk: DiePerCol
		m_oBinBlk[i].SetDiePerCol(0);

		//CBinBlk: DiePerBlk
		m_oBinBlk[i].SetDiePerBlk(0);

		//CBinBlk: Grade
		m_oBinBlk[i].SetGrade(1);

		//CBinBlk: Original Grade
		m_oBinBlk[i].SetOriginalGrade(1);

		//CBinBlk: IsDisableFromSameGradeMerge
		m_oBinBlk[i].SetIsDisableFromSameGradeMerge(FALSE);

		//CBinBlk: PhyBlkId
		m_oBinBlk[i].SetPhyBlkId(0);

		//CBinBlk: NoOfBondedDie
		m_oBinBlk[i].SetNVNoOfBondedDie(0);

		//CBinBlk: CurrBondIndex
		m_oBinBlk[i].SetNVCurrBondIndex(0);

		//CBinBlk: LastFileSaveIndex
		m_oBinBlk[i].SetNVLastFileSaveIndex(0);

		//CBinBlk: IsFull
		m_oBinBlk[i].SetNVIsFull(FALSE);

		//CBinBlk: IsSetup
		m_oBinBlk[i].SetIsSetup(FALSE);

		//CBinBlk: Status
		m_oBinBlk[i].SetStatus(BT_BLK_NORMAL);

		//CBinBlk: Empty unit
		m_oBinBlk[i].SetEmptyUnit(0);

		//Remove Temp file
		RemoveTempFileAndResetIndex(i, '0', CLEAR_BIN_BY_PHY_BLK);
	}

	//Remove values from string map file
	(*pBTfile).Remove("BinBlock");
	(*pBTfile).Remove("BinGradeData");

	CMSFileUtility::Instance()->SaveBTConfig();

} //end ClrAllBlksSettings


VOID CBinBlkMain::ClrBlkSettingsGivenPhyBlkId(ULONG ulPhyBlkId)
{
	ULONG i;
	
	for (i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetPhyBlkId() == ulPhyBlkId)
		{
			ClrSingleBlkSettings(i);
		}
	}
} //end ClrBlkSettingsGivenPhyBlkId


VOID CBinBlkMain::ClrAllGradeInfo()
{
	ULONG i;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return;
	}

	//Clear data structures
	for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		m_oBinGradeData[i].ClrGradeInfo();
	}

	//Remove values from string map file
	(*pBTfile).Remove("BinGradeData");

	CMSFileUtility::Instance()->SaveBTConfig();
} 


/************************************************/
/*    Clear Bin Counter (by Physical Block)     */
/************************************************/
BOOL CBinBlkMain::ClearBinCounterByPhyBlk(ULONG ulBlkToClear, CStringMapFile *pBTMSDfile)
{
	ULONG i;

	CStringMapFile *pBTfile = pBTMSDfile;
	if (pBTMSDfile == NULL)
	{
		pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	}

	if (ulBlkToClear > 80)	//MS100/MS60 max size = 150	//v4.52A8
	{
		ULONG ulIndex = 0;
		for (i=GetNoOfBlk(); i>=1; i--)
		{
			if (m_oBinBlk[i].GetPhyBlkId() == ulBlkToClear)
			{
				i = min(i, BT_MAX_BINBLK_SIZE - 1);	//v4.53	Klocwork

				if (m_bStoreBinBondedCount == TRUE)
				{		
					ULONG ulAccCount = m_oBinBlk[i].GetNVNoOfBondedDie() + m_oBinBlk[i].GetAccmulatedCount();
					SetBinAccBondedCount(i, ulAccCount, pBTfile);		//v4.52A8
				}

				ClearBinCounter(i, FALSE);	//v4.52A8

				//v4.52A8	
				//Do it here instead of inside ClearBinCounter() to avoid too many OPEN/CLOSE
				if (pBTfile != NULL)
				{
					(*pBTfile)["BinBlock"][i]["Status"]		= m_oBinBlk[i].GetStatus();
					(*pBTfile)["BinBlock"][i]["EmptyUnit"]	= m_oBinBlk[i].GetEmptyUnit();
				}
			}
		}
	}
	else
	{
		for (i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[i].GetPhyBlkId() == ulBlkToClear)
			{
				if (m_bStoreBinBondedCount == TRUE)
				{		
					ULONG ulAccCount = m_oBinBlk[i].GetNVNoOfBondedDie() + m_oBinBlk[i].GetAccmulatedCount();
					SetBinAccBondedCount(i, ulAccCount, pBTfile);	//v4.52A8
				}

				ClearBinCounter(i, FALSE);	//v4.52A8

				//v4.52A8	
				//Do it here instead of inside ClearBinCounter() to avoid too many OPEN/CLOSE
				if (pBTfile != NULL)
				{
					(*pBTfile)["BinBlock"][i]["Status"]		= m_oBinBlk[i].GetStatus();
					(*pBTfile)["BinBlock"][i]["EmptyUnit"]	= m_oBinBlk[i].GetEmptyUnit();
				}
			}
		}
	}

	if (pBTMSDfile == NULL)
	{
		CMSFileUtility::Instance()->SaveBTConfig();
	}
	CheckIfGradesAreStillValid();
	return TRUE;

} //end ClearBinCounterByPhyBlk

BOOL CBinBlkMain::SaveClearBinCountDataForOutputFileByBlkId(ULONG ulBlkId, BOOL bIfGenOutputFile, BOOL bUseOutputBarcode,
		CString szMachineNo, CString szLotNo, CString szBinLotDirectory, CString szOutputFileSummaryPath,
		CString szFileNo, CString szBlueTapeNo, 
		CString szMapSpecVersion, CString szBinParameter, 
		CString szOutputFileNamingFormat,
		CString szOutputFilename, CString szUnloadDate, 
		CString szUnloadTime, CString szOutputBarcode,
		CString szLabelFilePath, 
		CString szWorkNo, CString szPrefix, CString szLoadPKGCount,
		CString szType, CString szSortBin, CString szDevice, 
		CString szWaferBin, CString szModelNo, CString szLotID,
		CString szProduct, CString szBTSpecVersion, CString szMachineType,
		CString szSlotID, CString szMapLotNo, CString szOperatorId, CString szBTOrientation,
		CString szaExtraBinInfoField[], CString szaExtraBinInfo[],
		CString szSoftwareVersion, CString szPKGFilename, CString szProberMachineNo, 
		CString szProberDateTime, CString szBinTableSerialNo, CString szMESOutgoingPath,
		CString szInternalProductNo, CString szMode, CString szSpecification, CString szMapOther,
		CString szSortBinFilename, CString szWaferLotFilename, CString szMapBinTable, 
		CString szDuplicateFileRetryCount, CString szESDVoltage,
		CString szTestTime, CString szWO, CString szSubstarteID, CString szCSTID,
		CString szRecipeID, CString szRecipeName, CString szMapFileOperator,
		CString szMapFileDate, CString szRemark1, CString szRemark2, 
		CString szPartNo, CString szBinDieType, LONG lBinBarcodeMinDigit,
		CString szDN, CString szWN, CString szCM1, CString szCM2, CString szCM3,
		CString szUN, CString szCOND, CString szLO, CString szUP, CString szFT, CString szLIM,
		CString szCOEF1, CString szCOEF2, CString szCOEF3, CString szPLLMDieFabMESLot, CString szFullMagSlot,
		CString szLabelPrinterSel, CString szMapSerialNumber, CString szRetryCount, CString szWaferMapCenterPoint,
		CString szWaferMapLifeTime, CString szWaferMapESD, CString szWaferMapPadColor, CString szWaferMapBinCode, 
		CString szMachineModel, CString szPRDeviceID, CString szUseBarcode,
		CString szOutputFiePath2, BOOL bForSummary,
		CStringMapFile *pSMapFile)	
{
	CString szForGenerateSummary;
	CString szBinBlkId, szBinBlkGrade, szDieCount, szSerialNo;
	CString szBinBlkOrgGrade;
	CString szBinBlkAlgorithm;
	CString	szDiePerRow, szDiePerCol;
	CString szBinBarcodeMinDigit;
	CString szWaffleSizeX, szWaffleSizeY;
	CString szIsEnableWafflePad;
	CString szGetFirstRowSkipUnitCount;
	CString szRemark3;
	CString szUpperLeftX, szUpperLeftY;
	CString szLowerRightX, szLowerRightY;
	CString szOrgGradeString;
	CString szControlRank, szRankNo;
	CString szTotalNoOfBlk;
	CString szBMRadius;
	CString szIsEnableBM;
	CString szBinFullCount;

	CGenerateDatabase	*pGenDB = ((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[ulBlkId]);
	CQueryDatabase		*pQryDB = (CQueryDatabase*)CMS896AStn::m_objaQueryDatabase[ulBlkId];
	if (pGenDB == NULL)
	{
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Generate data base is null for this block");
		return FALSE;
	}
	if( pQryDB==NULL )
	{
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Query data base is null for this block");
		return FALSE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("To Cleanup Query DB before clear bin");
	pQryDB->Cleanup();
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("To Cleanup Gen DB before clear bin");
	pGenDB->Cleanup();
	//Bond Start Date
	pGenDB->SetBinBlockInfoItems("Bond Start Date",		pApp->GetBondStartDate());
	//Bond Start Time
	pGenDB->SetBinBlockInfoItems("Bond Start Time",		pApp->GetBondStartTime());
	//Bond End Date
	pGenDB->SetBinBlockInfoItems("Bond End Date",		pApp->GetBondStopDate());
	//Bond End Time
	pGenDB->SetBinBlockInfoItems("Bond End Time",		pApp->GetBondStopTime());

	szForGenerateSummary.Format("%d", bForSummary);

	szBinBlkId.Format("%d", ulBlkId);
	szBinBlkGrade.Format("%u", GrabGrade(ulBlkId));
	szDieCount.Format("%d", m_oBinBlk[ulBlkId].GetNVNoOfBondedDie());

	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Save Data to GenDB Grade:" + szBinBlkGrade + " Blk Id:" + szBinBlkId);
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Barcode:" + szOutputBarcode + " Die Count:" + szDieCount);

	szSerialNo.Format("%d", m_oBinBlk[ulBlkId].GetSerialNo());
	szBinBarcodeMinDigit.Format("%d", lBinBarcodeMinDigit);
	szGetFirstRowSkipUnitCount.Format("%d", m_oBinBlk[ulBlkId].GetFirstRowColSkipUnit());
	szWaffleSizeX.Format("%d", m_oBinBlk[ulBlkId].GetWafflePadSizeX());
	szWaffleSizeY.Format("%d", m_oBinBlk[ulBlkId].GetWafflePadSizeY());

	szUpperLeftX.Format("%d", m_oBinBlk[ulBlkId].GetUpperLeftX());
	szUpperLeftY.Format("%d", m_oBinBlk[ulBlkId].GetUpperLeftY());
	szLowerRightX.Format("%d", m_oBinBlk[ulBlkId].GetLowerRightX());
	szLowerRightY.Format("%d", m_oBinBlk[ulBlkId].GetLowerRightY());

	szIsEnableBM.Format("%d", m_oBinBlk[ulBlkId].m_bEnableBinMap);

	szUseBarcode.Format("%d", bUseOutputBarcode);


	unsigned short usOrgGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(GrabGrade(ulBlkId) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset())
								- CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
	szBinBlkOrgGrade.Format("%d", usOrgGrade);
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("OrgGrade:" + szBinBlkOrgGrade);

	CString szCategory = "";
	CStringMapFile  *psmf;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();

	if (pUtl->LoadLastState("BBX: SaveClearBinCountDataForOutputFileByBlkId") == TRUE) 
	{
		psmf = pUtl->GetLastStateFile("BBX: SaveClearBinCountDataForOutputFileByBlkId");		
		if (psmf != NULL)	//Klocwork	//v4.02T5
		{
			CString szCategory	= (*psmf)[WT_MAP_HEADER_INFO]["Map Header Category"];
		}

		pUtl->CloseLastState("BBX: SaveClearBinCountDataForOutputFileByBlkId");
	}

	szOrgGradeString = CMS896AStn::m_WaferMapWrapper.GetOriginalGradeString(GrabGrade(ulBlkId) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset());
	//szOrgGradeString = CMS896AStn::m_WaferMapWrapper.GetReader()->GetOriginalGradeString(GrabGrade(ulBlkId) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset());
	if (szCategory == "")
	{
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("NumOrgGrade:" + szBinBlkOrgGrade);
		szOrgGradeString = szBinBlkOrgGrade;
	}

	szBinBlkAlgorithm.Format("%d", m_oBinBlk[ulBlkId].GetWalkPath());
	szIsEnableWafflePad.Format("%d", m_oBinBlk[ulBlkId].GetIsEnableWafflePad());
	szDiePerRow.Format("%d", m_oBinBlk[ulBlkId].GetDiePerRow());
	szDiePerCol.Format("%d", m_oBinBlk[ulBlkId].GetDiePerCol());
	szBinFullCount.Format("%d",GrabNoOfDiePerBlk(ulBlkId));
	(*pSMapFile)["BinTable"]["ClearBin"]["Bin Full Count"] = szBinFullCount;

	CString szExtraInfoFormat	= m_oBinBlk[ulBlkId].GetExtraInfoFormat();
	CString szStartDate			= m_oBinBlk[ulBlkId].GetStartDate();

	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Save Data to GenDB Barcode:" + szOutputBarcode + " Grade:" + szBinBlkGrade + " Blk Id:" + szBinBlkId +  
			" Die Count:" + szDieCount);

	//Software Version
	pGenDB->SetSoftwareInfoItems("Software Version", szSoftwareVersion);

	//Package Filename
	pGenDB->SetSoftwareInfoItems("Package Filename", szPKGFilename);

	//MachineNo
	pGenDB->SetMachineInfoItems("Machine No",	szMachineNo);

	pGenDB->SetMachineInfoItems("Machine Model", szMachineModel);

	//Machine Type
	pGenDB->SetMachineInfoItems("Machine Type", szMachineType);
	
	//Operator Id
	pGenDB->SetMachineInfoItems("Operator Id", szOperatorId);

	// Output File Summary Path
	pGenDB->SetMachineInfoItems("Output File Summary Path", szOutputFileSummaryPath);

	pGenDB->SetMachineInfoItems("Use Barcode", szUseBarcode);
	
	pGenDB->SetOthersItems("Generate Summary", szForGenerateSummary);
	//LotNo
	pGenDB->SetOthersItems("Lot No", szLotNo);

	//Bin Lot Directory
	pGenDB->SetOthersItems("Bin Lot Directory", szBinLotDirectory);

	//Work No
	pGenDB->SetOthersItems("Work No", szWorkNo);

	//File name prefix
	pGenDB->SetOthersItems("FileName Prefix", szPrefix);

	//OutputFileNamingFormat (CBinBlk)
	pGenDB->SetOthersItems("Output file-naming format", szOutputFileNamingFormat);

	//Wafermap extra info is space or comma seperate
	pGenDB->SetOthersItems("Extra Information Format", szExtraInfoFormat);

	pGenDB->SetOthersItems("Use Output Barcode", szUseBarcode);

	pGenDB->SetOthersItems("Label Orientation",		"Portrait");
	pGenDB->SetOthersItems("Label File Path",		szLabelFilePath);
	
	CString szSubBin;
	szSubBin.Format("%d",CMS896AApp::m_bEnableSubBin);
	pGenDB->SetOthersItems("Enable Sub Bin",		szSubBin);

	szTotalNoOfBlk.Format("%d", GetNoOfBlk());
	pGenDB->SetBinBlockInfoItems("Total No Of Blocks",	szTotalNoOfBlk);

	//FileNo (CBinBlk)
	pGenDB->SetBinBlockInfoItems("File No",		szFileNo);

	//BlueTapeNo (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Blue Tape No", szBlueTapeNo);

	//Map SpecVersion (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Spec Version", szMapSpecVersion);

	//BT SpecVersion (CBinBlk)
	pGenDB->SetBinBlockInfoItems("BT Spec Version", szBTSpecVersion);

	// Bin Die Type check items
	pGenDB->SetBinBlockInfoItems("Bin Die Type",	szBinDieType);

	//Bin Block Id (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Bin Block Id",	szBinBlkId);

	//Bin Block Grade (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Bin Block Grade", szBinBlkGrade);
	if (pSMapFile != NULL)
	{
		//Osram Penang RESORT map SortingBin header field
		CString szWafSortingBin	= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTINGBIN];
		if (szWafSortingBin != "")
		{
			pGenDB->SetBinBlockInfoItems("Bin Block Grade", szWafSortingBin);
		}
	}

	//Bin Block Original Grade (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Bin Block Original Grade",		szBinBlkOrgGrade);
	pGenDB->SetBinBlockInfoItems("Bin Block Original Grade String",	szOrgGradeString);		//v4.31T2	//PLSG

	//Die Count (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Die Count", szDieCount);

	//Start Date & Time (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Start Date/Time", szStartDate);

	//Unload Date (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Unload Date", szUnloadDate);

	//Unload Time (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Unload Time", szUnloadTime);

	//Serial No (CBinBlk)
	pGenDB->SetBinBlockInfoItems("Serial No",	szSerialNo);

	//Output Barcode Name & Option
	CMSPrescanUtility  *pUtility = CMSPrescanUtility::Instance();
	if (pApp->GetCustomerName() == CTM_SEMITEK && pUtility->GetPrescanDummyMap())
	{
		if (pSMapFile != NULL)
		{
			CString szResortModeBarcode = (*pSMapFile)["WaferTable"]["WT1InBarcode"];
			pGenDB->SetBinBlockInfoItems("Output Barcode", szResortModeBarcode);
		}
	}
	else
	{
		pGenDB->SetBinBlockInfoItems("Output Barcode", szOutputBarcode);
	}

	//Die per row
	pGenDB->SetBinBlockInfoItems("Die Per Row", szDiePerRow);

	//Die per col
	pGenDB->SetBinBlockInfoItems("Die Per Col", szDiePerCol);
	//Bin Full Count
	pGenDB->SetBinBlockInfoItems("Bin Full Count", szBinFullCount);

	//Bin Life Time
	pGenDB->SetBinBlockInfoItems("Life Time", GrabLifeTime());

	//Bin ESD
	pGenDB->SetBinBlockInfoItems("ESD", GrabESD());
	
	//Bin IF
	pGenDB->SetBinBlockInfoItems("IF", GrabIF());

	// BinTable Filename
	pGenDB->SetBinBlockInfoItems("Bin Table Filename", GrabBinTableFilename());

	// TapeID
	pGenDB->SetBinBlockInfoItems("Tape Id",		GrabTapeID());

	//v4.43T8
	//v4.42andrewng
	//if (m_oBinBlk[ulBlkId].GetUseCircularArea() == TRUE)
	if (CMS896AStn::m_oNichiaSubSystem.IsEnabled() && CMS896AStn::m_oNichiaSubSystem.m_bEnableArrCodeFcn)
	{
		//pGenDB->SetBinBlockInfoItems("Output Pattern", "1");
		CString szCode;
		szCode.Format("%d", CMS896AStn::m_oNichiaSubSystem.GetBinArrCode(ulBlkId));
		pGenDB->SetBinBlockInfoItems("Output Pattern", szCode);
	}
	else
	{
		pGenDB->SetBinBlockInfoItems("Output Pattern", "0");
	}

	// Block Size
	pGenDB->SetBinBlockInfoItems("Block Size",	GrabBlockSize(GrabGrade(ulBlkId)));

	//Bin Name
	pGenDB->SetBinBlockInfoItems("RankID",		GrabRankID(GrabGrade(ulBlkId)));

	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("To grab bin clear count");
	//Bin cleared count
	pGenDB->SetBinBlockInfoItems("Clear Count", GrabBinClearedCount());

	pGenDB->SetBinBlockInfoItems("Bin Blk Algorithm",	szBinBlkAlgorithm);

	pGenDB->SetBinBlockInfoItems("Enable Waffle Pad",	szIsEnableWafflePad);

	pGenDB->SetBinBlockInfoItems("Waffle Pad SizeX",	szWaffleSizeX);

	pGenDB->SetBinBlockInfoItems("Waffle Pad SizeY",	szWaffleSizeY);

	pGenDB->SetBinBlockInfoItems("First Row Skip Unit", szGetFirstRowSkipUnitCount);

	pGenDB->SetBinBlockInfoItems("Upper Left X", szUpperLeftX);
	pGenDB->SetBinBlockInfoItems("Upper Left Y", szUpperLeftY);
	pGenDB->SetBinBlockInfoItems("Lower Right X", szLowerRightX);
	pGenDB->SetBinBlockInfoItems("Lower Right Y", szLowerRightY);

	//v4.57A9	//Finisar
	CString szBinDiePitchX, szBinDiePitchY;
	szBinDiePitchX.Format("%ld", _round(GrabDDiePitchX(ulBlkId)));
	szBinDiePitchY.Format("%ld", _round(GrabDDiePitchY(ulBlkId)));
	pGenDB->SetBinBlockInfoItems("Bin Die Pitch X", szBinDiePitchX);
	pGenDB->SetBinBlockInfoItems("Bin Die Pitch Y", szBinDiePitchY);

	//Add the circle case
	pGenDB->SetBinBlockInfoItems("Is Enable BinMap", szIsEnableBM);

	//Load PKG file count
	pGenDB->SetOthersItems("PKG File Count",		szLoadPKGCount);

	//Wafermap Lot ID (For Epivalley)
	pGenDB->SetOthersItems("WaferMap LotID",		szLotID);

	//Wafermap BinParameter (For Semileds)
	pGenDB->SetOthersItems("WaferMap BinParameter", szBinParameter);

	//Wafermap "Device, WaferBins" (For OSRAM)
	pGenDB->SetOthersItems("WaferMap Device",		szDevice);

	pGenDB->SetOthersItems("WaferMap WaferBins",	szWaferBin);

	//Wafermap "ModelNo, Product" (For Unitex)
	pGenDB->SetOthersItems("WaferMap ModelNo",	szModelNo);

	pGenDB->SetOthersItems("WaferMap Product",	szProduct);

	//Wafermap "Type" & "SortBinItem" (For HPO)
	pGenDB->SetOthersItems("WaferMap Type",		szType);

	pGenDB->SetOthersItems("WaferMap SortBinItem", szSortBin);
	
	//Wafermap "LotNumber" (For FullSun)
	pGenDB->SetOthersItems("WaferMap LotNumber",	szMapLotNo);

	pGenDB->SetOthersItems("WaferMap Date",			szMapFileDate);

	pGenDB->SetOthersItems("WaferMap SerialNumber", szMapSerialNumber);

	//Binblk row & col orientation
	pGenDB->SetOthersItems("BT Orientation",	szBTOrientation);

	pGenDB->SetOthersItems("Prober Machine No", szProberMachineNo);

	pGenDB->SetOthersItems("Prober Date Time",	szProberDateTime);

	pGenDB->SetOthersItems("Bin Table Serial No", szBinTableSerialNo);

	pGenDB->SetOthersItems("MES Connection Path", szMESOutgoingPath);

	pGenDB->SetOthersItems("Internal Product No", szInternalProductNo);	

	pGenDB->SetOthersItems("Mode", szMode);								

	pGenDB->SetOthersItems("Specification", szSpecification);

	pGenDB->SetOthersItems("Map Other",		szMapOther);

	pGenDB->SetOthersItems("Sort Bin Item", szSortBinFilename);

	pGenDB->SetOthersItems("WaferLot Filename", szWaferLotFilename);

	pGenDB->SetOthersItems("WaferMap Bin Table", szMapBinTable);

	pGenDB->SetOthersItems("Duplicate File Retry Count", szDuplicateFileRetryCount);

	pGenDB->SetOthersItems("Retry Count", szRetryCount);

	pGenDB->SetOthersItems("WaferMap ESD Voltage", szESDVoltage);

	pGenDB->SetOthersItems("WaferMap TestTime", szTestTime);

	pGenDB->SetOthersItems("WaferMap WO", szWO);

	pGenDB->SetOthersItems("WaferMap SubstarteID", szSubstarteID);

	pGenDB->SetOthersItems("WaferMap CSTID",	szCSTID);

	pGenDB->SetOthersItems("WaferMap RecipeID",	szRecipeID);

	pGenDB->SetOthersItems("WaferMap RecipeName", szRecipeName);

	pGenDB->SetOthersItems("WaferMap Operator", szMapFileOperator);

	pGenDB->SetOthersItems("WaferMap Remark1",	szRemark1);

	pGenDB->SetOthersItems("WaferMap Remark2",	szRemark2);

	pGenDB->SetOthersItems("WaferMap PartNo",	szPartNo);

	pGenDB->SetOthersItems("Bin Barcode Min Digit", szBinBarcodeMinDigit);

	//Wafermap "DN" (Product Name) (for Sharp)
	pGenDB->SetOthersItems("DN",	szDN);

	//Wafermap "WN" (Product Name) (for Sharp)
	pGenDB->SetOthersItems("WN",	szWN);

	//Wafermap "CM1" (Comment1) (for Sharp)
	pGenDB->SetOthersItems("CM1",	szCM1);

	//Wafermap "CM2" (Comment2) (for Sharp)
	pGenDB->SetOthersItems("CM2",	szCM2);

	//Wafermap "CM3" (Comment3) (for Sharp)
	pGenDB->SetOthersItems("CM3",	szCM3);

	pGenDB->SetOthersItems("UN",	szUN);
	pGenDB->SetOthersItems("COND",	szCOND);
	pGenDB->SetOthersItems("LO",	szLO);
	pGenDB->SetOthersItems("UP",	szUP);
	pGenDB->SetOthersItems("FT",	szFT);
	pGenDB->SetOthersItems("LIM",	szLIM);
	pGenDB->SetOthersItems("COEF1", szCOEF1);
	pGenDB->SetOthersItems("COEF2", szCOEF2);
	pGenDB->SetOthersItems("COEF3", szCOEF3);

	pGenDB->SetOthersItems("WaferMap CenterPoint",	szWaferMapCenterPoint);

	pGenDB->SetOthersItems("WaferMap LifeTime",	szWaferMapLifeTime);
	pGenDB->SetOthersItems("WaferMap ESD",		szWaferMapESD);
	pGenDB->SetOthersItems("WaferMap PadColor",	szWaferMapPadColor);
	pGenDB->SetOthersItems("WaferMap BinCode",	szWaferMapBinCode);	
	pGenDB->SetOthersItems("Full Mag Slot",		szFullMagSlot);

	//v4.13T1	//DingCheng	-> APlusEpi
	CString szWafType = "", szMapBinName = "", szU1 = "", szU2 = "", szU3 = "", szResortDie = "";
	//SeoulSemi
	CString szHeaderProject;
	CString szHeaderProductCode;
	CString szRankName;
	CString szWaferLotFilePath;
	CString szHeaderDesign = "";

	if (pSMapFile != NULL)
	{
		szWafType		= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFTYPE];
		szMapBinName	= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINNAME];
		szU1			= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U1];
		szU2			= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U2];
		szU3			= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_U3];
		szRemark3		= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK3];

		szControlRank	= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CONTROL_RANK];
		szRankNo		= (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RANK_NO];
		szWaferLotFilePath = (*pSMapFile)["WaferTable"]["WaferLotInfoFilePath"];
		//
		szBMRadius		= (*pSMapFile)["BinTable"]["ClearBin"]["BMRadius"];
		szResortDie		= (*pSMapFile)[BT_TABLE_OPTION][BT_IS_RESORT_MODE];

		//SeoulSemi
		szHeaderProject = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["Project"];
		szHeaderProductCode	= (*pSMapFile)["MS896A"][WT_MAP_HEADER]["Product Code"];
		szRankName		= (*pSMapFile)["MS896A"][WT_MAP_HEADER]["Rank Name"][szBinBlkGrade];

		//4.53D5 Save Header
		szHeaderDesign  = (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DESIGN];
	}
	pGenDB->SetBinBlockInfoItems("Resort Die", szResortDie);

	pGenDB->SetOthersItems("Wafer Type",	szWafType);
	pGenDB->SetOthersItems("Map Bin Name",	szMapBinName);
	pGenDB->SetOthersItems("U1",			szU1);
	pGenDB->SetOthersItems("U2",			szU2);
	pGenDB->SetOthersItems("U3",			szU3);
	pGenDB->SetOthersItems("WaferLotInfoFilePath",	szWaferLotFilePath);

	pGenDB->SetOthersItems("WaferMap Control Rank",		szControlRank);
	pGenDB->SetOthersItems("WaferMap Rank No",			szRankNo);

	pGenDB->SetOthersItems("Remark3",			szRemark3);

	//Circle radius
	pGenDB->SetBinBlockInfoItems("BinMap Radius", szBMRadius);

	//PLLM Die Fab MES Lot
	pGenDB->SetOthersItems("PLLM Die Fab MES Lot",	szPLLMDieFabMESLot);
	CString szRecipeFilePath = (*pSMapFile)["MS896A"]["RecipeFilePath"];
	pGenDB->SetOthersItems("Recipe File Path",		szRecipeFilePath);	//v4.46T20	//PLSG

	pGenDB->SetOthersItems("PR Device Id", szPRDeviceID);

	//SeoulSemi (Save RankName in szBinBlkGrade)
	pGenDB->SetOthersItems("Project", szHeaderProject);
	pGenDB->SetOthersItems("Product Code", szHeaderProductCode);
	pGenDB->SetOthersItems("Rank Name", szRankName);

	pGenDB->SetOthersItems("Design Name", szHeaderDesign);


	// Extra Info for clear bin
	for (INT i = 0; i < BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		if (szaExtraBinInfoField[i] != "")
		{
			pGenDB->SetOthersItems(szaExtraBinInfoField[i], szaExtraBinInfo[i]);
		}
	}

	//v2.64 Lumileds DLA
	//Print label or just generate label file?
	CString szPrintLabel;
	if (CMS896AStn::m_bOfflinePrintLabel)		// Used in SETUP mode only
	{
		szPrintLabel = "FALSE";
	}
	else
	{
		szPrintLabel = "TRUE";
	}
	pGenDB->SetOthersItems("Print Label",	szPrintLabel);

	//SlotID (Use for DL machine)
	pGenDB->SetOthersItems("SlotID",					szSlotID);
	pGenDB->SetOthersItems("Label Printer Sel",			szLabelPrinterSel);
	pGenDB->SetOthersItems("Bin Output File Path2",		szOutputFiePath2);
	//v4.44T2	//Silan
	CString szPath3, szPath4;
	szPath3 = (*pSMapFile)["BinTable"]["Output File Path 3"];
	szPath4 = (*pSMapFile)["BinTable"]["Output File Path 4"];
	pGenDB->SetOthersItems("Bin Output File Path3",		szPath3);
	pGenDB->SetOthersItems("Bin Output File Path4",		szPath4);

	//AoYang Model
	CString szAoYangModel;
	if ((CMS896AStn::m_WaferMapWrapper.GetReader() != NULL) && 
		 CMS896AStn::m_WaferMapWrapper.IsMapValid())				//v4.44A1
	{
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("Model", szAoYangModel);
		pGenDB->SetOthersItems("AoYang Model", szAoYangModel);
	}

	//Nichia//v4.43T7
	BOOL bGenNichiaOutputFile	= (BOOL)(LONG)(*pSMapFile)["BinOutputFile"]["Enable Nichia OutputFile"];
	BOOL bGenNichiaOTriggerFile = (BOOL)(LONG)(*pSMapFile)["BinOutputFile"]["Enable Nichia OTriggerFile"];
	CString szGenNichiaOutputFile, szGenNichiaOTriggerFile;
	szGenNichiaOutputFile.Format("%d",			bGenNichiaOutputFile);
	pGenDB->SetOthersItems("Nichia Create Output File",		szGenNichiaOutputFile);
	szGenNichiaOTriggerFile.Format("%d",		bGenNichiaOTriggerFile);
	pGenDB->SetOthersItems("Nichia Create OTrigger File",	szGenNichiaOTriggerFile);
	if (pApp->GetCustomerName() == CTM_NICHIA && pApp->GetProductLine()=="MS90" && m_pBinTable != NULL)
	{
		CString szEnable;
		szEnable.Format("%d", m_pBinTable->m_bEnableBinOutputFile);
		pGenDB->SetOthersItems("Enable Output File",		szEnable);
		szEnable.Format("%d", m_pBinTable->m_bFinForBinOutputFile);
		pGenDB->SetOthersItems("Enable Output File Fin",	szEnable);
		szEnable.Format("%d", m_pBinTable->m_bEnableBinMapFile);
		pGenDB->SetOthersItems("Enable Bin Map File",		szEnable);
		szEnable.Format("%d", m_pBinTable->m_bFin2ForBinMapFile);
		pGenDB->SetOthersItems("Enable Bin Map File Fin 2",	szEnable);
	}

	//TESTER
	BOOL bMultiGradeSortingtoSingleBin = (BOOL)(LONG)(*pSMapFile)["Wafer Table Options"]["MultiGradeSortToSingleBin"];
	CString szMultiGradeSortingtoSingleBin;
	szMultiGradeSortingtoSingleBin.Format("%d",	bMultiGradeSortingtoSingleBin); 
	pGenDB->SetOthersItems("MultiGrade Sorting to Single Bin",	szMultiGradeSortingtoSingleBin);

	//OSRAM
	CString szEnableOsramBinMixMap;
	szEnableOsramBinMixMap.Format("%d",m_bEnableOsramBinMixMap);
	pGenDB->SetOthersItems("OSRAMDLLBinMixMap",	szEnableOsramBinMixMap);

	CString szBinGradeDL;
	szBinGradeDL = (*pSMapFile)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DELI_BINGRADE];
	pGenDB->SetOthersItems("BinGradeDL",	szBinGradeDL);
	CString szVF1;
	szVF1 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["VF1"];
	pGenDB->SetOthersItems("VF1DL",	szVF1);

	if (pApp->GetCustomerName() == "Solidlite")
	{
		CString szItem1,szItem2,szItem3;
		szItem1 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["SOLIDLITEPN"];
		szItem2 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["SOLIDLITELOT"];
		szItem3 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["SOLIDLITEQA"];
		pGenDB->SetOthersItems("SOLIDLITEPN",	szItem1);
		pGenDB->SetOthersItems("SOLIDLITELOT",	szItem2);
		pGenDB->SetOthersItems("SOLIDLITEQA",	szItem3);
	}

	if (pApp->GetCustomerName() == "ZhongWei")
	{
		CString szZWSpec = (*pSMapFile)["MS896A"]["ZWSpec"];
		pGenDB->SetOthersItems("ZWSpec",szZWSpec);
	}

	if (pApp->GetCustomerName() == "TianXin"  || pApp->GetCustomerName() == "BabyLighting")   // v4.51D3 TianXin
	{
		CString szTianXinItem1;
		CString szTianXinItem2;
		CString	szTianXinItem3;
		CString szTianXinItem4;
		CString szTianXinItem5;
		CString szTianXinItem6;
		CString szTianXinItem7;
		CString szTianXinItem8;
		CString szTianXinItem9;

		szTianXinItem1 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_PN"];
		szTianXinItem2 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_LOT"];
		szTianXinItem3 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_PCS"];

		szTianXinItem4 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_START_DATE"];			
		szTianXinItem5 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_SETUP_PACKAGE"];		
		szTianXinItem6 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_PR_PACKAGE"];			
		szTianXinItem7 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_TESTER_COND_FILE"];	
		szTianXinItem8 = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["TIANXIN_SOURCE"];
		szTianXinItem9 = (*pSMapFile)["MS896A"]["MapFilePath"];

		pGenDB->SetOthersItems("TIANXIN_PN",					szTianXinItem1);
		pGenDB->SetOthersItems("TIANXIN_LOT",					szTianXinItem2);
		pGenDB->SetOthersItems("TIANXIN_PCS",					szTianXinItem3);
		pGenDB->SetOthersItems("TIANXIN_START_DATE",			szTianXinItem4);
		pGenDB->SetOthersItems("TIANXIN_SETUP_PACKAGE",			szTianXinItem5);
		pGenDB->SetOthersItems("TIANXIN_PR_PACKAGE",			szTianXinItem6);
		pGenDB->SetOthersItems("TIANXIN_TESTER_COND_FILE",		szTianXinItem7);
		pGenDB->SetOthersItems("TIANXIN_SOURCE",				szTianXinItem8);
		pGenDB->SetOthersItems("MapFilePath",					szTianXinItem9);

	}

	CString szWaferMapFilePath;
	szWaferMapFilePath = (*pSMapFile)["MS896A"]["MapFilePath"];
	pGenDB->SetOthersItems("MapFilePath",					szWaferMapFilePath);  // 4.51D13

	if (pApp->GetCustomerName() == "FiberOptics")    // v4.51D10 // Dicon
	{
		CString szSortBin;
		CString szOutputFileBlueTapeNo;
		szSortBin			   = (*pSMapFile)["MS896A"][WT_MAP_HEADER]["SORT_BIN"];
		szOutputFileBlueTapeNo = (*pSMapFile)["MS896A"]["OutputFileBlueTapeNo"];
		pGenDB->SetOthersItems("Sort Bin",					szSortBin);
		pGenDB->SetOthersItems("OutputFile BlueTape No",	szOutputFileBlueTapeNo);
	}

	CString szMinGradeBin =(*pSMapFile)["Wafer Table Options"]["Min Grade Bin"];
	pGenDB->SetOthersItems("Min Grade Bin",	szMinGradeBin);


	long lMapRotation		= (*pSMapFile)["MS896A"]["Map Rotation"];  // 4.50D10
	long lAutoWaferRotation = (*pSMapFile)["MS896A"]["Table Rotation"];	
	bool  bMapVertFlip		= (*pSMapFile)["MS896A"]["Flip Vertical"];
	bool  bMapHoriFlip		= (*pSMapFile)["MS896A"]["Flip Horizontal"];	
	CString szMapRotation, szAutoWaferRotation, szMapVertFlip, szMapHoriFlip;

	szMapRotation.Format("%d", lMapRotation);
	szAutoWaferRotation.Format("%d", lAutoWaferRotation);
	szMapVertFlip.Format("%d", bMapVertFlip);
	szMapHoriFlip.Format("%d", bMapHoriFlip);

	// Rotation and Mirror
	pGenDB ->SetOthersItems("Map Rotation", szMapRotation  ); 
	pGenDB ->SetOthersItems("Table Rotation", szAutoWaferRotation);
	pGenDB ->SetOthersItems("Flip Vertical", szMapVertFlip);
	pGenDB ->SetOthersItems("Flip Horizontal", szMapHoriFlip  );

	CString szCheckOutputFileRepeat;
	BOOL bCheckOutputFileRepeat;
	bCheckOutputFileRepeat = (BOOL)(LONG)(*pSMapFile)["BinTable"]["Check OutputFile Repeat"]; // SanAn 4.51D6
	szCheckOutputFileRepeat.Format("%d", bCheckOutputFileRepeat);
	//AfxMessageBox("SaveClear -- szOutputFileRepeat: " + szOutputFileRepeat);
	pGenDB ->SetOthersItems("OutputFile Repeat", szCheckOutputFileRepeat  );


	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Save Data to geneDB complete");
	return TRUE;
}


BOOL CBinBlkMain::SaveClearBinCountDataForOutputFile(ULONG ulBlkId, UCHAR ucGrade, BOOL bIfGenOutputFile, BOOL bUseOutputBarcode,
		CString szMachineNo, CString szLotNo, CString szBinLotDirectory, CString szOutputFileSummaryPath,
		CString szFileNo, CString szBlueTapeNo, 
		CString szMapSpecVersion, CString szBinParameter, 
		CString szOutputFileNamingFormat,
		CString szOutputFilename, CString szUnloadDate, 
		CString szUnloadTime, CString szOutputBarcode,
		CString szLabelFilePath, 
		CString szWorkNo, CString szPrefix, CString szLoadPKGCount,
		CString szType, CString szSortBin, CString szDevice, 
		CString szWaferBin, CString szModelNo, CString szLotID,
		CString szProduct, CString szBTSpecVersion, CString szMachineType,
		CString szSlotID, CString szMapLotNo, CString szOperatorId, ULONG ulMode,
		CString szaExtraBinInfoField[], CString szaExtraBinInfo[], 
		CString szSoftwareVersion, CString szPKGFilename, CString szProberMachineNo, 
		CString szProberDateTime, CString szBinTableSerialNo, CString szMESOutgoingPath,
		CString szInternalProductNo, CString szMode, CString szSpecification, CString szMapOther,
		CString szSortBinFilename, CString szWaferLotFilename, CString szMapBinTable, 
		CString szDuplicateFileRetryCount, CString szMapESDVoltage, CString szTestTime, CString szWO, CString szSubstarteID, CString szCSTID,
		CString szRecipeID, CString szRecipeName, CString szMapFileOperator, 
		CString szMapFileDate, CString szRemark1, CString szRemark2, CString szPartNo, 
		CString szBinDieType, LONG lBinBarcodeMinDigit,
		CString szDN, CString szWN, CString szCM1, CString szCM2, CString szCM3,
		CString szUN, CString szCOND, CString szLO, CString szUP, CString szFT, CString szLIM,
		CString szCOEF1, CString szCOEF2, CString szCOEF3, CString szPLLMDieFabMESLot, CString szFullMagSlot, 
		CString szLabelPrinterSel, CString szMapSerialNumber, CString szRetryCount, CString szWaferMapCenterPoint,
		CString szWaferMapLifeTime, CString szWaferMapESD, CString szWaferMapPadColor, CString szWaferMapBinCode, CString szMachineModel,
		CString szPRDeviceID, CString szUseBarcode,
		CString szOutputFiePath2, BOOL bForSummary,
		CStringMapFile *pSMapFile)
{
	CString szBTOrientation;

	if (GrabLSBondPattern() == TRUE)
	{
		szBTOrientation = "LS_BOND";
	}
	else
	{
		szBTOrientation = "NORMAL";
	}

	if (ulMode == CLEAR_BIN_BY_PHY_BLK)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[i].GetPhyBlkId() == ulBlkId)
			{
				SaveClearBinCountDataForOutputFileByBlkId(i, bIfGenOutputFile, bUseOutputBarcode,
						szMachineNo, szLotNo, szBinLotDirectory, szOutputFileSummaryPath , szFileNo, szBlueTapeNo, szMapSpecVersion, szBinParameter, 
						szOutputFileNamingFormat, szOutputFilename, szUnloadDate, szUnloadTime, szOutputBarcode, szLabelFilePath, 
						szWorkNo, szPrefix, szLoadPKGCount, szType, szSortBin, szDevice, szWaferBin, szModelNo, szLotID,
						szProduct, szBTSpecVersion, szMachineType, szSlotID, szMapLotNo, szOperatorId, szBTOrientation, 
						szaExtraBinInfoField, szaExtraBinInfo, szSoftwareVersion, szPKGFilename, szProberMachineNo, szProberDateTime,
						szBinTableSerialNo, szMESOutgoingPath, szInternalProductNo, szMode, szSpecification, szMapOther,
						szSortBinFilename, szWaferLotFilename, szMapBinTable, szDuplicateFileRetryCount, szMapESDVoltage,
						szTestTime, szWO, szSubstarteID, szCSTID, szRecipeID, szRecipeName, szMapFileOperator, 
						szMapFileDate, szRemark1, szRemark2, szPartNo, szBinDieType, lBinBarcodeMinDigit,
						szDN, szWN, szCM1, szCM2, szCM3, szUN, szCOND, szLO, szUP, szFT, szLIM, szCOEF1, szCOEF2, szCOEF3,
						szPLLMDieFabMESLot, szFullMagSlot, szLabelPrinterSel, szMapSerialNumber, szRetryCount, szWaferMapCenterPoint,
						szWaferMapLifeTime, szWaferMapESD, szWaferMapPadColor, szWaferMapBinCode, szMachineModel, szPRDeviceID, szUseBarcode,
						szOutputFiePath2, bForSummary, pSMapFile);		//v4.13T1
			}
		}
	}
	else if (ulMode == CLEAR_BIN_BY_GRADE)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if ((m_oBinBlk[i].GetGrade() == ucGrade) && (m_oBinBlk[i].GetNVNoOfBondedDie() > 0))
			{
				SaveClearBinCountDataForOutputFileByBlkId(i, bIfGenOutputFile, bUseOutputBarcode,
						szMachineNo, szLotNo, szBinLotDirectory, szOutputFileSummaryPath , szFileNo, szBlueTapeNo, szMapSpecVersion, szBinParameter, 
						szOutputFileNamingFormat, szOutputFilename, szUnloadDate, szUnloadTime, szOutputBarcode, szLabelFilePath, 
						szWorkNo, szPrefix, szLoadPKGCount, szType, szSortBin, szDevice, szWaferBin, szModelNo, szLotID,
						szProduct, szBTSpecVersion, szMachineType, szSlotID, szMapLotNo, szOperatorId, szBTOrientation, 
						szaExtraBinInfoField, szaExtraBinInfo, szSoftwareVersion, szPKGFilename, szProberMachineNo, szProberDateTime,
						szBinTableSerialNo, szMESOutgoingPath, szInternalProductNo, szMode, szSpecification, szMapOther,
						szSortBinFilename, szWaferLotFilename, szMapBinTable, szDuplicateFileRetryCount, szMapESDVoltage,
						szTestTime, szWO, szSubstarteID, szCSTID, szRecipeID, szRecipeName, szMapFileOperator, 
						szMapFileDate, szRemark1, szRemark2, szPartNo, szBinDieType, lBinBarcodeMinDigit,
						szDN, szWN, szCM1, szCM2, szCM3, szUN, szCOND, szLO, szUP, szFT, szLIM, szCOEF1, szCOEF2, szCOEF3,
						szPLLMDieFabMESLot, szFullMagSlot, szLabelPrinterSel, szMapSerialNumber, szRetryCount, szWaferMapCenterPoint,
						szWaferMapLifeTime, szWaferMapESD, szWaferMapPadColor, szWaferMapBinCode, szMachineModel, szPRDeviceID, szUseBarcode, 
						szOutputFiePath2, bForSummary, pSMapFile);		//v4.13T1
			}
		}

	}
	else
	{
		SaveClearBinCountDataForOutputFileByBlkId(ulBlkId, bIfGenOutputFile, bUseOutputBarcode,
				szMachineNo, szLotNo, szBinLotDirectory, szOutputFileSummaryPath , szFileNo, szBlueTapeNo, szMapSpecVersion, szBinParameter, 
				szOutputFileNamingFormat, szOutputFilename, szUnloadDate, szUnloadTime, szOutputBarcode, szLabelFilePath, 
				szWorkNo, szPrefix, szLoadPKGCount, szType, szSortBin, szDevice, szWaferBin, szModelNo, szLotID,
				szProduct, szBTSpecVersion, szMachineType, szSlotID, szMapLotNo, szOperatorId, szBTOrientation, 
				szaExtraBinInfoField, szaExtraBinInfo, szSoftwareVersion, szPKGFilename, szProberMachineNo, szProberDateTime,
				szBinTableSerialNo, szMESOutgoingPath, szInternalProductNo, szMode, szSpecification, szMapOther,
				szSortBinFilename, szWaferLotFilename, szMapBinTable, szDuplicateFileRetryCount, szMapESDVoltage,
				szTestTime, szWO, szSubstarteID, szCSTID, szRecipeID, szRecipeName, szMapFileOperator, 
				szMapFileDate, szRemark1, szRemark2, szPartNo, szBinDieType, lBinBarcodeMinDigit,
				szDN, szWN, szCM1, szCM2, szCM3, szUN, szCOND, szLO, szUP, szFT, szLIM, szCOEF1, szCOEF2, szCOEF3,
				szPLLMDieFabMESLot, szFullMagSlot, szLabelPrinterSel, szMapSerialNumber, szRetryCount, szWaferMapCenterPoint,
				szWaferMapLifeTime, szWaferMapESD, szWaferMapPadColor, szWaferMapBinCode, szMachineModel, szPRDeviceID, szUseBarcode, 
				szOutputFiePath2, bForSummary, pSMapFile);		//v4.13T1
	}

	return TRUE;
}


BOOL CBinBlkMain::BackupClearBinFilesByBlkId(ULONG ulBlkId, BOOL nEnable, CString szTimeStamp, 
											 BOOL bJustTempFile)
{
	CString szBlkId, szTargetFolder;
	CString szTempFilename, szDataFilename;
	CString szHeaderFileName, szFormatFileName;

	szBlkId.Format("%d", ulBlkId); 
	if (ulBlkId < 10)
	{
		szBlkId = "0" + szBlkId;
	}

	szHeaderFileName = gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt";
	szFormatFileName = gszROOT_DIRECTORY + "\\Exe\\OutputFileFormat.txt";

	szTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkId + "TempFile.csv";
	szDataFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkId + "Database.txt";

	szTargetFolder = gszUSER_DIRECTORY + "\\OutputFile\\ClearBin\\" + "LOG_" + szTimeStamp;
	//v4.48A10	//WH Sanan
	if ( (m_szBackupTempFilePath.GetLength() > 0) && (_access(m_szBackupTempFilePath, 0) != -1) )
	{
		szTargetFolder = m_szBackupTempFilePath + "\\LOG_" + szTimeStamp;
	}

	if (nEnable == TRUE)
	{
		CreateDirectory(szTargetFolder, NULL);

		if (bJustTempFile == TRUE)
		{
			CopyFile(szTempFilename, szTargetFolder + "\\Blk" + szBlkId + "TempFile.csv", FALSE);
		}
		else
		{
			CopyFile(szHeaderFileName, szTargetFolder + "\\WaferMapColumns.txt", FALSE);
			CopyFile(szFormatFileName, szTargetFolder + "\\OutputFileFormat.txt", FALSE);
			CopyFile(szTempFilename, szTargetFolder + "\\Blk" + szBlkId + "TempFile.csv", FALSE);
			CopyFile(szDataFilename, szTargetFolder + "\\Blk" + szBlkId + "Database.txt", FALSE);
		}
	}
	else
	{
		if (bJustTempFile == TRUE)
		{
			DeleteFile(szTargetFolder + "\\Blk" + szBlkId + "TempFile.csv");
			RemoveDirectory(szTargetFolder);
		}
		else
		{
			DeleteFile(szTargetFolder + "\\WaferMapColumns.txt");
			DeleteFile(szTargetFolder + "\\OutputFileFormat.txt");

			DeleteFile(szTargetFolder + "\\Blk" + szBlkId + "TempFile.csv");
			DeleteFile(szTargetFolder + "\\Blk" + szBlkId + "Database.txt");

			RemoveDirectory(szTargetFolder);
		}
	}

	return TRUE;

}


BOOL CBinBlkMain::BackupClearBinFiles(ULONG ulBlkId, UCHAR ucGrade, BOOL nEnable, 
										CString szTimeStamp, ULONG ulMode)
{
	//v4.48A10	//v4.48A29
//	SanAn TJ 2017
	CString szBackupFolder = gszUSER_DIRECTORY + "\\OutputFile\\ClearBin";
	if ( (m_szBackupTempFilePath.GetLength() > 0) && (_access(m_szBackupTempFilePath, 0) != -1) )
	{
		szBackupFolder = m_szBackupTempFilePath;
	}
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(szBackupFolder, 30);

	if (ulMode == CLEAR_BIN_BY_PHY_BLK)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[i].GetPhyBlkId() == ulBlkId)
			{
				BackupClearBinFilesByBlkId(i, nEnable, szTimeStamp, FALSE);
			}
		}

	}
	else if (ulMode == CLEAR_BIN_BY_GRADE)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if ((m_oBinBlk[i].GetGrade() == ucGrade))
			{
				BackupClearBinFilesByBlkId(i, nEnable, szTimeStamp, FALSE);
			}
		}
	}
	else
	{
		BackupClearBinFilesByBlkId(ulBlkId, nEnable, szTimeStamp, FALSE);
	}

	return TRUE;
}


/***************************************/
/*    Clear Bin Counter (by Grade)     */
/***************************************/

BOOL CBinBlkMain::ClearBinCounterByGrade(UCHAR ucGradeToClear, CDWordArray &dwaBinBlkIds)
{
	ULONG i;
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	for (i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetGrade() == ucGradeToClear)
		{
			if (m_bStoreBinBondedCount == TRUE)
			{
				ULONG ulAccCount = m_oBinBlk[i].GetNVNoOfBondedDie() + m_oBinBlk[i].GetAccmulatedCount();
				SetBinAccBondedCount(i, ulAccCount);
			}

			dwaBinBlkIds.Add(i);

			//CurrBondIndex (CBinBlk)
			m_oBinBlk[i].SetNVCurrBondIndex(0);

			//NoOfBondedDie (CBinBlk)
			m_oBinBlk[i].SetNVNoOfBondedDie(0);

			//IsFull (CBinBlk)
			m_oBinBlk[i].SetNVIsFull(FALSE);

			//Status (CBinBlk)
			m_oBinBlk[i].SetStatus(BT_BLK_NORMAL);

			//Empty unit (CBinBlk)
			m_oBinBlk[i].SetEmptyUnit(0);

			//LastFileSaveIndex (CBinBlk)
			m_oBinBlk[i].SetNVLastFileSaveIndex(0);

			//Remove temp files and reset LastFileSaveIndex
			RemoveTempFileAndResetIndex(0, m_oBinBlk[i].GetGrade(), CLEAR_BIN_BY_GRADE);

			//Update SMF file
			if (pBTfile != NULL)
			{
				(*pBTfile)["BinBlock"][i]["Status"]		= m_oBinBlk[i].GetStatus();
				(*pBTfile)["BinBlock"][i]["EmptyUnit"]	= m_oBinBlk[i].GetEmptyUnit();
			}
		}
	}

	//NoOfSortedDie (CBinGradeData)
	m_oBinGradeData[ucGradeToClear].SetNVNoOfSortedDie(0);

	//IsFull(CBinGradeData)
	m_oBinGradeData[ucGradeToClear].SetNVIsFull(FALSE);

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
} //end ClearBinCounterByGrade


VOID CBinBlkMain::ClearBinCounter(ULONG ulBlkId, BOOL bUpdateMSD)
{
	UCHAR ucBlkGrade;

	ucBlkGrade = GrabGrade(ulBlkId);
	ucBlkGrade = min(ucBlkGrade, BT_MAX_BINBLK_SIZE - 1);		//Klocwork	//v4.02T5

	//CBinGradeData: NoOfSortedDie
	ULONG ulBondedNo;
	ULONG ulSortedNo;

	ulBondedNo = m_oBinBlk[ulBlkId].GetNVNoOfBondedDie();
	ulSortedNo = m_oBinGradeData[ucBlkGrade].GetNVNoOfSortedDie();

	//2018.5.15 if clear bin counter, it should set the number of the sorted die to 0 becasuse what is bond die ?
//	m_oBinGradeData[ucBlkGrade].SetNVNoOfSortedDie(m_oBinGradeData[ucBlkGrade].GetNVNoOfSortedDie() - m_oBinBlk[ulBlkId].GetNVNoOfBondedDie());
	m_oBinGradeData[ucBlkGrade].SetNVNoOfSortedDie(0);

	//CBinGradeData: IsFull
	m_oBinGradeData[ucBlkGrade].SetNVIsFull(FALSE);

	//CBinBlk: CurrBondIndex
	m_oBinBlk[ulBlkId].SetNVCurrBondIndex(0);

	//CBinBlk: NoOfBondedDie
	m_oBinBlk[ulBlkId].SetNVNoOfBondedDie(0);

	//CBinBlk: IsFull
	m_oBinBlk[ulBlkId].SetNVIsFull(FALSE);

	//CBinBlk: Status
	m_oBinBlk[ulBlkId].SetStatus(BT_BLK_NORMAL);

	//CBinBlk: Empty unit
	m_oBinBlk[ulBlkId].SetEmptyUnit(0);

	//CBinBlk: LastFileSaveIndex
	m_oBinBlk[ulBlkId].SetNVLastFileSaveIndex(0);

	//CBinBlk: IsAligned
	m_oBinBlk[ulBlkId].SetNVIsAligned(FALSE);

	//CBinBlk: XOffset
	m_oBinBlk[ulBlkId].SetNVXOffset(0);

	//CBinBlk: YOffset
	m_oBinBlk[ulBlkId].SetNVYOffset(0);

	//CBinBlk: RotateAngleX
	m_oBinBlk[ulBlkId].SetNVRotateAngleX(0.0);

	//2018.10.23 remove the SetFrameRotationOnly becasue rotation was only controlled by bin table rotation
//	if (CMS896AApp::m_bEnableSubBin == TRUE)
//	{
//		m_oBinBlk[ulBlkId].SetFrameRotationOnly(FALSE);
//	}
	//Remove temp files and reset LastFileSaveIndex
	RemoveTempFileAndResetIndex(ulBlkId, '0', CLEAR_BIN_BY_PHY_BLK);	//v4.52A8

	//Update SMF file
	if (bUpdateMSD)		//v4.52A8	//enhanced/speedup version to reduce file access
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		if (pBTfile != NULL)
		{
			(*pBTfile)["BinBlock"][ulBlkId]["Status"]	= m_oBinBlk[ulBlkId].GetStatus();
			(*pBTfile)["BinBlock"][ulBlkId]["EmptyUnit"] = m_oBinBlk[ulBlkId].GetEmptyUnit();
			CMSFileUtility::Instance()->SaveBTConfig();
		}
	}

} //end ClearBinCounter

BOOL CBinBlkMain::IsBondingInProgress()
{
	BOOL bInProgress = FALSE;

	for (ULONG i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (GrabNVNoOfBondedDie(i) > 0)
		{
			bInProgress = TRUE;
			break;
		}
	}

	return bInProgress;
}


VOID CBinBlkMain::ClearAllGradeAndBinCounter()
{
	UCHAR i;

	for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		m_oBinGradeData[i].SetNVNoOfSortedDie(0);
		m_oBinBlk[i].SetNVNoOfBondedDie(0);
	}

} //end ClearAllGradeCounter


/*******************************/
/*    Clear all bin counter    */
/*******************************/

BOOL CBinBlkMain::UpdateBlkInUse(ULONG ulCurrBlkId)
{
	UCHAR ucCurrBlkGrade;

	ucCurrBlkGrade = m_oBinBlk[ulCurrBlkId].GetGrade();

	ucCurrBlkGrade = min(ucCurrBlkGrade, BT_MAX_BINBLK_SIZE - 1);		//Klocwork	//v4.02T5

	if (m_oBinGradeData[ucCurrBlkGrade].GetNVBlkInUse() > ulCurrBlkId)
	{
		m_oBinGradeData[ucCurrBlkGrade].SetNVBlkInUse(ulCurrBlkId);
	}

	return TRUE;
} //end UpdateBlkInUse


VOID CBinBlkMain::UpdateSerialNoGivenBinBlkId(ULONG ulBinBlkId, CStringMapFile *pBTMSDfile)
{
	int nCurrYear, nCurrMonth, nCurrDay;
	CString szCurrDate, szMsg;
	CString szLogMsg;
	CTime theTime;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	theTime = CTime::GetCurrentTime(); 

	nCurrYear = theTime.GetYear();
	nCurrMonth = theTime.GetMonth();
	nCurrDay = theTime.GetDay();

	szCurrDate.Format("%d-%d-%d", nCurrYear, nCurrMonth, nCurrDay);

	CStringMapFile *pBTfile = pBTMSDfile;
	if (pBTMSDfile == NULL)
	{
		pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	}

	if (GrabSerialNoFormat() == BT_SNR_RESET_DAILY) //Reset everyday
	{
		if (szCurrDate != m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate())
		{
			szLogMsg.Format("Reset %d SRN by date %s %s", ulBinBlkId, szCurrDate, m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate());
			CMSLogFileUtility::Instance()->BT_ClearBinLog(szLogMsg);

			//Update the date
			m_oBinBlk[ulBinBlkId].SetLastResetSerialNoDate(szCurrDate);
			if (pBTfile != NULL)	//v4.52A8
			{
				(*pBTfile)["BinBlock"][ulBinBlkId]["LastResetSerialNoDate"] = 
					m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate();
			}

			//Reset counter & +1
			m_oBinBlk[ulBinBlkId].SetSerialNo(1);
			if (pBTfile != NULL)	//v4.52A8
			{
				(*pBTfile)["BinBlock"][ulBinBlkId]["SerialNo"] = m_oBinBlk[ulBinBlkId].GetSerialNo();
			}
		}
		else
		{
			//Increment counter
			m_oBinBlk[ulBinBlkId].SetSerialNo(m_oBinBlk[ulBinBlkId].GetSerialNo() + 1);
			if (pBTfile != NULL)	//v4.52A8
			{
				(*pBTfile)["BinBlock"][ulBinBlkId]["SerialNo"] = m_oBinBlk[ulBinBlkId].GetSerialNo();
			}

			szLogMsg.Format("Update %d SRN by date %d", ulBinBlkId, m_oBinBlk[ulBinBlkId].GetSerialNo());
			CMSLogFileUtility::Instance()->BT_ClearBinLog(szLogMsg);
		}
	}
	else if (GrabSerialNoFormat() == BT_SNR_RESET_HIT_LIMIT) //Reset when hit 9999
	{
		if (m_oBinBlk[ulBinBlkId].GetSerialNo() >= GrabSerialNoMaxLimit())
		{
			//Update the date
			m_oBinBlk[ulBinBlkId].SetLastResetSerialNoDate(szCurrDate);
			if (pBTfile != NULL)	//v4.52A8
			{
				(*pBTfile)["BinBlock"][ulBinBlkId]["LastResetSerialNoDate"] = 
					m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate();
			}

			//Reset counter & +1
			m_oBinBlk[ulBinBlkId].SetSerialNo(1);
			if (pBTfile != NULL)	//v4.52A8
			{
				(*pBTfile)["BinBlock"][ulBinBlkId]["SerialNo"] = m_oBinBlk[ulBinBlkId].GetSerialNo();
			}

			szLogMsg.Format("Reset %d SRN by %d", ulBinBlkId, GrabSerialNoMaxLimit());	//v4.53A29
			CMSLogFileUtility::Instance()->BT_ClearBinLog(szLogMsg);
		}
		else
		{
			//Update the date
			m_oBinBlk[ulBinBlkId].SetLastResetSerialNoDate(szCurrDate);
			if (pBTfile != NULL)	//v4.52A8
			{
				(*pBTfile)["BinBlock"][ulBinBlkId]["LastResetSerialNoDate"] = 
					m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate();
			}

			//Increment counter
			m_oBinBlk[ulBinBlkId].SetSerialNo(m_oBinBlk[ulBinBlkId].GetSerialNo() + 1);
			if (pBTfile != NULL)	//v4.52A8
			{
				(*pBTfile)["BinBlock"][ulBinBlkId]["SerialNo"] = m_oBinBlk[ulBinBlkId].GetSerialNo();
			}

			szLogMsg.Format("Update %d SRN by 9999 %d", ulBinBlkId, m_oBinBlk[ulBinBlkId].GetSerialNo());
			CMSLogFileUtility::Instance()->BT_ClearBinLog(szLogMsg);
		}
	}
	else if (GrabSerialNoFormat() == BT_SNR_RESET_CHANGE_LOTDIR) //Reset by input new Lot directory
	{
		//Update the date
		m_oBinBlk[ulBinBlkId].SetLastResetSerialNoDate(szCurrDate);
		if (pBTfile != NULL)	//v4.52A8
		{
			(*pBTfile)["BinBlock"][ulBinBlkId]["LastResetSerialNoDate"] = 
				m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate();
		}

		//Increment counter
		m_oBinBlk[ulBinBlkId].SetSerialNo(m_oBinBlk[ulBinBlkId].GetSerialNo() + 1);
		if (pBTfile != NULL)	//v4.52A8
		{
			(*pBTfile)["BinBlock"][ulBinBlkId]["SerialNo"] = m_oBinBlk[ulBinBlkId].GetSerialNo();
		}

		szLogMsg.Format("Update %d SRN by NewLot %d", ulBinBlkId, m_oBinBlk[ulBinBlkId].GetSerialNo());
		CMSLogFileUtility::Instance()->BT_ClearBinLog(szLogMsg);
	}
	else if (GrabSerialNoFormat() == BT_SNR_RESET_LOAD_BIN_SUMMARY) //Reset by input new Lot directory
	{
		//Update the date
		m_oBinBlk[ulBinBlkId].SetLastResetSerialNoDate(szCurrDate);
		if (pBTfile != NULL)	//v4.52A8
		{
			(*pBTfile)["BinBlock"][ulBinBlkId]["LastResetSerialNoDate"] = 
				m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate();
		}

		//Increment counter
		m_oBinBlk[ulBinBlkId].SetSerialNo(m_oBinBlk[ulBinBlkId].GetSerialNo() + 1);
		if (pBTfile != NULL)	//v4.52A8
		{
			(*pBTfile)["BinBlock"][ulBinBlkId]["SerialNo"] = m_oBinBlk[ulBinBlkId].GetSerialNo();
		}

		szLogMsg.Format("Update %d SRN by Load Summary File Option %d", ulBinBlkId, m_oBinBlk[ulBinBlkId].GetSerialNo());
		CMSLogFileUtility::Instance()->BT_ClearBinLog(szLogMsg);
	}

	if (pBTMSDfile == NULL)	//v4.52A8
	{
		CMSFileUtility::Instance()->SaveBTConfig();	
	}

} //end UpdateSerialNoGivenBinBlkId


VOID CBinBlkMain::UpdateSerialNo(ULONG ulPhyBlkId, UCHAR ucGrade, ULONG ulMode,
								 BOOL bUseMultiSerialCounter, CONST BOOL bUseUSerialNo)
{
	ULONG i=0;
	
	if (bUseMultiSerialCounter == TRUE)
	{
		UpdateMultipleBinSerialNo(CMS896AStn::m_lOTraceabilityFormat - 1);
		return;
	}
	
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();	//v4.52A8
	if (ulMode == CLEAR_BIN_BY_PHY_BLK)
	{
		for (i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			//v3.23T1
			if (bUseUSerialNo)	//if use unique serial num update all blocks every time
			{
				UpdateSerialNoGivenBinBlkId(i, pBTfile);

			}
			else if (m_oBinBlk[i].GetPhyBlkId() == ulPhyBlkId)
			{
				UpdateSerialNoGivenBinBlkId(i, pBTfile);
			}
		}
	}
	else if (ulMode == CLEAR_BIN_BY_GRADE)
	{
		for (i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			//v3.23T1
			if (bUseUSerialNo)	//if use unique serial num update all blocks every time
			{
				UpdateSerialNoGivenBinBlkId(i, pBTfile);
			}
			else if ((m_oBinBlk[i].GetGrade() == ucGrade) && (m_oBinBlk[i].GetNVNoOfBondedDie() > 0))
			{
				UpdateSerialNoGivenBinBlkId(i, pBTfile);
				
			}
		}
	}
	else
	{
		UpdateSerialNoGivenBinBlkId(ulPhyBlkId, pBTfile);
	}

	CMSFileUtility::Instance()->SaveBTConfig();		//v4.52A8

} //end UpdateSerialNoGivenPhyBlkId


VOID CBinBlkMain::ResetSerialNo(ULONG ulBinBlkId, CStringMapFile *pBTMSDfile)
{
	int nCurrYear, nCurrMonth, nCurrDay;
	CString szCurrDate;
	CTime theTime;
	
	CStringMapFile *pBTfile = pBTMSDfile;
	if (pBTMSDfile == NULL)
	{
		pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	}

	theTime = CTime::GetCurrentTime(); 

	nCurrYear = theTime.GetYear();
	nCurrMonth = theTime.GetMonth();
	nCurrDay = theTime.GetDay();

	szCurrDate.Format("%d-%d-%d", nCurrYear, nCurrMonth, nCurrDay);

	//Update the date
	m_oBinBlk[ulBinBlkId].SetLastResetSerialNoDate(szCurrDate);

	//Check Load/Save Data
	if (pBTfile != NULL)
	{
		(*pBTfile)["BinBlock"][ulBinBlkId]["LastResetSerialNoDate"] = 
			m_oBinBlk[ulBinBlkId].GetLastResetSerialNoDate();
	}

	//Reset counter
	m_oBinBlk[ulBinBlkId].SetSerialNo(0);
	
	//Check Load/Save Data
	if (pBTfile != NULL)
	{
		(*pBTfile)["BinBlock"][ulBinBlkId]["SerialNo"] = 
			m_oBinBlk[ulBinBlkId].GetSerialNo();
	}

	if (pBTMSDfile == NULL)
	{
		CMSFileUtility::Instance()->SaveBTConfig();	//v4.52A8
	}

} //end ResetSerialNo


//----------------------------------------------//
// 		Functions for multiple bin serial No	//
//		for differenet output file format		//
//---------------------------------------------//


VOID CBinBlkMain::UpdateMultipleBinSerialNo(LONG lIndex)
{
	int nCurrYear, nCurrMonth, nCurrDay;
	CString szCurrDate, szMsg;
	CString szLogMsg;
	CTime theTime;
	theTime = CTime::GetCurrentTime(); 
	nCurrYear = theTime.GetYear();
	nCurrMonth = theTime.GetMonth();
	nCurrDay = theTime.GetDay();
	szCurrDate.Format("%d-%d-%d", nCurrYear, nCurrMonth, nCurrDay);

	LONG lPrevSerialNo;
	CString szPrevResetSerialDate;

	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *pBinSerial;

	// open config file
	if (pUtl->LoadMultipleBinSerialFile() == FALSE)
	{
		return;
	}

	// get file pointer
	pBinSerial = pUtl->GetMultipleBinSerialFile();

	if (pBinSerial == NULL)
	{
		return;
	}

	szPrevResetSerialDate = (*pBinSerial)["Bin Serial"][lIndex]["LastResetSerialNoDate"];
	lPrevSerialNo = (*pBinSerial)["Bin Serial"][lIndex]["SerialNo"];

	if (GrabSerialNoFormat() == BT_SNR_RESET_DAILY) //Reset everyday
	{
		if (szCurrDate != szPrevResetSerialDate)
		{
			// Reset the Serial
			(*pBinSerial)["Bin Serial"][lIndex]["LastResetSerialNoDate"] = szCurrDate;
			(*pBinSerial)["Bin Serial"][lIndex]["SerialNo"] = 1;
		}
		else
		{
			//Increment counter
			(*pBinSerial)["Bin Serial"][lIndex]["SerialNo"] = lPrevSerialNo + 1;
		}
	}
	else if (GrabSerialNoFormat() == BT_SNR_RESET_HIT_LIMIT) //Reset when hit 9999
	{
		if (lPrevSerialNo == GrabSerialNoMaxLimit())
		{
			// Reset the Serial
			(*pBinSerial)["Bin Serial"][lIndex]["LastResetSerialNoDate"] = szCurrDate;
			(*pBinSerial)["Bin Serial"][lIndex]["SerialNo"] = 1;
		}
		else
		{
			(*pBinSerial)["Bin Serial"][lIndex]["SerialNo"] = lPrevSerialNo + 1;
		}
	}
	else if (GrabSerialNoFormat() == BT_SNR_RESET_CHANGE_LOTDIR) //Reset by input new Lot directory
	{
		(*pBinSerial)["Bin Serial"][lIndex]["SerialNo"] = lPrevSerialNo + 1;
	}
	else if (GrabSerialNoFormat() == BT_SNR_RESET_LOAD_BIN_SUMMARY)
	{
		(*pBinSerial)["Bin Serial"][lIndex]["SerialNo"] = lPrevSerialNo + 1;
	}

	pUtl->UpdateMultipleBinSerialFile();
	pUtl->CloseMultipleBinSerialFile();

}



VOID CBinBlkMain::ResetMultipleBinSerialNo(LONG lIndex)
{
	int nCurrYear, nCurrMonth, nCurrDay;
	CString szCurrDate;
	CTime theTime;

	//LONG lIndex = CMS896AStn::m_lOTraceabilityFormat -1;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *pBinSerial;

	// open config file
	if (pUtl->LoadMultipleBinSerialFile() == FALSE)
	{
		return;
	}

	// get file pointer
	pBinSerial = pUtl->GetMultipleBinSerialFile();

	if (pBinSerial == NULL)
	{
		return;
	}

	theTime = CTime::GetCurrentTime(); 

	nCurrYear = theTime.GetYear();
	nCurrMonth = theTime.GetMonth();
	nCurrDay = theTime.GetDay();

	szCurrDate.Format("%d-%d-%d", nCurrYear, nCurrMonth, nCurrDay);

	//m_szaLastResetSerialDate[lIndex] = szCurrDate;
	
	//Check Load/Save Data
	if (pBinSerial != NULL)
	{
		(*pBinSerial)["Bin Serial"][lIndex]["LastResetSerialNoDate"] = szCurrDate;
	}

	//Reset counter
	//m_aulBinSerialNo[lIndex] = 0;
	
	//Check Load/Save Data
	if (pBinSerial != NULL)
	{
		(*pBinSerial)["Bin Serial"][lIndex]["SerialNo"] = 0;
	}

	pUtl->UpdateMultipleBinSerialFile();
	pUtl->CloseMultipleBinSerialFile();
	
}

/************************************/
/*             Step Move            */
/************************************/

BOOL CBinBlkMain::StepMove(ULONG ulBlkId, LONG lIndex, LONG &lXPosn, LONG &lYPosn, BOOL bLog)
{
	//LONG lX, lY;
	DOUBLE dPosX=0, dPosY=0;	//v4.57A7

	//v4.03
	ULONG ulNewIndex = lIndex;
	ULONG ulTempRow = 0, ulTempCol = 0;
	GetDieIndexInBinMap(ulBlkId, ulNewIndex, ulTempRow, ulTempCol);	//INdex here means die-number, not bond pos, so need to use ver2
	//GetNextDieIndexInOSRAMBinMixMap(ulBlkId, ulNewIndex);

	//v4.48A20	//WH SanAn, 3E DL
	if (bLog && IsUseBinMapBondArea(ulBlkId))
	{
		CString szLog;
		szLog.Format("STEP-MOVE (blk #%d): NewIndex=%lu, OrigIndex=%lu", ulBlkId, ulNewIndex, lIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}

	m_oBinBlk[ulBlkId].FindBinTableXYPosnGivenIndex(ulNewIndex, dPosX, dPosY, bLog);

	if (GrabNVIsAligned(ulBlkId) == TRUE) //  conver value 
	{
		DOUBLE dOldX = dPosX;
		DOUBLE dOldY = dPosY;
		//LONG lX1, lY1;
		DOUBLE dX1=0, dY1=0;

		DOUBLE dX, dY, dDistX, dDistY, dAngle;
		m_oBinBlk[ulBlkId].FindBinTableXYPosnGivenIndex(1, dX1, dY1);
		dDistX = (DOUBLE)(dPosX - dX1);
		dDistY = (DOUBLE)(dPosY - dY1);
		dAngle = GrabNVRotateAngleX(ulBlkId);
		dX = dDistX * cos(dAngle) - dDistY * sin(dAngle) + GrabNVXOffset(ulBlkId);
		dY = dDistY * cos(dAngle) + dDistX * sin(dAngle) + GrabNVYOffset(ulBlkId);
		dPosX = dX + dX1;
		dPosY = dY + dY1;

		//v4.57A10
		if (bLog)
		{
			CString szLog;
			szLog.Format("STEP-MOVE (blk #%d): XY (%f, %f), Realign(%ld, %ld, %f), Final-XY(%f, %f)",
						ulBlkId, dOldX, dOldY, 
						GrabNVXOffset(ulBlkId), GrabNVYOffset(ulBlkId), dAngle,
						dPosX, dPosY); 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
	}

	if (bLog)
	{
		CString szLog;
		szLog.Format("STEP-MOVE (blk #%d): Final-XY(%f, %f)",
					ulBlkId, dPosX, dPosY); 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	}

	lXPosn = (LONG) dPosX;
	lYPosn = (LONG) dPosY;
	return TRUE;
} //end StepMove

BOOL CBinBlkMain::StepDMove(ULONG ulBlkId, LONG lIndex, DOUBLE &dXPosn, DOUBLE &dYPosn, BOOL bLog)
{
	ULONG ulNewIndex = lIndex;
	ULONG ulTempRow = 0, ulTempCol = 0;
	GetDieIndexInBinMap(ulBlkId, ulNewIndex, ulTempRow, ulTempCol);	//INdex here means die-number, not bond pos, so need to use ver2
	//GetNextDieIndexInOSRAMBinMixMap(ulBlkId, ulNewIndex);
	if (bLog && IsUseBinMapBondArea(ulBlkId))
	{
		CString szLog;
		szLog.Format("STEP-MOVE (blk #%d): NewIndex=%lu, OrigIndex=%lu", ulBlkId, ulNewIndex, lIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}

	return SubStepDMove(ulBlkId, ulNewIndex, dXPosn, dYPosn, bLog);
}


BOOL CBinBlkMain::SubStepDMove(ULONG ulBlkId, ULONG ulNewIndex, DOUBLE &dXPosn, DOUBLE &dYPosn, BOOL bLog)
{
	DOUBLE dPosX = 0, dPosY = 0;

	m_oBinBlk[ulBlkId].FindBinTableXYPosnGivenIndex(ulNewIndex, dPosX, dPosY, bLog);

	if (GrabNVIsAligned(ulBlkId) == TRUE) //  conver value 
	{
		DOUBLE dOldX = dPosX;
		DOUBLE dOldY = dPosY;
		DOUBLE dX1 = 0, dY1 = 0;

		DOUBLE dX, dY, dDistX, dDistY, dAngle;
		m_oBinBlk[ulBlkId].FindBinTableXYPosnGivenIndex(1, dX1, dY1);
		dDistX = (DOUBLE)(dPosX - dX1);
		dDistY = (DOUBLE)(dPosY - dY1);
		dAngle = GrabNVRotateAngleX(ulBlkId);
		dX = dDistX * cos(dAngle) - dDistY * sin(dAngle) + GrabNVXOffset(ulBlkId);
		dY = dDistY * cos(dAngle) + dDistX * sin(dAngle) + GrabNVYOffset(ulBlkId);
		dPosX = dX + dX1;
		dPosY = dY + dY1;

		if (bLog)
		{
			CString szLog;
			szLog.Format("STEP-MOVE (blk #%d): XY (%f, %f), Realign(%ld, %ld, %f), Final-XY(%f, %f)",
						ulBlkId, dOldX, dOldY, 
						GrabNVXOffset(ulBlkId), GrabNVYOffset(ulBlkId), dAngle,
						dPosX, dPosY); 
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
	}

	if (bLog)
	{
		CString szLog;
		szLog.Format("STEP-MOVE (blk #%d): Final-XY(%f, %f)",
					ulBlkId, dPosX, dPosY); 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	}

	dXPosn = dPosX;
	dYPosn = dPosY;
	return TRUE;

} //end StepMove

BOOL CBinBlkMain::GetIndexMovePosnXY(ULONG ulBlkId, LONG lIndex, DOUBLE &dXPosn, DOUBLE &dYPosn)
{
	//LONG lX = 0, lY = 0;
	DOUBLE dX = 0, dY = 0;	//v4.57A7
	ULONG ulNewIndex = lIndex;

	if (m_oBinBlk[ulBlkId].FindBinTableXYPosnGivenIndex(ulNewIndex, dX, dY))
	{
		dXPosn = dX;		//in Um
		dYPosn = dY;		//in Um
	}
	return TRUE;
}

BOOL CBinBlkMain::UpdateCurrBondIndex(ULONG ulBlkId, ULONG ulNewIndex)
{
	m_oBinBlk[ulBlkId].SetNVCurrBondIndex(ulNewIndex);

	return TRUE;
} //end UpdateCurrBondIndex


VOID CBinBlkMain::UpdateLastFileSaveIndex(ULONG ulBlkId, ULONG ulNewIndex)
{
	m_oBinBlk[ulBlkId].SetNVLastFileSaveIndex(ulNewIndex);
} //end UpdateLastFileSaveIndex


/************************************/
/*         Grade Information        */
/************************************/

BOOL CBinBlkMain::SaveGradeInfo(BOOL bEnableAliasGrade, UCHAR ucGrade, ULONG ulInputCount, 
								UCHAR ucAliasGrade,
								CStringMapFile *pBTMSDfile)
{
	CStringMapFile *pBTfile = pBTMSDfile;
	if (pBTMSDfile == NULL)
	{
		pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	}

	//InputCount
	m_oBinGradeData[ucGrade].SetInputCount(ulInputCount);
	if (pBTfile != NULL)	//v4.52A8
	{
		(*pBTfile)["BinGradeData"][ucGrade]["InputCount"] = 
			m_oBinGradeData[ucGrade].GetInputCount();
	}

	//IsFull
	if (m_oBinGradeData[ucGrade].GetInputCount() > m_oBinGradeData[ucGrade].GetNVNoOfSortedDie())
	{
		m_oBinGradeData[ucGrade].SetNVIsFull(FALSE);
	}
	else
	{
		m_oBinGradeData[ucGrade].SetNVIsFull(TRUE);
	}

	//AliasGrade 
	//Disable AliasGrade funtion
	m_oBinGradeData[ucGrade].SetAliasGrade(0);

	if (pBTfile != NULL)	//v4.52A8
	{
		(*pBTfile)["BinGradeData"][ucGrade]["AliasGrade"] =
			m_oBinGradeData[ucGrade].GetAliasGrade();
	}

	//SetMsgCode("Grade Information is saved!");
	SetMsgCode(IDS_BT_GRADEINFOSAVED);
	if (pBTMSDfile == NULL)
	{
		CMSFileUtility::Instance()->SaveBTConfig();
	}
	return TRUE;

} //end SaveGradeInfo

/*************************************/
/*    Assist Physical Block Setup    */
/*************************************/
BOOL CBinBlkMain::IfDuringBond(ULONG ulPhyBlkId)
{
	ULONG i;

	for (i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetPhyBlkId() == ulPhyBlkId)
		{
			//if (m_oBinBlk[i].GetNoOfBondedDie() > 0)
			if (m_oBinBlk[i].GetNVNoOfBondedDie() > 0)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
} //end IfDuringBond


BOOL CBinBlkMain::IfContainBinBlk(ULONG ulPhyBlkId)
{
	ULONG i;

	for (i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetPhyBlkId() == ulPhyBlkId)
		{
			return TRUE;
		}
	}
	return FALSE;
} //end IfContainBinBlk

/************************************/
/*      Generate Output File        */
/************************************/
LONG CBinBlkMain::GenAllTempFiles(BOOL bUseNVData, BOOL bBackupTempFile)
{	
	ULONG i;
	BOOL bHasUpdatedTempFile;
	LONG lStatus = 1;
	BOOL bNeedToReset = FALSE;
	if (CMS896AStn::m_bEnableGradeMappingFile == TRUE)
	{
		GradeMapping();
	}

	UpdateBinMapFile();		//v4.04		//PLSG binmap fcn array printout only

	BOOL bWaferMapElectricInfoEmpty = FALSE;
	for (i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetNVNoOfBondedDie() > 0)
		{
			lStatus = m_oBinBlk[i].GenTempFile(i, m_bFilenameASWaferID, CMS896AStn::m_bAddDieTimeStamp, CMS896AStn::m_bCheckProcessTime,CMS896AStn::m_bAddSortingSequence, 
												bHasUpdatedTempFile, bWaferMapElectricInfoEmpty, bUseNVData, CMS896AStn::m_bForceDisableHaveOtherFile);
			if (bHasUpdatedTempFile)
			{
				bNeedToReset = TRUE;
			}
			if (bBackupTempFile == TRUE && bHasUpdatedTempFile == TRUE)
			{
				CString szTimeStamp;
				CTime curTime = CTime::GetCurrentTime();
				szTimeStamp = curTime.Format("%y%m%d%H%M%S");
				//BackupClearBinFilesByBlkId(i, TRUE, szTimeStamp, TRUE);	//v4.48A29
			}
		}
	}

	//v4.65A1
	//Reset TEMP-FILE data in NV RAM only AFTER tempfile is updated, to avoid
	// any data loss due to sw crash;
	if (bNeedToReset == TRUE)
	{
		ResetNVTempFileData();	
	}

	if (bWaferMapElectricInfoEmpty)
	{
		return BT_BACKUP_NVRAM_ERROR;
	}
	return lStatus;
} //end GenAllTempFiles


BOOL CBinBlkMain::BackupTempFile(ULONG ulBlkId)
{
	//v4.48A10	//v4.48A29
	CString szBackupFilePath = gszUSER_DIRECTORY + "\\TempFileBackup";
	if (_access(szBackupFilePath, 0) != -1)
	{
		//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		//CMSLogFileUtility::Instance()->MS_LogOperation("BackupTempFile: Start REMOVE ...");
		//pApp->SearchAndRemoveFiles(szBackupFilePath, 30);
		//CMSLogFileUtility::Instance()->MS_LogOperation("BackupTempFile: REMOVE done");
	}

	if (ulBlkId < BT_MAX_BINBLK_SIZE - 1)
	{
		m_oBinBlk[ulBlkId].BackupTempFile(ulBlkId);
	}

	return TRUE;
}

BOOL CBinBlkMain::BackupAllTempFiles()
{
	for (ULONG i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5
		m_oBinBlk[i].BackupTempFile(i);
	}

	return TRUE;
}


LONG CBinBlkMain::GenOutputFileDatabaseByBlkId(ULONG ulBlkId, CStringArray &szaPopUpMsg)
{
	CString szDatabaseFilename, szOutputFilename, szTempFilename, szBlkId, szTempFilenameCopy;
	CStdioFile cfTempFile;
	CStdioFile cfWaferMapColumns, cfOutputFileFormat;
	BOOL bIfTempFileExist, bIfGenDatabaseSuccess;
	LONG lGenDatabaseStatus = 1;
	CString szMsg;

	CGenerateDatabase	*pGenDB = ((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[ulBlkId]);
	CQueryDatabase		*pQryDB = (CQueryDatabase*)CMS896AStn::m_objaQueryDatabase[ulBlkId];
	szBlkId.Format("%d", ulBlkId);
	if (ulBlkId < 10)
	{
		szBlkId = "0" + szBlkId;
	}

	if (m_oBinBlk[ulBlkId].GetNVNoOfBondedDie() == 0)
	{
		lGenDatabaseStatus = 0;
		szMsg = "There are no die bonded on Blk " + szBlkId + ", so no output file is generated!";
		szaPopUpMsg.Add(szMsg);
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
		m_oBinBlk[ulBlkId].SetIsDatabaseGenerated(FALSE);
	}
	else
	{
		szTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkId + "TempFile.csv";

		bIfTempFileExist = cfTempFile.Open(_T(szTempFilename),
										   CFile::modeRead | CFile::shareDenyNone | CFile::typeText);

		szMsg.Format("Opened the Temp File: %d with path: %s",bIfTempFileExist, szTempFilename ); //4.53D50
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);

		if (bIfTempFileExist)
		{
			cfTempFile.Close();

			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Generated Entire Database");

			CStringArray szaRtnMsg;
			bIfGenDatabaseSuccess = pGenDB->GenEntireDatabase(szaRtnMsg);

			if (szaRtnMsg.GetSize() > 0)
			{
				szaPopUpMsg.Append(szaRtnMsg);
			}

			if (bIfGenDatabaseSuccess)
			{
				m_oBinBlk[ulBlkId].SetIsDatabaseGenerated(TRUE);

				szDatabaseFilename = pGenDB->GetDatabaseFilename();

				szMsg.Format("Database File name: %s", szDatabaseFilename );
				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);

				pQryDB->SetDatabaseFilename(szDatabaseFilename);
				
				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Setted name to Query Database");

				pQryDB->LoadDatabase();

				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Loaded Database");
			}
			else
			{
				m_oBinBlk[ulBlkId].SetIsDatabaseGenerated(FALSE);
				lGenDatabaseStatus = -1;
				szMsg = "Error: Failed to generate database for Blk " + szBlkId + "!";
				szaPopUpMsg.Add(szMsg);
				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
			}
		}
		else
		{
			m_oBinBlk[ulBlkId].SetIsDatabaseGenerated(FALSE);
			lGenDatabaseStatus = -2;
			szMsg = "Error: Failed to load temp file for Blk" + szBlkId + "!";
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
			szaPopUpMsg.Add(szMsg);
		}
	}

	if( lGenDatabaseStatus != 1 )
	{
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Cleanup Query DB and Gen DB because gen output DB fail");
		pGenDB->Cleanup();
		pQryDB->Cleanup();
	}

	return lGenDatabaseStatus;
}


LONG CBinBlkMain::GenOutputFileDatabase(ULONG ulBlkId, UCHAR ucGrade, CStringArray &szaPopUpMsg, ULONG ulMode)
{
	CString szMsg;
	LONG lGenDatabaseStatus = 1;	//OK	//Klocwork
	
	//--------------- Default is clear bin blk by phyblk id --------------//
	if (ulMode == CLEAR_BIN_BY_PHY_BLK)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[i].GetPhyBlkId() == ulBlkId)
			{
				szMsg.Format("phy find %lu, phy blk %lu", i, ulBlkId);
				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);

				lGenDatabaseStatus = GenOutputFileDatabaseByBlkId(i, szaPopUpMsg);
				if (lGenDatabaseStatus != 1)
				{
					return lGenDatabaseStatus;
				}
			}
		}
	}
	else if (ulMode == CLEAR_BIN_BY_GRADE)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if ((m_oBinBlk[i].GetGrade() == ucGrade) && (m_oBinBlk[i].GetNVNoOfBondedDie() > 0))
			{
				lGenDatabaseStatus = GenOutputFileDatabaseByBlkId(i, szaPopUpMsg);

				if (lGenDatabaseStatus != 1)
				{
					return lGenDatabaseStatus;
				}
			}
		} 

	}
	else
	{
		//------------------- Clear Bin Blk By Logical Block ------------------//
		lGenDatabaseStatus = GenOutputFileDatabaseByBlkId(ulBlkId, szaPopUpMsg);
	}
	
	//Return:
	//1 = OK; 0 = 0 Die count; -1 = Gen Database error; -2 = Load temp file error
	return lGenDatabaseStatus;
} //end GenOutputFileDatabase

LONG CBinBlkMain::GenStandardOutputFileByBlkId(ULONG ulBlkId, CString szOutputFilePath,
		CString szBinOutputFileFormat, BOOL bPrintLabel, CStringArray &szaPopUpMsg)
{
	CString szOutputFilename, szCustomSerialNumber, szBinBlkId;
	LONG lGenOutputFileStatus = 0;
	BOOL bIfLoadOk, bIfPrintBarcodeOk, bIfGenOutputFileOk;
	//BOOL bIfGenAllOutputFileOk;
	BOOL bFileExistErr = FALSE;
	BOOL bFileExist = FALSE;
	CString szMsg;

	CQueryDatabase		 *pQryDB = (CQueryDatabase*)CMS896AStn::m_objaQueryDatabase[ulBlkId];
	CGenerateDatabase	 *pGenDB = ((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[ulBlkId]);
	COutputFileInterface *pOutFI = CFactoryMap::GetInstance()->CreateInstance(szBinOutputFileFormat);

	if( (pOutFI == NULL) || (pQryDB==NULL) || (pGenDB==NULL) )
	{
		if( pQryDB!=NULL )
			pQryDB->Cleanup();
		else
			szaPopUpMsg.Add("Error: Query DB Object can't be created");

		if( pGenDB!=NULL )
			pGenDB->Cleanup();
		else
			szaPopUpMsg.Add("Error: Generate DB Object can't be created");

		if( pOutFI!=NULL )
		{
			pOutFI->Cleanup();
			delete [] pOutFI;
		}
		else
			szaPopUpMsg.Add("Error: Output File Object can't be created");
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Cleanup because some pointer is null");
		return lGenOutputFileStatus;
	}

	if (m_bFirstTimeInitOutputFile)
	{
		pOutFI->InitOleObject(); //do only once
		m_bFirstTimeInitOutputFile = FALSE; //reset
	}

	pOutFI->GenOutputFilename(pQryDB, szOutputFilePath);
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Set Customer Serial Number");
	szCustomSerialNumber = pGenDB->GetCustomSerialNumber();
	pOutFI->SetCustomSerialNumber(szCustomSerialNumber);
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Load Output File Format");
	bIfLoadOk = pOutFI->LoadOutputFileFormat(pGenDB);

	m_szCustomOutputFileName = pOutFI->GetCustomFileName();
	if (bIfLoadOk)
	{
		CStringArray szaRtnMsg;
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Generate Entire Output File");
		
		bIfGenOutputFileOk = pOutFI->GenEntireOutputFile(pQryDB, szaRtnMsg);

		if (szaRtnMsg.GetSize() > 0)
		{
			szaPopUpMsg.Append(szaRtnMsg);
		}

		if (!bIfGenOutputFileOk)
		{
			// Check File Exist Err
			for (INT i = 0; i < szaRtnMsg.GetSize(); i++)
			{
				if (szaRtnMsg.GetAt(i) == FILE_EXIST_ERR)
				{
					lGenOutputFileStatus = -4;
					bFileExistErr = TRUE;
					break;
				}
			}

			for (INT i = 0; i < szaRtnMsg.GetSize(); i++)
			{
				if (szaRtnMsg.GetAt(i) == BIN_FRAME_BC_CHECK_FAILED)
				{
					lGenOutputFileStatus = -5;
					
					break;
				}
			}

			// Check File Exist Err
			for (INT i = 0; i < szaRtnMsg.GetSize(); i++)
			{
				if (szaRtnMsg.GetAt(i) == FILE_EXIST_ERR_WITHOUT_RETRY)
				{
					lGenOutputFileStatus = -6;
					bFileExistErr = TRUE;
					break;
				}
			}

			// loop back to the output file for pop up the dialog for input
			for (INT i = 0; i < szaRtnMsg.GetSize(); i++)
			{
				if (szaRtnMsg.GetAt(i) == NO_EXTRA_INFO_FILE_DATA_ERR)
				{
					lGenOutputFileStatus = -7;
					break;
				}
			}

			// loop back to the output file for pop up the dialog for input
			for (INT i = 0; i < szaRtnMsg.GetSize(); i++)
			{
				if (szaRtnMsg.GetAt(i) == MANUAL_INPUT_BARCODE)
				{
					lGenOutputFileStatus = -8;
					break;
				}
			}

			//4.55T08 compare Physical Die Count with Temp File die count
			for (INT i = 0; i < szaRtnMsg.GetSize(); i++)
			{
				if (szaRtnMsg.GetAt(i) == DIE_COUNT_MATCH_ERR)
				{
					lGenOutputFileStatus = -9;
					break;
				}
			}

			szMsg.Format( "GenOutputFile is not Ok --- GenOutputFileStatus:%ld", lGenOutputFileStatus ); //4.52D11
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
		}
		else
		{
			if (bPrintLabel == TRUE)
			{
				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Print Barcode");
				bIfPrintBarcodeOk = pOutFI->PrintBarcode(pGenDB, pQryDB);
				
				if (bIfPrintBarcodeOk)
				{
					//bIfGenAllOutputFileOk = TRUE;
					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Print Barcode OK");
					lGenOutputFileStatus = 1;
				}
				else
				{
					//bIfGenAllOutputFileOk = FALSE;
					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Print Barcode Fail");
					szaPopUpMsg.Add("Label Printer Error: Please check the followings:\n1.Printer is connected properly.\n2.There are labels in printer. ");
					lGenOutputFileStatus = -2;
				}

				// Enabled in auto-Load frame only
				//if (bIfPrintBarcodeOk && (CMS896AStn::m_bStPrintLabelInABMode))		
				//{
				//	CMS896AStn::m_bStStartPrintLabel = TRUE;	// Trigger CSafety station to prtin label instead!!
				//}
			}
			else
			{
				//bIfGenAllOutputFileOk = TRUE;
				lGenOutputFileStatus = 1;
			}
		}

		szMsg.Format( "Load OutputFileFormat is ok --- GenOutputFileStatus:%ld", lGenOutputFileStatus );//4.52D11
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
	}
	else
	{
		//bIfGenAllOutputFileOk = FALSE;
		lGenOutputFileStatus = 0;

	}

	pGenDB->Cleanup();
	pQryDB->Cleanup();
	pOutFI->Cleanup();
	delete pOutFI;

	return lGenOutputFileStatus;
}


LONG CBinBlkMain::GenStandardOutputFile(ULONG ulBlkId, UCHAR ucGrade, CString szOutputFilePath,
										CString szBinOutputFileFormat, BOOL bPrintLabel, CStringArray &szaPopUpMsg, ULONG ulMode)
{
	LONG lGenOutputFileStatus = 1;		//Klocwork
	//bIfGenAllOutputFileOk = FALSE;
	szaPopUpMsg.RemoveAll();

	//--------------- Default is clear bin blk by phyblk id --------------//
	if (ulMode == CLEAR_BIN_BY_PHY_BLK)
	{
		
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if ((m_oBinBlk[i].GetPhyBlkId() == ulBlkId) && (m_oBinBlk[i].GetIsDatabaseGenerated()))
			{
				lGenOutputFileStatus = GenStandardOutputFileByBlkId(i, szOutputFilePath,
									   szBinOutputFileFormat, bPrintLabel, szaPopUpMsg);

				if (lGenOutputFileStatus != 1)
				{
					return lGenOutputFileStatus;
				}
			}
		}
	}
	else if (ulMode == CLEAR_BIN_BY_GRADE)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if ((m_oBinBlk[i].GetGrade() == ucGrade) && (m_oBinBlk[i].GetNVNoOfBondedDie() > 0) && (m_oBinBlk[i].GetIsDatabaseGenerated()))
			{
				lGenOutputFileStatus = GenStandardOutputFileByBlkId(i, szOutputFilePath,
									   szBinOutputFileFormat, bPrintLabel, szaPopUpMsg);

				if (lGenOutputFileStatus != 1)
				{
					return lGenOutputFileStatus;
				}
			}
		}

	}
	else
	{
		//------------------- Clear Bin Blk By Logical Block ------------------//
		lGenOutputFileStatus = GenStandardOutputFileByBlkId(ulBlkId, szOutputFilePath,
							   szBinOutputFileFormat, bPrintLabel, szaPopUpMsg);
	}

	//v4.48A18	//Cree HuiZhou
	if (m_bEnable2DBarcodeOutput)
	{
		ULONG ulData = 0;
		CString szData = "";
		CString szValue = "";

		for (INT i=0; i<10; i++)
		{
			ulData = (ULONG) m_oBinBlk[ulBlkId].GetRandomHoleDieIndex(ulBlkId, i);
			szValue.Format("%lu", ulData);
			szData = szData + szValue + ",";
		}

		CString szMsg;
		szMsg.Format("BIN #%d Random HOLE index (Gen Standard OutputFile): ", ulBlkId);
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
	}

	//0 = Cannot create object; 1 = OK; -1 = fail to gen output file; -2 = print label error; -3 = output format not define
	return lGenOutputFileStatus;
} //end GenStandardOutputFile

VOID CBinBlkMain::RemoveTempFileAndResetIndexByBlkId(ULONG ulBlkId)
{
	CString szBlkId, szTempFilename;
	CString szLogFilename;

	szBlkId.Format("%d", ulBlkId);
	if (ulBlkId < 10)
	{
		szBlkId = "0" + szBlkId;
	}

	//changelight request 1:	//v4.60A3
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetCustomerName() == "ChangeLight(XM)") || 
		(pApp->GetCustomerName() == "SanAn") )
	{
		CString szNewBinTimeFile = gszROOT_DIRECTORY + "\\Exe\\NewBinTime\\" + szBlkId + ".txt";
		remove(szNewBinTimeFile);
	}

	szTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkId + "TempFile.csv";

	remove(szTempFilename);
//	SanAn TJ 2017
	for(int mm=0; mm<3; mm++)
	{
		if (_access(szTempFilename, 0) != -1)
		{
			CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
			pApp->SetErrorMessage("BT remove tempfile and reset index, re-delete " + szTempFilename);
			DeleteFile(szTempFilename);
		}
		else
		{
			break;
		}
	}
//	SanAn TJ 2017

	m_oBinBlk[ulBlkId].SetNVLastFileSaveIndex(0);

}

VOID CBinBlkMain::RemoveTempFileAndResetIndex(ULONG ulBlkId, UCHAR ucGrade, ULONG ulMode)
{	
	if (ulMode == CLEAR_BIN_BY_PHY_BLK)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[i].GetPhyBlkId() == ulBlkId)
			{
				RemoveTempFileAndResetIndexByBlkId(i);
			}

		}
	}
	else if (ulMode == CLEAR_BIN_BY_GRADE)
	{
		for (ULONG i = 1; i <= GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[i].GetGrade() == ucGrade)
			{
				RemoveTempFileAndResetIndexByBlkId(i);
			}
		}
	}
	else
	{
		RemoveTempFileAndResetIndexByBlkId(ulBlkId);
	}
} //end RemoveTempFileAndResetIndex




/***********************************/
/*     Generate Wafer End File     */
/***********************************/
BOOL CBinBlkMain::GenBinBlkFile(LONG &lPickTotal, LONG &lMapTotal)
{
	CStdioFile cfBinBlkFile;
	CFileException e;
	UCHAR ucBinBlkGrade;
	ULONG ulNoLeft = 0, ulNoPicked = 0, ulNoTotal = 0;
	ULONG ulNoMiss = 0, ulNoDefect = 0, ulNoBadCut = 0, ulNoEmpty= 0, ulNoInk = 0, ulNoRotate = 0;
	CString szBinBlkId, szBinBlkGrade, szNoLeft, szNoPicked, szNoTotal;
	CString szNoMiss, szNoDefect, szNoBadcut, szNoEmpty, szNoInk, szNoRotate;
	CString szBarCode, szRankID;
	ULONG ulCol, ulRow;
	ULONG i;
	ULONG ulMaxRow = 0, ulMaxCol = 0;
	ULONG ulDieState;
	ULONG ulMissingDieNo;

	ULONG	ulNoPickedOnBin;
	CString	szNoPickedOnBin;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//v4.42T17
	if (!CMS896AStn::m_WaferMapWrapper.IsMapValid() == TRUE)
	{
		return FALSE;
	}
	if (CMS896AStn::m_WaferMapWrapper.GetMapDimension(ulMaxRow, ulMaxCol) != TRUE)
	{
		return FALSE;
	}

	if (_access(gszEXE_DIRECTORY + "\\BinBlkFile.csv", 0) != -1)
	{
		remove(gszEXE_DIRECTORY + "\\BinBlkFile.csv");
	}

	BOOL bOpen = cfBinBlkFile.Open(_T(gszEXE_DIRECTORY + "\\BinBlkFile.csv"), 
					  CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText,&e);

	if (bOpen == FALSE)
	{
		return FALSE;
	}

	try
	{
		cfBinBlkFile.SeekToEnd();

		BOOL bFinisar = (pApp->GetCustomerName() == CTM_FINISAR);
		ULONG ulNoOfBlk = GetNoOfBlk();
		if( bFinisar )
		{
			ulNoOfBlk = 150;
		}

		UCHAR ucOffset = CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
		CUIntArray aulAllGrades;
		aulAllGrades.RemoveAll();
		for (i = 1; i <= ulNoOfBlk; i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			ucBinBlkGrade	= m_oBinBlk[i].GetGrade();

			// Calculate the missing die
			ulMissingDieNo	= 0;
			ulNoMiss		= 0;
			ulNoDefect		= 0;
			ulNoBadCut		= 0;
			ulNoEmpty		= 0;
			ulNoLeft		= 0;
			ulNoPicked		= 0;
			ulNoTotal		= 0;
			ulNoInk			= 0;
			ulNoRotate  	= 0;	

			if( bFinisar && CMS896AApp::m_bEnableSubBin )
			{
				CUIntArray aulGradeList;
				aulGradeList.RemoveAll();
				for (int p = 1; p <= m_pBinTable->m_nNoOfSubBlk; p++)
				{
					if( m_pBinTable->m_nSubBlk[p]==i )
					{
						for(int q = 1; q <= m_pBinTable->m_nNoOfSubGrade; q ++)
						{
							aulGradeList.Add(m_pBinTable->m_nSubGrade[p][q] + ucOffset);
							aulAllGrades.Add(m_pBinTable->m_nSubGrade[p][q] + ucOffset);
						}
					}
				}

				if( aulGradeList.GetSize() == 0 )
				{
					BOOL bInUse = FALSE;
					for (int p = 1; p <= m_pBinTable->m_nNoOfSubBlk; p ++)
					{
						for(int q = 1; q <= m_pBinTable->m_nNoOfSubGrade; q ++)
						{
							if(ucBinBlkGrade == m_pBinTable->m_nSubGrade[p][q])
							{
								bInUse = TRUE;
								break;
							}
						}
						if( bInUse )
						{
							break;
						}
					}
					if( bInUse==FALSE )
					{
						for(int kk=0; kk<aulAllGrades.GetSize(); kk++)
						{
							if( (ucBinBlkGrade + ucOffset)==aulAllGrades.GetAt(kk) )
							{
								bInUse = TRUE;
							}
						}
					}
					if( bInUse==FALSE )
					{
						aulGradeList.Add(ucBinBlkGrade + ucOffset);
						aulAllGrades.Add(ucBinBlkGrade + ucOffset);
					}
				}

				if( aulGradeList.GetSize()>0 )
				{
					ucBinBlkGrade	= m_oBinBlk[i].GetGrade();
				}
				else
				{
					ucBinBlkGrade	= (UCHAR)i;
				}

				for(UINT mm = 0; mm < (UINT)aulGradeList.GetSize(); mm++)
				{
					ULONG ulGradeLeft = 0, ulGradePicked = 0, ulGradeTotal = 0;
					UCHAR ucMapGrade		= aulGradeList.GetAt(mm);
					CMS896AStn::m_WaferMapWrapper.GetStatistics(ucMapGrade, ulGradeLeft, ulGradePicked, ulGradeTotal);

					for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
					{
						for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
						{
							UCHAR ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalDieInformation(ulRow, ulCol);
							if( ucMapGrade!=ucOrigGrade )
							{
								continue;
							}

							ulDieState = CMS896AStn::m_WaferMapWrapper.GetDieState(ulRow, ulCol);
							if (ulDieState == WT_MAP_DIESTATE_DEFECT)
							{
								ulNoDefect++;
							}
							else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
							{
								ulNoBadCut++;
							}
							else if (ulDieState == WT_MAP_DIESTATE_EMPTY)
							{
								ulNoEmpty++;
							}
							else if (ulDieState > WT_MAP_DIESTATE_MISSING)
							{
								ulMissingDieNo++;
							}
						}
					}
					ulNoLeft += ulGradeLeft;
					ulNoPicked += ulGradePicked;
					ulNoTotal += ulGradeTotal;
				}
			}
			else
			{
				UCHAR ucMapGrade		= ucBinBlkGrade + ucOffset;
				CMS896AStn::m_WaferMapWrapper.GetStatistics(ucMapGrade, ulNoLeft, ulNoPicked, ulNoTotal);

				for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
				{
					for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
					{
						ulDieState = CMS896AStn::m_WaferMapWrapper.GetDieState(ulRow, ulCol);
						if (ulDieState > WT_MAP_DIESTATE_MISSING)
						{
							// Extract the grade info
							UCHAR	ucMissingGrade = (UCHAR)(ulDieState - WT_MAP_DIESTATE_MISSING);
							if( ulDieState > WT_MAP_DS_BH_UPLOOK_FAIL )
							{
								ucMissingGrade = (UCHAR)(ulDieState - WT_MAP_DS_BH_UPLOOK_FAIL);
							}

							if (ucMissingGrade == ucBinBlkGrade)
							{
								ulMissingDieNo++;
							}
						}
		
						//v4.42T17	//bug fix of AoYang WaferEnd library
						UCHAR ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetGrade(ulRow, ulCol);
						if( bFinisar )
						{
							ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalDieInformation(ulRow, ulCol);
						}
						if( ucMapGrade!=ucOrigGrade )
						{
							continue;
						}

				
						if (ulDieState == WT_MAP_DIESTATE_DEFECT)
						{
							ulNoDefect++;
						}
						else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
						{
							ulNoBadCut++;
						}
						else if (ulDieState == WT_MAP_DIESTATE_EMPTY)
						{
							ulNoEmpty++;
						}
						else if (ulDieState == WT_MAP_DIESTATE_INK)
						{
							ulNoInk++;					
						}
						else if (ulDieState == WT_MAP_DIESTATE_ROTATE)
						{
							ulNoRotate++;
						}
					
					}
				}
			}

			if (ulNoPicked > ulMissingDieNo)
			{
				ulNoPicked = (ulNoPicked - ulMissingDieNo);
			}
			else
			{
				ulNoPicked = 0;
			}
			if( bFinisar )
			{
				ulNoLeft = ulNoTotal - ulNoPicked;
			}
			ulNoPickedOnBin = m_oBinBlk[i].GetNVNoOfBondedDie();

			szBinBlkId.Format("%d",		i);
			szBinBlkGrade.Format("%d",	ucBinBlkGrade);
			if (CMS896AStn::m_bUseOptBinCountDynAssignGrade == TRUE)
			{
				USHORT usOrgGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(ucBinBlkGrade + ucOffset) - ucOffset;
				szBinBlkGrade.Format("%d", usOrgGrade);
			}
			szNoLeft.Format("%d",		ulNoLeft);
			szNoPicked.Format("%d",		ulNoPicked);
			szNoPickedOnBin.Format("%d", ulNoPickedOnBin); 
			szNoTotal.Format("%d",		ulNoTotal);
			if (m_pBinTable != NULL)
				szBarCode = m_pBinTable->GetBLBarcodeData(i);	//v4.51A17
			else
				szBarCode = "BarcodeBTNull";
			szNoMiss.Format("%lu",		ulMissingDieNo);
			szNoDefect.Format("%lu",	ulNoDefect);
			szNoBadcut.Format("%lu",	ulNoBadCut);
			szNoEmpty.Format("%lu",		ulNoEmpty);
			szRankID = GrabRankID(ucBinBlkGrade);
			szNoInk.Format("%lu",	ulNoInk);
			szNoRotate.Format("%lu",	ulNoRotate);
		
			lPickTotal = lPickTotal + ulNoPicked;
			lMapTotal  = lMapTotal + ulNoTotal;

			if (CMS896AStn::m_bUseOptBinCountDynAssignGrade == TRUE)
			{
				USHORT usOrgGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(m_oBinBlk[i].GetGrade() + 
									CMS896AStn::m_WaferMapWrapper.GetGradeOffset()) - CMS896AStn::m_WaferMapWrapper.GetGradeOffset();

				szBinBlkGrade.Format("%d", usOrgGrade);
			}

			cfBinBlkFile.WriteString(szBinBlkId + "," + szBinBlkGrade + "," + szNoLeft + "," +
									 szNoPicked + "," + szNoPickedOnBin + "," + szNoTotal	+ "," + szBarCode + "," + 
									 szNoMiss	+ "," + szNoDefect		+ "," + szNoBadcut	+ "," + szNoEmpty + "," +	//v4.40T14
									 szNoInk    + "," + szNoRotate      + "," + szRankID + "\n");
		}


		cfBinBlkFile.Close();
	}
	catch( CFileException* e )
	{
		if( e->m_cause == CFileException::badPath )
		{
			//m_szaPopUpMsg.Add("Error: File path not found!");
		}
		e->Delete();
		return FALSE;
	}

	return TRUE;
} //end GenBinBlkFile

/***********************************************/
/*     Generate Bin Summary Output File Name   */
/***********************************************/
// Auto Generate Bin Summary File if 
// Current Time > GenSummaryTime && LastGenFileTime < GenSummaryFileTime
/*

*/
VOID CBinBlkMain::SetBinSummaryGenTime(CString szGenTime[])
{
	for (INT i = 0; i < BT_SUMMARY_TIME_NO; i++)
	{
		m_szBinSummaryGenTime[i] = szGenTime[i];
	}
}

VOID CBinBlkMain::SetBinBlkSummaryPeriodNum(LONG lPeriodNum)
{
	m_lGenSummaryPeriodNum = lPeriodNum;
}

VOID CBinBlkMain::SetLastGenSummaryTime(CString szLastGenTime)
{
	m_szLastGenSummaryTime = szLastGenTime;
}

CString CBinBlkMain::GetLastGenSummaryTime()
{
	return m_szLastGenSummaryTime;
}

VOID CBinBlkMain::UpdateLastGenSummaryTime()
{
	CTime curTime = CTime::GetCurrentTime();
	m_szLastGenSummaryTime = curTime.Format("%Y/%m/%d,%H:%M");
}

BOOL CBinBlkMain::ConvertStringToDate(CString szDate, INT &nYear, INT &nMonth, INT &nDay)
{
	INT nCol;

	nCol = szDate.Find('/');
	if (nCol != -1)
	{
		nYear = atoi(szDate.Left(nCol));
		szDate = szDate.Mid(nCol + 1);

		nCol = szDate.Find('/');

		if (nCol != -1)
		{
			nMonth = atoi(szDate.Left(nCol));
			nDay = atoi(szDate.Mid(nCol + 1));
		}
	}

	return TRUE;
}

BOOL CBinBlkMain::ConvertStringToTime(CString szTime, INT &nHour, INT &nMin)
{
	INT nCol;

	nCol = szTime.Find(':');
	if (nCol != -1)
	{
		nHour = atoi(szTime.Left(nCol));
		nMin = atoi(szTime.Mid(nCol + 1));
	}
	
	return TRUE;
}


BOOL CBinBlkMain::CheckAutoGenerateBinSummary()
{
	INT nIndex = -1;
	INT nYear, nMonth, nDay, nHour, nMin;

	CTime curTime = CTime::GetCurrentTime();
	CTime ctGenSummaryTime;
	CString szCurTime;

	CString szGenTimeString;
	
	if (m_szLastGenSummaryTime != "")
	{	
		INT nCol;
		CString szDate, szTime;

		nCol = m_szLastGenSummaryTime.Find(',');

		if (nCol == -1)
		{
			return FALSE;
		}
		
		szDate = m_szLastGenSummaryTime.Left(nCol);
		szTime = m_szLastGenSummaryTime.Mid(nCol + 1);
		
		ConvertStringToDate(szDate , nYear, nMonth, nDay);
		ConvertStringToTime(szTime , nHour, nMin);
		
		CTime ctLastGenSummaryTime(nYear, nMonth, nDay, nHour, nMin, 0);

		for (INT i = 0; i < m_lGenSummaryPeriodNum; i++)
		{
			ConvertStringToTime(m_szBinSummaryGenTime[i], nHour, nMin);

			if (m_szBinSummaryGenTime[i] != "")
			{
				ctGenSummaryTime = CTime::CTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), nHour , nMin , 0);

				szGenTimeString = ctGenSummaryTime.Format("%Y%m%d,%H%M%S");
				szCurTime = curTime.Format("%Y%m%d,%H%M%S");

				if (curTime >= ctGenSummaryTime)
				{
					nIndex = i;
				}
			}
		}

		if (nIndex == -1)
		{
			return FALSE;
		}

		ConvertStringToTime(m_szBinSummaryGenTime[nIndex], nHour, nMin);
		ctGenSummaryTime = CTime::CTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), nHour , nMin , 0);

		if (ctLastGenSummaryTime < ctGenSummaryTime)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBinBlkMain::AutoGenBinSummaryFile(CString szBinSummaryOutputPath, CString szMachineNo)
{
	if (m_lGenSummaryPeriodNum <= 0)
	{
		return FALSE;
	}

	if (CheckAutoGenerateBinSummary() == TRUE)
	{
		
		GenBinSummaryOutputFile(szBinSummaryOutputPath, szMachineNo);
		// Update the Last Gen Summary File Time
		
		return TRUE;
	}
	
	return FALSE;
}

VOID CBinBlkMain::GenBinSummaryOutputFileName(CString szBinSummaryOutputPath, CString szMachineNo)	
{
	int nYear, nMonth, nDay, nMachineNo, nHour, nMin;
	CString szFileName; 
	CString szYear, szMonth, szDay, szHour, szMin;

	//Get current time
	CTime theTime = CTime::GetCurrentTime();

	nYear	= theTime.GetYear();
	nMonth	= theTime.GetMonth();
	nDay	= theTime.GetDay();
	nHour	= theTime.GetHour();
	nMin	= theTime.GetMinute();
	
	szYear.Format("%d", nYear);
	szYear = szYear.Right(2);

	szMonth.Format("%d", nMonth);
	if (nMonth < 10)
	{
		szMonth = "0" + szMonth;
	}

	szDay.Format("%d", nDay);
	if (nDay < 10)
	{
		szDay = "0" + szDay;
	}

	szHour.Format("%d", nHour);
	if (nHour < 10)
	{
		szHour = "0" + szHour;
	}

	szMin.Format("%d", nMin);
	if (nMin < 10)
	{
		szMin = "0" + szMin;
	}

	nMachineNo = atoi((LPCTSTR)szMachineNo);

	if (nMachineNo < 10)
	{
		szMachineNo = "0" + szMachineNo;    //adjust to 2 digits
	} 

	szFileName = "M" + szMachineNo + szYear + szMonth + szDay + szHour + szMin;
	
	m_szBinSummaryOutputPath = szBinSummaryOutputPath + "\\" + szFileName + ".txt";

} //end GenBinSummaryOutputFileName


/*******************************************/
/*     Generate Bin Summary Output File    */
/*******************************************/
BOOL CBinBlkMain::GenBinSummaryOutputFile(CString szBinSummaryOutputPath, CString szMachineNo)	
{
	CStdioFile cfOutputFile;
	CFileException e;
	BOOL bRtn = FALSE;
	ULONG ulNoOfSortedDie, i;
	CString szNoOfSortedDie, szTemp;
	UCHAR ucGrade;

	GenBinSummaryOutputFileName(szBinSummaryOutputPath, szMachineNo);
	
	remove(m_szBinSummaryOutputPath);

	if (!cfOutputFile.Open(m_szBinSummaryOutputPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite, &e))
	{
		return FALSE;
	}
	else
	{
		try
		{
			cfOutputFile.SeekToEnd();

			for (i = 1; i <= GetNoOfBlk(); i++)
			{
				ucGrade = GrabGrade(i);
				ulNoOfSortedDie = GrabNVNoOfSortedDie(ucGrade);
				szNoOfSortedDie.Format("%d", ulNoOfSortedDie);

				szTemp.Format("B%d", i);

				cfOutputFile.WriteString(szTemp + " = " + szNoOfSortedDie + "\n");
			}

			cfOutputFile.Close();
		}
		catch (CFileException *e)
		{
			e->Delete();
			return FALSE;
		}
		bRtn = TRUE;
	}
	return bRtn;
} //end GenBinSummaryOutputFile

VOID CBinBlkMain::SetBackupTempFilePath(CString szPath)		//v4.48A10	//WH Sanan
{
	m_szBackupTempFilePath = szPath;
}


/************************************/
/*    Grab CBinBlk Class members    */
/************************************/
BOOL CBinBlkMain::GrabIsSetup(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetIsSetup();
	}
	else
	{
		return FALSE;
	}
} //end GrabIsSetup


UCHAR CBinBlkMain::GrabStatus(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetStatus();
	}
	else
	{
		return 0;
	}
} // end GrabIsDisable


LONG CBinBlkMain::GrabBlkUpperLeftX(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetUpperLeftX();
} //end GrabBlkUpperLeftX


LONG CBinBlkMain::GrabBlkUpperLeftY(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetUpperLeftY();
} //end GrabBlkUpperLeftY


LONG CBinBlkMain::GrabBlkLowerRightX(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetLowerRightX();
} //end GrabBlkLowerRightX


LONG CBinBlkMain::GrabBlkLowerRightY(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetLowerRightY();
} //end GrabBlkLowerRightY


LONG CBinBlkMain::GrabBlkUpperLeftXFromSetup(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetUpperLeftXFromSetup();
} //end GrabBlkUpperLeftXFromSetup


LONG CBinBlkMain::GrabBlkUpperLeftYFromSetup(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetUpperLeftYFromSetup();
} //end GrabBlkUpperLeftYFromSetup


LONG CBinBlkMain::GrabBlkLowerRightXFromSetup(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetLowerRightXFromSetup();
} //end GrabBlkLowerRightXFromSetup


LONG CBinBlkMain::GrabBlkLowerRightYFromSetup(ULONG ulBlkId)
{	
	return m_oBinBlk[ulBlkId].GetLowerRightYFromSetup();
} //end GrabBlkLowerRightYFromSetup


UCHAR CBinBlkMain::GrabGrade(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetGrade();
	}
	else
	{
		return 0;
	}
} //end GrabGrade


UCHAR CBinBlkMain::GrabOriginalGrade(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetOriginalGrade();
	}
	else
	{
		return 0;
	}
} //end GrabOriginalGrade


BOOL CBinBlkMain::SetDDiePitchX(ULONG ulBlkId, DOUBLE dDiePitchX)		//D means DOUBLE
{
	m_oBinBlk[ulBlkId].SetDDiePitchX(dDiePitchX);
	return TRUE;
}


DOUBLE CBinBlkMain::GrabDDiePitchX(ULONG ulBlkId)		//D means DOUBLE
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_oBinBlk[ulBlkId].GetDDiePitchX();				//v4.59A19	//MS90 with 0.1um encoder
	}
	else
	{
		return 0;
	}
}


BOOL CBinBlkMain::SetDDiePitchY(ULONG ulBlkId, DOUBLE dDiePitchY)		//D means DOUBLE
{	
	m_oBinBlk[ulBlkId].SetDDiePitchY(dDiePitchY);
	return TRUE;
}


DOUBLE CBinBlkMain::GrabDDiePitchY(ULONG ulBlkId)		//D means DOUBLE
{	
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetDDiePitchY();				//v4.59A19	//MS90 with 0.1um encoder
	}
	else
	{
		return 0;
	}
}

BOOL CBinBlkMain::GrabUsePt5UmInDiePitchX(ULONG ulBlkId)
{	
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_oBinBlk[ulBlkId].GetPoint5UmInDiePitchX();
	}
	else
	{
		return FALSE;
	}
}

BOOL CBinBlkMain::GrabUsePt5UmInDiePitchY(ULONG ulBlkId)
{	
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_oBinBlk[ulBlkId].GetPoint5UmInDiePitchY();
	}
	else
	{
		return FALSE;
	}
}

VOID CBinBlkMain::SetUsePt5UmInDiePitchX(ULONG ulBlkId, BOOL bEnable)
{
	CString szLog;
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchX(bEnable);
	}
	else if (ulBlkId == 0)
	{
		for (INT i=1; i<=(BT_MAX_BINBLK_SIZE - 1); i++)
		{
			m_oBinBlk[i].SetPoint5UmInDiePitchX(bEnable);
		}
	}
	else
	{
		return;
	}
}

VOID CBinBlkMain::SetUsePt5UmInDiePitchY(ULONG ulBlkId, BOOL bEnable)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].SetPoint5UmInDiePitchY(bEnable);
	}
	else if (ulBlkId == 0)
	{
		for (INT i=1; i<=(BT_MAX_BINBLK_SIZE - 1); i++)
		{
			m_oBinBlk[i].SetPoint5UmInDiePitchY(bEnable);
		}
	}
	else
	{
		return;
	}
}

BOOL CBinBlkMain::GrabUseByPassBinMap(ULONG ulBlkId)
{	
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_oBinBlk[ulBlkId].GetByPassBinMap();
	}
	else
	{
		return FALSE;
	}
}

VOID CBinBlkMain::SetUseByPassBinMap(ULONG ulBlkId, BOOL bEnable)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();	
	if (pBTfile == NULL)
	{
		return;
	}

	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].SetByPassBinMap(bEnable);
		(*pBTfile)["BinBlock"][ulBlkId]["ByPassBinMap"] = bEnable;
	}
	else if (ulBlkId == 0)
	{
		for (INT i=1; i<=(BT_MAX_BINBLK_SIZE - 1); i++)
		{
			m_oBinBlk[i].SetByPassBinMap(bEnable);
			(*pBTfile)["BinBlock"][i]["ByPassBinMap"] = bEnable;
		}
	}
	else
	{
	}
	CMSFileUtility::Instance()->SaveBTConfig();
}

DOUBLE CBinBlkMain::GrabBondAreaOffsetX(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_oBinBlk[ulBlkId].GetBondAreaOffsetX();
	}
	else
	{
		return 0;
	}
}

DOUBLE CBinBlkMain::GrabBondAreaOffsetY(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_oBinBlk[ulBlkId].GetBondAreaOffsetY();
	}
	else
	{
		return 0;
	}
}

VOID CBinBlkMain::SetBondAreaOffset(ULONG ulBlkId, DOUBLE dOffsetXInFileUnit, DOUBLE dOffsetYInFileUnit)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].SetBondAreaOffset(dOffsetXInFileUnit, dOffsetYInFileUnit);
	}
	else if (ulBlkId == 0)
	{
		for (INT i=1; i<=(BT_MAX_BINBLK_SIZE - 1); i++)
		{
			m_oBinBlk[i].SetBondAreaOffset(dOffsetXInFileUnit, dOffsetYInFileUnit);
		}
	}
	else
	{
		return;
	}
}

ULONG CBinBlkMain::GrabOriginalWalkPath(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetOriginalWalkPath();
	}
	else
	{
		return 0;
	}
} //end GrabOriginalWalkPath


ULONG CBinBlkMain::GrabWalkPath(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetWalkPath();
	}
	else
	{
		return 0;
	}
} //end GrabWalkPath


ULONG CBinBlkMain::GrabWalkPath(ULONG ulBlkId, const ULONG ulCurWalkPath)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetWalkPath(ulCurWalkPath);
	}
	else
	{
		return 0;
	}
}

ULONG CBinBlkMain::GrabNoOfDiePerBlk(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return  m_oBinBlk[ulBlkId].GetDiePerBlk();
	}
	else
	{
		return 0;
	}
} //end GrabNoOfDiePerBlk


BOOL CBinBlkMain::SetNoOfDiePerBlk(ULONG ulBlkId, ULONG ulDiePerBlk)
{
	m_oBinBlk[ulBlkId].SetDiePerBlk(ulDiePerBlk);
	return TRUE;
}


ULONG CBinBlkMain::GrabNoOfDiePerRow(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetDiePerRow();
	}
	else
	{
		return 0;
	}
} //end GrabNoOfDiePerRow


BOOL CBinBlkMain::SetNoOfDiePerRow(ULONG ulBlkId, ULONG ulDiePerRow)
{
	m_oBinBlk[ulBlkId].SetDiePerRow(ulDiePerRow);
	return TRUE;
}


ULONG CBinBlkMain::GrabNoOfDiePerCol(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetDiePerCol();
	}
	else
	{
		return 0;
	}
} //end GrabNoOfDiePerCol


BOOL CBinBlkMain::SetNoOfDiePerCol(ULONG ulBlkId, ULONG ulDiePerCol)
{
	m_oBinBlk[ulBlkId].SetDiePerCol(ulDiePerCol);
	return TRUE;
}


BOOL CBinBlkMain::GrabIsDisableFromSameGradeMerge(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetIsDisableFromSameGradeMerge();
	}
	else
	{
		return 0;
	}
} //end GrabIsDisableFromSameGradeMerge


ULONG CBinBlkMain::GrabPhyBlkId(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetPhyBlkId();
	}
	else
	{
		return 0;
	}
} //end GrabPhyBlkId


ULONG CBinBlkMain::GrabSerialNo(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))			//v3.35
	{
		return m_oBinBlk[ulBlkId].GetSerialNo();
	}
	else
	{
		return 0;
	}
} //end GrabSerialNo


ULONG CBinBlkMain::GrabNVNoOfBondedDie(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVNoOfBondedDie();
	}
	else
	{
		return 0;
	}
} //end GrabNVNoOfBondedDie


ULONG CBinBlkMain::GrabBondRowColGivenIndex(ULONG ulBlkId, ULONG Index, LONG &lRow, LONG &lCol)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].FindBondRowColGivenIndex(Index, lRow, lCol,TRUE);
	}
	else
	{
		return 0;
	}
}

ULONG CBinBlkMain::GrabOriginalIndex(ULONG ulBlkId, ULONG Index)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetOriginalIndex(Index);
	}
	else
	{
		return 0;
	}
}

ULONG CBinBlkMain::GrabNVCurrBondIndex(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVCurrBondIndex();
	}
	else
	{
		return 0;
	}
} 

ULONG CBinBlkMain::GrabNVLastFileSaveIndex(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVLastFileSaveIndex();
	}
	else
	{
		return 0;
	}
} //end GrabNVLastFileSaveIndex


BOOL CBinBlkMain::GrabNVIsFull(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVIsFull();
	}
	else
	{
		return FALSE;
	}
} //end GrabNVIsFull


BOOL CBinBlkMain::GrabNVIsAligned(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVIsAligned();
	}
	else 
	{
		return FALSE;
	}
} //end GrabNVIsAligned


LONG CBinBlkMain::GrabNVXOffset(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVXOffset();
	}
	else
	{
		return FALSE;
	}
} //end GrabNVXOffset


LONG CBinBlkMain::GrabNVYOffset(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVYOffset();
	}
	else
	{
		return FALSE;
	}
}//end GrabNVYOffset


DOUBLE CBinBlkMain::GrabNVRotateAngleX(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNVRotateAngleX();
	}
	else
	{
		return FALSE;
	}
} //end GrabNVRotateAngleX


VOID CBinBlkMain::SetIsAligned(ULONG ulBlkId, BOOL bIsAligned)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		m_oBinBlk[ulBlkId].SetNVIsAligned(bIsAligned);
	}
} //end SetIsAligned


VOID CBinBlkMain::SetXOffset(ULONG ulBlkId, LONG lXOffset)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		m_oBinBlk[ulBlkId].SetNVXOffset(lXOffset);
	}
} //end SetXOffset


VOID CBinBlkMain::SetYOffset(ULONG ulBlkId, LONG lYOffset)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		m_oBinBlk[ulBlkId].SetNVYOffset(lYOffset);
	}
} //end SetYOffset


VOID CBinBlkMain::SetRotateAngleX(ULONG ulBlkId, DOUBLE dRotateAngleX)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		m_oBinBlk[ulBlkId].SetNVRotateAngleX(dRotateAngleX);
	}
} //end SetRotateAngleX


BOOL CBinBlkMain::GrabUseCircularArea(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetUseCircularArea();
	}
	else
	{
		return FALSE;
	}
}

LONG CBinBlkMain::GrabCircularBinRadius(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetRadius();
	}
	else
	{
		return 0;
	}
}

LONG CBinBlkMain::GrabCircularBinCenterX(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetCirBinCenterX();
	}
	else
	{
		return 0;
	}
}

LONG CBinBlkMain::GrabCircularBinCenterY(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetCirBinCenterY();    //pllm
	}	
	else
	{
		return 0;
	}
}

BOOL CBinBlkMain::SetUseCircularBinArea(ULONG ulBlkId, BOOL bEnable, LONG lCenterX, LONG lCenterY, LONG lRadius)
{
	INT i;

	if (ulBlkId == 0)
	{
		for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++) 
		{
			m_oBinBlk[i].SetUseCircularArea(bEnable, lRadius);
			m_oBinBlk[i].SetCirBinCenterXY(lCenterX, lCenterY);
		}
	}
	else
	{
		if ((ulBlkId >= 1) && (ulBlkId <= (BT_MAX_BINBLK_SIZE - 1)))	
		{
			m_oBinBlk[ulBlkId].SetUseCircularArea(bEnable, lRadius);
			m_oBinBlk[ulBlkId].SetCirBinCenterXY(lCenterX, lCenterY);
		}
		else
		{
			return FALSE;
		}
	}	

	return TRUE;
}

UCHAR CBinBlkMain::GrabCurrSortDirection(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_oBinBlk[ulBlkId].GetSortDirection();
	}
	else
	{
		return 0;
	}
}

VOID CBinBlkMain::SetLeaveEmptyRowArray(BOOL bSet)
{
	for (INT i = 0; i < BT_MAX_BINBLK_SIZE; i++)
	{
		if (bSet)
		{
			if (GrabNVNoOfBondedDie(i) > 0)
			{
				//ONly set those bin frames already with dices on it to leave an empty row;
				//No need to do it with EMTPY frames!
				m_bLeaveEmptyRow[i]		= TRUE;
			}
		}
		else
		{
			m_bLeaveEmptyRow[i]		= FALSE;
		}
	}
}

BOOL CBinBlkMain::CheckIfEmptyRow(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		return m_bLeaveEmptyRow[ulBlkId];
	}
	else
	{
		return FALSE;
	}
}

VOID CBinBlkMain::SetLeaveEmptyRow(ULONG ulBlkId, BOOL bSet)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		m_bLeaveEmptyRow[ulBlkId] = bSet;
	}
}

ULONG CBinBlkMain::GetEmptyRow1stRowIndex(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_ul1stRowIndex[ulBlkId];
	}
	return 0;
}

VOID CBinBlkMain::ResetEmptyRow1stRowIndex(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		m_ul1stRowIndex[ulBlkId] = 0;
	}
}

BOOL CBinBlkMain::CheckIfNeedToLeaveEmptyRow(ULONG ulBlkToUse, ULONG &ulNextIndex, BOOL bRealign)
{
	CString szMsg;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ( (pApp->GetCustomerName() != "CyOptics")	&& 
		 (pApp->GetCustomerName() != "FiberOptics") &&		// = Dicon
		 (pApp->GetCustomerName() != "Inari")	)			//v4.51A24		//Not yet available in CTM
	{
		return TRUE;
	}

	if (!CheckIfEmptyRow(ulBlkToUse))
	{
		return TRUE;
	}

	if (ulNextIndex <= 1)		//v4.52A3
	{
		//If no die is bonded (EMPTY frame -> Index = 1), then no need to EMPTY 3 rows
		return TRUE;
	}

	BOOL bIsFiberOptics = FALSE;		// = Dicon
	if (pApp->GetCustomerName() == "FiberOptics")
		bIsFiberOptics = TRUE;


	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulQuotient, ulRemainder;
	
	ULONG ulBinIndex	= ulNextIndex;
	ULONG ulDiePerRow	= GrabNoOfDiePerRow(1);
	ULONG ulDiePerCol	= GrabNoOfDiePerCol(1);
	ULONG ulWalkPath	= GrabWalkPath(1);

	BOOL bDieFound		= FALSE;
	BOOL bEndOfCurrRow	= FALSE;
	BOOL bEndOfNextRow	= FALSE;
	BOOL bEndOf2ndRow	= FALSE;		//v4.51A24		//Avago US //Inari
	BOOL bEndOf3rdRow	= FALSE;		//v4.51A24		//Avago US //Inari

	//v4.40T2
	if (ulNextIndex - 1 < ulDiePerRow)
	{
		//If change new row here with in-complete first-row dices, this may affect
		//3pt bin realignment when finding 2nd die, so need to record 2nd-die index here;
		m_ul1stRowIndex[ulBlkToUse] = ulNextIndex - 1;
	}

	LONG lCount = 1;	//v4.52A4
	CString szLog;

szLog.Format("INARI EMPTY-ROW: Bin #%ld, CurrIndex = %lu, DiePerRow = %lu, DiePerCol = %lu", 
				ulBlkToUse, ulBinIndex, ulDiePerRow, ulDiePerCol);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	do
	{
		if (ulWalkPath == BT_TLV_PATH)				//TL-Vertical
		{
			ulQuotient	= ulBinIndex / ulDiePerCol;
			ulRemainder = ulBinIndex % ulDiePerCol;
		}
		else if (ulWalkPath == BT_TRV_PATH)			//TR-Vertical
		{
			ulQuotient	= ulBinIndex / ulDiePerCol;
			ulRemainder = ulBinIndex % ulDiePerCol;
		}
		else if (ulWalkPath == BT_TRH_PATH)			//TR-Horizontal
		{
			ulQuotient	= ulBinIndex / ulDiePerRow;
			ulRemainder = ulBinIndex % ulDiePerRow;
		}
		else	//if (ulWalkPath == BT_TLH_PATH)	//TL-Horizontal
		{
			ulQuotient	= ulBinIndex / ulDiePerRow;
			ulRemainder = ulBinIndex % ulDiePerRow;
		}


		//if (ulRemainder == 0)	//1)	//Next ROW (or COLUMN) is reached, break the loop!		//v4.31T6 CyOptics
		//{
		//	bDieFound = TRUE;
		//	break;
		//}

		if ( (ulRemainder == 1) && 
			 !bEndOfCurrRow && !bEndOfNextRow && !bEndOf2ndRow && !bEndOf3rdRow)	// &&
			 //(ulBinIndex != ulNextIndex))		//v4.31T11		//v4.52A4
		{
			bEndOfCurrRow = TRUE;
szLog.Format("INARI EMPTY-ROW: Bin #%ld, bEndOfCurrRow at INDEX = %lu", ulBlkToUse, ulBinIndex);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
		else if ((ulRemainder == 1) && bEndOfCurrRow) 
		{
			bEndOfNextRow = TRUE;
			bEndOfCurrRow = FALSE;
szLog.Format("INARI EMPTY-ROW: Bin #%ld, bEndOfNextRow at INDEX = %lu", ulBlkToUse, ulBinIndex);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			if (bIsFiberOptics)		//v4.51A24
			{
				break;
			}
		}
		else if ((ulRemainder == 1) && bEndOfNextRow) 
		{
			bEndOfNextRow	= FALSE;
			bEndOf2ndRow	= TRUE;
szLog.Format("INARI EMPTY-ROW: Bin #%ld, bEndOf2ndRow at INDEX = %lu", ulBlkToUse, ulBinIndex);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		}
		else if ((ulRemainder == 1) && bEndOf2ndRow) 
		{
			bEndOf2ndRow	= FALSE;
			bEndOf3rdRow	= TRUE;
szLog.Format("INARI EMPTY-ROW: Bin #%ld, bEndOf3rdRow at INDEX = %lu", ulBlkToUse, ulBinIndex);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			break;
		}

		ulBinIndex++;
		lCount++;			//v4.52A4
	} 
	while (ulBinIndex <= GrabInputCount((UCHAR)ulBlkToUse));		//v4.52A6
	//while (ulBinIndex <= GrabGradeCapacity(1));

	if (bIsFiberOptics)
	{
		if (!bEndOfNextRow)
		{
			//AfxMessageBox("CyOptics: CheckIfNeedToLeaveEmptyRow = Die not found!", MB_SYSTEMMODAL);
			szLog.Format("DICON EMPTY-ROW ERROR: Bin #%ld, NoOfRow=%ld, NoOfCol=%ld, OrigIndex=%lu, NewIndex=%lu", 
							ulBlkToUse, ulDiePerCol, ulDiePerRow, ulNextIndex, ulBinIndex);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			return FALSE;
		}
	}
	else
	{
		if (!bEndOf3rdRow)
		{
			//AfxMessageBox("CyOptics: CheckIfNeedToLeaveEmptyRow = Die not found!", MB_SYSTEMMODAL);
			szLog.Format("INARI EMPTY-ROW ERROR: Bin #%ld, NoOfRow=%ld, NoOfCol=%ld, OrigIndex=%lu, NewIndex=%lu", 
							ulBlkToUse, ulDiePerCol, ulDiePerRow, ulNextIndex, ulBinIndex);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			return FALSE;
		}
	}

//andrew
if (bRealign)
{
	szLog.Format("INARI EMPTY-ROW (REALIGN): Bin #%ld, NoOfRow=%ld, NoOfCol=%ld, OrigIndex=%lu, NewIndex=%lu", 
					ulBlkToUse, ulDiePerCol, ulDiePerRow, ulNextIndex, ulBinIndex);
}
else
{
	szLog.Format("INARI EMPTY-ROW: Bin #%ld, NoOfRow=%ld, NoOfCol=%ld, OrigIndex=%lu, NewIndex=%lu", 
					ulBlkToUse, ulDiePerCol, ulDiePerRow, ulNextIndex, ulBinIndex);
}
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


	ulNextIndex	= ulBinIndex;
	return TRUE;
}

BOOL CBinBlkMain::SaveLeaveEmptyRowData()
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	for (INT i = 0; i < BT_MAX_BINBLK_SIZE; i++)
	{
		(*pBTfile)["BinBlock"][i]["LeaveEmptyRow"]	= m_bLeaveEmptyRow[i];
		(*pBTfile)["BinBlock"][i]["1stRowIndex"]	= m_ul1stRowIndex[i];
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
}

ULONG CBinBlkMain::GrabSkipUnit(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		return m_oBinBlk[ulBlkId].GetSkipUnit();
	}
	else
	{
		return 0;
	}
} //end GrabSkipUnit

ULONG CBinBlkMain::GrabMaxUnit(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		return m_oBinBlk[ulBlkId].GetMaxUnit();
	}
	else
	{
		return 0;
	}
} //end GrabMaxUnit

BOOL CBinBlkMain::GrabIsTeachWithPhysicalBlk(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetIsTeachWithPhysicalBlk();
}

BOOL CBinBlkMain::GrabIsCentralizedBondArea(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetIsCentralizedBondArea();
}
//end GrabIsCentralizedBondArea


LONG CBinBlkMain::GrabCentralizedOffsetX(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetCentralizedOffsetX();
	}
	else
	{
		return 0;
	}
} //end GrabCentralizedOffsetX

LONG CBinBlkMain::GrabCentralizedOffsetY(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetCentralizedOffsetY();
	}
	else
	{
		return 0;
	}
} //end GrabCentralizedOffsetY

BOOL CBinBlkMain::GrabIsUseBlockCornerAsFirstDiePos(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetIsUseBlockCornerAsFirstDiePos();
	}
	else
	{
		return FALSE;
	}
}

BOOL CBinBlkMain::GrabNoReturnTravel(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetNoReturnTravel();
	}
	else
	{
		return FALSE;
	}
}

BOOL CBinBlkMain::GrabIsEnableWafflePad(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetIsEnableWafflePad();
	}
	else
	{
		return 0;
	}
}

BOOL CBinBlkMain::GrabWafflePadSizeX(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetWafflePadSizeX();
	}
	else
	{
		return 0;
	}
}

BOOL CBinBlkMain::GrabWafflePadSizeY(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetWafflePadSizeY();
	}
	else
	{
		return 0;
	}
}

BOOL CBinBlkMain::GrabWafflePadDistX(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetWafflePadDistX();
	}
	else
	{
		return 0;
	}
}

BOOL CBinBlkMain::GrabWafflePadDistY(ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetWafflePadDistY();
	}
	else
	{
		return 0;
	}
}

DOUBLE CBinBlkMain::GrabThermalDeltaPitchX(ULONG ulBlkId)	//v4.59A22
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetThermalDeltaPitchX();
	}
	else
	{
		return 0;
	}
}

DOUBLE CBinBlkMain::GrabThermalDeltaPitchY(ULONG ulBlkId)	//v4.59A22
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].GetThermalDeltaPitchY();
	}
	else
	{
		return 0;
	}
}

DOUBLE CBinBlkMain::FindThermalDeltaPitchX(ULONG ulBlkId, LONG lCurrCol)	//v4.59A22
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].CalculateThermalDeltaPitchX(lCurrCol);
	}
	else
	{
		return 0;
	}
}

DOUBLE CBinBlkMain::FindThermalDeltaPitchY(ULONG ulBlkId, LONG lCurrRow)	//v4.59A22
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinBlk[ulBlkId].CalculateThermalDeltaPitchY(lCurrRow);
	}
	else
	{
		return 0;
	}
}

VOID CBinBlkMain::SetThermalDeltaPitch(ULONG ulBlkId, DOUBLE dX, DOUBLE dY)		//v4.59A22
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		m_oBinBlk[ulBlkId].SetThermalDeltaPitch(dX, dY);
	}
}



/******************************************/
/*    Grab CBinGradeData Class members    */
/******************************************/
VOID CBinBlkMain::SetAllGradeIsAssigned(BOOL bEnable)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
			
	if (pBTfile == NULL)
	{
		return;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		m_oBinGradeData[i].SetIsAssigned(bEnable);
		(*pBTfile)["BinGradeData"][i]["IsAssigned"] = bEnable;
	}

	CMSFileUtility::Instance()->SaveBTConfig();
}


VOID CBinBlkMain::SetGradeIsAssigned(UCHAR ucGrade, BOOL bEnable, CStringMapFile *pBTfile)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		//v4.52A8	//reduce no of OPEN/CLOSE access of MSD file
		//CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();	
		CStringMapFile *pBTMSDfile = pBTfile;
		if (pBTfile == NULL)
		{
			pBTMSDfile = CMSFileUtility::Instance()->GetBTConfigFile();
		}

		m_oBinGradeData[ucGrade].SetIsAssigned(bEnable);
		if (pBTMSDfile != NULL)		//v4.53	Klocwork
		{
			(*pBTMSDfile)["BinGradeData"][ucGrade]["IsAssigned"] = bEnable;
		}

		if (pBTfile == NULL)
		{
			CMSFileUtility::Instance()->SaveBTConfig();
		}
	}
}

BOOL CBinBlkMain::GrabIsAssigned(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		return m_oBinGradeData[ucGrade].GetIsAssigned();
	}
	else
	{
		return FALSE;
	}
} //end GrabIsAssigned


ULONG CBinBlkMain::GrabGradeCapacity(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		return m_oBinGradeData[ucGrade].GetGradeCapacity();
	}
	else 
	{
		return 0;
	}
} //end GrabGradeCapacity


ULONG CBinBlkMain::GrabInputCount(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		return m_oBinGradeData[ucGrade].GetInputCount();
	}
	else
	{
		return 0;
	}
} //end GrabInputCount


UCHAR CBinBlkMain::GrabAliasGrade(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		return m_oBinGradeData[ucGrade].GetAliasGrade();
	}
	else
	{
		return 0;
	}
} //end GrabAliasGrade


ULONG CBinBlkMain::GrabStatRowIndex(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))		//v3.35
	{
		return m_oBinGradeData[ucGrade].GetStatRowIndex();
	}
	else
	{
		return 0;
	}
} //end GrabStatRowIndex


ULONG CBinBlkMain::GrabNVNoOfSortedDie(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinGradeData[ucGrade].GetNVNoOfSortedDie();
	}
	else 
	{
		return 0;
	}
} //end GrabNVNoOfSortedDie


ULONG CBinBlkMain::GrabNVBlkInUse(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinGradeData[ucGrade].GetNVBlkInUse();
	}
	else
	{
		return 0;
	}
} //end GrabNVBlkInUse


BOOL CBinBlkMain::SetIntputCount(UCHAR ucGrade, ULONG ulInputCount)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		
		if (pBTfile == NULL)
		{
			return FALSE;
		}

		m_oBinGradeData[ucGrade].SetInputCount(ulInputCount);

		(*pBTfile)["BinGradeData"][ucGrade]["InputCount"] = ulInputCount;

		CMSFileUtility::Instance()->SaveBTConfig();

		return TRUE;
	}

	return FALSE;
}

BOOL CBinBlkMain::UpdateGradeInputCount(UCHAR ucGrade, ULONG ulInputCount)
{
	if (ulInputCount > GrabGradeCapacity(ucGrade))
	{
		ulInputCount = GrabGradeCapacity(ucGrade);
	}

	SetIntputCount(ucGrade, ulInputCount);


	return TRUE;
}

BOOL CBinBlkMain::SetLotRemainCount(UCHAR ucGrade, ULONG ulRemainCount)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		CMSFileUtility  *pUtl = CMSFileUtility::Instance();
		CStringMapFile  *psmf;
	
		if (pUtl->LoadLastState("BBX: SetLotRemainCount") == FALSE) 	// open LastState file
		{
			return FALSE;
		}

		psmf = pUtl->GetLastStateFile("BBX: SetLotRemainCount");		
		
		if (psmf != NULL)
		{
			(*psmf)["BinGradeData"][ucGrade]["Lot Remain Count"] = ulRemainCount;
			
		}

		m_oBinGradeData[ucGrade].SetLotRemainingCount(ulRemainCount);

		pUtl->UpdateLastState("BBX: SetLotRemainCount");
		pUtl->CloseLastState("BBX: SetLotRemainCount");

		return TRUE;
	}

	return FALSE;
}

ULONG CBinBlkMain::GetMinFrameDieCount(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinGradeData[ucGrade].GetMinFrameDieCount();
	}

	return 0;
}

ULONG CBinBlkMain::GetMaxFrameDieCount(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinGradeData[ucGrade].GetMaxFrameDieCount();
	}

	return 0;
}

BOOL CBinBlkMain::SetMinFrameDieCount(UCHAR ucGrade, ULONG ulMinFrameDieCount)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		
		if (pBTfile == NULL)
		{
			return FALSE;
		}
		
		//m_oBinGradeData[ucGrade].UpdateFrameMinDieCount();
		m_oBinGradeData[ucGrade].SetMinFrameDieCount(ulMinFrameDieCount);
		(*pBTfile)["BinGradeData"][ucGrade]["Min Frame Die Count"] = m_oBinGradeData[ucGrade].GetMinFrameDieCount();
			
		CMSFileUtility::Instance()->SaveBTConfig();
		return TRUE;
	}

	return FALSE;
}

BOOL CBinBlkMain::CheckOptimizeBinCountSetting(UCHAR &ucGrade)
{
	ULONG ulMaxFrameDieCount = 0;

	for (ULONG j = 1; j <= GetNoOfBlk(); j++)
	{
		j = min(j, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		ucGrade = m_oBinBlk[j].GetGrade();

		if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
		{
			ulMaxFrameDieCount = m_oBinGradeData[ucGrade].GetMaxFrameDieCount();
		
			if (ulMaxFrameDieCount > m_oBinGradeData[ucGrade].GetGradeCapacity())
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CBinBlkMain::SetMaxFrameDieCount(UCHAR ucGrade, ULONG ulMaxFrameDieCount)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		
		if (pBTfile == NULL)
		{
			return FALSE;
		}
		
		m_oBinGradeData[ucGrade].SetMaxFrameDieCount(ulMaxFrameDieCount);
		(*pBTfile)["BinGradeData"][ucGrade]["Max Frame Die Count"] = m_oBinGradeData[ucGrade].GetMaxFrameDieCount();
			
		CMSFileUtility::Instance()->SaveBTConfig();

		return TRUE;
	}
	
	return FALSE;
}


BOOL CBinBlkMain::UpdateAllOptimizeMinCount(ULONG ulMinFrameDieCount)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		m_oBinGradeData[i].SetMinFrameDieCount(ulMinFrameDieCount);
		(*pBTfile)["BinGradeData"][i]["Min Frame Die Count"] = m_oBinGradeData[i].GetMinFrameDieCount();
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
}

BOOL CBinBlkMain::UpdateAllOptimizeMaxCount(ULONG ulMaxFrameDieCount)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		m_oBinGradeData[i].SetMaxFrameDieCount(ulMaxFrameDieCount);
		(*pBTfile)["BinGradeData"][i]["Max Frame Die Count"] = m_oBinGradeData[i].GetMaxFrameDieCount();
	}

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
}


/*********************************/
/*   Set CBinBlk Class Members   */
/*********************************/
VOID CBinBlkMain::SetIsBinBlkAssignedWithSlot(ULONG ulBinBlkId, BOOL bIsAssignedWithSlot)
{
	if ((ulBinBlkId >= 1) && (ulBinBlkId <= BT_MAX_BINBLK_SIZE - 1))
	{
		m_oBinBlk[ulBinBlkId].SetIsAssignedWithSlot(bIsAssignedWithSlot);
	}
} //end SetIsBinBlkAssignedWithSlot


VOID CBinBlkMain::SetStatus(ULONG ulBlkId, UCHAR ucStatus)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return;
	}

	(*pBTfile)["BinBlock"][ulBlkId]["Status"] = ucStatus;

	m_oBinBlk[ulBlkId].SetStatus(ucStatus);

	CMSFileUtility::Instance()->SaveBTConfig();

} //end SetStatus


/***************************************/
/*   Set CBinGradeData Class Members   */
/***************************************/
VOID CBinBlkMain::SetStatRowIndexPerGrade(UCHAR ucGrade, ULONG ulStatRowIndex)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		m_oBinGradeData[ucGrade].SetStatRowIndex(ulStatRowIndex);
	}
} //end SetStatRowIndexPerGrade

/******************************************/
/*    Check CBinGradeData Class members   */
/******************************************/

BOOL CBinBlkMain::IsLastPos(UCHAR ucGrade)
{
	if (m_oBinGradeData[ucGrade].GetInputCount() == (m_oBinGradeData[ucGrade].GetNVNoOfSortedDie() + 1))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} //end IsLastPos


BOOL CBinBlkMain::CheckIfGradesAreStillValid()
{
	ULONG i, j;
	BOOL bIsValidGrade = TRUE;	//Klocwork
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
	{
		for (j = 1; j <= GetNoOfBlk(); j++)
		{
			bIsValidGrade = FALSE;
		
			j = min(j, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[j].GetGrade() == UCHAR(i))
			{
				bIsValidGrade = TRUE;
				break;
			}
		}

		if (!bIsValidGrade)
		{
			//IsAssigned
			m_oBinGradeData[i].SetIsAssigned(FALSE);
			(*pBTfile)["BinGradeData"][i]["IsAssigned"] = 
				m_oBinGradeData[i].GetIsAssigned();
			
			//GradeCapacity
			m_oBinGradeData[i].SetGradeCapacity(0);
			(*pBTfile)["BinGradeData"][i]["GradeCapacity"] = 
				m_oBinGradeData[i].GetGradeCapacity();

			//InputCount
			m_oBinGradeData[i].SetInputCount(0);
			(*pBTfile)["BinGradeData"][i]["InputCount"] = 
				m_oBinGradeData[i].GetInputCount();

			//AliasGrade
			m_oBinGradeData[i].SetAliasGrade(0);
			(*pBTfile)["BinGradeData"][i]["AliasGrade"] = 
				m_oBinGradeData[i].GetAliasGrade();

			m_oBinGradeData[i].SetNVBlkInUse(0);
			m_oBinGradeData[i].SetNVNoOfSortedDie(0);
			m_oBinGradeData[i].SetNVIsFull(FALSE);
		}
	}

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
} //end CheckIfGradesAreStillValid


/******************************************/
/*    Set & grab RankID					  */
/******************************************/

BOOL CBinBlkMain::ResetRankID()
{
	UCHAR ucGrade;

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	for (ucGrade = 1; ucGrade <= BT_MAX_BINBLK_SIZE - 1; ucGrade++)
	{
		m_oBinGradeData[ucGrade].SetRankID("");
		(*pBTfile)["BinGradeData"][ucGrade]["RankID"] = "";

		m_oBinGradeData[ucGrade].SetBlockSize("");
		(*pBTfile)["BinGradeData"][ucGrade]["BlockSize"] = "";
	}

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}// end SetRankID



BOOL CBinBlkMain::SetRankID(UCHAR ucGrade, CString szRankID)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		if (pBTfile == NULL)
		{
			return FALSE;
		}

		m_oBinGradeData[ucGrade].SetRankID(szRankID);

		(*pBTfile)["BinGradeData"][ucGrade]["RankID"] = szRankID;

		CMSFileUtility::Instance()->SaveBTConfig();

		return TRUE;
	}

	return FALSE;
}// end SetRankID

CString CBinBlkMain::GrabRankID(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinGradeData[ucGrade].GetRankID();
	}
	
	return NULL;
}// end GrabRankID

BOOL CBinBlkMain::CheckValidRankID(UCHAR ucGrade)			//v4.21T4	//TongFang RankID checking fcn
{
	CString szRankID = "";

	if ((ucGrade < 1) || (ucGrade > BT_MAX_BINBLK_SIZE - 1))
	{
		return FALSE;
	}
		
	szRankID = m_oBinGradeData[ucGrade].GetRankID();

	if (szRankID.GetLength() > 0)
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CBinBlkMain::SetBlockSize(UCHAR ucGrade, CString szBlockSize)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		if (pBTfile == NULL)
		{
			return FALSE;
		}

		m_oBinGradeData[ucGrade].SetBlockSize(szBlockSize);

		(*pBTfile)["BinGradeData"][ucGrade]["BlockSize"] = szBlockSize;

		CMSFileUtility::Instance()->SaveBTConfig();

		return TRUE;
	}

	return FALSE;
}

CString CBinBlkMain::GrabBlockSize(UCHAR ucGrade)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		return m_oBinGradeData[ucGrade].GetBlockSize();
	}
	
	return NULL;
}

BOOL CBinBlkMain::SetLifeTime(CString szLifeTime)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}


	m_szLifeTime = szLifeTime;

	(*pBTfile)["BinBlock"]["LifeTime"] = m_szLifeTime;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}// end SetLifeTime


CString CBinBlkMain::GrabLifeTime()
{
	return m_szLifeTime;
}// end GrabLifeTime


BOOL CBinBlkMain::SetESD(CString szESD)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_szESD = szESD;

	(*pBTfile)["BinBlock"]["ESD"] = m_szESD;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}// end SetESD

CString CBinBlkMain::GrabESD()
{
	return m_szESD;
}// end GrabESD


BOOL CBinBlkMain::SetIF(CString szIF)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_szIF = szIF;

	(*pBTfile)["BinBlock"]["IF"] = m_szIF;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}// end SetIF


CString CBinBlkMain::GrabIF()
{
	return m_szIF;
}// end GrabIF


BOOL CBinBlkMain::SetTapeID(CString szTapeID)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_szTapeID = szTapeID;

	(*pBTfile)["BinBlock"]["TapeID"] = m_szTapeID;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}// end SetESD

CString CBinBlkMain::GrabTapeID()
{
	return m_szTapeID;
}// end GrabTapeID


BOOL CBinBlkMain::SetBinTableFilename(CString szBinTableFilename)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_szBinTableFilename = szBinTableFilename;

	(*pBTfile)["BinBlock"]["BinTableFilename"] = m_szBinTableFilename;

	CMSFileUtility::Instance()->SaveBTConfig();
	return TRUE;
}

CString CBinBlkMain::GrabBinTableFilename()
{
	return m_szBinTableFilename;
}

BOOL CBinBlkMain::SetSerialNoFormat(ULONG ulSerialFormat)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_ulSerialNoFormat = ulSerialFormat;

	(*pBTfile)["BinBlock"]["SerialNoFormat"] = m_ulSerialNoFormat;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}// end SetSerialNoFormat


ULONG CBinBlkMain::GrabSerialNoFormat()
{
	if (m_bDisableBinSNRFormat == TRUE)
	{
		return m_ulDefaultBinSNRFormat;
	}

	return m_ulSerialNoFormat;
}// end GrabSerialNoFormat


BOOL CBinBlkMain::SetSerialNoMaxLimit(ULONG ulMaxLimit)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_ulSerialNoMaxLimit = ulMaxLimit;

	(*pBTfile)["BinBlock"]["SerialNoMaxLimit"] = m_ulSerialNoMaxLimit;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}//end SetSerialNoMaxLimit


ULONG CBinBlkMain::GrabSerialNoMaxLimit()
{
	return m_ulSerialNoMaxLimit;
}//end GrabSerialNoMaxLimit



BOOL CBinBlkMain::SetBinClearFormat(ULONG ulFormat)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_ulBinClearFormat = ulFormat;

	(*pBTfile)["BinBlock"]["Bin Clear Format"] = m_ulBinClearFormat;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}//end SetBinClearFormat


ULONG CBinBlkMain::GrabBinClearFormat()
{
	if (m_bDisableClearCountFormat == TRUE)
	{
		return m_ulDefaultClearCountFormat;
	}

	return m_ulBinClearFormat;
}//end GrabBinClearFormat


BOOL CBinBlkMain::SetBinClearInitCount(ULONG ulInitCount)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_ulBinClearInitCount = ulInitCount;

	(*pBTfile)["BinBlock"]["Bin Clear InitCount"] = m_ulBinClearInitCount;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}
//end SetBinClearInitCount


ULONG CBinBlkMain::GrabBinClearInitCount()
{
	return m_ulBinClearInitCount;
}//end GrabBinClearInitCount


BOOL CBinBlkMain::SetBinClearMaxLimit(ULONG ulMaxLimit)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_ulBinClearMaxLimit = ulMaxLimit;

	(*pBTfile)["BinBlock"]["Bin Clear MaxLimit"] = m_ulBinClearMaxLimit;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}//end SetBinClearMaxLimit


ULONG CBinBlkMain::GrabBinClearMaxLimit()
{
	return m_ulBinClearMaxLimit;
}//end GrabBinClearMaxLimit


BOOL CBinBlkMain::ResetBinClearedCount()
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_ulBinClearedCount = GrabBinClearInitCount();
	
	(*pBTfile)["BinBlock"]["Bin Clear Count"] = m_ulBinClearedCount;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}//end ResetBinClearedCount


CString CBinBlkMain::GrabBinClearedCount()
{
	int nCurrYear, nCurrMonth, nCurrDay;
	int nCol = -1;
	CString szCurrDate;
	CString szReturn = "";
	CTime theTime;

	theTime		= CTime::GetCurrentTime(); 
	nCurrYear	= theTime.GetYear();
	nCurrMonth	= theTime.GetMonth();
	nCurrDay	= theTime.GetDay();

	szCurrDate.Format("%d-%d-%d", nCurrYear, nCurrMonth, nCurrDay);

	if (GrabBinClearFormat() == BT_CLEAR_BIN_COUNT_DAILY)
	{
		//Reset by date
		if (szCurrDate != m_szBinClearedDate)
		{
			ResetBinClearedCount();
		}
	}
	else if (GrabBinClearFormat() == BT_CLEAR_BIN_COUNT_HIT_LIMIT)
	{
		//Reset when hit limit 
		if (m_ulBinClearedCount > GrabBinClearMaxLimit())
		{
			ResetBinClearedCount();
		}
	}
	else if (GrabBinClearFormat() == BT_CLEAR_BIN_COUNT_MONTHLY)
	{
		CString szCurMonth;
		CString szPrevYear, szPrevMonth, szPrevDay, szTemp;

		szCurMonth.Format("%d", nCurrMonth);
		szTemp = m_szBinClearedDate;

		nCol = szTemp.Find("-");

		if (nCol != -1)
		{
			szPrevYear	= szTemp.Left(nCol);
			szTemp		= szTemp.Right(szTemp.GetLength() - nCol - 1);

			nCol = szTemp.Find("-");

			if (nCol != -1)
			{
				szPrevMonth = szTemp.Left(nCol);
				szPrevDay	= szTemp.Right(szTemp.GetLength() - nCol - 1);
			}
		}

		if (szPrevMonth.IsEmpty() == FALSE)
		{
			if (szPrevMonth != szCurMonth)
			{
				ResetBinClearedCount();
			}
		}
	}
	else if (GrabBinClearFormat() == BT_CLEAR_BIN_COUNT_LOAD_BIN_SUMMARY)
	{
	}

	szReturn.Format("%d", m_ulBinClearedCount);
	if (m_ulBinClearedCount < 10)
	{
		szReturn = "0" + szReturn;
	}

	m_szBinClearedDate = szCurrDate;

	m_ulBinClearedCount++;

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return szReturn;
	}

	(*pBTfile)["BinBlock"]["Bin Clear Count"]	= m_ulBinClearedCount;
	(*pBTfile)["BinBlock"]["Bin Clear Date"]	= m_szBinClearedDate;

	CMSFileUtility::Instance()->SaveBTConfig();

	return szReturn;
}// end UpdateBinClearedCount


VOID CBinBlkMain::SetAccCountOption(BOOL bStore)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return;
	}

	m_bStoreBinBondedCount = bStore;

	(*pBTfile)["BinBlock"]["Store Acc. Bonded Count"] = m_bStoreBinBondedCount;

	CMSFileUtility::Instance()->SaveBTConfig();

}//end SetAccCountOption


BOOL CBinBlkMain::GrabAccCountOption()
{
	return m_bStoreBinBondedCount;
}//end GrabAccCountOption


VOID CBinBlkMain::SetBinAccBondedCount(ULONG ulBlkId, ULONG ulCount, CStringMapFile *pBTMSDfile)
{
	CStringMapFile *pBTfile = pBTMSDfile;
	if (pBTMSDfile == NULL)
	{
		pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	}

	if (pBTfile != NULL)
	{
		(*pBTfile)["BinBlock"][ulBlkId]["Accumulated"] = ulCount;
	}

	m_oBinBlk[ulBlkId].SetAccmulatedCount(ulCount);

	if (pBTMSDfile == NULL)
	{
		CMSFileUtility::Instance()->SaveBTConfig();
	}

}//end SetBinAccBondedCount


ULONG CBinBlkMain::GrabBinAccBondedCount(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetAccmulatedCount();
}//end GrabBinAccBondedCount


VOID CBinBlkMain::SetAutoAssignGrade(BOOL bEnable)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return;
	}

	m_bEnableAutoAssignGrade = bEnable;

	(*pBTfile)["BinBlock"]["Enable Auto Ass. Grade"] = m_bEnableAutoAssignGrade;

	CMSFileUtility::Instance()->SaveBTConfig();

}//end SetAutoAssignGrade


BOOL CBinBlkMain::GrabAutoAssignGrade()
{
	return m_bEnableAutoAssignGrade;
}//end GrabAutoAssignGrade


/**************************************************/
/*  Functions of NVRAM pointer for RunTime Data   */
/**************************************************/
BOOL CBinBlkMain::SetPSTNVRunTimeData(void *pvNVRAM, LONG lNVRAM_BinTable_Start, 
									  LONG lNVRAM_BinTable_Size)
{
	ULONG i;
	BT_NVRUNTIMEDATA *pTemp;

	m_pNvRunTimeCounter = (ULONG*)((ULONG)pvNVRAM + 
								   lNVRAM_BinTable_Start + (lNVRAM_BinTable_Size / 2) 
								   + (lNVRAM_BinTable_Size / 4));

	//Use 3K NVRam Space
	pTemp = (BT_NVRUNTIMEDATA*)((ULONG)pvNVRAM + 
								lNVRAM_BinTable_Start + (lNVRAM_BinTable_Size / 2) 
								+ (lNVRAM_BinTable_Size / 4) 
								+ sizeof(ULONG));

	for (i = 0; i < MAX_RT_LIMIT; i++)
	{
		m_pstNvRunTimeData[i] = pTemp + i;
	}

	return TRUE;
}

VOID CBinBlkMain::ResetNVRunTimeData()
{
	ULONG i;
	CString szFileName;

	*m_pNvRunTimeCounter = 0;
	for (i = 0; i < MAX_RT_LIMIT; i++)
	{
		m_pstNvRunTimeData[i]->ucBlkNo = 0;
		m_pstNvRunTimeData[i]->ucGrade = 0;
		m_pstNvRunTimeData[i]->ssWaferMapX = 0;
		m_pstNvRunTimeData[i]->ssWaferMapY = 0;
	}

	//Remove files
	if (CMS896AApp::m_bEnableMachineLog == FALSE)
	{
		DeleteFile(gszUSER_DIRECTORY + "\\NVRAM\\NV_Data.txt");
		for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
		{
			szFileName.Format("NV_%d.txt", i);
			DeleteFile(gszUSER_DIRECTORY + "\\NVRAM\\" + szFileName);
		}
	}
}


LONG CBinBlkMain::SaveNVRunTimeData(ULONG ulBlkId, UCHAR ucGrade, LONG lWaferX, LONG lWaferY)
{
	CString szLogMsg = "";

	m_pstNvRunTimeData[*m_pNvRunTimeCounter]->ucBlkNo = (UCHAR)ulBlkId;
	m_pstNvRunTimeData[*m_pNvRunTimeCounter]->ucGrade = ucGrade;
	m_pstNvRunTimeData[*m_pNvRunTimeCounter]->ssWaferMapX = (SHORT)lWaferX;
	m_pstNvRunTimeData[*m_pNvRunTimeCounter]->ssWaferMapY = (SHORT)lWaferY;

	if (*m_pNvRunTimeCounter < (m_ulBondCountToGenTempFile - 1)) //MAX_RT_LIMIT
	{
		*m_pNvRunTimeCounter = *m_pNvRunTimeCounter + 1; 
		return 0;
	}
	else
	{
		CTime theTime = CTime::GetCurrentTime(); 
		szLogMsg.Format("%2d %2d:%2d:%2d - Start GenAll TempFiles\n", 
						theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);

		DOUBLE dDuration = GetTime();		//v4.47T8
		LONG lRet = GenAllTempFiles();
		dDuration = GetTime() - dDuration;

		szLogMsg.Format("%2d %2d:%2d:%2d - Complete GenAll TempFiles (%.1f ms)\n",
						theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), dDuration);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);

		dDuration = GetTime();		//v4.47T8
		ResetNVRunTimeData();
		dDuration = GetTime() - dDuration;

		szLogMsg.Format("%2d %2d:%2d:%2d - Complete Reset NVRunTimeData (%.1f ms)\n",
						theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), dDuration);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);

		if (lRet == BT_BACKUP_NVRAM_ERROR)
		{
			return lRet;
		}
		return BT_BACKUP_NVRAM;
	}

	return 0;
}

BOOL CBinBlkMain::RestoreNVRunTimeData()
{
#ifdef OFFLINE
	return TRUE;
#else
	BOOL bReturn = RestoreNVRunTimeData_Main();
	if (bReturn)	//v4.22T2
	{
		ResetNVRunTimeData();    //Reset program memory only, not NVRAM
	}		
	return bReturn;
#endif
}

BOOL CBinBlkMain::RestoreNVRunTimeData_Main()
{
	ULONG i, ulTotal;
	UCHAR ucBlk;
	CString szFileName;
	CString szLog;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	//Log history into file
	CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("Init NV Ram");
	//if ( (fNVLog = fopen(gszUSER_DIRECTORY + "\\NVRAM\\NV_History.txt", "a+")) != NULL )
	//{
	//	CTime theTime = CTime::GetCurrentTime(); 
	//	fprintf(fNVLog, "%d-%2d-%2d (%2d:%2d:%2d) - Init NV Ram\n", 
	//								theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
	//	fclose(fNVLog);
	//}


	//Init RunTime Data
	SetPSTNVRunTimeData(m_pvNVRAM, m_lNVRAM_BinTable_Start, glNVRAM_BinTable_Size);

	//Check Counter must less than MAX_RT_LIMIT & Map should be valid, otherwise Reset all value
	if (*m_pNvRunTimeCounter >= MAX_RT_LIMIT)
	{
		CTime theTime = CTime::GetCurrentTime(); 
		szLog.Format("%d-%2d-%2d (%2d:%2d:%2d) - Exceed Buffer Size", 
					 theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		pApp->SetErrorMessage(szLog);

		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("Exceed Buffer Size\n");
		//if ( (fNVLog = fopen(gszUSER_DIRECTORY + "\\NVRAM\\NV_History.txt", "a+")) != NULL )
		//{
		//	fprintf(fNVLog, "%s\n", szLog);
		//	fclose(fNVLog);
		//}
		return FALSE;
	}

	if (m_bEnableNVRunTimeData != TRUE)
	{
		pApp->SetErrorMessage("Restore NV Run Time Data not enable");
		return FALSE;
	}


	//v4.22T2
	// if current map is not valid
	if (CMS896AStn::m_WaferMapWrapper.IsMapValid() == FALSE) 
	{
		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("warning: Map is not valid");
	}
	if (CMS896AStn::m_bWaferMapWarmStartFail == TRUE)
	{
		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("warning: Map memory is corrupted (WARM-START fail)");
	}


	//Check Machine is not properly stop or not - NVRAM may have run time data & gen tmp stil not generated
	if ((*m_pNvRunTimeCounter <= 0))
	{
		CTime theTime = CTime::GetCurrentTime(); 
		szLog.Format("%d-%2d-%2d (%2d:%2d:%2d) - No data need to recover", 
					 theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		pApp->SetErrorMessage(szLog);
		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("No data need to recover");
		//if ( (fNVLog = fopen(gszUSER_DIRECTORY + "\\NVRAM\\NV_History.txt", "a+")) != NULL )
		//{
		//	fprintf(fNVLog, "%s\n", szLog);
		//	fclose(fNVLog);
		//}

		//v4.22T2
		CMS896AStn::m_bWaferMapWarmStartFail	= FALSE;
		CMS896AStn::m_bRestoreNVRamInBondPage	= FALSE;
		return TRUE;	//FALSE;		//v4.22T2
	}

	//Log history into file
	CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("Restore files");
	//if ( (fNVLog = fopen(gszUSER_DIRECTORY + "\\NVRAM\\NV_History.txt", "a+")) != NULL )
	//{
	//	CTime theTime = CTime::GetCurrentTime(); 
	//	fprintf(fNVLog, "%d-%2d-%2d (%2d:%2d:%2d) - Restore files\n", 
	//								theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
	//	fclose(fNVLog);
	//}

	//Dump all data into files for debug
	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, gszUSER_DIRECTORY + "\\NVRAM\\NV_Data.txt", "a+");
	if ((nErr == 0) && (fp != NULL))
	{
		CTime theTime = CTime::GetCurrentTime(); 
		fprintf(fp, "%d-%2d-%2d (%2d:%2d:%2d) - Start Record\n", 
				theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		fprintf(fp, "[BT CurrBond]\n");
		for (i = 1; i <= BT_MAX_BINBLK_SIZE - 1; i++)
		{
			fprintf(fp, "%ld:%ld,%ld\n", i, GrabNVCurrBondIndex(i), GrabNVLastFileSaveIndex(i));
		}

		fprintf(fp, "[NV Count]\n");
		fprintf(fp, "%ld\n", *m_pNvRunTimeCounter);

		fprintf(fp, "[NV Data]\n");

		for (i = 0; i < *m_pNvRunTimeCounter; i++)
		{
			fprintf(fp, "%lu:%d,%d,%d,%d\n",
					i + 1,
					m_pstNvRunTimeData[i]->ucBlkNo,
					m_pstNvRunTimeData[i]->ucGrade,
					m_pstNvRunTimeData[i]->ssWaferMapX,
					m_pstNvRunTimeData[i]->ssWaferMapY);
		}
		fclose(fp);
	}


	// if current map is not valid 
	if ((CMS896AStn::m_WaferMapWrapper.IsMapValid() == FALSE) || (CMS896AStn::m_bWaferMapWarmStartFail))
	{
		CTime theTime = CTime::GetCurrentTime(); 
		szLog.Format("%d-%2d-%2d (%2d:%2d:%2d) - Map is not valid (RestoreNVRunTimeData_Main)", 
					 theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		pApp->SetErrorMessage(szLog);	
		CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("Map is not valid (RestoreNVRunTimeData_Main)\n");
		//if ( (fNVLog = fopen(gszUSER_DIRECTORY + "\\NVRAM\\NV_History.txt", "a+")) != NULL )
		//{
		//	fprintf(fNVLog, "%s\n", szLog); 
		//	fclose(fNVLog);
		//}
		CMS896AStn::m_bWaferMapWarmStartFail	= FALSE;
		CMS896AStn::m_bRestoreNVRamInBondPage	= TRUE;
		return FALSE;
	}


	//Divide NV Raw data into corresponding Binblk File
	for (ucBlk = 1; ucBlk <= BT_MAX_BINBLK_SIZE - 1; ucBlk++)
	{
		ulTotal = 0;
		szFileName.Format("NV_%d.txt", ucBlk);
		errno_t nErr = fopen_s(&fp, gszUSER_DIRECTORY + "\\NVRAM\\" + szFileName, "w");

		//write total no of this blk
		for (i = 0; i < *m_pNvRunTimeCounter; i++)
		{
			if (ucBlk == m_pstNvRunTimeData[i]->ucBlkNo)
			{
				ulTotal++;
			}
		}

		if ((nErr == 0) && (fp != NULL))
		{
			fprintf(fp, "Total No of Die,%lu\n", ulTotal);  
		}

		if (ulTotal > 0)
		{
			//write this blk data into file
			for (i = 0; i < *m_pNvRunTimeCounter; i++)
			{
				if ((ucBlk == m_pstNvRunTimeData[i]->ucBlkNo) && (nErr == 0) && (fp != NULL))
				{
					fprintf(fp, "%d,%d,%d\n", 
							m_pstNvRunTimeData[i]->ucGrade,
							m_pstNvRunTimeData[i]->ssWaferMapX,
							m_pstNvRunTimeData[i]->ssWaferMapY);
				}
			}
		}

		if ((nErr == 0) && (fp != NULL))
		{
			fclose(fp);
		}
	}


	//Log history into file
	CTime theTime = CTime::GetCurrentTime(); 
	szLog.Format("%d-%2d-%2d (%2d:%2d:%2d) - Append BT temp files start", 
				 theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
	pApp->SetErrorMessage(szLog);
	CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("Append BT temp files start");
	//if ( (fNVLog = fopen(gszUSER_DIRECTORY + "\\NVRAM\\NV_History.txt", "a+")) != NULL )
	//{
	//	fprintf(fNVLog, "%s\n", szLog);
	//	fclose(fNVLog);
	//}


	//Append NV data into corresponding BT temp files
	GenAllTempFiles(TRUE);


	theTime = CTime::GetCurrentTime(); 
	szLog.Format("%d-%2d-%2d (%2d:%2d:%2d) - Append BT temp files finish", 
				 theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
	pApp->SetErrorMessage(szLog);
	//Log history into file
	CMSLogFileUtility::Instance()->MS_LogNVRAMHistory("Append BT temp files finish");
	//if ( (fNVLog = fopen(gszUSER_DIRECTORY + "\\NVRAM\\NV_History.txt", "a+")) != NULL )
	//{
	//	fprintf(fNVLog, "%s\n", szLog);
	//	fclose(fNVLog);
	//}

	CMS896AStn::m_bRestoreNVRamInBondPage = FALSE;

	return TRUE;
}


BOOL CBinBlkMain::EnableNVRunTimeData(BOOL bEnable)
{	
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return FALSE;
	}

	m_bEnableNVRunTimeData = bEnable;

	(*pBTfile)["BinBlock"]["Enable NVProtect"] = m_bEnableNVRunTimeData;

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}


/************************************/
/*  Set use FileName as WaferID     */
/************************************/
VOID CBinBlkMain::SetFileNameAsWaferID(BOOL bAsWaferID)
{	
	m_bFilenameASWaferID = bAsWaferID;
}


/************************************************/
/*  Read BinBlk barcode from SMF file (DL only) */
/************************************************/
CString CBinBlkMain::GetBinBlkCatcode(ULONG ulBlkNo)
{
	CStringMapFile pSmfFile;
	CString szCatcode;


	szCatcode.Empty();

	if ((_access(MSD_PLLM_CATCODE_FILE, 0)) == -1)
	{
		//File not exist when in standard MS899 machine
	}
	else
	{
		if (pSmfFile.Open(MSD_PLLM_CATCODE_FILE, FALSE, FALSE) == 1)
		{
			szCatcode = (pSmfFile)[ulBlkNo]["Catcode"];
			pSmfFile.Close();
		}
	}

	return szCatcode;
}


BOOL CBinBlkMain::ResetBinBlkCatcode()
{
	CStringMapFile pSmfFile;
	ULONG i;

	if ((_access(MSD_PLLM_CATCODE_FILE, 0)) == -1)
	{
		return FALSE;
	}
	if (pSmfFile.Open(MSD_PLLM_CATCODE_FILE, FALSE, FALSE) != 1)
	{
		return FALSE;
	}

	for (i = 0; i <= 100; i++)
	{
		(pSmfFile)[i]["Catcode"] = "";
	}
		
	pSmfFile.Update();	//v3.71T5
	pSmfFile.Close();
	return TRUE;
}


/*****************************/
/*  Set/Get LS bond pattern  */
/*****************************/
VOID CBinBlkMain::SetLSBondPattern(BOOL bEnable)
{
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return;
	}


	m_bEnableLSBondPattern = bEnable;

	(*pBTfile)["BinBlock"]["Enable LS Pattern Bond"] = m_bEnableLSBondPattern;


	//Reset Empty unit to zero if disable
	if (m_bEnableLSBondPattern == FALSE)
	{
		ULONG i;
		for (i = 1; i <= GetNoOfBlk(); i++)
		{
			(*pBTfile)["BinBlock"][i]["EmptyUnit"] = (ULONG)0;
			m_oBinBlk[i].SetEmptyUnit(0);
		}
	}

	CMSFileUtility::Instance()->SaveBTConfig();

}//end SetLSBondPattern


BOOL CBinBlkMain::GrabLSBondPattern()
{
	return m_bEnableLSBondPattern;
}//end GrabLSBondPattern


BOOL CBinBlkMain::GrabLSBondPatternIndex(ULONG ulBlkToUse, ULONG ulCurrIndex, ULONG &ulNextIndex)
{
	if (!GrabLSBondPattern())
	{
		return FALSE;
	}

	ULONG ulIndex = 0;

	if (m_oBinBlk[ulBlkToUse].GetLSBondIndex(ulCurrIndex, ulIndex) == TRUE)
	{
	}

	ulNextIndex = ulIndex;
	return TRUE;
}


BOOL CBinBlkMain::RecalculateLSBondPatternIndex(ULONG ulBlkToUse, ULONG ulDieCount, ULONG &ulLSBondIndex)
{
	if (!GrabLSBondPattern())
	{
		return FALSE;
	}
	if ((ulBlkToUse < 1) || (ulBlkToUse > BT_MAX_BINBLK_SIZE - 1))
	{
		return FALSE;
	}

	ULONG ulOrigEmptyUnit = m_oBinBlk[ulBlkToUse].GetEmptyUnit();
	ulLSBondIndex = m_oBinBlk[ulBlkToUse].RecalculateLSBondIndex(ulDieCount);
	ULONG ulNewEmptyUnit = m_oBinBlk[ulBlkToUse].GetEmptyUnit();

	CString szLog;
	szLog.Format("(BLK #%lu) EMPTYUNIT re-calculated : Old=%lu, New=%lu", ulBlkToUse, ulOrigEmptyUnit, ulNewEmptyUnit);
	/******/ 
	CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/
	return TRUE;
}


VOID CBinBlkMain::SaveEmptyUnitsToMSD()
{
	if (!GrabLSBondPattern())
	{
		return;
	}

	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		return;
	}

	for (INT i = 1; i < BT_MAX_BINBLK_SIZE; i++)
	{
		(*pBTfile)["BinBlock"][i]["EmptyUnit"]	= m_oBinBlk[i].GetEmptyUnit();
	}

	CMSFileUtility::Instance()->SaveBTConfig();
}


// Control the whole grade mapping logic
BOOL CBinBlkMain::GradeMapping()
{
	CString szExtension = "", szMapFullPath = "";
	INT nCol = 0;
	
	if (CMS896AStn::m_WaferMapWrapper.GetReader() == NULL)	//v4.48A29
		return FALSE;

	szMapFullPath = CMS896AStn::m_WaferMapWrapper.GetFileName();
	if (szMapFullPath.GetLength() == 0)		//v4.48A29
		return FALSE;

	nCol = szMapFullPath.ReverseFind('.');
	if ((nCol != -1) && (szMapFullPath.GetLength() > nCol + 1))
	{
		szExtension = szMapFullPath.Mid(nCol + 1);
	}
	
	if (CheckNeedUpdateMapping(szMapFullPath) == TRUE)	
	{
		//Update the mapping if it is a rmp file
		if (szExtension == "rmp")
		{
			UpdateGradeMappingInBinBlk(szMapFullPath);
		}
	}

	return TRUE;
}

// Update the mapping variable stored in the BinBlk
BOOL CBinBlkMain::UpdateGradeMappingInBinBlk(CString &szMapFullPath)
{
	CMapStringToString szaMappingGrade;

	ReadMappedGrade(szMapFullPath, szaMappingGrade);
	
	for (ULONG i = 1; i <= GetNoOfBlk(); i++)
	{
		CString szMappedGrade, szOrgGrade;

		szOrgGrade.Format("%u", m_oBinBlk[i].GetGrade());

		if (szaMappingGrade.Lookup(szOrgGrade, szMappedGrade))
		{
			m_oBinBlk[i].SetMappedGrade(szMappedGrade);
			
		}
	}
	
	return TRUE;
}

// Check whether the mapping need to update using the map file name
BOOL CBinBlkMain::CheckNeedUpdateMapping(CString &szMapFullPath)
{
	CString szTemp = "";

	if (szMapFullPath != m_szMapFilename)
	{
		m_szMapFilename = szMapFullPath;

		// Clear the mapping variable in the BinBlk
		for (ULONG i = 0; i < GetNoOfBlk(); i++)
		{
			m_oBinBlk[i].SetMappedGrade(szTemp);
		}

		return TRUE;
	}
	
	return FALSE;
}

// Function for Reading the mapping file
BOOL CBinBlkMain::ReadMappedGrade(CString &szMapFullPath, CMapStringToString &szaGradeMapping)
{
	CStdioFile cfMappingFile;
	CString szData, szMappingFileFullPath;
	BOOL bOpen = FALSE, bReturn = FALSE;
	INT nCol = -1;

	//using the same path as the map file but with different extension
	nCol = szMapFullPath.ReverseFind('.');
	
	if (nCol != -1)
	{
		szMappingFileFullPath = szMapFullPath.Left(nCol);
	}

	bOpen = cfMappingFile.Open(szMappingFileFullPath + ".mdb", CFile::modeRead);

	if (bOpen == TRUE)
	{
		cfMappingFile.ReadString(szData); // Read the header Map Changer Mapping File
		cfMappingFile.ReadString(szData); // Read the filename
		cfMappingFile.ReadString(szData); // Read the header [Start of mapping info]

		do
		{
			if (cfMappingFile.ReadString(szData) == TRUE)
			{
				nCol = szData.Find(',');

				if (nCol != -1)
				{
					if (nCol + 1 <= szData.GetLength())
					{
						szaGradeMapping.SetAt(szData.Mid(nCol + 1), szData.Left(nCol));
						bReturn = TRUE;
					}
				}
			}

		} 
		while (szData != "");
		
		cfMappingFile.Close();
	}
							
	return bReturn;
}

//Get the info for generating 2D Barcode (for Cree)		//v3.33T3
VOID CBinBlkMain::SubmitRandomHoleDieInfo(BOOL bEnable2DBarcodeOutput, ULONG ulHoleDieNum)	
{
	m_bEnable2DBarcodeOutput = bEnable2DBarcodeOutput;
	m_ulHoleDieNum = ulHoleDieNum;
}

//Generate random holes for 2D Barcode (for Cree)		//v3.33T3
VOID CBinBlkMain::GenRandomHole(ULONG ulBlkToUse)
{
	m_ulCurrHoleDieNum[ulBlkToUse] = 0;

	//v3.35
	//Need Input-COunt to generate Radom-Hole number for 2D barcode
	UCHAR ucGrade = m_oBinBlk[ulBlkToUse].GetGrade();
	ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5
	ULONG ulInputCount = m_oBinGradeData[ucGrade].GetInputCount();

	m_oBinBlk[ulBlkToUse].GenerateRandomHoleDieIndex(ulBlkToUse, m_ulHoleDieNum, ulInputCount);		//v3.35
	
	SaveRandomHoleData(ulBlkToUse);
	SaveCurHoleDieNum(ulBlkToUse);
}

//Get the random hole index generated (for Cree)		//v3.33T3
ULONG CBinBlkMain::GrabRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulIndex)
{
	return m_oBinBlk[ulBlkToUse].GetRandomHoleDieIndex(ulBlkToUse, ulIndex);
}

//Get the current number of holes generated on the bin for each bin blk (for Cree)		//v3.33T3
ULONG CBinBlkMain::GetCurrHoleDieNum(ULONG ulBlkToUse)
{
	if (m_bEnable2DBarcodeOutput == FALSE)
	{
		return 0;
	}

	return m_ulCurrHoleDieNum[ulBlkToUse];
}

VOID CBinBlkMain::SetEmptyHoleDieNum(ULONG ulBlkToUse, ULONG ulHoleDieNum)
{
	m_oBinBlk[ulBlkToUse].SetEmptyHoleDieNum(ulHoleDieNum);
}

BOOL CBinBlkMain::CreateBackupTempFileList()
{
	LONG lNoOfFiles = 0;
	CString szBlockNo, szFilename;

	CMSFileUtility::Instance()->LoadBinBlkRunTimeDataFile();
	CStringMapFile *fBinBlkRunTimeFile = CMSFileUtility::Instance()->GetBinBlkRunTimeDataFile();
	if (fBinBlkRunTimeFile == NULL)		//Klocwork	//v4.02T5
	{
		//SetErrorMessage("BT: Load BinBlk Runtime data MSD file fails 1!");
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_NO; i++) //Grade No
	{
		if (m_oBinBlk[i].GetNVNoOfBondedDie() > 0)
		{
			lNoOfFiles = lNoOfFiles + 1;
			szBlockNo.Format("%d", i);
			
			if (i < 10)
			{
				szBlockNo = "0" + szBlockNo;
			}

			szFilename = "Blk" + szBlockNo + "TempFile.csv";
			(*fBinBlkRunTimeFile)["FileList"][lNoOfFiles] = szFilename; 
		}
	}

	(*fBinBlkRunTimeFile)["FileList"]["No Of File"] = lNoOfFiles;

	CMSFileUtility::Instance()->SaveBinBlkRunTimeDataFile();
	CMSFileUtility::Instance()->CloseBinBlkRunTimeDataFile();
	
	return TRUE;
}

BOOL CBinBlkMain::SaveBinBlkRunTimeData()
{

	CMSFileUtility::Instance()->LoadBinBlkRunTimeDataFile();
	CStringMapFile *fBinBlkRunTimeFile = CMSFileUtility::Instance()->GetBinBlkRunTimeDataFile();
	if (fBinBlkRunTimeFile == NULL)		//Klocwork	//v4.02T5
	{
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_NO; i++) //Grade No
	{
		(*fBinBlkRunTimeFile)["BinBlk"][i]["NoOfBondedDie"] = m_oBinBlk[i].GetNVNoOfBondedDie();
		(*fBinBlkRunTimeFile)["BinBlk"][i]["CurBondIndex"] = m_oBinBlk[i].GetNVCurrBondIndex();
		(*fBinBlkRunTimeFile)["BinBlk"][i]["LastFileSaveIndex"] = m_oBinBlk[i].GetNVLastFileSaveIndex();
		(*fBinBlkRunTimeFile)["BinBlk"][i]["IsFull"] = m_oBinBlk[i].GetNVIsFull();
		(*fBinBlkRunTimeFile)["BinBlk"][i]["IsAligned"] = m_oBinBlk[i].GetNVIsAligned();
		(*fBinBlkRunTimeFile)["BinBlk"][i]["XOffset"] = m_oBinBlk[i].GetNVXOffset();
		(*fBinBlkRunTimeFile)["BinBlk"][i]["YOffset"] = m_oBinBlk[i].GetNVYOffset();
		(*fBinBlkRunTimeFile)["BinBlk"][i]["RotateAngleX"] = m_oBinBlk[i].GetNVRotateAngleX();
	}

	CMSFileUtility::Instance()->SaveBinBlkRunTimeDataFile();
	CMSFileUtility::Instance()->CloseBinBlkRunTimeDataFile();

	return TRUE;
}

BOOL CBinBlkMain::LoadBinBlkRunTimeData()
{
	ULONG ulNoOfBondedDie;
	ULONG ulCurrBondIndex;
	ULONG ulLastFileSaveIndex;
	BOOL bIsFull;
	BOOL bIsAligned;
	LONG lXOffset;
	LONG lYOffset;
	DOUBLE dRotateAngleX;

	CMSFileUtility::Instance()->LoadBinBlkRunTimeDataFile();
	CStringMapFile *fBinBlkRunTimeFile = CMSFileUtility::Instance()->GetBinBlkRunTimeDataFile();
	if (fBinBlkRunTimeFile == NULL)		//Klocwork
	{
		//SetErrorMessage("BT: Load BinBlk Runtime data MSD file fails 2!");
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_NO; i++) //Grade No
	{
		ulNoOfBondedDie = (ULONG)(*fBinBlkRunTimeFile)["BinBlk"][i]["NoOfBondedDie"]; 
		ulCurrBondIndex = (ULONG)(*fBinBlkRunTimeFile)["BinBlk"][i]["CurBondIndex"]; 
		ulLastFileSaveIndex = (ULONG)(*fBinBlkRunTimeFile)["BinBlk"][i]["LastFileSaveIndex"];
		bIsFull = (BOOL)(LONG)(*fBinBlkRunTimeFile)["BinBlk"][i]["IsFull"];
		bIsAligned = (BOOL)(LONG)(*fBinBlkRunTimeFile)["BinBlk"][i]["IsAligned"]; 
		lXOffset = (LONG)(*fBinBlkRunTimeFile)["BinBlk"][i]["XOffset"];
		lYOffset = (LONG)(*fBinBlkRunTimeFile)["BinBlk"][i]["YOffset"];
		dRotateAngleX = (DOUBLE)(*fBinBlkRunTimeFile)["BinBlk"][i]["RotateAngleX"]; 
		
		m_oBinBlk[i].SetNVNoOfBondedDie(ulNoOfBondedDie);
		m_oBinBlk[i].SetNVCurrBondIndex(ulCurrBondIndex);
		m_oBinBlk[i].SetNVLastFileSaveIndex(ulLastFileSaveIndex);
		m_oBinBlk[i].SetNVIsFull(bIsFull);
		m_oBinBlk[i].SetNVIsAligned(bIsAligned);
		m_oBinBlk[i].SetNVXOffset(lXOffset);
		m_oBinBlk[i].SetNVYOffset(lYOffset);
		m_oBinBlk[i].SetNVRotateAngleX(dRotateAngleX);
	}

	CMSFileUtility::Instance()->CloseBinBlkRunTimeDataFile();
	return TRUE;
}

BOOL CBinBlkMain::SaveBinGradeRunTimeData()
{
	CString szGrade;

	CMSFileUtility::Instance()->LoadBinBlkRunTimeDataFile();
	CStringMapFile *fBinBlkRunTimeFile = CMSFileUtility::Instance()->GetBinBlkRunTimeDataFile();
	if (fBinBlkRunTimeFile == NULL)		//Klocwork	//v4.02T5
	{
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_NO; i++) //Grade No
	{
		szGrade.Format("%d", i);
		(*fBinBlkRunTimeFile)["BinGrade"][i]["NoOfSortedDie"] = m_oBinGradeData[i].GetNVNoOfSortedDie();
		(*fBinBlkRunTimeFile)["BinGrade"][i]["BlkInUse"] = m_oBinGradeData[i].GetNVBlkInUse();
		(*fBinBlkRunTimeFile)["BinGrade"][i]["IsFull"] = m_oBinGradeData[i].GetNVIsFull();
	}

	CMSFileUtility::Instance()->SaveBinBlkRunTimeDataFile();
	CMSFileUtility::Instance()->CloseBinBlkRunTimeDataFile();

	return TRUE;
}

BOOL CBinBlkMain::LoadBinGradeRunTimeData()
{
	//CString szGrade;
	BOOL bIsFull;
	ULONG ulBlkInUse, ulNoOfSortedDie;

	CMSFileUtility::Instance()->LoadBinBlkRunTimeDataFile();
	CStringMapFile *fBinBlkRunTimeFile = CMSFileUtility::Instance()->GetBinBlkRunTimeDataFile();
	if (fBinBlkRunTimeFile == NULL)		//Klocwork
	{
		//SetErrorMessage("BT: Load BinBlk Runtime data MSD file fails 3!");
		return FALSE;
	}

	for (INT i = 1; i <= BT_MAX_BINBLK_NO; i++) //Grade No
	{
		//szGrade.Format("%d", i);
		ulNoOfSortedDie = (ULONG)(*fBinBlkRunTimeFile)["BinGrade"][i]["NoOfSortedDie"];
		ulBlkInUse = (ULONG)(*fBinBlkRunTimeFile)["BinGrade"][i]["BlkInUse"];
		bIsFull = (BOOL)(LONG)(*fBinBlkRunTimeFile)["BinGrade"][i]["IsFull"];

		m_oBinGradeData[i].SetNVNoOfSortedDie(ulNoOfSortedDie);
		m_oBinGradeData[i].SetNVBlkInUse(ulBlkInUse);
		m_oBinGradeData[i].SetNVIsFull(bIsFull);
	}

	CMSFileUtility::Instance()->CloseBinBlkRunTimeDataFile();
	return TRUE;
}


BOOL CBinBlkMain::GenerateBinRunTimeSummary(CString szBinRunTimeSummaryFile, CString szMachineNo)
{
	CTime curTime = CTime::GetCurrentTime();
	CStdioFile cfBinRunTimeSummary;
	CString szBlkNo, szBarcode, szNoOfSortedDie, szCurTime;
	ULONG ulNoOfSortedDie;
	CString szMsg;
	CString szBarCode;

	DeleteFile(szBinRunTimeSummaryFile);

	if (cfBinRunTimeSummary.Open(szBinRunTimeSummaryFile, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		return FALSE;
	}

	szCurTime = curTime.Format("%Y-%m-%d");
	cfBinRunTimeSummary.WriteString("Unloaded Date:" + szCurTime + "\n");
	cfBinRunTimeSummary.WriteString("Machine:" + szMachineNo + "\n");
	cfBinRunTimeSummary.WriteString("\n");
	cfBinRunTimeSummary.WriteString("Bin#,Sorted Die,Sheet Bar Code,Qty\n");

	for (INT i = 1; i <= (INT)GetNoOfBlk(); i++)
	{
		szMsg.Empty();
		szBlkNo.Format("%d", i);
		if (m_pBinTable != NULL)
			szBarCode = m_pBinTable->GetBLBarcodeData(i);	//v4.51A17
		else
			szBarcode = "BarcodeBTNull";
		ulNoOfSortedDie = GrabNVNoOfBondedDie(i);
		szNoOfSortedDie.Format("%d", ulNoOfSortedDie);
		szMsg = szBlkNo + "," + szBarcode + "," + szNoOfSortedDie;

		cfBinRunTimeSummary.WriteString(szMsg + "\n");			
	}

	cfBinRunTimeSummary.Close();

	return TRUE;
}

BOOL CBinBlkMain::UpdateLotRemainCountByGrade(UCHAR ucGrade)
{
	ULONG ulPrevRemain, ulCurRemain;
	ULONG ulBinFullCount;

	for (INT i = 1; i <= (INT) GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetGrade() == ucGrade)
		{
			ulBinFullCount = m_oBinBlk[i].GetNVNoOfBondedDie();
		
			ulPrevRemain = m_oBinGradeData[ucGrade].GetLotRemainingCount();
			ulCurRemain = ulPrevRemain - ulBinFullCount;

			if (ulPrevRemain < ulBinFullCount)
			{
				ulCurRemain = 0;	
			}
			else
			{
				ulCurRemain = ulPrevRemain - ulBinFullCount;
			}
		
			//m_oBinGradeData[ucGrade].SetLotRemainingCount(ulCurRemain);
			SetLotRemainCount(ucGrade, ulCurRemain);
			
			CString szLog;
			szLog.Format("Update Remain Count - Grade:%d RemainingCount:%d", ucGrade, ulCurRemain);
			CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
		}
	}

	return TRUE;
}

BOOL CBinBlkMain::UpdateLotRemainCountByPhyBlk(ULONG ulBinBlk)
{
	ULONG ulPrevRemain, ulCurRemain;
	ULONG ulBinFullCount;
	UCHAR ucGrade;

	for (INT i = 1; i <= (INT) GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetPhyBlkId() == ulBinBlk)
		{
			ulBinFullCount = m_oBinBlk[i].GetNVNoOfBondedDie();

			ucGrade = m_oBinBlk[i].GetGrade();
			ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			ulPrevRemain = m_oBinGradeData[ucGrade].GetLotRemainingCount();

			if (ulPrevRemain < ulBinFullCount)
			{
				ulCurRemain = 0;	
			}
			else
			{
				ulCurRemain = ulPrevRemain - ulBinFullCount;
			}
		
			//m_oBinGradeData[ucGrade].SetLotRemainingCount(ulCurRemain);
			SetLotRemainCount(ucGrade, ulCurRemain);

			CString szLog;
			szLog.Format("Update Remain Count - Grade:%d RemainingCount:%d", ucGrade, ulCurRemain);
			CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
		}
	}

	return TRUE;
}

BOOL CBinBlkMain::OptimizeBinFrameCountByGrade(UCHAR ucGrade, UCHAR ucOptimizeBinCountMode, ULONG ulOption)
{
	ULONG ulLotRemainDieCount;
	ULONG ulNumOfFrames = 0, ulRemainder = 0;
	ULONG ulInputCount = 0 , ulMinFrameDieCount = 0 , ulMaxFrameDieCount = 0 ;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == _T("Lextar") || ulOption > 1)
	{
		return OptimizeBinFrameCountByGrade_Luxtar(ucGrade, ucOptimizeBinCountMode);
	}

	for (INT i = 1; i <= (INT) GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetGrade() == ucGrade)
		{
			ulLotRemainDieCount = m_oBinGradeData[ucGrade].GetLotRemainingCount();
			ulMinFrameDieCount = m_oBinGradeData[ucGrade].GetMinFrameDieCount();
			ulMaxFrameDieCount = m_oBinGradeData[ucGrade].GetMaxFrameDieCount();

			if (ulLotRemainDieCount == 0)
			{
				SetIntputCount(ucGrade, ulMaxFrameDieCount);
				return FALSE;
			}
			
			if (ulMaxFrameDieCount == 0)
			{
				return FALSE;
			}

			ulNumOfFrames = ulLotRemainDieCount / ulMaxFrameDieCount;
			ulRemainder = ulLotRemainDieCount % ulMaxFrameDieCount;

			if (ulNumOfFrames == 1 && ulRemainder > 0)
			{
				ULONG ulLastFrameDieCount = 0, ul2ndLastFrameDieCount = 0;
				// Not enough for last frame
				if (ulRemainder < ulMinFrameDieCount)
				{
					ulInputCount = 0;

					if (ulMaxFrameDieCount > ulMinFrameDieCount)
					{
						ul2ndLastFrameDieCount = ulMaxFrameDieCount - ulMinFrameDieCount;
						ulLastFrameDieCount = ulMinFrameDieCount + ulRemainder;
						
						// sort smaller die count first
						if (ucOptimizeBinCountMode == 0)
						{
							ulInputCount = min(ul2ndLastFrameDieCount, ulLastFrameDieCount);
						}
						// sort larger die count first
						else if (ucOptimizeBinCountMode == 1)
						{
							ulInputCount = max(ul2ndLastFrameDieCount, ulLastFrameDieCount);
						}
					}

					SetIntputCount(ucGrade, ulInputCount);
					
					CString szLog;
					szLog.Format("Optimize Bin Count - Grade:%d Max:%d Min:%d Lot Remain:%d Input Count:%d",
								 ucGrade, ulMaxFrameDieCount, ulMinFrameDieCount, ulLotRemainDieCount, ulInputCount);
					CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
					szLog.Format("2ndLastFrameDieCount:%d LastFrameDieCount:%d", ul2ndLastFrameDieCount,
								 ulLastFrameDieCount);
					CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);

					return TRUE;
				}
			}
			else if (ulNumOfFrames == 0)
			{
				// set to max count for both smaller die count first or larger die count first.
				SetIntputCount(ucGrade, ulMaxFrameDieCount);
				CString szLog;
				szLog.Format("Last Frame - RemainDieCount:%d ", ulRemainder);
				CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
			}
		}
	}
			
	return FALSE;
}

BOOL CBinBlkMain::OptimizeBinFrameCountByPhyBlk(ULONG ulBinBlk, UCHAR ucOptimizeBinCountMode, ULONG ulOption)
{
	ULONG ulLotRemainDieCount;
	ULONG ulNumOfFrames = 0, ulRemainder = 0;
	ULONG ulInputCount = 0 , ulMinFrameDieCount = 0 , ulMaxFrameDieCount = 0;

	UCHAR ucGrade;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == _T("Lextar") || ulOption > 1)
	{
		return OptimizeBinFrameCountByPhyBlk_Luxtar(ulBinBlk, ucOptimizeBinCountMode);
	}
		
	for (INT i = 1; i <= (INT) GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetPhyBlkId() == ulBinBlk)
		{
			ucGrade = m_oBinBlk[i].GetGrade();
			ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			ulLotRemainDieCount = m_oBinGradeData[ucGrade].GetLotRemainingCount();
			ulMinFrameDieCount = m_oBinGradeData[ucGrade].GetMinFrameDieCount();
			ulMaxFrameDieCount = m_oBinGradeData[ucGrade].GetMaxFrameDieCount();

			if (ulLotRemainDieCount == 0)
			{
				SetIntputCount(ucGrade, ulMaxFrameDieCount);
				return FALSE;
			}

			if (ulMaxFrameDieCount == 0)
			{
				return FALSE;
			}

			ulNumOfFrames = ulLotRemainDieCount / ulMaxFrameDieCount;
			ulRemainder = ulLotRemainDieCount % ulMaxFrameDieCount;

			if (ulNumOfFrames == 1 && ulRemainder > 0)
			{
				ULONG ulLastFrameDieCount = 0, ul2ndLastFrameDieCount = 0;
				// Not enough for last frame
				if (ulRemainder < ulMinFrameDieCount)
				{
					ulInputCount = 0;

					if (ulMaxFrameDieCount > ulMinFrameDieCount)
					{
						ul2ndLastFrameDieCount = ulMaxFrameDieCount - ulMinFrameDieCount;
						ulLastFrameDieCount = ulMinFrameDieCount + ulRemainder;

						// sort smaller die count first
						if (ucOptimizeBinCountMode == 0)
						{
							ulInputCount = min(ul2ndLastFrameDieCount, ulLastFrameDieCount);
						}
						// sort larger die count first
						else if (ucOptimizeBinCountMode == 1)
						{
							ulInputCount = max(ul2ndLastFrameDieCount, ulLastFrameDieCount);
						}
					}
					
					SetIntputCount(ucGrade, ulInputCount);
					
					CString szLog;
					szLog.Format("Optimize Bin Count - Grade:%d Max:%d Min:%d Lot Remain:%d Input Count:%d",
								 ucGrade, ulMaxFrameDieCount, ulMinFrameDieCount, ulLotRemainDieCount, ulInputCount);
					CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
					szLog.Format("2ndLastFrameDieCount:%d LastFrameDieCount:%d", ul2ndLastFrameDieCount,
								 ulLastFrameDieCount);
					CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);

					return TRUE;
				}
			}
			else if (ulNumOfFrames == 0)
			{
				SetIntputCount(ucGrade, ulMaxFrameDieCount);
				CString szLog;
				szLog.Format("Last Frame - RemainDieCount:%d ", ulRemainder);
				CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
			}
			
		}
	}
			
	return FALSE;
}

BOOL CBinBlkMain::IsTwoDimensionBarcodeHole(ULONG ulBondedBlkId)
{
	BOOL bHole = FALSE;

	if (m_bEnable2DBarcodeOutput)
	{
		LONG lCurrentIndex = m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 1;		
		
		for (INT i = 0; i < (INT)m_ulHoleDieNum; i++)
		{
			if (lCurrentIndex == GrabRandomHoleDieIndex(ulBondedBlkId, i))
			{
				bHole = TRUE;
			}
		}
	}
	
	return bHole;
}

BOOL CBinBlkMain::TwoDimensionBarcodeFindNextBondIndex(ULONG ulBlkToUse, ULONG &ulNextIndex)
{
	BOOL bUpdate = FALSE;

	if (m_bEnable2DBarcodeOutput)	//skip the index which equal to the random hole generated	//v3.33T3
	{		
		//BOOL bUpdate = FALSE;		//v4.48A26
		for (INT i = 0; i < (INT) m_ulHoleDieNum; i++)
		{
			if (ulNextIndex == GrabRandomHoleDieIndex(ulBlkToUse, i))
			{
				//v4.48A10	//Cree HuiZhou
				//if (CMSLogFileUtility::Instance()->GetEnableMachineLog())
				//{
					LONG lBinRow = 0;
					LONG lBinCol = 0;
					m_oBinBlk[ulBlkToUse].FindBondRowColGivenIndex(ulNextIndex, lBinRow, lBinCol);

					//v4.51A22
					//CString szMsg;
					//szMsg.Format("BIN #%d HOLE found at INDEX = %ld; New INDEX = %ld; HOLE-Index = %d, RowCol (%ld, %ld), Path=%lu", 
					//				ulBlkToUse, ulNextIndex, ulNextIndex+1, i, lBinRow, lBinCol, 
					//				m_oBinBlk[ulBlkToUse].GetWalkPath());
					//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
				//}

				ulNextIndex++;
				bUpdate = TRUE;
				break;	//v4.48A18
			}
		}

		//v4.48A15	//Re-enabled for Cree HuiZhou; used to update curr HOLE no. if INDEX is updated
		//if (bUpdate)
		//{
		//	ULONG ulEmptyHoleUnit = 0;
		//	for(INT j = 0; j < (INT) m_ulHoleDieNum; j++)
		//	{
		//		if (ulNextIndex >= GrabRandomHoleDieIndex(ulBlkToUse, j))
		//		{
		//			ulEmptyHoleUnit++;
		//		}
		//	}
		//			  
		//	m_ulCurrHoleDieNum[ulBlkToUse] = ulEmptyHoleUnit;
		//	SaveCurHoleDieNum(ulBlkToUse);
		//}
	}

	return bUpdate;
}

BOOL CBinBlkMain::IsNear2DBarcodeBondIndex(ULONG ulBlkToUse, ULONG ulIndex)
{
	if (!m_bEnable2DBarcodeOutput)	
		return FALSE;

	ULONG ulHoleIndex = 0;

	for (INT i = 0; i < (INT) m_ulHoleDieNum; i++)
	{
		ulHoleIndex = GrabRandomHoleDieIndex(ulBlkToUse, i);

		if (labs(ulIndex - ulHoleIndex) <= 4)	//TO log the XY encoder posns for dices before/after the HOLE
		{
			return TRUE;
		}
	}

	return FALSE;
}

ULONG CBinBlkMain::TwoDimensionBarcodeGetTotalNoOfHoleOnFrame(UCHAR ucGrade)
{
	ULONG ulEmptyHoleCount = 0;

	if (m_bEnable2DBarcodeOutput)	//v3.35
	{
		for (INT i = 0; i < (INT)GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

			if (m_oBinBlk[i].GetGrade() == ucGrade)
			{
				ulEmptyHoleCount = ulEmptyHoleCount + m_ulCurrHoleDieNum[i];
			}
		}
	}

	return ulEmptyHoleCount; 
}

BOOL CBinBlkMain::IsFirstRowColSkipPatternSkipUnit(ULONG ulBondedBlkId)
{
	ULONG ulCurrentIndex = 0;

	if (m_oBinBlk[ulBondedBlkId].GetFirstRowColSkipPattern() == FALSE)
	{
		return FALSE;
	}

	ulCurrentIndex = m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 1;

	return m_oBinBlk[ulBondedBlkId].IsFirstRowColSkipBondPatternIndex(ulCurrentIndex);
}

BOOL CBinBlkMain::FirstRowColSkipPatternFindNextBondIndex(ULONG ulBlkToUse, ULONG &ulNextIndex)
{
	if (m_oBinBlk[ulBlkToUse].GetFirstRowColSkipPattern() == FALSE)
	{
		return FALSE;
	}

	ULONG ulUpdatedIndex = 0;
	
	if (m_oBinBlk[ulBlkToUse].GetFirstRowColSkipBondPatternIndex(ulNextIndex, ulUpdatedIndex) == TRUE)
	{
		ulNextIndex = ulUpdatedIndex;
		return TRUE;
	}
	
	return FALSE;
}

ULONG CBinBlkMain::GetFirstRowColSkipPatternSkippedUnitCount(ULONG ulBlkToUse)
{
	if (m_oBinBlk[ulBlkToUse].GetFirstRowColSkipPattern() == FALSE)
	{
		return 0;
	}

	ULONG ulCurrentIndex = m_oBinBlk[ulBlkToUse].GetNVCurrBondIndex();

	return m_oBinBlk[ulBlkToUse].GetSkippedUnitForFirstRowColSkipPattern(0, ulCurrentIndex);
}


ULONG CBinBlkMain::GetFirstRowColSkipPatTotalSkipUnitCount(ULONG ulBlkToUse)
{
	UCHAR ucGrade = 1;
	ULONG ulGradeInputCount = 0;

	if (m_oBinBlk[ulBlkToUse].GetFirstRowColSkipPattern() == FALSE)
	{
		return 0;
	}

	ucGrade = m_oBinBlk[ulBlkToUse].GetGrade();
	ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

	ulGradeInputCount = m_oBinGradeData[ucGrade].GetInputCount();
	return m_oBinBlk[ulBlkToUse].GetFirstRowColSkipPatTotalSkipUnitCount(ulGradeInputCount);
}

BOOL CBinBlkMain::GetFirstRowColSkipPattern(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetFirstRowColSkipPattern();
}

ULONG CBinBlkMain::GetFirstRowColSkipPatternSkipUnit(ULONG ulBlkId)
{
	return m_oBinBlk[ulBlkId].GetFirstRowColSkipUnit();
}

BOOL CBinBlkMain::GetIsWafflePadEnable(ULONG ulBondedBlkId)
{
	return m_oBinBlk[ulBondedBlkId].GetIsEnableWafflePad();
}

BOOL CBinBlkMain::IsWafflePadSkipUnit(ULONG ulBondedBlkId)
{
	ULONG ulCurrentIndex = 0;

	if (m_oBinBlk[ulBondedBlkId].GetIsEnableWafflePad() == FALSE)
	{
		return FALSE;
	}

	ulCurrentIndex = m_oBinBlk[ulBondedBlkId].GetNVCurrBondIndex() + 1;

	return m_oBinBlk[ulBondedBlkId].IsWafflePadSkipPos(ulCurrentIndex);
}


BOOL CBinBlkMain::WafflePadSkipPosFindNextBondIndex(ULONG ulBlkToUse, ULONG &ulNextIndex)
{
	if (m_oBinBlk[ulBlkToUse].GetIsEnableWafflePad() == FALSE)
	{
		return FALSE;
	}

	ULONG ulUpdatedIndex = 0;
	
	if (m_oBinBlk[ulBlkToUse].GetWafflePadSkipIndex(ulNextIndex, ulUpdatedIndex) == TRUE)
	{
		ulNextIndex = ulUpdatedIndex;
		return TRUE;	
	}

	return FALSE;
}

ULONG CBinBlkMain::GetWafflePadSkipUnitCount(ULONG ulBlkToUse)
{
	if (m_oBinBlk[ulBlkToUse].GetIsEnableWafflePad() == FALSE)
	{
		return 0;
	}

	ULONG ulCurrentIndex = m_oBinBlk[ulBlkToUse].GetNVCurrBondIndex();

	return m_oBinBlk[ulBlkToUse].GetSkippedUnitForWafflePad(0, ulCurrentIndex);
}

ULONG CBinBlkMain::GetWafflePadSkipUnitTotalCount(ULONG ulBlkToUse)
{
	UCHAR ucGrade = 1;
	ULONG ulGradeInputCount = 0;

	if (m_oBinBlk[ulBlkToUse].GetIsEnableWafflePad() == FALSE)
	{
		return 0;
	}

	ucGrade = m_oBinBlk[ulBlkToUse].GetGrade();
	ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

	ulGradeInputCount = m_oBinGradeData[ucGrade].GetInputCount();
	return m_oBinBlk[ulBlkToUse].GetWafflePadTotalSkipUnitCount(ulGradeInputCount);
}

BOOL CBinBlkMain::OptimizeBinFrameCount(UCHAR ucGrade, UCHAR ucOptimizeBinCountMode)
{
	ULONG ulLotRemainDieCount;
	ULONG ulNumOfFrames = 0, ulRemainder = 0;
	ULONG ulMinFrameDieCount = 0 , ulMaxFrameDieCount = 0 ;

	ulLotRemainDieCount = m_oBinGradeData[ucGrade].GetLotRemainingCount();
	ulMinFrameDieCount = m_oBinGradeData[ucGrade].GetMinFrameDieCount();
	ulMaxFrameDieCount = m_oBinGradeData[ucGrade].GetMaxFrameDieCount();

	if (ulLotRemainDieCount == 0)
	{
		SetIntputCount(ucGrade, ulMaxFrameDieCount);
		return FALSE;
	}
	
	if (ulMaxFrameDieCount == 0)
	{
		return FALSE;
	}

	ulNumOfFrames = ulLotRemainDieCount / ulMaxFrameDieCount;
	ulRemainder = ulLotRemainDieCount % ulMaxFrameDieCount;

	if (ulNumOfFrames == 1 && ulRemainder > 0) // >Max and < 2Max
	{
		ULONG ulLastFrameDieCount, ulPrevFrameDieCount;
		ULONG ulInputCount = 0;
		// Not enough for last frame
		if (ulRemainder < ulMinFrameDieCount) // Remain < Min
		{
			if (ulLotRemainDieCount >= (ulMinFrameDieCount * 2))
			{
				ulPrevFrameDieCount = ulMinFrameDieCount;	// less
				ulLastFrameDieCount = ulLotRemainDieCount - ulMinFrameDieCount;
			}
			else
			{
				//	ulPrevFrameDieCount = ulLotRemainDieCount - ulMinFrameDieCount;
				//	upLastFrameDieCount = ulMinFrameDieCount;
				ulPrevFrameDieCount = ulLotRemainDieCount - ulMaxFrameDieCount;
				ulLastFrameDieCount = ulMaxFrameDieCount;
			}
			
			// sort smaller die count first
			ulInputCount = ulPrevFrameDieCount;
			if (ucOptimizeBinCountMode == 0)
			{
				ulInputCount = ulPrevFrameDieCount;
				//	ulInputCount = min(ulPrevFrameDieCount, ulLastFrameDieCount);
			}
			// sort larger die count first
			else if (ucOptimizeBinCountMode == 1)
			{
				ulInputCount = ulLastFrameDieCount;
				//	ulInputCount = max(ulPrevFrameDieCount, ulLastFrameDieCount);
			}

			SetIntputCount(ucGrade, ulInputCount);
			
			CString szLog;
			szLog.Format("Optimize Bin Count - Grade:%d Max:%d Min:%d Lot Remain:%d Input Count:%d",
						 ucGrade, ulMaxFrameDieCount, ulMinFrameDieCount, ulLotRemainDieCount, ulInputCount);
			CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
			szLog.Format("2ndLastFrameDieCount:%d LastFrameDieCount:%d", ulPrevFrameDieCount,
						 ulLastFrameDieCount);
			CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
			return TRUE;
		}
		else // Remain > Min
		{
			ulPrevFrameDieCount = ulLotRemainDieCount - ulMaxFrameDieCount;
			ulLastFrameDieCount = ulMaxFrameDieCount;
			ulInputCount = ulPrevFrameDieCount;
			if (ucOptimizeBinCountMode == 0)
			{
				ulInputCount = ulPrevFrameDieCount;
			}
			else if (ucOptimizeBinCountMode == 1)
			{
				ulInputCount = ulLastFrameDieCount;
			}
			SetIntputCount(ucGrade, ulInputCount);

			CString szLog;
			szLog.Format("Optimize Bin Count - Grade:%d Max:%d Min:%d Lot Remain:%d Input Count:%d",
						 ucGrade, ulMaxFrameDieCount, ulMinFrameDieCount, ulLotRemainDieCount, ulInputCount);
			CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
			szLog.Format("2ndLastFrameDieCount:%d LastFrameDieCount:%d", ulPrevFrameDieCount,
						 ulLastFrameDieCount);
			CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);

			return TRUE;
		}
	}
	else if (ulNumOfFrames == 0) // < Max
	{
		// set to max count for both smaller die count first or larger die count first.
		SetIntputCount(ucGrade, ulMaxFrameDieCount);
		CString szLog;
		szLog.Format("Last Frame - RemainDieCount:%d ", ulRemainder);
		CMSLogFileUtility::Instance()->BT_BinRemainingCountLog(szLog);
		return TRUE;
	}
	else // <=Max or >=2Max
	{
		// set to max count for both smaller die count first or larger die count first.
		SetIntputCount(ucGrade, ulMaxFrameDieCount);
		return TRUE;
	}

	return FALSE;
}

BOOL CBinBlkMain::OptimizeBinFrameCountByGrade_Luxtar(UCHAR ucGrade, UCHAR ucOptimizeBinCountMode)
{
	for (INT i = 1; i <= (INT) GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetGrade() == ucGrade)
		{
			return OptimizeBinFrameCount(ucGrade, ucOptimizeBinCountMode);
		}
	}
			
	return FALSE;
}

BOOL CBinBlkMain::OptimizeBinFrameCountByPhyBlk_Luxtar(ULONG ulBinBlk, UCHAR ucOptimizeBinCountMode)
{
	UCHAR ucGrade;
	
	for (INT i = 1; i <= (INT) GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetPhyBlkId() == ulBinBlk)
		{
			ucGrade = m_oBinBlk[i].GetGrade();
			ucGrade = min(ucGrade, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5
			return OptimizeBinFrameCount(ucGrade, ucOptimizeBinCountMode);
		}
	}
			
	return FALSE;
}

UCHAR CBinBlkMain::GetBinMixCurrentGrade()//benableosrambinmixmap
{
	return m_ucBinMixCurrentGrade;
}


VOID CBinBlkMain::SetFrameRotation(ULONG ulBlkId, BOOL bEnable180Rotation, 
								   LONG lBinCalibX, LONG lBinCalibY)	//v4.49A9
{
	CString szLog;
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		if (bEnable180Rotation)
			m_oBinBlk[ulBlkId].SetFrameRotation(BBX_FRAME_ROTATE_180, lBinCalibX, lBinCalibY);
		else
			m_oBinBlk[ulBlkId].SetFrameRotation(BBX_FRAME_ROTATE_0, lBinCalibX, lBinCalibY);
	}
	else if (ulBlkId == 0)
	{
		for (INT i=1; i<=(BT_MAX_BINBLK_SIZE - 1); i++)
		{
			if (bEnable180Rotation)
				m_oBinBlk[i].SetFrameRotation(BBX_FRAME_ROTATE_180, lBinCalibX, lBinCalibY);
			else
				m_oBinBlk[i].SetFrameRotation(BBX_FRAME_ROTATE_0, lBinCalibX, lBinCalibY);
		}
	}
	else
	{
		return;
	}
}

LONG CBinBlkMain::GetBinCalibX(const ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].GetBinCalibX();
	}

	return 0;
}


LONG CBinBlkMain::GetBinCalibY(const ULONG ulBlkId)
{
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
		m_oBinBlk[ulBlkId].GetBinCalibY();
	}

	return 0;

}


BOOL CBinBlkMain::GetFrameRotation(ULONG ulBlkId)	//v4.59A26
{
	CString szLog;
	UCHAR ucRotation = 0;
	if ((ulBlkId >= 1) && (ulBlkId <= BT_MAX_BINBLK_SIZE - 1))	
	{
//enum {BBX_FRAME_ROTATE_0 = 0, BBX_FRAME_ROTATE_90 = 1, BBX_FRAME_ROTATE_180 = 2, BBX_FRAME_ROTATE_270 = 3};
		ucRotation = m_oBinBlk[ulBlkId].GetFrameRotation();
		if (ucRotation == BBX_FRAME_ROTATE_180)
		{
			return TRUE;	//180 degree rotate
		}
	}
	return FALSE;			// 0 degree rotate
}

LONG CBinBlkMain::CleanupGenAndQueryDB(ULONG ulBlkId, UCHAR ucGrade, ULONG ulMode)
{
	//--------------- Default is clear bin blk by phyblk id --------------//
	for (ULONG i = 1; i <= GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);

		if( (m_oBinBlk[i].GetNVNoOfBondedDie() == 0) )
		{
			continue;
		}

		BOOL bThisOK = FALSE;
		if( (m_oBinBlk[i].GetPhyBlkId()==ulBlkId) && (ulMode==CLEAR_BIN_BY_PHY_BLK) )
		{
			bThisOK = TRUE;
		}
		if( (m_oBinBlk[i].GetGrade()==ucGrade) && (ulMode==CLEAR_BIN_BY_GRADE) )
		{
			bThisOK = TRUE;
		}

		if( bThisOK )
		{
			CString szMsg;
			szMsg.Format("Clean up Gen DB and Query DB for bin block %lu, phy blk %lu, grade %d", i, ulBlkId, ucGrade);
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);

			CGenerateDatabase	*pGenDB = ((CGenerateDatabase*)CMS896AStn::m_objaGenerateDatabase[i]);
			CQueryDatabase		*pQryDB = (CQueryDatabase*)CMS896AStn::m_objaQueryDatabase[i];
			if( pGenDB!=NULL )
				pGenDB->Cleanup();
			if( pQryDB!=NULL )
				pQryDB->Cleanup();
		}
	}

	return 1;
} //end CleanupGenAndQueryDB

BOOL CBinBlkMain::GenBinBlkFile_Finisar()
{
	CStdioFile cfBinBlkFile;
	UCHAR ucBinBlkGrade;
	ULONG ulNoLeft = 0, ulNoPicked = 0, ulNoTotal = 0;
	ULONG ulNoMiss = 0, ulNoDefect = 0, ulNoBadCut = 0, ulNoEmpty= 0;
	CString szBinBlkId, szBinBlkGrade, szNoLeft, szNoPicked, szNoTotal;
	CString szGradeNumNgLeft, szNoDefect, szNoBadcut, szNoEmpty;
	CString szRankID;
	ULONG ulCol, ulRow;
	ULONG i;
	ULONG ulMaxRow = 0, ulMaxCol = 0;
	ULONG ulDieState;
	ULONG ulMissingDieNo;
	ULONG ulGradeNumNgLeft = 0;

	ULONG	ulNoGrdMapOrgTotal;
	CString	szNoGrdMapOrgTotal;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//v4.42T17
	if (!CMS896AStn::m_WaferMapWrapper.IsMapValid() == TRUE)
	{
		return FALSE;
	}
	if (CMS896AStn::m_WaferMapWrapper.GetMapDimension(ulMaxRow, ulMaxCol) != TRUE)
	{
		return FALSE;
	}


	remove("BinBlkFile.csv");

	if (!cfBinBlkFile.Open(_T("BinBlkFile.csv"), 
					  CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeText))
	{
		return FALSE;
	}

	cfBinBlkFile.SeekToEnd();

	ULONG ulNoOfBlk = 150;	//	GetNoOfBlk();

	UCHAR ucOffset = CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
	CUIntArray aulAllGrades;
	aulAllGrades.RemoveAll();
	for (i = 1; i <= ulNoOfBlk; i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		ucBinBlkGrade	= m_oBinBlk[i].GetGrade();

		// Calculate the missing die
		ulGradeNumNgLeft	= 0;
		ulMissingDieNo	= 0;
		ulNoMiss		= 0;
		ulNoDefect		= 0;
		ulNoBadCut		= 0;
		ulNoEmpty		= 0;
		ulNoLeft		= 0;
		ulNoPicked		= 0;
		ulNoTotal		= 0;
		ulNoGrdMapOrgTotal = 0;

		if( CMS896AApp::m_bEnableSubBin )
		{
			CUIntArray aulGradeList;
			aulGradeList.RemoveAll();
			for (int p = 1; p <= m_pBinTable->m_nNoOfSubBlk; p++)
			{
				if( m_pBinTable->m_nSubBlk[p]==i )
				{
					for(int q = 1; q <= m_pBinTable->m_nNoOfSubGrade; q ++)
					{
						aulGradeList.Add(m_pBinTable->m_nSubGrade[p][q] + ucOffset);
						aulAllGrades.Add(m_pBinTable->m_nSubGrade[p][q] + ucOffset);
					}
				}
			}

			if( aulGradeList.GetSize() == 0 )
			{
				BOOL bInUse = FALSE;
				for (int p = 1; p <= m_pBinTable->m_nNoOfSubBlk; p ++)
				{
					for(int q = 1; q <= m_pBinTable->m_nNoOfSubGrade; q ++)
					{
						if(ucBinBlkGrade == m_pBinTable->m_nSubGrade[p][q])
						{
							bInUse = TRUE;
							break;
						}
					}
					if( bInUse )
					{
						break;
					}
				}
				if( bInUse==FALSE )
				{
					for(int kk=0; kk<aulAllGrades.GetSize(); kk++)
					{
						if( (ucBinBlkGrade + ucOffset)==aulAllGrades.GetAt(kk) )
						{
							bInUse = TRUE;
						}
					}
				}
				if( bInUse==FALSE )
				{
					CString szOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalGradeString(ucBinBlkGrade + ucOffset);
					if( szOrigGrade.Find("Fail")==-1 )
					{
						aulGradeList.Add(ucBinBlkGrade + ucOffset);
						aulAllGrades.Add(ucBinBlkGrade + ucOffset);
					}
				}
			}

			if( aulGradeList.GetSize()>0 )
			{
				ucBinBlkGrade	= m_oBinBlk[i].GetGrade();
			}
			else
			{
				ucBinBlkGrade	= (UCHAR)i;
			}

			for(UINT mm = 0; mm < (UINT)aulGradeList.GetSize(); mm++)
			{
				ULONG ulGradeLeft = 0, ulGradePicked = 0, ulGradeTotal = 0, ulGradeMapTotal = 0;
				UCHAR ucMapGrade		= aulGradeList.GetAt(mm);
				CMS896AStn::m_WaferMapWrapper.GetStatistics(ucMapGrade, ulGradeLeft, ulGradePicked, ulGradeTotal);

				for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
				{
					for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
					{
						UCHAR ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalDieInformation(ulRow, ulCol);
						if( ucMapGrade!=ucOrigGrade )
						{
							continue;
						}

						ulGradeMapTotal++;
						ulDieState = CMS896AStn::m_WaferMapWrapper.GetDieState(ulRow, ulCol);
						if (ulDieState == WT_MAP_DIESTATE_DEFECT)
						{
							ulNoDefect++;
						}
						else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
						{
							ulNoBadCut++;
						}
						else if (ulDieState == WT_MAP_DIESTATE_EMPTY)
						{
							ulNoEmpty++;
						}
						else if (ulDieState > WT_MAP_DIESTATE_MISSING)
						{
							ulMissingDieNo++;
						}
						if( ulDieState>=WT_MAP_DS_PR_DEFECT )
						{
							ulGradeNumNgLeft++;
						}
					}
				}
				ulNoLeft += ulGradeLeft;
				ulNoPicked += ulGradePicked;
				ulNoTotal += ulGradeTotal;
				ulNoGrdMapOrgTotal += ulGradeMapTotal;
			}
		}
		else
		{
			UCHAR ucMapGrade		= ucBinBlkGrade + ucOffset;
			CMS896AStn::m_WaferMapWrapper.GetStatistics(ucMapGrade, ulNoLeft, ulNoPicked, ulNoTotal);

			for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
			{
				for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
				{
					//v4.42T17	//bug fix of AoYang WaferEnd library
					UCHAR ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetGrade(ulRow, ulCol);
					ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalDieInformation(ulRow, ulCol);
					if( ucMapGrade!=ucOrigGrade )
					{
						continue;
					}

					ulDieState = CMS896AStn::m_WaferMapWrapper.GetDieState(ulRow, ulCol);
				
					if (ulDieState == WT_MAP_DIESTATE_DEFECT)
					{
						ulNoDefect++;
					}
					else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
					{
						ulNoBadCut++;
					}
					else if (ulDieState == WT_MAP_DIESTATE_EMPTY)
					{
						ulNoEmpty++;
					}
					else if (ulDieState > WT_MAP_DIESTATE_MISSING)
					{
						// Extract the grade info
						UCHAR	ucMissingGrade = (UCHAR)(ulDieState - WT_MAP_DIESTATE_MISSING);
						if( ulDieState > WT_MAP_DS_BH_UPLOOK_FAIL )
						{
							ucMissingGrade = (UCHAR)(ulDieState - WT_MAP_DS_BH_UPLOOK_FAIL);
						}

						if (ucMissingGrade == ucBinBlkGrade)
						{
							ulMissingDieNo++;
						}
						if( ulDieState>=WT_MAP_DS_PR_DEFECT )
						{
							ulGradeNumNgLeft++;
						}
					}
				}
			}
			ulNoGrdMapOrgTotal = m_oBinBlk[i].GetNVNoOfBondedDie();
		}

		if (ulNoPicked > ulMissingDieNo)
		{
			ulNoPicked = (ulNoPicked - ulMissingDieNo);
		}
		else
		{
			ulNoPicked = 0;
		}
		ulNoLeft = ulNoTotal - ulNoPicked;

		szBinBlkId.Format("%d",		i);
		szBinBlkGrade.Format("%d",	ucBinBlkGrade);
		if (CMS896AStn::m_bUseOptBinCountDynAssignGrade == TRUE)
		{
			USHORT usOrgGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(ucBinBlkGrade + ucOffset) - ucOffset;
			szBinBlkGrade.Format("%d", usOrgGrade);
		}
		szNoLeft.Format("%d",		ulNoLeft);
		szNoPicked.Format("%d",		ulNoPicked);
		szNoGrdMapOrgTotal.Format("%d", ulNoGrdMapOrgTotal); 
		szNoTotal.Format("%d",		ulNoTotal);
		szGradeNumNgLeft.Format("%lu",	ulGradeNumNgLeft);
		szNoDefect.Format("%lu",	ulNoDefect);
		szNoBadcut.Format("%lu",	ulNoBadCut);
		szNoEmpty.Format("%lu",		ulNoEmpty);
		szRankID = GrabRankID(ucBinBlkGrade);

		cfBinBlkFile.WriteString(szBinBlkId + "," + szBinBlkGrade + "," + szNoLeft + "," +
								 szNoPicked + "," + szNoGrdMapOrgTotal + "," + szNoTotal + "," + szGradeNumNgLeft + "," + 
								 szGradeNumNgLeft	+ "," + szNoDefect		+ "," + szNoBadcut	+ "," + szNoEmpty + "," +	//v4.40T14
								 szRankID + "\n");
	}

	cfBinBlkFile.Close();

	return TRUE;
} //end 


VOID CBinBlkMain::SetCountByGrade(UCHAR ucGrade, ULONG ulGradeCounter)
{
	if ((ucGrade >= 1) && (ucGrade <= BT_MAX_BINBLK_SIZE - 1))
	{
		CString szLog;
		szLog.Format("SetCountByGrade ucGrade = %d, Count = %d", ucGrade, ulGradeCounter);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		m_oBinGradeData[ucGrade].SetNVNoOfSortedDie(ulGradeCounter);
	}

/*
	for (INT i = 1; i <= (INT) m_ulNoOfBlk; i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetGrade() == ucGrade)
		{
			m_oBinBlk[i].SetNVNoOfBondedDie(ulGradeCounter);
			return;
		}
	}
*/
}

ULONG CBinBlkMain::GetCountByGrade(UCHAR ucGrade)
{
	for (INT i = 1; i <= (INT) m_ulNoOfBlk; i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);	//Klocwork	//v4.02T5

		if (m_oBinBlk[i].GetGrade() == ucGrade)
		{
			return m_oBinBlk[i].GetNVNoOfBondedDie();
		}
	}
	return 0;
}


ULONG CBinBlkMain::GetBlock(UCHAR ucGrade)
{
	for (INT i = 1; i <= (INT) m_ulNoOfBlk; i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE - 1);

		if (m_oBinBlk[i].GetGrade() == ucGrade)
		{
			return i;
		}
	}
	return 0;
}



BOOL CBinBlkMain::UpdateSingleGradeCapacity(ULONG ulBlkToSetup, BOOL bCopyFromGrade1)
{
	ULONG ulDiePerRow, ulDiePerCol, ulDiePerBlk;
	DOUBLE dDiePitchX, dDiePitchY;

	if (bCopyFromGrade1)
	{
		m_oBinGradeData[ulBlkToSetup].SetGradeCapacity(m_oBinBlk[1].GetDiePerBlk());
	}
	else
	{
		dDiePitchX = m_oBinBlk[ulBlkToSetup].GetDDiePitchX();
		dDiePitchY = m_oBinBlk[ulBlkToSetup].GetDDiePitchY();

		ulDiePerRow = m_oBinBlk[ulBlkToSetup].CalculateDiePerRow((DOUBLE)dDiePitchX);
		m_oBinBlk[ulBlkToSetup].SetDiePerRow(ulDiePerRow);

		ulDiePerCol = m_oBinBlk[ulBlkToSetup].CalculateDiePerCol((DOUBLE)dDiePitchY);
		m_oBinBlk[ulBlkToSetup].SetDiePerCol(ulDiePerCol);

		ulDiePerBlk = m_oBinBlk[ulBlkToSetup].CalculateDiePerBlk();
		m_oBinBlk[ulBlkToSetup].SetDiePerBlk(ulDiePerBlk);

		m_oBinGradeData[ulBlkToSetup].SetGradeCapacity(m_oBinBlk[ulBlkToSetup].GetDiePerBlk());
	}

	return TRUE;
}
/**************************** Class CBinBlkMain End ****************************/
