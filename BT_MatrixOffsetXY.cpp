#include <stdio.h>
#include <math.h>
#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "HmiDataManager.h"
#include "BinTable.h"
#include "FileUtil.h"
#include "BT_CmdName.h"
#include "GenerateDatabase.h"
#include "TakeTime.h"
#include "MS_SecCommConstant.h"
#include "FlushMessageThread.h"
#include "MachineStat.h"
//#include "FitCircle.h"
#include "WT_SubRegion.h"

#pragma once

BOOL CBinTable::IsMatrixMarksInUse()
{
	return FALSE;
}

LONG CBinTable::XUM_ToEncoder(DOUBLE dUM_X)	//	input um, return encoder
{
// m1 is the base mark of bin frame.
// same to ASM map coordinate
//			Y-
//	x-	m1			m2	x+
//			Y+
	LONG lConvertedValue = 0;	//Klocwork

	if (dUM_X > 0)
	{
		lConvertedValue = (LONG)((dUM_X * m_dXResolution) + 0.5);
	}
	else if (dUM_X < 0)
	{
		lConvertedValue = (LONG)((dUM_X * m_dXResolution) - 0.5);
	}

	lConvertedValue = 0 - lConvertedValue; //new added
	return lConvertedValue;
}

LONG CBinTable::YUM_ToEncoder(DOUBLE dUM_Y)	//	input um, return encoder
{
// m1 is the base mark of bin frame.
// same to ASM map coordinate
//			Y-
//	x-	m1			m2	x+
//			Y+
	LONG lConvertedValue = 0;

	if (dUM_Y > 0)
	{
		lConvertedValue = (LONG)((dUM_Y * m_dYResolution) + 0.5);
	}
	else if (dUM_Y < 0)
	{
		lConvertedValue = (LONG)((dUM_Y * m_dYResolution) - 0.5);
	}

	return 0 - lConvertedValue; //new added
}


BOOL CBinTable::BM_GetNewXY(LONG &lBT_X, LONG &lBT_Y, BOOL bRotate, BOOL bVerifyOnly)
{
	LONG lOffsetX = 0, lOffsetY = 0;
	if ( BT_BM_CalculateOffset(lBT_X, lBT_Y, lOffsetX, lOffsetY , bRotate) )
	{
		//if( labs(lOffsetX) < 400 && labs(lOffsetY) < 400 )	//v4.71A20

		if (bVerifyOnly)	//v4.71A21
		{
			//Offline Verifcation cmd must within 100 motor steps
			//if ( labs(lOffsetX) <= 100 && labs(lOffsetY) <= 100 )
			//{
				lBT_X += lOffsetX;
				lBT_Y += lOffsetY;
			//}
			//else
			//{
			//	return FALSE;
			//}
		}
		else
		{
			//Sampling tol set to max. 1000 steps diff. from original BT INDEX position
			if ( labs(lOffsetX) < 1000 && labs(lOffsetY) < 1000 )
			{
				lBT_X += lOffsetX;
				lBT_Y += lOffsetY;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CBinTable::BT_BM_CalculateOffset(LONG lInBTX, LONG lInBTY, LONG &lOffsetX, LONG &lOffsetY, BOOL bRotate)
{
	lOffsetX = 0;
	lOffsetY = 0;

	if (IsErrMapInUse() == FALSE)
	{
		return FALSE;
	}

	BT_CMatrixOffsetInfo *pstErrMap;
	if (IsErrMapInUse())
	{
		if (!bRotate)
		{
			pstErrMap = &m_stErrMap1;
			//CMSLogFileUtility::Instance()->BT_TableIndexLog("N");
		}
		else
		{
			//CMSLogFileUtility::Instance()->BT_TableIndexLog("Y");
			pstErrMap = &m_stErrMap2;
		}
	}
	else
	{
		pstErrMap = &m_stBM_Data1;
	}


	if ( pstErrMap->BM_GetState() == FALSE )
	{
		return FALSE;
	}

	CString szTemp;
	ULONG ulRow11 = 0, ulCol11 = 0, ulRow21 = 0, ulCol21 = 0, ulRow12 = 0, ulCol12 = 0, ulRow22 = 0, ulCol22 = 0;
	LONG  lBTX_11 = 0, lBTY_11 = 0, lBTX_21 = 0, lBTY_21 = 0, lBTX_12 = 0, lBTY_12 = 0, lBTX_22 = 0, lBTY_22 = 0;
	LONG  lOSX_11 = 0, lOSY_11 = 0, lOSX_21 = 0, lOSY_21 = 0, lOSX_12 = 0, lOSY_12 = 0, lOSX_22 = 0, lOSY_22 = 0;

	BOOL bFindOne = FALSE;
	//	TL				TR
	//	11--------------21
	//	-----------------
	//	-----------------
	//	12--------------22
	//	BL				BR
	BOOL bFindTL = pstErrMap->BM_GetPointCornerTL(lInBTX, lInBTY, ulRow11, ulCol11);
	BOOL bFindBL = pstErrMap->BM_GetPointCornerBL(lInBTX, lInBTY, ulRow12, ulCol12);
	BOOL bFindTR = pstErrMap->BM_GetPointCornerTR(lInBTX, lInBTY, ulRow21, ulCol21);
	BOOL bFindBR = pstErrMap->BM_GetPointCornerBR(lInBTX, lInBTY, ulRow22, ulCol22);
	if( bFindTL && bFindTR && bFindBL && bFindBR )
	{
		bFindOne = TRUE;
		BOOL bGood_11 = pstErrMap->BM_GetPoint(ulRow11, ulCol11, lBTX_11, lBTY_11, lOSX_11, lOSY_11)>0;
		BOOL bGood_12 = pstErrMap->BM_GetPoint(ulRow12, ulCol12, lBTX_12, lBTY_12, lOSX_12, lOSY_12)>0;
		BOOL bGood_21 = pstErrMap->BM_GetPoint(ulRow21, ulCol21, lBTX_21, lBTY_21, lOSX_21, lOSY_21)>0;
		BOOL bGood_22 = pstErrMap->BM_GetPoint(ulRow22, ulCol22, lBTX_22, lBTY_22, lOSX_22, lOSY_22)>0;
		szTemp.Format("Point11 %d,%d good=%d,Sample(%d,%d),Offset(%d,%d)",
			ulRow11, ulCol11, bGood_11,lBTX_11, lBTY_11, lOSX_11, lOSY_11);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);

		szTemp.Format("Point12 %d,%d good=%d,Sample(%d,%d),Offset(%d,%d)",
			ulRow12, ulCol12, bGood_12, lBTX_12, lBTY_12, lOSX_12, lOSY_12);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);

		szTemp.Format("Point21 %d,%d good=%d,Sample(%d,%d),Offset(%d,%d)",
			ulRow21, ulCol21, bGood_21,lBTX_21, lBTY_21, lOSX_21, lOSY_21);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);

		szTemp.Format("Point22 %d,%d good=%d,Sample(%d,%d),Offset(%d,%d)",
			ulRow22, ulCol22, bGood_22,lBTX_22, lBTY_22, lOSX_22, lOSY_22);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);

		LONG lX1 = max(lBTX_11, lBTX_12);
		LONG lX2 = min(lBTX_21, lBTX_22);
		LONG lY1 = max(lBTY_11, lBTY_21);
		LONG lY2 = min(lBTY_12, lBTY_22);

		WT_CPO_PointsInfo *pCPInfo = WT_CPO_PointsInfo::Instance();
		lOffsetX = pCPInfo->BiLinearInterpolation(lX1, lY1, lX2, lY2, lOSX_11, lOSX_21, lOSX_12, lOSX_22, lInBTX, lInBTY);
		szTemp.Format("BT UL %d,%d LR %d,%d Offset X UL %d,%d, LR %d,%d for BT %d,%d, Xoffset %d",
			lX1, lY1, lX2, lY2, lOSX_11, lOSX_21, lOSX_12, lOSX_22, lInBTX, lInBTY, lOffsetX);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);
		lOffsetY = pCPInfo->BiLinearInterpolation(lX1, lY1, lX2, lY2, lOSY_11, lOSY_21, lOSY_12, lOSY_22, lInBTX, lInBTY);
		szTemp.Format("BT UL %d,%d LR %d,%d Offset Y UL %d,%d, LR %d,%d for BT %d,%d, Yoffset %d",
			lX1, lY1, lX2, lY2, lOSY_11, lOSY_21, lOSY_12, lOSY_22, lInBTX, lInBTY, lOffsetY);
		CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);
		return TRUE;
	}

	if ( pstErrMap->BM_GetNearestValid(lInBTX, lInBTY, ulRow11, ulCol11) )
	{
		if ( pstErrMap->BM_GetPoint(ulRow11, ulCol11, lBTX_11, lBTY_11, lOSX_11, lOSY_11) > 0 )
		{
			lOffsetX = lOSX_11;
			lOffsetY = lOSY_11;
			szTemp.Format("BT %d,%d, nearst (%2d,%2d) offset %d,%d,Corner(%d,%d,%d,%d)",
				lInBTX, lInBTY, ulRow11, ulCol11, lOffsetX, lOffsetY,bFindTL,bFindBL,bFindTR,bFindBR);
			CMSLogFileUtility::Instance()->BT_TableIndexLog(szTemp);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CBinTable::ParseRawData(CString szRawData, CStringArray& szaRawDataByCol)	//	data with , into string array;
{
	CString szData;
	INT nCol = -1;

	while((nCol = szRawData.Find(",")) != -1)
	{
		szData = szRawData.Left(nCol);
		szRawData = szRawData.Right(szRawData.GetLength() - nCol -1);
		szaRawDataByCol.Add(szData);
	}

	// add the last item
	if (szRawData != "")
	{
		szaRawDataByCol.Add(szRawData);
	}

	return TRUE;
}
