//////////////////////////////////////////////////////////////////////////////
//	BL_Constant.h : Defines the Constant for BinLoader class
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

enum eBL_EVENTID {BT_UNLOAD_POS_X = 1, BT_UNLOAD_POS_Y, BIN_FRAME_REALIGN, BIN_FRAME_REALIGN_COUNT, 
BIN_FRAME_REALIGN_DELAY, BINTABLE_VACUUM, BINTABLE_VACUUM_DELAY, EXCHANGE_ARM_VACUUM_SETTLE_DELAY,
BIN_GRIPPER_PREUNLOAD_POS, BIN_GRIPPER_LOAD_MAG_POS, BIN_GRIPPER_UNLOAD_POS, BIN_GRIPPER_READY_POS, 
BIN_GRIPPER_BC_POS, BIN_GRIPPER_UNLOAD_OFFSET, BIN_GRIPPER_LOAD_IN_CLAMP_OFFSET_X, EXCHANGE_ARM_PICK_POS,
EXCHANGE_ARM_PLACE_POS, BIN_SCAN_BARCODE, BIN_CHECK_BARCODE, BL_MAGAZINE_TOP_SLOT_POS, BL_MAGAZINE_CENTER_Y_POS,
BL_MAGAZINE_SLOT_PITCH, ELEVATOR_Y_POS, ELEVATOR_Z_POS, ELEVATOR_CHECK, BIN_GRIPPER_UNLOAD_MAG_POS,};

enum eBL_CLEAR_FRAME_MODE {BL_CLEAR_FRAME_MODE_DEFAULT = 0, BL_CLEAR_FRAME_MODE_ASC, BL_CLEAR_FRAME_MODE_DEC};

//Motor Axis
#define BL_AXIS_X					"BinGripperAxis"
#define BL_AXIS_X2					"BinGripper2Axis"
#define BL_AXIS_Y					"BinLoaderYAxis"
#define BL_AXIS_Z					"BinLoaderZAxis"
#define BL_AXIS_EXARM				"BinExchgArmAxis"
#define BL_AXIS_THETA				"BinLoaderThetaAxis"
#define BL_AXIS_UPPER				"BinLoaderUpperAxis"
#define BL_AXIS_LOWER				"BinLoaderLowerAxis"

#define BL_SLOW_PROF				0
#define BL_NORMAL_PROF				1
#define BL_FAST_PROF				2

//SI bits
#define BL_SI_FrameLevel			"ibFrameLevel"
#define BL_SI_FrameLevel2			"ibFrameLevel2"
#define BL_SI_FramePos				"ibFramePosition"
#define BL_SI_FramePos2				"ibFramePosition2"
#define BL_SI_FrameProtect			"ibFrameProtection"
#define BL_SI_FrameProtect2			"ibFrameProtection2"
#define BL_SI_FrameInMag			"ibFrameInMagazine"
#define BL_SI_FrameDetect			"ibFrameDetect"
#define BL_SI_FrameDetect2			"ibFrameDetect2"
#define BL_SI_FrameVac				"iwFrameVacuum"
#define BL_SI_FrameVac2				"iwFrameVacuum2"

#define BL_SI_BufferSafetySensor	"ibBufferSafetySensor"

#define BL_SI_ElevDoorCover				"ibElevatorDoorCover"
#define BL_SI_FrontRightElevDoorCover	"ibFrontRightElevatorDoorCover"

#define BL_SI_ElevDoorCover2			"ibElevatorDoorCover2"
#define BL_SI_FrontRightElevDoorCover2	"ibFrontRightElevatorDoorCover2"
//#define BL_SI_ElevCover2			"ibElevatorCover2"		//v4.06		//CEMark 2nd bit
//#define BL_SI_RELEVATORCOVER		"ibRElevatorCover"			// Rear Elevator cover // MS109
//#define BL_SI_RELEVATORCOVER2		"ibRElevatorCover2"

//#define BL_SI_FRONT_LEFT_COVER2	"iFLeftCover2"
//#define BL_SI_COVER2				"iCoverSensor2"		

#define BL_SI_FrameOutMag			"ibFrameOutOfMgz"
#define BL_SI_FrameOutMag2			"ibFrameOutOfMgz2"
#define BL_SI_MagExist1				"ibMgznExist1"
#define BL_SI_MagExist2				"ibMgznExist2"
#define BL_SI_MagExist3				"ibMgznExist3"
#define BL_SI_MagExist4				"ibMgznExist4"
#define BL_SI_MagExist5				"ibMgznExist5"
#define BL_SI_MagExist6				"ibMgznExist6"
#define BL_SI_MagExist7				"ibMgznExist7"			//v4.03		//MS100 8mag sensor support
#define BL_SI_MagExist8				"ibMgznExist8"			//v4.03	//MS100 8mag sensor support
#define BL_SI_MagExist				"ibMagazineExist1"
#define BL_SI_Mag2Exist				"ibMagazine2Exist1"
#define BL_SI_BufferFrameExist		"ibBufferFrameExist"
#define BL_SI_RArmFrameExist		"ibRearArmFrameExist"
#define BL_SI_FArmFrameExist		"ibFrontArmFrameExist"
#define BL_SI_FArmReady				"ibFontArmReady"
#define BL_SI_RArmReady				"ibRearArmReady"
//Dual Buffer Table
#define BL_SI_DualBufferUpperExist		"ibUpBufferExist"
#define BL_SI_DualBufferLowerExist		"ibLowBufferExist"
#define BL_SI_DualBufferRightProtect	"ibBufferRProtection"
#define BL_SI_DualBufferLeftProtect		"ibBufferLProtection"
#define BL_SI_DualBufferLevel			"ibBufferLevel"
//MS90 New Buffer Pusher Snrs	//v4.59A40
#define BL_SI_MS90_BUF_PUSHER_Z_SAFETY	"ibMS90BufPusherZHome"
#define BL_SI_MS90_BUF_PUSHER_X_LIMIT	"ibMS90BufPusherXLimit"
#define BL_SI_MS90_BUF_PUSHER_X_HOME	"ibMS90BufPusherXHome"
//MS50_02
#define BL_SI_FRAMEJAM1				"ibFrameJam"
#define BL_SI_FRAMEJAM2				"ibFrameJam2"

//AutoLine 
#define BL_SI_FrontGateSnr				"ibFrontGateSensor"
#define BL_SI_BackGateSnr				"ibBackGateSensor"

//Search profile	//v3.94T3
#define BL_SP_HOME_Y				"spfBinLoaderYHome"
#define BL_SP_INDEX_Y				"spfBinLoaderYIndex"
#define BL_SP_HI_SPD_HOME_Y			"spfBinLoaderYHiSpdHome"
#define BL_SP_INDEX_Z				"spfBinLoaderZIndex"
#define BL_SP_UPPER_LIMIT_Z			"spfBinLoaderZUpperLimit"
#define BL_SP_HOME_SENSOR_Z			"spfBinLoaderZHomeSensor"

#define BL_SP_BIN_GRIPPER_SRCH_IN_CLAMP	"spfBinGripperSearchInClamp"
#define BL_SP_BIN_GRIPPER_SRCH_JAM		"spfBinGripperSearchJam"
//MOVE profile		//v3.94T3
#define BL_MP_HI_SPD_HOME_Y			"mpfBinLoaderHiSpdHomeY"

//SO bits
#define BL_SO_GripperState			"obGripperState"
#define BL_SO_Gripper2State			"obGripperState2"
#define BL_SO_GripperLevel			"obGripperLevel"
#define BL_SO_Gripper2Level			"obGripper2Level"
#define BL_SO_GripperPusher			"obGripperPusher"
#define BL_SO_FrameVac				"obFrameVacuum"
#define BL_SO_FrameVac2				"obFrameVacuum2"
#define BL_SO_FrameLevel			"obFrameLevel"
#define BL_SO_FrameLevel2			"obFrameLevel2"
#define BL_SO_FrameUpBlow			"obFrameUpBlow"
#define BL_SO_FrameAlign			"obFrameAlign"
#define BL_SO_FrameAlign2			"obFrameAlign2"
#define BL_SO_BufferAlign			"obBufferAlign"
#define BL_SO_FArmLevel				"obFrontArmLevel"
#define BL_SO_FArmVac				"obFrontArmVacuum"
#define BL_SO_RArmLevel				"obRearArmLevel"
#define BL_SO_RArmVac				"obRearArmVacuum"
//Dual Buffer Table
#define BL_SO_DualBufferLevel		"obBufferLevel"
//MS60 AutoLine
#define BL_SO_AL_FRONT_GATE			"obMgznFrontGate"
#define BL_SO_AL_BACK_GATE			"obMgznBackGate"
//#define BL_SO_AL_MGZN_CLAMP			"obMgznClamp"
//MS90 New Buffer Pusher 	//v4.59A40
#define BL_SO_MS90_BUF_PUSHER_Z		"obMS90BufPusherZ"
#define BL_SO_MS90_BUF_PUSHER_X		"obMS90BufPusherX"
//MS50	//v4.59A40
#define BL_SO_BHUPDNSOL				"obBHUpDnSol"

//v4.31T11		//MS100Plus single-loader option	//Yealy MS100Plus
#define MS_BL_WL_MGZN_NUM			4

#define BL_MGZ_TOP_1				0
#define BL_MGZ_MID_1				1
#define BL_MGZ_BTM_1				2
#define BL_MGZ_TOP_2				3
#define BL_MGZ_MID_2				4
#define BL_MGZ_BTM_2				5
#define BL_MGZ8_BTM_1				6
#define BL_MGZ8_BTM_2				7

//Error  code
#define Err_BinGripperMoveHome			1
#define Err_BinGripperMove				2
#define Err_BinLoaderZMoveHome			3
#define Err_BinLoaderZMove				4
#define Err_BinLoaderZCommutate			5
#define Err_BinLoaderYMoveHome			6
#define Err_BinLoaderYMove				7
#define Err_BinLoaderYCommutate			8
#define Err_BinExchgArmMoveHome			9
#define Err_BinExchgArmMove				10
#define	Err_BinExchgArmCommutate		11
#define Err_BinMagzineOutputWIPFull		20
#define Err_BinBackGateNotClosed		21
#define Err_BinLoaderZNotPowerOn		22
#define Err_BinLoaderZNotHomed			23
#define Err_BinLoaderThetaMissingStep	24
#define Err_BinLoaderMagazineNotExist	25

#define Err_MotorMissingStep			30


#define Err_BLLoadEmptyBarcodeFail	-2		//v2.83T46	//PLLM
#define Err_BLLoadEmptyBarcodeStop	-3		//v2.83T46	//PLLM
#define Err_BLLoadDuplicateBarcode	-4		//v2.83T70	//v3.70T3
#define Err_BLLoadEmptyAbort		-5		//shiraishi01

#define BL_FRAME_NOT_EXIST			0
#define BL_FRAME_ON_RHS_EDGE		1
#define BL_FRAME_ON_LHS_EDGE		2
#define BL_FRAME_ON_CENTER			3

#define BL_MGZ_IS_FULL				2

//Default barcode name
#define BL_DEFAULT_BARCODE			"Unknown"

//Slot Full history file
#define BL_TEMP_MAG_NAME			"C:\\MapSorter\\UserData\\Statistic\\BL_Temp"
#define BL_HISTORY_NAME				"C:\\MapSorter\\UserData\\Statistic\\BL_History.txt"
#define BL_HISTORY_TITLE			"Unload Date/Time,MagNo,SlotNo,Barcode Name,Grade ,SNRN ,Output,OPTFN"
#define BL_BINFRAME_STATUS_SUMMARY	"C:\\MapSorter\\UserData\\Statistic\\BL_BinFrameStatusSummary.txt"
#define BL_BINFRAME_STATUS_SUMMARY_TITLE "Date Time, Status, MagNo, SlotNo, Barcode, Grade"



//Magazine Full summary file
#define BL_SUMMARY_PATH				"C:\\MapSorter\\UserData\\Statistic"
#define BL_SUMMARY_TITLE			"Unload Date/Time,SlotNo,Barcode Name,Grade ,SNRN ,Output,OPTFN"
#define BL_SUMMARY_TITLE2			"Unload Date/Time,SlotNo,Barcode Name,Grade ,SNRN ,Output,OPTFN,Wafer ID"
#define BL_SUMMARY_TITLE3			"Unload Date/Time,SlotNo,Barcode Name,Grade ,SNRN ,Output,OPTFN,Wafer ID,Bin Full Count"


//Gripper Home Offset & Max pos limit
#define BL_GRIPPER_HOME_OFFSET						-300
#define BL_GRIPPER_MAX_POS_LIMIT					18500
#define BL_DTABLE_GRIPPER_MAX_POS_LIMIT				8000

//ExArm Safe Position Tolarence
#define BL_EXARM_SAFE_POS_TOLARENCE					500
#define BL_EXARM_PICK_PLACE_SAFE_POS_TOLARENCE		200

//v3.59T1
//Gripper Channel Type
#define BL_SVO_MOTOR				6
#define BL_STP_MOTOR				5

//Barcode Model
#define BL_BAR_SYMBOL				0
#define BL_BAR_KEYENCE				1
#define BL_BAR_CCD					2
#define BL_BAR_REGAL				3
#define BL_BAR_DATALOGIC			4

#define BL_BUFFER_SIZE				50

//SMF file 
#define BL_DATA						"BinLoader Process"
#define	BL_RUNTIME					"RunTime Data"

#define BL_BARCODE                  "Bar Code"
#define BL_USE_BARCODE              "Use Barcode"
#define BL_USE_EXTENSION            "Use Extension"
#define BL_CHK_BARCODE				"Check Barcode"
#define	BL_CHK_BC_SKIP_EMPTY		"Check Barcode Skip Empty"
#define BL_CMP_BARCODE				"Compare Barcode"
#define BL_STOP_CHGGRADE			"Disable ChgGrade Scanning"
#define BL_EXTENSION_NAME           "Extension Name"
#define BL_SCAN_RANGE               "Scan Range"
#define BL_RETRY_COUNTER            "Retry Counter"
#define BL_SCAN_COM_PORT            "COM Port No"
#define BL_SCAN_MODEL				"Model"
#define BL_BARCODE_PREFIX			"Barcode Prefix"
#define BL_BARCODE_LENGTH			"Barcode Length"

#define BL_GRIPPER					"Bin Gripper"
#define BL_GRIPPER2					"Bin Gripper 2"
#define BL_PREUNLOAD_POS            "PreUnload Position"
#define BL_UNLOAD_POS				"Unload Position"
#define BL_LOAD_MAG_POS				"Load Position"
#define BL_UNLOAD_MAG_POS			"Unload Magazine Position"
#define BL_READY_POS				"Ready Position"
#define BL_SCAN_POS					"Scan barcode Position"
#define BL_BUFFER_UNLOAD_POS		"Buffer Unload Position"
#define BL_POWER_ON_HOME			"Home after power on"

#define BL_WL_ELEVATOR				"Bin WL Magazine"

#define BL_ELEVATOR					"Bin Magazine"
#define BL_OPERATION_MODE           "Operation Mode"
#define BL_CUR_MAGAZINE             "Current Magazine No"
#define BL_CUR_SLOT					"Current Slot No"
#define BL_READY_POS_Y              "Ready Position Y"
#define BL_READY_LVL_Z              "Ready Level Z"
#define BL_AUTOLINE_UNLOAD_Z		"AutoLine Unload Z"
#define BL_LOAD_POS1_T				"Load Pos1 T"
#define BL_LOAD_POS2_T				"Load Pos2 T"
#define BL_READY_POS_T				"Ready Pos T"
#define BL_UNLOAD_OFFSET            "Unload Offset"
#define BL_UNLOAD_OFFSET_X			"Unload Offset X"
#define BL_UNLOAD_OFFSET_Y			"Unload Offset Y"
#define BL_UNLOAD_PUSHER_OFFSET_X	"Unload Pusher Offset X"
#define BL_LOAD_SEARCHINCLAMP_OFFSET_X	"Load InClamp Offset X"
#define BL_SIS_TOP1_LOAD_CLAMP_OFFSET_X "SIS TOP1 Load Clamp Offset X"
#define BL_TOP2_LOAD_CLAMP_OFFSET_X		"TOP2 Load Clamp Offset X"
#define BL_SIS_TOP1_LOAD_CLAMP_OFFSET_X2 "SIS TOP1 Load Clamp Offset X2"
#define BL_TOP2_LOAD_CLAMP_OFFSET_X2	 "TOP2 Load Clamp Offset X2"
#define BL_SLOT_TOP_LVL				"Top Slot Level"
#define BL_SLOT_MID_POS			    "Center Slot Position"
#define BL_SLOT_PITCH				"Slot Pitch"
#define BL_TOTAL_SLOT				"Total Slot No"

#define BL_CUR_MAGAZINE2            "Current Magazine 2 No"
#define BL_CUR_SLOT2				"Current Slot 2 No"

#define BL_EXCHG_ARM				"Bin Exchange Arm"
#define BL_EXARM_SCAN_BARCODE_ON_GRIPPER	"Arm Scan Barcode On Gripper"
#define BL_EXARM_PICK_POS			"Arm Pick Position"
#define BL_EXARM_PLACE_POS			"Arm Place Position"
#define BL_EXARM_READY_POS			"Arm Ready Position"
#define BL_EXARM_PICK_DELAY			"Arm Pick Delay"
#define BL_EXARM_VACUUM_DELAY		"Arm Vacuum Delay"
#define BL_EXARM_UP_DELAY			"Arm Up Delay"
#define BL_EXARM_CURR_BLOCK			"Current Buffer Block"
#define BL_EXARM_PRELOAD_EMPTY_ON_BUFFER	"Preload Empty Frame On Buffer"

#define BL_DUAL_DL					"Bin DualTable"
#define BL_DUAL_DL_PRELOAD_EMPTY	"Preload Empty Frame"

#define BL_MGZN_DEFAULT				"Default Magazine Setting"
#define BL_MGZN_CURRENT				"Current Magazine Setting"
#define BL_MODE                     "MD"
#define BL_MGZN                     "MGZ"
#define BL_MGZN2                    "MGZ2"						//MS100 9Inch dual-table config
#define BL_SLOT                     "SLOT"
#define BL_MGZN_USAGE               "Mgz Usage"
#define BL_MGZN_STATE               "Mgzn State"
#define BL_SLOT_BLOCK               "Block"
#define BL_SLOT_WIP					"WIP"
#define BL_SLOT_USAGE               "Usage"
#define BL_TRANSFER_SLOT_USAGE		"Transfer Usage"
#define BL_SLOT_BCODE               "BarCode"
#define BL_SLOT_SN		            "SN"
#define BL_SLOT_LOT_NO		        "Lot No"
#define BL_SLOT_TRANSFER_ACTION		"TransferAction"

#define BL_TABLE					"Bin Table"
#define BL_UNLOAD_PHY_X				"Unload Position X"
#define BL_UNLOAD_PHY_Y				"Unload Position Y"
#define BL_UNLOAD_PHY_X2			"Unload Position X2"
#define BL_UNLOAD_PHY_Y2			"Unload Position Y2"
#define BL_BARCODE_POS_X			"Barcode Position X"
#define BL_BARCODE_POS_Y			"Barcode Position Y"
#define BL_BTCURR_BLOCK             "Current Block"
#define BL_BTCURR_BLOCK2            "Current Block 2"
#define BL_BTCURR_MGZN				"BT Current Magazine No"
#define BL_BTCURR_SLOT				"BT Current Slot No"
#define BL_REALIGN_BF               "Realign Bin Frame"
#define BL_USE_VACUUM               "Use Vacuum"
#define BL_VACUUM_DELAY				"Vacuum Delay"
#define BL_FRAME_UPBLOW_TIME		"Frame Up Blow Time"
#define BL_BT_ALN_COUNT				"Align Frame Count"
#define BL_BT_ALN_DELAY				"Align Frame Delay"
#define BL_VACUUM_SETTLE_DELAY		"Vacuum Settle Delay"		//v3.67T1
#define BL_LOAD_OFFSET_Y			"Load Offset Y"				//v3.79
#define	BL_CHECK_BC_SAME_FAIL		"Check BC Same Fail"
#define BL_BUT_ALN_COUNT			"Align Buffer Frame Count"
#define BL_BUT_ALN_DELAY			"Align Buffer Frame Delay"
#define	BL_WAFER_TO_BIN_RESORT		"Bin Loader Wafer To Bin Resort"
#define BL_CHK_BF_ALIGNED			"Check Frame Aligned"
#define BL_CHK_ELEV_COVER			"Check Elevator Cover"
#define BL_EXARM_OFFSET_X			"ExArm Unload Offset X"		//v3.74T41
#define BL_EXARM_OFFSET_Y			"ExArm Unload Offset Y"		//v3.74T41
#define BL_MS90_TABLE_PUSHER		"MS90 Table Pusher"			//v4.59A40

#define	BL_TEMPERATURE_DISPLAY			"Temperature OSRAM"		//v3.74T41

#define BL_BIN_FRAME_BARCODE		"Bin Frame Barcode"
#define BL_BIN_FRAME2_BARCODE		"Bin Frame 2 Barcode"
#define BL_EXARM_BUF_FRAME_BARCODE	"Buffer Frame Barcode"

#define BL_CLEAR_ALL_FRAME_STATE	"Clear All Frame State"
#define BL_UNLOAD_TO_FULL			"Unload bin frame to full mgz"
#define BL_MGZN_SUMMARY_FILE		"Magazine Summary File"
#define BL_MGZ_FILE_ENABLE			"Output Summary File"
#define BL_MGZ_FILE_FULL_ENABLE		"Output Summary File When MgznFull"		//v4.15T9	//HPO by Leo Hung
#define BL_MGZ_FILE_PATH			"Summary File Path"
#define BL_CREE_BC_FILE_PATH		"Cree Bin Summary File Path"
#define BL_ENABLE_CREE_BC_FILE		"Enable Cree Bin Summary"

#define BL_EMPTY_FRAME_FILE			"Empty Frame File"

#define BL_EMPTY_FRAME_PATH			"Empty Frame Path"

#define BL_MISC_OPTION				"Misc Option"
#define BL_MIN_CLRALL_COUNT			"Min. ClearAll Bin Counter"
#define BL_CLEAR_ALL_FRAME_MODE		"Clear All Frame Mode"
#define BL_NEW_Z_CONFIG				"New Z Configuration"
#define BL_FAST_BC_SCAN				"Fast BC Scan Method"
#define BL_LOAD_EMPTY_RETRY_COUNT	"Load Empty Retry Count"
#define BL_RENESAS_CONFIG_MODE		"Renesas BL Config Mode"				//v4.59A20

#define BL_GEN_BIN_FULL_FILE		"Gen Bin Full File"

#define BL_FRAME_CHECK_POS_SUCCESS		2
#define BL_FRAME_WT_NO_BAR_CODE			2
#define BL_EMPTY_FRAME_BC_CHECK_AGAIN	10


#define BL_ERR_GRIPPER_JAM				2

#define BL_DUAL_BUFFER					"Bin Dual Buffer Table"
#define BL_U_BUFFER_CURR_BLOCK			"Current Upper Buffer Block"
#define BL_L_BUFFER_CURR_BLOCK			"Current Lower Buffer Block"
#define BL_U_BUFFER_CURR_BARCODE		"Current Upper Buffer Barcode"
#define BL_L_BUFFER_CURR_BARCODE		"Current Lower Buffer Barcode"
#define BL_DB_PRELOAD_EMPTY_ON_BUFFER	"Dual Buffer Preload Empty Frame On Buffer"
#define BL_LOWTOUP_OFFSET_Z				"LowerToUpper Offset Z"

#define BL_TRANSFER_STANDALONE_TO_LINE	"Transfer Standalone to Inline"
#define BL_TRANSFER_LINE_TO_STANDALONE1	"Transfer Inline to Standalone1"
#define BL_TRANSFER_LINE_TO_STANDALONE2	"Transfer Inline to Standalone2"
#define BL_TRANSFER_LINE_TO_STANDALONE3	"Transfer Inline to Standalone3"

#define BL_BIN_SUMMARY_PATH	 "C:\\Mapsorter\\UserData\\OutputFile\\ClearBin\\OutputSummary"
#define BL_BIN_SUMMARY_INFO_FILE_PATH_GENERAL "C:\\Mapsorter\\UserData\\OutputFile\\ClearBin\\OutputSummary\\SummaryInfo.txt"
#define BL_BIN_SUMMARY_INFO_FILE_PATH	"C:\\MapSorter\\UserData\\OutputFile\\ClearBin\\SummaryInfo.txt"
#define BL_BIN_OUTPUT_SUMMARY_PATH_IND	"OutputSummaryPath"
#define BL_BIN_BACKUP_SUMMARY_PATH_IND	"BackupSummaryPath"
#define BL_BIN_BACKUP_LOCAL_SUMMARY_PATH_IND	"BackupToLocalSummaryPath"


#define BL_JAM_UNLOAD_RETRY_LIMIT		3

//limit from Home Sensor	//for MS100
#define BL_Y_POS_LIMIT_FROM_HOME				360000	//357000
#define BL_Y_NEG_LIMIT_FROM_HOME				-14000	//-12500
#define BL_TIMEBELT_Y_POS_LIMIT_FROM_HOME		795000	//357000
#define BL_TIMEBELT_Y_NEG_LIMIT_FROM_HOME		-50000	//-12500
#define BL_Y_NEG_LIMIT_FROM_HOME				-14000	//-12500
#define BL_Z_POS_LIMIT_FROM_HOME				98000	//87000
#define BL_Z_NEG_LIMIT_FROM_HOME				-1269000	//-1263500	//-1262500	//v4.59A10


