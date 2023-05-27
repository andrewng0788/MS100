#include "stdafx.h"
#include "MS896A_Constant.h"
#include "BondPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	CP_DC_FOLDER	gszUSER_DIRECTORY + "\\OutputFile\\ClearBin"

////////////////////////////////////////////
//	Prober Daily Check Relative on the display result with SPC
////////////////////////////////////////////
LONG CBondPr::LoadSourceAndProbeData(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = FALSE;
	// load file to get the criteria

	m_unCheckItemList.RemoveAll();
	m_szItemList.RemoveAll();

	CStringArray szSpecList;	// average
	CStringArray szEvyLoList, szEvyHiList;
	CStringArray szAvgLoList, szAvgHiList;
	CStringArray szSrcLoList, szSrcHiList;
	szSpecList.RemoveAll();
	szEvyLoList.RemoveAll();
	szEvyHiList.RemoveAll();
	szAvgLoList.RemoveAll();
	szAvgHiList.RemoveAll();
	szSrcLoList.RemoveAll();
	szSrcHiList.RemoveAll();

	CStringArray szGdnData, szPrbData;
	szGdnData.RemoveAll();
	szPrbData.RemoveAll();

	if( access(gszPrbDCStdFilename, 0)==-1 )
	{
		HmiMessage_Red_Back("Daily Check Standard file not loaded.", "Prober");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}
	if(	access(m_szSourceMapFullPath, 0)==-1 )
	{
		HmiMessage_Red_Back("Daily Check source map file (golden sample) not found.", "Prober");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}
	if( access(m_szOutputMapFullPath, 0)==-1 )
	{
		HmiMessage_Red_Back("Daily Check probed map file (CP100 map) not found.", "Prober");
		svMsg.InitMessage(sizeof(BOOL),	&bReturn);
		return 1;
	}

	CStdioFile fFile;
	// read DC std file
	if( fFile.Open(gszPrbDCStdFilename, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szReading;
		fFile.SeekToBegin();
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, m_szItemList);
		}	//	Item,BIN,CONTA,
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, szSpecList);
		}	//	blue-baiguang(mw),,
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, szEvyLoList);
		}	//	EveryDieL,,
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, szEvyHiList);
		}	//	EveryDieH,,
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, szAvgLoList);
		}	//	AvgLow,,
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, szAvgHiList);
		}	//	AvgHigh,,,
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, szSrcLoList);
		}	//	SrcDataLow,,
		if( fFile.ReadString(szReading) )
		{
			ParseRawData(szReading, szSrcHiList);
		}	//	SrcDataHigh,,
	}
	fFile.Close();

	for(int i=2; i<(m_szItemList.GetSize()-2); i++)
	{
		if( i>= szSpecList.GetSize() )
			break;
		if( i>=szEvyLoList.GetSize() )
			break;
		if( i>=szEvyHiList.GetSize() )
			break;
		if( i>=szAvgLoList.GetSize() )
			break;
		if( i>=szAvgHiList.GetSize() )
			break;
		if( i>=szSrcLoList.GetSize() )
			break;
		if( i>=szSrcHiList.GetSize() )
			break;

		if( szSpecList.GetAt(i).IsEmpty() )
		{
			continue;
		}

		if( (szEvyLoList.GetAt(i).IsEmpty() || szEvyHiList.GetAt(i).IsEmpty()) && 
			(szAvgLoList.GetAt(i).IsEmpty() || szAvgHiList.GetAt(i).IsEmpty()) && 
			(szSrcLoList.GetAt(i).IsEmpty() || szSrcHiList.GetAt(i).IsEmpty()) )
		{
			continue;
		}
		m_unCheckItemList.Add(i);
		CMSLogFileUtility::Instance()->ProberGenMapFileLog(m_szItemList.GetAt(i) + " is valid");
	}

	if( m_unCheckItemList.GetSize()==0 )
	{
		HmiMessage_Red_Back("No spec data in Daily Check Std file.", "Prober");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	// read golden sample std result file
	if( fFile.Open(m_szSourceMapFullPath, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szReading;
		fFile.SeekToBegin();
		while( fFile.ReadString(szReading) )
		{
			szReading = szReading.MakeUpper();
			if( szReading.Find("TEST,BIN,")!=-1 )
			{
				break;
			}
		}
		while( fFile.ReadString(szReading) )
		{
			CMSLogFileUtility::Instance()->ProberGenMapFileLog(szReading);
			if( szReading.GetLength()< 5 )
				continue;
			szGdnData.Add(szReading);
		}
	}
	fFile.Close();

	// read our probing result data file
	if( fFile.Open(m_szOutputMapFullPath, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szReading;
		fFile.SeekToBegin();
		while( fFile.ReadString(szReading) )
		{
			szReading = szReading.MakeUpper();
			if( szReading.Find("TEST,BIN,")!=-1 )
			{
				break;
			}
		}
		while( fFile.ReadString(szReading) )
		{
			CMSLogFileUtility::Instance()->ProberGenMapFileLog(szReading);
			if( szReading.GetLength()<5 )
				continue;
			szPrbData.Add(szReading);
		}
	}
	fFile.Close();

	m_lPrbTotalDieCounter = szPrbData.GetSize();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->SearchAndRemoveFiles(CP_DC_FOLDER);	
	FILE *fp;

	CString szErrList = "Fail,", szMinList = "Min,", szAvgList = "Avg,", szMaxList = "Max,";
	CString szTitleList = "Item,";
	BOOL bDCFail = FALSE;
	// generate all items corresponding summary file.
	for(int i=0; i<m_unCheckItemList.GetSize(); i++)
	{
		CString szItemValue;
		DOUBLE dMinValue = 0.0, dMaxValue = 0.0, dAvgValue = 0.0;
		LONG lFailCount = 0, lTotalCount = 0;
		LONG lItmIndex = m_unCheckItemList.GetAt(i);
		CString szCurrItemName = m_szItemList.GetAt(lItmIndex);
		CString szItemFilename = CP_DC_FOLDER + "\\" + szCurrItemName + ".txt";

		BOOL bLOP = FALSE;
		CString szTitle = szCurrItemName.MakeUpper();
		if( szTitle.Find("LOP")!=-1 )
			bLOP = TRUE;
		fp = fopen(szItemFilename, "w");
		DOUBLE	dEvyLo = 0 - atof(szEvyLoList.GetAt(lItmIndex));
		DOUBLE	dEvyHi = atof(szEvyHiList.GetAt(lItmIndex));
		DOUBLE	dAvgLo = 0 - atof(szAvgLoList.GetAt(lItmIndex));
		DOUBLE	dAvgHi = atof(szAvgHiList.GetAt(lItmIndex));
		DOUBLE	dSrcLo = atof(szSrcLoList.GetAt(lItmIndex));
		DOUBLE	dSrcHi = atof(szSrcHiList.GetAt(lItmIndex));
		if( szEvyLoList.GetAt(lItmIndex).IsEmpty() )
		{
			if( szAvgLoList.GetAt(lItmIndex).IsEmpty() )
			{
				dEvyLo = dSrcLo;
			}
			else
			{
				dEvyLo = dAvgLo;
			}
		}
		if( szEvyHiList.GetAt(lItmIndex).IsEmpty() )
		{
			if( szAvgHiList.GetAt(lItmIndex).IsEmpty() )
			{
				dEvyHi = dSrcHi;
			}
			else
			{
				dEvyHi = dAvgHi;
			}
		}
		DOUBLE	dEvyAv = (dEvyHi + dEvyLo)/2; 
		if( fp!=NULL )
		{
			fprintf(fp, "%.4f\n",	dEvyLo);
			fprintf(fp, "%.4f\n",	dEvyHi);
			fprintf(fp, "%.4f\n",	dAvgLo);
			fprintf(fp, "%.4f\n",	dAvgHi);
			fprintf(fp, "%.4f\n",	dEvyAv);

			for(int j=0; j<szPrbData.GetSize(); j++)
			{
				CStringArray szPrbLnList;
				szPrbLnList.RemoveAll();
				ParseRawData(szPrbData.GetAt(j), szPrbLnList);
				if( szPrbLnList.GetSize()<=lItmIndex )
				{
					continue;
				}
				szItemValue = szPrbLnList.GetAt(lItmIndex);
				if( szItemValue.IsEmpty() )
				{
					fprintf(fp, ",,\n");
					continue;
				}
				DOUBLE dPrbValue = atof(szItemValue);

				BOOL bSrcLoHiOK = TRUE;
				for(int k=0; k<m_unCheckItemList.GetSize(); k++)
				{
					LONG lSrcIndex = m_unCheckItemList.GetAt(k);
					if( lSrcIndex>=szPrbLnList.GetSize() )
					{
						continue;
					}
					if( szSrcLoList.GetAt(lSrcIndex).IsEmpty() )
					{
						continue;
					}
					if( szSrcHiList.GetAt(lSrcIndex).IsEmpty() )
					{
						continue;
					}
					if( szPrbLnList.GetAt(lSrcIndex).IsEmpty() )
					{
						continue;
					}

					DOUBLE dSrcDataLo = atof(szSrcLoList.GetAt(lSrcIndex));
					DOUBLE dSrcDataHi = atof(szSrcHiList.GetAt(lSrcIndex));
					DOUBLE dPrbDataLn = atof(szPrbLnList.GetAt(lSrcIndex));
					if( (dPrbDataLn<dSrcDataLo) || (dPrbDataLn>dSrcDataHi) )
					{
						bSrcLoHiOK = FALSE;
						break;
					}
				}
				if( bSrcLoHiOK==FALSE )
				{
					lFailCount++;
					fprintf(fp, ",,\n");
					continue;
				}

				CStringArray szGdnLineList;
				szGdnLineList.RemoveAll();
				if( GetPSCMode()==PSC_REGN )
				{
					INT nPrbIndex = szPrbLnList.GetSize()-1;
					CString szPrbCol = szPrbLnList.GetAt(nPrbIndex-1);
					CString szPrbRow = szPrbLnList.GetAt(nPrbIndex);
					for(int m=0; m<szGdnData.GetSize(); m++)
					{
						szGdnLineList.RemoveAll();
						ParseRawData(szGdnData.GetAt(m), szGdnLineList);
						INT nSrcIndex = szGdnLineList.GetSize()-1;
						CString szSrcCol = szGdnLineList.GetAt(nSrcIndex-1);
						CString szSrcRow = szGdnLineList.GetAt(nSrcIndex);
						if( szSrcCol==szPrbCol && szSrcRow==szPrbRow )
						{
							break;
						}
					}
				}
				else
				{
					ParseRawData(szGdnData.GetAt(j), szGdnLineList);
				}

				if( szGdnLineList.GetSize()<=lItmIndex )
				{
					continue;
				}

				szItemValue = szGdnLineList.GetAt(lItmIndex);
				if( szItemValue.IsEmpty() )
				{
					continue;
				}

				DOUBLE dGdnValue = atof(szItemValue);
				DOUBLE dChartValue = dPrbValue - dGdnValue;

				if( bLOP )
				{
					if( dPrbValue!=0.0 )
						dChartValue = dChartValue*100/dPrbValue;
					else
						dChartValue = 0.0;
				}
				fprintf(fp, "%f,%f,%f\n", dChartValue, dPrbValue, dGdnValue);

				if( j==0 )
				{
					dMinValue = dChartValue;
					dMaxValue = dChartValue;
				}
				if( dMinValue>dChartValue )
					dMinValue = dChartValue;
				if( dMaxValue<dChartValue )
					dMaxValue = dChartValue;
				dAvgValue += dChartValue;
				lTotalCount++;
			}
			fclose(fp);
		}

		if( lTotalCount>0 )
			dAvgValue = dAvgValue/lTotalCount;
		if( lFailCount>0 )
			bDCFail = TRUE;

		szItemValue.Format("%d,", lFailCount);
		szErrList += szItemValue;
		szItemValue.Format("%f,", dMinValue);
		szMinList += szItemValue;
		szItemValue.Format("%f,", dAvgValue);
		szAvgList += szItemValue;
		szItemValue.Format("%f,", dMaxValue);
		szMaxList += szItemValue;
		szTitleList = szTitleList + szCurrItemName + ",";
	}

	fp = fopen(gszROOT_DIRECTORY + "\\Exe\\PSCTitle.csv", "w");
	if ( fp!=NULL )
	{
		fprintf(fp, "%s\n", (LPCTSTR) szTitleList);
		fclose(fp);
	}

	fp = fopen(gszROOT_DIRECTORY + "\\Exe\\PSCSummary.csv", "w");
	if ( fp!=NULL )
	{
		fprintf(fp, "%s\n", (LPCTSTR) szErrList);
		fprintf(fp, "%s\n", (LPCTSTR) szMinList);
		fprintf(fp, "%s\n", (LPCTSTR) szAvgList);
		fprintf(fp, "%s\n", (LPCTSTR) szMaxList);
		fclose(fp);
	}

	if( bDCFail )
	{
		HmiMessage_Red_Back("Daily Check probing fail!", "Prober");
	}

	m_lPSCDataColumns = m_unCheckItemList.GetSize() + 1;
	m_ulCheckItemIndex = 0;
	DisplayPSCResult();	// into page

	bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL),	&bReturn);
	return 1;
}

LONG CBondPr::PrevSPCDataDisplay(IPC_CServiceMessage& svMsg)
{
	if( m_ulCheckItemIndex<=0 )
		return 1;

	m_ulCheckItemIndex--;

	DisplayPSCResult();	// previous pressed

	return 1;
}

LONG CBondPr::NextSPCDataDisplay(IPC_CServiceMessage& svMsg)
{
	if( (m_ulCheckItemIndex+1)>=(ULONG)m_unCheckItemList.GetSize() )
		return 1;

	m_ulCheckItemIndex++;

	DisplayPSCResult();	// next pressed

	return 1;
}

LONG CBondPr::SavePSCDisplayData(IPC_CServiceMessage& svMsg)		
{
	BOOL	bReturn = TRUE;

	bReturn = ExportPSCChartData();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBondPr::LogPSCData(DOUBLE fDieRotate)
{
	double	dUnitX = 0;
	double	dUnitY = 0;
	//RunTime result

	m_lTotalPSCData++;
	m_dTotalPSCValue += (double)fDieRotate;
	m_dAvgPSCValue = m_dTotalPSCValue / ((DOUBLE)(m_lTotalPSCData) + 1);

	*m_pAvgDieAngle				= m_dAvgPSCValue;
	*m_pDspDieX					= dUnitX;
	*m_pDspDieY					= dUnitY;
	*m_pDspDieTheta				= (double)(fDieRotate);
	m_pDspBin->Log();

	return TRUE;
}

BOOL CBondPr::ExportPSCChartData()
{
	SPC_CProperty::SPC_EPropertyType	eType5;

	CString	szFilePath;

	FILE	*fp;

	CreateDirectory(CP_DC_FOLDER, NULL);
	szFilePath = CP_DC_FOLDER + "\\" + m_szCurrItemName + ".txt";

	if ( (fp = fopen((LPCTSTR) szFilePath, "w+")) == NULL )
		return FALSE;


	//Ready get data from SPC
	m_pDspBin->Flush();
	m_pDspBin->BeginRetrieveFromStart(0, 1);

	ULONG	ulIndex = 0;	
	BOOL	bEndOfRecord = FALSE;	
	void	*pDieAngle;
	pDieAngle		= m_pDspBin->GetVariableInFileProperty("Die Rotation", eType5);

	fprintf(fp, "DieNo,Value\n");
	while (!bEndOfRecord)
	{
		SPC_CPart::SPC_ERetrievalResult		eResult;
		unsigned long ulTempDataID, ulParentID, ulOwnID;
		CTime	TimeStamp; 
		eResult = m_pDspBin->RetrieveAdvance(ulTempDataID, TimeStamp, ulParentID, ulOwnID);
		bEndOfRecord = (eResult == SPC_CPart::SPC_RETRIEVE_END);

		if (!bEndOfRecord)
		{
			//Get raw data....	
			ulIndex++;
			DOUBLE dSPCDieAngle	= SPC_CProperty::ConvertToDouble(pDieAngle, eType5);
			
			//ouput each data
			fprintf(fp, "%lu,%.2f\n", ulIndex, dSPCDieAngle);
		}
	}

	m_pDspBin->EndRetrieve();
	fclose(fp);

	return TRUE;
}

LONG CBondPr::DisplayPSCResult()
{
	LONG lCheckIndex = m_ulCheckItemIndex;
	LONG lCurrIndex = m_unCheckItemList.GetAt(lCheckIndex);
	LONG lPrevIndex = lCurrIndex;
	LONG lNextIndex = lCurrIndex;
	if( lCheckIndex>0 )
		lPrevIndex = m_unCheckItemList.GetAt(lCheckIndex-1);
	if( (lCheckIndex+1)<m_unCheckItemList.GetSize() )
		lNextIndex = m_unCheckItemList.GetAt(lCheckIndex+1);
	m_szPrevItemName = m_szItemList.GetAt(lPrevIndex);
	m_szCurrItemName = m_szItemList.GetAt(lCurrIndex);
	m_szNextItemName = m_szItemList.GetAt(lNextIndex);

	// delete current DC display data firstly.
	m_pDspBin->Delete(100);
	
	CString szItemFilename = CP_DC_FOLDER + "\\" + m_szCurrItemName + ".txt";
	CStdioFile fFile;
	if( fFile.Open(szItemFilename, CFile::modeRead|CFile::shareDenyNone)==FALSE )
	{
		fFile.Close();
		return 0;
	}

	CString szItemValue;

	m_szPSCChartTitle = "Daily Check Display: " + m_szCurrItemName;
	m_szPSCDisplayCmd = "Title=" + m_szPSCChartTitle + ";";
	fFile.ReadString(szItemValue);
	m_szPSCDisplayCmd += "YMinimum=" + szItemValue + ";";
	fFile.ReadString(szItemValue);
	m_szPSCDisplayCmd += "YMaximum=" + szItemValue + ";";
	fFile.ReadString(szItemValue);
	fFile.ReadString(szItemValue);
	fFile.ReadString(szItemValue);
	m_szPSCDisplayCmd += "YAverage=" + szItemValue + ";";

	SetAlarmLog(m_szPSCDisplayCmd);

	m_lTotalPSCData = 0;
	m_dTotalPSCValue = 0.0;
	m_dAvgPSCValue	= 0.0;

	while( fFile.ReadString(szItemValue) )
	{
		CStringArray szDataList;
		szDataList.RemoveAll();
		ParseRawData(szItemValue, szDataList);
		szItemValue = szDataList.GetAt(0);
		if( szItemValue.IsEmpty() )
			continue;
		DOUBLE dChartValue = atof(szItemValue);
		LogPSCData(dChartValue);
	}

	return 1;
}

LONG CBondPr::SpinPSCDataDisplay(IPC_CServiceMessage& svMsg)
{
	LONG lScanYieldIndex = 0;
	svMsg.GetMsg(sizeof(LONG), &lScanYieldIndex);

	m_lPSCDataSpinIndex = lScanYieldIndex;
	UpdatePSCDataTable();	// spin select a item

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;	
}

LONG CBondPr::IntoPSCDataDisplay(IPC_CServiceMessage& svMsg)
{
	m_lPSCDataSpinIndex	= 0;
	UpdatePSCDataTable();	// into data page


	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);

	return 1;	
}

BOOL CBondPr::UpdatePSCDataTable()
{
	LONG lTotalLines = m_lPrbTotalDieCounter;
	LONG lSpinIndex = m_lPSCDataSpinIndex;
	if( lSpinIndex>=lTotalLines && lTotalLines>0 )
	{
		lSpinIndex -= 10;
	}
	if( lTotalLines==0 )
	{
		lSpinIndex = 0;
	}
	m_lPSCDataSpinIndex = lSpinIndex;

	CStdioFile fFile;
	FILE *fp;
	fp = fopen(gszROOT_DIRECTORY + "\\Exe\\PSCData.csv", "w");
	if ( fp!=NULL )
	{
		CString szItemValue;
		for(LONG j=lSpinIndex; j<lSpinIndex+10; j++)
		{
			szItemValue.Format("%d", j+1);
			if( j>=lTotalLines )
			{
				szItemValue = "";
			}
			fprintf(fp, "%s,", (LPCTSTR) szItemValue);		//v4.40	//Klocwork
		}
		fprintf(fp, "\n");
		for(int i=0; i<m_unCheckItemList.GetSize(); i++)
		{
			LONG lCurrIndex = m_unCheckItemList.GetAt(i);
			if( lCurrIndex>=m_szItemList.GetSize() )
			{
				continue;
			}
			CString szCurrItemName = m_szItemList.GetAt(lCurrIndex);
			CString szItemFilename = CP_DC_FOLDER + "\\" + szCurrItemName + ".txt";
			if( fFile.Open(szItemFilename, CFile::modeRead|CFile::shareDenyNone)==FALSE )
			{
				fFile.Close();
				continue;
			}

			CString szLineData;
			fFile.ReadString(szLineData);
			fFile.ReadString(szLineData);
			fFile.ReadString(szLineData);
			fFile.ReadString(szLineData);

			LONG j= 0;
			while( fFile.ReadString(szLineData) )
			{
				if( j>=lSpinIndex )
				{
					break;
				}
				j++;
			}

			for(j=lSpinIndex; j<lSpinIndex+10; j++)
			{
				if( fFile.ReadString(szLineData) )
				{
					CStringArray szDataList;
					szDataList.RemoveAll();
					ParseRawData(szLineData, szDataList);
					szItemValue = szDataList.GetAt(0);
				}
				else
				{
					szItemValue = "";
				}
				fprintf(fp, "%s,", (LPCTSTR) szItemValue);		//v4.40	//Klocwork
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}

	return TRUE;
}

