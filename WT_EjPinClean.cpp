/////////////////////////////////////////////////////////////////
// WT_Common.cpp : Common functions of the CWaferTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, March 07, 2019
//	Revision:	1.00
//
//	By:			Tang Liang Hong
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "MS896A.h"
#include "BondHead.h"
#include "EjPinCleanRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CWaferTable::RegisterVariables_EjPinClean()
{
	RegVariable(_T("WT_EjPin_lCleanDirtMatrixRow"),			&m_pEjPinCleanRegion->m_lCleanDirtMatrixRow);
	RegVariable(_T("WT_EjPin_lCleanDirtMatrixCol"),			&m_pEjPinCleanRegion->m_lCleanDirtMatrixCol);
	RegVariable(_T("WT_EjPin_lCleanDirtUpLeftPosX"),		&m_pEjPinCleanRegion->m_lCleanDirtUpleftPosX);
	RegVariable(_T("WT_EjPin_lCleanDirtUpLeftPosY"),		&m_pEjPinCleanRegion->m_lCleanDirtUpleftPosY);
	RegVariable(_T("WT_EjPin_lCleanDirtLowerRightPosX"),	&m_pEjPinCleanRegion->m_lCleanDirtLowerRightPosX);
	RegVariable(_T("WT_EjPin_lCleanDirtLowerRightPosY"),	&m_pEjPinCleanRegion->m_lCleanDirtLowerRightPosY);

	RegVariable(_T("WT_EjPin_lCleanDirtPinUpLevel"),		&m_pEjPinCleanRegion->m_lCleanDirtPinUpLevel);
	RegVariable(_T("WT_EjPin_lCleanDirtCycleAreaCount"),	&m_pEjPinCleanRegion->m_lCleanDirtCycleAreaCount);
	RegVariable(_T("WT_EjPin_lCleanDirtCycleEjUpCount"),	&m_pEjPinCleanRegion->m_lCleanDirtCycleEjUpCount);
	RegVariable(_T("WT_EjPin_lCleanDirtSamplingCount"),		&m_pEjPinCleanRegion->m_lCleanDirtSamplingCount);
	RegVariable(_T("WT_EjPin_bCleanDirtEJPinEnable"),		&m_pEjPinCleanRegion->m_lCleanDirtEJPinEnable);

	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ChangePosition"),			&CWaferTable::ChangePosition);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ConfirmSetup"),				&CWaferTable::ConfirmSetup);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CancelSetup"),				&CWaferTable::CancelSetup);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetCleanDirtEjectorPinCount"),	&CWaferTable::ResetCleanDirtEjectorPinCount);
	m_comServer.IPC_REG_SERVICE_COMMAND(_T("CleanDirtEjectorPinTest"),			&CWaferTable::CleanDirtEjectorPinTest);
}



LONG CWaferTable::ChangePosition(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	LONG lOption = 0;
	LONG lMoveX=0, lMoveY=0;

	svMsg.GetMsg(sizeof(LONG), &lOption);
	switch(lOption)
	{
	default:
	case 0:		//Clean Dirt Area for Ejector Pin Clean	
		lMoveX = m_pEjPinCleanRegion->GetCleanDirtUpleftPosX();
		lMoveY = m_pEjPinCleanRegion->GetCleanDirtUpleftPosY();
		break;
	case 1:		//Clean Dirt Area for Ejector Pin Clean	
		lMoveX = m_pEjPinCleanRegion->GetCleanDirtLowerRightPosX();
		lMoveY = m_pEjPinCleanRegion->GetCleanDirtLowerRightPosY();
		break;
	}

	SetJoystickOn(FALSE);
	XY_MoveTo(lMoveX, lMoveY);
	SetJoystickOn(TRUE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferTable::ConfirmSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	CString szMsg;
	LONG lOption = 0;
	svMsg.GetMsg(sizeof(LONG), &lOption);
	
	GetEncoderValue();
	if (lOption == 0)	//Clean Dirt Area for Ejector Pin Clean	
	{
		m_pEjPinCleanRegion->SetCleanDirtUpleftPos(m_lEnc_X, m_lEnc_Y);
		
		szMsg.Format("Clean Dirt Upper-Left position is updated to (%ld, %ld)", 
			m_pEjPinCleanRegion->GetCleanDirtUpleftPosX(), m_pEjPinCleanRegion->GetCleanDirtUpleftPosY());
		HmiMessage(szMsg);
	}
	else if (lOption == 1)	//Clean Dirt Area for Ejector Pin Clean	
	{
		m_pEjPinCleanRegion->SetCleanDirtLowerRightPos(m_lEnc_X, m_lEnc_Y);
		
		szMsg.Format("Clean Dirt Upper-Left position is updated to (%ld, %ld)", 
			m_pEjPinCleanRegion->GetCleanDirtLowerRightPosX(), m_pEjPinCleanRegion->GetCleanDirtLowerRightPosY());
		HmiMessage(szMsg);
	}

	SaveData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::CancelSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


LONG CWaferTable::CleanDirtEjectorPinTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	SelectWaferCamera();
	if (!CleanDirtEjectorPin())
	{
		CString szError;
		szError.Format("Clean Ejctor Pin failure");
		HmiMessage_Red_Back(szError, "Clean Ejector Pin");
	}
	else
	{
		HmiMessage("Clean Dir Eector Pin Finished");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::ResetCleanDirtEjectorPinCount(IPC_CServiceMessage& svMsg)
{
	m_pEjPinCleanRegion->ResetCleanDirtEJPinCount();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}


BOOL CWaferTable::MoveToEjPinCleanDirt()
{
	LONG lX = 0, lY = 0;

	m_pEjPinCleanRegion->GetCleanDirtXYPosn(lX, lY);
	//Move table to updated position
	if (IsWithinWaferLimit(lX, lY) == FALSE)	// OUT OF WAFER LIMIT
	{
		return FALSE;
	}

	X_Sync();
	Y_Sync();
	if (!XY_SafeMoveTo(lX, lY))
	{
		return FALSE;
	}
	X_Sync();
	Y_Sync();
	Sleep(20);

	return TRUE;
}


BOOL CWaferTable::MoveEjectorTableToHomePosn()
{
	LONG lCDiePos_EjX = (*m_psmfSRam)["BondHead"]["CDiePos_EjX"];
	LONG lCDiePos_EjY = (*m_psmfSRam)["BondHead"]["CDiePos_EjY"];
	LONG lEjtX = lCDiePos_EjX;
	LONG lEjtY = lCDiePos_EjY;

	EjX_MoveTo(lEjtX, SFM_NOWAIT);
	EjY_MoveTo(lEjtY, SFM_WAIT);
	EjX_Sync();
		
	if (!EjX_IsPowerOn() || !EjY_IsPowerOn())
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CWaferTable::CleanDirtEjectorPin()
{
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead != NULL)
	{

		SetEjectorVacuum(FALSE);
		Sleep(100);
		pBondHead->EjectorMoveToStandBy();
		if (!MoveEjectorTableToHomePosn())
		{
			return FALSE;
		}

		for (LONG i = 0; i < m_pEjPinCleanRegion->GetCleanDirtCycleAreaCount(); i++)
		{
			if (!MoveToEjPinCleanDirt())
			{
				return FALSE;
			}

			SetEjectorVacuum(TRUE);
			Sleep(100);
			if (!pBondHead->CleanDirtEjectorPin(m_pEjPinCleanRegion->GetCleanDirtPinUpLevel(), m_pEjPinCleanRegion->GetCleanDirtCycleEjPinUpCount()))
			{
				return FALSE;
			}
			SetEjectorVacuum(FALSE);
			Sleep(100);
			m_pEjPinCleanRegion->MoveToNextCleanDirtArea();	
		}
		m_pEjPinCleanRegion->ResetCleanDirtEJPinCount();
	}
	return TRUE;
}