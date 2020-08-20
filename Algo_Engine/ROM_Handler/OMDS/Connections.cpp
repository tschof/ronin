// Connections.cpp: implementation of the CConnections class.
//
//////////////////////////////////////////////////////////////////////
//
//
//	CConnections
//
//	This class mannages the connections (TIB and Client)
//	This object, like CRunnable, is started by calling
//	CConnections::start()
//
//	this class contains two primary classes:
//		CConClient
//		CConTib
//	which manage the client and tib connections, respectively.
//
#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "Connections.h"
#include "ConClient.h"
#include "Main.h"
#include "MessageProcessor.h"
#include "ConTib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnections::CConnections(CMain * pMain)
{
	m_status = CONNECTION_DOWN;
	m_pConTib = new CConTib(this);
	m_pConClient = new CConClient(this);
	m_log.setObjectName("CConnections");
}

CConnections::~CConnections()
{
	delete m_pConTib;
	m_pConTib = NULL; // set to null
	delete m_pConClient;
	m_pConClient = NULL; // set to null
}

void CConnections::start()
{
	// The start method in CRunnable starts only 1 thread
	// this start method will crank up a read and a write thread
	int		retrySleep = 15000;			// 15 second
	int		retryCount = 0;
	int		maxSleep = 1000 * 60 * 10;	// 10 minutes

	m_status = CONNECTION_STARTING;
	m_log.write(LOG_DEBUG, "CConnections::start");

	while(m_status == CONNECTION_STARTING)
	{
		try
		{
			startTib();
			startClient();
			m_status = CONNECTION_RUNNING;
		}
		catch(...)
		{
			// start failed.
			// shutdown any open connections
			m_pConClient->shutDown();
			m_pConTib->shutDown();
		}
		return;

		// sleep longer between successive retries
		retrySleep *= 2;
		if(retrySleep > maxSleep)
		{
			retrySleep = maxSleep;
		}
	}
}

void CConnections::startTib()
{
	std::string		service;
	std::string		network;
	std::string		daemon;
	service = m_config.getValue("tibServicePrimary");
	network = m_config.getValue("tibNetworkPrimary");
	daemon = m_config.getValue("tibDaemonPrimary");
	//	service = m_config.getValue("tibServiceBackup");
	//	network = m_config.getValue("tibNetworkBackup");
	//	daemon = m_config.getValue("tibDaemonBackup");

	if ((service.size() == 0) || (network.size() == 0) || (daemon.size() == 0))
	{
		// don't have enough information to start.
		char	buff[512];
		sprintf(buff,"invalid tib configuration.  (Primary) Service = '%s' Network = '%s' Daemon = '%s'",
			service.c_str(),
			network.c_str(),
			daemon.c_str());
		m_log.write(LOG_ERROR,buff);
	}
	else
	{
		m_pConTib->start();
	}
}

void CConnections::startClient()
{
	unsigned short	sVar;
	std::string		param;
	HANDLE			handle = NULL;

	// get configuration for client listener port
	param = m_config.getValue("clientPort");
	sVar = atoi(param.c_str());
	if(sVar == 0)
	{
		sVar = 80;
	}
	m_pConClient->setServicePort(sVar);

	// clientQueueLength is the maximum number of messages allowed on a clients
	// output queue.  If the queue exeeds this maximum, it will be assumed that
	// the client isn't listening, and the client connection will be dropped.

	param = m_config.getValue("clientQueueLength");
	sVar = atoi(param.c_str());
	if(sVar == 0)
	{
		sVar = 20;
	}
	else
	{
		// arbitrary maximum queue length
		if(sVar > 500)
		{
			sVar = 500;
		}
	}
	m_pConClient->setMaxQueue(sVar);
	m_pConClient->run();
}


CConClientMessage CConnections::getNextClientMessage()
{
	// retrieve and return the next client message
	CConClientMessage clientMessage = m_pConClient->messageFactory();
	return clientMessage;
}


CDTibMessage * CConnections::getNextTibMessage()
{
	// retrive the next message from the TIB
	return(m_pConTib->getNextCDTibMessage());
}

void CConnections::sendToClient(int clientId, const std::string & message)
{
	// send a message to a specific client
	CConClientMessage	cm;

	cm.m_clientId = clientId;
	cm.message = message+"\n";
	m_pConClient->sendData(cm);
}

void CConnections::onClientShutdown(HANDLE thread)
{

}

void CConnections::sendHeartbeat()
{
	// send a heartbeat message to all client connections
	m_pConClient->sendHeartbeat();
}

void CConnections::disconnectClient(int cid)
{
	// disconnect the client
	m_pConClient->disconnectClient(cid);
}

void CConnections::onTibShutdown()
{
	// initiate a server shutdown only once
	if (m_status != CONNECTION_SHUTDOWN)
		shutDown();
}

int CConnections::getStatus()
{
	if(m_status == CONNECTION_RUNNING)
	{
		// verify that the connections are still up
		// if either the client or tib are down there is a problem
		if( !m_pConTib->isRunning() )
		{
			m_log.write(LOG_ERROR,"connection failure.  Shutting down.");
			shutDown();
		}
	}

	return(m_status);
}

void CConnections::shutDown()
{
	if(m_status != CONNECTION_CONFIGERROR)
		m_status = CONNECTION_SHUTDOWN;

	if(m_status != CONNECTION_DOWN)
	{
		m_pConTib->shutDown();
		m_pConClient->shutDown();
	}

	if(m_status != CONNECTION_CONFIGERROR)
		m_status = CONNECTION_DOWN;
}

CMessageProcessor * CConnections::getMessageProcessor()
{
	return CMain::getInstance()->getMessageProcessor();
}
