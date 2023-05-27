/////////////////////////////////////////////////////////////////
// BondResult.cpp : Global function for logging the bond result
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

#include "stdafx.h"
#include "BondResult.h"
#include <afxmt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		RECORD_SIZE		1000

static long			g_lCount	= 0;

static CCriticalSection g_cs;

// Get the die record number
LONG GetRecordNumber(VOID)
{
	return g_lCount;
}

// Next record
VOID NextBondRecord(VOID)
{
	g_cs.Lock();

	if ( g_lCount >= RECORD_SIZE )
		g_lCount = 0;

	g_lCount++;

	g_cs.Unlock();
}

// Reset the logging record
VOID ResetBondRecord(VOID)
{
	g_lCount = 0;
}

// Save the time to file
VOID SaveRecordToFile(VOID)
{
}
