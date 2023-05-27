#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"

LONG CBondHead::Z_PresetBJL()
{
	//AfxMessageBox("Z_PresetBJL", MB_SYSTEMMODAL);

	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	short sPresetBJLNo = BP_Z_UBH_BJL_NO;
	short sReturn = 0;

	sReturn = hp_free_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb, sPresetBJLNo);
	
	if (sReturn != 0)
	{
		CString szMsg;
		szMsg.Format("Free Preset BJL ERR:%d", sReturn);
		DisplayMessage(szMsg);
		return FALSE;
	}
	
	try
	{
		// Phase 1, define the obw part
		SFM_CObwProfile* pObwProfile;
		SFM_CHipecMotionProcess ObwMoveProcessBlk;
		Hp_process	hp_ObwMoveProcessBlk;
		SFM_CProfile mpfBondHeadZObwBJL;
		Hp_OBwMove_profile hp_ObwMoveProfile;
		//Hp_obw_profile_shift_factor hp_ObwMoveShiftFactor;
		Hp_OBwMove_input hp_ObwMoveInput;
		

		m_pDcServo_Z->GetModule()->AddMotionProcessDataBlock(ObwMoveProcessBlk,"ObwMoveProcessBlk", FALSE);
		byte byObwMoveProcessBlkID = m_pDcServo_Z->GetModule()->GetMotionProcessDataBlockId("ObwMoveProcessBlk", FALSE);

		CString szTemp;
		szTemp.Format("Obw Move Process:%d", byObwMoveProcessBlkID);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		
		hp_ObwMoveProcessBlk.blkno = byObwMoveProcessBlkID;
		hp_ObwMoveProcessBlk.mode = HP_PROCESS_TIME;
		hp_ObwMoveProcessBlk.type = HP_PROCESS_CAPTURE_DATA;
		hp_ObwMoveProcessBlk.time = -40;
		hp_ObwMoveProcessBlk.source = HP_MIN_CMD_POS_PORT + m_pDcServo_Z->GetChannelId()-1;
		hp_ObwMoveProcessBlk.destination = HP_SW32_PORT_14;
		hp_ObwMoveProcessBlk.next_blkno = HP_END_OF_PROCESS;
		
		sReturn = hp_set_process_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_ObwMoveProcessBlk);

		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Move Process Block Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
			
		m_pDcServo_Z->GetModule()->AddProfileDataBlock(mpfBondHeadZObwBJL,"mpfBondHeadZObwBJL", FALSE);
		byte byObwProfileBlockId = m_pDcServo_Z->GetModule()->GetProfileDataBlockId("mpfBondHeadZObwBJL", FALSE);
	
		if ((pObwProfile = GetObwProfile("mpfBondHeadZObwDefault")) == NULL)
		{
			CString szMsg;
			szMsg.Format("Hp Get Obw Profile Move Block Failed");
			DisplayMessage(szMsg);
			return FALSE;
		}
	
		hp_ObwMoveProfile.blkno = byObwProfileBlockId;
		hp_ObwMoveProfile.amax = (short) pObwProfile->GetMaxAcceleration();
		hp_ObwMoveProfile.dmax = (short) pObwProfile->GetMaxDeceleration();
		hp_ObwMoveProfile.vmax = (short) pObwProfile->GetMaxVelocity();
		hp_ObwMoveProfile.final_vel = (short) pObwProfile->GetFinalVelocity();
		
		sReturn = hp_set_obw_profile_move_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_ObwMoveProfile);
		
		if (sReturn != 0 )
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Profile Move Block Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}


		// May need to added select that ctrl
		byte byDynControlBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwDyn", FALSE);

		szTemp.Format("Obw Dyn Control Blk:%d", byDynControlBlockId);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		//CString szTemp;
		szTemp.Format("Bond Level:%d", m_lBondLevel_Z);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		hp_ObwMoveInput.ctrl_blkno = byDynControlBlockId ;
		hp_ObwMoveInput.ctrl_type = HP_CTRL_GFL_FFC_PD_INTG;
		//hp_ObwMoveInput.distance =  m_lBondLevel_Z - m_lSwingLevel_Z + 100;
		hp_ObwMoveInput.distance = m_lPickLevel_Z - m_lSwingLevel_Z;
		hp_ObwMoveInput.process_blkno = byObwMoveProcessBlkID;
		hp_ObwMoveInput.prof_blkno = byObwProfileBlockId ;

		sReturn = hp_setup_obw_mv_with_final_vel_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb ,sPresetBJLNo, &hp_ObwMoveInput);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Move With Final Vel Preset BJL failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}

		
		// Phase 2, define search part
		/*
		SFM_CHipecMotionProcess LearnDACProcessBlk;	
		Hp_process hp_LearnDACProcessBlk;
		m_pDcServo_Z->GetModule()->AddMotionProcessDataBlock(LearnDACProcessBlk,"LearnDACProcess", FALSE);
		byte byLearnDACProcessBlkID = m_pDcServo_Z->GetModule()->GetMotionProcessDataBlockId("LearnDACProcess", FALSE);
		hp_LearnDACProcessBlk.blkno = byLearnDACProcessBlkID;
		hp_LearnDACProcessBlk.mode = HP_PROCESS_TIME;
		hp_LearnDACProcessBlk.type = HP_PROCESS_CAPTURE_DATA;
		hp_LearnDACProcessBlk.time = -1;
		hp_LearnDACProcessBlk.source = HP_CH_MTRDAC_PORT_0 + m_pDcServo_Z->GetChannelId()-1;
		hp_LearnDACProcessBlk.destination = HP_SW32_PORT_1;//HP_SW32_PORT_14;
		hp_LearnDACProcessBlk.next_blkno = HP_END_OF_PROCESS;
		
		sReturn = hp_set_process_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_LearnDACProcessBlk);
		
		if (sReturn != 0 )
		{
			CString szMsg;
			szMsg.Format("hp_set_process_block: %d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
		

		SFM_CHipecMotionProcess OpenDACProcessBlk;
		Hp_process	hp_OpenDACProcessBlk;
		m_pDcServo_Z->GetModule()->AddMotionProcessDataBlock(OpenDACProcessBlk,"OpenDACProcessBlk", FALSE);
		byte byOpenDACProcessBlkID = m_pDcServo_Z->GetModule()->GetMotionProcessDataBlockId("OpenDACProcessBlk", FALSE);

		hp_OpenDACProcessBlk.blkno = byOpenDACProcessBlkID;
		hp_OpenDACProcessBlk.mode = HP_PROCESS_TIME;
		hp_OpenDACProcessBlk.type = HP_PROCESS_CAPTURE_DATA;
		hp_OpenDACProcessBlk.time = -1;
		hp_OpenDACProcessBlk.source = HP_SW32_PORT_5;
		hp_OpenDACProcessBlk.destination = HP_SW32_PORT_3;

		hp_OpenDACProcessBlk.next_blkno = HP_END_OF_PROCESS;

		sReturn = hp_set_process_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_OpenDACProcessBlk);

		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Set OpenDAC Process Block Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
		*/

	
		SFM_CHipecMotionProcess SearchProcessBlk;
		Hp_process			hp_SearchProcessBlk;
		SFM_CSearchProfile	spfBondHeadZBJLSearch;
		Hp_Search_profile	hp_SearchProfile;
		Hp_Search_input		hp_SearchInput;
		byte bySearchControlBlockId = 0;

		INT nSearchSpeed = m_lUBHSearchSpeed_Z;
		INT nSearchDriveInSample = m_lUBHSearchDriveInSample_Z;
		INT nSearchDriveInSpeed = m_lUBHSearchDriveInSpeed_Z;
		INT nMinSearchTrggerSample = m_lUBHMinPosErrTriggerSample_Z;

		//CString szTemp;
		//szTemp.Format("m_lUBHSearchSpeed_Z:%d m_lUBHSearchDriveInSample_Z:%d m_lUBHSearchDriveInSpeed_Z:%d m_lMinPosErrTriggerSample_Z:%d", m_lUBHSearchSpeed_Z,m_lUBHSearchDriveInSample_Z, m_lUBHSearchDriveInSpeed_Z, m_lMinPosErrTriggerSample_Z);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		

		m_pDcServo_Z->GetModule()->AddMotionProcessDataBlock(SearchProcessBlk,"SearchProcessBlk", FALSE);
		byte bySearchProcessBlkID = m_pDcServo_Z->GetModule()->GetMotionProcessDataBlockId("SearchProcessBlk", FALSE);
		hp_SearchProcessBlk.blkno = bySearchProcessBlkID;
		hp_SearchProcessBlk.mode = HP_PROCESS_TIME;
		hp_SearchProcessBlk.type = HP_PROCESS_CAPTURE_DATA;
		hp_SearchProcessBlk.time = nMinSearchTrggerSample;
		hp_SearchProcessBlk.source = HP_SW32_PORT_0;
		hp_SearchProcessBlk.destination = HP_SW32_PORT_13;


		//hp_SearchProcessBlk.source = HP_MIN_CMD_POS_PORT + m_pDcServo_Z->GetChannelId()-1;
		//hp_SearchProcessBlk.destination = HP_SW32_PORT_14;

		
		hp_SearchProcessBlk.next_blkno = HP_END_OF_PROCESS;
		

		//hp_SearchProcessBlk.next_blkno = byOpenDACProcessBlkID;

		sReturn = hp_set_process_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_SearchProcessBlk);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Move With Final Vel Preset BJL failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
		


		m_pDcServo_Z->GetModule()->AddSearchProfileDataBlock(spfBondHeadZBJLSearch, "spfBondHeadZBJLSearch", FALSE);
		byte byBondHeadZBJLSearchProBlkID = m_pDcServo_Z->GetModule()->GetSearchProfileDataBlockId("spfBondHeadZBJLSearch", FALSE);
		hp_SearchProfile.blkno = byBondHeadZBJLSearchProBlkID;
		hp_SearchProfile.search_type = HP_SRH_RANGE;
		hp_SearchProfile.SourcePort = HP_SW32_PORT_15;
		hp_SearchProfile.SearchPort = HP_SW32_PORT_4;
		hp_SearchProfile.UpperRange = 1;
		hp_SearchProfile.LowerRange = 0;
		hp_SearchProfile.search_speed = nSearchSpeed * 256;
		hp_SearchProfile.limit = 500;
		hp_SearchProfile.debounce = 1;
		hp_SearchProfile.drivein_count = nSearchDriveInSample;
		hp_SearchProfile.drivein_speed = nSearchDriveInSpeed * 256;
		hp_SearchProfile.enable_error_chk = HP_ENABLE;

		sReturn = hp_set_search_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_SearchProfile);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Search Block Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}

		CObject* pObject;
		// May need to added select that ctrl
		if (m_pInitOperation->GetControlParameterMap()->Lookup("ctrBondHeadZObwSettling", pObject))
		{
			SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
			m_pDcServo_Z->SetControlParam("ctrBondHeadZObwSettling", *pCtr);
			CString szCurControl = m_pDcServo_Z->SelectControlParam("ctrBondHeadZObwSettling");
			bySearchControlBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSettling", FALSE);
			m_pDcServo_Z->SelectControlParam(szCurControl);
		}

		hp_SearchInput.ctrl_type = HP_CTRL_GFL_PD_INTG;
		hp_SearchInput.process_blkno = bySearchProcessBlkID;
		//hp_SearchInput.process_blkno = HP_NIL_PROCESS;
		hp_SearchInput.ctrl_blkno = bySearchControlBlockId;
		hp_SearchInput.prof_blkno = byBondHeadZBJLSearchProBlkID;
		hp_SearchInput.search_dir = HP_NEGATIVE_DIR;
		hp_SearchInput.setpos_flag = HP_DISABLE;
		hp_SearchInput.setpos_to = 0;

		sReturn = hp_setup_search_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb, sPresetBJLNo, &hp_SearchInput);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Search BLJ Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
		
		sReturn = hp_store_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb, sPresetBJLNo); 
		if (sReturn !=0)
		{
			CString szMsg;
			szMsg.Format("Hp Store Preset BJL Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}

		return TRUE;

	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
	
	return TRUE;
}

LONG CBondHead::Z_PresetBJLBond()
{
	//AfxMessageBox("Z_PresetBJL", MB_SYSTEMMODAL);
	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	short sPresetBJLNo = BP_Z_UBH_BJL_NO + 1;
	short sReturn = 0;

	sReturn = hp_free_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb, sPresetBJLNo);
	
	if (sReturn != 0)
	{
		CString szMsg;
		szMsg.Format("Free Preset BJL ERR:%d", sReturn);
		DisplayMessage(szMsg);

		return FALSE;
	}
	
	try
	{

		// Phase 1, define the obw part
		SFM_CObwProfile* pObwProfile;
		SFM_CHipecMotionProcess ObwMoveProcessBlk;
		Hp_process	hp_ObwMoveProcessBlk;
		SFM_CProfile mpfBondHeadZObwBJL;
		Hp_OBwMove_profile hp_ObwMoveProfile;
		//Hp_obw_profile_shift_factor hp_ObwMoveShiftFactor;
		Hp_OBwMove_input hp_ObwMoveInput;
		

		m_pDcServo_Z->GetModule()->AddMotionProcessDataBlock(ObwMoveProcessBlk,"ObwMoveBondProcessBlk", FALSE);
		byte byObwMoveProcessBlkID = m_pDcServo_Z->GetModule()->GetMotionProcessDataBlockId("ObwMoveBondProcessBlk", FALSE);

		CString szTemp;
		szTemp.Format("Obw Move Process:%d", byObwMoveProcessBlkID);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		
		hp_ObwMoveProcessBlk.blkno = byObwMoveProcessBlkID;
		hp_ObwMoveProcessBlk.mode = HP_PROCESS_TIME;
		hp_ObwMoveProcessBlk.type = HP_PROCESS_CAPTURE_DATA;
		hp_ObwMoveProcessBlk.time = -10;
		hp_ObwMoveProcessBlk.source = HP_MIN_CMD_POS_PORT + m_pDcServo_Z->GetChannelId()-1;
		hp_ObwMoveProcessBlk.destination = HP_SW32_PORT_14;
		hp_ObwMoveProcessBlk.next_blkno = HP_END_OF_PROCESS;
		
		sReturn = hp_set_process_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_ObwMoveProcessBlk);

		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Move Process Block Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
		
		m_pDcServo_Z->GetModule()->AddProfileDataBlock(mpfBondHeadZObwBJL,"mpfBondHeadZBondObwBJL", FALSE);
		byte byObwProfileBlockId = m_pDcServo_Z->GetModule()->GetProfileDataBlockId("mpfBondHeadZBondObwBJL", FALSE);
	
		if ((pObwProfile = GetObwProfile("mpfBondHeadZObwDefault")) == NULL)
		{
			CString szMsg;
			szMsg.Format("Hp Get Obw Profile Move Block Failed");
			DisplayMessage(szMsg);
			return FALSE;
		}
	
		hp_ObwMoveProfile.blkno = byObwProfileBlockId;
		hp_ObwMoveProfile.amax = (short) pObwProfile->GetMaxAcceleration();
		hp_ObwMoveProfile.dmax = (short) pObwProfile->GetMaxDeceleration();
		hp_ObwMoveProfile.vmax = (short) pObwProfile->GetMaxVelocity();
		hp_ObwMoveProfile.final_vel = (short) pObwProfile->GetFinalVelocity();
		
		sReturn = hp_set_obw_profile_move_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_ObwMoveProfile);
		
		if (sReturn != 0 )
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Profile Move Block Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
	/*
		hp_ObwMoveShiftFactor.acc_shift_factor = 0x0;
		hp_ObwMoveShiftFactor.blkno = byObwProfileBlockId;
		hp_ObwMoveShiftFactor.vel_shift_factor = 0x4;
		sReturn = hp_set_obw_profile_shift_factor_block (&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_ObwMoveShiftFactor);
		if (sReturn != 0 )
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Profile Shift Factor Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
		*/

		// May need to added select that ctrl
		byte byDynControlBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwDyn", FALSE);

		szTemp.Format("Obw Dyn Control Blk:%d", byDynControlBlockId);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		//CString szTemp;
		szTemp.Format("Bond Level:%d", m_lBondLevel_Z);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		hp_ObwMoveInput.ctrl_blkno = byDynControlBlockId ;
		hp_ObwMoveInput.ctrl_type = HP_CTRL_GFL_FFC_PD_INTG;
		hp_ObwMoveInput.distance =  m_lBondLevel_Z - m_lSwingLevel_Z + 300;
		//hp_ObwMoveInput.distance = m_lPickLevel_Z - m_lSwingLevel_Z;
		hp_ObwMoveInput.process_blkno = byObwMoveProcessBlkID;
		hp_ObwMoveInput.prof_blkno = byObwProfileBlockId ;

		sReturn = hp_setup_obw_mv_with_final_vel_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb ,sPresetBJLNo, &hp_ObwMoveInput);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Move With Final Vel Preset BJL failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}

		
		// Phase 2, define search part
		SFM_CHipecMotionProcess SearchProcessBlk;
		Hp_process			hp_SearchProcessBlk;
		SFM_CSearchProfile	spfBondHeadZBJLSearch;
		Hp_Search_profile	hp_SearchProfile;
		Hp_Search_input		hp_SearchInput;
		byte bySearchControlBlockId = 0;

		INT nSearchSpeed = 3;
		INT nSearchDriveInSample = 5;
		INT nSearchDriveInSpeed = 6;

		m_pDcServo_Z->GetModule()->AddMotionProcessDataBlock(SearchProcessBlk,"SearchBondProcessBlk", FALSE);
		byte bySearchProcessBlkID = m_pDcServo_Z->GetModule()->GetMotionProcessDataBlockId("SearchBondProcessBlk", FALSE);

		szTemp.Format("Search Process Blk:%d", bySearchProcessBlkID);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		hp_SearchProcessBlk.blkno = bySearchProcessBlkID;
		hp_SearchProcessBlk.mode = HP_PROCESS_TIME;
		hp_SearchProcessBlk.type = HP_PROCESS_CAPTURE_DATA;
		hp_SearchProcessBlk.time = 30;
		hp_SearchProcessBlk.source = HP_SW32_PORT_0;
		hp_SearchProcessBlk.destination = HP_SW32_PORT_13;
		hp_SearchProcessBlk.next_blkno = HP_END_OF_PROCESS;

		sReturn = hp_set_process_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_SearchProcessBlk);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Obw Move With Final Vel Preset BJL failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}

		m_pDcServo_Z->GetModule()->AddSearchProfileDataBlock(spfBondHeadZBJLSearch, "spfBondHeadZBondBJLSearch", FALSE);

		byte byBondHeadZBJLSearchProBlkID = m_pDcServo_Z->GetModule()->GetSearchProfileDataBlockId("spfBondHeadZBJLSearch", FALSE);

		szTemp.Format("Bond Head BJL Search Blk:%d", byBondHeadZBJLSearchProBlkID);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		hp_SearchProfile.blkno = byBondHeadZBJLSearchProBlkID;
		hp_SearchProfile.search_type = HP_SRH_RANGE;
		hp_SearchProfile.SourcePort = HP_SW32_PORT_15;
		hp_SearchProfile.SearchPort = HP_SW32_PORT_4;
		hp_SearchProfile.UpperRange = 1;
		hp_SearchProfile.LowerRange = 0;
		hp_SearchProfile.search_speed = nSearchSpeed * 256;
		hp_SearchProfile.limit = 200;
		hp_SearchProfile.debounce = 1;
		hp_SearchProfile.drivein_count = nSearchDriveInSample;
		hp_SearchProfile.drivein_speed = nSearchDriveInSpeed * 256;
		//hp_SearchProfile.enable_error_chk = HP_ENABLE;
		hp_SearchProfile.enable_error_chk = HP_DISABLE;

		sReturn = hp_set_search_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_SearchProfile);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Search Block Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}

		CObject* pObject;
		// May need to added select that ctrl
		if (m_pInitOperation->GetControlParameterMap()->Lookup("ctrBondHeadZObwSettling", pObject))
		{
			SFM_CControlParam* pCtr = (SFM_CControlParam *) pObject;
			m_pDcServo_Z->SetControlParam("ctrBondHeadZObwSettling", *pCtr);
			CString curControl = m_pDcServo_Z->SelectControlParam("ctrBondHeadZObwSettling");
			bySearchControlBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSettling", FALSE);
	
			// restore back to dynamic control
			m_pDcServo_Z->SelectControlParam(curControl);
		}

		hp_SearchInput.ctrl_type = HP_CTRL_GFL_PD_INTG;
		hp_SearchInput.process_blkno = bySearchProcessBlkID;
		hp_SearchInput.ctrl_blkno = bySearchControlBlockId;
		hp_SearchInput.prof_blkno = byBondHeadZBJLSearchProBlkID;
		hp_SearchInput.search_dir = HP_NEGATIVE_DIR;
		hp_SearchInput.setpos_flag = HP_DISABLE;
		hp_SearchInput.setpos_to = 0;

		sReturn = hp_setup_search_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb, sPresetBJLNo, &hp_SearchInput);
		if (sReturn != 0)
		{
			CString szMsg;
			szMsg.Format("Hp Set Search BLJ Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}
		

		sReturn = hp_store_presetbjl(&m_pDcServo_Z->GetModule()->m_HpMcb, sPresetBJLNo); 
		if (sReturn !=0)
		{
			CString szMsg;
			szMsg.Format("Hp Store Preset BJL Failed:%d", sReturn);
			DisplayMessage(szMsg);
			return FALSE;
		}

		//AfxMessageBox("Preset BJL Complete", MB_SYSTEMMODAL);
		return TRUE;

	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
	
	return TRUE;
}