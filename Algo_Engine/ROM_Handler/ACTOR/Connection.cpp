// Connection.cpp : implementation file
//

#include "stdafx.h"
#include "Connection.h"

#pragma warning(disable:4786)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnection

CConnection::CConnection(const CString& Name, const CString& AppName, const CString& Path) :
	CAsyncSocket()
{
	m_Name				= Name;
	m_LeftOver			= "";
	m_LoggedOn			= FALSE;
	m_Connected			= FALSE;
	m_ConnectionTime	= "Not Connected";
	m_Sent				= 0;
	m_Received			= 0;

	// init
		m_hSocket = INVALID_SOCKET;
	
	ReadParameters(AppName, Path);
}

CConnection::CConnection()
{
	m_Name				= "New Connection";
	m_LeftOver			= "";
	m_LoggedOn			= FALSE;
	m_Connected			= TRUE;
	m_ConnectionTime	= (CTime::GetCurrentTime()).Format("%Y%m%d-%H:%M:%S");
	m_Sent				= 0;
	m_Received			= 0;
}

CConnection::~CConnection()
{
	if (m_Connected)
		Close();
}

void CConnection::ReadParameters(const CString& AppName, const CString& Path)
{
	// port
		m_Port = GetPrivateProfileInt(m_Name + " Connection", "Port", 0, Path + AppName + ".DAT");

	// host name OR IP
		GetPrivateProfileString(m_Name + " Connection", "Host", "", m_Host.GetBuffer(128), 128, Path + AppName + ".DAT"); m_Host.ReleaseBuffer();
}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CConnection, CAsyncSocket)
	//{{AFX_MSG_MAP(CConnection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// message processing functions

BOOL CConnection::SendMessage(CString Message)
{
	int	BytesSent = 0;

	if (m_Connected)
		{
		ProcessLogEntry("Sending: " + Message);
		if ((BytesSent = Send(Message, Message.GetLength())) == SOCKET_ERROR)
			{
			PostError(GetLastError());
			BytesSent = 0;
			Close();
			}
		else
			{
			m_Sent++;
			ProcessLogEntry("Sent: " + Message);
			}
		}
	else
		ProcessError("Can't Send. Not Connected To Host");
	
return(BytesSent);
}

void CConnection::ConnectToHost()
{
	int Error;

	if (m_hSocket != INVALID_SOCKET)
		Close();
	else
		{
		if (Create())
			{
			
			ProcessStatus("Attempting To Connect To Host");

			AsyncSelect(FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE );

			if (!(Connect(m_Host, m_Port)))
				{
				if ((Error = GetLastError()) != WSAEWOULDBLOCK)
					PostError(Error);
				}
			else
				ProcessStatus("Connecting To Host...");
			}
		else
			PostError(GetLastError());	
		}
	}

/////////////////////////////////////////////////////////////////////////////
// socket functions

void CConnection::OnClose(int nErrorCode) 
{
	m_Connected =	FALSE;
	m_LoggedOn	=	FALSE; 

	if (nErrorCode)
		PostError(nErrorCode);

	ProcessError("Connection Lost");
	
	CAsyncSocket::OnClose(nErrorCode);
}

void CConnection::OnConnect(int nErrorCode) 
{
	
	if (!(nErrorCode))
		{
		m_Connected = TRUE;
		m_ConnectionTime = (CTime::GetCurrentTime()).Format("%Y%m%d-%H:%M:%S");
		ProcessStatus("Connected To Host");
		}
	else
		{
		m_Connected = FALSE;
		ProcessError("ERROR Connecting To Host");
		PostError(GetLastError());
		Close();
		}
	
	CAsyncSocket::OnConnect(nErrorCode);
}


void CConnection::OnReceive(int nErrorCode) 
{
	CString	WholeString;
	int		BytesRead	= 0;
	
	if (m_hSocket != INVALID_SOCKET)
		{
		// temporarily turn off even notifications so we don't miss anything (work around for old MFC bug)
			AsyncSelect(0);

		// read
			BytesRead = Receive(WholeString.GetBuffer(1024), 1024); WholeString.ReleaseBuffer();
			WholeString = WholeString.Left(BytesRead);

		if (BytesRead == SOCKET_ERROR)
			{
			PostError(GetLastError());
			Close();
			}
		else if (BytesRead == 0)	// socket closed on other side
			{
			m_Connected = FALSE;		// set closed flag
			ProcessError("Host Closed Socket");
			Close();						// close on our side
			}
		else
			{

			if (m_LeftOver != "")
				{
				WholeString = m_LeftOver + WholeString;
				m_LeftOver = "";
				}
			
			ProcessLogEntry("RECV(" + m_Name + "): " + WholeString);

			ProcessMessage(WholeString);
			}

		// turn events back on
		if (m_hSocket != INVALID_SOCKET)	// check to make sure socket wasn't closed on us during read (error or bytesread = 0)
			AsyncSelect();
		}
}

void CConnection::SetLogonStatus(BOOL NewStatus)
{
	m_LoggedOn = NewStatus;

	if (m_LoggedOn)
		ProcessStatus("Logged On To Host");
	else
		ProcessError("NOT Logged On To Host");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// error handling

CString CConnection::GetErrorText(int nErrorCode)
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

void CConnection::PostError(int Error)
{
	ProcessError(GetErrorText(Error));
	Close();
	m_Connected = FALSE;
}
