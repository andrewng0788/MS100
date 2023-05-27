/////////////////////////////////////////////////////////////////
// BL_CycleState.cpp : Cycle Operation State of the CBinLoader class
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
#include "ms896a.h"
#include "SC_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CString g_aszBLState[20] =
	{	"Wait Operate",			// WAIT_OPERATE_Q
		"Change Grade",			// CHANGE_GRADE_Q
		"Bin Full",				// BIN_FULL_Q
		"House Keeping",		// HOUSE_KEEPING_Q
		"DBuffer Load Frame",	// DBUFFER_LOAD_FRAME_Q
		"DBuffer Unload Frame"	// DBUFFER_UNLOAD_FRAME_Q
		"DBuffer Change Grade",	// DBUFFER_CHANGE_GRADE_Q
		"DBuffer Bin Full",		// DBUFFER_BIN_FULL_Q
		"DBuffer Preload Empty"	// DBUFFER_PRELOAD_EMPTY_FRAME_Q
	};

VOID CBinLoader::AutoOperation()
{
	RunOperation();
}

VOID CBinLoader::DemoOperation()
{
	RunOperation();
}

VOID CBinLoader::RunOperation()
{
	LONG lMode, lFrameOnBT;
	ULONG ulBlkInUse;
//	ULONG ulNextBlk;
	LONG lStatus;

	BOOL bGoToUnload = FALSE;
	BOOL bGoToLoad = FALSE;
	BOOL bGoToChange = FALSE;
	BOOL bUnloadBuffer = FALSE;
	BOOL bDirectLoadToTable = FALSE;
	BOOL bBufferToTable = FALSE;
	BOOL bBinFull = FALSE;
	BOOL bRtCheckCoverSensor = FALSE;
	BOOL bDisablePreLoadEmpty = FALSE;		//v3.68T1
	CString szTemp;
	BOOL bNoMotionHouseKeeping = FALSE;
			
//	BOOL bBufferLevel;
	LONG lBTInUse = 0;			//0=BT1, 1=BT2
	LONG lCurrBTCurrBlk	= GetCurrBinOnBT();

	INT nLoadBinFrameStatus = 0;

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//Set burnin mode if application is burnin

	bDisablePreLoadEmpty = ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD);

    if ((Command() == glSTOP_COMMAND) &&
		(Action() == glABORT_STOP))
	{
		DisplaySequence("BL - STOP");
		State(STOPPING_Q);
	}
	else		
	{
		try
		{
			bRtCheckCoverSensor = pAppMod->GetFeatureStatus(MS896A_FUNC_SENSOR_BL_RT_COVERSENSOR_CHECK);	//v3.60T1

			if ( IsMotionCE() == TRUE )
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if ( (pApp->GetCustomerName() == "Cree") && (IsAllMotorsEnable() == FALSE) )	//v4.45T2
			{
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			if( IsBLBCRepeatCheckFail() )
			{
				HmiMessage_Red_Back("Bin Frame barcode repeated already, please unload it manually!");
				SetErrorMessage("Bin Frame barcode repeated already, please unload it manually!");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}
			
			if (m_bStopAllMotion == TRUE)
			{
				bNoMotionHouseKeeping = TRUE;
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			switch (m_qSubOperation)
			{
			case WAIT_OPERATE_Q:
				if ( m_bMotionFail == TRUE )
				{
					SetAlert_Red_Yellow(IDS_BL_MODULE_ERROR);
					SetErrorMessage("BL Module error");
					m_bIsBLIdleinABMode = TRUE;		//v4.42T7
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if ( m_bStop )
				{
					m_bIsBLIdleinABMode = TRUE;	
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if (m_lSecsGemHostCommandErr > 0)
				{
					CString szMsg;
					if (m_lSecsGemHostCommandErr == HCACK_LOADER_BACK_GATE_NOT_OPENED)
					{
						szMsg = _T("HostCommand::Bin Loader Back Gate Not Opened or Front Gate Not Closed");
						CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);
						SetAlert_Msg_Red_Yellow(IDS_BL_BACK_GATE_NOT_OPENED, szMsg);		
						SetErrorMessage("HostCommand::Bin Loader Back Gate Not Opened or Front Gate Not Closed");
					}
					else if (m_lSecsGemHostCommandErr == HCACK_LOADER_BACK_GATE_NOT_CLOSED)
					{
						szMsg = _T("HostCommand::Bin Loader Back Gate Not Closed or Front Gate Not Opened");
						CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);
						SetAlert_Msg_Red_Yellow(IDS_BL_BACK_GATE_NOT_CLOSED, szMsg);		
						SetErrorMessage("HostCommand::Bin Loader Back Gate Not Closed or Front Gate Not Opened");
					}
					else if (m_lSecsGemHostCommandErr == HCACK_LOADER_BACK_GATE_OTHER_ERR)
					{
						szMsg = _T("HostCommand::Bin Loader Motion Error");
						CMSLogFileUtility::Instance()->BL_LogStatus(szMsg);
						SetAlert_Msg_Red_Yellow(IDS_BL_Z_MOVE_ERR, szMsg);		
						SetErrorMessage("HostCommand::Bin Loader Motion Error");
					}
					m_lSecsGemHostCommandErr = 0;
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if ((m_lBinLoaderConfig == BL_CONFIG_DUAL_DL) && m_bBhAutoCleanCollet)		//v4.42T7
				{
					DisplaySequence("BL - Wait OP ACC");
					m_bIsBLIdleinABMode = TRUE;		
					Sleep(500);
					break;
				}
				//v4.43T9	//WH SanAn	//MS100PlusII
				else if (WaitBLOperate() || 
						 (CMS896AStn::m_bBTAskBLChangeGrade == TRUE) ||
						 (CMS896AStn::m_bBTAskBLBinFull == TRUE))			//** Normal BINFULL or CHANGE-GRADE event **//
				{
					if (m_bBhInBondState)
					{
						DisplaySequence("BL - Wait OP BH In Bond State");
						break;
					}

					//v4.26T1
					DisplaySequence("BL - Wait BL Operation is true");
					if (!OpNeedResetEmptyFullMgzn())
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					SetBLOperate(FALSE);
					m_bIsBLIdleinABMode = FALSE;

					if (CMS896AStn::m_bBTAskBLChangeGrade == TRUE)
						CMS896AStn::m_bBTAskBLChangeGrade = FALSE;
					if (CMS896AStn::m_bBTAskBLBinFull == TRUE)
						CMS896AStn::m_bBTAskBLBinFull = 2;

					DisplaySequence("BL - check BL model");
					lMode = (*m_psmfSRam)["BinTable"]["BL_Mode"];
					switch(lMode)
					{
					case 1:
						m_qSubOperation = BIN_FULL_Q;
						if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A45
						{
							CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("Start UDB_BIN_FULL");
							m_qSubOperation = UDB_BIN_FULL_Q;
						}
/*
						else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
						{
							CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("Start DBUFFER_BIN_FULL");
							m_qSubOperation = DBUFFER_BIN_FULL_Q;
						}
*/
						break;

					case 2:
					default:
						m_qSubOperation = CHANGE_GRADE_Q;
						if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A45
						{
							CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("Start UDB_CHANGE_GRADE");
							m_qSubOperation = UDB_CHANGE_GRADE_Q;
						}
/*
						else if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
						{
							CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("Start DBUFFER_CHANGE_GRADE");
							m_qSubOperation = DBUFFER_CHANGE_GRADE_Q;
						}
*/
						break;
					}
				}
				else if (WaitBLAOperate())			// PRELOAD NEXT-GRADE-FRAME event
				{
					if (CMS896AApp::m_bEnableSubBin == TRUE)
					{
						SetBLAOperate(FALSE);
						break;
					}

					DisplaySequence("BL - Wait BLA ok");
					m_bIsBLIdleinABMode = FALSE;
					m_qSubOperation = UDB_PRELOAD_FRAME_Q;
//					m_qSubOperation = DBUFFER_LOAD_FRAME_Q;			//Pre-load next frame
				}
				else if ( WaitBLPreOperate() )			//** PRELOAD EMPTY-FRAME event **//
				{
					if (CMS896AApp::m_bEnableSubBin == TRUE)
					{
						SetBLPreOperate(FALSE);
						break;
					}
					DisplaySequence("BL - Wait BL Pre OP ok");
					m_bIsBLIdleinABMode = FALSE;			
					if (!OpNeedResetEmptyFullMgzn())
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					m_qSubOperation = UDB_PRELOAD_EMPTY_Q;					//Pre-load new empty frame
					//m_qSubOperation = DBUFFER_PRELOAD_EMPTY_FRAME_Q;		//Pre-load new empty frame
				}
				else if (bRtCheckCoverSensor && !CheckElevatorCover(TRUE))
				{
					SetErrorMessage("BL: RT elevator cover check triggered to stop!");
					m_bMotionFail = TRUE;
					m_bIsBLIdleinABMode = TRUE;			
					m_bCoverSensorAbort = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					m_bIsBLIdleinABMode = TRUE;			
					DisplaySequence("BL - Wait OP looping");
				}
				break;

/*
			case DBUFFER_LOAD_FRAME_Q:		//Preload Next Grade Frame
				DisplaySequence("BL - ExArm Load Frame");

				if ( m_bStop )
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
				{
					if (OpDualBufferPreLoadNextFrame() == FALSE)
					{
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				SetBLAOperate(FALSE);
				m_qSubOperation = WAIT_OPERATE_Q;
				DisplaySequence("BL - Exarm Load Frame ==> Wait Operate");
				break;

			case DBUFFER_PRELOAD_EMPTY_FRAME_Q:
				DisplaySequence("BL - Ready To Full");

				if ( m_bStop )
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
				{
					ulBlkInUse	= (*m_psmfSRam)["BinTable"]["PhyBlkInUse"]; // physical block id

					if ( OpDualBufferPreloadEmptyFrame(ulBlkInUse) == FALSE )
					{
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					OpPreMoveElevatorToNextUnLoad(TRUE);
				}
				else
				{
					if ( OpMoveToFullMagzine() == FALSE )
					{
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				SetBLPreOperate(FALSE);
				m_qSubOperation = WAIT_OPERATE_Q;
				DisplaySequence("BL - Ready To Bin Full ==> Wait Operate");
				break;
*/

			case CHANGE_GRADE_Q:
				DisplaySequence("BL - Change Grade");
				BL_DEBUGBOX("BL - Change Grade");

			//	if( pApp->GetCustomerName()=="Cree" )	 check BH safe to change bin
				{
					LONG lLoopCounter = 0;
					LONG lBHAtPick = 0;
					while( 1 )
					{
						lBHAtPick = (*m_psmfSRam)["BondHead"]["AtPrePick"];
						if( lBHAtPick )
						{
							CMSLogFileUtility::Instance()->BL_LogStatus("BL in change grade q bh at prepick\n");
							break;
						}
						Sleep(100);
						lLoopCounter++;
						if( lLoopCounter>10 )
						{
							CMSLogFileUtility::Instance()->BL_LogStatus("BL in chagne grade q bh not at prepick but wait over 1 seconds\n");
							break;
						}
					}

					if ( IsMotionCE() == TRUE )
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						CMSLogFileUtility::Instance()->BL_LogStatus("BL in chagne grade q bh cirtical error\n");
						break;
					}
				}

				ulBlkInUse = (*m_psmfSRam)["BinTable"]["PhyBlkInUse"]; // physical block id

				if (IsMSAutoLineMode())
				{
					CString szLog;
					szLog.Format("CHANGE_GRADE_Q : ulBlkInUse = %d", ulBlkInUse);
					CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
/*
					if (CheckFrameOnBinTable() != BL_FRAME_NOT_EXIST)
					{
						if (IsCassetteSlotTransferAction(m_lCurrMgzn, m_lCurrSlot))
						{
							LONG ulBlkInUse1 = GetCassetteSlotGradeBlock(m_lCurrMgzn, m_lCurrSlot);
							szLog.Format("CHANGE_GRADE_Q : Slot(%d, %d), GetCassetteSlotGradeBlock = %d", m_lCurrMgzn, m_lCurrSlot, ulBlkInUse1);
							CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
							if (ulBlkInUse1 > 0)
							{
								ulBlkInUse = ulBlkInUse1;
							}
						}
						szLog.Format("CHANGE_GRADE_Q : new ulBlkInUse = %d, GetCurrBinOnBT = %d", ulBlkInUse, GetCurrBinOnBT());
						CMSLogFileUtility::Instance()->BL_LogStatus(szLog);
					}
*/
				}
				//v4.56A11
				/*if (!OpCheckAutoLineChangeGrade(ulBlkInUse))
				{
					if ( m_bStop )
					{
						DisplaySequence("BL - Wait AUTOLINE ChangeGrade to STOP");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}

					DisplaySequence("BL - Wait AUTOLINE in ChangeGrade ...");
					Sleep(100);
					break;
				}*/

				lFrameOnBT = CheckFrameOnBinTable();			// check frame on bin table physically
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
					if (GetCurrBinOnBT() == 0)
					{
						bGoToUnload = FALSE;
						bGoToLoad = TRUE;
					}
					else
					{
						//Error
						CString szString;
						szString.Format("\n%d", GetCurrBinOnBT());

						SetAlert_Msg_Red_Yellow(IDS_BL_NOFRAME_EXIST_SYS, szString);

						szString.Format("BL - No Frame exist but current block = %d", GetCurrBinOnBT());
						SetErrorMessage(szString);

						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}
				else
				{
					//Frame is detected
					if (GetCurrBinOnBT() == 0)
					{
						//Error		
						CString szString;
						szString = "\n0";

						SetAlert_Msg_Red_Yellow(IDS_BL_FRAME_EXIST_SYS, szString);
						SetErrorMessage("BL - Frame exist but current block = 0");

						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					else
					{
						if (GetCurrBinOnBT() != ulBlkInUse)
						{
							bGoToUnload = TRUE;
							bGoToLoad = TRUE;
						}
						else
						{
							bGoToUnload = FALSE;
							bGoToLoad = FALSE;
						}
					}
				}

				m_bUnloadDone = FALSE;

				if (bGoToUnload == TRUE) // there is a frame or burn in mode
				{
					// Unload the original frame
					if ( UnloadBinFrame(m_bBurnInEnable, &m_clLowerGripperBuffer, FALSE) == FALSE ) // 
					{
						CString szLog;
						szLog.Format("UnloadBinFrame Failure");
						CMSLogFileUtility::Instance()->BL_LogStatus(szLog);

						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					m_bUnloadDone = TRUE;
				}
				
				if (bGoToLoad == TRUE)
				{
					do{
						nLoadBinFrameStatus = LoadBinFrame(m_bBurnInEnable, &m_clUpperGripperBuffer, ulBlkInUse);
					}while (nLoadBinFrameStatus == BL_FRAME_WT_NO_BAR_CODE);
					
					if ((nLoadBinFrameStatus == FALSE) || 
						(nLoadBinFrameStatus == Err_BLLoadEmptyBarcodeFail))	//pllm
					{
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				//Trigger Bin Table
				SetBLReady(TRUE);

				if (m_bStop)
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					m_qSubOperation = WAIT_OPERATE_Q;
					DisplaySequence("BL - Change Grade ==> Wait Operate");
				}
				break;

/*
			case DBUFFER_CHANGE_GRADE_Q:
				DisplaySequence("BL - ExArm change Grade");

				if (OpDBufferChangeGradeFrames() == FALSE)	//v4.59A38
				{
					m_bMotionFail = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
				}		


				lPreloadBufferBlock = DB_PreloadBlock();
				ulNextBlk	= (*m_psmfSRam)["BinTable"]["BLA_Mode"]["Next Blk"];
				ulBlkInUse	= (*m_psmfSRam)["BinTable"]["PhyBlkInUse"]; // physical block id
				
				if (m_bNoSensorCheck)		//v3.03
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

						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
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

						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
					else
					{
						if ( GetCurrBinOnBT() != ulBlkInUse )
						{
							bGoToUnload	= TRUE;
							bGoToChange	= TRUE;

							if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
							{
								if ( lPreloadBufferBlock == ulBlkInUse )	//frame in Preload buffer is the next grade
								{
									bUnloadBuffer = FALSE;					//no need Preload uppper buffer frame
									bGoToLoad = FALSE;						//no need loading to buffer
								}
								else if ( lPreloadBufferBlock == 0 )	//no frame on buffer table
								{
									bUnloadBuffer = FALSE;				//no need to unload buffer frame
									bGoToLoad = TRUE;					//load next grade to buffer table
									ulNextBlk = ulBlkInUse;
								}
								else									//frame on buffer is NOT the next grade
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
					SetSuckingHead(TRUE);		//v4.05

					if ( OpDualBufferChangeGrade(bUnloadBuffer, bGoToLoad, bBufferToTable, bGoToChange, bDirectLoadToTable, (LONG)ulNextBlk) == FALSE )
					{
						SetSuckingHead(FALSE);	//v4.05

						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;	
					}

					SetSuckingHead(FALSE);		//v4.05
				}

				m_bDLAChangeGrade = TRUE;			// Tell BH to increase delay for 1st cycle	//v3.10T3
				if (!bGoToChange)					//v2.93T2
					SetBLReady(TRUE);				//Wait until bin realign result is back!!	//v2.72a5	//v2.78T2	//v2.82T4
				SetBLAOperate(FALSE);				//Make sure Chg-Grade event is cleared from BT
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
					Y_Sync();

					//Wait until Realign reault back if UNLOAD is not needed
					if (bGoToChange && !RealignBinFrameRpy())
					{
						HmiMessage_Red_Yellow("BL: Realignbin frame fails 1!");
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}

					OpPreMoveElevatorToNextLoad(GetCurrBinOnBT());		//v4.41T2
				}
				break;
	

			case DBUFFER_UNLOAD_FRAME_Q:	//Unload Last-Grade Frame
				DisplaySequence("BL - ExArm UnLoad Frame");
				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
				{
					if ( OpDualBufferUnloadLastGradeFrame() == FALSE )
					{
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				DisplaySequence("BL - Exarm Unload Frame ==> Wait Operate");
				m_qSubOperation = WAIT_OPERATE_Q;
				break;
*/

			case BIN_FULL_Q:
				DisplaySequence("BL - Bin Full");
				BL_DEBUGBOX("BL - Bin Full");

				//	if( pApp->GetCustomerName()=="Cree" ) check BH safe to change bin
				{
					LONG lLoopCounter = 0;
					LONG lBHAtPick = 0;
					while( 1 )
					{
						lBHAtPick = (*m_psmfSRam)["BondHead"]["AtPrePick"];
						if( lBHAtPick )
						{
							CMSLogFileUtility::Instance()->BL_LogStatus("BL in bin full q bh at prepick\n");
							break;
						}
						Sleep(100);
						lLoopCounter++;
						if( lLoopCounter>10 )
						{
							CMSLogFileUtility::Instance()->BL_LogStatus("BL in bin full q bh not at prepick but wait over 1 seconds\n");
							break;
						}
					}

					if ( IsMotionCE() == TRUE )
					{
						m_qSubOperation = HOUSE_KEEPING_Q;
						CMSLogFileUtility::Instance()->BL_LogStatus("BL in bin full q bh cirtical error\n");
						break;
					}
				}

				m_bUnloadDone = TRUE;
				ulBlkInUse = (*m_psmfSRam)["BinTable"]["PhyBlkInUse"]; // physical block id

				if (GetCurrBinOnBT() != 0)		//v4.56A11	//AutoLine
				{
					CMSLogFileUtility::Instance()->BL_LogStatus("BL BIN_FULL_Q : UnloadBinFrame\n");
					if ( UnloadBinFrame(m_bBurnInEnable, &m_clLowerGripperBuffer, TRUE) == FALSE )	// 
					{
						CMSLogFileUtility::Instance()->BL_LogStatus("BL BIN_FULL_Q : UnloadBinFrame Failure\n");
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}
				}

				//v4.56A11
				/*if (!OpCheckAutoLineBinFull(ulBlkInUse))
				{
					if ( m_bStop )
					{
						DisplaySequence("BL - Wait AUTOLINE BINFULL to STOP");
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;
					}

					DisplaySequence("BL - Wait AUTOLINE in BINFULL ...");
					Sleep(100);
					break;
				}*/

				do{
					nLoadBinFrameStatus = LoadBinFrame(m_bBurnInEnable, &m_clUpperGripperBuffer, ulBlkInUse);
				}while (nLoadBinFrameStatus == BL_FRAME_WT_NO_BAR_CODE);
				
				////4.55T09 Auto
				//do{
				//	nLoadBinFrameStatus = LoadBinFrame(m_bBurnInEnable, &m_clUpperGripperBuffer, ulBlkInUse);
				//}while (nLoadBinFrameStatus == BL_FRAME_WT_NO_BAR_CODE || nLoadBinFrameStatus ==  BL_EMPTY_FRAME_BC_CHECK_AGAIN);

				if ((nLoadBinFrameStatus == FALSE) || 
					(nLoadBinFrameStatus == Err_BLLoadEmptyBarcodeFail))	//pllm
				{
					m_bMotionFail = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}
				
				SetBLReady(TRUE);

				if (m_bStop)
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					m_qSubOperation = WAIT_OPERATE_Q;
					DisplaySequence("BL - Bin full ==> Wait Operate");
				}
				break;

/*
			case DBUFFER_BIN_FULL_Q:
				DisplaySequence("BL - ExArm Bin Full");

				ulBlkInUse	= (*m_psmfSRam)["BinTable"]["PhyBlkInUse"]; // physical block id

				bGoToLoad = TRUE;
				bGoToUnload = TRUE;
				bGoToChange	= TRUE;
				bUnloadBuffer = TRUE;

				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
				{
					SetSuckingHead(TRUE);	

					if (bDisablePreLoadEmpty)
					{
						lStatus = OpDualBufferBinFull_NoPreload(bUnloadBuffer, bGoToLoad, bGoToChange, bGoToUnload, (LONG)ulBlkInUse);
					}
					else
					{
						lStatus = OpDualBufferBinFull(bUnloadBuffer, bGoToLoad, bGoToChange, bGoToUnload, (LONG)ulBlkInUse);
					}
					
					if ( lStatus == FALSE )
					{
						SetSuckingHead(FALSE);	//v4.05
						m_bMotionFail = TRUE;
						m_qSubOperation = HOUSE_KEEPING_Q;
						break;	
					}

					SetSuckingHead(FALSE);		//v4.05
				}

				m_bDLAChangeGrade = TRUE;			// Tell BH to increase delay for 1st cycle	//v3.10T3
				
				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
				{
					//v4.23T1	//SetBLReady() is called within LoadFromBufferToTable();
					// if disable preload, direct load empty frame will occur
					if (bDisablePreLoadEmpty)
					{
						SetBLReady(TRUE);	
					}
					else if (m_lOMRT == BL_MODE_H)		//v4.34T2	//Yealy
					{
						SetBLReady(TRUE);		//Need to set here because all EMPTY frame is treated as GRADE frame in Mode-H
					}
				}

				SetBLPreOperate(FALSE);			
				m_nExArmReAlignBinConvID = 0;

				CMSLogFileUtility::Instance()->BL_LoadUnloadTimeLog("End\n\n");
				m_qSubOperation = DBUFFER_UNLOAD_FULL_FRAME_Q;
				break;
			
			case DBUFFER_UNLOAD_FULL_FRAME_Q:	//Unload FULL Frame
				DisplaySequence("BL - ExArm UnLoad FULL Frame");

				if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//bin table no frame
				{
					LONG lBufferBlockToBeUnload = 0;

					if (DB_PreloadLevel() == BL_BUFFER_UPPER)
					{
						bBufferLevel = BL_BUFFER_LOWER;
						lBufferBlockToBeUnload = m_clLowerGripperBuffer.GetBufferBlock();
					}
					else
					{
						bBufferLevel = BL_BUFFER_UPPER;
						lBufferBlockToBeUnload = m_clUpperGripperBuffer.GetBufferBlock();
					}

					if (bDisablePreLoadEmpty)	//v3.94
					{

					}
					else
					{
						//UNLOAD-Frame fcn will set current frame to ACTIVE2FULL state
						if ( DB_UnloadFromBufferToMgz(m_bBurnInEnable, TRUE, FALSE, FALSE, FALSE, bBufferLevel, FALSE, FALSE, TRUE) == FALSE )	//axyz23	// To FULL mgzn for sure!!	//aandrew789
						{
							//  Actual FULL frame already cleared in ExArm_BINFULL_Q, so need to restore
							//  current frame data on BT anyway

							//UPdate BC only when true BIN-FULL with EMPYT frame on table; 
							// if BIN-FULL change-grade only, BC cannot be updated here !!!
							if (lBufferBlockToBeUnload == GetCurrBinOnBT())		//v4.22T5
							{
								if (m_lOMRT != BL_MODE_H)	//v4.34T1	Yealy
								{
									//Restore pre-load barcode as current new frame on table
									m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]	= m_szAutoPreLoadBarCode;
									m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE;
									SaveBarcodeData(GetCurrBinOnBT(), m_szAutoPreLoadBarCode, m_lCurrMgzn, m_lCurrSlot);	//v2.71
								}
							}
							else
							{
								//v4.22T6
								//If BIN-FULL change-grade, still need to find the corresponding slot to reset 
								//	the slot status for the FULL frame just unloaded;
								LONG lMgzn=0, lSlot=0;
								GetMgznSlot(lBufferBlockToBeUnload, lMgzn, lSlot);
								m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE2FULL;
								//v4.41T7
								CString szLog;
								szLog.Format("Unload Full frame after ChagneGrade 1 reset Bin #%d Mgzn #%d Slot #%d to Act2F",
												lBufferBlockToBeUnload, lMgzn, lSlot);
								BL_DEBUGBOX(szLog);
							
							}
						
							m_szAutoPreLoadBarCode = "";	//v4.22T5

							CString szText = "\nBinLoader";
							SetStatusMessage("Binloader Unload FULL buffer frame fails");
							SetErrorMessage("Binloader Unload FULL buffer frame fails");
							SetAlert_Msg_Red_Yellow(IDS_BL_EX_UNLOAD_FULL_BUFFER_FRAME_FAIL, szText);
							HmiMessage_Red_Yellow("Please unload the FULL frame on buffer table manually into FULL magazine!");

							//Need to reset table index even though UNLOAD fails because bin count already cleared!
							m_lExArmBufferBlock		= 0;	//Buffer frame has to be cleared anyway!!
							m_clLowerGripperBuffer.SetBufferBarcode("");

							//v3.80
							LONG lMgzn, lSlot;
							LONG lFullStatus = GetFullMgznSlot(lMgzn, lSlot);
							if (lFullStatus)
							{
								if (bBufferLevel == BL_BUFFER_LOWER)
								{
									//UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, TRUE, FALSE, lFullStatus, FALSE, m_bBurnInEnable, BL_BUFFER_LOWER);
									m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_clLowerGripperBuffer.GetBufferBlock();
									m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = m_clLowerGripperBuffer.GetBufferBarcode();
								}
								else if (bBufferLevel == BL_BUFFER_UPPER)
								{
									//UpdateDualBufferUnloadFrameMagazineStatus(lMgzn, lSlot, TRUE, FALSE, lFullStatus, FALSE, m_bBurnInEnable, BL_BUFFER_UPPER);
									m_stMgznRT[lMgzn].m_lSlotBlock[lSlot] = m_clUpperGripperBuffer.GetBufferBlock();
									m_stMgznRT[lMgzn].m_SlotBCName[lSlot] = m_clUpperGripperBuffer.GetBufferBarcode();
								}
							}

							m_clUpperGripperBuffer.InitBufferInfo();
							m_clLowerGripperBuffer.InitBufferInfo();

							m_bMotionFail = TRUE;
							m_qSubOperation = HOUSE_KEEPING_Q;
							break;
						}
					}

					OpPreMoveElevatorToNextLoad(GetCurrBinOnBT());			//v4.41T2

					if (!bDisablePreLoadEmpty)	//No need to restore PreLoad barcode if DisablePreLoad fcn is enabled
					{
						//v4.40T5	//Nichia MS100+ with BT T
						BOOL bEnableBtT = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["EnableT"];
						if (bEnableBtT)
						{
							BL_DEBUGBOX("Recover & Use BT T barcode after UNLOAD FULL = " + m_szBtTBarCode);
							m_szAutoPreLoadBarCode	= m_szBtTBarCode;
						}

						//UPdate BC only when true BIN-FULL with EMPYT frame on table; 
						// if BIN-FULL change-grade only, BC cannot be updated here !!!
						if (lBufferBlockToBeUnload == GetCurrBinOnBT())		//v4.22T5
						{
							if (m_lOMRT != BL_MODE_H)	//v4.34T1	Yealy
							{
								//Restore pre-load barcode as current new frame on table
								m_stMgznRT[m_lCurrMgzn].m_SlotBCName[m_lCurrSlot]	= m_szAutoPreLoadBarCode;
								m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE;
								SaveBarcodeData(GetCurrBinOnBT(), m_szAutoPreLoadBarCode, m_lCurrMgzn, m_lCurrSlot);
							}
						}
						else
						{
							//v4.22T6
							//If BIN-FULL change-grade, still need to find the corresponding slot to reset 
							//	the slot status for the FULL frame just unloaded;
							LONG lMgzn=0, lSlot=0;
							GetMgznSlot(lBufferBlockToBeUnload, lMgzn, lSlot);
							m_stMgznRT[m_lCurrMgzn].m_lSlotUsage[m_lCurrSlot]	= BL_SLOT_USAGE_ACTIVE2FULL;
							//v4.41T7
							CString szLog;
							szLog.Format("Unload Full frame after ChagneGrade 2 reset Bin #%d Mgzn #%d Slot #%d to Act2F",
											lBufferBlockToBeUnload, lMgzn, lSlot);
							BL_DEBUGBOX(szLog);
						}
					}

					m_szAutoPreLoadBarCode = "";	//v4.22T5
				}

				//v4.34T1	//YEaly
				if (m_lOMRT == BL_MODE_H)
				{
					BOOL bEmpty = FALSE;
					GetLoadMgzSlot(bEmpty, GetCurrBinOnBT(), FALSE, TRUE);	//Update m_lCurrMgzn & m_lCurrSlot
				}

				DisplaySequence("BL - Exarm Unload Full Frame ==> Wait Operate");
				m_qSubOperation = WAIT_OPERATE_Q;
				break;
*/

			case UDB_PRELOAD_FRAME_Q:		//Preload Next Grade Frame
				DisplaySequence("BL - UDB Preload Frame");
				BL_DEBUGBOX("BL - UDB Preload Frame");

				if ( m_bStop )
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				if (OpUDBufferPreLoadNextFrame() == FALSE)
				{
					m_bMotionFail = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				SetBLAOperate(FALSE);
				m_qSubOperation = WAIT_OPERATE_Q;
				DisplaySequence("BL - UDB Preload Frame ==> Wait Operate");
				break;


			case UDB_PRELOAD_EMPTY_Q:
				DisplaySequence("BL - Preload EMPTY");
				BL_DEBUGBOX("BL - Preload EMPTY");

				if (m_bStop)
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				ulBlkInUse	= (*m_psmfSRam)["BinTable"]["PhyBlkInUse"];		// physical block id

				if ( OpUDBufferPreloadEmptyFrame(ulBlkInUse) == FALSE )
				{
					m_bMotionFail = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				SetBLPreOperate(FALSE);
				m_qSubOperation = WAIT_OPERATE_Q;
				DisplaySequence("BL - UDB Preload EMPTY ==> Wait Operate");
				break;

			case UDB_UNLOAD_FRAME_Q:
				DisplaySequence("BL - UDB UnLoad Frame");
				BL_DEBUGBOX("BL - UDB UnLoad Frame");

				if (OpUDBufferUnloadLastGradeFrame() == FALSE)
				{
					m_bMotionFail = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				DisplaySequence("BL - UDB Unload Frame ==> Wait Operate");
				m_qSubOperation = WAIT_OPERATE_Q;
				break;


			case UDB_UNLOAD_FULL_FRAME_Q:	
				DisplaySequence("BL - UDB UnLoad FULL Frame");
				BL_DEBUGBOX("BL - UDB UnLoad FULL Frame");
				if (OpUDBufferUnloadFullFrame() == FALSE)
				{
					m_bMotionFail = TRUE;
					m_qSubOperation = HOUSE_KEEPING_Q;
					break;
				}

				DisplaySequence("BL - UDB Unload Full Frame ==> Wait Operate");
				m_qSubOperation = WAIT_OPERATE_Q;
				break;


			case UDB_CHANGE_GRADE_Q:
				DisplaySequence("BL - UDB Change Grade");
				BL_DEBUGBOX("BL - UDB Change Grade");

				if (OpUDBufferChangeGradeFrames() == FALSE)
				{
					//m_bMotionFail = TRUE;
					//m_qSubOperation = HOUSE_KEEPING_Q;
				}		

				m_qSubOperation = WAIT_OPERATE_Q;		//andrewng //2020-1208
				break;


			case UDB_BIN_FULL_Q:
				DisplaySequence("BL - ExArm Bin Full");
				BL_DEBUGBOX("BL - ExArm Bin Full");

				ulBlkInUse	= (*m_psmfSRam)["BinTable"]["PhyBlkInUse"]; // physical block id
				bDisablePreLoadEmpty = ((CMS896AApp*)AfxGetApp())->GetFeatureStatus(MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD);

				bGoToLoad = TRUE;
				bGoToUnload = TRUE;
				bGoToChange	= TRUE;
				bUnloadBuffer = TRUE;

				if (bDisablePreLoadEmpty)
				{
					lStatus = OpUDBufferBinFull_NoPreload(bUnloadBuffer, bGoToLoad, bGoToChange, bGoToUnload, (LONG)ulBlkInUse);
				}
				else
				{
					lStatus = OpUDBufferBinFull(bUnloadBuffer, bGoToLoad, bGoToChange, bGoToUnload, (LONG)ulBlkInUse);
				}
				
				if ( lStatus == FALSE )
				{
					//andrewng //2020-1208
					//SetSuckingHead(FALSE);	//v4.05
					//m_bMotionFail = TRUE;
					//m_qSubOperation = HOUSE_KEEPING_Q;
					//break;	
				}

				if (bDisablePreLoadEmpty)
				{
					SetBLReady(TRUE);	
				}

				SetBLPreOperate(FALSE);			
				m_nExArmReAlignBinConvID = 0;
				m_bDLAChangeGrade = TRUE;		

				m_qSubOperation = UDB_UNLOAD_FULL_FRAME_Q;
				break;


			case HOUSE_KEEPING_Q:
				DisplaySequence("BL - House Keeping Q");

				SetAutoCycleLog(FALSE);
				if (GetHouseKeepingToken("BinloaderStn") == TRUE)
				{
					SetHouseKeepingToken("BinloaderStn", FALSE);
				}
				else
				{
					Sleep(10);
					break;
				}

				if (IsAllSorted() && CMS896AApp::m_bEnableSubBin == TRUE)
				{
					m_bFrameToBeUnloadAtIdleState = TRUE;
				}
				//Need to make sure REALIGN-BIN reply is back before exit
				//RealignBinFrameRpy();		//v2.72a5
				m_nExArmReAlignBinConvID = 0;			//v2.93T2
				BTFramePlatformDown();
				if (bNoMotionHouseKeeping == FALSE)
				{
					//v2.93T2
					if (m_bIsExArmGripperAtUpPosn)
					{
						if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)	//v3.70T4
						{
							//Temporarily used by dual-buffer to memorize gripper pre-load position status
							BinGripperMoveTo(m_lReadyPos_X);
						}
						else
						{
							SetGripperLevel(FALSE);
						}
						m_bIsExArmGripperAtUpPosn = FALSE;
					}

 					LogCycleStopState("BL - Save Data");

					TRY {
						SaveData();
					} CATCH (CFileException, e)
					{
						BL_DEBUGBOX("Exception in SaveData at HouseKeeping !!!");
					}
					END_CATCH

 					TRY {
						SaveMgznRTData();		//v4.06
					} CATCH (CFileException, e)
					{
						BL_DEBUGBOX("Exception in SaveMgznRTData at HouseKeeping !!!");
					}
					END_CATCH

					if ( m_bMotionFail == FALSE )
						OpHouseKeeping_Move();		// Home all motors
				}

				StopCycle("BinLoaderStn");
				LogCycleStopState("BL - stop completely");
				State(STOPPING_Q);

				SetHouseKeepingToken("BinloaderStn", TRUE);

				break;

			default:
				DisplaySequence("BL - Unknown");
				m_nLastError = glINVALID_STATE;
				break;
			}

			if (m_nLastError != gnAMS_OK)
			{
				if (!IsAbort())
				{
					SetError(m_nLastError);
				}

				Result(gnNOTOK);
				State(ERROR_Q);
			}
			else
			{
				m_qPreviousSubOperation = m_qSubOperation;
			}

			if ( m_bHeapCheck == TRUE )
			{
				// Check the heap
				INT nHeapStatus = _heapchk();
				if ( nHeapStatus != _HEAPOK )
				{				
					CString szMsg;
					szMsg.Format("BL - Heap corrupted (%d) before BL [%s]", nHeapStatus, 
								g_aszBLState[m_qSubOperation]);
					DisplayMessage(szMsg);
					
					LogCycleStopState(szMsg);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			Result(gnNOTOK);

			//State(ERROR_Q);
			State(STOPPING_Q);

			NeedReset(TRUE);
			StopCycle("BinLoaderStn");
			SetMotionCE(TRUE, "Bin Loader Run Operation Stop");
		}			
	}
}

