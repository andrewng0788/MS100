/////////////////////////////////////////////////////////////////
// BT_BondingMap.h : 
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

#pragma once

#ifndef _BT_BONDINGMAP_H_
#define _BT_BONDINGMAP_H_

#include "MS896A.h"
#include "MS896A_Constant.h"


class CBondingMap : public CObject
{
public:
	CBondingMap(); //Constructor
	~CBondingMap(); //Deconstructor

	VOID InitObj();
	static LONG ResetBondCounter();
	static LONG AddBondCounter();
	static LONG GetBondCounter();

public:
	LONG	m_lBondDieEnc_X;
	LONG	m_lBondDieEnc_Y;
	BOOL	m_bIsDieBond;
	BOOL	m_bIsDieSetup;
	static LONG	 m_lBondCounter;
};
#endif