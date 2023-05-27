
/////////////////////////////////////////////////////////////////
// BL_LoadFrame.cpp : Move functions of the CBinLoader class
//
//	Description:
//		MS899 Mapping Die Sorter
//
//	Date:		Friday, April 13, 2010
//	Revision:	1.00
//
//	By:			Andrew (PLLM)
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004-2010
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MS896A_Constant.h"
#include "BinLoader.h"
#include "MS_SecCommConstant.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
// ==========================================================================================================
// ==========================================================================================================
//Magazine->Buffer
LONG CBinLoader::DB_LoadFromMgzToBuffer(BOOL bBurnInLoad, LONG lLoadBlock, BOOL bClearAllMode, BOOL bBufferLevel, BOOL bBinFullLoadEmpty, BOOL bIsPreLoad)
{
	LONG lMgzn = 0, lSlot = 0;
	LONG lStatus = 0;
	BOOL bUseEmpty = FALSE;
	BOOL bFrameExist = FALSE;
	BOOL bFrameProtect = FALSE;
	CString str;
	CString szText1;
	CString szBinBlkGrade = "";
	BOOL bUseBarcode = m_bUseBarcode;	//v4.39T7


    BL_DEBUGBOX("Load from Mgz to Buffer start");

	BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];
	if (bEnableBtT && !IsMS90())		//v4.39T7	//Nichia MS100+		//v4.50A25
	{
		//With Bt T the barcode scanning will be performed on table instead of gripper;
		//so here the barcode scanning on gripper is disabled
		bUseBarcode = FALSE;
	}

	//Check Gripper position is valid
	if ( IsMagazineSafeToMove() == FALSE )
	{
		BL_DEBUGBOX("Gripper is not in safe position\n");
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);
		return FALSE;
	}

	BL_DEBUGBOX("gripper move to ready");
	if (!m_bIsExArmGripperAtUpPosn && (BinGripperMoveTo_Auto(m_lReadyPos_X, SFM_NOWAIT) == FALSE))
	{
		SetErrorMessage("Bin Gripper fails when moving to Ready Pos");
		return FALSE;
	}

	if (bBufferLevel == BL_BUFFER_UPPER)
	{
		// check frame on upper buffer table logically
		if (m_clUpperGripperBuffer.IsBufferBlockExist() && (bBurnInLoad == FALSE) && (!m_bDisableBL))
		{
			if ( bClearAllMode == FALSE )
			{
				if ( BL_YES_NO(HMB_BL_BUFFBLK_NOT_ZERO, IDS_BL_LOAD_FRAME) == FALSE )
				{
					SetErrorMessage("Logical Block of Upper Buffer is not Zero");
					return FALSE;
				}
			}
		}

		//Check Upper Buffer has frame or not physically
		bFrameExist = IsDualBufferUpperExist();
		if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
		{
			if ( (bFrameExist == TRUE) && (m_bNoSensorCheck) )
			{
				SetErrorMessage("Frame exists in Upper Buffer but by-pass by Disabling Sensor Check");
			}

			bFrameExist = FALSE;
		}

		if ( bFrameExist == TRUE )
		{
			SetErrorMessage("BL Frame exists in Upper Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			return FALSE;
		}

		//Set buffer down if necessary
		if ( IsDualBufferLevelDown() == FALSE)
		{
			BL_DEBUGBOX("buffer down");
			SetBufferLevel(FALSE);	//check status later
		}
	}
	else if (bBufferLevel == BL_BUFFER_LOWER)
	{
		//Safety checking, upper buffer must be empty when loading to lower buffer frame to avoid crash
		if (IsDualBufferUpperExist())
		{
			SetErrorMessage("BL Frame exists in Upper Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			return FALSE;
		}

		// check frame on lower buffer table logically
		if (m_clLowerGripperBuffer.IsBufferBlockExist() && (bBurnInLoad == FALSE) && (!m_bDisableBL) )
		{
			if ( bClearAllMode == FALSE )
			{
				if ( BL_YES_NO(HMB_BL_BUFFBLK_NOT_ZERO, IDS_BL_LOAD_FRAME) == FALSE )
				{
					SetErrorMessage("Logical Block of Lower Buffer is not Zero");
					return FALSE;
				}
			}
		}

		//Check Lower Buffer has frame or not physically
		bFrameExist = IsDualBufferLowerExist();
		if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
		{
			if ( (bFrameExist == TRUE) && (m_bNoSensorCheck) )
			{
				SetErrorMessage("Frame exists in Lower Buffer but by-pass by Disabling Sensor Check");
			}

			bFrameExist = FALSE;
		}

		if ( bFrameExist == TRUE )
		{
			SetErrorMessage("BL Frame exists in Lower Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
			return FALSE;
		}

		//Set buffer up if necessary
		if ( IsDualBufferLevelDown() == TRUE )
		{
			BL_DEBUGBOX("buffer up");
			SetBufferLevel(TRUE);
		}
	}

    if( GetLoadMgzSlot(bUseEmpty, lLoadBlock)==FALSE )
    {
		CString szMsg;
		szMsg.Format("\n%d",lLoadBlock);
		SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
		SetErrorMessage("BL No slot is assigned to this physical block");
		SetBufferLevel(FALSE);
        return FALSE;
    }

	//Make sure last elevator motion is complete before UNLOAD
    if( ElevatorZSynMove_Auto()==FALSE )
	{
		SetBufferLevel(FALSE);
        return FALSE;
	}

	if( bUseEmpty==FALSE )
    {
        // move elevator to the slot and gripper to ready
        lMgzn = m_lCurrMgzn;
        lSlot = m_lCurrSlot;
        str.Format("Load filmframe from M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
        BL_DEBUGMESSAGE(str);
		
		if (CheckElevatorCover() == FALSE)
        {
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
        }

        BL_DEBUGBOX("magazine move to load");
		if (MoveElevatorToLoad(lMgzn, lSlot, bBufferLevel) == FALSE)
		{
			//HouseKeeping(m_lReadyPos_X, FALSE);	//DO not move any motors when this fcn fails!
			return FALSE;
		}

		if ( bBurnInLoad==FALSE && IsMagazineExist(lMgzn)==FALSE )
		{
			Sleep(200);	//v4.41T2
			if (IsMagazineExist(lMgzn, FALSE, TRUE)==FALSE)
			{
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
		}

        if ( CheckElevatorCover() == FALSE )
        {
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
        }
  
		BL_DEBUGBOX("gripper move to load");
		BinGripperMoveTo(m_lLoadPos_X);

		//v3.89		//Move to before InClamp checking
		if ( IsAllMotorsEnable() == FALSE )		//v3.80
		{
			//v4.45T2
#ifndef NU_MOTION
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
			{
				//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
				(*m_psmfSRam)["BinLoaderStn"]["GripperJam"] = TRUE;
			}
#endif

			SetErrorMessage("Motor is OFF during LOAD mag frame");
			SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
			X_Home();
			HouseKeeping(bBufferLevel, m_lReadyPos_X, FALSE);
			return FALSE;
		}

		if ( IsFrameJam() == TRUE )
		{
			CMS896AStn::MotionStop(BL_AXIS_X, &m_stBLAxis_X);
			Sleep(100);
			//SetLoaderXPower(FALSE);

			//v4.445T2
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
			{
				SetMotionCE(TRUE, "Bin Loader Gripper Jam (DB_LoadFromMgzToBuffer)");
			}

			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			X_Home();
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}

		if ( !IsFrameInClamp() )
        {
			// search the frame in clamp
			if ( BinGripperSearchInClamp(HP_POSITIVE_DIR, m_lLoadSearchInClampOffsetX, 100)==FALSE )
			{
				if ( IsAllMotorsEnable() == FALSE )		//v3.80
				{
					//v4.445T2
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
					{
						SetMotionCE(TRUE, "Bin Loader Motor Not Enable (DB_LoadFromMgzToBuffer)");
					}

					SetErrorMessage("Motor is OFF during LOAD mag frame");
					SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
					X_Home();
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}

				if ( IsFrameJam() == TRUE )
				{
					CMS896AStn::MotionStop(BL_AXIS_X, &m_stBLAxis_X);
					Sleep(100);
					//SetLoaderXPower(FALSE);
					
					//v4.445T2
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
					{
						SetMotionCE(TRUE, "Bin Loader Gripper Jam (DB_LoadFromMgzToBuffer)");
					}

					SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
					X_Home();	//v3.80
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}

				if (bBurnInLoad==FALSE)
				{
					BOOL bResult = BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, IDS_BL_LOAD_FRAME, CMS896AStn::m_bDisableOKSelectionInAlertMsg);
					if (bResult == TRUE)
					{
						if ( !IsFrameInClamp() )
						{
							SetErrorMessage("BL No frame is detected in gripper");
							SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);
							X_Home();	//v3.80
							HouseKeeping(m_lReadyPos_X, FALSE);
							return FALSE;
						}
					}
					else
					{
						SetErrorMessage("BL No frame is detected in gripper");
						SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);
						X_Home();	//v3.80
						HouseKeeping(m_lReadyPos_X, FALSE);
						return FALSE;
					}
				}
			}
        }

    }
    else	//search empty frame
    {
		INT nRetryCount = 0;
        while (1)
        {
						//4.54T16 Xu Cai Jun
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			if(pApp->GetCustomerName() == "LMP")
			{
				m_lLoadFrameRetryCount = 0;
			}

			//v4.43T13	//SeoulOpto
			nRetryCount++;
			if (m_lLoadFrameRetryCount > 0 && (nRetryCount > m_lLoadFrameRetryCount))
			{
				CString szErr;
				szErr.Format("Load BL EMPTY frame retry fail - Count = %ld", m_lLoadFrameRetryCount);
				BL_DEBUGBOX(szErr);
				SetErrorMessage(szErr);
			
				//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if (pApp->GetCustomerName() == CTM_RENESAS)
					SetAlert_NoMaterial(IDS_BL_EX_LOAD_EMPTY_FRAME_FAIL);	//v4.59A16
				else
					SetAlert_Red_Yellow(IDS_BL_EX_LOAD_EMPTY_FRAME_FAIL);
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}

			//shiraishi02
			if ((pApp->GetCustomerName() == CTM_RENESAS) && (State() != IDLE_Q))
			{
				if (m_bStop)
				{
					CString szErr;
					szErr.Format("Load BL EMPTY frame retry aborted - Count = %ld", m_lLoadFrameRetryCount);
					BL_DEBUGBOX(szErr);
					SetErrorMessage(szErr);
					HouseKeeping(m_lReadyPos_X, FALSE);	
					return Err_BLLoadEmptyAbort;
				}
			}

            BL_DEBUGBOX("gripper ready and magazine to empty");

			//Only available for Renesas MS90 //v4.59A16
			LONG lGetNullStatus = GetNullMgznSlot(lMgzn, lSlot, lLoadBlock);
			if (lGetNullStatus == 2)	
			{
				LONG lNullMgzn = 0;
				GetNullMgznNo(lNullMgzn);

				CString szLog;
				szLog.Format("BL Reset Mgzn #%ld after GetNullMgznSlot", lNullMgzn);
				BL_DEBUGBOX(szLog);

				ResetMagazine(lNullMgzn);		//Reset EMPTY mgzn and start searching new EMPTY frame again
				lGetNullStatus = GetNullMgznSlot(lMgzn, lSlot, lLoadBlock);
			}

            if (lGetNullStatus == FALSE )
			{
				if (bBinFullLoadEmpty == FALSE && bIsPreLoad == FALSE)
				{
					CString szText = "\nBinLoader";
					str.Format("DB_LoadFromMgzToBuffer - Generate special communication file(Block:%d)", lLoadBlock);
					CMSLogFileUtility::Instance()->BL_LogStatus(str);

					if (GenerateSpecialCommunicationFileForCree(lLoadBlock) == FALSE)
					{
						CMSLogFileUtility::Instance()->BL_LogStatus("Fail to generate special communication file");
						SetAlert_Msg_Red_Yellow(IDS_BL_FAIL_TO_GENERATE_COM_FILE, szText);
					}
				}

				HouseKeeping(m_lReadyPos_X, FALSE);
                return FALSE;
			}

			str.Format("DB Load filmframe from Empty M%d S%d", lMgzn, lSlot);
            BL_DEBUGMESSAGE(str);
			
			if ( CheckElevatorCover() == FALSE )
            {
				RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
            }

			if ( MoveElevatorToLoad(lMgzn, lSlot, bBufferLevel) == FALSE )
			{
				if (bBufferLevel == BL_BUFFER_LOWER)
					SetBufferLevel(TRUE);
				//HouseKeeping(m_lReadyPos_X, FALSE);	//Do not move any motor when this fcn fails
				RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
				return FALSE;
			}
			
            if ( bBurnInLoad==FALSE && IsMagazineExist(lMgzn)==FALSE )
            {	
				Sleep(200);		//v4.41T2
				if (IsMagazineExist(lMgzn, FALSE, TRUE)==FALSE)
				{
					RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}
            }

            if ( CheckElevatorCover() == FALSE )
            {
				RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
            }

			BinGripperMoveTo(m_lLoadPos_X);

			//v3.89	//moved to before InClamp checking
			if ( IsAllMotorsEnable() == FALSE )		//v3.80
			{
				SetErrorMessage("Motor is OFF during LOAD mag EMPTY frame");
				SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
				X_Home();	
				RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}

			if ( IsFrameJam() == TRUE )
			{
				CMS896AStn::MotionStop(BL_AXIS_X, &m_stBLAxis_X);
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				X_Home();		//v3.80
				RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}			

 			if ( !IsFrameInClamp() )
			{
				if ( BinGripperSearchInClamp(HP_POSITIVE_DIR, m_lLoadSearchInClampOffsetX, 100)==FALSE )
				{
					if ( IsAllMotorsEnable() == FALSE )		//v3.80
					{
						SetErrorMessage("Motor is OFF during LOAD EMPTY mag frame");
						SetAlert_Red_Yellow(IDS_BL_MOTOR_OFF);
						X_Home();	
						HouseKeeping(m_lReadyPos_X, FALSE);
						RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
						return FALSE;
					}

					if ( IsFrameJam() == TRUE )
					{
						CMS896AStn::MotionStop(BL_AXIS_X, &m_stBLAxis_X);
						SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
						X_Home();		//v3.80
						HouseKeeping(m_lReadyPos_X, FALSE);
						RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
						return FALSE;
					}

					if( bBurnInLoad==FALSE )
					{
						Sleep(100);
						if (!BinGripperMoveTo(m_lReadyPos_X))
						{
							//v3.94T4
							if (bBufferLevel == BL_BUFFER_LOWER)
								SetBufferLevel(TRUE);

							//v3.94T4	
							//Additional protection breaking loop in case gripper-move has problem
							SetErrorMessage("Gripper move back to READY fails during LOAD EMPTY mag frame");
							RestoreNullMgznSlot(lMgzn, lSlot);	//v4.39T8	//Cree
							return FALSE;
						}
						continue;
					}
				}
            }

            break;
        }
    }

		//v3.70T4
	if (m_bIsExArmGripperAtUpPosn)
		m_bIsExArmGripperAtUpPosn = FALSE;

	Sleep(100);
	SetGripperState(TRUE);
	//Sleep(150);
	Sleep(500);			//v4.47T1	//LeoLam

	if (bBufferLevel == BL_BUFFER_UPPER)
	{
		m_clUpperGripperBuffer.SetBufferBlock(lLoadBlock);
		m_clUpperGripperBuffer.SetBufferBarcode("");
	}
	else if (bBufferLevel == BL_BUFFER_LOWER)
	{
		m_clLowerGripperBuffer.SetBufferBlock(lLoadBlock);
		m_clLowerGripperBuffer.SetBufferBarcode("");
	}

	//Original barcode scanning method on gripper
	if (bUseBarcode == TRUE)
	{		
		BL_DEBUGBOX("#1 gripper move to barcode pos");
		if (BinGripperMoveTo(m_lBarcodePos_X, TRUE) == FALSE)		//v4.02T6	//Barcode prescan fcn
		{
			HouseKeeping(m_lReadyPos_X);
			return FALSE;	
		}

		if (!bBurnInLoad && !IsFrameInClamp())					//v4.10	//BURNIN
		{
			BOOL bExist = FALSE;

			if (bBufferLevel == BL_BUFFER_UPPER)
			{
				if (IsDualBufferUpperExist())
				{
                    bExist = TRUE;		
				}
				else if ( bBurnInLoad || m_bNoSensorCheck )		//v3.94
				{
                    bExist = TRUE;		
				}
			}
			else if (bBufferLevel == BL_BUFFER_LOWER)
			{
				if (IsDualBufferLowerExist())
				{
                    bExist = TRUE;		
				}
				else if ( bBurnInLoad || m_bNoSensorCheck )		//v3.94
				{
                    bExist = TRUE;		
				}
			}
		
			//search frame again if it is detected in buffer
			if (bExist == TRUE)
			{
				SetGripperState(FALSE);
				BinGripperSearchInClamp(HP_POSITIVE_DIR, 1000, 50, TRUE);
				Sleep(200);

				if (!bBurnInLoad && !IsFrameInClamp())		//v4.10	//BURNIN
				{
					SetGripperState(FALSE);
					if( BL_YES_NO(HMB_BL_AUTO_PUSH_FRAME_BACK_TO_MGZ, IDS_BL_LOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
					{	
						HouseKeeping(m_lReadyPos_X, FALSE);
					}
					else
					{
						SetGripperState(FALSE);
						if (BinGripperMoveSearchJam(m_lLoadPos_X) == FALSE)
						{
							BinGripperMoveTo(m_lReadyPos_X);
						}
						else
						{
							HouseKeeping(m_lReadyPos_X, FALSE);
						}
					}
					
					if (bBufferLevel == BL_BUFFER_UPPER)
					{
						m_clUpperGripperBuffer.SetBufferBlock(0);
					}
					else if (bBufferLevel == BL_BUFFER_LOWER)
					{
						m_clLowerGripperBuffer.SetBufferBlock(0);
					}

					//SetAlarmLamp_Yellow();
					return FALSE;
				}
				else
				{
					BL_DEBUGBOX("Retry scan barcode");

					SetGripperState(TRUE);
					//Sleep(200);
					Sleep(500);		//v4.47T1	//LeoLam

					BL_DEBUGBOX("#2 gripper move to barcode pos");
					if (BinGripperMoveTo(m_lBarcodePos_X) == FALSE)
					{
						HouseKeeping(m_lReadyPos_X);
						return FALSE;	
					}
				}
			}
			else
			{
				SetGripperState(FALSE);
				SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);		

				//andrew123		//Push back frame to slot
				SetGripperState(FALSE);
				HmiMessage("Frame will push back to slot by output gripper ...");
				Sleep(200);
				if (BinGripperMoveSearchJam(m_lLoadPos_X) == FALSE)
				{
					//If JAM!
					BinGripperMoveTo(m_lReadyPos_X);
				}

				//xyz	//andrew123
				if (bBufferLevel == BL_BUFFER_UPPER)
				{
					m_clUpperGripperBuffer.SetBufferBlock(0);
				}
				else if (bBufferLevel == BL_BUFFER_LOWER)
				{
					m_clLowerGripperBuffer.SetBufferBlock(0);
				}

				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
		}

		//v4.02T6	//barcode prescan fcn in BInGripperMoveTo() above
		if ( !m_szDBPreScanBCName.IsEmpty() )
		{
			//v4.51A17
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			BOOL bEnableScanTwiceFcn = pApp->GetFeatureStatus(MS896A_FUNC_BLBC_EMPTY_SCAN_TWICE);
			if (bEnableScanTwiceFcn && bUseEmpty)
			{
				BL_DEBUGBOX("prescan barcode done but need scan twice (SANAN)");

				m_szDBPreScanBCName = _T("");
				lStatus		= FALSE;

				CString szLog;
				szLog.Format("ScanningBarcode (PreScan) EMPTY = %d", bUseEmpty);
				BL_DEBUGBOX(szLog);
				
				CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
				lStatus = ScanningBarcode(pGripperBuffer, bUseEmpty);	//v4.51A17
				if (lStatus == FALSE)
				{
					BL_DEBUGBOX("No barcode read (Prescan)");
				}
				else if (lStatus == -1)
				{
					return FALSE;
				}
			}
			else
			{
				m_szBCName	= m_szDBPreScanBCName;
				m_szDBPreScanBCName = _T("");
				lStatus		= TRUE;
				BL_DEBUGBOX("prescan barcode done and BC " + m_szBCName);
			}
		}
		else
		{
			BL_DEBUGBOX("#3 scan barcode");
			CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
			lStatus = ScanningBarcode(pGripperBuffer, bUseEmpty);	//v4.51A17	//XM SanAn
			if ( lStatus == FALSE )
			{
				BL_DEBUGBOX("No barcode read");
			}
			else if ( lStatus == -1 )
			{
				return FALSE;
			}
		}
	}

	//if ( bBurnInLoad==FALSE && bUseBarcode==TRUE )
	if ( bUseBarcode == TRUE )
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);

		if( pApp->IsBLBCUseOldGoStop() )
			bPLLMSpecialFcn = TRUE;

		if (m_bCheckBarcode)
		{
			BL_DEBUGBOX("Check barcode load from mgzn to buffer");

			// Manual input barcode for selective customers
			CString szSlotBarcode = GetCurrMgznSlotBC();
			BL_DEBUGBOX("Load dual buffer compare barcode :  READ = " + m_szBCName + "; SLOT = " + szSlotBarcode);

			if( (pApp->GetCustomerName()=="TongHui") && 
				(m_szBCName==BL_DEFAULT_BARCODE) && 
				(szSlotBarcode.IsEmpty() || bUseEmpty) )
			{
				CString szText = "\n";
				AlertMsgForManualInputBarcode(lLoadBlock, m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
				SetErrorMessage(m_szBCName + " input by operator from empty DB");
				BL_DEBUGBOX("TH Empty Load DB input BC " + m_szBCName);
			}


			// New DISCARD method for EMPTY frames
			if (((m_szBCName == BL_DEFAULT_BARCODE) || (m_szBCName.IsEmpty())) && 
				(szSlotBarcode.IsEmpty() || bUseEmpty))
			{
				CString szText = "No barcode read on empty frame!";	
				LONG lOption = 0;		//v4.02T6
				BOOL bRead = FALSE;
				CString szErr;

				if (bPLLMSpecialFcn)	//v4.02T6	//Only made it available for PLLM REBEL only
				{
					lOption = SetAlert_Msg_Red_Yellow(IDS_BL_CHECK_BARCODE, szText, "RETRY", "Discard");		
					if (lOption == 1)	//RETRY
					{
						do 
						{
							BL_DEBUGBOX("compare barcode RETRY");	

							CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
							lStatus = ScanningBarcode(pGripperBuffer);
							if ((lStatus == TRUE) && (m_szBCName.GetLength() > 0) && (m_szBCName != BL_DEFAULT_BARCODE))
							{
								BL_DEBUGBOX("compare barcode retry OK - " + m_szBCName);	
								bRead = TRUE;
								break;
							}
							else
							{
								BL_DEBUGBOX("compare barcode retry fail - " + m_szBCName);	
								lOption = SetAlert_Msg_Red_Back(IDS_BL_CHECK_BARCODE, szText, "RETRY", "Discard");

								if (lOption != 1)	//if DISCARD	
								{
									break;
								}
							}

						} while (lOption == 1);		//While RETRY
					}
				}

				if (bRead)
				{
					SetAlarmLamp_Green(FALSE, TRUE);
				}
				else
				{		
					//v4.16T2
					//PushBack frame to original slot, either EMPTY or GRADE slot
					//BinGripperMoveTo(m_lBufferUnloadPos_X);
					//HouseKeeping(m_lReadyPos_X, FALSE);

					if (bBufferLevel == BL_BUFFER_UPPER)
					{
						szErr.Format("compare barcode discards upper EMPTY -- #%d (%d)", m_clUpperGripperBuffer.GetBufferBlock(), lOption); 
						m_clUpperGripperBuffer.SetBufferBlock(0);
					}
					else if (bBufferLevel == BL_BUFFER_LOWER)
					{
						szErr.Format("compare barcode discards lower EMPTY -- #%d (%d)", m_clLowerGripperBuffer.GetBufferBlock(), lOption); 
						m_clLowerGripperBuffer.SetBufferBlock(0);
					}
					BL_DEBUGBOX(szErr);	

					if (bUseEmpty)
					{
						PushBackFrameFromGripper_DL(bBurnInLoad);
						return Err_BLLoadEmptyBarcodeFail;		//EMPTY frame from EMPTY mag allows retry next slot
					}
					else
					{
						//v4.16T2
						BL_DEBUGBOX("ERROR: LOAD frame is pushed back to ABORT");
						SetErrorMessage("ERROR: LOAD frame is pushed back to ABORT");
						HmiMessage_Red_Yellow("No barcode read on bin frame; frame is pushed back to slot");		
						PushBackFrameFromGripper_DL(bBurnInLoad);
						HouseKeeping(m_lReadyPos_X, FALSE);

						return FALSE;							//EMPTY frame from Grade-slot must abort!
					}
				}
			}
		
			//Check if current BC is already in-use in other current slots
			BOOL bCheckMgznBC	= BL_CheckBcInMgzs(lLoadBlock, m_szBCName, bBurnInLoad);
			BOOL bValidateBC	= ValidateBinFrameBarcode(m_szBCName);
			BOOL bRightBCCharacter = CheckBarcodeCharacters(m_szBCName);
			if (bBurnInLoad)	//v4.20
			{
				bCheckMgznBC	= TRUE;
				bValidateBC		= TRUE;
				bRightBCCharacter = TRUE;
			}
			if (!bCheckMgznBC || !bValidateBC || !bRightBCCharacter)
			{
				PushBackFrameFromGripper_DL(bBurnInLoad);
				if (bBufferLevel == BL_BUFFER_UPPER)
				{
					m_clUpperGripperBuffer.SetBufferBlock(0);
				}
				else if (bBufferLevel == BL_BUFFER_LOWER)
				{
					m_clLowerGripperBuffer.SetBufferBlock(0);
				}

				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;	//STOP
			}

			//v4.43T4	//v4.47T17	//Re-added to dual-buffer config for Testar
			if (bUseEmpty || GetCurrMgznSlotBC().IsEmpty())
			{
				if (!CheckBarcodeInMES(lLoadBlock, m_szBCName))	//v4.43T1
				{
					PushBackFrameFromGripper_DL(bBurnInLoad);
					if (bBufferLevel == BL_BUFFER_UPPER)
					{
						m_clUpperGripperBuffer.SetBufferBlock(0);
					}
					else if (bBufferLevel == BL_BUFFER_LOWER)
					{
						m_clLowerGripperBuffer.SetBufferBlock(0);
					}
					HouseKeeping(m_lPreUnloadPos_X, TRUE);
					return FALSE;
				}
			}

			//v2.83T70		//v3.70T3
			//CHeck if current BC is already used before in BC history
			CString szErrCode;
			m_bUseEmptyFrame = bUseEmpty;
			BOOL bCheckBCHistory = CheckBcHistoryInCurrLot(lLoadBlock, m_szBCName, TRUE, szErrCode);
			if (bBurnInLoad)	//v4.20
				bCheckBCHistory = TRUE;
			if (!bCheckBCHistory)
			{
				CString szErr = szErrCode;
				HmiMessage_Red_Yellow(szErr);		
				BL_DEBUGBOX(szErr);	

				PushBackFrameFromGripper_DL(bBurnInLoad);
				if (bBufferLevel == BL_BUFFER_UPPER)
				{
					m_clUpperGripperBuffer.SetBufferBlock(0);
				}
				else if (bBufferLevel == BL_BUFFER_LOWER)
				{
					m_clLowerGripperBuffer.SetBufferBlock(0);
				}

				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;	//Err_BLLoadDuplicateBarcode;	//FALSE;
			}
		}


		if (!m_bCompareBarcode)
		{
			BL_DEBUGBOX("No compare-barcode");

			//Only update Barcode name if this is not Default
			if ( m_szBCName != BL_DEFAULT_BARCODE )
			{
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
				SaveBarcodeData(lLoadBlock, m_szBCName);
			}
		}
		else
		{
			BL_DEBUGBOX("Compare barcode ...");

			if ( bUseEmpty == TRUE )	//EMPTY frame from EMPTY mag
			{
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
				SaveBarcodeData(lLoadBlock, m_szBCName);
			}
			else if ( GetCurrMgznSlotBC().IsEmpty() == TRUE )	//EMPTY frame from GRADE mag
			{
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
				SaveBarcodeData(lLoadBlock, m_szBCName);
			}
			else						//GRAGE frames with dices
			{
				CString szSlotBC = GetCurrMgznSlotBC();

				if( (m_bSemitekBLMode) && 
					(szSlotBC == BL_DEFAULT_BARCODE || szSlotBC.IsEmpty()) &&
					(m_szBCName == BL_DEFAULT_BARCODE) )
				{
					CString szText = "\n";
					AlertMsgForManualInputBarcode(lLoadBlock, m_lCurrMgzn, m_lCurrSlot, m_szBCName, szText);
					SetErrorMessage(m_szBCName + " input by operator");
					BL_DEBUGBOX("CPBC DB Load input BC " + m_szBCName);
				}

				szSlotBC = GetCurrMgznSlotBC();


				//v4.59A15
				//Requested by Roy Ip and Leo Lam, when barcode is not readable,
				//Do not use the original slot BC, just UNLOAD back;

//				if( m_bSemitekBLMode && (szSlotBC!=m_szBCName) )
//				{
//					if( (szSlotBC.IsEmpty()==FALSE) && (m_szBCName.IsEmpty()==FALSE) &&
//						(szSlotBC==BL_DEFAULT_BARCODE || m_szBCName==BL_DEFAULT_BARCODE) )
//					{
//						BL_DEBUGBOX("CPBC DB Load force BC " + m_szBCName + " slot " + szSlotBC);
//						if( szSlotBC==BL_DEFAULT_BARCODE )
//							m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
//						if( m_szBCName==BL_DEFAULT_BARCODE )
//							m_szBCName = szSlotBC;
//						BL_DEBUGBOX("CPBC DB Load after BC " + m_szBCName);
//					}
//				}

				//v4.21T3	//TongFang //Re-scan barcode if not the same as old one
				if ( GetCurrMgznSlotBC() != m_szBCName )
				{
					CString szOldBC		= m_szBCName;
					CString szSlotBC	= GetCurrMgznSlotBC();

					BL_DEBUGBOX("#1 Rescan barcode - curr = " + m_szBCName);
					CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
					lStatus = ScanningBarcode(pGripperBuffer);
					BL_DEBUGBOX("#1 Rescan barcode done - new = " + m_szBCName);

					//v4.21T6
					CString szErr;
					szErr.Format("RESCAN BC: MGZN #%ld, SLOT #%ld - Slot BC = " + szSlotBC, m_lCurrMgzn, m_lCurrSlot+1);
					szErr = szErr + "; Scan-BC = " + szOldBC + "; ReScan-BC = " + m_szBCName;
					CMSLogFileUtility::Instance()->BL_BarcodeLog(szErr);
				}

				//Compare new and old barcode
				if ( GetCurrMgznSlotBC() != m_szBCName )
				{
					CString szText;	
					LONG lOption;

					//v3.70T2	//PLLM special feature
					ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
					//if (nPLLM == PLLM_LUMIRAMIC)
					if (pApp->GetProductLine() == "Lumiramic")		//v4.35T1
						bPLLMSpecialFcn = TRUE;		//Lumiramic Dual-DLA also uses REBEL sequence!!		//v4.02T4

					if( pApp->IsBLBCUseOldGoStop() )
						bPLLMSpecialFcn = TRUE;

					szText = "Load Bin Frame\n";
					szText += "Old: [@@" + GetCurrMgznSlotBC()  + "@@]\nNew: [@@" + m_szBCName + "@@]";

					if (bPLLMSpecialFcn)	
					{
						if ( (m_szBCName == "") || (m_szBCName == BL_DEFAULT_BARCODE) )	//v3.71T7
						{
							szText = "Output frame barcode cannot be read!\n";
							szText += "old label: " + GetCurrMgznSlotBC();
						}
						else
						{
							szText = "Barcode read on output frame is different from\n";
							szText += "old label: " + GetCurrMgznSlotBC();
						}
						lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "CONTINUE", "STOP");	//v3.70T2
					}
					else if (pApp->GetCustomerName() == "SanAn")	//v4.49A3
					{
						szText.Format("SanAn: BC on frame is different from old label: READ (%s), OLD (%s)",
							m_szBCName, GetCurrMgznSlotBC());
						SetErrorMessage(szText);
						SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText);		
						lOption = -1;		
					}
					else if (m_bSemitekBLMode)	//v4.42T7
					{
						szText = "Barcode read on output frame is different from\n";
						szText += "old label: " + GetCurrMgznSlotBC();
						SetErrorMessage("Semitek: " + szText);
						BL_DEBUGBOX("Semitek: " + szText);
						lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Stop", "", NULL, glHMI_ALIGN_LEFT);		
					
						PushBackFrameFromGripper_DL(bBurnInLoad);
						if (bBufferLevel == BL_BUFFER_UPPER)
						{
							m_clUpperGripperBuffer.SetBufferBlock(0);
						}
						else if (bBufferLevel == BL_BUFFER_LOWER)
						{
							m_clLowerGripperBuffer.SetBufferBlock(0);
						}

						HouseKeeping(m_lReadyPos_X, FALSE);
						return FALSE;	//STOP
					}
					else
					{
						CString szMsg;
						szMsg = szText;
						szMsg.Replace('\n', ' ');
						SetStatusMessage(szMsg);
						lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Old", "New");		
					}

					BL_DEBUGBOX(szText);
					switch(lOption)
					{
					case -1:	//ABORT
						PushBackFrameFromGripper_DL(bBurnInLoad);	//v4.13T6
						if (bBufferLevel == BL_BUFFER_UPPER)
						{
							m_clUpperGripperBuffer.SetBufferBlock(0);
						}
						else if (bBufferLevel == BL_BUFFER_LOWER)
						{
							m_clLowerGripperBuffer.SetBufferBlock(0);
						}
						HouseKeeping(m_lReadyPos_X, FALSE);
						SetStatusMessage("DBLMB Compare barcode fail: Abort");
						BL_DEBUGBOX("LOAD: barcode comparison fails -> abort");	
						return FALSE;

					case 1:
						BL_DEBUGBOX("LOAD: barcode comparison fails -> use old: " + GetCurrMgznSlotBC());	
						if (bPLLMSpecialFcn)	//v3.70T2
						{
							SaveBarcodeData(lLoadBlock, GetCurrMgznSlotBC());
						}

						m_szBCName = GetCurrMgznSlotBC();
						SetStatusMessage("DBLMB Compare barcode fail: Use old barcode -> " + m_szBCName);
						break;

					case 2:		//NEW or STOP
						if (bPLLMSpecialFcn)	//v3.70T2
						{
							//Push back frame into magazine	//v3.71T3
							SetGripperState(FALSE);
							BinGripperMoveTo(m_lLoadPos_X);
							Sleep(500);

							if (m_bIsDualBufferExist)	//Dual-Buffer config	//v3.71T5
							{
								if (bBufferLevel == BL_BUFFER_UPPER)
								{
									m_clUpperGripperBuffer.SetBufferBlock(0);
								}
								else if (bBufferLevel == BL_BUFFER_LOWER)
								{
									m_clLowerGripperBuffer.SetBufferBlock(0);
								}
							}

							HouseKeeping(m_lReadyPos_X, FALSE);
							BL_DEBUGBOX("LOAD: PLLM barcode comparison fails -> abort");	
							//m_bBarcodeReadAbort = TRUE;
							//SetAlarmLamp_Yellow();		//andrew1234
							return FALSE;
						}
						else
						{
							BL_DEBUGBOX("LOAD: barcode comparison fails -> new");	
							m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
							SetStatusMessage("DBLMB Compare barcode fail: Use new barcode -> " + m_szBCName);
							SaveBarcodeData(lLoadBlock, m_szBCName);
						}
						break;
					}
				}
				else	//v4.13T6
				{
					//Else BC comparison is OK
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
					SaveBarcodeData(lLoadBlock, m_szBCName);
				}

			}//end ELSE
		}//end ELSE (m_bCompareBarcode)
		m_szLoadReadBarcode = m_szBCName;
	}//If (bUseBarcode)


	//v4.15T9	//Walsin LiWha
	//No matter any gripper errors happened below, the frame is assumed LOAD into buffer,
	//  so slot status has to be updated for EMPTY frame here
	if( bUseEmpty==TRUE )
	{
		szText1.Format("S%d", m_lCurrSlot+1);
		str = "Upate EMPTY frame slot status for " + GetMagazineName(m_lCurrMgzn).MakeUpper() + ", " + szText1 + " (E), BLBarcode = " + m_szBCName;
		BL_DEBUGBOX(str);

		switch( m_lOMRT )
		{
		case BL_MODE_A:
		case BL_MODE_E:
		case BL_MODE_F:		//MS100 8mag 150bins config
		case BL_MODE_G:		//MS100 8mag 175bins config
			m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE;
			break;
		case BL_MODE_H:		//MS100 4mag 100bins config		//v4.31T10
			m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE;
			break;
		}
	}

	BL_DEBUGBOX("gripper move to buffer unload");
    if ( BinGripperMoveTo(m_lBufferUnloadPos_X - 50) == FALSE )		//v3.87		//Additional 50 steps is added
	{
		HouseKeeping(m_lReadyPos_X, FALSE);
		return FALSE;
	}

	if (bBurnInLoad == FALSE && !IsFrameInClamp())
	{
		Sleep(500);
		if (!IsFrameInClamp())	//v4.59A13
		{
			SetGripperState(FALSE);
			SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);		
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}
	}
    
	SetGripperState(FALSE);
    Sleep(500);		//v4.59A13
	

	BL_DEBUGBOX("gripper move to ready");
    if ( BinGripperMoveTo(m_lReadyPos_X) == FALSE )
	{
		HouseKeeping(m_lReadyPos_X, FALSE);
		return FALSE;
	}


	if (bBufferLevel == BL_BUFFER_UPPER)
	{
		//Check upper Buffer has frame or not
		bFrameExist = IsDualBufferUpperExist();
		if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			//v4.16T2
			CString szTemp;
			szTemp.Format("Mgz->Buf: Frame #%d not detected in upper buffer slot", m_clUpperGripperBuffer.GetBufferBlock());
			BL_DEBUGBOX(szTemp);

			if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
			{
				//v4.16T2
				szTemp.Format("Mgz->Buf: Frame #%d aborted due to frame not detected on upper buffer slot", m_clUpperGripperBuffer.GetBufferBlock());
				BL_DEBUGBOX(szTemp + "\n");
				SetErrorMessage(szTemp);
				
				// V3.30 Alarm
				//SetAlarmLamp_Yellow(FALSE);
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
		}

		m_clUpperGripperBuffer.SetBufferInfo(lLoadBlock, m_szBCName);
	}
	else if (bBufferLevel == BL_BUFFER_LOWER)
	{
		//Check upper Buffer has frame or not
		bFrameExist = IsDualBufferLowerExist();
		if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			//v4.16T2
			CString szTemp;
			szTemp.Format("Mgz->Buf: Frame #%d not detected in lower buffer slot", m_clLowerGripperBuffer.GetBufferBlock());
			BL_DEBUGBOX(szTemp);

			if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
			{
				//v4.16T2
				szTemp.Format("Mgz->Buf: Frame #%d aborted due to frame not detected on lower buffer slot", m_clLowerGripperBuffer.GetBufferBlock());
				BL_DEBUGBOX(szTemp + "\n");
				SetErrorMessage(szTemp);

				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
		}

		m_clLowerGripperBuffer.SetBufferInfo(lLoadBlock, m_szBCName);
	}

	//Check bin frame sits in the center on the buffer
	if (m_bIsNewDualBuffer)
		bFrameProtect = !IsDualBufferLeftProtect();
	else
		bFrameProtect = IsDualBufferLeftProtect() && IsDualBufferRightProtect();

	if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
	{
		bFrameProtect = TRUE;
	}

	if (bFrameProtect == FALSE)
	{
		//PLLM v3.74T31
		Sleep(200);
		if (m_bIsNewDualBuffer)
			bFrameProtect = !IsDualBufferLeftProtect();
		else
			bFrameProtect = IsDualBufferLeftProtect() && IsDualBufferRightProtect();

		if (bFrameProtect == FALSE)
		{
			//v4.16T2
			CString szTemp;
			szTemp.Format("Mgz->Buf: Frame #%d not detected at CENTER position", lLoadBlock);
			BL_DEBUGBOX(szTemp);

			if ( BL_YES_NO(HMB_BL_BUFF_FRAME_NOT_IN_CENTER_CONTINUE, IDS_BL_LOAD_FRAME) == FALSE )
			{
				//v4.16T2
				szTemp.Format("Mgz->Buf: Frame #%d aborted due to frame not detected on CENTER position", lLoadBlock);
				BL_DEBUGBOX(szTemp + "\n");
				SetErrorMessage(szTemp);
				SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_IN_CENTER);	

				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
			else
			{
				//check again for safety
				if (m_bIsNewDualBuffer == FALSE)
				{
					bFrameProtect = IsDualBufferLeftProtect() && IsDualBufferRightProtect();
				}
				else
				{
					bFrameProtect = !IsDualBufferLeftProtect();
				}
				if (bFrameProtect == FALSE)
				{
					SetErrorMessage("Mgz->Buf: Frame does not sit in the buffer center");
					SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_IN_CENTER);		
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}
			}
		}
	}

	if (bBufferLevel == BL_BUFFER_LOWER)
	{		
		BL_DEBUGBOX("down buffer");
		SetBufferLevel(FALSE);
		Sleep(200);

		if ( IsDualBufferLevelDown() == FALSE)
		{
			SetBufferLevel(FALSE);
			Sleep(300);

			//v3.71T1	//Insert 1sec timeout
			INT nCount = 0;
			while (!IsDualBufferLevelDown())
			{
				Sleep(50);
				nCount++;
				if (nCount > 20)
					break;
			}

			if ( IsDualBufferLevelDown() == FALSE)
			{
				Sleep(100);
				if ( IsDualBufferLevelDown() == FALSE)
				{
					SetErrorMessage("BL Buffer is not down 4");
					SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_DOWN);
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
			}
		}
	}

	if( bUseEmpty==TRUE )
	{
		switch( m_lOMRT )
		{
		case BL_MODE_A:
		case BL_MODE_E:
		case BL_MODE_F:		//v3.82		//MS100 8mag 150bins config
		case BL_MODE_G:		//v3.82		//MS100 8mag 175bins config
			m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE;
			break;
		case BL_MODE_H:		//v4.31T10	//MS100 4mag 100bins config
			m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE;
			break;
		}

		szText1.Format("S%d", lSlot+1);
		str = "#1 BL FilmFrame is loaded from " + GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + " (E), BLBarcode = " + m_szBCName;

		//Send bin table Load event
		SendBinTableLoadEvent_8010(m_lCurrMgzn, m_lCurrSlot, m_szBCName, "B1T", "MT");

	}
	else
	{
		if ( IsMapDetectSkipMode() )	
		{
			LONG lOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade((UCHAR)(lLoadBlock + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()))
								- CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
			szText1.Format("S%d, B%d(%ld)", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot], lOrigGrade);
		}
		else
		{
			szText1.Format("S%d, B%d", lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		}
		str = "#2 BL FilmFrame is loaded from " + GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 +  ", BLBarcode =" + m_szBCName;

		//ANDREW_SC
		//Set SECSGEM Value
		ULONG ulBindieCount = 0;
		IPC_CServiceMessage stMsg;
		LONG lBinNo = m_stMgznRT[m_lCurrMgzn].m_lSlotBlock[m_lCurrSlot];
		CString szCassettePos =	GetCassettePositionName(lMgzn);

		SendBinCasseteUnloadWIPEvent_8015(m_lCurrMgzn, m_lCurrSlot, m_szBCName, szCassettePos);
		//Set SECSGEM Value
		if (lMgzn == BL_MGZ_MID_1)		//lMgzn is 0-based
		{
			SECS_UpdateCassetteSlotInfo(lSlot + 1, TRUE);		//v4.59A10	//Autoline
		}

		//Send bin table Load event
		SendBinTableLoadEvent_8010(m_lCurrMgzn, m_lCurrSlot, m_szBCName, szCassettePos, "WP");
	}

	SetStatusMessage(str);
    BL_DEBUGBOX(str);

	szBinBlkGrade.Format("%d", lLoadBlock);
	if (!m_bDisableBinFrameStatusSummaryFile)	//v4.53A5	//Dicon
	{
		CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Load Frame", GetMagazineName(lMgzn).MakeUpper(), 
			lSlot+1, m_szBCName, szBinBlkGrade, GetBinBlkBondedCount(lSlot + 1), m_bUseBarcode);
	}

	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData on LoadFrame !!!");
	}
	END_CATCH

	szText1.Format("#1 U.Buffer = %d BC %s, L.Buffer = %d BC %s, Current = %d BC %s",
		m_clUpperGripperBuffer.GetBufferBlock(), m_clUpperGripperBuffer.GetBufferBarcode(),
		m_clLowerGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBarcode(),
		GetCurrBinOnBT(), m_szBinFrameBarcode);
	BL_DEBUGBOX(szText1);
    BL_DEBUGBOX("Load from Mgn to Buffer finish\n");
	return TRUE;
}


//Buffer->Magazine
LONG CBinLoader::DB_UnloadFromBufferToMgz(BOOL bBurnInUnload, BOOL bBinFull, BOOL bClearBin, BOOL bIsEmptyFrame, 
										  BOOL bIsMgzPreMove, BOOL bBufferLevel, BOOL bCheckRealignRpy, BOOL bXStopAtHome,
										  BOOL bBinFullExchange, BOOL bIsManualUnload)
{
	LONG lFullStatus = 1;
	LONG lMgzn, lSlot;
	LONG lStatus = 0;
    BOOL bModeDError;
	BOOL bScanBarcode = TRUE;
	BOOL bFrameExist = TRUE;
    CString str;
	CString szText1, szText2;
	LONG lDualBufferTempBlock = m_clUpperGripperBuffer.GetBufferBlock();	//Klocwork
	CString szBinBlkGrade;
	LONG lBinNo = 0;
	BOOL bRealignBin = TRUE;
	BOOL bContinueAort = TRUE;
	BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];	//v4.39T7


	str.Format("Unload from Buffer to Mgz start: BinFull=%d, ClearBin=%d, IsEmptyFrame=%d, CheckReallignRpy=%d",
					bBinFull, bClearBin, bIsEmptyFrame, bCheckRealignRpy);
	BL_DEBUGBOX(str);

	//Check Gripper position is valid
	if ( IsMagazineSafeToMove() == FALSE )
	{
		SetErrorMessage("Gripper is not in safe position");
		SetAlert_Red_Yellow(IDS_BL_GRIPPER_NOT_SAFE);

        return FALSE;
	}

	if (bBinFullExchange)
	{
	}
	else
	{
		if (!bCheckRealignRpy)		//andrew123456	//no need to move to READY pos in AUTOBOND during exchange frame
		{
			BL_DEBUGBOX("gripper move to ready");
			if ( BinGripperMoveTo_Auto(m_lReadyPos_X, SFM_NOWAIT) == FALSE )
			{
				return FALSE;
			}
		}
	}

	if (bBufferLevel == BL_BUFFER_LOWER)	//normal sequence
	{
		// check lower frame on buffer table logically
		if (!IsAutoLoadUnloadQCTest() && !m_clLowerGripperBuffer.GetBufferBlock())
		{
			SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_LBUFFER_FRAME);	//andrew78
			SetErrorMessage("BL Lower buffer block is zero");
			return FALSE;
		}

		//Safety checking, upper buffer must be empty when unloading lower buffer frame to avoid crash
		if (IsDualBufferUpperExist())
		{
			SetErrorMessage("BL Frame exists in Upper Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			SetErrorMessage("BL Frame exists in Upper Buffer");
			return FALSE;
		}

		//Check Lower Buffer has frame or not
		bFrameExist = IsDualBufferLowerExist();
		if ( (bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) )
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
			{
				return FALSE;
			}
		}

		lDualBufferTempBlock = m_clLowerGripperBuffer.GetBufferBlock();
	}
	else if (bBufferLevel == BL_BUFFER_UPPER)
	{
		// check lower frame on buffer table logically
		if (!m_clUpperGripperBuffer.IsBufferBlockExist())
		{
			if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
			{
				SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_UBUFFER_FRAME);	//andrew78
				SetErrorMessage("BL Upper buffer block is zero");
			}
			return FALSE;
		}

		//Check Upper Buffer has frame or not
		bFrameExist = IsDualBufferUpperExist();
		if ( (bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) )
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
			{
				return FALSE;
			}
		}

		lDualBufferTempBlock = m_clUpperGripperBuffer.GetBufferBlock();
	}

	bModeDError = FALSE;
    if( m_lOMRT==BL_MODE_B )
        bBinFull = TRUE;

	if ( bBinFull == TRUE )
	{
		BL_DEBUGBOX("Unload(FULL)");
	}
	else
	{
		BL_DEBUGBOX("Unload(NORM)");
	}

	lBinNo = lDualBufferTempBlock;

    if( bBinFull==TRUE )
    {
		//Get current buffer block original magazine & slot No
		BOOL bUseEmpty;
		if ( GetLoadMgzSlot(bUseEmpty, lDualBufferTempBlock) == FALSE )
		{
			CString szMsg;
			szMsg.Format("\n%d",lDualBufferTempBlock);
			SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
			SetErrorMessage("BL No slot is assigned to this physical block");
			return FALSE;
		}

		LONG lMgzn = m_lCurrMgzn;
		LONG lSlot = m_lCurrSlot;
		lFullStatus = GetFullMgznSlot(lMgzn, lSlot, FALSE, lDualBufferTempBlock);	//v4.59A20

		if ( lFullStatus == FALSE )
		{
			return FALSE;
		}

		str.Format("Unload filmframe to full M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
		BL_DEBUGBOX(str);	//v4.41T1
	}
    else
    {
		if (bIsEmptyFrame)
		{
			//andrew1234
			//DO not use GetNullMgznSlot to increment index by 1
			//if (GetNullMgznSlot(lMgzn, lSlot) == FALSE)
			if (GetNextNullMgznSlot(lMgzn, lSlot) == FALSE)
			{
				CString szMsg;
				szMsg.Format("\n%d",lDualBufferTempBlock);
				SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
				SetErrorMessage("BL No slot is assigned to this physical empty block");
				return FALSE;
			}

			if (m_lOMRT != BL_MODE_H)
				lSlot = lSlot - 1;		// Unload to last slot instead
		}
		else
		{
			BOOL bUseEmpty;
			lFullStatus = GetLoadMgzSlot(bUseEmpty, lDualBufferTempBlock);

			if ( lFullStatus == FALSE )
			{
				CString szMsg;
				szMsg.Format("\n%d",lDualBufferTempBlock);
				SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
				SetErrorMessage("BL No slot is assigned to this physical block");
				return FALSE;
			}

			lMgzn = m_lCurrMgzn;
			lSlot = m_lCurrSlot;
			str.Format("Unload filmframe to M%d, S%d, B%d", lMgzn, lSlot+1, m_stMgznRT[lMgzn].m_lSlotBlock[lSlot]);
			BL_DEBUGBOX(str);	//v4.41T1
		}
    }
    BL_DEBUGMESSAGE(str);

    if ( CheckElevatorCover() == FALSE )
    {
		return FALSE;
    }

	if (bCheckRealignRpy && (m_nExArmReAlignBinConvID != 0))	//v3.98T3 added ID checking here
		bRealignBin = RealignBinFrameRpy(TRUE);
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bNewMagCheck = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_NEW_BL_MAGEXIST_CHECKING);
	
	// wait loader move complete
	if (bIsManualUnload == TRUE)
	{
		if (bNewMagCheck)
		{
			BL_DEBUGBOX("magazine to unload (NEW)");
			if ( MoveElevatorToUnload(lMgzn, lSlot, FALSE, TRUE) == FALSE )
			{
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
		}
		else
		{
			BL_DEBUGBOX("magazine to unload");
			if ( MoveElevatorToUnload(lMgzn, lSlot, TRUE, TRUE) == FALSE )
			{
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
		}
	}
	else
	{
		if (!bIsMgzPreMove)
		{
			if (bNewMagCheck)	//v3.58		//New MagExist checking sequence for production
			{
				BL_DEBUGBOX("magazine to unload (NEW)");
				if ( MoveElevatorToUnload(lMgzn, lSlot, FALSE, FALSE) == FALSE )
				{
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}
			}
			else
			{
				BL_DEBUGBOX("magazine to unload");
				if ( MoveElevatorToUnload(lMgzn, lSlot, TRUE, FALSE) == FALSE )
				{
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}
			}
		}
	}

	//andrew7	//xyz
	if (bBufferLevel == BL_BUFFER_LOWER)
	{
		if( BinGripperSynMove_Auto()==FALSE )
			return FALSE;
	}


	if (bBufferLevel == BL_BUFFER_LOWER)
	{
		BL_DEBUGBOX("buffer up");
		SetBufferLevel(TRUE);
		Sleep(100);

		if ( IsDualBufferLevelDown() == TRUE)	//check whether buffer table still in down level
		{
			SetBufferLevel(TRUE);
			Sleep(100);

			if ( m_bNoSensorCheck || m_bDisableBL )		//v3.94
			{
			}
			else
			{
				//v3.71T1	//Insert 1sec timeout
				INT nCount = 0;
				while (IsDualBufferLevelDown())
				{
					Sleep(50);
					nCount++;
					if (nCount > 20)
						break;
				}

				if ( IsDualBufferLevelDown() == TRUE)
				{
					Sleep(100);
					if ( IsDualBufferLevelDown() == TRUE)
					{
						SetErrorMessage("BL Buffer is not up 5");
						SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_UP);
						HouseKeeping(m_lReadyPos_X, FALSE);
						return FALSE;
					}
				}
			}
		}
	}
	else if (bBufferLevel == BL_BUFFER_UPPER)
	{
		if ( IsDualBufferLevelDown() == FALSE)	//check buffer table need to be down or not
		{
			BL_DEBUGBOX("buffer down");
			SetBufferLevel(FALSE);
			Sleep(50);		//andrew123456

			if ( IsDualBufferLevelDown() == FALSE)	//check whether buffer table still in up level
			{
				SetBufferLevel(FALSE);
				Sleep(50);

				//v3.71T1	//Insert 1sec timeout
				INT nCount = 0;
				while (!IsDualBufferLevelDown())
				{
					Sleep(50);
					nCount++;
					if (nCount > 20)
						break;
				}

				if ( IsDualBufferLevelDown() == FALSE)
				{
					Sleep(100);
					if ( IsDualBufferLevelDown() == FALSE)
					{
						SetErrorMessage("BL Buffer is not down 6");
						SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_DOWN);
						HouseKeeping(m_lReadyPos_X, FALSE);
						return FALSE;
					}
				}
			}
		}
	}

	//andrew7
    if( BinGripperSynMove_Auto()==FALSE )
        return FALSE;

	if ( (m_nExArmReAlignBinConvID != 0) && (bCheckRealignRpy) )
		bRealignBin = RealignBinFrameRpy(TRUE);

	//Determine to use barcode or not
	bScanBarcode = m_bUseBarcode;
	if ( bScanBarcode == TRUE )
	{
		if ( (bBinFull == FALSE) && (m_bStopChgGradeScan == TRUE) )
		{
			bScanBarcode = FALSE;
		}
		if (bIsEmptyFrame)				//Back to EMPTY magazine, so no need to update barcode
		{
			bScanBarcode = FALSE;
		}

		//v4.20
		//if (bBurnInUnload)				//v4.13T6
		//	bScanBarcode = FALSE;

		BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];
		if (bEnableBtT && !IsMS90())		//v4.39T7	//Nichia MS100+
		{
			//With Bt T the barcode scanning will be performed on table instead of gripper;
			//so here the barcode scanning on gripper is disabled
			bScanBarcode = FALSE;
		}

		if (bScanBarcode)
			m_szBCName = "";
	}


	BL_DEBUGBOX("Searching frame");
	LONG lReturn = TRUE;
	LONG lOffsetX = m_lUnloadOffsetX;
	if (lOffsetX > 0)
	{
		lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, lOffsetX+100, 120, TRUE);

		//v4.19		//Ubilux
		if (IsFrameOutOfMgz() == TRUE)
		{
			AbortElevatorMotionIfOutOfMag();
			SetErrorMessage("BL Frame is out of magazine");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}
	}
	else
	{
		//lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, abs(m_lBufferUnloadPos_X-m_lReadyPos_X)+100, 80, FALSE);
		if (bBinFullExchange)		//andrew7	//xyz	//no need to move to READY pos in AUTOBOND during exchange frame
		{
		}
		else
		{
			if (!bCheckRealignRpy) 
			{
				BL_DEBUGBOX("Gripper to Buffer-UNLOAD posn");
				BinGripperMoveTo(m_lBufferUnloadPos_X);	// - 200);
			}
		}

		//v4.19	//CMLT
		if (IsFrameOutOfMgz() == TRUE)
		{
			AbortElevatorMotionIfOutOfMag();
			SetErrorMessage("BL Frame is out of magazine 2");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}

		if (m_bNoSensorCheck || bBurnInUnload)
		{
			BL_DEBUGBOX("Searching frame in Clamp (BURNIN)");
			lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, 100, 50, TRUE);
		}
		else
		{
			if (m_szDBPreScanBCName == "")	//andrew123456
			{
				BL_DEBUGBOX("Searching frame in Clamp");
				//m_szBCName = "";	//andrew1234
				lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, 1000, 50, TRUE, FALSE);	//andrew1234
			}
		}
		//Sleep(100);		//andrew123
	}
	

//v2.83T2
if ( (m_nExArmReAlignBinConvID != 0) && (bCheckRealignRpy) )
	bRealignBin = RealignBinFrameRpy(TRUE);		//andrew1234
	//X_Sync();
	
	//v4.22T2	//CMLT
	if (IsFrameOutOfMgz() == TRUE)
	{
		AbortElevatorMotionIfOutOfMag();
		SetErrorMessage("BL Frame is out of magazine 3");
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		HouseKeeping(m_lReadyPos_X, FALSE);
		return FALSE;
	}

	if (lReturn == FALSE)
	{
		//if( bBurnInUnload==FALSE && BL_OK_CANCEL("Please pull frame into gripper", "Unload Frame Message")==FALSE )
		if( bBurnInUnload==FALSE && 
			!m_bDisableBL )
		{
			SetErrorMessage("BL No frame is detected in gripper at buffer-UNLOAD");		//andrew1234
			SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}
	}
	

	//andrew1234
	if (bScanBarcode && (m_szDBPreScanBCName != ""))		//andrew1234	//Prescan OK!
	{
		BL_DEBUGBOX("Prescan barcode OK");
		m_szBCName = m_szDBPreScanBCName;
		m_szDBPreScanBCName = "";
	}
	else
	{
		BL_DEBUGBOX("Prescan barcode fail; gripper ON");
		Sleep(50);		//andrew1234
		SetGripperState(TRUE);		//Grab frame
		Sleep(100);
	}

	//v4.19	//CMLT
	if (IsFrameOutOfMgz() == TRUE)
	{
		AbortElevatorMotionIfOutOfMag();
		SetErrorMessage("BL Frame is out of magazine 4");
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		HouseKeeping(m_lReadyPos_X, FALSE);
		BL_DEBUGBOX("BL Frame is out of magazine 4");
		return FALSE;
	}

	//if (m_bUseBarcode == TRUE)
	if ((bScanBarcode == TRUE) && (m_szBCName == ""))	//andrew1234
	{
		BL_DEBUGBOX("#4 scan barcode");

		//v4.02T6	//Support New BC prescan fcn before SEARCH-motion completion
		LONG lGripper = 0;
#ifndef NU_MOTION
		lGripper = BinGripperSearchJam(HP_POSITIVE_DIR, m_lBarcodePos_X, 400);
#else
		//lGripper = BinGripperMoveSearchJam(m_lBarcodePos_X);
		lGripper = BinGripperMoveSearchJam(m_lBarcodePos_X, FALSE, TRUE);
#endif
		if ( lGripper == FALSE )
		{
#ifndef NU_MOTION
			//v4.45T2
			SetLoaderXPower(FALSE);
			if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
			{
				BL_DEBUGBOX("Andrew: GripperJam for BH triggered");
				//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
				(*m_psmfSRam)["BinLoaderStn"]["GripperJam"] = TRUE;
			}
			else
			{
				CString szErr;
				szErr.Format("Else State = %d; Ctm = " + pApp->GetCustomerName(), State());
				BL_DEBUGBOX(szErr);
			}
#endif		
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam when scanning BC");
			BL_DEBUGBOX("Gripper is jammed when scanning BC");
			HouseKeeping(m_lReadyPos_X, FALSE, TRUE);	//v3.77		//Home gripper to avoid missing step afterwards
			return FALSE;	
		}
		else
		{
			CString szErr;
			szErr.Format("Else Gripper State = %d is OK", lGripper);
			BL_DEBUGBOX(szErr);
		}

		if (!m_szDBPreScanBCName.IsEmpty())				//v4.02T6	//New BC prescan before SEARCH motion complete
		{
			m_szBCName	= m_szDBPreScanBCName;
			m_szDBPreScanBCName = _T("");
			lStatus		= TRUE;
		}
		else
		{
			CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
			lStatus = ScanningBarcode(pGripperBuffer);
			if (lStatus == TRUE)
			{
				BL_DEBUGBOX("Barcode read OK at UNLOAD");
				//CMSLogFileUtility::Instance()->BL_BarcodeLog("Barcode read OK at UNLOAD");	//v2.78T2
			}
			if ( lStatus == FALSE )
			{
				BL_DEBUGBOX("No barcode read");

				//if (bBinFull)
					//CMSLogFileUtility::Instance()->BL_BarcodeLog("Barcode read FAIL at UNLOAD FULL frame");	//v2.78T2
				//else
					//CMSLogFileUtility::Instance()->BL_BarcodeLog("Barcode read FAIL at UNLOAD");	//v2.78T2
			}
			else if ( lStatus == -1 )
			{
#ifndef NU_MOTION
				if (IsFrameJam())	//v4.45T2
				{
					SetLoaderXPower(FALSE);
					if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
					{
						BL_DEBUGBOX("Andrew: GripperJam for BH triggered");
						//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
						(*m_psmfSRam)["BinLoaderStn"]["GripperJam"] = TRUE;
					}
				}
#endif
				BL_DEBUGBOX("No barcode read 3");
				return FALSE;
			}
		}
	}

BL_DEBUGBOX("andrew 1");		//v4.41T2
	if ( (m_nExArmReAlignBinConvID != 0) && bCheckRealignRpy )
			bRealignBin = RealignBinFrameRpy(TRUE);	
BL_DEBUGBOX("andrew 2");		//v4.41T2

	//v4.19		//Ubilux
	if (IsFrameOutOfMgz() == TRUE)
	{
		AbortElevatorMotionIfOutOfMag();
		SetErrorMessage("BL Frame is out of magazine 4");
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		HouseKeeping(m_lReadyPos_X, FALSE);
		return FALSE;
	}

	if (bScanBarcode == TRUE)
	{
		//v4.13T6
		if (m_bCheckBarcode)
		{
			CString szSlotBC = GetCurrMgznSlotBC();
			if( m_bSemitekBLMode && (szSlotBC != m_szBCName) )
			{
				if( (szSlotBC.IsEmpty()==FALSE) && (m_szBCName.IsEmpty()==FALSE) &&
					(szSlotBC==BL_DEFAULT_BARCODE || m_szBCName==BL_DEFAULT_BARCODE) )
				{
					BL_DEBUGBOX("CPBC DB UnLoad force BC " + m_szBCName + " slot " + szSlotBC);
					if( szSlotBC==BL_DEFAULT_BARCODE )
						m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
					if( m_szBCName==BL_DEFAULT_BARCODE )
						m_szBCName = szSlotBC;
					BL_DEBUGBOX("CPBC DB UnLoad after BC " + m_szBCName);
				}
			}

			//v4.42T9	//Cree
			CString szErrCode;
			BOOL bCheckBCHistory = CheckBcHistoryInCurrLot(lDualBufferTempBlock, m_szBCName, FALSE, szErrCode);
			if (bBurnInUnload)
				bCheckBCHistory = TRUE;
			if (!bCheckBCHistory)
			{
				CString szErr = szErrCode;
				HmiMessage_Red_Yellow(szErr);		
				BL_DEBUGBOX(szErr);	

				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}

		}

		if (m_bCompareBarcode == FALSE)
		{
			//Only update Barcode name if this is not Default
			if ( m_szBCName != BL_DEFAULT_BARCODE )
			{
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
				SaveBarcodeData(lDualBufferTempBlock, m_szBCName, m_lCurrMgzn, m_lCurrSlot);
			}
		}
		else
		{
			CString szSlotBarcode = GetCurrMgznSlotBC(); 
			BL_DEBUGBOX("Unload dual buffer compare barcode :  READ = " + m_szBCName + "; SLOT = " + szSlotBarcode);

			//v4.21T3	//TongFang //Re-scan barcode if not the same as old one
			if ( GetCurrMgznSlotBC() != m_szBCName )
			{
				CString szOldBC		= m_szBCName;
				CString szSlotBC	= GetCurrMgznSlotBC();

				BL_DEBUGBOX("#2 Rescan barcode - curr = " + m_szBCName);
				CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
				lStatus = ScanningBarcode(pGripperBuffer);
				BL_DEBUGBOX("#2 Rescan barcode done - new = " + m_szBCName);

				//v4.21T6
				CString szErr;
				szErr.Format("RESCAN BC: MGZN #%ld, SLOT #%ld - Slot BC = " + szSlotBC, m_lCurrMgzn, m_lCurrSlot+1);
				szErr = szErr + "; Scan-BC = " + szOldBC + "; ReScan-BC = " + m_szBCName;
				CMSLogFileUtility::Instance()->BL_BarcodeLog(szErr);
			}

			if ( (m_szBCName == BL_DEFAULT_BARCODE ) ||	
				 (GetCurrMgznSlotBC() != m_szBCName) )
			{
				BL_DEBUGBOX("compare barcode fail");

				CString szText;	
				LONG lOption;

				//v3.70T2	//PLLM special feature
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				BOOL bPLLMSpecialFcn = pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
				ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);
				//if (nPLLM == PLLM_LUMIRAMIC)
				if (pApp->GetProductLine() == "Lumiramic")		//v4.35T1
					bPLLMSpecialFcn = TRUE;		//Lumiramic Dual-DLA also uses REBEL sequence!!		//v4.02T4

				if( pApp->IsBLBCUseOldGoStop() )
					bPLLMSpecialFcn = TRUE;

				szText = "#2 Unload Bin Frame\n";
				szText += "Old: [@@" + GetCurrMgznSlotBC()  + "@@]\nNew: [@@" + m_szBCName + "@@]";

				if (bPLLMSpecialFcn)
				{
					if ( (m_szBCName == "") || (m_szBCName == BL_DEFAULT_BARCODE) )	//v3.71T7
					{
						szText = "Output frame barcode cannot be read!\n";
						szText += "old label: " + GetCurrMgznSlotBC();
					}
					else
					{
						szText = "Barcode read on output frame is different from\n";
						szText += "old label: " + GetCurrMgznSlotBC();
					}
					lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "CONTINUE", "ABORT");		//v3.70T2	
				}
				else if (m_bSemitekBLMode)	//v4.42T7
				{
					szText = "Barcode read on output frame is different from\n";
					szText += "old label: " + GetCurrMgznSlotBC();
					SetErrorMessage("Semitek: " + szText);
					BL_DEBUGBOX("Semitek: " + szText);
					lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Stop", "", NULL, glHMI_ALIGN_LEFT);		
					lOption = -1;			//1;	//Use OLD	//v4.42T9
					bContinueAort = FALSE;	//ABORT	after Unload done
					
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

					if (m_bSemitekBLMode)	//v4.40T14
					{
						HmiMessage_Red_Back(szText, "Unload Frame");
						lOption = -1;
					}
					else
					{
						lOption = SetAlert_Msg_Red_Back(IDS_BL_SELECT_BARCODE, szText, "Old", "New");		
					}
				}

				BL_DEBUGBOX(szText);
				switch(lOption)
				{
					case -1:
						SetStatusMessage("DBULBM ompare barcode fail: Abort");
						BL_DEBUGBOX("barcode comparison fails -> abort");	
						SetErrorMessage("barcode comparison fails -> abort");
						HouseKeeping(m_lReadyPos_X, FALSE);		//assume frame still on buffer table
						m_szBCName = GetCurrMgznSlotBC();
						SaveBarcodeData(lDualBufferTempBlock, m_szBCName);
						return FALSE;

					case 1:		//OLD or CONTINUE
						BL_DEBUGBOX("barcode comparison fails -> use old: " + GetCurrMgznSlotBC() + " (" + m_szBCName + ")");		//v2.93T2
						SetErrorMessage("barcode comparison fails -> use old:" + GetCurrMgznSlotBC() + " (" + m_szBCName + ")");	//v3.65
						m_szBCName = GetCurrMgznSlotBC();
						SetStatusMessage("DBULBM Compare barcode fail: Use old barcode -> " + m_szBCName);
						SaveBarcodeData(lDualBufferTempBlock, m_szBCName);
						break;

					case 2:		//NEW or ABORT
						if (bPLLMSpecialFcn)	//v3.70T2
						{
							BL_DEBUGBOX("barcode comparison fails ABORT -> use old");	
							SetErrorMessage("barcode comparison fails ABORT -> use old");
							bContinueAort = FALSE;	//ABORT		//andrew1234
						}
						else
						{
							BL_DEBUGBOX("barcode comparison fails -> use new");	
							SetErrorMessage("barcode comparison fails -> use new");	
							m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
							SetStatusMessage("DBULBM Compare barcode fail: Use new barcode -> " + m_szBCName);
							SaveBarcodeData(lDualBufferTempBlock, m_szBCName);
						}
						break;
				}
			}
			else	//Compare BC OK!
			{
				//v3.93T1	//CMLT bug fix
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot] = m_szBCName;
				SaveBarcodeData(lDualBufferTempBlock, m_szBCName, m_lCurrMgzn, m_lCurrSlot);
				BL_DEBUGBOX("COMPARE barcode done and saved: " + m_szBCName);	//v4.19
			}

		}	//Else (m_bCompareBarcode)


		if (bBinFull)	//v3.65
		{
			CString szMsg;
			szMsg.Format("BIN-FULL Blk #%d (Mgzn %d Slot %d) : BC = ", lDualBufferTempBlock, m_lCurrMgzn, m_lCurrSlot);
			BL_DEBUGBOX(szMsg + GetCurrMgznSlotBC());
			SetErrorMessage(szMsg + GetCurrMgznSlotBC());

			szMsg = "BIN-FULL Barcode in " + MSD_BL_BARCODE_FILE + " (" + GetBarcodeData(lDualBufferTempBlock) + ")";
			BL_DEBUGBOX(szMsg);
			SetErrorMessage(szMsg);

			CString szCurrBarcode	= GetCurrMgznSlotBC();
			CString szStoredBarcode = GetBarcodeData(m_lExArmBufferBlock);

			if (bClearBin && (szStoredBarcode != szCurrBarcode))		//If need to clear bin counter (i.e. NOT in AUTOBOND mode)
			{
				szMsg.Format("BIN-FULL Blk #%d (Mgzn %d Slot %d) : BC updated (bClearBin) to ", m_lExArmBufferBlock, m_lCurrMgzn, m_lCurrSlot);
				BL_DEBUGBOX(szMsg + szCurrBarcode);
				SaveBarcodeData(m_lExArmBufferBlock, szCurrBarcode, m_lCurrMgzn, m_lCurrSlot);
			}
		}
	}

	//v4.19		//CMLT
	if (IsFrameOutOfMgz() == TRUE)
	{
		AbortElevatorMotionIfOutOfMag();
		SetErrorMessage("BL Frame is out of magazine\n");
		SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
		SetGripperState(FALSE);		//push frame into magazine to avoid jamming
		Sleep(50);
		HouseKeeping(m_lReadyPos_X, FALSE);
		return FALSE;
	}

BL_DEBUGBOX("andrew 3");		//v4.41T2

	//Y_Sync();
	//Z_Sync();
	if (bIsManualUnload == FALSE)
	{
		BOOL bStatus = YZ_Sync_OutOfMagChecking(TRUE);	//v4.49A6
		if (!bStatus)									//v4.49A6
		{
			SetErrorMessage("YZ_Sync_OutOfMagChecking fail");
			BL_DEBUGBOX("YZ_Sync_OutOfMagChecking fail\n");	
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}
	}


	if (!bIsMgzPreMove)
	{
BL_DEBUGBOX("andrew 4");		//v4.41T2
		if ( ElevatorZSynMove_Auto()==FALSE )
		{
			SetErrorMessage("ElevatorZSynMove_Auto fail");		//v4.41T2
			BL_DEBUGBOX("ElevatorZSynMove_Auto fail\n");		//v4.41T2
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}

		CString szMsg;
		if (bNewMagCheck)	//v3.58		//New MagExist checking sequence for production
		{
			if ( bBurnInUnload==FALSE && IsMagazineExist(lMgzn)==FALSE )
			{
				Sleep(200);	
				if (IsMagazineExist(lMgzn, FALSE, TRUE)==FALSE)	//v4.41T2
				{
					szMsg.Format("DB_UnloadFromBufferToMgz fail becuase Mgzn %ld sensor is not triggered", lMgzn);  
					SetErrorMessage(szMsg);		//v4.41T2
					BL_DEBUGBOX(szMsg + "\n");	//v4.41T2
					if (bScanBarcode)
						BinGripperMoveTo(m_lBufferUnloadPos_X);
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}
			}

			if ( MoveElevatorToUnload(lMgzn, lSlot, TRUE, TRUE) == FALSE )
			{
				SetErrorMessage("MoveElevatorToUnload fail");		//v4.41T2
				BL_DEBUGBOX("MoveElevatorToUnload fail\n");			//v4.41T2
				HouseKeeping(m_lReadyPos_X, FALSE);
				return FALSE;
			}
		}
		else
		{
			if ( bBurnInUnload==FALSE && IsMagazineExist(lMgzn)==FALSE )
			{
				Sleep(200);
				if (IsMagazineExist(lMgzn, FALSE, TRUE)==FALSE)	//v4.41T2
				{
					szMsg.Format("DB_UnloadFromBufferToMgz fail 2 becuase Mgzn %ld sensor is not triggered", lMgzn);  
					SetErrorMessage(szMsg);		//v4.41T2
					BL_DEBUGBOX(szMsg + "\n");	//v4.41T2
					HouseKeeping(m_lReadyPos_X, FALSE);
					return FALSE;
				}
			}
		}
	}

	if ( CheckElevatorCover() == FALSE )
    {
		BL_DEBUGBOX("Check elevator cover stop\n");
		HouseKeeping(m_lReadyPos_X, FALSE);
		return FALSE;
    }

	if (bEnableBtT && !IsMS90())	//v4.39T7	//v4.50A25
	{
		BL_DEBUGBOX("Gripper still ON to unload frame");
	}
	else
	{
		BL_DEBUGBOX("Gripper OFF to unload frame");
		SetGripperState(FALSE);		//push frame into magazine to avoid jamming
		Sleep(50);
	}


	//v4.15T9	//Walsin LiWha
	//Update slot status if frame is unloaded into grade slot only
	if (!bBinFull && !bClearBin && !bIsEmptyFrame)
	{
		CString szText1;
		szText1.Format("S%d", lSlot+1);
		str = "Upate UNLOAD slot status for " + GetMagazineName(lMgzn).MakeUpper() + ", " + szText1 + ", BLBarcode = " + m_szBCName;
		BL_DEBUGBOX(str);
	}


	BOOL bUnloadJam = FALSE;	//v3.77

	BL_DEBUGBOX("gripper move to load");
	if ( BinGripperMoveSearchJam(m_lLoadPos_X, TRUE) == FALSE )		//andrew1234
    {
		bUnloadJam = TRUE;
        SetGripperState(FALSE);

#ifndef NU_MOTION
		//v4.45T2
		if ( (pApp->GetCustomerName() == "Cree") && (State() != IDLE_Q) )
		{
			//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
			(*m_psmfSRam)["BinLoaderStn"]["GripperJam"] = TRUE;
		}
#endif

 		BL_DEBUGBOX("gripper jam at LOAD");
		CBinGripperBuffer *pGripperBuffer = (bBufferLevel == BL_BUFFER_UPPER) ? &m_clUpperGripperBuffer : &m_clLowerGripperBuffer;
		BOOL bOk = BinLoaderUnloadJamMessage_DB(pGripperBuffer, bBurnInUnload);

        if( bBurnInUnload==FALSE && 
			!m_bDisableBL && 
			bOk == FALSE )
        {
			//Assume frame is push into magazine, updated the status so that it will not put into same full slot
			if (bBinFull == TRUE)
			{
				if (bBufferLevel == BL_BUFFER_LOWER)
				{
					UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_LOWER);
					m_clLowerGripperBuffer.InitBufferInfo();
				}
				else if (bBufferLevel == BL_BUFFER_UPPER)
				{
					UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_UPPER);
					m_clUpperGripperBuffer.InitBufferInfo();
				}
			}
			else	//andrew123
			{
				//Has to clear because assume frame is pushed into magazine!!
				if (bBufferLevel == BL_BUFFER_UPPER)
				{
					m_clUpperGripperBuffer.InitBufferInfo();
				}
				else if (bBufferLevel == BL_BUFFER_LOWER)
				{
					m_clLowerGripperBuffer.InitBufferInfo();
				}
			}

			BL_DEBUGBOX("gripper jam at LOAD abort\n");
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
        }
		else	//v3.78
		{
			BL_DEBUGBOX("gripper jam at LOAD continue");
			X_Home();	//v3.95T1

			if (bBinFull == TRUE)
			{
				if (bBufferLevel == BL_BUFFER_LOWER)
				{
					UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_LOWER);
					m_clLowerGripperBuffer.InitBufferInfo();
				}
				else if (bBufferLevel == BL_BUFFER_UPPER)
				{
					UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_UPPER);
					m_clUpperGripperBuffer.InitBufferInfo();
				}
			}
		}
    }


	//Immediately down the buffer table
	if (bBufferLevel == BL_BUFFER_LOWER)
	{
		BL_DEBUGBOX("down buffer");
		SetBufferLevel(FALSE);
		Sleep(100);
	}

    //Sleep(200);
    //SetGripperState(FALSE);	//andrew123456
	if (bEnableBtT && !IsMS90())	//v4.39T7	//v4.50A25
	{
		BL_DEBUGBOX("Gripper OFF at LOAD pos");
		SetGripperState(FALSE);		
		Sleep(100);
	}


	if ( (m_nExArmReAlignBinConvID != 0) && (bCheckRealignRpy) )
		bRealignBin = RealignBinFrameRpy(TRUE);	

    if ( CheckElevatorCover() == FALSE )
    {
		if (bUnloadJam)		//PLLM v3.74T35
		{
			BL_DEBUGBOX("gripper HOME because of JAM");
			X_Home();
			Sleep(100);
		}

		BL_DEBUGBOX("Check Elevator fail");
		HouseKeeping(m_lReadyPos_X, FALSE);
		
		if (bBufferLevel == BL_BUFFER_LOWER)
		{
			UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_LOWER);
			m_clLowerGripperBuffer.InitBufferInfo();
		}
		else if (bBufferLevel == BL_BUFFER_UPPER)
		{
			UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_UPPER);
			m_clUpperGripperBuffer.InitBufferInfo();
		}
		
		BL_DEBUGBOX("Check elevator cover stop 2\n");
		return FALSE;
    }


	//v3.95T1	//Re-position before MoveElevatorToLoad() below
	if (bUnloadJam)		//v3.77
	{
		BL_DEBUGBOX("gripper HOME because of JAM");
		X_Home();
		Sleep(200);
	}


	if (!bIsMgzPreMove)
	{
		//xyz	//Disable OutOfMgzn checking as gripper may block the sensor at this point
		// Move back to original unload position
		if ( MoveElevatorToUnload(lMgzn, lSlot, FALSE, TRUE, TRUE) == FALSE ) //Must wait Z motion complete, otherwise gripper will crashes with elevator
		{
			HouseKeeping(m_lReadyPos_X, FALSE);
			if (bBufferLevel == BL_BUFFER_LOWER)
			{
				UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_LOWER);
				m_clLowerGripperBuffer.InitBufferInfo();
			}
			else if (bBufferLevel == BL_BUFFER_UPPER)
			{
				UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_UPPER);
				m_clUpperGripperBuffer.InitBufferInfo();
			}
			return FALSE;
		}
	}


	BL_DEBUGBOX("gripper move to ready");
	BOOL bGripperStatus = TRUE;
	if (bXStopAtHome)	//v3.70T4
	{
		bGripperStatus = BinGripperMoveTo_Auto(-500, SFM_NOWAIT);	//v3.94T5	//gripper stop at HOME to safe next LOAD time in dual-buffer sequence
		m_bIsExArmGripperAtUpPosn = TRUE;		//temporary use DLA var to memorize it
	}
	else
	{
		bGripperStatus = BinGripperMoveTo_Auto(m_lReadyPos_X, SFM_NOWAIT);
		m_bIsExArmGripperAtUpPosn = FALSE;		//temporary use DLA var to memorize it
	}

	//andrew123456
	if ( (m_nExArmReAlignBinConvID != 0) && (bCheckRealignRpy) )
		bRealignBin = RealignBinFrameRpy();	

	BinGripperSynMove_Auto();

	if ( bGripperStatus == FALSE )
	{
		HouseKeeping(m_lReadyPos_X, FALSE);
		if (bBufferLevel == BL_BUFFER_LOWER)
		{
			UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_LOWER);
			m_clLowerGripperBuffer.InitBufferInfo();
		}
		else if (bBufferLevel == BL_BUFFER_UPPER)
		{
			UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_UPPER);
			m_clLowerGripperBuffer.InitBufferInfo();
		}
		return FALSE;
	}

	if (bBufferLevel == BL_BUFFER_LOWER)
	{		
		if (UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_LOWER) == FALSE)
		{
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}

		m_clLowerGripperBuffer.InitBufferInfo();

		BL_DEBUGBOX("check down buffer");
		//SetBufferLevel(FALSE);
		//Sleep(200);

		if ( IsDualBufferLevelDown() == FALSE)
		{
			SetBufferLevel(FALSE);
			Sleep(300);

			//v3.71T1	//Insert 1sec timeout
			INT nCount = 0;
			while (!IsDualBufferLevelDown())
			{
				Sleep(50);
				nCount++;
				if (nCount > 20)
					break;
			}

			if ( IsDualBufferLevelDown() == FALSE)
			{
				Sleep(100);
				if ( IsDualBufferLevelDown() == FALSE)
				{
					SetErrorMessage("BL Buffer is not down 7");
					SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_DOWN);
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
			}
		}
	}
	else if (bBufferLevel == BL_BUFFER_UPPER)
	{
		if (UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, bBinFull, bClearBin, lFullStatus, bIsEmptyFrame, bBurnInUnload, BL_BUFFER_UPPER) == FALSE)
		{
			HouseKeeping(m_lReadyPos_X, FALSE);
			return FALSE;
		}

		m_clUpperGripperBuffer.InitBufferInfo();
	}

 	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

 	TRY {
		SaveMgznRTData();		//v4.06
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	//Check REALIGN result	//v2.72a5
	if (bCheckRealignRpy)
	{
		if (!bRealignBin)
		{
			BL_DEBUGBOX("Re-align bin frame - FAIL");				//v3.98T3
			SetErrorMessage("BL: Realign bin frame fails 2!");
			return FALSE;
		}
		else if (m_nExArmReAlignBinConvID != 0)
		{
			BL_DEBUGBOX("Re-align bin frame - ID not 0!?");		//v3.98T3
		}
	}

	szText1.Format("#2 U.Buffer = %d BC %s, L.Buffer = %d BC %s, Current = %d BC %s",
		m_clUpperGripperBuffer.GetBufferBlock(), m_clUpperGripperBuffer.GetBufferBarcode(),
		m_clLowerGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBarcode(),
		GetCurrBinOnBT(), m_szBinFrameBarcode);
	BL_DEBUGBOX(szText1);
    BL_DEBUGBOX("Unload from Buffer to Mgz finish\n");
	//CMSLogFileUtility::Instance()->BL_BackupLogStatus();		//v4.21T5

	//szBinBlkGrade.Format("%d", GetBinBlkGrade(lBinNo));
	szBinBlkGrade.Format("%d", lBinNo);	
	if (!m_bDisableBinFrameStatusSummaryFile)		//v4.53A5
	{
		CMSLogFileUtility::Instance()->BL_BinFrameStatusSummaryFile("Unload Frame", GetMagazineName(lMgzn).MakeUpper(), 
			lSlot+1, m_szBCName, szBinBlkGrade, GetBinBlkBondedCount(lSlot + 1), m_bUseBarcode);
	}

	//andrew1234
	if (!bContinueAort)		//PLLM REBEL only
		return FALSE;

	return TRUE;
}


// buffer->table
LONG CBinLoader::DB_LoadFromBufferToTable(BOOL bBurnInLoad, BOOL bOffline, BOOL bBufferLevel, BOOL bExchangingFrame, BOOL bPreScanBarcode)
{
	BOOL bUseEmpty = FALSE;
	LONG lFrameOnBT = 0;
	BOOL bFrameExist = FALSE;
    CString str;
	CString szText1, szMsg;
	BOOL bUseBarcode = m_bUseBarcode;	//v4.39T7


	BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];
	if (bEnableBtT && !IsMS90())		//v4.39T7	//Nichia MS100+		//v4.50A25
	{
		//With Bt T the barcode scanning will be performed on table instead of gripper;
		//so here the barcode scanning on gripper is disabled
		bUseBarcode = FALSE;
	}

    BL_DEBUGBOX("Load from Buffer to Table start");

	if ( IsBondArmSafe() == FALSE)	//v3.70T4
	{
		if (IsFrameLevel() == FALSE)	//v3.86
			SetFrameLevel(FALSE);		// down bin table first if necessary
		return FALSE;
	}

	// check frame on bin table logically
    if ( (GetCurrBinOnBT() !=0) && (bBurnInLoad == FALSE) ) // logically
    {
		if( BL_YES_NO(HMB_BL_LOGICALBLK_NOT_ZERO, IDS_BL_LOAD_FRAME)==FALSE )
		{	
			SetErrorMessage("BL Current logical block is not zero");
			return FALSE;
		}
    }


	//v4.46T11	//For Semitek NewAlignMethod frame angle alignment in BT and BPR
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Enable"]	= FALSE;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Enable"]	= FALSE;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["MiddleDie"]["Enable"]	= FALSE;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["X"]		= 0;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["FirstDie"]["Y"]		= 0;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["X"]			= 0;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["LastDie"]["Y"]			= 0;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["X"]			= 0;
	(*m_psmfSRam)["BinTable"]["AlignFrame"]["MidDie"]["Y"]			= 0;


	// check frame on bin table physically
    lFrameOnBT = CheckFrameOnBinTable();

	if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
    {
		lFrameOnBT = BL_FRAME_NOT_EXIST;
    }
	
	if( lFrameOnBT != BL_FRAME_NOT_EXIST )
    {
        //if( BL_OK_CANCEL("Frame exists in bintable!\nPlease remove the frame", "Load Frame Message")==FALSE )
		if ( BL_OK_CANCEL(HMB_BL_REMOVE_BT_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
        {
			SetErrorMessage("BL frame exist on table");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_ON_TABLE);
			return FALSE;
        }
		else
		{
			if ( CheckFrameOnBinTable() != BL_FRAME_NOT_EXIST)	//check again
			{
				SetErrorMessage("BL frame exist on table when checking again");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_ON_TABLE);
				return FALSE;
			}
		}
    }

	BL_DEBUGBOX("gripper move to ready");
	if ( BinGripperMoveTo_Auto(m_lReadyPos_X, SFM_NOWAIT) == FALSE )
	{
		HouseKeeping(m_lReadyPos_X);
		return FALSE;
	}

	if (bBufferLevel == BL_BUFFER_UPPER)
	{
		// check frame on Upper Buffer table logically
		if (!m_clUpperGripperBuffer.IsBufferBlockExist())
		{
			SetErrorMessage("BL Current logical block is zero");
			SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_FRAME);
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}

		// check frame on Upper Buffer table physically
		bFrameExist = IsDualBufferUpperExist();
	    
		if( (bBurnInLoad==TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			SetErrorMessage("BL No Frame is detected in Upper Buffer slot");
			if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
			{
				SetErrorMessage("BL No Frame is detected in Upper Buffer slot -> abort");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_U_BUFFER);
				HouseKeeping(m_lReadyPos_X);
				return FALSE;
			}
		}

		//update the magazine and slot to match with frame to be loaded to table
		if ( (GetLoadMgzSlot(bUseEmpty, m_clUpperGripperBuffer.GetBufferBlock()) == FALSE) && (bBurnInLoad == FALSE) )
		{
			CString szMsg;
			szMsg.Format("\n%d",m_clUpperGripperBuffer.GetBufferBlock());
			SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
			SetErrorMessage("BL No slot is assigned to this physical block");
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}

		//down buffer table if ncessary
		if ( IsDualBufferLevelDown() == FALSE)
		{
			BL_DEBUGBOX("down buffer");
			SetBufferLevel(FALSE);
			Sleep(300);
			
			//v3.71T1	//Insert 1sec timeout
			INT nCount = 0;
			while (!IsDualBufferLevelDown())
			{
				Sleep(50);
				nCount++;
				if (nCount > 20)
					break;
			}

			if ( IsDualBufferLevelDown() == FALSE)
			{
				Sleep(100);
				if ( IsDualBufferLevelDown() == FALSE)
				{
					SetErrorMessage("BL Buffer Table is not down 8");
					SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_DOWN);
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
			}
		}
	}
	else if (bBufferLevel == BL_BUFFER_LOWER)
	{
		// check frame on Lower Buffer table logically
		if (!m_clLowerGripperBuffer.IsBufferBlockExist())
		{
			SetErrorMessage("BL Current logical block is zero");
			SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_FRAME);
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}

		// check frame on Lower Buffer table physically
		bFrameExist = IsDualBufferLowerExist();
	    
		if( (bBurnInLoad==TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			Sleep(200);

			if (IsDualBufferLowerExist() == FALSE)
			{
				if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
				{
					SetErrorMessage("BL No Frame is detected in Lower Buffer");
					SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_L_BUFFER);
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
			}
		}

		//update the magazine and slot to match with frame to be loaded to table
		if ( (GetLoadMgzSlot(bUseEmpty, m_clLowerGripperBuffer.GetBufferBlock()) == FALSE) && (bBurnInLoad == FALSE) )
		{
			CString szMsg;
			szMsg.Format("\n%d", m_clLowerGripperBuffer.GetBufferBlock());
			SetAlert_Msg_Red_Yellow(IDS_BL_NOSLOTASSIGN_THIS_PHYBLK, szMsg);		
			SetErrorMessage("BL No slot is assigned to this physical block");
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}

		if ( BinGripperSynMove_Auto() == FALSE )	//sync gripper before up buffer
		{
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}

		//up buffer table
		BL_DEBUGBOX("Move low buffer Up");
		SetBufferLevel(TRUE);
		Sleep(300);

		if ( IsDualBufferLevelDown() == TRUE)
		{
			SetBufferLevel(TRUE);
			Sleep(300);

			if ( m_bNoSensorCheck || m_bDisableBL )		//v3.94
			{
			}
			else
			{
				//v3.71T1	//Insert 1sec timeout
				INT nCount = 0;
				while (IsDualBufferLevelDown())
				{
					Sleep(50);
					nCount++;
					if (nCount > 20)
						break;
				}

				if ( IsDualBufferLevelDown() == TRUE)
				{
					Sleep(100);
					if ( IsDualBufferLevelDown() == TRUE)
					{
						SetErrorMessage("BL Buffer Table is not up 9");
						SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_UP);
						HouseKeeping(m_lReadyPos_X);
						return FALSE;
					}
				}
			}
		}
	}

    if ( CheckElevatorCover() == FALSE )
    {
		HouseKeeping(m_lReadyPos_X);
		return FALSE;
    }
	
	if ( MoveWaferTableToSafePosn(TRUE, bOffline, FALSE) == FALSE )		//v3.61	DDD		//v4.53A21
		return FALSE;

	if (bExchangingFrame == FALSE)	//Offline mode, need to move table
	{
		BL_DEBUGBOX("BT move to unload");
		if (!IsBTFramePlatformDown())
		{
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);		
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}

		if ( (m_lBTLoadOffset_Y != 0) && (labs(m_lBTLoadOffset_Y) < 1000) )		//v3.80
		{
			if( MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y + m_lBTLoadOffset_Y, 1) == FALSE )
			{
				HouseKeeping(m_lReadyPos_X);
				return FALSE;
			}
		}
		else
		{
			if( MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y) == FALSE )
			{
				HouseKeeping(m_lReadyPos_X);
				return FALSE;
			}
		}
	}
	else
	{
		//Move again anyway in AUTOBOND due to LOAD OFFSET difference in EXCHANGE
		if ( (m_lBTLoadOffset_Y != 0) && (labs(m_lBTLoadOffset_Y) < 1000) )		//v3.80
		{
			if( MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y + m_lBTLoadOffset_Y, 1) == FALSE )
			{
				HouseKeeping(m_lReadyPos_X);
				return FALSE;
			}
		}
	}


    if( BinGripperSynMove_Auto()==FALSE )
	{
		HouseKeeping(m_lReadyPos_X);
        return FALSE;
	}

	if (bExchangingFrame == FALSE)
	{
		BL_DEBUGBOX("BT SettleDown");
		SettleDownBeforeUnload();
	}

	SetFrameAlign(FALSE);

	BL_DEBUGBOX("Searching frame");
	LONG lOffsetX = m_lUnloadOffsetX;
	LONG lReturn = TRUE;
	if (lOffsetX > 0)
		lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, lOffsetX+100, 160);
	else
	{
		BinGripperMoveTo(m_lBufferUnloadPos_X - 100);	//-50);	//200);		//andrew123456	//v3.87 //additional 50 steps added when preload
		if (m_bNoSensorCheck || bBurnInLoad)
			lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, 200, 50);
		else
			lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, abs(m_lBufferUnloadPos_X-m_lReadyPos_X)+100, 50);
	}

	if (!lReturn)
	{
		if ((bBurnInLoad==FALSE) && 
			(!m_bDisableBL) &&	
			(BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE))
		{
			SetErrorMessage("No Frame is Detected in gripper");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}
	}

	// Updated to open the bin align after complete search frame
    SetGripperState(TRUE);
	SetFrameAlign(FALSE);
	Sleep(100);

	BL_DEBUGBOX("gripper move to unload");
	if (BinGripperMoveTo_Auto(m_lUnloadPos_X, SFM_NOWAIT) == FALSE)	//andrew7
	{
		HouseKeeping(m_lPreUnloadPos_X, TRUE, TRUE);
		return FALSE;
	}

	
	if (bExchangingFrame)
	{
		if (bUseBarcode)
		{
			if (GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC)
			{
				m_szBCName = "";
				m_szDBPreScanBCName = "";
			}
			else if (GetBCModel() == BL_BAR_REGAL)
			{
				m_szBCName = "";
				m_szDBPreScanBCName = "";
			}
		}
	}

	BinGripperSynMove_Auto();	


	//v3.76 
	//Re-enable for in-clamp checking
//	if (!IsFrameInClamp())
//	{
//		Sleep(100);
//		if (!IsFrameInClamp())
//		{
//			SetGripperState(FALSE);
//			Sleep(200);
//			lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, 500, 50);
//
//			if (!lReturn)
//			{
//				SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);		
//				SetGripperState(FALSE);
//				Sleep(100);
//				HouseKeeping(m_lPreUnloadPos_X);
//				return FALSE;
//			}
//
//			SetGripperState(TRUE);
//			Sleep(200);
//			BinGripperMoveTo(m_lUnloadPos_X);
//
//			if (!IsFrameInClamp())
//			{
//				SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);		
//				SetGripperState(FALSE);
//				Sleep(100);
//				HouseKeeping(m_lPreUnloadPos_X);
//				return FALSE;
//			}
//		}
//	}

	SetGripperState(FALSE);
    Sleep(200);

	if (IsMS90() && m_bEnableMS90TablePusher)
	{
		BL_DEBUGBOX("Aligning frame");
		if (!AlignBinFrameOnTable_MS90())	//v4.59A40	//Finisar TX MS90 New Method
		{
			CString szMsg = "BL Load Frame: BT Pusher XZ not at SAFE position";
			SetErrorMessage(szMsg);
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);	
			SetGripperState(FALSE);
			Sleep(500);
			if (X_IsPowerOn())
				BinGripperMoveTo(m_lPreUnloadPos_X);
			Sleep(500);
			HouseKeeping(m_lPreUnloadPos_X);

			if (bBufferLevel == BL_BUFFER_UPPER)
			{
				m_lBTCurrentBlock = m_clUpperGripperBuffer.GetBufferBlock();
				m_clUpperGripperBuffer.SetBufferBlock(0);

				CString szMsg;
				szMsg.Format("false updated to upper block %d", GetCurrBinOnBT());
				BL_DEBUGBOX(szMsg);
				m_szBinFrameBarcode = m_clUpperGripperBuffer.GetBufferBarcode();
				m_clUpperGripperBuffer.SetBufferBarcode("");
			}
			else if (bBufferLevel == BL_BUFFER_LOWER)
			{
				m_lBTCurrentBlock = m_clLowerGripperBuffer.GetBufferBlock();;
				m_clLowerGripperBuffer.SetBufferBlock(0);

				CString szMsg;
				szMsg.Format("false updated to lower block %d", GetCurrBinOnBT());
				BL_DEBUGBOX(szMsg);
				m_szBinFrameBarcode = m_clLowerGripperBuffer.GetBufferBarcode();
				m_clLowerGripperBuffer.SetBufferBarcode("");
			}

			SaveData();
			return FALSE;
		}

		//Re-enable for in-clamp checking
		if (!IsFrameInClamp())
		{
			Sleep(100);
			if (!IsFrameInClamp())
			{
				SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);		
				SetGripperState(FALSE);
				Sleep(100);
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
			}
		}
	}

	BL_DEBUGBOX("gripper move to preunload");
    if ( BinGripperMoveTo_Auto(m_lPreUnloadPos_X, SFM_NOWAIT) == FALSE )
	{
		X_Sync();
		HouseKeeping(m_lPreUnloadPos_X);
		return FALSE;
	}
	
	BinGripperSynMove_Auto();

	//v4.22T5	//Requested by SanAn (Wan Yi Ming)
	if (!IsMS90())	//v4.50A23	.//MS90new sensor design cannot check this sensor at UPlevel
	{
		if (!bBurnInLoad && !m_bNoSensorCheck && !IsFrameOnBTPosition())		//If NO frame detected on table by lower-right BT FRAME-POS sensor
		{
			CString szMsg;
			szMsg.LoadString(HMB_BL_NO_FRAME_ON_OUT_TABLE);
			SetErrorMessage(szMsg);
			BL_DEBUGBOX(szMsg);
			SetAlert_Red_Back(HMB_BL_NO_FRAME_ON_OUT_TABLE);
			HouseKeeping(m_lPreUnloadPos_X, FALSE);
			return FALSE;
		}
	}

	BL_DEBUGBOX("Aligning frame");
	if (IsMS90() && m_bEnableMS90TablePusher)
	{
		//AlignBinFrameOnTable_MS90();	//v4.59A40	//Finisar TX MS90 New Method
		SetFrameLevel(FALSE);	
	}
	else if (IsMS60())					//v4.50A30		//LeoLam
	{
		AlignBinFrame();
		SetFrameLevel(FALSE);	
	}
	else
	{
		SetFrameLevel(FALSE);	
		AlignBinFrame();		//v3.76	
	}

	if (IsFrameLevel() == FALSE)
	{
		Sleep(50);

		INT nCount = 0;
		while (IsFrameLevel() == FALSE)
		{
			Sleep(50);
			nCount++;
			if (nCount > 50)
				break;
		}

		if (IsFrameLevel() == FALSE)
		{
			HmiMessage_Red_Yellow("ERROR: please check frame level on output table!");
			SetErrorMessage("check frame level error");
			BL_DEBUGBOX("check frame level error");
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}
	
	//v4.51A1
	//AlignBinFrame();
	if (IsMS60() && m_bUseBinTableVacuum && (m_lBTVacuumDelay > 0))	
	{
		Sleep(m_lBTVacuumDelay);	//Used AFTER Vac is applied, before BT motion
	}


	//extra delay for platform down
	if (m_bNoSensorCheck)
		Sleep(350);
	else
		Sleep(50);

	BL_DEBUGBOX("check frame on bin table");
    lFrameOnBT = CheckFrameOnBinTable();
	if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
	{
		lFrameOnBT = BL_FRAME_ON_CENTER;
	}
	
	//v3.57T3	//Added vac-meter sensor checking
	BOOL bCheckFrameExistByVacFail = FALSE;
	if (!CheckFrameExistByVacuum())
	{
		lFrameOnBT = BL_FRAME_NOT_EXIST;
		if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) )
		{
			lFrameOnBT = BL_FRAME_ON_CENTER;
		}
		else
		{
			bCheckFrameExistByVacFail = TRUE;
			BL_DEBUGBOX("check frame on table with vac-meter fail");
		}
	}

	switch( lFrameOnBT )
	{
		case BL_FRAME_NOT_EXIST:
			if (!bCheckFrameExistByVacFail)		//v3.59
				SetFrameVacuum(FALSE);
		    SetFrameAlign(FALSE);
			if( BL_YES_NO(HMB_BL_BT_NO_FRAME, IDS_BL_LOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
            {
				if (bCheckFrameExistByVacFail)
					SetFrameVacuum(FALSE);
				HouseKeeping(m_lPreUnloadPos_X);
                return FALSE;
            }

		    SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);
			SetFrameVacuum(TRUE);
			Sleep(m_lBTVacuumDelay);

			//Check sensor again in case of problem		//v3.01T4
			lFrameOnBT = CheckFrameOnBinTable();
			if (lFrameOnBT != BL_FRAME_ON_CENTER)
			{
				SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
				SetErrorMessage("No Frame exists on Bin Table");
				HouseKeeping(m_lPreUnloadPos_X);
                return FALSE;
			}
            break;

        case BL_FRAME_ON_RHS_EDGE: 
            SetFrameVacuum(FALSE);
		    SetFrameAlign(FALSE);
            //if( BL_OK_CANCEL("Frame exists on bintable right edge only!\nPlease push it back", "Load Frame Message")==FALSE )
			if( BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_RHS, IDS_BL_LOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
            {
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
            }

		    SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);
			SetFrameVacuum(TRUE);
			Sleep(m_lBTVacuumDelay);

 			//Check sensor again in case of problem		//v3.01T4
			lFrameOnBT = CheckFrameOnBinTable();
			if (lFrameOnBT != BL_FRAME_ON_CENTER)
			{
				SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
				SetErrorMessage("No Frame exists on Bin Table");
				HouseKeeping(m_lPreUnloadPos_X);
                return FALSE;
			}
			break;

		case BL_FRAME_ON_LHS_EDGE: 
            SetFrameVacuum(FALSE);
		    SetFrameAlign(FALSE);
            //if( BL_OK_CANCEL("Frame exists on bintable left edge only!\nPlease push it back", "Load Frame Message")==FALSE )
            if( BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_LHS, IDS_BL_LOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
            {
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
            }

		    SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);
			SetFrameVacuum(TRUE);
			Sleep(m_lBTVacuumDelay);

 			//Check sensor again in case of problem		//v3.01T4
			lFrameOnBT = CheckFrameOnBinTable();
			if (lFrameOnBT != BL_FRAME_ON_CENTER)
			{
				SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
				SetErrorMessage("No Frame exists on Bin Table");
				HouseKeeping(m_lPreUnloadPos_X);
                return FALSE;
			}
            break;

        case BL_FRAME_ON_CENTER:
            break;
	}

	BL_DEBUGBOX("gripper move to ready");
	//if ( BinGripperMoveTo(m_lReadyPos_X) == FALSE )
	//andrew123456
	LONG lMoveStatus = TRUE;
	if (bExchangingFrame)
		lMoveStatus = BinGripperMoveTo_Auto(m_lBufferUnloadPos_X, SFM_NOWAIT);
	else
		lMoveStatus = BinGripperMoveTo_Auto(m_lReadyPos_X, SFM_NOWAIT);
	if ( lMoveStatus == FALSE )
	{
		HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		if (bBufferLevel == BL_BUFFER_UPPER)
		{
			m_lBTCurrentBlock = m_clUpperGripperBuffer.GetBufferBlock();
			m_clUpperGripperBuffer.SetBufferBlock(0);

			CString szMsg;
			szMsg.Format("false updated to upper block %d", GetCurrBinOnBT());
			BL_DEBUGBOX(szMsg);
			m_szBinFrameBarcode = m_clUpperGripperBuffer.GetBufferBarcode();
			m_clUpperGripperBuffer.SetBufferBarcode("");
		}
		else if (bBufferLevel == BL_BUFFER_LOWER)
		{
			m_lBTCurrentBlock = m_clLowerGripperBuffer.GetBufferBlock();;
			m_clLowerGripperBuffer.SetBufferBlock(0);

			CString szMsg;
			szMsg.Format("false updated to lower block %d", GetCurrBinOnBT());
			BL_DEBUGBOX(szMsg);
			m_szBinFrameBarcode = m_clLowerGripperBuffer.GetBufferBarcode();
			m_clLowerGripperBuffer.SetBufferBarcode("");
		}
		return FALSE;
	}

	if (bBufferLevel == BL_BUFFER_UPPER)
	{
		m_lBTCurrentBlock = m_clUpperGripperBuffer.GetBufferBlock();
		m_clUpperGripperBuffer.SetBufferBlock(0);

		CString szMsg;
		szMsg.Format("updated to upper block %d", GetCurrBinOnBT());
		BL_DEBUGBOX(szMsg);

		m_szBinFrameBarcode = m_clUpperGripperBuffer.GetBufferBarcode();
		m_clUpperGripperBuffer.SetBufferBarcode("");
	}
	else if (bBufferLevel == BL_BUFFER_LOWER)
	{
		m_lBTCurrentBlock = m_clLowerGripperBuffer.GetBufferBlock();
		m_clLowerGripperBuffer.SetBufferBlock(0);

		CString szMsg;
		szMsg.Format("updated to lower block %d", GetCurrBinOnBT());
		BL_DEBUGBOX(szMsg);

		m_szBinFrameBarcode = m_clLowerGripperBuffer.GetBufferBarcode();
		m_clLowerGripperBuffer.SetBufferBarcode("");

		//up buffer table
		BL_DEBUGBOX("down buffer");
		SetBufferLevel(FALSE);
		
		
//		Sleep(100);
//
//		//v3.71T1	//Insert 1sec timeout
//		INT nCount = 0;
//		while (!IsDualBufferLevelDown())
//		{
//			Sleep(50);
//			nCount++;
//			if (nCount > 20)
//				break;
//		}
//
//		if ( IsDualBufferLevelDown() == FALSE)
//		{
//			Sleep(100);
//			if ( IsDualBufferLevelDown() == FALSE)
//			{
//				SetErrorMessage("BL Buffer Table is not down 13");
//				SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_DOWN);
//				HouseKeeping(m_lReadyPos_X);
//				return FALSE;
//			}
//		}

	}
	
	UpdateExArmMgzSlotNum(GetCurrBinOnBT());	//v3.94		//Update slot number on menu
	ResetFrameIsAligned(GetCurrBinOnBT());		//v4.42T10	//SanAn

	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

 	TRY {
		SaveMgznRTData();		//v4.06
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	if ( bOffline == TRUE )
	{
		BL_DEBUGBOX("BT to Home");

		(*m_psmfSRam)["BinLoaderStn"]["BT1"]["RealignFrame"]	= FALSE;		//v4.43T1

		if (!IsBTFramePlatformDown())
		{
			SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);		
			HouseKeeping(m_lReadyPos_X);
			return FALSE;
		}
		if ( MoveBinTable(0, 0) == FALSE )
			return FALSE;
		if ( MoveWaferTableToSafePosn(FALSE, bOffline) == FALSE )		//v3.61	DDD
			return FALSE;
	}
	else
	{
		//For BT T barcode scanning	//v4.40T5	//Nichia MS100+
		(*m_psmfSRam)["BinLoader"]["CurrMgzn"]		= m_lCurrMgzn;
		(*m_psmfSRam)["BinLoader"]["CurrSlot"]		= m_lCurrSlot;
		(*m_psmfSRam)["BinLoader"]["BCScanRange"]	= m_lScanRange;
		(*m_psmfSRam)["BinLoader"]["BCRetryLimit"]	= m_lTryLimits;

		//Move to Block 1 each time
		IPC_CServiceMessage svMsg;
		int nConvID = 0;
		ULONG ulBlkNo = (ULONG)GetCurrBinOnBT();

		svMsg.InitMessage(sizeof(ULONG), &ulBlkNo);
		if (m_bRealignBinFrame)
		{
			//BL_DEBUGBOX("BT to 1st die for alignment");
			BL_DEBUGBOX("BT to last die (Realign)");

			//BT Realign Frame Option: 0=DEFAULT(3-pt); 1=1-pt; 2=2-pt
			LONG lRealignFrameOption = (LONG)(*m_psmfSRam)["BinTable"]["RealignFrameOption"];	//v3.71T5

			if (lRealignFrameOption == 1)	//1-pt realignment
			{
				nConvID = m_comClient.SendRequest("BinTableStn", "MoveToBinBlk", svMsg);		//Move to last-die
			}
			else							//default = 3-pt realignment
			{
				nConvID = m_comClient.SendRequest("BinTableStn", "MoveToBinBlk1stDie", svMsg);	//MOVE to UL die
			}
		}
		else
		{
			BL_DEBUGBOX("BT to last die");
			nConvID = m_comClient.SendRequest("BinTableStn", "MoveToBinBlk", svMsg);			//Move directly to last-die
		}
		
		BOOL bMoveStatus = TRUE;	//v4.40T7
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				svMsg.GetMsg(sizeof(BOOL), &bMoveStatus);	//v4.40T7
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if (!bMoveStatus)
			return FALSE;
		if ( MoveWaferTableToSafePosn(FALSE, bOffline, FALSE) == FALSE )	//v3.61	DDD		//v4.53A21
			return FALSE;

		//v3.76
		//Re-check frame on table after BT MOVE
		BL_DEBUGBOX("check frame on bin table 2 **DualBuffer**");
		lFrameOnBT = CheckFrameOnBinTable();
		if ( (bBurnInLoad == TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
		{
			lFrameOnBT = BL_FRAME_ON_CENTER;
		}
		if (lFrameOnBT != BL_FRAME_ON_CENTER)
		{
			Sleep(50);
			lFrameOnBT = CheckFrameOnBinTable();
			if (lFrameOnBT != BL_FRAME_ON_CENTER)
			{
				SetAlert_Red_Yellow(IDS_BL_NOFRAME_EXIST);
				SetErrorMessage("No Frame exists on Bin Table 2");
				return FALSE;
			}
		}


		//Let table stable for some time
		//Sleep(100);	//andrew123456

		if( bUseEmpty==FALSE )		//v3.94		//Allow RealignBinframe in BURNIN mode
		{
			BL_DEBUGBOX("Re-align bin frame start");

			BOOL bRtn;
			int nConvID = 0;
			BOOL bTemp;
			IPC_CServiceMessage stMsg;

			bTemp = FALSE;
			stMsg.InitMessage(sizeof(BOOL), &bTemp);
			nConvID = m_comClient.SendRequest("BinTableStn", "SetAlignBinMode", stMsg);
			m_comClient.ScanReplyForConvID(nConvID, 30000);
			m_comClient.ReadReplyForConvID(nConvID, stMsg);

			X_Sync();	//must sync first! Otherwise bond arm may crash with gripper!

			bRtn = RealignBinFrameReq(GetCurrBinOnBT(), bUseEmpty);
			if ( bRtn == TRUE )
			{
				BL_DEBUGBOX("Re-align bin frame - sent");
			}
			else
			{
				BL_DEBUGBOX("Re-align bin frame - failed");
			}

			szText1.Format("#3 U.Buffer = %d BC %s, L.Buffer = %d BC %s, Current = %d BC %s",
				m_clUpperGripperBuffer.GetBufferBlock(), m_clUpperGripperBuffer.GetBufferBarcode(),
				m_clLowerGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBarcode(),
				GetCurrBinOnBT(), m_szBinFrameBarcode);
			BL_DEBUGBOX(szText1);
			BL_DEBUGBOX("Load from Buffer to Table finish\n");

			return bRtn;
		}
		else
		{
			//Nichia//v4.43T7
			BL_DEBUGBOX("Re-align EMPTY frame start");

			BOOL bRtn;
			int nConvID = 0;
			BOOL bTemp;
			IPC_CServiceMessage stMsg;

			bTemp = FALSE;
			stMsg.InitMessage(sizeof(BOOL), &bTemp);
			nConvID = m_comClient.SendRequest("BinTableStn", "SetAlignBinMode", stMsg);
			m_comClient.ScanReplyForConvID(nConvID, 30000);
			m_comClient.ReadReplyForConvID(nConvID, stMsg);

			X_Sync();	//must sync first! Otherwise bond arm may crash with gripper!

			bRtn = RealignBinFrameReq(GetCurrBinOnBT(), TRUE);
			if ( bRtn == TRUE )
			{
				BL_DEBUGBOX("Re-align EMPTY frame - sent");
				bRtn = RealignBinFrameRpy(FALSE, TRUE);			//v4.43T10
				if (bRtn)
					BL_DEBUGBOX("Re-align EMPTY frame - Done");
				else
					BL_DEBUGBOX("Re-align EMPTY frame - fail");
			}
			else
			{
				BL_DEBUGBOX("Re-align EMPTY frame - failed");
			}

			szText1.Format("#4 U.Buffer = %d BC %s, L.Buffer = %d BC %s, Current = %d BC %s",
				m_clUpperGripperBuffer.GetBufferBlock(), m_clUpperGripperBuffer.GetBufferBarcode(),
				m_clLowerGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBarcode(),
				GetCurrBinOnBT(), m_szBinFrameBarcode);
			BL_DEBUGBOX(szText1);
			BL_DEBUGBOX("Load from Buffer to Table finish\n");
			return bRtn;

//			X_Sync();
//			SetBLReady(TRUE);		

		}
	}
	
	szText1.Format("#5 U.Buffer = %d BC %s, L.Buffer = %d BC %s, Current = %d BC %s",
		m_clUpperGripperBuffer.GetBufferBlock(), m_clUpperGripperBuffer.GetBufferBarcode(),
		m_clLowerGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBarcode(),
		GetCurrBinOnBT(), m_szBinFrameBarcode);
	BL_DEBUGBOX(szText1);
    BL_DEBUGBOX("Load from Buffer to Table finish\n");

	return TRUE;
}


// Table->Buffer
LONG CBinLoader::DB_UnloadFromTableToBuffer(BOOL bBurnInUnload, BOOL bOffline, BOOL bBufferLevel, BOOL bExchangingFrame)
{
	LONG lStatus = 0;
	LONG lFrameOnBT;
	BOOL bScanBarcode = TRUE;
    CString str;
	CString szText1, szMsg;
	BOOL bFrameExist = FALSE;
	BOOL bFrameProtect = FALSE;

    BL_DEBUGBOX("Unload from Table to Buffer start");

	if ( IsBondArmSafe() == FALSE)	//v3.70T4
	{
		if (IsFrameLevel() == FALSE)	//v3.86
			SetFrameLevel(FALSE);		// down bin table first if necessary
		return FALSE;
	}

    // check frame on bin table logically
    if( GetCurrBinOnBT()==0 )
    {
		SetAlert_Red_Yellow(IDS_BL_NOLOGICAL_FRAME);
		SetErrorMessage("BL Current logical block is zero");
		return FALSE;
    }

    // check frame on bin table physically
    lFrameOnBT = CheckFrameOnBinTable();
    
	if( (bBurnInUnload==TRUE) || (m_bNoSensorCheck == TRUE) || (m_bDisableBL) )
    {
		lFrameOnBT = BL_FRAME_ON_CENTER;
    }

	switch( lFrameOnBT )
	{
        case BL_FRAME_NOT_EXIST: 
			SetFrameVacuum(FALSE);
		    SetFrameAlign(FALSE);

			if ( bOffline == TRUE )
			{
				if( BL_YES_NO(HMB_BL_PUT_FRAME_INTO_BT, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
				{
					if( BL_YES_NO(HMB_BL_UNLOAD_MANUALLY, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
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
            //if( BL_OK_CANCEL("Frame exists on bintable right edge only!\nPlease push it back", "Unload Frame Message")==FALSE )
			if( BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_RHS, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
                return FALSE;

		    SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);

			SetFrameVacuum(TRUE);
            break;

		case BL_FRAME_ON_LHS_EDGE: 
			SetFrameVacuum(FALSE);
		    SetFrameAlign(FALSE);
            //if( BL_OK_CANCEL("Frame exists on bintable left edge only!\nPlease push it back", "Unload Frame Message")==FALSE )
			if( BL_OK_CANCEL(HMB_BL_FRAME_ON_BT_LHS, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE )
                return FALSE;

			SetFrameAlign(TRUE);
			Sleep(m_lBTAlignFrameDelay);

			SetFrameVacuum(TRUE);
            break;

        case BL_FRAME_ON_CENTER: 
            break;
    }

	//Also check in burn in cycle for safety purpose
	if (bBufferLevel == BL_BUFFER_LOWER)
	{
		if (IsDualBufferLowerExist())
		{
			SetErrorMessage("BL Frame exists in Lower Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_L_BUFFER);
			return FALSE;
		}

		BL_DEBUGBOX("Buffer Table Up");
		SetBufferLevel(TRUE);  //Up buffer table 
	}
	else
	{
		if (IsDualBufferUpperExist())
		{
			SetErrorMessage("BL Frame exists in Upper Buffer");
			SetAlert_Red_Yellow(IDS_BL_FRMAE_EXIST_IN_U_BUFFER);
			return FALSE;
		}

		BL_DEBUGBOX("Buffer Table Down");
		SetBufferLevel(FALSE);  //Down buffer table 
	}

	if (!IsBTFramePlatformDown())
	{
		SetAlert_Msg_Red_Yellow(IDS_BL_PLATFORM_NOT_DOWN, szMsg);		
		return FALSE;
	}


	//v3.70T4
	BOOL bAtPrePick = (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
	BOOL bAtSafe	= IsBHStopped();

	if ( (State() != IDLE_Q) && (bAtPrePick == 0) && (bAtSafe == 0))
	{
		Sleep(1000);
		bAtPrePick	= (BOOL)(LONG)(*m_psmfSRam)["BondHead"]["AtPrePick"];
		BOOL bAtSafe	= IsBHStopped();
		if ((bAtPrePick == 0) && (bAtSafe == 0))
		{
//AfxMessageBox("Dual-Buffer: BH not yet at PREPICK!", MB_SYSTEMMODAL);
		}
	}


	BL_DEBUGBOX("gripper move to preload");
	if ( BinGripperMoveTo_Auto(m_lPreUnloadPos_X, SFM_NOWAIT) == FALSE )
	{
		return FALSE;
	}

	BL_DEBUGBOX("BT DUAL BUFFER move to unload");
	WftToSafeLog("DUAL BUFFER BT move to unload, move wft");
	if ( MoveWaferTableToSafePosn(TRUE, bOffline, FALSE) == FALSE )		//v3.61		//v4.53A21
		return FALSE;
	if( MoveBinTable(m_lBTUnloadPos_X, m_lBTUnloadPos_Y, TRUE) == FALSE )	//v4.59A16
	{
		return FALSE;
	}

    if( BinGripperSynMove_Auto()==FALSE )
        return FALSE;

	BL_DEBUGBOX("SettleDown for unload");
	SettleDownBeforeUnload();

	if (bBufferLevel == BL_BUFFER_LOWER)
	{
		BL_DEBUGBOX("check buffer level");
		if ( IsDualBufferLevelDown() == TRUE )	//if cannot up
		{
			SetBufferLevel(TRUE);
			Sleep(300);

			if ( m_bNoSensorCheck || m_bDisableBL )		//v3.94
			{
			}
			else
			{
				//v3.71T1	//Insert 1sec timeout
				INT nCount = 0;
				while (IsDualBufferLevelDown())
				{
					Sleep(50);
					nCount++;
					if (nCount > 20)
						break;
				}

				if ( IsDualBufferLevelDown() == TRUE)
				{
					Sleep(100);
					if ( IsDualBufferLevelDown() == TRUE)
					{
						SetErrorMessage("BL Buffer table is not up 10");
						SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_UP);
						HouseKeeping(m_lPreUnloadPos_X);
						return FALSE;
					}
				}
			}
		}
	}
	else if (bBufferLevel == BL_BUFFER_UPPER)
	{
		BL_DEBUGBOX("check buffer level");
		if ( IsDualBufferLevelDown() == FALSE )	//if cannot up
		{
			SetBufferLevel(FALSE);
			Sleep(300);

			//v3.71T1	//Insert 1sec timeout
			INT nCount = 0;
			while (!IsDualBufferLevelDown())
			{
				Sleep(50);
				nCount++;
				if (nCount > 20)
					break;
			}

			if ( IsDualBufferLevelDown() == FALSE)
			{
				Sleep(100);
				if ( IsDualBufferLevelDown() == FALSE)
				{
					SetErrorMessage("BL Buffer table is not down 11");
					SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_DOWN);
					HouseKeeping(m_lPreUnloadPos_X);
					return FALSE;
				}
			}
		}
	}
	
	LONG lOffsetX = m_lUnloadOffsetX;
	if ( (lOffsetX > 0) && (m_lUnloadPos_X-lOffsetX  > m_lPreUnloadPos_X) )
	{
		BL_DEBUGBOX("gripper move to unload-offset");
		if ( BinGripperMoveTo((m_lUnloadPos_X-lOffsetX)) == FALSE )
		{
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
		}
	}

	SetFrameAlign(FALSE);	//xyz

	//if (!IsFrameInClamp() )
	//{
		BL_DEBUGBOX("Searching frame");
		LONG lReturn = TRUE;
		if (lOffsetX > 0)
			lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, lOffsetX+200, 160);
		else
		{
			BinGripperMoveTo(m_lUnloadPos_X);
			if (IsFrameInClamp() )
				lReturn = TRUE;
			else
				lReturn = BinGripperSearchInClamp(HP_POSITIVE_DIR, abs(m_lUnloadPos_X-m_lPreUnloadPos_X)+100, 160);
		}

		if (!lReturn)
		{
			if ((bBurnInUnload==FALSE) && 
				(!m_bDisableBL) )
			{
				if (BL_OK_CANCEL(HMB_BL_PUT_FRAME_INTO_GRIP, IDS_BL_UNLOAD_FRAME,CMS896AStn::m_bDisableOKSelectionInAlertMsg)==FALSE)
				{
					SetErrorMessage("BL No Frame is detected in gripper 3");	//v4.52A6
					SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);
					HouseKeeping(m_lPreUnloadPos_X, TRUE, FALSE, FALSE, TRUE);	//v4.52A4	//Fabrinet Thailand
					return FALSE;
				}
				else
				{
					if (!IsFrameInClamp() )
					{
						SetErrorMessage("BL No Frame is detected in gripper 4");	//v4.52A6
						SetAlert_Red_Yellow(IDS_BL_FRMAE_NOT_DETECTED_IN_GRIPPER);
						HouseKeeping(m_lPreUnloadPos_X, TRUE, FALSE, FALSE, TRUE);	//v4.52A4	//Fabrinet Thailand
						return FALSE;
					}
				}
			}
		}
	//}

//andrew1234
	// Updated to open the bin align after complete search frame
    Sleep(100);
    SetGripperState(TRUE);
	Sleep(100);

	//xyz

//	SetFrameAlign(FALSE);
//	if (m_lBTAlignFrameDelay > 200)
//		Sleep(m_lBTAlignFrameDelay);
//	else
//		Sleep(200);

	//andrew12345
	//Update buffer status as soon as frame starts to be pushed by gripper
	if (bBufferLevel == BL_BUFFER_LOWER)
	{
		m_clLowerGripperBuffer.SetBufferInfo(GetCurrBinOnBT(), m_szBinFrameBarcode);
	}
	else
	{
		m_clUpperGripperBuffer.SetBufferInfo(GetCurrBinOnBT(), m_szBinFrameBarcode);
	}
	m_lBTCurrentBlock = 0;
	m_szBinFrameBarcode = "";

	UpdateExArmMgzSlotNum(GetCurrBinOnBT());	//v3.94		//Update slot number on menu

	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

 	TRY {
		SaveMgznRTData();		//v4.06
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	BL_DEBUGBOX("gripper move to buffer unload");
	if ( BinGripperMoveSearchJam(m_lBufferUnloadPos_X) == FALSE )
    {
        SetGripperState(FALSE);

        if( bBurnInUnload==FALSE && 
			!m_bDisableBL && 
			BL_OK_CANCEL(HMB_BL_PUT_JAM_FRAME_TO_BUFFER, IDS_BL_UNLOAD_FRAME)==FALSE )
        {
			SetErrorMessage("BL gripper is jammed");
			HouseKeeping(m_lPreUnloadPos_X);
			return FALSE;
        }
		else
		{
			if (IsFrameJam() == TRUE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
			}
			else if (IsFrameOutOfMgz() == TRUE)
			{
				AbortElevatorMotionIfOutOfMag();		//v4.18T1
				SetErrorMessage("BL Frame is out of magazine");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
			}
		}
    }

	SetGripperState(FALSE);
    Sleep(150);

	BL_DEBUGBOX("gripper move to ready");
	if ( BinGripperMoveTo(m_lReadyPos_X) == FALSE )
	{
		HouseKeeping(m_lReadyPos_X, TRUE, TRUE);
		return FALSE;
	}

	if (bBufferLevel == BL_BUFFER_UPPER)
	{
		//Check upper Buffer has frame or not
		bFrameExist = IsDualBufferUpperExist();
		if (!bFrameExist)
		{
			Sleep(50);	//Sleep(200);	//v4.18T1
			bFrameExist = IsDualBufferUpperExist();
		}
		if ( (bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			HmiMessage_Red_Back("Upper buffer frame not detected ....");
			BL_DEBUGBOX("Upper buffer frame not detected ...");

			if (IsFrameOutOfMgz() == TRUE)		//v4.18T1
			{
				AbortElevatorMotionIfOutOfMag();
				SetErrorMessage("BL Frame is out of magazine with frame not exist on upper buffer frame");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
				HouseKeeping(m_lReadyPos_X);
				return FALSE;
			}
			else
			{
				if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
				{
					SetErrorMessage("Upper buffer frame not detected and aborted");
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
			}
		}
	}
	else if (bBufferLevel == BL_BUFFER_LOWER)
	{
		//Check upper Buffer has frame or not
		bFrameExist = IsDualBufferLowerExist();
		if (!bFrameExist)
		{
			Sleep(50);	//Sleep(200);	//v4.18T1
			bFrameExist = IsDualBufferLowerExist();
		}

		if ( (bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
		{
			bFrameExist = TRUE;
		}

		if ( bFrameExist == FALSE )
		{
			BL_DEBUGBOX("Lower buffer frame not detected ...");
			HmiMessage_Red_Back("Lower buffer frame not detected ....");

			if (IsFrameOutOfMgz() == TRUE)		//v4.18T1
			{
				AbortElevatorMotionIfOutOfMag();
				SetErrorMessage("BL Frame is out of magazine with frame not exist on lower buffer frame");
				SetAlert_Red_Yellow(IDS_BL_FRMAE_OUTMAGZ);
				HouseKeeping(m_lReadyPos_X);
				return FALSE;
			}
			else
			{
				if ( BL_YES_NO(HMB_BL_BUFF_NO_FRAME, IDS_BL_LOAD_FRAME) == FALSE )
				{
					SetErrorMessage("Lower buffer frame not detected and aborted");
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
			}
		}
	}

	//Check bin frame sits in the center on the buffer
	if (m_bIsNewDualBuffer == FALSE)
	{
		bFrameProtect = IsDualBufferLeftProtect() && IsDualBufferRightProtect();
	}
	else
	{
		bFrameProtect = !IsDualBufferLeftProtect();
	}

	if ( (bBurnInUnload == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
	{
		bFrameProtect = TRUE;
	}
	if (bFrameProtect == FALSE)
	{
		SetErrorMessage("Tbl->Buf: Frame does not sit in the buffer center (warning only)");
	}

	if (bExchangingFrame == FALSE)
	{
		BL_DEBUGBOX("down table");
		SetFrameLevel(FALSE);

		Sleep(1000);

		//v3.70T3
		if (IsFrameLevel() == FALSE)
		{
			Sleep(50);

			INT nCount = 0;
			while (IsFrameLevel() == FALSE)
			{
				Sleep(50);
				nCount++;
				if (nCount > 50)
					break;
			}

			if (IsFrameLevel() == FALSE)
			{
				HmiMessage_Red_Yellow("ERROR: please check frame level on output table!");
				SetErrorMessage("check frame level error");
				BL_DEBUGBOX("check frame level error");
				HouseKeeping(m_lPreUnloadPos_X);
				return FALSE;
			}
		}
	}

	if (bBufferLevel == BL_BUFFER_LOWER)
	{
		BL_DEBUGBOX("down buffer");
		SetBufferLevel(FALSE);
		Sleep(300);

		if ( IsDualBufferLevelDown() == FALSE)
		{
			SetBufferLevel(FALSE);
			Sleep(300);

			//v3.71T1	//Insert 1sec timeout
			INT nCount = 0;
			while (!IsDualBufferLevelDown())
			{
				Sleep(50);
				nCount++;
				if (nCount > 20)
					break;
			}

			if ( IsDualBufferLevelDown() == FALSE)
			{
				Sleep(100);
				if ( IsDualBufferLevelDown() == FALSE)
				{
					SetErrorMessage("BL Buffer table is not down 12");
					SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_DOWN);
					HouseKeeping(m_lReadyPos_X);
					return FALSE;
				}
			}
		}		
	}
	else if (bBufferLevel == BL_BUFFER_UPPER)
	{
	}

	TRY {
		SaveData();
		//SaveMgznRTData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData !!!");
	}
	END_CATCH

 	TRY {
		SaveMgznRTData();		//v4.06
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH


	szText1.Format("#6 U.Buffer = %d BC %s, L.Buffer = %d BC %s, Current = %d BC %s",
		m_clUpperGripperBuffer.GetBufferBlock(), m_clUpperGripperBuffer.GetBufferBarcode(),
		m_clLowerGripperBuffer.GetBufferBlock(), m_clLowerGripperBuffer.GetBufferBarcode(),
		GetCurrBinOnBT(), m_szBinFrameBarcode);
	BL_DEBUGBOX(szText1);
    BL_DEBUGBOX("Unload from Table to Buffer finish\n");
	return TRUE;
}

*/
