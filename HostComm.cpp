// HostComm.cpp : implementation file
//

#include "stdafx.h"
#include "MS896A.h"
#include "HostComm.h"
#include "MS_HostCommConstant.h"

// CHostComm

CHostComm::CHostComm()
{
	m_bBufferReady = FALSE;
	m_bConnected = FALSE;	
	m_nBufferIndex = 0;
}

CHostComm::~CHostComm()
{
}


// CHostComm member functions

void CHostComm::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	int nByteRead;
	int i;

	nByteRead = Receive(&m_acBuffer[m_nBufferIndex], 50000-m_nBufferIndex);
	m_nBufferIndex += nByteRead;

	for (i=0; i<m_nBufferIndex; i++)
	{
		if ( (m_acBuffer[i] == 10) || (m_acBuffer[i] == 13) )
		{
			m_acBuffer[i] = 0;
		}
	}

	m_nBufferIndex = 0;
	m_bBufferReady = TRUE;

/*
	if (m_acBuffer[m_nBufferIndex-1] == 10)
	{
		m_acBuffer[m_nBufferIndex-1] = 0;
		m_nBufferIndex = 0;
		m_bBufferReady = TRUE;
	}
*/
	if ( (nErrorCode == 0) && (m_bBufferReady == TRUE) )
	{
		GetMessage(m_acBuffer);
	}

	AsyncSelect();
	CAsyncSocket::OnReceive(nErrorCode);
}

void CHostComm::OnConnect(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	if (nErrorCode == 0)
	{
		m_bConnected = TRUE;
	}
	CAsyncSocket::OnConnect(nErrorCode);

	if (nErrorCode == 0)
	{
		//Get Connection Info
		CString szSockAddress, szPeerAddress;
		UINT uiSockPortNo, uiPeerPortNo;

		CAsyncSocket::GetSockName(szSockAddress, uiSockPortNo); 
		CAsyncSocket::GetPeerName(szPeerAddress, uiPeerPortNo); 

		CString szText;
		szText.Format("Local@%s:%d-%s:%d", szSockAddress, uiSockPortNo, szPeerAddress, uiPeerPortNo); 
		m_pParent->DisplayHostCommMessage(szText, TRUE);
	}
	else
	{
		switch(nErrorCode)
		{
			case WSAEADDRINUSE:	
				m_pParent->DisplayHostCommMessage("The specified address is already in use", TRUE);
				break;
			case WSAEADDRNOTAVAIL: 
				m_pParent->DisplayHostCommMessage("The specified address is not available from the local machine", TRUE);
				break;
			case WSAEAFNOSUPPORT: 
				m_pParent->DisplayHostCommMessage("Addresses in the specified family cannot be used with this socket", TRUE);
				break;
			case WSAECONNREFUSED: 
				m_pParent->DisplayHostCommMessage("The attempt to connect was forcefully rejected", TRUE);
				break;
			case WSAEDESTADDRREQ: 
				m_pParent->DisplayHostCommMessage("A destination address is required", TRUE);
				break;
			case WSAEFAULT: 
				m_pParent->DisplayHostCommMessage("The lpSockAddrLen argument is incorrect", TRUE);
				break;
			case WSAEINVAL: 
				m_pParent->DisplayHostCommMessage("The socket is already bound to an address", TRUE);
				break;
			case WSAEISCONN: 
				m_pParent->DisplayHostCommMessage("The socket is already connected", TRUE);
				break;
			case WSAEMFILE: 
				m_pParent->DisplayHostCommMessage("No more file descriptors are available", TRUE);
				break;
			case WSAENETUNREACH: 
				m_pParent->DisplayHostCommMessage("The network cannot be reached from this host at this time", TRUE);
				break;
			case WSAENOBUFS: 
				m_pParent->DisplayHostCommMessage("No buffer space is available. The socket cannot be connected", TRUE);
				break;
			case WSAENOTCONN: 
				m_pParent->DisplayHostCommMessage("The socket is not connected", TRUE);
				break;
			case WSAENOTSOCK: 
				m_pParent->DisplayHostCommMessage("The descriptor is a file, not a socket", TRUE);
				break;
			case WSAETIMEDOUT: 
				m_pParent->DisplayHostCommMessage("The attempt to connect timed out without establishing a connection", TRUE);
				break;
			default:
				CString szText;
				szText.Format("OnConnect error: %d",nErrorCode); 
				m_pParent->DisplayHostCommMessage(szText, TRUE);
				break;
		}
	}
}

void CHostComm::GetMessage(char* acBuffer)
{
	CString szMessage = acBuffer;
	CString szCommand;

	int nCol;


	m_pParent->DisplayHostCommMessage("Rx: " + szMessage, FALSE);


	//Send Machine ID if command match
	nCol = szMessage.Find(MS_HCOM_CMD_RX_MCCHECK);
	if (nCol != -1)
	{
		m_bBufferReady = FALSE;
		m_pParent->SendHostMachineIDAck();
		return;
	}

	//Decode others command
	nCol = szMessage.Find(":");
	if (nCol != -1)
	{
		szCommand = szMessage.Left(nCol);

		if (szCommand == MS_HCOM_CMD_RX_LOTNO)
		{
			if (m_pParent->DecodeHostLotNo(szMessage) == TRUE)
			{
				m_pParent->SendHostLotNoAck();
			}
			else
			{
				m_pParent->ReceiveHostUnknownMessage();
			}
		}
		else if (szCommand == MS_HCOM_CMD_ACK_WAFEREND)
		{
		}
		else if (szCommand == MS_HCOM_CMD_ACK_BINFULL)
		{
		}
		else
		{
			m_pParent->ReceiveHostUnknownMessage();
		}
	}

	m_bBufferReady = FALSE;
}

void CHostComm::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	m_bConnected = FALSE;
	CAsyncSocket::OnClose(nErrorCode);

	m_pParent->m_bHostCommAbortByHost = TRUE;
	m_pParent->DisplayHostCommMessage("Closed by Host", TRUE);
}




