/////////////////////////////////////////////////////////////////
//	NL_Constant.h : Defines the Constant for NVCLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Tue, July 07, 2020
//	Revision:	1.00
//
//	Copyright @ ASM Pacific Technology Ltd., 2020.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////
// Motor AXIS  Data

#define NL_AXIS_X				"NVCLoaderXAxis"
#define NL_AXIS_Y				"NVCLoaderYAxis"
#define NL_AXIS_Z1				"NVCLoaderZ1Axis"
#define NL_AXIS_Z2				"NVCLoaderZ2Axis"

#define NL_SLOW_PROF		0
#define NL_NORMAL_PROF		1
#define NL_FAST_PROF		2

//SI bits
#define NL_SI_FrameExist1		"ibFrameExist1"
#define NL_SI_FrameExist2		"ibFrameExist2"

//SO bits
#define NL_SO_Clamp1Open		"obSolAOpen"
#define NL_SO_Clamp1Close		"obSolAClose"
#define NL_SO_Clamp2Open		"obSolBOpen"
#define NL_SO_Clamp2Close		"obSolBClose"
#define NL_SO_Gripper1State		"oBinClamp"
#define NL_SO_Gripper2State		"oWaferClamp"

#define NL_ENC_TOL_X		100
#define NL_ENC_TOL_Y		100


/////////////////////////////////////////////////////////////////
// PKG Data

#define NL_GEN_DATA				"General Data"
#define NL_SYS_DATA				"System Data"

#define BL_MGZN_DATA			"Mgzn Data"

#define NL_XYTABLE				"XY Table"
#define NL_XYTABLE_WT			"WT"
#define NL_XYTABLE_BT			"BT"
#define NL_XYTABLE_MGZN			"MGZN"

#define NL_GRIPPER				"Gripper 1"
#define NL_GRIPPER2				"Gripper 2"



/////////////////////////////////////////////////////////////////
// MGZN Data

#define NL_MGZ_TOP_1				0
#define NL_MGZ_MID_1				1
#define NL_MGZ_BTM_1				2
#define NL_MGZ_TOP_2				3
#define NL_MGZ_MID_2				4
#define NL_MGZ_BTM_2				5
#define NL_MGZ8_BTM_1				6
#define NL_MGZ8_BTM_2				7

#define NL_MGZN_DEFAULT				"Default Magazine Setting"
#define NL_MGZN_CURRENT				"Current Magazine Setting"

#define NL_MGZN_DEFAULT				"Default Magazine Setting"
#define NL_MGZN_CURRENT				"Current Magazine Setting"
#define NL_MODE						"MD"
#define NL_MGZN						"MGZ"
#define NL_SLOT						"SLOT"
#define NL_MGZN_USAGE				"Mgz Usage"
#define NL_MGZN_STATE				"Mgzn State"
#define NL_SLOT_BLOCK				"Block"
#define NL_SLOT_WIP					"WIP"
#define NL_SLOT_USAGE				"Usage"
#define NL_TRANSFER_SLOT_USAGE		"Transfer Usage"
#define NL_SLOT_BCODE               "BarCode"
#define NL_SLOT_SN		            "SN"
#define NL_SLOT_LOT_NO		        "Lot No"
#define NL_SLOT_TRANSFER_ACTION		"TransferAction"

#define NL_DATA						"NVCLoader Data"
#define NL_ELEVATOR					"NVC Magazine"
#define NL_SLOT_TOP_LVL				"Top Slot Pos X"
#define NL_SLOT_TOP_LVL2			"Top Slot Pos X2"
#define NL_SLOT_MID_POS			    "Center Slot Pos Y"
#define NL_SLOT_MID_POS2			"Center Slot Pos Y2"
#define NL_SLOT_PITCH				"Slot Pitch"
#define NL_TOTAL_SLOT				"Total Slot No"
#define NL_SLOT_MID_Z1			    "Center Slot Pos Z1"
#define NL_SLOT_MID_Z2			    "Center Slot Pos Z2"

#define NL_WAFTABLE_DATA			"NVC WafTable Data"

#define NL_BINTABLE_DATA			"NVC BinTable Data"
#define NL_BT_UNLOAD_PHY_X			"BT Unload Pos X"
#define NL_BT_UNLOAD_PHY_Y			"BT Unload Pos Y"


enum // operation mode
{
    NL_MODE_A = 0,
    NL_MODE_B,
    NL_MODE_C,
    NL_MODE_D,
    NL_MODE_E,			//125 bin config			//v3.45
	NL_MODE_F,			//MS100 8mag 4x2 150bins config		//v3.82
	NL_MODE_G,			//MS100 8mag 4x2 175bins config		//v3.82
	NL_MODE_H			//MS100Plus 4mag 4x1 100bins config	//v4.31T10	//Yearly MS100Plus
};

enum // magazine id
{
    NL_MGZN_TOP1 = 0,
    NL_MGZN_MID1,
    NL_MGZN_BTM1,
    NL_MGZN_TOP2,
    NL_MGZN_MID2,
    NL_MGZN_BTM2,
	NL_MGZN8_BTM1,		//MS100 8mag 4x2 config		//v3.82
	NL_MGZN8_BTM2		//MS100 8mag 4x2 config		//v3.82
};

// the usage element for magazine
enum
{
    NL_MGZN_USAGE_UNUSE = 0,
    NL_MGZN_USAGE_FULL,
    NL_MGZN_USAGE_EMPTY,
    NL_MGZN_USAGE_ACTIVE,
    NL_MGZN_USAGE_MIXED,
	NL_MGZN_USAGE_WAFER
};

// magazine state, normal, full, empty
enum
{
    NL_MGZN_STATE_OK=0,
    NL_MGZN_STATE_FULL,
    NL_MGZN_STATE_EMPTY
};


enum // slot usage state
{
	NL_SLOT_USAGE_UNUSE = 0,
	NL_SLOT_USAGE_FULL,
	NL_SLOT_USAGE_EMPTY,
	NL_SLOT_USAGE_ACTIVE,
	NL_SLOT_USAGE_ACTIVE2FULL,		// just for run time use, at setup, no such option
	NL_SLOT_USAGE_INUSE,			//New for Mode-H only (Yealy)	//v4.34T1
	NL_SLOT_USAGE_SORTING
};
