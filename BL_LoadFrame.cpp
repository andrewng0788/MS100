/////////////////////////////////////////////////////////////////
// BL_LoadFrame.cpp : Move functions of the CBinLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			BarryChu
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
#include "CTmpChange.h"
#include "WaferTable.h"
#include "BinTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CBinLoader::SeparateClamp_BT(VOID)
{
	if (!m_fHardware)
	{
		return;
	}
	if (m_bDisableBL)	//v3.60
	{
		return;
	}

	//if( m_poGripperLevel->IsHigh() && m_poFrameLevel->IsHigh() )
	//    return ;
	SetBLCurTime(12, 6);
	SetGripperLevel(FALSE);
	//SetFrameLevel(FALSE);
	Sleep(500); // delay to move gripper and bin table
}

VOID CBinLoader::SettleDownBeforeUnload(BOOL bOffline)
{
	//Sleep(200);
	SetBLCurTime(12, 3);
	SetFrameVacuum(FALSE);

	
	//SetFrameAlign(FALSE);
	//Sleep(m_lBTAlignFrameDelay);

	SetBLCurTime(12, 4);
	SetFrameLevel(TRUE);
	SetBLCurTime(12, 7);
	SetGripperLevel(TRUE);
	SetBLCurTime(12, 9);
	SetGripperState(FALSE);
	Sleep(500); 
}

VOID CBinLoader::AlignBinFrame()
{
	LONG i;
	
	Sleep(100);			//v2.71
	for (i = 0; i < m_lBTAlignFrameCount; i++)
	{
		SetFrameAlign(TRUE);
		Sleep(m_lBTAlignFrameDelay);

		SetFrameAlign(FALSE);
		Sleep(m_lBTAlignFrameDelay);
	}

	//v4.43T3
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bAleadyEnableVac = FALSE;
	if ( (CMS896AApp::m_lCycleSpeedMode >= 5) )
	{
		Sleep(m_lVacSettleDelay);			//v3.67T1		//Semitek
		SetFrameVacuum(TRUE);
		bAleadyEnableVac = TRUE;			//v4.51A1
	}

	SetFrameAlign(TRUE);
	Sleep(m_lBTAlignFrameDelay);

	//v4.37T3
	if (!bAleadyEnableVac)					//If not MS100P3/MS60/MS90	//v4.51A1
	{
		if (m_bUseBinTableVacuum)			//Used before Vac is applied
		{
			Sleep(m_lVacSettleDelay);		//v3.67T1		//Semitek
			SetFrameVacuum(TRUE);
			Sleep(m_lBTVacuumDelay);		//Used AFTER Vac is applied
		}
	}
}


LONG CBinLoader::GetLoadMgzSlot(BOOL &bUseEmpty, LONG lBlock, BOOL bUseBT2, BOOL bDisableModeHSkipSlot)
{
	CString str;
	BOOL bExit = FALSE;
	LONG i, j;
	BOOL bModeHSkipOnce = FALSE;

	if (IsAutoLoadUnloadQCTest() || IsAutoLoadUnloadSISTest())
	{
		return TRUE;
	}

	//v4.59A11
	if (IsMSAutoLineMode())
	{
		return GetLoadMgzSlot_AutoLine(lBlock, bUseEmpty);
	}

	bExit = FALSE;
	switch (m_lOMRT)
	{
		case BL_MODE_A:		// GET LOAD
		case BL_MODE_F:		//v3.82		//MS100 8mag config
			for (i = 0; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
				{
					continue;
				}

				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE2FULL)
						{
							bUseEmpty = TRUE;
						}
						else
						{
							bUseEmpty = FALSE;
						}
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;
		case BL_MODE_B:
			bUseEmpty = TRUE;
			return TRUE;
		case BL_MODE_C: // GET LOAD
			bUseEmpty = FALSE;
			for (i = 0; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_MIXED)
				{
					continue;
				}
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
						{
							continue;
						}
						else
						{
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
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;
		case BL_MODE_D: // GET LOAD
			//v4.16T5	//New ModeD for Cree China
			for (i = 0; i < MS_BL_MGZN_NUM; i++)
			{
				if ((m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE) && 
						(m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_MIXED))
				{
					continue;
				}
				for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_EMPTY)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_FULL)
					{
						continue;
					}

					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE2FULL)
						{
							bUseEmpty = TRUE;
						}
						else
						{
							bUseEmpty = FALSE;
						}
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MAX_MGZN_SLOT)
			{
				return FALSE;
			}
			break;

		case BL_MODE_E:		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			for (i = 0; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
				{
					continue;
				}
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE2FULL)
						{
							bUseEmpty = TRUE;
						}
						else
						{
							bUseEmpty = FALSE;
						}
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;

		case BL_MODE_H:			//v4.33T9
			for (i = 0; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
				{
					continue;
				}
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE2FULL)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_FULL)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						//CHeck if table already has same grade frame -> new EMPTY frame is supposed to be pulled out
						if (!bDisableModeHSkipSlot && !bModeHSkipOnce && (lBlock == GetCurrBinOnBT()))
						{
							//So skip current slot and find next available one as new EMPTY frame in Grade mgzn
							bModeHSkipOnce = TRUE;
							continue;
						}

						bUseEmpty = FALSE;
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;

		default:
			SetAlert_Red_Yellow(IDS_BL_OPMODE_WRONG);
			SetErrorMessage("BL Incorrect operation mode");
			return FALSE;
	}

	if (bUseBT2)	//MS100 9Inch dual-table config		//v4.17T1
	{
		m_lCurrMgzn2 = i;
		m_lCurrSlot2 = j;
	}
	else
	{
		m_lCurrMgzn = i;
		m_lCurrSlot = j;
	}

	//v4.49A7
	CString szTemp;
	szTemp.Format("GetLoadMgzSlot - Mgzn #%d Slot #%d; bEmpty=%d", m_lCurrMgzn, m_lCurrSlot, bUseEmpty);
	BL_DEBUGBOX(szTemp);

	//Update HMI variable
//	if (!m_bIsExChgArmExist && !m_bIsDualBufferExist)	//v3.94	// Use UpdateExArmMgzSlotNum() to update HMI var instead for DLA
//	{
//		if (bUseBT2)
//		{
//			m_lCurrHmiSlot = m_lCurrSlot + 1;
//		}
//		else
//		{
//			m_lCurrHmiSlot = m_lCurrSlot + 1;
//		}
//	}

	return TRUE;
}


LONG CBinLoader::GetMgznSlot(LONG lBlock, LONG &lMgzn, LONG &lSlot)
{
	CString str;
	BOOL bExit;
	LONG i, j;

	if (IsAutoLoadUnloadQCTest() || IsAutoLoadUnloadSISTest())
	{
		lMgzn = m_lCurrMgzn;
		lSlot = m_lCurrSlot;
		return TRUE;
	}

	if (IsMSAutoLineMode())
	{
		return GetMgzSlot_AutoLine(lBlock, lMgzn, lSlot);
	}

	bExit = FALSE;
	switch (m_lOMRT)
	{
		case BL_MODE_A: // GET LOAD
		case BL_MODE_F: //v3.82		//MS100 8mag config
			for (i = 1; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
				{
					continue;
				}
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;

		case BL_MODE_B:
			return TRUE;

		case BL_MODE_C: // GET LOAD
			for (i = 1; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_MIXED)
				{
					continue;
				}
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
						{
							continue;
						}
						else
						{
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
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;

		case BL_MODE_D: // GET LOAD
			//v4.16T5	//New ModeD for Cree China
			for (i = 0; i < MS_BL_MGZN_NUM; i++)		//v4.39T5
			{
				if ((m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE) ||
						(m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED))
				{
					for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_EMPTY)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_FULL)
						{
							continue;
						}

						if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
						{
							bExit = TRUE;
							break;
						}
					}

					if (bExit == TRUE)
					{
						break;
					}
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MAX_MGZN_SLOT)
			{
				return FALSE;
			}
/*
        for(i=1; i<MS_BL_MGZN_NUM; i++)
        {
            if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_MIXED )
                continue;
            for(j=0; j<MS_BL_MGZN_SLOT; j++)
            {
                if( m_stMgznRT[i].m_lSlotUsage[j]!=BL_SLOT_USAGE_ACTIVE )
                    continue;
                if( m_stMgznRT[i].m_lSlotBlock[j]==lBlock )
                {
                    bExit = TRUE;
                    break;
                }
            }
            if( bExit==TRUE )
                break;
        }
        if( i==MS_BL_MGZN_NUM || j==MS_BL_MGZN_SLOT )
        {
            return FALSE;
        }
*/
			break;

		case BL_MODE_E:		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			for (i = 1; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
				{
					continue;
				}
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;

		case BL_MODE_H:		//MS100 4mag 100bins config		//v4.31T10		//v4.33T9
			for (i = 1; i < MS_BL_MGZN_NUM; i++)
			{
				if (m_stMgznRT[i].m_lMgznUsage != BL_MGZN_USAGE_ACTIVE)
				{
					continue;
				}
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
					{
						continue;
					}
					if (m_stMgznRT[i].m_lSlotBlock[j] == lBlock)
					{
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (i == MS_BL_MGZN_NUM || j == MS_BL_MGZN_SLOT)
			{
				return FALSE;
			}
			break;

		default:
			return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	return TRUE;
}

LONG CBinLoader::ClearAllFrameResetNullMagazine()
{
	CString szMagSlot;
	CString str;
	LONG lMgzn, lSlot;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bNichia = (pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == _T(""));	//v4.59A34

	if (GetNextNullMgznSlot(lMgzn, lSlot) == TRUE)
	{
		return TRUE;
	}

	if (DownElevatorThetaZToPosn(0) == FALSE)
	{
		return FALSE;
	}

	LONG lStatus = 0;
	switch (m_lOMRT)
	{
		case BL_MODE_A: // get full
		case BL_MODE_F: //v3.82		//MS100 8mag config
			if (pApp->GetCustomerName() == "ChangeLight(XM)")
			{
				lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
			}
			else
			{
				lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET, IDS_BL_OPERATION);
			}
			
			if (lStatus == TRUE)
			{
				if (GetNullMgznNo(lMgzn) == TRUE)
				{
					ResetMagazine(lMgzn);
				}

				if (bNichia)	//v4.41T1
				{
					LONG lMgzn2 = 0;
					if (GetNullMgznNo(lMgzn2, lMgzn) == TRUE)
					{
						ResetMagazine(lMgzn2);
					}
				}

				return TRUE;
			}
			else
			{
				return FALSE;
			}

		case BL_MODE_B: // get full
			return FALSE;
		
		case BL_MODE_C:
			return FALSE;

		case BL_MODE_D: // get full
			//v4.16T5	//New ModeD for Cree China
			if (pApp->GetCustomerName() == "ChangeLight(XM)")
			{
				lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
			}
			else
			{
				lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET, IDS_BL_OPERATION);
			}
			if (lStatus == TRUE)
			{
				if (GetNullMgznNo(lMgzn) == TRUE)
				{
					ResetMagazineEmpty(lMgzn);		//v4.16T5	New ResetMgzn command for ModeD MIX mgzn only!!
				}

				return TRUE;
			}
			else
			{
				return FALSE;
			}

		case BL_MODE_E:		// get full		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			//case BL_MODE_H:	//MS100 4mag 100bins config		//v4.31T10
			if (pApp->GetCustomerName() == "ChangeLight(XM)")
			{
				lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
			}
			else
			{
				lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET, IDS_BL_OPERATION);
			}
			if (lStatus == TRUE)
			{
				//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				//if(pApp->GetCustomerName()=="ChangeLight(XM)") 
				//{
				//	OpOutoutMgznSummary();
				//}
				if (GetNullMgznNo(lMgzn) == TRUE)
				{
					ResetMagazine(lMgzn);
				}

				return TRUE;
			}
			else
			{
				return FALSE;
			}
	}
	
	return TRUE;
}

LONG CBinLoader::GetNullMgznNo(LONG &lMgzn, CONST LONG lMgznToStart)
{
	CString szMagSlot;
	CString str;
	BOOL bExit;
	LONG i = lMgznToStart;		//v4.41T1	//Nichia
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//BOOL bNichia = (pApp->GetCustomerName() == "Nichia");	//anichia002
	BOOL bNichia = (pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == _T(""));	//v4.59A34

	bExit = FALSE;
	switch (m_lOMRT)
	{
		case BL_MODE_A:
		case BL_MODE_F: 
			if (bNichia)		//anichia002
			{
				for (i = lMgznToStart; i >= 0; i--) // get full slot in full magazine
				{
					if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
					{
						lMgzn = i;
						return TRUE; 
					}
				}
			}
			else
			{
				for (i = lMgznToStart; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
				{
					if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
					{
						lMgzn = i;
						return TRUE; 
					}
				}
			}
			break;

		case BL_MODE_B: // get full
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_SLOT_USAGE_EMPTY)
				{
					lMgzn = i;
					return TRUE;
				}
			}
			break;

		case BL_MODE_C:
			break;

		case BL_MODE_D:
			break;

		case BL_MODE_E:		// get full		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			//case BL_MODE_H:	//MS100 4mag 100bins config		//v4.31T10
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					lMgzn = i;
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}

BOOL CBinLoader::RestoreNullMgznSlot(LONG lMgzn, LONG lSlot)		//v4.39T8	//Cree
{
	if ((lMgzn < 0) || (lMgzn >= MS_BL_MGZN_NUM))
	{
		return FALSE;
	}
	if ((lSlot < 0) || (lSlot >= MS_BL_MGZN_SLOT))
	{
		return FALSE;
	}

	CString szMsg;
	szMsg.Format("RestoreNullSlot at Mgzn %d SLot %d", lMgzn, lSlot);

	switch (m_lOMRT)
	{
		case BL_MODE_A:	
		case BL_MODE_B:
		case BL_MODE_D:
		case BL_MODE_F:	
		case BL_MODE_G:
			m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_EMPTY;

			BL_DEBUGBOX(szMsg);
			break;
	}

	return TRUE;
}


//This funciton previous is GetNullMgznSlot_Auto
LONG CBinLoader::GetNullMgznSlot(LONG &lMgzn, LONG &lSlot, LONG lBlock)
{
	CString str;
	BOOL bExit;
	LONG i, j;
	CString szMsg;
	LONG lSlotBlock;


	//v4.59A12
	if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())	//AUTOLINE mode
	{
		return GetNullMgznSlot_AutoLine(lBlock, lMgzn, lSlot);
	}

	LONG lCurrBinNo = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];	//v4.33T9
	szMsg.Format("Get empty frame curr Bin No(Slot Block): %d (Mgzn=%d, Slot=%d)", 
		lCurrBinNo, m_lCurrMgzn, m_lCurrSlot);
	BL_DEBUGBOX(szMsg);
	
	bExit = FALSE;

	//v4.41T1	//Nichia
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRTMgznExchange = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE);
	BOOL bCheckMgznSnrs = FALSE;
	if (((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) || CMS896AStn::m_bUseBinMultiMgznSnrs) && bRTMgznExchange)
	{
		bCheckMgznSnrs = TRUE;
	}

	//BOOL bNichia = (pApp->GetCustomerName() == "Nichia");	//anichia002
	BOOL bNichia = (pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == _T(""));	//v4.59A34
	// 4.55T09 Get
	BOOL bCheckEmptyFrameBC	= pApp->GetFeatureStatus(MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK);					

	switch (m_lOMRT)
	{
		case BL_MODE_A:		// GET EMPTY
		case BL_MODE_F:		//v3.82		//MS100 8mag config
			
			if (bNichia)	//nichia002
			{
				for (i=MS_BL_MGZN_NUM-1; i>=0; i--) // search the empty frame in empty magazine
				{
					if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
					{
						if (bCheckMgznSnrs)		//v4.41T1
						{
							if (!IsMagazineExist(i))
							{
								Sleep(100);
								if (!IsMagazineExist(i))
								{
									continue;
								}
							}
						}

						for (j = 0; j < MS_BL_MGZN_SLOT; j++)
						{
							if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
							{
								continue;
							}
							else
							{
								m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
			}
			else if( bCheckEmptyFrameBC)
			{
				for (i = 0; i < MS_BL_MGZN_NUM; i++) // search the empty frame in empty magazine
				{
					if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
					{
						if (bCheckMgznSnrs)		
						{
							if (!IsMagazineExist(i))
							{
								Sleep(100);
								if (!IsMagazineExist(i))
								{
									continue;
								}
							}
						}
						
						for (j = 0; j < MS_BL_MGZN_SLOT; j++)
						{
							lSlotBlock = m_stMgznRT[i].m_lSlotBlock[j];

							if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY || lSlotBlock != lCurrBinNo)
							{
								continue;
							}
							else
							{
								m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
								bExit = TRUE;
								//4.55T09 change to unuse
								szMsg.Format("Frame change to unuse from M%d S%d B%d",i,j, lSlotBlock);
								BL_DEBUGBOX(szMsg);

								break;
							}
						}
					}
					if (bExit == TRUE)
					{
						break;
					}
				}
			}
			else
			{
				for (i = 0; i < MS_BL_MGZN_NUM; i++) // search the empty frame in empty magazine
				{
					if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
					{
						if (bCheckMgznSnrs)		//v4.41T1
						{
							if (!IsMagazineExist(i))
							{
								Sleep(100);
								if (!IsMagazineExist(i))
								{
									continue;
								}
							}
						}

						for (j = 0; j < MS_BL_MGZN_SLOT; j++)
						{
							if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
							{
								continue;
							}
							else
							{
								m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
			}
			
			if (bExit == FALSE)
			{
				
				SetAlert_Red_Yellow(IDS_BL_MA_NOEPYFRAME_IN_EPYMAG);

				SetErrorMessage("BL error: No Empty frame in empty magazine");
				szMsg.Format("Get Null MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		case BL_MODE_B:		// GET EMPTY
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get empty frame in empty magazine then
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
						{
							continue;
						}
						else
						{
							m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
				SetAlert_Red_Yellow(IDS_BL_MB_NOFRAME_IN_EMPTYSLOT);
				SetErrorMessage("BL Mode B error: No frame in empty slot");
				szMsg.Format("Get Null MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		case BL_MODE_C:
			SetAlert_Red_Yellow(IDS_BL_MC_NOEPYMAG);
			SetErrorMessage("BL Mode C error: No empty magazine");
			szMsg.Format("Get Null MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
			BL_DEBUGBOX(szMsg);
			return FALSE;

		case BL_MODE_D: // GET EMPTY
			//v4.16T5	//New ModeD for Cree China
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // search the empty frame in empty magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
						{
							continue;
						}
						else
						{
							m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
				SetAlert_Red_Yellow(IDS_BL_MA_NOEPYFRAME_IN_EPYMAG);
				SetErrorMessage("BL Mode D error: No Empty frame in empty magazine");
				szMsg.Format("Get Null MgznSlot mode is %ld MODE_D = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		case BL_MODE_E:		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // search the empty frame in empty magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
						{
							continue;
						}
						else
						{
							m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;		//EMPTY to FULL
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
				SetAlert_Red_Yellow(IDS_BL_ME_NOEPYFRAME_IN_EPYMAG);
				SetErrorMessage("BL Mode E error: No Empty frame in empty magazine");
				szMsg.Format("Get Null MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		case BL_MODE_H:		//v4.33T9
			for (i = 0; i < MS_BL_MGZN_NUM; i++)		// search the next available EMPTY frame in all grade magazines
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
						{
							continue;
						}
						else
						{
							//LONG lCurrBinNo = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];	//v4.33T9
							LONG lBinNo	= m_stMgznRT[i].m_lSlotBlock[j];
							if (lCurrBinNo != lBinNo)
							{
								continue;
							}
							if ((i == m_lCurrMgzn) && (j == m_lCurrSlot))	//Cannot be same slot as current frame on table
							{
								continue;
							}

							szMsg.Format("GetNullMgznSlot (Mode-H): Mgzn=%ld, Slot=%ld, Grade=%ld", i + 1, j + 1, lBinNo);
							BL_DEBUGBOX(szMsg);
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
				CString szErr;
				LONG lCurrBinNo = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];

				SetAlert_Red_Yellow(IDS_BL_ME_NOEPYFRAME_IN_EPYMAG);
				szErr.Format("BL Mode H error: No next Empty frame found in grade magazines - Mgzn=%ld, Slot=%ld, Bin=#%ld", 
							 m_lCurrMgzn + 1, m_lCurrSlot + 1, lCurrBinNo);
				SetErrorMessage("BL Mode H error: No next Empty frame found in grade magazines");
				//szMsg.Format("Get Null MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
				BL_DEBUGBOX(szErr);
				return FALSE;
			}
			break;

		default:
			SetAlert_Red_Yellow(IDS_BL_OPMODE_WRONG);
			SetErrorMessage("BL Incorrect operation mode");
			szMsg.Format("Get Null MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
			BL_DEBUGBOX(szMsg);
			return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	if (lSlot == MS_BL_MGZN_SLOT - 1)
	{
		m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_EMPTY;
		if (m_bBurnInEnable == FALSE)
		{
			SetErrorMessage("BL No empty slot next time in empty magazine");
			szMsg.Format("Get Null MgznSlot no empty slot next time, mode is %ld MODE_A = 0", m_lOMRT);
			BL_DEBUGBOX(szMsg);
		}
	}

	return TRUE;
}


LONG CBinLoader::GetNextNullMgznSlot(LONG &lMgzn, LONG &lSlot)
{
	CString str;
	BOOL bExit;
	LONG i, j;
	LONG lBufferBlock = 0;
	BOOL bModeHSkipOnce = FALSE;
	bExit = FALSE;

	//v4.41T1	//Nichia
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRTMgznExchange = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE);
	BOOL bCheckMgznSnrs = FALSE;
	if (((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) || CMS896AStn::m_bUseBinMultiMgznSnrs) && bRTMgznExchange)
	{
		bCheckMgznSnrs = TRUE;
	}

	//BOOL bNichia = (pApp->GetCustomerName() == "Nichia");	//anichia002
	BOOL bNichia = (pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == _T(""));	//v4.59A34


	switch (m_lOMRT)
	{
		case BL_MODE_A: // GET EMPTY
		case BL_MODE_F: //v3.82		//MS100 8mag config
			if (bNichia)			//anichia002
			{
				for (i=MS_BL_MGZN_NUM-1; i>=0; i--) // search the empty frame in empty magazine
				{
					if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
					{
						if (bCheckMgznSnrs)		//v4.41T1
						{
							if (!IsMagazineExist(i))
							{
								Sleep(100);
								if (!IsMagazineExist(i))
								{
									continue;
								}
							}
						}

						for (j = 0; j < MS_BL_MGZN_SLOT; j++)
						{
							if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
							{
								continue;
							}
							else
							{
								//m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
			}
			else
			{
				for (i = 0; i < MS_BL_MGZN_NUM; i++) // search the empty frame in empty magazine
				{
					if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
					{
						if (bCheckMgznSnrs)		//v4.41T1
						{
							if (!IsMagazineExist(i))
							{
								Sleep(100);
								if (!IsMagazineExist(i))
								{
									continue;
								}
							}
						}

						for (j = 0; j < MS_BL_MGZN_SLOT; j++)
						{
							if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
							{
								continue;
							}
							else
							{
								//m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
			}
			if (bExit == FALSE)
			{
				return FALSE;
			}
			break;

		case BL_MODE_B: // GET EMPTY
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get empty frame in empty magazine then
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
						{
							continue;
						}
						else
						{
							//m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
				SetAlert_Red_Yellow(IDS_BL_MB_NOFRAME_IN_EMPTYSLOT);
				SetErrorMessage("BL Mode B error: No frame in empty slot");
				return FALSE;
			}
			break;

		case BL_MODE_C:
			SetAlert_Red_Yellow(IDS_BL_MC_NOEPYMAG);
			SetErrorMessage("BL Mode C error: No empty magazine");
			return FALSE;

		case BL_MODE_D: // GET EMPTY
			//v4.16T5	//New ModeD for Cree China
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // search the empty frame in empty magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
						{
							continue;
						}
						else
						{
							//m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_UNUSE;
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
				return FALSE;
			}
			break;

		case BL_MODE_E:		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			for (i = 0; i < MS_BL_MGZN_NUM; i++)		// search the empty frame in empty magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY)
						{
							continue;
						}
						else
						{
							//m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
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
				return FALSE;
			}
			break;

		case BL_MODE_H:		//MS100 4mag 100bins config		//v4.31T10		//v4.34T1
			for (i = 0; i < MS_BL_MGZN_NUM; i++)		// search the empty frame in empty magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE)
						{
							continue;
						}

						lBufferBlock = DB_PreloadBlock();

						LONG lBinNum = m_stMgznRT[i].m_lSlotBlock[j];
					
						if (!bModeHSkipOnce && (lBinNum == lBufferBlock))
						{
							bModeHSkipOnce = TRUE;
							continue;
						}

						CString szLog;
						szLog.Format("GetNextNullMgznSlot (Mode-H): Mgzn=%ld, Slot =%ld", i, j);
						BL_DEBUGBOX(szLog);
						//m_stMgznRT[i].m_lSlotUsage[j] = BL_SLOT_USAGE_FULL;
						bExit = TRUE;
						break;
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (bExit == FALSE)
			{
				return FALSE;
			}
			break;

		default:
			SetAlert_Red_Yellow(IDS_BL_OPMODE_WRONG);
			SetErrorMessage("BL Incorrect operation mode");
			return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	return TRUE;
}


BOOL CBinLoader::ResetFullMagazine(LONG lMgzn)
{
	if (!IsFullSlotInMgzn(lMgzn))
	{
		return FALSE;
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	//BOOL bNichia = (pApp->GetCustomerName() == "Nichia");
	BOOL bNichia = (pApp->GetCustomerName() == CTM_NICHIA) && (pApp->GetProductLine() == _T(""));	//v4.59A34
	LONG lStatus = 0;

	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		lStatus = BL_YES_NO(IsMSAutoLineStandloneMode() ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET_CHANGELIGHTXM : HMB_BL_MAGZ_IS_FULL_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
	}
	else
	{
		lStatus = BL_YES_NO((lMgzn == BL_MGZ_TOP_2) ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET : HMB_BL_MAGZ_IS_FULL_RESET, IDS_BL_OPERATION);
	}

	if (lStatus == TRUE)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if(pApp->GetCustomerName()=="ChangeLight(XM)") 
		{
			//To magazine TOP2
			OpOutoutMgznSummary((lMgzn == BL_MGZ_TOP_2) ? TRUE : FALSE);
		}

		if ((lMgzn == BL_MGZ_TOP_2) || (GetFullMgznNo(lMgzn) == TRUE))
		{
			ResetMagazine(lMgzn);
		}

		if (bNichia)	//v4.41T1
		{
			LONG lMgzn2 = 0;
			if (GetFullMgznNo(lMgzn2, lMgzn) == TRUE)
			{
				ResetMagazine(lMgzn2);
			}
		}

		return TRUE;
	}

	return FALSE;
}

LONG CBinLoader::ClearAllFrameResetFullMagazine()
{
	LONG lMgzn = 0, lSlot = 0;
	
	if (IsMSAutoLineMode())
	{
		if (!IsClearFrameOutputFullMgazSlot_AutoLine(lMgzn, lSlot))
		{
			return TRUE;
		}
	}
	else
	{
		if (GetNextFullMgazSlot(lMgzn, lSlot) == TRUE)
		{
			return TRUE;
		}
	}

	if (DownElevatorThetaZToPosn(0) == FALSE)
	{
		return FALSE;
	}

	if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())
	{
		return ResetFullMagazine(BL_MGZ_TOP_2);
	}
	return ResetFullMagazine(lMgzn);
}


LONG CBinLoader::GetFullMgznNo(LONG &lMgzn, CONST LONG lMgznToStart)
{
	CString szMagSlot;
	CString str;
	BOOL bExit;
	LONG i = lMgznToStart;		//v4.41T1	//Nichia

	bExit = FALSE;
	switch (m_lOMRT)
	{
		case BL_MODE_A:
		case BL_MODE_F: 
			for (i = lMgznToStart; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_FULL)
				{
					lMgzn = i;
					return TRUE; 
				}
			}
			break;

		case BL_MODE_B: // get full
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_FULL)
				{
					lMgzn = i;
					return TRUE;
				}
			}
			break;

		case BL_MODE_C:
			break;

		case BL_MODE_D:
			//v4.16T5	//New ModeD for Cree China
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					lMgzn = i;
					return TRUE; 
				}
			}
			break;

		case BL_MODE_E:		// get full		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			//case BL_MODE_H:	//MS100 4mag 100bins config		//v4.31T10
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					lMgzn = i;
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}


LONG CBinLoader::GetNextFullMgazSlot(LONG &lMgzn, LONG &lSlot)
{
	CString szMagSlot;
	CString str;
	BOOL bExit;
	LONG i = 0, j = 0;		//Klocwork

	bExit = FALSE;
	switch (m_lOMRT)
	{
		case BL_MODE_A:
		case BL_MODE_F: 
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_FULL)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				return FALSE;
			}
			break;

		case BL_MODE_B: // get full
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_FULL)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				return FALSE;
			}
			break;

		case BL_MODE_C:
			i = m_lCurrMgzn;
			j = m_lCurrSlot;
			break;

		case BL_MODE_D: // get full
			//v4.16T5	//New ModeD for Cree China
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				return FALSE;
			}
			break;

		case BL_MODE_E:		// get full		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			//case BL_MODE_H:	//MS100 4mag 100bins config		//v4.31T10
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				return FALSE;
			}
			break;
	}

	lMgzn = i;
	lSlot = j;

	return TRUE;
}


BOOL CBinLoader::GetFullMgznSlot_Renesas(LONG &lMgzn, LONG &lSlot, BOOL bDisableFullCheck, LONG lPhysicalBlock)
{
	BOOL bExit = FALSE;
	INT i = 0;
	INT j = 0;

	if (m_nRenesasBLConfigMode == 1)	//RENESAS MODE_A
	{
		if (lPhysicalBlock == 1)
		{
			i = 1;		// Mid1 mgzn
		}
		else if (lPhysicalBlock == 2)
		{
			i = 2;		// Btm1 mgzn
		}
	}
	else if (m_nRenesasBLConfigMode == 2)	//RENESAS MODE_B
	{
		if (lPhysicalBlock == 1)
		{
			i = 4;		// Mid2 mgzn
		}
		else if (lPhysicalBlock == 2)
		{
			i = 5;		// Btm2 mgzn
		}
	}

	//shiraishi01
	CString szLog;
	szLog.Format("GetFullMgznSlot_Renesas: BLK = %ld, MGZN = %d, MgznUsage = %d",
				lPhysicalBlock, i, m_stMgznOM[i].m_lMgznUsage);
	BL_DEBUGBOX(szLog);

	if (m_stMgznOM[i].m_lMgznUsage == BL_MGZN_USAGE_FULL)
	{
		for (j = 0; j < MS_BL_MGZN_SLOT; j++)
		{
			if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
			{
				continue;
			}

			if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
			{
				lMgzn = i;
				lSlot = j;
				bExit = TRUE;
				break;
			}
		}
	}

	szLog.Format("GetFullMgznSlot_Renesas: RESULT = %ld, MGZN = %d, SLOT = %d",
				bExit, lMgzn, lSlot);
	BL_DEBUGBOX(szLog);

	return bExit;
}



//This function previous is GetFullMgznSlot_Auto
LONG CBinLoader:: GetFullMgznSlot(LONG &lMgzn, LONG &lSlot, BOOL bDisableFullCheck, LONG lPhysicalBlock)
{
	if (IsAutoLoadUnloadQCTest())
	{
		return TRUE;
	}

	//AutoLine	//v4.59A11
	BOOL bAutoLineMode = IsMSAutoLineMode();
	if (bAutoLineMode)
	{
		return GetFullMgznSlot_AutoLine(lMgzn, lSlot);
	}

	CString szMagSlot;
	CString str;
	BOOL bExit;
	LONG i, j;
	CString szMsg;

	//v4.41T1	//Nichia
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRTMgznExchange = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE);
	BOOL bCheckMgznSnrs = FALSE;
	if (((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) || CMS896AStn::m_bUseBinMultiMgznSnrs) && bRTMgznExchange)
	{
		bCheckMgznSnrs = TRUE;
	}

	bExit = FALSE;
	switch (m_lOMRT)
	{
		case BL_MODE_A: // get full
		case BL_MODE_F: //v3.82		//MS100 8mag config
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_FULL)
				{
					if (bCheckMgznSnrs)		//v4.41T1
					{
						if (!IsMagazineExist(i))
						{
							Sleep(100);
							if (!IsMagazineExist(i))
							{
								continue;
							}
						}
					}

					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
				SetErrorMessage("BL Mode F error: No free slot in full magazine");
				szMsg.Format("Get full MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		case BL_MODE_B: // get full
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_FULL)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				SetAlert_Red_Yellow(IDS_BL_MB_NOFREESLOT_IN_FULLMAG);
				SetErrorMessage("BL Mode B error: No free slot in full magazine");
				szMsg.Format("Get full MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		case BL_MODE_C:
			i = m_lCurrMgzn;
			j = m_lCurrSlot;
			break;

		case BL_MODE_D: // get full
			//v4.16T5	//New ModeD for Cree China
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetCustomerName() != "Cree")
				{
					SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
				}
				else
				{
					SetAlert_Red_Yellow(IDS_BL_MD_NOFREESLOT_IN_MAG);
				}
				SetErrorMessage("BL Mode D error: No free slot in full magazine");
				szMsg.Format("Get full MgznSlot mode is %ld MODE_D = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}

/*
        i = m_lCurrMgzn;
        if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_MIXED )
        {
            SetAlert_Red_Yellow(IDS_BL_MD_UNKNOWN_ERROR2);
			SetErrorMessage("BL Mode D error: Unknown error 2");
			szMsg.Format("Get full MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
			BL_DEBUGBOX(szMsg);
			return FALSE;
        }
        for(j=0; j<MS_BL_MGZN_SLOT; j++)
        {
            if( m_stMgznRT[i].m_lSlotUsage[j]!=BL_SLOT_USAGE_FULL )
                continue;
            if( m_stMgznRT[i].m_lSlotBlock[j]==0 )
            {
                break;
            }
        }
        if( j==MS_BL_MGZN_SLOT )
        {
            SetAlert_Msg_Red_Yellow(IDS_BL_MD_NOFREESLOT_IN_MAG, GetMagazineName(i).MakeUpper());		
			SetErrorMessage("BL Mode D error: No free slot in magazine");
			szMsg.Format("Get full MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
			BL_DEBUGBOX(szMsg);
			return FALSE;
        }
*/
			break;

		case BL_MODE_E:		// get full		//v3.45
		case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotBlock[j] == 0)
						{
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
				SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
				SetErrorMessage("BL Mode A error: No free slot in full magazine");
				szMsg.Format("Get full MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		case BL_MODE_H:		//MS100 4mag 100bins config		//v4.31T10	//v4.33T9
			for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
			{
				if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_ACTIVE)
				{
					for (j = 0; j < MS_BL_MGZN_SLOT; j++)
					{
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_UNUSE)
						{
							continue;
						}
						if (m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_FULL)
						{
							continue;
						}
						//if( m_stMgznRT[i].m_lSlotUsage[j] == BL_SLOT_USAGE_ACTIVE2FULL )                  
						//	continue;
						//if ( (i==m_lCurrMgzn) && (j==m_lCurrSlot) )
						//{
						LONG lCurrBinNo = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];
						LONG lBinNo		= m_stMgznRT[i].m_lSlotBlock[j];
						if (lBinNo != lCurrBinNo)
						{
							continue;
						}

						szMsg.Format("GetFullMgznSlot (Mode-H): Mgzn=%ld, Slot=%ld, Usage=%ld", 
									 i + 1, j + 1, m_stMgznRT[i].m_lSlotUsage[j]);
						BL_DEBUGBOX(szMsg);		//v4.44T1
						bExit = TRUE;
						break;
						//}
					}
				}
				if (bExit == TRUE)
				{
					break;
				}
			}
			if (bExit == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
				szMsg.Format("BL Mode H error: No free slot in grade mgzn - Mgzn=%ld, Slot=%ld", m_lCurrMgzn + 1, m_lCurrSlot + 1);
				SetErrorMessage(szMsg);
				BL_DEBUGBOX(szMsg);
				return FALSE;
			}
			break;

		default:
			SetAlert_Red_Yellow(IDS_BL_OPMODE_WRONG);
			SetErrorMessage("BL Incorrect operation mode");
			szMsg.Format("Get full MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
			BL_DEBUGBOX(szMsg);
			return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	szMagSlot.Format("%d/%d", lMgzn + 1, lSlot + 1);
	(*m_psmfSRam)["BinLoader"]["CurFullSlot"] = szMagSlot;

	//pllm
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
	//if (nPLLM == PLLM_LUMIRAMIC)
	if (pApp->GetProductLine() == "Lumiramic")		//v4.35T1
	{
		bPLLMSpecialFcn = TRUE;    //Lumiramic Dual-DLA also uses REBEL sequence!!		//v4.02T4
	}		

//	if( pApp->IsBLBCUseOldGoStop() )
//		bPLLMSpecailFcn = TRUE;

	if (bPLLMSpecialFcn)
	{
		if (lSlot >= MS_BL_MGZN_SLOT - 1)	
		{
			m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_FULL;
		}

		if (!bExit || (lSlot > MS_BL_MGZN_SLOT - 1))
		{
			if (m_bBurnInEnable == FALSE)
			{				
				//if (pApp->GetProductLine() != "WH")
				//{
				//	SetAlert_Msg_Red_Yellow(IDS_BL_NOFREESLOT_IN_FULLMAG, GetMagazineName(i).MakeUpper());		
				//	SetErrorMessage("BL No free slot in full magazine");
				//	return BL_MGZ_IS_FULL;
				//}//2019.05.01 Matt:No check this temporaily
			}
		}
	}
	else
	{
		if (m_lOMRT == BL_MODE_D)	//v4.19	//Cree HuiZhou
		{
			if (!bDisableFullCheck && (lSlot >= MS_BL_MAX_MGZN_SLOT - 1))
			{
				m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_FULL;

				if (m_bBurnInEnable == FALSE)
				{				
					//if( !(pApp->GetCustomerName()=="SanAn" && pApp->GetProductLine()=="WH") )
					//{
					//	SetAlert_Msg_Red_Yellow(IDS_BL_NOFREESLOT_IN_FULLMAG, GetMagazineName(i).MakeUpper());		
					//	SetErrorMessage("BL No free slot next time in full magazine");
					//	szMsg.Format("Get no free slot in full MgznSlot mode is %ld MODE_D = 0", m_lOMRT);
					//	BL_DEBUGBOX(szMsg);
					//	return BL_MGZ_IS_FULL;//2019.05.01 Matt:No check this temporaily
					//}
				}
			}
		}
		else if (m_lOMRT != BL_MODE_H)		//v4.34T2
		{
			if (!bDisableFullCheck && (lSlot >= MS_BL_MGZN_SLOT - 1))	//v3.89
			{
				m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_FULL;

				if (m_bBurnInEnable == FALSE)
				{
					if (bCheckMgznSnrs)		//v4.41T2
					{
						SetErrorMessage("BL No free slot next time in full magazine");
						szMsg.Format("Get no free slot in full Mgzn %d Slot in MODE %c", i, (CHAR)('A' + m_lOMRT));
						BL_DEBUGBOX(szMsg);
					}
					else
					{
						//if( !(pApp->GetCustomerName()=="SanAn" && pApp->GetProductLine()=="WH") )
						//{
						//	SetAlert_Msg_Red_Yellow(IDS_BL_NOFREESLOT_IN_FULLMAG, GetMagazineName(i).MakeUpper());		
						//	SetErrorMessage("BL No free slot next time in full magazine");
						//	szMsg.Format("Get no free slot in full MgznSlot mode is %ld MODE_A = 0", m_lOMRT);
						//	BL_DEBUGBOX(szMsg);
						//	return BL_MGZ_IS_FULL;
						//}//2019.05.01 Matt:No check this temporaily
					}
				}
			}
		}
	}

	return TRUE;
}

LONG CBinLoader::GetThetaLoadPosn(const LONG lMgzn)
{
	BOOL bToTop2T = FALSE;
	switch (lMgzn)
	{
	case BL_MGZ_TOP_2:
	case BL_MGZ_MID_2:
	case BL_MGZ_BTM_2:
	case BL_MGZ8_BTM_2:
		bToTop2T = TRUE;
		break;
	default:
		break;
	}

	// Move elevator T
	INT nPosT = m_lLoadPos1_T;
	if (bToTop2T)
	{
		nPosT = m_lLoadPos2_T;
	}
	return nPosT;
}

VOID CBinLoader::StopZMotion()
{
	CMS896AStn::MotionStop(BL_AXIS_THETA, &m_stBLAxis_Theta);
	CMS896AStn::MotionStop(BL_AXIS_Z, &m_stBLAxis_Z);
	Sleep(100);
	CMS896AStn::MotionClearError(BL_AXIS_THETA, &m_stBLAxis_Theta);
	CMS896AStn::MotionClearError(BL_AXIS_Z, &m_stBLAxis_Z);

//	Theta_Sync();
//	Z_Sync();
}


BOOL CBinLoader::CheckSafeToMove(const BOOL bAskCoverOpen)
{
	if ((IsFrameOutOfMgz() == TRUE))
	{
		SetErrorMessage("BL Frame is out of magazine");
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		return FALSE;
	}

	if (IsMagazineSafeToMove() == FALSE)
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		return FALSE;
	}

	if (IsElevatorCoverOpen() == TRUE)
	{
		if (bAskCoverOpen)
		{
			HmiMessage("Please close elevator door to proceed to READY position ...");
		}
		if (CheckElevatorCover() == FALSE)
		{
			if (bAskCoverOpen)
			{
				HmiMessage_Red_Yellow("Elevator cover door not closed!");
			}
			SetErrorMessage("BL Elevator cover door not closed!");
			return FALSE;
		}
	}
/*
	if (IsCoverOpen() == TRUE)
	{
		if (bAskCoverOpen)
		{
			HmiMessage("Please close main cover door to proceed to READY position ...");
		}
		if ( IsCoverOpen() == TRUE )
		{
			if (bAskCoverOpen)
			{
				HmiMessage_Red_Yellow("main cover door not closed!");
			}
			SetErrorMessage("main cover door not closed!");
			return FALSE;
		}
	}
*/
	return TRUE;
}


BOOL CBinLoader::CheckCoverOpen()
{
	if (IsElevatorCoverOpen() == TRUE)
	{
		StopZMotion();

		if (CheckElevatorCover() == FALSE)
		{
			if (IsFrameOutOfMgz() == FALSE)				
			{
				Theta_Home();
				Z_Home();
			}
			SetAlert_Red_Yellow(IDS_BL_ELEVATOR_COVER_OPEN);
			return FALSE;
		}
		else
		{
			m_bHome_Z = FALSE;
			SetAlert_Red_Yellow(IDS_BL_ELEVATOR_COVER_OPEN);
			return FALSE;
//				nResultT = Theta_MoveTo(nPosT, SFM_NOWAIT);
//				nResultZ = Z_SMoveTo(nPosZ, SFM_NOWAIT);
//				nResultZ = Z_MoveTo(nPosZ, SFM_NOWAIT);
		}
	}
/*
	if (IsCoverOpen() == TRUE)
	{
		StopZMotion();

		if (CheckCover() == FALSE)
		{
			if (IsFrameOutOfMgz() == FALSE)				
			{
				Theta_Home();
				Z_Home();
			}
			SetAlert_Red_Yellow(IDS_BL_ELEVATOR_COVER_OPEN);
			return FALSE;
		}
		else
		{
			m_bHome_Z = FALSE;
			SetAlert_Red_Yellow(IDS_BL_ELEVATOR_COVER_OPEN);
			return FALSE;

//			nResultT = Theta_MoveTo(nPosT, SFM_NOWAIT);
//			nResultZ = Z_MoveTo(nPosZ, SFM_NOWAIT);
		}
	}
*/
/*
	if ((IsFrameOutOfMgz() == TRUE))
	{
		Sleep(10);
		if ((IsFrameOutOfMgz() == TRUE))
		{
			StopZMotion();

			m_bHome_Z = FALSE;
			SetErrorMessage("BL Frame is out of magazine (CheckCoverOpen)");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
			return FALSE;
		}
	}
*/
	return TRUE;
}


LONG CBinLoader::MoveElevatorToLoad(LONG lMgzn, LONG lSlot, CBinGripperBuffer *pGripperBuffer, BOOL bUseBT2, BOOL bMoveGripperToReady)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}


	LONG lPosn = GetReadyPosn(pGripperBuffer);
	if (m_bNoSensorCheck || !IsFrameInClamp(pGripperBuffer->GetFrameInClampSensorName()))
	{
		lPosn = GetPreUnloadPosn(pGripperBuffer);
	}
	if (GripperMoveTo(pGripperBuffer, lPosn) != TRUE)
	{
		SetErrorMessage("Bin Gripper fails when moving to Ready Pos");
		//HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		X_Home();	//v3.94T4
		return FALSE;
	}

	if (!CheckSafeToMove())
	{
		return FALSE;
	}

	//remove the accumulating Z error value
	if (m_clLowerGripperBuffer.IsTriggerThetaHome() || m_clUpperGripperBuffer.IsTriggerThetaHome() ||
		_round(GetCurTime() - m_dZHomeTime) > BIN_Z_HOME_TIMEOUT_HOUR * 60 * 60 * 1000)
	{
		m_clLowerGripperBuffer.SetTriggerThetaHome(FALSE);
		m_clUpperGripperBuffer.SetTriggerThetaHome(FALSE);
		Z_SMoveTo(0, SFM_WAIT);
		Z_Home();
		Theta_Home();
	}

	INT nPosT = GetThetaLoadPosn(lMgzn);

	LONG lUpperToLowerOffsetZ = 0;
	if ( (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) && !pGripperBuffer->IsUpperBuffer())
	{
		lUpperToLowerOffsetZ = m_lUpperToLowerBufferOffsetZ;
	}

	INT nPosZ = m_stMgznRT[lMgzn].m_lTopLevel - m_stMgznRT[lMgzn].m_lSlotPitch * lSlot + lUpperToLowerOffsetZ;
	
	CString szLog;
	szLog.Format("MoveElevatorToLoad: Mgzn=%d, Slot=%d, BufOffsetZ=%ld, EncZ=%ld, EncT=%ld", 
					lMgzn, lSlot, lUpperToLowerOffsetZ, nPosZ, nPosT);
	BL_DEBUGBOX(szLog);

	return MoveElevatorToWithCheckMissingStep("MoveElevatorToLoad", nPosT, nPosZ, TRUE);
}


LONG CBinLoader::MoveElevatorToNextLoad(LONG lMgzn, LONG lSlot, BOOL bBufferLevel, BOOL bWait, BOOL bUseBT2)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	if (!CheckSafeToMove())
	{
		return FALSE;
	}

	//Get Theta Position
	INT nPosT = GetThetaLoadPosn(lMgzn);

	LONG lUpperToLowerOffsetZ = 0;
	if ( (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) && 
		 (bBufferLevel == BL_BUFFER_LOWER) )
	{
		lUpperToLowerOffsetZ = m_lUpperToLowerBufferOffsetZ;
	}

	// Move elevator Z
	INT nPosZ = m_stMgznRT[lMgzn].m_lTopLevel - m_stMgznRT[lMgzn].m_lSlotPitch * lSlot + lUpperToLowerOffsetZ;
	
	CString szLog;
	szLog.Format("MoveElevatorToNextLoad: Mgzn=%d, Slot=%d, BufOffsetZ=%ld, EncZ=%ld, EncT=%ld", 
					lMgzn, lSlot, lUpperToLowerOffsetZ, nPosZ, nPosT);
	BL_DEBUGBOX(szLog);
	
	return MoveElevatorToWithCheckMissingStep("MoveElevatorToNextLoad", nPosT, nPosZ, bWait);
}


LONG CBinLoader::MoveElevatorToUnload(LONG lMgzn, LONG lSlot, BOOL bBufferLevel, BOOL bMoveOffset, BOOL bWait, BOOL bDisableOutOfMgznCheck, BOOL bUseBT2)
{
	LONG lOffsetZ = 0;
	LONG lOffsetY = 0;
	LONG lEncoderPos = 0;
	BOOL bNeedWait = FALSE;

	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	if (!CheckSafeToMove())
	{
		return FALSE;
	}

	if (bMoveOffset == TRUE)
	{
		lOffsetZ = m_lUnloadOffset;
	}

	if (m_clLowerGripperBuffer.IsTriggerThetaHome() || m_clUpperGripperBuffer.IsTriggerThetaHome())
	{
		m_clLowerGripperBuffer.SetTriggerThetaHome(FALSE);
		m_clUpperGripperBuffer.SetTriggerThetaHome(FALSE);
		Z_SMoveTo(0, SFM_WAIT);
		Z_Home();
		Theta_Home();
	}

	//Get Theta Position
	INT nPosT = GetThetaLoadPosn(lMgzn);

	LONG lUpperToLowerOffsetZ = 0;
	if ((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) && 
		(bBufferLevel == BL_BUFFER_LOWER))
	{
		lUpperToLowerOffsetZ = m_lUpperToLowerBufferOffsetZ;
	}

	INT nPosZ = m_stMgznRT[lMgzn].m_lTopLevel - m_stMgznRT[lMgzn].m_lSlotPitch * lSlot + lOffsetZ + lUpperToLowerOffsetZ;

	CString szLog;
	szLog.Format("MoveElevatorToUnload: Mgzn=%d, Slot=%d, OffsetZ=%ld, BufOffsetZ=%ld, EncZ=%ld, EncT=%ld", 
					lMgzn, lSlot, lOffsetZ, lUpperToLowerOffsetZ, nPosZ, nPosT);
	BL_DEBUGBOX(szLog);

	return MoveElevatorToWithCheckMissingStep("MoveElevatorToUnload", nPosT, nPosZ, bWait, bDisableOutOfMgznCheck);
}



BOOL CBinLoader::YZ_Sync_OutOfMagChecking(BOOL bCheckRealignRpy)
{
	Sleep(100);

	while (CMS896AStn::MotionIsComplete(BL_AXIS_Z, &m_stBLAxis_Z) != TRUE)
	{
		Sleep(100);

		if (!CheckCoverOpen())
		{
			return FALSE;
		}

		if (bCheckRealignRpy)
		{
			BOOL bBinAlignRpy = RealignBinFrameRpy(TRUE);
			if (!bBinAlignRpy)
			{
				Z_Sync();
				BL_DEBUGBOX("Z_SyncWithOutOfMagChecking fails because bin realignment fails");
				SetErrorMessage("Z_SyncWithOutOfMagChecking fails because bin realignment fails");
				SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);
				return FALSE;
			}
		}
	}

	Z_Sync();
	Sleep(150);
	return TRUE;
}


LONG CBinLoader::DownElevatorToReady(CBinGripperBuffer *pGripperBuffer, BOOL bWait)
{
	return UDB_DownElevatorToReady(pGripperBuffer, bWait);
}


LONG CBinLoader::ElevatorZMoveTo_Auto(LONG lZLevel)
{
	if (ElevatorZSynMove_Auto() == FALSE)
	{
		return FALSE;
	}

	if (!CheckSafeToMove())
	{
		return FALSE;
	}

	if (Z_MoveTo(lZLevel, SFM_NOWAIT) != gnOK)
	{
		return FALSE;
	}

	return TRUE;
}

LONG CBinLoader::ElevatorZSynMove_Auto(VOID)
{
	if (Z_Sync() == gnOK)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinLoader::AbortElevatorMotionIfOutOfMag()		//v4.18T1	
{
	if ((IsFrameOutOfMgz() == TRUE))	//&& (m_bBurnInEnable == FALSE) )	//v3.89
	{
		Sleep(10);
		if ((IsFrameOutOfMgz() == TRUE))
		{
			CMS896AStn::MotionStop(BL_AXIS_Z, &m_stBLAxis_Z);
			Z_Sync();

			SetErrorMessage("Abort BL YZ motion due to OutOfMag sensor triggered.");
			BL_DEBUGBOX("Abort loader YZ motion due to OutOfMag sensor triggered.");
			return TRUE;
		}
	}
	return FALSE;
}

LONG CBinLoader::DownElevatorToReady_Auto(VOID)
{
	return ElevatorZMoveTo_Auto(m_lReadyLvlZ);
}


LONG CBinLoader::BinGripperSearchBTPosition(INT siDirection, LONG lSearchPosition, LONG lSearchVelocity, BOOL bWaitComplete)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableBL)	//v3.60
	{
		return FALSE;
	}

	LONG lSearchDistance;
	LONG lEnc = GetGripperEncoder();
	lSearchDistance = abs(lSearchPosition - lEnc) + 500;

	try
	{
		CMS896AStn::MotionSelectSearchProfile(BL_AXIS_UPPER, "spfBinGripperSearchBTPosition", &m_stBLAxis_Upper);
#ifndef NU_MOTION
		CMS896AStn::MotionUpdateSearchProfile(BL_AXIS_UPPER, "spfBinGripperSearchBTPosition",
											  lSearchVelocity, lSearchDistance, &m_stBLAxis_Upper);
#endif
		CMS896AStn::MotionSearch(BL_AXIS_UPPER, !siDirection, SFM_NOWAIT, &m_stBLAxis_Upper, "spfBinGripperSearchBTPosition");
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (bWaitComplete == SFM_WAIT)
	{
		Upper_Sync();
		Sleep(100);
	}

	return TRUE;
}


LONG CBinLoader::ExArmBinGripperMoveSearchJam(INT nPos, BOOL bCheckBinAlign, BOOL &bBinAlignRpy)
{
	if (m_bDisableBL)	//v3.60
	{
		return TRUE;
	}

	INT nResult;
	nResult = Upper_MoveTo(nPos, SFM_NOWAIT);

	BOOL bCheckAlign = TRUE;
	if (!bCheckBinAlign)
	{
		bCheckAlign = FALSE;
	}

	LONG lCheckTime = 0;
	LONG lProfileTime = 0;
	//CalculateProfileTime("mpfBinGripper", (m_lUnloadPos_X - nPos), lProfileTime);
	lProfileTime = CMS896AStn::MotionGetProfileTime(BL_AXIS_X, "mpfBinGripper", (m_lUnloadPos_X - nPos), (m_lUnloadPos_X - nPos), HIPEC_SAMPLE_RATE, &m_stBLAxis_X);

	if (lProfileTime > 0)
	{
		lCheckTime = lProfileTime / 3;		//v2.93T2
	}

	DOUBLE dEnd		= 0;
	DOUBLE dStart	= GetTime();

	CString szTemp;
	szTemp.Format("ExArmBinGripperMoveSearchJam: profile time = %d; check time = %d", lProfileTime, lCheckTime);
	BL_DEBUGBOX(szTemp);

	while (1)
	{
		if (nResult != gnOK)
		{
			return FALSE;
		}
		
		//if ( m_pStepper_X->IsComplete() == TRUE )
		if (CMS896AStn::MotionIsComplete(BL_AXIS_X, &m_stBLAxis_X) == TRUE)
		{
			X_Sync();
			break;
		}

		if (IsFrameJam() == TRUE)
		{
			//m_pStepper_X->Stop();
			CMS896AStn::MotionStop(BL_AXIS_X, &m_stBLAxis_X);
			BL_DEBUGBOX("Frame Jam when ExArmBinGripperMoveSearchJam()");
			return FALSE;
		}

		dEnd = GetTime();
		if (bCheckAlign && (lCheckTime > 0) && ((dEnd - dStart) > lCheckTime))
		{
			bCheckAlign = FALSE;
			bBinAlignRpy = RealignBinFrameRpy(TRUE);	//v2.93T2
		}

		Sleep(1);
	}

	BL_DEBUGBOX("End - ExArmBinGripperMoveSearchJam()");

	return TRUE;
}


LONG CBinLoader::LoadFrameFromSlotToSlot(BOOL bBurnIn, LONG lFromMgz, LONG lFromSlot, LONG lToMgz, LONG lToSlot)
{
	BL_DEBUGBOX("detach");
	SeparateClamp_BT(); // CALLED WHEN START BOND

	BL_DEBUGBOX("bt move to unload");
#ifdef NU_MOTION
	if (MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, 1, TRUE) == FALSE)	//v4.22T7
	{
		return FALSE;
	}
#else
	if (MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y) == FALSE)			//v4.23T2
	{
		return FALSE;
	}
#endif

	BL_DEBUGBOX("Gripper ready and magazine");
	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}
	// move elevator to the slot and gripper to ready
	CBinGripperBuffer *pGripperBuffer = &m_clLowerGripperBuffer;
	if (MoveElevatorToLoad(lFromMgz, lFromSlot, pGripperBuffer) == FALSE)
	{
		HouseKeeping(m_lReadyPos_X);
		return FALSE;
	}

	if (bBurnIn == FALSE && IsMagazineExist(lFromMgz, FALSE, TRUE) == FALSE)	//v4.41T2
	{
		return FALSE;
	}

	// begin to load frame
	SetGripperState(FALSE);
	SetGripperLevel(TRUE);
	Sleep(500);

	// search the frame in clamp
	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}


	BinGripperMoveTo(m_lLoadMagPos_X);
	if (!IsFrameInClamp())
	{
		if (BinGripperSearchInClamp(HP_POSITIVE_DIR, m_lLoadSearchInClampOffsetX, 400) == FALSE)
		{
			//if( bBurnIn==FALSE && BL_OK_CANCEL("Please pull frame into gripper", "Unload Frame Message")==FALSE )
			if ((bBurnIn == FALSE) && 
					(!m_bDisableBL) &&		//v3.60
					(BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE))
			{
				return FALSE;
			}
		}
	}
	Sleep(200);
	SetGripperState(TRUE);

	// wait the event from bin table move to unload complete
	SetFrameLevel(TRUE);
	SetFrameVacuum(FALSE);
	SetFrameAlign(FALSE);
	Sleep(500);

	BL_DEBUGBOX("gripper to scan position");
	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}

	if (BinGripperSearchJam(HP_POSITIVE_DIR, m_lBarcodePos_X, 1000) == FALSE)
	{
		SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_TABLE_TO_BUFFER);
		//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
		//SetErrorMessage("BL Gripper Jam");
		SetGripperState(FALSE);
		SetFrameLevel(FALSE);
		BL_DEBUGBOX("Gripper is jammed");

		return FALSE;
	}

	BL_DEBUGBOX("magazine to unload");
	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}

	if (MoveElevatorToUnload(lToMgz, lToSlot, pGripperBuffer->IsUpperBuffer()) == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	if (bBurnIn == FALSE && IsMagazineExist(lToMgz, FALSE, TRUE) == FALSE)	//v4.41T2
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	BL_DEBUGBOX("#1 gripper to load");
	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}

	if (BinGripperSearchJam(HP_POSITIVE_DIR, m_lLoadMagPos_X, 1000) == FALSE)
	{
		SetGripperState(FALSE);
		//if( bBurnIn==FALSE && BL_OK_CANCEL("Frame is jammed!\nPlease push it into the magazine slot", "Unload Frame Message")==FALSE )
		BOOL bOk = BinLoaderUnloadJamMessage(bBurnIn);

		if (bBurnIn == FALSE && bOk == FALSE)
		{
			SetFrameLevel(FALSE);
			return FALSE;
		}
	}
	Sleep(200);
	SetGripperState(FALSE);
	Sleep(200);

	BL_DEBUGBOX("gripper move to ready");

	if (BinGripperMoveTo(m_lReadyPos_X) != TRUE)
	{
		HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		return FALSE;
	}

	SetGripperLevel(FALSE);
	SetFrameLevel(FALSE);
	Sleep(1000);
	// send event to let bin table go

	BL_DEBUGBOX("down magazine");
	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}

	if (DownElevatorToReady(pGripperBuffer) == FALSE)
	{
		HouseKeeping(m_lReadyPos_X);
		return FALSE;
	}

	BL_DEBUGBOX("BT to 0,0");
	MoveBinTable(0, 0);

	return TRUE;
}


// =====================================================================================================================
// =====================================================================================================================
BOOL CBinLoader::RealignBinFrame(ULONG ulBlkID, BOOL bUseBT2)
{
	BOOL bReturn = TRUE;
	int nConvID = 0;
	IPC_CServiceMessage stMsg, svMsg;

	if (m_bRealignBinFrame == FALSE)
	{
		return TRUE;
	}

	if (ulBlkID <= 0 || ulBlkID > MS_MAX_BIN)
	{
		return TRUE;
	}

	if (m_bDisableBT)
	{
		return TRUE;
	}

	if (!m_bBurnInEnable && !m_bNoSensorCheck && !m_bDisableBL)	//v3.94
	{
		LONG lStatus = 0;
		if (bUseBT2)
		{
			lStatus = CheckFrameOnBinTable2();    //v4.17T6
		}		
		else
		{
			lStatus = CheckFrameOnBinTable();
		}

		if (lStatus == BL_FRAME_NOT_EXIST)	
		{
			SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
			SetErrorMessage("BL No frame exists in bintable");
			return FALSE;
		}
	}

	BL_DEBUGBOX("Manual Re-align bin frame start");
//2018.6.28
//	(*m_psmfSRam)["BinLoader"]["CurrMgzn"]		= m_lCurrMgzn;
//	(*m_psmfSRam)["BinLoader"]["CurrSlot"]		= m_lCurrSlot;
	(*m_psmfSRam)["BinLoader"]["BCScanRange"]	= m_lScanRange;
	(*m_psmfSRam)["BinLoader"]["BCRetryLimit"]	= m_lTryLimits;


	BL_DEBUGBOX("Realign bin frame start");
	stMsg.InitMessage(sizeof(ULONG), &ulBlkID);
	SetFrameAlign(TRUE);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("RealignPhysicalBlockCmd"), stMsg);

	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 100 * 60 * 1000) == TRUE) // 100 minutes
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

	svMsg.GetMsg(sizeof(BOOL), &bReturn);

	if (bReturn == FALSE)
	{
		BL_DEBUGBOX("Manual Re-align bin frame fail");
		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= FALSE;		//v4.42T12
		if (!m_bBurnInEnable)
		{
			//SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);	//v4.46T9
			if (State() != IDLE_Q)	//shiraishi02
			{
				SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);	//v4.46T9
			}
			if (bUseBT2)
			{
				SetErrorMessage("BL Frame re-alignment 2 failed");
			}
			else
			{
				SetErrorMessage("BL Frame re-alignment failed");
			}
		}
	}
	else
	{
		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= TRUE;		//v4.42T12
		BL_DEBUGBOX("Manual Re-align bin frame done\n");
	}

	return bReturn;
}



BOOL CBinLoader::RealignBinFrameReq(ULONG ulBlkID, BOOL bUseEmpty, BOOL bUseBT2)
{
	IPC_CServiceMessage stMsg, svMsg;
	m_nExArmReAlignBinConvID = 0;

	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable != NULL)
	{
		pBinTable->SetRealignBinFrameReq(FALSE, 0, FALSE, FALSE);
	}

	if (m_bRealignBinFrame == FALSE)
	{
		//v4.40T5	//Nichia MS100+ //For BT T barcode scanning purpose een if Realign is not enabled
//2018.6.28
//		(*m_psmfSRam)["BinLoader"]["CurrMgzn"]		= m_lCurrMgzn;
//		(*m_psmfSRam)["BinLoader"]["CurrSlot"]		= m_lCurrSlot;
		(*m_psmfSRam)["BinLoader"]["BCScanRange"]	= m_lScanRange;
		(*m_psmfSRam)["BinLoader"]["BCRetryLimit"]	= m_lTryLimits;
		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= TRUE;		//v4.42T12

		SetBLReady(TRUE);		// Allow BT to proceed if no need for BT realignment in DLA config
		return TRUE;
	}

	//if (m_bDisableBL)			//v3.60
	if (m_bDisableBT)			//v4.57A4
	{
		SetBLReady(TRUE);		// Allow BT to proceed if no need for BT realignment in DLA config
		return TRUE;
	}

	if (ulBlkID <= 0 || ulBlkID > MS_MAX_BIN)
	{
		return FALSE;
	}

	if (!m_bBurnInEnable && !m_bNoSensorCheck && !m_bDisableBL)	//v3.60
	{
		LONG lStatus = TRUE;
		if (bUseBT2)
		{
			lStatus = CheckFrameOnBinTable2();
		}
		else
		{
			lStatus = CheckFrameOnBinTable();
		}

		if (lStatus == BL_FRAME_NOT_EXIST)
		{
			SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
			SetErrorMessage("BL No frame exists in bintable");
			return FALSE;	//v3.39
		}
	}
//2018.6.28
//	(*m_psmfSRam)["BinLoader"]["CurrMgzn"]		= m_lCurrMgzn;
//	(*m_psmfSRam)["BinLoader"]["CurrSlot"]		= m_lCurrSlot;
	(*m_psmfSRam)["BinLoader"]["BCScanRange"]	= m_lScanRange;
	(*m_psmfSRam)["BinLoader"]["BCRetryLimit"]	= m_lTryLimits;

	if (State() != IDLE_Q)
	{
		if (!m_bBurnInEnable && (pBinTable != NULL))
		{
			pBinTable->SetRealignBinFrameReq(TRUE, ulBlkID, bUseEmpty, bUseBT2);
		}

		BL_DEBUGBOX("Re-align bin frame - SetBLReady TRUE");
		SetBLReady(TRUE);			// Allow BT to proceed only after BT realignment is done

		return TRUE;
	}

	stMsg.InitMessage(sizeof(ULONG), &ulBlkID);
	if (bUseEmpty)
	{
		if (CMS896AStn::m_bEnableEmptyBinFrameCheck == FALSE)
		{
			BL_DEBUGBOX("start RealignEmptyFrameCmd");
			m_nExArmReAlignBinConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("RealignEmptyFrameCmd"), stMsg);
		}
		else
		{
			BL_DEBUGBOX("start ExArmRealignEmptyFrameWithPRCmd");
			m_nExArmReAlignBinConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("ExArmRealignEmptyFrameWithPRCmd"), stMsg);

			if (RealignBinFrameRpy(FALSE, FALSE) == FALSE)
			{
				return FALSE;
			}

			BL_DEBUGBOX("complete ExArmRealignEmptyFrameWithPRCmd");

			m_nExArmReAlignBinConvID = 0;
		}
	}
	else
	{
		BL_DEBUGBOX("start RealignPhysicalBlockCmd");
		m_nExArmReAlignBinConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("RealignPhysicalBlockCmd"), stMsg);
	}

	return TRUE;
}


BOOL CBinLoader::RealignBinFrameRpy(CONST BOOL bCheckOnce, CONST BOOL bSetBLReady)
{
	if (m_nExArmReAlignBinConvID == 0)
	{
		BL_DEBUGBOX("No need to receive  Re-align bin frame reply");
		return TRUE;
	}

	IPC_CServiceMessage svMsg;

	if (bCheckOnce)
	{
		if (m_comClient.ScanReplyForConvID(m_nExArmReAlignBinConvID, 50) == TRUE)		//v2.93T2
		{
			m_comClient.ReadReplyForConvID(m_nExArmReAlignBinConvID, svMsg);
		}
		else
		{
			BL_DEBUGBOX("Re-align bin frame reply ONCE not yet");		//v3.98T3
			return TRUE;
		}
	}
	else
	{
		while (1)
		{
			if (m_comClient.ScanReplyForConvID(m_nExArmReAlignBinConvID, 1000) == TRUE)	// 1-sec
			{
				m_comClient.ReadReplyForConvID(m_nExArmReAlignBinConvID, svMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
	}


	m_nExArmReAlignBinConvID = 0;
	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bReturn);

	if (bReturn == FALSE)
	{
		BL_DEBUGBOX("Re-align bin frame reply FAIL");
		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= FALSE;
		if (!m_bBurnInEnable)
		{
			if (State() != IDLE_Q)
			{
				SetAlert_Red_Yellow(IDS_BL_FRAME_REALIGN_FAIL);
			}
			SetErrorMessage("BL Frame re-alignment failed 2");
			return FALSE;
		}
		return FALSE;
	}

	(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= TRUE;
	BL_DEBUGBOX("Re-align bin frame reply received");

	if (bSetBLReady)
	{
		BL_DEBUGBOX("Re-align bin frame - SetBLReady TRUE");
		SetBLReady(TRUE);			// Allow BT to proceed only after BT realignment is done
	}

	return TRUE;
}

BOOL CBinLoader::ResetFrameIsAligned(ULONG ulBlkID)		//v4.42T10
{
	if (ulBlkID <= 0 || ulBlkID > MS_MAX_BIN)
	{
		return FALSE;
	}
	if (m_bRealignBinFrame == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBL)
	{
		return TRUE;
	}
	//v4.42T13
/*
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(ULONG), &ulBlkID);
	INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("ResetFrameIsAligned"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 3000) == TRUE)
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
*/
	(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"] = FALSE;
	return TRUE;
}

BOOL CBinLoader::HomeBinTable(ULONG ulTable)		//v4.17T6
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg, svMsg;

	if (m_bDisableBT)
	{
		return TRUE;
	}
	if (m_lBinLoaderConfig != BL_CONFIG_DUAL_DL)
	{
		return TRUE;
	}

	stMsg.InitMessage(sizeof(ULONG), &ulTable);
	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("HomeTableForRealign_BL"), stMsg);

	// Get the reply
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 1000) == TRUE)  // 100 minutes
		{
			m_comClient.ReadReplyForConvID(nConvID, svMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bReturn);
	return bReturn;
}


BOOL CBinLoader::ClearBinFrameCounter(ULONG ulBlkID, CString szBarCodeName, BOOL bHouseKeeping)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bReturn = FALSE;

	//(*m_psmfSRam)["BinLoader"]["BarcodeOnFrame"] = szBarCodeName;
	stMsg.InitMessage(sizeof(ULONG), &ulBlkID);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("AutoClrBinCnt"), stMsg);
	while (1)
	{
		if (m_comClient.ScanReplyForConvID(nConvID, 100 * 60 * 1000) == TRUE) // 100 minutes
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	stMsg.GetMsg(sizeof(BOOL), &bReturn);
	if (bReturn == FALSE)
	{
		CString szMsg;
		szMsg.Format("%d", ulBlkID);

		SetAlert_Msg_Red_Yellow(IDS_BL_CLEARBIN_FAILED, szMsg);
		SetErrorMessage("BL Clear Bin Failed");

		if (bHouseKeeping == TRUE)
		{
			HouseKeeping(m_lPreUnloadPos_X);
		}
	}


	ULONG ulDieCount = (*m_psmfSRam)["BinTable"]["ClearBin"]["Bonded"];

	CString szCassetePos = GetCassettePositionName(m_lCurrMgzn);
	SendClearBin_8018(m_lCurrMgzn, m_lCurrSlot, szBarCodeName, szCassetePos, ulBlkID, ulDieCount);

	return bReturn;
}


ULONG CBinLoader::GetBinCount(ULONG ulBlkID)
{
	ULONG ulBinCount = 0;
	LONG lBinNo = ulBlkID;

	INT	nConvID = 0;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(LONG), &lBinNo);

	nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("GetBinNumber"), stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 100*60*1000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	stMsg.GetMsg(sizeof(ULONG), &ulBinCount);
	return ulBinCount;
}


BOOL CBinLoader::CheckPhysicalBlockValid(ULONG ulBlock)
{
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

	return bReturn;
}

//This function previous is PreStartInit_Auto
LONG CBinLoader::PreStartInit(VOID)
{
	//Check motors power is on before start move
	if (IsAllMotorsEnable() == FALSE)
	{
		m_bMotionFail = TRUE;
		SetErrorMessage("Motor is OFF");
		SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);

		return FALSE;
	}

	// up gripper level and gripper state open.
	SeparateClamp_BT();
	// down bin table level, on vacuum, on realign
	if (GetCurrBinOnBT() != 0) 
	{
		SetFrameAlign(TRUE);
		Sleep(m_lBTAlignFrameDelay);
		SetFrameVacuum(TRUE);
	}
	else	//release the clamp if need to load frame
	{
		SetFrameAlign(FALSE);
	}

	// move gripper to ready
	//v2.93T2
	if (m_bIsExArmGripperAtUpPosn)
	{
		SetGripperLevel(FALSE);
	}
	m_bIsExArmGripperAtUpPosn = FALSE;

	if (BinGripperMoveTo(m_lReadyPos_X) != TRUE)
	{
		return FALSE;
	}

	//v2.93T2
	if (m_bIsExChgArmExist)
	{
		BOOL bWait = FALSE;

		if (m_fHardware && !m_bDisableBL)		//v3.60
		{
			//if (!m_piFrontArmReady->IsLow())
			if (CMS896AStn::MotionReadInputBit(BL_SI_FArmReady))
			{
				SetFontArmLevel(FALSE);
				bWait = TRUE;
			}
			//if (!m_piRearArmReady->IsLow())
			if (CMS896AStn::MotionReadInputBit(BL_SI_RArmReady))
			{
				SetRearArmLevel(FALSE);
				bWait = TRUE;
			}
		}

		if (bWait)
		{
			Sleep(500);
			//if (m_piFrontArmReady->IsLow() && m_piRearArmReady->IsLow())
			if (!CMS896AStn::MotionReadInputBit(BL_SI_FArmReady) && !CMS896AStn::MotionReadInputBit(BL_SI_RArmReady))
			{
				Arm_MoveTo(m_lExArmReadyPos);
			}
		}
		else
		{
			Arm_MoveTo(m_lExArmReadyPos);
		}
	}

	//Check gripper pos before start
	if (IsMagazineSafeToMove() == FALSE)
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		return FALSE;
	}

	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}

	//v2.93T2
	/*
	// down elevator to ready
	if ( DownElevatorToReady_Auto() == FALSE )
	{
		HouseKeeping(m_lReadyPos_X);
		return FALSE;
	}*/

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//	UnloadBinFrame()
//		bBinFull		: UNLOAD to FULL magazine
//		bClearAllMode	: used to keep BT at UNLOAD-pos after UNLOAD for sub-sequent LOAD operation (for offline mode only)
//		bClearBin		: specify whether or not to clear bin counter when bBInFUll=TRUE (for MS100 9Inch UNLOAD)	//v4.21
///////////////////////////////////////////////////////////////////////////////////
LONG CBinLoader::UnloadBinFrame(BOOL bBurnInUnload, CBinGripperBuffer *pGripperBuffer, BOOL bBinFull, BOOL bOffline, BOOL bClearAllMode, 
								BOOL bClearBin, BOOL bCheckRealignRpy, BOOL bTransferWIPSlot)
{
	LONG lRet = 0;
	BOOL bClearAllFrameTOP2Mode = IsMSAutoLineMode() && bClearAllMode;

	//if bClearAllFrameTOP2Mode is TRUE, machine transfers the frame from WIP to TOP2 under inline-mode
	if (!bClearAllFrameTOP2Mode)
	{
		if (!LoadUnloadLockMutex())
		{
			CString szLog;

			szLog.Format("LoadUnloadLockMutex  UnloadBinFrame  = %ld", m_lLoadUnloadSECSCmdObject);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}

		BL_DEBUGBOX("UnloadBinFrame CheckClosedALBackGate");
		lRet = CheckClosedALBackGate();
		if (lRet)
		{
			LoadUnloadUnlock();
			return lRet;
		}
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//DL with UpDown Buffer Table
	{
		lRet = UDB_SubUnloadBinFrame(bBurnInUnload, pGripperBuffer, bBinFull, bOffline, 
									 bClearBin, bCheckRealignRpy, bTransferWIPSlot, bClearAllFrameTOP2Mode);
	}
	else
	{
//		lRet = SubUnloadBinFrame(bBurnInUnload, bBinFull, bOffline, bClearAllMode, 
//								 bClearBin, bCheckRealignRpy, bTransferWIPSlot);
	}

	if (!bClearAllFrameTOP2Mode)
	{
		LONG lRet1 = CloseALFrontGate();
	
		if (lRet == TRUE)
		{
			lRet = lRet1;
		}

		LoadUnloadUnlock();
	}
	return lRet;
}

/*
LONG CBinLoader::SubUnloadBinFrame(BOOL bBurnInUnload, BOOL bBinFull, BOOL bOffline, BOOL bClearAllMode, 
								BOOL bClearBin, BOOL bCheckRealignRpy, BOOL bTransferWIPSlot)
{
	ULONG i;
	LONG lFullStatus = 1;
	LONG lMgzn = 0, lSlot = 0, lFrameOnBT;
	LONG lStatus = 0;
	BOOL bModeDError;
	BOOL bScanBarcode = TRUE;
	BOOL bWaitElevatorComplete = FALSE;	//!m_bIsExChgArmExist;
	CString str;
	CString szText1, szText2, szMsg;
	INT nMESReply = 0;
	CString szBinBlkGrade = "";
	BOOL lBlockNo = 0;
	BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];	//v4.39T7
	BOOL bEnableCopyTempFile = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["Copy Temp File Only"]; //v4.44T6


	//Check Gripper position is valid
	if (IsMagazineSafeToMove() == FALSE)
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);

		return FALSE;
	}

	if (IsBondArmSafe() == FALSE)	
	{
		BL_DEBUGBOX("IsBondArmSafe False");
		return FALSE;
	}

	bModeDError = FALSE;
	if (m_lOMRT == BL_MODE_B)
	{
		bBinFull = TRUE;
	}

	// check frame on bin table logically
	if (!(IsAutoLoadUnloadQCTest() || IsAutoLoadUnloadSISTest()) && (GetCurrBinOnBT() == 0)) // logically
	{
		SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_FRAME);
		SetErrorMessage("BL Current logical block is zero");
		BL_DEBUGBOX("BL Current logical block is zero");
		return FALSE;
	}

	if (CMS896AStn::m_bCEMark)	//v4.50A23
	{
		bWaitElevatorComplete = TRUE;
	}

	// check frame on bin table physically
	lFrameOnBT = CheckFrameOnBinTable();

	if ((bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL))		//v3.60
	{
		lFrameOnBT = BL_FRAME_ON_CENTER;
	}
	str.Format("**** Unload lFrameOnBT = %ld ****", lFrameOnBT);
	BL_DEBUGBOX(str);

	switch (lFrameOnBT)
	{
		case BL_FRAME_NOT_EXIST: 
			SetFrameVacuum(FALSE);
			SetFrameAlign(FALSE);

			if (bOffline == TRUE)
			{
				if (BL_YES_NO(HMB_BL_PUT_FRAME_INTO_BT, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
				{
					if (BL_YES_NO(HMB_BL_UNLOAD_MANUALLY, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
					{
						return FALSE;
					}
					else
					{
						SetAlert_Red_Yellow(IDS_BL_MAN_UNLOAD);
						SetErrorMessage("BL manual unload this frame");
						return TRUE;
					}
				}
				else
				{
					SetFrameAlign(TRUE);
					Sleep(m_lBTAlignFrameDelay);
		
					SetFrameVacuum(TRUE);
				}
			}
			else
			{
				return FALSE;
			}
			break;

		case BL_FRAME_ON_RHS_EDGE: 
			SetFrameVacuum(FALSE);
			SetFrameAlign(FALSE);
			SetBHFrontCoverLock(TRUE);		//v4.50A30
			//if( BL_OK_CANCEL("Frame exists on bintable right edge only!\nPlease push it back", "Unload Frame Message")==FALSE )
			if (BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_RHS, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
			{
				return FALSE;
			}

			SetBHFrontCoverLock(FALSE);		//v4.50A30
			SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);
			SetFrameVacuum(TRUE);
			break;

		case BL_FRAME_ON_LHS_EDGE: 
			SetFrameVacuum(FALSE);
			SetFrameAlign(FALSE);
			SetBHFrontCoverLock(TRUE);		//v4.50A30
			//if( BL_OK_CANCEL("Frame exists on bintable left edge only!\nPlease push it back", "Unload Frame Message")==FALSE )
			if (BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_LHS, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
			{
				return FALSE;
			}

			SetBHFrontCoverLock(FALSE);		//v4.50A30
			SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);

			SetFrameVacuum(TRUE);
			break;

		case BL_FRAME_ON_CENTER: 
			break;
	}

	lBlockNo = GetCurrBinOnBT();
	
	if (bBinFull)
	{
		str.Format("**** Unload #%ld (FULL) start ****", lBlockNo);
	}
	else
	{
		str.Format("**** Unload #%ld start ****", lBlockNo);
	}
	BL_DEBUGBOX(str);

	//Update m_lCurrMgzn, m_lCurrSlot for the frame on bin table for Dual Buffer
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		BOOL bUseEmpty;

		lFullStatus = GetLoadMgzSlot(bUseEmpty, lBlockNo, FALSE, TRUE);	//v4.34T1	//YEaly	(new SKIP option added)

		if (lFullStatus == FALSE)
		{
			CString szMsg;
			szMsg.Format("\n%d", lBlockNo);
			SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
			SetErrorMessage("BL No slot is assigned to this physical block");
			return FALSE;
		}
	}


	LONG lOldMgzn = m_lCurrMgzn;
	LONG lOldSlot = m_lCurrSlot;
	CString szBCName = GetCassetteSlotBCName(m_lCurrMgzn, m_lCurrSlot);

	if ((bBinFull == TRUE) || bTransferWIPSlot)
	{
		lMgzn = m_lCurrMgzn;
		lSlot = m_lCurrSlot;
		str.Format("Unload filmframe current M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		lFullStatus = GetFullMgznSlot(lMgzn, lSlot, FALSE, lBlockNo);	//v4.59A20

		if (lFullStatus == FALSE)
		{
			return FALSE;
		}

		str.Format("Unload filmframe to full M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
	}
	else
	{
		//v4.59A11
		//If current frame was pulled out from INPUT slot (Slot #1 - #5),
		// then need to assign a new WIP slot (Slot #11 - 30) for UNLOAD;
		if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
		{
			LONG lUnloadMgzn = m_lCurrMgzn;
			LONG lUnloadSlot = m_lCurrSlot;
		
			GetUnloadMgznSlot_AutoLine(szBCName, lBlockNo, lUnloadMgzn, lUnloadSlot);

			m_lCurrMgzn	= lUnloadMgzn;
			m_lCurrSlot = lUnloadSlot;
		}
		
		lMgzn = m_lCurrMgzn;
		lSlot = m_lCurrSlot;
		str.Format("Unload filmframe to M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
	}

	BL_DEBUGBOX(str);
	BL_DEBUGMESSAGE(str);


	BL_DEBUGBOX("gripper to preunload");
	if (BinGripperMoveTo_Auto(m_lPreUnloadPos_X, SFM_NOWAIT) != TRUE)
	{
		return FALSE;
	}

	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}

	//v4.37T3
	BL_DEBUGBOX("BT to unload");
	if (bOffline && (IsFrameLevel() == FALSE))
	{
		SetFrameLevel(FALSE);	// down bin table first if necessary
		Sleep(500);
	}

	//Make sure last elevator motion is complete before UNLOAD
	Z_Sync();
	//Y_Sync();

	//Always complete Z motion in DL (not for DLA)
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bNewMagCheck = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_NEW_BL_MAGEXIST_CHECKING);

	if (bNewMagCheck)	//v3.57
	{
		BL_DEBUGBOX("New MagExist sensor checking");
		str.Format("Unload filmframe1 to M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		//New Mag Exist sensor checking sequence as required by production
		//if ( MoveElevatorToUnload(lMgzn, lSlot, FALSE, bWaitElevatorComplete) == FALSE )	
	}
	else
	{
		str.Format("Unload filmframe2 to M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
	}

	BL_DEBUGBOX(str);
	if (MoveElevatorToUnload(lMgzn, lSlot, TRUE, bWaitElevatorComplete) == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}


	if (MoveWaferTableToSafePosn(TRUE, bOffline) == FALSE)		//v3.61	BBB
	{
		return FALSE;
	}

#ifdef NU_MOTION
	if (!bOffline && bCheckRealignRpy)	//v4.37T5
	{
		BOOL bRealign = RealignBinFrameRpy(TRUE, FALSE);
		if (bRealign == FALSE)			//v4.38T2
		{
			return FALSE;
		}
	}
	else
	{
		if (MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, 1, TRUE) == FALSE)		//v4.22T7
		{
			return FALSE;
		}
	}
#else
	if (MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y) == FALSE)					//v4.23T2
	{
		return FALSE;
	}
#endif

	lMgzn = lOldMgzn;
	lSlot = lOldSlot;
	//Get the Magzine & Slot Number
	if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())
	{
		CBinGripperBuffer *pGripperBuffer = &m_clUpperGripperBuffer;
		pGripperBuffer->SetUnloadDieGradeCount(GetNoOfSortedDie((UCHAR)lBlockNo));
		if ((bBinFull == TRUE) || bTransferWIPSlot)
		{
			lFullStatus = GetFullMgznSlotWithUpdate_AutoLine(pGripperBuffer->GetUnloadDieGradeCount(), bTransferWIPSlot, lMgzn, lSlot);
			CString szLog;
			if (lFullStatus == FALSE)
			{
				szLog.Format("UNLOAD: cannot find a FULL slot for #%d", lBlockNo);
				HmiMessage_Red_Back(szLog);		
				SetErrorMessage(szLog);
				return FALSE;
			}
	
			szLog.Format("UNLOAD filmframe to FULL - M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
			BL_DEBUGBOX(szLog);	
		}
		else
		{
			GetUnloadMgznSlotWithUpdate_AutoLine(szBCName, lBlockNo, pGripperBuffer->GetUnloadDieGradeCount(), lMgzn, lSlot);
		}
	}

	//v2.71
	//New barcode scanning method on bin table
	if (m_bFastBcScanMethod)
	{
		BL_DEBUGBOX("m_bFastBcScanMethod = 1");
		bScanBarcode = m_bUseBarcode;
		if (bScanBarcode == TRUE)
		{
			if ((bBinFull == FALSE) && (bOffline == FALSE) && (m_bStopChgGradeScan == TRUE))
			{
				bScanBarcode = FALSE;
			}

			BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];
			if (bEnableBtT && !IsMS90())		//v4.39T7	//Nichia MS100+		//v4.50A25
			{
				//With Bt T the barcode scanning will be performed on table instead of gripper;
				//so here the barcode scanning on gripper is disabled
				bScanBarcode = FALSE;
			}
		}

		if (bScanBarcode)
		{
			if (bScanBarcode == TRUE)
			{
				BL_DEBUGBOX("scan barcode on table");
				lStatus = ScanningBarcodeOnBufferTable();
				if (lStatus == FALSE)
				{
					BL_DEBUGBOX("#1 No barcode read");
				}
				else if (lStatus == -1)
				{
					return FALSE;
				}
			}

			if (bScanBarcode == TRUE)
			{
				//BL_DEBUGBOX("compare barcode");
				//pllm
				CString szSlotBarcode = IsMSAutoLineMode() ? GetCassetteSlotBCName(lMgzn, lSlot) : GetCurrMgznSlotBC();
				BL_DEBUGBOX("Unload frame fast mode compare barcode :  READ = " + m_szBCName + "; SLOT = " + szSlotBarcode);

//				if (IsMSAutoLineMode())
//				{		
//					if (m_szBCName != szSlotBarcode)
//					{
//						SetAlert_Red_Yellow(IDS_BL_NOT_MATCHED_SIS_BARCODE);
//
//						// set as reject status for the current slot
//						SetSlotRejectStatus(lMgzn, lSlot);
//
//						BinGripperMoveTo(m_lUnloadPos_X);
//						SetGripperState(FALSE);
//						Sleep(500);
//						BinGripperMoveTo(m_lPreUnloadPos_X);
//						SetFrameLevel(FALSE);
//						SetGripperLevel(FALSE);
//						Sleep(500);
//						AlignBinFrame(); 
//
//						HouseKeeping(m_lPreUnloadPos_X);
//						return FALSE;
//					}
//				}

				//v4.42T9	//Cree
				if (m_bCheckBarcode)
				{
					CString szErrCode;
					BOOL bBCHistory = CheckBcHistoryInCurrLot(GetCurrBinOnBT(), m_szBCName, FALSE, szErrCode);
					if (bBurnInUnload)	//v4.20
					{
						bBCHistory = TRUE;
					}
					if (!bBCHistory)
					{
						CString szErr = szErrCode;
						HmiMessage_Red_Yellow(szErr);		
						BL_DEBUGBOX(szErr);	

						BinGripperMoveTo(m_lUnloadPos_X);
						SetGripperState(FALSE);
						Sleep(500);
						BinGripperMoveTo(m_lPreUnloadPos_X);
						SetFrameLevel(FALSE);
						SetGripperLevel(FALSE);
						Sleep(500);
						AlignBinFrame(); 

						HouseKeeping(m_lPreUnloadPos_X);
						//m_szBCName = GetCurrMgznSlotBC();
						//SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
						return FALSE;
					}
				}

				if (bBurnInUnload == TRUE)
				{
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
					SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
				}
				else
				{
					if (m_bCompareBarcode == FALSE)
					{
						//Only update Barcode name if this is not Default
						if (m_szBCName != BL_DEFAULT_BARCODE)
						{
							m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
							SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
						}
						
						if ((bBinFull == TRUE) && (GetCurrMgznSlotBC() == BL_DEFAULT_BARCODE) && (m_szBCName == BL_DEFAULT_BARCODE))
						{
							CString szText = "\n";
							AlertMsgForManualInputBarcode(GetCurrBinOnBT(), m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
						}
					}
					else
					{
						CString szSlotBC = GetCurrMgznSlotBC();

						if (GetCurrMgznSlotBC() != m_szBCName)
						{
							CString szText;	
							LONG lOption;

							//v3.71T3	//PLLM special feature
							CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
							BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
							ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
							//if (nPLLM == PLLM_LUMIRAMIC)
							if (pApp->GetProductLine() == "Lumiramic")		//v4.35T1
							{
								bPLLMSpecialFcn = TRUE;    //Lumiramic Dual-DLA also uses REBEL sequence!!		//v4.02T4
							}		

							if (pApp->IsBLBCUseOldGoStop())
							{
								bPLLMSpecialFcn = TRUE;
							}

							szText = "#5 Unload Bin Frame\n";
							szText += "Old: [@@" + GetCurrMgznSlotBC() + "@@]\nNew: [@@" + m_szBCName + "@@]";

							if (bPLLMSpecialFcn)	
							{
								if ((m_szBCName == "") || (m_szBCName == BL_DEFAULT_BARCODE))	//v3.71T7
								{
									szText = "Output frame barcode cannot be read!\n";
									szText += "old label: " + GetCurrMgznSlotBC();
								}
								else
								{
									szText = "Barcode read on output frame is different from\n";
									szText += "old label: " + GetCurrMgznSlotBC();
									
								}
								lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "CONTINUE", "STOP");
							}
							else if (m_bSemitekBLMode)	//v4.42T7
							{
								szText = "Barcode read on output frame is different from\n";
								szText += "old label: " + GetCurrMgznSlotBC();
								SetErrorMessage("Semitek: " + szText);
								BL_DEBUGBOX("Semitek: " + szText);
								lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Stop", "", NULL, glHMI_ALIGN_LEFT);		
								lOption = -1;	//1;	//Use OLD	//v4.42T9
								bModeDError = TRUE;		//Used to return FALSE in UnloadBinFrame() at the end
							
								//v4.42T9
								SetGripperState(FALSE);
								Sleep(500);
							}
							else
							{
								CString szMsg;
								szMsg = szText;
								szMsg.Replace('\n', ' ');
								SetStatusMessage(szMsg);
								lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Old", "New", NULL, glHMI_ALIGN_LEFT);
							}

							BL_DEBUGBOX(szText);
							switch (lOption)
							{
								case -1:
									BL_DEBUGBOX("LOAD: barcode comparison fails -> abort");	
									SetStatusMessage("DLUL Compare barcode fail: Abort");
									HouseKeeping(m_lPreUnloadPos_X);
									m_szBCName = GetCurrMgznSlotBC();
									SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
									return FALSE;

								case 1:		//OLD or CONTINUE
									
									BL_DEBUGBOX("LOAD: barcode comparison fails -> use old " + GetCurrMgznSlotBC());	
									m_szBCName = GetCurrMgznSlotBC();
									
									if (bPLLMSpecialFcn)	//v3.70T2
									{
										SaveBarcodeData(GetCurrBinOnBT(), GetCurrMgznSlotBC());
									}
									else
									{
										SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
										SetStatusMessage("DLUL Compare barcode fail: Use old barcode -> " + m_szBCName);
									}

									break;

								case 2:		//NEW or STOP
									BL_DEBUGBOX("LOAD: barcode comparison fails ABORT -> use new ");	
									if (bPLLMSpecialFcn)	//v3.70T2
									{
										SaveBarcodeData(GetCurrBinOnBT(), GetCurrMgznSlotBC());
									}
									else
									{
										m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
										SetStatusMessage("DLUL Compare barcode fail: Use new barcode -> " + m_szBCName);
										SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
									}
									break;
							}
						}
						else if ((bBinFull == TRUE) && (GetCurrMgznSlotBC() == BL_DEFAULT_BARCODE) && (m_szBCName == BL_DEFAULT_BARCODE))
						{
							//If unload to full & previous barcode & current barcode == BL_DEFAULT_BARCODE, prompt message to user
							CString szText = "\n";
							AlertMsgForManualInputBarcode(GetCurrBinOnBT(), m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
						}
					}
				}
			}
		}
	}

	//===================================================================
	BL_DEBUGBOX("gripper move to unload");
	if (BinGripperSynMove_Auto() == FALSE)
	{
		return FALSE;
	}

	if (bWaitElevatorComplete == FALSE)
	{
		YZ_Sync_OutOfMagChecking();		//v4.24
	}

	if (bNewMagCheck)	//v3.57	//New MagExist sensor checking sequence as required by production
	{
		if ((bBurnInUnload == FALSE) && (IsMagazineExist(lMgzn) == FALSE))
		{
			Sleep(200);
			if (IsMagazineExist(lMgzn, FALSE, TRUE) == FALSE)	//v4.41T2
			{
				CString szLog;
				szLog.Format("Magazine #%d not exist in new MagExist checking", lMgzn);
				BL_DEBUGBOX(szLog);
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
			}
		}

		if (MoveElevatorToUnload(lMgzn, lSlot, TRUE, TRUE) == FALSE)	//Use offsetYZ & Wait MotionCompleted
		{
			BL_DEBUGBOX("Error in elevator to UNLOAD + Offset-YZ in new MagExist checking");
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}
	else
	{
		if ((bBurnInUnload == FALSE) && (IsMagazineExist(lMgzn) == FALSE))
		{
			Sleep(200);
			if (IsMagazineExist(lMgzn, FALSE, TRUE) == FALSE)	//v4.41T2
			{
				CString szLog;
				szLog.Format("Magazine #%d not exist", lMgzn);
				BL_DEBUGBOX(szLog);
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
			}
		}
	}
	
	BL_DEBUGBOX("SettleDown for unload");
	SettleDownBeforeUnload(bOffline);
	

	//v2.71
	LONG lOffsetX = m_lUnloadOffsetX;

	//20170829 Leo Settle Down 300ms more for WolfSpeed Ivan Cheung Request
	if( (pApp->GetCustomerName()=="WolfSpeed") )
	{
		SettleDownBeforeUnload(bOffline);
		CString szLog;
		//20170829 Leo add log to check
		szLog.Format("WolfSpeed Settle 2nd Time, bOffline=%ld, OffsetX=%ld", bOffline, lOffsetX);
		BL_DEBUGBOX(szLog);
	}

	if (lOffsetX > 0)
	{
		if (BinGripperMoveTo_Auto((m_lUnloadPos_X - lOffsetX), SFM_WAIT) != TRUE)
		{
			return FALSE;
		}
	}

	if (IsFrameLevel() != FALSE)		//v4.47T12	//Osram buyoff
	{
		Sleep(1000);
	}

	BL_DEBUGBOX("Searching frame");
	LONG lReturn = TRUE;
	//if (BinGripperSearchInClamp(HP_POSITIVE_DIR, abs(m_lUnloadPos_X-m_lPreUnloadPos_X), 160)==FALSE )
	if (lOffsetX > 0)
	{
		lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, lOffsetX + 100, 160);
	}
	else
	{
		BinGripperMoveTo(m_lUnloadPos_X);
		if (IsFrameInClamp())
		{
			lReturn = TRUE;
		}
		else
		{
			Sleep(100);
			lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, 300, 80);
		}
	}

	if (!lReturn)
	{
		//if( bBurnInUnload==FALSE && BL_OK_CANCEL("Please pull frame into gripper", "Unload Frame Message")==FALSE )
		if ((bBurnInUnload == FALSE) && (!m_bDisableBL))
		{
			//v3.68T4	//Modified for Ubiliux & KH Lee
			BOOL bSensorCheck = FALSE;
			do 
			{
				if (BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
				{
					//v4.50A5	//PLLM
					SetErrorMessage("BL Gripper1 search frame fails at UNLOAD");
					BL_DEBUGBOX("BL Gripper1 search frame fails at UNLOAD\n");
					if (!bOffline && bBinFull)
					{
						//Must reset index because already clear-Bin in AUTOBOND mode
						m_lBTCurrentBlock = 0;	//v4.50A5
					}

					HouseKeeping(m_lPreUnloadPos_X);
					return FALSE;
				}

				bSensorCheck = IsFrameInClamp() && !IsFrameJam();
				if (bSensorCheck)
				{
					break;
				}

			} 
			while (1);
		}
	}

	// Updated to open the bin align after complete search frame
	Sleep(200);
	SetGripperState(TRUE);
	Sleep(100);
	SetFrameAlign(FALSE);

	if (!bOffline && bCheckRealignRpy)	//v4.37T5
	{
		BOOL bRealign = RealignBinFrameRpy(TRUE, FALSE);
		if (!bRealign)
		{
			SetErrorMessage("BL UnloadBinFrame REALIGN fail 1");
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}

	if (m_lBTAlignFrameDelay > 200)
	{
		Sleep(m_lBTAlignFrameDelay);
	}
	else
	{
		Sleep(200);
	}
	
	//Original barcode scanning method on gripper
	if (!m_bFastBcScanMethod)
	{
		BL_DEBUGBOX("m_bFastBcScanMethod = 0");

		//Determine to use barcode or not
		bScanBarcode = m_bUseBarcode;

		if (bScanBarcode == TRUE)
		{
			if ((bBinFull == FALSE) && (bOffline == FALSE) && (m_bStopChgGradeScan == TRUE))
			{
				bScanBarcode = FALSE;
			}

			//if (bBurnInUnload)			//v4.13T6	//v4.20
			//	bScanBarcode = FALSE;

			BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];
			if (bEnableBtT && !IsMS90())		//v4.39T7	//Nichia MS100+
			{
				//With Bt T the barcode scanning will be performed on table instead of gripper;
				//so here the barcode scanning on gripper is disabled
				bScanBarcode = FALSE;
			}
		}

		//if (m_bUseBarcode == TRUE)
		if (bScanBarcode == TRUE)
		{
			BL_DEBUGBOX("#1 scan barcode");

#ifndef NU_MOTION
			//if( BinGripperSearchJam(HP_POSITIVE_DIR, m_lBarcodePos_X, 800)==FALSE )
			//pllm	//Speed up for dual-loader config
			LONG lBcStatus = TRUE;
			if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
			{
				lBcStatus = BinGripperMoveSearchJam(m_lBarcodePos_X);
			}
			else
			{
				lBcStatus = BinGripperSearchJam(HP_POSITIVE_DIR, m_lBarcodePos_X, 800);
			}
			if (lBcStatus == FALSE)
#else
			if (BinGripperMoveSearchJam(m_lBarcodePos_X, bCheckRealignRpy, TRUE) == FALSE)	//v4.02T6
#endif
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				BL_DEBUGBOX("Gripper is jammed");
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
			}

			//v4.02T6	//barcode prescan fcn in BinGripperMoveSearchJam() above
			if ( (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER) &&		//v4.21T3
					!m_szDBPreScanBCName.IsEmpty() )
			{
				m_szBCName	= m_szDBPreScanBCName;
				m_szDBPreScanBCName = _T("");
				lStatus		= TRUE;
				BL_DEBUGBOX("from PreScan barcode " + m_szBCName);
			}
			else
			{
				CBinGripperBuffer *pGripperBuffer = &m_clUpperGripperBuffer;
				lStatus = ScanningBarcode(pGripperBuffer);
				BL_DEBUGBOX("Scan barcode " + m_szBCName);
				if (lStatus == FALSE)
				{
					BL_DEBUGBOX("#2 No barcode read");
				}
				else if (lStatus == -1)
				{
					return FALSE;
				}
			}
		}

		if (bScanBarcode == TRUE)
		{
			CString szSlotBarcode = IsMSAutoLineMode() ? GetCassetteSlotBCName(lMgzn, lSlot) : GetCurrMgznSlotBC();
			BL_DEBUGBOX("Unload frame compare barcode :  READ = " + m_szBCName + "; SLOT = " + szSlotBarcode);

//			if (IsMSAutoLineMode())
//			{		
//				if (m_szBCName != szSlotBarcode)
//				{
//					SetAlert_Red_Yellow(IDS_BL_NOT_MATCHED_SIS_BARCODE);
//
//					// set as reject status for the current slot
//					SetSlotRejectStatus(lMgzn, lSlot);
//
//					BinGripperMoveTo(m_lUnloadPos_X);
//					SetGripperState(FALSE);
//					Sleep(500);
//					BinGripperMoveTo(m_lPreUnloadPos_X);
//					SetFrameLevel(FALSE);
//					SetGripperLevel(FALSE);
//					Sleep(500);
//					AlignBinFrame(); 
//
//					HouseKeeping(m_lPreUnloadPos_X);
//					return FALSE;
//				}
//			}

			//v4.42T9	//Cree
			if (m_bCheckBarcode)
			{
				BL_DEBUGBOX("Check barcode (UNLOAD)");

				CString szErrCode;
				BOOL bBCHistory = CheckBcHistoryInCurrLot(GetCurrBinOnBT(), m_szBCName, FALSE, szErrCode);
				if (bBurnInUnload)	//v4.20
				{
					bBCHistory = TRUE;
				}
				if (!bBCHistory)
				{
					CString szErr = szErrCode;
					HmiMessage_Red_Yellow(szErr);		
					BL_DEBUGBOX(szErr);	

					BinGripperMoveTo(m_lUnloadPos_X);
					SetGripperState(FALSE);
					Sleep(500);
					BinGripperMoveTo(m_lPreUnloadPos_X);
					SetFrameLevel(FALSE);
					SetGripperLevel(FALSE);
					Sleep(500);
					AlignBinFrame(); 

					HouseKeeping(m_lPreUnloadPos_X);
					//m_szBCName = GetCurrMgznSlotBC();
					//SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
					return FALSE;
				}

				//v4.42T15
				BOOL bCheckMgznBC	= BL_CheckBcInMgzs(GetCurrBinOnBT(), m_szBCName, bBurnInUnload);
				if (bBurnInUnload)
				{
					bCheckMgznBC	= TRUE;
					//bValidateBC	= TRUE;
				}
				if (!bCheckMgznBC)	// || !bValidateBC)
				{
					BinGripperMoveTo(m_lUnloadPos_X);
					SetGripperState(FALSE);
					Sleep(500);
					BinGripperMoveTo(m_lPreUnloadPos_X);
					SetFrameLevel(FALSE);
					SetGripperLevel(FALSE);
					Sleep(500);
					AlignBinFrame(); 

					HouseKeeping(m_lPreUnloadPos_X);
					return FALSE;
				}
			}

			if (m_bCompareBarcode == FALSE)
			{
				BL_DEBUGBOX("No Compare barcode (UNLOAD)");
				//Only update Barcode name if this is not Default
				if (m_szBCName != BL_DEFAULT_BARCODE)
				{
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
					SaveBarcodeData(GetCurrBinOnBT(), m_szBCName, m_lCurrMgzn, m_lCurrSlot);
				}
				if ((bBinFull == TRUE) && (GetCurrMgznSlotBC() == BL_DEFAULT_BARCODE) && (m_szBCName == BL_DEFAULT_BARCODE))
				{
					CString szText = "\n";
					AlertMsgForManualInputBarcode(GetCurrBinOnBT(), m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
				}
			}
			else
			{
				BL_DEBUGBOX("Compare barcode (UNLOAD)");

				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				BOOL bSanAnWHSpecialFcn = FALSE;
				BOOL bCheckBCMessyCode	= FALSE;
				
				if(pApp->GetProductLine() == "WH")
				{
					bSanAnWHSpecialFcn = TRUE;
				}

				CString szSlotBC = GetCurrMgznSlotBC();
				if (m_bSemitekBLMode && (szSlotBC != m_szBCName))
				{
					if ((szSlotBC.IsEmpty() == FALSE) && (m_szBCName.IsEmpty() == FALSE) &&
							(szSlotBC == BL_DEFAULT_BARCODE || m_szBCName == BL_DEFAULT_BARCODE))
					{
						BL_DEBUGBOX("CPBC BL2 UnLoad force BC= " + m_szBCName + " slot= " + szSlotBC);
						if (szSlotBC == BL_DEFAULT_BARCODE)
						{
							m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
						}
						if (m_szBCName == BL_DEFAULT_BARCODE)
						{
							if( bSanAnWHSpecialFcn )
							{
								//Rescan again below
								HmiMessage_Red_Yellow("Compare Barocde: Currently No barocde read.");
							}
							else
							{
							    m_szBCName = szSlotBC;
						    }
							
						}
						BL_DEBUGBOX("CPBC BL2 UnLoad after BC " + m_szBCName);
					}
				}

				//v4.21T3	//TongFang //Re-scan barcode if not the same as old one
				CBinGripperBuffer *pGripperBuffer = &m_clUpperGripperBuffer;
				if (!GripperRescanningUnloadBarCode(pGripperBuffer))
				{
					return FALSE;
				}

				if (!CheckBarcodeCharacters(m_szBCName))
				{
					bCheckBCMessyCode = TRUE;
					BL_DEBUGBOX("Rescan barcode value is messy code = " + m_szBCName);
				}

				if( m_szBCName == BL_DEFAULT_BARCODE || m_szBCName == "" || bCheckBCMessyCode)
				{
					if( bSanAnWHSpecialFcn )//4.55T07
					{
						CString szText = "\n";
						//AlertMsgForManualInputBarcode(GetCurrBinOnBT(), m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
						AlertMsgForManualInputBarcode_NoSaveToRecord(m_szBCName, szText);
						BL_DEBUGBOX("Mannual input barcode done - new = " + m_szBCName);
					}
				}

				if (!CheckUnloadBarCodeValid(pGripperBuffer, bBinFull))
				{
					return FALSE;
				}
			}
		}
	}

	//pllm	//Release gripper before moving
	if ( (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER) && (!bEnableBtT || IsMS90()) )	//v4.50A25
	{
		BL_DEBUGBOX("Gripper OFF to unload frame");
		SetGripperState(FALSE);
		Sleep(100);
	}
	else if (bEnableBtT)		//v4.39T7
	{
		BL_DEBUGBOX("Gripper still ON to unload frame");
	}

	BOOL bUnloadJam = FALSE;	//v3.77
	if (BinGripperMoveSearchJam(m_lLoadMagPos_X) == FALSE)
	{
		bUnloadJam = TRUE;
		SetGripperState(FALSE);
		//if( bBurnInUnload==FALSE && BL_OK_CANCEL("Frame is jammed!\nPlease push it into the magazine slot", "Unload Frame Message")==FALSE )

		//v4.45T2
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
		{
			(*m_psmfSRam)["BinLoaderStn"]["GripperJam"] = TRUE;
		}

		BOOL bOk = FALSE;
		if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
		{
			bOk = BinLoaderUnloadJamMessage_DB(&m_clUpperGripperBuffer, bBurnInUnload);
		}
		else
		{
			bOk = BinLoaderUnloadJamMessage(bBurnInUnload);
		}

		//v4.45T2
		if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
		{
			bOk = FALSE;
		}

		if (bBurnInUnload == FALSE && !m_bDisableBL && bOk == FALSE)
		{
			HouseKeeping(m_lPreUnloadPos_X);

			//shiraishi01
			if (bBinFull == TRUE)
			{
				switch (m_lOMRT)
				{
					case BL_MODE_A:
					case BL_MODE_F:		//MS100 8mag config	
						m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = GetCurrBinOnBT();
						m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
						if (IsMSAutoLineMode())		//v4.56A11
						{
							m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_FULL;
						}
						else
						{
							m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
							m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
						}
						break;
				}
			}

			m_lBTCurrentBlock = 0;
			m_szBinFrameBarcode = "";
			return FALSE;
		}
		else
		{
			//v3.95T1
			BL_DEBUGBOX("x HOME because of JAM");
			X_Home();
		}
	}

	Sleep(200);
	SetGripperState(FALSE);
	Sleep(200);

	//v4.41T1	//PLLM MS109
	LONG lOldBlock = GetCurrBinOnBT();
//	if (IsMSAutoLineMode() && bTransferWIPSlot)
//	{
//		lOldBlock = GetCassetteSlotGradeBlock(m_lCurrMgzn, m_lCurrSlot);
//	}

	m_lBTCurrentBlock = 0;

	m_szBinFrameBarcode = "";	//Reset BC1 on menu after UNLOAD	//v4.37T5

	if (!bOffline && bCheckRealignRpy)	//v4.37T5
	{
		BOOL bRealign = RealignBinFrameRpy(FALSE, FALSE);
		if (!bRealign)
		{
			SetErrorMessage("BL UnloadBinFrame REALIGN fail 2");
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}

	BL_DEBUGBOX("x ready, down magazine");
	if (CheckElevatorCover() == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	if (MoveElevatorToUnload(lMgzn, lSlot, FALSE, TRUE, TRUE) == FALSE)	//v3.94T5	//Must wait Z motion complete, otherwise gripper will crashes with elevator
	{
		BL_DEBUGBOX("unload frame, move elevator to unload false");
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	if (bUnloadJam)		//v3.77
	{
		BL_DEBUGBOX("x HOME because of JAM 2");
		X_Home();
		Sleep(200);
	}

	if (BinGripperMoveTo(m_lReadyPos_X) != TRUE)
	{
		HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		return FALSE;
	}


	// TO check out of mgz after unload and gripper move away
	if (IsFrameOutOfMgz())	// && (m_bBurnInEnable == FALSE) )
	{
		Sleep(10);
		if ((IsFrameOutOfMgz() == TRUE))
		{
			SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
			SetErrorMessage("Move Elevator To Unload, frame out of magazine after gripper at ready");
			BL_DEBUGBOX("Move Elevator To Unload, frame out of magazine after gripper at ready");
			HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
			return FALSE;
		}
	}

	if (bOffline == TRUE)
	{
		SetGripperLevel(FALSE);
		SetFrameLevel(FALSE);
		Sleep(1000);

		if (bClearAllMode == FALSE)
		{
			if (MoveBinTable(0, 0) == FALSE)
			{
				BL_DEBUGBOX("unload frame, move bin table to 0,0 error, cut limit sensor?");
				return FALSE;
			}
		}
	}
	else
	{
	}


	if (MoveWaferTableToSafePosn(FALSE, bOffline) == FALSE)		//v3.61	BBB
	{
		BL_DEBUGBOX("unload frame, move wft to safe error");
		return FALSE;
	}

	if (bBinFull == TRUE)
	{
		BL_DEBUGBOX("Bin full - clear bin");

		if (bClearBin)		//** FOr MS100 9Inch option only **		//v4.21
		{
			if (!CreateOutputFileWithClearBin(lMgzn, lSlot, lFullStatus, lOldBlock, GetCurrMgznSlotBC()))
			{
				BL_DEBUGBOX("unload frame, clear counter error");
				HouseKeeping(m_lReadyPos_X);
				return FALSE;
			}

			//Reset barcode name to default
			if ( !bEnableCopyTempFile )  //if not copy tempfile //v4.44T4
			{
				SaveBarcodeData(lOldBlock, "", m_lCurrMgzn, m_lCurrSlot);
			}
		}

		switch (m_lOMRT)
		{
			case BL_MODE_A:
			case BL_MODE_F:		//MS100 8mag config		//v3.82		

				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lOldBlock;
				if (!IsAutoLoadUnloadQCTest() && IsMSAutoLineMode())		//v4.56A11
				{
					m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_FULL;
				}
				else
				{
					m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
					m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				}
				break;

			case BL_MODE_B:
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lOldBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				break;

			case BL_MODE_C:
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_FULL;
				break;

			case BL_MODE_D:

				//v4.16T5	//New ModeD for Cree China
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lOldBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				break;

			case BL_MODE_E:		//v3.45
			case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
				m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lOldBlock;
				m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = GetCurrMgznSlotBC();
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				break;

			case BL_MODE_H:	//MS100 4mag 100bins config		//v4.31T10
				m_stMgznRT[lMgzn].m_lSlotUsage[lSlot] = BL_SLOT_USAGE_FULL;
				break;
		}


		if ( IsMapDetectSkipMode() )	
		{
			LONG lOrigGrade = (LONG)(CMS896AStn::m_WaferMapWrapper.GetOriginalGrade((UCHAR)(m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()))
								- CMS896AStn::m_WaferMapWrapper.GetGradeOffset());
			szText1.Format("S%d, B%d(%ld)", lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot], lOrigGrade);
		}
		else
		{
			szText1.Format("S%d, B%d", lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		}

		str = "#5 BL FilmFrame is unloaded to " + 
				GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + " (F), BLBarCode = " + m_szBCName;
		

		if (lMgzn == BL_MGZ_TOP_1)		//lMgzn is 0-based
		{
			SECS_UpdateCassetteSlotInfo(lMgzn, lSlot + 1);		//v4.59A10	//Autoline
		}
		SendBinTableUnloadEvent_8011(m_lCurrMgzn, lSlot, _T("B2T"));

		SendBinTableUnloadCompletedEvent_8012(m_lCurrMgzn, lSlot, _T("B2T"));

	}
	else
	{
		if ( IsMapDetectSkipMode() )	
		{
			LONG lOrigGrade = (LONG)(CMS896AStn::m_WaferMapWrapper.GetOriginalGrade((UCHAR)(m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()))
								- CMS896AStn::m_WaferMapWrapper.GetGradeOffset());
			szText1.Format("S%d, B%d (Bin#%ld) (%ld)", lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot], lOldBlock, lOrigGrade);
		}
		else
		{
			szText1.Format("S%d, B%d (Bin#%ld)", lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot], lOldBlock);
		}

		if (m_bUseBarcode)
		{
			str = "#6 BL FilmFrame is unloaded to " + 
				 GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + ", BLBarCode = " + m_szBCName;
		}
		else
		{
			str = "#7 BL FilmFrame is unloaded to " + GetMagazineName(lMgzn).MakeUpper() + ", " + szText1;	//v4.46T9 Cree
		}

		//Set SECSGEM Value
		//ULONG ulBindieCount;
		CString szCassettePos = GetCassettePositionName(lMgzn);
		if (lMgzn == BL_MGZ_TOP_1)		//lMgzn is 0-based
		{
			SECS_UpdateCassetteSlotInfo(lMgzn, lSlot + 1);		//v4.59A10	//Autoline
		}
		SendBinTableUnloadEvent_8011(m_lCurrMgzn, lSlot, szCassettePos);

		SendBinCasseteLoadWIPEvent_8014(m_lCurrMgzn, lSlot, szCassettePos);
	}

	SetStatusMessage(str);
	BL_DEBUGBOX(str);
	
	szBinBlkGrade.Format("%d", lBlockNo);	//v3.89
	CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Unload Frame", GetMagazineName(lMgzn).MakeUpper(), 
			lSlot + 1, m_szBCName, szBinBlkGrade, GetBinBlkBondedCount(lSlot + 1), m_bUseBarcode);

	if (bBurnInUnload == TRUE)
	{
		BOOL bTemp = FALSE;
		switch (m_lOMRT)
		{
			case BL_MODE_A:
			case BL_MODE_B:
			case BL_MODE_F:		//MS100 8mag config		//v3.82
				for (i = 0; i < MS_BL_MGZN_NUM; i++)
				{
					if (m_stMgznRT[i].m_lMgznState == BL_MGZN_STATE_FULL)
					{
						ResetMagazine(i);
						m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
					}
				}
				break;

			case BL_MODE_C:
				if (GetLoadMgzSlot(bTemp, lOldBlock) == FALSE)
				{
					ResetMgznByPhyBlock(lOldBlock);
				}
				break;

			case BL_MODE_D:
				for (i = 0; i < MS_BL_MGZN_NUM; i++)
				{
					if ((m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED) && 
							(m_stMgznRT[i].m_lMgznState == BL_MGZN_STATE_FULL))
					{
						ResetMagazine(i);
						m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
					}
				}
				break;

			case BL_MODE_E:		//v3.45
			case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
				//case BL_MODE_H:	//MS100 4mag 100bins config		//v4.31T10
				for (i = 0; i < MS_BL_MGZN_NUM; i++)
				{
					if (m_stMgznRT[i].m_lMgznState == BL_MGZN_STATE_FULL)
					{
						ResetMagazine(i);
						m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
					}
				}
				break;
		}
	}

	//LONG lOldBlock = GetCurrBinOnBT();		//v4.24
	m_lBTCurrentBlock = 0;
	m_szBinFrameBarcode = "";
	szMsg.Format("unload frame #%ld updated to #%ld", lOldBlock, GetCurrBinOnBT());
	BL_DEBUGBOX(szMsg);

	if (m_bIsDualBufferExist)		//v3.94
	{
		UpdateExArmMgzSlotNum(GetCurrBinOnBT());
	}

	if (bModeDError == TRUE)
	{
		HouseKeeping(m_lReadyPos_X);
		return FALSE;
	}

	//v3.02T6
	TRY 
	{
		SaveData();
		//SaveMgznRTData();
	} CATCH(CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

	TRY 
	{
		SaveMgznRTData();		//v4.06
	} CATCH(CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	//CMSLogFileUtility::Instance()->BL_BackupLogStatus();	//v3.02T6	//v4.21T5
	BL_DEBUGBOX("Unload finish\n");
	return TRUE;
}
*/
//=========================================================================
//  Sync the load/unload frame function with SECS load/unload command
//=========================================================================
BOOL CBinLoader::WaitForLoadUnloadSECSCmdObject()
{
	while (m_lLoadUnloadSECSCmdObject == 1)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}
		if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
			(Action() == glABORT_STOP)))
		{
			return FALSE;
		}
		Sleep(100);
	}
	return TRUE;
}

BOOL CBinLoader::IsLoadUnloadSECSCmdLocked()
{
	return (m_lLoadUnloadSECSCmdObject == 1);
}

VOID CBinLoader::LoadUnloadSECSCmdLock()
{
	m_lLoadUnloadSECSCmdObject = 1;
}

VOID CBinLoader::LoadUnloadSECSCmdUnlock()
{
	m_lLoadUnloadSECSCmdObject = 0;
}


BOOL CBinLoader::WaitForLoadUnloadObject()
{
	long lCount = 0;
	while (m_lLoadUnloadObject == 1)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		Sleep(100);
		lCount++;
		if (lCount  > 50)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CBinLoader::LoadUnloadLockMutex()
{
	CSingleLock lock(&m_csLoadUnloadMutex);
	lock.Lock();

	if (!WaitForLoadUnloadSECSCmdObject())
	{
		return FALSE;
	}
	LoadUnloadLock();

	return TRUE;
}

VOID CBinLoader::LoadUnloadLock()
{
	m_lLoadUnloadObject = 1;
}

VOID CBinLoader::LoadUnloadUnlock()
{
	m_lLoadUnloadObject = 0;
}

BOOL CBinLoader::IsAutoLoadUnloadQCTest()
{
	return (m_bAutoLoadUnloadTest == 1);
}

BOOL CBinLoader::IsAutoLoadUnloadSISTest()
{
	return (m_bAutoLoadUnloadTest == 2);
}

LONG CBinLoader::CheckClosedALBackGate()
{
	if (IsMSAutoLineMode() && !CloseALBackGate(TRUE) && !IsCloseALBackGate())
	{
		CString szMsg;
		szMsg = _T("Bin Loader Back Gate Not Closed or Front Gate Not Opened");
		CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);
		SetAlert_Msg_Red_Yellow(IDS_BL_BACK_GATE_NOT_CLOSED, szMsg);		
		SetErrorMessage("Bin Loader Back Gate Not Closed or Front Gate Not Opened");
		return Err_BinBackGateNotClosed;
	}

	return 0;
}


LONG CBinLoader::TransferWIPBinFullFrame(BOOL bBurnInLoad, LONG lLoadBlock, BOOL bOffline, BOOL bClearAllMode, 
									 BOOL bDirectUnload, BOOL bDoNotHomeafterLoad, BOOL bBinFullLoadEmpty,
									 BOOL bManualChangeFrame)
{
	CString szLog;
	LONG lMgzn = 0, lSlot = 0;
	LONG lRet = 0;

	CBinGripperBuffer	*pGripperBuffer = &m_clUpperGripperBuffer;

	while (TRUE)
	{
		LONG lWIPLoadBlock = lLoadBlock;
		if (!IsOutputEmptySlot_AutoLine())
		{
			break;
		}

		if (!GetWIPFullSlot_AutoLine(lMgzn, lSlot))
		{
			break;
		}

		lWIPLoadBlock = GetCassetteSlotGradeBlock(lMgzn, lSlot);
		szLog.Format("TransferWIPBinFullFrame: Get WIP Frame Slot(%d,%d), (WIP)Bin Block = %d", lMgzn, lSlot, lWIPLoadBlock);
		BL_DEBUGBOX(szLog);

		//Get a WIP frame with full state to the output slot
		lRet = SubLoadBinFrameWithLock(bBurnInLoad, pGripperBuffer, lWIPLoadBlock, bOffline, bClearAllMode, 
									   bDirectUnload, bDoNotHomeafterLoad, bBinFullLoadEmpty, bManualChangeFrame, TRANSFER_WIP_FULL_BIN_FRAME);
		if (lRet == Err_BinBackGateNotClosed)
		{
			return FALSE;
		}
		else if (lRet == TRUE)
		{
			szLog.Format("TransferWIPBinFullFrame: Unload WIP Frame to Output");
			BL_DEBUGBOX(szLog);
			//Transfer WIP Slot, BIN FULL option is always taken as "FALSE" becasue 
			//if WIP is FULL Status, this output file was already created.
//			if (UnloadBinFrame(FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE) == FALSE)
			if (UnloadBinFrame(FALSE, pGripperBuffer, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE) == FALSE)
			{
				//Reset SRAM becasue transfer WIP slot failure
				(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= 0;
				(*m_psmfSRam)["BinTable"]["BlkInUse"]		= 0;
				return FALSE;
			}
		}
		else
		{
			//Reset SRAM becasue transfer WIP slot failure
			(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= 0;
			(*m_psmfSRam)["BinTable"]["BlkInUse"]		= 0;
			return lRet;
		}
	}
	return TRUE;
}

//================================================================================================
//  Find the slot with lLoadBlock ID in wafer load slot
//  if m_stMgznRT[lCurrMgzn].m_lSlotBlock[j] is the same as lLoadBlock, this slot#j is target slot
//  otherwise find a empty slot.
//================================================================================================
LONG CBinLoader::LoadBinFrame(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, LONG lLoadBlock, BOOL bOffline, BOOL bClearAllMode, 
							  BOOL bDirectUnload, BOOL bDoNotHomeafterLoad, BOOL bBinFullLoadEmpty,
							  BOOL bManualChangeFrame)
{
	//Auto Line mode wait for empty slot
	if (!HaveEmptyOutputSlot_AutoLine() && IsMSAutoLineMode())
	{
//		Sleep(2500);
		if (!WaitForAutoLineToRemoveOutputWIPFrame(bManualChangeFrame))
		{
			CString szLog;
			szLog.Format("BL: Err_BinMagzineOutputWIPFull, when load bin frame check empty slot");
			BL_DEBUGBOX(szLog);
			return Err_BinMagzineOutputWIPFull; //no response from autoline.
		}
	}

	LONG lRet = 0;

	lRet = SubLoadBinFrameWithLock(bBurnInLoad, pGripperBuffer, lLoadBlock, bOffline, bClearAllMode, 
								   bDirectUnload, bDoNotHomeafterLoad, bBinFullLoadEmpty, bManualChangeFrame);

	if (lRet == Err_BinBackGateNotClosed)
	{
		return FALSE;
	}

	if (!(IsAutoLoadUnloadQCTest() || IsAutoLoadUnloadSISTest()) && IsMSAutoLineMode() && (lRet == Err_BinMagzineOutputWIPFull))
	{
		if (!WaitForAutoLineToRemoveOutputWIPFrame(bManualChangeFrame))
		{
			return Err_BinMagzineOutputWIPFull; //no response from autoline.
		}

		//Reset Trigger "TransferBinFrame"
		m_bTriggerTransferBinFrame = FALSE; 

		CString szLog;
		szLog.Format("BL: Err_BinMagzineOutputWIPFull");
		BL_DEBUGBOX(szLog);

		LONG lMgzn = 0, lSlot = 0;

		//there has three cases when load a fram from WIP slot into output slot.
		BOOL bTransferWIPBinFullFrame = GetWIPFullSlot_AutoLine(lMgzn, lSlot);
		BOOL bTransferWIPNotSelectedBinFrame = FALSE;
		BOOL bTransferWIPBinOnlyOneFrame = FALSE;
		CUIntArray aulSelectdGradeList; 
		CUIntArray aulSelectdGradeLeftDieCountList;
		BOOL bASCIIGradeDisplayMode = FALSE;

		LONG lTransferWIPSlotMode = bTransferWIPBinFullFrame ? TRANSFER_WIP_FULL_BIN_FRAME : TRANSFER_WIP_NONE_FRAME;
		if (!bTransferWIPBinFullFrame)
		{
			CWaferTable *pWaferTable = dynamic_cast<CWaferTable*>(GetStation(WAFER_TABLE_STN));
			if (pWaferTable != NULL)
			{
				pWaferTable->GetWaferMapSelectedGradeList(aulSelectdGradeList, aulSelectdGradeLeftDieCountList);
				bASCIIGradeDisplayMode = pWaferTable->IsASCIIGradeDisplayMode();
			}

			bTransferWIPNotSelectedBinFrame = GetWIPActiveSlotWithoutGradeList_AutoLine(bASCIIGradeDisplayMode, aulSelectdGradeList, aulSelectdGradeLeftDieCountList, lMgzn, lSlot);
			if (bTransferWIPNotSelectedBinFrame)
			{
				lTransferWIPSlotMode = TRANSFER_WIP_NOT_SELECTED_BIN_FRAME;
			}
		}
		if (!bTransferWIPBinFullFrame && !bTransferWIPNotSelectedBinFrame)
		{
			bTransferWIPBinOnlyOneFrame = TRUE;
			lTransferWIPSlotMode = TRANSFER_WIP_ONLY_ONE_BIN_FRAME;
		}

		szLog.Format("LoadBinFrame: Get WIP Frame Mode = %d, %d, %d, %d, bASCIIGradeDisplayMode = %d", lTransferWIPSlotMode, 
					 bTransferWIPBinFullFrame, bTransferWIPNotSelectedBinFrame, bTransferWIPBinOnlyOneFrame, bASCIIGradeDisplayMode);
		BL_DEBUGBOX(szLog);

		while (TRUE)
		{
			LONG lWIPLoadBlock = lLoadBlock;
			if (!IsOutputEmptySlot_AutoLine())
			{
				break;
			}

			//Case#1: Only Get the FULL state's Frame in WIP slot
			if (bTransferWIPBinFullFrame && !GetWIPFullSlot_AutoLine(lMgzn, lSlot))
			{
				break;
			}

			//Case#2: Only Get the ACTIVE state's Frame in WIP slot, which is not belong to the selected grade list
			//of the wafer map
			if (bTransferWIPNotSelectedBinFrame && !GetWIPActiveSlotWithoutGradeList_AutoLine(bASCIIGradeDisplayMode, aulSelectdGradeList, aulSelectdGradeLeftDieCountList, lMgzn, lSlot))
			{
				break;
			}

			//Case#3: Only Get a ACTIVE state's Frame in WIP slot according to the sequence
			if (bTransferWIPBinOnlyOneFrame && !GetWIPActiveSlot_AutoLine(lMgzn, lSlot))
			{
				break;
			}

			lWIPLoadBlock = GetCassetteSlotGradeBlock(lMgzn, lSlot);
			szLog.Format("LoadBinFrame: Get WIP Frame Slot(%d,%d), (WIP)Bin Block = %d", lMgzn, lSlot, lWIPLoadBlock);
			BL_DEBUGBOX(szLog);

			//Get a WIP frame with full state to the output slot
			lRet = SubLoadBinFrameWithLock(bBurnInLoad, pGripperBuffer, lWIPLoadBlock, bOffline, bClearAllMode, 
										   bDirectUnload, bDoNotHomeafterLoad, bBinFullLoadEmpty, bManualChangeFrame, lTransferWIPSlotMode);

			if (lRet == Err_BinBackGateNotClosed)
			{
				return FALSE;
			}

			if (lRet == TRUE)
			{
				szLog.Format("LoadBinFrame: Unload WIP Frame to Output");
				BL_DEBUGBOX(szLog);

				//Transfer WIP Slot, BIN FULL option is always taken as "FALSE" becasue 
				//if WIP is FULL Status, this output file was already created.
//				if (UnloadBinFrame(bBurnInLoad, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE) == FALSE)
				if (UnloadBinFrame(bBurnInLoad, pGripperBuffer, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE) == FALSE)
				{
					//Reset SRAM becasue transfer WIP slot failure
					(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= 0;
					(*m_psmfSRam)["BinTable"]["BlkInUse"]		= 0;
					return FALSE;
				}
			}
			else
			{
				szLog.Format("LoadBinFrame: Get WIP Frame Failure");
				BL_DEBUGBOX(szLog);

				//Reset SRAM becasue transfer WIP slot failure
				(*m_psmfSRam)["BinTable"]["LastBlkInUse"]	= 0;
				(*m_psmfSRam)["BinTable"]["BlkInUse"]		= 0;
				return lRet;
			}

			//Only transfer one frame from WIP slots to output.
			if (bTransferWIPBinOnlyOneFrame)
			{
				break;
			}
		}
		lRet =  SubLoadBinFrameWithLock(bBurnInLoad, pGripperBuffer, lLoadBlock, bOffline, bClearAllMode, 
										bDirectUnload, bDoNotHomeafterLoad, bBinFullLoadEmpty, bManualChangeFrame);
		if (lRet == Err_BinBackGateNotClosed)
		{
			return FALSE;
		}
	}
	return lRet;
}

//This function previous is LoadBinFrame_Auto
LONG CBinLoader::SubLoadBinFrameWithLock(BOOL bBurnInLoad, CBinGripperBuffer *pGripperBuffer, LONG lLoadBlock, BOOL bOffline, BOOL bClearAllMode, 
								 BOOL bDirectUnload, BOOL bDoNotHomeafterLoad, BOOL bBinFullLoadEmpty,
								 BOOL bManualChangeFrame, LONG lTransferWIPSlotMode)
{
	BOOL bClearAllFrameTOP2Mode = IsMSAutoLineMode() && bClearAllMode;
	//if bClearAllFrameTOP2Mode is TRUE, machine transfers the frame from WIP to TOP2 under inline-mode

	LONG lRet = 0;
	if (!bClearAllFrameTOP2Mode)
	{
		if (!LoadUnloadLockMutex())
		{
			CString szLog;

			szLog.Format("LoadUnloadLockMutex  SubLoadBinFrameWithLock  = %ld", m_lLoadUnloadSECSCmdObject);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}

		BL_DEBUGBOX("SubLoadBinFrameWithLock CheckClosedALBackGate");
		lRet = CheckClosedALBackGate();
		if (lRet)
		{
			LoadUnloadUnlock();
			return lRet;
		}
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		lRet = UDB_SubLoadBinFrame(bBurnInLoad, pGripperBuffer, lLoadBlock, bOffline, 
								   bManualChangeFrame, lTransferWIPSlotMode, bClearAllFrameTOP2Mode);
	}
	else
	{
//		lRet = SubLoadBinFrame(bBurnInLoad, lLoadBlock, bOffline, bClearAllMode, 
//							bDirectUnload, bDoNotHomeafterLoad, bBinFullLoadEmpty,
//							bManualChangeFrame, lTransferWIPSlotMode);
	}
	
	if (!bClearAllFrameTOP2Mode)
	{
		LONG lRet1 = CloseALFrontGate();
	
		if (lRet == TRUE)
		{
			lRet = lRet1;
		}

		LoadUnloadUnlock();
	}
	return lRet;
}

//===============================================================================
// if Output&WIP slots is full, wait for autoline to Remove the output&WIP frame
//===============================================================================
BOOL CBinLoader::WaitForAutoLineToRemoveOutputWIPFrame(const BOOL bManualChangeFrame)
{
	CString szLog;
	CString szMsg;
	szMsg = "WIP & Output Slots are Full, Waiting for Smart Inline...";

	if (!bManualChangeFrame)
	{
//		EquipStateProductiveToUnscheduleDown();
		SetAlertTime(TRUE, EQUIP_ALARM_TIME, "", szMsg);
	}

	CTmpChange<BOOL> bStopAlign(&CMS896AApp::m_bStopAlign, FALSE);
	LONG lCount = 0;

	while (!bManualChangeFrame)
	{
		m_comServer.ProcessRequest();

		MSG Msg; 
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (lCount == 0)
		{
			CloseAlarm();
			SetAlert_WarningMsg(IDS_MS_WARNING_INLINE_BIN_LOADER_OUTPUT_SLOT_FULL, szMsg);
			BL_DEBUGBOX(szMsg);
		}

		if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
			(Action() == glABORT_STOP)))
		{
			SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
//			EquipStateUnscheduleDwonToProductive();
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			szLog.Format("BL: WIP&Output Command(), Action() = %ld, %d, %d", Command(), Action(), CMS896AApp::m_bStopAlign);
			BL_DEBUGBOX(szLog);
			break;
		}

		Sleep(100);
		if (++lCount == 100)
		{
			lCount = 0;
		}

		if (m_bAutoLineUnloadDone)
		{
			SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
//			EquipStateUnscheduleDwonToProductive();
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			szLog.Format("BL: WIP&Output = %ld", m_bAutoLineUnloadDone);
			BL_DEBUGBOX(szLog);
			return TRUE;
			break;
		}
	}

	SetAlert_Red_Yellow(IDS_BL_MA_NOEPYFRAME_IN_EPYMAG);
	szLog = "BL error: No WIP slot available in magazine (AUTOLINE)";
	SetErrorMessage(szLog);
	BL_DEBUGBOX(szLog);
	HouseKeeping(m_lReadyPos_X);
	return FALSE;
}


//===============================================================================
// if Input Slots is empty, wait for autoline to input the frame
//===============================================================================
BOOL CBinLoader::WaitForAutoLineToInputFrame()
{
	CString szLog;
	CString szMsg;
	szMsg = "No Frame in Input Slots, Waiting for Smart Inline...";

//	EquipStateProductiveToUnscheduleDown();
	SetAlertTime(TRUE, EQUIP_ALARM_TIME, "", szMsg);

	CTmpChange<BOOL> bStopAlign(&CMS896AApp::m_bStopAlign, FALSE);
	LONG lCount = 0;
	while (TRUE)
	{
		m_comServer.ProcessRequest();

		MSG Msg;
		if (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			::DispatchMessage(&Msg);
		}

		if (lCount == 0)
		{
			CloseAlarm();
			SetAlert_WarningMsg(IDS_MS_WARNING_INLINE_BIN_LOADER_INPUT_SLOT_EMPTY, szMsg);
			szLog.Format("BL: No Frame in Input Slots, Waiting for Smart Inline...");
			BL_DEBUGBOX(szLog);
		}

		if (CMS896AApp::m_bStopAlign || ((Command() == glSTOP_COMMAND) &&
			(Action() == glABORT_STOP)))
		{
			SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
//			EquipStateUnscheduleDwonToProductive();
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();

			szLog.Format("BL: Command(), Action() = %ld, %d, %d", Command(), Action(), CMS896AApp::m_bStopAlign);
			BL_DEBUGBOX(szLog);
			break;
		}

		Sleep(100);
		if (++lCount == 100)
		{
			lCount = 0;
		}

		if (m_bAutoLineLoadDone)
		{
			SetAlertTime(FALSE, EQUIP_ALARM_TIME, "", szMsg);
//			EquipStateUnscheduleDwonToProductive();
			SetAlarmLamp_Green(FALSE, TRUE);
			CloseAlarm();
			szLog.Format("BL: GetLoadMgzSlot = %ld", m_bAutoLineLoadDone);
			BL_DEBUGBOX(szLog);
			return TRUE;
			break;
		}
	}
	return FALSE;
}


LONG CBinLoader::PushBackFrameFromGripper_DL(BOOL bBurnInUnload)
{
	BL_DEBUGBOX("Push Back from gripper DL start");

	if (BinGripperMoveSearchJam(m_lLoadMagPos_X) == FALSE)
	{
		SetGripperState(FALSE);
		BOOL bOk = BinLoaderUnloadJamMessage(bBurnInUnload);

		if (bBurnInUnload == FALSE && 
				!m_bDisableBL && bOk == FALSE)
		{
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}

	Sleep(200);
	SetGripperState(FALSE);
	Sleep(500);

	if (BinGripperMoveTo(m_lReadyPos_X) != TRUE)
	{
		HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		return FALSE;
	}

	return TRUE;
}

LONG CBinLoader::PushBackFrameFromGripper2_DL(BOOL bBurnInUnload)
{
	BL_DEBUGBOX("Push Back from gripper2 DL start");

	if (BinGripper2MoveSearchJam(m_lLoadMagPos_X) == FALSE)
	{
		SetGripper2State(FALSE);
		BOOL bOk = BinLoaderUnloadJamMessage(bBurnInUnload);

		if (bBurnInUnload == FALSE && 
				!m_bDisableBL && bOk == FALSE)
		{
			HouseKeeping2(m_lPreUnloadPos_X2);
			return FALSE;
		}
	}

	Sleep(200);
	SetGripper2State(FALSE);
	Sleep(500);

	if (BinGripper2MoveTo(m_lReadyPos_X) != TRUE)
	{
		HouseKeeping2(m_lReadyPos_X2, TRUE, TRUE);
		return FALSE;
	}

	return TRUE;
}

LONG CBinLoader::PushBackFrameFromGripper(LONG lLoadBlock, BOOL bBurnInUnload, LONG lMgzn, LONG lSlot, BOOL bToFullMagzn)
{

	ULONG i;
	LONG lFullStatus = 1;

	BOOL bWaitElevatorComplete = TRUE;	//!m_bIsExChgArmExist;
	CString str, szErr;
	CString szText1, szText2;

	//Check Gripper position is valid
	if (IsMagazineSafeToMove() == FALSE)
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);

		return FALSE;
	}

	//BL_DEBUGBOX("Push Back start");
	str.Format("PushBack from gripper start: Mgzn=%ld, Slot=%ld, EMPTY=%d", lMgzn, lSlot, bToFullMagzn);	//v4.50A5
	BL_DEBUGBOX(str);
	
	//Get the free lMgzn & Slot no
	if (bToFullMagzn == TRUE)
	{
		//Get the empty slot from Full mgz
		//update the lMgzn & lSlot 
		lFullStatus = GetFullMgznSlot(lMgzn, lSlot, FALSE, lLoadBlock);		//v4.59A20

		if (lFullStatus == FALSE)
		{
			return FALSE;
		}

		szErr.Format("BL Push Back frame (Grade:%d) to Full Magazine to Slot:%d", lLoadBlock, lSlot + 1);
		SetErrorMessage(szErr);
		str.Format("Unload (PushBack) filmframe to FULL - M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
	}

	else
	{
		szErr.Format("BL Push Back frame to Empty Magazine to Slot:%d", lSlot + 1);
		SetErrorMessage(szErr);
		str.Format("Unload (PushBack) filmframe to M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
	}

	BL_DEBUGMESSAGE(str);

	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}
	
	//Always complete Z motion in DL (not for DLA)
	if (MoveElevatorToUnload(lMgzn, lSlot, TRUE, bWaitElevatorComplete, FALSE) == FALSE)		//v4.34T9	//Cree HuiZhou
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	if (BinGripperMoveSearchJam(m_lLoadMagPos_X) == FALSE)
	{
		SetGripperState(FALSE);
		//if( bBurnInUnload==FALSE && BL_OK_CANCEL("Frame is jammed!\nPlease push it into the magazine slot", "Unload Frame Message")==FALSE )

		BOOL bOk = BinLoaderUnloadJamMessage(bBurnInUnload);

		if (bBurnInUnload == FALSE && 
				!m_bDisableBL && bOk == FALSE)
		{
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}
	
	//Set the full magazine slot to used so that it will go to next slot for next time
	if (bToFullMagzn == TRUE)
	{
		m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lLoadBlock;
		str.Format("bToFullMagzn to M%d, S%d, B%d", lMgzn, lSlot + 1, lLoadBlock);
		BL_DEBUGBOX(str);
	}

	//handle mode A only
	// Reset the Magazine when it is in FULL state
	if (bBurnInUnload == TRUE)
	{
		BOOL bTemp = FALSE;
		for (i = 0; i < MS_BL_MGZN_NUM; i++)
		{
			if (m_stMgznRT[i].m_lMgznState == BL_MGZN_STATE_FULL)
			{
				ResetMagazine(i);
				m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
			}
		}
	}

	CString szMsg;
	szMsg.Format("push back frame from gripper updated to block %d", GetCurrBinOnBT());
	BL_DEBUGBOX(szMsg);

	m_lBTCurrentBlock = 0;
	m_szBinFrameBarcode = "";

	try
	{
		SaveData();
		//SaveMgznRTData();
	}
	catch (CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Push Back Frame From Gripper");
	}

	TRY 
	{
		SaveMgznRTData();		//v4.06
	} CATCH(CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	Sleep(200);
	SetGripperState(FALSE);
	Sleep(200);

	BL_DEBUGBOX("x ready, down magazine");

	if (CheckElevatorCover() == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	if (BinGripperMoveTo(m_lReadyPos_X) != TRUE)
	{
		HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		return FALSE;
	}

	//Must wait Z motion complete, otherwise gripper will crashes with elevator
	if (MoveElevatorToUnload(lMgzn, lSlot, FALSE, TRUE) == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	if (bToFullMagzn == TRUE)
	{
		//v4.50A5	//SEmitek
		CreateOutputFileWithClearBin(lMgzn, lSlot, lFullStatus, lLoadBlock, GetCurrMgznSlotBC());

		BL_DEBUGBOX("Push Back finish (FULL)\n");	//v4.50A5
	}
	else
	{
		BL_DEBUGBOX("Push Back finish\n");
	}
	
	return TRUE;
}

LONG CBinLoader::PushBackFrameFromBinTable(LONG lLoadBlock, BOOL bBurnInUnload, LONG lMgzn, LONG lSlot, BOOL bToFullMagzn, BOOL bOffline)
{

	ULONG i;
	LONG lFullStatus = 1;

	BOOL bWaitElevatorComplete = !m_bIsExChgArmExist;
	CString str, szErr;
	CString szText1, szText2;

	//Check Gripper position is valid
	if (IsMagazineSafeToMove() == FALSE)
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);

		return FALSE;
	}

	BL_DEBUGBOX("Push Back start");
	
	//check if the film is full or not
	if (bToFullMagzn == TRUE)
	{
		//Get the empty slot from Full mgz
		//update the lMgzn & lSlot to full Mgzn
		lFullStatus = GetFullMgznSlot(lMgzn, lSlot, FALSE, lLoadBlock);		//v4.59A20

		if (lFullStatus == FALSE)
		{
			return FALSE;
		}

		szErr.Format("BL Push Back frame (Grade:%d) to Full Magazine to Slot:%d", lLoadBlock, lSlot + 1);
		SetErrorMessage(szErr);
		str.Format("Unload filmframe to full M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
	}

	else
	{
		szErr.Format("BL Push Back frame to Empty Magazine to Slot:%d", lSlot + 1);
		SetErrorMessage(szErr);
		str.Format("Unload filmframe to M%d, S%d, B%d", lMgzn, lSlot + 1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
	}

	BL_DEBUGMESSAGE(str);

	INT nCheckFramePosSucc;

	//check the frame position on the bin table
	if ((nCheckFramePosSucc = CheckFramePosOnBinTable(bBurnInUnload , bOffline)) != BL_FRAME_CHECK_POS_SUCCESS)
	{
		return nCheckFramePosSucc;
	}
		
	//move gripper to unload position
	BL_DEBUGBOX("gripper to preload & magazine to unload");
	if (BinGripperMoveTo_Auto(m_lPreUnloadPos_X, SFM_NOWAIT) != TRUE)
	{
		return FALSE;
	}

	if (CheckElevatorCover() == FALSE)
	{
		return FALSE;
	}
	
	//move the elevator to unload position
	//Always complete Z motion in DL (not for DLA)
	if (MoveElevatorToUnload(lMgzn, lSlot, TRUE, bWaitElevatorComplete) == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	BL_DEBUGBOX("BT to unload");
#ifdef NU_MOTION
	if (MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, 1, TRUE) == FALSE)	//v4.22T7
	{
		return FALSE;
	}
#else
	if (MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y) == FALSE)				//v4.23T1
	{
		return FALSE;
	}
#endif

	BL_DEBUGBOX("gripper move to unload");

	if (BinGripperSynMove_Auto() == FALSE)
	{
		return FALSE;
	}

	if (bWaitElevatorComplete == FALSE)
	{
		Z_Sync();
//		Y_Sync();
/*
		if ((bBurnInUnload == FALSE) && (IsMagazineExist(lMgzn) == FALSE))
		{
			Sleep(200);
			if (IsMagazineExist(lMgzn, FALSE, TRUE) == FALSE)	//v4.41T2
			{
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
			}
		}
*/
	}

	if (bBurnInUnload == FALSE && IsMagazineExist(lMgzn) == FALSE)
	{
		Sleep(200);
		if (IsMagazineExist(lMgzn, FALSE, TRUE) == FALSE)	//v4.41T2
		{
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}

	BL_DEBUGBOX("SettleDown for unload");
	SettleDownBeforeUnload();

	LONG lOffsetX = m_lUnloadOffsetX;
	if (lOffsetX > 0)
	{
		if (BinGripperMoveTo_Auto((m_lUnloadPos_X - lOffsetX), SFM_WAIT) != TRUE)
		{
			return FALSE;
		}
	}

	//searching frame
	BL_DEBUGBOX("Searching frame");
	LONG lReturn = TRUE;
		
	if (lOffsetX > 0)
	{
		lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, lOffsetX + 100, 400);
	}
	else
	{
		lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, abs(m_lUnloadPos_X - m_lPreUnloadPos_X) + 100, 400);
	}

	if (!lReturn)
	{   
		if ((bBurnInUnload == FALSE) && (BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, 
										 IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE))
		{
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}

	Sleep(200);
	SetGripperState(TRUE);
	Sleep(200);


	if (BinGripperMoveSearchJam(m_lLoadMagPos_X) == FALSE)
	{
		SetGripperState(FALSE);
		//if( bBurnInUnload==FALSE && BL_OK_CANCEL("Frame is jammed!\nPlease push it into the magazine slot", "Unload Frame Message")==FALSE )
		BOOL bOk = BinLoaderUnloadJamMessage(bBurnInUnload);

		if (bBurnInUnload == FALSE && 
				!m_bDisableBL &&			//v3.60
				bOk == FALSE)
		{
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}

	Sleep(200);
	SetGripperState(FALSE);
	Sleep(200);

	BL_DEBUGBOX("x ready, down magazine");

	if (CheckElevatorCover() == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	//Must wait Z motion complete, otherwise gripper will crashes with elevator
	if (MoveElevatorToUnload(lMgzn, lSlot, FALSE, TRUE) == FALSE)
	{
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}

	if (BinGripperMoveTo(m_lReadyPos_X) != TRUE)
	{
		HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		return FALSE;
	}
	
	if (bToFullMagzn == TRUE)
	{

		m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = lLoadBlock;
	}

	//handle mode A only
	// Reset the Magazine when it is in FULL state
	if (bBurnInUnload == TRUE)
	{
		BOOL bTemp = FALSE;
		for (i = 0; i < MS_BL_MGZN_NUM; i++)
		{
			if (m_stMgznRT[i].m_lMgznState == BL_MGZN_STATE_FULL)
			{
				ResetMagazine(i);
				m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
			}
		}
	}

	m_lBTCurrentBlock = 0;
	m_szBinFrameBarcode = "";
	CString szMsg;
	szMsg.Format("push back frame from bt updated to block %d", GetCurrBinOnBT());
	BL_DEBUGBOX(szMsg);

	try
	{
		SaveData();
		//SaveMgznRTData();
	}
	catch (CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in Push Back Frame From Bin Table");
	}

	TRY 
	{
		SaveMgznRTData();		//v4.06
	} CATCH(CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	if (bToFullMagzn == TRUE)	//v4.50A5
	{
		//v4.50A5	//SEmitek
		CreateOutputFileWithClearBin(lMgzn, lSlot, lFullStatus, lLoadBlock, GetCurrMgznSlotBC());

		BL_DEBUGBOX("Push Back finish (FULL)\n");
	}
	else
	{
		BL_DEBUGBOX("Push Back finish\n");
	}

	return TRUE;
}


/*
Function to handle No Barcode read on the frame cases
check for whether the frame with no barcode can be successfully push back to empty/full magnz
If success, return BL_FRAME_WT_NO_BAR_CODE and get another film from empty magnz
else return false to stop
lMgzn and lSlot are slot of the empty magnz
*/

LONG CBinLoader::NoBarCodeHandler(LONG lLoadBlock, BOOL bBurnInLoad, LONG lMgzn, LONG lSlot, BOOL bUseEmpty, BOOL bOffline, BOOL bClearAllMode, BOOL bFromBinTable, BOOL bBT2)
{
	BL_DEBUGBOX("Unknown Barcode Handler Begins!");	
	if (bBT2 == TRUE)
	{
		if (m_szBCName2 != BL_DEFAULT_BARCODE && m_szBCName2 != "")
		{
			BL_DEBUGBOX("NoBarCodeHandler m_szBCName2 = " + m_szBCName2);	//v4.50A26
			return TRUE;
		}
		else
		{
			BL_DEBUGBOX("m_szBCName2 == BL_DEFAULT_BARCODE");	
		}
	}
	else if (m_szBCName != BL_DEFAULT_BARCODE && m_szBCName != "")
	{
		BL_DEBUGBOX("NoBarCodeHandler m_szBCName = " + m_szBCName);			//v4.50A26
		return TRUE;
	}
	else
	{
		BL_DEBUGBOX("m_szBCName == BL_DEFAULT_BARCODE");
	}
	(*m_psmfSRam)["BinLoader"]["ScanBarcodeName"] = m_szBCName;


	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetCustomerName() == "TongHui") && bUseEmpty)
	{
		CString szText = "\n";
		AlertMsgForManualInputBarcode(lLoadBlock, m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
		SetErrorMessage(m_szBCName + " input by operator from empty BL");
		BL_DEBUGBOX("TH Empty Load BL input BC " + m_szBCName);
		return TRUE;
	}

	if ((m_bCheckBarcode != TRUE))
	{
		BL_DEBUGBOX("NoBarCodeHandler: m_bCheckBarcode is disabled");		//v4.50A26
		return TRUE;
	}
	
	BL_DEBUGBOX("NoBarCodeHandler: Customer = " + pApp->GetCustomerName());	//v4.50A26
	if (pApp->GetCustomerName() == "SeoulSemi" || pApp->GetCustomerName() == "OSRAM")
	{
		BL_DEBUGBOX("Read Barcode Fails, Push back to original slot!");		//v4.50A26
		//HmiMessage_Red_Yellow("Read Barcode Fails, Push back to original slot!");
		SetAlert_Red_Yellow(IDS_BL_READ_BC_FAILURE);		//v4.59A22
	}

	//check frame is no barcode & current barcode is empty
	if ( (GetCurrMgznSlotBC().IsEmpty() == TRUE) || 
		 pApp->GetCustomerName() == "SeoulSemi" )
	{
		CString szErr;
		INT nPushBackFrameStatus = 0;

		szErr.Format("; EMPTY = %d; FromTable = %d", bUseEmpty, bFromBinTable);
		BL_DEBUGBOX("NoBarCodeHandler PushBackFrame - (" + m_szBCName2 + "); Customer: " + pApp->GetCustomerName() + szErr);

		if (bUseEmpty)
		{
			if (bBT2)
			{
				BL_DEBUGBOX("NoBarCodeHandler: BT2 PushBackFrameFromGripper2_DL EMPTY ...");//v4.50A26
				PushBackFrameFromGripper2_DL(bBurnInLoad);
				HouseKeeping2(m_lReadyPos_X2);
				return FALSE;
			}
			else
			{
				if (pApp->GetCustomerName() == "SeoulSemi" || pApp->GetCustomerName() == "OSRAM")
				{
					BL_DEBUGBOX("NoBarCodeHandler: BT1 PushBackFrameFromGripper_DL EMPTY ...");//v4.50A26
					PushBackFrameFromGripper_DL(bBurnInLoad);
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
				else
				{
					BL_DEBUGBOX("NoBarCodeHandler: BT1 PushBackFrameFromBinTable EMPTY ...");	//v4.50A26
					if (bFromBinTable)
					{
						nPushBackFrameStatus = PushBackFrameFromBinTable(lLoadBlock, bBurnInLoad, lMgzn, lSlot, FALSE);
					}
					else
					{
						nPushBackFrameStatus = PushBackFrameFromGripper(lLoadBlock, bBurnInLoad, lMgzn, lSlot, FALSE);
					}
				}
			}
		}
		else
		{
			if (bBT2)
			{
				BL_DEBUGBOX("NoBarCodeHandler: BT2 PushBackFrameFromGripper2_DL ...");//v4.50A26
				PushBackFrameFromGripper2_DL(bBurnInLoad);
				HouseKeeping2(m_lReadyPos_X2);
				return FALSE;
			}
			else
			{
				if (pApp->GetCustomerName() == "SeoulSemi" || pApp->GetCustomerName() == "OSRAM")
				{
					BL_DEBUGBOX("NoBarCodeHandler: BT1 PushBackFrameFromGripper_DL ...");//v4.50A26
					PushBackFrameFromGripper_DL(bBurnInLoad);
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
				else
				{
					BL_DEBUGBOX("NoBarCodeHandler: BT1 PushBackFrameFromBinTable ...");	//v4.50A26
					if (bFromBinTable)
					{
						nPushBackFrameStatus = PushBackFrameFromBinTable(lLoadBlock, bBurnInLoad, lMgzn, lSlot, TRUE);
					}
					else
					{
						nPushBackFrameStatus = PushBackFrameFromGripper(lLoadBlock, bBurnInLoad, lMgzn, lSlot, TRUE);
					}
				}
			}
		}


		if (nPushBackFrameStatus)
		{
			if (bUseEmpty && !bFromBinTable)
			{
				SetErrorMessage("BL: bin frame with no barcode");		//v3.27T2
				return BL_FRAME_WT_NO_BAR_CODE;
			}
			else
			{
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
				SetErrorMessage("BL: bin frame with no barcode");		//v3.27T2
				return BL_FRAME_WT_NO_BAR_CODE;
			}
		}
		else
		{
			m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
			m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
			//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);			
			SetErrorMessage("BL Gripper Jam at UNLOAD EMPTY frame||Unknow Barcode!!Push Back!!4.47T");
			BL_DEBUGBOX("Gripper is jammed");
			return FALSE;
		}
	}
	else	//check frame has barcode & current barcode is empty
	{
		CString szLog;
		szLog.Format("NoBarCodeHandler: frame already has BC before; CBC = %d", m_bCompareBarcode);
		BL_DEBUGBOX(szLog);

		if (m_bCompareBarcode == FALSE)
		{
			CString szText = " ";	
			LONG lOption;
			lOption = SetAlert_Msg_Red_Back(IDS_BL_CHECK_BARCODE, szText, "Ignore", "Stop");		

			if (lOption != 1)
			{
				SetErrorMessage("BL: Barcode check failure ABORT");			//v3.27T2
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
			}

			SetErrorMessage("BL: Barcode check failure IGNORE");		//v3.27T2
		}
	}

	return TRUE;
}

BOOL CBinLoader::UpdateExArmMgzSlotNum(LONG lBlock, CString szBCName)
{
    CString str;
    BOOL bExit;
    LONG i, j;

    bExit = FALSE;

	if (lBlock == 0)
	{
		m_lCurrHmiMgzn	= 0;	
		m_lCurrHmiSlot	= 0;
		return TRUE;
	}

	if (IsMSAutoLineMode())
	{
		LONG lMgzn = 0, lSlot = 0;
		BOOL bRet = GetOutputSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lBlock, lMgzn, lSlot);
		if (!bRet)
		{
			bRet = GetWIPSlotWithStatus_AutoLine(BL_SLOT_USAGE_SORTING, szBCName, lBlock, lMgzn, lSlot);
		}
		if (bRet)
		{
			m_lCurrHmiMgzn	= lMgzn;	
			m_lCurrHmiSlot	= lSlot + 1;
		}
		else
		{
			m_lCurrHmiMgzn	= 0;	
			m_lCurrHmiSlot	= 0;
		}
		return TRUE;
	}

    switch( m_lOMRT )
    {
    case BL_MODE_A:		// GET LOAD
	case BL_MODE_F:		//MS100 8mag config		//v3.82
        for(i=0; i<MS_BL_MGZN_NUM; i++)
        {
            if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_ACTIVE )
                continue;
            for(j=0; j<MS_BL_MGZN_SLOT; j++)
            {
                if( m_stMgznRT[i].m_lSlotUsage[j]==BL_SLOT_USAGE_UNUSE )
                    continue;
                if( m_stMgznRT[i].m_lSlotBlock[j]==lBlock )
                {
                    bExit = TRUE;
                    break;
                }
            }
            if( bExit==TRUE )
                break;
        }
        if( i==MS_BL_MGZN_NUM || j==MS_BL_MGZN_SLOT )
        {
            return FALSE;
        }
        break;

    case BL_MODE_B:
        return TRUE;

    case BL_MODE_C: // GET LOAD
        for(i=0; i<MS_BL_MGZN_NUM; i++)
        {
            if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_MIXED )
                continue;
            for(j=0; j<MS_BL_MGZN_SLOT; j++)
            {
                if( m_stMgznRT[i].m_lSlotUsage[j]==BL_SLOT_USAGE_UNUSE )
                    continue;
                if( m_stMgznRT[i].m_lSlotBlock[j]==lBlock )
                {
                    if( m_stMgznRT[i].m_lSlotUsage[j]!=BL_SLOT_USAGE_ACTIVE )
                    {
                        continue;
                    }
                    else
                    {
                        bExit = TRUE;
                        break;
                    }
                }
            }
            if( bExit==TRUE )
                break;
        }
        if( i==MS_BL_MGZN_NUM || j==MS_BL_MGZN_SLOT )
        {
            return FALSE;
        }
        break;

    case BL_MODE_D: // GET LOAD
        for(i=0; i<MS_BL_MGZN_NUM; i++)
        {
            if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_MIXED )
                continue;
            for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
            {
                if( m_stMgznRT[i].m_lSlotUsage[j]!=BL_SLOT_USAGE_ACTIVE )
                    continue;
                if( m_stMgznRT[i].m_lSlotBlock[j]==lBlock )
                {
                    bExit = TRUE;
                    break;
                }
            }
            if( bExit==TRUE )
                break;
        }
        if( i==MS_BL_MGZN_NUM || j==MS_BL_MAX_MGZN_SLOT )
        {
            return FALSE;
        }
        break;

    case BL_MODE_E:		// GET LOAD		//v3.45
	case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
        for(i=0; i<MS_BL_MGZN_NUM; i++)
        {
            if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_ACTIVE )
                continue;
            for(j=0; j<MS_BL_MGZN_SLOT; j++)
            {
                if( m_stMgznRT[i].m_lSlotUsage[j]==BL_SLOT_USAGE_UNUSE )
                    continue;
                if( m_stMgznRT[i].m_lSlotBlock[j]==lBlock )
                {
                    bExit = TRUE;
                    break;
                }
            }
            if( bExit==TRUE )
                break;
        }
        if( i==MS_BL_MGZN_NUM || j==MS_BL_MGZN_SLOT )
        {
            return FALSE;
        }
        break;

	case BL_MODE_H:		//MS100 100 bin config	//Yealy MS100Plus
        for(i=0; i<MS_BL_MGZN_NUM; i++)
        {
            if( m_stMgznRT[i].m_lMgznUsage!=BL_MGZN_USAGE_ACTIVE )
                continue;
            for(j=0; j<MS_BL_MGZN_SLOT; j++)
            {
                if( m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_ACTIVE )
                    continue;
                if( m_stMgznRT[i].m_lSlotBlock[j]==lBlock )
                {
                    bExit = TRUE;
                    break;
                }
            }
            if( bExit==TRUE )
                break;
        }
        if( i==MS_BL_MGZN_NUM || j==MS_BL_MGZN_SLOT )
        {
            return FALSE;
        }
        break;

    default:
		return FALSE;
    }

    //m_lCurrMgzn = i;
	m_lCurrHmiMgzn	= i;	
	m_lCurrHmiSlot	= j + 1;

    return TRUE;
}

// check the frame position on the bin table
LONG CBinLoader::CheckFramePosOnBinTable(BOOL bBurnInUnload, BOOL bOffline)
{
	INT lFrameOnBT;

	lFrameOnBT = CheckFrameOnBinTable();

	if ((bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL))
	{
		lFrameOnBT = BL_FRAME_ON_CENTER;
	}

	switch (lFrameOnBT)
	{
		case BL_FRAME_NOT_EXIST: 
			SetFrameVacuum(FALSE);
			SetFrameAlign(FALSE);

			if (bOffline == TRUE)
			{
				if (BL_YES_NO(HMB_BL_PUT_FRAME_INTO_BT, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
				{
					if (BL_YES_NO(HMB_BL_UNLOAD_MANUALLY, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
					{
						return FALSE;
					}
					else
					{
						//BL_WARNBOX("UNLOAD BY HAND, SCAN BAR CODE FIRST");
						SetAlert_Red_Yellow(IDS_BL_MAN_UNLOAD);
						SetErrorMessage("BL manual unload this frame");
						return TRUE;
					}
				}
				else
				{
					SetFrameAlign(TRUE);
					Sleep(m_lBTAlignFrameDelay);
		
					SetFrameVacuum(TRUE);
				}
			}
			else
			{
				return FALSE;
			}
			break;

		case BL_FRAME_ON_RHS_EDGE: 
			SetFrameVacuum(FALSE);
			SetFrameAlign(FALSE);
			//if( BL_OK_CANCEL("Frame exists on bintable right edge only!\nPlease push it back", "Unload Frame Message")==FALSE )

			if (BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_RHS, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
			{
				return FALSE;
			}

			SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);

			SetFrameVacuum(TRUE);
			break;

		case BL_FRAME_ON_LHS_EDGE: 
			SetFrameVacuum(FALSE);
			SetFrameAlign(FALSE);
			//if( BL_OK_CANCEL("Frame exists on bintable left edge only!\nPlease push it back", "Unload Frame Message")==FALSE )
			if (BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_LHS, IDS_BL_UNLOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg) == FALSE)
			{
				return FALSE;
			}

			SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);

			SetFrameVacuum(TRUE);
			break;

		case BL_FRAME_ON_CENTER: 
			break;
	}

	return BL_FRAME_CHECK_POS_SUCCESS;
}

//================================================================================================
//  For Clear All Bin Frame use.
//  Find Full Mgz and update slot status.
//  Bin Counter is also updated
//================================================================================================
LONG CBinLoader::LoadFrameToFullMgz(CBinGripperBuffer *pGripperBuffer, LONG lLoadBlock, BOOL bBinFull, BOOL bOffline, BOOL bClearAllMode)
{
	LONG lStatus		= TRUE;
	BOOL bClearAllFrameTOP2Mode = IsMSAutoLineMode() && bClearAllMode;
	LONG lRet = 0;

	BL_DEBUGBOX("Load Frame To Full Mgz");
	lStatus = UDB_LoadFromMgzToBuffer(FALSE, lLoadBlock, pGripperBuffer);
	if (lStatus == FALSE)
	{
		SetErrorMessage("BL ManualLoadFilmFrame (UDB): LOAD from Mgzn to Buffer fails");
		return FALSE;
	}
	Sleep(200);

	//if bClearAllFrameTOP2Mode is TRUE, machine transfers the frame from WIP to TOP2 under inline-mode
	if (!bClearAllFrameTOP2Mode)
	{
		if (!LoadUnloadLockMutex())
		{
			CString szLog;

			szLog.Format("LoadUnloadLockMutex  UnloadBinFrame  = %ld", m_lLoadUnloadSECSCmdObject);
			CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
			return FALSE;
		}

		BL_DEBUGBOX("UnloadBinFrame CheckClosedALBackGate");
		lRet = CheckClosedALBackGate();
		if (lRet)
		{
			LoadUnloadUnlock();
			return lRet;
		}
	}

	//Set Magzine&Slot Number
	m_lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
	m_lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
	pGripperBuffer->SetMgznSlotNo(m_lCurrMgzn, m_lCurrSlot);
	pGripperBuffer->SetBinFull(bBinFull);

	lStatus = UDB_UnloadFromBufferToMgz(m_bBurnInTestRun, bBinFull, pGripperBuffer, TRUE, FALSE, FALSE, bClearAllFrameTOP2Mode);
	if (lStatus == FALSE)
	{
		SetErrorMessage("BL ManualUnloadFilmFrame (UDB): UNLOAD from Buffer to Mgzn fails");
		return FALSE;
	}

	return TRUE;
}