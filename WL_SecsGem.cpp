/////////////////////////////////////////////////////////////////
// WaferLoader.cpp : interface of the CWaferLoader class
//
//	Description:
//		
//
//	Date:		12 August 2004
//	Revision:	1.00
//
//	By:			
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "FileUtil.h"
#include "winuser.h"
#include "MS_SecCommConstant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//========================================================================================
// Function Name: 		UpdateAllSGVariables
// Input arguments:		None
// Output arguments:	None
// Description:   		Update all releated SVID variable when host called some function
// Return:				None
// Remarks:				None
//========================================================================================
VOID CWaferLoader::UpdateAllSGVariables()
{
	if (!IsSecsGemInit()) 
	{
		return;
	}

	for (LONG j = 0; j < MS_WL_AUTO_LINE_MGZN_SLOT; j++)
	{
		SECS_UpdateCassetteSlotInfo(j + 1);	//Update SV SECSGEM variables
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
// Description:   		Initialize all BL_Slot1(SVID#5001)~WL_Slot30(SVID#5030)
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::SECS_InitCassetteSlotInfo()
{
    if (IsSecsGemInit() == FALSE)
        return TRUE;

	LONG lSlotSize = m_stWaferMagazine[0].m_lNoOfSlots;

	//m_lCurrentSlotNo

	CString szSlotPrefixName = "WL_Slot";
	CString szSlotID, szSlotName;
	CString szValue;

	INT i = 0, j = 0;
	LONG lMgznNo = 0;

	for (INT i = 1; i <= WL_MAX_MGZN_SLOT; i++)
	{
		szSlotID.Format("%d", i);
		szSlotName = szSlotPrefixName + szSlotID;
		(*m_pGemStation)[szSlotName].RemoveAll();

		szValue.Format("%d", i);										//Slot ID
		(*m_pGemStation)[szSlotName].AddTail(szValue);					

		CString szBC = m_stWaferMagazine[0].m_SlotBCName[i - 1];
		(*m_pGemStation)[szSlotName].AddTail(szBC);						//Barcode

		CString szSlotUsage;
		szSlotUsage.Format("%ld", m_stWaferMagazine[0].m_lSlotUsage[i - 1]);
		(*m_pGemStation)[szSlotName].AddTail(szSlotUsage);				//Slot Usage

		szValue = "0";													
		(*m_pGemStation)[szSlotName].AddTail(szValue);					//WIP, current remaining die no.

		szValue = "0";													
		(*m_pGemStation)[szSlotName].AddTail(szValue);					//Bin grade always 0 for wafer			
	}

	//Other SECSGEM variables initizlization	//v4.56A11
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	SetGemValue(MS_SECS_SV_CURR_RECIPE_NAME, szPKGFilename);

	DisplayMessage("WaferLoader - SECSGEM Slot info initialized");
	return TRUE;
}


//================================================================
// Function Name: 		SECS_UpdateCassetteSlotInfo
// Input arguments:		None
// Output arguments:	None
// Description:   		Update the current WL_SlotXXX(SVID#5XXX)
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::SECS_UpdateCassetteSlotInfo(CONST LONG lSlotNo, BOOL bIsLoad, 
											   INT nStatus, BOOL bFull)
{
    if (IsSecsGemInit() == FALSE)
	{
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("0: SECSGEM not init ");
        return TRUE;
	}

	if (!IsMSAutoLineMode())	//v4.59A16
	{
        return TRUE;
	}

	CString szSlotPrefixName = "WL_Slot";
	CString szSlotID, szSlotName;
	CString szValue;

	//v4.57A15
	LONG lCurrSlotNo = lSlotNo;
	if (lCurrSlotNo <= 0)
	{
		lCurrSlotNo = 1;
	}

	INT nIndex = lCurrSlotNo - 1;
	if (nIndex < 0)
	{
		nIndex = 0;
	}
	if (nIndex >= WL_MAX_MGZN_SLOT)
	{
		nIndex = WL_MAX_MGZN_SLOT - 1;
	}

	try
	{
		szSlotID.Format("%d", lCurrSlotNo);
		szSlotName = szSlotPrefixName + szSlotID;

		CString szLog;
		szLog.Format("WL: SECS_UpdateCassetteSlotInfo - SLOT=%ld (Input=%d, INDEX=%d), Name=" + szSlotName, 
						lCurrSlotNo, lSlotNo, nIndex);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		(*m_pGemStation)[szSlotName].RemoveAll();

		//1. Slot ID	
		szValue.Format("%d", lCurrSlotNo);								
		(*m_pGemStation)[szSlotName].AddTail(szValue);					


		//2. Barcode
		CString szBC = m_stWaferMagazine[0].m_SlotBCName[nIndex];		//SLOT1 -> INDEX = 0
		if (bIsLoad && (nStatus == TRUE))
		{
			szBC = "";
		}
		(*m_pGemStation)[szSlotName].AddTail(szBC);						


		//3. Slot Usage
		CString szSlotUsage;
		szSlotUsage.Format("%ld", m_stWaferMagazine[0].m_lSlotUsage[nIndex]);
		if (bIsLoad && (nStatus != TRUE))	//v4.59A11
		{
			szSlotUsage = _T("0");	//WL_SLOT_USAGE_UNUSE
		}
		else if (!bIsLoad && bFull)
		{
			szSlotUsage = _T("1");	//WL_SLOT_USAGE_FULL
		}
		(*m_pGemStation)[szSlotName].AddTail(szSlotUsage);				


		//4. WIP, current remaining die no.
		szValue = "0";
	//	szValue.Format("%ld", m_stWaferMagazine[0].m_lSlotWIPCounter[nIndex]);
		(*m_pGemStation)[szSlotName].AddTail(szValue);


		//5. Bin grade, always 0 for wafer
		szValue = "0";													
		(*m_pGemStation)[szSlotName].AddTail(szValue);								


		szLog.Format("WL: SECS_UpdateCassetteSlotInfo Done - SLOT=%ld, BC=%s", lSlotNo, szBC);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CWaferLoader::SECS_UpdateCassetteSlotInfo Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferLoader::SECS_UpdateCassetteSlotInfo Exception");
//		DisplayException(e);
	}

	return TRUE;
}


//================================================================
// Function Name: 		SECS_UpdateLoadUnloadStatus
// Input arguments:		None
// Output arguments:	None
// Description:   		Update the current 
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::SECS_UpdateLoadUnloadStatus(CONST LONG lSlotNo, BOOL bIsLoad, LONG lStatus)
{
    if (IsSecsGemInit() == FALSE)
	{
        return TRUE;
	}

	try
	{
		CString szSlotID;
		szSlotID.Format("%d", lSlotNo);

		SetGemValue(MS_SECS_DV_AL_FRAME_TYPE,	"W");
		SetGemValue(MS_SECS_DV_AL_SLOT_ID,		szSlotID);

		if (lStatus == 0)
		{
			SetGemValue(MS_SECS_DV_AL_LOADUNLOAD_STATUS,	"0");  //Sucess
		}
		else
		{
			CString szStatus;
			szStatus.Format("%d", lStatus);
			SetGemValue(MS_SECS_DV_AL_LOADUNLOAD_STATUS,	szStatus); //Failure
		}

		CString szLog;
		szLog.Format("WL: SECS_UpdateLoadUnloadStatus - IsLoad=%ld, Status=%d", bIsLoad, lStatus);
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
		DisplayMessage("CWaferLoader::SECS_UpdateLoadUnloadStatus Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferLoader::SECS_UpdateLoadUnloadStatus Exception");
//		DisplayException(e);
	}

	return TRUE;
}



//================================================================
// Function Name: 		SECS_UploadbackupMapinUnload
// Input arguments:		None
// Output arguments:	None
// Description:   		Update the current 
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::SECS_UploadbackupMapinUnload()	//v4.59A13
{
	//For Renesas MS90 only
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() != CTM_RENESAS)
	{
		return TRUE;
	}

	if (!IsSecsGemInit())
	{
		return TRUE;
	}

	if ((m_WaferMapWrapper.GetReader() == NULL) || (!m_WaferMapWrapper.IsMapValid())) 
	{
		SetAlert_Red_Yellow(IDS_SEC_WL_UPLOADMAP_NOTVALID);
		return TRUE;
	}
		

	IPC_CServiceMessage stMsg;
	BOOL bReturn = TRUE;

	//Set filename into wafer table station
	INT nConvID = m_comClient.SendRequest(SECS_COMM_STN, "SC_SendBackupMap", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 3600000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	if (!bReturn)
	{
		HmiMessage_Red_Yellow("WL: start upload BACKUP map through SECS fails");	//IDS_SEC_WL_UPLOADMAP_FAIL		//v4.59A15
		return FALSE;
	}

	BOOL bStatus = TRUE; 
	INT nTimeOut = 0;

	while (!CMS896AStn::m_bSECSWaferMapUpLoadFinish)
	{
		Sleep(100);
		nTimeOut++;
		if (nTimeOut > 100)
		{
			bStatus = FALSE;

			SetAlert_Red_Yellow(IDS_SEC_WL_UPLOADMAP_TIMEOUT);
			//HmiMessage_Red_Yellow("WL: upload BACKUP map through SECS timeout !");	//IDS_SEC_WL_UPLOADMAP_TIMEOUT	//v4.59A15
			break;
		}
	}

	return bStatus;
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
VOID CWaferLoader::SendResetSlotEvent_7018(const LONG lSlot, BOOL bResetAll)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		if (bResetAll)
		{
			SetGemValue(MS_SECS_SV_SLOT_NO, 0);				//3004
		}
		else
		{
			SetGemValue(MS_SECS_SV_SLOT_NO, lSlot + 1);		//3004
		}
		SetGemValue(MS_SECS_SV_FRAME_TYPE, "W");			//3066
		SetGemValue(MS_SECS_SV_RESET_SLOT, 1);				//5032
		SendEvent(SG_CEID_RESET_SLOT);     //7018
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendResetSlotEvent_7018 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendResetSlotEvent_7018 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendGripperEvent_7100
// Input arguments:		lUnloadPos_X, lStepOnUnloadPos, lLoadPos_X, lStepOnLoadPos
//						lReadyPos_X, lBarcodePos_X, lTopSlotLevel_Z, dSlotPitch
//						lTotalSlotNo, lCurrentSlotNo, lSkipSlotNo
// Output arguments:	None
// Description:   		Send Reset Slot 
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendGripperEvent_7100(const LONG lUnloadPos_X, const LONG lStepOnUnloadPos, const LONG lLoadPos_X, const LONG lStepOnLoadPos,
										 const LONG lReadyPos_X, const LONG lBarcodePos_X, const LONG lTopSlotLevel_Z, const double dSlotPitch,
										 const LONG lTotalSlotNo, const LONG lCurrentSlotNo, const LONG lSkipSlotNo)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		// 3321
		SetGemValue("WL_XUnloadPosn",			lUnloadPos_X);
		SetGemValue("WL_XUnloadOffset",			lStepOnUnloadPos);
		SetGemValue("WL_XLoadPosn",				lLoadPos_X);
		SetGemValue("WL_XLoadOffset",			lStepOnLoadPos);
		SetGemValue("WL_XReadyPosn",			lReadyPos_X);
		SetGemValue("WL_XScanPosn",				lBarcodePos_X);
	
		// 3322
		SetGemValue("WL_MgznTopSlotLevel",		lTopSlotLevel_Z);
		SetGemValue("WL_MgznSlotPitch",			dSlotPitch);
		SetGemValue("WL_MgznTotalSlot",			lTotalSlotNo);

		SetGemValue("WL_MgznCurrentSlot",		lCurrentSlotNo);
		SetGemValue("WL_MgznSkipSlot",			lSkipSlotNo);
		// 7100
		SendEvent(SG_CEID_WL_GRIPPER_MGZN,		FALSE);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendGripperEvent_7100 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendGripperEvent_7100 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendGripper2Event_7100
// Input arguments:		lUnloadPos_X, lStepOnUnloadPos, lLoadPos_X, lStepOnLoadPos
//						lReadyPos_X, lBarcodePos_X, lMagazineNo, lCurrentSlotNo, lSkipSlotNo
// Output arguments:	None
// Description:   		Send Reset Slot 
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendGripper2Event_7100(const LONG lUnloadPos_X, const LONG lStepOnUnloadPos, const LONG lLoadPos_X, const LONG lStepOnLoadPos,
										 const LONG lReadyPos_X, const LONG lBarcodePos_X, const LONG lMagazineNo, const LONG lCurrentSlotNo, const LONG lSkipSlotNo)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		// 3321
		SetGemValue("WL_XUnloadPosn2",			lUnloadPos_X);
		SetGemValue("WL_XUnloadOffset2",		lStepOnUnloadPos);
		SetGemValue("WL_XLoadPosn2",			lLoadPos_X);
		SetGemValue("WL_XLoadOffset2",			lStepOnLoadPos);
		SetGemValue("WL_XReadyPosn2",			lReadyPos_X);
		SetGemValue("WL_XScanPosn2",			lBarcodePos_X);
	
		if (lMagazineNo >= 0 && lMagazineNo < GetWL2MaxMgznNo())
		{   
			// 3322
			SetGemValue("WL_MgznTopSlotLevel2", m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z);
			SetGemValue("WL_MgznSlotPitch2",	m_stWaferMagazine2[lMagazineNo].m_dSlotPitch);
			SetGemValue("WL_MgznTotalSlot2",	m_stWaferMagazine2[lMagazineNo].m_lNoOfSlots);
		}

		SetGemValue("WL_MgznCurrentSlot2",		lCurrentSlotNo);
		SetGemValue("WL_MgznSkipSlot2",			lSkipSlotNo);
		// 7100
		SendEvent(SG_CEID_WL_GRIPPER_MGZN,		FALSE);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendGripper2Event_7100 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendGripper2Event_7100 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendSetupEvent_7101
// Input arguments:		
// Output arguments:	None
// Description:   		Send Reset Slot 
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendSetupEvent_7101()
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		// 3326
		SetGemValue("WL_EnableBarCode",		m_bUseBarcode);
		SetGemValue("WL_ScanRange",			m_lScanRange);
		// 3327
		SetGemValue("WL_HAOpenTime",		m_lHotBlowOpenTime);
		SetGemValue("WL_HACloseTime",		m_lHotBlowCloseTime);
		SetGemValue("WL_HACoolTime",		m_lMylarCoolTime);
		// 3328
		SetGemValue("WL_WVDelayBeforeSuct", m_lVacuumBefore);
		SetGemValue("WL_WVDelayAfterSuct",	m_lVacuumAfter);
		// 3329
		SetGemValue("WL_LHSCount",			m_lAngleLHSCount);
		SetGemValue("WL_RHSCount",			m_lAngleRHSCount);
		SetGemValue("WL_SprialSize",		m_lSprialSize);
		// 7101
		SendEvent(SG_CEID_WL_SETUP, FALSE);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendSetupEvent_7101 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendSetupEvent_7101 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendWTUnloadPosnEvent_7102
// Input arguments:		lUnloadPhyPosX, lUnloadPhyPosY -- position
// Output arguments:	None
// Description:   		Send Reset Slot 
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendWTUnloadPosnEvent_7102(const LONG lUnloadPhyPosX, const LONG lUnloadPhyPosY)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		// 3331
		SetGemValue("WL_WTUnloadX", lUnloadPhyPosX);
		SetGemValue("WL_WTUnloadY", lUnloadPhyPosY);
		// 7102
		SendEvent(SG_CEID_WL_WTUNLOADPOSN, FALSE);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendWTUnloadPosnEvent_7102 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendWTUnloadPosnEvent_7102 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendWTUnloadPosn2Event_7102
// Input arguments:		lUnloadPhyPosX, lUnloadPhyPosY -- position
// Output arguments:	None
// Description:   		Send Reset Slot 
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendWTUnloadPosn2Event_7102(const LONG lUnloadPhyPosX, const LONG lUnloadPhyPosY)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		// 3331
		SetGemValue("WL_WTUnloadX2", lUnloadPhyPosX);		// 3331
		SetGemValue("WL_WTUnloadY2", lUnloadPhyPosY); 
		SendEvent(SG_CEID_WL_WTUNLOADPOSN, FALSE);			// 7102
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendWTUnloadPosn2Event_7102 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendWTUnloadPosn2Event_7102 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendHomeDieTablePosnEvent_7103
// Input arguments:		lHomeDiePhyPosX, lHomeDiePhyPosY -- position
// Output arguments:	None
// Description:   		Send 
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendHomeDieTablePosnEvent_7103(const LONG lHomeDiePhyPosX, const LONG lHomeDiePhyPosY)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		// 3336
		SetGemValue("WL_HomeDieTablePosnX", lHomeDiePhyPosX);
		SetGemValue("WL_HomeDieTablePosnY", lHomeDiePhyPosY);
		// 7103
		SendEvent(SG_CEID_WL_HOMEDIETABPOSN, FALSE);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendHomeDieTablePosnEvent_7103 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendHomeDieTablePosnEvent_7103 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendCassetteMappedEvent_8001
// Input arguments:		None
// Output arguments:	None
// Description:   		Send 
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendCassetteMappedEvent_8001()
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		//Set SECSGEM Value
		CString szRegName;
		for(INT i=1; i<=WL_MAX_MAG_SLOT; i++)
		{
			szRegName.Format("SlotID%02d", i);
			SetGemValue(szRegName, m_szWfrBarcodeInCass[i]);			//3010 + i
		}
		SendEvent(SG_CEID_CassetteMapped, TRUE);
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendCassetteMappedEvent_8001 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendCassetteMappedEvent_8001 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendLoadCassetteEvent_8002
// Input arguments:		None
// Output arguments:	None
// Description:   		Send the CEID#8002 after Load a cassette
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendLoadCassetteEvent_8002()
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		SetGemValue(MS_SECS_SV_CASSETTE_TYPE, "WC");	//3006
		SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");	//3005
		SendEvent(SG_CEID_LoadCassette, TRUE);			//8002
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendLoadCassetteEvent_8002 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendLoadCassetteEvent_8002 Exception");
//		DisplayException(e);
	}
}


//================================================================
// Function Name: 		SendUnloadCassetteEvent_8003
// Input arguments:		None
// Output arguments:	None
// Description:   		Send the CEID#8003 after unload a cassette
// Return:				None
// Remarks:				None
//================================================================
VOID CWaferLoader::SendUnloadCassetteEvent_8003()
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		SetGemValue(MS_SECS_SV_CASSETTE_TYPE, "WC");	//3006
		SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");	//3005
		SendEvent(SG_CEID_UnloadCassette, TRUE);		//8003
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CBinLoader::SendUnloadCassetteEvent_8003 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CBinLoader::SendUnloadCassetteEvent_8003 Exception");
//		DisplayException(e);
	}
}



INT CWaferLoader::SendGemEvent_WL_Load(BOOL bLoad, INT nStatus, BOOL bFull)
{
	LONG lCurrSlotNo = GetCurrSlotNo();		//1-based
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if( bLoad )
	{
		//Set SECSGEM Value
		//m_szWfrBarcodeInCass start from 1
		SetGemValue(MS_SECS_SV_WAFER_ID, m_szWfrBarcodeInCass[m_lCurrentSlotNo]);			//3003
		SetGemValue(MS_SECS_SV_SLOT_NO, m_lCurrentSlotNo);									//3004
		SendEvent(SG_CEID_WfrCassUnload, TRUE);											//8004

		//Set SECSGEM Value
		CString strMapFilePath = (*m_psmfSRam)["MS896A"]["MapFilePath"];
		SetGemValue(MS_SECS_SV_WAFER_ID, m_szWfrBarcodeInCass[m_lCurrentSlotNo]);			//3003
		SetGemValue(MS_SECS_SV_SLOT_NO, m_lCurrentSlotNo);									//3004
		SetGemValue(MS_SECS_SV_BIN_MAP_FILE_PATH, strMapFilePath);							//3036
		SetGemValue(MS_SECS_SV_BIN_MAP_FILE_NAME, m_szBarcodeName);							//3037

		if (nStatus == TRUE)
		{
			SECS_UpdateCassetteSlotInfo(m_lCurrentSlotNo, TRUE);							//v4.59A10	//Autoline		
		}

		SendEvent(SG_CEID_WfrTableLoad, TRUE);												//8007 for AutoLine
	}
	else
	{
		CString strMapFilePath = (*m_psmfSRam)["MS896A"]["MapFilePath"];
		SetGemValue(MS_SECS_SV_WAFER_ID,	m_szWfrBarcodeInCass[m_lCurrentSlotNo]);		//3003
		SetGemValue(MS_SECS_SV_SLOT_NO,		m_lCurrentSlotNo);								//3004
		SetGemValue(MS_SECS_SV_BIN_MAP_FILE_PATH,		strMapFilePath);					//3036
		SetGemValue(MS_SECS_SV_BIN_MAP_FILE_NAME,		m_szBarcodeName);					//3037
		
		SECS_UpdateCassetteSlotInfo(m_lCurrentSlotNo, FALSE, TRUE, bFull);					//v4.59A10	//Autoline

		SendEvent(SG_CEID_WfrTableUnload,	TRUE);											//8006 for autoline
		
		SetGemValue(MS_SECS_SV_WAFER_ID,	m_szWfrBarcodeInCass[m_lCurrentSlotNo]);		//3003
		SetGemValue(MS_SECS_SV_SLOT_NO,		m_lCurrentSlotNo);								//3004
		SendEvent(SG_CEID_WfrCassLoad,		TRUE);											//8005
		SendEvent(SG_CEID_WfrCassLoad,		TRUE);										//8005
	}

	return 1;
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
VOID CWaferLoader::SendBarcodeRejectEvent_8026(const LONG lCurSlot, const LONG lRejectCode)
{
	try
	{
		SetGemValue(MS_SECS_SV_SLOT_NO, lCurSlot + 1);		//3004
		SetGemValue(MS_SECS_SV_FRAME_TYPE, "W");			//3066
		SetGemValue(MS_SECS_SV_REJECT_CODE, lRejectCode);	//3067
		SendEvent(SG_CEID_BarcodeReject_AUTOLINE, TRUE);	//8026
	}
	catch (...) //CAsmException e)
	{
		DisplayMessage("CWaferLoader::SendBarcodeRejectEvent_8026 Exception");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferLoader::SendBarcodeRejectEvent_8026 Exception");
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
VOID CWaferLoader::SetSlotRejectStatus(const LONG lCurrMgzn, const LONG lCurSlot, const LONG lRejectCode)
{
	// set as reject status for the current slot
	SetCassetteSlotStatus(lCurrMgzn, lCurSlot,
						m_stWaferMagazine[lCurrMgzn].m_lSlotWIPCounter[lCurSlot],
						WL_SLOT_USAGE_UNUSE,
						m_stWaferMagazine[lCurrMgzn].m_SlotBCName[lCurSlot]);
	SendBarcodeRejectEvent_8026(lCurSlot, lRejectCode);
}
