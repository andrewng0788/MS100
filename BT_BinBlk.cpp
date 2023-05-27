#include "stdafx.h"
#include <Math.h>
#include <WaferMapWrapper.h>
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_BinBlk.h"
#include "BT_BinBlkMain.h"
#include "FileUtil.h"
#include "MathFunc.h"

#pragma once

/****************************** Class CBinBlk End ******************************/
char	CBinBlk::m_cBondedDieMap[MAX_BINMAP_SIZE_X][MAX_BINMAP_SIZE_Y];	
INT		CBinBlk::m_nBinMap[MAX_BINMAP_SIZE_X][MAX_BINMAP_SIZE_Y];	


/***********************************/
/*     Constructor/Destructor      */
/***********************************/

CBinBlk::CBinBlk()
{
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	m_pstNvBinBlkData		= NULL;
	m_pstNvBinBlkData_HW	= NULL;

	m_lUpperLeftX = 0;
	m_lUpperLeftXFromSetup = 0;
	m_lUpperLeftY = 0;
	m_lUpperLeftYFromSetup = 0;
	m_lLowerRightX = 0; 
	m_lLowerRightXFromSetup = 0;
	m_lLowerRightY = 0;
	m_lLowerRightYFromSetup = 0;
	m_ulWidth = 0;
	m_ulWidthFromSetup = 0;
	m_ulHeight = 0;
	m_ulHeightFromSetup = 0;
	m_ucGrade = 1;
	m_ucOriginalGrade = 1;
	m_dDiePitchX = 500;
	m_dDiePitchY = 500;
	m_bPoint5UmInDiePitchX	= FALSE;		//v4.42T5
	m_bPoint5UmInDiePitchY	= FALSE;		//v4.42T5
	m_dBondAreaOffsetX		= 0;
	m_dBondAreaOffsetY		= 0;
	m_dThermalDeltaPitchX	= 0;			//v4.59A22			
	m_dThermalDeltaPitchY	= 0;			//v4.59A22			
	m_ulWalkPath = 0;
	m_ulDiePerRow = 0;
	m_ulDiePerCol = 0;
	m_ulDiePerUserRow = 0;
	m_ulDiePerUserCol = 0;
	m_ulDiePerBlk = 0;
	m_ulSkipUnit = 0;
	m_ulMaxUnit = 0;
	m_bIsDisableFromSameGradeMerge = FALSE;
	m_ulPhyBlkId = 0;
	m_bIsAssignedWithSlot = TRUE;
	m_bIsSetup = FALSE;
	m_ucStatus = 0;
	m_ucSortDirection	= 0;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
	m_bByPassBinMap	= FALSE;

	m_bFirstTimeWriteTempFile = TRUE;

	m_bIsDatabaseGenerated = FALSE;
	m_szOutputFilename = "";
	m_ulSerialNo = 0;
	m_szLastResetSerialNoDate = "";
	m_szExtraInfoFormat = "";
	m_szStartDate = "";
	m_ulAccmulatedCount = 0;

	//v3.70T3
	m_bUseCircularPattern = FALSE;
	m_lCenterX	= 0;
	m_lCenterY	= 0;
	m_lRadius	= 0;

	m_bEnableBinMap		= FALSE;
	m_bEnableBinMapOffset = FALSE;
	m_lBinMapRowOffset	= 0;
	m_lBinMapColOffset	= 0;
	m_ulBinMapWalkPath	= 0;
	m_dBinMapCircleRadius	= 0;
	m_lBinMapTopEdge	= 0;
	m_lBinMapBtmEdge	= 0;
	m_lBinMapLeftEdge	= 0;
	m_lBinMapRightEdge	= 0;

	m_ucFrameRotation	= BBX_FRAME_ROTATE_0;	//0-degree by default	//v4.49A7
	
	m_ulEmptyUnit = 0;

	m_szaDieBondTime.SetSize(1000);
	m_dwaWMapX.SetSize(1000);
	m_dwaWMapY.SetSize(1000);
	m_dwaWMapSeqNo.SetSize(1000);
	m_dwaWMapEncX.SetSize(1000);
	m_dwaWMapEncY.SetSize(1000);
	m_dwaBtEncX.SetSize(1000);
	m_dwaBtEncY.SetSize(1000);

	m_szMappedGrade = "";

	for (INT i = 0; i < BINBLK_MAX_SIZE; i++)
		for (INT j = 0; j < 10; j++)
			m_ulHoleDieIndex[i][j] = 0;

	m_ulHoleDieNum = 0;

	m_bEnableFirstRowColSkipPattern = FALSE;
	m_ulFirstRowColSkipUnit = 0;

	m_bTeachWithPhysicalBlk			= FALSE;
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		//m_bTeachWithPhysicalBlk = TRUE;
	}
	m_bUseBlockCornerAsFirstDiePos	= FALSE;
	m_bNoReturnTravel				= FALSE;

} //end constructor


CBinBlk::~CBinBlk()
{
	if (m_dwaWMapX.GetSize() > 0)
		m_dwaWMapX.RemoveAll();

	if (m_dwaWMapY.GetSize() > 0)
		m_dwaWMapY.RemoveAll();

	if (m_dwaWMapSeqNo.GetSize() > 0)
		m_dwaWMapSeqNo.RemoveAll();

	if (m_szaDieBondTime.GetSize() > 0)
		m_szaDieBondTime.RemoveAll();

	//andrewng //2020-0619
	if (m_dwaWMapEncX.GetSize() > 0)
		m_dwaWMapEncX.RemoveAll();

	if (m_dwaWMapEncY.GetSize() > 0)
		m_dwaWMapEncY.RemoveAll();

	//andrewng //2020-0630
	if (m_dwaBtEncX.GetSize() > 0)
		m_dwaBtEncX.RemoveAll();

	if (m_dwaBtEncY.GetSize() > 0)
		m_dwaBtEncY.RemoveAll();

} //end destructor


LONG CBinBlk::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}


/***********************************/
/*        Get/Set functions        */
/***********************************/
LONG CBinBlk::GetUpperLeftX()
{
	return m_lUpperLeftX;
} //end GetUpperLeftX


BOOL CBinBlk::SetUpperLeftX(LONG lUpperLeftX)
{
	m_lUpperLeftX = lUpperLeftX;
	return TRUE;
} //end SetUpperLeftX


LONG CBinBlk::GetUpperLeftXFromSetup()
{
	return m_lUpperLeftXFromSetup;
} //end GetUpperLeftXFromSetup


BOOL CBinBlk::SetUpperLeftXFromSetup(LONG lUpperLeftXFromSetup)
{
	m_lUpperLeftXFromSetup = lUpperLeftXFromSetup;
	return TRUE;
} //end SetUpperLeftXFromSetup

LONG CBinBlk::GetUpperLeftY()
{
	return m_lUpperLeftY;
} //end GetUpperLeftY


BOOL CBinBlk::SetUpperLeftY(LONG lUpperLeftY)
{
	m_lUpperLeftY = lUpperLeftY;
	return TRUE;
} //end SetUpperLeftY


LONG CBinBlk::GetUpperLeftYFromSetup()
{
	return m_lUpperLeftYFromSetup;
} //end GetUpperLeftYFromSetup


BOOL CBinBlk::SetUpperLeftYFromSetup(LONG lUpperLeftYFromSetup)
{
	m_lUpperLeftYFromSetup = lUpperLeftYFromSetup;
	return TRUE;
} //end SetUpperLeftYFromSetup


LONG CBinBlk::GetLowerRightX()
{
	return m_lLowerRightX;
} //end GetLowerRightX


BOOL CBinBlk::SetLowerRightX(LONG lLowerRightX)
{
	m_lLowerRightX = lLowerRightX;
	return TRUE;    
} //end SetLowerRightX


LONG CBinBlk::GetLowerRightXFromSetup()
{
	return m_lLowerRightXFromSetup;
} //end GetLowerRightXFromSetup


BOOL CBinBlk::SetLowerRightXFromSetup(LONG lLowerRightXFromSetup)
{
	m_lLowerRightXFromSetup = lLowerRightXFromSetup;
	return TRUE;    
} //end SetLowerRightXFromSetup


LONG CBinBlk::GetLowerRightY()
{
	return m_lLowerRightY;
} //end GetLowerRightY


BOOL CBinBlk::SetLowerRightY(LONG lLowerRightY)
{
	m_lLowerRightY = lLowerRightY;
	return TRUE;
} //end SetLowerRightY
		

LONG CBinBlk::GetLowerRightYFromSetup()
{
	return m_lLowerRightYFromSetup;
} //end GetLowerRightYFromSetup


BOOL CBinBlk::SetLowerRightYFromSetup(LONG lLowerRightYFromSetup)
{
	m_lLowerRightYFromSetup = lLowerRightYFromSetup;
	return TRUE;
} //end SetLowerRightYFromSetup


ULONG CBinBlk::GetWidth()
{
	return m_ulWidth;
} //end GetWidth


BOOL CBinBlk::SetWidth()
{
	m_ulWidth = m_lLowerRightX - m_lUpperLeftX;
	return TRUE;
} //end SetWidth


ULONG CBinBlk::GetWidthFromSetup()
{
	return m_ulWidthFromSetup;
} //end GetWidthFromSetup


BOOL CBinBlk::SetWidthFromSetup()
{
	m_ulWidthFromSetup = m_lLowerRightXFromSetup - m_lUpperLeftXFromSetup;
	return TRUE;
} //end SetWidthFromSetup
		

ULONG CBinBlk::GetHeight()
{
	return m_ulHeight;
} //end GetHeight


BOOL CBinBlk::SetHeight()
{
	m_ulHeight = m_lLowerRightY - m_lUpperLeftY;
	return TRUE;
} //end SetHeight


ULONG CBinBlk::GetHeightFromSetup()
{
	return m_ulHeightFromSetup;
} //end GetHeightFromSetup


BOOL CBinBlk::SetHeightFromSetup()
{
	m_ulHeightFromSetup = m_lLowerRightYFromSetup - m_lUpperLeftYFromSetup;
	return TRUE;
} //end SetHeightFromSetup


UCHAR CBinBlk::GetGrade()
{
	return m_ucGrade;
} //end GetGrade


BOOL CBinBlk::SetGrade(UCHAR ucGrade)
{
	m_ucGrade = ucGrade;
	//m_ucGrade = ucGrade + '0';
	return TRUE;
} //end SetGrade


UCHAR CBinBlk::GetOriginalGrade()
{
	return m_ucOriginalGrade;
} //end GetOriginalGrade


BOOL CBinBlk::SetOriginalGrade(UCHAR ucGrade)
{
	m_ucOriginalGrade = ucGrade;
	return TRUE;
} //end SetOriginalGrade


BOOL CBinBlk::SetDDiePitchX(DOUBLE dDiePitchX)
{
	m_dDiePitchX = dDiePitchX;
	return TRUE;
}

BOOL CBinBlk::SetDDiePitchY(DOUBLE dDiePitchY)
{
	m_dDiePitchY = dDiePitchY;
	return TRUE;
}

DOUBLE CBinBlk::GetDDiePitchX()
{
	return m_dDiePitchX;
}

DOUBLE CBinBlk::GetDDiePitchY()
{
	return m_dDiePitchY;
}

BOOL CBinBlk::GetPoint5UmInDiePitchX()
{
	return m_bPoint5UmInDiePitchX;
}

VOID CBinBlk::SetPoint5UmInDiePitchX(CONST BOOL bEnable)
{
	m_bPoint5UmInDiePitchX = bEnable;
}

BOOL CBinBlk::GetPoint5UmInDiePitchY()
{
	return m_bPoint5UmInDiePitchY;
}

VOID CBinBlk::SetPoint5UmInDiePitchY(CONST BOOL bEnable)
{
	m_bPoint5UmInDiePitchY = bEnable;
}

BOOL CBinBlk::GetByPassBinMap()
{
	return m_bByPassBinMap;
}

VOID CBinBlk::SetByPassBinMap(CONST BOOL bEnable)
{
	m_bByPassBinMap = bEnable;
}

DOUBLE CBinBlk::GetBondAreaOffsetX()
{
	return m_dBondAreaOffsetX;
}

DOUBLE CBinBlk::GetBondAreaOffsetY()
{
	return m_dBondAreaOffsetY;
}

DOUBLE CBinBlk::GetThermalDeltaPitchX()
{
	return m_dThermalDeltaPitchX;
}

DOUBLE CBinBlk::GetThermalDeltaPitchY()
{
	return m_dThermalDeltaPitchY;
}

VOID CBinBlk::SetBondAreaOffset(DOUBLE dOffsetX, DOUBLE dOffsetY)
{
	m_dBondAreaOffsetX = dOffsetX;
	m_dBondAreaOffsetY = dOffsetY;
}

VOID CBinBlk::SetThermalDeltaPitch(DOUBLE dX, DOUBLE dY)
{
	m_dThermalDeltaPitchX = dX;
	m_dThermalDeltaPitchY = dY;
}

DOUBLE CBinBlk::CalculateThermalDeltaPitchX(LONG lCurrCol)
{
	if (m_dThermalDeltaPitchX == 0)
		return 0;
	DOUBLE dMidCol = GetDiePerRow() / 2.0 + 0.5;
	LONG lMidCol = (LONG) dMidCol;
	DOUBLE dX = m_dThermalDeltaPitchX * (lCurrCol - lMidCol);

	CString szLog;
	szLog.Format("BLK: Thermal Delta X = %f (CurrCol = %ld, MidCol = %ld, MaxCol = %ld",
					dX, lCurrCol, lMidCol, GetDiePerRow());
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

	return dX;
}

DOUBLE CBinBlk::CalculateThermalDeltaPitchY(LONG lCurrRow)
{
	if (m_dThermalDeltaPitchY == 0)
		return 0;
	DOUBLE dMidRow = GetDiePerCol() / 2.0 + 0.5;
	LONG lMidRow = (LONG) dMidRow;
	DOUBLE dY = m_dThermalDeltaPitchY * (lCurrRow - lMidRow);

	CString szLog;
	szLog.Format("BLK: Thermal Delta Y = %f (CurrRow = %ld, MidRow = %ld, MaxRow = %ld",
					dY, lCurrRow, lMidRow, GetDiePerCol());
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);

	return dY;
}

ULONG CBinBlk::GetWalkPath()
{
	ULONG ulWalkPath = GetOriginalWalkPath();

	return GetWalkPath(ulWalkPath);
} //end GetWalkPath


ULONG CBinBlk::GetWalkPath(const ULONG ulCurWalkPath)
{
	ULONG ulWalkPath = ulCurWalkPath;
	
	switch (m_ucFrameRotation)
	{
	case BBX_FRAME_ROTATE_180:
		if (ulWalkPath == BT_TLH_PATH)
		{
			ulWalkPath = BT_BRH_PATH;
		}
		else if (ulWalkPath == BT_TLV_PATH)
		{
			ulWalkPath = BT_BRV_PATH;
		}
		else if (ulWalkPath == BT_TRH_PATH)
		{
			ulWalkPath = BT_BLH_PATH;
		}
		else if (ulWalkPath == BT_TRV_PATH)
		{
			ulWalkPath = BT_BLV_PATH;
		}
		else if (ulWalkPath == BT_BLH_PATH)
		{
			ulWalkPath = BT_TRH_PATH;
		}
		else if (ulWalkPath == BT_BLV_PATH)
		{
			ulWalkPath = BT_TRV_PATH;
		}
		else if (ulWalkPath == BT_BRH_PATH)
		{
			ulWalkPath = BT_TLH_PATH;
		}
		else if (ulWalkPath == BT_BRV_PATH)
		{
			ulWalkPath = BT_TLV_PATH;
		}
		break;

	case BBX_FRAME_ROTATE_0:
	default:
		break;
	}

	return ulWalkPath;
} //end GetWalkPath

ULONG CBinBlk::GetOriginalWalkPath()
{
	//andrewng //2020-0805
	//m_ulWalkPath = BT_TLH_PATH; //hardcode

	//Disregard current frame rotation angle in m_ucFrameRotation
	return m_ulWalkPath;	
}

BOOL CBinBlk::SetWalkPath(ULONG ulWalkPath)
{
	m_ulWalkPath = ulWalkPath;

	//CString szLog;
	//szLog.Format("SetWalkPath = %d", ulWalkPath); 	
	//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	//andrewng //2020-0805
	/*if (m_ulWalkPath != BT_TLH_PATH)
	{
		CString szLog;
		szLog.Format("SetWalkPath is not TLH"); 	
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}

	m_ulWalkPath = BT_TLH_PATH; //hardcode
	*/
	return TRUE;
} //end SetWalkPath

BOOL CBinBlk::SetFrameRotation(UCHAR ucRotation, LONG lBinCalibX, LONG lBinCalibY)
{
	m_ucFrameRotation	= ucRotation;
	m_lBinCalibX		= lBinCalibX;
	m_lBinCalibY		= lBinCalibY;
/*
	if ( (m_ulPhyBlkId > 0) && (m_ulPhyBlkId < 4) )	//v4.49A9
	{
		CString szLog;
		szLog.Format("SetFrameRotation - Blk #%d: UL(%ld, %ld); COR(%ld, %ld); LR(%ld, %ld)",
			m_ulPhyBlkId, m_lUpperLeftX, m_lUpperLeftY, lBinCalibX, lBinCalibY, m_lLowerRightX, m_lLowerRightY); 	
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
*/
	return TRUE;
}

LONG CBinBlk::GetBinCalibX()
{
	return m_lBinCalibX;
}


LONG CBinBlk::GetBinCalibY()
{
	return m_lBinCalibY;
}

BOOL CBinBlk::SetFrameRotationOnly(UCHAR ucRotation)//enablesubbin
{
	m_ucFrameRotation	= ucRotation;
	return TRUE;
}

UCHAR CBinBlk::GetFrameRotation()
{
	return m_ucFrameRotation;
}

BOOL CBinBlk::RotateBondPos(DOUBLE& dX, DOUBLE& dY)		//v4.59A19
{
	if (m_ucFrameRotation != BBX_FRAME_ROTATE_180)
		return FALSE;
	if (m_lBinCalibX == 0)
		return FALSE;
	if (m_lBinCalibY == 0)
		return FALSE;

	DOUBLE dOldX = dX;
	DOUBLE dOldY = dY;

	DOUBLE dDX=0, dDY=0;
	DOUBLE dNewX=0, dNewY=0;

	if (dX < m_lBinCalibX)
	{
		dDX		= m_lBinCalibX - dX;
		dNewX	= dX + 2 * dDX;
	}
	else
	{
		dDX		= dX - m_lBinCalibX;
		dNewX	= dX - 2 * dDX;
	}

	if (dY < m_lBinCalibY)
	{
		dDY		= m_lBinCalibY - dY;
		dNewY	= dY + 2 * dDY;
	}
	else
	{
		dDY = dY - m_lBinCalibY;
		dNewY	= dY - 2 * dDY;
	}

	dX = dNewX;
	dY = dNewY;

	CString szLog;
	szLog.Format("BT 180 rotation: OLD (%f, %f), CENTER(%ld, %ld), NEW(%f, %f)",
				dOldX, dOldY, m_lBinCalibX, m_lBinCalibY, dNewX, dNewY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	return TRUE;
}

BOOL CBinBlk::RotateBlkCorners(LONG& lULX, LONG& lULY, LONG& lLRX, LONG& lLRY)
{
	if (m_ucFrameRotation != BBX_FRAME_ROTATE_180)
		return FALSE;
	if (m_lBinCalibX == 0)
		return FALSE;
	if (m_lBinCalibY == 0)
		return FALSE;

	LONG lOldULX = lULX;
	LONG lOldULY = lULY;
	LONG lOldLRX = lLRX;
	LONG lOldLRY = lLRY;

	LONG lDX = m_lBinCalibX - lULX;
	LONG lDY = m_lBinCalibY - lULY;
	LONG lNewLRX = lULX + 2 * lDX;
	LONG lNewLRY = lULY + 2 * lDY;

	lDX = lLRX - m_lBinCalibX;
	lDY = lLRY - m_lBinCalibY;
	LONG lNewULX = lLRX - 2 * lDX;
	LONG lNewULY = lLRY - 2 * lDY;

	lULX = lNewULX;
	lULY = lNewULY;
	lLRX = lNewLRX;
	lLRY = lNewLRY;
/*
	if ( (m_ulPhyBlkId > 0) && (m_ulPhyBlkId < 4) )		//v4.49A9
	{
		CString szLog;
		szLog.Format("RotateBlkCorners - Blk #%d: UL(%ld, %ld); COR(%ld, %ld); New-LR(%ld, %ld)",
			m_ulPhyBlkId, lULX, lULY, 
			m_lBinCalibX, m_lBinCalibY, 
			lNewLRX, lNewLRY); 	
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

		szLog.Format("RotateBlkCorners - Blk #%d: LR(%ld, %ld); COR(%ld, %ld); New-UL(%ld, %ld)",
			m_ulPhyBlkId, lLRX, lLRY, 
			m_lBinCalibX, m_lBinCalibY, 
			lNewULX, lNewULY); 	
		CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
	}
*/
	return TRUE;
}

/*
BOOL CBinBlk::FrameRotateTransformPosXY(LONG& lX, LONG& lY)
{
	if (m_ucFrameRotation != BBX_FRAME_ROTATE_180)
	{
CMSLogFileUtility::Instance()->MS_LogOperation("FrameRotateTransformPosXY: fail 1");
		return FALSE;
	}
	if (m_lBinCalibX == 0)
	{
CMSLogFileUtility::Instance()->MS_LogOperation("FrameRotateTransformPosXY: fail 2");
		return FALSE;
	}
	if (m_lBinCalibY == 0)
	{
CMSLogFileUtility::Instance()->MS_LogOperation("FrameRotateTransformPosXY: fail 3");
		return FALSE;
	}
	

	LONG lOldX = lX;
	LONG lOldY = lY;
	LONG lDX = labs(m_lBinCalibX - lX);
	LONG lDY = labs(m_lBinCalibY - lY);

	LONG lNewX=lX, lNewY=lY;
	if (lX < m_lBinCalibX)
		lNewX = lOldX + 2 * lDX;
	else
		lNewX = lOldX - 2 * lDX;

	if (lY < m_lBinCalibY)
		lNewY = lOldY + 2 * lDY;
	else
		lNewY = lOldY - 2 * lDY;

	lX = lNewX;
	lY = lNewY;

	//v4.50A25
	CString szLog;
	szLog.Format("Centralize XY (180): Old (%ld, %ld), DXY (%ld, %ld), Cal (%ld, %ld), New (%ld, %ld)",
			lOldX, lOldY, lDX, lDY, m_lBinCalibX, m_lBinCalibY, lNewX, lNewY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	return TRUE;
}
*/

BOOL CBinBlk::CalculateFrameRotateXY(LONG lX, LONG lY, LONG& lNewX, LONG& lNewY)
{
	if (m_ucFrameRotation != BBX_FRAME_ROTATE_180)
		return FALSE;
	if (m_lBinCalibX == 0)
		return FALSE;
	if (m_lBinCalibY == 0)
		return FALSE;

	LONG lOldX = lX;
	LONG lOldY = lY;

	LONG lDX = m_lBinCalibX - lX;
	LONG lDY = m_lBinCalibY - lY;
	LONG lNewPosX = lX + 2 * lDX;
	LONG lNewPosY = lY + 2 * lDY;

	lNewX = lNewPosX;
	lNewY = lNewPosY;
	return TRUE;
}

ULONG CBinBlk::GetDiePerRow()
{
	return m_ulDiePerRow;
} //end GetDiePerRow
	

BOOL CBinBlk::SetDiePerRow(ULONG ulDiePerRow)
{
	m_ulDiePerRow = ulDiePerRow;
	return TRUE;
} //end SetDiePerRow	


ULONG CBinBlk::GetDiePerCol()
{
	return m_ulDiePerCol;
} //end GetDiePerCol
	

BOOL CBinBlk::SetDiePerCol(ULONG ulDiePerCol)
{
	m_ulDiePerCol = ulDiePerCol;
	return TRUE;
} //end SetDiePerCol	


ULONG CBinBlk::GetDiePerBlk()
{
	return m_ulDiePerBlk;
} //end of GetDiePerBlk
	
BOOL CBinBlk::SetDiePerBlk(ULONG ulDiePerBlk)
{
	m_ulDiePerBlk = ulDiePerBlk;
	return TRUE;
} //end SetDiePerBlk


BOOL CBinBlk::GetIsDisableFromSameGradeMerge()
{
	return m_bIsDisableFromSameGradeMerge;
} //end GetIsDisableFromSameGradeMerge


BOOL CBinBlk::SetIsDisableFromSameGradeMerge(BOOL bIsDisableFromSameGradeMerge)
{
	m_bIsDisableFromSameGradeMerge= bIsDisableFromSameGradeMerge;
	return TRUE;
} //end SetIsDisableFromSameGradeMerge


ULONG CBinBlk::GetPhyBlkId()
{
	return m_ulPhyBlkId;
} //end GetPhyBlkId


BOOL CBinBlk::SetPhyBlkId(ULONG ulPhyBlkId)
{
	m_ulPhyBlkId = ulPhyBlkId;
	return TRUE;
} //end SetPhyBlkId


BOOL CBinBlk::GetIsAssignedWithSlot()
{
	return m_bIsAssignedWithSlot;
} //end GetIsAssignedWithSlot


BOOL CBinBlk::SetIsAssignedWithSlot(BOOL bIsAssignedWithSlot)
{
	m_bIsAssignedWithSlot = bIsAssignedWithSlot;
	return TRUE;
} //end SetIsAssignedWithSlot


BOOL CBinBlk::GetIsSetup()
{
	return m_bIsSetup;
} //end GetIsSetup


BOOL CBinBlk::SetIsSetup(BOOL bIsSetup)
{
	m_bIsSetup = bIsSetup;
	return TRUE;
} //end SetIsSetup


UCHAR CBinBlk::GetStatus()
{
	return m_ucStatus;
} //end GetStatus


BOOL CBinBlk::SetStatus(UCHAR ucStatus)
{
	m_ucStatus = ucStatus;
	return TRUE;
} //end SetStatus

UCHAR CBinBlk::GetSortDirection()
{
	return m_ucSortDirection;
}

BOOL CBinBlk::GetIsDatabaseGenerated()
{
	return m_bIsDatabaseGenerated;
} //end GetIsDatabaseGenerated


BOOL CBinBlk::SetIsDatabaseGenerated(BOOL bIsDatabaseGenerated)
{
	m_bIsDatabaseGenerated = bIsDatabaseGenerated;
	return TRUE;
} //end SetIsDatabaseGenerated


ULONG CBinBlk::GetSerialNo()
{
	return m_ulSerialNo;
} //end GetSerialNo


BOOL CBinBlk::SetSerialNo(ULONG ulSerialNo)
{
	m_ulSerialNo = ulSerialNo;
	return TRUE;
} //end SetSerialNo


CString CBinBlk::GetLastResetSerialNoDate()
{
	return m_szLastResetSerialNoDate;
} //end GetLastResetSerialNoDate


BOOL CBinBlk::SetLastResetSerialNoDate(CString szLastResetSerialNoDate)
{
	m_szLastResetSerialNoDate = szLastResetSerialNoDate;
	return TRUE;
} //end SetLastResetSerialNoDate


CString CBinBlk::GetExtraInfoFormat()
{
	return m_szExtraInfoFormat;
} //end GetExtraInfoFormat


BOOL CBinBlk::SetExtraInfoFormat(CString szExtraInfoFormat)
{
	m_szExtraInfoFormat = szExtraInfoFormat;
	return TRUE;
} //end SetExtraInfoFormat


BOOL CBinBlk::SetPSTNVBinBlkData(void *pvNVRAM, LONG lNVRAM_BinTable_Start, ULONG ulBlkId)
{
	BT_NVBINBLKDATA *pTemp;

	pTemp = (BT_NVBINBLKDATA*)((ULONG)pvNVRAM + lNVRAM_BinTable_Start);
	m_pstNvBinBlkData = pTemp + ulBlkId;

	return TRUE;
} //end SetPSTNVBinBlkData

BOOL CBinBlk::SetPSTNVBinBlkData_HW(void *pvNVRAM_HW, LONG lNVRAM_BinTable_Start, ULONG ulBlkId)
{
	BT_NVBINBLKDATA *pTemp;

	pTemp = (BT_NVBINBLKDATA*)((ULONG)pvNVRAM_HW + lNVRAM_BinTable_Start);
	m_pstNvBinBlkData_HW = pTemp + ulBlkId;

	return TRUE;
}


ULONG CBinBlk::GetNVNoOfBondedDie()
{
	if( m_pstNvBinBlkData==NULL )
		return 0;
	return m_pstNvBinBlkData->ulNoOfBondedDie;
} //end GetNVNoOfBondedDie


BOOL CBinBlk::SetNVNoOfBondedDie(ULONG ulNvNoOfBondedDie)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->ulNoOfBondedDie = ulNvNoOfBondedDie;
	//v4.65A3
	if (m_pstNvBinBlkData_HW != NULL)
	{
		m_pstNvBinBlkData_HW->ulNoOfBondedDie = ulNvNoOfBondedDie;
	}

	return TRUE;
} //end SetNVNoOfBondedDie


ULONG CBinBlk::GetNVCurrBondIndex()
{
	if( m_pstNvBinBlkData==NULL )
		return 0;
	return m_pstNvBinBlkData->ulCurrBondIndex;
} //end GetNVCurrBondIndex


BOOL CBinBlk::SetNVCurrBondIndex(ULONG ulNvCurrBondIndex)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->ulCurrBondIndex = ulNvCurrBondIndex;
	//v4.65A3
	if (m_pstNvBinBlkData_HW != NULL)
	{
		m_pstNvBinBlkData_HW->ulCurrBondIndex = ulNvCurrBondIndex;
	}
	return TRUE;
} //end SetNVCurrBondIndex


ULONG CBinBlk::GetNVLastFileSaveIndex()
{
	if( m_pstNvBinBlkData==NULL )
		return 0;
	return m_pstNvBinBlkData->ulLastFileSaveIndex;
} //end GetNVLastFileSaveIndex


BOOL CBinBlk::SetNVLastFileSaveIndex(ULONG ulNvLastFileSaveIndex)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->ulLastFileSaveIndex = ulNvLastFileSaveIndex;
	//v4.65A3
	if (m_pstNvBinBlkData_HW != NULL)
	{
		m_pstNvBinBlkData_HW->ulLastFileSaveIndex = ulNvLastFileSaveIndex;
	}	
	return TRUE;
} //end SetNVLastFileSaveIndex


BOOL CBinBlk::GetNVIsFull()
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	return m_pstNvBinBlkData->bIsFull;
} //end GetNVIsFull


BOOL CBinBlk::SetNVIsFull(BOOL bNvIsFull)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->bIsFull = bNvIsFull;
	//v4.65A3
	if (m_pstNvBinBlkData_HW != NULL)
	{
		m_pstNvBinBlkData_HW->bIsFull = bNvIsFull;
	}	
	return TRUE;
} //end SetNVIsFull


BOOL CBinBlk::GetNVIsAligned()
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	return m_pstNvBinBlkData->bIsAligned;
} //end GetNVIsAligned


BOOL CBinBlk::SetNVIsAligned(BOOL bIsAligned)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->bIsAligned = bIsAligned;
	return TRUE;
} //end SetNVIsAligned


LONG CBinBlk::GetNVXOffset()
{
	if( m_pstNvBinBlkData==NULL )
		return 0;
	return m_pstNvBinBlkData->lXOffset;
} // end GetNVXOffset


BOOL CBinBlk::SetNVXOffset(LONG lXOffset)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->lXOffset = lXOffset;
	return TRUE;
} //end SetNVXOffset


LONG CBinBlk::GetNVYOffset()
{
	if( m_pstNvBinBlkData==NULL )
		return 0;
	return m_pstNvBinBlkData->lYOffset;
} //end GetNVYOffset


BOOL CBinBlk::SetNVYOffset(LONG lYOffset)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->lYOffset = lYOffset;
	return TRUE;
} //end SetNVYOffset


DOUBLE CBinBlk::GetNVRotateAngleX()
{
	if( m_pstNvBinBlkData==NULL )
		return 0;
	return m_pstNvBinBlkData->dRotateAngleX;
} //end GetNVRotateAngleX


BOOL CBinBlk::SetNVRotateAngleX(DOUBLE dRotateAngleX)
{
	if( m_pstNvBinBlkData==NULL )
		return FALSE;
	m_pstNvBinBlkData->dRotateAngleX = dRotateAngleX;
	return TRUE;
} //end SetNVRotateAngleX


BOOL CBinBlk::SetSkipUnit(ULONG ulSkipNo)
{
	m_ulSkipUnit = ulSkipNo;
	return TRUE;
} //end SetSkipUnit


ULONG CBinBlk::GetSkipUnit()
{
	return m_ulSkipUnit;
} //end GetSkipUnit


BOOL CBinBlk::SetMaxUnit(ULONG ulMaxNo)
{
	m_ulMaxUnit = ulMaxNo;
	return TRUE;
} //end SetMaxUnit


ULONG CBinBlk::GetMaxUnit()
{
	return m_ulMaxUnit;
} //end GetMaxUnit


BOOL CBinBlk::SetEmptyUnit(ULONG ulEmptyNo)
{
	m_ulEmptyUnit = ulEmptyNo;
	return TRUE;
} //end SetEmptyUnit


ULONG CBinBlk::GetEmptyUnit()
{
	return m_ulEmptyUnit;
} //end GetEmptyUnit


CString CBinBlk::GetStartDate()
{
	return m_szStartDate;
} //end GetStartDate


BOOL CBinBlk::SetStartDate(CString szDate)
{
	m_szStartDate = szDate;
	return TRUE;
}


ULONG CBinBlk::GetAccmulatedCount()
{
	return m_ulAccmulatedCount;
}


BOOL CBinBlk::SetAccmulatedCount(ULONG ulCount)
{
	m_ulAccmulatedCount = ulCount;
	return TRUE;
}


BOOL CBinBlk::SetUserRowCol(ULONG ulDiePerUserRow, ULONG ulDiePerUserCol)
{
	m_ulDiePerUserRow = ulDiePerUserRow;
	m_ulDiePerUserCol = ulDiePerUserCol;
	return TRUE;
}

BOOL CBinBlk::GetUserRowCol(ULONG& ulDiePerUserRow, ULONG& ulDiePerUserCol)
{
	ulDiePerUserRow	= m_ulDiePerUserRow;
	ulDiePerUserCol = m_ulDiePerUserCol;
	return TRUE;
}

BOOL CBinBlk::GetIsTeachWithPhysicalBlk()
{
	return m_bTeachWithPhysicalBlk;
}

VOID CBinBlk::SetTeachWithPhysicalBlk(BOOL bTeachWithPhysicalBlk)
{
	m_bTeachWithPhysicalBlk = bTeachWithPhysicalBlk;
}

BOOL CBinBlk::GetIsCentralizedBondArea()
{
	return m_bCentralizedBondArea;
}


BOOL CBinBlk::SetIsCentralizedBondArea(BOOL bCentralizedBondArea)
{
	m_bCentralizedBondArea = bCentralizedBondArea;

	return TRUE;
}


LONG CBinBlk::GetCentralizedOffsetX()
{
	return m_lCentralizedOffsetX;
}


BOOL CBinBlk::SetCentralizedOffsetX(LONG lCentralizedOffsetX)
{
	m_lCentralizedOffsetX = lCentralizedOffsetX;

	return TRUE;
}


LONG CBinBlk::GetCentralizedOffsetY()
{
	return m_lCentralizedOffsetY;
}


BOOL CBinBlk::SetCentralizedOffsetY(LONG lCentralizedOffsetY)
{
	m_lCentralizedOffsetY = lCentralizedOffsetY;

	return TRUE;
}


BOOL CBinBlk::GetUseCircularArea()
{
	return m_bUseCircularPattern;
}

LONG CBinBlk::GetRadius()
{
	return m_lRadius;
}

LONG CBinBlk::GetCirBinCenterX()
{
	return m_lCenterX;
}

LONG CBinBlk::GetCirBinCenterY()
{
	return m_lCenterY;
}

BOOL CBinBlk::GetEnableBinMapOffset()
{
	return m_bEnableBinMapOffset;
}

LONG CBinBlk::GetBinMapRowOffset()
{
	return m_lBinMapRowOffset;
}

LONG CBinBlk::GetBinMapColOffset()
{
	return m_lBinMapColOffset;
}

VOID CBinBlk::SetUseCircularArea(BOOL bEnable, LONG lRadius)
{
	m_bUseCircularPattern = bEnable;
	if (lRadius > 0)
		m_lRadius = lRadius;
	else
		m_lRadius = 0;
}

VOID CBinBlk::SetCirBinCenterXY(LONG lCX, LONG lCY)
{
	m_lCenterX = lCX;
	m_lCenterY = lCY;
}

VOID CBinBlk::SetBinMapOffset(BOOL bEnable, LONG lRowOffset, LONG lColOffset)
{
	m_bEnableBinMapOffset	= bEnable;
	m_lBinMapRowOffset		= lRowOffset;
	m_lBinMapColOffset		= lColOffset;
}

ULONG CBinBlk::GetBinMapWalkPath()
{
	return m_ulBinMapWalkPath;
}


BOOL CBinBlk::SetBinMapWalkPath(ULONG ulWalkPath)
{
	m_ulBinMapWalkPath = ulWalkPath;
	return TRUE;
}

VOID CBinBlk::SetBinMapCircleRadius(DOUBLE dRadiusInUm)
{
	m_dBinMapCircleRadius = dRadiusInUm;
}

DOUBLE CBinBlk::GetBinMapCircleRadius()
{
	return m_dBinMapCircleRadius;
}

LONG CBinBlk::GetBinMapTopEdge()
{
	return m_lBinMapTopEdge;
}

LONG CBinBlk::GetBinMapBtmEdge()
{
	return m_lBinMapBtmEdge;
}

LONG CBinBlk::GetBinMapLeftEdge()
{
	return m_lBinMapLeftEdge;
}

LONG CBinBlk::GetBinMapRightEdge()
{
	return m_lBinMapRightEdge;
}

VOID CBinBlk::SetBinMapEdgeSize(LONG lTEdge, LONG lBEdge, LONG lLEdge, LONG lREdge)
{
	m_lBinMapTopEdge	= lTEdge;
	m_lBinMapBtmEdge	= lBEdge;
	m_lBinMapLeftEdge	= lLEdge;
	m_lBinMapRightEdge	= lREdge;
}

VOID CBinBlk::GetRowColWithTLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG ulQuotient  = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulDiePerRow;
		}
		else
		{
			if (ulQuotient % 2 == 1)	//last in row
			{
				lBinCol = ulDiePerRow;
			}
			else						//first in row
			{
				lBinCol = 1;
			}
		}
		lBinRow = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulRemainder;
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = ulRemainder;
			}
			else //on even row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
			}
		}
		lBinRow = (ulQuotient + 1);
	}
}


VOID CBinBlk::GetRowColWithTRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG ulQuotient  = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = 1;
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lBinCol = 1;
			}
			else //first in row
			{
				lBinCol = ulDiePerRow;
			}
		}
		lBinRow = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = (ulDiePerRow - ulRemainder + 1);
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
			}
			else //on even row
			{
				lBinCol = ulRemainder;
			}
		}
		lBinRow = (ulQuotient + 1);
	}
}


VOID CBinBlk::GetRowColWithBLH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG ulQuotient  = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulDiePerRow;
		}
		else
		{
			if (ulQuotient % 2 == 1)	//last in row
			{
				lBinCol = ulDiePerRow;
			}
			else						//first in row
			{
				lBinCol = 1;
			}
		}
		//lBinRow = ulQuotient;
		lBinRow = ulDiePerCol - ulQuotient - 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinCol = ulRemainder;
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = ulRemainder;
			}
			else //on even row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
			}
		}
		//lBinRow = (ulQuotient + 1);
		lBinRow = ulDiePerCol - ulQuotient;
	}
}


VOID CBinBlk::GetRowColWithBRH_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG ulQuotient  = ulBinIndex / ulDiePerRow;
	ULONG ulRemainder = ulBinIndex % ulDiePerRow;

	if (ulRemainder == 0) //change row die - either the first or last in row
	{
		if (bNoReturnTravel)		//v4.42T1	//Nichia
		{
			lBinCol = 1;
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lBinCol = 1;
			}
			else //first in row
			{
				lBinCol = ulDiePerRow;
			}
		}
		lBinRow = ulDiePerCol - ulQuotient - 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)		//v4.42T1	//Nichia
		{
			lBinCol = (ulDiePerRow - ulRemainder + 1);
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinCol = (ulDiePerRow - ulRemainder + 1);
			}
			else //on even row
			{
				lBinCol = ulRemainder;
			}
		}
		lBinRow = ulDiePerCol - ulQuotient;
	}
}

VOID CBinBlk::GetRowColWithTLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerCol;
	ULONG ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulDiePerCol;
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = ulDiePerCol;
			}
			else //first in column
			{
				lBinRow = 1;
			}
		}
		lBinCol = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulRemainder;
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinRow = ulRemainder;
			}
			else //on even row
			{
				lBinRow = (ulDiePerCol - ulRemainder + 1);
			}
		}
		lBinCol = ulQuotient + 1;
	}
}


VOID CBinBlk::GetRowColWithTRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG ulQuotient = ulBinIndex / ulDiePerCol;
	ULONG ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulDiePerCol;
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = ulDiePerCol;
			}
			else //first in column
			{
				lBinRow = 1;
			}
		}
		lBinCol = ulDiePerRow - ulQuotient + 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)			//v4.42T1	//Nichia
		{
			lBinRow = ulRemainder;
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinRow = ulRemainder;
			}
			else //on even row
			{
				lBinRow = (ulDiePerCol - ulRemainder + 1);
			}
		}
		lBinCol = ulDiePerRow - ulQuotient;
	}
}


VOID CBinBlk::GetRowColWithBLV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG	ulQuotient  = ulBinIndex / ulDiePerCol;
	ULONG	ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)
		{
			lBinRow = 1;	//ulDiePerCol;
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = 1;
			}
			else //first in column
			{
				lBinRow = ulDiePerCol;
			}
		}
		lBinCol = ulQuotient;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)
		{
			//lBinRow = ulRemainder;
			lBinRow = (ulDiePerCol - ulRemainder + 1);
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				//lBinRow = ulRemainder;
				lBinRow = (ulDiePerCol - ulRemainder + 1);
			}
			else //on even row
			{
				//lBinRow = (ulDiePerCol - ulRemainder + 1);
				lBinRow = ulRemainder;
			}
		}
		lBinCol = ulQuotient + 1;
	}
}

VOID CBinBlk::GetRowColWithBRV_PATH(const ULONG ulBinIndex, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
									LONG &lBinRow, LONG &lBinCol)
{
	ULONG	ulQuotient  = ulBinIndex / ulDiePerCol;
	ULONG	ulRemainder = ulBinIndex % ulDiePerCol;

	if (ulRemainder == 0) //change row die - either the first or last in column
	{
		if (bNoReturnTravel)		//v4.42T1	//Nichia
		{
			lBinRow = 1;
		}
		else
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lBinRow = 1;
			}
			else //first in column
			{
				lBinRow = ulDiePerCol;
			}
		}
		lBinCol = ulDiePerRow - ulQuotient + 1;
	}
	else //die other than the change row one
	{
		if (bNoReturnTravel)		//v4.42T1	//Nichia
		{
			lBinRow = (ulDiePerCol - ulRemainder + 1);
		}
		else
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lBinRow = (ulDiePerCol - ulRemainder + 1);
			}
			else //on even row
			{
				lBinRow = ulRemainder;
			}
		}
		lBinCol = ulDiePerRow - ulQuotient;
	}
}


VOID CBinBlk::GetRowColWithIndexInBinMap(const ULONG ulBinIndex, const ULONG ulWalkPath, const ULONG ulDiePerRow, const ULONG ulDiePerCol, const BOOL bNoReturnTravel,
										 LONG &lBinRow, LONG &lBinCol)
{
	if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
	{
		GetRowColWithTLH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}
	else if (ulWalkPath == BT_TRH_PATH) //TR-Horizontal
	{
		GetRowColWithTRH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}
	else if (ulWalkPath == BT_BLH_PATH) //BL-Horizontal		//5432123
	{
		GetRowColWithBLH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}
	else if (ulWalkPath == BT_BRH_PATH) //BR-Horizontal		//5432123
	{
		GetRowColWithBRH_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}

	else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
	{
		GetRowColWithTLV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}
	else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
	{
		GetRowColWithTRV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}
	else if (ulWalkPath == BT_BLV_PATH)		//BL-Vertical	//5432123
	{
		GetRowColWithBLV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}
	else if (ulWalkPath == BT_BRV_PATH)		//BR-Vertical	//5432123
	{
		GetRowColWithBRV_PATH(ulBinIndex, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);
	}
}


VOID CBinBlk::InitBondedDieInBinMap()
{
	for (LONG i = 0; i < MAX_BINMAP_SIZE_X; i++)
	{
		for (LONG j = 0; j < MAX_BINMAP_SIZE_X; j++)
		{
			m_cBondedDieMap[i][j] = 0;
		}
	}
}


BOOL CBinBlk::IsUseBinMap()
{
	if (m_bEnableBinMap)
	{
		return TRUE;
	}
	if (CMS896AStn::m_oNichiaSubSystem.IsEnabled())	//v4.43T13	//Only for nichia BinMap fcn
	{
		return TRUE;
	}

	if (m_bByPassBinMap)	//v4.48A13
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CBinBlk::CreateBondedDieInBinMap(const ULONG ulGradeCapacity, const ULONG ulLastIndex)
{
	LONG lBinRow = 0, lBinCol = 0;

	ULONG ulBinIndex	= 1;

	ULONG ulDiePerRow	= GetDiePerRow();
	ULONG ulDiePerCol	= GetDiePerCol();
	ULONG ulWalkPath	= GetOriginalWalkPath(); //GetWalkPath();

	InitBondedDieInBinMap();

	if (IsUseBinMap())
	{
		return TRUE;
	}

	BOOL bNoReturnTravel = GetNoReturnTravel();
	BOOL bDieFound = FALSE;

	do
	{
		GetRowColWithIndexInBinMap(ulBinIndex, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);

		if (IsDieInBinMap(lBinRow - 1, lBinCol - 1))
		{
			if (ulBinIndex <= ulLastIndex)
			{
				m_cBondedDieMap[lBinRow - 1][lBinCol - 1] = 1; //bonded
			}
		}

		ulBinIndex++;
	} 
	while ((ulBinIndex <= ulLastIndex) &&  (ulBinIndex <= ulGradeCapacity));

	return TRUE;
}


BOOL CBinBlk::IsBondedDieInBinMap(const ULONG ulRow, const ULONG ulCol)
{
	if ((ulCol >= 0) && (ulCol < MAX_BINMAP_SIZE_X) && 
		(ulRow >= 0) && (ulRow < MAX_BINMAP_SIZE_Y))
	{
		return (m_cBondedDieMap[ulRow][ulCol] == 1);
	}
	
	return FALSE;
}


BOOL CBinBlk::GetNextDieIndexInBinMap(ULONG ulGradeCapacity, ULONG &ulNextIndex, 
									  LONG &lBMapRow, LONG &lBMapCol)
{
	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulBinIndex	= ulNextIndex;

	ULONG ulDiePerRow	= GetDiePerRow();
	ULONG ulDiePerCol	= GetDiePerCol();
	ULONG ulWalkPath	= GetOriginalWalkPath(); //GetWalkPath();

	if (IsUseBinMap())
	{
		return TRUE;
	}

	BOOL bNoReturnTravel = GetNoReturnTravel();

	BOOL bDieFound = FALSE;

	do
	{
		GetRowColWithIndexInBinMap(ulBinIndex, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);

		if (IsDieInBinMap(lBinRow - 1, lBinCol - 1))
		{
			lBMapRow	= lBinRow;
			lBMapCol	= lBinCol;
			bDieFound = TRUE;			//done; target index found
			break;
		}

		ulBinIndex++;

	} 
	while (ulBinIndex <= ulGradeCapacity);

	if (!bDieFound)
	{
		return FALSE;
	}

	ulNextIndex	= ulBinIndex;
	return TRUE;
}

BOOL CBinBlk::GetDieIndexInBinMapWithRowCol(ULONG ulGradeCapacity, const ULONG ulRow, const ULONG ulCol, ULONG &ulIndex)
{
	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulBinIndex	= 1;

	ULONG ulDiePerRow	= GetDiePerRow();
	ULONG ulDiePerCol	= GetDiePerCol();
	ULONG ulWalkPath	= GetOriginalWalkPath();  //GetWalkPath();

	if (IsUseBinMap())
	{
		return TRUE;
	}

	BOOL bNoReturnTravel = GetNoReturnTravel();

	BOOL bDieFound = FALSE;

	do
	{
		GetRowColWithIndexInBinMap(ulBinIndex, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);

		if ((lBinRow - 1 == (LONG)ulRow) && (lBinCol - 1 == (LONG)ulCol))
		{
			bDieFound = TRUE;			//done; target index found
			break;
		}

		ulBinIndex++;

	} 
	while (ulBinIndex <= ulGradeCapacity);

	if (!bDieFound)
	{
		return FALSE;
	}

	ulIndex	= ulBinIndex;
	return TRUE;
}

BOOL CBinBlk::GetDieIndexInBinMap(ULONG ulGradeCapacity, ULONG &ulIndex, 
									  LONG &lBMapRow, LONG &lBMapCol)
{
	LONG lBinRow = 0, lBinCol = 0;
	ULONG ulBinIndex	= ulIndex;

	ULONG ulDiePerRow	= GetDiePerRow();
	ULONG ulDiePerCol	= GetDiePerCol();
	ULONG ulWalkPath	= GetOriginalWalkPath(); //GetWalkPath();

	BOOL bNoReturnTravel = GetNoReturnTravel();
	BOOL bDieFound = FALSE;

	if (IsUseBinMap())	//v4.48A13
	{
		return TRUE;
	}

	GetRowColWithIndexInBinMap(ulBinIndex, ulWalkPath, ulDiePerRow, ulDiePerCol, bNoReturnTravel, lBinRow, lBinCol);

	if (IsDieInBinMap(lBinRow - 1, lBinCol - 1))
	{
		lBMapRow	= lBinRow;
		lBMapCol	= lBinCol;
		bDieFound	= TRUE;			//done; target index found
	}
	else
	{
		bDieFound = FALSE;
	}

	return bDieFound;
}


ULONG CBinBlk::GetLastDieIndexIn1stBMapRow(ULONG &ulNoOfDiesIn1stRow)
{
	if (IsUseBinMap())	//v4.48A13
	{
		return TRUE;
	}

	ULONG ulIndex = 0;
	ULONG ulDieCount = 0;
	INT i = 0;
	ULONG ulWalkPath = GetOriginalWalkPath(); //GetWalkPath();
	CString szLog;

	INT n1stRow = 0;
	INT n1stCol = 0;
	BOOL b1stRowColFound = FALSE;

	//Find first ROW or COLUMN before counting
	if ((ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH))
	{
		for (INT m = 0; m < MAX_BINMAP_SIZE_Y; m++)		//ROW
		{
			for (INT n = 0; n < MAX_BINMAP_SIZE_X; n++)	//COL
			{
				if (IsDieInBinMap(m, n) == TRUE)
				{
					n1stRow = m;
					b1stRowColFound = TRUE;
					break;
				}
			}

			if (b1stRowColFound)	
			{
				break; 
			}		
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TLH) -> 1st row = %d", n1stRow);
		/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (ulWalkPath == BT_TRV_PATH)		//TR-Vertical
	{
		for (INT n = MAX_BINMAP_SIZE_X - 1; n >= 0; n--)	//COL
		{
			for (INT m = 0; m < MAX_BINMAP_SIZE_Y; m++)	//ROW
			{
				if (IsDieInBinMap(m, n) == TRUE)
				{
					n1stCol = n;
					b1stRowColFound = TRUE;
					break;
				}
			}

			if (b1stRowColFound)
			{
				break; 
			}		
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TRV) -> Last col = %d", n1stCol);
		/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (ulWalkPath == BT_BLV_PATH)		//BL-Verticall
	{
		for (INT n = MAX_BINMAP_SIZE_X-1; n >=0; n--)		//COL
		{
			for (INT m = MAX_BINMAP_SIZE_Y-1; m >= 0; m--)	//ROW
			{
				if (IsDieInBinMap(m, n) == TRUE)
				{
					n1stCol = n;
					b1stRowColFound = TRUE;
					break;
				}
			}
			
			if (b1stRowColFound)
			{
				break;
			}		
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TLV) -> 1st col = %d", n1stCol);
		/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else	//TL-Vertical
	{
		for (INT n = 0; n < MAX_BINMAP_SIZE_X; n++)		//COL
		{
			for (INT m = 0; m < MAX_BINMAP_SIZE_Y; m++)	//ROW
			{
				if (IsDieInBinMap(m, n) == TRUE)
				{
					n1stCol = n;
					b1stRowColFound = TRUE;
					break;
				}
			}
			
			if (b1stRowColFound)
			{
				break;
			}		
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TLV) -> 1st col = %d", n1stCol);
		/******/ CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}


	//Then cotun how many dices on this first ROW/COL, as well as the lastDieIndex
	if (ulWalkPath == BT_TLH_PATH)			//TL-Horizontal
	{
		for (i = 0; i < MAX_BINMAP_SIZE_X; i++)
		{
			//if (m_nBinMap[n1stRow][i] == 1)
			if (IsDieInBinMap(n1stRow, i) == TRUE)
			{
				ulDieCount++;
				ulIndex = i + 1;
			}
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TLH) -> die count = %lu, Index = %lu", ulDieCount, ulIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (ulWalkPath == BT_TLV_PATH)		//TL-Vertical
	{
		for (i = 0; i < MAX_BINMAP_SIZE_X; i++)
		{
			//if (m_nBinMap[i][n1stCol] == 1)
			if (IsDieInBinMap(i, n1stCol) == TRUE)
			{
				ulDieCount++;
				ulIndex = i + 1;
			}
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TLV) -> die count = %lu, Index = %lu", ulDieCount, ulIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (ulWalkPath == BT_TRH_PATH)		//TR-Horizontal
	{
		for (i = MAX_BINMAP_SIZE_X - 1; i >= 0; i--)
		{
			//if (m_nBinMap[n1stRow][i] == 1)
			if (IsDieInBinMap(n1stRow, i) == TRUE)
			{
				ulDieCount++;
				ulIndex = i + 1;
			}
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TRH) -> die count = %lu, Index = %lu", ulDieCount, ulIndex);
		/******/ 
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (ulWalkPath == BT_TRV_PATH)		//TR-Vertical
	{
		for (i = 0; i < MAX_BINMAP_SIZE_X; i++)
		{
			//if (m_nBinMap[i][n1stCol] == 1)
			if (IsDieInBinMap(i, n1stCol) == TRUE)
			{
				ulDieCount++;
				ulIndex = i + 1;
			}
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (TRV) -> die count = %lu, Index = %lu", ulDieCount, ulIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}
	else if (ulWalkPath == BT_BLV_PATH)		//BL-Vertical	//5432123
	{
		for (i = MAX_BINMAP_SIZE_X-1; i > 0; i--)
		{
			if (IsDieInBinMap(i, n1stCol) == TRUE)
			{
				ulDieCount++;
				ulIndex = i + 1;
			}
		}

		szLog.Format("BLK: GetLastDieIndexIn1stBMapRow (BLV) -> die count = %lu, Index = %lu", ulDieCount, ulIndex);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+"); /******/ 
	}

	ulNoOfDiesIn1stRow = ulDieCount;
	return ulIndex;
}

BOOL CBinBlk::IsDieInBinMap(CONST LONG lRow, CONST LONG lCol)
{
	CString szMsg;

	if ((lRow < 0) || (lRow >= MAX_BINMAP_SIZE_X))
	{
		return FALSE;
	}
	if ((lCol < 0) || (lCol >= MAX_BINMAP_SIZE_Y))
	{
		return FALSE;
	}
/*
	if (GetBinMapCircleRadius() > 0)
	{
		LONG lCDieRow	= m_lBinMapRowOffset;
		LONG lCDieCol	= m_lBinMapColOffset;

		DOUBLE dRadiusInUm = GetBinMapCircleRadius();
		LONG lRadiusX		= (LONG) (dRadiusInUm / GetDDiePitchX());		//Radius in X die steps
		LONG lRadiusY		= (LONG) (dRadiusInUm / GetDDiePitchY());		//Radius in Y die steps

		if (IsWithinOvalBinMapLimit((DOUBLE)lCDieCol,	(DOUBLE)lCDieRow, 
									(DOUBLE)lCol,		(DOUBLE)lRow, 
									lRadiusX,			lRadiusY) )
		{
			if (GetBinMapLeftEdge() > 0)
			{
				LONG lStartCol = GetBinMapLeftEdge();
				if (lCol < lStartCol)
					return FALSE;
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
*/
	return TRUE;
}

BOOL CBinBlk::IsWithinOvalBinMapLimit(DOUBLE dCX, DOUBLE dCY, DOUBLE dMapX, DOUBLE dMapY, DOUBLE dRadiusX, DOUBLE dRadiusY)
{
	BOOL bStatus = TRUE;

	// Is within Oval limit:
	//
	//	sq(x)/sq(a) + sq(y)/sq(b) <= 1
	//

	DOUBLE dLengthX = pow(dMapX + 1 - dCX, 2.0);
	DOUBLE dLengthY = pow(dMapY + 1 - dCY, 2.0);
	DOUBLE dA		= pow(dRadiusX, 2.0);
	DOUBLE dB		= pow(dRadiusY, 2.0);

	DOUBLE dResult = dLengthX / dA + dLengthY / dB;

	if (dResult >= 1)		//Nichia//v4.43T7
		bStatus = FALSE;
	return bStatus;
}

BOOL CBinBlk::FindBinMapTEdge1stRow(ULONG ulDicesOnRow, LONG &lStartRow)
{
	BOOL bStatus = TRUE;
	ULONG ulNextIndex = 1;
	LONG lBinRow = 0;
	LONG lBinCol = 0;

	GetNextDieIndexInBinMap(m_ulDiePerBlk, ulNextIndex, lBinRow, lBinCol);
	LONG lCurrRow = lBinRow;
	LONG lTotalRowIncrement = 1;
	LONG lRowDieCount = 1;

	do
	{
		ulNextIndex = ulNextIndex + 1;
		bStatus = GetNextDieIndexInBinMap(m_ulDiePerBlk, ulNextIndex, lBinRow, lBinCol);
		if (!bStatus)
			return FALSE;
		if (lBinRow == lCurrRow)		//If still current row, increment RowDieCount 
		{
			lRowDieCount++;
		}
		else if (lBinRow > lCurrRow)	//If next row, then reset RowDieCount
		{
			lRowDieCount = 1;
			lCurrRow = lBinRow;
			lTotalRowIncrement++;
		}

		if (lTotalRowIncrement > 10)
			break;

	} while (lRowDieCount < (LONG)ulDicesOnRow);


	if (lRowDieCount < (LONG)ulDicesOnRow)
	{
		return FALSE;
	}

	lStartRow = lCurrRow;
	return TRUE;
}

BOOL CBinBlk::FindBinMapLEdge1stCol(ULONG ulDicesOnCol, LONG &lStartCol)
{
	BOOL bStatus = TRUE;
	ULONG ulNextIndex = 1;
	LONG lBinRow = 0;
	LONG lBinCol = 0;
	CString szTemp;

	SetBinMapEdgeSize(0,0,0,0);

	if (ulDicesOnCol == 0)
	{
		lStartCol = 0;
		return TRUE;
	}

	bStatus = GetNextDieIndexInBinMap(m_ulDiePerBlk, ulNextIndex, lBinRow, lBinCol);
	if (!bStatus)
	{
AfxMessageBox("FindBinMapTEdge1stRow fail on 1st die", MB_SYSTEMMODAL);
		return FALSE;
	}
	
	//szTemp.Format("1st col at = %ld", lBinCol);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	LONG lCurrCol = lBinCol;
	LONG lLastCol = lBinCol;
	LONG lTotalColIncrement = 1;
	LONG lLastColDieCount = 1;
	LONG lColDieCount = 1;

	do
	{
		ulNextIndex = ulNextIndex + 1;
		bStatus = GetNextDieIndexInBinMap(m_ulDiePerBlk, ulNextIndex, lBinRow, lBinCol);
		if (!bStatus)
			return FALSE;

		if (lBinCol == lCurrCol)		//If still current row, increment RowDieCount 
		{
			lColDieCount++;
		}
		else if (lBinCol > lCurrCol)	//If next row, then reset RowDieCount
		{
			//szTemp.Format("Change col from %ld to %ld, row count = %ld", 
			//	lCurrCol, lBinCol, lColDieCount);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

			if (lColDieCount >= (LONG)ulDicesOnCol)
			{
				break;
			}

			lLastColDieCount = lColDieCount;
			lLastCol = lCurrCol;
			lColDieCount = 1;
			lCurrCol = lBinCol;
			lTotalColIncrement++;
		}

		//if (lTotalColIncrement > 10)
		//	break;

	} while (lTotalColIncrement < (LONG)GetDiePerRow());


	if (lColDieCount < (LONG)ulDicesOnCol)
	{
		szTemp.Format("Error: FindBinMapLEdge1stCol: LeftEdge col cannot be found - %lu", ulDicesOnCol);
AfxMessageBox(szTemp, MB_SYSTEMMODAL);
		return FALSE;
	}


	LONG lUpCountDiff	= lColDieCount - ulDicesOnCol;
	LONG lLowCountDiff	= ulDicesOnCol - lLastColDieCount;
	BOOL bUseLowCount = FALSE;
	if (lLowCountDiff <= lUpCountDiff)
		bUseLowCount = TRUE;

	//szTemp.Format("last Col %ld count = %ld; Col-diff = %ld", 
	//					lLastCol,	lLastColDieCount,	lLowCountDiff);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	//szTemp.Format("Curr Col %d count = %ld;  Row-diff = %ld", 
	//					lCurrCol,	lColDieCount,		lUpCountDiff);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	

	if (lCurrCol > 1)
		lCurrCol = lCurrCol - 1;
	lStartCol = lCurrCol;
	if (bUseLowCount && (lStartCol > 1))
	{
		lStartCol = lStartCol - 1;
	}

	//szTemp.Format("Dice on col = %lu;  UseLowCount = %d; StartCol = %ld", 
	//	ulDicesOnCol, bUseLowCount, lStartCol);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	return TRUE;
}

BOOL CBinBlk::GetIsUseBlockCornerAsFirstDiePos()
{
	return m_bUseBlockCornerAsFirstDiePos;
}

BOOL CBinBlk::SetIsUseBlockCornerAsFirstDiePos(BOOL bUseBlockCornerAsFirstDiePos)
{
	m_bUseBlockCornerAsFirstDiePos = bUseBlockCornerAsFirstDiePos;
	return TRUE;
}

BOOL CBinBlk::GetNoReturnTravel() CONST
{
	return m_bNoReturnTravel;
}

VOID CBinBlk::SetNoReturnTravel(CONST BOOL bEnable)
{
	m_bNoReturnTravel = bEnable;
}

VOID CBinBlk::SetEnableWafflePad(BOOL bEnable)
{
	m_bEnableWafflePad = bEnable;
}

BOOL CBinBlk::GetIsEnableWafflePad()
{
	return m_bEnableWafflePad;
}

VOID CBinBlk::SetWafflePadSizeX(LONG lSizeX)
{
	m_lWafflePadSizeX = lSizeX;
}

LONG CBinBlk::GetWafflePadSizeX()
{
	return m_lWafflePadSizeX;
}

VOID CBinBlk::SetWafflePadSizeY(LONG lSizeY)
{
	m_lWafflePadSizeY = lSizeY;
}

LONG CBinBlk::GetWafflePadSizeY()
{
	return m_lWafflePadSizeY;
}

VOID CBinBlk::SetWafflePadDistX(LONG lDistX)
{
	m_lWafflePadDistX = lDistX;
}

LONG CBinBlk::GetWafflePadDistX()
{
	return m_lWafflePadDistX;
}

VOID CBinBlk::SetWafflePadDistY(LONG lDistY)
{
	m_lWafflePadDistY = lDistY;
}

LONG CBinBlk::GetWafflePadDistY()
{
	return m_lWafflePadDistY;
}


/********************************************/
/*        Bin Blocks Setup functions        */   
/********************************************/
BOOL CBinBlk::CheckBlockSize(LONG lBondXOffset, LONG lBondYOffset)
{
	ULONG ulDiePerRow = 0;
	ULONG ulDiePerCol = 0;

	if (GetIsUseBlockCornerAsFirstDiePos() == TRUE)
	{
		lBondXOffset = 0;
		lBondYOffset = 0;
	}

	if (GetDDiePitchX() == 0 ||  GetDDiePitchY() == 0)
		return FALSE;

	if ((LONG)m_ulWidth <= (lBondXOffset * 2))
		ulDiePerRow = 0;

	if (GetDDiePitchX() != 0)
	{
		//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
		DOUBLE dWidth		= m_ulWidth - (lBondXOffset * 2);
		DOUBLE dDiePitchX	= GetDDiePitchX();
		if (GetPoint5UmInDiePitchX())
			dDiePitchX = dDiePitchX + 0.5;
		ulDiePerRow			= (ULONG) (dWidth / dDiePitchX + 1.0);
	}

	if (ulDiePerRow < m_ulDiePerUserRow)
	{
		return FALSE;
	}

	if ((LONG)m_ulHeight <= (lBondYOffset * 2))
		ulDiePerCol = 0;

	if (GetDDiePitchY() != 0)
	{
		//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
		DOUBLE dHeight		= m_ulHeight - (lBondYOffset * 2);
		DOUBLE dDiePitchY	= GetDDiePitchY();
		if (GetPoint5UmInDiePitchY())
			dDiePitchY = dDiePitchY + 0.5;
		ulDiePerCol			= (ULONG) (dHeight / dDiePitchY + 1.0);
	}

	if (ulDiePerCol < m_ulDiePerUserCol)
	{
		return FALSE;
	}

	return TRUE;
}

ULONG CBinBlk::CalulateAcutalDiePerBlk(LONG lBondXOffset, LONG lBondYOffset)
{
	ULONG ulDiePerRow = 0;
	ULONG ulDiePerCol = 0;

	if (GetIsUseBlockCornerAsFirstDiePos() == TRUE)
	{
		lBondXOffset = 0;
		lBondYOffset = 0;
	}

	if ((LONG)m_ulWidth <= (lBondXOffset * 2))
		ulDiePerRow = 0;

	if (GetDDiePitchX() != 0)
	{
		//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
		DOUBLE dWidth		= m_ulWidth - (lBondXOffset * 2);
		DOUBLE dDiePitchX	= GetDDiePitchX();
		if (GetPoint5UmInDiePitchX())
			dDiePitchX = dDiePitchX + 0.5;
		ulDiePerRow = (ULONG) (dWidth / dDiePitchX + 1.0);
	}


	if ((LONG)m_ulHeight <= (lBondYOffset * 2))
		ulDiePerCol = 0;

	if (GetDDiePitchY() != 0)
	{
		//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
		DOUBLE dHeight		= m_ulHeight - (lBondYOffset * 2);
		DOUBLE dDiePitchY	= GetDDiePitchY();
		if (GetPoint5UmInDiePitchY())
			dDiePitchY = dDiePitchY + 0.5;
		ulDiePerCol			= (ULONG) (dHeight / dDiePitchY + 1.0);
	}

	return ulDiePerRow * ulDiePerCol;
}

ULONG CBinBlk::CalculateDiePerRow(DOUBLE dBondXOffset)
{
	ULONG ulDiePerRow;

	if (GetIsUseBlockCornerAsFirstDiePos() == TRUE)
	{
		dBondXOffset = 0;
	}

	if ((DOUBLE)m_ulWidth <= dBondXOffset * 2)
	{
		ulDiePerRow = 0;
	}

	if (fabs(GetDDiePitchX() - 0.00000001) > 0)
	{
		//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
		DOUBLE dWidth		= m_ulWidth - dBondXOffset * 2;
		DOUBLE dDiePitchX	= GetDDiePitchX();
		if (GetPoint5UmInDiePitchX())
		{
			dDiePitchX = dDiePitchX + 0.5;
		}
		ulDiePerRow			= (ULONG) (dWidth / dDiePitchX + 1.0);
	}
	else
	{
		ulDiePerRow = 0;
	}

	if (ulDiePerRow > m_ulDiePerUserRow && m_ulDiePerUserRow != 0)
	{
		ulDiePerRow = m_ulDiePerUserRow;
	}

	return ulDiePerRow;
} //end CalculateDiePerRow


ULONG CBinBlk::CalculateDiePerCol(DOUBLE dBondYOffset)
{
	ULONG ulDiePerCol;

	if (GetIsUseBlockCornerAsFirstDiePos() == TRUE)
	{
		dBondYOffset = 0;
	}

	if ((DOUBLE)m_ulHeight <= dBondYOffset * 2)
	{
		ulDiePerCol = 0;
	}

	if (fabs(GetDDiePitchY() - 0.00000001) > 0)
	{
		//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
		DOUBLE dHeight		= m_ulHeight - dBondYOffset * 2;
		DOUBLE dDiePitchY	= GetDDiePitchY();
		if (GetPoint5UmInDiePitchY())
		{
			dDiePitchY = dDiePitchY + 0.5;
		}
		ulDiePerCol			= (ULONG) (dHeight / dDiePitchY + 1.0);

//CString szLog;
//szLog.Format("CalculateDiePerCol  %lu - Heigth=%.1f, PitchY=%.1f", ulDiePerCol, dHeight, dDiePitchY);
//AfxMessageBox(szLog, MB_SYSTEMMODAL);
	}
	else
	{
		ulDiePerCol = 0;
	}

	if (ulDiePerCol > m_ulDiePerUserCol && m_ulDiePerUserCol != 0)
	{
		ulDiePerCol = m_ulDiePerUserCol;
	}

	return ulDiePerCol;
} //end CalculateDiePerCol


ULONG CBinBlk::CalculateDiePerBlk()
{
	ULONG ulDiePerBlk;

	if (m_ulDiePerRow == 0)
		ulDiePerBlk = m_ulDiePerCol;
	else if (m_ulDiePerCol == 0)
		ulDiePerBlk = m_ulDiePerRow;
	else
		ulDiePerBlk = m_ulDiePerCol * m_ulDiePerRow;

	if (m_bUseCircularPattern)		//v3.71T10
		return CalculateDiePerCircularBlk();

	return ulDiePerBlk;
} //end CalculateDiePerBlk

BOOL CBinBlk::CalculateCentralizedBondArea()
{
	//v4.42T5
	DOUBLE dDiePitchX = GetDDiePitchX();
	if (GetPoint5UmInDiePitchX())
		dDiePitchX = dDiePitchX + 0.5;
	DOUBLE dDiePitchY = GetDDiePitchY();
	if (GetPoint5UmInDiePitchY())
		dDiePitchY = dDiePitchY + 0.5;
	DOUBLE dBondAreaWidth  = m_ulDiePerRow * dDiePitchX;
	DOUBLE dBondAreaHeight = m_ulDiePerCol * dDiePitchY;


	LONG lBondXOffset=0, lBondYOffset=0;

	if (GetIsUseBlockCornerAsFirstDiePos() == TRUE)
	{
		lBondXOffset = 0;
		lBondYOffset = 0;
	}

	//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
	DOUBLE dWidth		= m_ulWidth - (lBondXOffset * 2);
	ULONG ulDiePerRow	= (ULONG) (dWidth / dDiePitchX + 1.0);

	if ( m_ulDiePerRow == ulDiePerRow )
	{
		m_lCentralizedOffsetX = 0;
	}
	else
	{
		m_lCentralizedOffsetX = _round((m_ulWidth - dBondAreaWidth) / 2);
	}

	//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
	DOUBLE dHeight		= m_ulHeight - (lBondYOffset * 2);
	//DOUBLE dDiePitchY	= m_dDiePitchY;
	ULONG ulDiePerCol	= (ULONG) (dHeight / dDiePitchY + 1.0);
	
	if ( m_ulDiePerCol == ulDiePerCol )
	{
		m_lCentralizedOffsetY = 0;
	}
	else
	{
		m_lCentralizedOffsetY = _round((m_ulHeight - dBondAreaHeight) / 2);
	}

	return TRUE;
} //end CalculateCentralizedBondArea

BOOL  CBinBlk::CalculateCentralizedBondAreaFromPhyLimit(LONG lPhyULX, LONG lPhyULY, 
														LONG lPhyLRX, LONG lPhyLRY)
{
	DOUBLE dDiePitchX = GetDDiePitchX();
	if (GetPoint5UmInDiePitchX())
		dDiePitchX = dDiePitchX + 0.5;
	DOUBLE dDiePitchY = GetDDiePitchY();
	if (GetPoint5UmInDiePitchY())
		dDiePitchY = dDiePitchY + 0.5;
	DOUBLE dBondAreaWidth  = m_ulDiePerRow * dDiePitchX;
	DOUBLE dBondAreaHeight = m_ulDiePerCol * dDiePitchY;
	
	LONG lBondXOffset=0, lBondYOffset=0;
	if (GetIsUseBlockCornerAsFirstDiePos() == TRUE)
	{
		lBondXOffset = 0;
		lBondYOffset = 0;
	}

	//v4.42T4	//Change precision from LONG to DOUBLE	//Nichia
	ULONG ulWidth		= labs(lPhyLRX - lPhyULX);
	DOUBLE dWidth		= ulWidth - (lBondXOffset * 2);
	ULONG ulDiePerRow	= (ULONG) (dWidth / dDiePitchX + 1.0);
	CString szTemp;

	if ( m_ulDiePerRow == ulDiePerRow )
	{
		m_lCentralizedOffsetX = 0;
//szTemp.Format("a: %lu, %lu", m_ulDiePerRow, ulDiePerRow);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	}
	else
	{
		m_lCentralizedOffsetX = _round((ulWidth - dBondAreaWidth) / 2);
//szTemp.Format("b: %ld, l(%ld, %ld) p(%ld, %ld)", m_lCentralizedOffsetX, m_lUpperLeftX, m_lLowerRightX, lPhyULX, lPhyLRX);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	}

	ULONG ulHeight		= lPhyLRY - lPhyULY;
	DOUBLE dHeight		= ulHeight - (lBondYOffset * 2);
	ULONG ulDiePerCol	= (ULONG) (dHeight / dDiePitchY + 1.0);
	
	if ( m_ulDiePerCol == ulDiePerCol )
	{
		m_lCentralizedOffsetY = 0;
//szTemp.Format("c: %lu, %lu", m_ulDiePerCol, ulDiePerCol);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	}
	else
	{
		m_lCentralizedOffsetY = _round((ulHeight - dBondAreaHeight) / 2);
//szTemp.Format("D: %ld, L(%ld, %ld) p(%ld, %ld)", m_lCentralizedOffsetY, m_lUpperLeftY, m_lLowerRightY, lPhyULY, lPhyLRY);
//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	}

	return TRUE;
}


/******************************************/
/*              Step Move                 */
/******************************************/
BOOL CBinBlk::FindBinTableXYPosnGivenIndex(ULONG ulIndex, DOUBLE& dXPosn, DOUBLE &dYPosn, BOOL bLog)
{
	//LONG lX=0, lY=0;
	ULONG ulQuotient=0, ulRemainder=0;		//Klocwork
	ULONG ulBinIndex = 1;
	//v4.42T5
	DOUBLE dX=0, dY=0;
	DOUBLE dDiePitchX = GetDDiePitchX();
	DOUBLE dDiePitchY = GetDDiePitchY();
	//v4.42T9
	DOUBLE dCurrBondOffsetX = 0;
	DOUBLE dCurrBondOffsetY = 0;
	CString szLog;


	if (GetPoint5UmInDiePitchX())
	{
		dDiePitchX = dDiePitchX + 0.5;
	}

	if (GetPoint5UmInDiePitchY())
	{
		dDiePitchY = dDiePitchY + 0.5;
	}

	ulBinIndex = ulIndex + m_ulSkipUnit;


	//v3.71T10
	//Support of new ciruclar Bin sorting for PLLM REBEL
	if (m_bUseCircularPattern)		//v3.71T10
	{
		LONG lX = (LONG) dXPosn;
		LONG lY = (LONG) dYPosn;
		BOOL bStatus = FindBinTableXYCirPosnGivenIndex(ulIndex, lX, lY);
		dXPosn = lX;
		dYPosn = lY;
		return bStatus;
	}

	if (m_bEnableWafflePad)
	{
		//v4.59A19	//Changed from LONG to DOUBLE for MS90 0.1um encoder
		BOOL bStatus = FindBinTableXYWafflePadPosnGivenIndex(ulIndex, dXPosn, dYPosn);		
		return bStatus;	
	}

	//v4.49A9
	ULONG ulOrigPath = GetOriginalWalkPath();
	ULONG ulWalkPath = GetWalkPath();
	LONG lULX = m_lUpperLeftX;
	LONG lULY = m_lUpperLeftY;
	LONG lLRX = m_lLowerRightX;
	LONG lLRY = m_lLowerRightY;
	RotateBlkCorners(lULX, lULY, lLRX, lLRY);

	BOOL bRotate180 = FALSE;
	if (m_ucFrameRotation == BBX_FRAME_ROTATE_180)
	{
		bRotate180 = TRUE;
	}

	//v4.57A10
	if (bLog && bRotate180)
	{
		szLog.Format("BBX: FindBinTableXYPos (in um) - (%ld, %ld, %ld, %ld) (%lu); 180 rotate-> (%ld, %ld, %ld, %ld) (%lu); COR (%ld, %ld)", 
			m_lUpperLeftX, m_lUpperLeftY, m_lLowerRightX, m_lLowerRightY, ulOrigPath,
			lULX, lULY, lLRX, lLRY, ulWalkPath,
			m_lBinCalibX, m_lBinCalibY);
		//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
		CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
	}


	//Calculations below
	if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
	{
		if( m_ulDiePerRow!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerRow;
			ulRemainder = ulBinIndex % m_ulDiePerRow;
		}

		//v4.42T9
		UCHAR ucDir = 0;
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulQuotient, m_ulDiePerCol);
		}
		if (m_dBondAreaOffsetY != 0)
		{
			if (ulRemainder == 0)
			{
				if (m_bNoReturnTravel)
					ucDir = 2;		//RIGHT
				else if (ulQuotient % 2 == 1)
					ucDir = 2;		//RIGHT
				else
					ucDir = 1;		//LEFT
			}
			else
			{
				if (m_bNoReturnTravel)
					ucDir = 2;		//RIGHT
				else if (ulQuotient % 2 == 0)	//on odd row
					ucDir = 2;		//RIGHT
				else
					ucDir = 1;		//LEFT
			}
			dCurrBondOffsetY = CalculateCurrBondOffsetY(ulRemainder, m_ulDiePerRow, ucDir);
		}

		if ((dCurrBondOffsetX != 0) || (dCurrBondOffsetY != 0))
		{
CString szLog;
szLog.Format("BLK: TLH - BondOffset XY : %f um, %f um (%ld, %ld, %d)", 
				dCurrBondOffsetX, 
				dCurrBondOffsetY, ulRemainder, m_ulDiePerRow, ucDir);
CMSLogFileUtility::Instance()->BT_TableIndexLog(szLog);
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dX = dDiePitchX * m_ulDiePerRow + lULX + dCurrBondOffsetX;
				//Always from LEFT to RIGHT when m_bNoReturnTravel is set
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1)	//last in row
				{
					dX = dDiePitchX * m_ulDiePerRow + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in row
				{
					dX = dDiePitchX + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dY = dDiePitchY * ulQuotient + lULY + dCurrBondOffsetY;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dX = dDiePitchX * ulRemainder + lULX + dCurrBondOffsetX;
				//Always from LEFT to RIGHT when m_bNoReturnTravel is set
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN

				if (ulRemainder == 1)		//andrewng1
				{
					m_ucSortDirection = 0;
				}
			}
			else
			{
				if (ulQuotient % 2 == 0)		//on odd row
				{
					dX = dDiePitchX * ulRemainder + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dX =  dDiePitchX * (m_ulDiePerRow - ulRemainder + 1) + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}

				if (ulRemainder == 1)		//andrewng1
				{
					m_ucSortDirection	= 4;
				}
			}
			dY = dDiePitchY * (ulQuotient + 1) + lULY + dCurrBondOffsetY;
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dX = dX - dDiePitchX;

			if (ulBinIndex != 0)
			{
				if (bRotate180)		//v4.59A10
					dY = dY + dDiePitchY;
				else
					dY = dY - dDiePitchY;
			}

			CString szMsg;
			szMsg.Format("Andrew: %f, %f (%f, %f)",  dX, dY, dDiePitchX, dDiePitchY);
//CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szMsg, "a+");	//v4.59A20

		}
	}
	else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
	{
		if( m_ulDiePerCol!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerCol;
			ulRemainder = ulBinIndex % m_ulDiePerCol;
		}

		//v4.42T9
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulRemainder, m_ulDiePerCol);
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = dDiePitchY * m_ulDiePerCol + lULY;
				//Always from TOP to BOTTOM when m_bNoReturnTravel is set
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = dDiePitchY * m_ulDiePerCol + lULY;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in column
				{
					dY = dDiePitchY + lULY;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dX = dDiePitchX * ulQuotient + lULX + dCurrBondOffsetX;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = dDiePitchY * ulRemainder + lULY;
				//Always from TOP to BOTTOM when m_bNoReturnTravel is set
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = dDiePitchY * ulRemainder + lULY;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dY = dDiePitchY * (m_ulDiePerCol - ulRemainder + 1) + lULY;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dX = dDiePitchX * (ulQuotient + 1) + lULX + dCurrBondOffsetX;
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dY = dY - dDiePitchY;
			if (ulBinIndex != 0)
			{
				dX = dX - dDiePitchX;
			}
		}
	}
	else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
	{
		if( m_ulDiePerRow!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerRow;
			ulRemainder = ulBinIndex % m_ulDiePerRow;
		}

		//v4.42T9
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulQuotient, m_ulDiePerCol);
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dX = dDiePitchX + lULX + dCurrBondOffsetX;
				//Always from RIGHT to LEFT when m_bNoReturnTravel is set
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					dX = dDiePitchX + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in row
				{
					dX = dDiePitchX * m_ulDiePerRow + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dY = dDiePitchY * ulQuotient + lULY;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dX = dDiePitchX * (m_ulDiePerRow - ulRemainder + 1) + lULX + dCurrBondOffsetX;
				//Always from RIGHT to LEFT when m_bNoReturnTravel is set
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dX = dDiePitchX * (m_ulDiePerRow - ulRemainder + 1) + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dX = dDiePitchX * ulRemainder + lULX + dCurrBondOffsetX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dY = dDiePitchY * (ulQuotient + 1) + lULY;
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dX = dX + dDiePitchX;
			if (ulBinIndex != 0)
			{
				if (bRotate180)		//v4.59A10
					dY = dY + dDiePitchY;
				else
					dY = dY - dDiePitchY;
			}
		}
	}
	else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
	{
		if( m_ulDiePerCol!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerCol;
			ulRemainder = ulBinIndex % m_ulDiePerCol;
		}

		//v4.42T9
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulRemainder, m_ulDiePerCol);
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = lULY + dDiePitchY * m_ulDiePerCol;
				//Always from TOP to BOTTOM when m_bNoReturnTravel is set
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lULY + dDiePitchY * m_ulDiePerCol;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in column
				{
					dY = lULY + dDiePitchY;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient + 1) + dCurrBondOffsetX;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = lULY + dDiePitchY * ulRemainder;
				//Always from TOP to BOTTOM when m_bNoReturnTravel is set
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lULY + dDiePitchY * ulRemainder;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dY = lULY + dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient) + dCurrBondOffsetX;
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dY = dY - dDiePitchY;
			if (ulBinIndex != 0)
			{
				dX = dX + dDiePitchX;
			}
		}
	}
	else if (ulWalkPath == BT_BLH_PATH) //BL-Horizontal
	{
		if( m_ulDiePerRow!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerRow;
			ulRemainder = ulBinIndex % m_ulDiePerRow;
		}

		//v4.42T9
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulQuotient, m_ulDiePerCol);
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				if (bRotate180)			//v4.57A11
					dX = lLRX + dCurrBondOffsetX;
				else
					dX = lULX + dDiePitchX * m_ulDiePerRow + dCurrBondOffsetX;
				
				//Always from LEFT to RIGHT when m_bNoReturnTravel is set
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					if (bRotate180)			//v4.57A11
						dX = lLRX + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX * m_ulDiePerRow + dCurrBondOffsetX;

					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in row
				{
					if (bRotate180)			//v4.57A11
						dX = lLRX - dDiePitchX * (m_ulDiePerRow - 1) + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX + dCurrBondOffsetX;

					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}

			dY = lLRY - dDiePitchY * ulQuotient;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				if (bRotate180)				//v4.57A11
					dX = lLRX - dDiePitchX * (m_ulDiePerRow - ulRemainder) + dCurrBondOffsetX;
				else
					dX = lULX + dDiePitchX * ulRemainder + dCurrBondOffsetX;
				//Always from LEFT to RIGHT when m_bNoReturnTravel is set
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					if (bRotate180)			//v4.57A11
						dX = lLRX - dDiePitchX * (m_ulDiePerRow - ulRemainder) + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX * ulRemainder + dCurrBondOffsetX;
					
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					if (bRotate180)			//v4.57A11
						dX = lLRX - dDiePitchX * (ulRemainder - 1) + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1) + dCurrBondOffsetX;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}

			dY =  lLRY - dDiePitchY * (ulQuotient + 1);
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dX = dX - dDiePitchX;
			if (ulBinIndex != 0)
			{
				if (bRotate180)		//v4.59A10
					dY = dY + dDiePitchY;
				else
					dY = dY - dDiePitchY;
			}
		}
	}
	else if (ulWalkPath == BT_BLV_PATH) //BL-Vertical
	{
		if( m_ulDiePerCol!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerCol;
			ulRemainder = ulBinIndex % m_ulDiePerCol;
		}

		//v4.42T9
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulRemainder, m_ulDiePerCol);
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = lLRY - dDiePitchY * m_ulDiePerCol;
				//Always from BOTTOM to TOP when m_bNoReturnTravel is set
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lLRY - dDiePitchY * m_ulDiePerCol;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in column
				{
					dY = lLRY - dDiePitchY;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}

			if (bRotate180)			//v4.57A11
				dX = lLRX - dDiePitchX * (m_ulDiePerRow - ulQuotient) + dCurrBondOffsetX;
			else
				dX = lULX + dDiePitchX * ulQuotient + dCurrBondOffsetX;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = lLRY - dDiePitchY * ulRemainder;
				//Always from BOTTOM to TOP when m_bNoReturnTravel is set
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lLRY - dDiePitchY * ulRemainder;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dY = lLRY - dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}

			if (bRotate180)			//v4.57A11
				dX = lLRX - dDiePitchX * (m_ulDiePerRow - ulQuotient - 1) + dCurrBondOffsetX;
			else
				dX = lULX + dDiePitchX * (ulQuotient + 1) + dCurrBondOffsetX;
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dY = dY - dDiePitchY;
			if (ulBinIndex != 0)
			{
				dX = dX - dDiePitchX;
			}
		}
	}
	else if (ulWalkPath == BT_BRH_PATH)		//BR-Horizontal
	{
		if( m_ulDiePerRow!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerRow;
			ulRemainder = ulBinIndex % m_ulDiePerRow;
		}

		//v4.42T9
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulQuotient, m_ulDiePerCol);
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				if (bRotate180)			//v4.49A10
					dX = lLRX - dDiePitchX * m_ulDiePerRow + dCurrBondOffsetX;
				else
					dX = lULX + dDiePitchX + dCurrBondOffsetX;
				//Always from RIGHT to LEFT when m_bNoReturnTravel is set
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					if (bRotate180)			//v4.49A10
						dX = lLRX - dDiePitchX * m_ulDiePerRow + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX + dCurrBondOffsetX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in row
				{
					if (bRotate180)			//v4.49A10
						dX = lLRX - dDiePitchX + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX * m_ulDiePerRow + dCurrBondOffsetX;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dY = lLRY - dDiePitchY * ulQuotient;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				if (bRotate180)			//v4.49A10
					dX = lLRX - dDiePitchX * ulRemainder + dCurrBondOffsetX;
				else
					dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1) + dCurrBondOffsetX;
				//Always from RIGHT to LEFT when m_bNoReturnTravel is set
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					if (bRotate180)			//v4.49A10
						dX = lLRX - dDiePitchX * ulRemainder + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1) + dCurrBondOffsetX;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					if (bRotate180)			//v4.49A10
						dX = lLRX - dDiePitchX * (m_ulDiePerRow - ulRemainder + 1) + dCurrBondOffsetX;
					else
						dX = lULX + dDiePitchX * ulRemainder + dCurrBondOffsetX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			dY =  lLRY - dDiePitchY * (ulQuotient + 1);
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dX = dX + dDiePitchX;
			if (ulBinIndex != 0)
			{
				if (bRotate180)		//v4.59A10
					dY = dY + dDiePitchY;
				else
					dY = dY - dDiePitchY;
			}

		}
	}
	else if (ulWalkPath == BT_BRV_PATH)		//BR-Vertical
	{
		if( m_ulDiePerCol!=0 )	// divide by zero
		{
			ulQuotient = ulBinIndex / m_ulDiePerCol;
			ulRemainder = ulBinIndex % m_ulDiePerCol;
		}

		//v4.42T9
		if (m_dBondAreaOffsetX != 0)
		{
			dCurrBondOffsetX = CalculateCurrBondOffsetX(ulRemainder, m_ulDiePerCol);
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = lLRY - dDiePitchY * m_ulDiePerCol;
				//Always from BOTTOM to TOP when m_bNoReturnTravel is set
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lLRY - dDiePitchY * m_ulDiePerCol;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in column
				{
					dY = lLRY + dDiePitchY;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			//Nichia//v4.43T7
			//dX = m_lLowerRightX - dDiePitchX * (m_ulDiePerRow - ulQuotient + 1) + dCurrBondOffsetX;
			dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient + 1) + dCurrBondOffsetX;
		}
		else //die other than the change row one
		{
			if (m_bNoReturnTravel)	//v4.42T1	//Nichia
			{
				dY = lLRY - dDiePitchY * ulRemainder;
				//Always from BOTTOM to TOP when m_bNoReturnTravel is set
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lLRY - dDiePitchY * ulRemainder;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dY = lLRY - dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
			}
			//Nichia//v4.43T7
			//dX = m_lLowerRightX - dDiePitchX * (m_ulDiePerRow - ulQuotient) + dCurrBondOffsetX;
			dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient) + dCurrBondOffsetX;
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dY = dY - dDiePitchY;
			if (ulBinIndex != 0)
			{
				dX = dX + dDiePitchX;
			}
		}
	}

	if (m_bCentralizedBondArea)
	{
		//v4.50A24	
		LONG lCentralizedOffsetX = m_lCentralizedOffsetX;
		LONG lCentralizedOffsetY = m_lCentralizedOffsetY;

		if (bRotate180 == TRUE)	//MS90 BT frame rotation
		{
			//FrameRotateTransformPosXY(lCentralizedOffsetX, lCentralizedOffsetY);
			//v4.50A25
			dXPosn = (LONG) (dX - lCentralizedOffsetX);
			dYPosn = (LONG) (dY - lCentralizedOffsetY);
		}
		else
		{
			dXPosn = (LONG) (dX + lCentralizedOffsetX);
			dYPosn = (LONG) (dY + lCentralizedOffsetY);
		}

		//v4.57A10
		if (bLog)
		{
			if (bRotate180)
			{
				szLog.Format("BBX: FindBinTableXYPos (180) = XY(%f, %f) - CentralizedXY(%ld, %ld) = (%f, %f)", 
					dX, dY, lCentralizedOffsetX, lCentralizedOffsetY, dXPosn, dYPosn);
			}
			else
			{
				//v4.59A23	//Debugged by Jay Lee
				//szLog.Format("BBX: FindBinTableXYPos (180) = XY(%f, %f) + CentralizedXY(%ld, %ld) = (%f, %f)", 
				szLog.Format("BBX: FindBinTableXYPos = XY(%f, %f) + CentralizedXY(%ld, %ld) = (%f, %f)", 
					dX, dY, lCentralizedOffsetX, lCentralizedOffsetY, dXPosn, dYPosn);
			}
			//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}

	}
	else
	{
		dXPosn = dX;
		dYPosn = dY;

		//v4.57A10
		if (bLog && bRotate180)
		{
			if (GetIsUseBlockCornerAsFirstDiePos())
			{
//				szLog.Format("BBX: FindBinTableXYPos (180) = XY(%f, %f); DiePitchXY(%f, %f)", 
//						dXPosn, dYPosn, dDiePitchX, dDiePitchY);
			}
			else
			{
//				szLog.Format("BBX: FindBinTableXYPos = XY(%f, %f)", dXPosn, dYPosn);	//v4.59A23
			}

			//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
//			CMSLogFileUtility::Instance()->BT_ReAlignBinFrameLog(szLog, "a+");
		}
	}

	return TRUE;
	
} //end FindBinTableXYPosnGivenIndex


BOOL CBinBlk::FindBinTableXYWafflePadPosnGivenIndex(ULONG ulIndex, DOUBLE& dXPosn, DOUBLE &dYPosn)
{
	DOUBLE dX=0, dY=0;
	ULONG ulQuotient=0, ulRemainder=0;		//Klocwork
	LONG lCurDiePerRow , lCurDiePerCol;
	LONG lPadsNoX, lPadsNoY;
	LONG lCurPadIndex;
	ULONG ulBinIndex = 1;
	ULONG ulPadIndex = 1;
	DOUBLE dPadUpperLeftX, dPadUpperLeftY;
	LONG lPadX, lPadY;
	LONG lBondPointsInPad = m_lWafflePadSizeX * m_lWafflePadSizeY;

	ulBinIndex = ulIndex;

	// special handle 0 index
	if (ulBinIndex == 0)
	{
		dXPosn = m_lUpperLeftX;
		dYPosn = m_lUpperLeftY;
		return TRUE;
	}

	ULONG ulWalkPath = GetOriginalWalkPath();	//GetWalkPath();	//v4.59A1


	//v4.57A11
	ULONG ulOrigPath = GetOriginalWalkPath();
	LONG lULX = m_lUpperLeftX;
	LONG lULY = m_lUpperLeftY;
	LONG lLRX = m_lLowerRightX;
	LONG lLRY = m_lLowerRightY;
	RotateBlkCorners(lULX, lULY, lLRX, lLRY);
	BOOL bRotate180 = FALSE;
	if (m_ucFrameRotation == BBX_FRAME_ROTATE_180)
	{
		bRotate180 = TRUE;
	}


	if (lBondPointsInPad != 0)
	{
		if (ulBinIndex % lBondPointsInPad == 0)
		{
			lCurPadIndex = (ulBinIndex/lBondPointsInPad) - 1;
		}
		else
		{
			lCurPadIndex = (ulBinIndex/lBondPointsInPad);
		}
	}
	else
	{
		lCurPadIndex = 0;
	}
	
	if (m_lWafflePadSizeX != 0)
	{
		lPadsNoX =  m_ulDiePerRow / m_lWafflePadSizeX;
	}
	else
	{
		lPadsNoX = 1;
	}

	if (m_lWafflePadSizeY != 0)
	{
		lPadsNoY = m_ulDiePerCol / m_lWafflePadSizeY;
	}
	else
	{
		lPadsNoY =1;
	}

	dPadUpperLeftX = m_lUpperLeftX;
	dPadUpperLeftY = m_lUpperLeftY;

	lPadY = lCurPadIndex / lPadsNoX;
	lPadX = lCurPadIndex % lPadsNoX;

	if (lPadY % 2 == 0)
	{
		dPadUpperLeftX = dPadUpperLeftX + lPadX * ((m_lWafflePadDistX - GetDDiePitchX()) + m_lWafflePadSizeX * GetDDiePitchX() );
		dPadUpperLeftY = dPadUpperLeftY + lPadY * ((m_lWafflePadDistY - GetDDiePitchY()) + m_lWafflePadSizeY* GetDDiePitchY());
	}
	else
	{
		dPadUpperLeftX = dPadUpperLeftX + (lPadsNoX - lPadX - 1) * ((m_lWafflePadDistX - GetDDiePitchX()) + m_lWafflePadSizeX * GetDDiePitchX() );
		dPadUpperLeftY = dPadUpperLeftY + lPadY * ((m_lWafflePadDistY - GetDDiePitchY()) + m_lWafflePadSizeY* GetDDiePitchY());
	}
	
	
	// calcualate the bond point with respect to the single pad
	ulPadIndex = ulBinIndex - lCurPadIndex * (lBondPointsInPad);

	if (ulPadIndex == 0)
	{
		ulPadIndex = 1;
	}

	if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
	{
		if( m_lWafflePadSizeX!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeX;
			ulRemainder = ulPadIndex % m_lWafflePadSizeX;
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lCurDiePerRow = m_lWafflePadSizeX;
				dX = dPadUpperLeftX + GetDDiePitchX() * m_lWafflePadSizeX;
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in row
			{
				lCurDiePerRow = 1;
				dX = dPadUpperLeftX + GetDDiePitchX();
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lCurDiePerCol = ulQuotient;
			dY = dPadUpperLeftY + GetDDiePitchY() * ulQuotient;
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lCurDiePerRow = ulRemainder;
				dX = dPadUpperLeftX + GetDDiePitchX() * ulRemainder;
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lCurDiePerRow = m_lWafflePadSizeX - ulRemainder + 1;
				dX = dPadUpperLeftX + GetDDiePitchX() * (m_lWafflePadSizeX - ulRemainder + 1);
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lCurDiePerCol = ulQuotient + 1;
			dY =  dPadUpperLeftY + GetDDiePitchY() * (ulQuotient + 1);
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dX = dX - GetDDiePitchX();
			if ((ulBinIndex != 0) && !bRotate180)	//v4.59A10
			{
				dY = dY - GetDDiePitchY();
			}
		}
	}
	else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
	{
		if( m_lWafflePadSizeY!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeY;
			ulRemainder = ulPadIndex % m_lWafflePadSizeY;
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lCurDiePerCol = m_lWafflePadSizeY;
				dY = dPadUpperLeftY + GetDDiePitchY() * m_lWafflePadSizeY;
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in column
			{
				lCurDiePerCol = 1;
				dY = dPadUpperLeftY + GetDDiePitchY();
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			lCurDiePerRow = ulQuotient;
			dX = dPadUpperLeftX + GetDDiePitchX() * ulQuotient;
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lCurDiePerCol = ulRemainder;
				dY = dPadUpperLeftY + GetDDiePitchY() * ulRemainder;
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lCurDiePerCol = m_lWafflePadSizeY - ulRemainder + 1;
				dY = dPadUpperLeftY + GetDDiePitchY() * (m_lWafflePadSizeY - ulRemainder + 1);
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lCurDiePerRow = ulQuotient + 1;
			dX = dPadUpperLeftX + GetDDiePitchX() * (ulQuotient + 1);
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dY = dY - GetDDiePitchY();
			if (ulBinIndex != 0)
			{
				dX = dX - GetDDiePitchX();
			}
		}
	}

	else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
	{
		if( m_lWafflePadSizeX!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeX;
			ulRemainder = ulPadIndex % m_lWafflePadSizeX;
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lCurDiePerRow = 1;
				dX = dPadUpperLeftX + GetDDiePitchX();
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in row
			{
				lCurDiePerRow = m_lWafflePadSizeX;
				dX = dPadUpperLeftX + GetDDiePitchX() * m_lWafflePadSizeX;
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			
			lCurDiePerCol = ulQuotient;
			dY = dPadUpperLeftY + GetDDiePitchY() * ulQuotient;
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lCurDiePerRow = m_lWafflePadSizeX - ulRemainder + 1;
				dX = dPadUpperLeftX + GetDDiePitchX() * (m_lWafflePadSizeX - ulRemainder + 1);
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lCurDiePerRow = ulRemainder;
				dX = dPadUpperLeftX + GetDDiePitchX() * ulRemainder;
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lCurDiePerCol = ulQuotient + 1;
			dY =  dPadUpperLeftY + GetDDiePitchY() * (ulQuotient + 1);
		}

		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dX = dX - GetDDiePitchX();
			if ((ulBinIndex != 0) && !bRotate180)
			{
				dY = dY - GetDDiePitchY();
			}
		}		
	}
	else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
	{
		if( m_ulDiePerCol!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeY;
			ulRemainder = ulPadIndex % m_lWafflePadSizeY;
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lCurDiePerCol = m_lWafflePadSizeY;
				dY = dPadUpperLeftY + GetDDiePitchY() * m_lWafflePadSizeY;
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in column
			{
				lCurDiePerCol = 1;
				dY = dPadUpperLeftY + GetDDiePitchY();
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lCurDiePerRow = m_lWafflePadSizeX - ulQuotient + 1;
			dX = dPadUpperLeftX + GetDDiePitchX() * (m_lWafflePadSizeX - ulQuotient + 1);
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lCurDiePerCol = ulRemainder;
				dY = dPadUpperLeftY + GetDDiePitchY() * ulRemainder;
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lCurDiePerCol = m_lWafflePadSizeY - ulRemainder + 1;
				dY = dPadUpperLeftY + GetDDiePitchY() * (m_lWafflePadSizeY - ulRemainder + 1);
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			
			lCurDiePerRow = m_lWafflePadSizeX - ulQuotient;
			dX = dPadUpperLeftX + GetDDiePitchX() * (m_lWafflePadSizeX - ulQuotient);
		}
	
		//Use Logical Block Corner as First Die Position
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			dY = dY - GetDDiePitchY();
			if (ulBinIndex != 0)
			{
				dX = dX - GetDDiePitchX();
			}
		}
	}

	if (bRotate180)
	{
		RotateBondPos(dX, dY);		//v4.59A1

		//v4.59A10
		if (GetIsUseBlockCornerAsFirstDiePos())
		{
			if (ulWalkPath == BT_TLH_PATH)		//TL-Horizontal
			{
				if (ulBinIndex != 0)
				{
					dY = dY + GetDDiePitchY();
				}
			}
			else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
			{
				if ((ulBinIndex != 0))
				{
					dY = dY + GetDDiePitchY();
				}
			}
		}
	}

	dXPosn = dX;
	dYPosn = dY;

	return TRUE;
}

/*****************************************/
/*         Save Die Info functions       */
/*****************************************/		

VOID CBinBlk::CalculateDieInfoDbCapacity(ULONG ulDiePerBlk, BOOL bAddDieTimeStamp, BOOL bCheckProcessTime)
{
	m_dwaWMapX.RemoveAll();
	m_dwaWMapY.RemoveAll();
	m_dwaWMapSeqNo.RemoveAll();
	m_szaDieBondTime.RemoveAll();
	m_dwaWMapEncX.RemoveAll();
	m_dwaWMapEncY.RemoveAll();
	m_dwaBtEncX.RemoveAll();
	m_dwaBtEncY.RemoveAll();

	m_dwaWMapX.SetSize(ulDiePerBlk + 1);
	m_dwaWMapY.SetSize(ulDiePerBlk + 1);
	m_dwaWMapSeqNo.SetSize(ulDiePerBlk + 1);
	m_dwaWMapEncX.SetSize(ulDiePerBlk + 1);
	m_dwaWMapEncY.SetSize(ulDiePerBlk + 1);
	m_dwaBtEncX.SetSize(ulDiePerBlk + 1);
	m_dwaBtEncY.SetSize(ulDiePerBlk + 1);

	if (bAddDieTimeStamp == TRUE)
	{
		m_szaDieBondTime.SetSize(ulDiePerBlk + 1);
	}

	if (bCheckProcessTime == TRUE)
	{
		m_szaDieProcessTime.SetSize(ulDiePerBlk + 1);
	}

} //end CalculateDieInfoDbCapacity


BOOL CBinBlk::SaveDieInfo(ULONG ulBondIndex, LONG lWaferMapX, LONG lWaferMapY, 
						  BOOL bAddDieTimeStamp, BOOL bCheckProcessTime,
						  LONG lWafEncX, LONG lWafEncY,			//andrewng //2020-0619
						  LONG lBtEncX, LONG lBtEncY)			//andrewng //2020-0630
{
	ULONG ulXSize = (ULONG)m_dwaWMapX.GetSize();
	ULONG ulYSize = (ULONG)m_dwaWMapY.GetSize();
	ULONG ulBondTimeSize = (ULONG)m_szaDieBondTime.GetSize();
	ULONG ulDieProcessTimeSize = (ULONG)m_szaDieProcessTime.GetSize();

	if ( (ulBondIndex >= ulXSize) || (ulBondIndex >= ulYSize))
	{
		return FALSE;
	}

	if (bAddDieTimeStamp == TRUE)
	{
		if (ulBondIndex >= ulBondTimeSize)
		{
			return FALSE;
		}
	}

	if (bCheckProcessTime == TRUE)
	{
		if(ulBondIndex >= ulDieProcessTimeSize)
		{
			return FALSE;
		}
	}

	//andrewng //2020-0729
	CString szLog;
	szLog.Format("BBX: SaveDieInfo - WT ENC (%ld, %ld)", lWafEncX, lWafEncY);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);


	m_dwaWMapX.SetAt(ulBondIndex, lWaferMapX);
	m_dwaWMapY.SetAt(ulBondIndex, lWaferMapY);
	m_dwaWMapSeqNo.SetAt(ulBondIndex,	ulBondIndex);	//Nichia//v4.43T7
	//andrewng //2020-0619
	m_dwaWMapEncX.SetAt(ulBondIndex, lWafEncX);
	m_dwaWMapEncY.SetAt(ulBondIndex, lWafEncY);
	//andrewng //2020-0630
	m_dwaBtEncX.SetAt(ulBondIndex, lBtEncX);
	m_dwaBtEncY.SetAt(ulBondIndex, lBtEncY);


	if (bAddDieTimeStamp == TRUE)
	{
		CString szGenDieInfoTime;
		CTime ctGenDieInfoTime = CTime::GetCurrentTime();
		szGenDieInfoTime = ctGenDieInfoTime.Format("%Y-%m-%d %H:%M:%S");

		m_szaDieBondTime.SetAt(ulBondIndex, szGenDieInfoTime);
	}

	if (bCheckProcessTime == TRUE)
	{
		//calculate process time of each die
		double dCurrentTime = GetTime();
		CString szTimeDiff;
		if ( m_dLastTime > 0.0 )
		{
			m_dCycleTime = fabs(dCurrentTime - m_dLastTime);

			if ( m_dCycleTime > 99999 )
			{
				m_dCycleTime = 10000;
			}
			
		}
		else
		{
			m_dCycleTime = 100;
		}
		m_dLastTime = dCurrentTime;

		szTimeDiff.Format(_T("%f"), m_dCycleTime);
		m_szaDieProcessTime.SetAt(ulBondIndex,szTimeDiff); 
	}

	return TRUE;
} //end SaveDieInfo

/**********************************/
/*   Output File Manipulations    */
/**********************************/

BOOL CBinBlk::FindBondRowColGivenIndex(ULONG ulIndex, LONG& lRow, LONG& lCol, BOOL bUseOrigPath)
{
	LONG lBinRow=0, lBinCol=0;
	ULONG ulBinIndex = 1;

	ulBinIndex = ulIndex + m_ulSkipUnit;

	//andrew7890	//pllm
	if (GetUseCircularArea())
	{
		LONG lCirRow=0, lCirCol=0;
		BOOL bStatus = FindCirBondRowColGivenIndex(ulIndex, lCirRow, lCirCol);
		lRow = lCirRow;
		lCol = lCirCol;
		return bStatus;
	}

	if (m_bEnableWafflePad == TRUE)
	{
		BOOL bStatus = FindWafflePadBondRowColGivenIndex(ulIndex, lRow, lCol);
		return bStatus;
	}
	
	ULONG ulWalkPath = GetWalkPath();	//v4.49A7
	//v4.57A4
	if (bUseOrigPath)
	{
		ulWalkPath = GetOriginalWalkPath();
	}

	GetRowColWithIndexInBinMap(ulBinIndex, ulWalkPath, m_ulDiePerRow, m_ulDiePerCol, m_bNoReturnTravel, lBinRow, lBinCol);

	lRow = lBinRow;
	lCol = lBinCol;
	return TRUE;

} //end FindBondRowColGivenIndex


BOOL CBinBlk::FindWafflePadBondRowColGivenIndex(ULONG ulIndex, LONG& lRow, LONG &lCol)
{
	LONG lX=0, lY=0;
	ULONG ulQuotient=0, ulRemainder=0;	//Klocwork
	LONG lPadsNoX, lPadsNoY;
	LONG lCurPadIndex;
	ULONG ulBinIndex = 1;
	ULONG ulPadIndex = 1;
	LONG lOffsetRow, lOffsetCol;
	LONG lPadX, lPadY;
	LONG lBondPointsInPad = m_lWafflePadSizeX * m_lWafflePadSizeY;

	ulBinIndex = ulIndex;

	if (lBondPointsInPad != 0)
	{
		if (ulBinIndex % lBondPointsInPad == 0)
		{
			lCurPadIndex = (ulBinIndex/lBondPointsInPad) - 1;
		}
		else
		{
			lCurPadIndex = (ulBinIndex/lBondPointsInPad);
		}
	}
	else
	{
		lCurPadIndex = 0;
	}
	
	if (m_lWafflePadSizeX != 0)
	{
		lPadsNoX =  m_ulDiePerRow / m_lWafflePadSizeX;
	}
	else
	{
		lPadsNoX = 1;
	}

	if (m_lWafflePadSizeY != 0)
	{
		lPadsNoY = m_ulDiePerCol / m_lWafflePadSizeY;
	}
	else
	{
		lPadsNoY =1;
	}

	lPadY = lCurPadIndex / lPadsNoX;
	lPadX = lCurPadIndex % lPadsNoX;

	if (lPadY % 2 == 0)
	{
		lOffsetRow = lPadY * m_lWafflePadSizeY;
		lOffsetCol = lPadX * m_lWafflePadSizeX;
	}
	else
	{
		lOffsetRow = lPadY * m_lWafflePadSizeY;
		lOffsetCol = (lPadsNoX - lPadX - 1) * m_lWafflePadSizeX;
	}
	
	
	// calcualate the bond point with respect to the single pad
	ulPadIndex = ulBinIndex - lCurPadIndex * (lBondPointsInPad);

	if (ulPadIndex == 0)
	{
		ulPadIndex = 1;
	}

	ULONG ulWalkPath = GetWalkPath();	//v4.49A7

	if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
	{
		if( m_lWafflePadSizeX!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeX;
			ulRemainder = ulPadIndex % m_lWafflePadSizeX;
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lCol = m_lWafflePadSizeX;	
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in row
			{
				lCol = 1;
			}

			lRow = ulQuotient;
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lCol = ulRemainder;
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lCol = m_lWafflePadSizeX - ulRemainder + 1;
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lRow = ulQuotient + 1;
		}

		//CString szTemp;
		//szTemp.Format("Row:%d Col:%d",lRow, lCol);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	}
	else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
	{
		if( m_lWafflePadSizeY!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeY;
			ulRemainder = ulPadIndex % m_lWafflePadSizeY;
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lRow = m_lWafflePadSizeY;
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in column
			{
				lRow = 1;
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			
			lCol = ulQuotient;
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lRow = ulRemainder;
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lRow = m_lWafflePadSizeY - ulRemainder + 1;
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lCol = ulQuotient + 1;
		}

		//CString szTemp;
		//szTemp.Format("Row:%d Col:%d",lRow, lCol);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	}

	else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
	{
		if( m_lWafflePadSizeX!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeX;
			ulRemainder = ulPadIndex % m_lWafflePadSizeX;
		}

		if (ulRemainder == 0) //change row die - either the first or last in row
		{
			if (ulQuotient % 2 == 1) //last in row
			{
				lCol = 1;
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in row
			{
				lCol = m_lWafflePadSizeX;
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			
			lRow = ulQuotient;
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lCol = m_lWafflePadSizeX - ulRemainder + 1;
				m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lCol = ulRemainder;
				m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lRow = ulQuotient + 1;
		}	

		//CString szTemp;
		//szTemp.Format("Row:%d Col:%d",lRow, lCol);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

	}
	else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
	{
		if( m_ulDiePerCol!=0 )	// divide by zero
		{
			ulQuotient = ulPadIndex / m_lWafflePadSizeY;
			ulRemainder = ulPadIndex % m_lWafflePadSizeY;
		}

		if (ulRemainder == 0) //change row die - either the first or last in column
		{
			if (ulQuotient % 2 == 1) //last in column
			{
				lRow = m_lWafflePadSizeY;
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //first in column
			{
				lRow = 1;
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}

			lCol = m_lWafflePadSizeX - ulQuotient + 1;
		}
		else //die other than the change row one
		{
			if (ulQuotient % 2 == 0) //on odd row
			{
				lRow = ulRemainder;
				m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			else //on even row
			{
				lRow = m_lWafflePadSizeY - ulRemainder + 1;
				m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
			}
			
			lCol = m_lWafflePadSizeX - ulQuotient;
		}

		//CString szTemp;
		//szTemp.Format("Row:%d Col:%d",lRow, lCol);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

		
	}

	lRow = lRow + lOffsetRow;
	lCol = lCol + lOffsetCol;

	return TRUE;
}


BOOL CBinBlk::IsContentEmpty(const CString szElectricInfo)
{
	CString szTemp = szElectricInfo;

	while (!szTemp.IsEmpty())
	{
		LONG lPos = szTemp.Find(",");
		if ((lPos > 0) || (lPos == -1))
		{
			return FALSE;
		}

		if (szTemp.GetLength() == 1)
		{
			return TRUE;
		}
		szTemp = szTemp.Right(szTemp.GetLength() - 1);
	}

	return TRUE;
}

BOOL CBinBlk::GenTempFileFromNVRAM(BT_NVTEMPDATA* pNVData, ULONG ulBlkId, BOOL bFilenameASWaferID, BOOL bAddDieTimeStamp, BOOL bAddSortingSequence)
{
	CStdioFile cfWaferMapFile, cfTempFile;
	CString szBlkId, szBlkIdForFilename;
	CString szFilename, szMapFilename;
	CString szWaferId, szElectricInfo;
	BOOL bMapFileOpened = FALSE;
	BOOL bHaveOtherFile = FALSE;
	ULONG ulTotalNoOfDie = 0;
	INT nIndex = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bDetectWrongData = FALSE;
	CString szLogMsg;


	CTime ctCurTime = CTime::GetCurrentTime();
	CString szGenDieInfoTime = ctCurTime.Format("%Y/%m/%d %H:%M:%S %p");
	CString szCurrTime;
	szCurrTime.Format("%d-%2d-%2d %2d:%2d:%2d", ctCurTime.GetYear(), ctCurTime.GetMonth(), ctCurTime.GetDay(), 
					  ctCurTime.GetHour(), ctCurTime.GetMinute(), ctCurTime.GetSecond());

	szLogMsg.Format("%s - Gen TEMP File from NVRAM: Blk %lu; FNameAsWaferID=%d, TimeStamp=%d, SortSeq=%d", 
								szCurrTime, ulBlkId, bFilenameASWaferID, bAddDieTimeStamp, bAddSortingSequence);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg, TRUE);


	//Decode file name from wafermap (Search any others file next to map file name)
	szMapFilename = CMS896AStn::m_WaferMapWrapper.GetFileName();
	INT nCol = szMapFilename.ReverseFind(';');
	if ( nCol != -1 )
	{
		bHaveOtherFile = TRUE;
		szMapFilename = szMapFilename.Left(nCol);
	}


	szBlkId.Format("%d", ulBlkId);
	szBlkIdForFilename.Format("%d", ulBlkId);
	if (ulBlkId < 10)
	{
		szBlkIdForFilename = "0" + szBlkIdForFilename;
	}

	szFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkIdForFilename + "TempFile.csv";
	
	if (cfTempFile.Open(_T(szFilename), CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		szLogMsg.Format("%s - Failed to generate file: Blk %d", szCurrTime, ulBlkId);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg, TRUE);
		return FALSE;
	}

	cfTempFile.SeekToEnd();

	szWaferId		= "";
	szElectricInfo	= "";


	szLogMsg.Format("%s - Map File = %s; Output File = %s; Other File = %d", 
						szCurrTime, szMapFilename, szFilename, bHaveOtherFile);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

	//Get info from HD
	CString szLocalFileName = MS_LCL_CURRENT_MAP_FILE;

	bMapFileOpened = cfWaferMapFile.Open(szLocalFileName, CFile::modeRead|CFile::shareDenyNone);
	if (bMapFileOpened == TRUE) 
	{
		CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferId);

		szLogMsg.Format("\n%s - Get Raw Wafer Id: %s", szCurrTime, szWaferId);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");


		if ( pApp->GetCustomerName()!="SanAn" && pApp->GetCustomerName()!="TongHui" && pApp->GetCustomerName()!="BSE" )
		{
			nIndex = szWaferId.ReverseFind('.');	//cut extension for all waferID (for Ubilux)
			if ( nIndex != -1 )
			{
				szWaferId = szWaferId.Left(nIndex);
			}
		}

		szWaferId = szWaferId.Trim(",");
		

		szLogMsg.Format("\n%s - Wafer Id: %s", szCurrTime, szWaferId);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

		if ( szWaferId.IsEmpty() == TRUE ) //no wafer id get from wafer map library
		{
			szWaferId = CMS896AStn::m_szMapFileName;
			if (szWaferId == "")
			{
				szWaferId = "NO_ID";
			}
		}


		szLogMsg.Format("%s - Wafer Id 2: %s,FileNameAsWaferID,%d,MapFileName,%s,Path,%s", 
			szCurrTime, szWaferId,bFilenameASWaferID,CMS896AStn::m_szMapFileName,szLocalFileName);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

		if ( bFilenameASWaferID ) //Use Filename as WaferID option @ Map Setting 2nd page
		{
			szWaferId = CMS896AStn::m_szMapFileName;
			if( CMS896AStn::m_szMapFileExt1.IsEmpty() == FALSE )
			{
				if(pApp->GetCustomerName() == CTM_TESTAR)
				{
					szWaferId = szWaferId +  CMS896AStn::m_szMapFileExt1;
				}
				else
				{
				    szWaferId = szWaferId + "." + CMS896AStn::m_szMapFileExt1;
		         }
			}
		}

		//Use Map Header "LOT" + "WAFER" as WaferID
		if ( bHaveOtherFile == TRUE )
		{
			CString szTmp1, szTmp2;
			if ( CMS896AStn::m_WaferMapWrapper.GetReader() != NULL )
			{
				szTmp1 = CMS896AStn::m_WaferMapWrapper.GetReader()->GetHeader().GetLotID();
				CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_WAFER, szTmp2);
			}

			szWaferId = szTmp1 + ":" + szTmp2;

			szLogMsg.Format("%s - Wafer Id 3: %s", szCurrTime, szWaferId);
			CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
		}

		if (pApp->GetFeatureStatus(MS896A_FUNC_APPEND_IN_WAFERID) == TRUE)
		{
			AppendInfoInWaferID(szWaferId, pApp->GetCustomerName());
		}
		
		
		ulTotalNoOfDie = pNVData->lIndex;

		ULONG ulDieCount = 0;

		if (ulTotalNoOfDie > 0)
		{
			CString szTotalNoOfDie;
			CString szMapCol, szMapRow;
			CString szBinCol, szBinRow;
			CString szGrade;

			LONG lUserCol = 0, lUserRow = 0;
			USHORT usMapAngle = CMS896AStn::m_usMapAngle;
			BOOL bHFlip = CMS896AStn::m_bMapOHFlip;
			BOOL bVFlip = CMS896AStn::m_bMapOVFlip;
			ULONG ulMapCol = 0, ulMapRow = 0;
			LONG lBinCol = 0, lBinRow = 0;
			LONG lBinIndex = 0;
			UCHAR ucGrade = 0;
			UCHAR ucOrigGrade = 0, ucOrigGradeMap = 0;
			//BOOL bIsHole = FALSE;


			szTotalNoOfDie.Format("Total No of Die,%d\n", ulTotalNoOfDie);
			cfTempFile.WriteString(szTotalNoOfDie);

			szLogMsg.Format("%s - Bin #%lu, Total: %lu", szCurrTime, ulBlkId, ulTotalNoOfDie);
			CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");


			for (INT i = 0; i < (LONG)ulTotalNoOfDie; i++)
			{
				//bIsHole = FALSE;
				if (i < 500)
				{
					lUserCol = pNVData->lWaferMapCol[i];
					lUserRow = pNVData->lWaferMapRow[i];
				}
				else
				{
					lUserCol = 0;
					lUserRow = 0;
				}

				CMS896AStn::ConvertOrgUserToAsm(lUserRow, lUserCol, ulMapRow, ulMapCol);

				ucGrade = m_ucGrade;//pNVData->lGrade;
				lBinIndex = pNVData->lBinIndex[i];

				//v4.67
				szLogMsg.Format("BBX: GenTempFileFromNVRAM #%d: MAP(%ld, %ld), ASM(%ld, %ld), Grade=%d, INDEX=%d",
						i+1, lUserRow, lUserCol, ulMapRow, ulMapCol, ucGrade, lBinIndex);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLogMsg);
				

				ULONG ulOrigIndex = 0;
				ulOrigIndex = GetOriginalIndex(lBinIndex);
				FindBondRowColGivenIndex(ulOrigIndex, lBinRow, lBinCol, TRUE);


				//v4.04		//PLSG binmap temp file generataion for no-die position 
				if (m_bEnableBinMap && !m_bByPassBinMap)	//v4.48A13
				{
					LONG lMRow = lBinRow;
					LONG lMCol = lBinCol;

					lMRow = min(lMRow, MAX_BINMAP_SIZE_X);
					lMRow = max(lMRow, 1);
					lMCol = min(lMCol, MAX_BINMAP_SIZE_Y);
					lMCol = max(lMCol, 1);

					if (m_nBinMap[lMRow-1][lMCol-1] == 0)	//Must minus 1 to transfer to (0,0)-based coordinate
					{
						//v4.42T7	//v4.47T8
						//szLogMsg.Format("%s - enable bin map skip:%d", szCurrTime, i);
						//CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
						continue;	//Skip bond-die positions that are not available in binmap
					}
				}

				//v4.42T9	//Nichia, Testar
				//Make center of circular binmap as (0,0), instead of default (0,0) at upper-left die
				if (m_bEnableBinMap && !m_bByPassBinMap && GetEnableBinMapOffset())		//v4.48A13
				{
					lBinRow = lBinRow - GetBinMapRowOffset();
					lBinCol = lBinCol - GetBinMapColOffset();
				}
				else if (m_bEnableBinMap && !m_bByPassBinMap && CMS896AApp::m_bMS90)
				{
					lBinRow = lBinRow - 1;
					lBinCol = lBinCol - 1;
				}

				szMapCol.Format("%ld", lUserCol);
				szMapRow.Format("%ld", lUserRow);

				szBinRow.Format("%ld", lBinRow);
				szBinCol.Format("%ld", lBinCol);

				szGrade.Format("%d", ucGrade);


				if (CMS896AStn::m_WaferMapWrapper.GetExtraInformation(&cfWaferMapFile, ulMapRow, ulMapCol, szElectricInfo) == FALSE)
				{
					szLogMsg.Format("%s - GetExtraInformation failed wafer id:%s row:%lu col:%lu user row:%ld user col:%ld", szCurrTime, 
									szWaferId, ulMapRow, ulMapCol, lUserRow, lUserCol);		
					pApp->SetErrorMessage(szLogMsg);
				}

				SetExtraInfoFormat("Comma");
				if (szElectricInfo.IsEmpty() == FALSE)
				{
					int nCol = 0;
					nCol = szElectricInfo.Find(",");

					if (nCol == -1)
					{
						nCol = szElectricInfo.Find("\t");
						if ( nCol != -1 )
						{
							SetExtraInfoFormat("Tab");
							szElectricInfo.Replace("\t", ",");
							szElectricInfo = "," + szElectricInfo;
						}
						else
						{
							SetExtraInfoFormat("Space");
							szElectricInfo.Replace(" ", ",");
						}
					}
					
					if (szElectricInfo.GetLength() > 0)
					{
						if (szElectricInfo.GetAt(0) != ',')
						{
							szElectricInfo = "," + szElectricInfo;
						}
					}
				}

				if ( bHaveOtherFile == TRUE )
				{
					if (szElectricInfo.GetAt(0) == ',')
					{
						szElectricInfo.TrimLeft(",");
					}

					szGrade = szElectricInfo.Left(3);
					szElectricInfo.Empty();
				}

				if (CMS896AStn::m_bEnableGradeMappingFile == TRUE)
				{			
					if (m_szMappedGrade != "")
						szGrade = m_szMappedGrade;
				}

				if (bAddDieTimeStamp == TRUE)
				{
					cfTempFile.WriteString(szWaferId + "," + szMapCol + "," + szMapRow + "," +
										szBinCol + "," + szBinRow + "," + szGrade + "," + szGenDieInfoTime + szElectricInfo + "\n");
				}
				else
				{
					cfTempFile.WriteString(szWaferId + "," + szMapCol + "," + szMapRow + "," +
										szBinCol + "," + szBinRow + "," + szGrade + szElectricInfo + "\n");
				}
			}
		}

		cfWaferMapFile.Close();
	}
	else
	{
		szLogMsg.Format("%s - Failed to open CurrentMap.txt", szCurrTime);
		pApp->SetErrorMessage(szLogMsg);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

		bDetectWrongData = TRUE;
	}

	//Update LastFileSaveIndex to Current Index
	SetNVLastFileSaveIndex(GetNVCurrBondIndex());
	cfTempFile.Close();

	szLogMsg.Format("%s - Gen Temp Complete", szCurrTime);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n\n");

	if( bDetectWrongData )
	{
		return FALSE;
	}

	return TRUE;
}


LONG CBinBlk::GenTempFile(ULONG ulBlkId, BOOL bFilenameASWaferID, BOOL bAddDieTimeStamp, BOOL bCheckProcessTime, BOOL bAddSortingSequence, BOOL& bHasUpdatedFile,
						  BOOL &bWaferMapElectricInfoEmpty, BOOL bUseNVData, BOOL bForceDisableHaveOtherFile)
{
	ULONG ulLastIndex = GetNVLastFileSaveIndex();
	ULONG ulCurrIndex = GetNVCurrBondIndex();
	//Return if Current Index == Last Index	
	if (ulLastIndex == ulCurrIndex)
	{
		bHasUpdatedFile = FALSE;
		return 2;
	}

	CStdioFile cfWaferMapFile, cfTempFile;
	CFile cfWaferMap;
	LONG lMapCol=0, lMapRow=0, lBinCol=0, lBinRow=0, lGrade=0;
	LONG lMapEncX = 0, lMapEncY = 0;
	LONG lBTEncX = 0, lBTEncY = 0;
	UCHAR ucGrade;
	CString szWaferId, szElectricInfo, szMapCol, szMapRow, szBinCol, szBinRow, szGrade;
	CString szBlkId, szBlkIdForFilename, szDieCounter, szTotalNoOfDie;
	CString szTemp, szMapFilename, szLocalFileName;
	CString szFilename, szTempFilename, szNVTempFilename;
	CString szLogMsg;
	CString szSequenceNo;
	CString szWafEncoderInfo;		//andrewng //2020-0619
	CUIntArray aulAllGradeList;
	ULONG ulLeft, ulPick, ulDieTotal;
	ULONG ulTotalPicked;
	//BOOL bAddSequenceNo = FALSE;
	BOOL bDetectWrongBinGrade	= FALSE;	//v4.42T7
	LONG lSortingSequenceNoStart=0, lSortingSequenceNo=0;	//Klocwork	//v4.40T8
	CString szGenDieProcessTime;
	CTime ctCurTime = CTime::GetCurrentTime();
	CString szGenDieInfoTime = ctCurTime.Format("%Y/%m/%d %H:%M:%S %p");
	CString szCurrTime;
	szCurrTime.Format("%d-%2d-%2d %2d:%2d:%2d", ctCurTime.GetYear(), ctCurTime.GetMonth(), ctCurTime.GetDay(), 
					  ctCurTime.GetHour(), ctCurTime.GetMinute(), ctCurTime.GetSecond());

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bSingleBinSort = pApp->GetFeatureStatus(MS896A_FUNC_MULTIGRADE_SORT_TO_SINGLEBIN);	//v4.42T5
	//v4.65A3
	szLogMsg.Format("%s - Gen TEMP File: Blk %lu; FNameAsWaferID=%d, TimeStamp=%d, SortSeq=%d, UpdateFile=%d, UseNVData=%d, ForceOtherFile=%d", 
								szCurrTime, ulBlkId, bFilenameASWaferID, bAddDieTimeStamp, bAddSortingSequence,
								bHasUpdatedFile, bUseNVData, bForceDisableHaveOtherFile);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg, TRUE);
	int nIndex;
	int ulTotalNoOfDie = 0;
	int nCol = 0;

	ULONG ulFirstRowColSkippedUnit = 0;
	ULONG ulWafflePadSkippedUnit = 0;

	BOOL bMapFileOpened = FALSE;
	BOOL bCFileOpened	= FALSE;
	BOOL bHaveOtherFile = FALSE;
	BOOL bHaveNVData = FALSE;
	ULONG i;

	//v4.21T8		//Osram RESORT mode to eliminate extra ";" in electrical info
	BOOL bOsramResortMode = FALSE;
	if ( (pApp->GetCustomerName() == "OSRAM") && pApp->GetFeatureStatus(MS896A_FUNC_OSRAM_RESORT_MODE))
		bOsramResortMode = TRUE;
	BOOL bNichia = FALSE;
	//if (pApp->GetCustomerName() == "Nichia")			//v4.43T12
	if (CMS896AStn::m_oNichiaSubSystem.IsEnabled())		//v4.43T13
		bNichia = TRUE;

	//v4.50A5	//Cree HuiZhou
	BOOL bCreeUseOriginalGrade = FALSE;
	if( pApp->IsMapDetectSkipMode() )
	{
		bCreeUseOriginalGrade = TRUE;
	}

	FILE *fNVData = NULL;
	CString szLogFileName, szNVFileName;

	//NV data file
	szNVFileName.Format("NV_%d.txt", ulBlkId);
	szNVFileName = gszUSER_DIRECTORY + "\\NVRAM\\" + szNVFileName;

	bHasUpdatedFile = TRUE;

	szBlkId.Format("%d", ulBlkId);
	szBlkIdForFilename.Format("%d", ulBlkId);

	if (ulBlkId < 10)
	{
		szBlkIdForFilename = "0" + szBlkIdForFilename;
	}

	szNVTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkIdForFilename + "TempFileNV.csv";
	szTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkIdForFilename + "TempFile.csv";

	// write to a dummy file first and copy to replace orginal if restore is complete
	if (bUseNVData)
	{
		CopyFile(szTempFilename, szNVTempFilename, FALSE);
		szFilename = szNVTempFilename;
	}
	else
	{
		szFilename = szTempFilename;
	}

	//Do append
	if (cfTempFile.Open(_T(szFilename), 
						CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		szLogMsg.Format("%s - Failed to generate file: Blk %d", szCurrTime, ulBlkId);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg, TRUE);
		if( bUseNVData )
			pApp->SetErrorMessage(szLogMsg);
		return FALSE;
	}

	cfTempFile.SeekToEnd();

	szWaferId = "";
	szElectricInfo = "";

	if (bUseNVData == TRUE)
	{
		szLogMsg.Format("%s - Use NV Data, Filename: %s", szCurrTime, szNVFileName);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
		pApp->SetErrorMessage(szLogMsg);
	}

	//Decode file name from wafermap (Search any others file next to map file name)
	szMapFilename = CMS896AStn::m_WaferMapWrapper.GetFileName();
	nCol = szMapFilename.ReverseFind(';');
	if ( nCol != -1 )
	{
		bHaveOtherFile = TRUE;
		szMapFilename = szMapFilename.Left(nCol);
	}
	
	if (bForceDisableHaveOtherFile == TRUE)
	{
		bHaveOtherFile = FALSE;
	}


	szLogMsg.Format("%s - Map: %s, Other File: %d", szCurrTime, szMapFilename, bHaveOtherFile);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

	//Get info from HD
	szLocalFileName = MS_LCL_CURRENT_MAP_FILE;

	bMapFileOpened = cfWaferMapFile.Open(szLocalFileName, CFile::modeRead|CFile::shareDenyNone);
	if( FALSE/*pApp->GetCustomerName()== "AOT"*/ )
	{
		bCFileOpened = cfWaferMap.Open(szLocalFileName, CFile::modeRead|CFile::shareDenyNone);
	}
	if (bMapFileOpened == TRUE) 
	{
		CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferId);
		szLogMsg.Format("\n%s - Get Raw Wafer Id: %s", szCurrTime, szWaferId); // 4.52D12
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

		if(pApp->GetCustomerName() == "Electech3E(DL)")
		{
			nCol = szWaferId.ReverseFind(';');
			if ( nCol != -1 )
			{
				szWaferId = szWaferId.Left(nCol);
				bHaveOtherFile = FALSE;
			}

			szLogMsg.Format("%s - Wafer Id 1: %s", szCurrTime, szWaferId);
			CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
		}

		if( pApp->GetCustomerName()!="SanAn" && pApp->GetCustomerName()!="TongHui" && pApp->GetCustomerName()!="BSE" )
		{
			nIndex = szWaferId.ReverseFind('.');	//cut extension for all waferID (for Ubilux)		//v3.33T1
			if ( nIndex != -1 )
				szWaferId = szWaferId.Left(nIndex);
		}
		else
		{
			if( pApp->GetCustomerName()=="TongHui" )
			{
				CString szTongHuiID;
				szTongHuiID = szWaferId;
				szTongHuiID = szTongHuiID.MakeLower();
				nIndex = szTongHuiID.Find(".csv");
				if ( nIndex != -1 )
				{
					szWaferId = szWaferId.Left(nIndex);
				}
			}
		}

		szWaferId = szWaferId.Trim(",");
		
		szLogMsg.Format("\n%s - Wafer Id: %s", szCurrTime, szWaferId);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

		if ( szWaferId.IsEmpty() == TRUE ) //no wafer id get from wafer map library
		{
			szWaferId = CMS896AStn::m_szMapFileName;
			if (szWaferId == "")
			{
				szWaferId = "NO_ID";
			}
		}

		szLogMsg.Format("%s - Wafer Id 2: %s", szCurrTime, szWaferId);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

		if ( bFilenameASWaferID ) //Use Filename as WaferID option @ Map Setting 2nd page
		{
			szWaferId = CMS896AStn::m_szMapFileName;
			if( CMS896AStn::m_szMapFileExt1.IsEmpty()==FALSE )
			{
				if(pApp->GetCustomerName()=="Testar")
				{
					szWaferId = szWaferId +  CMS896AStn::m_szMapFileExt1; // v4.51D4 Testar

				}
				else
				{
				    szWaferId = szWaferId + "." + CMS896AStn::m_szMapFileExt1;
		         }
			}
		}

		//Use Map Header "LOT" + "WAFER" as WaferID
		if ( bHaveOtherFile == TRUE )
		{
			CString szTmp1, szTmp2;
			if ( CMS896AStn::m_WaferMapWrapper.GetReader() != NULL )
			{
				szTmp1 = CMS896AStn::m_WaferMapWrapper.GetReader()->GetHeader().GetLotID();
				CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(WT_MAP_HEADER_WAFER, szTmp2);
			}

			szWaferId = szTmp1 + ":" + szTmp2;

			szLogMsg.Format("%s - Wafer Id 3: %s", szCurrTime, szWaferId);
			CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
		}

		if (pApp->GetFeatureStatus(MS896A_FUNC_APPEND_IN_WAFERID) == TRUE)
		{
			AppendInfoInWaferID(szWaferId, pApp->GetCustomerName());
		}
		
		//Handle the Total die count when hole(s) for 2D barcode is/are generated on output bin (for Cree)		//v3.33T3
		LONG lCurrentNumOfHole = 0;

		for (i = (ulLastIndex + 1); i <= ulCurrIndex; i++)
		{
			for (INT j = 0; j < (INT) m_ulHoleDieNum; j++)
			{
				if (i == m_ulHoleDieIndex[ulBlkId][j])
					lCurrentNumOfHole++;					
			}
		}
		
		ulFirstRowColSkippedUnit = GetSkippedUnitForFirstRowColSkipPattern(ulLastIndex, ulCurrIndex);
		ulWafflePadSkippedUnit = GetSkippedUnitForWafflePad(ulLastIndex, ulCurrIndex);

		//v4.04	//PLSG Binmap fcn temp file generation with holes
		ULONG ulBinMapEmptyCount = 0;
		if (m_bEnableBinMap && !m_bByPassBinMap)	//v4.48A13
		{
			if (bNichia)	//v4.43T12
			{
				//Nichia//v4.43T7
				LONG lMCol=0, lMRow=0;
				BOOL bExist = TRUE;
				for (i = (ulLastIndex + 1); i <= ulCurrIndex; i++)
				{
					bExist = GetDieIndexInBinMap(m_ulDiePerBlk, i, lMRow, lMCol);
					if (!bExist)
					{
						ulBinMapEmptyCount++;
					}
				}
			}
			else
			{
				CString szTemp;
				ULONG ulOrigIndex;
				LONG lMCol=0, lMRow=0;

				for (i = (ulLastIndex + 1); i <= ulCurrIndex; i++)
				{
					ulOrigIndex = GetOriginalIndex(i);
					FindBondRowColGivenIndex(ulOrigIndex, lMRow, lMCol, TRUE);	//this fcn returns a (1,1)-based coordinate

					lMRow = min(lMRow, MAX_BINMAP_SIZE_X);
					lMRow = max(lMRow, 1);
					lMCol = min(lMCol, MAX_BINMAP_SIZE_Y);
					lMCol = max(lMCol, 1);

					if (CBinBlk::m_nBinMap[lMRow-1][lMCol-1] == 0)				//Must minus 1 to transfer to (0,0)-based coordinate
					{
						ulBinMapEmptyCount++;
					}
				}
			}
		}

		ulTotalNoOfDie = ulCurrIndex - ulLastIndex - lCurrentNumOfHole 
						 - ulFirstRowColSkippedUnit - ulWafflePadSkippedUnit - ulBinMapEmptyCount;
		ULONG ulDieCount = 0;	//v4.43T12

		if (ulTotalNoOfDie > 0)
		{
			szTotalNoOfDie.Format("Total No of Die,%d\n", ulTotalNoOfDie);
			cfTempFile.WriteString(szTotalNoOfDie);

			//Open NV Blk data file to get die info.
			if ( (bUseNVData == TRUE) &&  (_access(szNVFileName, 0) != -1) )
			{
				szLogMsg.Format("%s - Recover data from file: %s", szCurrTime, szNVFileName);
				CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
				pApp->SetErrorMessage(szLogMsg);

				//File exist
				errno_t nErr = fopen_s(&fNVData, szNVFileName, "r");
				if ((nErr == 0) && (fNVData != NULL))
				{
					ULONG ulNVTotal;

					fscanf(fNVData, "Total No of Die,%d\n", &ulNVTotal);
					if ( ulNVTotal == ulTotalNoOfDie )
					{
						bHaveNVData = TRUE;
					}
				}
			}

			BOOL bIsHole = FALSE;

			//v4.43T12
			szLogMsg.Format("%s - Bin #%lu, LastIndex: %lu, CurrentIndex: %lu, Total: %lu (%lu)", 
							szCurrTime, ulBlkId, ulLastIndex+1, ulCurrIndex, ulTotalNoOfDie, ulDieCount);
			CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");

			if( bUseNVData )
				pApp->SetErrorMessage(szLogMsg);


			if (bAddSortingSequence == TRUE)
			{
				// calculate the sorting sequence using the die count
				ulTotalPicked = 0;
				CMS896AStn::m_WaferMapWrapper.GetAvailableGradeList(aulAllGradeList);
				for (int m=0; m<aulAllGradeList.GetSize(); m++)
				{
					CMS896AStn::m_WaferMapWrapper.GetStatistics(aulAllGradeList.GetAt(m), ulLeft, ulPick, ulDieTotal);
					ulTotalPicked = ulTotalPicked + ulPick;
				}

				lSortingSequenceNoStart = ulTotalPicked - (ulCurrIndex - ulLastIndex) + 1;
				szLogMsg.Format("SortingSequenceNoStart:%d", lSortingSequenceNoStart);
				CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
				if (lSortingSequenceNoStart <= 0)
				{
					lSortingSequenceNoStart = 1;
					CMSLogFileUtility::Instance()->BT_GenTempLog("Update SortingSequenceNoStart:1\n");
				}

				lSortingSequenceNo = lSortingSequenceNoStart;
			}

			for (i = (ulLastIndex + 1); i <= ulCurrIndex; i++)
			{
				bIsHole = FALSE;

				//skip the data which is empty				//v3.33T3
				for (INT j = 0; j < (INT) m_ulHoleDieNum; j++)	
				{
					if (i == m_ulHoleDieIndex[ulBlkId][j])
					{
						bIsHole = TRUE;
					}
				}

				if (bIsHole == TRUE)
				{
					szLogMsg.Format("%s - Hole Die Index:%d", szCurrTime, i);

					CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
					continue;
				}

				if (IsFirstRowColSkipBondPatternIndex(i) == TRUE)
				{
					szLogMsg.Format("%s - IsFirstRowColSkipBondPatternIndex:%d", szCurrTime, i);
					CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
					continue;
				}

				if (IsWafflePadSkipPos(i) == TRUE)
				{
					szLogMsg.Format("%s - waffle pad skip pos:%d", szCurrTime, i);
					CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
					continue;
				}
				
				if ( bHaveNVData == TRUE )
				{
					//Get die infor from NV data file
					if (fNVData != NULL)	//Klocwork
					{
						fscanf(fNVData, "%ld,%ld,%ld\n", &lGrade, &lMapCol, &lMapRow);
					}
					ucGrade = (UCHAR)lGrade;
					//szLogMsg.Format("Get from NV_xx.txt; %d,%d,%d", lGrade, lMapCol, lMapRow);
					//pApp->SetErrorMessage(szLogMsg);	//v4.47T9
				}
				else
				{
					if (i < (ULONG) m_dwaWMapX.GetSize())
					{
						lMapCol		= m_dwaWMapX.GetAt(i);
						lMapRow		= m_dwaWMapY.GetAt(i);
						lMapEncX	= m_dwaWMapEncX.GetAt(i);
						lMapEncY	= m_dwaWMapEncY.GetAt(i);
						lBTEncX		= m_dwaBtEncX.GetAt(i);
						lBTEncY		= m_dwaBtEncY.GetAt(i);

						CString szLog;
						szLog.Format("BBX: GenTempFile - WT ENC (%ld, %ld)", lMapEncX, lMapEncY);
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

					}
				
					if (bAddDieTimeStamp == TRUE)
					{
						szGenDieInfoTime = m_szaDieBondTime.GetAt(i);
					}

					if (bCheckProcessTime == TRUE)
					{
						szGenDieProcessTime = m_szaDieProcessTime.GetAt(i);
					}

					ucGrade = m_ucGrade;
					if( bUseNVData )
					{
						szLogMsg.Format("Get from memory; %d,%d,%d, index %d", ucGrade, lMapCol, lMapRow, i);
						pApp->SetErrorMessage(szLogMsg);
					}
				}


				//Convert to original user coord.
				LONG lUserCol = 0, lUserRow = 0;
				USHORT usMapAngle = CMS896AStn::m_usMapAngle;
				BOOL bHFlip = CMS896AStn::m_bMapOHFlip;
				BOOL bVFlip = CMS896AStn::m_bMapOVFlip;
	
				if (CMS896AStn::m_WaferMapWrapper.ConvertInternalToOriginalUser((ULONG)lMapRow, (ULONG)lMapCol, bHFlip, bVFlip, usMapAngle, lUserRow,lUserCol) == FALSE)
				{
					szLogMsg.Format("%s -  failed wafer id:%s row:%d col:%d user row:%d user col:%d", 
									szCurrTime, szWaferId , lMapRow, lMapCol, lUserRow, lUserCol);

					CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
					pApp->SetErrorMessage(szLogMsg);
				}

				//v4.37T11
				//NEw grade retrieval mehtod to support mixed grade sorting	//SanAn
				UCHAR ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalDieInformation((ULONG)lMapRow, (ULONG)lMapCol); 
				ucOrigGrade -= CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
				UCHAR ucOrigGradeMap = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(ucGrade + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()) 
											- CMS896AStn::m_WaferMapWrapper.GetGradeOffset();

				if (bSingleBinSort)					//v4.42T5
				{
					ucGrade = ucOrigGrade;			//Use original grade from wafermap lib array created in LoadMap
				}
				else if (bCreeUseOriginalGrade)		//v4.50A5	//Cree HuiZhou
				{
					ucGrade = ucOrigGradeMap;	
				}
				else
				{
					////v4.42T6	//SanAn	Compare grade between frame and map memory
					//if (!bDetectWrongBinGrade && (ucGrade != ucOrigGrade) )
					//{
					//	//v4.50A12
					//	szLogMsg.Format("%s -  grade not match ID:%s, Die at U(%ld, %ld), M(%ld, %ld)\n GradeOnFrame: %d, GradeInMap: %d", 
					//					szCurrTime, szWaferId, lUserRow, lUserCol, lMapRow, lMapCol, ucGrade, ucOrigGrade);
					//	//CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
					//	pApp->SetErrorMessage(szLogMsg);
					//	bDetectWrongBinGrade = TRUE;
					//}
				}


				ULONG ulOrigIndex;
				ulOrigIndex = GetOriginalIndex(i);
				FindBondRowColGivenIndex(ulOrigIndex, lBinRow, lBinCol, TRUE);	//v4.57A4

				//v4.04		//PLSG binmap temp file generataion for no-die position 
				if (m_bEnableBinMap && !m_bByPassBinMap)	//v4.48A13
				{
					LONG lMRow = lBinRow;
					LONG lMCol = lBinCol;

					lMRow = min(lMRow, MAX_BINMAP_SIZE_X);
					lMRow = max(lMRow, 1);
					lMCol = min(lMCol, MAX_BINMAP_SIZE_Y);
					lMCol = max(lMCol, 1);

					if (bNichia)	//v4.43T12
					{
						//Nichia//v4.43T7
						if (!IsDieInBinMap(lMRow-1, lMCol-1))
						{
							continue;
						}
					}
					else
					{
						if (m_nBinMap[lMRow-1][lMCol-1] == 0)	//Must minus 1 to transfer to (0,0)-based coordinate
						{
							//v4.42T7	//v4.47T8
							//szLogMsg.Format("%s - enable bin map skip:%d", szCurrTime, i);
							//CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
							continue;	//Skip bond-die positions that are not available in binmap
						}
					}
				}

				//v4.42T9	//Nichia, Testar
				//Make center of circular binmap as (0,0), instead of default (0,0) at upper-left die
				if (m_bEnableBinMap && !m_bByPassBinMap && GetEnableBinMapOffset())		//v4.48A13
				{
					lBinRow = lBinRow - GetBinMapRowOffset();
					lBinCol = lBinCol - GetBinMapColOffset();
				}
				//v4.57A4
				else if (m_bEnableBinMap && !m_bByPassBinMap && CMS896AApp::m_bMS90)
				{
					lBinRow = lBinRow - 1;
					lBinCol = lBinCol - 1;
				}

				szMapCol.Format("%ld",	lUserCol);
				szMapRow.Format("%ld",	lUserRow);
				szBinRow.Format("%ld",	lBinRow);
				szBinCol.Format("%ld",	lBinCol);
				szGrade.Format("%d",	ucGrade);
				szSequenceNo.Format("%ld",			lSortingSequenceNo);
				szWafEncoderInfo.Format(",%ld,%ld,%ld,%ld",	lMapEncX, lMapEncY, lBTEncX, lBTEncY);	//andrewng //2020-0619

				CString szLog;
				szLog.Format("BBX: GenTempFile - WT ENC str = %s", (LPCTSTR) szWafEncoderInfo);
//CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

				//Nichia//v4.43T7
				if (bAddSortingSequence == TRUE)
				{
					szLogMsg.Format("SortingSeq - CurrIndex:%ld, DieIndex:%;d", lSortingSequenceNo, m_dwaWMapSeqNo.GetAt(i));
					CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
				}
				lSortingSequenceNo = lSortingSequenceNo + 1;

				if ( FALSE/*pApp->GetCustomerName()== "AOT"*/ )
				{
					unsigned long lPos = CMS896AStn::m_WaferMapWrapper.GetReader()->GetExtraInfo(lMapRow,lMapCol);
					cfWaferMap.Seek(lPos, CFile::begin);     // MapFile is in CFile type!
					unsigned char acData[2];
					CString szLine = "";
					while (cfWaferMap.Read(acData, 2) == 2)
					{
						if (acData[0] == 0x0A)
						{
							break;
						}
						if (acData[0] != 0x0D)
						{
							szLine += acData[0];
						}
					} 
					szElectricInfo = szLine;
				}
				else if (CMS896AStn::m_WaferMapWrapper.GetExtraInformation(&cfWaferMapFile, lMapRow, lMapCol, szElectricInfo) == FALSE)
				{
					szElectricInfo.Empty();
					szLogMsg.Format("%s - GetExtraInformation failed wafer id:%s row:%d col:%d user row:%d user col:%d", szCurrTime, 
									szWaferId, lMapRow, lMapCol, lUserRow, lUserCol);
					
					CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
					pApp->SetErrorMessage(szLogMsg);
				}

				if ((ucOrigGrade != ucGrade) || szElectricInfo.IsEmpty() || IsContentEmpty(szElectricInfo))
				{
					szLogMsg.Format("%s - (Not Match Info) wafer id:%s row:%d col:%d user row:%d user col:%d, m_ucGrade:%d, ucOrigGrade:%d, ucGrade:%d, szElectricInfo:%s", szCurrTime, 
										szWaferId, lMapRow, lMapCol, lUserRow, lUserCol, m_ucGrade, ucOrigGrade, ucGrade, szElectricInfo);
					CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
					pApp->SetErrorMessage(szLogMsg);

					//display error 2019.1.1
					bWaferMapElectricInfoEmpty = TRUE;
				}
		
				SetExtraInfoFormat("Comma");
				if (szElectricInfo.IsEmpty() == FALSE)
				{
					int nCol = 0;
					nCol = szElectricInfo.Find(",");

					if (nCol == -1)
					{
						nCol = szElectricInfo.Find("\t");
						if ( nCol != -1 )
						{
							SetExtraInfoFormat("Tab");
							szElectricInfo.Replace("\t", ",");
							szElectricInfo = "," + szElectricInfo;
						}
						else
						{
							SetExtraInfoFormat("Space");
							szElectricInfo.Replace(" ", ",");
						}
					}
					
					if (szElectricInfo.GetLength() > 0)
					{
						if (szElectricInfo.GetAt(0) != ',')
						{
							szElectricInfo = "," + szElectricInfo;
						}
					}

					//v4.21T8
					if (bOsramResortMode)
					{
						szElectricInfo.Replace(";", "");	//OSRAM resort mode electrical info to eliminate extra ";"
					}
				}

				if ( bHaveOtherFile == TRUE )
				{
					if (szElectricInfo.GetAt(0) == ',')
					{
						szElectricInfo.TrimLeft(",");
					}

					szGrade = szElectricInfo.Left(3);
					szElectricInfo.Empty();
				}

				if (CMS896AStn::m_bEnableGradeMappingFile == TRUE)
				{			
					if (m_szMappedGrade != "")
						szGrade = m_szMappedGrade;
				}

				//szElectricInfo = szElectricInfo + szWafEncoderInfo;	//andrewng //2020-0619
				szElectricInfo = szWafEncoderInfo;						//andrewng //2020-729

				//if (bAddDieTimeStamp == TRUE)
				//{
				//	cfTempFile.WriteString(szWaferId + "," + szMapCol + "," + szMapRow + "," +
				//						   szBinCol + "," + szBinRow + "," + szGrade + "," + szGenDieInfoTime + szElectricInfo + "\n");
				//}
				if (bCheckProcessTime == TRUE)
				{
					cfTempFile.WriteString(szWaferId + "," + szMapCol + "," + szMapRow + "," +
										   szBinCol + "," + szBinRow + "," + szGrade + "," + szGenDieInfoTime + ","  + szGenDieProcessTime + szElectricInfo + "\n");
				}

				else if  (bAddSortingSequence == TRUE)
				{
					cfTempFile.WriteString(szWaferId + "," + szMapCol + "," + szMapRow + "," +
										   szBinCol + "," + szBinRow + "," + szGrade + "," + szSequenceNo + szElectricInfo + "\n");
				}
				else
				{
				
					cfTempFile.WriteString(szWaferId + "," + szMapCol + "," + szMapRow + "," +
										   szBinCol + "," + szBinRow + "," + szGrade + szElectricInfo + "\n");
				}
			}
		}

		cfWaferMapFile.Close();
		if (FALSE/*pApp->GetCustomerName()== "AOT" && bCFileOpened*/)
		{
			cfWaferMap.Close();
		}

		if (fNVData != NULL)
			fclose(fNVData);
	}
	else
	{
		szLogMsg.Format("%s - Failed to open CurrentMap.txt", szCurrTime);
		pApp->SetErrorMessage(szLogMsg);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
	}


	//Update LastFileSaveIndex to Current Index
	SetNVLastFileSaveIndex(ulCurrIndex);

	cfTempFile.Close();

	if (bUseNVData)
	{
		CopyFile(szNVTempFilename, szTempFilename, FALSE);
	}

	if (CMS896AStn::m_bBackupTempFile == TRUE)
	{
		szLogMsg.Format("%s - Backup Temp File", szCurrTime);
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n");
		BackupTempFile(ulBlkId);
	}

	szLogMsg.Format("%s - Gen Temp Complete", szCurrTime);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg + "\n\n");

	return TRUE;
} //end GenTempFile


VOID CBinBlk::BackupTempFile(ULONG ulBlkId)
{
	CTime curTime = CTime::GetCurrentTime();
	CString szCurTime;
	CString szOrgFilename;
	CString szBlkId, szBlkIdForFilename;
	CString szBackupFileOrgGrade;
	CString szBackupFilePath, szBackupFilename;

	szBlkId.Format("%d", ulBlkId);
	szBlkIdForFilename.Format("%d", ulBlkId);

	if (ulBlkId < 10)
	{
		szBlkIdForFilename = "0" + szBlkIdForFilename;
	}
	//AfxMessageBox("BackupTempFile", MB_SYSTEMMODAL);

	szOrgFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkIdForFilename + "TempFile.csv";

	if (_access(szOrgFilename, 0) == -1)
	{
		return;
	}

	//AfxMessageBox("BackupTempFile2", MB_SYSTEMMODAL);
	szCurTime = curTime.Format("%Y%m%d%H%M%S");
	szBackupFileOrgGrade.Format("%d", m_ucOriginalGrade);

	szBackupFilePath = gszUSER_DIRECTORY + "\\TempFileBackup\\Processing_TempFileBackupSingleBlk";
	
	//v4.51D7 SanAn(WH)
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		CMSLogFileUtility::Instance()->BT_GenTempLog("\n The BinTable.msd is null\n");
		return;
	}
	
	ULONG ulTempBlkId;
	ulTempBlkId = (*pBTfile)["BinBlock"]["Temp Blk ID"];
	
	CString szTempFileName;
	CString szTempFileName2;
	CString szOrgBackupFilename;
	CString szBackupFileMainPath;
	CString szBackupFileMainName;
	CString szMsg;

	if(ulBlkId == ulTempBlkId)
	{
		//Delete all previous Temp files if present in the folder
		if (_access(szBackupFilePath, 0) != -1)
		{
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			pApp->SearchAndRemoveFiles(szBackupFilePath, 30, TRUE);
			//AfxMessageBox("Have Deleted All previous Temp Files");
		}
		CreateDirectory(szBackupFilePath, NULL);
		szTempFileName = szCurTime + "_" + szBackupFileOrgGrade;
		szBackupFilename = szBackupFilePath + "\\" +  szTempFileName;
		(*pBTfile)["BinBlock"]["TempFile Backup Name"] = szTempFileName;
		CopyFile(szOrgFilename, szBackupFilename, FALSE);
	}
	else
	{
		// main Backup Tempfile input
		szTempFileName =  (*pBTfile)["BinBlock"]["TempFile Backup Name"];
		szOrgBackupFilename = szBackupFilePath + "\\" +  szTempFileName;
		szBackupFileMainPath = gszUSER_DIRECTORY + "\\TempFileBackup"; // Main the backup path
		szBackupFileMainName = szBackupFileMainPath +  "\\" +  szTempFileName;
		
		//Copy processing TempleFile
		szTempFileName2 = szCurTime + "_" + szBackupFileOrgGrade;
		(*pBTfile)["BinBlock"]["TempFile Backup Name"] = szTempFileName2;
		szBackupFilename = szBackupFilePath + "\\" + szTempFileName2;
		CreateDirectory(szBackupFilePath, NULL);
		CopyFile(szOrgFilename, szBackupFilename, FALSE);
		
		// Copy main Backup Tempfile
		if (_access(szOrgBackupFilename, 0) != -1)
		{
			CreateDirectory(szBackupFileMainPath, NULL);
			CopyFile(szOrgBackupFilename, szBackupFileMainName, FALSE);

		}	
		else
		{
			szMsg.Format("\nThe %s  is null\n", szOrgBackupFilename);
			CMSLogFileUtility::Instance()->BT_GenTempLog(szMsg);
			return;

		}

	}

	szMsg.Format("%s -[BackupTempFile] -ulBlkId: %d, ulTempBlkId: %d, szTempFileName: %s, szTempFileName2: %s \n", szCurTime, ulBlkId, ulTempBlkId, szTempFileName, szTempFileName2);
	CMSLogFileUtility::Instance()->BT_GenTempLog(szMsg);
	(*pBTfile)["BinBlock"]["Temp Blk ID"] = ulBlkId;
	CMSFileUtility::Instance()->SaveBTConfig();
}

BOOL CBinBlk::IsFirstRowLastDieSkipDie()
{
	if (m_bEnableFirstRowColSkipPattern == FALSE)
	{
		return FALSE;
	}

	if (m_ulFirstRowColSkipUnit == 0)
	{
		return FALSE;
	}

	ULONG ulWalkPath = GetWalkPath();

	if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) || 
		 (ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH) )		//v4.40T7
	{
		if ( m_ulDiePerRow % m_ulFirstRowColSkipUnit == 0)
		{
			return TRUE;
		}
	}
	else if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) || 
			  (ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_BRV_PATH) )	//v4.40T7
	{
		if ( m_ulDiePerCol %  m_ulFirstRowColSkipUnit == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

ULONG CBinBlk::GetFirstRowColSkipPatTotalSkipUnitCount(ULONG ulInputCount)
{
	ULONG ulTotalNoOfHole = 0;

	ULONG ulDiePerBlk = ulInputCount;		//m_ulDiePerBlk;	
	
	if (ulInputCount > m_ulDiePerBlk)
		ulDiePerBlk = m_ulDiePerBlk;

	for (ULONG i=1; i<=ulDiePerBlk; i++)
	{
		if (IsFirstRowColSkipBondPatternIndex(i) == TRUE)
		{
			ulTotalNoOfHole = ulTotalNoOfHole + 1;
		}
	}
	
	return ulTotalNoOfHole;
}

BOOL CBinBlk::IsFirstRowColSkipBondPatternIndex(ULONG ulIndex)
{
	LONG lBinCol, lBinRow;
	ULONG ulNewIndex;

	ulNewIndex = ulIndex + m_ulEmptyUnit;
	
	if (m_ulFirstRowColSkipUnit == 0)
	{
		return FALSE;
	}

	FindBondRowColGivenIndex(ulNewIndex, lBinRow, lBinCol);

	ULONG ulWalkPath = GetWalkPath();	//v4.49A7

	if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) || 
		 (ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH) )		//v4.40T7
	{
		if (lBinRow == 1)
		{
			if (lBinCol % m_ulFirstRowColSkipUnit == 0)
			{
				return TRUE;
			}
		}
	}
	else if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) || 
			  (ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_BRV_PATH) )	//v4.40T7
	{
		if (lBinCol == 1)
		{
			if (lBinRow % m_ulFirstRowColSkipUnit == 0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


ULONG CBinBlk::GetSkippedUnitForFirstRowColSkipPattern(ULONG ulLastIndex, ULONG ulCurrentIndex)
{
	ULONG ulSkippedUnit = 0;

	for (ULONG i = (ulLastIndex + 1); i <= ulCurrentIndex; i++)
	{
		if (IsFirstRowColSkipBondPatternIndex(i) == TRUE)
		{
			ulSkippedUnit = ulSkippedUnit + 1;
		}
	}

	return ulSkippedUnit;
}


BOOL CBinBlk::GetFirstRowColSkipBondPatternIndex(ULONG ulOrigIndex, ULONG& ulUpdatedIndex)
{
	ULONG ulNewIndex;
	LONG lBinCol, lBinRow;
	BOOL bIndexUpdated = FALSE;

	ulNewIndex = ulOrigIndex + m_ulEmptyUnit;

	if (m_ulFirstRowColSkipUnit == 0)
	{
		ulUpdatedIndex = ulNewIndex;
		return TRUE;
	}

	FindBondRowColGivenIndex(ulNewIndex, lBinRow, lBinCol);

	ULONG ulWalkPath = GetWalkPath();		//v4.49A7

	if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ||
		 (ulWalkPath == BT_BLH_PATH) || (ulWalkPath == BT_BRH_PATH) )		//v4.40T7
	{
		if (lBinRow == 1)
		{
			if (lBinCol % m_ulFirstRowColSkipUnit == 0)
			{
				ulNewIndex = ulNewIndex + 1;
				bIndexUpdated = TRUE;
			}
		}
	}
	else if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) || 
			  (ulWalkPath == BT_BLV_PATH) || (ulWalkPath == BT_BRV_PATH) )	//v4.40T7
	{
		if (lBinCol == 1)
		{
			if (lBinRow % m_ulFirstRowColSkipUnit == 0)
			{
				ulNewIndex = ulNewIndex + 1;
				bIndexUpdated = TRUE;
			}
		}
	}

	ulUpdatedIndex = ulNewIndex;
	return bIndexUpdated;
}


BOOL CBinBlk::GetLSBondIndex(ULONG ulOrigIndex, ULONG &ulLSBondIndex)
{
	ULONG ulNewIndex;
	LONG lBinCol, lBinRow;
	BOOL bIndexUpdated = FALSE;

	//v4.35T2	//PLLM Lumiramic MS109
	if (ulOrigIndex <= 1)
	{
		m_ulEmptyUnit = 0;
	}

	ulNewIndex = ulOrigIndex + m_ulEmptyUnit;
	ULONG ulWalkPath = GetWalkPath();	//v4.49A7

	while(1)
	{
		FindBondRowColGivenIndex(ulNewIndex, lBinRow, lBinCol);

		if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ) //TL-Horizontal or TR-Horizontal
		{
			if ( lBinRow == 1 )
			{
				if ( lBinCol == 2 )
				{
					break;
				}
				else
				{
					m_ulEmptyUnit++;
					bIndexUpdated = TRUE;
				}
			}
			else
			{
				if ( lBinRow >= 3 )
				{
					if ( lBinCol > 1 )
					{
						break;
					}
					else
					{
						m_ulEmptyUnit++;
						bIndexUpdated = TRUE;
					}
				}
				else
				{
					break;
				}
			}
		}
		else if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) ) //TL-Vertical or TR-Vertical
		{
			if ( lBinCol == 1 )
			{
				if ( lBinRow == 2 )
				{
					break;
				}
				else
				{
					m_ulEmptyUnit++;
					bIndexUpdated = TRUE;
				}
			}
			else
			{
				if ( lBinCol >= 3 )
				{
					if ( lBinRow > 1 )
					{	
						break;
					}
					else
					{
						m_ulEmptyUnit++;
						bIndexUpdated = TRUE;
					}
				}
				else
				{
					break;
				}
			}
		}

		ulNewIndex++;
	}

	ulLSBondIndex = ulNewIndex;
	return bIndexUpdated;
}


ULONG CBinBlk::RecalculateLSBondIndex(ULONG ulDieCount)
{
	ULONG ulLSBondIndex = 0;
	ULONG ulIndex = 1;
	ULONG ulCount = 0;
	LONG lBinCol, lBinRow;

	ULONG ulOrigUnit = m_ulEmptyUnit;
	m_ulEmptyUnit = 0;		//Re-calculate this EMPTY unit value
	ULONG ulWalkPath = GetWalkPath();	//v4.49A7

	//** e.g. LS Bond sequence with TLH path, DiePerRow = 10 **//
	//
	//     1								//Row #1	
	// 11 10  9  8  7  6  5  4  3  2		//Row #2
	//    12 13 14 15 16 17 18 19 20		//ROw #3
	//    29 28 27 26 25 24 23 22 21		//Row #4
	//    31 32 33 .......
	//

	while (ulCount < ulDieCount)
	{
		FindBondRowColGivenIndex(ulIndex, lBinRow, lBinCol);

		if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ) //TL-Horizontal or TR-Horizontal
		{
			if ( lBinRow == 1 )				//Row #1
			{
				if ( lBinCol == 2 )			//Only 2nd Index has 1st die
				{
					ulCount++;
				}
				else
				{
					m_ulEmptyUnit++;
				}
			}
			else
			{
				if ( lBinRow >= 3 )			//Row #3 ........
				{
					if ( lBinCol > 1 )
					{
						ulCount++;
					}
					else
					{
						m_ulEmptyUnit++;	//1st column is always an EMPTY unit
					}
				}
				else						//Row #2 - no EMPTY index required
				{
					ulCount++;
				}
			}
		}
		else if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) ) //TL-Vertical or TR-Vertical
		{
			if ( lBinCol == 1 )
			{
				if ( lBinRow == 2 )
				{
					ulCount++;
				}
				else
				{
					m_ulEmptyUnit++;
				}
			}
			else
			{
				if ( lBinCol >= 3 )
				{
					if ( lBinRow > 1 )
					{	
						ulCount++;
					}
					else
					{
						m_ulEmptyUnit++;
					}
				}
				else
				{
					ulCount++;
				}
			}
		}

		ulIndex++;
	}

	ulLSBondIndex = ulIndex;
	return TRUE;
}


ULONG CBinBlk::GetOriginalIndex(ULONG ulIndex)
{
	ULONG ulOrigIndex = 1;
	ULONG ulBondCount = 0;
	LONG lBinCol, lBinRow;
	ULONG ulWalkPath = GetWalkPath();	//v4.49A7

	if ( m_ulEmptyUnit == 0 )
	{
		ulOrigIndex = ulIndex;
		return ulOrigIndex;
	}

	while(1)
	{
		FindBondRowColGivenIndex(ulOrigIndex, lBinRow, lBinCol);

		if ( (ulWalkPath == BT_TLH_PATH) || (ulWalkPath == BT_TRH_PATH) ) //TL-Horizontal or TR-Horizontal
		{
			if ( lBinRow == 1 )
			{
				if ( lBinCol == 2 )
				{
					ulBondCount++;
					if ( ulIndex == ulBondCount )
					{
						break;
					}
				}
			}
			else
			{
				if ( lBinRow >= 3 )
				{
					if ( lBinCol > 1 )
					{
						ulBondCount++;
						if ( ulIndex == ulBondCount )
						{
							break;
						}
					}
				}
				else
				{
					ulBondCount++;
					if ( ulIndex == ulBondCount )
					{
						break;
					}
				}
			}
		}
		else if ( (ulWalkPath == BT_TLV_PATH) || (ulWalkPath == BT_TRV_PATH) ) //TL-Vertical or TR-Vertical
		{
			if ( lBinCol == 1 )
			{
				if ( lBinRow == 2 )
				{
					ulBondCount++;
					if ( ulIndex == ulBondCount )
					{
						break;
					}
				}
			}
			else
			{
				if ( lBinCol >= 3 )
				{
					if ( lBinRow > 1 )
					{
						ulBondCount++;
						if ( ulIndex == ulBondCount )
						{
							break;
						}
					}
				}
				else
				{
					ulBondCount++;
					if ( ulIndex == ulBondCount )
					{
						break;
					}
				}
			}
		}

		ulOrigIndex++;
	}

	return ulOrigIndex;
}


BOOL CBinBlk::AppendInfoInWaferID(CString& szWaferId, CString szCustomer)
{
	const CMapStringToString *szMapHeaderInfo;
	szMapHeaderInfo = CMS896AStn::m_WaferMapWrapper.GetHeaderInfo();

	if (szMapHeaderInfo != NULL)
	{
		//if (szCustomer == "Nichia")						//v4.40T13
		if ((szCustomer == CTM_NICHIA) && CMS896AStn::m_oNichiaSubSystem.IsEnabled())		//v4.59A34
		{
			CString szLotNo, szLotValue;
			szLotNo.Format("%s", ucaMapHeaderLotNo);
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szLotNo, szLotValue);

			CString szDevice, szDeviceValue;
			szDevice.Format("%s", ucaMapHeaderSortTitle);
			CMS896AStn::m_WaferMapWrapper.GetReader()->GetMapData()->Lookup(szDevice, szDeviceValue);

			szWaferId = szWaferId + ":" + szLotValue + ":" + szDeviceValue;		
		}
		else if (szCustomer == "EverVision")
		{
			CString szPartNo,szCLotNo,szPLotNo,szWIPNo;
			szMapHeaderInfo->Lookup(WT_PLOT_NO, szPLotNo);
			szPLotNo.Remove(',');
			//szWaferId = szWaferId + ":" + szPLotNo;

			szMapHeaderInfo->Lookup(WT_WIP_NO, szWIPNo);
			szWIPNo.Remove(',');
			//szWaferId = szWaferId + ":" + szWIPNo;

			szMapHeaderInfo->Lookup(WT_PART_NO, szPartNo);
			szPartNo.Remove(',');
			//	szWaferId = szWaferId + ":" + szPartNo;

			szMapHeaderInfo->Lookup(WT_CLOT_NO, szCLotNo);
			szCLotNo.Remove(',');
			//	szWaferId = szWaferId + ":" + szCLotNo;

			szWaferId = szWaferId + ":" + szPLotNo + ":" + szWIPNo + ":" + szPartNo + ":" + szCLotNo;
		}
		else if (szCustomer == "LiteStar")
		{
			CString szCodeNumber = "";
			CString szSerialNumber = "";

			szMapHeaderInfo->Lookup(WT_MAP_HEADER_CODENUMBER, szCodeNumber);
			szCodeNumber.Remove(',');
				
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_SERIALNUMBER, szSerialNumber);
			szSerialNumber.Remove(',');

			szWaferId = szWaferId + ":" + szCodeNumber + ":" + szSerialNumber;
		}
		else if (szCustomer == "GeneLite")
		{
			szWaferId = szWaferId + ":" + 
						CMS896AStn::m_WaferMapWrapper.GetReader()->GetHeader().GetLotID();
		}
		else if (szCustomer == "LatticePower")
		{
			CString szLotNo, szEPID, szDeviceNumber, szResortingBin;
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_LOTNUMBER, szLotNo);
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_EPIID, szEPID);
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_DEVICENUMBER, szDeviceNumber);
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_RESORTINGBIN, szResortingBin);
				
			szLotNo.Trim(",\"");
			szEPID.Trim(",\"");
			szDeviceNumber.Trim(",\"");
			szResortingBin.Trim(",\"");

			szWaferId = szWaferId + ":" + szLotNo + ":" + szEPID + ":" + szDeviceNumber + ":" + szResortingBin; 
			//szWaferId = szWaferId + ":" + "aaaa:bbbb:cccc" ;
		}
		else if (szCustomer == "Galaxia")
		{
			szWaferId = szWaferId + ":" + CMS896AStn::m_WaferMapWrapper.GetReader()->GetHeader().GetLotID();
				
		}
		else if (szCustomer == "SeoulOpto")
		{
			szWaferId = szWaferId + ":" + CMS896AStn::m_WaferMapWrapper.GetReader()->GetHeader().GetLotID();
		}
		else if (szCustomer == "HPO")
		{
			CString szType, szSortBinItem;
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_TYPE, szType);
			CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup(WT_MAP_HEADER_SORTBIN, szSortBinItem);
			szWaferId = szWaferId + ":" + szType + ":" + szSortBinItem;
		}
		else if ( (szCustomer == "APlusEpi") || (szCustomer == "DingCheng") )		//v4.13T2
		{
			CString szSapphireID = "";
			szMapHeaderInfo->Lookup("Sapphire ID", szSapphireID);
			if (szSapphireID != "")
				szWaferId = szWaferId + ":" + szSapphireID;
		}
		// for FatcAplusEpi
		else if (szCustomer == "FATC")
		{
			CString szSapphireID = "";
			szMapHeaderInfo->Lookup("Sapphire ID", szSapphireID);

			if (szSapphireID != "")
			{
				szWaferId = szWaferId + ":" + szSapphireID;
			}
		}
		// for TyntekAplusEpi
		else if (szCustomer == "Tyntek")
		{
			CString szSapphireID = "";
			szMapHeaderInfo->Lookup("Sapphire ID", szSapphireID);

			if (szSapphireID != "")
			{
				szWaferId = szWaferId + ":" + szSapphireID;
			}
		}
		else if (szCustomer == "Aqualite")
		{
			CString szRemark3 = "";
			szMapHeaderInfo->Lookup("Remark3", szRemark3);

			szRemark3.Remove(',');

			if (szRemark3 != "")
			{
				szWaferId = szWaferId + ":" + szRemark3;
			}
		}
		else if (szCustomer == "Finisar")		//v4.57A9
		{
			CString szLayourSpecifier = "";
			szMapHeaderInfo->Lookup(WT_MAP_HEADER_FINISAR_E142_LAYOUTSPECIFIER, szLayourSpecifier);

			if (szLayourSpecifier != "")
			{
				szWaferId = szWaferId + ":" + szLayourSpecifier;
			}
		}
	}

	return TRUE;
}

VOID CBinBlk::SetMappedGrade(CString szMappedGrade)
{
	m_szMappedGrade = szMappedGrade;
}


//Generate the random hole die index for Output 2D Barcode (for Cree)				//v3.33T3
VOID CBinBlk::GenerateRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulHoleDieNum, ULONG ulInputCount)	
{
	ULONG nSegment, ulValue;
	ULONG ulQuotient=0, ulRemainder=0;		//Klocwork
	m_ulHoleDieNum = ulHoleDieNum;
	
	//v3.35
	//Need to check if Radom number is calculated from Input-COunt or max-die-count per frame
	ULONG ulDiePerBlk = ulInputCount;		//m_ulDiePerBlk;	
	if (ulInputCount > m_ulDiePerBlk)
		ulDiePerBlk = m_ulDiePerBlk;

	for (INT i = 0; i < 10; i++)
		m_ulHoleDieIndex[ulBlkToUse][i] = 0;
	
	if( ulDiePerBlk <= 0 )
		return ;
	if( m_ulDiePerCol <= 3 )
		return ;
	if( ulHoleDieNum == 0 )
		return ;
	if( ulHoleDieNum > ulDiePerBlk )
		return ;

	srand( (unsigned)time( NULL ) );
		
	for(ULONG j=0; j < ulHoleDieNum; j++)
	{
		nSegment = ulDiePerBlk/ulHoleDieNum;
		Sleep(50);
		
		ulValue = (unsigned long) (rand()%nSegment + 1 + nSegment*j);

		//szHoleDieIndex.Format(" Pre m_ulHoleDieIndex[%d][%d] = %d", ulBlkToUse, j, ulValue);
		//fprintf(fpRecord, szHoleDieIndex + "\n");
		
		if( ulValue <= 1 )
			ulValue = 2;

		if( ulValue >= (ulDiePerBlk - 2) )
			ulValue = ulDiePerBlk - 2;

		//v4.51A16	//Cree HZ
		ULONG ulWalkPath = GetWalkPath();
		if ((ulWalkPath == BT_TLV_PATH) ||
			(ulWalkPath == BT_TRV_PATH) ||
			(ulWalkPath == BT_BLV_PATH) ||
			(ulWalkPath == BT_BRV_PATH) )
		{
			if (m_ulDiePerCol != 0)	// divide by zero
			{
				ulQuotient = ulValue / m_ulDiePerCol;
				ulRemainder = ulValue % m_ulDiePerCol;
			}
		}
		else
		{
			if (m_ulDiePerRow != 0)	// divide by zero
			{
				ulQuotient = ulValue / m_ulDiePerRow;
				ulRemainder = ulValue % m_ulDiePerRow;
			}
		}

		if (ulRemainder == 0) //end row die - either the first or last in row
		{
			if (ulQuotient % 2 == 1)	//last in row
			{
				ulValue = ulValue - 1;
			}
			else						//1st in row	//v4.48A10	//Cree HuiZhou
			{
				ulValue = ulValue + 1;
			}
		}
		else if (ulRemainder == 1) //start row die - either the first or last in row
		{
			if (ulQuotient % 2 == 1)	//1st in row
			{
				ulValue = ulValue + 1;
			}
			else						//last in row	//v4.48A10	//Cree HuiZhou
			{
				ulValue = ulValue - 1;
			}
		}

		m_ulHoleDieIndex[ulBlkToUse][j] = ulValue;
	}
}


//Get the random hole index generated (for Cree)		//v3.33T3
ULONG CBinBlk::GetRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulIndex)
{
	return	m_ulHoleDieIndex[ulBlkToUse][ulIndex];
}

VOID CBinBlk::SetRandomHoleDieIndex(ULONG ulBlkToUse, ULONG ulIndex, ULONG ulRandomNum)
{
	if (ulBlkToUse < BINBLK_MAX_SIZE)	//Klocwork	//v4.46
	{
		if (ulIndex < 10)
		{
			m_ulHoleDieIndex[ulBlkToUse][ulIndex] = ulRandomNum; 
		}
	}
}

VOID CBinBlk::SetEmptyHoleDieNum(ULONG ulHoleDieNum)
{
	m_ulHoleDieNum = ulHoleDieNum;
}

VOID CBinBlk::SetFirstRowColSkipPattern(BOOL bEnalblePattern)
{
	m_bEnableFirstRowColSkipPattern = bEnalblePattern;
}

BOOL CBinBlk::GetFirstRowColSkipPattern()
{
	return m_bEnableFirstRowColSkipPattern;
}

VOID CBinBlk::SetFirstRowColSkipUnit(ULONG ulRowColSkipUnit)
{
	m_ulFirstRowColSkipUnit = ulRowColSkipUnit;
}

ULONG CBinBlk::GetFirstRowColSkipUnit()
{
	return m_ulFirstRowColSkipUnit;
}


/******************************************/
/*    CIrcular Pattern Fcn (PLLM)         */
/******************************************/
//v3.70T2
//pllm
BOOL CBinBlk::IsWithinCircularArea(DOUBLE dDiex, DOUBLE dDiey, DOUBLE& dDist)
{
	double dLength = 0.00;
	
	double dX = dDiex - m_lCenterX;
	double dY = dDiey - m_lCenterY;

	dLength = sqrt( pow(dX, 2.0) + pow(dY, 2.0) );
	dDist = dLength;

	if (dLength <= m_lRadius)
		return TRUE;	
	return FALSE;
}


BOOL CBinBlk::FindBinTableXYCirPosnGivenIndex(ULONG ulIndex, LONG& lXPosn, LONG &lYPosn)
{
	//LONG lX=0, lY=0;
	ULONG ulQuotient=0, ulRemainder=0;
	ULONG ulBinIndex = 1;
	DOUBLE dDist = 0;
	//v4.42T5
	DOUBLE dX=0, dY=0;
	DOUBLE dDiePitchX = GetDDiePitchX();
	DOUBLE dDiePitchY = GetDDiePitchY();

	if (GetPoint5UmInDiePitchX())
		dDiePitchX = dDiePitchX + 0.5;
	if (GetPoint5UmInDiePitchY())
		dDiePitchY = dDiePitchY + 0.5;

	ulBinIndex = ulIndex + m_ulSkipUnit;

	ULONG ulCurrIndex = 1;
	ULONG ulCurrDieCount = 0;
	ULONG ulAreaTotalCount = m_ulDiePerRow * m_ulDiePerCol;

	//v4.49A9
	ULONG ulWalkPath = GetWalkPath();
	LONG lULX = m_lUpperLeftX;
	LONG lULY = m_lUpperLeftY;
	LONG lLRX = m_lLowerRightX;
	LONG lLRY = m_lLowerRightY;
	RotateBlkCorners(lULX, lULY, lLRX, lLRY);


	for (UINT i=0; i<ulAreaTotalCount; i++)
	{
		if (ulCurrDieCount == ulBinIndex)
			break;

		if (ulCurrIndex > ulAreaTotalCount)
		{
			AfxMessageBox("FindBinTableXYCirPosnGivenIndex: FALSE", MB_SYSTEMMODAL);
			return FALSE;
		}

		//Calculations below
		if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
		{
			if( m_ulDiePerRow!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerRow;
				ulRemainder = ulCurrIndex % m_ulDiePerRow;
			}

			if (ulRemainder == 0) //change row die - either the first or last in row
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					dX = lULX + dDiePitchX * m_ulDiePerRow;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in row
				{
					dX = lULX + dDiePitchX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dY = lULY + dDiePitchY * ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dX = lULX + dDiePitchX * ulRemainder;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1);
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dY =  lULY + dDiePitchY * (ulQuotient + 1);
			}
		}
		else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
		{
			if( m_ulDiePerCol!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerCol;
				ulRemainder = ulCurrIndex % m_ulDiePerCol;
			}

			if (ulRemainder == 0) //change row die - either the first or last in column
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lULY + dDiePitchY * m_ulDiePerCol;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in column
				{
					dY = lULY + dDiePitchY;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dX = lULX + dDiePitchX * ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lULY + dDiePitchY * ulRemainder;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dY = lULY + dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dX = lULX + dDiePitchX * (ulQuotient + 1);
			}
		}
		else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
		{
			if( m_ulDiePerRow!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerRow;
				ulRemainder = ulCurrIndex % m_ulDiePerRow;
			}

			if (ulRemainder == 0) //change row die - either the first or last in row
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					dX = lULX + dDiePitchX;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in row
				{
					dX = lULX + dDiePitchX * m_ulDiePerRow;
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dY = lULY + dDiePitchY * ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1);
					m_ucSortDirection	= 1;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dX = lULX + dDiePitchX * ulRemainder;
					m_ucSortDirection	= 2;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dY =  lULY + dDiePitchY * (ulQuotient + 1);
			}
		}
		else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
		{
			if( m_ulDiePerCol!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerCol;
				ulRemainder = ulCurrIndex % m_ulDiePerCol;
			}

			if (ulRemainder == 0) //change row die - either the first or last in column
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lULY + dDiePitchY * m_ulDiePerCol;
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //first in column
				{
					dY = lULY + dDiePitchY;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient + 1);
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lULY + dDiePitchY * ulRemainder;
					m_ucSortDirection	= 4;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				else //on even row
				{
					dY = lULY + dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
					m_ucSortDirection	= 3;	//0=NONE, 1=LEFT, 2=RIGHT, 3=UP, 4=DOWN
				}
				dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient);
			}
		}

		ulCurrIndex++;
		if (IsWithinCircularArea(dX, dY, dDist))
		{
			ulCurrDieCount++;
		}
		else
		{
		}
	}

	if (m_bCentralizedBondArea)
	{
		lXPosn = _round(dX + m_lCentralizedOffsetX);
		lYPosn = _round(dY + m_lCentralizedOffsetY);
	}
	else
	{
		lXPosn = _round(dX);
		lYPosn = _round(dY);
	}

	return TRUE;
	
} //end FindBinTableXYPosnGivenIndex


ULONG CBinBlk::CalculateDiePerCircularBlk()
{
	//LONG lX=0, lY=0;
	ULONG ulQuotient=0, ulRemainder=0;	//Klocwork
	ULONG ulBinIndex = 1;
	DOUBLE dDist;
	//v4.42T5
	DOUBLE dX=0, dY=0;
	DOUBLE dDiePitchX = GetDDiePitchX();
	DOUBLE dDiePitchY = GetDDiePitchY();

	if (GetPoint5UmInDiePitchX())
		dDiePitchX = dDiePitchX + 0.5;
	if (GetPoint5UmInDiePitchY())
		dDiePitchY = dDiePitchY + 0.5;

	ULONG ulCurrIndex = 1;
	ULONG ulCurrDieCount = 0;
	ULONG ulAreaTotalCount = m_ulDiePerRow * m_ulDiePerCol;

	//v4.49A9
	ULONG ulWalkPath = GetWalkPath();
	LONG lULX = m_lUpperLeftX;
	LONG lULY = m_lUpperLeftY;
	LONG lLRX = m_lLowerRightX;
	LONG lLRY = m_lLowerRightY;
	RotateBlkCorners(lULX, lULY, lLRX, lLRY);

	for (UINT i=0; i<ulAreaTotalCount; i++)
	{
		if (ulCurrIndex > ulAreaTotalCount)
		{
			return ulCurrDieCount;
		}

		//Calculations below
		if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
		{
			if( m_ulDiePerRow!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerRow;
				ulRemainder = ulCurrIndex % m_ulDiePerRow;
			}

			if (ulRemainder == 0) //change row die - either the first or last in row
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					dX = lULX + dDiePitchX * m_ulDiePerRow;
				}
				else //first in row
				{
					dX = lULX + dDiePitchX;
				}
				dY = lULY + dDiePitchY * ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dX = lULX + dDiePitchX * ulRemainder;
				}
				else //on even row
				{
					dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1);
				}
				dY =  lULY + dDiePitchY * (ulQuotient + 1);
			}
		}
		else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
		{
			if( m_ulDiePerCol!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerCol;
				ulRemainder = ulCurrIndex % m_ulDiePerCol;
			}

			if (ulRemainder == 0) //change row die - either the first or last in column
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lULY + dDiePitchY * m_ulDiePerCol;
				}
				else //first in column
				{
					dY = lULY + dDiePitchY;
				}
				dX = lULX + dDiePitchX * ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lULY + dDiePitchY * ulRemainder;
				}
				else //on even row
				{
					dY = lULY + dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
				}
				dX = lULX + dDiePitchX * (ulQuotient + 1);
			}
		}
		else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
		{
			if( m_ulDiePerRow!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerRow;
				ulRemainder = ulCurrIndex % m_ulDiePerRow;
			}

			if (ulRemainder == 0) //change row die - either the first or last in row
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					dX = lULX + dDiePitchX;
				}
				else //first in row
				{
					dX = lULX + dDiePitchX * m_ulDiePerRow;
				}
				dY = lULY + dDiePitchY * ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1);
				}
				else //on even row
				{
					dX = lULX + dDiePitchX * ulRemainder;
				}
				dY =  lULY + dDiePitchY * (ulQuotient + 1);
			}
		}
		else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
		{
			if( m_ulDiePerCol!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerCol;
				ulRemainder = ulCurrIndex % m_ulDiePerCol;
			}

			if (ulRemainder == 0) //change row die - either the first or last in column
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lULY + dDiePitchY * m_ulDiePerCol;
				}
				else //first in column
				{
					dY = lULY + dDiePitchY;
				}
				dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient + 1);
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lULY + dDiePitchY * ulRemainder;
				}
				else //on even row
				{
					dY = lULY + dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
				}
				dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient);
			}
		}

		ulCurrIndex++;
		if (IsWithinCircularArea(dX, dY, dDist))
		{
			ulCurrDieCount++;
		}
		else
		{
		}
	}

	//andrew78
	//if (m_ucGrade == 1)
	//{
		//CString szTemp;
		//szTemp.Format("CalculateDiePerCircularBlk returned = %d; area total = %d(%d)", ulCurrDieCount, ulAreaTotalCount, ulCurrIndex);
		//AfxMessageBox(szTemp, MB_SYSTEMMODAL);
	//}
	return ulCurrDieCount;
}


BOOL CBinBlk::FindCirBondRowColGivenIndex(ULONG ulIndex, LONG& lRow, LONG& lCol)
{
	//LONG lX=0, lY=0;
	ULONG ulQuotient=0, ulRemainder=0;		//Klocwork
	LONG lBinRow=1, lBinCol=1;
	DOUBLE dDist;
	//v4.42T5
	DOUBLE dX=0, dY=0;
	DOUBLE dDiePitchX = GetDDiePitchX();
	DOUBLE dDiePitchY = GetDDiePitchY();

	if (GetPoint5UmInDiePitchX())
		dDiePitchX = dDiePitchX + 0.5;
	if (GetPoint5UmInDiePitchY())
		dDiePitchY = dDiePitchY + 0.5;

	ULONG ulCurrIndex = 1;
	ULONG ulCurrDieCount = 0;
	ULONG ulAreaTotalCount = m_ulDiePerRow * m_ulDiePerCol;
	ULONG ulBinIndex = ulIndex + m_ulSkipUnit;

	//v4.49A9
	ULONG ulWalkPath = GetWalkPath();
	LONG lULX = m_lUpperLeftX;
	LONG lULY = m_lUpperLeftY;
	LONG lLRX = m_lLowerRightX;
	LONG lLRY = m_lLowerRightY;
	RotateBlkCorners(lULX, lULY, lLRX, lLRY);

	lRow = 0;
	lCol = 0;

	for (UINT i=0; i<ulAreaTotalCount; i++)
	{
		if (ulCurrIndex > ulAreaTotalCount)
			return FALSE;
		if (ulCurrDieCount == ulBinIndex)
			break;

		//Calculations below
		if (ulWalkPath == BT_TLH_PATH) //TL-Horizontal
		{
			if( m_ulDiePerRow!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerRow;
				ulRemainder = ulCurrIndex % m_ulDiePerRow;
			}

			if (ulRemainder == 0) //change row die - either the first or last in row
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					dX = lULX + dDiePitchX * m_ulDiePerRow;
					lBinCol = m_ulDiePerRow;
				}
				else //first in row
				{
					dX = lULX + dDiePitchX;
					lBinCol = 1;
				}

				dY = lULY + dDiePitchY * ulQuotient;
				lBinRow = ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dX = lULX + dDiePitchX * ulRemainder;
					lBinCol = ulRemainder;
				}
				else //on even row
				{
					dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1);
					lBinCol = m_ulDiePerRow - ulRemainder + 1;
				}
				dY =  lULY + dDiePitchY * (ulQuotient + 1);
				lBinRow = ulQuotient + 1;
			}
		}
		else if (ulWalkPath == BT_TLV_PATH) //TL-Vertical
		{
			if( m_ulDiePerCol!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerCol;
				ulRemainder = ulCurrIndex % m_ulDiePerCol;
			}

			if (ulRemainder == 0) //change row die - either the first or last in column
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lULY + dDiePitchY * m_ulDiePerCol;
					lBinRow = m_ulDiePerCol;
				}
				else //first in column
				{
					dY = lULY + dDiePitchY;
					lBinRow = 1;
				}

				dX = lULX + dDiePitchX * ulQuotient;
				lBinCol = ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lULY + dDiePitchY * ulRemainder;
					lBinRow = ulRemainder;
				}
				else //on even row
				{
					dY = lULY + dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
					lBinRow = m_ulDiePerCol - ulRemainder + 1;
				}

				dX = lULX + dDiePitchX * (ulQuotient + 1);
				lBinCol = ulQuotient + 1;
			}
		}
		else if (ulWalkPath == BT_TRH_PATH)	//TR-Horizontal
		{
			if( m_ulDiePerRow!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerRow;
				ulRemainder = ulCurrIndex % m_ulDiePerRow;
			}

			if (ulRemainder == 0) //change row die - either the first or last in row
			{
				if (ulQuotient % 2 == 1) //last in row
				{
					dX = lULX + dDiePitchX;
					lBinCol = 1;
				}
				else //first in row
				{
					dX = lULX + dDiePitchX * m_ulDiePerRow;
					lBinCol = m_ulDiePerRow;
				}

				dY = lULY + dDiePitchY * ulQuotient;
				lBinRow = ulQuotient;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dX = lULX + dDiePitchX * (m_ulDiePerRow - ulRemainder + 1);
					lBinCol = m_ulDiePerRow - ulRemainder + 1;
				}
				else //on even row
				{
					dX = lULX + dDiePitchX * ulRemainder;
					lBinCol = ulRemainder;
				}

				dY =  lULY + dDiePitchY * (ulQuotient + 1);
				lBinRow = ulQuotient + 1;
			}
		}
		else if (ulWalkPath == BT_TRV_PATH) //TR-Vertical
		{
			if( m_ulDiePerCol!=0 )	// divide by zero
			{
				ulQuotient = ulCurrIndex / m_ulDiePerCol;
				ulRemainder = ulCurrIndex % m_ulDiePerCol;
			}

			if (ulRemainder == 0) //change row die - either the first or last in column
			{
				if (ulQuotient % 2 == 1) //last in column
				{
					dY = lULY + dDiePitchY * m_ulDiePerCol;
					lBinRow = m_ulDiePerCol;
				}
				else //first in column
				{
					dY = lULY + dDiePitchY;
					lBinRow = 1;
				}

				dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient + 1);
				lBinCol = m_ulDiePerRow - ulQuotient + 1;
			}
			else //die other than the change row one
			{
				if (ulQuotient % 2 == 0) //on odd row
				{
					dY = lULY + dDiePitchY * ulRemainder;
					lBinRow = ulRemainder;
				}
				else //on even row
				{
					dY = lULY + dDiePitchY * (m_ulDiePerCol - ulRemainder + 1);
					lBinRow = m_ulDiePerCol - ulRemainder + 1;
				}

				dX = lULX + dDiePitchX * (m_ulDiePerRow - ulQuotient);
				lBinCol = m_ulDiePerRow - ulQuotient;
			}
		}

		ulCurrIndex++;
		if (IsWithinCircularArea(dX, dY, dDist))
		{
			ulCurrDieCount++;
		}
		else
		{
		}
	}

	lRow = lBinRow;
	lCol = lBinCol;
	return TRUE;
}

BOOL CBinBlk::IsWafflePadSkipPos(ULONG ulIndex)
{	
	if (CMS896AStn::m_bWafflePadIdentification == FALSE)
	{
		return FALSE;
	}

	ULONG ulBinIndex, lCurPadIndex;
	ULONG ulPadIndex;

	if (GetIsEnableWafflePad() == FALSE)
	{
		return FALSE;
	}

	LONG lBondPointsInPad = m_lWafflePadSizeX * m_lWafflePadSizeY;

	ulBinIndex = ulIndex;

	if (lBondPointsInPad != 0)
	{
		if (ulBinIndex % lBondPointsInPad == 0)
		{
			lCurPadIndex = (ulBinIndex/lBondPointsInPad) - 1;
		}
		else
		{
			lCurPadIndex = (ulBinIndex/lBondPointsInPad);
		}
	}
	else
	{
		lCurPadIndex = 0;
	}
	
	// calcualate the bond point with respect to the single pad
	ulPadIndex = ulIndex - lCurPadIndex * (lBondPointsInPad);

	if (ulPadIndex == (lCurPadIndex + 1))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinBlk::GetWafflePadSkipIndex(ULONG ulIndex, ULONG& ulUpdatedIndex)
{
	if (IsWafflePadSkipPos(ulIndex))
	{
		ulUpdatedIndex = ulIndex + 1;
		return TRUE;
	}

	ulUpdatedIndex = ulIndex;
	return FALSE;
}

ULONG CBinBlk::GetSkippedUnitForWafflePad(ULONG ulLastIndex, ULONG ulCurrentIndex)
{
	ULONG ulSkippedUnit = 0;

	if (GetIsEnableWafflePad() == FALSE)
	{
		return ulSkippedUnit;
	}

	for (ULONG i = (ulLastIndex + 1); i <= ulCurrentIndex; i++)
	{
		if (IsWafflePadSkipPos(i) == TRUE)
		{
			ulSkippedUnit = ulSkippedUnit + 1;
		}
	}

	return ulSkippedUnit;
}

ULONG CBinBlk::GetWafflePadTotalSkipUnitCount(ULONG ulInputCount)
{
	ULONG ulTotalNoOfHole = 0;

	ULONG ulDiePerBlk = ulInputCount;		//m_ulDiePerBlk;	
	
	if (ulInputCount > m_ulDiePerBlk)
		ulDiePerBlk = m_ulDiePerBlk;

	for (ULONG i=1; i<=ulDiePerBlk; i++)
	{
		if (IsWafflePadSkipPos(i) == TRUE)
		{
			ulTotalNoOfHole = ulTotalNoOfHole + 1;
		}
	}
	
	return ulTotalNoOfHole;
}

DOUBLE CBinBlk::CalculateCurrBondOffsetX(ULONG ulCurrRow, ULONG ulMaxRow)
{
	if (m_dBondAreaOffsetX == 0)
		return 0;
	if (ulMaxRow == 0)
		return 0;
	DOUBLE dCurrRow = ulCurrRow;
	DOUBLE dMaxRow	= ulMaxRow;
	return (m_dBondAreaOffsetX * dCurrRow / dMaxRow);
}

DOUBLE CBinBlk::CalculateCurrBondOffsetY(ULONG ulCurrCol, ULONG ulMaxCol, UCHAR ucSortDir)
{
	if (m_dBondAreaOffsetY == 0)
		return 0;
	if (ulMaxCol == 0)
		return 0;
	DOUBLE dCurrCol = ulCurrCol;
	DOUBLE dMaxCol	= ulMaxCol;
	
	DOUBLE dOffset = 0;

	if (ucSortDir == 2)				//to RIGHT
	{
		if (ulCurrCol == 0)			//v4.57A10
			dOffset = m_dBondAreaOffsetY;
		else
			dOffset = (m_dBondAreaOffsetY * dCurrCol / dMaxCol);
	}
	else if (ucSortDir == 1)		//to LEFT
	{
		if (ulCurrCol == 0)			//v4.57A10
			dOffset = 0;
		else
			dOffset = (m_dBondAreaOffsetY * (dMaxCol - dCurrCol) / dMaxCol);
	}
	return dOffset;
}

/*
VOID CBinBlk::SetFirstRowColSkipUnit(ULONG ulSkipUnit)
{
	m_ulFirstRowColSkipUnit = ulSkipUnit;
}

ULONG CBinBlk::GetFristRowColSkipUnit()
{
	return m_ulFirstRowColSkipUnit;
}
		
ULONG GetFirstRowColSkipUnit();
*/

/****************************** Class CBinBlk End ******************************/
