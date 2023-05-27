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


//m_lCurrMgzn  is BL_MGZ_MID_1
//m_lCurrSlot starts from 0 to 24.
BOOL CBinLoader::InitMSAutoLineBLModule()
{
	SetALBackGate(FALSE);
	Sleep(500);
	SetALFrontGate(TRUE);

/*
	if (IsMagazineExist(1, FALSE, TRUE))
	{
		SetALMgznClamp(TRUE);
	}
*/
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
BOOL CBinLoader::CheckHostLoadBarcodeID(const CString szSlotIDTitle, const CString szFrameBarCodeTitle, 
										const LONG lSlotID, const CString szBarcode, CString &szParaErrName)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	if (!szBarcode.IsEmpty())
	{
		if (lSlotID < 1 || lSlotID > MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT)
		{
			szParaErrName = szSlotIDTitle;
			return FALSE;
		}

		if (m_stMgznRT[BL_MGZ_TOP_1].m_lSlotUsage[lSlotID - 1] != BL_SLOT_USAGE_EMPTY)
		{
			szParaErrName = szSlotIDTitle; // CString(" Not Empty Status");
			return FALSE;
		}

		if (!m_stMgznRT[BL_MGZ_TOP_1].m_SlotBCName[lSlotID - 1].IsEmpty())
		{
			szParaErrName = szFrameBarCodeTitle; //  CString(" Not Empty");
			return FALSE;
		}

		//Check whether this barcode is existed or not
		for (long i = 0; i < MS_BL_AUTO_LINE_MGZN_NUM; i++)
		{
			for (long j = 0; j < MS_BL_AUTO_LINE_OUTPUT_END_MGZN_SLOT; j++)
			{
				if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
				{
					CString szSlotBarcode;
					szSlotBarcode = m_stMgznRT[i].m_SlotBCName[j];
					if (((i != BL_MGZ_TOP_1) || (j != lSlotID - 1)) && (szSlotBarcode == szBarcode))
					{
						szParaErrName = szFrameBarCodeTitle; // + CString(" Have the Same Barcode");
						return FALSE;  //the Same Barcode, it return FALSE;
					}
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
BOOL CBinLoader::CheckHostLoadSlotUsageStatus()
{
	CString szSlotIDTitle;
	CString szSlotGradeTitle;
	CString szFrameBarCodeTitle;
	BOOL bAllDataValid = FALSE;

	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLine"];
	for (LONG lSlotNo = 1; lSlotNo <= 5; lSlotNo++)
	{
		szSlotIDTitle.Format("SlotID%d", lSlotNo);
		szSlotGradeTitle.Format("Slot%dGrade", lSlotNo);
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
		LONG lGrade		= (*pElement)[szSlotGradeTitle];
	
		if ((lID < 0) || (lID > MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT))
		{
			return FALSE;
		}

		if (lID > 0)
		{
			LONG lMgznNo = BL_MGZ_TOP_1;
			LONG nSlotIndex = lID - 1;
			//only check whether there has a barcode in load_frame command
			if (!szBC.IsEmpty() && (m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] != BL_SLOT_USAGE_EMPTY))
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
BOOL CBinLoader::CheckHostUnloadBarcodeID(const BOOL bOnlyOneSlot, const BOOL bFirstSlot, 
										  const CString szSlotIDTitle, const CString szSlotGradeTitle, 
										  const CString szFrameBarCodeTitle, const CString szFrameWIPCounterTitle, 
										  const LONG lSlotID, const LONG lGrade, const CString szBarcode, const long lWIPCounter,
										  CString &szParaErrName)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG lMgznNo = BL_MGZ_TOP_1;
	if (lSlotID < 1 || lSlotID > MS_BL_AUTO_LINE_OUTPUT_END_MGZN_SLOT)
	{
		szParaErrName = szSlotIDTitle;
		return FALSE;
	}

	//Is empty and sorting, return
	if (szBarcode.IsEmpty() && (m_stMgznRT[lMgznNo].m_lSlotUsage[lSlotID - 1] == BL_SLOT_USAGE_SORTING))
	{
		return TRUE;
	}

	if (!bOnlyOneSlot || bOnlyOneSlot && !szBarcode.IsEmpty())
	{
		if (m_stMgznRT[lMgznNo].m_SlotBCName[lSlotID - 1] != szBarcode)
		{
			szParaErrName = szFrameBarCodeTitle; //CString(" Not Matched");
			return FALSE;
		}


		if (m_stMgznRT[lMgznNo].m_SlotBCName[lSlotID - 1].IsEmpty())
		{
			if ((m_stMgznRT[lMgznNo].m_lSlotUsage[lSlotID - 1] == BL_SLOT_USAGE_FULL) ||
				(m_stMgznRT[lMgznNo].m_lSlotUsage[lSlotID - 1] == BL_SLOT_USAGE_ACTIVE) ||
				(m_stMgznRT[lMgznNo].m_lSlotUsage[lSlotID - 1] == BL_SLOT_USAGE_SORTING))
			{
				szParaErrName = szSlotIDTitle; // + CString(" Sorting Status");
				return FALSE;
			}
			return TRUE;
		}

		if (m_stMgznRT[lMgznNo].m_lSlotUsage[lSlotID - 1] == BL_SLOT_USAGE_EMPTY)
		{
			szParaErrName = szSlotIDTitle; //CString(" Empty Status");
			return FALSE;
		}

		if (m_stMgznRT[lMgznNo].m_lSlotUsage[lSlotID - 1] == BL_SLOT_USAGE_SORTING)
		{
			szParaErrName = szSlotIDTitle; //CString(" Sorting Status");
			return FALSE;
		}

		if (m_stMgznRT[lMgznNo].m_lSlotBlock[lSlotID - 1] != lGrade)
		{
			szParaErrName = szSlotGradeTitle; //CString(" Not Matched");
			return FALSE;
		}

		if (m_stMgznRT[lMgznNo].m_lSlotWIPCounter[lSlotID - 1] != lWIPCounter)
		{
			szParaErrName = szFrameWIPCounterTitle; //CString(" Not Matched");
			return FALSE;
		}

		//Check whether this barcode is existed or not
		for (long j = MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1; j < MS_BL_AUTO_LINE_OUTPUT_END_MGZN_SLOT; j++)
		{
			if ((m_stMgznRT[lMgznNo].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY) &&
				(m_stMgznRT[lMgznNo].m_lSlotUsage[j] != BL_SLOT_USAGE_SORTING))
			{
				CString szSlotBarcode;
				szSlotBarcode = m_stMgznRT[lMgznNo].m_SlotBCName[j];
				if ((j != lSlotID - 1) && (szSlotBarcode == szBarcode))
				{
					szParaErrName = szFrameBarCodeTitle; //CString(" Have the Same Barcode");
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
BOOL CBinLoader::IsHostUnloadSlot(const LONG nSlotIndex, const CString szBC)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG lMgznNo = BL_MGZ_TOP_1;
	if ((m_stMgznRT[lMgznNo].m_SlotBCName[nSlotIndex] == szBC) &&
		((m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] == BL_SLOT_USAGE_FULL) ||
		(m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] == BL_SLOT_USAGE_UNUSE) ||
		(m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] == BL_SLOT_USAGE_ACTIVE)))
	{
		return TRUE;
	}

	//Skip Sorting slot with empty barcode
	if (szBC.IsEmpty() &&
		(m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] == BL_SLOT_USAGE_SORTING))
	{
		return TRUE;
	}

	//Skip Empty slot with empty barcode
	if (szBC.IsEmpty() &&
		(m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] == BL_SLOT_USAGE_EMPTY))
	{
		return TRUE;
	}

	return FALSE;
}


//==================================================================================================
// Function Name: 		IsOnlyOneSlot
// Input arguments:		None
// Output arguments:	None
// Description:   		Unload only one slot or more than one slot(2~5)
// Return:				None
// Remarks:				None
//==================================================================================================
BOOL CBinLoader::IsOnlyOneSlot()
{
	CString szSlotIDTitle, szFrameBarCodeTitle;
	BOOL bAllDataValid = FALSE;
	LONG lSlotCount = 0;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLine"];

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

		LONG lID		= (*pElement)[szSlotIDTitle];
		CString szBC	= (*pElement)[szFrameBarCodeTitle];
		
		if ((lID < 0) || (lID > MS_BL_MGZN_SLOT))
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


//==================================================================================================
// Function Name: 		CheckHostUnloadSlotIDBarcode
// Input arguments:		None
// Output arguments:	None
// Description:   		Check whether the usage of current Slot meets the FRAME_UNLOAD'condition
// Return:				None
// Remarks:				None
//==================================================================================================
BOOL CBinLoader::CheckHostUnloadSlotIDBarcode()
{
	CString szSlotIDTitle, szFrameBarCodeTitle;
	BOOL bAllDataValid = FALSE;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLine"];

	BOOL bOnlyOneSlot = IsOnlyOneSlot();

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

		LONG lID		= (*pElement)[szSlotIDTitle];
		CString szBC	= (*pElement)[szFrameBarCodeTitle];
		
		if ((lID < 0) || (lID > MS_BL_MGZN_SLOT))
		{
			return FALSE;
		}

		if (lID > 0)
		{
			//only check whether there has a barcode in unload_frame command
			if (bOnlyOneSlot)
			{
				if (!szBC.IsEmpty())
				{
					if (!IsHostUnloadSlot(lID - 1, szBC))
					{
						BL_DEBUGBOX("bOnlyOneSlot : IsHostUnloadSlot Failure");
						return FALSE;
					}
					bAllDataValid = TRUE;
				}
			}
			else
			{
				if (!IsHostUnloadSlot(lID - 1, szBC))
				{
					BL_DEBUGBOX("more than one slot : IsHostUnloadSlot Failure");
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
BOOL CBinLoader::SubUpdateBulkLoadSlots(BOOL bUnload, CString szBC, LONG lID, LONG lGrade, LONG lWIP)
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

	if (lGrade < 0)		//v4.59A11	//WHen Bulk-Load, Grade = -1
	{
		lGrade = 0;
	}
	if (lWIP < 0)
	{
		lWIP = 0;
	}

	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG lMgznNo = BL_MGZ_TOP_1;
	//m_lSlotBlock ID is regarded as the same as lBinGrade for AutoLine, and ignore the mapping between Slot Block ID and Bin Grade.
	if (bUnload)
	{
		if (m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] != BL_SLOT_USAGE_SORTING)
		{
			m_stMgznRT[lMgznNo].m_lSlotBlock[nSlotIndex] = 0;
			m_stMgznRT[lMgznNo].m_lSlotWIPCounter[nSlotIndex] = 0;
			m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] = BL_SLOT_USAGE_EMPTY;
			m_stMgznRT[lMgznNo].m_lTransferSlotUsage[nSlotIndex] = BL_SLOT_USAGE_UNUSE;
			m_stMgznRT[lMgznNo].m_SlotBCName[nSlotIndex] = "";
			m_stMgznRT[lMgznNo].m_SlotSN[nSlotIndex].Empty();
			m_stMgznRT[lMgznNo].m_SlotLotNo[nSlotIndex].Empty();
		}
	}
	else
	{
		m_stMgznRT[lMgznNo].m_lSlotBlock[nSlotIndex] = lGrade;
		m_stMgznRT[lMgznNo].m_lSlotWIPCounter[nSlotIndex] = lWIP;
		m_stMgznRT[lMgznNo].m_lSlotUsage[nSlotIndex] = BL_SLOT_USAGE_ACTIVE;
		m_stMgznRT[lMgznNo].m_lTransferSlotUsage[nSlotIndex] = BL_SLOT_USAGE_UNUSE;
		m_stMgznRT[lMgznNo].m_SlotBCName[nSlotIndex] = szBC;
		m_stMgznRT[lMgznNo].m_SlotSN[nSlotIndex].Empty();
		m_stMgznRT[lMgznNo].m_SlotLotNo[nSlotIndex].Empty();
	}
	SECS_UpdateCassetteSlotInfo(lMgznNo, lID);

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
BOOL CBinLoader::UpdateBulkLoadSlots(BOOL bUnload)		//v4.59A11
{
	CString szSlotIDTitle, szFrameBarCodeTitle, szSlotGradeTitle, szSlotWIPTitle;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLine"];
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

		LONG lID		= (*pElement)[szSlotIDTitle];
		CString szBC	= (*pElement)[szFrameBarCodeTitle];
		LONG lGrade		= (*pElement)[szSlotGradeTitle];
		LONG lWIP		= (*pElement)[szSlotWIPTitle];

		if (!SubUpdateBulkLoadSlots(bUnload, szBC, lID, lGrade, lWIP))
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
BOOL CBinLoader::ResetBulkLoadData()
{
	CString szSlotIDTitle, szFrameBarCodeTitle, szSlotGradeTitle, szSlotWIPTitle;

	CMapElement *pElement = &(*m_psmfSRam)["MS"]["AutoLine"];
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
//			Related GetLoadMgzSlot_AutoLine function
// ==========================================================================================================
BOOL CBinLoader::IsInputSlot_AutoLine(const LONG lMgzn, const LONG lSlot)
{
	return ((lMgzn == BL_MGZ_TOP_1) && 
			(lSlot >= MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1) && (lSlot < MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT));
}

BOOL CBinLoader::IsOutputSlot_AutoLine(const LONG lMgzn, const LONG lSlot)
{
	return ((lMgzn == BL_MGZ_TOP_1) && 
			(lSlot >= MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1) && (lSlot < MS_BL_AUTO_LINE_OUTPUT_END_MGZN_SLOT));
}

//================================================================
// Function Name: 		GetGradeSlot_AutoLine
// Input arguments:		Block (Grade)
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Called by  GetLoadMgzSlot_AutoLine
// Return:				1 -- Magazine Number
//						0~24 -- Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetGradeSlot_AutoLine(const LONG lBlock, LONG &lMgzn, LONG &lSlot)
{
	LONG lMS_BL_MGZN_NUM = MS_BL_AUTO_LINE_MGZN_NUM;

	for (LONG i = BL_MGZ_MID_1; i < lMS_BL_MGZN_NUM; i++)
	{
		if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
		{
			for (LONG j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE)
				{
					//In AUTOLINE mode, lBlock = Grade, index (j) = slot number
					//In normal MS, lBlock = slot number = grade
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)	//Grade
					{
						lMgzn = i;
						lSlot = j;
						//if get bin frame from the input slot(slot#1~slot#5), and update the No of sorted die
						//SECS_UpdateInputSlotGradeWIPCounter()
						return TRUE;
					}	
				}
			}
		}
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetAvailableInputSlot_AutoLine
// Input arguments:		lCurrMgzn -- the current magazine number.
// Output arguments:	None
// Description:   		Called by GetLoadMgzSlot_AutoLine etc
// Return:				-1 -- No available slot
//						0~4 -- Input Slot Number
// Remarks:				None
//================================================================
LONG CBinLoader::GetAvailableInputSlot_AutoLine(const LONG lCurrMgzn)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	if ((m_stMgznRT[lCurrMgzn].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE) ||
		m_stMgznRT[lCurrMgzn].m_lMgznUsage == BL_MGZN_USAGE_FULL)
	{
		//Slot #1 - #15 are INPUT (or EMPTY) slots
		//Slot #16 - #25 are Output slots
		//Find a active slot in the input slot(slot#1~slot#15)
		for (LONG j = MS_BL_AUTO_LINE_INPUT_START_MGZN_SLOT - 1; j < MS_BL_AUTO_LINE_INPUT_END_MGZN_SLOT; j++)
		{
			if ((m_stMgznRT[lCurrMgzn].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE) &&	//Have frame
				(m_stMgznRT[lCurrMgzn].m_lSlotBlock[j] == 0))						//not yet assigned
			{
				//If there is a frame at input slot, and grade is 0
				//then this is new EMPTY frame !!
				return j;
			}
		}
	}

	return -1; //Not exist availabe slot
}

//================================================================
// Function Name: 		GetAvailableInputSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Called by GetNullMgznSlot_AutoLine
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
BOOL CBinLoader::GetAvailableInputSlot_AutoLine(LONG &lMgzn, LONG &lSlot)
{
	LONG i = BL_MGZ_TOP_1, j = 0;

	if ((j = GetAvailableInputSlot_AutoLine(i)) >= 0)
	{
		lMgzn = i;
		lSlot = j;
		return TRUE;
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetMgzSlot_AutoLine
// Input arguments:		None
// Output arguments:	lBlock -- the current block(grade).
//						bUseEmpty -- available input slot exist or not
// Description:   		Get the current Magazine Nume and Slot Number
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetMgzSlot_AutoLine(const LONG lBlock, LONG &lMgzn, LONG &lSlot)
{
	CString str;
	LONG i = 0, j = 0;
	BOOL bUseEmpty = TRUE;

	if (GetGradeSlot_AutoLine(lBlock, i, j))
	{
		bUseEmpty = FALSE;
	}

	if (bUseEmpty)
	{
		lMgzn = BL_MGZ_TOP_1;
		if ((j = GetAvailableInputSlot_AutoLine(m_lCurrMgzn)) < 0)
		{
			CString szTemp;
			szTemp.Format("GetAvailableInputSlot_AutoLine Failure (AUTOLINE - Mgzn #%d Slot #%d; bEmpty=%d, m_lOMRT = %d", m_lCurrMgzn, m_lCurrSlot, bUseEmpty, m_lOMRT);
			BL_DEBUGBOX(szTemp);
			return FALSE;
		}
		lSlot = j;
	}
	else
	{
		lMgzn = i;
		lSlot = j;
	}

	CString szTemp;
	szTemp.Format("GetMgzSlot_AutoLine (AUTOLINE - Mgzn #%d Slot #%d; bEmpty=%d, m_lOMRT = %d", lMgzn, lSlot, bUseEmpty, m_lOMRT);
	BL_DEBUGBOX(szTemp);

	return TRUE;
}
// ==========================================================================================================


//================================================================
// Function Name: 		GetLoadMgzSlot_AutoLine
// Input arguments:		None
// Output arguments:	lBlock -- the current block(grade).
//						bUseEmpty -- available input slot exist or not
// Description:   		Get the current Magazine Nume and Slot Number
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetLoadMgzSlot_AutoLine(const LONG lBlock, BOOL &bUseEmpty)
{
	CString str;
	LONG i = 0, j = 0;
	bUseEmpty = TRUE;

	if (GetGradeSlot_AutoLine(lBlock, i, j))
	{
		bUseEmpty = FALSE;
	}

	if (bUseEmpty)
	{
		m_lCurrMgzn = BL_MGZ_TOP_1;
		if ((j = GetAvailableInputSlot_AutoLine(m_lCurrMgzn)) < 0)
		{
			m_bAutoLineLoadDone = FALSE; //reset
			CString szTemp;
			szTemp.Format("GetAvailableInputSlot_AutoLine Failure (AUTOLINE - Mgzn #%d Slot #%d; bEmpty=%d, m_lOMRT = %d", m_lCurrMgzn, m_lCurrSlot, bUseEmpty, m_lOMRT);
			BL_DEBUGBOX(szTemp);
			return FALSE;
		}
		m_lCurrSlot = j;
	}
	else
	{
		m_lCurrMgzn = i;
		m_lCurrSlot = j;
	}

	CString szTemp;
	szTemp.Format("GetLoadMgzSlot (AUTOLINE - Mgzn #%d Slot #%d; bEmpty=%d, m_lOMRT = %d", m_lCurrMgzn, m_lCurrSlot, bUseEmpty, m_lOMRT);
	BL_DEBUGBOX(szTemp);

	//Update HMI variable
//	if (!m_bIsExChgArmExist && !m_bIsDualBufferExist)	
//	{
//		m_lCurrHmiSlot = m_lCurrSlot + 1;
//	}

	return TRUE;
}
// ==========================================================================================================


// ==========================================================================================================
//			Related GetUnloadMgznSlot_AutoLine function
// ==========================================================================================================
BOOL CBinLoader::IsWIPSlot_AutoLine(const LONG lMgzn, const LONG lSlot)
{
	return ((lMgzn >= BL_MGZ_MID_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
			(lSlot >= MS_BL_AUTO_LINE_WIP_START_MGZN_SLOT - 1) && (lSlot < MS_BL_MGZN_SLOT));
}

// ==========================================================================================================
//			Related GetUnloadMgznSlot_AutoLine function
// ==========================================================================================================
BOOL CBinLoader::IsWIPFullActiveSlot_AutoLine(const LONG lMgzn, const LONG lSlot)
{
	return ((lMgzn >= BL_MGZ_MID_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
			(lSlot >= MS_BL_AUTO_LINE_WIP_START_MGZN_SLOT - 1) && (lSlot < MS_BL_MGZN_SLOT) &&
			((m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] == BL_SLOT_USAGE_FULL) ||
			(m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] == BL_SLOT_USAGE_ACTIVE) ||
			(m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] == BL_SLOT_USAGE_SORTING)));
}


//================================================================
// Function Name: 		IsWIPActiveSlot_AutoLine
// Input arguments:		szBCName -- Barcode Name
//						lGradeBlock --Block
//						lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Output arguments:	None
// Description:   		Called by  GetNullMgznSlot_AutoLine
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						1~25 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::IsWIPActiveSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, const LONG lMgzn, const LONG lSlot)
{
	return ((lMgzn >= BL_MGZ_MID_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
			(lSlot >= MS_BL_AUTO_LINE_WIP_START_MGZN_SLOT - 1) && (lSlot < MS_BL_MGZN_SLOT) &&
			(m_stMgznRT[lMgzn].m_SlotBCName[lSlot] == szBCName) &&
			(m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] == lGradeBlock) &&
			(m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] == BL_SLOT_USAGE_ACTIVE));
}

//================================================================
// Function Name: 		GetWIPSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Called by  GetNullMgznSlot_AutoLine
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						1~25 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot)
{
	CString str;
	LONG lMS_BL_MGZN_NUM = MS_BL_AUTO_LINE_MGZN_NUM;

	for (LONG i = BL_MGZ_MID_1; i < lMS_BL_MGZN_NUM; i++) 
	{
		if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
		{
			str.Format("GetWIPEmptySlot_AutoLine (AutoLine): active mgzn found at1: #%d", i);
			BL_DEBUGBOX(str);

			//Slot #1 - #25 are WIP slots
			//Find a WIP slot with the given slot status
			for (LONG j = MS_BL_AUTO_LINE_WIP_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
			{
				//In AUTOLINE mode, lBlock = Grade, index (j) = slot number
				//In normal MS, lBlock = slot number = grade
				if (m_stMgznRT[i].m_lSlotUsage[j] == lSlotUsage)
				{
					str.Format("GetWIPSlotWithStatus_AutoLine(only slot usage): EMPTY NO-GRADE slot found at Slot #%d (0-based)", j);
					BL_DEBUGBOX(str);

					lMgzn = i;
					lSlot = j;
					return TRUE;

				}
			}
		}
	}
	return FALSE;
}

//================================================================
// Function Name: 		GetWIPSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Called by  GetNullMgznSlot_AutoLine
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						1~25 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPSlotWithStatus_AutoLine(const LONG lSlotUsage, const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	CString str;
	LONG lMS_BL_MGZN_NUM = MS_BL_AUTO_LINE_MGZN_NUM;

	for (LONG i = BL_MGZ_MID_1; i < lMS_BL_MGZN_NUM; i++) 
	{
		if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
		{
			str.Format("GetWIPEmptySlot_AutoLine (AutoLine): active mgzn found at2: #%d", i);
			BL_DEBUGBOX(str);

			//Slot #1 - #25 are WIP slots
			//Find a WIP slot with the given slot status
			for (LONG j = MS_BL_AUTO_LINE_WIP_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
			{
				//Find the original grade slot this frame was pulled out from; 
				//In AUTOLINE mode, lBlock = Grade, index (j) = slot number
				//In normal MS, lBlock = slot number = grade
				if ((m_stMgznRT[i].m_lSlotUsage[j] == lSlotUsage) &&
					(szBCName.IsEmpty() || szBCName == m_stMgznRT[i].m_SlotBCName[j]) &&
					(m_stMgznRT[i].m_lSlotBlock[j] == lGradeBlock))
				{
					str.Format("GetWIPSlotWithStatus_AutoLine: slot found at Mgzn=%ld, Slot #%d (0-based)", i, j);
					BL_DEBUGBOX(str);

					lMgzn = i;
					lSlot = j;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}



//================================================================
// Function Name: 		GetWIPFullSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the WIP Full Slot
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						10~24 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPFullSlot_AutoLine(LONG &lMgzn, LONG &lSlot)
{
	return GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_FULL, lMgzn, lSlot);
//	str.Format("GetWIPFullSlot_AutoLine (AutoLine): Full slot found at Slot #%d (0-based), grade = %d", j, m_stMgznRT[i].m_lSlotBlock[j]);
//	BL_DEBUGBOX(str);
}


//================================================================
// Function Name: 		GetWIPActiveSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the active WIP slot 
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						10~24 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPActiveSlot_AutoLine(LONG &lMgzn, LONG &lSlot)
{
	return GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_ACTIVE, lMgzn, lSlot);
}


BOOL CBinLoader::IsSlotGradeInSet_AutoLine(const LONG lGrade, const BOOL bASCIIGradeDisplayMode, CUIntArray &aulSelectdGradeList, CUIntArray &aulSelectdGradeLeftDieCountList)
{
	LONG lSelectedGradeNum = (LONG)aulSelectdGradeList.GetSize();
	for (LONG i = 0; i < lSelectedGradeNum; i++)
	{
		UCHAR ucTempGrade = aulSelectdGradeList.GetAt(i);
		LONG lLeftDieCount = aulSelectdGradeLeftDieCountList.GetAt(i);

		ucTempGrade -= 0x30;
		if ((lGrade == ucTempGrade) && (lLeftDieCount > 0))
		{
			return TRUE;
		}
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetWIPActiveSlotWithGradeList_AutoLine
// Input arguments:		aulSelectdGradeList, aulSelectdGradeLeftDieCountList
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Find a slot with ACTIVE status and it is not belong to aulSelectdGradeList
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						10~24 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPActiveSlotWithoutGradeList_AutoLine(const BOOL bASCIIGradeDisplayMode, CUIntArray &aulSelectdGradeList, CUIntArray &aulSelectdGradeLeftDieCountList, LONG &lMgzn, LONG &lSlot)
{
	LONG lMS_BL_MGZN_NUM = MS_BL_AUTO_LINE_MGZN_NUM;

	for (LONG i = BL_MGZ_MID_1; i < lMS_BL_MGZN_NUM; i++)
	{
		if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
		{
			//Slot #1 - #25 are WIP slots for grade frames
			for (LONG j = MS_BL_AUTO_LINE_WIP_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
			{
				if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE)
				{
					//In AUTOLINE mode, lBlock = Grade, index (j) = slot number
					//In normal MS, lBlock = slot number = grade
					if (!IsSlotGradeInSet_AutoLine(m_stMgznRT[i].m_lSlotBlock[j], bASCIIGradeDisplayMode, aulSelectdGradeList, aulSelectdGradeLeftDieCountList))
					{
						lMgzn = i;
						lSlot = j;
						return TRUE;
					}	
				}
			}
		}
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetWIPEmptySlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Called by  GetNullMgznSlot_AutoLine
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						1~25 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPEmptySlot_AutoLine(const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	return GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_EMPTY, "", lGradeBlock, lMgzn, lSlot);
}

//================================================================
// Function Name: 		IsWIPSortingSlotExisted_AutoLine
// Input arguments:		None
// Output arguments:	NULL
//						
// Description:   		Called by  GetNullMgznSlot_AutoLine
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						10~24 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::IsWIPSortingSlotExisted_AutoLine(const LONG lGradeBlock)
{
	LONG lMgzn = 0, lSlot = 0;

	return GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot);
}

//================================================================
// Function Name: 		GetWIPSortingSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Called by  GetNullMgznSlot_AutoLine
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						10~24 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPSortingSlot_AutoLine(const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	return GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot);
}


//================================================================
// Function Name: 		GetWIPEmptySortingSlotWithGrade_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Called by  GetNullMgznSlot_AutoLine
// Return:				-1 -- No available Magazine Number
//						1 -- Magazine Number
//						-1 -- No available slot
//						10~24 -- Input Slot Number
// Remarks:				None
//================================================================
BOOL CBinLoader::GetWIPEmptySortingSlotWithGrade_AutoLine(const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	if (GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lGradeBlock, lMgzn, lSlot))
	{
		return TRUE;
	}

	if (GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_ACTIVE, szBCName, lGradeBlock, lMgzn, lSlot))
	{
		return TRUE;
	}

	if (GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_ACTIVE2FULL, "", lGradeBlock, lMgzn, lSlot))
	{
		return TRUE;
	}
	
	return GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_EMPTY, "", lGradeBlock, lMgzn, lSlot);
}


//================================================================
// Function Name: 		GetUnloadMgznSlot_AutoLine
// Input arguments:		None
// Output arguments:	lBlock -- the current block(grade).
//						lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the current Magazine Nume and Slot Number for ulload
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetUnloadMgznSlot_AutoLine(const CString szBCName, const LONG lBlock, LONG &lMgzn, LONG &lSlot)
{
	CString str;
	LONG i = 0, j = 0;
	LONG lMS_BL_MGZN_NUM = MS_BL_AUTO_LINE_MGZN_NUM;

	if (!IsMSAutoLineMode())
	{
		return FALSE;
	}

	if (GetWIPEmptySortingSlotWithGrade_AutoLine(szBCName, lBlock, i, j))
	{
	}
	else
	{
		if (GetWIPEmptySlot_AutoLine(lBlock, i, j) || GetWIPSortingSlot_AutoLine(lBlock, i, j))
		{
		}
		else
		{
			//Because there has WIP empty slot and output has empty slot, 
			//so this frame will be put into the output slot
//			if (IsOutputSlot_AutoLine(lMgzn, lSlot))
//			{
//			}
//			else
			{
				SetAlert_Red_Yellow(IDS_BL_OPMODE_WRONG);
				str = "GetUnloadMgznSlot (AutoLine): no WIP can be found at UNLOAD";
				SetErrorMessage(str);
				BL_DEBUGBOX(str);
			}
			return FALSE;
		}
	}

	lMgzn = i;
	lSlot = j;

	CString szTemp;
	szTemp.Format("GetUnLoadMgzSlot (AUTOLINE) - Mgzn #%d, WIP Slot #%d", lMgzn, lSlot);
	BL_DEBUGBOX(szTemp);
	return TRUE;
}
// ==========================================================================================================

//================================================================
// Function Name: 		GetUnloadMgznSlotWithUpdate_AutoLine
// Input arguments:		None
// Output arguments:	lBlock -- the current block(grade).
//						lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the current Magazine Nume and Slot Number for ulload
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetUnloadMgznSlotWithUpdate_AutoLine(const CString szBCName, const LONG lBlock, const ULONG ulUnloadDieGradeCount, LONG &lMgzn, LONG &lSlot)
{
	CString str;
	LONG i = 0, j = 0;
	LONG lMS_BL_MGZN_NUM = MS_BL_AUTO_LINE_MGZN_NUM;

	if (!IsMSAutoLineMode())
	{
		return FALSE;
	}

	if (GetWIPEmptySortingSlotWithGrade_AutoLine(szBCName, lBlock, i, j))
	{
		str.Format("GetUnloadMgznSlot_AutoLine1 (Slot = %d), (Grade = %d), Unloaded Die Count = %d", j, lBlock, ulUnloadDieGradeCount);
		BL_DEBUGBOX(str);
		//Set the usage status of slot as ACTIVE in WIP slot Region(#11~#25)
		SetCassetteSlotStatus(i, j, lBlock,
							  ulUnloadDieGradeCount, BL_SLOT_USAGE_ACTIVE,
							  m_stMgznRT[lMgzn].m_SlotBCName[lSlot],
							  m_stMgznRT[lMgzn].m_SlotSN[lSlot],
							  m_stMgznRT[lMgzn].m_SlotLotNo[lSlot]);

		if ((i != lMgzn) || (j != lSlot))
		{
			//Set the usgae status of slot as EMPTY (origin slot from input slot region or WIP slot region)
			SetCassetteSlotStatus(lMgzn, lSlot, 0, 0, BL_SLOT_USAGE_EMPTY, "", "", "");
			m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;
		}

		str.Format("GetUnloadMgznSlot (AutoLine): assign old WIP slot(Mgzn %d Slot %d)", i, j);
		BL_DEBUGBOX(str);
	}
	else
	{
		str.Format("GetUnloadMgznSlot (AutoLine): assign new WIP slot from Mgzn %d Slot %d", lMgzn, lSlot);
		BL_DEBUGBOX(str);

		if (GetWIPEmptySlot_AutoLine(lBlock, i, j) || GetWIPSortingSlot_AutoLine(lBlock, i, j))
		{
			str.Format("GetUnloadMgznSlot_AutoLine2 (Slot = %d), (Grade = %d), Unloaded Die Count = %d", j, lBlock, ulUnloadDieGradeCount);
			BL_DEBUGBOX(str);
			//Set the usage status of slot as ACTIVE in WIP slot Region(#11~#25)
			SetCassetteSlotStatus(i, j, lBlock,
								ulUnloadDieGradeCount, BL_SLOT_USAGE_ACTIVE,
								m_stMgznRT[lMgzn].m_SlotBCName[lSlot],
								m_stMgznRT[lMgzn].m_SlotSN[lSlot],
								m_stMgznRT[lMgzn].m_SlotLotNo[lSlot]);

			str.Format("GetUnloadMgznSlot_AutoLine : Org Mgzn=%ld, Org Slot =%ld, Org BC = %s, Full Mgzn=%ld, Full Slot =%ld, FULL BC = %s, FULL Count = %d", lMgzn, lSlot, m_stMgznRT[lMgzn].m_SlotBCName[lSlot], i, j, m_stMgznRT[i].m_SlotBCName[j], ulUnloadDieGradeCount);
			BL_DEBUGBOX(str);

			if ((i != lMgzn) || (j != lSlot))
			{
				//Set the usgae status of slot as EMPTY (origin slot from input slot region or WIP slot region)
				SetCassetteSlotStatus(lMgzn, lSlot, 0, 0, BL_SLOT_USAGE_EMPTY, "", "", "");
				m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;
			}

			str.Format("GetUnloadMgznSlot (AutoLine): Reset original Mgzn #%d Slot #%d to EMPTY", lMgzn, lSlot);
			BL_DEBUGBOX(str);

			str.Format("GetUnloadMgznSlot (AutoLine): %d %d", i, j);
			BL_DEBUGBOX(str);
		}
		else
		{
			//Because there has WIP empty slot and output has empty slot, 
			//so this frame will be put into the output slot
			if (IsOutputSlot_AutoLine(lMgzn, lSlot))
			{
				str.Format("GetUnloadMgznSlot_AutoLine3 (Slot = %d), (Grade = %d), Unloaded Die Count = %d", lSlot, lBlock, ulUnloadDieGradeCount);
				BL_DEBUGBOX(str);
				//Only Update the counter.
				SetCassetteSlotStatus(lMgzn, lSlot, lBlock,
									ulUnloadDieGradeCount, BL_SLOT_USAGE_ACTIVE,
									m_stMgznRT[lMgzn].m_SlotBCName[lSlot],
									m_stMgznRT[lMgzn].m_SlotSN[lSlot],
									m_stMgznRT[lMgzn].m_SlotLotNo[lSlot]);

				//Create WIP output file.
				if (!CreateWIPOutputFileWithBarcode(lMgzn, lSlot))
				{
					return FALSE;
				}
			}
			else
			{
				SetAlert_Red_Yellow(IDS_BL_OPMODE_WRONG);
				str = "GetUnloadMgznSlot (AutoLine): no WIP can be found at UNLOAD";
				SetErrorMessage(str);
				BL_DEBUGBOX(str);
			}
			return FALSE;
		}
	}

	lMgzn = i;
	lSlot = j;

	CString szTemp;
	szTemp.Format("GetUnLoadMgzSlot (AUTOLINE) - Mgzn #%d, WIP Slot #%d", lMgzn, lSlot);
	BL_DEBUGBOX(szTemp);
	return TRUE;
}
// ==========================================================================================================


//================================================================
// Function Name: 		GetNullMgznSlot_AutoLine
// Input arguments:		None
// Output arguments:	lPhysicalBlock -- the current block(grade).
//						lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the current Magazine Nume and Slot Number
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetNullMgznSlot_AutoLine(const LONG lPhysicalBlock, LONG &lMgzn, LONG &lSlot)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	LONG i=0, j=0;
	CString szMsg;

	//4.55T09 search the bin matched blk
	szMsg.Format("Get empty frame curr Bin No(Slot Block): %d", lPhysicalBlock);
	BL_DEBUGBOX(szMsg);
	
	if (!GetAvailableInputSlot_AutoLine(i, j))
	{
		SetAlert_Red_Yellow(IDS_BL_MA_NOEPYFRAME_IN_EPYMAG);
		SetErrorMessage("BL error: No Empty frame in EMPTY magazine (AUTOLINE)");
		szMsg.Format("Get Null MgznSlot mode (AUTOLINE) is %ld MODE_A = 0", m_lOMRT);
		BL_DEBUGBOX(szMsg);
		return FALSE;
	}
	else
	{
		//Now, find a WIP & Output slot number for host before bonding
		if (!GetEmptyOutputWIPSlot_AutoLine("", lPhysicalBlock, i, j))
		{
			//How to handle for auto-line 2017.10.25 (WIP Full)
			m_bAutoLineUnloadDone = FALSE; //reset
			return Err_BinMagzineOutputWIPFull;
		}
	}

	//WIP slot is used
	lMgzn = i;
	lSlot = j;

	szMsg.Format("GetNullMgznSlot (AUTOLINE): WIP slot allocated to Mgzn #%ld Slot #%ld", lMgzn, lSlot);
	BL_DEBUGBOX(szMsg);
	return TRUE;
}


// ==========================================================================================================
//			Related GetFullMgznSlot_AutoLine function
// ==========================================================================================================
//================================================================
// Function Name: 		GetOutputSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot to WIP slot(#16~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#16~#25)
// Remarks:				Called by  
//================================================================
BOOL CBinLoader::GetOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG i = BL_MGZ_TOP_1;

	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)		//Now only TOP1 mgzn
	{
		//Slot #16 - #25 are output slots, if full, it will put it into WIP slots
		for (LONG j = MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
		{
			//Find the original grade slot this frame was pulled out from; 
			if (m_stMgznRT[i].m_lSlotUsage[j] == lSlotUsage)
			{
				CString szLog;
				szLog.Format("GetOutputSlotWithStatus_AutoLine : Empty Mgzn=%ld, Empty Slot =%ld", i, j);
				BL_DEBUGBOX(szLog);
				lMgzn = i;
				lSlot = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//================================================================
// Function Name: 		GetOutputSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot to WIP slot(#16~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#16~#25)
// Remarks:				Called by  
//================================================================
BOOL CBinLoader::GetOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, const CString szBCName, const LONG lSlotGrade, LONG &lMgzn, LONG &lSlot)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG i = BL_MGZ_TOP_1;
	CString szLog;
	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)		//Now only TOP1 mgzn
	{
		//Slot #16 - #25 are output slots, if full, it will put it into WIP slots
		for (LONG j = MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
		{
			//Find the original grade slot this frame was pulled out from; 
			if ((m_stMgznRT[i].m_lSlotUsage[j] == lSlotUsage) &&
				(szBCName.IsEmpty() || (m_stMgznRT[i].m_SlotBCName[j] == szBCName)) &&
				(m_stMgznRT[i].m_lSlotBlock[j] == lSlotGrade))
			{
				szLog.Format("GetOutputSlotWithStatus_AutoLine : Empty Mgzn=%ld, Empty Slot =%ld, Slot Usage,%d, BC,%s", i, j,lSlotUsage,szBCName);
				BL_DEBUGBOX(szLog);
				lMgzn = i;
				lSlot = j;
				return TRUE;
			}
		//	else
		//	{
		//		szLog.Format("Get Out Put Slot with status FALSE,Slot,%d,Usage,%d,Check Usage,%d,BC%s",j,m_stMgznRT[i].m_lSlotUsage[j],lSlotUsage,szBCName);
		//		BL_DEBUGBOX(szLog);
		//	}
		}
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetOutputSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot to WIP slot(#6~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#6~#25)
// Remarks:				Called by  
//================================================================
BOOL CBinLoader::GetOutputSlotWithStatusBarcode_AutoLine(const LONG lSlotUsage, const CString szBarcode, LONG &lMgzn, LONG &lSlot)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG i = BL_MGZ_TOP_1;

	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)		//Now only MID-1 mgzn
	{
		//Slot #16 - #25 are output slots, if full, it will put it into WIP slots
		for (LONG j = MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
		{
			//Find the original grade slot this frame was pulled out from; 
			if ((m_stMgznRT[i].m_lSlotUsage[j] == lSlotUsage) &&
				(m_stMgznRT[i].m_SlotBCName[j] == szBarcode))
			{
				CString szLog;
				szLog.Format("GetOutputSlotWithStatusBarcode_AutoLine : Empty Mgzn=%ld, Empty Slot =%ld", i, j);
				BL_DEBUGBOX(szLog);
				lMgzn = i;
				lSlot = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CBinLoader::IsOutputEmptySlot_AutoLine()
{
	LONG lMgzn = 0, lSlot = 0;
	return GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_EMPTY, "", 0, lMgzn, lSlot);
}


//================================================================
// Function Name: 		GetAvailableOutputSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the available Slot from the output slot to WIP slot(#6~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#6~#25)
// Remarks:				Called by  GetFullMgznSlot_AutoLine
//================================================================
BOOL CBinLoader::GetAvailableOutputSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	//find a slot with sorting status and grade
	if (GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lGradeBlock, lMgzn, lSlot))
	{
		return TRUE;
	}

	//find a slot with sorting status and barcode becasue after restart machine, there will lose the current magzine&slot information for the current BT frame
//	if (GetOutputSlotWithStatusBarcode_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lMgzn, lSlot))
//	{
//		return TRUE;
//	}

	//find a slot with empty status
	return GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_EMPTY, "", 0, lMgzn, lSlot);
}

//================================================================
// Function Name: 		GetEmptyOutputSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot to WIP slot(#6~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#6~#25)
// Remarks:				Called by  GetFullMgznSlot_AutoLine
//================================================================
BOOL CBinLoader::GetEmptyOutputSlot_AutoLine(const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	return GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_EMPTY, "", lGradeBlock, lMgzn, lSlot);
}

//================================================================
// Function Name: 		GetEmptyOutputWIPSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot to WIP slot(#6~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#6~#25)
// Remarks:				Called by  GetFullMgznSlot_AutoLine
//================================================================
BOOL CBinLoader::GetEmptyOutputWIPSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	//find a slot with sorting status
	if (GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot))
	{
		CString szBarcodeName = GetCassetteSlotBCName(lMgzn, lSlot);
		if (!szBCName.IsEmpty() && (szBarcodeName == szBCName))
		{
			//Already exist the same barcode name of sorting slot
			return FALSE;
		}
		return GetWIPEmptySlot_AutoLine(lGradeBlock, lMgzn, lSlot);
	}

	return GetEmptyOutputSlot_AutoLine(0, lMgzn, lSlot);
}


//================================================================
// Function Name: 		GetFullMgznSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the current Magazine Nume and Slot Number
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetFullMgznSlot_AutoLine(LONG &lMgzn, LONG &lSlot)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szMagSlot;
	LONG i = 0, j = 0;
	CString szMsg;

	LONG lGradeBlock = GetCassetteSlotGradeBlock(lMgzn, lSlot);
	CString szBCName = GetCassetteSlotBCName(lMgzn, lSlot);

	szMsg.Format("Get Full Mgzn Slot,GradeBlock,%d,BC,%s",lGradeBlock,szBCName);
	BL_DEBUGBOX(szMsg);
	if (szBCName.IsEmpty())
	{
		//Find a sorting slot again becasue there has machine hang up etc
		if (!GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot))
		{
			GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot);
		}
	}

	if (GetAvailableOutputSlot_AutoLine(szBCName, lGradeBlock, i, j))
	{
	}
	else
	{
		SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
		szMsg = "GetFullMgznSlot (AutoLine) no output slot found in magazine";
		SetErrorMessage(szMsg);
		BL_DEBUGBOX(szMsg);
		return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	szMagSlot.Format("%d/%d", lMgzn + 1, lSlot + 1);
	(*m_psmfSRam)["BinLoader"]["CurFullSlot"] = szMagSlot;

	return TRUE;
}

//================================================================
// Function Name: 		GetFullMgznSlotWithUpdate_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the current Magazine Nume and Slot Number
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetFullMgznSlotWithUpdate_AutoLine(const BOOL bClearAllFrameTOP2Mode, const LONG ulUnloadDieGradeCount, const BOOL bTransferWIPSlot, LONG &lMgzn, LONG &lSlot)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szMagSlot;
	LONG i = 0, j = 0;
	BOOL bRet = FALSE;
	CString szMsg;

	LONG lGradeBlock = GetCassetteSlotGradeBlock(lMgzn, lSlot);
	CString szBCName = GetCassetteSlotBCName(lMgzn, lSlot);
	if (szBCName.IsEmpty())
	{
		//Find a sorting slot again becasue there has machine hang up etc
		if (bClearAllFrameTOP2Mode)
		{
			bRet = GetClearFrameOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot);
		}
		else
		{
			bRet = GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot);
		}
		if (!bRet)
		{
			GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot);
		}
	}

	if (bClearAllFrameTOP2Mode)
	{
		bRet = GetAvailableClearFrameOutputSlot_AutoLine(szBCName, lGradeBlock, i, j);
	}
	else
	{
		bRet = GetAvailableOutputSlot_AutoLine(szBCName, lGradeBlock, i, j);
	}

	if (bRet)
	{
		ULONG ulDieGradeWIPCount = ulUnloadDieGradeCount;
		LONG lSlotUsageStatus   = BL_SLOT_USAGE_FULL;

		CSingleLock lock(&m_csBinSlotInfoMutex);
		lock.Lock();

		if ((bClearAllFrameTOP2Mode || bTransferWIPSlot) && IsWIPFullActiveSlot_AutoLine(lMgzn, lSlot))
		{
			ulDieGradeWIPCount = GetCassetteSlotWIPCounter(lMgzn, lSlot);
			lSlotUsageStatus = 	m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot];
				//GetCassetteSlotUsage(lMgzn, lSlot);
			//Send Event CEID#8024
			SendBinTransferEvent_8024(lMgzn, lSlot, i, j);
		}
		CString szLog;
		szLog.Format("GetFullMgznSlotWithUpdate_AutoLine (Slot = %d), (Grade = %d), Unloaded Die Count = %d", lSlot, GetCassetteSlotGradeBlock(lMgzn, lSlot), ulUnloadDieGradeCount);
		BL_DEBUGBOX(szLog);
		//Set the usage status of slot as FULL in Output Slot(#16~#25)
		SetCassetteSlotStatus(i, j, GetCassetteSlotGradeBlock(lMgzn, lSlot),
							  ulDieGradeWIPCount, lSlotUsageStatus,
							  GetCassetteSlotBCName(lMgzn, lSlot),
							  GetCassetteSlotSN(lMgzn, lSlot),
							  GetCassetteSlotLotNo(lMgzn, lSlot));
		szLog.Format("GetFullMgznSlotWithUpdate_AutoLine : Org Mgzn=%ld, Org Slot =%ld, Org BC = %s, Full Mgzn=%ld, Full Slot =%ld, FULL BC = %s, FULL Count = %d", lMgzn, lSlot, m_stMgznRT[lMgzn].m_SlotBCName[lSlot], i, j, m_stMgznRT[i].m_SlotBCName[j], ulDieGradeWIPCount);

		if ((i != lMgzn) || (j != lSlot))
		{
			//Set the usgae status of slot as EMPTY 
			SetCassetteSlotStatus(lMgzn, lSlot, 
								  (lMgzn == BL_MGZ_TOP_1) ? 0 : GetCassetteSlotGradeBlock(lMgzn, lSlot), //not clear grade
								  0, BL_SLOT_USAGE_EMPTY, "", "", "");
			m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;
		}

		szLog.Format("GetFullMgznSlotWithUpdate_AutoLine : Org Mgzn=%ld, Org Slot =%ld, Full Mgzn=%ld, Full Slot =%ld", lMgzn, lSlot, i, j);
		BL_DEBUGBOX(szLog);
	}
	else
	{
		SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
		szMsg = "GetFullMgznSlotWithUpdate_AutoLine (AutoLine) no output slot found in magazine";
		SetErrorMessage(szMsg);
		BL_DEBUGBOX(szMsg);
		return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	szMagSlot.Format("%d/%d", lMgzn + 1, lSlot + 1);
	(*m_psmfSRam)["BinLoader"]["CurFullSlot"] = szMagSlot;

	return TRUE;
}


VOID CBinLoader::UpdateBTCurrMgznSlot(const LONG lCurrMgzn, const LONG lCurrSlot)
{
	m_lCurrMgzn = lCurrMgzn;
	m_lCurrSlot = lCurrSlot;
	m_lCurrHmiSlot = lCurrSlot + 1;
}
//================================================================
// Function Name: 		TransferSortingMgznSlotFromWIPToOutput_AutoLine
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
BOOL CBinLoader::TransferSortingMgznSlotFromWIPToOutput_AutoLine()
{
	LONG lOutputMgzn = 0, lOutputSlot = 0;
	LONG lMgzn = 0, lSlot = 0;

	if (!GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, lMgzn, lSlot))
	{
		return FALSE;
	}

	LONG lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
	LONG lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
	if ((lCurrMgzn != lMgzn) || (lCurrSlot != lSlot))
	{
		return FALSE;
	}

	if (!GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_EMPTY, "", 0, lOutputMgzn, lOutputSlot))
	{
		return FALSE;
	}

	if ((lMgzn > BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT) &&
		(lOutputMgzn == BL_MGZ_TOP_1) &&
		(lOutputSlot >= MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT) && (lOutputSlot < MS_BL_MGZN_SLOT))
	{
		CSingleLock lock(&m_csBinSlotInfoMutex);
		lock.Lock();

		SetCassetteSlotStatus(lOutputMgzn, lOutputSlot,
							  GetCassetteSlotGradeBlock(lMgzn, lSlot),
							  GetCassetteSlotWIPCounter(lMgzn, lSlot),
							  GetCassetteSlotUsage(lMgzn, lSlot),
							  GetCassetteSlotBCName(lMgzn, lSlot),
							  GetCassetteSlotSN(lMgzn, lSlot),
							  GetCassetteSlotLotNo(lMgzn, lSlot));
		if ((lMgzn != lOutputMgzn) || (lSlot != lOutputSlot))
		{
			//Set the usgae status of slot as EMPTY 
			SetCassetteSlotStatus(lMgzn, lSlot, GetCassetteSlotGradeBlock(lMgzn, lSlot),
								  0, BL_SLOT_USAGE_EMPTY, "", "", "");
			m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;

			(*m_psmfSRam)["BinLoader"]["CurrMgzn"] = lOutputMgzn;
			(*m_psmfSRam)["BinLoader"]["CurrSlot"] = lOutputSlot;
			UpdateBTCurrMgznSlot(lOutputMgzn, lOutputSlot);
		}
		return TRUE;
	}
	return FALSE;
}

//================================================================
// Function Name: 		AdjustOutputSlotSortingPosn_AutoLine
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
BOOL CBinLoader::AdjustOutputSlotSortingPosn_AutoLine()
{
	LONG lMgzn = 0, lSlot = 0;
	LONG lOutputMgzn = 0, lOutputSlot = 0;

	if (!GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, lMgzn, lSlot))
	{
		return FALSE;
	}

	if (!GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_EMPTY, "", 0, lOutputMgzn, lOutputSlot))
	{
		return FALSE;
	}

	if ((lMgzn == BL_MGZ_TOP_1) &&
		(lSlot >= MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT) && (lSlot < MS_BL_MGZN_SLOT) &&
		(lOutputMgzn == BL_MGZ_TOP_1) &&
		(lOutputSlot >= MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT) && (lOutputSlot < MS_BL_MGZN_SLOT) &&
		(lSlot > lOutputSlot))
	{
		CSingleLock lock(&m_csBinSlotInfoMutex);
		lock.Lock();

		SetCassetteSlotStatus(lOutputMgzn, lOutputSlot,
							  GetCassetteSlotGradeBlock(lMgzn, lSlot),
							  GetCassetteSlotWIPCounter(lMgzn, lSlot),
							  GetCassetteSlotUsage(lMgzn, lSlot),
							  GetCassetteSlotBCName(lMgzn, lSlot),
							  GetCassetteSlotSN(lMgzn, lSlot),
							  GetCassetteSlotLotNo(lMgzn, lSlot));
		if (lSlot != lOutputSlot)
		{
			//Set the usgae status of slot as EMPTY 
			SetCassetteSlotStatus(lMgzn, lSlot, 0,
								  0, BL_SLOT_USAGE_EMPTY, "", "", "");
			m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;

			(*m_psmfSRam)["BinLoader"]["CurrMgzn"] = lOutputMgzn;
			(*m_psmfSRam)["BinLoader"]["CurrSlot"] = lOutputSlot;
			UpdateBTCurrMgznSlot(lOutputMgzn, lOutputSlot);
		}
		return TRUE;
	}
	return FALSE;
}


//================================================================
// Function Name: 		TransferMgznSlot_AutoLine
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
LONG CBinLoader::TransferMgznSlot_AutoLine(LONG lMgzn, LONG lSlot, LONG lNewMgzn, LONG lNewSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lNewMgzn >= BL_MGZ_TOP_1) && (lNewMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT) &&
		(lNewSlot >= 0) && (lNewSlot < MS_BL_MGZN_SLOT))
	{
		CSingleLock lock(&m_csBinSlotInfoMutex);
		lock.Lock();

		SetCassetteSlotStatus(lNewMgzn, lNewSlot,
							  GetCassetteSlotGradeBlock(lMgzn, lSlot),
							  GetCassetteSlotWIPCounter(lMgzn, lSlot),
							  GetCassetteSlotUsage(lMgzn, lSlot),
							  GetCassetteSlotBCName(lMgzn, lSlot),
							  GetCassetteSlotSN(lMgzn, lSlot),
							  GetCassetteSlotLotNo(lMgzn, lSlot), TRUE);
		if ((lMgzn != lNewMgzn) || (lSlot != lNewSlot))
		{
			//Set the usgae status of slot as EMPTY 
			SetCassetteSlotStatus(lMgzn, lSlot, 0, 0, BL_SLOT_USAGE_EMPTY, "", "", "", TRUE);
			m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;
		}
		return TRUE;
	}
	return FALSE;
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
LONG CBinLoader::TransferBondingMgznSlot_AutoLine(LONG lMgzn, LONG lSlot, LONG lNewMgzn, LONG lNewSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lNewMgzn >= BL_MGZ_TOP_1) && (lNewMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT) &&
		(lNewSlot >= 0) && (lNewSlot < MS_BL_MGZN_SLOT))
	{
		CSingleLock lock(&m_csBinSlotInfoMutex);
		lock.Lock();

		SetCassetteSlotStatus(lNewMgzn, lNewSlot,
							  GetCassetteSlotGradeBlock(lMgzn, lSlot),
							  GetCassetteSlotWIPCounter(lMgzn, lSlot),
							  BL_SLOT_USAGE_SORTING, //GetCassetteSlotUsage(lMgzn, lSlot),
							  GetCassetteSlotBCName(lMgzn, lSlot),
							  GetCassetteSlotSN(lMgzn, lSlot),
							  GetCassetteSlotLotNo(lMgzn, lSlot));
		if ((lMgzn != lNewMgzn) || (lSlot != lNewSlot))
		{
			//Set the usgae status of slot as EMPTY 
			SetCassetteSlotStatus(lMgzn, lSlot, 0, 0, BL_SLOT_USAGE_EMPTY, "", "", "");
			m_stMgznRT[lMgzn].m_lTransferSlotUsage[lSlot] = BL_SLOT_USAGE_UNUSE;
		}
		return TRUE;
	}
	return FALSE;
}


LONG CBinLoader::GetCassetteSlotGradeBlock(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT))
	{
		return m_stMgznRT[lMgzn].m_lSlotBlock[lSlot];  		//Get grade from this slot 
	}
	return 0;
}

LONG CBinLoader::GetCassetteSlotWIPCounter(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT))
	{
		return m_stMgznRT[lMgzn].m_lSlotWIPCounter[lSlot];  		//Get WIP Counter from this slot 
	}
	return 0;
}

LONG CBinLoader::GetCassetteSlotUsage(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT))
	{
		return m_stMgznRT[lMgzn].m_lSlotUsage[lSlot];  		//Get Slot Usage from this slot 
	}
	return 0;
}

CString CBinLoader::GetCassetteSlotBCName(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT))
	{
		return m_stMgznRT[lMgzn].m_SlotBCName[lSlot];  		//Get Barcode Name from this slot 
	}
	return CString(_T(""));
}

CString CBinLoader::GetCassetteSlotSN(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT))
	{
		return m_stMgznRT[lMgzn].m_SlotSN[lSlot];  		//Get Barcode Name from this slot 
	}
	return CString(_T(""));
}


CString CBinLoader::GetCassetteSlotLotNo(const LONG lMgzn, const LONG lSlot)
{
	if ((lMgzn >= BL_MGZ_TOP_1) && (lMgzn < MS_BL_AUTO_LINE_MGZN_NUM) &&
		(lSlot >= 0) && (lSlot < MS_BL_MGZN_SLOT))
	{
		return m_stMgznRT[lMgzn].m_SlotLotNo[lSlot];  		//Get Barcode Name from this slot 
	}
	return CString(_T(""));
}


VOID CBinLoader::SetCassetteSlotGradeBlock(const LONG lMgzn, const LONG lSlot, const LONG lGradeBlock)
{
	m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lGradeBlock;
	SaveData();
}

VOID CBinLoader::SetCassetteSlotUsage(const LONG lMgzn, const LONG lSlot, const LONG lSlotUsageStatus)
{
	m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = lSlotUsageStatus;
	SaveData();
}

VOID CBinLoader::SetCassetteSlotBCName(const LONG lMgzn, const LONG lSlot, const CString szSlotBCName)
{
	m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = szSlotBCName;
}

VOID CBinLoader::SetCassetteSlotStatus(const LONG lMgzn, const LONG lSlot, const LONG lSlotGradeBlock,
									   const LONG lSlotWIPCounter, const LONG lSlotUsageStatus,
									   const CString szSlotBCName,
									   const CString szSlotSN, const CString szSlotLotNo, const BOOL bCopy)
{
	if (bCopy || !IsWIPSlot_AutoLine(lMgzn, lSlot))
	{
		m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lSlotGradeBlock;  		//assign grade to this slot 
	}
	m_stMgznRT[lMgzn].m_lSlotWIPCounter[lSlot]	= lSlotWIPCounter;	//assign the counter of grade to this slot 
	m_stMgznRT[lMgzn].m_lSlotUsage[lSlot]		= lSlotUsageStatus;
	m_stMgznRT[lMgzn].m_SlotBCName[lSlot]		= szSlotBCName;
	m_stMgznRT[lMgzn].m_SlotSN[lSlot]			= szSlotSN;
	m_stMgznRT[lMgzn].m_SlotLotNo[lSlot]		= szSlotLotNo;
	SECS_UpdateCassetteSlotInfo(lMgzn, lSlot + 1);
	SaveData();
}


//================================================================
// Function Name: 		IsTransferFullWIPtoOutput
// Input arguments:		None
// Output arguments:	None
// Description:   		Check whether there has a WIP slot with FULL status and
//						a output slot with EMPTY status
// Return:				TRUE -- Can transfer a WIP slot with FYLL status to the output
//						FALSE -- Can not
// Remarks:				None
//================================================================
BOOL CBinLoader::IsTransferFullWIPtoOutput()
{
	LONG lMgzn = 0, lSlot = 0;

	if (GetWIPFullSlot_AutoLine(lMgzn, lSlot) && IsOutputEmptySlot_AutoLine())
	{
		return TRUE;
	}
	return FALSE;
}
				

//================================================================
// Function Name: 		CreateWIPOutputFileWithBarcode
// Input arguments:		None
// Output arguments:	None
// Description:   		Generate a WIP output file for SIS other machine in share folder
//						when this WIP frame was pulled into RGV
// Return:				TRUE -- OK, FALSE -- Failure
// Remarks:				None
//================================================================
LONG CBinLoader::CreateWIPOutputFileWithBarcode(const LONG lCurrMgzn, const LONG lCurrSlot)
{
	BOOL bGenWIPSlotOutputFile = FALSE;
	//Create a output file for this slot in SIS share folder
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable != NULL)
	{
		CString szTempFileName;
		CString szTargetFileName;
		CString szWIPSlotOutputFilePath;
		CString szBarcodeName = GetCassetteSlotBCName(lCurrMgzn, lCurrSlot);
		
		//Get WIP Temp File Name
		szTempFileName = pBinTable->GetSlotTempFileName(GetCassetteSlotGradeBlock(lCurrMgzn, lCurrSlot));

		//Get WIP output file name with barcode
		szWIPSlotOutputFilePath = pBinTable->GetSmartInlineShareFolder();
		if (szBarcodeName.IsEmpty())
		{
			//If barcode name is empty, use slot number as name
			CString szBlkIdForFilename;

			szBlkIdForFilename.Format("%d", lCurrSlot + 1);
			if (lCurrSlot + 1 < 10)
			{
				szBlkIdForFilename = "0" + szBlkIdForFilename;
			}

			szBarcodeName = "Blk" + szBlkIdForFilename + "TempFile";
		}
		szTargetFileName = szWIPSlotOutputFilePath + "\\" + szBarcodeName + ".csv";

		if ((_access(szTempFileName, 0) != -1) && (_access(szWIPSlotOutputFilePath, 0) != -1))
		{
			::CopyFile(szTempFileName, szTargetFileName, FALSE);
			if (_access(szTargetFileName, 0) != -1)
			{
				bGenWIPSlotOutputFile = TRUE;
				//CFile::Remove(szTempFileName);

				CString szLog;
				szLog.Format("CreateWIPOutputFileWithBarcode OK");
				BL_DEBUGBOX(szLog);
			}
		}

		//Clear the staistics counter of grade(block) 
		pBinTable->BT_ClearBinCounter((ULONG)GetCassetteSlotGradeBlock(lCurrMgzn, lCurrSlot));
	}

	if (!bGenWIPSlotOutputFile)
	{
		CString szMsg;
		szMsg.Format("Generate the WIP Slot output file failure");
		SetAlert_Msg_Red_Yellow(IDS_BL_CREATE_WIP_OUTPUT_FILE_FAIL, szMsg);
		SetErrorMessage("Generate the WIP Slot output file failure");
		return FALSE;
	}
	return TRUE;
}

//================================================================
// Function Name: 		CreateWIPTempFile
// Input arguments:		None
// Output arguments:	None
// Description:   		Generate a WIP temp file from SIS share folder
//						when this WIP frame was loaded from RGV
// Return:				TRUE -- OK, FALSE -- Failure
// Remarks:				None
//================================================================
LONG CBinLoader::CreateWIPTempFile(const LONG lCurrMgzn, const LONG lCurrSlot)
{
	BOOL bGenWIPSlotTempFile = FALSE;
	//Create a output file for this slot in SIS share folder
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable != NULL)
	{
		CString szTempFileName;
		CString szTargetFileName;
		CString szWIPSlotOutputFilePath;
		CString szBarcodeName = GetCassetteSlotBCName(lCurrMgzn, lCurrSlot);

		//Get WIP temp file name
		szTempFileName = pBinTable->GetSlotTempFileName(GetCassetteSlotGradeBlock(lCurrMgzn, lCurrSlot));

		//Get WIP output file name with barcode
		szWIPSlotOutputFilePath = pBinTable->GetSmartInlineShareFolder();
		szTargetFileName = szWIPSlotOutputFilePath + "\\" + szBarcodeName + ".csv";

		if (_access(szTargetFileName, 0) != -1)
		{
			::CopyFile(szTargetFileName, szTempFileName, FALSE);
			if (_access(szTempFileName, 0) != -1)
			{
				bGenWIPSlotTempFile = TRUE;
			}
		}
	}

	if (!bGenWIPSlotTempFile)
	{
		CString szMsg;
		szMsg.Format("Generate the WIP Slot temporary file failure");
		SetAlert_Msg_Red_Yellow(IDS_BL_CREATE_WIP_TMP_FILE_FAIL, szMsg);
		SetErrorMessage("Generate the WIP Slot temporary file failure");
		return FALSE;
	}
	return TRUE;
}


//================================================================
// Function Name: 		GetNoOfSortedDie
// Input arguments:		lBlkId
// Output arguments:	None
// Description:   		Get the total counter of the sorted die
// Return:				Total sorted die counter
// Remarks:				None
//================================================================
LONG CBinLoader::GetNoOfSortedDie(const UCHAR lBlkId)
{
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable != NULL)
	{
		return pBinTable->GetNoOfSortedDie(lBlkId);
	}
	return 0;
}


//================================================================
// Function Name: 		GetCustomOutputFileName
// Input arguments:		lBlkId
// Output arguments:	None
// Description:   		Get the output file name
// Return:				Output file name
// Remarks:				None
//================================================================
CString CBinLoader::GetCustomOutputFileName()
{
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable != NULL)
	{
		return pBinTable->GetCustomOutputFileName();
	}
	return CString(_T(""));
}

//Check if there is empty slot to unload when load frame
BOOL CBinLoader::HaveEmptyOutputSlot_AutoLine()
{
	LONG i = BL_MGZ_TOP_1;

	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)		//Now only MID-1 mgzn
	{
		//Slot #15- #25 check empty slot if empty return true; else return false;
		for (LONG j = MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
		{
			if ((m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_EMPTY) && (m_stMgznRT[i].m_lSlotBlock[j] == 0))
			{
				CString szLog;
				szLog.Format("1Empty slot has found : Empty Mgzn=%ld, Empty Slot =%ld", i, j);
				BL_DEBUGBOX(szLog);
				return TRUE;
			}
		}
	}
	return FALSE;
}


//Check if there is Full slot to unload when load frame
BOOL CBinLoader::IsFullOutputSlot_AutoLine()
{
	LONG i = BL_MGZ_TOP_1;

	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)		//Now only MID-1 mgzn
	{
		//Slot #15- #25 check empty slot if empty return true; else return false;
		for (LONG j = MS_BL_AUTO_LINE_OUTPUT_START_MGZN_SLOT - 1; j < MS_BL_MGZN_SLOT; j++)
		{
			if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
			{
				CString szLog;
				szLog.Format("2Empty slot has found : Empty Mgzn=%ld, Empty Slot =%ld", i, j);
				BL_DEBUGBOX(szLog);
				return FALSE;
			}
		}
	}
	return TRUE;
}

//========================================================================================================================
//					TOP2 clear all Frame Magazine handling function
//=======================================================================================================================
BOOL CBinLoader::IsFullSlotInMgzn(const LONG lMgzn)
{
	for (LONG j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		if (m_stMgznRT[lMgzn].m_lSlotUsage[j] == BL_SLOT_USAGE_FULL)
		{
			return TRUE;
		}
	}

	return FALSE;
}
//================================================================
// Function Name: 		IsClearFrameOutputFullMgazSlot_AutoLine
// Input arguments:		lBlkId
// Output arguments:	None
// Description:   		check whether there has all full frame in the TOP magazine
// Return:				the empty slot number if it is not full magazine
// Remarks:				None
//================================================================
BOOL CBinLoader::IsClearFrameOutputFullMgazSlot_AutoLine(LONG &lMgzn, LONG &lSlot)
{
	LONG i = BL_MGZ_TOP_2;

	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_UNUSE)		//Now only Top2 mgzn
	{
		//Slot #15- #25 check empty slot if empty return true; else return false;
		for (LONG j = 0; j < MS_BL_MGZN_SLOT; j++)
		{
			if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
			{
				lMgzn = i;
				lSlot = j;
				CString szLog;
				szLog.Format("3Empty slot has found : Empty Mgzn=%ld, Empty Slot =%ld", i, j);
				BL_DEBUGBOX(szLog);
				return FALSE;
			}
		}
	}
	return TRUE;
}

//================================================================
// Function Name: 		GetClearFrameOutputSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot to WIP slot(#1~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#1~#25)
// Remarks:				Called by  
//================================================================
BOOL CBinLoader::GetClearFrameOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, LONG &lMgzn, LONG &lSlot)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG i = BL_MGZ_TOP_2;

	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_UNUSE)		//Now only TOP2 mgzn
	{
		//Slot #1 - #25 are output slots, if full, it will put it into WIP slots
		for (LONG j = 0; j < MS_BL_MGZN_SLOT; j++)
		{
			//Find the original grade slot this frame was pulled out from; 
			if (m_stMgznRT[i].m_lSlotUsage[j] == lSlotUsage)
			{
				CString szLog;
				szLog.Format("GetClearFrameOutputSlotWithStatus_AutoLine1 : Empty Mgzn=%ld, Empty Slot =%ld", i, j);
				BL_DEBUGBOX(szLog);
				lMgzn = i;
				lSlot = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//================================================================
// Function Name: 		GetClearFrameOutputSlotWithStatus_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the Empty Slot from the output slot to WIP slot(#1~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#1~#25)
// Remarks:				Called by  
//================================================================
BOOL CBinLoader::GetClearFrameOutputSlotWithStatus_AutoLine(const LONG lSlotUsage, const CString szBCName, const LONG lSlotGrade, LONG &lMgzn, LONG &lSlot)
{
	CSingleLock lock(&m_csBinSlotInfoMutex);
	lock.Lock();

	LONG i = BL_MGZ_TOP_2;

	if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_UNUSE)		//Now only TOP 2 mgzn
	{
		//Slot #1 - #25 are output slots, if full, it will put it into WIP slots
		for (LONG j = 0; j < MS_BL_MGZN_SLOT; j++)
		{
			//Find the original grade slot this frame was pulled out from; 
			if ((m_stMgznRT[i].m_lSlotUsage[j] == lSlotUsage) &&
				(szBCName.IsEmpty() || (m_stMgznRT[i].m_SlotBCName[j] == szBCName)) &&
				(m_stMgznRT[i].m_lSlotBlock[j] == lSlotGrade))
			{
				CString szLog;
				szLog.Format("GetClearFrameOutputSlotWithStatus_AutoLine : Empty Mgzn=%ld, Empty Slot =%ld", i, j);
				BL_DEBUGBOX(szLog);
				lMgzn = i;
				lSlot = j;
				return TRUE;
			}
		}
	}
	return FALSE;
}


//================================================================
// Function Name: 		GetAvailableClearFrameOutputSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the available Slot from the output slot to WIP slot(#1~#25)
// Return:				lMgzn - available Magazine Number
//						lSlot -- Slot Number(#1~#25)
// Remarks:				Called by  GetFullMgznSlot_AutoLine
//================================================================
BOOL CBinLoader::GetAvailableClearFrameOutputSlot_AutoLine(const CString szBCName, const LONG lGradeBlock, LONG &lMgzn, LONG &lSlot)
{
	//find a slot with sorting status and grade
	if (GetClearFrameOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lGradeBlock, lMgzn, lSlot))
	{
		return TRUE;
	}

	//find a slot with sorting status and barcode becasue after restart machine, there will lose the current magzine&slot information for the current BT frame
//	if (GetOutputSlotWithStatusBarcode_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lMgzn, lSlot))
//	{
//		return TRUE;
//	}

	//find a slot with empty status
	return GetClearFrameOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_UNUSE, "", 0, lMgzn, lSlot);
}


//================================================================
// Function Name: 		GetClearFrameFullMgznSlot_AutoLine
// Input arguments:		None
// Output arguments:	lMgzn -- the current magazine number.
//						lSlot -- the current slot number.
// Description:   		Get the current Magazine Nume and Slot Number
// Return:				TRUE -- Has Available Input Slot
//						FALSE -- No Availbale Input Slot
// Remarks:				None
//================================================================
LONG CBinLoader::GetClearFrameFullMgznSlot_AutoLine(LONG &lMgzn, LONG &lSlot)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szMagSlot;
	LONG i = 0, j = 0;
	CString szMsg;

	LONG lGradeBlock = GetCassetteSlotGradeBlock(lMgzn, lSlot);
	CString szBCName = GetCassetteSlotBCName(lMgzn, lSlot);
/*
	if (szBCName.IsEmpty())
	{
		//Find a sorting slot again becasue there has machine hang up etc
		if (!GetClearFrameOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot))
		{
			GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, "", lGradeBlock, lMgzn, lSlot);
		}
	}
*/
	if (GetAvailableClearFrameOutputSlot_AutoLine(szBCName, lGradeBlock, i, j))
	{
	}
	else
	{
		SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
		szMsg = "GetClearFrameFullMgznSlot_AutoLine (AutoLine) no output slot found in magazine";
		SetErrorMessage(szMsg);
		BL_DEBUGBOX(szMsg);
		return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	szMagSlot.Format("%d/%d", lMgzn + 1, lSlot + 1);
	(*m_psmfSRam)["BinLoader"]["CurFullSlot"] = szMagSlot;

	return TRUE;
}
