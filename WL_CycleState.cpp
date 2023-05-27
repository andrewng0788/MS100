/////////////////////////////////////////////////////////////////
// BH_CycleState.cpp : Cycle Operation State of the CBondHead class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Saturday, December 4, 2004
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
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "SC_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////
//	Main Operation State Functions
/////////////////////////////////////////////////////////////////

VOID CWaferLoader::AutoOperation()
{
	RunOperation();
}

VOID CWaferLoader::DemoOperation()
{
	RunOperation();
}

VOID CWaferLoader::RunOperation()
{
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

    if ((Command() == glSTOP_COMMAND) &&
		(Action() == glABORT_STOP))
	{
		DisplaySequence("WL - STOP");
		State(STOPPING_Q);
	}
	else		
	{
		try
		{
			if ( IsMotionCE() == TRUE )
			{
				DisplaySequence("WL - critical error ==> house keeping");
				m_qSubOperation = HOUSE_KEEPING_Q;
			}

			m_nLastError = gnOK;
			switch (m_qSubOperation)
			{
			case WAIT_LOADUNLOAD_Q:
				DisplaySequence("WL - Wait LoadUnload Loop");
				if (m_lSecsGemHostCommandErr > 0)
				{
					CString szMsg;
					if (m_lSecsGemHostCommandErr == HCACK_LOADER_BACK_GATE_NOT_OPENED)
					{
						szMsg = _T("HostCommand::Wafer Loader Back Gate Not Opened or Front Gate Not Closed");
						CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
						SetAlert_Msg_Red_Yellow(IDS_WL_BACK_GATE_NOT_OPENED, szMsg);		
						SetErrorMessage("HostCommand::Wafer Loader Back Gate Not Opened or Front Gate Not Closed");
					}
					else if (m_lSecsGemHostCommandErr == HCACK_LOADER_BACK_GATE_NOT_CLOSED)
					{
						szMsg = _T("HostCommand::Wafer Loader Back Gate Not Closed or Front Gate Not Opened");
						CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
						SetAlert_Msg_Red_Yellow(IDS_WL_BACK_GATE_NOT_CLOSED, szMsg);		
						SetErrorMessage("HostCommand::Wafer Loader Back Gate Not Closed or Front Gate Not Opened");
					}
					else if (m_lSecsGemHostCommandErr == HCACK_LOADER_BACK_GATE_OTHER_ERR)
					{
						szMsg = _T("HostCommand::Bin Loader Motion Error");
						CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
						SetAlert_Msg_Red_Yellow(IDS_WL_Z_MOVE_ERR, szMsg);		
						SetErrorMessage("HostCommand::Wafer Loader Motion Error");
					}
					m_lSecsGemHostCommandErr	= 0;
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else if (OpCheckIfFrameNeedToBeUnload())
				{
					m_qSubOperation = UNLOAD_FRAME_Q;
					DisplaySequence("WL - Wait LoadUnload ==> Unload frame");
					break;
				}
				if (OpCheckIfFrameNeedToBePreload())
				{
					m_qSubOperation = PRELOAD_FRAME_Q;
					DisplaySequence("WL - Wait LoadUnload ==> Preload frame");
					break;
				}
				if (m_bStop)
				{
					m_qSubOperation = HOUSE_KEEPING_Q;
					DisplaySequence("WL - Wait LoadUnload ==> House Keeping");
				}
				else if (m_bDisableWL)
				{
					//m_qSubOperation = WAIT_LOADUNLOAD_Q;
					//DisplaySequence("WL - Wait LoadUnload ==> WAIT_LOADUNLOAD_Q");
				}
				Sleep(100);
				break;

			case UNLOAD_FRAME_Q:
				if( OpUnloadLastFilmFrame()==FALSE )
				{
					DisplaySequence("WL - Unload last film frame fail ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					DisplaySequence("WL - Unload last film frame ==> wait loadunload");
					m_qSubOperation = WAIT_LOADUNLOAD_Q;
				}
				break;

			case PRELOAD_FRAME_Q:
				if( OpPreloadNextFilmFrame()==FALSE )
				{
					DisplaySequence("WL - preload next film frame fail ==> house keeping");
					m_qSubOperation = HOUSE_KEEPING_Q;
				}
				else
				{
					DisplaySequence("WL - preload next film frame ==> wait loadunload");
					m_qSubOperation = WAIT_LOADUNLOAD_Q;
				}
				break;

			case HOUSE_KEEPING_Q:
				DisplaySequence("WL - House Keeping Q");
				if (m_bIsMagazineFull && (!m_bUseDualTablesOption || m_bIsMagazine2Full) )
				{
				//	SetAlert_Red_Yellow(IDS_WL_MAGS_FULL);	//	auto cycle, house keeping, clear wafer lot data
				//	SetErrorMessage("ES101 AutoLoad WaferFrame - All magazines are full");
					CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
					if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
					{
						DisplaySequence("WL - House Keeping to clear wafer lot data as magazines full");
						IPC_CServiceMessage stMsg;

						// Get the reply for the encoder value
						int	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ClearWaferLotDataCmd", stMsg);
						while(1)
						{
							if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
							{
								m_comClient.ReadReplyForConvID(nConvID, stMsg);
								break;
							}
							else
							{
								Sleep(10);
							}	
						}
					}
				}	//	all magazines full done.
				SetAutoCycleLog(FALSE);
				m_bFrameOperationInAutoMode = FALSE;
				if (GetHouseKeepingToken("WaferLoaderStn") == TRUE)
				{
					SetHouseKeepingToken("WaferLoaderStn", FALSE);
				}
				else
				{
					Sleep(10);
					break;
				}
				StopCycle("WaferLoaderStn");
				LogCycleStopState("WFL - stop completely");
				State(STOPPING_Q);
				SetHouseKeepingToken("WaferLoaderStn", TRUE);
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

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			Result(gnNOTOK);

			StopCycle("WaferLoaderStn");
			State(STOPPING_Q);

			NeedReset(TRUE);
			SetMotionCE(TRUE, "Wafer Loader RunOperation Stop");
		}
	}

}


////////////////////////////////////////////
//	SubState Operations
////////////////////////////////////////////

BOOL CWaferLoader::OpCheckIfFrameNeedToBeUnload()
{
	if (IsWLManualMode())
		return FALSE;

	if ( IsESDualWT()==FALSE )
		return FALSE;
	if( m_bDisableWT2InAutoBondMode )	//	4.24TX 4
		return FALSE;
	if (m_lFrameToBeUnloaded > 0)
	{
		CString szLog;
		szLog.Format("Auto Cycle to unload frame for WT %d", m_lFrameToBeUnloaded);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		return TRUE;
	}
	return FALSE;
}


BOOL CWaferLoader::OpCheckIfFrameNeedToBePreload()
{
	if (IsWLManualMode())
		return FALSE;

	if ( IsESDualWT()==FALSE )
		return FALSE;
	if( m_bDisableWT2InAutoBondMode )	//	4.24TX 4
		return FALSE;
	if (m_lFrameToBePreloaded > 0)
	{
		CString szLog;
		szLog.Format("Auto Cycle to preload frame for WT %d", m_lFrameToBePreloaded);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		return TRUE;
	}
	return FALSE;
}


BOOL CWaferLoader::OpUnloadLastFilmFrame()
{
	BOOL bReturn = TRUE;
	CString szLogMsg	= "WL: none";
	BOOL bBurnIn		= IsBurnIn();

	SetES101PreloadFrameDone(FALSE);	//	4.24TX 4	// begin unload frame
	if (m_lFrameToBeUnloaded == 2)				//WT2 frame need to be unloaded
	{
		if ( UnloadFilmFrame_WT2(FALSE, bBurnIn) != TRUE )
		{
			szLogMsg = "WL unload frame WT2 fail";
			bReturn = FALSE;
		}
		else
		{
			if (m_bIsMagazine2Full == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("OpUnloadLastFilmFrame - FrameToBePreloaded = 2");

				szLogMsg = "WL Filmframe WT2 is unloaded";
				m_lFrameToBePreloaded = 2;		//0=none; 1=WT1; 2=WT2
			}
			else
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("OpUnloadLastFilmFrame - Magazine2 Full FrameToBePreloaded = 0");

				szLogMsg = "Magazine2 complete no preload";
				m_lFrameToBePreloaded = 0;		//0=none; 1=WT1; 2=WT2
			}
		}
	}
	else if (m_lFrameToBeUnloaded == 1)			//WT1 frame need to be unloaded
	{
		if ( UnloadFilmFrame(FALSE, bBurnIn) != TRUE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL unload frame fail");
			bReturn = FALSE;
		}
		else
		{
			if (m_bIsMagazineFull == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("OpUnloadLastFilmFrame - FrameToBePreloaded = 1");

				szLogMsg = "WL Filmframe WT1 is unloaded";
				m_lFrameToBePreloaded = 1;		//0=none; 1=WT1; 2=WT2
			}
			else
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("OpUnloadLastFilmFrame - Magazine Full FrameToBePreloaded = 0");

				szLogMsg = "Magazine1 complete no preload";
				m_lFrameToBePreloaded = 0;		//0=none; 1=WT1; 2=WT2
			}
		}
	}

	SetStatusMessage(szLogMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
	if( bReturn )
		m_lFrameToBeUnloaded = 0;	//0=none; 1=WT1; 2=WT2
	SaveData();

	return bReturn;
}


BOOL CWaferLoader::OpPreloadNextFilmFrame()
{
	BOOL bReturn = TRUE;
	CString szLogMsg	= "WL: none";
	LONG lReturn = TRUE;
	BOOL bIsPreLoadedFrame = FALSE;
	BOOL bBurnIn = IsBurnIn();

	if (m_lFrameToBePreloaded == 2)				//WT2 frame need to be preloaded
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("OpPreloadNextFilmFrame: PreLoader Wafer Table 2 Frame (2)");

		lReturn = LoadFilmFrame_WT2(FALSE, bBurnIn, TRUE, FALSE);

		if( lReturn != TRUE )
		{
			szLogMsg = "WL Filmframe WT2 fails to be preloaded";
			bReturn = FALSE;

			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

			if (lReturn == Err_MagazineFull)
			{
				bReturn = TRUE;
			}
		}
		else
		{
			bIsPreLoadedFrame = TRUE;
			szLogMsg = "WL Filmframe WT2 is preloaded";
		}
	}
	else if (m_lFrameToBePreloaded == 4)				//WT2 frame need to be preloaded
	{

		CMSLogFileUtility::Instance()->WL_LogStatus("OpPreloadNextFilmFrame: PreLoader Wafer Table 2 Frame (4)");

		lReturn = LoadFilmFrame_WT2(TRUE, bBurnIn, TRUE, FALSE);

		if( lReturn != TRUE )
		{
			szLogMsg = "WL Filmframe WT4 fails to be preloaded";
			bReturn = FALSE;

			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

			if (lReturn == Err_MagazineFull)
			{
				bReturn = TRUE;
			}
		}
		else
		{
			bIsPreLoadedFrame = TRUE;
			szLogMsg = "WL Filmframe WT4 is preloaded";
		}
	}
	else if (m_lFrameToBePreloaded == 1)		//WT1 frame need to be preloaded
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("OpPreloadNextFilmFrame: PreLoader Wafer Table Frame (1)");

		lReturn = LoadFilmFrame(FALSE, bBurnIn, TRUE, FALSE);

		if ( lReturn != TRUE )
		{
			szLogMsg = "WL Filmframe WT1 fails to be preloaded";
			bReturn = FALSE;

			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

			if (lReturn == Err_MagazineFull)
			{
				bReturn = TRUE;
			}
		}
		else
		{
			bIsPreLoadedFrame = TRUE;
			szLogMsg = "WL Filmframe WT1 is preloaded";
		}
	}
	else if (m_lFrameToBePreloaded == 3)		//WT1 frame need to be preloaded
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("OpPreloadNextFilmFrame: PreLoader Wafer Table Frame (3)");

		lReturn = LoadFilmFrame(TRUE, bBurnIn, TRUE, FALSE);

		if ( lReturn != TRUE )
		{
			szLogMsg = "WL Filmframe WT3 fails to be preloaded";
			bReturn = FALSE;

			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			
			if (lReturn == Err_MagazineFull)
			{
				bReturn = TRUE;
			}
		}
		else
		{
			bIsPreLoadedFrame = TRUE;
			szLogMsg = "WL Filmframe WT3 is preloaded";
		}
	}

	if( bReturn && bIsPreLoadedFrame)	//	4.24TX 4
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("OpPreloadNextFilmFrame: SetES101PreloadFrameDone");
		SetES101PreloadFrameDone(TRUE);	// preload successfully
	}

	m_bIsFramebePreloaded = bIsPreLoadedFrame;
	m_lFrameToBePreloaded = 0;	//0=none; 1=WT1; 2=WT2
	SaveData();

	return bReturn;
}



/////////////////////////////////////////////////////////////////
//	ENd of WL_CycleState.cpp
/////////////////////////////////////////////////////////////////

