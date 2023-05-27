#include "stdafx.h"
#include "MS896A_Constant.h"
#include "MS896A.h"
#include "HmiDataManager.h"
#include "CycleState.h"
#include "MS_AccessConstant.h"
#include "FileUtil.h"
#include "MS_SecCommConstant.h"
#include "MS_HostCommConstant.h"
#include  "MESConnector.h"
#include "StrInputDlg.h"
//#include "Encryption.h"

#include <atltime.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CMS896AApp::AWET_CheckAlarmCode(CString szCode)
{
	if (!m_bAWET_Enabled) // test name first
	{
		return FALSE;
	}

	CString szMsg;
	CStdioFile cfAWET_ListFile;
	if (cfAWET_ListFile.Open(gszAWET_FILENAME, CFile::modeNoTruncate|CFile::modeRead|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		return FALSE;
	}

	CStringArray szList;
	CString szRead;
	bool bFoundInList = FALSE; 
	LONG lAlarmLimit = 1, lAlarmTime = 10;
	while( cfAWET_ListFile.ReadString(szRead) )
	{
		if (szRead == "")
		{
			continue;
		}
		szList.RemoveAll();
		ParseRawDataComma(szRead, szList);
		if (szList.GetSize() < 1)
		{
			continue;
		}
		if (szList.GetAt(0) == szCode)  // Compare AlarmList with alarm msg
		{
			bFoundInList = TRUE;
			if( szList.GetSize()>1 )
			{
				lAlarmLimit = atoi(szList.GetAt(1));
			}
			if( szList.GetSize()>2 )
			{
				lAlarmTime = atoi(szList.GetAt(2));
			}
			break;
		}
	}
	cfAWET_ListFile.Close();

	if (m_ulAWET_LastCounter == 0)
	{
		m_szAWET_LastCode		= "";
	}

	if (m_szAWET_LastCode != szCode)
	{
		m_ulAWET_LastCounter	= 1;
		m_ulAWET_LastRunTime	= m_eqMachine.GetTime(EQUIP_RUN_TIME);
	}
	else
	{
		m_ulAWET_LastCounter++;
	}
	m_szAWET_LastCode			= szCode;

	if (szCode == "OP SOS")
	{
		bFoundInList = TRUE;
	}
	if (bFoundInList != TRUE)
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("[Alarm Wait Engineer Time] not found for " + szCode);
		return FALSE;
	}

	if (lAlarmLimit > 1)
	{
		if (m_ulAWET_LastCounter < (ULONG)lAlarmLimit)
		{
			szMsg.Format("[Alarm Wait Engineer Time] found for %s but counter %lu < limit %ld", szCode, m_ulAWET_LastCounter, lAlarmLimit);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			return FALSE;
		}

		ULONG ulTotalTime = m_eqMachine.GetTime(EQUIP_RUN_TIME) - m_ulAWET_LastRunTime;
		if (ulTotalTime > (ULONG)(lAlarmTime * 60))
		{
			m_ulAWET_LastCounter	= 1;
			m_ulAWET_LastRunTime	= m_eqMachine.GetTime(EQUIP_RUN_TIME);
			szMsg.Format("[Alarm Wait Engineer Time] found for %s but total time %lu > %ld seconds",
				szCode, ulTotalTime, lAlarmTime*60);
			CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			return FALSE;
		}
		szMsg.Format("[Alarm Wait Engineer Time] found for %s, counter %lu(%ld), total time %lu seconds",
			szCode, m_ulAWET_LastCounter, lAlarmLimit, ulTotalTime);
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	}
	else
	{
		szMsg = "[Alarm Wait Engineer Time] found for " + szCode;
		CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	}

	SetAlarmLamp_Status(ALARM_GREEN_YELLOW_RED_ON, TRUE, FALSE);
	m_lAWET_EngineerFlowState	= 1;
	m_bAWET_AutoSetScreen	= TRUE;	//	alarm
	m_bAWET_Triggered		= TRUE; // let Alarm message display
	m_szAWET_AlarmCode		= szCode;
	m_ulAWET_LastCounter	= 0;
	m_szAWET_LastCode		= "";

	CString szStartStopSetupTime = m_smfSRam["MS896A"]["StartStop Setup Time"]; 
	if(szStartStopSetupTime == "Start" )
	{
	//	CMSLogFileUtility::Instance()->MS_LogOperation("[Alarm Wait Engineer Time] == Start, with Closed Setup Time");
	//	m_eqMachine.SetTime (FALSE, EQUIP_SETUP_TIME, szCode);// Stop SetTime
	}
	else
	{
		m_eqMachine.SetTime(TRUE, EQUIP_WAITING_SETUP_TIME, szCode); // Start WaitingSetTime;
		m_eqMachine2.SetTime(TRUE, EQUIP_WAITING_SETUP_TIME, szCode); // Start WaitingSetTime;
		CMSLogFileUtility::Instance()->MS_LogOperation("[Alarm Wait Engineer Time] Opened EQUIP WAITING SETUP TIME ");
	}

	return TRUE;
}

LONG CMS896AApp::AWET_IntoRecoverScreen(IPC_CServiceMessage &svMsg)
{
	m_bAWET_AutoSetScreen		= FALSE;	// screen pre-roution
	m_bAWET_CtrlEngContainer	= FALSE;
	m_bAWET_CtrlExitButton		= FALSE;
	m_bAWET_CtrlOpContainer		= FALSE;
	if( m_lAWET_EngineerFlowState<1 )
	{
		m_bAWET_CtrlExitButton	= TRUE;
	}
	else if( m_lAWET_EngineerFlowState>=3 )
	{
		m_bAWET_CtrlOpContainer = TRUE;
	}
	else
	{
		m_bAWET_CtrlEngContainer = TRUE;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CMS896AApp::AWET_EngineerLogIn(IPC_CServiceMessage &svMsg)
{
	SetAlarmLamp_Yellow(FALSE, FALSE);
	m_bAWET_CtrlOpContainer		= FALSE;
	m_bAWET_CtrlEngContainer	= FALSE;
	m_bAWET_CtrlExitButton		= FALSE;
	m_lAWET_EngineerFlowState++;
	CString szCode = m_szAWET_AlarmCode;
	m_eqMachine.SetTime(FALSE, EQUIP_WAITING_SETUP_TIME, szCode); // Start WaitingSetTime;
	m_eqMachine2.SetTime(FALSE, EQUIP_WAITING_SETUP_TIME, szCode); // Start WaitingSetTime;
	if( m_lAWET_EngineerFlowState<3 )
		m_bAWET_CtrlExitButton	= TRUE;
	else
		m_bAWET_CtrlOpContainer = TRUE;

	if( m_lAWET_EngineerFlowState == 3 )
	{
		m_eqMachine.SetTime (FALSE, EQUIP_SETUP_TIME, szCode); 
		m_eqMachine2.SetTime(FALSE, EQUIP_SETUP_TIME, szCode);
		m_szAWET_AlarmCode = "";
		m_bAWET_Triggered  = FALSE; // engineer handling problem done
		m_lAWET_EngineerFlowState = 0;
	}
	else
	{
		m_eqMachine.SetTime (TRUE, EQUIP_SETUP_TIME, szCode); 
		m_eqMachine2.SetTime(TRUE, EQUIP_SETUP_TIME, szCode);
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::AWET_OperatorLogIn(IPC_CServiceMessage &svMsg)
{
	m_bAWET_CtrlOpContainer		= FALSE;
	m_bAWET_CtrlEngContainer	= FALSE;
	m_bAWET_CtrlExitButton		= TRUE;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CMS896AApp::AWET_OperatorAskHelp(IPC_CServiceMessage &svMsg)
{
	AWET_CheckAlarmCode("OP SOS");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
