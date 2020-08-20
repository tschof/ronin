// DClientRequest.cpp: implementation of the CDClientRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DClientRequest.h"
#include "Locker.h"
#include "Main.h"
#include "Connections.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDClientRequest::CDClientRequest() : m_type(0), m_underlying(""), m_count(0)
{
}

CDClientRequest::~CDClientRequest()
{
	m_Clients.clear();
}

bool CDClientRequest::addClient( int nClientID )
{
	Locker locker( &m_ClientLock );
	try
	{
		IntIntMap::iterator it = m_Clients.find( nClientID );
		if( it != m_Clients.end() ) // already exist
		{
			m_Clients[nClientID] = m_Clients[nClientID]+1;
		}
		else
		{
			m_Clients.insert( std::make_pair<int,int>(nClientID,1) );
		}
		return true;
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write( LOG_ERROR, "addClient exception" );
	}
	return false;
}

int CDClientRequest::delClient( int nClientID )
{
	Locker locker( &m_ClientLock );
	try
	{
		IntIntMap::iterator it = m_Clients.find( nClientID );
		if( it == m_Clients.end() ) // doesn't exist
			return -1;
		if( (*it).second == 1 ) // last request for this client
		{
			m_Clients.erase( it );
		}
		else
		{
			m_Clients[nClientID] = (*it).second - 1;
		}
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write( LOG_ERROR, "delClient exception" );
	}
	return m_Clients.size();
}

void CDClientRequest::clearClients()
{
	Locker locker( &m_ClientLock );
	m_Clients.clear();
}

IntIntMap CDClientRequest::getClients()
{ 
	Locker locker( &m_ClientLock );
	IntIntMap clients = m_Clients;
	return clients; 
}

// ===================================================================
// CDCRStockQuote
// ===================================================================
CDCRStockQuote::CDCRStockQuote( const std::string & szUnder ) 
{
	// do subscription!
	m_type = DATACLIENTREQUEST_STOCKQUOTE;
	m_underlying = szUnder;
	m_underlying.begin();
}

CDCRStockQuote::~CDCRStockQuote()
{
}

CDCRL2StockQuote::CDCRL2StockQuote( const std::string & szUnder ) : CDCRStockQuote( szUnder )
{
	// do subscription!
	m_type = DATACLIENTREQUEST_L2STOCKQUOTE;
}

CDCRL2StockQuote::~CDCRL2StockQuote()
{
}

// ===================================================================
// CDCRDeleteAll
// ===================================================================

CDCROptionSeries::CDCROptionSeries( const std::string & szUnder )
{
	m_type = DATACLIENTREQUEST_OPTIONSERIES;
	m_underlying = szUnder;
	m_underlying.begin();
}

CDCRFutureSeries::CDCRFutureSeries( const std::string & szUnder )
{
	m_type = DATACLIENTREQUEST_FUTURESERIES;
	m_underlying = szUnder;
	m_underlying.begin();
}

CDCROptionQuote::CDCROptionQuote( const std::string & szUnder, const std::string & szClass, const std::string & szExch )
{
	m_type = DATACLIENTREQUEST_OPTIONQUOTE;
	m_underlying = szUnder;
	m_underlying.begin();
	m_ric = szClass;
	m_ric.begin();
	m_exchange = szExch;
	m_exchange.begin();
}

CDCROptionQuote::~CDCROptionQuote()
{
}

CDCROptionQuoteBest::CDCROptionQuoteBest( const std::string & szUnder, const std::string & szClass )
{
	m_type = DATACLIENTREQUEST_OPTIONQUOTEBEST;
	m_underlying = szUnder;
	m_underlying.begin();
	m_ric = szClass;
	m_ric.begin();
}

CDCROptionQuoteBest::~CDCROptionQuoteBest()
{
}

CDCRGreekDelta::CDCRGreekDelta( const std::string & szUnder )
{
	m_type = DATACLIENTREQUEST_GREEKDELTA;
	m_underlying = szUnder;
	m_underlying.begin();
}

CDCRGreekDelta::~CDCRGreekDelta()
{
}

CDCRFutureQuote::CDCRFutureQuote( const std::string & szUnder, const std::string & szSymbol )
{
	m_type = DATACLIENTREQUEST_FUTUREQUOTE;
	m_underlying = szUnder;
	m_underlying.begin();
	m_ric = szSymbol;
	m_ric.begin();
}

CDCRFutureQuote::~CDCRFutureQuote()
{

}
