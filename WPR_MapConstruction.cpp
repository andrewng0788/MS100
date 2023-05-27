#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "GallerySearchController.h"
#include "WAF_CPhysicalInformation.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"
#include "LastScanPosition.h"
#include "TakeTime.h"
#include "WT_SubRegion.h"
#include "MS896A.h"
#include "WaferTable.h"
#include "WL_Constant.h"
#include "Utility.H"

#define	WPR_ADD_DIE_INDEX	-101
#define	WPR_SCAN_MAP_OFFSET	20

VOID CWaferPr::ConstructPrescanMap_BarWafer()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//Get available wafermap grade & its total count
	m_lMapScanOffsetRow = 0 - WPR_SCAN_MAP_OFFSET;	// map - scan
	m_lMapScanOffsetCol = 0 - WPR_SCAN_MAP_OFFSET;	// map - scan

	if( pApp->GetCustomerName()=="emcore" )
	{
		return ConstructPrescanMap_EmcoreBW();
	}

	BOOL bOcrBarWaferNoMap = m_bOCRBarwaferNoMap;

#ifdef	PR_INSP_MACHINE
	if( bOcrBarWaferNoMap && IsCharDieInUse() )
	{
		INT nDieNum = 0;
		// xxx file is fixed and copied from serve when do alignment.
		CString szLclFullName	= PRESCAN_RESULT_FULL_PATH + "OcrBarWafer.xxx";
		CStdioFile fCheckFile;
		if( fCheckFile.Open(szLclFullName, CFile::modeRead|CFile::shareDenyNone) )
		{
			CStringArray szaList;
			CString szRawData;
			// seek to raw data part
			while( fCheckFile.ReadString(szRawData) )
			{
				if( szRawData.CompareNoCase("Encoding,Test_BIN")==0 )
				{
					break;
				}
			}

			while( fCheckFile.ReadString(szRawData) )
			{
				szaList.RemoveAll();
				CUtility::Instance()->ParseRawData(szRawData, szaList);
				if( szaList.GetSize()>=2 )
				{
					nDieNum++;
				}
			}
			fCheckFile.Close();
		}

		if( nDieNum<=0 )
			bOcrBarWaferNoMap = FALSE;
	}
#endif

	if( bOcrBarWaferNoMap )
	{
		return ConstructPrescanMap_OcrBWNoMap();
	}

	ULONG ulIndex;
	BOOL bInfo;
	ULONG lRefDieCount = 0;
	CString szConLogFile;

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
	UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;
	UCHAR ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();

	BOOL  bBadCutAction	= m_bPrescanBadCutAction;
	BOOL  bDefectAction	= m_bPrescanDefectAction;
	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;
	BOOL  bFakeAction	= m_bScanDetectFakeEmpty;
	UCHAR ucFakeGrade	= m_ucScanFakeEmptyGrade;
	BOOL  bExtraAction  = m_bScanExtraDieAction;
	UCHAR ucExtraGrade	= m_ucScanExtraGrade + ucOffset;
	if( bExtraAction==FALSE )
		ucExtraGrade = ucNullBin;

	if( m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;
	if( m_bPrescanDefectToNullBin )
		ucDefectGrade = ucNullBin;
	if (m_bPrescanBadCutToNullBin)
		ucBadCutGrade = ucNullBin;

	CString szMsg;

	DOUBLE dUsedTime = GetTime();

	// capture last image
	LONG lEncX, lEncY;

	CDWordArray dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol;
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	ULONG ulMapRowMax, ulMapColMax;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

	ULONG ulDiePitchX = labs(GetDiePitchX_X());
	ULONG ulDiePitchY = labs(GetDiePitchY_Y());
	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();

	// add all alignment point and refer points to list
	LONG lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY, lHomeNewRow, lHomeNewCol;
	pUtl->GetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);
	lHomeNewRow = lHomeOriRow + WPR_SCAN_MAP_OFFSET;
	lHomeNewCol = lHomeOriCol + WPR_SCAN_MAP_OFFSET;

	dwaRefWfX.Add(lHomeWfX);
	dwaRefWfY.Add(lHomeWfY);
	dwaRefRow.Add(lHomeNewRow);
	dwaRefCol.Add(lHomeNewCol);
	lRefDieCount++;

	LONG lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY;
	for(ulIndex=0; ulIndex<pUtl->GetNumOfReferPoints(); ulIndex++)
	{
		if( pUtl->GetReferPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			dwaRefWfX.Add(lAlgnWfX);
			dwaRefWfY.Add(lAlgnWfY);
			lAlgnRow += WPR_SCAN_MAP_OFFSET;
			lAlgnCol += WPR_SCAN_MAP_OFFSET;
			dwaRefRow.Add(lAlgnRow);
			dwaRefCol.Add(lAlgnCol);
			lRefDieCount++;
		}
	}

	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			if( lAlgnRow==lHomeOriRow && lAlgnCol==lHomeOriCol )
				continue;
			dwaRefWfX.Add(lAlgnWfX);
			dwaRefWfY.Add(lAlgnWfY);
			lAlgnRow += WPR_SCAN_MAP_OFFSET;
			lAlgnCol += WPR_SCAN_MAP_OFFSET;
			dwaRefRow.Add(lAlgnRow);
			dwaRefCol.Add(lAlgnCol);
			lRefDieCount++;
		}
	}

	ClearGoodInfo();

	szConLogFile = szLogPath + PRESCAN_PASSIN_PSN;
	FILE *fpPassIn = NULL;
	errno_t nPassInErr = fopen_s(&fpPassIn, szConLogFile, "w");

	// add all prescan die data list to physical map builder include refer points
	for(ulIndex=0; ulIndex<lRefDieCount; ulIndex++)
	{
		lEncX = dwaRefWfX.GetAt(ulIndex);
		lEncY = dwaRefWfY.GetAt(ulIndex);
		LONG lIndex = 0-ulIndex;
		m_cPIMap.SortAdd_Tail(lEncX, lEncY, lIndex, FALSE);
		if ((nPassInErr == 0) && (fpPassIn != NULL))
		{
			fprintf(fpPassIn, "%ld,%ld,%ld,%ld,%ld\n",
				lEncX, lEncY, dwaRefRow.GetAt(ulIndex), dwaRefCol.GetAt(ulIndex), lIndex);
		}
	}

	// build physical map
	int nMaxSpan = 1;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	int nMaxIterateCount = -1;
	int nMaxAllowLeft = 10;
	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: begin pass massive data to builder");

	if ((nPassInErr == 0) && (fpPassIn != NULL))
	{
		fprintf(fpPassIn, "%lu,%lu,%lu,%lu,%d,%d,%d,%d,%ld,%ld,%ld,%ld\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY, 
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan, GetDieSizeX(), GetDieSizeY(), GetDiePitchX_X(), GetDiePitchY_Y());
	}

	if( IsPrecanWith2Pr() )	// pass raw data to PIMap builder
	{
		ULONG *pulReferBadList;
		pulReferBadList = new ULONG[WSGetPrescanTotalDie() + 10];
		memset(pulReferBadList, 0, sizeof(pulReferBadList));
		ULONG lReferBadIndex = 0;
		// refer scan, make sure good die not a refer die and 
		//						 refer die not a good die and 
		//						 refer die to refer die less 2 size and large 0.2 
		if( IsPrescanReferDie() )
		{
			for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
			{
				USHORT uwDiePrID = 0;
				if( WSGetPosnPrID(ulIndex, lEncX, lEncY, uwDiePrID)==false )
					continue;

				if( uwDiePrID==m_uwScanRecordID[0] )	// good, to check not is a refer die.
				{
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetScanReferTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						if( WSGetScanReferPosition(ulChkIndex, lChkX, lChkY)!=true )
						{
							continue;
						}

						if( abs(lEncX-lChkX)<abs(GetDieSizeX()*3/4) && abs(lEncY-lChkY)<abs(GetDieSizeY()*3/4) )
						{
							pulReferBadList[lReferBadIndex] = ulIndex;
							lReferBadIndex++;
							break;
						}
					}	// find in reference list
				}	// good, to check not is a refer die.
				else	// refer, can not be a good die and can not too near to another refer die
				{
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetScanReferTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						if( WSGetScanReferPosition(ulChkIndex, lChkX, lChkY)!=true )
						{
							continue;
						}

						if( (abs(lEncX-lChkX)>abs(GetDieSizeX()/5) || abs(lEncY-lChkY)>abs(GetDieSizeY()/5)) &&
							(abs(lEncX-lChkX)<abs(GetDieSizeX()*2) && abs(lEncY-lChkY)<abs(GetDieSizeY()*2)) )
						{
							pulReferBadList[lReferBadIndex] = ulIndex;
							lReferBadIndex++;
							break;
						}
					}	// find in reference list, not too near
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetPrescanTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						USHORT uwChkPrID = 0;
						if( WSGetPosnPrID(ulChkIndex, lChkX, lChkY, uwChkPrID)==false )
						{
							continue;
						}

						if( uwChkPrID==m_uwScanRecordID[0] )
						{
							if( abs(lEncX-lChkX)<abs(GetDieSizeX()*3/4) && abs(lEncY-lChkY)<abs(GetDieSizeY()*3/4) )
							{
								pulReferBadList[lReferBadIndex] = ulIndex;
								lReferBadIndex++;
								break;
							}
						}
					}	// check in good list, if it is a good die, skip
				}	// refer, can not be a good die and can not too near to another refer die
			}	// all list
		}	// refer die scan

		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
			{
				continue;
			}

			if( IsPrescanReferDie() && lReferBadIndex>0 )
			{
				for(ULONG ulChkIndex=0; ulChkIndex<lReferBadIndex; ulChkIndex++)
				{
					if( pulReferBadList[ulChkIndex]==ulIndex )
					{
						continue;
					}
				}
			}

			m_cPIMap.SortAdd_Tail(lEncX, lEncY, ulIndex, FALSE);
			if ((nPassInErr == 0) && (fpPassIn != NULL))
			{
				fprintf(fpPassIn, "%ld,%ld,%lu\n", lEncX, lEncY, ulIndex);
			}
		}
		delete [] pulReferBadList;
	}	//	end of 2nd pr prescan
	else
	{
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			USHORT uwDiePrID;

			if(	WSGetPosnPrID(ulIndex, lEncX, lEncY, uwDiePrID)==FALSE )
				continue;

			if ((nPassInErr == 0) && (fpPassIn != NULL))
			{
				fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
			}
		}
	}

	if ((nPassInErr == 0) && (fpPassIn != NULL))
	{
		fclose(fpPassIn);
	}

	bool *pbGetOut;
	pbGetOut = new bool[WSGetPrescanTotalDie()+10];
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		pbGetOut[ulIndex] = false;
	}

	if( IsPrecanWith2Pr() )
	{
		m_cPIMap.KeepIsolatedDice(FALSE);
		m_cPIMap.SetScan2Pr(TRUE);
	}
	else
	{
		m_cPIMap.KeepIsolatedDice(FALSE);
		m_cPIMap.SetScan2Pr(FALSE);
	}

	SaveScanTimeEvent("WPR: to construct normal map");
	if( GetDieShape()==WPR_RHOMBUS_DIE )
	{
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();
		m_cPIMap.SetXOffset(lDiePitchY_X);
		szMsg.Format("WPR: die pitch x %d,%d, y %d,%d", ulDiePitchX, lDiePitchX_Y, ulDiePitchY, lDiePitchY_X);
		SaveScanTimeEvent(szMsg);
	}
	else
	{
		m_cPIMap.SetXOffset(0);
	}

	m_cPIMap.ConstructMap(dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
		bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);
	SaveScanTimeEvent("WPR: construct map complete");

	m_lTimeSlot[7] = (LONG)(GetTime()-dUsedTime);	//	07.	constuct map used time.

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: AFTER build PHY MAP");

	ULONG ulRow, ulCol;

	SaveScanTimeEvent("WPR: to clear prescan info");
	ClearPrescanInfo();
	SaveScanTimeEvent("WPR: to reset map phy posn");
	ResetMapPhyPosn();

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: clear physical prescan and map info done");

	// get all phsical list infromation to prescan result list
	FILE *fpMap = NULL;
	CString szScanPmpFile = szLogPath + "_Scan.pmp";
	errno_t nErr = fopen_s(&fpMap, szScanPmpFile, "w");\

	BOOL bFirstGood = FALSE;
	ULONG ulScnDieMaxRow = 0, ulScnDieMaxCol = 0, ulScnDieMinRow = 9999, ulScnDieMinCol = 9999;
	ULONG ulPyiRowMax = 0, ulPyiColMax = 0;
	m_cPIMap.GetDimension(ulPyiRowMax, ulPyiColMax);

	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "PHY range Row(%lu,%lu)  Col(%lu,%lu)\n", 0, ulPyiRowMax, 0, ulPyiColMax);
		fprintf(fpMap, "MAP range Row(0,%lu)  Col(0,%lu)\n", ulMapRowMax, ulMapColMax);
		fprintf(fpMap, "MAP range valid(%lu,%lu)  (%lu,%lu)\n", 
			GetMapValidMinRow(), GetMapValidMinCol(), GetMapValidMaxRow(), GetMapValidMaxCol());
		fprintf(fpMap, "DataFileName,,%s\n", (LPCTSTR)szScanPmpFile);
		fprintf(fpMap, "LotNumber,,\nDeviceNumber,,\nwafer id=\nTestTime,\nMapFileName,,\nTransferTime,\n");
		fprintf(fpMap, "\n");
	}

	for(ulRow=0; ulRow<=ulPyiRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulPyiColMax; ulCol++)
		{
			BOOL bDieOverLap = FALSE;
			LONG lIndex = 0;
			bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bInfo==FALSE )
			{
				continue;
			}

			if( lIndex<=WPR_ADD_DIE_INDEX )
			{
				continue;
			}

			if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
			{
				continue;
			}

			UCHAR ucMapGrade = 1;
			if ((nErr == 0) && (fpMap != NULL))
			{
				if( bFirstGood==FALSE )
				{
					fprintf(fpMap, "map data\n");
					fprintf(fpMap, "%lu,%lu,\n", ulCol, ulRow);
					bFirstGood = TRUE;
				}
				fprintf(fpMap, "%lu,%lu,%d\n", ulCol, ulRow, ucMapGrade);
			}

			if( ulRow>ulScnDieMaxRow )
				ulScnDieMaxRow = ulRow;
			if( ulCol>ulScnDieMaxCol )
				ulScnDieMaxCol = ulCol;
			if( ulRow<ulScnDieMinRow )
				ulScnDieMinRow = ulRow;
			if( ulCol<ulScnDieMinCol )
				ulScnDieMinCol = ulCol;
		}
	}

	if ((nErr == 0) && (fpMap != NULL))
	{
		fclose(fpMap);
	}

	szMsg.Format("WPR: get out PHY range Row(%lu,%lu)  Col(%lu,%lu)",
		ulScnDieMinRow, ulScnDieMaxRow, ulScnDieMinCol, ulScnDieMaxCol);
	SaveScanTimeEvent(szMsg);

	ULONG ulFrameID = 0;
	DOUBLE dDieAngle;
	CString szDieBin = "0";
	szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpOut = NULL;
	errno_t nOutErr = fopen_s(&fpOut, szConLogFile, "w");

	dUsedTime = GetTime();

	LONG lDieSizeTolX = (LONG) ulDieSizeXTol;
	LONG lDieSizeTolY = (LONG) ulDieSizeYTol;
	for (ulRow = ulScnDieMinRow; ulRow <= ulScnDieMaxRow; ulRow++)
	{
		for (ulCol = ulScnDieMinCol; ulCol <= ulScnDieMaxCol; ulCol++)
		{
			LONG lIndex = 0;
			BOOL bDieOverLap = FALSE;
			bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if (bInfo == FALSE)
			{
				continue;
			}

			if (lIndex <= WPR_ADD_DIE_INDEX)
				continue;
			if (lIndex > 0)
				pbGetOut[lIndex] = true;

			LONG ulInRow = (LONG)ulRow + m_lMapScanOffsetRow;
			LONG ulInCol = (LONG)ulCol + m_lMapScanOffsetCol;

			USHORT usDieState = 0;
			PR_UWORD uwScanDiePrID = 0;
			if( lIndex>0 )
			{
				LONG lDummyX = 0, lDummyY = 0;
				WSGetScanPosition(lIndex, lDummyX, lDummyY, dDieAngle, szDieBin, usDieState, uwScanDiePrID, ulFrameID);
			}
			else
			{
				dDieAngle = 0.0;
				szDieBin = "0";
				uwScanDiePrID = 0;
				if( ulRow==lHomeNewRow && ulCol==lHomeNewCol &&
					lEncX==lHomeWfX && lEncY==lHomeWfY )
				{
					ulFrameID = pUtl->GetAlignPrFrameID();
				}
				else
				{
					ulFrameID = 0;
				}
			}

			if( IsPrecanWith2Pr() && !m_bKeepScan2ndPrDie )
			{
				if( uwScanDiePrID==m_uwScanRecordID[1] )
				{
					continue;
				}
			}

			if( IsAOIOnlyMachine() && IsPrescanMapIndex() && usDieState!=0 )
			{
				USHORT usState = 0, usPrID = 0;
				DOUBLE dAngle = 0;
				LONG lDummyX = 0, lDummyY = 0;
				CString szNewBin = "";
				ULONG ulGrabFrameID = 0;
				for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
				{
					if( WSGetScanPosition(ulIndex, lDummyX, lDummyY, dAngle, szNewBin, usState, usPrID, ulGrabFrameID) )
					{
						if( usState!=0 )
						{
							continue;
						}
						if( labs(lDummyX-lEncX)<lDieSizeTolX && labs(lDummyY-lEncY)<lDieSizeTolY )
						{
							lEncX = lDummyX;
							lEncY = lDummyY;
							dDieAngle = dAngle;
							szDieBin = szNewBin;
							usDieState = usState;
							ulFrameID = ulGrabFrameID;
						}
					}
				}
			}

			SetScanMapRecordID(ulInRow, ulInCol, (UCHAR) uwScanDiePrID);
			SetScanInfo(ulInRow, ulInCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);	//	from physical map builder
			SetPrescanFrameInfo(ulInRow, ulInCol, ulFrameID);
			if ((nOutErr == 0) && (fpOut != NULL))
			{
				fprintf(fpOut, "%ld,%ld,%lu,%lu,%ld,%ld\n", ulInRow, ulInCol, ulRow, ulCol, lEncX, lEncY);
			}
		}
	}
	SaveScanTimeEvent("WPR: after set BW scan info done from builder.");

	LONG lDieTolX = ulDieSizeXTol;
	LONG lDieTolY = ulDieSizeYTol;
	WSClearScanRemainDieList();

	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if( pbGetOut[ulIndex]==true )
		{
			if ((nOutErr == 0) && (fpOut != NULL))
			{
				fprintf(fpOut, "indexed is %lu\n", ulIndex);	//v4.49 Klocwork
			}
			continue;
		}

		if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
		{
			continue;
		}

		LONG lEncX1 = 0, lEncY1 = 0;
		for(ULONG j=0; j<lRefDieCount; j++)
		{
			lEncX1 = dwaRefWfX.GetAt(j);
			lEncY1 = dwaRefWfY.GetAt(j);
			if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
			{
				pbGetOut[ulIndex] = true;
				if ((nOutErr == 0) && (fpOut != NULL))
				{
					fprintf(fpOut, "indexed is %lu done at refer %lu\n", ulIndex, j);
				}
				break;
			}
		}

		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}

		for(ULONG j=1; j<=WSGetPrescanTotalDie(); j++)
		{
			if( WSGetPrescanPosn(j, lEncX1, lEncY1) )
			{
				if( j<ulIndex )
				{
					if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
					{
						pbGetOut[ulIndex] = true;
						if ((nOutErr == 0) && (fpOut != NULL))
						{
							fprintf(fpOut, "indexed is %lu done at %lu\n", ulIndex, j);
						}
						break;
					}
				}
				else if( j>ulIndex )
				{
					if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
					{
						if( pbGetOut[j]==true )
						{
							pbGetOut[ulIndex] = true;
							if ((nOutErr == 0) && (fpOut != NULL))
							{
								fprintf(fpOut, "indexed is %lu done at %lu\n", ulIndex, j);
							}
							break;
						}
					}
				}
			}
		}
		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}
		if ((nOutErr == 0) && (fpOut != NULL))
		{
			fprintf(fpOut, "index %lu to remain list,%ld,%ld\n", ulIndex, lEncX, lEncY);
		}
		pbGetOut[ulIndex] = true;
		WSAddScanRemainDieIntoList(lEncX, lEncY, ulIndex);
	}

	if ((nOutErr == 0) && (fpOut != NULL))
	{
		fclose(fpOut);
	}

	delete [] pbGetOut;

	SaveScanTimeEvent("WPR: to remove all data of physical map builder.");

	m_cPIMap.RemoveAll();
	m_cPIMap.SetScan2Pr(FALSE);

	// transfer data from prescan result to map physical position array
	LONG lUserRow = 0, lUserCol = 0;

	CStringArray	szaPrSummaryList;

	nMaxSpan = 5;

	ULONG ulEmptyIdx = 0, ulDefectIdx = 0, ulBadCutIdx = 0, ulExtraIdx = 0, ulGoodIdx = 0;
	ULONG ulMapTotal = 0;

	SaveScanTimeEvent("WPR: get out complete and to update wafer map");

	DelPrescanRunPosn();
	SaveScanTimeEvent("WPR: loop to fill in map position.");

	for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			BOOL bIsIgnore = FALSE;
			BOOL bToUpdatePosn = TRUE;
			BOOL bToChangeGrade = TRUE;
			UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);

			BOOL bIsExtra = FALSE;
			if( ucB4ScanGrade==ucNullBin )
			{
				if( bExtraAction )
				{
					bIsExtra = TRUE;
				}
				else
				{
					bIsIgnore = TRUE;
					bToUpdatePosn = FALSE;
				}
			}

			if( bIsExtra!=TRUE && m_bPrescanEmptyToNullBin==FALSE && m_bPrescanEmptyAction && ucB4ScanGrade==ucEmptyGrade )
			{
				bIsIgnore = TRUE;
			}

			if( IsScanMapNgGrade(ucB4ScanGrade-ucOffset) )
			{
				bToChangeGrade = FALSE;
			}

			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);

			if( bInfo )
			{
				if( bToUpdatePosn )
				{
					m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
					SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
				}
				else
				{
					if( IsInMapValidRange(ulRow, ulCol) )
					{
						SetExtraInfo(ulExtraIdx, ulRow, ulCol);
						ulExtraIdx++;
					}
				}
			}

			if( bInfo==FALSE && bIsExtra )
			{
				bIsIgnore = TRUE;
			}
			if( bIsIgnore )
			{
				continue;
			}

			BOOL bIsRefer = FALSE;
			if( ucReferGrade==ucB4ScanGrade )
				bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol);
			BOOL bIsSpRef = IsSpecialReferGrade(ulRow, ulCol);
			BOOL bIsNoDie = IsNoDieGrade(ulRow, ulCol);
			if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
				bIsRefer = TRUE;

			if( bIsRefer )
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if( pApp->GetCustomerName()=="Huga" )	// MARK REFER DIE
				{
					COLORREF crUserColor = RGB(255,5,5); //(in Red color)
					m_WaferMapWrapper.AddDescription(ulRow, ulCol, 96, crUserColor, "R");
				}
			}

			if( bIsRefer || bIsSpRef || bIsNoDie )
			{
				continue;
			}

			BOOL bScoreCount = TRUE;
			if( IsAOIOnlyMachine() && m_bPrescanSkipNgGrade && bToChangeGrade==FALSE )
			{
				bScoreCount = FALSE;
			}

			if( m_bPrescanSkipNgGrade && m_ucScanMapStartNgGrade==0 && ucB4ScanGrade==ucOffset )
			{
				bScoreCount = FALSE;
			}

			if( m_WaferMapWrapper.GetReader()!=NULL && m_WaferMapWrapper.GetReader()->IsInvalidDie(ulRow, ulCol) )
			{
				bScoreCount = FALSE;
			}

			if( bIsExtra )
			{
				bScoreCount = FALSE;
			}

			if( bScoreCount )
			{
				ulMapTotal++;
			}

			if( bInfo==FALSE )
			{
				BOOL bGetPred = GetDieValidInX(ulRow, ulCol, nMaxSpan, lEncX, lEncY, FALSE);

				if( bGetPred )
				{
					PR_UWORD uwDiePrID = 0;
					if( FindAndFillFromRemainList(ulRow, ulCol, lDieTolX, lDieTolY, lEncX, lEncY, uwDiePrID) )
					{
						bInfo = TRUE;
						m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
						SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
					}
				}
			}

			CString szDieOcrValue = "";
			if( bInfo )
			{
				BOOL bNeedChangeGrade = FALSE;
				UCHAR ucChangeGrade = ucB4ScanGrade;
				BOOL bNgDieGrade = FALSE;

				if( bIsExtra )
				{
					SetExtraInfo(ulExtraIdx, ulRow, ulCol);
					ulExtraIdx++;

					if( IsNoPickExtra() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_UNPICK_SCAN_EXTRA);
					}

					if( bToChangeGrade )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucExtraGrade;
					}
				}
				else
				if (bIsBadCut)
				{
					if( bScoreCount )
					{
						SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucB4ScanGrade);
						ulBadCutIdx++;
					}

					if( IsNoPickBadCut() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
					}

					if( bBadCutAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucBadCutGrade;
							bNgDieGrade = TRUE;
						}
					}
					else
					{
						if( m_bPrescanBadCutMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else if( bIsFakeEmpty )
				{
					if( bScoreCount )
					{
						SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
						ulDefectIdx++;
					}

					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);

					if( bFakeAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucFakeGrade;
							bNgDieGrade = TRUE;
						}
					}
				}
				else if( bIsDefect )
				{
					if( bScoreCount )
					{
						SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
						ulDefectIdx++;
					}

					if( IsNoPickDefect() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
					}

					if( bDefectAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucDefectGrade;
							bNgDieGrade = TRUE;
						}
					}
					else
					{
						if( m_bPrescanDefectMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else
				{
					if( bScoreCount )
					{
						ulGoodIdx++;
					}
					if( bGoodAction && bToChangeGrade )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucGoodGrade;
					}
				}

				UCHAR ucDieBin = 0;
				if( IsAOIOnlyMachine() && szDieBin.IsEmpty()==FALSE )
				{
					ucDieBin = (UCHAR)atoi(szDieBin);
				}
				if( m_bSaveNgDieSubGrade && bNeedChangeGrade && (GetPsmEnable()==FALSE) )
				{
					ucChangeGrade += ucDieBin;
				}

				if( IsUseGradeConvert() && m_lGradeMappingLimit>0 )
				{
					CString szTemp, szAppend = "";
					if( m_bUseAoiGradeConvert )
					{
						szTemp.Format("map (%d,%d) grade %d DieBin %s Bin %d ==> ", ulRow, ulCol, (ucB4ScanGrade - ucOffset), szDieBin, ucDieBin);
						for(int i=0; i<m_lGradeMappingLimit; i++)
						{
							if( m_sMapOrgGrade[i]==(ucB4ScanGrade - ucOffset) && m_sPrInspectGrade[i]==ucDieBin )
							{
								ucChangeGrade = (UCHAR) m_sMapNewGrade[i];
								bNeedChangeGrade = TRUE;
								szAppend.Format("map grade aoi convert to %d", ucChangeGrade);
								ucChangeGrade = ucChangeGrade + ucOffset;
								break;
							}
						}
					}

					if( m_bScanNgGradeConvert )
					{
						UCHAR ucChkGrade = ucB4ScanGrade - ucOffset;
						szTemp.Format("map (%d,%d) grade %d ==> ", ulRow, ulCol, ucChkGrade);
						if( m_sMapOrgGrade[ucChkGrade]==ucChkGrade )
						{
							if( bNeedChangeGrade )
							{
								ucChangeGrade = (UCHAR) m_sMapNewGrade[ucChkGrade];
							}
							else
							{
								ucChangeGrade = (UCHAR) m_sPrInspectGrade[ucChkGrade];
							}
							szAppend.Format("map grade convert to %d", ucChangeGrade);
							ucChangeGrade = ucChangeGrade + ucOffset;
						}
					}
					szTemp += szAppend;
				}

				if( bNeedChangeGrade )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucChangeGrade);
				}
			}
			else // set to empty grade
			{
				lEncX = lEncY = 0;
				if( bScoreCount )
				{
					SetEmptyFull(ulEmptyIdx, ulRow, ulCol, ucB4ScanGrade);
					ulEmptyIdx++;
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
				}

				if( bEmptyAction )
				{
					if( bToChangeGrade )
					{
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
					}
				}
				else
				{
					if( m_bPrescanEmptyMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						UpdateDie(ulRow, ulCol, ucB4ScanGrade, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
					}
				}
			}

#ifdef PR_INSP_MACHINE
			LONG lDieGradeState = 1;
			if( IsAOIOnlyMachine() && m_bPrescanSkipNgGrade && bToChangeGrade==FALSE )
			{
				lDieGradeState = 0;
			}
			LONG lUserRow = 0, lUserCol = 0;
			ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);

			UCHAR ucOldGrade = ucB4ScanGrade;
			UCHAR ucNewGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol) - ucOffset;
			USHORT usMapGrade = m_WaferMapWrapper.GetOriginalGrade(ucOldGrade);
			ucOldGrade = ucOldGrade - ucOffset;
			usMapGrade = usMapGrade - ucOffset;
			DOUBLE dPrX = 0, dPrY = 0;
			ConvertMotorStepToPrPixel(lEncX, lEncY, dPrX, dPrY);
			LONG lFrameID = ulFrameID;
			if( bInfo )
			{
				GetPrescanFrameInfo(ulRow, ulCol,  ulFrameID);
				lFrameID = ulFrameID ;
			}
			else
			{
				lFrameID = -1;
			}

			CString szLineData;
			szLineData.Format("%ld,%.0f,%.0f,%lu,%lu,%ld,%ld,%ld,%d,%d,%d", lFrameID, 
				dPrX, dPrY, ulRow, ulCol, lUserRow, lUserCol, lDieGradeState, ucNewGrade, ucOldGrade, usMapGrade);

			if( IsCharDieInUse() )
			{
				CHAR szXY[5] = "";
				CHAR ucChar2 = '0', ucChar3 = '0', ucChar4 = '0', ucChar5 = '0';
				ucChar2 = (CHAR)lUserRow/10 + 'A';
				ucChar3 = (CHAR)lUserRow%10 + '0';
				ucChar4 = (CHAR)lUserCol/10 + 'A';
				ucChar5 = (CHAR)lUserCol%10 + '0';
				szXY[0] = ucChar2;
				szXY[1] = ucChar3;
				szXY[2] = ucChar4;
				szXY[3] = ucChar5;
				szXY[4] = NULL;

				szLineData += ",";
				szLineData += szXY;
			}
			szaPrSummaryList.Add(szLineData);
#endif
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
	SaveScanTimeEvent("WPR: get out complete and to update wafer map done");
	m_lTimeSlot[8] = (LONG)(GetTime()-dUsedTime);	//	08.	update wafer map grade and position

	BOOL bFailBreak = FALSE;
	for(ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			BOOL bIsIgnore = FALSE;

			LONG lEncX1, lEncY1, lEncX2, lEncY2;
			if( GetPrescanPosition(ulRow, ulCol, lEncX1, lEncY1) )
			{
				if( GetPrescanPosition(ulRow, ulCol+1, lEncX2, lEncY2) )
				{
					if( labs(lEncX1-lEncX2)<lDieTolX && 
						labs(lEncY1-lEncY2)<lDieTolY )
					{
						LONG lUserRow1, lUserCol1, lUserRow2, lUserCol2;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow1, lUserCol1);
						ConvertAsmToOrgUser(ulRow, ulCol+1, lUserRow2, lUserCol2);
						CString szMsg;
						szMsg.Format("Map die (%d,%d) = (%d,%d).\nPosition overlap, please stop!", lUserRow1, lUserCol1, lUserRow2, lUserCol2);
						HmiMessage_Red_Back(szMsg, "Hisense");
						m_lPrescanVerifyResult += 16384;
						SetErrorMessage(szMsg);
						bFailBreak = TRUE;
						break;
					}
				}
			}
			if( bFailBreak )
			{
				break;
			}
		}
		if( bFailBreak )
		{
			break;
		}
	}

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: PASS DATA AND UPDATE GRADE LIST");

	lHomeNewRow += m_lMapScanOffsetRow;
	lHomeNewCol += m_lMapScanOffsetCol;

	pUtl->SetAlignPosition(lHomeNewRow, lHomeNewCol, lHomeWfX, lHomeWfY);

	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulEmptyIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= ulExtraIdx;

	szaPrSummaryList.RemoveAll();

	SetMapTotalDie(ulMapTotal);

	// build prescan map complete
	ClearGoodInfo();	ClearWSPrescanInfo();	WSClearScanRemainDieList();
	DelPrescanFrameInfo();

	UpdateBackLightDownState();

	SaveScanTimeEvent("WPR: CLEAR BUILDing materials");

	SendPrSummaryFileToEagle();

	UpdateBackLightDownState();

	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();
}

#define	USE_NEW_EMCORE_BARWAFER	1
#if	!USE_NEW_EMCORE_BARWAFER
VOID CWaferPr::ConstructPrescanMap_EmcoreBW()	//	old
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
	UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;

	BOOL  bBadCutAction	= m_bPrescanBadCutAction;
	BOOL  bDefectAction	= m_bPrescanDefectAction;
	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;

	if( m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;
	if( m_bPrescanDefectToNullBin )
		ucDefectGrade = ucNullBin;
	if (m_bPrescanBadCutToNullBin)
		ucBadCutGrade = ucNullBin;

	DOUBLE dUsedTime = GetTime();

	ULONG lRefDieCount = 0;
	CDWordArray dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol;
	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();

	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	ULONG ulDiePitchX = labs(GetDiePitchX_X());
	ULONG ulDiePitchY = labs(GetDiePitchY_Y());
	LONG  ulStreetInY = labs(GetDiePitchY_Y());
	if( m_nDieSizeX!=0 )
	{
		ulDiePitchY = labs(ulDiePitchX*m_nDieSizeY/m_nDieSizeX);
	}

	// add all alignment point and refer points to list
	LONG lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY, lHomeNewRow, lHomeNewCol;
	pUtl->GetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);
	lHomeNewRow = lHomeOriRow + WPR_SCAN_MAP_OFFSET;
	lHomeNewCol = lHomeOriCol + WPR_SCAN_MAP_OFFSET;

	dwaRefWfX.Add(lHomeWfX);
	dwaRefWfY.Add(lHomeWfY);
	dwaRefRow.Add(lHomeNewRow);
	dwaRefCol.Add(lHomeNewCol);
	lRefDieCount++;

	ULONG ulIndex = 0;
	LONG lAlgnRow, lAlgnCol, lEncX = 0, lEncY = 0;
	for(ulIndex=0; ulIndex<pUtl->GetNumOfReferPoints(); ulIndex++)
	{
		if( pUtl->GetReferPosition(ulIndex, lAlgnRow, lAlgnCol, lEncX, lEncY) )
		{
			dwaRefWfX.Add(lEncX);
			dwaRefWfY.Add(lEncY);
			lAlgnRow += WPR_SCAN_MAP_OFFSET;
			lAlgnCol += WPR_SCAN_MAP_OFFSET;
			dwaRefRow.Add(lAlgnRow);
			dwaRefCol.Add(lAlgnCol);
			lRefDieCount++;
		}
	}

	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lEncX, lEncY) )
		{
			if( lAlgnRow==lHomeOriRow && lAlgnCol==lHomeOriCol )
			{
				continue;
			}
			dwaRefWfX.Add(lEncX);
			dwaRefWfY.Add(lEncY);
			lAlgnRow += WPR_SCAN_MAP_OFFSET;
			lAlgnCol += WPR_SCAN_MAP_OFFSET;
			dwaRefRow.Add(lAlgnRow);
			dwaRefCol.Add(lAlgnCol);
			lRefDieCount++;
		}
	}

	ClearGoodInfo();

	CString szConLogFile = szLogPath + PRESCAN_PASSIN_PSN;
	FILE *fpPassIn = fopen(szConLogFile, "w");
	szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpOut = fopen(szConLogFile, "w");

	m_cPIMap.RemoveAll();

	// add all prescan die data list to physical map builder include refer points
	for(ulIndex=0; ulIndex<lRefDieCount; ulIndex++)
	{
		lEncX = dwaRefWfX.GetAt(ulIndex);
		lEncY = dwaRefWfY.GetAt(ulIndex);
		LONG lIndex = 0-ulIndex;
		m_cPIMap.SortAdd_Tail(lEncX, lEncY, lIndex, FALSE);
		if( fpPassIn!=NULL )
		{
			fprintf(fpPassIn, "%ld,%ld,%ld,%ld,%ld\n",
				lEncX, lEncY, dwaRefRow.GetAt(ulIndex), dwaRefCol.GetAt(ulIndex), lIndex);
		}
	}

	CString szMsg;
	BOOL bInfo;
	// build physical map
	int nMaxIterateCount = -1, nMaxAllowLeft = 10, nMaxSpan = 2;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);

	SaveScanTimeEvent("WPR: begin pass massive data to builder");

	if( fpPassIn!=NULL )
	{
		fprintf(fpPassIn, "%lu,%lu,%lu,%lu,%d,%d,%d,%d,%ld,%ld,%ld,%ld\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY, 
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan, 
			GetDieSizeX(), GetDieSizeY(), GetDiePitchX_X(), GetDiePitchY_Y());
	}

	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if(	WSGetPrescanPosn(ulIndex, lEncX, lEncY)==FALSE )
		{
			continue;
		}

		BOOL bInLine = FALSE;
		for(ULONG ulRefer=0; ulRefer<lRefDieCount; ulRefer++)
		{
			LONG lReferY = dwaRefWfY.GetAt(ulRefer);
			if( labs(lEncY-lReferY)<(ulStreetInY/2) )
			{
				bInLine = TRUE;
				break;
			}
		}

		if( bInLine )
		{
			m_cPIMap.SortAdd_Tail(lEncX, lEncY, ulIndex, TRUE);
			if( fpPassIn!=NULL )
			{
				fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
			}
		}
	}

	m_cPIMap.KeepIsolatedDice(FALSE);
	m_cPIMap.SetScan2Pr(FALSE);
	m_cPIMap.SetXOffset(0);

	SaveScanTimeEvent("WPR: to construct normal map");
	m_cPIMap.ConstructMap(dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
		bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);
	SaveScanTimeEvent("WPR: construct map complete");

	m_lTimeSlot[7] = (LONG)(GetTime()-dUsedTime);	//	07.	constuct map used time.

	SaveScanTimeEvent("WPR: to clear prescan info");
	ClearPrescanInfo();
	SaveScanTimeEvent("WPR: to reset map phy posn");
	ResetMapPhyPosn();
	SaveScanTimeEvent("WPR: clear physical prescan and map info done");

	ULONG ulPyiRowMax = 0, ulPyiColMax = 0;
	m_cPIMap.GetDimension(ulPyiRowMax, ulPyiColMax);

	ULONG ulRow, ulCol;
	BOOL bFirstGood = FALSE;
	ULONG ulScnDieMaxRow = 0, ulScnDieMaxCol = 0, ulScnDieMinRow = 9999, ulScnDieMinCol = 9999;
	for(ulRow=0; ulRow<=ulPyiRowMax; ulRow++)
	{
		for(ulCol=0; ulCol<=ulPyiColMax; ulCol++)
		{
			BOOL bDieOverLap = FALSE;
			LONG lIndex = 0;
			bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bInfo==FALSE )
			{
				continue;
			}

			if( lIndex<=WPR_ADD_DIE_INDEX )
			{
				continue;
			}

			if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
			{
				continue;
			}

			if( ulRow>ulScnDieMaxRow )
				ulScnDieMaxRow = ulRow;
			if( ulCol>ulScnDieMaxCol )
				ulScnDieMaxCol = ulCol;
			if( ulRow<ulScnDieMinRow )
				ulScnDieMinRow = ulRow;
			if( ulCol<ulScnDieMinCol )
				ulScnDieMinCol = ulCol;
		}
	}

	szMsg.Format("WPR: get out PHY range Row(%lu,%lu)  Col(%lu,%lu)",
		ulScnDieMinRow, ulScnDieMaxRow, ulScnDieMinCol, ulScnDieMaxCol);
	SaveScanTimeEvent(szMsg);

	dUsedTime = GetTime();
	bool *pbGetOut;
	pbGetOut = new bool[WSGetPrescanTotalDie()+10];
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		pbGetOut[ulIndex] = false;
	}

	DOUBLE dDieAngle;
	CString szDieBin = "0";

	for(ulRow=ulScnDieMinRow; ulRow<=ulScnDieMaxRow; ulRow++)
	{
		for(ulCol=ulScnDieMinCol; ulCol<=ulScnDieMaxCol; ulCol++)
		{
			LONG lIndex = 0;
			BOOL bDieOverLap = FALSE;
			bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bInfo==FALSE )
			{
				continue;
			}

			if( lIndex<=WPR_ADD_DIE_INDEX )
			{
				continue;
			}
			if( lIndex>0 )
			{
				pbGetOut[lIndex] = true;
			}

			LONG ulInRow = (LONG)ulRow + m_lMapScanOffsetRow;
			LONG ulInCol = (LONG)ulCol + m_lMapScanOffsetCol;

			USHORT usDieState = 0;
			if( lIndex>0 )
			{
				LONG lDummyX = 0, lDummyY = 0;
				ULONG ulFrameID = 0;
				PR_UWORD uwScanDiePrID = 0;
				WSGetScanPosition(lIndex, lDummyX, lDummyY, dDieAngle, szDieBin, usDieState, uwScanDiePrID, ulFrameID);
			}
			else
			{
				dDieAngle = 0.0;
				szDieBin = "0";
			}

			SetScanInfo(ulInRow, ulInCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);	//	from physical map builder
			if( fpOut!=NULL )
			{
				fprintf(fpOut, "%ld,%ld,%lu,%lu,%ld,%ld\n", ulInRow, ulInCol, ulRow, ulCol, lEncX, lEncY);
			}
		}
	}
	SaveScanTimeEvent("WPR: after set emcoreBW scan info done from builder.");

	LONG lDieTolX = ulDieSizeXTol;
	LONG lDieTolY = ulDieSizeYTol;
	WSClearScanRemainDieList();

	if( fpOut!=NULL )
	{
		fprintf(fpOut, "below is the list into remain\n");
	}
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}

		if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
		{
			continue;
		}

		LONG lEncX1 = 0, lEncY1 = 0;
		for(ULONG j=0; j<lRefDieCount; j++)
		{
			lEncX1 = dwaRefWfX.GetAt(j);
			lEncY1 = dwaRefWfY.GetAt(j);
			if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
			{
				pbGetOut[ulIndex] = true;
				break;
			}
		}

		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}

		for(ULONG j=1; j<=WSGetPrescanTotalDie(); j++)
		{
			if( WSGetPrescanPosn(j, lEncX1, lEncY1) )
			{
				if( j<ulIndex )
				{
					if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
					{
						pbGetOut[ulIndex] = true;
						break;
					}
				}
				else if( j>ulIndex )
				{
					if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
					{
						if( pbGetOut[j]==true )
						{
							pbGetOut[ulIndex] = true;
							break;
						}
					}
				}
			}
		}
		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}
		if( fpOut!=NULL )
		{
			fprintf(fpOut, "index %lu to remain list,%ld,%ld\n", ulIndex, lEncX, lEncY);
		}
		pbGetOut[ulIndex] = true;
		WSAddScanRemainDieIntoList(lEncX, lEncY, ulIndex);
	}
	delete [] pbGetOut;

	if( fpPassIn!=NULL )
	{
		fclose(fpPassIn);
	}
	if( fpOut!=NULL )
	{
		fclose(fpOut);
	}

	SaveScanTimeEvent("WPR: to remove all data of physical map builder.");
	m_cPIMap.RemoveAll();
	m_cPIMap.SetScan2Pr(FALSE);

	// transfer data from prescan result to map physical position array
	ULONG ulEmptyIdx = 0, ulDefectIdx = 0, ulBadCutIdx = 0, ulGoodIdx = 0, ulMapTotal = 0;
	nMaxSpan = 5;

	SaveScanTimeEvent("WPR: get out complete and to update wafer map");
	DelPrescanRunPosn();

	SaveScanTimeEvent("WPR: loop to fill in map position.");
	for(ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
	{
		for(ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);

			if( ucB4ScanGrade==ucNullBin )
			{
				continue;
			}

			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			BOOL bHasDie = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);

			if( bHasDie )
			{
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
				SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
			}
			else
			{
				if( GetDieValidInX(ulRow, ulCol, nMaxSpan, lEncX, lEncY, FALSE) )
				{
					PR_UWORD uwDiePrID = 0;
					if( FindAndFillFromRemainList(ulRow, ulCol, lDieTolX, lDieTolY, lEncX, lEncY, uwDiePrID) )
					{
						bHasDie = TRUE;
						m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
						SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
					}
				}
			}

			if( bHasDie==FALSE )
			{
				continue;
			}

			ulMapTotal++;

			BOOL bNeedChangeGrade = FALSE;
			UCHAR ucChangeGrade = ucB4ScanGrade;

			if (bIsBadCut)
			{
				SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucB4ScanGrade);
				ulBadCutIdx++;

				if( IsNoPickBadCut() )
				{
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
				}

				if( bBadCutAction )
				{
					bNeedChangeGrade = TRUE;
					ucChangeGrade = ucBadCutGrade;
				}
				else
				{
					if( m_bPrescanBadCutMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
					}
				}
			}
			else if( bIsFakeEmpty )
			{
				SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
				ulDefectIdx++;

				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);
			}
			else if( bIsDefect )
			{
				SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
				ulDefectIdx++;

				if( IsNoPickDefect() )
				{
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
				}

				if( bDefectAction )
				{
					bNeedChangeGrade = TRUE;
					ucChangeGrade = ucDefectGrade;
				}
				else
				{
					if( m_bPrescanDefectMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
					}
				}
			}
			else
			{
				ulGoodIdx++;
				if( bGoodAction )
				{
					bNeedChangeGrade = TRUE;
					ucChangeGrade = ucGoodGrade;
				}
			}

			if( bNeedChangeGrade )
			{
				m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucChangeGrade);
			}
		}
	}

	for(ulRow=GetMapValidMaxRow(); ulRow>GetMapValidMinRow(); ulRow--)
	{
		for(ulCol=GetMapValidMaxCol(); ulCol>GetMapValidMinCol(); ulCol--)
		{
			UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);

			if( ucB4ScanGrade==ucNullBin )
			{
				continue;
			}

			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			BOOL bHasDie = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);

			if( bHasDie )
			{
				continue;
			}

			if( GetDieValidInX(ulRow, ulCol, 0-nMaxSpan, lEncX, lEncY, FALSE) )
			{
				PR_UWORD uwDiePrID = 0;
				if( FindAndFillFromRemainList(ulRow, ulCol, lDieTolX, lDieTolY, lEncX, lEncY, uwDiePrID) )
				{
					bHasDie = TRUE;
					m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
					SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
				}
			}

			ulMapTotal++;

			if( bHasDie )
			{
				BOOL bNeedChangeGrade = FALSE;
				UCHAR ucChangeGrade = ucB4ScanGrade;

				if (bIsBadCut)
				{
					SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucB4ScanGrade);
					ulBadCutIdx++;

					if( IsNoPickBadCut() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
					}

					if( bBadCutAction )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucBadCutGrade;
					}
					else
					{
						if( m_bPrescanBadCutMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else if( bIsFakeEmpty )
				{
					SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
					ulDefectIdx++;

					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);
				}
				else if( bIsDefect )
				{
					SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
					ulDefectIdx++;

					if( IsNoPickDefect() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
					}

					if( bDefectAction )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucDefectGrade;
					}
					else
					{
						if( m_bPrescanDefectMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else
				{
					ulGoodIdx++;
					if( bGoodAction )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucGoodGrade;
					}
				}

				if( bNeedChangeGrade )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucChangeGrade);
				}
			}
			else // set to empty grade
			{
				lEncX = lEncY = 0;
				SetEmptyFull(ulEmptyIdx, ulRow, ulCol, ucB4ScanGrade);
				ulEmptyIdx++;
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);

				if( bEmptyAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
				}
				else
				{
					if( m_bPrescanEmptyMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						UpdateDie(ulRow, ulCol, ucB4ScanGrade, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
					}
				}
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
	SaveScanTimeEvent("WPR: get out complete and to update wafer map done");
	m_lTimeSlot[8] = (LONG)(GetTime()-dUsedTime);	//	08.	update wafer map grade and position

	BOOL bFailBreak = FALSE;
	for(ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
	{
		for(ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			BOOL bIsIgnore = FALSE;

			LONG lEncX1, lEncY1, lEncX2, lEncY2;
			if( GetPrescanPosition(ulRow, ulCol, lEncX1, lEncY1) )
			{
				if( GetPrescanPosition(ulRow, ulCol+1, lEncX2, lEncY2) )
				{
					if( labs(lEncX1-lEncX2)<lDieTolX && 
						labs(lEncY1-lEncY2)<lDieTolY )
					{
						LONG lUserRow1, lUserCol1, lUserRow2, lUserCol2;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow1, lUserCol1);
						ConvertAsmToOrgUser(ulRow, ulCol+1, lUserRow2, lUserCol2);
						CString szMsg;
						szMsg.Format("Map die (%d,%d) = (%d,%d).\nPosition overlap, please stop!", lUserRow1, lUserCol1, lUserRow2, lUserCol2);
						HmiMessage_Red_Back(szMsg, "Hisense");
						m_lPrescanVerifyResult += 16384;
						SetErrorMessage(szMsg);
						bFailBreak = TRUE;
						break;
					}
				}
			}
			if( bFailBreak )
			{
				break;
			}
		}
		if( bFailBreak )
		{
			break;
		}
	}

	lHomeNewRow += m_lMapScanOffsetRow;
	lHomeNewCol += m_lMapScanOffsetCol;
	pUtl->SetAlignPosition(lHomeNewRow, lHomeNewCol, lHomeWfX, lHomeWfY);

	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulEmptyIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= 0;

	SetMapTotalDie(ulMapTotal);

	SaveScanTimeEvent("WPR: CLEAR BUILDing materials");
	// build prescan map complete
	ClearGoodInfo();	ClearWSPrescanInfo();	WSClearScanRemainDieList();
	DelPrescanFrameInfo();

	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();
}	//	EMCORE bar wafer
#endif

VOID CWaferPr::ConstructPrescanMap_OcrBWNoMap()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//Get available wafermap grade & its total count

	ULONG ulIndex;
	BOOL bInfo;
	ULONG lRefDieCount = 0;
	CString szConLogFile;

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
	UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;
	UCHAR ucReferGrade = m_WaferMapWrapper.GetReader()->GetConfiguration().GetReferenceAlignBin();

	BOOL  bBadCutAction	= m_bPrescanBadCutAction;
	BOOL  bDefectAction	= m_bPrescanDefectAction;
	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;
	BOOL  bFakeAction	= m_bScanDetectFakeEmpty;
	UCHAR ucFakeGrade	= m_ucScanFakeEmptyGrade;
	BOOL  bExtraAction  = m_bScanExtraDieAction;
	UCHAR ucExtraGrade	= m_ucScanExtraGrade + ucOffset;
	if( bExtraAction==FALSE )
		ucExtraGrade = ucNullBin;

	if( m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;
	if( m_bPrescanDefectToNullBin )
		ucDefectGrade = ucNullBin;
	if (m_bPrescanBadCutToNullBin)
		ucBadCutGrade = ucNullBin;

	CString szMsg;

	DOUBLE dUsedTime = GetTime();

	// capture last image
	LONG lEncX, lEncY;

	CDWordArray dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol;
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	ULONG ulMapRowMax, ulMapColMax;
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMapRowMax, ulMapColMax);

	ULONG ulDiePitchX = labs(GetDiePitchX_X());
	ULONG ulDiePitchY = labs(GetDiePitchY_Y());
	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();

	// add all alignment point and refer points to list
	LONG lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY, lHomeNewRow, lHomeNewCol;
	pUtl->GetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);
	lHomeNewRow = lHomeOriRow + WPR_SCAN_MAP_OFFSET;
	lHomeNewCol = lHomeOriCol + WPR_SCAN_MAP_OFFSET;

	dwaRefWfX.Add(lHomeWfX);
	dwaRefWfY.Add(lHomeWfY);
	dwaRefRow.Add(lHomeNewRow);
	dwaRefCol.Add(lHomeNewCol);
	lRefDieCount++;

	LONG lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY;
	for(ulIndex=0; ulIndex<pUtl->GetNumOfReferPoints(); ulIndex++)
	{
		if( pUtl->GetReferPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			dwaRefWfX.Add(lAlgnWfX);
			dwaRefWfY.Add(lAlgnWfY);
			lAlgnRow += WPR_SCAN_MAP_OFFSET;
			lAlgnCol += WPR_SCAN_MAP_OFFSET;
			dwaRefRow.Add(lAlgnRow);
			dwaRefCol.Add(lAlgnCol);
			lRefDieCount++;
		}
	}

	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			if( lAlgnRow==lHomeOriRow && lAlgnCol==lHomeOriCol )
				continue;
			dwaRefWfX.Add(lAlgnWfX);
			dwaRefWfY.Add(lAlgnWfY);
			lAlgnRow += WPR_SCAN_MAP_OFFSET;
			lAlgnCol += WPR_SCAN_MAP_OFFSET;
			dwaRefRow.Add(lAlgnRow);
			dwaRefCol.Add(lAlgnCol);
			lRefDieCount++;
		}
	}

	ClearGoodInfo();

	szConLogFile = szLogPath + PRESCAN_PASSIN_PSN;
	FILE *fpPassIn = NULL;
	errno_t nErr = fopen_s(&fpPassIn, szConLogFile, "w");

	// add all prescan die data list to physical map builder include refer points
	for (ulIndex = 0; ulIndex < lRefDieCount; ulIndex++)
	{
		lEncX = dwaRefWfX.GetAt(ulIndex);
		lEncY = dwaRefWfY.GetAt(ulIndex);
		LONG lIndex = 0-ulIndex;
		m_cPIMap.SortAdd_Tail(lEncX, lEncY, lIndex, FALSE);
		if ((nErr == 0) && (fpPassIn != NULL))
		{
			fprintf(fpPassIn, "%ld,%ld,%ld,%ld,%ld\n",
				lEncX, lEncY, dwaRefRow.GetAt(ulIndex), dwaRefCol.GetAt(ulIndex), lIndex);
		}
	}

	// build physical map
	int nMaxSpan = 1;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	int nMaxIterateCount = -1;
	int nMaxAllowLeft = 10;
	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: begin pass massive data to builder");

	if ((nErr == 0) && (fpPassIn != NULL))
	{
		fprintf(fpPassIn, "%lu,%lu,%lu,%lu,%d,%d,%d,%d,%ld,%ld,%ld,%ld\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY, 
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan, GetDieSizeX(), GetDieSizeY(), GetDiePitchX_X(), GetDiePitchY_Y());
	}

	if( IsPrecanWith2Pr() )	// pass raw data to PIMap builder
	{
		ULONG *pulReferBadList;
		pulReferBadList = new ULONG[WSGetPrescanTotalDie() + 10];
		memset(pulReferBadList, 0, sizeof(pulReferBadList));
		ULONG lReferBadIndex = 0;
		// refer scan, make sure good die not a refer die and 
		//						 refer die not a good die and 
		//						 refer die to refer die less 2 size and large 0.2 
		if( IsPrescanReferDie() )
		{
			for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
			{
				USHORT uwDiePrID = 0;
				if( WSGetPosnPrID(ulIndex, lEncX, lEncY, uwDiePrID)==false )
					continue;

				if( uwDiePrID==m_uwScanRecordID[0] )	// good, to check not is a refer die.
				{
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetScanReferTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						if( WSGetScanReferPosition(ulChkIndex, lChkX, lChkY)!=true )
						{
							continue;
						}

						if( abs(lEncX-lChkX)<abs(GetDieSizeX()*3/4) && abs(lEncY-lChkY)<abs(GetDieSizeY()*3/4) )
						{
							pulReferBadList[lReferBadIndex] = ulIndex;
							lReferBadIndex++;
							break;
						}
					}	// find in reference list
				}	// good, to check not is a refer die.
				else	// refer, can not be a good die and can not too near to another refer die
				{
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetScanReferTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						if( WSGetScanReferPosition(ulChkIndex, lChkX, lChkY)!=true )
						{
							continue;
						}

						if( (abs(lEncX-lChkX)>abs(GetDieSizeX()/5) || abs(lEncY-lChkY)>abs(GetDieSizeY()/5)) &&
							(abs(lEncX-lChkX)<abs(GetDieSizeX()*2) && abs(lEncY-lChkY)<abs(GetDieSizeY()*2)) )
						{
							pulReferBadList[lReferBadIndex] = ulIndex;
							lReferBadIndex++;
							break;
						}
					}	// find in reference list, not too near
					for(ULONG ulChkIndex=1; ulChkIndex<=WSGetPrescanTotalDie(); ulChkIndex++)
					{
						LONG lChkX, lChkY;
						USHORT uwChkPrID = 0;
						if( WSGetPosnPrID(ulChkIndex, lChkX, lChkY, uwChkPrID)==false )
						{
							continue;
						}

						if( uwChkPrID==m_uwScanRecordID[0] )
						{
							if( abs(lEncX-lChkX)<abs(GetDieSizeX()*3/4) && abs(lEncY-lChkY)<abs(GetDieSizeY()*3/4) )
							{
								pulReferBadList[lReferBadIndex] = ulIndex;
								lReferBadIndex++;
								break;
							}
						}
					}	// check in good list, if it is a good die, skip
				}	// refer, can not be a good die and can not too near to another refer die
			}	// all list
		}	// refer die scan

		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
			{
				continue;
			}

			if( IsPrescanReferDie() && lReferBadIndex>0 )
			{
				for(ULONG ulChkIndex=0; ulChkIndex<lReferBadIndex; ulChkIndex++)
				{
					if( pulReferBadList[ulChkIndex]==ulIndex )
					{
						continue;
					}
				}
			}

			m_cPIMap.SortAdd_Tail(lEncX, lEncY, ulIndex, FALSE);
			if ((nErr == 0) && (fpPassIn != NULL))
			{
				fprintf(fpPassIn, "%ld,%ld,%lu\n", lEncX, lEncY, ulIndex);
			}
		}
		delete[] pulReferBadList;
	}	//	end of 2nd pr prescan
	else
	{
		for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
		{
			USHORT uwDiePrID;

			if(	WSGetPosnPrID(ulIndex, lEncX, lEncY, uwDiePrID)==FALSE )
				continue;

			if ((nErr == 0) && (fpPassIn != NULL))
			{
				fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
			}
		}
	}

	if ((nErr == 0) && (fpPassIn != NULL))
	{
		fclose(fpPassIn);
	}

	bool *pbGetOut;
	pbGetOut = new bool[WSGetPrescanTotalDie()+10];
	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		pbGetOut[ulIndex] = false;
	}

	if( IsPrecanWith2Pr() )
	{
		m_cPIMap.KeepIsolatedDice(FALSE);
		m_cPIMap.SetScan2Pr(TRUE);
	}
	else
	{
		m_cPIMap.KeepIsolatedDice(FALSE);
		m_cPIMap.SetScan2Pr(FALSE);
	}

	SaveScanTimeEvent("WPR: to construct normal map");
	if( GetDieShape()==WPR_RHOMBUS_DIE )
	{
		LONG lDiePitchX_Y = GetDiePitchX_Y();
		LONG lDiePitchY_X = GetDiePitchY_X();
		m_cPIMap.SetXOffset(lDiePitchY_X);
		szMsg.Format("WPR: die pitch x %d,%d, y %d,%d", ulDiePitchX, lDiePitchX_Y, ulDiePitchY, lDiePitchY_X);
		SaveScanTimeEvent(szMsg);
	}
	else
	{
		m_cPIMap.SetXOffset(0);
	}

	ULONG ulCtrCol = ulMapColMax/2;

	m_cPIMap.ConstructMap(dwaRefWfX, dwaRefWfY, dwaRefRow, dwaRefCol,
		ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
		bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);
	SaveScanTimeEvent("WPR: construct map complete");

	m_lTimeSlot[7] = (LONG)(GetTime()-dUsedTime);	//	07.	constuct map used time.

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: AFTER build PHY MAP");

	SaveScanTimeEvent("WPR: to clear prescan info");
	ClearPrescanInfo();
	SaveScanTimeEvent("WPR: to reset map phy posn");
	ResetMapPhyPosn();

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: clear physical prescan and map info done");

	// get all phsical list infromation to prescan result list
	FILE *fpMap = NULL;
	CString szScanPmpFile = szLogPath + "_Scan.pmp";
	nErr = fopen_s(&fpMap, szScanPmpFile, "w");

	BOOL bFirstGood = FALSE;
	ULONG ulScnDieMaxRow = 0, ulScnDieMaxCol = 0, ulScnDieMinRow = 9999, ulScnDieMinCol = 9999;
	ULONG ulPyiRowMax = 0, ulPyiColMax = 0;
	m_cPIMap.GetDimension(ulPyiRowMax, ulPyiColMax);

	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "PHY range Row(%lu,%lu)  Col(%lu,%lu)\n", 0, ulPyiRowMax, 0, ulPyiColMax);
		fprintf(fpMap, "MAP range Row(0,%lu)  Col(0,%lu)\n", ulMapRowMax, ulMapColMax);
		fprintf(fpMap, "MAP range valid(%lu,%lu)  (%lu,%lu)\n", 
			GetMapValidMinRow(), GetMapValidMinCol(), GetMapValidMaxRow(), GetMapValidMaxCol());
		fprintf(fpMap, "DataFileName,,%s\n", (LPCTSTR)szScanPmpFile);
		fprintf(fpMap, "LotNumber,,\nDeviceNumber,,\nwafer id=\nTestTime,\nMapFileName,,\nTransferTime,\n");
		fprintf(fpMap, "\n");
	}

	for (ULONG ulRow = 0; ulRow <= ulPyiRowMax; ulRow++)
	{
		for (ULONG ulCol = 0; ulCol <= ulPyiColMax; ulCol++)
		{
			BOOL bDieOverLap = FALSE;
			LONG lIndex = 0;
			bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bInfo==FALSE )
			{
				continue;
			}

			if( lIndex<=WPR_ADD_DIE_INDEX )
			{
				continue;
			}

			if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
			{
				continue;
			}

			UCHAR ucMapGrade = 1;

			if ((nErr == 0) && (fpMap != NULL))
			{
				if( bFirstGood==FALSE )
				{
					fprintf(fpMap, "map data\n");
					fprintf(fpMap, "%lu,%lu,\n", ulCol, ulRow);
					bFirstGood = TRUE;
				}
				fprintf(fpMap, "%lu,%lu,%d\n", ulCol, ulRow, ucMapGrade);
			}

			if( ulRow>ulScnDieMaxRow )
				ulScnDieMaxRow = ulRow;
			if( ulCol>ulScnDieMaxCol )
				ulScnDieMaxCol = ulCol;
			if( ulRow<ulScnDieMinRow )
				ulScnDieMinRow = ulRow;
			if( ulCol<ulScnDieMinCol )
				ulScnDieMinCol = ulCol;
		}
	}

	if ((nErr == 0) && (fpMap != NULL))
	{
		fclose(fpMap);
	}

	szMsg.Format("WPR: get out PHY range Row(%lu,%lu)  Col(%lu,%lu)",
		ulScnDieMinRow, ulScnDieMaxRow, ulScnDieMinCol, ulScnDieMaxCol);
	SaveScanTimeEvent(szMsg);

	ULONG ulFrameID = 0;
	DOUBLE dDieAngle;
	CString szDieBin = "0";
	szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpOut = NULL;
	nErr = fopen_s(&fpOut, szConLogFile, "w");

	dUsedTime = GetTime();

	LONG lDieSizeTolX = (LONG) ulDieSizeXTol;
	LONG lDieSizeTolY = (LONG) ulDieSizeYTol;
	for(ULONG ulRow=ulScnDieMinRow; ulRow<=ulScnDieMaxRow; ulRow++)
	{
		for(ULONG ulCol=ulScnDieMinCol; ulCol<=ulScnDieMaxCol; ulCol++)
		{
			LONG lIndex = 0;
			BOOL bDieOverLap = FALSE;
			bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
			if( bInfo==FALSE )
			{
				continue;
			}

			if( lIndex<=WPR_ADD_DIE_INDEX )
				continue;
			if( lIndex>0 )
				pbGetOut[lIndex] = true;

			LONG ulInRow = (LONG)ulRow + m_lMapScanOffsetRow;
			LONG ulInCol = (LONG)ulCol + m_lMapScanOffsetCol;

			USHORT usDieState = 0;
			PR_UWORD uwScanDiePrID = 0;
			if( lIndex>0 )
			{
				LONG lDummyX = 0, lDummyY = 0;
				WSGetScanPosition(lIndex, lDummyX, lDummyY, dDieAngle, szDieBin, usDieState, uwScanDiePrID, ulFrameID);
			}
			else
			{
				dDieAngle = 0.0;
				szDieBin = "0";
				uwScanDiePrID = 0;
				if( ulRow==lHomeNewRow && ulCol==lHomeNewCol &&
					lEncX==lHomeWfX && lEncY==lHomeWfY )
				{
					ulFrameID = pUtl->GetAlignPrFrameID();
				}
				else
				{
					ulFrameID = 0;
				}
			}

			if( IsPrecanWith2Pr() && !m_bKeepScan2ndPrDie )
			{
				if( uwScanDiePrID==m_uwScanRecordID[1] )
				{
					continue;
				}
			}

			if( IsAOIOnlyMachine() && IsPrescanMapIndex() && usDieState!=0 )
			{
				USHORT usState = 0, usPrID = 0;
				DOUBLE dAngle = 0;
				LONG lDummyX = 0, lDummyY = 0;
				CString szNewBin = "";
				ULONG ulGrabFrameID = 0;
				for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
				{
					if( WSGetScanPosition(ulIndex, lDummyX, lDummyY, dAngle, szNewBin, usState, usPrID, ulGrabFrameID) )
					{
						if( usState!=0 )
						{
							continue;
						}
						if( labs(lDummyX-lEncX)<lDieSizeTolX && labs(lDummyY-lEncY)<lDieSizeTolY )
						{
							lEncX = lDummyX;
							lEncY = lDummyY;
							dDieAngle = dAngle;
							szDieBin = szNewBin;
							usDieState = usState;
							ulFrameID = ulGrabFrameID;
						}
					}
				}
			}

			SetScanMapRecordID(ulInRow, ulInCol, (UCHAR) uwScanDiePrID);
			SetScanInfo(ulInRow, ulInCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);	//	from physical map builder
			SetPrescanFrameInfo(ulInRow, ulInCol, ulFrameID);
			if( fpOut!=NULL )
			{
				fprintf(fpOut, "%ld,%ld,%lu,%lu,%ld,%ld\n", ulInRow, ulInCol, ulRow, ulCol, lEncX, lEncY);
			}
		}
	}
	SaveScanTimeEvent("WPR: after set BW scan info done from builder.");

	LONG lDieTolX = ulDieSizeXTol;
	LONG lDieTolY = ulDieSizeYTol;
	WSClearScanRemainDieList();

	for(ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
	{
		if( pbGetOut[ulIndex]==true )
		{
			if( fpOut!=NULL )
			{
				fprintf(fpOut, "indexed is %lu\n", ulIndex);	//v4.49 Klocwork
			}
			continue;
		}

		if( WSGetPrescanPosn(ulIndex, lEncX, lEncY)==false )
		{
			continue;
		}

		LONG lEncX1 = 0, lEncY1 = 0;
		for(ULONG j=0; j<lRefDieCount; j++)
		{
			lEncX1 = dwaRefWfX.GetAt(j);
			lEncY1 = dwaRefWfY.GetAt(j);
			if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
			{
				pbGetOut[ulIndex] = true;
				if( fpOut!=NULL )
					fprintf(fpOut, "indexed is %lu done at refer %lu\n", ulIndex, j);
				break;
			}
		}

		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}

		for(ULONG j=1; j<=WSGetPrescanTotalDie(); j++)
		{
			if( WSGetPrescanPosn(j, lEncX1, lEncY1) )
			{
				if( j<ulIndex )
				{
					if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
					{
						pbGetOut[ulIndex] = true;
						if( fpOut!=NULL )
							fprintf(fpOut, "indexed is %lu done at %lu\n", ulIndex, j);
						break;
					}
				}
				else if( j>ulIndex )
				{
					if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
					{
						if( pbGetOut[j]==true )
						{
							pbGetOut[ulIndex] = true;
							if( fpOut!=NULL )
								fprintf(fpOut, "indexed is %lu done at %lu\n", ulIndex, j);
							break;
						}
					}
				}
			}
		}
		if( pbGetOut[ulIndex]==true )
		{
			continue;
		}
		if( fpOut!=NULL )
		{
			fprintf(fpOut, "index %lu to remain list,%ld,%ld\n", ulIndex, lEncX, lEncY);
		}
		pbGetOut[ulIndex] = true;
		WSAddScanRemainDieIntoList(lEncX, lEncY, ulIndex);
	}

	if( fpOut!=NULL )
	{
		fclose(fpOut);
	}

	delete [] pbGetOut;

	SaveScanTimeEvent("WPR: to remove all data of physical map builder.");

	m_cPIMap.RemoveAll();
	m_cPIMap.SetScan2Pr(FALSE);

	// transfer data from prescan result to map physical position array
	LONG lUserRow = 0, lUserCol = 0;

	CStringArray	szaPrSummaryList;
	nMaxSpan = 5;

	FILE *fpOcr = NULL;
	if( IsCharDieInUse() && IsBLInUse() )
	{
		AutoBondWaferScreenUpdate(PR_FALSE);
		AutoBondScreen(FALSE);
		DrawSearchBox(PR_COLOR_GREEN);

		//	should make sure the back light at up level.
		BLZ_Sync();
		UpdateBackLightDownState();
		MoveBackLightWithTableCheck(TRUE);
	}

	for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			if( lAlgnCol>10 )
			{
				ulCtrCol = lAlgnCol - 10;
				break;
			}
		}
	}

	if( IsCharDieInUse() )
	{
		CString szOcrFullName = szLogPath + ".OCR";	//	to gen the ocr value file.
		DeleteFile(szOcrFullName);
		fpOcr = NULL;
		errno_t nErr = fopen_s(&fpOcr, szOcrFullName, "wt");
		if ((nErr == 0) && (fpOcr != NULL))
		{
			LONG lUserRow = 0, lUserCol = 0;
			ConvertAsmToOrgUser(ulMapRowMax/2, ulCtrCol, lUserRow, lUserCol);
			fprintf(fpOcr, "Map Center,%d,%d\n", lUserRow, lUserCol);
			fprintf(fpOcr, "construct scan map,%d,%d,%d,%d\n", 
				ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY);
			for(ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
			{
				if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
				{
					ConvertAsmToOrgUser(lAlgnRow, lAlgnCol, lUserRow, lUserCol);
					fprintf(fpOcr, "added refer points,%d,%d\n", lUserRow, lUserCol);
				}
			}
		}
	}

	ULONG ulEmptyIdx = 0, ulDefectIdx = 0, ulBadCutIdx = 0, ulExtraIdx = 0, ulGoodIdx = 0;
	ULONG ulMapTotal = 0;

	SaveScanTimeEvent("WPR: get out complete and to update wafer map");
	LONG lTolX = (ulDiePitchX*50/100);
	LONG lTolY = (ulDiePitchY*50/100);

	// build up the physical map firstly, alignment at left
	pUtl->GetAlignPosition(lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY);

	// low part of left
	FillUpBarWaferMap(lAlgnRow,	  ulMapRowMax, ulCtrCol, 0, TRUE,  TRUE, lTolX, lTolY);
	// up part of left
	FillUpBarWaferMap(lAlgnRow-1,			0, ulCtrCol, 0, FALSE, TRUE, lTolX, lTolY);

	// if have added refere point at right, build up map
	if( pUtl->GetAssistPointsNum()>0 )
	{
		pUtl->GetAssistPosition(0, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY);
		// low part of right
		FillUpBarWaferMap(lAlgnRow, ulMapRowMax, ulCtrCol+1, ulMapColMax, TRUE,  FALSE, lTolX, lTolY);
		// up part of right
		FillUpBarWaferMap(lAlgnRow-1,		  0, ulCtrCol+1, ulMapColMax, FALSE, FALSE, lTolX, lTolY);
	}

	DelPrescanRunPosn();
#ifdef PR_INSP_MACHINE
	PR_COORD stDieOffset;
	PR_REAL	 fDieRotate;

	CString szAlarmMsg = "";
	ConvertAsmToOrgUser(0, ulCtrCol, lUserRow, lUserCol);
	szAlarmMsg.Format("OCR Map center column %d(%d).",	lUserCol, ulCtrCol);
	SetAlarmLog(szAlarmMsg);

	CStringArray szOcrList, szBinList;
    szOcrList.RemoveAll();
	szBinList.RemoveAll();

	if( IsCharDieInUse() )
	{
		// xxx file is fixed and copied from serve when do alignment.
		CString szLclFullName	= PRESCAN_RESULT_FULL_PATH + "OcrBarWafer.xxx";
		CStdioFile fCheckFile;
		if( fCheckFile.Open(szLclFullName, CFile::modeRead|CFile::shareDenyNone) )
		{
			CStringArray szaList;
			CString szRawData;
			// seek to raw data part
			while( fCheckFile.ReadString(szRawData) )
			{
				if( szRawData.CompareNoCase("Encoding,Test_BIN")==0 )
				{
					break;
				}
			}

			while( fCheckFile.ReadString(szRawData) )
			{
				szaList.RemoveAll();
				CUtility::Instance()->ParseRawData(szRawData, szaList);
				if( szaList.GetSize()>=2 )
				{
					szOcrList.Add(szaList.GetAt(0));
					szBinList.Add(szaList.GetAt(1));
				}
			}
			fCheckFile.Close();
		}
	}

	for(ULONG ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		//	try to find left part of this row match point
		LONG lGoodCount_L = 0, lGoodCount_R = 0;
		for(ULONG ulCol=0; ulCol<ulCtrCol; ulCol++)
		{
			if( GetMapPhyPosn(ulRow, ulCol, lEncX, lEncY) )
			{
				lGoodCount_L++;
			}
		}
		if( lGoodCount_L<=1 )
		{
			for(ULONG ulCol=0; ulCol<ulCtrCol; ulCol++)
			{
				if( m_WaferMapWrapper.GetGrade(ulRow, ulCol)!=ucNullBin )
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
			}
		}
		for(ULONG ulCol=ulCtrCol; ulCol<ulMapColMax; ulCol++)
		{
			if( GetMapPhyPosn(ulRow, ulCol, lEncX, lEncY) )
			{
				lGoodCount_R++;
			}
		}
		if( lGoodCount_R<=1 )
		{
			for(ULONG ulCol=ulCtrCol; ulCol<ulMapColMax; ulCol++)
			{
				if( m_WaferMapWrapper.GetGrade(ulRow, ulCol)!=ucNullBin )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
				}
			}
		}

		if( lGoodCount_L<=1 && lGoodCount_R<=1 )
		{
			continue;
		}

		LONG lStartLoop = 1, lEndLoop = 3;
		if( lGoodCount_L<8 )
		{
			lStartLoop = 0;
			lEndLoop = lGoodCount_L/2-1;
		}
		if( IsCharDieInUse()==FALSE )
		{
			lEndLoop = -1;
		}
		SetAlarmLog("\n");
		szAlarmMsg.Format("OCR Map row(%d) left  part loop (%d,%d) good %d.", ulRow, lStartLoop, lEndLoop, lGoodCount_L);
		SetAlarmLog(szAlarmMsg);

		BOOL bFindRowOCR_L = FALSE;
		LONG lOcrColIndex_L = 0, lMapColIndex_L = 0;
		for(LONG ulLoop=lStartLoop; ulLoop<=lEndLoop; ulLoop++)
		{
			LONG lLeftCol = -1, lRightCol = -1;
			LONG lLoopCount = 0;
			for(ULONG ulCol=0; ulCol<ulCtrCol; ulCol++)
			{
				bInfo = GetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
				if( bInfo )
				{
					if( lLoopCount==ulLoop )
					{
						lLeftCol = ulCol;
						break;
					}
					lLoopCount++;
				}
			}
			lLoopCount = 0;
			for(ulCol=ulCtrCol; ulCol>0; ulCol--)
			{
				bInfo = GetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
				if( bInfo )
				{
					if( lLoopCount==ulLoop )
					{
						lRightCol = ulCol;
						break;
					}
					lLoopCount++;
				}
			}

			if( lLeftCol==-1 || lRightCol==-1 || lLeftCol>lRightCol )
			{
				break;
			}

			LONG lLeftOcrCol = -1, lLeftMapCol = -1, lRightOcrCol = -1, lRightMapCol = -1;
			//	here move table, shown on map, do pr, get surface value.
			m_WaferMapWrapper.SetCurrentPosition(ulRow, lLeftCol);
			GetMapPhyPosn(ulRow, lLeftCol, lEncX, lEncY);
			MoveWaferTable(lEncX, lEncY);
			CString szLeftOcr = "";
			for(int kkk=1; kkk<5; kkk++)
			{
				Sleep(100*kkk);
				if( SearchAoiOcrDie(&stDieOffset, &fDieRotate)==TRUE )
				{
					szAlarmMsg.Format("OCR Map LL %d OCR %s", kkk, m_szAoiOcrPrValue);
					SetAlarmLog(szAlarmMsg);
					CString szValue = m_szAoiOcrPrValue;
					szLeftOcr = szValue.Right(4);

					//	look into map file data and find its grade/state
					for(int i=0; i<szOcrList.GetSize(); i++)
					{
						if( szLeftOcr.Compare(szOcrList.GetAt(i))==0 )
						{
							lLeftOcrCol = i;
							lLeftMapCol = lLeftCol;
							break;
						}
					}
				}
				if( lLeftOcrCol!=-1 )
				{
					break;
				}
			}
			//	here move table, shown on map, do pr, get surface value.
			m_WaferMapWrapper.SetCurrentPosition(ulRow, lRightCol);
			GetMapPhyPosn(ulRow, lRightCol, lEncX, lEncY);
			MoveWaferTable(lEncX, lEncY);
			CString szRightOcr = "";
			for(int kkk=1; kkk<5; kkk++)
			{
				Sleep(100*kkk);
				if( SearchAoiOcrDie(&stDieOffset, &fDieRotate)==TRUE )
				{
					szAlarmMsg.Format("OCR Map LR %d OCR %s", kkk, m_szAoiOcrPrValue);
					SetAlarmLog(szAlarmMsg);
					CString szValue = m_szAoiOcrPrValue;
					szRightOcr = szValue.Right(4);

					//	look into map file data and find its grade/state
					for(int i=0; i<szOcrList.GetSize(); i++)
					{
						if( szRightOcr.Compare(szOcrList.GetAt(i))==0 )
						{
							lRightOcrCol = i;
							lRightMapCol = lRightCol;
							break;
						}
					}
				}
				if(lRightOcrCol!=-1  )
					break;
			}

			LONG lUserRow = 0, lUserColL = 0, lUserColR = 0;
			ConvertAsmToOrgUser(ulRow,  lLeftCol, lUserRow, lUserColL);
			ConvertAsmToOrgUser(ulRow, lRightCol, lUserRow, lUserColR);
			szAlarmMsg.Format("OCR Map left  part L col %d(%d) PR %s OCR index %d; R col %d(%d) PR %s OCR index %d.",
				lUserColL,  lLeftCol,  szLeftOcr,  lLeftOcrCol, 
				lUserColR, lRightCol, szRightOcr, lRightOcrCol);
			SetAlarmLog(szAlarmMsg);

			if( lLeftOcrCol!=-1 && lRightOcrCol!=-1 )
			{
				if( (lRightMapCol-lLeftMapCol)==(lRightOcrCol-lLeftOcrCol) )
				{
					lMapColIndex_L = lLeftMapCol;
					lOcrColIndex_L = lLeftOcrCol;
					bFindRowOCR_L = TRUE;
					break;
				}
			}
		}

		if( IsCharDieInUse() && bFindRowOCR_L==FALSE && lGoodCount_L>1 )
		{
			LONG lUserRow = 0, lUserCol = 0;
			ConvertAsmToOrgUser(ulRow, ulCtrCol, lUserRow, lUserCol);
			szAlarmMsg.Format("OCR Map row %d(%d), column %d(%d) left  part, OCR mismatch with wafer.",
				lUserRow, ulRow, lUserCol, ulCtrCol);
			HmiMessage_Red_Back(szAlarmMsg);
		}

		// try to find right part OCR match point
		BOOL bFindRowOCR_R = FALSE;
		LONG lOcrColIndex_R = 0, lMapColIndex_R = 0;

		lStartLoop = 1;
		lEndLoop = 3;
		if( lGoodCount_R<8 )
		{
			lStartLoop = 0;
			lEndLoop = lGoodCount_R/2-1;
		}
		if( IsCharDieInUse()==FALSE )
		{
			lEndLoop = -1;
		}
		szAlarmMsg.Format("OCR Map row(%d) right part loop (%d,%d) good %d.", ulRow, lStartLoop, lEndLoop, lGoodCount_R);
		SetAlarmLog(szAlarmMsg);

		for(LONG ulLoop=lStartLoop; ulLoop<lEndLoop; ulLoop++)
		{
			LONG lLeftCol = -1, lRightCol = -1;
			LONG lLoopCount = 0;
			for(ULONG ulCol=ulCtrCol; ulCol<ulMapColMax; ulCol++)
			{
				bInfo = GetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
				if( bInfo )
				{
					if( lLoopCount==ulLoop )
					{
						lLeftCol = ulCol;
						break;
					}
					lLoopCount++;
				}
			}
			lLoopCount = 0;
			for(ULONG ulCol=ulMapColMax; ulCol>ulCtrCol; ulCol--)
			{
				bInfo = GetMapPhyPosn(ulRow, ulCol, lEncX, lEncY);
				if( bInfo )
				{
					if( lLoopCount==ulLoop )
					{
						lRightCol = ulCol;
						break;
					}
					lLoopCount++;
				}
			}

			if( lLeftCol==-1 || lRightCol==-1 || lLeftCol>lRightCol )
			{
				break;
			}

			LONG lLeftOcrCol = -1, lLeftMapCol = -1, lRightOcrCol = -1, lRightMapCol = -1;
			//	here move table, shown on map, do pr, get surface value.
			m_WaferMapWrapper.SetCurrentPosition(ulRow, lLeftCol);
			GetMapPhyPosn(ulRow, lLeftCol, lEncX, lEncY);
			MoveWaferTable(lEncX, lEncY);
			CString szLeftOcr = "";
			for(int kkk=1; kkk<5; kkk++)
			{
				Sleep(100*kkk);
				if( SearchAoiOcrDie(&stDieOffset, &fDieRotate)==TRUE )
				{
					szAlarmMsg.Format("OCR Map RL %d OCR %s", kkk, m_szAoiOcrPrValue);
					SetAlarmLog(szAlarmMsg);
					CString szValue = m_szAoiOcrPrValue;
					szLeftOcr = szValue.Right(4);

					//	look into map file data and find its grade/state
					for(int i=0; i<szOcrList.GetSize(); i++)
					{
						if( szLeftOcr.Compare(szOcrList.GetAt(i))==0 )
						{
							lLeftOcrCol = i;
							lLeftMapCol = lLeftCol;
							break;
						}
					}
				}
				if( lLeftOcrCol!=-1 )
					break;
			}
			//	here move table, shown on map, do pr, get surface value.
			m_WaferMapWrapper.SetCurrentPosition(ulRow, lRightCol);
			GetMapPhyPosn(ulRow, lRightCol, lEncX, lEncY);
			MoveWaferTable(lEncX, lEncY);
			CString szRightOcr = "";
			for(int kkk=1; kkk<5; kkk++)
			{
				Sleep(100*kkk);
				if( SearchAoiOcrDie(&stDieOffset, &fDieRotate)==TRUE )
				{
					szAlarmMsg.Format("OCR Map RR %d OCR %s", kkk, m_szAoiOcrPrValue);
					SetAlarmLog(szAlarmMsg);
					CString szValue = m_szAoiOcrPrValue;
					szRightOcr = szValue.Right(4);

					//	look into map file data and find its grade/state
					for(int i=0; i<szOcrList.GetSize(); i++)
					{
						if( szRightOcr.Compare(szOcrList.GetAt(i))==0 )
						{
							lRightOcrCol = i;
							lRightMapCol = lRightCol;
							break;
						}
					}
				}
				if( lRightOcrCol!=1 )
				{
					break;
				}
			}

			LONG lUserRowL = 0, lUserColL = 0, lUserRowR = 0, lUserColR = 0;
			ConvertAsmToOrgUser(ulRow, lLeftCol, lUserRowL, lUserColL);
			ConvertAsmToOrgUser(ulRow, lRightCol, lUserRowR, lUserColR);
			szAlarmMsg.Format("OCR Map right part L col %d(%d) PR %s OCR index %d; R col %d(%d) PR %s OCR index %d.",
				lUserColL,  lLeftCol,  szLeftOcr,  lLeftOcrCol, 
				lUserColR, lRightCol, szRightOcr, lRightOcrCol);
			SetAlarmLog(szAlarmMsg);

			if( lLeftOcrCol!=-1 && lRightOcrCol!=-1 )
			{
				if( (lRightMapCol-lLeftMapCol)==(lRightOcrCol-lLeftOcrCol) )
				{
					lMapColIndex_R = lLeftMapCol;
					lOcrColIndex_R = lLeftOcrCol;
					bFindRowOCR_R = TRUE;
					break;
				}
			}
		}

		if( IsCharDieInUse() && bFindRowOCR_R==FALSE && lGoodCount_R>1 )
		{
			LONG lUserRow = 0, lUserCol = 0;
			ConvertAsmToOrgUser(ulRow, ulCtrCol, lUserRow, lUserCol);
			szAlarmMsg.Format("OCR Map row %d(%d), column %d(%d) right part, OCR mismatch with wafer.",
				lUserRow, ulRow, lUserCol, ulCtrCol);
			HmiMessage_Red_Back(szAlarmMsg);
		}

		// update this column contents.
		for(ULONG ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			BOOL bNeedChangeGrade = FALSE;
			BOOL bConvertGrade = FALSE;
			UCHAR ucScnGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			UCHAR ucOldGrade = ucScnGrade;

			if( ucScnGrade==ucNullBin )
			{
				continue;
			}

			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);

			if( bInfo==FALSE )
			{
				BOOL bGetPred = GetDieValidInX(ulRow, ulCol, nMaxSpan, lEncX, lEncY, FALSE);
				if( bGetPred )
				{
					PR_UWORD uwDiePrID = 0;
					if( FindAndFillFromRemainList(ulRow, ulCol, lTolX, lTolY, lEncX, lEncY, uwDiePrID) )
					{
						m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
						bInfo = TRUE;
					}
				}
			}

				if( bInfo )
			{
				ulMapTotal++;
			}

			CString szDieOcrValue = "";
			UCHAR ucChangeGrade = ucScnGrade;
			BOOL bNgDieGrade = FALSE;
			if( bInfo )
			{
				UCHAR ucMapGrade = ucScnGrade - ucOffset;
				if( IsCharDieInUse() && ulCol<ulCtrCol && bFindRowOCR_L )
				{
					LONG i = ulCol + lOcrColIndex_L - lMapColIndex_L;
					if( i>=szBinList.GetSize() || i<0 )
					{
						if( ucMapGrade>1 )
							ucMapGrade = ucMapGrade - 1;
					}
					else
					{
						bConvertGrade = TRUE;
						ucMapGrade = atoi(szBinList.GetAt(i));
						if( i<szOcrList.GetSize() )
						{
							szDieOcrValue = szOcrList.GetAt(i);
						}
					}
				}
				else if( IsCharDieInUse() && ulCol>ulCtrCol && bFindRowOCR_R )
				{
					LONG i = ulCol + lOcrColIndex_R - lMapColIndex_R;
					if( i>=szBinList.GetSize()|| i<0 )
					{
						if( ucMapGrade>1 )
							ucMapGrade = ucMapGrade - 1;
					}
					else
					{
						bConvertGrade = TRUE;
						ucMapGrade = atoi(szBinList.GetAt(i));
						if( i<szOcrList.GetSize() )
						{
							szDieOcrValue = szOcrList.GetAt(i);
						}
					}
				}
				else
				{
					if( IsCharDieInUse() )
					{
						if( ucMapGrade>2 )
							ucMapGrade = ucMapGrade - 2;
					}
					else
					{
						bConvertGrade = TRUE;
					}
				}

				if (bIsBadCut)
				{
					bNeedChangeGrade = TRUE;
					SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucOldGrade);
					ulBadCutIdx++;

					if( IsUseGradeConvert()==FALSE && bBadCutAction )
					{
						ucMapGrade = ucBadCutGrade - ucOffset;
						bNgDieGrade = TRUE;
					}
					else if( IsNoPickBadCut() && !IsUseGradeConvert() && !bBadCutAction )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
					}
				}
				else if( bIsFakeEmpty )
				{
					bNeedChangeGrade = TRUE;
					SetDefectFull(ulDefectIdx, ulRow, ulCol, ucOldGrade);
					ulDefectIdx++;

					if( IsUseGradeConvert()==FALSE && bFakeAction )
					{
						ucMapGrade = ucFakeGrade - ucOffset;
						bNgDieGrade = TRUE;
					}
					else if( !IsUseGradeConvert() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);
					}
				}
				else if( bIsDefect )
				{
					bNeedChangeGrade = TRUE;
					SetDefectFull(ulDefectIdx, ulRow, ulCol, ucOldGrade);
					ulDefectIdx++;

					if( IsUseGradeConvert()==FALSE && bDefectAction )
					{
						ucMapGrade = ucDefectGrade - ucOffset;
						bNgDieGrade = TRUE;
					}
					else if( IsNoPickDefect() && !IsUseGradeConvert() && !bDefectAction )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
					}
				}
				else
				{
					ulGoodIdx++;
					if( IsUseGradeConvert()==FALSE && bGoodAction )
					{
						ucMapGrade = ucGoodGrade - ucOffset;
					}
				}

				UCHAR ucPrBin = 0;
				if( szDieBin.IsEmpty()==FALSE )
					ucPrBin = (UCHAR)atoi(szDieBin);
				if( IsUseGradeConvert() && m_lGradeMappingLimit>0 )
				{
					CString szTemp, szAppend = "";
					if( m_bUseAoiGradeConvert && szDieBin.IsEmpty()==FALSE && bConvertGrade )
					{
						szTemp.Format("map (%d,%d) grade %d DieBin %s Bin %d ==> ", ulRow, ulCol, ucMapGrade, szDieBin, ucPrBin);
						for(int i=0; i<m_lGradeMappingLimit; i++)
						{
							if( m_sMapOrgGrade[i]==ucMapGrade && m_sPrInspectGrade[i]==ucPrBin )
							{
								ucMapGrade = (UCHAR) m_sMapNewGrade[i];
								szAppend.Format("map grade aoi convert to %d", ucMapGrade);
								break;
							}
						}
					}

					if( m_bScanNgGradeConvert )
					{
						UCHAR ucChkGrade = ucMapGrade;
						szTemp.Format("map (%d,%d) grade %d ==> ", ulRow, ulCol, ucChkGrade);
						if( m_sMapOrgGrade[ucChkGrade]==ucChkGrade )
						{
							if( bNeedChangeGrade )
							{
								ucMapGrade = (UCHAR) m_sMapNewGrade[ucChkGrade];
							}
							else
							{
								ucMapGrade = (UCHAR) m_sPrInspectGrade[ucChkGrade];
							}
							szAppend.Format("map grade convert to %d", ucMapGrade);
						}
					}

					szTemp += szAppend;
				}

				ucChangeGrade = ucMapGrade + ucOffset;
				if( m_bSaveNgDieSubGrade && (GetPsmEnable()==FALSE) )
				{
					ucChangeGrade = ucChangeGrade + ucPrBin;
				}
			}
			else // set to empty grade
			{
				lEncX = lEncY = 0;
				SetEmptyFull(ulEmptyIdx, ulRow, ulCol, ucOldGrade);
				ulEmptyIdx++;
				if( !bEmptyAction )
				{
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
				}

				if( bEmptyAction )
					ucChangeGrade = ucEmptyGrade;
			}
			m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucChangeGrade);

			LONG lUserRow = 0, lUserCol = 0;
			ConvertAsmToOrgUser(ulRow, ulCol, lUserRow, lUserCol);
			DOUBLE dPrX = 0, dPrY = 0;
			ConvertMotorStepToPrPixel(lEncX, lEncY, dPrX, dPrY);

			UCHAR  ucNewGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol) - ucOffset;
			USHORT usOrgGrade = m_WaferMapWrapper.GetOriginalGrade(ucOldGrade) - ucOffset;

			LONG lFrameID = -1;
			if( bInfo )
			{
				ULONG ulFrameID = 0;
				GetPrescanFrameInfo(ulRow, ulCol,  ulFrameID);
				lFrameID = ulFrameID ;
			}

			CString szLineData;
			szLineData.Format("%ld,%.0f,%.0f,%lu,%lu,%ld,%ld,%ld,%d,%d,%d", lFrameID, dPrX, dPrY, 
				ulRow, ulCol, lUserRow, lUserCol, 1, ucNewGrade, ucOldGrade - ucOffset, usOrgGrade);

			CHAR szXY[5] = "";
			if( szDieOcrValue.GetLength()<5 )
			{
				if( fpOcr!=NULL )
				{
					fprintf(fpOcr, "%d,%d,%s\n", lUserRow, lUserCol, szDieOcrValue);
				}
				strcpy_s(szXY, sizeof(szXY), szDieOcrValue);
			}
			else
			{
				szXY[0] = NULL;
			}

			szLineData += ",";
			szLineData += szXY;
			szaPrSummaryList.Add(szLineData);
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
	szOcrList.RemoveAll();
	szBinList.RemoveAll();
#else
	SaveScanTimeEvent("WPR: loop to fill in map position.");

	for(ULONG ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			BOOL bIsIgnore = FALSE;
			BOOL bToUpdatePosn = TRUE;
			BOOL bToChangeGrade = TRUE;
			UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);

			BOOL bIsExtra = FALSE;
			if( ucB4ScanGrade==ucNullBin )
			{
				if( bExtraAction )
				{
					bIsExtra = TRUE;
				}
				else
				{
					bIsIgnore = TRUE;
					bToUpdatePosn = FALSE;
				}
			}

			if( bIsExtra!=TRUE && m_bPrescanEmptyToNullBin==FALSE && m_bPrescanEmptyAction && ucB4ScanGrade==ucEmptyGrade )
			{
				bIsIgnore = TRUE;
			}

			if( IsScanMapNgGrade(ucB4ScanGrade-ucOffset) )
			{
				bToChangeGrade = FALSE;
			}

			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			bInfo = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);

			if( bInfo )
			{
				if( bToUpdatePosn )
				{
					m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
					SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
				}
				else
				{
					if( IsInMapValidRange(ulRow, ulCol) )
					{
						SetExtraInfo(ulExtraIdx, ulRow, ulCol);
						ulExtraIdx++;
					}
				}
			}

			if( bInfo==FALSE && bIsExtra )
			{
				bIsIgnore = TRUE;
			}
			if( bIsIgnore )
			{
				continue;
			}

			BOOL bIsRefer = FALSE;
			if( ucReferGrade==ucB4ScanGrade )
				bIsRefer = m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol);
			BOOL bIsSpRef = IsSpecialReferGrade(ulRow, ulCol);
			BOOL bIsNoDie = IsNoDieGrade(ulRow, ulCol);
			if( m_WaferMapWrapper.GetDieState(ulRow, ulCol)==WT_MAP_DIESTATE_DIMREFER )
				bIsRefer = TRUE;

			if( bIsRefer )
			{
				CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
				if( pApp->GetCustomerName()=="Huga" )	// MARK REFER DIE
				{
					COLORREF crUserColor = RGB(255,5,5); //(in Red color)
					m_WaferMapWrapper.AddDescription(ulRow, ulCol, 96, crUserColor, "R");
				}
			}

			if( bIsRefer || bIsSpRef || bIsNoDie )
			{
				continue;
			}

			BOOL bScoreCount = TRUE;
			if( IsAOIOnlyMachine() && m_bPrescanSkipNgGrade && bToChangeGrade==FALSE )
			{
				bScoreCount = FALSE;
			}

			if( m_bPrescanSkipNgGrade && m_ucScanMapStartNgGrade==0 && ucB4ScanGrade==ucOffset )
			{
				bScoreCount = FALSE;
			}

			if( m_WaferMapWrapper.GetReader()!=NULL && m_WaferMapWrapper.GetReader()->IsInvalidDie(ulRow, ulCol) )
			{
				bScoreCount = FALSE;
			}

			if( bIsExtra )
			{
				bScoreCount = FALSE;
			}

			if( bScoreCount )
			{
				ulMapTotal++;
			}

			if( bInfo==FALSE )
			{
				BOOL bGetPred = GetDieValidInX(ulRow, ulCol, nMaxSpan, lEncX, lEncY, FALSE);

				if( bGetPred )
				{
					PR_UWORD uwDiePrID = 0;
					if( FindAndFillFromRemainList(ulRow, ulCol, lDieTolX, lDieTolY, lEncX, lEncY, uwDiePrID) )
					{
						bInfo = TRUE;
						m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
						SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
					}
				}
			}

			CString szDieOcrValue = "";
			if( bInfo )
			{
				BOOL bNeedChangeGrade = FALSE;
				UCHAR ucChangeGrade = ucB4ScanGrade;
				BOOL bNgDieGrade = FALSE;

				if( bIsExtra )
				{
					SetExtraInfo(ulExtraIdx, ulRow, ulCol);
					ulExtraIdx++;

					if( IsNoPickExtra() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_UNPICK_SCAN_EXTRA);
					}

					if( bToChangeGrade )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucExtraGrade;
					}
				}
				else
				if (bIsBadCut)
				{
					if( bScoreCount )
					{
						SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucB4ScanGrade);
						ulBadCutIdx++;
					}

					if( IsNoPickBadCut() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
					}

					if( bBadCutAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucBadCutGrade;
							bNgDieGrade = TRUE;
						}
					}
					else
					{
						if( m_bPrescanBadCutMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else if( bIsFakeEmpty )
				{
					if( bScoreCount )
					{
						SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
						ulDefectIdx++;
					}

					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);

					if( bFakeAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucFakeGrade;
							bNgDieGrade = TRUE;
						}
					}
				}
				else if( bIsDefect )
				{
					if( bScoreCount )
					{
						SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
						ulDefectIdx++;
					}

					if( IsNoPickDefect() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
					}

					if( bDefectAction )
					{
						if( bToChangeGrade )
						{
							bNeedChangeGrade = TRUE;
							ucChangeGrade = ucDefectGrade;
							bNgDieGrade = TRUE;
						}
					}
					else
					{
						if( m_bPrescanDefectMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else
				{
					if( bScoreCount )
					{
						ulGoodIdx++;
					}
					if( bGoodAction && bToChangeGrade )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucGoodGrade;
					}
				}

				UCHAR ucDieBin = 0;
				if( IsAOIOnlyMachine() && szDieBin.IsEmpty()==FALSE )
				{
					ucDieBin = (UCHAR)atoi(szDieBin);
				}
				if( m_bSaveNgDieSubGrade && bNeedChangeGrade && (GetPsmEnable()==FALSE) )
				{
					ucChangeGrade += ucDieBin;
				}

				if( IsUseGradeConvert() && m_lGradeMappingLimit>0 )
				{
					CString szTemp, szAppend = "";
					if( m_bUseAoiGradeConvert )
					{
						szTemp.Format("map (%d,%d) grade %d DieBin %s Bin %d ==> ", ulRow, ulCol, (ucB4ScanGrade - ucOffset), szDieBin, ucDieBin);
						for(int i=0; i<m_lGradeMappingLimit; i++)
						{
							if( m_sMapOrgGrade[i]==(ucB4ScanGrade - ucOffset) && m_sPrInspectGrade[i]==ucDieBin )
							{
								ucChangeGrade = (UCHAR) m_sMapNewGrade[i];
								bNeedChangeGrade = TRUE;
								szAppend.Format("map grade aoi convert to %d", ucChangeGrade);
								ucChangeGrade = ucChangeGrade + ucOffset;
								break;
							}
						}
					}

					if( m_bScanNgGradeConvert )
					{
						UCHAR ucChkGrade = ucB4ScanGrade - ucOffset;
						szTemp.Format("map (%d,%d) grade %d ==> ", ulRow, ulCol, ucChkGrade);
						if( m_sMapOrgGrade[ucChkGrade]==ucChkGrade )
						{
							if( bNeedChangeGrade )
							{
								ucChangeGrade = (UCHAR) m_sMapNewGrade[ucChkGrade];
							}
							else
							{
								ucChangeGrade = (UCHAR) m_sPrInspectGrade[ucChkGrade];
							}
							szAppend.Format("map grade convert to %d", ucChangeGrade);
							ucChangeGrade = ucChangeGrade + ucOffset;
						}
					}
					szTemp += szAppend;
				}

				if( bNeedChangeGrade )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucChangeGrade);
				}
			}
			else // set to empty grade
			{
				lEncX = lEncY = 0;
				if( bScoreCount )
				{
					SetEmptyFull(ulEmptyIdx, ulRow, ulCol, ucB4ScanGrade);
					ulEmptyIdx++;
					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
				}

				if( bEmptyAction )
				{
					if( bToChangeGrade )
					{
						m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
					}
				}
				else
				{
					if( m_bPrescanEmptyMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						UpdateDie(ulRow, ulCol, ucB4ScanGrade, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
					}
				}
			}
		}
	}

	m_WaferMapWrapper.BatchPhysicalPositionUpdate();
#endif

	if( fpOcr!=NULL )
	{
		fclose(fpOcr);
	}

	if( IsCharDieInUse() && IsBLInUse() )
	{
		AutoBondWaferScreenUpdate(PR_TRUE);
		AutoBondScreen(TRUE);
		DrawSearchBox(PR_COLOR_GREEN);

		//	should make sure the back light at low level.
		MoveBackLightWithTableCheck(FALSE);
		BLZ_Sync();
		UpdateBackLightDownState();
	}

	SaveScanTimeEvent("WPR: get out complete and to update wafer map done");
	m_lTimeSlot[8] = (LONG)(GetTime()-dUsedTime);	//	08.	update wafer map grade and position

	BOOL bFailBreak = FALSE;
	for(ULONG ulRow=0; ulRow<=ulMapRowMax; ulRow++)
	{
		for(ULONG ulCol=0; ulCol<=ulMapColMax; ulCol++)
		{
			BOOL bIsIgnore = FALSE;

			LONG lEncX1, lEncY1, lEncX2, lEncY2;
			if( GetPrescanPosition(ulRow, ulCol, lEncX1, lEncY1) )
			{
				if( GetPrescanPosition(ulRow, ulCol+1, lEncX2, lEncY2) )
				{
					if( labs(lEncX1-lEncX2)<lDieTolX && 
						labs(lEncY1-lEncY2)<lDieTolY )
					{
						LONG lUserRow1, lUserCol1, lUserRow2, lUserCol2;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow1, lUserCol1);
						ConvertAsmToOrgUser(ulRow, ulCol+1, lUserRow2, lUserCol2);
						CString szMsg;
						szMsg.Format("Map die (%d,%d) = (%d,%d).\nPosition overlap, please stop!", lUserRow1, lUserCol1, lUserRow2, lUserCol2);
						HmiMessage_Red_Back(szMsg, "Hisense");
						m_lPrescanVerifyResult += 16384;
						SetErrorMessage(szMsg);
						bFailBreak = TRUE;
						break;
					}
				}
			}
			if( bFailBreak )
			{
				break;
			}
		}
		if( bFailBreak )
		{
			break;
		}
	}

	UpdateBackLightDownState();
	SaveScanTimeEvent("WPR: PASS DATA AND UPDATE GRADE LIST");

	lHomeNewRow += m_lMapScanOffsetRow;
	lHomeNewCol += m_lMapScanOffsetCol;

	pUtl->SetAlignPosition(lHomeNewRow, lHomeNewCol, lHomeWfX, lHomeWfY);

	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulEmptyIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= ulExtraIdx;

	szaPrSummaryList.RemoveAll();

	SetMapTotalDie(ulMapTotal);

	// build prescan map complete
	ClearGoodInfo();	ClearWSPrescanInfo();	WSClearScanRemainDieList();
	DelPrescanFrameInfo();

	UpdateBackLightDownState();

	SaveScanTimeEvent("WPR: CLEAR BUILDing materials");

	SendPrSummaryFileToEagle();

	UpdateBackLightDownState();

	dwaRefWfX.RemoveAll();
	dwaRefWfY.RemoveAll();
	dwaRefRow.RemoveAll();
	dwaRefCol.RemoveAll();
}	//	ocr bar wafer

#if	USE_NEW_EMCORE_BARWAFER
VOID CWaferPr::ConstructPrescanMap_EmcoreBW()
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	UCHAR ucNullBin		= m_WaferMapWrapper.GetNullBin();
	UCHAR ucOffset		= m_WaferMapWrapper.GetGradeOffset();
	UCHAR ucGoodGrade	= m_ucPrescanGoodGrade + ucOffset;
	UCHAR ucEmptyGrade	= m_ucPrescanEmptyGrade + ucOffset;
	UCHAR ucDefectGrade	= GetScanDefectGrade() + ucOffset;
	UCHAR ucBadCutGrade	= GetScanBadcutGrade() + ucOffset;

	BOOL  bBadCutAction	= m_bPrescanBadCutAction;
	BOOL  bDefectAction	= m_bPrescanDefectAction;
	BOOL  bEmptyAction	= m_bPrescanEmptyAction;
	BOOL  bGoodAction	= m_bPrescanGoodAction;

	if( m_bPrescanEmptyToNullBin )
		ucEmptyGrade = ucNullBin;
	if( m_bPrescanDefectToNullBin )
		ucDefectGrade = ucNullBin;
	if (m_bPrescanBadCutToNullBin)
		ucBadCutGrade = ucNullBin;

	DOUBLE dUsedTime = GetTime();

	ULONG lRefDieCount = 0;
	CDWordArray aRefListWfX, aRefListWfY, aRefListRow, aRefListCol;
	aRefListWfX.RemoveAll();	aRefListWfY.RemoveAll();
	aRefListRow.RemoveAll();	aRefListCol.RemoveAll();

	ObtainEmcoreBarColumnStart();	//	new method to build bar wafer map.

	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);

	ULONG ulDiePitchX = labs(GetDiePitchX_X());
	ULONG ulDiePitchY = labs(GetDiePitchY_Y());
	LONG  ulStreetInY = labs(GetDiePitchY_Y());
	if( m_nDieSizeX!=0 )
	{
		ulDiePitchY = labs(ulDiePitchX*m_nDieSizeY/m_nDieSizeX);
	}

	// add all alignment point and refer points to list
	LONG lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY, lHomeNewRow, lHomeNewCol;
	pUtl->GetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);
	lHomeNewRow = lHomeOriRow + WPR_SCAN_MAP_OFFSET;
	lHomeNewCol = lHomeOriCol + WPR_SCAN_MAP_OFFSET;

	aRefListWfX.Add(lHomeWfX);		aRefListWfY.Add(lHomeWfY);
	aRefListRow.Add(lHomeNewRow);	aRefListCol.Add(lHomeNewCol);
	lRefDieCount++;

	for(ULONG ulIndex=0; ulIndex<pUtl->GetNumOfReferPoints(); ulIndex++)
	{
		LONG lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY;
		if( pUtl->GetReferPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			aRefListWfX.Add(lAlgnWfX);			aRefListWfY.Add(lAlgnWfY);
			aRefListRow.Add(lAlgnRow + WPR_SCAN_MAP_OFFSET);
			aRefListCol.Add(lAlgnCol + WPR_SCAN_MAP_OFFSET);
			lRefDieCount++;
		}
	}

	for(ULONG ulIndex=0; ulIndex<pUtl->GetAssistPointsNum(); ulIndex++)
	{
		LONG lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY;
		if( pUtl->GetAssistPosition(ulIndex, lAlgnRow, lAlgnCol, lAlgnWfX, lAlgnWfY) )
		{
			if( lAlgnRow==lHomeOriRow && lAlgnCol==lHomeOriCol )
			{
				continue;
			}
			aRefListWfX.Add(lAlgnWfX);			aRefListWfY.Add(lAlgnWfY);
			aRefListRow.Add(lAlgnRow + WPR_SCAN_MAP_OFFSET);
			aRefListCol.Add(lAlgnCol + WPR_SCAN_MAP_OFFSET);
			lRefDieCount++;
		}
	}

	LONG lEncX = 0, lEncY = 0;
	CString szConLogFile = szLogPath + PRESCAN_PASSIN_PSN;
	FILE *fpPassIn = NULL;
	errno_t nPassInErr = fopen_s(&fpPassIn, szConLogFile, "w");
	szConLogFile = szLogPath + PRESCAN_GETOUT_PSN;
	FILE *fpOut = NULL;
	errno_t nOutErr = fopen_s(&fpOut, szConLogFile, "w");

	ULONG ulDieSizeXTol = (ULONG) (GetDieSizeX() * m_dPrescanPitchTolX/100.0);
	ULONG ulDieSizeYTol = (ULONG) (GetDieSizeY() * m_dPrescanPitchTolY/100.0);
	CString szMsg;
	LONG lDieTolX = ulDieSizeXTol;
	LONG lDieTolY = ulDieSizeYTol;
	// build physical map
	int nMaxIterateCount = -1, nMaxAllowLeft = 10, nMaxSpan = 2;	// nMaxAllowPitch
	BOOL bPreSorted = TRUE;
	bool *pbGetOut;
	pbGetOut = new bool[WSGetPrescanTotalDie()+10];

	SaveScanTimeEvent("WPR: to clear prescan info");
	ClearPrescanInfo();
	SaveScanTimeEvent("WPR: to reset map phy posn");
	ResetMapPhyPosn();

	CDWordArray dwaPiMapWfX, dwaPiMapWfY, dwaPiMapRow, dwaPiMapCol;

	if ((nPassInErr == 0) && (fpPassIn != NULL))
	{
		fprintf(fpPassIn, "%lu,%lu,%lu,%lu,%d,%d,%d,%d,%ld,%ld,%ld,%ld\n", 
			ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY, 
			bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan, 
			GetDieSizeX(), GetDieSizeY(), GetDiePitchX_X(), GetDiePitchY_Y());
	}

	SaveScanTimeEvent("WPR: to construct normal map");
	for(LONG ulLoopRow=GetMapValidMinRow(); ulLoopRow<=GetMapValidMaxRow(); ulLoopRow++)
	{
		ULONG ulRefrCol = aRefListCol.GetAt(0) - WPR_SCAN_MAP_OFFSET;
		LONG  lRefWftX = aRefListWfX.GetAt(0);
		LONG  lRefWftY = aRefListWfY.GetAt(0);
		LONG ulFirstDie = GetMapValidMaxCol()+10;
		LONG ulLastDie  = GetMapValidMaxCol();
		LONG ul2ndStart = GetMapValidMaxCol();
		for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			if (WM_CWaferMap::Instance()->IsMapHaveBin(ulLoopRow, ulCol))
			{
				if( ulFirstDie>GetMapValidMaxCol() )
				{
					ulFirstDie = ulCol;
				}
				ulLastDie = ulCol;
			}
			if( m_WaferMapWrapper.GetDieState(ulLoopRow, ulCol)==WT_MAP_DS_BAR_2ND_START )
			{
				ul2ndStart = ulCol;
			}
		}

		if( ulFirstDie>ulLastDie )
		{
			szMsg.Format("WPR: no die in row %lu", ulLoopRow);
			if ((nOutErr == 0) && (fpOut != NULL))
			{
				fprintf(fpOut, "%s\n", szMsg);
			}
			continue;
		}	//	no any good die, skip this line/row

		for(UCHAR ucLoop=0; ucLoop<2; ucLoop++)	//	two columns of bar in same row, 0 is left and 1 is right
		{
			ClearGoodInfo();
			BOOL bFindBase = FALSE;
			//	found align or reference point
			for(ULONG ulIndex=0; ulIndex<lRefDieCount; ulIndex++)
			{
				if( aRefListRow.GetAt(ulIndex)!=(ulLoopRow + WPR_SCAN_MAP_OFFSET) )
				{
					continue;
				}
				LONG lAssistCol = aRefListCol.GetAt(ulIndex) - WPR_SCAN_MAP_OFFSET;
				if( (ucLoop==0 && lAssistCol<ul2ndStart) || (ucLoop==1 && lAssistCol>=ul2ndStart) || (ul2ndStart>=ulLastDie) )
				{
					bFindBase = TRUE;
					ulRefrCol = lAssistCol;
					lRefWftX = aRefListWfX.GetAt(ulIndex);
					lRefWftY = aRefListWfY.GetAt(ulIndex);
					break;
				}
			}

			if( bFindBase==FALSE )
			{
				szMsg.Format("WPR: no align point at right=%d in row %lu", ucLoop, ulLoopRow);
				if( fpOut!=NULL )
				{
					fprintf(fpOut, "%s\n", szMsg);
				}
				continue;
			}	//	no refer point, just skip this bar

			LONG ulStartCol = ulFirstDie;
			LONG ulStopCol  = ul2ndStart-1;
			if( ucLoop==1 )
			{
				ulStartCol = ul2ndStart;
				ulStopCol  = ulLastDie;
			}
			LONG lStartX = lRefWftX - (ulStartCol - ulRefrCol) * ulDiePitchX + GetPrescanPitchX();
			LONG lStopX  = lRefWftX - (ulStopCol  - ulRefrCol) * ulDiePitchX - GetPrescanPitchX();

			dwaPiMapWfX.RemoveAll();	dwaPiMapWfY.RemoveAll();
			dwaPiMapRow.RemoveAll();	dwaPiMapCol.RemoveAll();
			dwaPiMapWfX.Add(lRefWftX);	dwaPiMapWfY.Add(lRefWftY);
			dwaPiMapRow.Add(ulLoopRow + WPR_SCAN_MAP_OFFSET);
			dwaPiMapCol.Add(ulRefrCol + WPR_SCAN_MAP_OFFSET);
			// add all prescan die data list to physical map builder include refer points
			m_cPIMap.RemoveAll();
			m_cPIMap.SortAdd_Tail(lRefWftX, lRefWftY, 0, FALSE);

			if( fpPassIn!=NULL )
			{
				fprintf(fpPassIn, "\n%ld,%ld,%ld,%ld,0\n", lRefWftX, lRefWftY, ulLoopRow, ulRefrCol);
			}

			for(ULONG ulIndex=1; ulIndex<=WSGetPrescanTotalDie(); ulIndex++)
			{
				pbGetOut[ulIndex] = false;
				if(	WSGetPrescanPosn(ulIndex, lEncX, lEncY)==FALSE )
				{
					continue;
				}

				if( lEncX>=lStopX && lEncX<=lStartX && labs(lEncY-lRefWftY)<(ulStreetInY/2) )
				{
					m_cPIMap.SortAdd_Tail(lEncX, lEncY, ulIndex, TRUE);
					if( fpPassIn!=NULL )
					{
						fprintf(fpPassIn, "%ld,%ld,%ld\n", lEncX, lEncY, ulIndex);
					}
					SetGoodPosnOnly(lEncX, lEncY, 0, "", ulIndex, ulIndex);
				}
			}

			m_cPIMap.KeepIsolatedDice(FALSE);
			m_cPIMap.SetScan2Pr(FALSE);
			m_cPIMap.SetXOffset(0);

			m_cPIMap.ConstructMap(dwaPiMapWfX, dwaPiMapWfY, dwaPiMapRow, dwaPiMapCol,
				ulDieSizeXTol, ulDieSizeYTol, ulDiePitchX, ulDiePitchY,
				bPreSorted, nMaxIterateCount, nMaxAllowLeft, nMaxSpan);

			// get all phsical list infromation to prescan result list
			ULONG ulPyiRowMax = 0, ulPyiColMax = 0;
			m_cPIMap.GetDimension(ulPyiRowMax, ulPyiColMax);

			ULONG ulScnDieMaxRow = 0, ulScnDieMaxCol = 0, ulScnDieMinRow = 9999, ulScnDieMinCol = 9999;
			for(ULONG ulRow=0; ulRow<=ulPyiRowMax; ulRow++)
			{
				for(ULONG ulCol=0; ulCol<=ulPyiColMax; ulCol++)
				{
					BOOL bDieOverLap = FALSE;
					LONG lIndex = 0;
					BOOL bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
					if( bInfo==FALSE )
					{
						continue;
					}

					if( lIndex<=WPR_ADD_DIE_INDEX )
					{
						continue;
					}

					if( IsWithinScanLimit(lEncX, lEncY)==FALSE )
					{
						continue;
					}

					if( ulRow>ulScnDieMaxRow )
						ulScnDieMaxRow = ulRow;
					if( ulCol>ulScnDieMaxCol )
						ulScnDieMaxCol = ulCol;
					if( ulRow<ulScnDieMinRow )
						ulScnDieMinRow = ulRow;
					if( ulCol<ulScnDieMinCol )
						ulScnDieMinCol = ulCol;
				}
			}

			szMsg.Format("WPR: get out PHY range Row(%lu,%lu)  Col(%lu,%lu)",
				ulScnDieMinRow, ulScnDieMaxRow, ulScnDieMinCol, ulScnDieMaxCol);
			if( fpOut!=NULL )
			{
				fprintf(fpOut, "%s\n", szMsg);
			}

			ULONG ulFrameID = 0;
			DOUBLE dDieAngle;
			CString szDieBin = "0";
			for(ULONG ulRow=ulScnDieMinRow; ulRow<=ulScnDieMaxRow; ulRow++)
			{
				for(ULONG ulCol=ulScnDieMinCol; ulCol<=ulScnDieMaxCol; ulCol++)
				{
					LONG lIndex = 0;
					BOOL bDieOverLap = FALSE;
					BOOL bInfo = m_cPIMap.GetInfo(ulRow, ulCol, lEncX, lEncY, lIndex, bDieOverLap);
					if( bInfo==FALSE )
					{
						continue;
					}

					if( lIndex<=WPR_ADD_DIE_INDEX )
						continue;

					if( lIndex>0 )
						pbGetOut[lIndex] = true;

					LONG ulInRow = (LONG)ulRow + m_lMapScanOffsetRow;
					LONG ulInCol = (LONG)ulCol + m_lMapScanOffsetCol;

					USHORT usDieState = 0;
					if( lIndex>0 )
					{
						LONG lDummyX = 0, lDummyY = 0;
						PR_UWORD uwScanDiePrID = 0;
						WSGetScanPosition(lIndex, lDummyX, lDummyY, dDieAngle, szDieBin, usDieState, uwScanDiePrID, ulFrameID);
					}
					else
					{
						dDieAngle = 0.0;
						szDieBin = "0";
					}

					SetScanInfo(ulInRow, ulInCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);	//	from physical map builder
					if( fpOut!=NULL )
					{
						fprintf(fpOut, "%ld,%ld,%lu,%lu,%ld,%ld\n", ulInRow, ulInCol, ulLoopRow, ulCol, lEncX, lEncY);
					}
				}
			}

			WSClearScanRemainDieList();
			for(ULONG ulIndex=1; ulIndex<=GetGoodTotalDie(); ulIndex++)
			{
				LONG lWsIndex = 0;
				if( GetGoodPosnAll(ulIndex, lEncX, lEncY, dDieAngle, szDieBin, lWsIndex, lWsIndex)==FALSE )
				{
					continue;
				}

				if( lWsIndex>(LONG)WSGetPrescanTotalDie() || lWsIndex<1 )
				{
					continue;
				}
				if( pbGetOut[lWsIndex]==true )
				{
					continue;
				}

				if( labs(lRefWftX-lEncX)<lDieTolX && labs(lRefWftY-lEncY)<lDieTolY )
				{
					pbGetOut[lWsIndex] = true;
					continue;
				}

				for(ULONG j=1; j<=GetGoodTotalDie(); j++)
				{
					LONG lEncX1, lEncY1, lWsIndex1;
					if( GetGoodPosnAll(j, lEncX1, lEncY1, dDieAngle, szDieBin, lWsIndex1, lWsIndex1)==FALSE )
					{
						continue;
					}

					if( j<ulIndex )
					{
						if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
						{
							pbGetOut[lWsIndex] = true;
							break;
						}
					}
					else if( j>ulIndex )
					{
						if( labs(lEncX1-lEncX)<lDieTolX && labs(lEncY1-lEncY)<lDieTolY )
						{
							if( pbGetOut[lWsIndex1]==true )
							{
								pbGetOut[lWsIndex] = true;
								break;
							}
						}
					}
				}
				if( pbGetOut[lWsIndex]==true )
				{
					continue;
				}
				if( fpOut!=NULL )
				{
					fprintf(fpOut, "index %lu to remain list,%ld,%ld\n", ulIndex, lEncX, lEncY);
				}
				pbGetOut[lWsIndex] = true;
				WSAddScanRemainDieIntoList(lEncX, lEncY, lWsIndex);
			}

			m_cPIMap.RemoveAll();
			m_cPIMap.SetScan2Pr(FALSE);

			// transfer data from prescan result to map physical position array
			nMaxSpan = 5;
			for(LONG lLoopCol=ulStartCol; lLoopCol<=ulStopCol; lLoopCol++)
			{
				UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulLoopRow, lLoopCol);
				if( ucB4ScanGrade==ucNullBin )
				{
					continue;
				}

				BOOL bHasDie = GetPrescanPosition(ulLoopRow, lLoopCol, lEncX, lEncY);
				if( bHasDie==FALSE )
				{
					if( GetDieValidInX(ulLoopRow, lLoopCol, nMaxSpan, lEncX, lEncY, FALSE) )
					{
						PR_UWORD uwDiePrID = 0;
						FindAndFillFromRemainList(ulLoopRow, lLoopCol, lDieTolX, lDieTolY, lEncX, lEncY, uwDiePrID);
					}
				}
			}

			for(LONG lLoopCol=ulStopCol; lLoopCol>=ulStartCol; lLoopCol--)
			{
				if( lLoopCol<0 )
				{
					break;
				}
				UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulLoopRow, lLoopCol);
				if( ucB4ScanGrade==ucNullBin )
				{
					continue;
				}

				BOOL bHasDie = GetPrescanPosition(ulLoopRow, lLoopCol, lEncX, lEncY);
				if( bHasDie==FALSE )
				{
					if( GetDieValidInX(ulLoopRow, lLoopCol, 0-nMaxSpan, lEncX, lEncY, FALSE) )
					{
						PR_UWORD uwDiePrID = 0;
						FindAndFillFromRemainList(ulLoopRow, lLoopCol, lDieTolX, lDieTolY, lEncX, lEncY, uwDiePrID);
					}
				}
			}

			if( ul2ndStart>=ulLastDie )	//	only one bar in this line
			{
				break;
			}
		}	//	one line, left and right bar
	}	// line by line from top to bottom
	delete [] pbGetOut;

	DOUBLE dDieAngle = 0.0;
	CString szDieBin = "";
	SaveScanTimeEvent("WPR: get out complete and to update wafer map");
	DelPrescanRunPosn();
	SaveScanTimeEvent("WPR: loop to fill in map position.");
	ULONG ulEmptyIdx = 0, ulDefectIdx = 0, ulBadCutIdx = 0, ulGoodIdx = 0,ulMapTotal = 0 ;
	for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
	{
		for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			USHORT usDieState = 0;
			BOOL bIsDefect = FALSE, bIsBadCut = FALSE, bIsFakeEmpty = FALSE;
			BOOL bHasDie = GetScanInfo(ulRow, ulCol, lEncX, lEncY, dDieAngle, szDieBin, usDieState);
			DecodeDieState(usDieState, bIsDefect, bIsBadCut, bIsFakeEmpty);
			if( bHasDie )
			{
				m_WaferMapWrapper.BatchSetPhysicalPosition(ulRow, ulCol, lEncX, lEncY);
				SetPrescanRunPosn(ulRow, ulCol, lEncX, lEncY);
			}

			UCHAR ucB4ScanGrade = m_WaferMapWrapper.GetGrade(ulRow, ulCol);
			BOOL bNeedChangeGrade = FALSE;
			UCHAR ucChangeGrade = ucB4ScanGrade;

			if( ucB4ScanGrade==ucNullBin )
			{
				continue;
			}

			ulMapTotal++;

			if( bHasDie )
			{
				BOOL bNeedChangeGrade = FALSE;
				UCHAR ucChangeGrade = ucB4ScanGrade;

				if (bIsBadCut)
				{
					SetBadCutFull(ulBadCutIdx, ulRow, ulCol, ucB4ScanGrade);
					ulBadCutIdx++;

					if( IsNoPickBadCut() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT);
					}

					if( bBadCutAction )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucBadCutGrade;
					}
					else
					{
						if( m_bPrescanBadCutMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else if( bIsFakeEmpty )
				{
					SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
					ulDefectIdx++;

					m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DS_SCAN_BACKUP_ALIGN);
				}
				else if( bIsDefect )
				{
					SetDefectFull(ulDefectIdx, ulRow, ulCol, ucB4ScanGrade);
					ulDefectIdx++;

					if( IsNoPickDefect() )
					{
						m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT);
					}

					if( bDefectAction )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucDefectGrade;
					}
					else
					{
						if( m_bPrescanDefectMarkUnPick )
						{
							m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						}
					}
				}
				else
				{
					ulGoodIdx++;
					if( bGoodAction )
					{
						bNeedChangeGrade = TRUE;
						ucChangeGrade = ucGoodGrade;
					}
				}

				if( bNeedChangeGrade )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucChangeGrade);
				}
			}
			else // set to empty grade
			{
				lEncX = lEncY = 0;
				SetEmptyFull(ulEmptyIdx, ulRow, ulCol, ucB4ScanGrade);
				ulEmptyIdx++;
				m_WaferMapWrapper.SetDieState(ulRow, ulCol, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);

				if( bEmptyAction )
				{
					m_WaferMapWrapper.ChangeGrade(ulRow, ulCol, ucEmptyGrade);
				}
				else
				{
					if( m_bPrescanEmptyMarkUnPick )
					{
						m_WaferMapWrapper.MarkDie(ulRow, ulCol, TRUE);
						UpdateDie(ulRow, ulCol, ucB4ScanGrade, WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY);
					}
				}
			}
		}
	}
	m_WaferMapWrapper.BatchPhysicalPositionUpdate();

	SaveScanTimeEvent("WPR: construct map complete");

	if( fpPassIn!=NULL )
	{
		fclose(fpPassIn);
	}
	if( fpOut!=NULL )
	{
		fclose(fpOut);
	}

	SaveScanTimeEvent("WPR: get out complete and to update wafer map done");
	m_lTimeSlot[7] = (LONG)(GetTime()-dUsedTime);	//	07.	constuct map used time.
	m_lTimeSlot[8] = (LONG)(GetTime()-dUsedTime);	//	08.	update wafer map grade and position

	BOOL bFailBreak = FALSE;
	for(LONG ulRow=GetMapValidMinRow(); ulRow<=GetMapValidMaxRow(); ulRow++)
	{
		for(LONG ulCol=GetMapValidMinCol(); ulCol<=GetMapValidMaxCol(); ulCol++)
		{
			BOOL bIsIgnore = FALSE;

			LONG lEncX1, lEncY1, lEncX2, lEncY2;
			if( GetPrescanPosition(ulRow, ulCol, lEncX1, lEncY1) )
			{
				if( GetPrescanPosition(ulRow, ulCol+1, lEncX2, lEncY2) )
				{
					if( labs(lEncX1-lEncX2)<lDieTolX && 
						labs(lEncY1-lEncY2)<lDieTolY )
					{
						LONG lUserRow1, lUserCol1, lUserRow2, lUserCol2;
						ConvertAsmToOrgUser(ulRow, ulCol, lUserRow1, lUserCol1);
						ConvertAsmToOrgUser(ulRow, ulCol+1, lUserRow2, lUserCol2);
						CString szMsg;
						szMsg.Format("Map die (%d,%d) = (%d,%d).\nPosition overlap, please stop!", lUserRow1, lUserCol1, lUserRow2, lUserCol2);
						HmiMessage_Red_Back(szMsg, "Hisense");
						m_lPrescanVerifyResult += 16384;
						SetErrorMessage(szMsg);
						bFailBreak = TRUE;
						break;
					}
				}
			}
			if( bFailBreak )
			{
				break;
			}
		}
		if( bFailBreak )
		{
			break;
		}
	}

	lHomeNewRow += m_lMapScanOffsetRow;
	lHomeNewCol += m_lMapScanOffsetCol;
	pUtl->SetAlignPosition(lHomeNewRow, lHomeNewCol, lHomeWfX, lHomeWfY);

	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulGoodIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT]	= ulDefectIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT]	= ulBadCutIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY]	= ulEmptyIdx;
	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA]	= 0;

	SetMapTotalDie(ulMapTotal);

	SaveScanTimeEvent("WPR: CLEAR BUILDing materials");
	// build prescan map complete
	ClearGoodInfo();	ClearWSPrescanInfo();	WSClearScanRemainDieList();
	DelPrescanFrameInfo();

	aRefListWfX.RemoveAll();
	aRefListWfY.RemoveAll();
	aRefListRow.RemoveAll();
	aRefListCol.RemoveAll();
}	//	EMCORE bar wafer
#endif

BOOL CWaferPr::IsNoPickDefect()
{
	return m_bToPickDefectDie==FALSE && m_bNgPickDefect==FALSE;
}

BOOL CWaferPr::IsNoPickBadCut()
{
	return m_bToPickDefectDie==FALSE && m_bNgPickBadCut==FALSE;
}

BOOL CWaferPr::IsNoPickExtra()
{
	return m_bToPickDefectDie==FALSE && m_bNgPickExtra==FALSE;
}

