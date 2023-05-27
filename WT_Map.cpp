/////////////////////////////////////////////////////////////////
// WT_Map.cpp : Wafer Map function of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
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
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////
//	Map commands for Wafer Map Control
/////////////////////////////////////////////////////////////////

BOOL CWaferTable::PeekMapNextDie(unsigned long ulSkipNumberOfDice,
							  unsigned long &ulRow, unsigned long &ulCol, unsigned char &ucGrade, 
							  WAF_CDieSelectionAlgorithm::WAF_EDieAction& eAction,
							  BOOL &bEndOfWafer, CString szMsg)
{
	CString csMsg;
	BOOL bReturn;

	if ( m_bLoadMap == FALSE )
	{
//		m_WaferMapWrapper.GetStartPosition(ulRow, ulCol);
		m_WaferMapWrapper.StartMap();
		m_bLoadMap = TRUE;
		CMSLogFileUtility::Instance()->WT_GetDieLog("Start map when peek next die first time");
	}
	
	if ( m_bResetStartPos == TRUE )
	{
		//Init current die & last die structure
		ULONG ulCurrRow = 0, ulCurrCol = 0;
		
		GetEncoderValue();
		m_WaferMapWrapper.GetCurrentPosition(ulCurrRow, ulCurrCol);

		m_stCurrentDie.lY		= ulCurrRow;	
		m_stCurrentDie.lX		= ulCurrCol;
		m_stCurrentDie.ulStatus	= WT_MAP_DIESTATE_DEFAULT; 
		m_stCurrentDie.ucGrade	= m_WaferMapWrapper.GetGrade(ulCurrRow, ulCurrCol);
		// init the grade
		m_stCurrentDie.eAction	= WAF_CDieSelectionAlgorithm::ALIGN;
		m_stCurrentDie.lX_Enc	= GetCurrX();
		m_stCurrentDie.lY_Enc	= GetCurrY();
		m_stCurrentDie.lT_Enc	= GetCurrT();

		m_stLastDie = m_stCurrentDie;
		m_bResetStartPos = FALSE;
		csMsg.Format("first after align to set last is current at %lu,%lu", ulCurrRow, ulCurrCol);
		CMSLogFileUtility::Instance()->WT_GetDieLog(csMsg);
	}

	bReturn = PeekMapDie(ulSkipNumberOfDice, ulRow, ulCol, ucGrade, 
								eAction, bEndOfWafer);

	//GetDie Log
	CString szLogText;
	LONG lUserRow = 0, lUserCol = 0;
	ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
	ULONG ulState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
	szLogText.Format("PND %d,%d(%d,%d),%d(%d), act %d, stt %d; step %d, end %d", 
		ulRow, ulCol, lUserRow, lUserCol, ucGrade, m_WaferMapWrapper.GetGradeOffset(), 
		eAction, ulState, ulSkipNumberOfDice, bEndOfWafer);
	if( szMsg.IsEmpty()==FALSE )
		szLogText = szLogText + ", " + szMsg;
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);
	if( bEndOfWafer )
	{
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
		szLogText = "PND end of wafer";
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
	}

	return bReturn;
}


BOOL CWaferTable::GetMapNextDie(unsigned long &ulRow, unsigned long &ulCol, unsigned char &ucGrade,
						WAF_CDieSelectionAlgorithm::WAF_EDieAction& eAction, BOOL &bEndOfWafer)
{
	if( m_lGetNextDieRow != -1 && m_lGetNextDieCol != -1 )
	{
		CMSLogFileUtility::Instance()->WT_GetDieLog("NEXT ERROR, previous die not updated");
	}

	BOOL bReturn = GetMapDie(ulRow, ulCol, ucGrade, eAction, bEndOfWafer);

	m_lGetNextDieRow = ulRow;
	m_lGetNextDieCol = ulCol;

	//GetDie Log
	CString szLogText;
	ULONG ulState = m_WaferMapWrapper.GetDieState(ulRow, ulCol);
	szLogText.Format("GND %d,%d,%d, act %d, end %d stt %d",ulRow, ulCol, ucGrade, eAction, bEndOfWafer, ulState);
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);

	if( bEndOfWafer || bReturn==FALSE )
	{
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
		if( bReturn==FALSE )
			szLogText = "GND next die return false";
		else
			szLogText = "GND end of wafer";
		CMSLogFileUtility::Instance()->WT_GetDieLog(szLogText);
		CMSLogFileUtility::Instance()->WT_GetIdxLog(szLogText);
	}

	return bReturn;
}

BOOL CWaferTable::SetMapNextDie(const ULONG ulRow, const ULONG ulCol, 
							const UCHAR ucGrade, const WAF_CDieSelectionAlgorithm::WAF_EDieAction eAction, 
							const ULONG ulDieState)

{
	WAF_CDieSelectionAlgorithm::WAF_EDieAction eUpAct;
	eUpAct = eAction;
	CString szState, szLogMsg;
	switch (ulDieState)
	{
	case WT_MAP_DIESTATE_DEFAULT:
		szState = " - DEFAULT";
		break;
	case WT_MAP_DIESTATE_PICK:
		szState = " - PICK";
		break;
	case WT_MAP_DIESTATE_DEFECT:
		szState = " - DEFECT";
		break;
	case WT_MAP_DIESTATE_INK:
		szState = " - INK";
		break;
	case WT_MAP_DIESTATE_CHIP:
		szState = " - CHIP";
		break;
	case WT_MAP_DIESTATE_BADCUT:
		szState = " - BADCUT";
		break;
	case WT_MAP_DIESTATE_EMPTY:
		szState = " - EMPTY";
		break;
	case WT_MAP_DIESTATE_ROTATE:
		szState = " - ROTATE";
		break;
	case WT_MAP_DIESTATE_UNPICK:
		szState = " - UNPICK";
		break;
	default:
		szState.Format(" - %d", ulDieState);
		break;
	}

	// prevent first die is reference die and used to align using normal die record and then updated to missing
	if (m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol))
	{
		ULONG ulStartRow, ulStartCol;
		m_WaferMapWrapper.GetStartPosition(ulStartRow, ulStartCol);

		if ((ulRow == ulStartRow) && (ulCol == ulStartCol) && (eUpAct == WAF_CDieSelectionAlgorithm::MISSING))
		{
			eUpAct = WAF_CDieSelectionAlgorithm::ALIGN;
		}
	}
	else if (IsBlkFunc2Enable() && ((eUpAct == WAF_CDieSelectionAlgorithm::MISSING) || ulDieState == WT_MAP_DIESTATE_EMPTY))
	{
		if (m_WaferMapWrapper.GetReader() != NULL)
		{
			UCHAR ucInvalidGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetInvalidBin();
			if (ucGrade == ucInvalidGrade)
			{
				eUpAct = WAF_CDieSelectionAlgorithm::INVALID;
			}
		}
	}
	
	szLogMsg.Format("MUD2 %d,%d,%d, act %d(%d), stt %s", ulRow, ulCol, ucGrade, eUpAct, eAction, szState);
	CMSLogFileUtility::Instance()->WT_GetDieLog(szLogMsg);
	m_lGetNextDieRow = -1;
	m_lGetNextDieCol = -1;

	return UpdateMapDie(ulRow, ulCol, ucGrade, eUpAct, ulDieState);
}

INT CWaferTable::UpdMapLastDie(CString szWhere)
{
	CMSLogFileUtility::Instance()->WT_GetDieLog("UPD last loop " + szWhere);
	// Update the last die pos wafer map
	return UpdateMap(GetLastDieRow(), GetLastDieCol(), m_stLastDie.ucGrade, GetLastDieEAct(), m_stLastDie.ulStatus);
}
