#pragma once

#define MS896A_FUNC_WAFERMAP_ADAPT_ALGORITHM	_T("Map Adaptive Algorithm")
#define	MS896A_FUNC_WAFERMAP_REALIGN			_T("Save Map Realign Prescan")
#define MS896A_FUNC_PRESCAN_INTERFACE			_T("Prescan Interface")

#define	REG_PRESCAN_WAFER_OVERLAP			_T("Prescan Wafer Over Lap")
#define	REG_PRESCAN_WAFER_EDGE_NUM			_T("Prescan Wafer Edge Num")
#define	REG_PRESCAN_MAP_EDGE_NUM_LEFT		_T("Prescan Map Edge Num Left")
#define	REG_PRESCAN_MAP_EDGE_NUM_RIGHT		_T("Prescan Map Edge Num Right")
#define	REG_PRESCAN_MAP_EDGE_NUM_TOP		_T("Prescan Map Edge Num Top")
#define	REG_PRESCAN_MAP_EDGE_NUM_BOTTOM		_T("Prescan Map Edge Num Bottom")

#define	SCAN_DUMMY_MAP_ROW_DIRECTION		_T("Prescan Dummy Map Row Direction")
#define	SCAN_DUMMY_MAP_COL_DIRECTION		_T("Prescan Dummy Map Col Direction")
#define	SCAN_SET_DUMMYMA3_HOMEDIE			_T("Prescan Set Dummy Ma3 Home Die")

enum	E_MS_PRESCAN_MODE
{
	SCAN_METHOD_MAP_WHOLE_WAFER		= 0,	CP_SCAN_CONT_PROBE_WAFER	= 0,
	SCAN_METHOD_MAP_REGION_WAFER	= 1,	CP_SCAN_MAP_REGION_WAFER	= 1,
	SCAN_METHOD_DUMMY_SORT_ALL		= 2,	CP_SCAN_PROBE_EXACT_MAP		= 2,
	SCAN_METHOD_DUMMY_EXACT_MAP,
	SCAN_METHOD_DUMMY_AREA_PICKING,
	SCAN_METHOD_BAR_WAFER
};

enum	E_ES_PRESCAN_MODE
{
	ES_SCAN_MAP_WAFER	=	0,
	ES_SCAN_NOMAP_WAFER,
	ES_SCAN_BAR_WAFER,
	ES_SCAN_NOMAP_BAR_WAFER
};

#define WT_REGION_REFER_MAXLIMIT	26

#define	SCAN_SMS_DIE_MAX_LIMIT	1500

#define	SCAN_MATRIX_MIN_ROW		0
#define	SCAN_MATRIX_MIN_COL		0
#define	SCAN_MAX_MATRIX_ROW		200
#define	SCAN_MAX_MATRIX_COL		200

/*-- Prescan SRAM Constant -- */
#define PRESCAN_ENABLED						"Prescan Enabled"
#define	PRESCAN_PRESCANNING					"Prescanning"
#define	PRESCAN_END							"Prescan End"
#define PRESCAN_BLKPICK_ALIGN				"Prescan Aligning"
#define PRESCAN_FINISHED_ACTION				"Prescan Finished Action"

#define	PRESCAN_RESULT_NUM_GOOD				"Prescan Result Num Good"
#define	PRESCAN_RESULT_NUM_DEFECT			"Prescan Result Num Defect"
#define	PRESCAN_RESULT_NUM_BADCUT			"Prescan Result Num BadCut"
#define	PRESCAN_RESULT_NUM_EXTRA			"Prescan Result Num Extra"
#define	PRESCAN_RESULT_NUM_EMPTY			"Prescan Result Num Empty"

#define PRESCAN_FINISHED_DO_NOTHING			0
#define PRESCAN_FINISHED_SORT_BY_GRADE		1
#define PRESCAN_FINISHED_PICK_NG			2

enum	{	PSC_NONE = 0,	PSC_TORT,	PSC_TOTP,	PSC_TOLT,	PSC_TOBM,	PSC_REGN	};

enum	{	SCAN_ERR_FPC = 0,		SCAN_ERR_EMPTY_DIE_CHECK,	SCAN_ERR_RECT_CORNER,	SCAN_ERR_PASSSCORE,
			SCAN_ERR_KEY_DIE,		SCAN_ERR_BLOCK_REFER_CHECK,	SCAN_ERR_SP_REFER,		SCAN_ERR_NML_REFER,
			SCAN_ERR_REALIGN_FAIL,	SCAN_ERR_END	};

#define	CP_ITEM_INVALID_VALUE	-999999.9

#define	WT_PROBER_SAMPLE_CHECK_MODE			"Prober Sample Check Mode"
#define	WT_PROBER_SAMPLE_CHECK_GRADE		"Prober Sample Check Grade"
#define	WT_PROBER_SAMPLE_CHECK_ENABLE		"Prober Sample Check Enable"

#define	WT_OLD_SCAN_ACT_B4PSC_MODE			"Prescan Action Before PSC"
#define	WT_TESTER_TSF_FILE_PATH				"Prober Tester TSF File Path"
#define	WT_RESORTING_SPEC_FILE_PATH			"Prober Resorting Spec File Path"
#define	WT_TEST_CONDITION_FILE_PATH			"Prober Test Condition File Path"
#define	WT_PRB_NEAR_CHECK_FAIL_LIMIT		"Prober Near Check Fail Limit"
#define WT_STOP_UNLOAD_WAFEREND				"Prober Stop Unload when probe end"
#define	WT_PRB_NEAR_CHECK_MIN_VALUE			"Prober Near Check Min Value"
#define	WT_PRB_NEAR_CHECK_MAX_VALUE			"Prober Near Check Max Value"
#define	WT_PRB_NEAR_CHECK_TOL_VALUE			"Prober Near Check Tol Value"
#define	WT_PRB_NEAR_CHECK_ITEM_TITLE		"Prober Near Check Item Title"
#define	WT_PRB_ITEM_GRADE_MAP_TITLE			"Prober Item Grade Map Title"
#define	WT_PSC_SOURCE_MAP_FILE				"Prober Sample Check Source Map File"
#define	WT_PSC_PROBED_MAP_FILE				"Prober Sample Check Probed Map File"
#define	WT_PRB_OUTPUT_MAP_ROTATION			"Probed Output Map Rotation"
#define	WT_PRB_OUTPUT_MAP_FLIP_HORI			"Probed Output Map Flip Hori"
#define	WT_PRB_OUTPUT_MAP_FLIP_VERT			"Probed Output Map Flip Vert"
#define	WT_PRB_RECHECK_AFTER_WAFEREND		"Prober Recheck After WaferEnd"
#define	WT_PRB_ITEM_MAP_DISPLAY_MODE		"Prober Item Map Display Mode"

#define	WT_PRB_OPTIONS						"CP100 Options"

#define	WT_PRB_SP_ENABLE					"SP Enable"
#define	WT_PRB_SP_GRADE						"SP Grade"
#define	WT_PRB_SP_JUMP_ROW					"SP Jump Row"
#define	WT_PRB_SP_JUMP_COL					"SP Jump Col"
#define	WT_PRB_REMARK1_NAME					"Remark1 Name"

#define	WT_PRB_BLOCK_PROBE_ROW				"CP Block Probe Row"
#define	WT_PRB_BLOCK_PROBE_COL				"CP Block Probe Col"
#define	WT_PRB_BLOCK_PROBE_SPAN_ROW			"CP Block Probe Span Row"
#define	WT_PRB_BLOCK_PROBE_SPAN_COL			"CP Block Probe Span Col"

#define	CP_PLUS_T_LOCK_DELAY				"CP Plus Theta Lock Delay"
#define	CP_PLUS_T_UNLOCK_DELAY				"CP Plus Theta Unlock Delay"
#define	CP_PLUS_T_SETTLE_DELAY				"CP Plus Theta Settle Delay"
#define	CP_PLUS_T_LOCK_OFF					"CP Plus Theta Lock Off"

#define	WT_PRB_SEQUENCE_PROBE_PATH			"WT PRB Sequence Probe Path"
#define	WT_PRB_SEQUENCE_PROBE_LIMIT			"WT PRB Sequence Probe Limit"

#define	WT_PRB_BINMAP_ITEM_MIN				"Bin Map Item Min Value"
#define	WT_PRB_BINMAP_ITEM_MAX				"Bin Map Item Max Value"
#define	WT_PRB_BINMAP_ITEM_STEP				"Bin Map Item Step Value"

#define	WT_PRB_OUTMAP				"CP100 Output Map"

#define	WT_OM_MAP_FILENAME			"Output Map Filename"
#define WT_OM_OUTPUT_PATH			"Output Map Path"
#define	WT_OM_CSVDATA_PATH			"Csv and Data Output Path"
#define	WT_OM_DATA_PATH				"Data Output Path"
#define	WT_OM_DC_OUT_PATH			"Daily Check Output Path"
#define	WT_DC_STOP_IF_FAIL			"Daily Check Stop if fail"
#define	WT_PRB_TESTER_MODEL			"Tester Model"
#define WT_PRB_TESTER_SPEC_NAME		"Tester Spec Name"
#define WT_LOADTSF_WHEN_LOADPKG		"Load TSF when Load PKG"
#define WT_ENABLE_TESTER_LIST		"Enable Tester Spec LIST"
#define	WT_PRB_MAPDISPLAY_ITEM_FILENAMAE	"Prober MapDisplay Item file name"
#define	WT_PRB_MAP_GRADE_COLOR_FILENAME		"Prober Map Grade Color file name"
#define	WT_PRB_KEYFILES_OPERATION_MODE		"Prober Key Files Operation Mode"
#define	WT_PRB_CURR_KEYPKG_FILENAME			"Prober Current Key PKG filename"
#define	WT_PRB_KEYFILES_PATH				"Prober Key Files Path"

#define	WT_MS90_HOME_OPTIONS		"MS90 Home Die Options"
#define	WT_MS90_HOME_2ND_ROW		"MS90 2nd Home Row"
#define	WT_MS90_HOME_2ND_COL		"MS90 2nd Home Col"
#define	WT_MS90_HOME_2ND_OFFSET_ROW	"MS90 2nd Home Offset Row"
#define	WT_MS90_HOME_2ND_OFFSET_COL	"MS90 2nd Home Offset Col"

#define	WP_DATA						"CP100 Data"
#define	WP_ACPP						"Auto Clean Probe Pin"
#define	WP_TABLE_T					"ACP Table T"
#define	WP_UPLEFT_X					"Up Left X"
#define	WP_ULLEFT_Y					"Up Left Y"
#define	WP_UPRIGHT_X				"Up Right X"
#define	WP_UPRIGHT_Y				"Up Right Y"
#define	WP_LOWLEFT_X				"Low Left X"
#define	WP_LOWLEFT_Y				"LOW Left Y"
#define	WP_MATRIX_ROW				"Matrix Row"
#define	WP_MATRIX_COL				"Matrix Col"
#define	WP_CP_POSITION_X			"Clean Pin Position X"
#define	WP_CP_POSITION_Y			"Clean Pin Position Y"


#define PB_ENABLE_AUTO_SEARCH_WAFER_LIMIT_IN_DAILY_CHECK		"PB Enable Auto Search wafer Limit in Daily Check"
#define PB_ENABLE_AUTO_SEARCH_WAFER_EDGE_IN_DAILY_CHECK			"PB Enable Auto Search wafer Edge in Daily Check"
#define PB_MAP_EDGE_SIZE_IN_DAILY_CHECK							"PB Map Edge Size in Daily Check"

#define	CP_PROBE_TEMP_FILE			gszROOT_DIRECTORY + "\\Exe\\ProbeResultTemp.csv"
#define	CP_ITEM_GRADE_DATA_FILE		gszROOT_DIRECTORY + "\\Exe\\ProbeItemData.csv"
#define	CP_CONTINUE_PROBE_FILE		gszROOT_DIRECTORY + "\\Exe\\ContinueProbeResult.csv"
#define	CP_PROBE_BACKUP_FILE		gszUSER_DIRECTORY + "\\ProbeResultTemp.csv"

#define WT_ENABLE_MAP_DM_FILE				"Nichia Load Map with DM File"
#define WT_PRESCAN_DEBUG_LOG				"Prescan Debug Log Enable"
#define	WT_PRESCAN_ATCDEB_LOG				"Prescan ATC Deb Log Enable"
#define WT_PRESCAN_ENABLED					"Prescan Enabled"
#define WT_PRESCAN_FINISHEDACTION			"Prescan Finished Action"
#define	WT_PRESCAN_BLKPICK_ALIGN			"Prescan Blk Pick Align"
#define	WT_WAFEREND_UNCHECK_REFER			"Prescan WaferEnd Uncheck Refer"
#define WT_PRESCAN_PR_METHOD				"Prescan Pr method"
#define WT_SCAN_WALK_TOUR					"Prescan Walk Tour"
#define	WT_SCAN_RUNTIME_DETECT_EDGE			"Prescan RunTime Detect Edge"
#define	WT_SCAN_ADE_EMPTY_FRAME_LIMIT		"Prescan ADE Empty Frame Limit"
#define	WT_PRESCAN_MAP_USE_BACKUP_EXT		"Prescan Map Use Backup Ext"
#define	WT_PRESCAN_OEM_CUSTOMER_NAME		"Prescan AOI Oem Customer Name"
#define	WT_PRESCAN_OEM_PRODUCT_NAME			"Prescan AOI Oem Product Name"
#define	WT_PRESCAN_OEM_TEST_SYSTEM			"Prescan AOI Oem Test System"
#define	WT_PRESCAN_AOI_MA3_HOME_ROW			"Prescan AOI MA3 Home Row"
#define	WT_PRESCAN_AOI_MA3_HOME_COL			"Prescan AOI MA3 Home Col"
#define	WT_PRESCAN_AOI_MA3_RHOME_ROW		"Prescan AOI MA3 Real Home Row"
#define	WT_PRESCAN_AOI_MA3_RHOME_COL		"Prescan AOI MA3 Real Home Col"
#define	WT_RESCAN_AFTER_REALING				"Prescan Rescan after realign wafer"

#define	WT_SCAN_FPC_CHECK_NORMAL_ON_REFER	"Prescan FPC Check Normal on Refer Die"
#define	WT_ERROR_CHOOSE_GO_FPC				"Error Choose to Go FPC"
#define WT_SCAN_FPC_ALARM_LIMIT				"Scan FPC alarm limit"

#define WT_PRESCAN_MAP_SHAPE_TYPE			"Prescan Map Shape Type"

#define	WT_SCNCHECK_DIE_AS_REFDIE			"Prescan ScnCheck Die as RefDie"
#define	WT_PRESCAN_AUTO_DIEPITCH			"Prescan Auto Die Pitch"
#define	WT_PRESCAN_REGION_SIZE_ROW			"Prescan Region Size Row"
#define	WT_PRESCAN_REGION_SIZE_COL			"Prescan Region Size Col"
#define	WT_PRESCAN_DUMMY_AUTO_LIMIT			"Prescan Dummy Auto Limit"
#define	WT_PRESCAN_FIX_MAP_CENTER			"Prescan Fix Map Center"
#define	WT_BLK2_ALIGN_HMIUSE_CHAR			"Blk2 Align Hmi Use Char Die"
#define	WT_ALARM_RED_BUZZ_TWICE				"Alarm Red Twice and one Buzz"
#define	WT_PRESCAN_AUTO_MAP_LIMIT			"Prescan Auto Search Map Limit"
#define	WT_PRESCAN_GO_CHECK_LIMITS			"Prescan Go Button Check Limits"
#define	WT_AUTO_GO_PE_AROUND				"Auto Go DEB PE Around"
#define	WT_GO_CHECK_DIE_PR_IN4X				"Go Check PR die in 4X Mode"

#define	WT_REGION_SCAN_WITH_HOME_ONLY		"Enable Region Scan Align With Home Only"
#define	WT_REGION_SCAN_MANUAL_ALIGN			"Enable Region Scan Manual Align Only"
#define	WT_REGION_SCAN_THEN_SHORTPATH		"Enable Build ShortPath After scan"
#define	WT_ENABLE_6INCH_WAFERTABLE			"Enable 6 Inch Wafer Table"
#define	WT_FIND_ALL_REFER_4_SCAN			"Enable Find All Refer Die 4 Scan"
#define	WT_ALL_REFER_SPIRAL_LOOP			"Find All Refer Die Spiral Loop"
#define	WT_PRESCAN_LOG_PR_IMAGE				"Prescan Log PR defect Image"
#define	WT_GRAB_SCAN_MAP_IMAGE				"Grab Map Image Option"
#define	WT_GRAB_SAVE_MAP_IMAGE_PATH			"Grab and Save Map Image Path"
#define	WT_LOG_SCAN_PR_NG_IMAGE_PATH		"Scan Pr Ng Image Path"

#define	WT_MAP_DUMMY_SCAN_ALIGN				"Enable Map Dummy Prescan Alignment"
#define	WT_MAP_DUMMY_SCAN_CORNER_CHECK		"Enable Map Dummy Prescan Corner Check"
#define	WT_SORT_DEFECTIVE_DIE				"Enable Sort Defective Die"
#define	WT_SCAN_PICKED_DIE_RESORT			"Enable Scan Picked die Resort"
#define	WT_DEFECTIVE_DIE_SORT_GRADE			"Defective Die Sorting Grade"
#define	WT_PRESCAN_WAFER_TWICE				"Prescan Wafer Twice"
#define	WT_PRESCAN_PSM_STATE				"Prescan Psm Enable State"
#define	WT_SMALL_MAP_SORT_RPT				"Prescan Small Map Sort RPT"
#define	WT_GEN_RPT_OF_SMS					"Prescan Gen Rpt of SMS"
#define	WT_ES100_ENABLE_PSM					"Prescan ES100 Enable PSM"
#define	WT_ES100_MAP_INDEX					"Prescan ES100 by Map Index"
#define	WT_ES100_DISABLE_SCN				"Prescan ES100 Disable SCN"
#define	WT_CHECK_DIE_ORIENTATION			"Prescan Check Die Orientation"
#define	WT_PRESCAN_DUMMY_PNP_GRADE			"Prescan Dummy PNP Grade"
#define	WT_PSM_FILE_LOADED					"Check PSM file loaded"
#define	WT_PSM_FILE_EXIST					"Check PSM file Exist"
#define	WT_PSM_LOAD_ACTION					"PSM file load action"
#define	WT_PRESCAN_PSM_MAP_MC_NO			"Prescan Psm Map Machine No"
#define	WT_BLK_4REFER_CHECKSCORE			"Block Pick 4 Refer Check Score"
#define	WT_PRESCAN_INDEX_STEP_ROW			"Prescan Map Index Step Row"
#define	WT_PRESCAN_INDEX_STEP_COL			"Prescan Map Index Step Col"
#define	WT_PRESCAN_MAP_INDEX_LOOP			"Prescan Map Index Loop"

#define	WT_PITCH_CHECK_ALARM_LIMIT			"WT Pitch Check Alarm Limit"
#define	WT_PRESCAN_ROTATE_DEGREE			"Prescan Rotate Degree"
#define	WT_PRESCAN_GOOD_PASSSCORE			"Prescan Good Pass Score"
#define WT_SCAN_OFFLINE_UPLOAD_SCAN_FILE	"Prescan Offline Upload Scan File"
#define	WT_PRESCAN_CHECK_EMPTY				"Prescan Check Empty"
#define	WT_PRESCAN_CHECK_DEFECT				"Prescan Check Defect"
#define	WT_PRESCAN_CHECK_BADCUT				"Prescan Check BadCut"
#define	WT_PRESCAN_CHECK_EXTRA				"Prescan Check Extra"
#define	WT_PRESCAN_NG_PICK_BADCUT			"Prescan Ng Pick Bad Cut"
#define	WT_PRESCAN_NG_PICK_DEFECT			"Prescan Ng Pick Defect"
#define	WT_PRESCAN_NG_PICK_EDGEDIE			"Prescan Ng Pick Edge Die"

#define	WT_ACF_SAVEIMAGES				"ACF SaveImages"
#define	WT_ACF_MAP_BACKUP				"ACF Map Backup"

#define	WT_PRESCAN_NORMAL3_ATHOME			"Prescan Normal3 At Home"
#define	WT_REALIGN_PICK_MIN_DIE				"Block Realign Pick Min Die"
#define	WT_VERIFY_PITCH_MIN_COUNT			"Verify Pitch Min Die Count"
#define	WT_AUTO_RESCAN_SORTING_WAFER		"WT Auto Rescan Sorting Wafer"
#define	WT_AUTO_RESCAN_ALL_DICE				"WT Auto Rescan All Dice"
#define	WT_ADV_OFFSET_SMART_SAMPLE			"WT Adv Offset Smart Sample"
#define	WT_ADV_OFFSET_EXPIRE_TIME			"WT Adv Offset Expire Time"
#define	WT_ADV_OFFSET_UPD_MINTIME			"WT Adv Offset Upd Min Time"
#define	WT_ADV_OFFSET_MIN_SAMPLE			"WT Adv Offset Min Sample"
#define	WT_PRESCAN_DUMMY_DIAMETER			"WT Prescan Dummy Wafer Diameter"
#define WT_PRESCAN_DUMMY_QUARTER_WAFER		"WT Prescan Dummy Quarter Wafer"
#define	WT_BARWAFER_BIRDGE_LENGTH			"WT Bar Wafer Bridge Length"
#define	WT_AUTO_HOME_PATTERN_ROW			"WT auto home die pattern row"
#define	WT_AUTO_HOME_PATTERN_COL			"WT auto home die pattern col"
#define	WT_AUTO_HOME_PITCH_ROW				"WT auto home die pitch row"
#define	WT_AUTO_HOME_PITCH_COL				"WT auto home die pitch col"
#define	WT_AUTO_HOME_KEY_DIE_ROW			"WT auto home die key die row"
#define	WT_AUTO_HOME_KEY_DIE_COL			"WT auto home die key die col"

#define	WT_AUTO_SAMPLE_AFTER_PE				"WT auto sample rescan after PE"
#define	WT_AUTO_SAMPLE_AFTER_PR				"WT auto sample rescan after PR error"

#define	WT_SCAN_WITH_WAFER_LIMIT			"WT Prescan Area With Wafer Limit"
#define	WT_SCAN_DIVIDE_MS90_MAP				"WT Prescan Then Divide MS90 Map"

#define	WT_PRESCAN_AREA_CONFIG				"WT Prescan Area Config"
#define	WT_PRESCAN_AREA_UL_X				"WT Prescan Area Up Left X"
#define	WT_PRESCAN_AREA_UL_Y				"WT Prescan Area Up Left Y"
#define	WT_PRESCAN_AREA_LR_X				"WT Prescan Area Low Right X"
#define	WT_PRESCAN_AREA_LR_Y				"WT Prescan Area Low Right Y"

#define	WT_REGION_PICK_MIN_LIMIT			"Prescan Region Pick Min Limit"
#define	WT_REGION_BLKPICK_ORDER				"Prescan Region Block Pick Order"
#define	WT_REGION_REFER_ALIGNMENT			"Prescan Region Reference Align"
#define	WT_REGION_REFER_COUNT				"Prescan Region Reference Count"
#define	WT_REGION_REFER_ROW					"Prescan Region Reference Row"
#define	WT_REGION_REFER_COL					"Prescan Region Reference Col"
#define WT_REGION_REFER_PRID				"Prescan Region Reference PrID"
#define	WT_REGION_PICK_INDEX				"Prescan Region Pick Index"
#define	WT_REGION_STATE_DATA				"Prescan Region State Data"

#define	WT_PITCH_VERIFY_MAP_WAFER			"Pitch Alarm Verfify Map Wafer"
#define WT_PRESCAN_SEARCH_KEYDIE			"Prescan Move Search Key Die"
#define	WT_PRESCAN_ADAPTIVE_PREDICT			"Prescan Adaptive Predict Option"
#define	WT_DEB_GIVEUP_KEY_DIE				"Prescan DEB Give up key die"
#define	WT_DEB_SAMPLE_KEY_DIE_OLD			"Prescan DEB Sample Key Die old"
#define	WT_DEB_KEY_DIE_PICK_GRADE			"Prescan DEB Key Die Pick Grade"
#define	WT_REALIGN_PICK_MAX_DIE				"Block Realign Pick Max Die"
#define	WT_PRESCAN_PR_DELAY					"Prescan PR Delay"
#define	WT_ORIGINAL_HORI_FLIP				"Map Original Horizontal Flip"
#define	WT_ORIGINAL_VERT_FLIP				"Map Original Vertical Flip"
#define	WT_ORIGINAL_ROT_ANGLE				"Map Original Rotation Angle"
#define	WT_WAFER_MYLAR_PREDICTION			"Wafer Mylar Prediction Method"
#define	WT_RESCAN_DEB_SAMPLE_SPAN			"Rescan DEB Sample Span"
#define	WT_RESCAN_DEB_SAMPLE_START			"Rescan DEB Sample Start"

#define	WT_ADV_DEB_SAMPLE_BY_BT				"Sample Key Die when Change Bin"
#define	WT_ADV_STAGE1_START_COUNT			"Block Realign Pick Min Die"
#define	WT_ADV_STAGE1_SAMPLE_SPAN			"Block Realign Pick Max Die"
#define	WT_ADV_STAGE2_START_COUNT			"Adv Stage 2 Start Count"
#define	WT_ADV_STAGE2_SAMPLE_SPAN			"Adv Stage 2 Sample Span"
#define	WT_ADV_STAGE3_START_COUNT			"Adv Stage 3 Start Count"
#define	WT_ADV_STAGE3_SAMPLE_SPAN			"Adv Stage 3 Sample Span"

#define	WT_HMI_OPERATION_LOG_ENABLE			"Hmi Operation Log Enable"
#define	WT_HMI_OPERATION_LOG_PATH			"Hmi Operation Log Path"

#define	WT_AUTOFOCUS_GRID_SAMPLING			"Prescan Auto Focus Grid Sampling"
#define	WPR_AUTOFOCUS_DYNAMIC_ADJUST		"Prescan Auto Focus Dynamic Adjust"
#define	WPR_AUTOFOCUS_DYNAMIC_ADJUST_STEP	"Prescan Auto Focus Dynamic Adjust Step"
#define	WPR_AUTOFOCUS_DYNAMIC_FOCUS_SCORE	"Prescan Auto Focus Dynamic Focus Score"
#define	WPR_AUTOFOCUS_RESCAN_BAD_FRAME		"Prescan Auto Focus Rescan Bad Frame"
#define	WPR_AUTOFOCUS_RESCAN_FOCUS_SCORE	"Prescan Auto Focus Rescan Focus Score"

#define	WPR_PR_CHECK_REFER_DIE				"Prescan Pr Check Refer Die"

#define	WPR_PRESCAN_EXTRA_OVERLAPX			"Prescan Extra Overlap X"
#define	WPR_PRESCAN_EXTRA_OVERLAPY			"Prescan Extra Overlap Y"
#define	WPR_PRESCAN_SRH_NDIE_PASSSCORE		"PScn Pass score"
#define	WPR_AOI_SRH_OCR_PASSSCORE			"AOI OCR Pass score"
#define	WPR_SRH_NDIE_PRESCAN_FORWARDSIZE_X	"Pscn Look Forward Size X"
#define	WPR_SRH_NDIE_PRESCAN_FORWARDSIZE_Y	"Pscn Look Forward Size Y"
#define	WPR_AOI_NDIE_PRESCAN_INSPECTION		"AOI prescan inspection"
#define	WPR_PRESCAN_DIE_PR_ID				"Pscn Die Pr ID"
#define	WPR_SCAN_2ND_PR_DIE_TO_KEEP			"Prescan 2nd PR found die to Keep"
#define	WPR_SCAN_USE_19_AS_2NDPR_ID			"Prescan 2nd PR 19 as record ID"
#define	WPR_SCAN_WITH_TWO_PR_RECORDS		"AOI 2nd PR prescan search"
#define	WPR_SRH_HOME_UNIQUE_CHECK			"Check Home Die Pattern Unique"
#define	WPR_MAP_INDEX_REDUCE_ROW			"Wafer PR index Reduce Row"
#define	WPR_MAP_INDEX_REDUCE_COL			"Wafer PR index Reduce Col"
#define	WPR_FOV_FIND_WAFER_ANGLE			"Prescan Find Wafer Angle by FOV"
#define	WPR_FOV_FIND_WAFER_ANGLE_MOVE		"Prescan Find Wafer Angle and Move by FOV"
#define	WPR_AOI_RUN_PARTIAL_DIE_PR			"Prescan AOI Run Partial Die PR"
#define	WPR_PRESCAN_ALIGN_FROM_FILE			"Prescan Align Wafer from File"
#define	WPR_CHECK_MAP_HOLES_WITH_WAFER		"Prescan Check Map Holes With Wafer"

#define	WPR_SCAN_ALIGN_HOLE_AUTO_FROM_MAP	"Scan Align Wafer Hole Auto from Map"
#define	WPR_SCAN_ALIGN_TOTAL_HOLES_MIN		"Scan Align Wafer Total Holes Min"
#define	WPR_SCAN_ALIGN_PATTERN_HOLES_MIN	"Scan Align Wafer Pattern Holes Min"
#define	WPR_SCAN_ALIGN_PATTERN_HOLES_MAX	"Scan Align Wafer Pattern Holes Max"
#define	WPR_SCAN_ALIGN_MATCH_PATTERNS_PERCENT	"Scan Align Wafer Match Patterns Percent"
#define	WPR_SCAN_ALIGN_MATCH_LOW_PERCENT	"Scan Align Wafer Match Holes Percent"

#define	WPR_SCAN_ALIGN_WAFER				"Prescan Align Wafer Setting"
#define	WPR_SCAN_ALIGN_POINTS_NUM			"Prescan Align Wafer Points Num"
#define	WPR_SCAN_ALIGN_HOLE_SKIP_LIMIT		"Prescan Align Wafer Hole Skip Limit"
#define	WPR_SCAN_ALIGN_HOLE_ROW				"Prescan Align Wafer Hole Row"
#define	WPR_SCAN_ALIGN_HOLE_COL				"Prescan Align Wafer Hole Col"
#define	WPR_SCAN_ALIGN_HOLE_STA				"Prescan Align Wafer Hole Sta"
#define	WPR_CP100_HOME_OFFSET_ROW			"CP100 Home Die Offset Row"
#define	WPR_CP100_HOME_OFFSET_COL			"CP100 Home Die Offset Col"

#define	REG_SCAN_BARCODE_TIMEOUT			_T("Scan Barcode Timeout(ms)")
#define	REG_SCAN_BARCODE_RETRY_LIMIT		_T("Scan Barcode Retry Limit")

#define	WT_OUTPUT_SCAN_SUMMARY_PATH			"Prescan Output Summary Path"
#define	WT_OUPUT_SCAN_SUMMARY				"Prescan Output Summary Enable"

#define	WT_SCAN_MA1_FILEPATH				"Prescan MA1 File Path"
#define	WT_SCAN_MA1_YIELD_LIMIT				"Prescan MA1 Yield Limit"

#define	WPR_MAP_DUMMY_SCAN_RESULT_VERIFY	"Map Dummy Prescan Result Verify"

#define WPR_ES100_SRCH_THREAD_NUM			"ES100 Prescan Thread Num"
#define	WPR_SCAN_GRAB_IMAGE_LIMIT			"Prescan Grab Image Limit"

#define	WPR_DEFECT_SCAN_SETTING			"Defect Scan Setting"
#define	WPR_HAVE_GOOD_DIE_ACTION		"Have Good Die Action"
#define	WPR_HAVE_DEFECT_DIE_ACTION		"Have Defect Die Action"
#define	WPR_HAVE_EMPTY_DIE_ACTION		"Have Empty Die Action"
#define	WPR_HAVE_BAD_CUT_DIE_ACTION		"Have Bad Cut Die Action"
#define	WPR_SCAN_EXTRA_NG_PICK			"Scan Extra Die Pick NG"
#define	WPR_SCAN_EXTRA_DIE_ACTION		"Scan Extra Die Action"
#define	WPR_SCAN_EXTRA_DIE_GRADE		"Scan Extra Die Grade"
#define	WPR_SCAN_FAKE_EMPTY_ACTION		"Scan Fake Empty Action"
#define	WPR_SCAN_FAKE_EMPTY_GRADE		"Scan Fake Empty Grade"

#define	WPR_NO_ORIENTATION_DIE_MERGE	"Soraa No Orientation Die Merge"
#define	WPR_GOOD_DIE_GRADE				"Good Die Grade"
#define	WPR_DEFECT_DIE_GRADE			"Defect Die Grade"
#define	WPR_EMPTY_DIE_GRADE				"Empty Die Grade"
#define	WPR_BAD_CUT_DIE_GRADE			"Bad Cut Die Grade"

#define	WPR_SKIP_MAP_NG_GRADE_DIE		"Have Skip Map Ng Grade"
#define	WPR_MAP_NG_GRADE_VALUE			"Map Ng Grade"
#define	WPR_MAP_END_NG_GRADE_VALUE		"Map End Ng Grade"
#define	WPR_SCAN_RUN_TIME_DISPLAY		"Scan Map Run time display"

#define WPR_SKIP_MAP_AOI_NG_GRADE_DIE	"Have Skip Map AOI Ng Grade"
#define	WPR_MAP_AOI_NG_GRADE_VALUE		"Map AOI Ng Grade"
#define	WPR_MAP_END_AOI_NG_GRADE_VALUE	"Map End AOI Ng Grade"

#define	WPR_PRESCAN_MAP_EDGE_SIZE		"Prescan Map Edge Size"
#define	WPR_PRESCAN_MAP_EDGE_SIZE_Y		"Prescan Map Edge Size Y"
#define	WPR_PRESCAN_MAP_EDGE_GRADE		"Prescan Map Edge Grade"
#define	WPR_PRESCAN_MAP_EDGE_GOODSCORE	"Prescan Map Edge Good Score"
#define	WPR_PROBE_EDGE_TO_OUTPUT_MAP	"Probe Edge Die to output map"
#define	WPR_SUMMARY_ONLY_NEW_GRADE		"Prescan PR Summary only New Grade"

#define	WPR_IM_PRESCAN_REFER_DIE		"AOI Prescan Refer Die"
#define	WPR_IM_FASTHOME_SCAN_DIE		"AOI FastHome Scan Die"

#define	WPR_DEFECT_DIE_TO_NULL_BIN		"Defect Die To Null Bin"
#define	WPR_EMPTY_DIE_TO_NULL_BIN		"Empty Die To Null Bin"
#define	WPR_BAD_CUT_DIE_TO_NULL_BIN		"Bad Cut Die To Null Bin"

#define	WPR_DEFECT_DIE_MARK_UNPICK		"Defect Die Mark Unpick"
#define	WPR_EMPTY_DIE_MARK_UNPICK		"Empty Die Mark Unpick"
#define	WPR_BAD_CUT_DIE_MARK_UNPICK		"Bad Cut Die Mark Unpick"
#define WPR_PRESCAN_KEEP_PSM_UNPICK		"Prescan Keep Psm Unpick Die"

#define	WPR_PRESCAN_PITCH_TOL_X			"Prescan Pitch Tolerance X"
#define	WPR_PRESCAN_PITCH_TOL_Y			"Prescan Pitch Tolerance Y"

#define	WPR_REFSCAN_MERGE_ID			"Prescan Refer Merge ID"
#define	WPR_REFSCAN_MERGE_ELEMENTS		"Prescan Refer Merge Elements"

#define	WPR_PSCAN_OPTIONS				"Prescan Optins"

#define	WPR_HOME_LOOK_AROUND_ROW		"WPR Prescan Home Look Around Row"
#define	WPR_HOME_LOOK_AROUND_COL		"WPR Prescan Home Look Around Col"

#define	WT_SCN_ALIGN_SETTINGS		"SCN align settings"
#define	WT_SA_SET_AS_REFER			"Set as refer die"
#define	WT_SA_USE_REFER				"Use refer die"
#define	WT_SA_TOTAL_COUNTS			"Total points count"
#define	WT_SA_REFER_DIE_NO			"Refer die no"
#define	WT_SA_SCAN_BONDEDWAFER		"SCN align prescan if wafer bonded"
#define WT_SCN_HMI_ROW				"SCN HMI Row"
#define WT_SCN_HMI_COL				"SCN HMI Col"
#define WT_SCN_PRESCAN_GO_TO_TYPE	"SCN Prescan Go To Type"
#define WT_SCN_REF_ROW_OFFSET		"Ref Row Offset"
#define WT_SCN_REF_COL_OFFSET		"Ref Col Offset"
#define WT_SCN_TOTAL_CHECK_DIE		"Total Check Die"

#define	WT_SA_POINT_ROW				"Row"
#define	WT_SA_POINT_COL				"Col"
#define	WT_AOI_REMOVE_MAP_HEADER	"Prescan Remove Map Header"

#define	WPR_PSCAN_SCAN_ALIGN_PASSSCORE	"Scan Align Manual Pass Score"
#define	WPR_ES101_OCR_WINDOWS			"Scan OCR Windows"
#define	WPR_SCAN_NG_GRADE_LIST			"Scan Ng Grade List"
#define	WPR_SCAN_NG_GRADE_LIST_SIZE		"Scan Ng Grade List Size"
#define WPR_SCAN_MAP_NG_PASS_SCORE		"Map Ng Grade Scan Pass Score"
#define WPR_SCAN_MAP_AOI_NG_PASS_SCORE	"Map AOI Ng Grade Scan Pass Score"
#define	WPR_SCAN_AOI_NG_GRADE_LIST			"Scan AOI Ng Grade List"
#define	WPR_SCAN_AOI_NG_GRADE_LIST_SIZE		"Scan AOI Ng Grade List Size"
#define WPR_SCAN_MAP_AOI_NG_PASS_SCORE		"Map AOI Ng Grade Scan Pass Score"
#define	WPR_SCAN_ACCURATE_RESCAN			"Rescan With Accurate PR"
#define	WPR_SCAN_AUTO_REFILL_RESCAN			"Rescan with auto refile"
#define	WPR_RESCAN_GOOD_RATIO				"Rescan good ratio"
#define	WPR_SCAN_ACCEPT_ANGLE_1SEARCH		"5M Zoom Accept Scan Angle"

#define	WPR_BARWAFER_ALIGN_WITH_OCR		"Barwafer align with OCR die"
#define	WPR_AOI_GRADE_CONVERT			"AOI Grade Convert"
#define	WPR_SCN_NG_GRADE_CONVERT		"SCN NG Grade Convert"
#define	WPR_AOI_GRADE_MAPPLING_LIMIT	"AOI Grade Mapping Limit"
#define	WPR_AOI_MAP_ORIGE_GRADE			"AOI Map Orige Grade"
#define	WPR_AOI_PR_INSPECT_GRADE		"AOI PR Inspect Grade"
#define	WPR_AOI_MAP_NEW_GRADE			"AOI Map New Grade"

#define	WPR_SHOW_NG_DIE_SUB_GRADE		"Show Scan Ng Die Sub Grade"
#define	WPR_SAVE_NG_DIE_SUB_GRADE		"Save Scan Ng Die Sub Grade"
#define	WPR_SEND_WAFERID_TO_PR			"Send Wafer ID to PR side"

#define	WPR_ES_CONTOUR_OPTIONS		"ES WL Contour Options"

#define	WPR_WL1_COAXLIGHT_LEVEL		"WL1 COAX LIGHT LEVEL"
#define	WPR_WL1_RINGLIGHT_LEVEL		"WL1 RING LIGHT LEVEL"
#define	WPR_WL1_SIDELIGHT_LEVEL		"WL1 SIDE LIGHT LEVEL"
#define	WPR_WL1_BACKLIGHT_LEVEL		"WL1 BACK LIGHT LEVEL"

#define	WPR_WL2_COAXLIGHT_LEVEL		"WL2 COAX LIGHT LEVEL"
#define	WPR_WL2_RINGLIGHT_LEVEL		"WL2 RING LIGHT LEVEL"
#define	WPR_WL2_SIDELIGHT_LEVEL		"WL2 SIDE LIGHT LEVEL"
#define	WPR_WL2_BACKLIGHT_LEVEL		"WL2 BACK LIGHT LEVEL"

// contour camera
#define	WPR_WL1CALIBRATED		"WL1 CALIBRATED"
#define	WPR_WL1CALIBXX			"WL1 CALIB XX"
#define	WPR_WL1CALIBXY			"WL1 CALIB XY"
#define	WPR_WL1CALIBYY			"WL1 CALIB YY"
#define	WPR_WL1CALIBYX			"WL1 CALIB YX"

#define	WPR_WL2CALIBRATED		"WL2 CALIBRATED"
#define	WPR_WL2CALIBXX			"WL2 CALIB XX"
#define	WPR_WL2CALIBXY			"WL2 CALIB XY"
#define	WPR_WL2CALIBYY			"WL2 CALIB YY"
#define	WPR_WL2CALIBYX			"WL2 CALIB YX"

#define	WPR_WL1WAFEROFFSETX		"WL1 WAFER OFFSET X"
#define	WPR_WL1WAFEROFFSETY		"WL1 WAFER OFFSET Y"
#define	WPR_WL2WAFEROFFSETX		"WL2 WAFER OFFSET X"
#define	WPR_WL2WAFEROFFSETY		"WL2 WAFER OFFSET Y"

#define	WPR_CONTOUR_IMAGE_PATH	"PR Contour Image Path"

#define	WT_PSCAN_OPTIONS				"Prescan Options"


#define	WT_CORNER_PATTERN_CHECK_MODE	"Wafer Corner Pattern Check Mode"
#define	WT_CORNER_PATTERN_CHECK_PR_ID	"Wafer Corner Pattern Check Pr ID"
#define	WT_CORNER_PATTERN_CHECK_ALL		"Wafer Corner Pattern Check All"
#define	WT_CORNER_PATTERN_CHECK_DIST_X	"Wafer Corner Pattern Check Dist X"
#define	WT_CORNER_PATTERN_CHECK_DIST_Y	"Wafer Corner Pattern Check Dist Y"


#define	WT_PSCAN_CHANGE_REGION_GRADE	"Prescan Change Region Grade"
#define	WT_PSCAN_MAP_ADD_TIME_STAMP		"Prescan Map Add TimeStamp"
#define	WT_PSCAN_LOG_NG_IMAGE			"Prescan Log PR NG Image"
#define	WT_TABLET_PIXEL_X				"Prescan Tablet Pixel X"
#define	WT_TABLET_PIXEL_Y				"Prescan Tablet Pixel Y"
#define	WT_TABLET_DIE_EXTRA_EDGE_Y		"Prescan Tablet Die Extra Edge Y"
#define	WT_TABLET_DIE_EXTRA_EDGE_E		"Prescan Tablet Die Extra Edge E"
#define	WT_TABLET_DIE_EXTRA_EDGE_N		"Prescan Tablet Die Extra Edge N"
#define	WT_TABLET_DIE_EXTRA_EDGE_X		"Prescan Tablet Die Extra Edge X"

#define	ES_PNG_GRID_ENABLE			"ES101 PNG Grid Enable"
#define	ES_PNG_GRID_ROW				"ES101 PNG Grid Row"
#define	ES_PNG_GRID_COL				"ES101 PNG Grid Col"
#define	ES_PNG_GRID_THICK			"ES101 PNG Grid Thick"

#define	ES_PNG_GRID_COLOUR_ON_1		"ES101 PNG Grid Colour On 1"
#define	ES_PNG_GRID_COLOUR_ON_2		"ES101 PNG Grid Colour On 2"
#define	ES_PNG_GRID_COLOUR_ON_3		"ES101 PNG Grid Colour On 3"
#define	ES_PNG_GRID_COLOUR_ON_4		"ES101 PNG Grid Colour On 4"
#define	ES_PNG_GRID_COLOUR_ON_5		"ES101 PNG Grid Colour On 5"

#define	ES_PNG_GRID_COLOUR_1		"ES101 PNG Grid Colour 1"
#define	ES_PNG_GRID_COLOUR_2		"ES101 PNG Grid Colour 2"
#define	ES_PNG_GRID_COLOUR_3		"ES101 PNG Grid Colour 3"
#define	ES_PNG_GRID_COLOUR_4		"ES101 PNG Grid Colour 4"
#define	ES_PNG_GRID_COLOUR_5		"ES101 PNG Grid Colour 5"

#define	WT_PNG_IMAGE_PIXEL_X			"Prescan PNG Image Pixel X"
#define	WT_PNG_IMAGE_PIXEL_Y			"Prescan PNG Image Pixel Y"
#define	WT_TABLET_DIM_X_INCH			"Prescan Tablet Dim Inch (X Dimension)"
#define	WT_TABLET_DIM_Y_INCH			"Prescan Tablet Dim Inch (Y Dimension)"


#define	WT_PSCAN_LF_PITCH_TOL_X			"Prescan LF Pitch Tolerance X"
#define	WT_PSCAN_LF_PITCH_TOL_Y			"Prescan LF Pitch Tolerance Y"
#define	WT_PSCAN_LF_PCHecK_LIMIT		"Prescan LF Pitch Check Limit"
#define	WT_RESCAN_PE_LIMIT				"Rescan Pitch Error Limit"
#define	WT_LF_PE_LIMIT					"LF Pitch Error Limit"	//	Look Forward, accumulated.

#define	WT_PSCAN_FAIL_PASSSCORE			"Prescan Fail Pass Score"
#define	WT_PSCAN_MAP_BASE_NUM			"Prescan Map Base Num"

#define	MS_LCL_CURRENT_MAP_FILE			gszUSER_DIRECTORY + "\\MapFile\\CurrentMap.txt"

#define	PRESCAN_RESULT_FULL_PATH		gszUSER_DIRECTORY + "\\PrescanResult\\"

#define	PRESCAN_ATCLIB_GET				"_AtcLib_Get.txt"
#define	PRESCAN_ATCLIB_UPD				"_AtcLib_Upd.txt"
#define	PRESCAN_ATCLIB_KEY				"_AtcLib_Key.txt"

#define	PRESCAN_MSLOG_KEYC				"_MsLogKeyC.txt"
#define	PRESCAN_MSLOG_INIT				"_MsLog.txt"
#define	PRESCAN_MSLOG_XY				"_MsLogXY.txt"

#define	PRESCAN_SUMMARY					gszUSER_DIRECTORY + "\\History\\" + "ScanSummary_"
#define	PRESCAN_PR_SUMMARY				"_PrSummary.txt"
#define	PRESCAN_PR_AOIBIN				"_AOIBin.txt"

#define	PRESCAN_MOVE_POSN				"_Move_Posn.txt"
#define	PRESCAN_REGION_LOG				"_RegnOrder.txt"
#define	PRESCAN_PSN_RAWDIE				"_Psn_RawDie.txt"	//	all scan found die from PR
#define	PRESCAN_PSN_SCNWFT				"_Psn_ScnWft.txt"	//	scn position by press go button with prescan
#define	PRESCAN_PSN_MAPPHY				"_Psn_MapPhy.txt"	//	map physical position.
#define	PRESCAN_PASSIN_PSN				"_Psn_PassIn.txt"	//	pass in to build map
#define	PRESCAN_GETOUT_PSN				"_Psn_GetOut.txt"	//	build physical map out result.
#define	PRESCAN_NGGRADE_TXT				"_NgGradeList.txt"	//	prescan, NG grade list file for offline program.

#define	RESCAN_RAW_DIE_FILE				"_Rsn_RawDie.txt"	//	all scan found die from PR
#define	RESCAN_PASS_IN_FILE				"_Rsn_PassIn.txt"
#define	RESCAN_REMAIN_FILE				"_Rsn_Remain.txt"
#define	RESCAN_BASS_IN_FILE				"_Rsn_BaseIn.txt"
#define	RESCAN_PMP_MAP_FILE				"_Rsn_PmpMap.txt"
#define	RESCAN_GET_OUT_FILE				"_Rsn_GetOut.txt"
#define	RESCAN_OLD_PHY_FILE				"_Rsn_OldPhy.txt"	//	old map physical
#define	RESCAN_NEW_PHY_FILE				"_Rsn_NewPhy.txt"	//	new map physical, same to scn phy
#define	RESCAN_OLD_SCN_FILE				"_Rsn_OldScn.txt"	//	old Scn Wafter Table position
#define	RESCAN_NEW_SCN_FILE				"_Rsn_NewScn.txt"	//	new Scn Wafter Table position
#define	RESCAN_OLD_MAP_FILE				"_Rsn_OldMap.txt"	//	old map file just before rescan
#define	RESCAN_NEW_MAP_FILE				"_Rsn_NewMap.txt"	//	new map file just after rescan
#define	RESCAN_TIMING_FILE				"_Rsn_Timing.txt"	//	used time just after rescan
#define	RESCAN_OFFSET_FILE				"_Rsn_Offset.txt"	//	Rescan to previous rescan position offset
#define	RESCAN_MYLAR_SHIFT_FILE			"_Rsn_MShift.txt"	//	Rescan to 1st prescan mylar shift offset

#define	PRESCAN_NGVIEW_ALL				0
#define	PRESCAN_NGVIEW_EMPTY			1
#define	PRESCAN_NGVIEW_DEFECT			2
#define	PRESCAN_NGVIEW_BADCUT			3
#define	PRESCAN_NGVIEW_EXTRA			4
#define	PRESCAN_NGVIEW_REFER			5
#define	PRESCAN_NGVIEW_DISPLAY			6

#define	WPR_GRAB_SAVE_IMG_UFO			-1
#define	WPR_GRAB_SAVE_IMG_5PC			0
#define	WPR_GRAB_SAVE_IMG_MAN			1
#define	WPR_GRAB_SAVE_IMG_PCV			2
#define	WPR_GRAB_SAVE_IMG_UGO			3
#define	WPR_GRAB_SAVE_IMG_ARP			4
#define	WPR_GRAB_SAVE_IMG_FHD			5
#define	WPR_GRAB_SAVE_IMG_SCN			6
#define	WPR_GRAB_SAVE_IMG_ALL			7

#define	WPR_GRAB_SAVE_IMG_N2RC			8
#define	WPR_GRAB_SAVE_IMG_N2RX			9
#define	WPR_GRAB_SAVE_IMG_NOUT			10
#define	WPR_GRAB_SAVE_IMG_RC2N			11
#define	WPR_GRAB_SAVE_IMG_RX2N			12
#define	WPR_GRAB_SAVE_IMG_PCM			13
#define	WPR_GSI_ADV_OFFSET_OVER			14
#define	WPR_GSI_ADV_VERIFY_FAIL			15

#define WT_MAP_SHAPE_CIRCLE				0

#define	WFT_SHOW_DIR_X					-1
#define	WFT_SHOW_DIR_Y					-1
#define	MAP_SHOW_COL_X					1
#define	MAP_SHOW_ROW_Y					-1

#define	PRESCAN_DUAL_WAY_TIP_WAFER		"Soraa Wafer Has Dual Way Tip"

#define	PRESCAN_PASSWORD_SUPER_USER		"SuperUser"

#define	SORRA_SORT_MODE_ENABLE			"Sorra Sort Mode Enable"
#define	MS90_HALF_SORT_MODE_ENABLE		"MS90 Half Sort Mode Enable"
#define	MS90_SCAN_DISPLAY_SELECT		"MS90 Half Sort Scan Display Select"
#define	MS90_SCAN_DISPLAY_WHOLE			"MS90 Half sort Scan Display Whole"

#define	CONTOUR_STEPS				5
#define	PR_CONTOUR_LEARN_SEARCH		1

#define	ES_CONTOUR_WL1_FILE			"C:\\Mapsorter\\UserData\\History\\WL1Contour.txt"
#define	ES_CONTOUR_WL2_FILE			"C:\\Mapsorter\\UserData\\History\\WL2Contour.txt"

#define	WT_MAP_NEW_REFER			"Map New Added Refer"
#define	WT_MAP_NEW_REFER_NUM		"Map New Added Refer Num"
#define	WT_MAP_NEW_REFER_ROW		"Row"
#define	WT_MAP_NEW_REFER_COL		"Column"
