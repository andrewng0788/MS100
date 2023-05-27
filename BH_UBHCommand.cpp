#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A_Constant.h"
#include "BondHead.h"

LONG CBondHead::ContactSearchTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	AfxMessageBox("UBH_Z_Contact_Search", MB_SYSTEMMODAL);
	//Z_ContactSearchBondLevel()
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::BondHeadTiltAngleTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	//if (CheckZMotorType() == BH_DC_SVO_MOTOR)
	if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
		(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		Z_TiltAngleTest();
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::StartUBHZTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (State() != IDLE_Q)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//if (CheckZMotorType() == BH_DC_SVO_MOTOR)
	if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
		(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		//T_Profile(CLEAN_COLLET_PROF);	
		//T_MoveTo(m_lBondPos_T, SFM_WAIT);
		T_Profile(NORMAL_PROF);

		//T_Sync();

		InitBHZFlexiControlPort();
		Z_MoveTo(m_lSwingLevel_Z,SFM_WAIT);
		Z_PresetBJL();

		m_bStartUBHZTest = TRUE;
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::StopUBHZTest(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	
	//if (CheckZMotorType() == BH_DC_SVO_MOTOR)
	if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
		(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		m_bStartUBHZTest = FALSE;
		
		Z_Home();
		Sleep(500);

		T_Profile(SETUP_PROF); //CLEAN_COLLET_PROF);
		T_MoveTo(m_lPrePickPos_T, SFM_WAIT);
		T_Profile(NORMAL_PROF);
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::ZBJLTest(IPC_CServiceMessage& svMsg)
{
	AfxMessageBox("Start BJL Test", MB_SYSTEMMODAL);
	BOOL bReturn = TRUE;
	short sPresetBJLNo = BP_Z_UBH_BJL_NO;
	short sReturn = 0;

	//if (CheckZMotorType() == BH_DC_SVO_MOTOR)
	if ( (CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) &&
		(CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		InitBHZFlexiControlPort();
		Z_MoveTo(0,SFM_WAIT);

		if (Z_PresetBJL())
		{
			sReturn = hp_run_presetbjl(&m_pDcServo_Z->m_HpCcb, sPresetBJLNo);

			if (sReturn != 0)
			{
				AfxMessageBox("Fail to Run BJL", MB_SYSTEMMODAL);
				Z_Home();
				Z_ResetFlexiPorts();
				svMsg.InitMessage(sizeof(BOOL), &bReturn);
				return 1;

			}
			
			Z_BondDelay();
			
		}
		
		Z_MoveTo(0,SFM_WAIT);
		Z_ResetFlexiPorts();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondHead::UBHLearnPickLevelDac_Z(IPC_CServiceMessage& svMsg)
{
	//if (CheckZMotorType() != BH_DC_SVO_MOTOR)
	if ( !(CMS896AApp::m_lBondHeadConfig == MS896A_ULTRA_BOND_HEAD) ||
		(!CMS896AStn::MotionIsDcServo(BH_AXIS_Z, &m_stBHAxis_Z)) )
	{
		return TRUE;
	}

	Z_MoveTo(m_lSwingLevel_Z);
	T_MoveTo(m_lPickPos_T);
	
	short sReturn = 0;
	BOOL bReturn = TRUE;

	SFM_CHipecMotionProcess LearnDACProcessBlk;	
	Hp_process hp_LearnDACProcessBlk;

	m_pDcServo_Z->GetModule()->AddMotionProcessDataBlock(LearnDACProcessBlk,"LearnDACProcess", FALSE);
	byte byLearnDACProcessBlkID = m_pDcServo_Z->GetModule()->GetMotionProcessDataBlockId("LearnDACProcess", FALSE);

	//m_pDcServo_Z->
	//LearnDACProcessBlk

	//CString szTemp;
	//szTemp.Format("Obw Move Process:%d", byLearnDACProcessBlkID);
	//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	hp_LearnDACProcessBlk.blkno = byLearnDACProcessBlkID;
	hp_LearnDACProcessBlk.mode = HP_PROCESS_TIME;
	hp_LearnDACProcessBlk.type = HP_PROCESS_CAPTURE_DATA;
	hp_LearnDACProcessBlk.time = -1;
	hp_LearnDACProcessBlk.source = HP_CH_MTRDAC_PORT_0 + m_pDcServo_Z->GetChannelId()-1;
	hp_LearnDACProcessBlk.destination = HP_SW32_PORT_1;
	hp_LearnDACProcessBlk.next_blkno = HP_END_OF_PROCESS;
	
	sReturn = hp_set_process_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &hp_LearnDACProcessBlk);
	
	if (sReturn != 0 )
	{
		CString szMsg;
		szMsg.Format("hp_set_process_block: %d", sReturn);
		DisplayMessage(szMsg);
		return FALSE;
	}

	/*
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
	*/

	// May need to added select that ctrl
	SFM_CHipecDdbModule* pDDBModule = (SFM_CHipecDdbModule*) m_pDcServo_Z->GetModule();
	byte byObwProfileBlockId = pDDBModule->m_ObwProfileBlock.GetDataBlockId("mpfBondHeadZObwDefault", FALSE);
	//byte byObwProfileBlockId = m_pDcServo_Z->GetModule()->GetProfileDataBlockId("mpfBondHeadZObwDefault", FALSE);
	byte byDynControlBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwDyn", FALSE);
	
	Hp_OBwMove_input hp_ObwMoveInput;
	hp_ObwMoveInput.ctrl_blkno = byDynControlBlockId ;
	hp_ObwMoveInput.ctrl_type = HP_CTRL_GFL_FFC_PD_INTG;
	hp_ObwMoveInput.distance =  m_lPickLevel_Z;
	hp_ObwMoveInput.process_blkno = HP_NIL_PROCESS;
	hp_ObwMoveInput.prof_blkno = byObwProfileBlockId ;

	
	SFM_CHipecDelay delayLearnBHZDAC;
	Hp_Delay_profile BHZModuleDelayBlock;
	Hp_Delay_input hp_LearnBHZDACDelay;
	m_pDcServo_Z->GetModule()->AddDelayProfileDataBlock(delayLearnBHZDAC, "delayLearnBHZDAC", FALSE);

	byte byDelayBHZ = m_pDcServo_Z->GetModule()->GetDelayProfileDataBlockId("delayLearnBHZDAC", FALSE);
	
	BHZModuleDelayBlock.blkno = byDelayBHZ;
	BHZModuleDelayBlock.delay_time = 32;

	hp_set_delay_block(&m_pDcServo_Z->GetModule()->m_HpMcb, &BHZModuleDelayBlock);
	byte byStaticControlBlockId = m_pDcServo_Z->GetModule()->GetControlParamDataBlockId("ctrBondHeadZObwSts", FALSE);
	
	hp_LearnBHZDACDelay.ctrl_type		= HP_CTRL_GFL_FFC_PD_INTG;
	//m_hpBHDelayInput_Z.ctrl_blkno		= bSettlingBlockId;
	hp_LearnBHZDACDelay.ctrl_blkno		= byStaticControlBlockId;
	hp_LearnBHZDACDelay.process_blkno	= byLearnDACProcessBlkID;
	hp_LearnBHZDACDelay.prof_blkno		= byDelayBHZ;
	

	sReturn = hp_obw_move_to(&m_pDcServo_Z->m_HpCcb, &hp_ObwMoveInput);

	if (sReturn != 0 )
	{
		CString szMsg;
		szMsg.Format("hp_set_process_block: %d", sReturn);
		DisplayMessage(szMsg);
		return FALSE;
	}

	
	//SetupDataLogZ();
	//EnableDataLogZ(TRUE);
	sReturn = hp_delay(&m_pDcServo_Z->m_HpCcb, &hp_LearnBHZDACDelay);

	if (sReturn != 0 )
	{
		CString szMsg;
		szMsg.Format("hp_set_process_block: %d", sReturn);
		DisplayMessage(szMsg);
		return FALSE;
	}
	

	sReturn = hp_motion_complete(&m_pDcServo_Z->m_HpCcb, 160000); 

	//EnableDataLogZ(FALSE);

	if (sReturn != 0)
	{
		CString szMsg;
		szMsg.Format("hp_motion_complete: %d", sReturn);
		DisplayMessage(szMsg);
		return FALSE;
	}

	ReadSoftwarePort(m_pDcServo_Z->m_HpCcb.hp_servo_ch.p, HP_SW32_PORT_1, m_lUBHPickLevelDac_Z);

	// Update the dac value
	m_lUBHPickLevelDac_Z = abs(m_lUBHPickLevelDac_Z);

	//LONG lTemp;
	//ReadSoftwarePort(m_pDcServo_Z->m_HpCcb.hp_servo_ch.p, HP_SW32_PORT_1, m_lPickLevelDac_Z);
	//m_lPickLevelDac_Z = abs(m_lPickLevelDac_Z);
	
	//ReadSoftwarePort(m_pDcServo_Z->m_HpCcb.hp_servo_ch.p, HP_CH_MTRDAC_PORT_0 + m_pDcServo_Z->GetChannelId()-1, 
	//		lTemp);

	CString szMsg;
	szMsg.Format("Start Open Dac with value: %d", m_lUBHPickLevelDac_Z);
	AfxMessageBox(szMsg, MB_SYSTEMMODAL);
	
	//Sleep(m_lBondDelay);

	/*


	EnableBHZOpenDac(TRUE);
	Sleep(1000);
	EnableBHZOpenDac(FALSE);
	
	Sleep(300);
	*/

	//AfxMessageBox("Start Open DAC", MB_SYSTEMMODAL);
	//EnableBHZOpenDacWithFlexiControl(TRUE);
	//Sleep(m_lPickDelay);
	//EnableBHZOpenDacWithFlexiControl(FALSE);
	//AfxMessageBox("End Open DAC", MB_SYSTEMMODAL);
	
	//Sleep(300);
	
	//AfxMessageBox("Home", MB_SYSTEMMODAL);
	//Z_Home();	

	Z_MoveTo(0);
	T_MoveTo(m_lPrePickPos_T);
	
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;

}