// ConClientSocket.cpp: implementation of the CConClientSocket class.
//
//////////////////////////////////////////////////////////////////////
//
//
//	The CConClientSocket object handes the client connection.
//	This object will have two threads running.  one for send and one
//	for receive.  When the socket is ready to send the pendingSend method
//	will be called to begin the send.  when the reveive thread is ready
//	to receive, and there is something to receive, the pendingReceive
//	method will be called.
#include "stdafx.h"
#include "ConClientSocket.h"
#include "ConClient.h"
#include "Locker.h"
#include "Main.h"
#include "MessageProcessor.h"
#include "DClientRequest.h"

static long nextClientId = 1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConClientSocket::CConClientSocket(CConClient * pConClient) : m_bLogin( false ), m_nCount(0)
{
	m_pCConClient = pConClient;
	m_maxQueueLength = 0;

	m_clientId = InterlockedIncrement(&nextClientId);
	//m_clientId = nextClientId++;
	m_errorCode = 0;
	m_log.setObjectName("CConClientSocket");
	m_sBuffer = "";
	m_szUser = "";
}

CConClientSocket::~CConClientSocket()
{
//	BOOL	bLinger = TRUE;

//	SetSockOpt(SO_DONTLINGER, &bLinger, sizeof(BOOL), SOL_SOCKET );

//	ShutDown();
//	Close();
//	closesocket(m_hSocket );
//	m_hSocket = INVALID_SOCKET;
}

int CConClientSocket::getClientId()
{
	return(m_clientId);
}

std::string CConClientSocket::readLine()
{
	// parse the input data and get the next line
	std::string	nextString = "";
	int		i,j;
	if (m_sBuffer.size() > 0)
	{
		i = m_sBuffer.find_first_of("\n");
		if (i>=0)
		{
			nextString = m_sBuffer.substr(0,i);
			m_sBuffer = m_sBuffer.substr(++i,-1);

			i = nextString.find('<');
			j = nextString.find_last_of('>') + 1;

			if((i<0) || (j<1))
			{
				nextString = readLine();
			}
			else
			{
				nextString = nextString.substr(i, j-i);
			}
		}
	}
	return(nextString);
}

void CConClientSocket::receiveForReadLine()
{
	//	read from the socket until there is
	//	nothing left
	int		rc;

	do
	{
		memset(m_cBuffer,0,2001);

		rc = Receive(m_cBuffer,2000,0);
		if( SOCKET_ERROR == rc )
		{
			m_log.write(LOG_DEBUG, "Receive Socket Err");
			shutDown();
			return;
		}
		if (rc > 0)
		{
			m_sBuffer += m_cBuffer;
		}
		else if( rc == 0 )
		{
			char debugMsg[256];
			sprintf(debugMsg,"receiveForReadLine detected clientId %d disconnect", getClientId());
			m_log.write(LOG_DEBUG, debugMsg);
			shutDown();
		}
		else
		{
			char debugMsg[640];
			sprintf(debugMsg,"receiveForReadLine detected clientId %d Socket Err %d", getClientId(), WSAGetLastError());
			m_log.write(LOG_DEBUG, debugMsg);
			shutDown();
		}
	}
	while (rc == 2000);
}

void CConClientSocket::pendingSend( const std::string & data )
{
	// this is the method by which the message processor thread
	// places an outgoing message on the client's message queue
	try
	{
		Locker	locker(&m_Lock);
		m_newDataList.push_back(data);
		int len = m_newDataList.size();
		if(m_newDataList.size() > m_maxQueueLength)
		{
			m_newDataList.clear();
			m_log.write(LOG_WARNING,"client queue too large.  Empty Queue.");
			return;
			/*
			m_log.write(LOG_WARNING,"client queue too large.  Shuting down client connection.");
			SetLinger();
			ShutDown(2);
			CMain::getInstance()->CloseConnection(m_clientId);
			*/
		}
	}
	catch(...)
	{
		m_log.write(LOG_WARNING,"client queue error.  Shutting down client connection.");
		ShutDown(2);
	}

}

void CConClientSocket::sendData(const std::string &data)
{
	socketSend( data );
}

void CConClientSocket::setMaxQueueLength(int maxQueueLength)
{
	m_maxQueueLength = maxQueueLength;
}

void CConClientSocket::onShutdown()
{
	// call new data to wake up the send thread
	// so it knows to shutdown
	char debugMsg[100];
	sprintf(debugMsg,"onShutdown clientId %d disconnect", getClientId());
	m_log.write(LOG_DEBUG, debugMsg);
	m_pCConClient->disconnectClient(m_clientId);
}

void CConClientSocket::OnReceive( int nErrorCode )
{
	if( nErrorCode )
	{
		m_log.write(LOG_DEBUG, "OnReceive() network failure!");
	}
	m_pCConClient->receiveData(this);
}

void CConClientSocket::OnClose( int nErrorCode )
{
	shutDown();
}

void CConClientSocket::OnSend( int nErrorCode )
{
	// the socket is ready for a send
	// see if there is something to send and send it
	std::string	data;
	int		dataSize;
	{
		Locker	locker(&m_Lock);
		dataSize = m_newDataList.size();
	}
	if( nErrorCode != 0 )
	{
		char debugMsg[256];
		sprintf(debugMsg,"CConClientSocket::OnSend() ErrorCode %d", nErrorCode);
		m_log.write(LOG_ERROR, debugMsg);
		shutDown();
		return;
	}

	while(dataSize > 0)
	{
		try
		{
			Locker	locker(&m_Lock);
			if( m_newDataList.size() )
			{
				data = m_newDataList.front();
				data.begin();
				m_newDataList.pop_front();
			}
		}
		catch(...)
		{
			char debugMsg[256];
			sprintf(debugMsg,"CConClientSocket::pendingSend() output queue corrupt. clientId %d disconnect", getClientId());
			m_log.write(LOG_ERROR, debugMsg);
			shutDown();
			return;
		}

		if( SOCKET_ERROR == socketSend( data ) )
		{
			return;
		}
		else
		{
			Locker	locker(&m_Lock);
			dataSize = m_newDataList.size();
		}
	}
}

int CConClientSocket::socketSend( const std::string & data )
{
	int		nSize = 0;
	char	debugMsg[256];

	nSize = CAsyncSocket::Send(data.c_str(), data.size(), 0 );
	if( nSize == SOCKET_ERROR )
	{
		DWORD dwError = GetLastError();
		switch( dwError )
		{
		case WSAEWOULDBLOCK: // The socket is marked as nonblocking and the requested operation would block
			pendingSend(data);
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAEWOULDBLOCK" );
			m_log.write(LOG_DEBUG, debugMsg);
			return 0;
			break;
		case WSAEINPROGRESS: // A blocking Windows Sockets operation is in progress
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAEINPROGRESS" );
			break;
		case WSAECONNABORTED:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAECONNABORTED" );
			break;
		case WSAECONNRESET:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAECONNRESET" );
			break;
		case WSAENOBUFS:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAENOBUFS" );
			break;
		case WSAENETDOWN:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAENETDOWN" );
			break;
		case WSAENETRESET:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAENETRESET" );
			break;
		case WSAENOTCONN:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAENOTCONN" );
			break;
		case WSAENOTSOCK:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAENOTSOCK" );
			m_log.write(LOG_ERROR, debugMsg);
			return SOCKET_ERROR;
		case WSAESHUTDOWN:
			sprintf(debugMsg, "Client %d OnSend Socket Err %s", m_clientId, "WSAESHUTDOWN" );
			break;
		default:
			sprintf(debugMsg, "Client %d OnSend Socket Err %d", m_clientId, dwError );
			break;
		}
		m_log.write(LOG_DEBUG, debugMsg);
		//ShutDown(2);
		SetLinger();
		ShutDown(2);
		CMain::getInstance()->CloseConnection(m_clientId);
		return SOCKET_ERROR;
	}
	else if ( nSize != data.size() )
	{
		sprintf(debugMsg, "OnSend Socket Sent %d out of %d bytes", nSize, data.size() );
		m_log.write(LOG_ERROR, debugMsg);
	}
	return nSize;
}


bool CConClientSocket::SetLinger()
{
	linger lg;
	lg.l_onoff = 0;
	lg.l_linger = 1;
	if( !SetSockOpt(SO_LINGER, &lg, sizeof(linger), SOL_SOCKET ) )
	{
		int error = GetLastError();
		switch( error )
		{
		case	WSANOTINITIALISED:	
			TRACE( "A successful AfxSocketInit must occur before using this API.\n");
			break;
		case 	WSAENETDOWN:	
			TRACE( "The Windows Sockets implementation detected that the network subsystem failed.\n" );
			break;
		case	WSAEFAULT:			
			TRACE( "lpOptionValue is not in a valid part of the process address space.\n");
			break;
		case	WSAEINPROGRESS:		
			TRACE( "A blocking Windows Sockets operation is in progress.\n");
			break;
		case	WSAEINVAL:		
			TRACE( "nLevel is not valid, or the information in lpOptionValue is not valid.\n");
			break;
		case	WSAENETRESET:		
			TRACE( "Connection has timed out when SO_KEEPALIVE is set.\n");
			break;
		case	WSAENOPROTOOPT:		
			TRACE( "The option is unknown or unsupported. In particular, SO_BROADCAST is not supported on sockets of type SOCK_STREAM, while SO_DONTLINGER, SO_KEEPALIVE, SO_LINGER, and SO_OOBINLINE are not supported on sockets of type SOCK_DGRAM.\n");
			break;
		case	WSAENOTCONN:		
			TRACE( "Connection has been reset when SO_KEEPALIVE is set.\n");
			break;
		case	WSAENOTSOCK:		
			TRACE( "The descriptor is not a socket.\n");
			break;
		default:
			break;
		}
		return false;
	}
	return true;
}

