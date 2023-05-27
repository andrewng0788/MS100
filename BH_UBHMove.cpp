#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"

// settling
INT CBondHead::T_HpDelay()
{
	INT nResult;

	//union Hp_ccb* pChannel = &m_pServo_T->m_HpCcb;
	
	union Hp_ccb* pChannel = NULL;
	try
	{
		if (GetAxisType(BH_AXIS_T) == BH_DC_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_T));
			
			if (pServo != NULL)
			{
				pChannel = &pServo->m_HpCcb;
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	nResult = hp_delay(pChannel, &m_hpBHDelayInput_T);

	return nResult;
}

// settling
INT CBondHead::Z_HpDelay()
{
	INT nResult;

	union Hp_ccb* pChannel = &m_pDcServo_Z->m_HpCcb;
	nResult = hp_delay(pChannel, &m_hpBHDelayInput_Z);

	return nResult;
}

BOOL CBondHead::Z_StepMove(INT nMode)
{
	INT nResult = gnOK;

	if (m_fHardware && m_bSel_Z)
	{
		//if (CheckZMotorType() == BH_DC_SVO_MOTOR)		//UBH
		if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
			(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
		{
			try
			{			
				union Hp_ccb* pChannel = &m_pDcServo_Z->m_HpCcb;
				
				if (hp_stepmove(pChannel, &m_hpStepMoveInput_Z) != 0)
				{
					nResult = Err_BhZMove;
				}
				
				if (nMode == SFM_WAIT)
				{
					if (hp_motion_complete(pChannel, 20000) != 0)
					{
						nResult = Err_BhZMove;
					}
				}
			}
			catch (CAsmException e)
			{
				DisplayException(e);
				//CheckHiPECResult("BondHeadZAxis");
				//ClearDcServoError("srvBondHeadZ");
				CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
				CMS896AStn::MotionClearError(BH_AXIS_Z, &m_stBHAxis_Z);
				m_bHome_Z	= FALSE;
				nResult = Err_BhZMove;
			}
		}
		else
		{
			return gnOK;
		}
	}
	else
	{
		if (m_bSel_Z)
		{
			if (nMode == SFM_WAIT)
			{
				Sleep(40);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	CheckResult(nResult, _T("BondHead Axis - StepMove"));

	if ( nResult != gnOK )
	{
		SetMotionCE(TRUE, "Z_StepMove To fail");
	}

	return nResult;
}

LONG CBondHead::Z_RunPresetBJLPick()
{
	BOOL bReturn = FALSE;
	short sReturn;

	if (m_fHardware && m_bSel_Z)
	{
		/*
		if ( CMS896AApp::m_bEnableMachineLog == TRUE )
		{
			SetupDataLogZ();
			EnableDataLogZ(TRUE);
		}
		*/

		sReturn = hp_run_presetbjl(&m_pDcServo_Z->m_HpCcb, BP_Z_UBH_BJL_NO);
		
		if (sReturn != 0)
		{
			return FALSE;
		}
		
		/*
		if ( CMS896AApp::m_bEnableMachineLog == TRUE )
			EnableDataLogZ(FALSE);
		*/

		//bReturn = CheckHiPECResult("BondHeadZAxis");
		bReturn = CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
		return bReturn;
	}
	else
	{
		if (m_bSel_Z)
		{
			Sleep(40);
		}
	}

	return TRUE;
}

LONG CBondHead::Z_RunPresetBJLBond()
{
		if (m_fHardware && m_bSel_Z)
	{
		BOOL bReturn = FALSE;
		short sReturn;
		
		/*
		if ( CMS896AApp::m_bEnableMachineLog == TRUE )
		{
		
			SetupDataLogZ();
			EnableDataLogZ(TRUE);
		}
		*/

		sReturn = hp_run_presetbjl(&m_pDcServo_Z->m_HpCcb, BP_Z_UBH_BJL_NO + 1);
		
		if (sReturn != 0)
		{
			return FALSE;
		}
		
		/*
		if ( CMS896AApp::m_bEnableMachineLog == TRUE )
			EnableDataLogZ(FALSE);
		*/

		if (Z_BondDelay() == FALSE)
		{
			return FALSE;
		}

		//bReturn = CheckHiPECResult("BondHeadZAxis");
		bReturn = CMS896AStn::MotionCheckResult(BH_AXIS_Z, &m_stBHAxis_Z);
		return bReturn;
	}
	else
	{
		if (m_bSel_Z)
		{
			Sleep(40);
		}
	}

	return TRUE;
}