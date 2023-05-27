/////////////////////////////////////////////////////////////////
// BPR_Log.h : Header file for logging the Bond PR Info
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, January 20, 2005
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2005.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

typedef struct {
	LONG	lID;
	BOOL	bBTStable;
	BOOL	bBHTReady;
	BOOL	bPBGrab;
	BOOL	bGrabNDisp;
	BOOL	bPBGrabDone;
	BOOL	bPBInsp;
	BOOL	bGrabNDispDone;
	BOOL	bHouseKeeping;
} BPR_LOG_INFO;

VOID BPRLog_CreateRecord(LONG lID);
VOID BPRLog_ResetRecord(VOID);
BPR_LOG_INFO *BPRLog_GetRecord(VOID);
VOID BPRLog_SaveRecordToFile(VOID);

