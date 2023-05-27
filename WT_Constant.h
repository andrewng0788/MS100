//////////////////////////////////////////////////////////////////////////////
//	WT_Constant.h : Defines the Constant for WaferTable class
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

enum eWT_Pattern {P3x3=0, P1x3=1, P3x1=2, P4x3=3, P3x4=4, NICHIA8x3=5, NICHIA8x6=6, NICHIA6x6=7};

enum eWT_EVENTID {MAP_DIE_TYPE_CHECK = 1, MAP_DIE_TYPE_CHECK_DIE_TYPE, WAFERMAP_ALGO_PATH, WAFERMAP_ALGO_TOUR, WAFERMAP_ALGO_MODE,
WT_HOME_DIE_OPTION, WT_HOME_DIE_ROW, WT_HOME_DIE_COL, WT_PRESCAN_DELAY, WT_GRADE_SORT_ORDER, WT_ADAPTIVE_PREDICTION, WT_PRESCAN};

//enum eU2UPatternID {HEX_PATTERN = 1, TRI_PATTERN, DIAMOND_PATTERN, RECT_PATTERN };

enum eU2UPatternID {DIAMOND_PATTERN = 1, RECT_PATTERN, HEX_PATTERN, TRI_PATTERN };

enum eScanWalkTour	{WT_SCAN_WALK_HOME_HORI = 0,	WT_SCAN_WALK_HOME_VERT,	WT_SCAN_WALK_TOP_HORI,	WT_SCAN_WALK_LEFT_VERT};

enum ePLLM_WAFER_TYPE {REBEL_TILE_TYPE=0, FLASH_TILE_TYPE=1, REBEL_U2U_TYPE=2};

// Default Axis name
#define WT_AXIS_X						"WaferXAxis"
#define WT_AXIS_Y						"WaferYAxis"
#define WT_AXIS_T						"WaferTAxis"
#define WT_AXIS_X2						"WaferX2Axis"
#define WT_AXIS_Y2						"WaferY2Axis"
#define WT_AXIS_T2						"WaferT2Axis"

//WT Input Bits
#define WT_SI_EJT_ULimit				"iEJT_UL"
#define WT_SI_EJT_LLimit				"iEJT_LL"

//WT Output Bits
#define WT_SO_EJT_SLIDEUP				"obEJTSlideUp"
#define WT_SO_EJT_SLIDEDN				"obEJTSlideDn"
#define WT_SO_WAF_CLAMP					"obWaferClamp"				//MS899EL	//Walsin China	//v4.26T1
#define WT_SO_TABLE_VACUUM				"obWTVacuum"
#define WT_SO_CPPLUS_THETA_BRAKE		"obThetaBrake"	//	""

#define	WT_SO_TABLE_X_READER_SWITCH		"obSwitchBtmXEncoderReader"
#define	WT_SO_TABLE_Y_READER_SWITCH		"obSwitchTopYEncoderReader"

//WT SEARCH profiles		//v4.24
#define WT_SP_NEG_LIMIT_X				"spfWaferXNegLimit"
#define WT_SP_POS_LIMIT_X				"spfWaferXPosLimit"
#define WT_SP_NEG_LIMIT_X2				"spfWaferX2NegLimit"
#define WT_SP_POS_LIMIT_X2				"spfWaferX2PosLimit"
#define WT_SP_INDEX_X					"spfWaferXIndex"
#define WT_SP_HOME_X					"spfWaferXHome"

#define WT_SP_NEG_LIMIT_Y				"spfWaferYNegLimit"
#define WT_SP_POS_LIMIT_Y				"spfWaferYPosLimit"
#define WT_SP_NEG_LIMIT_Y2				"spfWaferY2NegLimit"
#define WT_SP_POS_LIMIT_Y2				"spfWaferY2PosLimit"
#define WT_SP_INDEX_Y					"spfWaferYIndex"
#define WT_SP_HOME_Y					"spfWaferYHome"

// Error Code
#define Err_WTableXMoveHome				1
#define Err_WTableXNotSafe				2
#define Err_WTableXMove					3
#define Err_WTableXCommutate			4

#define Err_WTableYMoveHome				5
#define Err_WTableYNotSafe				6
#define Err_WTableYMove					7
#define Err_WTableYCommutate			8

#define Err_WTableTMoveHome				9
#define Err_WTableTNotSafe				10
#define Err_WTableTMove					11
#define Err_WTableTCommutate			12
#define Err_WTableXYTMove				13
#define Err_WTableXYTMoveHome			14
#define Err_WTableXYMove				15

#define Err_WTableDiePitchFailed		20
#define Err_WTableCrossPitchFailed		21
#define Err_WTableAbsDiePitchFailed		22

#define	WT_PITCH_ERR_ABS_NEARBY			201
#define	WT_PITCH_ERR_SCN_PREDCT			202
#define	WT_PITCH_ERR_ADV_UPDATE			203
#define	WT_PITCH_ERR_ADV_SAMPLE			204
#define	WT_PITCH_ERR_DEB_SAMPLE			205
#define	WT_CLEAN_EJ_PIN_FAILED			210
#define	WT_PITCH_OVER_TOO_MUCH			211

#define Err_WTableOutOfLimit			22
#define Err_WTWaferEnd					23
#define Err_WTHitSeparationGrade		24

#define Err_WTableSearchBarCodeFailed	25
#define Err_WTBlkLongJump				26		//v3.25T16		//OptoTech

#define	Err_WTBlk_CheckRefer_Fail		27

#define WT_NOT_MOVE						0xffffffff

//WaferTable Channel Type
#define WT_SVO_MOTOR				6
#define WT_STP_MOTOR				5

// WaferTable Profile
#define LOW_PROF					0
#define NORMAL_PROF					1
#define FAST_PROF					2
#define LOW_PROF1					3
#define	CP_VLOW_PROFILE				4
#define NORMAL_PROF1				6

// WaferTable Joystick Max limit
#define WT_JOY_MAXLIMIT_POS_X		1000000
#define WT_JOY_MAXLIMIT_NEG_X		-1000000
#define WT_JOY_MAXLIMIT_POS_Y		1000000
#define WT_JOY_MAXLIMIT_NEG_Y		-1000000

#define	RAD_2_DEG					57.2958

#define MAP_MAX_ROW			1200
#define MAP_MAX_COL			1200
#define MAPMAXROW2			MAP_MAX_ROW
#define MAPMAXCOL2			MAP_MAX_COL

// SCN Data size; verify Max Limit & Max die jump
#define WT_SCN_MAX_ROW				MAP_MAX_ROW
#define WT_SCN_MAX_COL				MAP_MAX_COL
#define WT_ALN_MAXCHECK_SCN			51	// 11
#define WT_SCN_MAX_JUMP_STEP		10
#define WT_MAX_SELECT_REGION		11

// Align wafer
#define	WT_ALN_OK					1
#define	WT_ALN_ERROR				-1
#define	WT_ALN_NOTOK				0
#define	WT_ALN_IS_GOOD				1
#define	WT_ALN_IS_DEFECT			2	
#define	WT_ALN_IS_EMPTY				3
#define	WT_ALN_IS_NOGRADE			4
#define	WT_ALN_OUT_WAFLIMIT			5
#define	WT_ALN_OUT_MAPLIMIT			6
#define	WT_ALN_SEARCH_DIE_ERROR		7
#define	WT_ALN_SCN_OK				8
#define	WT_ALN_SCN_WARNING			9
#define	WT_ALN_XCHECK_ERROR			10
#define WT_ALN_BLK_OK				11
#define WT_ALN_SCN_NO_NAR_GRADE		12
#define WT_ALN_MAP_NULL_BIN_NOTOK	13
#define WT_ALN_MAP_NULL_BIN_OK		14


//Check RefDie status
#define WT_CHK_REFDIE_OK			1
#define WT_CHK_REFDIE_STOP			-1
#define WT_CHK_REFDIE_IGNORE		-2

//SearchHome die 
#define WT_MANUAL_HOMEDIE			0
#define WT_ORGMAP_HOMEDIE			1
#define WT_SCNMAP_HOMEDIE			2

//Align wafer method
#define WT_SPIRAL_SEARCH			0
#define WT_CORNER_SEARCH			1

//Polygon wafer limit
#define WT_CIRCLE_LIMIT				0
#define WT_POLYGON_LIMIT			1
#define WT_MAX_POLYGON_PT			15

//Die Shape
#define	WT_RECTANGLE_DIE			0
#define	WT_HEXAGON_DIE				1

//Wafermap header
#define WT_MAX_MAP_COL				31
#define WT_MAX_MAP_COL_ALL			181
#define WT_MAX_OUTPUT_COL			34
#define WT_MAX_OUTPUT_COL_ALL		199

#define WT_WAFER_LIMIT_TEMPLATE_NO	4

#define WT_OUTPUT_FORMAT_REF_HEADER	"[Output File Format Ref File]"


// WaferTable.smf  
/*-- Process Data --*/ 
#define	WT_PROCESS_DATA				"Wafer Process Setup"
#define	WT_UNLOAD_X					"Wafer Unload X"
#define	WT_UNLOAD_Y					"Wafer Unload Y"
#define	WT_CENTER_X					"Wafer Center X"
#define	WT_CENTER_Y					"Wafer Center Y"
#define	WT_CALIB_X					"Wafer Calib X"
#define	WT_CALIB_Y					"Wafer Calib Y"
#define	WT_CALIB_X2					"Wafer Calib X2"
#define	WT_CALIB_Y2					"Wafer Calib Y2"
#define	WT_WAFER_TYPE				"Wafer Type"
#define	WT_WAFER_SIZE				"Wafer Size"
#define WT_WORKING_ANGLE			"Wafer Working Angle"
#define	WT_INDEX_PATH				"Wafer Index Path"
#define	WT_INDEX_DIRECTION			"Wafer Index Direction"
#define	WT_EMPTY_LIMIT				"Wafer Empty Limit"
#define	WT_EDGE_LIMIT				"Wafer Edge Limit"
#define	WT_STREET_LIMIT				"Wafer Street Limit"
#define	WT_WAFER_LIMIT_TYPE			"Wafer Limit Type"
#define	WT_WAFER_POLY_LIMIT			"Wafer Polygon Limit"
#define WT_WAFER_LIMIT_TEMPLATE		"Wafer Limit Template"
#define WT_LIMIT_TEMPLATE_SEL		"Wafer Limit Template Selection"
#define WT_TEMPLATE_SETUP_COMPLETE	"Template Setup Complete"
#define WT_NGPICK_OFFSET_X			"NGPick Offset X"
#define WT_NGPICK_OFFSET_Y			"NGPick Offset Y"
#define WT_PROBE_OFFSET_X			"Probe Offset X"
#define WT_PROBE_OFFSET_Y			"Probe Offset Y"
#define WT_WAFERTABLE_DELAY			"Wafer Table Delay"
#define WT2_OFFSET_X				"WT2 Offset X"
#define WT2_OFFSET_Y				"WT2 Offset Y"

#define LEARN_PROBE_TABLE_LEVEL_BEFORE	 "Learn Probe Table Level Before"
#define LEARN_PROBE_TABLE_LEVEl_CORNERS_POINTS "Learn Probe Table Level Corner Points Before"
#define WT_PROBE_TABLE_LEVEL_LEARN_CONRER "WT Table Level Learn Corner"
#define WT_PROBE_TABLE_LEVEL_DATA	"WT Table Level Data"
#define USE_PROBE_TABLE_LEVEL_BY_REGION "Use Probe Table Level By Region"
#define SIZE_X						"Size X"
#define SIZE_Y						"Size Y"
#define PITCH_X						"Pitch X"
#define PITCH_Y						"Pitch Y"

#define WT_POLYGON_LIMIT_NO			"No of Polygon Pos"
#define	WT_X						"X"
#define	WT_Y						"Y"
#define EJR_NEW_SEQ					"New Ejector Sequence"

#define	WT_GT_NEW_POSITION			"Wafer Table New GT Position"
#define	WT_GT_NEW_POSITION_X		"Wafer Table New GT Position X"
#define	WT_GT_NEW_POSITION_Y		"Wafer Table New GT Position Y"
#define	WT_GT_NEW_POSITION_X2		"Wafer Table New GT Position X2"
#define	WT_GT_NEW_POSITION_Y2		"Wafer Table New GT Position Y2"

/*--- Options ---*/
#define	WT_OPTION					"Wafer Table Options"
#define	WT_TABLE_INUSE				"Table InUse"					//v4.24T9	//ES101
#define	WT_DIE_PITCH_CHECK			"Die Pitch Check"
#define	WT_ABS_DIE_PITCH_CHECK		"Abs Die Pitch Check"			//v2.78T2
#define	WT_COR_MOVE_PITCHTOL_X		"COR Move Pitch Tol X"
#define	WT_COR_MOVE_PITCHTOL_Y		"COR Move Pitch Tol Y"
#define	WT_COR_BACK_PITCHTOL_X		"COR Back Pitch Tol X"
#define	WT_COR_BACK_PITCHTOL_Y		"COR Back Pitch Tol Y"
#define	WT_PREDSCAN_PITCHTOL_X		"Pred Scan Pitch Tol X"
#define	WT_PREDSCAN_PITCHTOL_Y		"Pred Scan Pitch Tol X"
#define	WT_PITCH_X_TOLERANCE		"Pitch X Tolerance"
#define	WT_PITCH_Y_TOLERANCE		"Pitch Y Tolerance"
#define	WT_MAX_IN_PITCH_CHECK		"Pitch Check Limit"
#define WT_BACKUP_MAP_PATH			"Backup Map Path"
#define WT_BACKUP_MAP_EXT			"Backup Map Extension"
#define WT_UPLOAD_BARCODE_ID_PATH	"Upload BarcodeID Path"
#define WT_WAITEJUP					"wait ejector up delay"
#define WT_PLLM_WAFER_ROW			"PLLM Wafer Row"
#define WT_PLLM_WAFER_COL			"PLLM Wafer Col"

#define WT_WAIT_EJECTOR_UP			"Wait Ejector Up"

#define	WT_SCAN_BACKUP_MAP_PATH		"Prescan Backup Map Path"
#define WT_SCAN_BACKUP_MAP_ENABLE	"Prescan Backup Map Enable"
#define	WT_AOI_SCAN_MAP_FORMAT		"Prescan AOI map format"

#define WT_MAP_GRADE_DISPLAY_MODE			"Wafer Map Grade Display Mode"
#define WT_MAP_GRADE_DISPLAY_MODE_SELECT	"Wafer Map Grade Display Mode Select"

#define WT_MAP_PATH					"Wafer Map Path"
#define WT_MAP_EXT					"Wafer Map Extension"
#define WT_MAP_ROTATION				"Wafer Map Orientation"
#define	WT_AUTO_WAFER_ROTATION		"Auto Wafer Rotation"
#define WT_AUTO_BC_ROTATION			"Auto BC Rotation"
#define	WT_MAP_EDGE_SIZE			"Wafer Map Edge Size"
#define WT_MAP_HORI_FLIP			"Wafer Map Horizontal Flip"
#define WT_MAP_VERT_FLIP			"Wafer Map Vertical Flip"
#define WT_MAP_SHOW_FIRST_DIE		"Wafer Map Show 1st Die"
#define WT_MAP_SORT_MODE			"Wafer Map Sorting Mode"
#define WT_MAP_MIN_GRADE_COUNT		"Wafer Map Min. Grade Count"
#define WT_MAP_MIN_GRADE_BIN		"Wafer Map Min. Grade Bin"
#define WT_MAP_SCOLOR_GRADE			"Wafer Map Color Start Grade"
#define WT_MAP_ECOLOR_GRADE			"Wafer Map Color End Grade"
#define WT_MAP_TCOLOR_GRADE			"Wafer Map Color Target Grade"
#define WT_MAP_HEADER_MATCH_CHECK	"Wafer Map Header matching Check"
#define WT_MAP_PICK_MODE			"Wafer Map Pick Mode"
#define WT_MAP_DIE_STEP				"Wafer Map Die Step"
#define WT_MAP_TOUR					"Wafer Map Tour"
#define WT_MAP_PATH_SELECTION		"Wafer Map Path Selection"
#define WT_MAP_MULTI_WAFER			"Multi Wafer Support"
#define WT_ENABLE_GLOBAL_THETA		"Enable Global Theta"
#define WT_AUTO_SEARCH_HOME			"Auto Search Home"
#define WT_MAP_SMART_WALK			"Smart Walk"
#define WT_ENABLE_AUTO_PICKNPLACE	"Auto Pick N Place"
#define WT_MAP_SMART_1STDIE_FINDER	"Smart Walk 1stDie Finder"
#define WT_MAP_SPIRAL_SEARCH_DIR	"Spiral Search Direction"
#define WT_MAP_START_POINT			"Start Point"
#define WT_MAP_SPECIAL_REFDIE_GRADE	"Special Ref-Die Grade"
#define WT_MAP_AUTO_BY_REGION_TO_BY_GRADE_SIZE	"Auto By Region To By Grade Size"
#define WT_MAP_BLK_FUNC_ENABLE		"Blk Func Enable"
#define	WT_OSRAM_RESORT_MODE		"Osram Resort Mode"
#define WT_DISABLE_WAFERMAP_GRADE_SELECT	"Disable WaferMap Grade Select"
#define WT_USE_WAFER_MAP_CATEGORY	"Wafer Map Category"

// ES: Wafer Pricture Colour
#define WT_BOUND_DIE_COLOUR			"Normal Boundary Die Colour"// Boundary die
#define WT_NG_DIE_COLOUR_Y			"NG Die Colour"// NG die	
#define WT_NG_DIE_COLOUR_E			"NG Die E Colour"// NG die	
#define WT_NG_DIE_COLOUR_N			"NG Die N Colour"// NG die N
#define WT_NG_DIE_COLOUR_X			"NG Die X Colour"// NG die X

#define	WT_TC_OPTION				"Wafer Table TC Option"
#define	WT_TC_ENABLE				"Wafer Table TC Enable"
#define	WT_TC_CHECK_TEMP			"Wafer Table TC Check Tmp"
#define	WT_TC_CHECK_ALARM			"Wafer Table TC Check Alarm Bit"
#define	WT_TC_POWER_ON				"Wafer Table TC To Power On"
#define	WT_TC_CALIBRATED			"Wafer Table TC Calibrated"
#define	WT_TC_TUNE_CALIBRATED		"Wafer Table TC Tune Calibrated"

#define	WT_TC_DATA					"Wafer Table TC Data"
#define	WT_TC_CALIB_GRADIENT		"Wafer Table TC Calib Gradient"
#define	WT_TC_CALIB_OFFSET			"Wafer Table TC Calib Offset"
#define	WT_TC_TARGET_TEMP			"Wafer Table TC Target Temp"
#define	WT_TC_TEMP_TOLERANCE		"Wafer Table TC Temp Tolerance"
#define	WT_TC_TUNE_TEMP				"Wafer Table TC Tune Temp"
#define	WT_TC_ACTUAL_TEMP			"Wafer Table TC Actual Temp"

//v2.93T2
#define WT_ENABLE_DIRECT_CSRCH_DIE	"Direct Corner Search Die"
#define WT_CSRCH_DIE_X				"Corner Search Pos X"
#define WT_CSRCH_DIE_Y				"Corner Search Pos Y"

#define WT_ENABLE_SORTING_CHECK_POINTS "Enable Sorting Check Points"

#define	WT_DIE_CROSS_CHECK			"Die Cross Check"
#define WT_PICK_CROSS_CHECK_DIE		"Pick Cross Check Die"
#define	WT_XCHECK_TOLERANCEX		"Cross Check X Tolerance"
#define	WT_XCHECK_TOLERANCEY		"Cross Check Y Tolerance"

#define	WT_REF_CROSS_ROW			"Reference Cross Row"
#define	WT_REF_CROSS_COL			"Reference Cross Col"
#define	WT_REF_CROSS_WIDTH			"Reference Cross Width"

#define	WT_MAP_SUB_ROWS				"Map Sub Rows"
#define	WT_MAP_SUB_COLS				"Map Sub Cols"
#define	WT_MAP_SORT_BY_GRADE		"Region Sort By Grade"

#define	WT_No_Die_COOR_X				"Wafer Table No Die X Coordinate"
#define	WT_No_Die_COOR_Y				"Wafer Table No Die Y Coordinate"
#define WT_Use_LF_2nd_Alignment			"Wafer Table Use LF 2nd Alignment"
#define WT_Use_LF_2nd_Align_NoMotion	"Wafer Table Use LF 2nd Align NoMotion"

#define WT_LONGJUMP_DELAY			"LongJump Delay"
#define WT_LONGJUMP_MOTTIME			"LongJump Time"

#define	WT_MAP_IGNORE_REGION		"Map Ignore Region"
#define WT_MAP_SELECT_REGIONS		"Select Regions"
#define WT_MAP_SELECT_REGION1_ULX	"Select Region1 ULX"
#define WT_MAP_SELECT_REGION1_ULY	"Select Region1 ULY"
#define WT_MAP_SELECT_REGION1_LRX	"Select Region1 LRX"
#define WT_MAP_SELECT_REGION1_LRY	"Select Region1 LRY"
#define WT_MAP_SELECT_REGION2_ULX	"Select Region2 ULX"
#define WT_MAP_SELECT_REGION2_ULY	"Select Region2 ULY"
#define WT_MAP_SELECT_REGION2_LRX	"Select Region2 LRX"
#define WT_MAP_SELECT_REGION2_LRY	"Select Region2 LRY"
#define WT_MAP_SELECT_REGION3_ULX	"Select Region3 ULX"
#define WT_MAP_SELECT_REGION3_ULY	"Select Region3 ULY"
#define WT_MAP_SELECT_REGION3_LRX	"Select Region3 LRX"
#define WT_MAP_SELECT_REGION3_LRY	"Select Region3 LRY"
#define WT_MAP_SELECT_REGION4_ULX	"Select Region4 ULX"
#define WT_MAP_SELECT_REGION4_ULY	"Select Region4 ULY"
#define WT_MAP_SELECT_REGION4_LRX	"Select Region4 LRX"
#define WT_MAP_SELECT_REGION4_LRY	"Select Region4 LRY"
#define WT_MAP_SELECT_REGION5_ULX	"Select Region5 ULX"
#define WT_MAP_SELECT_REGION5_ULY	"Select Region5 ULY"
#define WT_MAP_SELECT_REGION5_LRX	"Select Region5 LRX"
#define WT_MAP_SELECT_REGION5_LRY	"Select Region5 LRY"

#define WT_MAP_SELECT_REGION_GRADE	"Select Region1 Grade"

#define WT_MAP_BARCODE_READMAP		"Enable Barcode load map"
#define WT_MAP_BARCODE_SEARCH_MAP	"Enable Barcode Search Map"
#define	WT_MAP_SEARCH_SUB_FOLDER	"Map Search Sub Folder"
#define	WT_MAP_SEARCH_PART_MAP		"Enable Barcode Search Part Map"
#define	WT_MAP_SEARCH_FOLDER_NAME	"Map Search Folder Name"
#define WT_MAP_ENABLE_SUFFIX		"Enable Suffix"
#define WT_MAP_LAST_LOT_NUMBER		"Last Lot Number"
#define WT_MAP_SUFFIX_NAME			"Suffix Name"
#define WT_MAP_LOCAL_LOADMAP		"Enable Local Load Map"
#define	WT_MAP_USER_LOCAL_LOAD		"Local Load Map User Mode"
#define WT_MAP_FIXNAME_READMAP		"Enable Fix name load map"
#define WT_MAP_FIXNAME_MAPNAME		"Fix map name"
#define WT_MAP_PREFIX_READMAP		"Enable Prefix name load map"
#define WT_MAP_PREFIX_CHECK			"Enable Prefix check load map"
#define WT_MAP_SUFFIX_CHECK			"Enable Suffix check load map"
#define WT_MAP_PREFIX_NAME			"Prefix name"
#define WT_MAP_MATCH_FILE_NAME		"Match File Name Option"
#define WT_MAP_AUTO_LOAD_BACKUP_MAP	"Enable Auto Load Backup Map"

#define WT_MAP_ADV_DIE_STEP			"Enable Adaptive Die Step"
#define WT_MAP_CHECK_SEP_GRADE		"Check Separation Grade"
#define WT_MAP_DIE_SHAPE			"Die Shape"

#define WT_WAFEREND_ELECTECH_3EDL	"WaferEnd for Electech 3E DL"

#define WT_MAP_CHECK_REPEAT			"Check Repeat Map"
#define WT_MAP_CURRMAP_PATH			"Current Map Path"
#define WT_MAP_DISABLE_REFDIE		"Disable Ref Die"
#define WT_CHECK_GD_ON_REFDIE		"Check GD on RefDie Pos"

#define WT_KEEP_LAST_UNLOAD_GRADE	"Keep Last Unload Grade"
#define	WT_MAP_VALID_MIN_ROW		"WT Map Valid Min Row"
#define	WT_MAP_VALID_MIN_COL		"WT Map Valid Min Col"
#define	WT_MAP_VALID_MAX_ROW		"WT Map Valid Max Row"
#define	WT_MAP_VALID_MAX_COL		"WT Map Valid Max Col"

#define WT_ENABLE_PICK_N_PLACE				"Enable Pick N Place"
#define WT_ENABLE_SMART_WALK_PICK_N_PLLACE	"Enable Smart Walk In Pick N Place"
#define WT_MULTIGRADE_SORT_TO_SINGLEBIN		"MUltiGrade Sort to SingleBin"
#define WT_TRACEABILITY_DISABLE_LOTID		"OTraceability Disable LotID"
#define	WT_ENABLE_PR_NG_TO_GRADE			"PR NG die change map grade"
#define	WT_MAP_SORT_MULTI_TO_SAME_BIN		"WT Map Sort Multi Grade to Same Bin"

#define WT_ENABLE_CHECK_LOT_NO		"Enable Check Lot No"

#define WT_BLOCK					"Block Options"
#define WT_SrchHomeGrid				"SrchHome Step"
#define WT_SrchRefGrid				"SrchRefDie Step"
#define WT_MaxJumpCtr				"MaxJump In Ctr"
#define WT_MaxJumpEdge				"MaxJump In Edge"
#define WT_NoDieGrade				"Empty Check"
#define	WT_EMPTY_GRADE_MIN_COUNT	"Empty Grade Die Check Min Count"

#define WT_MinDieForPick			"Minimum Dice"
#define WT_EnableEdge				"Edge Alignment"
#define WT_PassPercent				"PassPercent"
#define WT_EnableF					"Digit F"
#define WT_UpdateRefDieOffset		"Auto Update RefDie Offset"		//Block2
#define WT_MAP_ALIGN_COR			"Enable COR Checking"

#define WT_GRADE_COLOR				"Grade Color"

#define WT_OUTPUTFILE_OPTION		"Output File Options"
#define WT_MAPHEADER_FILE_NAME		"Map Header FileName"
#define WT_OUTPUTFORMAT_FILE_NAME	"Output Format FileName"

#define WT_UPDATE_MAP_AFTER_SYNMOVE	"Update Map After Synmove"


#define WT_LOT_HEADER_A					"[GLOBAL]"
#define WT_LOT_HEADER_A_DEFFILE			"SORTINGDEFINITION"
#define WT_LOT_HEADER_A_WAFER_NO		"NUM_OF_WAFERS"
#define WT_LOT_HEADER_B					"[WAFER]"
#define WT_LOT_WAFER_NO					"WAFER_"
#define WT_LOT_WAFER_LOADED				"Loaded"
#define WT_LOT_CVS_HEADER				"Map File Name"
#define WT_LOT_CHECK_NAME				"Enable Check Wafer Name"
#define WT_DATA							"WaferTable Data"
#define WT_WAFER_LOT_INFO				"Wafer Lot Info"
#define WT_LOT_FILE_PATH				"Lot File Path"
#define WT_LOT_FILE_NAME				"Lot File Name"
#define WT_LOT_FILE_EXT					"Lot File Ext"
#define WT_LOT_OPERATOR_ID				"Lot Input Operator ID"
#define	WT_OUT_WAFER_INFO_FILE_PATH		"Lot Out Wafer Info File Path"
#define	WT_MAP_NAME_ONLY				"Wafer Map File Name Only"
#define	WT_MAP_EXTENSION				"Wafer Map File Extension"
#define	WT_MAP_NAME_FULL_PATH			"Wafer Map File Load Full Path"

#define WT_WAFER_ID						"Wafer Id"
#define WT_USER_DEFINE_DIE_TYPE			"User Define Die Type"
#define	WT_MAP_DIE_TYPE_CHECK			"Map Die Type Check"
#define WT_PACKAGE_FILE_CHECK			"Package File Check"
#define	WT_PACKAGE_FILE_CHECK_STATUS	"Package File Check Status"
#define WT_MAP_DIE_TYPE_CHECK_ENABLE	"Enable Map Die Type Check"
#define WT_MAP_DIE_TYPE_CHECK_STATUS	"Map Die Type Check Status"

#define	WT_MAP_HEADER_CHECK				"Map Header Check Option"
#define	WT_MAP_HEADER_CHECK_WAFER_ID	"Map Header Check Wafer ID"

#define WT_ENABLE_AUTO_DIE_TYPE_CHECK		"Enable Auto Map Die Type Check"
#define WT_AUTO_CHECK_LOTID_SELECTED		"Auto Check LotID Selected"				//v3.31T1
#define WT_AUTO_CHECK_SERIAL_NUM_SELECTED	"Auto Check Serial Num Selected"
#define WT_AUTO_CHECK_PRODUCT_NUM_SELECTED	"Auto Check Product Num Selected"
#define	WT_AUTO_CHECK_MODE_SELECTED			"Auto Check Mode Selected"

#define WT_AUTO_CHECK_LOTID				"Auto Check LotID"							//v3.31T1
#define WT_AUTO_CHECK_SERIAL_NUM		"Auto Check Serial Num"
#define WT_AUTO_CHECK_PRODUCT_NUM		"Auto Check Product Num"
#define WT_AUTO_CHECK_MODE				"Auto Check Mode"

#define WT_AUTO_CHECK_LOTID_STATUS			"Auto Check LotID Status"				//v3.31T1
#define WT_AUTO_CHECK_SERIAL_NUM_STATUS		"Auto Check Serial Num Status"
#define WT_AUTO_CHECK_PRODUCT_NUM_STATUS	"Auto Check Product Num Status"
#define WT_AUTO_CHECK_MODE_STATUS			"Auto Check Mode Status"

#define WT_SORT_BIN_ITEM				"Sort Bin Item"
#define WT_CHECK_SORT_BIN_ITEM_STATUS			"Check Sort Bin Item Status"

#define WT_PRESCAN_SETTING				"Wafer Table Prescan Setting"
#define WT_RESCAN_SETTING				"Wafer Table Rescan Setting"
#define	WT_PRESCAN_MODE					"Prescan mode"
#define	WT_RESCAN_MODE					"Rescan method"
#define	WT_STAGE_1_START				"Stage 1 start"
#define	WT_STAGE_1_FREQ					"Stage 1 freq."
#define	WT_STAGE_2_START				"Stage 2 start"
#define	WT_STAGE_2_FREQ					"Stage 2 freq."
#define	WT_STAGE_3_START				"Stage 3 start"
#define	WT_STAGE_3_FREQ					"Stage 3 freq."

//v3.44	//ShowaDenko
#define WT_MULTI_COR_OFFSET			"Use Multi COR Offset"
#define WT_CALIB_COR_A1				"COR Offset A1"
#define WT_CALIB_COR_A2				"COR Offset A2"
#define WT_CALIB_COR_B1				"COR Offset B1"
#define WT_CALIB_COR_B2				"COR Offset B2"
#define WT_CALIB_ACOR_A1			"ACOR Offset A1"
#define WT_CALIB_ACOR_A2			"ACOR Offset A2"
#define WT_CALIB_ACOR_B1			"ACOR Offset B1"
#define WT_CALIB_ACOR_B2			"ACOR Offset B2"

#define WT_MES_CONNECTION				"MES Connection"
#define WT_MES_CONNECTION_ENABLE		"Enable MES Connection"
#define WT_MES_INCOMING_PATH			"MES Incoming Path"
#define WT_MES_OUTGOING_PATH			"MES Outgoging Path"
#define WT_MES_WAFER_ID_CHECK_STATUS	"MES Wafer Id Check Status"
#define WT_MES_WAFER_ID_CHECK_MSG		"MES Wafer Id Check Msg"	
#define WT_MES_TIMEOUT					"MES Timeout"

#define WT_ENABLE_2D_BARCODE			"Enable 2D Barcode"				//v3.33T1
#define WT_2D_BARCODE_CHECK_99			"2D Barcode Check Grade 99"		//v3.59

#define WT_LOAD_CURRENT_MAP_STATUS		"Load Current Map Status"
	
#define WT_SORTING_CHECK_POINT_FILENAME	"C:\\MapSorter\\UserData\\MapFile\\CheckPoints.txt"

#define WT_LOT_HEADER_A					"[GLOBAL]"
#define WT_LOT_HEADER_CSVF				"[CSVF]"

#define	WT_LOT_CSVF_LOT					"Lot No"
#define WT_LOT_LOTNO					"Lot No"

#define WT_LOT_HEADER_A_DEFFILE			"SORTINGDEFINITION"
#define WT_LOT_HEADER_A_WAFER_NO		"NUM_OF_WAFERS"

#define WT_LOT_HEADER_B					"[WAFER]"
#define WT_LOT_WAFER_NO					"WAFER_"

#define WT_LOTINFO_LOAD_MAP_OPERATORID	"Operator ID"
#define WT_LOTINFO_LOAD_MAP_DATE		"Load Map Date"
#define WT_LOTINFO_LOAD_MAP_TIME		"Load Map Time"

#define	WT_SLOW_PROFILE_TRIGGER			"Slow Profile Trigger"

#define WT_WAFERMAP_LOAD_MAP_ERROR_HEADER	"Error"
#define WT_WAFERMAP_GRADE_MAP_ERROR			"GradeMapFailed"

//	Chip Prober probing position offset PO
#define	CP_COMP_SETTINGS				"CP100 Probe Offset Settings"
#define	CP_COMP_DONE					"CP100 Probe Offset Done"
#define	CP_COMP_STATE					"CP100 Probe Offset State"
#define	CP_COMP_POINT					"CP100 Probe Offset Point "
#define	CP_COMP_SPAN_X					"CP100 Grid Span X"
#define	CP_COMP_SPAN_Y					"CP100 Grid Span Y"
#define	CP_COMP_OFFSET_X				"CP100 Grid Offset X"
#define	CP_COMP_OFFSET_Y				"CP100 Grid Offset Y"

#define	CP_COMP_POINT_STATE				"Point State"
#define	CP_COMP_POINT_SCN_X				"Point Scan X"
#define	CP_COMP_POINT_SCN_Y				"Point Scan Y"
#define	CP_COMP_POINT_OFFSET_X			"Point Offset X"
#define	CP_COMP_POINT_OFFSET_Y			"Point Offset Y"

#define	CP_DC_SETTINGS					"CP100 Grid DC Settings"
#define	CP_DC_POINT						"Grid DC Point "

#define	CP_DC_DONE						"Grid DC Done"
#define	CP_DC_STATE						"Grid DC State"
#define	CP_DC_POINT_STATE				"Point State"
#define	CP_DC_POINT_SCN_X				"Point Scan X"
#define	CP_DC_POINT_SCN_Y				"Point Scan Y"
#define	CP_DC_POINT_CLB_X				"Point Clb X"
#define	CP_DC_POINT_CLB_XY				"Point Clb XY"
#define	CP_DC_POINT_CLB_Y				"Point Clb Y"
#define	CP_DC_POINT_CLB_YX				"Point Clb YX"

#define PB_CONTACT_SNR_1				"ibPbcs1"
#define PB_CONTACT_SNR_2				"ibPbcs2"
#define PB_CONTACT_SNR_3				"ibPbcs3"
#define PB_CONTACT_SNR_4				"ibPbcs4"
#define	PB_OUTPUT_MAP_COLUMNS			gszROOT_DIRECTORY + "\\Exe\\WaferMapColumns.txt"
#define	PB_OUTPUT_KEY_CONFIG_FILE		gszROOT_DIRECTORY + "\\UserData\\KeyConfig.csv"
#define	PB_OUTPUT_KEY_CONFIGHMI_FILE	gszROOT_DIRECTORY + "\\UserData\\KeyConfigHmi.csv"
//	Chip Prober probing position offset PO




