/////////////////////////////////////////////////////////////////
// WT_Command.cpp : HMI Registered Command of the CWaferTable class
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
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "HmiDataManager.h"
#include "WT_CmdName.h"
#include "MS_SecCommConstant.h"
#include "StrInputDlg.h"
// prescan relative code
#include "PrescanUtility.h"
#include "WaferPr.h"
//#include "StrHdrInputDlg2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef struct {
	LONG lX;
	LONG lY;
	LONG lT;
} WT_XYT_STRUCT;

typedef struct {
	LONG lX;
	LONG lY;
	DOUBLE dT;
} WT_XYT_STRUCT2;

typedef struct {
	LONG lX;
	LONG lY;
} WT_XY_STRUCT;

typedef struct {
	LONG lX;
	LONG lY;
	LONG lUnload;
} WT_XY_UNLOAD_STRUCT;


VOID CWaferTable::RegisterVariables2()
{
	CString szText;

	try
	{
		RegVariable(_T("WT_nWTInUse"),					&m_nWTInUse);	
		RegVariable(_T("WT_lWT2OffsetX"),				&m_lWT2OffsetX);	
		RegVariable(_T("WT_lWT2OffsetY"),				&m_lWT2OffsetY);	
		RegVariable(_T("WT_lEjtOffsetX"),				&m_lEjtOffsetX);
		RegVariable(_T("WT_lEjtOffsetY"),				&m_lEjtOffsetY);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T1_MoveToCmd"),				&CWaferTable::T1_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T2_MoveToCmd"),				&CWaferTable::T2_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY1_MoveToCmd"),			&CWaferTable::XY1_MoveToCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY1_MoveCmd"),				&CWaferTable::XY1_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_MoveCmd"),				&CWaferTable::XY2_MoveCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetWT1EncoderCmd"),			&CWaferTable::GetWT1EncoderCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("GetWT2EncoderCmd"),			&CWaferTable::GetWT2EncoderCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("SetJoystickLimitCmd2"),		&CWaferTable::SetJoystickLimitCmd2);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("X2_SyncCmd"),				&CWaferTable::X2_SyncCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("T2_SyncCmd"),				&CWaferTable::T2_SyncCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("Y2_SyncCmd"),				&CWaferTable::Y2_SyncCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("XY2_SyncCmd"),				&CWaferTable::XY2_SyncCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoRotateWT"),				&CWaferTable::AutoRotateWT);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("AutoRotateWafer2"),			&CWaferTable::AutoRotateWafer2);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachWT2OffsetXY"),			&CWaferTable::TeachWT2OffsetXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TeachNGPickOffsetXY"),		&CWaferTable::TeachNGPickOffsetXY);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("TestNGPick"),				&CWaferTable::TestNGPick);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ResetWTInUse"),				&CWaferTable::ResetWTInUse);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveTable1ToEjtOffset"),	&CWaferTable::MoveTable1ToEjtOffset);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("MoveTable1ToHome"),			&CWaferTable::MoveTable1ToHome);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("UpdateWaferDiameter"),		&CWaferTable::UpdateWaferDiameter);
		
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
}

LONG CWaferTable::XY1_MoveToCmd(IPC_CServiceMessage& svMsg)	// no wafer limit check
{
	WT_XY_STRUCT stPos;
	BOOL bResult = TRUE;
	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);
	

	if (XY_IsPowerOff())
	{
		HmiMessage("Wafer Table XY not power ON!");
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	LONG lX = stPos.lX;
	LONG lY = stPos.lY;

	if ( (lX < m_lXNegLimit) || (lX > m_lXPosLimit) )
	{
CString szErr;
szErr.Format("X exceeds limit: %ld (%ld, %ld)", lX, m_lXNegLimit, m_lXPosLimit);
AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}

	if ( (lY < m_lYNegLimit) || (lY > m_lYPosLimit) )
	{
CString szErr;
szErr.Format("Y exceeds limit: %ld (%ld, %ld)", lY, m_lYNegLimit, m_lYPosLimit);
AfxMessageBox(szErr, MB_SYSTEMMODAL);
	}


	INT nResult = 0;
	nResult = X1_Profile(LOW_PROF);
	nResult = Y1_Profile(LOW_PROF);
	if (nResult != gnOK)
	{
		bResult = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (XY1_MoveTo(stPos.lX, stPos.lY, SFM_WAIT) == gnOK)		// Move table
		bResult = TRUE;											// Get the encoder value from hardware
	else
		bResult = FALSE;		//v3.60

	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);

	bResult = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bResult);

	return 1;
}

LONG CWaferTable::XY1_MoveCmd(IPC_CServiceMessage& svMsg)
{
	WT_XY_STRUCT stPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);

	X1_Sync();
	Y1_Sync();

	XY_Move(stPos.lX, stPos.lY, SFM_WAIT, FALSE);	// Move table

	GetEncoderValue();		// Get the encoder value from hardware
	stEnc.lX = m_lEnc_X1;
	stEnc.lY = m_lEnc_Y1;
	stEnc.lT = m_lEnc_T1;

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

LONG CWaferTable::XY2_MoveCmd(IPC_CServiceMessage& svMsg)
{
	WT_XY_STRUCT stPos;
	WT_XYT_STRUCT stEnc;
	svMsg.GetMsg(sizeof(WT_XY_STRUCT), &stPos);

	X2_Sync();
	Y2_Sync();

	XY_Move(stPos.lX, stPos.lY, SFM_WAIT, TRUE);	// Move table

	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = m_lEnc_X2;
	stEnc.lY = m_lEnc_Y2;
	stEnc.lT = m_lEnc_T2;
	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

LONG CWaferTable::T1_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	INT nResult = T1_MoveTo(lPos, SFM_WAIT);	// Move table

	svMsg.InitMessage(sizeof(INT), &nResult);

	return 1;
}

LONG CWaferTable::T2_MoveToCmd(IPC_CServiceMessage& svMsg)
{
	LONG lPos;
	svMsg.GetMsg(sizeof(LONG), &lPos);

	INT nResult = T2_MoveTo(lPos, SFM_WAIT);	// Move table

	svMsg.InitMessage(sizeof(INT), &nResult);

	return 1;
}

LONG CWaferTable::GetWT1EncoderCmd(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT stEnc;
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = m_lEnc_X1;
	stEnc.lY = m_lEnc_Y1;
	stEnc.lT = m_lEnc_T1;

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}

LONG CWaferTable::GetWT2EncoderCmd(IPC_CServiceMessage& svMsg)
{
	WT_XYT_STRUCT stEnc;
	GetEncoderValue();		// Get the encoder value from hardware

	stEnc.lX = m_lEnc_X2;
	stEnc.lY = m_lEnc_Y2;
	stEnc.lT = m_lEnc_T2;

	svMsg.InitMessage(sizeof(WT_XYT_STRUCT), &stEnc);

	return 1;
}


LONG CWaferTable::SetJoystickLimitCmd2(IPC_CServiceMessage &svMsg)
{
	BOOL	bMax, bOK=TRUE;
	svMsg.GetMsg(sizeof(BOOL), &bMax);

	try
	{
		if (bMax == TRUE)
		{
			//Enlarge joystick limit when teach wafer limit
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X2, WT_JOY_MAXLIMIT_NEG_X, WT_JOY_MAXLIMIT_POS_X, &m_stWTAxis_X2);		
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y2, WT_JOY_MAXLIMIT_NEG_Y, WT_JOY_MAXLIMIT_POS_Y, &m_stWTAxis_Y2);
		}
		else
		{
			//Restore original limit 
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_X2, GetCircleWaferMinX(), GetCircleWaferMaxX(), &m_stWTAxis_X2);		
			CMS896AStn::MotionSetJoystickPositionLimit(WT_AXIS_Y2, GetCircleWaferMinY(), GetCircleWaferMaxY(), &m_stWTAxis_Y2);		
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
		CMS896AStn::MotionCheckResult(WT_AXIS_X2, &m_stWTAxis_X2);
		CMS896AStn::MotionCheckResult(WT_AXIS_Y2, &m_stWTAxis_Y2);
		bOK = FALSE;
	}	

	svMsg.InitMessage(sizeof(BOOL), &bOK);

	return 1;
}

// Command for Sync Motor X
LONG CWaferTable::X2_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	try
	{
		if( X2_Sync()!=gnOK )
			bReturn = FALSE;
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

LONG CWaferTable::T2_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	try
	{
		if( T2_Sync()!=gnOK )
			bReturn = FALSE;
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

// Command for Sync Motor Y
LONG CWaferTable::Y2_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	try
	{
		Y2_Sync();
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}
	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

//V2.83T4 Sync X & Sync Y
LONG CWaferTable::XY2_SyncCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	try
	{
		X2_Sync();
		Y2_Sync();
	}
	catch (CAsmException e)
	{
		bReturn = FALSE;
	}

	svMsg.InitMessage(sizeof(BOOL),&bReturn);
	return 1;
}

LONG CWaferTable::AutoRotateWT(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	INT nPos;
	BOOL b4WT2 = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &b4WT2);
	if( IsESDualWT()==FALSE )
		b4WT2 = FALSE;

	nPos = (INT)((90 * m_ucAutoWaferRotation) / m_dThetaRes);
	if( b4WT2 )
		T2_MoveTo(nPos, SFM_WAIT);
	else
		T1_MoveTo(nPos, SFM_WAIT);	// Move table

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::ManualRotateWaferTheta2(IPC_CServiceMessage &svMsg)
{
	typedef struct {
		LONG lDirection;
		LONG lRelPos;
		BOOL bUsingAngle;
	} ROT_THETA;


	BOOL bReturn = TRUE;
	ROT_THETA stPos;
	LONG lRelPos = 0;

	svMsg.GetMsg(sizeof(ROT_THETA), &stPos);
	BOOL bBinLoader = IsBLEnable();		// DL/DLA or STD config?	//v3.00T1
	BOOL bWaferLoader	= (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"];

	//v3.48	//CHeck Wexpander lock
	if (!CheckWExpander2Lock())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		SetErrorMessage("Manual Rotate Wafer T fail because Expander not lock");
		return 1;
	}


	if (stPos.bUsingAngle == TRUE)
		lRelPos = (LONG) (stPos.lRelPos / m_dThetaRes);
	else
		lRelPos = (LONG) stPos.lRelPos;

	if ( stPos.lDirection == 1 )
	{
		if ( bBinLoader || bWaferLoader )
			T2_Move(lRelPos, SFM_WAIT);	// Move POS direction (clock-wise in DL)
		else
			T2_Move(-lRelPos, SFM_WAIT);	// Move NEG direction (anti clock-wise in DL)
	}
	else
	{
		if ( bBinLoader || bWaferLoader )
			T2_Move(-lRelPos, SFM_WAIT);	// Move NEG direction (anti clock-wise in DL)
		else
			T2_Move(lRelPos, SFM_WAIT);	// Move POS direction (clock-wise in DL)
	}
	
	GetEncoderValue();		// Get the encoder value from hardware
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

// For Rotating the wafer
LONG CWaferTable::AutoRotateWaferT2(BOOL bKeepBarcodeAngle)
{
	INT nBarcodePos = 0;
	LONG lRotateTime = 0;

	if ( IsESDualWT()==FALSE )
	{
		return 0;
	}

	GetEncoderValue();
	if (bKeepBarcodeAngle == TRUE)
	{
		nBarcodePos = (INT)GetCurrT2();
	}

	int nPos = (INT)((90 * m_ucAutoWaferRotation) / m_dThetaRes) + nBarcodePos;
	LONG lDist = nPos - GetCurrT2();
	lRotateTime = T2_ProfileTime(m_nProfile_T2, lDist, lDist);
	T2_MoveTo(nPos, SFM_NOWAIT);

	return lRotateTime;
}

LONG CWaferTable::AutoRotateWafer2(IPC_CServiceMessage& svMsg)
{
	BOOL bKeepBarcodeAngle = FALSE;

	svMsg.GetMsg(sizeof(BOOL), &bKeepBarcodeAngle);

	LONG lRotateTime = AutoRotateWaferT2(bKeepBarcodeAngle);

	svMsg.InitMessage(sizeof(LONG), &lRotateTime);
	return 1;
}

LONG CWaferTable::TeachWT2OffsetXY(IPC_CServiceMessage &svMsg)				//v4.24T10
{
	BOOL bReturn = TRUE;

	if ( IsESDualWT()==FALSE )
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//1. MOve WT1 to ref pos
	SetJoystickOn(FALSE);			//Off joystick before move
	Sleep(100);
	HomeTable2();
	T1_MoveTo(0, SFM_WAIT);

	X1_Profile(LOW_PROF1);
	Y1_Profile(LOW_PROF1);
	XY1_MoveTo(m_lWaferCalibX, m_lWaferCalibY);
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
	m_bCheckWaferLimit = FALSE;	
	SetJoystickOn(TRUE, FALSE);		//On joystick

	CString szContent = "Please move WT1 XY to reference position.";
	LONG lReturn = HmiMessageEx(szContent, "Learn WT2 Offset XY", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	if (lReturn != glHMI_YES)
	{
		SetJoystickOn(FALSE);	
		m_bCheckWaferLimit = TRUE;	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);			//Off joystick before move
	Sleep(100);
	GetEncoderValue();	
	LONG lX1 = m_lEnc_X1;
	LONG lY1 = m_lEnc_Y1;


	//2. MOve WT2 to ref pos
	HomeTable1();
	T2_MoveTo(0, SFM_WAIT);

	X2_Profile(LOW_PROF1);
	Y2_Profile(LOW_PROF1);
	XY2_MoveTo(m_lWaferCalibX2, m_lWaferCalibY2);
	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);
	m_bCheckWaferLimit = FALSE;	
	SetJoystickOn(TRUE, TRUE);		//On WT2 joystick 

	szContent = "Please move WT2 XY to same reference position.";
	lReturn = HmiMessageEx(szContent, "Learn WT2 Offset XY", glHMI_MBX_YESNO, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
	if (lReturn != glHMI_YES)
	{
		SetJoystickOn(FALSE);	
		m_bCheckWaferLimit = TRUE;	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);			//Off joystick before move
	Sleep(100);
	GetEncoderValue();	
	LONG lX2 = m_lEnc_X2;
	LONG lY2 = m_lEnc_Y2;


	//3. Calculate Offset XY
	LONG lOffsetX = lX2 - lX1;
	LONG lOffsetY = lY2 - lY1;

	szContent.Format("WT2 Offset X = %ld (OLD = %ld), Y = %ld (OLD = %ld); confirm?", 
						lOffsetX, m_lWT2OffsetX, lOffsetY, m_lWT2OffsetY);
	lReturn = HmiMessage(szContent, "Learn WT2 Offset XY", glHMI_MBX_YESNO);
	if (lReturn != glHMI_YES)
	{
		m_bCheckWaferLimit = TRUE;	
		HomeTable2();
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_lWT2OffsetX = lOffsetX;
	m_lWT2OffsetY = lOffsetY;

	SaveWaferTblData();
	HomeTable2();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::TeachNGPickOffsetXY(IPC_CServiceMessage &svMsg)			//v4.24T10
{
	BOOL bReturn = TRUE;

	if( !IsES101() && !IsES201() )
	{
		AfxMessageBox("WT: TeachNGPickOffsetXY: not ES101 config!", MB_SYSTEMMODAL);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	/****/ CMSLogFileUtility::Instance()->MS_LogOperation("TeachNGPickOffsetXY: Start");	/****/ 

	//1. MOve WT1 to ref pos
	SetJoystickOn(FALSE);			//Off joystick before move
	Sleep(100);
	HomeTable2();
	T1_MoveTo(0, SFM_WAIT);

CString szLog;
szLog.Format("1. move WT1 to CAL pos   %ld  %ld", m_lWaferCalibX, m_lWaferCalibY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	X1_Profile(LOW_PROF);
	Y1_Profile(LOW_PROF);
	XY1_MoveTo(GetWft1CenterX(), GetWft1CenterY());
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
	m_bCheckWaferLimit = FALSE;	
	SetJoystickOn(TRUE);			//On joystick

	CString szContent = "Please move WT1 XY to reference position for EJ moves UP.";
	LONG lReturn = HmiMessageEx(szContent, "Learn NGPIck Offset XY", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn != glHMI_YES)
	{
		SetJoystickOn(FALSE);	
		m_bCheckWaferLimit = TRUE;	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);			//Off joystick before move
	Sleep(100);
	GetEncoderValue();	
	LONG lX1 = m_lEnc_X1;
	LONG lY1 = m_lEnc_Y1;


	//2. EJ moves UP then DOWN to put a mark on myler sheet
	


	//3. EJ moves UP then DOWN to put a mark on myler sheet
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;
	bReturn = FALSE;
	BOOL bStart	= FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bStart);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "StartEjElevator", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!bReturn)
	{
		HmiMessage("Error in calling StartEjElevator");
		SetJoystickOn(FALSE);	
		m_bCheckWaferLimit = TRUE;	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}


	//4. MOve WT1 to ref pos under wafer optics
szLog.Format("3. move WT1 to  %ld  %ld", m_lWaferCalibX, m_lWaferCalibY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);

	X1_Profile(LOW_PROF);
	Y1_Profile(LOW_PROF);
	XY1_MoveTo(GetWft1CenterX(), GetWft1CenterY());
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);
	m_bCheckWaferLimit = TRUE;	
	MoveES101BackLightZUpDn(TRUE);	
	SetJoystickOn(TRUE);		

	szContent = "Please move WT1 XY to same reference position under the wafer optics center.";
	lReturn = HmiMessageEx(szContent, "Learn NGPIck Offset XY", glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn != glHMI_YES)
	{
		SetJoystickOn(FALSE);	
		MoveES101BackLightZUpDn(FALSE);	
		m_bCheckWaferLimit = TRUE;	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	SetJoystickOn(FALSE);			//Off joystick before move
	MoveES101BackLightZUpDn(FALSE);	
	//Sleep(100);
	m_bCheckWaferLimit = TRUE;	
	GetEncoderValue();	
	LONG lX2 = m_lEnc_X1;
	LONG lY2 = m_lEnc_Y1;


	//5. Calculate Offset XY
	LONG lOffsetX = lX1 - lX2;
	LONG lOffsetY = lY1 - lY2;

	szContent.Format("NGPick Offset X = %ld (OLD = %ld), Y = %ld (OLD = %ld); confirm?", 
						lOffsetX, m_lEjtOffsetX, lOffsetY, m_lEjtOffsetY);
	lReturn = HmiMessage(szContent, "Learn NGPIck Offset XY", glHMI_MBX_YESNO);
	if (lReturn != glHMI_YES)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	m_lEjtOffsetX = lOffsetX;
	m_lEjtOffsetY = lOffsetY;

	SaveWaferTblData();
	/****/ CMSLogFileUtility::Instance()->MS_LogOperation("TeachNGPickOffsetXY: Done");	/****/ 

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::TestNGPick(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	IPC_CServiceMessage stMsg;
	INT	nConvID = 0;


	if( !IsES101() && !IsES201() )
	{
		AfxMessageBox("WT: TestNGPick: not ES101 config!", MB_SYSTEMMODAL);
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//v4.30		//Enable NGPick for WT2
	//if (IsWT2InUse())
	//{
	//	AfxMessageBox("WT: TestNGPick: Table 1 is not IN-USE!", MB_SYSTEMMODAL);
	//	bReturn = FALSE;
	//	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	//	return 1;
	//}

	MoveES101BackLightZUpDn(TRUE);	
	SetJoystickOn(TRUE);
	LONG lReturn = HmiMessageEx("Perform NGPick on current die at PR center?", "NGPick Test",
					glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, 0, 400, 300);
	if (lReturn != glHMI_YES)
	{
		MoveES101BackLightZUpDn(FALSE);	
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	/****/ CMSLogFileUtility::Instance()->MS_LogOperation("TestNGPick: Start");	/****/ 


	//0. Search normal die if available under camera for NGPIck
	SetJoystickOn(FALSE);	//Off joystick before move
	Sleep(10);
	GetEncoderValue();	
	LONG lCurrX=0, lCurrY=0;
	if (IsWT2InUse())
	{
		lCurrX = m_lEnc_X2;
		lCurrY = m_lEnc_Y2;
	}
	else
	{
		lCurrX = m_lEnc_X1;
		lCurrY = m_lEnc_Y1;
	}

	REF_TYPE	stInfo;
	SRCH_TYPE	stSrchInfo;
	stSrchInfo.bShowPRStatus	= FALSE;
	stSrchInfo.bNormalDie		= TRUE;
	stSrchInfo.lRefDieNo		= 1;
	stSrchInfo.bDisableBackupAlign = FALSE;
	stMsg.InitMessage(sizeof(SRCH_TYPE), &stSrchInfo);

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "SearchCurrentDie", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE)
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(REF_TYPE), &stInfo);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (stInfo.bStatus && stInfo.bFullDie)
	{
		lCurrX += stInfo.lX;	
		lCurrY += stInfo.lY;
		XY_MoveTo(lCurrX, lCurrY);
		Sleep(1000);
	}

if (IsWT2InUse())
{
AfxMessageBox("NGPIck: WT2 move backlight down ...", MB_SYSTEMMODAL);
}

	//1. Move table 1 to EJR offset posn before NGPIck
	MoveES101BackLightZUpDn(FALSE);	
	Sleep(10);
	GetEncoderValue();
	LONG lX1=0, lY1=0;
	if (IsWT2InUse())
	{
		lX1 = m_lEnc_X2;
		lY1 = m_lEnc_Y2;
	}
	else
	{
		lX1 = m_lEnc_X1;
		lY1 = m_lEnc_Y1;
	}

if (IsWT2InUse())
{
AfxMessageBox("NGPIck: WT2 move to EJ offset ...", MB_SYSTEMMODAL);
}

	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
	X_MoveTo(lX1 + m_lEjtOffsetX, SFM_NOWAIT);
	Y_MoveTo(lY1 + m_lEjtOffsetY, SFM_NOWAIT);
	X_Sync();
	Y_Sync();
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	Sleep(100);

if (IsWT2InUse())
{
AfxMessageBox("NGPIck: WT2 up & DN EJ elevator ...", MB_SYSTEMMODAL);
}

	//2. Perform NGPICK
	//IPC_CServiceMessage stMsg;
	//int	nConvID = 0;
	BOOL bStart	= TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bStart);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "StartEjElevator", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (!bReturn)
	{
		HmiMessage("Error in calling StartEjElevator");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

if (IsWT2InUse())
{
AfxMessageBox("NGPIck: WT2 back to camera pos ...", MB_SYSTEMMODAL);
}

	//3. Move Table 1 back to original posn
	X_Profile(LOW_PROF);
	Y_Profile(LOW_PROF);
	X_MoveTo(lX1, SFM_NOWAIT);
	Y_MoveTo(lY1, SFM_NOWAIT);
	X_Sync();
	Y_Sync();
	X_Profile(NORMAL_PROF);
	Y_Profile(NORMAL_PROF);
	Sleep(100);


	MoveES101BackLightZUpDn(TRUE);	
	SetJoystickOn(TRUE);			//ON joystick after test

	//HmiMessageEx("NG-PICK current die is done.");
	HmiMessageEx("NG-PICK current die is done.", "Test NGPick", glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER, 
					36000000, glHMI_MSG_MODAL, 0, 400, 300);
	MoveES101BackLightZUpDn(FALSE);	

	/****/ CMSLogFileUtility::Instance()->MS_LogOperation("TestNGPick: Done");	/****/ 

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::ResetWTInUse(IPC_CServiceMessage& svMsg)
{
	SetWT2InUse(!m_nWTInUse);
	SaveData();

	if( IsWT2InUse() )
		CMSLogFileUtility::Instance()->MS_LogOperation("ES101: WTInUse is toggle manually to 1 (WT2)");
	else
		CMSLogFileUtility::Instance()->MS_LogOperation("ES101: WTInUse is toggle manually to 0 (WT1)");

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::MoveTable1ToEjtOffset(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bUseDualTablesOption)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if( IsESMachine() )
	{
		IPC_CServiceMessage stMsg;
		int nConvID = m_comClient.SendRequest(WAFER_LOADER_STN, "IsExpanderSafeToMove", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		if( IsWL2ExpanderSafeToMove()==FALSE )
		{
			bReturn = FALSE;
		}
		if( bReturn==FALSE )
		{
			svMsg.InitMessage(sizeof(BOOL), &bReturn);
			return 1;
		}
	}

	BOOL bToEjtOffset = FALSE;
	svMsg.GetMsg(sizeof(BOOL), &bToEjtOffset);

	if (IsWT2InUse())
	{
		HomeTable2();	
	}

	X1_Profile(LOW_PROF);
	Y1_Profile(LOW_PROF);

	if (bToEjtOffset)
	{
		X1_MoveTo(GetWft1CenterX(TRUE), SFM_NOWAIT);
		Y1_MoveTo(GetWft1CenterY(TRUE), SFM_NOWAIT);
	}
	else
	{
		X1_MoveTo(GetWft1CenterX(FALSE), SFM_NOWAIT);
		Y1_MoveTo(GetWft1CenterY(FALSE), SFM_NOWAIT);
	}

	X1_Sync();
	Y1_Sync();
	X1_Profile(NORMAL_PROF);
	Y1_Profile(NORMAL_PROF);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferTable::MoveTable1ToHome(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bUseDualTablesOption)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsWT2InUse())
	{
		HomeTable2();	
	}

	HomeTable1();	

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


//	4.24TX1 	should choose profile based on move distance
BOOL CWaferTable::XY1_SafeMoveTo(LONG lPosX, LONG lPosY, DOUBLE dScale, BOOL bUsePosOffset)
{
	LONG lDistX, lDistY;
	BOOL bX1st=TRUE, bY1st=TRUE, bXYOK = TRUE;
	BOOL bRtnMove = TRUE, bChgProfY = FALSE, bChgProfX = FALSE;

	X1_Sync();
	Y1_Sync();
	GetEncoderValue();
	LONG lX = GetCurrX1();
	LONG lY = GetCurrY1();
	LONG lT = GetCurrT1();

	{
		bX1st = IsWithinWaferLimit(lPosX, lY, dScale, bUsePosOffset);
		bY1st = IsWithinWaferLimit(lX, lPosY, dScale, bUsePosOffset);
	}

	bXYOK = IsWithinWaferLimit(lPosX, lPosY, dScale, bUsePosOffset);

	if( bXYOK==FALSE )
	{
		//qnichia004
		BOOL b1 = IsWithinWaferLimit(lPosX, lPosY, dScale, bUsePosOffset);
		BOOL b2 = TRUE;
		CString szTemp;
		szTemp.Format("XY1_SafeMoveTo fails at bXYOK (%d %d)", b1, b2);
		SetErrorMessage(szTemp);
		return FALSE;
	}

	lDistX = labs(lX-lPosX);
	lDistY = labs(lY-lPosY);

	LONG lTimeX = 0, lTimeY = 0;
#ifdef NU_MOTION
	if( m_nProfile_X!=LOW_PROF )
	{
		/*if (lDistX > GetSlowProfileDist())
		{
			X1_Profile(LOW_PROF);
			bChgProfX = TRUE;
			lTimeX = X_ProfileTime(LOW_PROF, lDistX, lDistX);
		}*/
	}

	if( m_nProfile_Y!=LOW_PROF )
	{
		/*if (lDistY > GetSlowProfileDist())
		{
			Y1_Profile(LOW_PROF);
			bChgProfY = TRUE;
			lTimeY = Y_ProfileTime(LOW_PROF, lDistY, lDistY);
		}*/
	}
#else
	X1_Profile(LOW_PROF);
	Y1_Profile(LOW_PROF);
#endif

	if( bXYOK && bX1st && bY1st )	// sync move mode
	{
		if( X1_MoveTo(lPosX, SFM_NOWAIT)==Err_WTableXMove )
		{
			//anichia004
			CString szTemp;
			szTemp = "XY1_SafeMoveTo fails at X1_MoveTo";
			SetErrorMessage(szTemp);
			bRtnMove = FALSE;
		}
		if( Y1_MoveTo(lPosY, SFM_NOWAIT)==Err_WTableYMove )
		{
			//qnichia004
			CString szTemp;
			szTemp = "XY1_SafeMoveTo fails at Y1_MoveTo";
			SetErrorMessage(szTemp);
			bRtnMove = FALSE;
		}
		X1_Sync();
		Y1_Sync();
	}
	else
	{
		if ( bX1st == TRUE )
		{
			if( X1_MoveTo(lPosX, SFM_WAIT) == Err_WTableXMove )
			{
				//anichia004
				CString szTemp;
				szTemp = "XY1_SafeMoveTo fails at X1_MoveTo bX1st";
				SetErrorMessage(szTemp);
				bRtnMove = FALSE;
			}
			if( Y1_MoveTo(lPosY, SFM_WAIT) == Err_WTableYMove )
			{
				//nichia004
				CString szTemp;
				szTemp = "XY1_SafeMoveTo fails at Y1_MoveTo bX1st";
				SetErrorMessage(szTemp);
				bRtnMove = FALSE;
			}
		}
		else if ( bY1st == TRUE )
		{
			if( Y1_MoveTo(lPosY, SFM_WAIT) == Err_WTableYMove )
			{
				//anichia004
				CString szTemp;
				szTemp = "XY1_SafeMoveTo fails at Y1_MoveTo bY1st";
				SetErrorMessage(szTemp);
				bRtnMove = FALSE;
			}
			if( X1_MoveTo(lPosX, SFM_WAIT) == Err_WTableXMove )
			{
				//anichia004
				CString szTemp;
				szTemp = "XY1_SafeMoveTo fails at X1_MoveTo bY1st";
				SetErrorMessage(szTemp);
				bRtnMove = FALSE;
			}
		}
		else
		{
			bRtnMove = FALSE;
		}
	}

#ifdef NU_MOTION
	if( bChgProfY )
		Y1_Profile(NORMAL_PROF);
	if( bChgProfX )
		X1_Profile(NORMAL_PROF);
#else
		X1_Profile(NORMAL_PROF);
		Y1_Profile(NORMAL_PROF);
#endif

	LONG lWTIndexTime	= max(lTimeX, lTimeY);
	if ( (m_lLongJumpMotTime >= 30) && (lWTIndexTime >= m_lLongJumpMotTime) && m_lLongJumpDelay > 0 )
	{
		Sleep( m_lLongJumpDelay );
	}

	return bRtnMove;
}

//	4.24TX1 	should choose profile based on move distance
BOOL CWaferTable::XY2_SafeMoveTo(LONG lPosX, LONG lPosY, DOUBLE dScale, BOOL bUsePosOffset)
{
	LONG lDistX, lDistY;
	BOOL bX1st=TRUE, bY1st=TRUE, bXYOK = TRUE;
	BOOL bRtnMove = TRUE, bChgProfY = FALSE, bChgProfX = FALSE;

	X2_Sync();
	Y2_Sync();
	GetEncoderValue();
	LONG lX = GetCurrX2();
	LONG lY = GetCurrY2();
	LONG lT = GetCurrT2();
	bX1st = IsWithinWaferLimit(lPosX, lY, dScale, bUsePosOffset);
	bY1st = IsWithinWaferLimit(lX, lPosY, dScale, bUsePosOffset);
	bXYOK = IsWithinWaferLimit(lPosX, lPosY, dScale, bUsePosOffset);

	if( bXYOK==FALSE )
	{
		return FALSE;
	}

	lDistX = labs(lX-lPosX);
	lDistY = labs(lY-lPosY);

#ifdef NU_MOTION
	if( m_nProfile_X2!=LOW_PROF )
	{
		if (lDistX > GetSlowProfileDist())
		{
			X2_Profile(LOW_PROF);
			bChgProfX = TRUE;
		}
	}

	if( m_nProfile_Y2!=LOW_PROF )
	{
		if (lDistY > GetSlowProfileDist())
		{
			Y2_Profile(LOW_PROF);
			bChgProfY = TRUE;
		}
	}
#else
	X2_Profile(LOW_PROF);
	Y2_Profile(LOW_PROF);
#endif

	if( bXYOK && bX1st && bY1st )	// sync move mode
	{
		if( X2_MoveTo(lPosX, SFM_NOWAIT)==Err_WTableXMove )
		{
			bRtnMove = FALSE;
		}
		if( Y2_MoveTo(lPosY, SFM_NOWAIT)==Err_WTableYMove )
		{
			bRtnMove = FALSE;
		}
		X2_Sync();
		Y2_Sync();
	}
	else
	{
		if ( bX1st == TRUE )
		{
			if( X2_MoveTo(lPosX, SFM_WAIT) == Err_WTableXMove )
			{
				bRtnMove = FALSE;
			}
			if( Y2_MoveTo(lPosY, SFM_WAIT) == Err_WTableYMove )
			{
				bRtnMove = FALSE;
			}
		}
		else if ( bY1st == TRUE )
		{
			if( Y2_MoveTo(lPosY, SFM_WAIT) == Err_WTableYMove )
			{
				bRtnMove = FALSE;
			}
			if( X2_MoveTo(lPosX, SFM_WAIT) == Err_WTableXMove )
			{
				bRtnMove = FALSE;
			}
		}
		else
		{
			bRtnMove = FALSE;
		}
	}

#ifdef NU_MOTION
	if( bChgProfY )
		Y2_Profile(NORMAL_PROF);
	if( bChgProfX )
		X2_Profile(NORMAL_PROF);
#else
	X2_Profile(NORMAL_PROF);
	Y2_Profile(NORMAL_PROF);
#endif

	return bRtnMove;
}


LONG CWaferTable::GetCurrX1()
{
	return m_lEnc_X1;
}

LONG CWaferTable::GetCurrY1()
{
	return m_lEnc_Y1;
}

LONG CWaferTable::GetCurrCmdY1()
{
	return m_lCmd_Y1;
}

LONG CWaferTable::GetCurrT1()
{
	return m_lEnc_T1;
}

BOOL CWaferTable::IsWT1UnderCamera()
{
	GetEncoderValue();
	LONG lX = GetCurrX1();
	LONG lY = GetCurrY1();

	return IsWithinWT1WaferLimit(lX, lY);
}

BOOL CWaferTable::IsWT2UnderCamera()
{
	if( IsESDualWT()==FALSE )
		return FALSE;

	GetEncoderValue();
	LONG lX = GetCurrX2();
	LONG lY = GetCurrY2();

	return IsWithinWT2WaferLimit(lX, lY);
}

BOOL CWaferTable::IsWT1UnderEjectorPos()
{
	GetEncoderValue();
	LONG lX = GetCurrX1();
	LONG lY = GetCurrY1();

	return IsWithinWT1WaferLimit(lX, lY, 1, TRUE);
}

BOOL CWaferTable::IsWT2UnderEjectorPos()
{
	GetEncoderValue();
	LONG lX = GetCurrX1();
	LONG lY = GetCurrY1();

	return IsWithinWT2WaferLimit(lX, lY, 1, TRUE);
}

LONG CWaferTable::UpdateWaferDiameter(IPC_CServiceMessage& svMsg)
{
	CString szMsg;
	DOUBLE dNewValue = 0;
	svMsg.GetMsg(sizeof(DOUBLE), &dNewValue);

	if (dNewValue > 5.8)
	{
		szMsg.Format("Wafer diameter (%.2f inches) is greater than 5.8 inches and change to 5.8 inches", dNewValue);
		HmiMessage(szMsg);
		dNewValue = 5.8;
	}

	//Update Wafer Diameter on HMI (mil)
	m_dWaferDiameter = dNewValue;
	LONG lWafDiameter = (LONG) (dNewValue * 25.4 / m_dXYRes);
	m_lWaferSize = lWafDiameter;

	SaveWaferTblData();

	szMsg.Format("Wafer diameter (%.2f mil) is updated (Enc = %lu)", m_dWaferDiameter, m_lWaferSize);
	HmiMessage(szMsg);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


