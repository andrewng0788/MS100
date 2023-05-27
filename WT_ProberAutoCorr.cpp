/////////////////////////////////////////////////////////////////
// WT_ProberAutoCorr.cpp : Auto Corrleation function of ChipProber
//
//	Description:
//		CP100 Chip Prober
//
//	Date:		Friday, December 02, 2014
//	Revision:	4.48
//
//	By:			Chris Leung
//				ATHK Product Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2014.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MarkConstant.h" 
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferTable.h"
#include "WT_Log.h"
#include "math.h"
#include "PrescanInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef PROBER
#include "AutoCorrelation.h"

#define	CP_DC_RECORD_FILE		"c:\\Mapsorter\\UserData\\OutputFile\\DailyCheckRecord.csv"

VOID CWaferTable::InitAutoCorrVariable()
{
	// Check this boolean if some function need to generate auto corr file first
	m_bACMainIsSetup = FALSE;

	// The input and output path
	m_szAutoCorrStdFile		= "";
	m_szAutoCorrTestFile	= "";
	m_szAutoCorrOutputPath	= "";
	/* Simple setting */
	m_bEnableSimpleAutoCorr	= FALSE;
	m_dMaxIRInput		= 0.0;
	m_dMinLOP1			= 0.0;
	m_dACUpperBoundary	= 0.0;
	m_dACLowerBoundary	= 0.0;
	m_lXAxisSelection	= 0;
	m_szPWRPath = "PWR.csv";	// a/b	LOP1
	m_lTesterEditorLeft	= 1600;
	m_lTesterStudioLeft	= 1600;
	/* Simple setting */

	SetUpTheAutoCorrMain();
}

/////////////////////////////////////////////////////////////////
//	IPC Commend for setting the input path and output path
/////////////////////////////////////////////////////////////////

LONG CWaferTable::AutoCorrSetOutputFileDir(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	pApp->GetPath(m_szAutoCorrOutputPath);

	if ( m_szAutoCorrOutputPath != "" )
	{
		SaveCP100IndependentData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::AutoCorrSetStdFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	m_szAutoCorrStdFile	= GetPathByFileDialog("","*","*");		// ("C:\\","*","*") for .csv files
	if ( m_szAutoCorrStdFile != "" )
	{
		SaveCP100IndependentData();
	}
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::AutoCorrSetTestFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	m_szAutoCorrTestFile = GetPathByFileDialog("","*","*");

	if ( m_szAutoCorrTestFile != "" )
	{
		SaveCP100IndependentData();
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::TesterClear(IPC_CServiceMessage &svMsg)
{
	CString szResult = "";

	BOOL bReturn = TRUE;
	CString szContent = "Are You Sure To Clear Tester Corelation Data?";
	CString szTitle   = "Clear Data";

	m_szCorInVF1.MakeUpper();
	m_szCorInVF2.MakeUpper();
	m_szCorInWLP.MakeUpper();
	m_szCorInWLD.MakeUpper();
	m_szCorInHW1.MakeUpper();
	m_szCorInVZ1.MakeUpper();
	m_szCorInLOP.MakeUpper();
	m_szCorInVZ2.MakeUpper();

	if (HmiMessage(szContent, szTitle, glHMI_MBX_YESNO, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 250, NULL, NULL, NULL, NULL) == glHMI_YES)
	{
		TesterCORSET("1",	m_szCorInVF1,	"D1",	"1",	"0",	"1",	"0",	szResult);
		TesterCORSET("1",	m_szCorInVF2,	"D1",	"1",	"0",	"1",	"0",	szResult);
		TesterCORSET("1",	m_szCorInWLD,	"D1",	"1",	"0",	"1",	"0",	szResult);
		TesterCORSET("1",	m_szCorInWLP,	"D1",	"1",	"0",	"1",	"0",	szResult);
		TesterCORSET("1",	m_szCorInVZ1,	"D1",	"1",	"0",	"1",	"0",	szResult);
		TesterCORSET("1",	m_szCorInLOP,	"D1",	"1",	"0",	"1",	"0",	szResult);
		TesterCORSET("1",	m_szCorInHW1,	"D1",	"1",	"0",	"1",	"0",	szResult);
		TesterCORSET("1",	m_szCorInVZ2,	"D1",	"1",	"0",	"1",	"0",	szResult);
	}

	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format("%Y%m%d%H%M%S");

	CStdioFile cfNew;
	if (cfNew.Open(CP_DC_RECORD_FILE, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE)
	{
		HmiMessage("Logging Fails");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	m_dDiff_VF1 = 0; m_dDiff_VF2 = 0; m_dDiff_WLD = 0; m_dDiff_WLP = 0;
	m_dDiff_LOP = 0; m_dDiff_HW1 = 0; m_dDiff_VZ1 = 0; m_dDiff_VZ2 = 0;

	cfNew.SeekToEnd();
	CString szTemp;
	szTemp.Format("%s,[Clear],0(%s),0(%s),0(%s),0(%s),0(%s),0(%s),0(%s),0(%s)\n",
		szTime, m_szCorInVF1, m_szCorInVF2, m_szCorInWLD, m_szCorInWLP, m_szCorInVZ1, m_szCorInHW1, m_szCorInVZ2, m_szCorInLOP);
	cfNew.WriteString(szTemp);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::CalculateDailyCheck(IPC_CServiceMessage &svMsg)
{
	m_szCorInVF1.MakeUpper();
	m_szCorInVF2.MakeUpper();
	m_szCorInWLP.MakeUpper();
	m_szCorInWLD.MakeUpper();
	m_szCorInHW1.MakeUpper();
	m_szCorInVZ1.MakeUpper();
	m_szCorInLOP.MakeUpper();
	m_szCorInVZ2.MakeUpper();
	SaveData();

	BOOL bReturn = DailyCheck(m_szAutoCorrStdFile, m_szAutoCorrTestFile);
	if ( bReturn == TRUE )
		HmiMessage("Daily Check Completes!", "Auto Correlation");
	else
		HmiMessage("Daily Check Fails!",		"Auto Correlation");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::SetDailyCheck(IPC_CServiceMessage &svMsg)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bReturn = TRUE;
	CTime theTime = CTime::GetCurrentTime();
	CString szTime = theTime.Format("%Y%m%d%H%M%S");
	CStdioFile cfNew;

	m_szCorInVF1.MakeUpper();
	m_szCorInVF2.MakeUpper();
	m_szCorInWLP.MakeUpper();
	m_szCorInWLD.MakeUpper();
	m_szCorInHW1.MakeUpper();
	m_szCorInLOP.MakeUpper();
	m_szCorInVZ1.MakeUpper();
	m_szCorInVZ2.MakeUpper();

	if (cfNew.Open(CP_DC_RECORD_FILE, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText)==FALSE)
	{
		HmiMessage("Logging Fails");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}
	cfNew.SeekToEnd();
	CString szWrite;
	CString szResult;
	CString szVF1Diff, szVF2Diff, szWLDDiff, szWLPDiff, szHW1Diff, szLOPDiff, szVZ1Diff, szVZ2Diff;
	
	CString szCorGetVF1[8], szCorGetVF2[8], szCorGetWLD[8], szCorGetWLP[8],
			szCorGetVZ1[8], szCorGetLOP[8], szCorGetHW1[8], szCorGetVZ2[8];
	CString szTemp;
	int a;
	double  dVF1Ori = 0, dVF2Ori = 0, dWLDOri = 0, dWLPOri = 0, dHW1Ori = 0 , dLOPOri = 0, dVZ1Ori = 0,dVZ2Ori = 0;
	double  dVF1Diff = 0, dVF2Diff = 0, dWLDDiff = 0, dWLPDiff = 0, dHW1Diff = 0, dLOPDiff = 0, dVZ1Diff = 0, dVZ2Diff = 0;

	TesterCORGET("1", m_szCorInVF1, "D1", szResult);	//	VF1
	for (int i = 0; i < 7; i ++ )
	{
		a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetVF1[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetVF1[i + 1] = szResult;
			}
		}
	}
	if (szCorGetVF1[6] != "")
		dVF1Ori = atof((LPCTSTR)szCorGetVF1[6]);


	TesterCORGET("1", m_szCorInVF2, "D1", szResult);	//	VF2
	for (int i = 0; i < 7; i ++ )
	{
		a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetVF2[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetVF2[i + 1] = szResult;
			}
		}
	}
	if (szCorGetVF2[6] != "")
		dVF2Ori = atof((LPCTSTR)szCorGetVF2[6]);

	TesterCORGET("1", m_szCorInWLD, "D1", szResult);	//	WLD
	for (int i = 0; i < 7; i ++ )
	{a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetWLD[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetWLD[i + 1] = szResult;
			}
		}
	}
	if (szCorGetWLD[6] != "")
		dWLDOri = atof((LPCTSTR)szCorGetWLD[6]);

	TesterCORGET("1", m_szCorInWLP, "D1", szResult);	//	WLP
	for (int i = 0; i < 7; i ++ )
	{a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetWLP[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetWLP[i + 1] = szResult;
			}
		}
	}
	if (szCorGetWLP[6] != "")
		dWLPOri = atof((LPCTSTR)szCorGetWLP[6]);

	TesterCORGET("1", m_szCorInVZ1, "D1", szResult);	//	VZ1
	for (int i = 0; i < 7; i ++ )
	{
		a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetVZ1[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetVZ1[i + 1] = szResult;
			}
		}
	}
	if (szCorGetVZ1[6] != "")
		dVZ1Ori = atof((LPCTSTR)szCorGetVZ1[6]);

	TesterCORGET("1", m_szCorInHW1, "D1", szResult);	//	Addition 1
	for (int i = 0; i < 7; i ++ )
	{
		a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetHW1[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetHW1[i + 1] = szResult;
			}
		}
	}
	if (szCorGetHW1[6] != "")
		dHW1Ori = atof((LPCTSTR)szCorGetHW1[6]);

	TesterCORGET("1", m_szCorInVZ2, "D1", szResult);	//	Addition 2
	for (int i = 0; i < 7; i ++ )
	{
		a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetVZ2[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetVZ2[i + 1] = szResult;
			}
		}
	}
	if (szCorGetVZ2[6] != "")
		dVZ2Ori = atof((LPCTSTR)szCorGetVZ2[6]);

	TesterCORGET("1", m_szCorInLOP, "D1", szResult);	//	LOP
	for (int i = 0; i < 7; i ++ )
	{a = szResult.Find(",");
		if (a != -1)
		{
			szTemp = szResult.Left(a);
			szResult = szResult.Mid(a + 1);	
			szCorGetLOP[i] = szTemp;

			a = szResult.Find(",");
			if ( a == -1)
			{
				szResult.Replace(";","");
				szCorGetLOP[i + 1] = szResult;
			}
		}
	}
	if (szCorGetLOP[5] != "")
		dLOPOri = atof((LPCTSTR)szCorGetLOP[5]);

	CString szContent = "Set With Original Data?";
	CString szTitle   = "Set Data";
	
	CString szMsg = "Set Data";
	int nReply = SetAlert_Msg(IDS_BH_CHECK_PROBE_POS, szMsg, "With Original Data", "Current Data" ,"Last Set Data" , glHMI_ALIGN_CENTER, -1, 0);

	if( m_szCorInVZ2 == "IR")
	{
		dVZ2Diff = dVZ2Diff/1000;
	}

	if ( nReply == 1 )
	{
		m_dLast_VF1 = dVF1Diff = m_dDiff_VF1 + dVF1Ori;
		m_dLast_VF2 = dVF2Diff = m_dDiff_VF2 + dVF2Ori;
		m_dLast_WLP = dWLPDiff = m_dDiff_WLP + dWLPOri;
		m_dLast_WLD = dWLDDiff = m_dDiff_WLD + dWLDOri;
		m_dLast_HW1 = dHW1Diff = m_dDiff_HW1 + dHW1Ori;
		m_dLast_VZ1 = dVZ1Diff = m_dDiff_VZ1 + dVZ1Ori;
		m_dLast_VZ2 = dVZ2Diff = m_dDiff_VZ2 + dVZ2Ori;
		m_dLast_LOP = dLOPDiff = m_dDiff_LOP * dLOPOri;

		szWrite.Format("%s,[Original],%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s)",
		szTime, dVF1Ori, m_szCorInVF1, dVF2Ori, m_szCorInVF2, dWLDOri, m_szCorInWLD, dWLPOri, m_szCorInWLP, dHW1Ori, m_szCorInHW1, m_szCorInVZ1, dVZ1Ori, dVZ2Ori, m_szCorInVZ2, dLOPOri, m_szCorInLOP);    
		cfNew.WriteString(szWrite);
		cfNew.WriteString("\n");
	}
	else if (nReply ==5)
	{
		m_dLast_VF1 = dVF1Diff = m_dDiff_VF1;
		m_dLast_VF2 = dVF2Diff = m_dDiff_VF2;
		m_dLast_WLP = dWLPDiff = m_dDiff_WLP;
		m_dLast_WLD = dWLDDiff = m_dDiff_WLD;
		m_dLast_HW1 = dHW1Diff = m_dDiff_HW1;
		m_dLast_LOP = dLOPDiff = m_dDiff_LOP;
		m_dLast_VZ1 = dVZ1Diff = m_dDiff_VZ1;
		m_dLast_VZ2 = dVZ2Diff = m_dDiff_VZ2;
	}
	else
	{
		m_dDiff_VF1		=	dVF1Diff		=	m_dLast_VF1;
		m_dDiff_VF2		=	dVF2Diff		=	m_dLast_VF2;
		m_dDiff_WLP		=	dWLPDiff		=	m_dLast_WLP;
		m_dDiff_WLD		=	dWLDDiff		=	m_dLast_WLD;
		m_dDiff_HW1		=	dHW1Diff		=	m_dLast_HW1;
		m_dDiff_VZ1		=	dVZ1Diff		=	m_dLast_VZ1;
		m_dDiff_VZ2		=	dVZ2Diff		=	m_dLast_VZ2;
		m_dDiff_LOP		=	dLOPDiff		=	m_dLast_LOP;
	}

	szVF1Diff.Format("%f", dVF1Diff);
	szVF2Diff.Format("%f", dVF2Diff);
	szWLPDiff.Format("%f", dWLPDiff);
	szWLDDiff.Format("%f", dWLDDiff);
	szHW1Diff.Format("%f", dHW1Diff);
	szVZ1Diff.Format("%f", dVZ1Diff);
	szVZ2Diff.Format("%f", dVZ2Diff);
	szLOPDiff.Format("%f", dLOPDiff);

	//if (m_dDiff_VF1 != 0)
	//{	
			TesterCORSET("1", m_szCorInVF1, "D1", "1", szCorGetVF1[4], szCorGetVF1[5], szVF1Diff, szResult);
	//}
	//if (m_dDiff_VF2 != 0)
	//{
			TesterCORSET("1", m_szCorInVF2, "D1", "1", szCorGetVF2[4], szCorGetVF2[5], szVF2Diff, szResult);
	////}	
	////if (m_dDiff_WLD != 0)
	////{
			TesterCORSET("1", m_szCorInWLD, "D1", "1", szCorGetWLD[4], szCorGetWLD[5], szWLDDiff, szResult);
	//}
	//if (m_dDiff_WLP != 0)
	//{
			TesterCORSET("1", m_szCorInWLP, "D1", "1", szCorGetWLP[4], szCorGetWLP[5], szWLPDiff, szResult);
	//}
	//if (m_dDiff_HW1 != 0)
	//{
			TesterCORSET("1", m_szCorInVZ1, "D1", "1", szCorGetVZ1[4], szCorGetVZ1[5], szVZ1Diff, szResult);	
	//}
	//if (m_dDiff_VZ1 != 0)
	//{
			TesterCORSET("1", m_szCorInHW1, "D1", "1", szCorGetHW1[4], szCorGetHW1[5], szHW1Diff, szResult);
	//}
	//if (m_dDiff_VZ2 != 0)
	//{
			TesterCORSET("1", m_szCorInVZ2, "D1", "1", szCorGetVZ2[4], szCorGetVZ2[5], szVZ2Diff, szResult);
	//}
	//if (m_dDiff_LOP != 0)
	//{
			TesterCORSET("1", m_szCorInLOP, "D1", "1", szCorGetLOP[4], szLOPDiff, szCorGetLOP[6], szResult);
	//}

	CString szTestSpecification ="";
	if (ConnectTesterTerminal())
	{
		SendRequestToTesterTerminal("TS", szTestSpecification);
		DisconnectTesterTerminal();
	}

	a = szTestSpecification.ReverseFind('\\');
	if ( a != -1)
	{
		szTestSpecification = szTestSpecification.Mid(a+1);
	}
	a = szTestSpecification.Find(".");
	if ( a != -1)
	{
		szTestSpecification = szTestSpecification.Left(a);
	}

	szWrite.Format("%s,[Set],%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%s",
		szTime, m_dDiff_VF1, m_szCorInVF1, m_dDiff_VF2, m_szCorInVF2, m_dDiff_WLD, m_szCorInWLD, m_dDiff_WLP, m_szCorInWLP,
				m_dDiff_HW1, m_szCorInHW1, m_dDiff_VZ1, m_szCorInVZ1, m_dDiff_VZ2, m_szCorInVZ2, m_dDiff_LOP, m_szCorInLOP, szTestSpecification);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	cfNew.WriteString(szWrite);
	cfNew.WriteString("\n");

	cfNew.WriteString(szTime + ",[Result]," + 
		szVF1Diff + "(" + m_szCorInVF1 + ")," + 
		szVF2Diff + "(" + m_szCorInVF2 + ")," +
		szWLDDiff + "(" + m_szCorInWLD + ")," + 
		szWLPDiff + "(" + m_szCorInWLP + ")," +
		szHW1Diff + "(" + m_szCorInHW1 + ")," + 
		szVZ1Diff + "(" + m_szCorInVZ1 + ")," +
		szVZ2Diff + "(" + m_szCorInVZ2 + ")," +
		szLOPDiff + "(" + m_szCorInLOP + "),");
	cfNew.WriteString("\n");
	cfNew.Close();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferTable::StartAutoCorrelation_HMI(IPC_CServiceMessage &svMsg)
{
	SaveCP100IndependentData();

	BOOL bReturn = StartAutoCorrelation();
// auto to do, no need manual press it.
	m_oACMain.SmoothAllDataColumn(m_lACMovingAvarage);
	m_oACMain.AddFirstPointAndEndPoint_ALL(m_dACUpperBoundary,m_dACLowerBoundary);

	if ( bReturn == TRUE )
	{
		m_bACMainIsSetup = TRUE;
		HmiMessage("Generate Auto Correlation File Complete!", "Auto Correlation");
	}
	else
	{
		HmiMessage("Generate Auto Correlation File Failed!", "Auto Correlation");
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SetUpAutoCorrMain_HMI(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	
	SetUpTheAutoCorrMain();

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::SelectAutoCorrelationDisplay(IPC_CServiceMessage &svMsg)
{
	LONG lSelection = 0;
	svMsg.GetMsg(sizeof(LONG), &lSelection);

	switch( lSelection )
	{
	case 0:	//	chart
		m_lTesterEditorLeft	= 1600;
		m_lTesterStudioLeft	= 1600;
		break;
	case 1:	//	editor
		m_lTesterEditorLeft	= 780;
		m_lTesterStudioLeft	= 1600;
		break;
	case 2:	//	studio
		m_lTesterEditorLeft	= 1600;
		m_lTesterStudioLeft	= 780;
		break;
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferTable::SetUpTheAutoCorrMain()
{
	if ( m_szAutoCorrStdFile == "" || m_szAutoCorrTestFile == "" || m_szAutoCorrOutputPath == "" )
	{
		return FALSE;
	}

	m_oACMain.SetStdFile(m_szAutoCorrStdFile);
	m_oACMain.SetTestFile(m_szAutoCorrTestFile);
	m_oACMain.SetOutputFileDir(m_szAutoCorrOutputPath);

	return TRUE;
}

BOOL CWaferTable::StartAutoCorrelation()
{
	if ( m_szAutoCorrStdFile == "" || m_szAutoCorrTestFile == "" || m_szAutoCorrOutputPath == "" )
	{
		return FALSE;
	}

	m_oACMain.SetStdFile(m_szAutoCorrStdFile);
	m_oACMain.SetTestFile(m_szAutoCorrTestFile);
	m_oACMain.SetOutputFileDir(m_szAutoCorrOutputPath);
	BOOL bSetUpReturn = m_oACMain.SetUpTheRawDataMap();
	if ( bSetUpReturn == FALSE )
	{
		return FALSE;
	}

	//if ( m_bEnableSimpleAutoCorr == TRUE )
	//{
	//	if ( m_bACMainIsSetup == FALSE )
	//	{
	//		HmiMessage("Please click 'Generate!' First");
	//		bReturn = FALSE;
	//	}
	//	else
	//	{
	//		AC_SIMPLE_PACKAGE acsp;
	//		acsp.dMaxIRInput		= m_dMaxIRInput;
	//		acsp.dMinLOP1			= m_dMinLOP1;
	//		acsp.lXAxisSelection	= m_lXAxisSelection;
	//		acsp.dACUpperBoundary	= m_dACUpperBoundary;
	//		acsp.dACLowerBoundary	= m_dACLowerBoundary;
	//		acsp.szPWRPath			= m_szAutoCorrOutputPath + "\\" + m_szPWRPath;	// a/b
	//		acsp.szWLPPath			= m_szAutoCorrOutputPath + "\\" + m_szCorInWLP + ".csv";	// a-b
	//		acsp.szHW1Path			= m_szAutoCorrOutputPath + "\\" + m_szCorInHW1 + ".csv";	// a-b
	//		acsp.szWLDPath			= m_szAutoCorrOutputPath + "\\" + m_szCorInWLD + ".csv";	// a-b
	//		bReturn = m_oACMain.AddFirstPointAndEndPoint(acsp);
	//		return m_oACMain.SimpleGenerate(acsp);	//	dimmed before
	//	}
	//}

	return TRUE;
}

BOOL CWaferTable::DailyCheck(CString szGoldenFile, CString szProbedFile)
{
	CStdioFile cfGolden, cfProbed;
	if (cfGolden.Open(szGoldenFile, CFile::modeReadWrite) == FALSE)
	{
		HmiMessage("Fail to open golden file " + szGoldenFile, "Auto Correlation");
		return FALSE;
	}

	//Calculate First File Data Size
	CString szTemp1;
	cfGolden.SeekToBegin();
	while( 1 )
	{
		if( cfGolden.ReadString(szTemp1) )
		{
			if( szTemp1.Find("TEST,BIN")!=-1 )
			{
				break;
			}
		}
	}

	//Define Data
	CStringArray szGdnVF1, szGdnVF2, szGdnWLP, szGdnWLD, szGdnLOP, szGdnHW1, szGdnVZ1, szGdnVZ2;
	szGdnVF1.Add("DummyStart");		szGdnWLP.Add("DummyStart");
	szGdnWLD.Add("DummyStart");		szGdnLOP.Add("DummyStart");
	szGdnVF2.Add("DummyStart");		szGdnHW1.Add("DummyStart");
	szGdnVZ1.Add("DummyStart");		szGdnVZ2.Add("DummyStart");
	WSDelScanReferInfo();
	int nIdxCol = 0, nIdxRow = 0, nIdxVF1 = 0, nIdxVF2 = 0, nIdxWLD = 0, nIdxWLP = 0, nIdxLOP = 0, nIdxHW1 = 0, nIdxVZ1 = 0, nIdxVZ2 = 0;
	CStringArray szaDataList;
	szaDataList.RemoveAll();
	ParseRawData(szTemp1, szaDataList);
	for(int j=0; j<szaDataList.GetSize(); j++)
	{
		CString szTemp = szaDataList.GetAt(j);
		if (szTemp.Find(m_szCorInVF1) != -1)
		{
			nIdxVF1 = j;
		}
		if (szTemp.Find(m_szCorInVF2) != -1)
		{
			nIdxVF2 = j;
		}
		if (szTemp.Find(m_szCorInWLD) != -1)
		{		
			nIdxWLD = j;
		}
		if (szTemp.Find(m_szCorInWLP) != -1)
		{
			nIdxWLP = j;
		}
		if (szTemp.Find(m_szCorInVZ1) != -1)
		{
			nIdxHW1 = j;
		}
		if (szTemp.Find(m_szCorInLOP) != -1)
		{
			nIdxLOP = j;
		}
	
		if (szTemp.Find(m_szCorInHW1) != -1)
		{
			nIdxVZ1 = j;
		}
		if (szTemp.Find(m_szCorInVZ2) != -1)
		{
			nIdxVZ2 = j;
		}
		if( szTemp.Find("PosX") != -1 )
		{
			nIdxCol = j;
		}
		if( szTemp.Find("PosY") != -1 )
		{
			nIdxRow = j;
		}
	}

	int nIdxMax = 0;
	nIdxMax = max(nIdxVF1, nIdxMax);		nIdxMax = max(nIdxVF2, nIdxMax);
	nIdxMax = max(nIdxWLP, nIdxMax);		nIdxMax = max(nIdxWLD, nIdxMax);
	nIdxMax = max(nIdxLOP, nIdxMax);		nIdxMax = max(nIdxHW1, nIdxMax);
	nIdxMax = max(nIdxVZ1, nIdxMax);		nIdxMax = max(nIdxVZ2, nIdxMax);
	nIdxMax = max(nIdxCol, nIdxMax);		nIdxMax = max(nIdxRow, nIdxMax);
	while( 1 )
	{
		if( cfGolden.ReadString(szTemp1) )
		{
			szaDataList.RemoveAll();
			ParseRawData(szTemp1, szaDataList);
			int k = szaDataList.GetSize();

			if( k>nIdxMax )
			{
				szGdnVF1.Add(szaDataList.GetAt(nIdxVF1));
				szGdnVF2.Add(szaDataList.GetAt(nIdxVF2));
				szGdnWLP.Add(szaDataList.GetAt(nIdxWLP));
				szGdnWLD.Add(szaDataList.GetAt(nIdxWLD));
				szGdnHW1.Add(szaDataList.GetAt(nIdxHW1));
				szGdnLOP.Add(szaDataList.GetAt(nIdxLOP));
				szGdnVZ1.Add(szaDataList.GetAt(nIdxVZ1));
				szGdnVZ2.Add(szaDataList.GetAt(nIdxVZ2));
				INT nPosX = atoi(szaDataList.GetAt(nIdxCol));
				INT nPosY = atoi(szaDataList.GetAt(nIdxRow));
				WSSetScanReferPosition(nPosX, nPosY);
			}
		}
		else
		{
			break;
		}
	}
	cfGolden.Close();
	//First File Decode Finish.

	//Calculate Second File Date Size
	if (cfProbed.Open(szProbedFile, CFile::modeReadWrite) == FALSE)
	{
		HmiMessage("Fail to open probed file " + szProbedFile, "Auto Correlation");
		return FALSE;
	}

	CString szTemp2;
	cfProbed.SeekToBegin();
	while( 1 )
	{
		if( cfProbed.ReadString(szTemp2) )
		{
			if( szTemp2.Find("TEST,BIN")!=-1 )
			{
				break;
			}
		}
	}

	CStringArray szPrbVF1, szPrbVF2, szPrbWLP, szPrbWLD, szPrbLOP, szPrbHW1, szPrbVZ1, szPrbVZ2;
	szPrbVF1.Add("DummyStart");	szPrbWLP.Add("DummyStart");
	szPrbWLD.Add("DummyStart");	szPrbLOP.Add("DummyStart");
	szPrbVF2.Add("DummyStart");	szPrbHW1.Add("DummyStart");
	szPrbVZ1.Add("DummyStart");	szPrbVZ2.Add("DummyStart");
	WSClearScanRemainDieList();
	nIdxCol = 0; nIdxRow = 0; nIdxVF1 = 0; nIdxVF2 = 0; nIdxWLD = 0; nIdxWLP = 0; nIdxLOP = 0; nIdxHW1 = 0; nIdxVZ1 = 0; nIdxVZ2 = 0;
	szaDataList.RemoveAll();
	ParseRawData(szTemp2, szaDataList);
	for(int j=0; j<szaDataList.GetSize(); j++)
	{
		CString szTemp = szaDataList.GetAt(j);
		if (szTemp.Find(m_szCorInVF1) != -1)
		{
			nIdxVF1 = j;
		}
		if (szTemp.Find(m_szCorInVF2) != -1)
		{
			nIdxVF2 = j;
		}
		if (szTemp.Find(m_szCorInWLD) != -1)
		{
			nIdxWLD = j;
		}
		if (szTemp.Find(m_szCorInWLP) != -1)
		{
			nIdxWLP = j;
		}
		if (szTemp.Find(m_szCorInVZ1) != -1)
		{
			nIdxHW1 = j;
		}
		if (szTemp.Find(m_szCorInLOP) != -1)
		{
			nIdxLOP = j;
		}
		if (szTemp.Find(m_szCorInHW1) != -1)
		{
			nIdxVZ1 = j;
		}
		if (szTemp.Find(m_szCorInVZ2) != -1)
		{
			nIdxVZ2 = j;
		}
		if( szTemp.Find("PosX") != -1 )
		{
			nIdxCol = j;
		}
		if( szTemp.Find("PosY") != -1 )
		{
			nIdxRow = j;
		}
	}

	nIdxMax = 0;
	nIdxMax = max(nIdxVF1, nIdxMax);		nIdxMax = max(nIdxVF2, nIdxMax);
	nIdxMax = max(nIdxWLP, nIdxMax);		nIdxMax = max(nIdxWLD, nIdxMax);
	nIdxMax = max(nIdxLOP, nIdxMax);		nIdxMax = max(nIdxHW1, nIdxMax);
	nIdxMax = max(nIdxVZ1, nIdxMax);		nIdxMax = max(nIdxVZ2, nIdxMax);
	nIdxMax = max(nIdxCol, nIdxMax);		nIdxMax = max(nIdxRow, nIdxMax);
	while( 1 )
	{
		if( cfProbed.ReadString(szTemp2) )
		{
			szaDataList.RemoveAll();
			ParseRawData(szTemp2, szaDataList);
			int k = szaDataList.GetSize();
			if( k>nIdxMax )
			{
				szPrbVF1.Add(szaDataList.GetAt(nIdxVF1));
				szPrbVF2.Add(szaDataList.GetAt(nIdxVF2));
				szPrbWLP.Add(szaDataList.GetAt(nIdxWLP));
				szPrbWLD.Add(szaDataList.GetAt(nIdxWLD));
				szPrbHW1.Add(szaDataList.GetAt(nIdxHW1));
				szPrbLOP.Add(szaDataList.GetAt(nIdxLOP));
				szPrbVZ1.Add(szaDataList.GetAt(nIdxVZ1));
				szPrbVZ2.Add(szaDataList.GetAt(nIdxVZ2));
				INT nPosX = atoi(szaDataList.GetAt(nIdxCol));
				INT nPosY = atoi(szaDataList.GetAt(nIdxRow));
				WSAddScanRemainDieIntoList(nPosX, nPosY, 0);
			}
		}
		else
		{
			break;
		}
	}
	cfProbed.Close();
	//Second File Decode Finish

	double dDiffsVF1 = 0, dDiffsVF2 = 0, dDiffsWLD = 0, dDiffsWLP = 0, dDiffsLOP = 0, dDiffsHW1 = 0, dDiffsVZ1 = 0, dDiffsVZ2 = 0;
	double dCountVF1 = 0, dCountVF2 = 0, dCountWLD = 0, dCountWLP = 0, dCountLOP = 0, dCountHW1 = 0, dCountVZ1 = 0, dCountVZ2 = 0;

	FILE *fpWLD = fopen("C:\\MapSorter\\UserData\\OutputFile\\WLD.csv", "wt");
	FILE *fpWLP = fopen("C:\\MapSorter\\UserData\\OutputFile\\WLP.csv", "wt");
	FILE *fpLOP = fopen("C:\\MapSorter\\UserData\\OutputFile\\LOP.csv", "wt");
	FILE *fpHW1 = fopen("C:\\MapSorter\\UserData\\OutputFile\\HW.csv",  "wt");

	for(ULONG jP=1; jP<=WSGetScanRemainDieTotal(); jP++)	//	Tested P raw data
	{
		ULONG ulIndex = 0;
		LONG lPrbCol = 0, lPrbRow = 0, lGdnCol = 0, lGdnRow = 0;
		if( WSGetScanRemainDieFromList(jP, lPrbCol, lPrbRow, ulIndex)==FALSE )
		{
			continue;
		}

		for(ULONG jG=1; jG<=WSGetScanReferTotalDie(); jG++)	//	Golden A raw data
		{
			if( WSGetScanReferPosition(jG, lGdnCol, lGdnRow)==FALSE )
			{
				continue;
			}

			if( lGdnCol==lPrbCol && lGdnRow==lPrbRow )
			{
				BOOL bValidWLD = FALSE, bValidWLP = FALSE, bValidLOP = FALSE, bValidHW1 = FALSE;
				double dGdnVF1 = 0, dGdnVF2 = 0, dGdnWLD = 0, dGdnWLP = 0, dGdnLOP = 0, dGdnHW1 = 0, dGdnVZ1 =0, dGdnVZ2 =0;
				double dPrbVF1 = 0, dPrbVF2 = 0, dPrbWLD = 0, dPrbWLP = 0, dPrbLOP = 0, dPrbHW1 = 0, dPrbVZ1 =0, dPrbVZ2 =0;
				double dDifWLD = 0, dDifWLP = 0, dDifHW1 = 0, dGinLOP = 0;

				if( szGdnWLD.GetAt(jG) != "" && szPrbWLD.GetAt(jP) != "" )
				{
					dGdnWLD = atof((LPCTSTR)szGdnWLD.GetAt(jG));
					dPrbWLD = atof((LPCTSTR)szPrbWLD.GetAt(jP));
					if (dGdnWLD > m_dCorDnWLD && dGdnWLD < m_dCorUpWLD &&
						dPrbWLD > m_dCorDnWLD && dPrbWLD < m_dCorUpWLD )
					{
						dDifWLD = dGdnWLD - dPrbWLD;
						dCountWLD ++;
						dDiffsWLD = dDiffsWLD + dDifWLD;
						bValidWLD = TRUE;
					}
				}

				if( szGdnWLP.GetAt(jG) != "" && szPrbWLP.GetAt(jP) != "" )
				{
					dGdnWLP = atof((LPCTSTR)szGdnWLP.GetAt(jG));
					dPrbWLP = atof((LPCTSTR)szPrbWLP.GetAt(jP));
					if( dGdnWLP < m_dCorUpWLP && dGdnWLP > m_dCorDnWLP &&
						dPrbWLP < m_dCorUpWLP && dPrbWLP > m_dCorDnWLP )
					{
						dDifWLP = dGdnWLP - dPrbWLP;
						dCountWLP ++;
						dDiffsWLP = dDiffsWLP + dDifWLP;
						bValidWLP = TRUE;
					}
				}

				if( szGdnHW1.GetAt(jG) != "" && szPrbHW1.GetAt(jP) != "" )
				{
					dGdnHW1 = atof((LPCTSTR)szGdnHW1.GetAt(jG));
					dPrbHW1 = atof((LPCTSTR)szPrbHW1.GetAt(jP));
					if (dGdnHW1 < m_dCorUpHW1 && dGdnHW1 > m_dCorDnHW1 &&
						dPrbHW1 < m_dCorUpHW1 && dPrbHW1 > m_dCorDnHW1 )
					{
						dDifHW1 = dGdnHW1 - dPrbHW1;
						dCountHW1 ++;
						dDiffsHW1 = dDiffsHW1 + dDifHW1;
						bValidHW1 = TRUE;
					}
				}

				if( szGdnLOP.GetAt(jG) != "" && szPrbLOP.GetAt(jP) != "" )
				{
					dGdnLOP = atof((LPCTSTR)szGdnLOP.GetAt(jG));
					dPrbLOP = atof((LPCTSTR)szPrbLOP.GetAt(jP));
					if( dGdnLOP < m_dCorUpLOP && dGdnLOP > m_dCorDnLOP &&
						dPrbLOP < m_dCorUpLOP && dPrbLOP > m_dCorDnLOP &&
						dPrbLOP!=0 )
					{
						dGinLOP = dGdnLOP / dPrbLOP;
						dCountLOP ++;
						dDiffsLOP = dDiffsLOP + dGinLOP;
						bValidLOP = TRUE;
					}
				}

				if( szGdnVF1.GetAt(jG) != "" && szPrbVF1.GetAt(jP) != "" )
				{
					dGdnVF1 = atof((LPCTSTR)szGdnVF1.GetAt(jG));
					dPrbVF1 = atof((LPCTSTR)szPrbVF1.GetAt(jP));
					if (dGdnVF1 > m_dCorDnVF1 && dGdnVF1 < m_dCorUpVF1 && 
						dPrbVF1 > m_dCorDnVF1 && dPrbVF1 < m_dCorUpVF1 )
					{
						dDiffsVF1 = dDiffsVF1 + dGdnVF1 - dPrbVF1;
						dCountVF1 ++;
					}
				}

				if( szGdnVF2.GetAt(jG) != "" && szPrbVF2.GetAt(jP) != "" )
				{
					dGdnVF2 = atof((LPCTSTR)szGdnVF2.GetAt(jG));
					dPrbVF2 = atof((LPCTSTR)szPrbVF2.GetAt(jP));
					if (dGdnVF2 < m_dCorUpVF2 && dGdnVF2 > m_dCorDnVF2 &&
						dPrbVF2 < m_dCorUpVF2 && dPrbVF2 > m_dCorDnVF2 )
					{
						dCountVF2 ++;
						dDiffsVF2 = dDiffsVF2 + dGdnVF2 - dPrbVF2;
					}
				}

				if( szGdnVZ1.GetAt(jG) != "" && szPrbVZ1.GetAt(jP) != "" )
				{
					dGdnVZ1 = atof((LPCTSTR)szGdnVZ1.GetAt(jG));
					dPrbVZ1 = atof((LPCTSTR)szPrbVZ1.GetAt(jP));
					if (dGdnVZ1 < m_dCorUpVZ1 && dGdnVZ1 > m_dCorDnVZ1 &&
						dPrbVZ1 < m_dCorUpVZ1 && dPrbVZ1 > m_dCorDnVZ1 )
					{
						dCountVZ1 ++;
						dDiffsVZ1 = dDiffsVZ1 + dGdnVZ1 - dPrbVZ1;
					}
				}

				if( szGdnVZ2.GetAt(jG) != "" && szPrbVZ2.GetAt(jP) != "" )
				{
					dGdnVZ2 = atof((LPCTSTR)szGdnVZ2.GetAt(jG));
					dPrbVZ2 = atof((LPCTSTR)szPrbVZ2.GetAt(jP));
					if (dGdnVZ2 < m_dCorUpVZ2 && dGdnVZ2 > m_dCorDnVZ2 &&
						dPrbVZ2 < m_dCorUpVZ2 && dPrbVZ2 > m_dCorDnVZ2 )
					{
						dCountVZ2 ++;
						dDiffsVZ2 = dDiffsVZ2 + dGdnVZ2 - dPrbVZ2;
					}
				}

				if( m_lXAxisSelection==0 && bValidWLD )
				{
					if( fpWLD!=NULL && bValidWLD )
					{
						fprintf(fpWLD, "%f,%f\n", dGdnWLD, dDifWLD);
					}
					if( fpWLP!=NULL && bValidWLD )
					{
						fprintf(fpWLP, "%f,%f\n", dGdnWLD, dDifWLP);
					}
					if( fpLOP!=NULL && bValidLOP )
					{
						fprintf(fpLOP, "%f,%f\n", dGdnWLD, dGinLOP);
					}
					if( fpHW1!=NULL && bValidHW1 )
					{
						fprintf(fpHW1, "%f,%f\n", dGdnWLD, dDifHW1);
					}
				}
				if( m_lXAxisSelection==1 && bValidWLP )
				{
					if( fpWLD!=NULL && bValidWLD )
					{
						fprintf(fpWLD, "%f,%f\n", dGdnWLP, dDifWLD);
					}
					if( fpWLP!=NULL && bValidWLP )
					{
						fprintf(fpWLP, "%f,%f\n", dGdnWLP, dDifWLP);
					}
					if( fpLOP!=NULL && bValidLOP )
					{
						fprintf(fpLOP, "%f,%f\n", dGdnWLP, dGinLOP);
					}
					if( fpHW1!=NULL && bValidHW1 )
					{
						fprintf(fpHW1, "%f,%f\n", dGdnWLP, dDifHW1);
					}
				}

				break;
			}	//	same row and column, i.e. the same die
		}		//	Golden raw data list
	}			//	Probed raw data list.

	if( fpWLD!=NULL )
	{
		fclose(fpWLD);
	}
	if( fpWLP!=NULL )
	{
		fclose(fpWLP);
	}
	if( fpLOP!=NULL )
	{
		fclose(fpLOP);
	}
	if( fpHW1!=NULL )
	{
		fclose(fpHW1);
	}

	WSClearScanRemainDieList();
	WSDelScanReferInfo();

	m_dDiff_VF1 = dDiffsVF1 / dCountVF1; 
	m_dDiff_VF2 = dDiffsVF2 / dCountVF2;
	m_dDiff_WLD = dDiffsWLD / dCountWLD;
	m_dDiff_WLP = dDiffsWLP / dCountWLP;
	m_dDiff_HW1 = dDiffsHW1 / dCountHW1;
	m_dDiff_LOP = dDiffsLOP / dCountLOP;
	m_dDiff_VZ1 = dDiffsVZ1 / dCountVZ1;
	m_dDiff_VZ2 = dDiffsVZ2 / dCountVZ2;

	CStdioFile cfNew;
	if (cfNew.Open(CP_DC_RECORD_FILE, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText))
	{
		CTime theTime = CTime::GetCurrentTime();
		CString szTime = theTime.Format("%Y%m%d%H%M%S");
		cfNew.SeekToEnd();
		CString szTemp;
		szTemp.Format(",[Calculate],%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s),%f(%s)\n",
			m_dDiff_VF1, m_szCorInVF1, m_dDiff_VF2, m_szCorInVF2, m_dDiff_WLD, m_szCorInWLD, m_dDiff_WLP, m_szCorInWLP,
			m_dDiff_HW1, m_szCorInHW1, m_dDiff_VZ1, m_szCorInVZ1, m_dDiff_VZ2, m_szCorInVZ2, m_dDiff_LOP, m_szCorInLOP);
		szTemp = szTime + szTemp;
		cfNew.WriteString(szTemp);
		cfNew.Close();
	}

	return TRUE;
}

LONG CWaferTable::SmoothTheData(IPC_CServiceMessage &svMsg)	//	removed on HMI
{
	BOOL bReturn = TRUE;

	m_oACMain.SmoothAllDataColumn(m_lACMovingAvarage);
	HmiMessage("Smooth Complete!", "Auto Correlation");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferTable::AddBoundary(IPC_CServiceMessage &svMsg)	//	removed on HMI
{
	BOOL bReturn = TRUE;

	m_oACMain.AddFirstPointAndEndPoint_ALL(m_dACUpperBoundary,m_dACLowerBoundary);
	HmiMessage("Boundary is added!", "Auto Correlation");

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


#endif