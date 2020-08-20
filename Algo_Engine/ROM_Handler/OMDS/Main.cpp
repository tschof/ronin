/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * Main.cpp : 
 *
 *    This is where everything starts. CMain extends CRunnable so the   
 *	  call to CMain::start() will create a thread and execute the
 *	  CMain::run method. run is responsibe for starting everything up.
 *
 *	  The CMain::initializeCache method prepopulates the symbol cach with
 *	  commonly used symbols to prevent request delays during the inital
 *	  request of the symbol.  This will in effect grab the symbol's
 *	  option series and deltas.
 * 
 *	  The three main objects started from this object are:
 *		  CConnections
 *		  CDataManager
 *		  CMessageProcessor
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 
 *
 *******************************************************************************/
// Main.cpp: implementation of the CMain class.
//
//////////////////////////////////////////////////////////////////////
//
//
//	CMain
//
//	This is where everything starts. CMain extends CRunnable so the
//	call to CMain::start() will create a thread and execute the
//	CMain::run method. run is responsibe for starting everything up.
//
//	The CMain::initializeCache method prepopulates the symbol cach with
//	commonly used symbols to prevent request delays during the inital
//	request of the symbol.  This will in effect grab the symbol's
//	option series and deltas.
//
//	The three main objects started from this object are:
//		CConnections
//		CDataManager
//		CMessageProcessor


#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "Main.h"
#include "Connections.h"
#include "DataManager.h"
#include "MessageProcessor.h"
#include "ThreadClientProcessor.h"
#include "ThreadTibProcessor.h"
#include "UserData.h"

#include <fstream>
#include <iostream>
#include "Locker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMain * CMain::m_pMain = NULL;

CMain::CMain() : m_hWnd( NULL ), m_bEnableSecurity(true)
{
	m_nClients			= 0;
	m_nMsgIn			= 0;
	m_nMsgOut			= 0;
	m_nTibSubject		= 0;
	m_nClientSubject	= 0;

	m_pDataManager = new CDataManager();
	m_pConnections = new CConnections(this);
	m_pMessageProcessor = new CMessageProcessor(m_pDataManager, m_pConnections);
}

CMain::~CMain()
{
	delete m_pConnections;
	m_pConnections = NULL; // set to null
	delete m_pDataManager;
	m_pDataManager = NULL; // set to null
	delete m_pMessageProcessor;
	m_pMessageProcessor = NULL; // set to null
	CloseDB();
}

void CMain::run()
{
	int		i;
	std::string	param;

	CThreadTibProcessor * pTP;
	CThreadClientProcessor * pCP;
	TTPList::iterator	tibIt;
	TCPList::iterator	clientIt;

	// run only if executed from start()

	//==============================================
	// Open Log
	//==============================================
	param = m_config.getValue("logLevel");
	if(!param.compare("LOG_ERROR"))
	{
		i = LOG_ERROR;
	}
	else
	{
		if(!param.compare("LOG_WARNING"))
		{
			i = LOG_WARNING;
		}
		else
		{
			if(!param.compare("LOG_MESSAGE"))
			{
				i = LOG_MESSAGE;
			}
			else
			{
				if(!param.compare("LOG_DEBUG"))
				{
					i = LOG_DEBUG;
				}
				else
				{
					i = atoi(param.c_str());
				}
			}
		}
	}

	if(i <= 0) i = LOG_DEBUG; //LOG_WARNING;
	m_log.setLevel(i);

	param = m_config.getValue("logDirectory");
	if(param.size() == 0) param = "C:/TEMP";
	m_log.setDirectory(param);

	param = m_config.getValue("logFileName");
	if(param.size() == 0) param = "OMDS";
	m_log.setFile(param);

	char buff[256];
	sprintf(buff," debug level %d", m_log.getLevel());

	param = m_config.getValue("logFileSize");
	if(param.size() == 0)
	{
		i = 1000000;
	}
	else
	{
		i = atoi(param.c_str());
	}
	if(i < 10000) i = 10000;
	m_log.setLogSize(i);

	param = m_config.getValue("logFileCount");
	if(param.size() == 0)
	{
		i = 5;
	}
	else
	{
		i = atoi(param.c_str());
	}
	if(i < 2) i = 2;
	m_log.setMaxLogs(i);

	m_log.open();
	//==============================================
	// Start Threads
	//==============================================
	int		maxThreads = atoi(m_config.getValue("MaxThreads").c_str());

	if((maxThreads <= 0) || (maxThreads > 64)) {
		maxThreads = MESSAGE_PROCESS_THREAD_COUNT;
	}

	if( !ConnectDB() )
	{
		m_log.write("Database Connection Failure!");
	}

	char logMsg[100];
	sprintf(logMsg,"MaxThreads set to %i",maxThreads);
	m_log.write(logMsg);

	{
		// Locker	locker(&m_Lock);

		for (i=0; i<maxThreads; i++)
		{
			pCP = new CThreadClientProcessor(m_pMessageProcessor);
			m_clientProcList.push_back(pCP);
			pCP->start();
		}

		for (i=0; i<maxThreads; i++)
		{
			pTP = new CThreadTibProcessor(m_pMessageProcessor);
			m_tibProcList.push_back(pTP);
			pTP->start();
		}
	}

	//==============================================
	// Pre-load symbols
	//==============================================
	m_pConnections->start();
	if(m_pConnections->getStatus() != CONNECTION_RUNNING)
	{
		// we are not running
		shutDown();
	}

	param = m_config.getValue("CacheFile");
	if(param.size() == 0)
		initializeCache(INIT_FILE);
	else
		initializeCache(param);

}

void CMain::shutDown()
{

	CThreadTibProcessor * pTP = NULL;
	CThreadClientProcessor * pCP = NULL;
	TTPList::iterator	tibIt;
	TCPList::iterator	clientIt;

	m_log.write(LOG_MESSAGE, "Main ShutDown...");

	m_pConnections->shutDown();

	for(tibIt = m_tibProcList.begin(); tibIt != m_tibProcList.end(); tibIt++)
	{
		pTP = *tibIt;
		if(pTP != NULL)
		{
			pTP->shutDown();
			delete pTP;
		}
	}
	m_tibProcList.clear();

	for(clientIt = m_clientProcList.begin(); clientIt != m_clientProcList.end(); clientIt++)
	{
		pCP = *clientIt;
		if(pCP != NULL)
		{
			pCP->shutDown();
			delete pCP;
		}
	}
	m_clientProcList.clear();

	m_pDataManager->shutDown();

	CloseDB();
	delete m_pMain;
	m_pMain = NULL;
}

void CMain::initializeCache(std::string fileName)
{
	std::ifstream		symbolCache;
	std::string			symbol;

	symbolCache.open(fileName.c_str());

	if(symbolCache.is_open())
	{
		m_log.write(LOG_MESSAGE, "Symbol Cache file found, now processing, please wait...");
		while(!symbolCache.eof())
		{
			symbolCache >> symbol ;
//zack: to do list 		if(symbol.size()>0) m_pMessageProcessor->registerUnderlying(symbol);

			::Sleep(2);
		}
		m_log.write(LOG_MESSAGE, "Symbol Cache file processing completed.");
	}
	else
	{
		m_log.write(LOG_MESSAGE, "NO Symbol Cache file found");
	}

	symbolCache.close();

	m_nClients			= 0;
	m_nMsgIn			= 0;
	m_nMsgOut			= 0;
	m_nTibSubject		= 0;
	m_nClientSubject	= 0;
}

CMessageProcessor * CMain::getMessageProcessor()
{
	return m_pMessageProcessor;
}

CConnections * CMain::getConnections()
{
	return m_pConnections;
}

void CMain::ClearConnection()
{
	m_nClients = 0;
	::PostMessage( m_hWnd, WM_CLIENT_UPDATE, m_nClients, 0 );
}

void CMain::AddConnection()
{
	InterlockedIncrement( &m_nClients );
	::PostMessage( m_hWnd, WM_CLIENT_UPDATE, m_nClients, 0 );
}

void CMain::RemoveConnection()
{
	InterlockedDecrement( &m_nClients );
	::PostMessage( m_hWnd, WM_CLIENT_UPDATE, m_nClients, 0 );
}

void CMain::CloseConnection( int cid)
{
	::PostMessage( m_hWnd, WM_CLIENT_CLOSE, cid, 0 );
}

void CMain::UpdateMsgIn( long nSize )
{
	::PostMessage( m_hWnd, WM_REQUEST_UPDATE, nSize, 0 );
	UpdateInTransaction();
}

void CMain::UpdateMsgOut( long nSize )
{
	::PostMessage( m_hWnd, WM_RESPONSE_UPDATE, nSize, 0 );
	UpdateOutTransaction();
}

void CMain::UpdateInTransaction()
{
	InterlockedIncrement( &m_nMsgIn );
	::PostMessage( m_hWnd, WM_IN_TRANS, m_nMsgIn, 0 );
}

void CMain::UpdateOutTransaction()
{
	InterlockedIncrement( &m_nMsgOut );
	::PostMessage( m_hWnd, WM_OUT_TRANS, m_nMsgOut, 0 );
}

void CMain::UpdateWarning()
{
	::PostMessage( m_hWnd, WM_WARNING_UPDATE, 0, 0 );
}

void CMain::UpdateError()
{
	::PostMessage( m_hWnd, WM_ERROR_UPDATE, 0, 0 );
}

void CMain::AddError( const std::string  & szError )
{
	Locker locker(&m_Lock);
	m_lstErrors.push_back( szError );
	UpdateError();
}

void CMain::AddWarning( const std::string & szWarning )
{
	{
		Locker	locker(&m_Lock);
		m_lstWarnings.push_back( szWarning );
	}
	UpdateWarning();
}

bool CMain::GetNextError( std::string & szError )
{
	Locker	locker(&m_Lock);
	if( !m_lstErrors.size() )
		return false;
	szError = m_lstErrors.front();
	m_lstErrors.pop_front();
	return true;
}

bool CMain::GetNextWarning( std::string & szWarning )
{
	Locker	locker(&m_Lock);
	if( !m_lstWarnings.size() )
		return false;
	szWarning = m_lstWarnings.front();
	m_lstWarnings.pop_front();
	return true;
}

bool CMain::ConnectDB()
{
	std::string szUser, szPass, szDatabase, szDataSource, szStation;
	szUser = m_config.getValue("DBUser"); //"rom_user";
	szPass = m_config.getValue("DBPass"); // "resu_mor";
	szDataSource = m_config.getValue("DSN"); //"RON_SYB_DEV1";
	if( szUser.empty() || szPass.empty() || szDataSource.empty() )
		return false;
	m_Entitlement.Init( szDataSource, szUser, szPass );
	return true;
}

void CMain::CloseDB()
{
	return;
}

// <L|user|token>
bool CMain::IsValidLogin( const std::string & szMsg, std::string & szUser )
{
	int nIndex = szMsg.find( "<L|" );
	if( nIndex == -1 ) // not a logon message
	{
#ifdef ENABLE_ENTITLEMENT
		return false;
#else
	if( !m_config.getDWORD("EnableSecurity") )
	{
		m_bEnableSecurity = false;
		return true;
	}
	return false;		
#endif
	}
	std::string szToken;
	std::string temp = szMsg.substr( nIndex+3, -1 );
	nIndex = temp.find( "|" );
	if( nIndex == -1 )
		return false;
	szUser = temp.substr( 0, nIndex );
	temp = temp.substr( nIndex+1, -1 );
	nIndex = temp.rfind( ">" );
	if( nIndex == -1 )
		return false;
	szToken = temp.substr( 0, nIndex );
	if( !m_config.getDWORD("EnableSecurity") )
	{
		m_bEnableSecurity = false;
		return true;
	}
	return m_Entitlement.IsValidUser( szUser, szToken );
}


