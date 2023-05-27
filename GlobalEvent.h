/*
	Singleton class to store the event
*/
#pragma once

#include "stdafx.h"

class CGlobalEvent
{
public:
	static BOOL m_bAutoCycle;
	static BOOL m_bStopCycle;
	static BOOL m_bReInit;
	static BOOL m_bBH_PrePick;
	static BOOL m_bEJ_NextDie;
	static BOOL m_bNoDie;
	static BOOL m_bManualHandle;
	static BOOL m_bWT_Indexed;
	static BOOL m_bWT_Indexed_BT;
	static BOOL m_bFullBlock;
	static BOOL m_bPickReady;
	static BOOL m_bDieContact;
	static BOOL m_bToBond;
	static BOOL m_bBA_DiePlaced;
	static BOOL m_bBH_DiePlaced;
	static BOOL m_bToPick;
	static BOOL m_bBA_DiePicked;
	static BOOL m_bBT_Indexed;
	static BOOL m_bBH_Blocked_FromPick;
	static BOOL m_bBH_Blocked_FromBond;
	static BOOL m_bBH_Clean_ToPick;
	static BOOL m_bBH_Clean_ToBond;

	void ClearAll();

};