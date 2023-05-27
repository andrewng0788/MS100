#include "stdafx.h"
#include "Math.h"
#include "MS896A.h"
#include "MS896AStn.h"
#include "Motion_Constant.h"
#include "MarkConstant.h"
#include "BH_Constant.h"

#ifdef NU_MOTION_MS60		//v4.50A16	//compatible with MS100Plus2/3
//#include "CtrlTool.h"
#include "agmp_ctrl_class_ext.h"
#include "agmp_ctrl_error.h"
#include "agmp_ctrl_fun_ext.h"
#include "agmp_ctrl_macro.h"
#include "agmp_ctrl_struct.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static	BOOL	m_bNuDataLogEnable = FALSE;

VOID CMS896AStn::InitAxisData(CMSNmAxisInfo &pAxisInfo)
{
	LONG i=0;

	pAxisInfo.m_lPoles					= 0;
	pAxisInfo.m_lNouse					= 0;
	pAxisInfo.m_lEncoderCount			= 0;
	pAxisInfo.m_ulEncoderCountPerRev	= 0;
	pAxisInfo.m_sDacLimit				= 0;
	pAxisInfo.m_sPosErrorLimit			= 0;
	pAxisInfo.m_sTimeLimit				= 0;
	pAxisInfo.m_lStaticControl			= 0;
	pAxisInfo.m_lDynamicControl			= 0;
	pAxisInfo.m_lJoystickControl		= 0;
	pAxisInfo.m_lProfileType			= 0;
	pAxisInfo.m_lCommMethod				= 0;
	pAxisInfo.m_lDirection				= 0;
	pAxisInfo.m_lMotorType				= 0;
	pAxisInfo.m_ucMoveProfileID			= 0;
	pAxisInfo.m_ucSrchProfileID			= 0;
	pAxisInfo.m_ucControlID				= 0;

#ifdef NU_MOTION	
	pAxisInfo.m_sErrCode				= 0;

	//GMP structure
	ZeroMemory(&pAxisInfo.m_stMotionID, sizeof(pAxisInfo.m_stMotionID));
#endif

	//Joystick Data
	pAxisInfo.m_stJoyConfig.m_ucProfileID	= 0;
	pAxisInfo.m_stJoyConfig.m_ulBitPos		= 0;
	pAxisInfo.m_stJoyConfig.m_lLowerLimit	= 0;
	pAxisInfo.m_stJoyConfig.m_lUpperLimit	= 0;

	for (i=0; i<UM_MAX_JOYSTICK_PROFILE; i++)
	{
		pAxisInfo.m_stJoyConfig.stProfile[i].m_dLowVel = 0;
		pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxAcc	= 0;
		pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxDec	= 0;
		pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxVel	= 0;
	}

	//Profile Data
	for (i=0; i<UM_MAX_MOVE_PROFILE; i++)
	{
		pAxisInfo.m_stMoveProfile[i].m_dFinalVel	= 0;
		pAxisInfo.m_stMoveProfile[i].m_dJerk		= 0;
		pAxisInfo.m_stMoveProfile[i].m_dMaxAcc		= 0;
		pAxisInfo.m_stMoveProfile[i].m_dMaxDec		= 0;
		pAxisInfo.m_stMoveProfile[i].m_dMaxVel		= 0;

		pAxisInfo.m_stSrchProfile[i].m_lType		= 0;
		pAxisInfo.m_stSrchProfile[i].m_dSpeed		= 0;
		pAxisInfo.m_stSrchProfile[i].m_lDistance	= 0;
		pAxisInfo.m_stSrchProfile[i].m_ulMask		= 0;
		pAxisInfo.m_stSrchProfile[i].m_ulAState	= 0;

		pAxisInfo.m_stSrchProfile[i].m_dDrvInSpeed	= 0;
		pAxisInfo.m_stSrchProfile[i].m_ulDrvInDistance = 0;
	}
}

BOOL CMS896AStn::GetAxisData(CMSNmAxisInfo &pAxisInfo)
{
	BOOL bReturn = FALSE;
	/*
	//Get general data
	pAxisInfo.m_lDynamicControl		= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_DYNA_CONTROL);
	pAxisInfo.m_lStaticControl		= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_STAT_CONTROL);
	pAxisInfo.m_lJoystickControl	= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_JOY_CONTROL);
	pAxisInfo.m_lDirection			= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_MOTOR_DIRECTION);
	pAxisInfo.m_lProfileType		= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_PROFILE_TYPE);
	pAxisInfo.m_lCommMethod			= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_SWCOMM);
	pAxisInfo.m_lMinPos				= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_MIN_DISTANCE);
	pAxisInfo.m_lMaxPos				= GetChannelInformation(pAxisInfo.m_szTag, MS896A_CFG_CH_MAX_DISTANCE);
	*/

	m_CSect.Lock();

	CString szName; 
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
	CStringMapFile  *psmf = NULL;
	LONG i;


	//load & get config file ptr
	pUtl->LoadMachineConfig(); 
	psmf = pUtl->GetMachineConfigFile();

	if ( psmf != NULL )
	{
		//Get general data
		pAxisInfo.m_lDynamicControl		= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_DYNA_CONTROL);
		pAxisInfo.m_lStaticControl		= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_STAT_CONTROL);
		pAxisInfo.m_lJoystickControl	= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_JOY_CONTROL);
		pAxisInfo.m_lDirection			= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_MOTOR_DIRECTION);
		pAxisInfo.m_lProfileType		= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_PROFILE_TYPE);
		pAxisInfo.m_lCommMethod			= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_SWCOMM);
		pAxisInfo.m_lMinPos				= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_MIN_DISTANCE);
		pAxisInfo.m_lMaxPos				= GetChannelInformation(psmf, pAxisInfo.m_szTag, MS896A_CFG_CH_MAX_DISTANCE);

		//Axis Name
		pAxisInfo.m_szName = (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_NAME];

		//Motor Type
		pAxisInfo.m_lMotorType = (LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOTOR_TYPE];
		pAxisInfo.m_bUseSCFProfile = (BOOL)(LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_USE_SCF_PROFILE];

		//Motor Direction	//v3.61
		pAxisInfo.m_lDirection = (LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOTOR_DIRECTION];

		//Motor Sensor
		pAxisInfo.m_stSensor.m_szHome	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SENSOR][MS896A_CFG_CH_SNR_HOME];
		pAxisInfo.m_stSensor.m_szULimt	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SENSOR][MS896A_CFG_CH_SNR_U_LIMIT];
		pAxisInfo.m_stSensor.m_szLLimt	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SENSOR][MS896A_CFG_CH_SNR_L_LIMIT];			

		//Joystick Config & Profile
		pAxisInfo.m_stJoyConfig.m_szPort		= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][MS896A_CFG_CH_JY_PORT];
		pAxisInfo.m_stJoyConfig.m_ulBitPos		= (ULONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][MS896A_CFG_CH_JY_BIT_POS];
		pAxisInfo.m_stJoyConfig.m_lLowerLimit	= (LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][MS896A_CFG_CH_JY_UPPER_LIMIT];
		pAxisInfo.m_stJoyConfig.m_lUpperLimit	= (LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][MS896A_CFG_CH_JY_LOWER_LIMIT];

		//v0.02
		pAxisInfo.m_NmCHPort[MS896A_ENC_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_ENC];
		pAxisInfo.m_NmCHPort[MS896A_MODE_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_MODE];
		pAxisInfo.m_NmCHPort[MS896A_CMD_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_CMD];
		pAxisInfo.m_NmCHPort[MS896A_DAC_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_DAC];
		pAxisInfo.m_NmCHPort[MS896A_ERR_PORT].m_szName	= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_PORT][MS896A_CFG_CH_PORT_ERR];

		for (i=0; i<UM_MAX_JOYSTICK_PROFILE; i++)
		{
			pAxisInfo.m_stJoyConfig.stProfile[i].m_szID		= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][i+1][MS896A_CFG_CH_JY_ID];
			pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxAcc	= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][i+1][MS896A_CFG_CH_JY_MAX_ACC];
			pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxDec	= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][i+1][MS896A_CFG_CH_JY_MAX_DEC];
			pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxVel	= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][i+1][MS896A_CFG_CH_JY_MAX_VEL];
			pAxisInfo.m_stJoyConfig.stProfile[i].m_dLowVel	= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_JOY_PROFILE][i+1][MS896A_CFG_CH_JY_LOW_VEL];
		}

		//Move & Search Profiles
		for (i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			pAxisInfo.m_stMoveProfile[i].m_szID				= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOVE_PROFILE][i+1][MS896A_CFG_CH_MP_ID];
			pAxisInfo.m_stMoveProfile[i].m_dJerk			= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOVE_PROFILE][i+1][MS896A_CFG_CH_MP_JERK];
			pAxisInfo.m_stMoveProfile[i].m_dMaxAcc			= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOVE_PROFILE][i+1][MS896A_CFG_CH_MP_MAX_ACC];
			pAxisInfo.m_stMoveProfile[i].m_dMaxDec			= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOVE_PROFILE][i+1][MS896A_CFG_CH_MP_MAX_DEC];
			pAxisInfo.m_stMoveProfile[i].m_dMaxVel			= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOVE_PROFILE][i+1][MS896A_CFG_CH_MP_MAX_VEL];
			pAxisInfo.m_stMoveProfile[i].m_dFinalVel		= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_MOVE_PROFILE][i+1][MS896A_CFG_CH_MP_FINAL_VEL];

			pAxisInfo.m_stSrchProfile[i].m_szID				= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_ID];
			pAxisInfo.m_stSrchProfile[i].m_szPort			= (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_PORT];
			pAxisInfo.m_stSrchProfile[i].m_lType			= (LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_TYPE];
			pAxisInfo.m_stSrchProfile[i].m_dSpeed			= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_SPEED];
			pAxisInfo.m_stSrchProfile[i].m_lDistance		= (LONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_DISTANCE];
			pAxisInfo.m_stSrchProfile[i].m_ulMask			= (ULONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_MASK];
			pAxisInfo.m_stSrchProfile[i].m_ulAState			= (ULONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_ASTATE];
			pAxisInfo.m_stSrchProfile[i].m_dDrvInSpeed		= (DOUBLE)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_DRVIN_SPEED];
			pAxisInfo.m_stSrchProfile[i].m_ulDrvInDistance	= (ULONG)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_SRCH_PROFILE][i+1][MS896A_CFG_CH_SP_DRVIN_DISTANCE];
		}

		//Control
		for (i=0; i<UM_MAX_MOVE_CONTROL; i++)
		{
			pAxisInfo.m_stControl[i].m_szID = (CString)(*psmf)[MS896A_CFG_CHANNEL_ATTRIB][pAxisInfo.m_szTag][MS896A_CFG_CH_NU_DATA][MS896A_CFG_CH_CONTROL][i+1][MS896A_CFG_CH_CT_ID];
		}

		//initialization
		for (int i = 0; i < PROC_L_NUM; i++)
		{
			memset(&pAxisInfo.m_astPBL[i], 0x00, sizeof(pAxisInfo.m_astPBL[i]));
			pAxisInfo.m_astPBL[i].u8PBLSize = 0;
		}
		for (int i = 0; i < PROC_NUM; i++)
		{
			memset(&pAxisInfo.m_astProcessBlk[i], 0x00, sizeof(pAxisInfo.m_astProcessBlk[i]));
			pAxisInfo.m_astProcessBlk[i].pstProcBlkData		= &pAxisInfo.m_aProcBlkData[i];
			pAxisInfo.m_astProcessBlk[i].punProcessMode		= &pAxisInfo.m_aProcMode[i];
			pAxisInfo.m_astProcessBlk[i].punProcessActionT	= &pAxisInfo.m_aProcActionT[i];
			pAxisInfo.m_astProcessBlk[i].punProcessActionF	= &pAxisInfo.m_aProcActionF[i];
		}

		//close config file ptr
		pUtl->CloseMachineConfig();
	}

	m_CSect.Unlock();

	bReturn = TRUE;
	return bReturn;
}


VOID CMS896AStn::SetAxisMoveSrchProfileRatio(CMSNmAxisInfo &pAxisInfo, const double dRatio)
{
	//Move & Search Profiles
	for (LONG i = 0; i < UM_MAX_MOVE_PROFILE; i++)
	{
		pAxisInfo.m_stMoveProfile[i].m_dJerk			*= dRatio;
		pAxisInfo.m_stMoveProfile[i].m_dMaxAcc			*= dRatio;
		pAxisInfo.m_stMoveProfile[i].m_dMaxDec			*= dRatio;
		pAxisInfo.m_stMoveProfile[i].m_dMaxVel			*= dRatio;
		pAxisInfo.m_stMoveProfile[i].m_dFinalVel		*= dRatio;

		pAxisInfo.m_stSrchProfile[i].m_dSpeed			*= dRatio;
		pAxisInfo.m_stSrchProfile[i].m_lDistance		= _round(pAxisInfo.m_stSrchProfile[i].m_lDistance * dRatio);
		pAxisInfo.m_stSrchProfile[i].m_dDrvInSpeed		*= dRatio;
		pAxisInfo.m_stSrchProfile[i].m_ulDrvInDistance	= _round(pAxisInfo.m_stSrchProfile[i].m_ulDrvInDistance * dRatio);
	}
}


BOOL CMS896AStn::PrintAxisData(CMSNmAxisInfo &pAxisInfo, UCHAR ucIndex)
{
	BOOL bReturn = TRUE;
	CString szLogFileName = gszUSER_DIRECTORY + "\\Motion\\Axis_Data_" + pAxisInfo.m_szName + ".log";
	
	//v4.59A25
	if (ucIndex > 0)
	{
		CString szIndex;
		szIndex.Format("_%d", ucIndex);
		szLogFileName = gszUSER_DIRECTORY + "\\Motion\\Axis_Data_" + pAxisInfo.m_szName + szIndex + ".log";
	}
	
	CTime theTime = CTime::GetCurrentTime(); 
	FILE *fp=NULL;

	errno_t nErr = fopen_s(&fp, szLogFileName, "w+");
	if ((nErr == 0) && (fp != NULL))
	{
		fprintf(fp, "\n-------------------------------------------------\n");
		fprintf(fp, "%s\n", (LPCTSTR) theTime.Format("%y-%m-%d (%H:%M:%S)"));
		fprintf(fp, "ASIX =  %s\n\n", (LPCTSTR) pAxisInfo.m_szTag);
		fprintf(fp, "Dynamic Control    = %ld\n", pAxisInfo.m_lDynamicControl);
		fprintf(fp, "Static Control     = %ld\n", pAxisInfo.m_lStaticControl);
		fprintf(fp, "Joystick Control   = %ld\n", pAxisInfo.m_lJoystickControl);
		fprintf(fp, "Motor Direction    = %ld\n", pAxisInfo.m_lDirection);
		fprintf(fp, "Profile Type       = %ld\n", pAxisInfo.m_lProfileType);
		fprintf(fp, "SW Comm            = %ld\n\n", pAxisInfo.m_lCommMethod);
		
		//Axis Name
		fprintf(fp, "Name            = %s\n", (LPCTSTR) pAxisInfo.m_szName);
		
		//Motor Type
		fprintf(fp, "Motor Type      = %ld\n\n", pAxisInfo.m_lMotorType);
		
		//Motor Sensor
		fprintf(fp, "Sensor(Home)    = %s\n", (LPCTSTR) pAxisInfo.m_stSensor.m_szHome);
		fprintf(fp, "Sensor(U Limit) = %s\n", (LPCTSTR) pAxisInfo.m_stSensor.m_szULimt);
		fprintf(fp, "Sensor(L Limit) = %s\n\n", (LPCTSTR) pAxisInfo.m_stSensor.m_szLLimt);
		//v0.02
		fprintf(fp, "ENC Port Name   = %s\n",   (LPCTSTR) pAxisInfo.m_szName + pAxisInfo.m_NmCHPort[MS896A_ENC_PORT].m_szName);
		fprintf(fp, "MODE Port Name  = %s\n",   (LPCTSTR) pAxisInfo.m_szName + pAxisInfo.m_NmCHPort[MS896A_MODE_PORT].m_szName);
		fprintf(fp, "CMD Port Name   = %s\n",   (LPCTSTR) pAxisInfo.m_szName + pAxisInfo.m_NmCHPort[MS896A_CMD_PORT].m_szName);
		fprintf(fp, "DAC Port Name   = %s\n",   (LPCTSTR) pAxisInfo.m_szName + pAxisInfo.m_NmCHPort[MS896A_DAC_PORT].m_szName);
		fprintf(fp, "ERR Port Name   = %s\n\n", (LPCTSTR) pAxisInfo.m_szName + pAxisInfo.m_NmCHPort[MS896A_ERR_PORT].m_szName);

		//Joystick Config & Profile
		/*fprintf(fp, "JoyStick Profile(Port)     = %s\n", (LPCTSTR) pAxisInfo.m_stJoyConfig.m_szPort);
		fprintf(fp, "JoyStick Profile(Bit Pos)  = %lu\n", pAxisInfo.m_stJoyConfig.m_ulBitPos);
		fprintf(fp, "JoyStick Profile(U Limit)  = %ld\n", pAxisInfo.m_stJoyConfig.m_lLowerLimit);
		fprintf(fp, "JoyStick Profile(L Limit)  = %ld\n", pAxisInfo.m_stJoyConfig.m_lUpperLimit);
		
		for (INT i=0; i<UM_MAX_JOYSTICK_PROFILE; i++)
		{
			fprintf(fp, "JoyStick Profile(%d) ID      = %s\n", i, (LPCTSTR) pAxisInfo.m_stJoyConfig.stProfile[i].m_szID);
			fprintf(fp, "JoyStick Profile(%d) Max Acc = %.6f\n", i, pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxAcc);
			fprintf(fp, "JoyStick Profile(%d) Max Dec = %.6f\n", i, pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxDec);
			fprintf(fp, "JoyStick Profile(%d) Max Vel = %.6f\n", i, pAxisInfo.m_stJoyConfig.stProfile[i].m_dMaxVel);
			fprintf(fp, "JoyStick Profile(%d) Low Vel = %.6f\n", i, pAxisInfo.m_stJoyConfig.stProfile[i].m_dLowVel);
		}*/

		//Move Profiles
		for (INT i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			fprintf(fp, "Move Profile(%d) ID        = %s\n",   (INT)i, (LPCTSTR) pAxisInfo.m_stMoveProfile[i].m_szID);
			fprintf(fp, "Move Profile(%d) Jerk      = %.6f\n", (INT)i, pAxisInfo.m_stMoveProfile[i].m_dJerk);
			fprintf(fp, "Move Profile(%d) Max Acc   = %.6f\n", (INT)i, pAxisInfo.m_stMoveProfile[i].m_dMaxAcc);
			fprintf(fp, "Move Profile(%d) Max Dec   = %.6f\n", (INT)i, pAxisInfo.m_stMoveProfile[i].m_dMaxDec);
			fprintf(fp, "Move Profile(%d) Mav Vel   = %.6f\n", (INT)i, pAxisInfo.m_stMoveProfile[i].m_dMaxVel);
			fprintf(fp, "Move Profile(%d) Final Vel = %.6f\n", (INT)i, pAxisInfo.m_stMoveProfile[i].m_dFinalVel);
		}

		//Search Profiles
		for (INT i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			fprintf(fp, "Search Profile(%d) ID               = %s\n",	(INT)i, (LPCTSTR) pAxisInfo.m_stSrchProfile[i].m_szID);
			fprintf(fp, "Search Profile(%d) Port             = %s\n",	(INT)i, (LPCTSTR) pAxisInfo.m_stSrchProfile[i].m_szPort);
			fprintf(fp, "Search Profile(%d) Type             = %ld\n",	(INT)i, pAxisInfo.m_stSrchProfile[i].m_lType);
			fprintf(fp, "Search Profile(%d) Speed            = %.6f\n", (INT)i, pAxisInfo.m_stSrchProfile[i].m_dSpeed);
			fprintf(fp, "Search Profile(%d) Distance         = %ld\n",	(INT)i, pAxisInfo.m_stSrchProfile[i].m_lDistance);
			fprintf(fp, "Search Profile(%d) Mask             = %lu\n",	(INT)i, pAxisInfo.m_stSrchProfile[i].m_ulMask);
			fprintf(fp, "Search Profile(%d) Active State     = %lu\n",	(INT)i, pAxisInfo.m_stSrchProfile[i].m_ulAState);
			fprintf(fp, "Search Profile(%d) DriveIn Speed    = %.6f\n", (INT)i, pAxisInfo.m_stSrchProfile[i].m_dDrvInSpeed);
			fprintf(fp, "Search Profile(%d) DriveIn Distance = %lu\n",	(INT)i, pAxisInfo.m_stSrchProfile[i].m_ulDrvInDistance);
		}

		//Control
		for (INT i=0; i<UM_MAX_MOVE_CONTROL; i++)
		{
			fprintf(fp, "Control(%d) ID	= %s\n", (INT)i, (LPCTSTR) pAxisInfo.m_stControl[i].m_szID);
		}

		fclose(fp);
	}

	return bReturn;
}

BOOL CMS896AStn::PrintNuConfigData()
{
	BOOL bReturn = TRUE;

	CString szPort, szBit;
	CString szChName;

#ifdef NU_MOTION
	//CString szLogFileName = gszUSER_DIRECTORY + "\\History\\Motion_Config_Data.log";
	CString szLogFileName = gszUSER_DIRECTORY + "\\Motion\\Motion_Config_Data.log";
	CTime theTime = CTime::GetCurrentTime(); 
	FILE *fp=NULL;
	
	//if ( (fp = fopen(szLogFileName, "a+")) != NULL)
	errno_t nErr = fopen_s(&fp, szLogFileName, "w+");
	if ((nErr == 0) && (fp != NULL))		//v3.83
	{
		fprintf(fp, "\n-------------------------------------------------\n");
		fprintf(fp, "%s\n", (LPCTSTR) theTime.Format("%y-%m-%d (%H:%M:%S)"));

		//fprintf(fp, "SCF File Name        =  %s\n", CMS896AApp::GetMachineInformation(MS896A_CFG_SCF_FILENAME, MS896A_CFG_NU_CONFIG));
		//fprintf(fp, "Activation File Name =  %s\n", CMS896AApp::GetMachineInformation(MS896A_CFG_ACTIVATOIN_FILENAME, MS896A_CFG_NU_CONFIG));

		//Print SI
		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			fprintf(fp, "\n");
			szPort.Format("%s_%ld", MS896A_CFG_IO_PORT, i);
			fprintf(fp, szPort + " = %s\n", (LPCTSTR) CMS896AApp::m_NmSIPort[i].m_szName);

			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( j < 10 )
				{
					szBit.Format("%s_0%ld", MS896A_CFG_IO_BIT, j);
				}
				else
				{
					szBit.Format("%s_%ld", MS896A_CFG_IO_BIT, j);
				}
				fprintf(fp, szBit + " = %s\n", (LPCTSTR) CMS896AApp::m_NmSIPort[i].m_szBit[j]);
			}
		}

		//Print SO
		for (LONG i=0; i<UM_MAX_SO_PORT; i++)
		{
			fprintf(fp, "\n");
			szPort.Format("%s_%ld", MS896A_CFG_IO_PORT, i);
			fprintf(fp, szPort + " = %s\n", (LPCTSTR) CMS896AApp::m_NmSOPort[i].m_szName);

			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( j < 10 )
				{
					szBit.Format("%s_0%ld", MS896A_CFG_IO_BIT, j);
				}
				else
				{
					szBit.Format("%s_%ld", MS896A_CFG_IO_BIT, j);
				}
				fprintf(fp, szBit + " = %s\n", (LPCTSTR) CMS896AApp::m_NmSOPort[i].m_szBit[j]);
			}
		}

		//Print CH Port
		fprintf(fp, "\nENC Port  = %s\n",	(LPCTSTR) CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName);
		fprintf(fp, "MODE Port =  %s\n",	(LPCTSTR) CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName);
		fprintf(fp, "CMD Port  =  %s\n",	(LPCTSTR) CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName);
		fprintf(fp, "DAC Port  =  %s\n",	(LPCTSTR) CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName);
		fprintf(fp, "ERR Port  =  %s\n",	(LPCTSTR) CMS896AApp::m_NmCHPort[MS896A_ERR_PORT].m_szName);

		//Print Software Port
		for (LONG i=0; i<UM_MAX_SW_PORT; i++)
		{
			szPort.Format("%s_%ld", MS896A_CFG_IO_PORT, i);
			fprintf(fp, szPort + " = %s\n", (LPCTSTR) CMS896AApp::m_NmSWPort[i].m_szName);
		}

		fclose(fp);
	}
#endif

	return bReturn;
}

BOOL CMS896AStn::MotionIsServo(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsServo: " + szAxis);
			return FALSE;
		}
		if (pAxisInfo->m_lMotorType == MS899_SVO_MOTOR)
		{
			return TRUE;
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				return TRUE;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionIsDcServo(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsDcServo: " + szAxis);
			return FALSE;
		}
		if (pAxisInfo->m_lMotorType == MS899_DCSVO_MOTOR)
		{
			return TRUE;
		}
#else
		if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)	
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return TRUE;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionIsStepper(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsStepper: " + szAxis);
			return FALSE;
		}
		if (pAxisInfo->m_lMotorType == MS899_STP_MOTOR)
		{
			return TRUE;
		}
#else
		if (GetAxisType(szAxis) == MS899_STP_MOTOR)	
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return TRUE;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionIsPowerOn(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	try
	{
#ifdef OFFLINE
		return TRUE;
#endif

#ifdef NU_MOTION
		GMP_U32 uiResult = 0;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsPowerOn: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "PowerOn");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_rpt_ch_status(pcChName, &uiResult)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_rpt_ch_status", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			if ( uiResult & GMP_CH_STATUS_DRV_ON )
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				return pServo->IsPowerOn();
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return  pStepper->IsPowerOn();
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return pDcServo->IsPowerOn();
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

BOOL CMS896AStn::MotionPowerOn(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{

	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionPowerOn: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "PowerOn");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		GMP_STATIC_INPUT stStaticInput;
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);
		stStaticInput.pcStaticCtrlType = pcCtrlID;

		if ( (pAxisInfo->m_sErrCode = gmp_select_static_ctrl(pcChName, &stStaticInput)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_select_static_ctrl", pAxisInfo->m_szName + ":" + pAxisInfo->m_stControl[PL_STATIC].m_szID);
			throw e;
		}

		if ( (pAxisInfo->m_sErrCode = gmp_enable_driver(pcChName, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_enable_driver", pAxisInfo->m_szName);
			throw e;
		}

		if ( (pAxisInfo->m_sErrCode = gmp_close_servo_loop(pcChName, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_close_servo_loop", pAxisInfo->m_szName);
			throw e;
		}

		return TRUE;
#else	
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->PowerOn();
				return TRUE;
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->PowerOn();
				return TRUE;
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->PowerOn();
				return TRUE;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return FALSE;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionPowerOff(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

	try
	{
#ifdef OFFLINE
		return TRUE;
#endif

#ifdef NU_MOTION
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionPowerOff: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "PoweMotionPowerOffrOff");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_disable_driver(pcChName, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_disable_driver", pAxisInfo->m_szName);
			throw e;
		}

		return TRUE;
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->PowerOff();
				return TRUE;
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->PowerOff();
				return TRUE;
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->PowerOff();
				return TRUE;
			}
		}
#endif	
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}


BOOL CMS896AStn::MotionResetController(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionPowerOff: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "PoweMotionPowerOffrOff");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_reset_controller(pcChName, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_reset_controller", pAxisInfo->m_szName);
			throw e;
		}

		return TRUE;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}


BOOL CMS896AStn::MotionEnableEncoder(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionEnableEncoder: " + szAxis);
			return FALSE;
		}
		if (pAxisInfo->m_lMotorType == MS899_STP_MOTOR)
		{
			return TRUE;
		}
#else
		if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->EnableEncoder();
				return TRUE;
			}
		}
		else
		{
			return  TRUE;
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return FALSE;
	}
	
	return FALSE;
}

LONG CMS896AStn::MotionGetCommandPosition(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		GMP_S32 iPos;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionGetCommandPosition: " + szAxis);
			return 0;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionGetCommandPosition");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_rpt_cmd_posn(pcChName, &iPos)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_rpt_cmd_posn", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			return (LONG)iPos;
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return 0;
}

LONG CMS896AStn::MotionGetEncoderPosition(const CString& szAxis, DOUBLE dScale, CMSNmAxisInfo* pAxisInfo)
{

#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		GMP_S32 iPos;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionGetEncoderPosition: " + szAxis);
			return 0;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionGetEncoderPosition");
			throw e;
		}
		strcpy_s(pcChName,sizeof(pcChName),  pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_rpt_ch_enc_posn(pcChName, &iPos)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_rpt_ch_enc_posn", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			if (pAxisInfo->m_lMotorType == MS899_STP_MOTOR)
			{
				return (LONG)((DOUBLE)iPos * dScale + 0.5);
			}
			else
			{
				return (LONG)iPos;
			}
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				 return ((LONG)(pServo->GetEncoderPosition() * dScale));
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return ((LONG)(pStepper->GetEncoderPosition() * dScale));
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return ((LONG)(pDcServo->GetEncoderPosition() * dScale));
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return 0;
}

BOOL CMS896AStn::MotionSetEncoderDirection(const CString& szAxis, BYTE ucDirection, CMSNmAxisInfo* pAxisInfo)
{
	SHORT sResult=0;

	try
	{
#ifdef NU_MOTION
		
		//No need to implement this function

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				sResult = hp_set_encoder_dir(&pServo->m_HpCcb, ucDirection);
				return sResult;
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				sResult = hp_set_encoder_dir(&pStepper->m_HpCcb, ucDirection);
				return sResult;
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				sResult = hp_set_encoder_dir(&pDcServo->m_HpCcb, ucDirection);
				return sResult;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionReverseMotorDirection(const CString& szAxis, BYTE ucReverse, CMSNmAxisInfo* pAxisInfo)
{
	SHORT sResult=0;

	try
	{
#ifdef NU_MOTION
		
		//No need to implement this function

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				sResult = hp_reverse_motor_dir(&pServo->m_HpCcb, HP_AC_MOTOR, ucReverse);
				return sResult;
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				sResult = hp_reverse_motor_dir(&pStepper->m_HpCcb, HP_DC_MOTOR, ucReverse);
				return sResult;
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				sResult = hp_reverse_motor_dir(&pDcServo->m_HpCcb,  HP_DC_MOTOR, ucReverse);
				return sResult;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionSetSamplingFreqNew(const CString& szAxis, BYTE ucFreq, CMSNmAxisInfo* pAxisInfo)
{
	SHORT sResult=0;

	try
	{
#ifdef NU_MOTION

		//don't know

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				sResult = hp_set_sampling_freq_new(pServo->m_HpCcb.hp_servo_ch.p, ucFreq);
				return sResult;
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				sResult = hp_set_sampling_freq_new(pStepper->m_HpCcb.hp_servo_ch.p, ucFreq);
				return sResult;
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				sResult = hp_set_sampling_freq_new(pDcServo->m_HpCcb.hp_servo_ch.p, ucFreq);
				return sResult;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionIsComplete(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware == FALSE)
	{
		return TRUE;
	}

#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsComplete: " + szAxis);
			return FALSE;
		}
		if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 0)) != 0 )
		{
			//if ( pAxisInfo->m_sErrCode == GMP_TIMEOUT )
			if (pAxisInfo->m_sErrCode == GMP_MOTION_COMPLETE_TIMEOUT)
			{
				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				return pServo->IsComplete();
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return pStepper->IsComplete();
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return pDcServo->IsComplete();
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}
	
	return FALSE;
}

INT CMS896AStn::MotionMoveHome(const CString& szAxis, BOOL bMode, INT nSimWait, CMSNmAxisInfo* pAxisInfo, BOOL bDirectionReverse, BOOL bActiveStateReverse)
{
	INT nResult			= gnOK;
	
	try
	{
#ifdef OFFLINE
		return gnAMS_OK;
#endif
#ifdef NU_MOTION
		GMP_MOV_INPUT stMoveInput;
		GMP_EVT_OBJECT stSrhHome;
		GMP_PROCESS_BLK_LIST stPBL;
		GMP_U32 uiBitPos=0;
		GMP_U8 ucStartSrchDir = GMP_POSITIVE_SRH_DIR;
		CString szPortID;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionMoveHome: " + szAxis);
			return FALSE;
		}

		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( CMS896AApp::m_NmSIPort[i].m_szBit[j] == pAxisInfo->m_stSensor.m_szHome )
				{
					szPortID = CMS896AApp::m_NmSIPort[i].m_szName;
					uiBitPos = (GMP_U32)j;
					break;
				}
			}
		}			

		if ( szPortID.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No Home sensor", pAxisInfo->m_szName);
			throw e;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "SearchHome");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);
		strcpy_s(pcPortID, sizeof(pcPortID), szPortID);

		//Check condition
		if ( pAxisInfo->m_stSrchProfile[0].m_lDistance < 0 )
		{
			ucStartSrchDir = GMP_NEGATIVE_SRH_DIR;
		}
		else
		{
			ucStartSrchDir = GMP_POSITIVE_SRH_DIR;
		}

		if (bDirectionReverse)
		{
			ucStartSrchDir = (ucStartSrchDir == GMP_POSITIVE_SRH_DIR) ? GMP_NEGATIVE_SRH_DIR : GMP_POSITIVE_SRH_DIR;
		}

		stPBL.u8PBLSize								= 0;

		stMoveInput.enInputType						= GMP_SEARCH_TYPE;
		stMoveInput.pcCtrlType						= pcCtrlID;
		stMoveInput.stSrhInput.u8SrhDir				= ucStartSrchDir;
		stMoveInput.stSrhInput.bEnaSetPosn			= GMP_FALSE;
		stMoveInput.stSrhInput.s32DesiredAbsPosn	= 0;
		stMoveInput.stSrhInput.u8UserMotID			= 1;

		stSrhHome.enDrvInType						= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrhHome.enType							= GMP_EVT_SRH_PORT_AND;
		stSrhHome.enLmtType							= GMP_EVT_LMT_DIST; 
		stSrhHome.bEnaErrChk						= GMP_FALSE;
		stSrhHome.u16SrhDebounceSmpl				= 0;           
		stSrhHome.fSrhDrvInVel						= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[0].m_dDrvInSpeed;
		stSrhHome.u32DrvInCnt						= (GMP_U32)pAxisInfo->m_stSrchProfile[0].m_ulDrvInDistance;
		stSrhHome.fSrhVel							= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[0].m_dSpeed;
		stSrhHome.u32SrhLmt							= (GMP_U32)(abs(pAxisInfo->m_stSrchProfile[0].m_lDistance));
		stSrhHome.pcPort1							= pcPortID;
		stSrhHome.u32Mask							= 1<<uiBitPos;
		stSrhHome.u32ActiveState					= 1<<uiBitPos;

		if (bActiveStateReverse)
		{
			if ( stSrhHome.u32ActiveState == 0 )
			{
				stSrhHome.u32ActiveState = 1<<uiBitPos;
			}
			else
			{
				stSrhHome.u32ActiveState = 0;
			}
		}

		if ( stSrhHome.fSrhDrvInVel > stSrhHome.fSrhVel )
		{
			stSrhHome.fSrhDrvInVel	= stSrhHome.fSrhVel;
		}

		//Power On motor before call search motion
		CMS896AStn::MotionPowerOn(szAxis, pAxisInfo);

		//Search 1st direction
		if ( (pAxisInfo->m_sErrCode = gmp_srh(pcChName, &stMoveInput, &stSrhHome, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_srh(Home)", pAxisInfo->m_szName);
			throw e;
		}
		Sleep(250);

		/*
		if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
			throw e;
		}
		*/
		MotionSync(szAxis, 20000, pAxisInfo);
		Sleep(250);

		//Search 2nd direction
		if ( ucStartSrchDir == GMP_POSITIVE_SRH_DIR )
		{
			stMoveInput.stSrhInput.u8SrhDir	= GMP_NEGATIVE_SRH_DIR;
		}
		else
		{
			stMoveInput.stSrhInput.u8SrhDir	= GMP_POSITIVE_SRH_DIR;
		}

		if ( stSrhHome.u32ActiveState == 0 )
		{
			stSrhHome.u32ActiveState = 1<<uiBitPos;
		}
		else
		{
			stSrhHome.u32ActiveState = 0;
		}

		if ( (pAxisInfo->m_sErrCode = gmp_srh(pcChName, &stMoveInput, &stSrhHome, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_srh(Home)", pAxisInfo->m_szName);
			throw e;
		}
		Sleep(250);

		/*
		if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
			throw e;
		}
		*/
		MotionSync(szAxis, 20000, pAxisInfo);
		Sleep(250);

		return gnAMS_OK;

#else
		nResult = MoveHome(szAxis);
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return nResult;
}

INT CMS896AStn::MotionMoveHome2(const CString& szAxis, BOOL bMode, INT nSimWait, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult	= gnOK;
#ifdef OFFLINE
	return gnAMS_OK;
#endif
	
	try
	{
#ifdef NU_MOTION
		GMP_MOV_INPUT stMoveInput;
		GMP_EVT_OBJECT stSrhHome;
		GMP_PROCESS_BLK_LIST stPBL;
		GMP_U32 uiBitPos=0;
		GMP_U8 ucStartSrchDir = GMP_POSITIVE_SRH_DIR;
		CString szPortID;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionMoveHome: " + szAxis);
			return FALSE;
		}

		//v4.50A24	//Osram Germany CEMark MS109
		if (IsCEMarkMotors(szAxis) && CheckCEMarkCoverSensors())
		{
			DisplayMessage("Motion aborted due to CEMark Door Sensors in MotionMoveHome2: " + szAxis);
			return FALSE;
		}

		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( CMS896AApp::m_NmSIPort[i].m_szBit[j] == pAxisInfo->m_stSensor.m_szHome )
				{
					szPortID = CMS896AApp::m_NmSIPort[i].m_szName;
					uiBitPos = (GMP_U32)j;
					break;
				}
			}
		}			

		if ( szPortID.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No Home sensor", pAxisInfo->m_szName);
			throw e;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "SearchHome");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);
		strcpy_s(pcPortID, sizeof(pcPortID), szPortID);

		//Check condition
		if ( pAxisInfo->m_stSrchProfile[0].m_lDistance < 0 )
		{
			ucStartSrchDir = GMP_NEGATIVE_SRH_DIR;
		}
		else
		{
			ucStartSrchDir = GMP_POSITIVE_SRH_DIR;
		}

		stPBL.u8PBLSize								= 0;

		stMoveInput.enInputType						= GMP_SEARCH_TYPE;
		stMoveInput.pcCtrlType						= pcCtrlID;
		stMoveInput.stSrhInput.u8SrhDir				= ucStartSrchDir;
		stMoveInput.stSrhInput.bEnaSetPosn			= GMP_FALSE;
		stMoveInput.stSrhInput.s32DesiredAbsPosn	= 0;
		stMoveInput.stSrhInput.u8UserMotID			= 1;

		stSrhHome.enDrvInType						= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrhHome.enType							= GMP_EVT_SRH_PORT_AND;
		stSrhHome.enLmtType							= GMP_EVT_LMT_DIST; 
		stSrhHome.bEnaErrChk						= GMP_FALSE;
		stSrhHome.u16SrhDebounceSmpl				= 0;           
		stSrhHome.fSrhDrvInVel						= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[0].m_dDrvInSpeed;
		stSrhHome.u32DrvInCnt						= (GMP_U32)pAxisInfo->m_stSrchProfile[0].m_ulDrvInDistance;
		stSrhHome.fSrhVel							= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[0].m_dSpeed;
		stSrhHome.u32SrhLmt							= (GMP_U32)(abs(pAxisInfo->m_stSrchProfile[0].m_lDistance));
		stSrhHome.pcPort1							= pcPortID;
		stSrhHome.u32Mask							= 1<<uiBitPos;
		stSrhHome.u32ActiveState					= 1<<uiBitPos;

		if ( stSrhHome.fSrhDrvInVel > stSrhHome.fSrhVel )
		{
			stSrhHome.fSrhDrvInVel	= stSrhHome.fSrhVel;
		}

		//Power On motor before call search motion
		CMS896AStn::MotionPowerOn(szAxis, pAxisInfo);

		//Search 1st direction
		if ( (pAxisInfo->m_sErrCode = gmp_srh(pcChName, &stMoveInput, &stSrhHome, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_srh(Home)", pAxisInfo->m_szName);
			throw e;
		}

		Sleep(250);
		if (bMode)	//SFM_WAIT
		{
			MotionSync(szAxis, 20000, pAxisInfo);
			Sleep(250);
		}

/*
		//Search 2nd direction
		if ( ucStartSrchDir == GMP_POSITIVE_SRH_DIR )
		{
			stMoveInput.stSrhInput.u8SrhDir	= GMP_NEGATIVE_SRH_DIR;
		}
		else
		{
			stMoveInput.stSrhInput.u8SrhDir	= GMP_POSITIVE_SRH_DIR;
		}

		if ( stSrhHome.u32ActiveState == 0 )
		{
			stSrhHome.u32ActiveState = 1;
		}
		else
		{
			stSrhHome.u32ActiveState = 0;
		}

		if ( (pAxisInfo->m_sErrCode = gmp_srh(pcChName, &stMoveInput, &stSrhHome, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_srh(Home)", pAxisInfo->m_szName);
			throw e;
		}

		Sleep(250);
		MotionSync(szAxis, 20000, pAxisInfo);
		Sleep(250);
*/
		return gnAMS_OK;

#else
		nResult = MoveHome(szAxis);
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return nResult;
}

LONG CMS896AStn::MotionSetPosition(const CString& szAxis, LONG lPos, CMSNmAxisInfo* pAxisInfo)
{
	LONG lResult = gnOK;

	try
	{
#ifdef OFFLINE

		return TRUE;

#else	//andrewng //2020-0609

		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		GMP_U8 u8SetPosnFlag = GMP_SET_ABS_CMD_FB_POSN;		//v4.39T4

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetPosition: " + szAxis);
			return gnNOTOK;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "SetPosition");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if (pAxisInfo->m_lMotorType == MS899_STP_MOTOR)
		{
			GMP_COMPONENT_INFO stComponentInfo;
			GMP_S16 ssReturn = gmp_get_component_info(pcChName, &stComponentInfo);

			if ((ssReturn == 0) && (stComponentInfo.stCompInfo.stCh.u8NumOfEnc == 0))
			{
				u8SetPosnFlag = GMP_SET_ABS_CMD_POSN;
			}
		}

		if ( (pAxisInfo->m_sErrCode = gmp_set_posn(pcChName, u8SetPosnFlag, (GMP_S32)lPos, 1, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_set_posn", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			return TRUE;
		}

#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return lResult;
}

INT CMS896AStn::MotionSetContourProfile(const CString& szAxis, FLOAT* fPos, FLOAT* fVel, FLOAT* fAcc, 
										FLOAT* fJerk, UINT uiNoofPoint, UINT uiStartOffset, CMSNmAxisInfo* pAxisInfo)
{
	try
	{

#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "SetContourProfile");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_set_contour_mov(pcChName, fPos, fVel, fAcc, fJerk, uiNoofPoint, (GMP_U32)uiStartOffset)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "SetContourProfile", pAxisInfo->m_szName);
			throw e;
		}
#endif

	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}


INT CMS896AStn::MotionContourMove(const CString& szAxis, UINT uiNoofPoint, UINT uiStartOffset, BOOL bWait, 
								  CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef OFFLINE
		return TRUE;
#endif

#ifdef NU_MOTION		
		GMP_MOV_INPUT stMovInput;
		GMP_PROF stProf;                               
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];


		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "ContourMove");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);

		stPBL.u8PBLSize = 0;

		stProf.enProfType							= PRESET_CONTOUR_PROF;
	
		stMovInput.enInputType						= GMP_CONTOURMOV_TYPE;
		stMovInput.pcCtrlType						= pcCtrlID;
		stMovInput.stContourMovInput.u32Len			= uiNoofPoint;
		stMovInput.stContourMovInput.u32StartOfst	= uiStartOffset;
		stMovInput.stContourMovInput.u8UserMotID	= 1;


		if ( (pAxisInfo->m_sErrCode = gmp_mov(pcChName, &stMovInput, &stProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_contour_mov", pAxisInfo->m_szName);
			throw e;
		}
#endif

	}
	catch (CAsmException e)
	{
	
		DisplayException(e);
	}

	return TRUE;
}

INT CMS896AStn::MotionScfProfileMove(const CString& szAxis, INT nPos, INT nMode, CMSNmAxisInfo* pAxisInfo)
{
	INT nTime = 0;
#ifdef OFFLINE
	return TRUE;
#endif

	try
	{
#ifdef NU_MOTION
		GMP_MOV_INPUT stMovInput;                           
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionMove: " + szAxis);
			return gnNOTOK;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionMove");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);
		stPBL.u8PBLSize = 0;
		stMovInput.enInputType					= GMP_3ORD_MOV_B_TYPE;
		stMovInput.pcCtrlType					= pcCtrlID;
		//stMovInput.st3OrdMovBInput.s32Dist	= (GMP_S32)nPos * nMotorDirection;
		stMovInput.st3OrdMovBInput.s32Dist		= (GMP_S32)nPos;
		stMovInput.st3OrdMovBInput.u8UserMotID	= 1;

		//v3.64
		CString szLog;
		szLog.Format("gmp_mov by channel name (%d) -- ", nPos);
		szLog = szLog + pAxisInfo->m_szName;
		CMSLogFileUtility::Instance()->MS_LogNuMotionOperation(szLog);

		CHAR pcProfileName[GMP_CH_NAME_CHAR_LEN];
		strcpy_s(pcProfileName, sizeof(pcProfileName), pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_szID);
		
		if ( (pAxisInfo->m_sErrCode = gmp_mov(pcChName, &stMovInput, pcProfileName, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_mov by channnel name", pAxisInfo->m_szName);
			throw e;
		}		

		if ( nMode == SFM_WAIT )
		{
			MotionSync(szAxis, 20000, pAxisInfo);
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}

INT CMS896AStn::MotionMove(const CString& szAxis, INT nPos, INT nMode, CMSNmAxisInfo* pAxisInfo)
{
	//INT nResult = gnOK;
	INT nTime = 0;
#ifdef OFFLINE
	return TRUE;
#endif

	try
	{
		GMP_MOV_INPUT stMovInput;
		GMP_PROF stProf;                               
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionMove: " + szAxis);
			return gnNOTOK;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionMove");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);

		stPBL.u8PBLSize = 0;

		stProf.enProfType						= THIRD_ORD_MOV_PROF_B;
		stProf.st3OrdMoveProfB.fFinalVel		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dFinalVel;
		stProf.st3OrdMoveProfB.fJerk			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dJerk;
		stProf.st3OrdMoveProfB.fMaxVel			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;
		stProf.st3OrdMoveProfB.fMaxAcc			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc;
		stProf.st3OrdMoveProfB.fMaxDec			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec;
		
		CString szLog;
		szLog.Format("MotionMove %s (%s): Ctrl = %s, Profile = %f %f %f %f %f, Dist = %d", 
			szAxis,
			pAxisInfo->m_szName,
			pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dFinalVel,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dJerk,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec,
			nPos);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		
		/*
		//v3.61
		INT nMotorDirection = 1;
		if (pAxisInfo->m_lDirection < 0)
		{
			nMotorDirection = -1;
		}
		*/

		stMovInput.enInputType					= GMP_3ORD_MOV_B_TYPE;
		stMovInput.pcCtrlType					= pcCtrlID;
		stMovInput.st3OrdMovBInput.s32Dist		= (GMP_S32)nPos;
		stMovInput.st3OrdMovBInput.u8UserMotID	= 1;

		//v3.64
		//CString szLog;
		//szLog.Format("gmp_mov (%d) -- ", nPos);
		//szLog = szLog + pAxisInfo->m_szName;
		//CMSLogFileUtility::Instance()->MS_LogNuMotionOperation(szLog);

		if ( (pAxisInfo->m_sErrCode = gmp_mov(pcChName, &stMovInput, &stProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_mov", pAxisInfo->m_szName);
			throw e;
		}

		if ( nMode == SFM_WAIT )
		{
			/*
			if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
				throw e;
			}
			*/
			MotionSync(szAxis, 20000, pAxisInfo);
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;

}


INT CMS896AStn::MotionMoveTo(const CString& szAxis, INT nPos, INT nMode, CMSNmAxisInfo* pAxisInfo, short sProcListSaveID)
{
#ifdef OFFLINE
	return TRUE;
#endif

	try
	{
#ifdef NU_MOTION
		GMP_MOV_INPUT stMovInput;
		GMP_PROF stProf;                               
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionMoveTo: " + szAxis);
			return FALSE;
		}

		//v4.50A24	//Osram Germany CEMark MS109
		if (IsCEMarkMotors(szAxis) && CheckCEMarkCoverSensors())
		{
			DisplayMessage("Motion aborted due to CEMark Door Sensors in MotionMoveTo: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionMoveTo");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);

		if (sProcListSaveID >= 0)
		{
			stPBL = pAxisInfo->m_astPBL[sProcListSaveID];
		}
		else
		{
			stPBL.u8PBLSize = 0;
		}

		stProf.enProfType						= THIRD_ORD_MOV_PROF_B;
		stProf.st3OrdMoveProfB.fFinalVel		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dFinalVel;
		stProf.st3OrdMoveProfB.fJerk			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dJerk;
		stProf.st3OrdMoveProfB.fMaxVel			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;
		stProf.st3OrdMoveProfB.fMaxAcc			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc;
		stProf.st3OrdMoveProfB.fMaxDec			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec;
		
		/*
		//v3.61
		INT nMotorDirection = 1;
		if (pAxisInfo->m_lDirection < 0)
		{
			nMotorDirection = -1;
		}
		*/

		stMovInput.enInputType					= GMP_3ORD_MOV_B_TYPE;
		stMovInput.pcCtrlType					= pcCtrlID;
		//stMovInput.st3OrdMovBInput.s32Dist	= (GMP_S32)nPos * nMotorDirection;	//v3.61
		stMovInput.st3OrdMovBInput.s32Dist		= (GMP_S32)nPos;
		stMovInput.st3OrdMovBInput.u8UserMotID	= 1;

		//v3.64
		CString szLog;
		szLog.Format("gmp_mov_to (%d) -- ", nPos);
		szLog = szLog + pAxisInfo->m_szName;
		CMSLogFileUtility::Instance()->MS_LogNuMotionOperation(szLog);

		if ( (pAxisInfo->m_sErrCode = gmp_mov_to(pcChName, &stMovInput, &stProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_mov_to", pAxisInfo->m_szName);
			throw e;
		}

		if ( nMode == SFM_WAIT )
		{
			/*
			if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
				throw e;
			}
			*/
			MotionSync(szAxis, 20000, pAxisInfo);
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->MoveTo(nPos, nMode);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));

			if (pStepper != NULL)
			{
				pStepper->MoveTo(nPos, nMode);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->MoveTo(nPos, nMode);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}

INT CMS896AStn::MotionObwMove(const CString& szAxis, INT nPos, INT nMode, CMSNmAxisInfo* pAxisInfo)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		GMP_MOV_INPUT stMovInput;
		GMP_PROF stProf;                               
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionObwMove: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionObwMove");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);

		stPBL.u8PBLSize = 0;

		//for Obw
		stProf.enProfType						= FIFTH_ORD_MOV_PROF_B;
		stProf.st5OrdMoveProfB.fFinalVel		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dFinalVel;
		stProf.st5OrdMoveProfB.fMaxVel			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;
		stProf.st5OrdMoveProfB.fMaxAcc			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc;
		stProf.st5OrdMoveProfB.fMaxDec			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec;
	
		/*
		//v3.61
		INT nMotorDirection = 1;
		if (pAxisInfo->m_lDirection < 0)
		{
			nMotorDirection = -1;
		}
		*/
		
		stMovInput.enInputType					= GMP_OBWMOV_B_TYPE;
		stMovInput.pcCtrlType					= pcCtrlID;
		//stMovInput.stObwMovBInput.s32Dist		= (GMP_S32)nPos * nMotorDirection;		//v3.61
		stMovInput.stObwMovBInput.s32Dist		= (GMP_S32)nPos;
		stMovInput.stObwMovBInput.u8UserMotID	= 1;

		CString szLog;
		//szLog.Format("gmp_mov (OBW) (%d) -- ", nPos);
		//szLog = szLog + pAxisInfo->m_szName;
		//CMSLogFileUtility::Instance()->MS_LogNuMotionOperation(szLog);

		/*szLog.Format("MotionObwMove %s (%s): Ctrl = %s, Profile = %f %f %f %f %f, Dist = %d", 
			szAxis,
			pAxisInfo->m_szName,
			pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dFinalVel,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dJerk,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc,
			pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec,
			nPos);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		*/
		

		if ( (pAxisInfo->m_sErrCode = gmp_mov(pcChName, &stMovInput, &stProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_mov", pAxisInfo->m_szName);
			throw e;
		}

		if ( nMode == SFM_WAIT )
		{
			/*
			if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
				throw e;
			}
			*/
			MotionSync(szAxis, 20000, pAxisInfo);
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));

			if (pServo != NULL)
			{
				pServo->ObwMove(nPos, nMode);
			}
		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));

			if (pStepper != NULL)
			{
				pStepper->ObwMove(nPos, nMode);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->ObwMove(nPos, nMode);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;

}

INT CMS896AStn::MotionObwMoveTo(const CString& szAxis, INT nPos, INT nMode, CMSNmAxisInfo* pAxisInfo, short sProcListSaveID)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		GMP_MOV_INPUT stMovInput;
		GMP_PROF stProf;                               
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionObwMoveTo: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionObwMoveTo");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);

		if (sProcListSaveID >= 0)
		{
			stPBL = pAxisInfo->m_astPBL[sProcListSaveID];
		}
		else
		{
			stPBL.u8PBLSize = 0;
		}

		stProf.enProfType						= FIFTH_ORD_MOV_PROF_B;
		stProf.st5OrdMoveProfB.fFinalVel		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dFinalVel;
		stProf.st5OrdMoveProfB.fMaxVel			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;
		stProf.st5OrdMoveProfB.fMaxAcc			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc;
		stProf.st5OrdMoveProfB.fMaxDec			= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec;
		
		/*
		//v3.61
		INT nMotorDirection = 1;
		if (pAxisInfo->m_lDirection < 0)
		{
			nMotorDirection = -1;
		}
		*/
		
		stMovInput.enInputType					= GMP_OBWMOV_B_TYPE;
		stMovInput.pcCtrlType					= pcCtrlID;
		//stMovInput.stObwMovBInput.s32Dist		= (GMP_S32)nPos * nMotorDirection;		//v3.61
		stMovInput.stObwMovBInput.s32Dist		= (GMP_S32)nPos;
		stMovInput.stObwMovBInput.u8UserMotID	= 1;

		//v3.64
		CString szLog;
		szLog.Format("gmp_mov_to (OBW) (%d) -- ", nPos);
		szLog = szLog + pAxisInfo->m_szName;
		CMSLogFileUtility::Instance()->MS_LogNuMotionOperation(szLog);

		if ( (pAxisInfo->m_sErrCode = gmp_mov_to(pcChName, &stMovInput, &stProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_mov_to", pAxisInfo->m_szName);
			throw e;
		}

		if ( nMode == SFM_WAIT )
		{
			/*
			if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
				throw e;
			}
			*/
			MotionSync(szAxis, 20000, pAxisInfo);
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->ObwMoveTo(nPos, nMode);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));

			if (pStepper != NULL)
			{
				pStepper->ObwMoveTo(nPos, nMode);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->ObwMoveTo(nPos, nMode);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}

INT CMS896AStn::MotionMoveToSearch(const CString& szAxis, INT nPos, INT nMode, CMSNmAxisInfo* pAxisInfo, USHORT usDebounce)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		
		GMP_MOV_INPUT stMoveInput;
		GMP_PROF stMoveProf;
		GMP_EVT_OBJECT stSrhEvent;
		GMP_PROCESS_BLK_LIST stMovePBL, stSrchPBL;
		GMP_U32 uiBitPos=0;
		GMP_U8 ucSrchDirection;
		GMP_FLOAT fSrcVel, fMaxVel;
		CString szPortID;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcMoveCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcSrchCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionMoveToSearch: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionMoveToSearch");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		// static control
//		if( szAxis==BH_AXIS_PROBER_Z && IsProber() )
//			strcpy_s(pcSrchCtrlID, sizeof(pcSrchCtrlID), pAxisInfo->m_stControl[PL_MOVESEARCH].m_szID);
//		else
			strcpy_s(pcSrchCtrlID, sizeof(pcSrchCtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);

		strcpy_s(pcMoveCtrlID, sizeof(pcMoveCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);
		strcpy_s(pcPortID, sizeof(pcPortID), pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_szPort);

		//Protect SearchVel should not be > Move MaxVel
		fSrcVel = (GMP_FLOAT)pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_dSpeed;
		fMaxVel = (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;
		if ( fSrcVel > fMaxVel )
		{
			fSrcVel = fMaxVel;
		}

		stSrchPBL.u8PBLSize	= 0;

		stSrhEvent.enDrvInType						= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrhEvent.enLmtType						= GMP_EVT_LMT_DIST; 
		stSrhEvent.bEnaErrChk						= GMP_FALSE;
		stSrhEvent.enType							= (GMP_EVT_TYPE)pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_lType;
		stSrhEvent.pcPort1							= pcPortID;
		stSrhEvent.u16SrhDebounceSmpl				= usDebounce;           
		stSrhEvent.fSrhDrvInVel						= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_dDrvInSpeed;
		stSrhEvent.u32DrvInCnt						= (GMP_U32)pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_ulDrvInDistance;
		stSrhEvent.fSrhVel							= fSrcVel;
		stSrhEvent.u32SrhLmt						= (GMP_U32)(abs(pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_lDistance));
		stSrhEvent.u32Mask							= (GMP_U32)pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_ulMask;
		stSrhEvent.u32ActiveState					= (GMP_U32)pAxisInfo->m_stSrchProfile[pAxisInfo->m_ucSrchProfileID].m_ulAState;

		if ( stSrhEvent.fSrhDrvInVel > stSrhEvent.fSrhVel )
		{
			stSrhEvent.fSrhDrvInVel	= stSrhEvent.fSrhVel;
		}

		if ( nPos < 0 )
		{
			ucSrchDirection = GMP_NEGATIVE_SRH_DIR;
		}
		else
		{
			ucSrchDirection = GMP_POSITIVE_SRH_DIR;
		}

		//Define MoveSrch values
		stMovePBL.u8PBLSize	= 0;

		if ( pAxisInfo->m_lProfileType == MS896A_OBW_PROFILE)
		{
			stMoveProf.enProfType							= FIFTH_ORD_MOV_PROF_B;
			stMoveProf.st5OrdMoveProfB.fFinalVel			= stSrhEvent.fSrhVel;	//Final Velocity should equal to search speed
			stMoveProf.st5OrdMoveProfB.fMaxAcc				= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc;
			stMoveProf.st5OrdMoveProfB.fMaxDec				= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec;
			stMoveProf.st5OrdMoveProfB.fMaxVel				= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;

			stMoveInput.enInputType							= GMP_OBWMOVSRH_TYPE;
			stMoveInput.pcCtrlType							= pcMoveCtrlID;
			stMoveInput.stObwMovSrhInput.pcSrhCtrlType		= pcSrchCtrlID;
			stMoveInput.stObwMovSrhInput.bEnaSetPosn		= GMP_FALSE;
			stMoveInput.stObwMovSrhInput.s32DesiredAbsPosn	= 0;
			stMoveInput.stObwMovSrhInput.s32Dist			= (GMP_S32)nPos;
			stMoveInput.stObwMovSrhInput.u8UserMotID		= 1;
			stMoveInput.stObwMovSrhInput.u8SrhDir			= ucSrchDirection;
		}
		else
		{
			stMoveProf.enProfType							= THIRD_ORD_MOV_PROF_B;
			stMoveProf.st3OrdMoveProfB.fFinalVel			= stSrhEvent.fSrhVel;	//Final Velocity should equal to search speed
			stMoveProf.st3OrdMoveProfB.fJerk				= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dJerk;
			stMoveProf.st3OrdMoveProfB.fMaxVel				= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;
			stMoveProf.st3OrdMoveProfB.fMaxAcc				= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc;
			stMoveProf.st3OrdMoveProfB.fMaxDec				= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec;

			stMoveInput.enInputType							= GMP_3ORD_MOVSRH_TYPE;
			stMoveInput.pcCtrlType							= pcMoveCtrlID;
			stMoveInput.st3OrdMovSrhInput.pcSrhCtrlType		= pcSrchCtrlID;
			stMoveInput.st3OrdMovSrhInput.bEnaSetPosn		= GMP_FALSE;
			stMoveInput.st3OrdMovSrhInput.s32DesiredAbsPosn	= 0;
			stMoveInput.st3OrdMovSrhInput.s32Dist			= (GMP_S32)nPos;
			stMoveInput.st3OrdMovSrhInput.u8UserMotID		= 1;
			stMoveInput.st3OrdMovSrhInput.u8SrhDir			= ucSrchDirection;
		}
		
		if ( (pAxisInfo->m_sErrCode = gmp_mov_to_srh(pcChName, &stMoveInput, &stMoveProf, &stSrhEvent, &stMovePBL, &stSrchPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_mov_to_srh", pAxisInfo->m_szName);
			throw e;
		}

		if ( nMode == SFM_WAIT )
		{
			MotionSync(szAxis, 20000, pAxisInfo);
		}


#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}

INT CMS896AStn::MotionSearch(const CString& szAxis, INT nDirection, BOOL fWait, CMSNmAxisInfo* pAxisInfo, const CString& szProfileID, BOOL bDirectionReverse, BOOL bEnaErrChk, BOOL bActiveStateReverse)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		BOOL bProfile = FALSE;
		LONG lSrchID;

		for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
			{
				bProfile = TRUE;
				lSrchID = i;
				break;
			}
		}

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSearch: " + szAxis);
			return FALSE;
		}

		//v4.50A24	//Osram Germany CEMark MS109
		if (IsCEMarkMotors(szAxis) && CheckCEMarkCoverSensors())
		{
			DisplayMessage("Motion aborted due to CEMark Door Sensors in MotionMoveHome2: " + szAxis);
			return FALSE;
		}

		if ( bProfile == FALSE )
		{
			CAsmException e((UINT)0, "Search", "No such Profile: " + szProfileID);
			throw e;
		}
		
		GMP_MOV_INPUT stSrchInput;
		GMP_EVT_OBJECT stSrhEvent;
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionSearch");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);
		strcpy_s(pcPortID, sizeof(pcPortID), pAxisInfo->m_stSrchProfile[lSrchID].m_szPort);

		stPBL.u8PBLSize								= 0;

		stSrchInput.enInputType						= GMP_SEARCH_TYPE;
		stSrchInput.pcCtrlType						= pcCtrlID;

		stSrchInput.stSrhInput.bEnaSetPosn			= GMP_FALSE;
		stSrchInput.stSrhInput.s32DesiredAbsPosn	= 0;
		stSrchInput.stSrhInput.u8UserMotID			= 1;
		stSrchInput.stSrhInput.u8SrhDir				= GMP_POSITIVE_SRH_DIR;

		if (pAxisInfo->m_stSrchProfile[lSrchID].m_lDistance < 0)
		{
			stSrchInput.stSrhInput.u8SrhDir			= GMP_NEGATIVE_SRH_DIR;
		}
		if (bDirectionReverse)
		{
			stSrchInput.stSrhInput.u8SrhDir = (stSrchInput.stSrhInput.u8SrhDir == GMP_POSITIVE_SRH_DIR) ? GMP_NEGATIVE_SRH_DIR : GMP_POSITIVE_SRH_DIR;
		}

		//Search event (Home, Index & limit no need to use Port Value)
		stSrhEvent.enDrvInType						= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrhEvent.enLmtType						= GMP_EVT_LMT_DIST; 
		stSrhEvent.bEnaErrChk						= bEnaErrChk ? GMP_TRUE : GMP_FALSE;
		stSrhEvent.enType							= (GMP_EVT_TYPE)pAxisInfo->m_stSrchProfile[lSrchID].m_lType;
		stSrhEvent.pcPort1							= pcPortID;
		stSrhEvent.u16SrhDebounceSmpl				= 0;           
		stSrhEvent.fSrhDrvInVel						= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[lSrchID].m_dDrvInSpeed;
		stSrhEvent.u32DrvInCnt						= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulDrvInDistance;
		stSrhEvent.fSrhVel							= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[lSrchID].m_dSpeed;
		stSrhEvent.u32SrhLmt						= (GMP_U32)(abs(pAxisInfo->m_stSrchProfile[lSrchID].m_lDistance));
		stSrhEvent.u32Mask							= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulMask;
		stSrhEvent.u32ActiveState					= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulAState;

		if (bActiveStateReverse)
		{
			if (stSrhEvent.u32ActiveState == 0 )
			{
				stSrhEvent.u32ActiveState = 1;
			}
			else
			{
				stSrhEvent.u32ActiveState = 0;
			}
		}

		if (bDirectionReverse)
		{
			if (stSrhEvent.u32ActiveState == 0)
			{
				stSrhEvent.u32ActiveState = 1;
			}
			else
			{
				stSrhEvent.u32ActiveState = 0;
			}
		}

		//CString szTemp;
		//szTemp.Format("%d", stSrhEvent.u32ActiveState);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		if ( stSrhEvent.fSrhDrvInVel > stSrhEvent.fSrhVel )
		{
			stSrhEvent.fSrhDrvInVel	= stSrhEvent.fSrhVel;
		}

		if ( (pAxisInfo->m_sErrCode = gmp_srh(pcChName, &stSrchInput, &stSrhEvent, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_srh", pAxisInfo->m_szName);
			throw e;
		}

		if ( fWait == SFM_WAIT )
		{
			/*
			if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
				throw e;
			}
			*/
			MotionSync(szAxis, 50000, pAxisInfo);
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->Search(nDirection, fWait);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));

			if (pStepper != NULL)
			{
				pStepper->Search(nDirection, fWait);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->Search(nDirection, fWait);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}


INT CMS896AStn::MotionSearchInRange(const CString& szAxis, INT nDirection, BOOL fWait, CMSNmAxisInfo* pAxisInfo, const CString& szProfileID)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		BOOL bProfile = FALSE;
		LONG lSrchID;

		for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
			{
				bProfile = TRUE;
				lSrchID = i;
				break;
			}
		}

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSearch: " + szAxis);
			return FALSE;
		}

		if ( bProfile == FALSE )
		{
			CAsmException e((UINT)0, "Search", "No such Profile: " + szProfileID);
			throw e;
		}
		
		GMP_MOV_INPUT stSrchInput;
		GMP_EVT_OBJECT stSrhEvent;
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionSearch");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);
		strcpy_s(pcPortID, sizeof(pcPortID), pAxisInfo->m_stSrchProfile[lSrchID].m_szPort);

		stPBL.u8PBLSize								= 0;

		stSrchInput.enInputType						= GMP_SEARCH_TYPE;
		stSrchInput.pcCtrlType						= pcCtrlID;

		stSrchInput.stSrhInput.bEnaSetPosn			= GMP_FALSE;
		stSrchInput.stSrhInput.s32DesiredAbsPosn	= 0;
		stSrchInput.stSrhInput.u8UserMotID			= 1;
		stSrchInput.stSrhInput.u8SrhDir				= GMP_POSITIVE_SRH_DIR;

		if ( pAxisInfo->m_stSrchProfile[lSrchID].m_lDistance < 0 )
		{
			stSrchInput.stSrhInput.u8SrhDir			= GMP_NEGATIVE_SRH_DIR;
		}

		//Search event (Home, Index & limit no need to use Port Value)
		stSrhEvent.enDrvInType						= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrhEvent.enLmtType						= GMP_EVT_LMT_DIST; 
		stSrhEvent.bEnaErrChk						= GMP_FALSE;
		stSrhEvent.enType							= (GMP_EVT_TYPE)pAxisInfo->m_stSrchProfile[lSrchID].m_lType;
		stSrhEvent.pcPort1							= pcPortID;
		stSrhEvent.u16SrhDebounceSmpl				= 0;           
		stSrhEvent.fSrhDrvInVel						= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[lSrchID].m_dDrvInSpeed;
		stSrhEvent.u32DrvInCnt						= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulDrvInDistance;
		stSrhEvent.fSrhVel							= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[lSrchID].m_dSpeed;
		stSrhEvent.u32SrhLmt						= (GMP_U32)(abs(pAxisInfo->m_stSrchProfile[lSrchID].m_lDistance));
		stSrhEvent.u32Mask							= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulMask;
		stSrhEvent.u32ActiveState					= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulAState;

		if ( stSrhEvent.fSrhDrvInVel > stSrhEvent.fSrhVel )
		{
			stSrhEvent.fSrhDrvInVel	= stSrhEvent.fSrhVel;
		}

		if ( (pAxisInfo->m_sErrCode = gmp_srh(pcChName, &stSrchInput, &stSrhEvent, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_srh", pAxisInfo->m_szName);
			throw e;
		}

		if ( fWait == SFM_WAIT )
		{
			/*
			if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
				throw e;
			}
			*/
			MotionSync(szAxis, 20000, pAxisInfo);
		}
#else
#endif
	}
	catch (CAsmException e) {
		DisplayException(e);
		throw e;
	}
	return TRUE;
}


//v3.94T3
INT CMS896AStn::MotionFastHomeSearch(const CString& szAxis, INT nDirection, BOOL fWait, CMSNmAxisInfo* pAxisInfo, const CString& szProfileID, const CString& szSrchProfileID)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		BOOL bSrchProfile	= FALSE;
		BOOL bProfile		= FALSE;
		LONG lSrchID, lMoveID;


		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSearch: " + szAxis);
			return FALSE;
		}

		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionSearch");
			throw e;
		}


		//Find Srch profile from Axis struct
		for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			if ( pAxisInfo->m_stSrchProfile[i].m_szID == szSrchProfileID )
			{
				bSrchProfile = TRUE;
				lSrchID = i;
				break;
			}
		}

		if ( bSrchProfile == FALSE )
		{
AfxMessageBox("No such Srch Profile: " + szSrchProfileID, MB_SYSTEMMODAL);
			CAsmException e((UINT)0, "Search", "No such Srch Profile: " + szSrchProfileID);
			throw e;
		}

		//FInd MOVE profile from Axis struct
		for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
			{
				bProfile = TRUE;
				lMoveID = i;
				break;
			}
		}

		if ( bProfile == FALSE )
		{
AfxMessageBox("No such MOVE Srch Profile: " + szProfileID, MB_SYSTEMMODAL);
			CAsmException e((UINT)0, "Search", "No such MOVE Srch Profile: " + szProfileID);
			throw e;
		}

		//Search motor HOME sensor bit from all available IN ports
		CString szPortID;
		GMP_U32 uiBitPos=0;
		for (LONG a=0; a<UM_MAX_SI_PORT; a++)
		{
			for (LONG b=0; b<UM_MAX_IO_BIT; b++)
			{
				if ( CMS896AApp::m_NmSIPort[a].m_szBit[b] == pAxisInfo->m_stSensor.m_szHome )
				{
					szPortID = CMS896AApp::m_NmSIPort[a].m_szName;
					uiBitPos = (GMP_U32)b;
					break;
				}
			}
		}			


		GMP_MOV_INPUT	stSrchInput;
		GMP_EVT_OBJECT	stSrhEvent;
		GMP_PROF		stProf;
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		//Copy ChName
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID,sizeof(pcCtrlID),  pAxisInfo->m_stControl[PL_STATIC].m_szID);
		//strcpy_s(pcPortID, sizeof(pcPortID), pAxisInfo->m_stSrchProfile[lSrchID].m_szPort);
		strcpy_s(pcPortID, sizeof(pcPortID), szPortID);
		stPBL.u8PBLSize								= 0;


		//Search MOVE Input
		stSrchInput.enInputType						= GMP_HISPDSRH_TYPE;	//Hi-speed search type
		stSrchInput.pcCtrlType						= pcCtrlID;
		stSrchInput.stHiSpdSrhInput.bEnaSetPosn			= GMP_TRUE;
		stSrchInput.stHiSpdSrhInput.s32DesiredAbsPosn	= 0;
		stSrchInput.stHiSpdSrhInput.u8UserMotID			= 0;
		stSrchInput.stHiSpdSrhInput.s32Dist				= pAxisInfo->m_stSrchProfile[lSrchID].m_lDistance;
		//stSrchInput.stHiSpdSrhInput.u8SrhDir			= GMP_POSITIVE_SRH_DIR;
		//if ( pAxisInfo->m_stSrchProfile[lSrchID].m_lDistance < 0 )
		//{
		//	stSrchInput.stSrhInput.u8SrhDir			= GMP_NEGATIVE_SRH_DIR;
		//}

		//Search event (Home, Index & limit no need to use Port Value)
		stSrhEvent.enDrvInType						= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrhEvent.enLmtType						= GMP_EVT_LMT_DIST; 
		stSrhEvent.bEnaErrChk						= GMP_FALSE;
		stSrhEvent.enType							= GMP_EVT_SRH_PORT_AND;	//(GMP_EVT_TYPE)pAxisInfo->m_stSrchProfile[lSrchID].m_lType;
		stSrhEvent.pcPort1							= pcPortID;
		stSrhEvent.u16SrhDebounceSmpl				= 0;           
		stSrhEvent.fSrhDrvInVel						= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[lSrchID].m_dDrvInSpeed;
		stSrhEvent.u32DrvInCnt						= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulDrvInDistance;
		stSrhEvent.fSrhVel							= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[lSrchID].m_dSpeed;
		stSrhEvent.u32SrhLmt						= (GMP_U32)(abs(pAxisInfo->m_stSrchProfile[lSrchID].m_lDistance));
		//stSrhEvent.u32Mask						= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulMask;
		//stSrhEvent.u32ActiveState					= (GMP_U32)pAxisInfo->m_stSrchProfile[lSrchID].m_ulAState;
		stSrhEvent.u32Mask							= 1<<uiBitPos;
		stSrhEvent.u32ActiveState					= 1<<uiBitPos;

		if ( stSrhEvent.fSrhDrvInVel > stSrhEvent.fSrhVel )
		{
			stSrhEvent.fSrhDrvInVel	= stSrhEvent.fSrhVel;
		}


		//MOVE Profile
		stProf.enProfType					= THIRD_ORD_MOV_PROF_B;
		stProf.st3OrdMoveProfB.fFinalVel	= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lMoveID].m_dFinalVel;
		stProf.st3OrdMoveProfB.fJerk		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lMoveID].m_dJerk;
		stProf.st3OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lMoveID].m_dMaxAcc;
		stProf.st3OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lMoveID].m_dMaxDec;
		stProf.st3OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lMoveID].m_dMaxVel;


		if ( (pAxisInfo->m_sErrCode = gmp_hi_speed_srh(pcChName, &stSrchInput, &stProf, &stSrhEvent, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_hi_speed_srh", pAxisInfo->m_szName);
			throw e;
		}

		if ( fWait == SFM_WAIT )
		{
			/*
			if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 20000)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
				throw e;
			}
			*/
			MotionSync(szAxis, 20000, pAxisInfo);
		}

#else
#endif

	}
	catch (CAsmException e) {
		DisplayException(e);
		throw e;
	}
	return TRUE;
}


INT CMS896AStn::MotionStop(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionStop: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionStop");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_stop(pcChName, 40, GMP_RAMP_STOP)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_stop", pAxisInfo->m_szName);
			throw e;
		}
		
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				//pServo->Search(nDirection, fWait);
				pServo->Stop();
				pServo->Synchronize(10000);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));

			if (pStepper != NULL)
			{
				//pStepper->Search(nDirection, fWait);
				pStepper->Stop();
				pStepper->Synchronize(10000);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->Stop();
				pDcServo->Synchronize(10000);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}

INT CMS896AStn::MotionQuickStop(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
#ifdef OFFLINE
	return TRUE;
#endif
	try
	{
#ifdef NU_MOTION
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionQuickStop: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionQuickStop");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_stop(pcChName, GMP_QUICK_STOP)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_stop", pAxisInfo->m_szName);
			throw e;
		}
		
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				//pServo->Search(nDirection, fWait);
				pServo->Stop();
				pServo->Synchronize(10000);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));

			if (pStepper != NULL)
			{
				//pStepper->Search(nDirection, fWait);
				pStepper->Stop();
				pStepper->Synchronize(10000);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->Stop();
				pDcServo->Synchronize(10000);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}

INT CMS896AStn::MotionSetPositionErrorLimit(const CString& szAxis, SHORT sPositionLimit, CMSNmAxisInfo* pAxisInfo)
{
	SHORT sResult = gnOK;

	try
	{
#ifdef NU_MOTION
		
		//No need to implement this function	//defined in SCF

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->SetPositionErrorLimit(sPositionLimit);
				return TRUE;
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->SetPositionErrorLimit(sPositionLimit);
				return TRUE;
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->SetPositionErrorLimit(sPositionLimit);
				return TRUE;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		sResult = gnNOTOK;
		DisplayException(e);
		throw e;
	}

	return sResult;
}

VOID CMS896AStn::MotionClearError(const CString& szAxis, LONG lProtectionType, CMSNmAxisInfo* pAxisInfo)
{
#ifdef OFFLINE
	return;
#endif
	try
	{
#ifdef NU_MOTION
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionClearError: " + szAxis);
			return;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionClearError");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_clear_ch_err(pcChName)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_clear_ch_err", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			//Reset error code
			pAxisInfo->m_sErrCode = 0;
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->ClearError(lProtectionType);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->ClearError(lProtectionType);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->ClearError(lProtectionType);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

}

INT CMS896AStn::MotionClearError(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
#ifdef OFFLINE
	return TRUE;
#endif
	INT nResult			= gnOK;

	try
	{
#ifdef NU_MOTION
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionClearError: " + szAxis);
			return gnNOTOK;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionClearError");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		if ( (pAxisInfo->m_sErrCode = gmp_clear_ch_err(pcChName)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_clear_ch_err", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			//Reset error code
			pAxisInfo->m_sErrCode = 0;
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{				
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			if (pServo != NULL)
			{
				hp_reset_driver(&pServo->m_HpCcb);			//v3.65
			}
			nResult = ClearServoError(GetActuatorName(szAxis));
		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			nResult = ClearStepperError(GetActuatorName(szAxis));
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	
			if (pDcServo != NULL)
			{
				hp_reset_driver(&pDcServo->m_HpCcb);		//v3.65
			}
			nResult = ClearDcServoError(GetActuatorName(szAxis));
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return nResult;
}

BOOL CMS896AStn::ClearServoError(const CString& szServo)
{
#ifdef OFFLINE
	return TRUE;
#endif
	BOOL bResult			= FALSE;
	SFM_CHipecAcServo*		pServo;
	CString					szReport;
	CString					szLog = szServo;

	
	pServo = GetHipecAcServo(szServo);
	LONG lReport = 0;

	if ((pServo != NULL) &&
		((lReport = pServo->ReportErrorStatus()) != 0))
	{
		szReport = pServo->GenerateErrorReport();
		TRACE0(szReport);
		DisplayMessage(szReport);

		if (lReport & HP_LIMIT_SENSOR)
		{
			pServo->ClearError(HP_LIMIT_SENSOR);
			szLog = szServo + _T(": clear HP_LIMIT_SENSOR");
			DisplayMessage(szLog);
		}

		if (lReport & 0x0002)
		{
			pServo->ClearError(0x0002);
			szLog = szServo + _T(": clear 0x0002");
			DisplayMessage(szLog);
		}

		if (lReport & HP_DAC_LIMIT)
		{
			pServo->ClearError(HP_DAC_LIMIT);
			szLog = szServo + _T(": clear HP_DAC_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_SW_POS_LIMIT)
		{
			pServo->ClearError(HP_SW_POS_LIMIT);
			szLog = szServo + _T(": clear HP_SW_POS_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_DRIVER_FAULT)
		{
			pServo->ClearError(HP_DRIVER_FAULT);
			szLog = szServo + _T(": clear HP_DRIVER_FAULT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_ENCODER_DISCONNECT)
		{
			pServo->ClearError(HP_ENCODER_DISCONNECT);
			szLog = szServo + _T(": clear HP_ENCODER_DISCONNECT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_ISOLATION_POWER_FAULT)
		{
			pServo->ClearError(HP_ISOLATION_POWER_FAULT);
			szLog = szServo + _T(": clear HP_ISOLATION_POWER_FAULT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_POS_ERROR_LIMIT)
		{
			pServo->ClearError(HP_POS_ERROR_LIMIT);
			szLog = szServo + _T(": clear HP_POS_ERROR_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_MOTION_ABORT)
		{
			pServo->ClearError(HP_MOTION_ABORT);
			szLog = szServo + _T(": clear HP_MOTION_ABORT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_MOTION_TIMEOUT)
		{
			pServo->ClearError(HP_MOTION_TIMEOUT);
			szLog = szServo + _T(": clear HP_MOTION_TIMEOUT");
			DisplayMessage(szLog);
		}

		bResult = TRUE;
	}
	
	return bResult;
}

BOOL CMS896AStn::ClearDcServoError(const CString& szServo)
{
	BOOL bResult			= FALSE;
	SFM_CHipecDcServo*		pServo;
	CString					szReport;
	CString					szLog = szServo;

	pServo = GetHipecDcServo(szServo);
	LONG lReport = 0;
#ifdef OFFLINE
	return TRUE;
#endif

	if ((pServo != NULL) &&
		((lReport = pServo->ReportErrorStatus()) != 0))
	{
		szReport = pServo->GenerateErrorReport();
		TRACE0(szReport);
		DisplayMessage(szReport);

		if (lReport & HP_LIMIT_SENSOR)
		{
			pServo->ClearError(HP_LIMIT_SENSOR);
			szLog = szServo + _T(": clear HP_LIMIT_SENSOR");
			DisplayMessage(szLog);
		}

		if (lReport & 0x0002)
		{
			pServo->ClearError(0x0002);
			szLog = szServo + _T(": clear 0x0002");
			DisplayMessage(szLog);
		}

		if (lReport & HP_DAC_LIMIT)
		{
			pServo->ClearError(HP_DAC_LIMIT);
			szLog = szServo + _T(": clear HP_DAC_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_SW_POS_LIMIT)
		{
			pServo->ClearError(HP_SW_POS_LIMIT);
			szLog = szServo + _T(": clear HP_SW_POS_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_DRIVER_FAULT)
		{
			pServo->ClearError(HP_DRIVER_FAULT);
			szLog = szServo + _T(": clear HP_DRIVER_FAULT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_ENCODER_DISCONNECT)
		{
			pServo->ClearError(HP_ENCODER_DISCONNECT);
			szLog = szServo + _T(": clear HP_ENCODER_DISCONNECT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_ISOLATION_POWER_FAULT)
		{
			pServo->ClearError(HP_ISOLATION_POWER_FAULT);
			szLog = szServo + _T(": clear HP_ISOLATION_POWER_FAULT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_POS_ERROR_LIMIT)
		{
			pServo->ClearError(HP_POS_ERROR_LIMIT);
			szLog = szServo + _T(": clear HP_POS_ERROR_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_MOTION_ABORT)
		{
			pServo->ClearError(HP_MOTION_ABORT);
			szLog = szServo + _T(": clear HP_MOTION_ABORT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_MOTION_TIMEOUT)
		{
			pServo->ClearError(HP_MOTION_TIMEOUT);
			szLog = szServo + _T(": clear HP_MOTION_TIMEOUT");
			DisplayMessage(szLog);
		}

		bResult = TRUE;
	}
	
	return bResult;
}

BOOL CMS896AStn::ClearStepperError(const CString& szStepper)
{
	BOOL bResult			= FALSE;
	SFM_CHipecStepper*		pStepper;
	CString					szReport;
	CString					szLog = szStepper;

	pStepper = GetHipecStepper(szStepper);
	LONG lReport = 0;
#ifdef OFFLINE
	return TRUE;
#endif

	if ((pStepper != NULL) &&
		((lReport = pStepper->ReportErrorStatus()) != 0))
	{
		szReport = pStepper->GenerateErrorReport();
		TRACE0(szReport);
		//DisplayMessage(szReport);

		if (lReport & HP_LIMIT_SENSOR)
		{
			pStepper->ClearError(HP_LIMIT_SENSOR);
			szLog = szStepper + _T(": clear HP_LIMIT_SENSOR");
			DisplayMessage(szLog);
		}

		if (lReport & 0x0002)
		{
			pStepper->ClearError(0x0002);
			szLog = szStepper + _T(": clear 0x0002");
			DisplayMessage(szLog);
		}

		if (lReport & HP_DAC_LIMIT)
		{
			pStepper->ClearError(HP_DAC_LIMIT);
			szLog = szStepper + _T(": clear HP_DAC_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_SW_POS_LIMIT)
		{
			pStepper->ClearError(HP_SW_POS_LIMIT);
			szLog = szStepper + _T(": clear HP_SW_POS_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_DRIVER_FAULT)
		{
			pStepper->ClearError(HP_DRIVER_FAULT);
			szLog = szStepper + _T(": clear HP_DRIVER_FAULT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_ENCODER_DISCONNECT)
		{
			pStepper->ClearError(HP_ENCODER_DISCONNECT);
			szLog = szStepper + _T(": clear HP_ENCODER_DISCONNECT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_ISOLATION_POWER_FAULT)
		{
			pStepper->ClearError(HP_ISOLATION_POWER_FAULT);
			szLog = szStepper + _T(": clear HP_ISOLATION_POWER_FAULT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_POS_ERROR_LIMIT)
		{
			pStepper->ClearError(HP_POS_ERROR_LIMIT);
			szLog = szStepper + _T(": clear HP_POS_ERROR_LIMIT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_MOTION_ABORT)
		{
			pStepper->ClearError(HP_MOTION_ABORT);
			szLog = szStepper + _T(": clear HP_MOTION_ABORT");
			DisplayMessage(szLog);
		}

		if (lReport & HP_MOTION_TIMEOUT)
		{
			pStepper->ClearError(HP_MOTION_TIMEOUT);
			szLog = szStepper + _T(": clear HP_MOTION_TIMEOUT");
			DisplayMessage(szLog);
		}

		bResult = TRUE;
	}
	
	return bResult;
}

LONG CMS896AStn::MotionReportErrorStatus(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	INT lResult			= gnOK;

	try
	{
#ifdef OFFLINE
		return gnOK;
#endif
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionReportErrorStatus: " + szAxis);
			return gnNOTOK;
		}

		GMP_U32 uiResult = 0;
		GMP_S16 sResult = 0;
		CString szPort;
		CHAR pcPort[GMP_PORT_NAME_CHAR_LEN];

		//szPort = pAxisInfo->m_szName + CMS896AApp::m_NmCHPort[MS896A_ERR_PORT].m_szName;
		szPort = pAxisInfo->m_szName + pAxisInfo->m_NmCHPort[MS896A_ERR_PORT].m_szName;
		strcpy_s(pcPort, sizeof(pcPort), szPort);

		if ( pAxisInfo->m_sErrCode == 0 )
		{
			if ( (sResult = gmp_read_io_port(pcPort, &uiResult)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_read_io_port", szPort);
				throw e;
			}

			if ( uiResult != 0 )
			{
#ifdef PROBER	
				pAxisInfo->m_sErrCode = uiResult;
#else				
				pAxisInfo->m_sErrCode = GMP_CHANNEL_ERROR;
#endif
			}
		}

		return (LONG)pAxisInfo->m_sErrCode;
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				lResult = pServo->ReportErrorStatus();
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				lResult = pStepper->ReportErrorStatus();
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				lResult = pDcServo->ReportErrorStatus();
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		lResult = gnNOTOK;
		DisplayException(e);
		throw e;
	}

	return lResult;
}


LONG CMS896AStn::ReportUserErrorStatus(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, long lStartErrorCode)
{
	long nErr = 0;

#ifdef OFFLINE
	return TRUE;
#endif
	LONG lReport = MotionReportErrorStatus(szAxis, pAxisInfo);

	if (lReport & GMP_DEPEND_CH_ERR)
	{
		nErr = lStartErrorCode + 6;
	}
	if (lReport & GMP_SRH_TIMEOUT)
	{
		nErr = lStartErrorCode;
	}
	if (lReport & GMP_EXCEED_SW_LMT)
	{
		nErr = lStartErrorCode + 1;
	}
	if (lReport & GMP_HIT_LMT_SNR)
	{
		nErr = lStartErrorCode + 2;
	}
	if (lReport & GMP_ENCODER_FAULT)
	{
		nErr = lStartErrorCode + 3;
	}

	if (lReport & GMP_DRV_FAULT)
	{
		nErr = lStartErrorCode + 4;
	}
	if (lReport & GMP_MOTION_ABORT)
	{
		nErr = lStartErrorCode + 5;
	}
	if (lReport & GMP_CTRL_ERR)
	{
		nErr = lStartErrorCode + 7;
	}
	if (lReport & GMP_BRAKE_LOCKED)
	{
		nErr = lStartErrorCode + 8;
	}
	if (lReport & GMP_EMERGENCY_STOP_ERR)	
	{
		nErr = lStartErrorCode + 9;
	}
	if (lReport & GMP_HIT_PSW_LMT)
	{
		nErr = lStartErrorCode + 10;
	}
	return nErr;
}

SHORT CMS896AStn::MotionGetLastWarning(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	SHORT sResult = gnOK;

	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionGetLastWarning: " + szAxis);
			return gnNOTOK;
		}
		return (SHORT)pAxisInfo->m_sErrCode;
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				sResult = pServo->GetLastWarning();
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				sResult = pStepper->GetLastWarning();
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				sResult = pDcServo->GetLastWarning();
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		sResult = gnNOTOK;
		DisplayException(e);
		throw e;
	}

	return sResult;
}

INT CMS896AStn::MotionSetLastWarning(const CString& szAxis, SHORT sLastWarn, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult = gnOK;

	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetLastWarning: " + szAxis);
			return gnNOTOK;
		}
		pAxisInfo->m_sErrCode = (GMP_S16)sLastWarn;
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->SetLastWarning(sLastWarn);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->SetLastWarning(sLastWarn);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->SetLastWarning(sLastWarn);
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		nResult = gnNOTOK;
		DisplayException(e);
		throw e;
	}

	return nResult;

}

INT CMS896AStn::MotionEnableProtection(const CString& szAxis, LONG lProtectionType, BOOL fTriggerState, BOOL bEnable, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult = gnOK;
#ifdef OFFLINE
	return 0;
#endif
	try
	{
#ifdef NU_MOTION
		GMP_TRIGGER_STATE emTriggerState = GMP_TRIGGER_STATE_ACTIVE_HIGH;
		GMP_PROT_TYPE  enProtType;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionEnableProtection: " + szAxis);
			return gnNOTOK;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionEnableProtection");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		switch(lProtectionType)
		{
			case HP_LIMIT_SENSOR:
				enProtType = GMP_PROT_TYPE_LMT_SENSOR;
				if ( fTriggerState == FALSE )
				{
					emTriggerState = GMP_TRIGGER_STATE_ACTIVE_LOW;
				}
				break;

			case HP_DRIVER_FAULT:
				enProtType = GMP_PROT_TYPE_DRIVER_FAULT;
				//Force state to HIGH
				emTriggerState = GMP_TRIGGER_STATE_ACTIVE_HIGH;
				break;

			case HP_ENCODER_FAULT:
				enProtType = GMP_PROT_TYPE_ENCODER_FAULT;
				break;	

			default:
				enProtType = (GMP_PROT_TYPE)lProtectionType;
				break;
		}

		if ( (pAxisInfo->m_sErrCode = gmp_ch_prot(pcChName, enProtType, (GMP_U8)bEnable, emTriggerState)) != 0 )
		{
			CString szText;

			szText.Format(": %d, %d, %d", enProtType, bEnable, fTriggerState);
			szText = pAxisInfo->m_szName + szText;
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_ch_prot", szText);
			throw e;
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				if (bEnable)
				{
					pServo->EnableProtection(lProtectionType, fTriggerState);
				}
				else
				{
					pServo->DisableProtection(lProtectionType);
				}
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				if (bEnable)
				{
					pStepper->EnableProtection(lProtectionType, fTriggerState);
				}
				else
				{
					pStepper->DisableProtection(lProtectionType);
				}
			}
			
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				if (bEnable)
				{
					pDcServo->EnableProtection(lProtectionType, fTriggerState);
				}
				else
				{
					pDcServo->DisableProtection(lProtectionType);
				}
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		nResult = gnNOTOK;
		DisplayException(e);
		throw e;
	}

	return nResult;

}


BOOL CMS896AStn::MotionSetSoftwareLimit(const CString& szAxis, LONG lUpperLimit, LONG lLowerLimit, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult = gnOK;

#ifdef OFFLINE
	return gnOK;
#endif
	try
	{
#ifdef NU_MOTION
		
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetSoftwareLimit: " + szAxis);
			return gnNOTOK;
		}

		GMP_S32 s32UpperSWLmt;
		GMP_S32 s32LowerSWLmt;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionSetSoftwareLimit");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		// set SW Posn Limit
		s32UpperSWLmt = (GMP_S32)lUpperLimit;
		s32LowerSWLmt = (GMP_S32)lLowerLimit;

		if ( (pAxisInfo->m_sErrCode = gmp_set_sw_posn_lmt(pcChName, s32UpperSWLmt, s32LowerSWLmt)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_set_sw_posn_lmt", pAxisInfo->m_szName);
			throw e;
		}
#endif
	}
	catch (CAsmException e)
	{
		nResult = gnNOTOK;
		DisplayException(e);
		throw e;
	}

	return nResult;
}


INT CMS896AStn::CommutateServo(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult		= gnAMS_OK;

#ifdef OFFLINE
	return gnAMS_OK;
#endif
	if (m_fHardware)
	{
		try
		{
			if (MotionIsServo(szAxis, pAxisInfo) == FALSE)
			{
				return gnNOTOK;
			}

			Commutate(szAxis);
			Sleep(200);
		}
		catch (CAsmException e)
		{
			ClearServoError(GetActuatorName(szAxis));

			try
			{
				Commutate(szAxis);
				Sleep(200);
			}
			catch (CAsmException e)
			{
				nResult = gnNOTOK;
				DisplayException(e);
			}
		}
	}

	return nResult;
}


INT CMS896AStn::DirectionalCommutateServo(const CString& szServoChannel, 
										  INT nPole, INT nEncoderCnt, INT nDirection)
{
	INT nResult		= gnAMS_OK;

#ifdef OFFLINE
	return gnAMS_OK;
#endif
	if (m_fHardware)
	{
		SFM_CHipecAcServo*	pServo;
		LONG				lParam[3];

		lParam[0]	= nPole;
		lParam[1]	= nEncoderCnt;
		lParam[2]	= nDirection;

		try
		{
			/*
			if (MotorIsServo(szAxis) == FALSE)
			{
				return gnNOTOK;
			}
			*/

			pServo	= GetHipecAcServo(szServoChannel);

			if (pServo)
			{
				pServo->DirectionalCommutate(lParam);
				Sleep(200);
			}
		}
		catch (CAsmException e)
		{
			if (pServo)
			{
				try
				{
					pServo->ClearError(HP_LIMIT_SENSOR|
						HP_DRIVER_FAULT| HP_ENCODER_DISCONNECT|HP_ISOLATION_POWER_FAULT|
						HP_DAC_LIMIT|HP_SW_POS_LIMIT|HP_POS_ERROR_LIMIT);

					//Change direction to comm again
					if (lParam[2] == HP_NEGATIVE_DIR)
					{
						lParam[2] = HP_POSITIVE_DIR;
					}
					else
					{
						lParam[2] = HP_NEGATIVE_DIR;
					}

					pServo->DirectionalCommutate(lParam);
					Sleep(200);
				}
				catch (CAsmException e)
				{
					nResult = gnNOTOK;
					DisplayException(e);
				}
			}
			else
			{
				nResult = gnNOTOK;
			}
		}
	}

	return nResult;
}


INT CMS896AStn::OpenCommutateServo(const CString& szServoChannel,
							   INT nPole, INT nEncoderCnt,
							   INT nCurrentLimit, INT nTime,
							   INT nInitRamp, INT nRampTime,
							   INT nHoldTime,
							   INT nDacLimit, INT nDacTime,
							   INT nOrgAngle, INT nStepAngle)
{
	INT nResult		= gnAMS_OK;

#ifdef OFFLINE
	return gnAMS_OK;
#endif
	if (m_fHardware)
	{
		SFM_CHipecAcServo*			pServo;

		CString		szCommReport;
		LONG		lParam[9];
		lParam[0]	= nPole;
		lParam[1]	= nEncoderCnt;
		lParam[2]	= 1;
		lParam[3]	= nInitRamp;
		lParam[4]	= nRampTime;
		lParam[5]	= nHoldTime;
		lParam[6]	= nCurrentLimit;
		lParam[7]	= nOrgAngle;
		lParam[8]	= nStepAngle;


		try
		{
			/*
			if (MotorIsServo(szAxis) == FALSE)
			{
				return gnNOTOK;
			}
			*/

			pServo	= GetHipecAcServo(szServoChannel);

			if (pServo)
			{
				pServo->OpenCommutate(lParam, &szCommReport);
				Sleep(200);
			}
		}
		catch (CAsmException e)
		{
			if (pServo)
			{
				try
				{
					pServo->ClearError(HP_LIMIT_SENSOR|
						HP_DRIVER_FAULT| HP_ENCODER_DISCONNECT|HP_ISOLATION_POWER_FAULT|
						HP_DAC_LIMIT|HP_SW_POS_LIMIT|HP_POS_ERROR_LIMIT);
					pServo->OpenCommutate(lParam, &szCommReport);
					Sleep(200);
				}
				catch (CAsmException e)
				{
					nResult = gnNOTOK;
					DisplayException(e);
				}
			}
			else
			{
				nResult = gnNOTOK;
			}
		}

		if (pServo != NULL)		//Klocwork
			pServo->SetDacLimit(nDacLimit, nDacTime);
	}

	return nResult;
}

INT CMS896AStn::MotionCommutateServo(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult		= gnAMS_OK;

	if (MotionIsServo(szAxis, pAxisInfo) == FALSE)
	{
		return nResult;
	}

	if (m_fHardware)
	{
#ifdef OFFLINE
		return gnAMS_OK;
#endif
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionCommutateServo: " + szAxis);
			return gnNOTOK;
		}

		try
		{
			CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
			GMP_COMMUTATION_INPUT stCommInput;

			//Copy ChName
			if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
			{
				CAsmException e((UINT)0, "No such ch name", "MotionCommutateServo");
				throw e;
			}
			strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

			stCommInput.enCommOption								= GMP_COMMUTATION_OPTION_OPEN_STATIONARY;
			stCommInput.stOpenStationaryCommInput.dbAcceptance		= 0.3;
			stCommInput.stOpenStationaryCommInput.dbIncreamentDAC	= 1000;
			stCommInput.stOpenStationaryCommInput.dbInitialDAC		= 2000;
			stCommInput.stOpenStationaryCommInput.dbMaximumDAC		= 20000;
			stCommInput.stOpenStationaryCommInput.u32SlowDownFactor	= 2;

			if ( (pAxisInfo->m_sErrCode = gmp_init_commutation(pcChName, &stCommInput)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_init_commutation(Stat_I)", pAxisInfo->m_szName);
				throw e;
			}
		}
		catch (CAsmException e)
		{
			nResult = gnNOTOK;
			DisplayException(e);
		}
#else
		try
		{
			Commutate(szAxis);
			Sleep(200);
		}
		catch (CAsmException e)
		{
			ClearServoError(GetActuatorName(szAxis));

			try
			{
				Commutate(szAxis);
				Sleep(200);
			}
			catch (CAsmException e)
			{
				nResult = gnNOTOK;
				DisplayException(e);
			}
		}
#endif
	}

	return nResult;
}


INT CMS896AStn::MotionDirectionalCommutateServo(const CString& szAxis, 
												INT nPole, INT nEncoderCnt, INT nDirection, 
												CMSNmAxisInfo* pAxisInfo)
{
#ifdef OFFLINE
	return gnAMS_OK;
#endif
	INT nResult		= gnAMS_OK;

	if (MotionIsServo(szAxis, pAxisInfo) == FALSE)
	{
		return nResult;
	}

	if (m_fHardware)
	{
#ifdef NU_MOTION
		try
		{
			CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
			GMP_COMMUTATION_INPUT stCommInput;

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionDirectionalCommutateServo: " + szAxis);
				return gnNOTOK;
			}

			//Copy ChName
			if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
			{
				CAsmException e((UINT)0, "No such ch name", "MotionDirectionalCommutateServo");
				throw e;
			}
			strcpy_s(pcChName,sizeof(pcChName), pAxisInfo->m_szName);

			stCommInput.enCommOption				= GMP_COMMUTATION_OPTION_DIRECTIONAL;
			stCommInput.stDirCommInput.u8CommDir	= (GMP_U8)nDirection;

			if ( (pAxisInfo->m_sErrCode = gmp_init_commutation(pcChName, &stCommInput)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_init_commutation(Direct)", pAxisInfo->m_szName);
				throw e;
			}
		}
		catch (CAsmException e)
		{
			nResult = gnNOTOK;
			DisplayException(e);
		}
#else
		SFM_CHipecAcServo*	pServo;
		LONG				lParam[3];
		CString				szServoChannel;

		lParam[0]	= nPole;
		lParam[1]	= nEncoderCnt;
		lParam[2]	= nDirection;

		try
		{
			szServoChannel = GetActuatorName(szAxis);
			pServo	= GetHipecAcServo(szServoChannel);

			if (pServo)
			{
				pServo->DirectionalCommutate(lParam);
				Sleep(200);
			}
		}
		catch (CAsmException e)
		{
			if (pServo)
			{
				try
				{
					pServo->ClearError(HP_LIMIT_SENSOR|
						HP_DRIVER_FAULT| HP_ENCODER_DISCONNECT|HP_ISOLATION_POWER_FAULT|
						HP_DAC_LIMIT|HP_SW_POS_LIMIT|HP_POS_ERROR_LIMIT);

					//Change direction to comm again
					if (lParam[2] == HP_NEGATIVE_DIR)
					{
						lParam[2] = HP_POSITIVE_DIR;
					}
					else
					{
						lParam[2] = HP_NEGATIVE_DIR;
					}

					pServo->DirectionalCommutate(lParam);
					Sleep(200);
				}
				catch (CAsmException e)
				{
					nResult = gnNOTOK;
					DisplayException(e);
				}
			}
			else
			{
				nResult = gnNOTOK;
			}
		}
#endif
	}

	return nResult;
}


INT CMS896AStn::MotionOpenCommutateServo(const CString& szAxis,
							   CMSNmAxisInfo* pAxisInfo,
							   INT nPole, INT nEncoderCnt,
							   INT nCurrentLimit, INT nTime,
							   INT nInitRamp, INT nRampTime,
							   INT nHoldTime,
							   INT nDacLimit, INT nDacTime,
							   INT nOrgAngle, INT nStepAngle)
{
	INT nResult		= gnAMS_OK;

	if (MotionIsServo(szAxis, pAxisInfo) == FALSE)
	{
		return nResult;
	}

	if (m_fHardware)
	{
#ifdef NU_MOTION
		try
		{
			CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
			GMP_COMMUTATION_INPUT stCommInput;

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionOpenCommutateServo: " + szAxis);
				return gnNOTOK;
			}

			//Copy ChName
			if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
			{
				CAsmException e((UINT)0, "No such ch name", "MotionOpenCommutateServo");
				throw e;
			}
			strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

			stCommInput.enCommOption								= GMP_COMMUTATION_OPTION_OPEN_STATIONARY;
			stCommInput.stOpenStationaryCommInput.dbAcceptance		= 0.3;
			stCommInput.stOpenStationaryCommInput.dbIncreamentDAC	= 1000;
			stCommInput.stOpenStationaryCommInput.dbInitialDAC		= 2000;
			stCommInput.stOpenStationaryCommInput.dbMaximumDAC		= 20000;
			stCommInput.stOpenStationaryCommInput.u32SlowDownFactor	= 2;

			if ( (pAxisInfo->m_sErrCode = gmp_init_commutation(pcChName, &stCommInput)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_init_commutation(OpenStat)", pAxisInfo->m_szName);
				throw e;
			}
		}
		catch (CAsmException e)
		{
			nResult = gnNOTOK;
			DisplayException(e);
		}
#else
		SFM_CHipecAcServo*			pServo;
		CString						szServoChannel;

		CString		szCommReport;
		LONG		lParam[9];
		lParam[0]	= nPole;
		lParam[1]	= nEncoderCnt;
		lParam[2]	= 1;
		lParam[3]	= nInitRamp;
		lParam[4]	= nRampTime;
		lParam[5]	= nHoldTime;
		lParam[6]	= nCurrentLimit;
		lParam[7]	= nOrgAngle;
		lParam[8]	= nStepAngle;


		try
		{
			szServoChannel = GetActuatorName(szAxis);
			pServo	= GetHipecAcServo(szServoChannel);

			if (pServo)
			{
				pServo->OpenCommutate(lParam, &szCommReport);
				Sleep(200);
			}
		}
		catch (CAsmException e)
		{
			if (pServo)
			{
				try
				{
					pServo->ClearError(HP_LIMIT_SENSOR|
						HP_DRIVER_FAULT| HP_ENCODER_DISCONNECT|HP_ISOLATION_POWER_FAULT|
						HP_DAC_LIMIT|HP_SW_POS_LIMIT|HP_POS_ERROR_LIMIT);
					pServo->OpenCommutate(lParam, &szCommReport);
					Sleep(200);
				}
				catch (CAsmException e)
				{
					nResult = gnNOTOK;
					DisplayException(e);
				}
			}
			else
			{
				nResult = gnNOTOK;
			}
		}

		if (pServo != NULL)		//Klocwork
			pServo->SetDacLimit(nDacLimit, nDacTime);
#endif
	}

	return nResult;
}

INT CMS896AStn::MotionAbortMotion(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	try
	{

#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionAbortMotion: " + szAxis);
			return gnNOTOK;
		}

		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "AbortMotion");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
	
		if ( (pAxisInfo->m_sErrCode = gmp_abort(pcChName, GMP_QUICK_STOP)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_abort", pAxisInfo->m_szName);
			throw e;
		}
		
		/*
		if ( (pAxisInfo->m_sErrCode = gmp_stop(pcChName, GMP_QUICK_STOP)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_abort", pAxisInfo->m_szName);
			throw e;
		}
		*/

		
		/*
		if ( (pAxisInfo->m_sErrCode = gmp_abort(pcChName, 8 ,GMP_RAMP_STOP)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_abort", pAxisInfo->m_szName);
			throw e;
		}
		*/

#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}


INT CMS896AStn::MotionCommandPosSync(const CString& szAxis, LONG lCommandPos, ULONG ulTimeout, CMSNmAxisInfo* pAxisInfo)
{

	INT nResult			= gnOK;
	INT nTime			= 0;

#ifdef OFFLINE
	return 0;
#endif
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION
			
			GMP_S32	iPos;

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSync: " + szAxis);
				return gnNOTOK;
			}

			CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

			//Copy ChName
			if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
			{
				CAsmException e((UINT)0, "No such ch name", "AbortMotion");
				throw e;
			}

			strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

			while(1)
			{	
				if ( (pAxisInfo->m_sErrCode = gmp_rpt_cmd_posn(pcChName, &iPos)) != 0 )
				{
					CAsmException e((UINT)pAxisInfo->m_sErrCode, "MotionCommandPosSync", pAxisInfo->m_szName);
					throw e;
				}

				if (iPos == lCommandPos)
				{
					break;
				}

				nTime++;
				Sleep(1);
			
				if ( nTime >= (INT)ulTimeout )
				{
					CString szTemp;
					szTemp.Format("MotionCommandPosSync - Timeout Command Pos :%d", iPos);
					CAsmException e((UINT)pAxisInfo->m_sErrCode, szTemp, pAxisInfo->m_szName);
					throw e;
				}
			}
#endif
		}
		catch (CAsmException e)
		{
			nResult = gnNOTOK;
			DisplayException(e);
			throw e;
		}
	}

	return nResult;
}

//v4.59A45	//Finisar TX MC3 BT T encoder check hangup testing 
INT CMS896AStn::MotionSync(const CString& szAxis, ULONG ulTimeout, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult			= gnOK;
	INT nTime			= 0;

	BOOL bLog = FALSE;
	CString szLog;

#ifdef OFFLINE
	return gnOK;
#endif
	if (m_fHardware)
	{
		try
		{
			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSync: " + szAxis);
				return gnNOTOK;
			}

			while(1)
			{	
				if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 0)) == 0 )
				{
					break;
				}

				if (pAxisInfo->m_sErrCode == GMP_MOTION_COMPLETE_TIMEOUT)
				{
					nTime++;
					Sleep(1);

					if ( nTime >= (INT)ulTimeout )
					{
						CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
						throw e;
						break;
					}
				}
				else
				{
					CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
					throw e;

					return gnNOTOK;
				}
			}
		}
		catch (CAsmException e)
		{
			nResult = gnNOTOK;
		}
	}

	return nResult;
}

/*
INT CMS896AStn::MotionSync(const CString& szAxis, ULONG ulTimeout, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult			= gnOK;
	INT nTime			= 0;

	if (m_fHardware)
	{
		try
		{
			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSync: " + szAxis);
				return gnNOTOK;
			}

			//if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, (GMP_U32)ulTimeout)) != 0 )
			//{
			//	CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
			//	throw e;
			//}

			while(1)
			{	
				if ( (pAxisInfo->m_sErrCode = gmp_motion_complete(&pAxisInfo->m_stMotionID, 0)) == 0 )
				{
					break;
				}
				else
				{
					//if ( pAxisInfo->m_sErrCode == GMP_TIMEOUT )
					if (pAxisInfo->m_sErrCode == GMP_MOTION_COMPLETE_TIMEOUT)
					{
						nTime++;
						Sleep(1);
					}
					else if ( nTime >= (INT)ulTimeout )
					{
						CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
						throw e;
					}
					else
					{
						CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_motion_complete", pAxisInfo->m_szName);
						throw e;
					}
				}
			}
		}
		catch (CAsmException e)
		{
			nResult = gnNOTOK;
			DisplayException(e);
			throw e;
		}
	}

	return nResult;
}
*/

INT CMS896AStn::MotionWait(const CString &szAxis, LONG lWaitTime, CMSNmAxisInfo *pAxisInfo)
{
	INT nResult = gnOK;

#ifdef OFFLINE
	return gnOK;
#endif
#ifdef NU_MOTION
      try
      {
            GMP_MOTION_ID stMotID;
            GMP_PROCESS_BLK_LIST stPBL;
            GMP_PROF stWaitProf;
            GMP_MOV_INPUT stWaitInput;
            GMP_S16 s16RetVal = GMP_SUCCESS;
            CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
            CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

			strcpy(pcChName, pAxisInfo->m_szName);
			strcpy(pcCtrlID, pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);
            stPBL.u8PBLSize							= 0;
            stWaitProf.enProfType					= DELAY_PROF_B;
            stWaitProf.stDelayProf.u32DelayTime		= (GMP_U32)(lWaitTime * NU_MOTION_SAMPLE_RATE);
            stWaitInput.enInputType					= GMP_DELAY_B_TYPE;
            stWaitInput.pcCtrlType					= pcCtrlID;
            stWaitInput.stDelayBInput.u8UserMotID	= 1;

			if ((pAxisInfo->m_sErrCode = gmp_wait(pcChName, &stWaitInput, &stWaitProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0)
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_wait", pAxisInfo->m_szName);
				throw e;
			}

            MotionSync(szAxis, lWaitTime + 100, pAxisInfo);
      }
      catch (CAsmException e)
      {
		  DisplayException(e);
		  throw e;
      }
#else
	Sleep(lWaitTime);
#endif
	return nResult;
}

INT CMS896AStn::MotionControlWait(const CString &szAxis, LONG lWaitTime, UCHAR ucControlID, CMSNmAxisInfo *pAxisInfo)
{
	INT nResult = gnOK;

#ifdef NU_MOTION
      try
      {
            GMP_MOTION_ID stMotID;
            GMP_PROCESS_BLK_LIST stPBL;
            GMP_PROF stWaitProf;
            GMP_MOV_INPUT stWaitInput;
            GMP_S16 s16RetVal = GMP_SUCCESS;
            CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
            CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];

			strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
			strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[ucControlID].m_szID);
            stPBL.u8PBLSize							= 0;
            stWaitProf.enProfType					= DELAY_PROF_B;
            stWaitProf.stDelayProf.u32DelayTime		= (GMP_U32)(lWaitTime * NU_MOTION_SAMPLE_RATE);
            stWaitInput.enInputType					= GMP_DELAY_B_TYPE;
            stWaitInput.pcCtrlType					= pcCtrlID;
            stWaitInput.stDelayBInput.u8UserMotID	= 1;

			if ((pAxisInfo->m_sErrCode = gmp_wait(pcChName, &stWaitInput, &stWaitProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0)
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_wait", pAxisInfo->m_szName);
				throw e;
			}

            MotionSync(szAxis, lWaitTime + 100, pAxisInfo);
      }
      catch (CAsmException e)
      {
		  DisplayException(e);
		  throw e;
      }
#else
	Sleep(lWaitTime);
#endif
	return nResult;
}

INT CMS896AStn::SyncServo(SFM_CHipecAcServo* pServo, int nTimeOut)
{
	INT nResult			= gnOK;
	int nTime			= 0;

#ifdef OFFLINE
	return gnOK;
#endif
	if (pServo)
	{
		while (!pServo->IsComplete())
		{
			Sleep (1);

			if (++nTime > nTimeOut)
			{
				nResult	= gnNOTOK;
				break;
			}
		}
	}

	return nResult;
}

INT CMS896AStn::SyncDcServo(SFM_CHipecDcServo* pServo, int nTimeOut)
{
	INT nResult			= gnOK;
	int nTime			= 0;

#ifdef OFFLINE
	return gnOK;
#endif
	if (pServo)
	{
		while (!pServo->IsComplete())
		{
			Sleep (1);

			if (++nTime > nTimeOut)
			{
				nResult	= gnNOTOK;
				break;
			}
		}
	}

	return nResult;
}

INT CMS896AStn::SyncStepper(SFM_CHipecStepper* pStepper, int nTimeOut)
{
	INT nResult			= gnOK;
	int nTime			= 0;

#ifdef OFFLINE
	return gnOK;
#endif
	if (pStepper)
	{
		while (!pStepper->IsComplete())
		{
			Sleep (1);

			if (++nTime > nTimeOut)
			{
				nResult	= gnNOTOK;
				break;
			}
		}
	}

	return nResult;
}

BOOL CMS896AStn::MotionSetJoystickConfig(const CString& szModuleID, const CString& szJoystickID, 
										 const CString& szInputBit1ID, const CString& szInputBit2ID)
{
	try
	{
#ifdef NU_MOTION
		
		//No need to implement this function

#else
		if (CMS896AApp::m_lHardwareConfigNo == MS896A_DDBS_WITH_RING_MASTER)
		{	
			SFM_CHipecModule *pModule = m_pInitOperation->GetHipecModule(szModuleID);
			SFM_CHipecDigitalInput *pInputBit1 = m_pInitOperation->GetSiBit(szInputBit1ID);
			SFM_CHipecDigitalInput *pInputBit2 = m_pInitOperation->GetSiBit(szInputBit2ID);

			pModule->SetJoystickConfig(szJoystickID, pInputBit1, pInputBit2);
		}
		else
		{
			SFM_CHipecModule *pModule = m_pInitOperation->GetHipecModule(szModuleID);
			SFM_CHipecDigitalInput *pInputBit1 = m_pInitOperation->GetInputBit(szInputBit1ID);
			SFM_CHipecDigitalInput *pInputBit2 = m_pInitOperation->GetInputBit(szInputBit2ID);

			pModule->SetJoystickConfig(szJoystickID, pInputBit1, pInputBit2);
		}
#endif
	}
	catch (CAsmException e)
	{
		
		DisplayException(e);
		throw e;
	}

	return TRUE;
}


VOID CMS896AStn::MotionJoyStickOn(const CString& szAxis, BOOL bOn, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef OFFLINE
		return;
#endif
#ifdef NU_MOTION
		GMP_MOV_INPUT stMovInput;
		GMP_PROF stProf;
		GMP_PROCESS_BLK_LIST stPBL;
		GMP_JOYSTICK_MODE emMode = GMP_JOYSTICK_MODE_ENABLE;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];
		UCHAR ucCtrlID = PL_STATIC;

		//if ( pAxisInfo->m_stControl[AD899_JOYSTICK_CONTROL].szID.IsEmpty() == TRUE )
		//{
		//	ucCtrlID = AD899_STATIC_CONTROL;
		//}
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionJoyStickOn: " + szAxis);
			return;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionJoyStickOn");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[ucCtrlID].m_szID);
		strcpy_s(pcPortID, sizeof(pcPortID), pAxisInfo->m_stJoyConfig.m_szPort);

		stPBL.u8PBLSize								= 0;

		stProf.enProfType							= JOYSTICK_PROF;
		stProf.stJoystickProf.fLowVel				= (GMP_FLOAT)pAxisInfo->m_stJoyConfig.stProfile[pAxisInfo->m_stJoyConfig.m_ucProfileID].m_dLowVel;
		stProf.stJoystickProf.fMaxAcc				= (GMP_FLOAT)pAxisInfo->m_stJoyConfig.stProfile[pAxisInfo->m_stJoyConfig.m_ucProfileID].m_dMaxAcc;
		stProf.stJoystickProf.fMaxDec				= (GMP_FLOAT)pAxisInfo->m_stJoyConfig.stProfile[pAxisInfo->m_stJoyConfig.m_ucProfileID].m_dMaxDec;
		stProf.stJoystickProf.fMaxVel				= (GMP_FLOAT)pAxisInfo->m_stJoyConfig.stProfile[pAxisInfo->m_stJoyConfig.m_ucProfileID].m_dMaxVel;
		stProf.stJoystickProf.u16HoldTime			= 1;

		stMovInput.enInputType						= GMP_JOYSTICK_TYPE;
		stMovInput.pcCtrlType						= pcCtrlID;
		stMovInput.stJoystickInput.pcPort			= pcPortID;
		stMovInput.stJoystickInput.u8BitPos			= (GMP_U8)pAxisInfo->m_stJoyConfig.m_ulBitPos;
		stMovInput.stJoystickInput.u8UserMotID		= 1;
		stMovInput.stJoystickInput.s32LowerLimit	= (GMP_S32)pAxisInfo->m_stJoyConfig.m_lLowerLimit;
		stMovInput.stJoystickInput.s32UpperLimit	= (GMP_S32)pAxisInfo->m_stJoyConfig.m_lUpperLimit;
		
		if ( bOn == FALSE )
		{
			emMode = GMP_JOYSTICK_MODE_DISABLE;
		}
		
		if ( MotionIsPowerOn(szAxis, pAxisInfo) == TRUE )
		{
			if ( (pAxisInfo->m_sErrCode = gmp_setup_joystick(pcChName, emMode, &stMovInput, &stProf, &stPBL, &pAxisInfo->m_stMotionID)) != 0 )
			{
				if ( bOn == FALSE )
				{
					CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_setup_joystick(Off)", pAxisInfo->m_szName);
					throw e;
				}
				else
				{
					CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_setup_joystick(On)", pAxisInfo->m_szName);
					throw e;
				}
			}
		}

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				if (bOn)
				{
					pServo->JoystickOn();
				}
				else
				{
					pServo->JoystickOff();
				}
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				if (bOn)
				{
					pStepper->JoystickOn();
				}
				else
				{
					pStepper->JoystickOff();
				}
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				if (bOn)
				{
					pDcServo->JoystickOn();
				}
				else
				{
					pDcServo->JoystickOff();
				}	
			}
		}
#endif
		
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}
}


VOID CMS896AStn::MotionSetIncrementParam(const CString& szAxis, const CString& szIncrementParamID, 
									  LONG lVelocity, LONG lVelocityLow, LONG lAcceleration, LONG lDeceleration, 
									  CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION

		//No need to implement this function

#else
		// meanningless to call if pr joystick mode
		if (CMS896AStn::m_ulJoyStickMode == MS899_JS_MODE_PR)
		{
			return;
		}

		SFM_CIncrementParam cIncrement;
		cIncrement.Create(0, lVelocity, lVelocityLow, lAcceleration, lDeceleration, -65535, 65535, 1);

		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->SetIncrementParam(szIncrementParamID, cIncrement);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->SetIncrementParam(szIncrementParamID, cIncrement);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->SetIncrementParam(szIncrementParamID, cIncrement);	
			}
		}
#endif
	
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//throw e;
	}
}


VOID CMS896AStn::MotionSelectIncrementParam(const CString& szAxis, const CString& szIncrementParamID, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION
		BOOL bProfile = FALSE;
		
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSelectIncrementParam: " + szAxis);
			return;
		}

		for (LONG i=0; i<UM_MAX_JOYSTICK_PROFILE; i++)
		{
			//update joystick profile ID
			if ( pAxisInfo->m_stJoyConfig.stProfile[i].m_szID == szIncrementParamID )
			{
				bProfile = TRUE;
				pAxisInfo->m_stJoyConfig.m_ucProfileID = (UCHAR)i;
				break;
			}
		}

		if ( bProfile == FALSE )
		{
			CAsmException e((UINT)0, "MotionSelectIncrementParam", "No such Param: " + szIncrementParamID);
			throw e;
		}
#else
		// meanningless to call if pr joystick mode
		if (CMS896AStn::m_ulJoyStickMode == MS899_JS_MODE_PR)
		{
			return;
		}

		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->SelectIncrementParam(szIncrementParamID);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->SelectIncrementParam(szIncrementParamID);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->SelectIncrementParam(szIncrementParamID);	
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//throw e;
	}
}


VOID CMS896AStn::MotionSelectJoystickConfig(const CString& szAxis, const CString& szJoyStickConfigID, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION

		//No need to implement this function

#else
		// meanningless to call if pr joystick mode
		if (CMS896AStn::m_ulJoyStickMode == MS899_JS_MODE_PR)
		{
			return;
		}

		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->SelectJoystickConfig(szJoyStickConfigID);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->SelectJoystickConfig(szJoyStickConfigID);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->SelectJoystickConfig(szJoyStickConfigID);		
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//throw e;
	}
}


VOID CMS896AStn::MotionSelectJoystickFactor(const CString& szAxis, SHORT sScaleFactor, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION

		//No need to implement this function

#else
		// meanningless to call if pr joystick mode
		if (CMS896AStn::m_ulJoyStickMode == MS899_JS_MODE_PR)
		{
			return;
		}

		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->SelectJoystickFactor(sScaleFactor);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->SelectJoystickFactor(sScaleFactor);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->SelectJoystickFactor(sScaleFactor);		
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//throw e;
	}
}


VOID CMS896AStn::MotionSetJoystickPositionLimit(const CString& szAxis, LONG lLowLimit, LONG lHighLimit, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetJoystickPositionLimit: " + szAxis);
			return;
		}
		pAxisInfo->m_stJoyConfig.m_lLowerLimit = lLowLimit;
		pAxisInfo->m_stJoyConfig.m_lUpperLimit = lHighLimit;
#else
		// meanningless to call if pr joystick mode
		if (CMS896AStn::m_ulJoyStickMode == MS899_JS_MODE_PR)
		{
			return;
		}

		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				pServo->SetJoystickPositionLimit((INT)lLowLimit, (INT)lHighLimit);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				pStepper->SetJoystickPositionLimit((INT)lLowLimit, (INT)lHighLimit);
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				pDcServo->SetJoystickPositionLimit((INT)lLowLimit, (INT)lHighLimit);		
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//throw e;
	}
}

INT CMS896AStn::MotionGetInputBitType(const CString& szBitId)
{
	try
	{
		SFM_CHipecDigitalInput*	pInputBit;
		
		try
		{
			pInputBit = m_pInitOperation->GetSiBit(szBitId);

			if (pInputBit != NULL)
			{
				return MS899_SI_BIT;
			}
		}
		catch (CAsmException e)
		{
		
		}

		try
		{
			pInputBit = m_pInitOperation->GetInputBit(szBitId);

			if (pInputBit != NULL)
			{
				return MS899_INPUT_BIT;
			}
		}
		catch (CAsmException e)
		{
		
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	return FALSE;
}

INT CMS896AStn::MotionGetOutputBitType(const CString& szBitId)
{
	try
	{
		SFM_CHipecDigitalOutput*	pOutputBit;
		
		try
		{
			pOutputBit = m_pInitOperation->GetSoBit(szBitId);

			if (pOutputBit != NULL)
			{
				return MS899_SO_BIT;
			}
		}
		catch (CAsmException e)
		{
		
		}

		try
		{
			pOutputBit = m_pInitOperation->GetOutputBit(szBitId);

			if (pOutputBit != NULL)
			{
				return MS899_OUTPUT_BIT;
			}
		}
		catch (CAsmException e)
		{
		
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionReadInputBit(const CString& szBitId)
{
	try
	{
#ifdef OFFLINE
		return 0;
#endif
#ifdef NU_MOTION
		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				//Input Bit name (m_szBit) defined between app & machine only (not in SCF) 
				if ( CMS896AApp::m_NmSIPort[i].m_szBit[j] == szBitId )	
				{
					GMP_S16 sResult = 0;
					GMP_U32 uiResult = 0;
					CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

					//Port ID (m_szName) = "IOMODULE_IN_PORT_IN_PORTx" (specified in Machine; 
					//							must match with port name defined between machine & SCF)
					strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSIPort[i].m_szName);	

					if ( (sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0 )
					{
						CAsmException e((UINT)sResult, "gmp_read_io_port", CMS896AApp::m_NmSIPort[i].m_szName + ":" + szBitId);
						throw e;
					}
					else
					{
						if ( (uiResult & (GMP_U32)(1<<j)) > 0 )		//Read specific bit from retrieved I-PORT result
						{
							return TRUE;
						}
						return FALSE;
					}
				}
			}
		}
#else
		if (MotionGetInputBitType(szBitId) == MS899_SI_BIT)
		{
			SFM_CHipecDigitalInput*	pInputBit;

			pInputBit = m_pInitOperation->GetSiBit(szBitId);

			return pInputBit->IsHigh();
		}
		else if (MotionGetInputBitType(szBitId) == MS899_INPUT_BIT)
		{
			SFM_CHipecDigitalInput*	pInputBit;

			pInputBit = m_pInitOperation->GetInputBit(szBitId);

			return pInputBit->IsHigh();
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayMessage("SI-Bit ERROR: " + szBitId);
		DisplayException(e);
		return FALSE;
	}

	return FALSE;
}


UINT CMS896AStn::MotionReadInputADCPort(const CString szPortName)
{
	UINT nResult = 0;

	try
	{
#ifdef OFFLINE
		return 0;
#endif
#ifdef NU_MOTION

		GMP_S16 sResult = 0;
		GMP_U32 uiResult = 0;
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];
		strcpy_s(pcPortID, sizeof(pcPortID), szPortName);	

		if ( (sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0 )
		{
			CAsmException e((UINT)sResult, "gmp_read_io_port", szPortName);
			throw e;
		}
		else
		{
			nResult = (INT) uiResult;
		}

#endif
	}
	catch (CAsmException e)
	{
		DisplayMessage("SI-ADC ERROR: " + szPortName);
		DisplayException(e);
	}

	return nResult;
}


INT CMS896AStn::MotionSetOutputBit(const CString& szBitId, BOOL bSetState)
{
	CString szSolName;
	try
	{
#ifdef OFFLINE
		return 0;
#endif
#ifdef NU_MOTION
		BOOL bBitExist = FALSE;

		for (LONG i=0; i<UM_MAX_SO_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				//Output Bit name (m_szBit) defined between app & machine only (not in SCF) 
				szSolName = CMS896AApp::m_NmSOPort[i].m_szBit[j];
				if ( CMS896AApp::m_NmSOPort[i].m_szBit[j] == szBitId )
				{
					GMP_S16 sResult = 0;
					GMP_U32 uiMask = 1<<j;
					GMP_U32 uiPattern = 1<<j;
					CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

					//Port ID (m_szName) = "IOMODULE_IN_PORT_IN_PORTx" (specified in Machine; 
					//							must match with port name defined between machine & SCF)
					strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSOPort[i].m_szName);
					
					bBitExist = TRUE;
					if ( bSetState == TRUE )
					{
						uiPattern = 0;
					}

					if ( (sResult = gmp_write_io_port(pcPortID, (ULONG)uiPattern, (ULONG)uiMask, GMP_BY_PASS, GMP_KEEP)) != 0 )
					{
						CAsmException e((UINT)sResult, "gmp_write_io_port", CMS896AApp::m_NmSOPort[i].m_szName + ":" + szBitId);
						throw e;
					}
				}
			}
		}
#else
		if (MotionGetOutputBitType(szBitId) == MS899_SO_BIT)
		{
			SFM_CHipecDigitalOutput*	pOutputBit;

			pOutputBit = m_pInitOperation->GetSoBit(szBitId);

			if (bSetState)
			{
				pOutputBit->SetLow();
				return TRUE;
			}
			else 
			{
				pOutputBit->SetHigh();
				return TRUE;
			} 
		}
		else if (MotionGetOutputBitType(szBitId) == MS899_OUTPUT_BIT)
		{
			SFM_CHipecDigitalOutput*	pOutputBit;
			pOutputBit = m_pInitOperation->GetOutputBit(szBitId);

			if (bSetState)
			{
				pOutputBit->SetLow();
				return TRUE;
			}
			else 
			{
				pOutputBit->SetHigh();
				return TRUE;
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayMessage("SO-Bit ERROR: " + szBitId);
		DisplayException(e);
		return FALSE;
	}

	return FALSE;
}


BOOL CMS896AStn::MotionGetOutputBitInfo(const CString& szBitId, CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN], GMP_U32 &uiMask)
{
	CString szSolName;
	try
	{
		for (LONG i = 0; i < UM_MAX_SO_PORT; i++)
		{
			for (LONG j = 0; j < UM_MAX_IO_BIT; j++)
			{
				//Output Bit name (m_szBit) defined between app & machine only (not in SCF) 
				szSolName = CMS896AApp::m_NmSOPort[i].m_szBit[j];
				if ( CMS896AApp::m_NmSOPort[i].m_szBit[j] == szBitId )
				{
					uiMask = 1<<j;
					strcpy_s(pcPortID, GMP_PORT_NAME_CHAR_LEN, CMS896AApp::m_NmSOPort[i].m_szName);
					return TRUE;
				}
			}
		}
	}
	catch (CAsmException e)
	{
		DisplayMessage("SO-Bit ERROR: " + szBitId);
		DisplayException(e);
		return FALSE;
	}

	return FALSE;
}

INT CMS896AStn::MotionIsNegativeLimitHigh(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult = gnOK;

	try
	{
#ifdef OFFLINE
		return gnOK;
#endif
#ifdef NU_MOTION
		CString szSensor;
		szSensor = pAxisInfo->m_stSensor.m_szLLimt;
		
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsNegativeLimitHigh: " + szAxis);
			return gnNOTOK;
		}

		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( CMS896AApp::m_NmSIPort[i].m_szBit[j] == szSensor )
				{
					GMP_S16 sResult = 0;
					GMP_U32 uiResult = 0;
					CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];


					strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSIPort[i].m_szName);

					if ( (sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0 )
					{
						CAsmException e((UINT)sResult, "gmp_read_io_port", CMS896AApp::m_NmSIPort[i].m_szName + ":" + szSensor);
						throw e;
					}
					else
					{
						if ( (uiResult & (GMP_U32)(1<<j)) > 0 )
						{
							return TRUE;
						}
						return FALSE;
					}
				}
			}
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				return pServo->IsNegLimitSensorHigh();
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return pStepper->IsNegLimitSensorHigh();
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return pDcServo->IsNegLimitSensorHigh();
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		nResult = gnNOTOK;
		DisplayException(e);
		throw e;
		return TRUE;
	}

	return TRUE;
}

INT CMS896AStn::MotionIsPositiveLimitHigh(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult = gnOK;

#ifdef OFFLINE
	return gnOK;
#endif
	try
	{
#ifdef NU_MOTION
		CString szSensor;
		szSensor = pAxisInfo->m_stSensor.m_szULimt;

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsPositiveLimitHigh: " + szAxis);
			return gnNOTOK;
		}

		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( CMS896AApp::m_NmSIPort[i].m_szBit[j] == szSensor )
				{
					GMP_S16 sResult = 0;
					GMP_U32 uiResult = 0;
					CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];


					strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSIPort[i].m_szName);

					if ( (sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0 )
					{
						CAsmException e((UINT)sResult, "gmp_read_io_port", CMS896AApp::m_NmSIPort[i].m_szName + ":" + szSensor);
						throw e;
					}
					else
					{
						if ( (uiResult & (GMP_U32)(1<<j)) > 0 )
						{
							return TRUE;
						}
						return FALSE;
					}
				}
			}
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				return pServo->IsPosLimitSensorHigh();
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return pStepper->IsPosLimitSensorHigh();
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return pDcServo->IsPosLimitSensorHigh();
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return TRUE;
	}

	return TRUE;
}


BOOL CMS896AStn::MotionIsHomeSensorHigh(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef OFFLINE
		return 0;
#endif
#ifdef NU_MOTION
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionIsHomeSensorHigh: " + szAxis);
			return FALSE;
		}

		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( CMS896AApp::m_NmSIPort[i].m_szBit[j] == pAxisInfo->m_stSensor.m_szHome )
				{
					GMP_S16 sResult = 0;
					GMP_U32 uiResult = 0;
					CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];


					strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSIPort[i].m_szName);

					if ( (sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0 )
					{
						CAsmException e((UINT)sResult, "gmp_read_io_port", CMS896AApp::m_NmSIPort[i].m_szName + ":" + pAxisInfo->m_stSensor.m_szHome);
						throw e;
					}
					else
					{
						if ( (uiResult & (GMP_U32)(1<<j)) > 0 )
						{
							return TRUE;
						}
						return FALSE;
					}
				}
			}
		}
#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				return pServo->IsHomeSensorHigh();
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return pStepper->IsHomeSensorHigh();
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return pDcServo->IsHomeSensorHigh();
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return TRUE;
	}

	return TRUE;
}


VOID CMS896AStn::MotionGetSearchProfile(const CString& szAxis, const CString& szProfile, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
#ifdef OFFLINE
		return;
#endif
		try
		{
#ifdef NU_MOTION
			BOOL bProfile = FALSE;

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionGetSearchProfile: " + szAxis);
				return;
			}

			for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfile )
				{
					bProfile = TRUE;
					break;
				}
			}

			if ( bProfile == FALSE )
			{
				//return FALSE;
			}

#else
			GetSearchProfile(GetActuatorName(szAxis), szProfile);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

VOID CMS896AStn::MotionSelectProfile(const CString& szAxis, const CString &szProfileID, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSelectProfile: " + szAxis);
				return;
			}

			for (UCHAR i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				//Check whether input profile is exist or not
				if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
				{
					pAxisInfo->m_ucMoveProfileID = i;

CString szLog;
szLog.Format("%s profile ID selected = %d", szAxis, pAxisInfo->m_ucMoveProfileID);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
					return;
				}
			}

			CAsmException e((UINT)0, "MotionSelectProfile", "Invalid Profile: " + szProfileID);
			throw e;

		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

VOID CMS896AStn::MotionSelectObwProfile(const CString& szAxis, const CString &szProfileID, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSelectObwProfile: " + szAxis);
				return;
			}

			for (UCHAR i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				//Check whether input profile is exist or not
				if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
				{
					pAxisInfo->m_ucMoveProfileID = i;
					return;
				}
			}

			CAsmException e((UINT)0, "MotionSelectProfile", "Invalid Profile: " + szProfileID);
			throw e;

#else
			if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
			{
				SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
				
				if (pServo != NULL)
				{
					pServo->SelectObwProfile(szProfileID);
				}

			}
			else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
			{
				SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
				
				if (pStepper != NULL)
				{
					pStepper->SelectObwProfile(szProfileID);
				}
			}
			else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
			{
				SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

				if (pDcServo != NULL)
				{
					pDcServo->SelectObwProfile(szProfileID);
				}
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

VOID CMS896AStn::MotionSelectSearchProfile(const CString& szAxis, const CString& szProfileID, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION
			BOOL bProfile = FALSE;

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSelectSearchProfile: " + szAxis);
				return;
			}

			for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
				{
					bProfile = TRUE;
					pAxisInfo->m_ucSrchProfileID = (UCHAR)i;
				}
			}

			if ( bProfile == FALSE )
			{
				CAsmException e((UINT)0, "MotionSelectSearchProfile", "No such Profile: " + szProfileID);
				throw e;
			}

#else
			SelectSearchProfile(szAxis, szProfileID);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

BOOL CMS896AStn::MotionGetSearchProfileParameters(const CString& szAxis, CString szProfileID, DOUBLE& dSpeed, LONG& lDistance, DOUBLE& dDriveInSpeed,
												  LONG& lDriveInDistance, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			BOOL bProfile = FALSE;
			
			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionUpdateSearchProfile: " + szAxis);
				return FALSE;
			}

			for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
				{
					//dSpeed = m_stSrchProfile[i].
					dSpeed = pAxisInfo->m_stSrchProfile[i].m_dSpeed;
					lDistance = pAxisInfo->m_stSrchProfile[i].m_lDistance;
					dDriveInSpeed = pAxisInfo->m_stSrchProfile[i].m_dDrvInSpeed;
					lDriveInDistance = pAxisInfo->m_stSrchProfile[i].m_ulDrvInDistance;					
					return TRUE;
				}
			}
#endif

			return FALSE;
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
	return FALSE;
}

VOID CMS896AStn::MotionUpdateSearchProfile(const CString& szAxis, const CString& szProfileID, 
										DOUBLE dVelocity, LONG lDistance,
										CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			BOOL bProfile = FALSE;
			
			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionUpdateSearchProfile: " + szAxis);
				return;
			}

			for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
				{
					bProfile = TRUE;
					pAxisInfo->m_ucSrchProfileID = (UCHAR)i;
					//pAxisInfo->m_stSrchProfile[i].m_dSpeed = dVelocity;
					pAxisInfo->m_stSrchProfile[i].m_lDistance = lDistance;
				}
			}

			if ( bProfile == FALSE )
			{
				CAsmException e((UINT)0, "MotionUpdateSearchProfile", "No such Profile: " + szProfileID);
				throw e;
			}

#else
			SelectSearchProfile(szAxis, szProfileID);
			SetSearchVelocity(szAxis, szProfileID, (LONG)dVelocity);
			SetSearchLimit(szAxis, szProfileID, lDistance);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

VOID CMS896AStn::MotionUpdateSearchProfile(const CString& szAxis, const CString& szProfileID, 
										DOUBLE dVelocity, LONG lDistance, DOUBLE dDriveInSpeed, 
										ULONG ulDriveInDistance, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			BOOL bProfile = FALSE;
			
			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionUpdateSearchProfile: " + szAxis);
				return;
			}

			for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
				{
					bProfile = TRUE;
					pAxisInfo->m_ucSrchProfileID = (UCHAR)i;

					if (dVelocity != 0.0)
					{
						pAxisInfo->m_stSrchProfile[i].m_dSpeed = dVelocity;
					}
					
					if (lDistance != 0)
					{
						pAxisInfo->m_stSrchProfile[i].m_lDistance = lDistance;
					}

					if (ulDriveInDistance != 0)
					{
						pAxisInfo->m_stSrchProfile[i].m_ulDrvInDistance = ulDriveInDistance;
					}

					if (dDriveInSpeed != 0.0)
					{
						pAxisInfo->m_stSrchProfile[i].m_dDrvInSpeed = dDriveInSpeed;
					}
				}
			}

			if ( bProfile == FALSE )
			{
				CAsmException e((UINT)0, "MotionUpdateSearchProfile", "No such Profile: " + szProfileID);
				throw e;
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

VOID CMS896AStn::MotionUpdateMotionProfile(const CString& szAxis, const CString& szProfile, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			// don't know

#else
			SFM_CProfile*	pProf	= GetMotionProfile(szProfile);
			SetMotionProfile(szAxis, szProfile, *pProf);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}

}


BOOL CMS896AStn::MotionUpdateProfile(const CString& szAxis, const CString& szProfileID, 
							   DOUBLE dVmax, DOUBLE dAcc, DOUBLE dDec, DOUBLE dJerk, DOUBLE dFinalVel, 
							   CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION
		BOOL bProfile = FALSE;

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionUpdateProfile: " + szAxis);
			return FALSE;
		}

		for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
			{
				if ( dAcc > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dMaxAcc	= dAcc; 
				}

				if ( dDec > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dMaxDec	= dDec; 
				}

				if ( dVmax > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dMaxVel	= dVmax; 
				}

				if ( dJerk > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dJerk = dJerk; 
				}
				
				/*Not update final velocity
				if ( dFinalVel > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dFinalVel = dFinalVel;
				}
				*/
				bProfile = TRUE;
				break;
			}
		}

		if ( bProfile == FALSE )
		{
			CAsmException e((UINT)0, "MotionUpdateProfile", "No such Profile: " + szProfileID);
			throw e;
		}

#else
		SFM_CProfile *pProf = GetMotionProfile(szProfileID);

		if (pProf != NULL)
		{
			if ( dVmax >= 1  )
			{
				pProf->SetVelocity((LONG)dVmax);
			}
			
			if ( dAcc >= 1 )
			{
				pProf->SetAcceleration((LONG)dAcc);
			}
			
			if ( dDec >= 1 )
			{
				pProf->SetDeceleration((LONG)dDec);
			}

			if ( dJerk >= 1 )
			{
				pProf->SetJerk((LONG)dJerk);
			}

			if ( dFinalVel >= 1 )
			{
				pProf->SetFinalVelocity((LONG)dFinalVel);
			}

			SetMotionProfile(szAxis, szProfileID, *pProf);

			return TRUE;
		
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return FALSE;
	}

	return FALSE;
}

BOOL CMS896AStn::MotionUpdateObwProfile(const CString& szAxis, const CString& szProfileID, DOUBLE dVmax, 
								  DOUBLE dAmax, DOUBLE dDmax, DOUBLE dFinalVel, 
								  CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION
		BOOL bProfile = FALSE;

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionUpdateObwProfile: " + szAxis);
			return FALSE;
		}

		for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
		{
			if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
			{
				if ( dVmax > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dMaxVel	= dVmax; 
				}
				
				if ( dAmax > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dMaxAcc	= dAmax; 
				}

				if ( dDmax > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dMaxDec	= dDmax; 
				}
				
				/* Not update final velocity
				if ( dFinalVel > 0 )
				{
					pAxisInfo->m_stMoveProfile[i].m_dFinalVel = dFinalVel;
				}
				*/
				bProfile = TRUE;
				break;
			}
		}

		if ( bProfile == FALSE )
		{
			CAsmException e((UINT)0, "MotionUpdateObwProfile", "No such Profile: " + szProfileID);
			throw e;
		}

#else
		SFM_CObwProfile *pProf = GetObwProfile(szProfileID);

		if (pProf != NULL)
		{

			if ( dVmax >= 1 )
			{
				pProf->SetMaxVelocity((LONG)dVmax);
			}
			
			if ( dAmax >= 1 )
			{
				pProf->SetMaxAcceleration((LONG)dAmax);
			}
			
			if ( dDmax >= 1 )
			{
				pProf->SetMaxDeceleration((LONG)dDmax);
			}

			if ( dFinalVel >= 1 )
			{
				pProf->SetFinalVelocity((LONG)dFinalVel);
			}

			SetObwProfile(szAxis, szProfileID, pProf);
			return TRUE;
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return FALSE;
	}

	return FALSE;
}

LONG CMS896AStn::MotionGetSCFProfileTime(const CString& szAxis, const CString& szProfileID, LONG lMaxTravelDist, LONG lQueryDist, LONG lSamplingRate, 
									  CMSNmAxisInfo* pAxisInfo)
{
	if ( m_fHardware == FALSE )
	{
		return (LONG)20;
	}

	int iMaxDist, iQueryDist;

	iQueryDist = (int)abs(lQueryDist);
	iMaxDist = (int)abs(lMaxTravelDist);


#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION

	GMP_PROF stProf;
	GMP_DWORD uiTime;
	LONG lProfile=0;
        
	if (pAxisInfo == NULL)
	{
		DisplayMessage("NULL Axis Info in MotionGetProfileTime: " + szAxis);
		return FALSE;
	}

	CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
	CHAR pcProfileName[GMP_CH_NAME_CHAR_LEN];

	strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
	strcpy_s(pcProfileName, sizeof(pcProfileName), szProfileID);

	if ( (pAxisInfo->m_sErrCode = gmp_get_move_time_by_given_dist(pcChName, pcProfileName, (GMP_S32)iMaxDist, (GMP_S32)iQueryDist, &uiTime)) != 0 )
	{
		CString szText;
		szText.Format("%s:%s, %d, %d, %d", pAxisInfo->m_szName, szProfileID, iMaxDist, iQueryDist, uiTime/NU_MOTION_SAMPLE_RATE);

		CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_get_move_time_by_given_dist SCF profile", szText);
		throw e;
	}
	else
	{
		return (LONG)(uiTime/NU_MOTION_SAMPLE_RATE);
	}

#endif
}

LONG CMS896AStn::MotionGetProfileTime(const CString& szAxis, const CString& szProfileID, LONG lMaxTravelDist, LONG lQueryDist, LONG lSamplingRate, 
									  CMSNmAxisInfo* pAxisInfo)
{
	if ( m_fHardware == FALSE )
	{
		return (LONG)20;
	}

	int iMaxDist, iQueryDist;

	iQueryDist = (int)abs(lQueryDist);
	iMaxDist = (int)abs(lMaxTravelDist);


#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION

	GMP_PROF stProf;
	GMP_DWORD uiTime;
	LONG lProfile=0;
        
	if (pAxisInfo == NULL)
	{
		DisplayMessage("NULL Axis Info in MotionGetProfileTime: " + szAxis);
		return FALSE;
	}

	for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
	{
		if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
		{
			lProfile = i;
			break;
		}
	}

	stProf.enProfType					= THIRD_ORD_MOV_PROF_B;
	stProf.st3OrdMoveProfB.fFinalVel	= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dFinalVel;
	stProf.st3OrdMoveProfB.fJerk		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dJerk;
	stProf.st3OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxAcc;
	stProf.st3OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxDec;
	stProf.st3OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxVel;

	if ( (pAxisInfo->m_sErrCode = gmp_get_move_time_by_given_dist(&stProf, (GMP_S32)iMaxDist, (GMP_S32)iQueryDist, &uiTime)) != 0 )
	{
		CString szText;
		szText.Format("%s:%s, %d, %d, %d", pAxisInfo->m_szName, szProfileID, iMaxDist, iQueryDist, uiTime/NU_MOTION_SAMPLE_RATE);

		CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_get_move_time_by_given_dist", szText);
		throw e;
	}
	else
	{
		return (LONG)(uiTime/NU_MOTION_SAMPLE_RATE);
	}

#else
	/*
	SFM_CProfile *pProf = GetMotionProfile(szProfileID);

	if (pProf != NULL)
	{
		Hp_Move_profile stMoveProfile; 
		Hp_profile_shift_factor	stShiftProfile;
		int iTime;
		CString szText;

		stMoveProfile.acc				 = (short)pProf->GetAcceleration();
		stMoveProfile.dec				 = (short)pProf->GetDeceleration();
		stMoveProfile.vmax				 = (short)pProf->GetVelocity();
		stMoveProfile.jerk				 = (short)pProf->GetJerk();
		stMoveProfile.final_vel			 = (short)pProf->GetFinalVelocity();
		stShiftProfile.acc_shift_factor  = (short)pProf->GetAccShiftFactor();
		stShiftProfile.vel_shift_factor	 = (short)pProf->GetVelShiftFactor();
		stShiftProfile.jerk_shift_factor = (short)pProf->GetJerkShiftFactor();

		if ( hp_get_move_time_by_dist(&stMoveProfile, &stShiftProfile, iMaxDist, iQueryDist, &iTime) == 0 )
		{		
			return (LONG)(iTime/lSamplingRate);
		}
	}
	*/

	SFM_CProfile* pProf = GetMotionProfile(szProfileID);
	DOUBLE a_max, d_max, v_max, jerk;
	DOUBLE t1, t2, t3, t4, t5;
	DOUBLE dist,total;
	LONG lTime = 20;

	a_max = (DOUBLE) pProf->GetAcceleration() / 256.0;
	d_max = (DOUBLE)pProf->GetDeceleration() / 256.0;
	v_max = (DOUBLE)pProf->GetVelocity() / 64.0;
	jerk  = (DOUBLE)pProf->GetJerk()/ 8192.0;
	dist = fabs(lQueryDist);
	t1 = max((a_max/jerk), 1);
	t4 = max((d_max/jerk), 1);
	t2 = max((v_max/a_max) - t1, 0);
	t5 = max((v_max/d_max) - t4, 0);
	t3 = ((2*dist/v_max) - (2*t1) - t2 - (2*t4) - t5)/(DOUBLE)2;
	t3 = max(t3, 0);

	if (t3 == 0)
	{
		t2=(sqrt(t4*(t4+8*dist/(t1*jerk*(t1+t4))))-t4)/(DOUBLE)2 - t1;
		t2=max(t2, 0);
		t5=a_max*(t1+t2)/d_max - t4;
		t5=max(t5, 0);
	}

	total=(t1+t1+t2+t3+t4+t4+t5)*1000/2000;		//_sample_freq;
	lTime = (LONG)(total + 0.5);

	return lTime;

#endif

	return FALSE;
}

#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
LONG CMS896AStn::MotionGenerateFifthOrderProfileUsedForILC(const CString& stAxis, const CString& szProfileID, CTRL_ILC_CONFIG_STRUCT stILC,  
														   float fDistance, CMSNmAxisInfo* pAxisInfo)
{

	if ( m_fHardware == FALSE )
	{
		return 10;
	}


#ifdef OFFLINE
	return TRUE;
#endif
	LONG lProfile = 0;	//Klocwork	//v4.15T9
	CString szText;

	for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
	{
		if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
		{
			lProfile = i;
			break;
		}
	}

	float fFinalVel = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dFinalVel;
	float fMaxAcc = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxAcc;
	float fMaxDec = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxDec;
	float fMaxVel = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxVel;

	if ( (pAxisInfo->m_sErrCode = agmp_ctrl_ILC_5th_order_profile( stILC, fMaxAcc, fMaxDec, fMaxVel, fDistance, fFinalVel)) != 0 )
	{
		CAsmException e((UINT)pAxisInfo->m_sErrCode, "agmp_ctrl_ILC_5th_order_profile");
		throw e;
	}

	return 1;

}
#endif
#endif
#endif

LONG CMS896AStn::MotionGetFifthOrderProfileTime(const CString& stAxis, const CString& szProfileID, float flDistance, LONG lSamplingRate, 
										 CMSNmAxisInfo* pAxisInfo)
{
#ifdef ES101
	return 0;		//v4.25	//Klocwork
#elif PROBER
	return 0;
#else
#ifdef NU_MOTION
	if ( m_fHardware == FALSE )
	{
		return 10;
	}

	CString szText;
	GMP_DWORD uiTime;
	LONG lProfile=0;
        
	if (pAxisInfo == NULL)
	{
		DisplayMessage("NULL Axis Info in Get_Fifth_Order_Profile_totalsample: " + stAxis);
		return 10;
	}

	for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
	{
		if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
		{
			lProfile = i;
			break;
		}
	}

	float fFinalVel = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dFinalVel;
	float fMaxAcc = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxAcc;
	float fMaxDec = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxDec;
	float fMaxVel = (float)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxVel;

	uiTime = Get_Fifth_Order_Profile_totalsample (fMaxAcc, fMaxDec,  fMaxVel, flDistance, fFinalVel) ;

	return (LONG)(uiTime/NU_MOTION_SAMPLE_RATE);
#else
	return 0;
#endif
#endif
}

LONG CMS896AStn::MotionGetObwProfileTime(const CString& stAxis, const CString& szProfileID, LONG lMaxTravelDist, LONG lQueryDist, LONG lSamplingRate, 
										 CMSNmAxisInfo* pAxisInfo)
{
	if ( m_fHardware == FALSE )
	{
		return 10;
	}

	int iMaxDist, iQueryDist;

	iQueryDist = (int)abs(lQueryDist);
	iMaxDist = (int)abs(lMaxTravelDist);


#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION

	GMP_PROF stProf;
	GMP_DWORD uiTime;
	LONG lProfile=0;
        
	if (pAxisInfo == NULL)
	{
		DisplayMessage("NULL Axis Info in MotionGetObwProfileTime: " + stAxis);
		return 10;
	}

	for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
	{
		if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
		{
			lProfile = i;
			break;
		}
	}

	stProf.enProfType					= FIFTH_ORD_MOV_PROF_B;
	stProf.st5OrdMoveProfB.fFinalVel	= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dFinalVel;
	stProf.st5OrdMoveProfB.fMaxAcc		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxAcc;
	stProf.st5OrdMoveProfB.fMaxDec		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxDec;
	stProf.st5OrdMoveProfB.fMaxVel		= (GMP_FLOAT)pAxisInfo->m_stMoveProfile[lProfile].m_dMaxVel;

	if ( abs(lQueryDist) > abs(lMaxTravelDist) )
	{
		iMaxDist = (int)abs(lQueryDist);
	}

	if ( (pAxisInfo->m_sErrCode = gmp_get_move_time_by_given_dist(&stProf, (GMP_S32)iMaxDist, (GMP_S32)iQueryDist, &uiTime)) != 0 )
	{
		CString szText;
		szText.Format("%s:%s, %d, %d, %d", pAxisInfo->m_szName, szProfileID, iMaxDist, iQueryDist, uiTime/NU_MOTION_SAMPLE_RATE);

		CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_get_move_time_by_given_dist", szText);
		throw e;
	}
	else
	{
		return (LONG)(uiTime/NU_MOTION_SAMPLE_RATE);
	}

#else
	SFM_CObwProfile *pProf = GetObwProfile(szProfileID);

	if (pProf != NULL)
	{
		Hp_OBwMove_profile stMoveProfile; 
		Hp_obw_profile_shift_factor	stShiftProfile;
		int iTime;
		CString szText;

		stMoveProfile.amax				= (short)pProf->GetMaxAcceleration();
		stMoveProfile.dmax				= (short)pProf->GetMaxDeceleration();
		stMoveProfile.vmax				= (short)pProf->GetMaxVelocity();
		stMoveProfile.final_vel			= (short)pProf->GetFinalVelocity();
		stShiftProfile.acc_shift_factor = (short)pProf->GetAccShiftFactor();
		stShiftProfile.vel_shift_factor	= (short)pProf->GetVelShiftFactor();

		iMaxDist = max(iMaxDist, iQueryDist);

		if ( hp_get_obw_move_time_by_dist(&stMoveProfile, &stShiftProfile, iMaxDist, iQueryDist, &iTime) == 0 )
		{
/*
			if (szProfileID == "mpfEjectorObwDefault")
			{
				CString szLog;
				szLog.Format("%d, %d,    %d, %d", iMaxDist, iQueryDist, iTime, lSamplingRate);
				AfxMessageBox(szLog, MB_SYSTEMMODAL);
			}
*/
			if( lSamplingRate!=0 )	// divide by zero
				return (LONG)(iTime/lSamplingRate);	//Ultra Bond Head Channel Frequency also = 8K Hz
			else
				return 0;
		}
		else
		{
			//v3.60
			CString szLog;
			szLog = "Error calculate OBW Profile error: " + szProfileID;
			DisplayMessage(szLog);
		}
	}
	else
	{
		//v3.60
		CString szLog;
		szLog = "Error Get OBW Profile error: " + szProfileID;
		DisplayMessage(szLog);
		//AfxMessageBox(szLog, MB_SYSTEMMODAL);
	}
#endif

	return FALSE;
}

VOID CMS896AStn::MotionCalculateObwProfileData(const CString& stAxis, const CString& szProfileID, LONG lMotionTime, LONG lTravelDist, LONG lSamplingRate, DOUBLE &dAccMax, DOUBLE &dDecMax, 
											   CMSNmAxisInfo* pAxisInfo)
{
	DOUBLE dVmax;
	CString szText;
	DOUBLE dMaxAcc = 0;
	DOUBLE dMaxDec = 0;

#ifdef NU_MOTION
	
	if (pAxisInfo == NULL)
	{
		DisplayMessage("NULL Axis Info in MotionCalculateObwProfileData: " + stAxis);
		return;
	}

	for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
	{
		if ( pAxisInfo->m_stMoveProfile[i].m_szID == szProfileID )
		{
			if( lMotionTime!=0 )	// divide by zero
			{
				dVmax	= (DOUBLE)(2 * lTravelDist) / (DOUBLE)(lMotionTime * NU_MOTION_SAMPLE_RATE);
				dAccMax = (dVmax * PI) / (lMotionTime * NU_MOTION_SAMPLE_RATE);
				dDecMax = (dVmax * PI) / (lMotionTime * NU_MOTION_SAMPLE_RATE);
			}
			dMaxAcc = pAxisInfo->m_stMoveProfile[i].m_dMaxAcc;
			dMaxDec = pAxisInfo->m_stMoveProfile[i].m_dMaxDec;
			break;
		}
	}

#else

	SFM_CObwProfile *pProf = GetObwProfile(szProfileID);

	if( lMotionTime!=0 && lSamplingRate!=0 )	// divide by zero
	{
		dVmax	= (DOUBLE)(2 * lTravelDist) / (DOUBLE)(lMotionTime * lSamplingRate);
		dAccMax = (dVmax * PI * 256) / (lMotionTime * lSamplingRate);
		dDecMax = (dVmax * PI * 256) / (lMotionTime * lSamplingRate);
	}

	dMaxAcc = (DOUBLE)pProf->GetMaxAcceleration();
	dMaxDec = (DOUBLE)pProf->GetMaxDeceleration();

#endif

	if ( dAccMax > dMaxAcc )
	{
		dAccMax = dMaxAcc;
	}

	if ( dDecMax > dMaxDec )
	{
		dDecMax = dMaxDec;
	}

#ifndef NU_MOTION
	if ( dAccMax < 1 )
	{
		dAccMax = 1;
	}

	if ( dDecMax < 1 )
	{
		dDecMax = 1;
	}
#endif

}

DWORD CMS896AStn::MotionSelectControlType(const CString& szAxis, LONG lControlNo, LONG lControlType, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION
		
		//No need to implement this function

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				if ( lControlType == MS899_DYN_CTRL )
				{
					return pServo->SelectControlType((DWORD)lControlNo);
				}
				else
				{
					return pServo->SelectStaticControlType((DWORD)lControlNo);
				}
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				if ( lControlType == MS899_DYN_CTRL )
				{
					return pStepper->SelectControlType((DWORD)lControlNo);
				}
				else
				{
					return pStepper->SelectStaticControlType((DWORD)lControlNo);
				}
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				if ( lControlType == MS899_DYN_CTRL )
				{
					return pDcServo->SelectControlType((DWORD)lControlNo);
				}
				else
				{
					return pDcServo->SelectStaticControlType((DWORD)lControlNo);
				}
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return (DWORD)0;
	}

	return (DWORD)0;
}


CString CMS896AStn::MotionSelectControlParam(const CString& szAxis, const CString &szControlParaID, CMSNmAxisInfo* pAxisInfo
											 , const CString &szNuControlParaID)
{
	try
	{
#ifdef NU_MOTION

		BOOL bControl = FALSE;

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSelectControlParam: " + szAxis);
			return 0;
		}

		for (UCHAR i=0; i<UM_MAX_MOVE_CONTROL; i++)
		{
			if ( pAxisInfo->m_stControl[i].m_szID == szNuControlParaID )
			{
				bControl = TRUE;
				pAxisInfo->m_ucControlID = i;
				break;
			}
		}

		if ( bControl == FALSE )
		{
			//CAsmException e((UINT)0, "MotionSelectControlParam", pAxisInfo->m_szName + ":" + szControlParaID);
			//throw e;
		}

#else
		if (GetAxisType(szAxis) == MS899_SVO_MOTOR)	
		{
			SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));
			
			if (pServo != NULL)
			{
				return pServo->SelectControlParam(szControlParaID);
			}

		}
		else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
		{
			SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
			
			if (pStepper != NULL)
			{
				return pStepper->SelectControlParam(szControlParaID);	
			}
		}
		else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
		{
			SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	

			if (pDcServo != NULL)
			{
				return pDcServo->SelectControlParam(szControlParaID);	
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
		return 0;
	}

	return 0;
}

CString CMS896AStn::MotionSelectStaticControl(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const INT nControlID)
{
	if (m_fHardware)
	{
		try
		{

#ifdef OFFLINE
			return 0;
#endif
#ifdef NU_MOTION
			GMP_STATIC_INPUT stStaticInput;
			CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
			CHAR pcLv1CtrlID[GMP_CH_NAME_CHAR_LEN];

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSelectStaticControl: " + szAxis);
				return FALSE;
			}

			//Copy ChName
			if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
			{
				CAsmException e((UINT)0, "No such ch name", "MotionSelectStaticControl");
				throw e;
			}

			strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
			strcpy_s(pcLv1CtrlID, sizeof(pcLv1CtrlID), pAxisInfo->m_stControl[nControlID].m_szID);

			// setup the parameters
			stStaticInput.pcStaticCtrlType  = pcLv1CtrlID;

 			if ( (pAxisInfo->m_sErrCode = gmp_select_static_ctrl(pcChName, &stStaticInput)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_select_static_ctrl", pAxisInfo->m_szName);
				throw e;
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
			return 0;
		}
	}

	return 0;		
}

CString CMS896AStn::MotionSelectSettlingControl(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, const INT nControlID, const LONG lSettlingTime)
{
	if (m_fHardware)
	{
		try
		{
#ifdef OFFLINE
			return 0;
#endif
#ifdef NU_MOTION
			GMP_SETTLING_INPUT stSettlingInput;
			CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
			CHAR pcLv1CtrlID[GMP_CH_NAME_CHAR_LEN];
			CHAR pcLv2CtrlID[GMP_CH_NAME_CHAR_LEN];

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSelectSettlingControl: " + szAxis);
				return FALSE;
			}

			//Copy ChName
			if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
			{
				CAsmException e((UINT)0, "No such ch name", "MotionSelectSettlingControl");
				throw e;
			}
			strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
			strcpy_s(pcLv1CtrlID, sizeof(pcLv1CtrlID), pAxisInfo->m_stControl[nControlID].m_szID);
			strcpy_s(pcLv2CtrlID, sizeof(pcLv2CtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);

			// setup the parameters
			stSettlingInput.pcLv1SettlingCtrlType = pcLv1CtrlID;
			stSettlingInput.pcLv2SettlingCtrlType = pcLv2CtrlID;
			stSettlingInput.u16Lv1SettlingTime = (GMP_S16)lSettlingTime;
			stSettlingInput.u16Lv2SettlingTime = 0;    //settling = 0 to not use the 2nd level

 			if ( (pAxisInfo->m_sErrCode = gmp_select_settling_ctrl(pcChName, &stSettlingInput)) != 0 )
			{
				CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_select_settling_ctrl", pAxisInfo->m_szName);
				throw e;
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
			return 0;
		}
	}

	return 0;
}

BOOL CMS896AStn::MotionSetControlParam(const CString& szAxis, const CString &szControlParaID, CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION

		//No need to implement this function

#else
		CObject *pObject;
		CMapStringToOb *pMap = m_pInitOperation->GetControlParameterMap();
		
		if (pMap->Lookup(szControlParaID, pObject)) 
		{
			SetControlParam(szAxis, szControlParaID, *(SFM_CControlParam *)pObject);
		}
		else
		{
			return FALSE;
		}
#endif	
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}
	return TRUE;
}

BOOL CMS896AStn::MoitonAddControlParam(const CString &szModuleID, const CString& szAxis, const CString &szControlParaID, 
									   CMSNmAxisInfo* pAxisInfo)
{
	try
	{
#ifdef NU_MOTION

		//No need to implement this function

#else
		SFM_CHipecModule *pModule = m_pInitOperation->GetHipecModule(szModuleID);
		CMapStringToOb *pMap = m_pInitOperation->GetControlParameterMap();
		CObject *pObject;


		if (pMap->Lookup(szControlParaID, pObject)) 
		{
			pModule->AddControlParamDataBlock(*(SFM_CControlParam *)pObject, szControlParaID);
			SetControlParam(szAxis, szControlParaID, *(SFM_CControlParam *)pObject);
		}
		else
		{
			return FALSE;
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}
	return TRUE;
}


VOID CMS896AStn::UpdateSearchProfile(const CString& szAxis, const CString& szProfile, CMSNmAxisInfo* pAxisInfo)
{
	//Note: Critical section locking is performed by UpdateProfile();
//	CMapElement&		rElement  = (*m_psmfMachine)[m_szStationName][szAxis][gszMRK_SRCH_PROF][szProfile];

	if (m_fHardware)
	{
		try
		{
			SFM_CSearchProfile*	pProf	  = GetSearchProfile(GetActuatorName(szAxis), szProfile);
			/*	CMarkAxis*			pAxis	  = (CMarkAxis*) GetAxis(szAxis);

				pProf->SetSearchVelocity(rElement[gszMRK_SRCH_VEL]);
				pProf->SetDriveIn((LONG) ((DOUBLE) rElement[gszMRK_DRV_IN] * pAxis->GetConversion()));
				pProf->SetDebounce(rElement[gszMRK_DEBOUNCE]);
			*/
			
			SetSearchProfile(szAxis, szProfile, *pProf);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}

}

VOID CMS896AStn::UpdateMotionProfile(const CString& szAxis, const CString& szProfile, CMSNmAxisInfo* pAxisInfo)
{
	//Note: Critical section locking is performed by UpdateProfile();
//	CMapElement&	rElement	= (*m_psmfMachine)[m_szStationName][szAxis][gszMRK_MOT_PROF][szProfile];
	if (m_fHardware)
	{
		try
		{
			SFM_CProfile*	pProf		= GetMotionProfile(szProfile);
		/*	
			pProf->SetVelocity(rElement[gszMRK_VEL]);
			pProf->SetAcceleration(rElement[gszMRK_ACC]);
			pProf->SetDeceleration(rElement[gszMRK_DECEL]);
			pProf->SetJerk(rElement[gszMRK_JERK]);
			pProf->SetFinalVelocity(rElement[gszMRK_FINAL_VEL]);
		*/
			SetMotionProfile(szAxis, szProfile, *pProf);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}


VOID CMS896AStn::CalculateProfileTime(const CString& szProfileID, LONG lRel_Dist, LONG &lTime)
{
	if (m_fHardware == FALSE)
	{
		lTime = 20;
		return;
	}

	SFM_CProfile* pProf = GetMotionProfile(szProfileID);
	DOUBLE a_max, d_max, v_max, jerk;
	DOUBLE t1, t2, t3, t4, t5;
	DOUBLE dist,total;

	a_max = (DOUBLE) pProf->GetAcceleration() / 256.0;
	d_max = (DOUBLE)pProf->GetDeceleration() / 256.0;
	v_max = (DOUBLE)pProf->GetVelocity() / 64.0;
	jerk  = (DOUBLE)pProf->GetJerk()/ 8192.0;
	dist = fabs((double)lRel_Dist);
	t1 = max((a_max/jerk), 1);
	t4 = max((d_max/jerk), 1);
	t2 = max((v_max/a_max) - t1, 0);
	t5 = max((v_max/d_max) - t4, 0);
	t3 = ((2*dist/v_max) - (2*t1) - t2 - (2*t4) - t5)/(DOUBLE)2;
	t3 = max(t3, 0);

	if (t3 == 0)
	{
		t2=(sqrt(t4*(t4+8*dist/(t1*jerk*(t1+t4))))-t4)/(DOUBLE)2 - t1;
		t2=max(t2, 0);
		t5=a_max*(t1+t2)/d_max - t4;
		t5=max(t5, 0);
	}

	total=(t1+t1+t2+t3+t4+t4+t5)*1000/2000;		//_sample_freq;
	lTime = (LONG)(total + 0.5);
}

////////////////////////////////////////////////////////////////
// For Check HiPEC command return code
////////////////////////////////////////////////////////////////

BOOL CMS896AStn::MotionCheckResult(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	//Klocwork
	if (pAxisInfo == NULL)
	{
#ifdef NU_MOTION
		DisplayMessage("NULL Axis Info in MotionCheckResult: " + szAxis);
		return FALSE;
#else

#endif
	}


	if (m_fHardware)
	{
		SHORT		siResult = HP_SUCCESS;
		LONG		lReport = 0;
		CString		szError;
		BOOL bHaveError = FALSE;
		BOOL bShowAlarm = TRUE;
		UINT niCode = IDS_MS_MOTION_ERROR;
		//short ssDrvierState = 0;

#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionCheckResult: " + szAxis);
			return FALSE;
		}

		try
		{
			GMP_U32 uiResult = 0;
			GMP_S16 sResult = 0;
			CString szPort;
			CHAR pcPort[GMP_PORT_NAME_CHAR_LEN];

			niCode = IDS_MS_NU_MOTION_ERROR;
			//szPort = pAxisInfo->m_szName + CMS896AApp::m_NmCHPort[MS896A_ERR_PORT].m_szName;
			szPort = pAxisInfo->m_szName + pAxisInfo->m_NmCHPort[MS896A_ERR_PORT].m_szName;		//v0.02
			strcpy_s(pcPort, sizeof(pcPort), szPort);

			if ( pAxisInfo->m_sErrCode == 0 )
			{
				if ( (sResult = gmp_read_io_port(pcPort, &uiResult)) != 0 )
				{
					CAsmException e((UINT)sResult, "gmp_read_io_port", szPort);
					throw e;
				}

				if ( uiResult != 0 )
				{
					pAxisInfo->m_sErrCode = GMP_CHANNEL_ERROR;
				}
			}

			switch(pAxisInfo->m_sErrCode)
			{
				//case -1:
				case GMP_DRIVER_NOT_ON_WHEN_MOVE:
					bHaveError = TRUE;
					szError.Format("Motor not on");
					break;

				case 0:
					bHaveError = FALSE;
					break;

				case GMP_TIMEOUT:
					bHaveError = TRUE;
					szError.Format("Motion Timeout");
					break;

				case GMP_INVALID_CHANNEL_NAME:
					bHaveError = TRUE;
					szError.Format("Invalid channel name");
					break;

				case GMP_INVALID_MOTION_ID:
					bHaveError = TRUE;
					szError.Format("Invalid motion ID");
					break;

				case GMP_CHANNEL_ERROR:
					bHaveError = TRUE;
					if ( (sResult = gmp_read_io_port(pcPort, &uiResult)) != 0 )
					{
						CAsmException e((UINT)sResult, "gmp_read_io_port", szPort);
						throw e;
					}

					if ( uiResult  & GMP_DRV_FAULT )
					{
						GMP_U32 uiDriverState;
						CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

						//Copy ChName
						if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
						{
							CAsmException e((UINT)0, "No such ch name", "CheckMotionResult");
							throw e;
						}
						strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

						if ( (pAxisInfo->m_sErrCode = gmp_rpt_driver_state(pcChName, &uiDriverState)) != 0 )
						{
							CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_rpt_driver_state", pAxisInfo->m_szName);
							throw e;
						}
						else
						{
							szError.Format("Driver fault = %d", uiDriverState);
						}
					}
					else if ( uiResult & GMP_ENCODER_FAULT )
					{
						szError.Format("Encoder fault");
					}
					if ( uiResult & GMP_SRH_TIMEOUT )
					{
						szError.Format("Search failed");
					}
					else if ( uiResult & GMP_EXCEED_SW_LMT )
					{
						szError.Format("Hit s/w Limit");
					}
					else if ( uiResult & GMP_HIT_LMT_SNR )
					{
						szError.Format("Hit limit sensor");
					}
					else if ( uiResult & GMP_CTRL_ERR )
					{
						szError.Format("Hit control error");
					}
					else if ( uiResult & GMP_MOTION_ABORT )
					{
						szError.Format("Hit motion abort");
					}
					else if ( uiResult & GMP_DEPEND_CH_ERR )
					{
						szError.Format("Hit depend ch error");
					}
					else if ( uiResult & GMP_HIT_PSW_LMT )
					{
						szError.Format("Hit psw limit");
					}
					else
					{
						szError.Format("Others fault = %d", uiResult);
					}
					break;

				case GMP_JOYSTICK_NOT_DISABLED:
					bHaveError = TRUE;
					szError.Format("Joystick NOT OFF");
					break;
					
				case GMP_EXCEED_SW_POSN_LMT:
					bHaveError = TRUE;
					szError.Format("Hit Projected s/w Limit");
					break;
				
				default:
					bHaveError = TRUE;
					if (pAxisInfo->m_szName.GetLength() == 0)	//v4.59A40
					{
						bShowAlarm = FALSE;
					}
					szError.Format("Unknown error = %d", pAxisInfo->m_sErrCode);
					break;
			}

			if (bHaveError == TRUE )
			{
				//v4.59A41
				SetStatusMessage("Motion Error: " + szAxis + " " + pAxisInfo->m_szTag + " " + szError);
				SetErrorMessage("Motion Error: " + szAxis + " " + pAxisInfo->m_szTag + " " + szError);
				if (bShowAlarm)
				{
					SetAlert_Msg_Red_Yellow(niCode, ("\n" + szAxis + " " + pAxisInfo->m_szTag + " " + szError));
				}
			}
			return TRUE;
		}
		catch (CAsmException e)
		{
			m_bStopAllMotion = TRUE;

			// Backup all data asap
			// prevent to wait the operator to access the NVRam
			MotionErrorBackupNVRam();

			DisplayException(e);

			SetStatusMessage("Motion Error: " + szAxis + " " + pAxisInfo->m_szTag + " Unclassified Motion error");
			SetErrorMessage("Motion Error: " + szAxis + " " + pAxisInfo->m_szTag + " Unclassified Motion error");

			SetAlert_Msg_Red_Yellow(niCode, ("\n" + szAxis + " " + pAxisInfo->m_szTag + " Unclassified Motion error"));

			return FALSE;
		}

#else

		try
		{
			SFM_CHipecAcServo*	pServo = NULL;
			SFM_CHipecStepper*	pStepper = NULL;
			SFM_CHipecDcServo*	pDCServo = NULL;
			BOOL				IsUseServo = TRUE;	

			// 6 Represent AcServo
			// 7 Represent DCServo
			// elese Stepper
			
			if (GetAxisType(szAxis) == 6)
			{
				pServo = GetHipecAcServo(GetActuatorName(szAxis));
				IsUseServo = TRUE;
			}
			else if (GetAxisType(szAxis) == 7)
			{
				pDCServo = GetHipecDcServo(GetActuatorName(szAxis));
			}
			else
			{
				pStepper = GetHipecStepper(GetActuatorName(szAxis));
				IsUseServo = FALSE;
			}

			if (pServo || pStepper || pDCServo)
			{
				if (GetAxisType(szAxis) == 6)
				{
					if (pServo != NULL)		//Klocwork
					{
						siResult = pServo->GetLastWarning();	
						lReport	= pServo->ReportErrorStatus();
					}
				}
				else if (GetAxisType(szAxis) == 7)
				{
					if (pDCServo != NULL)	//Klocwork
					{
						siResult = pDCServo->GetLastWarning();	
						lReport	= pDCServo->ReportErrorStatus();
					}
				}
				else
				{
					if (pStepper != NULL)	//Klocwork
					{
						siResult = pStepper->GetLastWarning();	
						lReport	= pStepper->ReportErrorStatus();
					}
				}

				//v3.65		//andrew
				//By pass GetLastWarning() result
				if ((lReport == HP_SUCCESS) && (siResult != HP_MOTOR_NOT_ON) && (siResult != HP_INVALID_MOTION_STATE_IN_INCR))
				{
					return TRUE;
				}

				if (lReport != HP_SUCCESS)
				{
					siResult = HP_CHANNEL_ERROR;
				}
				if (siResult != HP_SUCCESS)
				{
					switch(siResult)
					{
						case HP_MOTOR_NOT_ON:
							szError.Format("Motor NOT ON");
							break;

						case HP_INVALID_MOTION_STATE_IN_INCR:
							szError.Format("Joystick NOT OFF");
							break;

						case HP_CHANNEL_ERROR:
	//						lReport	= pServo->ReportErrorStatus();

							if (lReport & HP_LIMIT_SENSOR)
							{
								szError.Format("Hit limit sensor");
							}
							else if (lReport & HP_SEARCH_FAIL)
							{
								szError.Format("Search failed");
							}
							else if (lReport & HP_DAC_LIMIT)
							{
								szError.Format("Hit DAC Limit");
							}
							else if (lReport & HP_SW_POS_LIMIT)
							{
								szError.Format("Hit s/w Limit");
							}
							else if (lReport & HP_DRIVER_FAULT)
							{
								szError.Format("Driver fault");
							}
							else if (lReport & HP_ENCODER_FAULT)
							{
								szError.Format("Encoder fault");
							}
							else if (lReport & HP_ISOLATION_POWER_FAULT)
							{
								szError.Format("Power fault");
							}
							else if (lReport & HP_POS_ERROR_LIMIT)
							{
								szError.Format("Hit Position Error Limit");
							}
							else if (lReport & HP_TEMPERATURE_POWER_FAULT)
							{
								szError.Format("Driver temperature fault");
							}
							else 
							{
								szError.Format("Others fault = %d",lReport);
							}
							break;

						default:
							szError.Format("Unknown Error = %d",siResult);
							break;
					}

					//MotionErrorBackupNVRam();

					SetStatusMessage("Motion Error: " + szAxis + " " + szError);
					SetErrorMessage("Motion Error: " + szAxis + " " + szError);

					SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, ("\n" + szAxis + " " + szError));

					return FALSE;
				}
			}

			return TRUE;
		}
		catch (CAsmException e)
		{
			m_bStopAllMotion = TRUE;

			MotionErrorBackupNVRam();

			DisplayException(e);

			SetStatusMessage("Motion Error: " + szAxis + " Unclassified HiPEC error");
			SetErrorMessage("Motion Error: " + szAxis + " Unclassified HiPEC error");
			
			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, ("\n" + szAxis + " Unclassified HiPEC error"));
		}
#endif
	}

	return FALSE;
}

BOOL CMS896AStn::CheckHiPECResult(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		SFM_CHipecAcServo*	pServo = NULL;
		SFM_CHipecStepper*	pStepper = NULL;
		SFM_CHipecDcServo*	pDCServo = NULL;
		BOOL				IsUseServo = TRUE;	

		SHORT				siResult = HP_SUCCESS;
		LONG				lReport = 0;
		CString				szError;

		try
		{
			// 6 Represent AcServo
			// 7 Represent DCServo
			// elese Stepper
			
			if (GetAxisType(szAxis) == 6)
			{
				pServo = GetHipecAcServo(GetActuatorName(szAxis));
				IsUseServo = TRUE;
			}
			else if (GetAxisType(szAxis) == 7)
			{
				pDCServo = GetHipecDcServo(GetActuatorName(szAxis));
			}
			else
			{
				pStepper = GetHipecStepper(GetActuatorName(szAxis));
				IsUseServo = FALSE;
			}

			if (pServo || pStepper || pDCServo)
			{
				if (GetAxisType(szAxis) == 6)
				{
					if (pServo != NULL)		//Klocwork
					{
						siResult = pServo->GetLastWarning();	
						lReport	= pServo->ReportErrorStatus();
					}
				}
				else if (GetAxisType(szAxis) == 7)
				{
					if (pDCServo != NULL)	//Klocwork
					{
						siResult = pDCServo->GetLastWarning();	
						lReport	= pDCServo->ReportErrorStatus();
					}
				}
				else
				{
					if (pStepper != NULL)	//Klocwork
					{
						siResult = pStepper->GetLastWarning();	
						lReport	= pStepper->ReportErrorStatus();
					}
				}

				//v3.65		//andrew
				//By pass GetLastWarning() result
				if ((lReport == HP_SUCCESS) && (siResult != HP_MOTOR_NOT_ON) && (siResult != HP_INVALID_MOTION_STATE_IN_INCR))
				{
					return TRUE;
				}
				if (lReport != HP_SUCCESS)
				{
					siResult = HP_CHANNEL_ERROR;
				}
				if (siResult != HP_SUCCESS)
				{
					switch(siResult)
					{
						case HP_MOTOR_NOT_ON:
							szError.Format("Motor NOT ON");
							break;

						case HP_INVALID_MOTION_STATE_IN_INCR:
							szError.Format("Joystick NOT OFF");
							break;

						case HP_CHANNEL_ERROR:
	//						lReport	= pServo->ReportErrorStatus();

							if (lReport & HP_LIMIT_SENSOR)
							{
								szError.Format("Hit limit sensor");
							}
							else if (lReport & HP_SEARCH_FAIL)
							{
								szError.Format("Search failed");
							}
							else if (lReport & HP_DAC_LIMIT)
							{
								szError.Format("Hit DAC Limit");
							}
							else if (lReport & HP_SW_POS_LIMIT)
							{
								szError.Format("Hit s/w Limit");
							}
							else if (lReport & HP_DRIVER_FAULT)
							{
								szError.Format("Driver fault");
							}
							else if (lReport & HP_ENCODER_FAULT)
							{
								szError.Format("Encoder fault");
							}
							else if (lReport & HP_ISOLATION_POWER_FAULT)
							{
								szError.Format("Power fault");
							}
							else if (lReport & HP_POS_ERROR_LIMIT)
							{
								szError.Format("Hit Position Error Limit");
							}
							else if (lReport & HP_TEMPERATURE_POWER_FAULT)
							{
								szError.Format("Driver temperature fault");
							}
							else 
							{
								szError.Format("Others fault = %d",lReport);
							}
							break;

						default:
							szError.Format("Unknown Error = %d",siResult);
							break;
					}

					SetStatusMessage("Motion Error: " + szAxis + " " + szError);
					SetErrorMessage("Motion Error: " + szAxis + " " + szError);

					SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, ("\n" + szAxis + " " + szError));

					return FALSE;
				}
			}

			return TRUE;
		}
		catch (CAsmException e)
		{
			m_bStopAllMotion = TRUE;

			MotionErrorBackupNVRam();

			DisplayException(e);

			SetStatusMessage("Motion Error: " + szAxis + " Unclassified HiPEC error");
			SetErrorMessage("Motion Error: " + szAxis + " Unclassified HiPEC error");

			SetAlert_Msg_Red_Yellow(IDS_MS_MOTION_ERROR, ("\n" + szAxis + " Unclassified HiPEC error"));
		}
	}

	return FALSE;
}

BOOL CMS896AStn::MotionWriteSoftwarePort(const CString &szPortID, USHORT usBit, BOOL bState)
{
	if ( m_fHardware == FALSE )
	{
		return FALSE;
	}

	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		for (LONG i=0; i<UM_MAX_SW_PORT; i++)
		{
			if ( CMS896AApp::m_NmSWPort[i].m_szName == szPortID)
			{
				GMP_S16 sResult = 0;
				GMP_U32 uiMask = 1<<(GMP_U32)usBit;
				GMP_U32 uiPattern = 1<<(GMP_U32)usBit;
				CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];
				CString szBit;

				szBit.Format("%d", usBit);
				strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSWPort[i].m_szName);
				
				if ( bState == TRUE )
				{
					uiPattern = 0;
				}

				if ( (sResult = gmp_write_io_port(pcPortID, (ULONG)uiPattern, (ULONG)uiMask, GMP_BY_PASS, GMP_KEEP)) != 0 )
				{
					CAsmException e((UINT)sResult, "gmp_write_io_port", CMS896AApp::m_NmSWPort[i].m_szName + ":" + szBit);
					throw e;
				}
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	return TRUE;
}

BOOL CMS896AStn::MotionDirectReadSWPort(const CString &szPortID, LONG &lValue)
{
	if ( m_fHardware == FALSE )
	{
		return FALSE;
	}
	// not through MSD
	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		GMP_S16 sResult = 0;
		GMP_U32 uiResult = 0;
		GMP_CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];
		strcpy_s(pcPortID, sizeof(pcPortID), szPortID);
		if ( (sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0 )
		{
			CAsmException e((UINT)sResult, "gmp_read_io_port", szPortID);
			throw e;
		}
		else
		{
			lValue = (LONG)uiResult;
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	return TRUE;
}

BOOL CMS896AStn::MotionReadSoftwarePort(const CString &szPortID, LONG &lBit)
{
	if ( m_fHardware == FALSE )
	{
		return FALSE;
	}

	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		for (LONG i=0; i<UM_MAX_SW_PORT; i++)
		{
			if ( CMS896AApp::m_NmSWPort[i].m_szName == szPortID )
			{
				GMP_S16 sResult = 0;
				GMP_U32 uiResult = 0;
				BOOL bAState = FALSE;
				GMP_CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];
				CString szBit;

				szBit.Format("%d", i);
				strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSWPort[i].m_szName);

				if ( (sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0 )
				{
					CAsmException e((UINT)sResult, "gmp_read_io_port", CMS896AApp::m_NmSWPort[i].m_szName + ":" + szBit);
					throw e;
				}
				else
				{
					lBit = (LONG)uiResult;
				}
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

	return TRUE;
}

BOOL CMS896AStn::MotionDirectResetSWPort(const CString &szPortID)
{
	if ( m_fHardware == FALSE )
	{
		return FALSE;
	}

	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		GMP_S16 sResult = 0;
		GMP_U32 uiMask = 0xFFFFFFFF;
		GMP_U32 uiPattern = 0xFFFFFFFF;
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		strcpy_s(pcPortID, sizeof(pcPortID), szPortID);
		
		if ( (sResult = gmp_write_io_port(pcPortID, (ULONG)uiPattern, (ULONG)uiMask, GMP_BY_PASS, GMP_KEEP)) != 0 )
		{
			CAsmException e((UINT)sResult, "gmp_write_io_port", szPortID);
			throw e;
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

BOOL CMS896AStn::MotionResetSoftwarePort(const CString &szPortID)
{
	if ( m_fHardware == FALSE )
	{
		return FALSE;
	}

	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		for (LONG i=0; i<UM_MAX_SW_PORT; i++)
		{
			if ( CMS896AApp::m_NmSWPort[i].m_szName == szPortID )
			{
				GMP_S16 sResult = 0;
				GMP_U32 uiMask = 0xFFFFFFFF;
				GMP_U32 uiPattern = 0xFFFFFFFF;
				CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

				strcpy_s(pcPortID, sizeof(pcPortID), CMS896AApp::m_NmSWPort[i].m_szName);
				
				if ( (sResult = gmp_write_io_port(pcPortID, (ULONG)uiPattern, (ULONG)uiMask, GMP_BY_PASS, GMP_KEEP)) != 0 )
				{
					CAsmException e((UINT)sResult, "gmp_write_io_port", CMS896AApp::m_NmSWPort[i].m_szName);
					throw e;
				}
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

short CMS896AStn::WriteSoftwarePort(Hp_mcb* hp_Module, short sPortNo, LONG lData, LONG lMask)
{
#ifdef OFFLINE
	return TRUE;
#endif
	short sReturn;
	//Hp_mcb *hp_Module;
	Hp_io_ext hp_IOInput;
    
	//hp_Module = &hp_Module;

	hp_IOInput.io_port = sPortNo;
	hp_IOInput.mask = lMask;

	/*
	// 8 bit sw port
	if (lPortType == 8)
	{
		hp_IOInput.mask = 0xff;
	}
	else
	{
		hp_IOInput.mask = 0xffffffff;
	}
	*/

	hp_IOInput.pattern = lData;// writing_data is in decimal		
	sReturn = hp_io_write_ext(hp_Module, &hp_IOInput );// Put data to the HP_SW_PORT

	return sReturn;
}

short CMS896AStn::ReadSoftwarePort(Hp_mcb* hp_Module, short sPortNo, LONG& lData)
{
#ifdef OFFLINE
	return TRUE;
#endif
	short sReturn;
	//Hp_mcb *hp_Module;
	Hp_io_ext hp_IOInput;
    
	//hp_Module = &hp_Module;

	hp_IOInput.io_port = sPortNo;
	hp_IOInput.mask = 0xffffffff;
	hp_IOInput.pattern = lData;// writing_data is in decimal
		
	sReturn = hp_io_read_ext(hp_Module, &hp_IOInput );// Put data to the HP_SW_PORT

	if (hp_IOInput.pattern > 0x7fffffff)
		lData = hp_IOInput.pattern - 0xffffffff - 1;
	else
		lData = hp_IOInput.pattern;

	return sReturn;
}

SHORT CMS896AStn::MotionGetDacLimit(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	SHORT ssReult=0;

	if ( MotionIsServo(szAxis, pAxisInfo) == FALSE )
	{
		return FALSE;
	}

	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			//don't know

#else
			if (GetAxisType(szAxis) == MS899_SVO_MOTOR)
			{
				SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(szAxis));

				if (pServo != NULL)
				{
					ssReult = pServo->GetDacLimit();
					return ssReult;
				}
			}
			else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
			{
				SFM_CHipecDcServo *pDcServo = GetHipecDcServo(GetActuatorName(szAxis));

				if (pDcServo != NULL)
				{
					ssReult = pDcServo->GetDacLimit();
					return ssReult;
				}		
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
	}

	return FALSE;
}

SHORT CMS896AStn::MotionGetDacTimeLimit(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	SHORT ssReult=0;

	if ( MotionIsServo(szAxis, pAxisInfo) == FALSE )
	{
		return FALSE;
	}

	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			//don't know

#else
			if (GetAxisType(szAxis) == MS899_SVO_MOTOR)
			{
				SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(szAxis));

				if (pServo != NULL)
				{
					ssReult = pServo->GetDacTimeLimit();
					return ssReult;
				}
			}
			else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
			{
				SFM_CHipecDcServo *pDcServo = GetHipecDcServo(GetActuatorName(szAxis));

				if (pDcServo != NULL)
				{
					ssReult = pDcServo->GetDacTimeLimit();
					return ssReult;
				}		
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
	}

	return FALSE;
}

BOOL CMS896AStn::MotionSetDacLimit(const CString& szAxis, SHORT ssCurrDacLimit, SHORT ssOrgDacLimit, CMSNmAxisInfo* pAxisInfo)
{
	if ( MotionIsServo(szAxis, pAxisInfo) == FALSE )
	{
		return TRUE;
	}

	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			//don't know

#else
			if (GetAxisType(szAxis) == MS899_SVO_MOTOR)
			{
				SFM_CHipecAcServo *pServo = GetHipecAcServo(GetActuatorName(szAxis));

				if (pServo != NULL)
				{
					pServo->SetDacLimit(ssCurrDacLimit, ssOrgDacLimit);
				}
			}
			else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
			{
				SFM_CHipecDcServo *pDcServo = GetHipecDcServo(GetActuatorName(szAxis));

				if (pDcServo != NULL)
				{
					pDcServo->SetDacLimit(ssCurrDacLimit, ssOrgDacLimit);
				}
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
	}

	return TRUE;
}

VOID CMS896AStn::MotionUpdateDriveIn(const CString& szAxis, const CString& szProfileID, LONG lDriveIn, 
									 CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			//don't know

#else
			SetDriveIn(szAxis, szProfileID, lDriveIn);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

VOID CMS896AStn::MotionUpdateDebounce(const CString& szAxis, const CString& szProfileID, LONG lDebounce, 
									  CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			//don't know

#else
			SetDebounce(szAxis, szProfileID, lDebounce);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

BOOL CMS896AStn::MotionSetVelocityFilter(const CString& szAxis, SHORT ssVelFilterGainX, SHORT ssVelFilterWeightX, 
										 CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			//don't know

#else
			SetVelocityFilter(szAxis, ssVelFilterGainX, ssVelFilterWeightX);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
	}

	return TRUE;
}

VOID CMS896AStn::MotionSetSearchLimit(const CString& szAxis, const CString& szProfileID, LONG lDistance, 
									  CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			BOOL bProfile = FALSE;

			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionSetSearchLimit: " + szAxis);
				return;
			}

			for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
				{
					bProfile = TRUE;
					pAxisInfo->m_stSrchProfile[i].m_lDistance = lDistance;
				}
			}

			if ( bProfile == FALSE )
			{
				CAsmException e((UINT)0, "MotionSetSearchLimit", "No such Profile: " + szProfileID);
				throw e;
			}

#else
			SetSearchLimit(szAxis, szProfileID, lDistance);
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
}

LONG CMS896AStn::MotionGetPosition(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	if (m_fHardware)
	{
		try
		{
#ifdef NU_MOTION

			//don't know

#else
			if (GetAxisType(szAxis) == MS899_SVO_MOTOR)
			{
				SFM_CHipecAcServo* pServo = GetHipecAcServo(GetActuatorName(szAxis));	
				
				if (pServo != NULL)
				{
					return ( (LONG)((DOUBLE)pServo->GetPosition()) );
				}
			}
			else if (GetAxisType(szAxis) == MS899_STP_MOTOR)
			{
				SFM_CHipecStepper* pStepper = GetHipecStepper(GetActuatorName(szAxis));	
				
				if (pStepper != NULL)
				{
					return ( (LONG)((DOUBLE)pStepper->GetPosition()) );
				}
			}
			else if (GetAxisType(szAxis) == MS899_DCSVO_MOTOR)
			{
				SFM_CHipecDcServo* pDcServo = GetHipecDcServo(GetActuatorName(szAxis));	
				
				if (pDcServo != NULL)
				{
					return ( (LONG)((DOUBLE)pDcServo->GetPosition()) );
				}
			}
#endif
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}

	return 0;
}


BOOL CMS896AStn::LogAxisPerformance(const CString &szModuleID, CMSNmAxisInfo* pAxisInfo, BOOL bEnable, LONG lSettleTime, DATA_LOG_INFO &stLogInfo, 
									UCHAR ucLogMode, BOOL bDisplay, BOOL bWriteToFile)
{
	if (!m_fHardware)
		return FALSE;
	
	TRY 
	{
#ifdef NU_MOTION
		return NuMotionDataLog(szModuleID, *pAxisInfo, bEnable, lSettleTime, stLogInfo, ucLogMode, bDisplay, bWriteToFile);	//v3.94
#endif
	} 
	CATCH (CFileException, e)
	{
		return FALSE;
	}END_CATCH
	
	return TRUE;
}


BOOL CMS896AStn::LogAxisPerformance2(const CString &szModuleID1, const CString &szModuleID2, CMSNmAxisInfo* pAxisInfo1, CMSNmAxisInfo* pAxisInfo2, 
									 BOOL bEnable, BOOL bDisplay, BOOL bSpecialLog, BOOL bWriteToFile)
{
	if (!m_fHardware)
		return FALSE;
	
	TRY 
	{
#ifdef NU_MOTION
		return NuMotionDataLog2(szModuleID1, szModuleID2, *pAxisInfo1, *pAxisInfo2, bEnable, bDisplay, bSpecialLog, bWriteToFile);		//v3.94
#endif
	} 
	CATCH (CFileException, e)
	{
		return FALSE;
	}END_CATCH
	
	return TRUE;
}

BOOL CMS896AStn::LogAxisPerformance3(const CString &szModuleID1, const CString &szModuleID2, const CString &szModuleID3, CMSNmAxisInfo* pAxisInfo1, CMSNmAxisInfo* pAxisInfo2, 
									 CMSNmAxisInfo* pAxisInfo3, BOOL bEnable, BOOL bWriteToFile)
{
	if (!m_fHardware)
		return FALSE;
	
	TRY 
	{
#ifdef NU_MOTION
		return NuMotionDataLog3(szModuleID1, szModuleID2, szModuleID3, *pAxisInfo1, *pAxisInfo2, *pAxisInfo3, bEnable, bWriteToFile);
#endif
	} 
	CATCH (CFileException, e)
	{
		return FALSE;
	}END_CATCH
	
	return TRUE;
}

BOOL CMS896AStn::GetNuMotionDataLogData(const CString &szModuleID, CMSNmAxisInfo &stAxis, BOOL bEnable, 
									INT& nNoOfSampleUpload,INT* pPos, INT* pEnc, INT* pChannelMode)
{
	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION

		GMP_U32 uiPort = 4;

		if ( bEnable == TRUE )
		{
			GMP_S16 sResult;
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;
			GMP_CHAR *pcPortList[4];
			GMP_CHAR pcPort[4][GMP_CH_NAME_CHAR_LEN];
			CString szPort;
			
			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_ONE_SHOT;

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_CMD_PORT].m_szName;
			pcPortList[0] = strcpy(pcPort[0], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_ENC_PORT].m_szName;
			pcPortList[1] = strcpy(pcPort[1], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_MODE_PORT].m_szName;
			pcPortList[2] = strcpy(pcPort[2], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_DAC_PORT].m_szName;
			pcPortList[3] = strcpy(pcPort[3], szPort);

			if ( (sResult = gmp_request_datalog("Axis Log", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_config("Axis Log", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("Axis Log", uiPort, pcPortList)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}

			if ( (sResult = gmp_enable_datalog("Axis Log")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}
		}
		else
		{
			GMP_U32 uiData[UM_MAX_CH_PORT*0x1000];
			GMP_U32 uiNoOfSampleUpload=0;
			GMP_U32 uiMinValue=0, uiMaxValue=0;
			GMP_U32 uiStartLogSample=0;
			GMP_U32 uiMotionCompleteSample=0;
			INT siValue=0, siMinValue=0, siMaxValue=0;
			UINT i, uiIndex;

			memset(uiData,0, sizeof(uiData));

			//Stop datalog
			gmp_disable_datalog("Axis Log");
			gmp_get_datalog("Axis Log", uiData, 0x1000-1, &uiNoOfSampleUpload);
			gmp_request_datalog("Axis Log", GMP_RELEASE);

			nNoOfSampleUpload = (INT)uiNoOfSampleUpload;

			for (i=0; i<uiNoOfSampleUpload; i++)
			{
				uiIndex = i*(uiPort+1);
				pPos[i] = (INT)uiData[uiIndex+1];
				pEnc[i] = (INT)uiData[uiIndex+2];
				pChannelMode[i] = (INT)uiData[uiIndex+3];
			}

		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

BOOL CMS896AStn::NuMotionDataLog(const CString &szModuleID, CMSNmAxisInfo &stAxis, BOOL bEnable, LONG lSettleTime, DATA_LOG_INFO &stLogInfo, UCHAR ucLogMode, 
								 BOOL bDisplay, BOOL bWriteToFile)
{
	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION

		GMP_U32 uiPort = 4;

		if ( lSettleTime < 0 )
		{
			lSettleTime = 1;
		}

		if ( bEnable == TRUE )
		{
			GMP_S16 sResult;
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;
			GMP_CHAR *pcPortList[4];
			GMP_CHAR pcPort[4][GMP_CH_NAME_CHAR_LEN];
			CString szPort;
			

			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_CONTINUOUS;	//GMP_ONE_SHOT;		//v4.24

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_CMD_PORT].m_szName;			//v0.02
			pcPortList[0] = strcpy(pcPort[0], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_ENC_PORT].m_szName;			//v0.02
			pcPortList[1] = strcpy(pcPort[1], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_MODE_PORT].m_szName;		//v0.02
			pcPortList[2] = strcpy(pcPort[2], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_DAC_PORT].m_szName;			//v0.02
			pcPortList[3] = strcpy(pcPort[3], szPort);

			if ( (sResult = gmp_request_datalog("Axis Log", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
	
			throw e;
			}

			if ( (sResult = gmp_set_datalog_config("Axis Log", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("Axis Log", uiPort, pcPortList)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}

			if ( (sResult = gmp_enable_datalog("Axis Log")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}
		}
		else
		{
			if (bWriteToFile == FALSE)
			{
				gmp_disable_datalog("Axis Log");
				gmp_request_datalog("Axis Log", GMP_RELEASE);
				return TRUE;
			}

			//v3.94	//Changed NuMotion data log folder to \DataLog sub-folder in \UserData
			CString szLogFile = gszUSER_DIRECTORY + "\\DataLog\\" + szModuleID + "_DataLog.csv";
			CString szDacFile = gszUSER_DIRECTORY + "\\DataLog\\" + szModuleID + "_DacLog.csv";
			CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\" + szModuleID + "_FullLog.csv";
			CString szDisplayLog = gszUSER_DIRECTORY + "\\Diagnostics\\NuMotionLog.csv";		//v3.94	//New display log file for HMI display purpose only

			FILE *fp = NULL, *fFullLog = NULL, *fpDisplay=NULL;
			GMP_U32 uiData[UM_MAX_CH_PORT*0x8000];
			GMP_U32 uiNoOfSampleUpload=0;
			GMP_U32 uiMinValue=0, uiMaxValue=0;
			GMP_U32 uiStartLogSample=0;
			GMP_U32 uiMotionCompleteSample=0;
			INT siValue=0, siMinValue=0, siMaxValue=0;
			UINT i, uiIndex;

			memset(uiData,0, sizeof(uiData));
			if ( lSettleTime > 0 )
			{
				Sleep(lSettleTime);
			}

			//Stop datalog
			gmp_disable_datalog("Axis Log");
			gmp_get_datalog("Axis Log", uiData, 0x8000-1, &uiNoOfSampleUpload);
			gmp_request_datalog("Axis Log", GMP_RELEASE);

			Sleep(500);


			//Output full log file
			errno_t nErr = fopen_s(&fFullLog, szFullLog, "w");
			if ((nErr == 0) && (fFullLog != NULL))
			{
				errno_t nfDisplayErr = -1;
				if (bDisplay)	//v3.94
				{
					nfDisplayErr = fopen_s(&fpDisplay, szDisplayLog, "w");
				}

				fprintf(fFullLog, "Number of Sample = %d\n", uiNoOfSampleUpload);
				fprintf(fFullLog, "CMD,ENC,MODE,DAC\n");

				for (i=0; i<uiNoOfSampleUpload; i++)
				{
					uiIndex = i*(uiPort+1);
					siValue = uiData[uiIndex+4];
					fprintf(fFullLog, "%d,%d,%d,%d\n", uiData[uiIndex+1], uiData[uiIndex+2], uiData[uiIndex+3], siValue/NU_MOTION_DAC_RATIO);
				
					//v3.94		//Add aditional diaply log file for HMI display
					if (bDisplay && (nfDisplayErr == 0) && (fpDisplay != NULL) && (i <= 10000))
					{
						fprintf(fpDisplay, "%d,%d,%d\n", i+1, uiData[uiIndex+2], uiData[uiIndex+1]);
					}
				}
				
				fclose(fFullLog);

				//v3.94
				if (bDisplay && (nfDisplayErr == 0) && (fpDisplay != NULL))
				{
					fclose(fpDisplay);
				}
			}

			if (ucLogMode == 0)
			{
				//Output DAC into Dac file
				errno_t nErr = fopen_s(&fp, szDacFile, "w");
				if ((nErr == 0) && (fp != NULL))
				{
					siMinValue = 0;
					siMaxValue = 0;

					for (i=0; i<uiNoOfSampleUpload; i++)
					{
						uiIndex = i*(uiPort+1);
						siValue = uiData[uiIndex+4];
						fprintf(fp, "%d,%d\n", i/NU_MOTION_SAMPLE_RATE, siValue/NU_MOTION_DAC_RATIO);

						if ( siValue < siMinValue )
						{
							siMinValue = siValue;
							stLogInfo.lMinDacValue	= siMinValue/NU_MOTION_DAC_RATIO;
							stLogInfo.lMinDacEncPos = uiData[uiIndex+2];
						}
						else if ( siValue > siMaxValue )
						{
							siMaxValue = siValue;
							stLogInfo.lMaxDacValue	= siMaxValue/NU_MOTION_DAC_RATIO;
							stLogInfo.lMaxDacEncPos = uiData[uiIndex+2];
						}
					}

					fclose(fp);
				}

				//Check which sample motion is completed
				for (i=0; i<uiNoOfSampleUpload; i++)
				{
					uiIndex = i*(uiPort+1);
					
					if ( (i >= 5) && (uiData[uiIndex+3] == 0x10000) )
					{
						uiMotionCompleteSample = i;

						//Record end DAC value
						siValue = uiData[uiIndex+4];
						stLogInfo.lEndDacValue = siValue/NU_MOTION_DAC_RATIO;

						//Set start sample 5ms before motion complete
						uiStartLogSample = uiMotionCompleteSample - (5*NU_MOTION_SAMPLE_RATE);
						//if ( uiStartLogSample < 0 )	//Klocwork
						//{
						//	uiStartLogSample = 0;
						//}
						break;
					}
				}

				nErr = fopen_s(&fp, szLogFile, "w");
				if ((nErr == 0) && (fp != NULL))
				{
					//Output command pos
					for (i=uiStartLogSample; i<uiNoOfSampleUpload; i++)
					{
						uiIndex = i*(uiPort+1);
						fprintf(fp, "%d,%d\n", i/NU_MOTION_SAMPLE_RATE, uiData[uiIndex+1]);

						uiMinValue = uiData[uiIndex+1];
						uiMaxValue = uiData[uiIndex+1];
					}
					
					//Output encoder pos
					for (i=uiStartLogSample; i<uiNoOfSampleUpload; i++)
					{
						uiIndex = i*(uiPort+1);
						fprintf(fp, "%d,%d\n", i/NU_MOTION_SAMPLE_RATE, uiData[uiIndex+2]);

						//only calcualte Min & Max value after motion complete + settle time
						if ( i >= (UINT)((uiMotionCompleteSample + lSettleTime*NU_MOTION_SAMPLE_RATE)) )
						{
							uiMinValue = min(uiMinValue, uiData[uiIndex+2]);
							uiMaxValue = max(uiMaxValue, uiData[uiIndex+2]);

							//Record settle DAC value
							siValue = uiData[uiIndex+4];
							stLogInfo.lSettleDacValue = siValue/NU_MOTION_DAC_RATIO;
						}
					}

					//Output motion complete time vertical line
					for (i=uiStartLogSample; i<uiNoOfSampleUpload; i++)
					{
						uiIndex = i*(uiPort+1);
						fprintf(fp, "%d,%d\n", uiMotionCompleteSample/NU_MOTION_SAMPLE_RATE, uiData[uiIndex+2]);
					}

					//Output settle time vertical line
					for (i=uiStartLogSample; i<uiNoOfSampleUpload; i++)
					{
						uiIndex = i*(uiPort+1);
						fprintf(fp, "%ld,%d\n", (uiMotionCompleteSample/NU_MOTION_SAMPLE_RATE) + lSettleTime, uiData[uiIndex+2]);
					} 

					stLogInfo.lMotionTime	= _round(uiMotionCompleteSample / NU_MOTION_SAMPLE_RATE);
					stLogInfo.lStaticError	= _round(labs(uiMinValue - uiMaxValue));

					fclose(fp);
				}
			}
		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

BOOL CMS896AStn::NuMotionDataLog_DAC(const CString &szModuleID, CMSNmAxisInfo &stAxis, BOOL bEnable, LONG lSettleTime, DATA_LOG_INFO &stLogInfo, 
										BOOL bPickToBond)
{
	try
	{
#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION

		GMP_U32 uiPort = 4;

		if ( lSettleTime < 0 )
		{
			lSettleTime = 1;
		}

		if ( bEnable == TRUE )
		{
			GMP_S16 sResult;
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;
			GMP_CHAR *pcPortList[4];
			GMP_CHAR pcPort[4][GMP_CH_NAME_CHAR_LEN];
			CString szPort;
			

			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_ONE_SHOT;

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_CMD_PORT].m_szName;		//v0.02
			pcPortList[0] = strcpy(pcPort[0], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_ENC_PORT].m_szName;		//v0.02
			pcPortList[1] = strcpy(pcPort[1], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_MODE_PORT].m_szName;	//v0.02
			pcPortList[2] = strcpy(pcPort[2], szPort);

			//szPort = stAxis.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis.m_szName + stAxis.m_NmCHPort[MS896A_DAC_PORT].m_szName;		//v0.02
			pcPortList[3] = strcpy(pcPort[3], szPort);

			if ( (sResult = gmp_request_datalog("Axis Log", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
	
			throw e;
			}

			if ( (sResult = gmp_set_datalog_config("Axis Log", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("Axis Log", uiPort, pcPortList)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}

			if ( (sResult = gmp_enable_datalog("Axis Log")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}
		}
		else
		{
			//Changed NuMotion data log folder to \DataLog sub-folder in \UserData
			CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\" + szModuleID + "_FullLog.csv";
			CString szDacFile; 	
			if (bPickToBond)
			{
				szDacFile = gszUSER_DIRECTORY + "\\Diagnostics\\BH\\DacFromPickToBond_Graph.csv";
			}
			else
			{
				szDacFile = gszUSER_DIRECTORY + "\\Diagnostics\\BH\\DacFromBondToPick_Graph.csv";
			}


			FILE *fp=NULL, *fFullLog=NULL;
			GMP_U32 uiData[UM_MAX_CH_PORT*0x8000];
			GMP_U32 uiNoOfSampleUpload=0;
			GMP_U32 uiMinValue=0, uiMaxValue=0;
			GMP_U32 uiStartLogSample=0;
			GMP_U32 uiMotionCompleteSample=0;
			INT siValue=0, siMinValue=0, siMaxValue=0;
			UINT i, uiIndex;

			memset(uiData,0, sizeof(uiData));
			if ( lSettleTime > 0 )
			{
				Sleep(lSettleTime);
			}

			//Stop datalog
			gmp_disable_datalog("Axis Log");
			gmp_get_datalog("Axis Log", uiData, 0x8000-1, &uiNoOfSampleUpload);
			gmp_request_datalog("Axis Log", GMP_RELEASE);

			Sleep(500);

			//Output full log file
			errno_t nErr = fopen_s(&fFullLog, szFullLog, "w");
			if ((nErr == 0) && (fFullLog != NULL))
			{
				fprintf(fFullLog, "Number of Sample = %d\n", uiNoOfSampleUpload);
				fprintf(fFullLog, "CMD,ENC,MODE,DAC\n");

				for (i=0; i<uiNoOfSampleUpload; i++)
				{
					uiIndex = i*(uiPort+1);
					siValue = uiData[uiIndex+4];
					fprintf(fFullLog, "%d,%d,%d,%d\n", uiData[uiIndex+1], uiData[uiIndex+2], uiData[uiIndex+3], siValue/NU_MOTION_DAC_RATIO);
				}
				
				fclose(fFullLog);
			}

			//Output DAC into Dac file
			nErr = fopen_s(&fp, szDacFile, "w");
			if ((nErr == 0) && (fp != NULL))
			{
				siMinValue = 0;
				siMaxValue = 0;

				for (i=0; i<uiNoOfSampleUpload; i++)
				{
					uiIndex = i*(uiPort+1);
					siValue = uiData[uiIndex+4];
					//fprintf(fp, "%d,%d\n", i/NU_MOTION_SAMPLE_RATE, siValue/NU_MOTION_DAC_RATIO);
					fprintf(fp, "%d,%d\n", i, siValue/NU_MOTION_DAC_RATIO);

					if ( siValue < siMinValue )
					{
						siMinValue = siValue;
						stLogInfo.lMinDacValue	= siMinValue/NU_MOTION_DAC_RATIO;
						stLogInfo.lMinDacEncPos = uiData[uiIndex+2];
					}
					else if ( siValue > siMaxValue )
					{
						siMaxValue = siValue;
						stLogInfo.lMaxDacValue	= siMaxValue/NU_MOTION_DAC_RATIO;
						stLogInfo.lMaxDacEncPos = uiData[uiIndex+2];
					}
				}

				fclose(fp);
			}

		}
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}


BOOL CMS896AStn::NuMotionDataLog2(const CString &szModuleID1, const CString &szModuleID2, CMSNmAxisInfo &stAxis1, CMSNmAxisInfo &stAxis2, BOOL bEnable, 
								  BOOL bDisplay, BOOL bSpecialLog, BOOL bWriteToFile)
{
#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION
	try
	{
		GMP_U32 uiPort = 8;
		
		//v4.43T12
		if ( (szModuleID1 == BH_AXIS_Z) && (szModuleID2 == BH_AXIS_Z2) && (bSpecialLog) )		//v4.43T12
		{
			uiPort = 11;
		}

		if ( bEnable == TRUE )
		{
			GMP_S16 sResult;
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;
			GMP_CHAR *pcPortList[11];		//Klocwork	//v4.46
			GMP_CHAR pcPort[8][GMP_CH_NAME_CHAR_LEN];
			CString szPort;
			
			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_CONTINUOUS;	//GMP_ONE_SHOT;		//v4.24

			//if ( (szModuleID1 == BH_AXIS_Z) && (bSpecialLog) )		//v4.43T12
			//{
			//	pcPortList[0] = BHZ1_ADC_MD;
			//}
			//else
			//{
			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_CMD_PORT].m_szName;		//v0.02
			pcPortList[0] = strcpy(pcPort[0], szPort);
			//}

			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_ENC_PORT].m_szName;		//v0.02
			pcPortList[1] = strcpy(pcPort[1], szPort);

			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_MODE_PORT].m_szName;		//v0.02
			pcPortList[2] = strcpy(pcPort[2], szPort);

			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_DAC_PORT].m_szName;		//v0.02
			pcPortList[3] = strcpy(pcPort[3], szPort);

			//if ( (szModuleID2 == BH_AXIS_Z2) && (bSpecialLog) )		//v4.43T12
			//{
			//	pcPortList[4] = BHZ2_ADC_MD;
			//}
			//else
			//{
			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_CMD_PORT].m_szName;		//v0.02
			pcPortList[4] = strcpy(pcPort[4], szPort);
			//}

			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_ENC_PORT].m_szName;		//v0.02
			pcPortList[5] = strcpy(pcPort[5], szPort);

			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_MODE_PORT].m_szName;		//v0.02
			pcPortList[6] = strcpy(pcPort[6], szPort);

			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_DAC_PORT].m_szName;		//v0.02
			pcPortList[7] = strcpy(pcPort[7], szPort);

			if (uiPort == 11)	//v4.43T12
			{
				pcPortList[8]	= BHZ1_ADC_MD;
				pcPortList[9]	= BHZ2_ADC_MD;
				pcPortList[10]	= BHZ_AIRFLOW_PORT;
			}

			if ( (sResult = gmp_request_datalog("Axis Log", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_config("Axis Log", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("Axis Log", uiPort, pcPortList)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}

			if ( (sResult = gmp_enable_datalog("Axis Log")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}

			//if (bWriteToFile == FALSE)
			//{
			//	Sleep(200);
			//}
		}
		else
		{
			if (bWriteToFile == FALSE)
			{
				gmp_disable_datalog("Axis Log");
				gmp_request_datalog("Axis Log", GMP_RELEASE);
				return TRUE;
			}

			//v3.94	//Changed NuMotion data log folder to \DataLog sub-folder in \UserData
			CString szFullLog			= gszUSER_DIRECTORY + "\\DataLog\\" + szModuleID1 + "_" + szModuleID2 + "_" + "FullLog.csv";
			CString szDisplayLog		= gszUSER_DIRECTORY + "\\Diagnostics\\NuMotionLog.csv";			//v3.94		//New display log file for HMI display purpose only
			CString szDisplayLog_DAC	= gszUSER_DIRECTORY + "\\Diagnostics\\NuMotionLog_DAC.csv";		//v3.94T4	//New display log file for HMI display purpose only
			CString szDisplayLog_Sp		= gszUSER_DIRECTORY + "\\Diagnostics\\NuMotionLog_Sp.csv";		//v3.94T4	//New display log file for HMI display purpose only
			CString szLogAttrib = "w";
			FILE *fFullLog = NULL;
			FILE *fDisplayLog = NULL;
			FILE *fDisplayLog_DAC = NULL;
			FILE *fDisplayLog_Sp = NULL;
			GMP_U32 *uiData;
			GMP_U32 uiNoOfSampleUpload=0;
			UINT i, uiIndex;
			UINT uiNoOfData = 30000;
			INT siValue1, siValue2;
		

			uiData = new GMP_U32[uiNoOfData*(uiPort+1)];
			memset(uiData,0, sizeof(uiData));

			//Stop datalog
			gmp_disable_datalog("Axis Log");
			gmp_get_datalog("Axis Log", uiData, uiNoOfData, &uiNoOfSampleUpload);
			gmp_request_datalog("Axis Log", GMP_RELEASE);

			Sleep(500);

			//Output full log file
			errno_t nErr = fopen_s(&fFullLog, szFullLog, szLogAttrib);
			if ((nErr == 0) && (fFullLog != NULL))
			{
				if (bDisplay)		//v3.94
				{
					nErr = fopen_s(&fDisplayLog, szDisplayLog,	"w");
					nErr = fopen_s(&fDisplayLog_DAC, szDisplayLog_DAC,	"w");
				}

				if (bSpecialLog)	//v3.94T4
				{
					nErr = fopen_s(&fDisplayLog_Sp, szDisplayLog_Sp,		"w");
				}

				fprintf(fFullLog, "Number of Sample = %d\n", uiNoOfSampleUpload);
				fprintf(fFullLog, "CMD,ENC,MODE,DAC,CMD,ENC,MODE,DAC\n");

				for (i=0; i<uiNoOfSampleUpload; i++)
				{
					uiIndex = i*(uiPort+1);
					siValue1 = uiData[uiIndex+4];
					siValue2 = uiData[uiIndex+8];

					if (uiPort == 11)	//v4.43T12
					{
						fprintf(fFullLog, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",	
																		uiData[uiIndex+1], 
																		uiData[uiIndex+2], 
																		uiData[uiIndex+3], 
																		siValue1/NU_MOTION_DAC_RATIO,
																		uiData[uiIndex+5], 
																		uiData[uiIndex+6], 
																		uiData[uiIndex+7], 
																		siValue2/NU_MOTION_DAC_RATIO,
																		uiData[uiIndex+9], 
																		uiData[uiIndex+10], 
																		uiData[uiIndex+11]);
					}
					else
					{
						fprintf(fFullLog, "%d,%d,%d,%d,%d,%d,%d,%d\n",	uiData[uiIndex+1], 
																		uiData[uiIndex+2], 
																		uiData[uiIndex+3], 
																		siValue1/NU_MOTION_DAC_RATIO,
																		uiData[uiIndex+5], 
																		uiData[uiIndex+6], 
																		uiData[uiIndex+7], 
																		siValue2/NU_MOTION_DAC_RATIO);
					}
				
					//v3.94		//Add aditional diaply log file for HMI display
					if (bDisplay && (fDisplayLog != NULL) && (i <= 9998))
					{
						fprintf(fDisplayLog,		"%d,%d,%d\n", i+1, uiData[uiIndex+2], uiData[uiIndex+6]);
						if (fDisplayLog_DAC != NULL)
							fprintf(fDisplayLog_DAC,	"%d,%d,%d\n", i+1, siValue1/NU_MOTION_DAC_RATIO, siValue2/NU_MOTION_DAC_RATIO);
					}	

					if (bSpecialLog && (fDisplayLog_Sp != NULL) && (i <= 9998))
					{
						//i,BHZ-DAC,EJ_ENC
						fprintf(fDisplayLog_Sp,		"%d,%d,%d\n", i+1, siValue1/NU_MOTION_DAC_RATIO, uiData[uiIndex+6]);
					}
				}

				fclose(fFullLog);

				if (bDisplay && (fDisplayLog != NULL))
				{
					fclose(fDisplayLog);			//v3.94
				}
				if (bDisplay && (fDisplayLog_DAC != NULL))
				{
					fclose(fDisplayLog_DAC);		//v3.94T4
				}
				if (bSpecialLog && (fDisplayLog_Sp != NULL))
				{
					fclose(fDisplayLog_Sp);			//v3.94T4
				}
			}

			delete [] uiData;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
#endif
	return TRUE;
}

BOOL CMS896AStn::NuMotionDataLog3(const CString &szModuleID1, const CString &szModuleID2, const CString &szModuleID3, CMSNmAxisInfo &stAxis1, CMSNmAxisInfo &stAxis2, CMSNmAxisInfo &stAxis3, 
								  BOOL bEnable, BOOL bWriteToFile)
{
#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION
	try
	{
		GMP_U32 uiPort = 12;

		if ( bEnable == TRUE )
		{
			GMP_S16 sResult;
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;
			GMP_CHAR *pcPortList[12];
			GMP_CHAR pcPort[12][GMP_CH_NAME_CHAR_LEN];
			CString szPort;
			

			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_CONTINUOUS;	//GMP_ONE_SHOT;		//v4.24

			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_CMD_PORT].m_szName;
			pcPortList[0] = strcpy(pcPort[0], szPort);

			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_ENC_PORT].m_szName;
			pcPortList[1] = strcpy(pcPort[1], szPort);

			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_MODE_PORT].m_szName;
			pcPortList[2] = strcpy(pcPort[2], szPort);

			//szPort = stAxis1.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis1.m_szName + stAxis1.m_NmCHPort[MS896A_DAC_PORT].m_szName;
			pcPortList[3] = strcpy(pcPort[3], szPort);

			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_CMD_PORT].m_szName;
			pcPortList[4] = strcpy(pcPort[4], szPort);

			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_ENC_PORT].m_szName;
			pcPortList[5] = strcpy(pcPort[5], szPort);

			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_MODE_PORT].m_szName;
			pcPortList[6] = strcpy(pcPort[6], szPort);

			//szPort = stAxis2.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis2.m_szName + stAxis2.m_NmCHPort[MS896A_DAC_PORT].m_szName;
			pcPortList[7] = strcpy(pcPort[7], szPort);

			//szPort = stAxis3.m_szName + CMS896AApp::m_NmCHPort[MS896A_CMD_PORT].m_szName;
			szPort = stAxis3.m_szName + stAxis3.m_NmCHPort[MS896A_CMD_PORT].m_szName;
			pcPortList[8] = strcpy(pcPort[8], szPort);

			//szPort = stAxis3.m_szName + CMS896AApp::m_NmCHPort[MS896A_ENC_PORT].m_szName;
			szPort = stAxis3.m_szName + stAxis3.m_NmCHPort[MS896A_ENC_PORT].m_szName;
			pcPortList[9] = strcpy(pcPort[9], szPort);

			//szPort = stAxis3.m_szName + CMS896AApp::m_NmCHPort[MS896A_MODE_PORT].m_szName;
			szPort = stAxis3.m_szName + stAxis3.m_NmCHPort[MS896A_MODE_PORT].m_szName;
			pcPortList[10] = strcpy(pcPort[10], szPort);

			//szPort = stAxis3.m_szName + CMS896AApp::m_NmCHPort[MS896A_DAC_PORT].m_szName;
			szPort = stAxis3.m_szName + stAxis3.m_NmCHPort[MS896A_DAC_PORT].m_szName;
			pcPortList[11] = strcpy(pcPort[11], szPort);

			if ( (sResult = gmp_request_datalog("Axis Log", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_config("Axis Log", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("Axis Log", uiPort, pcPortList)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}

			if ( (sResult = gmp_enable_datalog("Axis Log")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}

			//Sleep(200);
		}
		else
		{
			if (bWriteToFile == FALSE)
			{
				gmp_disable_datalog("Axis Log");
				gmp_request_datalog("Axis Log", GMP_RELEASE);
				return TRUE;
			}

			//v3.94	//Changed NuMotion data log folder to \DataLog sub-folder in \UserData
			CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\" + szModuleID1 + "_" + szModuleID2 + "_" + szModuleID3 + "_" + "FullLog.csv";
			CString szLogAttrib = "w";
			GMP_U32 *uiData;
			GMP_U32 uiNoOfSampleUpload=0;
			UINT i, uiIndex;
			UINT uiNoOfData = 30000;
			INT siValue1, siValue2, siValue3;
		

			uiData = new GMP_U32[uiNoOfData*(uiPort+1)];
			memset(uiData,0, sizeof(uiData));

			//Stop datalog
			gmp_disable_datalog("Axis Log");
			gmp_get_datalog("Axis Log", uiData, uiNoOfData, &uiNoOfSampleUpload);
			gmp_request_datalog("Axis Log", GMP_RELEASE);

			Sleep(500);

			//Output full log file
			FILE *fFullLog = NULL;
			errno_t nErr = fopen_s(&fFullLog, szFullLog, szLogAttrib);
			if ((nErr == 0) && (fFullLog != NULL))
			{
				fprintf(fFullLog, "Number of Sample = %d\n", uiNoOfSampleUpload);
				fprintf(fFullLog, "CMD,ENC,MODE,DAC,CMD,ENC,MODE,DAC,CMD,ENC,MODE,DAC\n");

				for (i=0; i<uiNoOfSampleUpload; i++)
				{
					uiIndex = i*(uiPort+1);
					siValue1 = uiData[uiIndex+4];
					siValue2 = uiData[uiIndex+8];
					siValue3 = uiData[uiIndex+12];

					fprintf(fFullLog, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",	uiData[uiIndex+1], 
																				uiData[uiIndex+2], 
																				uiData[uiIndex+3], 
																				siValue1/NU_MOTION_DAC_RATIO,
																				uiData[uiIndex+5], 
																				uiData[uiIndex+6], 
																				uiData[uiIndex+7], 
																				siValue2/NU_MOTION_DAC_RATIO,
																				uiData[uiIndex+9], 
																				uiData[uiIndex+10], 
																				uiData[uiIndex+11], 
																				siValue3/NU_MOTION_DAC_RATIO);
				}
				fclose(fFullLog);
			}

			delete [] uiData;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
#endif
	return TRUE;
}

BOOL CMS896AStn::NuMotionDataLogForBHDebug(BOOL bEnable)
{
#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION
	try
	{
		GMP_U32 uiPort = 21;

		if ( bEnable == TRUE )
		{
			GMP_S16 sResult;
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;
			GMP_CHAR *pcPortList[21];
			GMP_CHAR pcPort[21][GMP_CH_NAME_CHAR_LEN];
			CString szPort;
			

			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_CONTINUOUS;	//GMP_ONE_SHOT;		//v4.24

			//Bond Head T
			szPort = "BONDHEAD_CHANNELT_CMDPOS_PORT";
			pcPortList[0] = strcpy(pcPort[0], szPort);

			szPort = "BONDHEAD_CHANNELT_ENC_PORT_ENC_PORT";
			pcPortList[1] = strcpy(pcPort[1], szPort);

			szPort = "BONDHEAD_CHANNELT_DAC_PORT";
			pcPortList[2] = strcpy(pcPort[2], szPort);

			szPort = "BONDHEAD_CHANNELT_CHMODE_PORT";
			pcPortList[3] = strcpy(pcPort[3], szPort);

			szPort = "BONDHEAD_CHANNELT_ERR_PORT";
			pcPortList[4] = strcpy(pcPort[4], szPort);

			szPort = "BONDHEAD_CHANNELT_DRVSTATE_PORT";
			pcPortList[5] = strcpy(pcPort[5], szPort);


			//Bond Head Z1
			szPort = "BONDHEAD_CHANNELZ1_CMDPOS_PORT";
			pcPortList[6] = strcpy(pcPort[6], szPort);

			szPort = "BONDHEAD_CHANNELZ1_ENC_PORT_ENC_PORT";
			pcPortList[7] = strcpy(pcPort[7], szPort);

			szPort = "BONDHEAD_CHANNELZ1_DAC_PORT";
			pcPortList[8] = strcpy(pcPort[8], szPort);

			szPort = "BONDHEAD_CHANNELZ1_CHMODE_PORT";
			pcPortList[9] = strcpy(pcPort[9], szPort);

			szPort = "BONDHEAD_CHANNELZ1_ERR_PORT";
			pcPortList[10] = strcpy(pcPort[10], szPort);

			szPort = "BONDHEAD_CHANNELZ1_DRVSTATE_PORT";
			pcPortList[11] = strcpy(pcPort[11], szPort);


			//Bond Head Z2
			szPort = "BONDHEAD_CHANNELZ2_CMDPOS_PORT";
			pcPortList[12] = strcpy(pcPort[12], szPort);

			szPort = "BONDHEAD_CHANNELZ2_ENC_PORT_ENC_PORT";
			pcPortList[13] = strcpy(pcPort[13], szPort);

			szPort = "BONDHEAD_CHANNELZ2_DAC_PORT";
			pcPortList[14] = strcpy(pcPort[14], szPort);

			szPort = "BONDHEAD_CHANNELZ2_CHMODE_PORT";
			pcPortList[15] = strcpy(pcPort[15], szPort);

			szPort = "BONDHEAD_CHANNELZ2_ERR_PORT";
			pcPortList[16] = strcpy(pcPort[16], szPort);

			szPort = "BONDHEAD_CHANNELZ2_DRVSTATE_PORT";
			pcPortList[17] = strcpy(pcPort[17], szPort);


			//Others
			szPort = "RAWENC_0";
			pcPortList[18] = strcpy(pcPort[18], szPort);

			szPort = "RAWENC_2";
			pcPortList[19] = strcpy(pcPort[19], szPort);

			szPort = "RAWENC_3";
			pcPortList[20] = strcpy(pcPort[20], szPort);


			if ( (sResult = gmp_request_datalog("Axis Log", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_config("Axis Log", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("Axis Log", uiPort, pcPortList)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}

			if ( (sResult = gmp_enable_datalog("Axis Log")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}

			Sleep(200);
		}
		else
		{
			//v3.94	//Changed NuMotion data log folder to \DataLog sub-folder in \UserData
			CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\" + "BH_Debug_" + "FullLog.csv";
			CString szLogAttrib = "w";
			GMP_U32 *uiData;
			GMP_U32 uiNoOfSampleUpload=0;
			UINT i, uiIndex;
			UINT uiNoOfData = 30000;
			INT siValue1, siValue2, siValue3;
		

			uiData = new GMP_U32[uiNoOfData*(uiPort+1)];
			memset(uiData,0, sizeof(uiData));

			//Stop datalog
			gmp_disable_datalog("Axis Log");
			gmp_get_datalog("Axis Log", uiData, uiNoOfData, &uiNoOfSampleUpload);
			gmp_request_datalog("Axis Log", GMP_RELEASE);

			Sleep(500);

			//Output full log file
			FILE *fFullLog = NULL;
			errno_t nErr = fopen_s(&fFullLog, szFullLog, szLogAttrib);
			if ((nErr == 0) && (fFullLog != NULL))
			{
				fprintf(fFullLog, "Number of Sample = %d\n", uiNoOfSampleUpload);
				fprintf(fFullLog, "T_CMD,T_ENC,T_DAC,T_MODE,T_ERR,T_DRVSTATE,");
				fprintf(fFullLog, "Z1_CMD,Z1_ENC,Z1_DAC,Z1_MODE,Z1_ERR,Z1_DRVSTATE,");
				fprintf(fFullLog, "Z2_CMD,Z2_ENC,Z2_DAC,Z2_MODE,Z2_ERR,Z2_DRVSTATE,");
				fprintf(fFullLog, "RAWENC_0,RAWENC_2,RAWENC_3\n");

				for (i=0; i<uiNoOfSampleUpload; i++)
				{
					uiIndex = i*(uiPort+1);
					siValue1 = uiData[uiIndex+3];
					siValue2 = uiData[uiIndex+9];
					siValue3 = uiData[uiIndex+15];

					fprintf(fFullLog, "%d,%d,%d,%d,%d,%d,",	uiData[uiIndex+1], 
															uiData[uiIndex+2], 
															siValue1/NU_MOTION_DAC_RATIO, 
															uiData[uiIndex+4], 
															uiData[uiIndex+5], 
															uiData[uiIndex+6]);

					fprintf(fFullLog, "%d,%d,%d,%d,%d,%d,",	uiData[uiIndex+7], 
															uiData[uiIndex+8], 
															siValue2/NU_MOTION_DAC_RATIO,
															uiData[uiIndex+10], 
															uiData[uiIndex+11], 
															uiData[uiIndex+12]);

					fprintf(fFullLog, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",	uiData[uiIndex+13], 
																		uiData[uiIndex+14],
																		siValue3/NU_MOTION_DAC_RATIO,
																		uiData[uiIndex+16],
																		uiData[uiIndex+17], 
																		uiData[uiIndex+18],
																		uiData[uiIndex+19],
																		uiData[uiIndex+20], 
																		uiData[uiIndex+21]);
				}
				fclose(fFullLog);
			}

			delete [] uiData;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
#endif
	return TRUE;
}

BOOL CMS896AStn::NuMotionDataLogAdv(BOOL bEnable)
{
	if (!m_bNuDataLogEnable && !bEnable)
	{
		return FALSE;
	}

	if (m_bNuDataLogEnable && bEnable)
	{
		return TRUE;
	}

	m_bNuDataLogEnable = bEnable;

	return NuMotionDataLogExt_MS100(bEnable, TRUE);
}	//	Special run time on/off data log.


BOOL CMS896AStn::NuMotionDataLogExt_MS100(BOOL bEnable, BOOL bWriteToFile)
{
	//AfxMessageBox("NuMotionDataLog3-->SUUPERMODE");
#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION
	try
	{
		GMP_U32 uiPort = 12;

		CString szLogList = gszUSER_DIRECTORY + "\\DataLog\\DatalogList.txt";

		CStdioFile cPortListFile;
		CStringList cPortList;
		CString szContent;
		UINT uiColNum = 0;

		// Check the function is turn ON

		// Check the list file wehther exist
		if ((_access(szLogList, 0 )) == -1)
		{
			return FALSE;
		}

		// Get no. of elements will be use into datalog function
		if ( cPortListFile.Open(szLogList, CFile::modeRead) == TRUE )
		{
			while ( cPortListFile.ReadString(szContent) == TRUE )
			{
				if ( szContent.GetLength() > 0) 
				{
					cPortList.AddTail(szContent);
					uiColNum++;
				}
			}
			cPortListFile.Close();
			uiPort = uiColNum;
		}
		else
		{
			return FALSE;
		}

		if ( uiColNum == 0 )
		{
			return TRUE;
		}

		//Check List size should be < 64
		//uiPort = uiColNum = (UINT)cPortList.GetSize();
		if ( (uiColNum > 64) || (uiColNum < 1) ) 
		{
			return FALSE;
		}

		if ( bEnable == TRUE )
		{
			INT j=0;	//v4.59A45
			GMP_S16 sResult;
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;
			CString szPort;
			POSITION pos;

			// new: max 64 variable column
			GMP_CHAR **pcPortList = new GMP_CHAR*[uiColNum];
			GMP_CHAR **pcPort;
			pcPort = new GMP_CHAR*[uiColNum];
			for (j = 0 ; j < (INT)uiColNum; j++)
			{
				pcPort[j] = new GMP_CHAR[GMP_CH_NAME_CHAR_LEN];
			}

			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_CONTINUOUS;	//GMP_ONE_SHOT;		//v4.24

			for ( LONG i = 0 ; i < cPortList.GetSize() ; i++ )  // Size of PortList = uiPort
			{
				if ( (pos = cPortList.FindIndex(i)) != NULL )
				{
					szPort = cPortList.GetAt(pos);
					pcPortList[i] = strcpy(pcPort[i], szPort);
				}
			}

			if ( (sResult = gmp_request_datalog("Axis Log", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_config("Axis Log", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("Axis Log", uiPort, pcPortList)) != 0 )
			{
				delete[] pcPortList;
				for (j = 0; j < (INT)uiColNum; j++)
				{
					delete[] pcPort[j];
				}
				delete[] pcPort;
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}
			delete[] pcPortList;
			for (j = 0; j < (INT)uiColNum; j++)
			{
				delete[] pcPort[j];
			}
			delete[] pcPort;

			if ( (sResult = gmp_enable_datalog("Axis Log")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}
			//Sleep(200);
		}
		else
		{
			if (bWriteToFile == FALSE)
			{
				gmp_disable_datalog("Axis Log");
				gmp_request_datalog("Axis Log", GMP_RELEASE);
				return TRUE;
			}

			//v3.94	//Changed NuMotion data log folder to \DataLog sub-folder in \UserData
			CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\Test.csv";
			CString szLogAttrib = "w";
			GMP_U32 *uiData;
			GMP_U32 uiNoOfSampleUpload=0;
			UINT i, uiIndex;
			UINT uiNoOfData = 75000;

			uiData = new GMP_U32[uiNoOfData*(uiPort+1)];
			memset(uiData,0, sizeof(uiData));

			//Stop datalog
			gmp_disable_datalog("Axis Log");
			gmp_get_datalog("Axis Log", uiData, uiNoOfData, &uiNoOfSampleUpload);
			gmp_request_datalog("Axis Log", GMP_RELEASE);

			Sleep(500);

			//Output full log file
			FILE *fFullLog = NULL;
			errno_t nErr = fopen_s(&fFullLog, szFullLog, szLogAttrib);
			if ((nErr == 0) && (fFullLog != NULL))
			{
				fprintf(fFullLog, "Number of Sample = %d\n", uiNoOfSampleUpload);
				fprintf(fFullLog, "Time,");
				for (UINT a=1; a<=uiColNum; a++)
				{
					fprintf(fFullLog, "item%d,", a);
				}
				fprintf(fFullLog, "\n");

				for (i=0; i<uiNoOfSampleUpload; i++)	// total i line
				{
					fprintf(fFullLog, "%f,", i*0.125);							// add time 
					for (UINT j=0; j<uiColNum; j++)		// total j column
					{
						uiIndex = i*(uiPort+1) + j + 1;
						fprintf(fFullLog, "%d,",uiData[uiIndex]);				// add "data(INDEX),"
					}
					fprintf(fFullLog, "\n");			// end of j'th line
				}
				fclose(fFullLog);
			}
			delete [] uiData;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
#endif
	return TRUE;
}


BOOL CMS896AStn::NuMotionDataLogExt(BOOL bEnable)
{

#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION

	CString szLogList = gszUSER_DIRECTORY + "\\DataLog\\DatalogList.txt";

	if ((_access(szLogList, 0 )) == -1)
	{
		return FALSE;
	}

	try
	{
		if ( bEnable == TRUE )
		{
			CStdioFile cPortListFile;
			CStringList cPortList;
			CString szContent;
			GMP_S16 sResult;


			//Get no. of elements will be use into datalog function
			if ( cPortListFile.Open(szLogList, CFile::modeRead) == TRUE )
			{
				while ( cPortListFile.ReadString(szContent) == TRUE )
				{
					if ( szContent.GetLength() > 0) 
					{
						cPortList.AddTail(szContent);
					}
				}
				cPortListFile.Close();
			}
			else
			{
				return FALSE;
			}
			
			//Check List size should be < 64
			m_uiMotionDataLogSize = (UINT)cPortList.GetSize();
			if ( (m_uiMotionDataLogSize > 64) || (m_uiMotionDataLogSize < 1) ) 
			{
				m_uiMotionDataLogSize = 0;
				return FALSE;
			}

			
			GMP_DATALOG_STOP_CFG stConfig;
			GMP_DATALOG_CONFIG_INFO stDatalogCofigInfo;

			char u32DatalogPortsStr[64][64];
			char *pcPortList[64];
			POSITION pos;
			
		
			for (UINT i=0; i<m_uiMotionDataLogSize; i++)
			{
				if ( (pos = cPortList.FindIndex(i)) != NULL )
				{
					pcPortList[i] = strcpy(u32DatalogPortsStr[i], cPortList.GetAt(pos));
				}
			}

			stConfig.pstLogStopCfgInfo = &stDatalogCofigInfo;
			stConfig.pstDatalogStopEvt = NULL;
			stConfig.pstLogStopCfgInfo->enDatalogMode = GMP_CONTINUOUS; //GMP_ONE_SHOT;

			if ( (sResult = gmp_request_datalog("EnaDatalog", GMP_REQUEST)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_port_entries("EnaDatalog", (GMP_U32)m_uiMotionDataLogSize, pcPortList)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_config", "");
				throw e;
			}

			if ( (sResult = gmp_set_datalog_config("EnaDatalog", &stConfig)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_set_datalog_port_entries", "");
				throw e;
			}

			if ( (sResult = gmp_enable_datalog("EnaDatalog")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_enable_datalog", "");
				throw e;
			}
		}
		else
		{
			if (m_uiMotionDataLogSize == 0)
			{
				return FALSE;
			}

			CStdioFile cDataFile;
			CreateDirectory(gszUSER_DIRECTORY + "\\DataLog\\Result", NULL);
			CString szFullLog = gszUSER_DIRECTORY + "\\DataLog\\Result\\DatalogResult.txt";
			UINT uiSampleNum = 30000;
			UINT uiColNum = m_uiMotionDataLogSize + 1;
			UINT u32NoOfSamplesUploaded = 0;
			UINT *uiData = new GMP_U32[uiColNum*uiSampleNum];
			CString szTemp;
			GMP_S16 sResult;


			//Stop datalog & retrieve data from memory
			if ( (sResult = gmp_disable_datalog("EnaDatalog")) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_disable_datalog", "");
				delete [] uiData;
				throw e;
			}

			if ( (sResult = gmp_get_datalog("EnaDatalog", uiData, (GMP_U32)uiSampleNum, &u32NoOfSamplesUploaded)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_get_datalog", "");
				delete [] uiData;
				throw e;
			}

			if ( (sResult = gmp_request_datalog("EnaDatalog", GMP_RELEASE)) != 0 )
			{
				CAsmException e((UINT)sResult, "gmp_request_datalog", "");
				delete [] uiData;
				throw e;
			}

			m_uiMotionDataLogSize = 0;

			//Delete last save file
			DeleteFile(szFullLog);
	
			//Write data into file
			if ( cDataFile.Open(szFullLog, CFile::modeCreate|CFile::modeWrite) == TRUE )
			{
				for (UINT i=0; i<u32NoOfSamplesUploaded; i++)
				{
					for(UINT j=0; j<uiColNum; j++)
					{
						szTemp.Format("%d\t", uiData[i * uiColNum + j]);
						cDataFile.WriteString(szTemp);
					}
					cDataFile.WriteString("\n");
				}
				cDataFile.Close();
			}

			delete [] uiData;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

#endif

	return TRUE;
}

//Open DAC with velocity damping
INT CMS896AStn::MotionOpenDACwithVelDamping(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, LONG lOpenDACValue, LONG lHoldTime, INT nSwPort)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

	INT nResult	= gnOK;
	int nTime	= 0;

#ifdef OFFLINE
	return gnOK;
#endif	
	try
	{
#ifndef ES101
#ifndef PROBER
#ifdef NU_MOTION
		CTRL_FUNC_RETURN stCtrlReturn;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		GMP_S32 s16OpenDAC = lOpenDACValue;
		//GMP_U32	u32BondTime = lHoldTime*NU_MOTION_SAMPLE_RATE;
		GMP_U32	u32BondTime = 0xffffffff;
		CHAR pcSwPortName[GMP_CH_NAME_CHAR_LEN];

		//Check control is exist or not
		if ( pAxisInfo->m_stControl[PL_FORCE_CONTROL].m_szID.IsEmpty() == TRUE )
		{
			return nResult;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_FORCE_CONTROL].m_szID);
		strcpy_s(pcSwPortName, sizeof(pcSwPortName), CMS896AApp::m_NmSWPort[nSwPort].m_szName);

		stCtrlReturn = agmp_ctrl_open_dac_with_velocity_damp(pcChName, pcCtrlID, s16OpenDAC, pcSwPortName, u32BondTime);
		if ( stCtrlReturn.IsSuccess() == GMP_FALSE )
		{
			pAxisInfo->m_sErrCode = stCtrlReturn.GMP_err;
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "CTR_setup_open_dac_with_velocity_damp", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			pAxisInfo->m_sErrCode = 0;
		}
#else
		nResult = gnOK;
#endif
#endif
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		nResult = gnNOTOK;
		throw e;
	}

	return nResult;
}

INT CMS896AStn::MotionSyncViaEjCmdDistance(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, CMSNmAxisInfo* pAxisInfo2, LONG lTriggerValue, LONG lZOffset)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

	INT nResult	= gnOK;
	int nTime	= 0;

#ifdef OFFLINE
	return gnOK;
#endif	
	try
	{
		CTRL_FUNC_RETURN stCtrlReturn;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcChName2[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID2[GMP_CH_NAME_CHAR_LEN];
		GMP_FLOAT* fPos = m_fFifthOrderPos_Ej;
		GMP_FLOAT* fVel = m_fFifthOrderVel_Ej;
		GMP_FLOAT* fAcc = m_fFifthOrderAcc_Ej;
		GMP_FLOAT* fJerk = m_fFifthOrderJerk_Ej;
		GMP_U32 u32EjectMoveLength = (GMP_U32) CMS896AStn::m_lEjectMoveLength;
		GMP_U32	u32ZTriggerValue = lTriggerValue;
		GMP_U8	u8SyncMotType = (GMP_U8)0;
		GMP_U8  u8TriggerMode = (GMP_U8)0;
		GMP_U32	u32ZOffset = (GMP_U32)lZOffset;

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcChName2, sizeof(pcChName2), pAxisInfo2->m_szName);
		//strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_DYNAMIC].m_szID);
		//strcpy_s(pcCtrlID2, sizeof(pcCtrlID2), pAxisInfo2->m_stControl[PL_DYNAMIC].m_szID);
		//strcpy_s(pcCtrlID, sizeof(pcCtrlID), "PL_Dynamic");
		//strcpy_s(pcCtrlID2, sizeof(pcCtrlID2), "PL_Dynamic");
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[pAxisInfo->m_ucControlID].m_szID);
		strcpy_s(pcCtrlID2, sizeof(pcCtrlID2), pAxisInfo2->m_stControl[pAxisInfo2->m_ucControlID].m_szID);

		//andrewng //2020-0601
		FLOAT fEjRes		= 3.0 / 2100.0;		//0.0014286
		FLOAT fBinEjCapRes	= 0.59 / 800.0;		//0.0007375
		GMP_FLOAT fResEj	= (GMP_FLOAT) fEjRes;
		GMP_FLOAT fResEjCap	= (GMP_FLOAT) fBinEjCapRes;

		/*
		CTRL_API AGMP_CTRL_FUNC_RETURN agmp_ctrl_execute_sync_pick_motion(GMP_CHAR  *pcZChName,							
				GMP_CHAR  *pcEjtChName,						
				GMP_CHAR  *pcCtrlStrZ,						
				GMP_CHAR  *pcCtrlStrEjt,						
				GMP_FLOAT *pos,								
				GMP_FLOAT *vel,								
				GMP_FLOAT *acc,								
				GMP_FLOAT *jerk,							
				GMP_U32	u32EjectMoveLength,					
				GMP_U32	u32ZTriggerValue,
				GMP_U8	u8SyncMotType,						
				GMP_U8	u8TriggerMode,						
				GMP_U32	u32ZPreload,						
				GMP_U8    u8SyncGroup,                         
				GMP_U8	u8LogFile = 0,							
				GMP_MOTION_ID *pstMotID = NULL,
				GMP_FLOAT ejtEncRes = 1,	
				GMP_FLOAT	ZEncRes = 1,
				GMP_BOOLEAN bEncoderDirection = GMP_TRUE);
		*/

		//stCtrlReturn = CTR_sync_motion(pcChName, pcChName2, pcCtrlID, pcCtrlID2, 
		stCtrlReturn = agmp_ctrl_execute_sync_pick_motion(pcChName, pcChName2, pcCtrlID, pcCtrlID2, 
								fPos, fVel, fAcc, fJerk, 
								u32EjectMoveLength, 
								u32ZTriggerValue,	//if u8TriggerMode = 0 , it is eject cmd distance (positive value), after eject move equal or more
													//than this value, Z begins to move, unit: cnts
													//if u8TriggerMode = 1 , it is Z delay time after eject start move, unit: sample
								u8SyncMotType,
								u8TriggerMode,
								u32ZOffset,
								1,
								0,
								NULL,
								fResEj,			//1,
								fResEjCap,		//1,
								GMP_FALSE);	
			
		
		if ( stCtrlReturn.IsSuccess() == GMP_FALSE )
		{
			pAxisInfo->m_sErrCode = stCtrlReturn.GMP_err;

			CString szErr;
			szErr.Format("NUMOTION: agmp_ctrl_execute_sync_pick_motion: GMP ERR = %d, CTM ERR = %lu,%lu,%lu,%lu", 
				stCtrlReturn.GMP_err, stCtrlReturn.CTR_err[0],
				stCtrlReturn.CTR_err[1], stCtrlReturn.CTR_err[2], stCtrlReturn.CTR_err[3]);
			SetErrorMessage(szErr);

			CAsmException e((UINT)pAxisInfo->m_sErrCode, "CTR_sync_motion", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			pAxisInfo->m_sErrCode = 0;
		}

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		nResult = gnNOTOK;
		throw e;
	}

	return nResult;
}

INT CMS896AStn::MotionSetupSyncViaEjCmdDistance(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, LONG lDistance)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

	try
	{
		CTRL_FUNC_RETURN stCtrlReturn;
		GMP_U32 u32EjectMoveLength;
		FLOAT fAmax;
		FLOAT fDmax;
		FLOAT fVmax;
		FLOAT fDist;
		FLOAT fVfinal;

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetupSyncViaEjCmdDistance: " + szAxis);
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionSetupSyncViaEjCmdDistance");
			throw e;
		}

		fAmax = (FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxAcc;
		fDmax = (FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxDec;
		fVmax = (FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dMaxVel;
		fDist = (FLOAT)lDistance;
		fVfinal = (FLOAT)pAxisInfo->m_stMoveProfile[pAxisInfo->m_ucMoveProfileID].m_dFinalVel;

		u32EjectMoveLength = Get_Fifth_Order_Profile_totalsample(fAmax, fDmax, fVmax, fDist, fVfinal);
	
		stCtrlReturn = Fifth_Order_Profile(fAmax, fDmax, fVmax,	fDist, fVfinal, 
			m_fFifthOrderPos_Ej, m_fFifthOrderVel_Ej, m_fFifthOrderAcc_Ej, m_fFifthOrderJerk_Ej);

		if ( stCtrlReturn.IsSuccess() == GMP_FALSE )
		{
			pAxisInfo->m_sErrCode = stCtrlReturn.GMP_err;
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "Fifth_Order_Profile", pAxisInfo->m_szName);
			throw e;
		}
		else
		{
			pAxisInfo->m_sErrCode = 0;
		}

		m_lEjectMoveLength = (LONG)u32EjectMoveLength;

		CString szErr;
		szErr.Format("NUMOTION: MotionSetupSyncViaEjCmdDistance: EJ MOVE LENGTH = %ld samples", m_lEjectMoveLength);
		SetErrorMessage(szErr);

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}


INT CMS896AStn::OpenDac(CMSNmAxisInfo* pAxisInfo, LONG lHoldValue, LONG lEndValue, 
						LONG lRampUpTime, LONG lHoldTime, LONG lRampDownTime)
{
	if (m_fHardware == FALSE)
	{
		return FALSE;
	}

	try
	{

#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION
		GMP_MOV_INPUT stMovInput;
		GMP_PROF stProf;                               
		GMP_PROCESS_BLK_LIST stPBL;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		//Copy ChName
		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetupSyncViaEjCmdDistance: ");	// + pAxisInfo->m_szName);	//Klocwork
			return FALSE;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionSetupSyncViaEjCmdDistance");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		stPBL.u8PBLSize = 0;
		stProf.enProfType								= OPEN_DAC_PROF;
		stMovInput.enInputType							= GMP_OPENDAC_TYPE;
		stMovInput.stOpenDacInput.u8Mode                = 0;
		stMovInput.stOpenDacInput.u8UserMotID           = 0;
		stMovInput.stOpenDacInput.s32DacHoldValue		= (GMP_S32)lHoldValue;
		stMovInput.stOpenDacInput.s32DacEndValue		= (GMP_S32)lEndValue;
		stMovInput.stOpenDacInput.dwRampUpTime          = (GMP_DWORD)lRampUpTime * NU_MOTION_SAMPLE_RATE;
		stMovInput.stOpenDacInput.dwDACHoldTime         = (GMP_DWORD)lHoldTime * NU_MOTION_SAMPLE_RATE;
		stMovInput.stOpenDacInput.dwRampDownTime		= (GMP_DWORD)lRampDownTime * NU_MOTION_SAMPLE_RATE;

		if ( (pAxisInfo->m_sErrCode = gmp_mov(pcChName, &stMovInput, &stProf, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_mov", pAxisInfo->m_szName);
			throw e;
		}
#endif

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return TRUE;
}


BOOL CMS896AStn::MotionGetMotorRMSValues(const CString &szAxis, const UINT unLevel,
										DOUBLE& dTimeInSec, DOUBLE& dRmsR, DOUBLE& dRmsS, DOUBLE& dRmsT,
										CMSNmAxisInfo *pAxisInfo)
{
	try
	{

#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION

		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		GMP_U32	uLevel = unLevel;
		GMP_DOUBLE dRmsTimeInSec = 0;
		GMP_DOUBLE dDbRsmR = 0;
		GMP_DOUBLE dDbRsmS = 0;
		GMP_DOUBLE dDbRsmT = 0;

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionGetMotorRMSValues: " + szAxis);
			return FALSE;
		}
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionGetMotorRMSValues");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		GMP_S16 sReturn = gmp_get_rms(pcChName, uLevel, &dRmsTimeInSec, &dDbRsmR, &dDbRsmS, &dDbRsmT);
		if (sReturn != GMP_SUCCESS)
		{
			return FALSE;
			//CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_get_rms", pAxisInfo->m_szName);
			//throw e;
		}

		dTimeInSec = (DOUBLE) dRmsTimeInSec;
		dRmsR = (DOUBLE) dDbRsmR;
		dRmsS = (DOUBLE) dDbRsmS;
		dRmsT = (DOUBLE) dDbRsmT;

#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//throw e;
		return FALSE;
	}

	return TRUE;
}


BOOL CMS896AStn::MotionSetMotorThermalCompDAC(const CString &szAxis, USHORT usDAC, 
											  CMSNmAxisInfo *pAxisInfo)
{
	try
	{

#ifdef OFFLINE
		return TRUE;
#endif
#ifdef NU_MOTION

		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		GMP_S16	sValue = usDAC;

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetMotorThermalCompDAC: " + szAxis);
			return FALSE;
		}
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "MotionSetMotorThermalCompDAC");
			throw e;
		}

		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

		pAxisInfo->m_sErrCode = gmp_set_thermal_compensation_dac(pcChName, sValue);
		if (pAxisInfo->m_sErrCode != GMP_SUCCESS)
		{
			//CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_set_thermal_compensation_dac", pAxisInfo->m_szName);
			//throw e;
		}

#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		//throw e;
		return FALSE;
	}

	return TRUE;
}

//v4.59A16
BOOL CMS896AStn::MotionSetThermalCtrlTimer(const CString &szAxis, CONST USHORT usTempInDegree, CMSNmAxisInfo *pAxisInfo)
{

#ifdef OFFLINE
	return TRUE;
#endif
#ifdef NU_MOTION_MS60

    GMP_S16 s16RetVal = GMP_SUCCESS;

    GMP_PROCESS_BLK stPB;
    GMP_PROC_BLK_DATA stPBData;
    GMP_PROCESS_MODE stPBMode;
    GMP_PROCESS_ACTION stPBActionT, stPBActionF;

	GMP_U32 uBlkNo		= 0;
	GMP_U32 BlkListNo	= 0;

    stPB.pstProcBlkData			= &stPBData;
    stPB.punProcessMode			= &stPBMode;
    stPB.punProcessActionT		= &stPBActionT;
    stPB.punProcessActionF		= &stPBActionF;
    stPBData.u16ProcessBlkNum	= uBlkNo;   
    stPBData.enMode				= GMP_PORT_C;		//GMP_PORT_A

	CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
	strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);

	CHAR pcSwPortName[GMP_CH_NAME_CHAR_LEN];
	//strcpy_s(pcSwPortName, sizeof(pcSwPortName), CMS896AApp::m_NmSWPort[nSwPort].m_szName);
	CString szPortName = BHT_THERMOSTAT;
	strcpy_s(pcSwPortName, sizeof(pcSwPortName), szPortName);

	/*
    stPBMode.stModePortA.enLogic		= GMP_GE;			//Greater than
    stPBMode.stModePortA.pcPortName		= pcSwPortName;
    stPBMode.stModePortA.u32Mask		= UINT_MAX;	//0xFFFFFFFF;
    stPBMode.stModePortA.u32Pattern		= (GMP_U32) usTempInDegree; 
    stPBMode.stModePortA.u8DebounceCtr	= 1;
	*/
    stPBMode.stModePortC.enLogic		= GMP_GE;
    stPBMode.stModePortC.pcPortName		= pcSwPortName;
    stPBMode.stModePortC.u32Mask		= UINT_MAX;
    stPBMode.stModePortC.s32Pattern		= (GMP_U32) usTempInDegree;
    stPBMode.stModePortC.u8DebounceCtr	= 1;


	stPBData.enProcessActionT						= GMP_PROCESS_ACTION_TYPE_CH_ABORT;
    stPBActionT.stActionChAbort.pcChName			= pcChName;
    stPBActionT.stActionChAbort.enActionAfterStop	= GMP_ACTION_OPEN_LOOP_DIS_DRV;	//GMP_ACTION_OPEN_LOOP_DELAY_DIS_DRV;
    stPBActionT.stActionChAbort.enStopType			= GMP_RAMP_STOP;
    stPBData.u16NextBlkNumT							= 0;	//GMP_END_OF_PROCESS;	//If TRUE, BH temp is overheat, ABORT motion !!!
    

    stPBData.enProcessActionF						= GMP_PROCESS_ACTION_TYPE_DO_NOTHING;	//GMP_PROCESS_ACTION_TYPE_RAMP_A;
	//stPBActionF.stActionRampA.fCurSampleVal		= 0;
    //stPBActionF.stActionRampA.fIncrement			= 1;
    //stPBActionF.stActionRampA.pcDestinationPort	= pcSwPortName;
    stPBData.u16NextBlkNumF							= uBlkNo;						//If FALSE, back to itself


	//v4.59A20
	try
	{
		pAxisInfo->m_sErrCode = gmp_set_sys_process_blk(&stPB);
		if (pAxisInfo->m_sErrCode != 0)
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_set_sys_process_blk", pAxisInfo->m_szName);
			throw e;
		}

		GMP_PROCESS_BLK_LIST stPBL;
		stPBL.rgu16BlkNum[0]	= uBlkNo;       
		stPBL.u8PBLSize			= 1;

		pAxisInfo->m_sErrCode = gmp_run_sys_pbl(BlkListNo, &stPBL);  //Start running the system PBL
		if (pAxisInfo->m_sErrCode != 0)
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_run_sys_pbl", pAxisInfo->m_szName);
			throw e;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return FALSE;
	}

#endif
	return TRUE;
}


#ifdef NU_MOTION_MS60		//v4.50A16	//compatible with MS100Plus2/3
GMP_U32 CMS896AStn::CalibratePrepickPositionForPreHeat(const CString &szAxis,CMSNmAxisInfo *pAxisInfo)
{

	try
	{
		//typedef struct
		//{
		//	GMP_U32     u32RA;                   
		//	GMP_U32     u32CountPerRev;
		//	GMP_U32		u32PolePair;
		//}GMP_SWCOM_PARAM;
		GMP_SWCOM_PARAM pCommu_Para;
		pCommu_Para.u32RA = 0;

		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionSetMotorThermalCompDAC: " + szAxis);
			return FALSE;
		}
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "CalibratePrepickPositionForPreHeat");
			throw e;
		}

		GMP_S16 ssValue = 0;
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);


		ssValue = UploadRA(pcChName, &pCommu_Para);
		return pCommu_Para.u32RA;

	}
	catch (CAsmException e)
	{
		DisplayException(e);
		return 0;
	}


	return 0;
}
#endif

BOOL CMS896AStn::IsCEMarkMotors(const CString szAxis)
{
	if (!CMS896AStn::m_bCEMark)
		return FALSE;

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->State() != IDLE_Q)
		return FALSE;

	if (szAxis == "BinGripperAxis")
		return TRUE;
	else if (szAxis == "BinGripper2Axis")
		return TRUE;
	else if (szAxis == "BinLoaderYAxis")
		return TRUE;
	else if (szAxis == "BinLoaderZAxis")
		return TRUE;

	return FALSE;
}


BOOL CMS896AStn::CheckCEMarkCoverSensors()		//v4.50A24
{
	if (m_fHardware == FALSE)
		return FALSE;
	if (!CMS896AStn::m_bCEMark)
		return FALSE;

	BOOL bFrontCover = FALSE;
	BOOL bFrontLeftCover = FALSE;
	BOOL bElevatorCover = FALSE;
	BOOL bRElevatorCover = FALSE;

	//IsFrontCoverOpen()
	BOOL bCover1 = CMS896AStn::MotionReadInputBit("iCoverSensor");
	BOOL bCover2 = CMS896AStn::MotionReadInputBit("iCoverSensor2");
	if ((bCover1 == TRUE) && (bCover2 == TRUE))
		bFrontCover = TRUE;		//Assume OPEN if TURE-FALSE reverse state is wrong
	else if ((bCover1 == FALSE) && (bCover2 == FALSE))
		bFrontCover = TRUE;		//Assume OPEN if TURE-FALSE reverse state is wrong
	else
		bFrontCover = (bCover1 && (!bCover2));
/*
	//IsFrontLeftCoverOpen()
	BOOL bFLCover1 = CMS896AStn::MotionReadInputBit("iFLeftCover");
	BOOL bFLCover2 = CMS896AStn::MotionReadInputBit("iFLeftCover2");
	if ((bFLCover1 == TRUE) && (bFLCover2 == TRUE))
		bFrontLeftCover = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
	else if ((bFLCover1 == FALSE) && (bFLCover2 == FALSE))
		bFrontLeftCover = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
	else
		bFrontLeftCover = bFLCover1 && !bFLCover2;
*/
	BOOL bElCover1 = CMS896AStn::MotionReadInputBit("ibElevatorCover");
	BOOL bElCover2 = CMS896AStn::MotionReadInputBit("ibElevatorCover2");
	if ((bElCover1 == TRUE) && (bElCover2 == TRUE))
		bElevatorCover = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
	else if ((bElCover1 == FALSE) && (bElCover2 == FALSE))
		bElevatorCover = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
	else
		bElevatorCover = bElCover1 && !bElCover2;
/*
	//IsRearElevatorCoverOpen()
	BOOL bRElCover1 = CMS896AStn::MotionReadInputBit("ibRElevatorCover");
	BOOL bRElCover2 = CMS896AStn::MotionReadInputBit("ibRElevatorCover2");

	if ((bRElCover1 == TRUE) && (bRElCover2 == TRUE))
		bRElevatorCover = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
	else if ((bRElCover1 == FALSE) && (bRElCover2 == FALSE))
		bRElevatorCover = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
	else
		bRElevatorCover = bRElCover1 && !bRElCover2;
*/
	if (bFrontCover || bFrontLeftCover || bElevatorCover || bRElevatorCover)
	{
		CString szLog;
		szLog.Format("NUMOTION Cover Sensor Error: FCover=%d, FLCover=%d, ECover=%d, RECover=%d",
				bFrontCover, bFrontLeftCover, bElevatorCover, bRElevatorCover);
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		return TRUE;
	}
	return FALSE;
}


VOID CMS896AStn::MotionUpFullSearchProfile(const CString &szAxis, const CString &szProfileID, ULONG ulMask, ULONG ulAState, 
								   DOUBLE dVelocity, LONG lDistance, DOUBLE dDriveInSpeed, ULONG ulDriveInDistance, CMSNmAxisInfo *pAxisInfo)
{
#ifdef NU_MOTION
	if (m_fHardware)
	{
		try
		{
			BOOL bProfile = FALSE;
			if (pAxisInfo == NULL)
			{
				DisplayMessage("NULL Axis Info in MotionUpFullSearchProfile: " + szAxis);
				return;
			}

			for (LONG i=0; i<UM_MAX_MOVE_PROFILE; i++)
			{
				if ( pAxisInfo->m_stSrchProfile[i].m_szID == szProfileID )
				{
					bProfile = TRUE;
					if (dVelocity != 0.0)
					{
						pAxisInfo->m_stSrchProfile[i].m_dSpeed = dVelocity;
					}
					
					if (lDistance != 0)
					{
						pAxisInfo->m_stSrchProfile[i].m_lDistance = lDistance;
					}

					if (ulDriveInDistance != 0)
					{
						pAxisInfo->m_stSrchProfile[i].m_ulDrvInDistance = ulDriveInDistance;
					}

					if (dDriveInSpeed != 0.0)
					{
						pAxisInfo->m_stSrchProfile[i].m_dDrvInSpeed = dDriveInSpeed;
					}

					pAxisInfo->m_stSrchProfile[i].m_ulMask	= ulMask;
					pAxisInfo->m_stSrchProfile[i].m_ulAState = ulAState;
				}
			}

			if ( bProfile == FALSE )
			{
				CAsmException e((UINT)0, "MotionUpFullSearchProfile", "No such Profile: " + szProfileID);
				throw e;
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			throw e;
		}
	}
#endif
}	//	update search profile


BOOL CMS896AStn::IsAxisExist(CMSNmAxisInfo &stAxis)
{
	try
	{
		if (stAxis.m_lMotorType == 0)
		{
			return FALSE;
		}
		return TRUE;
	}
	catch (CAsmException e)
	{
		return FALSE;
	}
}

BOOL CMS896AStn::CheckSIOLinkStatus(GMP_S16 ssErrorCode)
{
	if ((ssErrorCode == GMP_SIO_LINK_ERR) ||
		(ssErrorCode == GMP_FIBER_LINK_DOWN) ||
		(ssErrorCode == GMP_FIBER_NODE_POWER_DOWN))
	{
		CString szContent = "SIO Link Error!\nPlease restart the application";
//		szContent.LoadString(HMB_MS_SIO_LINK_ERROR);

		m_bIsSIOLinkFailed = TRUE;
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->IsHmiExecuted())
		{
			SetAlert_Msg(IDS_MS_NU_MOTION_ERROR, szContent);
		}
		else
		{
			AfxMessageBox("Motion Error(I): " + szContent, MB_SYSTEMMODAL | MB_ICONSTOP);
		}

		return FALSE;
	}
	
	return TRUE;
}

BOOL CMS896AStn::IsMTRHomeActive(CMSNmAxisInfo &stAxis)
{
	if ((IsAxisExist(stAxis) == FALSE) || (m_bIsSIOLinkFailed == TRUE))
	{
	    return TRUE;
	}

	try
	{
		CString szPortID;
		GMP_S16 sResult = 0;
		GMP_U32 uiResult = 0;
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];
 
		szPortID = stAxis.m_szName + "_MTRSNR_PORT";

//		CStringToCharBuffer(pcPortID, GMP_PORT_NAME_CHAR_LEN, szPortID);
		strncpy_s(pcPortID, sizeof(pcPortID), (const char*)szPortID, GMP_PORT_NAME_CHAR_LEN);
		if ((sResult = gmp_read_io_port(pcPortID, &uiResult)) != 0)
		{
			CheckSIOLinkStatus(sResult);
			CAsmException e((UINT)sResult, "gmp_read_io_port", szPortID);
			throw e;
		}
		else
		{
			if ((uiResult & GMP_HOME_MASK) > 0)
			{
				return TRUE;
			}
			return FALSE;
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}
	return FALSE;
}


//================================================================
// Function Name: 		SetMotorPower
// Input arguments:		szAxis, pAxisInfo, bPowerOn 
// Output arguments:	None
// Description:   		Turn On/Off Motor
// Return:				TRUE -- OK, FALSE -- Failure
// Remarks:				None
//================================================================
INT CMS896AStn::SetMotorPower(const CString& szAxis, CMSNmAxisInfo* pAxisInfo, BOOL bPowerOn)
{
	INT nResult = gnOK;

	if (!m_fHardware)
	{
		return nResult;
	}

	try
	{
		if (bPowerOn)
		{
			MotionPowerOn(szAxis, pAxisInfo);	
		}
		else
		{
			MotionPowerOff(szAxis, pAxisInfo);
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		MotionCheckResult(szAxis, pAxisInfo);
		nResult = gnNOTOK;
	}
	return nResult;
}


short CMS896AStn::SetProcessProf(CMSNmAxisInfo* pAxisInfo, short sSaveID, unsigned short usBlkNum, unsigned short usNextBlk, int nTime,
								  char *szDestination, unsigned int unOutput, unsigned int unMask, byte ucRelTimeMode)
{
	if (ucRelTimeMode)
	{
		pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->enMode = GMP_TIME_B;
		pAxisInfo->m_astProcessBlk[sSaveID].punProcessMode->stModeTimeB.u32RelTime = abs(nTime);
	}
	else if (nTime >= 0)
	{
		pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->enMode = GMP_TIME_A;
		pAxisInfo->m_astProcessBlk[sSaveID].punProcessMode->stModeTimeA.u32AbsTime = nTime;
	}
	else
	{
		pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->enMode = GMP_TIME_C;
		pAxisInfo->m_astProcessBlk[sSaveID].punProcessMode->stModeTimeC.u32AbsTime = abs(nTime);
	}
	pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->u16ProcessBlkNum = usBlkNum;
	pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->enProcessActionF = GMP_PROCESS_ACTION_TYPE_DO_NOTHING;
	pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->enProcessActionT = GMP_PROCESS_ACTION_TYPE_PATTERN;
	pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->u16NextBlkNumF = usBlkNum;
	pAxisInfo->m_astProcessBlk[sSaveID].pstProcBlkData->u16NextBlkNumT = usNextBlk;

	pAxisInfo->m_astProcessBlk[sSaveID].punProcessActionT->stActionPattern.enLogicalOperation = GMP_BY_PASS;
	pAxisInfo->m_astProcessBlk[sSaveID].punProcessActionT->stActionPattern.enUnmaskedBitHandle = GMP_KEEP;
	pAxisInfo->m_astProcessBlk[sSaveID].punProcessActionT->stActionPattern.pcDestinationPort = szDestination;
	pAxisInfo->m_astProcessBlk[sSaveID].punProcessActionT->stActionPattern.u32OutputPattern = unOutput;
	pAxisInfo->m_astProcessBlk[sSaveID].punProcessActionT->stActionPattern.u32OutputMask = unMask;

	CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
	strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
	return gmp_set_process_blk(pcChName, &pAxisInfo->m_astProcessBlk[sSaveID]);
}


void CMS896AStn::SetProcessList(CMSNmAxisInfo* pAxisInfo, short sSaveID, byte ucSize, unsigned short *psSaveIDList)
{
	pAxisInfo->m_astPBL[sSaveID].u8PBLSize		= ucSize;
	for (unsigned short i = 0; i < ucSize; i++)
	{
		pAxisInfo->m_astPBL[sSaveID].rgu16BlkNum[i] = pAxisInfo->m_astProcessBlk[psSaveIDList[i]].pstProcBlkData->u16ProcessBlkNum;
	}
}


LONG CMS896AStn::AgmpMotionHealthCheck(CMSNmAxisInfo* pAxisInfo, CString &szMHCResult, BOOL IsMHCTrained)
{
	//CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
	//strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
	//CString szText;
	//GMP_U32 u32RetVal = GMP_SUCCESS;

	//CString szPath = "C:\\system\\control\\MHC\\BONDHEAD_CHANNELT\\MotionHealthTest_0\\MonitoredIndex\\MHC_BONDHEAD_CHANNELT_MotionHealthTest_0_Monitored_Index.zip";
	//if (_access(szPath, 0) != -1)
	//{
	//	DeleteFile(szPath);
	//}
	//szPath = "C:\\system\\control\\MHC\\BONDHEAD_CHANNELT\\MotionHealthTest_1\\MonitoredIndex\\MHC_BONDHEAD_CHANNELT_MotionHealthTest_1_Monitored_Index.zip";
	//if (_access(szPath, 0) != -1)
	//{
	//	DeleteFile(szPath);
	//}

 //   // User would choose to train model or the model is already trained
	//GMP_BOOLEAN bIsModelTrained = GMP_TRUE;
	//if (!IsMHCTrained)
	//{
	//	bIsModelTrained = GMP_FALSE;
	//}

 //   // MHM class instance
	//CAgmpCtrlMotionHealthCheck &stMotionHealthCheck = stMotionHealthCheck.GetInstance();

	//// Set output directory
	//u32RetVal = stMotionHealthCheck.SetOutputDir("C:\\system\\control\\MHC", GMP_TRUE);
	//if (u32RetVal != GMP_SUCCESS)
	//{
	//	return u32RetVal;
	//}

	//// Load xml config file
	//u32RetVal = stMotionHealthCheck.LoadConfig("C:\\system\\control\\MHC\\MHCConfig.xml", GMP_TRUE);
	//if (u32RetVal != GMP_SUCCESS)
	//{
	//	return u32RetVal;
	//}

	//if (!bIsModelTrained)
	//{
	//	// Train all test item
	//	u32RetVal = stMotionHealthCheck.TrainAll(GMP_TRUE);
	//	if (u32RetVal != GMP_SUCCESS)
	//	{
	//		return u32RetVal;
	//	}
	//}

	////Start monitoring
	//u32RetVal = stMotionHealthCheck.StartMonitoring(GMP_TRUE);
	//if (u32RetVal != GMP_SUCCESS)
	//{
	//	return u32RetVal;
	//}

 //   //Available time
	//Sleep(5000);

	//u32RetVal = stMotionHealthCheck.StopMonitoring();
	//if (u32RetVal != GMP_SUCCESS)
	//{
	//	return u32RetVal;
	//}

 //   // Get index
	//std::vector<AGMP_CTRL_MOTION_HEALTH_CHECK_INDEX> vstIndex;
	//stMotionHealthCheck.GetIndex(vstIndex);

	//CString sztext;
	//CStdioFile oFile;
	//szPath = gszUSER_DIRECTORY + "\\MHCRecord.txt";
	//if (_access(szPath, 0) == -1)
	//{
	//	oFile.Open(szPath, CFile::modeCreate|CFile::modeWrite);
	//	szText = "Porfile 1 RInductance,Porfile 1 SInductance,Porfile 2 RInductance,Porfile 2 SInductance\n";
	//	oFile.WriteString(szText);
	//}
	//else
	//{
	//	oFile.Open(szPath, CFile::modeWrite);
	//}
	//oFile.SeekToEnd();
	//for(int j = 0; j < vstIndex[0].vstTestListIndex.size(); j++)
	//{
	//	if (j == 0)
	//	{
	//		szText.Format("%4.5f,%4.5f,",
	//		vstIndex[0].vstTestListIndex[j].stMotionHealthTestIndex.stSystemIDIndex.fRInductance,
	//		vstIndex[0].vstTestListIndex[j].stMotionHealthTestIndex.stSystemIDIndex.fSInductance);
	//	}
	//	else
	//	{
	//		szText.Format("%4.5f,%4.5f\n",
	//		vstIndex[0].vstTestListIndex[j].stMotionHealthTestIndex.stSystemIDIndex.fRInductance,
	//		vstIndex[0].vstTestListIndex[j].stMotionHealthTestIndex.stSystemIDIndex.fSInductance);
	//	}
	//	oFile.WriteString(szText);
	//	
	//	sztext.Format("(%4.5f,%4.5f), ", 
	//		vstIndex[0].vstTestListIndex[j].stMotionHealthTestIndex.stSystemIDIndex.fRInductance, 
	//		vstIndex[0].vstTestListIndex[j].stMotionHealthTestIndex.stSystemIDIndex.fSInductance);
	//	szMHCResult = szMHCResult + sztext;
	//}

	//oFile.Close();

	//if(u32RetVal != GMP_SUCCESS)
	//{
	//	return u32RetVal;
	//}

	return 1;
}


INT CMS896AStn::MotionLeaveHomeSensor(const CString& szAxis, CMSNmAxisInfo* pAxisInfo)
{
	INT nResult			= gnOK;
	
	BOOL bHomeSnr = MotionIsHomeSensorHigh(szAxis,	pAxisInfo);

	if (!bHomeSnr)
	{
		return gnAMS_OK;
	}

	try
	{
#ifdef NU_MOTION
		GMP_MOV_INPUT stMoveInput;
		GMP_EVT_OBJECT stSrhHome;
		GMP_PROCESS_BLK_LIST stPBL;
		GMP_U32 uiBitPos=0;
		GMP_U8 ucStartSrchDir = GMP_POSITIVE_SRH_DIR;
		CString szPortID;
		CHAR pcChName[GMP_CH_NAME_CHAR_LEN];
		CHAR pcCtrlID[GMP_CH_NAME_CHAR_LEN];
		CHAR pcPortID[GMP_PORT_NAME_CHAR_LEN];

		if (pAxisInfo == NULL)
		{
			DisplayMessage("NULL Axis Info in MotionMoveHome: " + szAxis);
			return FALSE;
		}

		for (LONG i=0; i<UM_MAX_SI_PORT; i++)
		{
			for (LONG j=0; j<UM_MAX_IO_BIT; j++)
			{
				if ( CMS896AApp::m_NmSIPort[i].m_szBit[j] == pAxisInfo->m_stSensor.m_szHome )
				{
					szPortID = CMS896AApp::m_NmSIPort[i].m_szName;
					uiBitPos = (GMP_U32)j;
					break;
				}
			}
		}			

		if ( szPortID.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No Home sensor", pAxisInfo->m_szName);
			throw e;
		}

		//Copy ChName
		if ( pAxisInfo->m_szName.IsEmpty() == TRUE )
		{
			CAsmException e((UINT)0, "No such ch name", "SearchHome");
			throw e;
		}
		strcpy_s(pcChName, sizeof(pcChName), pAxisInfo->m_szName);
		strcpy_s(pcCtrlID, sizeof(pcCtrlID), pAxisInfo->m_stControl[PL_STATIC].m_szID);
		strcpy_s(pcPortID, sizeof(pcPortID), szPortID);

		//Check condition
		if ( pAxisInfo->m_stSrchProfile[0].m_lDistance < 0 )
		{
			ucStartSrchDir = GMP_NEGATIVE_SRH_DIR;
		}
		else
		{
			ucStartSrchDir = GMP_POSITIVE_SRH_DIR;
		}

		ucStartSrchDir = (ucStartSrchDir == GMP_POSITIVE_SRH_DIR) ? GMP_NEGATIVE_SRH_DIR : GMP_POSITIVE_SRH_DIR;

		stPBL.u8PBLSize								= 0;

		stMoveInput.enInputType						= GMP_SEARCH_TYPE;
		stMoveInput.pcCtrlType						= pcCtrlID;
		stMoveInput.stSrhInput.u8SrhDir				= ucStartSrchDir;
		stMoveInput.stSrhInput.bEnaSetPosn			= GMP_FALSE;
		stMoveInput.stSrhInput.s32DesiredAbsPosn	= 0;
		stMoveInput.stSrhInput.u8UserMotID			= 1;

		stSrhHome.enDrvInType						= GMP_EVT_DRIVE_IN_DEFAULT;
		stSrhHome.enType							= GMP_EVT_SRH_PORT_AND;
		stSrhHome.enLmtType							= GMP_EVT_LMT_DIST; 
		stSrhHome.bEnaErrChk						= GMP_FALSE;
		stSrhHome.u16SrhDebounceSmpl				= 0;           
		stSrhHome.fSrhDrvInVel						= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[0].m_dDrvInSpeed;
		stSrhHome.u32DrvInCnt						= (GMP_U32)pAxisInfo->m_stSrchProfile[0].m_ulDrvInDistance;
		stSrhHome.fSrhVel							= (GMP_FLOAT)pAxisInfo->m_stSrchProfile[0].m_dSpeed;
		stSrhHome.u32SrhLmt							= (GMP_U32)(abs(pAxisInfo->m_stSrchProfile[0].m_lDistance));
		stSrhHome.pcPort1							= pcPortID;
		stSrhHome.u32Mask							= 1<<uiBitPos;
		stSrhHome.u32ActiveState					= 1<<uiBitPos;

		if ( stSrhHome.u32ActiveState == 0 )
		{
			stSrhHome.u32ActiveState = 1<<uiBitPos;
		}
		else
		{
			stSrhHome.u32ActiveState = 0;
		}

		if ( stSrhHome.fSrhDrvInVel > stSrhHome.fSrhVel )
		{
			stSrhHome.fSrhDrvInVel	= stSrhHome.fSrhVel;
		}

		//Power On motor before call search motion
		CMS896AStn::MotionPowerOn(szAxis, pAxisInfo);

		//Search 1st direction
		if ( (pAxisInfo->m_sErrCode = gmp_srh(pcChName, &stMoveInput, &stSrhHome, &stPBL, GMP_NON_SYNC_GROUP, &pAxisInfo->m_stMotionID)) != 0 )
		{
			CAsmException e((UINT)pAxisInfo->m_sErrCode, "gmp_srh(Home)", pAxisInfo->m_szName);
			throw e;
		}
		Sleep(250);

		MotionSync(szAxis, 20000, pAxisInfo);
		Sleep(250);

		return gnAMS_OK;

#else
		nResult = MoveHome(szAxis);
#endif
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		throw e;
	}

	return nResult;
}