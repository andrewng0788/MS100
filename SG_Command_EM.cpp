#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "SecsComm.h"
#include "SecsMapCoord.h"
#include "SecsMapCoordData.h"
#include "LogFileUtil.h"
#include "EqpStatReportTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LONG HOST_GetCmdParaStringItem(SPara *pHostCmdParamRec, char *szParaName, short sParaValueLength, char *szParaValue)
{
	if (strlen(pHostCmdParamRec->pchName) == 0) 
	{
		return HCMD_PARA_CPNAME_EMPTY;    //Parameter Name (CPNAME) does not exist
	}
	if (strcmp(strupr(pHostCmdParamRec->pchName), szParaName) != 0) 
	{
		return HCMD_PARA_CPNAME_NOT_DESIRED;    ////Parameter Name (CPNAME) Error
	} 
	if (pHostCmdParamRec->nType == SECSASCII)
	{
		if ((short)strlen(pHostCmdParamRec->pchValue) > sParaValueLength)
		{
			strncpy_s(szParaValue, sParaValueLength, pHostCmdParamRec->pchValue, sParaValueLength);
			szParaValue[sParaValueLength - 1] = 0;
		}
		else
		{
			strcpy_s(szParaValue, sParaValueLength, pHostCmdParamRec->pchValue);
		}
		return HCMD_PARA_SUCC; //Obtain Parameter sucessfully
	}
	return HCMD_PARA_CPVAL_ILLEGAL_FORMAT; //Illegal Format Specified for CPVAL
}


LONG HOST_GetCmdParaTime(SHostCmmd *pstCmdRecv, short sIndex, char *szParaName, Stat_Time_Stamp &stTime, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;
	char szTime[60];

	strcpy_s(szTime, sizeof(szTime), "");
	SPara *pHostCmdParamRec = &(pstCmdRecv->arrCmmd[sIndex]);
	if ((lRet = HOST_GetCmdParaStringItem(pHostCmdParamRec, szParaName, 32, szTime)) != 0)
	{
		return lRet;
	}

	sscanf(szTime, "%d-%d-%d %d:%d:%d", &stTime.lYear, &stTime.lMonth, &stTime.lDay, &stTime.lHour, &stTime.lMinute, &stTime.lSecond);
	if ((stTime.lYear < 2012) || (stTime.lMonth < 1) || (stTime.lMonth > 12) ||
		(stTime.lDay < 1) || (stTime.lDay > 31) ||
		(stTime.lHour < 0) || (stTime.lHour > 24) ||
		(stTime.lMinute < 0) || (stTime.lMinute > 60) ||
		(stTime.lSecond < 0) || (stTime.lSecond > 60)) 
	{
		lRet = HCMD_PARA_CPVAL_ILLEGAL_VALUE; //Illegal Value Specified for CPVAL 
		return lRet;
	}
	return HCMD_PARA_SUCC;
}

LONG HOST_GetCmdParaLOTID(SHostCmmd *pstCmdRecv, short sIndex, char *szLOTID, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;
	if (szLOTID == NULL)
	{
		return lRet;
	}

	strcpy_s(szLOTID, sizeof(szLOTID), "");
	SPara *pHostCmdParamRec = &(pstCmdRecv->arrCmmd[sIndex]);
	if ((lRet = HOST_GetCmdParaStringItem(pHostCmdParamRec, "LOTID", 32, szLOTID)) != 0)
	{
		strcpy_s(szParaErrName, 60, "LOTID"); 
		return lRet;
	}
	return HCMD_PARA_SUCC;
}

//=====================================================================================================================
//   Start Report Host Command
//=====================================================================================================================
LONG HOST_GetCmdParaStartReportAt(SHostCmmd *pstCmdRecv, short sIndex, Stat_Time_Stamp &stStartReportAt, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;

	if (lRet = HOST_GetCmdParaTime(pstCmdRecv, sIndex, "STARTREPORTAT", stStartReportAt, szParaErrName))
	{
		strcpy_s(szParaErrName, 60, "StartReportAt"); 
	} 
	return lRet;
}


LONG HOST_GetCmdParaIntervalInMinutes(SHostCmmd *pstCmdRecv, short sIndex, LONG &lIntervalInMinutes, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;
	char szIntervalInMinutes[60];

	strcpy_s(szIntervalInMinutes, sizeof(szIntervalInMinutes), "");
	SPara *pHostCmdParamRec = &(pstCmdRecv->arrCmmd[sIndex]);
	if ((lRet = HOST_GetCmdParaStringItem(pHostCmdParamRec, "INTERVALINMINUTES", 32, szIntervalInMinutes)) != 0)
	{
		strcpy_s(szParaErrName, sizeof(szParaErrName), "IntervalInMinutes"); 
		return lRet;
	}
	lIntervalInMinutes = atoi(szIntervalInMinutes);
	return HCMD_PARA_SUCC;
}


//================================================================
// Function Name: 		HOST_GetConfigCycleRptCommand
// Input arguments:		pstCmdRecv -- the pointer of Secs Host Command Recive Record.
// Output arguments:	szStartReportAt ---  Start Report Time, at this time send report first time
//						szIntervalInMinutes ---  After some Minutes, and send report again
// Description:   		Called by HOST_ConfigCycleRptCommand
// Return:				0 -- Obtain Parameter sucessfully
//						1 -- Parameter Name (CPNAME) does not exist
//						2 -- Illegal Value Specified for CPVAL 
//						3 -- Illegal Format Specified for CPVAL
//						4 -- Illegal Parameter Name(CPNAME), not desired
// Remarks:				None
//================================================================
LONG CSecsComm::HOST_GetConfigCycleRptCommand(SHostCmmd *pstCmdRecv, Stat_Time_Stamp &stStartReportAt, LONG &lIntervalInMinutes, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;
	short i = 0;

	strcpy_s(szParaErrName, 60, "");
	if (pstCmdRecv->nParameter == 0) 
	{
		return 1;    // No Parameter
	}
	if ((lRet = HOST_GetCmdParaStartReportAt(pstCmdRecv, i++, stStartReportAt, szParaErrName)) != 0) 
	{
		return lRet;
	}
	if ((lRet = HOST_GetCmdParaIntervalInMinutes(pstCmdRecv, i++, lIntervalInMinutes, szParaErrName)) != 0) 
	{
		return lRet;
	}
	return HCMD_PARA_SUCC;
}


LONG CSecsComm::HOST_ConfigCycleRptCommand(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr)
{
	Stat_Time_Stamp stStartReportAt;
	LONG lIntervalInMinutes = 0;
	char szParaErrName[60];
	memset(&stStartReportAt, 0x00, sizeof(Stat_Time_Stamp));

	LONG lParaErrorCode = HOST_GetConfigCycleRptCommand(pHostCmmd, stStartReportAt, lIntervalInMinutes, szParaErrName);
	switch (lParaErrorCode)
	{
	case HCMD_PARA_SUCC:
		CEqpStatTime::Instance()->SetConfigCycleRptTime(stStartReportAt, lIntervalInMinutes);
		SaveData();
		pstHostCmmdErr->Ack = HCACK_COMPLETED; // Acknowledge, Command has been performed
		pstHostCmmdErr->nErr = 0;
		break;
	default:
		HOST_CommandParaErrorHandle(pstHostCmmdErr, lParaErrorCode, szParaErrName);
		break;
	}
	return 0;
}


//=====================================================================================================================
//  Recall Statistics Host Command
//=====================================================================================================================
LONG HOST_GetCmdParaFromTime(SHostCmmd *pstCmdRecv, short sIndex, Stat_Time_Stamp &stFromTime, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;

	if (lRet = HOST_GetCmdParaTime(pstCmdRecv, sIndex, "FROMTIME", stFromTime, szParaErrName))
	{
		strcpy_s(szParaErrName, 60, "FromTime"); 
	} 
	return lRet;
}

LONG HOST_GetCmdParaToTime(SHostCmmd *pstCmdRecv, short sIndex, Stat_Time_Stamp &stToTime, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;

	if (lRet = HOST_GetCmdParaTime(pstCmdRecv, sIndex, "TOTIME", stToTime, szParaErrName))
	{
		strcpy_s(szParaErrName, 60, "ToTime"); 
	} 
	return lRet;
}

//================================================================
// Function Name: 		HOST_GetConfigCycleRptCommand
// Input arguments:		pstCmdRecv -- the pointer of Secs Host Command Recive Record.
// Output arguments:	szStartReportAt ---  Start Report Time, at this time send report first time
//						szIntervalInMinutes ---  After some Minutes, and send report again
// Description:   		Called by HOST_ConfigCycleRptCommand
// Return:				0 -- Obtain Parameter sucessfully
//						1 -- Parameter Name (CPNAME) does not exist
//						2 -- Illegal Value Specified for CPVAL 
//						3 -- Illegal Format Specified for CPVAL
//						4 -- Illegal Parameter Name(CPNAME), not desired
// Remarks:				None
//================================================================
LONG CSecsComm::HOST_GetRecallStatisticsCommand(SHostCmmd *pstCmdRecv, CString &strLotID, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;
	short i = 0;
	char szLOTID[60];

	strcpy_s(szParaErrName, 60, "");
	if (pstCmdRecv->nParameter == 0) 
	{
		return 1;    // No Parameter
	}
	if ((lRet = HOST_GetCmdParaLOTID(pstCmdRecv, i++, szLOTID, szParaErrName)) != 0) 
	{
		return lRet;
	}
	strLotID = szLOTID;
	return HCMD_PARA_SUCC;
}

LONG CSecsComm::HOST_GetRecallStatisticsCommand(SHostCmmd *pstCmdRecv, Stat_Time_Stamp &stFromTime, Stat_Time_Stamp &stToTime, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;
	short i = 0;

	strcpy_s(szParaErrName, 60, "");
	if (pstCmdRecv->nParameter == 0) 
	{
		return 1;    // No Parameter
	}
	if ((lRet = HOST_GetCmdParaFromTime(pstCmdRecv, i++, stFromTime, szParaErrName)) != 0) 
	{
		return lRet;
	}
	if ((lRet = HOST_GetCmdParaToTime(pstCmdRecv, i++, stToTime, szParaErrName)) != 0) 
	{
		return lRet;
	}
	return HCMD_PARA_SUCC;
}

LONG CSecsComm::HOST_RecallStatistcsCommand(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr)
{
	BOOL bOK = TRUE;
//	if (pHostCmmd->nParameter == 1)
//	{
//		return HOST_RecallStatistcsLERCommand(pHostCmmd);
//	}
	Stat_Time_Stamp stFromTime;
	Stat_Time_Stamp stToTime;
	char szParaErrName[60];
	memset(&stFromTime, 0x00, sizeof(Stat_Time_Stamp));
	memset(&stToTime, 0x00, sizeof(Stat_Time_Stamp));
	LONG lParaErrorCode = HOST_GetRecallStatisticsCommand(pHostCmmd, stFromTime, stToTime, szParaErrName);
	switch (lParaErrorCode)
	{
	case HCMD_PARA_SUCC:
		bOK = CEqpStatTime::Instance()->SetRecallStatistics(stFromTime, stToTime);
		if (!bOK)
		{
			pstHostCmmdErr->Ack = HCACK_CANNOT_PERFORM_NOW; // Cannot perform now
			pstHostCmmdErr->nErr = 0; 
		}
		else
		{
			pstHostCmmdErr->Ack = HCACK_COMPLETED; // Acknowledge, Command has been performed
			pstHostCmmdErr->nErr = 0;
		}
		break;
	default:
		HOST_CommandParaErrorHandle(pstHostCmmdErr, lParaErrorCode, szParaErrName);
		break;
	}
	return 0;
}


//=====================================================================================================================
//  Auto Delete Statistics Host Command
//=====================================================================================================================
LONG HOST_GetCmdParaBeforeTime(SHostCmmd *pstCmdRecv, short sIndex, Stat_Time_Stamp &stBeforeTime, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;

	if (lRet = HOST_GetCmdParaTime(pstCmdRecv, sIndex, "BEFORETIME", stBeforeTime, szParaErrName))
	{
		strcpy_s(szParaErrName, 60, "BeforeTime"); 
	} 
	return lRet;
}

//================================================================
// Function Name: 		HOST_GetConfigCycleRptCommand
// Input arguments:		pstCmdRecv -- the pointer of Secs Host Command Recive Record.
// Output arguments:	szStartReportAt ---  Start Report Time, at this time send report first time
//						szIntervalInMinutes ---  After some Minutes, and send report again
// Description:   		Called by HOST_ConfigCycleRptCommand
// Return:				0 -- Obtain Parameter sucessfully
//						1 -- Parameter Name (CPNAME) does not exist
//						2 -- Illegal Value Specified for CPVAL 
//						3 -- Illegal Format Specified for CPVAL
//						4 -- Illegal Parameter Name(CPNAME), not desired
// Remarks:				None
//================================================================
LONG CSecsComm::HOST_GetDeleteStatisticsCommand(SHostCmmd *pstCmdRecv, Stat_Time_Stamp &stBeforeTime, char *szParaErrName)
{
	LONG lRet = HCMD_PARA_SUCC;
	short i = 0;

	strcpy_s(szParaErrName, 60, "");
	if (pstCmdRecv->nParameter == 0) 
	{
		return 1;    // No Parameter
	}
	if ((lRet = HOST_GetCmdParaBeforeTime(pstCmdRecv, i++, stBeforeTime, szParaErrName)) != 0) 
	{
		return lRet;
	}
	return HCMD_PARA_SUCC;
}

//================================================================
// Function Name: 		HOST_DeleteStatistcsCommand
// Input arguments:		pstCmdRecv -- the pointer of Secs Host Command Recive Record.
// Output arguments:	
// Description:   		Called by HOST_HostCmdFunc
// Return:				0 -- Obtain Parameter sucessfully
//						1 -- Parameter Name (CPNAME) does not exist
//						2 -- Illegal Value Specified for CPVAL 
//						3 -- Illegal Format Specified for CPVAL
//						4 -- Illegal Parameter Name(CPNAME), not desired
// Remarks:				None
//================================================================
LONG CSecsComm::HOST_DeleteStatistcsCommand(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr)
{
	Stat_Time_Stamp stBeforeTime;
	char szParaErrName[60];

	memset(&stBeforeTime, 0x00, sizeof(Stat_Time_Stamp));
	LONG lParaErrorCode = HOST_GetDeleteStatisticsCommand(pHostCmmd, stBeforeTime, szParaErrName);
	switch (lParaErrorCode)
	{
	case HCMD_PARA_SUCC:
		if (!CEqpStatTime::Instance()->DeleteEquipStateTime(stBeforeTime))
		{
			pstHostCmmdErr->Ack = HCACK_CANNOT_PERFORM_NOW; // Cannot perform now
			pstHostCmmdErr->nErr = 0;
		}
		else
		{
			pstHostCmmdErr->Ack = HCACK_COMPLETED; // Acknowledge, Command has been performed
			pstHostCmmdErr->nErr = 0;
		}
		break;
	default:
		HOST_CommandParaErrorHandle(pstHostCmmdErr, lParaErrorCode, szParaErrName);
		break;
	}
	return 0;
}

/*
ACK of S2F42 should follow customer's definition
  HCACK
	1 = Command does not exists 
	2 = After Conversation Timeout, and receive CANCEL command from Host
	3 = Parameter is invalid
	if HCACK = 1 or 2, no need to send CPACK
	if HCACK = 3, send CPACK
  CPACK
	1 = Incorrect Parameter (CPNAME)
	2 = Incorrect Value (CPVAL)

const	UCHAR	CPACK_NAME_NOT_EXIST			= 0x01;
const	UCHAR	CPACK_ILLEGAL_VALUE				= 0x02;
const	UCHAR	CPACK_ILLEGAL_FORMAT			= 0x03;
*/
VOID CSecsComm::HOST_CommandParaErrorHandle(SHostCmmdErr *pstHostCmmdErr, LONG lParaErrorCode, char *szParaErrName)
{
	if (pstHostCmmdErr == NULL)
	{
		return;
	}

	switch (lParaErrorCode)
	{
	case HCMD_PARA_EMPTY:
		pstHostCmmdErr->Ack = HCACK_PARA_INVALID; // Acknowledge, At least one parameter is invalid 
		pstHostCmmdErr->nErr = 1; 
		strcpy_s(pstHostCmmdErr->arrParaErr[0].pchName, sizeof(pstHostCmmdErr->arrParaErr[0].pchName), "No Parameter List");
		pstHostCmmdErr->arrParaErr[0].ucAck = CPACK_NAME_NOT_EXIST; //Parameter Name (CPNAME) does not exist
		break;
	case HCMD_PARA_CPNAME_EMPTY:
	case HCMD_PARA_CPNAME_NOT_DESIRED:
		pstHostCmmdErr->Ack = HCACK_PARA_INVALID; // Acknowledge, At least one parameter is invalid
		pstHostCmmdErr->nErr = 1; 
		strcpy_s(pstHostCmmdErr->arrParaErr[0].pchName, sizeof(pstHostCmmdErr->arrParaErr[0].pchName), szParaErrName);
		pstHostCmmdErr->arrParaErr[0].ucAck = CPACK_NAME_NOT_EXIST; //Parameter Name (CPNAME) is not desired for command
		break;
	case HCMD_PARA_CPVAL_ILLEGAL_VALUE:
	case HCMD_PARA_CPVAL_NOT_DESIRED:
		pstHostCmmdErr->Ack = HCACK_PARA_INVALID; // Acknowledge, At least one parameter is invalid
		pstHostCmmdErr->nErr = 1; 
		strcpy_s(pstHostCmmdErr->arrParaErr[0].pchName, sizeof(pstHostCmmdErr->arrParaErr[0].pchName), szParaErrName);
		pstHostCmmdErr->arrParaErr[0].ucAck = CPACK_ILLEGAL_VALUE; //Parameter Value (CPVALUE) is not desired for command
		break;
	case HCMD_PARA_CPVAL_ILLEGAL_FORMAT:
		pstHostCmmdErr->Ack = HCACK_PARA_INVALID; // Acknowledge, At least one parameter is invalid 
		pstHostCmmdErr->nErr = 1; 
		strcpy_s(pstHostCmmdErr->arrParaErr[0].pchName, sizeof(pstHostCmmdErr->arrParaErr[0].pchName), szParaErrName);
		pstHostCmmdErr->arrParaErr[0].ucAck = CPACK_ILLEGAL_FORMAT; //Parameter Value (CPVALUE) Illegal format 
		break;
	}
}


//=========================================================================================================
//                             Equip Statistics Info
//=========================================================================================================
VOID CSecsComm::ReportEquipStatisticsInfo()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (!pApp->m_eqMachine.IsEquipmentManager())
	{
		return;
	}
	pApp->m_eqMachine.SetEMHourlyRecipe(pApp->m_szDeviceFile);
	LONG lRet = pApp->m_eqMachine.ReportEquipStatisticsInfo();

	switch (lRet)
	{
	case 4: //log the statistics data at Start time
	case 0:
		break;
	case 3:
//		SendModuleErrorInfo();
//		SendEndLotEqpStatisticsInfo(pLotManager->m_pCurLotInfo->GetLotID());
		break;
	default:
		//SendModuleErrorInfo();
		SendEqpStatisticsInfo();
		//Auto Delete
//		m_pEqpStatTime->AutoDeleteEquipStateTime();
		break;
	}
}


BOOL CSecsComm::SendEqpStatisticsInfo()
{
    if (IsSecsGemInit() == FALSE)
	{
        return FALSE;
	}

	FILE *fs = NULL;
	char *buf = NULL;
	CString szINIContent;

	try
	{
		errno_t nErr = fopen_s(&fs, EQP_STAT_REPORT_FILE, "rb");
		if ((nErr != 0) || (fs == NULL))
		{
			return FALSE;
		}

		long size = _filelength(_fileno(fs));
		buf = (char *)malloc(size);
		if (buf == NULL)
		{
			fclose(fs);
			return FALSE;
		}
		
		if (fread(buf, 1, size, fs) != size)
		{
			fclose(fs);
			free(buf);
			return FALSE;
		}

		szINIContent = buf;
		SetGemValue(MS_SECS_SV_ES_INIBODY, szINIContent);
	}
	catch (...) //CAsmException e)
	{
//		CDebugLog::Instance()->ForceLogMsg(TRUE, "CEqpStat::SendEqpStatisticsInfo...%d", iTmp); // [514V9-25-68#2 20151202]
	}
	if (fs)
	{
		fclose(fs);
	}
	if (buf)
	{
		free(buf);
	}

	CString szCurrDate;
	CTime curTime = CTime::GetCurrentTime();
	szCurrDate = curTime.Format("%Y-%m-%d %H:%M:%S");

	CString szLog;
	szLog.Format("\t>>> CEID #%d : (Date = " + szCurrDate, SG_CEID_EQUIPMENT_STATISTIC_DATA_EVENT);
	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);

	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szINIContent);

	SendEvent(SG_CEID_EQUIPMENT_STATISTIC_DATA_EVENT);		//v4.37T5
	return TRUE;
}


/*
//Equipment time statistics
VOID CMS896AStn::SetEquipmentStatistic(CTime curTime, CTime lastTime, ULONG ulNotifyInterval, BOOL bSend)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
    if( IsSecsGemInit()==FALSE )
        return;
	if (!m_bEquipmentManager)	//v4.59A32
		return;

	ULONG ulIntervalInSec = ulNotifyInterval * 60;		//Transform from min to sec

	CTimeSpan timeDiff			= curTime - lastTime;
	ULONG ulTotalTimeInterval	= (ULONG)timeDiff.GetTotalSeconds();

	ULONG ulOutputCount		= pApp->m_eqMachine.GetUnit() - m_ulEMLastOutput;
	INT nNoOfAlarm			= pApp->m_eqMachine.GetNoOfAlarm() - m_ulEMLastNumOfFailures;
	INT nNoOfAssist			= pApp->m_eqMachine.GetNoOfAssist() - m_ulEMLastNumOfAssists;
	ULONG ulProductiveTime	= pApp->m_eqMachine.GetTime(EQUIP_RUN_TIME) - m_ulEMLastProdTime;
	ULONG ulDownTime		= pApp->m_eqMachine.GetTime(EQUIP_DOWN_TIME) - m_ulEMLastDownTime;
	ULONG ulEngTime			= pApp->m_eqMachine.GetTime(EQUIP_SETUP_TIME) - m_ulEMLastSetupTime;
	//ULONG ulStandbyTime		= pApp->m_eqMachine.GetTime(EQUIP_IDLE_TIME) - m_ulEMLastStandbyTime;
	ULONG ulStandbyTime		= ulTotalTimeInterval - ulProductiveTime - ulDownTime - ulEngTime;

	DOUBLE dMTBA = 0;
	if ((nNoOfAssist) > 0)
		dMTBA = 1.0 * ulProductiveTime / nNoOfAssist;
	else
		dMTBA = 1.0 * ulProductiveTime;

	DOUBLE dMTBF = 0;
	if ((nNoOfAlarm) > 0)
		dMTBF = 1.0 * ulProductiveTime / nNoOfAlarm;
	else
		dMTBF = 1.0 * ulProductiveTime;

//	SetGemValue(MS_SECS_SV_UNIT_PER_HOUR,			(INT) m_dMachineUPH);
//	SetGemValue(MS_SECS_SV_EQUIP_MTBA,				(INT) nMTBA);
//	SetGemValue(MS_SECS_SV_EQUIP_MTBF,				(INT) nMTBF);
//	SetGemValue(MS_SECS_SV_TOTAL_DOWN_TIME,			(INT) pApp->m_eqMachine.GetTime(EQUIP_DOWN_TIME));
//	SetGemValue(MS_SECS_SV_TOTAL_IDLE_TIME,			(INT) pApp->m_eqMachine.GetTime(EQUIP_IDLE_TIME));
//	SetGemValue(MS_SECS_SV_TOTAL_PRODUCTIVE_TIME,	(INT) pApp->m_eqMachine.GetTime(EQUIP_RUN_TIME));
//	SetGemValue(MS_SECS_SV_NO_OF_FAILURE,			(INT) nNoOfAlarm);
//	SetGemValue(MS_SECS_SV_NO_OF_ASSIST,			(INT) nNoOfAssist);
//	SetGemValue(MS_SECS_SV_STATISTIC_START_DATE,	(CString) pApp->m_eqMachine.GetEquipDate());
//	if ( bSend == TRUE )
//	{
//		SendEvent(SG_CEID_EQUIPMENT_BASIC_STATISTIC);
//	}


	CString szINIContent = _T("");

	CString szCurrDate, szLastDate;
	szLastDate = lastTime.Format("%Y-%m-%d %H:%M:%S");
	szCurrDate = curTime.Format("%Y-%m-%d %H:%M:%S");

	CString szHeader, szRecipeSection;
	CString szLine1, szLine2, szLine3, szLine4, szLine5, szLine6, szLine7, szLine8, szLine9, szLine10, szLine11;
	CString szOutput, szProductiveTime, szStandbyTime, szSDownTime, szNonDownTime, szDownTime, szSetupTime, szNumOfAssits, szNumOfFailures;
	CString szMTBA, szMTBF, szUPH;
	CString szLot		= _T("");	//(*m_psmfSRam)["MS896A"]["LotNumber"];
	CString szRecipe	= pApp->m_szDeviceFile;
	CString szOEE;

	szOutput.Format("%lu",			ulOutputCount);
	szProductiveTime.Format("%lu",	ulProductiveTime);
	szStandbyTime.Format("%lu",		ulStandbyTime);
	szSetupTime.Format("%lu",		ulEngTime);
	szSDownTime						= _T("0");
	szDownTime.Format("%lu",		ulDownTime);
	szNumOfAssits.Format("%d",		nNoOfAssist);
	szNumOfFailures.Format("%d",	nNoOfAlarm);
	szMTBA.Format("%.1f",			dMTBA);
	szMTBF.Format("%.1f",			dMTBF);
	szOEE.Format("%.2f",			1.00 * ulProductiveTime / ulIntervalInSec);
	szNonDownTime = _T("0");

	DOUBLE dUPH = 0;
	if (ulProductiveTime > 0)
		dUPH = 1.00 * ulOutputCount / ulProductiveTime * 3600.0;
	szUPH.Format("%.2f", dUPH);

	//INI Header Secton
	szLine1 = _T("[Overall]\r");
	szLine2 = _T("Start=") + szLastDate + _T("\r");
	szLine3 = _T("End=") + szCurrDate + _T("\r");
	szLine4 = _T("Loaded Recipes=") + szRecipe + _T("\r");
	szLine5 = _T("Loaded Lots=") + szLot + _T("\r\r");
	szHeader = szLine1 + szLine2 + szLine3 + szLine4 + szLine5;

	//Update Last EM Data in seconds
	m_ulEMLastOutput		= pApp->m_eqMachine.GetUnit();
	m_ulEMLastProdTime		= pApp->m_eqMachine.GetTime(EQUIP_RUN_TIME);
	m_ulEMLastStandbyTime	= pApp->m_eqMachine.GetTime(EQUIP_IDLE_TIME);
	m_ulEMLastDownTime		= pApp->m_eqMachine.GetTime(EQUIP_DOWN_TIME);
	m_ulEMLastSetupTime		= pApp->m_eqMachine.GetTime(EQUIP_SETUP_TIME);
	m_ulEMLastNumOfAssists	= pApp->m_eqMachine.GetNoOfAssist();
	m_ulEMLastNumOfFailures	= pApp->m_eqMachine.GetNoOfAlarm();

	//INI Receipe Section
	szLine1 = _T("[") + szRecipe + _T("]\n\r");
	szLine2 = _T("Output=")							+ szOutput			+ _T("\r");
	szLine3 = _T("Total Productive Time=")			+ szProductiveTime	+ _T("\r");
	szLine4 = _T("Total Standby Time=")				+ szStandbyTime		+ _T("\r");
	szLine5 = _T("Total Engineering Time=")			+ szSetupTime		+ _T("\r");
	szLine6 = _T("Total Scheduled Down Time=")		+ szSDownTime		+ _T("\r");
	szLine7 = _T("Total Unscheduled Down Time=")	+ szDownTime		+ _T("\r");
	szLine8 = _T("Total NonScheduled Down Time=")	+ szNonDownTime		+ _T("\r");
	szLine9 = _T("Number Of Assists=")				+ szNumOfAssits		+ _T("\r");
	szLine10 = _T("Number Of Failures=")			+ szNumOfFailures	+ _T("\r");
	szRecipeSection = szLine1 + szLine2 + szLine3 + szLine4 + szLine5 + szLine6 + szLine7 + 
						szLine8 + szLine9 + szLine10;
	szLine1 = _T("Mean Time Between Assists=")		+ szMTBA			+ _T("\r");
	szLine2 = _T("Mean Time Between Failures=")		+ szMTBF			+ _T("\r");
	szLine3 = _T("OEE=")							+ szOEE				+ _T("\r");
	szLine4 = _T("Unit Per Hour=")							+ szUPH				+ _T("\r");
	szLine5 = _T("\r");
	szRecipeSection = szRecipeSection + szLine1	+ szLine2 + szLine3 + szLine4 + szLine5;


	szINIContent = szHeader + szRecipeSection;
	szINIContent.Replace("\r", "\n");	//v4.59A33

	SetGemValue(MS_SECS_SV_ES_INIBODY, szINIContent);

	if ( bSend == TRUE )
	{
		CString szLog;
		szLog.Format("\t>>> CEID #%d : (Date = " + szCurrDate, SG_CEID_EQUIPMENT_STATISTIC_DATA_EVENT);
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
		
		//szINIContent.Replace("\r", "\n");
		CMSLogFileUtility::Instance()->MS_LogSECSGEM(szINIContent);

		SendEvent(SG_CEID_EQUIPMENT_STATISTIC_DATA_EVENT);		//v4.37T5
	}
}
*/


BOOL CSecsComm::EM_GenerateCyclicReport()
{
	if( !IsSecsGemInit() || (m_pGemStation->m_hThread == NULL) )
	{
		return FALSE;
	}

	ReportEquipStatisticsInfo();
/*
	m_ulCyclicReportUpdateCount++;
	if (m_ulCyclicReportUpdateCount < 100)
	{
		return FALSE;
	}

	m_ulCyclicReportUpdateCount = 0;

	if (m_ulCNotifyIntervalInMinute == 0)
	{
		return FALSE;
	}

	CTimeSpan TimeDiff;
	CTime theTime	= CTime::GetCurrentTime();
	CTime lastTime	= CTime::CTime(	m_ulLastCyclicReportTimeInYear, 
									m_ulLastCyclicReportTimeInMonth, 
									m_ulLastCyclicReportTimeInDay, 
									m_ulLastCyclicReportTimeInHour, 
									m_ulLastCyclicReportTimeInMinute, 
									0, 0);

	TimeDiff	= theTime - lastTime;
	if (TimeDiff.GetTotalMinutes() < m_ulCNotifyIntervalInMinute)
	{
		return FALSE;
	}

	m_ulLastCyclicReportTimeInYear		= theTime.GetYear();
	m_ulLastCyclicReportTimeInMonth		= theTime.GetMonth();
	m_ulLastCyclicReportTimeInDay		= theTime.GetDay();
	m_ulLastCyclicReportTimeInHour		= theTime.GetHour();
	m_ulLastCyclicReportTimeInMinute	= theTime.GetMinute();
	SaveData();

	SetEquipmentStatistic(theTime, lastTime, m_ulCNotifyIntervalInMinute, TRUE);
*/
	return TRUE;
}


//=======================================================================================
//   Auto Delete Statistics Menu
//=======================================================================================
LONG CSecsComm::EquipStateStatisticsExpiredDays(IPC_CServiceMessage &svMsg)
{
	LONG lExpireDays;
	svMsg.GetMsg(sizeof(LONG), &lExpireDays);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp)
	{
		pApp->m_eqMachine.SetEMHourlyStatisticsLogExpiredDays(lExpireDays);
	}
	return TRUE;
}

//=========================================================================================================
//                            the period of Statistics Data Display in menu
//=========================================================================================================
LONG CSecsComm::EquipStateTimeLogDateInput(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		LONG lFromYear;
		LONG lFromMonth;
		LONG lFromDay;
		LONG lFromHour;
		LONG lFromMinute;
		LONG lFromSecond;

		LONG lToYear;
		LONG lToMonth;
		LONG lToDay;
		LONG lToHour;
		LONG lToMinute;
		LONG lToSecond;
	} SET;

	SET stInfo;
	svMsg.GetMsg(sizeof(SET), &stInfo);
//	CString szLog;
//	szLog.Format("From Time = %d-%d-%d %d:%d:%d", stInfo.lFromYear, stInfo.lFromMonth, stInfo.lFromDay, stInfo.lFromHour, stInfo.lFromMinute, stInfo.lFromSecond);
//	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
//	szLog.Format("To Time = %d-%d-%d %d:%d:%d", stInfo.lToYear, stInfo.lToMonth, stInfo.lToDay, stInfo.lToHour, stInfo.lToMinute, stInfo.lToSecond);
//	CMSLogFileUtility::Instance()->MS_LogSECSGEM(szLog);
	
	CEqpStatTime::Instance()->EquipStateTimeLogDateInput(stInfo.lFromYear, stInfo.lFromMonth, stInfo.lFromDay, stInfo.lFromHour, stInfo.lFromMinute, stInfo.lFromSecond,
														 stInfo.lToYear, stInfo.lToMonth, stInfo.lToDay, stInfo.lToHour, stInfo.lToMinute, stInfo.lToSecond);
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CSecsComm::LoadEquipStateTime(IPC_CServiceMessage &svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp)
	{
		pApp->m_eqMachine.SetEMHourlyRecipe(pApp->m_szDeviceFile);
		if (!pApp->m_eqMachine.SaveEquipStateTime())
		{
			HmiMessage("Can not log data into the database, please check it");
			return 0;
		}
		CEqpStatTime::Instance()->LoadEquipStateTime();
		HmiMessage("Statistics on Specific Period Loaded");
	}
	return 0;
}


//=========================================================================================================
//                           Delete the period of Statistics Data in menu
//=========================================================================================================
LONG CSecsComm::EquipStateTimeExpireDateInput(IPC_CServiceMessage &svMsg)
{
	typedef struct
	{
		LONG lExpireYear;
		LONG lExpireMonth;
		LONG lExpireDay;
		LONG lExpireHour;
		LONG lExpireMinute;
		LONG lExpireSecond;
	} SET;

	SET stInfo;
	svMsg.GetMsg(sizeof(SET), &stInfo);
	CString szLog;
	szLog.Format("From Time = %d-%d-%d %d:%d:%d", stInfo.lExpireYear, stInfo.lExpireMonth, stInfo.lExpireDay, stInfo.lExpireHour, stInfo.lExpireMinute, stInfo.lExpireSecond);
	CEqpStatTime::Instance()->EquipStateTimeExpireDateInput(stInfo.lExpireYear, stInfo.lExpireMonth, stInfo.lExpireDay, stInfo.lExpireHour, stInfo.lExpireMinute, stInfo.lExpireSecond);
	return TRUE;
}

// Add Function for Delete Statistics On or Before Specific Date
LONG CSecsComm::DeleteEquipStateTime(IPC_CServiceMessage &svMsg)
{
	//if (DispQuestion("Delete Records of Statistics On or Before Specific Date?(Y/N)") == glHMI_YES) 
	CString str;
	str = "Delete Records of Statistics On or Before Specific Date?(Y/N)";
	if (HmiMessage(str, "System Message", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		if (CEqpStatTime::Instance()->DeleteEquipStateTime())
		{
			HmiMessage("Records On or Before Specific Date Deleted");
			return TRUE;
		}
		else 
		{
			HmiMessage("Record Cannot Be Deleted Currently");
			return FALSE;
		}
	}
	return TRUE;
}


VOID CSecsComm::RegisterVariables_EquipmentStaistics_VAR()
{
	try
	{
		CEqpStatTime *pEqpStatTime = CEqpStatTime::Instance();
		RegVariable(_T("SPC_lStatisticsLogExpiredDays"),	&pEqpStatTime->m_pEqpStatReportTime->m_lStatisticsLogExpiredDays);

		RegVariable(_T("SPC_lStatisticsFromYear"),		&pEqpStatTime->m_stFromTime.lYear);
		RegVariable(_T("SPC_lStatisticsFromMonth"),		&pEqpStatTime->m_stFromTime.lMonth);
		RegVariable(_T("SPC_lStatisticsFromDay"),		&pEqpStatTime->m_stFromTime.lDay);
		RegVariable(_T("SPC_lStatisticsFromHour"),		&pEqpStatTime->m_stFromTime.lHour);
		RegVariable(_T("SPC_lStatisticsFromMinute"),	&pEqpStatTime->m_stFromTime.lMinute);
		RegVariable(_T("SPC_lStatisticsFromSecond"),	&pEqpStatTime->m_stFromTime.lSecond);

		RegVariable(_T("SPC_lStatisticsToYear"),		&pEqpStatTime->m_stToTime.lYear);
		RegVariable(_T("SPC_lStatisticsToMonth"),		&pEqpStatTime->m_stToTime.lMonth);
		RegVariable(_T("SPC_lStatisticsToDay"),			&pEqpStatTime->m_stToTime.lDay);
		RegVariable(_T("SPC_lStatisticsToHour"),		&pEqpStatTime->m_stToTime.lHour);
		RegVariable(_T("SPC_lStatisticsToMinute"),		&pEqpStatTime->m_stToTime.lMinute);
		RegVariable(_T("SPC_lStatisticsToSecond"),		&pEqpStatTime->m_stToTime.lSecond);

		RegVariable(_T("SPC_lStatisticsExpireYear"),	&pEqpStatTime->m_stExpireTime.lYear);
		RegVariable(_T("SPC_lStatisticsExpireMonth"),	&pEqpStatTime->m_stExpireTime.lMonth);
		RegVariable(_T("SPC_lStatisticsExpireDay"),		&pEqpStatTime->m_stExpireTime.lDay);
		RegVariable(_T("SPC_lStatisticsExpireHour"),	&pEqpStatTime->m_stExpireTime.lHour);
		RegVariable(_T("SPC_lStatisticsExpireMinute"),	&pEqpStatTime->m_stExpireTime.lMinute);
		RegVariable(_T("SPC_lStatisticsExpireSecond"),	&pEqpStatTime->m_stExpireTime.lSecond);

		CRunEqpStatTime *pRunEqpStatTime = CEqpStatTime::Instance()->GetRunEqpStatTime(FALSE);
		// 20120614 Yip: Add Variables for Showing Statistics of Specific Date
		RegVariable(_T("CSpecificProdTime"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szProductiveTime);
		RegVariable(_T("CSpecificStandByTime"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szStandbyTime);
		RegVariable(_T("CSpecificEngineeringTime"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szEngineeringTime);
		RegVariable(_T("CSpecificScheduledDownTime"),	&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szScheduledDownTime);
		RegVariable(_T("CSpecificUnScheduledDownTime"),	&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szUnScheduledDownTime);
		RegVariable(_T("CSpecificNoScheduledTime"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szNoScheduledTime);

		RegVariable(_T("CSpecificMfgTime"),				&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szManufacturingTime);
		RegVariable(_T("CSpecificOperTime"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szManufacturingTime);
		RegVariable(_T("CSpecificUpTime"),				&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szUpTime);
		RegVariable(_T("CSpecificDownTime"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szDownTime);
		RegVariable(_T("CSpecificTotalStopTime"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szTotalStopTime);
		RegVariable(_T("CSpecificTotalTime"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szTotalTime);
		RegVariable(_T("CSpecificIdleTime"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szIdleTime);

		RegVariable(_T("CSpecificMTBA"),				&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szMTBA);
		RegVariable(_T("CSpecificMTBF"),				&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szMTBF);
		RegVariable(_T("CSpecificMTTA"),				&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szMTTA);
		RegVariable(_T("CSpecificMTTR"),				&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.szMTTR);
		RegVariable(_T("Eqp_lSpecificTotalFailure"),	&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.lTotalFailure);
		RegVariable(_T("Eqp_lSpecificTotalAssistance"),	&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.lTotalAssistance);

		RegVariable(_T("Eqp_dOEE"),						&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatTimeString_HMI.dOEE);

		RegVariable(_T("Eqp_lGoodDieCount"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lGoodDieCount);
		RegVariable(_T("Eqp_lDefectDieCount"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lDefectDieCount);
		RegVariable(_T("Eqp_lNoDieCount"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lNoDieCount);
		RegVariable(_T("Eqp_lInkDieCount"),			&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lInkDieCount);
		RegVariable(_T("Eqp_lAngularDieCount"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lAngularDieCount);
		RegVariable(_T("Eqp_lMissingDieCount"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lMissingDieCount);
		RegVariable(_T("Eqp_lColletCloggedCount"),	&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lColletCloggedCount);
	
		RegVariable(_T("Eqp_lTotalUnitBonded"),		&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lTotalUnitBonded);
		RegVariable(_T("Eqp_lTotalPBIGoodUnitBonded"),	&pRunEqpStatTime->m_EqpStatTimeHMI.m_stStatEquipState_HMI.stCountStatis.m_lTotalPBIGoodUnitBonded);
		//=====================================================================================================================================
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
}