// ActorConnection.cpp : implementation file
//

#include "stdafx.h"
#include "actor.h"
#include "ActorConnection.h"
#include "Act.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActorConnection
CString CActorConnection::m_sLastHost;

void CActorConnection::GetLastHost()
{
	CString Path = "c:\\OrderManager\\Act.dat";
	GetPrivateProfileString("Act Connection", "Current Host", "", m_sLastHost.GetBuffer(128), 128, Path); m_sLastHost.ReleaseBuffer();
}


void CActorConnection::SetLastHost(CString &Host)
{
	m_sLastHost = Host;
}


CActorConnection::CActorConnection(const CString& Name, const CString& AppName, const CString& Path)
:CConnection(Name, AppName, Path)
{
	m_Connected = FALSE;
	ClearSMsg();
	ClearRMsg();
	ConnectToHost(m_Port);
}

CActorConnection::~CActorConnection()
{
}


/*CString CActorConnection::GetErrorText(int nErrorCode)
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
}*/


/*void CConnection::ReadParameters(const CString& AppName, const CString& Path)
{
	// port
	m_Port = GetPrivateProfileInt(m_Name + " Connection", "Port", 0, Path + AppName + ".DAT");
	m_LocalPort = GetPrivateProfileInt(m_Name + " Connection", "LocalPort", 0, Path + AppName + ".DAT");
	// host name OR IP
	GetPrivateProfileString(m_Name + " Connection", "Host", "", m_Host.GetBuffer(128), 128, Path + AppName + ".DAT"); m_Host.ReleaseBuffer();
}*/

void CActorConnection::ConnectToHost(int Port)
{
	if(Port < 0)
		Port = m_Port;

	int Error;

	if (m_hSocket != INVALID_SOCKET)
		Close();
	else
	{
		m_Host = m_ActHost.GetHost();

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

		//if (m_Host != CActorConnection::m_sLastHost)
		//{
			// Reset the ACT sequence
		//	CActTrailer::LastSent(0);
			//(CActTrailer::m_SequenceSet).SetSequence(0);
		//}

		SetLastHost(m_Host);
	}
	else
	{
		m_Connected = FALSE;
		((CMessageApp*)AfxGetApp())->ProcessLogEntry(m_Name, "Could Not Connect To " + m_Host);
		PostError(GetLastError());
	}
	
	CAsyncSocket::OnConnect(nErrorCode);
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
			if (GetLastError() == WSAEWOULDBLOCK)
			{
				PostError(GetLastError(), FALSE);
			}
			else
			{
				PostError(GetLastError());
			}
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
				if (m_pRPos == m_RMsgBuffer)
				{
					if (ReadBytes >= 2)
					{
						memcpy(&m_RMsgSize, pPos, 2);
						m_RMsgSize = ntohs(m_RMsgSize);

						memcpy(m_RMsgBuffer, &m_RMsgSize, 2);
						memcpy(pPos, &m_RMsgSize, 2);

						pPos += 2;
						m_pRPos += 2;
						m_RLeftOverBytes = m_RMsgSize - 2;
						ReadBytes -= 2;
					}
					else
					{
						memcpy(m_pRPos, pPos, 1);
						m_pRPos += 1;
						return;
					}
				}
				else if (m_pRPos == m_RMsgBuffer + 1)
				{
					memcpy(m_pRPos, pPos, 1);
					m_pRPos += 1;
					memcpy(&m_RMsgSize, m_RMsgBuffer, 2);
					m_RMsgSize = ntohs(m_RMsgSize);

					memcpy(m_RMsgBuffer, &m_RMsgSize, 2);
					m_RLeftOverBytes = m_RMsgSize - 2;
					pPos += 1;
					ReadBytes -= 1;

					if (ReadBytes == 0)
					{
						return;
					}
				}

				
				// Take care of the body
				if (ReadBytes >= m_RLeftOverBytes)
				{
					memcpy(m_pRPos, pPos, m_RLeftOverBytes);
					pPos += m_RLeftOverBytes;
					ReadBytes -= m_RLeftOverBytes;
					//((CMessageApp*)AfxGetApp())->ProcessTraffic(m_Name, "Receiving," + WholeString);
					m_LastReception	= CTime::GetCurrentTime();
					((CTerraNovaApp*)AfxGetApp())->ProcessActorMessage(m_RMsgBuffer, m_RMsgSize);
					ClearRMsg();
				}
				else
				{
					memcpy(m_pRPos, pPos, ReadBytes);
					m_pRPos += ReadBytes;
					m_RLeftOverBytes -= ReadBytes;
					ReadBytes = 0;
				}
			}
		}
		// turn events back on
		if (m_hSocket != INVALID_SOCKET)	// check to make sure socket wasn't closed on us during read (error or bytesread = 0)
		{
			AsyncSelect();
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}


BOOL CActorConnection::SendMessage(char *Message, int Size)
{
	BOOL rc = FALSE;
	short sz;

	memcpy(&sz, Message, 2);
	sz = htons(sz);
	memcpy(Message, &sz, 2);

	if (m_hSocket != INVALID_SOCKET)
	{
		// temporarily turn off even notifications so we don't miss anything (work around for old MFC bug)
		AsyncSelect(0);

		if (m_Connected)
		{
			int	BytesSent = 0;
			int LeftOver = Size;
			char *p = Message;
			CString msg;

			//msg.Format("Sending,%s", Message);
			//((CMessageApp*)AfxGetApp())->ProcessTraffic(m_Name, msg);
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
				
				//CString msg;
				//msg.Format("Sent,%s", Message);

				//((CMessageApp*)AfxGetApp())->ProcessTraffic(m_Name, msg);
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
