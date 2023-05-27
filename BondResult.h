/////////////////////////////////////////////////////////////////
// BondResult.h : Header file for logging the bond result
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tuesday, November 23, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


LONG GetRecordNumber(VOID);				// Get the record number
VOID NextBondRecord(VOID);				// Next record
VOID ResetBondRecord(VOID);				// Reset the logging record
VOID SaveRecordToFile(VOID);			// Save the bond record to file
