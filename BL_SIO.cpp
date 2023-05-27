/////////////////////////////////////////////////////////////////
// BL_Common.cpp : Common functions of the CBinLoader class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Thursday, November 25, 2004
//	Revision:	1.00
//
//	By:			BarryChu
//				
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "MS_SecCommConstant.h"
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// HAREWARE RELATIVE FUNCIONS----IN BIT
BOOL CBinLoader::IsFrameLevel()
{
	if (m_bDisableBL)		//v3.60
	{
		return TRUE;
	}

	if (m_bNoSensorCheck)
	{
		return TRUE;
	}

	//m_bFrameLevel = m_piFrameLevel->IsHigh();
	m_bFrameLevel = CMS896AStn::MotionReadInputBit(BL_SI_FrameLevel);
	return m_bFrameLevel;
}

BOOL CBinLoader::IsFrameLevel2(VOID)
{
	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_bNoSensorCheck)
	{
		return TRUE;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		m_bFrameLevel = CMS896AStn::MotionReadInputBit(BL_SI_FrameLevel);
	}
	else
	{
		m_bFrameLevel = CMS896AStn::MotionReadInputBit(BL_SI_FrameLevel2);
	}
	return m_bFrameLevel;
}

BOOL CBinLoader::IsFrameOnBTPosition()
{
	if (m_bDisableBL)
	{
		return TRUE;
	}
/*
	if (m_bNoSensorCheck)
	{
		//return TRUE;
		return FALSE;		//v4.22T5	//Assume HAS frame on table
	}
*/
	//m_bFrameOnBTPosition = m_piFrameOnBTPosition->IsHigh();
	m_bFrameOnBTPosition = !CMS896AStn::MotionReadInputBit(BL_SI_FramePos);
	return m_bFrameOnBTPosition;
}

BOOL CBinLoader::IsFrameOnBT2Position(VOID)
{
	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_bNoSensorCheck)
	{
		return TRUE;
	}

	//m_bFrameOnBTPosition = m_piFrameOnBTPosition->IsHigh();
	m_bFrameOnBTPosition2 = CMS896AStn::MotionReadInputBit(BL_SI_FramePos2);
	return m_bFrameOnBTPosition2;
}

//Not use in MS50 2018.1.12
BOOL CBinLoader::IsFrameProtection(VOID)
{
	if (m_bDisableBL)		//v3.60
	{
		return TRUE;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return TRUE;
	}

	if (m_bNoSensorCheck == TRUE)
	{
		return TRUE;
	}

	if (IsMS90())			//v4.49A10
	{
		return TRUE;	
	}
	//m_bFrameProtection = m_piFrameProtection->IsHigh();
	m_bFrameProtection = CMS896AStn::MotionReadInputBit(BL_SI_FrameProtect);
	return m_bFrameProtection;
}

//Not use in MS50 2018.1.12
BOOL CBinLoader::IsFrameProtection2(VOID)
{
	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_bNoSensorCheck == TRUE)
	{
		return TRUE;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return TRUE;
	}

	m_bFrameProtection2 = CMS896AStn::MotionReadInputBit(BL_SI_FrameProtect2);
	return m_bFrameProtection2;
}

BOOL CBinLoader::IsFrameInMagazine(VOID)
{
	if (m_bDisableBL)		//v3.60
		return TRUE;
	if ( m_bNoSensorCheck == TRUE )
	{
		return TRUE;
	}

	//m_bFrameInMagazine = m_piFrameInMagazine->IsHigh();
	m_bFrameInMagazine = CMS896AStn::MotionReadInputBit(BL_SI_FrameInMag);
	return m_bFrameInMagazine;
}


BOOL CBinLoader::IsFrameInClamp(const CString szBL_SI_FrameDetect)
{
	if (m_bDisableBL)		//v3.60
	{
		return TRUE;
	}

	if (m_bNoSensorCheck == TRUE)
	{
		return TRUE;
	}

	//m_bFrameInClampDetect	= m_piFrameInClampDetect->IsHigh();
	return CMS896AStn::MotionReadInputBit(szBL_SI_FrameDetect);
}

BOOL CBinLoader::IsFrameInClamp(VOID)
{
	m_bFrameInClampDetect = IsFrameInClamp(BL_SI_FrameDetect);
	return m_bFrameInClampDetect;
}

BOOL CBinLoader::IsFrameInClamp2(VOID)
{
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return TRUE;
	}

	m_bFrameInClampDetect2 = IsFrameInClamp(BL_SI_FrameDetect2);
	return m_bFrameInClampDetect2;
}


BOOL CBinLoader::IsFrameJam(const CString szBL_SI_FrameJam)
{
	if (m_bDisableBL)		//v3.60
	{
		return FALSE;
	}

	if (m_bNoSensorCheck)
	{
		return FALSE;
	}

	return CMS896AStn::MotionReadInputBit(szBL_SI_FrameJam);
}

BOOL CBinLoader::IsFrameJam(VOID)
{
	m_bFrameJam = IsFrameJam(BL_SI_FRAMEJAM1);
	return m_bFrameJam;
}

BOOL CBinLoader::IsFrameJam2(VOID)
{
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)	//v4.59A40	//MS50
	{
		return FALSE;
	}
	
	m_bFrameJam2 = IsFrameJam(BL_SI_FRAMEJAM2);
	return m_bFrameJam2;
}

/*
BOOL CBinLoader::IsFrameOnBTDetect(VOID)
{
	m_bFrameOnBTDetect = m_piFrameOnBTDetect->IsHigh();

	if ( m_bNoSensorCheck == TRUE )
	{
		return TRUE;
	}

	return m_bFrameOnBTDetect;
}
*/


BOOL CBinLoader::IsBufferSafetySensor()
{
	m_bBufferSafetySensor_HMI = CMS896AStn::MotionReadInputBit(BL_SI_BufferSafetySensor);
	return m_bBufferSafetySensor_HMI;
}


BOOL CBinLoader::IsElevatorCoverOpen()
{
	if (m_bCheckElevatorCover)
	{
		if (IsElevatorDoorCoverOpen() == TRUE)
		{
			return TRUE;
		}

		if (IsFrontRightElevatorDoorCoverOpen() == TRUE)
		{
			return TRUE;
		}
	}
	  
	return FALSE;
}

BOOL CBinLoader::IsElevatorDoorCoverOpen()
{
	if (m_bDisableBL)
	{
		return FALSE;
	}

	//**Should be put before sensor checking to avoid crash by sensor problem
	//if ( (m_bNoSensorCheck == TRUE) || (m_bCheckElevatorCover == FALSE) )
	//{
	//	return FALSE;
	//}

	//v4.51A14	//Cree HZ
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Cree")
	{
		if (m_bCheckElevatorCover == FALSE)
		{
			return FALSE;
		}
	}
	
	//SI-Bit Light = ON  -> sw bit = OFF -> menu light = ON			//Door CLOSE	-> return FALSE
	//SI-Bit Light = OFF -> sw bit = ON  -> menu light = OFF		//Door OPEN		-> return TRUE

	if (CMS896AStn::m_bCEMark)		//v4.06
	{
		BOOL bBit1 = CMS896AStn::MotionReadInputBit(BL_SI_ElevDoorCover);
		BOOL bBit2 = CMS896AStn::MotionReadInputBit(BL_SI_ElevDoorCover2);

		if ((bBit1 == TRUE) && (bBit2 == TRUE))
		{
			m_bElevatorDoorCover_HMI = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
		}
		else if ((bBit1 == FALSE) && (bBit2 == FALSE))
		{
			m_bElevatorDoorCover_HMI = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
		}
		else
		{
			m_bElevatorDoorCover_HMI = bBit1 && !bBit2;
		}
	}
	else
	{
		m_bElevatorDoorCover_HMI = CMS896AStn::MotionReadInputBit(BL_SI_ElevDoorCover);
	}

    return m_bElevatorDoorCover_HMI;
}

BOOL CBinLoader::IsFrontRightElevatorDoorCoverOpen()
{
	if (m_bDisableBL)
	{
		return FALSE;
	}

	//v4.51A14	//Cree HZ
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Cree")
	{
		if (m_bCheckElevatorCover == FALSE)
		{
			return FALSE;
		}
	}


	//SI-Bit Light = ON  -> sw bit = OFF -> menu light = ON			//Door CLOSE	-> return FALSE
	//SI-Bit Light = OFF -> sw bit = ON  -> menu light = OFF		//Door OPEN		-> return TRUE

	if (CMS896AStn::m_bCEMark)		//v4.06
	{
		BOOL bBit1 = CMS896AStn::MotionReadInputBit(BL_SI_FrontRightElevDoorCover);
		BOOL bBit2 = CMS896AStn::MotionReadInputBit(BL_SI_FrontRightElevDoorCover2);

		if ((bBit1 == TRUE) && (bBit2 == TRUE))
		{
			m_bFrontRightElevatorDoorCover_HMI = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
		}
		else if ((bBit1 == FALSE) && (bBit2 == FALSE))
		{
			m_bFrontRightElevatorDoorCover_HMI = TRUE;	//Assume OPEN if TURE-FALSE reverse state is wrong
		}
		else
		{
			m_bFrontRightElevatorDoorCover_HMI = bBit1 && !bBit2;
		}
	}
	else
	{
		m_bFrontRightElevatorDoorCover_HMI = CMS896AStn::MotionReadInputBit(BL_SI_FrontRightElevDoorCover);
	}

    return m_bFrontRightElevatorDoorCover_HMI;
}


//======================================================================================================
//   Main Cover Sensor indluding :
//	Front Cover Sensor, Front Left Cover Sensor, Front Middle Cover Sensor
//	Rear Cover Sensor,  Rear Right Cover Sensor, Left Cover Sensor
//======================================================================================================
BOOL CBinLoader::IsCoverOpen()
{
	CBondHead *pBondHead = dynamic_cast<CBondHead*>(GetStation(BOND_HEAD_STN));
	if (pBondHead != NULL)
	{
		return pBondHead->IsCoverOpen();
	}

	return FALSE;
}
//=========================================================================================================

BOOL CBinLoader::IsFrameOutOfMgz()
{
	if (!m_fHardware)
	{
		return FALSE;
	}

	if (m_bDisableBL)		//v3.60
	{
		return FALSE;
	}

	//**Should be put before sensor checking to avoid crash by sensor problem
	if (m_bNoSensorCheck == TRUE)
	{
		return FALSE;
	}

	//m_bFrameOutOfMgz = m_piFrameOutOfMgz->IsLow();
	m_bFrameOutOfMgz = CMS896AStn::MotionReadInputBit(BL_SI_FrameOutMag) ||	CMS896AStn::MotionReadInputBit(BL_SI_FrameOutMag2);
	return m_bFrameOutOfMgz;
}


BOOL CBinLoader::IsMagazineExist(LONG lMagazine, BOOL bUseBT2, BOOL bNeedByPassDisableAlarm)
{
    CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	BOOL bDisplayAlarm = bNeedByPassDisableAlarm;
	
	if (lMagazine >= MS_BL_MGZN_NUM || lMagazine < 0)
	{
        return FALSE;
	}

	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_bNoSensorCheck == TRUE)
	{
		return TRUE;
	} 

	BOOL bRTMgznExchange = pApp->GetFeatureStatus(MS896A_FUNC_SENSOR_BL_RT_MGZN_EXCHANGE);	//v4.41T1

	if ((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) || CMS896AStn::m_bUseBinMultiMgznSnrs)
	{
		//For 6-sensor config
		//m_bMagazineExist[lMagazine] = !m_piMgzExist[lMagazine]->IsHigh();
		switch (lMagazine)
		{
			case 0:
				if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())
				{
					m_bMagazineExist[lMagazine] = TRUE;
				}
				else
				{
					m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist1);
				}
				break;
			case 1:
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist2);
				break;
			case 2:
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist3);
				break;
			case 3:
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist4);
				break;
			case 4:
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist5);
				break;
			case 5:
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist6);
				break;
			case 6:		//v4.03		//MS100 8mag sensor support
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist7);
				break;
			case 7:		//v4.03		//MS100 8mag sensor support
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist8);
				break;
			default:
				break;
		}
	}
	else
	{
		//For only one sensor.
		if (bUseBT2)
		{
			m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_Mag2Exist);
		}
		else
		{
			if (pApp->GetCustomerName()==CTM_WOLFSPEED)
			{
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist);
			}
			else
			{
				m_bMagazineExist[lMagazine] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist);
			}
		}
	}

	if (!m_bMagazineExist[lMagazine])
	{
		CString strTmp;
		CString szMagName;

        switch( lMagazine )
        {
			case 0:
				szMagName = "Top 1 "; 
				break;
			case 1:
				szMagName = "Middle 1 "; 
				break;
			case 2:
				szMagName = "Bottom 1 "; 
				break;
			case 3:
				szMagName = "Top 2 "; 
				break;
			case 4:
				szMagName = "Middle 2 "; 
				break;
			case 5:
				szMagName = "Bottom 2 "; 
				break;
			case 6:
				szMagName = "8-Bottom 1 ";
				break;
			case 7:
				szMagName = "8-Bottom 2 ";
				break;
			default: 
				szMagName = "Invalid "; 
				break;
		}

		strTmp.Format(" magazine [%d] does not exist!", lMagazine);
		SetErrorMessage(strTmp);
		if (bRTMgznExchange)		//v4.41T1	//Nichia
		{
			if (bDisplayAlarm)
			{
				SetAlert_Msg_Red_Yellow(IDS_BL_NO_THISMAG, (szMagName + strTmp));
			}
		}
		else
		{
			if (bDisplayAlarm)	//autoline1
			{
				SetAlert_Msg_Red_Yellow(IDS_BL_NO_THISMAG, (szMagName + strTmp));
			}
		}
		return FALSE;
	}
	return TRUE;
}


// return   0 not exist
//          1 far out right edge of bin table(move to right side)
//          2 far out left edge of bin table(move to left side)
//          3 on bin table
LONG CBinLoader::CheckFrameOnBinTable(VOID)
{
    BOOL bPosn = FALSE, bDetect = FALSE;
    bPosn = IsFrameOnBTPosition();
//	bDetect = !IsFrameProtection();

	//v4.50A22	
	if (IsMS90() && bPosn)
	{
		bDetect = TRUE;	//New BT platform design to eliminate this sensor bit
	}

    return (bPosn+bDetect*2);
}

LONG CBinLoader::CheckFrameOnBinTable2(VOID)
{
    BOOL bPosn, bDetect;
    bPosn = !IsFrameOnBT2Position();
	bDetect = !IsFrameProtection2();

    return (bPosn+bDetect*2);
}

BOOL CBinLoader::CheckFrameOnBufferTable(VOID)
{
	if (m_bDisableBL)		//v3.60
		return TRUE;

	//m_bBufferFrameExist	= m_piBufferFrameExist->IsLow();
	m_bBufferFrameExist	= !CMS896AStn::MotionReadInputBit(BL_SI_BufferFrameExist);
    return m_bBufferFrameExist;
}

//Dual Buffer Table Sensors
BOOL CBinLoader::IsDualBufferExist(const CString szBL_SI_DualBufferExistName)
{
	if (m_bDisableBL)
	{
		return FALSE;	//TRUE;		//v3.94
	}

	if (m_bNoSensorCheck)
	{
		return FALSE;
	}

	if ((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER) ||
		(m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER))
	{
		return CMS896AStn::MotionReadInputBit(szBL_SI_DualBufferExistName);
	}

	return TRUE;
}

BOOL CBinLoader::IsDualBufferUpperExist()
{
	m_bDualBufferUpperExist = IsDualBufferExist(BL_SI_DualBufferUpperExist);
	return m_bDualBufferUpperExist;
}

BOOL CBinLoader::IsDualBufferLowerExist()
{
	m_bDualBufferLowerExist = IsDualBufferExist(BL_SI_DualBufferLowerExist);
	return m_bDualBufferLowerExist;
}

BOOL CBinLoader::IsDualBufferRightProtect(VOID)
{
	if (m_bDisableBL)
		return TRUE;

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		m_bDualBufferRightProtect	= CMS896AStn::MotionReadInputBit(BL_SI_DualBufferRightProtect);
		return m_bDualBufferRightProtect;
	}
	else
	{
		return TRUE;
	}
}

BOOL CBinLoader::IsDualBufferLeftProtect(VOID)
{
	if (m_bDisableBL)
		return TRUE;

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		m_bDualBufferLeftProtect	= CMS896AStn::MotionReadInputBit(BL_SI_DualBufferLeftProtect);
		return m_bDualBufferLeftProtect;
	}
	else
	{
		return TRUE;
	}
}

BOOL CBinLoader::IsDualBufferLevelDown(VOID)
{
	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		m_bDualBufferLevel	= !CMS896AStn::MotionReadInputBit(BL_SI_DualBufferLevel);
		return m_bDualBufferLevel;
	}
	return TRUE;
}

//v4.59A40
BOOL CBinLoader::IsMS90BufPusherZAtSafePos()
{
	if (m_bDisableBL)
		return TRUE;
	if (!IsMS90())
		return TRUE;
	m_bMS90PusherZ	= CMS896AStn::MotionReadInputBit(BL_SI_MS90_BUF_PUSHER_Z_SAFETY);
	return m_bMS90PusherZ;
}

BOOL CBinLoader::IsMS90BufPusherXAtHomePos()
{
	if (m_bDisableBL)
		return TRUE;
	if (!IsMS90())
		return TRUE;
	m_bMS90PusherX	= !CMS896AStn::MotionReadInputBit(BL_SI_MS90_BUF_PUSHER_X_HOME);
	return m_bMS90PusherX;
}

BOOL CBinLoader::IsMS90BufPusherXAtLimitPos()
{
	if (m_bDisableBL)
		return TRUE;
	if (!IsMS90())
		return TRUE;
	m_bMS90PusherLimitX	= !CMS896AStn::MotionReadInputBit(BL_SI_MS90_BUF_PUSHER_X_LIMIT);
	return m_bMS90PusherLimitX;
}

BOOL CBinLoader::CheckMS90BugPusherAtSafePos(BOOL bForceCheck)
{
	if (m_bDisableBL)
		return TRUE;
	if (!m_bEnableMS90TablePusher)
	{
		if (!bForceCheck)
		{
			return TRUE;
		}
	}
	if ( m_bNoSensorCheck == TRUE )
		return TRUE;

	if (!IsMS90BufPusherZAtSafePos())
		return FALSE;
	if (!IsMS90BufPusherXAtHomePos())
		return FALSE;
	return TRUE;	//v4.59A45
}

// HAREWARE RELATIVE FUNCIONS----OUT BIT
VOID CBinLoader::SetGripperState(CBinGripperBuffer *pGripperBuffer, BOOL bSet)
{
	if (!m_fHardware)
	{
		return;
	}
#ifndef NU_MOTION
	if (m_bDisableBL)
	{
		return;
	}
#endif

	BOOL bOn = bSet;
	if (m_bGripperPusher)
	{
		bOn = !bOn;
	}

	CString szBitId = pGripperBuffer->GetGripperClampName();

	pGripperBuffer->SetGripperClampStatus(bOn);
	CMS896AStn::MotionSetOutputBit(szBitId, bOn);
}

//Upper Gripper
VOID CBinLoader::SetGripperState(BOOL bSet)
{
	CBinGripperBuffer *pGripperBuffer = &m_clUpperGripperBuffer;
	SetGripperState(pGripperBuffer, bSet);
}

//Lower Gripper
VOID CBinLoader::SetGripper2State(BOOL bSet)
{
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
	{
		return;
	}
	CBinGripperBuffer *pGripperBuffer = &m_clLowerGripperBuffer;
	SetGripperState(pGripperBuffer, bSet);
}

VOID CBinLoader::SetGripperLevel(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableBL)		//v3.60
		return;
#endif

	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_BUFFER)		//Gripper does not have up/down design for Dual Buffer
	{
		CMS896AStn::MotionSetOutputBit(BL_SO_GripperLevel, bSet);
	}
}

VOID CBinLoader::SetGripper2Level(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
		return;

	CMS896AStn::MotionSetOutputBit(BL_SO_Gripper2Level, bSet);
}

VOID CBinLoader::SetGripperPusher(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableBL)		//v3.60
		return;
#endif
	if (!m_bGripperPusher)
		return;

	CMS896AStn::MotionSetOutputBit(BL_SO_GripperPusher, bSet);
}

VOID CBinLoader::SetFrameVacuum(BOOL bSet)
{
	//CMS896AStn::m_bRealignFrameDone = FALSE;
	if ( (m_fHardware == FALSE) || (m_bUseBinTableVacuum == FALSE) )
		return;
#ifndef NU_MOTION
	if (m_bDisableBL)		//v3.60
		return;
#endif
	CMS896AStn::MotionSetOutputBit(BL_SO_FrameVac, bSet);
}

VOID CBinLoader::SetFrameVacuum2(BOOL bSet)
{
	if ( (m_fHardware == FALSE) || (m_bUseBinTableVacuum == FALSE) )
		return;
	if (m_bDisableBL)
		return;
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
		return;
	CMS896AStn::MotionSetOutputBit(BL_SO_FrameVac2, bSet);
}

VOID CBinLoader::SetFrameLevel(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableBL)		//v3.60
		return;
#endif
	//CMS896AStn::MotionSetOutputBit(BL_SO_FrameLevel, bSet);
	m_bFrameLevelOn = bSet;
}

BOOL CBinLoader::IsFrameLevelOn()
{
	return m_bFrameLevelOn;
}

VOID CBinLoader::SetFrameLevel2(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if (m_bDisableBL)
		return;
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
		return;
	//CMS896AStn::MotionSetOutputBit(BL_SO_FrameLevel2, bSet);
}

VOID CBinLoader::SetFrameAlign(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableBL)
		return;
#endif
		CMS896AStn::MotionSetOutputBit(BL_SO_FrameAlign, bSet);
}

VOID CBinLoader::SetFrameAlign2(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if (m_bDisableBL)
		return;
	if (m_lBinLoaderConfig != BL_CONFIG_DL_WITH_UPDN_BUFFER)
		return;
	CMS896AStn::MotionSetOutputBit(BL_SO_FrameAlign2, bSet);
}

VOID CBinLoader::SetBufferAlign(BOOL bSet)
{
	if (!m_fHardware)
		return;
#ifndef NU_MOTION
	if (m_bDisableBL)		//v3.60
		return;
#endif
	CMS896AStn::MotionSetOutputBit(BL_SO_BufferAlign, bSet);
}


VOID CBinLoader::SetFontArmLevel(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if (m_bDisableBL)		//v3.60
		return;
	CMS896AStn::MotionSetOutputBit(BL_SO_FArmLevel, bSet);
}


VOID CBinLoader::SetFontArmVacuum(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if (m_bDisableBL)		//v3.60
		return;
	CMS896AStn::MotionSetOutputBit(BL_SO_FArmVac, bSet);
}


VOID CBinLoader::SetRearArmLevel(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if (m_bDisableBL)		//v3.60
		return;
	CMS896AStn::MotionSetOutputBit(BL_SO_RArmLevel, bSet);
}


VOID CBinLoader::SetRearArmVacuum(BOOL bSet)
{
	if (!m_fHardware)
		return;
	if (m_bDisableBL)		//v3.60
		return;
	CMS896AStn::MotionSetOutputBit(BL_SO_RArmVac, bSet);
}

BOOL CBinLoader::SetBHFrontCoverLock(CONST BOOL bOpen)
{
	//Only used in AUTOBOND mode
	if ((State() != AUTO_Q)	&& (State() != DEMO_Q))
		return TRUE;

	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bLock = !bOpen;

	stMsg.InitMessage(sizeof(BOOL), &bLock);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, _T("FrontCoverLock"), stMsg);
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

	BOOL bResult = TRUE;
	stMsg.GetMsg(sizeof(BOOL), &bResult);
	return TRUE;
}


BOOL CBinLoader::SetBHSideCoverLock(CONST BOOL bOpen)
{
	//Only used in AUTOBOND mode
	if ((State() != AUTO_Q)	&& (State() != DEMO_Q))
		return TRUE;

	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	BOOL bLock = !bOpen;

	stMsg.InitMessage(sizeof(BOOL), &bLock);
	nConvID = m_comClient.SendRequest(BOND_HEAD_STN, _T("SideCoverLock"), stMsg);
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

	BOOL bResult = TRUE;
	stMsg.GetMsg(sizeof(BOOL), &bResult);
	return TRUE;
}

VOID CBinLoader::SetBinElevatorCoverLock(BOOL bSet)
{
	if ( (m_fHardware == FALSE) || (CMS896AApp::m_lBinElevatorCoverLock == FALSE) )
	{
		return;
	}

#ifndef NU_MOTION
	if (m_bDisableBH || m_bDisableBL)
		return;
#endif
	CMS896AStn::MotionSetOutputBit("oBinElevatorLock", bSet);	//BH_SO_BIN_EL_COVER
}

VOID CBinLoader::SetSideCoverLock(BOOL bSet)
{
	if ((m_fHardware == FALSE) || (CMS896AApp::m_lSideCoverLock == FALSE))
	{
		return;
	}

#ifndef NU_MOTION
	if (m_bDisableBH || m_bDisableBL)
	{
		return;
	}
#endif
	CMS896AStn::MotionSetOutputBit("oSideCoverLock", bSet);
}

//Not used in MS50
VOID CBinLoader::SetBufferLevel(BOOL bSet)
{
/*
	if (!m_fHardware)
	{
		return;
	}

	if (m_bDisableBL)		//v3.60
	{
		return;
	}

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{	
		if (m_bIsNewDualBuffer == FALSE)
		{
			if ( IsDualBufferLeftProtect() && IsDualBufferRightProtect() )	//safe for up/down
			{
				CMS896AStn::MotionSetOutputBit(BL_SO_DualBufferLevel, bSet);
			}
			else
			{
				SetErrorMessage("Frame does not sit in the buffer center");
				SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_IN_CENTER);		
			}
		}
		else
		{
			//LeftProtect = TRUE -> Frame is NOT on buffer center
			if ( (IsDualBufferUpperExist() && IsDualBufferLeftProtect())	 
				|| (IsDualBufferLowerExist() && IsDualBufferLeftProtect()) )
			{
				SetErrorMessage("Frame does not sit in the buffer center");
				SetAlert_Red_Yellow(IDS_BL_BUFFER_NOT_IN_CENTER);		
			}
			else
			{
				CMS896AStn::MotionSetOutputBit(BL_SO_DualBufferLevel, bSet);
			}
		}
	}
*/
	return;
}

VOID CBinLoader::SetALFrontGate(BOOL bSet)
{
	if (!m_fHardware || m_bDisableBL || !m_bDisableLoaderY)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(BL_SO_AL_FRONT_GATE, bSet);
}

VOID CBinLoader::SetALBackGate(BOOL bSet)
{
	if (!m_fHardware || m_bDisableBL || !m_bDisableLoaderY)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(BL_SO_AL_BACK_GATE, bSet);
}

/*
VOID CBinLoader::SetALMgznClamp(BOOL bSet)
{
	if (!m_fHardware || m_bDisableBL || !m_bDisableLoaderY)
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(BL_SO_AL_MGZN_CLAMP, bSet);
}
*/


VOID CBinLoader::SetMS90BufPusherZ(BOOL bSet)
{
	if (!m_fHardware || m_bDisableBL)
	{
		return;
	}
	if (!IsMS90())
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(BL_SO_MS90_BUF_PUSHER_Z, bSet);
}

VOID CBinLoader::SetMS90BufPusherX(BOOL bSet)
{
	if (!m_fHardware || m_bDisableBL)
	{
		return;
	}
	if (!IsMS90())
	{
		return;
	}
	CMS896AStn::MotionSetOutputBit(BL_SO_MS90_BUF_PUSHER_X, bSet);
}


BOOL CBinLoader::IsFrontGateSensorOn()
{
	if (!m_fHardware || m_bDisableBL)
	{
		return FALSE;
	}

	m_bFrontGateSensor = !CMS896AStn::MotionReadInputBit(BL_SI_FrontGateSnr);
	//if Cut the sensor, the status of sensor is 0
	//if leave the sensor, the status of sensor is 1
	return m_bFrontGateSensor;
}

BOOL CBinLoader::IsBackGateSensorOn()
{
	if (!m_fHardware || m_bDisableBL)
	{
		return FALSE;
	}

	m_bBackGateSensor = !CMS896AStn::MotionReadInputBit(BL_SI_BackGateSnr);
	//if Cut the sensor, the status of sensor is 0
	//if leave the sensor, the status of sensor is 1
	return m_bBackGateSensor;
}


BOOL CBinLoader::IsAllMagazineExist(CONST BOOL bNeedByPassDisableAlarm)
{
	BOOL bReturn = TRUE;
	INT i;

	if (m_bDisableBL)
	{
		return TRUE;
	}

	if (m_bNoSensorCheck == TRUE)
	{
		return TRUE;
	} 

	if ((m_lBinLoaderConfig == BL_CONFIG_DL_WITH_UPDN_BUFFER) || CMS896AStn::m_bUseBinMultiMgznSnrs)
	{
		//For 6-sensor config

		if (IsMSAutoLineMode() || IsMSAutoLineStandloneMode())
		{
			m_bMagazineExist[0] = TRUE;
		}
		else
		{
			m_bMagazineExist[0] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist1);
		}
		m_bMagazineExist[1] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist2);
		m_bMagazineExist[2] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist3);
		m_bMagazineExist[3] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist4);
		m_bMagazineExist[4] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist5);
		m_bMagazineExist[5] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist6);
		m_bMagazineExist[6] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist7);
		m_bMagazineExist[7] = CMS896AStn::MotionReadInputBit(BL_SI_MagExist8);
		
		for (i = MS_BL_MGZN_NUM-1; i >= 0; i--)
		{
			if (m_bMagazineExist[i] == FALSE)
			{
				bReturn = FALSE;
				break;
			}
		}
		
	}

	if (!bReturn)
	{
		CString strTmp;
		CString szMagName;

        switch(i)
        {
			case 0:
				szMagName = "Top 1 "; 
				break;
			case 1:
				szMagName = "Middle 1 "; 
				break;
			case 2:
				szMagName = "Bottom 1 "; 
				break;
			case 3:
				szMagName = "Top 2 "; 
				break;
			case 4:
				szMagName = "Middle 2 "; 
				break;
			case 5:
				szMagName = "Bottom 2 "; 
				break;
			case 6:
				szMagName = "8-Bottom 1 ";
				break;
			case 7:
				szMagName = "8-Bottom 2 ";
				break;
			default: 
				szMagName = "Invalid "; 
				break;
		}

		strTmp.Format(" magazine [%d] does not exist!", i);
		SetErrorMessage(strTmp);

		if (!bNeedByPassDisableAlarm)
		{
			SetAlert_Msg_Red_Yellow(IDS_BL_NO_THISMAG, (szMagName + strTmp));
		}

		return bReturn;
	}

	return bReturn;
}