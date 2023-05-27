/////////////////////////////////////////////////////////////////
// ST_Command.cpp : HMI Registered Command of the CSafety class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
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
#include "MS896A_Constant.h"
#include "Safety.h"
#include "HmiDataManager.h"
#include "resource.h"
#include "PRSelect.h"

#ifdef ALLDIESORT	
	#include "AS899stn.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Registered HMI Command 
VOID CSafety::RegisterVariables()
{
	try
	{
		// Register WaferMap Commands 
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Test"),					&CSafety::TestWaferMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestBinMap"),			&CSafety::TestBinMap);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestPrbMap2"),			&CSafety::TestPrbMap2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestPrbMap3"),			&CSafety::TestPrbMap3);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestPrbMap4"),			&CSafety::TestPrbMap4);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("LogItems"),				&CSafety::LogItems);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SrvStopAutoAlign"),		&CSafety::SrvStopAutoAlign);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PressStopButton"),		&CSafety::PressStopButton);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PressStartButton"),		&CSafety::PressStartButton);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PressStandByButton"),	&CSafety::PressStandByButton);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SrvAlarmEvent"),		&CSafety::SrvAlarmEvent);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetAlarmLamp"),			&CSafety::SetAlarmLamp);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetAlarmLampTest"),		&CSafety::SetAlarmLampTest);
		
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PreOnSelectShowPrImage"),	&CSafety::PreOnSelectShowPrImage);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserSearchDie"),			&CSafety::UserSearchDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("HotKeySearchRefDie"),		&CSafety::HotKeySearchRefDie);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SelectCamera"),				&CSafety::SelectCamera);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("PrescanGoToSelectCamPreTask"),	&CSafety::PrescanGoToSelectCamPreTask);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SaveMachineData"),			&CSafety::SaveMachineData);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateAction"),				&CSafety::UpdateAction);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UserPRLoopTest"),			&CSafety::UserPRLoopTest);


		//RegVariable(_T("ST_lProgressBarStep"),	&m_lProgressBarStep);	
		//RegVariable(_T("ST_lProgressBarLimit"),	&m_lProgressBarLimit);	
		RegVariable(_T("ST_bCheckLANConnection"),	&m_bCheckLANConnection);
		// For Wafer Map
		RegVariable(_T("WT_ulMapCommandCode"),		&m_WaferMapWrapper.m_ulMapCommandCode);
		RegVariable(_T("WT_strMapCommand"),			&m_WaferMapWrapper.m_strMapCommand);
		RegVariable(_T("ST_bFirstBufferReady"),		&m_WaferMapWrapper.m_bFirstBufferReady);
		RegVariable(_T("ST_bSecondBufferReady"),	&m_WaferMapWrapper.m_bSecondBufferReady);
		RegVariable(_T("ST_lBHTThermalSetPoint"),	&m_lBHTThermalSetPoint);

		//v4.03		//PLLM REBEL bin map support
		RegVariable(_T("BT_ulMapCommandCode"),		&m_BinMapWrapper.m_ulMapCommandCode);
		RegVariable(_T("BT_strMapCommand"),			&m_BinMapWrapper.m_strMapCommand);
		RegVariable(_T("ST_bFirstBinBufferReady"),	&m_BinMapWrapper.m_bFirstBufferReady);	
		RegVariable(_T("ST_bSecondBinBufferReady"),	&m_BinMapWrapper.m_bSecondBufferReady);	

		//v3.30T1
		RegVariable(_T("WT_bStopGoToBlkDie"),		&m_bStopGoToBlkDie);
		RegVariable(_T("ST_bGeneralStopCycle"),		&CMS896AStn::m_bGeneralStopCycle);
		//v4.50A26
		RegVariable(_T("ST_ucMS100PrDisplayID"),	&CMS896AStn::m_ucMS100PrDisplayID);
		//NVC v0.02
		RegVariable(_T("ST_bMotionCycleStop"),		&CMS896AStn::m_bMotionCycleStop);

		DisplayMessage("SafetyStn Service Registered...");

	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

LONG CSafety::TestWaferMap(IPC_CServiceMessage& svMsg)
{
	char* pMessage = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pMessage);
	m_WaferMapWrapper.Execute(pMessage);
	delete[] pMessage;
	return 0;
}

LONG CSafety::TestBinMap(IPC_CServiceMessage& svMsg)
{
	char* pMessage = new char[svMsg.GetMsgLen()];
	svMsg.GetMsg(svMsg.GetMsgLen(), pMessage);
	m_BinMapWrapper.Execute(pMessage);
	delete[] pMessage;
	return 0;
}

LONG CSafety::TestPrbMap2(IPC_CServiceMessage& svMsg)
{
	return 0;
}

LONG CSafety::TestPrbMap3(IPC_CServiceMessage& svMsg)
{
	return 0;
}

LONG CSafety::TestPrbMap4(IPC_CServiceMessage& svMsg)
{
	return 0;
}


LONG CSafety::SrvAlarmEvent(IPC_CServiceMessage& svMsg)
{
	return 0;
}

// for stop button only
LONG CSafety::PressStopButton(IPC_CServiceMessage& svMsg)
{
	CMS896AApp::m_bStopAlign = TRUE;	//v4.50A22	//Osram Germany
	//m_bStopByCommand = TRUE;
	CString szMsg;
	szMsg = "Stop button pressed by operator via HMI.";
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	LogCycleStopState(szMsg);

    CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
    LONG lAlarmOrgStatus = GetAlarmLamp_Status();
    BOOL bNeedToLog            = FALSE;
    if ((pApp->GetCustomerName() == "ChangeLight(XM)") && (lAlarmOrgStatus != ALARM_YELLOW))
    {
            bNeedToLog = TRUE;
    }
       
    SetAlarmLamp_Yellow(FALSE, FALSE);

    if ((pApp->GetCustomerName() == "ChangeLight(XM)") && (bNeedToLog == TRUE))
    {
            CString szAlarmCode = _T("Stop Pressed");
            AlarmLampStatusChangeLog(lAlarmOrgStatus, ALARM_YELLOW,szAlarmCode);
    }

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

//4.53D42
LONG CSafety::PressStartButton(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CString szMsg;
	szMsg = "Start button pressed by operator via HMI.";
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->LogCycleStartStatus(szMsg);

	if (pApp->m_bCycleStarted == TRUE)
	{
		bReturn = FALSE;
	}

	if( pApp->m_bAWET_Enabled )
	{
		if( (pApp->m_lAWET_EngineerFlowState>0 && pApp->m_lAWET_EngineerFlowState<3) || pApp->m_bAWET_Triggered )
		{
			HmiMessage_Red_Back("Engineer Does not fix alarm.\n" + pApp->m_szAWET_AlarmCode);
			bReturn = FALSE;
		}
	}

	LONG lAlarmOrgStatus = GetAlarmLamp_Status();
	BOOL bNeedToLog		 = FALSE;
	if ((pApp->GetCustomerName() == "ChangeLight(XM)") && (lAlarmOrgStatus != ALARM_GREEN))
	{
		bNeedToLog = TRUE;
	}
	
	SetAlarmLamp_Green(FALSE, FALSE);

	if ((pApp->GetCustomerName() == "ChangeLight(XM)") && (bNeedToLog == TRUE))
	{
		CString szAlarmCode = _T("Start Pressed");
		AlarmLampStatusChangeLog(lAlarmOrgStatus, ALARM_GREEN,szAlarmCode);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}
//1.08S
LONG CSafety::PressStandByButton(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CString szMsg;
	szMsg = "StandBy button pressed by operator via HMI.";
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	CMSLogFileUtility::Instance()->LogCycleStartStatus(szMsg);

	//1.08S
	m_lEquipmentStatus = glSG_STANDBY;
	SetEquipState(m_lEquipmentStatus);

	theApp.m_bMS_StandByButtonPressed_HMI = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSafety::SetAlarmLamp(IPC_CServiceMessage& svMsg)
{
	LONG lLevel;
	svMsg.GetMsg(sizeof(LONG), &lLevel);
	
	SetAlarmLog("Hmi Alarm Lamp");
	SetAlarmLamp_Status(lLevel, FALSE, TRUE);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CSafety::SetAlarmLampTest(IPC_CServiceMessage& svMsg)
{
	LONG lLevel;
	svMsg.GetMsg(sizeof(LONG), &lLevel);

	BOOL bOldState = m_bEnableAlarmLog;
	m_bEnableAlarmLog = FALSE;

	SetAlarmLamp_Status(lLevel, FALSE, TRUE);
	SetAlarmLog("Hmi alarm testing");

	m_bEnableAlarmLog = bOldState;

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;
}

LONG CSafety::PreOnSelectShowPrImage(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT nConvID;

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "PreOnSelectShowPrImage", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	BOOL bIsBondPrEnabled = (BOOL)(LONG) (*m_psmfSRam)["BondPrStn"]["Enabled"];

	if (bIsBondPrEnabled == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}
		
	nConvID = m_comClient.SendRequest(BOND_PR_STN, "PreOnSelectShowPrImage", stMsg);

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSafety::UserSearchDie(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bReturn=TRUE;

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchUpLookDieCmd", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
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
	if( bReturn==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if ( (LONG)(*m_psmfSRam)["MS896A"]["Current Camera"] == 0 )
	{
		BOOL bF1SearchWithRotate = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["F1 Search With Rotate"];
		
		if (bF1SearchWithRotate == FALSE)
		{
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchDie", stMsg);
		}
		else
		{
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchDie_Rotate", stMsg);
		}
	}
	else
	{
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "UserSearchDie", stMsg);
	}


	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSafety::HotKeySearchRefDie(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bReturn=TRUE;


	if ( (LONG)(*m_psmfSRam)["MS896A"]["Current Camera"] == 0 )
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "HotKeySearchRefDie", stMsg);
	}
	else
	{
		nConvID = m_comClient.SendRequest(BOND_PR_STN, "HotKeySearchRefDie", stMsg);
	}

	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CSafety::SelectCamera(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	int nConvID		= 0;
	BOOL bReturn	= TRUE;

#ifdef ALLDIESORT

	nConvID = m_comClient.SendRequest("ASWaferPrStn", "ToggleCamera", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

#else

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectCamera", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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

#endif

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSafety::PrescanGoToSelectCamPreTask(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	ULONG ulPosition;
	INT nConvID = 0;
	BOOL bReturn = TRUE;

	svMsg.GetMsg(sizeof(ULONG), &ulPosition);

	if (ulPosition == 0)
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SelectWaferCamera", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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
	}
	else
	{
		// switch tooling stage to bond cam and turn on bond cam
		UCHAR ucToolingStage = MS_TOOLINGSTAGE_CAM;
		stMsg.InitMessage(sizeof(UCHAR), &ucToolingStage);

		INT nConvID = m_comClient.SendRequest(BOND_PR_STN, "SelectToolingStage", stMsg);

		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
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
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSafety::SaveMachineData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CSafety::UpdateAction(IPC_CServiceMessage& svMsg)
{
	Sleep(500);
	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CSafety::LogItems(IPC_CServiceMessage& svMsg)
{
	LONG lEventNo = 0;
	svMsg.GetMsg(sizeof(LONG), &lEventNo);

	LogItems(lEventNo);
	return 1;
}

LONG CSafety::SrvStopAutoAlign(IPC_CServiceMessage& svMsg)
{
	BOOL bStop = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bStop);
	CMS896AApp::m_bStopAlign = bStop;

	CString szMsg;
	szMsg.Format("Stop align button pressed to stop is %d", bStop);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	SaveScanTimeEvent(szMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	426TX	2

/*
LONG CSafety::UserPRLoopTest(IPC_CServiceMessage& svMsg)
{
	IPC_CServiceMessage stMsg;
	PRSelect dlg;
	dlg.DoModal();
	CString szChannel = "";
	CString szType = "";
	LONG lCount = 0, lDelay = 0, lLoop = 0, lID = 0;
	dlg.GetPara(szChannel, szType, lDelay, lCount, lID);
	CString szGet;
	szGet.Format("Channel,%s,Type,%s,Count,%d,Delay,%d,ID,%d",szChannel,szType,lDelay,lCount,lID);
	HmiMessage(szGet);
	int nConvID = 0;
	BOOL bReturn = TRUE;
	while(1)
	{
		if (lCount == 0)
		{
			AfxMessageBox("break");
			break;
		}
		if (szChannel == "Wafer")
		{
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchDie_NoMove", stMsg);
		}
		else if (szChannel == "Bond")
		{
			nConvID = m_comClient.SendRequest(BOND_PR_STN, "UserSearchDie_NoMove", stMsg);
		}
		else if (szChannel == "Uplook")
		{

		}
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		Sleep(lDelay);
		lLoop ++;
		if (lLoop > lCount)
		{
			break;
		}
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/


LONG CSafety::UserPRLoopTest(IPC_CServiceMessage& svMsg)
{
      typedef struct 
      {
            BOOL  bStatus;
            double      dOffsetX;
            double      dOffsetY;
            DOUBLE      dAngle;
      } SRCH_RESULT;
      SRCH_RESULT       stResult;
      LONG lSet = 0;
      BOOL bReturn = TRUE;
      CStringList szList;
      szList.AddTail("Wafer Camera");
      szList.AddTail("Bond Camera");
      szList.AddTail("Up1");
	  szList.AddTail("BH Mark1");
	  szList.AddTail("BH Mark2");
	  szList.AddTail("BH ColletHole1");
	  szList.AddTail("BH ColletHole2");
	  szList.AddTail("Bond Camera(Object)");
 //     szList.AddTail("Up2");
//      szList.AddTail("Bond Camera(Object)");
      CString szContent = "Please select Camera";
      CString szTitle =  "PR Loop Test";
      CString szLoop,szLog;
      CStringArray szaAngle; szaAngle.RemoveAll();
      DOUBLE dMax1 = 0,dMax2 = 0,dMin1 = 0,dMin2 = 0,dSD1 = 0,dSD2 = 0, dAngleMax = 0, dAngleMin = 0, dAngleSD = 0;
      LONG lCount = 1, lLoop = 0,lNorNo;
	  BOOL bGet = TRUE;

      IPC_CServiceMessage stMsg;
	  int nConvID = 0;

      LONG lCameraSelection = HmiSelection(szContent, szTitle, szList, lSet);
	  if (lCameraSelection == 0)
	  {
		szList.RemoveAll();
		szList.AddTail("1");
		szList.AddTail("2");
		szList.AddTail("3");
		LONG lSet = 0;
		CString szContent = "Please Select Normal Die No";
		CString szTitle =  "WPR";
		lNorNo = HmiSelection(szContent, szTitle, szList, lSet);
	  }
	  else if ((lCameraSelection == 3) || (lCameraSelection == 5))//Mark1
	  {
		  nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_TnZToPick1", stMsg);
		  while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bGet);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
	  }
	  else if ((lCameraSelection == 4) || (lCameraSelection == 6)) //Mark2
	  {
		  nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_TnZToPick2", stMsg);
		  while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bGet);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
	  }

      szTitle = "Please input loop count";
      HmiStrInputKeyboard(szTitle, szLoop);
      lLoop = atoi(LPCTSTR(szLoop));

	  CMSLogFileUtility::Instance()->MissingDieThreshold_LogOpen(FALSE);
	  if (lCameraSelection == 7)
	  {
			CString szThreshold;
			szTitle = "Please input threshold";
			HmiStrInputKeyboard(szTitle, szThreshold);
			LONG lThreshold = atoi(LPCTSTR(szThreshold));
			(*m_psmfSRam)["BondPr"]["BlockThreshold"] = lThreshold;
	  }
      while(1)
      {
            if (lCameraSelection == 0)
            {
				stMsg.InitMessage(sizeof(LONG), &lNorNo);
                nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchNormalDie_NoMove", stMsg);
            }
            else if (lCameraSelection == 1)
            {
                  nConvID = m_comClient.SendRequest(BOND_PR_STN, "UserSearchDie_NoMove", stMsg);
            }
            else if (lCameraSelection == 2)
            {
                  LONG lMode = 11;
                  stMsg.InitMessage(sizeof(LONG), &lMode);
                  nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserUplookSearchDie", stMsg);
            }
            else if (lCameraSelection == 3)
            {
                LONG lLoopTest = 1;
                stMsg.InitMessage(sizeof(LONG), &lLoopTest);
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchMark1", stMsg);
            }
            else if (lCameraSelection == 4)
            {
				 LONG lLoopTest = 1;
				 stMsg.InitMessage(sizeof(LONG), &lLoopTest);
                 nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchMark2", stMsg);
            }
           else if (lCameraSelection == 5)
            {
                LONG lLoopTest = 1;
                stMsg.InitMessage(sizeof(LONG), &lLoopTest);
				nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchColletHole1", stMsg);
            }
            else if (lCameraSelection == 6)
            {
				 LONG lLoopTest = 1;
				 stMsg.InitMessage(sizeof(LONG), &lLoopTest);
                 nConvID = m_comClient.SendRequest(WAFER_PR_STN, "UserSearchColletHole2", stMsg);
            }
		   else if (lCameraSelection == 7)
		   {
		    	nConvID = m_comClient.SendRequest("BondPrStn", "ExtractObjectCmd", stMsg);
		   }
            else
            {
                  break;
            }
            while (1)
            {
                  if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
                  {
                        m_comClient.ReadReplyForConvID(nConvID,stMsg);
                        stMsg.GetMsg(sizeof(SRCH_RESULT), &stResult);
                        szLog.Format("PRLoopTest(in count),Offsetx,%f,Offsety,%f,Angle,%f",stResult.dOffsetX,stResult.dOffsetY,stResult.dAngle);
						CMSLogFileUtility::Instance()->MissingDieThreshold_Log(FALSE, szLog);
                        
                        SD_GetDataAdv(szaAngle, stResult.dAngle);
                        SD_GetData(stResult.dOffsetX,stResult.dOffsetY);
                        break;
                  }
                  else
                  {
                        Sleep(10);
                  }
            }
            lCount ++;
            if (lCount > lLoop)
            {
                  SD_Calculate(dMax1,dMin1,dMax2,dMin2,dSD1,dSD2);
                  SD_CalculateAdv(szaAngle,dAngleMax,dAngleMin,dAngleSD);
                  break;
            }
      }
	  CMSLogFileUtility::Instance()->MissingDieThreshold_LogClose(FALSE);
      szLog.Format("X,Max,%.2f,Min,%.2f,Range,%f,SD,%f\n Y,Max,%.2f,Min,%.2f,Range,%f,SD,%f\n Angle,Max,%.4f,Min,%f,Range,%.4f,SD,%lf"
            ,dMax1,dMin1,dMax1 - dMin1,dSD1,dMax2,dMin2,dMax2 - dMin2, dSD2,
            dAngleMax,dAngleMin,dAngleMax - dAngleMin,dAngleSD);
      HmiMessage("Loop Test Finish!\n" + szLog);
	  if ((lCameraSelection == 3) || (lCameraSelection == 5)) //Mark1
	  {
		  nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_CancelBondHeadSetup", stMsg);
		  while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bGet);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
	  }
	  else if ((lCameraSelection == 4) || (lCameraSelection == 6)) //Mark2
	  {
		  nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "BH_CancelBondHeadSetupZ2", stMsg);
		  while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID,stMsg);
					stMsg.GetMsg(sizeof(BOOL), &bGet);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
	  }
      svMsg.InitMessage(sizeof(BOOL), &bReturn);
      return 1;
}