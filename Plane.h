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

#define BT_BIN_BLOCK_NO		175

#define MAX_NO_OF_POINTS	3
#define MAX_NO_OF_EQUATION	3

class CPlane: public CObject
{
	public:
		CPlane();
		~CPlane();

		BOOL CalPlaneEqn(LONG lPosX[], LONG lPosY[], LONG lPosZ[]);
		BOOL CalZLevel(LONG lEqnNo, LONG lX, LONG lY, LONG& lZ);

	private:
		LONG m_lMaxLevel;
		LONG m_lMinLevel;
		LONG m_lAvgLevel;

		DOUBLE m_dFactorX[MAX_NO_OF_POINTS];
		DOUBLE m_dFactorY[MAX_NO_OF_POINTS];
		DOUBLE m_dFactorZ[MAX_NO_OF_POINTS];
		DOUBLE m_dFactorSum[MAX_NO_OF_POINTS];
}; 
