//////////////////////////////////////////////////////////////////////////////
//	BT_Constant.h : Defines the Constant for BinTable class
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
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

enum eBT_EVENTID {NO_OF_BIN_BLOCKS = 1, MAX_DIE_QUANTITIES, BIN_BLOCKS_DIE_QUANTITIES, BIN_BLOCKS_WALK_PATH, 
FARME_REALIGN_LAST_DIE_OFFSET_X, FARME_REALIGN_LAST_DIE_OFFSET_Y, FRAME_REALIGN_METHOD, BT_CHANGE_GRADE_LIMIT};

// Default Axis name
#define BT_AXIS_X						"BinTableXAxis"
#define BT_AXIS_Y						"BinTableYAxis"
#define BT_AXIS_X2						"BinTableX2Axis"		//9INCH_MS Dual tables
#define BT_AXIS_Y2						"BinTableY2Axis"		//9INCH_MS Dual tables
#define BT_AXIS_T						"BinTableTAxis"			//MS100 Nichia

#define BT_STATIC_CONTROL_X				"ctrBinTableXSts"
#define BT_DYNAMIC_CONTROL_X			"ctrBinTableXDyn"
#define BT_STATIC_CONTROL_Y				"ctrBinTableYSts"
#define BT_DYNAMIC_CONTROL_Y			"ctrBinTableYDyn"
#define BT_SLOW_DYNAMIC_CONTROL_X		"ctrBinTableXSlowDyn"
#define BT_SLOW_DYNAMIC_CONTROL_Y		"ctrBinTableYSlowDyn"

//Search profile
#define BT_SP_INDEX_X					"spfBinTableXIndex"
#define BT_SP_HOME_X					"spfBinTableXHome"  //for Magnetic Encoder  Ver1.10
#define BT_SP_POS_LIMIT_X				"spfBinTableXPosLimit"
#define BT_SP_NEG_LIMIT_X				"spfBinTableXNegLimit"

#define BT_SP_INDEX_Y					"spfBinTableYIndex"
#define BT_SP_HOME_Y					"spfBinTableYHome"  //for Magnetic Encoder Ver1.10
#define BT_SP_POS_LIMIT_Y				"spfBinTableYPosLimit"
#define BT_SP_NEG_LIMIT_Y				"spfBinTableYNegLimit"

#define BT_SP_HOME_T					"spfBinTableTHome"
#define BT_T_HIGHT_RESOLUTION			1	//(0.06678749 * 2)
#define BT_T_RESOLUTION					2	//0.06678749   //change interpolator value of encoder from 200 to 16 //old 0.00533604  
//#define BT_T_RESOLUTION				0.00533604

//Machine TYPE
#define BT_MACHTYPE_STD					1		//MS899 STD
#define BT_MACHTYPE_DL_DLA				2		//MS899DL & MS899DLA
#define BT_MACHTYPE_DBUFFER				3		//MS100 &MS100Plus
#define BT_MACHTYPE_DTABLE				4		//9INCH_MS

// Error Code
#define Err_BinTableXMoveHome			1
#define Err_BinTableXNotSafe			2
#define Err_BinTableXMove				3
#define Err_BinTableXCommutate			4

#define Err_BinTableYMoveHome			5
#define Err_BinTableYNotSafe			6
#define Err_BinTableYMove				7
#define Err_BinTableYCommutate			8

#define Err_BinTableXYMove				9

#define Err_BinTableXPosLimit			10
#define Err_BinTableXNegLimit			11
#define Err_BinTableYPosLimit			12
#define Err_BinTableYNegLimit			13
#define Err_BinTableOutXLimit			20
#define Err_BinTableOutYLimit			21

#define Err_BinBlockNotAssign			999


#define BT_NOT_MOVE						0xffffffff
#define BT_DEFAULT_TRAVEL_TIME			20	//45	//v4.57A1

// BinTable Profile
#define LOW_PROF						0
#define NORMAL_PROF						1
#define FAST_PROF						2
#define LOW_PROF1						3
#define LOW_PROF2						4

// BinTable Control		//v3.67T5
#define BT_DYN_CTRL						0
#define BT_STA_CTRL						1
#define BT_SLOW_DYN_CTRL				2

// Next action for DL-A
#define BT_DLA_NO_ACTION				0

// Bin Block Re-alignment 
#define BT_SKIP_ROW						2
#define BT_SKIP_COL						2
#define BT_PR_ALL_SIDE					9
#define BT_PR_CENTER					0
#define	BT_PR_LEFT						1	//LHS
#define	BT_PR_RIGHT						5	//RHS	
#define	BT_PR_TOP						3	//UP
#define	BT_PR_BOTTOM					7	//DOWN
#define	BT_PR_TOP_LEFT					2	//UP+LHS
#define	BT_PR_BOTTOM_LEFT				8	//DOWN+LHS	
#define	BT_PR_TOP_RIGHT					4	//UP+RHS
#define	BT_PR_BOTTOM_RIGHT				6	//DOWN+RHS
#define BT_X_DIR						(1)
#define BT_Y_DIR						(-1)


// Bin Block extra file type
#define BT_EXTRA_UNKNOWN_FILE			-1
#define BT_EXTRA_RANK_FILE				0
#define BT_EXTRA_GRADE_FILE				1
#define BT_EXTRA_TYPE_FILE				2
#define BT_EXTRA_RANK_FILE_2			3
#define BT_EXTRA_GRADE_FILE_2			4

// Optimize Grade use
#define BT_OZ_BLK_NOT_ASSIGN			0
#define BT_OZ_BLK_IS_ASSIGN				1
#define BT_OZ_BLK_NOT_FULL				2
#define BT_OZ_BLK_IS_FULL				3
#define BT_OZ_GRADE_NOT_ASSIGN			0
#define BT_OZ_GRADE_MORE_BLK			1
#define BT_OZ_GRADE_LESS_BLK			2

// Bin Block Status
#define BT_BLK_NORMAL					0
#define BT_BLK_DISABLE					1
#define BT_BLK_NOT_FULL					2
#define BT_BLK_IS_FULL					3

#include "BPR_Constant.h"

#define BT_TLH_NAME						"TL-Horz"
#define BT_TLV_NAME						"TL-Vert"
#define BT_TRH_NAME						"TR-Horz"
#define BT_TRV_NAME						"TR-Vert"
#define BT_BLH_NAME						"BL-Horz"	//v4.40T7
#define BT_BLV_NAME						"BL-Vert"	//v4.40T7
#define BT_BRH_NAME						"BR-Horz"	//v4.40T7
#define BT_BRV_NAME						"BR-Vert"	//v4.40T7

//Label File Header
#define BT_LABEL_FILE_HEADER			"[MS899 Label File]"
#define BT_LABEL_FILE_HEADER_TSC		"[MS899 Label File(TSC)]"
#define BT_DYMO_LABEL_PRINTER_U			"DYMO"
#define BT_DYMO_LABEL_PRINTER			"Dymo"
#define BT_TSC_LABEL_PRINTER			"TSC"
#define BT_ZEBRA_LABEL_PRINTER			"Zebra"

#define BT_LABEL_TEXT_FORMAT			"T"
#define BT_LABEL_BARCODE_FORMAT			"B"
#define BT_LABEL_GRAPHIC_FORMAT			"G"
#define BT_LABEL_SELECT_FONT			"F"
#define BT_LABEL_SELECT_FONT2			"F2"
#define BT_LABEL_DRAW_LINE				"L"

//Bin Table min travel limit
#define BT_MIN_LIMIT					5000	
#define BT_ENCODER_TOL_T				200		//v4.59A45	

//MSD file content
#define BT_TABLE_OPTION					"BinTable"
#define BT_TABLE2_OPTION				"BinTable2"
#define BT_TABLE_MACHINE_NO				"Machine No"
#define BT_TABLE_IN_USE					"Table In Use"

#define BT_TABLE_X_POS_LIMIT			"X Positive Limit"
#define BT_TABLE_X_NEG_LIMIT			"X Negative Limit"
#define BT_TABLE_Y_POS_LIMIT			"Y Positive Limit"
#define BT_TABLE_Y_NEG_LIMIT			"Y Negative Limit"
#define BT_TABLE_UNLOAD_X_POS			"Unload X Position"
#define BT_TABLE_UNLOAD_Y_POS			"Unload Y Position"

#define BT_TABLE_X2_POS_LIMIT			"X2 Positive Limit"
#define BT_TABLE_X2_NEG_LIMIT			"X2 Negative Limit"
#define BT_TABLE_Y2_POS_LIMIT			"Y2 Positive Limit"
#define BT_TABLE_Y2_NEG_LIMIT			"Y2 Negative Limit"
#define BT_TABLE_UNLOAD_X2_POS			"Unload X2 Position"
#define BT_TABLE_UNLOAD_Y2_POS			"Unload Y2 Position"

#define BT_TABLE_COLLET_OFFSET_X1		"Collet Offset X1"
#define BT_TABLE_COLLET_OFFSET_Y1		"Collet Offset Y1"
#define BT_2CROSS_THETA_OFFSET			"Two cross theta offset(degree)"
#define BT_TWO_CROSS_DISTANCE_X			"Two cross distance X"
#define BT_TWO_CROSS_DISTANCE_Y			"Two cross distance Y"
#define BT_TABLE_COR_X					"Bin COR X"
#define BT_TABLE_COR_Y					"Bin COR Y"
#define BT_TABLE_NGPICK_POCKET_X		"NGPick Pocket X"
#define BT_TABLE_NGPICK_POCKET_Y		"NGPick Pocket Y"

#define	BT_WAFEREND_UPLOAD_MAP_MPD		"BT Wafer End Upload Map"
#define	BT_ENABLE_OUTPUT_FILE			"BT Clear Bin Output File"
#define	BT_BIN_OUTPUT_FILE_FIN			"BT Clear Bin Output File Fin"
#define	BT_ENABLE_BIN_MAP_FILE			"BT Enable Bin Map File"
#define	BT_BIN_MAP_FILE_FIN2			"BT Bin Map File Fin2"
#define BT_TABLE_REALIGN_DELAY			"Realignment Delay"
#define BT_TABLE_REALIGN_SAMEVIEW		"Realignment Use SameView"
#define BT_TABLE_SHOW_BINBLK_IMAGE		"Show BinBlk Image"
#define BT_TABLE_SPEC_VERSION			"BT Spec Version"
#define BT_TABLE_ENABLE_2DBARCODE_OUTPUT	"Enable 2D Barcode Output"	//v3.33T3
#define BT_TABLE_NUM_OF_HOLE_DIE		"Number of Hole Die"			///v3.33T3
#define BT_TABLE_ENABLE_AUTO_LOAD_RANK_FILE	"Auto Load Rank File"
#define BT_LOAD_RANK_FILE_FROM_MAP		"Load Rank File From Map"
#define BT_TABLE_AUTO_SETUP_BINBLK_WITH_TEMPL "Auto Setup Bin Blk With Templ"
#define BT_TABLE_RANK_FILE_PATH			"Rank File Path"
#define BT_RANK_ID_FILENAME				"Rank ID Filename"
#define BT_BIN_LOT_SUM_PATH				"Bin Lot Summary Path"
#define BT_BIN_LOT_SUN_FILENAME			"Bin Lot Summary Filename"
#define BT_BIN_INPUT_COUNT_SETUP_FILEPATH	"Input Count Setup File Path"
#define BT_BIN_INPUT_COUNT_SETUP_FILENAME	"Input Count Setup Filename"
#define BT_OPTIMIZE_BIN_COUNT_MODE		"Optimize Bin Grade Mode"
#define BT_OPTIMIZE_BIN_COUNT			"Enable Optimize Bin Count"
#define BT_BIN_SUMMARY_WAFER_LOT_CHECK	"Bin Summary Wafer Lot Check"
#define BT_MIN_LOT_COUNT				"Bin Min Lot Count"
#define BT_MIN_BAR_DIGIT				"Bin Min Barcode Digit"
#define BT_MIN_FRAME_DIE_COUNT			"Min Frame Die Count"
#define BT_MAX_FRAME_DIE_COUNT			"Max Frame Die Count"
#define BT_SPEEDMODE_OFFSET_X			"Speed Mode Offset X"
#define BT_SPEEDMODE_OFFSET_Y			"Speed Mode Offset Y"
#define BT_BHZ2_BOND_OFFSET_X			"BHZ2 Bond Offset X"
#define BT_BHZ2_BOND_OFFSET_Y			"BHZ2 Bond Offset Y"
#define BT_BHZ1_BOND_OFFSET_X			"BHZ1 Bond Offset X"
#define BT_BHZ1_BOND_OFFSET_Y			"BHZ1 Bond Offset Y"
#define BT_BHZ1_PREPICK_OFFSET_X		"BHZ1 PREPICK OFFSET X"
#define BT_BHZ1_PREPICK_OFFSET_Y		"BHZ1 PREPICK OFFSET Y"
#define BT_BHZ2_PREPICK_OFFSET_X		"BHZ2 PREPICK OFFSET X"
#define BT_BHZ2_PREPICK_OFFSET_Y		"BHZ2 PREPICK OFFSET Y"

#define BT_BOND_180_DEGREE_OFFSET_X_UM  "BOND 180 DEGREE OFFSET X UM"
#define BT_BOND_180_DEGREE_OFFSET_Y_UM  "BOND 180 DEGREE OFFSET Y UM"

#define BT_REALIGN_FRAME_OPTION			"Bin RealignFrame Option"
#define BT_PR_PREBOND_ALIGNMENT			"PreBond Alignment"				//v3.79
#define BT_PR_PREBOND_ALIGNPAD			"PreBond Align Pad"				//CSP
#define BT_C1_PAD_OFFSET_X				"PreBond Pad C1 Offset X"
#define BT_C1_PAD_OFFSET_Y				"PreBond Pad C1 Offset Y"
#define BT_C2_PAD_OFFSET_X				"PreBond Pad C2 Offset X"
#define BT_C2_PAD_OFFSET_Y				"PreBond Pad C2 Offset Y"
#define BT_REALIGN_1PT_COMP				"1Pt Realign Compensation"		//v3.86
#define BT_BINMAP						"Bin Map"						//v4.03
#define BT_BINMAP_CDIE_OFFSET			"Bin Map CDie Offset"			//v4.42
#define BT_RANKID_FILE					"Rank ID File"					//v4.21T4
#define BT_BINMAP_CIRCLE_RADIUS			"Bin Map Circle Radius"			//v4.36
#define BT_BINMAP_EDGE_SIZE				"Bin Map Edge Size"				//v4.36T2
#define BT_BINMAP_BEDGE_SIZE			"Bin Map BEdge Size"			//v4.36T2
#define BT_BINMAP_LEDGE_SIZE			"Bin Map LEdge Size"			//v4.36T2
#define BT_BINMAP_REDGE_SIZE			"Bin Map REdge Size"			//v4.36T2
#define BT_BINMAP_FILE_PATH				"Bin Map File Path"				//v4.37T11
#define BT_BINMAP_CDIE_ROW_OFFSET		"Bin Map CDie Row Offset"
#define BT_BINMAP_CDIE_COL_OFFSET		"Bin Map CDie Col Offset"
#define BT_BINMAP_BYPASS_SP_GRADE1		"Bin Map ByPass Sp Grade 1"		//v4.48A2	//3E DL
#define BT_BINMAP_BYPASS_SP_GRADE2		"Bin Map ByPass Sp Grade 2"		//v4.48A2
#define BT_BINMAP_BYPASS_SP_GRADE3		"Bin Map ByPass Sp Grade 3"		//v4.48A2

#define BT_No_Die_COOR_X				"Bin Table No Die X Coordinate"
#define BT_No_Die_COOR_Y				"Bin Table No Die Y Coordinate"

#define BT_OSRAM_BINMAP					"Osram Bin Map"
#define BT_OSRAM_PATTERN_TYPE			"Osram Bin Map Pattern Type"
#define BT_OSRAM_TYPEA_DIE				"Osram Bin Map TypeA Die"
#define BT_OSRAM_TYPEB_DIE				"Osram Bin Map TypeB Die"
#define BT_OSRAM_TYPEA_QTY				"Osram Bin Map TypeA Qty"
#define BT_OSRAM_TYPEB_QTY				"Osram Bin Map TypeB Qty"
#define BT_OSRAM_TYPEA_STOPPOINT		"Osram Bin Map TypeA StopPoint"
#define BT_OSRAM_TYPEB_STOPPOINT		"Osram Bin Map TypeB StopPoint"
#define BT_SAVE_TEMP_FILE				"Save TempFile Only"

#define BT_BC_POS_X						"Barcode Pos X"					//v4.39T7	//Nichia MS100+
#define BT_BC_POS_Y						"Barcode Pos Y"					//v4.39T7	//Nichia MS100+

//MS100 9Inch dual-table config		//v4.17T3
#define BT2_OFFSET_X					"Table2 Offset X"
#define BT2_OFFSET_Y					"Table2 Offset Y"

// Resort Mode
#define BT_IS_RESORT_MODE				"Bin Table Resort Mode"

// Auto Clean Collet
#define BT_AUTO_CC_LIQUID_X				"Bin Table ACC Liquid Posn X"
#define BT_AUTO_CC_LIQUID_Y				"Bin Table ACC Liquid Posn Y"
#define BT_AUTO_CC_BRUSH_X				"Bin Table ACC Brush Posn X"
#define BT_AUTO_CC_BRUSH_Y				"Bin Table ACC Brush Posn Y"
#define BT_AUTO_CC_RANGE_X				"Bin Table ACC Move Range X"
#define BT_AUTO_CC_RANGE_Y				"Bin Table ACC Move Range Y"
#define BT_AUTO_CC_CYCLE_COUNT			"Bin Table ACC Cycle Count"

#define BT_AUTO_CC_MATRIX_ROW			"Bin Table ACC Matrix Row"
#define BT_AUTO_CC_MATRIX_COL			"Bin Table ACC Matrix Col"
#define BT_AUTO_CC_AREA_SIZEX			"Bin Table ACC Area SizeX"
#define BT_AUTO_CC_AREA_SIZEY			"Bin Table ACC Area SizeY"
#define BT_AUTO_CC_AREA_LIMIT			"Bin Table ACC Area Limit"
#define BT_AUTO_CC_AREA_COUNT			"Bin Table ACC Area Count"
#define BT_AUTO_CC_AREA_INDEX			"Bin Table ACC Area Index"
#define	BT_ATUO_CC_TOGGLE_BH_VACUUM		"Bin Table ACC Toggle BH Vacuum"

#define BT_AUTO_CLEAN_DIRT_MATRIX_ROW	"Bin Table Clean Dirt Matrix Row"
#define BT_AUTO_CLEAN_DIRT_MATRIX_COL	"Bin Table Clean Dirt Matrix COL"
#define BT_AUTO_CLEAN_DIRT_UP_LEFT_X	"Bin Table Clean Dirt UpLeft X"
#define BT_AUTO_CLEAN_DIRT_UP_LEFT_Y	"Bin Table Clean Dirt UpLeft Y"
#define BT_AUTO_CLEAN_DIRT_LOWER_RIGHT_X "Bin Table Clean Dirt LowerRight X"
#define BT_AUTO_CLEAN_DIRT_LOWER_RIGHT_Y "Bin Table Clean Dirt LowerRight Y"

#define BT_MANUAL_CC_GBIN_X				"Bin Table MCC Garbage Bin X"
#define BT_MANUAL_CC_GBIN_Y				"Bin Table MCC Garbage Bin Y"
//shiraishi02
#define BT_CHK_FRAME_ORIENT				"Bin Frame Orient Check"
#define BT_ORIENT_X						"Bin Orient X"
#define BT_ORIENT_Y						"Bin Orient Y"

//v4.50A5
#define BT_AUTO_CHG_COLLET				"BT Auto Change Collet"
#define AGC_COLLETS_PER_TRAY			8	//4	//8		//v4.52A16	//v4.56A12
#define AGC_COLLETS_TRAY_TOTAL			16	//8	//8		//v4.53A13	//v4.56A12
#define AGC_USE_TRAY_2					"AGC Use Tray 2"
#define AGC_CLAMP_POS_X					"AGC Clamp Pos X"
#define AGC_CLAMP_POS_Y					"AGC Clamp Pos Y"
#define AGC_HOLDER_INSTALL_POS_X		"AGC Holder Install Pos X"
#define AGC_HOLDER_INSTALL_POS_Y		"AGC Holder Install Pos Y"
#define AGC_HOLDER_UPLOAD_POS_X			"AGC Holder Upload Pos X"
#define AGC_HOLDER_UPLOAD_POS_Y			"AGC Holder Upload Pos Y"
#define AGC_PUSHER3_POS_X				"AGC Pusher3 Pos X"
#define AGC_PUSHER3_POS_Y				"AGC Pusher3 Pos Y"
#define AGC_UPLOOK_POS_X				"AGC UpLook Pos X"
#define AGC_UPLOOK_POS_Y				"AGC UpLook Pos Y"
#define AGC_UPLOOK_UPLOAD_POS_X			"AGC UpLook Upload Pos X"
#define AGC_UPLOOK_UPLOAD_POS_Y			"AGC UpLook Upload Pos Y"

#define BT_OUTPUT_FILE_PATH2			"Output File Path2"
#define BT_OUTPUT_FILE_PATH3			"Output File Path3"
#define BT_OUTPUT_FILE_PATH4			"Output File Path4"
#define BT_OUTPUTFILE_OPTION			"Output File Option"
#define BT_OUTPUT_FILE_PATH				"Output File Path"
#define BT_OUTPUT_FORMAT_NAME			"Output Format Name"
#define BT_OUTPUT_BLUETAPE_NO			"Output Blue Tape No" // v4.51D10 Dicon 
#define BT_LABEL_PRINTER_SEL			"Label Printer Selection"
#define BT_OUTPUT_FILENAME_AS_WAFERID	"Use Filename as WaferID"
#define BT_OUTPUT_USE_BARCODE_NAME		"Use Barcode as Output File Name"
#define BT_OUTPUT_LABEL_FILEPATH		"Label File Path"
#define BT_OUTPUT_PRINT_LABEL			"Print Label"
#define BT_OUTPUT_OFFLINE_PRINT_LABEL	"Offline Print Label"		//v2.70
#define BT_AUTO_CLEAR_BIN				"Auto Clear Bin"			//v2.93T2
//Nichia//v4.43T7
#define BT_NICHIA_ENABLE_OUTPUTFILE		"Enable Nichia OutputFile"
#define BT_NICHIA_ENABLE_OTRIGGERFILE	"Enable Nichia OTriggerFile"
#define BT_NICHIA_ENABLE_RPTFILE		"Enable Nichia RPTFile"

#define BT_WAFEREND_FILE_ENABLE			"Enable WaferEnd File"
#define BT_WAFEREND_FILE_PATH			"WaferEnd File Path"
#define BT_WAFEREND_FORMAT_NAME			"WaferEnd Format Name"
#define BT_WAFEREND_YIELD				"WaferEnd Yield"

#define BT_WAFER_END_SUMMARY_PATH		"Wafer End Summary File Path"
#define BT_OUTPUT_FILE_SUMMARY_PATH		"Output File Summary Path"
#define BT_UNIQUE_SERIAL_NUM			"Use Unique Serial Num"
#define BT_MULTIPLE_SERIAL_COUNTER		"Use Multiple Serial Counter"

#define BT_BIN_SUMMARY_FILE				"Bin Summary File"
#define BT_GEN_SUMMARY_PERIOD_NUM		"Bin Summary Period Num"
#define BT_LAST_GEN_SUMMARY_TIME		"Last Gen Summary Time"
#define BT_BIN_SUMMARY_OUTPUT_PATH		"Bin Summary Output Path"

#define BT_GRADECHANGE_OPTION			"Grade Change Option"
#define BT_GRADE_COUNT					"Change Count"

#define BT_PKG_DISPLAY					"PKG Display"
#define BT_PKG_FILENAME					"Package Filename"
#define BT_PKG_DIEPITCH_X_DISPLAY		"Die Pitch X Display"
#define BT_PKG_DIEPITCH_Y_DISPLAY		"Die Pitch Y Display"
#define BT_PKG_INPUT_COUNT_DISPLAY		"Input Count Display"
#define BT_PKG_DIE_PER_ROW_DISPLAY		"Die Per Row Display"
#define BT_PKG_DIE_PER_COL_DISPLAY		"Die Per Col Display"
#define BT_PKG_DIE_CAPACITY_DISPLAY		"Capacity Display"
#define BT_PKG_BIN_BLK_NO_DISPLAY		"Bin Block No Display"

#define BT_NOT_YET_GENERATE_WAFER_END_FILE "Not Yet Generate Wafer End File"


#define BT_SNR_RESET_DAILY				1
#define BT_SNR_RESET_HIT_LIMIT			2
#define BT_SNR_RESET_CHANGE_LOTDIR		3
#define BT_SNR_RESET_LOAD_BIN_SUMMARY	4

#define BT_CLEAR_BIN_COUNT_DAILY				0
#define BT_CLEAR_BIN_COUNT_HIT_LIMIT			1
#define BT_CLEAR_BIN_COUNT_LOAD_BIN_SUMMARY		2
#define BT_CLEAR_BIN_COUNT_MONTHLY				3

#define BT_REALIGN_LASTDIE_OFFSET_X			"ReAlign LastDie Offset X"
#define BT_REALIGN_LASTDIE_OFFSET_Y			"ReAlign LastDie Offset Y"
#define BT_REALIGN_LASTDIE_REL_OFFSET_X		"ReAlign LastDie REL Offset X"
#define BT_REALIGN_LASTDIE_REL_OFFSET_Y		"ReAlign LastDie REL Offset Y"

#define BT_REALIGN_USE_FRAME_CROSS			"ReAlign T Use Cross Mark"		//v4.59A1

#define WAFER_DATA "WaferData"
#define	DIE_COUNT_BF_SCN				"Die Count Bf SCN"
#define	DIE_MIN_COUNT_BF_SCN			"Die Min Count BF Prescan"
#define	DIE_UNSELECTED_COUNT_BF_SCN		"Die Unselected count BF Prescan"

// Extra Info for clear bin & output file
#define BT_EXTRA_BIN_INFO		"Extra Bin Information"
#define BT_EXTRA_BIN_INFO_FIELD_NAME "Bin Info Field Name"
#define BT_EXTRA_BIN_INFO_ITEM		"Bin Information Item"

#define BT_SUMMARY_TIME_NO				3

#define BT_MIN_GEN_OUTPUT_FILE_DIE_TIME		180

#define FILE_EXIST_ERR					"File Exist Error"
#define FILE_EXIST_ERR_WITHOUT_RETRY	"File Exist Error Without Retry"
#define BIN_FRAME_BC_CHECK_FAILED		"Bin Frame BC Check Failed"
#define NO_EXTRA_INFO_FILE_DATA_ERR		"No Extra Info File Data Err"
#define MANUAL_INPUT_BARCODE			"Manual Input Barcode"
#define DIE_COUNT_MATCH_ERR				"Physical Die Count mathch Temp file Error"

#define BT_BACKUP_NVRAM					2
#define BT_BACKUP_NVRAM_ERROR			3

#define BT_EXTRA_BIN_INFO_ITEM_NO			8

#define BT_BATCHID_INFO_FILE				"C:\\Mapsorter\\Exe\\BatchIdFileConfig.txt"

#define BT_MAG_SUMMARY_TEMP_PATH			"C:\\MapSorter\\UserData\\Statistic\\BL_Mag_Sum_Temp.txt"	//v4.43T8

//v4.34T10
#define BT_DUAL_TABLE_PARKING_DEFAULT_POSX	-80000
#define BT_DUAL_TABLE2_PARKING_DEFAULT_POSX	-80000
#define BT_DUAL_TABLE_PARKING_DEFAULT_POSY	-550000
#define BT_DUAL_TABLE2_PARKING_DEFAULT_POSY	550000
#define BT_DUAL_TABLE_PARKING_OFFSET_X		20000
#define BT_DUAL_TABLE_PARKING_OFFSET_Y		3000

#define BT_LAYOUT_FILE_TAG				"[LayoutFile]"
#define BT_LABEL_WIDTH_TAG				"[LabelWidth]"
#define BT_LABEL_HEIGHT_TAG				"[LabelHeight]"
#define BT_LABEL_GAP_TAG				"[LabelGap]"
#define BT_LABEL_DARKNESS				"[Darkness]"
#define BT_LABEL_FILE_CONTENT			"[Content]"
#define BT_LABEL_FILE_EXT_LBE			"lbe"
#define BT_LABEL_FILE_EXT_LBEX			"lbex"
#define BT_WAFER_LABEL_LAYOUT_FILE		"WaferLabelLayout.xml"
#define BT_OUTPUT_REEL_LABEL_LAYOUT_FILE	"OutputLabelLayout.xml"

#define BT_MAG_SUM_YEAR					"Mag Sum Year"
#define BT_MAG_SUM_MONTH				"Mag Sum Month"
#define BT_MAG_SUM_DAY					"Mag Sum Day"

#define BT_DATA							"Bin Table Data"

#define BT_ERROR_MAP_FCN				"Error Map"				//andrewng //2020-0806
