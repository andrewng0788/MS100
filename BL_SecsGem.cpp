/////////////////////////////////////////////////////////////////
// BL_LoadFrame.cpp : Move functions of the CBinLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, Jan 01, 2018
//	Revision:	1.00
//
//	By:			TANG
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinLoader.h"
#include "MS_SecCommConstant.h"		//ANDREW_SC
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//================================================================
// Function Name: 		UpdateAllSGVariables
// Input arguments:		None
// Output arguments:	None
// Description:   		Update all releated SVID variable when host called some function
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::UpdateAllSGVariables()
{
	if (!IsSecsGemInit()) 
	{
		return;
	}

	for (LONG i = 0; i < MS_BL_AUTO_LINE_MGZN_NUM; i++)
	{
		for (LONG j = 0; j < MS_BL_MGZN_SLOT; j++)
		{
			SECS_UpdateCassetteSlotInfo(i, j + 1);	//Update SV SECSGEM variables
		}
	}
}

// ==========================================================================================================
//			Related Host Command FRAME_LOAD, FRAME_LOAD_COMPLETE, 
//			FRAME_UNLOAD, FRAME_UNLOAD_COMPLETE function
// ==========================================================================================================
//================================================================
// Function Name: 		SECS_InitCassetteSlotInfo
// Input arguments:		None
// Output arguments:	None
// Description:   		Initialize all BL_Slot1(SVID#4001)~BL_Slot200(SVID#4200)
// Return:				None
// Remarks:				None
//================================================================
BOOL CBinLoader::SECS_InitCassetteSlotInfo()
{
   if (!IsSecsGemInit()) 
	   {
        return TRUE;
	}

	if (!IsMSAutoLineMode())
	{
        return TRUE;
	}

	CString szSlotPrefixName = "BL_Slot";
	CString szValue;

	for (LONG lMgznNo = 0; lMgznNo < MS_BL_AUTO_LINE_MGZN_NUM; lMgznNo++)
	{
		if (m_stMgznRT[lMgznNo].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
		{
			for (INT j = 1; j < MS_BL_MGZN_SLOT; j++)
			{
				INT nIndex = j - 1;

				//BL_Slot SVID list
				CString szSlotID, szSlotName;
				szSlotID.Format("%d", j);
				szSlotName = szSlotPrefixName + szSlotID;
				(*m_pGemStation)[szSlotName].RemoveAll();
			
				//SVID List:
				//1. slot number
				//2. Barcode ID
				//3. Slot Usage
				//4. WIP Bin Counter
				//5. Bin Grade
				//6. SN
				//7. LotNo

				szValue.Format("%d", m_stMgznRT[lMgznNo].m_lSlotBlock[nIndex]);		//Slot ID
				(*m_pGemStation)[szSlotName].AddTail(szValue);					

				(*m_pGemStation)[szSlotName].AddTail(m_stMgznRT[lMgznNo].m_SlotBCName[nIndex]);	//Barcode

				szValue.Format("%d", m_stMgznRT[lMgznNo].m_lSlotUsage[nIndex]);		//Slot Usage
				(*m_pGemStation)[szSlotName].AddTail(szValue);

				szValue.Format("%d", m_stMgznRT[lMgznNo].m_lSlotWIPCounter[nIndex]); //WIP, bin counts
				(*m_pGemStation)[szSlotName].AddTail(szValue);

				szValue.Format("%d", m_stMgznRT[lMgznNo].m_lSlotBlock[nIndex]);		//Bin grade
				(*m_pGemStation)[szSlotName].AddTail(szValue);

				(*m_pGemStation)[szSlotName].AddTail(m_stMgznRT[lMgznNo].m_SlotSN[nIndex]);   //SN

				(*m_pGemStation)[szSlotName].AddTail(m_stMgznRT[lMgznNo].m_SlotLotNo[nIndex]);	//Lot No
			}
		}
	}

	DisplayMessage("BinLoader - SECSGEM Slot info initialized");
	return TRUE;
}


//================================================================
// Function Name: 		GetSlotIndex
// Input arguments:		lMgzNo based on ZERO, lSlotNo based on ZERO
// Output arguments:	None
// Description:   		Get Slot index in all magazine
// Return:				None
// Remarks:				None
//================================================================
LONG CBinLoader::GetSlotIndex(const LONG lMgznNo, const LONG lSlotNo)
{
	//1~200 slots, there has 25 slots each magazine
	LONG lSlotIndex = lMgznNo * MS_BL_MGZN_SLOT + lSlotNo;

	switch (lMgznNo)
	{
	case BL_MGZN_TOP1:		//SLOT(0--25) Grade(0--0)
		break;
	case BL_MGZN_MID1:		//SLOT(26-50) Grade(0--25)
		break;
	case BL_MGZN_MID2:		//SLOT(51-75) Grade(26--50)
		lSlotIndex = 2 * MS_BL_MGZN_SLOT + lSlotNo;
		break;
	case BL_MGZN_BTM1:		//SLOT(76--100) Grade(51-75)
		lSlotIndex = 3 * MS_BL_MGZN_SLOT + lSlotNo;
		break;
	case BL_MGZN_BTM2:		//SLOT(101--125) Grade(76-100)
		lSlotIndex = 4 * MS_BL_MGZN_SLOT + lSlotNo;
		break;
	case BL_MGZN8_BTM1:		//SLOT(126--150) Grade(101-125)
		lSlotIndex = 5 * MS_BL_MGZN_SLOT + lSlotNo;
		break;
	case BL_MGZN8_BTM2:		//SLOT(151--175) Grade(126-150)
		lSlotIndex = 6 * MS_BL_MGZN_SLOT + lSlotNo;
		break;
	case BL_MGZN_TOP2:		//not used, it takes as SLOT(176--200)
		//Ignore TOP2 magazine if use smart inline system
		lSlotIndex = 7 * MS_BL_MGZN_SLOT + lSlotNo;
		break;
	}

	return lSlotIndex;
}

//================================================================
// Function Name: 		SECS_UpdateCassetteSlotInfo
// Input arguments:		None
// Output arguments:	None
// Description:   		Update the current BL_SlotXXX(SVID#4XXX)
// Return:				None
// Remarks:				None
//================================================================
BOOL CBinLoader::SECS_UpdateCassetteSlotInfo(CONST LONG lMgznNo, CONST LONG lSlotNo, BOOL bIsLoad)
{
    if (!IsSecsGemInit()) 
	{
        return TRUE;
	}

	if (!IsMSAutoLineMode())
	{
        return TRUE;
	}

//	becasue TOP2 with BL_MGZN_USAGE_UNUSE as a clear frame's magazine, so the following code is removed in 2018.07.14
//	if (m_stMgznRT[lMgznNo].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
//	{
//		return TRUE;
//	}

	CString szSlotPrefixName = "BL_Slot";

	INT nIndex = lSlotNo - 1;
	if (nIndex < 0)
	{
		nIndex = 0;
	}

	if (nIndex >= MS_BL_MGZN_SLOT)
	{
		nIndex = MS_BL_MGZN_SLOT - 1;
	}

	//1~200 slots, there has 25 slots each magazine
	LONG lSlotIndex = GetSlotIndex(lMgznNo, lSlotNo - 1) + 1;

	try
	{
		CString szSlotID, szSlotName;

		szSlotID.Format("%d", lSlotIndex);
		szSlotName = szSlotPrefixName + szSlotID;

		(*m_pGemStation)[szSlotName].RemoveAll();	//v4.57A2

		//SVID List:
		//1. slot number
		//2. Barcode ID
		//3. Slot Usage
		//4. WIP Bin Counter
		//5. Bin Grade
		//6. SN
		//7. LotNo

		szSlotID.Format("%d", lSlotIndex);									//Slot ID: 1 - 200
		(*m_pGemStation)[szSlotName].AddTail(szSlotID);					

		CString szBC = m_stMgznRT[lMgznNo].m_SlotBCName[nIndex];
		if (bIsLoad)	//v4.59A11
		{
			szBC = "";
		}

		(*m_pGemStation)[szSlotName].AddTail(szBC);							//Barcode

		CString szSlotUsage;
		szSlotUsage.Format("%d", m_stMgznRT[lMgznNo].m_lSlotUsage[nIndex]);		//Slot Usage
		(*m_pGemStation)[szSlotName].AddTail(szSlotUsage);

	//	szValue = "10000";													//WIP, bin counts
		CString szSlotWIP;
		szSlotWIP.Format("%d", m_stMgznRT[lMgznNo].m_lSlotWIPCounter[nIndex]);//WIP, bin counts
		(*m_pGemStation)[szSlotName].AddTail(szSlotWIP);

		CString szSlotBinGrade;
		szSlotBinGrade.Format("%d", m_stMgznRT[lMgznNo].m_lSlotBlock[nIndex]);	//Bin grade
		(*m_pGemStation)[szSlotName].AddTail(szSlotBinGrade);	

		CString szSN = m_stMgznRT[lMgznNo].m_SlotSN[nIndex];					//SN(Serial Number)
		(*m_pGemStation)[szSlotName].AddTail(szSN);

		CString szLotNo = m_stMgznRT[lMgznNo].m_SlotLotNo[nIndex];					//Lot No
		(*m_pGemStation)[szSlotName].AddTail(szLotNo);

		CString szLog;
		szLog.Format("BL: SECS_UpdateCassetteSlotInfo - SLOT=%ld, BC=%s, SlotUsage = %s, WIP =%d, BinGrade = %d, SN = %s, LotNo = %s", 
					 lSlotIndex, szBC, GetSlotUsage(m_stMgznRT[lMgznNo].m_lSlotUsage[nIndex]), 
					 m_stMgznRT[lMgznNo].m_lSlotWIPCounter[nIndex], m_stMgznRT[lMgznNo].m_lSlotBlock[nIndex],
					 m_stMgznRT[lMgznNo].m_SlotSN[nIndex], m_stMgznRT[lMgznNo].m_SlotLotNo[nIndex]);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SECS_UpdateCassetteSlotInfo Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SECS_UpdateCassetteSlotInfo Exception");
//		DisplayException(e);
	}

	return TRUE;
}



//================================================================
// Function Name: 		SECS_UpdateLoadUnloadStatus
// Input arguments:		None
// Output arguments:	None
// Description:   		Update the load/unload status
// Return:				None
// Remarks:				None
//================================================================
BOOL CBinLoader::SECS_UpdateLoadUnloadStatus(CONST LONG lBlock, LONG lMagzNo, LONG lSlotNo, BOOL bIsLoad, LONG lStatus)
{
    if (!IsSecsGemInit()) 
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("BL: SECS_UpdateLoadUnloadStatus fails.");
		return TRUE;
	}

	try
	{
		int nBaseIndex = 0;		//v4.57A3
		int nIndex = 1;

		nIndex = nBaseIndex + lSlotNo;
		CString szSlotID;
		szSlotID.Format("%d", nIndex);

		CString szReportID = "0";

		SetGemValue(MS_SECS_DV_AL_FRAME_TYPE,	"B");
		SetGemValue(MS_SECS_DV_AL_SLOT_ID,		szSlotID);

		if (lStatus == 0)
			SetGemValue(MS_SECS_DV_AL_LOADUNLOAD_STATUS,	"0");	//Success
		else
		{
			CString szStatus;
			szStatus.Format("%d", lStatus);
			SetGemValue(MS_SECS_DV_AL_LOADUNLOAD_STATUS,	szStatus);	//Fail
		}

		//v4.56A3
		CString szLog;
		szLog.Format("BL: SECS_UpdateLoadUnloadStatus - IsLoad=%ld, Status=%d", bIsLoad, lStatus);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		if (bIsLoad)
		{
			SendEvent(SG_CEID_FRAME_LOAD_RESPONSE_EVENT);
		}
		else
		{
			SendEvent(SG_CEID_FRAME_UNLOAD_RESPONSE_EVENT);
		}

		CMSLogFileUtility::Instance()->MS_LogSECSGEM("Done\n");
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SECS_UpdateLoadUnloadStatus Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SECS_UpdateLoadUnloadStatus Exception");
//		DisplayException(e);
	}
	return TRUE;
}

// ==========================================================================================================
//			Related SECS/GEM host Command 
// ==========================================================================================================
//================================================================
// Function Name: 		SECS_UpdateInputSlotGradeWIPCounter
// Input arguments:		None
// Output arguments:	None
// Description:   		Called by GetLoadMgzSlot_AutoLine etc
// Return:				-1 -- No available slot
//						0~4 -- Input Slot Number
// Remarks:				None
//================================================================
VOID CBinLoader::SECS_UpdateInputSlotGradeWIPCounter()
{
	LONG lMgzn = BL_MGZ_TOP_1;

	//Update WIP data for grade
	IPC_CServiceMessage svMsg;
	typedef struct 
	{
		ULONG	ulBlk[5];
		ULONG	ulUnloadDieGradeCount[5];		
	} BLKDATA;

	BLKDATA stInfo;
	for (LONG i = MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1; i < 5; i++)
	{
		stInfo.ulBlk[i]					= m_stMgznRT[lMgzn].m_lSlotBlock[i];
		if (m_stMgznRT[lMgzn].m_lSlotWIPCounter[i] < 0)
		{
			m_stMgznRT[lMgzn].m_lSlotWIPCounter[i] = 0;
		}
		stInfo.ulUnloadDieGradeCount[i] = m_stMgznRT[lMgzn].m_lSlotWIPCounter[i];
	}
	
	svMsg.InitMessage(sizeof(BLKDATA), &stInfo);
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("BT_SetCountByGrade"), svMsg);
								
	CString szLog;
	for (int i = MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1; i < 5; i++)
	{
		if (i == 0)
		{
			szLog.Format("SECS_LoadFrameDoneCmd: BT_SetCountByGrade - (slot, count)=%ld, %ld", m_stMgznRT[lMgzn].m_lSlotBlock[0], m_stMgznRT[lMgzn].m_lSlotWIPCounter[0]);
		}
		else
		{
			szLog.AppendFormat("SECS_LoadFrameDoneCmd: BT_SetCountByGrade - (slot, count)=%ld, %ld", m_stMgznRT[lMgzn].m_lSlotBlock[i], m_stMgznRT[lMgzn].m_lSlotWIPCounter[i]);
		}
	}
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
}


//================================================================
// Function Name: 		GetCassettePositionName
// Input arguments:		lMgzn -- the current magazine number.
// Output arguments:	None
// Description:   		Get the name of cassette position
// Return:				None
// Remarks:				None
//================================================================
CString CBinLoader::GetCassettePositionName(const LONG lMgzn)
{
	CString szCassettePos;
	switch (lMgzn)
	{
		case BL_MGZN_TOP1:	szCassettePos = "B1T"; break;
		case BL_MGZN_MID1:	szCassettePos = "B1M"; break;
		case BL_MGZN_BTM1:	szCassettePos = "B1B"; break;
		case BL_MGZN_TOP2:	szCassettePos = "B2T"; break;
		case BL_MGZN_MID2:	szCassettePos = "B2M"; break;
		case BL_MGZN_BTM2:	szCassettePos = "B2B"; break;
		case BL_MGZN8_BTM1:	szCassettePos = "B1B8"; break;
		case BL_MGZN8_BTM2:	szCassettePos = "B2B8"; break;

		default:			szCassettePos = "   "; break;
	}

	return szCassettePos;
}


//================================================================
// Function Name: 		SendResetSlotEvent_7018
// Input arguments:		lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Output arguments:	None
// Description:   		Send Reset Slot 
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendResetSlotEvent_7018(const LONG lMgzn, const LONG lSlot, BOOL bResetAll)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		LONG lSlotIndex = GetSlotIndex(lMgzn, lSlot);
		if (bResetAll)
		{
			SetGemValue(MS_SECS_SV_SLOT_NO, 0);					//3004
		}
		else
		{
			SetGemValue(MS_SECS_SV_SLOT_NO, lSlotIndex + 1);	//3004
		}
		SetGemValue(MS_SECS_SV_FRAME_TYPE, "B");				//3066
		SetGemValue(MS_SECS_SV_RESET_SLOT, 1);					//5032
		SendEvent(SG_CEID_RESET_SLOT);							//7018
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendResetSlotEvent_7018 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendResetSlotEvent_7018 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendGripperEvent_7050
// Input arguments:		lPreUnloadPos_X,
//						lUnloadPos_X, lLoadPos_X,
//						lReadyPos_X, lBarcodePos_X
//						lBTUnloadPos_X, lBTUnloadPos_Y
// Output arguments:	None
// Description:   		send CEID#7050 after setup the parameters of gripper
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendGripperEvent_7050(const LONG lPreUnloadPos_X, const LONG lUnloadPos_X, const LONG lLoadPos_X, const LONG lReadyPos_X,
									   const LONG lBarcodePos_X, const LONG lBTUnloadPos_X, const LONG lBTUnloadPos_Y)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
	   // 3301
		SetGemValue("BL_XPreUnloadPosn",	lPreUnloadPos_X);
		SetGemValue("BL_XUnloadPosn",		lUnloadPos_X);
		SetGemValue("BL_XLoadPosn",			lLoadPos_X);
		SetGemValue("BL_XReadyPosn",		lReadyPos_X);
		SetGemValue("BL_XScanPosn",			lBarcodePos_X);
		// 3302
		SetGemValue("BL_BTUnloadPosnX",		lBTUnloadPos_X);
		SetGemValue("BL_BTUnloadPosnY",		lBTUnloadPos_Y);
		// 7050
		SendEvent(SG_CEID_BL_GRIPPER,		FALSE);
	}
	catch (...) //CAsmException e)
	{
		DisplaySequence("SendGripperEvent_7050 failure");
	}
}


//================================================================
// Function Name: 		SendMagazineEvent_7051
// Input arguments:		lMagazineNo,
//						lReadyPosY, lReadyLvlZ,
// Output arguments:	None
// Description:   		send CEID#7051 after setup the parameters of magazine
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendMagazineEvent_7051(const LONG lMagazineNo, const LONG lReadyPosY, const LONG lReadyLvlZ)
{
	if (!IsSecsGemInit())
	{
		return;
	}

     // 3306
    SetGemValue("BL_MgznIndex",			lMagazineNo);
    SetGemValue("BL_MgznTotalSlots",	m_stMgznRT[lMagazineNo].m_lNoOfSlots);
    SetGemValue("BL_MgznSlotPitch",		m_stMgznRT[lMagazineNo].m_lSlotPitch);
    SetGemValue("BL_MgznTopSlotLevel",	m_stMgznRT[lMagazineNo].m_lTopLevel);
    SetGemValue("BL_MgznCenterYPosn",	m_stMgznRT[lMagazineNo].m_lMidPosnY);
    // 3307
    SetGemValue("BL_ElevatorReadyPosnY", lReadyPosY);
    SetGemValue("BL_ElevatorReadyPosnZ", lReadyLvlZ);
    // 7051
    SendEvent(SG_CEID_BL_MAGAZINE, FALSE);
}


//================================================================
// Function Name: 		SendBarcodeEvent_7052
// Input arguments:		bUseBarcode, lTryLimits, lScanRange
//						bUseExtension, szExtName
// Output arguments:	None
// Description:   		send CEID#7052 after setup the parameters of barcode 
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBarcodeEvent_7052(const BOOL bUseBarcode, const LONG lTryLimits, const LONG lScanRange,
									   const BOOL bUseExtension, const CString szExtName)				
{
	if (!IsSecsGemInit())
	{
		return;
	}

    // 3316
    SetGemValue("BL_EnableBarCode",		bUseBarcode);
    SetGemValue("BL_ScanRetryLimit",	lTryLimits);
    SetGemValue("BL_ScanRange",			lScanRange);
    // 3317
    SetGemValue("BL_UseExtension",		bUseExtension);
    SetGemValue("BL_BarCodeExt",		szExtName);
    // 7052
    SendEvent(SG_CEID_BL_BARCODE,		FALSE);
}


//================================================================
// Function Name: 		SendOMRuntimeEvent_7053
// Input arguments:		lOMRT
// Output arguments:	None
// Description:   		send CEID#7053 after setup the parameters of OMRuntime 
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendOMRuntimeEvent_7053(const LONG lOMRT)				
{
	if (!IsSecsGemInit())
	{
		return;
	}


    SetGemValue("BL_OMRunTime", lOMRT);			//3311
    SendEvent(SG_CEID_BL_OMRUNTIME, FALSE);		//7503
}

//================================================================
// Function Name: 		SendBinTableUnloadEvent_8011
// Input arguments:		lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Output arguments:	None
// Description:   		Send Bin table unload event#8011
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBinTableLoadEvent_8010(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos,
											const CString szCassetteType)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		LONG lSlotIndex = GetSlotIndex(lMgzn, lSlot);
		SetGemValue(MS_SECS_SV_BL_FRAME_ID, szBCName);			//3042
		SetGemValue(MS_SECS_SV_SLOT_NO, lSlotIndex + 1);		//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS, szCassettePos);	//3005
		SetGemValue(MS_SECS_SV_CASSETTE_TYPE, szCassetteType);	//3006
		SetGemValue(MS_SECS_SV_BT_BIN_NUMBER, 0);				//3038
		SetGemValue(MS_SECS_SV_BT_BIN_NAME, "");				//3039
		SetGemValue(MS_SECS_SV_BT_BIN_DIE_COUNT, 0);			//3043

		SendEvent(SG_CEID_FrmBinTableLoad, TRUE);		//8010	
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendBinTableLoadEvent_8010 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendBinTableLoadEvent_8010 Exception");
//		DisplayException(e);
	}
}

//================================================================
// Function Name: 		SendBinTableUnloadEvent_8011
// Input arguments:		lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Output arguments:	None
// Description:   		Send Bin table unload event#8011
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBinTableUnloadEvent_8011(const LONG lMgzn, const LONG lSlot, const CString szCassettePos)
{
	LONG lBinNo = m_stMgznRT[lMgzn].m_lSlotBlock[lSlot];
	LONG lWIPCountNo = m_stMgznRT[lMgzn].m_lSlotWIPCounter[lSlot];
	CString szBCName;
	szBCName = m_stMgznRT[lMgzn].m_SlotBCName[lSlot];
	SendBinTableUnloadEvent_8011(lMgzn, lSlot, szBCName, szCassettePos, lWIPCountNo);
}

VOID CBinLoader::SendBinTableUnloadEvent_8011(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos, const LONG lWIPCountNo)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		LONG lSlotIndex = GetSlotIndex(lMgzn, lSlot);
		SetGemValue(MS_SECS_SV_BL_FRAME_ID, szBCName);			//3042
		SetGemValue(MS_SECS_SV_SLOT_NO,  lSlotIndex + 1);		//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS, szCassettePos);	//3005
		SetGemValue(MS_SECS_SV_BT_BIN_NUMBER, 0);				//3038
		SetGemValue(MS_SECS_SV_BT_BIN_NAME, "");				//3039
		SetGemValue(MS_SECS_SV_BT_BIN_DIE_COUNT, lWIPCountNo);	//3043

		SendEvent(SG_CEID_FrmBinTableUnload, TRUE);		//8011
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendBinTableUnloadEvent_8011 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendBinTableUnloadEvent_8011 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendBinTableUnloadCompletedEvent_8012
// Input arguments:		lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Output arguments:	None
// Description:   		Send Bin table unload event#8012
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBinTableUnloadCompletedEvent_8012(const LONG lMgzn, const LONG lSlot, const CString szCassettePos)
{
	CString szBCName;
	szBCName = m_stMgznRT[lMgzn].m_SlotBCName[lSlot];
	SendBinTableUnloadCompletedEvent_8012(lMgzn, lSlot, szBCName, szCassettePos);
}

VOID CBinLoader::SendBinTableUnloadCompletedEvent_8012(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		//Set SECSGEM Value
		LONG lSlotIndex = GetSlotIndex(lMgzn, lSlot);
		SetGemValue(MS_SECS_SV_BL_FRAME_ID, szBCName);			//3042
		SetGemValue(MS_SECS_SV_SLOT_NO, lSlotIndex + 1);		//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS, szCassettePos);	//3005
		SetGemValue(MS_SECS_SV_BT_BIN_NUMBER, 0);				//3038
		SetGemValue(MS_SECS_SV_BT_BIN_NAME, "");				//3039
		SetGemValue(MS_SECS_SV_BT_BIN_DIE_COUNT, 0);			//3043
				
		SendEvent(SG_CEID_FrmCmpltCassLoad, TRUE);		//8012
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendBinTableUnloadCompletedEvent_8012 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendBinTableUnloadCompletedEvent_8012 Exception");
//		DisplayException(e);
	}
}

//================================================================
// Function Name: 		SendBinCasseteLoadWIPEvent_8014
// Input arguments:		lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Output arguments:	None
// Description:   		Send Bin table unload event#8012
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBinCasseteLoadWIPEvent_8014(const LONG lMgzn, const LONG lSlot, const CString szCassettePos)
{
	CString szBCName;
	szBCName = m_stMgznRT[lMgzn].m_SlotBCName[lSlot];
	SendBinCasseteLoadWIPEvent_8014(lMgzn, lSlot, szBCName, szCassettePos);
}

VOID CBinLoader::SendBinCasseteLoadWIPEvent_8014(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		//Set SECSGEM Value for CEID#8014
		LONG lSlotIndex = GetSlotIndex(lMgzn, lSlot);
		SetGemValue(MS_SECS_SV_BL_FRAME_ID, szBCName);			//3042
		SetGemValue(MS_SECS_SV_SLOT_NO, lSlotIndex + 1);		//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS, szCassettePos);	//3005
		SetGemValue(MS_SECS_SV_BT_BIN_NUMBER, 0);				//3038
		SetGemValue(MS_SECS_SV_BT_BIN_NAME, "");				//3039
		SetGemValue(MS_SECS_SV_BT_BIN_DIE_COUNT, 0);			//3043

		SendEvent(SG_CEID_FrmWipCassLoad, TRUE);		//8014
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendBinCasseteLoadWIPEvent_8014 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendBinCasseteLoadWIPEvent_8014 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendBinCasseteUnloadWIPEvent_8015
// Input arguments:		lSlot -- the current slot number.
//						szBCName, szCassettePos
// Output arguments:	None
// Description:   		Send Bin Cassette unload WIP Frame Event
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBinCasseteUnloadWIPEvent_8015(const LONG lMgzn, const LONG lSlot, const CString szBCName, const CString szCassettePos)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		LONG lSlotIndex = GetSlotIndex(lMgzn, lSlot);
		SetGemValue(MS_SECS_SV_BL_FRAME_ID, szBCName);			//3042
		SetGemValue(MS_SECS_SV_SLOT_NO, lSlotIndex + 1);		//3004
		SetGemValue(MS_SECS_SV_BT_BIN_NAME, "");				//3039
		SetGemValue(MS_SECS_SV_CASSETTE_POS, szCassettePos);	//3005

		SendEvent(SG_CEID_FrmWipCassUnload, TRUE);		//8015
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendBinCasseteUnloadWIPEvent_8015 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendBinCasseteUnloadWIPEvent_8015 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendClearBin_8018
// Input arguments:		lMgzn -- the previous slot number.
//						lSlot -- the current slot number.
//						szCassetePos -- the position of cassete
//						ulBlkID
//						szBarCodeName -- the barcode name of the frame
//						ulDieCount -- the bonded die number
// Output arguments:	None
// Description:   		send CEID#8018 after Clear Bin
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendClearBin_8018(const LONG lMgzn, const LONG lSlot,  const CString szBCName, const CString szCassetePos,
								   const ULONG ulBlkID, const ULONG ulDieCount)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		LONG lSlotIndex = GetSlotIndex(lMgzn, lSlot);
		SetGemValue(MS_SECS_SV_SLOT_NO,				lSlotIndex + 1);	//3004
		SetGemValue(MS_SECS_SV_CASSETTE_POS,		szCassetePos);		//3005
		SetGemValue(MS_SECS_SV_CASSETTE_TYPE,		"CC");				//3006
		SetGemValue(MS_SECS_SV_BT_BIN_NUMBER,		ulBlkID);			//3038
		SetGemValue(MS_SECS_SV_BT_BIN_NAME,			"");				//3039
		SetGemValue(MS_SECS_SV_BL_FRAME_ID,			szBCName);			//3042
		SetGemValue(MS_SECS_SV_BT_BIN_DIE_COUNT,	ulDieCount);		//3043
		SetGemValue(MS_SECS_SV_BT_BIN_DATE_TIME,	"");				//3047
//		SendEvent(SG_CEID_ClearBin, TRUE);								//CEID: 8018 (Report ID: 4018)
//do not wait for reply
		SendEvent(SG_CEID_ClearBin, FALSE);								//CEID: 8018 (Report ID: 4018)
	}
	catch (...) //CAsmException e)
	{
		DisplaySequence("SG_CEID_ClearBin failure");
	}
}


//================================================================
// Function Name: 		SendBinTransferEvent_8024
// Input arguments:		lPreSlot -- the previous slot number.
//						lCurSlot -- the current slot number.
// Output arguments:	None
// Description:   		send CEID 8024 which transfer bin from the previuos slot to the current slot
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBinTransferEvent_8024(const LONG lPreMgzn, const LONG lPreSlot, const LONG lCurMgzn, const LONG lCurSlot)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		LONG lCurSlotIndex = GetSlotIndex(lCurMgzn, lCurSlot);
		SetGemValue(MS_SECS_SV_SLOT_NO, lCurSlotIndex + 1);		//3004

		LONG lPreSlotIndex = GetSlotIndex(lPreMgzn, lPreSlot);
		SetGemValue(MS_SECS_SV_PRE_SLOT_NO, lPreSlotIndex + 1);	//3008
		SendEvent(SG_CEID_BinTransfer_AUTOLINE, TRUE);	//8024
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendBinTransferEvent_8024 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendBinTransferEvent_8024 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendBarcodeRejectEvent_8026
// Input arguments:		Frame Type -- "W".
//						lCurSlot -- the current slot number.
// Output arguments:	None
// Description:   		send CEID 8026 which read barcode failure or search Jam
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SendBarcodeRejectEvent_8026(const LONG lCurMgzn, const LONG lCurSlot, const LONG lRejectCode)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		LONG lCurSlotIndex = GetSlotIndex(lCurMgzn, lCurSlot);
		SetGemValue(MS_SECS_SV_SLOT_NO, lCurSlotIndex + 1);	//3004
		SetGemValue(MS_SECS_SV_FRAME_TYPE, "B");			//3066
		SetGemValue(MS_SECS_SV_REJECT_CODE, lRejectCode);	//3067
		SendEvent(SG_CEID_BarcodeReject_AUTOLINE, TRUE);	//8026
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendBarcodeRejectEvent_8026 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendBarcodeRejectEvent_8026 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SetSlotRejectStatus
// Input arguments:		lCurrMgzn - the magzine number
//						lCurSlot -- the current slot number.
// Output arguments:	None
// Description:   		Set the reject status of slot and 
//						send CEID 8026 which read barcode failure or search Jam
// Return:				None
// Remarks:				None
//================================================================
VOID CBinLoader::SetSlotRejectStatus(const LONG lCurrMgzn, const LONG lCurSlot, const LONG lRejectCode, const CString szBarCode)
{
	// set as reject status for the current slot
	if (!IsMSAutoLineMode() || (lCurrMgzn > BL_MGZ_TOP_1))
	{
		SetCassetteSlotUsage(lCurrMgzn, lCurSlot, BL_SLOT_USAGE_ACTIVE);
	}
	else
	{
		SetCassetteSlotUsage(lCurrMgzn, lCurSlot, BL_SLOT_USAGE_UNUSE);
	}

	SetCassetteSlotBCName(lCurrMgzn, lCurSlot, szBarCode);
	SendBarcodeRejectEvent_8026(lCurrMgzn, lCurSlot, lRejectCode);
}
