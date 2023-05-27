#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "SecsComm.h"
#include "WaferLoader.h"
#include "BinLoader.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CSecsComm::GetFrameLoadUnloadPara(SHostCmmd *pHostCmmd, const CString szCommandTitle,
									   CString &szFrameType, CString &szSlotID, CString &szBarCode, CString &szBinGrade, CString &szWIP)
{
	CString szParam, szValue;

	szFrameType = "";
	szSlotID	= "";
	szBarCode	= "";
	szBinGrade	= "";
	szWIP		= "";

	for (int i = 0; i < pHostCmmd->nParameter; i++) 
	{
		szParam = pHostCmmd->arrCmmd[i].pchName;
		szParam.Replace("\n", "");
		szParam.Replace("\r", "");

		szValue = pHostCmmd->arrCmmd[i].pchValue;
		szValue.Replace("\n", "");
		szValue.Replace("\r", "");

		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szCommandTitle + "(" + szParam + " = " + szValue + ")");

		if (szParam == "FRAME_TYPE")
		{
			szFrameType = szValue;
		}
		else if (szParam == "SLOT_ID")
		{
			szSlotID = szValue;
		}
		else if (szParam == "BARCODE")
		{
			szBarCode = szValue;
		}
		else if (szParam == "BIN_GRADE")
		{
			szBinGrade = szValue;
		}
		else if (szParam == "WIP")
		{
			szWIP = szValue;
		}
	}
}

BOOL CSecsComm::CheckFrameLoadUnloadPara(const CString szFrameType, const CString szSlotID, const CString szBarCode,
								   const CString szBinGrade, const CString szWIP, CString &szParaErrName)
{
	if ((szFrameType != "W") && (szFrameType != "B"))
	{
		szParaErrName = "FRAME_TYPE";
		return FALSE;
	}

	if (szSlotID.IsEmpty())
	{
		szParaErrName = "SLOT_ID";
		return FALSE;
	}

	if (szBarCode.IsEmpty())
	{
		szParaErrName = "BARCODE";
		return FALSE;
	}

	if (szBinGrade.IsEmpty())
	{
		szParaErrName = "BIN_GRADE";
		return FALSE;
	}

	if (szWIP.IsEmpty())
	{
		szParaErrName = "WIP";
		return FALSE;
	}

	return TRUE;
}


VOID CSecsComm::GetFrameLoadUnloadCompletePara(SHostCmmd *pHostCmmd, const CString szCommandTitle,
											   CString &szFrameType, CString &szStatus, CString &szErrMsg)
{
	CString szParam, szValue;

	szFrameType = "";
	szStatus	= "";
	szErrMsg	= "";

	for (int i = 0; i < pHostCmmd->nParameter; i++) 
	{
		szParam = pHostCmmd->arrCmmd[i].pchName;
		szParam.Replace("\n", "");
		szParam.Replace("\r", "");

		szValue = pHostCmmd->arrCmmd[i].pchValue;
		szValue.Replace("\n", "");
		szValue.Replace("\r", "");

		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szCommandTitle + "(" + szParam + " = " + szValue + ")");

		if (szParam == "FRAME_TYPE")
		{
			szFrameType = szValue;
		}
		else if (szParam == "STATUS")
		{
			szStatus = szValue;
		}
		else if (szParam == "ERROR_CODE")
		{
			szErrMsg = szValue;
		}
	}
}


BOOL CSecsComm::CheckFrameLoadUnloadCompletePara(const CString szFrameType, const CString szStatus, CString &szParaErrName)
{
	if ((szFrameType != "W") && (szFrameType != "B"))
	{
		szParaErrName = "FRAME_TYPE";
		return FALSE;
	}

	if (szStatus.IsEmpty())
	{
		szParaErrName = "STATUS";
		return FALSE;
	}

	return TRUE;
}

BOOL CSecsComm::IsLoadUnloadObjectLocked(CString szFrameType)
{
	if (szFrameType == "B")
	{
		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (pBinLoader)
		{
			return pBinLoader->IsLoadUnloadSECSCmdLocked();
		}
		return FALSE;
	}

	if (szFrameType == "W")
	{
		CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
		if (pWaferLoader)
		{
			return pWaferLoader->IsLoadUnloadSECSCmdLocked();
		}
	}
	return FALSE;
}


VOID CSecsComm::SetS2F41Ack(SHostCmmdErr *pstHostCmmdErr, 
							const UCHAR	ucCommandAck, const CString szParaErrName,  UCHAR ucParaErrAck)
{
	pstHostCmmdErr->Ack = ucCommandAck; // Acknowledge, At least one parameter is invalid

	if (!szParaErrName.IsEmpty())
	{
		pstHostCmmdErr->nErr = 1;
		strcpy_s(pstHostCmmdErr->arrParaErr[0].pchName, sizeof(pstHostCmmdErr->arrParaErr[0].pchName), (const char*)szParaErrName);
		pstHostCmmdErr->arrParaErr[0].ucAck = ucParaErrAck; //Parameter Value (CPVALUE) is not desired for command
	}
	else
	{
		pstHostCmmdErr->nErr = 0;
	}
}

BOOL CSecsComm::S2F41HostCmd_FrameLoad(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr)
{
	CString szFrameType = "";
	CString szSlotID	= "";
	CString szBarCode	= "";
	CString szBinGrade	= "";
	CString szWIP		= "";

	GetFrameLoadUnloadPara(pHostCmmd, CString("\t<<< S2F41 -- FRAME_LOAD "),
						   szFrameType, szSlotID, szBarCode, szBinGrade, szWIP);

	if (IsLoadUnloadObjectLocked(szFrameType))
	{
		CString szLog;
		szLog.Format("Reject S2F41HostCmd_FrameLoad becasue MS is in Load/Unload status");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		SetS2F41Ack(pstHostCmmdErr, HCACK_CANNOT_PERFORM_NOW);
		return FALSE;
	}

	//Check Parameter Name
	CString szLog;
	CString szParaErrName;
	if (!CheckFrameLoadUnloadPara(szFrameType, szSlotID, szBarCode, szBinGrade, szWIP, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("INVALID FRAME_LOAD cmd received - %s, %s, %s, %s, %s", szFrameType, szSlotID, szBarCode, szBinGrade, szWIP); 
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	LONG lSlotID = 0, lGrade = 0;
	LONG lMinRange	= (szFrameType == "W") ? MS_WL_AUTO_LINE_INPUT_START_MGZN_SLOT : MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT;
	LONG lMaxRange	= (szFrameType == "W") ? MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT : MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT;
	BOOL bIsWL		= (szFrameType == "W") ? TRUE : FALSE;

	CMapElement *pElement = bIsWL ? &(*m_psmfSRam)["MS"]["AutoLineWL"] : &(*m_psmfSRam)["MS"]["AutoLine"];
	if (!DecodeBulkLoadSlotIDs(TRUE, bIsWL, pElement, lMinRange, lMaxRange,
							   szSlotID, szBarCode, szBinGrade, szWIP, lSlotID, lGrade, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("\t\t<<< -- FRAME_LOAD Parameter Invalid");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}
	
	if (IsRepeatedBarcodeNameinBulkLoadData(pElement, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("\t\t<<< -- FRAME_LOAD Repeated Barcode Name");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	typedef struct 
	{
		LONG	lSlotID;
		LONG	lSlotGrade;	
	} MSAUTOLINESETUP;

	MSAUTOLINESETUP stInfo;
	stInfo.lSlotID		= lSlotID;
	stInfo.lSlotGrade	= lGrade;

	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(MSAUTOLINESETUP), &stInfo);
	INT nConvID = 0;

	LONG lRet = 0;
	if (szFrameType == "W")			//Wafer
	{
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "SECS_LoadFrameCmd", stMsg);	//autoline1
	}
	else							//Bin
	{
		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (pBinLoader)
		{
			lRet = pBinLoader->SECS_SubLoadFrameCmd(lSlotID, lGrade);
		}
		else
		{
			lRet = HCACK_CANNOT_PERFORM_NOW;
		}
	}

//  EM does Not Wait for result becasue there has timeout issue(more than 10 second) in EM
//	if (!GetIPCReply(nConvID, lRet))
//	{
//		lRet = HCACK_CANNOT_PERFORM_NOW;
//	}
	szLog.Format("\t\t<<< -- FRAME_LOAD (TYPE=%s, SLOTID=%s (%d), BC=%s, GRADE=%s), IPC-ID=%d, lRet = %d", 
					szFrameType, szSlotID, stInfo.lSlotID, szBarCode, szBinGrade, nConvID, lRet);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if (lRet)
	{
		SetS2F41Ack(pstHostCmmdErr, (UCHAR)lRet);
	}
	return TRUE;
}

BOOL CSecsComm::S2F41HostCmd_FrameLoadComplete(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr)
{
	CString szFrameType = "";
	CString szStatus	= "";
	CString szErrMsg	= "";
	GetFrameLoadUnloadCompletePara(pHostCmmd, CString("\t<<< S2F41 -- FRAME_LOAD_COMPLETE "),
								   szFrameType, szStatus, szErrMsg);

	//Check Parameter Name
	CString szLog;
	CString szParaErrName;
	if (!CheckFrameLoadUnloadCompletePara(szFrameType, szStatus, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("INVALID FRAME_LOAD_COMPLETE cmd received - %s %s", szFrameType, szStatus); 
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	LONG lStatus = 0;
	lStatus	= atol((LPCTSTR) szStatus);

	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(LONG), &lStatus);
	INT nConvID = 0;

	LONG lRet = 0;
	if (szFrameType == "W")			//Wafer
	{
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "SECS_LoadFrameDoneCmd", stMsg);	//autoline1
		if (!GetIPCReply(nConvID, lRet))
		{
			lRet = HCACK_CANNOT_PERFORM_NOW;
		}
	}
	else							//Bin
	{
		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (pBinLoader)
		{
			lRet = pBinLoader->SECS_SubLoadFrameDoneCmd(lStatus);
		}
		else
		{
			lRet = HCACK_CANNOT_PERFORM_NOW;
		}
	}

	szLog.Format("\t\t<<< -- FRAME_LOAD_COMPLETE (TYPE=%s, STATUS=%s, ERRMSG=%s, lRet = %d)", 
					szFrameType, szStatus, szErrMsg, lRet);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if (lRet)
	{
		SetS2F41Ack(pstHostCmmdErr, (UCHAR)lRet);
	}

	return TRUE;
}

BOOL CSecsComm::S2F41HostCmd_FrameUnload(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr)
{
	CString szFrameType = "";
	CString szSlotID	= "";
	CString szBarCode	= "";
	CString szBinGrade	= "";
	CString szWIP		= "";

	GetFrameLoadUnloadPara(pHostCmmd, CString("\t<<< S2F41 -- FRAME_UNLOAD"),
						   szFrameType, szSlotID, szBarCode, szBinGrade, szWIP);

	if (IsLoadUnloadObjectLocked(szFrameType))
	{
		CString szLog;
		szLog.Format("Reject S2F41HostCmd_FrameUnload because MS is in Load/Unload status");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		SetS2F41Ack(pstHostCmmdErr, HCACK_CANNOT_PERFORM_NOW);
		return FALSE;
	}

	//Check Parameter Name
	CString szLog;
	CString szParaErrName;
	if (!CheckFrameLoadUnloadPara(szFrameType, szSlotID, szBarCode, szBinGrade, szWIP, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("INVALID FRAME_UNLOAD cmd received - %s %s", szFrameType, szSlotID); 
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}


	LONG lSlotID=0, lGrade=0;

	LONG lMinRange	= (szFrameType == "W") ? MS_WL_AUTO_LINE_OUTPUT_START_MGZN_SLOT : MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT;
	LONG lMaxRange	= (szFrameType == "W") ? MS_WL_AUTO_LINE_OUTPUT_END_MGZN_SLOT : MS_BL_AUTO_LINE_OUTPUT_END_MGZN_SLOT;
	BOOL bIsWL		= (szFrameType == "W") ? TRUE : FALSE;

	CMapElement *pElement = bIsWL ? &(*m_psmfSRam)["MS"]["AutoLineWL"] : &(*m_psmfSRam)["MS"]["AutoLine"];
	if (!DecodeBulkLoadSlotIDs(FALSE, bIsWL, pElement, lMinRange, lMaxRange,
							   szSlotID, szBarCode, szBinGrade, szWIP, lSlotID, lGrade, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("\t\t<<< -- FRAME_UNLOAD Parameter Invalid");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	if (IsRepeatedBarcodeNameinBulkLoadData(pElement, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("\t\t<<< -- FRAME_UNLOAD Repeated Barcode Name");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}


	typedef struct 
	{
		LONG	lSlotID;
		LONG	lSlotGrade;	
	} MSAUTOLINESETUP;

	MSAUTOLINESETUP stInfo;
	stInfo.lSlotID		= lSlotID;
	stInfo.lSlotGrade	= lGrade;

	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(MSAUTOLINESETUP), &stInfo);
	INT nConvID = 0;

	LONG lRet = 0;
	if (szFrameType == "W")			//Wafer
	{
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "SECS_UnloadFrameCmd", stMsg);	//autoline1
	}
	else							//Bin
	{
		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (pBinLoader)
		{
			lRet = pBinLoader->SECS_SubUnloadFrameCmd(lSlotID, lGrade);
		}
		else
		{
			lRet = HCACK_CANNOT_PERFORM_NOW;
		}
	}

//  EM does Not Wait for result becasue there has timeout issue(more than 10 second) in EM
//	if (!GetIPCReply(nConvID, lRet))
//	{
//		lRet = HCACK_CANNOT_PERFORM_NOW;
//	}

	szLog.Format("\t\t<<< -- FRAME_UNLOAD (TYPE=%s, SLOTID=%s (%ld), BC=%s, GRADE=%s), IPC-ID=%d, lRet = %d", 
					szFrameType, szSlotID, stInfo.lSlotID, szBarCode, szBinGrade, nConvID, lRet);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if (lRet)
	{
		SetS2F41Ack(pstHostCmmdErr, (UCHAR)lRet);
	}

	return TRUE;
}

BOOL CSecsComm::S2F41HostCmd_FrameUnloadComplete(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr)
{
	CString szFrameType = "";
	CString szStatus	= "";
	CString szErrMsg	= "";
	GetFrameLoadUnloadCompletePara(pHostCmmd, CString("\t<<< S2F41 -- FRAME_UNLOAD_COMPLETE "),
								   szFrameType, szStatus, szErrMsg);
	//Check Parameter Name
	CString szLog;
	CString szParaErrName;
	if (!CheckFrameLoadUnloadCompletePara(szFrameType, szStatus, szParaErrName))
	{
		SetS2F41Ack(pstHostCmmdErr, HCACK_PARA_INVALID, szParaErrName, CPACK_ILLEGAL_VALUE);

		szLog.Format("INVALID FRAME_UNLOAD COMPLETE cmd received - %s %s", szFrameType, szStatus); 
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		return FALSE;
	}

	LONG lStatus = 0;
	lStatus	= atol((LPCTSTR) szStatus);

	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(LONG), &lStatus);
	INT nConvID = 0;

	LONG lRet = 0;
	if (szFrameType == "W")			//Wafer
	{
		nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "SECS_UnloadFrameDoneCmd", stMsg);	//autoline1
		if (!GetIPCReply(nConvID, lRet))
		{
			lRet = HCACK_CANNOT_PERFORM_NOW;
		}
	}
	else							//Bin
	{
		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (pBinLoader)
		{
			lRet = pBinLoader->SECS_SubUnloadFrameDoneCmd(lStatus);
		}
		else
		{
			lRet = HCACK_CANNOT_PERFORM_NOW;
		}
	}

	szLog.Format("\t\t<<< -- FRAME_UNLOAD_COMPLETE (TYPE=%s, STATUS=%s, ERRMSG=%s, lRet = %d)", 
					szFrameType, szStatus, szErrMsg, lRet);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if (lRet)
	{
		SetS2F41Ack(pstHostCmmdErr, (UCHAR)lRet);
	}

	return TRUE;
}

BOOL CSecsComm::ResetBulkLoadData(CMapElement *pElement)
{
	CString szSlotIDTitle, szFrameBarCodeTitle, szSlotGradeTitle, szSlotWIPTitle;

	for (LONG lSlotNo = 1; lSlotNo <= 5; lSlotNo++)
	{
		szSlotIDTitle.Format("SlotID%d", lSlotNo);
		if (lSlotNo == 1)
		{
			szFrameBarCodeTitle = _T("FrameBarCode");
		}
		else
		{
			szFrameBarCodeTitle.Format("FrameBarCode%d", lSlotNo);
		}
		szSlotGradeTitle.Format("Slot%dGrade", lSlotNo);
		szSlotWIPTitle.Format("Slot%dWIP", lSlotNo);

		(*pElement)[szSlotIDTitle]			= 0;
		(*pElement)[szFrameBarCodeTitle]	= "";
		(*pElement)[szSlotGradeTitle]		= 0;
		(*pElement)[szSlotWIPTitle]			= 0;
	}
/*
	(*pElement)["FrameBarCode"]		= "";
	(*pElement)["FrameBarCode2"]	= "";
	(*pElement)["FrameBarCode3"]	= "";
	(*pElement)["FrameBarCode4"]	= "";
	(*pElement)["FrameBarCode5"]	= "";

	(*pElement)["SlotID1"] = 0;
	(*pElement)["SlotID2"] = 0;
	(*pElement)["SlotID3"] = 0;
	(*pElement)["SlotID4"] = 0;
	(*pElement)["SlotID5"] = 0;

	(*pElement)["Slot1Grade"] = 0;
	(*pElement)["Slot2Grade"] = 0;
	(*pElement)["Slot3Grade"] = 0;
	(*pElement)["Slot4Grade"] = 0;
	(*pElement)["Slot5Grade"] = 0;

	(*pElement)["Slot1WIP"] = 0;
	(*pElement)["Slot2WIP"] = 0;
	(*pElement)["Slot3WIP"] = 0;
	(*pElement)["Slot4WIP"] = 0;
	(*pElement)["Slot5WIP"] = 0;
*/
	return TRUE;
}


BOOL CSecsComm::IsRepeatedBarcodeNameinBulkLoadData(CMapElement *pElement, CString &szParaErrName)
{
	CString szFrameBarCodeTitle, szFrameBarCodeTitle2;

	for (LONG lSlotNo = 1; lSlotNo <= 5; lSlotNo++)
	{
		if (lSlotNo == 1)
		{
			szFrameBarCodeTitle = _T("FrameBarCode");
		}
		else
		{
			szFrameBarCodeTitle.Format("FrameBarCode%d", lSlotNo);
		}

		CString szBCName = (*pElement)[szFrameBarCodeTitle];

		if (!szBCName.IsEmpty())
		{
			for (LONG j = lSlotNo + 1; j <= 5; j++)
			{
				szFrameBarCodeTitle2.Format("FrameBarCode%d", j);

				CString szBCName2 = (*pElement)[szFrameBarCodeTitle2];

				if (!szBCName2.IsEmpty() && (szBCName == szBCName2))
				{
					szParaErrName = szFrameBarCodeTitle;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CSecsComm::IsValidSlotID(const LONG lSlotID, const LONG lMinRange, const LONG lMaxRange)
{
	//return ((lSlotID >= lMinRange) && (lSlotID <= lMaxRange));
	return TRUE;
}

BOOL CSecsComm::IsBulkItemDataValid(const BOOL bLoad, const BOOL bIsWL, const BOOL bOnlyOneSlot, const BOOL bFirstSlot, const CString szSlotIDTitle, const CString szSlotGradeTitle, 
									const CString szFrameBarCodeTitle, const CString szFrameWIPCounterTitle, 
									const LONG lSlotID, const LONG lGrade, const CString szBarcode, const LONG lWIPCounter, CString &szParaErrName)
{
	if (lGrade > BT_MAX_BINBLK_NO)
	{
		szParaErrName = szSlotGradeTitle + CString("More Than 150");
		return FALSE;
	}

	if (bIsWL)
	{
		CWaferLoader *pWaferLoader = dynamic_cast<CWaferLoader*>(GetStation(WAFER_LOADER_STN));
		if (pWaferLoader)
		{
			if (bLoad)
			{
				if (!pWaferLoader->CheckHostLoadBarcodeID(szSlotIDTitle, szFrameBarCodeTitle, lSlotID, szBarcode, szParaErrName))
				{
					return FALSE;
				}
			} 
			else if (!pWaferLoader->CheckHostUnloadBarcodeID(bOnlyOneSlot, bFirstSlot, szSlotIDTitle, szFrameBarCodeTitle, lSlotID, szBarcode, szParaErrName))
			{
				return FALSE;
			}
		}
	}
	else
	{
		CBinLoader *pBinLoader = dynamic_cast<CBinLoader*>(GetStation(BIN_LOADER_STN));
		if (pBinLoader)
		{
			if (bLoad)
			{
				if (!pBinLoader->CheckHostLoadBarcodeID(szSlotIDTitle, szFrameBarCodeTitle, lSlotID, szBarcode, szParaErrName))
				{
					return FALSE;
				}
			}
			else if (!pBinLoader->CheckHostUnloadBarcodeID(bOnlyOneSlot, bFirstSlot, szSlotIDTitle, szSlotGradeTitle, szFrameBarCodeTitle, szFrameWIPCounterTitle, 
														   lSlotID, lGrade, szBarcode, lWIPCounter, szParaErrName))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CSecsComm::DecodeItemBulkLoadSlotIDs(const BOOL bLoad, const BOOL bIsWL, CMapElement *pElement, const BOOL bOnlyOneSlot, const LONG lSlotNo,
										  CString &szBulkSlotID, CString &szBulkBarCode,
										  CString &szBulkGrade, 
										  CString &szBulkWIP,
										  LONG &lCurSlotID, LONG &lCurGrade,
										  CString &szParaErrName)

{
	CString szSlotIDTitle, szFrameBarCodeTitle, szSlotGradeTitle, szSlotWIPTitle;
	CString szID, szBC, szGrade, szWIP, szLog;
	LONG lID = 0, lGrade = 0, lWIP = 0;
	BOOL bFirstSlot = FALSE;

	szSlotIDTitle.Format("SlotID%d", lSlotNo);
	if (lSlotNo == 1)
	{
		szFrameBarCodeTitle = _T("FrameBarCode");
		bFirstSlot = TRUE;
	}
	else
	{
		szFrameBarCodeTitle.Format("FrameBarCode%d", lSlotNo);
	}
	szSlotGradeTitle.Format("Slot%dGrade", lSlotNo);
	szSlotWIPTitle.Format("Slot%dWIP", lSlotNo);

	szLog.Format("%d : [%s], [%s], [%s], [%s]", lSlotNo, szSlotIDTitle, szFrameBarCodeTitle, szSlotGradeTitle, szSlotWIPTitle);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
/*
	if (!bIsWL && (lSlotNo == 5))
	{
		//exceed 5 bin frames in host command
		return FALSE;
	}

	if (bIsWL && (lSlotNo == 3))
	{
		//exceed 3 wafer frames in host command
		return FALSE;
	}
*/
	if (szBulkSlotID.GetLength() == 0)
	{
		return FALSE;
	}

	INT nIndex		= szBulkSlotID.Find("#");
	INT nBCIndex	= szBulkBarCode.Find("#");
	INT nGIndex		= szBulkGrade.Find("#");
	INT nWIPIndex	= szBulkWIP.Find("#");
	
	if (nIndex == -1)
	{
		//Last one
		lID = atol((LPCTSTR) szBulkSlotID);
		(*pElement)[szSlotIDTitle] = lID;
		lCurSlotID = lID;

		if (nBCIndex == -1)
		{
			szBC = szBulkBarCode;
		}
		else
		{
			szBC = szBulkBarCode.Left(nBCIndex);
		}
		(*pElement)[szFrameBarCodeTitle] = szBC;

		if (nGIndex == -1)
		{
			szGrade = szBulkGrade;
		}
		else
		{
			szGrade = szBulkGrade.Left(nGIndex);
		}
		lGrade = atol((LPCTSTR) szGrade);
		(*pElement)[szSlotGradeTitle] = lGrade;
		lCurGrade = lGrade;

		if (nWIPIndex == -1)
		{
			szWIP = szBulkWIP;
		}
		else
		{
			szWIP = szBulkWIP.Left(nWIPIndex);
		}

		lWIP = atol((LPCTSTR) szWIP);
		(*pElement)[szSlotWIPTitle] = lWIP;

		szLog.Format("%d : Slot #%d, BC = %s, Grade = %s, WIP = %s", lSlotNo, lID, szBC, szGrade, szWIP);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		szBulkSlotID = szBulkBarCode = szBulkGrade = szBulkWIP = "";

		if (!IsBulkItemDataValid(bLoad, bIsWL, bOnlyOneSlot, bFirstSlot, szSlotIDTitle, szSlotGradeTitle, szFrameBarCodeTitle, szSlotWIPTitle, lID, lGrade, szBC, lWIP, szParaErrName))
		{
			return FALSE;
		}
		return TRUE;
	}

	if ((nBCIndex == -1) || (nGIndex == -1) || (nWIPIndex == -1))
	{
		return FALSE;
	}

	szID = szBulkSlotID.Left(nIndex);
	szBulkSlotID = szBulkSlotID.Mid(nIndex + 1);
	lID = atol((LPCTSTR) szID);
	(*pElement)[szSlotIDTitle] = lID;
	lCurSlotID = lID;

	szBC = szBulkBarCode.Left(nBCIndex);
	szBulkBarCode = szBulkBarCode.Mid(nBCIndex + 1);
	(*pElement)[szFrameBarCodeTitle] = szBC;

	szGrade = szBulkGrade.Left(nGIndex);
	lGrade = atol((LPCTSTR) szGrade);
	szBulkGrade = szBulkGrade.Mid(nGIndex + 1);
	(*pElement)[szSlotGradeTitle] = lGrade;
	lCurGrade = lGrade;

	szWIP = szBulkWIP.Left(nWIPIndex);
	lWIP = atol((LPCTSTR) szWIP);
	szBulkWIP = szBulkWIP.Mid(nWIPIndex + 1);
	(*pElement)[szSlotWIPTitle] = lWIP;

	szLog.Format("%d : Slot #%d, BC = %s, Grade = %s, WIP = %s, szBulkWIP = %s", lSlotNo, lID, szBC, szGrade, szWIP, szBulkWIP);

	lID		= (*pElement)[szSlotIDTitle];
	szBC	= (*pElement)[szFrameBarCodeTitle];
	lGrade	= (*pElement)[szSlotGradeTitle];
	lWIP	= (*pElement)[szSlotWIPTitle];
	szLog.Format("%d : Slot #%d, BC = %s, Grade = %s, WIP = %s, szBulkWIP = %s", lSlotNo, lID, szBC, szGrade, szWIP, szBulkWIP);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	if (!IsBulkItemDataValid(bLoad, bIsWL, bOnlyOneSlot, bFirstSlot, szSlotIDTitle, szSlotGradeTitle, szFrameBarCodeTitle, szSlotWIPTitle, lID, lGrade, szBC, lWIP, szParaErrName))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CSecsComm::IsOnlyOneSlot(const CString szBarCode)
{
	CString szBulkBarCode	= szBarCode;
	CString szBC;
	LONG lCount = 0;

	for (LONG lSlotNo = 1; lSlotNo <= 5; lSlotNo++)
	{
		INT nBCIndex	= szBulkBarCode.Find("#");
		if (nBCIndex == -1)
		{
			return (lCount == 1);
		}
		szBC = szBulkBarCode.Left(nBCIndex);
		if (!szBC.IsEmpty())
		{
			lCount++;
		}
		szBulkBarCode = szBulkBarCode.Mid(nBCIndex + 1);
	}

	return (lCount == 1);
}

BOOL CSecsComm::DecodeBulkLoadSlotIDs(const BOOL bLoad, const BOOL bIsWL, CMapElement *pElement, const LONG lMinRange, const LONG lMaxRange,
									  const CString szSlotID, const CString szBarCode, 
									  const CString szBinGrade, const CString szBinWIP,
									  LONG& lSlotID1, LONG& lGrade1,
									  CString &szParaErrName)
{
	CString szBulkSlotID	= szSlotID;
	CString szBulkBarCode	= szBarCode;
	CString szBulkGrade		= szBinGrade;
	CString szBulkWIP		= szBinWIP;
	CString szID, szBC, szGrade, szWIP, szLog;
	LONG lID = 0, lGrade = 0, lWIP = 0;

	BOOL bOnlyOneSlot = IsOnlyOneSlot(szBarCode);

	ResetBulkLoadData(pElement);

	for (LONG lSlotNo = 1; lSlotNo <= 5; lSlotNo++)
	{
		if (!DecodeItemBulkLoadSlotIDs(bLoad, bIsWL, pElement, bOnlyOneSlot, lSlotNo, szBulkSlotID, szBulkBarCode,
									  szBulkGrade, szBulkWIP, lID, lGrade, szParaErrName))
		{
			return FALSE;
		}

		if (lSlotNo == 1)
		{
			lSlotID1	= lID;
			lGrade1		= lGrade;
		}

		if (szBulkSlotID.IsEmpty())
		{
			return TRUE;
		}
	}
	return TRUE;

/*

	//Slot #1
	INT nIndex		= szBulkSlotID.Find("#");
	INT nBCIndex	= szBulkBarCode.Find("#");
	INT nGIndex		= szBulkGrade.Find("#");
	INT nWIPIndex	= szBulkWIP.Find("#");
	if (nIndex == -1)
	{
		lID = atol((LPCTSTR) szBulkSlotID);
		(*pElement)["SlotID1"] = lID;
		lSlotID1 = lID;
		if (!IsValidSlotID(lID, lMinRange, lMaxRange))
		{
			return FALSE;
		}

		szBC = szBulkBarCode;
		(*pElement)["FrameBarCode"] = szBC;

		szGrade = szBulkGrade;
		lGrade = atol((LPCTSTR) szGrade);
		(*pElement)["Slot1Grade"] = lGrade;
		lGrade1 = lGrade;

		szWIP = szBulkWIP;
		lWIP = atol((LPCTSTR) szWIP);
		(*pElement)["Slot1WIP"] = lWIP;

		szLog.Format("1 : Slot #%d, BC = %s, Grade = %s, WIP = %s", lID, szBC, szGrade, szWIP);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		return TRUE;
	}
	if ((nBCIndex == -1) || (nGIndex == -1) || (nWIPIndex == -1))
	{
		return FALSE;
	}
	szID = szBulkSlotID.Left(nIndex);
	szBulkSlotID = szBulkSlotID.Mid(nIndex + 1);
	lID = atol((LPCTSTR) szID);
	(*pElement)["SlotID1"] = lID;
	lSlotID1 = lID;
	if (!IsValidSlotID(lID, lMinRange, lMaxRange))
	{
		return FALSE;
	}
	
	szBC = szBulkBarCode.Left(nBCIndex);
	szBulkBarCode = szBulkBarCode.Mid(nBCIndex + 1);
	(*pElement)["FrameBarCode"] = szBC;

	szGrade = szBulkGrade.Left(nGIndex);
	lGrade = atol((LPCTSTR) szGrade);
	szBulkGrade = szBulkGrade.Mid(nGIndex + 1);
	(*pElement)["Slot1Grade"] = lGrade;

	szWIP = szBulkWIP.Left(nWIPIndex);
	lWIP = atol((LPCTSTR) szWIP);
	szBulkWIP = szBulkWIP.Mid(nWIPIndex + 1);
	(*pElement)["Slot1WIP"] = lWIP;

	szLog.Format("1 : Slot #%d, BC = %s, Grade = %s, WIP = %s, szBulkWIP = %s", lID, szBC, szGrade, szWIP, szBulkWIP);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	lGrade1 = atol((LPCTSTR) szGrade);

	//Slot #2
	nIndex		= szBulkSlotID.Find("#");
	nBCIndex	= szBulkBarCode.Find("#");
	nGIndex		= szBulkGrade.Find("#");
	nWIPIndex	= szBulkWIP.Find("#");
	if (nIndex == -1)
	{
		lID = atol((LPCTSTR) szBulkSlotID);
		(*pElement)["SlotID2"] = lID;
		if (!IsValidSlotID(lID, lMinRange, lMaxRange))
		{
			return FALSE;
		}

		szBC = szBulkBarCode;
		(*pElement)["FrameBarCode2"] = szBC;

		szGrade = szBulkGrade;
		lGrade = atol((LPCTSTR) szGrade);
		(*pElement)["Slot2Grade"] = lGrade;

		szWIP = szBulkWIP;
		lWIP = atol((LPCTSTR) szWIP);
		(*pElement)["Slot2WIP"] = lWIP;

		szLog.Format("2 : Slot #%d, BC = %s, Grade = %s, WIP = %s", lID, szBC, szGrade, szWIP);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		return TRUE;
	}
	if ((nBCIndex == -1) || (nGIndex == -1) || (nWIPIndex == -1))
	{
		return FALSE;
	}

	szID = szBulkSlotID.Left(nIndex);
	szBulkSlotID = szBulkSlotID.Mid(nIndex + 1);
	lID = atol((LPCTSTR) szID);
	(*pElement)["SlotID2"] = lID;
	if (!IsValidSlotID(lID, lMinRange, lMaxRange))
	{
		return FALSE;
	}

	szBC = szBulkBarCode.Left(nBCIndex);
	szBulkBarCode = szBulkBarCode.Mid(nBCIndex + 1);
	(*pElement)["FrameBarCode2"] = szBC;

	szGrade = szBulkGrade.Left(nGIndex);
	lGrade = atol((LPCTSTR) szGrade);
	szBulkGrade = szBulkGrade.Mid(nGIndex + 1);
	(*pElement)["Slot2Grade"] = lGrade;

	szWIP = szBulkWIP.Left(nWIPIndex);
	lWIP = atol((LPCTSTR) szWIP);
	szBulkWIP = szBulkWIP.Mid(nWIPIndex + 1);
	(*pElement)["Slot2WIP"] = lWIP;

	szLog.Format("2 : Slot #%d, BC = %s, Grade = %s, WIP = %s, szBulkWIP = %s", lID, szBC, szGrade, szWIP, szBulkWIP);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);


	//Slot #3
	nIndex		= szBulkSlotID.Find("#");
	nBCIndex	= szBulkBarCode.Find("#");
	nGIndex		= szBulkGrade.Find("#");
	nWIPIndex	= szBulkWIP.Find("#");
	if (nIndex == -1)
	{
		lID = atol((LPCTSTR) szBulkSlotID);
		(*pElement)["SlotID3"] = lID;
		if (!IsValidSlotID(lID, lMinRange, lMaxRange))
		{
			return FALSE;
		}

		szBC = szBulkBarCode;
		(*pElement)["FrameBarCode3"] = szBC;

		szGrade = szBulkGrade;
		lGrade = atol((LPCTSTR) szGrade);
		(*pElement)["Slot3Grade"] = lGrade;

		szWIP = szBulkWIP;
		lWIP = atol((LPCTSTR) szWIP);
		(*pElement)["Slot3WIP"] = lWIP;

		szLog.Format("3 : Slot #%d, BC = %s, Grade = %s, WIP = %s", lID, szBC, szGrade, szWIP);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		return TRUE;
	}
	if ((nBCIndex == -1) || (nGIndex == -1) || (nWIPIndex == -1))
	{
		return FALSE;
	}

	szID = szBulkSlotID.Left(nIndex);
	szBulkSlotID = szBulkSlotID.Mid(nIndex + 1);
	lID = atol((LPCTSTR) szID);
	(*pElement)["SlotID3"] = lID;
	if (!IsValidSlotID(lID, lMinRange, lMaxRange))
	{
		return FALSE;
	}

	szBC = szBulkBarCode.Left(nBCIndex);
	szBulkBarCode = szBulkBarCode.Mid(nBCIndex + 1);
	(*pElement)["FrameBarCode3"] = szBC;

	szGrade = szBulkGrade.Left(nGIndex);
	lGrade = atol((LPCTSTR) szGrade);
	szBulkGrade = szBulkGrade.Mid(nGIndex + 1);
	(*pElement)["Slot3Grade"] = lGrade;

	szWIP = szBulkWIP.Left(nWIPIndex);
	lWIP = atol((LPCTSTR) szWIP);
	szBulkWIP = szBulkWIP.Mid(nWIPIndex + 1);
	(*pElement)["Slot3WIP"] = lWIP;

	szLog.Format("3 : Slot #%d, BC = %s, Grade = %s, WIP = %s, szBulkWIP = %s", lID, szBC, szGrade, szWIP, szBulkWIP);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);


	//Slot #4
	nIndex		= szBulkSlotID.Find("#");
	nBCIndex	= szBulkBarCode.Find("#");
	nGIndex		= szBulkGrade.Find("#");
	nWIPIndex	= szBulkWIP.Find("#");
	if (nIndex == -1)
	{
		lID = atol((LPCTSTR) szBulkSlotID);
		(*pElement)["SlotID4"] = lID;
		if (!IsValidSlotID(lID, lMinRange, lMaxRange))
		{
			return FALSE;
		}

		szBC = szBulkBarCode;
		(*pElement)["FrameBarCode4"] = szBC;

		szGrade = szBulkGrade;
		lGrade = atol((LPCTSTR) szGrade);
		(*pElement)["Slot4Grade"] = lGrade;

		szWIP = szBulkWIP;
		lWIP = atol((LPCTSTR) szWIP);
		(*pElement)["Slot4WIP"] = lWIP;

		szLog.Format("4 : Slot #%d, BC = %s, Grade = %s, WIP = %s", lID, szBC, szGrade, szWIP);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

		return TRUE;
	}
	if ((nBCIndex == -1) || (nGIndex == -1) || (nWIPIndex == -1))
	{
		return FALSE;
	}

	szID = szBulkSlotID.Left(nIndex);
	szBulkSlotID = szBulkSlotID.Mid(nIndex + 1);
	lID = atol((LPCTSTR) szID);
	(*pElement)["SlotID4"] = lID;
	if (!IsValidSlotID(lID, lMinRange, lMaxRange))
	{
		return FALSE;
	}

	szBC = szBulkBarCode.Left(nBCIndex);
	szBulkBarCode = szBulkBarCode.Mid(nBCIndex + 1);
	(*pElement)["FrameBarCode4"] = szBC;

	szGrade = szBulkGrade.Left(nGIndex);
	lGrade = atol((LPCTSTR) szGrade);
	szBulkGrade = szBulkGrade.Mid(nGIndex + 1);
	(*pElement)["Slot4Grade"] = lGrade;

	szWIP = szBulkWIP.Left(nWIPIndex);
	lWIP = atol((LPCTSTR) szWIP);
	szBulkWIP = szBulkWIP.Mid(nWIPIndex + 1);
	(*pElement)["Slot4WIP"] = lWIP;

	szLog.Format("4 : Slot #%d, BC = %s, Grade = %s, WIP = %s, szBulkWIP = %s", lID, szBC, szGrade, szWIP, szBulkWIP);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);


	//Slot #5 (Last one)
	nIndex = szBulkSlotID.Find("#");
	if ((nIndex != -1) || szBulkSlotID.GetLength() == 0)
	{
		//more than 5 data
		return FALSE;
	}
	if (szBulkBarCode.GetLength() == 0 || szBulkGrade.GetLength() == 0 || szBulkWIP.GetLength() == 0)
	{
		return FALSE;
	}

	lID = atol((LPCTSTR) szBulkSlotID);
	(*pElement)["SlotID5"] = lID;
	if (!IsValidSlotID(lID, lMinRange, lMaxRange))
	{
		return FALSE;
	}

	szBC = szBulkBarCode;
	(*pElement)["FrameBarCode5"] = szBC;

	szGrade = szBulkGrade;
	lGrade = atol((LPCTSTR) szGrade);
	(*pElement)["Slot5Grade"] = lGrade;

	szWIP = szBulkWIP;
	lWIP = atol((LPCTSTR) szWIP);
	(*pElement)["Slot5WIP"] = lWIP;

	szLog.Format("5 : Slot #%d, BC = %s, Grade = %s, WIP = %s", lID, szBC, szGrade, szWIP);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	return TRUE;
*/
}
