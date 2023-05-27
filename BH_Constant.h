//////////////////////////////////////////////////////////////////////////////
//	BH_Constant.h : Defines the Constant for BondHead class
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

///////////////////////////////////////
// Constants Definition
///////////////////////////////////////
enum eBH_EVENTID {MISSING_DIE = 1, COLLECT_JAM, PICK_POSITION, BOND_POSITION, PICK_LEVEL_Z1, PICK_LEVEL_Z2, BOND_LEVEL_Z1, 
				  BOND_LEVEL_Z2, EJECTOR_UP_LEVEL, PICK_DELAY, BOND_DELAY, ARM_PICK_DELAY, ARM_BOND_DELAY, HEAD_PICK_DELAY, HEAD_BOND_DELAY,
				  HEAD_PREPICK_DELAY, SYNC_PICK_DELAY, PR_DELAY, WT_THETA_DELAY, BT_DELAY, EJ_UP_DELAY, EJ_DOWN_DELAY, COLLECT_JAM_RETRY_COUNT,
				  SYNC_MOVE, SYNC_MOVE_TRIGGER_LEVEL, SYNC_MOVE_PRELOAD, EJECTOR_STANDBY_LEVEL, CLEAN_COLLET_LEVEL, AUTO_CLEAN_COLLET, 
				  CHECK_MACHINE_COVER, CLEAN_COLLET_LIMIT, CHANGE_COLLET_LIMIT_Z1, CHANGE_COLLET_LIMIT_Z2, CHANGE_EJECTOR_LIMIT
				 };

enum eBA_MOVEMODE {BH_OBW_MV = 0, BH_CONTOUR};

enum eBA_MOVEPOS {BH_NA = -1, BH_T_PICK_TO_BOND = 0, BH_T_BOND_TO_PICK, BH_T_PREPICK_TO_PICK, BH_T_PREPICK_TO_BOND};
enum eILC_STATE {ILC_WARM = 0, ILC_COOL};

// Default Axis name
#define BH_AXIS_Z						"BondHeadZAxis"
#define BH_AXIS_Z2						"BondHeadZAxis2"
#define BH_AXIS_T						"BondHeadTAxis"

#define BH_AXIS_EJ						"EjectorAxis"
#define BH_AXIS_EJ_X					"EjectorXAxis"
#define BH_AXIS_EJ_Y					"EjectorYAxis"
#define BH_AXIS_EJ_T					"EjectorTAxis"
#define BH_AXIS_EJELEVATOR				"EjElevatorAxis"
#define BH_AXIS_EJCAP					"EjCapAxis"

#define BH_AXIS_BIN_EJ					"BinEjectorAxis"
#define BH_AXIS_BIN_EJ_X				"BinEjectorXAxis"
#define BH_AXIS_BIN_EJ_Y				"BinEjectorYAxis"
#define BH_AXIS_BIN_EJ_T				"BinEjectorTAxis"
#define BH_AXIS_BIN_EJELEVATOR			"BinEjElevatorAxis"
#define BH_AXIS_BIN_EJCAP				"BinEjCapAxis"

#define BH_AXIS_NGPICK					"NGPickTAxis"
#define BH_AXIS_PROBER_Z				"ProberZAxis"
//v4.59A44	//MS50
#define BH_AXIS_MS50CHGCOLLET_T			"ChgColletTAxis"
#define BH_AXIS_MS50CHGCOLLET_Z			"ChgColletZAxis"

// I/O bit name
#define	BH_SI_BH_ARM_LIMIT				"iBHT_L"
#define BH_SI_MISSING_DIE				"iMissingDie"
#define BH_SI_MISSING_DIE_Z2			"iMissingDie2"
#define BH_SI_CONTACT					"iBHContact"
#define BH_SI_CONTACT_Z2				"iBH2Contact"
#define BH_SI_PRESSURE					"iPressureSensor"			
#define BH_SI_PRESSURE1					"iPressureSensor1"			//v4.59A12
#define BH_SI_PRESSURE2					"iPressureSensor2"			//v4.59A12	
#define BH_SI_PRESSURE_EJ				"iEjPressureSensor"			//v4.59A12
#define	BH_SI_VACUUM_FLOW				"iVacuumFlowSensor"

#define BH_SI_INPUT_IONIZER				"iBHInputIonizer"			//v4.59A12
#define BH_SI_OUTPUT_IONIZER			"iBHOutputIonizer"			//v4.59A12
#define BH_SI_CATCHANDBLOW_Y			"iBHCatchAndBlowYSnr"		//v4.59A12
#define BH_SI_ScopeLevel				"iwScopeLevel"				//v4.59A12

#define BH_SI_CleanColletSafetySensor	"iBHCleanColletSafetySensor"

#define MAIN_SI_FRONT_COVER				"iFrontCover"
#define MAIN_SI_FRONT_LEFT_DOOR_COVER	"iFrontLeftDoorCover"
#define MAIN_SI_FRONT_MIDDLE_DOOR_COVER	"iFrontMiddleDoorCover"
#define MAIN_SI_REAR_LEFT_DOOR_COVER	"iRearLeftDoorCover"
#define MAIN_SI_REAR_RIGHT_DOOR_COVER	"iRearRightDoorCover"
#define MAIN_SI_LEFT_DOOR_COVER			"iLeftDoorCover"

//CE Mark
#define MAIN_SI_FRONT_COVER2				"iFrontCover2"
#define MAIN_SI_FRONT_LEFT_DOOR_COVER2		"iFrontLeftDoorCover2"
#define MAIN_SI_FRONT_MIDDLE_DOOR_COVER2	"iFrontMiddleDoorCover2"
#define MAIN_SI_REAR_LEFT_DOOR_COVER2		"iRearLeftDoorCover2"
#define MAIN_SI_REAR_RIGHT_DOOR_COVER2		"iRearRightDoorCover2"
#define MAIN_SI_LEFT_DOOR_COVER2			"iLeftDoorCover2"
//#define BH_SI_COVER						"iCoverSensor"				//Also in BL class
//#define BH_SI_COVER2					"iCoverSensor2"				//v4.06		//CEMark	//Also in BL class
//#define BH_SI_FRIGHTCOVER				"iFRightCover"				// Front right cover // MS109
//#define BH_SI_FRIGHTCOVER2				"iFRightCover2"
//#define BH_SI_SIDECOVER					"ibElevatorCover"			//v4.10T5	//CEMark	//Also in BL class
//#define BH_SI_SIDECOVER2				"ibElevatorCover2"			//v4.10T5	//CEMark	//Also in BL class
//#define BH_SI_SIDEDOORCOVER				"iSideDoorCover"
//#define BH_SI_SIDEDOORCOVER2			"iSideDoorCover2"
//#define BH_SI_FRONT_LEFT_COVER			"iFLeftCover"
//#define BH_SI_FRONT_LEFT_COVER2			"iFLeftCover2"
//#define BH_SI_FRONT_COVER				"iFontCoverClose"
//#define BH_SI_SIDE_COVER				"iSideCoverClose"
#define BH_SI_BINH_EL_COVER				"iBinElevatorClose"
#define BH_SI_EMO						"ibEMO"						//v3.91
#define TC_SI_ALARM_BIT					"iTCAlarmSensor"
#define BH_SI_MS60_EJT_HOME				"iEJT_H"					//v4.55A6

#define MS60_BHZ_FAN_1					"BHZ_FAN_1"
#define MS60_BHZ_FAN_2					"BHZ_FAN_2"
#define MS60_BHT_UP_FAN_1				"BHT_UP_FAN_1"
#define MS60_BHT_UP_FAN_2				"BHT_UP_FAN_2"
#define MS60_BHT_LOW_FAN_1				"BHT_LOW_FAN_1"
#define MS60_BHT_LOW_FAN_2				"BHT_LOW_FAN_2"

#define BHZ1_ADC_MD						"BONDHEAD_ADC_PORT_Z1_MISSING_DIE"
#define BHZ2_ADC_MD						"BONDHEAD_ADC_PORT_Z2_MISSING_DIE"
#define BHT_THERMOSTAT					"BONDHEAD_ADC_PORT_T_THERMOSTAT"		//v4.49A5
#define BHZ_AIRFLOW_PORT				"IOMODULE_OUT_PORT_OUT_PORT3"			//v4.43T12

#define MS60_BH_MAX_TEMPERATURE_AUTOBOND	48//65		//in degree		//v4.53A19
#define MS60_BH_MAX_TEMPERATURE_IDLE		48//55		//in degree		//v4.53A19
#define MS60_BH_MIN_TEMPERATURE_PREHEAT		45		//in degree		//v4.53A19

#define MS60_BH_THERMALCHECK_CYCLE			990		//in degree		//v4.53A19
#define MS60_BH_THERMALCHECK_CYCLE_IDLE		51		//in degree		//v4.53A19


//v4.08	//MS100Plus
//#define BH_SI_BH_FAN					"iBHFan"
//#define BH_SI_DBH_FAN1				"ibBHFan1"					//v4.24		//BH Thermal Sensor 1 for MS100 9Inch
//#define BH_SI_DBH_FAN2				"ibBHFan2"					//v4.24		//BH Thermal Sensor 2 for MS100 9Inch
//v4.25		//MS100Pluse v2.2 
#define BH_SI_FRONT_THERMAL				"ibBHFrontThermal"
#define BH_SI_RIGHT_THERMAL				"ibBHRightThermal"

//v4.24T8	//ES101 NGPick module
#define NGPICK_SI_TENSIONCHECK			"ibTensionCheck"
#define NGPICK_SI_DN					"ibNGDn"
#define NGPICK_SI_UP					"ibNGUp"
#define NGPICK_SI_TAPEEMPTY				"ibTapeEmpty"
#define NGPICK_SO_UPDN					"oNGPickUpDn"
#define NGPICK_SO_REELSTOP				"oNGPickReelStop"

#define PB_CONTACT_SNR_1				"ibPbcs1"
#define PB_CONTACT_SNR_2				"ibPbcs2"
#define PB_CONTACT_SNR_3				"ibPbcs3"
#define PB_CONTACT_SNR_4				"ibPbcs4"
#define PB_CONTACT_SNR_5				"ibPbcs5"//M1MultiProbe
#define PB_CONTACT_SNR_6				"ibPbcs6"//M1MultiProbe
#define	PB_ADC_VALUE_CONTACT_1			"PROBER_ADC_PORT_F1"
#define	PB_ADC_VALUE_CONTACT_2			"PROBER_ADC_PORT_F2"
#define	PB_ADC_VALUE_CONTACT_3			"PROBER_ADC_PORT_F3"
#define	PB_ADC_VALUE_CONTACT_4			"PROBER_ADC_PORT_F4"
#define	PB_ADC_VALUE_CONTACT_5			"PROBER_ADC_PORT_F5"//M1MultiProbe
#define	PB_ADC_VALUE_CONTACT_6			"PROBER_ADC_PORT_F6"//M1MultiProbe

#define	PB_SO_CLEAN_PROBE_PIN			"obOpenCleanAir"
#define	BPR_SO_PPP_OPENCLOSE			"oPppOpenClose"


#define BH_SO_VACUUM					"oBHVacuum"
#define BH_SO_VACUUM_Z2					"oBHVacuum2"
#define BH_SO_EJR_CAP_VACUUM			"obEjectorCapVaccum"
#define BH_SO_STRONG_BLOW				"oBHStrongBlow"
#define BH_SO_STRONG_BLOW_Z2			"oBHStrongBlow2"
#define BH_SO_FRONT_COVER				"oFontCoverLock"
#define BH_SO_SIDE_COVER				"oSideCoverLock"
#define BH_SO_BIN_EL_COVER				"oBinElevatorLock"
#define BH_SO_CLEANCOLLET_POCKET		"obColletClean"				//v3.65	//Lexter
#define BH_SO_SUCKING_HEAD				"obSuckingHead"				//v4.05	//MS100 
#define BH_SO_BH_FAN					"obBondHeadFan"				//v4.09	//MS100Plus

#define MS60_SO_BH_FAN_RESET			"owBHFanReset"
#define MS60_SO_COLLET_CLAMP			"obBHColletClamp"			//v4.50A5
#define MS60_SO_COLLET_PUSHER			"obBHColletPusher"			//v4.50A5
#define MS60_SO_COLLET_UPLOAD_PUSHER	"obBHColletUploadPusher"
#define MS60_SO_COLLET_HOLDER_VACUUM	"obBHColletHolderVacuum"
#define CLEAN_COLLET_SO_HOLDER_STRONG_BLOW	"obBHColletHolderStrongBlow"


//v4.59A12	//Renesas MS90 
#define MS90_SO_CATCH_Y_SOL				"obCatchYSol"
#define MS90_SO_CATCH_Z_SOL				"obCatchZSol"
#define MS90_SO_BLOW_SOL				"obBlowSol"

// Motion profile
#define BH_MP_Z_NORMAL					"mpfBondHeadZNormal"
#define BH_MP_Z_OBW_DEFAULT				"mpfBondHeadZObwDefault"
#define BH_MP_Z_OBW_BOND_DEFAULT		"mpfBondHeadZObwDefault"	//"mpfBondHeadZObwBondDefault"	//"mpfBondHeadZObwDefault"	//"mpfBondHeadZObwBondDefault"
//#define BH_MP_Z_OBW_BOND_DEFAULT		"mpfBondHeadZObwBond"		//v4.36T1	//For MS100Plus slow BOND profile
#define BH_MP_Z_OBW_SETUP				"mpfBondHeadZObwSetup"
#define BH_MP_Z_OBW_UP					"mpfBondHeadZObwUp"			//v3.61

#define BH_MP_Z2_OBW_DEFAULT			"mpfBondHeadZ2ObwDefault"
#define BH_MP_Z2_OBW_BOND_DEFAULT		"mpfBondHeadZ2ObwDefault"
//#define BH_MP_Z2_OBW_BOND_DEFAULT		"mpfBondHeadZ2ObwBond"		//v4.36T1	//For MS100Plus slow BOND profile
#define BH_MP_Z2_OBW_SETUP				"mpfBondHeadZ2ObwSetup"
#define BH_MP_Z2_OBW_UP					"mpfBondHeadZ2ObwUp"		//v3.93T1

#define BH_MP_T_NORMAL					"mpfBondHeadTNormal"
#define BH_MP_T_CLEANCOLLET				"mpfBondHeadTCleanCollet"
#define BH_MP_T_OBW_DEFAULT				"mpfBondHeadTObwDefault"
#define BH_MP_T_OBW_PREPICK				"mpfBondHeadTObwPrePick"
#define BH_MP_T_OBW_SETUP				"mpfBondHeadTObwSetup"
#define BH_MP_T_OBW_CLEANCOLLET			"mpfBondHeadTObwCleanCollet"
#define BH_MP_T_ILC_CONTOUR				"mpfBondHeadTILCContour"
#define BH_MP_T_ILC_CONTOUR_SLOW		"mpfBondHeadTILCContourSlow"

//Ejector
#define BH_MP_E_NORMAL					"mpfEjectorNormal"
#define BH_MP_E_OBW_DEFAULT				"mpfEjectorObwDefault"
#define BH_MP_E_OBW_SETUP				"mpfEjectorObwSetup"
#define BH_MP_E_OBW_SLOW				"mpfEjectorObwSlow"
#define BH_MP_E_OBW_DOWN				"mpfEjectorObwDown"		
#define BH_MP_E_OBW_SLOW_DOWN			"mpfEjectorObwSlowDown"
//Ejector T
#define BH_MP_E_T_OBW_DEFAULT			"mpfEjectorTObwDefault"
#define BH_MP_E_T_OBW_DOWN				"mpfEjectorTObwDown"
//Ejector Cap
#define BH_MP_E_CAP_OBW_DEFAULT			"mpfEjectorCapObwDefault"
#define BH_MP_E_CAP_OBW_DOWN			"mpfEjectorCapObwDown"

//Bin Ejector
#define BH_MP_BIN_E_OBW_DEFAULT			"mpfBinEjectorObwDefault"
#define BH_MP_BIN_E_OBW_SLOW			"mpfBinEjectorObwSlow"
#define BH_MP_BIN_E_OBW_DOWN			"mpfBinEjectorObwDown"
#define BH_MP_BIN_E_OBW_SLOW_DOWN		"mpfBinEjectorObwSlowDown"
#define BH_MP_BIN_E_OBW_SETUP			"mpfBinEjectorObwSetup"
//Bin Ejector T
#define BH_MP_BIN_E_T_OBW_DEFAULT		"mpfBinEjectorTObwDefault"
#define BH_MP_BIN_E_T_OBW_DOWN			"mpfBinEjectorTObwDown"
//Bin Ejector Cap
#define BH_MP_BIN_E_CAP_OBW_DEFAULT		"mpfBinEjectorCapObwDefault"
#define BH_MP_BIN_E_CAP_OBW_DOWN		"mpfBinEjectorCapObwDown"


#define BH_MP_E_X_NORMAL				"mpfEjectorXNormal"
#define BH_MP_E_X_SLOW					"mpfEjectorXSlow"
#define BH_MP_E_Y_NORMAL				"mpfEjectorYNormal"
#define BH_MP_E_Y_SLOW					"mpfEjectorYSlow"

#define BH_MP_PROBERZ_NORMAL			"mpfProberZNormal"
#define BH_MP_PROBERZ_OBW_DEFAULT		"mpfProberZObwDefault"
#define BH_MP_PROBERZ_OBW_BOND_DEFAULT	"mpfProberZObwDefault"
//#define BH_MP_Z2_OBW_BOND_DEFAULT		"mpfBondHeadZ2ObwBond"
#define BH_MP_PROBERZ_OBW_SETUP			"mpfProberZObwSetup"
#define BH_MP_PROBERZ_OBW_SLOW			"mpfProberZObwSlow"

#define BH_MP_CHG_COLLET_T				"mpfChgColletT"
#define BH_MP_CHG_COLLET_Z				"mpfChgColletZ"

//Search profile
#define BH_SP_POS_LIMIT_T				"spfBondHeadTIdxLimit"
#define BH_SP_POS_LIMIT_SLOW_T			"spfBondHeadTIdxLimitSlow"
#define BH_SP_INDEX_T					"spfBondHeadTIdxOnly"
#define BH_SP_INDEX_SLOW_T				"spfBondHeadTIdxSlow"
#define BH_SP_SOFT_TOUCH_Z				"spfBondHeadZSoftTouch"
#define BH_SP_Z_SYNC_MOVE				"spfBondHeadSyncMove"
#define BH_SP_E_SYNC_MOVE				"spfEjectorSyncMove"
#define BH_SP_INDEXER_EJELZ				"spfEjectorElevatorIdx"
#define BH_SP_INDEX_EJECTOR_CAP			"spfEjectorCapIdx"
#define BH_SP_HOME_BIN_EJELZ			"spfBinEjectorElevatorHome"
#define BH_SP_HOME_BIN_EJECTOR_CAP		"spfBinEjectorCapHome"
#define BH_SP_HOME_EJELZ				"spfEjectorElevatorHome"
#define BH_SP_HOME_EJECTOR_CAP			"spfEjectorCapHome"

#define BH_SP_INDEX_PROBER_Z					"spfProberZIndex"
#define BH_SP_CONTACT_SRCH_FIX_DIST_PROBER_Z	"spfProberZContactSrchFixDist"
#define BH_SP_SETUP_CONTACT_SRCH_PROBER_Z		"spfProberZSetupContactSrch"
#define BH_SP_CONTACT_SRCH_PROBER_Z_NEW			"spfProberZContactSrchNew"


#define BH_SP_E_HOME_X							"spfEjectorXHome"
#define BH_SP_E_INDEX_X							"spfEjectorXIndex"
#define BH_SP_E_INDEX2_X						"spfEjectorXIndex2"
#define BH_SP_E_HOME_Y							"spfEjectorYHome"
#define BH_SP_E_INDEX_Y							"spfEjectorYIndex"
#define BH_SP_E_INDEX2_Y						"spfEjectorYIndex2"

// Control name
#define BH_STATIC_CONTROL_HOME_OBW_T	"ctrBondHeadHomeTObwSts"
#define BH_DYNAMIC_CONTROL_OBW_T		"ctrBondHeadTObwDyn"
#define BH_STATIC_CONTROL_OBW_T			"ctrBondHeadTObwSts"
#define BH_ILC_CONTROL_T				"ctrBondHeadTILC"

#define BH_DYNAMIC_CONTROL_OBW_Z		"ctrBondHeadZObwDyn"
#define BH_DYNAMIC_CONTROL_OBW_Z2		"ctrBondHeadZ2ObwDyn"
#define BH_STATIC_CONTROL_OBW_Z			"ctrBondHeadZObwSts"

#define BH_DYNAMIC_CONTROL_OBW_PROBER_Z	"ctrProberZObwDyn"
#define BH_DYNAMIC1_CONTROL_OBW_PROBER_Z "ctrProberZObwDyn1"
#define BH_STATIC_CONTROL_OBW_PROBER_Z	"ctrProberZObwSts"

#define EJ_DYNAMIC_CONTROL_OBW				"ctrEjectorObwDyn"
#define BIN_EJ_DYNAMIC_CONTROL_OBW			"ctrBinEjectorObwDyn"
#define EJ_THETA_DYNAMIC_CONTROL_OBW		"ctrEjectorThetaObwDyn"
#define BIN_EJ_THETA_DYNAMIC_CONTROL_OBW	"ctrBinEjectorThetaObwDyn"

#define EJCAP_DYNAMIC_CONTROL_OBW		"ctrEjectorCapObwDyn"

#define	BH_ACC_POCKET_TURN_ON		TRUE
#define	BH_ACC_POCKET_TURN_OFF		FALSE

#define CHANGE_EJ_STANDBY			1
#define CHANGE_EJ_CONTACT			2			//andrewng //2020-0603
#define EJ_PIN_LEARN_LEVEL			0
#define EJ_THETA_LEARN_LEVEL		1
#define EJ_CAP_LEARN_LEVEL			2
#define EJ_ELEVATOR_LEARN_LEVEL		3

// Error Code
#define Err_BhXMoveHome				1
#define Err_BhXLimit				2
#define Err_BhXMove					3
#define Err_BhXCommutate			4

#define Err_BhYMoveHome				5
#define Err_BhYLimit				6
#define Err_BhYMove					7
#define Err_BhYCommutate			8

#define Err_BhZMoveHome				9
#define Err_BhZNotSafe				10
#define Err_BhZMove					11
#define Err_BhZCommutate			12

#define Err_BhTMoveHome				13
#define Err_BhTNotSafe				14
#define Err_BhTMove					15
#define Err_BhTCommutate			16

#define Err_EjectorMoveHome			17
#define Err_EjectorNotSafe			18
#define Err_EjectorMove				19
#define Err_EjectorCommutate		20

#define Err_RotaryCollet0			21
#define Err_RotaryCollet180			22

#define Err_MissingDie				23
#define Err_MotionTimeoutX			24
#define Err_MotionTimeoutY			25
#define Err_MotionTimeoutZ			26
#define Err_MotionTimeoutT			27

#define Err_ColletClog				28

#define Err_ProberZMoveHome			29
#define Err_ProberZMove				30
#define Err_ProberZCommutate		31

#define Err_ChgColletTMoveHome		32
#define Err_ChgColletTNotSafe		33
#define Err_ChgColletTMove			34
#define Err_ChgColletTCommutate		35

#define Err_ChgColletZMoveHome		36
#define Err_ChgColletZNotSafe		37
#define Err_ChgColletZMove			38
#define Err_ChgColletZCommutate		39

#define Err_EjectorCapMoveHome		40
#define Err_EjectorCapNotSafe		41
#define Err_EjectorCapMove			42
#define Err_EjectorCapCommutate		43

#define Err_EjectorTMoveHome		44
#define Err_EjectorTNotSafe			45
#define Err_EjectorTMove			46
#define Err_EjectorTCommutate		47

// Bond Assy Profile
#define NORMAL_PROF					1
#define CLEAN_COLLET_PROF			2
#define PREPICK_PROF				3
#define SETUP_PROF					4

//Bondhead min. check collet jam wait time
#define BH_MIN_CHECK_COLLET_TIME	1000

#define	BH_WT_MAX_MOTION_TIME		80
#define	BH_WT_SPECIAL_MOVE_TIME		2000

//Bondhead Channel Type
#define BH_DC_SVO_MOTOR				7
#define BH_SVO_MOTOR				6
#define BH_STP_MOTOR				5

//BondArm Max Travel Limit
#define	BH_ARM_DEFAULT_LIMIT		125000

// Max wafer no recorded
#define BH_MAX_WAFER_NO				500

//Linear Ejector travel range (mm)
#define	BH_LEJT_TRAVEL_RANGE			0.15

#define BH_T_TRAVEL_RANGE_MS100_OFFSET		20000

#define BH_OPENDAC_CALIBRATION_RATIO		-10.0
#define BH_DACDISTANCE_CALIBRATION_RATIO	-0.0

#define BH_Z_MAX_HOMEOFFSET			4100	//Cree HuiZhou PkgSort	//v4.47A5

//	Bond Head Levels and BT titling for auto learn.
//#define	PB_MPL_GRID_ROWS			15
//#define	PB_MPL_GRID_COLS			15
#define	BH_BT_SAMPLE_POINTS			5	//	must less than above 15.
#define	BH_BT_TILTING_LIMIT			300

//Arm1/2 for MS100
#define BH_MS100_BHZ1				1
#define BH_MS100_BHZ2				2

#define BP_DATA						"BondProcess"
#define BP_DELAY					"BondDelay"
#define	BP_ONVACUUM_AT_PICK			"On Vacuum At Pick Die"
#define BP_PICKDELAY				"BP_PickDelay"
#define BP_APDELAY					"BP_ArmPickDelay"
#define BP_HPDELAY					"BP_HeadPickDelay"
#define BP_HPPDELAY					"BP_HeadPrePickDelay"
#define BP_EJUPDELAY				"BP_EjectorUpDelay"
#define BP_EJDOWNDELAY				"BP_EjectorDownDelay"
#define BP_BONDDELAY				"BP_BondDelay"
#define	BP_FLOATBLOWDELAY			"BP_FloatBlowDelay"
#define BP_ABDELAY					"BP_ArmBondDelay"
#define BP_HBDELAY					"BP_HeadBondDelay"
#define BP_BTDELAY					"BP_BinTableDelay"
#define BP_PRDELAY					"BP_PRDelay"
#define BP_WBOFFDELAY				"BP_WeakBlowOffDelay"
#define BP_HBTIME					"BP_HighBlowTime"
#define	BP_WTTDELAY					"BP_WTTDelay"
#define	BP_WTSETTLINGDELAY			"BP_WTSettlingDelay"
#define	BP_BTDELAY					"BP_BinTableDelay"
#define BP_MC_ALARM					"BP_MachineAlarm"
#define BP_DEFAULT					"BP_Default"
#define BP_SYNPICKDELAY				"BP_SyncPickDelay"

#define BP_EJECTOR					"BondEjector"
#define BP_BIN_EJECTOR				"BondBinEjector"
#define BP_EJSBLEVEL				"BP_StandbyLevel_Ej"
#define BP_EJUPLEVEL				"BP_EjectLevel_Ej"
#define BP_EJCTLEVEL				"BP_ContactLevel_Ej"
#define BP_EJ_UPDRIVEIN_EL			"BP_ELUpDriveIn_Ej"		//v4.59A40
#define BP_EJT_AUTOLEARN			"BP_Ejector use AutoLearn"
#define BP_EJT_UPDRIVEIN			"BP_Ejector Up DriveIn"
#define	BP_EJT_UP_LIMIT_TYPE		"BP_Ejector Up Limit Type"
#define BP_EJVACOFFELAY				"BP_EjVacOffDelay"
#define BP_DIEHEIGHT				"BP_DieHeight"
#define BP_EJCTLEVEL_OFFSET			"BP EjCTLevel Offset"

#define BP_EJECTOR_ELEVATOR			"BondEjectorElevator"
#define BP_EJECTOR_EL_UPLEVEL		"BP_UpLevel_EL"
#define BP_EJECTOR_EL_SBLEVEL		"BP_StandbyLevel_EL"
#define BP_EJECTOR_EL_DNOFFSET		"BP_DownOffset_EL"		//v4.59A40
#define BP_EJECTOR_ELV_SBLEVEL		"BP_StandbyLevel_ELV"
#define	BP_EJECTOR_ELV_UPLEVEL		"BP_UpLevel_ELV"
#define BP_EJECTOR_THETA_SBLEVEL	"BP_StandbyLevel_EJT"
#define	BP_EJECTOR_THETA_UPLEVEL	"BP_UpLevel_EJT"

#define BP_BIN_EJECTOR_SBLEVEL			"BP_StandbyLevel_BinEJ"
#define	BP_BIN_EJECTOR_UPLEVEL			"BP_UpLevel_BinEJ"
#define BP_BIN_EJECTOR_CAP_SBLEVEL		"BP_StandbyLevel_BinEJCap"
#define	BP_BIN_EJECTOR_CAP_UPLEVEL		"BP_UpLevel_BinEJCap"
#define BP_BIN_EJECTOR_ELV_SBLEVEL		"BP_StandbyLevel_BinELV"
#define	BP_BIN_EJECTOR_ELV_UPLEVEL		"BP_UpLevel_BinELV"
#define BP_BIN_EJECTOR_THETA_SBLEVEL	"BP_StandbyLevel_BinEJT"
#define	BP_BIN_EJECTOR_THETA_UPLEVEL	"BP_UpLevel_BinEJT"

#define BP_EJECTOR_TABLE			"BondEjectorTable"
#define EJT_ENABLE					"Enable Ejt XY"
#define BP_EJT_CDIE_POS_X			"EJT CDie Pos X"
#define BP_EJT_CDIE_POS_Y			"EJT CDie Pos Y"

#define BP_HEAD						"BondHead"
#define	BP_WAFFLE_PAD				"Waffle Pad"
#define	BHZ1_LEARNT					"BHZ1 Learnt"
#define	BHZ1_LEVEL					"BHZ1 Level"
#define	BHZ1_BT_X					"BHZ1 BT X"
#define	BHZ1_BT_Y					"BHZ1 BT Y"
#define	BHZ2_LEARNT					"BHZ2 Learnt"
#define	BHZ2_LEVEL					"BHZ2 Level"
#define	BHZ2_BT_X					"BHZ2 BT X"
#define	BHZ2_BT_Y					"BHZ2 BT Y"

#define	BP_Z1_PADSLEVEL_OFFSET_Z1	"Z1 Pads Bond Level Offset"
#define	BP_Z1_PADSLEVEL_OFFSET_Z2	"Z2 Pads Bond Level Offset"
#define	BP_Z1_BONDLEVEL_BT_X		"Z1 Learn Bond Level BT X"
#define	BP_Z1_BONDLEVEL_BT_Y		"Z1 Learn Bond Level BT Y"
#define	BP_Z2_BONDLEVEL_BT_X		"Z2 Learn Bond Level BT X"
#define	BP_Z2_BONDLEVEL_BT_Y		"Z2 Learn Bond Level BT Y"
#define	BP_Z1_BONDLEVEL_BT_R		"Z1 Learn Bond Level BT R"
#define	BP_Z2_BONDLEVEL_BT_R		"Z2 Learn Bond Level BT R"
#define BP_PREPICKPOS				"BP_PrePickPos_T"
#define BP_PICKPOS					"BP_PickPos_T"
#define BP_PREBONDPOS				"BP_PreBondPos_T"
#define BP_BONDPOS					"BP_BondPos_T"
#define BP_CCTPOS					"BP_CleanColletPos_T"
#define BP_CCRPOS					"BP_ChangeCarrierPos_T"
#define BP_PICKLEVEL				"BP_PickLevel_Z"
#define BP_BONDLEVEL				"BP_BondLevel_Z"
#define BP_REPLACELEVEL				"BP_ReplaceLevel_Z"
#define BP_SWINGOFFSET				"BP_SwingOffset_Z"
#define BP_PICKLEVEL_Z2				"BP_PickLevel_Z2"
#define BP_BONDLEVEL_Z2				"BP_BondLevel_Z2"
#define BP_REPLACELEVEL_Z2			"BP_ReplaceLevel_Z2"
#define BP_SWINGOFFSET_Z2			"BP_SwingOffset_Z2"
#define BP_CLEAN_COLLET_Z			"BP_AutoCleanCollet_Level_Z"
#define BP_CLEAN_COLLET_T			"BP_AutoCleanCollet_Level_T"
#define	BP_CLEAN_COLLET_OFFSET_Z	"BP_Clean_Level_Offset_Z"
#define BP_Z1_AUTO_LEARN_LEVEL		"BP_Z1_AutoLearn_Level"
#define BP_Z2_AUTO_LEARN_LEVEL		"BP_Z2_AutoLearn_Level"
#define BP_CLEAN_COLLET_AIRBLOW_LIMIT		"BP_AutoCleanCollet_Blow_Limit"
#define BP_CLEAN_COLLET_AUTOLEARNZ_LIMIT	"BP_AutoClean_AutoLearnZ_Limit"
#define BP_CLEAN_COLLET_AUTOLEARNZ_COUNT	"BP_AutoClean_AutoLearnZ_Count"
#define BP_CLEAN_COLLET_AUTOLEARNZ_OFFSET	"BP_AutoClean_AutoLearnZ_Offset"
#define BP_AUTO_CLEAN_AUTO_LEARN_Z			"BP_AutoClean_AutoLearnZ"	//v4.49A6	//WH SanAn
#define BP_LOW_BHTOLERANCE_LEVEL			"BP BH Tolerance Level"		//v4.44A4	//SanAn by Chris
#define	BP_Z_PB_LEVEL_OFFSET_LIMIT			"BP Z Pick Bond Level Offset Limit"
#define BP_BT2_OFFSET_Z				"BP BT2 Offset Z"			//v4.21		//MS100 9Inch dual-table config
#define BP_BT2_OFFSET_X				"BP BT2 Offset X"			//v4.37		//MS109
#define BP_BT2_OFFSET_Y				"BP BT2 Offset Y"			//v4.37		//MS109

#define BP_AUTO_CHG_COLLET				"BH Auto Change Collet"
#define BP_CLAMP_LEVEL_OFFSET_Z			"AGC Clamp Level OffsetZ"
#define BP_UPLOOK_PR_LEVEL_OFFSET_Z		"AGC Uplook PR Level OffsetZ"
#define BP_MS50_1ST_CPOS_T				"MS50 1st Collet T"
#define BP_MS50_TOTAL_COLLETS			"MS50 Total Collets"
#define BP_MS50_COLLET_INDEX			"MS50 Collet Index"

#define BP_C1_OFFSET_X_UM				"AGC C1 Offset X"		//v4.50A30
#define BP_C1_OFFSET_Y_UM				"AGC C1 Offset Y"
#define BP_C2_OFFSET_X_UM				"AGC C2 Offset X"
#define BP_C2_OFFSET_Y_UM				"AGC C2 Offset Y"

#define BP_PROBER						"Prober"
#define PB_PROBE_DELAY					"Probe Delay"
#define BP_PROBER_CONTACT_LEVEL_Z		"PB ContactLevel_Z"
#define BP_WAFER_FOCUS_LEVEL_Z			"PB FocusLevel_Z"
#define BP_PROBER_STANDBY_LEVEL_Z		"PB StandbyLevel_Z"
#define PB_STANDBY_LEVEL_OFFSET_Z		"PB StandbyLevelOffset Z"
#define PB_PROBER_USE_DYNAMIC_LEVEL		"PB Use dynamic level to probe"
#define PB_PROBER_USE_PROCESS_BLOCK		"PB Add Process Block in Motion"
#define PB_PROCESS_BLOCK_SAVE_ENCORDER	"PB Save Encorder in Process Block"
#define	PB_PROCESS_BLOCK_STOP_LIMIT		"PB Cycle Stop Limit"
#define PB_PROCESS_BLOCK_ABOUT_LEVEL	"PB Abort Level of Process Block"
#define PB_PROCESS_BLOCK_CHECK_CONTACT	"PB Check Contect Sensor in Process Block"
#define PB_PROCESS_BLOCK_CHECK_ABORT	"PB Check Contect Sensor in About LEVEL"
#define	PB_PPP_OPEN_DELAY				"CP Plus PPP Open Delay"
#define	PB_PPP_CLOSE_DELAY				"CP Plus PPP Close Delay"

//	Bond Head Collet and Ejector record
#define	BH_HW_RECORD						"BH HW Record"
#define	BH_HW_EJECTOR_OLD_REMARK			"BH HW Ejector Old Remark"
#define	BH_HW_EJECTOR_OLD_TYPE				"BH HW Ejector Old Type"
#define	BH_HW_COLLET_OLD_REMARK				"BH HW Collet Old Remark"
#define	BH_HW_COLLET_OLD_TYPE				"BH HW Collet Old Type"
#define	BH_HW_COLLET_1_OLD_MODE				"BH HW Collet 1 Old Mode"
#define	BH_HW_COLLET_2_OLD_MODE				"BH HW Collet 2 Old Mode"
#define	BH_HW_COLLET_SHIFT_NO				"BH HW Collet Shift No"

// prober
#define PB_PROCESS							"Prober Process"
#define PB_PROBER_SRCH_FIX_DIST				"PB Search Fix Dist"
#define PB_CONTACT_LEVEL_OFFSET				"PB Contact Level Offset"
#define PB_CONTACT_SRCH_DISTANCE			"PB Contact Search Distance"
#define PB_CONTACT_SRCH_VELOCITY			"PB Contact Search Velocity"
#define PB_CONTACT_SRCH_DRIVEIN_VELOCITY	"PB Contact Search Drive In Velocity"
#define PB_CONTACT_SRCH_DEBOUNCE			"PB Contact Search Debounce"
#define PB_CONTACT_SRCH_DRIVEIN_DISTANCE	"PB Contact Search Drive In Distance"
#define PB_USE_LEARN_PROBE_TABLE_LEVEL		"PB Use Learn Probe Table Level"
#define	PB_USE_MULTI_PROBE_LEVEL			"PB Use Multi Probe Level"
#define	PB_MULTI_LEVEL_GRID_ROWS			"PB Multi Level Grid Rows"
#define	PB_MULTI_LEVEL_GRID_COLS			"PB Multi Level Grid Cols"
#define	PB_LEARN_LEVEL_OFFSET				"PB Learn Level Offset"
#define	PB_REGION_LEVEL_TABLE				"PB Region Level Table"
#define	PB_MULTI_PL_STATE					"PB Multi Probe Level State"
#define	PB_MULTI_PL_TABLE					"PB Multi PL Table"
#define	PB_PROBE_FOCUS_LEVEL_Z				"PB Probe Focust Level Z"
#define	PB_PROBING_LEVEL_OFFSET_Z			"PB Probing Level Offset Z"
// prober
#define	BH_MULTI_BL_STATE_2					"BH Multi Bond Level State 2"
#define	BH_MULTI_BL_TABLE_2					"BH Multi BL Table 2"
#define	BH_MULTI_BL_UL_X					"BH Multi BL UL X"
#define	BH_MULTI_BL_UL_Y					"BH Multi BL UL Y"
#define	BH_MULTI_BL_LR_X					"BH Multi BL LR X"
#define	BH_MULTI_BL_LR_Y					"BH Multi BL LR Y"

#define BP_COMM_RETRY					"COMM Retry"
#define BP_COMM_INTERVAL				"COMM Interval"

#define BP_DAIRFLOW_RUNTIME_CHECK			"DAirFlow RunTime Check"
#define BP_Z1_DAIRFLOW_SETZERO_OFFSET		"BHZ1 DAirFlow SetZero Offset"
#define BP_Z2_DAIRFLOW_SETZERO_OFFSET		"BHZ2 DAirFlow SetZero Offset"
#define BP_Z1_DAIRFLOW_BLOCK_VALUE			"BHZ1 DAirFlow Block Value"
#define BP_Z1_DAIRFLOW_UNBLOCK_VALUE		"BHZ1 DAirFlow UnBlock Value"
#define BP_Z1_DAIRFLOW_THRESHOLD			"BHZ1 DAirFlow Threshold"
#define BP_Z1_DAIRFLOW_THRESHOLD_CJ			"BHZ1 DAirFlow Threshold CJ"
#define BP_Z2_DAIRFLOW_BLOCK_VALUE			"BHZ2 DAirFlow Block Value"
#define BP_Z2_DAIRFLOW_UNBLOCK_VALUE		"BHZ2 DAirFlow UnBlock Value"
#define BP_Z2_DAIRFLOW_THRESHOLD			"BHZ2 DAirFlow Threshold"
#define BP_Z2_DAIRFLOW_THRESHOLD_CJ			"BHZ2 DAirFlow Threshold CJ"

#define BP_Z1_THRESHOLD_PCT					"BHZ1 Threshold Pct"
#define BP_Z2_THRESHOLD_PCT					"BHZ2 Threshold Pct"
#define BP_Z1_THRESHOLD_LIMIT				"BHZ1 Threshold Limit"
#define BP_Z2_THRESHOLD_LIMIT				"BHZ2 Threshold Limit"

#define BP_Z1_THRESHOLD_UPPER_LIMIT			"BHZ1 Threshold Upper Limit"
#define BP_Z1_THRESHOLD_CLEAN__COLLET		"BHZ1 Threshold Clean Collet"
#define BP_Z2_THRESHOLD_CLEAN__COLLET		"BHZ2 Threshold Clean Collet"

#define BP_Z1_UPLOOK_CALIB_FACTOR			"BHZ1 Uplook Calib Factor"
#define BP_Z2_UPLOOK_CALIB_FACTOR			"BHZ2 Uplook Calib Factor"

#define BP_Z_OFFSET_PB_EMPTYCHECK			"BHZ PB EmptyCheck Offset Z"
#define BP_Z_AUTO_LEARN_DRIVEIN_LIMIT		"BHZ PB Auto Learn Drive In Limit"

//ES101
#define NGPICK						"NGPickT"
#define NG_INDEX_T					"NG Index T"

#define BP_AUTOLEARN				"BP_Head use AutoLearn"
#define BP_CTSENSOR_LEARN			"BP_CTSensor_Learn"
#define BP_PICKDRIVEIN				"BP_Head Pick DriveIn"
#define BP_BONDDRIVEIN				"BP_Head Bond DriveIn"
#define BP_CHECK_MISSDIE			"BP_Check Missing Die"
#define BP_CHECK_COLLETJAM			"BP_Check Collet Jam"
#define BP_CHECK_COVERSNR			"BP_Check Cover Sensor"
#define	BP_FRONT_COVER_LOCK			"BP_lock Front Cover"
#define	BP_SIDE_COVER_LOCK			"BP_lock Side Cover"
#define BP_BIN_EL_COVER_LOCK		"BP_lock Bin Elevator Cover"
#define	BP_COVER_SENSOR_ON			"BP Cover Sensor On"		//v4.28T5
#define BP_SCAN_CHECK_COVERSNR		"BP Scan Check Cover Sensor"
#define BP_CHECK_BH1_PRESSURE		"BP Check BH1 Pressure Snr"
#define BP_CHECK_BH2_PRESSURE		"BP Check BH2 Pressure Snr"
#define BP_CHECK_EJ_PRESSURE		"BP Check EJ Pressure Snr"

#define BP_SYNC_MOVE				"Sync Move"
#define BP_SYNC_MOVE_ENABLE			"Enable"
#define BP_SYNC_MOVE_DIST			"Distance"
#define BP_SYNC_MOVE_SPEED			"Speed"

#define BP_COUNT								"BondCount"
#define	BP_MAXEJECTOR							"BP_MaxEjectorCount"
#define	BP_MAXCOLLET							"BP_MaxColletCount"
#define	BP_MAXCOLLET2							"BP_MaxColletCount2"
#define	BP_MAXCLEAN								"BP_MaxCleanCount"
#define	BP_CLEAN_COLLET_TIME_OUT				"BP_Clean Collet Time Out"
#define	BP_MAXSKIP								"BP_MaxSkipCount"
#define BP_AUTOCLEAN_REPLACE_COUNT				"AutoClean Replace Count"

#define	BP_EJ_KOFFSET							"BP_Ejector_KOffset"
#define	BP_EJ_KCOUNT							"BP_Ejector_KCount"
#define	BP_EJ_KOFFSET_LIMIT						"BP_Ejector_KOffset_Limit"
#define	BP_EJ_SUBREGION_KOFFSET					"BP_Ej_SubRegion_KOffset"
#define	BP_EJ_SUBREGION_KCOUNT					"BP_Ej_SubRegion_KCount"
#define	BP_EJ_SUBREGION_SKOFFSET				"BP_Ej_SubRegion_SKOffset"
#define	BP_EJ_SUBREGION_SKCOUNT					"BP_Ej_SubRegion_SKCount"
#define	BP_Z1_SKOFFSET							"BP_Z1_SKOffset"
#define	BP_Z1_SKCOUNT							"BP_Z1_SKCount"
#define	BP_Z2_SKOFFSET							"BP_Z2_SKOffset"
#define	BP_Z2_SKCOUNT							"BP_Z2_SKCount"
#define BP_EJ_KOFFSET_BHZ1_MD					"BP_Ejector_KOffset_BHZ1MD"
#define BP_EJ_KOFFSET_BHZ2_MD					"BP_Ejector_KOffset_BHZ2MD"
#define BP_EJ_KOFFSET_INC_VALUE					"BP_Ej_KOffset_BHZ_INCVALUE"
#define	BP_DAF_BH1_KOFFSET						"BP_DAF_BH1_KOffset"
#define	BP_DAF_BH1_KCOUNT						"BP_DAF_BH1_KCount"
#define	BP_DAF_BH1_KOFFSET_LIMIT				"BP_DAF_BH1_KOffset_Limit"
#define	BP_DAF_BH2_KOFFSET						"BP_DAF_BH2_KOffset"
#define	BP_DAF_BH2_KCOUNT						"BP_DAF_BH2_KCount"
#define	BP_DAF_BH2_KOFFSET_LIMIT				"BP_DAF_BH2_KOffset_Limit"

#define BP_BHZ_AUTO_CLEANCOLLET_LIMIT			"BP AutoCleanCollet Limit"
#define BP_BHZ_AUTO_CLEANCOLLET_REPLACE_LIMIT	"AutoCleanCollet Replace Limit"
#define BP_BHZ_AUTO_CLEANCOLLET_SWING_TIME		"AutoCleanCollet Swing Time"
#define BP_BHZ_ACC_LAST_DROP_DOWN_TIME			"AutoCleanCollet Last Drop Down Time"
#define BP_EJ_KOFFSET_RESET_WAFEREND			"Ej KOffset Reset WaferEnd"
#define	BP_BHZ_ACC_LIQUID_EXPIRE_TIME			"AutoCleanCollet Liquid Expire Time"

#define	BP_MAX_CYCLE_MD_LIMIT		"BP_Missing Die Cycle Limit"
#define	BP_MAX_MISSDIE_RETRY		"BP_Missing Die Retry Count"
#define	BP_MAX_COLLETJAM_RETRY		"BP_Collet Jam Retry Count"
#define BP_AUTO_CLEAN_COLLET		"Auto Clean Collet"
#define BP_ACC_SEARCH_COLLET_HOLE	"Auto Clean Collet Search Collet"
#define BP_VAC_NEUTRAL_STATE		"VAC Neutral State"
#define BP_PR_2DCODE				"BH PR 2D Barcode"
#define BP_PR_2DCODE_LIMIT			"BH PR 2D Barcode Limit"
#define BP_MD_RETRY_AUTOCLEAN		"BH MD Retry Use AutoClean"
#define BP_MAX_UPLOOK_PR_FAIL_LIMIT	"BH Uplook PR Fail Limit"

#define BP_CUSTOM_COUNT_1			"Custom Counter 1"
#define BP_CUSTOM_COUNT_2			"Custom Counter 2"
#define BP_CUSTOM_COUNT_3			"Custom Counter 3"

#define BP_CUSTOM_COUNT_1_LIMIT		"Customer Counter 1 Limit"
#define BP_CUSTOM_COUNT_2_LIMIT		"Customer Counter 2 Limit"
#define BP_CUSTOM_COUNT_3_LIMIT		"Customer Counter 3 Limit"

#define BP_Z_UBH_BJL_NO				HP_MIN_PRESETBJL_NO

#define	BP_UBHPREEJUPDELAY			"BP_UBH PreEjectorUpDelay"
#define BP_UBHSTEPMOVESTEP			"BP_UBH StepMoveStep"
#define BP_UBHPICKLEVELDACZ			"BP_UBH PickLevelDacZ"
#define BP_UBHOPENDACSTEPSIZE		"BP_UBH OpenDacStepSize"
#define	BP_UBHOPENDACMAXDAC			"BP_UBH OpenDacMaxDac"
#define BP_UBHSEARCHTRIGGERERR		"BP_UBH Search Trgger Pos Err"
#define BP_UBHSEARCHSPEED			"BP_UBH Search Speed"
#define BP_UBHDRIVEINSPEED			"BP_UBH Drive In Speed"
#define BP_UBHDRIVEINSAMPLE			"BP_UBH Drive In Sample"
#define BP_UBHMINSEARCHRIGGERSAMPLE	"BP_UBH Search Min Trigger Sample"
#define BP_UBHOPENDACOFFSET			"BP_UBH OpenDacOffset"	
#define BP_UBHSEPARATESETTLING_T	"BP_UBH SeparateSettlingT"
#define BP_UBHSETTLINGTIME_T		"BP_UBH SettlingTimeT"

#define BP_DEFAULT					"BP_Default"

#define BP_BHZ_OPEN_DAC_ENABLE		"BP_HEAD BHZ Open Dac Enable"
#define BP_Z1_PICK_DAC				"BP_HEAD Z1 Pick Side DAC"
#define BP_Z1_BOND_DAC				"BP_HEAD Z1 Bond Side DAC"
#define BP_Z2_PICK_DAC				"BP_HEAD Z2 Pick Side DAC"
#define BP_Z2_BOND_DAC				"BP_HEAD Z2 Bond Side DAC"
#define BP_Z1_PICK_DAC_OFFSET		"BP_HEAD Z1 Pick Side DAC Offset"
#define BP_Z1_BOND_DAC_OFFSET		"BP_HEAD Z1 Bond Side DAC Offset"
#define BP_Z2_PICK_DAC_OFFSET		"BP_HEAD Z2 Pick Side DAC Offset"
#define BP_Z2_BOND_DAC_OFFSET		"BP_HEAD Z2 Bond Side DAC Offset"

#define BP_Z_ENABLE_SYNC_MOTION_VIA_EJ_CMD	"BP_HEAD Z Enable Sync Motion Via Ej Cmd"
#define BP_Z_SYNC_TRIGGER_VALUE				"BP_HEAD Z Sync Trigger Value"
#define BP_Z_SYNC_Z_OFFSET					"BP_HEAD Z Sync Z Offset"
#define BP_Z_SOFT_TOUCH_PICK_DISTANCE		"BP_HEAD Soft Touch Pick Distance"
#define BP_Z_SOFT_TOUCH_BOND_DISTANCE		"BP_HEAD Soft Touch Bond Distance"
#define BP_Z_SOFT_TOUCH_VELOCITY			"BP_HEAD Soft Touch Velocity"

#define BP_Z1_OPEN_DAC_CALIB_RATIO	"BP_HEAD Z1 Open Dac Calibration Ratio"
#define BP_Z2_OPEN_DAC_CALIB_RATIO	"BP_HEAD Z2 Open Dac Calibration Ratio"
#define BP_Z1_DAC_DIST_CALIB_RATIO	"BP_HEAD Z1 Dac Distance Calibration Ratio"
#define BP_Z2_DAC_DIST_CALIB_RATIO	"BP_HEAD Z2 Dac Distance Calibration Ratio"

#define BP_T_ILC_RESTART_UPDATE_MIN_TIME	"BP_RESTART_UPDATE_MIN_TIME"

#define	BP_EJ_SOFT_TOUCH					"BP Ejector Soft Touch"
#define	BP_EJCAP_SOFT_TOUCH					"BP EjCap Soft Touch"

#define BH_DATA						"Bond Head Data"
#define BH_DELAY_SETTING			"Bond Head Delay Setting"
#define BH_PICKING_PROCESS			"Bond Head Picking Process"
#define BH_ALARM_SETTING			"Bond Head ALARM SETTING"
#define	BH_PICK_DELAY				"Pick Delay"
#define	BH_BOND_DELAY				"Bond Delay"
#define	BH_ARM_PICK_DELAY			"Arm Pick Delay"
#define	BH_ARM_BOND_DELAY			"Arm Bond Delay"
#define	BH_HEAD_PICK_DELAY			"Head Pick Delay"
#define	BH_HEAD_BOND_DELAY			"Head Bond Delay"
#define	BH_HEAD_PREPICK_DELAY		"Head Prepick Delay"
#define	BH_PR_DELAY					"PR Delay"
#define	BH_EJECTOR_UP_DELAY			"Ejector up Delay"
#define	BH_EJECTOR_DOWN_DELAY		"Ejector Down Delay"
#define	BH_ENABLE_SYNC_PICK			"Enable Sync Pick"
#define	BH_TRIGGER_LEVEL			"Trigger level"
#define	BH_PRELOAD_LEVEL			"Preload level"
#define	BH_PICK_LEVEL_DRIVE_IN		"Pick level Drive-In"
#define	BH_BOND_LEVEL_DRIVE_IN		"Bond level Drive-In"
#define	BH_EJECTOR_SPEED			"Ejector speed"
#define	BH_ENABLE_BT_LEVELING		"Enable BT leveling"
#define	BH_ENABLE_MD_ALARM			"Enable MD alarm"
#define	BH_MD_RETRY					"MD retry"
#define	BH_ENABLE_CJ_ALARM			"Enable CJ alarm"
#define	BH_CJ_RETRY					"CJ retry"

// prober
#define PB_COUNT						"Probe Count"
#define	PB_MAX_PROBEPIN_COUNT			"PB Max Probe Pin Count"
#define	PB_MAX_CLEAN_PIN_COUNT			"PB Max Clean Probe Pin Count"
#define PB_MAX_PROBE_FAIL_LIMIT			"PB Max ProbeFailLimit"
#define PB_MAX_CONTACT_FAIL_LIMIT		"PB Max Probe Contact Limit"
#define PB_MAX_CTCT_CON_FAIL_LIMIT		"PB Max Probe Contact Continue Limit"
#define PB_CHECK_FIRST_PROBE_POSN		"PB Check First Probe Pos"
#define PB_CHECK_PROBE_CONTACT_SNR		"PB Check Probe Contact Snr"
#define	PB_MAX_PRB_MARK_CHECK_LIMIT		"PB Max Probe Mark Check Limit"

#define PB_NUMBER_OF_PIN				"PB number of pin using"
#define	PB_ADC_READ_CONTACT_SENSOR	"PB Read ADC Value for Contact Sensor"
#define PB_RECHECK_CYCLE_NUMBER		"PB Recheck Cycle Number"
#define PB_SENSOR_CHECK_TORLANCE	"PB Sensor Check Torlance"
#define	PB_IS_ENABLE_CHECKING		"PB Enable Cycle-Checking"

#define PB_CONTACT_LEVEL_RANGE		"PB Contact Level Range"
#define PB_IS_CHECK_CONTACT_RANGE	"PB Is Check Contact Range"

#define	PB_REPROBE_LIMIT			"PB ReProbe Limit"
#define	PB_REPROBE_VIA_VF			"PB ReProbe Via VF"
#define	PB_REPROBE_VIA_IR			"PB ReProbe Via IR"

// prober
#define	CP_DATA						"CP100 Data"
#define	CP_ACPP						"Auto Clean Probe Pin"
#define	CP_ENABLE					"ACP Enable"
#define	CP_ASK_ADD_LIQUID			"Ask to add liquid"
#define	CP_POINT_LIFE_LIMIT			"Point Life Limit"
#define	CP_POINT_CYCLE				"Point Clean Cycle"
#define	CP_USE_POINTS				"Clean Use Points"
#define	CP_DRIVE_IN_DIST			"Up Drive In Dist"
#define	CP_BRUSH_DIST_X				"Brush Dist X"
#define	CP_BRUSH_DIST_Y				"Brush Dist Y"
#define	CP_PIN_UP_DELAY				"Pin Up Delay"
#define	CP_PIN_DOWN_DELAY			"Pin Down Delay"
#define	CP_EVAPORATE_DELAY			"Evaporate Delay"
#define	CP_POINT_INDEX				"Point Index"
#define	CP_TOTAL_POINTS				"Total Points"
#define	CP_POINT_COUNT				"Point Count"
#define	CP_CLOTH_EXPIRED			"Clean Cloth Expired"
#define	CP_MULTI_TESTERS_PROBING	"Multi Testers Probing"

//v3.97
#define MS100_MODE3_MIN_ARMPICK_DELAY		8		//in ms
#define MS100_MODE3_MIN_HEADPICK_DELAY		28		//in ms
#define MS100_MODE3_MIN_HEADBOND_DELAY		30		//in ms
#define MS100_MODE3_MIN_EJUP_DELAY			10		//in ms

#define PROBE_TABLE_LEARN_POINT_NUM			4

#define	BA_CONTOUR_MOVE_BUF_SIZE			4096

#define BA_ILC_CONVERGE			1
#define	BA_ILC_NOT_CONVERGE		-1

#define BA_ILC_DATALOG_OFFSET	0

#define BA_ILC_PTB_PROFILE_PATH		gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PTB_Profile.txt")

#define BA_ILC_BTP_PROFILE_PATH		gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\BTP_Profile.txt")
#define BA_ILC_PREPTP_PROFILE_PATH	gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PrePTP_Profile.txt")
#define BA_ILC_PREPTB_PROFILE_PATH	gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PrePTB_Profile.txt")

#define BA_ILC_PTB_PVAJ_CONVERGED_PATH		gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PTB_PVAJ_Converged.txt")
#define BA_ILC_BTP_PVAJ_CONVERGED_PATH		gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\BTP_PVAJ_Converged.txt")
#define BA_ILC_PREPTP_PVAJ_CONVERGED_PATH	gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PrePTP_PVAJ_Converged.txt")
#define BA_ILC_PREPTB_PVAJ_CONVERGED_PATH	gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PrePTB_PVAJ_Converged.txt")

#define BA_ILC_PTB_OUTPUT_PATH		gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PTB_Output.txt")
#define BA_ILC_BTP_OUTPUT_PATH		gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\BTP_Output.txt")
#define BA_ILC_PREPTP_OUTPUT_PATH	gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PrePTP_Output.txt")
#define BA_ILC_PREPTB_OUTPUT_PATH	gszROOT_DIRECTORY + _T("\\Exe\\NuData\\ILC\\PrePTB_PVAJ_Output.txt")

#define AGC_COLLET_INDEX			"AGC Collet Index"




