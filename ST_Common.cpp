/////////////////////////////////////////////////////////////////
// ST_Common.cpp : Common functions of the CSafety class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Monday, March 21, 2005
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
#include "Safety.h"

#include "LblPtr.h"	
#include "PtrFactory.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CSafety::LoadData(VOID)
{
	CStringMapFile pFile;

	if ( pFile.Open(MSD_SAFETY_FILE, FALSE, TRUE) == 1 )
	{
		m_bCheckLANConnection = (BOOL)(LONG)(pFile)["Safety"]["Check LAN Connection"];
		m_lBHTThermalSetPoint = (LONG)(pFile)["Safety"]["BHT Thermal Set Point"];

		pFile.Close();
		return TRUE;
	}

	CMSFileUtility::Instance()->LogFileError("Cannot Open File: " + MSD_SAFETY_FILE);

	return FALSE;
}


BOOL CSafety::SaveData(VOID)
{	
	CStringMapFile pFile;

	if ( pFile.Open(MSD_SAFETY_FILE, FALSE, TRUE) == 1 )
	{
		(pFile)["Safety"]["Check LAN Connection"] = m_bCheckLANConnection;
		(pFile)["Safety"]["BHT Thermal Set Point"] = m_lBHTThermalSetPoint;

		pFile.Update();
		pFile.Close();
		return TRUE;
	}

	CMSFileUtility::Instance()->LogFileError("Cannot Save File: " + MSD_SAFETY_FILE);

	return FALSE;
}


BOOL CSafety::PrintLabelInAutoBondMode()
{
	CStdioFile cfLabelFile;
	CString szPrinterModel;
	CString szLabelName;
	CString szContent, szContent1;
	CString szTempValue;
	CString szFormat;
	int nCol = 0;
	int nTextTop = 0;
	int nTextLeft = 0;
	int nTextBottom = 0;
	int nTextRight = 0;  
	int nRtnCode = 0;
	RECT TextRect;
	CPtrFactory PtrFactory;
	CLblPtr* pPtrObj;


	//Read file 
	if ( cfLabelFile.Open("c:\\MapSorter\\Exe\\ms899.lbe", CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText) == TRUE )		
	{
		//Create printer obj
		pPtrObj = PtrFactory.Create(0);
		if ( pPtrObj == NULL )
		{
			cfLabelFile.Close();
			return FALSE;
		}


		cfLabelFile.SeekToBegin();

		//Read Header Info
		cfLabelFile.ReadString(szContent);
		if ( szContent != "[MS899 Label File]" )
		{
			cfLabelFile.Close();
			delete pPtrObj;
			return FALSE;
		}

		//Read Printer name
		cfLabelFile.ReadString(szPrinterModel);

		//Read & Set Label name
		cfLabelFile.ReadString(szLabelName);
		pPtrObj->SelectLabel(szLabelName);

		//Read & set label angle
		cfLabelFile.ReadString(szContent);
		pPtrObj->SetRotation(atoi((LPCTSTR)szContent));

		//Read & set Text font & size
		cfLabelFile.ReadString(szContent);
		cfLabelFile.ReadString(szContent1);
		pPtrObj->SetFont(szContent, atoi((LPCTSTR)szContent1), TRUE, FALSE, FALSE, FALSE);

		//Read & Set barcode type & size
		cfLabelFile.ReadString(szContent);
		cfLabelFile.ReadString(szContent1);
		pPtrObj->SetBCType(atoi((LPCTSTR)szContent));
		pPtrObj->SetBCSize(atoi((LPCTSTR)szContent1));

		//Set Text & barcode
		pPtrObj->SetHAlign(0);
		pPtrObj->SetTextVAlign(1);
		pPtrObj->SetBCTextPos(0);


		//Read contents
		while ( cfLabelFile.ReadString(szContent) !=  NULL )
		{
			//Read Format
			nCol = szContent.Find(":");
		if( nCol!=-1 )
			szFormat = szContent.Left(nCol);
			szContent = szContent.Right(szContent.GetLength() - nCol - 1);

			//Read Top
			nCol = szContent.Find(",");
		if( nCol!=-1 )
			TextRect.top = atoi((LPCTSTR)szContent.Left(nCol));
			szContent = szContent.Right(szContent.GetLength() - nCol - 1);

			//Read Left
			nCol = szContent.Find(",");
		if( nCol!=-1 )
			TextRect.left = atoi((LPCTSTR)szContent.Left(nCol));
			szContent = szContent.Right(szContent.GetLength() - nCol - 1);

			//Read bottom
			nCol = szContent.Find(",");
		if( nCol!=-1 )
			TextRect.bottom = atoi((LPCTSTR)szContent.Left(nCol));
			szContent = szContent.Right(szContent.GetLength() - nCol - 1);

			//Read right
			nCol = szContent.Find(",");
		if( nCol!=-1 )
			TextRect.right = atoi((LPCTSTR)szContent.Left(nCol));
			szContent = szContent.Right(szContent.GetLength() - nCol - 1);

			//Add information for printer use
			if ( szFormat == "T" )
			{
				pPtrObj->AddText(szContent, TextRect);
			}
			else
			{
				pPtrObj->AddBarcode(szContent, TextRect);
			}
		}

		cfLabelFile.Close();
		pPtrObj->PrintLabel(szPrinterModel);		// Print label
/*
		do
		{
			nRtnCode = pPtrObj->GetPrinterStatus(szPrinterModel);

			if (nRtnCode == 0X00000010)	//JOB_STATUS_PRINTING
			{
				Sleep(500);
			}
			else
			{
				if (nRtnCode != 0)
				{
					if ((nRtnCode & 0X00000001) != 0)	//JOB_STATUS_PAUSED
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000002) != 0)	//JOB_STATUS_ERROR
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000004) != 0)	//JOB_STATUS_DELETING
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000008) != 0)	//JOB_STATUS_SPOOLING
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000020) != 0)	//JOB_STATUS_OFFLINE
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000040) != 0)	//JOB_STATUS_PAPEROUT
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000080) != 0)	//JOB_STATUS_PRINTED
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000100) != 0)	//JOB_STATUS_DELETED
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000200) != 0)	//JOB_STATUS_BLOCKED_DEVQ
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000400) != 0)	//JOB_STATUS_USER_INTERVENTION
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}

					if ((nRtnCode & 0X00000800) != 0)	//JOB_STATUS_RESTART
					{
						pPtrObj->AbortPrintJobs(szPrinterModel);
						delete pPtrObj;
						return FALSE;
					}
				}
				else //Success
				{
					break;
				}
			}
		} while (TRUE);
*/

		delete pPtrObj;
		return TRUE;
	}

	return FALSE;
}

BOOL CSafety::CalculateMouseClickPos(PR_COORD stDieOffset, int *siStepX, int *siStepY)
{
	PR_COORD	stRelMove;

	if ( m_lJsTableMode == 1 )
	{
		LONG lCenterX	= (*m_psmfSRam)["BondPr"]["CursorCenter"]["X"];
		LONG lCenterY	= (*m_psmfSRam)["BondPr"]["CursorCenter"]["Y"];
		stRelMove.x = (PR_WORD)lCenterX - stDieOffset.x;
		stRelMove.y = (PR_WORD)lCenterY - stDieOffset.y;
	}
	else
	{
		LONG lCenterX	= (*m_psmfSRam)["WaferPr"]["CursorCenter"]["X"];
		LONG lCenterY	= (*m_psmfSRam)["WaferPr"]["CursorCenter"]["Y"];
		stRelMove.x		= (PR_WORD)lCenterX - stDieOffset.x;
		stRelMove.y		= (PR_WORD)lCenterY - stDieOffset.y;
	}

	return (ConvertPixelToMotorStep(stRelMove, siStepX, siStepY));
}

BOOL CSafety::ConvertPixelToMotorStep(PR_COORD stPixel, int *siStepX, int *siStepY)
{
	DOUBLE dCalibX=0.0, dCalibY=0.0, dCalibXY=0.0, dCalibYX=0.0;

	if ( m_lJsTableMode == 1 )
	{
		dCalibX		= (*m_psmfSRam)["BondPr"]["Calibration"]["X"];
		dCalibY		= (*m_psmfSRam)["BondPr"]["Calibration"]["Y"];
		dCalibXY	= (*m_psmfSRam)["BondPr"]["Calibration"]["XY"];
		dCalibYX	= (*m_psmfSRam)["BondPr"]["Calibration"]["YX"];
	}
	else
	{
		dCalibX		= (*m_psmfSRam)["WaferPr"]["Calibration"]["X"];
		dCalibY		= (*m_psmfSRam)["WaferPr"]["Calibration"]["Y"];
		dCalibXY	= (*m_psmfSRam)["WaferPr"]["Calibration"]["XY"];
		dCalibYX	= (*m_psmfSRam)["WaferPr"]["Calibration"]["YX"];
		//	zoomview later for calibration for mouse joystick.
	}

	if ( (dCalibX == 0.0) || (dCalibY == 0.0) )
	{
		return FALSE;
	}
	else
	{	
		*siStepX = (int)((DOUBLE)stPixel.x * dCalibX + (DOUBLE)stPixel.y * dCalibXY);
		*siStepY = (int)((DOUBLE)stPixel.y * dCalibY + (DOUBLE)stPixel.x * dCalibYX);
		return TRUE;
	}
}

BOOL CSafety::TableMoveTo(LONG lTargetX, LONG lTargetY)
{
	if (m_bCheckWaferLimit)
	{
		if ( (m_lJsTableMode == 1) )		//BT
		{
			if (IsWithinBinTableLimit(lTargetX, lTargetY) == FALSE)
			{
				return FALSE;
			}
		}
		else
		{
			if (IsWithinInputWaferLimit(lTargetX, lTargetY) == FALSE)
			{
				return FALSE;
			}
		}
	}

	if (m_fHardware)
	{
#ifndef NU_MOTION
		SFM_CHipecChannel* pMotorX = NULL;
		SFM_CHipecChannel* pMotorY = NULL;
		
		try
		{
			if ( (m_lJsTableMode == 1) )		//BT
			{
				pMotorX = m_pInitOperation->GetHipecAcServo("BinTableStn", "srvBinTableX");
				pMotorY = m_pInitOperation->GetHipecAcServo("BinTableStn", "srvBinTableY");
			}
			else
			{
				pMotorX = m_pInitOperation->GetHipecAcServo("WaferTableStn", "srvWaferX");
				pMotorY = m_pInitOperation->GetHipecAcServo("WaferTableStn", "srvWaferY");
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}

		if ( (pMotorX == NULL) || (pMotorY == NULL) )
			return FALSE;

		if ( (m_lJsTableMode == 1) )		//BT
		{
			if ( (pMotorX->IsPowerOn() == FALSE) || (pMotorY->IsPowerOn() == FALSE) )
			{
				HmiMessage("Bin Table XY not power ON!");
				return FALSE;
			}
		}
		else
		{
			if ( (pMotorX->IsPowerOn() == FALSE) || (pMotorY->IsPowerOn() == FALSE) )
			{
				HmiMessage("Wafer Table XY not power ON!");
				return FALSE;
			}
		}

		try
		{
			if (m_bJoystickOn && m_bXJoystickOn && m_bYJoystickOn)
			{
				pMotorX->MoveTo(lTargetX, 0);
				pMotorY->MoveTo(lTargetY, 0);
				pMotorX->Synchronize(1000);
				pMotorY->Synchronize(1000);
			}
			else if (m_bXJoystickOn)
			{
				pMotorX->MoveTo(lTargetX, 0);
				pMotorX->Synchronize(1000);
			}
			else if (m_bYJoystickOn)
			{
				pMotorY->MoveTo(lTargetY, 0);
				pMotorY->Synchronize(1000);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
#else
		if (IsMotionCE())	//v4.51A17
		{
			SetErrorMessage("ST TableMoveTo - Critical Error");
			return FALSE;
		}

		try
		{
			CString szNuControlParaID = "";

			if ( (m_lJsTableMode == 1) )		//BT
			{
				if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
				{
					if ( (CMS896AStn::MotionIsPowerOn("BinTableX2Axis", &m_stBTAxis_X2) == FALSE) || 
						 (CMS896AStn::MotionIsPowerOn("BinTableY2Axis", &m_stBTAxis_Y2) == FALSE) )
					{
						HmiMessage("Bin Table 2 XY not power ON!");
						return FALSE;
					}
				}
				else
				{
					if ( (CMS896AStn::MotionIsPowerOn(JS_BT_AXIS_X, &m_stBTAxis_X) == FALSE) || 
						 (CMS896AStn::MotionIsPowerOn(JS_BT_AXIS_Y, &m_stBTAxis_Y) == FALSE) )
					{
						HmiMessage("Bin Table XY not power ON!");
						return FALSE;
					}
				}
			}
			else
			{
				if ( (CMS896AStn::MotionIsPowerOn(JS_WT_AXIS_X, &m_stWTAxis_X) == FALSE) || 
					(CMS896AStn::MotionIsPowerOn(JS_WT_AXIS_Y, &m_stWTAxis_Y) == FALSE) )
				{
					HmiMessage("Wafer Table XY not power ON!");
					return FALSE;
				}	
			}

			if ( (m_lJsTableMode == 1) )		//BT
			{
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					if (CMS896AApp::m_bMS100Plus9InchOption)
					{
						if (m_lJsBinTableInUse == 1)	//BT2	//v4.17T1
						{
							szNuControlParaID = m_stBTAxis_Y2.m_stControl[PL_DYNAMIC1].m_szID;
							CMS896AStn::MotionSelectControlParam("BinTableY2Axis", "", &m_stBTAxis_Y2, szNuControlParaID);
							//change to FAST profile for very-small dist travel (< 1mm)
							CMS896AStn::MotionSelectProfile("BinTableX2Axis", "mpfBinTableXFast", &m_stBTAxis_X2);
							CMS896AStn::MotionSelectProfile("BinTableY2Axis", "mpfBinTableYFast", &m_stBTAxis_Y2);
						}
						else	//BT1
						{
							//change to PL_Dynamic1 control for BT Y Axis, for Standard MS100
							szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC1].m_szID;
							CMS896AStn::MotionSelectControlParam(JS_BT_AXIS_Y, "", &m_stBTAxis_Y, szNuControlParaID);
							//change to slow profile
							CMS896AStn::MotionSelectProfile(JS_BT_AXIS_X, "mpfBinTableXFast", &m_stBTAxis_X);
							CMS896AStn::MotionSelectProfile(JS_BT_AXIS_Y, "mpfBinTableYFast", &m_stBTAxis_Y);
						}
					}
					else
					{
						//change to PL_Dynamic1 control for BT Y Axis, for Standard MS100
						//szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC1].m_szID;
						szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
						CMS896AStn::MotionSelectControlParam(JS_BT_AXIS_Y, "", &m_stBTAxis_Y, szNuControlParaID);
						//change to slow profile
						CMS896AStn::MotionSelectProfile(JS_BT_AXIS_X, "mpfBinTableXLow", &m_stBTAxis_X);
						CMS896AStn::MotionSelectProfile(JS_BT_AXIS_Y, "mpfBinTableYLow", &m_stBTAxis_Y);
					}
				}
			}
			else
			{
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					//change to PL_Dynamic1 control for WT Y Axis, for Standard MS100
					//szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_DYNAMIC1].m_szID;
					szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
					CMS896AStn::MotionSelectControlParam(JS_WT_AXIS_Y, "", &m_stWTAxis_Y, szNuControlParaID);
				}

				//change to slow profile
				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_X, "mpfWaferXSlow", &m_stWTAxis_X);
				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_Y, "mpfWaferYSlow", &m_stWTAxis_Y);
			}
			
			if (m_bJoystickOn && m_bXJoystickOn && m_bYJoystickOn)
			{
				if ( (m_lJsTableMode == 1) )		//BT
				{
					if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
					{
						CMS896AStn::MotionMoveTo("BinTableX2Axis", lTargetX, SFM_NOWAIT, &m_stBTAxis_X2);
						CMS896AStn::MotionMoveTo("BinTableY2Axis", lTargetY, SFM_NOWAIT, &m_stBTAxis_Y2);
						CMS896AStn::MotionSync("BinTableX2Axis", 10000, &m_stBTAxis_X2);
						CMS896AStn::MotionSync("BinTableY2Axis", 10000, &m_stBTAxis_Y2);
						//Sleep(100);
					}
					else
					{
						LONG lBT_X_ProfileType = (*m_psmfSRam)["MS896A"]["BinTableX_ProfileType"];
						LONG lBT_Y_ProfileType = (*m_psmfSRam)["MS896A"]["BinTableY_ProfileType"];
						if (lBT_X_ProfileType == MS896A_OBW_PROFILE)
						{
							CMS896AStn::MotionObwMoveTo(JS_BT_AXIS_X, lTargetX, SFM_NOWAIT, &m_stBTAxis_X);
						}
						else
						{
							CMS896AStn::MotionMoveTo(JS_BT_AXIS_X, lTargetX, SFM_NOWAIT, &m_stBTAxis_X);
						}

						if (lBT_Y_ProfileType == MS896A_OBW_PROFILE)
						{
							CMS896AStn::MotionObwMoveTo(JS_BT_AXIS_Y, lTargetY, SFM_NOWAIT, &m_stBTAxis_Y);
						}
						else
						{
							CMS896AStn::MotionMoveTo(JS_BT_AXIS_Y, lTargetY, SFM_NOWAIT, &m_stBTAxis_Y);
						}

						CMS896AStn::MotionSync(JS_BT_AXIS_X, 10000, &m_stBTAxis_X);
						CMS896AStn::MotionSync(JS_BT_AXIS_Y, 10000, &m_stBTAxis_Y);
					}
				}
				else
				{
					LONG lWT_X_ProfileType = (*m_psmfSRam)["MS896A"]["WaferTableX_ProfileType"];
					LONG lWT_Y_ProfileType = (*m_psmfSRam)["MS896A"]["WaferTableY_ProfileType"];
					if (lWT_X_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(JS_WT_AXIS_X, lTargetX, SFM_NOWAIT, &m_stWTAxis_X);
					}
					else
					{
						CMS896AStn::MotionMoveTo(JS_WT_AXIS_X, lTargetX, SFM_NOWAIT, &m_stWTAxis_X);
					}

					if (lWT_Y_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMoveTo(JS_WT_AXIS_Y, lTargetY, SFM_NOWAIT, &m_stWTAxis_Y);
					}
					else
					{
						CMS896AStn::MotionMoveTo(JS_WT_AXIS_Y, lTargetY, SFM_NOWAIT, &m_stWTAxis_Y);
					}

					CMS896AStn::MotionSync(JS_WT_AXIS_X, 10000, &m_stWTAxis_X);
					CMS896AStn::MotionSync(JS_WT_AXIS_Y, 10000, &m_stWTAxis_Y);				
				}
			}
			else if (m_bXJoystickOn)
			{
				if ( (m_lJsTableMode == 1) )		//BT
				{
					if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
					{
						CMS896AStn::MotionMoveTo("BinTableX2Axis", lTargetX, SFM_NOWAIT, &m_stBTAxis_X2);
						CMS896AStn::MotionSync("BinTableX2Axis", 10000, &m_stBTAxis_X2);
					}
					else
					{
						CMS896AStn::MotionMoveTo(JS_BT_AXIS_X, lTargetX, SFM_NOWAIT, &m_stBTAxis_X);
						CMS896AStn::MotionSync(JS_BT_AXIS_X, 10000, &m_stBTAxis_X);
					}
				}
				else
				{
					CMS896AStn::MotionMoveTo(JS_WT_AXIS_X, lTargetX, SFM_NOWAIT, &m_stWTAxis_X);
					CMS896AStn::MotionSync(JS_WT_AXIS_X, 10000, &m_stWTAxis_X);
				}
			}
			else if (m_bYJoystickOn)
			{
				if ( (m_lJsTableMode == 1) )		//BT
				{
					if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
					{
						CMS896AStn::MotionMoveTo("BinTableY2Axis", lTargetY, SFM_NOWAIT, &m_stBTAxis_Y2);
						CMS896AStn::MotionSync("BinTableY2Axis", 10000, &m_stBTAxis_Y2);
					}
					else
					{
						CMS896AStn::MotionMoveTo(JS_BT_AXIS_Y, lTargetY, SFM_NOWAIT, &m_stBTAxis_Y);
						CMS896AStn::MotionSync(JS_BT_AXIS_Y, 10000, &m_stBTAxis_Y);
					}
				}
				else
				{
					CMS896AStn::MotionMoveTo(JS_WT_AXIS_Y, lTargetY, SFM_NOWAIT, &m_stWTAxis_Y);
					CMS896AStn::MotionSync(JS_WT_AXIS_Y, 10000, &m_stWTAxis_Y);
				}
			}

			if ( (m_lJsTableMode == 1) )		//BT
			{
				//change to slow profile
				if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
				{
					CMS896AStn::MotionSelectProfile("BinTableX2Axis", "mpfBinTableXNormal", &m_stBTAxis_X2);
					CMS896AStn::MotionSelectProfile("BinTableY2Axis", "mpfBinTableYNormal", &m_stBTAxis_Y2);
				}
				else
				{
					CMS896AStn::MotionSelectProfile(JS_BT_AXIS_X, "mpfBinTableXNormal", &m_stBTAxis_X);
					CMS896AStn::MotionSelectProfile(JS_BT_AXIS_Y, "mpfBinTableYNormal", &m_stBTAxis_Y);
				}
			}
			else
			{
				//change to Normal profile
				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_X, "mpfWaferXNormal", &m_stWTAxis_X);
				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_Y, "mpfWaferYNormal", &m_stWTAxis_Y);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
#endif
	}
	return TRUE;

}

BOOL CSafety::TableMove(LONG lDiff_X, LONG lDiff_Y)
{
	LONG lEnc_X = 0;
	LONG lEnc_Y = 0;

	if (IsMotionCE())	//v4.51A17
	{
		SetErrorMessage("Mouse Drag - Critical Error");
		return FALSE;
	}

	if ( GetTableEncoder(&lEnc_X, &lEnc_Y) == FALSE )
	{
		SetErrorMessage("Mouse Drag - Get Encoder Error");
		return FALSE;
	}

	lEnc_X = lEnc_X + lDiff_X;
	lEnc_Y = lEnc_Y + lDiff_Y;


	if (m_bCheckWaferLimit)
	{
		if ( (m_lJsTableMode == 1)	)	//BT	//v4.37T11
		{
			if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
			{
				if (IsWithinBinTable2Limit(lEnc_X, lEnc_Y) == FALSE)
				{
					return FALSE;
				}
			}
			else
			{
				if (IsWithinBinTableLimit(lEnc_X, lEnc_Y) == FALSE)
				{
					return FALSE;
				}
			}
		}
		else
		{
			if (IsWithinInputWaferLimit(lEnc_X, lEnc_Y) == FALSE)
			{
				return FALSE;
			}
		}
	}

	if (m_fHardware)
	{
#ifndef NU_MOTION
		SFM_CHipecChannel* pMotorX = NULL;
		SFM_CHipecChannel* pMotorY = NULL;
		
		try
		{
			if ( (m_lJsTableMode == 1) )		//BT	//v4.37T11
			{
				pMotorX = m_pInitOperation->GetHipecAcServo("BinTableStn", "srvBinTableX");
				pMotorY = m_pInitOperation->GetHipecAcServo("BinTableStn", "srvBinTableY");
			}
			else
			{
				pMotorX = m_pInitOperation->GetHipecAcServo("WaferTableStn", "srvWaferX");
				pMotorY = m_pInitOperation->GetHipecAcServo("WaferTableStn", "srvWaferY");
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}

		if ( (pMotorX == NULL) || (pMotorY == NULL) )
			return FALSE;

		if ( (m_lJsTableMode == 1) )		//BT	//v4.37T11
		{
			if ( (pMotorX->IsPowerOn() == FALSE) || (pMotorY->IsPowerOn() == FALSE) )
			{
				HmiMessage("Bin Table XY not power ON!");
				return FALSE;
			}
		}
		else
		{
			if ( (pMotorX->IsPowerOn() == FALSE) || (pMotorY->IsPowerOn() == FALSE) )
			{
				HmiMessage("Wafer Table XY not power ON!");
				return FALSE;
			}
		}

		try
		{
			if (m_bJoystickOn && m_bXJoystickOn && m_bYJoystickOn)
			{
				if (lDiff_X != 0)
					pMotorX->Move(lDiff_X, 0);
				if (lDiff_Y != 0)
					pMotorY->Move(lDiff_Y, 0);
				if (lDiff_X != 0)
					pMotorX->Synchronize(1000);
				if (lDiff_Y != 0)
					pMotorY->Synchronize(1000);
			}
			else if (m_bXJoystickOn)
			{
				if (lDiff_X != 0)
				{
					pMotorX->Move(lDiff_X, 0);
					pMotorX->Synchronize(1000);
				}
			}
			else if (m_bYJoystickOn)
			{
				if (lDiff_Y != 0)
				{
					pMotorY->Move(lDiff_Y, 0);
					pMotorY->Synchronize(1000);
				}
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
#else
		try
		{
			CString szNuControlParaID = "";
			// check power state firstly
			if ( (m_lJsTableMode == 1)	)	//BT	//v4.37T11
			{
				if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
				{
					if ( (CMS896AStn::MotionIsPowerOn("BinTableX2Axis", &m_stBTAxis_X2) == FALSE) 
						|| (CMS896AStn::MotionIsPowerOn("BinTableY2Axis", &m_stBTAxis_Y2) == FALSE) )
					{
						HmiMessage("Bin Table 2 XY not power ON!");
						return FALSE;
					}
				}
				else
				{
					if ( (CMS896AStn::MotionIsPowerOn(JS_BT_AXIS_X, &m_stBTAxis_X) == FALSE) 
						|| (CMS896AStn::MotionIsPowerOn(JS_BT_AXIS_Y, &m_stBTAxis_Y) == FALSE) )
					{
						HmiMessage("Bin Table XY not power ON!");
						return FALSE;
					}
				}
			}
			else
			{
				if ( (CMS896AStn::MotionIsPowerOn(JS_WT_AXIS_X, &m_stWTAxis_X) == FALSE) 
					|| (CMS896AStn::MotionIsPowerOn(JS_WT_AXIS_Y, &m_stWTAxis_Y) == FALSE) )
				{
					HmiMessage("Wafer Table XY not power ON!");
					return FALSE;
				}
			}

			//change to slow profile
			if ( (m_lJsTableMode == 1) )		//BT	//v4.37T11
			{

#ifndef ALLDIESORT
				if (CMS896AApp::m_bIsPrototypeMachine == FALSE)
				{
					//change to PL_Dynamic1 control for BT Y Axis, for Standard MS100
					if (CMS896AApp::m_bMS100Plus9InchOption)
					{
						if (m_lJsBinTableInUse == 1)	//BT2	//v4.17T1
						{
							szNuControlParaID = m_stBTAxis_Y2.m_stControl[PL_DYNAMIC1].m_szID;
							CMS896AStn::MotionSelectControlParam("BinTableY2Axis", "", &m_stBTAxis_Y2, szNuControlParaID);
							//change to FAST profile for very-small dist travel (< 1mm)
							CMS896AStn::MotionSelectProfile("BinTableX2Axis", "mpfBinTableXFast", &m_stBTAxis_X2);
							CMS896AStn::MotionSelectProfile("BinTableY2Axis", "mpfBinTableYFast", &m_stBTAxis_Y2);
						}
						else	//BT1
						{
							szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC1].m_szID;
							CMS896AStn::MotionSelectControlParam(JS_BT_AXIS_Y, "", &m_stBTAxis_Y, szNuControlParaID);
							//change to slow profile
							CMS896AStn::MotionSelectProfile(JS_BT_AXIS_X, "mpfBinTableXFast", &m_stBTAxis_X);
							CMS896AStn::MotionSelectProfile(JS_BT_AXIS_Y, "mpfBinTableYFast", &m_stBTAxis_Y);
						}

					}
					else
					{
						szNuControlParaID = m_stBTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
						CMS896AStn::MotionSelectControlParam(JS_BT_AXIS_Y, "", &m_stBTAxis_Y, szNuControlParaID);
						//change to slow profile
						CMS896AStn::MotionSelectProfile(JS_BT_AXIS_X, "mpfBinTableXLow", &m_stBTAxis_X);
						CMS896AStn::MotionSelectProfile(JS_BT_AXIS_Y, "mpfBinTableYLow", &m_stBTAxis_Y);
					}
				}
#endif
			}
			else
			{

#ifndef ALLDIESORT
				if (!CMS896AApp::m_bIsPrototypeMachine && 
					!CMS896AApp::m_bMS100Plus9InchOption)	//v4.19 under MS1009Inch option JS profile should use PL_DYNAMIC instead of PL_DYNAMIC1
				{
					//change to PL_Dynamic1 control for WT Y Axis, for Standard MS100
					szNuControlParaID = m_stWTAxis_Y.m_stControl[PL_DYNAMIC].m_szID;
					CMS896AStn::MotionSelectControlParam(JS_WT_AXIS_Y, "", &m_stWTAxis_Y, szNuControlParaID);
				}
#endif

				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_X, "mpfWaferXSlow", &m_stWTAxis_X);
				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_Y, "mpfWaferYSlow", &m_stWTAxis_Y);
			}

//	move table
			if (m_bJoystickOn && m_bXJoystickOn && m_bYJoystickOn)
			{
				if ( (m_lJsTableMode == 1) )	//BT
				{
					if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
					{
						CMS896AStn::MotionMove("BinTableX2Axis", lDiff_X, SFM_NOWAIT, &m_stBTAxis_X2);
						CMS896AStn::MotionMove("BinTableY2Axis", lDiff_Y, SFM_NOWAIT, &m_stBTAxis_Y2);
						CMS896AStn::MotionSync("BinTableX2Axis", 10000, &m_stBTAxis_X2);
						CMS896AStn::MotionSync("BinTableY2Axis", 10000, &m_stBTAxis_Y2);
						//Sleep(100);
					}
					else
					{
						LONG lBT_X_ProfileType = (*m_psmfSRam)["MS896A"]["BinTableX_ProfileType"];
						LONG lBT_Y_ProfileType = (*m_psmfSRam)["MS896A"]["BinTableY_ProfileType"];
						if (lBT_X_ProfileType == MS896A_OBW_PROFILE)
						{
							CMS896AStn::MotionObwMove(JS_BT_AXIS_X, lDiff_X, SFM_NOWAIT, &m_stBTAxis_X);
						}
						else
						{
							CMS896AStn::MotionMove(JS_BT_AXIS_X, lDiff_X, SFM_NOWAIT, &m_stBTAxis_X);
						}

						if (lBT_Y_ProfileType == MS896A_OBW_PROFILE)
						{
							CMS896AStn::MotionObwMove(JS_BT_AXIS_Y, lDiff_Y, SFM_NOWAIT, &m_stBTAxis_Y);
						}
						else
						{
							CMS896AStn::MotionMove(JS_BT_AXIS_Y, lDiff_Y, SFM_NOWAIT, &m_stBTAxis_Y);
						}
						CMS896AStn::MotionSync(JS_BT_AXIS_X, 10000, &m_stBTAxis_X);
						CMS896AStn::MotionSync(JS_BT_AXIS_Y, 10000, &m_stBTAxis_Y);
					}
				}
				else
				{
					LONG lWT_X_ProfileType = (*m_psmfSRam)["MS896A"]["WaferTableX_ProfileType"];
					LONG lWT_Y_ProfileType = (*m_psmfSRam)["MS896A"]["WaferTableY_ProfileType"];
					if (lWT_X_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMove(JS_WT_AXIS_X, lDiff_X, SFM_NOWAIT, &m_stWTAxis_X);
					}
					else
					{
						CMS896AStn::MotionMove(JS_WT_AXIS_X, lDiff_X, SFM_NOWAIT, &m_stWTAxis_X);
					}

					if (lWT_Y_ProfileType == MS896A_OBW_PROFILE)
					{
						CMS896AStn::MotionObwMove(JS_WT_AXIS_Y, lDiff_Y, SFM_NOWAIT, &m_stWTAxis_Y);
					}
					else
					{
						CMS896AStn::MotionMove(JS_WT_AXIS_Y, lDiff_Y, SFM_NOWAIT, &m_stWTAxis_Y);
					}
					CMS896AStn::MotionSync(JS_WT_AXIS_X, 10000, &m_stWTAxis_X);
					CMS896AStn::MotionSync(JS_WT_AXIS_Y, 10000, &m_stWTAxis_Y);
				}
			}
			else if (m_bXJoystickOn)
			{
				if ( (m_lJsTableMode == 1) )		//BT
				{
					if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
					{
						CMS896AStn::MotionMove("BinTableX2Axis", lDiff_X, SFM_NOWAIT, &m_stBTAxis_X2);
						CMS896AStn::MotionSync("BinTableX2Axis", 10000, &m_stBTAxis_X2);
					}
					else
					{
						CMS896AStn::MotionMove(JS_BT_AXIS_X, lDiff_X, SFM_NOWAIT, &m_stBTAxis_X);
						CMS896AStn::MotionSync(JS_BT_AXIS_X, 10000, &m_stBTAxis_X);
					}
				}
				else
				{
					CMS896AStn::MotionMove(JS_WT_AXIS_X, lDiff_X, SFM_NOWAIT, &m_stWTAxis_X);
					CMS896AStn::MotionSync(JS_WT_AXIS_X, 10000, &m_stWTAxis_X);
				}
			}
			else if (m_bYJoystickOn)
			{
				if ( (m_lJsTableMode == 1) )		//BT
				{
					if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
					{
						CMS896AStn::MotionMove("BinTableY2Axis", lDiff_Y, SFM_NOWAIT, &m_stBTAxis_Y2);
						CMS896AStn::MotionSync("BinTableY2Axis", 10000, &m_stBTAxis_Y2);
					}
					else
					{
						CMS896AStn::MotionMove(JS_BT_AXIS_Y, lDiff_Y, SFM_NOWAIT, &m_stBTAxis_Y);
						CMS896AStn::MotionSync(JS_BT_AXIS_Y, 10000, &m_stBTAxis_Y);
					}
				}
				else
				{
					CMS896AStn::MotionMove(JS_WT_AXIS_Y, lDiff_Y, SFM_NOWAIT, &m_stWTAxis_Y);
					CMS896AStn::MotionSync(JS_WT_AXIS_Y, 10000, &m_stWTAxis_Y);
				}
			}

			if ( (m_lJsTableMode == 1) )		//BT
			{
				//change to slow profile
				if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
				{
					CMS896AStn::MotionSelectProfile("BinTableX2Axis", "mpfBinTableXNormal", &m_stBTAxis_X2);
					CMS896AStn::MotionSelectProfile("BinTableY2Axis", "mpfBinTableYNormal", &m_stBTAxis_Y2);
				}
				else
				{
					CMS896AStn::MotionSelectProfile(JS_BT_AXIS_X, "mpfBinTableXNormal", &m_stBTAxis_X);
					CMS896AStn::MotionSelectProfile(JS_BT_AXIS_Y, "mpfBinTableYNormal", &m_stBTAxis_Y);
				}
			}
			else
			{
				//chnage to Normal profile
				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_X, "mpfWaferXNormal", &m_stWTAxis_X);
				CMS896AStn::MotionSelectProfile(JS_WT_AXIS_Y, "mpfWaferYNormal", &m_stWTAxis_Y);
			}

			Sleep(20);
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
#endif
	}
	return TRUE;
}

BOOL CSafety::GetTableEncoder(LONG *lEnc_X, LONG *lEnc_Y)
{
	if (m_fHardware)
	{
#ifndef NU_MOTION
		SFM_CHipecChannel* pMotorX = NULL;
		SFM_CHipecChannel* pMotorY = NULL;
		
		try
		{
			if ( (m_lJsTableMode == 1)	)	//BT
			{
				pMotorX = m_pInitOperation->GetHipecAcServo("BinTableStn", "srvBinTableX");
				pMotorY = m_pInitOperation->GetHipecAcServo("BinTableStn", "srvBinTableY");
			}
			else
			{
				pMotorX = m_pInitOperation->GetHipecAcServo("WaferTableStn", "srvWaferX");
				pMotorY = m_pInitOperation->GetHipecAcServo("WaferTableStn", "srvWaferY");
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}

		if ( (pMotorX == NULL) || (pMotorY == NULL) )
			return FALSE;

		*lEnc_X = pMotorX->GetEncoderPosition();
		*lEnc_Y = pMotorY->GetEncoderPosition();
#else
		try
		{
			if ( (m_lJsTableMode == 1)	)	//BT	//v4.37T11
			{
				if (CMS896AApp::m_bMS100Plus9InchOption && (m_lJsBinTableInUse == 1))	//BT2	//v4.17T1
				{
					*lEnc_X = CMS896AStn::MotionGetEncoderPosition("BinTableX2Axis", 1, &m_stBTAxis_X2);
					*lEnc_Y = CMS896AStn::MotionGetEncoderPosition("BinTableY2Axis", 1, &m_stBTAxis_Y2);
				}
				else
				{
					*lEnc_X = CMS896AStn::MotionGetEncoderPosition(JS_BT_AXIS_X, 1, &m_stBTAxis_X);
					*lEnc_Y = CMS896AStn::MotionGetEncoderPosition(JS_BT_AXIS_Y, 1, &m_stBTAxis_Y);
				}

			}
			else
			{
				*lEnc_X = CMS896AStn::MotionGetEncoderPosition(JS_WT_AXIS_X, 1, &m_stWTAxis_X);
				*lEnc_Y = CMS896AStn::MotionGetEncoderPosition(JS_WT_AXIS_Y, 1, &m_stWTAxis_Y);
			}
		}
		catch (CAsmException e)
		{
			DisplayException(e);
			return FALSE;
		}
#endif
	}

	return TRUE;
}

VOID CSafety::UpdateStationData()
{
	m_oSafetyDataBlk.m_szEnableLanConnectionCheck = m_oSafetyDataBlk.ConvertBoolToOnOff(m_bCheckLANConnection);

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SetSafetyDataBlock(m_oSafetyDataBlk);
}

LONG CSafety::LogItems(LONG lEventNo)
{
	if (m_bEnableItemLog == FALSE)
	{
		return 1;
	}

	CString szMsg;
	
	GetLogItemsString(lEventNo, szMsg);
	SetLogItems(szMsg);

	return 1;
}

LONG CSafety::GetLogItemsString(LONG lEventNo, CString& szMsg)
{
	switch(lEventNo)
	{
	case CHECK_LAN_CONNECTION:
		if (m_bCheckLANConnection)
		{
			szMsg = "Check LAN Connection,On";
		}
		else 
		{
			szMsg = "Check LAN Connection,Off";
		}
		break;
	}

	return 1;
}

BOOL CSafety::LightTowerOperate()
{
	LONG lLevel;

	if (CMS896AApp::m_bEnableAlarmTowerBlink == TRUE)
	{
		BOOL bUpdateStatus = FALSE;
		
		if (m_bSetAlarmBlinkTimer == FALSE)
		{
			bUpdateStatus = TRUE;
		}
		else
		{
			CTimeSpan ctSpan;
			ctSpan = CTime::GetCurrentTime() - m_ctAlarmBlinkTimer;
		
			if (ctSpan.GetTotalSeconds() >= 1)
			{
				bUpdateStatus = TRUE;
			}
		}

		if (bUpdateStatus == FALSE)
		{
			return TRUE;
		}
		
		// Get the Alarm Lamp level
		lLevel = GetAlarmLamp_Status();
		
		if ( lLevel != m_lLevel )
		{
			SetLampDirect(lLevel, FALSE);
			SetAlarmLog(" - Run Blink Set Lamp to match ");
		}
		else
		{
			//v4.59A15	//Renensas MS90 to support new NO_MATERIAL blink lighting
			LONG lBlinkLevelOff = ALARM_OFF_LIGHT;
			if (lLevel == ALARM_YELLOWGREEN_NOMATERIAL)
			{
				lBlinkLevelOff = ALARM_OFF_NOMATERIAL;
			}

			SetAlarmLog(" - Run Lamp to off ");
			SetLampDirect(lBlinkLevelOff, FALSE);
		}

		m_bSetAlarmBlinkTimer = TRUE;
		m_ctAlarmBlinkTimer = CTime::GetCurrentTime();
	}
	else
	{
		// Get the Alarm Lamp level
		lLevel = GetAlarmLamp_Status();
		
		if ( lLevel != m_lLevel )
		{
			SetLampDirect(lLevel, FALSE);		
			SetAlarmLog(" - Run Set Lamp to match ");
		}

		m_bSetAlarmBlinkTimer = FALSE;
	}

	return TRUE;
}

