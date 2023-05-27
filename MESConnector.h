#pragma once
#include "io.h"
#include "MS896A_Constant.h"




#define MES_TIME_OUT	80
#define MES_TX_PATH	"C:\\S2MES"
#define MES_RX_PATH	"C:\\MES2S"

#define MES_CHECK_WAFER_ID_ID	"1"
#define	MES_OPERATOR_ID_INFO	"2"
#define MES_CHANGE_MACHINE_STATUS_ID	"2"


class CMESConnector: public CObject
{

protected:
	CMESConnector();
	virtual ~CMESConnector();

public:
	
	static CMESConnector* Instance();

	VOID EnableMESConnector(BOOL bSet);
	BOOL IsMESConnectorEnable();
	
	INT UpdateMachineStatus(INT nState, CString& szMsg);
	VOID UpdateSendMsgTime();
	
	VOID SetMESIncomingPath(CString szPath);
	VOID SetMESOutgoingPath(CString szPath);
	VOID SetMESTimeout(LONG lTimeout);
	VOID SetMachineNo(CString szMachineNo);

	BOOL SendMessage(CString szCmdID, CString szMsg);
	INT RecvMessage(CString& szMsg);

	BOOL FindnRemoveFiles(CString szPath);

	CString GetMESOutgoingPath();
	
private:

	BOOL MessageLog(CString szMsg);
	BOOL SearchFilesInFolder(CString szPath, CString& szFilename);
	
	BOOL m_bEnableMESConnector;
	static CMESConnector* m_pInstance;
	CTime m_ctSendMsgTime;
	BOOL m_bStopMachineRunning;
	INT m_nCurrentMachineStatus;
	INT m_lMESTimeOut;
	CString m_szMachineNo;
	CString m_szIncomingPath;
	CString m_szOutgoingPath;
	CString m_szMESIncomingBackupPath;

};