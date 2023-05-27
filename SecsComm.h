/////////////////////////////////////////////////////////////////
// SecsComm.cpp : interface of the CSecsComm class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#include "MS896AStn.h"
#include "GemStation.h"
#include "SC_Constant.h"
#include "SecsWaferMapData.h"
#include "SecsMapID.h"
#include "FileUtil.h"
#include "MS_SecCommConstant.h"

class CSecsComm : public CMS896AStn
{
	DECLARE_DYNCREATE(CSecsComm)

public:
	typedef enum
	{
		CTRL_MODE_LOCAL,
		CTRL_MODE_REMOTE
	} CLIENT_CTRL_MODE ;

public:
	CSecsComm();
	virtual	~CSecsComm();
	virtual	BOOL InitInstance();
	virtual	INT	ExitInstance();

	VOID TurnOffSecsComm();
	VOID SetStartOperationStatus();
	VOID SetStopOperationStatus();
    //======================================================
	// Public Member Functions
	VOID Operation();

	LONG SC_EquipUploadPPG(const char *szPPID);
	LONG SC_EquipDownloadPPG(const char *pPPID);
	CString GetPPGMAckErrMsg();
// Implementation
protected:
	//Update
	VOID UpdateOutput();
	VOID UpdateProfile();
	VOID UpdatePosition();
	virtual VOID RegisterVariables();

	VOID RunOperation();
	virtual	VOID AutoOperation();		// AUTOSORT HMI page
	virtual	VOID DemoOperation();
	virtual	VOID PreStartOperation();
    virtual VOID StopOperation();

	INT OpPreStart();

    //======================================================
	// Protected Member Functions
    CString	SecsGetTime();
    INT  StringLength(PCHAR pcBuffer);

	BOOL SetupSecsPara();
	VOID SetupSecsComm();
	VOID SetControlOnlineState(BOOL bSecsOnline);
    VOID QueryCommState();
	VOID UpdateControlState(BOOL bInit = FALSE);
	BOOL CheckCommState();			//v4.42T16
   // BOOL UpdateControlStatus(INT nCtrl, INT nSubCtrl, BOOL bInit=FALSE);
//	BOOL CheckControlStatus();		//OsramTrip 8/22

	VOID QueryEquipmentState();
	USHORT GetProcessState();
	VOID QueryProcessState();
	VOID AssignHMIProcessState(USHORT hProcessState);

    BOOL LoadData(VOID);
    BOOL SaveData(VOID);

	//v4.55A7
	//S2F41 Host command Handling Fcn (AutoLine)
	BOOL S2F41HostCmd_StartCmmd(SHostCmmd HostCmmd, SHostCmmdErr &HostCmmdErr);
	BOOL S2F41HostCmd_StopCmmd(SHostCmmd HostCmmd, SHostCmmdErr &HostCmmdErr);

	VOID GetFrameLoadUnloadPara(SHostCmmd *pHostCmmd, const CString szCommandTitle,
									   CString &szFrameType, CString &szSlotID, CString &szBarCode, CString &szBinGrade, CString &szWIP);
	BOOL CheckFrameLoadUnloadPara(const CString szFrameType, const CString szSlotID, const CString szBarCode,
							      const CString szBinGrade, const CString szWIP, CString &szParaErrName);
	VOID GetFrameLoadUnloadCompletePara(SHostCmmd *pHostCmmd, const CString szCommandTitle,
										CString &szFrameType, CString &szStatus, CString &szErrMsg);
	BOOL CheckFrameLoadUnloadCompletePara(const CString szFrameType, const CString szStatus, CString &szParaErrName);
	BOOL IsLoadUnloadObjectLocked(CString szFrameType);
	VOID SetS2F41Ack(SHostCmmdErr *pstHostCmmdErr, 
					 const UCHAR	ucCommandAck, const CString szParaErrName = "",  UCHAR ucParaErrAck = 0);
	BOOL S2F41HostCmd_FrameLoad(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr);
	BOOL S2F41HostCmd_FrameLoadComplete(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr);
	BOOL S2F41HostCmd_FrameUnload(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr);
	BOOL S2F41HostCmd_FrameUnloadComplete(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr);
	BOOL S2F41HostCmd_ClearBin(SHostCmmd HostCmmd);		//v4.59A19	//Osram Penang
	BOOL S2F41HostCmd_StartLot(SHostCmmd HostCmmd);		//OsramTrip 8/22

	BOOL IsValidSlotID(const LONG lSoltID, const LONG lMinRange, const LONG lMaxRange);
	BOOL ResetBulkLoadData(CMapElement *pElement);
	BOOL IsRepeatedBarcodeNameinBulkLoadData(CMapElement *pElement, CString &szParaErrName);
	BOOL IsOnlyOneSlot(const CString szBarCode);
	BOOL DecodeBulkLoadSlotIDs(const BOOL bLoad, const BOOL bIsWL, CMapElement *pElement, const LONG lMinRange, const LONG lMaxRange,
							   const CString szSlotID, const CString szBarCode, 
							   const CString szBinGrade, const CString szBinWIP,
							   LONG& lSlotID1, LONG& lGrade1,
							   CString &szParaErrName);
	BOOL IsBulkItemDataValid(const BOOL bLoad, const BOOL bIsWL, const BOOL bOnlyOneSlot, const BOOL bFirstSlot, const CString szSlotIDTitle, const CString szSlotGradeTitle, 
							const CString szFrameBarCodeTitle, const CString szFrameWIPCounterTitle, 
							const LONG lSlotID, const LONG lGrade, const CString szBarcode, const LONG lWIPCounter, CString &szParaErrName);
	BOOL DecodeItemBulkLoadSlotIDs(const BOOL bLoad, const BOOL bIsWL, CMapElement *pElement, const BOOL bOnlyOneSlot, const LONG lIndex,
								   CString &szBulkSlotID, CString &szBulkBarCode,
								   CString &szBulkGrade, 
								   CString &szBulkWIP,
								   LONG &lCurSlotID, LONG &lCurGrade,
								   CString &szParaErrName);

	BOOL StartS2F41RemoteStartCmd();	//v4.59A11


protected:
	//======================================================
	// HMI COMMAND Functions
    // equiment status
	LONG UpdateAction(IPC_CServiceMessage &svMsg);				//OsramTrip 8/22
    LONG SC_CmdEnterBondPage(IPC_CServiceMessage& svMsg);
	LONG SC_CmdEnterBondSetupPage(IPC_CServiceMessage& svMsg);
    LONG SC_CmdEnterSetupPage(IPC_CServiceMessage& svMsg);
    LONG SC_CmdEnterServicePage(IPC_CServiceMessage& svMsg);
    LONG SC_CmdEnterHelpPage(IPC_CServiceMessage& svMsg);
    // genera.
    LONG SC_CmdLoadSettings(IPC_CServiceMessage& svMsg);
	VOID SC_UpdateSettings(BOOL bDispMess);
	LONG SC_CmdUpdateSettings(IPC_CServiceMessage& svMsg);
	LONG SC_CmdStartSecsGem(IPC_CServiceMessage& svMsg);
	LONG SC_CmdChangeSecsControlMode(IPC_CServiceMessage &svMsg);
	LONG SC_CmdEnableSecs(IPC_CServiceMessage& svMsg);
    LONG SC_CmdSwitchControlState(IPC_CServiceMessage& svMsg);
    LONG SC_CmdEnableEvent(IPC_CServiceMessage& svMsg);
    LONG SC_CmdRequestTime(IPC_CServiceMessage& svMsg); //s2f17
    LONG SC_CmdAreYouThere(IPC_CServiceMessage& svMsg);
    LONG SC_CmdSendEvent(IPC_CServiceMessage& svMsg);
    LONG SC_CmdSetAlarm(IPC_CServiceMessage& svMsg);

    // process program
    LONG SC_CmdGetPPFilePath(IPC_CServiceMessage& svMsg);
	//Upload Process program
	SHORT EquipUploadProcessProgram(const char *szPPFile, const char *szPPID);
    LONG SG_CmdDownloadPPAck(IPC_CServiceMessage& svMsg);
//    LONG SC_CmdDownloadPPG(IPC_CServiceMessage& svMsg);  //Called by menu

	//download Process program
//	LONG SC_CmdRequestPPG(IPC_CServiceMessage& svMsg);  //Called by menu
	VOID SetPPGMAck(SHORT bPPGMAck);
	SHORT PPGMAck();
	LONG SG_CmdReceivePPRequest(IPC_CServiceMessage& svMsg);

    LONG SC_CmdRequestPPDirectory(IPC_CServiceMessage& svMsg);
    LONG SC_CmdDeletePPG(IPC_CServiceMessage& svMsg);
    LONG SG_CmdDeletePPG(IPC_CServiceMessage& svMsg);
    LONG SG_CmdDirectoryPPG(IPC_CServiceMessage& svMsg);
    LONG SG_CmdReceiveRequestEPPD(IPC_CServiceMessage& svMsg);
    LONG SG_CmdHostDeletePPIDAck(IPC_CServiceMessage& svMsg);
    LONG SG_CmdHost_EquipDownloadPPG(IPC_CServiceMessage& svMsg);
	LONG SG_CmdHost_EquipUploadPPG(IPC_CServiceMessage& svMsg); // UPLOAD_PROCESS_PROG S7F5
	LONG SG_CmdHostReqGrantDnPPG(IPC_CServiceMessage& svMsg);
    // process program
	
	// SECS-GEM commands for gem station call back
    // to handle the stream/function from host
	LONG SG_CmdCommState(IPC_CServiceMessage& svMsg);
	LONG SG_CmdTimeOut(IPC_CServiceMessage& svMsg);
	LONG SG_CmdHostUpdateEC(IPC_CServiceMessage& svMsg);					//S2F15
	LONG SG_CmdHostReqOnOffLine(IPC_CServiceMessage& svMsg);				//S1F15, S1F17
    // time
	LONG SG_CmdHostTime(IPC_CServiceMessage& svMsg);						//S2F18
    // remote command
	LONG SG_CmdHostSendRC(IPC_CServiceMessage& svMsg);						//S2F21
	LONG SG_CmdHostSendCommand(IPC_CServiceMessage& svMsg);					//S2F41
    // report event alarm
	LONG SG_CmdHostDefineEvent(IPC_CServiceMessage& svMsg);					//S2F35
	LONG SG_CmdHostDefineReport(IPC_CServiceMessage& svMsg);				//S2F33
	LONG SG_CmdHostUpdateTime(IPC_CServiceMessage& svMsg);
	LONG SG_CmdHostEnableAlarm(IPC_CServiceMessage& svMsg);
	LONG SG_CmdHostEnableEvent(IPC_CServiceMessage& svMsg);					//S2F37	
	LONG SG_CmdHostReceiveEventAck(IPC_CServiceMessage& svMsg);
    LONG SG_CmdHostReceiveAlarmAck(IPC_CServiceMessage& svMsg);
	LONG SG_CmdHostReqEvent(IPC_CServiceMessage& svMsg);
    LONG SG_CmdReceiveEnableAlarmAck(IPC_CServiceMessage& svMsg);
    LONG SG_CmdReceiveEnableEventAck(IPC_CServiceMessage& svMsg);
    LONG SG_CmdReceiveHostRequestReport(IPC_CServiceMessage& svMsg);
    // terminal message service
    LONG SC_CmdSendTerminalMsg(IPC_CServiceMessage& svMsg);					//s10f1
	LONG SG_CmdHostSendTerminalMsg(IPC_CServiceMessage& svMsg);
	LONG SG_CmdHostSendTerminalMsg_MB(IPC_CServiceMessage& svMsg);
	LONG SG_CmdTerminalMsgAck(IPC_CServiceMessage& svMsg);
    LONG SC_CmdGetCString(IPC_CServiceMessage& svMsg);
    // download wafer map
	LONG SC_CmdDownLoadMap(IPC_CServiceMessage& svMsg);					
	LONG SC_CmdDownLoadMapTest(IPC_CServiceMessage& svMsg);
	LONG SG_CmdMapSetupData(IPC_CServiceMessage& svMsg);					//S12F4
	LONG SG_CmdDownLoadType1Map(IPC_CServiceMessage& svMsg);
	LONG SG_CmdDownLoadType2Map(IPC_CServiceMessage& svMsg);				//S12F16
	LONG SG_CmdDownLoadType3Map(IPC_CServiceMessage& svMsg);
	// Upload backup map to HOST	
	LONG SC_CmdSendBackupMap(IPC_CServiceMessage& svMsg);
	LONG SG_CmdMapDataAck(IPC_CServiceMessage& svMsg);
	LONG SG_CmdMapDataAck2(IPC_CServiceMessage& svMsg);
	//Package File
	LONG SG_CmdDownloadPackage(IPC_CServiceMessage& svMsg);
	LONG SG_CmdUploadPackage(IPC_CServiceMessage& svMsg);
    // spool
    LONG SG_CmdSpoolActivate(IPC_CServiceMessage& svMsg);
    LONG SG_CmdSpoolDeactivate(IPC_CServiceMessage& svMsg);
	// Lexter custom commands	//v4.11T1
    LONG SG_CmdLoadUnloadPortStatusQy(IPC_CServiceMessage& svMsg);			//S1F77
    LONG SG_CmdRequestOnlineCom(IPC_CServiceMessage& svMsg);				//S1F79
    LONG SG_CmdOnlineSumStatusRpReply(IPC_CServiceMessage& svMsg);			//S1F70
    LONG SG_CmdCasLoadCompleteRpReply(IPC_CServiceMessage& svMsg);			//S1F74
    LONG SG_CmdCasUnloadReqComRpReply(IPC_CServiceMessage& svMsg);			//S1F76
    LONG SG_CmdEquStatusChangeRpReply(IPC_CServiceMessage& svMsg);			//S1F66
    LONG SG_CmdLotProcessDataTransfer(IPC_CServiceMessage& svMsg);			//S7F65
    LONG SG_CmdLotStatusChangeRpReply(IPC_CServiceMessage& svMsg);			//S1F68
    LONG SG_CmdWaferProcessDataRpReply(IPC_CServiceMessage& svMsg);			//S6F66
    LONG SG_CmdLotProcessDataRpReply(IPC_CServiceMessage& svMsg);			//S6F70
    LONG SG_CmdAlarmOccRemRpReply(IPC_CServiceMessage& svMsg);				//S5F66
    LONG SG_CmdReplyLotProcessDataReq(IPC_CServiceMessage& svMsg);			//S7F72
	LONG SG_CmdLotCancelReqReply(IPC_CServiceMessage& svMsg);				//S7F74
    LONG SG_CmdOnlineReply(IPC_CServiceMessage& svMsg);						//S1F2

	//Map Progress Bar
	LONG UpdateProgressBar(IPC_CServiceMessage& svMsg);
	LONG SetProgressBarLimit(IPC_CServiceMessage& svMsg);

	//v4.51A12
	LONG Test(IPC_CServiceMessage& svMsg);
	LONG TestSecs(IPC_CServiceMessage& svMsg); // 4.51D1 Secs

	//Equipment Manager specific Functions
	LONG EquipStateStatisticsExpiredDays(IPC_CServiceMessage &svMsg);
	LONG EquipStateTimeLogDateInput(IPC_CServiceMessage &svMsg);
	LONG LoadEquipStateTime(IPC_CServiceMessage &svMsg);
	LONG EquipStateTimeExpireDateInput(IPC_CServiceMessage &svMsg);
	LONG DeleteEquipStateTime(IPC_CServiceMessage &svMsg);
	VOID RegisterVariables_EquipmentStaistics_VAR();

	LONG HOST_GetConfigCycleRptCommand(SHostCmmd *pstCmdRecv, Stat_Time_Stamp &stStartReportAt, LONG &lIntervalInMinutes, char *szParaErrName);
	LONG HOST_ConfigCycleRptCommand(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr);

	LONG HOST_GetRecallStatisticsCommand(SHostCmmd *pstCmdRecv, CString &strLotID, char *szParaErrName);
	LONG HOST_GetRecallStatisticsCommand(SHostCmmd *pstCmdRecv, Stat_Time_Stamp &stFromTime, Stat_Time_Stamp &stToTime, char *szParaErrName);
	LONG HOST_RecallStatistcsCommand(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr);

	LONG HOST_GetDeleteStatisticsCommand(SHostCmmd *pstCmdRecv, Stat_Time_Stamp &stBeforeTime, char *szParaErrName);
	LONG HOST_DeleteStatistcsCommand(SHostCmmd *pHostCmmd, SHostCmmdErr *pstHostCmmdErr);

	VOID HOST_CommandParaErrorHandle(SHostCmmdErr *pstHostCmmdErr, LONG lParaErrorCode, char *szParaErrName);

	VOID ReportEquipStatisticsInfo();
	BOOL SendEqpStatisticsInfo();
	BOOL EM_GenerateCyclicReport();

	//===========================================================================
	//  Update SVID, ECID etc
	//===========================================================================
	LONG UpdateSGVariables();
	LONG SG_CmdHostRecSelectEquipmentStatus(IPC_CServiceMessage &svMsg);
	LONG SG_CmdHostRecSelectEquipmentConst(IPC_CServiceMessage &svMsg);
	LONG SG_CmdHostRecReqStatusNameList(IPC_CServiceMessage &svMsg);
	LONG SG_CmdHostRecReqEquipmentConstNameList(IPC_CServiceMessage &svMsg);
	LONG SG_CmdHostRecReqEquipmentConst(IPC_CServiceMessage &svMsg);
	virtual VOID UpdateAllSGVariables();
	virtual BOOL UpdateSGEquipmentConst(INT nECID);

// ================================================================ ++
//                HMI Variables
//	e.g. CHAR, BOOL, UCHAR, LONG, ULONG, double, CString
// ================================================================ ++
protected:
    // Terminal Message Display
    CStringList m_stTMsgTextList;
    CString     m_szTMsgText[10];
    INT         m_nTMsgTextCnt;
    INT         m_nDisplayPageNum;
    BOOL        m_bTMsgNextPage;
    BOOL        m_bTMsgPrevPage;
    VOID SetDisplayText(INT nDisplayPage);
    VOID SwitchHMIPage();

	CString		m_szSCDownMapFileName;

    CString     m_szSGConfigFileName;
	CString		m_szSecsFilePath;

    CString     m_szStnName;

	CString		m_szCommState;
	CString		m_szSecCtrlStatus;
    CString     m_szEquipState;
	CString     m_szProcessState;
	INT         m_nOnOfflineState;
	//BOOL		m_bIsSecsCommEnabled;
	CString		m_szSecsCtrlMode_HMI;
	BOOL		m_bSecsOnline_HMI;
	BOOL		m_bTriggerCommStateAlarm;	//v4.42T16
	CString		m_szTempString;				//OsramTrip 8/22

	//======================================================
	// Attributes
    CString     m_szPPFilePath_HMI;
    CString     m_szHostIP_HMI;
	CString		m_szEquipId_HMI;		//aaa111
    BOOL        m_bAutoStart_HMI;
	ULONG		m_ulHostPort_HMI;
    _TINT       m_usHsmsConnectMode_HMI;
	ULONG		m_ulConverTimer_HMI;
	ULONG		m_ulHsmsT3Timeout_HMI;
	ULONG		m_ulHsmsT5Timeout_HMI;
	ULONG		m_ulHsmsT6Timeout_HMI;
	ULONG		m_ulHsmsT7Timeout_HMI;
	ULONG		m_ulHsmsT8Timeout_HMI;
	ULONG		m_ulHsmsLinkTestTimer_HMI;

    CString     m_szPPFilePath;
	CString		m_szHostIPAddress;
	CString		m_szEquipId;			//aaa111
	LONG		m_bAutoStart;
	ULONG		m_ulHostPort;
    _TINT       m_usHsmsConnectMode; // unsigned short _TINT
	ULONG		m_ulConverTimer;
	ULONG		m_ulHsmsT3Timeout; // - The (Reply Timeout) timer value in mini seconds.
	ULONG		m_ulHsmsT5Timeout; // - The (Connect Separation Timeout) timer value in mini seconds.
	ULONG		m_ulHsmsT6Timeout; // - The (Control Transaction Timeout) timer value in mini seconds.
	ULONG		m_ulHsmsT7Timeout; // - The (NOT SELECTED Timeout) timer value in mini seconds.
	ULONG		m_ulHsmsT8Timeout; // - The (Network Intercharacter Timeout) timer value in mini seconds.
	ULONG		m_ulHsmsLinkTestTimer; // - The HSMS Link test timer value in mini seconds.

	//"Equipment Manager" specific parameters for S2F41/42 & S2F15/16
	ULONG		m_ulCyclicReportUpdateCount;
	ULONG		m_ulCNotifyIntervalInMinute;

	ULONG		m_ulLastCyclicReportTimeInYear;
	ULONG		m_ulLastCyclicReportTimeInMonth;
	ULONG		m_ulLastCyclicReportTimeInDay;
	ULONG		m_ulLastCyclicReportTimeInHour;
	ULONG		m_ulLastCyclicReportTimeInMinute;

	//======================================================
	// Other Attributes
	LONG	m_lProgressBarStep;	
	LONG	m_lProgressBarLimit;

	BOOL	m_bS2F41RemoteStartCmd;	//v4.59A11

	//v4.57A3	//AutoLine
	char*	m_pPPGBuffer;	
	INT		m_nPPGBufferIndex;

	//v4.59A13
	LONG	m_lMapType;
	WAF_CMapDieInformation *m_arrRefDie;
	INT		m_nNumberOfMapRefDie;
	UCHAR	m_ucNullBin;
	ULONG	m_ulType2MapRowCount;
	ULONG	m_ulType2MapColCount;

private:
	SHORT		m_hSecsCtrlMode;
	BOOL		m_bSecsOnline;

	BOOL		m_bSecsGemSetupParaOk;
	SHORT volatile	m_bPPGMAck;
};
