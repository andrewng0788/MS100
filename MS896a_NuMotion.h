
/////////////////////////////////////////////////////////////////
//	MS896A_Constant.h : Defines the Constant header
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wed, Oct 28, 2009
//	Revision:	1.00
//
//	By:			Andrew Ng
//				AAA COB DieSorter Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2009.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#ifdef NU_MOTION
#include "gmp_struct.h"
#ifndef ES101
#ifndef PROBER
//#include "CtrlTool.h"	//Open DAC with velocity damping
#include "agmp_ctrl_class_ext.h"
#include "agmp_ctrl_error.h"
#include "agmp_ctrl_fun_ext.h"
#include "agmp_ctrl_macro.h"
#include "agmp_ctrl_struct.h"
#endif
#endif
//#include "GModeExportHeader.h" // Sim Card Protection
#endif



////////////////////////////////////////////
//	Motion Platform
////////////////////////////////////////////
#define NU_SIM_CONTENT_SIZE			16

#define NU_MOTION_SAMPLE_RATE		8
#define NU_MOTION_DAC_RATIO			65536
#define UM_MAX_MOVE_PROFILE			10	//7		//NVC v0.02
#define UM_MAX_MOVE_CONTROL			11
#define UM_MAX_JOYSTICK_PROFILE		3
#define UM_MAX_SI_PORT				7	//4		//v4.16T3	//MS100 9Inch
#define UM_MAX_SO_PORT				7	//4		//v4.16T3	//MS100 9Inch
#define UM_MAX_CH_PORT				5
#define UM_MAX_SW_PORT				4

// change the size from 24 to 32 due to diemap sorter
#define UM_MAX_IO_BIT				32


typedef struct 
{
	LONG lMotionTime;
	LONG lStaticError;
	LONG lMinDacEncPos;
	LONG lMaxDacEncPos;
	LONG lMaxDacValue;
	LONG lMinDacValue;
	LONG lEndDacValue;
	LONG lSettleDacValue;

} DATA_LOG_INFO;


class CMSNmMoveControl: public CObject
{
	public:
		CString m_szID;
};


class CMSNmMoveProfile: public CObject
{
	public:
		CString m_szID; // original ID for machine msd
		DOUBLE	m_dJerk;
		DOUBLE	m_dMaxAcc;
		DOUBLE	m_dMaxDec;
		DOUBLE	m_dMaxVel;
		DOUBLE	m_dFinalVel;
};


class CMSNmSrchProfile: public CObject
{
	public:
		CString		m_szID;
		CString		m_szPort;
		LONG		m_lType;
		DOUBLE		m_dSpeed;
		DOUBLE		m_dDrvInSpeed;
		LONG		m_lDistance;
		ULONG		m_ulDrvInDistance;
		ULONG		m_ulMask;
		ULONG		m_ulAState;
};


class CMSNmJoyProfile: public CObject
{
	public:
		CString m_szID;
		DOUBLE	m_dMaxAcc;
		DOUBLE	m_dMaxDec;
		DOUBLE	m_dMaxVel;
		DOUBLE	m_dLowVel;
};


class CMSNmJoyConfig: public CObject
{
	public:
		CString m_szPort;
		UCHAR	m_ucProfileID;
		ULONG	m_ulBitPos;
		LONG	m_lUpperLimit;
		LONG	m_lLowerLimit;
		CMSNmJoyProfile stProfile[UM_MAX_JOYSTICK_PROFILE];
};


class CMSNmIoPort: public CObject
{
	public:
		CString m_szName;
		CString m_szBit[UM_MAX_IO_BIT];
};


class CMSNmChPort: public CObject
{
	public:
		CString m_szName;
};

class CMSNmSwPort: public CObject
{
	public:
		CString m_szName;
};

class CMSNmChSensor: public CObject
{
	public:
		CString m_szHome;
		CString m_szULimt;
		CString m_szLLimt;
};


class CMSNmAxisInfo: public CObject
{
	#define PROC_NUM	20
	#define PROC_L_NUM	10

	public:
		CMSNmAxisInfo() {};
		~CMSNmAxisInfo() {};

	public:
		CString m_szTag;
		CString m_szName;
		LONG	m_lPoles;
		LONG	m_lNouse;
		LONG	m_lEncoderCount;
		ULONG	m_ulEncoderCountPerRev;
		SHORT	m_sDacLimit;
		SHORT	m_sPosErrorLimit;
		SHORT	m_sTimeLimit;
		LONG	m_lStaticControl;
		LONG	m_lDynamicControl;
		LONG	m_lJoystickControl;
		LONG	m_lProfileType;
		LONG	m_lCommMethod;
		LONG	m_lDirection;
		LONG	m_lMotorType;
		UCHAR	m_ucMotionSystem;
		UCHAR	m_ucMoveProfileID;
		UCHAR	m_ucSrchProfileID;
		UCHAR	m_ucControlID;
		BOOL	m_bUseSCFProfile;
		LONG	m_lMinPos;
		LONG	m_lMaxPos;

		CMSNmJoyConfig		m_stJoyConfig;
		CMSNmChSensor		m_stSensor;
		CMSNmMoveProfile	m_stMoveProfile[UM_MAX_MOVE_PROFILE];
		CMSNmSrchProfile	m_stSrchProfile[UM_MAX_MOVE_PROFILE];
		CMSNmMoveControl	m_stControl[UM_MAX_MOVE_CONTROL];
		CMSNmChPort			m_NmCHPort[UM_MAX_CH_PORT];			//v0.02
		GMP_PROCESS_BLK		 m_astProcessBlk[PROC_NUM];
		GMP_PROC_BLK_DATA	 m_aProcBlkData[PROC_NUM];
		GMP_PROCESS_MODE	 m_aProcMode[PROC_NUM];
		GMP_PROCESS_ACTION	 m_aProcActionT[PROC_NUM];
		GMP_PROCESS_ACTION	 m_aProcActionF[PROC_NUM];

		GMP_PROCESS_BLK_LIST m_astPBL[PROC_L_NUM];
	public:
#ifdef NU_MOTION
		GMP_S16			m_sErrCode;
		GMP_MOTION_ID	m_stMotionID;
#endif
		double m_dEncResolution;
};

#define MS896A_ENC_PORT						0
#define MS896A_MODE_PORT					1
#define MS896A_CMD_PORT						2
#define MS896A_DAC_PORT						3
#define MS896A_ERR_PORT						4

//Software Port
#define MS896A_CFG_IO_SW					"Software Port"
#define MS896A_SW_OPENDAC_Z1				0
#define MS896A_SW_OPENDAC_Z2				1
#define MS896A_SW_AUTOBOND					2
#define MS896A_SW_WPR_GRAB_IMAGE			3

#define MS896A_CFG_NU_CONFIG				"Nu Config"
#define MS896A_CFG_SCF_FILENAME				"SCF File Name"
#define MS896A_CFG_ACTIVATOIN_FILENAME		"Activation File Name"
#define MS896A_CFG_IO						"I/O"
#define MS896A_CFG_IO_SI					"SI"
#define MS896A_CFG_IO_SO					"SO"
#define MS896A_CFG_IO_PORT					"Port"
#define MS896A_CFG_IO_BIT					"Bit"


#define MS896A_CFG_CH_PORT					"Channel Port"
#define MS896A_CFG_CH_PORT_ENC				"ENC Port"
#define MS896A_CFG_CH_PORT_SNR				"SNR Port"
#define MS896A_CFG_CH_PORT_MODE				"MODE Port"
#define MS896A_CFG_CH_PORT_CMD				"CMD Port"
#define MS896A_CFG_CH_PORT_DAC				"DAC Port"
#define MS896A_CFG_CH_PORT_ERR				"ERR Port"


//Begin Nu Data
#define MS896A_CFG_CH_NU_DATA				"Nu Data"
#define MS896A_CFG_CH_NAME					"Name"
#define MS896A_CFG_CH_MOTOR_TYPE			"Motor Type"
#define MS896A_CFG_CH_USE_SCF_PROFILE		"Use SCF Profile"
//#define MS896A_CFG_CH_MOTOR_DIRECTION		"Direction"

#define MS896A_CFG_CH_SENSOR				"Sensor"
#define MS896A_CFG_CH_SNR_HOME				"Home"
#define MS896A_CFG_CH_SNR_U_LIMIT			"U Limit"
#define MS896A_CFG_CH_SNR_L_LIMIT			"L Limit"

#define MS896A_CFG_CH_MOVE_PROFILE			"Move Profile"
#define MS896A_CFG_CH_MP_ID					"ID"
#define MS896A_CFG_CH_MP_JERK				"Jerk"
#define MS896A_CFG_CH_MP_MAX_ACC			"Max Acc"
#define MS896A_CFG_CH_MP_MAX_DEC			"Max Dec"
#define MS896A_CFG_CH_MP_MAX_VEL			"Max Vel"
#define MS896A_CFG_CH_MP_FINAL_VEL			"Final Vel"
#define MS896A_CFG_CH_MP_DISTANCE_MAX		"Distance Max"
#define MS896A_CFG_CH_MP_ACC_K_FACTOR		"Acc K Factor"

#define MS896A_CFG_CH_SRCH_PROFILE			"Search Profile"
#define MS896A_CFG_CH_SP_ID					"ID"
#define MS896A_CFG_CH_SP_TYPE				"Type"
#define MS896A_CFG_CH_SP_SPEED				"Speed"
#define MS896A_CFG_CH_SP_DISTANCE			"Distance"
#define MS896A_CFG_CH_SP_MASK				"Mask"
#define MS896A_CFG_CH_SP_ASTATE				"Active State"
#define MS896A_CFG_CH_SP_PORT				"Port"
#define MS896A_CFG_CH_SP_DRVIN_SPEED		"DrvIn Speed"
#define MS896A_CFG_CH_SP_DRVIN_DISTANCE		"DrvIn Distance"

#define MS896A_CFG_CH_JOY_PROFILE			"Joystick Profile"
#define MS896A_CFG_CH_JY_ID					"ID"
#define MS896A_CFG_CH_JY_PORT				"Port"
#define MS896A_CFG_CH_JY_BIT_POS			"Bit Pos"
#define MS896A_CFG_CH_JY_MAX_VEL			"Max Vel"
#define MS896A_CFG_CH_JY_MAX_ACC			"Max Acc"
#define MS896A_CFG_CH_JY_MAX_DEC			"Max Dec"
#define MS896A_CFG_CH_JY_LOW_VEL			"Low Vel"
#define MS896A_CFG_CH_JY_UPPER_LIMIT		"Upper Limit"
#define MS896A_CFG_CH_JY_LOWER_LIMIT		"Lower Limit"

#define MS896A_CFG_CH_CONTROL				"Control"
#define MS896A_CFG_CH_CT_ID					"ID"
//end Nu Data



