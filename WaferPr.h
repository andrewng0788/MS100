/////////////////////////////////////////////////////////////////
// WaferPr.cpp : interface of the CWaferPr class
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

#include "MS896AStn.h"
#include "Dpr_Ram.h"
#include "math.h"
#include "LogFileUtil.h"
#include "resource.h"
#include <prheader.h>
#include <prmsg.h>
#include <prstatus.h>
#include <prtype.h>
#include "MS896A.h"
#include "WT_1stDieFinder.h"
#include "WAF_CPhysicalInformation.h"
#include "WaferSrch_Var.h"

#include <deque>
#include <map>
#include <set>
#include <vector>

using std::deque;
using std::map;
using std::set;
using std::vector;

enum eWPR_EVENTID {WPR_LOOK_FORWARD = 1 , WPR_WAFER_THETA_CORRECTION, WPR_NO_DIE_MAX_SKIP_COUNT, WPR_PR_LEARN_DIE_LIGHT_COX,
				   WPR_PR_LEARN_DIE_LIGHT_RING, WPR_PR_LEARN_DIE_LIGHT_SIDE, WPR_DIE_PASS_SCORE,
				   WPR_INSP_CHECK_DEFECTIVE, WPR_INSP_CHECK_DEFECTIVE_CHIP, WPR_INSP_CHECK_DEFECTIVE_CHIP_AREA, WPR_INSP_CHECK_DEFECTIVE_SINGLE_DEFECT,
				   WPR_INSP_CHECK_DEFECTIVE_TOTAL_DEFECT, WPR_GREY_LEVEL_TYPE, WPR_DIE_CHECK_DEFECTIVE_THRESHOLD,
				   WPR_BAD_CUT_TOLERANCE_X, WPR_BAD_CUT_TOLERANCE_Y, WPR_DIE_MAX_SKIP_COUNT, WPR_PRESCAN_SEARCH_DIE_SCORE,
				   WPR_SEARCH_AREA_X, WPR_SEARCH_AREA_Y
				  };

#define		BL_SP_INDEXER_Z				"spfBLIdx"

//v3.99T3
#define		WPR_PKG_FILE_IMAGE_PATH			"C:\\MapSorter\\UserData\\PkgPrImage"
#define		WPR_SHOW_IMAGE_PATH				"C:\\MapSorter\\UserData\\ShowPrImage"
#define		WPR_PR_RECORD_PATH				"C:\\WinEagle\\record\\tmplDir."
#define		WPR_MAX_DIE						70	//	40
#define		WPR_GEN_OCR_DIE_START			30

#define		WPR_BADCUT_POINT				6

//string map file name
#define		WPR_SMF_NAME					"WaferPrStn.smf";
#define		WPR_LRN_NDIE_DATA				"WaferPr Learn Die Data"
#define		WPR_LRN_NDIE_ALNALGO			"Die Align Algorithm"
#define		WPR_LRN_NDIE_BKGROUND			"Die Align Back Ground"
#define		WPR_LRN_NDIE_DIETYPE			"Die Type"
#define		WPR_LRN_NDIE_FINESCH			"Fine Search Res"
#define		WPR_LRN_NDIE_INSPRES			"Inspection Res"
#define		WPR_LRN_NDIE_INSPMETHOD			"Inspection Method"
#define		WPR_LRN_NDIE_DIESIZEX			"Die SizeX"
#define		WPR_LRN_NDIE_DIESIZEY			"Die SizeY"
#define		WPR_LRN_NDIE_ZOOM_MODE			"Zoom Mode"
#define		WPR_LRN_NDIE_CALIBRATION		"Calibration"
#define		WPR_LRN_NDIE_EXTRA_GRAB_DELAY	"NormalDieExtraGrabDelay"
#define		WPR_DIE_CIRCLE_DETECTION		"Die Circle Detection"
#define		WPR_DIE_CAL_1ST_STEP_SIZE		"PR Calibration Step Size"
#define		WPR_IM_AUTO_LEARN_WAFER			"IM Auto Learn Wafer"
#define		WPR_ENABLE_2_LIGHTING			"Enable 2 Lighting"
#define		WPR_COMPARE_DIESCORE_HOMEREFER	"Compare Home Refer Die Score"
#define		WPR_SRN_RDIE_REFDIE_FACE_VALUE_NO		"Ref Die Face Value Number"		//v4.48A26

#define		WPR_SRH_NDIE_DATA				"WaferPr Search Die Data"
#define		WPR_SRH_NDIE_ALGNRES			"Die Align Res"
#define		WPR_SRH_NDIE_DEFECTTHRE			"Defect Thres"
#define		WPR_SRH_NDIE_SCRHAREA_X			"Seach die area X"
#define		WPR_SRH_NDIE_SCRHAREA_Y			"Seach die area Y"
#define		WPR_SRH_NDIE_PASSSCORE			"Pass score"
#define		WPR_SRH_NDIE_GREYLEVEL			"Grey Level defect"
#define		WPR_SRH_NDIE_CHIPAREA			"Reject Chip area"
#define		WPR_SRH_NDIE_DEFECTAREA			"Reject Defect area"
#define		WPR_SRH_NDIE_BACKUPALIGN		"Backup Alignment"
#define		WPR_SRH_NDIE_LOOKFORWARD		"Look forward"
#define		WPR_SRH_NDIE_SIGNLEDEFECT		"Min Single Defect area"
#define		WPR_SRH_NDIE_TOTALDEFECT		"Total Defect area"
#define		WPR_SRH_NDIE_MINCHIPAREA		"Min Chip area"
#define		WPR_SRH_NDIE_WTINDEXTEST		"WT Index Test"
#define		WPR_SRH_NDIE_THETACORRECT		"Theta Correction"
#define		WPR_SRH_NDIE_CONFIRMSRCH		"Confirm Search"
#define		WPR_SRH_NDIE_MINANGLE			"Min Correct Angle"
#define		WPR_SRH_NDIE_MAXANGLE			"Max Correct Angle"
#define		WPR_SRH_NDIE_ACEPTANGLE			"Max Accept Angle"
#define		WPR_SRH_NDIE_FORWARDSIZE		"Look Forward Size"
#define		WPR_SRH_NDIE_FORWARDSIZE_X		"Look Forward Size X"
#define		WPR_SRH_NDIE_FORWARDSIZE_Y		"Look Forward Size Y"
#define		WPR_SRN_NDIE_SRCHULCX			"Search die ULC X"
#define		WPR_SRN_NDIE_SRCHULCY			"Search die ULC Y"
#define		WPR_SRN_NDIE_SRCHLRCX			"Search die LRC X"
#define		WPR_SRN_NDIE_SRCHLRCY			"Search die LRC Y"
#define		WPR_SRN_NDIE_DEFECT_CHECK		"Defect Die Check"
#define		WPR_SRN_NDIE_CHIP_CHECK			"Chip Die Check"
#define		WPR_SRN_NDIE_BADCUT_CHECK		"BadCut Die Check"
#define		WPR_SRN_NDIE_BADCUT_POS			"BadCut Die Point"
#define		WPR_SRN_NDIE_BC_BI_DETECT		"BadCut Binary Detect"
#define		WPR_SRN_NDIE_BC_BI_THRESHOLD	"BadCut Binary Threshold"
#define		WPR_SRH_MAX_EPOXY_SIZE_FACTOR	"Max Epoxy Size Factor"

#define		WPR_SRN_NDIE_UPLOOK_PR			"Enable BH Uplook PR"

#define		WPR_LRN_NORMAL_ROUGH_LED_DIE	"Normal Rough LED Die"

#define		WPR_SRN_NDIE_BADCUT_TOL_X		"BadCut Die Tolerence X"
#define		WPR_SRN_NDIE_BADCUT_TOL_Y		"BadCut Die Tolerence Y"
#define		WPR_SET_REGION_BY_DIE_SIZE_X	"Set Region by Die Size X"
#define		WPR_SET_REGION_BY_DIE_SIZE_Y	"Set Region by Die Size Y"

#define		WPR_SRN_NDIE_BADCUT_SCN_X		"BadCut Die Scanwidth X"
#define		WPR_SRN_NDIE_BADCUT_SCN_Y		"BadCut Die Scanwidth Y"
#define		WPR_SRN_RDIE_USE_NO				"Ref Die Use No"
#define		WPR_SRN_RDIE_REFDIE_CHECK		"Ref Die Check"
#define		WPR_SRN_RDIE_CHK_ALL_REFDIE		"Check All Learnt Ref Die"
#define		WPR_SRN_NDIE_MAX_SKIP			"Max Skip Count"
#define		WPR_SRN_NDIE_MAX_NODIE_SKIP		"Max No Die Skip Count"
#define		WPR_SRN_NDIE_MAX_PR_SKIP		"Max PR Skip Count"
#define		WPR_SRN_NDIE_MAX_BADCUT_SKIP	"Max BadCut Skip Count"
#define		WPR_SRN_NDIE_REV_DIE_STATUS		"Reverse Die Status"
#define		WPR_SRN_NDIE_PICK_ALIGN_DIE		"Pick Alignable Die"
#define		WPR_SRN_NDIE_CHK_ALL_DIE		"Check All die"
#define		WPR_SRN_RDIE_AUTO_LEARN_REFDIE	"Auto Learn Ref Die"
#define		WPR_AOI_NDIE_SORT_INSPECTION	"AOI Sorting inspection"
#define		WPR_AOI_NDIE_NG_PICK_PARTIAL	"AOI Ng Pick Partial Die"
#define		WPR_NDIE_NG_PICK_AREA_TOL_MIN	"Ng Pick Die Area Tol Min"
#define		WPR_NDIE_NG_PICK_AREA_TOL_MAX	"Ng Pick Die Area Tol Max"
#define		WPR_SRN_NDIE_MAX_DEFECT_SKIP	"Max Defect Skip Count"
#define		WPR_SRN_NDIE_GOODDIE_RATIO		"Wafer Good Die Ratio"
#define		WPR_SRN_NDIE_WAFER_DEFECT_LIMIT	"Wafer Defect Die Limit"
#define		WPR_SRN_NDIE_WAFER_DEFECT_COUNT	"Wafer Defect Die Count"
#define		WPR_OCR_DIE_ORIENTATION			"PR OCR Die Orientation"
#define		WPR_SRN_NDIE_MAX_START_SKIP		"WPR Max Start Skip NG Limit"

#define		WPR_SRH_NDIE_LINE_DEFECT				"Line Defect"
#define		WPR_SRH_NDIE_MIN_LINE_DEFECT_LENGTH		"Min Line Defect Length"
#define		WPR_SRH_NDIE_MIN_NUM_OF_LINE_DEFECT		"Min Num Of Line Defect"

#define		WPR_SRN_NDIE_AUTO_ENLARGE_SRCHWND		"Auto Enlarge SrchWnd"

#define		WPR_GEN_NDIE_DATA				"WaferPr General Die Data"
#define		WPR_GEN_NDIE_PRID				"PR Search ID"
#define		WPR_CURSOR_CENTER_X				"PR Cursor Center X"
#define		WPR_CURSOR_CENTER_Y				"PR Cursor Center Y"
#define		WPR_WAFBOND_CENTER_OFFSET_X		"Wafer To Bond PR Center Offset X"
#define		WPR_WAFBOND_CENTER_OFFSET_Y		"Wafer To Bond PR Center Offset Y"
#define		WPR_UPLOOK_BH1_LEARN_CENTER_X				"PR Uplook BH1 Learn Center X"  //4.52D17
#define		WPR_UPLOOK_BH1_LEARN_CENTER_Y				"PR Uplook BH1 Learn Center Y"
#define		WPR_UPLOOK_BH2_LEARN_CENTER_X				"PR Uplook BH2 Learn Center X"
#define		WPR_UPLOOK_BH2_LEARN_CENTER_Y				"PR Uplook BH2 Learn Center Y"				
#define		WPR_EJT_COLLET1_OFFSET_X_PIXEL	"Wafer To Ejector Collet1 Offset X(in pixel)"
#define		WPR_EJT_COLLET1_OFFSET_Y_PIXEL	"Wafer To Ejector Collet1 Offset Y(in pixel)"
#define		WPR_EJT_COLLET2_OFFSET_X_PIXEL	"Wafer To Ejector Collet2 Offset X(in pixel)"
#define		WPR_EJT_COLLET2_OFFSET_Y_PIXEL	"Wafer To Ejector Collet2 Offset Y(in pixel)"
#define		WPR_EJT_COLLET1_COOR_X			"Wafer To Ejector Collet1 X Coordinate"
#define		WPR_EJT_COLLET1_COOR_Y			"Wafer To Ejector Collet1 Y Coordinate"
#define		WPR_EJT_COLLET2_COOR_X			"Wafer To Ejector Collet2 X Coordinate"
#define		WPR_EJT_COLLET2_COOR_Y			"Wafer To Ejector Collet2 Y Coordinate"

//WPR Output Bits
#define		WPR_SO_SEARCH_DIE_CMD			"obWprSearchDieCmd"
#define		WPR_SO_SEARCH_DIE_REPLY_1		"obWprSearchDieRpy1"
#define		WPR_SO_SEARCH_DIE_REPLY_2		"obWprSearchDieRpy2"
#define		WPR_SO_SEARCH_DIE_REPLY_3		"obWprSearchDieRpy3"

#define		WPR_GEN_NDIE_LEARNDIE			"Die Learn"
#define		WPR_GEN_NDIE_LEARNPITCH			"Die Pitch"
#define		WPR_GEN_NDIE_LEARNCALIB			"Die Calib"
#define		WPR_GEN_NDIE_CALIBX				"Die CalibX"
#define		WPR_GEN_NDIE_CALIBY				"Die CalibY"
#define		WPR_GEN_NDIE_CALIBXY			"Die CalibXY"
#define		WPR_GEN_NDIE_CALIBYX			"Die CalibYX"
#define		WPR_GEN_NDIE_PITCHXX			"Die PitchXX"
#define		WPR_GEN_NDIE_PITCHXY			"Die PitchXY"
#define		WPR_GEN_NDIE_PITCHYX			"Die PitchYX"
#define		WPR_GEN_NDIE_PITCHYY			"Die PitchYY"
#define		WPR_GEN_NDIE_PIXELXX			"Die PixelXX"
#define		WPR_GEN_NDIE_PIXELXY			"Die PixelXY"
#define		WPR_GEN_NDIE_PIXELYX			"Die PixelYX"
#define		WPR_GEN_NDIE_PIXELYY			"Die PixelYY"

#define		WPR_EJT_CALIBX					"EJT CalibX"
#define		WPR_EJT_CALIBY					"EJT CalibY"
#define		WPR_EJT_CALIBXY					"EJT CalibXY"
#define		WPR_EJT_CALIBYX					"EJT CalibYX"

#define		WPR_EJT_COLLET1_OFFSET_X		"EJT Collet1 Offset X"
#define		WPR_EJT_COLLET1_OFFSET_Y		"EJT Collet1 Offset Y"
#define		WPR_EJT_COLLET2_OFFSET_X		"EJT Collet2 Offset X"
#define		WPR_EJT_COLLET2_OFFSET_Y		"EJT Collet2 Offset Y"

#define		WPR_GEN_SHOW_BPR_AUTO			"Show BPR at Top in Auto"
#define		WPR_GEN_NDIE_AUTOSCN			"AutoBond ScreenMode"
#define		WPR_GEN_NDIE_AUTOFGL			"Foreground Pos"
#define		WPR_GEN_RDIE_LEARNT				"Total Ref Die Learnt"
#define		WPR_GEN_NDIE_SHAPE				"Die Shape"
#define		WPR_LIGHT_SETTING				"Light Setting"
#define		WPR_LRN_COAX_LIGHT_LEVEL		"Coax Light Level"
#define		WPR_LRN_RING_LIGHT_LEVEL		"Ring Light Level"
#define		WPR_LRN_SIDE_LIGHT_LEVEL		"Side Light Level"
#define		WPR_LRN_BACK_LIGHT_LEVEL		"Back Light Level"
#define		WPR_LRN_EXPOSURE_TIME_LEVEL		"Exposure Time Level"

#define		WPR_SCM_LRN_EXPOSURE_TIME		"Scan Cam Exposure Time Level"
#define		WPR_SCM_LRN_COAX_LIGHT_LEVEL	"Scan Cam Coax Light Level"
#define		WPR_SCM_LRN_RING_LIGHT_LEVEL	"Scan Cam Ring Light Level"
#define		WPR_SCM_LRN_SIDE_LIGHT_LEVEL	"Scan Cam Side Light Level"

#define		WPR_CAMERA_DATA					"Camera Data"
#define		WPR_MOTORIZED_ZOOM_LEVEL		"Motorized Zoom Level"
#define		WPR_MOTORIZED_FOCUS_LEVEL		"Motorized Focus Level"
#define		W_ALIGN_MASTER_PITCH_X			"W-Align Master Pitch X"
#define		W_ALIGN_MASTER_PITCH_Y			"W-Align Master Pitch Y"
#define		WPR_GEN_NDIE_OFFSETX			"NDie Offset X"
#define		WPR_GEN_NDIE_OFFSETY			"NDie Offset Y"
#define		WPR_F1_SRCH_DIE_WITH_ROTATION	"Search Die With Rotation"
#define		W_R_MASTER_SIZE_X				"Refer Master Size X"
#define		W_R_MASTER_SIZE_Y				"Refer Master Size Y"
#define		W_LEARN_PITCH_SPAN_X			"Learn Pitch Span X"
#define		W_LEARN_PITCH_SPAN_Y			"Learn Pitch Span Y"

#define		PPR_LIGHT_SETTING				"PostSeal Light Setting"
#define		PPR_GEN_COAX_LIGHT_LEVEL		"PPR Coax Light Level"
#define		PPR_GEN_RING_LIGHT_LEVEL		"PPR Ring Light Level"
#define		PPR_GEN_SIDE_LIGHT_LEVEL		"PPR Side Light Level"

#define		WPR_PKG_FILE_DISPLAY			"PKG File Display"
#define		WPR_PKG_FILE_NOR_DIE_IMG_PATH	"Normal Die Image Path"
#define		WPR_PKG_FILE_REF_DIE_IMG_PATH	"Ref Die Image Path"

#define		WPR_GRAB_SAVE_IMAGE				"Grab and Save Image"
#define		WPR_GRAB_SAVE_IMAGE_PATH		"Grab and Save Image Path"

#define		WPR_ZOOM_SENSOR					"Zoom Sensor"
#define		WPR_ZOOM_FACTOR					"Zoom Factor"
#define		WPR_CONTINUOUS_ZOOM_MODE		"Zoom Continuous Mode"
#define		WPR_CON_ZOOM_SUB_FACTOR			"Continuous Zoom Sub Factor"
#define		WPR_FOV_SHRINK_MODE				"Zoom FOV Shrink Mode"
#define		WPR_CON_ZOOM_MODE_DP5M			"DP5M Zoom Continuous Mode"
#define		WPR_CON_ZOOM_SUB_FACTOR_DP5M	"DP5M Continuous Zoom Sub Factor"
#define		WPR_ROI_ZOOM_SHRINK_DP5M		"DP5M ROI Zoom Shrink Factor"
#define		WPR_ROI_ZOOM_SHRINK				"ROI Zoom Shrink Factor"
#define		WPR_NORMAL_DIE_ZOOM_FACTOR		"Noraml Die Zoom Factor"
#define		WPR_NORMAL_ZOOM_FACTOR			"Noraml Zoom Factor"
#define		WPR_PRESCAN_ZOOM_FACTOR			"Prescan Zoom Factor"
#define		WPR_NORMAL_DIGITAL_ZOOM			"Normal Digital Zoom Factor"
#define		WPR_PRESCAN_CALIB_XX			"Prescan Calib XX"
#define		WPR_PRESCAN_CALIB_XY			"Prescan Calib XY"
#define		WPR_PRESCAN_CALIB_YY			"Prescan Calib YY"
#define		WPR_PRESCAN_CALIB_YX			"Prescan Calib YX"


#define		BPR_GRAB_DIGITAL_ZOOM			"Bond Grab Digital Zoom"

#define		WPR_NORMAL_DIE_1ST				0

#define		WPR_NORMAL_DIE					0
#define		WPR_REFERENCE_DIE				1

#define		WPR_CAM_WAFER					0
#define		WPR_CAM_BOND					1
#define		WPR_CUR_FORWARD					4
#define		WPR_MAX_FORWARD					8
#define		WPR_CT_DIE						-1	//CENTER

#define		WPR_LT_DIE						0	//LHS
#define		WPR_RT_DIE						1	//RHS	
#define		WPR_UP_DIE						2	//UP
#define		WPR_DN_DIE						3	//DOWN
#define		WPR_UL_DIE						4	//UP+LHS
#define		WPR_DL_DIE						5	//DOWN+LHS	
#define		WPR_UR_DIE						6	//UP+RHS
#define		WPR_DR_DIE						7	//DOWN+RHS

#define		WPR_MAX_CORNER_DIE_CHECK		4

#define		WPR_RECTANGLE_DIE				0
#define		WPR_HEXAGON_DIE					1
#define		WPR_TRIANGULAR_DIE				2	//v4.06
#define		WPR_RHOMBUS_DIE					3

#define		WPR_TRIANGLE_CORNERS			3
#define		WPR_RHOMBUS_CORNERS				4
#define		WPR_HEXAGON_CORNERS				6

#define		WPR_MAX_DIE_CORNER				6
#define		WPR_DIE_CORNER_0				0
#define		WPR_DIE_CORNER_1				1
#define		WPR_DIE_CORNER_2				2
#define		WPR_DIE_CORNER_3				3
#define		WPR_DIE_CORNER_4				4
#define		WPR_DIE_CORNER_5				5

#define		WPR_OCR_MAX_WINDOWS				8

#define		WPR_MAX_DETECT_WIN				4
#define		WPR_MAX_IGNORE_WIN				WPR_MAX_DETECT_WIN
#define		WPR_DETECT_DIE_CORNER			500
#define		WPR_MIN_DIE_CORNER				100

#define		WPR_HMI_NORM_DIE				"Normal"
#define		WPR_HMI_REF_DIE					"Reference"

#define		WPR_GEN_CDIE_MAX_DIE			10
#define		WPR_SRN_CDIE_CHAR_NO			2
#define		WPR_GEN_CDIE_OFFSET				4		//2
#define		WPR_GEN_CDIE_START				'0'
#define		WPR_SRN_CDIE_OFFSET_X			100
#define		WPR_SRN_CDIE_OFFSET_Y			100
#define		WPR_SRN_CDIE_MIN_SCORE			90

#define		WPR_GEN_NDIE_OFFSET				-1
#define		WPR_GEN_RDIE_OFFSET				2
#define		WPR_PSCN_2NDPR_RECORD_ID		(WPR_GEN_RDIE_OFFSET+1+16)
#define		WPR_PSCN_2NDPR_DEFAULT_ID		2

//for normal die option
#define		WPR_NORMAL_PR_DIE_INDEX1		1
#define		WPR_NORMAL_PR_DIE_INDEX2		2  //Second PR
#define		WPR_NORMAL_PR_DIE_INDEX3		3  //FF Mode(prescan)

//for Reference PR option
#define		WPR_REFERENCE_PR_DIE_INDEX1		1  //Wafer Map Reference#1
#define		WPR_REFERENCE_PR_DIE_INDEX2		2  //no use
#define		WPR_REFERENCE_PR_DIE_INDEX3		3  //no use
#define		WPR_REFERENCE_PR_DIE_INDEX4		4  //Collet Hole with Epoxy Pattern
#define		WPR_REFERENCE_PR_DIE_INDEX5		5  //BH1 Mark //Ejector Pin for NVC
#define		WPR_REFERENCE_PR_DIE_INDEX6		6  //BH2 Mark
#define		WPR_REFERENCE_PR_DIE_INDEX7		7  //BH1 Collet Hole With Circle
#define		WPR_REFERENCE_PR_DIE_INDEX8		8  //BH2 Collet Hole With Circle
#define		WPR_REFERENCE_PR_DIE_INDEX9		9  //NG Grade PR

//#define		WPR_LRN_NDIE_MAXNUM				3
//#define		WPR_LRN_RDIE_MAXNUM				3

#define		WPR_AOI_GRADE_MAX_MAPPING		500
#define		WPR_MAX_LIGHT_GROUP				2

//Software bound for Zoom & Focus Motors
#define		WPR_ZOOM_POSITIVE_LIMIT			60000
#define		WPR_ZOOM_NEGATIVE_LIMIT			0

#define		WPR_FOCUS_POSITIVE_LIMIT		0
#define		WPR_FOCUS_NEGATIVE_LIMIT		-10000

#define		WPR_2D_BARCODE_SEARCH_MARG		200		//1000		//v4.43T8	//Knowles MS109

//ERROR/warning
#define		WPR_ERR_CALIB_NO_1ST_DIE		-1
#define		WPR_ERR_CALIB_NO_LT_DIE			-2
#define		WPR_ERR_CALIB_NO_RT_DIE			-3
#define		WPR_ERR_CALIB_NO_UP_DIE			-4
#define		WPR_ERR_CALIB_NO_DN_DIE			-5
#define		WPR_ERR_CALIB_ZERO_VALUE		-6

#define		WPR_ERR_PITCH_NO_1ST_DIE		-1
#define		WPR_ERR_PITCH_NO_X_DIE			-2
#define		WPR_ERR_PITCH_NO_Y_DIE			-3
#define		WPR_ERR_INVALID_PITCH_X			-4	//xyz
#define		WPR_ERR_INVALID_PITCH_Y			-5	//xyz

#define		WPR_ERR_LRN_CHAR_DIE_OUT_LIMIT	-1

#define		Err_ZoomHome					-7
#define		Err_ZoomMove					-8

#define		Err_FocusHome					-9
#define		Err_FocusMove					-10


// Color for selected and unselected camera button 
#define		WPR_SELECTED_COLOR				0x804000	
#define		WPR_UNSELECTED_COLOR			0xA7A794		

#define		WPR_SEARCHAREA_SHIFT_FACTOR		0.5

#define		WPR_DIE_PITCH_TOLERANCE			0.3
#define		WPR_RDIE_SIZE_TOLERANCE			0.2

//#define		WPR_MAX_POLYGON_SIZE			6
#define		WPR_SHR_FIRST_DIE_RETERY_LOOP		2
#define		WPR_SRH_FIRST_DIE_RETRY_WITH_LF		3
#define		WPR_SRH_FIRST_DIE_RETRY_NO_LF		7

#define WAFERPR_AXIS_Z			"WaferPrZAxis"
#define WAFERPR_BACKLIGHT_Z		"BackLightZAxis"		//v4.24		//ES101

#define WPR_MP_BACKLIGHT_Z_NORMAL	"mpfBackLightZ"		//v4.39T8	//ES101 v1.2


#define	WPR_AUTO_FOCUS_WAFER			"Motorized Focus Auto by Wafer"
#define	WPR_AUTO_FOCUS_RESULT			"Motorized Focus Result"
#define	WPR_AUTO_FOCUS_POSITION			"Motorized Focus Position"
#define	WPR_AUTO_FOCUS_POSITION2		"Motorized Focus Position2"
#define	WPR_AUTO_FOCUS_PR_DELAY			"Motorized Focus Pr Delay"
#define	WPR_AUTO_FOCUS_UP_LIMIT			"Motorized Focus Up Limit"
#define	WPR_AUTO_FOCUS_LOW_LIMIT		"Motorized Focus Low Limit"
#define	WPR_AUTO_FOCUS_ERROR_TOL		"Motorized Focus Error Tol"
#define	WPR_AUTO_FOCUS_MIN_DIST			"Motorized Focus Min Dist"
#define	WPR_AUTO_FOCUS_RVALUE_LIMIT		"Motorized Focus rValue Limit"
#define WPR_AUTO_FOCUS_STANDBY_OFFSET	"Motorized Focus Standby Offset"
#define WPR_BACKLIGHT_ELEVATOR_UP_LEVEL				"Back Light Elevator Up Level"
#define WPR_BACKLIGHT_ELEVATOR_STANDBY_LEVEL		"Back Light Elevator Stand By Level"
#define	WPR_BACKLIGHT_AUTO_FOCUS_DELAY				"Back Light Auto Focus Delay"

#define WPR_DATA						"Wafer PR Data"
#define	WPR_PRESCAN_SETTING				"Wafer PR Prescan Setting"
#define	WPR_OVERLAP_X					"Overlap X"
#define	WPR_OVERLAP_Y					"Overlap Y"
#define	WPR_PITCH_X						"Pitch Tol X"
#define	WPR_PITCH_Y						"Pitch Tol Y"
#define	WPR_WAFER_NO_DIE_LIMIT			"Wafer No die limit"

typedef struct
{
	BOOL	m_bEnableZoomSensor;
	CString	m_szZoomFactor;
	BOOL	m_bIsFixZoom;

	DOUBLE	m_dScanCalibXX;
	DOUBLE	m_dScanCalibXY;
	DOUBLE	m_dScanCalibYY;
	DOUBLE	m_dScanCalibYX;
}
ZOOM_VIEW;

typedef struct 
{
	CDWordArray m_awHoleRow;
	CDWordArray m_awHoleCol;
}	SCAN_ALIGN_HOLE_LIST;

#define	SA_HOLES_MAX	50
typedef struct 
{
	CDWordArray m_awHoleRow;
	CDWordArray m_awHoleCol;
	CByteArray	m_ucHoleNul;
}	SCAN_ALIGN_WAFER_HOLES;

typedef struct 
{
	LONG m_lMatchPoints;
	LONG m_lMapAlnRow;
	LONG m_lMapAlnCol;
	LONG m_lScnAlnRow;
	LONG m_lScnAlnCol;
}	SCAN_ALIGN_TOP_LIST;


#define	MAX_MAP_DIM		1200	//	Max row or column

class CWaferPr;
class CIMEvent : public CObject
{
public:

	static PR_VVOID PR_MouseClick(PR_RFUNC_ON_MOUSE_CLICK_IN	 const	*pstInPar,
								  PR_RFUNC_ON_MOUSE_CLICK_OUT			*pstOutPar);
	static PR_VVOID PR_MouseDrag(PR_RFUNC_ON_MOUSE_DRAG_IN const	*pstInPar,
								 PR_RFUNC_ON_MOUSE_DRAG_OUT		*pstOutPar);

	VOID SetWaferPr(CWaferPr *pPr);
	static CWaferPr *m_pWaferPr;

};

class CAdaptWaferWrapper : public CObject
{
public:

//#ifdef PR_ADAPT_WAFER
	static PR_VVOID MoveMotorFunc(PR_RFUNC_MOVE_WAF_TABLE_IN	 const	*pstInPar,
								  PR_RFUNC_MOVE_WAF_TABLE_OUT			*pstOutPar);
	static PR_VVOID GetMotorFunc(PR_RFUNC_GETPOS_WAF_TABLE_IN const	*pstInPar,
								 PR_RFUNC_GETPOS_WAF_TABLE_OUT		*pstOutPar);
//#endif

	VOID SetWaferPr(CWaferPr *pPr);
	static CWaferPr *m_pWaferPr;
};

typedef struct
{
	LONG	m_lFrameFocusStep;
	LONG	m_lFrameFocusDir;
	DOUBLE  m_dLastFocusScore;
	LONG	m_lFrameMinCol;
	LONG	m_lFrameMaxCol;
	LONG	m_lFrameMinRow;
	LONG	m_lFrameMaxRow;
}	AF_FOCUS_SCORE;


class CPrZoomSensorMode;
class CWaferPr : public CMS896AStn
{
	DECLARE_DYNCREATE(CWaferPr)

protected:
	#define	MAX_PTNS		500

	// Operation state for PR Init in Idle State
	enum {	DPR_INIT_Q,
			WAIT_PR_Q,
			INIT_PR_Q,
			INIT_COMPLETED_Q,
			INIT_IM_Q				//v2.82T4
		 };

	// Operation state in Auto State
	enum {	WAIT_WT_STABLE_Q,
			GRAB_IMAGE_Q,
			WAIT_GRAB_DONE_Q,
			INSPECTION_Q,
			LOOKFORWARD_Q,
			LOOKFORWARD_Q_IM_RPY3,		//v3.44T1
// prescan relative code	B
			PRESCAN_WAIT_WT_STABLE_Q,
			PRESCAN_GRAB_Q,
			PRESCAN_WAIT_PR_READY_Q,
			PRESCAN_WAIT_MAP_DONE_Q,
			PRESCAN_WAIT_BAR_DONE_Q,
// prescan relative code	E
			HOUSE_KEEPING_Q
		 };

protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	VOID Operation();

	// State Operation (WPR_State.cpp)
	LONG GetDiePROffsetX() const;
	LONG GetDiePROffsetY() const;
	VOID SetDiePROffsetResult(const LONG lXOffset, const LONG lYOffset, const DOUBLE dTOffset, const BOOL bRotate);
	VOID SetDiePRResult(const LONG lXOffset, const LONG lYOffset, const DOUBLE dTOffset, const BOOL bRotate, const LONG lDieResult);
	VOID RunOperation();
	VOID RunOperationWpr_Scan();

	virtual	VOID IdleOperation();
	virtual	VOID InitOperation();
	virtual	VOID PreStartOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
	virtual VOID CycleOperation();
	virtual	VOID StopOperation();

	virtual VOID RegisterVariables();
	VOID RegisterEjtVariables();

	BOOL SavePrData(BOOL bGetHMIVariable = TRUE);
	BOOL LoadPrData(VOID);
	BOOL LoadWaferPrOption();
	BOOL SaveRecordID(VOID);
	BOOL LoadRecordID(VOID);
	VOID InitVariable(VOID);
	VOID UpdateHmiVariable(VOID);
	VOID GetHmiVariable(VOID);
	VOID ChangeCamera(BOOL bCameraNo, BOOL bChangeJoystick = TRUE, UINT unPostSealID = 0);
	VOID OutText(char *pMessage, PR_COORD stTxtCoord, PR_COLOR eTxtColor);
	VOID DrawAndEraseCursor(PR_COORD stStartPos, unsigned char ucCorner, BOOL bDraw);
	VOID DrawAndEraseLine(PR_COORD stStartPos, PR_COORD stEndPos, BOOL bDraw);
	VOID DrawRectangleBox(PR_COORD stCorner1, PR_COORD stCorner2, PR_COLOR ssColor);
	VOID DrawRectangleDieSize(PR_COLOR ssColor);
	VOID DrawSrchDieResultBox(BOOL bDieType, LONG lRefNo, PR_COLOR ssColor);
	VOID DrawHomeCursor(PR_COORD stCenter, BOOL bClearScreen = FALSE);
	VOID UpdateSearchDieArea(LONG lDieNo, DOUBLE dSearchCodeX, DOUBLE dSearchCodeY, BOOL bUpdate, BOOL bChangeCam = TRUE);
	BOOL EnableMouseClickCallbackCmd(CONST BOOL bEnable);

	VOID GetSearchDieArea(PR_WIN *stSrchArea, INT nDieNo, INT nEnlarge = 0);
	VOID DisplaySearchDieResult(PR_UWORD usDieType, BOOL bDieType, CONST LONG lDieNo, PR_REAL	fDieRotate, PR_COORD stDieOffset, PR_REAL fDieScore);
	BOOL DisplayLearnDieResult(PR_UWORD lLearnStatus);
	VOID ConvertPixelToUM(const LONG lPixelX, const LONG lPixelY, double &dXum, double &dYum);
	VOID ConvertPixelToUM(PR_COORD stPixel, double &dXum, double &dYum);
	VOID ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY);
	VOID ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY, 
								 double dCalibX, double dCalibY, double dCalibXY, double dCalibYX);
	VOID ConvertMotorStepToPixel(int siStepX, int siStepY, PR_COORD &stPixel);
	VOID ConvertMotorStepToFFPixel(int siStepX, int siStepY, PR_COORD &stPixel);
	VOID ConvertMotorStepToUnit(LONG lStepX, LONG lStepY, LONG *lUnitX, LONG *lUnitY);
	VOID ConvertMotorStepToDUnit(LONG lStepX, LONG lStepY, DOUBLE& dUnitX, DOUBLE& dUnitY);	//v4.59A18
	VOID ConvertPixelToUnit(PR_COORD stPixel, LONG *lUnitX, LONG *lUnitY, BOOL bUseUm=FALSE);
	VOID ConvertPixelToDUnit(PR_COORD stPixel, DOUBLE &dUnitX, DOUBLE &dUnitY, BOOL bUseUm=FALSE);	//shiraishi03
	DOUBLE ConvertDUnitToPixel(CONST DOUBLE dUnit);
	BOOL VerifyPRRegion(PR_WIN *stInputArea);
	BOOL VerifyPRRegion_MxN(PR_WIN *stInputArea);						//v4.46T17
	BOOL VerifyPRRegion_MxN(PR_COORD &stCorner1, PR_COORD &stCorner2);	//v4.46T17
	BOOL VerifyPRPoint(PR_COORD *stPoint);
	VOID ConvertDieTypeToText(PR_UWORD usDieType, CString &szText);
	VOID GetLightingConfig(int nGroupID, PR_SOURCE &emCoaxID, PR_SOURCE &emRingID, PR_SOURCE &emSideID);

	BOOL CounterCheckReferenceDieWithNormalDie();

	BOOL MoveWaferTable(int siXAxis, int siYAxis);
	VOID GetWaferTableEncoder(int *siXAxis, int *siYAxis, int *siTAxis);
	VOID GetWaferTableCommander(int *siXAxis, int *siYAxis, int *siTAxis);
	VOID SetWaferTableJoystick(BOOL bState, BOOL bWT2=FALSE);
	VOID SetBinTableJoystick(BOOL bState);
	VOID UseWaferTableJoystick(BOOL bState);	// Consider the current state
	VOID UseBinTableJoystick(BOOL bState);		// Consider the current state
	VOID SetWaferTableJoystickSpeed(LONG lLevel);
	VOID SetBinTableJoystickSpeed(LONG lLevel);

	VOID CalculatePrescanFov(BOOL bLogIt);
	VOID CalculateDieInView(CONST BOOL bPrMsd = FALSE);
	VOID CalculateLFSearchArea(VOID);	
	BOOL GetLFSearchArea_3x3(PR_WIN &stWnd, CONST INT nLFRegion);							//v3.13T3
	BOOL GetLFSearchArea_5x3(PR_WIN &stWnd, CONST INT nLFRegion);							//v3.13T3
	BOOL GetLFSearchArea_5x5(PR_WIN &stWnd, CONST INT nLFRegion);							//v3.13T3
	BOOL GetLFSearchArea_7x7(PR_WIN &stWnd, CONST INT nLFRegion);							//v4.43T2
	BOOL GetLFSearchArea_9x9(PR_WIN &stWnd, CONST INT nLFRegion);
	BOOL GetLFSearchArea_11x11(PR_WIN &stWnd, CONST INT nLFRegion);
	BOOL GetLFSearchArea_13x13(PR_WIN &stWnd, CONST INT nLFRegion);
	BOOL GetLFSearchWindow(LONG lDiffRow, LONG lDiffCol, BOOL bMxN, PR_WIN &stWnd);
	INT  GetLFRegion_5x5(CONST ULONG ulCurRow, CONST ULONG ulCurCol, 
						 CONST ULONG ulTargetRow, CONST ULONG ulTargetCol);					//v4.43T2
	INT  GetLFRegion_7x7(CONST ULONG ulCurRow, CONST ULONG ulCurCol, 
						 CONST ULONG ulTargetRow, CONST ULONG ulTargetCol);					//v4.43T2
	VOID DisplayLFSearchArea(PR_COLOR ssColor);
	VOID DisplayRectArea(PR_COORD stCorner1, PR_COORD stCorner2, PR_COLOR ssColor);
	VOID DisplayLFSearchArea_mxn(PR_COLOR ssColor, INT nRegionSize = REGION_SIZE_3x3, INT nCurrLFPosn = 0);		//v3.13T3
	VOID UpdateDieTypeCounter(PR_UWORD usDieType, DOUBLE dDieAngle);
//	VOID DisplayJoystickSpeed(VOID);
	VOID RotateLFSearchArea(DOUBLE dDegree);

	BOOL ScanDieIsDefect(PR_UWORD usAlignType, PR_UWORD usInspType);
	BOOL ResetAutoBondScreen(VOID);
	BOOL UpdateAutoBondScreen(BOOL bOn);
	BOOL DieIsAlignable(PR_UWORD usDieType);
	BOOL DieIsGood(PR_UWORD usDieType);
	BOOL DieIsDefective(PR_UWORD usDieType);
	BOOL DieIsBadCut(PR_UWORD usDieType);
	BOOL DieIsInk(PR_UWORD usDieType);
	BOOL DieIsChip(PR_UWORD usDieType);
	BOOL DieIsOutAngle(DOUBLE dDieAngle);
	BOOL DieNeedRotate(PR_REAL fDieRotate);
	BOOL CheckLookForward(VOID);
	BOOL IsWithinThetaLimit(PR_REAL fDieRotate);
	BOOL LogDieResult(CONST LONG lRow, CONST LONG lCol, CONST LONG lStatus, PR_UWORD usDieType, BOOL bLookForward = FALSE);	//v3.75

	BOOL AP_SetGoodDie(LONG nDieX, LONG nDieY, bool bIsBadCut, bool bIsDefect, DOUBLE dDieRot, CString szDieBin);
	LONG ReScanRunTimeDisplayMapIndexForMS(LONG lGrabRow, LONG lGrabCol, INT nOffsetX, INT nOffsetY, LONG nDieWfX, LONG nDieWfY, bool bIsBadCut, bool bIsDefect);
	void GetSubWindow(LONG lIndex, PR_WORD &lULX, PR_WORD &lULY, PR_WORD &lLRX, PR_WORD &lLRY);

	BOOL HasReferDieCheck();
	BOOL HasSpRefDieCheck();
	BOOL IsNormalDieLearnt();
	BOOL IsMS60NGPickEmptyHoleLearnt();		//v4.54A5

	VOID SetLockPrMouseJoystick(BOOL bLock);
	BOOL GetLockPrMouseJoystick();

	BOOL ConfirmRefDieCheck(VOID);
	BOOL ConfirmSpecialgradeRefDieCheck(VOID);			//v3.24T1
	BOOL AutoBondScreen(BOOL bOn);
	BOOL AutoSearchDie(CONST BOOL bLogMap = FALSE);		//v2.96T2
	BOOL AutoGrabDone(VOID);
	BOOL AutoDieResult(CONST BOOL bCheckScore = FALSE);
	BOOL AutoLogDieResult();							//v3.34	//For collet-hole measurement only
	BOOL AutoLookForwardDie(BOOL bEnable);
	BOOL AutoLookForwardDie_HwTrigger(BOOL bEnable);
	BOOL AutoLookBackwardLastDie(BOOL bMS60NewLFCycle=FALSE);	//v4.54A5	//MS60 NGPick mode
	BOOL PerformWTPrVibrationTest();							//v4.59A40
	BOOL AutoLookAroundDie();			//BLOCKPICK
	BOOL AutoLearnRefDie(VOID);			//v2.78T2
	BOOL AutoEnlargeSrchWndSrchDie(PR_SRCH_DIE_RPY2 *pstNewSrchRpy2, PR_SRCH_DIE_RPY3 *pstNewSrchRpy3);
	BOOL CheckWaferIDWithBondIDs(CONST LONG lWaferID);		//v4.49A7
	//v4.54A6	//MS60 NGPick fcn
	BOOL AutoLearnEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner);
	BOOL AutoSearchEpoxy(BOOL bAuto, BOOL bLatch, BOOL bDisplayResult, LONG pcx, LONG pcy, UINT &nX, UINT &nY, ULONG& lEpoxySize, CEvent *pevGrabImageReady,CString &szErrorMsg);

	LONG FindDieCalibration(VOID);
	LONG FindScanCalibration(VOID);
	LONG FindDiePitch(BOOL bAutoLearn);
	VOID FreeAllPrRecords(BOOL bResetState = TRUE);

	//v4.42T3
	LONG UserSearchDieWithEjt(IPC_CServiceMessage &svMsg);			//v4.44T2					
	LONG UserSearchDie_Ejt(IPC_CServiceMessage &svMsg);					
	LONG LearnEjtXYCalibration(IPC_CServiceMessage &svMsg);
	LONG LearnEjtColletOffsetXY(IPC_CServiceMessage &svMsg);
	LONG ConfirmEjtColletOffsetXY(IPC_CServiceMessage &svMsg);		//v4.43T5
	LONG CancelEjtColletOffsetXY(IPC_CServiceMessage &svMsg);		//v4.43T5
	LONG SearchColletHoleEpoxyPattern(IPC_CServiceMessage &svMsg);
	LONG AutoSearchColletHole(IPC_CServiceMessage &svMsg);			//v4.43T5
	LONG SearchColletHole(IPC_CServiceMessage &svMsg);				//v4.47A5
	LONG ShowEjtColletOffsetXY(IPC_CServiceMessage &svMsg);
	LONG FindEjtXYCalibration(VOID);					
	VOID ManualEjtDieCompenate(PR_COORD stDieOffset);
	VOID CalculateEjtDieCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY, BOOL bFF = FALSE);
	BOOL MoveEjectorTable(int siXAxis, int siYAxis);
	BOOL MoveEjectorTableToColletOffset(CONST BOOL bBH2);
	VOID GetEjtXYEncoder(int *siXAxis, int *siYAxis);
	VOID ConvertPixelToEjtMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY);
	VOID ConvertFFPixelToEjtMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY);
	VOID ConvertFFPixelToUM(PR_COORD stPixel, double &dStepX_um, double &dStepY_um);
	LONG ConvertEjtXMotorStepToPixel(int nStepX);				//v4.50A31
	LONG ConvertEjtYMotorStepToPixel(int nStepY);				//v4.50A31
	VOID CalculateEjtCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY);
	DOUBLE	m_dEjtCalibX,	m_dEjtCalibY;		// EjtXY calibration factor
	DOUBLE	m_dEjtCalibXY,	m_dEjtCalibYX;	// EjtXY calibration factor
	DOUBLE	m_dEjtFFCalibX,	m_dEjtFFCalibY;
	DOUBLE	m_dEjtFFCalibXY,	m_dEjtFFCalibYX;
	LONG	m_lEjtCollet1OffsetX, m_lEjtCollet1OffsetY;		//in EJT motor steps
	LONG	m_lEjtCollet2OffsetX, m_lEjtCollet2OffsetY;		//in EJT motor steps

	LONG	m_lEjtCollet1CoorX, m_lEjtCollet1CoorY;		// coor of arm1
	LONG	m_lEjtCollet2CoorX, m_lEjtCollet2CoorY;		// coor of arm2

	VOID MoveBhToPick(CONST BOOL bPick, CONST LONG lOffset = 0);
	VOID MoveBhToPick_Z2(CONST BOOL bPick);	//dual arm learn collet
	BOOL WPR_GetFrameLevel();

	BOOL CheckColletOffsetOver500();

	BOOL UpdatePKGFileRecordImage(PR_WORD ssID, BOOL bIsNormalDie);
	BOOL ShowPrImage(CString &szImagePath, LONG &lDieNo, BOOL bDieType, LONG lRefDieNo);
	BOOL UploadPrPkgPreviewImage(CString szImageDestPath, BOOL bDieType, LONG lRefDieNo);

	BOOL	 CG_AutoDieResult(CONST BOOL bCheckScore = FALSE);
	PR_UWORD CG_SearchDieCmd();

	PR_UWORD AutoDetectDieCorner(BOOL bDieType, LONG lRefDieNo);	//v2.72a14
	PR_UWORD AutoLearnDie(BOOL bDieType, LONG lRefDieNo);
	PR_UWORD SearchDieCmd(BOOL bDieType, LONG lInputDieNo, PR_UBYTE ubSID, PR_UBYTE ubRID, PR_COORD stDieULC, PR_COORD stDieLRC, 
							PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, 
							LONG lGraphicInfo, FLOAT fOrientation = 0, UCHAR ucColletID=0, BOOL bRepeatAngleChk = FALSE);
	PR_UWORD SearchDieRpy1(PR_UBYTE ubSID, PR_SRCH_DIE_RPY1 *stSrchRpy1);
	PR_UWORD SearchDieRpy2(PR_UBYTE ubSID, PR_SRCH_DIE_RPY2 *stSrchRpy2);
	PR_UWORD SearchDieRpy3(PR_UBYTE ubSID, PR_SRCH_DIE_RPY3 *stSrchRpy3);
	PR_UWORD SearchDieRpy4(PR_UBYTE ubSID, PR_SRCH_DIE_RPY4 *stSrchRpy4);
	PR_UWORD ExtractDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, PR_COORD *stDieSize);
	PR_WORD ManualSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
							PR_COORD stCorner1, PR_COORD stCorner2, BOOL bLog = FALSE, BOOL bRepeatAngleChk = FALSE);
	PR_WORD ManualUpLookSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
							PR_COORD stCorner1, PR_COORD stCorner2, BOOL bLog=FALSE); //4.52D17

	BOOL CalculateDefectScore(PR_UWORD usDieType, CONST LONG lDieNo, PR_SRCH_DIE_RPY3 *stSrchRpy3);		
	BOOL CalculateAutobondDefectScore(PR_UWORD usDieType, CONST LONG lDieNo, PR_SRCH_DIE_RPY3 *stSrchRpy3);
	PR_WORD ManualSearchNormalDie(PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_WIN stWin);
	PR_WORD ManualSearchScanDie(PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_WIN stWin);
	PR_WORD SemiSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
							PR_COORD stCorner1, PR_COORD stCorner2);
	
	//Hardware Trigger only use in auto bonding
	PR_WORD ManualSearchDie_HWTri(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
							PR_COORD stCorner1, PR_COORD stCorner2, BOOL bLog);
	PR_UWORD GrabShareImage(BOOL bDieType, LONG lInputDieNo, PR_UBYTE ubSID, PR_UBYTE ubRID);	//Call before GrabHwTrigger()
	BOOL	 GrabHwTrigger();	//Set Hw Trigger bit
	PR_UWORD GrabShareImageRpy2(PR_UBYTE ubSID, PR_ULWORD *ulImageID, PR_ULWORD *ulStationID);	//Get Rpy2
	PR_UWORD ExtractGrabShareImgDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, PR_COORD *stDieSize);
	BOOL ReceiveHWTriRpyinHouseKeeping();	//andrewng 2020-0616

	BOOL IndexAndFovSearchNormalDie(int *siPosX, int *siPosY, LONG lDirection, LONG &lNewAlignMode);
	BOOL AutoLearnWaferAngle(LONG lPosX, LONG lPosY, LONG &lNewAlignMode);
	BOOL AutoLearnGTByDieAngle(LONG lPosX, LONG lPosY);
	BOOL IndexAndFovSearchReferDie(int *siPosX, int *siPosY, LONG lDirection);
	BOOL IndexAndSearchRefDie(int *siPosX, int *siPosY, LONG lDirection, LONG lRefDieNo);
	BOOL IndexAndSearchRefHomeDie(int *siPosX, int *siPosY, LONG lDirection);
	BOOL IndexAndSearchNormalDie(int *siPosX, int *siPosY, LONG lIdxPos, LONG lSrchPos, BOOL bMoveTable, BOOL bDoComp, PR_BOOLEAN bLatch, PR_BOOLEAN bCheckDefect = PR_FALSE);
	BOOL LookAroundNormalDie(int &siPosX, int &siPosY, LONG lSrchPos, PR_BOOLEAN bLatch, CONST LONG lDieNo=0);	
	BOOL LookAroundRefDie(int &siPosX, int &siPosY, LONG lSrchPos, PR_BOOLEAN bLatch);		//v4.15T2
	BOOL LookAroundNormalDie_mxn(int &siPosX, int &siPosY, INT nLFRegionSize, LONG lSrchPos, PR_BOOLEAN bLatch);		//v3.13T3
	VOID ManualDieCompenate(PR_COORD stDieOffset, PR_REAL fDieRotate);
	VOID ManualDieCompenate_Rotate(PR_COORD stDieOffset, PR_REAL fDieRotate);
	BOOL WprCompensateRotateDie(LONG siOrigX, LONG siOrigY, DOUBLE fDieRotate);
	BOOL FindOneDieInWafer();
	BOOL SpiralSearchNormalDie(LONG &lNewAlignMode);

	//Char RefDie functions
	PR_UWORD AutoLearnCharDie(LONG lDigitNo);
	PR_UWORD AutoLearnOCRDie(LONG lDigitNo);
	PR_UWORD SearchCharDie(PR_COORD stDieCenter, PR_RCOORD *stDieScore, PR_COORD *stDieDigit);
	PR_WORD ManualSearchCharDie(PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_RCOORD *stDieScore);
	VOID DisplaySearchCharDieResult(PR_UWORD usDieType, PR_REAL	fDieRotate, PR_COORD stDieOffset, PR_RCOORD stDieScore);

	//2D barcode support for PLLM REBEL			//v2.83T61	//v3.70T2
	BOOL Search2DBarCodePattern(CString &szCode, UINT unID = 1, CONST BOOL bRetry = FALSE, BOOL bMirrorMode = TRUE);
	BOOL Search2DBarCodePatternFromWafer(CString &szCode, CONST BOOL bRetry = FALSE, BOOL bMirrorMode = TRUE);
	//v3.00T1
	//Block fcns
	PR_UWORD GetRefDieSrchCmd(PR_SRCH_DIE_CMD *stSrchCmd, BOOL bDieType, LONG lInputDieNo, PR_COORD stDieULC, PR_COORD stDieLRC, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, LONG lGraphicInfo);
	PR_UWORD SearchNumberDie(PR_COORD *stDieOffset, PR_REAL *fDieRotate, PR_RCOORD *stDieScore, PR_COORD *stDieDigit);
	PR_UWORD SearchAoiOcrDie(PR_COORD *stDieOffset, PR_REAL *fDieRotate, BOOL bDebugLog=FALSE);

	//For set region by die size
	VOID CloneCoordinate(PR_COORD *stSource , PR_COORD *stDest , INT size);
	VOID DrawAndErasePolygon(PR_COORD *stCorners, INT size, PR_COLOR color);
	VOID DrawAndEraseLineForPolygon(PR_COORD stStartPos, PR_COORD stEndPos , PR_COLOR color);
	DOUBLE FindAngle(PR_COORD stPos1, PR_COORD stPos2);
	DOUBLE FindLength(PR_COORD stPos1, PR_COORD stPos2);

	//PostSeal Optics for 2D barcode fcn	//v3.71T5
	BOOL GetPostSealLighting(UINT unID = 1);
	BOOL SetPostSealLighting(CONST BOOL bOn, UINT unID = 1);
	BOOL SetPostSealGenLighting(CONST BOOL bOn, UINT unID = 1);
	BOOL TurnOffPSLightings();

	VOID SetEjectorVacuum(BOOL bSet);		//v3.85		//MS100

	// Adapt Wafer functions
	BOOL PR_NotInit();
	BOOL PR_Init();
	BOOL PR_InitDieMapSort();
	BOOL PR_InitMSSystemPara();
	
	INT	 IM_GetEffectiveView();
	PR_WORD	GetPRWinULX();
	PR_WORD	GetPRWinULY();
	PR_WORD	GetPRWinLRX();
	PR_WORD	GetPRWinLRY();
	PR_WORD	GetScanPRWinULX();
	PR_WORD	GetScanPRWinULY();
	PR_WORD	GetScanPRWinLRX();
	PR_WORD	GetScanPRWinLRY();
	PR_WIN  GetScanFovWindow();
	BOOL SubCutScanFOV(DOUBLE &dShrinkRatio);

	VOID UpdateHmiCurNormalDieSize();
	VOID UpdateHmiCurPitchSize();

	LONG GetPrScaleFactor();
	BOOL TurnOnOffPrLighting(BOOL bOn);

	// search circle
	PR_WORD AutoSearchCircle(LONG pcx , LONG pcy , LONG ecx , LONG ecy , UINT &nX, UINT &nY );
	PR_UWORD AutoLearnCircle(PR_COORD stULCorner, PR_COORD stLRCorner);

	BOOL IM_InitPrHighLevelInfo(BOOL bWaferAligned = FALSE);

	CString	m_szToPrWaferID;

	BOOL MotionTest();

	BOOL DP_GetAllLightings();
	BOOL GetGeneralExposureTime();
	BOOL WPR_GetGeneralLighting();
	//Zoom sensor functions	//	Dual Path
	LONG	WPR_ToggleZoom(BOOL bToScan, BOOL bGrab=FALSE, INT nDebug = 0);
	BOOL	IsDP();
	BOOL	IsDP_ScanCam();
	BOOL	IsEnableZoom();
	VOID	InitZoomView();
	PR_WORD InitZoomSensor();
	VOID	SetZoomViewFixZoom(BOOL bSet);
	BOOL	LiveViewZoom(LONG lZoomFactor, BOOL bGrab=FALSE, BOOL bChange=TRUE, INT nDebug = 0);
	PR_WORD WPR_ZoomSensor(LONG lZoomMode, PR_WORD ssType);
	BOOL IsZoomFreezed();
	VOID SetTransferEjtColletCoordInZoomMode(LONG lCurZoomMode, LONG lNewZoomMode);

	VOID UpdateLearnDieZoomView();
	VOID UpdateZoomViewByRecord(LONG lInputDieNo);
	VOID UpdateZoomViewByRecord(BOOL bDieType, LONG lInputDieNo);
	VOID SetZoomFactorByRecord(BOOL bDieType, LONG lInputDieNo);
	VOID SetNormalDieZoomFactor(LONG lZoomFacotr);
	VOID WPR_DigitalZoom(short hZoomFactor);
	LONG ConfirmDigitalZoom(IPC_CServiceMessage& svMsg);
	VOID SetNormalZoomFactor(LONG lZoomFacotr);
	VOID SetPrescanZoomFactor(LONG lZoomFactor);
	VOID SetZoomViewCalibration(double dCalibX, double dCalibY, double dCalibXY, double dCalibYX);
	VOID SetFFCalibration(double dCalibX, double dCalibY, double dCalibXY, double dCalibYX);
	PR_WORD TransformFromLocalCoordToGlobalCoord(LONG lZoomMode, LONG lPrRoiX, LONG lPrRoiY,
			LONG &lPrGlobalX, LONG &lPrGlobalY);
	PR_WORD TransformFromGlobalCoordToLocalCoord(LONG lZoomMode, LONG lPrGlobalX, LONG lPrGlobalY, LONG lPrRoiX, LONG lPrRoiY,
			LONG &lPrLocalX, LONG &lPrLocalY);

public:

	CWaferPr();
	virtual	~CWaferPr();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data content for cold-start
	virtual VOID FlushMessage();		// Flush IPC Message	
	virtual VOID UpdateStationData();

	BOOL IsWaferRotated180();
	INT IM_SetJoystickOn(CONST BOOL bOn);
	VOID IM_ManualDieCompenate(PR_COORD stDieOffset, PR_REAL fDieRotate);
	INT	 OpPrescanInit(UCHAR ucDieNo, BOOL bFastHomeMerge = TRUE);
	BOOL PrescanAutoMultiGrabDone(CDWordArray &dwList, BOOL bDrawDie = FALSE, BOOL bIsFindGT = FALSE, BOOL b2ndPR = FALSE);
	// Adapter Wafer Functions
	INT AdaptMoveWaferTable(CONST LONG lx, CONST LONG ly, CONST DOUBLE dT, LONG &lEncX, LONG &lEncY, DOUBLE &dEncT);
	INT GetWTStatus(LONG &lEncX, LONG &lEncY, LONG &lEncT, DOUBLE &dTheta);
	BOOL OpAutoSearchColletHoleWithEpoxyPattern();
	//General PUBLIC fcns
	VOID CalculateDieCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY);
	VOID CalculateNDieOffsetXY(INT &siStepX, INT &siStepY);		//v3.77
	BOOL IsMouseClickModeAvailable();

	BOOL WPRIndexTest();
	BOOL WprSerachBinEjectorPin(LONG lEjTEnc, LONG lEjTCmd);
	BOOL WaferPrLogFailureCaseImage();

	BOOL IsNGBlock(const LONG lRow, const LONG lCol);

	BOOL UpdateWPRInfoList();
	BOOL SetbGenDieCalibration(BOOL bCalibration);
	BOOL SetszGenDieZoomMode(BOOL bEmpty);

	BOOL LoadPackageMsdWPRData(VOID);
	BOOL SavePackageMsdWPRData(VOID);
	BOOL UpdateWPRPackageList(VOID);

	LONG SetNGGradeScore(LONG lSrchNGDieScore);

	BOOL IsInitPreScanWaferCamFailed()
	{
		return m_bInitPreScanWaferCamFailed;
	}
	VOID BackupBurnInStatus();
	VOID RestoreBurnInStatus();
	VOID SubSearchCurrentDie(const SRCH_TYPE stSrchInfo, REF_TYPE &stInfo);
	LONG SearchAOTDie();
	LONG ChangePrRecordZoomMode(LONG lPrID);
	BOOL PrescanGoodDiePercentageCheck(ULONG ulGoodDie);

	BOOL GetWPRMultiSearchPreScan();

	BOOL IsPrestartFinished();
private:

	DPR_Struct	*m_pDPR;					// Pointer to the DPR structure
	CIMEvent	*m_pIMFunc;					// New Vision IM interface		//v2.83T2
	CAdaptWaferWrapper *m_pAdapterWaferFunc;

	BOOL		m_bAutoBondMode;			//v4.12T1	//Cree US
	BOOL		m_bUseHWTrigger;
	INT			m_unHWGrabState;			//andrewng //2020-0616
	BOOL		m_lLetBPRGrabFirstImage;

	BOOL		m_bPRInit;					// Init flag of PR
	BOOL		m_bDieCalibrated;			// Die Calibration 
	BOOL		m_bPitchLearnt;				// Pitch learnt 
	BOOL		m_bGetBadCutReply;			// Get Reply 4 if badcut is used			

	DOUBLE		m_dCalibX, m_dCalibY;		// Die calibration factor
	DOUBLE		m_dCalibXY, m_dCalibYX;		// Die calibration factor
	DOUBLE		m_dFFCalibX, m_dFFCalibY;
	DOUBLE		m_dFFCalibXY, m_dFFCalibYX;	
	PR_COORD	m_stLearnDieCornerPos[WPR_MAX_DIE_CORNER];	// Learnt normal die corner 
	PR_COORD	m_stDieCenter; 
	PR_REAL		m_fDieAngle;

	PR_WIN		m_stIMEffView;

	PR_WIN		m_stSearchArea;				// Normal die search area
	PR_WIN		m_stLFSearchArea[WPR_MAX_FORWARD];	// Look Forward Search area 

	int			m_siDiePitchXX;				// Die Pitch X (X) Enc Pos
	int			m_siDiePitchXY;				// Die Pitch X (Y) Enc Pos
	int			m_siDiePitchYX;				// Die Pitch Y (X) Enc Pos
	int			m_siDiePitchYY;				// Die Pitch Y (X) Enc Pos

	int			m_siDiePixelXX;				// Die Pitch X (X) PR Pixel
	int			m_siDiePixelXY;				// Die Pitch X (Y) PR Pixel
	int			m_siDiePixelYX;				// Die Pitch Y (X) PR Pixel
	int			m_siDiePixelYY;				// Die Pitch Y (X) PR Pixel
	PR_UWORD	m_uwFrameSubImageNum;
	PR_WIN		m_stSubImageWindow[100];

	DOUBLE		m_dLFSize;					// Look Forward Size
	DOUBLE		m_dLFSizeX;					// Look Forward Size
	DOUBLE		m_dLFSizeY;					// Look Forward Size

	DOUBLE		m_dWTXinUm;					//Wafer Table X resolution retrieved from Machine MSD	//v4.28T5
	DOUBLE		m_dWTYinUm;					//Wafer Table Y resolution retrieved from Machine MSD	//v4.28T5

	DOUBLE		m_dThetaRes;				//Wafer Theta Resolution
	LONG		m_lThetaMotorDirection;
	CString		m_szSoftVersion;			// PR Software Version
	CString		m_szSWReleaseNo;			// PR Software Release Number

	// General die information
	PR_WORD		m_ssGenPRSrchID[WPR_MAX_DIE];
	PR_COORD	m_stGenDieSize[WPR_MAX_DIE];
	BOOL		m_bGenDieLearnt[WPR_MAX_DIE];
	CString		m_szGenDieZoomMode[WPR_MAX_DIE];
	BOOL		m_bGenDieCalibration[WPR_MAX_DIE];

	CString		m_szPkgNameCurrent;
	CString		m_szPkgNamePrevious;
	LONG		m_saPkgRecordCurrent[WPR_MAX_DIE];
	LONG		m_saPkgRecordPrevious[WPR_MAX_DIE];
	BOOL		SaveRuntimeTwoRecords();
	BOOL		LoadRuntimeTwoRecords();
	BOOL		ObtainWftWprFiles(CString& szDevName, CString szCurrPkgName, BOOL bLoadWFT);

	BOOL		m_bGenSrchEnableDefectCheck[WPR_MAX_DIE];
	BOOL		m_bGenSrchEnableChipCheck[WPR_MAX_DIE];

	LONG		m_lGenLrnAlignAlgo[WPR_MAX_DIE];
	LONG		m_lGenLrnBackgroud[WPR_MAX_DIE];
	LONG		m_lGenLrnFineSrch[WPR_MAX_DIE];
	LONG		m_lGenLrnInspMethod[WPR_MAX_DIE];
	LONG		m_lGenLrnInspRes[WPR_MAX_DIE];

	LONG		m_lGenSrchAlignRes[WPR_MAX_DIE];
	LONG		m_lGenSrchDefectThres[WPR_MAX_DIE];
	DOUBLE		m_lGenSrchDieAreaX[WPR_MAX_DIE];
	DOUBLE		m_lGenSrchDieAreaY[WPR_MAX_DIE];
	LONG		m_lGenSrchDieScore[WPR_MAX_DIE];
	LONG		m_lGenSrchGreyLevelDefect[WPR_MAX_DIE];

	DOUBLE		m_dGenSrchSingleDefectArea[WPR_MAX_DIE];
	DOUBLE		m_dGenSrchTotalDefectArea[WPR_MAX_DIE];
	DOUBLE		m_dGenSrchChipArea[WPR_MAX_DIE];


	PR_COORD	m_stBadCutPoint[WPR_BADCUT_POINT];
	LONG		m_stBadCutTolerenceX;
	LONG		m_stBadCutTolerenceY;
	LONG		m_stBadCutScanWidthX;
	LONG		m_stBadCutScanWidthY;

	UCHAR		m_ucGenDieShape;
	UCHAR		GetDieShape();
	UCHAR		CheckDieShape(UCHAR &ucDieShape);

	BOOL		BH_TnZToPick1();
	BOOL		BH_TnZToPick2();
	BOOL		BH_CancelToPrePick1();
	BOOL		BH_CancelToPrePick2();
	BOOL		IsEnableBHMark();

	//Change Collet Center
	LONG		m_lChangeColletTempCenter1X;
	LONG		m_lChangeColletTempCenter1Y;
	LONG		m_lChangeColletTempCenter2X;
	LONG		m_lChangeColletTempCenter2Y;

	//AutoBond use variable
	BOOL		m_bSetAutoBondScreen;
	DOUBLE		m_dCurrentAngle;			// Keep current wafer Angle
	ULONG		m_ulSkipCount;				// Keep the skip count
	ULONG		m_ulNoDieSkipCount;			// Keep the No Die skip count
	ULONG		m_ulPRSkipCount;			// Keep the PR skip count
	ULONG		m_ulBadCutSkipCount;		// Keep the Bad-cut skip count
	ULONG		m_ulMaxSkipCount;			// Maximum Skip count (No Die, Skip & Bad-cut)
	ULONG		m_ulMaxNoDieSkipCount;		// Maximum No Die Skip count
	ULONG		m_ulMaxPRSkipCount;			// Maximum PR Skip count
	ULONG		m_ulMaxBadCutSkipCount;		// Maximum Bad-cut Skip count
	ULONG		m_ulDefectSkipCount;		// Keep the Defective die count during sorting, no reset
	ULONG		m_ulMaxDefectSkipCount;		// Maximum Derect die skip count for accumulative in sorting
	ULONG		m_ulStartSkipCount;			// Counter reset when start
	ULONG		m_ulMaxStartSkipLimit;		// Max skip limit after press start button

	// Event 
	CEvent	m_evWTStable;		// Wafer Table is stable for WPR start
	CEvent	m_evPRLatched;		// WPR latched the image
	CEvent	m_evPRStart;		// WPR Start latching the image
	CEvent	m_evDieReady;		// WPR inspected a good die
	CEvent	m_evBadDie;			// WPR inspected a bad die
	CEvent	m_evBadDieForT;		// WPR inspected a bad die for BondHead T
	CEvent	m_evLFReady;		// Look-Forward result is ready
	CEvent	m_evConfirmSrch;	// Confirm Search event 
	CEvent	m_evBhToPrePick;	// Wait BH to Pre-Pick after Confirm-Search
	CEvent	m_evPreCompensate;	// Look-Forward result is ready			//v2.96T3
	CEvent	m_evBhTReadyForWPR;	// For BH COllet hole measurement on wafer side
	CEvent	m_evWTReadyForWPREmptyCheck;	//v4.54A5	//MS60 NGPick empty Hole checking
	CEvent	m_evWPREmptyCheckDone;			//v4.54A5
	CEvent	m_evInitAFZ;			// AF Z init event for WT T and WL gripper X
	CEvent	m_evSetPRTesting;
	CEvent	m_evSearchMarkReady;
	CEvent	m_evSearchMarkDone;
	CEvent	m_evBhReadyForWPRHwTrigger;
	CEvent	m_evWPRGrabImageReady;

	// For Block Function
	BOOL	m_bCheckAllNmlDie;
	BOOL	m_bCheckAllRefDie;
	LONG	m_lCurrSrchDieType;
	LONG	m_lCurrSrchDieId;
	LONG	m_lMaxSortNmlDieID;
	BOOL	m_bRefCheckRst;
	INT		m_iMapDig1;
	INT		m_iMapDig2;
	INT		m_iWafDig1;
	INT		m_iWafDig2;

	// For Character Die
	BOOL	m_bLrnDigit0;
	BOOL	m_bLrnDigit1;
	BOOL	m_bLrnDigit2;
	BOOL	m_bLrnDigit3;
	BOOL	m_bLrnDigit4;
	BOOL	m_bLrnDigit5;
	BOOL	m_bLrnDigit6;
	BOOL	m_bLrnDigit7;
	BOOL	m_bLrnDigit8;
	BOOL	m_bLrnDigit9;
	BOOL	m_bEnableOCR;

	//For multiple reference die search
	LONG	m_lCurrSrchRefDieId;
	BOOL	m_bAllRefDieCheck;
	BOOL	m_bAutoLearnRefDieDone;		//v2.78T2

	//For set Region by die size
	PR_COORD	m_stNewBadCutPoint[WPR_MAX_DIE_CORNER];
	PR_COORD	m_stDieCoordinate[WPR_MAX_DIE_CORNER];
	INT			m_nPolygonSize;
	DOUBLE		m_dSetRegionByDieSizeX;
	DOUBLE		m_dSetRegionByDieSizeY;
	DOUBLE		m_dSetRegionByDieSizeXCopy;
	DOUBLE		m_dSetRegionByDieSizeYCopy;

	BOOL		m_bWarningNoInspection;

	BOOL		m_bNeedTurnOffLighting;
	LONG		m_lEjtCollet1OffsetX_Pixel;
	LONG		m_lEjtCollet1OffsetY_Pixel;
	LONG		m_lEjtCollet2OffsetX_Pixel;
	LONG		m_lEjtCollet2OffsetY_Pixel;

	DOUBLE		m_dMaxEpoxySizeFactor;			//v4.57A1

	BOOL m_bInitPreScanWaferCamFailed;
	DOUBLE m_dEjtXYRes;
	DOUBLE m_dWTXYRes;
	DOUBLE m_dBTResolution_UM_CNT;
private:
	// Set Event (WPR_Event.cpp)
	VOID SetWTStable(BOOL bState = TRUE);
	VOID SetPRLatched(BOOL bState = TRUE);
	VOID SetPRStart(BOOL bState = TRUE);
	VOID SetDieReady(BOOL bState = TRUE);
	VOID SetBadDie(BOOL bState = TRUE, CString szLog="");	//v4.49A3
	VOID SetBadDieForT(BOOL bState = TRUE);
	VOID SetLFReady(BOOL bState = TRUE);
	VOID SetConfirmSrch(BOOL bState = TRUE);
	VOID SetBhToPrePick(BOOL bState = TRUE);
	VOID SetPreCompensate(BOOL bState = TRUE);				//v2.96T3
	VOID SetBhTReadyForWPR(BOOL bState = TRUE);				//v3.34
	VOID SetWTReadyForWPREmptyCheck(BOOL bState = TRUE);	//v4.54A5
	VOID SetWPREmptyCheckDone(BOOL bState = TRUE);			//v4.54A5
	VOID SetInitAFZState(BOOL bState = TRUE);
	VOID SetPRTesting(BOOL bState = FALSE);
	VOID SetSearchMarkReady(BOOL bState = TRUE);
	VOID SetSearchMarkDone(BOOL bState = TRUE);
	VOID SetBhReadyForWPRHwTrigger(BOOL bState = TRUE);
	VOID SetWPRGrabImageReady(BOOL bState = TRUE, CString szMsg = "");

	// Wait Event (WPR_Event.cpp)
	BOOL WaitWTStable(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBhToPrePick(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBhTReadyForWPR(INT nTimeout = LOCK_TIMEOUT);	//v3.34
	BOOL WaitWTReadyForWPREmptyCheck(INT nTimeout = LOCK_TIMEOUT);	//v4.54A5
	BOOL WaitWPREmptyCheckDone(INT nTimeout = LOCK_TIMEOUT);		//v4.54A5
	BOOL WaitSetPRTesting(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBhReadyForWPRHwTrigger(INT nTimeout = LOCK_TIMEOUT);

	// Sub-State Functions (WPR_SubState.cpp)
	INT OpInitialize();
	INT OpPreStart();
	VOID OpUpdateDieIndex();		// Update Die Index
	BOOL OpIsMaxSkipCount();		// Check the skip count
	BOOL OpSearchMultipleDie();		// Search Multiple die (Normal die or ref die)
	BOOL OpResearchNormalDieAtReferPosn();
	BOOL OpAutoDieResultOnly();
	BOOL OpConfirmSearch();
	BOOL OpAutoResearchDie1();		//v4.41T5
	BOOL OpAutoResearchDie2();		//v4.41T5
	BOOL SearchBHColletHole(const BOOL bEpoxyPattern);
	BOOL SearchBHMark();
	BOOL OpAutoSearchBHMark();
	BOOL SearchColletHoleWithEpoxyPattern();
	
	BOOL OpAutoSearchEmptyDie();	//v4.54A5	//MS60 New NGPick EmptyDieCheck fcn
	BOOL OpSetPRTesting();
	BOOL OpAutoLogPRCase(BOOL bForceEnable, ULONG ulPRCase);
	// SECS function	
	VOID SendCE_DebugPr(VOID);
	VOID SendCE_SearchParam(VOID);

	//v4.04
	//MS100 AOI Zoom optics motion fcns (Option)	
	VOID GetAxisInformation();			//NuMotion MSD file access
	INT Z_Home();
	INT Z_PowerOn(BOOL bOn);
	INT Z_Move(INT nPos, INT nMode = SFM_WAIT);
	INT Z_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT	Z_Sync();

	//v4.24		//ES101 Backlight Z stepper motor
	INT BLZ_Home();						
	INT BLZ_PowerOn(BOOL bOn);
	INT BLZ_Move(INT nPos, INT nMode = SFM_WAIT);
	INT BLZ_MoveTo(INT nPos, INT nMode = SFM_WAIT);
	INT BLZ_Sync();

	BOOL	MoveBackLightWithTableCheck(CONST BOOL bUp, CONST BOOL bToLocateDie = FALSE);
	LONG	WprLearnDiePitch(CONST BOOL bAuto);

	//Motorized Zoom functions (MS899)
	BOOL InitCameraMotors();
	BOOL ResumeCameraStatus();
	INT Z_HomeZoom();
	INT Z_HomeFocus();
	INT SetZoomZPower(CONST BOOL bSet);
	INT SetFocusZPower(CONST BOOL bSet);
	INT Z_Zoom_Move(INT nPos, INT nMode = SFM_WAIT, BOOL bCheckHome = TRUE);
	INT Z_Focus_Move(INT nPos, INT nMode = SFM_WAIT, BOOL bCheckHome = TRUE);

	INT UpdateDirectCornerSearchPos(INT lX, INT lY);

	VOID IMGetPRDeviceId();

	LONG AutoLookAroundDie_Cmd(IPC_CServiceMessage &svMsg);
	LONG AutoLookAroundNearDie(IPC_CServiceMessage &svMsg);

	LONG TurnOnOffPrLightingCmd(IPC_CServiceMessage &svMsg);

	LONG RefreshAllRingLighting(IPC_CServiceMessage &svMsg);

	//General
	LONG MeasureObject(IPC_CServiceMessage &svMsg);
	LONG SelectCamera(IPC_CServiceMessage &svMsg);
	LONG SelectWaferCamera(IPC_CServiceMessage &svMsg);
	LONG SelectBondCamera(IPC_CServiceMessage &svMsg);
	LONG SelectPostSealCamera(IPC_CServiceMessage &svMsg);		//v2.83T61	//v3.70T2
	LONG TurnOffPSLighting(IPC_CServiceMessage &svMsg);
	LONG SwitchToTarget(IPC_CServiceMessage &svMsg);
	LONG SwitchToTarget_dbh(IPC_CServiceMessage &svMsg);		//v3.93
	LONG SetGeneralLighting(IPC_CServiceMessage &svMsg);
	LONG SetGeneralLighting2(IPC_CServiceMessage &svMsg);		//v4.46T28
	LONG GetGeneralLighting(IPC_CServiceMessage &svMsg);
	LONG SetGeneralExposureTime(IPC_CServiceMessage &svMsg);	//v3.30T1
	LONG RestoreGeneralLighting(IPC_CServiceMessage &svMsg);
	LONG UpdateHMIData(IPC_CServiceMessage &svMsg);
	LONG UpdateAction(IPC_CServiceMessage &svMsg);
	LONG LogItems(IPC_CServiceMessage &svMsg);
	LONG ResetPrCenterXY(IPC_CServiceMessage &svMsg);			//v3.80
	LONG UpdatePrCenterXY(IPC_CServiceMessage &svMsg);			//v4.08
	LONG UpdateDefectFromHmi(IPC_CServiceMessage &svMsg);
	LONG CheckLearnAlignAlgorithm(IPC_CServiceMessage &svMsg);
	LONG RecoverBackupLighting(IPC_CServiceMessage &svMsg);

	
	//Learn Die
	LONG WprAutoColorAdjust(IPC_CServiceMessage &svMsg);
	LONG WprLiveColorAdjust(IPC_CServiceMessage &svMsg);
	VOID BHMarkMoveToPrePick(const LONG bSelectDieType, const LONG lCurRefDieNo);
	LONG CancelLearnDie(IPC_CServiceMessage &svMsg);
	LONG ConfirmLearnDie(IPC_CServiceMessage &svMsg);
	LONG GetLearnDieLighting(IPC_CServiceMessage &svMsg);
	LONG SetLearnDieLighting(IPC_CServiceMessage &svMsg);
	LONG SetLearnDieLighting2(IPC_CServiceMessage &svMsg);
	LONG SetExposureTime(IPC_CServiceMessage &svMsg);
	LONG MovePRCursor(IPC_CServiceMessage &svMsg);
	LONG AutoLearnFFModeDie();
	LONG AutoLearnDie(IPC_CServiceMessage &svMsg);
	BOOL AutoUpLookSearchDie_Rpy1(IPC_CServiceMessage &svMsg);  //4.52D17
	LONG AutoDetectDie(IPC_CServiceMessage &svMsg);
	LONG AutoLearnRefDie(IPC_CServiceMessage &svMsg);					//v2.78T1
	LONG GenerateConfigData(IPC_CServiceMessage &svMsg);	

	LONG CheckRebel_R2N_Cmd(IPC_CServiceMessage &svMsg);	// corner refer to normal check
	LONG CheckRebel_N2R_Cmd(IPC_CServiceMessage &svMsg);	// corner normal to refer check
	LONG GetReferDieSize(IPC_CServiceMessage &svMsg);
	BOOL CheckRebel_RC2N_Location(BOOL bAutoLearnDie);
	BOOL CheckRebel_RUL2N_Location(BOOL bAutoLearnDie);
	BOOL CheckRebel_RR2N_Location(BOOL bAutoLearnDie);
	BOOL CheckRebel_N2RC_Location(BOOL bFullyAuto);
	BOOL CheckRebel_N2R4_Location(ULONG ulCornerType);
	LONG GetRebel_R_N_Pitch(LONG &lPitchX, LONG &lPitchY);	//	always in absolute value.
	LONG GetRebel_R_DieSize(INT &lRDieSizeX, INT &lRDieSizeY, UCHAR ucReferPrID);
	BOOL SearchAllDie(BOOL bReferDie, LONG lPrID);

	LONG EnableLookForwardCmd(IPC_CServiceMessage &svMsg);

	//Search Die
	LONG ConfirmSearchDiePara(IPC_CServiceMessage &svMsg);
	LONG ConfirmSearchDieArea(IPC_CServiceMessage &svMsg);
	LONG ConfirmSearchDieAreaNew(IPC_CServiceMessage &svMsg);
	LONG UserSearchDie(IPC_CServiceMessage &svMsg);
	LONG UserSearchDie_CheckNoDie(IPC_CServiceMessage &svMsg);			//v3.98T3	//For auto-Learn Ej fcn
	LONG UserSearchDie_Rotate(IPC_CServiceMessage &svMsg);
	LONG UserSearchDie_NoMove(IPC_CServiceMessage &svMsg);
	LONG UserSearchNormalDie_NoMove(IPC_CServiceMessage &svMsg);
	BOOL UserSearchMark(const CString szTitle, const LONG lLoopTest, const LONG lArmNo, const LONG lDieNo, PR_COORD &stDieOffset, PR_REAL &fDieRotate, LONG &lBHMarkRefX, LONG &lBHMarkRefY);
	LONG UserSearchMark(IPC_CServiceMessage &svMsg, 
					  const CString szTitle, const LONG lArmNo, const LONG lDieNo,
					  LONG &lBHColletHoleRefX, LONG &lBHColletHoleRefY,
					  LONG &lBHMarkRefX, LONG &lBHMarkRefY);
	LONG UserSearchMark1(IPC_CServiceMessage &svMsg);
	LONG UserSearchMark2(IPC_CServiceMessage &svMsg);
	LONG UserSearchColletHole1(IPC_CServiceMessage &svMsg);
	LONG UserSearchColletHole2(IPC_CServiceMessage &svMsg);
	LONG UserSearchRefDie_NoMove(IPC_CServiceMessage &svMsg);			//v3.70T3
	LONG UserSearchRefDie(IPC_CServiceMessage &svMsg);
	LONG UserSearchAllRefDie(IPC_CServiceMessage &svMsg);
	BOOL UserSearchAllDie(IPC_CServiceMessage &svMsg);
	BOOL UserSearchUpLookDie(LONG lDieID, BOOL bF1Srch);
	LONG UserSearchUpLookDieCmd(IPC_CServiceMessage &svMsg);
	LONG UserSearchLookForwardDie(IPC_CServiceMessage &svMsg);
	LONG ManualFailureCaseLog(IPC_CServiceMessage &svMsg);

	LONG SearchCurrentDie(IPC_CServiceMessage &svMsg);
	LONG SearchCurrentDie_PLLM_REBEL(IPC_CServiceMessage &svMsg);		//v4.41T1
	LONG SearchDieInSearchWindow(IPC_CServiceMessage &svMsg);
	LONG SearchCurrentDieInFOV(IPC_CServiceMessage &svMsg);				//v3.04
	LONG SearchDieInBigWindow(IPC_CServiceMessage &svMsg);
	LONG HotKeySearchRefDie(IPC_CServiceMessage &svMsg);

	LONG UserSearchRefDie_NoMove_GivenID(IPC_CServiceMessage &svMsg);	//v3.70T3
	LONG SearchCurrentDieInFOV_Spiral(IPC_CServiceMessage &svMsg);		//v3.04

	//Other setup	
	LONG LearnDiePitch(IPC_CServiceMessage &svMsg);
	LONG LearnDieCalibration(IPC_CServiceMessage &svMsg);
	LONG WprLearnDieCalibration();
	LONG DeletePRRecord(IPC_CServiceMessage &svMsg);
	LONG DisplayPRRecord(IPC_CServiceMessage &svMsg);
	LONG EnableDebugFlag(IPC_CServiceMessage &svMsg);
	LONG EnableDebugLog(IPC_CServiceMessage &svMsg);
	LONG DisplayLFSearchRegion(IPC_CServiceMessage &svMsg);				//v3.13T3
	LONG UpdateSoraaDiePitch(IPC_CServiceMessage &svMsg);				//v4.28T5
	LONG SetPrDebugFlag(IPC_CServiceMessage &svMsg);
	LONG DisplayPRIdListRecord(IPC_CServiceMessage &svMsg);

//	Chip Prober probing position offset PO
	LONG GridFindDieCalibration(LONG lPrDelay, INT nSubCut = 1);
	LONG GridLearnDieCalibration(IPC_CServiceMessage &svMsg);
	LONG GridLearnDieCalibration_AutoMode(IPC_CServiceMessage &svMsg);
	BOOL CP_GridFixDieCalibration(LONG lPrDelay);
	CString m_szGridDCLog;
	LONG CP100CheckTableLinearity(IPC_CServiceMessage &svMsg);
	LONG CPCheckOpticsDistortion(IPC_CServiceMessage &svMsg);
	LONG DrawBlockProbeDie(IPC_CServiceMessage &svMsg);
	BOOL MoveWftNoCheck(LONG lXAxis, LONG lYAxis, BOOL bToUnload, INT nMode, BOOL bWT2);
	LONG	m_lLCMoveDistance;
	LONG	m_lLCMoveDelay;
	BOOL	m_bLCUseBondCam;
	LONG	m_lLCMoveTotalLimit;
	DOUBLE	m_dGridCalibX;
	DOUBLE	m_dGridCalibXY;
	DOUBLE	m_dGridCalibY;
	DOUBLE	m_dGridCalibYX;
	BOOL	m_bNoWprBurnIn;
	DOUBLE	m_dDieThicknessInput;
//	Chip Prober probing position offset PO

	//Debug only
	LONG OfflineSearchDie(IPC_CServiceMessage &svMsg);

	LONG ToggleJoystickSpeed(IPC_CServiceMessage &svMsg);

	// Mouse
	LONG UsePRMouse(IPC_CServiceMessage &svMsg);
	LONG DrawComplete(IPC_CServiceMessage &svMsg);
	LONG CancelMouseDraw(IPC_CServiceMessage &svMsg);

	LONG SetInspDetectRegion(IPC_CServiceMessage &svMsg);
	LONG EndInspDetectRegion(IPC_CServiceMessage &svMsg);
	LONG SetInspIgnoreRegion(IPC_CServiceMessage &svMsg);
	LONG EndInspIgnoreRegion(IPC_CServiceMessage &svMsg);

	LONG SetAlnDetectRegion(IPC_CServiceMessage &svMsg);
	LONG EndAlnDetectRegion(IPC_CServiceMessage &svMsg);
	LONG SetAlnIgnoreRegion(IPC_CServiceMessage &svMsg);
	LONG EndAlnIgnoreRegion(IPC_CServiceMessage &svMsg);

	// Upload and Download PR records
	LONG UploadPrRecords(IPC_CServiceMessage &svMsg);
	LONG DownloadPrRecords(IPC_CServiceMessage &svMsg);
	LONG DeletePrRecords(IPC_CServiceMessage &svMsg);
	LONG RunTimeDownloadPrRecords(IPC_CServiceMessage &svMsg);
	LONG PkgKeyParametersTask(IPC_CServiceMessage &svMsg);

	LONG UploadPrPkgPreviewImage(IPC_CServiceMessage &svMsg);

	// Set Badcut region
	LONG BadCutSetupStart(IPC_CServiceMessage &svMsg);
	LONG BadCutSetupStop(IPC_CServiceMessage &svMsg);

	// Use in wafer loader
	LONG FindFirstDieInWafer(IPC_CServiceMessage &svMsg);
	LONG FindFirstDieAngleInWafer(IPC_CServiceMessage &svMsg);			//xyz
	LONG AlignWaferAngle(IPC_CServiceMessage &svMsg);
	LONG SpiralSearchHomeDie(IPC_CServiceMessage &svMsg);
	LONG SpiralSearchRefDie(IPC_CServiceMessage &svMsg);
	LONG CornerSearchHomeDie(IPC_CServiceMessage &svMsg);
	LONG TestHomeDiePattern(IPC_CServiceMessage &svMsg);
	LONG LookAheadULeftRectWaferCorner(IPC_CServiceMessage &svMsg);		//v2.82
	LONG LookAheadURightRectWaferCorner(IPC_CServiceMessage &svMsg);	//v4.21T3
	LONG LookAheadLRightRectWaferCorner(IPC_CServiceMessage &svMsg);	//v2.82
	LONG LookAheadLRightRectWaferCornerWithIncompleteLastRow(IPC_CServiceMessage &svMsg);	//v2.82
	LONG LookAheadLLeftRectWaferCorner(IPC_CServiceMessage &svMsg);		//v2.82
	LONG LookAheadLLeftRectWaferWithInCompleteCorner(IPC_CServiceMessage &svMsg);
	LONG LookAroundBaseOnMap(IPC_CServiceMessage &svMsg);
	LONG LookAroundNormalDie(IPC_CServiceMessage &svMsg);				//v3.13T3
	LONG GetFOVSize(IPC_CServiceMessage &svMsg);						//v3.13T3
	LONG SearchAlignPattern(IPC_CServiceMessage &svMsg);				//v3.42
	LONG TurnOnPostSealLighting(IPC_CServiceMessage &svMsg);			//v3.71T5	//PLLM REBEL

	// PR Dialog Box
	LONG ShowPrDiagnosticDialog(IPC_CServiceMessage &svMsg);
	LONG ShowPrServiceDialog(IPC_CServiceMessage &svMsg);
	LONG ShowPrColorDialog(IPC_CServiceMessage &svMsg);

	// PR ID checking function
	LONG CheckRecordID(IPC_CServiceMessage &svMsg);

	//Check Die Die learn	
	LONG IsDieLearnt(IPC_CServiceMessage &svMsg);
	LONG DownloadMultiSearchRecord(IPC_CServiceMessage &svMsg);

	//Check Die Inspection Set
	LONG IsInspectionSet(IPC_CServiceMessage &svMsg);

	//Adaptive Learn Die (load more die)
	LONG AdaptiveLearnDie(IPC_CServiceMessage &svMsg);

	// For Block Function
	LONG SearchReferDieInWafer(IPC_CServiceMessage &svMsg);
	LONG SearchNormalDieInWafer(IPC_CServiceMessage &svMsg);
	LONG SearchDieInWafer(IPC_CServiceMessage &svMsg);
	LONG SearchDigitalInWafer(IPC_CServiceMessage &svMsg);				//v3.00T1
	LONG SearchNmlDieAround(IPC_CServiceMessage &svMsg);				//v3.28T1	//BLOCKPICK

	LONG SetupPrCrossHair(IPC_CServiceMessage &svMsg);
	LONG AdjustCrossHairStepByStep(IPC_CServiceMessage &svMsg);
	
	// Select Character die No
	LONG SelectLearnCharDie(IPC_CServiceMessage &svMsg);
	LONG SelectCharDieNo(IPC_CServiceMessage &svMsg);
	LONG UserSearchCharDie(IPC_CServiceMessage &svMsg);
	LONG SearchCharDieInFOV(IPC_CServiceMessage &svMsg);
	LONG SelectAoiOcrDieNo(IPC_CServiceMessage &svMsg);
	LONG SetOcrDieDetectWindow(IPC_CServiceMessage &svMsg);
	LONG GetOcrDieDetectWindow(IPC_CServiceMessage &svMsg);
	LONG ResetOcrDieWindows(IPC_CServiceMessage &svMsg);
	LONG LoadAoiGradeConvertFile(IPC_CServiceMessage &svMsg);

	// Switch PR Mode
	VOID SetPRSysBondMode(IPC_CServiceMessage &svMsg);
	VOID SetPRSysLiveMode(IPC_CServiceMessage &svMsg);
	LONG BadCutSetupUsingDieSize(IPC_CServiceMessage &svMsg);
	LONG BadCutBinaryThreshold(IPC_CServiceMessage &svMsg);

	//v4.04
	//MS100 AOI Zoom Z motor
	LONG HomeAOIZoomZ(IPC_CServiceMessage &svMsg);
	LONG PowerOnAOIZoomZ(IPC_CServiceMessage &svMsg);
	LONG MoveAOIZoomZ(IPC_CServiceMessage &svMsg);

	// Motorized Zoom fucntions
	LONG MoveZoom(IPC_CServiceMessage &svMsg);
	LONG MoveFocus(IPC_CServiceMessage &svMsg);
	LONG PowerOnZoom(IPC_CServiceMessage &svMsg);
	LONG PowerOnFocus(IPC_CServiceMessage &svMsg);
	LONG MoveZoomToHome(IPC_CServiceMessage &svMsg);
	LONG MoveFocusToHome(IPC_CServiceMessage &svMsg);
	LONG AutoFocus(IPC_CServiceMessage &svMsg);
	LONG ConfirmFocusLevel(IPC_CServiceMessage &svMsg);
	LONG GotoFocusLevel(IPC_CServiceMessage &svMsg);
	LONG GotoTableFocus(IPC_CServiceMessage &svMsg);
	LONG FocusZSync(IPC_CServiceMessage &svMsg);
	LONG GotoHomeLevel(IPC_CServiceMessage &svMsg);
	LONG PR_AutoFocus(CString &szMsg);
	VOID WPR_GetEncoderValue();
	BOOL IsWT1UnderCamera();
	BOOL IsWT2UnderCamera();
	VOID GetES101WTEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis, BOOL bWT2 = FALSE);
	VOID MoveWaferThetaTo(LONG lStep, BOOL bWT2);

	LONG HomeBackLightElevator(IPC_CServiceMessage &svMsg);
	LONG PowerOnBackLightElevator(IPC_CServiceMessage &svMsg);
	LONG MoveBackLightElevator(IPC_CServiceMessage &svMsg);

	LONG ChangeBackLightElevatorPos(IPC_CServiceMessage &svMsg);
	LONG ConfirmBackLightElevatorPos(IPC_CServiceMessage &svMsg);
	LONG CancelBackLightPos(IPC_CServiceMessage &svMsg);
	LONG KeyInElevatorPos(IPC_CServiceMessage &svMsg);
	LONG MoveElevatorPositivePos(IPC_CServiceMessage &svMsg);
	LONG MoveElevatorNegativePos(IPC_CServiceMessage &svMsg);

	LONG MoveBackLightToSafeLevel(IPC_CServiceMessage &svMsg);
	LONG MoveBackLightToUpStandby(IPC_CServiceMessage &svMsg);
	LONG MoveBackLightToUpStandbyRoutine(IPC_CServiceMessage &svMsg);
	LONG BondMoveBackLightToStandby(IPC_CServiceMessage &svMsg);

	LONG StartBackLightElevatorMotionTest(IPC_CServiceMessage &svMsg);

	LONG LockLiveViewZoom(IPC_CServiceMessage &svMsg);
	LONG LiveViewZoomCmd(IPC_CServiceMessage &svMsg);
	LONG LiveViewConZoomCmd(IPC_CServiceMessage &svMsg);
	LONG LiveViewRoiZoomCmd(IPC_CServiceMessage &svMsg);
	LONG ZoomScreenCmd(IPC_CServiceMessage &svMsg);
	LONG ToggleZoomViewCmd(IPC_CServiceMessage &svMsg);
	LONG ToggleNml4XCmd(IPC_CServiceMessage &svMsg);
	LONG ToggleAutoZoomViewCmd(IPC_CServiceMessage &svMsg);
	LONG ToggleLearnColletZoomScreenCmd(IPC_CServiceMessage &svMsg);
	LONG ToggleZoomScreenCmd(IPC_CServiceMessage &svMsg);
	LONG SwitchWaferDualPath(IPC_CServiceMessage &svMsg);

	//Draw Center
	VOID InitCenter();
	LONG DisplayEjtColletOffsetCenter1(IPC_CServiceMessage &svMsg);
	LONG DisplayEjtColletOffsetCenter2(IPC_CServiceMessage &svMsg);
	LONG WPR_EjtDisplayCollet_Move_Up(IPC_CServiceMessage &svMsg);
	LONG WPR_EjtDisplayCollet_Move_Down(IPC_CServiceMessage &svMsg);
	LONG WPR_EjtDisplayCollet_Move_Left(IPC_CServiceMessage &svMsg);
	LONG WPR_EjtDisplayCollet_Move_Right(IPC_CServiceMessage &svMsg);
	VOID CalcEjtColletOffsetCount(const LONG lEjtColletCoorX, const LONG lEjtColletCoorY, LONG &nStepX, LONG &nStepY);
	VOID CalcEjtColletOffset(const DOUBLE dRatio = 1);
	VOID CalcBTColletOffset(const DOUBLE dRatio,
							const LONG lChangeColletTempCenter1X, const LONG lChangeColletTempCenter1Y,
							const LONG lChangeColletTempCenter2X, const LONG lChangeColletTempCenter2Y);
	LONG WPR_ConfirmEjtSetUp(IPC_CServiceMessage &svMsg);
	LONG WPR_UpdateAGCOffset(IPC_CServiceMessage &svMsg);	//v4.50A31
	LONG WPR_ResetEjtCenter(IPC_CServiceMessage &svMsg);
	LONG EnableEjtColletOffset(IPC_CServiceMessage &svMsg);
	LONG IsFaceValueExists(IPC_CServiceMessage& svMsg);		//v4.48A26	//Avago

	LONG ManualLearnArmHole(IPC_CServiceMessage& svMsg);	

public:

	INT AF_Z_Move(INT nPos, INT nMode = SFM_WAIT);
	//v4.04
	VOID ChangeDirectView(const BOOL bDirectView);
	LONG	GetSortFovCol();
	LONG	GetSortFovRow();

private:

	LONG SetSrchThreadNumCmd(IPC_CServiceMessage &svMsg);
	LONG ClearDisplay(IPC_CServiceMessage &svMsg);

	//IM Support cmds	//v2.82T3
	LONG InitIMPopupPage(IPC_CServiceMessage &svMsg);
	LONG InitIMCriteriaPage(IPC_CServiceMessage &svMsg);
	LONG IMUserSearchDie(IPC_CServiceMessage &svMsg);
	// old function for WT to setup the adapt wafer
	LONG InitHighLevelVisionInfo(IPC_CServiceMessage &svMsg);
	// function to setup adapt wafer when learn die
	LONG IM_InitPrHighLevelInfoCmd(IPC_CServiceMessage &svMsg);
	LONG IMAutoLearnWafer(IPC_CServiceMessage &svMsg);				//v3.06
	LONG IMManualAutoLearnWafer(IPC_CServiceMessage &svMsg);
	LONG IMManualAutoLearnWaferSeperated(IPC_CServiceMessage &svMsg);
	LONG IMChangeWafer(IPC_CServiceMessage &svMsg);					//v3.28T1
	LONG IM_StartWafer(IPC_CServiceMessage &svMsg);
	LONG IMWaferEndChecking(IPC_CServiceMessage &svMsg);
	LONG IMGetPRDeviceIdCmd(IPC_CServiceMessage &svMsg);
	LONG SetLockPrMouseJoystickCmd(IPC_CServiceMessage &svMsg);
	LONG SpiralSearchDie(IPC_CServiceMessage &svMsg);
	LONG ClearBadCutPoints(IPC_CServiceMessage &svMsg);
	LONG AdaptWaferInitHighLevelVisionInfo(IPC_CServiceMessage &svMsg);

	//Teach COR display
	LONG DrawCORCirclePattern(IPC_CServiceMessage &svMsg);
	
	LONG SetMouseControlCmd(IPC_CServiceMessage &svMsg);

	LONG CopyRefDieSettings(IPC_CServiceMessage &svMsg);
	LONG Search2DBarCode(IPC_CServiceMessage &svMsg);				//v3.70T2
	LONG Search2DBarCodeFromWafer(IPC_CServiceMessage &svMsg);	

	LONG RemoveShowImages(IPC_CServiceMessage &svMsg);
	LONG PreOnSelectShowPrImage(IPC_CServiceMessage &svMsg);
	LONG OnSelectShowPrImage(IPC_CServiceMessage &svMsg);
	LONG DisplayPRRecordInSummary(IPC_CServiceMessage &svMsg);

	// Save Display Image in indirect view
	LONG SearchAndSaveNormalDieImages(IPC_CServiceMessage &svMsg);
	LONG SearchAndSaveReferDieImages(IPC_CServiceMessage &svMsg);
	LONG CounterCheckReferOnNormalDie(IPC_CServiceMessage &svMsg);
	//v4.41T6	//SanAn
	LONG EnablePickAndPlace(IPC_CServiceMessage &svMsg);
	//v4.46T1   //Nanojoin
	LONG ItemMapToTop(IPC_CServiceMessage &svMsg);

protected:

	PR_WIN GetSrchArea();

	// Dual Path
	PR_CAMERA	GetScnCamID();
	PR_UBYTE	GetScnSenID();
	PR_UBYTE	GetScnRecID();
	PR_PURPOSE	GetScnPurpose();

	PR_CAMERA	GetRunCamID();
	PR_UBYTE	GetRunSenID();
	PR_UBYTE	GetRunRecID();
	PR_PURPOSE	GetRunPurposeI();
	PR_PURPOSE	GetRunPurposeG();

	LONG	GetRunDigitalZoom();
	LONG	GetRunZoom();
	LONG	GetNmlZoom();
	LONG	GetScnZoom();
	UCHAR	GetPrescanPrID();
	UCHAR	GetPrescan2ndPrID();
//	zoomview	begin
	DOUBLE	GetCalibX();
	DOUBLE	GetCalibY();
	DOUBLE	GetCalibXY();
	DOUBLE	GetCalibYX();
	PR_WORD GetNmlSizePixelX();
	PR_WORD GetNmlSizePixelY();
	PR_WORD GetDieSizePixelX(LONG lDieNo);
	PR_WORD GetDieSizePixelY(LONG lDieNo);

	//v4.49A3
	BOOL DumpAppLog(PR_WORD wRecordID);
	VOID CalculateExtraExposureGrabTime();
	BOOL LogLearnDieRecord(LONG lDieNo, INT nCamNo, INT nPurpose, 
							UCHAR ucDieShape, USHORT usPRLrnStatus, 
							INT nSendID, INT nRecvID);		//v4.49A10
	BOOL LogUserSearchDieResult(LONG lDieNo, USHORT usPrResult, 
							INT nX, INT nY, FLOAT fDieRotation, FLOAT fDieScore,
							PR_COORD stCorner1, PR_COORD stCorner2,
							INT nSendID, INT nRecvID);		//v4.49A10
	CString GetPurposeInString(INT nPurpose);				//v4.49A10

	int		GetPitchPixelXX();
	int		GetPitchPixelYY();
	int		GetPitchPixelXY();	// normal sorting die pitch
	int		GetPitchPixelYX();

	DOUBLE	GetScanCalibX();
	DOUBLE	GetScanCalibY();
	DOUBLE	GetScanCalibXY();
	DOUBLE	GetScanCalibYX();
	int		GetScanPitchPixelXX();
	int		GetScanPitchPixelYY();
	PR_WORD GetScanNmlSizePixelX();
	PR_WORD GetScanNmlSizePixelY();
	PR_WORD GetScanDieSizePixelX(UCHAR ucDieNo);
	PR_WORD GetScanDieSizePixelY(UCHAR ucDieNo);

	VOID	DrawSearchBox(PR_COLOR ssColor);
	DOUBLE	ConvertScanDUnitToPixel(CONST DOUBLE dUnit);
//	zoomview	end	done

	INT	 GetDiePitchXX();
	INT	 GetDiePitchXY();
	INT	 GetDiePitchYY();
	INT	 GetDiePitchYX();

	LONG GetPrCenterX();
	LONG GetPrCenterY();
	VOID SetPrCenterX(LONG lPrCenterX);
	VOID SetPrCenterY(LONG lPrCenterY);

	DOUBLE GetSrchDieAreaX();
	DOUBLE GetSrchDieAreaY();
	DOUBLE GetGenSrchAreaX(INT nDieNo);
	DOUBLE GetGenSrchAreaY(INT nDieNo);

	CString m_szWPRConfigFileName;

	LONG	GetWprDieNo();
	
	//HMI use variable -- No need to save in file
	LONG	m_lHmiPrCenterX;
	LONG	m_lHmiPrCenterY;

	LONG	m_lPrCenterX;					//v3.80
	LONG	m_lPrCenterY;					//v3.80
	LONG	m_lPrUplookBH1LearnCenterX;		//4.52D17
	LONG	m_lPrUplookBH1LearnCenterY;
	LONG	m_lPrUplookBH2LearnCenterX;
	LONG	m_lPrUplookBH2LearnCenterY;
	LONG	m_lPrCenterXInPixel;			//v4.08
	LONG	m_lPrCenterYInPixel;			//v4.08
	LONG	m_lWaferToBondPrCenterOffsetX;	//v4.08
	LONG	m_lWaferToBondPrCenterOffsetY;	//v4.08

	LONG	m_lAoiOcrBoxHeight;
	LONG	m_lAoiOcrBoxWidth;
	BOOL	m_bAoiOcrDieLearnt;
	BOOL	m_bUseAoiGradeConvert;
	BOOL	m_bScanNgGradeConvert;
	LONG	m_lGradeMappingLimit;
	SHORT	m_sMapOrgGrade[WPR_AOI_GRADE_MAX_MAPPING];
	SHORT	m_sPrInspectGrade[WPR_AOI_GRADE_MAX_MAPPING];
	SHORT	m_sMapNewGrade[WPR_AOI_GRADE_MAX_MAPPING];
	CString	m_szAoiMapInValue;
	CString m_szAoiLearnOcrValue;
	PR_WIN	m_stOcrDetectWin[WPR_OCR_MAX_WINDOWS];

	BOOL	m_bLearnLookupCamera;	//4.52D17
	BOOL	m_bDieIsLearnt;
	BOOL	m_bSelectDieType;	
	BOOL	m_bSrchDiePara;
	BOOL	m_bCurrentCamera;
	BOOL	m_bKeepShowBondInAuto;
	ULONG	m_ulAutoSwitchPrCount;
	BOOL	m_bPostSealCamera;				//v3.70T2
	UINT	m_unCurrPostSealID;	//v4.39T11			
	BOOL	m_bDebugVideoTest;	
	BOOL	m_bSrchEnableDefectCheck;
	BOOL	m_bSrchEnableChipCheck;
	BOOL	m_bEnableLrnAlignAlgo;	
	LONG	m_lDebugFlag;	
	LONG	m_lDebugLog;	
	LONG	m_lLrnAlignAlgo;
	LONG	m_lLrnBackground;
	LONG	m_lLrnFineSrch;
	LONG	m_lLrnInspMethod;
	LONG	m_lLrnInspRes;
	LONG	m_lSrchAlignRes;
	LONG	m_lSrchDefectThres;
	DOUBLE	m_lSrchDieAreaX;
	DOUBLE	m_lSrchDieAreaY;
	DOUBLE	m_dSrchDieAreaX;			//v2.60
	DOUBLE	m_dSrchDieAreaY;			//v2.60
	LONG	m_lSrchDieScore;
	LONG	m_lSrchGreyLevelDefect;
	DOUBLE	m_dSrchSingleDefectArea;
	DOUBLE	m_dSrchTotalDefectArea;
	DOUBLE	m_dSrchChipArea;
	BOOL	m_bPrCircleDetection;		//v2.77
	BOOL	m_bAutoLearnRefDie;			//v2.78T2
	LONG	m_lPrCal1stStepSize;		//v2.89T1
	LONG	m_lLearnPitchSpanX;
	LONG	m_lLearnPitchSpanY;
	INT		m_nCurrLFPosn;				//v3.13T3	//for 5x5 LF area only
	BOOL	m_bEnable2Lighting;			//v3.44T1	//SanAn
	ULONG	m_ulOcrDieOrientation;

	ULONG	m_ulLogPRCaseInHQ;
	//v3.17T1
	BOOL	m_bEnableLineDefect;
	DOUBLE	m_dMinLineDefectLength;
	LONG	m_lMinNumOfLineDefects;

	BOOL	m_bAutoEnlargeSrchWnd;

	//For recover light setting
	LONG m_lWPRGenCoaxLightLevel_Backup;
	LONG m_lWPRGenRingLightLevel_Backup;
	LONG m_lWPRGenSideLightLevel_Backup;
	LONG m_lWPRGenBackLightLevel_Backup;
	LONG m_lWPRGenExposureTimeLevel_Backup;
	LONG m_lSCMGenExposureTimeLevel_Backup;

	LONG m_lWPRGenCoaxLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRGenRingLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRGenSideLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRGenBackLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRGenExposureTimeLevel;
	LONG m_lSCMGenExposureTimeLevel;

	LONG m_lWPRLrnCoaxLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRLrnRingLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRLrnSideLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRLrnBackLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRLrnExposureTimeLevel;
	LONG m_lSCMLrnCoaxLightLevel;
	LONG m_lSCMLrnRingLightLevel;
	LONG m_lSCMLrnSideLightLevel;
	LONG m_lSCMLrnExposureTimeLevel;

	LONG m_lWPRTmpCoaxLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRTmpRingLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRTmpSideLightLevel[WPR_MAX_LIGHT_GROUP];
	LONG m_lWPRTmpBackLightLevel[WPR_MAX_LIGHT_GROUP];


	LONG m_lWPRSpecial1_RingLightLevel;	// Yealy ES101 3 layer ring light 
	LONG m_lWPRSpecial2_RingLightLevel;	// Yealy ES101 3 layer ring light 

	LONG m_lWPRLrnCoaxLightHmi;
	LONG m_lWPRLrnRingLightHmi;
	LONG m_lWPRLrnSideLightHmi;
	LONG m_lWPRGenExposureTimeHmi;
	LONG m_lWprLrnExposureTimeHmi;

	BOOL m_bWprUseBackLight;
	BOOL m_bDisplayDieInUM;
	
	// Back light
	LONG m_lWPRGeneral_1; // for up level
	LONG m_lWPRGeneral_2; // for standby level
	LONG m_lWPRGeneral_TMP;
	LONG m_lBackLightElevatorUpLevel;
	LONG m_lBackLightElevatorStandByLevel;
	LONG m_lBLZAutoFocusDelay;

	//v3.70T5
	LONG m_lPPRGenCoaxLightLevel;
	LONG m_lPPRGenRingLightLevel;
	LONG m_lPPRGenSideLightLevel;

	LONG m_lCurNmlDieNo;
	LONG m_lCurRefDieNo;
	LONG m_lCurNormDieID;
	LONG m_lCurNormDieSizeX;
	LONG m_lCurNormDieSizeY;
	DOUBLE m_dCurNormDieSizeX;		//v3.23T1
	DOUBLE m_dCurNormDieSizeY;		//v3.23T1
	LONG m_lCurBadCutSizeX;
	LONG m_lCurBadCutSizeY;
	LONG m_lCurPitchSizeX;
	LONG m_lCurPitchSizeY;
	DOUBLE m_dCurPitchSizeX;
	DOUBLE m_dCurPitchSizeY;
	DOUBLE m_dCurSrchDieSizeX;		//v3.01T1
	DOUBLE m_dCurSrchDieSizeY;		//v3.01T1

	LONG m_lInspDetectWinNo;
	LONG m_lInspIgnoreWinNo;
	LONG m_lAlnDetectWinNo;
	LONG m_lAlnIgnoreWinNo;
	PR_WIN	m_stInspDetectWin[WPR_MAX_DETECT_WIN];	
	PR_WIN	m_stInspIgnoreWin[WPR_MAX_IGNORE_WIN];	
	PR_WIN	m_stAlnDetectWin[WPR_MAX_DETECT_WIN];	
	PR_WIN	m_stAlnIgnoreWin[WPR_MAX_IGNORE_WIN];	


	LONG m_lJoystickLevel_BT;
	LONG m_lJoystickLevel_WT;

	ULONG m_ulWaferCameraColor;
	ULONG m_ulBondCameraColor;
	ULONG m_ul2DCameraColor;		//pllm
	ULONG m_ulPrWindowHeight;
	ULONG m_ulPrWindowWidth;
	BOOL  m_bBinMapShow;
	WAF_CPhysicalInformationMap m_cPIMap;

	ULONG m_ulBH1UplookCameraColor;		//4.52D17
	ULONG m_ulBH2UplookCameraColor;		

	CString m_szSrhDieResult;
	CString m_szLrnDieType;
	CString m_sz2DCodePrTitle;
	UCHAR m_ucDieShape;

	CString m_szDieTypeInfo;

	BOOL	m_bUseMouse;

	DOUBLE	m_dAlignPassScore;
	DOUBLE	m_dSingleDefectScore;
	DOUBLE	m_dTotalDefectScore;
	DOUBLE	m_dChipAreaScore;
	DOUBLE	m_dCurAlignPassScore;
	DOUBLE	m_dCurSingleDefectScore;
	DOUBLE	m_dCurTotalDefectScore;
	DOUBLE	m_dCurChipAreaScore;
	CString m_szCurPrStatus;
	DOUBLE	m_dSpSingleDefectScore;		// Used in UserSearchDie only
	DOUBLE	m_dSpTotalDefectScore;		// Used in UserSearchDie only
	DOUBLE	m_dSpChipAreaScore;			// Used in UserSearchDie only

	//Motorized Zoom & Focus Motors Varaibles
	BOOL m_bUseMotorizedZoom;

	BOOL m_bLockPRMouseJoystick;

	//v4.04
	CMSNmAxisInfo	m_stZoomAxis_Z;		
	CMSNmAxisInfo	m_stBackLight_Z;	//v4.24		//ES101
	BOOL IsMotionHardwareReady();		// Check whether the motion hardware are ready

	BOOL	m_bSel_Z;
	BOOL	m_bSelBL_Z;
	BOOL	m_bComm_Z;
	BOOL	m_bBackLightComm_Z;				//v4.39T8	//ES101 v1.2

	BOOL m_bFocusHome_Z;
	BOOL m_bZoomHome_Z;
	BOOL m_bBackLightHome_Z;				//v4.24

	//For Display Focus & Zoom sensor status
	BOOL m_bZoomPower;
	BOOL m_bFocusPower;
	BOOL m_bZoomHomeSensor;
	BOOL m_bFocusHomeSensor;
	BOOL m_bZoomLimitSensor;
	BOOL m_bFocusLimitSensor;
	BOOL m_bBLZPower;

	BOOL	m_bAutoFocusPower;
	BOOL	m_bAutoFocusHomeSensor;
	BOOL	m_bAutoFocusLimitSensor;
	BOOL	m_bWaferAutoFocus;
	BOOL	m_bAutoFocusOK;
	LONG	m_lAFZPosLimit;
	LONG	m_lAFZNegLimit;
	LONG	m_lAutoFocusPosition;
	LONG	m_lAutoFocusPosition2;
	LONG	m_lAF_RuntimeLevel;
	LONG	m_lEnc_AF;
	LONG	m_lUsedSrchThreadNum;
	LONG	m_lWaitingImageLimit;
	LONG	m_lAFPrDelay;
	LONG	m_lAFUpLimit;
	LONG	m_lAFLowLimit;
	LONG	m_lAFErrorTol;
	LONG	m_lAFMinDist;
	LONG	m_lAFStandbyOffset;
	DOUBLE	m_dAFrValueLimit;
	BOOL	m_bAutoFocusDown;
	BOOL	m_bAFDynamicAdjust;
	BOOL	m_bAFRescanBadFrame;
	BOOL	m_bPrescanTwicePrDone;
	BOOL	m_bScanFrameRangeSent;
	LONG	m_lAFDynamicAdjustStep;
	DOUBLE	m_dAFDynamicAdjustLimit;
	DOUBLE	m_dAFRescanBadFrameLimit;
	AF_FOCUS_SCORE	m_stAF_Data;
	LONG	m_lLastGrabWfX;
	LONG	m_lLastGrabWfY;

	LONG	m_lEnc_BLZ;
	BOOL	m_bBLZHome;

	BOOL	m_bF1SearchDieWithRoatation;
	
	//Backup Variable
	BOOL m_bSrchEnableLookForward_Backup;
	ULONG m_ulMaxNoDieSkipCount_Backup;

	//HMI use variable -- Must save in file
	LONG m_lAutoScreenFG;	
	LONG m_lAutoScreenMode;	
	BOOL m_bSrchEnableLookForward;
	BOOL m_bSrchEnableBackupAlign;
	BOOL m_bThetaCorrection;
	BOOL m_bBinTableThetaCorrection;	
	BOOL m_bConfirmSearch;	
	BOOL m_bBadCutDetection;	
	BOOL m_bRefDieCheck;	
	BOOL m_bReverseDieStatus;
	BOOL m_bPickAlignableDie;
	BOOL m_bBadcutBiDetect;
	BOOL m_bNormalRoughLedDie;
	LONG m_lBadcutBiThreshold;
	LONG m_lSrchRefDieNo;
	LONG m_lLrnTotalRefDie;
	LONG m_lNoLastStateFile;
	DOUBLE m_dMaxDieAngle;	
	DOUBLE m_dMinDieAngle;	
	DOUBLE m_dAcceptDieAngle;
	DOUBLE m_dScanAcceptAngle;
	LONG m_lLrnTotalNmlDie;
	BOOL m_bAoiSortInspction;
	LONG m_lNgPickDieAreaTolMin;
	LONG m_lNgPickDieAreaTolMax;
	LONG m_lRefDieFaceValueNo;			//v4.48A26	//Avago
	BOOL m_bEnableBHUplookPr;			//v4.57A8
	BOOL m_bEnableWTIndexTest;

	typedef struct
	{
		LONG lX;
		LONG lY;
	} ENCVAL;
	ENCVAL m_stWTIndexTestLFEnc;

	//IM HMI variables
	BOOL m_bIMAutoLearnWafer;			//v3.06
	BOOL	m_bRunPartialDie;
	LONG	m_lImageStichExtraDelay;

	BOOL m_bCompareHomeReferDieScore;
	BOOL	m_bHomeDieUniqueCheck;
	BOOL	m_bHomeDieUniqueResult;
	BOOL	m_bScanCheckPoints[9];
	UCHAR	m_ucScanCheckMapOnWafer;	//	After scan, check map with wafer on the matchness.
	BOOL	m_bScanAlignFromFile;
	BOOL	m_bScanAlignAutoFromMap;	// Hole pattern obtainned from map automatically.
	LONG	m_lScanAlignTotalHolesMin;
	LONG	m_lScanAlignPatternHolesMin;
	LONG	m_lScanAlignPatternHolesMax;
	DOUBLE	m_dSAMatchPatternsPercent;
	DOUBLE	m_dScanAlignMatchLowPercent;
	ULONG		m_ulScanAlignTotalPoints;
	ULONG		m_ulHoleSkipPointLimit;
	CDWordArray	m_awPointsListRow;
	CDWordArray	m_awPointsListCol;
	CByteArray	m_ucPointsListSta;
	ULONG		m_ulFoundHoleCounter;
	SCAN_ALIGN_WAFER_HOLES	m_astHoleList[SA_HOLES_MAX];

	DOUBLE m_dWAlignMasterPitchInMilX;	//v2.78T2
	DOUBLE m_dWAlignMasterPitchInMilY;	//v2.78T2
	DOUBLE m_dRMasterDieSizeX[3];
	DOUBLE m_dRMasterDieSizeY[3];

	LONG m_lNDieOffsetInMotorStepX;		//v3.77
	LONG m_lNDieOffsetInMotorStepY;		//v3.77

	//Position for the Zoom and Focus motors
	LONG m_lEnc_Zoom;
	LONG m_lEnc_Focus;

	//dual arm learn collet
	INT		m_nArmSelection;

	BOOL	m_bEnableMouseJoystick;

	LONG	m_lMinDieDistX;
	LONG	m_lMinDieDistY;

	LONG	m_lBHMarkRef1X;
	LONG	m_lBHMarkRef1Y;
	LONG	m_lBHMarkRef2X;
	LONG	m_lBHMarkRef2Y;

	LONG	m_lBHColletHoleRef1X;
	LONG	m_lBHColletHoleRef1Y;
	LONG	m_lBHColletHoleRef2X;
	LONG	m_lBHColletHoleRef2Y;

	//Due to UpdateDieTypeCounter() is called in a bonding sequence 2 times, check NoDieSkipCount is updated?
	ULONG	m_ulPreviousUpdateDieTypeCounterRow;
	ULONG	m_ulPreviousUpdateDieTypeCounterCol;
protected:


	CString m_szPKGNormalDieRecordPath;
	CString m_szPKGRefDieRecordPath;

	CString m_szShowImagePath[WPR_MAX_DIE];
	CString m_szGenShowImagePath;
	CString m_szShowImageType;
	LONG	m_lShowImageNo;

// prescan relative code	B

private:
	UCHAR m_ucaMatrix[MAX_MAP_DIM][MAX_MAP_DIM];
protected:
	BOOL	m_bUseAutoBondMode;

	DOUBLE	m_dPrescanPitchTolX;
	DOUBLE	m_dPrescanPitchTolY;
	LONG	m_lPrescanSrchDieScore;
	LONG	m_lOcrPassScore;
	LONG	m_lExtraPrescanOverlapX;
	LONG	m_lExtraPrescanOverlapY;
	LONG	m_lLookAroundRow;
	LONG	m_lLookAroundCol;
	UCHAR	m_ucPrescanDiePrID;
	UCHAR	m_ucScanAlignVerify;
	BOOL	m_bAoiPrescanInspction;
	BOOL	m_bSendWaferIDToPR;
	UCHAR	m_ucCornerAlignDieState;
	LONG	m_lSelect2ndPrID;
	BOOL	m_bKeepScan2ndPrDie;
	BOOL	m_bEnable2ndPrSearch;
	BOOL	m_bFovToFindGT;
	BOOL	m_bFovFindGTMove;
	BOOL	m_bFovFoundWaferAngle;

	DOUBLE	m_dScanPrCleanTime;
	DOUBLE	m_dRescanGoodRatio;
	LONG	m_lScanIndexStopCount;
	LONG	m_lPrescanImageCount;
	LONG	m_lPrescanImageLimit;
	LONG	m_lPrescanIndexRowReduce;
	LONG	m_lPrescanIndexColReduce;
	LONG	m_lScanPrAbnormalCount;

	BOOL	m_bPrescanNoTipDieMerge;
	BOOL	m_bScanRunTimeDisplay;

	BOOL	m_bSummaryOnlyNewGrade;
	
	LONG	m_lMapNgPassScore;
	LONG	m_lMapAOINgPassScore;
	LONG	m_lRescanMissingTotal;


	PR_WIN	m_stOcrViewWindow;
	LONG	m_lMapCtrCol;

#define	BAR_MAP_CTR_COL			100
#define	BAR_SCAN_MAP_OFFSET_L	10
#define	BAR_SCAN_MAP_OFFSET_R	(BAR_MAP_CTR_COL+BAR_SCAN_MAP_OFFSET_L)
#define	BAR_PHY_CTR_COL			(BAR_MAP_CTR_COL*2)
#define	BAR_PHY_END_COL			(BAR_PHY_CTR_COL*2)

#define	BAR_MAP_CTR_ROW			100
#define	BAR_SCAN_MAP_OFFSET_U	10
#define	BAR_SCAN_MAP_OFFSET_D	(BAR_MAP_CTR_ROW + BAR_SCAN_MAP_OFFSET_U)
#define	BAR_PHY_CTR_ROW			(BAR_MAP_CTR_ROW*2)
#define	BAR_PHY_END_ROW			(BAR_PHY_CTR_ROW*2)

#define	BAR_OCR_IMG_PATH		"c:\\Mapsorter\\UserData\\OcrImage"
#define	BAR_OCR_NO_IMG_PATH		"C:\\MapSorter\\Png\\CharacterQuestion.png"
#define	PAGE_BAR_MAX	5
	CString	m_szOcrBarName[PAGE_BAR_MAX];
	CString	m_szOcrValueLFBar[PAGE_BAR_MAX];
	CString	m_szOcrValueLLBar[PAGE_BAR_MAX];
	CString	m_szOcrValueRFBar[PAGE_BAR_MAX];
	CString	m_szOcrValueRLBar[PAGE_BAR_MAX];
	BOOL	m_bOcrResultBarL[PAGE_BAR_MAX];
	BOOL	m_bOcrResultBarR[PAGE_BAR_MAX];
	LONG	m_lOcrBarIndex;
	BOOL	m_bOcrBarRightSide;
	LONG	m_lOcrBarCurrRow;
	LONG	m_lOcrBarCurrMapCol;
	LONG	m_lOcrBarCurrScnCol;
	LONG	m_lOcrImageRotation;
	BOOL	m_bOcrConfirmControl;
	BOOL	m_bOcrConfirmButton;
	CString	m_szBarFirstDieImage;
	CString	m_szBarLastDieImage;
	UCHAR	m_ucOcrBarMapStage;
	bool	*m_pbGetOut;
// AOI PR TEST
	BOOL	m_bManualSrchDie;
	BOOL	ConvertMotorStepToPrPixel(LONG lX, LONG lY, DOUBLE &dPrX, DOUBLE &dPrY);
	BOOL	RConvertMotorStepToPixel(LONG lX, LONG lY, PR_COORD &stPixel);
// AOI PR TEST
	VOID	SetMapPhysicalPosition(LONG ulMapRow, LONG ulMapCol, LONG lWftX, LONG lWftY);

	BOOL	m_bPrescanDefectMarkUnPick;
	BOOL	m_bPrescanEmptyMarkUnPick;
	BOOL	m_bPrescanBadCutMarkUnPick;
	BOOL	m_bPrescanKeepPsmUnPickDie;
	UCHAR	m_ucMapEdgeSize_inDailyCheck;
	UCHAR	m_ucPrescanEdgeSizeX;
	UCHAR	m_ucPrescanEdgeSizeY;
	DOUBLE	m_dEdgeGoodScore;
	ULONG	m_ulEdgeGoodDieTotal;

	BOOL	m_bAccurateRescan;
	BOOL	m_bAutoRefillRescan;
	BOOL	m_bPrescanPrCheckReferDie;
	BOOL	m_bWprInAutoSortingMode;
	CDWordArray	m_dwaRsnBaseRow;
	CDWordArray	m_dwaRsnBaseCol;
	CDWordArray	m_dwaRsnBaseWfX;
	CDWordArray	m_dwaRsnBaseWfY;

	BOOL	m_bMoveDirection;
	LONG	m_lMotionTestDelay;
	LONG	m_lMotionTestDist;

	LONG	m_lBackLightElevatorMoveTestCount;
	BOOL	m_bBackLightElevatorMoveTest;

	CString m_szScanResult;
	
	CEvent	m_evAllPrescanDone;

	PR_SRCH_DIE_CMD m_stMultiSrchCmd;
	PR_SRCH_DIE_CMD m_stScanSrchCmd2;
	PR_INSP_CMD		m_stMultiInspCmd;
	PR_INSP_CMD		m_stScanInspCmd2;
	PR_UWORD		m_uwScanRecordID[2];

	//	Zoom Sensor and Dual Path
	ZOOM_VIEW m_stZoomView;
	LONG	m_lNmlDigitalZoom;
	LONG	m_lRunDigitalZoom;
	LONG	m_lNmlZoomFactor;
	LONG	m_lScnZoomFactor;
	LONG	m_lRunZoomFactor;
	BOOL	m_bContinuousZoom;
	DOUBLE	m_dNmlZoomSubFactor;
	DOUBLE	m_dWprZoomRoiShrink;
	PR_WIN	m_stWprRoiEffView;
	UCHAR	m_ucFovShrinkMode;
	CPrZoomSensorMode *m_pPrZoomSensorMode;

	BOOL	m_bDualPath;
	BOOL	m_bRunIsDP_ScanCam;
	BOOL	m_bContinuousZoomDP5M;
	DOUBLE	m_dConZoomSubFactorDP5M;
	DOUBLE	m_dWprZoomRoiShrinkDP5M;
	BOOL	m_bContinuousZoomHMI;
	DOUBLE	m_dConZoomSubFactorHMI;
	DOUBLE	m_dWprZoomRoiShrinkHMI;
	//	Zoom Sensor and Dual Path


	LONG	m_lGrabDigitalZoom;


	CWaferPrDataBlock m_oWaferPrDataBlk;
	
	template<class> friend class CGallerySearchController;
	CGallerySearchController<CWaferPr> *m_pPrescanPrCtrl;

	VOID P_PrescanCycleLog(const CString &szMessage);
	VOID RegPrescanVarFunc();
	VOID InitPrescanVariables();
	VOID SavePrescanDataPr(CStringMapFile  *psmf);
	VOID LoadPrescanSetupData(CStringMapFile  *psmf);
	VOID LoadPrescanDataPr(CStringMapFile  *psmf);
	BOOL IsLFSizeOK();
	BOOL IsUseLF();
	BOOL IsThisDieLearnt(UCHAR ucDieIndex);
	PR_WORD GetDiePrID(UCHAR ucDieIndex);
	BOOL AutoBondWaferScreenUpdate(PR_BOOLEAN bNormalDie);
	LONG PrAutoBondScreenUpdate(IPC_CServiceMessage &svMsg);
	LONG RefreshPrScreen(IPC_CServiceMessage &svMsg);
	BOOL IsLastReply(PR_COMMON_RPY stStatus);
	LONG GetDieCoordinate(PR_RCOORD coDieCtr);

	LONG IntoMapScanCheckPage(IPC_CServiceMessage &svMsg);
	LONG ConfirmMapScanPoints(IPC_CServiceMessage &svMsg);
	LONG SelectCheckCenterDie(IPC_CServiceMessage &svMsg);
	LONG ClearAllMapScanPoints(IPC_CServiceMessage &svMsg);
	LONG SaveScanCheckOption(IPC_CServiceMessage &svMsg);
	LONG AddScanAlignPattern4MS90(IPC_CServiceMessage &svMsg);
	LONG ScanAlignPatternFromMapTest(IPC_CServiceMessage &svMsg);
	LONG SetScanAlignVerifyMode(IPC_CServiceMessage &svMsg);
	LONG CheckHolePatternInThisMap(IPC_CServiceMessage &svMsg);
	LONG ClearPrescanData(IPC_CServiceMessage &svMsg);
	LONG SavePrescanData(IPC_CServiceMessage &svMsg);
	LONG CP100MultiSearchDie(IPC_CServiceMessage &svMsg);
	LONG MultiSearchDie(IPC_CServiceMessage &svMsg);
	LONG MultiSearchInit(IPC_CServiceMessage &svMsg);
	LONG MultiSearchDieForLearnDieProcess(LONG lDieNo);
	LONG PrescanDieResult(IPC_CServiceMessage &svMsg);
	LONG PrescanDieAction(IPC_CServiceMessage &svMsg);
	LONG LFAutoLearnDiePitch(IPC_CServiceMessage &svMsg);
	LONG MultiSearchNml3Die(IPC_CServiceMessage &svMsg);
	LONG MultiSearchInitNmlDie1(IPC_CServiceMessage &svMsg);
	LONG MultiSearchNmlDie1(IPC_CServiceMessage &svMsg);
	LONG ReconstructMapByFile(IPC_CServiceMessage &svMsg);
	LONG CheckNormalDiePattern(IPC_CServiceMessage &svMsg);
	LONG LoadScanAlignSettingFile(IPC_CServiceMessage &svMsg);
	LONG LoadSkipNgGrades(IPC_CServiceMessage &svMsg);
	BOOL AutoLoadScanAlignSettingFile(CONST CString szFileName, BOOL bDisplayMsg=FALSE);		//v4.40T9	//Nichia MS100+
	VOID FindMinMaxRowColListInMatrix(const BOOL bScnMap,
									  const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow,
									  const ULONG ulMapValidMinCol, const ULONG ulMapValidMaxCol,
									  CDWordArray &dwaRowMinList, CDWordArray &dwaRowMaxList,
									  CDWordArray &dwaColMinList, CDWordArray &dwaColMaxList);
	LONG FindHoleList(const BOOL bScnMap,
					 const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow,
					 CDWordArray &dwaColMinList, CDWordArray &dwaColMaxList,
					 CDWordArray &awHoleListRow, CDWordArray &awHoleListCol,
					 ULONG &ulHoleMinRow, ULONG &ulHoleMaxRow,
					 ULONG &ulHoleMinCol, ULONG &ulHoleMaxCol);

	VOID MarkAllHoleDieFlag(const LONG lTotalHolePoints,
						    const ULONG ulHoleMinRow, const ULONG ulHoleMinCol,
							CDWordArray &awHoleListRow, CDWordArray &awHoleListCol);

	LONG CreateHolePatterns(const ULONG ulHoleMinRow, const ULONG ulHoleMaxRow,
						    const ULONG ulHoleMinCol, const ULONG ulHoleMaxCol,
						    CDWordArray &awHoleListRow, CDWordArray &awHoleListCol,
						    SCAN_ALIGN_HOLE_LIST	astPatternList[MAX_PTNS]);

	VOID FilterHolePatterns(const BOOL bScnMap, const ULONG ulMapValidMinCol, CDWordArray &dwaRowMinList, CDWordArray &dwaRowMaxList,
							const ULONG ulFoundPatterns, SCAN_ALIGN_HOLE_LIST	astPatternList[MAX_PTNS],
							ULONG &ulMapAlnPatterns, ULONG &ulMapAlnHolesNum,
							SCAN_ALIGN_HOLE_LIST	aMapAlnPatnList[MAX_PTNS]);

	VOID LogHoleMapFile(CString szLogPath, const ULONG &ulMapAlnPatterns, SCAN_ALIGN_HOLE_LIST aMapAlnPatnList[MAX_PTNS]);
	VOID LogAlignHoleFile(const errno_t nfHoleErr, FILE *fpHole, const BOOL bScnMap,
						 const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow, const ULONG ulMapValidMinCol, const ULONG ulMapValidMaxCol, 
						 const ULONG ulHoleMinRow, const ULONG ulHoleMaxRow, const ULONG ulHoleMinCol, const ULONG ulHoleMaxCol,
						 CDWordArray &dwaRowMinList, CDWordArray &dwaRowMaxList, CDWordArray &dwaColMinList, CDWordArray &dwaColMaxList,
						 const ULONG ulMapAlnPatterns, SCAN_ALIGN_HOLE_LIST	aMapAlnPatnList[MAX_PTNS]);

	LONG AutoScanAlignWaferWithMap(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, LONG &lMapScnOffsetRow, LONG &lMapScnOffsetCol, CString &szOutMsg);
	BOOL SortScanAlignHoleOrder();
	BOOL GenerateCheckPointsTableFile();
	LONG ScanAlignPhysicalWaferWithHoles(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, LONG &lMapScnOffsetRow, LONG &lMapScnOffsetCol);
	BOOL FindSanAnCrossPatternInMap(LONG lOrgRow, LONG lOrgCol, ULONG &ulHomeRow, ULONG &ulHomeCol);
	BOOL MatchScanAnCrossPatternInWafer(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, LONG &lScanRow, LONG &lScanCol);
	LONG ToggleFailureCaselog(IPC_CServiceMessage &svMsg);
	LONG ToggleBHMarkCaselog(IPC_CServiceMessage &svMsg);
	LONG SetDebugViewerLog(IPC_CServiceMessage &svMsg);
	VOID SetScanPosition(LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin, bool bIsDefect, bool bIsBadCut, bool bIsFakeEmpty, ULONG ulImageID, USHORT uwDiePrID);
	LONG RectWaferAroundDieCheck();
	LONG WprDumpLog(IPC_CServiceMessage &svMsg);
	LONG ScanSampleKeyDice(IPC_CServiceMessage &svMsg);
	LONG ToggleScanSortZoom(IPC_CServiceMessage &svMsg);
	LONG WalkOnTwinDice(IPC_CServiceMessage &svMsg);
	BOOL CheckHolesInWaferWithMap( ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, CString &szOutMsg);
	BOOL CheckHolesInWaferWithFile(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, CString &szOutMsg);

	CDWordArray m_awMapReferListRow;
	CDWordArray m_awMapReferListCol;
	CDWordArray m_awWaferRefListRow;
	CDWordArray m_awWaferRefListCol;
	BOOL ScanAlignWaferMapWithRefer(LONG ulScnMinRow, LONG ulScnMinCol, LONG ulScnMaxRow, LONG ulScnMaxCol, LONG &lMapScnOffsetRow, LONG &lMapScnOffsetCol);


	LONG GrabAndSaveImageCmd(IPC_CServiceMessage &svMsg);
	BOOL GrabAndSaveImage(BOOL bDieType, LONG lDiePrID, LONG lCheckType);
	BOOL SaveImageScreenData(CString szNamePrefix, BOOL bHmiOnly);
	LONG InputSaveImagePath(IPC_CServiceMessage &svMsg);
	LONG GetSaveImagePath(IPC_CServiceMessage &svMsg);
	BOOL SavePrImage(CString szNamePrefix);
	VOID CheckOcrViewWindow();

	VOID SetAllPrescanDone(BOOL bState);
	BOOL WaitAllPrescanDone(INT nTimeout = LOCK_TIMEOUT);

	INT	 PrescanNormalInit(UCHAR ucDieNo, BOOL bInspect = TRUE);
	LONG GetImageNumInGallery();
	BOOL AutoGrabShareImage();
	BOOL ScanAutoMultiSearch5M(CDWordArray &dwList, BOOL bDrawDie = FALSE, BOOL bIsFindGT = FALSE);
	BOOL RescanAutoMultiSearch5M();
	BOOL ScanAutoMultiSearch2ndPR(BOOL bDrawDie = FALSE);
	BOOL WprSearchDie(CONST BOOL bNormalDie, CONST LONG lPrID, LONG &lDieOffsetX, LONG &lDieOffsetY);
	BOOL WprSearchInspectDie(CONST BOOL bNormalDie, CONST LONG lPrID,
								   LONG &lDieOffsetX, LONG &lDieOffsetY, PR_REAL &fDieRotate, PR_UWORD &usDieType);
	BOOL UpdatePrescanRefDiePos(ULONG ulRow, ULONG ulCol, LONG lReferID, LONG lEncX, LONG lEncY);
	BOOL IsSpecialReferGrade(ULONG ulRow, ULONG ulCol);
	BOOL IsNoDieGrade(ULONG ulRow, ULONG ulCol);
	VOID AF_ObtainScore_MoveToLevel();
	VOID CalcScanDiePosition(CONST LONG lGrabX, CONST LONG lGrabY, PR_RCOORD stDieOffset, INT &siStepX, INT &siStepY);
	VOID RCalculateDieCompenate(PR_RCOORD stDieOffset, int *siStepX, int *siStepY);
	VOID RConvertPixelToMotorStep(PR_RCOORD rstPixel, int *siStepX, int *siStepY, 
								 double dCalibX, double dCalibY, double dCalibXY, double dCalibYX);
	BOOL IsScanPrIdle();
	BOOL IsScanPrReadyToGrab();
	PR_REAL GetSearchStartAngle();
	PR_REAL GetSearchEndAngle();
	PR_REAL GetScanRotTol(BOOL bUseScanAngle);
	PR_REAL GetScanStartAngle(BOOL bUseScanAngle);
	PR_REAL GetScanEndAngle(BOOL bUseScanAngle);
	BOOL IsWithinScanLimit(LONG lEncX, LONG lEncY);

	BOOL NextBarIndex();
	BOOL BarSelectDieAction(UCHAR ucAction);
	VOID ConfirmScanBarMap_Finisar();
	VOID SetNextHalfMapAsMissingDie(const BOOL bRowModeSeparateHalfMap, const ULONG ulAsmHalfRow, const LONG ulAsmHalfCol);

	VOID GetMinMaxRowCol(const ULONG ulTgtRow, const ULONG ulTgtCol,
					     ULONG &ulLastScnMaxRow, ULONG &ulLastScnMinRow,
						 ULONG &ulLastScnMaxCol, ULONG &ulLastScnMinCol);

	VOID ConstructPrescanMap();
	VOID ConstructPrescanMap_Dummy();
	VOID ConstructPrescanMap_Dummy2();
	VOID ConstructPrescanMap_BarWafer();
	VOID ConstructPrescanMap_EmcoreBW();
	ULONG GetPrescanMapMaxRow(const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow);
	ULONG GetPrescanMapMaxCol(const ULONG ulMapValidMinCol, const ULONG ulMapValidMaxCol);
	VOID ConstructPrescanMap_FinisarBW();
	VOID ConstructPrescanMap_OcrBWNoMap();
	VOID GeneratePrSummaryHeader();
	VOID GeneratePrSummaryFile(CONST BOOL bBadCutAction, CONST BOOL bDefectAction, CONST BOOL bEmptyAction);
	VOID SendPrSummaryFileToEagle();
	BOOL GetDieValidInY(LONG ulInRow, LONG ulInCol, LONG lLoop, LONG &lPhyX, LONG &lPhyY, BOOL bByMap = TRUE);
	VOID FillUpBarWaferMap(LONG lStartRow, LONG lEndRow, LONG lStartCol, LONG lEndCol, BOOL bAlnToDown, BOOL bCtrToLeft, LONG lTolX, LONG lTolY);
	BOOL FindAndFillFromRemainList(LONG ulRow, LONG ulCol, LONG lDieTolX, LONG lDieTolY, LONG &lEncX, LONG &lEncY, PR_UWORD &uwDiePrID);
	VOID AddPrescanEdge();
	VOID CheckPrescanEdgeDie();
	BOOL CheckRepeatDieInMatrix3X3(const ULONG ulRow, const ULONG ulCol, const LONG lNewWftX, const LONG lNewWftY, const ULONG ulDieSameTolX, const ULONG ulDieSameTolY);
	BOOL RescanConstructMap();
	BOOL IsUseGradeConvert();
	VOID SaveBadCutEvent(CString szLogMsg, BOOL bBackUp = FALSE);

	BOOL IsIgnoreDie(LONG lAsmRow, LONG lAsmCol);
	BOOL RefillScanInfoViaRemainDice(LONG lAsmRow, LONG lAsmCol, ULONG ulLoop);
	BOOL WprFivePointCheckDiePosition(VOID);
	BOOL ScanCheckFivePoints(BOOL bFirstTime);
	VOID FivePointCheckLog(CString szLogMsg);
	BOOL WprScnCheckDiePrResult(LONG lScnCheckRow, LONG lScnCheckCol, BOOL &bPromptMessage, CString &szText);
	LONG m_lMapScanOffsetRow;
	LONG m_lMapScanOffsetCol;
	DOUBLE	m_dScanAlignMPassScore;
	BOOL WprGetDieValidPrescanPosn(LONG ulIntRow, LONG ulIntCol, ULONG ulLoop, LONG &lPhyX, LONG &lPhyY);
	BOOL GetScanPosn(LONG ulRow, LONG ulCol, LONG &lPhyX, LONG &lPhyY);
	BOOL IsScnExist(LONG ulRow, LONG ulCol);
	BOOL IsMapExist(LONG ulRow, LONG ulCol);
	BOOL IsNoPickBadCut();
	BOOL IsNoPickDefect();
	BOOL IsNoPickExtra();
	BOOL UpdateDie(const ULONG ulRow, const ULONG ulCol, const UCHAR ucGrade, const ULONG ulDieState);

	VOID SetupMultiSearchDieCmd(LONG lInputDieNo, BOOL bFastHomeMerge, BOOL bInspect, BOOL bUseScanAngle);
	VOID SetupScanSearchCmd2(UCHAR lDieNoIndex, BOOL bInspect);
	BOOL SetMapPhyPosn(ULONG ulRow, ULONG ulCol, LONG lPhyX, LONG lPhyY);

	BOOL IsPrescanEndToPickNg();
	BOOL IsPrescanReferDie();
	BOOL IsPrecanWith2Pr();
	BOOL IsStitchMode();
	BOOL IM_DummySearchDie(ULONG ulRow, ULONG ulCol, LONG &lOffsetX, LONG &lOffsetY);
	PR_UWORD	m_uwIMMergeNewRecordID;
	PR_UWORD	m_uwIMMergeRecordID[WPR_MAX_DIE];

	BOOL SearchFirstDieInWafer();
	LONG	m_nPrescanIndexCounter;

	PR_DIE_ALIGN_ALG	GetLrnAlignAlgo(LONG lDieNo);
	PR_BACKGROUND_INFO	GetLrnBackGround(LONG lDieNo);
	PR_DIE_INSP_ALG		GetLrnInspMethod(LONG lDieNo);
	CString	GetLrnAlignAlgoInString(LONG lDieNo);		//v4.49A10
	CString	GetLrnInspMethodInString(LONG lDieNo);		//v4.49A10


// prescan relative code	E
// Contour edge detection
	CString		m_szContourImagePath;
	LONG		m_lWL1CoaxLightLevel;
	LONG		m_lWL1RingLightLevel;
	LONG		m_lWL1SideLightLevel;
	LONG		m_lWL1BackLightLevel;
	LONG		m_lWL2CoaxLightLevel;
	LONG		m_lWL2RingLightLevel;
	LONG		m_lWL2SideLightLevel;
	LONG		m_lWL2BackLightLevel;
	PR_COORD	m_stCalibCornerPos[WPR_MAX_DIE_CORNER];	// Learnt normal die corner 
	LONG		m_lContourULX;
	LONG		m_lContourULY;
	LONG		m_lContourLRX;
	LONG		m_lContourLRY;

	LONG SwitchContourLighting(IPC_CServiceMessage &svMsg);
	// Mouse
	LONG WLUsePRMouse(IPC_CServiceMessage &svMsg);
	LONG WLDrawComplete(IPC_CServiceMessage &svMsg);
	LONG WLCancelMouseDraw(IPC_CServiceMessage &svMsg);
	LONG WLMovePRCursor(IPC_CServiceMessage &svMsg);
	LONG WLCalibrateToCorner(IPC_CServiceMessage &svMsg);
	LONG WLConfirmCalibrate(IPC_CServiceMessage &svMsg);
	LONG WLTeachCameraOffset(IPC_CServiceMessage &svMsg);
	BOOL SetWTJoystickLimit(BOOL bMax, CONST BOOL bWT2);
	LONG SetContourImagePath(IPC_CServiceMessage &svMsg);
	LONG GetContourImagePath(IPC_CServiceMessage &svMsg);

	BOOL SetContourLighting(CONST BOOL bOn, CONST BOOL bWT2);

	LONG SetOcrDieViewWindow(IPC_CServiceMessage &svMsg);
	LONG GetOcrDieViewWindow(IPC_CServiceMessage &svMsg);
	LONG LearnBarPitch(IPC_CServiceMessage &svMsg);
	LONG SkipBarResult(IPC_CServiceMessage &svMsg);
	LONG BarScanGoDie(IPC_CServiceMessage &svMsg);
	LONG WprLearnBarPitch(CONST BOOL bInX);

	LONG PRIDDetailInfo(IPC_CServiceMessage &svMsg);

	LONG ChangeMapOcrValue(IPC_CServiceMessage &svMsg);
	LONG ConfirmBarResult(IPC_CServiceMessage &svMsg);
	LONG ConfirmAllDone(IPC_CServiceMessage &svMsg);
// Contour edge detection

	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString &szMsg);
};
