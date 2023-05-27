#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferLoader.h"
#include "math.h"
#include "FileUtil.h"
#include "MS896A.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UCHAR CWaferLoader::GetBCModel()
{
	return m_ucBarcodeModel;
}

BOOL CWaferLoader::WL_InputBarcode(INT nTable, CString &szBarcode)
{
	CString szMsg	= "input barcode: ";
	
	CString szTitle = "Please " + szMsg;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL bStatus = 0;

	while( 1 )
	{
		szBarcode = "";

		bStatus	= HmiStrInputKeyboard(szTitle, szBarcode);

		if( bStatus==FALSE )
		{
			break;
		}
		if( szBarcode.IsEmpty() )
		{
			HmiMessage_Red_Back("Please input barcode.", "WFT barcode");
			continue;
		}
		if( CheckBarcodeLength(szBarcode) )
		{
			break;
		}
		else
		{
			HmiMessage_Red_Back("Barcode length incorrect.", "WFT barcode");
		}
	}

	if( pApp->GetCustomerName()=="HuaLei" )
	{
		szBarcode = szBarcode.MakeUpper();
	}

	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg + szBarcode);

	return bStatus;
}

BOOL CWaferLoader::CreateBarcodeScanner(VOID)
{
	BOOL bIsCreated = FALSE;
	CString szLog;	//v4.50A4

	szLog.Format("WL: Create Barcode Model: %d", GetBCModel());
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (!m_oCommPort.IsCreated())	//v4.40T12
	{
		//Create COM port for barcode scanning (Port No get from Machine)
		if ( (GetBCModel() == WL_BAR_DEFAULT) || (GetBCModel() == WL_BAR_DEFAULT_COM) )		//v4.40T10
		{
			//New DEFAULT_COM com port settings for SanAn
			m_oCommPort.Create((DWORD)CMS896AApp::m_lBarCode1ComPort, 9600, 8, SFM_PARITY_NONE, 1, 5, 2000);
			szLog.Format("WL: BC scanner connected to COM #%d - 9600,8,NONE,1 (DEFAULT COM) (STATUS=%d)",
				CMS896AApp::m_lBarCode1ComPort, m_oCommPort.IsCreated()); 
		}
		else
		{
			//Default SYMBOL/DATALOGIC com port settings
			if( GetBCModel() == WL_BAR_DATALOGIC )
				m_oCommPort.Create((DWORD)CMS896AApp::m_lBarCode1ComPort, 9600, 7, SFM_PARITY_EVEN, 2, 5, 2000);
			else
				m_oCommPort.Create((DWORD)CMS896AApp::m_lBarCode1ComPort, 9600, 7, SFM_PARITY_EVEN, 1, 5, 2000);
			szLog.Format("WL: BC scanner connected to COM #%d - 9600,7,EVEN,1 (SYMBOL) (STATUS=%d)",
				CMS896AApp::m_lBarCode1ComPort, m_oCommPort.IsCreated()); 
		}
	}

	bIsCreated = m_oCommPort.IsCreated();
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);	//v4.50A4
	return TRUE;
}

BOOL CWaferLoader::ReadBarcode(CString *szData, int nTimeOut, int nRetry)
{
	BOOL bOnOff2DBarCodeScanner= FALSE; //4.54T22  

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( GetBCModel() == WL_BAR_DATALOGIC)
	{
		bOnOff2DBarCodeScanner = TRUE;
	}

	char scTurnOn2DLaser[]  = { 0x02,0x31, 0x03 };		//STX + "1" + ETX   is turn on 
	char scTurnOff2DLaser[] = { 0x02,0x32, 0x03 };		//STX + "2" + ETX   is turn off 
	char scTurnOnLaser[]  = { 0x02, 0x53, 0x54, 0x49, 0x45, 0x03, 0x08 };
	char scReadData[50];
	char scOutputData[50];
	char scTempData[50];
	short i = 0;
	short j = 0;


	//Init variable
	szData->Empty();
	memset(scReadData, NULL, sizeof(scReadData));
	memset(scOutputData, NULL, sizeof(scOutputData));
	memset(scTempData, NULL, sizeof(scTempData));


	//Check COM is created
	if (m_oCommPort.IsCreated() != TRUE)
	{
		return FALSE;
	}

	//Open COM & Check 
	m_oCommPort.SetReadConstant((DWORD)nTimeOut);
	m_oCommPort.Open();
	if (m_oCommPort.IsOpen() == TRUE)
	{
		m_oCommPort.Purge();
	}
	else
	{
		return FALSE;
	}


	try
	{
		if(bOnOff2DBarCodeScanner)
		{
			//Request Barcode off laser first
			m_oCommPort.Write(scTurnOff2DLaser, 3);
			m_oCommPort.Purge();

			//Request Barcode On laser
			m_oCommPort.Write(scTurnOn2DLaser, 3);
			m_oCommPort.Purge();
		}
		else
		{
			//Request Barcode On laser
		     m_oCommPort.Write(scTurnOnLaser, 7);
		     m_oCommPort.Purge();

		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	INT nCount=0;
	INT nLen=0;

	while(1)
	{
		try
		{
			//Read data
			m_oCommPort.Read(scReadData, 50);

			//Append data into temp.
			strcat_s(scTempData, sizeof(scTempData), scReadData);

			//check data end character
			nLen = (INT)strlen(scReadData) - 1;
			nLen = max(nLen, 0);	//Klocwork		//v4.02T5

			if ( scReadData[nLen] == '\n' )
			{
				//Copy temp data into ReadData
				strcpy_s(scReadData, sizeof(scReadData), scTempData);
				break;
			}
			else
			{
				memset(scReadData, NULL, sizeof(scReadData));
			}
		}
		catch (CAsmException e)
		{
			//No need to display exception
		}

		nCount++;
		if ( nCount >=nRetry )
		{
			break;
		}
	}

	try
	{
		if(bOnOff2DBarCodeScanner)
		{
			//Request Barcode off laser again
			m_oCommPort.Write(scTurnOff2DLaser, 3);
			m_oCommPort.Purge();
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	m_oCommPort.Close();

	//Decode data
	for (i = 0; i<50; i++)
	{
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}

	CString szBefore = *szData;
	szData->Format("%s",scOutputData);

	if( pApp->GetCustomerName()=="HuaLei" )
	{
		*szData = szData->MakeUpper();
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(ReadBarcode): " + szBefore + " --> " + *szData);

	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		return GetWaferIDInXML(szData);	// yealy: get wid by binframeID
	}

    if( szData->IsEmpty() )
	{
        return FALSE;
	}

	return TRUE;
}

BOOL CWaferLoader::StartBarcode(BOOL bStart)
{
	char scTurnOnLaser[]  = { 0x02, 0x4C, 0x4F, 0x4E, 0x03 };		//"STX" + "LON" + "ETX"
	char scTurnOffLaser[] = { 0x02, 0x4C, 0x4F, 0x46, 0x46, 0x03 };	//"STX" + "LOFF" + "ETX"

	//Check COM is created
	if (m_oCommPort.IsCreated() != TRUE)
	{
		return FALSE;
	}

	if ( bStart == TRUE )
	{
		//Open COM & Check 
		m_oCommPort.Open();
		if (m_oCommPort.IsOpen() == TRUE)
		{
			m_oCommPort.Purge();
		}
		else
		{
			return FALSE;
		}
	}

	//Request Barcode On laser
	try
	{
		if ( bStart == TRUE )
		{
			m_oCommPort.SetTimeOuts(5, 200);
			m_oCommPort.Write(scTurnOnLaser, 5);
		}
		else
		{
			m_oCommPort.Write(scTurnOffLaser, 6);
			m_oCommPort.Close();
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}

BOOL CWaferLoader::ReadBarcode_Keyence(CString *szData)
{
	char scReadData[50];
	char scOutputData[50];
	short i = 0;
	short j = 0;

	//Init variable
	memset(scReadData, NULL, sizeof(scReadData));
	memset(scOutputData, NULL, sizeof(scOutputData));

	//Check COM is created
	if (m_oCommPort.IsCreated() != TRUE)
	{
		szData->Empty();
		return FALSE;
	}

	//Read the Barcode value
	try 
	{
		m_oCommPort.Read(scReadData,50);
	} 
	catch (CAsmException e)
	{
		//No need to display exception
	}	

	//Decode data
	for (i = 0; i<50; i++)
	{
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}


	CString szBefore = *szData;
	szData->Format("%s",scOutputData);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		*szData = szData->MakeUpper();
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(ReadBarcode Keyence): " + szBefore + " --> " + *szData);

	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		return GetWaferIDInXML(szData);	// yealy: get wid by binframeID
	}

	return TRUE;
}

BOOL CWaferLoader::ReadBarcode_DefaultCom(CString *szData, int nTimeOut, int nRetry)
{
	//char scTurnOnLaser[]  = { 0x02, 0x53, 0x54, 0x49, 0x45, 0x03, 0x08 };
	char scReadData[50];
	char scOutputData[50];
	char scTempData[50];
	short i = 0;
	short j = 0;


	//Init variable
	memset(scReadData,		NULL, sizeof(scReadData));
	memset(scOutputData,	NULL, sizeof(scOutputData));
	memset(scTempData,		NULL, sizeof(scTempData));


	//Check COM is created
	if (m_oCommPort.IsCreated() != TRUE)
	{
		szData->Empty();
		return FALSE;
	}

	//Open COM & Check 
	m_oCommPort.Open();
	if (m_oCommPort.IsOpen() == TRUE)
	{
		m_oCommPort.Purge();
	}
	else
	{
		szData->Empty();
		return FALSE;
	}


	INT nCount=0;
	INT nLen=0;
	m_oCommPort.SetReadConstant((DWORD)nTimeOut);

	while(1)
	{
		try
		{
			//Read data
			m_oCommPort.Read(scReadData, 50);

			//Append data into temp.
			strcat_s(scTempData, sizeof(scTempData), scReadData);

			//check data end character
			nLen = (INT)strlen(scReadData) - 1;
			nLen = max(nLen, 0);	//Klocwork		//v4.02T5

			if ( scReadData[nLen] == '\n' )
			{
				//Copy temp data into ReadData
				strcpy_s(scReadData, sizeof(scReadData), scTempData);
				break;
			}
			else
			{
				memset(scReadData, NULL, sizeof(scReadData));
			}
		}
		catch (CAsmException e)
		{
			//No need to display exception
		}

		nCount++;
		if ( nCount >=nRetry )
		{
			break;
		}
	}
	m_oCommPort.Close();

	//Decode data
	for (i = 0; i<50; i++)
	{
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}

	CString szBefore = *szData;
	szData->Format("%s",scOutputData);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		*szData = szData->MakeUpper();
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(ReadBarcode_defaultcom): " + szBefore + " --> " + *szData);


	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		return GetWaferIDInXML(szData);	// yealy: get wid by binframeID
	}

	return TRUE;
}

BOOL CWaferLoader::StartBarcode_Regal(BOOL bStart)
{
	if (m_oCommPort.IsCreated() != TRUE)
	{
		return FALSE;
	}

	if ( bStart == TRUE )
	{
		//Open COM & Check 
		m_oCommPort.Open();
		if (m_oCommPort.IsOpen() == TRUE)
		{
			m_oCommPort.Purge();
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		m_oCommPort.Close();
	}

	return TRUE;
}

BOOL CWaferLoader::PurgeBarcode_Regal()
{
	if (m_oCommPort.IsOpen() == TRUE)
	{
		m_oCommPort.Purge();
	}
	
	return TRUE;
}

BOOL CWaferLoader::ReadBarcode_Regal(CString *szData, int nRetry)
{
	char scReadData[50];
	char scOutputData[50];
	char scTempData[50];
	short i = 0;
	short j = 0;
	int nLen = 0 , nCount = 0;


	//Init variable
	memset(scReadData, NULL, sizeof(scReadData));
	memset(scOutputData, NULL, sizeof(scOutputData));
	memset(scTempData, NULL, sizeof(scTempData));


	//Check COM is created
	if (m_oCommPort.IsCreated() != TRUE)
	{
		szData->Empty();
		return FALSE;
	}

	m_oCommPort.SetReadConstant((DWORD)100);

	while(1)
	{
		try
		{
			//Read data
			m_oCommPort.Read(scReadData,50);

			//Append data into temp.
			strcat_s(scTempData, sizeof(scTempData), scReadData);

			//check data end character
			nLen = (INT)strlen(scReadData) - 1;
			nLen = max(nLen, 0);

			if ( scReadData[nLen] == '\r' )
			{
				//Copy temp data into ReadData
				strcpy_s(scReadData, sizeof(scReadData), scTempData);
				break;
			}
			else
			{
				memset(scReadData, NULL, sizeof(scReadData));
			}
		}
		catch (CAsmException e)
		{
			//No need to display exception
		}

		nCount++;
		if ( nCount >=nRetry )
		{
			break;
		}
	}

	//Decode data
	for (i = 0; i<50; i++)
	{
		if (scReadData[i] == '\r')
		{
			break;
		}
		
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}

	szData->Format("%s",scOutputData);

	return TRUE;
}

CString CWaferLoader::GetAppKeyboard()
{
	CString szBarcode = CMS896AApp::m_szKeyboard;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		szBarcode = szBarcode.MakeUpper();
	}

	return szBarcode;
}

INT CWaferLoader::Scan2DBarCode()
{
	CMSLogFileUtility::Instance()->WL_LogStatus("Start 2D barcode scan...");

	m_szBarcodeName.Empty();
	(*m_psmfSRam)["PSPR"]["2DBarCode"] = _T("");	//pllm
	
	//Move Gripper to Scan bar code position
	X_MoveTo(m_l2DBarcodePos_X);
	INT i = 0;

	BOOL bManual = FALSE;
	BOOL bResult = FALSE;
	IPC_CServiceMessage stMsg;
	stMsg.InitMessage(sizeof(BOOL), &bManual);
	int	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "Search2DBarCode", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 360000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bResult);
			break;
		}
		else
		{
			Sleep(1);		
		}
	}
	
	if (bResult)
	{
		m_szBarcodeName = (*m_psmfSRam)["PSPR"]["2DBarCode"];
		SaveBarcodeName();
		return Err_No_Error;
	}
	
	Sleep(100);
	i++;
	
	return ERR_READ_BC_FAILED;	//pllm
}

//4.52D10fnc 2nd Scan
INT CWaferLoader::Scan2nd1DBarCode(BOOL bAllowSearch, BOOL bIsCompareBarcode)  
{
	CString szMsg;
	CString szBarcode;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	//Move Gripper to Scan bar code position
	X_MoveTo(m_l2DBarcodePos_X);
	szMsg.Format("WaferLabel -- Start 2nd 1D barcode scan, Move:%ld", m_l2DBarcodePos_X);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

	INT nStatus = Err_No_Error;
	if ( GetBCModel() == WL_BAR_SYMBOL || GetBCModel() == WL_BAR_DATALOGIC)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Scan by symbol ....");
		nStatus = ScanBySymbolWith2ndPosn(bAllowSearch, bIsCompareBarcode);

	}
	else if (GetBCModel() == WL_BAR_DEFAULT)		//WL_BAR_REGAL)
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel --Scan by default ....");
		nStatus = ScanByDefault();

		m_sz2nd1DBarcodeName.Replace("\n", "");
		m_sz2nd1DBarcodeName.Replace("\r", "");

		CString szResult;
		szResult.Format("WaferLabel --ScanByDefault WT1 result = %d; BC = %s (%s)", nStatus, m_sz2nd1DBarcodeName, GetAppKeyboard());
		CMSLogFileUtility::Instance()->WL_LogStatus(szResult);		
	}
	else if (GetBCModel() == WL_BAR_DEFAULT_COM)	//v4.40T10
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel --Scan by default COM ....");
		nStatus = ScanByDefaultCom();

		m_sz2nd1DBarcodeName.Replace("\n", "");
		m_sz2nd1DBarcodeName.Replace("\r", "");

		CString szResult;
		szResult.Format("WaferLabel --Scan ByDefaultCom WT1 result = %d; BC:%s", nStatus, m_sz2nd1DBarcodeName);
		CMSLogFileUtility::Instance()->WL_LogStatus(szResult);		
	}
	else
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel --Scan by other ....");
		nStatus = ScanByKeyence(bAllowSearch, bIsCompareBarcode);
	}

	//v4.06
	//Manual-input wafer barcode from keyboard or hand-held BC scanner
	if ((nStatus != Err_No_Error) && pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_MANUAL_INPUT_BC))
	{
		CString szBarcode	= _T("");
		nStatus				= WL_InputBarcode(1, szBarcode);
		if (nStatus == Err_No_Error)
		{
			m_sz2nd1DBarcodeName = szBarcode;
		}
	}
	
	return Err_No_Error;
}


BOOL CWaferLoader::CreateBarcodeScanner2(VOID)
{
	BOOL bIsCreated = FALSE;

	CString szLog;
	szLog.Format("WL2: Create Barcode Model: %d",GetBCModel());
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (!m_oCommPort2.IsCreated())	//v4.40T12
	{
		//Create COM port for barcode scanning (Port No get from Machine)
		if ( (GetBCModel() == WL_BAR_DEFAULT) || (GetBCModel() == WL_BAR_DEFAULT_COM) )	//v4.40T10
		{
			//New DEFAULT_COM com port settings for SanAn
			m_oCommPort2.Create((DWORD)CMS896AApp::m_lBarCode4ComPort, 9600, 8, SFM_PARITY_NONE, 1, 5, 2000);
		}
		else
		{
			//Default SYMBOL/DATALOGIC com port settings
			m_oCommPort2.Create((DWORD)CMS896AApp::m_lBarCode4ComPort, 9600, 7, SFM_PARITY_EVEN, 2, 5, 2000);
		}
	}

	bIsCreated = m_oCommPort2.IsCreated();
	return TRUE;
}

BOOL CWaferLoader::ReadBarcode2(CString *szData, int nTimeOut, int nRetry)
{

	char scTurnOnLaser[]  = { 0x02, 0x53, 0x54, 0x49, 0x45, 0x03, 0x08 };
	char scReadData[50];
	char scOutputData[50];
	char scTempData[50];
	short i = 0;
	short j = 0;


	//Init variable
	memset(scReadData, NULL, sizeof(scReadData));
	memset(scOutputData, NULL, sizeof(scOutputData));
	memset(scTempData, NULL, sizeof(scTempData));


	//Check COM is created
	if (m_oCommPort2.IsCreated() != TRUE)
	{
		szData->Empty();
		return FALSE;
	}

	//Open COM & Check 
	m_oCommPort2.Open();
	if (m_oCommPort2.IsOpen() == TRUE)
	{
		m_oCommPort2.Purge();
	}
	else
	{
		szData->Empty();
		return FALSE;
	}

	try
	{
		//Request Barcode On laser
		m_oCommPort2.Write(scTurnOnLaser, 7);
		m_oCommPort2.Purge();
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	INT nCount=0;
	INT nLen=0;

	m_oCommPort2.SetReadConstant((DWORD)nTimeOut);

	while(1)
	{
		try
		{
			//Read data
			m_oCommPort2.Read(scReadData,50);

			//Append data into temp.
			strcat_s(scTempData, sizeof(scTempData), scReadData);

			//check data end character
			nLen = (INT)strlen(scReadData) - 1;
			nLen = max(nLen, 0);	//Klocwork		//v4.02T5

			if ( scReadData[nLen] == '\n' )
			{
				//Copy temp data into ReadData
				strcpy_s(scReadData, sizeof(scReadData), scTempData);

				break;
			}
			else
			{
				memset(scReadData, NULL, sizeof(scReadData));
			}
		}
		catch (CAsmException e)
		{
			//No need to display exception
		}

		nCount++;
		if ( nCount >=nRetry )
		{
			break;
		}
	}
	m_oCommPort2.Close();

	//Decode data
	for (i = 0; i<50; i++)
	{
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}

	CString szBefore = *szData;
	szData->Format("%s",scOutputData);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		*szData = szData->MakeUpper();
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(ReadBarcode2): " + szBefore + " --> " + *szData);

	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		return GetWaferIDInXML(szData);	// yealy: get wid by binframeID
	}

	return TRUE;
}

BOOL CWaferLoader::ReadBarcode2_DefaultCom(CString *szData, int nTimeOut, int nRetry)
{
	// leo 20130723 char scTurnOnLaser[]  = { 0x02, 0x53, 0x54, 0x49, 0x45, 0x03, 0x08 };
	char scReadData[500];
	char scOutputData[500];
	char scTempData[500];
	short i = 0;
	short j = 0;
	
	CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(Read Barcode2_DefaultCom): begins");
	//Init variable
	memset(scReadData,		NULL, sizeof(scReadData));
	memset(scOutputData,	NULL, sizeof(scOutputData));
	memset(scTempData,		NULL, sizeof(scTempData));

	//Check COM is created
	if (m_oCommPort2.IsCreated() != TRUE)
	{
		szData->Empty();
		CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(Read Barcode2_DefaultCom): comport2 not created");
		return FALSE;
	}


	//Open COM & Check 
	m_oCommPort2.Open();
	if (m_oCommPort2.IsOpen() == TRUE)
	{
		m_oCommPort2.Purge();
	}
	else
	{
		szData->Empty();
		CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(Read Barcode2_DefaultCom): comport2 not open");
		return FALSE;
	}

	INT nCount=0;
	INT nLen=0;
	m_oCommPort2.SetReadConstant((DWORD)nTimeOut);

	while(1)
	{
		try
		{
			//Read data
			m_oCommPort2.Read(scReadData,500);
			Sleep(100);
			//Append data into temp.
			strcat_s(scTempData, sizeof(scTempData), scReadData);
			//check data end character
			nLen = (INT)strlen(scReadData)-1;
			nLen = max(nLen, 0);	//Klocwork		//v4.02T5

			if ( scReadData[nLen] == '\n' )
			{
				//Copy temp data into ReadData
				strcpy_s(scReadData, sizeof(scReadData), scTempData);
				break;
			}
			else
			{
				memset(scReadData, NULL, sizeof(scReadData));
			}
		}
		catch (CAsmException e)
		{
			//No need to display exception
		}

		nCount++;
		if ( nCount >=nRetry )
		{
			break;
		}
	}
	
	m_oCommPort2.Purge();//leo 20130723
	m_oCommPort2.Close();

	//Decode data
	for (i = 0; i<500; i++)
	{
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}

	CString szBefore = *szData;
	szData->Format("%s",scOutputData);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		*szData = szData->MakeUpper();
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(Read Barcode2_DefaultCom): " + szBefore + " --> " + *szData);

	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		return GetWaferIDInXML(szData);	// yealy: get wid by binframeID
	}

	return TRUE;
}


BOOL CWaferLoader::ReadBarcodeValue2(CString *szData)
{
	char scReadData[50];
	char scOutputData[50];
	short i = 0;
	short j = 0;

	//Init variable
	memset(scReadData, NULL, sizeof(scReadData));
	memset(scOutputData, NULL, sizeof(scOutputData));

	//Check COM is created
	if (m_oCommPort2.IsCreated() != TRUE)
	{
		szData->Empty();
		return FALSE;
	}

	//Read the Barcode value
	try 
	{
		m_oCommPort2.Read(scReadData,50);
	} 
	catch (CAsmException e)
	{
		//No need to display exception
	}	

	//Decode data
	for (i = 0; i<50; i++)
	{
		if ((scReadData[i] >= 0x20) && (scReadData[i] <= 0x7E))
		{
			scOutputData[j++] = scReadData[i];
		}
	}

	CString szBefore = *szData;
	szData->Format("%s",scOutputData);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="HuaLei" )
	{
		*szData = szData->MakeUpper();
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("Barcode scanned(ReadBarcode2 Value): " + szBefore + " --> " + *szData);

	if (pApp->GetCustomerName() == "Yealy" && IsAOIOnlyMachine())
	{
		return GetWaferIDInXML(szData);	// yealy: get wid by binframeID
	}

	return TRUE;
}


INT CWaferLoader::ScanningBarcode(BOOL bAllowSearch, BOOL bIsCompareBarcode, BOOL bIsCheckBarcodeFucEnabled)
{
	//v2.83T61		//v3.70T2	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	BOOL b2DBarCode			= pApp->GetFeatureStatus(MS896A_FUNC_2D_BARCODE_CHECK);
	BOOL bPLLMSpecialFcn	= pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
	BOOL bPostSealOptics	= pApp->GetFeatureStatus(MS896A_FUNC_VISION_POSTSEAL_OPTICS);

	BOOL b2nd1DBarCode		= pApp->GetFeatureStatus(MS896A_FUNC_2ND_1D_BARCODE_CHECK);

	CString szMsg;
	szMsg.Format("WaferLabel -- Scanning State(%d,%d,%d,%d)",b2DBarCode,bPLLMSpecialFcn,bPostSealOptics,b2nd1DBarCode);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
	m_sz2nd1DBarcodeName = _T("UnScan");

	//v4.46T10	//Knowles
	if (b2DBarCode && bPostSealOptics && m_bEnable2DBarcodeCheck && !b2nd1DBarCode)
	{
		//2D BC position must be smaller than BC position in order to activate this fcn
		if ((m_l2DBarcodePos_X < m_lUnloadPos_X) && (m_l2DBarcodePos_X > m_lLoadPos_X))	//v4.46T7	//Knowles
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Scanning PLLM 2D barcode ...");
			if (Scan2DBarCode() == Err_No_Error)
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Scanning 2D barcode OK BC = " + m_szBarcodeName);		//av2.83T63
				(*m_psmfSRam)["WaferLoaderStn"]["2D Barcode"] = TRUE;
				return Err_No_Error;
			}
			else
			{
				(*m_psmfSRam)["WaferLoaderStn"]["2D Barcode"] = FALSE;
				if (!bPLLMSpecialFcn)
				{
					CMSLogFileUtility::Instance()->WL_LogStatus("Scanning 2D barcode fail");
					return ERR_READ_BC_FAILED;
				}
				
				CMSLogFileUtility::Instance()->WL_LogStatus("Scanning 2D barcode fail; proceed to barcode scanner scanning ...");		//av2.83T63
			}
		}
		else
		{
			CString szLog;
			szLog.Format("Scanning 2D barcode with invalid gripper pos %ld (%ld, %ld)",  
				m_l2DBarcodePos_X, m_lLoadPos_X, m_lUnloadPos_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLog);

			if (!bPLLMSpecialFcn)	//v4.46T10
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Scanning 2D barcode fail");
				return ERR_READ_BC_FAILED;
			}
		}
	}


	//4.52D10Run scannning 2nd 1D barcode
	if (b2nd1DBarCode)
	{
		//2D BC position must be smaller than BC position in order to activate this fcn
		if ((m_l2DBarcodePos_X < m_lUnloadPos_X) && (m_l2DBarcodePos_X > m_lLoadPos_X))
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Scanning PLLM 2nd1D barcode ...");
			if (Scan2nd1DBarCode(bAllowSearch,bIsCompareBarcode) == Err_No_Error)
			{
				szMsg = "WaferLabel --  BC = " + m_sz2nd1DBarcodeName;
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);		

				(*m_psmfSRam)["WaferLoaderStn"]["2nd 1D Barcode"] = m_sz2nd1DBarcodeName; //Ring ID / Frame ID
			}
			else
			{
				(*m_psmfSRam)["WaferLoaderStn"]["2nd 1D Barcode"] = "Fail Scan";

				szMsg = "WaferLabel --  scanning 2nd 1D barcode fail; proceed to wafer barcode scanner scanning ...";
				CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);		

				HmiMessage(szMsg);
			}
		}
		else
		{
			
			szMsg.Format("WaferLabel -- Scanning 2nd 1D barcode with invalid gripper pos: %ld, (Load: %ld, UnLoad: %ld)",  
				m_l2DBarcodePos_X, m_lLoadPos_X, m_lUnloadPos_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);

			HmiMessage(szMsg);
		}
	}



	INT nStatus = Err_No_Error;
	if ( GetBCModel() == WL_BAR_SYMBOL || GetBCModel() == WL_BAR_DATALOGIC )
	{
		nStatus = ScanBySymbol(bAllowSearch, bIsCompareBarcode);
//HmiMessage("Scan by symbol ....");
	}
	else if (GetBCModel() == WL_BAR_DEFAULT)		//WL_BAR_REGAL)
	{
//HmiMessage("Scan by default ....");
		nStatus = ScanByDefault();

		m_szBarcodeName.Replace("\n", "");
		m_szBarcodeName.Replace("\r", "");

		CString szResult;
		szResult.Format("ScanByDefault WT1 result = %d; BC = %s (%s)", nStatus, m_szBarcodeName, GetAppKeyboard());
		CMSLogFileUtility::Instance()->WL_LogStatus(szResult);		
		//SetErrorMessage(szResult);
		//HmiMessage(szResult);
	}
	else if (GetBCModel() == WL_BAR_DEFAULT_COM)	//v4.40T10
	{
//HmiMessage("Scan by default COM ....");
		nStatus = ScanByDefaultCom();

		m_szBarcodeName.Replace("\n", "");
		m_szBarcodeName.Replace("\r", "");

		CString szResult;
		szResult.Format("Scan ByDefaultCom WT1 result = %d; BC:%s", nStatus, m_szBarcodeName);
		CMSLogFileUtility::Instance()->WL_LogStatus(szResult);		
	}
	else
	{
		nStatus = ScanByKeyence(bAllowSearch, bIsCompareBarcode);
//HmiMessage("Scan by other ....");

	}

	//v4.06
	//Manual-input wafer barcode from keyboard or hand-held BC scanner
	if ((nStatus != Err_No_Error) && pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_MANUAL_INPUT_BC))
	{
		CString szBarcode	= _T("");
		nStatus				= WL_InputBarcode(1, szBarcode);
		if (nStatus == Err_No_Error)
		{
			m_szBarcodeName = szBarcode;
			SaveBarcodeName();
		}
	}

	return nStatus;
}

INT CWaferLoader::ScanningBarcode2(BOOL bAllowSearch, BOOL bIsCompareBarcode, BOOL bIsCheckBarcodeFucEnabled)
{
	INT nStatus = Err_No_Error;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if ( (GetBCModel() == WL_BAR_SYMBOL) || GetBCModel() == WL_BAR_DATALOGIC )	// || (GetBCModel() == WL_BAR_DEFAULT) )	//v4.38T2
	{
		nStatus = ScanBySymbol2(bAllowSearch, bIsCompareBarcode);
	}
	else if (GetBCModel() == WL_BAR_DEFAULT)		//v4.40T11
	{
		nStatus = ScanByDefault2Com();		//v4.40T10

		m_szBarcodeName2.Replace("\n", "");
		m_szBarcodeName2.Replace("\r", "");

		CString szResult;
		szResult.Format("ScanByDefault WT2 result = %d; BC:%s (%s)", nStatus, m_szBarcodeName2, GetAppKeyboard());
		CMSLogFileUtility::Instance()->WL_LogStatus(szResult);
	}
	else if (GetBCModel() == WL_BAR_DEFAULT_COM)	//v4.40T11
	{
		nStatus = ScanByDefault2Com();

		m_szBarcodeName2.Replace("\n", "");
		m_szBarcodeName2.Replace("\r", "");

		CString szResult;
		szResult.Format("ScanBy DefaultCom WT2 result = %d; BC:%s", nStatus, m_szBarcodeName2);
		CMSLogFileUtility::Instance()->WL_LogStatus(szResult);
	}

	//v4.06
	//Manual-input wafer barcode from keyboard or hand-held BC scanner
	if ((nStatus != Err_No_Error) && pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_MANUAL_INPUT_BC))
	{
		CString szBarcode	= _T("");
		nStatus = WL_InputBarcode(2, szBarcode);
		if (nStatus == Err_No_Error)
		{
			m_szBarcodeName2 = szBarcode;
		}
	}

	return nStatus;
}

INT CWaferLoader::ScanningBarcodeOnTable(BOOL bAllowSearch, BOOL bHomeTable)
{
	short i = 0;
	
	m_szBarcodeName.Empty();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName()=="SanAn" && pApp->GetProductLine()=="WH" )
	{
		for (i=0; i< 5; i++)
		{
			ReadStaticBarcode(m_szBarcodeName);
			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}
			Sleep(1000);
		}
	}

	ReadStaticBarcode(m_szBarcodeName);

	if (m_lScanRange <= 0 || bAllowSearch == FALSE)
	{
		if (m_szBarcodeName.IsEmpty())
		{
			return ERR_READ_BC_FAILED;
		}
		else
		{
			if( CheckBarcodeLength(m_szBarcodeName) )
			{
				return Err_No_Error;
			}

			m_szBarcodeName.Empty();

			return ERR_READ_BC_FAILED;
		}
	}

	if (m_szBarcodeName.IsEmpty() == TRUE)
	{	
		CString szErrMsg = "Cannot Move the Wafer Table/n";

		if (bHomeTable)	//v4.48A15
			MoveWaferTable(m_lScanRange*10/2, 0);	//v2.93T2

		for (i=0; i< 2; i++)
		{  
			//-------------- Searching in X Direction Only --------------//
			
			if (MoveWaferTableForBarCode(0,HP_POSITIVE_DIR,m_lScanRange*10,FALSE) == FALSE)		//v2.93T2
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}

			//ReadBarcode(&m_szBarcodeName, 100, 4);		//v2.93T2
			ReadStaticBarcode(m_szBarcodeName);
			SyncWaferTableXYT("X");

			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}

#ifdef NU_MOTION
			if (MoveWaferTableForBarCode(0,HP_NEGATIVE_DIR,	-1*m_lScanRange*10, FALSE) == FALSE)		//v4.40T14
#else
			if (MoveWaferTableForBarCode(0,HP_NEGATIVE_DIR,	m_lScanRange*10,	FALSE) == FALSE)		//v2.93T2
#endif			
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}

			//ReadBarcode(&m_szBarcodeName, 100, 4);		//v2.93T2
			ReadStaticBarcode(m_szBarcodeName);
			SyncWaferTableXYT("X");
			
			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}

			//-------------- End of Searching in X Direction Only --------------//
		}

		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}

INT CWaferLoader::ScanByDefaultCom()
{
	short i				= 0;
	CString szTitle		= _T("Please input wafer barcode");
	CString szBarcode	= _T("");
	CString szLogMsg;

	m_szBarcodeName.Empty();
	
	X_MoveTo(m_lUnloadPos_X, SFM_NOWAIT);
	ReadBarcode_DefaultCom(&szBarcode);
	X_Sync();
	GetEncoderValue();

	if (szBarcode.GetLength() > 0)
	{
		szLogMsg.Format("1st read is %s", szBarcode);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		if( CheckBarcodeLength(szBarcode) )
		{
			m_szBarcodeName = szBarcode;
			SaveBarcodeName();
			szLogMsg.Format("WL gripper read barcode 1 is %s, on X %ld", 
				m_szBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			return Err_No_Error;
		}
		else
		{
			szBarcode.Empty();
		}
	}


	X_MoveTo(m_lBarcodePos_X, SFM_WAIT);

	if (szBarcode.IsEmpty() == FALSE)
	{
		return Err_No_Error;
	}

	INT nStatus = Err_No_Error;
	for (i=0; i<2; i++)
	{
		//1. First time
		nStatus = FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
		ReadBarcode_DefaultCom(&szBarcode);
		X_Sync();
		GetEncoderValue();

		if ( nStatus != Err_No_Error )
		{
			//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
			SetErrorMessage("Gripper is Jam @ scan barcode");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			return Err_FrameJammed;
		}
	
		if (szBarcode.IsEmpty() == FALSE)
		{
			szLogMsg.Format("move search1 read is %s", szBarcode);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			if( CheckBarcodeLength(szBarcode) )
			{
				m_szBarcodeName = szBarcode;
				SaveBarcodeName();
				szLogMsg.Format("WL gripper read barcode 3 is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				return Err_No_Error;
			}
			else
			{
				szBarcode.Empty();
			}
		}

		//2. Second time
		nStatus = FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE);
		ReadBarcode_DefaultCom(&szBarcode);
		X_Sync();
		GetEncoderValue();

		if ( nStatus != Err_No_Error )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
			SetErrorMessage("Gripper is Jam @ scan barcode");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			return Err_FrameJammed;
		}

		if (szBarcode.IsEmpty() == FALSE)
		{
			szLogMsg.Format("move search2 read is %s", szBarcode);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			if( CheckBarcodeLength(szBarcode) )
			{
				m_szBarcodeName = szBarcode;
				SaveBarcodeName();
				szLogMsg.Format("WL gripper read barcode 5 is %s, on X %ld", 
					m_szBarcodeName, m_lEnc_X);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				return Err_No_Error;
			}
			else
			{
				szBarcode.Empty();
			}
		}

		//3. Third time
		nStatus = FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
		ReadBarcode_DefaultCom(&szBarcode);
		X_Sync();
		GetEncoderValue();

		if ( nStatus != Err_No_Error )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
			SetErrorMessage("Gripper is Jam @ scan barcode");
			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			return Err_FrameJammed;
		}

		if (szBarcode.IsEmpty() == FALSE)
		{
			szLogMsg.Format("move search3 read is %s", szBarcode);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			if( CheckBarcodeLength(szBarcode) )
			{
				m_szBarcodeName = szBarcode;
				SaveBarcodeName();
				szLogMsg.Format("WL gripper read barcode 7 is %s, on X %ld", 
					m_szBarcodeName, m_lEnc_X);
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				return Err_No_Error;
			}
			else
			{
				szBarcode.Empty();
			}
		}
	}

	return ERR_READ_BC_FAILED;
}

INT CWaferLoader::ScanByDefault2Com()
{
	short i				= 0;
	CString szTitle		= _T("Please input wafer barcode");
	CString szBarcode	= _T("");
	CString szLogMsg;

	m_szBarcodeName2.Empty();
	
	X2_MoveTo(m_lUnloadPos_X2, SFM_NOWAIT);
	ReadBarcode2_DefaultCom(&szBarcode);
	X2_Sync();
	GetEncoderValue();

	if (szBarcode.GetLength() > 0)
	{
		if( CheckBarcodeLength(szBarcode) )
		{
			m_szBarcodeName2 = szBarcode;
			szLogMsg.Format("WL2 gripper default2com 1 is %s, on X %ld", szBarcode, m_lEnc_X2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			return Err_No_Error;
		}
		else
		{
			szBarcode.Empty();
		}
	}

	X2_MoveTo(m_lBarcodePos_X2, SFM_WAIT);

	INT nStatus = Err_No_Error;
	if (szBarcode.IsEmpty() == TRUE)
	{
		for (i=0; i<1; i++)
		{
			//1. First time
			nStatus = FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
			ReadBarcode2_DefaultCom(&szBarcode);
			X2_Sync();
			GetEncoderValue();

			if ( nStatus != Err_No_Error )
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ scan barcode");
				SetErrorMessage("Gripper2 is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}
		
			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName2 = szBarcode;
					szLogMsg.Format("WL2 gripper default2com 3 is %s, on X %ld", szBarcode, m_lEnc_X2);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				else
				{
					szBarcode.Empty();
				}
			}

			//2. Second time
			nStatus = FilmFrame2DriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE);
			ReadBarcode2_DefaultCom(&szBarcode);
			X2_Sync();
			GetEncoderValue();

			if ( nStatus != Err_No_Error )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ scan barcode");
				SetErrorMessage("Gripper2 is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName2 = szBarcode;
					szLogMsg.Format("WL2 gripper default2com 5 is %s, on X %ld", szBarcode, m_lEnc_X2);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				else
				{
					szBarcode.Empty();
				}
			}

			//3. Third time
			nStatus = FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
			ReadBarcode2_DefaultCom(&szBarcode);
			X2_Sync();
			GetEncoderValue();

			if ( nStatus != Err_No_Error )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ scan barcode");
				SetErrorMessage("Gripper2 is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName2 = szBarcode;
					szLogMsg.Format("WL2 gripper default2com 7 is %s, on X %ld", szBarcode, m_lEnc_X2);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				else
				{
					szBarcode.Empty();
				}
			}
		}
		return ERR_READ_BC_FAILED;
	}
	return Err_No_Error;
}


INT CWaferLoader::ScanByDefault()
{
	short i				= 0;
	CString szTitle		= _T("Please input wafer barcode");
	CString szBarcode	= _T("");
	CString szLogMsg;

	m_szBarcodeName.Empty();
	
	EnableKeyboardHook(TRUE);
	X_MoveTo(m_lUnloadPos_X, SFM_NOWAIT);
	X_Sync();
	Sleep(100);
	EnableKeyboardHook(FALSE);
	GetEncoderValue();
	szBarcode = GetAppKeyboard();

	if (szBarcode.GetLength() > 0)
	{
		if( CheckBarcodeLength(szBarcode) )
		{
			m_szBarcodeName = szBarcode;
			SaveBarcodeName();
			szLogMsg.Format("WL gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			return Err_No_Error;
		}
		else
		{
			szBarcode.Empty();
		}
	}


	X_MoveTo(m_lBarcodePos_X, SFM_WAIT);

	INT nStatus = Err_No_Error;
	if (szBarcode.IsEmpty() == TRUE)
	{
		for (i=0; i<2; i++)
		{
			//1. First time
			EnableKeyboardHook(TRUE);
			nStatus = FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
			X_Sync();
			Sleep(100);
			EnableKeyboardHook(FALSE);
			GetEncoderValue();
			szBarcode = GetAppKeyboard();

			if ( nStatus != Err_No_Error )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}
		
			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName = szBarcode;
					SaveBarcodeName();
					szLogMsg.Format("WL gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				szBarcode.Empty();
			}

			//2. Second time
			EnableKeyboardHook(TRUE);
			nStatus = FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE);
			X_Sync();
			Sleep(100);
			EnableKeyboardHook(FALSE);
			GetEncoderValue();
			szBarcode = GetAppKeyboard();

			if ( nStatus != Err_No_Error )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName = szBarcode;
					SaveBarcodeName();
					szLogMsg.Format("WL gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				szBarcode.Empty();
			}

			//3. Third time
			EnableKeyboardHook(TRUE);
			nStatus = FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
			X_Sync();
			Sleep(100);
			EnableKeyboardHook(FALSE);
			GetEncoderValue();
			szBarcode = GetAppKeyboard();

			if ( nStatus != Err_No_Error )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName = szBarcode;
					SaveBarcodeName();
					szLogMsg.Format("WL gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				szBarcode.Empty();
			}
		}
		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}

INT CWaferLoader::ScanByDefault2()
{
	short i				= 0;
	CString szTitle		= _T("Please input wafer barcode");
	CString szBarcode	= _T("");
	CString szLogMsg;

	m_szBarcodeName2.Empty();
	
	EnableKeyboardHook(TRUE);
	X2_MoveTo(m_lUnloadPos_X2, SFM_NOWAIT);
	X2_Sync();
	Sleep(100);
	EnableKeyboardHook(FALSE);
	GetEncoderValue();
	szBarcode = GetAppKeyboard();

	if (szBarcode.GetLength() > 0)
	{
		if( CheckBarcodeLength(szBarcode) )
		{
			m_szBarcodeName2 = szBarcode;
			szLogMsg.Format("WL2 gripper default is %s, on X %ld", szBarcode, m_lEnc_X2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			return TRUE;
		}

		szBarcode.Empty();
	}

	X2_MoveTo(m_lBarcodePos_X2, SFM_WAIT);

	INT nStatus = Err_No_Error;
	if (szBarcode.IsEmpty() == TRUE)
	{
		for (i=0; i<2; i++)
		{
			//1. First time
			EnableKeyboardHook(TRUE);
			nStatus = FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
			X2_Sync();
			Sleep(100);
			EnableKeyboardHook(FALSE);
			GetEncoderValue();
			szBarcode = GetAppKeyboard();

			if ( nStatus != Err_No_Error )
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ scan barcode");
				SetErrorMessage("Gripper2 is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}
		
			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName2 = szBarcode;
					szLogMsg.Format("WL2 gripper default is %s, on X %ld", szBarcode, m_lEnc_X2);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				szBarcode.Empty();
			}

			//2. Second time
			EnableKeyboardHook(TRUE);
			nStatus = FilmFrame2DriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE);
			X2_Sync();
			Sleep(100);
			EnableKeyboardHook(FALSE);
			GetEncoderValue();
			szBarcode = GetAppKeyboard();

			if ( nStatus != Err_No_Error )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ scan barcode");
				SetErrorMessage("Gripper2 is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName2 = szBarcode;
					szLogMsg.Format("WL2 gripper default is %s, on X %ld", szBarcode, m_lEnc_X2);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				szBarcode.Empty();
			}

			//3. Third time
			EnableKeyboardHook(TRUE);
			nStatus = FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE);
			X2_Sync();
			Sleep(100);
			EnableKeyboardHook(FALSE);
			GetEncoderValue();
			szBarcode = GetAppKeyboard();

			if ( nStatus != Err_No_Error )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper2 is Jam @ scan barcode");
				SetErrorMessage("Gripper2 is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			if (szBarcode.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(szBarcode) )
				{
					m_szBarcodeName2 = szBarcode;
					szLogMsg.Format("WL2 gripper default is %s, on X %ld", szBarcode, m_lEnc_X2);
					CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
					return Err_No_Error;
				}
				szBarcode.Empty();
			}
		}
		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}

BOOL CWaferLoader::EnableKeyboardHook(CONST BOOL bEnable)
{
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (bEnable)
	{
		CMS896AApp::m_szKeyboard = _T("");			//Reset keyboard key buffer
		CMS896AApp::m_bEnableKeyboardHook = TRUE;
		//Sleep(500);
	}
	else
	{
		Sleep(500);
		CMS896AApp::m_bEnableKeyboardHook = FALSE;
	}

	return TRUE;
}


INT CWaferLoader::ScanBySymbol(BOOL bAllowSearch, BOOL bIsCheckBarcode)
{
	short i = 0;
	
	m_szBarcodeName.Empty();

	//v4.02T5
	LONG lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
		(m_lBarcodePos_X - m_lLoadPos_X), (m_lBarcodePos_X - m_lLoadPos_X), 
		HIPEC_SAMPLE_RATE, &m_stWLAxis_X);

	LONG lDelay = 500;
	if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
		lDelay = lProfileTime - 200;		//Servo motor speed runs faster than stepper motor
	else
		lDelay = lProfileTime - 250;		//Stepper motor runs slower
	
	if (lDelay < WL_BARCODE_SCAN_MIN_TIME)
		lDelay = WL_BARCODE_SCAN_MIN_TIME;

	//Move Gripper to Scan bar code position
	//pllm
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	{
		X_MoveTo(m_lBarcodePos_X, SFM_NOWAIT);
		Sleep(lDelay);	//v4.01
		ReadBarcode(&m_szBarcodeName, 50, 5);	
		GetEncoderValue();
		CString szLogMsg;
		szLogMsg.Format("PPLM gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
		(*m_psmfSRam)["WaferTable"]["PPLM_WT1InBarcode"] = m_szBarcodeName; //4.53D82
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		X_Sync();
	}
	else if (bIsCheckBarcode == TRUE && GetExpType() != WL_EXP_GEAR_DIRECTRING)
	{
		X_MoveTo(m_lBarcodeCheckPos_X, SFM_NOWAIT);
		Sleep(lDelay);	//v4.01
		ReadBarcode(&m_szBarcodeName, 50, 5);	
		GetEncoderValue();
		CString szLogMsg;
		szLogMsg.Format("NDR gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		X_Sync();
	}
	else
	{
		//v3.98T1
		if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
		{
			lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
				(m_lUnloadPos_X - m_lLoadPos_X), (m_lUnloadPos_X - m_lLoadPos_X), HIPEC_SAMPLE_RATE, &m_stWLAxis_X);

			lDelay = lProfileTime /2;
			X_MoveTo(m_lUnloadPos_X, SFM_NOWAIT);
			Sleep(lDelay);	//v4.01
			ReadBarcode(&m_szBarcodeName, 50, 5);	
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("DR gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();
		}
		else
		{
			X_MoveTo(m_lBarcodePos_X, SFM_NOWAIT);
			Sleep(lDelay);	//v4.01
			ReadBarcode(&m_szBarcodeName, 50, 5);	
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("ND gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();
		}
	}
	
	if (m_szBarcodeName.IsEmpty() == FALSE)
	{
		if( CheckBarcodeLength(m_szBarcodeName) )
		{
			return Err_No_Error;
		}
		
		m_szBarcodeName.Empty();
	}

	if (m_szBarcodeName.IsEmpty() == TRUE)
	{
		if ( m_lScanRange == 0 || bAllowSearch == FALSE)
		{
			return ERR_READ_BC_FAILED;
		}

	
		for (i=0; i< 2; i++)
		{
#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}


			ReadBarcode(&m_szBarcodeName);
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("WFT gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();

			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}
				
				m_szBarcodeName.Empty();
			}

			if ( FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE) == FALSE )
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode(&m_szBarcodeName);
			GetEncoderValue();
			szLogMsg.Format("WFT try gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();

			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}
				
				m_szBarcodeName.Empty();
			}

#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode(&m_szBarcodeName);
			GetEncoderValue();
			szLogMsg.Format("WFT try2 gripper read barcode is %s, on X %ld", m_szBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();

			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}
				
				m_szBarcodeName.Empty();
			}
		}

		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}

//4.52D10 sub-fnc scan
INT CWaferLoader::ScanBySymbolWith2ndPosn(BOOL bAllowSearch, BOOL bIsCheckBarcode)
{
	short i = 0;
	
	m_sz2nd1DBarcodeName.Empty();
	CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 1");

	//v4.02T5
	LONG lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
		(m_lBarcodePos_X - m_lLoadPos_X), (m_lBarcodePos_X - m_lLoadPos_X), 
		HIPEC_SAMPLE_RATE, &m_stWLAxis_X);
	CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 2");

	LONG lDelay = 500;
	if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
		lDelay = lProfileTime - 200;		//Servo motor speed runs faster than stepper motor
	else
		lDelay = lProfileTime - 250;		//Stepper motor runs slower
	
	if (lDelay < WL_BARCODE_SCAN_MIN_TIME)
		lDelay = WL_BARCODE_SCAN_MIN_TIME;
	CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 3");

	//Move Gripper to Scan bar code position
	//pllm
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	{
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 3.1");

		X_MoveTo(m_l2DBarcodePos_X, SFM_NOWAIT);
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 3.2");

		Sleep(lDelay);	//v4.01
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 3.3");

		ReadBarcode(&m_sz2nd1DBarcodeName, 50, 5);
		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 3.4");

		GetEncoderValue();
		CString szLogMsg;
		szLogMsg.Format("WaferLabel -- PPLM gripper read barcode is %s, on X %ld", m_sz2nd1DBarcodeName, m_lEnc_X);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		X_Sync();
	}
	else if (bIsCheckBarcode == TRUE && GetExpType() != WL_EXP_GEAR_DIRECTRING)
	{
		X_MoveTo(m_lBarcodeCheckPos_X, SFM_NOWAIT);
		Sleep(lDelay);	//v4.01
		ReadBarcode(&m_sz2nd1DBarcodeName, 50, 5);	
		GetEncoderValue();
		CString szLogMsg;
		szLogMsg.Format("WaferLabel -- NDR gripper read barcode is %s, on X %ld", m_sz2nd1DBarcodeName, m_lEnc_X);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		X_Sync();
	}
	else
	{
		//v3.98T1
		if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
		{
			lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
				(m_lUnloadPos_X - m_lLoadPos_X), (m_lUnloadPos_X - m_lLoadPos_X), HIPEC_SAMPLE_RATE, &m_stWLAxis_X);

			lDelay = lProfileTime /2;
			X_MoveTo(m_lUnloadPos_X, SFM_NOWAIT);
			Sleep(lDelay);	//v4.01
			ReadBarcode(&m_sz2nd1DBarcodeName, 50, 5);	
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("WaferLabel -- DR gripper read barcode is %s, on X %ld", m_sz2nd1DBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();
		}
		else
		{
			X_MoveTo(m_l2DBarcodePos_X, SFM_NOWAIT);
			Sleep(lDelay);	//v4.01
			ReadBarcode(&m_sz2nd1DBarcodeName, 50, 5);	
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("WaferLabel -- ND gripper read barcode is %s, on X %ld", m_sz2nd1DBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();
		}
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 4");
	
	if (m_sz2nd1DBarcodeName.IsEmpty() == FALSE)
	{
		if( CheckBarcodeLength(m_sz2nd1DBarcodeName) )
		{
			return Err_No_Error;
		}
		
		m_sz2nd1DBarcodeName.Empty();
	}
	CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 5");

	if (m_sz2nd1DBarcodeName.IsEmpty() == TRUE)
	{
		if ( m_lScanRange == 0 || bAllowSearch == FALSE)
		{
			return ERR_READ_BC_FAILED;
		}

		CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 6");
	
		for (i=0; i< 2; i++)
		{
#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 7");

			ReadBarcode(&m_sz2nd1DBarcodeName);
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("WaferLabel -- WFT gripper read barcode is %s, on X %ld", m_sz2nd1DBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();

			if (m_sz2nd1DBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_sz2nd1DBarcodeName) )
				{
					return Err_No_Error;
				}
				
				m_sz2nd1DBarcodeName.Empty();
			}
			CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 8");

			if ( FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE) == FALSE )
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}
			CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 9");

			ReadBarcode(&m_sz2nd1DBarcodeName);
			GetEncoderValue();
			szLogMsg.Format("WaferLabel -- WFT try gripper read barcode is %s, on X %ld", m_sz2nd1DBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();

			if (m_sz2nd1DBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_sz2nd1DBarcodeName) )
				{
					return Err_No_Error;
				}
				
				m_sz2nd1DBarcodeName.Empty();
			}
			CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Test 10");

#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("WaferLabel -- Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode(&m_sz2nd1DBarcodeName);
			GetEncoderValue();
			szLogMsg.Format("WaferLabel -- WFT try2 gripper read barcode is %s, on X %ld", m_sz2nd1DBarcodeName, m_lEnc_X);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X_Sync();

			if (m_sz2nd1DBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_sz2nd1DBarcodeName) )
				{
					return Err_No_Error;
				}
				
				m_sz2nd1DBarcodeName.Empty();
			}
		}

		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}

INT CWaferLoader::ScanBySymbol2(BOOL bAllowSearch, BOOL bIsCheckBarcode)
{
	short i = 0;
	if ( IsESDualWL()==FALSE )
		return Err_No_Error;
	
	m_szBarcodeName2.Empty();

	//v4.02T5
	LONG lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X2, "mpfWaferGripper", 
		(m_lBarcodePos_X2 - m_lLoadPos_X2), (m_lBarcodePos_X2 - m_lLoadPos_X2), 
		HIPEC_SAMPLE_RATE, &m_stWLAxis_X2);

	LONG lDelay = 500;
	if (CMS896AStn::MotionIsServo(WL_AXIS_X2, &m_stWLAxis_X2) == TRUE)
		lDelay = lProfileTime - 200;		//Servo motor speed runs faster than stepper motor
	else
		lDelay = lProfileTime - 250;		//Stepper motor runs slower
	
	if (lDelay < WL_BARCODE_SCAN_MIN_TIME)
		lDelay = WL_BARCODE_SCAN_MIN_TIME;

	//Move Gripper to Scan bar code position
	//pllm
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS))
	{
		X2_MoveTo(m_lBarcodePos_X2, SFM_NOWAIT);
		Sleep(lDelay);	//v4.01
		ReadBarcode2(&m_szBarcodeName2, 50, 5);	
		GetEncoderValue();
		CString szLogMsg;
		szLogMsg.Format("WT2 PPLM gripper read barcode is %s, on X %ld", m_szBarcodeName2, m_lEnc_X2);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		X2_Sync();
	}
	else if (bIsCheckBarcode == TRUE && GetExpType() != WL_EXP_GEAR_DIRECTRING)
	{
		//v3.98T1
		X2_MoveTo(m_lBarcodeCheckPos_X2, SFM_NOWAIT);
		Sleep(lDelay);	//v4.01
		ReadBarcode2(&m_szBarcodeName2, 50, 5);	
		GetEncoderValue();
		CString szLogMsg;
		szLogMsg.Format("WT2 NDR gripper read barcode is %s, on X %ld", m_szBarcodeName2, m_lEnc_X2);
		CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
		X2_Sync();
	}
	else
	{
		if (GetExpType() == WL_EXP_GEAR_DIRECTRING)
		{
			lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X2, "mpfWaferGripper", 
				(m_lUnloadPos_X2 - m_lLoadPos_X2), (m_lUnloadPos_X2 - m_lLoadPos_X2), HIPEC_SAMPLE_RATE, &m_stWLAxis_X2);

			lDelay = lProfileTime /2;
			X2_MoveTo(m_lUnloadPos_X2, SFM_NOWAIT);
			Sleep(lDelay);	//v4.01
			ReadBarcode2(&m_szBarcodeName2, 50, 5);	
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("WT2 DR gripper read barcode is %s, on X %ld", m_szBarcodeName2, m_lEnc_X2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X2_Sync();
		}
		else
		{
			//v3.98T1
			X2_MoveTo(m_lBarcodePos_X2, SFM_NOWAIT);
			Sleep(lDelay);	//v4.01
			ReadBarcode2(&m_szBarcodeName2, 50, 5);	
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("WT2 ND gripper read barcode is %s, on X %ld", m_szBarcodeName2, m_lEnc_X2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X2_Sync();
		}
	}

	if (m_szBarcodeName2.IsEmpty() == FALSE)
	{
		if( CheckBarcodeLength(m_szBarcodeName2) )
		{
			return Err_No_Error;
		}
		
		m_szBarcodeName2.Empty();
	}

	if (m_szBarcodeName2.IsEmpty() == TRUE)
	{
		if ( m_lScanRange == 0 || bAllowSearch == FALSE)
		{
			return ERR_READ_BC_FAILED;
		}

	
		for (i=0; i< 2; i++)
		{
#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode2(&m_szBarcodeName2);
			GetEncoderValue();
			CString szLogMsg;
			szLogMsg.Format("WT2 gripper read barcode is %s, on X %ld", m_szBarcodeName2, m_lEnc_X2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X2_Sync();

			if (m_szBarcodeName2.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName2) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName2.Empty();
			}

			if ( FilmFrame2DriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE) == FALSE )
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode2(&m_szBarcodeName2);
			GetEncoderValue();
			szLogMsg.Format("WT2 try gripper read barcode is %s, on X %ld", m_szBarcodeName2, m_lEnc_X2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X2_Sync();

			if (m_szBarcodeName2.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName2) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName2.Empty();
			}

#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrame2DriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode2(&m_szBarcodeName2);
			GetEncoderValue();
			szLogMsg.Format("WT2 try2 gripper read barcode is %s, on X %ld", m_szBarcodeName2, m_lEnc_X2);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			X2_Sync();

			if (m_szBarcodeName2.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName2) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName2.Empty();
			}
		}

		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}

INT CWaferLoader::ScanByRegal(BOOL bAllowSearch, BOOL bIsCheckBarcode, BOOL bIsCheckBarcodeFucEnabled)
{
	short i = 0;
	
	m_szBarcodeName.Empty();

	//v4.02T5
	LONG lProfileTime = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
		(m_lBarcodePos_X - m_lLoadPos_X), (m_lBarcodePos_X - m_lLoadPos_X), 
		HIPEC_SAMPLE_RATE, &m_stWLAxis_X);

	LONG lDelay = 500;
	if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
		lDelay = lProfileTime - 200;		//Servo motor speed runs faster than stepper motor
	else
		lDelay = lProfileTime - 250;		//Stepper motor runs slower
	
	if (lDelay < WL_BARCODE_SCAN_MIN_TIME)
		lDelay = WL_BARCODE_SCAN_MIN_TIME;

	//Move Gripper to Scan bar code position
	//pllm
	if (bIsCheckBarcode == TRUE)
	{
		//StartBarcode_Regal(TRUE);
		PurgeBarcode_Regal();
		X_MoveTo(m_lBarcodeCheckPos_X, SFM_NOWAIT);
		ReadBarcode_Regal(&m_szBarcodeName, 10);
		//StartBarcode_Regal(FALSE);
		X_Sync();
	}
	else
	{
		//v3.98T1
		PurgeBarcode_Regal();

		if (bIsCheckBarcodeFucEnabled == TRUE)
		{
			X_MoveTo(m_lBarcodePos_X, SFM_NOWAIT);
		}
		else
		{
			X_MoveTo(m_lUnloadPos_X, SFM_NOWAIT);
		}

		ReadBarcode_Regal(&m_szBarcodeName);
		//StartBarcode_Regal(FALSE);
		X_Sync();
	}

	// start retry
	if (m_szBarcodeName.IsEmpty() == TRUE)
	{
		if ( m_lScanRange == 0 || bAllowSearch == FALSE)
		{
			return FALSE;
		}

		X_MoveTo(m_lBarcodePos_X, SFM_WAIT);
	
		for (i=0; i< 2; i++)
		{

			//StartBarcode_Regal(TRUE);
			
#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				////StartBarcode_Regal(FALSE);
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode_Regal(&m_szBarcodeName, 10);
			X_Sync();

			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}
		
			if ( FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE) == FALSE )
			{
				//StartBarcode_Regal(FALSE);
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode_Regal(&m_szBarcodeName, 10);
			X_Sync();

			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}
			
#ifdef NU_MOTION
			//v3.86T3
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
#else
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
#endif
			{
				//StartBarcode_Regal(FALSE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode_Regal(&m_szBarcodeName, 10);
			X_Sync();
			
			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}
		}

		//StartBarcode_Regal(FALSE);
		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}


INT CWaferLoader::ScanByKeyence(BOOL bAllowSearch, BOOL bIsCompareBarcode)
{
	short i = 0;
	
	m_szBarcodeName.Empty();

	//Move Gripper to Scan bar code position
	if (bIsCompareBarcode == TRUE)
	{
		X_MoveTo(m_lBarcodeCheckPos_X-1000);
	}
	else
	{
		X_MoveTo(m_lBarcodePos_X-1000);
	}

	StartBarcode(TRUE);
	FilmFrameDriveIn(HP_POSITIVE_DIR, 1000, 200, FALSE);

	ReadBarcode_Keyence(&m_szBarcodeName);
	if (m_szBarcodeName.IsEmpty() == TRUE)
	{
		if ( m_lScanRange == 0 || bAllowSearch == FALSE)
		{
			StartBarcode(FALSE);
			return ERR_READ_BC_FAILED;
		}
	
		//Move to -ve direction	
		Sleep(100);
		if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
			SetErrorMessage("Gripper is Jam @ scan barcode");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);

			StartBarcode(FALSE);
			return Err_FrameJammed;
		}
	
		ReadBarcode_Keyence(&m_szBarcodeName);
		X_Sync();
		if (m_szBarcodeName.IsEmpty() == FALSE)
		{
			if( CheckBarcodeLength(m_szBarcodeName) )
			{
				StartBarcode(FALSE);
				return Err_No_Error;
			}

			m_szBarcodeName.Empty();
		}

		//Loop start
		for (i=0; i< 2; i++)
		{
			//Move to +ve direction	
			Sleep(100);
			if ( FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE) == FALSE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");

				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);

				StartBarcode(FALSE);
				return Err_FrameJammed;
			}

			ReadBarcode_Keyence(&m_szBarcodeName);
			X_Sync();
			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					StartBarcode(FALSE);
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}

			//Move to -ve direction	
			Sleep(100);
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange*2, 200, FALSE) == FALSE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
			
				StartBarcode(FALSE);
				return Err_FrameJammed;
			}

			ReadBarcode_Keyence(&m_szBarcodeName);
			X_Sync();
			if (m_szBarcodeName.IsEmpty() == FALSE)
			{
				if( CheckBarcodeLength(m_szBarcodeName) )
				{
					StartBarcode(FALSE);
					return Err_No_Error;
				}

				m_szBarcodeName.Empty();
			}
		}

		//Move to +ve direction	
		Sleep(100);
		if ( FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange, 200, FALSE) == FALSE )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
			SetErrorMessage("Gripper is Jam @ scan barcode");

			SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);

			StartBarcode(FALSE);
			return Err_FrameJammed;
		}
	
		ReadBarcode_Keyence(&m_szBarcodeName);
		X_Sync();
		if (m_szBarcodeName.IsEmpty() == FALSE)
		{
			if( CheckBarcodeLength(m_szBarcodeName) )
			{
				StartBarcode(FALSE);
				return Err_No_Error;
			}

			m_szBarcodeName.Empty();
		}

		StartBarcode(FALSE);
		return ERR_READ_BC_FAILED;
	}

	StartBarcode(FALSE);
	return Err_No_Error;
}


INT CWaferLoader::ScanningBarcodeOnTableWithTheta(CString& szBarcode, BOOL bAllowSearch)
{
	//PLSG MS109	//v4.46T19
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	ULONG nPLLM = pApp->GetFeatureValue(MS896A_PLLM_PRODUCT);

	if (!AutoRotateWaferForBC(FALSE))	
	{
		return ERR_READ_BC_FAILED;
	}
	
	//v4.50A2
	//Move table to 2D code position on wafer
	LONG lGTPosX, lGTPosY;
	if (GetBCModel() == WL_BAR_2D_BARCODE)
	{
		LONG lNewGTNo = (*m_psmfSRam)["WaferTable"]["GTPosnUse2"];
		if (lNewGTNo == 0)
		{
			lGTPosX = (*m_psmfSRam)["WaferTable"]["GTPosnX"];
			lGTPosY = (*m_psmfSRam)["WaferTable"]["GTPosnY"];
		}
		else
		{
			lGTPosX = (*m_psmfSRam)["WaferTable"]["GTPosnX2"];
			lGTPosY = (*m_psmfSRam)["WaferTable"]["GTPosnY2"];
		}

		CString szTemp;
		szTemp.Format("WT to 2D BARCODE pos (%ld, %ld)", lGTPosX, lGTPosY);
//HmiMessage(szTemp);
		CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);
		CMSLogFileUtility::Instance()->WL_LogStatus(szTemp);		

		BOOL bTableStatus = MoveWaferTableLoadUnload(0, 0, FALSE);	//MS100 9Inch
		MoveWaferTable(lGTPosX, lGTPosY);
	}

	if (GetBCModel() == WL_BAR_DEFAULT)	
	{
		INT nStatus = ScanningBarcodeOnTableWithTheta_KbInput(szBarcode, bAllowSearch);

		szBarcode.Replace("\n", "");
		szBarcode.Replace("\r", "");

		CString szMsg;
		szMsg.Format("ScanningBarcode OnTableWithTheta: WT1 barcode (DEFAULT) scanned = %s", szBarcode);
		CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		//HmiMessage(szMsg);
		return nStatus;
	}

	//CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulTimeOut = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, 0);
	ULONG ulRetry = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, 0);
	if( ulTimeOut==0 )
		ulTimeOut = 100;
	if( ulRetry==0 )
		ulRetry = 10;
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, ulTimeOut);
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, ulRetry);

	CMSLogFileUtility::Instance()->WL_LogStatus("Scan barcode on table with T begin");
	ReadBarcode_4All(&szBarcode, ulTimeOut, ulRetry);

	if (pApp->GetCustomerName() == "Cree" && szBarcode == "")
	{
		MoveWaferTable(lGTPosX + m_lScanRange*15, lGTPosY);
		ReadBarcode_4All(&szBarcode, ulTimeOut, ulRetry);

		if (szBarcode == "")
		{
			MoveWaferTable(lGTPosX - m_lScanRange*15, lGTPosY);
			ReadBarcode_4All(&szBarcode, ulTimeOut, ulRetry);

			if (szBarcode == "")
			{
				MoveWaferTable(lGTPosX, lGTPosY - m_lScanRange*15);
				ReadBarcode_4All(&szBarcode, ulTimeOut, ulRetry);

				if (szBarcode == "")
				{
					MoveWaferTable(lGTPosX, lGTPosY + m_lScanRange*15);
					ReadBarcode_4All(&szBarcode, ulTimeOut, ulRetry);		
				}
			}
		}
	}

	LONG lOrgX = 0, lOrgY = 0, lOrgT = 0;
	GetES101WTEncoder(&lOrgX, &lOrgY, &lOrgT, FALSE);
	CString szLogMsg;
	szLogMsg.Format("WT1 On Table read barcode X(%ld),Y(%ld),T(%ld) BC:%s", lOrgX, lOrgY, lOrgT, szBarcode);

	if (szBarcode.IsEmpty() == FALSE)
	{
		if( CheckBarcodeLength(szBarcode) )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			return Err_No_Error;
		}
	}

	LONG lTCounter= 0;
	if (m_dThetaRes != 0 )
	{
		lTCounter = (LONG)(m_dWftTScanRange/m_dThetaRes);
	}
	LONG lYCounter = m_lWftYScanRange;
	LONG lScanBarcodeTimeout = 1000;
	for(int i=1; i<=3 ; i++)
	{
		if (MoveWaferTableForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode(szBarcode, lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, FALSE);
			return Err_No_Error;
		}
		SyncWaferTableXYT("T");
		Sleep(200);

		if (MoveWaferTableForBarCode(2, HP_POSITIVE_DIR, -lTCounter*2, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode(szBarcode, lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, FALSE);
			return Err_No_Error;
		}
		SyncWaferTableXYT("T");
		Sleep(200);

		if (MoveWaferTableForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode(szBarcode, lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, FALSE);
			return Err_No_Error;
		}
		SyncWaferTableXYT("T");
		Sleep(200);

		if( lYCounter==0 )
			break;

		switch( i )
		{
		case 1:
			if (MoveWaferTableForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 2:
			if (MoveWaferTableForBarCode(1, HP_POSITIVE_DIR, -lYCounter*2, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 3:
			if (MoveWaferTableForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		}
		ReadBarcode_4All(&szBarcode, ulTimeOut, ulRetry);		//v2.93T2
		LONG lX = 0, lY = 0, lT = 0;
		GetES101WTEncoder(&lX, &lY, &lT, FALSE);
		szLogMsg.Format("WT1 On Table read barcode X(%ld),Y(%ld),T(%ld) BC:%s", lX, lY, lT, szBarcode);
		SyncWaferTableXYT("Y");

		if (szBarcode.IsEmpty() == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT1 Y move read barcode is " + szBarcode);
			if( CheckBarcodeLength(szBarcode) )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				return Err_No_Error;
			}

			szBarcode.Empty();
		}
	}

	//anichia003
	BOOL bNichia = FALSE;
	//CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == CTM_NICHIA)
	{
		bNichia = TRUE;
	}
	
	//Manual-input wafer barcode from keyboard or hand-held BC scanner
	if ( bNichia || pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_MANUAL_INPUT_BC) || IsESMachine() )	//v4.50A28	//Cree HZ
	{
		CString szContent	= "Please input barcode manually";
		if (bNichia)	//anichia003
			SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_WARNING, szContent, "CLOSE");	//anichia003
		else
			HmiMessage_Red_Back(szContent);
		CString szNewBarcode	= _T("");
		if( WL_InputBarcode(1, szNewBarcode) )
		{
			szBarcode = szNewBarcode;
			return Err_No_Error;
		}
	}

	return ERR_READ_BC_FAILED;
}

INT CWaferLoader::ScanningBarcodeOnTableWithTheta_KbInput(CString& szBarcode, BOOL bAllowSearch)
{
	EnableKeyboardHook(TRUE);
	Sleep(300);
	EnableKeyboardHook(FALSE);
	szBarcode = GetAppKeyboard();

	LONG lOrgX = 0, lOrgY = 0, lOrgT = 0;
	GetES101WTEncoder(&lOrgX, &lOrgY, &lOrgT, FALSE);
	CString szLogMsg;
	szLogMsg.Format("WT1 On Table read barcode X(%ld),Y(%ld),T(%ld)", lOrgX, lOrgY, lOrgT);

	if (szBarcode.IsEmpty() == FALSE)
	{
		if( CheckBarcodeLength(szBarcode) )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			return Err_No_Error;
		}

		szBarcode.Empty();
	}


	LONG lTCounter= 0;
	if (m_dThetaRes != 0 )
	{
		lTCounter = (LONG)(m_dWftTScanRange/m_dThetaRes);
	}

	LONG lYCounter = m_lWftYScanRange;
	LONG lScanBarcodeTimeout = 1000;
	
	for (int i=1; i<=3 ; i++)
	{
		if (MoveWaferTableForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		//v4.40T13
		//lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		//if( ContinueScanBarcode_KbInput(lScanBarcodeTimeout) )
		//{
		//	if( m_bRotateBackAfterScan )
		//		MoveWaferThetaTo(lOrgT, FALSE);
		//	return TRUE;
		//}
		EnableKeyboardHook(TRUE);
		SyncWaferTableXYT("T");
		Sleep(200);
		EnableKeyboardHook(FALSE);
		szBarcode = GetAppKeyboard();

		if (szBarcode.GetLength() > 0)
		{
			if( CheckBarcodeLength(szBarcode) )
			{
				if ( m_bRotateBackAfterScan )
					MoveWaferThetaTo(lOrgT);
				return Err_No_Error;
			}
			else
			{
				szBarcode.Empty();
			}
		}


		if (MoveWaferTableForBarCode(2, HP_POSITIVE_DIR, -lTCounter*2, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		EnableKeyboardHook(TRUE);
		SyncWaferTableXYT("T");
		Sleep(200);
		EnableKeyboardHook(FALSE);
		szBarcode = GetAppKeyboard();

		if (szBarcode.GetLength() > 0)
		{
			if( CheckBarcodeLength(szBarcode) )
			{
				if ( m_bRotateBackAfterScan )
					MoveWaferThetaTo(lOrgT);
				return Err_No_Error;
			}
			else
			{
				szBarcode.Empty();
			}
		}


		if (MoveWaferTableForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}

		EnableKeyboardHook(TRUE);
		SyncWaferTableXYT("T");
		Sleep(200);
		EnableKeyboardHook(FALSE);
		szBarcode = GetAppKeyboard();

		if (szBarcode.GetLength() > 0)
		{
			if( CheckBarcodeLength(szBarcode) )
			{
				if ( m_bRotateBackAfterScan )
					MoveWaferThetaTo(lOrgT);
				return Err_No_Error;
			}
			else
			{
				szBarcode.Empty();
			}
		}

		if( lYCounter==0 )
			break;

		switch( i )
		{
		case 1:
			if (MoveWaferTableForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 2:
			if (MoveWaferTableForBarCode(1, HP_POSITIVE_DIR, -lYCounter*2, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 3:
			if (MoveWaferTableForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		}

		EnableKeyboardHook(TRUE);
		//Sleep(500);

		LONG lX = 0, lY = 0, lT = 0;
		GetES101WTEncoder(&lX, &lY, &lT, FALSE);
		szLogMsg.Format("WT1 On Table read barcode X(%ld),Y(%ld),T(%ld)", lX, lY, lT);
		SyncWaferTableXYT("Y");
		
		//v4.40T13
		Sleep(200);
		EnableKeyboardHook(FALSE);
		szBarcode = GetAppKeyboard();

		if (szBarcode.IsEmpty() == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT1 Y move read barcode is " + szBarcode);
			if( CheckBarcodeLength(szBarcode) )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				return Err_No_Error;
			}

			szBarcode.Empty();
		}
	}

	//anichia003
	BOOL bNichia = FALSE;
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == CTM_NICHIA)
	{
		bNichia = TRUE;
	}

	CString szContent	= "Please input barcode manually";
	if (bNichia)	//anichia003
		SetAlert_Msg_Red_Yellow(IDS_CTM_NICHIA_WARNING, szContent, "CLOSE");	//anichia003
	else
		HmiMessage_Red_Back(szContent);
	CString szNewBarcode	= _T("");
	if( WL_InputBarcode(1, szNewBarcode) )
	{
		szBarcode = szNewBarcode;
		return Err_No_Error;
	}

	return ERR_READ_BC_FAILED;
}

INT CWaferLoader::ScanningBarcodeOnTable2WithTheta_KbInput(BOOL bAllowSearch)
{
	EnableKeyboardHook(TRUE);
	Sleep(500);
	m_szBarcodeName2 = GetAppKeyboard();
	EnableKeyboardHook(FALSE);

	LONG lOrgX = 0, lOrgY = 0, lOrgT = 0;
	GetES101WTEncoder(&lOrgX, &lOrgY, &lOrgT, TRUE);
	CString szLogMsg;
	szLogMsg.Format("WT2 On Table read barcode X(%ld),Y(%ld),T(%ld)", lOrgX, lOrgY, lOrgT);

	if (m_szBarcodeName2.IsEmpty() == FALSE)
	{
		if( CheckBarcodeLength(m_szBarcodeName2) )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			return Err_No_Error;
		}

		m_szBarcodeName.Empty();
	}

	LONG lTCounter= 0;
	if (m_dThetaRes != 0 )
	{
		lTCounter = (LONG)(m_dWftTScanRange/m_dThetaRes);
	}

	LONG lYCounter = m_lWftYScanRange;
	LONG lScanBarcodeTimeout = 1000;
	for(int i=1; i<=3 ; i++)
	{
		if (MoveWaferTable2ForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode2_KbInput(lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, TRUE);
			return Err_No_Error;
		}
		SyncWaferTable2XYT("T");
		Sleep(200);

		if (MoveWaferTable2ForBarCode(2, HP_POSITIVE_DIR, -lTCounter*2, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode2_KbInput(lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, TRUE);
			return Err_No_Error;
		}
		SyncWaferTable2XYT("T");
		Sleep(200);

		if (MoveWaferTable2ForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode2_KbInput(lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, TRUE);
			return Err_No_Error;
		}

		SyncWaferTable2XYT("T");
		Sleep(200);

		if( lYCounter==0 )
			break;

		switch( i )
		{
		case 1:
			if (MoveWaferTable2ForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 2:
			if (MoveWaferTable2ForBarCode(1, HP_POSITIVE_DIR, -lYCounter*2, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 3:
			if (MoveWaferTable2ForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		}

		EnableKeyboardHook(TRUE);
		Sleep(500);
		m_szBarcodeName2 = GetAppKeyboard();
		EnableKeyboardHook(FALSE);

		LONG lX = 0, lY = 0, lT = 0;
		GetES101WTEncoder(&lX, &lY, &lT, TRUE);
		szLogMsg.Format("WT2 On Table read barcode X(%ld),Y(%ld),T(%ld)", lX, lY, lT);
		SyncWaferTable2XYT("Y");

		if (m_szBarcodeName2.IsEmpty() == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT2 Y move read barcode is " + m_szBarcodeName2);
			if( CheckBarcodeLength(m_szBarcodeName2) )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				return Err_No_Error;
			}

			m_szBarcodeName2.Empty();
		}
	}

	HmiMessage_Red_Back("please input barcode manually!");
	CString szBarcode	= _T("");
	if( WL_InputBarcode(2, szBarcode) )
	{
		m_szBarcodeName2 = szBarcode;
		return Err_No_Error;
	}

	return ERR_READ_BC_FAILED;
}


INT CWaferLoader::ScanningBarcodeOnTable2WithTheta(BOOL bAllowSearch)	// AOI try
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulTimeOut = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, 0);
	ULONG ulRetry = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, 0);
	if( ulTimeOut==0 )
		ulTimeOut = 100;
	if( ulRetry==0 )
		ulRetry = 10;
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, ulTimeOut);
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, ulRetry);
	ReadBarcode2_4All(&m_szBarcodeName2, ulTimeOut, ulRetry);
	if( GetBCModel() == WL_BAR_DEFAULT_COM )
		CMSLogFileUtility::Instance()->WL_LogStatus("WT2 barcode type: multi bean com");
	CMSLogFileUtility::Instance()->WL_LogStatus("WT2 on table read barcode is BC:" + m_szBarcodeName2);

	LONG lOrgX = 0, lOrgY = 0, lOrgT = 0;
	GetES101WTEncoder(&lOrgX, &lOrgY, &lOrgT, TRUE);
	CString szLogMsg;
	szLogMsg.Format("WT2 On Table read barcode X(%ld),Y(%ld),T(%ld)", lOrgX, lOrgY, lOrgT);
	CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);

	if (m_szBarcodeName2.IsEmpty() == FALSE)
	{
		if( CheckBarcodeLength(m_szBarcodeName2) )
		{
			return Err_No_Error;
		}

		m_szBarcodeName2.Empty();
	}

	LONG lTCounter = 0;
	if (m_dThetaRes != 0 )
	{
		lTCounter = (LONG)(m_dWftTScanRange/m_dThetaRes);
	}
	LONG lYCounter = m_lWftYScanRange;
	LONG lScanBarcodeTimeout = 1000;

	for (int i=1; i<=3 ; i++)
	{
		if (MoveWaferTable2ForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode2(lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, TRUE);
			return Err_No_Error;
		}
		SyncWaferTable2XYT("T");
		Sleep(200);

		if (MoveWaferTable2ForBarCode(2, HP_POSITIVE_DIR, -lTCounter*2, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode2(lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, TRUE);
			return Err_No_Error;
		}
		SyncWaferTable2XYT("T");
		Sleep(200);

		if (MoveWaferTable2ForBarCode(2, HP_POSITIVE_DIR, lTCounter, FALSE) == FALSE)
		{
			SetAlert(IDS_WT_CANNOT_MOVE);
			return ERR_WT_CANNOT_MOVE;
		}
		lScanBarcodeTimeout = (*m_psmfSRam)["WaferTable"]["Serch Barcode Move Time"];
		if( ContinueScanBarcode2(lScanBarcodeTimeout) )
		{
			if( m_bRotateBackAfterScan )
				MoveWaferThetaTo(lOrgT, TRUE);
			return Err_No_Error;
		}
		SyncWaferTable2XYT("T");
		Sleep(200);

		if( lYCounter==0 )
			break;

		switch( i )
		{
		case 1:
			if (MoveWaferTable2ForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 2:
			if (MoveWaferTable2ForBarCode(1, HP_POSITIVE_DIR, -lYCounter*2, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		case 3:
			if (MoveWaferTable2ForBarCode(1, HP_POSITIVE_DIR, lYCounter, FALSE) == FALSE)
			{
				SetAlert(IDS_WT_CANNOT_MOVE);
				return ERR_WT_CANNOT_MOVE;
			}
			break;
		}
		ReadBarcode2_4All(&m_szBarcodeName2, 100, 4);
		CString szLogMsg;
		LONG lX = 0, lY = 0, lT = 0;
		GetES101WTEncoder(&lX, &lY, &lT, TRUE);
		szLogMsg.Format("WT2 on table read barcode X(%ld),Y(%ld),T(%ld) BC:%s", lX, lY, lT, m_szBarcodeName2);
		SyncWaferTable2XYT("Y");
		if (m_szBarcodeName2.IsEmpty() == FALSE)
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT2 Y move read barcode is " + m_szBarcodeName2);
			if( CheckBarcodeLength(m_szBarcodeName2) )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
				return Err_No_Error;
			}

			m_szBarcodeName2.Empty();
		}
	}

	HmiMessage_Red_Back("please input barcode manually!");
	CString szBarcode	= _T("");
	if( WL_InputBarcode(2, szBarcode) )
	{
		m_szBarcodeName2 = szBarcode;
		return Err_No_Error;
	}

	return ERR_READ_BC_FAILED;
}

BOOL CWaferLoader::ContinueScanBarcode(CString& szBarcode, LONG lTimeOut)
{
	DOUBLE dStartTime = GetTime();
	LONG lTimeDiff = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulTimeOut = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, 0);
	ULONG ulRetry = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, 0);
	if( ulTimeOut==0 )
		ulTimeOut = 100;
	if( ulRetry==0 )
		ulRetry = 10;
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, ulTimeOut);
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, ulRetry);

	for(int i= 0; i<100; i++)
	{
		ReadBarcode_4All(&szBarcode, ulTimeOut, ulRetry);
		if( szBarcode.IsEmpty()==false )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT1 continue read barcode is BC:" + szBarcode);
			if( CheckBarcodeLength(szBarcode)!=TRUE )
			{
				szBarcode.Empty();
				continue;
			}
			LONG lX = 0, lY = 0, lT = 0;
			GetES101WTEncoder(&lX, &lY, &lT, FALSE);
			CString szLogMsg;
			szLogMsg.Format("WT1 continue read barcode X(%ld),Y(%ld),T(%ld)", lX, lY, lT);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			SyncWaferTableXYT("T");
			MoveWaferThetaTo(lT, FALSE);

			return TRUE;
		}
		lTimeDiff = (LONG)(GetTime()-dStartTime);
		if( lTimeDiff>lTimeOut )
			break;
	}

	return FALSE;
}

BOOL CWaferLoader::ContinueScanBarcode_KbInput(LONG lTimeOut)
{
	DOUBLE dStartTime = GetTime();
	LONG lTimeDiff = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulTimeOut = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, 0);
	ULONG ulRetry = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, 0);
	if( ulTimeOut==0 )
		ulTimeOut = 100;
	if( ulRetry==0 )
		ulRetry = 10;
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, ulTimeOut);
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, ulRetry);

	EnableKeyboardHook(TRUE);

	for (int i= 0; i<100; i++)
	{
		Sleep(200);
		m_szBarcodeName = GetAppKeyboard();
		SaveBarcodeName();
		if ( m_szBarcodeName.IsEmpty() == false )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT1 continue read barcode is " + m_szBarcodeName);
			if( CheckBarcodeLength(m_szBarcodeName)!=TRUE )
			{
				m_szBarcodeName.Empty();
				continue;
			}

			LONG lX = 0, lY = 0, lT = 0;
			GetES101WTEncoder(&lX, &lY, &lT, FALSE);
			CString szLogMsg;
			szLogMsg.Format("WT1 continue read barcode X(%ld),Y(%ld),T(%ld)", lX, lY, lT);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			SyncWaferTableXYT("T");
			MoveWaferThetaTo(lT, FALSE);
			EnableKeyboardHook(FALSE);
			return TRUE;
		}

		lTimeDiff = (LONG)(GetTime()-dStartTime);
		if( lTimeDiff>lTimeOut )
			break;
	}

	EnableKeyboardHook(FALSE);
	return FALSE;
}

BOOL CWaferLoader::ContinueScanBarcode2_KbInput(LONG lTimeOut)
{
	DOUBLE dStartTime = GetTime();
	LONG lTimeDiff = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulTimeOut = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, 0);
	ULONG ulRetry = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, 0);
	if( ulTimeOut==0 )
		ulTimeOut = 100;
	if( ulRetry==0 )
		ulRetry = 10;
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, ulTimeOut);
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, ulRetry);

	EnableKeyboardHook(TRUE);

	for (int i= 0; i<100; i++)
	{
		Sleep(200);
		m_szBarcodeName2 = GetAppKeyboard();

		if ( m_szBarcodeName2.IsEmpty() == false )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT2 continue read barcode is " + m_szBarcodeName2);
			if( CheckBarcodeLength(m_szBarcodeName2)!=TRUE )
			{
				m_szBarcodeName2.Empty();
				continue;
			}

			LONG lX = 0, lY = 0, lT = 0;
			GetES101WTEncoder(&lX, &lY, &lT, TRUE);
			CString szLogMsg;
			szLogMsg.Format("WT2 continue read barcode X(%ld),Y(%ld),T(%ld)", lX, lY, lT);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			SyncWaferTable2XYT("T");
			MoveWaferThetaTo(lT, TRUE);
			EnableKeyboardHook(FALSE);
			return TRUE;
		}

		lTimeDiff = (LONG)(GetTime()-dStartTime);
		if( lTimeDiff>lTimeOut )
			break;
	}

	EnableKeyboardHook(FALSE);
	return FALSE;
}

BOOL CWaferLoader::ContinueScanBarcode2(LONG lTimeOut)
{
	DOUBLE dStartTime = GetTime();
	LONG lTimeDiff = 0;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	ULONG ulTimeOut = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, 0);
	ULONG ulRetry = pApp->GetProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, 0);
	if( ulTimeOut==0 )
		ulTimeOut = 100;
	if( ulRetry==0 )
		ulRetry = 10;
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_TIMEOUT, ulTimeOut);
	pApp->WriteProfileInt(gszPROFILE_SETTING, REG_SCAN_BARCODE_RETRY_LIMIT, ulRetry);

	for(int i= 0; i<100; i++)
	{
		ReadBarcode2_4All(&m_szBarcodeName2, ulTimeOut, ulRetry);
		if( m_szBarcodeName2.IsEmpty()==false )
		{
			CMSLogFileUtility::Instance()->WL_LogStatus("WT2 continue read barcode is BC:" + m_szBarcodeName2);
			if( CheckBarcodeLength(m_szBarcodeName2)!=TRUE )
			{
				m_szBarcodeName2.Empty();
				continue;
			}
			LONG lX = 0, lY = 0, lT = 0;
			GetES101WTEncoder(&lX, &lY, &lT, TRUE);
			CString szLogMsg;
			szLogMsg.Format("WT2 continue read barcode X(%ld),Y(%ld),T(%ld)", lX, lY, lT);
			CMSLogFileUtility::Instance()->WL_LogStatus(szLogMsg);
			SyncWaferTable2XYT("T");
			MoveWaferThetaTo(lT, TRUE);
			return TRUE;
		}
		lTimeDiff = (LONG)(GetTime()-dStartTime);
		if( lTimeDiff>lTimeOut )
			break;
	}

	return FALSE;
}

BOOL CWaferLoader::ReadBarcode_4All(CString *szData, int nTimeOut, int nRetry)
{
	if (GetBCModel() == WL_BAR_2D_BARCODE)	//v4.50A2
	{
		return Read2DBarCodeFromWafer(szData);
	}
	else if ( GetBCModel() == WL_BAR_DEFAULT_COM )
		return ReadBarcode_DefaultCom(szData, nTimeOut, nRetry);
	else
		return ReadBarcode(szData, nTimeOut, nRetry);
}


BOOL CWaferLoader::ReadBarcode2_4All(CString *szData, int nTimeOut, int nRetry)
{
	if( GetBCModel() == WL_BAR_DEFAULT_COM )
		return ReadBarcode2_DefaultCom(szData, nTimeOut, nRetry);
	else
		return ReadBarcode2(szData, nTimeOut, nRetry);
}

BOOL CWaferLoader::ReadBarcode_DefaultCom(CString *szData)
{
	ULONG ulTimeOut = 100;
	ULONG ulRetry = 10;
	return ReadBarcode_DefaultCom(szData, ulTimeOut, ulRetry);
}

//v4.50A2
BOOL CWaferLoader::Read2DBarCodeFromWafer(CString *szData)
{
	IPC_CServiceMessage stMsg;
	int	nConvID = 0;
	BOOL bReturn;

	//stMsg.InitMessage(sizeof(TABLE_MOVE), &stInfo );

	nConvID = m_comClient.SendRequest(WAFER_PR_STN, "Search2DBarCodeFromWafer", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID,30000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID, stMsg);
			stMsg.GetMsg(sizeof(BOOL), &bReturn);
			break;
		}
	}

	CString szValue = (*m_psmfSRam)["WaferPr"]["2DBarCode"];
	*szData = szValue;

	//v4.50A3
	CString szMsg;
	szMsg.Format("2D Barcode read = %s", szValue);
	CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);		

	if (!bReturn)
		return FALSE;
	return TRUE;
}

BOOL CWaferLoader::CheckBarcodeLength(CString szBarcode)
{
	if (m_lBarcodeCheckLength == 0)
	{
		return TRUE;
	}

	if( m_lBarcodeCheckLengthUpperBound>0 )
	{
		if( szBarcode.GetLength()>=m_lBarcodeCheckLength && szBarcode.GetLength()<=m_lBarcodeCheckLengthUpperBound )
		{
			return TRUE;
		}
	}
	else
	{
		if( szBarcode.GetLength()==m_lBarcodeCheckLength )
		{
			return TRUE;
		}
	}

	return FALSE;
}
INT CWaferLoader::ScanBySymbolOnBufferTable(BOOL bAllowSearch, CString& szBarcodeName)
{
	short i = 0;

	szBarcodeName.Empty();
	CMSLogFileUtility::Instance()->WL_LogStatus("ScanBySymbol on Buffer Table");


	LONG lProfileTime1 = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
		(m_lBufferLoadPos_X - m_lLoadPos_X), (m_lBufferLoadPos_X - m_lLoadPos_X), 
		HIPEC_SAMPLE_RATE, &m_stWLAxis_X);

	LONG lProfileTime2 = CMS896AStn::MotionGetProfileTime(WL_AXIS_X, "mpfWaferGripper", 
		(m_lBarcodePos_X - m_lLoadPos_X), (m_lBarcodePos_X - m_lLoadPos_X), 
		HIPEC_SAMPLE_RATE, &m_stWLAxis_X);

	LONG lDelay = 500;
	if (CMS896AStn::MotionIsServo(WL_AXIS_X, &m_stWLAxis_X) == TRUE)
		lDelay = lProfileTime1 - 200;		//Servo motor speed runs faster than stepper motor
	else
		lDelay = lProfileTime1 - 250;		//Stepper motor runs slower
	
	if (lDelay < WL_BARCODE_SCAN_MIN_TIME)
		lDelay = WL_BARCODE_SCAN_MIN_TIME;


	//Move Gripper to Scan bar code position
	X_MoveTo(m_lBufferLoadPos_X, SFM_NOWAIT);
	Sleep(lDelay);
	ReadBarcode(&szBarcodeName, 50, 5);	
	X_Sync();


	if (szBarcodeName.IsEmpty() == TRUE)
	{
		if ( m_lScanRange == 0 || bAllowSearch == FALSE)
		{
			return ERR_READ_BC_FAILED;
		}

		X_MoveTo(m_lBarcodePos_X, SFM_WAIT);
	
		for (i=0; i<1; i++)
		{
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
			{
				//CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode", TRUE);
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode(&szBarcodeName);
			X_Sync();

			if (szBarcodeName.IsEmpty() == FALSE)
			{
				return Err_No_Error;
			}

			if ( FilmFrameDriveIn(HP_POSITIVE_DIR, m_lScanRange*2, 200, FALSE) == FALSE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode(&szBarcodeName);
			X_Sync();

			if (szBarcodeName.IsEmpty() == FALSE)
			{
				return Err_No_Error;
			}

			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( FilmFrameDriveIn(HP_NEGATIVE_DIR, m_lScanRange * -1, 200, FALSE) == FALSE )
			{
				CMSLogFileUtility::Instance()->WL_LogStatus("Gripper is Jam @ scan barcode");
				SetErrorMessage("Gripper is Jam @ scan barcode");
				SetAlert_Red_Yellow(IDS_WL_GRIPPER_JAM);
				return Err_FrameJammed;
			}

			ReadBarcode(&szBarcodeName);
			X_Sync();

			if (szBarcodeName.IsEmpty() == FALSE)
			{
				return Err_No_Error;
			}
		}

		return ERR_READ_BC_FAILED;
	}

	return Err_No_Error;
}


VOID CWaferLoader::WaferBarcodeMoveScanTest()
{
	//static INT nMoveDirection = 1;

	if (m_lWaferBarcodeReverseCount < m_lWaferBarcodeTestCount)
	{	
		m_lWaferBarcodeTestCount = 0;
		m_nWaferBarcodeMoveDirection = m_nWaferBarcodeMoveDirection * -1;
	}

	GetEncoderValue();

	// check whether out of limit
	if ((m_nWaferBarcodeMoveDirection * m_lBarcodeTestMoveStep + m_lEnc_X) > 0)
	{
		return;
	}

	X_Move(m_nWaferBarcodeMoveDirection * m_lBarcodeTestMoveStep, SFM_WAIT);
	
	CString szNewBarcode = "";
	//static CString szPrevBarcode = "";

	if ( GetBCModel() == WL_BAR_SYMBOL || GetBCModel() == WL_BAR_DATALOGIC)
	{
		ReadBarcode(&szNewBarcode);
	}
	//else if (GetBCModel() == WL_BAR_REGAL)
	//{
	//	PurgeBarcode_Regal();
	//	ReadBarcode_Regal(&szNewBarcode, 5);
	//}
	else
	{
		int nCount=0;
		StartBarcode(TRUE);
		
		while(1)
		{
			ReadBarcode_Keyence(&szNewBarcode);
			if ( szNewBarcode.IsEmpty() == FALSE )
			{
				break;
			}

			if ( nCount++ > 5 )
			{
				break;
			}

			Sleep(10);
		}

		StartBarcode(FALSE);
	}

	CMSLogFileUtility::Instance()->WL_BarcodeTestLog(szNewBarcode);

	Sleep(100);
	m_lWaferBarcodeTestCount = m_lWaferBarcodeTestCount + 1;
}

LONG CWaferLoader::WaferBarcodeTest(IPC_CServiceMessage &svMsg)
{
	BOOL bStart = FALSE;
	BOOL bResult = TRUE;

	svMsg.GetMsg(sizeof(BOOL), &bStart);

	if (WPR_MoveFocusToSafe() == FALSE)
	{
		svMsg.InitMessage(sizeof(BOOL), &bResult);
		return 1;
	}

	if (bStart == TRUE)
	{
		m_bStartWaferBarcodeTest	= TRUE;
		m_lWaferBarcodeTestCount	= 0;
		m_nWaferBarcodeMoveDirection = 1;
		CMSLogFileUtility::Instance()->WL_BarcodeTestLog("Start Barcode Test");
		X_MoveTo(m_lBarcodePos_X, SFM_WAIT);
	}
	else
	{
		m_bStartWaferBarcodeTest = FALSE;
		CMSLogFileUtility::Instance()->WL_BarcodeTestLog("End Barcode Test");
	}

	svMsg.InitMessage(sizeof(BOOL), &bResult);
	return 1;
}

//v4.44A6	//Electech 3E (DL) Manual-Load-Map button
LONG CWaferLoader::AutoReadBarcodeOnTable(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;

	if (!m_bUseBarcode)
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return Err_No_Error;
	}

	if (IsWLExpanderOpen())
	{
		bReturn = FALSE;
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return Err_No_Error;
	}

	CMSLogFileUtility::Instance()->MS_LogOperation("AutoReadBarcode on WT in Manal-Load-Map (3E DL)");

	SetWaferTableJoystick(FALSE);
	LONG lX=0, lY=0, lT=0;
	GetWaferTableEncoder(&lX, &lY, &lT);
	Sleep(200);
	MoveWaferTable(m_lUnloadPhyPosX, m_lUnloadPhyPosY);

	INT nBarcodeStatus = ScanningBarcodeOnTable();
	if ( (nBarcodeStatus == TRUE) && (m_szBarcodeName.GetLength() > 0) )
	{
		INT nID = LoadMapFileWithoutSyn(m_szBarcodeName);
		SyncLoadMapFile(nID);
	}
	else
	{
		SetAlert_Red_Yellow(IDS_WL_NO_BARCODE);
		bReturn = FALSE;
		m_WaferMapWrapper.InitMap();
	}

	MoveWaferTable(lX, lY);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return Err_No_Error;
}

LONG CWaferLoader::WL1InputBarcodeLoadMap(IPC_CServiceMessage& svMsg)
{
	(*m_psmfSRam)["WaferTable"]["WT1InBarcode"] = "";
	m_szBarcodeName = "";

	CString szBarcode	= _T("");
	if( WL_InputBarcode(1, szBarcode) )
	{
		(*m_psmfSRam)["WaferTable"]["WT1InBarcode"] = szBarcode;
		m_szBarcodeName	= szBarcode;
		SaveBarcodeName();
		ES101LoadOfflineMapFile(szBarcode);
		WaitMapValid();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return Err_No_Error;
}

LONG CWaferLoader::WL2InputBarcodeLoadMap(IPC_CServiceMessage& svMsg)
{
	(*m_psmfSRam)["WaferTable"]["WT2InBarcode"] = "";
	m_szBarcodeName2 = "";

	CString szBarcode	= _T("");
	if( WL_InputBarcode(2, szBarcode) )
	{
		(*m_psmfSRam)["WaferTable"]["WT2InBarcode"] = szBarcode;
		m_szBarcodeName2	= szBarcode;
		ES101LoadOfflineMapFile(szBarcode);
		WaitMapValid();
	}

	BOOL bReply = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReply);
	return 1;
}

LONG CWaferLoader::LoadBarcodeInCassetteFile(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szFilename, szInitPath;
	CString szTitle, szContent;

	// Get the Main Windows
	CMS896AApp *pAppMod = dynamic_cast<CMS896AApp*>(m_pModule);
	CWnd *pMainWnd;

	static char szFilters[] =
		"Txt File (*.txt)|*.txt|All Files (*.*)|*.*||";

	if (pAppMod != NULL)
	{
		pMainWnd = pAppMod->m_pMainWnd;
	}
	else
	{
		pMainWnd = NULL;
	}

	// Create an Open dialog; the default file name extension is ".txt".
	CFileDialog dlgFile(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , 
						szFilters, pMainWnd, 0);

	szInitPath = gszUSER_DIRECTORY;
	dlgFile.m_ofn.lpstrInitialDir = szInitPath;
	dlgFile.m_ofn.lpstrDefExt = "txt";

	if (pAppMod != NULL)
	{
		pAppMod->ShowApp(TRUE);		// Set the application to TopMost, otherwise the dialog box will only show at background
	}
	
	INT nReturn = (INT)dlgFile.DoModal();	// Show the file dialog box

	if (pAppMod != NULL)
	{
		pAppMod->ShowHmi();		// After input, restore the application to background
	}

	if (nReturn != IDOK)
	{
		SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	szTitle.LoadString(HMB_WL_LOAD_CAS_FILE);

	SetCurrentDirectory(gszROOT_DIRECTORY + "\\Exe");
	m_szBCInCassetteFilename = dlgFile.GetPathName();
	
	if (SaveBarcodeInCassette(m_szBCInCassetteFilename) == FALSE)
	{
		m_szBCInCassetteFilename.Empty();
		szContent.LoadString(HMB_WL_LOAD_CAS_FILE_FAILED);
		HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	//SaveLastState();

	szContent.LoadString(HMB_WL_LOAD_CAS_FILE_COMPLETE);
	HmiMessage(szContent, szTitle, glHMI_MBX_CLOSE, glHMI_ALIGN_CENTER);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferLoader::ResetBarcodeInCassette(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	ResetBarcodeInCassette();
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}


LONG CWaferLoader::TestBarcode(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = TRUE;
	CString szBarcode;
	CString szText, szTitle;
	szTitle.LoadString(HMB_WL_BARCODE_RESULT);


	ReadStaticBarcode(szBarcode);

	if (szBarcode.IsEmpty() == FALSE)
	{
		szText.Format("%s", szBarcode);
	}
	else
	{
		szText.LoadString(HMB_WL_NO_BARCODE_READ);
	}


	HmiMessage(szText, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	//BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::TestBarcode2(IPC_CServiceMessage &svMsg)
{
	CString szBarcode;
	CString szText, szTitle;

	szTitle.LoadString(HMB_WL_BARCODE_RESULT);

	if (GetBCModel() == WL_BAR_SYMBOL || GetBCModel() == WL_BAR_DATALOGIC)
	{
		ULONG ulTimeOut = 100;
		ULONG ulRetry = 10;
		ReadBarcode2(&szBarcode, ulTimeOut, ulRetry);
	}
	else if (GetBCModel() == WL_BAR_DEFAULT)	//leo 20130723		//v4.40T10
	{
		ULONG ulTimeOut = 100;
		ULONG ulRetry = 10;
		//ReadBarcode2_Default(&szBarcode, ulTimeOut, ulRetry);
		ReadBarcode2_DefaultCom(&szBarcode, ulTimeOut, ulRetry);
	}
	else if (GetBCModel() == WL_BAR_DEFAULT_COM)					//v4.40T10
	{
		ULONG ulTimeOut = 100;
		ULONG ulRetry = 10;
		ReadBarcode2_DefaultCom(&szBarcode, ulTimeOut, ulRetry);
	}

	if (szBarcode.IsEmpty() == FALSE)
	{
		szText.Format("%s", szBarcode);
	}
	else
	{
		szText.LoadString(HMB_WL_NO_BARCODE_READ);
	}


	HmiMessage(szText, szTitle, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::ManualReadBarCodeOnTable(IPC_CServiceMessage& svMsg)
{
	BOOL bReturn = TRUE;
	CString szLog, szText;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (pApp->GetCustomerName() != "Testar")
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	BOOL bIsEnableWL = (BOOL)((LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"]);	
	if (!bIsEnableWL)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}

	if (!m_bUseBarcode || !m_bReadBarCodeOnTable)
	{
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return TRUE;
	}


	LONG lX=0, lY=0, lT=0;
	GetWaferTableEncoder(&lX, &lY, &lT);
HmiMessage("Manual Check BC: moving WT to UNLOAD ....");
	BOOL bTableStatus = MoveWaferTableNoCheck(m_lUnloadPhyPosX, m_lUnloadPhyPosY, TRUE, SFM_WAIT, FALSE);

	CString szBarcode = _T("");
HmiMessage("Manual Check BC: Scanning BC ....");
	if ( (ScanningBarcodeOnTableWithTheta(szBarcode) == TRUE) )
	{	
		CMSLogFileUtility::Instance()->MS_LogOperation("Manual Check Barcode On Table");

		if (m_WaferMapWrapper.IsMapValid())
		{
			CString szMapFileName = GetMapFileName();

			szLog.Format("Manual Read BC checking (testar): Map = %s, BC = %s", 
				(LPCTSTR) szMapFileName, (LPCTSTR) szBarcode);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

			if ( (szMapFileName.GetLength() > 0) && (szMapFileName.Find(szBarcode) == -1) )
			{
				SetAlert_Red_Yellow(IDS_WL_COMPARE_BC_FAILED);
				SetErrorMessage("Manual Compare Barcode Failed! Barcode: " + szBarcode 
					+ "; Map: " + szMapFileName);
				bReturn = FALSE;
			}
		}
	}
	else
	{
		//szText.LoadString(HMB_WL_NO_BARCODE_READ);
		//HmiMessageEx(szText, szTitle, glHMI_MBX_OK);	//, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);
		//bReturn = FALSE;
	}

HmiMessage("Manual Check BC: moving WT back ....");
	bTableStatus = MoveWaferTableNoCheck(lX, lY, FALSE, SFM_WAIT, FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return TRUE;
}

LONG CWaferLoader::ReadStaticBarcode(CString &szBarcode)
{
	BOOL bReturn = TRUE;

	if (GetBCModel() == WL_BAR_DEFAULT)
	{
		EnableKeyboardHook(TRUE);
		Sleep(2000);
		EnableKeyboardHook(FALSE);

		Sleep(300);
		szBarcode = GetAppKeyboard();
	}
	else if (GetBCModel() == WL_BAR_DEFAULT_COM)
	{
		ReadBarcode_DefaultCom(&szBarcode);
	}
	else if (GetBCModel() == WL_BAR_2D_BARCODE)
	{
		Read2DBarCodeFromWafer(&szBarcode);
	}
	else if (GetBCModel() == WL_BAR_SYMBOL || GetBCModel() == WL_BAR_DATALOGIC)
	{
		ReadBarcode(&szBarcode);
	}
	//else if (GetBCModel() == WL_BAR_REGAL)
	//{
	//	PurgeBarcode_Regal();
	//	ReadBarcode_Regal(&szBarcode);
	//}
	else
	{
		int nCount = 0;
		StartBarcode(TRUE);
		while (1)
		{
			ReadBarcode_Keyence(&szBarcode);
			if (szBarcode.IsEmpty() == FALSE)
			{
				break;
			}

			if (nCount++ > 5)
			{
				break;
			}

			Sleep(10);
		}
		StartBarcode(FALSE);
	}

	return TRUE;
}
