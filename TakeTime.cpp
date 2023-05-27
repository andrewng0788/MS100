/////////////////////////////////////////////////////////////////
// TakeTime.cpp : Global function for measuring the time
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, November 19, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TakeTime.h"
#include <afxmt.h>
#include "PrescanUtility.h"
#include "LogFileUtil.h"
#include "MS896A.h"
#include "PRFailureCaseLog.h"
#include "MarkDieRegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		RECORD_SIZE		100000

typedef struct {
	LONG	lLocation;
	LONG	lTime;
} TIME_STRUCT;
static TIME_STRUCT*	g_astTime = new TIME_STRUCT[RECORD_SIZE];
static long			g_lCount	= 0;
static LONGLONG		g_llFirst	= 0;
static LARGE_INTEGER	g_liFreq={1};

static CCriticalSection g_cs;
static CCriticalSection g_csScanEventLog;
static CCriticalSection g_csErrHandling;

static BL_Time_Record	pBLTime;
static BOOL	bBLSave = FALSE;
static DOUBLE gdLastTime = 0;

// Get the current time
DOUBLE GetTime(VOID)
{
	g_cs.Lock();
	LARGE_INTEGER	liTime, liFreq;

	QueryPerformanceCounter(&liTime);
	QueryPerformanceFrequency(&liFreq);

	g_cs.Unlock();
	return ((DOUBLE)liTime.QuadPart * 1000.0)/ (DOUBLE)liFreq.QuadPart;
}

// Restart the time
VOID StartTime(LONG lRecord)
{
	g_cs.Lock();
	LARGE_INTEGER	liTime;

	if (g_lCount >= RECORD_SIZE) 
		g_lCount = 0;

	g_astTime[g_lCount].lLocation = REC_NUM;
	g_astTime[g_lCount].lTime = lRecord;
	g_lCount++;

	QueryPerformanceFrequency(&g_liFreq);
	QueryPerformanceCounter(&liTime);
	g_llFirst = liTime.QuadPart;
	g_cs.Unlock();
}

// Get the start time
LONGLONG GetStartTime(VOID)
{
	return g_llFirst;
}

// Record the time with given symbol
VOID TakeTime(TIME_ENUM emLocation)
{
	g_cs.Lock();
	LARGE_INTEGER	liTime;

	if ( g_lCount < RECORD_SIZE )
	{
		QueryPerformanceCounter(&liTime);
		g_astTime[g_lCount].lLocation = (LONG)emLocation;
//		g_astTime[g_lCount++].lTime = (LONG)(liTime.QuadPart - g_llFirst);
		g_astTime[g_lCount].lTime = (LONG)((((DOUBLE)(liTime.QuadPart - g_llFirst) * 1000.0)/(DOUBLE)g_liFreq.QuadPart) + 0.5);
		g_lCount++;
	}
	g_cs.Unlock();
}

// Record the given data
VOID TakeData(TIME_ENUM emLocation, LONG lData)
{
	g_cs.Lock();
	if ( g_lCount < RECORD_SIZE )
	{
		g_astTime[g_lCount].lLocation = (LONG)emLocation;
		g_astTime[g_lCount].lTime = lData;
		g_lCount++;
	}
	g_cs.Unlock();
}

// Clear the logging buffer
VOID ResetBuffer(VOID)
{
	g_lCount = 0;
}

// Save the time to file
VOID SaveTimeToFile(VOID)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanAreaPickMode() )
	{
		return SaveTimeToFile_AP();
	}

	if( CMS896AApp::m_bES100v2DualWftOption )
	{
		return SaveTimeToFile_ES();
	}

	FILE				*pfFile = NULL;
	LONG				i, j;
	LARGE_INTEGER		lnFreq;
	LONG				alTime[100];
	CString				szFileName;

	QueryPerformanceFrequency(&lnFreq);

	//andrewng //2020-0616
	//szFileName = "C:\\MapSorter\\UserData\\History\\Time.txt";
	szFileName = "C:\\MapSorter\\UserData\\History\\Time.csv";

	errno_t nErr = fopen_s(&pfFile, szFileName, "w");
	if ((nErr == 0) && (pfFile != NULL))
	{
		//fprintf(pfFile, "\n        T1   T2   T3   T4   Z1   Z2   Z3   Z4   Z5   Z6   Z7   Z8  EJ1  EJ2  EJ3  EJ4  WT1  WT2 WPR1 WPR2 WPR3 WPR4  CP1  CP2  BT1  BT2 BPR1 BPR2 BPR3   CJ   MD  PV1  PV2  EV1  EV2   BD  CJO  MDO  RD  LC1  LC2  LC3  LC4  LC5  LC6  LC7  LC8  LC9  LC10	CycleTime");
		//fprintf(pfFile, "\n,T1,T2,T3,T4,BT1,BT2,BT3,WT1,WT2,WT3,WPR1,WPR2,WPR3,WPR4,CP1,CP2,BPR1,BPR2,BPR3,CJ,MD,PV1,PV2,EV1,EV2,EJ1,EJ2,EJ3,EJ4,Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,BD,CJO,MDO,RD,LC1,LC2,LC3,LC4,LC5,LC6,LC7,LC8,LC9,LC10,CycleTime");
		//fprintf(pfFile, "\n,T1,T2,T3,T4,Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,EJ1,EJ2,EJ3,EJ4,WT1,WT2,WPR0,WPR1,WPR2,WPR3,WPR4,WPR5,WPR6,CP1,CP2,BT1,BT2,BPR1,BPR2,BPR3,CJ,MD,PV1,PV2,EV1,EV2,BD,CJO,MDO,RD,LC1,LC2,LC3,LC4,LC5,LC6,LC7,LC8,LC9,LC10,CycleTime");
		
		//andrewng //2020-0507
		//fprintf(pfFile,	"\n,T1,  T2,  T3,  T4,  Z1, Z2, Z3, Z4, Z5,  Z6,  Z7,  Z8,  EJ1,EJ2,EJ3,EJ4,WT1,WT2,WPR0,WPR1,WPR2,WPR3,WPR4,WPR5,WPR6,CP1,CP2,BT1,BT2,BPR1,BPR2,BPR3,CJ,MD,PV1,PV2,EV1,EV2,BD,CJO,MDO,RD,LC1,LC2,LC3,LC4,LC5,LC6,LC7,LC8,LC9,LC10,CycleTime");
		fprintf(pfFile,		"\n,BJT1,BJT2,BJT3,BJT4,BJ1,BJ2,BJ3,BJ4,EJT1,EJT2,EJT3,EJT4,EJ1,EJ2,EJ3,EJ4,WT1,WT2,WPR0,WPR1,WPR2,WPR3,WPR4,WPR5,WPR6,CP1,CP2,BT1,BT2,BPR1,BPR2,BPR3,CJ,MD,PV1,PV2,EV1,EV2,BD,CJO,MDO,RD,LC1,LC2,LC3,LC4,LC5,LC6,LC7,LC8,LC9,LC10,CycleTime");
		
		for (j = 0; j < (LONG)Last_Enum; j++ )
		{
			alTime[j] = 0;
		}
 
		for ( i=0; i < g_lCount; i++ )
		{
			if ( g_astTime[i].lLocation < (LONG)Last_Enum )
				alTime[g_astTime[i].lLocation] = g_astTime[i].lTime;
			// Check whether the cycle is end
			if ( (g_astTime[i+1].lLocation == (LONG)REC_NUM) || ( i == g_lCount-1) )
			{
				fprintf(pfFile, "\n");
				for (j=0; j < (LONG)Last_Enum; j++ )
				{
					//fprintf(pfFile, "%5ld", alTime[j]);
					fprintf(pfFile, "%5ld,", alTime[j]);
					alTime[j] = 0;
				}
			}
		}
		fclose(pfFile);
	}

	CString szTgtFileName, szStopTime;
	CTime ctTime = CTime::GetCurrentTime();
	szStopTime = ctTime.Format("_%m%d_%H%M%S");
	szTgtFileName = PRESCAN_RESULT_FULL_PATH + "AI Log\\Time" + szStopTime + ".txt";
//	CopyFile(szFileName, szTgtFileName, FALSE);
}

VOID SetBLStartTime()
{
	if (bBLSave == FALSE)
	{
		return;
	}	

	INT	i;
	g_cs.Lock();
	LARGE_INTEGER	liTime;

	//Init
	pBLTime.dChgFrameStart = 0;
	for (i=0; i<6; i++)
	{
		pBLTime.dBLX_MoveTo[i] = 0;
		pBLTime.dBLX_Arrive[i] = 0;
	}
	for (i=0; i<3; i++)
	{
		pBLTime.dBLY_MoveTo[i] = 0;
		pBLTime.dBLY_Arrive[i] = 0;
		pBLTime.dBLZ_MoveTo[i] = 0;
		pBLTime.dBLZ_Arrive[i] = 0;
	}
	for (i=0; i<4; i++)
	{
		pBLTime.dBTX_MoveTo[i] = 0;
		pBLTime.dBTX_Arrive[i] = 0;
		pBLTime.dBTY_MoveTo[i] = 0;
		pBLTime.dBTY_Arrive[i] = 0;
	}
	for (i=0; i<10; i++)
	{
		pBLTime.dFrameActLoad[i] = 0;
		pBLTime.dFrameActUnld[i] = 0;
	}
	pBLTime.dChgFrameEnd = 0;
	//

	QueryPerformanceFrequency(&g_liFreq);
	QueryPerformanceCounter(&liTime);
	pBLTime.dChgFrameStart = liTime.QuadPart;
	g_cs.Unlock();
}

VOID SetBLCurTime(INT iField, INT iElement)
{
	if (bBLSave == FALSE)
	{
		return;
	}	

	g_cs.Lock();
	LARGE_INTEGER	liTime, liFreq;
	LONGLONG dCurTime;

	QueryPerformanceCounter(&liTime);
	QueryPerformanceFrequency(&liFreq);
	g_cs.Unlock();

	//dCurTime = (LONGLONG)(((DOUBLE)liTime.QuadPart * 1000.0)/ (DOUBLE)liFreq.QuadPart);
	dCurTime = (LONGLONG)(((DOUBLE)liTime.QuadPart * 1000.0)/ (DOUBLE)liFreq.QuadPart)/1000;
	
	INT nIndex = iElement;
	nIndex = min(iElement, 6);
	nIndex = max(iElement, 0);
	
	switch (iField)
	{
	case 0:		pBLTime.dChgFrameStart = dCurTime;			break;
	case 1:		pBLTime.dBLX_MoveTo[nIndex] = dCurTime;	break;
	case 2:		pBLTime.dBLX_Arrive[nIndex] = dCurTime;	break;
	case 3:		pBLTime.dBLY_MoveTo[nIndex] = dCurTime;	break;
	case 4:		pBLTime.dBLY_Arrive[nIndex] = dCurTime;	break;
	case 5:		pBLTime.dBLZ_MoveTo[nIndex] = dCurTime;	break;
	case 6:		pBLTime.dBLZ_Arrive[nIndex] = dCurTime;	break;
	case 7:		pBLTime.dBTX_MoveTo[nIndex] = dCurTime;	break;
	case 8:		pBLTime.dBTX_Arrive[nIndex] = dCurTime;	break;
	case 9:		pBLTime.dBTY_MoveTo[nIndex] = dCurTime;	break;
	case 10:	pBLTime.dBTY_Arrive[nIndex] = dCurTime;	break;
	case 11:	pBLTime.dFrameActLoad[nIndex] = dCurTime;	break;
	case 12:	pBLTime.dFrameActUnld[nIndex] = dCurTime;	break;
	case 13:	pBLTime.dChgFrameEnd = dCurTime;			break;
	default:	break;
	}

}

VOID SaveBLTime()
{
	INT		i;
	FILE	*pfFile = NULL;

	if (bBLSave == FALSE)
	{
		return;
	}	

	errno_t nErr = fopen_s(&pfFile, "BLTime.txt", "w");
	if ((nErr == 0) && (pfFile != NULL))
	{
		//Gripper
		fprintf(pfFile, "\nT0       BLX_Home  BLX_Ready BLX_Load  BLX_Scan  BLX_Unl   BLX_PreUnl");
		fprintf(pfFile, "\n%8d ", (INT) pBLTime.dChgFrameStart);		//Klocwork
		for (i=0; i<6; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBLX_MoveTo[i]);		//Klocwork
		}
		fprintf(pfFile, "\n         ");
		for (i=0; i<6; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBLX_Arrive[i]);		//Klocwork
		}
		fprintf(pfFile, "\n         ");
		for (i=0; i<6; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) (pBLTime.dBLX_Arrive[i] - pBLTime.dBLX_MoveTo[i]) );
		}
		fprintf(pfFile, "\n");

		//Elevator
		fprintf(pfFile, "\nT0       BLY_Home  BLY_Ready BLY_Tgt   BLZ_Home  BLZ_Ready BLZ_Tgt  ");
		fprintf(pfFile, "\n%8d ", (INT) pBLTime.dChgFrameStart);
		for (i=0; i<3; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBLY_MoveTo[i]);
		}
		for (i=0; i<3; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBLZ_MoveTo[i]);
		}
		fprintf(pfFile, "\n         ");
		for (i=0; i<3; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBLY_Arrive[i]);
		}
		for (i=0; i<3; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBLZ_Arrive[i]);
		}
		fprintf(pfFile, "\n");

		//BinTable
		fprintf(pfFile, "\nT0       BTX_Home  BTX_Load  BTX_Unl   BTX_Tgt   BTY_Home  BTY_Load  BTY_Unl   BTY_Tgt  ");
		fprintf(pfFile, "\n%8d ", (INT) pBLTime.dChgFrameStart);
		for (i=0; i<4; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBTX_MoveTo[i]);
		}
		for (i=0; i<4; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBTY_MoveTo[i]);
		}
		fprintf(pfFile, "\n         ");
		for (i=0; i<4; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBTX_Arrive[i]);
		}
		for (i=0; i<4; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dBTY_Arrive[i]);
		}
		fprintf(pfFile, "\n");

		//OnFrame
		fprintf(pfFile, "\nT0       FmAlnOn   FmAlnOff 	VccmOn    VccmOff   FmLvlUp   FmLvlDn   GrpUp     GrpDn     GrpHold   GrpRels   ");
		fprintf(pfFile, "\n         ");
		for (i=0; i<10; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dFrameActLoad[i]);
		}
		fprintf(pfFile, "\n         ");
		for (i=0; i<10; i++)
		{
			fprintf(pfFile, "%8d  ", (INT) pBLTime.dFrameActUnld[i]);
		}
		fprintf(pfFile, "\n");

		//CycleEnd
		fprintf(pfFile, "\nCycleEnd:%8d\n", (INT) pBLTime.dChgFrameEnd);

		fclose(pfFile);
	}

}


VOID SaveScanTimeToFile(CONST ULONG ulPickCount)	// prescan_time
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	g_csScanEventLog.Lock();

	FILE				*pfFile = NULL;
	LONG				i, j;
	LARGE_INTEGER		lnFreq;
	LONG				alCycTime[100];
	CString				szFileName;

	QueryPerformanceFrequency(&lnFreq);

	CString szLogPartPath;
	pUtl->GetPrescanLogPath(szLogPartPath);
	szFileName = szLogPartPath + "_ScanTime.txt";
	if( ulPickCount>0 )
	{
		szFileName.Format("%s_%07lu%s", szLogPartPath, ulPickCount, RESCAN_TIMING_FILE);
	}
	szFileName = "C:\\MapSorter\\UserData\\History\\ScanTime.txt";

	errno_t nErr = fopen_s(&pfFile, szFileName, "w");
	if ((nErr == 0) && (pfFile != NULL))
	{
		fprintf(pfFile, " SERIAL IMGCMD IMGRP1 IMGRY2 IMGRY3 TOGRAB CTLRDY IMGLFT IMGDEL WTMVE1 WTMVE2 WTDLY1 WTDLY2 WTNXT1 WTNXT2 IMGDIE SNTime");
		for (j=0; j < (LONG)Last_Enum; j++ )
		{
			alCycTime[j] = 0;
		}
 
		LONG lTotalTime = 0;
		for ( i=0; i < g_lCount; i++ )
		{
			LONG lIndexType = g_astTime[i].lLocation;
			switch( lIndexType )
			{
			case SCAN_WPR_REC_NUM:		alCycTime[SCAN_WPR_REC_NUM]		= g_astTime[i].lTime;		break;
			case SCAN_WPR_IMG_GRAB:		alCycTime[SCAN_WPR_IMG_GRAB]	= g_astTime[i].lTime;		break;
			case SCAN_WPR_IMG_RPY1:		alCycTime[SCAN_WPR_IMG_RPY1]	= g_astTime[i].lTime;		break;
			case SCAN_WPR_IMG_RPY2:		alCycTime[SCAN_WPR_IMG_RPY2]	= g_astTime[i].lTime;		break;
			case SCAN_WPR_IMG_RPY3:		alCycTime[SCAN_WPR_IMG_RPY3]	= g_astTime[i].lTime;		break;
			case SCAN_WPR_FOCUS:		alCycTime[SCAN_WPR_FOCUS]		= g_astTime[i].lTime;		break;
			case SCAN_WPR_CTR_RDY:		alCycTime[SCAN_WPR_CTR_RDY]		= g_astTime[i].lTime;		break;
			case SCAN_WPR_IMG_LEFT:		alCycTime[SCAN_WPR_IMG_LEFT]	= g_astTime[i].lTime;		break;
			case SCAN_WPR_IMG_KBGN:		alCycTime[SCAN_WPR_IMG_KBGN]	= g_astTime[i].lTime;		break;
			case SCAN_WFT_MOVE_1:		alCycTime[SCAN_WFT_MOVE_1]		= g_astTime[i].lTime;		break;
			case SCAN_WFT_MOVE_2:		alCycTime[SCAN_WFT_MOVE_2]		= g_astTime[i].lTime;		break;
			case SCAN_WFT_PR_DLY1:		alCycTime[SCAN_WFT_PR_DLY1]		= g_astTime[i].lTime;		break;
			case SCAN_WFT_PR_DLY2:		alCycTime[SCAN_WFT_PR_DLY2]		= g_astTime[i].lTime;		break;
			case SCAN_WFT_NEXT_1:		alCycTime[SCAN_WFT_NEXT_1]		= g_astTime[i].lTime;		break;
			case SCAN_WFT_NEXT_2:		alCycTime[SCAN_WFT_NEXT_2]		= g_astTime[i].lTime;		break;
			case SCAN_WPR_IMG_KILL:		alCycTime[SCAN_WPR_IMG_KILL]	= g_astTime[i].lTime;		break;
			case SCAN_WFT_CT_44:		alCycTime[SCAN_CYCLE_LAST]		= g_astTime[i].lTime;		break;
			}

			LONG lNextType = g_astTime[i+1].lLocation;
			if( lNextType==SCAN_WPR_REC_NUM || ( i == g_lCount-1) )
			{
				if( alCycTime[0]==0 )
					continue;
				lTotalTime += alCycTime[SCAN_CYCLE_LAST];
				fprintf(pfFile, "\n");

				CString szLog = "";
				if( labs(alCycTime[SCAN_CYCLE_LAST]-alCycTime[SCAN_WFT_PR_DLY2])>10 )
				{
					szLog = "    STB, no grab;";
				}
				if( labs(alCycTime[SCAN_WPR_IMG_RPY1]-alCycTime[SCAN_WPR_IMG_GRAB])>=20 ) 
				{
					szLog += "    Long grab;";
				}
				if(	labs(alCycTime[SCAN_WPR_IMG_RPY2]-alCycTime[SCAN_WPR_IMG_RPY1])>(labs(alCycTime[SCAN_WFT_PR_DLY2]-alCycTime[SCAN_WFT_MOVE_1])+10) )
				{
					szLog += "Long transfer";
				}

				for(j=0; j<=SCAN_CYCLE_LAST; j++)
				{
					fprintf(pfFile, "%7ld", alCycTime[j]);
					alCycTime[j] = 0;
				}
				fprintf(pfFile, szLog);
			}
		}
		fprintf(pfFile, "\n");
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%2d (%2d:%2d:%2d)  WFT:  Stop prescan table index, Total Time %ld\n",
			theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), lTotalTime);
		fprintf(pfFile, "\n");
		fclose(pfFile);
	}

	g_csScanEventLog.Unlock();
}

VOID SaveScanTimeEvent(CString szLogMsg, BOOL bBackUp)	// prescan_time
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
		return ;

	g_csScanEventLog.Lock();

	// new file for sequencial loading
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "PrescanEvent.txt";
	if( bBackUp )
	{
		CStdioFile fLogFile;
		if( fLogFile.Open(szFileName, CFile::modeRead) )
		{
			ULONGLONG nFileSize = fLogFile.GetLength();
			fLogFile.Close();
			// Case of no need to backup the log file
			CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "PrescanEvent.bak";
			if( nFileSize>LOG_FILE_SIZE_LIMIT )
			{
				CopyFile(szFileName, szBkupName, FALSE);
				DeleteFile(szFileName);
			}
		}
	}

	FILE *pfFile = NULL;
	errno_t nErr = fopen_s(&pfFile, szFileName, "a");
	if ((nErr == 0) && (pfFile != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%2d (%2d:%2d:%2d)	%6ld	%s\n",
			theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LONG)(GetTime()-gdLastTime),	(LPCTSTR)szLogMsg);
		fclose(pfFile);
	}

	gdLastTime = GetTime();
	g_csScanEventLog.Unlock();
}

VOID SaveTestBitStatus(CString szLogMsg, BOOL bBackUp)	// prescan_time
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
		return ;

	g_csScanEventLog.Lock();

	// new file for sequencial loading
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "TestBitStatus.txt";
	if( bBackUp )
	{
		CStdioFile fLogFile;
		if( fLogFile.Open(szFileName, CFile::modeRead) )
		{
			ULONGLONG nFileSize = fLogFile.GetLength();
			fLogFile.Close();
			// Case of no need to backup the log file
			CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "TestBitStatus.bak";
			if( nFileSize>LOG_FILE_SIZE_LIMIT )
			{
				CopyFile(szFileName, szBkupName, FALSE);
				DeleteFile(szFileName);
			}
		}
	}

	FILE *pfFile = NULL;
	errno_t nErr = fopen_s(&pfFile, szFileName, "a");
	if ((nErr == 0) && (pfFile != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%2d (%2d:%2d:%2d)	%6ld	%s\n",
			theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LONG)(GetTime()-gdLastTime),	(LPCTSTR)szLogMsg);
		fclose(pfFile);
	}

	gdLastTime = GetTime();
	g_csScanEventLog.Unlock();
}

VOID SaveColletSizeEvent(CString szLogMsg, BOOL bBackUp)	// prescan_time
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
		return ;

	g_csScanEventLog.Lock();

	// new file for sequencial loading
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "ColletSizeEvent.txt";
	if( bBackUp )
	{
		CStdioFile fLogFile;
		if( fLogFile.Open(szFileName, CFile::modeRead) )
		{
			ULONGLONG nFileSize = fLogFile.GetLength();
			fLogFile.Close();
			// Case of no need to backup the log file
			CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "ColletSizeEvent.bak";
			if( nFileSize>LOG_FILE_SIZE_LIMIT )
			{
				CopyFile(szFileName, szBkupName, FALSE);
				DeleteFile(szFileName);
			}
		}
	}

	FILE *pfFile = NULL;
	errno_t nErr = fopen_s(&pfFile, szFileName, "a");
	if ((nErr == 0) && (pfFile != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%d,%d,%2d (%2d:%2d:%2d)	%6ld	%s\n",
			theTime.GetYear(),theTime.GetMonth(),theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LONG)(GetTime()-gdLastTime),	(LPCTSTR)szLogMsg);
		fclose(pfFile);
	}

	g_csScanEventLog.Unlock();
}

VOID SaveAutoErrorHandlingLog(CString szLogMsg, CStringMapFile* pSMapFile, BOOL bBackUp)	// prescan_time
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
		return ;

	g_csErrHandling.Lock();
	CTime theTime = CTime::GetCurrentTime();
	CString szFileNameDate;
	szFileNameDate.Format("%d-%d-%2d",theTime.GetYear(),theTime.GetMonth(),theTime.GetDay());
	// new file for sequencial loading
	CString Path		= gszUSER_DIRECTORY + "\\History\\ErrorHandling";
	CString szFileName  = gszUSER_DIRECTORY + "\\History\\ErrorHandling\\" + szFileNameDate + "AutoErrorHandling.txt";
	CreateDirectory(Path, NULL);
	//if( bBackUp )
	//{
	//	CStdioFile fLogFile;
	//	if( fLogFile.Open(szFileName, CFile::modeRead) )
	//	{
	//		ULONGLONG nFileSize = fLogFile.GetLength();
	//		fLogFile.Close();
	//		// Case of no need to backup the log file
	//		CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "AutoErrorHandling.bak";
	//		if( nFileSize>LOG_FILE_SIZE_LIMIT )
	//		{
	//			CopyFile(szFileName, szBkupName, FALSE);
	//			DeleteFile(szFileName);
	//		}
	//	}
	//}
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(Path, 30);
	LONG lBTGrade		=  (*pSMapFile)["BinTable"]["BondingGrade"];
	FILE *pfFile = NULL;
	errno_t nErr = fopen_s(&pfFile, szFileName, "a");
	if ((nErr == 0) && (pfFile != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%d-%d-%2d,%2d:%2d:%2d,%s,%d\n",
			theTime.GetYear(),theTime.GetMonth(),theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(),	(LPCTSTR)szLogMsg, lBTGrade);
		fclose(pfFile);
	}

	g_csErrHandling.Unlock();
}

VOID SaveWaferEndInfo(CString szLogMsg, CString szWaferId, BOOL bBackUp)	// prescan_time
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
		return ;

	g_csErrHandling.Lock();
	CTime theTime = CTime::GetCurrentTime();
	//CString szFileNameDate;
	//szFileNameDate.Format("%d%d%2d",theTime.GetYear(),theTime.GetMonth(),theTime.GetDay());
	// new file for sequencial loading
	CString Path		= gszUSER_DIRECTORY + "\\History\\WaferEndInfo";
	CString szFileName  = gszUSER_DIRECTORY + "\\History\\WaferEndInfo\\" + szWaferId + ".txt";
	CreateDirectory(Path, NULL);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(Path, 45);

	FILE *pfFile = NULL;
	errno_t nErr = fopen_s(&pfFile, szFileName, "a");
	if ((nErr == 0) && (pfFile != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%d-%d-%2d (%2d:%2d:%2d)	%s\n",
			theTime.GetYear(),theTime.GetMonth(),theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(),	(LPCTSTR)szLogMsg);
		fclose(pfFile);
	}

	g_csErrHandling.Unlock();
}


VOID SaveLogFile(CString szFileName, CString szBkupName, CString szLogMsg, BOOL bBackUp)
{
	if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
		return ;

	//g_csScanEventLog.Lock();

	// new file for sequencial loading
	if (bBackUp)
	{
		CStdioFile fLogFile;
		if( fLogFile.Open(szFileName, CFile::modeRead) )
		{
			ULONGLONG nFileSize = fLogFile.GetLength();
			fLogFile.Close();
			// Case of no need to backup the log file
			if( nFileSize > LOG_FILE_SIZE_LIMIT )
			{
				CopyFile(szFileName, szBkupName, FALSE);
				DeleteFile(szFileName);
			}
		}
	}

	FILE *pfFile = NULL;
	errno_t nErr = fopen_s(&pfFile, szFileName, "a");
	if ((nErr == 0) && (pfFile != NULL))
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%2d (%2d:%2d:%2d)	%6ld	%s\n",
			theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), (LONG)(GetTime()-gdLastTime),	(LPCTSTR)szLogMsg);
		fclose(pfFile);
	}

	//gdLastTime = GetTime();
	//g_csScanEventLog.Unlock();
}


VOID SaveBHMark1(CString szLogMsg, BOOL bBackUp)
{
	if (CPRFailureCaseLog::Instance()->IsEnableBHMarkCaselog())
	{
		CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "BHMark1.txt";
		CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "BHMark1.bak";
		SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
	}
}

VOID SaveBHMark2(CString szLogMsg, BOOL bBackUp)
{
	if (CPRFailureCaseLog::Instance()->IsEnableBHMarkCaselog())
	{
		CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "BHMark2.txt";
		CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "BHMark2.bak";
		SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
	}
}

/*
VOID SaveWTMark1(CString szLogMsg, BOOL bBackUp)
{
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "WTMark1.txt";
	CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "WTMark1.bak";
	SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
}

VOID SaveWTMark2(CString szLogMsg, BOOL bBackUp)
{
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "WTMark2.txt";
	CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "WTMark2.bak";
	SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
}
*/
VOID SaveEJTMark1(CString szLogMsg, BOOL bBackUp)
{
	if (CPRFailureCaseLog::Instance()->IsEnableBHMarkCaselog())
	{
		CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "EJTMark1.txt";
		CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "EJTMark1.bak";
		SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
	}
}

VOID SaveEJTMark2(CString szLogMsg, BOOL bBackUp)
{
	if (CPRFailureCaseLog::Instance()->IsEnableBHMarkCaselog())
	{
		CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "EJTMark2.txt";
		CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "EJTMark2.bak";
		SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
	}
}


VOID SaveMarkDieOffset(CString szLogMsg, BOOL bBackUp)
{
	WM_CMarkDieRegionManager *pMarkDieRegionManager = WM_CMarkDieRegionManager::Instance();
	if (pMarkDieRegionManager->IsEnableScanRegionMarkDieCaseLog())
	{
		CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "MarkDieOffset.txt";
		CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "MarkDieOffset.bak";
		SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
	}
}


VOID SaveBHArm1Offset(CString szLogMsg, BOOL bBackUp)
{
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "BHArm1Offset.txt";
	CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "BHArm1Offset.bak";
	SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
}

VOID SaveBHArm2Offset(CString szLogMsg, BOOL bBackUp)
{
	CString szFileName = gszUSER_DIRECTORY + "\\History\\" + "BHArm2Offset.txt";
	CString szBkupName = gszUSER_DIRECTORY + "\\History\\" + "BHArm2Offset.bak";
	SaveLogFile(szFileName, szBkupName, szLogMsg, bBackUp);
}

// Save the area pick time to file	
VOID SaveTimeToFile_AP(VOID)
{
	FILE				*pfFile = NULL;
	LONG				i, j;
	LARGE_INTEGER		lnFreq;
	LONG				alTime[100];
	CString				szFileName;

	QueryPerformanceFrequency(&lnFreq);

	szFileName = "C:\\MapSorter\\UserData\\History\\Time.txt";

	errno_t nErr = fopen_s(&pfFile, szFileName, "w");
	if ((nErr == 0) && (pfFile != NULL))
	{
		fprintf(pfFile, "\n        T1   T2   T3   T4   Z1   Z2   Z3   Z4   Z5   Z6   Z7   Z8  EJ1  EJ2  EJ3  EJ4  WT1  WT2  CP1  CP2  BT1  BT2 BPR1 BPR2 BPR3   CJ   MD  PV1  PV2  EV1  EV2   BD  CJO  MDO  RD  CycleTime");
		for (j=0; j < (LONG)Last_Enum; j++ )
		{
			alTime[j] = 0;
		}
 
		for ( i=0; i < g_lCount; i++ )
		{
			if ( g_astTime[i].lLocation < (LONG)Last_Enum )
				alTime[g_astTime[i].lLocation] = g_astTime[i].lTime;
			// Check whether the cycle is end
			if ( (g_astTime[i+1].lLocation == (LONG)REC_NUM) || ( i == g_lCount-1) )
			{
				fprintf(pfFile, "\n");
				for(j=0; j<=WT2; j++)
				{
					fprintf(pfFile, "%5ld", alTime[j]);
					alTime[j] = 0;
				}
				for(j=CP1; j<=RD; j++)
				{
					fprintf(pfFile, "%5ld", alTime[j]);
					alTime[j] = 0;
				}
				fprintf(pfFile, "%5ld", alTime[CycleTime]);
				alTime[CycleTime] = 0;
			}
		}
		fclose(pfFile);
	}
}

VOID SaveTimeToFile_ES(VOID)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	FILE				*pfFile = NULL;
	LONG				i, j;
	LARGE_INTEGER		lnFreq;
	LONG				alTime[100], alAction[100];
	CString				szFileName;

	QueryPerformanceFrequency(&lnFreq);

	szFileName = "C:\\MapSorter\\UserData\\History\\Time.txt";

	errno_t nErr = fopen_s(&pfFile, szFileName, "w");
	if ((nErr == 0) && (pfFile != NULL))
	{
		fprintf(pfFile, "\n       EJ1  EJ2  EJ3  EJ4  EV1  WT1  WT2  EV2  T1   T2   T3   T4   CycleTime");
		for (j=0; j < (LONG)Last_Enum; j++ )
		{
			alTime[j] = 0;
			alAction[j] = 0;
		}
 
		for ( i=0; i < g_lCount; i++ )
		{
			if ( g_astTime[i].lLocation < (LONG)Last_Enum )
				alTime[g_astTime[i].lLocation] = g_astTime[i].lTime;
			for (j=0; j < (LONG)Last_Enum; j++ )
			{
				alAction[j] = 0;
			}
			// Check whether the cycle is end
			if ( (g_astTime[i+1].lLocation == (LONG)REC_NUM) || ( i == g_lCount-1) )
			{
				fprintf(pfFile, "\n");
				alAction[0] = alTime[REC_NUM];
				alAction[1] = alTime[EJ1];
				alAction[2] = alTime[EJ2];
				alAction[3] = alTime[EJ3];
				alAction[4] = alTime[EJ4];
				alAction[5] = alTime[EV1];
				alAction[6] = alTime[WT1];
				alAction[7] = alTime[WT2];
				alAction[8] = alTime[EV2];
				alAction[9] = alTime[T1];
				alAction[10] = alTime[T2];
				alAction[11] = alTime[T3];
				alAction[12] = alTime[T4];
				alAction[13] = alTime[CycleTime];                            	
				for (j=0; j < 14; j++ )
				{
					fprintf(pfFile, "%5ld", alAction[j]);
				}
			}
		}
		fclose(pfFile);
	}
}
