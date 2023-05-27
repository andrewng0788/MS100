#pragma once

#include <afxsock.h>
#include "MS896A.h"

// CHostComm command target

class CHostComm : public CAsyncSocket
{
public:
	char m_acBuffer[5000];
	int m_nBufferIndex;
	BOOL m_bBufferReady;
	BOOL m_bConnected;

	CMS896AApp* m_pParent;

	CHostComm();
	virtual ~CHostComm();
//	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);

	void GetMessage(char* acBuffer);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

};


