// ConClient.h: interface for the CConClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONCLIENT_H__3FD6F85C_94C8_43B1_AC74_59E9C6E8359D__INCLUDED_)
#define AFX_CONCLIENT_H__3FD6F85C_94C8_43B1_AC74_59E9C6E8359D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
#include "Lock.h"
#include "Log.h"
#include <map>
#include <list>

#include "Connections.h"
#include "ConClientListener.h"
#include "ConClientSocket.h"
#include "ConClientMessage.h"

typedef std::map<int, CConClientSocket*>	IntClientSocketMap;
typedef std::list<CConClientMessage>		ConClientMessageList;

#define CONCLIENT_DISCONNECTED		0x0000
#define CONCLIENT_CONNECTED			0x0001


class CConClient
{
public:
	CConClient(CConnections * pCallback);
	virtual ~CConClient();

	int					getStatus();
	bool				isConnected();
	void				sendHeartbeat();
	void				disconnectClient(int clientId);
	void				disconnectClient();
	void				setMaxQueue(int maxNumberOfMessages);

	std::string			getUser( int clientId );

	// use start() to start listener
	void				run();
	void				setServicePort(unsigned short port);
	void				acceptConnection(CConClientListener * pListener);

	// get data
	void				receiveData(CConClientSocket * pClient);

	// send data
	void				sendData(CConClientMessage clientMessage);

	CConClientMessage 	messageFactory();
	void				shutDown()
	{
		delete m_pListener;
		m_pListener = NULL;
		disconnectClient();
	};

	void addRequest( const CConClientMessage & inMsg );
	CConnections * GetConnections(){ return m_pCConnections; }
private:
	int						m_status;
	int						m_maxQueueLength;
	void					onShutdown(HANDLE thread);
	void					newMessage();
	CConnections *			m_pCConnections;

	// startup
	IntClientSocketMap		m_idClientMap;
	CConClientListener *	m_pListener;
	Lock					m_cidLock;

	CLog					m_log;
	unsigned short			m_port;

	// get data
	ConClientMessageList	m_clientMessageList;
	Lock					m_cmlLock;
};

#endif // !defined(AFX_CONCLIENT_H__3FD6F85C_94C8_43B1_AC74_59E9C6E8359D__INCLUDED_)
