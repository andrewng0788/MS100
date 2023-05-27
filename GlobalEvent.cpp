/*
	Singleton class to store the events
*/
#include "GlobalEvent.h"

BOOL CGlobalEvent::m_bAutoCycle = FALSE;
BOOL CGlobalEvent::m_bStopCycle = FALSE;
BOOL CGlobalEvent::m_bReInit = FALSE;
BOOL CGlobalEvent::m_bBH_PrePick = FALSE;
BOOL CGlobalEvent::m_bEJ_NextDie = FALSE;
BOOL CGlobalEvent::m_bNoDie = FALSE;
BOOL CGlobalEvent::m_bManualHandle = FALSE;
BOOL CGlobalEvent::m_bWT_Indexed = FALSE;
BOOL CGlobalEvent::m_bWT_Indexed_BT = FALSE;
BOOL CGlobalEvent::m_bFullBlock = FALSE;
BOOL CGlobalEvent::m_bPickReady = FALSE;
BOOL CGlobalEvent::m_bDieContact = FALSE;
BOOL CGlobalEvent::m_bToBond = FALSE;
BOOL CGlobalEvent::m_bBA_DiePlaced = FALSE;
BOOL CGlobalEvent::m_bBH_DiePlaced = FALSE;
BOOL CGlobalEvent::m_bToPick = FALSE;
BOOL CGlobalEvent::m_bBA_DiePicked = FALSE;
BOOL CGlobalEvent::m_bBT_Indexed = FALSE;
BOOL CGlobalEvent::m_bBH_Blocked_FromPick = FALSE;
BOOL CGlobalEvent::m_bBH_Blocked_FromBond = FALSE;
BOOL CGlobalEvent::m_bBH_Clean_ToPick = FALSE;
BOOL CGlobalEvent::m_bBH_Clean_ToBond = FALSE;

void CGlobalEvent::ClearAll()
{
	m_bAutoCycle = FALSE;
	m_bStopCycle = FALSE;
	m_bReInit = FALSE;
	m_bBH_PrePick = FALSE;
	m_bEJ_NextDie = FALSE;
	m_bNoDie = FALSE;
	m_bManualHandle = FALSE;
	m_bWT_Indexed = FALSE;
	m_bWT_Indexed_BT = FALSE;
	m_bFullBlock = FALSE;
	m_bPickReady = FALSE;
	m_bDieContact = FALSE;
	m_bToBond = FALSE;
	m_bBA_DiePlaced = FALSE;
	m_bBH_DiePlaced = FALSE;
	m_bToPick = FALSE;
	m_bBA_DiePicked = FALSE;
	m_bBT_Indexed = FALSE;
	m_bBH_Blocked_FromPick = FALSE;
	m_bBH_Blocked_FromBond = FALSE;
	m_bBH_Clean_ToPick = FALSE;
	m_bBH_Clean_ToBond = FALSE;
}