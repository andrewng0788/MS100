//////////////////////////////////////////////////////////////////////////////
//	MS_SecCommConstant.h : Defines the Constant header
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wednesday, Nov 16, 2005
//	Revision:	1.00
//
//	By:			Barry Chu
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once
//** For SEC/GEM Constants for MS896aStn base class member fcns


#define SG_CEID_PROCESSING_STATE_STARTED        5
#define SG_CEID_PROCESSING_STATE_COMPLETED      6
#define SG_CEID_PROCESSING_STATE_STOPPED		7
#define SG_CEID_PROCESSING_STATE_CHANGE         8

//Collecting event ID
//#define AD899_SECS_CEID_EQUIPMENT_OFF_LINE		1
//#define AD899_SECS_CEID_CONTROL_STATE_LOCAL		2
//#define AD899_SECS_CEID_CONTROL_STATE_REMOTE		3
//#define AD899_SECS_CEID_PROCESSING_STATE_CHANGE	8
//#define AD899_SECS_CEID_ALARM_DETECTED			9
//#define AD899_SECS_CEID_ALARM_CLEARED				10
//#define AD899_SECS_CEID_E10_STATE					100

#define SG_CEID_ALARM_DETECTED						9		//v4.59A25		//Osram
#define SG_CEID_PROCESS_PROGRAM_SELECTED			12		//v4.59A33		//EM
#define	SG_CEID_CONTROL_STATE_CHANGE				50		//OsramTrip 8/22
#define SG_CEID_EQUIPMENT_E10_STATECHANGE			100		//E-10 State Change	//EquipMgr
#define SG_CEID_EQUIPMENT_BASIC_STATISTIC			101		//v4.31T9		//EquipMgr
#define SG_CEID_EQUIPMENT_MANAGEMENT_INFO			102		//v4.31T9		//EquipMgr
#define SG_CEID_MC_WARMSTART_COMPLETED				103		//OsramTrip 8/22
#define SG_CEID_ALARM_TOWER_SIGNAL					105		//OsramTrip 8/22
#define SG_CEID_EQUIPMENT_STATISTIC_DATA_EVENT		253		//v4.37T5		//EquipMgr
#define SG_CEID_FRAME_LOAD_RESPONSE_EVENT			8020	//Autoline
#define SG_CEID_FRAME_UNLOAD_RESPONSE_EVENT			8021	//Autoline


#define SG_CEID_AB_START            7000
#define SG_CEID_AB_STOP             7001
#define SG_CEID_WL_SCAN_START       7002
#define SG_CEID_WL_SCAN_DONE        7010
#define SG_CEID_BL_SCAN_START       7003
#define SG_CEID_BL_SCAN_DONE        7011
#define SG_CEID_WL_SLOT_REPORT      7004
#define SG_CEID_BL_SLOT_REPORT      7005
#define SG_CEID_BIN_FULL            7006
#define SG_CEID_BIN_LOADED          7007
#define SG_CEID_START_STOP			7017
#define SG_CEID_RESET_SLOT			7018

#define SG_CEID_BL_GRIPPER          7050
#define SG_CEID_BL_MAGAZINE         7051
#define SG_CEID_BL_BARCODE          7052
#define SG_CEID_BL_OMRUNTIME        7053

#define SG_CEID_WL_GRIPPER_MGZN     7100
#define SG_CEID_WL_SETUP            7101
#define SG_CEID_WL_WTUNLOADPOSN     7102
#define SG_CEID_WL_HOMEDIETABPOSN   7103

#define SG_CEID_BP_LEARNDIE         7150
#define SG_CEID_BP_SEARCHPARAM      7151
#define SG_CEID_BP_POSTBOND         7152

#define SG_CEID_EQ_STATUS           7200

#define SG_CEID_WP_LEARNDIE         7300
#define SG_CEID_WP_SEARCHPARAM      7301
#define SG_CEID_WP_DEBUG            7302
#define SG_CEID_WP_AUTOBOND         7303

#define SG_CEID_BT_SETUP            7400
#define SG_CEID_BT_PHYSETUP         7401
#define SG_CEID_BT_BINSETUP         7402
#define SG_CEID_BT_COUNTER          7403
#define SG_CEID_BT_OUTPUT           7404

#define SG_CEID_WT_WAFERSIZE        7500
#define SG_CEID_WT_CALIBRATE        7501
#define SG_CEID_WT_UNLOADPOSN       7502
#define SG_CEID_WT_BACKUPMAP        7503
#define SG_CEID_WT_SCNRESULT        7504
#define SG_CEID_WT_AUTOBOND         7505

#define SG_CEID_BA_POSN             7600
#define SG_CEID_BH_LEVEL            7601
#define SG_CEID_EJ_POSN             7602
#define SG_CEID_EJ_OFFSET           7603
#define SG_CEID_BH_DELAY            7604

#define SG_CEID_AB_OPTION           7700
#define SG_CEID_AB_TOOLLIFE         7701
#define SG_CEID_AB_COUNTER          7702
#define SG_CEID_AB_WFCOUNTER        7703
#define SG_CEID_AB_RUNTIME          7704

#define SG_CEID_StartAlignment		8000
//ANDREW_SC
#define SG_CEID_CassetteMapped		8001
#define SG_CEID_LoadCassette		8002
#define SG_CEID_UnloadCassette		8003
#define SG_CEID_WfrCassUnload		8004
#define SG_CEID_WfrCassLoad			8005
#define SG_CEID_WfrTableUnload		8006
#define SG_CEID_WfrTableLoad		8007
#define SG_CEID_FrmMtCassUnload		8008
#define SG_CEID_FrmMtCassLoad		8009
#define SG_CEID_FrmBinTableLoad		8010
#define SG_CEID_FrmBinTableUnload	8011
#define SG_CEID_FrmCmpltCassLoad	8012
#define SG_CEID_FrmCmpltCassUnload	8013
#define SG_CEID_FrmWipCassLoad		8014
#define SG_CEID_FrmWipCassUnload	8015
#define SG_CEID_StartBond			8016
#define SG_CEID_WaferCompleted		8017
#define SG_CEID_ClearBin			8018
#define SG_CEID_StopBond			8019
#define SG_CEID_ManualClearBin		8020		//v4.59A19	//Osram Penang
#define SG_CEID_PostBondCpkRpt		8021		//OsramTrip 8/22
#define SG_CEID_ManualClearBin_AUTOLINE		8022		
#define SG_CEID_PostBondCpkRpt_AUTOLINE		8023
#define SG_CEID_BinTransfer_AUTOLINE		8024
#define SG_CEID_WaferMapSelectedGrade_AUTOLINE		8025
#define SG_CEID_BarcodeReject_AUTOLINE	8026
//#define SG_CEID_WaferMapCurPickingGrade_AUTOLINE	8026


//v4.56A11
#define MS_SECS_SV_CURR_RECIPE_NAME			17		//_T("PPExecName")				//17

//Macros for Status Variable List (SV)		//v4.3T9	//EquipMgr
#define MS_SECS_SV_E10_STATE				100		//_T("E-10 State")				//100
#define MS_SECS_SV_UNIT_PER_HOUR			101		//_T("Actual Unit Per Hour")		//101
#define MS_SECS_SV_EQUIP_MTBA				102		//_T("Equipment MTBA")			//102
#define MS_SECS_SV_EQUIP_MTBF				103		//_T("Equipment MTBF")			//103
#define MS_SECS_SV_TOTAL_DOWN_TIME			104		//_T("Total Down Time")			//104
#define MS_SECS_SV_TOTAL_IDLE_TIME			105		//_T("Total Idle Time")			//105
#define MS_SECS_SV_TOTAL_PRODUCTIVE_TIME	106		//_T("Total Productive Time")		//106
#define MS_SECS_SV_NO_OF_FAILURE			107		//_T("No Of Failure")				//107
#define MS_SECS_SV_NO_OF_ASSIST				108		//_T("No Of Assist")				//108
#define MS_SECS_SV_STATISTIC_START_DATE		109		//_T("Statistic Start Data")		//109
#define MS_SECS_SV_MACHINE_ID				110		//_T("Machine ID")				//110
#define MS_SECS_SV_LAST_PM_DATE				111		//_T("Last PM Date")				//111
#define MS_SECS_SV_INSTALLATION_DATE		112		//_T("Installation Date")			//112
#define MS_SECS_SV_RECALL_STAT_START_DATE	113		//_T("Recall Statistic Start Date")	//113	//EM
#define MS_SECS_SV_RECALL_STAT_END_DATE		114		//_T("Recall Statistic End Date")		//114	//EM

#define MS_SECS_SV_TOOL_TYPE				1003	//_T("ToolType")
#define MS_SECS_SV_TOOL_ID					1004	//_T("ToolID")

#define MS_SECS_SV_WAFER_BARCODE			1200	//_T("WaferBarCode")
#define MS_SECS_SV_FRAME_ID					1201	//_T("FrameID")
#define MS_SECS_SV_WAFER_BIN_MAP_FILE_NAME	1202	//_T("WaferBinMapFileName")
#define MS_SECS_SV_WAFER_BIN_MAP_FILE_PATH	1203	//_T("WaferBinMapFilePath")

#define MS_SECS_SV_ES_INIBODY				1530	//_T("ES_INIBODY")			//1530	//EM


#define MS_SECS_SV_WAFER_ID					3003	//_T("WaferId")
#define MS_SECS_SV_SLOT_NO					3004	//_T("SlotNo")
#define MS_SECS_SV_CASSETTE_POS				3005	//_T("CassettePos")
#define MS_SECS_SV_CASSETTE_TYPE			3006	//_T("CassetteType")
#define MS_SECS_SV_MAP_LOAD_TIME			3007	//_T("MapLoadTime")
#define MS_SECS_SV_PRE_SLOT_NO				3008	//_T("PreSlotNo")
#define MS_SECS_SV_BT_BIN_NUMBER			3038	//_T("Binnumber")				//3038
#define MS_SECS_SV_BT_BIN_NAME				3039	//_T("BinName")					//3039
#define MS_SECS_SV_TOTAL_BIN_TABLE_DIE_ON_WAFER	3040	//_T("TotalBinableDieOnWafer")	//3040
#define MS_SECS_SV_TOTAL_DIE_BINED_FROM_WAFER	3041	//_T("TotalDieBinedFromWafer")	//3041
#define MS_SECS_SV_BL_FRAME_ID				3042	//_T("FrameId")					//3042
#define MS_SECS_SV_BT_BIN_DIE_COUNT			3043	//_T("BindieCount")				//3043
#define MS_SECS_SV_BT_BIN_DATE_TIME			3047	//_T("DateTimeFrameWasBorn")	//3047

//OsramTrip 8/22
#define MS_SECS_SV_BPR_PB_CPK_INFO			_T("PostBond CPK Info")			//2999

#define MS_SECS_SV_BIN_MAP_FILE_PATH		3036	//_T("BinMapFilePath")			//3036
#define MS_SECS_SV_BIN_MAP_FILE_NAME		3037	//_T("BinMapFileName")			//3037

//AUTOLINE 
#define MS_SECS_DV_AL_REPORT_ID				_T("AL_ReportID")				//3060
#define MS_SECS_DV_AL_FRAME_TYPE			_T("AL_szFrameType")			//3061
#define MS_SECS_DV_AL_SLOT_ID				_T("AL_szSlotID")				//3062
#define MS_SECS_DV_AL_LOADUNLOAD_STATUS		_T("AL_szLoadUnloadStatus")		//3063

#define MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_LIST				3009 //_T("WaferMapSelectedGradeList") //3009
#define MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIST		3010 //_T("WaferMapSelectedGradeDieCountList") //3010
#define MS_SECS_SV_WAFER_MAP_SELECTED_GRADE_DIE_COUNT_LIMIT		3064 //_T("WafermapSelectedGradeDieCountLimit") //3064
#define MS_SECS_SV_WAFER_MAP_SORTING_GRADE						3065 //_T("WafermapSortingGrade") //3065
#define MS_SECS_SV_FRAME_TYPE									3066 //_T("FrameType") //3066
#define MS_SECS_SV_REJECT_CODE									3067 //_T("RejectCode") //3067

#define MS_SECS_SV_START_STOP_PRESSED							5031 //_T("Stop Pressed") //5031
#define MS_SECS_SV_RESET_SLOT									5032
/*
AB_lScnResultRow			2861
AB_lScnResultCol			2862

WPR_lNormDiePitchX			2901
WPR_lNormDiePitchY			2902
WPR_lNormDieSizeX			2903
WPR_lNormDieSizeY			2904
WPR_lNormDieLearnt			2905
WPR_lNormDieSearchID		2906
WPR_lNormDieCalib			2907
WPR_lNormDiePitch			2908
WPR_lNormDieFOVSize			2909
WPR_lDieType				2911
WPR_lDieID					2912
WPR_lDieAlignAlgorithm		2913
WPR_lDieInpsectionMethod	2914
WPR_lCoaxialLight			2915
WPR_lRingLight				2916
WPR_lPixelAdjust			2917
WPR_SrhDieType				2921
WPR_SrhDieID				2922
WPR_SrhDiePassScore			2923
WPR_SrhDieCheckChip			2924
WPR_SrhDieCheckDefect		2925
WPR_SrhDieChipRate			2926
WPR_SrhDieSDefectRate		2927
WPR_SrhDieTDefectRate		2928
WPR_SrhDieAreaSize			2929
WPR_SrhDieTCorrect			2930
WPR_SrhDieTTolerance		2931
WPR_SrhDieTMinTolerance		2932
WPR_SrhDieTMaxTolerance		2933
WPR_SrhDieLookForward		2934
WPR_SrhDieBadCut			2935
WPR_SrhDieBadCutX			2936
WPR_SrhDieBadCutY			2937
WPR_SrhDieBackupAlign		2938
WPR_SrhDieAlignResol		2939
WPR_SrhDieGreyLevel			2940
WPR_SrhDieThreshold			2941
WPR_SrhDieRefCheck			2942
WPR_SrhDieRefCheckNo		2943
WPR_AutoBondScrnMode		2944
WPR_ABForeground			2945
WPR_lRefDieLearnt			2950
WPR_lPitchCheckX			2951
WPR_lPitchCheckY			2952

BPR_bDieLearnt				2961
BPR_bDieCalibrated			2962
BPR_lDieSearchID			2963
BPR_lDieSizeX				2964
BPR_lDieSizeY				2965
BPR_bEnableInspect			2966
BPR_bPBCheckAvgAngle		2967
BPR_bPBCheckMaxAngle		2968
BPR_fPBAvgAngle				2969
BPR_fPBMaxAngle				2970
BPR_lPBMaxAngleDies			2971
BPR_bPBCheckShift			2972
BPR_fPBMaxShiftX			2973
BPR_fPBMaxShiftY			2974
BPR_lPBMaxShiftDies			2975
BPR_bPBCheckDefect			2976
BPR_bPBDefectDies			2977
BPR_lPBBinBlockNo			2978
BPR_DieAlignMethod			2981
BPR_DieInspectMethod		2982
BPR_DieCoaxialLight			2983
BPR_DieRingLight			2984
BPR_DiePixelAlign			2985
BPR_SrhDiePassScore			2990
BPR_SrhDieCheckChip			2991
BPR_SrhDieCheckDefect		2992
BPR_SrhDieChipRate			2993
BPR_SrhDieSDefectRate		2994
BPR_SrhDieTDefectRate		2995
BPR_SrhDieAreaSize			2996
BPR_SrhDieGreyLevel			2997
BPR_SrhDefectThershold		2998

DataID					3000
CEID					3001
RPTID					3002
WaferId					3003
SlotNo					3004
CassettePos				3005
CassetteType			3006
SlotID01				3011
SlotID02				3012
SlotID03				3013
SlotID04				3014
SlotID05				3015
SlotID06				3016
SlotID07				3017
SlotID08				3018
SlotID09				3019
SlotID10				3020
SlotID11				3021
SlotID12				3022
SlotID13				3023
SlotID14				3024
SlotID15				3025
SlotID16				3026
SlotID17				3027
SlotID18				3028
SlotID19				3029
SlotID20				3030
SlotID21				3031
SlotID22				3032
SlotID23				3033
SlotID24				3034
SlotID25				3035
BinMapFilePath			3036
BinMapFileName			3037
Binnumber				3038
BinName					3039
TotalBinableDieOnWafer	3040
TotalDieBinedFromWafer	3041
FrameId					3042
BindieCount				3043
TotalDieInBin			3044
RemainingDieInBin		3045
TotalDiePickedInBin		3046
DateTimeFrameWasBorn	3047
WafX					3048
WafY					3049
SDSX					3050
SDSY					3051
BinCountFromCurWfr		3052
ListBinFrame			3053
ListWaferFrame			3054
*/




