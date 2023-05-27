/////////////////////////////////////////////////////////////////
// WPR_PreScanMap.h : 
//
//	Description:
//		
//
//	Date:		12 September 2019
//	Revision:	1.00
//
//	By:			Matthew Law
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////


#pragma once

#include "MS896A.h"
#include "MS896A_Constant.h"


class CPreScanMap : public CObject
{
public:
	CPreScanMap(); //Constructor
	~CPreScanMap(); //Deconstructor

	VOID InitObj();

public:
	LONG	m_lWaferDieEnc_X;
	LONG	m_lWaferDieEnc_Y;
	BOOL	m_bIsDieBond;
	BOOL	m_bIsDieSetup;
	static LONG	 m_lWaferTotalDie;
	static LONG	 m_lWaferDieCounter;
};