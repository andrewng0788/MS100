/////////////////////////////////////////////////////////////////
// DevFile.h : Header file for Device File Class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, January 21, 2005
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

#include "StringMapFile.h"

#define	HEADER_DIEPITCH	"Die_Pitch"
#define HEADER_MAXCOUNT	"Max_Count"
#define HEADER_BIN		"BIN"
#define HEADER_RECIPE	"RecipeName"
#define HEADER_LOTID	"Lot_ID"
#define HEADER_RESET	"Reset"


class CAmiFileDecode: public CObject
{
public:
	CAmiFileDecode();
	~CAmiFileDecode();

	VOID CleanUp();
	BOOL ReadCsvFormat(CONST CString szFilename);
	BOOL ReadFile(CString szFilename);
	BOOL IsResetAmiLoadFailStatus();
	
	BOOL m_bResetAmiLoadFailStatus;
	CString m_szDiePitchX;
	CString m_szDiePitchY;
	CString m_szMaxCount;
	CString m_szBinNo;
	CString m_szPKGFilename;
	CString m_szBinSummaryFilename;
	CStringArray m_szaHeaderItem;
	CStringArray m_szaItemValue;
}; 
