/////////////////////////////////////////////////////////////////
// BondPr.cpp : interface of the CBondPr class
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
#include <prheader.h>
#include <prmsg.h>
#include <prstatus.h>
#include <prtype.h>
#include "MS896A.h"
#include "math.h"
#include "SPC_CDatabase.h"
#include "SPC_CDataManagement.h"
#include "SPC_CProduct.h"
#include "SPC_CPart.h"
#include "SPC_CDoubleArray.h"
#include "SPC_CProperty.h"
#include "PrZoomSensor.h"

enum eBPR_EVENTID {BPR_PLACEMENT_CHECK =1 , BPR_PLACEMENT_MAX_SHIFT_X, BPR_PLACEMENT_MAX_SHIFT_Y, BPR_PLACEMENT_MAX_ALLOWED,
BPR_DIE_PASS_SCORE, BPR_DIE_ROTATION_AVG_ANGLE, BPR_DIE_ROTATION_AVG_ANGLE_ALLOW, BPR_DIE_ROTATION_CHECK_MAX_ANGLE,
BPR_DIE_ROTATION_MAX_ANGLE_DIE_ALLOW, BPR_DIE_EMPTY_DIE_CHECK, BPR_DIE_EMPTY_DIE_CHECK_MAX_ALLOWED, BPR_DIE_ROTATION_MAX_ANGLE,
BPR_SEARCH_AREA_X, BPR_SEARCH_AREA_Y};

#define	MSD_MAX_DIM		5
typedef struct {
	BOOL	bDieState[MSD_MAX_DIM];
} BPR_MULTI_SRCH_RESULT;    //4.51D20

typedef struct  
{
	int		m_nDieEncX;
	int		m_nDieEncY;
	BOOL    m_bResult;
	INT		m_nDieStateRight;
	INT		m_nDieStateLeft; 
	INT		m_nDieStateUp;
	INT		m_nDieStateDown;
	DOUBLE	m_dDieRowFovSize;  //4.53D01
	DOUBLE	m_dDieColFovSize;  

} BPR_MULTI_SRCH_DIE_POSITION;//4.51D20

//static ULONG	m_ulMultiDieCounter;
//static CDWordArray	m_gstMultiDieMap[m_ulMultiDieCounter];


#define		BPR_TOOLING_T					"ToolingTAxis"
#define		BPR_SP_TOOLING_T_OPTICS			"spfRotaryToolingHome"
#define		BPR_SP_TOOLING_T_OPTICS_NEG		"spfRotaryToolingHomeNeg"
#define		BPR_SP_TOOLING_T_DETECTOR		"spfRotaryToolingDetector"
#define		BPR_SP_TOOLING_T_DETECTOR_NEG	"spfRotaryToolingDetectorNeg"
#define		BPR_SP_TOOLING_T_ISP			"spfRotaryToolingISP"
#define		BPR_SP_TOOLING_T_ISP_NEG		"spfRotaryToolingISPNeg"

#define		BPR_POSTBOND_RUNTIME		1
#define		BPR_POSTBOND_OFFLINE		2

#define		BPR_MAX_BINBLOCK			175		//150

#define		BPR_MAX_DIE					20		//10
#define		BPR_NORMAL_DIE				0
#define		BPR_REFERENCE_DIE			1
#define		BPR_GEN_NDIE_OFFSET			-1
#define		BPR_GEN_RDIE_OFFSET			2

#define		BPR_GEN_NG_GRADE_INDEX_WITH_OFFSET	11

//for Normal PR option
#define		BPR_NORMAL_PR_DIE_INDEX1		1	//=BPR_NORMAL_PR_ARM1_INDEX (only for setup usage)
#define		BPR_NORMAL_PR_ARM2_INDEX2		2	//=BPR_NORMAL_PR_ARM2_INDEX (only for setup usage)
#define		BPR_NORMAL_PR_FF_MODE_INDEX3	3	//only for FF mode	

//for Reference PR option
#define		BPR_REFERENCE_PR_DIE_INDEX1		1  //Prebond
#define		BPR_REFERENCE_PR_DIE_INDEX2		2  //no use
#define		BPR_REFERENCE_PR_DIE_INDEX3		3  //no use
#define		BPR_REFERENCE_PR_DIE_INDEX4		4  //Uplook C1
#define		BPR_REFERENCE_PR_DIE_INDEX5		5  //Uplook C2
#define		BPR_REFERENCE_PR_DIE_INDEX6		6  //Prestart Epoxy Search Collet Hole
#define		BPR_REFERENCE_PR_DIE_INDEX7		7  //no use
#define		BPR_REFERENCE_PR_DIE_INDEX8		8  //BPR_REF_SRCH_ERR_MAP
#define		BPR_REFERENCE_PR_DIE_INDEX9		9  //NG grade PR

#define		BPR_REF_SRCH_ERR_MAP			BPR_REFERENCE_PR_DIE_INDEX8

#define		BPR_SHOW_IMAGE_PATH			"C:\\MapSorter\\UserData\\ShowPrImage"
#define		BPR_PR_RECORD_PATH			"C:\\WinEagle\\record\\tmplDir."

#define		BPR_RT_XYCOMP_ARRAY_SIZE	20

#define		BPR_MAX_DETECT_WIN				4

//Use to read back waferPr data in wafepr string map file

//Bond Pr data
#define		BPR_HMI_NORM_DIE			"Normal"
#define		BPR_HMI_REF_DIE				"Reference"
#define		BPR_LRN_NDIE_DATA			"BondPr Learn Die Data"
#define		BPR_LRN_NDIE_ALNALGO		"Die Align Algorithm"
#define		BPR_LRN_NDIE_BKGROUND		"Bond Die Align Back Ground"
#define		BPR_LRN_NDIE_DIETYPE		"Die Type"
#define		BPR_LRN_NDIE_FINESCH		"Fine Search Res"
#define		BPR_LRN_NDIE_INSPRES		"Inspection Res"
#define		BPR_LRN_NDIE_INSPMETHOD		"Inspection Method"
#define		BPR_LRN_NDIE_DIESIZEX		"Die SizeX"
#define		BPR_LRN_NDIE_DIESIZEY		"Die SizeY"
#define		BPR_LRN_NDIE_FOV			"Bond Fov"
#define		BPR_LRN_NDIE_ROW_FOV		"Bond Row Fov" //4.53D01
#define		BPR_LRN_NDIE_COL_FOV		"Bond Col Fov"
#define		BPR_LRN_NDIE_ZOOM_MODE		"BondPr Zoom Mode"
#define		BPR_LRN_NDIE_CALIBRATION	"BondPr Calibration"

#define		BPR_SRH_NDIE_DATA			"BondPr Search Die Data"
#define		BPR_SRH_NDIE_ALGNRES		"Die Align Res"
#define		BPR_SRH_NDIE_DEFECTTHRE		"Defect Thres"
#define		BPR_SRH_NDIE_SRCHULCX		"Search die ULC X"
#define		BPR_SRH_NDIE_SRCHULCY		"Search die ULC Y"
#define		BPR_SRH_NDIE_SRCHLRCX		"Search die LRC X"
#define		BPR_SRH_NDIE_SRCHLRCY		"Search die LRC Y"
#define		BPR_SRH_NDIE_SCRHAREA_X		"Seach die area X"
#define		BPR_SRH_NDIE_SCRHAREA_Y		"Seach die area Y"
#define		BPR_SRH_NDIE_PASSSCORE		"Pass score"
#define		BPR_SRH_NDIE_GREYLEVEL		"Grey Level defect"
#define		BPR_SRH_NDIE_SIGNLEDEFECT	"Min Single Defect area"
#define		BPR_SRH_NDIE_TOTALDEFECT	"Total Defect area"
#define		BPR_SRH_NDIE_MINCHIPAREA	"Min Chip area"
#define		BPR_SRN_NDIE_DEFECT_CHECK	"Defect Die Check"
#define		BPR_SRN_NDIE_CHIP_CHECK		"Chip Die Check"

#define		BPR_SRH_NDIE_TOTAL_DEFECTS				"Enable Total Defects"
#define		BPR_SRH_NDIE_LINE_DEFECT				"Enable Line Defect"
#define		BPR_SRH_NDIE_MIN_LINE_DEFECT_LENGTH		"Min Line Defect Length"
#define		BPR_SRH_NDIE_MIN_NUM_OF_LINE_DEFECT		"Min Num Of Line Defect"
#define		BPR_SRH_NDIE_MAX_NUM_OF_DEFECTS			"Max Num Of Defects"
#define		BPR_PREBOND_USE_EPOXYSEARCH				"PreBond Use Epoxy Search"	//v4.58A4
#define		BPR_FRAMEALIGN_HW_MARK					"Frame HW Cross Mark Align"	//v4.59A1

#define		BPR_GEN_NDIE_DATA			"BondPr General Die Data"
#define		BPR_GEN_NDIE_PRID			"PR Search ID"
#define		BPR_CURSOR_CENTER_X			"PR Cursor Center X"
#define		BPR_CURSOR_CENTER_Y			"PR Cursor Center Y"
#define		BPR_CURSOR_CENTER_X_PIXEL	"PR Center Offset X Pixel"
#define		BPR_CURSOR_CENTER_Y_PIXEL	"PR Center Offset Y Pixel"
#define		BPR_GEN_NDIE_LEARNDIE		"Die Learn"
#define		BPR_GEN_NDIE_LEARNCALIB		"Die Calib"
#define		BPR_GEN_NDIE_CALIBX			"Die CalibX"
#define		BPR_GEN_NDIE_CALIBY			"Die CalibY"
#define		BPR_GEN_NDIE_CALIBXY		"Die CalibXY"
#define		BPR_GEN_NDIE_CALIBYX		"Die CalibYX"

#define		MOVE_EJR_Z_AND_POSTBOND		"Move Ejr Z And PostBond"

#define		BPR_GEN_NDIE_FFMODE_CALIBX	"Die FFMode CalibX"
#define		BPR_GEN_NDIE_FFMODE_CALIBY	"Die FFMode CalibY"
#define		BPR_GEN_NDIE_FFMODE_CALIBXY	"Die FFMode CalibXY"
#define		BPR_GEN_NDIE_FFMODE_CALIBYX	"Die FFMode CalibYX"

#define		BPR_GEN_NDIE_LEARNCALIB2	"Die Calib2"
#define		BPR_GEN_NDIE_CALIBX2		"Die CalibX2"
#define		BPR_GEN_NDIE_CALIBY2		"Die CalibY2"
#define		BPR_GEN_NDIE_CALIBXY2		"Die CalibXY2"
#define		BPR_GEN_NDIE_CALIBYX2		"Die CalibYX2"
#define		BPR_GEN_NDIE_CALIB_1STSTEPSIZE		"Die Calib 1st Step Size"

#define		BPR_GEN_NDIE_PITCHXX		"Die PitchXX"
#define		BPR_GEN_NDIE_PITCHXY		"Die PitchXY"
#define		BPR_GEN_NDIE_PITCHYX		"Die PitchYX"
#define		BPR_GEN_NDIE_PITCHYY		"Die PitchYY"
#define		BPR_GEN_NDIE_SHAPE			"Die Shape"
#define		BPR_BOND_POS_OFFSET_X_PIXEL1	"Bond Pos Offset X Pixel1"
#define		BPR_BOND_POS_OFFSET_Y_PIXEL1	"Bond Pos Offset Y Pixel1"
#define		BPR_BOND_POS_OFFSET_X_PIXEL2	"Bond Pos Offset X Pixel2"
#define		BPR_BOND_POS_OFFSET_Y_PIXEL2	"Bond Pos Offset Y Pixel2"


#define		BPR_POSTBOND_DATA			"BondPr PostBond Data"
#define		BPR_PB_USE_POSTBOND			"Enable PostBond"
#define		BPR_PB_AVERAGE_CHECK		"Average Angle Check"
#define		BPR_PB_MAX_ANGLE_CHECK		"Max Angel Check"
#define		BPR_PB_PLACEMENT_CHECK		"Placement Check"
#define		BPR_PB_RELOFFSET_CHECK		"Rel Offset Check"					//v4.26T1	//Semitek
#define		BPR_PB_INSPECTION_TYPE		"Defect Inspection Type"
#define		BPR_PB_EMPTY_CHECK			"Empty Check"
#define		BPR_PB_MAX_ANGLE_ALLOW		"Max Angle Allow Count"
#define		BPR_PB_MAX_ACC_ANGLE_ALLOW	"Max Acc Angle Allow Count"
#define		BPR_PB_MAX_ANGLE			"Max Angle Degree"
#define		BPR_PB_MAX_AVERAGE			"Average Angle Degree"
#define		BPR_PB_MAX_SHIFT_ALLOW		"Max Shift Allow Count"
#define		BPR_PB_MAX_REL_SHIFT_ALLOW	"Max REL Shift Allow Count"
#define		BPR_PB_MAX_X_SHIFT			"Max X Shift"
#define		BPR_PB_MAX_Y_SHIFT			"Max Y Shift"
#define		BPR_PB_MAX_DEFECT_ALLOW		"Max Defect Allow Count"
#define		BPR_PB_MAX_EMPTY_ALLOW		"Max Empty Allow Count"
#define		BPR_PB_MAX_ACC_EMPTY_ALLOW	"Max acc Empty Allow Count"			//v4.40T9	//Sanan
#define		BPR_PB_RT_XYCOMP			"RT XY Compensation"				//v3.86		//MS100
#define		BPR_PB_MAX_CP_X_ALLOW		"Max CP X Count"	
#define		BPR_PB_MAX_CP_Y_ALLOW		"Max CP Y Count"	
#define		BPR_PB_MAX_CP_T_ALLOW		"Max CP T Count"	

#define		BPR_LIGHTING_ONOFF			"Diable Turn On Off Lighting"
#define		BPR_TOOLING_STAGE_TO_USE	"Tooling Stage To Use"
#define		BPR_ISP_DOWN_TIMEOUT		"Tooling Stage ISP Down Timeout"
#define		BPR_ISP_DOWN_SETTLE_DELAY	"Tooling Stage ISP Down Settle Delay"

#define		BPR_LIGHT_SETTING			"Light Setting"
#define		BPR_LRN_COAX_LIGHT_LEVEL	"Coax Light Level"
#define		BPR_LRN_RING_LIGHT_LEVEL	"Ring Light Level"
#define		BPR_LRN_SIDE_LIGHT_LEVEL	"Side Light Level"
#define		BPR_LRN_EXPOSURE_TIME_LEVEL		"Exposure Time Level"			//ching
#define		BPR_CLEAN_PROBE_LIGHT_LEVEL		"Clean Probe Light Level"		//CP100 clean pin level
#define		BPR_LRN_G_EXPOSURE_TIME_LEVEL	"General Exposure Time Level"	//v3.30T1

#define		BPR_DATA					"Bond PR Data"
#define		PB_ALARM					"PB Alarm"
#define		BPR_CAMERA_DATA					"Camera Data"
#define		BPR_ZOOM_SENSOR					"Zoom Sensor"
#define		BPR_NORMAL_ZOOM_FACTOR			"Bond PR Noraml Zoom Factor"
#define		BPR_CONTINUOUS_ZOOM_MODE		"BPR Zoom Continuous Mode"
#define		BPR_CONTINUE_ZOOM_FACTOR		"BPR Continuous Zoom Factor"

#define		BPR_SI_OPTICS_POS			"ibOpticsPosition"
#define		BPR_SI_ISP_POS				"ibIspPosition"
#define		BPR_SI_PD_POS				"ibPdPosition"
#define		BPR_SI_ISP_UP				"ibIspUp"
#define		BPR_SI_ISP_DOWN				"ibIspDown"

#define		BPR_SO_ISP_DOWN				"oIspUpDown"

#define		BPR_ENABLE_PR_EMPTY_ALARM	"Enble PB Empty alarm"
#define		BPR_EMPTY_CONT_LIMIT		"Empty cont. limit"
#define		BPR_EMPTY_ACC_LIMIT			"Empty Acc. limit"
#define		BPR_ENABLE_PLACEMENT_CHECK	"Enble placement Check"
#define		BPR_DIE_SHIFT_X				"Die Shift X"
#define		BPR_DIE_SHIFT_Y				"Die Shift Y"
#define		BPR_MAX_SHIFT_ALLOW			"Max Shift Allow"
#define		BPR_ENABLE_BT_ANGLE_CHECK	"Enable BT Angle Check"
#define		BPR_MAX_ANGLE				"Max Angle"
#define		BPR_MAX_ANGLE_ALLOW			"Max Angle Allow"
#define		BPR_ACC_ANGLE_ALLOW			"Max Acc Angel Allow"

#define		BPR_RECTANGLE_DIE				0
#define		BPR_HEXAGON_DIE					1
#define		BPR_TRIANGULAR_DIE				2
#define		BPR_RHOMBUS_DIE					3

#define		BPR_TRIANGLE_CORNERS			3
#define		BPR_RHOMBUS_CORNERS				4
#define		BPR_HEXAGON_CORNERS				6

#define		BPR_MAX_DIE_CORNER				6
#define		BPR_DIE_CORNER_0				0
#define		BPR_DIE_CORNER_1				1
#define		BPR_DIE_CORNER_2				2
#define		BPR_DIE_CORNER_3				3
#define		BPR_DIE_CORNER_4				4
#define		BPR_DIE_CORNER_5				5

#define		BPR_DETECT_DIE_CORNER		500
#define		BPR_MIN_DIE_CORNER			100

#define		BPR_CT_DIE					0	//CENTER
#define		BPR_LT_DIE					1	//LHS
#define		BPR_RT_DIE					5	//RHS	
#define		BPR_UP_DIE					3	//UP
#define		BPR_DN_DIE					7	//DOWN
#define		BPR_UL_DIE					2	//UP+LHS
#define		BPR_DL_DIE					8	//DOWN+LHS	
#define		BPR_UR_DIE					4	//UP+RHS
#define		BPR_DR_DIE					6	//DOWN+RHS

#define		BPR_MAX_LIGHT_GROUP			10  //2

//ERROR/warning
#define		BPR_ERR_CALIB_NO_1ST_DIE	-1
#define		BPR_ERR_CALIB_NO_LT_DIE		-2
#define		BPR_ERR_CALIB_NO_RT_DIE		-3
#define		BPR_ERR_CALIB_NO_UP_DIE		-4
#define		BPR_ERR_CALIB_NO_DN_DIE		-5
#define		BPR_ERR_CALIB_ZERO_VALUE	-6

#define		BPR_ERR_SPC_XY_LIMIT		10
#define		BPR_ERR_SPC_MAX_ANGLE		11
#define		BPR_ERR_SPC_AVG_ANGLE		12
#define		BPR_ERR_SPC_DEFECT			13
#define		BPR_ERR_SPC_EMPTY			14
#define		BPR_ERR_SPC_REL_XY_LIMIT	16			//v4.26T1	//Semitek	//Relative XY limit
#define		BPR_ERR_SPC_MAX_ACC_ANGLE	23			//Semitek

//v4.57A14
#define		BPR_ERR_SPC_XY_LIMIT2		17
#define		BPR_ERR_SPC_MAX_ANGLE2		18
#define		BPR_ERR_SPC_AVG_ANGLE2		19
#define		BPR_ERR_SPC_DEFECT2			20
#define		BPR_ERR_SPC_EMPTY2			21
#define		BPR_ERR_SPC_REL_XY_LIMIT2	22			//Semitek
#define		BPR_ERR_SPC_MAX_ACC_ANGLE2	24			//Semitek

#define		BPR_SEARCHAREA_SHIFT_FACTOR	0.5

//BPR Output Bits	//andrewng //2020-0619
#define		BPR_SO_SEARCH_DIE_CMD			"obBprSearchDieCmd"
#define		BPR_SO_SEARCH_DIE_REPLY_1		"obBprSearchDieRpy1"
#define		BPR_SO_SEARCH_DIE_REPLY_2		"obBprSearchDieRpy2"
#define		BPR_SO_SEARCH_DIE_REPLY_3		"obBprSearchDieRpy3"


class CPrZoomSensorMode;
class CBondPr : public CMS896AStn
{
	DECLARE_DYNCREATE(CBondPr)

protected:
	// Operation state in Auto State
	enum {	WAIT_BHT_READY_Q,
			WAIT_BT_STABLE_Q,
			WAIT_WPR_GRAB_IMAGE_READY_Q,
			GRAB_IMAGE_Q,
			WAIT_GRAB_DONE_Q,
			INSPECTION_Q,
			SPC_FAILED_Q,
			HOUSE_KEEPING_Q,
			INSPECT_LAST_DIE_Q};
public:
	
	CBondPr();
	virtual	~CBondPr();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();
	VOID Operation();
	virtual BOOL InitData();			// Init the data from SMF and NVRAM
	virtual VOID ClearData();			// Clear data content for cold-start
	virtual VOID FlushMessage();		// Flush IPC Message	
	virtual VOID UpdateStationData();

	PR_UBYTE GetPostBondPRSenderID();
	PR_UBYTE GetPostBondPRReceiverID();

	VOID CalculateNDieOffsetXY(INT &siStepX, INT &siStepY);		//v3.77 // 4.51D20
	BOOL IsAGCCenterInPixelValidate();

	VOID SetSensorZoomFFMode(BOOL bSensorZoomFFMode);
	BOOL IsSensorZoomFFMode();
	LONG GetBondPRDieNo();
	LONG GetBondPRDieNo(LONG lDieNo);
	BOOL IsEnableZoom();
	VOID InitZoomView();
	BOOL BPR_LiveViewZoom();
	BOOL BPR_LiveViewZoom(LONG lZoomFactor);
	VOID BPR_ZoomScreen(short hZoomFactor);
	LONG BPR_GetRunZoom();
	LONG BPR_GetNmlZoom();
	VOID BPR_SetNmlZoomFactor(LONG lZoomFacotr);
	LONG BPR_GetZoomDigital();
	BOOL BPR_ZoomSensor(LONG lZoomMode, PR_WORD ssType = CPrZoomSensorMode::PR_ZOOM_SNR_ZOOM_MODE_BOTH);
	BOOL IsBinFrameRotated180();
	PR_UWORD AutoManualLearnDie(BOOL bDieType, LONG lInputDieNo);
	VOID DecreaseDieEmptyCount(BOOL bIsArm1);
	LONG AutoLearnBondFFModeDie();

	LONG GetBondPosOffsetXPixel();
	LONG GetBondPosOffsetYPixel();

	VOID SetUseBinMapBondArea();
	BOOL IsNGBlockNow();
	BOOL IsNGBlock(UCHAR ucGrade);

	BOOL LoadPackageMsdBPRData(VOID);
	BOOL SavePackageMsdBPRData(VOID);
	BOOL UpdateBPRPackageList(VOID);

	BOOL SearchBPRDie(BOOL bMoveToCenter, BOOL bTakeLog);						//For BT Use
	BOOL BprSearchDie(LONG &lStepX, LONG &lStepY, CString &szLog);				//For BT Use	//andrewng //2020-0803
	BOOL BprSearchLFDie(BOOL bTakeLog, UCHAR ucLFDir, BOOL bGrab, CString szDieLog, LONG &lStepX, LONG &lStepY);	//For BT Use	//andrewng //2020-0803
	
	BOOL BprSerachWaferEjectorPin(LONG lEjTEnc, LONG lEjTCmd);
	BOOL BprSerachDieOnCenterOfRotation(LONG lBTThetaEnc, DOUBLE dRotateDegree);

	VOID CheckFFModeCalib();
	DOUBLE	GetCalibX();
	DOUBLE	GetCalibY();
	LONG BT_SearchDie(IPC_CServiceMessage& svMsg); 
	LONG BT_SearchDieXYInFOV(IPC_CServiceMessage& svMsg);		//v3.33
	LONG DrawSearchWindow(IPC_CServiceMessage& svMsg);			//v2.56
	LONG BT_SearchDieInFOV(IPC_CServiceMessage& svMsg);
	LONG BT_MultiSearchFirstDie1(IPC_CServiceMessage& svMsg);
	LONG BT_SearchFirstDie(IPC_CServiceMessage& svMsg);
	VOID BackupBurnInStatus();
	VOID RestoreBurnInStatus();
	VOID SaveAdjBondPosOffsetPixel(const BOOL bBHZ2, const LONG lBHZAdjBondPosOffsetX_count, const LONG lBHZAdjBondPosOffsetY_count);
	VOID ConvertPixelToUM(const LONG lPixelX, const LONG lPixelY, double &dXum, double &dYum);
	BOOL OpAutoSearchBPRColletHoleWithEpoxyPattern(BOOL bBH2);

	VOID ResetRtBHCompXYArrays(const BOOL bBHZ1);				//v4.35T1

	BOOL PreAutoSearchDie_HWTri(BOOL bAutoBond = FALSE);		//andrewng0788

public:

	CString m_szBprZoomFactor;

protected:
	
	PR_UWORD	m_uwFrameSubImageNum; // 4.51D20
	PR_SRCH_DIE_CMD m_stMultiSrchCmd;
	BOOL PrescanAutoMultiGrabDone(UCHAR ucCorner, CDWordArray &dwList, BOOL bDrawDie = FALSE, BOOL bIsFindGT = FALSE);
	BOOL ScanAutoMultiSearch5M(UCHAR ucCorner, CDWordArray &dwList, BOOL bDrawDie = FALSE, BOOL bIsFindGT = FALSE);
	LONG GetDieCoordinate(PR_RCOORD coDieCtr);
	VOID CalcScanDiePosition(CONST LONG lGrabX, CONST LONG lGrabY, PR_RCOORD stDieOffset, INT &siStepX, INT &siStepY);
	LONG GetPrCenterX();
	LONG GetPrCenterY();
	DOUBLE	GetCalibXY();
	DOUBLE	GetCalibYX();
	DOUBLE	GetScanCalibX();
	DOUBLE	GetScanCalibY();
	DOUBLE	GetScanCalibXY();
	DOUBLE	GetScanCalibYX();
	void GetSubWindow(LONG lIndex, PR_WORD &lULX, PR_WORD &lULY, PR_WORD &lLRX, PR_WORD &lLRY);
	VOID RCalculateDieCompenate(PR_RCOORD stDieOffset, int *siStepX, int *siStepY);
	VOID RConvertPixelToMotorStep(PR_RCOORD rstPixel, int *siStepX, int *siStepY, 
								 double dCalibX, double dCalibY, double dCalibXY, double dCalibYX);
	BOOL IsThisDieLearnt(UCHAR ucDieIndex);

	INT	 OpPrescanInit(UCHAR ucDieNo, BOOL bFastHomeMerge = TRUE);
	BOOL SetupMultiSearchDieCmd(LONG lInputDieNo, BOOL bFastHomeMerge, BOOL bInspect, BOOL bUseScanAngle);
	PR_WORD GetDiePrID(UCHAR ucDieIndex);
	PR_DIE_INSP_ALG		GetLrnInspMethod(LONG lDieNo);
	//BOOL DieIsAlignable(PR_UWORD usDieType);
	//BOOL DieIsGood(PR_UWORD usDieType);
	//BOOL DieIsDefective(PR_UWORD usDieType);
	//BOOL DieIsBadCut(PR_UWORD usDieType);
	//BOOL DieIsInk(PR_UWORD usDieType);
	//BOOL DieIsChip(PR_UWORD usDieType);
	//BOOL DieIsOutAngle(DOUBLE dDieAngle);
	//BOOL ScanDieIsDefect(PR_UWORD usAlignType, PR_UWORD usInspType);
	PR_WORD GetScanNmlSizePixelX();
	PR_WORD GetScanNmlSizePixelY();
	PR_WORD GetScanDieSizePixelX(UCHAR ucDieNo);
	PR_WORD GetScanDieSizePixelY(UCHAR ucDieNo);
	LONG GetPrScaleFactor();
	PR_REAL GetScanStartAngle(BOOL bUseScanAngle);
	PR_REAL GetScanEndAngle(BOOL bUseScanAngle);
	PR_REAL GetScanRotTol(BOOL bUseScanAngle);

	PR_WORD	GetScanPRWinULX();
	PR_WORD	GetScanPRWinULY();
	PR_WORD	GetScanPRWinLRX();
	PR_WORD	GetScanPRWinLRY();
	PR_WIN  GetScanPRWholeWindow();
	PR_WORD	GetPRWinULX();
	PR_WORD	GetPRWinULY();
	PR_WORD	GetPRWinLRX();
	PR_WORD	GetPRWinLRY();
	VOID CalculateDieInView(CONST BOOL bPrMsd = FALSE);
	BOOL RectBondSingleCornerCheck(INT &nDieStateRight, INT &nDieStateLeft, INT &nDieStateUp, INT &nDieStateDown, UCHAR ucCorner);
	LONG RectBondCornerCheck(INT &nDieStateRight, INT &nDieStateLeft, INT &nDieStateUp, INT &nDieStateDown, UCHAR ucCorner);
	BOOL IsLFSizeOK();
	VOID MultiSrchInitNmlDie1(BOOL bInspect=TRUE); 
	void SetMultiDiePosition(USHORT usIndex,LONG nDieX, LONG nDieY, DOUBLE dDieAngle, CString szDieBin, LONG nRow, LONG nCol);
	VOID CompareDiesPositionToFindFirstDies(const UCHAR ucCorner, const USHORT usDieSum);
	VOID FindFirstDieInUpperLeftCorner(const ULONG usDieSum);
	VOID FindFirstDieInUpperRightCorner(const ULONG usDieSum);
	VOID FindFirstDieInLowerLeftCorner(const ULONG usDieSum);
	VOID FindFirstDieInLowerRightCorner(const ULONG usDieSum);
	BOOL SearchFrameEdgeAngle(DOUBLE& dAngle);	//v4.59A1

	VOID GetAxisInformation();

	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	virtual VOID RegisterVariables();

	// State Operation (BPR_State.cpp)
	VOID RunOperation();

	virtual	VOID IdleOperation();
	virtual	VOID InitOperation();
	virtual	VOID PreStartOperation();
	virtual	VOID AutoOperation();
	virtual	VOID DemoOperation();
	virtual VOID CycleOperation();
	virtual	VOID StopOperation();

	LONG OpCheckRelOffsetinFOV();		//v4.26T1	//Semitek

	BOOL SavePrData(BOOL bUpdateHMIVariable = TRUE);
	BOOL LoadPrData(VOID);
	BOOL LoadBondPrOption();
	BOOL SaveRecordID(VOID);
	VOID GetHMIVariable(VOID);
	VOID CopyBinBlockVariable(const LONG lSoureBlock, const LONG lTargetBlock);
	VOID UpdateHMIVariable(VOID);
	BOOL LoadRecordID(VOID);
	VOID InitSPCControl(VOID);
	VOID InitSPCControl_Arm1(VOID);
	VOID InitSPCControl_Arm2(VOID);
	//VOID InitSPCControl_OfflinePBTest(VOID);		//v4.11T3
	VOID InitVariable(VOID);
	LONG GetDieNo(const LONG bSelectDieType, const LONG lRefDieNo);
	VOID UpdateHMIVisionVariable(VOID);
	VOID GetHMIVisionVariable(VOID);
	VOID SelectBondCamera(VOID);
	VOID SelectAutoBondMode(VOID);
	VOID SelectLiveVideoMode(VOID);
	VOID SelectPostSealCamera(VOID);//v4.50A13
	BOOL BTMoveToAGCUPLUploadPos();
	BOOL BTMoveToUpLookPos(BOOL bBHZ2 = FALSE);
	VOID OutText(char *pMessage, PR_COORD stTxtCoord, PR_COLOR eTxtColor);
	VOID DrawAndEraseCursor(PR_COORD stStartPos, unsigned char ucCorner, BOOL bDraw);
	VOID DrawAndEraseLine(PR_COORD stStartPos, PR_COORD stEndPos, BOOL bDraw);
	VOID DrawRectangleBox(PR_COORD stCorner1, PR_COORD stCorner2, PR_COLOR ssColor);
	VOID DrawRectangleDieSize(PR_COLOR ssColor, BOOL bDieType, LONG lInputDieNo);
	VOID UpdateSearchDieArea(DOUBLE dSearchCodeX, DOUBLE dSearchCodeY, BOOL bUpdate, BOOL bRedraw = TRUE);
	VOID DisplaySearchDieResult(PR_UWORD usDieType, BOOL bDieType, LONG lInputDieNo, PR_REAL	fDieRotate, PR_COORD stDieOffset, PR_REAL fDieScore);
	VOID DisplayLearnDieResult(PR_UWORD lLearnStatus);

	VOID ConvertMotorStepToPixel(int siStepX, int siStepY, PR_COORD &stPixel);

	VOID ConvertPixelToUM(PR_COORD stPixel, double &dXum, double &dYum);
	VOID ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY);
	VOID ConvertPixelToMotorStep2(PR_COORD stPixel, int *siStepX, int *siStepY);	//For BT2	//v4.35T4
	VOID ConvertPixelToUnit(PR_COORD stPixel, LONG *lUnitX, LONG *lUnitY, BOOL bUseUm=FALSE);
	VOID ConvertPixelToDUnit(PR_COORD stPixel, DOUBLE& dUnitX, DOUBLE& dUnitY, BOOL bUseUm=FALSE);	//v3.23T1
	DOUBLE ConvertDUnitToPixel(CONST DOUBLE dUnit);		//v3.25T2
	BOOL VerifyPRRegion(PR_WIN *stInputArea);
	VOID CalculateDieCompenate(PR_COORD stDieOffset, int *siStepX, int *siStepY);
	VOID CalculateLFSearchArea(LONG lDirection, LONG lNumDie, PR_WIN *stInputArea);
	VOID CalculateLFDiePosn(const LONG lDirection, const LONG lNumDie, LONG &lDiePosnOffsetX, LONG &lDiePosnOffsetY);
	VOID MoveBinTable(int siXAxis, int siYAxis, BOOL bUseBT2=FALSE);
	VOID IndexBinTable(ULONG ulIndexNo);
	VOID GetBinTableEncoder(int *siXAxis, int *siYAxis);
	VOID GetBinTable2Encoder(int *siXAxis, int *siYAxis);
	VOID SetBinTableJoystick(BOOL bState);
	VOID ManualDieCompenate(PR_COORD stDieOffset, PR_REAL fDieRotate = 0);
	VOID ConvertUnit(int siStepX, int siStepY, double *dUnitX, double *dUnitY);
	VOID ConvertUnitToUm(int siStepX, int siStepY, double *dumX, double *dumY);		//v4.40T7	//Nichia
	BOOL BprCompensateBinRotateDie(LONG lDieEncX, LONG lDieEncY, LONG lDieEncT, DOUBLE dDieRotate);

	ULONG       m_ulEpoxySize;

	LONG	m_lISPDownTimeout;
	LONG	m_lISPDownSettleDelay;
	DOUBLE	*m_pAvgDieAngle;
//	CP_HC

	LONG LogPostBondData(LONG lPostBondType, PR_UWORD usDieType, PR_REAL fDieRotate, PR_COORD stDieOffset);
	VOID LogPBDataWithRowCol(LONG lRow, LONG lCol, USHORT usDieType, 
								DOUBLE dX, DOUBLE dY, ULONG ulCurrentBin, DOUBLE dRotate);
	DOUBLE GetSrchDieAreaX();
	DOUBLE GetSrchDieAreaY();

	BOOL UpdateBPRInfoList();
	BOOL SetbGenDieCalibration(BOOL bCalibration);
	BOOL SetszGenDieZoomMode(BOOL bEmpty);

	LONG GetBTCurrentBlock();
	LONG ExportPostBondData(ULONG ulCurrentBin);
	BOOL ExportPostBondDisplayData();		//v4.11T3
	LONG ExportPostBondData_Arm1(ULONG ulCurrentBin);
	LONG ExportPostBondData_Arm2(ULONG ulCurrentBin);
	VOID ResetPostBondSPCFlags();		//v3.93
	VOID DoOfflinePostBond(VOID);
	VOID DisplayLFSearchArea(CONST INT nDirection, PR_COLOR ssColor);	//v2.58
	VOID UpdateFovSize();
	VOID GetLightingConfig(int nGroupID, PR_SOURCE &emCoaxID, PR_SOURCE &emRingID, PR_SOURCE &emSideID);

	BOOL DieIsEmpty(PR_UWORD usDieType);
	BOOL DieIsAlignable(PR_UWORD usDieType);
	BOOL DieIsGood(PR_UWORD usDieType);
	BOOL DieIsDefective(PR_UWORD usDieType);
	BOOL AutoGrabImage(VOID);
	BOOL AutoGrabDone(VOID);

	BOOL AutoSearchDie(VOID);
	BOOL AutoGrabDieDone(VOID);
	LONG AutoDieResult(VOID);
	LONG AutoLogDieResult(VOID);	//Log die result for collet-hole shift measurement
	BOOL AutoLookAroundDieInFOV(BOOL& bCriticalErr, ULONG ulBin, BOOL bIsArm1);			//v4.45T2	//SanAn

	//Hw Trigger
	//PR_UWORD GrabShareImage(BOOL bDieType, LONG lInputDieNo, PR_UBYTE ubSID, PR_UBYTE ubRID);
	//PR_UWORD GrabShareImageRpy2(PR_UBYTE ubSID, PR_ULWORD *ulImageID, PR_ULWORD *ulStationID);
	//andrewng //2020-0615
	PR_WORD ManualSearchDie_HWTri(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
					PR_COORD stCorner1, PR_COORD stCorner2, DOUBLE dRefAngle = 0);
	PR_UWORD ExtractGrabShareImgDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, PR_COORD *stDieSize);
	BOOL GrabHwTrigger();
	//BOOL PreAutoSearchDie_HWTri(VOID);
	BOOL AutoSearchDie_HWTri(BOOL bAutoBond = FALSE);
	BOOL AutoGrabDieDone_HWTri(VOID);
	LONG AutoDieResult_HWTri(PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, BOOL bPostBond = TRUE);

	BOOL m_bLastBH1DieIsEmpty;
	BOOL m_bLastBH2DieIsEmpty;
	BOOL AutoLFLastRowDieInFOV(BOOL& bCriticalError, ULONG ulBin, PR_COORD stCDieOffset, BOOL bIsArm1);		//v4.47T8	//WH SanAn
	BOOL AutoSearchRTPadOffsetXY(BOOL bIsArm1, INT nDieCenterXInPixel, INT nDieCenterYInPixel, 
									INT siDieStepX, INT siDieStepY, 
									DOUBLE dCenterXInPixel, DOUBLE dCenterYInPixel);		//CSP	
	VOID UpdateBH1BH2SearchRange(PR_COORD &stCollet1BondCorner1, PR_COORD &stCollet1BondCorner2,
								 PR_COORD &stCollet2BondCorner1, PR_COORD &stCollet2BondCorner2,
								 CString &szBH1Log, CString &szBH2Log);
	VOID GetBondAlignSearchRange(CString szTitle, const BOOL bIsBHArm2, PR_WIN &stSearchArea);
	
	LONG m_lLFLastRowDieCounter;		//v4.47T8
	LONG m_lLFLastRowDieErrAccCount;	//v4.47T10
	LONG OpBPRReSrchDie(PR_SRCH_DIE_RPY2 &stSrchRpy2, PR_SRCH_DIE_RPY3 &stSrchRpy3);			//v4.xx
	BOOL m_bReSearchCurrDie;

	BOOL CheckMaxRotation(ULONG ulBin, DOUBLE dAngle, BOOL bIsArm1=TRUE);
	BOOL CheckMaxAccRotation(ULONG ulBin, DOUBLE dAngle, BOOL bIsArm1=TRUE);	//v4.58A5
	BOOL CheckDieShifted(ULONG ulBin, DOUBLE dShiftX, DOUBLE dShiftY, BOOL bIsArm1=TRUE);
	BOOL CheckAverageRotation(ULONG ulBin);
	BOOL CheckAverageCp(ULONG ulBin, BOOL bIsArm1=TRUE);					//v4.51A17
	BOOL CheckDieDefect(ULONG ulBin, LONG lDieType);
	BOOL CheckDieEmpty(ULONG ulBin, LONG lDieType, BOOL bIsArm1=TRUE);		//v4.39T6

	BOOL OpEnableElectech3EDLPostBondChecking(ULONG ulBin);					//v4.50A31
	BOOL CheckElectech3EDLPostBondChecking();								//v4.51A2
	BOOL m_bEnableElectech3EDLCheck150Bin;

	LONG FindDieCalibration(BOOL bNormalDie = TRUE);
	LONG FindDieCalibration2(BOOL bNormalDie = TRUE);

	//PR_UWORD AutoDetectDieCorner(VOID);
	PR_UWORD AutoLearnDie(BOOL bDieType,LONG lInputDieNo);
	PR_UWORD SearchDieCmd(BOOL bDieType, LONG lInputNo, PR_COORD stDieULC, PR_COORD stDieLRC, 
						  PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect,
						  UCHAR ucColletID = 0, DOUBLE dRefAngle = 0);
	PR_UWORD SearchDieRpy1(PR_SRCH_DIE_RPY1 *stSrchRpy1);
	PR_UWORD SearchDieRpy2(PR_SRCH_DIE_RPY2 *stSrchRpy2);
	PR_UWORD SearchDieRpy3(PR_SRCH_DIE_RPY3 *stSrchRpy3);
	PR_UWORD ExtractDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore);
	//PR_UWORD ManualSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore);
	PR_WORD ManualSearchDie(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
					PR_COORD stCorner1, PR_COORD stCorner2, DOUBLE dRefAngle = 0, INT nDebug = 0);
	BOOL CalculateDefectScore(PR_UWORD usDieType, BOOL bDieType, LONG lInputDieNo, PR_SRCH_DIE_RPY3 *stSrchRpy3);	
	VOID GetSearchDieArea(PR_WIN* stSrchArea, INT nDieNo, INT nEnlarge=0);
	PR_UWORD ExtractObjectCmd(BOOL bColletOffset,double &lX,double &lY,double &dAngle);
	VOID FreeAllPrRecords(VOID);

	VOID MoveBhToBond(CONST BOOL bBond);	//v2.60
	VOID MoveBhToBond_Z2(CONST BOOL bBond);	//dual arm learn collet
	BOOL BPR_GetFrameLevel();

	BOOL ShowPrImage(CString& szImagePath, LONG& lDieNo, BOOL bDieType, LONG lRefDieNo);

	LONG LogItems(LONG lEventNo);
	LONG GetLogItemsString(LONG lEventNo, CString& szMsg);

	BOOL TurnOnOffPrLighting(BOOL bOn);

	PR_DIE_ALIGN_ALG	GetLrnAlignAlgo(LONG lDieNo);
	PR_BACKGROUND_INFO	GetLrnBackGround(LONG lDieNo);
	//v3.86
	//RUnTime BT XY compensation for BH thermo-shift effect	//MS100
	VOID InitRtBHCompXYArrays(const BOOL bBHZ1, BOOL bUpdateSRAM = FALSE);
	BOOL TransformBHCompXYArrays(CONST UCHAR ucRotation);	//v4.50A9
	BOOL AddRTBHCompXY(CONST INT nBHIndex, CONST LONG lStepX, CONST LONG lStepY);
	BOOL UpdateAvgRTCompOffsetXY(CONST INT nBHIndex, LONG &lStepX, LONG &lStepY, DOUBLE &dUnitX, DOUBLE &dUnitY);
	BOOL PrintRtBHXYArray();								//v4.48A18
	
	//CSP
	VOID ResetPreBondDieNPadXYArrays();
	BOOL AddRTPreBondDieNPadXY(CONST INT nBHIndex, CONST LONG lStepX, CONST LONG lStepY);
	BOOL UpdateAvgRTPreBondDieNPadXY(CONST INT nBHIndex);

	//Draw Window
//	LONG	DrawThePrWindow();

private:

	PR_WIN		m_stSubImageWindow[100]; //4.51D20
	PR_WIN		m_stIMEffView;  //Not Sure how to use it

	// Set Event (BPR_Event.cpp)
	VOID SetBPRLatched(BOOL bState = TRUE);
	VOID SetBhTReady(BOOL bState = TRUE);
	VOID SetBhTReadyForBPR(BOOL bState = TRUE);		//v2.60
	VOID SetBTStable(BOOL bState = TRUE);
	VOID SetBTReSrchDie(BOOL bState = TRUE);		//v4.xx
	VOID SetBPRPostBondDone(BOOL bState = TRUE);	//v4.40T6
	VOID SetWPRGrabImageReady(BOOL bState = TRUE, CString szMsg = "");
	VOID SetBhReadyForBPRHwTrigger(BOOL bState = TRUE);

	// Wait Event (BPR_Event.cpp)
	BOOL WaitBhTReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBhTReadyForBPR(INT nTimeout = LOCK_TIMEOUT);	//v2.60
	BOOL WaitBTStable(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBTReSrchDie(INT nTimeout = LOCK_TIMEOUT);		//v4.xx
	BOOL WaitWPRGrabImageReady(INT nTimeout = LOCK_TIMEOUT);
	BOOL WaitBhReadyForBPRHwTrigger(INT nTimeout = LOCK_TIMEOUT);

	// Sub-State Functions (BPR_SubState.cpp)
	INT OpInitialize();
	INT OpPreStart();
	VOID OpUpdateDieIndex();		// Update Die Index

	LONG UpdateOutput(IPC_CServiceMessage& svMsg);

	//General
	LONG UpdateHMIData(IPC_CServiceMessage& svMsg);

	LONG UpdateAction(IPC_CServiceMessage& svMsg);
	LONG LogItems(IPC_CServiceMessage& svMsg);
	LONG SetCleanProbeLighting(IPC_CServiceMessage& svMsg);
	LONG SaveTheCleanProbeLighting(IPC_CServiceMessage& svMsg);
	LONG SetGeneralLighting(IPC_CServiceMessage& svMsg);
	LONG GetGeneralLighting(IPC_CServiceMessage& svMsg);
	LONG RestoreGeneralLighting(IPC_CServiceMessage& svMsg);

	LONG BPR_BondLeftDie(IPC_CServiceMessage &svMsg);
	LONG ExtractObjectCmd(IPC_CServiceMessage &svMsg);

	//Learn Die
	LONG BprAutoColorAdjust(IPC_CServiceMessage& svMsg);
	LONG CancelLearnDie(IPC_CServiceMessage& svMsg);
	LONG ConfirmLearnDie(IPC_CServiceMessage& svMsg);
	LONG GetLearnDieLighting(IPC_CServiceMessage& svMsg);
	LONG SetLearnDieLighting(IPC_CServiceMessage& svMsg);
	LONG DisableLightingSetting(IPC_CServiceMessage& svMsg);
	LONG SetExposureTime(IPC_CServiceMessage& svMsg);			//ching
	LONG SetGExposureTime(IPC_CServiceMessage& svMsg);			//v3.30T1
	LONG GetGExposureTime(IPC_CServiceMessage& svMsg);			//v3.30T1
	LONG MovePRCursor(IPC_CServiceMessage& svMsg);
	LONG AutoLearnDie(IPC_CServiceMessage& svMsg);
	LONG AutoDetectDie(IPC_CServiceMessage& svMsg);

	LONG SetInspDetectRegion(IPC_CServiceMessage& svMsg);
	LONG EndInspDetectRegion(IPC_CServiceMessage& svMsg);
	LONG SetInspIgnoreRegion(IPC_CServiceMessage& svMsg);
	LONG EndInspIgnoreRegion(IPC_CServiceMessage& svMsg);
	BOOL m_bLrnAdvanceOption;
	LONG m_lInspDetectWinNo;
	LONG m_lInspIgnoreWinNo;
	PR_WIN	m_stInspDetectWin[BPR_MAX_DETECT_WIN];	
	PR_WIN	m_stInspIgnoreWin[BPR_MAX_DETECT_WIN];	

	LONG SavePrDataCmd(IPC_CServiceMessage& svMsg);

	//Search Die
	LONG ConfirmSearchDiePara(IPC_CServiceMessage& svMsg);
	LONG ConfirmSearchDieArea(IPC_CServiceMessage& svMsg);
	LONG UserSearchDie(IPC_CServiceMessage& svMsg);
	LONG UserSearchDie_NoMove(IPC_CServiceMessage& svMsg);
	LONG BT_MultiSearchDie(IPC_CServiceMessage& svMsg); //4.51D20
	BOOL IsPRLearnt(LONG lBondPRDieNo);
	LONG BT_MultiSearchInit(IPC_CServiceMessage &svMsg); //4.51D20
	LONG BT_MultiSearchFirstDie(IPC_CServiceMessage& svMsg);
//	LONG BT_MultiSearchFirstDie1(IPC_CServiceMessage& svMsg);
	VOID BT_ChangeSearchRange(PR_WIN &stSearchArea, const DOUBLE dRatio);
	LONG BT_LargeAreaSearchDie(IPC_CServiceMessage& svMsg); //20170824 leo
	LONG BT_SearchRefDie2(IPC_CServiceMessage& svMsg);			//v4.58A5
	LONG BT2_SearchDie(IPC_CServiceMessage& svMsg);				//v4.35T4	//BT2 in MS109
	LONG BT_SearchDieWithAngle(IPC_CServiceMessage& svMsg);		//v4.11T3	//PLLM Lumiramic Offline PB Test
//	LONG BT_SearchFirstDie(IPC_CServiceMessage& svMsg);
	LONG BT2_SearchFirstDie(IPC_CServiceMessage& svMsg);
    LONG BT_GetPRStatus(IPC_CServiceMessage& svMsg); 
	LONG BT_GetFOVSize(IPC_CServiceMessage &svMsg);
//	LONG BT_SearchDieInFOV(IPC_CServiceMessage& svMsg);
//	LONG BT_SearchDieXYInFOV(IPC_CServiceMessage& svMsg);		//v3.33
	LONG BT2_SearchDieXYInFOV(IPC_CServiceMessage& svMsg);		//v4.35T4
	LONG BT_SearchDieInMaxFOV(IPC_CServiceMessage& svMsg);
	LONG BT_SearchRefDieInFOV(IPC_CServiceMessage& svMsg);
	LONG BT_SearchRefDie2InFOV(IPC_CServiceMessage& svMsg);		//v4.58A5
	LONG BT_SearchRefDieErrMap(IPC_CServiceMessage& svMsg);		//andrewng //2020-0817
	LONG BT_SearchFrameAngleInFOV(IPC_CServiceMessage& svMsg);	//v4.58A5
//	LONG DrawSearchWindow(IPC_CServiceMessage& svMsg);			//v2.56
	LONG DrawDieSizeWindow(IPC_CServiceMessage& svMsg);
	LONG BT_GetDieSize(IPC_CServiceMessage& svMsg);				//v2.67
	LONG HotKeySearchRefDie(IPC_CServiceMessage& svMsg);		//v3.80
	LONG SearchPreBondPattern(IPC_CServiceMessage& svMsg);		//v3.80
	LONG LookForwardPreBondPattern(IPC_CServiceMessage& svMsg);		//andrewng1
	LONG ResetPrCenterXY(IPC_CServiceMessage& svMsg);			//v3.83

	// Mouse
	LONG UsePRMouse(IPC_CServiceMessage& svMsg);
	LONG DrawComplete(IPC_CServiceMessage& svMsg);
	LONG CancelMouseDraw(IPC_CServiceMessage& svMsg);

	//Other setup	
	LONG LearnDieCalibration(IPC_CServiceMessage& svMsg);
	LONG LearnDieCalibration2(IPC_CServiceMessage& svMsg);		//For BT2 of MS109	//v4.35T4
	LONG SelectRunTimeBinBlk(IPC_CServiceMessage& svMsg);
	LONG SelectOffLineBinBlk(IPC_CServiceMessage& svMsg);
	LONG ManualCompensate(IPC_CServiceMessage& svMsg);			//v3.50T1

	//postbond offline search
	LONG StartOfflinePostbond(IPC_CServiceMessage& svMsg);

	LONG PRIDDetailInfo(IPC_CServiceMessage &svMsg);
	LONG ConfirmSearchNGDiePara(IPC_CServiceMessage &svMsg);
	LONG ShowSearchDiePara(IPC_CServiceMessage &svMsg);
	LONG ClearAccEmptyCounter(IPC_CServiceMessage& svMsg);
	LONG ToggleLearnBondPosnZoomScreenCmd(IPC_CServiceMessage& svMsg);

	//postbond data handle
	LONG DeleteRuntimeData(IPC_CServiceMessage& svMsg);
	LONG DeleteOfflineData(IPC_CServiceMessage& svMsg);
	LONG DeleteRuntimeAllData(IPC_CServiceMessage& svMsg);
	LONG DeleteOfflineAllData(IPC_CServiceMessage& svMsg);
	LONG DisplaySPCMessage(IPC_CServiceMessage& svMsg);
	LONG CopyAllSPCValue(IPC_CServiceMessage& svMsg);

	// Upload and Download PR records
	LONG UploadPrRecords(IPC_CServiceMessage& svMsg);
	LONG DownloadPrRecords(IPC_CServiceMessage& svMsg);
	LONG DeletePrRecords(IPC_CServiceMessage& svMsg);
	LONG DeletePRRecord(IPC_CServiceMessage& svMsg);
	LONG DisplayPRRecord(IPC_CServiceMessage &svMsg);
	LONG DisplayPRIdListRecord(IPC_CServiceMessage &svMsg);

	LONG CheckPostBondSetup(IPC_CServiceMessage& svMsg);
	LONG PkgKeyParametersTask(IPC_CServiceMessage& svMsg);

	// PR ID checking function
	LONG CheckRecordID(IPC_CServiceMessage& svMsg);

	// Generate SPC data into HD
	LONG SaveSPCData(IPC_CServiceMessage& svMsg);
	LONG SaveSPCDisplayData(IPC_CServiceMessage& svMsg);

	LONG PreOnSelectShowPrImage(IPC_CServiceMessage& svMsg);
	LONG OnSelectShowPrImage(IPC_CServiceMessage& svMsg);
	LONG DisplayPRRecordInSummary(IPC_CServiceMessage& svMsg);

	VOID TeachBHZBondPos(const BOOL bBHZ2);
	VOID CalcBHZBondPosPixelToCount(const BOOL bBHZ2, int &siStepX, int &siStepY);
	BOOL ConfirmBHZBondPos(const BOOL bBHZ2, BOOL &bResult, int &siStepX, int &siStepY);
	BOOL PrestartConfirmBHZBondPos(const BOOL bBHZ2, BOOL &bResult, int &siStepX, int &siStepY);
	LONG CancelBHZBondPos(const BOOL bBHZ2);

	LONG TeachBHZ1BondPos(IPC_CServiceMessage& svMsg);
	LONG ConfirmBHZ1BondPos(IPC_CServiceMessage& svMsg);
	LONG CancelBHZ1BondPos(IPC_CServiceMessage& svMsg);
	LONG ResetBHZ1BondPos(IPC_CServiceMessage& svMsg);

	LONG TeachBHZ2BondPos(IPC_CServiceMessage& svMsg);
	LONG ConfirmBHZ2BondPos(IPC_CServiceMessage& svMsg);
	LONG CancelBHZ2BondPos(IPC_CServiceMessage& svMsg);
	LONG ResetBHZ2BondPos(IPC_CServiceMessage& svMsg);
	LONG AdjustCrossHair(IPC_CServiceMessage& svMsg);
	LONG AdjustCrossHairStepByStep(IPC_CServiceMessage& svMsg);
	LONG EnableBondPrInspction(IPC_CServiceMessage& svMsg);
	LONG TransformBHRTCompXY(IPC_CServiceMessage& svMsg);		//v4.50A9
	
	LONG CancelBHZ1OrBHZ2BondPos(IPC_CServiceMessage& svMsg);

	LONG CheckReferenceDie(IPC_CServiceMessage& svMsg);
				
	//v4.47A6
	LONG SearchColletHole(IPC_CServiceMessage &svMsg);
	PR_UWORD AutoLearnCircle(PR_COORD stULCorner, PR_COORD stLRCorner);
	PR_WORD AutoSearchCircle(LONG pcx, LONG pcy, LONG ecx, LONG ecy, UINT &nX, UINT &nY);
	VOID MoveBhToPick(CONST BOOL bPick);
	VOID MoveBhToPick_Z2(CONST BOOL bPick);

	//v4.50A10	//Bin Uplook camera for AGC
	LONG AGC_SearchUploadColletPR(IPC_CServiceMessage &svMsg);
	BOOL AGC_SearchUploadColletPR(LONG& lDXInPixel, LONG& lDYInPixel);

	LONG AGC_SearchColletHole(IPC_CServiceMessage &svMsg);
	LONG AGC_SearchColletInTray(IPC_CServiceMessage &svMsg);	//v4.53A12
	LONG AGC_ResetColletCentre(IPC_CServiceMessage &svMsg);		//v4.50A30
	BOOL AGC_SearchColletHole(CONST BOOL bBHZ2, LONG& lDXInPixel, LONG& lDYInPixel);
	PR_WORD AGC_AutoSearchCircle(LONG pcx, LONG pcy, LONG ecx, LONG ecy, UINT &nX, UINT &nY);
	PR_UWORD AGC_AutoLearnCircle(PR_COORD stULCorner, PR_COORD stLRCorner);
	BOOL CalibrateRectangle(DOUBLE dXYDimension, PR_COORD stDieULC, PR_COORD stDieLRC);
	LONG AGC_UsePRMouse(IPC_CServiceMessage& svMsg);
	LONG AGC_DrawRectComplete(IPC_CServiceMessage& svMsg);
	PR_WORD AGC_AutoSearchEpoxy(const LONG lPRID, const LONG lLearnEpoxySize,
							    LONG pcx, LONG pcy, LONG ecx, LONG ecy, UINT &nX, UINT &nY);
	BOOL AutoSearchPreBondEpoxy(BOOL bAuto, BOOL bLatch, LONG pcx, LONG pcy, UINT &nX, UINT &nY, ULONG& lEpoxySize);
	PR_WORD AGC_AutoShowEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner, const LONG lLearnUPLPRIndex);
	BOOL	AutoLearnPreBondEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner);	//v4.58A4
	PR_WORD		m_ssPSPRSrchID[2];				//v4.50A10	//PostSeal PR ID; for MS60 AGC fcn
	//v4.50A22
	LONG		m_lAGCC1CenterXInPixel;
	LONG		m_lAGCC1CenterYInPixel;
	LONG		m_lAGCC2CenterXInPixel;
	LONG		m_lAGCC2CenterYInPixel;
	DOUBLE		m_dAGCCalibX;	//Pixel to motor steps ratio
	DOUBLE		m_dAGCCalibY;	//Pixel to motor steps ratio
	PR_COORD	m_stAGCCalRectCornerPos[BPR_MAX_DIE_CORNER];	//v4.50A29

	LONG		m_lLearnEpoxySize[2];
	LONG		m_lBTCol;
	LONG		m_lBTRow;

	LONG GenerateConfigData(IPC_CServiceMessage& svMsg);
	BOOL SearchBPRColletHoleWithEpoxyPattern(BOOL bBH2);

	BOOL AutoLearnBondEpoxy(PR_COORD stULCorner, PR_COORD stLRCorner);
	BOOL BPR_AutoSearchBondEpoxy(BOOL bAuto, BOOL bLatch, BOOL bDisplayResult, LONG pcx, LONG pcy, UINT &nX, UINT &nY, 
		ULONG& lEpoxySize, CEvent *pevGrabImageReady,CString &szErrorMsg);
private:
	CEvent	m_evBPRLatched;				// BPR Grab Done
	CEvent	m_evBhTReady;				// BondHead T Ready for BPR start
	CEvent	m_evBhTReadyForBPR;			// BondHead T Ready for BPR Post-Bond at Bond		//v2.60
	CEvent	m_evBTStable;				// Bin Table Stable for BPR start
	CEvent	m_evBTReSrchDie;			// Confirm Search event		//v4.xx
	CEvent	m_evBPRPostBondDone;		// SPC logging done event	//v4.40T6
	CEvent	m_evWPRGrabImageReady;		// WPR Grab Image Ready for BPR Start
	CEvent	m_evBhReadyForBPRHwTrigger;	// BondHead Ready for BPR Hw Trigger

private:
	BOOL		m_bUsePostBond_Backup;
	BOOL		m_bPlacementCheck_Backup;
	BOOL		m_bRelOffsetCheck_Backup;
	BOOL		m_bDefectCheck_Backup;	
	BOOL		m_bEmptyCheck_Backup;	

	BOOL		m_bMoveEjrZAndPostBond;

	BOOL		m_bUseHWTrigger;							//andrewng //2020-0615
	PR_ULWORD	m_ulStationID;
	PR_ULWORD	m_ulImageID;
	INT			m_unHWGrabState;							//0=Not-Trigger, 1=PreGrabDone, 2=HWTriggerDone,

	LONG		m_lPrCenterX;								//v3.80
	LONG		m_lPrCenterY;								//v3.80
	LONG		m_lPrCenterOffsetXInPixel;					//v4.44A3
	LONG		m_lPrCenterOffsetYInPixel;					//v4.44A3
	LONG		m_lOfflineCount;
	BOOL		m_bStartOffline;	
	BOOL		m_bDieLearnt;	
	BOOL		m_bDieCalibrated;	
	BOOL		m_bDieCalibrated2;							// For BT2
	char		m_ucLearnDieCornerNo;						// Indicate LearnDieCorner No 
	DOUBLE		m_dCalibX, m_dCalibY;						// Die calibration factor
	DOUBLE		m_dCalibXY, m_dCalibYX;						// Die calibration factor
	DOUBLE		m_dCalibX2, m_dCalibY2;						// Die calibration factor for BT2
	DOUBLE		m_dCalibXY2, m_dCalibYX2;					// Die calibration factor for BT2
	DOUBLE		m_dFFModeCalibX, m_dFFModeCalibY;			// FF Mode Die calibration factor
	DOUBLE		m_dFFModeCalibXY, m_dFFModeCalibYX;			// FF Mode Die calibration factor
	LONG		m_lPrCal1stStepSize;						//v4.57A2	//MS90
	PR_COORD	m_stLearnDieCornerPos[BPR_MAX_DIE_CORNER];	// Learnt normal die corner 
	PR_COORD	m_stLearnNormalDieCornerPos[BPR_MAX_DIE_CORNER];	// Learnt normal die corner 
	PR_COORD	m_stLearnFFModeDieCornerPos[BPR_MAX_DIE_CORNER];	// Learnt normal die corner 

	BOOL		m_bSelectDieType;
	CString		m_szLrnDieType;
	LONG		m_lCurRefDieNo;
	PR_WORD		m_ssPRSrchID;								// Normal die search ID
	PR_COORD	m_stDieSize;								// Normal die size
	PR_WIN		m_stSearchArea;								// Normal die search area
	UCHAR		m_ucDieShape;								// Normal die shape
	UCHAR		m_ucGenDieShape;
	UCHAR		GetDieShape();
	UCHAR		CheckDieShape(UCHAR &ucDieShape);

	LONG		m_lNoLastStateFile;
	PR_WORD		m_ssGenPRSrchID[BPR_MAX_DIE];		
	PR_COORD	m_stGenDieSize[BPR_MAX_DIE];				
	BOOL		m_bGenDieLearnt[BPR_MAX_DIE];
	CString		m_szGenDieZoomMode[BPR_MAX_DIE];
	BOOL		m_bGenDieCalibration[BPR_MAX_DIE];
	BOOL		m_bGenSrchEnableDefectCheck[BPR_MAX_DIE];
	BOOL		m_bGenSrchEnableChipCheck[BPR_MAX_DIE];
	LONG		m_lGenLrnAlignAlgo[BPR_MAX_DIE];
	LONG		m_lGenLrnBackgroud[BPR_MAX_DIE];
	LONG		m_lGenLrnFineSrch[BPR_MAX_DIE];
	LONG		m_lGenLrnInspMethod[BPR_MAX_DIE];
	LONG		m_lGenLrnInspRes[BPR_MAX_DIE];
	LONG		m_lGenSrchAlignRes[BPR_MAX_DIE];
	LONG		m_lGenSrchDefectThres[BPR_MAX_DIE];
	LONG		m_lGenSrchDieAreaX[BPR_MAX_DIE];		//v3.82
	LONG		m_lGenSrchDieAreaY[BPR_MAX_DIE];		//v3.82
	LONG		m_lGenSrchDieScore[BPR_MAX_DIE];
	LONG		m_lGenSrchGreyLevelDefect[BPR_MAX_DIE];
	DOUBLE		m_dGenSrchChipArea[BPR_MAX_DIE];
	DOUBLE		m_dGenSrchSingleDefectArea[BPR_MAX_DIE];
	DOUBLE		m_dGenSrchTotalDefectArea[BPR_MAX_DIE];
	
	//Zoom Sensor
	CPrZoomSensorMode *m_pPrZoomSensorMode;
	BOOL	m_bBprContinuousZoom;
	LONG	m_lBprContZoomFactor;
	LONG	m_lBprZoomRoiShrink;
	LONG	m_lBprZoomDigital;
	LONG	m_lBprNmlZoomFactor;
	LONG	m_lBprRunZoomFactor;

	BOOL	m_bSensorZoomFFMode;
	DOUBLE	m_dBTResolution_UM_CNT;
protected:

	CString m_szBPRConfigFileName;
	CString m_szPostbondDataSource;
	CString m_szPostbondDataX;
	CString m_szPostbondDataY;
	CString m_szPostbondDataTheta;
	CString m_szPostbondDispX;
	CString m_szPostbondDispY;
	CString m_szPostbondDispTheta;

	/*** Arm1 ***/
	CString m_szPostbondDataSource_Arm1;
	CString m_szPostbondDataX_Arm1;
	CString m_szPostbondDataY_Arm1;
	CString m_szPostbondDataTheta_Arm1;
	CString m_szPostbondDispX_Arm1;
	CString m_szPostbondDispY_Arm1;
	CString m_szPostbondDispTheta_Arm1;
	/*** end Arm1 ***/

	/*** Arm2 ***/
	CString m_szPostbondDataSource_Arm2;
	CString m_szPostbondDataX_Arm2;
	CString m_szPostbondDataY_Arm2;
	CString m_szPostbondDataTheta_Arm2;
	CString m_szPostbondDispX_Arm2;
	CString m_szPostbondDispY_Arm2;
	CString m_szPostbondDispTheta_Arm2;
	/*** end Arm2 ***/

	CString m_szDieTypeInfo;

	//Lighting values
	LONG	m_lBPRLrnCoaxLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRLrnRingLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRLrnSideLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRGenCoaxLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRGenRingLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRGenSideLightLevel[BPR_MAX_LIGHT_GROUP];
	//v4.50A15
	LONG	m_lBPRLrnCoaxLightHmi;
	LONG	m_lBPRLrnRingLightHmi;
	LONG	m_lBPRLrnSideLightHmi;

	LONG	m_lBPRTmpCoaxLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRTmpRingLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRTmpSideLightLevel[BPR_MAX_LIGHT_GROUP];
	LONG	m_lBPRExposureTimeLevel;		//v3.30T1
	LONG	m_lBPRGExposureTimeLevel;		//v3.30T1
	//Learn Die variable
	LONG	m_lLrnAlignAlgo;
	LONG	m_lLrnBackground;
	LONG	m_lLrnFineSrch;
	LONG	m_lLrnInspMethod;
	LONG	m_lLrnInspRes;

	//Search die parameter
	BOOL	m_bSrchEnableDefectCheck;
	BOOL	m_bSrchEnableChipCheck;
	LONG	m_lSrchAlignRes;
	LONG	m_lSrchDefectThres;
	LONG	m_lSrchDieAreaX;
	LONG	m_lSrchDieAreaY;
	LONG	m_lSrchDieScore;
	LONG	m_lSrchGreyLevelDefect;
	DOUBLE	m_dSrchSingleDefectArea;
	DOUBLE	m_dSrchTotalDefectArea;
	DOUBLE	m_dSrchChipArea;
	DOUBLE	m_dSrchDieAreaX;		//v2.60
	DOUBLE	m_dSrchDieAreaY;		//v2.60

	//v2.96T4
	DOUBLE	m_dSpSingleDefectScore;
	DOUBLE	m_dSpTotalDefectScore;
	DOUBLE	m_dSpChipAreaScore;

	//v3.25T2
	BOOL	m_bEnableProberBondPR;
	BOOL	m_bEnableTotalDefect;
	BOOL	m_bEnableLineDefect;
	DOUBLE	m_dMinLineDefectLength;
	LONG	m_lMinNumOfLineDefects;
	LONG	m_lMaxNumOfDefects;
	LONG    m_bDisableLightingSetting;

	BOOL	m_bPreBondAlignUseEpoxySearch;	//v4.58A4	//MS90 PreBond Alignment
	BOOL	m_bCheckFrameAngleByHwMark;		//v4.59A1

	LONG	m_lCleanProbeLightLevel;

	//Postbond data - need to save
	BOOL	m_bAverageAngleCheck;	
	BOOL	m_bBinTableAngleCheck;	
	BOOL	m_bPlacementCheck;	
	BOOL	m_bRelOffsetCheck;			//v4.26T1		//Semitek
	BOOL	m_bDefectCheck;	
	BOOL	m_bEmptyCheck;				//v3.02T1
	//BOOL	m_bUsePostBond;				//v4.35T1		//Use MS896aStn static attribute instead
	BOOL	m_bMS100DisplayPbSrchRect;

	LONG	m_lCurrentBinBlock;	
	LONG	m_lGenMaxAngleAllow[BPR_MAX_BINBLOCK];	
	LONG	m_lGenMaxAccAngleAllow[BPR_MAX_BINBLOCK];	//v4.58A5
	LONG	m_lGenMaxDefectAllow[BPR_MAX_BINBLOCK];	
	LONG	m_lGenMaxEmptyAllow[BPR_MAX_BINBLOCK];		//v3.02T1
	LONG	m_lGenMaxAccEmptyAllow[BPR_MAX_BINBLOCK];	//v4.40T9
	LONG	m_lGenMaxShiftAllow[BPR_MAX_BINBLOCK];	
	LONG	m_lGenRelDieShiftAllow[BPR_MAX_BINBLOCK];	//v4.26T1	//SEmitek
	DOUBLE	m_dGenAverageAngle[BPR_MAX_BINBLOCK];	
	DOUBLE	m_dGenMaxAngle[BPR_MAX_BINBLOCK];	
	DOUBLE	m_dGenDieShiftX[BPR_MAX_BINBLOCK];	
	DOUBLE	m_dGenDieShiftY[BPR_MAX_BINBLOCK];	

	//HIM use - no need to save
	LONG	m_lMaxAngleAllow;	
	LONG	m_lMaxAccAngleAllow;	//v4.58A5	//SEmitek
	LONG	m_lMaxDefectAllow;	
	LONG	m_lMaxEmptyAllow;		//v3.02T1	
	LONG	m_lMaxAccEmptyAllow;	//v4.40T9	//Sanan	
	LONG	m_lMaxShiftAllow;	
	LONG	m_lMaxRelShiftAllow;	//v4.26T1	//SEmitek

	//v4.51A17	//SanAn TJ
	DOUBLE	m_dMinCpXAllow;
	DOUBLE	m_dMinCpYAllow;
	DOUBLE	m_dMinCpTAllow;
	DOUBLE	m_dSpcCpX;
	DOUBLE	m_dSpcCpY;
	DOUBLE	m_dSpcCpT;
	DOUBLE	m_dSpcCpkX;
	DOUBLE	m_dSpcCpkY;
	DOUBLE	m_dSpcCpkT;
	LONG	m_lSpcCpCounter;

	LONG	m_lCurNormDieID;
	LONG	m_lCurNormDieSizeX;
	LONG	m_lCurNormDieSizeY;
	DOUBLE	m_dCurNormDieSizeX;		//v3.23T1
	DOUBLE	m_dCurNormDieSizeY;		//v3.23T1
	DOUBLE	m_dAverageAngle;	
	DOUBLE	m_dMaxAngle;	
	DOUBLE	m_dMaxAccAngle;
	DOUBLE	m_dDieShiftX;	
	DOUBLE	m_dDieShiftY;	
	DOUBLE  m_dRowFovSize;	//4.53D01
	DOUBLE  m_dColFovSize;
	DOUBLE	m_dFovSize;			//v2.58
	BOOL	m_bUseMouse;
	DOUBLE	m_dBTXRes;			//v4.59A30	
	DOUBLE	m_dBTYRes;			//v4.59A30

	LONG	m_lBondPosOffsetXPixel1;
	LONG	m_lBondPosOffsetYPixel1;
	LONG	m_lBondPosOffsetXPixel2;
	LONG	m_lBondPosOffsetYPixel2;

	LONG	m_lBondPosOffsetXPixel;
	LONG	m_lBondPosOffsetYPixel;
	LONG	m_lTmpBondPosOffsetXPixel;
	LONG	m_lTmpBondPosOffsetYPixel;

	//Temp storage 
	LONG	m_lOutAngleCounter[BPR_MAX_BINBLOCK];
	LONG	m_lOutAngleCounter2[BPR_MAX_BINBLOCK];		//v4.57A14
	LONG	m_lAccOutAngleCounter[BPR_MAX_BINBLOCK];	//v4.46T4	//Semitek
	LONG	m_lAccOutAngleCounter2[BPR_MAX_BINBLOCK];	//v4.46T4	//Semitek

	LONG	m_lOutDefectCounter[BPR_MAX_BINBLOCK];	
	
	LONG	m_lOutEmptyCounter[BPR_MAX_BINBLOCK];		
	LONG	m_lOutEmptyCounter2[BPR_MAX_BINBLOCK];		//v4.39T6	//SanAn		//For MS100 BH2	
	LONG	m_lOutAccEmptyCounter[BPR_MAX_BINBLOCK];	//v4.40T9	//Sanan		
	LONG	m_lOutAccEmptyCounter2[BPR_MAX_BINBLOCK];	//v4.40T9	//Sanan
	LONG	m_lOutAccEmptyCounterTotal[BPR_MAX_BINBLOCK];

	//LONG	m_lOutCamBlockEmptyCounter[BPR_MAX_BINBLOCK];	//v4.46T9	//SanAn
	LONG	m_lOutCamBlockEmptyCounter;						//v4.46T9	//SanAn
	
	LONG	m_lOutShiftCounter[BPR_MAX_BINBLOCK];	
	LONG	m_lAccOutShiftCounter[BPR_MAX_BINBLOCK];	//v4.46T4	//Semitek
	LONG	m_lAccOutShiftCounter2[BPR_MAX_BINBLOCK];	//v4.46T4	//Semitek

	LONG	m_lOutRelShiftCounter[BPR_MAX_BINBLOCK];	//v4.26T1	//SEmitek
	LONG	m_lTotalDieBonded[BPR_MAX_BINBLOCK];	
	DOUBLE	m_dOutAverageAngle[BPR_MAX_BINBLOCK];	
	DOUBLE	m_dTotalAngle[BPR_MAX_BINBLOCK];	

	ULONG	m_ulLastBin;
	ULONG	m_ulLastIndex;
	LONG	m_lDiePitchX;				
	LONG	m_lDiePitchY;				
	BOOL	m_bMissingDie;

	PR_COORD m_stAutoDieOffset;		//v4.26T1	//Semitek	//Rel-XY shift checking

	INT		m_nArmSelection;	//dual arm learn collet
	INT		m_lLearnUPLPRIndex;

	CString m_szShowImagePath;
	CString m_szGenShowImagePath;
	CString m_szShowImageType;
	LONG	m_lShowImageNo;

	//Postbond inspection variable
	SPC_CDatabase* m_pDatabase;
	SPC_CProduct* m_pRuntimeProduct;
	SPC_CPart* m_pBin[BPR_MAX_BINBLOCK];

	SPC_CProduct* m_pDisplayProduct;
	SPC_CPart* m_pDspBin;

	CString m_szSpcYDisplayTitle;			//v4.59A16	//Renesas MS90


	//Run Time data
	DOUBLE	*m_pDieX[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieY[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieTheta[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieAvgAngle[BPR_MAX_BINBLOCK];
	BOOL	*m_pDieIsGood[BPR_MAX_BINBLOCK];
	LONG	*m_pDieType[BPR_MAX_BINBLOCK];
	ULONG	*m_pDieIndex[BPR_MAX_BINBLOCK];
	BOOL	m_arbIsPostBond[BPR_MAX_BINBLOCK];		//v3.93

	//For Display in autobond mode only
	BOOL	m_bDisplayUnitInUm;
	CString m_szSPCCmdDspDieX;		//v4.40T7
	CString m_szSPCCmdDspDieY;		//v4.40T7
	CString m_szSPCCmdDspDieT;		//v4.51A17
	DOUBLE	*m_pDspDieX;
	DOUBLE	*m_pDspDieY;
	DOUBLE	*m_pDspDieTheta;

	/*** Arm1 ***/
	//Postbond inspection variable
	SPC_CDatabase* m_pDatabase_Arm1;
	SPC_CProduct* m_pRuntimeProduct_Arm1;
	SPC_CPart* m_pBin_Arm1[BPR_MAX_BINBLOCK];
	SPC_CProduct* m_pDisplayProduct_Arm1;
	SPC_CPart* m_pDspBin_Arm1;

	//Run Time data
	DOUBLE	*m_pDieX_Arm1[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieY_Arm1[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieTheta_Arm1[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieAvgAngle_Arm1[BPR_MAX_BINBLOCK];
	BOOL	*m_pDieIsGood_Arm1[BPR_MAX_BINBLOCK];
	LONG	*m_pDieType_Arm1[BPR_MAX_BINBLOCK];
	ULONG	*m_pDieIndex_Arm1[BPR_MAX_BINBLOCK];

	//For Display in autobond mode only
	CString m_szSPCCmdDspDieX_Arm1;		//v4.40T7
	CString m_szSPCCmdDspDieY_Arm1;		//v4.40T7
	DOUBLE	*m_pDspDieX_Arm1;
	DOUBLE	*m_pDspDieY_Arm1;
	DOUBLE	*m_pDspDieTheta_Arm1;
	/*** end Arm1 ***/

	/*** Arm2 ***/
	//Postbond inspection variable
	SPC_CDatabase* m_pDatabase_Arm2;
	SPC_CProduct* m_pRuntimeProduct_Arm2;
	SPC_CPart* m_pBin_Arm2[BPR_MAX_BINBLOCK];
	SPC_CProduct* m_pDisplayProduct_Arm2;
	SPC_CPart* m_pDspBin_Arm2;
/*
	// Offline PB Test //	//v4.11T3	//PLLM Lumiramic
	//Postbond inspection variable
	SPC_CDatabase* m_pDatabase_OPBTest;
	SPC_CProduct* m_pDisplayProduct_OPBTest;
	SPC_CPart* m_pDspBin_OPBTest;
	DOUBLE	*m_pDspDieX_OPBTest;
	DOUBLE	*m_pDspDieY_OPBTest;
	DOUBLE	*m_pDspDieTheta_OPBTest;
	CString m_szPostbondDataSource_OPBTest;
	CString m_szPostbondDataX_OPBTest;
	CString m_szPostbondDataY_OPBTest;
	CString m_szPostbondDataTheta_OPBTest;
*/
	//Run Time data
	DOUBLE	*m_pDieX_Arm2[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieY_Arm2[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieTheta_Arm2[BPR_MAX_BINBLOCK];
	DOUBLE	*m_pDieAvgAngle_Arm2[BPR_MAX_BINBLOCK];
	BOOL	*m_pDieIsGood_Arm2[BPR_MAX_BINBLOCK];
	LONG	*m_pDieType_Arm2[BPR_MAX_BINBLOCK];
	ULONG	*m_pDieIndex_Arm2[BPR_MAX_BINBLOCK];

	//For Display in autobond mode only
	CString m_szSPCCmdDspDieX_Arm2;		//v4.40T7
	CString m_szSPCCmdDspDieY_Arm2;		//v4.40T7
	DOUBLE	*m_pDspDieX_Arm2;
	DOUBLE	*m_pDspDieY_Arm2;
	DOUBLE	*m_pDspDieTheta_Arm2;
	/*** end Arm2 ***/

	//v3.86
	//RUnTime BT XY compensation for BH thermo-shift effect	//MS100
	BOOL m_bEnableRTCompXY;
	LONG m_lRT_Count1;
	LONG m_lRT_Count2;
	LONG m_lBH1_RTX[BPR_RT_XYCOMP_ARRAY_SIZE];
	LONG m_lBH1_RTY[BPR_RT_XYCOMP_ARRAY_SIZE];
	LONG m_lBH2_RTX[BPR_RT_XYCOMP_ARRAY_SIZE];
	LONG m_lBH2_RTY[BPR_RT_XYCOMP_ARRAY_SIZE];

	//CSP
	LONG m_lRT_PreBondDieNPadCount1;
	LONG m_lRT_PreBondDieNPadCount2;
	LONG m_lBH1_PreBondDieNPadX[BPR_RT_XYCOMP_ARRAY_SIZE];
	LONG m_lBH1_PreBondDieNPadY[BPR_RT_XYCOMP_ARRAY_SIZE];
	LONG m_lBH2_PreBondDieNPadX[BPR_RT_XYCOMP_ARRAY_SIZE];
	LONG m_lBH2_PreBondDieNPadY[BPR_RT_XYCOMP_ARRAY_SIZE];

	BOOL m_bUpdateOutput;
	BOOL m_bDisableTurnOnOffPrLighting;

	CBondPrDataBlock m_oBondPrDataBlk;

	// Prober
	CMSNmAxisInfo	m_stRotarytTooling_T;		//Prober	//v4.37T11

	UCHAR	m_ucToolingStageToUse;
	
	//BOOL	m_bIsPowerOn_T;
	UCHAR	m_ucPbtToolingStageInUse;		//v4.37T11

	BOOL	m_bSkipTheCheckToolingHome;		//V1.01T635

	BOOL m_bIsPowerOn_Tooling_T;
	BOOL m_bSelTooling_T;

	BOOL m_bHMIReferenceDie;
};
