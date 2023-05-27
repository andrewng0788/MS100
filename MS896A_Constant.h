//////////////////////////////////////////////////////////////////////////////
//	MS896A_Constant.h : Defines the Constant header
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
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

//////////////////////////////////////////////////////////////////////////////
//	State Definitions
//////////////////////////////////////////////////////////////////////////////
enum {UN_INITIALIZE_Q, IDLE_Q, SYSTEM_INITIAL_Q, PRESTART_Q, DIAGNOSTICS_Q,
	  AUTO_Q, DEMO_Q, MANUAL_Q, STOPPING_Q, ERROR_Q, DE_INITIAL_Q, CYCLE_Q, LAST_Q
	 };

enum {MS_PACKAGE_FILE_TYPE, MS_PORTABLE_PACKAGE_FILE_TYPE};

enum {MS_PKG_SAVE_ALL, MS_PKG_SAVE_PATH, MS_PKG_SAVE_DEVICE};

enum {MS_OPERATOR_ACCESS_LEVEL = 1, MS_ENGINEER_ACCESS_LEVEL, MS_ADMIN_ACCESS_LEVEL};

enum {	ALARM_OFF_ALL = 0, 
		ALARM_GREEN, 
		ALARM_YELLOW, 
		ALARM_RED, 
		ALARM_RED_NO_BUZZER, 
		ALARM_REDYELLOW_BUZZER, 
		ALARM_REDYELLOW_NO_BUZZER, 
		ALARM_OFF_LIGHT, 
		ALARM_YELLOW_BUZZER, 
		ALARM_YELLOWGREEN_BUZZER,			// 4.51D8
		ALARM_YELLOWGREEN_NO_BUZZER,
		ALARM_YELLOWGREEN_NOMATERIAL,		//v4.59A15	//Renesas MS90
		ALARM_GREEN_YELLOW_RED_ON,
		ALARM_OFF_NOMATERIAL};				//v4.59A15	//Renesas MS90

enum {ALARM_RED_EVENT = 0, ALARM_RED_BLINK_EVENT, ALARM_YELLOW_EVENT, ALARM_YELLOW_BLINK_EVENT, ALARM_REDYELLOW_EVENT, ALARM_YELLOWGREEN_EVENT}; // 4.51D8

enum {MS_TOOLINGSTAGE_CAM = 0 , MS_TOOLINGSTAGE_ISP, MS_TOOLINGSTAGE_PD};


const CString gszMODULE_STATE[]		= {_T("Un-Initialize"),
									   _T("Idle"),
									   _T("System-Init"),
									   _T("Pre-Start"),
									   _T("Diagnostics"),
									   _T("Auto-Run"),
									   _T("Demonstration"),
									   _T("Manual"),
									   _T("Stopping"),
									   _T("Error"),
									   _T("De-Initialize"),
									   _T("Cycle")
								   };

//Application Name
const CString gszMS896A					= _T("MapSorter");
const CString gszSTOP_COMMAND			= _T("Stop");

const CString gzDRIVE					= _T("c:");
const CString gszROOT_DIRECTORY			= _T("c:\\MapSorter");
const CString gszEXE_DIRECTORY			= _T("c:\\MapSorter\\Exe");			//v4.50A2
const CString gszUSER_DIRECTORY			= _T("c:\\MapSorter\\UserData");
const CString gszUSER_DIR_MAPFILE_PROBER	= _T("c:\\MapSorter\\UserData\\MapFile\\Prober");
const CString gszUSER_DIR_MAPFILE_OUTPUT	= _T("c:\\MapSorter\\UserData\\MapFile\\Prescan");
const CString gszUSER_DIR_MAPFILE_DATACSV	= _T("c:\\MapSorter\\UserData\\MapFile\\DataCsv");
const CString gszPR_RECORD_PATH				= gszROOT_DIRECTORY + _T("\\OutputFiles\\PRTemp");

#define	HMI_SECURITY_FILE							"c:\\MapSorter\\Config\\HmiSecurity.txt"
#define	HMI_SECURITY_VAR_UPPER_LOWER_LIMIT_FILE		"c:\\MapSorter\\Config\\HmiSecurityVarUpLowLimit.txt"

//hard code now
const CString gszUnpickDieInfoPath 		= "C:\\Mapsorter\\UserData\\OutputFile\\WaferEnd\\UnpickDieInfo.txt";
const CString gszPrbDCStdFilename			= _T("c:\\MapSorter\\UserData\\MapFile\\Prober\\DailyCheckStd.csv");
const CString gszOUTPUT_FILE_TEMP_PATH		= gszROOT_DIRECTORY + _T("\\OutputFiles\\Temp");
const CString gszAWET_FILENAME				= gszUSER_DIRECTORY + _T("\\WaitEngineerAlarmList.txt");

const CString gszAsmHmiColorFile		= gszROOT_DIRECTORY + "\\AsmHmi\\WaferColor.txt";
const CString gszExeColorFile			= gszROOT_DIRECTORY + "\\Exe\\DefaultWaferColor.txt";
const CString gszGreenColorFile			= gszROOT_DIRECTORY + "\\AsmHmi\\WaferColorGreen.txt";
const CString gszLocalPkgCheckListFile	= _T("C:\\MapSorter\\UserData\\PkgKeyParameters.csv");
// Hardware Configuration
const CString gszENABLE_ALL_HARDWARE	= _T("Enable All Hardware");
const CString gszDISABLE_BH_MODULE		= _T("Disable BH Module");
const CString gszDISABLE_BT_MODULE		= _T("Disable BT Module");
const CString gszDISABLE_BL_MODULE		= _T("Disable BL Module");
const CString gszDISABLE_NL_MODULE		= _T("Disable NL Module");		//andrewng //2020-0707
const CString gszDISABLE_WT_MODULE		= _T("Disable WT Module");
const CString gszDISABLE_WL_MODULE		= _T("Disable WL Module");
const CString gszDISABLE_WL_MODULE_WITH_EXP	= _T("Disable WL Module (with Expander)");

const CString gszSPEED_MODE				= _T("Speed Mode");

// Define the Registry's Sections
const CString gszPROFILE_SETTING		= _T("Settings");
const CString gszPROFILE_GROUP_ID		= _T("Group Id");
const CString gszPROFILE_AUTO_START		= _T("Auto Start");
const CString gszPROFILE_LOG_MSG		= _T("Log Message");
const CString gszPROFILE_DIR_PATH		= _T("Directory Path");
const CString gszPROFILE_HW_CONFIG		= _T("Hardware Config");
const CString gszPROFILE_EXECUTE_HMI	= _T("Execute HMI");
const CString gszPROFILE_EXECUTE_RESORT	= _T("Execute RESORT");
const CString gszLOG_PATH_SETTING		= _T("Msg Log Path");
const CString gszLOG_FILE_SETTING		= _T("Msg Log File");
const CString gszLOG_BACKUP_SETTING		= _T("Total Backup Files");
const CString gszLOG_FILE_LINE			= _T("Total Number of Lines");
const CString gszOS_IMAGE_VERSION		= _T("Image Version");
const CString gszOS_SERIAL_VERSION		= _T("Serial Version");

//const CString gszMACHINE_PATH_SETTING		= _T("Machine File Path");
//const CString gszMACHINE_FILE_SETTING		= _T("Machine File");
const CString gszDEVICE_FILE_TYPE			= _T("Device Type");
const CString gszDEVICE_FILE_SETTING		= _T("Device File");
const CString gszDEVICE_PATH_SETTING		= _T("Device File Path");
const CString gszPORTABLE_DEVICE_PATH_SETTING	= _T("Portable Device File Path");
const CString gszDEVICE_COUNT_SETTING		= _T("Device File Count");
const CString gszBINSTORAGE_FILE_SETTING	= _T("Bin Storage File");
const CString gszBINSTORAGE_PATH_SETTING	= _T("Bin Storage File Path");
const CString gszMAPFILE_PATH_SETTING		= _T("Map File Path");
const CString gszINPUTCOUNTER_SETUP_PATH	= _T("Input Counter Setup File Path");
const CString gszINPUTCOUNTER_SETUP_PATH2	= _T("Input Counter Setup File Path2");
const CString gszLextarMapProgramN			= _T("Map Program Name");
const CString gszLextarMapProgramV			= _T("Map Program Version");
const CString gszPLLM_PASSWORD_FILE_PATH	= _T("Password File Path");

const CString gszMAPFILE_EXT_SETTING		= _T("Map File Extension");
const CString gszOUTPUT_PATH_SETTING		= _T("Output File Path");
const CString gszOUTPUT_FORMAT_SETTING		= _T("Output File Format");
const CString gszWAFEREND_PATH_SETTING		= _T("WaferEnd File Path");
const CString gszWAFEREND_FORMAT_SETTING	= _T("WaferEnd File Format");
const CString gszMACHINE_NO_SETTING			= _T("Machine No");
const CString gszMACHINE_NAME_SETTING		= _T("Machine Name");
const CString gszLOT_NO_SETTING				= _T("Lot No");
const CString gszLOT_NO_PREFIX_SETTING		= _T("Lot No Prefix");
const CString gszLOT_START_TIME				= _T("Lot Start Time");
const CString gszLOT_END_TIME				= _T("Lot End Time");
const CString gszBIN_LOT_DIRECTORY_SETTING	= _T("Bin Lot Directory");
const CString gszASSO_FILE_SETTING			= _T("Asso File");
const CString gszRECIPE_TABLE_PATH_SETTING	= _T("Recipe Table File Path");

const CString gszLOADMAP_DATE_SETTING		= _T("Load Map Date");
const CString gszLOADMAP_TIME_SETTING		= _T("Load Map Time");
const CString gszMAPSTART_DATE_SETTING		= _T("Map Start Date");
const CString gszMAPSTART_TIME_SETTING		= _T("Map Start Time");
const CString gszMAPSTART_RUN_DATE_SETTING	= _T("Map Start Run Date");
const CString gszMAPSTART_RUN_TIME_SETTING	= _T("Map Start Run Time");
const CString gszMAPRUN_TIME_SETTING		= _T("Map Run Time");
const CString gszMAPSTOP_TIME_SETTING		= _T("Map Stop Time");
const CString gszMAP_SUBFOLDER_NAME			= _T("Map Sub Folder");
const CString gszMAP_USER_NAME				= _T("Map User Name");
const CString gszMAP_PRODUCT_ID				= _T("Lumileds Product ID");
const CString gszSHIFT_ID_SETTING			= _T("Shift ID");			//v3.32T3	//Avago	//v4.48A21

const CString gszBIN_WORK_NO				= _T("Bin WorkNo");
const CString gszBIN_FILENAME_PREFIX		= _T("Bin FileName Prefix");

const CString gszDIE_FAB_GRADE				= _T("Die Fab Grade");
const CString gszDIE_FAB_MES_LOT			= _T("Die Fab MES Lot");
const CString gszDIE_FAB_MES_PART			= _T("Die Fab MES Part");

const CString gszLOG_ENABLE_OPTION			= _T("Enable Machine Log");
const CString gszLOG_ENABLE_WT_OPTION		= _T("Enable WT Machine Log");
const CString gszLOG_ENABLE_PRESCAN_DEBUG	= _T("Enable Prescan Debug Log");

const CString gszLOG_ENABLE_TABLEINDEX_OPTION	= _T("Enable TableIndex Log");

const CString gszEXPANDER_STATUS			= _T("Expander Status");
const CString gszEXPANDER2_STATUS			= _T("Expander2 Status");
const CString gszEXPANDER_TYPE				= _T("Expander Type");
const CString gszWL_BC_READER_MODEL			= _T("WL Barcode Reader Model");
const CString gszLoadPkgForNewWafer			= _T("Load Pkg For New Wafer Frame");

const CString gszWprDoFPC					= _T("Do Five Points Check at WPR");

const CString gszSTART_NEW_LOT				= _T("Start New Lot");

const CString gszENABLE_WAFER_LABEL_FILE	= _T("Enable Wafer Label File"); 

const CString gszTECHNICIAN_PASSWORD	= _T("Technician Code");
const CString gszENGINEER_PASSWORD		= _T("Engineer Code");
const CString gszADMIN_PASSWORD			= _T("Administrator Code");
const CString gszPLLM_REBEL_PASSWORD	= _T("PLLM REBEL Code");
const CString gszCONFIG_EXT				= _T(".cfg");
const CString gszCONFIG_BACK_EXT		= _T(".bcf");

const CString gszHmiAccessCodeAdmin		= _T("Hmi Access Admin Code");
const CString gszHmiAccessCodeServi		= _T("Hmi Access Servi Code");
const CString gszHmiAccessCodeSuper		= _T("Hmi Access Super Code");
const CString gszHmiAccessCodeEngin		= _T("Hmi Access Engin Code");
const CString gszHmiAccessCodeTechn		= _T("Hmi Access Techn Code");

const CString gszCONFIG_LANGUAGE		= _T("Language");

const CString gszLOG_MS_REGISTER_LICENSE= _T("Register License");

const CString gszES_ROTATE_T_UNDER_CAM	= _T("ES Rotate Theta under Camera");

const CString gszBHZ1_HOME_OFFSET		= _T("BHZ1 Home Offset");
const CString gszBHZ2_HOME_OFFSET		= _T("BHZ2 Home Offset");

//2017-4-19		//Requested by Yan Hua 
const LONG  glSG_PRODUCTIVE			= 1;
const LONG  glSG_STANDBY			= 2;
const LONG  glSG_ENGINEERING		= 3;
const LONG  glSG_SCHEDULE_DOWN		= 4;
const LONG  glSG_UNSCHEDULE_DOWN	= 5;
const LONG  glSG_NONSCHEDULED		= 6;

//////////////////////////////////////////////////////////////////////////////
// Command and Actions
//////////////////////////////////////////////////////////////////////////////

// System Error
static const LONG glINVALID_STATE			= 10000;
static const LONG glINVALID_COMMAND			= 10001;
static const LONG glINVALID_ACTION			= 10002;

static const LONG glCOMMUTATE_ACTION		= 101;
static const LONG glCLEAR_SERVO_ACTION		= 102;
static const LONG glSELECT_PROFILE_ACTION	= 103;
static const LONG glJOY_STICK_ACTION		= 104;
static const LONG glSELECT_JOY_PRF_ACTION	= 105;

////////////////////////////////////////////
//	Operation Signal Definitions
////////////////////////////////////////////

static const INT LOCK_TIMEOUT				= 1;

static const INT gnSTATION_COMPLETE			= 9999;
static const INT gnSTATION_ASSIST			= 9998;
static const INT gnSTATION_INITIAL			= 9997;

// Bond Head
static const INT gnBH_DIE_PICK				= 1;
static const INT gnBH_DIE_BOND				= 2;

// Wafer Table
static const INT gnWT_TABLE_READY			= 1;
static const INT gnWT_DIE_READY				= 2;

// Password Shift Code
static const INT gnEngineerShift			= 12;	
static const INT gnAdminShift				= -31;

static const INT gnHmiAccessPwShift			= 11;
////////////////////////////////////////////
//	Machine configuration Definitions
////////////////////////////////////////////

#define PI									3.1415926535

#define		PR_SRCH_START_ANGLE			-15.0//-45.0
#define		PR_SRCH_END_ANGLE			15.0//45.0
#define		PR_SRCH_ROT_TOL				15.0//45.0

#ifdef NU_MOTION_MS60		//v4.58A3			//compatible with MS100Plus2/3
	#define		NVRAM_8K		65536		//32768		//MAX=131072 in MS60	//v4.65A3
#else
	#define		NVRAM_8K		8192
#endif

////////////////////////////////////////////
//	Motion Platform
////////////////////////////////////////////

#define SYSTEM_HIPEC						0
#define SYSTEM_HIDRIVE						1
#define SYSTEM_NUMOTION						2

#define PR_SYSTEM_BW						0
#define PR_SYSTEM_AOI						1

#define HIPEC_SAMPLE_RATE					2
#define ULTRA_BH_SAMPLE_RATE				8

////////////////////////////////////////////
//	Machine configuration Definitions
////////////////////////////////////////////

#define MS896A_S_PROFILE				1
#define MS896A_OBW_PROFILE				2
#define MS896A_ILC_PROFILE				3

#define MS896A_GEN_SWCOMM					0
#define MS896A_DIR_SWCOMM					1
#define MS896A_OPEN_SWCOMM					2
#define MS896A_NO_SWCOMM					3

#define PL_STATIC			0
#define PL_DYNAMIC			1
#define PL_DYNAMIC1			2
#define PL_SETTLING			3
#define PL_FORCE_CONTROL	4 	//for Open DAC with velocity damping
#define PL_SETTLING1		5
#define PL_ILC				6
#define PL_DYNAMIC2			7
#define PL_STATIC1			8
#define PL_STATIC2			9
#define PL_SETTLING2		10
#define	PL_MOVESEARCH		6
#define PL_OPEN_DAC			8

#define MS896A_CFG_MODEL_NO					"Machine ModelNo"
#define MS896A_CFG_MARK_FILENAME			"Mark File Name"

#define MS_KM_GROUP_NO_COB					8
#define MS_LICENSE_SW_NAME					_T("MS100");

#define MS896A_CFG_HARDWARD_CONFIG			"Hardware Configuration"
#define MS896A_CFG_HW_MOTION_PLATFORM		"Motion Platform"
#define MS896A_CFG_HW_CHECK_COVER_SNR		"Cover Sensor Check"
#define MS896A_CFG_HW_BARCODE1_COM			"BarCode 1 COM Port"		//WT
#define MS896A_CFG_HW_BARCODE2_COM			"BarCode 2 COM Port"		//BT
#define MS896A_CFG_HW_BARCODE3_COM			"BarCode 3 COM Port"		//BT2	//MS100 9Inch
#define MS896A_CFG_HW_BARCODE4_COM			"BarCode 4 COM Port"		//WT2	//ES101
#define MS896A_CFG_HW_MAIN_COVER_LOCK		"Main Cover Lock"
#define MS896A_CFG_HW_SIDE_COVER_LOCK		"Side Cover Lock"
#define MS896A_CFG_HW_BIN_ELEVATOR_LOCK		"Bin Elevator Lock"
#define MS896A_CFG_HW_BOND_HEAD_CONFIG		"Bond Head Config"
#define MS896A_CFG_HW_CONFIG_NO				"Hardware Config No"
#define MS896A_CFG_HW_BOND_HEAD_MOUNTING	"Bond Head Mounting"
#define MS896A_CFG_HW_CT_SPEED_MODE			"Speed Mode"
#define MS896A_CFG_HW_SLOW_BT_CTRL			"Slow BT Control"			//v3.67T5
#define MS896A_CFG_HW_PROTOTYPE_MACHINE		"Prototype Machine"			//prototype MS100
#define MS896A_CFG_HW_NUMOTION_SINGLE_ARM	"Nu Motion Single Arm"		//prototype MS100
#define MS896A_CFG_HW_NUMOTION_100PLUS		"MS100 Plus Mode"			//v4.01
#define MS896A_CFG_HW_BOND_HEAD_SUCKINGHEAD	"BH Sucking Head"			//v4.05
#define MS896A_CFG_HW_AL_BOND_ARM			"AL BondArm"				//AL bond Arm -- Standard arm, FM CF Arm -- changelight standard arm
#define MS896A_CFG_HW_TABLE_XY_MAGNETIC_ENCODER "Table XY Magnetic Encoder" //1.10 version
#define MS896A_CFG_HW_BIN_TABLE_T_HIGH_RESOLUTION "Bin Table T High Resolution" //1.11 version
#define MS896A_CFG_HW_NEW_AUTO_CLEAN_COLLET       "New Auto Clean Collet" //1.12 version
#define MS896A_CFG_HW_WAFERTBALE_WITH_EXPANDER	"Wafer Table With Expander" //1.15 version
#define MS896A_CFG_HW_THETA_WITH_BRAKE			"Bin Loader Theta With Brake" //1.15 version

#define MS896A_CFG_HW_MS100_9INCH_OPTION	"MS100 9Inch Option"		//v4.16T3
#define MS896A_CFG_HW_ES100_DWFT_OPTION		"ES100 Dual WaferTable"		//v4.24T4
#define MS896A_CFG_HW_ES201_SWFT_OPTION		"ES201 Single WaferTable"
#define MS896A_CFG_HW_MS100_SLOADER_OPTION	"MS100 Single Loader"		//v4.31T10	//Yearly MS100Plus
#define MS896A_CFG_HW_PROBER_OPTION			"Prober"
#define MS896A_CFG_HW_MS_AUTOLINE_MODE		"Smart Inline System"			//v4.55A7
#define MS896A_CFG_HW_CG_MAX_INDEX			"Change Collet Max Index"
#define	CP100_CONTACT_SENSORS_NUM			"Prober NO of Contact Sensor"

// Start Of Hardware Config Types (Hipec Board Type)
#define MS896A_ORGINAL_HW_CONFIG			1	// Original Config with I/O Board & HSIO 
#define MS896A_DDBS_WITH_RING_MASTER		2	// DDBS with Ring Master

// Start of Bond Head config types
#define MS896A_ULTRA_BOND_HEAD				1	// Ultra Bond Head

// End Of Hardware Config Types

#define MS896A_CFG_HW_OGRIPPER_PUSHER		"Output Gripper Pusher"		//v2.91T1

#define		WPR_PR_DISPLAY_IMAGE_LOG		"C:\\MapSorter\\UserData\\SaveImages"

#define MS810_NEW_MOUNT						1

// Bin Loader Config
#define BL_CONFIG_DL						0
#define BL_CONFIG_DLA						1
#define BL_CONFIG_DL_WITH_BUFFER			2					//MS100 & MS100Plus
#define BL_CONFIG_DUAL_DL					3					//9INCH_MS100		//v4.16T1
#define BL_CONFIG_DL_WITH_UPDN_BUFFER		4					//MS50 BL config	//v4.59A38

//	Wafer Loader Operation sequence 
#define WL_AUTO_MODE					0	
#define WL_MANUAL_MODE					1	
#define WL_SEMI_AUTO_MODE				2		//v3.42T1	



#define MS896A_CFG_CHANNEL_ATTRIB			"Channel Attribute"
#define MS896A_CFG_CH_PROFILE_TYPE			"Profile Type"
#define MS896A_CFG_ILC_MOVE_SAMPLE			"ILC Move Sample"
#define MS896A_CFG_CH_DYNA_CONTROL			"Dynamic Control"
#define MS896A_CFG_CH_STAT_CONTROL			"Static Control"
#define MS896A_CFG_CH_JOY_CONTROL			"Joystick Control"
#define MS896A_CFG_CH_VELF_GAIN				"VelFilter Gain"
#define MS896A_CFG_CH_VELF_WEIGHT			"VelFilter Weight"
#define MS896A_CFG_CH_DAC_LIMIT				"DAC Limit Current"
#define MS896A_CFG_CH_DAC_TIME				"DAC Limit Time"
#define MS896A_CFG_CH_PBIAS_POS				"Phase Bias +ve"
#define MS896A_CFG_CH_PBIAS_NEG				"Phase Bias -ve"
#define MS896A_CFG_CH_ENABLE				"Enable"
#define MS896A_CFG_CH_DISABLE				"Disable"
#define MS896A_CFG_CH_BL_CONFIG				"BL Config"
#define MS896A_CFG_CH_BL_NEW_DUAL_BUFFER	"New Dual Buffer"
#define MS896A_CFG_V11_CONFIG				"ES101 WT v11 Config"	//v4.28T4	//ES101 new WT v1.1 config
#define MS896A_CFG_CH_ENABLE_ENCODER		"Enable Encoder"
#define	MS896A_CFG_EXPANDER_VACUUM_MODE		"Expander Vacuum Mode"

#define MS896A_CFG_CH_BINLOADER_Z			"BinLoader Z"
#define MS896A_CFG_CH_BINLOADER_Y			"BinLoader Y"
#define MS896A_CFG_CH_BINLOADER_THETA		"BinLoader Theta"
#define MS896A_CFG_CH_BINLOADER_UPPER		"BinLoader Upper"
#define MS896A_CFG_CH_BINLOADER_LOWER		"BinLoader Lower"
#define MS896A_CFG_CH_BINGRIPPER			"BinGripper"
#define MS896A_CFG_CH_BINGRIPPER2			"BinGripper2"			//v4.16
#define MS896A_CFG_CH_BINEXCHGARMR			"BinExchgArm"
#define MS896A_CFG_CH_WAFLOADER_Z			"WaferLoader Z"
#define MS896A_CFG_CH_WAFGRIPPER			"WaferGripper"
#define MS896A_CFG_CH_WAFLOADER2_Z			"WaferLoader2 Z"		//v4.24T1
#define MS896A_CFG_CH_WAFGRIPPER2			"WaferGripper2"			//v4.24T1
#define MS896A_CFG_CH_WAFEXPDCMOTOR			"WaferTable Z"			//v4.01
#define MS896A_CFG_CH_WAFEXPDCMOTOR2		"WaferTable Z2"			//v4.24T1
#define MS896A_CFG_CH_BINTABLE_X			"BinTable X"
#define MS896A_CFG_CH_BINTABLE_Y			"BinTable Y"
#define MS896A_CFG_CH_BINTABLE_T			"BinTable T"			//v4.39T7
#define MS896A_CFG_CH_BINTABLE2_X			"BinTable2 X"			//v4.16
#define MS896A_CFG_CH_BINTABLE2_Y			"BinTable2 Y"			//v4.16
#define MS896A_CFG_CH_WAFTABLE_X			"WaferTable X"
#define MS896A_CFG_CH_WAFTABLE_Y			"WaferTable Y"
#define MS896A_CFG_CH_WAFTABLE_T			"WaferTable Theta"
#define MS896A_CFG_CH_WAFTABLE2_X			"WaferTable2 X"			//v4.24T1
#define MS896A_CFG_CH_WAFTABLE2_Y			"WaferTable2 Y"			//v4.24T1
#define MS896A_CFG_CH_WAFTABLE2_T			"WaferTable2 Theta"		//v4.24T1
#define MS896A_CFG_CH_WAFERPR_Z				"WaferPr Z"				//v4.04
#define MS896A_CFG_CH_BACKLIGHT_Z			"BackLight Z"			//v4.24

#define MS896A_CFG_CH_NVCLOADER_X			"NVCLoader X"
#define MS896A_CFG_CH_NVCLOADER_Y			"NVCLoader Y"
#define MS896A_CFG_CH_NVCLOADER_Z1			"NVCLoader Z1"
#define MS896A_CFG_CH_NVCLOADER_Z2			"NVCLoader Z2"

#define MS896A_CFG_CH_EJECTOR				"Ejector"				//Wafer Ejector Pin
#define MS896A_CFG_CH_EJECTOR_CAP			"EjectorCap"			//Wafer Ejector Ring
#define	MS896A_CFG_CH_EJ_ELEVATOR			"EjectorElevator Z"		//Wafer Ejector Elevator
#define MS896A_CFG_CH_EJECTOR_X				"Ejector X"				//Wafer Ejector X
#define MS896A_CFG_CH_EJECTOR_Y				"Ejector Y"				//Wafer Ejector Y
#define MS896A_CFG_CH_EJECTOR_T				"Ejector T"				//Wafer Ejector Theta

#define MS896A_CFG_CH_BIN_EJECTOR			"BinEjector"			//Bin Ejector Pin
#define MS896A_CFG_CH_BIN_EJECTOR_CAP		"BinEjectorCap"			//Bin Ejector Ring
#define	MS896A_CFG_CH_BIN_EJ_ELEVATOR		"BinEjectorElevator Z"	//Bin Ejector Elevator
#define MS896A_CFG_CH_BIN_EJECTOR_X			"BinEjector X"			//Bin Ejector X
#define MS896A_CFG_CH_BIN_EJECTOR_Y			"BinEjector Y"			//Bin Ejector Y
#define MS896A_CFG_CH_BIN_EJECTOR_T			"BinEjector T"			//Bin Ejector Theta

#define	MS896A_CFG_CH_NGPICK				"NGPick T"				//v4.24
#define MS896A_CFG_CH_BONDARM				"BondArm"
#define MS896A_CFG_CH_BONDHEAD				"BondHead"
#define MS896A_CFG_CH_PROBERZ				"Prober Z"
#define MS896A_CFG_CH_BONDHEAD2				"BondHead2"
#define MS896A_CFG_CH_CHGCOLLET_T			"ChgCollet Theta"		//v4.59A44
#define MS896A_CFG_CH_CHGCOLLET_Z			"ChgCollet Z"

#define MS896A_CFG_CH_SWCOMM				"SW Comm"
#define MS896A_CFG_CH_RESOLUTION			"Resolution"
#define MS896A_CFG_CH_RESOLUTION_FACTOR		"Resolution Factor"
#define MS896A_CFG_CH_MOTOR_DIRECTION		"Motor Direction"
#define MS896A_CFG_CH_HOME_DIRECTION		"Home Direction"		//v4.51A18	//MS90 EJTXY 
#define MS896A_CFG_CH_MIN_DISTANCE			"Min Distance"
#define MS896A_CFG_CH_MAX_DISTANCE			"Max Distance"
#define MS896A_CFG_CH_HOME_OFFSET			"Home Offset"
#define MS896A_CFG_CH_NEGLIMIT_OFFSET		"Neg Limit Offset"
#define MS896A_CFG_CH_MOTOR_PROTECTION		"Motor Protection"
#define MS896A_CFG_CH_ENGAGE_OFFSET			"Engage Offset"
#define MS896A_CFG_CH_FAST_PROFILE_TRIGGER	"Fast Profile Trigger"
#define MS_CFG_CH_TOOLING_T					"Tooling Theta"			//v4.37T11

//Special HW config
#define MS896A_CFG_CH_LARGE_TRAVEL			"Long Travel Range"		//v3.61
#define MS896A_CFG_CH_BLY_TIMEBELT_MOTOR	"Use TimeBelt Motor"	//v3.94T3

#define MS896A_FUNC_CUSTOMER				"Customer Name"
#define MS896A_FUNC_PRODUCT					"Product Line"			//v4.33T1	//PLSG
#define MS896A_FUNC_VERSION					"Version"
#define MS896A_FUNC_GENERAL					"General"
#define MS896A_FUNC_ENABLE_DEB_2016			"DEB"

#define MS896A_FUNC_GENERAL_DELAY_BEFORE_TURN_OFF_EJ_VAC "Delay Before Turn Off EJ Vac"
#define MS896A_FUNC_GENERAL_CAMERA_BLOCK_DELAY "Camera Block Delay"
#define MS896A_FUNC_GENERAL_RT_UPDTAE_MISSING_DIE_THRESHOLD	"Runtime Update Missing Die Threshold"
#define MS896A_FUNC_GENERAL_PREBOND			"PreBond Inspection"
#define MS896A_FUNC_GENERAL_POSTBOND		"PostBond Inspection"
#define MS896A_FUNC_GENERAL_BINFRAMESTATUSSUM	"Bin Frame Status Summary"
#define MS896A_FUNC_GENERAL_REFDIE			"Support Reference Die"
#define MS896A_FUNC_GENERAL_CHAR_REFDIE		"Support Char Reference Die"
#define MS896A_FUNC_GENERAL_POLYGON_DIE		"Support Polygon Die"
#define MS896A_FUNC_GENERAL_CHINESE_MENU	"Chinese Menu"
#define MS896A_FUNC_PKG_SAVE_TEMP_FILES		"Enable Save Tempfiles with PKG"
#define MS896A_FUNC_NEW_CT_ENHANCEMENT		"New CT Enhancement"
#define MS896A_FUNC_NEW_STEPPER_ENC			"New Stepper Encoder"
#define MS896A_FUNC_DIS_OK_SELECTION_IN_ALERT_MSG		"Disable OK Selection In Alert Msg"
#define MS896A_FUNC_DISABLE_DEFAULT_SEARCH_WND			"Disable Default Search Wnd"
#define MS896A_LANGUAGE									"Language"
#define MS896A_FUNC_BIN_MULTI_MGZN_SNRS					"Bin Multi-Mgzn Sensors"
#define MS896A_FUNC_BIN_6_MGZN_SNRS						"6 Magazine Sensors"		//v3.79
#define MS896A_FUNC_EMO									"EMO Checking"				//v3.91
#define MS896A_FUNC_CEMARK								"CE Mark"					//v4.06
#define MS896A_FUNC_DBH_THERMAL_CHECK					"DBH Thermal Check"			//v4.26T1
#define MS896A_FUNC_DBH_HEATINGCOIL						"HeatingCoil"				//v4.49A5
#define MS896A_FUNC_DBH_THERMOSTAT						"Thermostat"				//v4.49A5
#define MS896A_FUNC_AUTO_CHG_COLLET						"Auto Chg Collet"			//v4.50A6
#define MS896A_FUNC_OPERATOR_VALIDATION					"Opeator Validation"
#define MS896A_FUNC_ENABLE_PORTABLE_PKG_FILE			"Enable Portable PKG File"
#define MS896A_FUNC_PORTABLE_PKG_INFO_PAGE				"Portable PKG File Info Page"
#define MS896A_FUNC_ENABLE_SUMMARY_PAGE					"Summary Page"
#define	MS896A_FUNC_ENABLE_BOND_ALARM_PAGE				"Bond Alarm Page"
#define MS896A_FUNC_WAFERLOT_WITH_BIN_SUMMARY_FORMAT	"WaferLot With Bin Summary Format"
#define MS896A_FUNC_WAFERLOT_LOADED_MAP_CHECK			"WaferLot Loaded Map Check"
#define MS896A_FUNC_DOWNLOAD_PORTABLE_PACKAGE_FILE_IMAGE	"Download Portable Package File Image"
#define	MS896A_FUNC_GENERAL_AOI_OCR_DIE					"AOI machine support OCR die"
#define MS896A_FUNC_GENERAL_REFDIE_FACE_VALUE			"Reference Die Face Value Check"	//v4.48A26	//Avago
#define MS896A_FUNC_GENERAL_NGPICK						"MS NGPick"				//v4.51A19	//Silan MS90
#define MS896A_FUNC_GENERAL_PB_CLEANCOLLET				"PB Error CleanCollet"
#define MS896A_FUNC_GENERAL_COLLET_OFFSET_WO_EJTXY		"Collet Offset wo EjtXY"	//v4.52A14
#define MS896A_FUNC_CHECK_PKG_KEY_PARAMETERS			"PKG Check Key Parameters"
#define MS896A_FUNC_ALARM_WAIT_ENGINEER_TIME			"Alarm Wait Engineer Time"

// erase the image and record after loaded the package file
#define MS896A_FUNC_ERASE_PORTABLE_PACKAGE_FILE_IMAGE	"Erase Portable Package File Image"
#define MS896A_FUNC_PORTABLE_PACKAGE_FILE_FILE_LIST		"File List"
#define MS896A_FUNC_PORTABLE_PACAKGE_FILE_FILE_LIST_LENGTH	"File List Length"
#define MS896A_FUNC_PORTABLE_PACKAGE_FILE_IGNORE_LIST	"Portable Package File Ignore List"
#define MS896A_FUNC_PORTABLE_PACAKGE_FILE_FILE_NO		"File No"
#define MS896A_FUNC_MULTI_COR_OFFSET					"Multi COR Offset"
#define MS896A_FUNC_WIDE_SCREEN_MONITOR					"Wide Screen Monitor"
#define MS896A_FUNC_TOUCH_SCREEN_MONITOR				"Touch Screen Monitor"
#define MS896A_FUNC_MSOPTIONS_BH_UPLOOK_CAMERA			"BH Uplook Camera"
#define MS896A_FUNC_MS100_ORG_BH_SQ						"MS100 Orginal BH Sequence"

#define MS896A_FUNC_EXARM_DISABLE_EMPTY_PRELOAD		"ExArm Disable Empty Preload"
#define MS896A_FUNC_NEW_BH_MOUNT					"New BH Mount"		//MS810
#define MS896A_FUNC_OPT_BIN_COUNT_DYN_ASSIGN_GRADE	"Opt Bin Count Dynamic Grade Assignment"
#define MS896A_FUNC_OPEN_MATCH_PKG_NAME_CHECK		"Open Match Pkg Name Check"
#define MS896A_FUNC_SLOW_BT_PROFILE					"Slow BT Profile"
#define MS896A_FUNC_NO_STATIC_CONTROL_BT_JOYSTICK	"Not Use Static Control For BT Joystick"	//for Cree MS896-DL only
#define MS896A_FUNC_BL_RESET_GRADEMAG_EMPTY			"Reset GradeMag to Empty"
#define MS896A_FUNC_PPLM_SPECIAL_FCNS				"PLLM Special Fcns"
#define MS896A_FUNC_NO_PR_RECORD_PKG_FILE			"No PR Record In PKG File"
#define MS896A_FUNC_MACHINE_REPORT					"Enable Machine Report"
#define MS896A_FUNC_ITEM_LOG						"Enable Item Log"
#define MS896A_FUNC_WAFER_LOT_FILE_PROTECTION		"Wafer Lot File Protection"
#define MS896A_FUNC_WAFER_END_CLEAR_MAP			"Clear Map After Wafer End"
#define MS896A_FUNC_BH_VAC_NEUTRALSTATE			"BHZ Vac Neutral State"
#define	MS896A_FUNC_WAFFLE_PAD_BOND_DIE			"Waffle Pad Bond Die"
#define MS896A_FUNC_WAFFLE_PAD_IDENTIFICATION	"Waffle Pad Identification"
#define MS896A_FUNC_GROUP_ALARM_ASSIST_SETUP_TO_IDLE "Group Alarm Assist Setup To Idle"
#define MS896A_FUNC_DETAILS_REPORT				"Enable Machine Time Details Report"
#define MS896A_FUNC_DETAILS_REPORT_FORMAT		"Machine Time Details Report Format"
#define MS896A_FUNC_MACHINE_TIME_REPORT_EXT		"Machine Time Report Ext"
#define MS896A_FUNC_ENABLE_THERMAL_EJECTOR		"Enable Thermal Ejector"
#define	MS896A_FUNC_ENABLE_COLOR_CAMERA			"Enable Color Camera"
#define	MS896A_FUNC_PRESCAN_EMPTY_UNMARK		"Enable Prescan Empty Die Unmark"
#define MS896A_FUNC_SLOW_EJ_PROFILE				"Enable Slow Ej Profile"		//v4.08		//For MS100
#define	MS896A_FUNC_HOME_FPC_USE_HMI_MAP		"Enable Home FPC Use Hmi Map"
#define	MS896A_FUNC_BLBC_USE_OLD_CONTINUE_STOP	"BL Barcode Use Old User Continue Stop"
#define MS896A_FUNC_BL_USE_29_MAGSLOTS			"BL Use 29 Mgzn Slots"			//v4.19		//Cree HuiZhou
#define	MS896A_FUNC_BLBC_EMPTY_SCAN_TWICE		"BL EMPTY Barcode Scan Twice"	//v4.51A17
#define	MS896A_FUNC_BL_EMPTY_FRAME_BC_CHECK		"BL EMPTY Frame Barcode Check"

#define MS896A_FUNC_WAFERMAP					"WaferMapping"
#define MS896A_FUNC_SORTING_PATH_CAL_TIME		"Sorting Path Cal Time"
#define MS896A_FUNC_DISPLAY_SORT_BIN_ITEM		"Display Sort Bin Item"
#define MS896A_FUNC_COMPLETE_FILENAME			"Search Complete Filename"
#define MS896A_FUNC_SEARCH_DEEP_IN_FOLDER		"Search Deep In Folder"
#define MS896A_FUNC_WAFERMAP_USE_SCN			"Support SCN"
#define MS896A_FUNC_WAFERMAP_USE_BLOCK			"Support Block Function"
#define MS896A_FUNC_WAFERMAP_BLOCK_MODE			"Block Pick Mode"
#define MS896A_FUNC_WAFERMAP_PSM_EXIST_CHECK	"PSM Exist Check"
#define MS896A_FUNC_WAFERMAP_BATCH_ID_FILE_CHECK	"Batch ID File Check"
#define MS896A_FUNC_WAFERMAP_CHK_SEP_GRADE		"Check Separation Grade"
#define MS896A_FUNC_WAFERMAP_CHECK_MASTER_PITCH "Check Master Pitch"
#define MS896A_FUNC_WAFERMAP_SWALK_1STDIE		"SWalk 1st Die Finder"
#define MS896A_FUNC_WAFERMAP_IGNORE_GRADE_0		"Ignore Grade 0"
#define MS896A_FUNC_WAFERMAP_EMPTYCHECK			"Support Empty Check"			//Block2
#define MS896A_FUNC_WAFERMAP_AUTO_MANUAL_ALIGN	"Auto Perform Manual Align"		//Block2
#define MS896A_FUNC_WAFERMAP_REVERIFY_REFDIE	"Enable Re-verify Ref Die"		//Cree
#define MS896A_FUNC_NONBLKPICK_VERIFY_REFDIE	"Enable Non BlkPick Verify Ref Die"
#define MS896A_FUNC_2D_BARCODE_CHECK			"2D Barcode Check"				//v3.60	//Cree China
#define	MS896A_FUNC_NOT_SAVE_WAFER_MAP			"Skip Save Wafer Map"

#define MS896A_FUNC_2ND_1D_BARCODE_CHECK		"2nd 1D Barcode Check"				
#define	MS896A_FUNC_WAFERMAP_PICK_DEFECT_DIE	"To Pick Defective Die"
#define MS896A_FUNC_WAFERMAP_ENABLE_BINMAP		"Enable Bin Map"				//v4.03
#define	MS896A_FUNC_WAFERMAP_MANUAL_INPUT_BC	"Enable Manual Input Barcode"	//v4.06
#define	MS896A_FUNC_MANUAL_ALIGN_REFER_DIE		"Enable Manual Align Refer Die"
#define MS896A_FUNC_MULTIGRADE_SORT_TO_SINGLEBIN	"MultiGrade Sort To SingleBin"		//v4.15T8
#define MS896A_FUNC_CMLT_REWORK						"Enable CMLT Rework"			//v4.15T9
#define MS896A_FUNC_NO_IGNORE_GRADE_ON_INTERFACE	"No Ignore Grade On Interface"
#define MS896A_FUNC_CHECK_IGNORE_GRADE_COUNT	"Check Ignore Count"
#define MS896A_FUNC_IGNORE_GRADE				"Ignore Grade"
#define MS896A_FUNC_CHECK_MAP_COLUMNS			"Check Map Columns"				//v4.40T13	//BlueLight
#define MS896A_FUNC_OSRAM_RESORT_MODE			"Enable Osram Resort Mode"		//v4.21T3

#define	MS896A_FUNC_DETECT_SKIP_MODE			"Detect Skip Mode"
#define MS896A_FUNC_ASSOC_FILE_FUNC				"Enable Assoc File Func"
#define MS896A_FUNC_ASSOC_FILE_EXT				"Assoc File Ext"
#define	MS896A_FUNC_WAFERMAP_RENAME_AFTER_END	"Rename map filename after wafer end"

#define MS896A_FUNC_ALWAYS_LOAD_MAP_LOCAL_HD	"Always Load Map From Local HD"

#define MS896A_FUNC_OUTFILES					"Output Files"
#define MS896A_FUNC_CLRBINCNT					"Clear Bin Count"
#define MS896A_FUNC_CLR_BIN_BY_GRADE			"Clear Bin By Grade"
#define MS896A_FUNC_AUTO_GEN_BIN_BLK_SUMMARY	"Auto Gen Bin Blk Summary"
#define MS896A_REMOVE_BACKUP_OUTPUTFILE			"Remove Backup Output File"
#define MS896A_KEEP_OUTPUT_FILE_LOG				"Keep Output File Log"
#define	MS896A_AUTO_TRANSFER_OUTPUT_FILE		"Auto Upload Output Files"

#define MS896A_ENABLE_AUTO_TYPE_FIELD_NAME_CHECK	"Enable Auto Type Field Name Check"
#define MS896A_DIE_TYPE_FIELD_NAME_IN_MAP			"Die Type Field Name In Map"
#define MS896A_PKG_FILE_MAP_HEADER_CHECK_FUNC		"Package File Map Header Check Func"
#define MS896A_PKG_FILE_CHECK_MAP_HEADER_STRING		"Package File Check Map Header String"
#define MS896A_UPDATE_WAFER_MAP_HEADER				"Update Wafermap Header"
#define MS896A_CLEAR_DIE_TYPE_FIELD_AFTER_CLEAR_ALL_BIN	"Clear Die Type Field After Clear All Bin"

#define MS896A_DIE_TYPE_CHECK						"Die Type Check"
#define MS896A_FUNC_ENABLE_DIE_TYPE_CHECK_FUNC		"Enable Die Type Check Func"
#define MS896A_FUNC_DIE_TYPE_CHECK_FIELDNAME		"Die Type Check Fieldname"

#define MS896A_FUNC_WAFERMAP_DISABLE_FIVE_POINT_CHECK_MSG_SEL	"Disable Five Point Check Msg Selection"

#define MS896A_FUNC_NEW_LOT_OPTIONS			"New Lot Options"
#define MS896A_FUNC_OF_FORMAT_SELECT		"Enable Format Selection"
#define MS896A_FUNC_OUTPUT_FILE_FORMAT		"Output File Format"
#define MS896A_FUNC_MULTIPLE_HEADER_PAGE	"Multiple Header Page"
#define MS896A_FUNC_CHANGE_GRADE_BACKUPTEMP_FILE	"Change Grade Backup TempFile"
#define MS896A_FUNC_ENABLE_EXTRA_CLR_BIN_INTO "Enable Extra Clear Bin Info"
#define MS896A_EXTRA_BIN_INFO				"Extra Bin Information"
#define MS896A_FUNC_DIE_TIMESTAMP			"Die Timestamp"
#define MS896A_FUNC_DIE_PROCESSTIME			"Die ProcessTime"
#define MS896A_FUNC_ADD_SORTING_SEQUENCE	"Add sorting sequence"
#define MS896A_FUNC_WAFEREND				"Wafer End"
#define MS896A_FUNC_REFDIESTATUSFILE		"RefDie Status File"
#define MS896A_FUNC_WAFERENDFILEPROTECTION	"Wafer End File Protection"
#define MS896A_FUNC_AUTO_GEN_WAFEREND		"Auto Generate Wafer End"
#define MS896A_FUNC_AUTO_GEN_WAFEREND_PRESCAN	"Auto Generate Wafer End After Prescan"
#define MS896A_FUNC_LABELPRINTOUT			"Label Printout"
#define MS896A_FUNC_LABELPRINTOUT_XML		"Label Printout in XML Format"
#define MS896A_FUNC_BINNO_DIRECTORY			"BinLot Directory"
#define MS896A_FUNC_BIN_WORKNO				"Bin WorkNo"
#define MS896A_FUNC_FILE_NAME_PREFIX		"FileName PreFix"
#define MS896A_FUNC_APPEND_IN_WAFERID		"Enable Append Info In WaferID"
#define MS896A_FUNC_WAFERID_IN_MSGSUMMARY	"Add WaferId in Magazine Summary File"
#define MS896A_FUNC_GRADE_MAPPING_FILE		"Enable Grade Mapping File"
#define MS896A_FUNC_PICK_N_PLACE_OUTPUT		"Enable Pick And Place Output"
#define MS896A_FUNC_BIN_SNR					"Bin Serial No"
#define MS896A_FUNC_DYNMAPHEADER			"Dynamic Map Header"
#define MS896A_FUNC_BACKUP_TEMP_FILE				"Backup Temp File"
#define MS896A_FUNC_BACKUP_OUTPUT_TEMP_FILE			"Backup Output Temp File"
#define MS896A_FUNC_FORCE_DISABLE_HAVE_OTHER_FILE	"Force disable have other file"
#define MS896A_FUNC_MANUAL_UPLOAD_BIN_SUMMARY_FILE	"Manual Upload Bin Summary File"
#define MS896A_FUNC_GEN_ACCUMULATE_DIE_COUNT_REPORT	"Generate Accumulate Die Count Report"
#define MS896A_FUNC_BIN_SNR_SEL_FORMAT				"Disable BSNR Format Selection"
#define MS896A_FUNC_BIN_SNR_DEFAULT_FORMAT			"Default BSNR Reset Format"
#define MS896A_FUNC_SEL_CLEAR_COUNT_FORMAT			"Disable Clear Count Selection"
#define MS896A_FUNC_CLEAR_COUNT_DEFAULT_FORMAT		"Default Clear Count Reset Format"

#define MS896A_FUNC_ALIGN_WAFER_IMAGE_LOG			"Enable Align Wafer Image Log"
#define MS896A_FUNC_TOOLS_USAGE_RECORD		"Tools Usage Record"
#define MS896A_FUNC_TOOLS_RECORD_FORMAT		"Tools Usage Record Format"
#define MS896A_FUNC_ENABLE_ITEM_LOG			"Enable Item Log"
#define	MS896A_FUNC_OUTPUT_FILE_PATH2		"Enable Bin Output File Path2"
#define	MS896A_FUNC_SCAN_DIE_PITCH_CHECK	"Enable Prescan Die Pitch Check"
#define MS896A_FUNC_PKG_FILE_LIST			"Enable PKG File List"
#define MS896A_FUNC_ALARM_LAMP_BLINK		"Enable Alarm Lamp Blink"
#define MS896A_FUNC_AMI_FILE				"Enable Ami File"
#define MS896A_FUNC_BARCODE_LENGTH_CHECK	"Enable Barcode Length Check"
#define MS896A_FUNC_INPUT_COUNT_SETUP_FILE		"Enable Input Count Setup File"
#define MS896A_FUNC_CLEAR_BIN_BEFORE_NEW_LOT	"Clear Bin Before New Lot"
#define MS896A_FUNC_NEW_LOT_CREATE_DIR			"New Lot Create Directory"
#define MS896A_FUNC_WL_BC_SELECTION			"WL Barcode Selection"
#define MS896A_FUNC_ON_OFF_2D_BAR_SCANNER	"On Off 2D barcode scanner"
#define MS896A_FUNC_GENERAL_SEMI_AUTO		"Semi Auto Mode"

#define MS896A_FUNC_PORTABLE_PACKAGE_FILE	"Portable Package File"
#define MS896A_FUNC_HMISCREEN				"HMI Screen"
#define	MS896A_FUNC_BOND_TASK_SHORTCUT_BAR	"Enable Bond Task Shortcut Bar"
#define	MS896A_FUNC_SCAN_TASK_SHORTCUT_BAR	"Enable Scan Task Shortcut Bar"
#define MS896A_FUNC_HMI_OPMENU_SHORTCUT		"OP Menu Shortcut"
#define MS896A_FUNC_HMIBHSHORTCUT			"Enable BH Shortcut"
#define MS896A_FUNC_HMIWPRSHORTCUT			"Enable WPR Shortcut"
#define MS896A_FUNC_HMIBPRSHORTCUT			"Enable BPR Shortcut"
#define MS896A_FUNC_HMIBINBLKSHORTCUT		"Enable BinBlock Shortcut"
#define MS896A_FUNC_HMI_WTSHORTCUT			"Enable WaferTable Shortcut"
#define MS896A_FUNC_HMI_PRRECORD_SHORTCUT	"PR Record Shortcut"
#define MS896A_FUNC_HMI_DISABLE_PKG_SHORTCUT	"Disable PKG Shortcut"
#define MS896A_FUNC_HMI_BINTABLE_SHORTCUT	"Enable BinTable Shortcut"
#define MS896A_FUNC_HMIPICKNPLACESHORTCUT	"Enable Pick&Place Shortcut"
#define MS896A_FUNC_HMI_OPTIONSTAB_SHORTCUT	"Optons Tab Shortcut"
#define MS896A_FUNC_HMI_DISABLE_OPTION_SHORTCUT		"Disable Option Shortcut"
#define MS896A_FUNC_HMI_SYSTEMSERVICE_SHORTCUT	"System Service Shortcut"
#define MS896A_FUNC_HMI_STEP_MOVE_SHORTCUT	"Step Move Shortcut"
#define MS896A_FUNC_HMI_OPERATION_MODE_SHORTCUT	"Operation Mode Shortcut"
#define MS896A_FUNC_HMI_PHY_BLOCK_SHORTCUT	"Physical Block Shortcut"
#define MS896A_FUNC_HMI_ALLOW_MINIMIZE_HMI	"Allow Minimize Hmi"
#define	MS896A_FUNC_HMI_TOGGLE_WAFER_RESORT_MODE	"Allow Toggle Wafer Resort Scan Mode"
#define	MS896A_FUNC_HMI_WAFER_BIN_RESORT_MODE		"Allow Wafer To Bin Resort Mode"

#define MS896A_FUNC_HOST_COMM					"Host Communication"
#define MS896A_FUNC_HOST_COMM_SECSGEM			"SECS/GEM"
#define MS896A_FUNC_HOST_COMM_TCPIP				"TCP/IP"
#define MS896A_FUNC_OTHERS_CONNECTION_TYPE		"Others Connection Type"
#define MS896A_FUNC_HOST_COMM_EQUIP_MANAGER		"Equipment Manager"

#define MS896A_FUNC_VISION						"Vision"
#define MS896A_FUNC_VISION_BT_REALIGN_FF_MODE	"BT Realign With FF Mode"
#define MS896A_FUNC_VISION_DUAL_COLOR			"Enable Dual Color Lighting"
#define MS896A_FUNC_PRS_MOTORIZEDZOOM			"PRS Motorized Zoom"
#define MS896A_FUNC_ENABLE_EMPTY_BINFRAME_CHECK	"Enable Empty BinFrame Check"
#define MS896A_FUNC_ENABLE_RESET_MAG_CHECK		"Enable Reset Magz Check"
#define MS896A_FUNC_VISION_CIRCLE_DETECTION		"Circle Detection"
#define MS896A_FUNC_VISION_AUTOLEARN_REFDIE		"Auto Learn Ref Die"
#define MS896A_FUNC_VISION_INSP_MACHINE			"Inspection Machine"
#define MS896A_FUNC_VISION_ADAPT_WAFER			"Enable Adapt Wafer"
#define MS896A_FUNC_VISION_LRN_DIE_COUNTER_CHECK_NORMAL_RECORD	"Lrn Die Counter Check Normal Record"
#define MS896A_FUNC_VISION_LINE_INSPECTION		"Line Inspection"
#define MS896A_FUNC_VISION_WPR_2_LIGHTING		"Wafer Pr Two Lighting"
#define MS896A_FUNC_VISION_AUTO_ENLARGE_SRCHWND	"Auto Enlarge SrchWnd"

#define MS896A_FUNC_VISION_POSTSEAL_OPTICS		"Post Seal Optics"			//v3.71T5
#define MS896A_FUNC_VISION_BH_POSTSEAL_OPTICS	"BH Post Seal Optics"		//v4.39T11
#define MS896A_FUNC_VISION_BH_UPLOOK_PR			"BH Uplook PR"				//v4.52A16
#define MS896A_FUNC_VISION_BH_UPLOOK_PR_ONLY	"BH Uplook PR 2"			//v4.58A3
#define	ES_FUNC_VISION_WL_CONTOUR_CAMERA		"ES WL Contour Camera"
#define	MS60_FUNC_VISION_DUAL_PATH				"MS Dual Path Camera"
#define	MS60_VISION_5M_SORTING_FF_MODE			"Enable Sorting FF Mode"

#define MS896A_FUNC_VISION_OFFLINE_PBTEST	"Offline Postbond Test"			//v.411T3
#define MS896A_FUNC_MANUAL_PR_JS_SPEED		"Use Manual PR JS Speed"		//v4.16T6	//Huga
#define MS896A_FUNC_MxN_LF_WND				"Wafer PR Mxn LF Search"		//v4.43T2
#define MS896A_FUNC_MS60_LF_SEQ				"MS60 LF Sequence"				//v4.47T3
#define MS896A_FUNC_WPR_BACK_LIGHT			"Wafer PR Back Light"			//v4.46T28	//Cree HuiZhou

//4.53D18
#define MS896A_FUNC_MAP_DIE_MIX				"Wafer Map Die Mixing"

#define MS896A_FUNC_ENABLE_MOUSE_CONTROL	"Enable Mouse Control"
#define MS896A_FUNC_JOYSTICK_MODE			"Joystick Mode"					//v3.76
#define MS896A_FUNC_PR_MOUSE_JOYSTICK		"PR Mouse Joystick"
#define MS896A_FUNC_BL_OUTMAG_MODE			"BL OutputMag Config Mode"		//v3.82

#define MS896A_FUNC_BINBLK						"Bin Block"
#define MS896A_FUNC_BINBLK_TEMPLATE				"Enable Bin Blk Template"
#define MS896A_FUNC_AUTOLOAD_NAMING_FILE		"Auto Load Naming File"
#define MS896A_FUNC_BINBLK_LOAD_RANKID_FROM_MAP	"Enable Load Rank ID From Map"
#define MS896A_FUNC_BINBLK_RANKID_NAME_IN_MAP	"RankID Name In Map"
#define MS896A_FUNC_BINBLK_RANKID_FILE_EXT		"RankID File Ext"
#define MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT	"Enable Optimize Bin Count Func"
#define MS896A_FUNC_BINBLK_OPTIMIZE_BIN_COUNT_PER_WAFER	"Enable Optimize Bin Count Func Per Wafer"
#define MS896A_FUNC_BINBLK_OPBC_LOADPATH		"Enable OpBinCnt In LoadPath"
#define MS896A_FUNC_ENABLE_RESTORE_BINRUMTIME	"Restore Bin RunTime Data"
#define	MS896A_FUNC_BINBLK_NEW_REALIGN			"Bin Frame New Realign Method"
#define MS896A_FUNC_BINBLK_SOFT_REALIGN			"Bin Frame Soft Realign Method"
#define	MS896A_FUNC_BINBLK_MULTI_SEARCH_FIRST_DIE		"Bin Multi Search First Die"   // 4.51D20
#define MS896A_FUNC_BINBLK_USE_PT5_DIEPITCH		"Bin Pt5 Die Pitch"
#define MS896A_FUNC_BINBLK_BONDAREA_OFFSET		"Bin BondArea Offset"
#define MS896A_FUNC_BINBLK_GELPAD				"Bin GelPad Support"
#define MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW		"BT NewLot EmptyRow Fcn"
#define MS896A_FUNC_GENERAL_PRORATA				"Generate Prorata Data"  //M69
#define MS896A_FUNC_DISABLE_BUZZER				"Disable Buzzer"

#define MS896A_FUNC_SENSOR					"Sensor Function"
#define MS896A_FUNC_SENSOR_NEW_BL_MAGEXIST_CHECKING		"BL New MagExist Checking"		//v3.57	//OsramGSB
#define MS896A_FUNC_SENSOR_BT_VAC_METER_CHECKING		"BT Vac Meter Check"			//v3.58	//Fatc DLA
#define MS896A_FUNC_SENSOR_BL_RT_COVERSENSOR_CHECK		"BL RT Cover Sensor Check"		//v3.60	//Fatc
#define MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE			"BL RT Mgzn Exchange"			//v4.40T13
#define MS896A_FUNC_SENSOR_COVER_LOCK					"BH Cover Lock"		//v3.60		//Cree, Lumileds (for Front & Side cover lock
#define MS896A_FUNC_COLLET_CLEAN_POCKET					"BH ColletClean Pocket"
#define MS896A_FUNC_BH_SUCKING_HEAD						"BH Sucking Head"			//v4.05
#define MS896A_FUNC_WAFER_SIZE_SELECT					"Wafer Size Select"
#define MS896A_FUNC_SO_WAFER_CLAMP						"Wafer Clamp"				//v4.26T1		//Walsin China	MS810EL-90
#define MS896A_FUNC_START_UP_COVER_SNR_CHECK			"Start up Cover Sensor Check"
#define MS896A_FUNC_BINLOADER_COVER_SNR_CHECK			"BinLoader Cover Sensor Check"
#define	MS896A_FUNC_CLEAN_EJECTOR_CAP_STOP_CYCLE		"Clean Ejector Cap Stop Cycle"
#define MS896A_FUNC_COVER_SENSOR_PROTECTION_MSG			"Cover Sensor Protection Msg"
#define MS896A_FUNC_EJ_REAPLCE_AT_WAFEREND				"Replace Ejector Pin Stop Cycle"
#define MS896A_FUNC_EJ_REAPLCE_AT_WAFERSTART			"Replace Ejector Pin Start Cycle"
#define MS896A_FUNC_MANUAL_CLOSE_EXP_BC					"Manual Close Exp Read BC"
#define MS896A_FUNC_BIN_FRAME_CROSS						"Bin Frame Have Cross"
#define MS896A_FUNC_ENABLE_COPY_TEMP_FILE_IN_CLEAR_BIN	"Enable Copy Temp Files in Clear Bin"
#define MS896A_FUNC_ENABLE_BONDHEAD_TOLERANCE_LEVEL		"Enable the bondhead tolerance level"
#define MS896A_FUNC_GENERAL_ENABLE_LOG_COLLET_HOLE_DATA	"Enable Log Collet Hole Data"
#define MS896A_FUNC_BHZ1_HOME_OFFSET					"MS100 BHZ1 HOME Offset"
#define MS896A_FUNC_BHZ2_HOME_OFFSET					"MS100 BHZ2 HOME Offset"
#define MS896A_FUNC_BPRERROR_NOCLEANCOLLET				"BPR Error No Clean Collet"

#define MS896A_LABEL_PRINTERS				"Label Printers"
#define MS896A_LABEL_PRINTERS_NUMBER		"Printers Numbers"
#define MS896A_PRINTER_MODEL				"Printers Model"

#define MS896A_SERIAL_VERSION "Serial Version"

#define MS896A_ENGLISH						0
#define MS896A_CHINESE_SIMPLIFIED			1
#define MS896A_CHINESE_TRADITIONAL			3

#define MS896A_DIE_TYPE_CHECK_LIMIT			8

#define WPR_PKG_PREVIEW_TEMP				"PR Preview Temp"
#define WPR_PREVIEW_IMAGES					"res"
#define WPR_PRVIEW_IMAGE_SIZE				230

// For LastState.smf use
/*-- Wafer Table: Alignment Data --*/
#define	WT_ALIGN_DATA				"Wafer Alignment"
#define	WT_ALIGNED					"Wafer Aligned"
#define	WT_GLOBAL_THETA				"Global Theta"
#define	WT_SCN_LOADED				"SCN Loaded"
#define	WT_SCN_DIE_A_X				"SCN Die A X"
#define WT_SCN_DIE_A_Y				"SCN Die A Y"
#define	WT_SCN_DIE_B_X				"SCN Die B X"
#define WT_SCN_DIE_B_Y				"SCN Die B Y"
#define	WT_SCN_DIE_A_ROW			"SCN Die A ROW"
#define WT_SCN_DIE_A_COL			"SCN Die A COL"
#define	WT_SCN_DIE_B_ROW			"SCN Die B ROW"
#define WT_SCN_DIE_B_COL			"SCN Die B COL"
#define WT_SCN_ANGLE_X				"SCN Angle X"
#define WT_SCN_ANGLE_Y				"SCN Angle Y"
#define	WT_MS_DIE_A_X				"MS Die A X"
#define WT_MS_DIE_A_Y				"MS Die A Y"
#define	WT_MS_DIE_B_X				"MS Die B X"
#define WT_MS_DIE_B_Y				"MS Die B Y"
#define WT_MS_ANGLE_X				"MS Angle X"
#define WT_MS_ANGLE_Y				"MS Angle Y"

#define WT_LHS_INDEX				"LHS Index"
#define WT_RHS_INDEX				"RHS Index"
#define	WT_AUTO_ALIGN_STD			"Auto Search Home STD"
#define WT_AUTO_HOME_OPTION			"Search HomeDie Option STD"
#define WT_HOME_PHY_X				"WT Home Die Physical Pos X"
#define WT_HOME_PHY_Y				"WT Home Die Physical Pos Y"
#define WT_SPRIAL_SIZE				"WT Loop Search Matrix Size"
#define WT_CORNER_SRCH_OPTION		"WT Corner Search Option"
#define WT_ALIGN_ANGLE_MODE			"WT Align Wafer Angle Mode"

#define WT_SCN_INDEX_X				"SCN Index X"
#define WT_SCN_INDEX_Y				"SCN Index Y"
#define WT_SCN_KEEP_ALIGNDIE		"SCN Keep Alignment Die"	
#define WT_SCN_ADVPITCH_ENABLE		"Enable SCN AdvPitch"
#define WT_SCN_ADVPITCH_SIZE		"SCN AdvPitch Size"
#define WT_SCN_CHECK_SCN_LOADED		"Check SCN Loaded"
#define WT_MOVEBACK_ANGLE			"Move Back Angle"
#define WT_SCN_CHECK_ADJ			"Scn Check Auto Adjustment"
#define WT_REF_HOME_DIE_ROW_OFFSET	"Ref Home Die Row Offset"
#define WT_REF_HOME_DIE_COL_OFFSET	"Ref Home Die Col Offset"
#define WT_REF_HOME_DIE_ROW			"Home Die Row"
#define WT_REF_HOME_DIE_COL			"Home Die Col"

#define	WT_SCN_CHECK_POS			"SCN Check Position"
#define WT_SCN_CHECK_ENABLE			"Enable SCN Check"
#define	WT_SCN_ALIGN_FPC_ENABLE		"Enable SCN align with FPC"
#define	WT_SCN_CHECK_ROW			"Row"
#define	WT_SCN_CHECK_COL			"Col"
#define	WT_SCN_CHECK_ORG_USER_ROW	"Org User Row"
#define	WT_SCN_CHECK_ORG_USER_COL	"Org User Col"
#define WT_SCN_CHECK_BY_VIS			"SCN Check By PR"
#define WT_SCN_CHECK_IS_REFDIE		"SCN Check Is RefDie"
#define	WT_SCN_CHECK_REFDIE_NO		"SCN Check RefDie No"
#define	WT_SCN_CHECK_X_TOL			"SCN Check X Tolerance"
#define	WT_SCN_CHECK_Y_TOL			"SCN Check Y Tolerance"
#define WT_SCN_TOTAL_DIE			"No of Total SCN Die"
#define WT_SCN_CHECK_NULL_BIN_IN_MAP	"SCN Check Null Bin In Map"
#define WT_MAP_CHECK_HOME_NULL_BIN		"Map Check Home Null Bin"

#define WT_ALIGN_DIE_STEP			"Alignment Die Step"

/*-- Wafer Map Data --*/
#define	WT_MAP_DATA					"Wafer Map Data"
#define	WT_MAP_FORMAT				"Map Format"

/* -- Wafer Map Header Info -- */
#define WT_MAP_HEADER				"MapHeader"
#define WT_MAP_HEADER_INFO			"Wafer Map Header Info"
#define WT_MAP_HEADER_SPEC			"Spec"
#define WT_MAP_HEADER_BINPARA		"BinParameter"
#define WT_MAP_HEADER_TYPE			"Type"
#define WT_MAP_HEADER_SORTBIN		"SortBinItem"
#define WT_MAP_HEADER_DEVICENUMBER	"DeviceNumber"
#define WT_MAP_HEADER_RESORTINGBIN	"Resorting_Bin"
#define WT_MAP_HEADER_DEVICE2		"device"
#define WT_MAP_HEADER_DEVICE		"DEVICE"
#define WT_MAP_HEADER_WAFERBIN		"WAFERBINS"
#define WT_MAP_HEADER_MODEL			"MODEL"
#define WT_MAP_HEADER_MODEL2		"Model"
#define WT_MAP_HEADER_MODELNO		"ModelNo"
#define WT_MAP_HEADER_MODELNO2		"Model No"
#define WT_MAP_HEADER_WAFER			"WAFER"
#define WT_MAP_HEADER_FINISAR_E142_LAYOUTSPECIFIER	"LayoutSpecifier"	//v4.57A9

//#define WT_MAP_HEADER_				
//#define WT_MAP_HEADER_CENTERPOINT	"Center Point"
#define WT_PLOT_NO					"P-Lot NO."
#define WT_WIP_NO					"WIP NO."
#define WT_PART_NO					"Part NO."
#define WT_CLOT_NO					"C-Lot NO."
#define WT_LOT_NO					"Lot No"
#define WT_LOT_VP					"VP Value"  //4.53D5
#define WT_LOT_NO_2					"lot no"
#define WT_MAP_HEADER_TEST_TIME				"TestTime"
#define WT_MAP_HEADER_WO						"WO"
#define WT_MAP_HEADER_WO_2						"Wo"
#define WT_MAP_HEADER_WO_3						"wo"
#define WT_LOT_ID					"LOT_ID"
#define WT_LOT_NAME					"LotName"
#define WT_MAP_HEADER_SUBSTRATE_ID				"SUBSTRATE_ID"
#define WT_MAP_HEADER_SUBSTRATE_ID_2			"substrate_id"
#define WT_MAP_HEADER_CST_ID					"CST_ID"
#define WT_MAP_HEADER_RECIPE_ID				"Recipe_ID"
#define WT_MAP_HEADER_RECIPE_ID_2			"recipe_id"
#define WT_MAP_HEADER_RECIPE_NAME				"Recipe_Name"
#define WT_MAP_HEADER_RECIPE_NAME_2				"recipe_name"
#define WT_MAP_HEADER_OPERATOR					"OPERATOR"
#define WT_MAP_HEADER_SERIALNUMBER				"SerialNumber"
#define WT_MAP_HEADER_CODENUMBER				"CodeNumber"
#define WT_MAP_HEADER_DESIGN					"DESIGN"  //4.53D5
#define WT_MAP_HEADER_PROJECT					"Project"  //4.53D16
#define WT_MAP_HEADER_PART_NO					"Part No"
#define WT_MAP_SERIAL_NUMBER					"SerialNumber"
#define WT_MAP_HEADER_DN						"DN"	//Product Name
#define WT_MAP_HEADER_WN						"WN"	//Wafer No.
#define WT_MAP_HEADER_CM1						"CM1"	//Comment1
#define WT_MAP_HEADER_CM2						"CM2"	//Comment2
#define WT_MAP_HEADER_CM3						"CM3"	//Comment3
#define WT_MAP_HEADER_REMARK3					"Remark3"

#define WT_MAP_ADD_LOTNUMBER					"Lot #"
#define WT_MAP_ADD_CONDFILE					"Cond File"
#define WT_MAP_ADD_TYPE						"Type"
#define WT_MAP_ADD_SOURCE					"Source"
#define WT_MAP_ADD_CORRELATION				"Correlation"
#define WT_MAP_ADD_SETUP					"Setup Package"
#define WT_MAP_ADD_PR						"PR"
#define WT_MAP_ADD_START					"Start Date"
#define WT_MAP_ADD_OP						"Operator"


#define WT_MAP_HEADER_UN						"UN"
#define WT_MAP_HEADER_COND						"COND"
#define WT_MAP_HEADER_LO						"LO"
#define WT_MAP_HEADER_UP						"UP"
#define WT_MAP_HEADER_FT						"FT"
#define WT_MAP_HEADER_LIM						"LIM"
#define WT_MAP_HEADER_COEF1						"COEF1"
#define WT_MAP_HEADER_COEF2						"COEF2"
#define WT_MAP_HEADER_COEF3						"COEF3"

#define WT_MAP_HEADER_LOTID					"LotID"
#define WT_MAP_HEADER_PRODUCT				"Product"
#define WT_MAP_HEADER_PRODUCTNAME			"ProductName"
#define WT_MAP_HEADER_SORTSETUPFILE			"SortSetupFile"
#define WT_MAP_HEADER_SORTBINFNAME			"SortBINFileName"
#define WT_MAP_HEADER_LOTNUMBER				"LotNumber"
#define WT_MAP_HEADER_LATTICE_LOTNUMBER		"Lattice LotNumber"
#define WT_MAP_HEADER_LOTNUMBER2			"LotNumber2"
#define WT_MAP_HEADER_EPIID					"EPI_ID"
#define WT_MAP_HEADER_PROBER_DATE_TIME		"Prober Date"
#define WT_MAP_HEADER_PROBER_TEST_TIME		"Prober Test Time"
#define WT_MAP_HEADER_PROBER_TEST_TIME_2	"prober test time"
#define WT_MAP_HEADER_SORTINGBIN			"SortingBin"			//Osram Penang RESORT map header

#define WT_MAP_HEADER_PROBER_MACHINE_NO		"Prober M/C"
#define WT_MAP_HEADER_PROBER_MACHINE_NO_2	"prober m/c"		
#define WT_MAP_HEADER_BIN_TABLE_SERIAL_NO	"Bin table serial no"
#define WT_MAP_HEADER_BIN_TABLE_NO			"Bin Table No"
#define WT_MAP_HEADER_BIN_TABLE_SERIAL_NO_2	"Bin Table Serial No"
//#define WT_MAP_HEADER_BIN_TABLE_SERIAL_NO3	"Bin Table Serial no;ver"
#define WT_MAP_HEADER_BIN_TABLE_SERIAL_NO_3	"bin table serial no;ver"

#define WT_MAP_HEADER_INTERNAL_PRODUCT_NO	"internal product no"	
#define WT_MAP_HEADER_MODE					"mode"				
#define WT_MAP_HEADER_MODE2					"Mode"
#define WT_MAP_HEADER_SPECIFICATION			"Specification"
#define WT_MAP_HEADER_SPECIFICATION_2		"specification"
#define WT_DATE_IN_MAP_FILE					"Date"
#define WT_MAP_HEADER_MAP_OTHER				"MapOther"
#define WT_MAP_HEADER_BINTABLE				"Bin Table"
#define WT_MAP_HEADER_BINTABLE_2			"BinTable"
#define WT_MAP_HEADER_BINTABLENAME			"BinTableName"
#define WT_MAP_HEADER_REMARK1				"Remark1"
#define WT_MAP_HEADER_REMARK1_2				"remark1"
#define WT_MAP_HEADER_REMARK2				"Remark2"
#define WT_MAP_HEADER_REMARK2_2				"remark2"
#define WT_MAP_HEADER_CENTERPOINT			"center point"
#define WT_MAP_HEADER_ESD_VOLTAGE			"esd voltage"
#define WT_MAP_HEADER_ESD_VOLTAGE_2			"ESD Voltage"			
#define WT_MAP_HEADER_LIFETIME				"life time"
#define WT_MAP_HEADER_ESD					"esd"
#define WT_MAP_HEADER_PAD_COLOR				"pad color"
#define WT_MAP_HEADER_BIN_CODE				"bin_Code"
#define WT_MAP_HEADER_CONTROL_RANK			"Control Rank"
#define WT_MAP_HEADER_RANK_NO				"Rank No."
#define WT_MAP_HEADER_AT					"@"
#define WT_MAP_HEADER_AOYANG_BIN_GRADE_NAME		"BinGradeName"

//DingCheng	//APlusEpi
#define WT_MAP_HEADER_WAFTYPE				"Type"
#define WT_MAP_HEADER_BINNAME				"Bin Name"
#define WT_MAP_HEADER_BINNAME_2				"BinName"
#define WT_MAP_HEADER_U1					"U1"
#define WT_MAP_HEADER_U2					"U2"
#define WT_MAP_HEADER_U3					"U3"
//CyOptics US
#define WT_MAP_HEADER_CYOPTICS_HOOPS		"Hoops"
#define WT_MAP_HEADER_DELI_BINGRADE			"BinGrade"


/* -- Search Home die option -- */
#define WT_SRCH_HOME_OPTION			"Auto Search Home Option"
#define WT_SRCH_OPTION				"Search Option"
#define WT_HOME_MAP_ROW				"Home Die Map Row"
#define WT_HOME_MAP_COL				"Home Die Map Col"
#define WT_ORGMAP_ROW_OFFSET		"OrgMap Row Offset"
#define WT_ORGMAP_COL_OFFSET		"OrgMap Col Offset"
#define WT_SCNMAP_ROW_OFFSET		"ScnMap Row Offset"
#define WT_SCNMAP_COL_OFFSET		"ScnMap Col Offset"
#define WT_FIND_HOMEDIE_FROMMAP		"Find HomeDie From Map"
#define	WT_ADD_REFER_PITCH_ROW		"WT Add Refer Die Pitch Row"
#define	WT_ADD_REFER_PITCH_COL		"WT Add Refer Die Pitch Col"

/*-- Pr Record ID --*/
#define WPR_DIE_PRID				"Wafer PR Record ID"
#define BPR_DIE_PRID				"Bond PR Record ID"
#define PSPR_DIE_PRID				"PostSeal PR Record ID"		//v4.50A15

/*-- Number of Reference Die Learnt ---*/
#define	WPR_RDIE_LEARNT				"Number of Ref Die Learnt"		

//Machine Statistic
#define BH_MACHINE_STATISITC		"Machine Statistic"
#define BH_CURRENT_CYCLE_TIME		"Current Cycle Time"
#define BH_AVERAGE_CYCLE_TIME		"Average Cycle Time"
// End LastState.smf use

//Wafer map die state
#define WT_MAP_DIESTATE_DEFAULT				0
#define WT_MAP_DIESTATE_PICK				1
#define WT_MAP_DIESTATE_DEFECT				2
#define WT_MAP_DIESTATE_INK					3
#define WT_MAP_DIESTATE_CHIP				4
#define WT_MAP_DIESTATE_BADCUT				5
#define WT_MAP_DIESTATE_EMPTY				6
#define WT_MAP_DIESTATE_ROTATE				7

#define WT_MAP_DS_DEFAULT					0
#define WT_MAP_DS_PICK						1
#define WT_MAP_DS_PR_DEFECT					2
#define WT_MAP_DS_PR_INK					3
#define WT_MAP_DS_PR_CHIP					4
#define WT_MAP_DS_PR_BADCUT					5
#define WT_MAP_DS_PR_EMPTY					6
#define WT_MAP_DS_PR_ROTATE					7
#define	WT_MAP_DS_BH_MISSING				8	//	this maps to WT_MAP_DIESTATE_MISSING for grade change

#define WT_MAP_DIESTATE_UNPICK				10

#define	WT_MAP_DIESTATE_15					15

#define	WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY	20
#define	WT_MAP_DS_UNPICK_REGRAB_EMPTY		21
#define	WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT	30
#define	WT_MAP_DS_UNPICK_SCAN_EXTRA			35
#define	WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT	40
#define	WT_MAP_DS_SCAN_TWIN_DIE_FIRST		41
#define	WT_MAP_DS_SCAN_TWIN_DIE_JOINT		42
#define	WT_MAP_DS_SCAN_BACKUP_ALIGN			45
#define	WT_MAP_DIESTATE_SCAN_EDGE			50
#define	WT_MAP_DIESTATE_SKIP_PREDICTED		51
#define	WT_MAP_DIESTATE_LF_NOT_FULL_DIE		55
#define	WT_MAP_DS_SCAN_EDGE_CHECK			56

#define	WT_MAP_DIESTATE_DIMREFER			100
#define	WT_MAP_DS_BAR_2ND_START				111

// Reverse 200 - 400 for identify which grade is missing
#define WT_MAP_DIESTATE_MISSING				200
#define WT_MAP_DS_BH_UPLOOK_FAIL			400

//Wafer map die state



#define WT_DIE_TYPE_CHECK_FAIL	3
#define MES_NG_RX	4
#define MES_TIME_OUT_RX	5
#define MES_STOP_MACHINE	6

#define	SPECIAL_DEBUG			0	//	V450X16	interlock check event
#define	SPECIAL_DEBUG_LOG_WS	1	//	wrong sort		// V450X23
#define	SPECIAL_DEBUG_LOG_WT	1	//	WT log
#define	SPECIAL_DEBUG_LOG_AI	0	//	V450X23
#define	SPECIAL_DEBUG_LOG_BT	0
#define	SPECIAL_DEBUG_LOG_BH	1	//Matt: add Log
// Wafer PR Constant
#define		WPR_LF_ALIGN_SZIE			5.2
#define		WPR_LF_SIZE					3.3
#define		WPR_LF_SIZE_5X5				5.3
#define		WPR_LF_SIZE_7X7				7.3
#define		WPR_LF_SIZE_9X9				9.3
#define		WPR_LF_SIZE_11X11			11.3
#define		WPR_LF_SIZE_13X13			13.3

#define		BPR_LF_SIZE					3.3		//v3.80

#define MS_PICK_N_PLACE_DISABLED		0
#define MS_PICK_N_PLACE_MANUAL_MODE		1
#define MS_PICK_N_PLACE_AUTO_MODE		2

//v3.76
#define MS899_JS_MODE_HW			0
#define MS899_JS_MODE_MOVE			1
#define MS899_JS_MODE_PR			2
//Polygon wafer limit
#define WT_CIRCLE_LIMIT				0
#define WT_POLYGON_LIMIT			1
#define WT_MAX_POLYGON_PT			15

//xyz	//Lumileds Product Line
#define MS896A_PLLM_PRODUCT				"PLLM Product Line"
#define PLLM_REBEL		0
#define PLLM_FLASH		1
#define PLLM_DIEFAB		2
#define PLLM_LUMIRAMIC	3			//v3.94
#define PLSG_DIEFAB		4			//v3.93	//Quah

//Mouse Joystick 
#define MOUSE_STOP		0
#define MOUSE_CLICK		1
#define MOUSE_DRAG		2
#define MOUSE_SETUP		3			//v3.84

#define MOUSE_JOY_SPEED_SLOW		0
#define MOUSE_JOY_SPEED_MEDIUM		1
#define MOUSE_JOY_SPEED_FAST		2

#define MOUSE_JOY_DIRECTION_NONE		0
#define MOUSE_JOY_DIRECTION_UP			1
#define MOUSE_JOY_DIRECTION_DOWN		2
#define MOUSE_JOY_DIRECTION_LEFT		3
#define MOUSE_JOY_DIRECTION_RIGHT		4
#define MOUSE_JOY_DIRECTION_UP_LEFT		5
#define MOUSE_JOY_DIRECTION_UP_RIGHT	6
#define MOUSE_JOY_DIRECTION_DOWN_LEFT	7
#define MOUSE_JOY_DIRECTION_DOWN_RIGHT	8

#define MS_PROBETABLE_LEVEL_DATA_NO		441 // 21X21 size
#define MS_LEARN_LEVEL_CORNER_POINT		3

#define MS_LASERTEST_TRIGGER_BIT	"owGripperState"			// use output port as trigger bit
#define BH_SO_SUCKING_HEAD			"obSuckingHead"				//v4.05	//MS100 

#define WL_HISTORY_NAME					"C:\\MapSorter\\UserData\\Statistic\\WL_History.txt"
#define WL_WAFER_END_TIME_LOG_NAME		"C:\\MapSorter\\UserData\\Statistic\\WaferEndTimeLog.txt"
#define WL_HISTORY_TITLE				"Unload Date/Time, SlotNo, Barcode Name, WaferID, Bonded, Total Selected, Yield%"

#define	MSD_WAFER_LOT_INFO_FILE			gszROOT_DIRECTORY + "\\Exe\\WaferLotInfo.msd"
#define	MSD_WAFER_LOT_INFO_BKF			gszROOT_DIRECTORY + "\\Exe\\WaferLotInfobkf.msd"
#define	MSD_WL_BC_IN_CASSETTE_FILE		gszROOT_DIRECTORY + "\\Exe\\BarcodeInCassette.msd"
#define	MSD_WL_BC_IN_CASSETTE_BKF		gszROOT_DIRECTORY + "\\Exe\\BarcodeInCassettebkf.msd"
#define MSD_WAFER_RECORD_FILE			gszROOT_DIRECTORY + "\\Exe\\WaferRecord.msd"
#define MSD_WAFER_RECORD_BKF			gszROOT_DIRECTORY + "\\Exe\\WaferRecordbkf.msd"
#define	MSD_WAFER_RECORD_HISTORY_BKF	gszUSER_DIRECTORY + "\\History\\WaferRecord.msd"
#define	MSD_PLLM_CATCODE_FILE			gszROOT_DIRECTORY + "\\Exe\\PLLM_Catcode.msd"
#define	MSD_BIN_SUMMARY_FILE			gszROOT_DIRECTORY + "\\Exe\\BinSummaryFile.msd"
#define	MSD_MAP_TABLE_FILE				gszROOT_DIRECTORY + "\\Exe\\MapTable.msd"
#define	MSD_WAFER_BARCODE_FILE			gszROOT_DIRECTORY + "\\Exe\\WBarcode.msd"
#define	MSD_BL_BARCODE_FILE				gszROOT_DIRECTORY + "\\Exe\\BLBarcode.msd"
#define	MSD_SAFETY_FILE					gszROOT_DIRECTORY + "\\Exe\\Safety.msd"
#define	MSD_APP_DATA_FILE				gszROOT_DIRECTORY + "\\Exe\\AppData.msd"
#define	MSD_O_BARCODE_FILE				gszROOT_DIRECTORY + "\\Exe\\OBarcode.msd"
#define	MSD_LOT_INFO_FILE				gszROOT_DIRECTORY + "\\Exe\\LotInfo.msd"
#define	MSD_LOT_INFO_BKF				gszROOT_DIRECTORY + "\\Exe\\LotInfobkf.msd"
#define	MSD_WAFER_SORTED_BIN_BC_FILE	gszROOT_DIRECTORY + "\\Exe\\WaferSortedBinBC.msd"
#define MSD_SUMMARY_DATA_FILE			gszROOT_DIRECTORY + "\\Exe\\SummaryData.msd"
#define MSD_WAFER_HEADER				gszROOT_DIRECTORY + "\\Exe\\WaferHeaderInfo.msd"


//////////////////////////////////////////////////////////////////////////////
//	Station Name Definitions
//////////////////////////////////////////////////////////////////////////////
#define WAFER_TABLE_STN		_T("WaferTableStn")
#define WAFER_PR_STN		_T("WaferPrStn")
#define BOND_HEAD_STN		_T("BondHeadStn")
#define BIN_TABLE_STN		_T("BinTableStn")
#define BOND_PR_STN			_T("BondPrStn")
#define SECS_COMM_STN		_T("SecsCommStn")
#define WAFER_LOADER_STN	_T("WaferLoaderStn")
#define BIN_LOADER_STN		_T("BinLoaderStn")
#define SAFETY_STN			_T("SafetyStn")
#define NVC_LOADER_STN		_T("NVCLoaderStn")


////////////////////////////////////////////
//	MS Customer Name
////////////////////////////////////////////
#define CTM_CREE		"Cree"
#define CTM_OSRAM		"OSRAM"
#define CTM_LUMILEDS	"Lumileds"

#define CTM_NICHIA		"Nichia"
#define CTM_RENESAS		"Renesas"

#define CTM_SANAN		"SanAn"
#define CTM_SEMITEK		"Semitek"

#define CTM_WOLFSPEED	"WolfSpeed"
#define CTM_FINISAR		"Finisar"

#define	CTM_SILAN		"SiLan"
#define	CTM_TESTAR		"Testar"
#define	CTM_LEXTAR		"Lextar"

#define CTM_CHANGELIGHTXM		"ChangeLight(XM)"
