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


//m_lCurrMgzn  start from 1..
//m_lCurrSlot starts from 1 to 8.
CString CWaferLoader::GetSlotUsage(const LONG lSlotUsageType)
{
	switch (lSlotUsageType)
	{
	case WL_SLOT_USAGE_UNUSE:
		return CString(_T("unuse"));
	case WL_SLOT_USAGE_FULL:
		return CString(_T(" full "));
	case WL_SLOT_USAGE_EMPTY:
		return CString(_T("empty"));
	case WL_SLOT_USAGE_ACTIVE:
		return CString(_T("active"));
	case WL_SLOT_USAGE_ACTIVE2FULL:
		return CString(_T("act2F"));
	case WL_SLOT_USAGE_INUSE:
		return CString(_T("inuse"));
	case WL_SLOT_USAGE_SORTING:
		return CString(_T("Sort"));
	default:
		return CString(_T("unuse"));
	}
	return CString(_T("unuse"));
}

VOID CWaferLoader::GenerateOMRTTableFile(ULONG ulMgzn)
{
    CString str;
	CStdioFile fTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\wl_ominform.csv"), CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText))
	{
		return;
	}

	for(LONG j = 0; j < 13; j++)
	{
		//left Side
		// slot id
		str.Format("%d,", j + 1); 
		fTemp.WriteString(str);
		// usage
		str.Format("%s,", GetSlotUsage(m_stWaferMagazine[ulMgzn].m_lSlotUsage[j]));
		fTemp.WriteString(str);

		// bar code
		str.Format("%s,", m_stWaferMagazine[ulMgzn].m_SlotBCName[j]);
		fTemp.WriteString(str);
		
		// space for separator
		fTemp.WriteString(",");

		//Right Side
		fTemp.WriteString(",");
		fTemp.WriteString(",");
		fTemp.WriteString(",");
		fTemp.WriteString("\n");
    }

    fTemp.Close();
}


BOOL CWaferLoader::InitMSAutoLineWLModule()
{
	SetALBackGate(TRUE);
	Sleep(500);
	SetALFrontGate(FALSE);

	if (IsMagazineExist())
	{
		SetALMgznClamp(TRUE);
	}

	return TRUE;
}

// ==========================================================================================================
//			Related Host Command FRAME_LOAD, FRAME_LOAD_COMPLETE, 
//			FRAME_UNLOAD, FRAME_UNLOAD_COMPLETE function
// ==========================================================================================================
//================================================================
// Function Name: 		CheckHostLoadBarcodeID
// Input arguments:		None
// Output arguments:	None
// Description:   		Check whether the usage of current Slot meets the FRAME_LOAD'condition
//						it require the usage of the current slot must be empty
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::CheckHostLoadBarcodeID(const CString szSlotIDTitle, const CString szFrameBarCodeTitle, 
										const LONG lSlotID, const CString szBarcode, CString &szParaErrName)
{
	//Check whether this barcode is existed or not
	if (!szBarcode.IsEmpty())
	{
		if (lSlotID < 1 || lSlotID > MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT)
		{
			szParaErrName = szSlotIDTitle;
			return FALSE;
		}

		if (m_stWaferMagazine[0].m_lSlotUsage[lSlotID - 1] != WL_SLOT_USAGE_EMPTY)
		{
			szParaErrName = szSlotIDTitle; // + CString(" Not Empty Status");
			return FALSE;
		}

		if (!m_stWaferMagazine[0].m_SlotBCName[lSlotID - 1].IsEmpty())
		{
			szParaErrName = szFrameBarCodeTitle; // + CString(" Not Empty");
			return FALSE;
		}

		for (long j = 0; j < MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT; j++)
		{
			if (m_stWaferMagazine[0].m_lSlotUsage[j] != WL_SLOT_USAGE_EMPTY)
			{
				CString szSlotBarcode;
				szSlotBarcode = m_stWaferMagazine[0].m_SlotBCName[j];
				if ((j != lSlotID - 1) && (szSlotBarcode == szBarcode))
				{
					szParaErrName = szFrameBarCodeTitle; // + CString(" Have the Same Barcode");
					return FALSE;  //the Same Barcode, it return FALSE;
				}
			}
		}
	}
	return TRUE;
}

//================================================================
// Function Name: 		CheckHostLoadSlotUsageStatus
// Input arguments:		None
// Output arguments:	None
// Description:   		Check whether the usage of current Slot meets the FRAME_LOAD'condition
//						it require the usage of the current slot must be empty
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::CheckHostLoadSlotUsageStatus()
{
	CString szSlotIDTitle;
	CString szFrameBarCodeTitle;
	BOOL bAllDataValid = FALSE;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLineWL"];
	for (LONG lSlotNo = 1; lSlotNo <= 3; lSlotNo++)
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

		LONG lID		= (*pElement)[szSlotIDTitle];
		CString szBC	= (*pElement)[szFrameBarCodeTitle];
		
		if ((lID < 0) || (lID > MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT))
		{
			return FALSE;
		}

		if (lID > 0)
		{
			LONG nSlotIndex = lID - 1;
			//only check whether there has a barcode in load_frame command
			if (!szBC.IsEmpty() && (m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex] != WL_SLOT_USAGE_EMPTY))
			{
				return FALSE;
			}
			bAllDataValid = TRUE;
		}
	}
	return bAllDataValid;
}


//================================================================
// Function Name: 		CheckHostUnloadBarcodeID
// Input arguments:		None
// Output arguments:	None
// Description:   		Check whether the usage of current Slot meets the FRAME_UNLOAD'condition
//						it require the usage of the current slot must be empty
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::CheckHostUnloadBarcodeID(const BOOL bOnlyOneSlot, const BOOL bFirstSlot, 
											const CString szSlotIDTitle, const CString szFrameBarCodeTitle, 
											const LONG lSlotID, const CString szBarcode, CString &szParaErrName)
{
	if (lSlotID < 1 || lSlotID > MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT)
	{
		szParaErrName = szSlotIDTitle;
		return FALSE;
	}

	//Is empty and sorting, return
	if (szBarcode.IsEmpty() && (m_stWaferMagazine[0].m_lSlotUsage[lSlotID - 1] == WL_SLOT_USAGE_SORTING))
	{
		return TRUE;
	}

	if (!bOnlyOneSlot || bOnlyOneSlot && !szBarcode.IsEmpty())
	{
		if (m_stWaferMagazine[0].m_SlotBCName[lSlotID - 1] != szBarcode)
		{
			szParaErrName = szFrameBarCodeTitle; // + CString(" Not Empty");
			return FALSE;
		}

		if (m_stWaferMagazine[0].m_SlotBCName[lSlotID - 1].IsEmpty())
		{
			if ((m_stWaferMagazine[0].m_lSlotUsage[lSlotID - 1] == WL_SLOT_USAGE_FULL) ||
				(m_stWaferMagazine[0].m_lSlotUsage[lSlotID - 1] == WL_SLOT_USAGE_ACTIVE) ||
				(m_stWaferMagazine[0].m_lSlotUsage[lSlotID - 1] == WL_SLOT_USAGE_SORTING))
			{
				szParaErrName = szSlotIDTitle; // + CString(" Sorting Status");
				return FALSE;
			}
			return TRUE;
		}

		if (m_stWaferMagazine[0].m_lSlotUsage[lSlotID - 1] == WL_SLOT_USAGE_EMPTY)
		{
			szParaErrName = szSlotIDTitle; // + CString(" Empty Status");
			return FALSE;
		}

		if (m_stWaferMagazine[0].m_lSlotUsage[lSlotID - 1] == WL_SLOT_USAGE_SORTING)
		{
			szParaErrName = szSlotIDTitle; // + CString(" Sorting Status");
			return FALSE;
		}

		//Check whether this barcode is existed or not
		for (long j = MS_WL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1; j < MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT; j++)
		{
			if ((m_stWaferMagazine[0].m_lSlotUsage[j] != WL_SLOT_USAGE_EMPTY) &&
				(m_stWaferMagazine[0].m_lSlotUsage[j] != WL_SLOT_USAGE_SORTING))
			{
				CString szSlotBarcode;
				szSlotBarcode = m_stWaferMagazine[0].m_SlotBCName[j];
				if ((j != lSlotID - 1) && (szSlotBarcode == szBarcode))
				{
					szParaErrName = szFrameBarCodeTitle; // + CString(" Have the Same Barcode");
					return FALSE;  //the Same Barcode, it return TRUE;
				}
			}
		}
	}
	return TRUE;
}

//================================================================
// Function Name: 		IsHostUnloadSlot
// Input arguments:		None
// Output arguments:	None
// Description:   		if there has barcode in host FRAME_UNLOAD cmmand, the
//						usage of the current slot must be FULL or ACTIVE
//						if the barcode is empty in host FRAME_UNLOAD cmmand, the
//						usage of the current slot must be SORTING and 
//						mark the host will not drag this frame.
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::IsHostUnloadSlot(const LONG nSlotIndex, const CString szBC)
{
	if ((m_stWaferMagazine[0].m_SlotBCName[nSlotIndex] == szBC) &&
		((m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex] == WL_SLOT_USAGE_FULL) ||
		(m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex] == WL_SLOT_USAGE_UNUSE) ||
		(m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex] == WL_SLOT_USAGE_ACTIVE)))
	{
		return TRUE;
	}

	if (szBC.IsEmpty() &&
		(m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex] == WL_SLOT_USAGE_SORTING))
	{
		return TRUE;
	}

	if (szBC.IsEmpty() &&
		(m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex] == WL_SLOT_USAGE_EMPTY))
	{
		return TRUE;
	}

	CString szLog;
	szLog.Format("CWaferLoader IsHostUnloadSlot SlotID = %d, Usage = %d, BC = [%s], SlotBC = [%s]", nSlotIndex, m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex], szBC, m_stWaferMagazine[0].m_SlotBCName[nSlotIndex]);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	return FALSE;
}

//==================================================================================================
// Function Name: 		IsOnlyOneSlot
// Input arguments:		None
// Output arguments:	None
// Description:   		Unload only one slot or more than one slot(2~3)
// Return:				None
// Remarks:				None
//==================================================================================================
BOOL CWaferLoader::IsOnlyOneSlot()
{
	CString szSlotIDTitle, szFrameBarCodeTitle;
	BOOL bAllDataValid = FALSE;
	LONG lSlotCount = 0;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLineWL"];
	for (LONG lSlotNo = 1; lSlotNo <= 3; lSlotNo++)
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

		LONG lID		= (*pElement)[szSlotIDTitle];
		CString szBC	= (*pElement)[szFrameBarCodeTitle];
		
		if ((lID < 0) || (lID > MS_WL_AUTO_LINE_MGZN_SLOT))
		{
			return FALSE;
		}

		if (lID > 0)
		{
			//only check whether there has a barcode in unload_frame command
			if (!szBC.IsEmpty())
			{
				lSlotCount++;
			}
		}
	}
	return (lSlotCount == 1);
}



//================================================================
// Function Name: 		CheckHostUnloadSlotIDBarcode
// Input arguments:		None
// Output arguments:	None
// Description:   		Check whether the usage of current Slot meets the FRAME_UNLOAD'condition
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::CheckHostUnloadSlotIDBarcode()
{
	CString szSlotIDTitle, szFrameBarCodeTitle;
	BOOL bAllDataValid = FALSE;

	BOOL bOnlyOneSlot = IsOnlyOneSlot();

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLineWL"];
	for (LONG lSlotNo = 1; lSlotNo <= 3; lSlotNo++)
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

		LONG lID		= (*pElement)[szSlotIDTitle];
		CString szBC	= (*pElement)[szFrameBarCodeTitle];
		
		if ((lID < 0) || (lID > MS_WL_AUTO_LINE_MGZN_SLOT))
		{
			CString szLog;
			szLog.Format("lID = %d", lID);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}

		if (lID > 0)
		{
			if (bOnlyOneSlot)
			{
				//only check whether there has a barcode in unload_frame command
				if (!szBC.IsEmpty())
				{
					if (!IsHostUnloadSlot(lID - 1, szBC))
					{
						CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferLoader bOnlyOneSlot : IsHostUnloadSlot Failure");
						return FALSE;
					}
					bAllDataValid = TRUE;
				}
			}
			else
			{
				//only check whether there has a barcode in unload_frame command
				if (!IsHostUnloadSlot(lID - 1, szBC))
				{
					CMSLogFileUtility::Instance()->MS_LogSECSGEM("CWaferLoader More than one slot : IsHostUnloadSlot Failure");
					return FALSE;
				}
				bAllDataValid = TRUE;
			}
		}
	}
	return bAllDataValid;
}


//================================================================
// Function Name: 		SubUpdateBulkLoadSlots
// Input arguments:		None
// Output arguments:	None
// Description:   		Update the current slot inforamtion in cassette
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::SubUpdateBulkLoadSlots(BOOL bUnload, CString szBC, LONG lID, LONG lWIP)
{
	if ((lID <= 0) || (lID > 30))
	{
		return FALSE;
	}

	if (szBC.IsEmpty())
	{
		//Ignore this slot
		return TRUE;
	}

	INT nSlotIndex = lID - 1;
	if (bUnload)
	{
		if (m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex] != WL_SLOT_USAGE_SORTING)
		{
			m_stWaferMagazine[0].m_lSlotWIPCounter[nSlotIndex]	= 0;
			m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex]		= WL_SLOT_USAGE_EMPTY;
			m_stWaferMagazine[0].m_SlotBCName[nSlotIndex]		= _T("");
		}
	}
	else
	{
		m_stWaferMagazine[0].m_lSlotWIPCounter[nSlotIndex]	= lWIP;
		m_stWaferMagazine[0].m_lSlotUsage[nSlotIndex]		= WL_SLOT_USAGE_ACTIVE;
		m_stWaferMagazine[0].m_SlotBCName[nSlotIndex]		= szBC;
	}

	SECS_UpdateCassetteSlotInfo(lID);

	return TRUE;
}


//================================================================
// Function Name: 		UpdateBulkLoadSlots
// Input arguments:		None
// Output arguments:	None
// Description:   		Update the slot inforamtion in cassette according to 
//						FRAME_LOAD or FRAME_UNLOAD parameters
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::UpdateBulkLoadSlots(BOOL bUnload)
{
	CString szSlotIDTitle, szFrameBarCodeTitle, szSlotWIPTitle;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLineWL"];
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
		szSlotWIPTitle.Format("Slot%dWIP", lSlotNo);

		LONG lID		= (*pElement)[szSlotIDTitle];
		CString szBC	= (*pElement)[szFrameBarCodeTitle];
		LONG lWIP		= (*pElement)[szSlotWIPTitle];

		if (!SubUpdateBulkLoadSlots(bUnload, szBC, lID, lWIP))
		{
			return TRUE;
		}
	}

	return TRUE;
}


//================================================================
// Function Name: 		ResetBulkLoadData
// Input arguments:		None
// Output arguments:	None
// Description:   		Reset the buffer which save 
//						FRAME_LOAD or FRAME_UNLOAD parameters
// Return:				None
// Remarks:				None
//================================================================
BOOL CWaferLoader::ResetBulkLoadData()
{
	CString szSlotIDTitle, szFrameBarCodeTitle, szSlotGradeTitle, szSlotWIPTitle;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLineWL"];
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

	return TRUE;
}



// ==========================================================================================================
// ==========================================================================================================
//================================================================
// Function Name: 		GetEmptyWIPSlot_AutoLine
// Input arguments:		lCurrMgzn -- the current magazine number.
// Output arguments:	None
// Description:   		Called by GetLoadMgzSlot_AutoLine etc
// Return:				-1 -- No available slot
//						6~7 -- WIP Slot Number
// Remarks:				None
//================================================================
LONG CWaferLoader::GetEmptyWIPSlot_AutoLine(const LONG lCurrMgzn)
{
	//Slot #1 - #3 are INPUT (or EMPTY) slots
	//Slot #4 - #6 are Output slots
	//Slot #7 - #8 are WIP slots
	//Find a active slot in the input slot(slot#1~slot#3)
	for (LONG j = MS_WL_AUTO_LINE_WIP_START_MGZN_SLOT - 1; j < MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT; j++)
	{
		if ((m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[j] == WL_SLOT_USAGE_EMPTY) ||	//Have frame
			(m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[j] == WL_SLOT_USAGE_SORTING))
		{
			//If there is a frame at input slot, and grade is 0
			//then this is new EMPTY frame !!
			return j;
		}
	}

	return -1; //Not exist availabe slot
}

//================================================================
// Function Name: 		GetEmptyWIPSlot_AutoLine
// Input arguments:		lCurrMgzn -- the current magazine number.
// Output arguments:	None
// Description:   		Called by GetLoadMgzSlot_AutoLine etc
// Return:				-1 -- No available slot
//						6~7 -- WIP Slot Number
// Remarks:				None
//================================================================
LONG CWaferLoader::GetFullWIPSlot_AutoLine(const LONG lCurrMgzn, const LONG lCurrSlot)
{
	//Slot #1 - #3 are INPUT (or EMPTY) slots
	//Slot #4 - #6 are Output slots
	//Slot #7 - #8 are WIP slots
	//Find a active slot in the input slot(slot#1~slot#3)
	if (lCurrSlot >= MS_WL_AUTO_LINE_WIP_START_MGZN_SLOT - 1 && 
		lCurrSlot < MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT)
	{
		if (m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[lCurrSlot] == WL_SLOT_USAGE_FULL)	//Have frame
		{
			//If there is a frame at WIP slot
			//then this is new FULL frame !!
			return lCurrSlot;
		}
	}
	for (LONG j = MS_WL_AUTO_LINE_WIP_START_MGZN_SLOT - 1; j < MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT; j++)
	{
		if (m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[j] == WL_SLOT_USAGE_ACTIVE)	//Have frame
		{
			//If there is a frame at WIP slot
			//then this is new FULL frame !!
			return j;
		}
	}

	return -1; //Not exist availabe slot
}

BOOL CWaferLoader::IsInputSlot_AutoLine(const LONG lMgzn, const LONG lSlot)
{
	return ((lMgzn == MS_WL_AUTO_LINE_MGZN_NUM - 1) && 
			(lSlot >= MS_WL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1) && (lSlot < MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT));
}

//================================================================
// Function Name: 		GetAvailableInputSlot_AutoLine
// Input arguments:		lCurrMgzn -- the current magazine number.
// Output arguments:	None
// Description:   		Called by GetLoadMgzSlot_AutoLine etc
// Return:				-1 -- No available slot
//						0~3 -- Input Slot Number
// Remarks:				None
//================================================================
LONG CWaferLoader::GetAvailableInputSlot_AutoLine(const LONG lCurrMgzn)
{
	//Slot #1 - #3 are INPUT (or EMPTY) slots
	//Slot #4 - #6 are Output slots
	//Slot #7 - #8 are WIP slots
	//Find a active slot in the input slot(slot#1~slot#3)
	for (LONG j = MS_WL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1; j < MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT; j++)
	{
		if (m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[j] == WL_SLOT_USAGE_ACTIVE)	//Have frame
		{
			//If there is a frame at input slot, and grade is 0
			//then this is new EMPTY frame !!
			return j;
		}
	}

	return -1; //Not exist availabe slot
}

BOOL CWaferLoader::GetLoadMgzSlot_AutoLine(const BOOL bGetWIPMgzSlot_AutoLine)
{
	CString szMsg;
	LONG j = 0;
	
	if (!IsMSAutoLineMode())
	{
		return FALSE;
	}

	if (bGetWIPMgzSlot_AutoLine)
	{
		j = GetFullWIPSlot_AutoLine(MS_WL_AUTO_LINE_MGZN_NUM - 1, m_lCurrentSlotNo - 1);
	}
	else
	{
		j = GetAvailableInputSlot_AutoLine(MS_WL_AUTO_LINE_MGZN_NUM - 1);
	}

	if (j >= 0)
	{
		m_lCurrentSlotNo = j + 1;
		szMsg.Format("GetLoadMgzSlot_AutoLine(AUTOLINE) is %ld", j + 1);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		return TRUE; 
	}
	if (bGetWIPMgzSlot_AutoLine)
	{
		szMsg.Format("GetLoadMgzSlot_AutoLine(AUTOLINE) : Not Available Slot(#7~#8) in Wafer Cassette");
	}
	else
	{
		szMsg.Format("GetLoadMgzSlot_AutoLine(AUTOLINE) : Not Available Slot(#1~#3) in Wafer Cassette");
	}
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	return FALSE;
}

/*
LONG CWaferLoader::GetMgzNoOfInputSlots_AutoLine(const LONG lNoOfSlots)
{
	LONG lNoOfInputSlots = lNoOfSlots;
	if (IsMSAutoLineMode())
	{
		lNoOfInputSlots = MS_WL_AUTO_LINE_INPUT_END_MGZN_SLOT;
	}
	return lNoOfInputSlots;
}
*/

//================================================================
// Function Name: 		GetOutputWIPSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot with a given slot usage(#4~#6)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#4~#6)
// Remarks:				Called by  
//================================================================
BOOL CWaferLoader::GetOutputWIPSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot)
{
	LONG i = 0;

	for (LONG j = MS_WL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_WL_AUTO_LINE_WIP_END_MGZN_SLOT; j++)
	{
		//Find the original grade slot this frame was pulled out from; 
		if (m_stWaferMagazine[i].m_lSlotUsage[j] == lSlotUsage)
		{
			lMgzn = i + 1; //based from 1
			lSlot = j + 1;
			return TRUE;
		}
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetAvailableOutputSlot_AutoLine
// Input arguments:		lCurrMgzn -- the current magazine number.
// Output arguments:	None
// Description:   		Called by GetUnloadMgzSlot_AutoLine etc
// Return:				-1 -- No available slot
//						3~5 -- output Slot Number
// Remarks:				None
//================================================================
LONG CWaferLoader::GetAvailableOutputSlot_AutoLine(const LONG lCurrMgzn)
{
	//Slot #1 - #3 are INPUT (or EMPTY) slots
	//Slot #4 - #6 are Output slots
	//Slot #7 - #8 are WIP slots
	//Find a active slot in the output slot(slot#4~slot#6)
	for (LONG j = MS_WL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_WL_AUTO_LINE_OUTPUT_END_MGZN_SLOT; j++)
	{
		//Slot #4 - #6 are output slots, if full, it will prompt error
		if ((m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[j] == WL_SLOT_USAGE_EMPTY) ||
			(m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[j] == WL_SLOT_USAGE_SORTING))
		{
			//If there is a empty slot at output slot, it can unload frame.
			return j;
		}
	}

	return -1; //Not exist availabe slot
}

//================================================================
// Function Name: 		GetEmptyOutputSlot_AutoLine
// Input arguments:		lCurrMgzn -- the current magazine number.
// Output arguments:	None
// Description:   		Called by GetUnloadMgzSlot_AutoLine etc
// Return:				-1 -- No available slot
//						3~5 -- output Slot Number
// Remarks:				None
//================================================================
LONG CWaferLoader::GetEmptyOutputSlot_AutoLine(const LONG lCurrMgzn)
{
	//Slot #1 - #3 are INPUT (or EMPTY) slots
	//Slot #4 - #6 are Output slots
	//Slot #7 - #8 are WIP slots
	//Find a active slot in the output slot(slot#4~slot#6)
	for (LONG j = MS_WL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_WL_AUTO_LINE_MGZN_SLOT; j++)
	{
		//Slot #4 - #6 are output slots, if full, it will prompt error
		if (m_stWaferMagazine[lCurrMgzn].m_lSlotUsage[j] == WL_SLOT_USAGE_EMPTY)
		{
			//If there is a empty slot at output slot, it can unload frame.
			return j;
		}
	}

	return -1; //Not exist availabe slot
}

VOID CWaferLoader::TransferUnloadMgznSlot_AutoLine(const LONG lFromMagNo, const LONG lFromSlotNo,
												   const LONG lUnloadMagNo, const LONG lUnloadSlotNo, const BOOL bGetWIPMgzSlot_AutoLine)
{
	SetCassetteSlotStatus(lUnloadMagNo, lUnloadSlotNo,
						  0,
						  bGetWIPMgzSlot_AutoLine? WL_SLOT_USAGE_ACTIVE : WL_SLOT_USAGE_FULL,
						  m_stWaferMagazine[lFromMagNo].m_SlotBCName[lFromSlotNo]);

	if ((lFromMagNo != lUnloadMagNo) || (lFromSlotNo != lUnloadSlotNo))
	{
		SetCassetteSlotStatus(lFromMagNo, lFromSlotNo,
							  0,
							  WL_SLOT_USAGE_EMPTY,
							  _T(""));
	}
}


BOOL CWaferLoader::GetUnloadMgzSlot_AutoLine(const BOOL bGetWIPMgzSlot_AutoLine, LONG &lCurrentSlotNo)
{
	CString szMsg;
	LONG j = 0;
	
	if (!IsMSAutoLineMode())
	{
		return FALSE;
	}

	if (bGetWIPMgzSlot_AutoLine)
	{
		j = GetEmptyWIPSlot_AutoLine(MS_WL_AUTO_LINE_MGZN_NUM - 1);
	}
	else
	{
		j = GetAvailableOutputSlot_AutoLine(MS_WL_AUTO_LINE_MGZN_NUM - 1);
	}

	if (j >= 0)
	{
		lCurrentSlotNo = j + 1;
		szMsg.Format("GetUnloadMgzSlot_AutoLine(AUTOLINE) is %ld", lCurrentSlotNo);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		return TRUE; 
	}
	if (bGetWIPMgzSlot_AutoLine)
	{
		szMsg.Format("GetUnloadMgzSlot_AutoLine(AUTOLINE) : Not Available Slot(#7~#8) in Wafer Cassette");
	}
	else
	{
		szMsg.Format("GetUnloadMgzSlot_AutoLine(AUTOLINE) : Not Available Slot(#4~#6) in Wafer Cassette");
	}
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	return FALSE;
}

BOOL CWaferLoader::MoveToLoadMgzSlot_AutoLine(const BOOL bGetWIPMgzSlot_AutoLine)
{
	if (IsMSAutoLineMode())
	{
		long lOldSlotNo = m_lCurrentSlotNo;
		CString szMsg;

		m_lCurrentMagNo = MS_WL_AUTO_LINE_MGZN_NUM;

		szMsg.Format("Current Slot (AUTOLINE) is %ld, %d", m_lCurrentMagNo, m_lCurrentSlotNo);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		
		if (!GetLoadMgzSlot_AutoLine(bGetWIPMgzSlot_AutoLine))
		{
			CString szMsg;

			if (bGetWIPMgzSlot_AutoLine)
			{
				szMsg.Format("MoveToLoadMgzSlot_AutoLine : Not Available Input Slot(#7~#8) in Wafer Cassette");
			}
			else
			{
				szMsg.Format("MoveToLoadMgzSlot_AutoLine : Not Available Input Slot(#1~#3) in Wafer Cassette");
			}
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			return FALSE;
		}

		if (lOldSlotNo != m_lCurrentSlotNo)
		{
			szMsg.Format("MoveToMagazineSlot Start (AUTOLINE) is %ld, %d", m_lCurrentMagNo, m_lCurrentSlotNo);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);

			szMsg.Format("MoveToMagazineSlot Finish (AUTOLINE) is %ld, %d", m_lCurrentMagNo, m_lCurrentSlotNo);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

//			Z_Home();
//			MoveToMagazineSlot(m_lCurrentMagNo, m_lCurrentSlotNo);
		}
	}
	return TRUE;
}


//================================================================
// Function Name: 		TransferBondingMgznSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
//						lNewMgzn -- the new magazine number.
//						lnewSlot -- the new slot number.
// Description:   		Transfer the current Magazine Nume and Slot Number to
//						the new Magazine Nume and Slot Number
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CWaferLoader::TransferBondingMgznSlot_AutoLine(LONG lMgzn, LONG lSlot, LONG lNewMgzn, LONG lNewSlot)
{
	CString szMsg;

	szMsg.Format("TransferBondingMgznSlot_AutoLine(AUTOLINE) is %ld, %d, %d, %d", lMgzn, lSlot + 1, lNewMgzn, lNewSlot + 1);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	if ((lMgzn == MS_WL_AUTO_LINE_MGZN_NUM - 1) && (lNewMgzn == MS_WL_AUTO_LINE_MGZN_NUM - 1) &&
		(lSlot >= 0) && (lSlot < MS_WL_AUTO_LINE_MGZN_SLOT) &&
		(lNewSlot >= 0) && (lNewSlot < MS_WL_AUTO_LINE_MGZN_SLOT))
	{
		SetCassetteSlotStatus(lNewMgzn, lNewSlot,
							  GetCassetteSlotWIPCounter(lMgzn, lSlot),
							  WL_SLOT_USAGE_SORTING, //GetCassetteSlotUsage(lMgzn, lSlot),
							  GetCassetteSlotBCName(lMgzn, lSlot));
		if ((lNewMgzn != lMgzn) || (lNewSlot != lSlot))
		{
			//Set the usgae status of slot as EMPTY 
			SetCassetteSlotStatus(lMgzn, lSlot, 0, WL_SLOT_USAGE_EMPTY, "");
		}
		return TRUE;
	}
	return FALSE;
}

LONG CWaferLoader::GetCassetteSlotWIPCounter(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn == MS_WL_AUTO_LINE_MGZN_NUM - 1) &&
		(lSlot >= 0) && (lSlot < MS_WL_AUTO_LINE_MGZN_SLOT))
	{
		return m_stWaferMagazine[lMgzn].m_lSlotWIPCounter[lSlot];  		//Get WIP Counter from this slot 
	}
	return 0;
}

LONG CWaferLoader::GetCassetteSlotUsage(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn == MS_WL_AUTO_LINE_MGZN_NUM - 1) &&
		(lSlot >= 0) && (lSlot < MS_WL_AUTO_LINE_MGZN_SLOT))
	{
		return m_stWaferMagazine[lMgzn].m_lSlotUsage[lSlot];  		//Get Slot Usage from this slot 
	}
	return 0;
}

CString CWaferLoader::GetCassetteSlotBCName(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn == MS_WL_AUTO_LINE_MGZN_NUM - 1) &&
		(lSlot >= 0) && (lSlot < MS_WL_AUTO_LINE_MGZN_SLOT))
	{
		return m_stWaferMagazine[lMgzn].m_SlotBCName[lSlot];  		//Get Barcode Name from this slot 
	}
	return CString(_T(""));
}


VOID CWaferLoader::SetCassetteSlotStatus(const LONG lMgzn, const LONG lSlot,
										 const LONG lSlotWIPCounter, const LONG lSlotUsageStatus,
										 const CString szSlotBCName)
{
	m_stWaferMagazine[lMgzn].m_lSlotWIPCounter[lSlot] = lSlotWIPCounter;	//assign the counter of grade to this slot 
	m_stWaferMagazine[lMgzn].m_lSlotUsage[lSlot] = lSlotUsageStatus;
	m_stWaferMagazine[lMgzn].m_SlotBCName[lSlot] = szSlotBCName;
	SECS_UpdateCassetteSlotInfo(lSlot + 1);
	SaveData();
}
