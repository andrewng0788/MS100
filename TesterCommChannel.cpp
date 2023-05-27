
#include "stdafx.h"
#include "TesterCommChannel.h"
#include <iostream>

#define STATE_SOT       (0x1)
#define STATE_ISRUNNING (0x2)
#define STATE_EOT       (0x4)


CTesterCommServer::CTesterCommServer()
{
	m_dwServerVer = TESTER_HANDLER_COMM_VERSION;
	m_dwClientVer = 0xBEEF;
	//Sequence must not change
	//NEW
	m_hRevisionDataFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0x0, sizeof(TesterHandlerCommRevision), TESTER_NAMED_FILE_MAPPING_VER);
	ASSERT(m_hRevisionDataFileMapping);

	m_pRevisionDataBuffer = MapViewOfFile(m_hRevisionDataFileMapping,FILE_MAP_ALL_ACCESS,0,0,0);
	ASSERT(m_pRevisionDataBuffer);

	TesterHandlerCommRevision dwVersion;
	dwVersion.dwServerVersion = m_dwServerVer;
	CopyMemory(m_pRevisionDataBuffer,&dwVersion,sizeof(TesterHandlerCommRevision));
	//NEW

	m_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0x0, sizeof(TesterData), TESTER_NAMED_FILE_MAPPING);
	ASSERT(m_hFileMapping);

	m_pBuffer = MapViewOfFile(m_hFileMapping,FILE_MAP_ALL_ACCESS,0,0,0);
	ASSERT(m_pBuffer);


	m_hSOT = CreateEvent(NULL,0,0,TESTER_NAMED_EVENT_SOT);
	ASSERT(m_hSOT);
	m_hIsRunning = CreateEvent(NULL,0,0,TESTER_NAMED_EVENT_ISRUNNING);
	ASSERT(m_hIsRunning);
	m_hEOT = CreateEvent(NULL,0,0,TESTER_NAMED_EVENT_EOT);
	ASSERT(m_hEOT);
	m_hIsRunningOn = CreateEvent(NULL,0,0,TESTER_NAMED_EVENT_ISRUNNING_ON);
	m_nState=0;
	m_bIsFirstRUN = true;
}

CTesterCommServer::~CTesterCommServer()
{
	UnmapViewOfFile(m_pBuffer);
	CloseHandle(m_hFileMapping);
	UnmapViewOfFile(m_pRevisionDataBuffer);
	CloseHandle(m_hRevisionDataFileMapping);
	CloseHandle(m_hSOT);
	CloseHandle(m_hIsRunning);
	CloseHandle(m_hEOT);

	std::map<unsigned int, HANDLE>::iterator it;
	for (it = m_hStationStartHOLDSignal.begin(); it != m_hStationStartHOLDSignal.end(); it++)
		 CloseHandle(m_hStationStartHOLDSignal[it->first]);

}

long CTesterCommServer::SetSOT()
{
	//if (m_bIsFirstRUN)
	//{ 
		//AfxMessageBox("m_bIsFirstRUN");
		//if (IsClientVersionWorkable() == false)
		//{
		//	CString msg;
		//	msg.Format("Tester Comm. Protocol Version is not UPDATED. Use version: V%X", TESTER_HANDLER_COMM_VERSION);
		//	AfxMessageBox(msg);
		//	throw logic_error( msg);
		//}
	//	m_bIsFirstRUN = false;
	//}
	SetEvent(m_hSOT);
	m_nState|=STATE_SOT;
	return 0;
}
bool CTesterCommServer::IsClientVersionFULLYWorkable()
{
	TesterHandlerCommRevision dwVersion;
	CopyMemory(&dwVersion,m_pRevisionDataBuffer,sizeof(TesterHandlerCommRevision)); 
	m_dwClientVer = dwVersion.dwClientVersion;
	if (dwVersion.dwClientVersion == 0xBEEF)
		return false;
	else if (dwVersion.dwServerVersion > dwVersion.dwClientVersion)
		return false;
	return true;
}

bool CTesterCommServer::CreateStationHOLDSignal(unsigned int nStationID)
{
	if (nStationID == 0)
		return false;
	else
	{

		if (m_hStationStartHOLDSignal.find(nStationID) == m_hStationStartHOLDSignal.end()) //not exists
		{
			CString csEventName;
			csEventName.Format("%s_ST%d",TESTER_NAMED_EVENT_STATION_HOLD,nStationID);
			/*
			HANDLE WINAPI CreateEvent(
			_In_opt_  LPSECURITY_ATTRIBUTES lpEventAttributes,
			_In_      BOOL bManualReset,
			_In_      BOOL bInitialState,
			_In_opt_  LPCTSTR lpName
			);
			*/
			m_hStationStartHOLDSignal[nStationID] =  CreateEvent(NULL,TRUE,FALSE,csEventName);
		}
	}
	return true;
}


bool CTesterCommServer::SetStationSTART(unsigned int nStationID)
{
	if (m_hStationStartHOLDSignal.find(nStationID) != m_hStationStartHOLDSignal.end())
		SetEvent(m_hStationStartHOLDSignal[nStationID]);
	else
		 return false;
	return true;
}

bool CTesterCommServer::ResetStationSTART(unsigned int nStationID)
{
	if (m_hStationStartHOLDSignal.find(nStationID) != m_hStationStartHOLDSignal.end())
		ResetEvent(m_hStationStartHOLDSignal[nStationID]);
	else
		return false;
	return true;
}





void CTesterCommServer::ResetSOT()
{
	ResetEvent(m_hSOT);
	m_nState|=STATE_SOT;
}

long CTesterCommServer::WaitForIsRunning(long timeout)
{
	return WaitForSingleObject(m_hIsRunning,timeout);
}

long CTesterCommServer::WaitForEOT(long timeout)
{
	return WaitForSingleObject(m_hEOT,timeout);
}
bool CTesterCommServer::SetTestData(const TesterData & testerData)
{
	if (m_bIsFirstRUN)
	{
		if ( (testerData.nDUT1ESDOritentation != -1) || testerData.nDUT2ESDOritentation !=- 1)
		{
			if (!IsClientVersionFULLYWorkable())
			{
				int nRet = AfxMessageBox("ESD Oritenation Is Not SUPPORTED! Yes - Ignore Warning; No - STOP", MB_YESNO|MB_ICONSTOP);
				if (nRet == IDYES)
					m_bIsFirstRUN = true;
				else
				{
					AfxMessageBox("Please Contact R&D to Update Tester Software");
					m_bIsFirstRUN = false;
				}

			}
			else
				m_bIsFirstRUN = false;
		}
		else
			m_bIsFirstRUN = false;
		
	}
	CopyMemory(m_pBuffer,&testerData,sizeof(TesterData));
	return true;
}

long CTesterCommServer::WaitForIsRunningOn(long timeout)
{
	return WaitForSingleObject(m_hIsRunning,timeout);
}

bool CTesterCommServer::GetTestData(TesterData* pTesterData)
{
	if (pTesterData!=NULL)
	{
		CopyMemory(pTesterData,m_pBuffer,sizeof(TesterData));
	}
	return true;
}

CTesterCommClient::CTesterCommClient()
{
	m_dwClientVer = TESTER_HANDLER_COMM_VERSION;
	m_dwServerVer = 0xBEEF;

	m_hFileMapping = NULL;
	m_pBuffer = NULL;
	m_hSOT = NULL;
	m_hIsRunning = NULL;
	m_hEOT = NULL;

	CString EventName;
	EventName.Format(_T("ASMTesterBreakSOTEvent%d"), 0);
	m_hBreakWaitSOT = CreateEvent(NULL,TRUE,FALSE,EventName);

	m_hFileMapping = 0;
	m_hSOT = 0;
	m_hIsRunning = 0;
	m_hEOT = 0;
	m_hIsRunningOn = 0;
	m_hRevisionDataFileMapping = NULL;
}
//0 - SERVER NOT EXIST
//1 - READY
//2 - SERVER TOO NEW
#define CONNECT_STATUS_SERVER_NOT_EXIST 0 
#define CONNECT_STATUS_READY            1 
#define CONNECT_STATUS_SERVER_TOO_NEW   2 

long CTesterCommClient::ConnectServer(int nRetry)
{
	int state = 0;
	while( --nRetry)
	{
		switch (state)
		{
		case 0:
			m_hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,0,TESTER_NAMED_FILE_MAPPING);
			if(m_hFileMapping!=NULL)
			{
				state=1;
			}
			else
				break;

		case 1:
			m_pBuffer = MapViewOfFile(m_hFileMapping,FILE_MAP_ALL_ACCESS,0,0,0);
			if (m_pBuffer != NULL)
			{
				TesterData stDefaultData;
				SetTestData(stDefaultData);
				m_hRevisionDataFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,0,TESTER_NAMED_FILE_MAPPING_VER);
				if (m_hRevisionDataFileMapping != NULL)
				{
					m_pRevisionDataBuffer = MapViewOfFile(m_hRevisionDataFileMapping,FILE_MAP_ALL_ACCESS,0,0,0);
					if (m_pRevisionDataBuffer != NULL)
					{
						TesterHandlerCommRevision dwVersion;
						CopyMemory(&dwVersion,m_pRevisionDataBuffer,sizeof(TesterHandlerCommRevision)); //Get from Buffer;
						m_dwServerVer = dwVersion.dwServerVersion;
						dwVersion.dwClientVersion = m_dwClientVer;
						CopyMemory(m_pRevisionDataBuffer,&dwVersion,sizeof(TesterHandlerCommRevision)); //Set back to Buffer;
						//Special NOte: 2016.01 - Product Group Samuel Chow use 0x0001 even in version supporting ESD Oritentation in his side 
						//                        in order to fake pass this checking of Tester 3.60.this checking is cancelled in future version
						//if (m_dwServerVer > m_dwClientVer)
						//	return CONNECT_STATUS_SERVER_TOO_NEW; //VERSION MIS-SMATCH 
					}
					else
						return CONNECT_STATUS_SERVER_NOT_EXIST;
				}
				else
				{
					m_dwServerVer = 0xBEEF; 
				}
				state=2;
			}
			else
				break;

		case 2:
			m_hSOT = OpenEvent(EVENT_ALL_ACCESS,0,TESTER_NAMED_EVENT_SOT);
			if(m_hSOT!=NULL)
				state=3;
			else
				break;

		case 3:
			m_hIsRunning = OpenEvent(EVENT_ALL_ACCESS,0,TESTER_NAMED_EVENT_ISRUNNING);
			if (m_hIsRunning != NULL)
				state=4; //-V112
			else
				break;
		case 4:
			m_hEOT = OpenEvent(EVENT_ALL_ACCESS,0,TESTER_NAMED_EVENT_EOT);
			if(m_hEOT!=NULL)
			{
				state=5;
			}
			else
			{
				break;
			}
		case 5:
			m_hIsRunningOn=OpenEvent(EVENT_ALL_ACCESS,0,TESTER_NAMED_EVENT_ISRUNNING_ON);
			if (m_hIsRunningOn!=NULL)
			{
				state = 6;
			}
			else
			{
				break;
			}
			return CONNECT_STATUS_READY;
		}
		Sleep(10);
	}
	//std::cerr<<"LED Tester Comm Init Error:"<< state<<std::endl;
	return CONNECT_STATUS_SERVER_NOT_EXIST;
}


void CTesterCommClient::DisconnectServer()
{
	CloseHandle(m_hBreakWaitSOT);
}

CTesterCommClient::~CTesterCommClient()
{
	if (m_hFileMapping != 0)
		CloseHandle(m_hFileMapping);
	if (m_hSOT != 0)
		CloseHandle(m_hSOT);
	if (m_hIsRunning != 0)
		CloseHandle(m_hIsRunning);
	if (m_hEOT != 0)
		CloseHandle(m_hEOT);
	if (m_hIsRunningOn != 0)
		CloseHandle(m_hIsRunningOn);


	if (m_hRevisionDataFileMapping != 0)
		CloseHandle(m_hRevisionDataFileMapping);

}

void CTesterCommClient::SetEOT()
{
	SetEvent(m_hEOT);
}

void CTesterCommClient::ResetEOT()
{
	ResetEvent(m_hEOT);
}

long  CTesterCommClient::WaitStationHOLD(unsigned int nStationID, long timeout)
{
	if (m_hStationStartHOLDSignal.find(nStationID) == m_hStationStartHOLDSignal.end()) //Key does not exist;
	{
		CString csEventName;
		csEventName.Format("%s_ST%d",TESTER_NAMED_EVENT_STATION_HOLD,nStationID);
		m_hStationStartHOLDSignal[nStationID] = OpenEvent(EVENT_ALL_ACCESS,0,csEventName);
	}
	if (m_hStationStartHOLDSignal[nStationID] == NULL)
	{
		return 0xDEAD;
	}
	else
	{

		return WaitForSingleObject(m_hStationStartHOLDSignal[nStationID],timeout);
	}
}

bool CTesterCommClient::ResetStationSTART(unsigned int nStationID)
{
	if (m_hStationStartHOLDSignal.find(nStationID) != m_hStationStartHOLDSignal.end())
		ResetEvent(m_hStationStartHOLDSignal[nStationID]);
	else
		 return false;
	return true;
}


void CTesterCommClient::ResetIsRunningOn()
{
	ResetEvent(m_hIsRunningOn);
}
void CTesterCommClient::SetIsRunningOn()
{
	SetEvent(m_hIsRunningOn);
}
long CTesterCommClient::WaitForSOT(long timeout)
{

	HANDLE hLocalEvents[2];
	hLocalEvents[0] = m_hSOT;
	hLocalEvents[1] = m_hBreakWaitSOT;
	DWORD dwWaitState;
	long ulState = -2;
	//WaitForSingleObject(m_hSOT,timeout);
	dwWaitState = WaitForMultipleObjects( 
		2,           // number of objects in array
		hLocalEvents,     // array of objects
		FALSE,       // wait all
		timeout);       // five-second wait

	switch (dwWaitState) 
	{
	case WAIT_OBJECT_0 + 0: 
		ulState = 0; //SOT
		break; 

	case WAIT_OBJECT_0 + 1: 
		ulState = 1; //BreakWaitSOT
		break; 

	case WAIT_TIMEOUT:
		ulState = -1;
		break;
	}

	return ulState;
}

void CTesterCommClient::BreakSOT()
{
	SetEvent(m_hBreakWaitSOT);
}

void CTesterCommClient::ResetWaitSOT()
{
	ResetEvent(m_hBreakWaitSOT);
}

void CTesterCommClient::SetIsRunning()
{
	SetEvent(m_hIsRunning);
}

void CTesterCommClient::ResetIsRunning()
{
	ResetEvent(m_hIsRunning);
}

bool CTesterCommClient::SetTestData(const TesterData & testerData)
{
	//if (testerData.)
	CopyMemory(m_pBuffer,&testerData,sizeof(TesterData));
	return true;
}

bool CTesterCommClient::GetTestData(TesterData* pTesterData)
{
	if (pTesterData!=NULL)
	{
		CopyMemory(pTesterData,m_pBuffer,sizeof(TesterData));
		return true;
	}
	return false;
}