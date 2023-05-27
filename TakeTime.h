/////////////////////////////////////////////////////////////////
// TakeTime.h : Header file for timing function
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
#include "StringMapFile.h"
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/*--- Enumerator for Time Taking ---*/
typedef	enum {
	REC_NUM=0,							/* Record number */
	T1, T2, T3, T4,						/* Bond Head T */
	Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8,		/* Bond Head Z */
	EJ1, EJ2, EJ3, EJ4,					/* Ejector */
	WT1, WT2,							/* Wafer Table */
	WPR0, WPR1, WPR2, WPR3, WPR4, WPR5, WPR6,	/* Wait Pr */
	CP1, CP2,							/* WT Compensation */
	BT1, BT2,							/* Bin Table */
	BPR1, BPR2, BPR3,					/* Bond Pr */
	CJ, MD,								/* Collet Jam and Missing Die Check */
	PV1, PV2,							/* Pick Vacuum */
	EV1, EV2,							/* Ejector Vacuum */
	BD, CJO, MDO, RD,					/* Bad Die, Collet Jam Occur, Missing Die Occur, Replace Die */
	LC1, LC2, LC3, LC4,					/* ILC1, ILC2, ILC3, ILC4*/
	LC5, LC6, LC7, LC8,	LC9, LC10,		/* ILC5, ILC6, ILC7, ILC8, ILC9, ILC10*/
	CycleTime,							/* Cycle Time */
//	BHT_ENC,							/* BH T Encoder value */
	Last_Enum
} TIME_ENUM;

typedef enum	{
	CP_TIME_START = Z3, WTB = Z3, WTE, WTM, ZUB, ZUE, SOT, PDB, ZDB, PDT, EOT, ADN, ZDE, UTB, UTE, CP_TIME_END
} CP_TIME_ENUM;

typedef	enum {
	SCAN_WPR_REC_NUM=0,	SCAN_WPR_IMG_GRAB,	SCAN_WPR_IMG_RPY1,	SCAN_WPR_IMG_RPY2,	SCAN_WPR_IMG_RPY3,
						SCAN_WPR_FOCUS,		SCAN_WPR_CTR_RDY,	SCAN_WPR_IMG_LEFT,	SCAN_WPR_IMG_KBGN,
						SCAN_WFT_MOVE_1,	SCAN_WFT_MOVE_2,	SCAN_WFT_PR_DLY1,	SCAN_WFT_PR_DLY2,
						SCAN_WFT_NEXT_1,	SCAN_WFT_NEXT_2,	SCAN_WPR_IMG_KILL,	SCAN_CYCLE_LAST,	SCAN_WFT_CT_44 = 44
} PRESCAN_TIME_ENUM;

//Time Record for BL
typedef struct
{
	LONGLONG	dChgFrameStart;
	LONGLONG	dBLX_MoveTo[6];				// BLX	0:Home,1:Ready,2:Load,3:Barcode,4:Unload,5:PreUnload
	LONGLONG	dBLX_Arrive[6];
	LONGLONG	dBLY_MoveTo[3];				// BLY	0:Home,1:Ready,2:TgtPos
	LONGLONG	dBLY_Arrive[3];
	LONGLONG	dBLZ_MoveTo[3];				// BLZ	0:Home,1:Ready,2:TgtPos
	LONGLONG	dBLZ_Arrive[3];
	LONGLONG	dBTX_MoveTo[4];				// BTX	0:Home,1:Load, 2:Unload, 3:TgtPos
	LONGLONG	dBTX_Arrive[4];	
	LONGLONG	dBTY_MoveTo[4];				// BTY	0:Home,1:Load, 2:Unload, 3:TgtPos
	LONGLONG	dBTY_Arrive[4];
	LONGLONG	dFrameActLoad[10];			// OnFrame	0:FrameAlignOn,	2:VaccumOn,	4:FrameLevelUp,	6:GripperUp, 8:GripHold, No.11
	LONGLONG	dFrameActUnld[10];			//			1:FrameAlignOff,3:VaccumOff,5:FrameLevelDn,	7:GripperDn, 9:GripRels
	LONGLONG	dChgFrameEnd;
} BL_Time_Record;

DOUBLE GetTime(VOID);					// Get current time (absolute count)
VOID StartTime(LONG lRecord);			// Start the time
LONGLONG GetStartTime(VOID);			// Get the start time			
VOID TakeTime(TIME_ENUM emTime);		// Record the time
VOID TakeData(TIME_ENUM emTime, LONG lData);	// Record the given data
VOID ResetBuffer(VOID);					// Reset the logging buffer
VOID SaveTimeToFile(VOID);				// Save the time to file	
VOID SaveTimeToFile_CP(VOID);				// Save the time to file of prober	
VOID SaveTimeToFile_AP(VOID);		// Save the area pick time to file	
VOID SaveTimeToFile_ES(VOID);		// Save the ES NG pick time to file	
VOID SaveScanTimeToFile(CONST ULONG ulPickCount=0);		// Save the scan time to file	
VOID SaveScanTimeEvent(CString szLogMsg, BOOL bBackUp = FALSE);		// Save the scan event to file	
VOID SaveColletSizeEvent(CString szLogMsg, BOOL bBackUp = FALSE);
VOID SaveTestBitStatus(CString szLogMsg, BOOL bBackUp = FALSE);
VOID SaveBHMark1(CString szLogMsg, BOOL bBackUp = FALSE);
VOID SaveBHMark2(CString szLogMsg, BOOL bBackUp = FALSE);
VOID SaveAutoErrorHandlingLog(CString szLogMsg, CStringMapFile* pSMapFile = NULL, BOOL bBackUp = FALSE);
VOID SaveWaferEndInfo(CString szLogMsg, CString szWaferID, BOOL bBackUp = FALSE);
//VOID SaveWTMark1(CString szLogMsg, BOOL bBackUp = FALSE);
//VOID SaveWTMark2(CString szLogMsg, BOOL bBackUp = FALSE);
VOID SaveEJTMark1(CString szLogMsg, BOOL bBackUp = FALSE);
VOID SaveEJTMark2(CString szLogMsg, BOOL bBackUp = FALSE);
VOID SaveMarkDieOffset(CString szLogMsg, BOOL bBackUp = TRUE);
VOID SaveBHArm1Offset(CString szLogMsg, BOOL bBackUp = TRUE);
VOID SaveBHArm2Offset(CString szLogMsg, BOOL bBackUp = TRUE);
VOID SetBLStartTime();
VOID SetBLCurTime(INT iField, INT iElement);
VOID SaveBLTime();
