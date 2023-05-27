#pragma once
#include "Afxmt.h"
#include <map>

 //Update this when ever there is change 
#define TESTER_HANDLER_COMM_VERSION 0x0003 


#define TESTER_ORIENTATION_0_DEGREE   0
#define TESTER_ORIENTATION_180_DEGREE 180
#define TESTER_ORIENTATION_AUTO       -1

//***Update Strategy****
//Tester side must be newer than Handler side in order to support FULL function
//otherwise, basic function are served only.
//Full Function : SOT, EOT, SendBin Number, ESD Oritnetation
//Basic Function: SOT, EOT, SendBin Number,
//for every initialization, When handler(server) side is newer than tester(client) side,
//A warning box will be prompted asking user to ignore the warning or force update tester software.


struct TesterDataV0000
{
	//Initial Version
	//V0000
	int nBinNumber;
	int nDUT1Present;
	int nDUT2Present;

	// feedback from tester
	int nTestPass;    //0 = FAIL, 1 = PASS
    int nOrientation; //degree
	// feedback from tester

	TesterDataV0000()
	{
		nBinNumber   = 0;
		nDUT1Present = 0;
		nDUT2Present = 0;
		nTestPass    = 0;
		nOrientation = 0;
	};

};

struct TesterDataV0001 :  TesterDataV0000
{
	int nDUT1ESDOritentation;  
	int nDUT2ESDOritentation; 

	
	TesterDataV0001()
	{
		nDUT1ESDOritentation = -1; 
		nDUT2ESDOritentation = -1;
	};

};

struct TesterDataV0002 :  TesterDataV0001
{
	int nDUT1TestSetNumber;	//test set number for the first station
	int nDUT2TestSetNumber; //test set number for the second station
	
	TesterDataV0002()
	{
		TesterDataV0000();
		TesterDataV0001();
		nDUT1TestSetNumber = 0;
	    nDUT2TestSetNumber = 0;
	};

};


//V0002 
struct TesterData :  TesterDataV0002
{
	//Tester will reset this to -1 each cycle
							//-1 - follow tester auto detected
							//0   - 0 Degree
							//180 - 180 degree
	TesterData()
	{
		TesterDataV0002();
	}

};

//must not change - dedicated for Version Check
struct TesterHandlerCommRevision
{
	DWORD dwServerVersion;
	DWORD dwClientVersion;    
	TesterHandlerCommRevision()
	{
		dwServerVersion = 0xBEEF;
		dwClientVersion = 0xBEEF;
	}
} ;


#define TESTER_NAMED_FILE_MAPPING        "TESTER_NAMED_FILE_MAPPING"
#define TESTER_NAMED_FILE_MAPPING_VER    "TESTER_NAMED_FILE_MAPPING_VER"
#define TESTER_NAMED_EVENT_SOT           "TESTER_NAMED_EVENT_SOT"
#define TESTER_NAMED_EVENT_ISRUNNING     "TESTER_NAMED_EVENT_ISRUNNING"
#define TESTER_NAMED_EVENT_EOT           "TESTER_NAMED_EVENT_EOT"
#define TESTER_NAMED_EVENT_ISRUNNING_ON  "TESTER_NAMED_EVENT_ISRUNNING_ON"

//will be appened with station ID, starting from 1
//e.g. "TESTER_NAMED_EVENT_STATION_HOLD_ST1" "TESTER_NAMED_EVENT_STATION_HOLD_ST2"
#define TESTER_NAMED_EVENT_STATION_HOLD  "TESTER_NAMED_EVENT_STATION_HOLD" 


class CTesterCommServer
{
public:
	CTesterCommServer();
	~CTesterCommServer();

	long SetSOT();
	void ResetSOT();
	long WaitForIsRunning(long timeout=INFINITE);
	long WaitForIsRunningOn(long timeout=INFINITE);
	long WaitForEOT(long timeout=INFINITE);
	bool SetTestData(const TesterData &);
	bool GetTestData(TesterData*);


	bool IsClientVersionFULLYWorkable(); //assume client is connected to server
	bool CreateStationHOLDSignal(unsigned int nStationID); //1 as station 1, 2 as station 2
	bool SetStationSTART(unsigned int nStationID); //1 as station 1, 2 as station 2
	bool ResetStationSTART(unsigned int nStationID); //1 as station 1, 2 as station 2

//private:
	HANDLE m_hSOT, m_hIsRunning, m_hEOT, m_hIsRunningOn;
	HANDLE m_hFileMapping;
	HANDLE m_hRevisionDataFileMapping;
	void* m_pBuffer;
	void* m_pRevisionDataBuffer;
	DWORD m_dwClientVer;
	DWORD m_dwServerVer;
	int m_nState;
	bool m_bIsFirstRUN;


	map<unsigned int, HANDLE> m_hStationStartHOLDSignal;

};

class CTesterCommClient
{
public:
	CTesterCommClient();
	~CTesterCommClient();

	long ConnectServer(int nRetry=10);
	void DisconnectServer();

	long WaitForSOT(long timeout=INFINITE);
	
	void BreakSOT();
	void ResetWaitSOT();

	void SetIsRunningOn();
	void ResetIsRunningOn();
	void SetIsRunning();
	void ResetIsRunning();
	void SetEOT();
	void ResetEOT();


	//For the 1st time of time of calling (Tester will call after SOT)
	//it will check if the event is ever created by HOST and use it
	//if so, tester will use it to wait for event INIFITELY, and start running station when EVENT is SET
	//if not created, tester will forever ignore it and start test immedately upon SOT
	//NOTE: the checking reset if tester re-open test spec or get restarted
	long WaitStationHOLD(unsigned int nStationID, long timeout=INFINITE);
	bool ResetStationSTART(unsigned int nStationID); //1 as station 1, 2 as station 2


	bool SetTestData(const TesterData &);
	bool GetTestData(TesterData*);
	//bool bStopWaitingSOT;

//private:
	HANDLE m_hSOT, m_hIsRunning, m_hEOT, m_hIsRunningOn;
    HANDLE m_hBreakWaitSOT;
	HANDLE m_hFileMapping;
	HANDLE m_hRevisionDataFileMapping;
	void* m_pBuffer;
	void* m_pRevisionDataBuffer;
	DWORD m_dwClientVer;
	DWORD m_dwServerVer;
	int m_nState;

	map<unsigned int, HANDLE> m_hStationStartHOLDSignal;

};

