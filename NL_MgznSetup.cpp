/////////////////////////////////////////////////////////////////
// NL_MgznSetup.cpp : interface of the CNVCLoader class
//
//	Description:
//		
//	Date:		07 Sept 2020
//	Revision:	1.00
//
//	By:	Andrew Ng		
//				
//	Copyright @ ASM Pacific Technology Ltd., .
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NVCLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CNVCLoader::ResetAllMagazine()
{
	m_lCurrBlock1		= 0;
	m_lCurrBlock2		= 0;

	m_lCurrBTBlock		= 0;
	m_lCurrBTMgzn		= 0;
	m_lCurrBTSlotID		= 0;
	
	m_lCurrWTBlock		= 0;
	m_lCurrWafSlotID	= 1;

	m_szFrameBarcode1	= "";
	m_szFrameBarcode2	= "";

	m_lCurrMgzn1		= 0;
	m_lCurrMgzn2		= 0;
	m_lCurrSlotID1		= 0;
	m_lCurrSlotID2		= 0;

	m_lCurrHmiMgzn1		= 0;
	m_lCurrHmiMgzn2		= 0;
	m_lCurrHmiSlot1		= 0;
	m_lCurrHmiSlot2		= 0;
	//m_szCurrMgznDisplayName = GetMagazineName(m_lCurrMgzn).MakeUpper();

	for (INT i = 0; i < MS_BL_MGZN_NUM; i++)
	{
		ResetMagazine(i, TRUE);
	}

	//Reset SRAM such that will trigger Grade-Change in AUTOBOND
	(*m_psmfSRam)["BinTable"]["LastBlkInUse"] = 0;
	(*m_psmfSRam)["BinTable"]["BlkInUse"] = 0;
	
	SaveData();

	SetStatusMessage("NL: Reset All Magazine Complete");
}

LONG CNVCLoader::ResetMagazine(ULONG lMgzn, BOOL bIsManual, BOOL bClearBin)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	
	CMS896AStn::m_bRealignFrameDone  = TRUE;
	CMS896AStn::m_bRealignFrame2Done = TRUE;


	CString szMsg;
	szMsg.Format("Reset Magazine %d", lMgzn);
	//BL_DEBUGBOX(szMsg);

    if (lMgzn >= MS_BL_MGZN_NUM)
	{
        return FALSE;
	}
	
	m_lOMSP = NL_MODE_A;
	m_lOMRT	= NL_MODE_A;


    LoadMgznOMData();

    m_stNVCMgznRT[lMgzn].m_lMgznState = NL_MGZN_STATE_OK;
    m_stNVCMgznRT[lMgzn].m_lMgznUsage = m_stMgznOM[lMgzn].m_lMgznUsage;

	BOOL bResetEmpty = FALSE;
	BOOL bMagResetEmpty = pApp->GetFeatureStatus(MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY);
	
	if ((bMagResetEmpty) && (m_stNVCMgznRT[lMgzn].m_lMgznUsage == NL_MGZN_USAGE_ACTIVE))
	{
		bResetEmpty = TRUE;
	}

    for (ULONG i = 0; i < MS_BL_MAX_MGZN_SLOT; i++)
    {
		ULONG ulBlock	= m_stNVCMgznRT[lMgzn].m_lSlotBlock[i];
		LONG lSlotUsage = m_stNVCMgznRT[lMgzn].m_lSlotUsage[i];
		BOOL bClearBinStatus = TRUE;

		BOOL bIsMoreThanMinCount = TRUE;
		if (bIsManual)
		{
			bIsMoreThanMinCount = FALSE;
			//if (GetBinBlkBondedCount((ULONG)ulBlock) >= m_ulMinClearAllCount)
			//{
			//	bMinClearAllCount = TRUE;
			//}
		}

		if (bIsMoreThanMinCount && bClearBin && (ulBlock != 0))
		{
			CString szLog;
			szLog.Format("NL: ResetMagazine (ClearBin #%ld): BC = %s ", ulBlock, m_stNVCMgznRT[lMgzn].m_SlotBCName[i]);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			//SaveBarcodeData(ulBlock, m_stNVCMgznRT[lMgzn].m_SlotBCName[i], lMgzn, i);
			//bClearBinStatus = ClearBinFrameCounter(ulBlock, m_stNVCMgznRT[lMgzn].m_SlotBCName[i], FALSE);		
		}

		if (!bClearBin ||												//If don't clear bin, just clear slot status, or
			(bClearBin && bIsMoreThanMinCount && bClearBinStatus))		//If need ClearBin, check if AboveMinCount + ClearOK
		{
			m_stNVCMgznRT[lMgzn].m_lSlotBlock[i] = m_stMgznOM[lMgzn].m_lSlotBlock[i];
			m_stNVCMgznRT[lMgzn].m_lSlotUsage[i] = m_stMgznOM[lMgzn].m_lSlotUsage[i];
			m_stNVCMgznRT[lMgzn].m_SlotBCName[i].Empty();
			m_stNVCMgznRT[lMgzn].m_SlotSN[i].Empty();
			m_stNVCMgznRT[lMgzn].m_SlotLotNo[i].Empty();
			m_stNVCMgznRT[lMgzn].m_lSlotWIPCounter[i] = 0;

			//set each grade slot to "no frame" and require sequence to retrieve from EMPTY magazine
			if (bResetEmpty)
			{
				m_stNVCMgznRT[lMgzn].m_lSlotUsage[i] = NL_SLOT_USAGE_ACTIVE2FULL;
				//m_stNVCMgznRT[lMgzn].m_lTransferSlotUsage[i] = BL_SLOT_USAGE_UNUSE;
			}
		}
    }

	try
	{
		SaveMgznRTData();
	}
	catch (CFileException e)
	{
		//BL_DEBUGBOX("BL SaveMgznRTData Exception in Reset Magazine");
	}

    return TRUE;
}

BOOL CNVCLoader::IsMagazineCanReset(ULONG lMgzn, BOOL bIsCheckAll)
{
	if (CMS896AStn::m_bEnableResetMagzCheck == FALSE)
	{
		return TRUE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	if (bIsCheckAll == TRUE)
	{
		if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
		{
			return FALSE;
		}

		return TRUE;
	}

	switch ( m_lOMRT )
	{
	case NL_MODE_A:
	case NL_MODE_F:		//MS100 8mag 150bins config		//v3.82
		if (m_stNVCMgznRT[lMgzn].m_lMgznUsage == NL_MGZN_USAGE_ACTIVE)
		{
			if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
			{
				return FALSE;
			}
		}
		break;

	case NL_MODE_E:
	case NL_MODE_G:		//MS100 8mag 175bins config		//v3.82
		if (m_stNVCMgznRT[lMgzn].m_lMgznUsage == NL_MGZN_USAGE_ACTIVE)
		{
			if (pApp->CheckIsAllBinClearedNoMsg() == FALSE)
			{
				return FALSE;
			}
		}
		break;
	}

	return TRUE;
}


VOID CNVCLoader::SetOperationMode_A()
{
    short j = 0;
	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	// top1
	// top1 as FULL magazine under standlone machine
	m_stMgznOM[0].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[0].m_lSlotBlock[j] = j + 76;
		m_stMgznOM[0].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// middle1
	m_stMgznOM[1].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for(j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[1].m_lSlotBlock[j] = j + 101;
		m_stMgznOM[1].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// bottom1
	m_stMgznOM[2].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[2].m_lSlotBlock[j] = j + 126;
		m_stMgznOM[2].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// Mode8 bottom1
	m_stMgznOM[6].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[6].m_lSlotBlock[j] = j + 151;
		m_stMgznOM[6].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}


	// top2
	// top2 as Empty Magazine under standlone
	m_stMgznOM[3].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[3].m_lSlotBlock[j] = j + 1;
		m_stMgznOM[3].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// middle2
	// middle2 as WIP Magazine under standlone
	m_stMgznOM[4].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[4].m_lSlotBlock[j] = j + 26;
		m_stMgznOM[4].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}
	
	// bottom2
	m_stMgznOM[5].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[5].m_lSlotBlock[j] = j + 51;
		m_stMgznOM[5].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// Mode8 bottom2
	m_stMgznOM[7].m_lMgznUsage = NL_MGZN_USAGE_WAFER;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[7].m_lSlotBlock[j] = 0;
		m_stMgznOM[7].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}
}

/*
VOID CNVCLoader::SetOperationMode_A()
{
    short j = 0;
	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();

	// top1
	// top1 as FULL magazine under standlone machine
	m_stMgznOM[0].m_lMgznUsage = NL_MGZN_USAGE_FULL;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[0].m_lSlotBlock[j] = 0;
		m_stMgznOM[0].m_lSlotUsage[j] = NL_SLOT_USAGE_FULL;
	}

	// middle1
	m_stMgznOM[1].m_lMgznUsage = NL_MGZN_USAGE_EMPTY;
	for(j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[1].m_lSlotBlock[j] = 0;
		m_stMgznOM[1].m_lSlotUsage[j] = NL_SLOT_USAGE_EMPTY;
	}

	// bottom1
	m_stMgznOM[2].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[2].m_lSlotBlock[j] = j + 76;
		m_stMgznOM[2].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// Mode8 bottom1
	m_stMgznOM[6].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[6].m_lSlotBlock[j] = j + 101;
		m_stMgznOM[6].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}


	// top2
	// top2 as Empty Magazine under standlone
	m_stMgznOM[3].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[3].m_lSlotBlock[j] = j + 1;
		m_stMgznOM[3].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// middle2
	// middle2 as WIP Magazine under standlone
	m_stMgznOM[4].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[4].m_lSlotBlock[j] = j + 26;
		m_stMgznOM[4].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}
	
	// bottom2
	m_stMgznOM[5].m_lMgznUsage = NL_MGZN_USAGE_ACTIVE;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[5].m_lSlotBlock[j] = j + 51;
		m_stMgznOM[5].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}

	// Mode8 bottom2
	m_stMgznOM[7].m_lMgznUsage = NL_MGZN_USAGE_WAFER;
	for (j = 0; j < MS_BL_MGZN_SLOT; j++)
	{
		m_stMgznOM[7].m_lSlotBlock[j] = 0;
		m_stMgznOM[7].m_lSlotUsage[j] = NL_SLOT_USAGE_ACTIVE;
	}
}*/

CString CNVCLoader::GetOMSlotUsage(ULONG ulMgzn, ULONG ulSlot)
{
    return GetSlotUsage(m_stMgznOM[ulMgzn].m_lSlotUsage[ulSlot]);
}

CString CNVCLoader::GetRTSlotUsage(ULONG ulMgzn, ULONG ulSlot)
{
    return GetSlotUsage(m_stNVCMgznRT[ulMgzn].m_lSlotUsage[ulSlot]);
}

CString CNVCLoader::GetMagazineName(ULONG ulMgzn)
{
    CString str;
    switch ( ulMgzn )
    {
		case NL_MGZ_TOP_1:
			str = "top 1";
			break;

		case NL_MGZ_MID_1:
			str = "mid 1";
			break;

		case NL_MGZ_BTM_1:
			str = "btm 1";
			break;

		case NL_MGZ_TOP_2:
			str = "top 2";
			break;

		case NL_MGZ_MID_2:
			str = "mid 2";
			break;

		case NL_MGZ_BTM_2:
			str = "btm 2";
			break;

		case NL_MGZ8_BTM_1:		
			str = "8mag btm 1";
			break;

		case NL_MGZ8_BTM_2:		
			str = "8mag btm 2";
			break;

		default:
			str = "NONE";	
			break;
    }

    return str;
}

CString CNVCLoader::GetSlotUsage(const LONG lSlotUsageType)
{
	switch (lSlotUsageType)
	{
	case NL_SLOT_USAGE_UNUSE:
		return CString(_T("unuse"));
	case NL_SLOT_USAGE_FULL:
		return CString(_T(" full "));
	case NL_SLOT_USAGE_EMPTY:
		return CString(_T("empty"));
	case NL_SLOT_USAGE_ACTIVE:
		return CString(_T("active"));
	case NL_SLOT_USAGE_ACTIVE2FULL:
		return CString(_T("act2F"));
	case NL_SLOT_USAGE_INUSE:
		return CString(_T("inuse"));
	case NL_SLOT_USAGE_SORTING:
		return CString(_T(" Sort "));
	default:
		return CString(_T("unuse"));
	}
	return CString(_T("unuse"));
}

CString CNVCLoader::GetMgznUsage(LONG lIndex)
{
    if( lIndex < 0 || lIndex > 5)
	{
        lIndex = 0;
	}

	CString szUsage = "UNUSE";
	switch (lIndex)
	{
	case 1:
		szUsage = "FULL";
		break;
	case 2:
		szUsage = "EMPTY";
		break;
	case 3:
		szUsage = "ACTIVE";
		break;
	case 4:
		szUsage = "MIXED";
		break;
	case 5:
		szUsage = "WAFER";
		break;
	case 0:
	default:
		szUsage = "UNUSE";
		break;
	}

	return szUsage;
}

VOID CNVCLoader::UpdateHmiMgznSlotID()
{
	/*
	LONG	m_lCurrMgzn1;
	LONG	m_lCurrMgzn2;
	LONG	m_lCurrSlotID1;
	LONG	m_lCurrSlotID2;
	LONG	m_lCurrBlock1;
	LONG	m_lCurrBlock2;
	LONG	m_lCurrHmiMgzn1;		// Current Mgzn no for DLA display only	
	LONG	m_lCurrHmiMgzn2;		// Current Mgzn no for DLA display only	
	LONG	m_lCurrHmiSlot1;		// Current Slot no 
	LONG	m_lCurrHmiSlot2;		// Current Slot no 
	CString m_szFrameBarcode1;
	CString m_szFrameBarcode2;
	*/
	if (m_lCurrBlock1 == 0)
	{
		m_lCurrHmiMgzn1			= 0;
		m_lCurrHmiSlot1			= 0;
		m_szCurrHmiMgznName1	= "";
		m_szFrameBarcode1		= "";
	}
	else
	{
		m_lCurrHmiMgzn1			= m_lCurrMgzn1 + 1;
		m_lCurrHmiSlot1			= m_lCurrSlotID1 + 1;
		m_szCurrHmiMgznName1	= GetMagazineName(m_lCurrMgzn1);

		if ( (m_lCurrMgzn1 >= 0)	&& (m_lCurrMgzn1 < MS_BL_MGZN_NUM) && 
			 (m_lCurrSlotID1 >= 0)	&& (m_lCurrSlotID1 < MS_BL_MAX_MGZN_SLOT) )
		{
			m_szFrameBarcode1	= m_stNVCMgznRT[m_lCurrMgzn1].m_SlotBCName[m_lCurrSlotID1];
		}
	}

	if (m_lCurrBlock2 == 0)
	{
		m_lCurrHmiMgzn2			= 0;
		m_lCurrHmiSlot2			= 0;
		m_szCurrHmiMgznName2	= "";
		m_szFrameBarcode2		= "";
	}
	else
	{
		m_lCurrHmiMgzn2		= m_lCurrMgzn2 + 1;
		m_lCurrHmiSlot2		= m_lCurrSlotID2 + 1;
		m_szCurrHmiMgznName2 = GetMagazineName(m_lCurrMgzn2);

		if ( (m_lCurrMgzn2 >= 0)	&& (m_lCurrMgzn2 < MS_BL_MGZN_NUM) && 
			 (m_lCurrSlotID2 >= 0)	&& (m_lCurrSlotID2 < MS_BL_MAX_MGZN_SLOT) )
		{
			m_szFrameBarcode2	= m_stNVCMgznRT[m_lCurrMgzn2].m_SlotBCName[m_lCurrSlotID2];
		}
	}
}


BOOL CNVCLoader::GetNullMgznSlot(LONG &lMgzn, LONG &lSlot, LONG lBlock)
{
	INT i = 0, j = 0;
	BOOL bExit = FALSE;
	CString szLog;

	for (i = 0; i < MS_BL_MGZN_NUM; i++) // search the empty frame in empty magazine
	{
		if ( (m_stNVCMgznRT[i].m_lMgznUsage == NL_MGZN_USAGE_EMPTY) ||
			 (m_stNVCMgznRT[i].m_lMgznUsage == NL_MGZN_USAGE_MIXED) )	
		{
			for (j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				if (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_EMPTY)
				{
					if (m_stNVCMgznRT[i].m_lMgznUsage == NL_MGZN_USAGE_MIXED)
					{
						//If MIX mgzn, set this slot to FULL once frame is pulled out to BT
						m_stNVCMgznRT[i].m_lSlotBlock[j] = 0;
						m_stNVCMgznRT[i].m_lSlotUsage[j] = NL_SLOT_USAGE_FULL;
					}
					else
					{
						m_stNVCMgznRT[i].m_lSlotUsage[j] = NL_SLOT_USAGE_UNUSE;
					}
					bExit = TRUE;
					break;
				}
			}
		}

		if (bExit == TRUE)
		{
			break;
		}
	}

	if (bExit == FALSE)
	{
		SetErrorMessage("BL error: No Empty frame in EMPTY magazine");
		SetAlert_Red_Yellow(IDS_BL_MA_NOEPYFRAME_IN_EPYMAG);
		return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	//Option: provide warning to user that the EMPTY mgzn is empty next time
	if (lSlot == m_stNVCMgznRT[lMgzn].m_lNoOfSlots - 1)
	{
	}

	return TRUE;
}

VOID CNVCLoader::GenerateOMSPTableFile(ULONG ulMgzn)
{
    int i = 0, j = 0;
    CString str;
	CStdioFile fTemp;

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\bl_omsetup.csv"), CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText))
	{
		return;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//BOOL bUse29Slots = pApp->GetFeatureStatus(MS896A_FUNC_BL_USE_29_MAGSLOTS);

	for (j = 0; j < 13; j++)
	{
		i = j;
		// slot id
		str.Format("%d,", i+1);
		fTemp.WriteString(str);
		// usage
		str.Format( "%s,", GetOMSlotUsage(ulMgzn, i) );
		fTemp.WriteString(str);
		// physical block
		str.Format("%d,", m_stMgznOM[ulMgzn].m_lSlotBlock[i]);
		fTemp.WriteString(str);
		// space for separator
		fTemp.WriteString(",");
	 
		i = j + 13;
		if (i == MS_BL_MGZN_SLOT)
		{
			fTemp.WriteString(",");
			fTemp.WriteString(",");
			fTemp.WriteString("\n");
			break;
		}

		// slot
		str.Format("%d,", i+1);
		fTemp.WriteString(str);
		// usage
		str.Format("%s,", GetOMSlotUsage(ulMgzn, i) );
		fTemp.WriteString(str);
		// physical
		str.Format("%d,", m_stMgznOM[ulMgzn].m_lSlotBlock[i]);
		fTemp.WriteString(str);
	}

    fTemp.Close();
}

LONG CNVCLoader::SelectSlotUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget)
{
    CStringList szList;
    LONG lUsageNew, lIndex;

    lUsageNew = lUsageOld;
    szList.AddTail(GetSlotUsage(NL_SLOT_USAGE_UNUSE));
    szList.AddTail(GetSlotUsage(lUsageTarget));
    if (lUsageOld == NL_SLOT_USAGE_UNUSE)
        lIndex = 0;
    else
        lIndex = 1;

    lIndex = HmiSelection("please select slot usage", "magazine setup", szList, lIndex);
    
	if (lIndex == 0)
        lUsageNew = NL_SLOT_USAGE_UNUSE;
    else if (lIndex == 1)
        lUsageNew = lUsageTarget;
    else
        lUsageNew = lUsageOld;

    return lUsageNew;
}

LONG CNVCLoader::SelectMgznUsage_UNUSE_TARGET(LONG lUsageOld, LONG lUsageTarget)
{
    CStringList szList;
    LONG lUsageNew, lIndex;

    lUsageNew = lUsageOld;
    szList.AddTail(GetMgznUsage(NL_MGZN_USAGE_UNUSE));
    szList.AddTail(GetMgznUsage(lUsageTarget));
    if ( lUsageOld == NL_MGZN_USAGE_UNUSE )
        lIndex = 0;
    else
        lIndex = 1;
   
	lIndex = HmiSelection("please select magazine usage", "magazine setup", szList, lIndex);
    
	if ( lIndex == 0 )
        lUsageNew = NL_MGZN_USAGE_UNUSE;
    else if ( lIndex == 1 )
        lUsageNew = lUsageTarget;
    else
        lUsageNew = lUsageOld;

    return lUsageNew;
}

LONG CNVCLoader::SelectMgznUsage_UNUSE_TARGET2(LONG lUsageOld, LONG lUsageTarget1, LONG lUsageTarget2)
{
    CStringList szList;
    LONG lUsageNew, lIndex;

    lUsageNew = lUsageOld;
    szList.AddTail(GetMgznUsage(NL_MGZN_USAGE_UNUSE));
    szList.AddTail(GetMgznUsage(lUsageTarget1));
    szList.AddTail(GetMgznUsage(lUsageTarget2));

    if( lUsageOld == NL_MGZN_USAGE_UNUSE )
        lIndex = 0;
    else
        lIndex = 1;

    lIndex = HmiSelection("please select magazine usage", "magazine setup", szList, lIndex);
    
	if (lIndex == 0)
        lUsageNew = NL_MGZN_USAGE_UNUSE;
    else if ( lIndex == 1 )
        lUsageNew = lUsageTarget1;
    else if ( lIndex == 2 )
        lUsageNew = lUsageTarget2;
    else
        lUsageNew = lUsageOld;

    return lUsageNew;
}

VOID CNVCLoader::ChangeOMMgznSlotUsage(LONG lMgznIndex, LONG lUsageNew)
{
	if ( (lMgznIndex == NL_MGZN_TOP1) || (lMgznIndex == NL_MGZN_MID1) )
	{
		for (INT i = 0; i < MS_BL_MGZN_SLOT; i++)
		{
			if (lUsageNew == NL_MGZN_USAGE_EMPTY)
				m_stMgznOM[lMgznIndex].m_lSlotUsage[i] = NL_SLOT_USAGE_EMPTY;
			else if (lUsageNew == NL_MGZN_USAGE_FULL)
				m_stMgznOM[lMgznIndex].m_lSlotUsage[i] = NL_SLOT_USAGE_FULL;
			else
				m_stMgznOM[lMgznIndex].m_lSlotUsage[i] = NL_SLOT_USAGE_UNUSE;
		}
	}
}

BOOL CNVCLoader::CheckPhysicalBlockValid(ULONG ulBlock)
{
	return TRUE;

	/*
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(ULONG), &ulBlock);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("CheckIfPhyBlkToSetupIsValid"), stMsg);

	// Get the reply
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 60 * 1000) == TRUE) // 1 minutes
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bReturn = FALSE;
	stMsg.GetMsg(sizeof(BOOL), &bReturn);
	return bReturn;*/
}

VOID CNVCLoader::GenerateOMRTTableFile(ULONG ulMgzn)
{
    int i, j;
    CString str;
	CStdioFile fTemp;

	//CSingleLock lock(&m_csBinSlotInfoMutex);
	//lock.Lock();

	if (!fTemp.Open(_T("c:\\mapsorter\\exe\\bl_ominform.csv"), CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive|CFile::typeText))
	{
		return;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bUse29Slots = pApp->GetFeatureStatus(MS896A_FUNC_BL_USE_29_MAGSLOTS);

	for (j=0; j<13; j++)
	{
		i = j;
		// slot id
		str.Format("%d,", i+1); 
		fTemp.WriteString(str);
		// usage
		str.Format("%s,", GetRTSlotUsage(ulMgzn, i));
		fTemp.WriteString(str);
		// physical block
		str.Format("%d,", m_stNVCMgznRT[ulMgzn].m_lSlotBlock[i]);
		fTemp.WriteString(str);
		// bar code
		str.Format("%s,", m_stNVCMgznRT[ulMgzn].m_SlotBCName[i]);
		fTemp.WriteString(str);
		// wip counter
		str.Format("%d,", m_stNVCMgznRT[ulMgzn].m_lSlotWIPCounter[i]);
		fTemp.WriteString(str);
		// space for separator
		fTemp.WriteString(",");
	 
		i = j+13;
		if( i==MS_BL_MGZN_SLOT )
		{
			fTemp.WriteString(",");
			fTemp.WriteString(",");
			fTemp.WriteString(",");
			fTemp.WriteString(",");
			fTemp.WriteString(",");
			fTemp.WriteString("\n");
			break;
		}
		// slot
		str.Format("%d,", i+1);
		fTemp.WriteString(str);
		// usage
		str.Format("%s,", GetRTSlotUsage(ulMgzn, i));
		fTemp.WriteString(str);
		// physical block
		str.Format("%d,", m_stNVCMgznRT[ulMgzn].m_lSlotBlock[i]);
		fTemp.WriteString(str);
		// bar code
		str.Format("%s,", m_stNVCMgznRT[ulMgzn].m_SlotBCName[i]);
		fTemp.WriteString(str);
		// wip counter
		str.Format("%d,", m_stNVCMgznRT[ulMgzn].m_lSlotWIPCounter[i]);
		fTemp.WriteString(str);
	}

    fTemp.Close();
}
