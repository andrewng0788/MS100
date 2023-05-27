/////////////////////////////////////////////////////////////////
// WL_LoadFrame2.cpp : Common functions of the CWaferLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
//	Revision:	1.00
//
//	By:			Andrew Ng
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2012.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "TakeTime.h"
#include "MS896A.h"
#include "HmiDataManager.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CWaferLoader::IsMagazineExist2(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if ( IsESDualWL()==FALSE )
		return FALSE;
	
	m_bMagazineExist2 = !CMS896AStn::MotionReadInputBit(WL_SI_MagExist2);
	return m_bMagazineExist2;
}

BOOL CWaferLoader::IsFrameDetect2(VOID)
{
	if (m_fHardware == FALSE)
		return TRUE;
	if ( IsESDualWL()==FALSE )
		return FALSE;
	
	m_bFrameDetect2 = CMS896AStn::MotionReadInputBit(WL_SI_FrameDetect2);
	return m_bFrameDetect2;
}

BOOL CWaferLoader::IsWaferFrameDetect2(VOID)
{
	if (m_fHardware == FALSE)
		return FALSE;
	if ( IsESDualWL()==FALSE )
		return FALSE;

	m_bWaferFrameDetect2 = !CMS896AStn::MotionReadInputBit(WL_SI_WafFrameDetect2);
	return m_bWaferFrameDetect2;
}

BOOL CWaferLoader::IsMagazineSafeToMove2(VOID)
{
	BOOL bSafe = TRUE;

	if (!m_fHardware)
		return TRUE;
	if (m_bDisableWL)
		return TRUE;
	if (m_bDisableWLWithExp)
		return TRUE;

	bSafe = TRUE;

	//Gripper protection against magazine Z motion		//SanAn
	//if (bSafe && !IsBurnIn())
	//{
	//	GetEncoderValue();
	//	if ( (m_lLoadPos_X < -10000) && ((m_lEnc_X - m_lLoadPos_X) < 1000) )
	//	{
	//		bSafe = FALSE;
	//	}
	//}

	if ( IsBurnIn() )
	{
		return TRUE;
	}
	return bSafe;
}


BOOL CWaferLoader::IsGripperAtSafePos2(LONG lCheckPos)
{
	if (!IsAllMotorsEnable())
		return FALSE;

	GetEncoderValue();

	/*
	if ( MotionIsServo(WL_AXIS_X2, &m_stWLAxis_X2) )
	{
		if (m_lEnc_X2 < (lCheckPos + 1000))
			return FALSE;
	}
	else
	{
		if (m_lEnc_X2 < (lCheckPos + 500))
			return FALSE;
	}
	*/

	return TRUE;
}


BOOL CWaferLoader::IsFrameJam2(VOID)
{
	SHORT siResult = HP_SUCCESS;
	LONG lReport = 0;

	if (m_fHardware == FALSE)
	{
		return FALSE;
	}
	if (m_bDisableWL)
	{
		return FALSE;
	}
	if (m_bDisableWLWithExp)	//STD machine with Expander
	{
		return FALSE;
	}
	if ( IsESDualWL()==FALSE )
		return FALSE;

	if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
	{
		m_bFrameJam2= !CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X2, &m_stWLAxis_X2);
		siResult	= CMS896AStn::MotionGetLastWarning(WL_AXIS_X2, &m_stWLAxis_X2);
		lReport		= CMS896AStn::MotionReportErrorStatus(WL_AXIS_X2, &m_stWLAxis_X2);
	}
	else
	{
		m_bFrameJam2= CMS896AStn::MotionIsNegativeLimitHigh(WL_AXIS_X2, &m_stWLAxis_X2);
		siResult	= CMS896AStn::MotionGetLastWarning(WL_AXIS_X2, &m_stWLAxis_X2);
		lReport		= CMS896AStn::MotionReportErrorStatus(WL_AXIS_X2, &m_stWLAxis_X2);
	}

	return m_bFrameJam2;
}


VOID CWaferLoader::SetGripper2State(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if ( IsESDualWL()==FALSE )
		return;

	CMS896AStn::MotionSetOutputBit(WL_SO_Gripper2State, bSet);
}

VOID CWaferLoader::SetGripper2Level(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if ( IsESDualWL()==FALSE )
		return;

	CMS896AStn::MotionSetOutputBit(WL_SO_Gripper2Level, bSet);
}

VOID CWaferLoader::SetPushUpTable2(BOOL bSet)
{
	if (!m_fHardware)
		return;
	CMS896AStn::MotionSetOutputBit(WL_SO_FrameAlign2, bSet);
}


INT CWaferLoader::MoveToMagazineSlot2(LONG lMagazineNo, LONG lSlotNo, LONG lOffset, BOOL bWait, BOOL bCheckSensors)
{
	LONG lPitch = 0;		

	if (bCheckSensors == TRUE)
	{
		if (IsWaferFrameDetect2() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT2 Frame is detected on track");
			SetErrorMessage("WT2 Frame is detected on track");

			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONTRACK);
			return Err_FrameExistOnTrack;
		}

		//Klocwork	//v4.25
		//if ( IsMagazineSafeToMove2() == FALSE )
		//{
		//	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is not in safe position");
		//	SetErrorMessage("Gripper2 is not in safe position");
		//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
		//	return Err_GripperNotInSafePos;
		//}
	}

	if (m_bUseDualTablesOption)
	{
		LONG lTopSlotLevel_Z; 
		DOUBLE dSlotPitch;

		BOOL bCheckMgzn = TRUE;
		LONG lMaxMagNo = GetWL2MaxMgznNo();
		if( m_bUseMagazine3AsX && m_szWT2YieldState == "N" && m_lMagazineX_No==lMagazineNo )
		{
			bCheckMgzn = FALSE;
		}
		if( m_bUseAMagazineAsG && m_szWT2YieldState == "Y" && m_lMagazineG_No==lMagazineNo )
		{
			bCheckMgzn = FALSE;
		}
		if( bCheckMgzn )
		{
			if (lMagazineNo > lMaxMagNo || lMagazineNo < 0)
			{
				CString szMsg;
				szMsg.Format("To magazine %d over limit %d", lMagazineNo, lMaxMagNo);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				return FALSE;
			}
		}
		else
		{
			lMaxMagNo = lMagazineNo;
		}

		lMagazineNo		= lMagazineNo - 1;
		if (lMagazineNo < 0)
			lMagazineNo = 0;								//Klocwork	//v4.25
		lMagazineNo		= min(lMagazineNo, lMaxMagNo-1);	//Klocwork	//v4.25


		lTopSlotLevel_Z = m_stWaferMagazine2[lMagazineNo].m_lTopLevel_Z;
		dSlotPitch		= m_stWaferMagazine2[lMagazineNo].m_dSlotPitch;
		lPitch = GetSlotNoPitchCount(lSlotNo, dSlotPitch, m_dZ2Res);

		if( Z2_IsPowerOn()==FALSE )
		{
			SetErrorMessage("Wafer Loader Z2 power off");
			HmiMessage_Red_Back("Wafer Loader Z2 power is off\nPlease check!", "Wafer Loader");
			return FALSE;
		}

		if( IsRightLoaderCoverOpen() )
		{
			HmiMessage_Red_Back("Wafer Loader Right cover open!", "Wafer Loader");
			return FALSE;
		}

		if (!bWait)
		{
			Z2_MoveTo(lTopSlotLevel_Z + lPitch + lOffset, SFM_NOWAIT);	
		}
		else
		{
			Z2_MoveTo(lTopSlotLevel_Z + lPitch + lOffset);
		}
	}
	else
	{
		lPitch = GetSlotNoPitchCount(lSlotNo, m_dSlotPitch, m_dZ2Res);

		if (!bWait)
		{
			Z2_MoveTo(m_lTopSlotLevel_Z + lPitch + lOffset, SFM_NOWAIT);
		}
		else
		{
			Z2_MoveTo(m_lTopSlotLevel_Z + lPitch + lOffset);
		}
	}

	return 1;
}


VOID CWaferLoader::MoveWafer2Theta(LONG lStep)
{
	IPC_CServiceMessage stMsg;

	int nConvID = 0;
	LONG lStepT = lStep;

	if ( IsESDualWT()==FALSE )
		return;

	stMsg.InitMessage(sizeof(LONG), &lStepT);

	// Get the reply for the encoder value
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T2_MoveToCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}	
	}
}

LONG CWaferLoader::AutoRotateWafer2(BOOL bKeepBarcodeAngle)
{
	IPC_CServiceMessage stMsg;
	INT nConvID =0;
	LONG lRotateTime = 0;

	stMsg.InitMessage(sizeof(BOOL), &bKeepBarcodeAngle);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AutoRotateWafer2", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(LONG), &lRotateTime);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}

	if( lRotateTime>0 )
	{
		Sleep(lRotateTime);
	}

	return TRUE;
}


INT CWaferLoader::FilmFrameGearReady2(INT nMode , BOOL bGripperLevelOn, BOOL bMoveTheta)
{
	if ( IsESDualWL()==FALSE )
		return FALSE;

	//Home Motor
	if (bMoveTheta == TRUE)
		MoveWafer2Theta(0);

	if( IsMotionCE() )
	{
		CString szMsg;
		szMsg = "Motin Critical Error in FilmFrame2 GearReady()";
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		SetErrorMessage(szMsg);
		return FALSE;
	}

	if (CMS896AStn::MotionIsServo(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE)
		X2_MoveTo(WL_GRIPPER_SRV_HOME_OFFSET_POS);
	else
		X2_MoveTo(0);

	//Reset Gripper I/O Status
	SetGripper2State(WL_OFF);
	SetGripper2Level(WL_OFF);

	//Move bondarm to blow position
	if (BondArmMoveToBlow() == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("BH to BLOW fail in FilmFrame2 GearReady()");
		SetErrorMessage("WL: BH to BLOW fail in FilmFrame2 GearReady()");
		return FALSE;
	}
	
	BOOL bTableStatus = TRUE;
	// 4.24TX
	bTableStatus = ES101MoveTableToUnload(nMode, TRUE);	//	4.24TX 4

	if (!bTableStatus)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WT2 move fail in FilmFrame GearReady()");
		SetErrorMessage("WL: WT2 move fail in FilmFrame GearReady()");
		return FALSE;
	}

	if (bGripperLevelOn == TRUE)
	{
		SetGripper2Level(WL_ON);
		//SetGripperLevel(!bBurnIn);
		Sleep(500);
	}

	return TRUE;
}

INT CWaferLoader::MoveSearchFrameExistAndJam2(INT nPos)
{
	if ( IsESDualWL()==FALSE )
		return TRUE;

	INT nResult;
	nResult = X2_MoveTo(nPos, SFM_NOWAIT);

	while(1)
	{
		if ( nResult != gnOK )
		{
			break;
		}
		
		if ( CMS896AStn::MotionIsComplete(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE )
		{
			X2_Sync();
			break;
		}

		if ( IsFrameJam2() == TRUE )
		{
			CMS896AStn::MotionStop(WL_AXIS_X2, &m_stWLAxis_X2);
			return Err_FrameJammed;
		}

		if ( IsFrameDetect2() == TRUE )
		{
			CMS896AStn::MotionStop(WL_AXIS_X2, &m_stWLAxis_X2);
			return Err_FrameExistInGripper;
		}
		Sleep(1);
	}

	return TRUE;
}

BOOL CWaferLoader::MoveWaferTable2ForBarCode(INT nXYDirection,INT siDirection, LONG lSearchDistance, BOOL bWaitComplete)
{
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;
	BOOL bReturn;

	typedef struct
	{
		INT	nXYDirection;
		INT siDirection;
		LONG lSearchDistance;
		BOOL bWaitComplete;
			
	}TABLE_MOVE ;

	TABLE_MOVE stInfo;
	stInfo.nXYDirection = nXYDirection;
	stInfo.siDirection = siDirection;
	stInfo.lSearchDistance	= lSearchDistance;
	stInfo.bWaitComplete = bWaitComplete;

	stMsg.InitMessage(sizeof(TABLE_MOVE), &stInfo );

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ReadBarCodeOnTable2", stMsg);
	
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID,30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
	}

	return bReturn;
}

BOOL CWaferLoader::SyncWaferTable2XYT(CString szDirection)
{
	IPC_CServiceMessage stMsg;
	INT nConvID = 0;

	if ( IsESDualWT()==FALSE )
	{
		return FALSE;
	}

	BOOL bReturn = TRUE;
	stMsg.InitMessage(sizeof(BOOL), &bReturn );

	if (szDirection.MakeUpper() == "X")
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "X2_SyncCmd", stMsg);
	}
	else if (szDirection.MakeUpper() == "Y")
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "Y2_SyncCmd", stMsg);
	}
	else if (szDirection.MakeUpper() == "T")
	{
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "T2_SyncCmd", stMsg);
	}
	else if (szDirection.MakeUpper() == "XY")
	{
		nConvID	= m_comClient.SendRequest(WAFER_TABLE_STN, "XY2_SyncCmd", stMsg);
	}

	if (nConvID > 0)
	{
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 100000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else
			{
				Sleep(1);
			}
		}
	}

	return bReturn;
}

INT CWaferLoader::FilmFrameSearchOnTable2(VOID)
{
	if ( IsESDualWL()==FALSE )
		return FALSE;

	try
	{
		CMS896AStn::MotionSelectSearchProfile(WL_AXIS_X2, "spfWaferGripperSeachFrameOnTable", &m_stWLAxis_X2);
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	CMS896AStn::MotionSearch(WL_AXIS_X2, 0, SFM_NOWAIT, &m_stWLAxis_X2, "spfWaferGripperSeachFrameOnTable");
	X2_Sync();
	return 1;
}


INT CWaferLoader::FilmFrame2DriveIn(INT siDirection, LONG lSearchDistance, LONG lSearchVelocity, BOOL bWaitComplete)
{
	CString szLog;
	if ( IsESDualWL()==FALSE )
		return TRUE;

	try
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Start FilmFrameDriveIn2");
		CMS896AStn::MotionSelectSearchProfile(WL_AXIS_X2, "spfWaferGripperSearchJam", &m_stWLAxis_X2);

		szLog.Format("Start MotionUpdateSearchProfile - SrchDist = %d, SrchVel = %d", lSearchDistance,  lSearchVelocity);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		CMS896AStn::MotionUpdateSearchProfile(WL_AXIS_X2, "spfWaferGripperSearchJam", lSearchVelocity, lSearchDistance, &m_stWLAxis_X2);

		CMSLogFileUtility::Instance()->WL_LogStatus("Start MotionSearch2");
		CMS896AStn::MotionSearch(WL_AXIS_X2, !siDirection, SFM_NOWAIT, &m_stWLAxis_X2, "spfWaferGripperSearchJam");
	}
	catch(CAsmException e)
	{
		DisplayException(e);
		CMSLogFileUtility::Instance()->WL_LogStatus("Exception: MotionSearch2");
	}
	

	CMSLogFileUtility::Instance()->WL_LogStatus("Start MotionSearch2 WaitComplete");
	if (bWaitComplete == TRUE)
	{
		X2_Sync();
		Sleep(100);
	}

	if (CMS896AStn::MotionIsPowerOn(WL_AXIS_X2, &m_stWLAxis_X2) == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("FilmFrameDriveIn 2 CheckPower fail");
		return FALSE;
	}

	if ( IsFrameJam2() )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("FilmFrameDriveIn2 checkJam fail");
		return FALSE;
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("FilmFrameDriveIn2 End");
	return 1;
}


LONG CWaferLoader::WaferGripper2MoveSearchJam(INT nPos, BOOL bOpenGripper, LONG lMotionTime)
{
	INT nResult;

	if (m_fHardware == FALSE)
		return TRUE;
	if (m_bDisableWL)
		return TRUE;
	if (m_bDisableWLWithExp)
		return TRUE;
	if ( IsESDualWL()==FALSE )
		return FALSE;

	INT nCount = 0;
	INT nJamCount = 0;
	DOUBLE dStartTime = GetTime();
	nResult = X2_MoveTo(nPos, SFM_NOWAIT);

	while(1)
	{
		if ( nResult != gnOK )
		{
			return FALSE;
		}
		
		if ( CMS896AStn::MotionIsComplete(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE )
		{
			X2_Sync();
			break;
		}

		if ( !X2_IsPowerOn() )
		{
			return FALSE;
		}

		if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
		{
			if ( IsFrameJam2() == TRUE )
			{
				nJamCount++;
	
				if (nJamCount >= 50)
				{
					CMS896AStn::MotionStop(WL_AXIS_X2, &m_stWLAxis_X2);
					return FALSE;
				}
			}
		}
		else
		{
			if ( IsFrameJam2() == TRUE )
			{
				CMS896AStn::MotionStop(WL_AXIS_X2, &m_stWLAxis_X2);
				return FALSE;
			}
		}

		if (bOpenGripper == TRUE)
		{
			LONG lDiff = (LONG)(GetTime() - dStartTime);
			if (lDiff >= lMotionTime/2)
			{
				SetGripper2State(WL_OFF);
			}
		}

		nCount++;
		if ( nCount > 5000 )
		{
			return FALSE;
		}
		Sleep(1);
	}

	return TRUE;
}

BOOL CWaferLoader::MoveSearchFrame2(INT nPos)
{
	INT nResult;

	if ( IsESDualWL()==FALSE )
		return TRUE;

	nResult = CMS896AStn::MotionMove(WL_AXIS_X2, nPos, SFM_NOWAIT, &m_stWLAxis_X2);
	CMSLogFileUtility::Instance()->WL_LogStatus("Start Move Search Frame 2");

	while(1)
	{
		
		if ( nResult != TRUE )
		{
			break;
		}
		
		if ( CMS896AStn::MotionIsComplete(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE )
		{
			X2_Sync();
			break;
		}

		if (IsFrameDetect2() == TRUE)
		{
			return TRUE;
		}

		if (!IsAllMotorsEnable2())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is OFF @ at LOAD");
			SetErrorMessage("Gripper2 is OFF @ at LOAD");
			return Err_FrameJammed;
		}

		if ( IsFrameJam2() == TRUE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to LOAD");
			SetErrorMessage("Gripper2 is Jam @ to LOAD");
			CMS896AStn::MotionStop(WL_AXIS_X2, &m_stWLAxis_X2);
			return Err_FrameJammed;
		}

		Sleep(1);
	}

	return TRUE;
}

INT CWaferLoader::GetNextFilmFrame2(BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bDisplayMsg)
{
	BOOL bFrameDetected = FALSE;
	int nStatus = TRUE;
	INT lMagStatus;
	LONG lSearchDistanceX;
	BOOL bAllMagazineComplete = FALSE;
	CString szStr;
	if ( IsESDualWL()==FALSE )
		return TRUE;

	//Searching FilmFrame until end
	while(1)
	{
		if (bSearchFrameInMagazine == TRUE)
		{
			if ( m_bIsGetCurrentSlot2 == FALSE )
			{
				m_lCurrentSlotNo2 += (m_lSkipSlotNo2 + 1);
				szStr.Format("GetNext FilmFrame2 - Get Current Slot:%d", m_lCurrentSlotNo2);
				CMSLogFileUtility::Instance()->WL_LogStatus(szStr);
			}
			else
			{
				m_bIsGetCurrentSlot2 = FALSE;
				szStr.Format("GetNext FilmFrame2 - Is Get Current Slot:%d", m_lCurrentSlotNo2);
			}

			LONG lMaxMagNo = GetWL2MaxMgznNo();

			if (m_lCurrentMagNo2 < 0 || m_lCurrentMagNo2 > lMaxMagNo)
			{
				bAllMagazineComplete = TRUE;
			}
			else
			{
				if (m_lCurrentMagNo2 < 1)
					m_lCurrentMagNo2 = 1;								//v4.25		//Klocwork
				m_lCurrentMagNo2 = min(m_lCurrentMagNo2, lMaxMagNo);	//v4.25		//Klocwork

				if (m_lCurrentSlotNo2 > m_stWaferMagazine2[m_lCurrentMagNo2-1].m_lNoOfSlots)
				{
					LONG lNextMagNo = m_lCurrentMagNo2 + 1;

					while (lNextMagNo <= lMaxMagNo)
					{
						if (m_stWaferMagazine2[lNextMagNo-1].m_lNoOfSlots > 0)
						{
							break;
						}

						lNextMagNo = lNextMagNo + 1;
					}

					if (lNextMagNo >  lMaxMagNo)
					{
						bAllMagazineComplete = TRUE;
					}
					else
					{
						m_lCurrentSlotNo2 = 1;
						m_lCurrentMagNo2 = lNextMagNo;
					}
				}
			}

			if (bAllMagazineComplete == FALSE)
			{
				lMagStatus = MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
				if ( lMagStatus != TRUE )
				{
					HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);

					nStatus = lMagStatus;
					break;
				}

				// check magazine exist and if not let it as the last slot of that magazine 
				//so that it will move to next magazine in next cycle
				if ((m_lCurrentSlotNo2 == 1) && (IsMagazineExist2() == FALSE) && (IsBurnIn() == FALSE))
				{
					m_lCurrentSlotNo2 = m_stWaferMagazine2[m_lCurrentMagNo2-1].m_lNoOfSlots;
					continue;
				}
			}
			else
			{
				m_lCurrentMagNo2	= 1;
				m_lCurrentSlotNo2	= 1;
				bAllMagazineComplete = FALSE;
				
				MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);
				Z2_Home();
				MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2);

				if (m_bUseBCInCassetteFile == TRUE)
				{
					ResetBarcodeInCassette();
					SetAlert_Red_Yellow(IDS_WL_BC_FILE_MAG_FULL);
					HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
					SetStatusMessage("WL2 Magazine is full");
					nStatus = ERR_CASS_FILE;
					break;
				}

				if (bBurnIn == FALSE && CMS896AApp::m_bEnableGripperLoadUnloadTest == FALSE)
				{
					if (m_bUseDualTablesOption == TRUE)
					{
						if (m_bIsMagazineFull == TRUE)
						{
							m_bIsMagazine2Full = TRUE;
							SaveData();
							SetAlert_Red_Yellow(IDS_WL_MAGS_FULL);
							HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
							SetStatusMessage("WL 2 All Magazines are full");
							CMSLogFileUtility::Instance()->WL_LogStatus("WL 2 All Magazines are full in GetNext FilmFrame2");
							nStatus = ERR_ALL_MAG_FULL;
							break;
						}
						else
						{
							m_bIsMagazine2Full = TRUE;
							SaveData();
							
							// for autoloadwaferframe only
							if (bDisplayMsg == TRUE)
							{
								SetAlert_Red_Yellow(IDS_WL_MAG2_FULL);
							}

							HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
							SetStatusMessage("WL Magazine2 is full");
							CMSLogFileUtility::Instance()->WL_LogStatus("Magazine is full in GetNext FilmFrame2");
							nStatus = Err_MagazineFull;
							break;
						}
					}
					else
					{
						LONG lOption;
						CString szText = " ";
								
						CMSLogFileUtility::Instance()->WL_LogStatus("Magazine2 is full");
						SetErrorMessage("Magazine2 is full");

						lOption = SetAlert_Msg_NoMaterial(IDS_WL_MAG_FULL, szText, "No", "Yes", NULL);

						SetStatusMessage("WL - Magazine 2 is full to clear wafer lot data");
						IPC_CServiceMessage stMsg;

						//	auto cycle, prestart, clear wafer lot data
						int	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ClearWaferLotDataCmd", stMsg);
						while(1)
						{
							if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
							{
								m_comClient.ReadReplyForConvID(nConvID, stMsg);
								break;
							}
							else
							{
								Sleep(10);
							}	
						}

						if ( lOption != 2 )
						{
							SaveData();
							HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
							SetStatusMessage("WL Magazine2 is full");
							nStatus = Err_MagazineFull;
							break;
						}
					}
				}
			}
		}

		//Move Gripper to load pos & do searching
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 move to load position");
		if (WaferGripper2MoveSearchJam(m_lLoadPos_X2) == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to load");
			SetErrorMessage("Gripper2 is Jam @ to load");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		if (!IsAllMotorsEnable2())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is OFF @ at LOAD");
			SetErrorMessage("Gripper2 is OFF @ at LOAD");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		if (IsFrameJam2() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to load");
			SetErrorMessage("Gripper2 is Jam @ to load");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);

			//if (X2_IsPowerOn())
			//{
			//	CMS896AStn::MotionPowerOff(WL_AXIS_X2);		//for safety purpose
			//}

			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);

			return Err_FrameJammed;
		}

		//andrew: suggested by Sing to check the frame-exist sensor before further drive-in
		bFrameDetected = IsFrameDetect2();
		if (bFrameDetected == TRUE)
		{
			break;
		}
		else
		{
			Sleep(200);
			bFrameDetected = IsFrameDetect2();
			if (bFrameDetected == TRUE)
			{
				break;
			}
		}

		//FilmFrameSearch();
		if (GetExpType() == WL_EXP_CYLINDER)
		{
			lSearchDistanceX = WL_GRIPPER_CYLINDER_SEARCH_DIST;	
		}
		else 
		{
			lSearchDistanceX = WL_GRIPPER_SEARCH_DIST;
		}
		
		if (MoveSearchFrame2(-1 * (lSearchDistanceX )) != TRUE)	
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Move Search Frame 2 fails at LOAD");
			SetErrorMessage("Move Search Frame 2 fails at LOAD");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		if (!IsAllMotorsEnable2())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is OFF @ at LOAD");
			SetErrorMessage("Gripper2 is OFF @ at LOAD");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
		if (IsFrameJam2() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to load");
			SetErrorMessage("Gripper2 is Jam @ to load");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);

			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Check Frame detect sensor
		bFrameDetected = IsFrameDetect2();
		if (bFrameDetected != TRUE)	
		{
			Sleep(200);
			bFrameDetected = IsFrameDetect2();
		}

		if (bFrameDetected == TRUE)
		{
			break;
		}
		else
		{
			if (bBurnIn == TRUE)
			{
				bFrameDetected = TRUE;
				break;
			}

			if (bSearchFrameInMagazine == TRUE)
			{
				X2_MoveTo(m_lReadyPos_X2);

				//Klocwork	//v4.25
				//if (!IsMagazineSafeToMove2())
				//{
				//	SetAlert_Red_Yellow(IDS_WL_GRIPPER_NOT_SAFE);
				//	CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 misssing-step to READY pos");
				//	return Err_FrameJammed;
				//}

				if (IsFrameDetect2() == TRUE && m_bNoSensorCheck == FALSE)
				{
					SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
					return Err_FrameExistInGripper;
				}
			}
			else
			{
				break;
			}
		}
	}//End search FilmFrame


	if ( nStatus != TRUE )
	{
		//Close expander
		if ( GetExpType() == WL_EXP_VACUUM )
		{
			//ExpanderVacuumPlatform(WL_DOWN, FALSE);
		}
		else
		{
			INT nStatus = 0;
			if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) ||
				 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28T4
			{
				nStatus = ExpanderDCMotor2Platform(WL_DOWN, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
			}
			else if (GetExpType() == WL_EXP_CYLINDER)
			{
				nStatus = ExpanderCylinderMotorPlatform2(WL_DOWN, TRUE, FALSE, FALSE);
			}
			else
			{
				//nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, FALSE, FALSE);
			}

			if ( nStatus != 1 )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot close");
				SetErrorMessage("Expander2 cannot close");
				SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
			}
		}

		HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
		return nStatus;
	}

	return bFrameDetected;
}

INT CWaferLoader::PushFrameBack2()
{
	//Push frame back into magazine
	CMSLogFileUtility::Instance()->WL_LogStatus("Start Push Frame Back 2");

	MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2, m_lUnloadOffset2, TRUE, FALSE);
	CMSLogFileUtility::Instance()->WL_LogStatus("Start Push Frame Move X2");
	
	X2_MoveTo(m_lLoadPos_X2);
	CMSLogFileUtility::Instance()->WL_LogStatus("PushFrameBack2 - Before Check Jam");

	if (IsFrameJam2() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ push back");
		SetErrorMessage("Gripper2 is Jam @ push back");
		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("PushFrameBack2 - Check Jam Complete");

	//Off Clip
	Sleep(100);
	SetGripper2State(WL_OFF);
	Sleep(100);
	return TRUE;
}

INT CWaferLoader::ExpanderDCMotor2Platform(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, 
										  LONG lMoveDac, BOOL bReadBarCode)
{
	BOOL bEnableHotAir = FALSE;
	LONG lRunTime = 0;	
	
	LONG lHotAirTime	= m_lHotBlowOpenTime2;	
	LONG lDriveInTime	= m_lOpenDriveInTime2;
	LONG lDacValue		= lMoveDac;

	if ( IsESDualWL()==FALSE )
		return FALSE;

	//v4.16T5	//Support of MS100 9Inch
	LONG lMotorDirection = (BOOL) GetChannelInformation(MS896A_CFG_CH_WAFEXPDCMOTOR2, MS896A_CFG_CH_MOTOR_DIRECTION);
	if (lMotorDirection<0)
		lDacValue = lDacValue * -1;		//minus DAC value for DC motor to MOVE UP

	if (m_bDisableWL)
		return TRUE;
	if (!m_bIsExpDCMotorExist)
		return TRUE;


	if (IsExpander2Lock() == FALSE)
	{
		SetExpander2Lock(WL_ON);
		Sleep(400);

		if (IsExpander2Lock() == FALSE)
			return Err_ExpanderLockFailed;
	}

	if ( (IsExpander2Open() == TRUE) && (bOpen == TRUE) )
	{
		SetExpander2Lock(WL_OFF);
		return Err_ExpanderAlreadyOpen;
	}

	if ( (IsExpander2Close() == TRUE) && (bOpen == FALSE) )
	{
		SetExpander2Lock(WL_OFF);
		return Err_ExpanderAlreadyClose;
	}

	if (bOpen == WL_DOWN)
	{
		lHotAirTime	= m_lHotBlowCloseTime2;
		lDriveInTime = m_lCloseDriveInTime2;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		// If load from gripper and close, check frame exist sensor in expander
		//Check new Frame Exist Sensor (Osram)
#ifndef NU_MOTION
		if( pApp->GetCustomerName()=="OSRAM" && IsBurnIn()==FALSE && bCheckFrameExist )
		{
			BOOL bFrameExisted = IsFrameExist2();
			CString szMsg;
			szMsg.Format("wl frame exist gear down = %d", bFrameExisted);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			if( bFrameExisted )
			{
				return Err_NoFrameExist;
			}
		}
#endif
	}
	else
	{
		lDacValue = lDacValue * -1;		//minus DAC value for DC motor to MOVE UP
	}

//CString szTemp;
//szTemp.Format("DC Motor2 DAC value = %d (%d)", lDacValue, m_lExpDCMotorDacValue);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	//Check Hot Air is need or not
	if ( bHotAir == FALSE )
	{
		lHotAirTime = 0;
	}

	if (lHotAirTime > 0)
	{
		bEnableHotAir = TRUE;
		SetHotAir2(WL_ON);
	}
	else
	{
		bEnableHotAir = FALSE;
		lHotAirTime = 0;
	}


	//Update Expander Status
	(*m_psmfSRam)["MS896A"]["Expander Status"] = bOpen;
	m_bExpander2Status = bOpen;
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER2_STATUS, (int)m_bExpander2Status);

	if (bReadBarCode && m_bUseBarcode)
	{
		//alex
		m_szBarcodeName2.Empty();
		ReadBarcode2(&m_szBarcodeName2, 100, 2);	// ~0.2sec
	}


	//Turn on & move DC Motor
	CMS896AStn::MotionPowerOn(WL_AXIS_EXP_Z2, &m_stWLExpAxis_Z2);
	Sleep(500);
	CMS896AStn::OpenDac(&m_stWLExpAxis_Z2, lDacValue, 10, 1, WL_EXP_TIMEOUT * 10, 1);


	//DC Motor openning or closing
	while(1)
	{
		if ( (IsExpander2Open() == bOpen) && (IsExpander2Close() == !bOpen) )
		{
			if (lDriveInTime > 0)
			{
				Sleep(lDriveInTime);
			}

			CMS896AStn::MotionStop(WL_AXIS_EXP_Z2, &m_stWLExpAxis_Z2);
			break;
		}

		Sleep(10);	//1);	//v3.93
		lRunTime++;

		//Protect expander for over drive
		if (lRunTime > WL_EXP_TIMEOUT) 
		{
			if (bEnableHotAir == TRUE)
			{
				SetHotAir2(WL_OFF);
			}

			CMS896AStn::MotionStop(WL_AXIS_EXP_Z2, &m_stWLExpAxis_Z2);
			Sleep(500);
			CMS896AStn::MotionPowerOff(WL_AXIS_EXP_Z2, &m_stWLExpAxis_Z2);
			Sleep(100);
			SetExpander2Lock(WL_OFF);
			return Err_ExpanderOpenCloseFailed;
		}

		//Stop Hot Air if hit time
		if ( (bEnableHotAir == TRUE) && (lRunTime >= lHotAirTime) )
		{
			SetHotAir2(WL_OFF);
		}
	}


	//Continue Hot air if necessary
	if (bEnableHotAir == TRUE)
	{
		if (lHotAirTime > lRunTime)
		{
			Sleep(lHotAirTime - lRunTime);
			SetHotAir2(WL_OFF);
		}
		else
		{
			SetHotAir2(WL_OFF);
		}
	}

/*
	//DriveIn Expander
	if (lDriveInTime > 0)
	{
		CMS896AStn::OpenDac(&m_stWLExpAxis_Z2, lDacValue, 10, 1, lDriveInTime, 1);
		Sleep(lDriveInTime);
		CMS896AStn::MotionStop(WL_AXIS_EXP_Z2, &m_stWLExpAxis_Z2);
	}
*/
	//Off DC Motor
	Sleep(500);
	CMS896AStn::MotionPowerOff(WL_AXIS_EXP_Z2, &m_stWLExpAxis_Z2);


	//Delay for mylar paper cool down
	if (m_lMylarCoolTime2 > 0)
	{
		Sleep(m_lMylarCoolTime2);
	}

	SetExpander2Lock(WL_OFF);
	return TRUE;
}


INT CWaferLoader::ExpanderCylinderMotorPlatform2(BOOL bOpen, BOOL bToUnloadPos, BOOL bHotAir, BOOL bCheckFrameExist, 
												BOOL bReadBarCode)
{
	BOOL bEnableHotAir = FALSE;
	LONG lRunTime = 0;	
	LONG lHotAirTime = m_lHotBlowOpenTime2;	
	LONG lDriveInTime = m_lOpenDriveInTime2;

	if (m_bDisableWL)
		return TRUE;
	if ( IsESDualWL()==FALSE )
		return FALSE;

	if ( (IsExpander2Open() == TRUE) && (bOpen == TRUE) )
	{
		return Err_ExpanderAlreadyOpen;
	}

	if ( (IsExpander2Close() == TRUE) && (bOpen == FALSE) )
	{
		return Err_ExpanderAlreadyClose;
	}

	if (bOpen == WL_DOWN)
	{
		lHotAirTime	= m_lHotBlowCloseTime2;
		lDriveInTime = m_lCloseDriveInTime2;

		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		// If load from gripper and close, check frame exist sensor in expander
		//Check new Frame Exist Sensor (Osram)
#ifndef NU_MOTION
		if( pApp->GetCustomerName()=="OSRAM" && IsBurnIn()==FALSE && bCheckFrameExist )
		{
			BOOL bFrameExisted = IsFrameExist2();
			CString szMsg;
			szMsg.Format("wl frame 2 exist gear down = %d", bFrameExisted);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			if( bFrameExisted )
			{
				return Err_NoFrameExist;
			}
		}
#endif
	}

	lRunTime = 1;
	SetPushUpTable2(bOpen);

	//Check Hot Air is need or not
	if ( bHotAir == FALSE )
	{
		lHotAirTime = 0;
	}

	if (lHotAirTime > 0)
	{
		bEnableHotAir = TRUE;
		SetHotAir2(WL_ON);
	}
	else
	{
		bEnableHotAir = FALSE;
		lHotAirTime = 0;
	}


	//Update Expander Status
	(*m_psmfSRam)["MS896A"]["Expander Status"] = bOpen;
	m_bExpander2Status = bOpen;

	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(gszPROFILE_SETTING, gszEXPANDER2_STATUS, (int)m_bExpander2Status);

	if (bReadBarCode)
	{
		//alex
		m_szBarcodeName2.Empty();
		ReadBarcode(&m_szBarcodeName2, 100, 2);	// ~0.2sec
	}

	//DC Motor openning or closing
	while(1)
	{
		if ( (IsExpander2Open() == bOpen) && (IsExpander2Close() == !bOpen) )
		{
			break;
		}

		Sleep(10);	//1);	//v3.93
		lRunTime++;

		//Protect expander for over drive
		if (lRunTime > WL_EXP_TIMEOUT) 
		{
			if (bEnableHotAir == TRUE)
			{
				SetHotAir2(WL_OFF);
			}
			Sleep(500);
			return Err_ExpanderOpenCloseFailed;
		}

		//Stop Hot Air if hit time
		if ( (bEnableHotAir == TRUE) && (lRunTime >= lHotAirTime) )
		{
			SetHotAir(WL_OFF);
		}
	}

	//Continue Hot air if necessary
	if (bEnableHotAir == TRUE)
	{
		if (lHotAirTime > lRunTime)
		{
			Sleep(lHotAirTime - lRunTime);
			SetHotAir(WL_OFF);
		}
		else
		{
			SetHotAir(WL_OFF);
		}
	}

	//Delay for mylar paper cool down
	if (m_lMylarCoolTime2 > 0)
	{
		Sleep(m_lMylarCoolTime2);
	}

	return TRUE;
}


INT CWaferLoader::HouseKeeping_WT2(BOOL bOnFrameVacuum, BOOL bGripperHitLimit, BOOL bHomeBHT, BOOL bHomeX)
{
	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping WT2 - Start HouseKeeping");

	if ( IsESDualWL()==FALSE )
		return TRUE;

	if (bGripperHitLimit == TRUE)
	{
		ResetGripperHitLimit();
	}

	//Off Clip
	Sleep(100);
	SetGripper2State(WL_OFF);
	Sleep(100);

	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping WT2 - After Set Gripper State");

	//Move Gripper back to zero
	if (bHomeX == TRUE)
	{
		try 
		{
			//Disable Limit sensor protection
			if (!X2_IsPowerOn())
			{
				//bNeedPowerOn = TRUE;
				m_bComm_X2 = FALSE;		//v3.59
				CMS896AStn::MotionClearError(WL_AXIS_X2, &m_stWLAxis_X2);
				Sleep(100);
				X2_Home();
			}
			else
			{
				X2_Home();
			}

		}
		catch(CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping WT2 - Before Close Expander");

	//Close expander & Gripper down
	if (GetExpType() != WL_EXP_CYLINDER)
	{
		SetExpander2Lock(WL_OFF);
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping WT2 - Before Set Gripper Level");

	SetGripper2Level(WL_OFF);
	Sleep(500);

	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping WT2 - Before BondArmMoveToPrePick");

	if (bHomeBHT && (m_bExpander2Status == WL_DOWN))	//Re-use bHomeBHT
	{
		Sleep(500);	
		BondArmMoveToPrePick();
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping WT2 - After BondArmMoveToPrePick");

	if ( ((GetExpType() != WL_EXP_VACUUM) && (GetExpType() != WL_EXP_CYLINDER)) && IsExpander2Lock())
	{
		Sleep(1000);
		if (IsExpander2Lock())
		{
			SetErrorMessage("Expander2 unlock failed");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
		}
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("HouseKeeping WT2 - Complete HouseKeeping");
	return TRUE;
}


INT CWaferLoader::UnloadFilmFrame_WT2(BOOL bStartFromHome, BOOL bBurnIn)
{
	BOOL bFrameDetected = FALSE;
	INT lStatus;
	LONG lLoadPosX, lUnloadPosX;
	LONG lUnloadFrameTime;
	BOOL bOpenGripperDuringUnload;

	if ( IsESDualWL()==FALSE )
		return TRUE;

	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus("Unload WT2 start");

	if( m_bFrameOperationInAutoMode==FALSE )
	{
		if( WPR_MoveFocusToSafe()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL AOI AF Z not at safe");
			return Err_AutoFocus_Z_Fail;
		}
	}

	//Check Banana scope
	if ( IsScopeDown() == TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Scope is down");
		SetErrorMessage("Scope is down");
		
		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}

	//Check Magazine is inside the loader Z
	if ( (IsMagazineExist2() == FALSE) && (bBurnIn == FALSE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Magazine WT2 not exist");
		SetErrorMessage("Magazine WT2 not exist");
		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	//v3.92
	// Check frame is on gripper before moving the gripper
	if (IsFrameDetect2() == TRUE && m_bNoSensorCheck == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper 2 detected frame on gripper before UNLOAD");
		SetErrorMessage("wafer gripper 2 detected frame on gripper before UNLOAD");
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		return Err_FrameExistInGripper;
	}

	//v3.97	//Lextar
	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && IsExpander2Lock() )	
	{
		SetExpander2Lock(WL_OFF);
		Sleep(500);
		if (IsExpander2Lock())
		{
			SetErrorMessage("Expander2 unlock fail at Unload FilmFrame");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			return Err_ExpanderLockFailed;
		}
	}


	//Move Elevator to current slot + unload offset
	CString szMsg;
	szMsg.Format("Unload Move to WT2 mag %d slot %d;%s", m_lCurrentMagNo2, m_lCurrentSlotNo2, m_szWT2YieldState);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	if ( IsES101() && (m_bUseMagazine3AsX || m_bUseAMagazineAsG) )	// X_Magazine unload 2
	{
		if ( m_bUseMagazine3AsX && m_szWT2YieldState == "N" )
		{
			if ( m_lCurrentMagazineX2SlotNo> m_stWaferMagazine2[m_lMagazineX_No-1].m_lNoOfSlots)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("move to WT2 X mgzn slot already full");
				HmiMessage_Red_Back("Low yield magazine 2 already full.", "AOI Scan");
				lStatus = FALSE;
			}
			else
			{
				szMsg.Format("Unload Move to WT2 X mag %d slot %d", m_lMagazineX_No, m_lCurrentMagazineX2SlotNo);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				lStatus = MoveToMagazineSlot2(m_lMagazineX_No, m_lCurrentMagazineX2SlotNo, m_lUnloadOffset2, TRUE);
				if ( (IsMagazineExist2() == FALSE) && (bBurnIn == FALSE) )
				{
					CString szMsg = "WL2 Magazine X not exist";
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
					SetErrorMessage(szMsg);
					SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
					HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
					return Err_NoMagazineExist;
				}

				m_lCurrentMagazineX2SlotNo += (m_lSkipSlotNo2 + 1);
				if ( m_lCurrentMagazineX2SlotNo> m_stWaferMagazine2[m_lMagazineX_No-1].m_lNoOfSlots)
				{
					HmiMessage_Red_Back("Low yield magazine 2 full.", "AOI Scan");
				}
				m_szWT2YieldState = "";	// should check unload success or not
			}
		}
		else if ( m_bUseAMagazineAsG && m_szWT2YieldState == "Y" )
		{
			if ( m_lMagazineG2CurrentSlotNo> m_stWaferMagazine2[m_lMagazineG_No-1].m_lNoOfSlots)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("move to WT2 G mgzn slot already full");
				HmiMessage_Red_Back("Good yield magazine 2 already full.", "AOI Scan");
				lStatus = FALSE;
			}
			else
			{
				szMsg.Format("Unload Move to WT2 G mag %d slot %d", m_lMagazineG_No, m_lMagazineG2CurrentSlotNo);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
				lStatus = MoveToMagazineSlot2(m_lMagazineG_No, m_lMagazineG2CurrentSlotNo, m_lUnloadOffset2, TRUE);
				if ( (IsMagazineExist2() == FALSE) && (bBurnIn == FALSE) )
				{
					CString szMsg = "WL2 Magazine G not exist";
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
					SetErrorMessage(szMsg);
					SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);
					HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
					return Err_NoMagazineExist;
				}

				m_lMagazineG2CurrentSlotNo += (m_lSkipSlotNo2 + 1);
				if ( m_lMagazineG2CurrentSlotNo> m_stWaferMagazine2[m_lMagazineG_No-1].m_lNoOfSlots)
				{
					HmiMessage_Red_Back("Good yield magazine 2 full.", "AOI Scan");
				}
				m_szWT2YieldState = "";	// should check unload success or not
			}
		}
		else
		{
			m_szWT2YieldState = "";	// should check unload success or not
			lStatus = MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2, m_lUnloadOffset2, FALSE);		//v2.93T2
		}
	}
	else
	{
		lStatus = MoveToMagazineSlot2(m_lCurrentMagNo2, m_lCurrentSlotNo2, m_lUnloadOffset2, FALSE);		//v2.93T2
		m_szWT2YieldState = "";	// should check unload success or not
	}

	if ( lStatus != TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("move to WT2 mgzn slot fail");
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return lStatus;
	}

	//(Move Gripper to pre-unload position & move wafer table at the same time)
/*
	if (GetExpType() == WL_EXP_VACUUM)
	{
		FilmFrameVacuumReady();
		if ( m_bMapWfrCass == FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Vacuum Expander Ready");
			ExpanderVacuumPlatform(WL_UP, TRUE);
			CMSLogFileUtility::Instance()->WL_LogStatus("Vacuum Expander UP");
		}
		else
		{
			ExpanderVacuumPlatform(WL_UP, FALSE);
		}

		//Align Frame
		AlignWaferFrame();
		//Enable Limit sensor protection
		//m_pStepper_X->EnableProtection(HP_LIMIT_SENSOR, TRUE);
		//CMS896AStn::MotionEnableProtection("WaferGripperAxis", HP_LIMIT_SENSOR, TRUE);

		//Move Gripper to unload pos & do searching
		X_MoveTo(m_lUnloadPos_X+WL_GRIPPER_SEARCH_DIST);
	}
	else
*/
	if( m_lFrameToBeUnloaded==2 )
		m_lFrameToBeUnloaded = 0;
	{
		// Check Expander open or not before moving gripper
		INT nStatus = 0;

		CMSLogFileUtility::Instance()->WL_LogStatus("Move WT2 T to 0");
		MoveWafer2Theta(0);

		if( IsMotionCE() )
		{
			CString szMsg;
			szMsg = "Motin Critical Error in Unload film frame WT2";
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			SetErrorMessage(szMsg);
			HmiMessage_Red_Back(szMsg, "Wafer Loader");
			return Err_ExpanderLockFailed;
		}

		if (GetExpType() != WL_EXP_CYLINDER)
		{
			if (IsExpander2Lock() == FALSE)
			{
				SetExpander2Lock(WL_ON);
				Sleep(400);

				if (IsExpander2Lock() == FALSE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("WL2 expander lock fail");
					SetAlert_Red_Yellow(IDS_WL_EXP_LOCK_FAIL);
					return Err_ExpanderLockFailed;
				}
			}
		}

		if ( (IsExpander2Open() == TRUE))
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 already open");
			SetErrorMessage("Expande2 already open");

			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderAlreadyOpen;
		}
	
		if (CMS896AApp::m_bMS100Plus9InchOption)
		{
			if (FilmFrameGearReady2(SFM_WAIT, FALSE, FALSE) == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL2 film frame gear ready false");
				HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
				return FALSE;
			}
		}
		else
		{
			//	4.24TX wait now
			if (FilmFrameGearReady2(SFM_WAIT, FALSE, FALSE) == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL2 film frame gear ready false");
				HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
				return FALSE;
			}
		}
		

		//Move Gripper to unload pos & do searching
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 to UNLOAD");
		
		if (MotionIsServo(WL_AXIS_X2, &m_stWLAxis_X2))	//MS899_SVO_MOTOR		//v3.59
		{
			if (m_lUnloadPos_X2+WL_GRIPPER_SVO_SEARCH_DIST_ON_TABLE > 0)
			{
				X2_MoveTo(0, SFM_NOWAIT);
			}
			else
			{
				X2_MoveTo(m_lUnloadPos_X2 + WL_GRIPPER_SVO_SEARCH_DIST_ON_TABLE, SFM_NOWAIT);
			}
		}
		else
		{
			if (GetExpType() == WL_EXP_CYLINDER)
			{
				lUnloadPosX = m_lUnloadPos_X2 + WL_GRIPPER_CYLINDER_SEARCH_DIST_ON_TABLE;	
			}
			else if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
			{
				lUnloadPosX = m_lUnloadPos_X2 + WL_GRIPPER_DIRECTRING_SEARCH_DIST_ON_TABLE;	
			}
			else 
			{
				if (CMS896AApp::m_bMS100Plus9InchOption)	//MS100 9Inch	
					lUnloadPosX = m_lUnloadPos_X2 + WL_GRIPPER_SEARCH_DIST_ON_TABLE * 2;		//Gripper X ENC resolution is doubled
				else
					lUnloadPosX = m_lUnloadPos_X2 + WL_GRIPPER_SEARCH_DIST_ON_TABLE;
			}

			if (lUnloadPosX > 0)
			{
				lUnloadPosX = 0;
			}
			
			X2_MoveTo(lUnloadPosX, SFM_NOWAIT);
		}

		// Sync Wafer Table Before Move
		SyncWaferTable2XYT("XY");
		SetGripper2Level(WL_ON);

		// Open the Expander After Moved the Wafer Table
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 is moving UP");
		
		if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
			 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28T4
		{
			nStatus = ExpanderDCMotor2Platform(WL_UP, TRUE, TRUE, FALSE, m_lExpDCMotorDacValue);
		}
		else if (GetExpType() == WL_EXP_CYLINDER)
		{
			nStatus = ExpanderCylinderMotorPlatform2(WL_UP, TRUE, TRUE, FALSE);		//not yet in-use 
		}
		else
		{
			//nStatus = ExpanderGearPlatform(WL_UP, TRUE, TRUE, FALSE);				//not used in MS100Plus
		}

		if ( nStatus == Err_ExpanderAlreadyOpen )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 already open");
			SetErrorMessage("Expander2 already open");

			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderAlreadyOpen;
		}
		else if ( nStatus == Err_ExpanderOpenCloseFailed )	
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 open timeout");
			SetErrorMessage("Expander2 open timeout");

			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
			HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}
		else if ( nStatus != TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot open");
			SetErrorMessage("Expander2 cannot open");

			SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);
			HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
			return Err_ExpanderOpenCloseFailed;
		}

		//v3.19T1
		if (!IsAllMotorsEnable2())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Not all WL2 motors enabled");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			return Err_FrameJammed;
		}

		X2_Sync();
	}

	if (IsFrameJam2() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to Unload");
		SetErrorMessage("Gripper2 is Jam @ to Unload");

		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	if (GetExpType() == WL_EXP_VACUUM)
	{
		//FilmFrameSearch();
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 search frame on table");
		FilmFrameSearchOnTable2();
	}

	m_bFrameExistOnExp2 = FALSE;			//v2.64
	CMSLogFileUtility::Instance()->WL_LogStatus("Save Data 2");

	if (IsFrameJam2() == TRUE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to Unload 2");
		SetErrorMessage("Gripper2 is Jam @ to Unload 2");

		SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
		HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
		return Err_FrameJammed;
	}

	//v3.19T1
	if (!IsAllMotorsEnable2())
	{
		SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
		return Err_FrameJammed;
	}

	//Check Frame detect sensor
	CMSLogFileUtility::Instance()->WL_LogStatus("Check frame exist on gripper2");
	//SetErrorMessage("Check frame exist on gripper");
	bFrameDetected = IsFrameDetect2();

	// hard code for test
	//bFrameDetected = TRUE;
	if (bBurnIn == TRUE)
	{
		bFrameDetected = TRUE;
	}
	else
	{
		if (!bFrameDetected)
		{
			Sleep(200);
			FilmFrameSearchOnTable2();
			Sleep(200);
			bFrameDetected = IsFrameDetect2();
		}
	}

	Z2_Sync();

	if (bFrameDetected == TRUE || m_bNoSensorCheck == TRUE)
	{
		if (IsFrameJam2() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ frame detected");
			SetErrorMessage("Gripper2 is Jam @ frame detected");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		//Offset for Unload Position
		/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 step on frame");	/*******/ //v2.93T1
		if (m_lStepOnUnloadPos2 != 0)
		{
			X2_Move(m_lStepOnUnloadPos2);
		}

		//On Clip
		if (m_bOpenGripper2InUnload == FALSE)
		{
			SetGripper2State(WL_ON);
		}
		else
		{
			SetGripper2State(WL_OFF);
		}

		//Move Gripper to load pos
		/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 move to LOAD");	/*******/ //v2.93T1

		//v3.19T1
		if (!IsAllMotorsEnable2())
		{
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			return Err_FrameJammed;
		}

		lLoadPosX = m_lLoadPos_X2;

		if (GetExpType() == WL_EXP_CYLINDER)
		{
			bOpenGripperDuringUnload = TRUE;
			GetEncoderValue();
			lUnloadFrameTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X2, "mpfWaferGripper", 
				(m_lEnc_X2 - m_lLoadPos_X2), (m_lEnc_X2 - m_lLoadPos_X2), HIPEC_SAMPLE_RATE, &m_stWLAxis_X2);
		}
		else
		{
			bOpenGripperDuringUnload = FALSE;
			lUnloadFrameTime = 0;
		}

		if (WaferGripper2MoveSearchJam(lLoadPosX, bOpenGripperDuringUnload, lUnloadFrameTime) == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to load");
			SetErrorMessage("Gripper2 is Jam @ to load");

			if (GetExpType() != WL_EXP_VACUUM)
			{
				ResetGripper2HitLimit();
				SetGripper2State(WL_OFF);
				SetGripper2Power(FALSE);		//v3.89	//Re-enable for safety purpose
			}

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			if( IsAOIOnlyMachine() )
			{
				if( HmiMessage_Red_Back("Please unload it into magazine by hand.", "Unload WT2", glHMI_MBX_YESNO)==glHMI_YES )
				{
					m_bFrameLoadedOnWT2	= FALSE;
				}
			}
			
			//v2.93T2
			if (GetExpType() == WL_EXP_VACUUM)
			{
				HouseKeeping_WT2(WL_ON, TRUE, FALSE, FALSE);
			}
			else
			{
				ResetGripper2HitLimit();
				SetGripper2State(WL_OFF);
				
				if (GetExpType() != WL_EXP_CYLINDER)
				{
					SetExpander2Lock(WL_OFF);
				}

				Sleep(500);
			}

			return Err_FrameJammed;
		}


		if (GetExpType() != WL_EXP_CYLINDER)
		{
			//Off Clip
			Sleep(100);
			SetGripper2State(WL_OFF);
			Sleep(100);
		}

		if (!IsAllMotorsEnable2())
		{
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			return Err_FrameJammed;
		}

		//Move Gripper to load pos
		/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 back to ready"); /*******/	//v2.93T1

		if (IsES101() && !bStartFromHome && (lLoadPosX < -10000))
			X2_MoveTo(lLoadPosX + 5000, SFM_NOWAIT);
		else
			X2_MoveTo(m_lReadyPos_X2, SFM_NOWAIT);
		SaveData();
		X2_Sync();
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No frame is detected 2");
		SetErrorMessage("No frame is detected 2");
		SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
	}


	if ((bFrameDetected == FALSE) || ((bStartFromHome == TRUE) && (bBurnIn == FALSE)))
	{
		if (bFrameDetected == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("No frame detected");
			SetErrorMessage("No frame is detected");
			HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
			return Err_NoFrameDetected;
		}
		else 
		{
			if (GetExpType() != WL_EXP_VACUUM)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("expander2 is moving down");

				if (CMS896AStn::MotionIsServo(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE)
				{
					X2_MoveTo(WL_GRIPPER_SRV_HOME_OFFSET_POS);
				}
				else
				{
					X2_MoveTo(-1000);
				}

				INT nStatus = 0;
				if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR) ||
					 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28T4
				{
					nStatus = ExpanderDCMotor2Platform(WL_DOWN, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
				}
				else if (GetExpType() == WL_EXP_CYLINDER)
				{
					nStatus = ExpanderCylinderMotorPlatform2(WL_DOWN, TRUE, FALSE, FALSE);
				}
				else
				{
					//nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, FALSE, FALSE);
				}

				if (nStatus != 1)
				{
					/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot close"); /*******/
					SetErrorMessage("Expander2 cannot close");
					SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);	
					HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
					return Err_ExpanderOpenCloseFailed;
				}

				HouseKeeping_WT2(WL_OFF, FALSE, TRUE, TRUE);
			}
			else
			{
				HouseKeeping_WT2(WL_OFF, FALSE, TRUE, TRUE);
			}
		}

		if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && (IsExpander2Lock() == TRUE) )
		{
			Sleep(1000);
			if (IsExpander2Lock())	
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 unlock failed at UnLoadFilmFrame");
				SetErrorMessage("Expander2 unlock failed");
				SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
				return Err_ExpanderLockFailed;
			}
		}
	}

	/*******/ CMSLogFileUtility::Instance()->WL_LogStatus("Unload WT2 finish\n"); /*******/

	//Log into History file
	if (CMS896AStn::m_bForceClearMapAfterWaferEnd == FALSE && IsES101()==FALSE )
	{
		LogWaferEndInformation(m_lCurrentSlotNo, m_szBarcodeName2);
	}

	m_bFrameLoadedOnWT2	= FALSE;

	return TRUE;
}


INT CWaferLoader::LoadFilmFrame_WT2(BOOL bStartFromHome, BOOL bBurnIn, BOOL bSearchFrameInMagazine, BOOL bSearhHomeDie, 
									BOOL bBarCodeOnTable, BOOL bDisplayMsg)
{
	INT nStatus = 0;
	BOOL bFrameDetected = FALSE;
	BOOL bFrameExisted = FALSE;
	BOOL bWaferNameFound = TRUE;
	BOOL bRepeatLoadFrame = FALSE;
	LONG nSearchStatus = 0;
	INT nFrameStatus = 0;
	INT nBarcodeStatus = 0;
	BOOL bPushBackAbort	= FALSE;
	BOOL bRtn = FALSE, bReturn = FALSE;
	CString szCompareBarcode;
	BOOL bScanBarcodeOnTable2 = FALSE;
	BOOL bKeepBarcodePos = FALSE;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( IsESDualWT()==FALSE )
		return FALSE;

	if (m_bUseBarcode == TRUE )
	{
		if (WaferEndFileGeneratingCheckingCmd() == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Wafer end file generating check failed");
			return FALSE;
		}

		if( IsES101()==FALSE )
			ClearMapFile();
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("");
	CMSLogFileUtility::Instance()->WL_LogStatus("WL2 Load start");

	if( m_bFrameOperationInAutoMode==FALSE )
	{
		if( WPR_MoveFocusToSafe()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WL2 load AOI move focus to safe fail");
			return Err_AutoFocus_Z_Fail;
		}
	}

	//Check Banana scope
	if ( IsScopeDown() == TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Scope is down");
		SetErrorMessage("Scope is down");

		SetAlert_Red_Yellow(IDS_WL_SCOPE_DOWN);
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return Err_ScopeDown;
	}

	//Make sure gripper on safe posn before performing LOAD
	if (!X2_IsPowerOn())
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper 2 power not ON");
		HmiMessage_Red_Yellow("Gripper 2 power not ON!", "Load Film Error", glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
		return FALSE;
	}

	//Check Magazine is inside the loader Z
	if ( (IsMagazineExist2() == FALSE) && (bBurnIn == FALSE) )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Magazine2 is not exist");
		SetErrorMessage("Magazine2 is not exist");

		SetAlert_Red_Yellow(IDS_WL_NO_MAG_INSIDE);	
		HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
		return Err_NoMagazineExist;
	}

	// Check frame is on gripper before moving the gripper
	if (IsFrameDetect2() == TRUE && m_bNoSensorCheck == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("wafer gripper2 detected frame on gripper before LOAD");
		SetErrorMessage("wafer gripper2 detected frame on gripper before LOAD");
		SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
		return Err_FrameExistInGripper;
	}

	//v3.97	//Lextar
	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && IsExpander2Lock() )	//OsramPenang
	{
		SetExpander2Lock(WL_OFF);
		Sleep(500);
		if (IsExpander2Lock())
		{
			SetErrorMessage("Expander2 unlock fail at Load FilmFrame");
			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);	
			return Err_ExpanderLockFailed;
		}
	}


	if (bStartFromHome == TRUE)
	{
		if (GetExpType() == WL_EXP_VACUUM)
		{
			//FilmFrameVacuumReady();	
		}
		else
		{
			if (FilmFrameGearReady2()== FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WL2 load fileframe gear ready fail");
				HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
				return FALSE;
			}

			INT nStatus = 0;

			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 starts to gear UP...");

			if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
				 (GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28T4
			{
				nStatus = ExpanderDCMotor2Platform(WL_UP, TRUE, FALSE, FALSE, m_lExpDCMotorDacValue);
			}
			else if (GetExpType() == WL_EXP_CYLINDER)
			{
				nStatus = ExpanderCylinderMotorPlatform2(WL_UP,TRUE, FALSE, FALSE);
			}
			else
			{
				//nStatus = ExpanderGearPlatform(WL_UP, TRUE, FALSE, FALSE);		//v2.56
			}

			if ( nStatus == Err_ExpanderAlreadyOpen )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 already open");
				SetErrorMessage("Expander2 already open");
				SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
				HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
				return Err_ExpanderAlreadyOpen;
			}
			else if ( nStatus != TRUE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot open");
				SetErrorMessage("Expander2 cannot open");

				SetAlert_Red_Yellow(IDS_WL_EXP_OPEN_FAIL);	
				HouseKeeping_WT2(WL_OFF, FALSE, FALSE, FALSE);
				return Err_ExpanderOpenCloseFailed;
			}

			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 at UP posn");
		}

		//Move Gripper to load pos
		nSearchStatus = MoveSearchFrameExistAndJam2(m_lReadyPos_X2);
		if ( nSearchStatus == Err_FrameJammed )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to ready");
			SetErrorMessage("Gripper2 is Jam @ to ready");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}
		else if ( nSearchStatus == Err_FrameExistInGripper )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Frame exist on gripper2");
			SetErrorMessage("Frame exist on gripper2");

			SetAlert_Red_Yellow(IDS_WL_FRAME_EXISTONGRIPPER);
			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameExistInGripper;
		}

		if (IsFrameJam2() == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ to ready");
			SetErrorMessage("Gripper2 is Jam @ to ready");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		if (!IsAllMotorsEnable2())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is OFF @ to ready");
			SetErrorMessage("Gripper2 is OFF @ to ready");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			return Err_FrameJammed;
		}
	}

	//Get next frame & scan barcode
	CMSLogFileUtility::Instance()->WL_LogStatus("Get next frame WT2 start");

	if (GetExpType() == WL_EXP_CYLINDER)
	{
		SetGripper2State(WL_ON);
	}

	while(1)
	{
		bRepeatLoadFrame = FALSE;

		// Check frame is on gripper before moving the magazine
		if (IsFrameDetect2() == TRUE && m_bNoSensorCheck == FALSE)
		{
			SetAlert_Red_Yellow(IDS_WL_FRAME_IS_DETECTED);
			return Err_FrameExistInGripper;
		}

		//Get next Frame
		CString szErr;
		nFrameStatus = GetNextFilmFrame2(bBurnIn, bSearchFrameInMagazine, bDisplayMsg);
		switch(nFrameStatus)
		{
		case TRUE:	 
			bFrameDetected = TRUE;
			break;

		case FALSE:
			bFrameDetected = FALSE;
			break;

		default:
			SaveData();
			szErr.Format("Get next frame 2fail; CODE = %d", nFrameStatus);
			CMSLogFileUtility::Instance()->WL_LogStatus(szErr);	
			return nFrameStatus;
		}
		

		if (!IsAllMotorsEnable2())	
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is OFF @ at LOAD");
			SetErrorMessage("Gripper2 is OFF @ at LOAD");
			SetAlert_Red_Yellow(IDS_WL_MOTOR_OFF);
			HouseKeeping_WT2(WL_ON, TRUE, FALSE, TRUE);
			return Err_FrameJammed;
		}

		m_bIsGetCurrentSlot2 = FALSE;

		//Get frame inside the expander
		if (bFrameDetected == TRUE)
		{
			//Offset for Load Position & On Clip
			X2_Move(m_lStepOnLoadPos2);

			if (GetExpType() != WL_EXP_CYLINDER)
			{
				SetGripper2State(WL_ON);
				Sleep(200);
			}

			if (m_bUseBarcode == TRUE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Start WL2 barcode scan...");
				
				if (m_bUseBCInCassetteFile == FALSE)
				{
					nBarcodeStatus = ScanningBarcode2(TRUE, FALSE, m_bCompareBarcode);

					if (nBarcodeStatus != TRUE && m_bReadBarCodeOnTable)
					{
						bScanBarcodeOnTable2 = TRUE;
						CMSLogFileUtility::Instance()->WL_LogStatus("WL2 gripper scan barcode fail, into scan on table");
					}

					if (bScanBarcodeOnTable2 == FALSE)
					{
						if (nBarcodeStatus == TRUE && m_bCompareBarcode == TRUE)
						{
							szCompareBarcode = m_szBarcodeName2;

							// Temp to disable the function
							nBarcodeStatus = ScanningBarcode2(TRUE, TRUE, m_bCompareBarcode);
							
							if (szCompareBarcode != m_szBarcodeName2)
							{
								nBarcodeStatus = Err_COMPARE_BC_FAILED;
							}
						}
					}
				}
				else
				{
					nBarcodeStatus = GetBarcodeInCassette(m_szBarcodeName2, m_lCurrentSlotNo2);

					if (nBarcodeStatus == FALSE)
					{
						SetAlert_Red_Yellow(IDS_WL_BCFILE_NO_BARCODE);
					}
				}

				//Skip below action if burn-in
				if ( bBurnIn == TRUE )
				{
					break;
				}

				if (bScanBarcodeOnTable2 == FALSE)
				{
					//Check Wafer Lot
					if ((bWaferNameFound = CheckWaferLotData(m_szBarcodeName2, TRUE)) == FALSE) 
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Check WaferLot Data");

						CString szMsg;
						szMsg.Format("Slot %d, Wafer ( ", m_lCurrentSlotNo2);
						szMsg += m_szBarcodeName2 + " ) not found in Lot\nPlease Check!\n\n\nCaution: Wafer will push back into magazine!";
							
						CMSLogFileUtility::Instance()->WL_LogStatus("No WaferName Found on LotInfo");
						SetErrorMessage("No WaferName Found on LotInfo");
						SetAlert_Msg_Red_Yellow(IDS_WL_NO_WAFER_IN_LOT, szMsg);		

						//Push frame back into magazine
						nFrameStatus = PushFrameBack2();
						if ( nFrameStatus != TRUE )
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("load frame 2 status not true");
							return nFrameStatus;
						}

						bPushBackAbort = TRUE;	
					}

					// Check compare barcode
					if (nBarcodeStatus == Err_COMPARE_BC_FAILED)
					{
						PushFrameBack2();
						SetAlert_Red_Yellow(IDS_WL_COMPARE_BC_FAILED);

						SetErrorMessage("Compare Barcode 2 Failed! First Barcode:"+ szCompareBarcode 
							+ "Second Barcode:" + m_szBarcodeName2);

						HouseKeeping_WT2(WL_ON, FALSE, FALSE, TRUE);
						return Err_COMPARE_BC_FAILED;
					}

					//Empty barcode check
					if ( (m_bBarcodeCheck == TRUE) && (nBarcodeStatus == FALSE) )
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Empty Barcode checking");

						CString szText = " ";	
						LONG lSelection = 0; 

						SetAlert_Red_Yellow(IDS_WL_NO_BARCODE);		

						lSelection = SetAlert_Msg(IDS_WL_RETRY_GET_FRAME, szText, "Continue", "Stop", "Ignore");		 

						//Push frame back into magazine if (Continue or Stop) is pressed
						if ( (lSelection == 1) || (lSelection == 5) )
						{
							if (lSelection == 5)	//ABORT	
							{
								bPushBackAbort = TRUE;
							}
							nFrameStatus = PushFrameBack2();
							if ( nFrameStatus != TRUE )
							{
								CMSLogFileUtility::Instance()->WL_LogStatus("load frame 2 status not true");
								return nFrameStatus;
							}
						}

						//Search next frame in magazine if continue is pressed
						if ( lSelection == 1 )		//CONTINUE
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Get Next Frame 2");

							X2_MoveTo(m_lReadyPos_X2);
							Sleep(500);
							bSearchFrameInMagazine = TRUE;
							bRepeatLoadFrame = TRUE;
							continue;
						}
						else
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("WL2 non scan on table, Stop or Ignore");
						}
					}

					//v2.78T1
					CString szMsg = "WT2 Barcode loaded = " + m_szBarcodeName2;
					CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

					//ANDREW_SC
					if ( m_bMapWfrCass == TRUE )
					{
						LONG lCurrSlotNo = GetCurrSlotNo();
						m_szWfrBarcodeInCass[lCurrSlotNo] = m_szBarcodeName2;
					}
				}
			}
		}

		CMSLogFileUtility::Instance()->WL_LogStatus("Get next frame 2 finish");
		SaveData();

		if (bFrameDetected == TRUE || m_bNoSensorCheck == TRUE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 to UNLOAD...");

			//Move Gripper to load pos
			X2_MoveTo(m_lUnloadPos_X2);

			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 to UNLOAD Complete...");

			//Get more into expander
			if (GetExpType() == WL_EXP_VACUUM)
			{
				FilmFrame2DriveIn(HP_POSITIVE_DIR, 30, 50, TRUE);
			}
			else if (GetExpType() != WL_EXP_GEAR_DIRECTRING)
			{
				FilmFrame2DriveIn(HP_POSITIVE_DIR, 200, 50, TRUE);
			}

			if (GetExpType() == WL_EXP_VACUUM)
			{
				//bFrameExisted = (IsFramePosition() && IsFrameProtection());	//Check Frame Position sensor
			}
			else
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Check Frame 2 Exist");
				bFrameExisted = IsFrameExist2();		//Check Frame Exist sensor
				CMSLogFileUtility::Instance()->WL_LogStatus("Check Frame 2 Exist Complete");
				CString szMsg;
				szMsg.Format("wl frame 2 exist load a = %d", bFrameExisted);
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
			}

			if ( (bBurnIn == TRUE) || (bWaferNameFound == FALSE) )
			{
				bFrameExisted = TRUE;
			}
		}
		else
		{
			if (!bPushBackAbort)	//However, PushBack Abort will not go here because bFrameDetect is still TRUE at this point
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("No frame is detected");
				SetErrorMessage("No frame is detected");
				SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
			}
		}

		if ((bFrameDetected == TRUE) && (bFrameExisted = TRUE))
		{
			if (GetExpType() == WL_EXP_VACUUM)
			{
	/*
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander starts to move down...");
				ExpanderVacuumPlatform(WL_DOWN, TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander at down posn...");

				//Off Clip
				Sleep(100);
				SetGripperState(WL_OFF);
				Sleep(100);

				//Align Frame
				AlignWaferFrame(TRUE);
				if ( (HouseKeeping(WL_ON, FALSE, TRUE, TRUE) != 1) && (bBurnIn == FALSE) )
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Expander cannot close");
					SetErrorMessage("Expander cannot close");
					SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);	
					return Err_ExpanderOpenCloseFailed;
				}
	*/
			}
			else
			{
				//Off Clip
				Sleep(100);
				BOOL bIsFrameDetect = FALSE;
				INT nCounter = 0;
				if( GetExpType()==WL_EXP_GEAR_DIRECTRING )
				{
					while( bIsFrameDetect==FALSE )
					{
						if (IsFrameDetect2() == TRUE)
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("WL2 frame in clipper before open");
							bIsFrameDetect = TRUE;
							break;
						}

						Sleep(10);
						nCounter++;
						if( nCounter>=10 )
						{
							bIsFrameDetect = FALSE;
							break;
						}
					}
				}
				CMSLogFileUtility::Instance()->WL_LogStatus("Start Open Gripper 2 State");
				SetGripper2State(WL_OFF);
				Sleep(100);

				// add retry on frame detect
				nCounter = 0;
				while( bIsFrameDetect==FALSE )
				{
					if (IsFrameDetect2() == TRUE)
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("WL2 frame in clipper after open");
						bIsFrameDetect = TRUE;
						break;
					}

					Sleep(10);
					nCounter++;

					if (nCounter >= 100)
					{
						bIsFrameDetect = FALSE;
						break;
					}
				}

				//Check Frame detect sensor again to ensure the frame is in right position
				if ((bIsFrameDetect == FALSE && bBurnIn == FALSE) && m_bNoSensorCheck == FALSE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("WL2 frame detect fail on wafer table");
					if (!bPushBackAbort)
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Cannot detect frame again");
						SetAlert_Red_Yellow(IDS_WL_NO_FRAME_DETECT);
					}
					
					if (bPushBackAbort)		//v4.01T1
					{
						//safe to Down expander here if pushback frame to STOP	//Cree China
						if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
							(GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28T4
						{
							ExpanderDCMotor2Platform(WL_DOWN, FALSE, FALSE, FALSE, m_lExpDCMotorDacValue);
						}
						else if (GetExpType() == WL_EXP_CYLINDER)
						{
							nStatus = ExpanderCylinderMotorPlatform2(WL_DOWN, FALSE, FALSE, FALSE);
						}
						else
						{
							//ExpanderGearPlatform(WL_DOWN, FALSE, FALSE, FALSE);
						}
					}
					Sleep(200);
					HouseKeeping_WT2(WL_OFF, TRUE, FALSE, TRUE);	//expander doesn't close to avoid breaking plastic frame
					return Err_NoFrameExist;
				}

				SendGemEvent_WL_Load(TRUE);

				X2_MoveTo(0);	//-250);	//modified as advised by production

				CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 starts to gear down...");

				if ( (GetExpType() == WL_EXP_GEAR_NUMOTION_DCMOTOR)	||
					(GetExpType() == WL_EXP_GEAR_DIRECTRING) )				//v4.28T4
				{
					nStatus = ExpanderDCMotor2Platform(WL_DOWN, TRUE, bWaferNameFound, TRUE, m_lExpDCMotorDacValue);
				}
				else if (GetExpType() == WL_EXP_CYLINDER)
				{
					nStatus = ExpanderCylinderMotorPlatform2(WL_DOWN, TRUE, bWaferNameFound, TRUE);
				}
				else
				{
					//nStatus = ExpanderGearPlatform(WL_DOWN, TRUE, bWaferNameFound, TRUE);
					// no gear platform type
					return FALSE;
				}

				if (nStatus != 1)
				{
#ifndef NU_MOTION		//Klocwork	//v4.46
					if (nStatus == Err_NoFrameExist)
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("No Frame Exist");
						SetErrorMessage("No Frame Exist");
						SetAlert_Red_Yellow(IDS_WL_EXP_NO_FRMAE_EXIST);
						HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
						return Err_NoFrameExist;
					}
					else
#endif
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 cannot close");
						SetErrorMessage("Expander2 cannot close");
						SetAlert_Red_Yellow(IDS_WL_EXP_CLOSE_FAIL);
						HouseKeeping_WT2(WL_OFF, FALSE, FALSE, TRUE);
						return Err_ExpanderOpenCloseFailed;
					}
				}

				CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 at down posn...");
				HouseKeeping_WT2(WL_ON, FALSE, TRUE, TRUE);

				if (bScanBarcodeOnTable2 == TRUE && m_bUseBarcode )
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("WL2 Scan Barcode On Table2..");

					nBarcodeStatus = ScanningBarcodeOnTable2WithTheta();
					if( nBarcodeStatus==TRUE )
					{		
						CMSLogFileUtility::Instance()->WL_LogStatus("WL2 scan barcode success:" + m_szBarcodeName2);
						if ((bWaferNameFound = CheckWaferLotData(m_szBarcodeName2, TRUE)) == FALSE) 
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Check WaferLot Data");

							CString szMsg;
							szMsg.Format("Slot %d, Wafer ( ", m_lCurrentSlotNo2);
							szMsg += m_szBarcodeName2 + " ) not found in Lot\nPlease Check!\n\n\nCaution: Wafer will push back into magazine!";
								
							CMSLogFileUtility::Instance()->WL_LogStatus("No WaferName Found on LotInfo");
							SetErrorMessage("No WaferName Found on LotInfo");
							SetAlert_Msg_Red_Yellow(IDS_WL_NO_WAFER_IN_LOT, szMsg);		
							
							return FALSE;
						}		
					}
					else
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("WL2 scan barcode failure");
					}

					//Empty barcode check
					if ( (m_bBarcodeCheck == TRUE) && (nBarcodeStatus == FALSE) )
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("Empty Barcode checking 2");

						CString szText = " ";	
						LONG lSelection = 0; 

						SetAlert_Red_Yellow(IDS_WL_NO_BARCODE);		

						lSelection = SetAlert_Msg(IDS_WL_RETRY_GET_FRAME, szText, "Continue", "Stop", "Ignore");		 

						//Push frame back into magazine if (Continue or Stop) is pressed
						if ( (lSelection == 1) || (lSelection == 5) )
						{
							if (lSelection == 5)	//ABORT	
							{
								bPushBackAbort = TRUE;
							}
							CMSLogFileUtility::Instance()->WL_LogStatus("WL2 select 1 or 5, unload wafer frame");
							nFrameStatus = PushFrameBackFromTable2(bStartFromHome, bBurnIn);
							if ( nFrameStatus != TRUE )
							{
								CMSLogFileUtility::Instance()->WL_LogStatus("load frame 2 status not true");
								return nFrameStatus;
							}
						}

						//Search next frame in magazine if continue is pressed
						if ( lSelection == 1 )		//CONTINUE
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("Get Next Frame 2");

							X2_MoveTo(m_lReadyPos_X2);
							Sleep(500);
							bSearchFrameInMagazine = TRUE;
							bRepeatLoadFrame = TRUE;
						}
						else
						{
							CMSLogFileUtility::Instance()->WL_LogStatus("WL2 scan on table, Stop or Ignore");
						}
					}
				}

				if( m_bUseContour )
				{
					//	CONTOUR	search/learn contour edge, expander down, barcode reading done
					LearnWLContourAuto(TRUE);
				}
			}
		}
		else
		{
			HouseKeeping_WT2(WL_ON, FALSE, FALSE, TRUE);
		}

		if ( bRepeatLoadFrame == FALSE )
		{
			break;
		}
	}

	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && (IsExpanderLock() == TRUE) )
	{
		Sleep(1000);
		if (IsExpander2Lock())
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 unlock failed at Load FilmFrame");
			SetErrorMessage("Expander2 unlock failed");

			SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);
			return Err_ExpanderLockFailed;
		}
	}

	//Exist if burn-in is enable
	if ( bBurnIn == TRUE )
	{
		if( m_bUseDualTablesOption==FALSE )
			WPR_MoveToFocusLevel();
		if (bPushBackAbort)
			m_bFrameExistOnExp2 = FALSE;	
		else
			m_bFrameExistOnExp2 = TRUE;	
		CMSLogFileUtility::Instance()->WL_LogStatus("Load 2 finish");
		m_bFrameLoadedOnWT2 = TRUE;
		return TRUE;
	}


	//Check Frame is exit on expander or not
	if (GetExpType() == WL_EXP_VACUUM)
	{
		//bFrameExisted = IsExpanderVacuum();	
		//m_bFrameExistOnExp2 = bFrameExisted;
	}
	else
	{
/*
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if( pApp->GetCustomerName()=="OSRAM" && FALSE )
		{
			if (GetExpType() != WL_EXP_CYLINDER)
			{
				SetExpander2Lock(WL_ON);
			}

			Sleep(1000);
			bFrameExisted = IsFrameExist2();
			
			if (GetExpType() != WL_EXP_CYLINDER)
	{
				SetExpander2Lock(WL_OFF);
			}

			Sleep(1000);
			CString szMsg;
			szMsg.Format("wl2 frame exist load done = %d", bFrameExisted);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		}
		else
*/
		bFrameExisted = TRUE; 
		if (bPushBackAbort)
			m_bFrameExistOnExp2 = FALSE;	
		else
			m_bFrameExistOnExp2 = bFrameExisted;
	}

	SaveData();

	if (bFrameExisted == FALSE && m_bNoSensorCheck == FALSE)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("No frame is detected inside expander2");
		SetErrorMessage("No frame is detected inside expander2");
		SetAlert_Red_Yellow(IDS_WL_FRAME_LOAD_FAIL);
		return Err_NoFrameExist;
	}

	if ( bFrameDetected == FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("frame detect false");
		return Err_NoFrameDetected;
	}

	if ( bWaferNameFound == FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("frame found false");
		return Err_WaferNameNotFound;
	}


	//MS100 9Inch	
	if (CMS896AApp::m_bMS100Plus9InchOption)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("9Inch table to HOME");
		MoveWaferTableNoCheck(0, 0, FALSE, SFM_WAIT, TRUE);
	}

	if (bScanBarcodeOnTable2 && GetExpType() == WL_EXP_GEAR_DIRECTRING)
	{
		bKeepBarcodePos = TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nRotateUnderCam = pApp->GetProfileInt(gszPROFILE_SETTING, gszES_ROTATE_T_UNDER_CAM, 0);
	if( (IsES101()==FALSE && IsES201()==FALSE) || (nRotateUnderCam==0) )
	{
		AutoRotateWafer2(bKeepBarcodePos);
	}

	(*m_psmfSRam)["WaferTable"]["WT2LoadMgzn"]	= m_lCurrentMagNo2;
	(*m_psmfSRam)["WaferTable"]["WT2LoadSlot"]	= m_lCurrentSlotNo2;
	(*m_psmfSRam)["WaferTable"]["WT2InBarcode"]	= m_szBarcodeName2;

	m_bFrameLoadedOnWT2	= TRUE;
	m_szLoadAlignBarcodeName = m_szBarcodeName2;

	return AlignFrameWafer(bFrameDetected, bFrameExisted, bSearhHomeDie);
	CMSLogFileUtility::Instance()->WL_LogStatus("WL2 Load Done\n");
	return TRUE;
}

LONG CWaferLoader::AlignFrameWafer(BOOL bFrameDetected, BOOL bFrameExisted, BOOL bSearhHomeDie, BOOL bFullyAuto, INT nLoadMapID)
{	//	426TX	2
	int	nConvMap = 0;
	BOOL bIsFullyAuto = bFullyAuto;
	BOOL bEnableSmartWalkInPicknPlace = (BOOL)(LONG)(*m_psmfSRam)["Wafer Table Options"]["SmartWalkInPicknPlace"];
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	BOOL bReturn = FALSE;
	BOOL bBLModule = (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["Enabled"];
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//	here if map preloaded, just swap the map and reset the flag.
	//v4.39T10
	CString szLog;
	szLog.Format("WL Align FrameWafer: Use-BC=%d; PrescanDummyMap=%d; Pick&Place=%d; FullyAuto %d, nLoadMapID = %d", 
		m_bUseBarcode, pUtl->GetPrescanDummyMap(), IsEnablePNP(), bFullyAuto, nLoadMapID); 
	CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

	m_dStartWaferAlignTime = GetTime();	// fully auto or auto align button
	BOOL bToFindHome = bSearhHomeDie && bFrameDetected && bFrameExisted;
	if( (IsEnablePNP() && bEnableSmartWalkInPicknPlace) || pUtl->GetPrescanDummyMap() )
	{
		bIsFullyAuto = FALSE;
		if( IsAOIOnlyMachine() && pUtl->GetPrescanAoiWafer() )
			bIsFullyAuto = TRUE;
	}
	else
	{
		if( bToFindHome && bFullyAuto && m_bUseBarcode==FALSE && IsBurnIn()==FALSE && pApp->GetCustomerName() != "WolfSpeed")
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Cannot start without barcode");
			SetErrorMessage("Cannot start without barcode");
			SetAlert_Red_Yellow(IDS_WL_CANNNOT_START_WITHOUT_BARCODE);
			HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
			return FALSE;
		}
	}

	if (pApp->GetCustomerName() == "WolfSpeed" && m_bUseBarcode==FALSE)
	{
		CString szWSMapName = (*m_psmfSRam)["WaferTable"]["Fix Map Name"];
		LoadMapFileWithoutSyn(szWSMapName);
	}

	BOOL bNeedSyncFirst = FALSE;
	if( pApp->IsRunTimeLoadPKGFile() && IsESMachine() )
	{
		bNeedSyncFirst = TRUE;
	}
	// To load wafer map file
	BOOL bUseBarcode = m_bUseBarcode;
	if( IsMS90HalfSortDummyScanAlign() )
	{
		bUseBarcode = TRUE;
	}
	if( (bUseBarcode || IsBurnIn()) && (bFullyAuto || pUtl->GetPrescanAoiWafer()) )		//v4.44T3	//Bugfix
	{
		if( IsESMachine() && IsBurnIn() )
		{
			nLoadMapID = 0;
			CMS896AApp::m_bMapLoadingFinish = TRUE;
		}
		if( (IsMS90HalfSortDummyScanAlign() || IsES101()) && CMS896AApp::m_bMapLoadingFinish )
		{
			SaveScanTimeEvent("  WFL: align frame wafer to clear map");
			ClearMapFile();
		}

		CString szBarcodeName = m_szLoadAlignBarcodeName;
		if( IsAOIOnlyMachine() && IsBurnIn() )
		{
			szBarcodeName = "AOIBurnIn";
		}
		BOOL bEmptyBarcode = FALSE;
		if( szBarcodeName.IsEmpty() )
		{
			if( pUtl->GetPrescanAoiWafer()==FALSE )
			{
				bEmptyBarcode = TRUE;
			}
			else
			{
				if( IsWT2InUse() )
				{
					if( m_szBarcodeName2.IsEmpty() )
						bEmptyBarcode = TRUE;
					else
						szBarcodeName = m_szBarcodeName2;
				}
				else
				{
					if( m_szBarcodeName.IsEmpty() )
						bEmptyBarcode = TRUE;
					else
						szBarcodeName = m_szBarcodeName;
				}
			}
		}

		if( IsMS90HalfSortDummyScanAlign() )
		{
			bEmptyBarcode = FALSE;
			if( m_bUseBarcode==FALSE )
				szBarcodeName = "";
		}	//
		if (bEmptyBarcode)
		{
			CMS896AStn::m_WaferMapWrapper.InitMap();
			CMSLogFileUtility::Instance()->WL_LogStatus("No barcode found");
			SetErrorMessage("No barcode found");
			SetAlert_Red_Yellow(IDS_WL_MAP_NO_NAME);
			if( IsWT2InUse() )
				HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
			else
				HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
			return FALSE;
		}
		szLog.Format("Load Map without waiting complete:m_bMapLoadingFinish:%d,nLoadMapID:%d",
					CMS896AApp::m_bMapLoadingFinish, nLoadMapID);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLog);
		// Load Map without waiting for it to complete
		if (nLoadMapID == 0 && CMS896AApp::m_bMapLoadingFinish)		//v4.37T12	//Andrew** need to speed up map loading by overlapping with expander DOWN motion in () **//
		{
			if( m_szMapFileName.CompareNoCase(szBarcodeName)==0 )	// same.
			{
				bNeedSyncFirst = FALSE;
			}
			SaveScanTimeEvent("  WFL: align frame wafer to load map " + szBarcodeName);
			nConvMap = LoadMapFileWithoutSyn(szBarcodeName);
		}
		else
		{
			nConvMap = nLoadMapID;
		}
	}

	//Request switch camera to wafer side
	IPC_CServiceMessage stMsg;
	BOOL bCamera = FALSE;
	stMsg.InitMessage(sizeof(BOOL), &bCamera);
	m_comClient.SendRequest(WAFER_PR_STN, "SwitchToTarget", stMsg);

	if( bToFindHome )
	{
		if( WPR_DoAutoFocus(bFullyAuto)==FALSE )
		{
			SetAlert_Red_Yellow(IDS_WL_WPR_AUTO_FOCUS_FAIL);
			CMSLogFileUtility::Instance()->WL_LogStatus("Auto focus fail after load frame");
			return FALSE;
		}
	}

	//Search HomeDie & align wafer
	if( bToFindHome )
	{
		SaveScanTimeEvent("  WFL: align frame wafer start");
		if( bIsFullyAuto )
		{
			if( bNeedSyncFirst )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Sync Load MapFile, check PKG run time loading done");		//v4.39T10
				for(int i=0; i<1000; i++)
				{
					BOOL bDone = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["PKG RunTime Loading done"];
					if( bDone )
					{
						break;
					}
					Sleep(100);
				}
				(*m_psmfSRam)["MS896A"]["PKG RunTime Loading done"] = FALSE;
			}

			if( !IsESMachine() || bNeedSyncFirst )
			{
				// Wait for load map to complete
				SaveScanTimeEvent("  WFL: align frame wafer after sync map");
				CMSLogFileUtility::Instance()->WL_LogStatus("Sync Load MapFile");		//v4.39T10
				if (SyncLoadMapFile(nConvMap) != TRUE )
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("sync first load map file error");
					if( IsWT2InUse() )
						HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
					else
						HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
					return FALSE;
				}
			}

			CreeGenerateParameterList();
		}
		else
		{
			// Checking Load Map Reply
			if (WaferMapHeaderChecking() != TRUE)
			{
				return FALSE;
			}
			if (CheckLoadCurrentMapStatus() != TRUE)
			{
				return FALSE;
			}
			// Auto Load Rank File
			if (AutoLoadRankIDFile() != TRUE)
			{
				return FALSE;
			}
		}

		BOOL bPreScan = IsPrescanEnable();
		IPC_CServiceMessage stMsg1, stMsg2;
		int	nConvID1 = 0, nConvID2 = 0;
		LONG lBlockNo = 0;

		if (bBLModule && !bPreScan && Is180Arm6InchWT() == FALSE)		// Disable Preload in PRESCAN mode
		{
			//Get block num from BT for BL to preload next grade frame in DLA config only
			CMSLogFileUtility::Instance()->WL_LogStatus("load bl module send ipc");
			stMsg1.InitMessage(sizeof(LONG), &lBlockNo);
			nConvID1 = m_comClient.SendRequest(BIN_TABLE_STN, "GetNewMapBlockInUse", stMsg1);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID1, 360000000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID1, stMsg1);
					stMsg1.GetMsg(sizeof(LONG), &lBlockNo);
					break;
				}
				else
				{
					Sleep(1);		
				}
			}

			if (lBlockNo != -1)
			{
				CString szGrade;
				szGrade.Format("BL Preload output frame with grade = %d", lBlockNo);
				CMSLogFileUtility::Instance()->WL_LogStatus(szGrade);

				stMsg2.InitMessage(sizeof(LONG), &lBlockNo);
				nConvID2 = m_comClient.SendRequest(BIN_LOADER_STN, "ExArmPreloadFrameForNextWafer", stMsg2);
			}
		}

		//Find HOME die on wafer
		m_bWaferAlignComplete = FALSE;
		Sleep(10);
		SaveScanTimeEvent("  WFL: align frame wafer find GT");
		DOUBLE dUsedTime = GetTime();
		if( FindWaferGlobalAngle(bFullyAuto)==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Find wafer global angle fail");
			m_bWaferAlignComplete = TRUE;
			return Err_SearchHomeDieFailed;
		}
		SaveScanTimeEvent("  WFL: align frame wafer find GT done");
		CString szMsg;
		m_lTimeSlot[13] = (LONG)(GetTime()-dUsedTime);	//	13.	find GT used time
		dUsedTime = GetTime();
		SaveScanTimeEvent("  WFL: align frame wafer auto learn adapt");
		if( AutoLearnAdaptWafer()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Find Home-Die adapt wafer fail");
			m_bWaferAlignComplete = TRUE;
			return Err_SearchHomeDieFailed;
		}
		m_lTimeSlot[14] = (LONG)(GetTime()-dUsedTime);	//	14.	auto adapt wafer used time
		dUsedTime = GetTime();
		
		SaveScanTimeEvent("  WFL: align frame wafer to find home die");

		BOOL bFindHomeDie = FindHomeDie();	// fully auto align wafer by auto align button or auto cycle
		m_lTimeSlot[15] = (LONG)(GetTime()-dUsedTime);	//	15.	find home die used time.
		
		//Request wafer table stn to update global angle again
		int	nConvID = 0;
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "SetWaferGlobalAnlge", stMsg);
		while (1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 36000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bReturn);
				break;
			}
			else 
			{
				Sleep(1);		
			}
		}

		if( bIsFullyAuto && IsESMachine() && bNeedSyncFirst==FALSE )
		{
			// Wait for load map to complete
			SaveScanTimeEvent("  WFL: find home and align frame wafer ok; to sync map");
			if (SyncLoadMapFile(nConvMap) != TRUE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("sync load map file error");
				if( IsWT2InUse() )
					HouseKeeping_WT2(WL_ON, FALSE, FALSE, FALSE);
				else
					HouseKeeping(WL_ON, FALSE, FALSE, FALSE);
				return FALSE;
			}
		}

		if( bFindHomeDie==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Find Home-Die fail");
			m_bWaferAlignComplete = TRUE;
			return Err_SearchHomeDieFailed;
		}
		//Perform auto wafer alignment
		SaveScanTimeEvent("  WFL: align frame wafer to align input wafer");
		bReturn = AlignInputWafer();
		m_bWaferAlignComplete = TRUE;

		if (bReturn == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Align input wafer 2 failed\n");
			if( IsWT2InUse() )
				SetErrorMessage("Align wafer 2 failed\n");
			else
				SetErrorMessage("Align wafer failed\n");
			SetAlert_Red_Yellow(IDS_WL_ALIGNWAF_FAIL);
			return FALSE;
		}

		SaveScanTimeEvent("  WFL: align frame wafer align input wafer done");

		//SEmi-Auto mode quits here !
		if ( IsWLSemiAutoMode() && bFullyAuto )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Load finish (Semi-AUTO)\n");
			HmiMessage_Red_Yellow("Semi-Auto mode is stopped; please align wafer manually before continue bonding.");
			return TRUE;
		}

		//Look for binloader pre-load frame result before proceeding
		if (bBLModule && !bPreScan && Is180Arm6InchWT() == FALSE)
		{
			BOOL bResult = TRUE;
			CMSLogFileUtility::Instance()->WL_LogStatus("load bl module result");
			if (lBlockNo != -1)
			{
				while(1)
				{
					if ( m_comClient.ScanReplyForConvID(nConvID2, 360000000) == TRUE )
					{
						m_comClient.ReadReplyForConvID(nConvID2, stMsg2);
						stMsg2.GetMsg(sizeof(BOOL), &bResult);
						break;
					}
					else
					{
						Sleep(1);		
					}
				}
			}
			else
			{
				bResult = TRUE;
			}
			if ( (bResult == FALSE) )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Ex Arm pre-load next grade frame fails\n");
				SetErrorMessage("Ex Arm pre-load next grade frame fails\n");
				HmiMessage_Red_Yellow("WL: Ex-arm pre-load next frame fails!");
				return FALSE;
			}
		}
		CMSLogFileUtility::Instance()->WL_LogStatus("load Align wafer finish");
		SaveScanTimeEvent("  WFL: align frame wafer finish");
	}
	else
	{
		//Request wafer table stn to update global angle
		m_comClient.SendRequest(WAFER_TABLE_STN, "SetWaferGlobalAnlge", stMsg);
	}

	if ( (GetExpType() != WL_EXP_VACUUM && GetExpType() != WL_EXP_CYLINDER) && (IsExpanderLock() == TRUE) )
	{
		if( IsWT2InUse() )
		{
			if (IsExpander2Lock())
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 unlock failed\n");
				SetErrorMessage("Expander2 unlock failed\n");
				SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);
				return Err_ExpanderLockFailed;
			}
		}
		else
		{
			if (IsExpanderLock())	//v3.90
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Expander unlock failed\n");
				SetErrorMessage("Expander unlock failed\n");
				SetAlert_Red_Yellow(IDS_WL_EXP_UNLOCK_FAIL);
				return Err_ExpanderLockFailed;
			}
		}
	}

	if( IsWT2InUse() )
		CMSLogFileUtility::Instance()->WL_LogStatus("Load 2 finish (Align FrameWafer)\n");
	else
		CMSLogFileUtility::Instance()->WL_LogStatus("Load finish (Align FrameWafer)\n");

	m_szLoadAlignBarcodeName.Empty();
	if (nConvMap != 0)
	{
		return SyncLoadMapFile(nConvMap);
	}
	return TRUE;
}

INT CWaferLoader::CheckExpander2(BOOL bMoveT)
{
	SFM_CHipecChannel* pWaferMotorT2 = NULL;
	CMSNmAxisInfo stWTAxis_T2;

	try
	{
	
		InitAxisData(stWTAxis_T2);
		stWTAxis_T2.m_szName				= "WaferT2Axis";
		stWTAxis_T2.m_szTag					= MS896A_CFG_CH_WAFTABLE2_T;
		stWTAxis_T2.m_ucControlID			= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(stWTAxis_T2);

		if (bMoveT)			//v4.16T3
		{
			if ( abs(CMS896AStn::MotionGetEncoderPosition("WaferT2Axis", 1, &stWTAxis_T2)) > 10 )
			{
				CMS896AStn::MotionMoveTo("WaferT2Axis", 0, SFM_WAIT, &stWTAxis_T2);
				Sleep(1000);
			}
		}
	}  
	catch (CAsmException e) 
	{
		DisplayException(e);
	}


	SetExpander2Lock(WL_ON);
	Sleep(1000);

	if (IsExpander2Lock() == FALSE)
	{
		SetExpanderLock(WL_OFF);
		return Err_ExpanderLockFailed;
	}

	if (IsExpander2Open() == TRUE)
	{
		return ExpanderDCMotor2Platform(FALSE, FALSE, FALSE, FALSE, 20000);
	}

	if (IsExpander2Close() == TRUE)
	{
		SetExpander2Lock(WL_OFF);
		return Err_ExpanderAlreadyClose;
	}

	SetExpander2Lock(WL_OFF);
	return Err_ExpanderLockFailed;
}


INT CWaferLoader::ManualCheckExpander2()
{
	SFM_CHipecChannel* pWaferMotorT2 = NULL;
	CMSNmAxisInfo stWTAxis_T2;

	try
	{

		InitAxisData(stWTAxis_T2);
		stWTAxis_T2.m_szName			= "WaferT2Axis";
		stWTAxis_T2.m_szTag				= MS896A_CFG_CH_WAFTABLE2_T;
		stWTAxis_T2.m_ucControlID		= PL_DYNAMIC;		//Use Dynamic as default
		GetAxisData(stWTAxis_T2);

		CMS896AStn::MotionPowerOff("WaferT2Axis", &stWTAxis_T2);

	}  
	catch (CAsmException e) 
	{
		DisplayException(e);
		return Err_ExpanderLockFailed;				// ENGAGE fails!!
	}

	INT nStatus = Err_ExpanderLockFailed;
	INT nCount = 0;
	do 
	{
		AfxMessageBox("Expander2 checking fails!  Please adjust the wafer T motor manually to ENGAGE position!");
		nStatus = CheckExpander2(FALSE);
		nCount++;

		if (nCount > 2)
			break;
	} while (nStatus != Err_ExpanderAlreadyClose);	// continue if still cannot be closed!

	CMS896AStn::MotionPowerOn("WaferT2Axis", &stWTAxis_T2);

	//v2.58
	if (nStatus != Err_ExpanderAlreadyClose) {
		if (AfxMessageBox("Use SAFE mode to by-pass waferloader checking?", MB_YESNO) == IDYES)
			nStatus = Err_ExpanderAlreadyClose;
	}

	return nStatus;
}

VOID CWaferLoader::GetES101WTEncoder(LONG *lXAxis, LONG *lYAxis, LONG *lTAxis, BOOL bWT2)
{
	IPC_CServiceMessage stMsg;

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG lT;
	} ENCVAL;

 	ENCVAL stEnc;

	int nConvID = 0;

	// Get the reply for the encoder value
	if( IsESDualWT() )
	{
		if( bWT2 )
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetWT2EncoderCmd", stMsg);
		else
			nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetWT1EncoderCmd", stMsg);
	}
	else
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "GetEncoderCmd", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(ENCVAL), &stEnc);
	
			*lXAxis = stEnc.lX;
			*lYAxis = stEnc.lY;
			*lTAxis = stEnc.lT;
			break;
		}
		else
		{
			Sleep(10);
		}	
	}
}

LONG CWaferLoader::ES101AutoLoadWaferFrame(BOOL	bBurnIn)
{
	BOOL bFrameInside = FALSE;

	if( m_fHardware && !X2_IsPowerOn() )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Gripper 2 not power ON");
		HmiMessage_Red_Yellow("wafer gripper 2 not power ON!");
		return 0;
	}

	if( MoveES101EjtElvtZToUpDown(FALSE)==FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Ejector Elevator motion error");
		HmiMessage_Red_Yellow("Ejector Elevator motion error!", "ES101");
		return 0;
	}

	if (m_bIsMagazineFull == TRUE && m_bIsMagazine2Full == TRUE)
	{
		SetAlert_Red_Yellow(IDS_WL_MAGS_FULL);	//	Pre-start, auto load wafer frame.
		SetErrorMessage("ES101 AutoLoad WaferFrame - All magazines are full");
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
		{
			DisplaySequence("WL - prestart to clear wafer lot data as magazines full");
			IPC_CServiceMessage stMsg;

			//	auto cycle, prestart, clear wafer lot data
			int	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "ClearWaferLotDataCmd", stMsg);
			while(1)
			{
				if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}	
			}
		}	//	all magazines full done.

		return 0;
	}

	if (m_bIsMagazineFull == TRUE)
	{
		SetWT2InUse(TRUE);
	}
	
	if (m_bIsMagazine2Full == TRUE)
	{
		SetWT2InUse(FALSE);
	}

	SaveData();

	if ( IsWLExpanderOpen() == TRUE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander not closed");
		SetErrorMessage("Expander not closed");

		return 0;
	}

	if ( !IsWT2InUse() )
	{
		bFrameInside = m_bFrameExistOnExp;
	//	if( bFrameInside==FALSE )
	//		bFrameInside = m_bFrameLoadedOnWT1;
	}
	else
	{
		bFrameInside = m_bFrameExistOnExp2;
	//	if( bFrameInside==FALSE )
	//		bFrameInside = m_bFrameLoadedOnWT2;
	}
	
	SetWaferTableJoystick(FALSE);

	m_lBackLightZStatus = 2;	//	4.24TX 4
	MoveES101BackLightZUpDn(FALSE);	// auto load wafer when press start button

	// trigger to preload
	if (bFrameInside == TRUE)
	{
		// 4.24TX1 b
		if( IsWT2InUse() )
		{
			if( m_bFrameExistOnExp==FALSE )
			{
				if (m_bIsMagazineFull == FALSE)
				{
					m_bIsGetCurrentSlot = TRUE;
					m_lFrameToBePreloaded = 3;

					CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine1 FrameToBePreloaded = 3");
				}
				else
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine1 Full FrameToBePreloaded = 0");
					m_lFrameToBePreloaded = 0;
				}
			}
		}
		else
		{
			if(m_bDisableWT2InAutoBondMode==FALSE)
			{
				if( m_bFrameExistOnExp2==FALSE )
				{
					if (m_bIsMagazine2Full == FALSE)
					{
						m_bIsGetCurrentSlot2 = TRUE;
						m_lFrameToBePreloaded = 4;

						CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine2 FrameToBePreloaded = 4");
					}
					else
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine2 Full FrameToBePreloaded = 0");
						m_lFrameToBePreloaded = 0;
					}
				}
			}
		}
		// 4.24TX1 e
		return 1;
	}

	if( WPR_MoveFocusToSafe() == FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL2 load AOI move focus to safe fail");
		return Err_AutoFocus_Z_Fail;
	}

	SaveScanTimeEvent("  WFL: auto WT UNLOAD FOR NEXT FRAME begin");
	ES101MoveTableToUnload(SFM_NOWAIT, FALSE);	//	4.24TX 4
	ES101MoveTableToUnload(SFM_NOWAIT, TRUE);

	SyncWaferTable2XYT("XY");
	SyncWaferTableXYT("XY");
	SaveScanTimeEvent("  WFL: auto WT UNLOAD FOR NEXT FRAME END");

	if ( !IsWT2InUse() )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Load wafer frame WT1");
		if( m_lFrameToBeUnloaded==1 )
			m_lFrameToBeUnloaded = 0;
		
		CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Load wafer frame WT1 Reset Current Slot 1&2");

		//Continue to search filmframe & get current magazine slot
		m_bIsGetCurrentSlot = TRUE;
		// reset the flag for preload
		m_bIsGetCurrentSlot2 = TRUE;

		if (m_bIsMagazineFull == FALSE)
		{
			LONG lLoadFrameStatus = LoadFilmFrame(TRUE, bBurnIn, TRUE, FALSE, FALSE, TRUE);

			if ( lLoadFrameStatus != TRUE )
			{
				if (lLoadFrameStatus == Err_MagazineFull)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine 1 is full");
				}

				SetErrorMessage("ES101 AutoLoad WaferFrame: fail to load frame on WT1");
				return 0;
			}


			SetWT2InUse(FALSE);
			SetStatusMessage("WL Filmframe is loaded to WT1 ");
		}
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Load wafer frame WT2");
		if( m_lFrameToBeUnloaded==2 )
			m_lFrameToBeUnloaded = 0;

		CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Load wafer frame WT2 Reset Current Slot 1&2");

		m_bIsGetCurrentSlot = TRUE;
		m_bIsGetCurrentSlot2 = TRUE;

		LONG lLoadFrameStatus = LoadFilmFrame_WT2(TRUE, bBurnIn, TRUE, FALSE, FALSE, TRUE);

		if ( lLoadFrameStatus != TRUE )
		{
			if (lLoadFrameStatus == Err_MagazineFull)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine 2 is full");
			}

			SetErrorMessage("ES101 AutoLoad WaferFrame: fail to load frame on WT2");
			return 0;
		}

		SetWT2InUse(TRUE);
		SetStatusMessage("WL Filmframe is loaded to WT2 ");
	}

	// preaparing for preload
	// 4.24TX1 b
	if( IsWT2InUse() )
	{
		if( m_bFrameExistOnExp==FALSE )
		{
			if (m_bIsMagazineFull == FALSE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine1  FrameToBePreloaded = 3");
				m_lFrameToBePreloaded = 3;
			}
			else
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine1 Full  FrameToBePreloaded = 0");
				m_lFrameToBePreloaded = 0;
			}
		}
	}
	else
	{
		if(m_bDisableWT2InAutoBondMode==FALSE)
		{
			if( m_bFrameExistOnExp2==FALSE )
			{
				if (m_bIsMagazine2Full == FALSE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine2 FrameToBePreloaded = 4");
					m_lFrameToBePreloaded = 4;
				}
				else
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("ES101 AutoLoad WaferFrame: Magazine2 Full FrameToBePreloaded = 0");
					m_lFrameToBePreloaded = 0;
				}
			}
		}
	}

	if( IsWT2InUse() )
	{
		m_szLoadAlignBarcodeName = m_szBarcodeName2;
	}
	else
	{
		m_szLoadAlignBarcodeName = m_szBarcodeName;
	}
	// 4.24TX1 e
	
	if( AlignFrameWafer(TRUE, TRUE, TRUE)!=TRUE )
	{
		MoveES101BackLightZUpDn(FALSE);	// auto load alignment fail
		return 0;
	}


//	SendGemEvent_WL_Load(TRUE);

	if (IsWLSemiAutoMode())		//New SEMI-AUTO mode
	{
		MoveES101BackLightZUpDn(FALSE);	// auto load semi auto mode
		return 0;	//Force to abort current loading
	}


	return 1;
}

LONG CWaferLoader::ES101AutoChangeFilmFrame(BOOL bBurnIn)
{
	CString szLogMsg;
	SaveScanTimeEvent("  WFL: ES aut change file frame begin");
	m_bFrameOperationInAutoMode = TRUE;	//	4.24TX 4
	SetWaferTableJoystick(FALSE);

	MoveES101BackLightZUpDn(FALSE);	// change frame

	// Move the table to unload position
	if( IsWT2InUse() )
	{
		ES101MoveTableToUnload(SFM_NOWAIT, TRUE);	//	4.24TX 4
		szLogMsg = "  WFL: ES101 AutoChange FilmFrame: WT2 to unload";
		if (m_bFrameExistOnExp2)
		{
			m_lFrameToBeUnloaded = 2;	//2=WT2, 1=WT1, 0=none		//v4.24T11
		}
		else
		{
			m_lFrameToBeUnloaded = 0;	//2=WT2, 1=WT1, 0=none		//v4.24T11
		}
	}
	else
	{
		ES101MoveTableToUnload(SFM_NOWAIT, FALSE);
		szLogMsg = "  WFL: ES101 AutoChange FilmFrame: WT1 to unload";
		if (m_bFrameExistOnExp)
		{
			m_lFrameToBeUnloaded = 1;	//2=WT2, 1=WT1, 0=none		//v4.24T11
		}
		else
		{
			m_lFrameToBeUnloaded = 0;	//2=WT2, 1=WT1, 0=none		//v4.24T11
		}
	}
	SaveScanTimeEvent(szLogMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

	// select the wafer loader need to change
	if( IsWT2InUse() )
	{
		if (m_bIsMagazineFull == TRUE)
		{
			// complete unload motion if use the same table
			SyncWaferTable2XYT("XY");
			SaveScanTimeEvent("  WFL: auto change file frame to load WT2 as Magazine1 complete");

			if (UnloadFilmFrame_WT2(FALSE, bBurnIn) != TRUE)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Unload Frame Fail");
				return 0;
			}
			m_lFrameToBeUnloaded = 0;
			
			CMSLogFileUtility::Instance()->WL_LogStatus("Magazine1 complete, load frame to from Magazine2");
			if( LoadFilmFrame_WT2(FALSE, bBurnIn, TRUE, FALSE) != TRUE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("load Read Barcode On Gripper WT2 Sequence fail");
				return 0;
			}
			
			SaveScanTimeEvent("  WFL: auto change file frame load WT2 done");
			SetWT2InUse(TRUE);	//WT2
			m_lFrameToBeUnloaded = 0;
			szLogMsg = "WL Filmframe WT2 is loaded";
		}
		else
		{
			if (!m_bIsFramebePreloaded)		//v4.24T11	//No need to load frame if it is already preloaded on table
			{
				SaveScanTimeEvent("  WFL: auto change file frame to load WT1 as not preloaded");
				CMSLogFileUtility::Instance()->WL_LogStatus("ES101AutoChangeFileFrame Preload WT1 fail, now to load it again");
				if ( LoadFilmFrame(TRUE, bBurnIn, TRUE, FALSE) != TRUE )
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("load Read Barcode On Gripper Sequence fail");
					return 0;
				}
				SaveScanTimeEvent("  WFL: auto change file frame load WT1 done");
			}

			SetWT2InUse(FALSE);		//WT1
			szLogMsg = "WL Filmframe WT1 is loaded";
		}
	}
	else
	{
		if (m_bDisableWT2InAutoBondMode)	//v4.24T11
		{
			SyncWaferTableXYT("XY");
			CMSLogFileUtility::Instance()->WL_LogStatus("WL DISABLE WT2 change WT1 frame");

			BOOL bStartFromHome2 = TRUE;
			if( m_bFrameExistOnExp )
			{
				szLogMsg = "  WFL: auto change file frame to unload WT1 as not preloaded WT2=X";
				SaveScanTimeEvent(szLogMsg);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				bStartFromHome2 = FALSE;
				if( UnloadFilmFrame(bStartFromHome2, bBurnIn) != TRUE )
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("WL unload WT1 frame fail");
					return 0;
				}
			}

			SaveScanTimeEvent("  WFL: auto change file frame to load WT1 as not preloaded WT2=X");
			SetErrorMessage("Start Read Barcode On Gripper Sequence ");
			
			if (IsBurnIn())
			{
				ES101MoveTableToUnload(SFM_WAIT, FALSE);
			}

			CMSLogFileUtility::Instance()->WL_LogStatus("WL disable WT2 load wafer frame");
			if ( LoadFilmFrame(bStartFromHome2, bBurnIn, TRUE, FALSE) != TRUE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("load Read Barcode On Gripper Sequence fail");
				return 0;
			}
			SaveScanTimeEvent("  WFL: auto change file frame load WT1 done");

			SetWT2InUse(FALSE);	//WT1
			m_lFrameToBeUnloaded = 0;	//2=WT2, 1=WT1, 0=none
			szLogMsg = "WL Filmframe is loaded";
		}
		else
		{
			if (m_bIsMagazine2Full == TRUE)
			{
				// complete unload motion if use the same table
				SyncWaferTableXYT("XY");

				SaveScanTimeEvent("  WFL: auto change file frame to load WT1 as not preloaded WT2=X");

				if (UnloadFilmFrame(FALSE, bBurnIn) != TRUE)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Unload Frame Fail");
					return 0;
				}
				m_lFrameToBeUnloaded = 0;

				SetErrorMessage("Start Read Barcode On Gripper Sequence ");
				if ( LoadFilmFrame(FALSE, bBurnIn, TRUE, FALSE) != TRUE )
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("load Read Barcode On Gripper Sequence fail");
					return 0;
				}

				SaveScanTimeEvent("  WFL: auto change file frame load WT1 done");

				SetWT2InUse(FALSE);	//WT1
				m_lFrameToBeUnloaded = 0;	//2=WT2, 1=WT1, 0=none
				szLogMsg = "WL Filmframe is loaded";
			}
			else
			{
				if (!m_bIsFramebePreloaded)		//v4.24T11	//No need to load frame if it is already preloaded on table
				{
					SaveScanTimeEvent("  WFL: auto change file frame to load WT2 as not preloaded");
					SetErrorMessage("Start Read Barcode On Gripper Sequence WT2");
					
					if (IsBurnIn())
					{
						ES101MoveTableToUnload(SFM_WAIT, TRUE);
					}

					if( LoadFilmFrame_WT2(TRUE, bBurnIn, TRUE, FALSE) != TRUE )
					{
						CMSLogFileUtility::Instance()->WL_LogStatus("load Read Barcode On Gripper WT2 Sequence fail");
						return 0;
					}
					SaveScanTimeEvent("  WFL: auto change file frame load WT2 done");
				}

				SetWT2InUse(TRUE);	//WT2
				szLogMsg = "WL Filmframe WT2 is loaded";
			}
		}

		m_bIsFramebePreloaded = FALSE;		//v4.24T11
	}
	SaveScanTimeEvent("  WFL: " + szLogMsg);

	SetStatusMessage(szLogMsg);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

	//Align wafer & find HOME dices
	if( IsWT2InUse() )
	{
		SyncWaferTable2XYT("XY");	//	4.24TX 4
		if( m_lWT2PositionStatus==1 )
			m_lWT2PositionStatus = 2;
		m_szLoadAlignBarcodeName = m_szBarcodeName2;
	}
	else
	{
		SyncWaferTableXYT("XY");	//	4.24TX 4
		if( m_lWT1PositionStatus==1 )
			m_lWT1PositionStatus = 2;
		m_szLoadAlignBarcodeName = m_szBarcodeName;
	}
	SaveScanTimeEvent("  WFL: auto change WT under camera; after sync table, to align frame wafer");
	if( AlignFrameWafer(TRUE, TRUE, TRUE)!=TRUE )
	{
		MoveES101BackLightZUpDn(FALSE);	// change frame, align fail
		return 0;
	}

//	SendGemEvent_WL_Load(TRUE);

	if ( IsWLSemiAutoMode() )
	{
		MoveES101BackLightZUpDn(FALSE);	// change frame semi auto
		SaveScanTimeEvent("  WFL: align frame wafer done, into prestart to semi-auto cycle\n\n");
		return 0;	//Force to abort current LOAD wafer operation
	}


	SaveScanTimeEvent("  WFL: align frame wafer done, into prestart to auto cycle\n\n");

	return 1;
}

BOOL CWaferLoader::MoveES101EjtElvtZToUpDown(BOOL bToUp)
{
	BOOL bReturn = TRUE;

	if( IsEjtElvtInUse() )	// 4.24TX2
	{
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bToUp);
		INT nConvID = m_comClient.SendRequest(BOND_HEAD_STN, "MoveEjtElvtToUpDown", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		stMsg.GetMsg(sizeof(BOOL), &bReturn);
	}

	return bReturn;
}

BOOL CWaferLoader::MoveFocusToWafer(BOOL bWft)
{
	if( IsWprWithAF() )
	{
		SaveScanTimeEvent("  WFL: down focus to wafer");
		BOOL bMove = FALSE;
		LONG lCounter = 0;
		if( bWft )
		{
			switch( m_lFocusZState )
			{
			case 22:
				break;
			case 11:
				while( 1 )
				{
					Sleep(10);
					if( m_lFocusZState!=11 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
				break;
			case 0:
			default:
				bMove = TRUE;
				break;
			}
		}
		else
		{
			switch( m_lFocusZState )
			{
			case 2:
				break;
			case 1:
				while( 1 )
				{
					Sleep(10);
					if( m_lFocusZState!=1 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
				break;
			case 0:
			default:
				bMove = TRUE;
				break;
			}
		}

		if( bMove )
		{
			INT nConvID;
			IPC_CServiceMessage stMsg;
			stMsg.InitMessage(sizeof(BOOL), &bWft);
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "GotoTableFocus", stMsg);
			while (1)
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			nConvID = m_comClient.SendRequest(WAFER_PR_STN, "FocusZSync", stMsg);
			while (1)	//	4.24TX 4
			{
				if( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
				{
					m_comClient.ReadReplyForConvID(nConvID, stMsg);
					break;
				}
				else
				{
					Sleep(10);
				}
			}
			if( bWft )
				m_lFocusZState = 22;
			else
				m_lFocusZState = 2;
		}
		SaveScanTimeEvent("  WFL: down focus to wafer done");
	}

	return TRUE;
}

BOOL CWaferLoader::MoveES101BackLightZUpDn(BOOL bUp)
{
	BOOL bReturn = TRUE;
	BOOL bMove = FALSE;

	if( IsBLInUse() )	//	4.24TX 4
	{
		if( bUp )
		{
			if( m_lBackLightZStatus==1 )
			{
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==2 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==3 )
			{
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==0 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==0 )
			{
				bMove = TRUE;
			}
		}
		else
		{
			if( m_lBackLightZStatus==1 )
			{
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==2 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==3 )
			{
				LONG lCounter = 0;
				while( 1 )
				{
					Sleep(10);
					if( m_lBackLightZStatus==0 )
						break;
					lCounter++;
					if( lCounter>2000 )
						break;
				}
			}
			if( m_lBackLightZStatus==2 )
			{
				bMove = TRUE;
			}
		}
		if( IsBurnIn() )
			bMove = TRUE;
	}

	if( bMove )	// 4.24TX
	{
	//	if( bUp )
	//	//	SaveScanTimeEvent("  WFL: BLZ to UP by IPC");
	//	else
	//	//	SaveScanTimeEvent("  WFL: BLZ to DN by IPC");
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(BOOL), &bUp);
		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MoveBackLightToUpStandby", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if( m_lBackLightTravelTime>0 )
		{
			Sleep(m_lBackLightTravelTime);	// between up and standby
		}
		if( bUp )
			m_lBackLightZStatus = 2;	//	4.24TX 4
		else
			m_lBackLightZStatus = 0;	//	4.24TX 4

		stMsg.GetMsg(sizeof(BOOL), &bReturn);
		if( bReturn==FALSE )
		{
			SetMotionCE(TRUE, "MoveBackLightToUpStandby Timeout (MoveES101BackLightZUpDn)");
		}
	//	SaveScanTimeEvent("  WFL: BLZ move by IPC complete");
	}

	return bReturn;
}

BOOL CWaferLoader::MoveBackLightToSafeLevel()
{
	BOOL bReturn = TRUE;

	if( IsBLInUse() )	// 4.24TX
	{
		IPC_CServiceMessage stMsg;

		INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MoveBackLightToSafeLevel", stMsg);
		while(1)
		{
			if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE ) 
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				break;
			}
			else
			{
				Sleep(10);
			}
		}
		stMsg.GetMsg(sizeof(BOOL), &bReturn);
	}

	if (bReturn == FALSE)
	{
		SetAlert_Red_Yellow(IDS_WPR_BL_NOT_IN_SAFE_POS);
	}

	return bReturn;
}

BOOL CWaferLoader::MoveBackLightToUpLevel()
{
	BOOL bUp = TRUE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bUp);
	INT nConvID = m_comClient.SendRequest(WAFER_PR_STN, "MoveBackLightToUpStandbyRoutine", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 500000) == TRUE ) 
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	return TRUE;
}

LONG CWaferLoader::AutoAlignFrameWafer(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	//	426TX	2
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (!pUtl->GetPrescanDummyMap())	// create dummy map for pick and place
	{
		if( IsWaferMapValid()==FALSE )
		{
			CString szTitle = "Auto Align Wafer";
			CString szContent;
			szContent.LoadString(HMB_WT_MAP_FILE_EMPTY);
			HmiMessage(szContent, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 0, glHMI_MSG_MODAL, NULL, 400, 300);
			svMsg.InitMessage(sizeof(BOOL),	&bReturn);
			return 1;
		}
	}

	if( IsWL1ExpanderSafeToMove()==FALSE || IsWL2ExpanderSafeToMove()==FALSE )
	{
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	CMSLogFileUtility::Instance()->WL_LogStatus("Hmi Search home Die");
	SetWaferTableJoystick(FALSE);
	if( WPR_MoveFocusToSafe()==FALSE )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WL load AOI move focus to safe fail");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	if( IsESMachine() )
	{
		BOOL bMoveAway = TRUE;
		if( IsWT2InUse() && IsWT2UnderCamera() )
		{
			bMoveAway = FALSE;
		}
		if( !IsWT2InUse() && IsWT1UnderCamera() )
		{
			bMoveAway = FALSE;
		}
		if( bMoveAway )
		{
			MoveBackLightToSafeLevel();	// auto align
			MoveES101EjtElvtZToUpDown(FALSE);
		}
		if( IsWT2InUse() )
			ES101MoveTableToUnload(SFM_WAIT, FALSE);
		else
			ES101MoveTableToUnload(SFM_WAIT, TRUE);	//	4.24TX 4
	}
	if( AlignFrameWafer(TRUE, TRUE, TRUE, FALSE)==TRUE )
		bReturn = TRUE;	// offline, press auto align button, no need to load map file again
	MoveBackLightToSafeLevel();	// auto align frame fail
	SetWaferTableJoystick(TRUE);

	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}	//	4.24TX 4


BOOL CWaferLoader::ES101MoveTableToUnload(INT nMode, BOOL bWT2)
{
	LONG lCurrentX = 0;
	LONG lCurrentY = 0;
	if( bWT2 )
	{
		if( IsES201() )
		{
			m_lWT2PositionStatus = 0;
			return TRUE;
		}
		if( nMode==SFM_WAIT )
			m_lWT2PositionStatus = 0;
		else
			m_lWT2PositionStatus = 3;
		lCurrentX = m_lUnloadPhyPosX2;
		lCurrentY = m_lUnloadPhyPosY2;
	}
	else
	{
		if( (IsES101()) && m_bFrameOperationInAutoMode && IsBurnIn() == FALSE)
		{
			if( m_lWT1PositionStatus==0 || m_lWT1PositionStatus==3 )
				return TRUE;
		}
		if( nMode==SFM_WAIT )
			m_lWT1PositionStatus = 0;
		else
			m_lWT1PositionStatus = 3;
		lCurrentX = m_lUnloadPhyPosX;
		lCurrentY = m_lUnloadPhyPosY;
	}

	WFTTGoHomeUnderCam(bWT2);

	return ES101MoveWaferTable(lCurrentX, lCurrentY, nMode, bWT2);
}	//	4.24TX 4

BOOL CWaferLoader::ES101MoveTableToHomeDie(INT nMode, BOOL bWT2)
{
	LONG	lCurrentX = 0;
	LONG	lCurrentY = 0;

	CString szMsg;
	if( bWT2 )
	{
		if( IsES201() )
		{
			m_lWT2PositionStatus = 2;
			return TRUE;
		}
		if( nMode==SFM_WAIT )
			m_lWT2PositionStatus = 2;
		else
			m_lWT2PositionStatus = 1;
		if( IsWL2ExpanderSafeToMove()==FALSE )
		{
			return FALSE;
		}
		GetWT2HomeDiePhyPosn(lCurrentX, lCurrentY);
		szMsg.Format("WT2 home die posn %d,%d", lCurrentX, lCurrentY);
		if( m_bUseContour )
		{
			lCurrentX = m_lWL2CenterX;
			lCurrentY = m_lWL2CenterY;
			szMsg.Format("WT2 contoure center die posn %d,%d", lCurrentX, lCurrentY);
		}
	}
	else
	{
		if( (IsES101()) && m_bFrameOperationInAutoMode )
		{
			if( m_lWT1PositionStatus==1 || m_lWT1PositionStatus==2 )
				return TRUE;
		}
		if( nMode==SFM_WAIT )
			m_lWT1PositionStatus = 2;
		else
			m_lWT1PositionStatus = 1;
		if( IsWL1ExpanderSafeToMove()==FALSE )
		{
			return FALSE;
		}
		GetHomeDiePhyPosn(lCurrentX, lCurrentY);
		szMsg.Format("WT1 home die posn %d,%d", lCurrentX, lCurrentY);
		if( m_bUseContour )
		{
			lCurrentX = m_lWL1CenterX;
			lCurrentY = m_lWL1CenterY;
			szMsg.Format("WT1 contoure center die posn %d,%d", lCurrentX, lCurrentY);
		}
	}

	SaveScanTimeEvent(szMsg);
	return ES101MoveWaferTable(lCurrentX, lCurrentY, nMode, bWT2);
}	//	4.24TX 4

BOOL CWaferLoader::ES101MoveWaferTable(LONG lXAxis, LONG lYAxis, INT nMode, BOOL bWT2)
{
	if (IsES201() && bWT2)
	{
		return TRUE;
	}

	typedef struct
	{
		LONG lX;
		LONG lY;
		LONG nMode;
	} WT_XY_HOME_LOAD_STRUCT;
 	WT_XY_HOME_LOAD_STRUCT stEsPos;

	stEsPos.lX	= lXAxis;
	stEsPos.lY	= lYAxis;
	stEsPos.nMode = nMode;

	BOOL bResult = TRUE;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	stMsg.InitMessage(sizeof(WT_XY_HOME_LOAD_STRUCT), &stEsPos);
	if (bWT2)
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY2_MoveToHomeLoadCmd",	stMsg);
	else
		nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XY1_MoveToHomeLoadCmd",	stMsg);

	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(10);
		}	
	}

	if (nMode == SFM_WAIT)
	{
		if( bWT2 )
			SyncWaferTable2XYT("XY");
		else
			SyncWaferTableXYT("XY");
	}

	return bResult;
}

BOOL CWaferLoader::IsWL1ExpanderSafeToMove()
{
	BOOL bReturn = TRUE;

	if( (GetExpType() != WL_EXP_VACUUM) && m_bExpanderStatus )
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("Expander is opened!!");
		SetErrorMessage("Expander is opened!!");

		SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
		bReturn = FALSE;
	}

	if( IsESMachine() && bReturn )
	{
		// check the gripper power is on or not, if on, check at home or not, if not home it (move to home).
		if( X_IsPowerOn()==FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper not power ON");
			HmiMessage_Red_Yellow("wafer gripper not power ON!");
			bReturn = FALSE;
		}
		else
		{
			X_MoveTo(0);
		}
	}

	return bReturn;
}

BOOL CWaferLoader::IsWL2ExpanderSafeToMove()
{
	BOOL bReturn = TRUE;

	if( IsESDualWL() )
	{
		if ( (GetExpType() != WL_EXP_VACUUM) && m_bExpander2Status )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Expander2 is opened!!");
			SetErrorMessage("Expander2 is opened!!");

			SetAlert_Red_Yellow(IDS_WL_EXP_ALREADY_OPEN);
			bReturn = FALSE;
		}

		// check the gripper2 power is on or not, if on, check at home or not, if not home it (move to home).
		if( bReturn )
		{
			if( X2_IsPowerOn()==FALSE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 not power ON");
				HmiMessage_Red_Yellow("wafer gripper2 not power ON!");
				bReturn = FALSE;
			}
			else
			{
				X2_MoveTo(0);
			}
		}	//	check gripper x2 power and position
	}

	return bReturn;
}

LONG CWaferLoader::ES101LoadOfflineMapFile(CString szMapFileName)
{
	if( IsAOIOnlyMachine() && IsBurnIn() )
	{
		szMapFileName = "AOIBurnIn";
	}
	if( szMapFileName.IsEmpty() )
	{
		return TRUE;
	}

	SaveScanTimeEvent("WL: ES101 offline load map file");
	ClearMapFile();

	CMSLogFileUtility::Instance()->WL_LogStatus("WL2 ES101 LoadOfflineMapFile start");
	LoadMapFileWithoutSyn(szMapFileName);

	return TRUE;
}

//	for ES101 and ES201 to rotate theta under camera in case crash with loader gripper
LONG CWaferLoader::RotateWFTTUnderCam(CONST BOOL bWT2)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nRotateUnderCam = pApp->GetProfileInt(gszPROFILE_SETTING, gszES_ROTATE_T_UNDER_CAM, 0);
	if( IsESMachine() && (nRotateUnderCam==1) )
	{
		SaveScanTimeEvent("  WFL: rotate WFT under camera");
		if( bWT2 )
		{
			AutoRotateWafer2(FALSE);
		}
		else
		{
			AutoRotateWafer(FALSE);
		}
	}

	return TRUE;
}

//	for ES101 and ES201 to rotate theta under camera in case crash with loader gripper
LONG CWaferLoader::WFTTGoHomeUnderCam(CONST BOOL bWT2)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	INT nRotateUnderCam = pApp->GetProfileInt(gszPROFILE_SETTING, gszES_ROTATE_T_UNDER_CAM, 0);
	if( IsESMachine() && (nRotateUnderCam==1) )
	{
		MoveWaferThetaTo(0, bWT2);
	}

	return TRUE;
}

LONG CWaferLoader::GetWL1MaxMgznNo()
{
	LONG lMaxMgznNo = 1;

	if( m_bUseBLAsLoaderZ )
	{
		lMaxMgznNo = WL_MAG_NO;
	}
	else if( IsESMachine() )
	{
		lMaxMgznNo = WL_MAG_NO-1;
		if( m_bUseMagazine3AsX )	// Move To slot, check range
		{
			lMaxMgznNo--;
		}
		if( m_bUseAMagazineAsG )
		{
			lMaxMgznNo--;
		}
	}

	return lMaxMgznNo;
}

LONG CWaferLoader::GetWL2MaxMgznNo()	// ES101 only.
{
	LONG lMaxMgznNo = WL_MAG_NO - 1;
	if( m_bUseMagazine3AsX )		// Move to slot2, check range
	{
		lMaxMgznNo--;
	}
	if( m_bUseAMagazineAsG )
	{
		lMaxMgznNo--;
	}

	return lMaxMgznNo;
}
