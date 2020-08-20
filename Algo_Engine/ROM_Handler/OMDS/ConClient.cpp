// ConClient.cpp: implementation of the CConClient class.
//
//////////////////////////////////////////////////////////////////////
//
//
//	CConClient
//	CConClient is the main client connection object
//	Incoming messages from all clients are queued on the m_newMessageList
//	CMessageProcessor threads pick up messages by calling the
//	messageFactory method.
//	CMessageProcessor threads can send messages by calling
//	sendData(CConCientMessage)
//	CConClientMessage contains a client Id to identify the source and
//	destination of the message.
//
//	This is a CRunnable object and is started by calling the start() method
//	start will call run() to do the actual initiailization.
//	The start method will crank up the CConClientListener object to handle
//	incoming connections.  The CConClientListener object will do a callback
//	to this.acceptConnection(CConCientListener *) to establish the new
//	client connection.

#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "ConClient.h"
#include "Connections.h"
#include "Main.h"
#include "Locker.h"
#include "MessageProcessor.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// =============================================================================
//	CConClient
// =============================================================================

CConClient::CConClient(CConnections * pCallback)
{
	m_pListener = NULL;
//	m_pListener = new CConClientListener(this);
	m_log.setObjectName("CConClient");
	m_port = 80;
	m_status = CONCLIENT_DISCONNECTED;
	m_pCConnections = pCallback;
}

CConClient::~CConClient()
{
	delete m_pListener;
	m_pListener = NULL; // set to null
//	disconnectClient();
}

void CConClient::acceptConnection(CConClientListener * pListener)
{
	CConClientSocket *	pClient = NULL;
	char debugMsg[100];

	try
	{
		try
		{
			pClient = new CConClientSocket(this);
//		    BOOL	bLinger = TRUE;
//			pClient->SetSockOpt(SO_DONTLINGER, &bLinger, sizeof(BOOL), SOL_SOCKET );
		}
		catch(...)
		{
			m_log.write(LOG_DEBUG, "CConClient::acceptConnection()... mem alloc for pClient failed");
			throw exception("Unkown Error");
		}

		if (pClient->getStatus() != 0)
		{
			// problem with client
			char buff[256];
			sprintf(buff, "New client connection failed with error %d.", pClient->getStatus());
			m_log.write(LOG_ERROR,buff);
			pClient->shutDown();
			delete pClient;
			pClient = NULL; // set to null
		}
		else
		{
			pClient->setMaxQueueLength(m_maxQueueLength);
			if( !pListener->Accept( *pClient ) )
			{
				m_log.write(LOG_ERROR,"Accept Failure");
			}
			pClient->AsyncSelect( FD_CLOSE|FD_READ|FD_WRITE );
			{

				try
				{
					Locker locker(&m_cidLock);
					pClient->AddRef();
					m_idClientMap[pClient->getClientId()] = pClient;

				}
				catch(...)
				{
					m_log.write(LOG_DEBUG, "CConClient::acceptConnection()... operation on m_idClientMap failed");
					throw exception("Unkown Error");
				}
			}

			CMain::getInstance()->AddConnection();
			sprintf(debugMsg,"acceptConnection clientId %d", pClient->getClientId());
			m_log.write(LOG_DEBUG, debugMsg);
		}
	}
	catch(...)
	{
		// problem with client
		m_log.write(LOG_ERROR, (std::string) "New client setup failed.");
		pClient->shutDown();
		delete pClient;
		pClient = NULL; // set to null
	}
}

void CConClient::receiveData(CConClientSocket *pClient)
{
	try
	{
		pClient->receiveForReadLine();
		CConClientMessage inMsg;
		inMsg.message = pClient->readLine();
		inMsg.m_clientId = pClient->getClientId();
		if( !pClient->GetLogin() )
		{
			std::string szUser = "";
			if( !CMain::getInstance()->IsValidLogin( inMsg.message, szUser ) )
			{
				disconnectClient( inMsg.m_clientId );
				return;
			}
			pClient->SetUser( szUser );
			pClient->SetLogin( true );
		}

		while(inMsg.message.size() > 0)
		{
			int i = inMsg.message.size();
			if (i>100)
				i=100;

			char debugMsg[512];
			sprintf(debugMsg, "receiveData [clientId %d] %s", inMsg.m_clientId, inMsg.message.substr(0, i).c_str());
			m_log.write(LOG_DEBUG, debugMsg);

			// Use <DISCONNECT> for testing only
			if(!inMsg.message.compare("<DISCONNECT>"))
			{
				// We got the <DISCONNECT> msg
				disconnectClient(inMsg.m_clientId);
				break;
			}
			else if (inMsg.message.substr(0,9).compare("<SM|MDFO|") == 0)
			{
				m_log.write(LOG_WARNING, inMsg.message);
			}
			else if (!inMsg.message.compare("<HB>"))
			{
				pClient->sendData("<HB>\n");
			}
			else
			{
				addRequest( inMsg );
				//m_log.write(LOG_DEBUG, "receiveData " + inMsg.message);
			}
			inMsg.message = pClient->readLine();
		}
	}
	catch(...)
	{
		m_log.write(LOG_WARNING, (std::string)"receiveData failed.  Message lost.");
	}
}

void CConClient::setServicePort(unsigned short port)
{
	// CLock lock((long) this);

	char debugMsg[50];
	sprintf(debugMsg,"setServicePort %d", port);
	m_log.write(LOG_MESSAGE, debugMsg);

	m_port = port;
}

void CConClient::run()
{
	try
	{
		// this should not return from the listener
		// if it does, no new clients can connect.
		// this shouldn't happen, but if it does
		// it would be a problem.
		if ( NULL == m_pListener)
			m_pListener = new CConClientListener(this);

		if( !m_pListener->Create(m_port) )
		{
			m_log.write(LOG_MESSAGE, "client listener create failure.");
			return;
		}

		if( !m_pListener->Listen() )
		{
			m_log.write(LOG_MESSAGE, "client listener bind failure.");
			return;
		}

		m_status = CONCLIENT_CONNECTED;
		m_log.write(LOG_MESSAGE, "client listener running.");
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"FAILURE -- CConClient::run()... m_pListener->listen(m_port)");
	}
}


CConClientMessage CConClient::messageFactory()
{
	// message receiver/processor threads pick up the next
	// client message by calling this method
	CConClientMessage	clientMessage;
	int					nSize = 0;

	{
		Locker locker(&m_cmlLock);
		nSize = m_clientMessageList.size();

		if (nSize > 0)
		{
			try
			{
				clientMessage = m_clientMessageList.front();
				m_clientMessageList.pop_front();
				CMain::getInstance()->UpdateMsgIn( nSize - 1 );
			}
			catch(...)
			{
				m_log.write(LOG_ERROR, "messageFactory message queue coruption.  Shutting down client connections.");
				throw exception("CConClient::messageFactory failed to get new client message");
			}
		}
		else
		{
			throw exception("CConClient::messageFactory no new client message");
		}
	}

	if (nSize > 0)
	{
		if (clientMessage.message.compare("<DELETE_ALL>|"))
		{
			Locker locker( &m_cidLock );

			IntClientSocketMap::iterator	it;
			it = m_idClientMap.find(clientMessage.m_clientId);
			if ( it == m_idClientMap.end())
			{
				char szMsg[128];
				sprintf(szMsg, "got a client request (%s) after the client %d disconnected",
					clientMessage.message.c_str(), clientMessage.m_clientId);
				clientMessage.message = "ClientClosed";
//				throw exception(szMsg);
			}
		}
	}

	return (clientMessage);
}


void CConClient::newMessage()
{
	// if there is a suspended receiver thread, wake it up
	CMain::getInstance()->getMessageProcessor()->ClientRelease();
}


void CConClient::sendData(CConClientMessage clientMessage)
{
	// find the appropriate CConClientSocket object
	// and call the sendData(message) method to send the message
	CConClientSocket *	pClient;

	try
	{
		Locker locker(&m_cidLock);
		IntClientSocketMap::iterator it = m_idClientMap.find(clientMessage.m_clientId);
		if( it == m_idClientMap.end() )
			return;
		pClient = (*it).second;
		if( pClient )
			pClient->AddRef();
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"sendData client map corrupted.  Shutting down client connections.");
		CMain::getInstance()->RemoveConnection();
		return;
	}
	try
	{
		if( pClient )
			pClient->sendData(clientMessage.message);
		
		//m_log.write(LOG_BUNCH_O_CRAP,(std::string) "sendData " + clientMessage.message);
	}
	catch(...)
	{
		char sBuff[256];
		sprintf(sBuff,"CConClient::sendData() failed to send data to client %d", clientMessage.m_clientId);
		m_log.write(LOG_ERROR,  sBuff);
	}
	if( pClient )
		pClient->Release();
}


void CConClient::setMaxQueue(int maxNumberOfMessages)
{
	// set the max queue length
	// this is the number of messages, not the byte length
	// the client connection will be dropped if the queue length
	// is reached.

	// CLock lock((long) this);

	char debugMsg[50];
	sprintf(debugMsg,"setMaxQueue %d", maxNumberOfMessages);
	m_log.write(LOG_MESSAGE, debugMsg);

	m_maxQueueLength = maxNumberOfMessages;
}

void CConClient::onShutdown(HANDLE thread)
{
	// shutdown the listener thread when the CConClient object
	// is shutdown
	m_status = CONCLIENT_DISCONNECTED;
	m_log.write(LOG_MESSAGE, "onShutdown Client listener shuting down.");

	try
	{
		m_pCConnections->onClientShutdown(thread);
	}
	catch(...)
	{
	}
}


bool CConClient::isConnected()
{
	return (m_status == CONCLIENT_CONNECTED);
}


int CConClient::getStatus()
{
	return(m_status);
}


void CConClient::disconnectClient()
{
	try
	{
		Locker locker(&m_cidLock);

		CConClientSocket * pCCS = NULL;

		IntClientSocketMap::iterator it = m_idClientMap.begin();
		while(it != m_idClientMap.end() )
		{
			pCCS = it->second;
			if ( pCCS != NULL)
			{
				pCCS->Release();
			}
			it++;
		}
		m_idClientMap.clear();
		CMain::getInstance()->ClearConnection();
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"sendData client map corrupted.  Shutting down client connections.");
		shutDown();
	}
}

void CConClient::addRequest( const CConClientMessage & inMsg )
{
	{
		Locker locker(&m_cmlLock);
		m_clientMessageList.push_back(inMsg);
	}
	newMessage();
}

void CConClient::disconnectClient(int clientId)
{
	// when the client disconnects for any reason
	// clean the client requests by sending a
	// <DELETE_ALL> message for the clientId in question
	// also remove the client from the client map list

	char debugMsg[50];
	sprintf(debugMsg,"disconnectClient id %d", clientId);
	m_log.write(LOG_DEBUG, debugMsg);

	// send message to message processor
	CConClientMessage	inMsg;
	try
	{
		inMsg.m_clientId = clientId;
		inMsg.message = "<DELETE_ALL>|";

		addRequest( inMsg );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "disconnectClient failed to process disconnect message");
	}

	// disconnect the socket
	CConClientSocket *	pClient = NULL;

	try
	{
		Locker locker(&m_cidLock);

		IntClientSocketMap::iterator it = m_idClientMap.find( clientId );
		if (it != m_idClientMap.end())
		{
			pClient = (*it).second;
			m_idClientMap.erase(it);

			try
			{
				if (pClient != NULL)
				{
					pClient->Close();
					pClient->Release();
				}
			}
			catch (...) {}

			CMain::getInstance()->RemoveConnection();
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "disconnectClient clientsocket map corrupted.");
		shutDown();
	}
}

void CConClient::sendHeartbeat()
{
	// this method wil send the heartbeat message to all the connected clients
	CConClientSocket *	pCS;
	IntClientSocketMap::iterator it;

	try
	{
		Locker locker(&m_cidLock);

		for(it=m_idClientMap.begin(); it != m_idClientMap.end(); it++)
		{
			pCS = it->second;
			pCS->sendData("\n<HB>\n");
		}
	}
	catch(...)
	{
		throw exception("Unkown Error"); // added throw
	}
}

std::string	CConClient::getUser( int clientId )
{
	Locker locker(&m_cidLock);
	IntClientSocketMap::iterator it;
	it = m_idClientMap.find(clientId);
	if ( it == m_idClientMap.end())
	{
		throw exception("User not found!");
	}

	return (*it).second->GetUser();
}


