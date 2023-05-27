/////////////////////////////////////////////////////////////////
// BT_BondingMap.cpp : 
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
#include "BT_BondingMap.h"

#pragma once

LONG CBondingMap::m_lBondCounter = 0;

CBondingMap::CBondingMap()
{
	m_bIsDieBond		= FALSE;
	m_bIsDieSetup		= FALSE;
	m_lBondDieEnc_X		= 0;
	m_lBondDieEnc_Y		= 0;
}


CBondingMap::~CBondingMap()
{
}


VOID CBondingMap::InitObj()
{
	m_bIsDieBond		= FALSE;
	m_bIsDieSetup		= FALSE;
	m_lBondDieEnc_X		= 0;
	m_lBondDieEnc_Y		= 0;
}


LONG CBondingMap::ResetBondCounter()
{
	CBondingMap::m_lBondCounter = 0;
	return 1;
}


LONG CBondingMap::AddBondCounter()
{
	LONG lBondCounter = CBondingMap::m_lBondCounter++;
	return lBondCounter;
}


LONG CBondingMap::GetBondCounter()
{
	return CBondingMap::m_lBondCounter;
}