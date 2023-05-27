/////////////////////////////////////////////////////////////////
// BL_SubState.cpp : SubState for Auto-cycle of the CBinLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, June 21, 2005
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BinLoader.h"
#include "MS896a.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////////////////
//	Check hardware ready 
////////////////////////////////////////////
BOOL CBinLoader::IsMotionHardwareReady()
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}
	if (m_bDisableBL)		//v3.60
	{
		return TRUE;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A45
	{
		CString szText;
		//Power On/Off
		if (!Upper_IsPowerOn())
		{
			szText = "\nBinLoader Upper Gripper is not power ON";
			SetStatusMessage("BinLoader Upper Gripper NOT power ON yet");
			SetErrorMessage("BinLoader Upper Gripper NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if (!Lower_IsPowerOn())
		{
			szText = "\nBinLoader Lower Gripper is not power ON";
			SetStatusMessage("BinLoader Lower Gripper NOT power ON yet");
			SetErrorMessage("BinLoader Lower Gripper NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if (!Theta_IsPowerOn())
		{
			szText = "\nBinLoader Theta is not power ON";
			SetStatusMessage("BinLoader Theta NOT power ON yet");
			SetErrorMessage("BinLoader Theta NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if (!Z_IsPowerOn())
		{
			szText = "\nBinLoader Z is not power ON";
			SetStatusMessage("BinLoader Z NOT power ON yet");
			SetErrorMessage("BinLoader Z NOT power ON yet");
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			return FALSE;
		}

		if ((MotionCheckResult(BL_AXIS_UPPER, &m_stBLAxis_Upper) == FALSE) ||
			(MotionCheckResult(BL_AXIS_LOWER, &m_stBLAxis_Lower) == FALSE) || 
			(MotionCheckResult(BL_AXIS_THETA, &m_stBLAxis_Theta) == FALSE) || 
			(MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z) == FALSE) ||
			!m_bHome_Upper || !m_bHome_Lower || !m_bHome_Theta || !m_bHome_Z)
		{
			StopCycle("BinLoaderStn");
			//(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
			SetMotionCE(TRUE, "ERROR: CE Set by BL IsMotionHardwareReady");		//v4.59A39

			if ((MotionCheckResult(BL_AXIS_UPPER, &m_stBLAxis_Upper) == FALSE) || !m_bHome_Upper)
			{
				szText = "\nBinGripper";
				SetStatusMessage("BinGripper (UPPER) NOT home yet");
				SetErrorMessage("BinGripper (UPPER) NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			}

			if ((MotionCheckResult(BL_AXIS_LOWER, &m_stBLAxis_Lower) == FALSE) || !m_bHome_Lower)
			{
				szText = "\nBinGripper 2";
				SetStatusMessage("BinGripper (LOWER) NOT home yet");
				SetErrorMessage("BinGripper (LOWER) NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			}

			if ((MotionCheckResult(BL_AXIS_THETA, &m_stBLAxis_Theta) == FALSE) || !m_bHome_Theta)
			{
				szText = "\nBinLoaderT";
				SetStatusMessage("BinLoaderT NOT home yet");
				SetErrorMessage("BinLoaderT NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			}
			
			if ((MotionCheckResult(BL_AXIS_Z, &m_stBLAxis_Z) == FALSE) || !m_bHome_Z)
			{
				szText = "\nBinLoaderZ";
				SetStatusMessage("BinLoaderZ NOT home yet");
				SetErrorMessage("BinLoaderZ NOT home yet");
				SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_NOT_HOME, szText);
			}

			return FALSE;
		}
	}

	return TRUE;
}


////////////////////////////////////////////
//	Sequence Operations
////////////////////////////////////////////

INT CBinLoader::OpInitialize()
{
	INT nResult				= gnOK;

	m_qPreviousSubOperation	= -1;
	m_qSubOperation			= 0;

	NeedReset(FALSE);

	DisplaySequence("BL - Operation Initialize");
	return nResult;
}

INT CBinLoader::OpPreStart()
{
	INT nResult = gnOK;
	//m_szCycleMessageBL		= "";

	if (m_fHardware == FALSE)
	{
		m_bHome_X = TRUE;
		//m_bHome_Y = TRUE;
		m_bHome_Z = TRUE;
		//m_bComm_Y = TRUE;
		m_bComm_Z = TRUE;
		m_bHome_Arm = TRUE;
		m_bComm_Arm = TRUE;
		m_bNoSensorCheck = TRUE;
	}
	else if (m_bDisableBL)
	{
		m_bHome_X = TRUE;
		//m_bHome_Y = TRUE;
		m_bHome_Z = TRUE;
		//m_bComm_Y = TRUE;
		m_bComm_Z = TRUE;
		m_bHome_Arm = TRUE;
		m_bComm_Arm = TRUE;
	}
	
	if ( IsMotionHardwareReady() == TRUE || PreStartInit()==TRUE )
	{
		m_qSubOperation	= WAIT_OPERATE_Q;
		DisplaySequence("BL - Prestart ==> Wait Operate");
	}
	else
	{
		m_qSubOperation	= HOUSE_KEEPING_Q;
	}

	//v2.68
	if (m_bMotionFail == TRUE)		//Auto-reset binloader module error
		m_bMotionFail = FALSE;
	
	if ( m_bMotionFail == FALSE )
	{
		OpHouseKeeping_Move();
	}

	m_bCoverSensorAbort = FALSE;	//v3.60
	
	if (CMS896AStn::m_bNGPick)		//v4.51A20
	{
		m_bNoSensorCheck = TRUE;
	}

	//v3.02T4
	CMSLogFileUtility::Instance()->BL_BackupLogStatus();	
	CMSLogFileUtility::Instance()->BL_BackupChangeGradeLog();
	CMSLogFileUtility::Instance()->BL_BackupLoadUnloadLog();
	CMSLogFileUtility::Instance()->WL_BackupLogStatus();
	CopyCreeBinBcSummaryFile();		//v3.75		//Cree China

	SetBLOperate(FALSE);	
	SetBLAOperate(FALSE);	
	SetBLPreOperate(FALSE);	
	SetBLPreChangeGrade(FALSE);				//v2.71
	//m_szAutoPreLoadBarCode = "";
	m_nExArmReAlignBinConvID = 0;			//v2.72a5
	m_bDLAChangeGrade	= FALSE;			//v3.10T3
	m_bBarcodeReadAbort	= FALSE;			//v2.83T45		//v3.71T3
	m_szDBPreScanBCName = "";				//pllm
	m_lOpBTForUnload	= 0;				//0=none, 1=BT1, 2=BT2	//v4.21T1
	m_bExArmPreloadFrame = FALSE;			//v4.31T6	//Lextar to avoid gripper collision with BT when autoclean collet 
	m_bFrameToBeUnloadAtIdleState = FALSE;	//v4.42T17	//Unload last Frame after WaferEnd

	m_clLowerGripperBuffer.SetTriggerThetaHome(FALSE);
	m_clUpperGripperBuffer.SetTriggerThetaHome(FALSE);
	m_clLowerGripperBuffer.SetContTriggerThetaHomeCounter(0);
	m_clUpperGripperBuffer.SetContTriggerThetaHomeCounter(0);

	(*m_psmfSRam)["BinLoader"]["PreLoadEmpty"] = FALSE;		//v4.22T5

	//v3.70T2	//PLLM special feature
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS) && !IsBurnIn() && !m_bRealignBinFrame)
	{
		//m_bRealignBinFrame = TRUE;		//v2.83T60		//v4.36
	}
	if( pApp->GetCustomerName()=="Semitek" )
	{
		m_bCompareBarcode		= TRUE;
	}

	//v4.40T7
	if( (pApp->GetCustomerName()==CTM_NICHIA)	&&
		(pApp->GetProductLine() == _T(""))		&&		//v4.59A34
		!IsBurnIn() )
	{
		if ((GetCurrBinOnBT() != 0) && (m_bUseBarcode))
		{
			LONG lCurrMgzn=0, lCurrSlot=0;
			GetMgznSlot(GetCurrBinOnBT(), lCurrMgzn, lCurrSlot);
			CString szSlotBarcode = m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot];

			BOOL bBinSheetCheck = CMS896AStn::m_oNichiaSubSystem.CheckBinSheetType(szSlotBarcode);
			if (!bBinSheetCheck)
			{
				CString szErr;
				szErr.Format("Nichia: bin sheet type check fails with %ld; barcode = " + szSlotBarcode, GetCurrBinOnBT());
				SetErrorMessage(szErr);
				//HmiMessage_Red_Yellow(szErr);
				szErr.Format("Nichia: bin sheet type check fails with %ld; \nbarcode = " + szSlotBarcode, GetCurrBinOnBT());
				SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_ERROR, szErr, "CLOSE");		//anichia003
				m_qSubOperation	= HOUSE_KEEPING_Q;
			}
		}
	}


	if ( IsBurnIn() == TRUE )
	{
		LoadMgznRTData();
	}

	m_bBurnInEnable = IsBurnIn();	//v3.89		//Updated once before AUTOBOND only

	DisplaySequence("BL - Operation Prestart");
	m_bIsFirstCycle = TRUE;

	return nResult;
}

INT CBinLoader::OpHouseKeeping_Move()
{
	INT nResult				= gnOK;
/* If problem occurs, further movement may make the situation worser
	// Incomplete ...

	BL_DEBUGBOX("x ready, down magazine");
    BTFramePlatformDown();
    SetGripperLevel(FALSE);
 	Sleep(500);
    BinGripperMoveTo(m_lReadyPos_X);
    Sleep(500);
	
	// send event to let bin table go
	DownElevatorToReady();

	BL_DEBUGBOX("bin table to ready");
	if ( MoveBinTable(0, 0) == FALSE )
		return gnNOTOK;
*/

    if ( CheckElevatorCover() == FALSE )
    {
		return FALSE;
    }

	// down elevator to ready

    BinGripperMoveTo(m_lReadyPos_X);
	
	//v2.78T2
	//if ( DownElevatorToReady_Auto() == FALSE )
	//{
	//	return gnNOTOK;
	//}

	m_bIsFirstCycle = FALSE;

	return nResult;
}


BOOL CBinLoader::OpNeedResetEmptyFullMgzn()
{
	INT i=0;

	//v4.39T10	//Cree US
	BOOL bUseElevatorCoverLock = FALSE;
	if ( (CMS896AApp::m_lBinElevatorCoverLock == TRUE) && (m_bEnableBinElevatorCoverLock == TRUE) )
		bUseElevatorCoverLock = TRUE;

	//v4.39T3
	LONG lMode = (*m_psmfSRam)["BinTable"]["BL_Mode"];
	if ( (m_lOMRT == BL_MODE_D) && (lMode != 1) )	//if NOT to BIN_FULL_Q
	{
		return TRUE;
	}

	//v4.56A11
	if (IsMSAutoLineMode())
	{
		return TRUE;
	}


	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bRTMgznExchange = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE);	//v4.41T2

	for (i=0; i<MS_BL_MGZN_NUM; i++)
	{
		//v4.25	//Cree HuiZhou
		if (m_lOMRT == BL_MODE_D)
		{
			if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_MIXED)
			{
				BOOL bNoFullSlot = TRUE;
				for (INT j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
				{
					if( m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_FULL )
						continue;
					if( m_stMgznRT[i].m_lSlotBlock[j] == 0 )
					{
						bNoFullSlot = FALSE;
						break;
					}
				}

				if (bNoFullSlot)
				{
					CString szLog;
					szLog.Format("BL cycle Reset mgzn (Mode D) in OpNeedResetEmptyFullMgzn: %d", i);
					BL_DEBUGBOX(szLog);

					//Move elevator to ready position for replace magazine
					if (DownElevatorThetaZToPosn(0) == FALSE)
					{
						//m_qSubOperation = HOUSE_KEEPING_Q;
						//break;
						return FALSE;
					}
					
					//v4.39T10	//Cree US
					if (bUseElevatorCoverLock)
					{
						SetBinElevatorCoverLock(FALSE);
					}

					LONG lStatus = 0;
					if (pApp->GetCustomerName() == "ChangeLight(XM)")
					{
						lStatus = BL_YES_NO(IsMSAutoLineStandloneMode() ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET_CHANGELIGHTXM : HMB_BL_MAGZ_IS_FULL_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
					}
					else
					{
						lStatus = BL_YES_NO(IsMSAutoLineStandloneMode() ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET : HMB_BL_MAGZ_IS_FULL_RESET, IDS_BL_OPERATION);
					}
					if (lStatus == TRUE)	
					{
						OpOutoutMgznSummary();	
						ResetMagazineFull(i);
					}

					//v4.39T10	//Cree US
					if (bUseElevatorCoverLock)
					{
						SetBinElevatorCoverLock(TRUE);
						Sleep(500);

						INT nCount = 0;
						while (!IsBinElevatorCoverLocked())
						{
							SetBinElevatorCoverLock(FALSE);
							HmiMessage_Red_Green("Please close elevator cover to continue!", "Reset Magazine");
							SetBinElevatorCoverLock(TRUE);
							Sleep(500);
							nCount++;
							if (nCount>3)
								break;
						}
					}
				
				}
			}
		}
		else
		{
			if( m_stMgznRT[i].m_lMgznState == BL_MGZN_STATE_EMPTY )
			{
				BOOL bNeedReset = TRUE;

				if (bRTMgznExchange)	//v4.41T2
				{
					for (INT j=0; j<MS_BL_MGZN_NUM; j++)
					{
						if (j == i)
							continue;
						if ( (m_stMgznRT[j].m_lMgznUsage == BL_MGZN_USAGE_EMPTY) &&
							 (m_stMgznRT[j].m_lMgznState == BL_MGZN_STATE_OK) )
						{
							bNeedReset = FALSE;
							break;
						}
					}
				}

				if (bNeedReset)
				{
					if (m_bBurnInEnable)		//v4.31T1	//Auto-reset mgzn for BURNIN mode only
					{
						ResetMagazine(i);
						m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
					}
					else
					{
						//Move elevator to ready position for replace magazine
						if (DownElevatorThetaZToPosn(0) == FALSE)
						{
							//m_qSubOperation = HOUSE_KEEPING_Q;
							//break;
							return FALSE;
						}

						//v4.39T10	//Cree US
						if (bUseElevatorCoverLock)
						{
							SetBinElevatorCoverLock(FALSE);
						}

						LONG lStatus = 0;
						if (pApp->GetCustomerName() == "ChangeLight(XM)")
						{
							lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
						}
						else
						{
							lStatus = BL_YES_NO(HMB_BL_MAGZ_IS_EMPTY_RESET, IDS_BL_OPERATION);
						}

						if (lStatus == TRUE )
						{
							ResetMagazine(i);
							m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;

							//v4.41T2
							//Also check other EMPTY mgzns
							for (INT k=0; k<MS_BL_MGZN_NUM; k++)
							{
								if (k == i)
									continue;
								if ( (m_stMgznRT[k].m_lMgznUsage == BL_MGZN_USAGE_EMPTY) &&
									 (m_stMgznRT[k].m_lMgznState == BL_MGZN_STATE_EMPTY) )
								{
									ResetMagazine(k);
									m_stMgznRT[k].m_lMgznState = BL_MGZN_STATE_OK;
								}
							}
						}

						//v4.39T10	//Cree US
						if (bUseElevatorCoverLock)
						{
							SetBinElevatorCoverLock(TRUE);
							Sleep(500);

							INT nCount = 0;
							while (!IsBinElevatorCoverLocked())
							{
								SetBinElevatorCoverLock(FALSE);
								HmiMessage_Red_Green("Please close elevator cover to continue!", "Reset Magazine");
								SetBinElevatorCoverLock(TRUE);
								Sleep(500);
								nCount++;
								if (nCount>3)
									break;
							}
						}
					}
				}//End If (bNeedReset)
			}

			if( m_stMgznRT[i].m_lMgznState==BL_MGZN_STATE_FULL )
			{
				BOOL bNeedReset = TRUE;

				if (bRTMgznExchange)	//v4.41T2
				{
					for (INT j=0; j<MS_BL_MGZN_NUM; j++)
					{
						if (j == i)
							continue;
						if ( (m_stMgznRT[j].m_lMgznUsage == BL_MGZN_USAGE_FULL) &&
							 (m_stMgznRT[j].m_lMgznState == BL_MGZN_STATE_OK) )
						{
							bNeedReset = FALSE;
							break;
						}
					}
				}

				if (bNeedReset)
				{
					if (m_bBurnInEnable)		//v4.31T1	//Auto-reset mgzn for BURNIN mode only
					{
						OpOutoutMgznSummary();
						ResetMagazine(i);
						m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;
					}
					else
					{
						//Move elevator to ready position for replace magazine
						if (DownElevatorThetaZToPosn(0) == FALSE)
						{
							//m_qSubOperation = HOUSE_KEEPING_Q;
							//break;
							return FALSE;
						}
						
						//v4.39T10	//Cree US
						if (bUseElevatorCoverLock)
						{
							SetBinElevatorCoverLock(FALSE);
						}

						LONG lStatus = 0;
						if (pApp->GetCustomerName() == "ChangeLight(XM)")
						{
							lStatus = BL_YES_NO(IsMSAutoLineStandloneMode() ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET_CHANGELIGHTXM : HMB_BL_MAGZ_IS_FULL_RESET_CHANGELIGHTXM, IDS_BL_OPERATION);
						}
						else
						{
							lStatus = BL_YES_NO(IsMSAutoLineStandloneMode() ? HMB_BL_AUTO_LINE_CLEAR_FRAME_MAGZ_IS_FULL_RESET : HMB_BL_MAGZ_IS_FULL_RESET, IDS_BL_OPERATION);
						}
						
						if (lStatus == TRUE)	//v3.88 Change EMPTY to FULL msg
						{
							OpOutoutMgznSummary();	//v4.16T1		//HPO by Leo Hung
							ResetMagazine(i);
							m_stMgznRT[i].m_lMgznState = BL_MGZN_STATE_OK;

							//v4.41T2
							//Also check other FULL mgzns
							for (INT k=0; k<MS_BL_MGZN_NUM; k++)
							{
								if (k == i)
									continue;
								if ( (m_stMgznRT[k].m_lMgznUsage == BL_MGZN_USAGE_FULL) &&
									 (m_stMgznRT[k].m_lMgznState == BL_MGZN_STATE_FULL) )
								{
									ResetMagazine(k);
									m_stMgznRT[k].m_lMgznState = BL_MGZN_STATE_OK;
								}
							}
						}

						//v4.39T10	//Cree US
						if (bUseElevatorCoverLock)
						{
							SetBinElevatorCoverLock(TRUE);
							Sleep(500);

							INT nCount = 0;
							while (!IsBinElevatorCoverLocked())
							{
								SetBinElevatorCoverLock(FALSE);
								HmiMessage_Red_Green("Please close elevator cover to continue!", "Reset Magazine");
								SetBinElevatorCoverLock(TRUE);
								Sleep(500);
								nCount++;
								if (nCount>3)
									break;
							}
						}
					}
				}//END if (bNeedReset)
			}
		}
	}

	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveData in CycleState !!!");
	}
	END_CATCH

	TRY {
		SaveMgznRTData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	return TRUE;
}


INT CBinLoader::OpMoveToFullMagzine()
{
	m_lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
	m_lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];

	LONG lMgzn = m_lCurrMgzn;
	LONG lSlot = m_lCurrSlot;
	if ( GetFullMgznSlot(lMgzn, lSlot) == FALSE )
	{
		return FALSE;
	}
	else
	{
		if ( MoveElevatorToUnload(lMgzn, lSlot, TRUE, TRUE) == FALSE )
		{
			return FALSE;
		}
	}
	return TRUE;
}


//================================================================
// OpPreMoveElevatorToNextLoad()
//   Created-By  : Andrew Ng
//   Date        : 10/6/2007 12:05:45 PM
//   Description : 
//   Remarks     : 
//================================================================
INT CBinLoader::OpPreMoveElevatorToNextLoad(CONST ULONG ulCurrBlk, BOOL bUseBT2)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg, svMsg;
	ULONG ulBlk = ulCurrBlk;

	CString szLog;
	szLog.Format("OpPreMoveElevator To NextLoad: Grade=%lu, BT2=%d",  ulCurrBlk, bUseBT2);		//v4.23T2
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsEnablePNP() || pUtl->GetPrescanDummyMap() )
	{
		ulBlk = 0;
	}
	else
	{
		stMsg.InitMessage(sizeof(ULONG), &ulBlk);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("GetNextGradeBlk"), stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 1000) == TRUE )		//1sec
			{
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		svMsg.GetMsg(sizeof(ULONG), &ulBlk);
	}

	LONG lCurrMgzn, lCurrSlot;
	CString szErr;

	if (ulBlk == 0)
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("OpPreMoveElevatorToNextLoad: zero blk detected!\n");
		return FALSE;
	}
	else if (ulBlk == 999)		//v2.93T2
	{
		//anichia002
		if ( (pApp->GetCustomerName() == CTM_NICHIA)	&& 
			(pApp->GetProductLine() == _T(""))			&&				//v4.59A34
			 (CMS896AStn::m_oNichiaSubSystem.m_ucProcessMode == 1) )	//Single Mode
		{
			BOOL bNewMagCheck = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_NEW_BL_MAGEXIST_CHECKING);
			if (GetFullMgznSlot(lCurrMgzn, lCurrSlot, TRUE))	//v3.88	//Add bPreLoad=TRUE
			{
				LONG lMoveStatus = 0;
				lMoveStatus = MoveElevatorToUnload(lCurrMgzn, lCurrSlot, TRUE, FALSE, FALSE, FALSE, bUseBT2);

				if ( lMoveStatus == FALSE )
				{
					CString szErr = "BL: pre-move elevator to next unload FULL slot fails";
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);
					return FALSE;
				}
			}
		}
		else
		{
			// curr blk no is "last" grade in grade-list, so no pre-move to NEXT is triggered
			CMSLogFileUtility::Instance()->BL_LogStatus("Pre-Move elevator: 999 blk detected!\n");

			if (GetMgznSlot(ulCurrBlk, lCurrMgzn, lCurrSlot))
			{
				if ( MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, TRUE, FALSE, bUseBT2) == FALSE )		//anichia001
				{
					HmiMessage_Red_Yellow("BL: pre-move elevator to last slot fails");
					return FALSE;
				}
			}
		}
	}
	else if (ulBlk == ulCurrBlk)		//New Empty frame 
	{
		//v2.93T2
		
		BOOL bDisablePreLoadEmpty = ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD);

		CString szTemp;
		szTemp.Format("1Pre-Move elevator to from frame #%d to empty #%d, Preloadempty %d\n", ulCurrBlk, ulBlk, bDisablePreLoadEmpty);
		CMSLogFileUtility::Instance()->BL_LogStatus(szTemp);

		//v3.94
		if (bDisablePreLoadEmpty)
		{
			if (GetFullMgznSlot(lCurrMgzn, lCurrSlot, TRUE) == TRUE)
			{
				if ( MoveElevatorToUnload(lCurrMgzn, lCurrSlot, TRUE, FALSE, FALSE, FALSE, bUseBT2) == FALSE )
				{
					CString szErr = "BL: pre-move elevator to next unload FULL slot fails";
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);
					return FALSE;
				}
			}
		}
		else
		{
			if (GetNextNullMgznSlot(lCurrMgzn, lCurrSlot))	
			{
				if (MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, FALSE, FALSE, bUseBT2) == FALSE)
				{
					szErr = "BL: pre-move elevator to next empty slot fails";
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);	
					return FALSE;
				}
			}
		}
	}
	else
	{
		//v2.93T2
		CString szTemp;
		szTemp.Format("2Pre-Move elevator to from frame #%d to #%d\n", ulCurrBlk, ulBlk);
		CMSLogFileUtility::Instance()->BL_LogStatus(szTemp);

		// Move elevator to next LOAD posn
		if (GetMgznSlot(ulBlk, lCurrMgzn, lCurrSlot))
		{
			//v4.44T5	//Cree HuiZhou	//Long
			if (m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] == BL_SLOT_USAGE_ACTIVE2FULL)
			{
				if (GetNextNullMgznSlot(lCurrMgzn, lCurrSlot))	
				{
					if ( MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, FALSE, FALSE, bUseBT2) == FALSE )
					{
						szErr = "BL: pre-move elevator to next GRADE empty slot fails";
						HmiMessage_Red_Yellow(szErr);
						SetErrorMessage(szErr);	
						return FALSE;
					}
				}
			}
			else
			{
				if ( MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, FALSE, FALSE, bUseBT2) == FALSE )
				{
					szErr = "BL: pre-move elevator to next slot fails";
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);	
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}


//v2.93T2
INT CBinLoader::OpPreMoveElevatorToNextUnLoad(CONST BOOL bFull, BOOL bUseBT2)
{
	if ((GetCurrBinOnBT() <= 0) && (m_lBTCurrentBlock2 <= 0))
		return TRUE;

	LONG lCurrMgzn, lCurrSlot;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bNewMagCheck = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_NEW_BL_MAGEXIST_CHECKING);
	LONG lMoveStatus = TRUE;

	if (bFull)
	{
		//v4.59A20	//For Renesas MS90 special BL config
		LONG lFullFrameToBeUnload = GetCurrBinOnBT();
		if (bUseBT2)
			lFullFrameToBeUnload = m_lBTCurrentBlock2;

		if (GetFullMgznSlot(lCurrMgzn, lCurrSlot, TRUE, lFullFrameToBeUnload))	//v3.88	//Add bPreLoad=TRUE
		{
			lMoveStatus = MoveElevatorToUnload(lCurrMgzn, lCurrSlot, TRUE, FALSE, FALSE, FALSE, bUseBT2);
			
			if ( lMoveStatus == FALSE )
			{
				CString szErr = "BL: pre-move elevator to next unload FULL slot fails";
				HmiMessage_Red_Yellow(szErr);
				SetErrorMessage(szErr);
				return FALSE;
			}
		}
	}
	else
	{
		LONG lStatus = TRUE;
		if (bUseBT2)
			lStatus = GetMgznSlot(m_lBTCurrentBlock2, lCurrMgzn, lCurrSlot);
		else
			lStatus = GetMgznSlot(GetCurrBinOnBT(), lCurrMgzn, lCurrSlot);

		if (lStatus == TRUE)
		{
			lMoveStatus = MoveElevatorToUnload(lCurrMgzn, lCurrSlot, TRUE, FALSE, FALSE, FALSE, bUseBT2);
			
			if ( lMoveStatus == FALSE )
			{
				CString szErr = "BL: pre-move elevator to next unload slot fails";
				HmiMessage_Red_Yellow(szErr);
				SetErrorMessage(szErr);
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*
INT CBinLoader::OpDBufferChangeGradeFrames()
{
	LONG lFrameOnBT = 0;
	BOOL bGoToUnload = FALSE;
	BOOL bGoToLoad = FALSE;
	BOOL bGoToChange = FALSE;
	BOOL bUnloadBuffer = FALSE;
	BOOL bDirectLoadToTable = FALSE;
	BOOL bBufferToTable = FALSE;
	BOOL bBinFull = FALSE;

	LONG lPreloadBufferBlock = DB_PreloadBlock();

	ULONG ulNextBlk		= (*m_psmfSRam)["BinTable"]["BLA_Mode"]["Next Blk"];
	ULONG ulBlkInUse	= (*m_psmfSRam)["BinTable"]["PhyBlkInUse"]; // physical block id
	
	if (m_bNoSensorCheck)
		lFrameOnBT	= BL_FRAME_ON_CENTER;
	else
		lFrameOnBT	= CheckFrameOnBinTable();			// check frame on bin table physically

	if ( (m_bBurnInEnable == TRUE) || (m_bNoSensorCheck == TRUE) || m_bDisableBL)
	{
		lFrameOnBT = BL_FRAME_ON_CENTER;
		if (GetCurrBinOnBT() == 0)
		{
			lFrameOnBT = BL_FRAME_NOT_EXIST;
		}
	}

	if (lFrameOnBT == BL_FRAME_NOT_EXIST)
	{
		//No Frame is detected
		if ( GetCurrBinOnBT() == 0 )
		{
			if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//bin table no frame
			{
				bGoToUnload	= FALSE;
				bBufferToTable = FALSE;
				bGoToChange	= FALSE;	//do not use buffer table to exchange frame

				if ( lPreloadBufferBlock == ulBlkInUse )	//frame on buffer is the next grade
				{
					bGoToLoad = FALSE;
					bBufferToTable = TRUE;	//load from buffer table
					bUnloadBuffer = FALSE;
				}
				else if ( lPreloadBufferBlock == 0 )	//No frame on buffer
				{
					//bGoToLoad = TRUE;
					bDirectLoadToTable = TRUE;
					bUnloadBuffer = FALSE;
					ulNextBlk = ulBlkInUse;
				}
				else	//Frame on buffer is not the next grade
				{
					//bGoToLoad = TRUE;
					bDirectLoadToTable = TRUE;
					bUnloadBuffer = TRUE;
					ulNextBlk = ulBlkInUse;
				}
			}
		}
		else
		{
			//Error
			CString szString;
			szString.Format("\n%d", GetCurrBinOnBT());

			SetAlert_Msg_Red_Yellow(IDS_BL_NOFRAME_EXIST_SYS, szString);

			szString.Format("BL - No Frame exist but current block = %d", GetCurrBinOnBT());
			SetErrorMessage(szString);

			//m_qSubOperation = HOUSE_KEEPING_Q;
			return FALSE;
		}
	}
	else
	{
		//Frame is detected
		if ( GetCurrBinOnBT() == 0 )
		{
			//Error		
			CString szString;
			szString = "\n0";

			SetAlert_Msg_Red_Yellow(IDS_BL_FRAME_EXIST_SYS, szString);
			SetErrorMessage("BL - Frame exist but current block = 0");

			//m_qSubOperation = HOUSE_KEEPING_Q;
			return FALSE;
		}
		else
		{
			if ( GetCurrBinOnBT() != ulBlkInUse )
			{
				bGoToUnload	= TRUE;
				bGoToChange	= TRUE;

				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
				{
					if ( lPreloadBufferBlock == ulBlkInUse )	// frame in Preload buffer is the next grade
					{
						bUnloadBuffer = FALSE;					//no need Preload uppper buffer frame
						bGoToLoad = FALSE;						//no need loading to buffer
					}
					else if ( lPreloadBufferBlock == 0 )	// no frame on buffer table
					{
						bUnloadBuffer = FALSE;				//no need to unload buffer frame
						bGoToLoad = TRUE;					//load next grade to buffer table
						ulNextBlk = ulBlkInUse;
					}
					else									// frame on buffer is NOT the next grade
					{
						bUnloadBuffer = TRUE;				//unload buffer frame
						bGoToLoad = TRUE;					//load the correct next grade to buffer table
						ulNextBlk = ulBlkInUse;
					}
				}
			}
		}
	}

				
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		SetSuckingHead(TRUE);
		if ( OpDualBufferChangeGrade(bUnloadBuffer, bGoToLoad, bBufferToTable, bGoToChange, bDirectLoadToTable, (LONG)ulNextBlk) == FALSE )
		{
			SetSuckingHead(FALSE);

			m_bMotionFail = TRUE;
			//m_qSubOperation = HOUSE_KEEPING_Q;
			return FALSE;	
		}
		SetSuckingHead(FALSE);	
	}

	m_bDLAChangeGrade = TRUE;			// Tell BH to increase delay for 1st cycle	//v3.10T3
	if (!bGoToChange)					//v2.93T2
		SetBLReady(TRUE);				//Wait until bin realign result is back!!	//v2.72a5	//v2.78T2	//v2.82T4
	SetBLAOperate(FALSE);				//Make sure Chg-Grade event is cleared from BT
	// LOG_LoadUnloadTime("End\n\n", "a+");	
	CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("End\n\n");


	if ( bGoToUnload == TRUE )
	{
		m_qSubOperation = DBUFFER_UNLOAD_FRAME_Q;
	}
	else
	{
		m_qSubOperation = WAIT_OPERATE_Q;
		DisplaySequence("BL - Exarm Change Grade ==> Wait Operate");
		Z_Sync();
//		Y_Sync();

		//Wait until Realign reault back if UNLOAD is not needed
		if (bGoToChange && !RealignBinFrameRpy())
		{
			HmiMessage_Red_Yellow("BL: Realignbin frame fails 1!");
			m_bMotionFail = TRUE;
			//m_qSubOperation = HOUSE_KEEPING_Q;
			return FALSE;
		}

		OpPreMoveElevatorToNextLoad(GetCurrBinOnBT());	
	}
	
	return TRUE;
}
*/

INT CBinLoader::OpDualBufferChangeGrade(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bBufferToTable, BOOL bGoToChange, BOOL bDirectLoadToTable, LONG lLoadBlk)
{
	CString szLog;
	CBinGripperBuffer *pUpperGripperBuffer = DB_GetPreloadGripperBuffer();
	CBinGripperBuffer *pLowerGripperBuffer = DB_GetUnloadGripperBuffer();
	LONG lBufferBlock = DB_PreloadBlock();
	INT nLoadBinFrameStatus = 0;

	//CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("START");
	szLog.Format("CHANGE-GRADE (UDB #%ld) Start **** (UNLOADBuf=%d, LoadFrmMgzn=%d, BufToTable=%d, GoToChg=%d, DirectLoad=%d)", 
		lLoadBlk, bUnloadBuffer, bGoToLoad, bBufferToTable, bGoToChange, bDirectLoadToTable);
	BL_DEBUGBOX(szLog);

	//Safe to reset here because DBH already at PREPICK state at this moment
	(*m_psmfSRam)["BinLoader"]["PreLoadEmpty"] = FALSE;	


	if ( bUnloadBuffer )
	{
		/*if (!m_bDualBufferPreLoadEmptyFrame && (lBufferBlock == m_lBTCurrentBlock))
		{
			CMSLogFileUtility::Instance()->BL_LogStatus("DBF change grade Preload empty set to true");
			m_bDualBufferPreLoadEmptyFrame = TRUE;
		}*/

		if ( UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, FALSE, pUpperGripperBuffer) == FALSE )	
		{
			m_bDualBufferPreLoadEmptyFrame = FALSE;
			BL_DEBUGBOX("Unload From Buffer to Mgz with lock Fail!");
			SetErrorMessage("Unload From Buffer to Mgz with lock Fail!");
			return FALSE;
		}

		m_bDualBufferPreLoadEmptyFrame = FALSE;
	}


	// Load next grade frame onto buffer
	if ( bGoToLoad || bDirectLoadToTable )
	{
		LONG lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, lLoadBlk, pUpperGripperBuffer);

		//** New DISCARD EMPTY Frame method **//
		if (lLoadStatus == Err_BLLoadEmptyBarcodeFail)	
		{
			BL_DEBUGBOX("BL: UDB Change load EMPTY Discard retry");	

			do {
				BOOL bBcUpdate = FALSE;
				if (m_bUseBarcode)
				{
					m_bUseBarcode = FALSE;
					bBcUpdate = TRUE;
				}

				if ( UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, TRUE, pUpperGripperBuffer) == FALSE )
				{
					if (bBcUpdate)
						m_bUseBarcode = TRUE;
					SetErrorMessage("BL: UDB Change load EMPTY Discard retry fail");
					return FALSE;
				}

				LONG lCurrMgzn=0, lCurrSlot=0;
				GetMgznSlot(lLoadBlk, lCurrMgzn, lCurrSlot);
				m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot].Empty();

				if (bBcUpdate)
					m_bUseBarcode = TRUE;

				lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, lLoadBlk, pUpperGripperBuffer);
			
			} while (lLoadStatus == Err_BLLoadEmptyBarcodeFail);
		}

		if (lLoadStatus != TRUE)
		{
			SetErrorMessage("BL: OpDualBufferChangeGrade - bGoToLoad fail");
			return FALSE;
		}
	}


	//Pull frame from buffer to table (table has no frame)
	if (bBufferToTable || bDirectLoadToTable)
	{
		BOOL bStatus = TRUE;

		bStatus = UDB_LoadFromBufferToTable(m_bBurnInEnable, pUpperGripperBuffer, FALSE);
		if ( bStatus == FALSE )
		{
			SetErrorMessage("BL: OpDualBufferChangeGrade - bBufferToTable fail");
			return FALSE;
		}
	}

	// Exhange frames on buffer and bin table
	if (bGoToChange == TRUE)
	{
		BOOL bStatus = TRUE;

		//Set Magzine&Slot Number
		m_lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
		m_lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
		szLog.Format("BL: OpDualBufferChangeGrade CurrMgzn = %d, CurrSlot = %d", m_lCurrMgzn, m_lCurrSlot);	
		BL_DEBUGBOX(szLog);

		pLowerGripperBuffer->SetMgznSlotNo(m_lCurrMgzn, m_lCurrSlot);
		pLowerGripperBuffer->SetBinFull(FALSE);

		if ((bStatus = UDB_UnloadFromTableToBuffer(m_bBurnInEnable, FALSE, pLowerGripperBuffer, FALSE)) != FALSE)
		{
			bStatus = UDB_LoadFromBufferToTable(m_bBurnInEnable, pUpperGripperBuffer, FALSE);
		}

		if ( bStatus == FALSE )
		{
			SetErrorMessage("BL: OpDualBufferChangeGrade - bGoToChange fail");
			return FALSE;
		}
	}

	return TRUE;
}


INT CBinLoader::OpDualBufferUnloadLastGradeFrame()
{
	return TRUE;
}


INT CBinLoader::OpDualBufferPreloadEmptyFrame(LONG lLoadBlk)
{
	return TRUE;
}


INT CBinLoader::OpOutoutMgznSummary(BOOL bMagzineTOP2)
{
// 2018.7.17 remove m_bExChgFullMgznOutputFile variable because this variable is not defined in HMI
//	if (!m_bExChgFullMgznOutputFile)
//		return TRUE;

	LONG lMagNo = BL_MGZ_TOP_1;

	switch( m_lOMRT )
	{
	case BL_MODE_A:	
		OutputMagzFullSummaryFile(BL_MGZ_TOP_2, FALSE);
		break;

	case BL_MODE_B:
		OutputMagzFullSummaryFile(BL_MGZ_TOP_2, FALSE);	
		OutputMagzFullSummaryFile(BL_MGZ_MID_2, FALSE);	
		OutputMagzFullSummaryFile(BL_MGZ_BTM_2, FALSE);	
		break;

	case BL_MODE_C:
	case BL_MODE_D:
		OutputMagzFullSummaryFile(m_lCurrMgzn, FALSE);	
		break;

	case BL_MODE_E:		//v3.45
		OutputMagzFullSummaryFile(BL_MGZ_TOP_1, FALSE);
		break;

	case BL_MODE_F:		//MS100 8mag 150bins config		//v3.82
		if (bMagzineTOP2 || IsMSAutoLineStandloneMode())
		{
			lMagNo = BL_MGZ_TOP_2;
		}
		OutputMagzFullSummaryFile(lMagNo, FALSE);
		break;

	case BL_MODE_G:		//MS100 8mag 175bins config		//v3.82
		OutputMagzFullSummaryFile(BL_MGZ_TOP_2, FALSE);
		break;

	default:
		break;
	}

	return TRUE;
}



BOOL CBinLoader::UnloadFrameOnUpperGripper(BOOL bFull, BOOL bClearBin, BOOL bIsManualUnload)
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		if (m_clUpperGripperBuffer.IsBufferBlockExist())
		{
			if (UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, bFull, &m_clUpperGripperBuffer, bClearBin) == FALSE)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}


BOOL CBinLoader::UnloadFrameOnLowerGripper(BOOL bFull, BOOL bClearBin, BOOL bIsManualUnload)
{
	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		if (m_clLowerGripperBuffer.IsBufferBlockExist())
		{
			if (UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, bFull, &m_clLowerGripperBuffer, bClearBin) == FALSE)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}



//v4.42T17	
INT CBinLoader::OpUnloadFrameOnTable()			//v4.20		//JDSU buyoff request
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bCreeGo = FALSE;

	LogCycleStopState("BL Auto Unload Film Frame unload frame table");

	if (CMS896AApp::m_lCycleSpeedMode < 3)		//Does not support MS899
	{
		LogCycleStopState("unload table frame not for ms899");
		return FALSE;
	}


	if ( pApp->GetCustomerName() == _T("Cree")		||
		 pApp->GetCustomerName() == _T("NanoJoin")	||
		 pApp->m_bMSAutoLineMode == 1				||
		 CMS896AApp::m_bEnableSubBin)				//v4.57A15	
	{
		bCreeGo = TRUE;
	}

	if ( bCreeGo==FALSE )
	{
		if ( (pApp->GetCustomerName() != "JDSU") && (pApp->GetCustomerName() != CTM_NICHIA) )
		{
			return TRUE;
		}
		if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_BUFFER)
		{
			return TRUE;
		}
	}

	CString szLog;
	szLog.Format("OpUnloadFrameOnTable Bin #%ld after WaferEnd ....", GetCurrBinOnBT());
	BL_DEBUGBOX(szLog);

	if (GetCurrBinOnBT() <= 0)
		return TRUE;
	if (IsBurnIn())
		return TRUE;

	LONG lBHAtPick = 0;
	LONG lBHAtSafe = 0;
	INT i=0;

	do
	{
		Sleep(500);
		lBHAtPick = (*m_psmfSRam)["BondHead"]["AtPrePick"];
		lBHAtSafe = IsBHStopped();

		if (lBHAtPick == 1)
			break;
		if (lBHAtSafe == 1)
			break;
		i++;

	} while (i < 10);

	//v4.40T7
	if (pApp->GetCustomerName() == CTM_NICHIA 	&&
		(pApp->GetProductLine() == _T("")) )
	{
		if (!UnloadFrameOnUpperGripper(FALSE, TRUE, FALSE))
		{
			return  FALSE;
		}

		if (!UnloadFrameOnLowerGripper(FALSE, FALSE, FALSE))
		{
			return  FALSE;
		}

		LONG lStatus = 0;

		if ( (CheckIfNichiaWafIDListExceedLimit(GetCurrBinOnBT(), TRUE) == TRUE) ||
			 (CMS896AStn::m_oNichiaSubSystem.m_ucProcessMode == 1) )	//Single Mode
		{
			lStatus = UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, TRUE, TRUE);
		}
		else
		{
			lStatus = UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, FALSE, TRUE);
		}

		return lStatus;
	}


	if( bCreeGo==FALSE )
	{
		CString szContent	= "Continue to unload output frame on bin-table?";
		CString szTitle		= "Wafer-End";
		LONG lReturn = HmiMessageEx(szContent, szTitle, glHMI_MBX_CONTINUESTOP, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		if (lReturn != glHMI_CONTINUE)
		{
			BL_DEBUGBOX("OpUnloadFrameOnTable at WaferEnd aborted by user.");
			return TRUE;
		}
	}

	BL_DEBUGBOX("OpUnloadFrameOnTable at WaferEnd ...");

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)		//v4.57A15	//AUTOLINE
	{
		if (!UnloadFrameOnUpperGripper(FALSE, TRUE, FALSE))
		{
			return  FALSE;
		}
		if (!UnloadFrameOnLowerGripper(FALSE, FALSE, FALSE))
		{
			return  FALSE;
		}
		
	}

	return UnloadBinFrame(m_bBurnInTestRun, &m_clLowerGripperBuffer, FALSE, TRUE);
}


INT CBinLoader::OpExerciseGripperClamp()	//TongFang	//v4.21T4	
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//Make it available for ALL ctm except Cree; advised by Leo Lam
	/*if (pApp->GetCustomerName() != "Cree")	
	{
		for (INT i=0; i<2; i++)
		{
			SetGripperState(TRUE);
			Sleep(300);
			SetGripperState(FALSE);
			Sleep(300);
		}
	}*/
	return TRUE;
}


/////////////////////////////////////////////////////////////////
// Dual-Table Sequence Functions
/////////////////////////////////////////////////////////////////

INT CBinLoader::OpMoveBTToUnload(CONST LONG lBTInUse, BOOL bWait)
{
	if (lBTInUse == 1)	//BT2
	{
		if( MoveBinTable2(m_lBTUnloadPos_X2, m_lBTUnloadPos_Y2, 1, bWait) == FALSE )		//v4.22T7
		{
			return FALSE;
		}
	}
	else				//BT1
	{	
		if( MoveBinTable (m_lBTUnloadPos_X,  m_lBTUnloadPos_Y,  1, bWait) == FALSE )		//v4.22T7
		{
			return FALSE;
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////
// AUTOLINE Sequence Functions
/////////////////////////////////////////////////////////////////

BOOL CBinLoader::OpCheckAutoLineChangeGrade(CONST LONG lLoadBlk)
{
	if (!IsMSAutoLineMode())
		return TRUE;

	BOOL bUseEmpty = FALSE;
	LONG lFullStatus = GetLoadMgzSlot(bUseEmpty, lLoadBlk, FALSE, TRUE);	

	if ((lFullStatus == FALSE) || (bUseEmpty == TRUE))
	{
		return FALSE;	//WAIT for HOST ........
	}

	return TRUE;		//OK, continue to UNLOAD-LOAD
}

BOOL CBinLoader::OpCheckAutoLineBinFull(CONST LONG lLoadBlk)
{
	if (!IsMSAutoLineMode())
		return TRUE;

	BOOL bUseEmpty = FALSE;
	LONG lFullStatus = GetLoadMgzSlot(bUseEmpty, lLoadBlk, FALSE, TRUE);	

	if ((lFullStatus == FALSE) || (bUseEmpty == TRUE))
	{
		return FALSE;	//WAIT for HOST ........
	}

	return TRUE;		//OK, continue to UNLOAD-LOAD
}


/////////////////////////////////////////////////////////////////
// UD Buffer Sequence Functions
/////////////////////////////////////////////////////////////////

INT CBinLoader::OpUDBufferBinFull(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bGoToChange, BOOL bGoToUnload, LONG lLoadBlk)
{
	CString str;
	LONG lBufferBlock;

	CBinGripperBuffer *pUnloadGripperBuffer = DB_GetUnloadGripperBuffer();
	CBinGripperBuffer *pGripperBuffer = DB_GetPreloadGripperBuffer();
	lBufferBlock = pGripperBuffer->GetBufferBlock();

	BOOL bNeedUnload = FALSE;
	BOOL bGradeChange = FALSE;
	CString szBarCodeOnTable;
	ULONG ulGradeLeftOnMap = (*m_psmfSRam)["BinTable"]["GradeLeft"];

	if (lBufferBlock != 0)
	{
		//if the preload gripper(upper) is not the cuurrent bonding grade and the die number of current grade will be left on wafer
		//and need unload the frame to magazine and load a new empty frame
		if ((lBufferBlock != m_lBTCurrentBlock) && (ulGradeLeftOnMap > 0))
		{
			bNeedUnload = TRUE;
		}
		if (ulGradeLeftOnMap == 0)
		{
			bGradeChange = TRUE;
		}
	}

	str.Format("BIN-FULL (UDBuffer) Start (%d, %d, %d, %d, %d, Bin=%ld) *********************************", 
					bUnloadBuffer, bGoToLoad, bGoToChange, bGoToUnload, bNeedUnload, lLoadBlk);		//v4.34T9	//FOr debugging
	BL_DEBUGBOX(str);	

	//1. if there has a frame in load buffer, it need be unload firstly
	if ( bUnloadBuffer && bNeedUnload )
	{
		//Unload current frame on buffer with other grades
		if (UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, FALSE, pGripperBuffer) == FALSE)
		{
			CString szText = "\nBinLoader";
			SetStatusMessage("Binloader Unload buffer frame fails");
			SetErrorMessage("Binloader Unload buffer frame fails");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_UNLOAD_BUFFER_FRAME_FAIL, szText);
			return FALSE;
		}

		lBufferBlock = 0;		//Need to reset if UNLOAD is performed!!
	}

	//for Standlong, this frame will get from TOP(input) magazine(status is empty)
	//for inline, ignore GetMgznSlot
	LONG lCurrMgzn = 0, lCurrSlot = 0;
	GetMgznSlot(lLoadBlk, lCurrMgzn, lCurrSlot);

	//2. have pre-load a empty frame
	if (bGoToLoad && (lBufferBlock == 0))
	{
		str.Format("Set Mgzn #%d Slot #%d to act2Fu", lCurrMgzn, lCurrSlot);
		BL_DEBUGBOX(str);

		//Load new EMPTY frame with same grade
		LONG lFrameStatus = 0;
		if (!IsMSAutoLineMode())
		{
			szBarCodeOnTable	= m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot];
			lFrameStatus		= m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot];
			m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
		}

		LONG lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, lLoadBlk, pGripperBuffer);

		//** New DISCARD EMPTY method **//	
		if (lLoadStatus == Err_BLLoadEmptyBarcodeFail)	
		{
			BL_DEBUGBOX("BL: UDBuffer FULL Load EMPTY Discard retry");

			do 
			{
				BOOL bBcUpdate = FALSE;
				if (m_bUseBarcode)
				{
					m_bUseBarcode = FALSE;
					bBcUpdate = TRUE;
					
					//Temporarily set BC of current DISCARD frame to EMPTY
					m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = "EMPTY";
				}

				if ( UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, TRUE, pGripperBuffer) == FALSE )
				{
					if (!IsMSAutoLineMode())
					{
						m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot]	= lFrameStatus;
						m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot]	= szBarCodeOnTable;
					}

					CString szText = "\nBinLoader";
					SetStatusMessage("BL UDBuffer Load EMPTY frame fails 1");
					SetErrorMessage("BL UDBuffer Load EMPTY frame fails 1");
					SetAlert_Msg_Red_Yellow(IDS_BL_EX_LOAD_EMPTY_FRAME_FAIL, szText);

					if (bBcUpdate)
						m_bUseBarcode = TRUE;
					return FALSE;
				}

				if (!IsMSAutoLineMode())
				{
					m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
					m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot].Empty();
				}

				if (bBcUpdate)
					m_bUseBarcode = TRUE;

				lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, lLoadBlk, pGripperBuffer);
			
			} while (lLoadStatus == Err_BLLoadEmptyBarcodeFail);
		}

		if (lLoadStatus != TRUE)
		{
			if (!IsMSAutoLineMode())
			{
				m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot]	= lFrameStatus;
				m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot]	= szBarCodeOnTable;
			}

			CString szText = "\nBinLoader";
			SetStatusMessage("BL UDBuffer Load EMPTY frame fails");
			SetErrorMessage("BL UDBuffer Load EMPTY frame fails");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_LOAD_EMPTY_FRAME_FAIL, szText);

			str.Format("OpDualBufferBinFull - Generate special communication file(Block:%d)", lLoadBlk);
			CMSLogFileUtility::Instance()->BL_LogStatus(str);
			return FALSE;
		}

		m_bDualBufferPreLoadEmptyFrame = TRUE;
		m_szAutoPreLoadBarCode = m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot];
		if (!IsMSAutoLineMode())
		{
			m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot]	= lFrameStatus;
			m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot]	= szBarCodeOnTable;
		}
		CMSLogFileUtility::Instance()->BL_LogStatus("DBF bin full preload empty set to true");
	}

	
	/*if ( bGoToLoad && (lBufferBlock == 0) &&  bGoToUnload == TRUE )
	{
		LONG lMgzn, lSlot;
		if ( GetFullMgznSlot(lMgzn, lSlot) == FALSE )
		{
            return FALSE;
        }
		else
		{
			if ( MoveElevatorToUnload(lMgzn, lSlot, TRUE, FALSE) == FALSE )
			{
				return FALSE;
			}
		}
		CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("Elevator to Unload FULL Frame");
	}*/


	if ( bGoToChange == TRUE )
	{
		//Set Magzine&Slot Number
		m_lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
		m_lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
		pUnloadGripperBuffer->SetMgznSlotNo(m_lCurrMgzn, m_lCurrSlot);

		// Temp. set current frame status to ACTIVE2FULL such that ExArmChangeBinFrame will treat 
		// buffer frame as EMPTY frame
		LONG lFrameStatus = TRUE;
		if (!bGradeChange)
		{
			//Need to set current empty frame to FULL state such that to bypass bin realignment 
			// in ExArmChangeBinFrame() below
			if (!IsMSAutoLineMode())
			{
				lFrameStatus = m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot];
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
			}

			str.Format("Set Mgzn #%d Slot #%d to act2Fu", m_lCurrMgzn, m_lCurrSlot);
			BL_DEBUGBOX(str);
		}

		pUnloadGripperBuffer->SetBinFull(TRUE);
		if (UDB_UnloadFromTableToBuffer(m_bBurnInEnable, TRUE, pUnloadGripperBuffer, FALSE) == FALSE)
		{
			CString szText = "\nBuffer Table";
			SetStatusMessage("Binloader exchange FULL frame fails -> table to buffer");
			SetErrorMessage("Binloader exchange FULL frame fails -> table to buffer");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_CHG_FULL_FRAME, szText);

			m_bDualBufferPreLoadEmptyFrame = TRUE;  
			CMSLogFileUtility::Instance()->BL_LogStatus("DBF bin full alarm preload empty set to true");

			if (!IsMSAutoLineMode() && !bGradeChange)
			{
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = lFrameStatus;
			}
			return FALSE;
		}

		if (bGoToUnload)
		{
			/////////////////////////////////////////////////////
			// UDBuffer: AutoClearBin in AUTOBOND mode
			/////////////////////////////////////////////////////
	 		if (DualBufferAutoClearBin(pUnloadGripperBuffer) == FALSE)
			{
				CString szText = "\nBinLoader";
				SetStatusMessage("Binloader Dual Buffer Pre-clear-bin fails");
				SetErrorMessage("Binloader Dual Buffer Pre-clear-bin fails");
				SetAlert_Msg_Red_Yellow(IDS_BL_EX_PRE_CLEAR_BIN_FAIL, szText);
				return FALSE;
			}
		}

		(*m_psmfSRam)["BinLoader"]["DB"]["BinFull"] = TRUE;

		if (UDB_LoadFromBufferToTable(m_bBurnInEnable, pGripperBuffer, FALSE) == FALSE)
		{
			CString szText = "\nBufffer Table";
			SetStatusMessage("Binloader exchange FULL frame fails -> buffer to table");
			SetErrorMessage("Binloader exchange FULL frame fails -> buffer to table");
			
			SetAlert_Msg_Red_Yellow(IDS_BL_FRMAE_EXIST_ON_TABLE, szText);

			if (bGoToUnload)
			{
				HmiMessage_Red_Yellow("The FULL frame in buffer table lower-slot is cleared;\nplease remove frame manually and put it to FULL magazine slot.", szText);
			
				//Assign the next FULL mgzn slot for current FULL frame on buffer table, even though
				//  it is not yet pushed into FULL mgzn due to this error; need OP to manually unload to
				//  this FULL slot;
				if (!IsMSAutoLineMode())
				{
					LONG lFullNgzn=0, lFullSlot=0;
					if (GetFullMgznSlot(lFullNgzn, lFullSlot) == TRUE)
					{
						m_stMgznRT[lFullNgzn].m_lSlotBlock[lFullSlot] = lLoadBlk;
					}
				}

				m_clUpperGripperBuffer.InitBufferInfo();
			}

			if (!bGradeChange)
			{
				//Restore frame status for current BinTable EMPTY frame;
				//  the FULL frame on buffer table is assumed to be cleared already -> 
				//	need OP to unload this frame manually!
				if (!IsMSAutoLineMode())
				{
					m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= lFrameStatus;
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]	= szBarCodeOnTable;	
					SaveBarcodeData(lLoadBlk, szBarCodeOnTable, m_lCurrMgzn, m_lCurrSlot);
				}
			}

			return FALSE;
		}

		//Tell BT that current LAOD frame is for BINFULL	//FOr Nichia MS100+ with BT T only
		//CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("Ex-arm change frame");
	}

	m_bDualBufferPreLoadEmptyFrame = FALSE;

	//Safe to reset here because DBH already at PREPICK state at this moment
	(*m_psmfSRam)["BinLoader"]["PreLoadEmpty"] = FALSE;	

	//Make sure elevator motion is complete before exit
	Z_Sync();
	
 	TRY {
		SaveData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("BL File Exception in OpExArmBinFull");
	}
	END_CATCH
	
 	TRY {
		SaveMgznRTData();
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH

	return TRUE;
}


INT CBinLoader::OpUDBufferBinFull_NoPreload(BOOL bUnloadBuffer, BOOL bGoToLoad, BOOL bGoToChange, BOOL bGoToUnload, LONG lLoadBlk)
{
	CString str;
	LONG lBufferBlock;

	BL_DEBUGBOX("BIN-FULL (UDBuffer wo PRELOAD) Start *********************************");

	CBinGripperBuffer *pGripperBuffer = DB_GetPreloadGripperBuffer();
	lBufferBlock = pGripperBuffer->GetBufferBlock();

	BOOL bNeedUnload = FALSE;
	BOOL bGradeChange = FALSE;
	CString szBarCodeOnTable;
	ULONG ulGradeLeftOnMap = (*m_psmfSRam)["BinTable"]["GradeLeft"];

	if (lBufferBlock != 0)
	{
		if ((lBufferBlock != m_lBTCurrentBlock))
			bNeedUnload = TRUE;
		if (ulGradeLeftOnMap == 0)
			bGradeChange = TRUE;
	}


	if ( bUnloadBuffer && bNeedUnload )
	{
		BL_DEBUGBOX("BL: DBuffer BIN-FULL Unload current Buffer frame");	

		//Unload current frame on buffer with other grades
		if ( UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, FALSE, pGripperBuffer) == FALSE )	
		{
			CString szText = "\nBinLoader";
			SetStatusMessage("Binloader Unload buffer frame fails");
			SetErrorMessage("Binloader Unload buffer frame fails");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_UNLOAD_BUFFER_FRAME_FAIL, szText);
			return FALSE;
		}
		//CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("Unload buffer frame");
	}


	if ((m_lBTCurrentBlock != 0) && (bGoToUnload == TRUE))
	{
		BL_DEBUGBOX("BL: DBuffer BIN-FULL Unload FULL frame from TABLE to BUFFER");	
		//Set Magzine&Slot Number
		m_lCurrMgzn = (*m_psmfSRam)["BinLoader"]["CurrMgzn"];
		m_lCurrSlot = (*m_psmfSRam)["BinLoader"]["CurrSlot"];
		pGripperBuffer->SetMgznSlotNo(m_lCurrMgzn, m_lCurrSlot);
		pGripperBuffer->SetBinFull(TRUE);

		LONG lUnloadStatus = UDB_UnloadFromTableToBuffer(m_bBurnInEnable, TRUE, pGripperBuffer, FALSE);

		/////////////////////////////////////////////////////
		// UDBuffer: AutoClearBin in AUTOBOND mode
		/////////////////////////////////////////////////////
 		if (DualBufferAutoClearBin(pGripperBuffer) == FALSE)
		{
			CString szText = "\nBinLoader";
			SetStatusMessage("Binloader Dual Buffer Pre-clear-bin fails");
			SetErrorMessage("Binloader Dual Buffer Pre-clear-bin fails");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_PRE_CLEAR_BIN_FAIL, szText);
			return FALSE;
		}

		if (lUnloadStatus != FALSE)
		{
			BL_DEBUGBOX("BL: DBuffer BIN-FULL Unload FULL frame from BUFFER to MGZN");	
			lUnloadStatus = UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, TRUE, pGripperBuffer, FALSE);
		}

		if (lUnloadStatus == FALSE)
		{
/*
			CString szText = "\nBinLoader";
			SetStatusMessage("Binloader UDBuffer FULL table frame fails");
			SetErrorMessage("Binloader UDBuffer FULL table frame fails");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_UNLOAD_FULL_BUFFER_FRAME_FAIL, szText);
			HmiMessage_Red_Yellow("Please unload the FULL frame on buffer table manually into FULL magazine!");
		
			if (m_lBTCurrentBlock > 0)
			{
				BL_DEBUGBOX("BL: UDBuffer BIN-FULL Unload FULL frame fails but ClearBin ..");	
				
				LONG lMgzn=0, lSlot=0;
				LONG lFullStatus = GetFullMgznSlot(lMgzn, lSlot, TRUE);
				if (CreateOutputFileWithClearBin(lMgzn, lSlot, lFullStatus, m_lBTCurrentBlock, m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]))
				{
					//Reset barcode name to default
					BOOL bEnableCopyTempFile = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["Copy Temp File Only"]; //v4.44T6
					if ( !bEnableCopyTempFile )  //if not copy tempfile
					{
						SaveBarcodeData(m_lBTCurrentBlock, "", m_lCurrMgzn, m_lCurrSlot);
					}

					//Reset Slot status
					switch (m_lOMRT)
					{
					case BL_MODE_G:		//MS100 8mag 175bins config	
						m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_lBTCurrentBlock;
						m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot];
						m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
						m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
						break;

					//case BL_MODE_A:
					case BL_MODE_F:		//MS100 8mag config	
					default:
						if (!IsMSAutoLineMode())
						{
							m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_lBTCurrentBlock;
							m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot];
							m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
							m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot].Empty();
						}
						break;
					}
				}
				else
				{
					BL_DEBUGBOX("Clear bin - FAIL");
				}

				pGripperBuffer->SetBinFullOutputFileCreated(TRUE);
				m_lBTCurrentBlock = 0;
				m_szBinFrameBarcode = "";
			}
*/						
			return FALSE;
		}
	}


	try
	{
		SaveData();
	}
	catch(CFileException e)
	{
		BL_DEBUGBOX("BL File Exception in OpDualBufferBinFull_NoPreload");
	}

 	TRY {
		SaveMgznRTData();	
	} CATCH (CFileException, e)
	{
		BL_DEBUGBOX("Exception in SaveMgznRTData !!!");
	}
	END_CATCH


	//THen load EMPTY frame
	if (bGoToLoad)
	{
		LONG lLoadStatus = TRUE;

		BL_DEBUGBOX("BL: UDBuffer load EMPTY frame from Mgzn to BUFFER");	
		lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, lLoadBlk, pGripperBuffer);

		if (lLoadStatus == TRUE)
		{
			BL_DEBUGBOX("BL: UDBuffer load EMPTY frame from BUFFER to TABLE");	
			lLoadStatus = UDB_LoadFromBufferToTable(m_bBurnInEnable, pGripperBuffer, FALSE);
		}
		else
		{
			CString szText = "\nBinLoader";
			SetStatusMessage("Binloader direct-load EMPTY frame fail");
			SetErrorMessage("Binloader direct-load EMPTY frame fail");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_LOAD_EMPTY_FRAME_FAIL, szText);
			return FALSE;
		}
	}

	return TRUE;
}


INT CBinLoader::OpUDBufferUnloadLastGradeFrame()
{
	LONG lBufferBlock = 0;
	LONG lFrameToBeUnloaded = 0;
	BOOL bBufferLevel;

	CBinGripperBuffer *pGripperBuffer = NULL;
	if (m_bDualBufferPreloadLevel == BL_BUFFER_UPPER)
	{
		lBufferBlock = m_clUpperGripperBuffer.GetBufferBlock();
		lFrameToBeUnloaded = m_clLowerGripperBuffer.GetBufferBlock();		//v4.23T1
		bBufferLevel = BL_BUFFER_UPPER;
		pGripperBuffer = &m_clLowerGripperBuffer;
	}
	else
	{
        lBufferBlock = m_clLowerGripperBuffer.GetBufferBlock();
		lFrameToBeUnloaded = m_clUpperGripperBuffer.GetBufferBlock();		//v4.23T1
		bBufferLevel = BL_BUFFER_LOWER;
		pGripperBuffer = &m_clUpperGripperBuffer;
	}

	LONG lCurrMgzn = m_lCurrMgzn;
	LONG lCurrSlot = m_lCurrSlot;


	CString szLog;
	szLog.Format("UNLOAD Bin #%ld (UDBuffer) Start *********************************", lFrameToBeUnloaded);		//v4.23T1
	BL_DEBUGBOX(szLog);

	LONG lStatus = TRUE;
	lStatus = UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, FALSE, pGripperBuffer, TRUE);

	if ( lStatus == FALSE )	
	{
		//CMSLogFileUtility::Instance()->BL_LoadUnloadLog(2, m_lCurrMgzn, m_lCurrSlot, -1);
		m_lCurrMgzn	= lCurrMgzn;
		m_lCurrSlot	= lCurrSlot;
		SetErrorMessage("BL OpUDBufferUnloadLastGradeFrame - UNLOAD Last Frame fail");
		return FALSE;
	}


	//Wait BT Realign result here until done
	if (!m_bStop)
	{
		if (!RealignBinFrameRpy())
		{
			SetErrorMessage("BL: Realignbin frame fails 4!");
			HmiMessage_Red_Yellow("BL: Realignbin frame fails 4!");
			return FALSE;
		}
	}

	m_lCurrMgzn	= lCurrMgzn;
	m_lCurrSlot	= lCurrSlot;

	OpDBUPreMoveElevatorToNextLoad(m_lBTCurrentBlock);
	return TRUE;
}

INT CBinLoader::OpUDBufferUnloadFullFrame()
{
	LONG lStatus = 0;
	BOOL bDisablePreLoadEmpty = ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD);

	CBinGripperBuffer *pGripperBuffer = DB_GetUnloadGripperBuffer();
	LONG lBufferBlockToBeUnload = pGripperBuffer->GetBufferBlock();

	//Check it
	m_lCurrMgzn = pGripperBuffer->GetMgznNo();
	m_lCurrSlot = pGripperBuffer->GetSlotNo();

	if (bDisablePreLoadEmpty)
	{

	}
	else
	{
		lStatus = UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, TRUE, pGripperBuffer, TRUE);

		//UNLOAD-Frame fcn will set current frame to ACTIVE2FULL state
		if (lStatus == FALSE)	// To FULL mgzn for sure!!
		{
/*
			//** Actual FULL frame already cleared in ExArm_BINFULL_Q, so need to restore
			//** current frame data on BT anyway

			//UPdate BC only when true BIN-FULL with EMPYT frame on table; 
			// if BIN-FULL change-grade only, BC cannot be updated here !!!
			if (lBufferBlockToBeUnload == m_lBTCurrentBlock)
			{
				//Restore pre-load barcode as current new frame on table
				if (!IsMSAutoLineMode())
				{
					m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]	= m_szAutoPreLoadBarCode;
					m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE;
					SaveBarcodeData(m_lBTCurrentBlock, m_szAutoPreLoadBarCode, m_lCurrMgzn, m_lCurrSlot);	//v2.71
				}
			}
			else
			{
				//If BIN-FULL change-grade, still need to find the corresponding slot to reset 
				//	the slot status for the FULL frame just unloaded;
				LONG lMgzn = 0, lSlot = 0;
				if (!IsMSAutoLineMode())
				{
					GetMgznSlot(lBufferBlockToBeUnload, lMgzn, lSlot);
					m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE2FULL;
				}
				
				CString szLog;
				szLog.Format("UDB UNLOAD Full frame after ChagneGrade 1 reset Bin #%d Mgzn #%d Slot #%d to Act2F",
								lBufferBlockToBeUnload, lMgzn, lSlot);
				BL_DEBUGBOX(szLog);
			}

			m_szAutoPreLoadBarCode = "";

			CString szText = "\nBinLoader";
			SetStatusMessage("Binloader Unload FULL buffer frame fails");
			SetErrorMessage("Binloader Unload FULL buffer frame fails");
			SetAlert_Msg_Red_Yellow(IDS_BL_EX_UNLOAD_FULL_BUFFER_FRAME_FAIL, szText);
			HmiMessage_Red_Yellow("Please unload the FULL frame on buffer table manually into FULL magazine!");

			//Need to reset table index even though UNLOAD fails because bin count already cleared!
			m_lExArmBufferBlock		= 0;	//Buffer frame has to be cleared anyway!!
			m_clLowerGripperBuffer.SetBufferBarcode("");

			if (!IsMSAutoLineMode())
			{
				LONG lMgzn = m_lCurrMgzn, lSlot = m_lCurrSlot;
				LONG lFullStatus = GetFullMgznSlot(lMgzn, lSlot);
				if (lFullStatus)
				{
					m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = pGripperBuffer->GetBufferBlock();
					m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = pGripperBuffer->GetBufferBarcode();
				}
			}

			m_clUpperGripperBuffer.InitBufferInfo();
			m_clLowerGripperBuffer.InitBufferInfo();
*/
			return FALSE;
		}
	}


	if (!bDisablePreLoadEmpty)	//No need to restore PreLoad barcode if DisablePreLoad fcn is enabled
	{
		//UPdate BC only when true BIN-FULL with EMPYT frame on table; 
		// if BIN-FULL change-grade only, BC cannot be updated here !!!
		if (lBufferBlockToBeUnload == m_lBTCurrentBlock)		//v4.22T5
		{
			//Restore pre-load barcode as current new frame on table
			if (!IsMSAutoLineMode())
			{
				m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]	= m_szAutoPreLoadBarCode;
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE;
				SaveBarcodeData(m_lBTCurrentBlock, m_szAutoPreLoadBarCode, m_lCurrMgzn, m_lCurrSlot);
			}
		}
		else
		{
			//If BIN-FULL change-grade, still need to find the corresponding slot to reset 
			//	the slot status for the FULL frame just unloaded;
			if (!IsMSAutoLineMode())
			{
				LONG lMgzn=0, lSlot=0;
				GetMgznSlot(lBufferBlockToBeUnload, lMgzn, lSlot);
				m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE2FULL;
				
				CString szLog;
				szLog.Format("UDB UNLOAD FULL Frame after ChagneGrade 2 reset Bin #%d Mgzn #%d Slot #%d to Act2F",
								lBufferBlockToBeUnload, lMgzn, lSlot);
				BL_DEBUGBOX(szLog);
			}
		}
	}

	m_szAutoPreLoadBarCode = "";
	OpDBUPreMoveElevatorToNextLoad(m_lBTCurrentBlock);
	return TRUE;
}


BOOL CBinLoader::OpUDBufferUnloadFrame()
{
	CBinGripperBuffer *pGripperBuffer = DB_GetUnloadGripperBuffer();
	LONG lBufferBlockToBeUnload = pGripperBuffer->GetBufferBlock();

	if (!pGripperBuffer->IsBufferBlockExist())
	{
		return TRUE;
	}
	//Check it
	m_lCurrMgzn = pGripperBuffer->GetMgznNo();
	m_lCurrSlot = pGripperBuffer->GetSlotNo();

	LONG lStatus = UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, TRUE, pGripperBuffer, TRUE);

	//UNLOAD-Frame fcn will set current frame to ACTIVE2FULL state
	if (lStatus == FALSE)	// To FULL mgzn for sure!!
	{
/*
		CString szLog;
		szLog.Format("UDB UNLOAD frame (OpUDBufferUnloadFrame) reset Bin #%d Mgzn #%d Slot #%d to Act2F",
					lBufferBlockToBeUnload, m_lCurrMgzn, m_lCurrSlot);
		BL_DEBUGBOX(szLog);
	

		CString szText = "\nBinLoader";
		SetStatusMessage("Binloader Unload buffer frame fails");
		SetErrorMessage("Binloader Unload buffer frame fails");
		SetAlert_Msg_Red_Yellow(IDS_BL_EX_UNLOAD_FULL_BUFFER_FRAME_FAIL, szText);
		HmiMessage_Red_Yellow("Please unload the frame on buffer table manually into magazine!");
*/
		return FALSE;
	}

	return TRUE;
}


INT CBinLoader::OpUDBufferPreloadEmptyFrame(LONG lLoadBlk)
{
	CString szLog;
	CBinGripperBuffer *pGripperBuffer = DB_GetPreloadGripperBuffer();
	LONG lBufferBlock = pGripperBuffer->GetBufferBlock();
	CBinGripperBuffer *pUnloadGripperBuffer = DB_GetUnloadGripperBuffer();

	if (pUnloadGripperBuffer->IsBufferBlockExist())
	{
		if (!OpUDBufferUnloadFrame())
		{
			return FALSE;
		}
	}
	//** This flag must be set BEFORE PRELOAD operation so DBH sequence is able to stop picking last-die
	//** for DBuffer config only at the moment
	(*m_psmfSRam)["BinLoader"]["PreLoadEmpty"] = TRUE;	
	//CMSLogFileUtility::Instance()->BL_ChangeGradeLog(lBufferBlock,lLoadBlk);
	
	if (lBufferBlock != 0)
	{
		if (lBufferBlock != lLoadBlk)
		{
			if (UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, FALSE, pGripperBuffer) == FALSE)
			{
				SetErrorMessage("BL OpUDBufferPreloadEmptyFrame - UNLOAD Buffer fail");
				return FALSE;
			}
			//CMSLogFileUtility::Instance()->BL_LoadUnloadLog(2, m_lCurrMgzn, m_lCurrSlot, lBufferBlock);
		}
		else
		{
			szLog.Format("PRELOAD EMPTY(%ld) (UDBuffer) frame already available", lLoadBlk);
			BL_DEBUGBOX(szLog);
			return TRUE;
		}
	}


	// Temp. set current frame status to ACTIVE2FULL such that DB_LoadFromMgzToBuffer will get 
	// next/empty frame instead of current frame
	LONG lCurrMgzn=0, lCurrSlot=0;
	LONG lUFrameStatus = 0;
	CString szCurrBarCode;

	if (!IsMSAutoLineMode())
	{
		GetMgznSlot(lLoadBlk, lCurrMgzn, lCurrSlot);
		lUFrameStatus		= m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot];
		szCurrBarCode		= m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot];
		m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
		//CMSLogFileUtility::Instance()->BL_LoadUnloadLog(3, lCurrMgzn, lCurrSlot, lLoadBlk);
	}

	szLog.Format("PRELOAD EMPTY(%ld) (DBuffer) Start *********************************", lLoadBlk);
	BL_DEBUGBOX(szLog);

	LONG lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, lLoadBlk, pGripperBuffer);
	
	//** New EMPTY DISCARD method **//
	if (lLoadStatus == Err_BLLoadEmptyBarcodeFail)	
	{
		do {
			BL_DEBUGBOX("BL: Discard retry");	

			BOOL bBcUpdate = FALSE;
			if (m_bUseBarcode)
			{
				m_bUseBarcode = FALSE;
				bBcUpdate = TRUE;

				if (!IsMSAutoLineMode())
				{
					m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = "EMPTY";	
					SaveBarcodeData(lLoadBlk, "EMPTY", lCurrMgzn, lCurrSlot);
				}
			}

			if (UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, TRUE, pGripperBuffer) == FALSE)
			{
				CString szText = "\nBinLoader";
				SetStatusMessage("Binloader Pre-load empty frame fails 1");
				SetErrorMessage("Binloader Pre-load empty frame fails 1");
				SetAlert_Msg_Red_Yellow(IDS_BL_EX_PRELOAD_EMPTY_FRAME_FAIL, szText);
				
				if (!IsMSAutoLineMode())
				{
					m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = lUFrameStatus;
				}
				
				if (bBcUpdate)
					m_bUseBarcode = TRUE;
				
				if (!IsMSAutoLineMode())
				{
					m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = szCurrBarCode;		//Restore BC for curr frame on bintable before UNLOAD
					SaveBarcodeData(lLoadBlk, szCurrBarCode, lCurrMgzn, lCurrSlot);	
				}
				return FALSE;
			}

			if (!IsMSAutoLineMode())
			{
				m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot].Empty();
			}

			if (bBcUpdate)
				m_bUseBarcode = TRUE;
			lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, lLoadBlk, pGripperBuffer);
		
		} while (lLoadStatus == Err_BLLoadEmptyBarcodeFail);
	}
	
	
	if (lLoadStatus != TRUE)
	{
		CString szText = "\nBinLoader";
		SetStatusMessage("Binloader Pre-load empty frame fails");
		SetErrorMessage("Binloader Pre-load empty frame fails");
		SetAlert_Msg_Red_Yellow(IDS_BL_EX_PRELOAD_EMPTY_FRAME_FAIL, szText);

		if (!IsMSAutoLineMode())
		{
			m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = lUFrameStatus;
		
			//Must restore table frame BC even if pre-load EMPTY frame fails
			m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = szCurrBarCode;		//Restore BC for curr frame on bintable before UNLOAD
			SaveBarcodeData(lLoadBlk, szCurrBarCode, lCurrMgzn, lCurrSlot);	
		}
		return FALSE;
	}

	if (!IsMSAutoLineMode())
	{
		//Restore current frame status
		m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = lUFrameStatus;

		m_szAutoPreLoadBarCode = m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot];
		m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot] = szCurrBarCode;		//Restore BC for curr frame on bintable before UNLOAD
		SaveBarcodeData(lLoadBlk, szCurrBarCode, lCurrMgzn, lCurrSlot);		//v3.65		//Fatc bugfix
	}

	m_bDualBufferPreLoadEmptyFrame	= TRUE;
	CMSLogFileUtility::Instance()->BL_LogStatus("DBF preload empty frame TRUE");

	pGripperBuffer = DB_GetPreloadGripperBuffer();
	if (!UDB_DownElevatorToReady(pGripperBuffer, FALSE))
	{
		return FALSE;
	}

	return TRUE;
}


INT CBinLoader::OpUDBufferPreLoadNextFrame()
{
	LONG lBufferBlock;
	CString szLog;

	CBinGripperBuffer *pUnloadGripperBuffer = DB_GetUnloadGripperBuffer();

	if (pUnloadGripperBuffer->IsBufferBlockExist())
	{
		if (!OpUDBufferUnloadFrame())
		{
			return FALSE;
		}
	}

	CBinGripperBuffer *pGripperBuffer = DB_GetPreloadGripperBuffer();
	lBufferBlock = pGripperBuffer->GetBufferBlock();

	ULONG ulNextBlk = (*m_psmfSRam)["BinTable"]["BLA_Mode"]["Next Blk"];

	////////////////////////////////////////////////////////////////////
	// 1. Unload buffer frame if currently contain another GRADE frame
	////////////////////////////////////////////////////////////////////
	if (lBufferBlock != 0)
	{
		if (lBufferBlock != ulNextBlk)
		{
			if ( UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, FALSE, pGripperBuffer, FALSE) == FALSE )
			{
				SetErrorMessage("BL: OpUDBufferPreLoadNextFrame - UNLOAD buffer fail");
				return FALSE;
			}
			//CMSLogFileUtility::Instance()->BL_LoadUnloadLog(2, m_lCurrMgzn, m_lCurrSlot, lBufferBlock);
		}
		else
		{
			//if (lBufferBlock == m_lBTCurrentBlock)		//same grade is pre-loaded
			//	OpPreMoveElevatorToNextUnLoad(TRUE);		//Pre-move elevator to FULL
			//else
			//	OpPreMoveElevatorToNextUnLoad();
			szLog.Format("PRELOAD NEXT(%ld) already available", ulNextBlk);
			BL_DEBUGBOX(szLog);
			return TRUE;
		}
	}


	szLog.Format("PRELOAD NEXT(%ld) (UDBuffer) Start *********************************", ulNextBlk);
	BL_DEBUGBOX(szLog);

	LONG lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, (LONG)ulNextBlk, pGripperBuffer);

	//** New EMPTY DISCARD method **//
	//Also check EMPTY pre-load here in case Next-Grade frame is also from EMPTY magazine
	if (lLoadStatus == Err_BLLoadEmptyBarcodeFail)	
	{
		BL_DEBUGBOX("BL: Preload next grade discard retry");
		
		do 
		{
			if (UDB_UnloadFromBufferToMgzWithLock(m_bBurnInEnable, TRUE, pGripperBuffer) == FALSE)
			{
				SetErrorMessage("BL: OpUDBufferPreLoadNextFrame - UNLOAD buffer RETRY fail");
				return FALSE;
			}

			if (!IsMSAutoLineMode())
			{
				LONG lCurrMgzn=0, lCurrSlot=0;
				GetMgznSlot(ulNextBlk, lCurrMgzn, lCurrSlot);
				m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] = BL_SLOT_USAGE_ACTIVE2FULL;
				m_stMgznRT[lCurrMgzn].m_SlotBCName[lCurrSlot].Empty();
			}

			lLoadStatus = UDB_LoadFromMgzToBufferWithLock(m_bBurnInEnable, (LONG)ulNextBlk, pGripperBuffer);
		
		} while (lLoadStatus == Err_BLLoadEmptyBarcodeFail);
	}

	if (lLoadStatus != TRUE)
	{
		SetErrorMessage("BL: OpUDBufferPreLoadNextFrame - UNLOAD buffer RETRY aborted");
		return FALSE;
	}


	pGripperBuffer = DB_GetPreloadGripperBuffer();
	if (!UDB_DownElevatorToReady(pGripperBuffer, FALSE))
	{
		return FALSE;
	}

	return TRUE;
}

INT CBinLoader::OpUDBufferChangeGradeFrames()
{
	ULONG ulBlkInUse	= 0;
	ULONG ulNextBlk		= 0;
	LONG lPreloadBufferBlock = 0;
	LONG lFrameOnBT		= BL_FRAME_NOT_EXIST;
	BOOL bGoToUnload	= FALSE;
	BOOL bGoToLoad		= FALSE;
	BOOL bGoToChange	= FALSE;
	BOOL bUnloadBuffer	= FALSE;
	BOOL bDirectLoadToTable = FALSE;
	BOOL bBufferToTable = FALSE;
	CString szLog;

	CBinGripperBuffer *pUnloadGripperBuffer = DB_GetUnloadGripperBuffer();

	if (pUnloadGripperBuffer->IsBufferBlockExist())
	{
		if (!OpUDBufferUnloadFrame())
		{
			return FALSE;
		}
	}

	CBinGripperBuffer *pGripperBuffer = DB_GetPreloadGripperBuffer();
	lPreloadBufferBlock = pGripperBuffer->GetBufferBlock();

	szLog.Format("OpUDBufferChangeGradeFrames: PRELOAD-Lvl=%d, PRELOAD=%ld",  
					m_bDualBufferPreloadLevel, lPreloadBufferBlock);
	BL_DEBUGBOX(szLog);

	ulNextBlk	= (*m_psmfSRam)["BinTable"]["BLA_Mode"]["Next Blk"];
	ulBlkInUse	= (*m_psmfSRam)["BinTable"]["PhyBlkInUse"];
				
	if ( (m_bBurnInEnable == TRUE) || m_bDisableBL || m_bNoSensorCheck)
	{
		lFrameOnBT = BL_FRAME_ON_CENTER;
		if (m_lBTCurrentBlock == 0)
		{
			lFrameOnBT = BL_FRAME_NOT_EXIST;
		}
	}
	else
	{
		lFrameOnBT	= CheckFrameOnBinTable();			// check frame on bin table physically
	}


	if (lFrameOnBT == BL_FRAME_NOT_EXIST)
	{
		//No Frame is detected
		if ( m_lBTCurrentBlock == 0 )
		{
			bGoToUnload		= FALSE;
			bBufferToTable	= FALSE;
			bGoToChange		= FALSE;	//do not use buffer table to exchange frame

			if ( lPreloadBufferBlock == ulBlkInUse )	//frame on buffer is the next grade
			{
				bGoToLoad = FALSE;
				bBufferToTable = TRUE;
				bUnloadBuffer = FALSE;
			}
			else if ( lPreloadBufferBlock == 0 )	//No frame on buffer
			{
				//bGoToLoad = TRUE;
				bDirectLoadToTable = TRUE;
				bUnloadBuffer = FALSE;
				ulNextBlk = ulBlkInUse;
			}
			else	//Frame on buffer is not the next grade
			{
				//bGoToLoad = TRUE;
				bDirectLoadToTable = TRUE;
				bUnloadBuffer = TRUE;
				ulNextBlk = ulBlkInUse;
			}
		}
		else
		{
			szLog.Format("BL - No Frame exist but current block = %d", m_lBTCurrentBlock);
			SetErrorMessage(szLog);

			szLog.Format("\n%d", m_lBTCurrentBlock);
			SetAlert_Msg_Red_Yellow(IDS_BL_NOFRAME_EXIST_SYS, szLog);
			return FALSE;
		}
	}
	else
	{
		if ( m_lBTCurrentBlock == 0 )
		{
			SetErrorMessage("BL - Frame exist but current block = 0");
			szLog = "\n0";
			SetAlert_Msg_Red_Yellow(IDS_BL_FRAME_EXIST_SYS, szLog);
			return FALSE;
		}
		else
		{
			//////////////////////////////////////////
			// **** Normal Senario ****
			//////////////////////////////////////////
			if ( m_lBTCurrentBlock != ulBlkInUse )
			{
				bGoToChange	= TRUE;			//Need to exchange frames on table <-> Buffer
				bGoToUnload	= TRUE;			//Need to unload last-grade frame AFTER exchange

				if ( lPreloadBufferBlock == ulBlkInUse )	/*** frame in Preload buffer is the next grade ***/
				{
					//**** Normal Senario ****//
					bUnloadBuffer = FALSE;					//no need to unload current buffer frame,
					bGoToLoad = FALSE;						//no need load frame from mgzn to buffer
				}
				else if ( lPreloadBufferBlock == 0 )		// New frame not yet on buffer table
				{
					bUnloadBuffer = FALSE;					//no need to unload buffer frame,
					bGoToLoad = TRUE;						//But need to load next-grade to buffer
					ulNextBlk = ulBlkInUse;
				}
				else										//frame on buffer is NOT the next grade, so
				{
					bUnloadBuffer = TRUE;					//first need to unload buffer frame,
					bGoToLoad = TRUE;						//then load correct next grade to buffer before exchange
					ulNextBlk = ulBlkInUse;
				}
			}
		}
	}

	szLog.Format("OpUDBufferChangeGradeFrames: UNLOAD=%d, LOAD=%ld, BufToTable=%d, Chg=%d, DIRECTLoad=%d, BLK=%lu",			
					bUnloadBuffer, bGoToLoad, bBufferToTable, bGoToChange, bDirectLoadToTable, ulNextBlk);
	BL_DEBUGBOX(szLog);

	if ( OpDualBufferChangeGrade(bUnloadBuffer, bGoToLoad, bBufferToTable, bGoToChange, bDirectLoadToTable, (LONG)ulNextBlk) == FALSE )
	{
		SetErrorMessage("BL OpDualBufferChangeGrade - fail");
		BL_DEBUGBOX("Dual Buffer Change Grade Fail!");
		return FALSE;
	}


	m_bDLAChangeGrade = TRUE;			// Tell BH to increase delay for 1st cycle	//v3.10T3
	if (!bGoToChange)					
		SetBLReady(TRUE);				//Wait until bin realign result is back!!	//v2.72a5	//v2.78T2	//v2.82T4
	SetBLAOperate(FALSE);				//Make sure Chg-Grade event is cleared from BT
	
	//CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("End\n\n");

	if ( bGoToUnload == TRUE )
	{
		m_qSubOperation = UDB_UNLOAD_FRAME_Q;
		DisplaySequence("BL - UDB Change Grade ==> UNLOAD Grade Frame");
	}
	else
	{
		m_qSubOperation = WAIT_OPERATE_Q;
		DisplaySequence("BL - UDB Change Grade ==> Wait Operate");
		Z_Sync();

		OpDBUPreMoveElevatorToNextLoad(m_lBTCurrentBlock);	
		//Wait until Realign reault back if UNLOAD is not needed
		if (bGoToChange && !RealignBinFrameRpy())
		{
			HmiMessage_Red_Yellow("BL: Realignbin frame fails 1!");
			return FALSE;
		}
	}

	return TRUE;
}

//================================================================
// OpPreMoveElevatorToNextLoad()
//   Created-By  : Andrew Ng
//   Date        : 10/6/2007 12:05:45 PM
//   Description : 
//   Remarks     : 
//================================================================
INT CBinLoader::OpDBUPreMoveElevatorToNextLoad(CONST ULONG ulCurrBlk, BOOL bUseBT2)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg, svMsg;
	ULONG ulBlk = ulCurrBlk;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( m_bEnablePickAndPlace || pUtl->GetPrescanDummyMap() )
	{
		ulBlk = 0;
	}
	else
	{
		stMsg.InitMessage(sizeof(ULONG), &ulBlk);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, _T("GetNextGradeBlk"), stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 1000) == TRUE )		//1sec
			{
				m_comClient.ReadReplyForConvID(nConvID, svMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		svMsg.GetMsg(sizeof(ULONG), &ulBlk);
	}
	CString szLog;
	szLog.Format("OpPreMoveElevator To NextLoad: Grade=%lu, BT2=%d,NextGradeBlk,%d",  ulCurrBlk, bUseBT2,ulBlk);
	CMSLogFileUtility::Instance()->BL_LogStatus(szLog);

	LONG lCurrMgzn, lCurrSlot;
	CString szErr;

	if (ulBlk == 0)
	{
		CMSLogFileUtility::Instance()->BL_LogStatus("OpUDBPreMoveElevatorToNextLoad: zero blk detected!\n");
		return FALSE;
	}
	else if (ulBlk == 999)	
	{
		// curr blk no is "last" grade in grade-list, so no pre-move to NEXT is triggered
		CMSLogFileUtility::Instance()->BL_LogStatus("Pre-Move elevator: 999 blk detected!\n");
		if (GetMgznSlot(ulCurrBlk, lCurrMgzn, lCurrSlot))
		{
			if ( MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, TRUE, FALSE, bUseBT2) == FALSE )		//anichia001
			{
				HmiMessage_Red_Yellow("BL: pre-move elevator to last slot fails");
				return FALSE;
			}
		}
	}
	else if (ulBlk == ulCurrBlk)		//New Empty frame 
	{
		BOOL bDisablePreLoadEmpty = ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD);

		CString szTemp;
		szTemp.Format("3Pre-Move elevator to from frame #%d to empty #%d, Preloadempty %d\n", ulCurrBlk, ulBlk, bDisablePreLoadEmpty);
		CMSLogFileUtility::Instance()->BL_LogStatus(szTemp);

		if (bDisablePreLoadEmpty)
		{
			BOOL bGetMgzn = FALSE;
			if (IsMSAutoLineMode())
			{
				bGetMgzn = GetMgznSlot(ulCurrBlk, lCurrMgzn, lCurrSlot);
			}
			else
			{
				bGetMgzn = GetFullMgznSlot(lCurrMgzn, lCurrSlot, TRUE);
			}
			if (bGetMgzn)
			{
				if ( MoveElevatorToUnload(lCurrMgzn, lCurrSlot, TRUE, FALSE, FALSE, FALSE, bUseBT2) == FALSE )
				{
					CString szErr = "BL: pre-move elevator to next unload FULL slot fails";
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);
					return FALSE;
				}
			}
		}
		else
		{
			if (GetNextNullMgznSlot(lCurrMgzn, lCurrSlot))	
			{
				//Move to ready position at Upper Clamp position and 
				//avoid to hit limit at the TOP2 with slot 1
				if ( MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, TRUE, FALSE, bUseBT2) == FALSE )
				{
					szErr = "BL: pre-move elevator to next empty slot fails";
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);	
					return FALSE;
				}
			}
		}
	}
	else
	{
		CString szTemp;
		szTemp.Format("4Pre-Move elevator to from frame #%d to #%d\n", ulCurrBlk, ulBlk);
		CMSLogFileUtility::Instance()->BL_LogStatus(szTemp);

		// Move elevator to next LOAD posn
		if (GetMgznSlot(ulBlk, lCurrMgzn, lCurrSlot))
		{
			//v4.44T5	//Cree HuiZhou	//Long
			if (m_stMgznRT[lCurrMgzn].m_lSlotUsage[lCurrSlot] == BL_SLOT_USAGE_ACTIVE2FULL)
			{
				if (GetNextNullMgznSlot(lCurrMgzn, lCurrSlot))	
				{
					if (MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, FALSE, FALSE, bUseBT2) == FALSE )
					{
						szErr = "BL: pre-move elevator to next GRADE empty slot fails";
						HmiMessage_Red_Yellow(szErr);
						SetErrorMessage(szErr);	
						return FALSE;
					}
				}
			}
			else
			{
				if (MoveElevatorToNextLoad(lCurrMgzn, lCurrSlot, FALSE, FALSE, bUseBT2) == FALSE )
				{
					szErr = "BL: pre-move elevator to next slot fails";
					HmiMessage_Red_Yellow(szErr);
					SetErrorMessage(szErr);	
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}


