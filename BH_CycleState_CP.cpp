#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondHead.h"
#include "CycleState.h"
#include "math.h"
#include "BondPr.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CBondHead::RunOperation_Prober()
{
	BOOL bDownPbtZ = TRUE;
	BOOL	bNoMotionHouseKeeping = FALSE;
	static	DOUBLE	s_dDownStartTime = 0;
	static	DOUBLE	s_dUpStartTime = 0;

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);

	if ((Command() == glSTOP_COMMAND) && (Action() == glABORT_STOP))
	{
		DisplaySequence("CPZ - STOP");
		State(STOPPING_Q);
		return ;
	}

	m_bStep = FALSE;

	try
	{
		if ( IsMotionCE() == TRUE )
		{
			m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
		}

		if (m_bStopAllMotion == TRUE)
		{
			m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
			bNoMotionHouseKeeping = TRUE;
		}

		m_nLastError = gnOK;

		switch (m_qSubOperation)
		{
		case CP_WAIT_WT_READY_TO_PROBE:
			if (m_bStop)
			{
				m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
				DisplaySequence("CPZ - Wait WT ==> House Keeping");
				break;
			}

			if (WaitWTReady())
			{
				SetWTReady(FALSE);
				SetProbeZReady(FALSE);

				DisplaySequence("CPZ - WT ready ==> Probe Z Up");
				if( m_bCycleFirstProbe )
					m_qSubOperation = CP_FIRST_PROBE_PREPARE;
				else
					m_qSubOperation = CP_Z_UP_TO_PROBE;
			}
			break;

		case CP_FIRST_PROBE_PREPARE:
			if (m_bPrescanJustFinishToProbe)
			{
				if ( m_bProcessBlockSavingEncorder == TRUE )
				{
					CMSPrescanUtility *pUtl = CMSPrescanUtility::Instance();
					CString szLogPath;
					pUtl->GetPrescanLogPath(szLogPath);
					CString szLogFileName = szLogPath + CP100_PIN_LOG;
					//DeleteFile(szLogFileName);
					CMSLogFileUtility::Instance()->CP100_PinLogOpen();
					CString szHeader;
					szHeader = ",X,Y,";
					for ( int i = 0 ; i < GetPinNo() ; i++ )
					{
						CString szTemp;
						szTemp.Format( "ES%d Encorder,", i+1 );
						szHeader += szTemp;
					}
					szHeader += "SW-Port value 0,";
					szHeader += "SW-Port value 1,";
					CMSLogFileUtility::Instance()->CP100_PinLog(szHeader);
					CMSLogFileUtility::Instance()->CP100_PinLogClose();
				}
				m_ulPrbTotalFailCount = 0;
				if( m_bUseMultiProbeLevel )
				{
					DisplaySequence("CPZ - Wait WT first probe, search multi level");
					IPC_CServiceMessage stMsg;
					PB_SearchMultiProbeLevel(stMsg);
				}

				if (IsCheckFirstProbePos() == FALSE)	// wafer once only
				{
					m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
					break;
				}
				//	block probing	check version, if old, alarm and stop
			}

			if ( m_bCycleFirstProbe )
			{
				if ( m_bProcessBlockSavingEncorder == TRUE )
				{
					CMSLogFileUtility::Instance()->CP100_PinLogOpen();
				}
				if( (!m_bReProbeViaVF && !m_bReProbeViaIR) && m_ulReProbeLimit>0 )
				{
					CString szCmd, szReply;
					CStringArray szaData;
					if ( ConnectTesterTerminal() )
					{
						for(int i=0; i<=255; i++)
						{
							//	BIN,bin no[,QT][,CD][,NG]
							//	Pass additional parameters for bin count(QT), bin code(CD) and NG status(NG).
							szCmd.Format("BIN,%d,QT,CD,NG", i);

							szReply = "";
							SendRequestToTesterTerminal(szCmd, szReply);	// BIN, to get bin attributes
							SetErrorMessage(szCmd + " rpy: " + szReply);
						//	BIN,1,0,Default,2;
							if( szReply.IsEmpty()==FALSE )
							{
								szReply.Replace(";", "");
								szaData.RemoveAll();
								ParseRawData(szReply, szaData);
								//	BIN,bin no[,bin count][,bin code][,NG];
								//	NG = 2 if default bin     1 if NG bin     0 otherwise
								if( szaData.GetSize()>=5 )
								{
									CString szNg = szaData.GetAt(4);
									if( szNg=="1" )
									{
										m_uiaCpNgGradeList.Add(i);
									}
								} 
							}
						}

						DisconnectTesterTerminal();
						szCmd = "ReProbe Ng Grade List ";
						szReply = "";
						for(int i=0; i<m_uiaCpNgGradeList.GetSize(); i++)
						{
							szReply.Format("%d,", m_uiaCpNgGradeList.GetAt(i));
							szCmd += szReply;
						}
						SetErrorMessage(szCmd);
						CMSLogFileUtility::Instance()->WT_GetDieLog(szCmd);
					}
				}

				ClosePPP(TRUE);	// begin first probing, before up prb z
				// first cycle move to ISP or PD
				if (MoveToToolingStageToUse() == FALSE)	//	CP cycle probing first time
				{
					SetAlert(IDS_BPR_ISP_IS_NOT_IN_SAFE_POSITION);
					m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
					break;
				}

				m_lStandByDynamicLevel_PBTZ = m_lStandByLevel_PBTZ;
				m_lContactDynamicLevel_PBTZ = m_lContactLevel_PBTZ;

				if ( m_bMotionApplyProcessBlock == TRUE )
				{
					PB_ResetEncoderSWPort();
					MotionDirectResetSWPort("PROBER_SW_PORT_0");
					MotionDirectResetSWPort("PROBER_SW_PORT_1");
				}

				m_bCycleFirstProbe = FALSE;
				m_ulProcessBlockFailCount = 0;
			}

			m_qSubOperation = CP_Z_UP_TO_PROBE;
			break;

		case CP_Z_UP_TO_PROBE:	// begin PB Z UP
			if (m_bStop)
			{				
				DisplaySequence("CPZ - Up to probe ==> House Keeping");
				m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
				break;
			}

			if ( AllowMove() == TRUE )
			{
				//Check machine pressure
				if (IsLowPressure() == TRUE)
				{
					SetStatusMessage("Machine low pressure");
					SetAlert_Red_Yellow(IDS_BH_LOW_PRESSURE);
					SetErrorMessage("Machine low pressure");

					m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
					DisplaySequence("CPZ - Low pressure ==> House Keeping");
					break;
				}

				// check machine cover state
				if ( IsCoverOpen() == TRUE )
				{
					SetStatusMessage("Machine Cover Open");
					SetAlert_Red_Yellow(IDS_BH_COVER_OPEN);
					SetErrorMessage("Machine Cover Open");

					m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
					DisplaySequence("CPZ - Cover open ==> House Keeping");
					break;
				}

				if (!OpCheckValidAccessMode(TRUE))	// currently for SanAn
				{
					SetStatusMessage("Access mode Validated (SanAn)");
					SetErrorMessage("Access mode Validated (SanAn)");

					m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
					DisplaySequence("CPZ - Invalid access mode ==> House Keeping");
					break;
				}

				// new learn plane by region function
			//	GetEncoderValue();
			//	CString szMsg;
			//	szMsg.Format("CPZ - now is %ld, before move Probe Z Up", m_lEnc_PBTZ);
			//	DisplaySequence(szMsg);
				DisplaySequence("CPZ - move Probe Z Up");
				TakeTime((TIME_ENUM) ZUB);	// probe z up begin	ZUB

				// add new process block
				if( PBTZ_Cycle_UpTo_Probe()==gnNOTOK )
				{
					m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
					break;
				}

				s_dUpStartTime = GetTime();

				m_qSubOperation = CP_AT_PROBE_UP_SOT;
				m_ulReProbeCount	= 0;
				DisplaySequence("CPZ - Probe Z Up ==> At Up Level");
			}
			break;

		case CP_AT_PROBE_UP_SOT:	// PB Z UP END
			if ( AllowMove() == TRUE )
			{
				if( IsEnableSearch() == TRUE )
				{
					if ( m_bIsEnableOpenDac == TRUE )
					{
						MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_OPEN_DAC);
						PBTZ_Move(0, SFM_NOWAIT);		//PL_OPEN_DAC
					}
					PBTZ_Sync();
				}
				else
				{
					LONG lTemp = (LONG)(GetTime() - s_dUpStartTime);
					if ( lTemp < (m_lPBTZUpTime_Z) )
					{
						Sleep(m_lPBTZDnTime_Z - lTemp);
					}
				}
				TakeTime((TIME_ENUM) ZUE);	// probe z up done  ZUE

				if( m_bCheckProbeHeadContactSnr == TRUE )
				{
					if ( IsContactSensorAND_ON(TRUE) == FALSE ) // return FALSE if one of contact sensor (OR) is off
					{
						m_ulProbeFailCount = m_ulProbeFailCount + 1;
						m_ulPrbContactFailAcuCount++;
						m_ulPrbContactFailConCount++;
						m_ulPrbTotalFailCount++;
						// OPEN_DAC
						if ( m_bIsEnableOpenDac == TRUE )
						{
							MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_STATIC);	//PL_OPEN_DAC
						}
						PBTZ_MoveTo(m_lProbeFocusLevelZ);
						if (IsMaxProbeFailLimit() == TRUE)
						{
							m_bIsProbeErrorInCycle = TRUE;
							m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
							break;
						}
						if( m_ulProbeFailCountLimit>0 )
						{
							break;
						}
					}
					else
					{
						m_ulPrbContactFailConCount = 0;
					}
				}

				BOOL bIsTooLow = FALSE, bIsTooDiff = FALSE;
				if ( m_bMotionApplyProcessBlock == TRUE )
				{
					if( m_bProcessBlockCycleCheckAbortLevel == TRUE || m_bProcessBlockCycleCheckContactDiff == TRUE )
					{
						CString szOutput;
						if( CheckPinDifferenceSuccess(bIsTooLow,bIsTooDiff,szOutput) == FALSE )
						{
							if ( m_ulProcessBlockStopLimit > 0 )
							{
								m_ulProcessBlockFailCount++;

								CString szRetry;
								szRetry.Format("[%d<%d]", m_ulProcessBlockFailCount,m_ulProcessBlockStopLimit);

								if ( m_ulProcessBlockFailCount >= m_ulProcessBlockStopLimit )
								{
									OutputDebugString("STOP"+szRetry);
									m_ulProcessBlockFailCount = 0;

									// OPEN_DAC
									if ( m_bIsEnableOpenDac == TRUE )
									{
										MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_STATIC);	// PL_OPEN_DAC
									}

									PBTZ_MoveTo(m_lProbeFocusLevelZ);
									HmiMessage_Red_Yellow(szOutput, "Runtime Check Pin ERROR");

									m_bIsProbeErrorInCycle = TRUE;
									m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
									break;	
								}
								else
								{
									OutputDebugString("SKIP"+szRetry);
								}
							}
							else
							{
								// OPEN_DAC
								if ( m_bIsEnableOpenDac == TRUE )
								{
									MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_STATIC);	// PL_OPEN_DAC
								}

								CString szOutput;
								PBTZ_MoveTo(m_lProbeFocusLevelZ);
								HmiMessage_Red_Yellow(szOutput, "Runtime Check Pin ERROR");

								m_bIsProbeErrorInCycle = TRUE;
								m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
								break;	
							}
						}
						else
						{
							m_ulProcessBlockFailCount = 0;
						}
					}
				}

			//	CString szMsg;
			//	GetEncoderValue();
			//	szMsg.Format("CPZ - at up now is %d, next to send SOT", m_lEnc_PBTZ);
			//	DisplaySequence(szMsg);
				m_ulContinueProbeCount = 0;
				DisplaySequence("CPZ - at up next to send SOT");
				m_qSubOperation = CP_AT_UP_SEND_SOT;
			}
			break;

		case CP_AT_UP_SEND_SOT:
			m_ulContinueProbeCount++;	//	at up level, keep contact, do probing several times.
			TakeTime((TIME_ENUM) SOT);	//	Send SOT	SOT 
			if (m_ulProbeDelay > 0)	//	block probing sleep the first time
			{
				Sleep(m_ulProbeDelay);
			}

			m_ulReProbeCount++;
			if( (m_bReProbeViaVF || m_bReProbeViaIR) && m_ulReProbeLimit>0 && m_ulReProbeCount<=m_ulReProbeLimit )
			{	// send VF or IR test to check, if fail, down and up to re-contact die and try again.
				BOOL bSingleTestOK = TRUE;
				if (ConnectTesterTerminal())
				{
					if( m_bReProbeViaVF )
					{
						//	RES,station no,test time in ms,source currnet in mA
						CString szCmd = _T("RES,0,0.5,source currnet in mA");
						CString szRpy = "";
						//	Fire a single VF test. Same source current will be applied to all of the dies.
						SendRequestToTesterTerminal(szCmd, szRpy);	// RES, VF test
						if( szRpy.IsEmpty()==FALSE )
						{
							CStringArray szaData;
							szaData.RemoveAll();
							ParseRawData(szRpy, szaData);
							//	return: RES,number of channels,D1 voltage[,D2 voltage][,D3 voltage]
							if( szaData.GetSize()>=3 )
							{
								CString szNg = szaData.GetAt(2);
								if( szNg.Find("10")!=-1 )
								{
									bSingleTestOK = FALSE;
								}
							} 
						}
						CString szMsg;
						szMsg = "ReProbe VF cmd:" + szCmd + " rpy:" + szRpy;
						CMSLogFileUtility::Instance()->WT_GetDieLog(szMsg);
					}

					DisconnectTesterTerminal();
				}
				if( bSingleTestOK==FALSE )
				{
					PBTZ_Cycle_DownTo_Standby();
					PBTZ_Sync();
					Sleep(10);
					PBTZ_Cycle_UpTo_Probe();
					s_dUpStartTime = GetTime();
					PBTZ_Sync();
					m_qSubOperation = CP_AT_PROBE_UP_SOT;
					break;
				}
			}

			DisplaySequence("CPZ - send SOT");
			if( (!m_bReProbeViaVF && !m_bReProbeViaIR) && m_ulReProbeLimit>0 && m_ulReProbeCount>1 )
			{
				if (ConnectTesterTerminal())
				{
					CString szCmd = _T("RETEST"), szRpy = "";
					SendRequestToTesterTerminal(szCmd, szRpy);	// RETEST, to tell tester, this is a re-probe
					DisconnectTesterTerminal();
				}
			}
			if ( m_stCurrDieTestData.m_acuPinNo.GetSize()>0 )
			{
				if( m_bMultiTestersProbing )	//	block probe with multi tester/sot, send sot
				{
					CByteArray ucaDutList;
					ucaDutList.RemoveAll();
					CString szMsg, szTemp;
					szMsg = "CPZ - send SOT DUT";
					for(int i = 0; i<m_stCurrDieTestData.m_acuPinNo.GetSize();i++)
					{
						ucaDutList.Add(m_stCurrDieTestData.m_acuPinNo.GetAt(i));
						szTemp.Format(", %d", m_stCurrDieTestData.m_acuPinNo.GetAt(i));
						szMsg += szTemp;
					}
					DisplaySequence(szMsg);
					OpStartMultiSOT(ucaDutList);	//	block probing send multi tester probing sot with dut
				}
				else
				{
					USHORT usPinNo = m_stCurrDieTestData.m_acuPinNo.GetAt(0)-1;
					OpStartSOT(usPinNo);	//	block probing send sot with pin no
					CString szMsg;
					szMsg.Format("CPZ - send SOT pin no %d", usPinNo);
					DisplaySequence(szMsg);
					for( int i = 1; i<m_stCurrDieTestData.m_acuPinNo.GetSize();i++)
					{
						DOUBLE dTime = GetTime();
						while( 1 )
						{
							OpWaitTestDone();			//	check running finish
							if (OpWaitEOT() == TRUE)
							{
								OpSaveTestResult(i-1);
								usPinNo = m_stCurrDieTestData.m_acuPinNo.GetAt(i)-1;
								OpStartSOT(usPinNo);
								szMsg.Format("CPZ - send SOT pin no %d", usPinNo);
								DisplaySequence(szMsg);
								break;
							}
							if( (GetTime() - dTime)>20000 )
							{
								break;
							}
						}
					}
				}
			}	//	block probe
			else
			{
				OpStartSOT(0);	//	non block probe send sot with default pin no 0
			}

			m_qSubOperation = CP_PROBE_DONE_TO_STANDBY;
			break;

		case CP_PROBE_DONE_TO_STANDBY:	// wait TESTING done, begin down probe Z
			DisplaySequence("CPZ - Wait probing done, down probe z to standby");
			if ( AllowMove() == TRUE )
			{
				bDownPbtZ = m_ulContinueProbeCount>=m_ulContinueProbeLimit;

				TakeData((TIME_ENUM) PDT, m_lPBTZDnTime_Z);
				TakeTime((TIME_ENUM) PDB);	//	to wait TST done  TST 
				OpWaitTestDone();			//	check running finish
				TakeTime((TIME_ENUM) ZDB);	// down probe z to standby level  ZDB
				if( m_bTesterTimeOut )
				{
					bDownPbtZ = TRUE;
					m_ulContinueProbeCount = m_ulContinueProbeLimit;
				}
				else	//	block probing, down pbt z false if last not probed
				{
				}

				if( bDownPbtZ )
				{
					PBTZ_Cycle_DownTo_Standby();
				}

				s_dDownStartTime = GetTime();

				DisplaySequence("CPZ - wait EOT, save result");
				// update it as picked die so that wafer table, wafer map can handle properly
				(*m_psmfSRam)["BondHeadPBT"]["Die Probed X"] = m_stCurrDieTestData.m_lMapCol;
				(*m_psmfSRam)["BondHeadPBT"]["Die Probed Y"] = m_stCurrDieTestData.m_lMapRow;

				m_qSubOperation = CP_AT_STANDBY_FOR_WT_MOVE;
				DisplaySequence("CPZ - to Wait probe Z down complete");
			}
			break;

		case CP_AT_STANDBY_FOR_WT_MOVE:	// WAIT PB Z down complete, trigger wafer table move ahead.
			bDownPbtZ = m_ulContinueProbeCount>=m_ulContinueProbeLimit;

			// get the testing result to the file
			if (OpWaitEOT() == TRUE)
			{
				TakeTime((TIME_ENUM) EOT);	// to wait EOT donw EOT
				USHORT usPinIndex = 0;
				if( m_stCurrDieTestData.m_acuPinNo.GetSize()>1 )
					usPinIndex = m_stCurrDieTestData.m_acuPinNo.GetSize()-1;
				if( m_bMultiTestersProbing )	//	block probe with multi tester/sot, get raw data
				{
					OpSaveMultiTestResult();
				}
				else
					OpSaveTestResult(usPinIndex);	//	block probe
			}
			TakeTime((TIME_ENUM) ADN);	// Testing finished. all done ADN

			if( m_bTesterTimeOut )
			{
				bDownPbtZ = TRUE;
			}
			else	//	block probing	down false if last unprobed
			{
			}

			if( bDownPbtZ )
			{
				SetEOT_Done(TRUE);	//	TEST RAW DATA GOT, let WFT to process raw data
				LONG lTemp = (LONG)(GetTime() - s_dDownStartTime);
				if ( lTemp < (m_lPBTZDnTime_Z) )
				{
					Sleep(m_lPBTZDnTime_Z - lTemp);
				}
				if( (!m_bReProbeViaVF && !m_bReProbeViaIR) && m_ulReProbeLimit>0 && m_ulReProbeCount<=m_ulReProbeLimit )
				{
					PBTZ_Sync();
					Sleep(10);
					PBTZ_Cycle_UpTo_Probe();
					s_dUpStartTime = GetTime();
					PBTZ_Sync();
					m_qSubOperation = CP_AT_PROBE_UP_SOT;
					break;
				}
				SetProbeZReady(TRUE);
				TakeTime((TIME_ENUM) ZDE);	// probe z down end  ZDE  
				CString szMsg;
				szMsg.Format("CPZ - down time %d ok, trigger WFT move", m_lPBTZDnTime_Z);
				DisplaySequence(szMsg);
				PBTZ_Sync();
				m_qSubOperation = CP_EOT_RAWDATA_TO_WT_READY;
			}
			else
			{
				m_qSubOperation = CP_AT_UP_SEND_SOT;
			}
			break;

		case CP_EOT_RAWDATA_TO_WT_READY:	// update counter ...
			DisplaySequence("CPZ - Wait Bond Delay");
			//Update counter
			OpUpdateCP100Counters();		//Update bonded unit

			// Find the cycle time
			double	dCurrentTime;
			dCurrentTime = GetTime();
			if ( m_dLastTime > 0.0 )
			{
				m_dCycleTime = fabs(dCurrentTime - m_dLastTime);

				if ( m_dCycleTime > 99999 )
				{
					m_dCycleTime = 10000;
				}

				// Calculate the average cycle time
				m_dAvgCycleTime = (m_dAvgCycleTime * m_ulCycleCount + m_dCycleTime) / (m_ulCycleCount + 1);
				m_ulCycleCount++;	// reset when start again.

				// 3501
				SetGemValue("AB_CurrentCycleTime",  m_dCycleTime);
				SetGemValue("AB_AverageCycleTime",  m_dAvgCycleTime);
				SetGemValue("AB_CycleCounter",      m_ulCycleCount);
			}
			m_dLastTime = dCurrentTime;

			if (IsCleanProbePinLimit())	//	CP Map Display
			{
				PBTZ_Sync();
				BOOL bReturn = FALSE;
				m_bWaferAlignComplete	= FALSE;
				if( m_bACPEnable )
				{
					bReturn = OpAutoCleanProbePin();
				}
				else
				{
					bReturn = OpManualCleanProbePin();
				}
				m_bWaferAlignComplete	= TRUE;
				PBTZ_MoveTo(m_lStandByLevel_PBTZ, SFM_WAIT);
				if( bReturn==TRUE )
				{
					m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
					break;
				}
			}

			// Check whether the collet & ejector count is larger than the user-defined maximum
			// Check probe-pin difference exceed the limit
			if ( IsMaxProbePinLimit() == TRUE )
			{
				SetStatusMessage("Max Probe Pin Limit");
				SetErrorMessage("Max Probe Pin Limit");
				m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
			} 
			else if (m_bTesterTimeOut == TRUE)
			{
				SetErrorMessage("CP100 tester time out");
				SetAlert_Red_Back(IDS_BH_TESTER_TIME_OUT);
				m_qSubOperation = CP_BH_HOUSE_KEEPING_Q;
			}
			else
			{
				m_qSubOperation = CP_WAIT_WT_READY_TO_PROBE;
				SetBackupNVRam(TRUE);
				DisplaySequence("CPZ - update counter&time ==> Wait WT Ready");
			}
			break;

		case CP_BH_HOUSE_KEEPING_Q:
			DisplaySequence("CPZ - House Keeping Q");
			if (GetHouseKeepingToken("BondHeadStn") == TRUE)
			{
				SetHouseKeepingToken("BondHeadStn", FALSE);
			}
			else
			{
				Sleep(10);
				break;
			}

			if ( m_bIsEnableOpenDac == TRUE )
			{
				MotionSelectStaticControl(BH_AXIS_PROBER_Z, &m_stBHAxis_ProberZ, PL_STATIC);	// PL_OPEN_DAC
			}

			NuMotionDataLogExt(FALSE);

			if ( bNoMotionHouseKeeping == FALSE )
			{
				PBTZ_Sync();

				if (PBTZ_IsPowerOn() && m_bIsWaferEnded == TRUE )
				{
					PBTZ_MoveToFocusLevel();
				}
			}

			if ( m_bIsProbeErrorInCycle == TRUE )
			{
				// rotate the PR
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

				// 2a:Turn On light
				LONG lOpticsID = 0;
				stMsg.InitMessage(sizeof(LONG), &lOpticsID);		
				nConvID = m_comClient.SendRequest("BondPrStn", "SetCleanProbeLighting", stMsg);
				SetPrAutoBondMode(FALSE);
				// 3: move to P-Level
				PBTZ_MoveTo(m_lProbeFocusLevelZ);
				Sleep(100);

				LONG lAlarmStatus = GetAlarmLamp_Status();
				SetAlarmLamp_Red(m_bEnableAlarmLampBlink, FALSE);
				CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
				CSingleLock slLock(&pAppMod->m_csMachineAlarmLamp);
				slLock.Lock();
				CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
				CString szMsg;
				szMsg.Format("Please check the probe position");

				SetAlert_Msg(IDS_BH_CHECK_PROBE_POS, szMsg, "", "" ,"" , glHMI_ALIGN_CENTER, 450, 0);

				slLock.Unlock();
				SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);
			}

			if ( m_bProcessBlockSavingEncorder == TRUE )
			{
				CMSLogFileUtility::Instance()->CP100_PinLogClose();
			}

			SelectToolingStage(MS_TOOLINGSTAGE_CAM);
			//ClosePPP(FALSE);	// house keeping
			CMS896AStn::m_lBondHeadAtSafePos = 1;

			if( m_bACPLifeExpired )
			{
				HmiMessage_Red_Back("Auto clean pin cloth expired.\nPlease replace it.", "Auto Clean Pin");
			}

			LogCycleStopState("CPZ - UpdateDieCounter");
			UpdateDieCounter();			//Update die counter in HMI

			LogCycleStopState("CPZ - LogWaferInfo");
			LogWaferInformation(FALSE);	//Update current wafer counter

			LogCycleStopState("CPZ - MachineStat");
			SaveMachineStatistic();		//update machine statistic	

			SaveAllProbeCount();

			// Acknowledge other stations to stop
			StopCycle("BondHeadStn");				
			LogCycleStopState("CPZ - stop completely");
			State(STOPPING_Q);
			SetHouseKeepingToken("BondHeadStn", TRUE);
			break;

		default:
			DisplaySequence("CPZ - Unknown");
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
				szMsg.Format("CPZ - Heap corrupted (%d) before BH [%d]", nHeapStatus, 
								m_qSubOperation);
				DisplayMessage(szMsg);
				LogCycleStopState(szMsg);
			}
		}
	}
	catch (CAsmException e)
	{
		//Handle normal stop cycle task
		LogCycleStopState("BH ex - UpdateDieCounter");
		UpdateDieCounter();			

		LogCycleStopState("BH ex - LogWaferInfo");
		LogWaferInformation(FALSE);	

		LogCycleStopState("BH ex - MachineStat");
		SaveMachineStatistic();		

		CMS896AStn::m_lBondHeadAtSafePos = 1;

		DisplayException(e);
		Result(gnNOTOK);

		State(STOPPING_Q);

		NeedReset(TRUE);
		StopCycle("BondHeadStn");
		(*m_psmfSRam)["MS899"]["CriticalError"] = TRUE;
	}
}
