/////////////////////////////////////////////////////////////////
// MS_SectComm.cpp : interface of the MS896AStn class
//
//	Description:
//		MS896A Sec Communication 
//
//	Date:		Wednesday, Nov 16, 2005
//	Revision:	1.00
//
//	By:			Barry Chu
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include "MS_SecCommConstant.h"
#include "SC_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// secs gem relative static variables
CGemStation*    CMS896AStn::m_pGemStation;
BOOL	CMS896AStn::m_bIsSecsGemInit;
BOOL	CMS896AStn::m_bIsSecsCommEnabled = FALSE;
LONG	CMS896AStn::m_lEquipmentStatus;
BOOL	CMS896AStn::m_bUseSecsGem;
//BOOL	CMS896AStn::m_bStopByCommand = FALSE;

//v4.11T1
#ifndef MS50_64BIT
Lextar	CMS896AStn::m_stLexter;					
#endif
BOOL	CMS896AStn::m_bEnableLextarSecsGem	= FALSE;
//v4.37T5	//EquipMgr
//ULONG	CMS896AStn::m_ulEMLastOutput		= 0;
//ULONG	CMS896AStn::m_ulEMLastProdTime		= 0;
//ULONG	CMS896AStn::m_ulEMLastStandbyTime	= 0;
//ULONG	CMS896AStn::m_ulEMLastDownTime		= 0;
//ULONG	CMS896AStn::m_ulEMLastNumOfAssists	= 0;
//ULONG	CMS896AStn::m_ulEMLastNumOfFailures	= 0;
//ULONG	CMS896AStn::m_ulEMLastSetupTime		= 0;

// secs gem relative object
BOOL CMS896AStn::IsSecsGemInit(VOID)
{
	if ((m_pGemStation != NULL) && (m_bIsSecsGemInit == TRUE))
	{
        return TRUE;
	}
    else
	{
        return FALSE;
	}
}

BOOL CMS896AStn::IsSecsGemStarted(VOID)
{
	if ((m_pGemStation != NULL) && (m_bIsSecsGemInit == TRUE) && m_bIsSecsCommEnabled)
	{
		return TRUE;
	}
	return FALSE;
}

// set gem station object value
// CString
VOID CMS896AStn::SetGemValue(CString szVName, CString szValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[szVName] = szValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, CString szValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[nVID] = szValue;
}

VOID CMS896AStn::GetGemValue(CString szVName, CString &szValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    szValue = (CString)(*m_pGemStation)[szVName];
}

VOID CMS896AStn::GetGemValue(INT nVID, CString &szValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    szValue = (CString)(*m_pGemStation)[nVID];
}

// CHAR // UCHAR
VOID CMS896AStn::SetGemValue(CString szVName, CHAR cValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[szVName] = cValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, CHAR cValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[nVID] = cValue;
}

VOID CMS896AStn::SetGemValue(CString szVName, UCHAR ucValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[szVName] = ucValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, UCHAR ucValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[nVID] = ucValue;
}

VOID CMS896AStn::GetGemValue(CString szVName, UCHAR &ucValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    ucValue = (*m_pGemStation)[szVName];
}

VOID CMS896AStn::GetGemValue(INT nVID, UCHAR &ucValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    ucValue = (*m_pGemStation)[nVID];
}

// INT
VOID CMS896AStn::SetGemValue(CString szVName, INT iValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[szVName] = iValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, INT iValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[nVID] = iValue;
}

VOID CMS896AStn::GetGemValue(CString szVName, INT &iValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    iValue = (*m_pGemStation)[szVName];
}

VOID CMS896AStn::GetGemValue(INT nVID, INT &iValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    iValue = (*m_pGemStation)[nVID];
}

// UINT
VOID CMS896AStn::SetGemValue(CString szVName, UINT uiValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[szVName] = uiValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, UINT uiValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[nVID] = uiValue;
}

VOID CMS896AStn::GetGemValue(CString szVName, UINT &uiValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    uiValue = (*m_pGemStation)[szVName];
}

VOID CMS896AStn::GetGemValue(INT nVID, UINT &uiValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    uiValue = (*m_pGemStation)[nVID];
}

// LONG 
VOID CMS896AStn::SetGemValue(CString szVName, ULONG ulValue)
{
    SetGemValue(szVName, (LONG) ulValue);
}

VOID CMS896AStn::SetGemValue(INT nVID, ULONG ulValue)
{
    SetGemValue(nVID, (LONG) ulValue);
}

VOID CMS896AStn::SetGemValue(CString szVName, LONG lValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[szVName] = lValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, LONG lValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    (*m_pGemStation)[nVID] = lValue;
}

VOID CMS896AStn::GetGemValue(CString szVName, LONG &lValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    lValue = (*m_pGemStation)[szVName];
}

VOID CMS896AStn::GetGemValue(INT nVID, LONG &lValue)
{
    if( IsSecsGemInit()==FALSE )
        return ;

    lValue = (*m_pGemStation)[nVID];
}

// FLOAT/DOUBLE
VOID CMS896AStn::SetGemValue(CString szVName, DOUBLE dValue)
{
    SetGemValue(szVName, (FLOAT)dValue);
}

VOID CMS896AStn::SetGemValue(INT nVID, DOUBLE dValue)
{
    SetGemValue(nVID, (FLOAT)dValue);
}

VOID CMS896AStn::SetGemValue(CString szVName, FLOAT fValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    (*m_pGemStation)[szVName] = fValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, FLOAT fValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    (*m_pGemStation)[nVID] = fValue;
}

VOID CMS896AStn::GetGemValue(CString szVName, DOUBLE &dValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    dValue = (*m_pGemStation)[szVName];
}

VOID CMS896AStn::GetGemValue(INT nVID, DOUBLE &dValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    dValue = (*m_pGemStation)[nVID];
}

// SHORT/USHORT
VOID CMS896AStn::SetGemValue(CString szVName, SHORT sValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    (*m_pGemStation)[szVName] = sValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, SHORT sValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    (*m_pGemStation)[nVID] = sValue;
}

VOID CMS896AStn::SetGemValue(CString szVName, USHORT usValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    (*m_pGemStation)[szVName] = usValue;
}

VOID CMS896AStn::SetGemValue(INT nVID, USHORT usValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    (*m_pGemStation)[nVID] = usValue;
}

VOID CMS896AStn::GetGemValue(CString szVName, USHORT &usValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    usValue = (*m_pGemStation)[szVName];
}

VOID CMS896AStn::GetGemValue(INT nVID, USHORT &usValue)
{
	if (!IsSecsGemInit())
	{
		return;
	}

    usValue = (*m_pGemStation)[nVID];
}

VOID CMS896AStn::SetEquipState(LONG lEquipState)
{
	BOOL bStateChanged = FALSE;

	if (!IsSecsGemInit())
	{
		return;
	}

	if (m_lEquipState != lEquipState)
	{
		bStateChanged	= TRUE;
		m_lPreEquipState = m_lEquipState;
		m_lEquipState = lEquipState;

		try
	    {
			CString szLog;

//			if (m_bEquipmentManager)
			{
				//v4.59A32	//EquipmentManager
				szLog.Format("\t>>> CEID #100 (EqState=%ld)", lEquipState);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

				if (IsSecsGemStarted())
				{
					SetGemValue(MS_SECS_SV_E10_STATE,	m_lEquipState);
					SendEvent(SG_CEID_EQUIPMENT_E10_STATECHANGE);
				}
			}
/*
			else
			{
				szLog.Format("\t>>> CEID #8 (EqState=%ld)", lEquipState);
				CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
				SetGemValue("ProcessState",			m_lEquipState);
				SetGemValue("PreviousProcessState", m_lPreEquipState);
				SendEvent(SG_CEID_PROCESSING_STATE_CHANGE);
			}
*/
	    }
	    catch (CAsmException e)
	    {
		    DisplayException(e);
	    }
	}
//	if(bStateChanged) SecsE32PerformanceData();
}

USHORT CMS896AStn::GetProcessState()
{
	USHORT hProcessState = PROCESS_IDLE_LOCAL;
	GetGemValue(CString("ProcessState"), hProcessState);
	return hProcessState;
}

VOID CMS896AStn::SetProcessState(SHORT hProcessState)
{
	USHORT hPreProcessState = 0, hPreProcessState1 = 0;

	if (this == NULL) 
	{
		return;
	}

	if (!IsSecsGemInit())
	{
		return;
	}

	hPreProcessState1 = GetProcessState();

	if (hProcessState != hPreProcessState1) //20090407
	{
		SendEvent(SG_CEID_PROCESSING_STATE_COMPLETED);
	}

	hPreProcessState = GetProcessState();
	SetGemValue(CString("PreviousProcessState"), hPreProcessState);
	if (hProcessState != hPreProcessState) //20090407
	{
		SendEvent(SG_CEID_PROCESSING_STATE_STOPPED);
	}

	if (hProcessState == PROCESS_IDLE_LOCAL)
	{
		//hProcessState = (m_hSecsCtrlMode == CTRL_MODE_REMOTE) ? PROCESS_IDLE_REMOTE : PROCESS_IDLE_LOCAL;
		hProcessState = PROCESS_IDLE_REMOTE;
	}
	else if (hProcessState == PROCESS_EXE_LOCAL)
	{
		//hProcessState = (m_hSecsCtrlMode == CTRL_MODE_REMOTE) ? PROCESS_EXE_REMOTE : PROCESS_EXE_LOCAL;
		hProcessState = PROCESS_EXE_REMOTE;
	}

	SetGemValue(CString("ProcessState"), hProcessState);
	if (hProcessState != hPreProcessState) //20090407
	{
		SendEvent(SG_CEID_PROCESSING_STATE_STARTED);
	}
	if (hProcessState != hPreProcessState)
	{
		SendEvent(SG_CEID_PROCESSING_STATE_CHANGE);
	}
}


BOOL CMS896AStn::IsEquipmentUnscheduleDown()
{
	return (m_lEquipmentStatus == glSG_UNSCHEDULE_DOWN);
}

VOID CMS896AStn::EquipStateProductiveToUnscheduleDown()
{
    if (m_lEquipmentStatus == glSG_PRODUCTIVE)
    {
        m_lEquipmentStatus = glSG_UNSCHEDULE_DOWN;
        SetEquipState(m_lEquipmentStatus);
		SetProcessState(PROCESS_ERROR);
    }
}

VOID CMS896AStn::EquipStateUnscheduleDwonToProductive()
{
    if( m_lEquipmentStatus==glSG_UNSCHEDULE_DOWN )
    {
        m_lEquipmentStatus = glSG_PRODUCTIVE;
        SetEquipState(m_lEquipmentStatus);
		SetProcessState(PROCESS_EXE_LOCAL);
    }
}
 
/*
VOID CMS896AStn::RecallEquipmentStatistic(CString szStartTime, CString szEndTime)
{
    if( IsSecsGemInit()==FALSE )
        return;
	if (!m_bEquipmentManager)	//v4.59A32
		return;

	CString szCurrDate;
	//CString szStartDate, szEndDate;
	//szStartDate = startTime.Format("%Y-%m-%d %H:%M:%S");
	//szEndDate	= endTime.Format("%Y-%m-%d %H:%M:%S");
	szCurrDate	= szStartTime + _T(" to ") + szEndTime + _T(")");

	CString szINIContent = _T("[No Record]");

	CString szLog;
	szLog.Format("\t>>> CEID #%d (Recall): (Date: " + szCurrDate, SG_CEID_EQUIPMENT_STATISTIC_DATA_EVENT);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szINIContent);

	//v4.59A33
	CTime tTime = CTime::GetCurrentTime();
	LoadEquipmentStatisticFromFile(tTime, szINIContent);

	SetGemValue(MS_SECS_SV_ES_INIBODY, szINIContent);
	SendEvent(SG_CEID_EQUIPMENT_STATISTIC_DATA_EVENT);
}


BOOL CMS896AStn::SaveEquipmentStatisticToFile(CTime tTime, CString szContent)
{
	CString szFullPath, szPath, szFileName;
	CString szLog;


	szPath		= gszUSER_DIRECTORY + _T("\\EM");
	szFileName	= tTime.Format("%Y%m%d_%H%M.csv");
	szFullPath	= gszUSER_DIRECTORY + "\\EM\\" + szFileName;

	CreateDirectory(szPath, NULL);

	if (_access(szFullPath, 0) != -1)
	{
		return FALSE;	//File already existed
	}

	CStdioFile oEMFile;
	if (!oEMFile.Open(szFullPath, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText))
	{
		szLog = "EM: fail to write to Equipment Statistics File - " + szFullPath; 
		SetErrorMessage(szLog);
		HmiMessage_Red_Back(szLog);
		return FALSE;
	}

	oEMFile.WriteString(szContent);
	oEMFile.Close();
	return TRUE;
}


BOOL CMS896AStn::LoadEquipmentStatisticFromFile(CTime tTime, CString& szContent)
{
	CString szFullPath, szPath, szFileName;
	CString szLog;

	szPath		= gszUSER_DIRECTORY + _T("\\EM");
	szFileName	= tTime.Format("%Y%m%d_%H%M.csv");
	szFullPath	= gszUSER_DIRECTORY + "\\EM\\" + szFileName;

	CreateDirectory(szPath, NULL);

	if (_access(szFullPath, 0) == -1)
	{
		szLog = "EM: Equipment Statistics File not existed - " + szFullPath; 
		SetErrorMessage(szLog);
		HmiMessage_Red_Back(szLog);
		return FALSE;	//File already existed
	}

	CStdioFile oEMFile;
	if (!oEMFile.Open(szFullPath, CFile::modeRead|CFile::shareExclusive|CFile::typeText))
	{
		szLog = "EM: fail to read Equipment Statistics File - " + szFullPath; 
		SetErrorMessage(szLog);
		HmiMessage_Red_Back(szLog);
		return FALSE;
	}

	CString szLine, szTotal;
	while (oEMFile.ReadString(szLine) != NULL)
	{
		szTotal += szLine + "\n";
	}
	oEMFile.Close();

	szContent = szTotal;
	return TRUE;
}

VOID CMS896AStn::BackupEquipmentStatisticFiles()
{
	CString szPath = gszUSER_DIRECTORY + _T("\\EM");
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(szPath, 90);
}
*/

VOID CMS896AStn::SetEquipmentManagementInfo(BOOL bSend)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	try
	{
		UpdateEquipmentBasicStatistics();

		if (bSend)
		{
			SendEvent(SG_CEID_EQUIPMENT_MANAGEMENT_INFO);
		}
	}
	catch (...) //CAsmException e)
	{
		DisplaySequence("SetEquipmentManagementInfo failure");
	}
}


VOID CMS896AStn::UpdateEquipmentBasicStatistics()
{
	if (!IsSecsGemInit())
	{
		return;
	}

	CMS896AApp* pApp = (CMS896AApp*)AfxGetApp();
	INT nNoOfAlarm			= pApp->m_eqMachine.GetNoOfAlarm();
	INT nNoOfAssist			= pApp->m_eqMachine.GetNoOfAssistance();
	ULONG ulProductiveTime	= pApp->m_eqMachine.GetTime(EQUIP_RUN_TIME);
	ULONG ulOutputCount		= pApp->m_eqMachine.GetUnit();
//	ulProductiveTime = 10000;
//	nNoOfAssist = 2;
//	nNoOfAlarm = 4;

	DOUBLE dMTBA = 0;
	if ((nNoOfAssist) > 0)
	{
		dMTBA = 1.0 * ulProductiveTime / nNoOfAssist;
	}
	else
	{
		dMTBA = 1.0 * ulProductiveTime;
	}

	DOUBLE dMTBF = 0;
	if ((nNoOfAlarm) > 0)
	{
		dMTBF = 1.0 * ulProductiveTime / nNoOfAlarm;
	}
	else
	{
		dMTBF = 1.0 * ulProductiveTime;
	}
//m_dMachineUPH = 210;

	if (ulProductiveTime > 0)
	{
		m_dMachineUPH = ulOutputCount / ((double)ulProductiveTime / 3600.0);
	}
	else
	{
		m_dMachineUPH = 0;
	}

	try
	{
		SetGemValue(MS_SECS_SV_UNIT_PER_HOUR,			_round(m_dMachineUPH));
		SetGemValue(MS_SECS_SV_EQUIP_MTBA,				_round(dMTBA));
		SetGemValue(MS_SECS_SV_EQUIP_MTBF,				_round(dMTBF));
		SetGemValue(MS_SECS_SV_TOTAL_DOWN_TIME,			(INT) pApp->m_eqMachine.GetTime(EQUIP_DOWN_TIME));
		SetGemValue(MS_SECS_SV_TOTAL_IDLE_TIME,			(INT) pApp->m_eqMachine.GetTime(EQUIP_IDLE_TIME));
		SetGemValue(MS_SECS_SV_TOTAL_PRODUCTIVE_TIME,	(INT) pApp->m_eqMachine.GetTime(EQUIP_RUN_TIME));
		SetGemValue(MS_SECS_SV_NO_OF_FAILURE,			(INT) nNoOfAlarm);
		SetGemValue(MS_SECS_SV_NO_OF_ASSIST,			(INT) nNoOfAssist);
		SetGemValue(MS_SECS_SV_STATISTIC_START_DATE,	(CString) pApp->m_eqMachine.GetEquipDate());

		CString szMachineNo		= (*m_psmfSRam)["MS896A"]["MachineNo"];
		CString szSWReleaseDate = (*m_psmfSRam)["MS896A"]["Release Date"];

		SetGemValue(MS_SECS_SV_MACHINE_ID,				(CString) szMachineNo);
		SetGemValue(MS_SECS_SV_LAST_PM_DATE,			(CString) szSWReleaseDate);
		SetGemValue(MS_SECS_SV_INSTALLATION_DATE,		(CString) szSWReleaseDate);
	}
	catch (...) //CAsmException e)
	{
		DisplaySequence("UpdateEquipmentBasicStatistics failure");
	}
}

VOID CMS896AStn::SendEquipmentBasicStatistics()
{
	if (!IsSecsGemInit())
	{
		return;
	}

	UpdateEquipmentBasicStatistics();
	SendEvent(SG_CEID_EQUIPMENT_BASIC_STATISTIC);
}


VOID CMS896AStn::DownloadPkgFile(CString szPPID)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	CString szDataFile;
	CString szDevicePath = (*m_psmfSRam)["MS896A"]["DevicePath"];

	//Delete existing file from original path
	//m_stSecsGem.szPPID = szPPID;
	szDataFile = szDevicePath + "\\" + szPPID;
	//DeleteFile(szDataFile);

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("DOWNLOAD PKG at: " + szDataFile);	

	//v4.51A12
	//BOOL bDownloadStatus = FALSE;	//m_pGemStation->ProcessProgramReq(szPPID);
	BOOL bDownloadStatus = m_pGemStation->ProcessProgramReq(szPPID);

	CString szLog;
	szLog.Format("DOWNLOAD PKG Status = %d", bDownloadStatus);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);	
	//SECS_SetAckStatus(FALSE, 0);
}


VOID CMS896AStn::UploadPkgFile(CString szPPID, BOOL bFromHost)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	BOOL bUploadStatus = TRUE;
	CString szDataFile;
	CFile cfFile;
	CString szDevicePath = (*m_psmfSRam)["MS896A"]["DevicePath"];

	//Delete existing file from original path
	//m_stSecsGem.szPPID = szPPID;
	szDataFile = szDevicePath + "\\" + szPPID;

	CMSLogFileUtility::Instance()->MS_LogSECSGEM("UPLOAD PKG at: " + szDataFile);	

	if ( cfFile.Open(szDataFile, CFile::modeRead) == TRUE )
    {
        UINT ullSize = (UINT)cfFile.GetLength();
        char *pBuffer = new char[ullSize];

		cfFile.SeekToBegin();
        cfFile.Read(pBuffer, ullSize);
        cfFile.Close();

		if ( bFromHost == FALSE )
		{
			bUploadStatus = m_pGemStation->DownloadProcessProgram(szPPID, ullSize, pBuffer);
		}
		else
		{
			bUploadStatus = m_pGemStation->UploadProcessProgram(szPPID, ullSize, pBuffer);
		}

		delete[] pBuffer;

		CString szLog;
		szLog.Format("UPLOAD PKG Status = %d", bUploadStatus);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);	
		//SECS_SetAckStatus(FALSE, 0);
    }
    else
    {
		if ( bFromHost == FALSE )
		{
			CString szTitle, szContent;
			szTitle		= _T("SECSGEM: Upload PKG file");
			szContent	= _T("");
			//szTitle.LoadString(HMB_MS_SYS_MESSAGE);
			//szContent.LoadString(HMB_MS_SECSGEM_SELECT_PKG_INVAILD);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300);
		}
		//SECS_SetAckStatus(TRUE, 1);
	}
}


// send event
VOID CMS896AStn::SendEvent(INT nCEID, BOOL bReply)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	//if (m_bEnableLextarSecsGem)		//v4.13T5
	//	return;

	//v4.13T1
	CString szLog;
	szLog.Format("\t>>> CEID #%d", nCEID);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

    m_pGemStation->SendEvent(nCEID, bReply);
}

VOID CMS896AStn::SetAlarm(int nAlarmID, BOOL bReply)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	if (m_bEnableLextarSecsGem)		//v4.13T5
		return;

	//v4.13T1
	CString szLog;
	szLog.Format("\t>>> AlarmID #%d (Reply = %d)", nAlarmID, bReply);	//v4.59A22
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

    m_pGemStation->SetAlarm(nAlarmID, bReply);
}

VOID CMS896AStn::SetAlarm(CString szHmiCode, BOOL bReply)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	if (m_bEnableLextarSecsGem)		//v4.13T5
		return;

	//v4.13T1
	CString szLog = "\t>>> AlarmID - " + szHmiCode;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

    m_pGemStation->SetAlarm(szHmiCode, bReply);
}

VOID CMS896AStn::ClearAlarm(int nAlarmID ,BOOL bReply)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	if (m_bEnableLextarSecsGem)		//v4.13T5
		return;

 	//v4.13T1
	CString szLog;
	szLog.Format("\t>>> Clear AlarmID #%d", nAlarmID);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	m_pGemStation->ClearAlarm(nAlarmID, bReply);
}

VOID CMS896AStn::ClearAlarm(CString szHmiCode ,BOOL bReply)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	if (m_bEnableLextarSecsGem)		//v4.13T5
		return;

	//v4.13T1
	CString szLog = "\t>>> Clear AlarmID - " + szHmiCode;
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

    m_pGemStation->ClearAlarm(szHmiCode, bReply);
}


/////////////////////////////////////////////////////////////////
// Lextar specific SECSGEM Functions
/////////////////////////////////////////////////////////////////

VOID CMS896AStn::EnableLextarSecsGemFcns(BOOL bEnable)
{
	m_bEnableLextarSecsGem = bEnable;
#ifndef MS50_64BIT
	m_stLexter.bLextar = bEnable;
	if (bEnable)
	{
		m_stLexter.szRemoteStatus = "A";
	}
#endif
	UpdateLextarGemData(TRUE);
}

VOID CMS896AStn::UpdateLextarGemData(BOOL bUpdate)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	if (!m_bEnableLextarSecsGem)
		return;

#ifndef MS50_64BIT
	if (bUpdate)
	{
		m_pGemStation->SetLextar(&m_stLexter);
	}
	else
	{
		m_pGemStation->GetLextar(m_stLexter);
	}
#endif
}

BOOL CMS896AStn::SendLextarStreamMessage(int nStream, int nFunction)
{
	if (!IsSecsGemInit())
	{
		return FALSE;
	}

	if (!m_bEnableLextarSecsGem)
		return FALSE;

#ifndef MS50_64BIT
	m_pGemStation->LextarSendMessage((int) nStream, (int) nFunction);
#endif
	return TRUE;
}

BOOL CMS896AStn::SendLexter_S1F65(UCHAR ucMachineState)
{
	if (!IsSecsGemInit())
	{
		return FALSE;
	}

	if (!m_bEnableLextarSecsGem)
		return FALSE;

	//ucMachineState: 0=OFFLINE, 1=IDLE, 2=RUNNING
	//Lexter Spec page #53

#ifndef MS50_64BIT
	m_stLexter.S1F65_UnitNo = 0;	//0="Whole equipment"
	switch (ucMachineState)
	{
	case 0:		//OFFLINE
		m_stLexter.S1F65_Status = "DDAL";
		break;
	case 2:		//RUNNING
		m_stLexter.S1F65_Status = "DRAL";
		break;
	case 1:		//IDLE
	default:
		m_stLexter.S1F65_Status = "DIAL";
		break;
	}
#endif

	//CMSLogFileUtility::Instance()->MS_LogSECSGEM("\t>>> S1F65 (" + m_stLexter.S1F65_Status + ")");
	UpdateLextarGemData(TRUE);
	SendLextarStreamMessage(1, 65);		//S1F55 SECSGEM custom Stream Function
	return TRUE;
}

BOOL CMS896AStn::SendLexter_S5F65(UCHAR ucAlarmCode, LONG lAlarmID, BOOL bSerious)
{
	if (!IsSecsGemInit())
	{
		return FALSE;
	}

	if (!m_bEnableLextarSecsGem)
		return FALSE;

	//UCHAR S5F65_UnitNo;
	//UCHAR S5F65_AlarmCode;
	//char* S5F65_AlarmReasonID;
	//CString S5F65_SequenceNumber;
	//S5F65_LotNumberList aS5F65_LotNumberList;
	//CString S5F65_AlarmText;
#ifndef MS50_64BIT

	m_stLexter.S1F65_UnitNo		= 0;			//0="Whole equipment"

	////////////////////////////////////////
	// Alarm COde BYTE definition:
	////////////////////////////////////////
	//  Bit #1: Danger for human
	//  Bit #2: Equipment error
	//  Bit #3: Parameter overflow cause process error
	//	Bit #4: Parameter overflow cause equipment can't work
	//	Bit #5: Can not recover trouble
	//	Bit #6: Equipment status warning
	//	Bit #7: process reach to predefined status
	//	Bit #8: 1 -> alarm occurred, 0 -> alarm removed
	m_stLexter.S5F65_AlarmCode	= ucAlarmCode;

	long lAlarmReason = lAlarmID << 8;
	if (bSerious)
		m_stLexter.S5F65_AlarmReasonID = lAlarmReason + 1;
	else
		m_stLexter.S5F65_AlarmReasonID = lAlarmReason;

	CTime ct = CTime::GetCurrentTime();
	CString szTime = ct.Format("%Y%m%d%H%M%S");
	m_stLexter.S5F65_SequenceNumber = szTime;
#endif

	CString szLog;
	//szLog.Format("\t>>> S5F65  ACode=%d, Reason=%ld, Time=", ucAlarmCode, m_stLexter.S5F65_AlarmReasonID);
	//CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog + szTime);
	UpdateLextarGemData(TRUE);
	SendLextarStreamMessage(5, 65);		//S5F55 SECSGEM custom Stream Function
	return TRUE;
}

VOID CMS896AStn::UpdateAllSGVariables()
{

}

BOOL CMS896AStn::UpdateSGEquipmentConst(INT nECID)
{
	return FALSE;
}


BOOL CMS896AApp::IsSecsGemInit()
{
	if ((m_pAppGemStation != NULL) && (CMS896AStn::m_bIsSecsGemInit == TRUE))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VOID CMS896AApp::UpdateAllSGVariables()
{
	extern CString gszEquipID;
	CString			szStn;
	SFM_CStation	*pStn = NULL;
	CMS896AStn		*pAppStn = NULL;
	if (!IsSecsGemInit()) 
	{
		return;
	}
	if (m_lAction == glSYSTEM_EXIT) 
	{
		return;
	}

	int iTmp = 0;
	try
	{
		if (m_pAppGemStation != NULL)
		{
		}
	}
	catch (...) //CAsmException e)
	{

		//DisplayException(e);
	}
		
	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		m_pStationMap.GetNextAssoc(pos, szStn, pStn);
		if (pStn != NULL)
		{
			pAppStn = dynamic_cast<CMS896AStn*>(pStn);			
			(pAppStn) ? pAppStn->UpdateAllSGVariables() : 0;  //Inform all stns & sub stns
		}
	}
}


VOID CMS896AApp::UpdateSGEquipmentConst(INT nECID)
{
	CString			szStn;
	SFM_CStation	*pStn;
	CMS896AStn		*pAppStn;


	for (POSITION pos = m_pStationMap.GetStartPosition(); pos;)
	{
		m_pStationMap.GetNextAssoc(pos, szStn, pStn);
		pAppStn = dynamic_cast<CMS896AStn*>(pStn);			
		if (pAppStn && pAppStn->UpdateSGEquipmentConst(nECID))  //Inform all stns & sub stns
		{
			return;
		}
	}
}


//=======================================================================================
//				Get Station Object
//=======================================================================================
CMS896AStn *CMS896AStn::GetStation(CString szStationName)
{
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	return dynamic_cast<CMS896AStn*>(pAppMod->GetStation(szStationName));
}
