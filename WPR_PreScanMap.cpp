/////////////////////////////////////////////////////////////////
// WPR_PreScanMap.cpp : 
//
//	Description:
//		
//
//	Date:		17 July 2019
//	Revision:	1.00
//
//	By:			Matthew Law
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "WPR_PreScanMap.h"

LONG CPreScanMap::m_lWaferTotalDie = 0;
LONG CPreScanMap::m_lWaferDieCounter = 0;

CPreScanMap::CPreScanMap()
{
	m_bIsDieBond		= FALSE;
	m_bIsDieSetup		= FALSE;
	m_lWaferDieEnc_X	= 0;
	m_lWaferDieEnc_Y	= 0;
}


CPreScanMap::~CPreScanMap()
{
}


VOID CPreScanMap::InitObj()
{
	m_bIsDieBond		= FALSE;
	m_bIsDieSetup		= FALSE;
	m_lWaferDieEnc_X	= 0;
	m_lWaferDieEnc_Y	= 0;
}