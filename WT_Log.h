/////////////////////////////////////////////////////////////////
// WT_Log.h : Header file for logging the Wafer Table Info
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

VOID WTLog_CreateRecord(LONG lID);
VOID WTLog_SaveRecordToFile(VOID);

