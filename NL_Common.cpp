/////////////////////////////////////////////////////////////////
// NL_CycleState.cpp : interface of the CNVCLoader class
//
//	Description:
//		
//	Date:		23 June 2020
//	Revision:	1.00
//
//	By:	Andrew Ng		
//				
//	Copyright @ ASM Pacific Technology Ltd., .
//	ALL rights reserved.
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NVCLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


VOID CNVCLoader::InitVariable(VOID)
{
	m_bUpdateOutput				= FALSE;
	m_bNoSensorCheck			= FALSE;
	m_bWTUseGripper1ForSetup	= TRUE;
	m_bBTUseGripper1ForSetup	= FALSE;

	m_bHome_X		= FALSE;	
	m_bHome_Y		= FALSE;	
	m_bHome_Z1		= FALSE;
	m_bHome_Z2		= FALSE;

	m_bIsGripper1FrameExist	= FALSE;
	m_bIsGripper2FrameExist	= FALSE;
	m_bIsWafExpanderOpen	= FALSE;
	m_bIsBinExpanderOpen	= FALSE;

	m_bSel_X		= TRUE;
	m_bSel_Y		= TRUE;
	m_bSel_Z1		= TRUE;	
	m_bSel_Z2		= TRUE;	

	m_bXYUseEncoder	= FALSE;

	m_bIsEnabled	= TRUE;		

	m_dXRes		= 0;		
	m_dYRes		= 0;	
	m_dZRes		= 0;	

	m_lEnc_X	= 0;	
	m_lEnc_Y	= 0;		
	m_lEnc_Z1	= 0;	
	m_lEnc_Z2	= 0;

	m_lLLimit_X		= 0;
	m_lHLimit_X		= 0;
	m_lLLimit_Y		= 0;
	m_lHLimit_Y		= 0;
	m_lLLimit_Z1	= 0;
	m_lHLimit_Z1	= 0;
	m_lLLimit_Z2	= 0;
	m_lHLimit_Z2	= 0;

	m_bLoopTestStart	= FALSE;
	m_bLoopTestAbort	= FALSE;
	m_bLoopTestForward	= FALSE;
	m_ucLoopTestNo		= 0;
	m_lLoopTestStartGrade	= 1;
	m_lLoopTestEndGrade		= 25;
	m_lDiagMoveDistance	= 0;

	m_lBackupPosition = 0;
	m_lBackupPos_X	= 0;
	m_lBackupPos_Y	= 0;
	m_lBackupPos_Z	= 0;

	m_lOMRT		= NL_MODE_A;		// 'A' + NL_MODE_F = 'F'
	m_lOMSP		= NL_MODE_A;		// 'A' + NL_MODE_F = 'F'
	m_szOMSP	= _T("A");
	m_bSetSlotBlockEnable = FALSE;
    m_ulMgznSelected	= 0;
    m_ulSlotSelected	= 0;
    m_ulSlotPhyBlock	= 0;

	m_lCurrMgzn1		= 0;
	m_lCurrMgzn2		= 0;
	m_lCurrSlotID1		= 0;
	m_lCurrSlotID2		= 0;
	m_lCurrHmiMgzn1		= 0;
	m_lCurrHmiMgzn2		= 0;
	m_lCurrHmiSlot1		= 0;
	m_lCurrHmiSlot2		= 0;
	m_szCurrHmiMgznName1	= "";
	m_szCurrHmiMgznName2	= "";
	m_szFrameBarcode1	= "";
	m_szFrameBarcode2	= "";

	m_lCurrWafSlotID	= 1;
	m_lCurrWTBlock		= 0;
	m_lCurrBTBlock		= 0;
	m_lCurrBTMgzn		= 0;
	m_lCurrBTSlotID		= 0;

	m_lSelMagazineID	= 0;
	m_lSelSlotID		= 1;
	m_dSlotPitch		= 0.0;

	m_lWTLoadOffsetX		= 0;
	m_lWTLoadOffsetY		= 0;
	m_lWTLoadOffsetZ		= 0;
	m_lWTUnloadOffsetX		= 0;
	m_lWTUnloadOffsetY		= 0;
	m_lWTUnloadOffsetZ		= 0;

	m_lBTLoadOffsetX		= 0;
	m_lBTLoadOffsetY		= 0;
	m_lBTLoadOffsetZ		= 0;
	m_lBTUnloadOffsetX		= 0;
	m_lBTUnloadOffsetY		= 0;
	m_lBTUnloadOffsetZ		= 0;

	m_lMgznLoadOffsetX		= 0;
	m_lMgznLoadOffsetY		= 0;
	m_lMgznLoadOffsetZ		= 0;
	m_lMgznUnloadOffsetX	= 0;
	m_lMgznUnloadOffsetY	= 0;
	m_lMgznUnloadOffsetZ	= 0;

    m_lNLGeneral_1	= 0;		
	m_lNLGeneral_2	= 0;		
    m_lNLGeneral_3	= 0;		
    m_lNLGeneral_4	= 0;		
	m_lNLGeneral_5	= 0;		
    m_lNLGeneral_6	= 0;		
    m_lNLGeneral_7	= 0;		
	m_lNLGeneral_8	= 0;		
	m_lNLGeneral_9	= 0;		
	m_lNLGeneral_10	= 0;	
	m_lNLGeneral_11	= 0;
	m_lNLGeneral_12	= 0;
	m_lNLGeneral_TmpA = 0;

    for (LONG i = 0; i < MS_BL_MGZN_NUM; i++)
    {
        m_stNVCMgznRT[i].m_lMidPosnY	= 0;
        m_stNVCMgznRT[i].m_lMidPosnY2	= 0;
		m_stNVCMgznRT[i].m_lTopLevel	= 0;
		m_stNVCMgznRT[i].m_lTopLevel2	= 0;
		m_stNVCMgznRT[i].m_lMidPosnZ1  = 0;
        m_stNVCMgznRT[i].m_lMidPosnZ2  = 0;
        m_stNVCMgznRT[i].m_lSlotPitch	= 19000;
        m_stNVCMgznRT[i].m_lNoOfSlots	= MS_BL_MGZN_SLOT;
        m_stNVCMgznRT[i].m_lMgznUsage	= NL_MGZN_USAGE_UNUSE;
        m_stNVCMgznRT[i].m_lMgznState	= NL_MGZN_STATE_OK;
        
		for (LONG j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
        {
            m_stNVCMgznRT[i].m_lSlotBlock[j] = 0;
            m_stNVCMgznRT[i].m_lSlotUsage[j] = NL_SLOT_USAGE_UNUSE;
			m_stNVCMgznRT[i].m_lTransferSlotUsage[j] = NL_SLOT_USAGE_UNUSE;
            m_stNVCMgznRT[i].m_SlotBCName[j].Empty();
			m_stNVCMgznRT[i].m_SlotSN[j].Empty();
			m_stNVCMgznRT[i].m_SlotLotNo[j].Empty();
        }
    }

	m_lBTUnloadPos_X        = 0;
	m_lBTUnloadPos_Y        = 0;		
}

INT CNVCLoader::OpInitialize()
{
	m_qSubOperation = WAIT_IDLE_Q;
	
	DisplaySequence("NL - Operation Initialize");
	return gnOK;
}

INT CNVCLoader::OpPreStart()
{
	m_qSubOperation = WAIT_IDLE_Q;
	return gnOK;
}

BOOL CNVCLoader::LoadData(VOID)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSFileUtility *pUtl = CMSFileUtility::Instance();
    CStringMapFile *psmf = NULL;
	
	if (pUtl->LoadNLConfig() == FALSE)
	{
		return FALSE;
	}

    psmf = pUtl->GetNLConfigFile();
	if (psmf == NULL)
	{
		return FALSE;
	}

	m_lOMRT						= (*psmf)[NL_SYS_DATA]["Operation Mode"];
	m_lOMSP						= (*psmf)[NL_SYS_DATA]["Operation Mode SP"];
	m_lOMRT	= NL_MODE_A;
	m_lOMSP	= NL_MODE_A;

	////////////////////////////////////////////////////////////////////////////
	// System Data that follows machine only
	m_lCurrMgzn1				= (LONG)(*psmf)[NL_SYS_DATA]["Current Mgzn 1"];
	m_lCurrMgzn2				= (LONG)(*psmf)[NL_SYS_DATA]["Current Mgzn 2"];
	m_lCurrSlotID1				= (LONG)(*psmf)[NL_SYS_DATA]["Current Slot 1"];
	m_lCurrSlotID2				= (LONG)(*psmf)[NL_SYS_DATA]["Current Slot 2"];
	m_lCurrBlock1				= (LONG)(*psmf)[NL_SYS_DATA]["Current Block 1"];
	m_lCurrBlock2				= (LONG)(*psmf)[NL_SYS_DATA]["Current Block 2"];

	m_lCurrWTBlock				= (LONG)(*psmf)[NL_SYS_DATA]["Current Waf BLK"];
	m_lCurrWafSlotID			= (LONG)(*psmf)[NL_SYS_DATA]["Current Waf Slot ID"];

	////////////////////////////////////////////////////////////////////////////
	// General Options Data that follows PKG
	//m_bNoSensorCheck			= (BOOL)(LONG)(*psmf)[NL_GEN_DATA]["No Sensor Check"];

	////////////////////////////////////////////////////////////////////////////
	// XY Table Data (follows PKG)
	m_lStandByPos_X				= (*psmf)[NL_XYTABLE]["Standby Pos"]["X"];
	m_lStandByPos_Y				= (*psmf)[NL_XYTABLE]["Standby Pos"]["Y"];
	m_lWTLoadUnloadPos1_X		= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 1"]["X"];
	m_lWTLoadUnloadPos1_Y		= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 1"]["Y"];
	m_lWTLoadUnloadPos2_X		= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 2"]["X"];
	m_lWTLoadUnloadPos2_Y		= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 2"]["Y"];
	m_lBTLoadUnloadPos1_X		= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 1"]["X"];
	m_lBTLoadUnloadPos1_Y		= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 1"]["Y"];
	m_lBTLoadUnloadPos2_X		= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 2"]["X"];
	m_lBTLoadUnloadPos2_Y		= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 2"]["Y"];

	m_lStandByPos_Z1			= (*psmf)[NL_XYTABLE]["Standby Pos"]["Z1"];
	m_lStandByPos_Z2			= (*psmf)[NL_XYTABLE]["Standby Pos"]["Z2"];
	m_lWafLoadPos_Z1			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos"]["Z1"];
	m_lWafLoadPos_Z2			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos"]["Z2"];
	m_lBinLoadPos_Z1			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos"]["Z1"];
	m_lBinLoadPos_Z2			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos"]["Z2"];

	m_lWTUnloadOffsetX			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Unload Offset"]["X"];
	m_lWTUnloadOffsetY			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Unload Offset"]["Y"];
	m_lWTUnloadOffsetZ			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Unload Offset"]["Z"];
	m_lWTLoadOffsetX			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Offset"]["X"];
	m_lWTLoadOffsetY			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Offset"]["Y"];
	m_lWTLoadOffsetZ			= (*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Offset"]["Z"];

	m_lBTUnloadOffsetX			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Unload Offset"]["X"];
	m_lBTUnloadOffsetY			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Unload Offset"]["Y"];
	m_lBTUnloadOffsetZ			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Unload Offset"]["Z"];
	m_lBTLoadOffsetX			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Offset"]["X"];
	m_lBTLoadOffsetY			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Offset"]["Y"];
	m_lBTLoadOffsetZ			= (*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Offset"]["Z"];

	m_lMgznUnloadOffsetX		= (*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Unload Offset"]["X"];
	m_lMgznUnloadOffsetY		= (*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Unload Offset"]["Y"];
	m_lMgznUnloadOffsetZ		= (*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Unload Offset"]["Z"];
	m_lMgznLoadOffsetX			= (*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Load Offset"]["X"];
	m_lMgznLoadOffsetY			= (*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Load Offset"]["Y"];
	m_lMgznLoadOffsetZ			= (*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Load Offset"]["Z"];

	////////////////////////////////////////////////////////////////////////////
	// Gripper Z1 and Z2 Data (follows PKG)

	////////////////////////////////////////////////////////////////////////////
	// BinTable Related Data for NVC Loader (follows PKG)
	m_lBTUnloadPos_X	= (*psmf)[NL_BINTABLE_DATA][NL_BT_UNLOAD_PHY_X];		
	m_lBTUnloadPos_Y	= (*psmf)[NL_BINTABLE_DATA][NL_BT_UNLOAD_PHY_Y];		

	////////////////////////////////////////////////////////////////////////////
	// WaferTable Related Data for NVC Loader (follows PKG)

	////////////////////////////////////////////////////////////////////////////
	// NVC real-time MGZN Config Data (follows PKG)
	CString szName1, szName2, szName3, szName4, szName5, szName6, szName7, szName8;
	for (INT i = 0; i < MS_BL_MGZN_NUM; i++)
    {
        szName1.Format("%s %d %s", NL_MGZN, i, NL_SLOT_TOP_LVL);
        szName2.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_POS);
        szName3.Format("%s %d %s", NL_MGZN, i, NL_SLOT_PITCH);
        szName4.Format("%s %d %s", NL_MGZN, i, NL_TOTAL_SLOT);
        szName5.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_Z1);
        szName6.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_Z2);
		//andrewng //2020-0910
        szName7.Format("%s %d %s", NL_MGZN, i, NL_SLOT_TOP_LVL2);
        szName8.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_POS2);

        m_stNVCMgznRT[i].m_lTopLevel	= (*psmf)[NL_DATA][NL_ELEVATOR][szName1];
        m_stNVCMgznRT[i].m_lMidPosnY	= (*psmf)[NL_DATA][NL_ELEVATOR][szName2];
        m_stNVCMgznRT[i].m_lSlotPitch	= (*psmf)[NL_DATA][NL_ELEVATOR][szName3];
        m_stNVCMgznRT[i].m_lNoOfSlots	= (*psmf)[NL_DATA][NL_ELEVATOR][szName4];
        m_stNVCMgznRT[i].m_lMidPosnZ1	= (*psmf)[NL_DATA][NL_ELEVATOR][szName5];
        m_stNVCMgznRT[i].m_lMidPosnZ2	= (*psmf)[NL_DATA][NL_ELEVATOR][szName6];
        m_stNVCMgznRT[i].m_lTopLevel2	= (*psmf)[NL_DATA][NL_ELEVATOR][szName7];
        m_stNVCMgznRT[i].m_lMidPosnY2	= (*psmf)[NL_DATA][NL_ELEVATOR][szName8];
    }

    pUtl->CloseNLConfig();

	////////////////////////////////////////////////////////////////////////////
	// Update SRAM after SaveData
	//(*m_psmfSRam)["BinLoader"]["UseBracode"] = m_bUseBarcode;
	
	////////////////////////////////////////////////////////////////////////////
	// Update HMI after SaveData
	UpdateHmiMgznSlotID();

	return TRUE;
}

BOOL CNVCLoader::SaveData(VOID)
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CMSFileUtility *pUtl = CMSFileUtility::Instance();
    CStringMapFile *psmf = NULL;
	
	if (pUtl->LoadNLConfig() == FALSE)
	{
		return FALSE;
	}


    psmf = pUtl->GetNLConfigFile();
	if (psmf == NULL)
	{
		return FALSE;
	}
	
	m_lOMRT	= NL_MODE_A;

	////////////////////////////////////////////////////////////////////////////
	// System Data that follows machine only
	(*psmf)[NL_SYS_DATA]["Operation Mode"]		= m_lOMRT;
	(*psmf)[NL_SYS_DATA]["Operation Mode SP"]	= m_lOMSP;
	(*psmf)[NL_SYS_DATA]["Current Mgzn 1"]		= m_lCurrMgzn1;
	(*psmf)[NL_SYS_DATA]["Current Mgzn 2"]		= m_lCurrMgzn2;
	(*psmf)[NL_SYS_DATA]["Current Slot 1"]		= m_lCurrSlotID1;
	(*psmf)[NL_SYS_DATA]["Current Slot 2"]		= m_lCurrSlotID2;
	(*psmf)[NL_SYS_DATA]["Current Block 1"]		= m_lCurrBlock1;
	(*psmf)[NL_SYS_DATA]["Current Block 2"]		= m_lCurrBlock2;
	(*psmf)[NL_SYS_DATA]["Current Waf BLK"]			= m_lCurrWTBlock;
	(*psmf)[NL_SYS_DATA]["Current Waf Slot ID"]		= m_lCurrWafSlotID;

	////////////////////////////////////////////////////////////////////////////
	// General Options Data that follows PKG
	(*psmf)[NL_GEN_DATA]["No Sensor Check"]		= m_bNoSensorCheck;

	////////////////////////////////////////////////////////////////////////////
	// XY Table Data (follows PKG)
	(*psmf)[NL_XYTABLE]["Standby Pos"]["X"]		= m_lStandByPos_X;
	(*psmf)[NL_XYTABLE]["Standby Pos"]["Y"]		= m_lStandByPos_Y;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 1"]["X"]	= m_lWTLoadUnloadPos1_X;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 1"]["Y"]	= m_lWTLoadUnloadPos1_Y;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 2"]["X"]	= m_lWTLoadUnloadPos2_X;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos 2"]["Y"]	= m_lWTLoadUnloadPos2_Y;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 1"]["X"]	= m_lBTLoadUnloadPos1_X;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 1"]["Y"]	= m_lBTLoadUnloadPos1_Y;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 2"]["X"]	= m_lBTLoadUnloadPos2_X;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos 2"]["Y"]	= m_lBTLoadUnloadPos2_Y;
 
	(*psmf)[NL_XYTABLE]["Standby Pos"]["Z1"]	= m_lStandByPos_Z1;
	(*psmf)[NL_XYTABLE]["Standby Pos"]["Z2"]	= m_lStandByPos_Z2;

	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos"]["Z1"]	= m_lWafLoadPos_Z1;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Unload Pos"]["Z2"]	= m_lWafLoadPos_Z2;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos"]["Z1"]	= m_lBinLoadPos_Z1;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Unload Pos"]["Z2"]	= m_lBinLoadPos_Z2;

	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Unload Offset"]["X"]	= m_lWTUnloadOffsetX;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Unload Offset"]["Y"]	= m_lWTUnloadOffsetY;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Unload Offset"]["Z"]	= m_lWTUnloadOffsetZ;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Offset"]["X"]		= m_lWTLoadOffsetX;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Offset"]["Y"]		= m_lWTLoadOffsetY;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_WT]["Load Offset"]["Z"]		= m_lWTLoadOffsetZ;

	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Unload Offset"]["X"]	= m_lBTUnloadOffsetX;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Unload Offset"]["Y"]	= m_lBTUnloadOffsetY;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Unload Offset"]["Z"]	= m_lBTUnloadOffsetZ;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Offset"]["X"]		= m_lBTLoadOffsetX;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Offset"]["Y"]		= m_lBTLoadOffsetY;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_BT]["Load Offset"]["Z"]		= m_lBTLoadOffsetZ;

	(*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Unload Offset"]["X"]	= m_lMgznUnloadOffsetX;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Unload Offset"]["Y"]	= m_lMgznUnloadOffsetY;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Unload Offset"]["Z"]	= m_lMgznUnloadOffsetZ;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Load Offset"]["X"]	= m_lMgznLoadOffsetX;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Load Offset"]["Y"]	= m_lMgznLoadOffsetY;
	(*psmf)[NL_XYTABLE][NL_XYTABLE_MGZN]["Load Offset"]["Z"]	= m_lMgznLoadOffsetZ;


	////////////////////////////////////////////////////////////////////////////
	// Gripper Z1 and Z2 Data (follows PKG)

	////////////////////////////////////////////////////////////////////////////
	// BinTable Related Data for NVC Loader (follows PKG)
	(*psmf)[NL_BINTABLE_DATA][NL_BT_UNLOAD_PHY_X]		= m_lBTUnloadPos_X;		
	(*psmf)[NL_BINTABLE_DATA][NL_BT_UNLOAD_PHY_Y]		= m_lBTUnloadPos_Y;		

	////////////////////////////////////////////////////////////////////////////
	// WaferTable Related Data for NVC Loader (follows PKG)

	////////////////////////////////////////////////////////////////////////////
	// NVC real-time MGZN Config Data (follows PKG)
	CString szName1, szName2, szName3, szName4, szName5, szName6, szName7, szName8;
	for (INT i = 0; i < MS_BL_MGZN_NUM; i++)
    {
        szName1.Format("%s %d %s", NL_MGZN, i, NL_SLOT_TOP_LVL);
        szName2.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_POS);
        szName3.Format("%s %d %s", NL_MGZN, i, NL_SLOT_PITCH);
        szName4.Format("%s %d %s", NL_MGZN, i, NL_TOTAL_SLOT);
        szName5.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_Z1);
        szName6.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_Z2);
		//andrewng //2020-0910
        szName7.Format("%s %d %s", NL_MGZN, i, NL_SLOT_TOP_LVL2);
        szName8.Format("%s %d %s", NL_MGZN, i, NL_SLOT_MID_POS2);

        (*psmf)[NL_DATA][NL_ELEVATOR][szName1]   = m_stNVCMgznRT[i].m_lTopLevel;
        (*psmf)[NL_DATA][NL_ELEVATOR][szName2]   = m_stNVCMgznRT[i].m_lMidPosnY;
        (*psmf)[NL_DATA][NL_ELEVATOR][szName3]   = m_stNVCMgznRT[i].m_lSlotPitch;
        (*psmf)[NL_DATA][NL_ELEVATOR][szName4]   = m_stNVCMgznRT[i].m_lNoOfSlots;
        (*psmf)[NL_DATA][NL_ELEVATOR][szName5]   = m_stNVCMgznRT[i].m_lMidPosnZ1;
        (*psmf)[NL_DATA][NL_ELEVATOR][szName6]   = m_stNVCMgznRT[i].m_lMidPosnZ2;
        (*psmf)[NL_DATA][NL_ELEVATOR][szName7]   = m_stNVCMgznRT[i].m_lTopLevel2;
        (*psmf)[NL_DATA][NL_ELEVATOR][szName8]   = m_stNVCMgznRT[i].m_lMidPosnY2;
    }


    pUtl->UpdateNLConfig();
    pUtl->CloseNLConfig();


	////////////////////////////////////////////////////////////////////////////
	// Update SRAM after SaveData
	//(*m_psmfSRam)["BinLoader"]["UseBracode"]	= m_bUseBarcode;


	////////////////////////////////////////////////////////////////////////////
	// Update HMI after SaveData
	UpdateHmiMgznSlotID();

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////
//	NL MGZN Supporting Functions 
///////////////////////////////////////////////////////////////////////////

BOOL CNVCLoader::LoadMgznOMData(VOID)
{
    short i, j;
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility *pUtl = CMSFileUtility::Instance();
    CStringMapFile *psmf;

	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	CString szMsg, szSMF_NameBLOM;

	m_lOMSP	= NL_MODE_A;

    // open config file
    if (pUtl->LoadBLOMConfig() == FALSE)
	{
		HmiMessage("NL: fail to load BLMgzSetup.msd");
		return FALSE;
	}

    // get file pointer
    psmf = pUtl->GetBLOMConfigFile();
	if (psmf == NULL)
	{
		return FALSE;
	}

    for (i = 0; i < MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", NL_MODE, m_lOMSP + 'A');
		
		m_stMgznOM[i].m_lMgznUsage = (*psmf)[NL_MGZN_DEFAULT][szMgzName][szModeName][NL_MGZN_USAGE];

        for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", NL_SLOT, j+1); 

			m_stMgznOM[i].m_lSlotBlock[j] = (*psmf)[NL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][NL_SLOT_BLOCK];
			m_stMgznOM[i].m_lSlotUsage[j] = (*psmf)[NL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][NL_SLOT_USAGE];
        }
    }

    // close config file
    pUtl->CloseBLOMConfig();
	return TRUE;
}

BOOL CNVCLoader::SaveMgznOMData(VOID)
{
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	CString szSMF_NameBLOM,szMsg;
	CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];

	m_lOMSP	= NL_MODE_A;

    // open config file
    if (pUtl->LoadBLOMConfig() == FALSE)
	{
		return FALSE;
	}

    // get file pointer
	psmf = pUtl->GetBLOMConfigFile();
	
	//Check Load/Save Data
	if (psmf == NULL)
	{
		return FALSE;
	}

    // update data
    for (LONG i = 0; i < MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", NL_MODE, m_lOMSP + 'A');
		
		(*psmf)[NL_MGZN_DEFAULT][szMgzName][szModeName][NL_MGZN_USAGE] = m_stMgznOM[i].m_lMgznUsage;

        for (LONG j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", NL_SLOT, j + 1); 

			(*psmf)[NL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][NL_SLOT_BLOCK] = m_stMgznOM[i].m_lSlotBlock[j];
			(*psmf)[NL_MGZN_DEFAULT][szMgzName][szModeName][szSlotName][NL_SLOT_USAGE] = m_stMgznOM[i].m_lSlotUsage[j];
        }
    }

	//Check Load/Save Data
    pUtl->UpdateBLOMConfig();

	// close config file
    pUtl->CloseBLOMConfig();

	m_szOMSP = _T("A");

    return TRUE;
}

BOOL CNVCLoader::LoadMgznRTData(VOID)
{
   short i, j;
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility *pUtl = CMSFileUtility::Instance();
    CStringMapFile *psmf;

	m_lOMRT	= NL_MODE_A;


    // open config file
    if (pUtl->LoadBLRTConfig() == FALSE)
	{
		return FALSE;	
	}

    // get file pointer
    psmf = pUtl->GetBLRTConfigFile();
	if (psmf == NULL)
	{
		return FALSE;
	}

    // retrive data
    for (i = 0; i < MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", NL_MODE, m_lOMRT + 'A');
		
		m_stNVCMgznRT[i].m_lMgznUsage = (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][NL_MGZN_USAGE];
		m_stNVCMgznRT[i].m_lMgznState = (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][NL_MGZN_STATE];

        for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", NL_SLOT, j+1); 

			m_stNVCMgznRT[i].m_lSlotBlock[j]		= (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_BLOCK];
			m_stNVCMgznRT[i].m_lSlotWIPCounter[j]	= (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_WIP];
			m_stNVCMgznRT[i].m_lSlotUsage[j]		= (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_USAGE];
			m_stNVCMgznRT[i].m_lTransferSlotUsage[j] = (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_TRANSFER_SLOT_USAGE];
			m_stNVCMgznRT[i].m_SlotBCName[j]		= (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_BCODE];
			m_stNVCMgznRT[i].m_SlotSN[j]			= (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_SN];
			m_stNVCMgznRT[i].m_SlotLotNo[i]		= (*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_LOT_NO];
        }
	}


    // close config file
    pUtl->CloseBLRTConfig();
    return TRUE;
}

BOOL CNVCLoader::SaveMgznRTData(VOID)
{
    short i, j;
    CString szMgzName, szModeName, szSlotName;
    CMSFileUtility *pUtl = CMSFileUtility::Instance();
    CStringMapFile *psmf = NULL;

	m_lOMRT	= NL_MODE_A;

    // open config file
    if (pUtl->LoadBLRTConfig() == FALSE)
	{
		return FALSE;
	}

    // get file pointer
	psmf = pUtl->GetBLRTConfigFile();
	if (psmf == NULL)
	{
		return FALSE;
	}

    // update data
    for (i = 0; i < MS_BL_MGZN_NUM; i++)
    {
		szMgzName = GetMagazineName(i);
		szModeName.Format("%s %c", NL_MODE, m_lOMRT + 'A');

		(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][NL_MGZN_USAGE] = m_stNVCMgznRT[i].m_lMgznUsage;
		(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][NL_MGZN_STATE] = m_stNVCMgznRT[i].m_lMgznState;

        for (j = 0; j < MS_BL_MAX_MGZN_SLOT; j++)
        {
			szSlotName.Format("%s %d", NL_SLOT, j+1); 

			(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_BLOCK]	= m_stNVCMgznRT[i].m_lSlotBlock[j];
			(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_WIP]	= m_stNVCMgznRT[i].m_lSlotWIPCounter[j];
			(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_USAGE]	= m_stNVCMgznRT[i].m_lSlotUsage[j];
			(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_TRANSFER_SLOT_USAGE] = m_stNVCMgznRT[i].m_lTransferSlotUsage[j];
			(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_BCODE]	= m_stNVCMgznRT[i].m_SlotBCName[j];
			(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_SN]		= m_stNVCMgznRT[i].m_SlotSN[j];
			(*psmf)[NL_MGZN_CURRENT][szMgzName][szModeName][szSlotName][NL_SLOT_LOT_NO] = m_stNVCMgznRT[i].m_SlotLotNo[j];
        }
    }

    pUtl->UpdateBLRTConfig();
    pUtl->CloseBLRTConfig();	
    return TRUE;
}



BOOL CNVCLoader::GetLoadMgzSlot(LONG &lMgzn, LONG &lSlot, BOOL &bUseEmpty, LONG lPhysicalBlock)
{
	INT i = 0, j = 0;
	BOOL bExit = FALSE;
	CString szLog;

	for (i = 0; i < MS_BL_MGZN_NUM; i++)
	{
		if ( (m_stNVCMgznRT[i].m_lMgznUsage != NL_MGZN_USAGE_ACTIVE) && 
			 (m_stNVCMgznRT[i].m_lMgznUsage != NL_MGZN_USAGE_MIXED) )	
		{
			continue;
		}

		for (j = 0; j < MS_BL_MGZN_SLOT; j++)
		{
			if ( (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_UNUSE) ||
				 (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_FULL)  ||	
				 (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_EMPTY) )	
			{
				continue;
			}

			if (m_stNVCMgznRT[i].m_lSlotBlock[j] == lPhysicalBlock)
			{
				if (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_ACTIVE2FULL)
				{
					bUseEmpty = TRUE;
				}
				else
				{
					bUseEmpty = FALSE;
				}
				bExit = TRUE;
				break;
			}
		}

		if (bExit == TRUE)
		{
			break;
		}
	}

	if (bExit == FALSE)
	{
		return FALSE;
	}

	lMgzn = i;
	lSlot = j;
	return TRUE;
}

BOOL CNVCLoader::GetFullMgznSlot(LONG &lMgzn, LONG &lSlot, LONG lPhysicalBlock)
{
	INT i = 0, j = 0;
	BOOL bExit = FALSE;
	CString szLog;


	for (i = 0; i < MS_BL_MGZN_NUM; i++) // get full slot in full magazine
	{
		if ( (m_stNVCMgznRT[i].m_lMgznUsage == NL_MGZN_USAGE_FULL) ||
			 (m_stNVCMgznRT[i].m_lMgznUsage == NL_MGZN_USAGE_MIXED) )	
		{
			for (j = 0; j < MS_BL_MGZN_SLOT; j++)
			{
				if ( (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_FULL) &&
					 (m_stNVCMgznRT[i].m_lSlotBlock[j] == 0) )
				{
					bExit = TRUE;
					break;
				}
			}
		}

		if (bExit == TRUE)
		{
			break;
		}
	}

	if (bExit == FALSE)
	{
		SetAlert_Red_Yellow(IDS_BL_MA_NOFREESLOT_IN_FULLMAG);
		SetErrorMessage("BL Mode error: No free slot found in FULL magazine");
		return FALSE;
	}

	lMgzn = i;
	lSlot = j;

	CString szMagSlot;
	szMagSlot.Format("%d/%d", lMgzn + 1, lSlot + 1);
	(*m_psmfSRam)["BinLoader"]["CurFullSlot"] = szMagSlot;

	return TRUE;
}

BOOL CNVCLoader::GetMgznSlot(LONG lPhysicalBlock, LONG& lMgzn, LONG& lSlot)
{
	INT i = 0, j = 0;
	BOOL bExit = FALSE;
	CString szLog;


	for (i = 0; i < MS_BL_MGZN_NUM; i++)
	{
		if ( (m_stNVCMgznRT[i].m_lMgznUsage != NL_MGZN_USAGE_ACTIVE) &&
			 (m_stNVCMgznRT[i].m_lMgznUsage != NL_MGZN_USAGE_MIXED) )		
		{
			continue;
		}

		for (j = 0; j < MS_BL_MGZN_SLOT; j++)
		{
			if ( (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_UNUSE) ||
				 (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_FULL)	||	
				 (m_stNVCMgznRT[i].m_lSlotUsage[j] == NL_SLOT_USAGE_EMPTY) )	
			{
				continue;
			}

			if (m_stNVCMgznRT[i].m_lSlotBlock[j] == lPhysicalBlock)
			{
				bExit = TRUE;
				break;
			}
		}

		if (bExit == TRUE)
		{
			break;
		}
	}

	if (bExit == FALSE)
	{
		return FALSE;
	}

	lMgzn = i;
	lSlot = j;
	return TRUE;
}


BOOL CNVCLoader::GetCurrWafMgzSlot(LONG lLoadBlock, LONG &lMgzn, LONG &lSlot)
{
	INT i = 0, j = 0;
	BOOL bExit = FALSE;
	CString szLog;


	for (i = 0; i < MS_BL_MGZN_NUM; i++)
	{
		if (m_stNVCMgznRT[i].m_lMgznUsage != NL_MGZN_USAGE_WAFER)		
		{
			continue;
		}

		if ((lLoadBlock > m_stNVCMgznRT[i].m_lNoOfSlots)	|| 
			(lLoadBlock > MS_BL_MAX_MGZN_SLOT)			||
			(lLoadBlock <= 0) )
		{
			return FALSE;
		}

		j = lLoadBlock - 1;	//j is 0-based

		m_stNVCMgznRT[i].m_lSlotUsage[j];
		bExit = TRUE;

		if (bExit == TRUE)
		{
			break;
		}
	}

	if (bExit == FALSE)
	{
		return FALSE;
	}

	lMgzn = i;		//0-based
	lSlot = j;		//0-based
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////
//	NL Other Utility Functions 
///////////////////////////////////////////////////////////////////////////

LONG CNVCLoader::NL_DEBUGBOX(CString str)
{
	CMSLogFileUtility::Instance()->BL_LogStatus(str);
	return TRUE;
}

LONG CNVCLoader::NL_DEBUGMESSAGE(CString str)
{
	DisplayMessage(str);
	return TRUE;
}

LONG CNVCLoader::NL_OK_CANCEL(CString strText, CString strTitle)
{
	SetAlertTime(TRUE, EQUIP_ASSIST_TIME, strTitle);

	EquipStateProductiveToUnscheduleDown();	
	
	LONG lStatus = HmiMessage_Red_Back(strText, strTitle, glHMI_MBX_OKCANCEL|0x80000000,
		glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	
	EquipStateUnscheduleDwonToProductive();

	if (lStatus != glHMI_OK)
	{
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return FALSE;
	}
    else
	{
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return TRUE;
	}
}

LONG CNVCLoader::NL_YES_NO(CString strText, CString strTitle)
{
	SetAlertTime(TRUE, EQUIP_ASSIST_TIME, strTitle);

	if( HmiMessage_Red_Back(strText, strTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL)!=glHMI_YES )
	{
		//SetAlarmLamp_Yellow();
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return FALSE;
	}
    else
	{
		SetAlertTime(FALSE, EQUIP_ASSIST_TIME, strTitle);
		return TRUE;
	}
}

VOID CNVCLoader::SetBinTableJoystick(BOOL bState)
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	stMsg.InitMessage(sizeof(BOOL), &bState);

	if (m_ulJoyStickMode == MS899_JS_MODE_PR)
	{
		nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SetMouseControlCmd", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID,stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bState);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		m_lJsBinTableInUse = 0;	//BT1
	}
	else
	{
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "SetJoystickCmdWithoutSrCheck", stMsg);
	
		// Get the reply
		m_comClient.ScanReplyForConvID(nConvID, 5000);
		m_comClient.ReadReplyForConvID(nConvID,stMsg);
	}
}





