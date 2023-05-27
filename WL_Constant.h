//////////////////////////////////////////////////////////////////////////////
//	WL_Constant.h : Defines the Constant for WaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			BarryChu
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

enum eWL_EVENTID {GRIPPER_LOAD_POS = 1, GRIPPER_UNLOAD_POS, GRIPPER_READY_POS, GRIPPER_BC_POS, MAGAZINE_TOP_SLOT_POS, 
MAGAZINE_PITCH, WAFER_HOME_DIE_POS_X, WAFER_HOME_DIE_POS_Y, GRIPPER_LOAD_POS2, GRIPPER_UNLOAD_POS2, GRIPPER_READY_POS2,
GRIPPER_BC_POS2, MAGAZINE_TOP_SLOT_POS2, MAGAZINE_PITCH2, WAFER_HOME_DIE_POS_X2, WAFER_HOME_DIE_POS_Y2};

//WL Axis Name
#define WL_AXIS_X						"WaferGripperAxis"
#define WL_AXIS_Z						"WaferLoaderZAxis"
#define WL_AXIS_EXP_Z					"WaferExpZAxis"
#define WL_AXIS_X2						"WaferGripper2Axis"		//ES100 v2 dual-table option
#define WL_AXIS_Z2						"WaferLoaderZ2Axis"		//ES100 v2 dual-table option
#define WL_AXIS_EXP_Z2					"WaferExpZ2Axis"		//ES100 v2 dual-table option

//WL Input Bits
#define WL_SI_MagExist					"iwMagazineExist"
#define WL_SI_MagExist2					"iwMagazineExist2"		//v4.24		//ES101
#define WL_SI_FrameDetect				"iwFrameDetect"
#define WL_SI_FrameDetect2				"iwFrameDetect2"		//v4.24		//ES101
#define WL_SI_ScopeLevel				"iwScopeLevel"
#define WL_SI_WafFrameDetect			"iwWaferFrameDetect"
#define WL_SI_WafFrameDetect2			"iwWaferFrameDetect2"	//v4.24		//ES101
#define WL_SI_FramePos					"iwFramePosition"
#define WL_SI_FrameProtection			"iwFrameProtection"
#define WL_SI_ExpanderClose				"iwExpanderClose"
#define WL_SI_Expander2Close			"iwExpander2Close"		//v4.24T4	//ES101
#define WL_SI_ExpanderOpen				"iwExpanderOpen"
#define WL_SI_Expander2Open				"iwExpander2Open"		//v4.24T4	//ES101
#define WL_SI_PusherLock				"iwPusherLock"
#define WL_SI_PusherLock2				"iwPusherLock2"
#define WL_SI_FrameExist				"iwFrameExist"
#define WL_SI_FrameExist2				"iwFrameExist2"			//v4.24T4	//ES101
#define WL_SI_LeftCover					"iLeftCoverSensor"		//v4.30		//ES101
#define WL_SI_RightCover				"iRightCoverSensor"		//v4.30		//ES101

#define WL_SI_FrontGateSnr				"iwFrontGateSensor"
#define WL_SI_BackGateSnr				"iwBackGateSensor"


//v4.31T11	//YEaly MS100Plus with Input buffer table
#define BL_SI_UpperSlotFrameExist		"ibILUpBufferExist"
#define BL_SI_LowerSlotFrameExist		"ibILLowBufferExist"
#define BL_SI_BufferFrameProtect		"ibILFrameProtect"

//WL Output Bits
#define WL_SO_HotAir					"owHotAir"
#define WL_SO_HotAir2					"owHotAir2"
#define WL_SO_DCPower					"owDCMPower"
#define WL_SO_DCMDirection				"owDCMDirecction"
#define WL_SO_GripperState				"owGripperState"
#define WL_SO_Gripper2State				"owGripperState2"
#define WL_SO_GripperLevel				"owGripperLevel"
#define WL_SO_Gripper2Level				"owGripperLevel2"
#define WL_SO_FrameVac					"owFrameVacuum"
#define WL_SO_FrameLevel				"owFrameLevel"
#define WL_SO_FrameAlign				"owFrameAlign"
#define WL_SO_FrameAlign2				"owFrameAlign2"			//v4.24T4	//ES101
//MS60 AutoLine
#define WL_SO_AL_FRONT_GATE				"owMgznFrontGate"
#define WL_SO_AL_BACK_GATE				"owMgznBackGate"
#define WL_SO_AL_MGZN_CLAMP				"owMgznClamp"

#define	WL_ON							TRUE
#define	WL_OFF							FALSE
#define	WL_UP							TRUE
#define	WL_DOWN							FALSE

#define WL_EXP_TIMEOUT					400	//3s	//4s	//v3.93
#define WL_EXP_VACUUM					0	//Vacuum version
#define WL_EXP_GEAR_FF					1	//Gear version (use standard filmframe)
#define WL_EXP_GEAR_ROF					2	//Gear version (use ring on frame)
#define WL_EXP_GEAR_DIRECTRING			3	//MS100 NuMotion DC Motor with Direct-Ring type		//v4.28T4
#define WL_EXP_GEAR_NUMOTION_DCMOTOR	4	//MS100 v2.1 NuMotion DC motor type
#define WL_EXP_CYLINDER					5
#define WL_EXP_NONE						6	//v4.39T10	//Knowles MS109 CTM manual expander support

//Error  code
#define Err_No_Error					1
#define Err_WaferGripperMove			2
#define Err_WaferLoaderZMoveHome		3
#define Err_WaferLoaderZMove			4
#define Err_WaferGripperCommutate		5
#define Err_MagazineFull				9
#define Err_NoMagazineExist				10
#define Err_NoFrameDetected				11
#define Err_FrameJammed					12
#define Err_FrameShifted				13
#define Err_NoFrameExist				14
#define Err_SearchHomeDieFailed			15
#define Err_VacuumFailed				16
#define Err_FrameExistOnTrack			17
#define Err_ScopeDown					18
#define Err_FrameExistInGripper			19

#define Err_ExpanderLockFailed			20
#define Err_ExpanderOpenCloseFailed		21
#define Err_ExpanderAlreadyOpen			22
#define Err_ExpanderAlreadyClose		23
#define Err_GripperNotInSafePos			24
#define Err_ContinueNextFrame			25

#define Err_COMPARE_BC_FAILED			26
#define Err_BATCH_ID_FAILED				27

#define ERR_ALL_MAG_FULL				28
#define ERR_CASS_FILE					29
#define ERR_MAG_NOT_SAFE				30
#define ERR_READ_BC_FAILED				31
#define ERR_WT_CANNOT_MOVE				32
#define Err_UnloadFrameFail				33
#define Err_EjNeedReplacement			34
#define Err_LoadFrameFail				35
#define Err_WaferGripperMoveHome		36
#define Err_WaferEndGenFail				37
#define Err_WaferGripperPowerOff		38
#define Err_NoFrameFoundOnLot			39
#define Err_InputAndOutputBCNotMatch	40
#define Err_NoOutputBCAndNotMatch		41

#define ERR_NOT_AVALIABLE_INPUT_SLOTS	100
#define ERR_OUTPUT_WIP_SLOTS_FULL		101
#define ERR_WL_BACK_GATE_NOT_CLOSED		102

#define	Err_AutoFocus_Z_Fail			999

#define Err_WaferNameNotFound			25

#define WL_GRIPPER_SEARCH_DIST						200
#define WL_GRIPPER_SEARCH_DIST_ON_TABLE				800		//1100		//v2.93T2
#define WL_GRIPPER_DIRECTRING_SEARCH_DIST_ON_TABLE	1200
#define WL_GRIPPER_SVO_SEARCH_DIST_ON_TABLE			1700				//v3.59		//v3.62		//pllm

#define WL_GRIPPER_CYLINDER_SEARCH_DIST				400
#define WL_GRIPPER_CYLINDER_SEARCH_DIST_ON_TABLE	2500
#define WL_GRIPPER_CYLINDER_UNLOAD_OFFSET			100

#define WL_GRIPPER_SRV_HOME_OFFSET_POS	-1500			//v3.69T1

//Align wafer option
#define WL_MANUAL_HOMEDIE				0
#define WL_ORGMAP_HOMEDIE				1
#define WL_SCNMAP_HOMEDIE				2
#define WL_SPIRAL_SEARCH				0
#define WL_CORNER_SEARCH				1

#define WL_CORNER_TL					0
#define WL_CORNER_TR					1
#define WL_CORNER_BL					2
#define WL_CORNER_BR					3

#define WL_MAX_MAG_SLOT					30

//Wafer Lot file
#define WL_LOT_HEADER_A					"[GLOBAL]"
#define WL_LOT_HEADER_CSVF				"[CSVF]"

#define	WL_LOT_CSVF_LOT					"Lot No"
#define WL_LOT_LOTNO					"Lot No"

#define WL_LOT_HEADER_A_DEFFILE			"SORTINGDEFINITION"
#define WL_LOT_HEADER_A_WAFER_NO		"NUM_OF_WAFERS"

#define WL_LOT_HEADER_B					"[WAFER]"
#define WL_LOT_WAFER_NO					"WAFER_"

//Barcode Model
#define WL_BAR_SYMBOL					0
#define WL_BAR_KEYENCE					1
//#define WL_BAR_REGAL					2
#define WL_BAR_DEFAULT					2		//v4.38T1
#define WL_BAR_DEFAULT_COM				3		//v4.40T10
#define WL_BAR_2D_BARCODE				4		//v4.50A2
#define WL_BAR_DATALOGIC				5		//v4.54T22

#define WL_BUFFER_LOWER					FALSE
#define WL_BUFFER_UPPER					TRUE

//SMF file 
#define WL_DATA							"WaferLoader Process"
#define WL_GRIPPER						"Wafer Gripper"
#define WL_GRIPPER2						"Wafer Gripper2"

#define WL_UNLOAD_POS					"Unload Position"
#define WL_LOAD_POS						"Load Position"
#define WL_READY_POS					"Ready Position"
#define WL_SCAN_POS						"Scan barcode Position"
#define WL_SCAN_CHECK_POS				"Scan barcode check Position"
#define WL_2DSCAN_POS					"2D Scan barcode Position"
#define WL_BUFLOAD_POS					"Buffer Load Position"
#define WL_POWER_ON_HOME				"Home after power on"
#define WL_STEPON_UNLOAD_POS			"StepOn Unload Position"
#define WL_STEPON_LOAD_POS				"StepOn Load Position"

#define WL_LOADER						"Wafer Loader Z"
#define WL_LOADER2						"Wafer Loader2 Z"
#define WL_AL_UNLOAD_Z					"WL AL Unload Z"
#define WL_TOP_SLOT_POS					"Top Slot Level"
#define WL_BTN_SLOT_POS					"Bottom Slot Level"
#define WL_SLOT_PITCH					"Slot Pitch"
#define WL_TOTAL_SLOT					"Total Slot No"
#define WL_SLOT_WIP_COUNTER				"Slot WIP Counter"
#define WL_SLOT_USAGE					"Slot Usage"
#define WL_SLOT_BARCODE					"Slot Barcode"
#define WL_SKIP_SLOT					"Skip Slot No"
#define WL_CUR_MGZN						"Current Mgzn No"
#define WL_CUR_SLOT						"Current Slot No"
#define WL_UNLOAD_OFFSET				"Unload Offset"
#define WL_FRAME_EXIST					"Frame Exist"					//v2.64
#define WL_FRAME_EXIST_IN_UBUFFER		"Frame Exist in UBuffer"		//v4.31T12
#define WL_FRAME_EXIST_IN_LBUFFER		"Frame Exist in LBuffer"		//v4.31T12
#define WL_BC_IN_UBUFFER				"Barcode in UBuffer"			//v4.31T12
#define WL_BC_IN_LBUFFER				"Barcode in LBuffer"			//v4.31T12
#define	WL_MAGAZINE_FULL				"Magazine Full"
#define WL_OPEN_GRIPPER_IN_UNLOAD		"Open Gripper In Unload"	
#define WL_BL_DISABLE_PRELOAD			"BL as LoaderZ Disable Preload"	//v4.39T9	//Yealy

#define WL_CURRENT_SLOT_OF_X			"Current Slot no of magazine X" //v4.46T9	//Yealy
#define WL_CURRENT_SLOT_OF_X2			"Current Slot no of magazine X2" //v4.46T9	//Yealy
#define	WL_ES_MAGAZINE3_AS_X			"WL ES Use Magazine3 As X"
#define	WL_ES_X_MAGAZINE_NO				"WL ES the Magazine No As X"

#define WL_ES_G1_CUR_SLOTNO				"Magazine G1 Current Slot no"
#define WL_ES_G2_CUR_SLOTNO				"Magazine G2 Current Slot no"
#define	WL_ES_MAGAZINE_AS_G				"WL ES Use A Magazine As G"
#define	WL_ES_MAGAZINE_G_NO				"WL ES Magazine No As G"

#define	WL_FRAME_LAOD_ON_WT1			"Frame Loaded on WT1"
#define	WL_FRAME_LAOD_ON_WT2			"Frame Loaded on WT2"

#define	WL_FRAME_TOBEUNLOADED			"Frame to be unloaded"			//v4.24T11
#define WL_IS_FRAME_PRELOADED			"Frame preloaded"				//v4.24T11
#define	WL_DISABLE_WT2					"Disable WT2"					//v4.24T11
#define WL_EXPANDER						"Wafer Expander"
#define WL_EXPANDER2					"Wafer Expander2"
#define WL_HOTAIR_OP_TIME				"Open Hot Blow Time"
#define WL_HOTAIR_CL_TIME				"Close Hot Blow Time"
#define WL_MYLARCOOL_TIME				"Mylar Paper Cool Time"
#define WL_VACUUM_BF_TIME				"Vacuum Before"
#define WL_VACUUM_AF_TIME				"Vacuum After"
#define WL_UNLOAD_PHY_X					"Unload Position X"
#define WL_UNLOAD_PHY_Y					"Unload Position Y"
#define WL_OPEN_DRIVEIN					"Open DriveIn"
#define WL_CLOSE_DRIVEIN				"Close DriveIn"
#define WL_UNLOAD_DRIVEIN				"Unload Open DriveIn"			//v4.59A17
#define WL_EXPANDER_TYPE				"Expander Type"
#define WL_WT_ALN_COUNT					"Align Frame Count"
#define WL_DCMOTOR_DAC					"Exp DC Motor DAC"
#define BL_LOADER_Y						"Bin Loader Y"					//v4.31T11	//Single Loader option only
#define WL_BUF_UPSLOT_Z					"Buffer U Slot Z"
#define WL_BUF_DNSLOT_Z					"Buffer L Slot Z"
#define WL_OPEN_DRIVEIN_OFFSETZ			"Exp DC Motor Open Z"
#define WL_CLOSE_DRIVEIN_OFFSETZ		"Exp DC Motor Close Z"

#define	WL_ES_CONTOUR_DO_GT				"ES Contour Do GT when search"
#define	WL_ES_CONTOUR_PRELOAD_MAP		"ES Contour Preload Map"
#define	WL_ES_CONTOUR_EXTRA_MARGIN_X	"ES Contour Extra Margin X"
#define	WL_ES_CONTOUR_EXTRA_MARGIN_Y	"ES Contour Extra Margin Y"
#define WL_ES_USE_CONTOUR				"Enable ES Use Contour"
#define WL_BARCODE						"Barcode Option"
#define WL_USE_BARCODE					"Enable Barcode Scan"
#define WL_COMPARE_BARCODE				"Compare Barcode"
#define WL_USE_BARCODE_FILE				"Use Barcode In Cassette File"
#define WL_NULL_BARCODE_CHECK			"Empty Barcode Check"
#define WL_2D_BARCODE_CHECK				"2D Barcode Check"
#define WL_BARCODE_NAME					"Barcode name"

#define WL_2ND_1D_BARCODE_CHECK			"2nd 1D Barcode Check"		
#define WL_SCAN_COM_PORT				"COM Port No"
#define WL_SCAN_RANGE					"Scanning Range"
#define WL_BC_CHECK_LENGTH				"Barcode Check Length"
#define WL_BC_CHECK_LENGTH_UPPER_BOUND	"Barcode Check Length Upper Bound"
#define	WL_SCAN_ROTATE_BACK				"Scanning WFT T Rotate Back"
#define	WL_SCAN_WFT_Y_RANGE				"Scanning WFT Y Range"
#define	WL_SCAN_WFT_T_ANGLE				"Scanning WFT T Angle"
#define WL_SCAN_MODEL					"Model"
#define WL_READ_ON_TABLE				"Read on Wafer Table"
#define WL_CASS_FILENAME				"Cassette Filename"


#define WL_ALIGN_WAFER					"Align Wafer Option"
#define	WL_HOME_SEARCH_ID				"Home Die PR Search ID"
#define WL_HOME_PHY_X					"Home Die Physical Pos X"
#define WL_HOME_PHY_Y					"Home Die Physical Pos Y"
#define WL_WT2_HOME_PHY_X				"WT2 Home Die Physical Pos X"
#define WL_WT2_HOME_PHY_Y				"WT2 Home Die Physical Pos Y"
#define WL_LHS_IDX_COUNT				"LHS Index Count"
#define WL_RHS_IDX_COUNT				"RHS Index Count"
#define WL_MATRIX_SIZE					"Loop Search Matrix Size"
#define WL_SRCH_HOME_OPTION				"Search HomeDie Option"
#define WL_CORNER_SRCH_OPTION			"Corner Search Option"
#define WL_ALIGN_ANGLE_MODE				"Align Wafer Angle Mode"
#define WL_PLLM_WAFER_ALIGN_OPTION		"PLLM Wafer Align Option"
#define WL_AUTO_ADAPT_WAFER				"Auto Adapt Wafer"

#define WL_EJR_CAP_CLEAN_LIMIT			"Ejector Cap Clean Limit"
#define WL_EJR_CAP_CLEAN_COUNT			"Ejecotr Cap Clean Count"

#define WL_WAFER_LOT_INFO				"Wafer Lot Information"
#define WL_LOT_CHECK_NAME				"Enable Check Wafer Name"
#define WL_LOT_FILE_PATH				"Lot Path"
#define WL_LOT_FILE_NAME				"Lot File Name"
#define WL_LOT_FILE_EXT					"Lot File Extension"

#define WL_MISC_OPTION					"Misc Option"
#define WL_AUTO_SEQUENCE				"AutoBond sequence"

#define WL_BC_FILE_MAGAZINE				"BC File Magazine"
#define WL_BC_FILE_TOTAL_SLOT			"BC File Total Slot"
#define WL_BC_FILE_SLOT_NO				"BC File Slot No"
#define WL_BC_FILE_BARCODE				"BC File Barcode"
#define WL_BC_FILE_MAG_CONFIG			"[Magazine Slot Barcode]"
#define WL_BC_FILE_MAG_BEGIN			"[Begin]"

#define WL_MAG_NO						4	//3
#define WL_MAG							"Magazine"

#define WL_MAX_MGZN_SLOT				30

#define WL_Z_POS_LIMIT_MS100			30000
#define WL_Z_NEG_LIMIT_MS100			-330000

#define WL_BARCODE_SCAN_MIN_TIME		100

#define WL_SCAN_ON_TABLE_THETA_RANGE	45




