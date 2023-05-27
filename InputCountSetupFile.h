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

#define HEADER_NEW_BIN "NewBin"
#define HEADER_FULL_BIN_DIE	"FullBinDies"


typedef struct 
{
	unsigned char ucGrade;
	LONG lInputCount;
}INPUT_COUNT_INFO;

class CInputCountSetupFile: public CObject
{
	public:
		CInputCountSetupFile();
		~CInputCountSetupFile();
		VOID CleanUp();
		BOOL ReadFile(CString szFilename);
		BOOL ExtractData(CString szStr, CString& szBin, CString& szInputCount);
		BOOL GetInputCount(unsigned char ucGrade, LONG& InputCount);
		CString GetLastError();		//v4.41T2
	
	private:
		CStringArray m_szaHeaderItem;
		CArray<INPUT_COUNT_INFO, INPUT_COUNT_INFO> m_aData;
		CString m_szErrorCode;
}; 
