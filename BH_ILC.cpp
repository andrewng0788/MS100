#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "FileUtil.h"
#include  "io.h"
#include "MS_SecCommConstant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG CBondHead::ToggleILCLog(IPC_CServiceMessage &svMsg)
{
	m_bILCLog = m_bILCLog & 0x0001;
	m_bILCLog = !m_bILCLog;

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBondHead::IsEnableILC()
{
	return FALSE;
}

BOOL CBondHead::ILC_IsInitSuccess()
{
	if (m_stBAILCPTB.bIsInit == FALSE || m_stBAILCBTP.bIsInit == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CBondHead::ILC_IsAutoLearnComplete()
{
	if (IsEnableILC() == FALSE)
	{
		return TRUE;
	}

	if (m_stBAILCPTB.bIsAutoLearnComplete == FALSE || m_stBAILCBTP.bIsAutoLearnComplete == FALSE)
	{
		return FALSE;
	}
	
	return TRUE;
}

VOID CBondHead::ILC_ResetAutoLearnComplete()
{
	if (IsEnableILC() == FALSE)
	{
		return;
	}

	m_stBAILCPTB.bIsAutoLearnComplete = FALSE;
	m_stBAILCBTP.bIsAutoLearnComplete = FALSE; 
}

BOOL CBondHead::ILC_IsNeedRestartUpdate()
{
	if (m_bSel_T == FALSE)
	{
		return FALSE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_lRestartUpdateMinTime == 0 )
	{
		m_lRestartUpdateMinTime = (LONG) pApp->GetProfileInt(gszPROFILE_SETTING, _T("ILC Update Expire Time"), 0);
	}

	if( m_lRestartUpdateMinTime != 0 )
	{
		if( m_ctILCBondingLastUpdateTime.GetYear() != 2000 )
		{
			CString szMsg;
			CString szFileName = "C:\\MapSorter\\EXE\\ILC\\updatetime.txt";
			if (_access(szFileName, 0) != -1)
			{
				FILE *fp = NULL;
				errno_t nErr = fopen_s(&fp, szFileName, "r");
				if ((nErr == 0) && (fp != NULL))
				{
					INT nYear = 2000, nMonth = 12, nDay = 25, nHour = 12, nMinute = 25, nSecond = 0;
					/* Set pointer to beginning of file: */
					fseek( fp, 0L, SEEK_SET );
					fscanf(fp, "%d-%d-%d %d:%d:%d ", &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
					fclose(fp);
					szMsg.Format("ILC auto update time:%d-%d-%d %d:%d:%d", nYear, nMonth, nDay, nHour, nMinute, nSecond);
					SetAlarmLog(szMsg);
					CTime stAutoTime = CTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
					if( stAutoTime>m_ctILCBondingLastUpdateTime )
						m_ctILCBondingLastUpdateTime = stAutoTime;
				}
			}

			CTime curTime = CTime::GetCurrentTime();
			CTimeSpan TimeDiff = curTime - m_ctILCBondingLastUpdateTime;
			if (TimeDiff > 0)
			{
				szMsg = m_ctILCBondingLastUpdateTime.Format("%Y-%m-%d %H:%M:%S");
				SetAlarmLog("ILC check time " + szMsg);
				if (TimeDiff.GetTotalMinutes() < m_lRestartUpdateMinTime )
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}


BOOL CBondHead::ILC_InitData()
{
	// prvent load pkg file to startup again
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - start");

	if (m_bILCFirstTimeStartup == FALSE)
	{
		return TRUE;
	}

	m_bILCFirstTimeStartup = FALSE;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - pick to bond");
	
	if (ILC_StructInit(m_stBAILCPTB, BH_T_PICK_TO_BOND) == FALSE)
	{
		CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - init fail");	
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - bond to pick");

	if (ILC_StructInit(m_stBAILCBTP, BH_T_BOND_TO_PICK) == FALSE)
	{
		CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - init fail");
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - complete");

	if (ILC_IsInitSuccess() == FALSE)
	{
		CString szTitle, szContent;
		szTitle.LoadString(HMB_BH_ILC);
		szContent.LoadString(HMB_BH_ILC_INIT_FAIL);
		HmiMessage(szContent, szTitle);
		
		CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - fail");
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_InitData - finish");
	
	return TRUE;
}

BOOL CBondHead::ILC_StructInit(ILC_STRUCT& stILC, LONG lBondHeadState)
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION

	LONG lILCInitRtn = 0;

	stILC.bIsInit = FALSE;
	stILC.bIsAutoLearnComplete = FALSE;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_StructInit - start");

	if (ILC_LoadParameters(stILC, lBondHeadState) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_StructInit - agmp_ctrl_ILC_initialization");

	if ((lILCInitRtn = agmp_ctrl_ILC_initialization(&stILC.ILCConfigInfo, m_bILCLog ? 1 : 0)) != 0)
	{
		CString szLog;
		szLog.Format("ILC_StructInit - agmp_ctrl_ILC_initialization fail:%d", lILCInitRtn);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		return FALSE;
	}

	stILC.bIsInit = TRUE;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_StructInit - ILC_LoadResult");

	if (agmp_ctrl_ILC_load(&stILC.ILCConfigInfo) == 0)
	{
		stILC.bIsAutoLearnComplete = TRUE;
		
	}
	
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_StructInit - complete");
#endif
#endif
#endif
	return TRUE;
}

BOOL CBondHead::ILC_RestartUpdateWarmCoolInitFunc()
{
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateWarmCoolInitFunc - pick to bond");

	if (ILC_RestartUpdateWarmCoolInit(m_stBAILCPTB) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateWarmCoolInitFunc - bond to pick");

	if (ILC_RestartUpdateWarmCoolInit(m_stBAILCBTP) == FALSE)
	{
		return FALSE;
	}
	
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateWarmCoolInitFunc - complete");

	return TRUE;
}

BOOL CBondHead::ILC_RestartUpdateWarmCoolInit(ILC_STRUCT& stILC)
{

	LONG lILCInitRtn = 0;
	stILC.ILCConfigInfo.ILCUpdateMode = ILC_RESTARTUPDATE;

	if ((lILCInitRtn = agmp_ctrl_ILC_initialization(&stILC.ILCConfigInfo, m_bILCLog ? 1 : 0)) != 0)
	{
		CString szLog;
		szLog.Format("ILC_RestartUpdateWarmCoolInit - agmp_ctrl_ILC_initialization fail:%d", lILCInitRtn);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		return FALSE;
	}

	//stILC.ILCConfigInfo.ILCUpdateMode = ILC_RESTARTUPDATE;
	stILC.ApplyingCycle = 0;
	stILC.bIsRestartUpdateConverged = FALSE;

	if (lILCInitRtn = agmp_ctrl_ILC_restart_warm_cool_load(&stILC.ILCConfigInfo) != 0)
	{
		CString szLog;
		szLog.Format("ILC_RestartUpdateWarmCoolInit - ILC_RestartWarmCoolLoad. fail:%d", lILCInitRtn);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::ILC_OpRestartUpdateWarmCool()
{
	// temp to disable log
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdateWarmCool - start");
	CycleEnableDataLog(FALSE, TRUE, FALSE);

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdateWarmCool - ILC_RestartUpdateInitFunc");
	// init restart udpate
	if (ILC_RestartUpdateWarmCoolInitFunc() == FALSE)
	{		
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdateWarmCool - ILC_RestartUpdate");
	
	if (ILC_RestartUpdateWarmCool() == FALSE)
	{
		return FALSE;
	}

	Z_MoveTo(m_lSwingLevel_Z);
	Z2_MoveTo(m_lSwingLevel_Z2);

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	T_MoveTo(m_lPrePickPos_T);

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdateWarmCool - Disable Data Log");

	// enable log again
	CycleEnableDataLog(TRUE, FALSE, FALSE);

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdateWarmCool - complete");

	return TRUE;
}

BOOL CBondHead::ILC_RestartUpdateWarmCool()
{
//	ILC_DATALOG_STRUCT *pstPTBWarmDataLog = new ILC_DATALOG_STRUCT;
//	ILC_DATALOG_STRUCT *pstPTBCoolDataLog = new ILC_DATALOG_STRUCT;
//	ILC_DATALOG_STRUCT *pstBTPWarmDataLog = new ILC_DATALOG_STRUCT;
//	ILC_DATALOG_STRUCT *pstBTPCoolDataLog = new ILC_DATALOG_STRUCT;

//	ILC_DATALOG_STRUCT *pstPTBWarmDataLog = (ILC_DATALOG_STRUCT *) malloc(sizeof(ILC_DATALOG_STRUCT));
//	ILC_DATALOG_STRUCT *pstPTBCoolDataLog = (ILC_DATALOG_STRUCT *) malloc(sizeof(ILC_DATALOG_STRUCT));
//	ILC_DATALOG_STRUCT *pstBTPWarmDataLog = (ILC_DATALOG_STRUCT *) malloc(sizeof(ILC_DATALOG_STRUCT));
//	ILC_DATALOG_STRUCT *pstBTPCoolDataLog = (ILC_DATALOG_STRUCT *) malloc(sizeof(ILC_DATALOG_STRUCT));
/*
	ILC_DATALOG_STRUCT stPTBWarmDataLog;
	ILC_DATALOG_STRUCT stPTBCoolDataLog;
	ILC_DATALOG_STRUCT stBTPWarmDataLog;
	ILC_DATALOG_STRUCT stBTPCoolDataLog;
*/

	ILC_DATALOG_STRUCT *pstPTBWarmDataLog, *pstPTBCoolDataLog;//64bit
	ILC_DATALOG_STRUCT *pstBTPWarmDataLog, *pstBTPCoolDataLog;

	pstPTBWarmDataLog = (ILC_DATALOG_STRUCT *)malloc(sizeof(ILC_DATALOG_STRUCT));
	pstPTBCoolDataLog = (ILC_DATALOG_STRUCT *)malloc(sizeof(ILC_DATALOG_STRUCT));
	pstBTPWarmDataLog = (ILC_DATALOG_STRUCT *)malloc(sizeof(ILC_DATALOG_STRUCT));
	pstBTPCoolDataLog = (ILC_DATALOG_STRUCT *)malloc(sizeof(ILC_DATALOG_STRUCT));


	BOOL bRet = ILC_SubRestartUpdateWarmCool(pstPTBWarmDataLog, pstPTBCoolDataLog,
											 pstBTPWarmDataLog, pstBTPCoolDataLog);
	if (pstPTBWarmDataLog != NULL)
	{
		free(pstPTBWarmDataLog);
	}
	if (pstPTBCoolDataLog != NULL)
	{
		free(pstPTBCoolDataLog);
	}
	if (pstBTPWarmDataLog != NULL)
	{
		free(pstBTPWarmDataLog);
	}
	if (pstBTPCoolDataLog != NULL)
	{
		free(pstBTPCoolDataLog);
	}

	return bRet;
}

BOOL CBondHead::ILC_SubRestartUpdateWarmCool(ILC_DATALOG_STRUCT *pstPTBWarmDataLog, ILC_DATALOG_STRUCT *pstPTBCoolDataLog,
											 ILC_DATALOG_STRUCT *pstBTPWarmDataLog, ILC_DATALOG_STRUCT *pstBTPCoolDataLog)
{

	BOOL bMoveDirection = TRUE;
	LONG lRtn = 0;
	CString szTemp;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateWarmCool - start");

	Z_MoveTo(m_lSwingLevel_Z);
	Z2_MoveTo(m_lSwingLevel_Z2);

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	T_MoveTo(m_lPickPos_T);
	Sleep(100);

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateWarmCool - ILC_LearnWarmCoolMove(m_stBAILCPTB, stPTBWarmDataLog, ILC_WARM)");

	if (IsCoverOpen() == TRUE)
	{

		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	if (ILC_LearnWarmCoolMove(m_stBAILCPTB, pstPTBWarmDataLog, ILC_WARM) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateWarmCool - ILC_LearnWarmCoolMove(m_stBAILCBTP, stBTPWarmDataLog, ILC_WARM");

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	if (ILC_LearnWarmCoolMove(m_stBAILCBTP, pstBTPWarmDataLog, ILC_WARM) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_LearnWarmCoolMove(m_stBAILCPTB, stPTBCoolDataLog, ILC_COOL)");

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	if (ILC_LearnWarmCoolMove(m_stBAILCPTB, pstPTBCoolDataLog, ILC_COOL) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_LearnWarmCoolMove(m_stBAILCPTB, stPTBCoolDataLog, ILC_COOL)");

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	if (ILC_LearnWarmCoolMove(m_stBAILCBTP, pstBTPCoolDataLog, ILC_COOL) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartWarmCoolJudge 1");

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}
	
	if (ILC_RestartWarmCoolJudge(m_stBAILCPTB.ILCConfigInfo, pstPTBCoolDataLog->nPos, pstPTBCoolDataLog->nEnc, pstPTBCoolDataLog->nNoOfSampleUpload, pstPTBCoolDataLog->nChannelMode,
		pstPTBWarmDataLog->nPos,pstPTBWarmDataLog->nEnc,pstPTBWarmDataLog->nNoOfSampleUpload,pstPTBWarmDataLog->nChannelMode) !=0)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartWarmCoolJudge 2");

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}
	
	if (ILC_RestartWarmCoolJudge(m_stBAILCBTP.ILCConfigInfo, pstBTPCoolDataLog->nPos, pstBTPCoolDataLog->nEnc, pstBTPCoolDataLog->nNoOfSampleUpload, pstBTPCoolDataLog->nChannelMode,
		pstBTPWarmDataLog->nPos,pstBTPWarmDataLog->nEnc,pstBTPWarmDataLog->nNoOfSampleUpload, pstBTPWarmDataLog->nChannelMode) !=0)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartWarmCoolJudge complete");
	return TRUE;
}


BOOL CBondHead::ILC_RestartUpdateInitFunc()
{
	
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateInitFunc - pick to bond");

	if (ILC_RestartUpdateInit(m_stBAILCPTB, BH_T_PICK_TO_BOND) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateInitFunc - bond to pick");

	if (ILC_RestartUpdateInit(m_stBAILCBTP, BH_T_BOND_TO_PICK) == FALSE)
	{
		return FALSE;
	}
	
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateInitFunc - complete");

	return TRUE;
}


BOOL CBondHead::ILC_RestartUpdateInit(ILC_STRUCT& stILC, LONG lBondHeadState)
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION

	LONG lILCInitRtn = 0;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateInit - Start");

	stILC.ILCConfigInfo.ILCUpdateMode = ILC_RESTARTUPDATE;

	if ((lILCInitRtn = agmp_ctrl_ILC_initialization(&stILC.ILCConfigInfo, m_bILCLog ? 1 : 0)) != 0)
	{
		CString szLog;
		szLog.Format("ILC_RestartUpdateInit - agmp_ctrl_ILC_initialization fail:%d", lILCInitRtn);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		return FALSE;
	}

	// clear the init points
	for (int i=0; i<stILC.ILCConfigInfo.LearningLength; i++)
	{
		stILC.ILCConfigInfo.OutputList[i] = 0;
	} 

	//stILC.ILCConfigInfo.ILCUpdateMode = ILC_RESTARTUPDATE;
	stILC.ApplyingCycle = 0;
	stILC.bIsRestartUpdateConverged = FALSE;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateInit - ILC_LoadResult");

	if (agmp_ctrl_ILC_load(&stILC.ILCConfigInfo) != 0)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateInit - complete");

#endif
#endif
#endif

	return TRUE;
}

BOOL CBondHead::ILC_OpRestartUpdate(BOOL bNeedUpdate)
{
	// temp to disable log6
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdate - start");
	CycleEnableDataLog(FALSE, TRUE, FALSE);

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdate - ILC_RestartUpdateInitFunc");
	// init restart udpate
	if (ILC_RestartUpdateInitFunc() == FALSE)
	{		
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdate - ILC_RestartUpdate");
	
	if (bNeedUpdate == TRUE)
	{
		if (ILC_RestartUpdate() == FALSE)
		{
			return FALSE;
		}

		if (IsCoverOpen() == TRUE)
		{
			SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
			return FALSE;
		}

		T_MoveTo(m_lPrePickPos_T);

		CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdate - ILC_AutoUpdateInitFunc");
	}
	
	// switch to ILC auto update mode
	if (ILC_AutoUpdateInitFunc() == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdate - Disable Data Log");

	// enable log again
	CycleEnableDataLog(TRUE, FALSE, FALSE);

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdate - ILC_UpdateContourMoveProfile");
	
	if (ILC_UpdateContourMoveProfile() == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_OpRestartUpdate - complete");

	m_ctILCBondingLastUpdateTime = CTime::GetCurrentTime();

	return TRUE;
}


BOOL CBondHead::ILC_RestartUpdate()
{
	BOOL bMoveDirection = TRUE;
	LONG lRtn = 0;
	CString szTemp;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdate - pick to bond");

	Z_MoveTo(m_lSwingLevel_Z);
	Z2_MoveTo(m_lSwingLevel_Z2);

	if (IsCoverOpen() == TRUE)
	{
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	T_MoveTo(m_lPickPos_T);
	Sleep(100);

	while(1)
	{
		if (IsCoverOpen() == TRUE)
		{
			SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
			return FALSE;
		}

		lRtn = ILC_RestartUpdateFunc(bMoveDirection);

		if (lRtn == BA_ILC_CONVERGE)
		{
			break;
		}

		if (lRtn == FALSE)
		{
			return FALSE;
		}

		bMoveDirection = !bMoveDirection;
	}
	return TRUE;
}


LONG CBondHead::ILC_RestartUpdateFunc(BOOL bMoveDirection)
{	

#ifdef ES101
	return BA_ILC_CONVERGE;	//v4.25	//Klocwork
#elif PROBER
	return BA_ILC_CONVERGE;
#else
#ifdef NU_MOTION
	CString szTitle, szContent;
	
	if (bMoveDirection)
	{
		if (ILC_LearnMove(m_stBAILCPTB, m_stBAILCPTB.bIsRestartUpdateConverged) == FALSE)
		{
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_RESTART_UPDATE_FAIL);
			szContent = szContent +  " (pick to bond)";
			HmiMessage_Red_Back(szContent, szTitle);

			CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateFunc - ILC_LearnMove fail");

			T_MoveTo(m_lPrePickPos_T);
			
			return FALSE;
		}

		if (agmp_ctrl_ILC_is_converged(&m_stBAILCPTB.ILCConfigInfo) == TRUE && m_stBAILCPTB.bIsRestartUpdateConverged == FALSE)
		{
			m_stBAILCPTB.bIsRestartUpdateConverged = TRUE;
			CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateFunc - ILC_SaveResult");

			if (agmp_ctrl_ILC_save(m_stBAILCPTB.ILCConfigInfo) != 0)
			{
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_BH_ILC_SAVE_RESULT_FAIL);
				szContent = szContent +  " (pick to bond)";
				HmiMessage_Red_Back(szTitle, szContent);
				T_MoveTo(m_lPrePickPos_T);
				
				return FALSE;
			}
		}
	}
	else
	{
		if (ILC_LearnMove(m_stBAILCBTP, m_stBAILCBTP.bIsRestartUpdateConverged) == FALSE)
		{
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_RESTART_UPDATE_FAIL);
			szContent = szContent +  " (bond to pick)";
			HmiMessage_Red_Back(szContent, szTitle);

			CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateFunc - ILC_LearnMove fail");

			T_MoveTo(m_lPrePickPos_T);
			
			return FALSE;
		}

		if (agmp_ctrl_ILC_is_converged(&m_stBAILCBTP.ILCConfigInfo) == TRUE && m_stBAILCBTP.bIsRestartUpdateConverged == FALSE)
		{
			m_stBAILCBTP.bIsRestartUpdateConverged = TRUE;
			CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_RestartUpdateFunc - ILC_SaveResult");

			if (agmp_ctrl_ILC_save(m_stBAILCBTP.ILCConfigInfo) != 0)
			{
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_BH_ILC_SAVE_RESULT_FAIL);
				szContent = szContent +  " (pick to bond)";
				HmiMessage_Red_Back(szTitle, szContent);
				T_MoveTo(m_lPrePickPos_T);
				
				return FALSE;
			}
		}
	}

	if (m_stBAILCBTP.bIsRestartUpdateConverged == TRUE && m_stBAILCPTB.bIsRestartUpdateConverged == TRUE)
	{
		T_MoveTo(m_lPrePickPos_T);
		
		return BA_ILC_CONVERGE;	
	}
	
	return BA_ILC_NOT_CONVERGE;	

#else
	return BA_ILC_CONVERGE;
#endif
#endif
	//return BA_ILC_CONVERGE;	//v4.25	//Klocwork
}


BOOL CBondHead::ILC_AutoUpdateInitFunc()
{

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateInitFunc - pick to bond");

	if (ILC_AutoUpdateInit(m_stBAILCPTB, BH_T_PICK_TO_BOND) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateInitFunc - bond to pick");

	if (ILC_AutoUpdateInit(m_stBAILCBTP, BH_T_BOND_TO_PICK) == FALSE)
	{
		return FALSE;
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateInitFunc - complete");

	return TRUE;
}

LONG CBondHead::ILC_AutoUpdateInit(ILC_STRUCT& stILC, LONG lBondHeadState)
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
	LONG lILCInitResult = 0;
	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateInit - start");

	stILC.ILCConfigInfo.ILCUpdateMode = ILC_MOVINGUPDATE;

	if ((lILCInitResult = agmp_ctrl_ILC_initialization(&stILC.ILCConfigInfo, m_bILCLog ? 1 : 0)) != 0)
	{
		CString szLog;
		szLog.Format("LC_AutoUpdateInit - agmp_ctrl_ILC_initialization fail:%d", lILCInitResult);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		return FALSE;
	}

	//stILC.ILCConfigInfo.ILCUpdateMode = ILC_MOVINGUPDATE;
	stILC.ApplyingCycle = 0;	
#endif
#endif
#endif
	return TRUE;
}

BOOL CBondHead::ILC_AutoUpdateInCycle(LONG lBondHeadState)
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
	if (m_bSel_T == FALSE)
	{
		return TRUE;
	}

	if ((m_BADataLog.bHasData == TRUE) && (lBondHeadState !=  m_BADataLog.lBondArmMode))
	{
		//v4.47T9
		BOOL bResult = TRUE;
		LONG lStatus = 0;

		if (m_BADataLog.lBondArmMode == BH_T_PICK_TO_BOND )
		{
//			if (m_BADataLog.bStartILCUpdate == FALSE)
			{
				CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateFuc - pick to bond (has data)");
				//TakeTime(LC1);
				TakeData(LC1, 999);
				bResult = ILC_AutoUpdateFromDataLog(m_stBAILCPTB, m_BADataLog, lStatus);
				TakeTime(LC2);
				//CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateFuc - ILC_AutoUpdateFromDataLog");
				m_BADataLog.bStartILCUpdate = TRUE;

				if (!bResult)	//v4.47T9	//MS60 debugging
				{
					CString szErr;
					szErr.Format("BH agmp_ctrl_ILC_update error: %ld", lStatus);
SetErrorMessage(szErr);

					if (lStatus == AGMP_CTRL_ERR_ILC_DATALOGLENGTH_SHORTER_THAN_UPDATELENGTH)
					{
						m_nILCAutoUpdateAccErrCount++;
						if (m_nILCAutoUpdateAccErrCount > 3)
						{
							m_nILCAutoUpdateAccErrCount = 0;
							return FALSE;
						}
					}
					else
					{
						m_nILCAutoUpdateAccErrCount = 0;
					}
				}
				else
				{
					m_nILCAutoUpdateAccErrCount = 0;
				}
			}
//			else
			{
				//TakeTime(LC3);
				TakeData(LC3, 999);
				ILC_RunTimeUpdateContourMoveProfile(m_BADataLog.lBondArmMode);
				TakeTime(LC4);
				//CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateFuc - ILC_RunTimeUpdateContourMoveProfile");
				m_stBAILCPTB./*ILCConfigInfo.*/ApplyingCycle = 0;
				m_BADataLog.bHasData = FALSE;
				m_BADataLog.bStartILCUpdate = FALSE;
				m_BADataLog.bEnable = FALSE;
			}
		}
		else if (m_BADataLog.lBondArmMode == BH_T_BOND_TO_PICK)
		{
//			if (m_BADataLog.bStartILCUpdate == FALSE)
			{
				CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateFuc - bond to pick (has data)");
				//TakeTime(LC1);
				TakeData(LC1, 888);
				bResult = ILC_AutoUpdateFromDataLog(m_stBAILCBTP, m_BADataLog, lStatus);
				TakeTime(LC2);
				//CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateFuc - ILC_AutoUpdateFromDataLog");
				m_BADataLog.bStartILCUpdate = TRUE;

				if (!bResult)	//v4.47T9	//MS60 debugging
				{
					CString szErr;
					szErr.Format("BH agmp_ctrl_ILC_update error: %ld", lStatus);
SetErrorMessage(szErr);

					if (lStatus == AGMP_CTRL_ERR_ILC_DATALOGLENGTH_SHORTER_THAN_UPDATELENGTH)
					{
						m_nILCAutoUpdateAccErrCount++;
						if (m_nILCAutoUpdateAccErrCount > 3)
						{
							m_nILCAutoUpdateAccErrCount = 0;
							return FALSE;
						}
					}
					else
					{
						m_nILCAutoUpdateAccErrCount = 0;
					}
				}
				else
				{
					m_nILCAutoUpdateAccErrCount = 0;
				}
			}
//			else
			{
				//TakeTime(LC3);
				TakeData(LC3, 888);
				ILC_RunTimeUpdateContourMoveProfile(m_BADataLog.lBondArmMode);
				TakeTime(LC4);

				//CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoUpdateFuc - ILC_RunTimeUpdateContourMoveProfile");
				m_stBAILCBTP./*ILCConfigInfo.*/ApplyingCycle = 0;
				m_BADataLog.bHasData = FALSE;
				m_BADataLog.bStartILCUpdate = FALSE;
				m_BADataLog.bEnable = FALSE;
			}
		}
	}
#endif
#endif
#endif

	return TRUE;
}


BOOL CBondHead::ILC_AutoUpdateFromDataLog(ILC_STRUCT& stILC, ILC_DATALOG_STRUCT& stBADataLog, LONG& lStatus)
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
	LONG lUpdateRtn;
	CString szLog;
	if ((lUpdateRtn = agmp_ctrl_ILC_update(&stILC.ILCConfigInfo, stBADataLog.nPos, stBADataLog.nEnc, stBADataLog.nNoOfSampleUpload, stBADataLog.nChannelMode)) != 0)
	{
		
		szLog.Format("ILC_AutoUpdate -  agmp_ctrl_ILC_update fail: %d", lUpdateRtn);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		lStatus = lUpdateRtn;		//v4.47T9
		return FALSE;
	}
	//szLog.Format("ILC Auto Update Result,ErrBand,%d,SettleTime,%f",stILC.ILCConfigInfo.CtrlErrBand,stILC.ILCConfigInfo.SettleTime);
	//CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
#endif
#endif
#endif

	return TRUE;
}

BOOL CBondHead::ILC_AutoLearnInitFunc(LONG lBondHeadState)
{
	BOOL bRtn = TRUE;
	CString szFilename;
	CString szPath;
	FLOAT fDistance;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoLearnInitFunc - start");

	if (lBondHeadState == BH_T_PICK_TO_BOND)
	{
		szFilename = BA_ILC_PTB_PROFILE_PATH;
		fDistance = (FLOAT)(m_lBondPos_T - m_lPickPos_T);
		
		bRtn = ILC_AutoLearnInit(m_stBAILCPTB, fDistance);
	}
	else if (lBondHeadState == BH_T_BOND_TO_PICK)
	{
		szFilename = BA_ILC_BTP_PROFILE_PATH;
		fDistance = (FLOAT)(m_lPickPos_T - m_lBondPos_T);

		bRtn = ILC_AutoLearnInit(m_stBAILCBTP, fDistance);
	}

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoLearnInitFunc - MotionGenerateFifthOrderProfileUsedForILC complete");

	return bRtn;
}

BOOL CBondHead::ILC_AutoLearnInit(ILC_STRUCT& stILC, FLOAT fDistance)
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
	LONG lILCInitRtn = 0, lLoadRtn = 0;

	stILC.bIsAutoLearnConverged = FALSE;

	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoLearnInit - Start");

	if (m_bILCSlowProfile)
	{
		if (CMS896AStn::MotionGenerateFifthOrderProfileUsedForILC(BH_AXIS_T, BH_MP_T_ILC_CONTOUR_SLOW, stILC.ILCConfigInfo, fDistance, &m_stBHAxis_T) == FALSE)
		{
			return FALSE;
		}
	}
	else
	{
		if (CMS896AStn::MotionGenerateFifthOrderProfileUsedForILC(BH_AXIS_T, BH_MP_T_ILC_CONTOUR, stILC.ILCConfigInfo, fDistance, &m_stBHAxis_T) == FALSE)
		{
			return FALSE;
		}
	}

	stILC.ILCConfigInfo.ILCUpdateMode = ILC_AUTOLEARN;

	if ((lILCInitRtn = agmp_ctrl_ILC_initialization(&stILC.ILCConfigInfo, m_bILCLog ? 1 : 0)) != 0)
	{
		CString szLog;
		szLog.Format("ILC_AutoLearnInit - agmp_ctrl_ILC_initialization fail:%d", lILCInitRtn);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		return FALSE;
	}

	// clear the init points
	for (int i=0; i<stILC.ILCConfigInfo.LearningLength; i++)
	{
		stILC.ILCConfigInfo.OutputList[i] = 0;
	}

	//stILC.ILCConfigInfo.ILCUpdateMode = ILC_AUTOLEARN
	lLoadRtn = 0;
	lLoadRtn = agmp_ctrl_ILC_load(&stILC.ILCConfigInfo);
	if (lLoadRtn  != 0)
	{
		CString szLog;
		szLog.Format("ILC_AutoLearnInit - CILC_Load fail:%d", lLoadRtn);
		CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
		return FALSE;
	}


	CMSLogFileUtility::Instance()->BH_ILCEventLog("ILC_AutoLearnInit - Complete");

#endif
#endif
#endif

	return TRUE;
}


//LONG CBondHead::ILC_AutoLearnFunc(LONG lBondHeadState)
LONG CBondHead::ILC_AutoLearnFunc()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
#ifdef ES101
	return TRUE;	//v4.25	//Klocwork	
#elif PROBER
	return TRUE;
#else
#ifdef NU_MOTION
	CString szTitle, szContent;
	LONG lSaveResult = 0;

	if (m_bMoveDirection)
	{
		if (IsCoverOpen() == TRUE)
		{
			SetErrorMessage("Machine Cover Open - Auto Learn ILC");
			//szTitle.LoadString(HMB_BH_ILC);
			//szContent.LoadString(HMB_BH_ILC_AUTO_LEARN_FAIL);
			//HmiMessage(szContent, szTitle);

			if (CheckCoverOpenInManual("Auto Learn Func") == TRUE)
			{
				T_MoveTo(m_lPrePickPos_T);
			}

			if (IsMS60() && m_bMS60ThermalCtrl)
			{	
				//m_bEnableMS60ThermalCheck = TRUE;	//v4.48A27
				EnableBHTThermalControl(TRUE);			//v4.50A9	
			}
			m_bILCAutoLearn = FALSE;
			pApp->EnableScreenButton(TRUE);

			return FALSE;
		}

		if (ILC_LearnMove(m_stBAILCPTB, m_stBAILCPTB.bIsAutoLearnConverged) == FALSE)
		{
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_AUTO_LEARN_FAIL);
			HmiMessage(szContent, szTitle);

			m_bILCAutoLearn = FALSE;
			pApp->EnableScreenButton(TRUE);

			if (CheckCoverOpenInManual("Auto Learn Func") == TRUE)
			{
				T_MoveTo(m_lPrePickPos_T);
			}

			if (IsMS60() && m_bMS60ThermalCtrl)
			{
				//m_bEnableMS60ThermalCheck = TRUE;	//v4.48A27
				EnableBHTThermalControl(TRUE);			//v4.50A9	
			}
			return FALSE;
		}

		if (agmp_ctrl_ILC_is_converged(&m_stBAILCPTB.ILCConfigInfo) == TRUE &&  m_stBAILCPTB.bIsAutoLearnConverged == FALSE)
		{
			m_stBAILCPTB.bIsAutoLearnConverged = TRUE;
			m_stBAILCPTB.bIsAutoLearnComplete = TRUE;

			if (agmp_ctrl_ILC_save(m_stBAILCPTB.ILCConfigInfo) != 0)
			{
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_BH_ILC_SAVE_RESULT_FAIL);
				HmiMessage(szContent, szTitle);
			}
		}

		if (m_stBAILCBTP.bIsAutoLearnConverged == TRUE && m_stBAILCPTB.bIsAutoLearnConverged == TRUE)
		{
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_CONVERGE);
			HmiMessage(szContent, szTitle);

			m_bILCAutoLearn = FALSE;
			pApp->EnableScreenButton(TRUE);

			if (CheckCoverOpenInManual("Auto Learn Func") == TRUE)
			{
				T_MoveTo(m_lPrePickPos_T);
			}

			if (IsMS60() && m_bMS60ThermalCtrl)
			{
				//m_bEnableMS60ThermalCheck = TRUE;		//v4.48A27
				EnableBHTThermalControl(TRUE);			//v4.50A9	
			}
			return TRUE;	
		}
	}
	else
	{
		if (IsCoverOpen() == TRUE)
		{
			//szTitle.LoadString(HMB_BH_ILC);
			//szContent.LoadString(HMB_BH_ILC_AUTO_LEARN_FAIL);
			//HmiMessage(szContent, szTitle);
			//m_bILCAutoLearn = FALSE;
			SetErrorMessage("Machine Cover Open - Auto Learn ILC");

			if (CheckCoverOpenInManual("Auto Learn Func") == TRUE)
			{
				T_MoveTo(m_lPrePickPos_T);
			}

			if (IsMS60() && m_bMS60ThermalCtrl)
			{
				//m_bEnableMS60ThermalCheck = TRUE;		//v4.48A27
				EnableBHTThermalControl(TRUE);			//v4.50A9	
			}
			m_bILCAutoLearn = FALSE;
			pApp->EnableScreenButton(TRUE);

			return FALSE;
		}

		if (ILC_LearnMove(m_stBAILCBTP, m_stBAILCBTP.bIsAutoLearnConverged) == FALSE)
		{
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_AUTO_LEARN_FAIL);
			HmiMessage(szContent, szTitle);

			m_bILCAutoLearn = FALSE;
			pApp->EnableScreenButton(TRUE);

			if (CheckCoverOpenInManual("Auto Learn Func") == TRUE)
			{
				T_MoveTo(m_lPrePickPos_T);
			}

			if (IsMS60() && m_bMS60ThermalCtrl)
			{
				//m_bEnableMS60ThermalCheck = TRUE;		//v4.48A27
				EnableBHTThermalControl(TRUE);			//v4.50A9	
			}
			return FALSE;
		}

		if (agmp_ctrl_ILC_is_converged(&m_stBAILCBTP.ILCConfigInfo) == TRUE && m_stBAILCBTP.bIsAutoLearnConverged == FALSE)
		{
			m_stBAILCBTP.bIsAutoLearnConverged = TRUE;
			m_stBAILCBTP.bIsAutoLearnComplete = TRUE;

			if (agmp_ctrl_ILC_save(m_stBAILCBTP.ILCConfigInfo) != 0)
			{
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_BH_ILC_SAVE_RESULT_FAIL);
				HmiMessage(szContent, szTitle);
			}
		}

		if (m_stBAILCBTP.bIsAutoLearnConverged == TRUE && m_stBAILCPTB.bIsAutoLearnConverged == TRUE)
		{
			szTitle.LoadString(HMB_BH_ILC);
			szContent.LoadString(HMB_BH_ILC_CONVERGE);
			HmiMessage(szContent, szTitle);
			m_bILCAutoLearn = FALSE;
			pApp->EnableScreenButton(TRUE);

			if (CheckCoverOpenInManual("Auto Learn Func") == TRUE)
			{
				T_MoveTo(m_lPrePickPos_T);
			}

			if (IsMS60() && m_bMS60ThermalCtrl)
			{
				//m_bEnableMS60ThermalCheck = TRUE;		//v4.48A27
				EnableBHTThermalControl(TRUE);			//v4.50A9	
			}
			return TRUE;	
		}
	}

	m_bMoveDirection = !m_bMoveDirection;

	if (m_lILCAutoLearnDelay > 0)
	{
		Sleep(m_lILCAutoLearnDelay);
	}

	Sleep(50);
	return BA_ILC_NOT_CONVERGE;
#else
	return TRUE;
#endif
#endif
	//return TRUE;	//v4.25	//Klocwork	
}

LONG CBondHead::ILC_LearnWarmCoolMove(ILC_STRUCT& stILC, ILC_DATALOG_STRUCT *pstDataLog, LONG lMode)
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	if (lMode == ILC_WARM)
	{
		MotionSetContourProfile(BH_AXIS_T, stILC.ILCConfigInfo.PList, stILC.ILCConfigInfo.VList, stILC.ILCConfigInfo.AList, 
			stILC.ILCConfigInfo.JJWarmList, stILC.ILCConfigInfo.LearningLength, 0 , &m_stBHAxis_T);
	}
	else
	{
		MotionSetContourProfile(BH_AXIS_T, stILC.ILCConfigInfo.PList, stILC.ILCConfigInfo.VList, stILC.ILCConfigInfo.AList, 
			stILC.ILCConfigInfo.JJCoolList, stILC.ILCConfigInfo.LearningLength, 0 , &m_stBHAxis_T);
	}

	GetNuMotionDataLogData(BH_AXIS_T, m_stBHAxis_T, TRUE, pstDataLog->nNoOfSampleUpload, pstDataLog->nPos, pstDataLog->nEnc, pstDataLog->nChannelMode);

	if (T_ContourMove((INT)stILC.ILCConfigInfo.PList[stILC.ILCConfigInfo.LearningLength -1], stILC.ILCConfigInfo.LearningLength,
		0 ,SFM_WAIT, TRUE) != gnOK)
	{
		return FALSE;
	}
	
	Sleep(10);
	
	GetNuMotionDataLogData(BH_AXIS_T, m_stBHAxis_T, FALSE, pstDataLog->nNoOfSampleUpload, pstDataLog->nPos, pstDataLog->nEnc, pstDataLog->nChannelMode);

#endif
#endif
#endif

	return TRUE;
}

LONG CBondHead::ILC_LearnMove(ILC_STRUCT& stILC, BOOL bIsConverged)
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	CString szNuControlParaID;
	INT *nPos = new INT [0x8000];
	INT *nEnc = new INT [0x8000];
	INT *nChannelMode = new INT [0x8000];
	INT nNoOfSampleUploaded;
	LONG lLearnRetn;
	//NewILCPara
	MotionSetContourProfile(BH_AXIS_T, stILC.ILCConfigInfo.PList, stILC.ILCConfigInfo.VList, stILC.ILCConfigInfo.AList, 
		stILC.ILCConfigInfo.JJList, stILC.ILCConfigInfo.LearningLength, 0 , &m_stBHAxis_T);

	GetNuMotionDataLogData(BH_AXIS_T, m_stBHAxis_T, TRUE, nNoOfSampleUploaded, nPos, nEnc, nChannelMode);

	if (T_ContourMove((INT)stILC.ILCConfigInfo.PList[stILC.ILCConfigInfo.LearningLength -1], stILC.ILCConfigInfo.LearningLength, 0 ,SFM_WAIT, TRUE) != gnOK)
	{
		if (nPos != NULL)
		{
			delete [] nPos;
		}
		if (nEnc != NULL)
		{
			delete [] nEnc;
		}
		if (nChannelMode != NULL)
		{
			delete [] nChannelMode;
		}
		return FALSE;
	}
	
	Sleep(10);
	
	GetNuMotionDataLogData(BH_AXIS_T, m_stBHAxis_T, FALSE, nNoOfSampleUploaded, nPos, nEnc, nChannelMode);

	if (bIsConverged == FALSE)
	{
		if ((lLearnRetn = agmp_ctrl_ILC_learn(&stILC.ILCConfigInfo, nPos, nEnc, nNoOfSampleUploaded, nChannelMode)) != 0)
		{
			if (nPos != NULL)
			{
				delete [] nPos;
			}
			if (nEnc != NULL)
			{
				delete [] nEnc;
			}
			if (nChannelMode != NULL)
			{
				delete [] nChannelMode;
			}
			CString szLog;
			szLog.Format("ILC_AutoLearnMove - agmp_ctrl_ILC_learn:%d", lLearnRetn);
			CMSLogFileUtility::Instance()->BH_ILCEventLog(szLog);
			return FALSE;
		}
	}

	if (nPos != NULL)
	{
		delete [] nPos;
	}
	if (nEnc != NULL)
	{
		delete [] nEnc;
	}
	if (nChannelMode != NULL)
	{
		delete [] nChannelMode;
	}

#endif
#endif
#endif
	return TRUE;
}

VOID CBondHead::ILC_AutoUpdateApplyingCycleCountInc()
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	m_stBAILCPTB./*ILCConfigInfo.*/ApplyingCycle = m_stBAILCPTB./*ILCConfigInfo.*/ApplyingCycle + 1;
	m_stBAILCBTP./*ILCConfigInfo.*/ApplyingCycle = m_stBAILCBTP./*ILCConfigInfo.*/ApplyingCycle + 1;
#endif
#endif
#endif
}


BOOL CBondHead::ILC_AutoUpdateApplyingCycleHitLimit()
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	if (m_stBAILCPTB./*ILCConfigInfo.*/ApplyingCycle >= m_stBAILCPTB.ILCConfigInfo.ApplyingCycleLmt * 2)
	{
		return TRUE;
	}

	if (m_stBAILCBTP./*ILCConfigInfo.*/ApplyingCycle >= m_stBAILCBTP.ILCConfigInfo.ApplyingCycleLmt * 2)
	{
		return TRUE;
	}

#endif
#endif
#endif

	return FALSE;
}


BOOL CBondHead::ILC_LoadParameters(ILC_STRUCT& stILC, LONG lBondHeadState)
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	CStringMapFile	ILCInfo; 

	if (ILCInfo.Open(gszROOT_DIRECTORY + "\\Exe\\NuData\\ILC\\ILC.msd", FALSE, TRUE) != 1)
	{
		return FALSE;
	}

	stILC.ILCConfigInfo.NumOfNotch = (INT)((LONG)(ILCInfo)["NumOfNotch"]);
	stILC.ILCConfigInfo.LearningLength = (INT)((LONG)(ILCInfo)["LearnedLength"]);

	if (stILC.ILCConfigInfo.LearningLength <= 0 || stILC.ILCConfigInfo.NumOfNotch <= 0)
	{
		return FALSE;
	}

	stILC./*ILCConfigInfo.*/ChannelNo = (INT)((LONG)(ILCInfo)["ChannelNo"]);
	stILC.ILCConfigInfo.SampleRate = (INT)((LONG)(ILCInfo)["SampleRate"]);

	stILC.ILCConfigInfo.AttenFactor = (FLOAT)((DOUBLE)(ILCInfo)["AttenFactor"]);
	stILC.ILCConfigInfo.CtrlErrBand =  (INT)(10/*(LONG)(ILCInfo)["ErrBand"]*/);
	stILC.ILCConfigInfo.PGain = (FLOAT)((DOUBLE)(ILCInfo)["PGain"]);
	stILC.ILCConfigInfo.DGain = (FLOAT)((DOUBLE)(ILCInfo)["DGain"]);

	stILC.ILCConfigInfo.ShiftSample = (INT)((LONG)(ILCInfo)["ShiftSample"]);

	stILC.ILCConfigInfo.GFilterF1 = (FLOAT)((DOUBLE)(ILCInfo)["F1"]);
	stILC.ILCConfigInfo.GFilterF2 = (FLOAT)((DOUBLE)(ILCInfo)["F2"]);
	stILC.ILCConfigInfo.GFilterDepth = (FLOAT)((DOUBLE)(ILCInfo)["GDepth"]);
	
	stILC.ILCConfigInfo.NotchFcList = new FLOAT[stILC.ILCConfigInfo.NumOfNotch];
	stILC.ILCConfigInfo.NotchBwList = new FLOAT[stILC.ILCConfigInfo.NumOfNotch];
	stILC.ILCConfigInfo.NotchAttenList = new FLOAT[stILC.ILCConfigInfo.NumOfNotch];
	
	stILC.ILCConfigInfo.NotchFcList[0] = (FLOAT)((DOUBLE)(ILCInfo)["NotchFc"]);
	stILC.ILCConfigInfo.NotchBwList[0] = (FLOAT)((DOUBLE)(ILCInfo)["NotchBw"]);
	stILC.ILCConfigInfo.NotchAttenList[0] = (FLOAT)((DOUBLE)(ILCInfo)["NotchAtten"]);

	stILC.ILCConfigInfo.NotchFcList[1] = (FLOAT)((DOUBLE)(ILCInfo)["NotchFc2"]);
	stILC.ILCConfigInfo.NotchBwList[1] = (FLOAT)((DOUBLE)(ILCInfo)["NotchBw2"]);
	stILC.ILCConfigInfo.NotchAttenList[1] = (FLOAT)((DOUBLE)(ILCInfo)["NotchAtten2"]);

	//ILC settings for convergence search
	stILC.ILCConfigInfo.NormType = (CTRL_ILC_NORM_TYPE)((LONG)(ILCInfo)["NormType"]);
	stILC.ILCConfigInfo.WindowSize = (INT)((LONG)(ILCInfo)["WindowSize"]);
	stILC.ILCConfigInfo.ConvgMargin = (FLOAT)((DOUBLE)(ILCInfo)["ConvgMargin"]);
	stILC.ILCConfigInfo.DivgMargin = (FLOAT)((DOUBLE)(ILCInfo)["DivgMargin"]);

	stILC.ILCConfigInfo.ConvgCntLmt = (INT)((LONG)(ILCInfo)["ConvgCntLmt"]);
	stILC.ILCConfigInfo.SrchCycleLmt = (INT)((LONG)(ILCInfo)["SrchCycleLmt"]);
	stILC.ILCConfigInfo.ForcedUpdating = (INT)((LONG)(ILCInfo)["ForcedUpdating"]);

	stILC.ILCConfigInfo.ILCEnableMode = (CTRL_ILC_ENABLE_TYPE)((LONG)(ILCInfo)["EnableMode"]);
	stILC.ILCConfigInfo.ILCUpdateMode = (CTRL_ILC_UPDATE_TYPE)0;

	//stILC.ILCConfigInfo.SetSettlingTime = (FLOAT)(DOUBLE)(ILCInfo)["SetSettlingTime"];
	stILC.ILCConfigInfo.PerfBound = (FLOAT)((DOUBLE)(ILCInfo)["PerfBound"]);
	stILC.ILCConfigInfo.AbsOptAvIdx = (DOUBLE)(ILCInfo)["AbsOptAvIdx"];
	stILC.ILCConfigInfo.FewNumber = (INT)(LONG)(ILCInfo)["FewNumber"];

	stILC.ILCConfigInfo.ApplyingLength =  (INT)((LONG)(ILCInfo)["ApplyingLength"]);
	stILC.ILCConfigInfo.ApplyingCycleLmt =  (INT)((LONG)(ILCInfo)["ApplyingCycleLmt"]);
	stILC./*ILCConfigInfo.*/ApplyingUpdateCount =  (INT)((LONG)(ILCInfo)["ApplyingUpdateCount"]);

	stILC.ILCConfigInfo.PIDCtrl.CtrlKp = (FLOAT)((DOUBLE)(ILCInfo)["CtrlKp"]);
	stILC.ILCConfigInfo.PIDCtrl.CtrlKd = (FLOAT)((DOUBLE)(ILCInfo)["CtrlKd"]);
	
	stILC.ILCConfigInfo.InputList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	stILC.ILCConfigInfo.OutputList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	stILC.ILCConfigInfo.PList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	stILC.ILCConfigInfo.VList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	stILC.ILCConfigInfo.AList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	stILC.ILCConfigInfo.JList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	stILC.ILCConfigInfo.JJList = new FLOAT[stILC.ILCConfigInfo.LearningLength];//NewILCPara
	stILC.ILCConfigInfo.CtrollerFlag = ILC_PIDCTRL_FLAG;
	stILC.ILCConfigInfo.OrigFFC.Kj = 1;
	stILC.ILCConfigInfo.JJWarmList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	stILC.ILCConfigInfo.JJCoolList = new FLOAT[stILC.ILCConfigInfo.LearningLength];
	strcpy_s(stILC.ILCConfigInfo.ILCDirectory, sizeof(stILC.ILCConfigInfo.ILCDirectory), "c:\\mapsorter\\exe\\");

	for (INT i=0; i<stILC.ILCConfigInfo.LearningLength; i++)
	{
		stILC.ILCConfigInfo.InputList[i] = 0;
		stILC.ILCConfigInfo.OutputList[i] = 0;
		stILC.ILCConfigInfo.PList[i] = 0;
		stILC.ILCConfigInfo.VList[i] = 0;
		stILC.ILCConfigInfo.AList[i] = 0;
		stILC.ILCConfigInfo.JList[i] = 0;
		stILC.ILCConfigInfo.JJList[i] = 0;
		stILC.ILCConfigInfo.JJWarmList[i] = 0;
		stILC.ILCConfigInfo.JJCoolList[i] = 0;
	}
	
	stILC.ApplyingCycle = 0;
	stILC.ILCConfigInfo.NodeNo = lBondHeadState;
	ILCInfo.Close();

	ILC_CalculateApplyLength(stILC, lBondHeadState);	//v4.47T3

#endif
#endif
#endif

	return TRUE;	
}

VOID CBondHead::ILC_SaveAllResult()
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	
	if (IsEnableILC() == FALSE)
	{
		return;
	}

	agmp_ctrl_ILC_save(m_stBAILCPTB.ILCConfigInfo);
	agmp_ctrl_ILC_save(m_stBAILCBTP.ILCConfigInfo);

#endif
#endif
#endif

}

BOOL CBondHead::ILC_IsHitDataLogLimit(LONG lBondHeadState)
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	
	if (m_bSel_T == FALSE)
	{
		return FALSE;
	}

	if (lBondHeadState == BH_T_PICK_TO_BOND)
	{
		if ((m_stBAILCPTB./*ILCConfigInfo.*/ApplyingCycle >= (m_stBAILCPTB.ILCConfigInfo.ApplyingCycleLmt - BA_ILC_DATALOG_OFFSET)))
		{
			return TRUE;
		}
	}
	// bond to pick
	else if (lBondHeadState == BH_T_BOND_TO_PICK )
	{
		if ((m_stBAILCBTP./*ILCConfigInfo*/ApplyingCycle >= (m_stBAILCBTP.ILCConfigInfo.ApplyingCycleLmt - BA_ILC_DATALOG_OFFSET)))
		{
			return TRUE;
		}
		
	}
	else if (lBondHeadState == BH_T_PREPICK_TO_PICK)
	{
		return FALSE;
	}
	else
	{
		return FALSE;
	}

#endif
#endif
#endif

	return FALSE;
}

BOOL CBondHead::ILC_LogRunTimeUpdateData(LONG lBondHeadState)
{	
	if ( m_BADataLog.bEnable == FALSE)
	{
		if (ILC_IsHitDataLogLimit(lBondHeadState) == TRUE)
		{
			//v4.53A12
			//1. Start ILC data logging; disable data log of other axes if currently enabled
			//		- m_BADataLog.bEnable is TRUE after this fcn call
			ILC_RunTimeDataLog(lBondHeadState, FALSE);

			if (lBondHeadState == BH_T_BOND_TO_PICK)
				TakeData(LC1, 111);
			else
				TakeData(LC1, 222);
		}
	}
	else
	{
		if (m_BADataLog.bHasData == FALSE)
		{
			if (lBondHeadState != m_BADataLog.lBondArmMode)
			{
				if (ILC_IsHitDataLogLimit(m_BADataLog.lBondArmMode) == TRUE)
				{
					//v4.53A12
					//2. Stop ILC data logging in next next cycle; re-enable data log of other axes
					//		- m_BADataLog.bHasData is set to TRUE after this fcn call
					ILC_RunTimeDataLog(lBondHeadState, TRUE);

					if (lBondHeadState == BH_T_BOND_TO_PICK)
						TakeData(LC3, 111);
					else
						TakeData(LC3, 222);
				}
			}
		}
	}

	return TRUE;
}

BOOL CBondHead::ILC_RunTimeDataLog(LONG lBondHeadState, BOOL bEnable)
{
	if (bEnable == TRUE)
	{
		GetNuMotionDataLogData(BH_AXIS_T, m_stBHAxis_T, FALSE, m_BADataLog.nNoOfSampleUpload, m_BADataLog.nPos, m_BADataLog.nEnc, m_BADataLog.nChannelMode);

		m_BADataLog.bHasData = TRUE;
		m_BADataLog.bStartILCUpdate = FALSE;
		
		CycleEnableDataLog(TRUE, FALSE, FALSE);
		return TRUE;
	}
	else
	{
		
		CycleEnableDataLog(FALSE, FALSE, FALSE);
		m_BADataLog.bEnable = TRUE;
		m_BADataLog.nNoOfSampleUpload = 0;
		m_BADataLog.lBondArmMode = lBondHeadState;
		GetNuMotionDataLogData(BH_AXIS_T, m_stBHAxis_T, TRUE, m_BADataLog.nNoOfSampleUpload, m_BADataLog.nPos, m_BADataLog.nEnc, m_BADataLog.nChannelMode);
		

		return TRUE;
	}

	return TRUE;
}


VOID CBondHead::ILC_CleanDataLog()
{
	if (m_BADataLog.bEnable == TRUE)
	{
		GetNuMotionDataLogData(BH_AXIS_T, m_stBHAxis_T, FALSE, m_BADataLog.nNoOfSampleUpload, m_BADataLog.nPos, m_BADataLog.nEnc, m_BADataLog.nChannelMode);

		m_BADataLog.bEnable = FALSE;
		m_BADataLog.bHasData = FALSE;
		m_BADataLog.bStartILCUpdate = FALSE;
		m_BADataLog.nNoOfSampleUpload = 0;
		m_BADataLog.lBondArmMode = BH_NA;
	}
}

BOOL CBondHead::ILC_ContourMoveTest()
{
	/*
	CString szContent, szTitle;

	if (IsCoverOpen())
	{
		SetErrorMessage("Open Machine Cover in CT Move Test");
		SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
		return FALSE;
	}

	// pick to bond
	if (m_lMotionTestDist == BH_T_PICK_TO_BOND)
	{
		if (m_bMoveDirection)
		{
			//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);	
			
			if (BA_MoveTo(m_lBondPos_T, SFM_WAIT)!= gnOK)
			{
				LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);

				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_ILC_MOVE_TEST_FAIL);
				HmiMessage(szTitle, szContent);
				
				return FALSE;
			}

			//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
		}
		else
		{
			if (BA_MoveTo(m_lPickPos_T, SFM_WAIT)!= gnOK)
			{
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_ILC_MOVE_TEST_FAIL);
				HmiMessage(szTitle, szContent);
				return FALSE;
			}
		}
	}
	// bond to pick
	else if (m_lMotionTestDist == BH_T_BOND_TO_PICK)
	{
		if (m_bMoveDirection)
		{
			if (BA_MoveTo(m_lPickPos_T, SFM_WAIT)!= gnOK)
			{
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_ILC_MOVE_TEST_FAIL);
				HmiMessage(szTitle, szContent);
				return FALSE;
			}
		}
		else
		{
			//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);	

			if (BA_MoveTo(m_lBondPos_T, SFM_WAIT)!= gnOK)
			{
				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_ILC_MOVE_TEST_FAIL);
				HmiMessage(szTitle, szContent);
				
				return FALSE;
			}

			//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);	

		}
	}
	else if (m_lMotionTestDist == BH_T_PREPICK_TO_PICK)
	{
		if (m_bMoveDirection)
		{
			//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, TRUE, 10, m_stDataLog);

			if (BA_MoveTo(m_lPickPos_T, SFM_WAIT)!= gnOK)
			{
				LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_ILC_MOVE_TEST_FAIL);
				HmiMessage(szTitle, szContent);

				return FALSE;
			}

			//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);	
		}
		else
		{	
			T_MoveTo(m_lPrePickPos_T);
		}
	}
	else
	{
		if (m_bMoveDirection)
		{
			if (BA_MoveTo(m_lBondPos_T, SFM_WAIT)!= gnOK)
			{
				//LogAxisPerformance(BH_AXIS_T, &m_stBHAxis_T, FALSE, 10, m_stDataLog);
				szTitle.LoadString(HMB_BH_ILC);
				szContent.LoadString(HMB_ILC_MOVE_TEST_FAIL);
				HmiMessage(szTitle, szContent);

				return FALSE;
			}
		}
		else
		{	
			T_MoveTo(m_lPrePickPos_T);
		}
	}
	*/
	return TRUE;
}

LONG CBondHead::ILC_GetContourMoveStartOffset(LONG lBondHeadState)
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	
	if (lBondHeadState == BH_T_PICK_TO_BOND)
	{
		return 0;
	}
	else
	{
		return m_stBAILCPTB.ILCConfigInfo.LearningLength;
	}
	/*
	else if (lBondHeadState == BH_T_PREPICK_TO_PICK)
	{
		return m_stBAILCPTB.ILCConfigInfo.LearningLength + m_stBAILCBTP.ILCConfigInfo.LearningLength;
	}
	else
	{
		return m_stBAILCPTB.ILCConfigInfo.LearningLength + m_stBAILCBTP.ILCConfigInfo.LearningLength +
			m_stBAILCPrePTP.ILCConfigInfo.LearningLength;
	}
	*/

#endif
#endif
#endif
	
	return 0;
}

BOOL CBondHead::ILC_RunTimeUpdateContourMoveProfile(LONG lBondHeadUpdateState)
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
	LONG lOffset = 0;

	if (lBondHeadUpdateState == BH_T_PICK_TO_BOND)
	{
		lOffset = ILC_GetContourMoveStartOffset(BH_T_PICK_TO_BOND);

		for (INT i=0; i<m_stBAILCPTB.ILCConfigInfo.LearningLength; i++ )
		{
			m_BAMove.fMovePos[i] = m_stBAILCPTB.ILCConfigInfo.PList[i];
			m_BAMove.fMoveVel[i] = m_stBAILCPTB.ILCConfigInfo.VList[i];
			m_BAMove.fMoveAcc[i] = m_stBAILCPTB.ILCConfigInfo.AList[i];
			m_BAMove.fMoveJerk[i] = m_stBAILCPTB.ILCConfigInfo.JJList[i];
		}

		if (T_SetContourProfile(m_BAMove.fMovePos, m_BAMove.fMoveVel, m_BAMove.fMoveAcc, m_BAMove.fMoveJerk, m_stBAILCPTB.ILCConfigInfo.LearningLength, lOffset) != gnOK)
		{
			return FALSE;
		}		
	}
	else if (lBondHeadUpdateState == BH_T_BOND_TO_PICK)
	{
		lOffset = ILC_GetContourMoveStartOffset(BH_T_BOND_TO_PICK);

		for (INT i=0; i<m_stBAILCBTP.ILCConfigInfo.LearningLength; i++ )
		{
			m_BAMove.fMovePos[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.PList[i];
			m_BAMove.fMoveVel[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.VList[i];
			m_BAMove.fMoveAcc[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.AList[i];
			m_BAMove.fMoveJerk[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.JJList[i];
		}

		if (T_SetContourProfile(m_BAMove.fMovePos, m_BAMove.fMoveVel, m_BAMove.fMoveAcc, m_BAMove.fMoveJerk, m_stBAILCBTP.ILCConfigInfo.LearningLength, lOffset) != gnOK)
		{
			return FALSE;
		}
	}

#endif
#endif
#endif

	return TRUE;
}

// update for running contour profile
// merge all contour move into one

BOOL CBondHead::ILC_UpdateContourMoveProfile()
{

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	LONG lOffset = 0;
	LONG lProfileSize = 0;

	LONG lTotalLength = m_stBAILCPTB.ILCConfigInfo.LearningLength + m_stBAILCBTP.ILCConfigInfo.LearningLength;

	if (lTotalLength > BA_CONTOUR_MOVE_BUF_SIZE)
	{
		return FALSE;
	}

	lOffset = ILC_GetContourMoveStartOffset(BH_T_PICK_TO_BOND);

	for (INT i=0; i<m_stBAILCPTB.ILCConfigInfo.LearningLength; i++ )
	{
		m_BAMove.fMovePos[i] = m_stBAILCPTB.ILCConfigInfo.PList[i];
		m_BAMove.fMoveVel[i] = m_stBAILCPTB.ILCConfigInfo.VList[i];
		m_BAMove.fMoveAcc[i] = m_stBAILCPTB.ILCConfigInfo.AList[i];
		m_BAMove.fMoveJerk[i] = m_stBAILCPTB.ILCConfigInfo.JJList[i];

		lProfileSize = lProfileSize + 1;
	}

	if (T_SetContourProfile(m_BAMove.fMovePos, m_BAMove.fMoveVel, m_BAMove.fMoveAcc, m_BAMove.fMoveJerk, m_stBAILCPTB.ILCConfigInfo.LearningLength, lOffset) != gnOK)
	{
		return FALSE;
	}

	lOffset = ILC_GetContourMoveStartOffset(BH_T_BOND_TO_PICK);

	for (INT i=0; i<m_stBAILCBTP.ILCConfigInfo.LearningLength; i++ )
	{
		m_BAMove.fMovePos[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.PList[i];
		m_BAMove.fMoveVel[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.VList[i];
		m_BAMove.fMoveAcc[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.AList[i];
		m_BAMove.fMoveJerk[lOffset + i] = m_stBAILCBTP.ILCConfigInfo.JJList[i];

		lProfileSize = lProfileSize + 1;
	}

	if (T_SetContourProfile(m_BAMove.fMovePos, m_BAMove.fMoveVel, m_BAMove.fMoveAcc, m_BAMove.fMoveJerk, m_stBAILCBTP.ILCConfigInfo.LearningLength, lOffset) != gnOK)
	{
		return FALSE;
	}

#endif
#endif
#endif

/*
	lOffset = lOffset + m_stBAILCBTP.ILCConfigInfo.LearningLength;

	for (INT i=0; i<m_stBAILCPrePTP.ILCConfigInfo.LearningLength; i++ )
	{
		m_BAMove.fMovePos[lOffset + i] = m_stBAILCPrePTP.ILCConfigInfo.PList[i];
		m_BAMove.fMoveVel[lOffset + i] = m_stBAILCPrePTP.ILCConfigInfo.VList[i];
		m_BAMove.fMoveAcc[lOffset + i] = m_stBAILCPrePTP.ILCConfigInfo.AList[i];
		m_BAMove.fMoveJerk[lOffset + i] = m_stBAILCPrePTP.ILCConfigInfo.JList[i];

		lProfileSize = lProfileSize + 1;
	}

	lOffset = lOffset + m_stBAILCPrePTP.ILCConfigInfo.LearningLength;

	for (INT i=0; i<m_stBAILCPrePTB.ILCConfigInfo.LearningLength; i++ )
	{
		m_BAMove.fMovePos[lOffset + i] = m_stBAILCPrePTB.ILCConfigInfo.PList[i];
		m_BAMove.fMoveVel[lOffset + i] = m_stBAILCPrePTB.ILCConfigInfo.VList[i];
		m_BAMove.fMoveAcc[lOffset + i] = m_stBAILCPrePTB.ILCConfigInfo.AList[i];
		m_BAMove.fMoveJerk[lOffset + i] = m_stBAILCPrePTB.ILCConfigInfo.JList[i];

		lProfileSize = lProfileSize + 1;
	}

	if (T_SetContourProfile(m_BAMove.fMovePos, m_BAMove.fMoveVel, m_BAMove.fMoveAcc, m_BAMove.fMoveJerk, lProfileSize, 0) != gnOK)
	{
		return FALSE;
	}
	*/

	return TRUE;
}

VOID CBondHead::ILC_Clean()
{
	ILC_FreeStructMemory(m_stBAILCPTB);
	ILC_FreeStructMemory(m_stBAILCBTP);
	//ILC_FreeStructMemory(m_stBAILCPrePTP);
	//ILC_FreeStructMemory(m_stBAILCPrePTB);
}


VOID CBondHead::ILC_FreeStructMemory(ILC_STRUCT& stILC)
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION 
	if (stILC.ILCConfigInfo.InputList != NULL)
	{
		delete [] stILC.ILCConfigInfo.InputList;
	}

	if (stILC.ILCConfigInfo.OutputList != NULL)
	{
		delete [] stILC.ILCConfigInfo.OutputList ;
	}

	if (stILC.ILCConfigInfo.PList != NULL)
	{
		delete [] stILC.ILCConfigInfo.PList;
	}

	if (stILC.ILCConfigInfo.VList != NULL)
	{
		delete [] stILC.ILCConfigInfo.VList;
	}

	if (stILC.ILCConfigInfo.AList != NULL)
	{
		delete [] stILC.ILCConfigInfo.AList;
	}

	if (stILC.ILCConfigInfo.JList != NULL)
	{
		delete [] stILC.ILCConfigInfo.JList;
	}
	if (stILC.ILCConfigInfo.JJList != NULL)
	{
		delete [] stILC.ILCConfigInfo.JJList;
	}
	if (stILC.ILCConfigInfo.JJWarmList != NULL)
	{
		delete [] stILC.ILCConfigInfo.JJWarmList;
	}

	if (stILC.ILCConfigInfo.JJCoolList != NULL)
	{
		delete [] stILC.ILCConfigInfo.JJCoolList;
	}

#endif
#endif
#endif

}

BOOL CBondHead::ILC_LogRunningContourPoints(FLOAT* fPos, FLOAT* fVel, FLOAT* fAcc, 
										FLOAT* fJerk, UINT uiNoofPoint)
{
	CStdioFile cfContourLogFile;
	CString szStr;

	if ( cfContourLogFile.Open("C:\\MapSorter\\Exe\\NuData\\ILC\\CurrentMoving.csv", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite) == FALSE )
	{
		return FALSE;
	}

	cfContourLogFile.SeekToEnd();
	cfContourLogFile.WriteString("\nPos,Vel,Acc,Jerk\n");

	for (INT i=0; i<(INT)uiNoofPoint; i++)
	{
		szStr.Format("%f,%f,%f,%f", fPos[i], fVel[i], fAcc[i], fJerk[i]);
		cfContourLogFile.WriteString(szStr + "\n");
	}
	
	cfContourLogFile.Close();

	return TRUE;
}

VOID CBondHead::ILC_LogStructData(ILC_STRUCT& stILC)
{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
	CString szTemp;

	CMSLogFileUtility::Instance()->BH_ILCLog("\n");

	szTemp.Format("Channel No: %d", stILC./*ILCConfigInfo.*/ChannelNo);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("SampleRate: %d", stILC.ILCConfigInfo.SampleRate);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("AttenFactor: %.5f", stILC.ILCConfigInfo.AttenFactor);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("LearningGrain: %.5f", stILC.ILCConfigInfo.PGain);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("LearningDGrain: %.5f", stILC.ILCConfigInfo.DGain );
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("GFilterF1: %.5f", stILC.ILCConfigInfo.GFilterF1 );
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("GFilterF2: %.5f", stILC.ILCConfigInfo.GFilterF2 );
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("GFilterDepth: %.5f", stILC.ILCConfigInfo.GFilterDepth);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NormType: %.5f", stILC.ILCConfigInfo.NormType);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);
	
	szTemp.Format("WindowSize: %d", stILC.ILCConfigInfo.WindowSize);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ConvgMargin: %.5f", stILC.ILCConfigInfo.ConvgMargin);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("DivgMargin: %.5f", stILC.ILCConfigInfo.DivgMargin);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);
	
	szTemp.Format("ConvgCntLmt: %d", stILC.ILCConfigInfo.ConvgCntLmt);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("SrchCycleLmt: %d", stILC.ILCConfigInfo.SrchCycleLmt);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ForcedUpdating: %d", stILC.ILCConfigInfo.ForcedUpdating);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NumOfNotch: %d", stILC.ILCConfigInfo.NumOfNotch);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("LearningLength: %d", stILC.ILCConfigInfo.LearningLength);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ShiftSample: %d", stILC.ILCConfigInfo.ShiftSample);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("CtrlKp: %.5f", stILC.ILCConfigInfo.PIDCtrl.CtrlKp);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("CtrlKd: %.5f", stILC.ILCConfigInfo.PIDCtrl.CtrlKd);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NotchFcList: %.5f", stILC.ILCConfigInfo.NotchFcList[0]);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NotchFcList1: %.5f", stILC.ILCConfigInfo.NotchFcList[1]);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NotchBwList: %.5f", stILC.ILCConfigInfo.NotchBwList[0]);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NotchBwList1: %.5f", stILC.ILCConfigInfo.NotchBwList[1]);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NotchAttenList: %.5f", stILC.ILCConfigInfo.NotchAttenList[0]);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("NotchAttenList1: %.5f", stILC.ILCConfigInfo.NotchAttenList[1]);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	//szTemp.Format("ILC Enable Mode: %d", stILC.ILCConfigInfo.ILCEnableMode );
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ILC Update Mode: %d", stILC.ILCConfigInfo.ILCUpdateMode);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ILC Applying Length: %d", stILC.ILCConfigInfo.ApplyingLength);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ILC Applying Cycle Lmt: %d", stILC.ILCConfigInfo.ApplyingCycleLmt);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ILC Applying ApplyingCycle: %d", stILC.ApplyingCycle);
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	szTemp.Format("ILC ApplyingUpdateCount: %d", stILC./*ILCConfigInfo.*/ApplyingUpdateCount );
	CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);

	for (INT i=0; i<stILC.ILCConfigInfo.LearningLength; i++)
	{
		szTemp.Format("PList:%f, VList:%f, AList:%f, JList:%f, JJList:%f", stILC.ILCConfigInfo.PList[i], stILC.ILCConfigInfo.VList[i], 
			stILC.ILCConfigInfo.AList[i], stILC.ILCConfigInfo.JList[i], stILC.ILCConfigInfo.JJList[i]);
		CMSLogFileUtility::Instance()->BH_ILCLog(szTemp);
	}

#endif
#endif
#endif

}

BOOL CBondHead::ILC_CalculateApplyLength(ILC_STRUCT& stILC, LONG lBondHeadState)	//v4.47T3	//MS60
{
	if (!IsMS60())
		return TRUE;
/*
#ifndef MS_DEBUG
#ifndef	PR_INSP_MACHINE
#ifndef	PROBER
#ifdef NU_MOTION

	LONG lCT	= 0;
	lCT			= 40 + m_lArmPickDelay + m_lHeadPickDelay + m_lEjectorUpDelay + m_lPickDelay + m_lEjectorDownDelay;
	if (lCT < 55)
		return TRUE;

	LONG lCTInNuMotionTime = lCT * 8 + 25;	//8k frequency sampling in NuMotion		//v4.47T9

	if (lCTInNuMotionTime > stILC.ILCConfigInfo.LearningLength)
	{
		lCTInNuMotionTime =  stILC.ILCConfigInfo.LearningLength;
	}

	long lOldValue = stILC.ILCConfigInfo.ApplyingLength;

	if (lCTInNuMotionTime != lOldValue)
	{
		stILC.ILCConfigInfo.ApplyingLength = lCTInNuMotionTime;

		CString szLog;
		szLog.Format("ILC (BH #%ld) changed ApplyLength: orig = %ld, new = %ld (%ld,%ld,%ld,%ld,%ld), Learn = %ld", 
			lBondHeadState+1, lOldValue, lCTInNuMotionTime, 
			m_lArmPickDelay, m_lHeadPickDelay, m_lEjectorUpDelay, m_lPickDelay, m_lEjectorDownDelay,
			stILC.ILCConfigInfo.LearningLength);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
#endif
#endif
#endif
#endif
*/
	return TRUE;
}




