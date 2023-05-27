#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"

#include "MS896A.h"		// For Timing

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CBondHead::OpStartMultiSOT(CByteArray &ucaDutList)
{
	TesterData stData;
	for(INT i=0; i<ucaDutList.GetSize(); i++)
	{
		switch( ucaDutList.GetAt(i) )
		{
		case 1:
			stData.nDUT1Present = 1;
			break;
		case 2:
		default:
			stData.nDUT2Present = 1;
			break;
		}
	}
//	stData.nDUT1TestSetNumber = usPinNo;
	m_TesterServer.SetTestData(stData);	//	block probe multi tester/sot

	m_TesterServer.SetSOT();
	m_dProbeTestStartTime	= GetTime();
 
	m_bIsProbeTestStarted = TRUE;
	return TRUE;
}

BOOL CBondHead::OpSaveMultiTestResult()
{
	CString szMsg;
	m_dProbeTestDuration	= GetTime() - m_dProbeTestStartTime;
	if( m_dProbeTestDuration>200 )
	{
		szMsg.Format("CP100 Wait EOT time %d > 200ms", (LONG)m_dProbeTestDuration);
		SetErrorMessage(szMsg);
	}

	TesterData stMultiData;
	m_TesterServer.GetTestData(&stMultiData);

	CStringArray szaRawData, szaItemData;
	szaRawData.RemoveAll();
	szaItemData.RemoveAll();
	USHORT usTesterNum = m_stCurrDieTestData.m_acuPinNo.GetSize();
	if( usTesterNum>=100 )
	{
		usTesterNum = 99;
		SetErrorMessage("CP Multi tester probing error, total number >100");
	}
	if (ConnectTesterTerminal())
	{
		CString szTestData	= TesterGetTestResult();
		CString szDataList[100];
		CStringArray szaList;
		szaList.RemoveAll();
		ParseRawData(szTestData, szaList);
		UINT nDataNum = szaList.GetSize();
		INT i=0;
		//v4.53	Klocwork
		USHORT usTesterIndex = 0;
		USHORT usTesterIndex0 = 0;

		while( nDataNum>=usTesterNum )
		{
			for(usTesterIndex=0; usTesterIndex<usTesterNum; usTesterIndex++)
			{
				szDataList[usTesterIndex] += szaList.GetAt(i);
				if( i<(nDataNum-usTesterNum) )
					szDataList[usTesterIndex] += ",";
				i++;
				if( i>=nDataNum )
				{
					break;
				}
			}
			if( i>=nDataNum )
			{
				break;
			}
		}

		for (usTesterIndex=0; usTesterIndex<usTesterNum; usTesterIndex++)
		{
			CString szIntensityResult;
			CString szIntensity = "SAT,PWR,D1"; 
			if( m_stCurrDieTestData.m_acuPinNo.GetAt(usTesterIndex)!=1 )
			{
				szIntensity.Format("SAT,PWR,D1_%d", m_stCurrDieTestData.m_acuPinNo.GetAt(usTesterIndex));
			}
			SendRequestToTesterTerminal(szIntensity, szIntensityResult);
			szIntensityResult = szIntensityResult.Left(5);
			szDataList[usTesterIndex] += ("," + szIntensityResult);
		}
		for (usTesterIndex0=0; usTesterIndex0<usTesterNum; usTesterIndex0++)
		{
			szaRawData.Add(szDataList[usTesterIndex0]);
		}

		for(usTesterIndex=0; usTesterIndex<usTesterNum; usTesterIndex++)
		{
			CString szItemGradeData = "";
			GetCurrDieItemGrades(szItemGradeData, m_stCurrDieTestData.m_acuPinNo.GetAt(usTesterIndex));
			szaItemData.Add(szItemGradeData);
		}
	}

	DisconnectTesterTerminal();

	for(USHORT usTesterIndex=0; usTesterIndex<usTesterNum; usTesterIndex++)
	{
		m_lTestGrade			= stMultiData.nBinNumber;
		LONG lRow = m_stCurrDieTestData.m_lMapRow;
		LONG lCol = m_stCurrDieTestData.m_lMapCol;
		if( usTesterIndex<m_stCurrDieTestData.m_aucPinRow.GetSize() &&
			usTesterIndex<m_stCurrDieTestData.m_aucPinCol.GetSize() )
		{
			lRow += m_stCurrDieTestData.m_aucPinRow.GetAt(usTesterIndex);
			lCol += m_stCurrDieTestData.m_aucPinCol.GetAt(usTesterIndex);
		}	//	block probe

		m_stCurrDieTestData.m_ucTestGrade	= (UCHAR)m_lTestGrade;
		m_stCurrDieTestData.m_szTestData	= szaRawData.GetAt(0);
		m_stCurrDieTestData.m_szMapItemData = szaItemData.GetAt(0);
		//	block probe
		m_stCurrDieTestData.m_aucPinGrade.Add((UCHAR)m_lTestGrade);
		m_stCurrDieTestData.m_aszPinItemData.Add(szaItemData.GetAt(usTesterIndex));
		m_stCurrDieTestData.m_aszPinData.Add(szaRawData.GetAt(usTesterIndex));

		//fill up station static object for current tested die
		LONG lUserRow, lUserCol;
		CString szRow, szCol, szGrade;
		if( GetPSCMode()==PSC_REGN && (m_ucMapRotation > 0)  )
			ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);
		else
			ConvertAsmToHmiUser(lRow, lCol, lUserRow, lUserCol);
		szRow.Format("%d", lUserRow);
		szCol.Format("%d", lUserCol);
		szGrade.Format("%d", (UCHAR)m_lTestGrade);

		BOOL bProbeOK = TRUE;
		if( (!m_bReProbeViaVF && !m_bReProbeViaIR) && m_ulReProbeLimit>0 && m_ulReProbeCount<=m_ulReProbeLimit )
		{
			for(int i=0; i<m_uiaCpNgGradeList.GetSize(); i++)
			{
				if( m_lTestGrade==m_uiaCpNgGradeList.GetAt(i) )
				{
					bProbeOK = FALSE;
				}
			}
		}

		CString szLogMsg;
		szLogMsg.Format("ReProbe %d, %d,%d,%d, %s", m_ulReProbeCount, lUserRow, lUserCol, m_lTestGrade, szaRawData.GetAt(usTesterIndex));
		if( bProbeOK )
		{
			if( m_ulReProbeCount>1 )
			{
				CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
			}
			m_ulReProbeCount = m_ulReProbeLimit + 1;
		}
		else
		{
			CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
			return FALSE;
		}

		szLogMsg.Format("BGT %d,%d,%d", lRow, lCol, m_lTestGrade);
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);

		if( m_ulContinueProbeLimit>1 )
		{
			BOOL bLast = (m_ulContinueProbeCount>=m_ulContinueProbeLimit);
			CString szHeader;
			szHeader.Format("%s,%s,%s", szRow, szCol, szGrade);
			GenContinueProbeDataFile(szHeader, szaRawData.GetAt(usTesterIndex), bLast);
		}

		if (GenerateProberTempFile(szaRawData.GetAt(usTesterIndex), szRow, szCol, szGrade) == FALSE)
		{
			return FALSE;
		}

		if( SaveItemGradeDataFile(szaItemData.GetAt(usTesterIndex), szRow, szCol, szGrade) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBondHead::OpStartSOT(USHORT usPinNo)
{
	TesterData stData;
	stData.nDUT1TestSetNumber = usPinNo;
	m_TesterServer.SetTestData(stData);	//	block probe

	m_TesterServer.SetSOT();
	m_dProbeTestStartTime	= GetTime();
 
	m_bIsProbeTestStarted = TRUE;
	return TRUE;
}

BOOL CBondHead::OpWaitEOT()
{
	if (!IsProber())
	{
		return FALSE;
	}
	if (!m_bIsProbeTestStarted)
	{
		return FALSE;
	}
	
	ULONG lTimeOut = m_ulProbeTestTimeout * 1000;
	
	LONG	lReturn = WAIT_OBJECT_0;	
	if( IsBurnInCP() && m_bNoTesterBurnInCP )
	{
		Sleep(40);
	}
	else
	{
		lReturn = m_TesterServer.WaitForEOT(lTimeOut);	
	}
	if (lReturn != WAIT_OBJECT_0)
	{
		m_bTesterTimeOut = TRUE;	// wait EOT time out
		m_TesterServer.ResetSOT();
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::OpWaitTestDone()
{
	if (!IsProber())
	{
		return FALSE;
	}

	if (!m_bIsProbeTestStarted)
	{
		return FALSE;
	}
	
	ULONG lTimeOut = m_ulProbeTestTimeout * 1000;
	m_bTesterTimeOut = FALSE;	// after send SOT, wait TEST DONE, before wait EOT

	DOUBLE dTime = GetTime();
	LONG	lReturn = WAIT_OBJECT_0;	
	if( IsBurnInCP() && m_bNoTesterBurnInCP )
	{
		Sleep(20);
	}
	else
	{
		lReturn = m_TesterServer.WaitForIsRunning(lTimeOut);	
	}

	if (lReturn != WAIT_OBJECT_0)
	{
		m_bTesterTimeOut = TRUE;	// after send SOT, wait TEST DONE, before wait EOT
		return FALSE;
	}
	dTime = GetTime() - dTime;
	if( dTime>100 )
	{
		CString szMsg;
		szMsg.Format("CP100 Wait Test Done time %d > 100ms", (LONG)dTime);
		SetErrorMessage(szMsg);
	}

	return TRUE;
}

BOOL CBondHead::OpSaveTestResult(USHORT usPinIndex)
{
	TesterData stData;
	if( IsBurnInCP() && m_bNoTesterBurnInCP )
	{
		stData.nBinNumber	= 1;
		stData.nTestPass	= 1;
	}
	else
	{
		m_TesterServer.GetTestData(&stData);
	}
	m_dProbeTestDuration	= GetTime() - m_dProbeTestStartTime;
	m_lTestGrade			= stData.nBinNumber;
	if( m_dProbeTestDuration>100 )
	{
		CString szMsg;
		szMsg.Format("CP100 Wait EOT time %d > 100ms", (LONG)m_dProbeTestDuration);
		SetErrorMessage(szMsg);
	}

	//	stData.nTestPass;
	LONG lRow = m_stCurrDieTestData.m_lMapRow;
	LONG lCol = m_stCurrDieTestData.m_lMapCol;
	if( usPinIndex<m_stCurrDieTestData.m_aucPinRow.GetSize() &&
		usPinIndex<m_stCurrDieTestData.m_aucPinCol.GetSize() )
	{
		lRow += m_stCurrDieTestData.m_aucPinRow.GetAt(usPinIndex);
		lCol += m_stCurrDieTestData.m_aucPinCol.GetAt(usPinIndex);
	}	//	block probe

	CString szTestData = "";
	CString szItemGradeData = "";
	CString szIntensity, szIntensityResult;

	if (ConnectTesterTerminal())
	{
		szTestData	= TesterGetTestResult();
		GetCurrDieItemGrades(szItemGradeData);

		szIntensity = "SAT,PWR,D1"; 
		SendRequestToTesterTerminal(szIntensity, szIntensityResult);
		szIntensityResult = szIntensityResult.Left(5);
		szTestData = szTestData + "," + szIntensityResult;
		DisconnectTesterTerminal();
	}

	//CMSLogFileUtility::Instance()->MS_LogOperation("intensity:" + szIntensityResult);
	m_stCurrDieTestData.m_ucTestGrade	= (UCHAR)m_lTestGrade;
	m_stCurrDieTestData.m_szTestData	= szTestData;
	m_stCurrDieTestData.m_szMapItemData = szItemGradeData;
	//	block probe
	m_stCurrDieTestData.m_aucPinGrade.Add((UCHAR)m_lTestGrade);
	m_stCurrDieTestData.m_aszPinItemData.Add(szItemGradeData);
	m_stCurrDieTestData.m_aszPinData.Add(szTestData);

	//fill up station static object for current tested die
	LONG lUserRow, lUserCol;
	CString szRow, szCol, szGrade;
	if( GetPSCMode()==PSC_REGN && (m_ucMapRotation > 0)  )
		ConvertAsmToOrgUser(lRow, lCol, lUserRow, lUserCol);
	else
		ConvertAsmToHmiUser(lRow, lCol, lUserRow, lUserCol);
	szRow.Format("%d", lUserRow);
	szCol.Format("%d", lUserCol);
	szGrade.Format("%d", (UCHAR)m_lTestGrade);

	BOOL bProbeOK = TRUE;
	if( (!m_bReProbeViaVF && !m_bReProbeViaIR) && m_ulReProbeLimit>0 && m_ulReProbeCount<=m_ulReProbeLimit )
	{
		for(int i=0; i<m_uiaCpNgGradeList.GetSize(); i++)
		{
			if( m_lTestGrade==m_uiaCpNgGradeList.GetAt(i) )
			{
				bProbeOK = FALSE;
			}
		}
	}

	CString szLogMsg;
	szLogMsg.Format("ReProbe %d, %d,%d,%d, %s", m_ulReProbeCount, lUserRow, lUserCol, m_lTestGrade, szTestData);
	if( bProbeOK )
	{
		if( m_ulReProbeCount>1 )
		{
			CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
		}
		m_ulReProbeCount = m_ulReProbeLimit + 1;
	}
	else
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
		return FALSE;
	}

	szLogMsg.Format("BGT %d,%d,%d", lRow, lCol, m_lTestGrade);
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);

	if( m_ulContinueProbeLimit>1 )
	{
		BOOL bLast = (m_ulContinueProbeCount>=m_ulContinueProbeLimit);
		CString szHeader;
		szHeader.Format("%s,%s,%s", szRow, szCol, szGrade);
		GenContinueProbeDataFile(szHeader, szTestData, bLast);
	}

	if (GenerateProberTempFile(szTestData, szRow, szCol, szGrade) == FALSE)
	{
		return FALSE;
	}

	if( SaveItemGradeDataFile(szItemGradeData, szRow, szCol, szGrade) )
	{
		return FALSE;
	}

	return TRUE;
}

CString CBondHead::TesterGetTestResult()
{
	CString szTestData = _T("");
	GetCurrDieTestData(szTestData);

	szTestData.Replace(";", "");
	
	return szTestData;
}

INT	CBondHead::PBTZ_Cycle_UpTo_Probe()
{
	INT nReturn = gnOK;

	if ( m_bMotionApplyProcessBlock == TRUE && IsEnableSearch() == TRUE )
	{
		if ( PBTZ_UpToContactLevel(TRUE, SFM_WAIT) == gnNOTOK )
		{
			MotionDirectReadSWPort("PROBER_SW_PORT_0", m_lProcessBlockSoftwarePortValue);
			PB_ReadEncoderSWPort();

			UpdateAndLogPinESValue();

			m_bIsProbeErrorInCycle = TRUE;

			nReturn = gnNOTOK;
		}
		else if ( m_bProcessBlockSavingEncorder == TRUE )
		{
			UpdateAndLogPinESValue();

			BOOL bIsUpdateHMIPinLevel = TRUE;
			for ( int i = 1 ; i <= GetPinNo() ; i++ )
			{
				if ( m_lProcessBlockESValue[i-1] == -1 )
				{
					bIsUpdateHMIPinLevel = FALSE;
				}
			}
						
			if ( bIsUpdateHMIPinLevel == TRUE )
			{
				m_lBPGeneral_BHZ1 = m_lProcessBlockESValue[0];
				m_lBPGeneral_BHZ2 = m_lProcessBlockESValue[1];
				if( GetPinNo()==4 )
				{
					m_lBPGeneral_BHZ3 = m_lProcessBlockESValue[2];
					m_lBPGeneral_BHZ4 = m_lProcessBlockESValue[3];
				}

				if (GetPinNo() == 6)   //M1MultiProbe
				{
					m_lBPGeneral_BHZ5 = m_lProcessBlockESValue[5];
					m_lBPGeneral_BHZ6 = m_lProcessBlockESValue[6];
				}
				//
			}
		}
	}
	else
	{
		nReturn = PBTZ_UpToContactLevel(TRUE, SFM_NOWAIT);
	}

	return nReturn;
}

INT	CBondHead::PBTZ_Cycle_DownTo_Standby()
{
	INT nReturn = gnOK;
	if( IsEnableSearch() )
	{
		GetEncoderValue();
		PBTZ_Profile(NORMAL_PROF, m_lEnc_PBTZ);
	}

	if ( m_bIsEnableOpenDac == TRUE )
	{
		MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_STATIC);	// PL_OPEN_DAC
	}

	if ( IsEnableSearch() == TRUE)
	{	
		if ( m_bProcessBlockSavingEncorder == TRUE )
		{
			m_lContactDynamicLevel_PBTZ = m_lEnc_PBTZ;			// Save the dynamic contact level
		}

		if ( m_bPBTZUseDynamicLevel == TRUE )
		{
			m_lContactDynamicLevel_PBTZ = m_lEnc_PBTZ;
			LONG lCommandPos = CMS896AStn::MotionGetCommandPosition(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ);
			if ( m_bPBTZUseCommendInDynamicLevel == TRUE && lCommandPos > 0)
			{
				CString t;
				t.Format("%d", lCommandPos);
				OutputDebugString(t);
				m_lContactDynamicLevel_PBTZ = lCommandPos;
			}
			m_lStandByDynamicLevel_PBTZ = m_lContactDynamicLevel_PBTZ + m_lStandByLevelOffset_PBTZ;
		}
		else 
		{
			m_lStandByDynamicLevel_PBTZ = m_lStandByLevel_PBTZ; // Keep the contact level to origion level
		}

		nReturn = PBTZ_MoveTo(m_lStandByDynamicLevel_PBTZ, SFM_NOWAIT);
	}
	else
	{
		nReturn = PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_NOWAIT);
	}

	return nReturn;
}

BOOL CBondHead::IsCheckFirstProbePos()	//	CP Map Display
{
	if (!IsBurnIn() && m_bIsCheckFirstProbePos == FALSE)
	{
		ClosePPP(TRUE);
		m_bPrescanJustFinishToProbe = FALSE;

		return TRUE;
	}

	CString szMsg;
	
	// Move To BPR Camera
	if (SelectToolingStage(MS_TOOLINGSTAGE_CAM) == FALSE)
	{
		SetStatusMessage("Check Probe Position Fail");
		SetErrorMessage("Check Probe Position Fail");
		return FALSE;
	}
	ClosePPP(FALSE);

	PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);

	// set the live video
	SetPrAutoBondMode(FALSE);

	// let user check the first probe position
	if( !IsBurnIn() )
	{
		szMsg.Format("Please check the probe position");
		if (SetAlert_Msg_Red_Back(IDS_BH_CHECK_PROBE_POS, szMsg, "Continue", "Stop") != 1)
		{
			SetStatusMessage("Check Probe Position Fail");
			SetErrorMessage("Check Probe Position Fail");
			return FALSE;
		}
	}
	else
	{
		Sleep(2000);
	}

	m_bPrescanJustFinishToProbe = FALSE;

	// set to auto bond mode
	SetPrAutoBondMode(TRUE);

	PBTZ_MoveToFocusLevel();

	ClosePPP(TRUE);
	// Move To Tool to use in auto bond cycle
	if (MoveToToolingStageToUse() == FALSE)
	{
		SetStatusMessage("Check Probe Position Fail");
		SetErrorMessage("Check Probe Position Fail");
		return FALSE;
	}

	return TRUE;
}

BOOL CBondHead::IsMaxProbePinLimit()
{
	BOOL bReturn = FALSE;

	if (m_ulMaxProbePinCount > 0 && m_ulProbePinCount > m_ulMaxProbePinCount)
	{
		SetAlert_Red_Yellow(IDS_BH_REPLACE_PROBEPIN);
		bReturn = TRUE;
		SetStatusMessage("Probe Pin lifetime is reached");
		SetErrorMessage("Probe Pin lifetime is reached");
	}

	if( m_lProbeMarkCheckLimit==0 || GetNewPickCount()==0 )
	{
		return bReturn;
	}

	if( (m_lProbeMarkCheckLimit>0 && (GetNewPickCount()%m_lProbeMarkCheckLimit)==0) ||
		(m_lProbeMarkCheckLimit<0 && GetNewPickCount()==labs(m_lProbeMarkCheckLimit)) )
	{
		m_bCPItemMapToTop = FALSE;
		IPC_CServiceMessage stMsg;
		// 1: SetISPDownCmd
		BOOL bFalse = FALSE;
		stMsg.InitMessage(sizeof(BOOL), &bFalse);
		INT nConvID = m_comClient.SendRequest("BondPrStn", "SetISPDownCmd", stMsg);
		Sleep(100);
		// 2: Rotate Optics (rotate camera and turn on light)
		nConvID = m_comClient.SendRequest("WaferPrStn", "SelectBondCamera", stMsg);
		Sleep(100);
		SelectToolingStage(MS_TOOLINGSTAGE_CAM);	//0=Optics; 1=ISP; 2=Detector
		SetPrAutoBondMode(FALSE);
		// 3: move to P-Level
		PBTZ_MoveTo(m_lProbeFocusLevelZ);
		Sleep(100);

		// 4: Alarm and ask for clean pin
		LONG lAlarmStatus = GetAlarmLamp_Status();
		SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
		CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
		CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
		slLock.Lock();
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		CString szMsg;
		szMsg.Format("Please check the probe position");
		pApp->m_bCycleStarted = FALSE;
		int nReply;
		while( 1 )
		{
			// Turn On light
			LONG lOpticsID = 0;
			stMsg.InitMessage(sizeof(LONG), &lOpticsID);		
			nConvID = m_comClient.SendRequest("BondPrStn", "SetCleanProbeLighting", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			Sleep(1000);

//Show Camera Image
			//1st turn of lighting
			BOOL bOn = 1;
			stMsg.InitMessage(sizeof(LONG), &bOn);		
			nConvID = m_comClient.SendRequest("WaferPrStn", "TurnOnOffPrLightingCmd", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			//2nd Set to Live Mode
			nConvID = m_comClient.SendRequest("WaferPrStn", "SetPRSysLiveMode", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			nConvID = m_comClient.SendRequest("WaferPrStn", "SelectWaferCamera", stMsg);
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
			nReply = SetAlert_Msg(IDS_BH_CHECK_PROBE_POS, szMsg, "4. Clean Pin", "OK" ,"STOP" , glHMI_ALIGN_CENTER, -1, 0);
//AutoCaptureScreenInCleanPin
			stMsg.InitMessage();
			nConvID = m_comClient.SendRequest("WaferTableStn", "AutoCaptureScreenInCleanPin", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			//if( nReply == 1 ) //Clean pin
			if( nReply == 5 )  // OK: resume cycle
			{
				MoveToToolingStageToUse();
				//close the camera screen and change colour
				nConvID = m_comClient.SendRequest("WaferPrStn", "ItemMapToTop", stMsg);
				while (1)
				{
					if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
					{
						m_comClient.ReadReplyForConvID(nConvID, stMsg);
						break;
					}
					else
					{
						Sleep(10);
					}
				}

//Show Map Image
				//1st Turn Of Lighting
			bOn = 0;
			stMsg.InitMessage(sizeof(LONG), &bOn);		
			nConvID = m_comClient.SendRequest("WaferPrStn", "TurnOnOffPrLightingCmd", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			//2nd Set to Bond Mode
			nConvID = m_comClient.SendRequest("WaferPrStn", "SetPRSysBondMode", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
				m_bCPItemMapToTop = TRUE;
				break;
			}
			else // cancel or OK
			{
				bReturn = TRUE;
				break;
			}
		}
		pApp->m_bCycleStarted = TRUE;

		slLock.Unlock();
		SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
		// Forth button
		if ( nReply == 1 )
		{
			PBTZ_MoveTo0Level();	//	PBTZ_MoveToFocusLevel();	//	CPP to home,
			stMsg.InitMessage();
			nConvID = m_comClient.SendRequest("WaferTableStn", "ManualCleanProbePinProcess", stMsg);
			while (1)
			{
				if (m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			PBTZ_MoveTo(m_lProbeFocusLevelZ);
		}

		SetPrAutoBondMode(TRUE);
		SetStatusMessage(szMsg);
		SetErrorMessage(szMsg);
	}

	return bReturn;
}

BOOL CBondHead::IsCleanProbePinLimit()
{
	if (m_ulMaxProbePinCleanCount > 0 && m_ulProbePinCleanCount > m_ulMaxProbePinCleanCount)
	{
		return TRUE;
	}

	return FALSE;
}

VOID CBondHead::PB_ResetEncoderSWPort()
{
	if ( m_bMotionApplyProcessBlock == TRUE )
	{
		for ( int i = 1 ; i <= GetPinNo(); i++ )
		{
			CString szTempPort;
			szTempPort.Format("PROBER_SW_PORT_ES%d", i);
			MotionDirectResetSWPort(szTempPort);
		}
	}
}

VOID CBondHead::PB_ReadEncoderSWPort()
{
	for ( int i = 1 ; i <= GetPinNo() ; i++ )
	{
		CString szTempPort;
		szTempPort.Format("PROBER_SW_PORT_ES%d", i);
		MotionDirectReadSWPort(szTempPort, m_lProcessBlockESValue[i-1]);
	}	

}

