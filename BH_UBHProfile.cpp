#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"

// ============Bond Head T===================

LONG CBondHead::SetupBHTSettlingBlk()
{
	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	CObject* pObject;
	SFM_CHipecDelay delayBHT;
	Hp_Delay_profile BHTModuleDelayBlock;
	
	SFM_CHipecAcServo* pServo = NULL;
	try
	{
		if (GetAxisType(BH_AXIS_T) == BH_DC_SVO_MOTOR)	
		{
			pServo = GetHipecAcServo(GetActuatorName(BH_AXIS_T));
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	if (pServo)
	{
		try
		{
			//m_pServo_T->GetModule()->AddDelayProfileDataBlock(delayBHT, "delayBHT", FALSE);
			pServo->GetModule()->AddDelayProfileDataBlock(delayBHT, "delayBHT", FALSE);

			//byte byDelayBHT = m_pServo_T->GetModule()->GetDelayProfileDataBlockId("delayBHT", FALSE);
			byte byDelayBHT = pServo->GetModule()->GetDelayProfileDataBlockId("delayBHT", FALSE);
			
			BHTModuleDelayBlock.blkno = byDelayBHT;
			BHTModuleDelayBlock.delay_time = (short) m_lUBHSettlingTime_T * 8;

			//hp_set_delay_block(&m_pServo_T->GetModule()->m_HpMcb, &BHTModuleDelayBlock);
			hp_set_delay_block(&pServo->GetModule()->m_HpMcb, &BHTModuleDelayBlock);

			// download it to hipec and get the block id
			if (m_pInitOperation->GetControlParameterMap()->Lookup("ctrBondHeadTObwSettling", pObject))
			{
				SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
				//m_pServo_T->SetControlParam("ctrBondHeadTObwSettling", *pCtr);
				//m_pServo_T->SelectControlParam("ctrBondHeadTObwSettling");
				//byte bSettlingBlockId = m_pServo_T->GetModule()->GetControlParamDataBlockId("ctrBondHeadTObwSettling", FALSE);
				pServo->SetControlParam("ctrBondHeadTObwSettling", *pCtr);
				pServo->SelectControlParam("ctrBondHeadTObwSettling");
				byte bSettlingBlockId = pServo->GetModule()->GetControlParamDataBlockId("ctrBondHeadTObwSettling", FALSE);
				
				m_hpBHDelayInput_T.ctrl_type		= HP_CTRL_GFL_PD_INTG;
				m_hpBHDelayInput_T.ctrl_blkno		= bSettlingBlockId;
				m_hpBHDelayInput_T.process_blkno	= HP_NIL_PROCESS;
				m_hpBHDelayInput_T.prof_blkno		= byDelayBHT;

				// restore back to dynamic control
				if (m_pInitOperation->GetControlParameterMap()->Lookup("ctrBondHeadTObwDyn", pObject))
				{
					SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
					//m_pServo_T->SetControlParam("ctrBondHeadTObwDyn", *pCtr);
					//m_pServo_T->SelectControlParam("ctrBondHeadTObwDyn");
					pServo->SetControlParam("ctrBondHeadTObwDyn", *pCtr);
					pServo->SelectControlParam("ctrBondHeadTObwDyn");
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
		}
	}

	return TRUE;
}

// ============Bond Head Z===================

LONG CBondHead::SetupBHZSettlingBlk()
{
	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	CObject* pObject;
	SFM_CHipecDelay delayBHZ;
	Hp_Delay_profile BHZModuleDelayBlock;

	try
	{
		m_pDcServo_Z->GetModule()->AddDelayProfileDataBlock(delayBHZ, "delayBHZ", FALSE);

		byte byDelayBHZ = m_pDcServo_Z->GetModule()->GetDelayProfileDataBlockId("delayBHZ", FALSE);
		
		BHZModuleDelayBlock.blkno = byDelayBHZ;
		BHZModuleDelayBlock.delay_time = 32;

		hp_set_delay_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &BHZModuleDelayBlock);

		// download it to hipec and get the block id
		if (m_pInitOperation->GetControlParameterMap()->Lookup("ctrBondHeadZObwSettling", pObject))
		{
			SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
			m_pDcServo_Z->SetControlParam("ctrBondHeadZObwSettling", *pCtr);
			m_pDcServo_Z->SelectControlParam("ctrBondHeadZObwSettling");
			//byte bSettlingBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSettling", FALSE);

			byte byStaticControlBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSts", FALSE);
			
			m_hpBHDelayInput_Z.ctrl_type		= HP_CTRL_GFL_PD_INTG;
			//m_hpBHDelayInput_Z.ctrl_blkno		= bSettlingBlockId;
			m_hpBHDelayInput_Z.ctrl_blkno		= byStaticControlBlockId;
			m_hpBHDelayInput_Z.process_blkno	= HP_NIL_PROCESS;
			m_hpBHDelayInput_Z.prof_blkno		= byDelayBHZ;

			// restore back to dynamic control
			if (m_pInitOperation->GetControlParameterMap()->Lookup("ctrBondHeadZObwDyn", pObject))
			{
				SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
				m_pDcServo_Z->SetControlParam("ctrBondHeadZObwDyn", *pCtr);
				m_pDcServo_Z->SelectControlParam("ctrBondHeadZObwDyn");
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

LONG CBondHead::Z_SetupStepMoveBlk()
{
	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	SFM_CStepProfile stepMoveZ;
	Hp_StepMove_profile hpstepMoveProfileZ;

	try
	{	
		//if (CheckZMotorType() == BH_DC_SVO_MOTOR)
		if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
			(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
		{
			m_pDcServo_Z->GetModule()->AddStepProfileDataBlock(stepMoveZ, "stepMoveZ", FALSE);
			byte byStepMoveBlk = m_pDcServo_Z->GetModule()->GetStepProfileDataBlockId("stepMoveZ", FALSE);
			
			hpstepMoveProfileZ.blkno = byStepMoveBlk;
			hpstepMoveProfileZ.step_time = 12;
			hpstepMoveProfileZ.mode = HP_ABSOLUTE_STEP_MOVE;
			//hpstepMoveProfileZ.mode = HP_RELATIVE_STEP_MOVE;
			hpstepMoveProfileZ.stepmove_to_pos = 0;
			//hpstepMoveProfileZ.stepmove_amplitude = 300;
			CString szTemp;
			szTemp.Format("Step Pos:%d", hpstepMoveProfileZ.stepmove_to_pos);
			DisplayMessage(szTemp);
			
			hp_set_stepmove_block_ext(&m_pDcServo_Z->GetModule()->m_HpMcb, &hpstepMoveProfileZ);
			byte byctrBondHeadZObwStsID = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSts", FALSE);
			m_hpStepMoveInput_Z.ctrl_blkno = byctrBondHeadZObwStsID;
			m_hpStepMoveInput_Z.ctrl_type = HP_CTRL_GFL_PD_INTG;
			m_hpStepMoveInput_Z.process_blkno = HP_NIL_PROCESS;
			m_hpStepMoveInput_Z.prof_blkno = byStepMoveBlk;
			
		}
		else
		{
			return TRUE;
		}

	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

LONG CBondHead::Z_UpdateStepMoveProf()
{
	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	SFM_CStepProfile stepMoveZ;
	Hp_StepMove_profile hpstepMoveProfileZ;

	try
	{	
		//if (CheckZMotorType() == BH_DC_SVO_MOTOR)
		if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
			(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
		{
			//m_pDcServo_Z->GetModule()->AddStepProfileDataBlock(stepMoveZ, "stepMoveZ", FALSE);
			byte byStepMoveBlk = m_pDcServo_Z->GetModule()->GetStepProfileDataBlockId("stepMoveZ", FALSE);
			
			hpstepMoveProfileZ.blkno = byStepMoveBlk;
			hpstepMoveProfileZ.step_time = 12;
			hpstepMoveProfileZ.mode = HP_ABSOLUTE_STEP_MOVE;
			//hpstepMoveProfileZ.mode = HP_RELATIVE_STEP_MOVE;
			hpstepMoveProfileZ.stepmove_to_pos = m_pDcServo_Z->GetEncoderPosition() + m_lUBHStepOffset_Z;
			//hpstepMoveProfileZ.stepmove_amplitude = 300;
			//CString szTemp;
			//szTemp.Format("Profile:%d Step Pos:%d", byStepMoveBlk,m_pDcServo_Z->GetEncoderPosition());
			//DisplayMessage(szTemp);

		//	Sleep(100);

			//szTemp.Format("Profile2:%d Step Pos:%d", byStepMoveBlk,hpstepMoveProfileZ.stepmove_to_pos);
			//DisplayMessage(szTemp);
			
			hp_set_stepmove_block_ext(&m_pDcServo_Z->GetModule()->m_HpMcb, &hpstepMoveProfileZ);
			/*
			byte byctrBondHeadZObwStsID = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSts", FALSE);
			m_hpStepMoveInput_Z.ctrl_blkno = byctrBondHeadZObwStsID;
			m_hpStepMoveInput_Z.ctrl_type = HP_CTRL_GFL_PD_INTG;
			m_hpStepMoveInput_Z.process_blkno = HP_NIL_PROCESS;
			m_hpStepMoveInput_Z.prof_blkno = byStepMoveBlk;
			*/
			
		}
		else
		{
			return TRUE;
		}

	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}


LONG CBondHead::SetupAutoLearnBondLevelBlks(Hp_Search_input& hp_LrnBondLevelInput_Z)
{
	/*
	Hp_Search_profile	hp_LrnBondLevelBlock_Z; 
	
	//SFM_CProfile		
	SFM_CSearchProfile	spfBondHeadZSearchBondLevel;
	
	INT max_trigger_perr_count = 5;
	INT min_trigger_perr_count = 0;
	INT contact_search_speed   = 1;
	INT triger_debounce_number = 0;
	INT nSearchDriveInSample = 0;
	INT nSearchDriveInSpeed = 0;
	
	try
	{
		m_pDcServo_Z->GetModule()->AddSearchProfileDataBlock(spfBondHeadZSearchBondLevel, "spfBondHeadZSearchBondLevel", FALSE);
		byte bySearchBondLevelBHZProBlkID = m_pDcServo_Z->GetModule()->GetSearchProfileDataBlockId("spfBondHeadZSearchBondLevel", FALSE);

		hp_LrnBondLevelBlock_Z.blkno = bySearchBondLevelBHZProBlkID;
		hp_LrnBondLevelBlock_Z.search_type = HP_SRH_RANGE;
		hp_LrnBondLevelBlock_Z.SourcePort = HP_SW32_PORT_11;
		hp_LrnBondLevelBlock_Z.SearchPort = HP_SW32_PORT_12;
		//hp_LrnBondLevelBlock_Z.UpperRange = 256*256*max_trigger_perr_count;
		//hp_LrnBondLevelBlock_Z.LowerRange = 256*256*min_trigger_perr_count;
		hp_LrnBondLevelBlock_Z.UpperRange = 256*256*max_trigger_perr_count;
		hp_LrnBondLevelBlock_Z.LowerRange = 256*256*min_trigger_perr_count;

		hp_LrnBondLevelBlock_Z.search_speed = 256*contact_search_speed;
		hp_LrnBondLevelBlock_Z.limit = 0;
		hp_LrnBondLevelBlock_Z.debounce = triger_debounce_number;

		//hp_LrnBondLevelBlock_Z.drivein_count = 0;
		//hp_LrnBondLevelBlock_Z.drivein_speed = 0;

		hp_LrnBondLevelBlock_Z.drivein_count = nSearchDriveInSample;
		hp_LrnBondLevelBlock_Z.drivein_speed = 256*nSearchDriveInSpeed;
		hp_LrnBondLevelBlock_Z.enable_error_chk = HP_ENABLE;
		hp_set_search_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_LrnBondLevelBlock_Z);
		
		byte bySearchBondLevelBHZCtrlID = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSts", FALSE);
		
		hp_LrnBondLevelInput_Z.ctrl_type = HP_CTRL_GFL_PD_INTG;
		hp_LrnBondLevelInput_Z.ctrl_blkno = bySearchBondLevelBHZCtrlID;
		hp_LrnBondLevelInput_Z.process_blkno = HP_NIL_PROCESS;
		hp_LrnBondLevelInput_Z.prof_blkno = bySearchBondLevelBHZProBlkID;
		hp_LrnBondLevelInput_Z.search_dir = HP_NEGATIVE_DIR;
		hp_LrnBondLevelInput_Z.setpos_flag = HP_DISABLE;
		hp_LrnBondLevelInput_Z.setpos_to = 0;

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	*/
	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	Hp_Search_profile	hp_LrnBondLevelBlock_Z; 
	
	//SFM_CProfile		
	SFM_CSearchProfile	spfBondHeadZSearchBondLevel;
	
	INT nMaxTriggerPerrCount = 5;
	INT nMinTriggerPerrCount = 0;
	INT nContactSearchSpeed   = 3;
	INT nTriggerDebounceNum = 1;
	INT nSearchDriveInSample = 0;
	INT nSearchDriveInSpeed = 0;
	
	try
	{
		m_pDcServo_Z->GetModule()->AddSearchProfileDataBlock(spfBondHeadZSearchBondLevel, "spfBondHeadZSearchBondLevel", FALSE);
		byte bySearchBondLevelBHZProBlkID = m_pDcServo_Z->GetModule()->GetSearchProfileDataBlockId("spfBondHeadZSearchBondLevel", FALSE);

		hp_LrnBondLevelBlock_Z.blkno = bySearchBondLevelBHZProBlkID;
		hp_LrnBondLevelBlock_Z.search_type = HP_SRH_RANGE;
		hp_LrnBondLevelBlock_Z.SourcePort = HP_SW32_PORT_11;
		hp_LrnBondLevelBlock_Z.SearchPort = HP_SW32_PORT_12;
		//hp_LrnBondLevelBlock_Z.UpperRange = 256*256*max_trigger_perr_count;
		//hp_LrnBondLevelBlock_Z.LowerRange = 256*256*min_trigger_perr_count;
		hp_LrnBondLevelBlock_Z.UpperRange = 256*256*nMaxTriggerPerrCount;
		hp_LrnBondLevelBlock_Z.LowerRange = 256*256*nMinTriggerPerrCount;

		hp_LrnBondLevelBlock_Z.search_speed = 256*nContactSearchSpeed;
		hp_LrnBondLevelBlock_Z.limit = 0;
		hp_LrnBondLevelBlock_Z.debounce = nTriggerDebounceNum;

		//hp_LrnBondLevelBlock_Z.drivein_count = 0;
		//hp_LrnBondLevelBlock_Z.drivein_speed = 0;

		hp_LrnBondLevelBlock_Z.drivein_count = nSearchDriveInSample;
		hp_LrnBondLevelBlock_Z.drivein_speed = 256*nSearchDriveInSpeed;
		hp_LrnBondLevelBlock_Z.enable_error_chk = HP_ENABLE;
		hp_set_search_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_LrnBondLevelBlock_Z);
		
		byte bySearchBondLevelBHZCtrlID = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwDyn", FALSE);
		
		hp_LrnBondLevelInput_Z.ctrl_type = HP_CTRL_GFL_PD_INTG;
		hp_LrnBondLevelInput_Z.ctrl_blkno = bySearchBondLevelBHZCtrlID;
		hp_LrnBondLevelInput_Z.process_blkno = HP_NIL_PROCESS;
		hp_LrnBondLevelInput_Z.prof_blkno = bySearchBondLevelBHZProBlkID;
		hp_LrnBondLevelInput_Z.search_dir = HP_NEGATIVE_DIR;
		hp_LrnBondLevelInput_Z.setpos_flag = HP_DISABLE;
		hp_LrnBondLevelInput_Z.setpos_to = 0;

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	return TRUE;

}