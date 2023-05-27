#pragma once
#ifndef _PR_FAILURE_CASE_LOG_H_
#define _PR_FAILURE_CASE_LOG_H_

#include <prheader.h>
#include <prmsg.h>
#include <prstatus.h>
#include <prtype.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CPRFailureCaseLog : public CObject
{
public:
	CPRFailureCaseLog(); //Constructor
	~CPRFailureCaseLog(); //Deconstructor

	static CPRFailureCaseLog* Instance();

	VOID SetData(LONG *plImageNo);
	VOID SetData(BOOL bFailureCaselogEnable, CString szFailureCaselogPath);
	VOID CheckImageNoValid();
	VOID ToggleBHMarkCaselog();
	BOOL IsEnableBHMarkCaselog();

	VOID ToggleFailureCaselog();
	BOOL IsEnableFailureCaselog();

	VOID ReadElement(CMapElement *pElement);
	VOID WriteElement(CMapElement *pElement);

	VOID SetFailureCaselogPath(CString szFailureCaselogPath);
	
	LONG LogFailureCaseImage(PR_UWORD uwRecordID, PR_UBYTE ubSenderID, PR_UBYTE ubReceiverID, CString szFileName = "");
	LONG LogFailureCaseImage(PR_ULWORD ulImageID, PR_ULWORD ulStationID, PR_UBYTE ubSenderID, PR_UBYTE ubReceiverID, CString szFileName = "");
	
	LONG LogPRImage(CString szPath, PR_UWORD uwRecordID, PR_UBYTE ubSID, PR_UBYTE ubRID, LONG &nErr);
	LONG LogPRImage(CString szPath, PR_ULWORD ulImageID, PR_ULWORD ulStationID, PR_UBYTE ubSID, PR_UBYTE ubRID, LONG &nErr);

	LONG LogFailureCaseDumpFile(PR_UBYTE ubSenderID, PR_UBYTE ubReceiverID, ULONG ulMaxNumOfLogCase);

public:
	LONG	*m_plImageNo;
	BOOL	m_bFailureCaselogEnable;
	CString m_szFailureCaselogPath;

	BOOL	m_bBHMarkCaselogEnable;
private:
	static CPRFailureCaseLog *m_pInstance;
};
#endif