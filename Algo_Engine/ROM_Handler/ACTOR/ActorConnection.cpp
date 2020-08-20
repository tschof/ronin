// ActorConnection.cpp : implementation file
//

#include "stdafx.h"
#include "actor.h"
#include "ActorConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActorConnection

CActorConnection::CActorConnection()
{
	m_Connected = FALSE;
	ClearSMsg();
	ClearRMsg();
}

CActorConnection::~CActorConnection()
{
}

void CActorConnection::OnConnect(int nErrorCode) 
{
	if (!(nErrorCode))
	{
		m_Connected = TRUE;
		m_ConnectionTime = (CTime::GetCurrentTime()).Format("%Y%m%d-%H:%M:%S");
		((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Connected To " + m_Host);
	}
	else
	{
		m_Connected = FALSE;
		((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Could Not Connect To " + m_Host);
		PostError(GetLastError());
	}
	
	CAsyncSocket::OnConnect(nErrorCode);
}


CString CActorConnection::GetErrorText(int nErrorCode)
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


void CActorConnection::ConnectToHost(int Port)
{
	if(Port < 0)
		Port = m_Port;

	int Error;

	if (m_hSocket != INVALID_SOCKET)
		Close();
	else
	{
		if (Create(Port))
		{
			//int setbool = 1;
			//SetSockOpt(SO_REUSEADDR, (void*)&setbool, sizeof(BOOL));

			AsyncSelect(FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE );

			if (!(Connect(m_Host, m_Port)))
			{
				if ((Error = GetLastError()) != WSAEWOULDBLOCK)
					PostError(Error);
			}
			else
				((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Connecting To Host...");
		}
		else
			PostError(GetLastError());	
	}
}


void CActorConnection::OnClose(int nErrorCode) 
{
	m_Connected = FALSE;
	m_LoggedOn	= FALSE; 

	if (nErrorCode)
		PostError(nErrorCode, FALSE);

	((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Connection Lost");
	
	CAsyncSocket::OnClose(nErrorCode);
}

void CActorConnection::OnConnect(int nErrorCode) 
{
	if (!(nErrorCode))
	{
		m_Connected = TRUE;
		m_ConnectionTime = (CTime::GetCurrentTime()).Format("%Y%m%d-%H:%M:%S");
		((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Connected To " + m_Host);
	}
	else
	{
		m_Connected = FALSE;
		((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Could Not Connect To " + m_Host);
		PostError(GetLastError());
	}
	
	CAsyncSocket::OnConnect(nErrorCode);
}

void CActorConnection::ProcessTimer()
{
	CMessageApp* l_pApp = (CMessageApp*) AfxGetApp();
	CTime l_CurrTime = CTime::GetCurrentTime();
	int l_CurrHour = l_CurrTime.GetHour();
	if(m_LogoutInitiated) {
		CTimeSpan l_TimeDiff = l_CurrTime - m_LastRcvTime;
		if(l_TimeDiff.GetTotalSeconds() > 5) {
			Shutdown();
			return;
		}
	}
	else
	{
		int HM = 100 * l_CurrTime.GetHour() + l_CurrTime.GetMinute();
		if(m_StartTime == m_EndTime) 
		{
			if(HM == m_StartTime) 
			{
				Logout();
				return;
			}
		}
		else if(m_StartTime > m_EndTime) 
		{
			if((HM >= m_EndTime) && (HM < m_StartTime)) 
			{
				Logout();
				return;
			}
		}
		else if((HM < m_StartTime) || (HM >= m_EndTime)) 
		{
			Logout();
			return;
		}
	}
	if(l_pApp->Host().Reconnect() && !l_pApp->Host().m_Connected)	
	{
		l_pApp->SetStatusTimer(15000);
 		l_pApp->ProcessError(l_pApp->Name(), "Connection To Host Lost");
		l_pApp->RecycleHost();	
		m_LogonInitiated = false;
	}
	else if(l_pApp->Host().Reconnect() && !l_pApp->Host().LoggedOn()) 
	{
		l_pApp->SetStatusTimer(2000);
		l_pApp->ProcessError(l_pApp->Name(), "Connected To Host But Not Logged On");
		Logon();
	}
	else if(l_pApp->Host().LoggedOn())
	{
		int l_TimeDiff = (l_CurrTime - m_LastSendTime).GetTotalSeconds();
		if(l_TimeDiff >= m_SendHeartbtInt) 
		{
			ProcessLogEntry("Sending heartbeat.");
			(CActSession*(l_pApp->Exchange()))->SendHeartBeat();
		}
	}

}

void CActorConnection::OnReceive(int nErrorCode) 
{
	char Buffer[4096];
	int ReadBytes = 0;
	char *pPos = Buffer;

	if (m_hSocket != INVALID_SOCKET)
	{
		// temporarily turn off even notifications so we don't miss anything (work around for old MFC bug)
		AsyncSelect(0);
		// read

		ReadBytes = Receive(Buffer, 4096);
		
		if (ReadBytes == SOCKET_ERROR)
		{
			PostError(GetLastError());
		}
		else if (ReadBytes == 0)	// socket closed on other side
		{
			m_Connected = FALSE;		// set closed flag
			((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Host Closed Socket");
			Close();						// close on our side
		}
		else
		{
			while( ReadBytes != 0 )
			{
				// Take care of the size
				if (m_pPos == m_RMsgBuffer)
				{
					if (ReadBytes >= 2)
					{
						memcpy(&m_RMsgSize, pPos, 2);
						memcpy(m_RMsgBuffer, pPos, 2);
						m_RMsgSize = ntohs(m_RMsgSize);
						memcpy(pPos, &m_RMsgSize, 2);

						pPos += 2;
						m_pPos += 2;
						m_RLeftOverBytes = m_RMsgSize - 2;
						ReadBytes -= 2;
					}
					else
					{
						memcpy(m_pPos, pPos, 1);
						m_pPos += 1;
						return;
					}
				}
				else if (m_pPos == m_RMsgBuffer + 1)
				{
					memcpy(m_pPos, pPos, 1);
					m_pPos += 1;
					memcpy(&m_RMsgSize, m_RMsgBuffer, 2);
					m_RMsgSize = ntohs(m_RMsgSize);
					m_RLeftOverBytes = m_RMsgSize - 2;
					pPos += 1;
					ReadBytes -= 1;

					if (ReadByte == 0)
					{
						return;
					}
				}

				
				// Take care of the body
				if (ReadBytes >= m_RLeftOverBytes)
				{
					memcpy(m_pPos, pPos, m_RLeftOverBytes);
					pPos += m_RLeftOverBytes;
					ReadBytes -= m_RLeftOverBytes;
					//((CMessageApp*)AfxGetApp())->ProcessTraffic(m_Name, "Receiving," + WholeString);
					m_LastReception	= CTime::GetCurrentTime();
					(CMessageApp*)AfxGetApp())->ProcessActMessage(m_RMsgBuffer, m_RMsgSize);
					ClearRMsg();
				}
				else
				{
					memcpy(m_pPos, pPos, ReadBytes);
					m_pPos += ReadBytes;
					ReadByte = 0;
				}
			}
		}
		// turn events back on
		if (m_hSocket != INVALID_SOCKET)	// check to make sure socket wasn't closed on us during read (error or bytesread = 0)
		{
			AsyncSelect();
		}
	}
}


BOOL CActorConnection::SendMessage(char *Message, int Size)
{
	BOOL rc = FALSE;

	if (m_hSocket != INVALID_SOCKET)
	{
		// temporarily turn off even notifications so we don't miss anything (work around for old MFC bug)
		AsyncSelect(0);

		if (m_Connected)
		{
			int	BytesSent = 0;
			int LeftOver = Size;
			char *p = Message;

			((CMessageApp*)AfxGetApp())->ProcessTraffic(m_Name, "Sending," + Message);
			while (LeftOver > 0)
			{
				if ((BytesSent = Send(p, LeftOver)) == SOCKET_ERROR)
				{
					PostError(GetLastError());
					BytesSent = 0;
					break;
				}
				else
				{
					LeftOver -= BytesSent;
					p += BytesSent;
				}
			}

			if (LeftOver == 0)
			{
				m_LastSendTime = CTime::GetCurrentTime();
				((CMessageApp*)AfxGetApp())->ProcessTraffic(m_Name, "Sent," + Message);
				m_Sent++;
				rc = TRUE;
			}
		}

		// turn events back on
		if (m_hSocket != INVALID_SOCKET)	// check to make sure socket wasn't closed on us during read (error or bytesread = 0)
		{
			AsyncSelect();
		}
	}

	return rc;
}

//void CActorConnection::OnSend(int nErrorCode)
//{
//}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CActorConnection, CAsyncSocket)
	//{{AFX_MSG_MAP(CActorConnection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CActorConnection member functions
