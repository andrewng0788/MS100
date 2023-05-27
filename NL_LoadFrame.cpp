/////////////////////////////////////////////////////////////////
// NL_CycleState.cpp : interface of the CNVCLoader class
//
//	Description:
//		
//	Date:		23 June 2020
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


///////////////////////////////////////////////////////////////////////////
//	NL Main LOAD UNLOAD Functions 
///////////////////////////////////////////////////////////////////////////

BOOL CNVCLoader::OpLoadWaferFrame(LONG lLoadBlock)
{
	CString szLog;

	if (m_lCurrBlock1 > 0)		//If Gripper #1 has frame
	{
		szLog.Format("Pre Unload wafer G1 to Mgzn = %ld", m_lCurrBlock1);
		HmiMessage(szLog);

		if (!UnloadWaferToMgzn(m_lCurrBlock1, TRUE))
		{
			szLog = "OpLoadWaferFrame: UnloadLastWaferToMgzn G1 fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}
	}
	if (m_lCurrBlock2 > 0)		//If Gripper #2 has frame
	{
		szLog.Format("Pre Unload wafer G2 to Mgzn = %ld", m_lCurrBlock2);
		HmiMessage(szLog);

		if (!UnloadWaferToMgzn(m_lCurrBlock2, FALSE))
		{
			szLog = "OpLoadWaferFrame: UnloadLastWaferToMgzn G2 fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}
	}


	szLog.Format("Load wafer G1 from Mgzn SLOT = %ld", lLoadBlock);
	//HmiMessage(szLog);

	if (!LoadWaferFromMgzn(lLoadBlock, TRUE))
	{
		szLog = "OpLoadWaferFrame: LoadWaferFromMgzn fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	//Unload WT Frame by gripper 2 if available
	BOOL bNeedUnload = FALSE;
	if (m_lCurrWTBlock > 0)
	{
		bNeedUnload = TRUE;

		szLog.Format("Unload wafer G1 from TABLE = %ld", m_lCurrWTBlock);
		//HmiMessage(szLog);

		if (!UnloadWaferFromWT(FALSE))
		{
			szLog = "OpLoadWaferFrame: UnloadWaferFromWT G2 fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}
	}


	szLog.Format("LOAD wafer G1 to Table SLOT = %ld", m_lCurrBlock1);
	//HmiMessage(szLog);

	//Load wafer frame finally to table
	if (!LoadWaferToWT(TRUE, FALSE, bNeedUnload))
	{
		szLog = "OpLoadWaferFrame: LoadWaferToTable G1 fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CNVCLoader::OpUnloadLastWaferFrame()
{
	CString szLog;

	if (m_lCurrBlock2 > 0)		//If Gripper #2 has frame
	{
		szLog.Format("Unload Last wafer G2 to Table SLOT = %ld", m_lCurrBlock2);
		//HmiMessage(szLog);

		if (!UnloadWaferToMgzn(m_lCurrBlock2, FALSE))
		{
			szLog = "OpUnloadLastWaferFrame: UnloadWaferToMgzn G2 fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CNVCLoader::LoadWaferFrame(LONG lLoadBlock, BOOL bUseGripper1, BOOL bToStandBy)
{
	BOOL bStatus = TRUE;
	CString szLog;


	if (!LoadWaferFromMgzn(lLoadBlock, bUseGripper1))
	{
		szLog = "LoadWaferFrame: LoadWaferFromMgzn fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	if (!LoadWaferToWT(bUseGripper1, bToStandBy))
	{
		szLog = "LoadWaferFrame: LoadWaferFromMgzn fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	return TRUE;
}


BOOL CNVCLoader::UnloadWaferFrame(CONST BOOL bUseGripper1, BOOL bToStandBy)
{
	CString szLog;

	if (!UnloadWaferFromWT(bUseGripper1))
	{
		szLog = "UnloadWaferFrame: UnloadWaferFromWT fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	LONG lUnloadBlock = m_lCurrBlock1;
	if (!bUseGripper1)
	{
		lUnloadBlock = m_lCurrBlock2;
	}

	if (!UnloadWaferToMgzn(lUnloadBlock, bUseGripper1, bToStandBy))
	{
		szLog.Format("UnloadWaferFrame: UnloadWaferToMgzn (%ld) fail!", lUnloadBlock);
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}
	
	return TRUE;
}

BOOL CNVCLoader::LoadWaferFromMgzn(LONG lLoadBlock, BOOL bUseGripper1)
{
	BOOL bStatus = TRUE;
	CString szLog;


	//////////////////////////////////////////////////
	// Detect if frame existed on gripper before LOAD
	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (IsGripper1FrameExist())
			{
				Sleep(500);
				if (IsGripper1FrameExist())
				{
					szLog = "LoadWaferFrame: Waf gripper detects frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetWafGripper(TRUE);
					//Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (IsGripper2FrameExist())
			{
				Sleep(500);
				if (IsGripper2FrameExist())
				{
					szLog = "LoadWaferFrame: Bin gripper detects frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetBinGripper(TRUE);
					//Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}


	/////////////////////////////////////////////
	// Get current wafer MGZN Slot
	LONG lMgzn = 0, lSlot = 0;
	bStatus = GetCurrWafMgzSlot(lLoadBlock, lMgzn, lSlot);
	if (!bStatus)
	{
		HmiMessage("LoadWaferFrame: Wafer Slot is not found !");
		return FALSE;
	}

	/////////////////////////////////////////////
	// Move Loader XY to MGZN
	bStatus = MoveXYToMgzn(lMgzn, lSlot, bUseGripper1, TRUE);
	if (!bStatus)
	{
		szLog = "LoadWaferFrame: Load XY fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	/////////////////////////////////////////////
	// Move Gripper Down to Mgzn
	//Sleep(500);
	MoveZDownToMgzn(lMgzn, lSlot, TRUE, bUseGripper1);
	Sleep(100);
	if (bUseGripper1)
	{
		SetWafGripper(FALSE);
	}
	else
	{
		SetBinGripper(FALSE);
	}
	Sleep(200);


	//////////////////////////////////////////////////
	// Detect if frame existed on gripper BEFORE LOAD
	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "LoadWaferFrame: Gripper 1 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetWafGripper(TRUE);
					Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "LoadWaferFrame: Gripper 2 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetBinGripper(TRUE);
					Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}


	MoveZUpToHome(bUseGripper1);
	//Sleep(500);


	//////////////////////////////////////////////////
	// Detect if frame existed on gripper after LOAD
	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					//SetWafGripper(TRUE);
					szLog = "LoadWaferFrame: Gripper 1 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					//SetBinGripper(TRUE);
					szLog = "LoadWaferFrame: Gripper 2 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}


	/////////////////////////////////////////////
	// Update parameters
	m_lCurrWafSlotID	= lLoadBlock;
	if (bUseGripper1)
	{
		m_lCurrMgzn1	= lMgzn;
		m_lCurrSlotID1	= lSlot;
		m_lCurrBlock1	= m_lCurrWafSlotID;
		
		szLog.Format("NL LoadWaferFromMgzn: LOAD Wafer (BLOCK=%ld, MGZN=%ld, SLOT=%ld) Done to Gripper 1", m_lCurrWafSlotID, lMgzn, lSlot);
	}
	else
	{
		m_lCurrMgzn2	= lMgzn;
		m_lCurrSlotID2	= lSlot;
		m_lCurrBlock2	= m_lCurrWafSlotID;
		
		szLog.Format("NL LoadWaferFromMgzn: LOAD Wafer (BLOCK=%ld, MGZN=%ld, SLOT=%ld) Done to Gripper 2", m_lCurrWafSlotID, lMgzn, lSlot);
	}

	SaveData();

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return TRUE;
}

BOOL CNVCLoader::LoadWaferToWT(BOOL bUseGripper1, BOOL bToStandBy, BOOL bAutoBond)
{
	CString szLog;

	/////////////////////////////////////////////
	// Move Loader XY to MGZN
	BOOL bStatus = MoveXYToWaferTable(bUseGripper1, TRUE, FALSE);
	if (!bStatus)
	{
		szLog = "LoadWaferFrame: Load XY 2 fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	
	INT nConvID1 = 0;
	bStatus = LoadUnloadWafExpander(TRUE, nConvID1, FALSE);
	if (!bStatus)
	{
		szLog = "LoadWaferToWT: Unload Wafer Expander fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Wait for XY motion completeion before allowing Z down
	X_Sync();
	Y_Sync();
	Sleep(100);
	if (!IsXYEncoderWithinRange())
	{
		return FALSE;
	}

	bStatus = LoadUnloadWafExpander_Sync(TRUE, nConvID1);
	if (!bStatus)
	{
		szLog = "LoadWaferToWT: Unload Wafer Expander SYNC fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Move Gripper Down to Table to LOAD a frame
	Sleep(500);
	MoveZDownToWafTable(bUseGripper1, TRUE);
	Sleep(100);
	if (bUseGripper1)
	{
		SetWafGripper(TRUE);
	}
	else
	{
		SetBinGripper(TRUE);
	}
	Sleep(100);

	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "LoadWaferFrame: Waf gripper cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetWafGripper(TRUE);
					//Z1_MoveTo(m_lStandByPos_Z1);
					Z1_Home();
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "LoadWaferFrame: Bin gripper cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetBinGripper(TRUE);
					//Z2_MoveTo(m_lStandByPos_Z2);
					Z2_Home();
					return FALSE;
				}
			}
		}
	}


	/////////////////////////////////////////////
	// Move Gripper UP to STANDBY level
	//MoveZUpToStandby(bUseGripper1);
	MoveZUpToHome(bUseGripper1);
	//Sleep(500);


	/////////////////////////////////////////////
	// Wafer Table to HOME
	if (!bAutoBond)		//If OFFLINE/SETUP mode, must close expander first
	{
		INT nConvID = 0;
		bStatus = LoadUnloadWafExpander(FALSE, nConvID, FALSE);
		if (!bStatus)
		{
			szLog = "LoadWaferFrame: Load Wafer Expander fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}


		if (bToStandBy)
		{
			MoveXYToStandby(FALSE);
		}

		bStatus = LoadUnloadWafExpander_Sync(FALSE, nConvID);
		if (!bStatus)
		{
			szLog = "LoadWaferFrame: Load Wafer Expander SYNC fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}

		if (bToStandBy)
		{
			X_Sync();
			Y_Sync();
			Sleep(100);
			if (!IsXYEncoderWithinRange())
			{
				return FALSE;
			}
		}
	}


	/////////////////////////////////////////////
	// Update WaferTable and NVCLoader status
	if (bUseGripper1)
	{
		m_lCurrWTBlock = m_lCurrBlock1;
		//m_lCurrBTBlock	= m_lCurrBlock1;
		//m_lCurrBTMgzn		= m_lCurrMgzn1;
		//m_lCurrBTSlotID	= m_lCurrSlotID1;
		m_lCurrMgzn1	= 0;
		m_lCurrSlotID1	= 0;
		m_lCurrBlock1	= 0;
	}
	else
	{
		//m_lCurrBTBlock	= m_lCurrBlock2;
		//m_lCurrBTMgzn		= m_lCurrMgzn2;
		//m_lCurrBTSlotID	= m_lCurrSlotID2;
		m_lCurrWTBlock = m_lCurrBlock2;
		m_lCurrMgzn2	= 0;
		m_lCurrSlotID2	= 0;
		m_lCurrBlock2	= 0;
	}

	//SaveData();
	return TRUE;
}

BOOL CNVCLoader::UnloadWaferFromWT(BOOL bUseGripper1)
{
	BOOL bStatus = TRUE;
	CString szLog;


	/////////////////////////////////////////////
	// Open gripper before operation
	if (bUseGripper1)
	{
		SetWafGripper(TRUE);
	}
	else
	{
		SetBinGripper(TRUE);
	}


	bStatus = MoveXYToWaferTable(bUseGripper1, FALSE, FALSE);
	if (!bStatus)
	{
		szLog = "UnloadWaferFrame: Unload XY fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Unload Bin expander 
	INT nConvID1 = 0;
	bStatus = LoadUnloadWafExpander(TRUE, nConvID1, TRUE);
	if (!bStatus)
	{
		szLog = "UnloadWaferFrame: Unload Bin Expander fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Wait for XY motion completeion before allowing Z down
	X_Sync();
	Y_Sync();
	Sleep(100);
	if (!IsXYEncoderWithinRange())
	{
		return FALSE;
	}


	/////////////////////////////////////////////
	// Move Gripper Down to UNLOAD level
	/////////////////////////////////////////////
	// Move Gripper Down to Mgzn
	Sleep(500);
	MoveZDownToWafTable(bUseGripper1, FALSE);
	Sleep(100);
	if (bUseGripper1)
	{
		SetWafGripper(FALSE);
	}
	else
	{
		SetBinGripper(FALSE);
	}
	Sleep(100);


	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "UnloadWaferFrame: Gripper 1 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetWafGripper(TRUE);
					Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "UnloadWaferFrame: Gripper 2 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetBinGripper(TRUE);
					Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}


	/////////////////////////////////////////////
	// Move Gripper UP to STANDBY level
	MoveZUpToHome(bUseGripper1);
	//Sleep(500);


	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "UnloadWaferFrame: Gripper 1 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//SetWafGripper(TRUE);
					//Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "UnloadWaferFrame: Gripper 2 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//SetBinGripper(TRUE);
					//Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}


	/////////////////////////////////////////////
	// Update NVCLoader status 
	if (bUseGripper1)
	{
		m_lCurrMgzn1		= 0;
		m_lCurrSlotID1		= 0;
		m_lCurrBlock1		= m_lCurrWTBlock;
	}
	else
	{
		m_lCurrMgzn2		= 0;
		m_lCurrSlotID2		= 0;
		m_lCurrBlock2		= m_lCurrWTBlock;
	}

	m_lCurrWTBlock = 0;

	return TRUE;
}

BOOL CNVCLoader::UnloadWaferToMgzn(LONG lUnloadBlock, BOOL bUseGripper1, BOOL bToStandBy)
{
	CString szLog;

	/////////////////////////////////////////////
	// Get current wafer MGZN Slot
	LONG lMgzn = 0, lSlot = 0;
	BOOL bStatus = GetCurrWafMgzSlot(lUnloadBlock, lMgzn, lSlot);
	if (!bStatus)
	{
		szLog.Format("UnloadWaferToMgzn: Wafer Slot is not found (SLOT=%d)", lUnloadBlock);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// HOME Wafer expander 
	INT nConvID = 0;
	bStatus = LoadUnloadWafExpander(FALSE, nConvID, FALSE);
	if (!bStatus)
	{
		szLog = "UnloadWaferToMgzn: HOME Wafer Expander fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	Sleep(200);


	/////////////////////////////////////////////
	// Move Loader XY to MGZN
	bStatus = MoveXYToMgzn(lMgzn, lSlot, bUseGripper1, FALSE);
	if (!bStatus)
	{
		szLog = "UnloadWaferToMgzn: Load XY fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	
	/////////////////////////////////////////////
	// UNload Wafer Expander
	bStatus = LoadUnloadWafExpander_Sync(FALSE, nConvID);
	if (!bStatus)
	{
		szLog = "UnloadWaferToMgzn: Reload Wafer Expander SYNC fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Move Gripper Down to Mgzn
	//Sleep(500);
	MoveZDownToMgzn(lMgzn, lSlot, FALSE, bUseGripper1);
	Sleep(100);
	if (bUseGripper1)
	{
		SetWafGripper(TRUE);	
	}
	else
	{
		SetBinGripper(TRUE);	
	}
	Sleep(100);
	MoveZUpToHome(bUseGripper1);
	//Sleep(500);


	/////////////////////////////////////////////
	// Move XY to STANDBY pos
	if (bToStandBy)
	{
		MoveXYToStandby(TRUE);
	}


	/////////////////////////////////////////////
	// Update NVCLoader status 
	if (bUseGripper1)
	{
		//m_lCurrWafSlotID	= m_lCurrBlock1;
		m_lCurrMgzn1		= 0;
		m_lCurrSlotID1		= 0;
		m_lCurrBlock1		= 0;
		
		szLog.Format("NL UnloadWaferToMgzn: UNLOAD Wafer (BLOCK=%ld, MGZN=%ld, SLOT=%ld) Done by Gripper 1", m_lCurrWafSlotID, lMgzn, lSlot);
	}
	else
	{
		//m_lCurrWafSlotID	= m_lCurrBlock2;
		m_lCurrMgzn2		= 0;
		m_lCurrSlotID2		= 0;
		m_lCurrBlock2		= 0;
		
		szLog.Format("NL UnloadWaferToMgzn: UNLOAD Wafer (BLOCK=%ld, MGZN=%ld, SLOT=%ld) Done by Gripper 2", m_lCurrWafSlotID, lMgzn, lSlot);
	}

	SaveData();

	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////
//	NL Bin Main LOAD UNLOAD Functions 
///////////////////////////////////////////////////////////////////////////

BOOL CNVCLoader::OpLoadBinFrame(LONG lLoadBlock)
{
	CString szLog;

	szLog.Format("Load Bin G1 from Mgzn SLOT = %ld", lLoadBlock);
	//HmiMessage(szLog);

	if (!LoadBinFrameFromMgzn(lLoadBlock, TRUE))
	{
		szLog = "OpLoadBinFrame: LoadBinFrameFromMgzn fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	//Unload WT Frame by gripper 2 if available
	BOOL bNeedUnload = FALSE;
	if (m_lCurrBTBlock > 0)
	{
		bNeedUnload = TRUE;

		szLog.Format("Unload Bin G1 from TABLE = %ld", m_lCurrBTBlock);
		//HmiMessage(szLog);

		if (!UnloadBinFrameFromTable(FALSE))
		{
			szLog = "OpLoadBinFrame: UnloadBinFrameFromTable G2 fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}
	}


	szLog.Format("LOAD Bin G1 to Table SLOT = %ld", m_lCurrBlock1);
	//HmiMessage(szLog);

	//Load wafer frame finally to table
	if (!LoadBinFrameToTable(TRUE, !bNeedUnload))
	{
		szLog = "OpLoadBinFrame: LoadBinFrameToTable G1 fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}

	return TRUE;
}


BOOL CNVCLoader::OpUnloadLastBinFrame()
{
	CString szLog;

	if (m_lCurrBlock2 > 0)		//If Gripper #2 has frame
	{
		szLog.Format("Unload Last Bin G2 to MGZN SLOT = %ld", m_lCurrBlock2);
		//HmiMessage(szLog);

		if (!UnloadBinFrameToMgzn(FALSE))
		{
			szLog = "OpUnloadLastBinFrame: UnloadBinFrameToMgzn G2 fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CNVCLoader::LoadBinFrameFromMgzn(LONG lLoadBlock, BOOL bUseGripper1)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bStatus = TRUE;
	CString szLog;


	if (bUseGripper1)
	{
		if (m_lCurrBlock1 != 0)
		{
			szLog.Format("ERR: gripper 1 already has FRAME ID #%ld (LOAD = %ld); please clear it and try again.", 
								m_lCurrBlock1, lLoadBlock);
			SetErrorMessage(szLog);
			HmiMessage_Red_Yellow(szLog, "NL load Bin Frame");
			return FALSE;
		}

		SetWafGripper(TRUE);
	}
	else
	{
		if (m_lCurrBlock2 != 0)
		{
			szLog.Format("ERR: gripper 2 already has FRAME ID #%ld (LOAD = %ld); please clear it and try again.", 
								m_lCurrBlock2, lLoadBlock);
			SetErrorMessage(szLog);
			HmiMessage_Red_Yellow(szLog, "NL load Bin Frame");
			return FALSE;
		}

		SetBinGripper(TRUE);
	}


	BOOL bEmpty = FALSE;
	LONG lMgzn = 0;
	LONG lSlot = 0;
	bStatus = GetLoadMgzSlot(lMgzn, lSlot, bEmpty, lLoadBlock);
	if (!bStatus)
	{
		szLog.Format("NL: Fail to get grade slot for Bin #%ld (%d, %d, %d)", lLoadBlock, lMgzn, lSlot, bEmpty);
		HmiMessage_Red_Yellow(szLog, "NL load Bin Frame");
		return FALSE;
	}


	if (bUseGripper1)
	{
		bStatus = MoveXYToMgzn(lMgzn, lSlot, TRUE, TRUE);
	}
	else
	{
		bStatus = MoveXYToMgzn(lMgzn, lSlot, FALSE, TRUE);
	}

	if (!bStatus)
	{
		szLog.Format("NL: Fail to MOVE XY Table to UNLOAD for Bin #%ld", lLoadBlock);
		HmiMessage_Red_Yellow(szLog, "NL load Bin Frame");
		return FALSE;
	}


	//Sleep(100);
	MoveZDownToMgzn(lMgzn, lSlot, TRUE, bUseGripper1);
	Sleep(100);
	
	if (bUseGripper1)	
	{
		SetWafGripper(FALSE);	//CLOSE clamp
	}
	else
	{
		SetBinGripper(FALSE);	//CLOSE clamp
	}
	

	//////////////////////////////////////////////////
	// Detect if frame existed on gripper before LOAD
	if (!IsGripperHasFrame(bUseGripper1, TRUE))
	{
		return FALSE;
	}
	/*if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "LoadBinFrameFromMgzn: Gripper 1 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					
					SetWafGripper(TRUE);	//OPEN clamp
					Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "LoadBinFrameFromMgzn: Gripper 2 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					
					SetBinGripper(TRUE);	//OPEN clamp
					Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}*/


	Sleep(200);
	MoveZUpToHome(bUseGripper1);
	//Sleep(100);


	//////////////////////////////////////////////////
	// Detect if frame existed on gripper before LOAD
	if (!IsGripperHasFrame(bUseGripper1, FALSE))
	{
		return FALSE;
	}
	/*if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "LoadBinFrameFromMgzn: Gripper 1 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "LoadBinFrameFromMgzn: Gripper 2 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}*/


	if (bUseGripper1)
	{
		m_lCurrMgzn1	= lMgzn;
		m_lCurrSlotID1	= lSlot;
		m_lCurrBlock1	= lLoadBlock;
	}
	else
	{
		m_lCurrMgzn2	= lMgzn;
		m_lCurrSlotID2	= lSlot;
		m_lCurrBlock2	= lLoadBlock;
	}
	
	UpdateHmiMgznSlotID();
	return TRUE;
}

BOOL CNVCLoader::LoadBinFrameToTable(BOOL bUseGripper1, BOOL bToStandBy)
{
	CString szLog;
	BOOL bStatus = TRUE;

	/*if (m_lCurrBTBlock != 0)
	{
		if (bUseGripper1)
		{
			szLog.Format("ERR: Bin Table already has FRAME ID #%ld (LOAD G1 = %ld); please clear it and try again.", 
								m_lCurrBTBlock, m_lCurrBlock1);
		}
		else
		{
			szLog.Format("ERR: Bin Table already has FRAME ID #%ld (LOAD G2 = %ld); please clear it and try again.", 
								m_lCurrBTBlock, m_lCurrBlock2);
		}

		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog, "NL load Bin Frame");
		return FALSE;
	}*/


	//////////////////////////////////////////////////
	// Detect if frame existed on gripper before LOAD
	if (!IsGripperHasFrame(bUseGripper1, FALSE))
	{
		return FALSE;
	}
	/*if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "LoadBinFrameToTable: Gripper 1 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//SetWafGripper(TRUE);
					//Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "LoadBinFrameToTable: Gripper 2 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//SetBinGripper(TRUE);
					//Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}*/


	/////////////////////////////////////////////
	// Move Loader XY to LOAD
	bStatus = MoveXYToBinTable(bUseGripper1, TRUE, FALSE);		//andrewng //2020-0904
	if (!bStatus)
	{
		szLog = "LoadBinFrameToTable: Unload XY fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}
	Sleep(100);


	INT nConvID1 = 0;
	bStatus = LoadUnloadBinExpander(TRUE, nConvID1, TRUE);
	if (!bStatus)
	{
		szLog = "LoadBinFrameToTable: Unload Bin Expander fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Wait for XY motion completeion before allowing Z down
	X_Sync();
	Y_Sync();
		
	Sleep(100);
	if (!IsXYEncoderWithinRange())
	{
		return FALSE;
	}

	/////////////////////////////////////////////
	// Move Gripper Down to UNLOAD level
	if (bUseGripper1)
	{
		Z1_MoveTo(m_lBinLoadPos_Z1);
	}
	else
	{
		Z2_MoveTo(m_lBinLoadPos_Z2);
	}


	/////////////////////////////////////////////
	// Grip frame 
	Sleep(100);
	if (bUseGripper1)	
	{
		SetWafGripper(TRUE);	//Open clamp
	}
	else			
	{
		SetBinGripper(TRUE);	//Open clamp
	}
	Sleep(100);


	if (!IsGripperHasFrame(bUseGripper1, TRUE))
	{
		return FALSE;
	}
	/*if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "LoadBinFrameToTable: Gripper 1 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetWafGripper(TRUE);
					Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "LoadBinFrameToTable: Gripper 2 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetBinGripper(TRUE);
					Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}*/


	/////////////////////////////////////////////
	// Move Gripper UP to STANDBY level
	if (bUseGripper1)
	{
		Z1_MoveTo(m_lStandByPos_Z1);
	}
	else
	{
		Z2_MoveTo(m_lStandByPos_Z2);
	}



	//andrewng //2020-0904
	if (bToStandBy)
	{
		/////////////////////////////////////////////
		// Reload BinTable to HOME
		INT nConvID = 0;
		bStatus = LoadUnloadBinExpander(FALSE, nConvID, FALSE);
		if (!bStatus)
		{
			szLog = "LoadBinFrameToTable: Load Bin Expander fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}

		Sleep(100);
		MoveXYToStandby(FALSE);

		bStatus = LoadUnloadBinExpander_Sync(FALSE, nConvID);
		if (!bStatus)
		{
			szLog = "LoadBinFrameToTable: Load Bin Expander SYNC fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}

		X_Sync();
		Y_Sync();
	}


	/////////////////////////////////////////////
	// Update BinTable and NVCLoader status
	if (bUseGripper1)
	{
		m_lCurrBTBlock	= m_lCurrBlock1;
		m_lCurrBTMgzn	= m_lCurrMgzn1;
		m_lCurrBTSlotID	= m_lCurrSlotID1;
		m_lCurrMgzn1	= 0;
		m_lCurrSlotID1	= 0;
		m_lCurrBlock1	= 0;
	}
	else
	{
		m_lCurrBTBlock	= m_lCurrBlock2;
		m_lCurrBTMgzn	= m_lCurrMgzn2;
		m_lCurrBTSlotID	= m_lCurrSlotID2;
		m_lCurrMgzn2	= 0;
		m_lCurrSlotID2	= 0;
		m_lCurrBlock2	= 0;
	}

	return TRUE;
}

BOOL CNVCLoader::UnloadBinFrameFromTable(CONST BOOL bUseGripper1, BOOL bToStandBy)
{
	CString szLog;
	BOOL bStatus = TRUE;


	if (m_lCurrBTBlock == 0)
	{
		szLog.Format("ERR: Bin Table has NO FRAME ID #%ld to be UNLOAD; please check.", m_lCurrBTBlock);

		SetErrorMessage(szLog);
		HmiMessage_Red_Yellow(szLog, "NL load Bin Frame");
		return FALSE;
	}


	//////////////////////////////////////////////////
	// Detect if frame existed on gripper before LOAD
	if (!IsGripperHasNoFrame(bUseGripper1))
	{
		return FALSE;
	}


	/////////////////////////////////////////////
	// Open gripper before operation
	if (bUseGripper1)
	{
		SetWafGripper(TRUE);
	}
	else
	{
		SetBinGripper(TRUE);
	}


	bStatus = MoveXYToBinTable(bUseGripper1, FALSE, FALSE);		//andrewng //2020-0904
	if (!bStatus)
	{
		szLog = "UnloadBinFrameFromTable: Unload XY fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}
	Sleep(200);


	/////////////////////////////////////////////
	// Unload Bin expander 
	INT nConvID1 = 0;
	bStatus = LoadUnloadBinExpander(TRUE, nConvID1, FALSE);
	if (!bStatus)
	{
		szLog = "UnloadBinFrameFromTable: Unload Bin Expander fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Wait for XY motion completeion before allowing Z down
	X_Sync();
	Y_Sync();
	
	Sleep(100);
	if (!IsXYEncoderWithinRange())
	{
		return FALSE;
	}


	/////////////////////////////////////////////
	// Unload Bin expander SYNC
	bStatus = LoadUnloadBinExpander_Sync(TRUE, nConvID1);
	if (!bStatus)
	{
		szLog = "UnloadBinFrameFromTable: Unload Bin Expander SYNC fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	/////////////////////////////////////////////
	// Move Gripper Down to UNLOAD level
	if (bUseGripper1)
	{
		Z1_MoveTo(m_lBinLoadPos_Z1);
	}
	else
	{
		Z2_MoveTo(m_lBinLoadPos_Z2);
	}


	/////////////////////////////////////////////
	// Grip frame 
	//Sleep(200);
	if (bUseGripper1)	
	{
		SetWafGripper(FALSE);
	}
	else			
	{
		SetBinGripper(FALSE);
	}
	Sleep(200);


	if (!IsGripperHasFrame(bUseGripper1, TRUE))
	{
		return FALSE;
	}
	/*if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "UnloadBinFrameFromTable: Gripper 1 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetWafGripper(TRUE);
					Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "UnloadBinFrameFromTable: Gripper 2 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					SetBinGripper(TRUE);
					Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}*/


	/////////////////////////////////////////////
	// Move Gripper UP to Loader
	if (bUseGripper1)
	{
		Z1_MoveTo(0);
	}
	else
	{
		Z2_MoveTo(0);
	}


	if (!IsGripperHasFrame(bUseGripper1, FALSE))
	{
		return FALSE;
	}
	/*if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "UnloadBinFrameFromTable: Gripper 1 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//SetWafGripper(TRUE);
					//Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "UnloadBinFrameFromTable: Gripper 2 cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//SetBinGripper(TRUE);
					//Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}*/


	//andrewng //2020-0904
	if (bToStandBy)
	{
		/////////////////////////////////////////////
		// Reload Bin expander 
		INT nConvID = 0;
		bStatus = LoadUnloadBinExpander(FALSE, nConvID, FALSE);
		if (!bStatus)
		{
			szLog = "UnloadBinFrameFromTable: Reload Bin Expander fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}

		Sleep(200);
		MoveXYToStandby(FALSE);
		//Sleep(100);
		//Y_MoveTo(0, SFM_NOWAIT);
		//Sleep(10);
		//X_MoveTo(20000, SFM_NOWAIT);

		bStatus = LoadUnloadBinExpander_Sync(FALSE, nConvID);
		if (!bStatus)
		{
			szLog = "UnloadBinFrameFromTable: Reload Bin Expander SYNC fail!";
			SetErrorMessage(szLog);
			HmiMessage(szLog);
			return FALSE;
		}

		X_Sync();
		Y_Sync();
		Sleep(100);

		if (!IsXYEncoderWithinRange())
		{
			return FALSE;
		}
	}


	/////////////////////////////////////////////
	// Update NVCLoader status 
	if (bUseGripper1)
	{
		m_lCurrMgzn1	= m_lCurrBTMgzn;
		m_lCurrSlotID1	= m_lCurrBTSlotID;
		m_lCurrBlock1	= m_lCurrBTBlock;
	}
	else
	{
		m_lCurrMgzn2	= m_lCurrBTMgzn;
		m_lCurrSlotID2	= m_lCurrBTSlotID;
		m_lCurrBlock2	= m_lCurrBTBlock;
	}
	

	/////////////////////////////////////////////
	// Update BinTable status 
	m_lCurrBTBlock	= 0;
	m_lCurrBTMgzn	= 0;
	m_lCurrBTSlotID	= 0;
	UpdateHmiMgznSlotID();
	return TRUE;
}


BOOL CNVCLoader::UnloadBinFrameToMgzn(CONST BOOL bUseGripper1)
{
	BOOL bStatus = TRUE;
	CString szLog;


	if (bUseGripper1)
	{
		if (m_lCurrBlock1 == 0)
		{
			szLog.Format("ERR: gripper 1 has NO FRAME ID #%ld (Mgzn=%ld, Slot=%ld); please check.", 
								m_lCurrBlock1, m_lCurrMgzn1 + 1, m_lCurrSlotID1 + 1);
			SetErrorMessage(szLog);
			HmiMessage_Red_Yellow(szLog, "NL Unload Bin Frame");
			return FALSE;
		}
	}
	else
	{
		if (m_lCurrBlock2 == 0)
		{
			szLog.Format("ERR: gripper 2 has NO FRAME ID #%ld (Mgzn=%ld, Slot=%ld); please check.", 
								m_lCurrBlock2, m_lCurrMgzn2 + 1, m_lCurrSlotID2 + 1);
			SetErrorMessage(szLog);
			HmiMessage_Red_Yellow(szLog, "NL Unload Bin Frame");
			return FALSE;
		}
	}


	if (!IsGripperHasFrame(bUseGripper1, FALSE))
	{
		return FALSE;
	}
	/*if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					SetWafGripper(TRUE);
					szLog = "UnloadBinFrameToMgzn: Waf gripper cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//Z1_MoveTo(m_lStandByPos_Z1);
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					SetWafGripper(TRUE);
					szLog = "UnloadBinFrameToMgzn: Bin gripper cannot detect any frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					//Z2_MoveTo(m_lStandByPos_Z2);
					return FALSE;
				}
			}
		}
	}*/


	LONG lMgzn = 0;
	LONG lSlot = 0;

	if (bUseGripper1)
	{
		bStatus = GetMgznSlot(m_lCurrBlock1, lMgzn, lSlot);
		if (!bStatus)
		{
			szLog.Format("NL: UNLOAD Gripper1 - fail  to get grade slot for Bin #%ld (Mgzn=%ld, Slot=%ld)", 
									m_lCurrBlock1, lMgzn + 1, lSlot + 1);
			HmiMessage_Red_Yellow(szLog, "NL Unload Bin Frame");
			return FALSE;
		}
	}
	else
	{
		bStatus = GetMgznSlot(m_lCurrBlock2, lMgzn, lSlot);
		if (!bStatus)
		{
			szLog.Format("NL: UNLOAD Gripper2 - fail  to get grade slot for Bin #%ld (Mgzn=%ld, Slot=%ld)", 
									m_lCurrBlock2, lMgzn + 1, lSlot + 1);
			HmiMessage_Red_Yellow(szLog, "NL Unload Bin Frame");
			return FALSE;
		}
	}


	INT nConvID = 0;
	bStatus = LoadUnloadBinExpander(FALSE, nConvID, FALSE);
	if (!bStatus)
	{
		szLog = "UnloadBinFrameToMgzn: UNLOAD Bin Expander fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}
	Sleep(100);


	bStatus = MoveXYToMgzn(lMgzn, lSlot, bUseGripper1, FALSE);
	if (!bStatus)
	{
		szLog.Format("NL: Fail to MOVE XY Table to UNLOAD for Bin #%ld (Mgzn=%ld(%ld), Slot=%ld(%ld))", 
								m_lCurrBlock1, m_lCurrMgzn1 + 1, lMgzn + 1, m_lCurrSlotID1 + 1, lSlot + 1);
		HmiMessage_Red_Yellow(szLog, "NL Unload Bin Frame");
		return FALSE;
	}


	bStatus = LoadUnloadBinExpander_Sync(FALSE, nConvID);
	if (!bStatus)
	{
		szLog = "UnloadBinFrameToMgzn: UNLOAD Bin Expander SYNC fail!";
		SetErrorMessage(szLog);
		HmiMessage(szLog);
		return FALSE;
	}


	//Sleep(500);
	MoveZDownToMgzn(lMgzn, lSlot, FALSE, bUseGripper1);
	Sleep(100);

	if (bUseGripper1)	
	{
		SetWafGripper(TRUE);
	}
	else
	{
		SetBinGripper(TRUE);
	}
		
	Sleep(200);
	MoveZUpToStandby(bUseGripper1);
	//Sleep(500);


	if (bUseGripper1)
	{
		UpdateMgznSlotStatus(FALSE, m_lCurrMgzn1, m_lCurrSlotID1, m_lCurrBlock1);
		m_lCurrMgzn1	= 0;
		m_lCurrSlotID1	= 0;
		m_lCurrBlock1	= 0;
	}
	else
	{
		UpdateMgznSlotStatus(FALSE, m_lCurrMgzn2, m_lCurrSlotID2, m_lCurrBlock2);
		m_lCurrMgzn2	= 0;
		m_lCurrSlotID2	= 0;
		m_lCurrBlock2	= 0;
	}
	
	UpdateHmiMgznSlotID();
	return TRUE;
}



///////////////////////////////////////////////////////////////////////////
//	NL LOAD UNLOAD Supporting Functions 
///////////////////////////////////////////////////////////////////////////

BOOL CNVCLoader::LoadUnloadWafExpander(BOOL bLoad, INT &nConvID, BOOL bWait)
{
	IPC_CServiceMessage stMsg;
	//INT nConvID = 0;
	BOOL bON = FALSE;
	BOOL bResult = TRUE;


	if (bLoad)
	{
		bON = TRUE;
		stMsg.InitMessage(sizeof(BOOL), &bON);
		nConvID = m_comClient.SendRequest("WaferTableStn", "MoveToUnload", stMsg);
		
		if (bWait)
		{
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (bResult)
			{
				Sleep(500);
				SetWafExpanderOpen(TRUE);
				m_bIsWafExpanderOpen = TRUE;
			}
		}
	}
	else
	{
		SetWafExpanderOpen(FALSE);
		Sleep(500);

		bON = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bON);
		nConvID = m_comClient.SendRequest("WaferTableStn", "MoveToUnload", stMsg);
		
		if (bWait)
		{
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			m_bIsWafExpanderOpen = FALSE;
		}
		else
		{
			Sleep(50);
		}
	}

	return bResult;
}

BOOL CNVCLoader::LoadUnloadWafExpander_Sync(BOOL bLoad, INT nConvID)
{
	IPC_CServiceMessage stMsg;
	//INT nConvID = 0;
	BOOL bON = FALSE;
	BOOL bResult = TRUE;

	if (nConvID == 0)
	{
		return TRUE;
	}

	if (bLoad)
	{
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult)
		{
			Sleep(500);
			SetWafExpanderOpen(TRUE);
			m_bIsWafExpanderOpen = TRUE;
		}
	}
	else
	{
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		m_bIsWafExpanderOpen = FALSE;
	}

	return bResult;
}

BOOL CNVCLoader::LoadUnloadBinExpander(BOOL bLoad, INT &nConvID, BOOL bWait)
{
	IPC_CServiceMessage stMsg;
	//INT nConvID = 0;
	BOOL bON = FALSE;
	BOOL bResult = TRUE;


	if (bLoad)
	{
		bON = TRUE;
		stMsg.InitMessage(sizeof(BOOL), &bON);
		nConvID = m_comClient.SendRequest("BinTableStn", "MoveToUnload", stMsg);
		
		if (bWait)
		{
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			if (bResult)
			{
				Sleep(500);
				SetBinExpanderOpen(TRUE);
				m_bIsBinExpanderOpen = TRUE;
			}
		}
	}
	else
	{
		SetBinExpanderOpen(FALSE);
		Sleep(500);

		bON = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bON);
		nConvID = m_comClient.SendRequest("BinTableStn", "MoveToUnload", stMsg);
		
		if (bWait)
		{
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bResult);
					break;
				}
				else
				{
					Sleep(10);
				}
			}

			m_bIsBinExpanderOpen = FALSE;
		}
		else
		{
			Sleep(50);
		}
	}

	return TRUE;
}

BOOL CNVCLoader::LoadUnloadBinExpander_Sync(BOOL bLoad, INT nConvID)
{
	IPC_CServiceMessage stMsg;
	//INT nConvID = 0;
	BOOL bON = FALSE;
	BOOL bResult = TRUE;


	if (nConvID == 0)
	{
		return TRUE;
	}

	if (bLoad)
	{
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (bResult)
		{
			Sleep(500);
			SetBinExpanderOpen(TRUE);
			m_bIsBinExpanderOpen = TRUE;
		}
	}
	else
	{
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bResult);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		m_bIsBinExpanderOpen = FALSE;
	}

	return bResult;
}

BOOL CNVCLoader::IsWafExpanderOpen()
{
	return m_bIsWafExpanderOpen;
}

BOOL CNVCLoader::IsBinExpanderOpen()
{
	return m_bIsBinExpanderOpen;
}


BOOL CNVCLoader::MoveXYToMgzn(LONG lMgzn, LONG lSlot, BOOL bUseGripper1, BOOL bLoad)
{
	CString szLog;

	if (lMgzn >= MS_BL_MGZN_NUM)
	{
		szLog.Format("MoveXYToMgzn: invalid MGZN index - %ld %ld", lMgzn, MS_BL_MGZN_NUM);
		HmiMessage(szLog);
		return FALSE;
	}
	if ( lSlot > (LONG)(m_stNVCMgznRT[lMgzn].m_lNoOfSlots - 1) )
	{
		szLog.Format("MoveXYToMgzn: invalid SLOT index - %ld %ld", lSlot, (LONG)(m_stNVCMgznRT[lMgzn].m_lNoOfSlots - 1));
		HmiMessage(szLog);
		return FALSE;
	}

	LONG lSlotIndex = lSlot;

	if (m_fHardware && !m_bDisableNL)
	{
		LONG lPosX = m_stNVCMgznRT[lMgzn].m_lTopLevel + m_stNVCMgznRT[lMgzn].m_lSlotPitch * lSlotIndex;
		if (!bUseGripper1)
		{
			lPosX = m_stNVCMgznRT[lMgzn].m_lTopLevel2 + m_stNVCMgznRT[lMgzn].m_lSlotPitch * lSlotIndex;
		}

		LONG lPosY = m_stNVCMgznRT[lMgzn].m_lMidPosnY;


		if (bLoad && (m_lMgznLoadOffsetX != 0) && (labs(m_lMgznLoadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lMgznLoadOffsetX;
		}
		else if (!bLoad && (m_lMgznUnloadOffsetX != 0) && (labs(m_lMgznUnloadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lMgznUnloadOffsetX;
		}

		if (bLoad && (m_lMgznLoadOffsetY != 0) && (labs(m_lMgznLoadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lMgznLoadOffsetY;
		}
		else if (!bLoad && (m_lMgznUnloadOffsetY != 0) && (labs(m_lMgznUnloadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lMgznUnloadOffsetY;
		}

		if (IsAllMotorsEnable())
		{
			if (IsWithinXYLimit(lPosX, lPosY))
			{
				XY_MoveTo(lPosX, lPosY, SFM_WAIT);
				
				Sleep(100);
				if (!IsXYEncoderWithinRange())
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CNVCLoader::MoveXYToStandby(BOOL bWait, BOOL bMoveZ)
{
	INT nMode = SFM_WAIT;
	if (!bWait)
	{
		nMode = SFM_NOWAIT;
	}

	if (bMoveZ)
	{
		Z1_MoveTo(m_lStandByPos_Z1, SFM_WAIT);
		Z2_MoveTo(m_lStandByPos_Z2, SFM_WAIT);
	}

	INT nResult = XY_MoveTo(m_lStandByPos_X, m_lStandByPos_Y, nMode);

	if (nResult != gnOK)
	{
		return FALSE;
	}

	if (bWait)
	{
		Sleep(100);
		if (!IsXYEncoderWithinRange())
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNVCLoader::MoveXYToBinTable(BOOL bUseGripper1, BOOL bLoad, BOOL bWait)
{
	INT nMode = SFM_WAIT;
	if (!bWait)
	{
		nMode = SFM_NOWAIT;
	}

	INT nResult = gnOK;
	if (bUseGripper1)
	{
		LONG lPosX = m_lBTLoadUnloadPos1_X;
		LONG lPosY = m_lBTLoadUnloadPos1_Y;

		if (bLoad && (m_lBTLoadOffsetX != 0) && (labs(m_lBTLoadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lBTLoadOffsetX;
		}
		else if (!bLoad && (m_lBTUnloadOffsetX != 0) && (labs(m_lBTUnloadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lBTUnloadOffsetX;
		}

		if (bLoad && (m_lBTLoadOffsetY != 0) && (labs(m_lBTLoadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lBTLoadOffsetY;
		}
		else if (!bLoad && (m_lBTUnloadOffsetY != 0) && (labs(m_lBTUnloadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lBTUnloadOffsetY;
		}

		nResult = XY_MoveTo(lPosX, lPosY, nMode);
	}
	else
	{
		LONG lPosX = m_lBTLoadUnloadPos2_X;
		LONG lPosY = m_lBTLoadUnloadPos2_Y;

		if (bLoad && (m_lBTLoadOffsetX != 0) && (labs(m_lBTLoadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lBTLoadOffsetX;
		}
		else if (!bLoad && (m_lBTUnloadOffsetX != 0) && (labs(m_lBTUnloadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lBTUnloadOffsetX;
		}

		if (bLoad && (m_lBTLoadOffsetY != 0) && (labs(m_lBTLoadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lBTLoadOffsetY;
		}
		else if (!bLoad && (m_lBTUnloadOffsetY != 0) && (labs(m_lBTUnloadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lBTUnloadOffsetY;
		}

		nResult = XY_MoveTo(lPosX, lPosY, nMode);
	}

	if (nResult != gnOK)
	{
		return FALSE;
	}

	if (bWait)
	{
		Sleep(100);
		if (!IsXYEncoderWithinRange())
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNVCLoader::MoveXYToWaferTable(BOOL bUseGripper1, BOOL bLoad, BOOL bWait)
{
	INT nMode = SFM_WAIT;
	if (!bWait)
	{
		nMode = SFM_NOWAIT;
	}

	INT nResult = gnOK;

	if (bUseGripper1)
	{
		LONG lPosX = m_lWTLoadUnloadPos1_X;
		LONG lPosY = m_lWTLoadUnloadPos1_Y;

		if (bLoad && (m_lWTLoadOffsetX != 0) && (labs(m_lWTLoadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lWTLoadOffsetX;
		}
		else if (!bLoad && (m_lWTUnloadOffsetX != 0) && (labs(m_lWTUnloadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lWTUnloadOffsetX;
		}

		if (bLoad && (m_lWTLoadOffsetY != 0) && (labs(m_lWTLoadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lWTLoadOffsetY;
		}
		else if (!bLoad && (m_lWTUnloadOffsetY != 0) && (labs(m_lWTUnloadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lWTUnloadOffsetY;
		}
	
		nResult = XY_MoveTo(lPosX, lPosY, nMode);
	}
	else
	{
		LONG lPosX = m_lWTLoadUnloadPos2_X;
		LONG lPosY = m_lWTLoadUnloadPos2_Y;

		if (bLoad && (m_lWTLoadOffsetX != 0) && (labs(m_lWTLoadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lWTLoadOffsetX;
		}
		else if (!bLoad && (m_lWTUnloadOffsetX != 0) && (labs(m_lWTUnloadOffsetX) <= 500))
		{
			lPosX = lPosX + m_lWTUnloadOffsetX;
		}

		if (bLoad && (m_lWTLoadOffsetY != 0) && (labs(m_lWTLoadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lWTLoadOffsetY;
		}
		else if (!bLoad && (m_lWTUnloadOffsetY != 0) && (labs(m_lWTUnloadOffsetY) <= 500))
		{
			lPosY = lPosY + m_lWTUnloadOffsetY;
		}

		nResult = XY_MoveTo(lPosX, lPosY, nMode);
	}

	if (nResult != gnOK)
	{
		return FALSE;
	}

	if (bWait)
	{
		Sleep(100);
		if (!IsXYEncoderWithinRange())
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNVCLoader::IsXYEncoderWithinRange()
{
	if (m_bDisableNL)
	{
		return TRUE;
	}
	if (!m_bSel_X || !m_bSel_Y)
	{
		return TRUE;
	}
	if (!m_bXYUseEncoder)
	{
		return TRUE;
	}

	//X_Sync();
	//Y_Sync();

	GetEncoderValue();

	LONG lCmd_X = CMS896AStn::MotionGetCommandPosition(NL_AXIS_X, &m_stNLAxis_X);
	LONG lCmd_Y = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Y, &m_stNLAxis_Y);

	if (labs(m_lEnc_X - lCmd_X) > 200)
	{
		CString szErr;
		szErr.Format("NL: Check X ENC & CMD missing-step error : ENC(%ld), CMD(%ld), TOL(200)", m_lEnc_X, lCmd_X);
		HmiMessage(szErr);
		return FALSE;
	}
	if (labs(m_lEnc_Y - lCmd_Y) > 200)
	{
		CString szErr;
		szErr.Format("NL: Check Y ENC & CMD missing-step error : ENC(%ld), CMD(%ld), TOL(200)", m_lEnc_Y, lCmd_Y);
		HmiMessage(szErr);
		return FALSE;
	}

	return TRUE;
}

BOOL CNVCLoader::MoveZDownToMgzn(LONG lMgzn, LONG lSlot, BOOL bLoad, BOOL bUseGripper1)
{
	if ((lMgzn < 0) || (lMgzn >= MS_BL_MGZN_NUM))
	{
		return FALSE;
	}

	if (bUseGripper1)
	{
		LONG lPosZ = m_stNVCMgznRT[lMgzn].m_lMidPosnZ1;

		if ( !bLoad && (m_lMgznUnloadOffsetZ != 0) && (labs(m_lMgznUnloadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lMgznUnloadOffsetZ;
		}
		else if ( bLoad && (m_lMgznLoadOffsetZ != 0) && (labs(m_lMgznLoadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lMgznLoadOffsetZ;
		}

		if (IsWithinZ1Limit(lPosZ))
		{
			Z1_MoveTo(lPosZ);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		LONG lPosZ = m_stNVCMgznRT[lMgzn].m_lMidPosnZ2;
		if (IsWithinZ2Limit(lPosZ))
		{
			Z2_MoveTo(lPosZ);
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CNVCLoader::MoveZDownToBinTable(BOOL bUseGripper1, BOOL bLoad)
{
	if (bUseGripper1)
	{
		LONG lPosZ = m_lBinLoadPos_Z1;
		LONG lCmd_Z1 = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z1, &m_stNLAxis_Z1);

		if (bLoad && (m_lBTLoadOffsetZ != 0) && (labs(m_lBTLoadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lBTLoadOffsetZ;
		}
		else if (!bLoad && (m_lBTUnloadOffsetZ != 0) && (labs(m_lBTUnloadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lBTUnloadOffsetZ;
		}

		if (IsWithinZ1Limit(lPosZ))
		{
			Z1_MoveTo(lPosZ);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		LONG lPosZ = m_lBinLoadPos_Z2;
		LONG lCmd_Z2 = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z2, &m_stNLAxis_Z2);

		if (bLoad && (m_lBTLoadOffsetZ != 0) && (labs(m_lBTLoadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lBTLoadOffsetZ;
		}
		else if (!bLoad && (m_lBTUnloadOffsetZ != 0) && (labs(m_lBTUnloadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lBTUnloadOffsetZ;
		}

		if (IsWithinZ2Limit(lPosZ))
		{
			Z2_MoveTo(lPosZ);
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CNVCLoader::MoveZDownToWafTable(BOOL bUseGripper1, BOOL bLoad)
{
	if (bUseGripper1)
	{
		LONG lPosZ = m_lWafLoadPos_Z1;
		LONG lCmd_Z1 = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z1, &m_stNLAxis_Z1);

		if ( bLoad && (m_lWTLoadOffsetZ != 0) && (labs(m_lWTLoadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lWTLoadOffsetZ;
		}
		else if ( !bLoad && (m_lWTUnloadOffsetZ != 0) && (labs(m_lWTUnloadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lWTUnloadOffsetZ;
		}

		if (IsWithinZ1Limit(lPosZ))
		{
			Z1_MoveTo(lPosZ);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		LONG lPosZ = m_lWafLoadPos_Z2;
		LONG lCmd_Z2 = CMS896AStn::MotionGetCommandPosition(NL_AXIS_Z2, &m_stNLAxis_Z2);

		if ( bLoad && (m_lWTLoadOffsetZ != 0) && (labs(m_lWTLoadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lWTLoadOffsetZ;
		}
		else if ( !bLoad && (m_lWTUnloadOffsetZ != 0) && (labs(m_lWTUnloadOffsetZ) <= 500) )
		{
			lPosZ = lPosZ + m_lWTUnloadOffsetZ;
		}

		if (IsWithinZ2Limit(lPosZ))
		{
			Z2_MoveTo(lPosZ);
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CNVCLoader::MoveZUpToStandby(BOOL bUseGripper1)
{
	if (bUseGripper1)
	{
		if (!m_bSel_Z1)
		{
			return TRUE;
		}
		if (!Z1_IsPowerOn())
		{
			return FALSE;
		}
		Z1_MoveTo(m_lStandByPos_Z1);
	}
	else
	{
		if (!m_bSel_Z2)
		{
			return TRUE;
		}
		if (!Z2_IsPowerOn())
		{
			return FALSE;
		}
		Z2_MoveTo(m_lStandByPos_Z2);
	}
	return TRUE;
}

BOOL CNVCLoader::MoveZUpToHome(BOOL bUseGripper1)
{
	if (bUseGripper1)
	{
		if (!m_bSel_Z1)
		{
			return TRUE;
		}
		if (!Z1_IsPowerOn())
		{
			Z1_Home();
			return FALSE;
		}
		Z1_MoveTo(0);
	}
	else
	{
		if (!m_bSel_Z2)
		{
			return TRUE;
		}
		if (!Z2_IsPowerOn())
		{
			Z2_Home();
			return FALSE;
		}
		Z2_MoveTo(0);
	}
	return TRUE;
}

BOOL CNVCLoader::IsGripperHasFrame(BOOL bUseGripper1, BOOL bFailToHome)
{
	CString szLog;

	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (!IsGripper1FrameExist())
			{
				Sleep(500);
				if (!IsGripper1FrameExist())
				{
					szLog = "IsGripperHasFrame Error: Gripper 1 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					
					if (bFailToHome)
					{
						SetWafGripper(TRUE);	//OPEN clamp
						Sleep(100);
						Z1_MoveTo(m_lStandByPos_Z1);
					}
					return FALSE;
				}
			}
		}
		else			
		{
			if (!IsGripper2FrameExist())
			{
				Sleep(500);
				if (!IsGripper2FrameExist())
				{
					szLog = "IsGripperHasFrame Error: Gripper 2 detects NO frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					
					if (bFailToHome)
					{
						SetBinGripper(TRUE);	//OPEN clamp
						Sleep(100);
						Z2_MoveTo(m_lStandByPos_Z2);
					}
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CNVCLoader::IsGripperHasNoFrame(BOOL bUseGripper1)
{
	CString szLog;

	if (!m_bNoSensorCheck)
	{
		if (bUseGripper1)	
		{
			if (IsGripper1FrameExist())
			{
				Sleep(500);
				if (IsGripper1FrameExist())
				{
					szLog = "IsGripperHasNoFrame Error: Gripper 1 detects frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					
					//if (bFailToHome)
					//{
					//	SetWafGripper(TRUE);	//OPEN clamp
					//	Z1_MoveTo(m_lStandByPos_Z1);
					//}
					return FALSE;
				}
			}
		}
		else			
		{
			if (IsGripper2FrameExist())
			{
				Sleep(500);
				if (IsGripper2FrameExist())
				{
					szLog = "IsGripperHasNoFrame Error: Gripper 2 detects frame!";
					SetErrorMessage(szLog);
					HmiMessage(szLog);
					
					//if (bFailToHome)
					//{
					//	SetBinGripper(TRUE);	//OPEN clamp
					//	Z2_MoveTo(m_lStandByPos_Z2);
					//}
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CNVCLoader::ScanningBarcode(CString &szBarcode, BOOL bScanTwice)
{

	return TRUE;
}

BOOL CNVCLoader::UpdateMgznSlotStatus(BOOL bIsLoad, LONG lMgzn, LONG lSlot, LONG lBlock)
{

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////
//	NL Sensor IO Functions 
///////////////////////////////////////////////////////////////////////////

VOID CNVCLoader::SetBinExpanderOpen(BOOL bSet)
{
	if (!m_fHardware || m_bDisableNL)
	{
		return;
	}
	BOOL bOn = bSet;
	CMS896AStn::MotionSetOutputBit(NL_SO_Gripper1State, bOn);
}

VOID CNVCLoader::SetWafExpanderOpen(BOOL bSet)
{
	if (!m_fHardware || m_bDisableNL)
	{
		return;
	}
	BOOL bOn = bSet;
	CMS896AStn::MotionSetOutputBit(NL_SO_Gripper2State, bOn);
}

BOOL CNVCLoader::SetWafGripperOpen(BOOL bSet)
{
	if (!m_fHardware || m_bDisableNL)
	{
		return FALSE;
	}

	//m_bIsGripper1Open = CMS896AStn::MotionReadInputBit(NL_SI_Clamp1Open);
	BOOL bOn = bSet;
	CMS896AStn::MotionSetOutputBit(NL_SO_Clamp1Open, bOn);
	return TRUE;
}

BOOL CNVCLoader::SetWafGripperClose(BOOL bSet)
{
	if (!m_fHardware || m_bDisableNL)
	{
		return FALSE;
	}

	//m_bIsGripper1Close = CMS896AStn::MotionReadInputBit(NL_SI_Clamp1Close);
	BOOL bOn = bSet;
	CMS896AStn::MotionSetOutputBit(NL_SO_Clamp1Close, bOn);
	return TRUE;
}

BOOL CNVCLoader::SetBinGripperOpen(BOOL bSet)
{
	if (!m_fHardware || m_bDisableNL)
	{
		return FALSE;
	}

	//m_bIsGripper2Open = CMS896AStn::MotionReadInputBit(NL_SI_Clamp2Open);
	BOOL bOn = bSet;
	CMS896AStn::MotionSetOutputBit(NL_SO_Clamp2Open, bOn);
	return TRUE;
}

BOOL CNVCLoader::SetBinGripperClose(BOOL bSet)
{
	if (!m_fHardware || m_bDisableNL)
	{
		return FALSE;
	}

	//m_bIsGripper2Close = CMS896AStn::MotionReadInputBit(NL_SI_Clamp2Close);
	BOOL bOn = bSet;
	CMS896AStn::MotionSetOutputBit(NL_SO_Clamp2Close, bOn);
	return TRUE;
}

BOOL CNVCLoader::IsGripper1FrameExist()
{
	if (!m_fHardware || m_bDisableNL)
	{
		return FALSE;
	}
	if (m_bNoSensorCheck)
	{
		return FALSE;
	}

	m_bIsGripper1FrameExist = CMS896AStn::MotionReadInputBit(NL_SI_FrameExist1);
	return m_bIsGripper1FrameExist;
}

BOOL CNVCLoader::IsGripper2FrameExist()
{
	if (!m_fHardware || m_bDisableNL)
	{
		return FALSE;
	}
	if (m_bNoSensorCheck)
	{
		return FALSE;
	}

	m_bIsGripper2FrameExist = CMS896AStn::MotionReadInputBit(NL_SI_FrameExist2);
	return m_bIsGripper2FrameExist;
}


BOOL CNVCLoader::CheckGripperFrameExistForXYMove(BOOL bUseGripper1)
{
	if (m_bNoSensorCheck)
	{
		return TRUE;	//XY can MOVE
	}

	CString szLog;

	if (bUseGripper1)	
	{
		if (IsGripper1FrameExist())
		{
			Sleep(500);
			if (IsGripper1FrameExist())
			{
				SetWafGripper(TRUE);
				szLog = "Waf gripper detects frame for XY Move";
				SetErrorMessage(szLog);
				HmiMessage(szLog);
				return FALSE;			//XY cannot MOVE due to gripper 1
			}
		}
	}
	else			
	{
		if (IsGripper2FrameExist())
		{
			Sleep(500);
			if (IsGripper2FrameExist())
			{
				SetWafGripper(TRUE);
				szLog = "Bin gripper detects frame for XY MOVE";
				SetErrorMessage(szLog);
				HmiMessage(szLog);
				//Z2_MoveTo(m_lStandByPos_Z2);
				return FALSE;			//XY cannot MOVE due to gripper 1
			}
		}
	}

	return TRUE;	//XY can MOVE
}


///////////////////////////////////////////////////////////////////////////
//	 
///////////////////////////////////////////////////////////////////////////

