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
#include "TakeTime.h"
#include "BT_MatrixOffset.h"

#define ERR_MAP_PATH				gszUSER_DIRECTORY + _T("\\ErrMap.txt")
#define	BT_ERRMAP_TXT				"\\History\\ErrMap.csv"



VOID CBinTable::RegisterBTErrorMapVariables()
{
	try
	{
		RegVariable(_T("BT_bEnableErrMap"),				&CMS896AApp::m_bEnableErrMap);

		RegVariable(_T("BT_ulErrMapNoOfRow"),			&m_ulErrMapNoOfRow);
		RegVariable(_T("BT_ulErrMapNoOfCol"),			&m_ulErrMapNoOfCol);
		RegVariable(_T("BT_dErrMapPitchX"),				&m_dErrMapPitchX);
		RegVariable(_T("BT_dErrMapPitchY"),				&m_dErrMapPitchY);
		RegVariable(_T("BT_ulErrMapDelay"),				&m_ulErrMapDelay);
		RegVariable(_T("BT_ulErrMapCount"),				&m_ulErrMapCount);
		RegVariable(_T("BT_dErrMapSlope"),				&m_dErrMapSlope);
		RegVariable(_T("BT_dErrMapSamplingTolX"),		&m_dErrMapSamplingTolX);
		RegVariable(_T("BT_dErrMapSamplingTolY"),		&m_dErrMapSamplingTolY);
		RegVariable(_T("BT_ulErrMapSamplingTolLimit"),	&m_ulErrMapSamplingTolLimit);
		RegVariable(_T("BT_lErrMapRefX1"),				&m_lErrMapRefX1);
		RegVariable(_T("BT_lErrMapRefY1"),				&m_lErrMapRefY1);
		RegVariable(_T("BT_lErrMapRefX1InUm"),			&m_lErrMapRefX1InUm);		//andrewng //2020-0820
		RegVariable(_T("BT_lErrMapRefY1InUm"),			&m_lErrMapRefY1InUm);
		//RegVariable(_T("BT_lErrMapRefX2"),			&m_lErrMapRefX2);
		//RegVariable(_T("BT_lErrMapRefY2"),			&m_lErrMapRefY2);
		RegVariable(_T("BT_lErrMapLimitX1"),			&m_lErrMapLimitX1);
		RegVariable(_T("BT_lErrMapLimitY1"),			&m_lErrMapLimitY1);
		RegVariable(_T("BT_lErrMapLimitX2"),			&m_lErrMapLimitX2);
		RegVariable(_T("BT_lErrMapLimitY2"),			&m_lErrMapLimitY2);
		RegVariable(_T("BT_lErrMapOffsetLimitX"),		&m_lErrMapOffsetLimitX);
		RegVariable(_T("BT_lErrMapOffsetLimitY"),		&m_lErrMapOffsetLimitY);
		RegVariable(_T("BT_ulMarksPrAccLimit"),			&m_ulMarksPrAccLimit);
		RegVariable(_T("BT_ulMarksPrConLimit"),			&m_ulMarksPrConLimit);

		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapCalThetaCmd"),			&CBinTable::ErrMapCalThetaCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapSamplingCmd"),			&CBinTable::ErrMapSamplingCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapSetRefCmd"),				&CBinTable::ErrMapSetRefCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapSearchRecord"),			&CBinTable::ErrMapSearchRecord);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapVerificationCmd"),		&CBinTable::ErrMapVerificationCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapGoToRefCmd"),				&CBinTable::ErrMapGoToRefCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapLoadRecordCmd"),			&CBinTable::ErrMapLoadRecordCmd);
		m_comServer.IPC_REG_SERVICE_COMMAND(_T("ErrMapClearDataCmd"),			&CBinTable::ErrMapClearDataCmd);
		
		DisplayMessage("BinTableStn Error Map Registered...");
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}
} //end RegisterVariables

VOID CBinTable::SaveErrMapEvent(CString szMsg)
{
	//if (!CMSLogFileUtility::Instance()->GetEnableMachineLog())
	//	return;

	//g_csErrMap.Lock();

	// new file for sequencial loading
	CString szFileName = gszUSER_DIRECTORY + BT_ERRMAP_TXT;
	//if( bBackUp )
	//{
	//	CMSLogFileUtility::Instance()->CheckAndBackupFileWithTimeAtFixPath(szFileName, 2 * LOG_FILE_SIZE_LIMIT);
	//}

	FILE *pfFile;
	pfFile = fopen(szFileName, "a");
	if (pfFile != NULL)
	{
		CTime theTime = CTime::GetCurrentTime();
		fprintf(pfFile, "%2d (%2d:%2d:%2d)\t%s\n",
			theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond(), 
			(LPCTSTR) szMsg);
		fclose(pfFile);
	}
	//g_csErrMap.Unlock();
}

LONG CBinTable::ErrMapCalThetaCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;
	if (!IsErrMapInUse())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return TRUE;
	}

	ErrMapCalTheta();
	
	CString szMsg;
	szMsg.Format("Error Map Slope = %f", m_dErrMapSlope);
	HmiMessage(szMsg);

	SaveBinTableData();

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapSamplingCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;
	if (!IsErrMapInUse())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return TRUE;
	}
	BOOL bRotate = FALSE;
	CMS896AApp::m_bStopAlign = FALSE;

	if (ErrMapCalTheta())
	{
		CString szFileName;
		szFileName  = ERR_MAP_PATH;
		remove(szFileName);
		ErrMapSampling(bRotate);
	}
	
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapSetRefCmd(IPC_CServiceMessage& svMsg)
{
	CString szMsg;
	BOOL bRotate = FALSE;
	LONG lX = 0, lY = 0, lOffsetX = 0, lOffsetY = 0, lPixelX = 0, lPixelY = 0;

	GetXYEncoderValue(lX, lY);
	if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
	{
		if (!bRotate)
		{
			m_lErrMapRefX1 = lX + lOffsetX;
			m_lErrMapRefY1 = lY + lOffsetY;
			m_lErrMapRefX1InUm = ConvertXEncoderValueForDisplay(m_lErrMapRefX1);
			m_lErrMapRefY1InUm = ConvertYEncoderValueForDisplay(m_lErrMapRefY1);

			szMsg.Format("Set Ref 1,%d,%d",m_lErrMapRefX1,m_lErrMapRefY1);
			SaveErrMapEvent(szMsg);
		}
		else
		{
			//m_lErrMapRefX2 = lX + lOffsetX;
			//m_lErrMapRefY2 = lX + lOffsetY;
			//szMsg.Format("Set Ref 2,%d,%d",m_lErrMapRefX1,m_lErrMapRefY1);
			//SaveErrMapEvent(szMsg);
		}

		SaveBinTableData();
	}
	
	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapSearchRecord(IPC_CServiceMessage& svMsg)
{
	LONG lOffsetX = 0, lOffsetY = 0;
	LONG lPixelX = 0, lPixelY = 0;

	BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY);
	LONG lX = 0, lY = 0;
	GetXYEncoderValue(lX, lY);
	CString szMsg;
	szMsg.Format("Resutl,%d,%d, Current Cncord,%d,%d",lOffsetX,lOffsetY,lX,lY);
	HmiMessage(szMsg);
	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapLoadRecordCmd(IPC_CServiceMessage& svMsg)
{
	if (ErrMapLoadRecord())
	{
		HmiMessage("ErrMap:Load Record Successfully!");
	}
	else
	{
		HmiMessage("ErrMap:Load Record Fails!");
	}
	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapClearDataCmd(IPC_CServiceMessage& svMsg)
{
	BT_CMatrixOffsetInfo *pstErrMap;
	pstErrMap = &m_stErrMap1;
	pstErrMap->BM_InitPoints();
	HmiMessage("ErrMap: Clear Data Done");
	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapVerificationCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;
	if (!IsErrMapInUse())
	{
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return TRUE;
	}


	BOOL bRotate = FALSE;
	ULONG ulRefPosX = 0, ulRefPosY = 0, ulMoveDistX = 0, ulMoveDistY = 0;
	DOUBLE dAngle = atan(m_dErrMapSlope);
	CString szMsg;
	LONG lOffsetX = 0, lOffsetY = 0, lX = 0, lY = 0;
	LONG lPixelX = 0, lPixelY = 0;
	ULONG ulAccCounter = 0;
	ULONG ulConCounter = 0;
	BOOL bState = TRUE;
	DOUBLE dDistX = 0, dDistY = 0;
	BT_CMatrixOffsetInfo *pstErrMap;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//pApp->ResetStopAlign();

	SwitchToBPR();
	//BT_MoveTo(m_lErrMapRefX1,m_lErrMapRefY1);
	SetJoystickOn(TRUE);
	CString	k = "Please locate start point, then press OK to continue....";
	HmiMessageEx_Red_Back(k);
	SetErrorMessage(k);
	SetJoystickOn(FALSE);
	Sleep(500);

	LONG lEncX=0, lEncY=0;
	GetXYEncoderValue(lEncX, lEncY);


	if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
	{
		Sleep(m_ulErrMapDelay);
		if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
		{
			Sleep(m_ulErrMapDelay);
			GetXYEncoderValue(lX, lY);
			ulRefPosX = lX;
			ulRefPosY = lY;
		}
	}
	else
	{
		HmiMessage_Red_Back("ErrMap(Verification): Search Start Position Fails!");
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return FALSE;
	}


	//v4.69A1
	//LONG lEncX = 0, lEncY = 0;
	//GetXYEncoderValue(lEncX, lEncY);
	ErrMapCalTheta(TRUE);
	CString szMsg1;
	szMsg1.Format("Slope = %f",m_dErrMapSlope);
	HmiMessage(szMsg1);

	BT_MoveTo(lEncX, lEncY);
	Sleep(m_ulErrMapDelay);

	//HmiMessage("Andrew: move back to START point ...");


	if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
	{
		Sleep(m_ulErrMapDelay);
		if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
		{
			Sleep(m_ulErrMapDelay);
			GetXYEncoderValue(lX, lY);
			ulRefPosX = lX;
			ulRefPosY = lY;
		}
	}
	else
	{
		HmiMessage_Red_Back("ErrMap(Verification): Search Start Position Fails!");
		svMsg.InitMessage(sizeof(BOOL), &bReply);
		return FALSE;
	}


	LONG lPitchX = XUM_ToEncoder(m_dErrMapPitchX);//Convert To Counter
	LONG lPitchY = YUM_ToEncoder(m_dErrMapPitchY);//Convert To Counter

	if (!bRotate)
	{
		pstErrMap = &m_stErrMap1;
	}
	else
	{
		pstErrMap = &m_stErrMap2;
	}

	//v4.69A3
	LONG lOffsetErrX = 0, lOffsetErrY = 0;
	BOOL bAbortTest = FALSE;
	BOOL bValidOffset = TRUE;	//v4.71A20	//GSB MS90

	//pstErrMap->BM_InitPoints();
	//pstErrMap->BM_SetScope(FALSE, m_ulErrMapNoOfRow, m_ulErrMapNoOfCol);
	//for (ULONG ulRow = 0; ulRow < m_ulErrMapNoOfRow; ulRow++)
	//{
	//	for (ULONG ulCol = 0; ulCol < m_ulErrMapNoOfCol; ulCol++)
	//	{
	for (ULONG q = 0; q < m_ulErrMapNoOfRow; q++)//ulRow
	{
		for (ULONG p = 0; p < m_ulErrMapNoOfCol; p++)//ulCol
		{
			ULONG ulRow = q;
			ULONG ulCol = p;
			
			if (ulRow % 2 == 1)
			{
				ulCol = m_ulErrMapNoOfCol - 1 - p;
			}
			//pstErrMap->BM_SetDrawingPoint(ulRow, ulCol, ulCol * m_dErrMapPitchX, ulRow * m_dErrMapPitchY, 0);//Set Um
			//pstErrMap->BM_GetDrawingPoint(ulRow, ulCol, dDistX, dDistY);//Get Um

			dDistX =  ulCol * m_dErrMapPitchX;
			dDistY =  ulRow * m_dErrMapPitchY;

			ulMoveDistX = XUM_ToEncoder(dDistX) * cos(dAngle) - YUM_ToEncoder(dDistY) * sin(dAngle);
			ulMoveDistY = YUM_ToEncoder(dDistY) * cos(dAngle) + XUM_ToEncoder(dDistX) * sin(dAngle);
			LONG lBT_X = ulRefPosX + ulMoveDistX;
			LONG lBT_Y = ulRefPosY + ulMoveDistY;
			
			bValidOffset = BM_GetNewXY(lBT_X, lBT_Y, bRotate, TRUE);	//v4.71A21	//Add bVerifyOnly
		
			//v4.71A20	//Abort Test if a pt exceeds offset tolerance of 1000 steps 
			if (!bValidOffset)
			{
				szMsg.Format("ErrMap Verification Error: Exceed Offset Tol of 100 steps at ROW #%ld COL #%ld, ENC=(%ld, %ld)",
					q + 1, p + 1, lBT_X, lBT_Y);
				
				SetErrorMessage(szMsg);
				HmiMessage_Red_Back(szMsg);

				bAbortTest = TRUE;
				bState = FALSE;
				break;
			}


			//v4.69A3
			if (p == 0 && q == 0)
			{
				lOffsetErrX = lBT_X - ulRefPosX;
				lOffsetErrY = lBT_Y - ulRefPosY;
			}

			BT_MoveTo(lBT_X - lOffsetErrX, lBT_Y - lOffsetErrY);
			Sleep(m_ulErrMapDelay);


			BOOL bOK = BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY);

			if ( bOK == FALSE )
			{
				ulAccCounter++;
				ulConCounter++;
			}
			else
			{
				ulConCounter = 0;
			}

			CString szMsg;
			if ( ulAccCounter > 0 && ulAccCounter >= m_ulMarksPrAccLimit && m_ulMarksPrAccLimit > 0)
			{
				szMsg.Format("Search Err Map; Acc. failed %lu >= Limit %lu",
					ulAccCounter, m_ulMarksPrAccLimit);
				HmiMessage_Red_Back(szMsg);

				bAbortTest = TRUE;
				bState = FALSE;
				break;
			}
			else if ( ulConCounter > 0 && ulConCounter >= m_ulMarksPrConLimit && m_ulMarksPrConLimit > 0)
			{
				szMsg.Format("Search Err Map; Con. failed %lu >= Limit %lu",
					ulConCounter, m_ulMarksPrConLimit);
				HmiMessage_Red_Back(szMsg);

				bAbortTest = TRUE;
				bState = FALSE;
				break;
			}
			//pstErrMap->BM_SetPoint(ulRow, ulCol, lBT_X, lBT_Y, lOffsetX, lOffsetY, bOK);
			//DOUBLE dOffsetX = (DOUBLE)lOffsetX / 10.0;
			//DOUBLE dOffsetY = (DOUBLE)lOffsetY / 10.0;

			szMsg.Format("Err Map Verification,Row,%d,Col,%d,DistUm(%.2f,%.2f),DistCount(%d,%d),Angle,%f,slope,%f,MoveDist(%d,%d),BT(%d,%d),OffsetErr(%ld,%ld),Offset(%d,%d)",
				ulRow, ulCol, dDistX, dDistY, 
				XUM_ToEncoder(dDistX), YUM_ToEncoder(dDistY), dAngle, m_dErrMapSlope,
				ulMoveDistX, ulMoveDistY, lBT_X, lBT_Y, lOffsetErrX, lOffsetErrY, lOffsetX, lOffsetY);
			SaveErrMapEvent(szMsg);
			//CMSLogFileUtility::Instance()->BT_TableIndexLog(szMsg);


			if( pApp->IsStopAlign() )
			{
				svMsg.InitMessage(sizeof(BOOL), &bReply);
				return FALSE;
			}
			if ( bState == FALSE )
			{
				break;
			}
		}

		if (bAbortTest)
		{
			break;
		}
	}

	if (bState)
	{
		HmiMessage("Verification Finish!");
	}
	else
	{
		HmiMessage("Verification Fail!");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapGoToRefCmd(IPC_CServiceMessage& svMsg)
{
	BOOL bReply = TRUE;
	BT_MoveTo(m_lErrMapRefX1, m_lErrMapRefY1);
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CBinTable::ErrMapCalTheta(BOOL bUseCurrEncoderPos)
{
	if (m_ulErrMapCount < 2)
	{
		HmiMessage("Get Slop Sample No. should bigger than 2!");
		return FALSE;
	}
	CString szMsg;
	SwitchToBPR();

	if (bUseCurrEncoderPos)		//v4.69A1
	{
		LONG lX=0, lY=0;
		GetXYEncoderValue(lX, lY);
		BT_MoveTo(lX, lY);
		szMsg.Format("(Slope) Go to curr ENCODER first,%d,%d", lX, lY);
	}
	else
	{
		BT_MoveTo(m_lErrMapRefX1, m_lErrMapRefY1);
		szMsg.Format("(Slope) Go to ref Pos first,%d,%d",m_lErrMapRefX1, m_lErrMapRefY1);
	}
	
	SaveErrMapEvent(szMsg);

	LONG lOffsetX = 0, lOffsetY = 0, lX = 0, lY = 0;
	LONG lPixelX = 0, lPixelY;
	LONG lPitchX = XUM_ToEncoder(m_dErrMapPitchX);//Convert To Counter
	
	//CDWordArray aX, aY;
	CArray<double, double> aX;
	CArray<double, double> aY;

	aX.SetSize(m_ulErrMapCount);
	aY.SetSize(m_ulErrMapCount);

	BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY);
	lOffsetX = 0;
	lOffsetY = 0;
	for (int n = 0; n <= m_ulErrMapCount - 1; n++)
	{
		GetXYEncoderValue(lX, lY);
		if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
		{
			aX.SetAt(n, (double) (lX + lOffsetX));
			aY.SetAt(n, (double) (lY + lOffsetY));
			szMsg.Format("(Slope) Get Err Map Result, %d, %d,%d, %f,%f", n, lX + lOffsetX, lY + lOffsetY,
				aX.GetAt(n), aY.GetAt(n));
			SaveErrMapEvent(szMsg);
		}
		else
		{
			return FALSE;
		}

		if (n != m_ulErrMapCount - 1)
		{
			X_Move(lPitchX);
			Sleep(m_ulErrMapDelay);
		}
	}

	m_dErrMapSlope = ErrMapGetSlope(m_ulErrMapCount, aX, aY);
	return TRUE;
}

LONG CBinTable::ErrMapSampling(BOOL bRotate)
{
	ULONG ulRefPosX = 0, ulRefPosY = 0, ulMoveDistX = 0, ulMoveDistY = 0;
	DOUBLE dAngle = atan(m_dErrMapSlope);
	CString szMsg;
	LONG lOffsetX = 0, lOffsetY = 0, lX = 0, lY = 0;
	LONG lPixelX = 0, lPixelY = 0;
	ULONG ulAccCounter = 0;
	ULONG ulConCounter = 0;
	ULONG ulSampleErrTolCount = 0;		//v4.69A1
	BOOL bState = TRUE;
	DOUBLE dDistX = 0, dDistY = 0;

	BT_CMatrixOffsetInfo *pstErrMap;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//pApp->ResetStopAlign();


	SwitchToBPR();
	BT_MoveTo(m_lErrMapRefX1, m_lErrMapRefY1);
	Sleep(m_ulErrMapDelay);		//v4.69A1

	//Search PR pattern 2 times to minimize position error
	if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
	{
		Sleep(m_ulErrMapDelay);
		if (BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY) == TRUE)
		{
			Sleep(m_ulErrMapDelay);
			GetXYEncoderValue(lX, lY);
			ulRefPosX = lX;
			ulRefPosY = lY;
		}
	}
	else
	{
		HmiMessage_Red_Back("ErrMap: Search Start Position Fails!");
		return FALSE;
	}

	LONG lPitchX = XUM_ToEncoder(m_dErrMapPitchX);//Convert To Counter
	LONG lPitchY = YUM_ToEncoder(m_dErrMapPitchY);//Convert To Counter

	if (!bRotate)
	{
		pstErrMap = &m_stErrMap1;
	}
	else
	{
		pstErrMap = &m_stErrMap2;
	}

	pstErrMap->BM_InitPoints();
	pstErrMap->BM_SetScope(FALSE, m_ulErrMapNoOfRow, m_ulErrMapNoOfCol);

	BOOL bAbortTest = FALSE;


	// Move through ALL SAMPLE points to build the ERROR-MAP
	for (ULONG q = 0; q < m_ulErrMapNoOfRow; q++)	//ulRow
	{
		for (ULONG p = 0; p < m_ulErrMapNoOfCol; p++)	//ulCol
		{
			ULONG	ulRow = q;
			ULONG	ulCol = p;
			if (ulRow % 2 == 1)
			{
				ulCol = m_ulErrMapNoOfCol - 1 - p;
			}
			pstErrMap->BM_SetDrawingPoint(ulRow, ulCol, ulCol * m_dErrMapPitchX, ulRow * m_dErrMapPitchY, 0);//Set Um
			pstErrMap->BM_GetDrawingPoint(ulRow, ulCol, dDistX, dDistY);//Get Um

			ulMoveDistX = XUM_ToEncoder(dDistX) * cos(dAngle) - YUM_ToEncoder(dDistY) * sin(dAngle);
			ulMoveDistY = YUM_ToEncoder(dDistY) * cos(dAngle) + XUM_ToEncoder(dDistX) * sin(dAngle);
			LONG lBT_X = ulRefPosX + ulMoveDistX;
			LONG lBT_Y = ulRefPosY + ulMoveDistY;
			

			//==========================================
			//#1.  Move to NEXT SAMPLE point
			BT_MoveTo(lBT_X, lBT_Y);
			Sleep(m_ulErrMapDelay);

			//==========================================
			//#2.  Search PR and find offset XY
			lOffsetX = 0;
			lOffsetY = 0;
			BOOL bOK = BT_SearchErrMap(lOffsetX, lOffsetY, lPixelX, lPixelY);

			if ( bOK == FALSE )
			{
				ulAccCounter++;
				ulConCounter++;
			}
			else
			{
				ulConCounter = 0;
			}

			if (m_ulMarksPrAccLimit > 0 && ulAccCounter >= m_ulMarksPrAccLimit)
			{
				szMsg.Format("Search Err Map; Acc. failed %lu >= Limit %lu",
					ulAccCounter, m_ulMarksPrAccLimit);
				HmiMessage_Red_Back(szMsg);

				bAbortTest	= TRUE;
				bState		= FALSE;
				break;
			}
			else if (m_ulMarksPrConLimit > 0 && ulConCounter >= m_ulMarksPrConLimit)
			{
				szMsg.Format("Search Err Map; Con. failed %lu >= Limit %lu",
					ulConCounter, m_ulMarksPrConLimit);
				HmiMessage_Red_Back(szMsg);

				bAbortTest	= TRUE;
				bState		= FALSE;
				break;
			}

			//If PR finds the pattern, check Offset XY		//v4.69A1
			if (bOK)	
			{
				//Check Offset XY tolerance; if exceed, then disable this SAMPLE point
				if ( (m_dErrMapSamplingTolX > 0) && (labs(lOffsetX) > m_dErrMapSamplingTolX) )
				{
					szMsg.Format("ERROR-MAP: SAMPLE point at (%lu, %lu) exceeds X tolerance of %f motor steps (%ld)", 
							q, p, m_dErrMapSamplingTolX, lOffsetX);
					HmiMessage_Red_Back(szMsg);

					bOK = FALSE;				//disable this SAMPLE point
					ulSampleErrTolCount++;
				}
				else if ( (m_dErrMapSamplingTolY > 0) && (labs(lOffsetY) > m_dErrMapSamplingTolY) )
				{
					szMsg.Format("ERROR-MAP: SAMPLE point at (%lu, %lu) exceeds Y tolerance of %f motor steps (%ld)", 
							q, p, m_dErrMapSamplingTolY, lOffsetY);
					HmiMessage_Red_Back(szMsg);
						
					bOK = FALSE;				//disable this SAMPLE point
					ulSampleErrTolCount++;
				}
				//v4.69A1
				//If Offset XY tolerance exceeds certain limit, need to ABORT operation !!
				if ( (m_ulErrMapSamplingTolLimit > 0) && (ulSampleErrTolCount >= m_ulErrMapSamplingTolLimit) )
				{
					szMsg.Format("ERROR-MAP: XY tolerance fail count exceed limit of (%lu); operation is aborted", m_ulErrMapSamplingTolLimit);
					HmiMessage_Red_Back(szMsg);

					bAbortTest	= TRUE;
					bState		= FALSE;				//Abort operation
				}
			}


			//==========================================
			//#3.  Update ERROR MAP for the offset XY
			pstErrMap->BM_SetPoint(ulRow, ulCol, lBT_X, lBT_Y, lOffsetX, lOffsetY, bOK);
			DOUBLE dOffsetX = (DOUBLE) lOffsetX / 2.0;		//motor step to um conversion; 1 step = 0.5um
			DOUBLE dOffsetY = (DOUBLE) lOffsetY / 2.0;		//motor step to um conversion; 1 step = 0.5um

			szMsg.Format("Err Map Sampling, Row,%d,Col,%d, DistUm(%.2f,%.2f), DistCount(%d,%d), Angle,%f, slope,%f,MoveDist(%d,%d),BT(%d,%d),OffsetInStep(%d,%d),DieCenterInPixel(%d,%d)",
				ulRow, ulCol, dDistX, dDistY, 
				XUM_ToEncoder(dDistX), YUM_ToEncoder(dDistY),
				dAngle, m_dErrMapSlope, ulMoveDistX, ulMoveDistY, lBT_X, lBT_Y, lOffsetX, lOffsetY,
				lPixelX, lPixelY);
			
			//CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
			SaveErrMapEvent(szMsg);


			//==========================================
			//#4.  
			if (ErrMapGenFile(ulRow, ulCol, lBT_X, lBT_Y, lOffsetX, lOffsetY, bOK, lPixelX, lPixelY) != TRUE)
			{
				HmiMessage("Err Map Gen File Fails!");
				bState = FALSE;
				break;
			}

			if( pApp->IsStopAlign() )
			{
				return FALSE;
			}

			if ( bState == FALSE )
			{
				break;
			}
		}

		if (bAbortTest)
		{
			break;
		}
	}

	//v4.69A1
	if (bAbortTest)
	{
		bState = FALSE;
	}

	m_lErrMapLimitX1 = ulRefPosX;
	m_lErrMapLimitY1 = ulRefPosY;
	m_lErrMapLimitX2 = ulRefPosX + m_dErrMapPitchX * m_ulErrMapNoOfCol;
	m_lErrMapLimitY2 = ulRefPosX + m_dErrMapPitchY * m_ulErrMapNoOfRow;
	ErrMapSetLimitToFile(m_lErrMapLimitX1, m_lErrMapLimitY1, m_lErrMapLimitX2, m_lErrMapLimitY2);
	pstErrMap->BM_SetState(bState);
	
	if (bState)
	{
		HmiMessage("Get Sample Finish!");
	}
	else
	{
		HmiMessage("Get Sample Fail!");
	}
	return 1;
}


LONG CBinTable::ErrMapGenFile(ULONG ulRow, ULONG ulCol, LONG lX, LONG lY, LONG lOffsetX, LONG lOffsetY, BOOL bOK,
								LONG lPixelX, LONG lPixelY)
{
	CStdioFile cfErrMap;
	CFileException e;
	CString szFileName, szContent;
	szFileName  = ERR_MAP_PATH;
	//remove(szFileName);

	TRY
	{
		if (cfErrMap.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite, &e) == FALSE)
		{
			return FALSE;
		}
		else
		{
			cfErrMap.SeekToEnd();
			szContent.Format("%lu,%lu,%ld,%ld,%ld,%ld,%d,%ld,%ld", 
								ulRow, ulCol, lX, lY, lOffsetX, lOffsetY, bOK, lPixelX, lPixelY);
			cfErrMap.WriteString(szContent);
			cfErrMap.WriteString("\n");
			cfErrMap.Close();

			return TRUE;
		}
	}
	CATCH (CFileException, e)
	{
	} END_CATCH
	return TRUE;
}

LONG CBinTable::ErrMapSetLimitToFile(LONG X1,LONG Y1,LONG X2,LONG Y2)
{
	CStdioFile cfErrMap;
	CFileException e;
	CString szFileName, szContent;
	szFileName  = ERR_MAP_PATH;
	//remove(szFileName);
	TRY
	{
		if (cfErrMap.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite, &e) == FALSE)
		{
			return FALSE;
		}
		else
		{
			szContent.Format("Limit,%d,%d,%d,%d",X1,Y1,X2,Y2);
			cfErrMap.SeekToEnd();
			cfErrMap.WriteString(szContent);
			cfErrMap.WriteString("\n");
			cfErrMap.Close();

			return TRUE;
		}
	}
	CATCH (CFileException, e)
	{
	} END_CATCH
	return TRUE;
}


LONG CBinTable::ErrMapLoadRecord()
{
	if (!IsErrMapInUse())
	{
		return TRUE;
	}
	CStdioFile cfErrMap;
	CFileException e;
	CString szFileName, szContent,szMsg;
	szFileName  = ERR_MAP_PATH;
	//remove(szFileName);

	BT_CMatrixOffsetInfo *pstErrMap;
	pstErrMap = &m_stErrMap1;
	CStringArray szAList;
	ULONG ulRow,ulCol;
	LONG lBT_X,lBT_Y,lOffsetX,lOffsetY;
	BOOL bOK, bState = FALSE;
	TRY
	{
		if (_access(ERR_MAP_PATH, 0) == -1)
		{
			return FALSE;
		}
		if (cfErrMap.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead, &e) == FALSE)
		{
			return FALSE;
		}
		else
		{
			bState = TRUE;
			while( cfErrMap.ReadString(szContent))
			{
				szAList.RemoveAll();
				ParseRawData(szContent, szAList);
				//if( szAList.GetSize() == 7 )
				if ( szAList.GetSize() == 9 )		//andrewng //2020-0817
				{
					ulRow		= atoi((LPCTSTR)szAList.GetAt(0));
					ulCol		= atoi((LPCTSTR)szAList.GetAt(1));
					lBT_X		= atoi((LPCTSTR)szAList.GetAt(2));
					lBT_Y		= atoi((LPCTSTR)szAList.GetAt(3));
					lOffsetX	= atoi((LPCTSTR)szAList.GetAt(4));
					lOffsetY	= atoi((LPCTSTR)szAList.GetAt(5));
					bOK			= atoi((LPCTSTR)szAList.GetAt(6));
					pstErrMap->BM_SetPoint(ulRow, ulCol, lBT_X, lBT_Y, lOffsetX, lOffsetY, bOK);
					DOUBLE dOffsetX = (DOUBLE)lOffsetX / 10.0;
					DOUBLE dOffsetY = (DOUBLE)lOffsetY / 10.0;

					szMsg.Format("Err Map Load Record,Row,%d,Col,%d,BT(%d,%d),Offset(%d,%d),OK,%d",
						ulRow, ulCol, lBT_X, lBT_Y, lOffsetX, lOffsetY, bOK);
					SaveErrMapEvent(szMsg);
				}

				if (szAList.GetSize() == 5)
				{
					if(szAList.GetAt(0) == "Limit")
					{
						m_lErrMapLimitX1		= atoi((LPCTSTR)szAList.GetAt(1));
						m_lErrMapLimitY1		= atoi((LPCTSTR)szAList.GetAt(2));
						m_lErrMapLimitX2		= atoi((LPCTSTR)szAList.GetAt(3));
						m_lErrMapLimitX2		= atoi((LPCTSTR)szAList.GetAt(4));
						szMsg.Format("Err Map Load Record,Limit(%d,%d,%d,%d)",
						m_lErrMapLimitX1, m_lErrMapLimitY1, m_lErrMapLimitX2, m_lErrMapLimitX2);
						SaveErrMapEvent(szMsg);
					}
				}
				
			}
			cfErrMap.Close();

			pstErrMap->BM_SetState(bState);
			return TRUE;
		}
	}
	CATCH (CFileException, e)
	{
	} END_CATCH
	return TRUE;
}

DOUBLE CBinTable::ErrMapGetSlope(LONG lIndex, CArray<double, double> &aX, CArray<double, double> &aY)
{
	//formula refers from https://classroom.synonym.com/calculate-trendline-2709.html
	DOUBLE a = 0, b = 0, c = 0, d = 0;
	DOUBLE a1 = 0, b1 = 0, b2 = 0, c1 = 0, d1 = 0;
	DOUBLE dX = 0, dY = 0;
	DOUBLE dSlope = 0,dIndex;
	CString szMsg;
	
	dIndex = (DOUBLE)lIndex;
	for (int n = 0; n <= lIndex - 1; n++)
	{
		dX = (DOUBLE) aX.GetAt(n);
		dY = (DOUBLE) aY.GetAt(n);
		//a
		a1 = a1 + dX * dY;

		//b
		b1 = b1 + dX;
		b2 = b2 + dY;

		//c
		c1 = c1 + dX * dX;

		//d
		d1 = d1 + dX;
		szMsg.Format("(Cal)1 Get Err Map Result,%d,%f,%f,%f,%f,%f,%f,%f",n,dX,dY,a1,b1,b2,c1,d1);
		SaveErrMapEvent(szMsg);
	}

	a = lIndex * a1;
	b = b1 * b2;
	c = lIndex * c1;
	d = d1 * d1;

	dSlope = (a - b) / (c - d);
	szMsg.Format("(Cal)2 ,S(%f),%f,%f,%f,%f",dSlope,a,b,c,d);
	SaveErrMapEvent(szMsg);
	return dSlope;
}

BOOL CBinTable::BT_SearchErrMap(LONG &lOffsetX, LONG &lOffsetY, LONG &lPixelX, LONG &lPixelY)
{
	typedef struct 
	{
		int	siStepX;
		int	siStepY;
		int	siPixelX;
		int	siPixelY;
		DOUBLE	dAngle;
		BOOL    bResult;
	} BPR_DIEOFFSET;

	BPR_DIEOFFSET stResult;
	lOffsetX = 0;
	lOffsetY = 0;

	IPC_CServiceMessage stMsg;
	int	nConvID = 0;
	stResult.bResult = FALSE;
	nConvID = m_comClient.SendRequest("BondPrStn", "BT_SearchRefDieErrMap", stMsg);
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID, 50000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BPR_DIEOFFSET), &stResult);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	if (stResult.bResult == FALSE)
	{
		HmiMessage_Red_Back( "Search ErrMap Record Fails" );
		return FALSE;
	}

	LONG lX = 0, lY = 0;
	GetXYEncoderValue(lX, lY);
	BT_MoveTo(lX + stResult.siStepX, lY + stResult.siStepY);
	Sleep(50);

	lOffsetX	= stResult.siStepX;
	lOffsetY	= stResult.siStepY;
	lPixelX		= stResult.siPixelX;
	lPixelY		= stResult.siPixelY;
	return TRUE;
}







