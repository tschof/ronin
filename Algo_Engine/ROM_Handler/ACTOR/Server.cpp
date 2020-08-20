// Server.cpp : implementation file
//

#include "stdafx.h"
#include "MessageApp.h"
#include "Server.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServer

CServer::CServer()
{
	ReadParameters();
}

CServer::~CServer()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CServer, CAsyncSocket)
	//{{AFX_MSG_MAP(CServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CServer member functions

void CServer::OnAccept(int nErrorCode) 
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	if (!(nErrorCode))
		{
		CClientConnection *pClient = pApp->Clients().Create();
		if (!(Accept(*pClient)))
			{
			pApp->Clients().Delete(pClient);
			ProcessError("Server", GetErrorText(nErrorCode));
			}
		}
	else
		ProcessError("Server", GetErrorText(nErrorCode));
		
	CAsyncSocket::OnAccept(nErrorCode);
}

void CServer::StartListening()
{
	ProcessError("Server", "Creating Server...");
	if (Create(m_Port))
		{
		ProcessError("Server", "Created Server, Listening...");

		Listen();
		}

}

void CServer::ReadParameters()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	// port
		m_Port = GetPrivateProfileInt(pApp->Name() + " Server", "Port", 0, pApp->Path() + pApp->Name() + ".DAT");

}

CString CServer::GetErrorText(int nErrorCode)
{

	switch (nErrorCode)
		{
		case WSANOTINITIALISED:
		   return("A successful AfxSocketInit must occur before using this API.");
			break;
		case WSAENETDOWN:
			return("The Windows Sockets implementation detected that the network subsystem failed.");
			break;
		case WSAEADDRINUSE:
			return("The specified address is already in use.");
			break;
		case WSAEINPROGRESS:
			return("A blocking Windows Sockets call is in progress.");
			break;
		case WSAEADDRNOTAVAIL:
			return("The specified address is not available from the local machine.");
			break;
		case WSAEAFNOSUPPORT:
			return("Addresses in the specified family cannot be used with this socket.");
			break;
		case WSAECONNREFUSED:
			return("The attempt to connect was rejected.");
			break;
		case WSAEDESTADDRREQ:
			return("A destination address is required.");
			break;
		case WSAEFAULT:
			return("The nSockAddrLen argument is incorrect.");
			break;
		case WSAEINVAL:
			return("The socket is not already bound to an address.");
			break;
		case WSAEISCONN:
			return("The socket is already connected.");
			break;
		case WSAEMFILE:
			return("No more file descriptors are available.");
			break;
		case WSAENETUNREACH:
			return("The network cannot be reached from this host at this time.");
			break;
		case WSAENOBUFS:
			return("No buffer space is available. The socket cannot be connected.");
			break;
		case WSAENOTSOCK:
			return("The descriptor is not a socket.");
			break;
		case WSAETIMEDOUT:
			return("Attempt to connect timed out without establishing a connection.");
			break;
		case WSAEWOULDBLOCK:
			return("The socket is marked as nonblocking and the connection cannot be completed immediately");
			break;
		default:
			return("Unkown Socket Error");
			break;
		};
}

