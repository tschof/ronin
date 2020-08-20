// ConTib.cpp: implementation of the CRvCallback class.
//
//////////////////////////////////////////////////////////////////////
//
//	CConTib is the tib connection manager. This is a CRunnable object.
//	The single thead is required by the rv library to start up the connection
//	Messages are returned via a callback to this object.
//	New subjects are added by the caling thread.
//

#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)


#include "Connections.h"
#include "MessageProcessor.h"
#include "DATib.h"
#include "ConTib.h"
#include "Main.h"
#include "Locker.h"
#include <vector>

#include <process.h>
#include <windows.h>
#include <string>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConTib::CConTib(CConnections * pConnections)
{
	m_pConnections = pConnections;
	m_log.setObjectName("ConTib");
	m_newSubjectCount = 0;
	m_bShutdown = true;
	m_pStockHandler			= NULL;
	m_pStock2Handler		= NULL;
	m_pOptionHandler		= NULL;
	m_pOptionBestHandler	= NULL;
	m_pGreekHandler			= NULL;
	m_pFutureHandler		= NULL;
}

CConTib::~CConTib()
{
	delete m_pStockHandler;
	delete m_pStock2Handler;
	delete m_pOptionHandler;
	delete m_pOptionBestHandler;
	delete m_pGreekHandler;
	delete m_pFutureHandler;
}

CDTibMessage * CConTib::getNextCDTibMessage()
{
	CDTibMessage * pTibMessage = NULL;

	Locker locker(&m_TibMessageListLock);
	try
	{
		int nSize = m_tibMessageList.size();

		if ( nSize > 0)
		{
			pTibMessage = m_tibMessageList.front();
			m_tibMessageList.pop_front();
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"messageFactory failed to get the next message.  Tib connection shutting down.");
	}

	return (pTibMessage);
}

void CConTib::newMessage()
{
	m_pConnections->getMessageProcessor()->TibRelease();
}

void CConTib::shutDown()
{
	if( m_pConnections == NULL )
		return;
	m_bShutdown = true;
	m_rvThread.Stop();
	rivas::RVThread::UnloadParms();
	m_pConnections->onTibShutdown();
}

bool CConTib::start()
{
	std::string szSubnet = CMain::getInstance()->m_config.getValue("SubNet");
	if( szSubnet.empty() )
		szSubnet = "CHI_DEV";
	std::string szDaemon = CMain::getInstance()->m_config.getValue("Daemon");
	rivas::RVParms::SetDefaultDaemon(szDaemon);
	if( !rivas::RVThread::LoadParms(szSubnet) )
	{
		m_log.write(LOG_ERROR, "LoadParms failed...");
		return false;
	}
	m_rvThread.Init();
	if( m_pConnections == NULL )
		return false;
	m_rvThread.SetLogFilePath( "c:\\OMDS\\Logs\\WinRVThread.log" );
	m_rvThread.SetDataCallback( this );
	CString szMsg;
	szMsg.Format( "RVThread started in subnet: %s, default daemon: %s", szSubnet.c_str(), szDaemon.empty()? "tcp:7500":szDaemon.c_str() );
	m_log.write(LOG_WARNING, (LPCSTR)szMsg );

	m_pStockHandler			= new StockQuoteHandler( this );
	m_pStock2Handler		= new Stock2QuoteHandler( this );
	m_pOptionHandler		= new OptionQuoteHandler( this );
	m_pOptionBestHandler	= new OptionQuoteBestHandler( this );
	m_pGreekHandler			= new GreekHandler( this );
	m_pFutureHandler		= new FutureHandler( this );
//	CRunable::start();
	m_bShutdown = false;
	return true;
}


void CConTib::run()
{
}

void CConTib::SubscribeStock( const std::string & szSymbol )
{
	if( !m_pStockHandler)
		return;
	m_rvThread.SubscribeStock( szSymbol, m_pStockHandler );
}

void CConTib::SubscribeStock2( const std::string & szSymbol )
{
	if( !m_pStock2Handler)
		return;
	m_rvThread.SubscribeStock2( szSymbol, m_pStock2Handler );
}

void CConTib::SubscribeOption( const std::string & szBase, const std::string & szClass, const std::string & szExch )
{
	if( !m_pOptionHandler)
		return;
	m_rvThread.SubscribeOption( szBase, szClass, szExch, m_pOptionHandler );
}

void CConTib::SubscribeOptionBest( const std::string & szBase, const std::string & szClass )
{
	if( !m_pOptionBestHandler)
		return;
	m_rvThread.SubscribeOptionBest( szBase, szClass, m_pOptionBestHandler );
}


void CConTib::UnSubscribeStock( const std::string & szSymbol )
{
	try
	{
		if( m_bShutdown)
			return;
		m_log.write(LOG_DEBUG,"CConTib::UnSubscribeStock(): "+szSymbol);
		m_rvThread.UnSubscribeStock( szSymbol );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CConTib::UnSubscribeStock() exception: "+szSymbol);
	}
}

void CConTib::UnSubscribeStock2( const std::string & szSymbol )
{
	try
	{
		if( m_bShutdown)
			return;
		m_log.write(LOG_DEBUG,"CConTib::UnSubscribeStock2(): "+szSymbol);
		m_rvThread.UnSubscribeStock2( szSymbol );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CConTib::UnSubscribeStock2() exception: "+szSymbol);
	}
}

void CConTib::UnSubscribeOption( const std::string & szBase, const std::string & szClass, const std::string & szExch )
{
	try
	{
		if( m_bShutdown)
			return;
		m_log.write(LOG_DEBUG,"CConTib::UnSubscribeOption(): "+szBase+"|"+szClass+"|"+szExch);
		m_rvThread.UnSubscribeOption( szBase, szClass, szExch );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CConTib::UnSubscribeOption() exception: "+szBase+"|"+szClass+"|"+szExch);
	}
}

void CConTib::UnSubscribeOptionBest( const std::string & szBase, const std::string & szClass )
{
	try
	{
		if( m_bShutdown)
			return;
		m_log.write(LOG_DEBUG,"CConTib::UnSubscribeBestOption(): "+szBase+"|"+szClass);
		m_rvThread.UnSubscribeOptionBest( szBase, szClass );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CConTib::UnSubscribeBestOption() exception: "+szBase+"|"+szClass);
	}
}

void CConTib::addMessage( CDTibMessage * pTM )
{
	int nSize = 0;
	{
		Locker locker(&m_TibMessageListLock);
		m_tibMessageList.push_back(pTM);
		nSize = m_tibMessageList.size();
	}
	 
	CMain::getInstance()->UpdateMsgOut(nSize-1);
	newMessage();
}

CDATib * CConTib::getDATib()
{
	if( !m_pConnections )
		return NULL;
	return m_pConnections->getMessageProcessor()->getDataAdapterTib();
}

COptionSeries CConTib::getOptionSeries( const std::string & szBase, const std::string & szEx, int nWait )
{
	if( m_bShutdown)
		throw exception("tib not running!");
	return m_rvThread.GetOptionSeries( szBase, 5, szEx, nWait );
}

void CConTib::SubscribeGreeks( const std::string & szBase )
{
	try
	{
		if( !m_pGreekHandler )
			return;
		m_log.write(LOG_DEBUG,"CConTib::SubscribeGreeks(): "+szBase);
		m_rvThread.SubscribeGreeks( szBase, m_pGreekHandler );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CConTib::SubscribeGreeks() exception: "+szBase);
	}
}

void CConTib::UnSubscribeGreeks( const std::string & szBase )
{
	try
	{
		if( m_bShutdown)
			return;
		m_log.write(LOG_DEBUG,"CConTib::UnSubscribeGreeks(): "+szBase);
		m_rvThread.UnSubscribeGreeks( szBase );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CConTib::UnSubscribeGreeks() exception: "+szBase);
	}
}

CFutureSeries CConTib::getFutureSeries( const std::string & szBase )
{
	if( m_bShutdown)
		throw exception("tib not running!");
	return m_rvThread.GetFutureSeries( szBase, 0 );
}

void CConTib::SubscribeFuture( const std::string & szBase, const std::string & szSymbol )
{
	if( !m_pFutureHandler )
		return;
	m_rvThread.SubscribeFuture( szBase, szSymbol, m_pFutureHandler );
}

void CConTib::UnSubscribeFuture( const std::string & szBase, const std::string & szSymbol )
{
	try
	{
		if( m_bShutdown)
			return;
		m_log.write(LOG_DEBUG,"CConTib::UnSubscribeFuture(): "+szBase+"|"+szSymbol);
		m_rvThread.UnSubscribeFuture( szBase, szSymbol );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CConTib::UnSubscribeFuture() exception: "+szBase+"|"+szSymbol);
	}
}

void CConTib::OnData( const std::string & szBase )
{
	addMessage( new CDTCMask(szBase) );
}
