#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinLoader.h"
#include "MS_SecCommConstant.h"	
#include "Utility.h"
//#ifndef MS_DEBUG
//	#include "mxml.h"	//v3.77
//#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// HAREWARE RELATIVE FUNCIONS----BAR CODE SCANNER
UCHAR CBinLoader::GetBCModel()
{
	return m_ucBarcodeModel;
}

//Create COM port for barcode scanning (COM 4)
BOOL CBinLoader::CreateBarcodeScanner(VOID)
{
	CString szLog;
	szLog.Format("BL: Create Barcode Model: %d",m_ucBarcodeModel);
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (m_oCommPort.IsCreated())
		return TRUE;

	//**Need to call this fcn after warm-start**//
	if ( GetBCModel() == BL_BAR_CCD || GetBCModel() == BL_BAR_REGAL )
	{
		m_oCommPort.Create(CMS896AApp::m_lBarCode2ComPort, 9600, 8, SFM_PARITY_NONE, 1, 5, 2000);
	}
	else
	{
		//Default SYMBOL/DATALOGIC com port settings
		m_oCommPort.Create(4, 9600, 7, SFM_PARITY_EVEN, 2, 5, 2000);
	}

	return m_oCommPort.IsCreated();
}

BOOL CBinLoader::CreateBarcodeScanner2(VOID)
{
	CString szLog;
	szLog.Format("BL2: Create Barcode Model: %d", GetBCModel());
	CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

	if (m_oCommPort2.IsCreated())
		return TRUE;

	//**Need to call this fcn after warm-start**//
	if ( GetBCModel() == BL_BAR_CCD )
	{
		m_oCommPort2.Create(CMS896AApp::m_lBarCode3ComPort, 9600, 8, SFM_PARITY_NONE, 1, 5, 2000);		//v4.35T4
	}
	else
	{
		//Default SYMBOL/DATALOGIC com port settings
		m_oCommPort2.Create(CMS896AApp::m_lBarCode3ComPort, 9600, 7, SFM_PARITY_EVEN, 2, 5, 2000);		//v4.35T4
	}

	return m_oCommPort2.IsCreated();
}


BOOL CBinLoader::ReadBarcode(CString *szData, int nTimeOut)
{
	BOOL bOnOff2DBarCodeScanner= FALSE; //4.54T22  

	if(GetBCModel() == BL_BAR_DATALOGIC)
	{
		bOnOff2DBarCodeScanner = TRUE;
	}

	char scTurnOn2DLaser[]  = { 0x02,0x31, 0x03};		//STX + "1" a+ STX +BS  is turn on 
	char scTurnOff2DLaser[] = { 0x02,0x32, 0x03};		//STX + "2" a+ STX +BS  is turn off 
	char scTurnOnLaser[]  = { 0x02, 0x53, 0x54, 0x49, 0x45, 0x03, 0x08 };
	char scReadData[50];
	char scOutputData[50];
	char scTempData[50];
	short i = 0;
	short j = 0;

	//Init variable
	szData->Empty();
	memset(scReadData,  NULL,   sizeof(scReadData));
	memset(scOutputData,NULL,   sizeof(scOutputData));
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

	/*
	try
	{
		//Read the Barcode value
		m_oCommPort.SetReadConstant((DWORD)nTimeOut);
		m_oCommPort.Read(scReadData,50);
		m_oCommPort.Close();
	}
	catch (CAsmException e)
	{
		//No need to display exception
	}
	*/


	INT nCount=0;
	INT nLen=0;

	//m_oCommPort.SetReadConstant((DWORD)nTimeOut);

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
		if ( nCount >= 5 )	//10 )	//v4.37
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

	szData->Format("%s",scOutputData);

    if( szData->IsEmpty() )
	{
        return FALSE;
	}

	return TRUE;
}


BOOL CBinLoader::StartBarcodeKeyence(BOOL bStart)
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

BOOL CBinLoader::ReadBarcodeValueKeyence(CString *szData)
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
		m_oCommPort.Read(scReadData, 50);
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

	szData->Format("%s",scOutputData);

    if( szData->IsEmpty() )
        return FALSE;
	return TRUE;
}



BOOL CBinLoader::StartBarcodeCCD(BOOL bStart)	//	On/Off CCD bar code scanner
{
	//unsigned char scTurnOnLaser[]  = { 0x02, 0x0D, 0x0A };		//"STX" + "CR" + "IF"
	//unsigned char scTurnOffLaser[] = { 0x03, 0x0D, 0x0A };		//"ETX" + "CR" + "IF"

	unsigned char Ondata[3];
	Ondata[0] = 0x02;
	Ondata[1] = 0x0D;
	Ondata[2] = 0x0A;

	unsigned char Offdata[3];
	Offdata[0] = 0x03;
	Offdata[1] = 0x0D;
	Offdata[2] = 0x0A;

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
			//Set scanner to serial-triggered mode
			unsigned char RobScArrData[20];
			int i=0;
			RobScArrData[i++] = 0x0D;
			RobScArrData[i++] = '$';
			RobScArrData[i++] = '+';
			RobScArrData[i++] = 'B';
			RobScArrData[i++] = 'K';
			RobScArrData[i++] = '0';
			RobScArrData[i++] = '$';
			RobScArrData[i++] = '-';
			RobScArrData[i++] = 0x0D;

			m_oCommPort.SetTimeOuts(5, 300);	//v2.93T2
			m_oCommPort.Write(RobScArrData, i);
			m_oCommPort.Write(Ondata, 3);	// Turn on scanner
		}
		else
		{
			m_oCommPort.Write(Offdata, 3);	//Turn off scanner
			m_oCommPort.Close();
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	return TRUE;
}


BOOL CBinLoader::ReadBarcodeValueCCD(CString *szData)
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

	szData->Format("%s",scOutputData);
	return TRUE;
}


BOOL CBinLoader::ClearComContentRegal()
{
	return TRUE;
#if 0
	if (m_oCommPort.IsOpen() == TRUE)
	{
		m_oCommPort.Purge();
	}
	
	return TRUE;
#endif
}


BOOL CBinLoader::ReadBarcodeValueRegal(CString *szData, int nTimeout, int nRetry)
{
	unsigned char sucTurnOnLaser[]  = { 0x7E, 0x80, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x82, 0x7E };		//"STX" + "LON" + "ETX"
	unsigned char sucTurnOffLaser[] = { 0x7E, 0x80, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x83, 0x7E };		//"STX" + "LOFF" + "ETX"

	char scReadData[50];
	char scOutputData[50];
	char scTempData[50];
	
	//Init variable
	szData->Empty();
	memset(scReadData,  NULL,   sizeof(scReadData));
	memset(scOutputData,NULL,   sizeof(scOutputData));
	memset(scTempData, NULL, sizeof(scTempData));

	//Check COM is created
	if (m_oCommPort.IsCreated() != TRUE)
	{
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
		return FALSE;
	}

	try
	{
		//Request Barcode On laser
		m_oCommPort.Write(sucTurnOnLaser, 10);
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}

	BOOL bRxMsg = FALSE;
	INT nCount=0;
	INT nLen = 0, nTempArrayCurPos = 0;

	m_oCommPort.SetReadConstant(nTimeout);

	while(1)
	{
		try
		{
			//Read data
			m_oCommPort.Read(scReadData, BL_BUFFER_SIZE);

			//check data end character
			nLen = (INT)strlen(scReadData) - 1;
			nLen = max(nLen, 0);

			bRxMsg = FALSE;

			if (nTempArrayCurPos < BL_BUFFER_SIZE)
			{
				for (int i=0; i<nLen; i++)
				{
					scTempData[nTempArrayCurPos] = scReadData[i];
					nTempArrayCurPos = nTempArrayCurPos + 1;
				}
			}

			for (int i=0; i<BL_BUFFER_SIZE; i++)
			{
				if ( scReadData[i] == '\r' ||  scReadData[i] == '\n')
				{
					bRxMsg = TRUE;
					strcpy_s(scReadData, sizeof(scReadData), scTempData);
					break;
				}
			}

			if (bRxMsg == TRUE)
			{
				break;
			}
			else
			{
				memset(scReadData,  NULL,   sizeof(scReadData));
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

	if (bRxMsg == FALSE)
	{
		m_oCommPort.Write(sucTurnOffLaser, 10);
		m_oCommPort.Close();
        return FALSE;
	}

	m_oCommPort.Close();

	INT nOutputCurPos = 0;

	//Decode data
	for (INT i = 0; i<BL_BUFFER_SIZE; i++)
	{
		if (scReadData[i] == '\r' || scReadData[i] == '\n')
		{
			break;
		}
		
		if ((scReadData[i] >= 0x20) && (scReadData[i] < 0x7E))
		{
			scOutputData[nOutputCurPos] = scReadData[i];
			nOutputCurPos = nOutputCurPos + 1;
		}
	}

	szData->Format("%s",scOutputData);

    if (szData->IsEmpty() == TRUE )
	{
		m_oCommPort.Write(sucTurnOffLaser, 10);
        return FALSE;
	}

	return TRUE;
}

BOOL CBinLoader::StartBarcodeRegal(BOOL bStart)
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


LONG CBinLoader::StartBarcodeScanner(BOOL bOn)
{
	BOOL bResult = FALSE;

	if (GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC)
	{
		return TRUE;
	}
	else if (GetBCModel() == BL_BAR_CCD)
	{
		StartBarcodeCCD(bOn);
	}
	else if (GetBCModel() == BL_BAR_REGAL)
	{
		return TRUE;
	}
	else
	{
		StartBarcodeKeyence(bOn);
	}
	
	return TRUE;
}

LONG CBinLoader::ReadBarcodeScanner(CString &szBarcode)
{
	BOOL bResult = FALSE;

	if (GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC)
	{
		bResult = ReadBarcode(&szBarcode);
	}
	else if (GetBCModel() == BL_BAR_CCD)
	{
		bResult = ReadBarcodeValueCCD(&szBarcode);
	}
	else if (GetBCModel() == BL_BAR_REGAL)
	{
		bResult = ReadBarcodeValueRegal(&szBarcode);
	}
	else
	{
		bResult = ReadBarcodeValueKeyence(&szBarcode);
	}
	
	return bResult;
}

LONG CBinLoader::ScanningBarcode(CBinGripperBuffer *pGripperBuffer, BOOL bScanTwice)	//	with gripper movement
{
	m_szBCName.Empty();
	if (GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC)
	{
		return ScanningBySymbol(pGripperBuffer, bScanTwice);
	}
	else if (GetBCModel() == BL_BAR_CCD)
	{
		return ScanningByCCD();
	}
	else if (GetBCModel() == BL_BAR_REGAL)
	{
		return ScanningByRegal();
	}
	else
	{
		return ScanningByKeyence();
	}
}


//	with gripper movement
BOOL CBinLoader::MoveGripperScanningPosn(CBinGripperBuffer *pGripperBuffer, INT siDirection, LONG lScanRange)
{
	if (GripperSearchScan(pGripperBuffer, siDirection, lScanRange) == FALSE)
	{
		SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_SCAN_BARCODE);
		//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);
		//SetErrorMessage("BL Gripper Jam");
		SetGripperState(pGripperBuffer, FALSE);
		SetFrameLevel(FALSE);
		BL_DEBUGBOX("Gripper is jammed (S1)");
		return FALSE;
	}

	if (IsFrameJam(pGripperBuffer->GetFrameJamSensorName()))
	{
		SetGripperJamAlarm(pGripperBuffer, IDS_BGP_UP_GRIPPER_JAM_SCAN_BARCODE);
		//SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
		//SetErrorMessage("BL Gripper Jam");
		SetGripperState(pGripperBuffer, FALSE);
		SetFrameLevel(FALSE);
		BL_DEBUGBOX("Gripper is jammed (S2)");
		return FALSE;
	}
	return TRUE;
}


LONG CBinLoader::MoveGripperScanningByBarcode(CBinGripperBuffer *pGripperBuffer, const BOOL bScanTwice, 
											  const INT siDirection, const LONG lScanRange, 
											  BOOL &bPerform2ndScan, CString &sz1stScanBC, BOOL &bResult)
{
	if (!MoveGripperScanningPosn(pGripperBuffer, siDirection, lScanRange))
	{
		return -1;
	}

	bResult = ReadBarcode(&m_szBCName, 50);
	GripperSync(pGripperBuffer);
	if (bResult == TRUE)
	{
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		BOOL bEnableScanTwiceFcn = pApp->GetFeatureStatus(MS896A_FUNC_BLBC_EMPTY_SCAN_TWICE);
		//v4.51A17	//XM SanAn by LeoLam
		if (bScanTwice && bEnableScanTwiceFcn)
		{
			if (!bPerform2ndScan)
			{
				BL_DEBUGBOX("scan barcode TWICE fcn: 1st: " + m_szBCName);
				bPerform2ndScan = TRUE;
				sz1stScanBC = m_szBCName;
				bResult = FALSE;
			}
			else
			{
				BL_DEBUGBOX("scan barcode TWICE fcn: 2nd: " + m_szBCName);
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}



LONG CBinLoader::ScanningBySymbol(CBinGripperBuffer *pGripperBuffer, BOOL bScanTwice)	//	with gripper movement
{
	short i = 0;
	BOOL bResult = TRUE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	if (!m_bUseBarcode)
	{
		return TRUE;
	}

	BOOL b1stScanBC = FALSE;
	BOOL bEnableScanTwiceFcn = pApp->GetFeatureStatus(MS896A_FUNC_BLBC_EMPTY_SCAN_TWICE);
	BOOL bPerform2ndScan = FALSE;
	CString sz1stScanBC = "";

	if (m_lBinLoaderConfig == BL_CONFIG_DL_WITH_BUFFER)
	{
		b1stScanBC = FALSE;
	}
	else
	{
		b1stScanBC = ReadBarcode(&m_szBCName);
	}

	if (b1stScanBC == FALSE)
	{
		if (m_lScanRange == 0)
		{
			m_szBCName = BL_DEFAULT_BARCODE;
			return FALSE;
		}

		for (i = 0; i < m_lTryLimits; i++)
		{
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			LONG lRet = MoveGripperScanningByBarcode(pGripperBuffer, bScanTwice, HP_NEGATIVE_DIR, -(i + 1) * m_lScanRange, bPerform2ndScan, sz1stScanBC, bResult);
			if (lRet == -1)
			{
				return -1;
			}
			if (lRet == TRUE)
			{
				break;
			}

			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			lRet = MoveGripperScanningByBarcode(pGripperBuffer, bScanTwice, HP_POSITIVE_DIR, (i + 1) * m_lScanRange * 2, bPerform2ndScan, sz1stScanBC, bResult);
			if (lRet == -1)
			{
				return -1;
			}
			if (lRet == TRUE)
			{
				break;
			}

			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			lRet = MoveGripperScanningByBarcode(pGripperBuffer, bScanTwice, HP_NEGATIVE_DIR, -(i + 1) * m_lScanRange, bPerform2ndScan, sz1stScanBC, bResult);
			if (lRet == -1)
			{
				return -1;
			}

			if (lRet == TRUE)
			{
				break;
			}
		}

		if (!bResult && (i >= m_lTryLimits))		//v4.37
		{
			m_szBCName = BL_DEFAULT_BARCODE;
			return FALSE;
		}
	}


	//v4.51A17
	if (bEnableScanTwiceFcn && bScanTwice)
	{
		CString szErr;
		//szErr.Format("(SANAN: EMPTY frame scan twice result: 1st (%s), 2nd (%s)",
		//				sz1stScanBC, m_szBCName);
		//BL_DEBUGBOX(szErr);

		if ( (sz1stScanBC.GetLength() > 0)	&& 
			 (m_szBCName.GetLength() > 0)	&& 
			 (sz1stScanBC != m_szBCName) )
		{
			szErr.Format("(SANAN: EMPTY frame barcode not matched in 2 scanning: 1st (%s), 2nd (%s)",
							sz1stScanBC, m_szBCName);
			HmiMessage_Red_Yellow(szErr, "BL Barcode Sanning");
			m_szBCName = BL_DEFAULT_BARCODE;
			return FALSE;
		}
	}

	if (m_bUseExtension == TRUE)
	{
		if (m_szExtName.IsEmpty() == FALSE)
		{
			m_szBCName = m_szBCName + "." + m_szExtName;
		}
	}

	return TRUE;
}


LONG CBinLoader::ScanningByRegal(VOID)	//	with gripper movement
{
	short i = 0;
	BOOL bResult;

	if (m_bUseBarcode == FALSE)
	{
		return TRUE;
	}

	ReadBarcodeValueRegal(&m_szBCName);
	
	if (m_szBCName.IsEmpty() == TRUE)
	{
		if (m_lScanRange == 0)
		{
			m_szBCName = BL_DEFAULT_BARCODE;
			return FALSE;
		}

		//ClearComContentRegal();

		//Move to -ve direction
		Sleep(100);
		if (BinGripperSearchScan(HP_NEGATIVE_DIR, m_lScanRange) == FALSE)
		{
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam");
			SetGripperState(FALSE);
			SetFrameLevel(FALSE);
			BL_DEBUGBOX("Gripper is jammed");
			return -1;
		}

		if (IsFrameJam())
		{
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam");
			SetGripperState(FALSE);
			SetFrameLevel(FALSE);
			BL_DEBUGBOX("Gripper is jammed");

			return -1;
		}

		bResult = ReadBarcodeValueRegal(&m_szBCName, 50);
		X_Sync();
		if (bResult == TRUE)
		{
			if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
			{
				m_szBCName = m_szBCName + "." + m_szExtName;
			}

			return TRUE;
		}

		//Loop start
		for (i = 0; i < m_lTryLimits; i++)
		{
			Sleep(100);
			if (BinGripperSearchScan(HP_POSITIVE_DIR, m_lScanRange * 2) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");
				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");
				return -1;
			}

			bResult = ReadBarcodeValueRegal(&m_szBCName, 50);
			X_Sync();
			if (bResult == TRUE)
			{
				break;
			}

			Sleep(100);
			if (BinGripperSearchScan(HP_NEGATIVE_DIR, m_lScanRange * 2) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				return -1;
			}

			bResult = ReadBarcodeValueRegal(&m_szBCName, 50);
			X_Sync();
			if (bResult == TRUE)
			{
				break;
			}
		}

		//Move to +ve direction
		if (bResult == FALSE)
		{
			Sleep(100);
			if (BinGripperSearchScan(HP_POSITIVE_DIR, m_lScanRange) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				return -1;
			}

			bResult = ReadBarcodeValueRegal(&m_szBCName, 50);
			X_Sync();
			if (bResult == TRUE)
			{
				if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
				{
					m_szBCName = m_szBCName + "." + m_szExtName;
				}
				return TRUE;
			}
		}

		if (i == m_lTryLimits)
		{
			m_szBCName = BL_DEFAULT_BARCODE;
			return FALSE;
		}
	}


	if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
	{
		m_szBCName = m_szBCName + "." + m_szExtName;
	}

	return TRUE;
}

LONG CBinLoader::ScanningByCCD(VOID)	//	with gripper movement
{
	short i = 0;
	BOOL bResult;


	if (m_bUseBarcode == FALSE)
	{
		return TRUE;
	}

	StartBarcodeCCD(TRUE);
	ReadBarcodeValueCCD(&m_szBCName);

	if (m_szBCName.IsEmpty() == TRUE)
	{
		if (m_lScanRange == 0)
		{
			m_szBCName = BL_DEFAULT_BARCODE;

			StartBarcodeCCD(FALSE);
			return FALSE;
		}

		//Move to -ve direction
		Sleep(100);
		if (BinGripperSearchScan(HP_NEGATIVE_DIR, m_lScanRange) == FALSE)
		{
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam");
			SetGripperState(FALSE);
			SetFrameLevel(FALSE);
			BL_DEBUGBOX("Gripper is jammed");

			StartBarcodeCCD(FALSE);
			return -1;
		}

		if (IsFrameJam())
		{
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam");
			SetGripperState(FALSE);
			SetFrameLevel(FALSE);
			BL_DEBUGBOX("Gripper is jammed");

			StartBarcodeCCD(FALSE);
			return -1;
		}

		bResult = ReadBarcodeValueCCD(&m_szBCName);
		X_Sync();
		if (bResult == TRUE)
		{
			if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
			{
				m_szBCName = m_szBCName + "." + m_szExtName;
			}

			StartBarcodeCCD(FALSE);
			return TRUE;
		}

		//Loop start
		for (i = 0; i < m_lTryLimits; i++)
		{
			Sleep(100);
			if (BinGripperSearchScan(HP_POSITIVE_DIR, m_lScanRange * 2) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeCCD(FALSE);
				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeCCD(FALSE);
				return -1;
			}

			bResult = ReadBarcodeValueCCD(&m_szBCName);
			X_Sync();
			if (bResult == TRUE)
			{
				break;
			}

			Sleep(100);
			if (BinGripperSearchScan(HP_NEGATIVE_DIR, m_lScanRange * 2) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeCCD(FALSE);
				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeCCD(FALSE);
				return -1;
			}

			bResult = ReadBarcodeValueCCD(&m_szBCName);
			X_Sync();
			if (bResult == TRUE)
			{
				break;
			}
		}

		//Move to +ve direction
		if (bResult == FALSE)
		{
			Sleep(100);
			if (BinGripperSearchScan(HP_POSITIVE_DIR, m_lScanRange) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeCCD(FALSE);
				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeCCD(FALSE);
				return -1;
			}

			bResult = ReadBarcodeValueCCD(&m_szBCName);
			X_Sync();
			if (bResult == TRUE)
			{
				if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
				{
					m_szBCName = m_szBCName + "." + m_szExtName;
				}

				StartBarcodeCCD(FALSE);
				return TRUE;
			}
		}

		if (i == m_lTryLimits)
		{
			m_szBCName = BL_DEFAULT_BARCODE;

			StartBarcodeCCD(FALSE);
			return FALSE;
		}
	}


	if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
	{
		m_szBCName = m_szBCName + "." + m_szExtName;
	}

	StartBarcodeCCD(FALSE);
	return TRUE;
}


LONG CBinLoader::ScanningByKeyence(VOID)	//	with gripper movement
{
	short i = 0;
	BOOL bResult;


	if (m_bUseBarcode == FALSE)
	{
		return TRUE;
	}

	StartBarcodeKeyence(TRUE);
	ReadBarcodeValueKeyence(&m_szBCName);

	if (m_szBCName.IsEmpty() == TRUE)
	{
		if (m_lScanRange == 0)
		{
			m_szBCName = BL_DEFAULT_BARCODE;

			StartBarcodeKeyence(FALSE);
			return FALSE;
		}

		//Move to -ve direction
		Sleep(100);
		if (BinGripperSearchScan(HP_NEGATIVE_DIR, m_lScanRange) == FALSE)
		{
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam");
			SetGripperState(FALSE);
			SetFrameLevel(FALSE);
			BL_DEBUGBOX("Gripper is jammed");

			StartBarcodeKeyence(FALSE);
			return -1;
		}

		if (IsFrameJam())
		{
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam");
			SetGripperState(FALSE);
			SetFrameLevel(FALSE);
			BL_DEBUGBOX("Gripper is jammed");

			StartBarcodeKeyence(FALSE);
			return -1;
		}

		bResult = ReadBarcodeValueKeyence(&m_szBCName);
		X_Sync();
		if (bResult == TRUE)
		{
			if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
			{
				m_szBCName = m_szBCName + "." + m_szExtName;
			}

			StartBarcodeKeyence(FALSE);
			return TRUE;
		}

		//Loop start
		for (i = 0; i < m_lTryLimits; i++)
		{
			Sleep(100);
			if (BinGripperSearchScan(HP_POSITIVE_DIR, m_lScanRange * 2) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeKeyence(FALSE);
				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeKeyence(FALSE);
				return -1;
			}

			bResult = ReadBarcodeValueKeyence(&m_szBCName);
			X_Sync();
			if (bResult == TRUE)
			{
				break;
			}

			Sleep(100);
			if (BinGripperSearchScan(HP_NEGATIVE_DIR, m_lScanRange * 2) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeKeyence(FALSE);
				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeKeyence(FALSE);
				return -1;
			}

			bResult = ReadBarcodeValueKeyence(&m_szBCName);
			X_Sync();
			if (bResult == TRUE)
			{
				break;
			}
		}

		//Move to +ve direction
		if (bResult == FALSE)
		{
			Sleep(100);
			if (BinGripperSearchScan(HP_POSITIVE_DIR, m_lScanRange) == FALSE)
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeKeyence(FALSE);
				return -1;
			}

			if (IsFrameJam())
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				StartBarcodeKeyence(FALSE);
				return -1;
			}

			bResult = ReadBarcodeValueKeyence(&m_szBCName);
			X_Sync();
			if (bResult == TRUE)
			{
				if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
				{
					m_szBCName = m_szBCName + "." + m_szExtName;
				}

				StartBarcodeKeyence(FALSE);
				return TRUE;
			}
		}

		if (i == m_lTryLimits)
		{
			m_szBCName = BL_DEFAULT_BARCODE;

			StartBarcodeKeyence(FALSE);
			return FALSE;
		}
	}

	if ((m_bUseExtension == TRUE) && (m_szExtName.IsEmpty() == FALSE))
	{
		m_szBCName = m_szBCName + "." + m_szExtName;
	}

	StartBarcodeKeyence(FALSE);
	return TRUE;
}


// ==========================================================================================================
// ==========================================================================================================

LONG CBinLoader::ScanningBarcode2(VOID)	//	with gripper movement
{
	if ( GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC)
	{
		return ScanningBySymbol2();
	}
	//else if (GetBCModel() == BL_BAR_CCD)
	//{
	//	return ScanningByCCD();
	//}
	//else
	//{
	//	return ScanningByKeyence();
	//}
	return FALSE;
}

LONG CBinLoader::ScanningBySymbol2(VOID)	//	with gripper movement
{
	short i = 0;
    BOOL bResult;

    if( m_bUseBarcode==FALSE )
        return TRUE;

	if( ReadBarcode2(&m_szBCName2)==FALSE )
	{
		if ( m_lScanRange == 0 )
		{
			m_szBCName2 = BL_DEFAULT_BARCODE;
            return FALSE;
		}

		for (i=0; i< m_lTryLimits; i++)
		{
#ifndef NU_MOTION
			if ( BinGripper2SearchScan(HP_NEGATIVE_DIR, m_lScanRange) == FALSE )
#else
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( BinGripper2SearchScan(HP_NEGATIVE_DIR, (m_lScanRange * -1)) == FALSE )
#endif
			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripperState(FALSE);
				SetFrameLevel(FALSE);
				BL_DEBUGBOX("Gripper is jammed");

				return -1;
			}

			if ( IsFrameJam2() )
			{
   				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper Jam");
				SetGripper2State(FALSE);
				SetFrameLevel2(FALSE);
				BL_DEBUGBOX("Gripper2 is jammed");

				return -1;
			}

			bResult = ReadBarcode2(&m_szBCName2, 50);
            X2_Sync();
            if( bResult==TRUE )
            {
				break;
            }

#ifndef NU_MOTION
			if ( BinGripper2SearchScan(HP_POSITIVE_DIR, m_lScanRange*2) == FALSE )
#else
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( BinGripper2SearchScan(HP_POSITIVE_DIR, m_lScanRange*2) == FALSE )
#endif

			{
				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper2 Jam");
				SetGripper2State(FALSE);
				SetFrameLevel2(FALSE);
				BL_DEBUGBOX("Gripper 2 is jammed");

				return -1;
			}

			if ( IsFrameJam2() )
			{
   				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper2 Jam");
				SetGripper2State(FALSE);
				SetFrameLevel2(FALSE);
				BL_DEBUGBOX("Gripper 2 is jammed");

				return -1;
			}

			bResult = ReadBarcode2(&m_szBCName2, 50);
            X2_Sync();
            if( bResult==TRUE )
            {
				break;
            }
#ifndef NU_MOTION
			if ( BinGripper2SearchScan(HP_NEGATIVE_DIR, m_lScanRange) == FALSE )
#else
			//NU NOTIOM: searching direction is determined by the sign of search distance(+/-)
			if ( BinGripper2SearchScan(HP_NEGATIVE_DIR, (m_lScanRange * -1)) == FALSE )
#endif
			{
   				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper2 Jam");
				SetGripper2State(FALSE);
				SetFrameLevel2(FALSE);
				BL_DEBUGBOX("Gripper 2 is jammed");

				return -1;
			}

			if ( IsFrameJam2() )
			{
   				SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
				SetErrorMessage("BL Gripper2 Jam");
				SetGripper2State(FALSE);
				SetFrameLevel2(FALSE);
				BL_DEBUGBOX("Gripper 2 is jammed");

				return -1;
			}

			bResult = ReadBarcode2(&m_szBCName2, 50);
            X2_Sync();
            if( bResult==TRUE )
            {
				break;
            }
		}

        if( i==m_lTryLimits )
        {
			m_szBCName2 = BL_DEFAULT_BARCODE;
            return FALSE;
        }
	}

    if( m_bUseExtension==TRUE )
    {
		if ( m_szExtName.IsEmpty() == FALSE )
		{
			m_szBCName2 = m_szBCName2 + "." + m_szExtName;
		}
    }

	return TRUE;
}

BOOL CBinLoader::ReadBarcode2(CString *szData, int nTimeOut)
{
	BOOL bOnOff2DBarCodeScanner = FALSE; //4.54T22  
	if(GetBCModel() == BL_BAR_DATALOGIC)
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
	memset(scReadData,  NULL,   sizeof(scReadData));
	memset(scOutputData,NULL,   sizeof(scOutputData));
	memset(scTempData,	NULL,	sizeof(scTempData));

	//Check COM is created
	if (m_oCommPort2.IsCreated() != TRUE)
	{
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
		return FALSE;
	}


	try
	{
		if( bOnOff2DBarCodeScanner )
		{
			//Request Barcode Off laser first
			m_oCommPort2.Write(scTurnOff2DLaser, 3);
			m_oCommPort2.Purge();

		//Request Barcode On laser
			//m_oCommPort2.SetTimeOuts(5, 200);
			m_oCommPort2.Write(scTurnOn2DLaser, 3);
		    m_oCommPort2.Purge();
	   }
	   else
	   {
	   		//Request Barcode On laser
			m_oCommPort2.Write(scTurnOnLaser, 7);
			m_oCommPort2.Purge();
	   }
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
		if ( nCount >= 10 )
		{
			break;
		}
	}



	try
	{	
		if( bOnOff2DBarCodeScanner )
		{
			//Request Barcode Off laser again
			m_oCommPort2.Write(scTurnOff2DLaser, 3);
			m_oCommPort2.Purge();
		}
	}
	catch (CAsmException e)
	{
		DisplayException(e);
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

	szData->Format("%s",scOutputData);

    if( szData->IsEmpty() )
        return FALSE;
	return TRUE;
}



LONG CBinLoader::StartBarCodeOnTable(BOOL bStart)
{
	BOOL bStatus = TRUE;

	if (bStart)
	{
		if ( GetBCModel() == BL_BAR_CCD )
		{
			bStatus = StartBarcodeCCD(TRUE);			// Turn on CCD BC scanner

		}
		else
		{
			bStatus = ReadBarcode(&m_szBCName);		//**Will consume 100ms timout here**
		}
	}
	else
	{
		if ( GetBCModel() == BL_BAR_CCD )
		{
			bStatus = ReadBarcodeValueCCD(&m_szBCName);

			if (bStatus && (!m_szBCName.IsEmpty()) && (m_bUseExtension == TRUE))
			{
				if ( m_szExtName.IsEmpty() == FALSE )
				{
					m_szBCName = m_szBCName + "." + m_szExtName;
				}
			}

			StartBarcodeCCD(FALSE);					// Turn off CCD BC scanner
		}
		else
		{
		}
	}

	return bStatus;
}


//================================================================
//   Created-By  : Andrew Ng
//   Date        : 6/2/2007 9:08:59 AM
//   Description : Scan barocde on buffer table after alignment
//   Remarks     : 
//================================================================
LONG CBinLoader::ScanningBarcodeOnBufferTable(VOID)
{
    if( m_bUseBarcode==FALSE )
        return TRUE;

	if ( GetBCModel() == BL_BAR_CCD )
	{
		return ScanningBarcodeOnBufferTableCCD();
	}
	
	if (GetBCModel() == BL_BAR_REGAL)
	{
		ReadBarcodeValueRegal(&m_szBCName, 10);
		
		if (m_szBCName.IsEmpty() == TRUE)
		{
			return FALSE;
		}

		if (m_bUseExtension == TRUE)
		{
			if ( m_szExtName.IsEmpty() == FALSE )
			{
				m_szBCName = m_szBCName + "." + m_szExtName;
			}
		}
		
		return TRUE;
	}

	BOOL bResult = TRUE;
	if( ReadBarcode(&m_szBCName)==FALSE )
	{
		int i = 0;

		for (i = 0; i < m_lTryLimits; i++)
		{
			Sleep(300);		//v2.78T2
			bResult = ReadBarcode(&m_szBCName);
            if ( bResult == TRUE )
			{
				break;
            }
		}

        if (i == m_lTryLimits)
        {
			m_szBCName = BL_DEFAULT_BARCODE;
            return FALSE;
        }
	}

    if (m_bUseExtension == TRUE)
    {
		if ( m_szExtName.IsEmpty() == FALSE )
		{
			m_szBCName = m_szBCName + "." + m_szExtName;
		}
    }

	return TRUE;
}


LONG CBinLoader::ScanningBarcodeOnBufferTableCCD(VOID)
{
    if( m_bUseBarcode==FALSE )
        return TRUE;

	BOOL bResult = TRUE;
	StartBarcodeCCD(TRUE);

	Sleep(300);		//v2.93T2
	if (( ReadBarcodeValueCCD(&m_szBCName)==FALSE) || m_szBCName.IsEmpty())		//v2.93T2
	{
		int i = 0;
		for (i = 0; i < m_lTryLimits; i++)
		{
			Sleep(200);
			bResult = ReadBarcodeValueCCD(&m_szBCName);
            if (bResult == TRUE)
			{
				break;
            }
		}

        if (i == m_lTryLimits)
        {
			m_szBCName = BL_DEFAULT_BARCODE;
			StartBarcodeCCD(FALSE);
            return FALSE;
        }
	}

    if (m_bUseExtension == TRUE)
    {
		if ( m_szExtName.IsEmpty() == FALSE )
		{
			m_szBCName = m_szBCName + "." + m_szExtName;
		}
    }

	StartBarcodeCCD(FALSE);
	return TRUE;
}

LONG CBinLoader::ExArmScanBarcodeOnGripper()	//	with motion complete and scan
{
	if( m_bUseBarcode==FALSE )
	{
        return TRUE;
	}

	StartBarcodeScanner(TRUE);
	
	if (ReadBarcodeScanner(m_szBCName) == TRUE)
	{
		StartBarcodeScanner(FALSE);
		return TRUE;
	}

	// with search
	INT i;
	LONG lSerachResult = 0;
	for (i =0 ; i<m_lTryLimits; i++)
	{
		lSerachResult = FALSE;

		if (BinGripperSearchScan(HP_POSITIVE_DIR, m_lScanRange) == FALSE)
		{
			SetAlert_Red_Yellow(IDS_BL_GRIPPER_JAM);		
			SetErrorMessage("BL Gripper Jam");
			SetGripperState(FALSE);
			SetFrameLevel(FALSE);
			BL_DEBUGBOX("Gripper is jammed");

			lSerachResult = -1;
		}
		else
		{
			X_Sync();

			lSerachResult = ReadBarcodeScanner(m_szBCName);
		}

		if (lSerachResult == TRUE)
		{
			StartBarcodeScanner(FALSE);
			return TRUE;
		}
		else if (lSerachResult == -1)
		{
			StartBarcodeScanner(FALSE);
			return FALSE;
		}
	}

	StartBarcodeScanner(FALSE);

	if (i == m_lTryLimits)
    {
		m_szBCName = BL_DEFAULT_BARCODE;
		
        return FALSE;
    }

	if (m_bUseExtension == TRUE)
    {
		if ( m_szExtName.IsEmpty() == FALSE )
		{
			m_szBCName = m_szBCName + "." + m_szExtName;
		}
    }
	
	return TRUE;
}

BOOL CBinLoader::ManualInputBarcode(CString *szBarcode)
{
	IPC_CServiceMessage srvMsg;
	CString szTemp;
	CString szResult;
	CHAR acPar[200];
	BOOL bResult;
	BOOL bMask = FALSE;
	BOOL bReturn = TRUE;
	int nIndex = 0;

	//Assign 1 HMI variable to be used
	szTemp = "szBLBarcodeValue";
	strcpy_s(acPar, sizeof(acPar), (LPCTSTR)szTemp);
	nIndex += szTemp.GetLength() + 1;
	
	//Append input box title
	szTemp.LoadString(HMB_BL_KEYIN_BARCODE);
	strcpy_s(&acPar[nIndex], sizeof(acPar) - nIndex,  (LPCTSTR)szTemp);
	nIndex += szTemp.GetLength() + 1;

	memcpy(&acPar[nIndex], &bMask, sizeof(BOOL));
	nIndex += sizeof(BOOL);
	memcpy(&acPar[nIndex], &bReturn, sizeof(BOOL));
	nIndex += sizeof(BOOL);

	CMS896AStn::m_oNichiaSubSystem.MLog_AddErrorCount("BL: ManualInputBarcode");		//v4.43T10
	CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(FALSE);		//anichia001

	srvMsg.InitMessage(nIndex, acPar);
	INT nConvID = m_comClient.SendRequest("HmiUserService", "HmiAlphaKeys", srvMsg);

	while (!m_comClient.ScanReplyForConvID(nConvID, 10))
	{
		Sleep(10);
	}

	m_comClient.ReadReplyForConvID(nConvID, srvMsg);
	m_comClient.ReadReplyForConvID(nConvID, srvMsg);
	CMS896AStn::m_oNichiaSubSystem.MLog_LogSortTime(TRUE);		//anichia001

	//Get reply 
	char* pBuffer = new char[srvMsg.GetMsgLen()];
	
	srvMsg.GetMsg(srvMsg.GetMsgLen(), pBuffer);

	//Get result
	memcpy(&bResult, pBuffer, sizeof(BOOL));

	if ( bResult == TRUE )
	{
		//Get input string
		*szBarcode = &pBuffer[sizeof(BOOL)]; 
	}
	else
	{
		szBarcode->IsEmpty();
	}

	delete[] pBuffer;

	return bResult;
}

//4.55T07
BOOL CBinLoader::AlertMsgForManualInputBarcode_NoSaveToRecord(CString& szBarcodeName, CString szErrMsg, 
															  BOOL bEnableInputBarcodeSelection, BOOL bAllowEmptyBarcode)
{	
	BOOL bExit = TRUE;
	BOOL bResult = TRUE;
	LONG lOption;
	LONG lAlarmStatus = GetAlarmLamp_Status();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_bSemitekBLMode )
	{
		bAllowEmptyBarcode = FALSE;
		bEnableInputBarcodeSelection = FALSE;
	}

	if (bEnableInputBarcodeSelection == TRUE)
	{
		lOption = SetAlert_Msg_Red_Back(IDS_BL_NOBARCODE_FOUND, szErrMsg, "No", "Yes", NULL, glHMI_ALIGN_LEFT);		
	}
	else
	{
		lOption = 2;
		SetAlert_Red_Back(IDS_BL_NOBARCODE_FOUND);		
	}

	if ( lOption == 2 )
	{	
		do
		{
			bExit = TRUE;

			if ( (bResult = ManualInputBarcode(&szBarcodeName)) == TRUE )
			{
			}

			// Prevent empty barcode
			if ((bAllowEmptyBarcode == FALSE) && ((szBarcodeName.IsEmpty() == TRUE) || (bResult == FALSE)))
				bExit = FALSE;
			
		}while(bExit == FALSE);
	}

	return TRUE;
}

BOOL CBinLoader::AlertMsgForManualInputBarcode(LONG lCurrentBlock,LONG lMagzNo,LONG lSlotNo,
												   CString& szBarcodeName,CString szErrMsg, 
												   BOOL bEnableInputBarcodeSelection, BOOL bAllowEmptyBarcode)
{	
	BOOL bExit = TRUE;
	BOOL bResult = TRUE;
	LONG lOption;
	LONG lAlarmStatus = GetAlarmLamp_Status();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( m_bSemitekBLMode )
	{
		bAllowEmptyBarcode = FALSE;
		bEnableInputBarcodeSelection = FALSE;
	}

	if (bEnableInputBarcodeSelection == TRUE)
	{
		lOption = SetAlert_Msg_Red_Back(IDS_BL_NOBARCODE_FOUND, szErrMsg, "No", "Yes", NULL, glHMI_ALIGN_LEFT);		
	}
	else
	{
		lOption = 2;
		SetAlert_Red_Back(IDS_BL_NOBARCODE_FOUND);		
	}

	if ( lOption == 2 )
	{	
		do
		{
			bExit = TRUE;

			if ( (bResult = ManualInputBarcode(&szBarcodeName)) == TRUE )
			{
				if( pApp->GetCustomerName()=="Semitek" )
					szBarcodeName = szBarcodeName.MakeUpper();
				m_stMgznRT[lMagzNo].m_SlotBCName[lSlotNo] = m_szBCName;
				SaveBarcodeData(lCurrentBlock, szBarcodeName, lMagzNo, lSlotNo);
			}

			// Prevent empty barcode
			if ((bAllowEmptyBarcode == FALSE) && ((szBarcodeName.IsEmpty() == TRUE) || (bResult == FALSE)))
				bExit = FALSE;
			
		}while(bExit == FALSE);
	}

	return TRUE;
}


LONG CBinLoader::BarcodeTest(IPC_CServiceMessage& svMsg)
{
	CString szBarcode;
	CString szText, szTitle;


	szTitle.LoadString(HMB_BL_BARCODE_RESULT);

	if ( GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC)
	{
		ReadBarcode(&szBarcode);
	}
	else if ( GetBCModel() == BL_BAR_CCD )		//v2.73
	{
		int nCount=0;
		StartBarcodeCCD(TRUE);
	
		while(1)
		{
			ReadBarcodeValueCCD(&szBarcode);
			if ( szBarcode.IsEmpty() == FALSE )
			{
				break;
			}

			if ( nCount++ > 5 )
			{
				break;
			}

			Sleep(10);
		}

		StartBarcodeCCD(FALSE);
	}
	else if (GetBCModel() == BL_BAR_REGAL)
	{
		ReadBarcodeValueRegal(&szBarcode);
	}
	else
	{
		int nCount=0;
		StartBarcodeKeyence(TRUE);
		while(1)
		{
			ReadBarcodeValueKeyence(&szBarcode);
			if ( szBarcode.IsEmpty() == FALSE )
			{
				break;
			}

			if ( nCount++ > 5 )
			{
				break;
			}

			Sleep(10);
		}
		StartBarcodeKeyence(FALSE);
	}

	if ( szBarcode.IsEmpty() == FALSE )
	{
		szText.Format("%s",szBarcode);
	}
	else
	{
		szText.LoadString(HMB_BL_NO_BARCODE_READ);
	}


	HmiMessage(szText, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CBinLoader::Barcode2Test(IPC_CServiceMessage& svMsg)
{
	CString szBarcode = _T("");
	CString szText, szTitle;

	szTitle.LoadString(HMB_BL_BARCODE_RESULT);

	if ( GetBCModel() == BL_BAR_SYMBOL || GetBCModel() == BL_BAR_DATALOGIC )
	{
		ReadBarcode2(&szBarcode);
	}

	if ( szBarcode.IsEmpty() == FALSE )
	{
		szText.Format("%s",szBarcode);
	}
	else
	{
		szText.LoadString(HMB_BL_NO_BARCODE_READ);
	}


	HmiMessage(szText, szTitle, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 400, 300, NULL, NULL, NULL, NULL);

	BOOL bReturn=TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CBinLoader::CopyCreeBinBcSummaryFile()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName() != "Cree" )
	{
		return TRUE;
	}

	//Search OutputSummary File in network path
	CString szFilename, szFolderPath;
	WIN32_FIND_DATA FileData;
	HANDLE hSearch;
	CString szFileName;

	hSearch = FindFirstFile(m_szCreeBinBcSummaryPath + "\\" + "*", &FileData);
	if ( hSearch == INVALID_HANDLE_VALUE ) 
	{ 
		SetErrorMessage("Copy CreeBinBcSummaryFile: Invliad SEARCH handle!");
		return FALSE;
	}

	//int nCount = 0;
	CString szTemp;
	BOOL bFileFound = FALSE;
	do 
	{
		// if it is a folder recurive call to remove file
		if (FileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;
        if ( (strcmp(FileData.cFileName, ".")==0) || (strcmp(FileData.cFileName, "..")==0) )
            continue;

		szFileName = FileData.cFileName;
		szFileName = szFileName.MakeLower();

		if (szFileName.Find("outputbinsummary_") != -1)
		{
			bFileFound = TRUE;
			break;
		}

	} while (FindNextFile(hSearch, &FileData));
	
	FindClose(hSearch);

	if (!bFileFound)
	{
		return TRUE;	//If file not exist, just return TRUE
	}


	CString szNetworkFile = m_szCreeBinBcSummaryPath + "\\" + szFileName;
	CString szBinSummaryFile;
	szBinSummaryFile = gszROOT_DIRECTORY + "\\Exe\\CreeBinBcSumary.txt";

	//Copy bin BC summary file to local HD whenever available from network
	BOOL bCopy = FALSE;
	TRY {
		BL_DEBUGBOX("Copy CREE Bin BC Summary File from: " + szNetworkFile);
		bCopy = CopyFile(szNetworkFile, szBinSummaryFile, FALSE);
	}
	CATCH (CFileException, e){
		SetErrorMessage("Copy CreeBinBcSummaryFile: COPYFILE exception");
		return FALSE;
	}
	END_CATCH

	//Delete bin BC summary file in network once after COPY
	TRY {
		if (bCopy && pApp->GetCustomerName() == "Cree")
		{
			BL_DEBUGBOX("Delete CREE Bin BC Summary File at: " + szNetworkFile);
			DeleteFile(szNetworkFile);
		}
	}
	CATCH (CFileException, e){
		SetErrorMessage("Copy CreeBinBcSummaryFile: DELETEFILE exception");
	}
	END_CATCH

	return TRUE;
}

BOOL CBinLoader::CheckBcHistoryInCurrLot(CONST LONG lBlock, CONST CString szBCName, CONST BOOL bLoad, CString& szErrCode)
{
	CString szOBarcode = szBCName;
	BOOL bPLLMRebel = FALSE;
	BOOL bCree		= FALSE;
	BOOL bSemitek	= FALSE;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	bPLLMRebel	= pApp->GetFeatureStatus(MS896A_FUNC_PPLM_SPECIAL_FCNS);
	bCree		= pApp->GetCustomerName() == "Cree";
	bSemitek	= pApp->GetCustomerName() == "Semitek";

	if( m_bCheckBarcode==FALSE )	// this function for PLLM and cree only
	{
		return TRUE;
	}


	if (bPLLMRebel)		//PLLM
	{
		//v4.42T12
	/*
		//v2.83T65
		CStringMapFile pSmfFile;
		BOOL bIsUsed = FALSE;
		if (pSmfFile.Open(MSD_O_BARCODE_FILE, FALSE, TRUE) == 1) 
		{
			bIsUsed = (BOOL)(LONG)(pSmfFile)[szOBarcode]["IsUsed"];
			pSmfFile.Update();
			pSmfFile.Close();
		}

		if (bIsUsed)
		{
			BL_DEBUGBOX("Duplicated BC found in current lot : " + szOBarcode);
			szErrCode = "Error: duplicated barcode = " + szOBarcode + " detected on buffer-table frame; please check and replace it!";
			return FALSE;	//Duplicated BC found in current lot!
		}
	*/			
		return TRUE;
	}


	//v4.40T14
	if (bSemitek)
	{
		CString szMachineNo = (*m_psmfSRam)["MS896A"]["MachineNo"];
		if ( (szMachineNo.GetLength() > 0) && (szBCName.Find(szMachineNo) == -1) )
		{
			CString szErr;
			szErr.Format("Semitek: Machine No = %s not found in BC (%s)", szMachineNo, szBCName);
			BL_DEBUGBOX(szErr);
			szErrCode = szErr;
			return FALSE;		//Machine No not found in bin frame barcode !!
		}

		CString szLog;
		szLog.Format("Semitek - CheckBcHistoryInCurrLot:  Bin #%ld, BC = %s", lBlock, (LPCTSTR) szBCName);
		BL_DEBUGBOX(szLog);

		BOOL bHasDie = FALSE;
		UCHAR ucBinGrade = (UCHAR) lBlock;
		IPC_CServiceMessage stMsg;
		stMsg.InitMessage(sizeof(UCHAR), &ucBinGrade);
		INT nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIfBlockHasDie", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bHasDie);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		//if( m_bCheckBCSkipEmpty && bLoad && (m_bUseEmptyFrame || bHasDie==FALSE) )
		//{
		//	return TRUE;
		//}

		if( !bLoad )
		{
			BL_DEBUGBOX("Samitek: Do Not Do Compare Barcode When Unload");
			return TRUE;
		}

		if((m_bUseEmptyFrame  == FALSE/*|| bHasDie==FALSE*/) )
		{
			BL_DEBUGBOX("This Frame isnt used for the first time");
			return TRUE;
		}

		//CopyCreeBinBcSummaryFile();
		BOOL bPathExist = _access(m_szCreeBinBcSummaryPath,0);
		CString szBinBarcodeFile;
		CString szNetworkFile = m_szCreeBinBcSummaryPath+ "\\outputbinsummary_" + szMachineNo +".txt";
		szBinBarcodeFile = "c:\\MapSorter\\Exe\\CreeBinBcSumary.txt";
		if (CopyFile(szNetworkFile,szBinBarcodeFile,FALSE) == FALSE)
		{
			if (bPathExist != -1)
			{
				HmiMessage_Red_Yellow("Get Barcode File From Network Fails!COPY FROM:" + szNetworkFile +";TO:" + szBinBarcodeFile);
			}
			BL_DEBUGBOX("Get Barcode File From Network Fails!");
			return TRUE;
		}


		CStdioFile oFile;
		if (oFile.Open(szBinBarcodeFile, CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText) != TRUE)
		{
			CString szErr = "Barcode Summary file not found at: " + szBinBarcodeFile;
			BL_DEBUGBOX(szErr);
			szErrCode = szErr;
			return TRUE;		//Still let program to continue if summary not exist
		}

		CStdioFile oCopy;
		CString szCopyFile = "c:\\MapSorter\\Exe\\CopyBinBcSumary.txt";
		if(oCopy.Open(szCopyFile,CFile::modeCreate|CFile::modeReadWrite) != TRUE)
		{
			CString szErr = "Barcode COPY file not found at: " + szCopyFile;
			BL_DEBUGBOX(szErr);
			szErrCode = szErr;
			return TRUE;		//Still let program to continue if summary not exist
		}

		CString szLine;
		CString szBinNo;
		CString szCode,szBar;
		szBinNo.Format("%d", lBlock);
		BOOL bFind = 0;
		while (oFile.ReadString(szLine) != FALSE)
		{
			int nLine = szLine.Find(",") ;
			if(nLine != -1)
			{
				szCode = szLine.Mid(nLine+1);
				szBar = szLine.Left(nLine);
			}
			if( szBar.CompareNoCase(szOBarcode)==0 && szCode == "0")
			{
				bFind = 1;
				BL_DEBUGBOX("Semitek Barcode file found: " + szBinNo + "," + szOBarcode  + ";Code:" + szCode);
				oCopy.WriteString(szOBarcode + ",1\n");
				//oFile.Close();
				//return TRUE;
			}
			else if(szBar.CompareNoCase(szOBarcode)==0 && szCode == "1")
			{
				bFind = 0;
				HmiMessage_Red_Yellow("This Barcode isnt used for the first time!");
				BL_DEBUGBOX("This Barcode:" + szOBarcode  + "isnt used for the first time!");
				oCopy.WriteString(szOBarcode + ",1\n");
			}
			else
			{
				oCopy.WriteString(szLine + "\n");
			}
		}
		oFile.Close();
		oCopy.Close();
		remove(szNetworkFile);

		if (CopyFile(szCopyFile,szNetworkFile,FALSE) == FALSE)
		{
			BL_DEBUGBOX("Update Barcode File TO Network Fails!COPY FROM:" + szCopyFile +";TO:" + szNetworkFile);
			HmiMessage_Red_Yellow("Update Barcode File TO Network Fails!");
		}
		remove(szBinBarcodeFile);
		remove(szCopyFile);

		if (bFind == 0)
		{ 
			BL_DEBUGBOX("Semitek Barcode file not found! - " + szBinNo + ","  + szOBarcode);
			szErrCode = "Error: Output frame barcode = " + szBinNo + ","  + szOBarcode + " not found in barcode file!";
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	if (bCree)				//Cree
	{
		CString szLog;
		szLog.Format("Cree - CheckBcHistoryInCurrLot:  Bin #%ld, BC = %s", lBlock, (LPCTSTR) szBCName);
		BL_DEBUGBOX(szLog);

		CopyCreeBinBcSummaryFile();
		CString szBinSummaryFile;
		szBinSummaryFile = gszROOT_DIRECTORY + "\\Exe\\CreeBinBcSumary.txt";
		CStdioFile oFile;

		BOOL bHasDie = FALSE;
		UCHAR ucBinGrade = (UCHAR) lBlock;
		IPC_CServiceMessage stMsg;
		int		nConvID = 0;
		stMsg.InitMessage(sizeof(UCHAR), &ucBinGrade);
		nConvID = m_comClient.SendRequest(BIN_TABLE_STN, "CheckIfBlockHasDie", stMsg);
		while (1)
		{
			if( m_comClient.ScanReplyForConvID(nConvID, 30000) == TRUE )
			{
				m_comClient.ReadReplyForConvID(nConvID, stMsg);
				stMsg.GetMsg(sizeof(BOOL), &bHasDie);
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if( m_bCheckBCSkipEmpty && bLoad && (m_bUseEmptyFrame || bHasDie==FALSE) )
		{
			return TRUE;
		}

		BOOL bXmlFormat = TRUE;
		CString szLine;
		if (oFile.Open(szBinSummaryFile, CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone) != TRUE)
		{
			CString szErr = "Barcode Summary file not found at: " + szBinSummaryFile;
			BL_DEBUGBOX(szErr);
			szErrCode = szErr;
			return TRUE;		//Still let program to continue if summary not exist
		}
		if (oFile.ReadString(szLine) != FALSE)
		{
			if (szLine.Find("Bin,LotID,MaterialName") != -1)
			{
				bXmlFormat = FALSE;
			}
		}
		oFile.Close();

		if( bXmlFormat )
		{
			return FALSE;	//v4.59A41
/*
#ifndef MS_DEBUG		//v4.47T5
			FILE *fp;
			mxml_node_t *tree;

			fp = fopen(szBinSummaryFile, "r");
			if (fp == NULL)
			{
				CString szErr = "Barcode Summary file not found at: " + szBinSummaryFile;
				BL_DEBUGBOX(szErr);
				szErrCode = szErr;
				return TRUE;			//Still let program to continue if summary XML file not exist
			}

			tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
			fclose(fp);
			
			if (!tree)
			{
				CString szErr = "Barcode Summary TREE cannot be created at: " + szBinSummaryFile;
				BL_DEBUGBOX(szErr);
				szErrCode = szErr;
				return FALSE;			//ABORT if tree memory cannot be created
			}

			mxml_node_t *node;
			CString szAttr1, szAttr2;
			node = mxmlFindElement(tree, tree, "OutputBin", "BinLabel", szOBarcode, MXML_DESCEND);
			if (!node)
			{
				CString szErr = "Barcode = " + szOBarcode + " cannot be found in XML file: " + szBinSummaryFile;
				BL_DEBUGBOX(szErr);
				szErrCode = szErr;
				return FALSE;			//ABORT if NODE not found -> no record in file!
			}

			szAttr1	= (LPCTSTR) node->value.element.attrs[0].value;		//BinNumber
			szAttr2	= (LPCTSTR) node->value.element.attrs[1].value;		//BinLabel

			//Check if BC is related t correct BinNumber in Summary file
			INT nBinNum = atoi((LPCTSTR) szAttr1);

			//v4.50A5	//Cree Dynamic GradeMapping Fcn support
			if ( IsMapDetectSkipMode() )
			{
				USHORT usOrigGradeMap = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(lBlock + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()) 
											- CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
				if (nBinNum != usOrigGradeMap)
				{
					CString szErr;
					szErr.Format("; Block=%ld, OrigGrade=%d", lBlock,  usOrigGradeMap);
					szErr = "Barcode = " + szOBarcode + " does not match with BinNumber = " + szAttr1 + szErr;
					BL_DEBUGBOX(szErr);
					szErrCode = szErr;
					return FALSE;			//ABORT if BC & BinNumber not match!
				}
			}
			else
			{
				if (nBinNum != lBlock)
				{
					CString szErr = "Barcode = " + szOBarcode + " does not match with BinNumber = " + szAttr1;
					BL_DEBUGBOX(szErr);
					szErrCode = szErr;
					return FALSE;			//ABORT if BC & BinNumber not match!
				}
			}

			//v4.42T10
			if (szOBarcode != szAttr2)
			{
				CString szErr = "Barcode = " + szOBarcode + " does not match with XML BinLabel = " + szAttr2;
				BL_DEBUGBOX(szErr);
				szErrCode = szErr;
				return FALSE;			//ABORT if BC & BinNumber not match!
			}

			mxmlDelete(tree);
			BL_DEBUGBOX("Cree BC XML Summary found: " + szOBarcode + " (" + szAttr2 + ")");
#endif
			return TRUE;	//OK to proceed
*/
		}
		else
		{
		//	Bin,LotID,MaterialName
		//	15,3215885SD,000WW100-1637
		//	CString szBinNo;
		//	szBinNo.Format("%d,", lBlock);
			CString szBinNo;
			oFile.Open(szBinSummaryFile, 
				CFile::modeNoTruncate|CFile::modeRead|CFile::shareDenyNone|CFile::typeText);
		/*	while (oFile.ReadString(szLine) != FALSE)
			{
				if( szLine.Find(szOBarcode) != -1 &&
					szLine.Find(szBinNo)!=-1 )
				{
					BL_DEBUGBOX("Cree BC Summary found: " + szBinNo + szOBarcode);
					oFile.Close();
					return TRUE;
				}
			}	*/
			while (oFile.ReadString(szLine) != FALSE)
			{
				CStringArray szDataList;
				CUtility::Instance()->ParseRawData(szLine, szDataList);
				szBinNo.Format("%d", lBlock);
				if( szDataList.GetSize()>=2 )
				{
					if( (szDataList.GetAt(0)).Find(szBinNo)!=-1 )
					{
						if ( (szDataList.GetAt(1)).CompareNoCase(szOBarcode)==0 )
						{
							BL_DEBUGBOX("Cree BC Summary found: " + szBinNo + "," + szOBarcode);
							oFile.Close();
							return TRUE;
						}
					}
				}
			}
			oFile.Close();
			BL_DEBUGBOX("Cree BC Summary not found! - " + szBinNo + ","  + szOBarcode);
			szErrCode = "Error output frame barcode = " + szBinNo + ","  + szOBarcode + " not found in summary file!";
			return FALSE;
		}
	}

	return TRUE;	//OK for other customers
}


BOOL CBinLoader::BL_CheckBcInMgzs(CONST LONG lBlock, CONST CString szBCName, BOOL bBURNIN, BOOL bUseBT2)
{
	if( m_bCheckBarcode == FALSE )	// this for osram only, check all barcodes in the BL mgzns
	{
		return TRUE;
	}

	//v4.08
	if ( (m_szBCName == "") || (m_szBCName == BL_DEFAULT_BARCODE) )
	{
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	//if( pApp->GetCustomerName() != "OSRAM" )
	//{
	//	return TRUE;
	//}

	CString szMsg;
	//szMsg.Format("BC %s, block %d, curr mgzn %d, slot %d", szBCName, lBlock, m_lCurrMgzn, m_lCurrSlot);
	//BL_DEBUGBOX(szMsg);
	szMsg.Format("BL_CheckBcInMgzs for Bin #%d, BC=" + szBCName, lBlock);
	BL_DEBUGBOX(szMsg);

	//v4.35T1
	LONG lCurrMgzn = 0;
	LONG lCurrSlot = 0;
	if (bUseBT2)
	{
		lCurrMgzn = m_lCurrMgzn2;
		lCurrSlot = m_lCurrSlot2;
	}
	else
	{
		lCurrMgzn = m_lCurrMgzn;
		lCurrSlot = m_lCurrSlot;
	}

	int i, j;
	for(i=0; i<MS_BL_MGZN_NUM; i++)
	{
		if( m_stMgznRT[i].m_lMgznUsage==BL_MGZN_USAGE_UNUSE ||
			m_stMgznRT[i].m_lMgznUsage==BL_MGZN_USAGE_EMPTY )
		{
			//BL_DEBUGBOX("BC In mgzn, unuse or empty");
			continue;
		}

		for(j=0; j<MS_BL_MAX_MGZN_SLOT; j++)
		{
			if( m_stMgznRT[i].m_lSlotUsage[j]==BL_SLOT_USAGE_UNUSE )
			{
				continue;
			}

			if( i==lCurrMgzn && j==lCurrSlot )
			{
				continue;
			}

			if( m_stMgznRT[i].m_SlotBCName[j] != szBCName )
			{
				continue;
			}

			if (!bBURNIN)
			{
				CString szErrCode;
				CString szText1, szText2;
				szText1.Format("Mgzn[%d]Slot[%d] Block[%d] BC(%s) same to", i, j, m_stMgznRT[i].m_lSlotBlock[j], szBCName);
				szText2.Format(" curr Mgzn[%d]Slot[%d] Block[%d] on BT(%d)", lCurrMgzn, lCurrSlot, lBlock, bUseBT2);
				szErrCode = szText1 + szText2;
				BL_DEBUGBOX(szErrCode);
				SetErrorMessage(szErrCode);

				CString szDispMsg;
				szDispMsg.Format("Current BIN #%ld barcode (%s) is already registered at BIN #%ld!  Please check.", 
										lBlock, szBCName, m_stMgznRT[i].m_lSlotBlock[j]);
				SetAlert_Msg_Red_Yellow(IDS_BL_SELECT_BARCODE, szDispMsg);
			
				m_bBinFrameCheckBCFail = TRUE;
			}

			//try
			//{
			//	SaveData();
			//}
			//catch(CFileException e)
			//{
			//	BL_DEBUGBOX("BL File Exception in check BC in mgzns");
			//}

			return FALSE;	//TRUE;		//v4.08
		}
	}

	return TRUE;
}

LONG CBinLoader::CompareBarcodeFromEmptyMgzs(CONST LONG lBlock, CONST CString szBCName, BOOL bBURNIN, BOOL bUseBT2, BOOL bCheckEmptyFrame)
{

	LONG lSlotBlock;
	CString szMsg;
	CString szPartOfBarcode,szLastSecondLetter;
	LONG lCurrMgzn = 0;
	LONG lCurrSlot = 0;
	INT nIndex;
	
	// this for Macom only, check all barcodes in the BL empty mgzns
	if( m_bCheckBarcode == FALSE || bCheckEmptyFrame == FALSE)	
	{
		szMsg.Format("Not open Barcdoe checking for Empty Mgz: (%d, %d)",m_bCheckBarcode , bCheckEmptyFrame);
		BL_DEBUGBOX(szMsg);
		HmiMessage_Red_Yellow(szMsg);

		return TRUE;
	}

	if ( (m_szBCName == "") || (m_szBCName == BL_DEFAULT_BARCODE) )
	{
		szMsg.Format("Read barcode fail : %s",m_szBCName);
		BL_DEBUGBOX(szMsg);
		HmiMessage_Red_Yellow(szMsg);
		return TRUE;
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if( pApp->GetCustomerName() != "Macom" )
	{
		szMsg.Format("Wrong CTM Use the checking: %s",pApp->GetCustomerName());
		BL_DEBUGBOX(szMsg);
		HmiMessage_Red_Yellow(szMsg);
		return TRUE;
	}



	szMsg.Format("Compare Barcode From Empty Mgzs, Bin #%d, BC= %s", lBlock, szBCName);
	BL_DEBUGBOX(szMsg);



	if (bUseBT2)
	{
		lCurrMgzn = m_lCurrMgzn2;
		lCurrSlot = m_lCurrSlot2;
	}
	else
	{
		lCurrMgzn = m_lCurrMgzn;
		lCurrSlot = m_lCurrSlot;
	}



	lSlotBlock = m_stMgznRT[lCurrMgzn]. m_lSlotBlock[lCurrSlot];

	if( lSlotBlock <= 0 || lSlotBlock > 41)
	{	
		szMsg.Format("Error: Slot Block %d (ASM Grade) is not between 1 and 41", lSlotBlock);
		BL_DEBUGBOX(szMsg);
		SetErrorMessage(szMsg);
		HmiMessage_Red_Yellow(szMsg);	
		return FALSE;
	}

	// A	B	C	D	E	F	G	H	I	NA	8	9	NA	0	NA	NA
	//	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	

	//J		K	L	M	N	O	P	Q	R	S	T	NA	U	V	W	X	Y	Z
	//17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34


	//1		2	3	4	5	6	7
	//35	36	37	38	39	40	41


	
	//char cCode[40][10] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
	//	"1","2","3","4","5","6","7","8","9","0"};

	char cCode[41][10] = {"A","B","C","D","E","F","G","H","I","NA", "8","9", "NA", "0","NA","NA","J","K","L","M","N","O","P","Q","R","S","T","NA","U","V","W","X","Y","Z","1","2","3","4","5","6","7"};//4.55T16

	szPartOfBarcode = cCode[lSlotBlock-1];
	nIndex = szBCName.GetLength();
	szMsg.Format("For BC comparison, SlotBlock(ASM Grade):%d find the related PartOfBarcode(Output Tape):%s, Barcode Length:%d",lSlotBlock, szPartOfBarcode, nIndex);
	BL_DEBUGBOX(szMsg);

	szLastSecondLetter = szBCName.GetAt(nIndex-2); //last second letter of barcode value
	//4.55T18
	szMsg.Format("Comparing Frame BC= %s by the last second letter %s (block=%d curr M%d, S%d, B%d) with the part of BC = %s", szBCName, szLastSecondLetter,lBlock, lCurrMgzn, lCurrSlot,lSlotBlock,szPartOfBarcode);
	BL_DEBUGBOX(szMsg);

	if( szPartOfBarcode == "NA")
	{	
		szMsg.Format("Error: ASM Grade %d is NA", lSlotBlock);
		BL_DEBUGBOX(szMsg);
		SetErrorMessage(szMsg);
		HmiMessage_Red_Yellow(szMsg);	
		return FALSE;
	}

	if(szLastSecondLetter == szPartOfBarcode)
	{
		return TRUE;
	}
	else
	{
		BOOL  bHaveMatchedEmptyFrameSlot = FALSE;
		LONG lTempSlotBlock = 0;
		INT i,j;

		szMsg.Format("BC check fail(M%d S%d B%d on BT%d), %s cannot match last second letter=%s from BC =%s", lCurrMgzn, lCurrSlot, lBlock, bUseBT2,szPartOfBarcode,szLastSecondLetter, szBCName);
		BL_DEBUGBOX(szMsg);
		SetErrorMessage(szMsg);

		m_bBinFrameCheckBCFail = TRUE;
		

		//4.55T09 // search the empty frame in empty magazine again
		for (i = 0; i < MS_BL_MGZN_NUM; i++) 
		{
			if (m_stMgznRT[i].m_lMgznUsage == BL_MGZN_USAGE_EMPTY)
			{
				
				for (j = 0; j < MS_BL_MGZN_SLOT; j++)
				{
					lTempSlotBlock = m_stMgznRT[i].m_lSlotBlock[j];

					if (m_stMgznRT[i].m_lSlotUsage[j] != BL_SLOT_USAGE_EMPTY || lSlotBlock != lTempSlotBlock)
					{
						continue;
					}
					else
					{
						bHaveMatchedEmptyFrameSlot = TRUE;

						szMsg.Format("Have found the Next Matched empty frame M%d S%d B%d",i,j, lSlotBlock);
						BL_DEBUGBOX(szMsg);

						break;
					}
				}
			}

		}

		if(bHaveMatchedEmptyFrameSlot)
		{
			BL_DEBUGBOX("Search the empty frame in empty magazine again");
			return BL_EMPTY_FRAME_BC_CHECK_AGAIN;  //4.55T09 // for checking again
		}
		else
		{
			szMsg.Format("Error: Final %s cannot match last second letter=%s from Barcode =%s", szPartOfBarcode,szLastSecondLetter, szBCName);
			BL_DEBUGBOX(szMsg);
			SetErrorMessage(szMsg);
			SetAlert_Msg_Red_Yellow(IDS_BL_SELECT_BARCODE, szMsg);
			return FALSE;
		}
		
	}

	return TRUE;
}

//4.54T23 //4.55T07
BOOL CBinLoader::CheckBarcodeCharacters(CString szBarcode)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (!(pApp->GetCustomerName() == "SanAn" && pApp->GetProductLine() == "WH"))	
	{
		return TRUE;
	}

	UCHAR ucDigit = 0;
	CString szMsg;

	for (INT i = 0; i < szBarcode.GetLength(); i++)
	{
		//szBarcode = szBarcode.MakeUpper();  //only lower code as messy code by Wong Yi Ming 20161212
		ucDigit	  = szBarcode.GetAt(i);
		if (ucDigit < '0' || ucDigit >'9')
		{
			if (ucDigit < 'A' ||  ucDigit > 'Z')
			{
				szMsg.Format("Error: Barcode Characters is not just in 0~9 or A~Z: %s ", szBarcode);
				SetErrorMessage(szMsg);
				HmiMessage_Red_Yellow(szMsg);
				return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CBinLoader::ValidateBinFrameBarcode(CString szBarcode)
{
	CString szBarcodePrefixTemp = "";

	// cannot scan barcode case, meaningless to check
	if (m_szBCName == BL_DEFAULT_BARCODE)
	{
		if( m_bSemitekBLMode )
		{
			SetErrorMessage("Read bar code is unknown");
			BL_DEBUGBOX("Read bar code is unknown");
		}
		return TRUE;
	}

	if ( (m_lBinBarcodeLength <= 0) && m_szBinBarcodePrefix.IsEmpty() )		//v4.52A3
	{
		return TRUE;	
	}

	// check barcode length
	if (m_lBinBarcodeLength > 0)	//v4.52A3
	{
		if ((INT)m_lBinBarcodeLength != szBarcode.GetLength())
		{
			SetErrorMessage("BL: barcode length is not matched in ValidateBinFrameBarcode - " + szBarcode);
			SetAlert_Red_Yellow(IDS_BL_CHECK_BC_LENGTH_FAIL);
			return FALSE;
		}
	}


	//v4.39T8
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Cree")	
	{
		CHAR cDigit;
		LONG lNoOfDigitsInBC = atol((LPCTSTR) m_szBinBarcodePrefix);

		CString szTemp;
		szTemp.Format("Cree: perform BL BC prefix validation - Length=%d, Prefix=%d, BC=" + szBarcode, 
					m_lBinBarcodeLength, lNoOfDigitsInBC);
		CMSLogFileUtility::Instance()->MS_LogOperation(szTemp);

		if ((lNoOfDigitsInBC > 0) && (lNoOfDigitsInBC <= m_lBinBarcodeLength))
		{
			for (INT m=0; m<lNoOfDigitsInBC; m++)
			{
				cDigit = (CHAR) szBarcode.GetAt(m);
				if ((cDigit < '0') || (cDigit > '9'))
				{
					HmiMessage_Red_Yellow("Bin barcode prefix (" + szBarcode + ") contiains non-numeric character!", "Validate Barcode Prefix");
					SetErrorMessage("BL: Bin barcode prefix contiains non-numeric characters - " + szBarcode);
					return FALSE;
				}
			}

			for (INT n=lNoOfDigitsInBC; n<szBarcode.GetLength(); n++)
			{
				cDigit = (CHAR) szBarcode.GetAt(n);
				if ((cDigit >= '0') && (cDigit <= '9'))
				{
					HmiMessage_Red_Yellow("Bin barcode suffix (" + szBarcode + ") contiains numeric character!", "Validate Barcode Prefix");
					SetErrorMessage("BL: Bin barcode suffix contiains numeric characters - " + szBarcode);
					return FALSE;
				}
			}
		}

		return TRUE;
	}


	// check barcode prefix
	if (m_szBinBarcodePrefix.IsEmpty() == FALSE)
	{
		if (szBarcode.GetLength() < m_szBinBarcodePrefix.GetLength())
		{
			SetAlert_Red_Yellow(IDS_BL_CHECK_BC_PREFIX_FAIL);
			return FALSE;
		}

		szBarcodePrefixTemp = szBarcode.Left(m_szBinBarcodePrefix.GetLength());

		if (szBarcodePrefixTemp != m_szBinBarcodePrefix)
		{
			SetAlert_Red_Yellow(IDS_BL_CHECK_BC_PREFIX_FAIL);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CBinLoader::CheckBarcodeInMES(CONST LONG lBlock, CONST CString szBCName)
{
	if (!CMESConnector::Instance()->IsMESConnectorEnable())
		return TRUE;

	//CString szCmdID = MES_CHECK_WAFER_ID_ID;
	CString szCmdID;
	//szCmdID.Format("%ld", lBlock);
	szCmdID = "7";		//v4.43T4
	CString szMsg	= szBCName;	//szFilename + "@" + szPKGFile + "@" + szMapDate + "\n" + MES_OPERATOR_ID_INFO + "@" + szOperatorID;

	CMESConnector::Instance()->SendMessage(szCmdID, szMsg);
	
	INT nCode = CMESConnector::Instance()->RecvMessage(szMsg);

	if (nCode != TRUE)
	{
		//v4.43T4
		HmiMessage_Red_Yellow("Check MES bin barcode failure - " + szBCName + "\n\n" + szMsg,	//v4.49A10
								"Check MES");
		
		CString szErr;
		szErr.Format("BL: CheckBarcodeInMES fail (BC = %s) - " + szMsg, (LPCTSTR) szBCName);
		SetErrorMessage(szErr);
		return FALSE;			//MES_NG_RX or MES_TIME_OUT_RX
	}
	
	return TRUE;
}

