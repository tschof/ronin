// RequestManager.cpp: implementation of the RequestManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "RequestManager.h"
#include "ConClientMessage.h"
#include "Locker.h"
#include "DClientRequest.h"
#include "Connections.h"
#include "DTibMessage.h"
#include "ConTib.h"
#include "ConClient.h"
#include "DataManager.h"
#include "Main.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <assert.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RequestManager::RequestManager(CConnections *pConnections, CDataManager *pDataManager) : m_pConnections( pConnections ), m_pDataManager(pDataManager)
{
	LoadIndexs();
}

RequestManager::~RequestManager()
{
	
}

void RequestManager::LoadIndexs()
{
	try
	{
		std::ifstream f("Index.txt");
		while( true )
		{
			char buff[16];
			f.getline( buff, '\n' );
			if( f.fail() )
				break;
			m_Indexs.insert( buff );
		}
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "Fail to load index file" );
	}
}

std::string RequestManager::parseSymbol( const std::string & szMessage )
{
	int	start	= szMessage.find('|') + 1;
	int	end		= szMessage.find_last_of('>');
	if( start == -1 || end == -1 || end <= start )
		return ""; // invalid message
	std::string szSymbol = szMessage.substr(start,end-start);
	if( szSymbol.find(">") != -1 || szSymbol.find("<") != -1 )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:parseSymbol invalid request: "+ szMessage );			
		return "";
	}
	
	return szSymbol;
}

bool RequestManager::IsIndex( const std::string & szSymbol )
{
	if( m_Indexs.find( szSymbol ) == m_Indexs.end() )
		return false;
	return true;
}

void RequestManager::updateStockQuoteRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() || szSymbol.find("|") != -1 )
		return;
	int nPos = szSymbol.find('.');
	if( nPos == -1 )
	{
		if( IsIndex( szSymbol ) )
		{
			szSymbol += ".NaE";
		}
		else
		{
			if( szSymbol.length() < 4 )
				szSymbol += ".N";
			else
			{
				int nIndex = szSymbol.find('^');
				if( nIndex != -1 && nIndex < 4 )
					szSymbol += ".N";
				else
					szSymbol += ".NaE"; // szSymbol += ".O"; 
					
			}
		}
	}
	else
	{
		std::string szBase = szSymbol.substr(0, nPos);
		std::string szEx = szSymbol.substr(nPos+1,-1);
		if( IsIndex( szBase ) && szEx != "NaE" ) // Index can only be NaE
			return;
	}

	int nClientId = conClientMessage.m_clientId;
	bool bDelete = false; // unsubscribe
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		bDelete = true;
	}
	CDClientRequest *pRequest = NULL;
	{
		Locker locker(&m_RequestLock);
		std::string requestKey = "QS|"+szSymbol;
		tRequestMap::iterator it = m_RequestMap.find(requestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			if( bDelete ) 
				return;
			pRequest = new CDCRStockQuote( szSymbol );
			pRequest->refIncrease();
			m_pConnections->getConTib()->SubscribeStock( szSymbol );
			if( requestKey.empty() )
				return;
			try
			{
				m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
			}
		}
		else
		{
			pRequest = it->second;
			if( bDelete )
			{
				int nSize = pRequest->delClient( nClientId );
				if( nSize == -1 ) // bad client
				{
					return;
				}
				else if( nSize == 0 ) // all gone
				{
					m_RequestMap.erase( it );
					m_pConnections->getConTib()->UnSubscribeStock( szSymbol );
				}
				try
				{
					pRequest->refDecrease();
				}
				catch(...)
				{
					CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateStockQuoteRequest refDecrease() exception: "+ conClientMessage.message );			
				}
				return;
			}
			pRequest->refIncrease();
			try
			{
				std::string reply = m_pDataManager->getStockQuoteSnapShot( szSymbol );
				if( !reply.empty() )
					m_pConnections->sendToClient( nClientId, reply ); 
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateStockQuoteRequest send snapshot exception: "+ conClientMessage.message );			
			}
		}
		pRequest->addClient( nClientId );
	}
}

void RequestManager::updateStock2QuoteRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() || szSymbol.find("|") != -1 )
		return;
	int nClientId = conClientMessage.m_clientId;
	bool bDelete = false; // unsubscribe
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		bDelete = true;
	}
	CDClientRequest *pRequest = NULL;
	{
		Locker locker(&m_RequestLock);
		std::string requestKey = "QS2|"+szSymbol;
		tRequestMap::iterator it = m_RequestMap.find(requestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			if( bDelete ) 
				return;
			pRequest = new CDCRL2StockQuote( szSymbol );
			pRequest->refIncrease();
			if( requestKey.empty() )
				return;
			try
			{
				m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
			}
			m_pConnections->getConTib()->SubscribeStock2( szSymbol );
		}
		else
		{
			pRequest = it->second;
			if( bDelete )
			{
				int nSize = pRequest->delClient( nClientId );
				if( nSize == -1 ) // bad client
				{
					return;
				}
				else if( nSize == 0 ) // all gone
				{
					m_RequestMap.erase( it );
					m_pConnections->getConTib()->UnSubscribeStock2( szSymbol );
				}
				try
				{
					pRequest->refDecrease();
				}
				catch(...)
				{
					CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateStockQuoteRequest refDecrease() exception: "+ conClientMessage.message );			
				}
				return;
			}
			pRequest->refIncrease();
			try
			{
				std::string reply = m_pDataManager->getStock2QuoteSnapShot( szSymbol );
				if( !reply.empty() )
					m_pConnections->sendToClient( nClientId, reply ); 
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateStock2QuoteRequest send snapshot exception: "+ conClientMessage.message );			
			}
		}
		pRequest->addClient( nClientId );
	}
}

void RequestManager::updateOptionSeriesRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() || szSymbol.find("|") != -1 )
		return;
	int nClientId = conClientMessage.m_clientId;
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		// ignored...
		return;
	}
	
	try
	{
		CDTOptionSeries * pSeries = new CDTOptionSeries( szSymbol );
		std::string reply;
		reply = pSeries->CreateResponse( szSymbol );
		delete pSeries;
		// reply = m_pDataManager->getOptionSeriesSnapShot( szSymbol );
		if( !reply.empty() )
		{
			m_pConnections->sendToClient( nClientId, reply ); 
			return;// all done!
		}
		return;
	}
	catch(...)
	{
		// YAHOO an non-existing symbol will cause this because CMask will never come back! CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionSeriesRequest send snap shot exception: "+ conClientMessage.message );			
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionSeriesRequest failed: "+ conClientMessage.message );			
		return;
		CDClientRequest *pRequest = NULL;
		{
			Locker locker(&m_RequestLock);
			std::string requestKey = "OS|"+szSymbol;
			tRequestMap::iterator it = m_RequestMap.find(requestKey);
			if( it == m_RequestMap.end() ) // doesn't exist
			{
				pRequest = new CDCROptionSeries( szSymbol );
				pRequest->refIncrease(); // only addRef once, because we will never remove it!
				if( requestKey.empty() )
					return;
				try
				{
					m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
				}
				catch(...)
				{
					CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
				}
			}
			else
			{
				pRequest = it->second;
			}
			pRequest->addClient( nClientId ); 
		}
		CDTibMessage *pDTM = new CDTOptionSeries(szSymbol);
		if( !pDTM )
		{
			CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionSeriesRequest out of memory: "+ conClientMessage.message );			
			return;
		}
		m_pConnections->getConTib()->addMessage( pDTM );
		return;
	}
}

void RequestManager::updateOptionQuoteRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() )
		return;
	int nClientId = conClientMessage.m_clientId;
	bool bDelete = false; // unsubscribe
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		bDelete = true;
	}
	CDClientRequest *pRequest = NULL;
	std::string szUnder, szClass, szExch;
	int i = szSymbol.find('|');
	if( i == -1 )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionQuoteRequest bad client request: "+ conClientMessage.message );			
		return;
	}
	int j = szSymbol.find_last_of('|');
	if( i == j || j == -1 || j-i <= 0 )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionQuoteRequest bad client request: "+ conClientMessage.message );			
		return;
	}
	szUnder = szSymbol.substr( 0, i );
	szClass = szSymbol.substr( i+1, j-i-1 );
	if( j == szSymbol.length()-1 )
		return;
	szExch = szSymbol.substr( j+1, -1 );
	if( szUnder.empty() || szClass.empty() || szExch.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionQuoteRequest bad client request: "+ conClientMessage.message );			
		return;
	}
	// do simulated client request for all option exchanges
	if( szExch == " " )
	{
		CreateOptionQuoteClientRequest( conClientMessage, szUnder );
		return;
	}
	szUnder = szUnder.substr(0,szUnder.find("."));
	{
		Locker locker(&m_RequestLock);
		std::string requestKey = "QO|"+szClass+"|"+szExch;
		tRequestMap::iterator it = m_RequestMap.find(requestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			if( bDelete ) 
				return;
			try
			{
				pRequest = new CDCROptionQuote( szUnder, szClass, szExch );
				pRequest->refIncrease();
				m_pConnections->getConTib()->SubscribeOption( szUnder, szClass, szExch );
			}
			catch(...)
			{
				// we expect this if cmask is not available!!!
				delete pRequest;
				Sleep(10);
				m_pConnections->getConClient()->addRequest( conClientMessage );
				return; // we expect this if cmask is not available!!!
			}
			if( requestKey.empty() )
				return;
			try
			{
				m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
			}
		}
		else
		{
			pRequest = it->second;
			if( bDelete )
			{
				int nSize = pRequest->delClient( nClientId ); 
				if( nSize == -1 ) // bad client
				{
					return;
				}
				else if( nSize == 0 ) // all gone
				{
					m_RequestMap.erase( it );
					m_pConnections->getConTib()->UnSubscribeOption( szUnder, szClass, szExch );
				}
				try
				{
					pRequest->refDecrease();
				}
				catch(...)
				{
					CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateStockQuoteRequest refDecrease() exception: "+ conClientMessage.message );			
				}
				return;
			}
			pRequest->refIncrease();
		}
		pRequest->addClient( nClientId );
	}

	try
	{
		std::string reply = m_pDataManager->getOptionQuoteSnapShot( szUnder, szClass, szExch );
		if( !reply.empty() )
			m_pConnections->sendToClient( nClientId, reply ); 
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionQuoteRequest send snap shot exception: "+ conClientMessage.message );			
	}

}

void RequestManager::updateOptionQuoteBestRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() )
		return;
	int nClientId = conClientMessage.m_clientId;
	bool bDelete = false; // unsubscribe
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		bDelete = true;
	}
	CDClientRequest *pRequest = NULL;
	std::string szUnder, szClass;
	int i = szSymbol.find('|');
	if( i == -1 )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionQuoteBestRequest bad client request: "+conClientMessage.message );			
		return;
	}
	szUnder = szSymbol.substr( 0, i );
	if( szUnder.find(".") != -1 )
		return;
	szClass = szSymbol.substr( i+1, -1 );
	try
	{
		Locker locker(&m_RequestLock);
		std::string requestKey = "QOB|"+szClass;
		tRequestMap::iterator it = m_RequestMap.find(requestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			if( bDelete ) 
				return;
			try
			{
				pRequest = new CDCROptionQuoteBest( szUnder, szClass );
				pRequest->refIncrease();
				m_pConnections->getConTib()->SubscribeOptionBest( szUnder, szClass );
			}
			catch(...)
			{
				// we expect this if cmask is not available!!!
				delete pRequest;
				Sleep(10);
				m_pConnections->getConClient()->addRequest( conClientMessage );
				return; 
			}
			if( requestKey.empty() )
				return;
			try
			{
				m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
			}
		}
		else
		{
			pRequest = it->second;
			if( bDelete )
			{
				int nSize = pRequest->delClient( nClientId );
				if( nSize == -1 ) // bad client
				{
					return;
				}
				else if( nSize == 0 ) // all gone
				{
					m_RequestMap.erase( it );
					m_pConnections->getConTib()->UnSubscribeOptionBest( szUnder, szClass );
				}
				try
				{
					pRequest->refDecrease();
				}
				catch(...)
				{
					CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateStockQuoteRequest refDecrease() exception: "+ conClientMessage.message );			
				}
				return;
			}
			pRequest->refIncrease();
		}
		pRequest->addClient( nClientId );
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionQuoteBestRequest memory corruption "+conClientMessage.message );			
	}
	try
	{
		std::string reply = m_pDataManager->getOptionQuoteBestSnapShot( szUnder, szClass );
		if( !reply.empty() )
			m_pConnections->sendToClient( nClientId, reply ); 
	}
	catch(...)
	{
		// log error
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionQuoteBestRequest send client snapshot exception: "+conClientMessage.message );			
	}
}

void RequestManager::deleteAllRequest( const CConClientMessage & conClientMessage )
{
	int nClientId = conClientMessage.m_clientId;
	try
	{
		Locker locker(&m_RequestLock);
		tRequestMap::iterator it;
		for( it = m_RequestMap.begin(); it != m_RequestMap.end(); it++ )
		{
			CDClientRequest *pRequest = NULL;
			pRequest = (*it).second;
			if( !pRequest || pRequest->getType() == DATACLIENTREQUEST_OPTIONSERIES )
				continue;
			int nResult = pRequest->delClient( nClientId );
			if( nResult == -1 )
				continue;
			switch( pRequest->getType() )
			{
			case DATACLIENTREQUEST_STOCKQUOTE:
				{
					if( nResult == 0 )
					{
						m_pConnections->getConTib()->UnSubscribeStock( pRequest->getUnderlying() );
						it = m_RequestMap.erase( it );
					}
				}
				break;
			case DATACLIENTREQUEST_L2STOCKQUOTE:
				{
					if( nResult == 0 )
					{
						it = m_RequestMap.erase( it );
						m_pConnections->getConTib()->UnSubscribeStock2( pRequest->getUnderlying() );
					}
				}
				break;
			case DATACLIENTREQUEST_OPTIONQUOTE:
				{
					if( nResult == 0 )
					{
						it = m_RequestMap.erase( it );
						CDCROptionQuote * ptr = dynamic_cast<CDCROptionQuote*>(pRequest);
						if( !ptr )
							break;
						m_pConnections->getConTib()->UnSubscribeOption( ptr->getUnderlying(), ptr->getRic(), ptr->getExchange() );
					}
				}
				break;
			case DATACLIENTREQUEST_OPTIONQUOTEBEST:
				{
					if( nResult == 0 )
					{
						it = m_RequestMap.erase( it );
						CDCROptionQuoteBest * ptr = dynamic_cast<CDCROptionQuoteBest*>(pRequest);
						if( !ptr )
							break;
						m_pConnections->getConTib()->UnSubscribeOptionBest( ptr->getUnderlying(), ptr->getRic() );
					}
				}
				break;
			case DATACLIENTREQUEST_GREEKDELTA:
				{
					if( nResult == 0 )
					{
						it = m_RequestMap.erase( it );
						m_pConnections->getConTib()->UnSubscribeGreeks( pRequest->getUnderlying() );
					}
				}
				break;
			default:
				continue;
			}
			pRequest->refDecrease();
			if( it == m_RequestMap.end() )
				break;

		}
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:deleteAllRequest exception. " );			
	}
}

void RequestManager::updateGreekDeltaRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() || szSymbol.find("|") != -1 )
		return;
	int nClientId = conClientMessage.m_clientId;
	bool bDelete = false; // unsubscribe
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		bDelete = true;
	}
	CDClientRequest *pRequest = NULL;
	{
		Locker locker(&m_RequestLock);
		std::string requestKey = "GD|"+szSymbol;
		tRequestMap::iterator it = m_RequestMap.find(requestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			if( bDelete ) 
				return;
			pRequest = new CDCRGreekDelta( szSymbol );
			pRequest->refIncrease();
			if( requestKey.empty() )
				return;
			try
			{
				m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
			}
			m_pConnections->getConTib()->SubscribeGreeks( szSymbol );
		}
		else
		{
			pRequest = it->second;
			if( bDelete )
			{
				int nSize = pRequest->delClient( nClientId );
				if( nSize == -1 ) // bad client
				{
					return;
				}
				else if( nSize == 0 ) // all gone
				{
					m_RequestMap.erase( it );
					m_pConnections->getConTib()->UnSubscribeGreeks( szSymbol );
				}
				try
				{
					pRequest->refDecrease();
				}
				catch(...)
				{
					CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateGreekDeltaRequest refDecrease() exception: "+ conClientMessage.message );			
				}
				return;
			}
			pRequest->refIncrease();
		}
		pRequest->addClient( nClientId );
	}
	try
	{
		std::string reply = m_pDataManager->getGreekDeltaSnapShot( szSymbol );
		if( !reply.empty() )
			m_pConnections->sendToClient( nClientId, reply ); 
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateGreekDeltaRequest send snapshot exception: "+ conClientMessage.message );			
	}
}

bool RequestManager::addRequest( const CConClientMessage & conClientMessage )
{

	/*
	Algorithm: 
	1. request map lookup  
	2. if request already exist, just addRef
	3. else create new request and do subscription
	4. call DataManager to get the cache data and send it back to client
	4. if unsubscribe request, update request client list call Release()
	5. if refCount == 0, delete request and unsubscribe if necessary
	*/ 
	int nClientId = conClientMessage.m_clientId;

	int i = conClientMessage.message.find('|') + 1;
	if( i == 0 )
		return false;

	// <QS|ric>, <!QS|ric>
	if ((!conClientMessage.message.substr(0,i).compare("<QS|")) || (!conClientMessage.message.substr(0,i).compare("<!QS|")))
	{
		// Stock Quote Subscribe/UnSubscribe Request
		try
		{
			CheckEntitlement( conClientMessage );
			updateStockQuoteRequest( conClientMessage );
		}
		catch( EntitlementException & ex )
		{
			m_pConnections->sendToClient( nClientId, ex.what() ); 
			CMain::getInstance()->m_log.write(LOG_WARNING,"Entitlement exception: "+conClientMessage.message );			
			return false;
		}
		catch(...)
		{
			CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:addRequest updateStockQuoteRequest exception: "+conClientMessage.message );			
			return false;
		}
	}
	else if( (!conClientMessage.message.substr(0,i).compare("<QS2|")) || (!conClientMessage.message.substr(0,i).compare("<!QS2|")) ) // <QS2|ric>, <!QS2|ric>
	{
		
		// Stock Level 2 Subscribe/UnSubscribe Request
		try
		{
			CheckEntitlement( conClientMessage );
			updateStock2QuoteRequest( conClientMessage );
		}
		catch( EntitlementException & ex )
		{
			m_pConnections->sendToClient( nClientId, ex.what() ); 
			CMain::getInstance()->m_log.write(LOG_WARNING,"Entitlement exception: "+conClientMessage.message );			
			return false;
		}
		catch(...)
		{
			CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:addRequest updateStock2QuoteRequest exception: "+conClientMessage.message );			
			return false;
		}
	}
	else if((!conClientMessage.message.substr(0,i).compare("<OS|"))) // Option Series Request
	{
		// Option Series request
		try
		{
			updateOptionSeriesRequest( conClientMessage );
		}
		catch(...)
		{
			CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:addRequest updateOptionSeriesRequest exception: "+conClientMessage.message );			
			return false;
		}
	}
	else if((!conClientMessage.message.substr(0,i).compare("<QO|")) || (!conClientMessage.message.substr(0,i).compare("<!QO|"))) 
	{
		// Option Quote Subscribe and UnSubscribe request
		try
		{
			CheckEntitlement( conClientMessage );
			updateOptionQuoteRequest( conClientMessage );
		}
		catch( EntitlementException & ex )
		{
			m_pConnections->sendToClient( nClientId, ex.what() ); 
			CMain::getInstance()->m_log.write(LOG_WARNING,"Entitlement exception: "+conClientMessage.message );			
			return false;
		}
		catch(...)
		{
			CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:addRequest updateOptionQuoteRequest exception: "+conClientMessage.message );			
			return false;
		}
	}
	else if((!conClientMessage.message.substr(0,i).compare("<QOB|")) || (!conClientMessage.message.substr(0,i).compare("<!QOB|")))
	{
		// Option QuoteBest Subscribe and UnSubscribe request
		try
		{
			CheckEntitlement( conClientMessage );
			updateOptionQuoteBestRequest( conClientMessage );
		}
		catch( EntitlementException & ex )
		{
			m_pConnections->sendToClient( nClientId, ex.what() ); 
			CMain::getInstance()->m_log.write(LOG_WARNING,"Entitlement exception: "+conClientMessage.message );			
			return false;
		}
		catch(...)
		{
			CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:addRequest updateOptionQuoteBestRequest exception: "+conClientMessage.message );			
			return false;
		}
	}
	else if((!conClientMessage.message.substr(0,i).compare("<GD|")) || (!conClientMessage.message.substr(0,i).compare("<!GD|")))
	{
		updateGreekDeltaRequest( conClientMessage );
	}
	else if((!conClientMessage.message.substr(0,i).compare("<FS|")) || (!conClientMessage.message.substr(0,i).compare("<!FS|")))
	{
		updateFutureSeriesRequest( conClientMessage );
	}
	else if((!conClientMessage.message.substr(0,i).compare("<QF|")) || (!conClientMessage.message.substr(0,i).compare("<!QF|")))
	{
		try
		{
			CheckEntitlement( conClientMessage );
			updateFutureQuoteRequest( conClientMessage );
		}
		catch( EntitlementException & ex )
		{
			m_pConnections->sendToClient( nClientId, ex.what() ); 
			CMain::getInstance()->m_log.write(LOG_WARNING,"Entitlement exception: "+conClientMessage.message );			
			return false;
		}
		catch(...)
		{
			CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:addRequest updateFutureQuoteRequest exception: "+conClientMessage.message );			
			return false;
		}
	}
	else
	{
		// Delete client
		// <DELETE>
		int i = conClientMessage.message.find('>') + 1;
		if (!conClientMessage.message.substr(0,i).compare("<DELETE_ALL>"))
		{
			// Delete All Request for a client
			deleteAllRequest( conClientMessage );
		}
		else
		{
			// Re-register all TIB subjects, doesn't make sense!!!
		}
	}
	return true;
}

void RequestManager::sendToClient( const std::string & szRequestKey, CDTibMessage * pDTibMsg )
{
	CDClientRequest *pRequest = NULL;
	std::string szUnder = "";
	Locker locker(&m_RequestLock);
	try
	{
		tRequestMap::iterator it = m_RequestMap.find(szRequestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			return;
		}
		pRequest = it->second;
		szUnder = pRequest->getUnderlying();
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:sendToClient exception 1: "+ szRequestKey );			
		return;
	}
	if( szUnder.empty() )
	{
		return;
	}
	std::string szReply = pDTibMsg->CreateResponse(szUnder);
	if( szReply.empty() )
		return;
	try
	{
		IntIntMap clients = pRequest->getClients();
		// double dispatch -2
		IntIntMap::iterator it;
		for( it = clients.begin(); it != clients.end(); it++ )
		{
			m_pConnections->sendToClient( (*it).first, szReply );
		}
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:sendToClient exception 2: "+ szRequestKey );			
	}
}

void RequestManager::clearClients( const std::string & szRequestKey )
{
	Locker locker(&m_RequestLock);
	tRequestMap::iterator it = m_RequestMap.find(szRequestKey);
	if( it != m_RequestMap.end() ) // doesn't exist
	{
		(*it).second->clearClients();
	}
}

void RequestManager::CreateOptionQuoteClientRequest( const CConClientMessage & conClientMessage, const std::string & szUnder )
{
	std::vector<std::string> vExchgs = m_pDataManager->getOptionExchanges( szUnder );
	if( vExchgs.size() == 0 )
	{
		/*
		CConClientMessage msg = conClientMessage;
		msg.message = "<OS|"+szUnder+">";
		m_pConnections->getConClient()->addRequest( msg );
		Sleep(10);
		m_pConnections->getConClient()->addRequest( conClientMessage );
		return;
		*/
		CDTOptionSeries * pSeries = new CDTOptionSeries( szUnder );
		std::string reply;
		reply = pSeries->CreateResponse( szUnder );
		delete pSeries;
		if( !reply.empty() )
		{
			m_pConnections->sendToClient( conClientMessage.m_clientId, reply ); 
			return;// all done!
		}
		return;
	}
	for( int i = 0; i < vExchgs.size(); i++ )
	{
		CConClientMessage msg;
		msg = conClientMessage;
		int nIndex = conClientMessage.message.find("| ");
		if( nIndex == -1 )
		{
			// error
			continue;
		}
		msg.message = msg.message.replace( nIndex+1, nIndex+2, vExchgs[i]+">" );
		m_pConnections->getConClient()->addRequest( msg );
	}
}

void RequestManager::updateFutureSeriesRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() || szSymbol.find("|") != -1 )
		return;
	int nClientId = conClientMessage.m_clientId;
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		// ignored...
		return;
	}
	try
	{
		std::string reply = m_pDataManager->getFutureSeriesSnapShot( szSymbol );
		if( !reply.empty() )
		{
			m_pConnections->sendToClient( nClientId, reply ); 
			return;// all done!
		}
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionSeriesRequest send snap shot exception: "+ conClientMessage.message );			
		return;
	}
	CDClientRequest *pRequest = NULL;
	{
		Locker locker(&m_RequestLock);
		std::string requestKey = "FS|"+szSymbol;
		tRequestMap::iterator it = m_RequestMap.find(requestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			pRequest = new CDCRFutureSeries( szSymbol );
			pRequest->refIncrease(); // only addRef once, because we will never remove it!
			if( requestKey.empty() )
				return;
			try
			{
				m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
			}
		}
		else
		{
			pRequest = it->second;
		}
		pRequest->addClient( nClientId ); 
	}
	CDTibMessage *pDTM = new CDTFutureSeries(szSymbol);
	if( !pDTM )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateOptionSeriesRequest out of memory: "+ conClientMessage.message );			
		return;
	}
	m_pConnections->getConTib()->addMessage( pDTM );
}

void RequestManager::updateFutureQuoteRequest( const CConClientMessage & conClientMessage )
{
	std::string szSymbol = parseSymbol(conClientMessage.message);
	if( szSymbol.empty() )
		return;
	int nClientId = conClientMessage.m_clientId;
	bool bDelete = false; // unsubscribe
	if( !conClientMessage.message.substr(0,2).compare("<!") )
	{
		bDelete = true;
	}
	CDClientRequest *pRequest = NULL;
	std::string szUnder, szClass;
	int i = szSymbol.find('|');
	if( i == -1 )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateFutureQuoteRequest bad client request: "+conClientMessage.message );			
		return;
	}
	szUnder = szSymbol.substr( 0, i );
	szClass = szSymbol.substr( i+1, -1 );
	try
	{
		Locker locker(&m_RequestLock);
		std::string requestKey = "QF|"+szClass;
		tRequestMap::iterator it = m_RequestMap.find(requestKey);
		if( it == m_RequestMap.end() ) // doesn't exist
		{
			if( bDelete ) 
				return;
			try
			{
				pRequest = new CDCRFutureQuote( szUnder, szClass );
				pRequest->refIncrease();
				m_pConnections->getConTib()->SubscribeFuture( szUnder, szClass );
			}
			catch(...)
			{
				// we expect this if cmask is not available!!!
				delete pRequest;
				Sleep(10);
				m_pConnections->getConClient()->addRequest( conClientMessage );
				return; 
			}
			if( requestKey.empty() )
				return;
			try
			{
				m_RequestMap.insert( std::make_pair<std::string,CDClientRequest *>(requestKey,pRequest) );
			}
			catch(...)
			{
				CMain::getInstance()->m_log.write(LOG_ERROR, "Unknown exception" );
			}
		}
		else
		{
			pRequest = it->second;
			if( bDelete )
			{
				int nSize = pRequest->delClient( nClientId );
				if( nSize == -1 ) // bad client
				{
					return;
				}
				else if( nSize == 0 ) // all gone
				{
					m_RequestMap.erase( it );
					m_pConnections->getConTib()->UnSubscribeFuture( szUnder, szClass );
				}
				try
				{
					pRequest->refDecrease();
				}
				catch(...)
				{
					CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateFutureQuoteRequest refDecrease() exception: "+ conClientMessage.message );			
				}
				return;
			}
			pRequest->refIncrease();
		}
		pRequest->addClient( nClientId );
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateFutureQuoteRequest memory corruption "+conClientMessage.message );			
	}
	try
	{
		std::string reply = m_pDataManager->getFutureQuoteSnapShot( szUnder, szClass );
		if( !reply.empty() )
			m_pConnections->sendToClient( nClientId, reply ); 
	}
	catch(...)
	{
		// log error
		CMain::getInstance()->m_log.write(LOG_ERROR,"RequestManager:updateFutureQuoteRequest send client snapshot exception: "+conClientMessage.message );			
	}
}

std::list<CDClientRequest *> RequestManager::GetRequestsForClient( long nClientId )
{
	std::list<CDClientRequest *> requests;
	Locker locker(&m_RequestLock);
	tRequestMap::iterator it;
	for( it = m_RequestMap.begin(); it != m_RequestMap.end(); it++ )
	{
		CDClientRequest * pRequest = (*it).second;
		IntIntMap clients = pRequest->getClients();
		if( clients.find( nClientId ) != clients.end() )
		{
			pRequest->refIncrease();
			requests.push_back( pRequest );
		}
	}
	return requests;
}

void RequestManager::CheckEntitlement( const CConClientMessage & conClientMessage )
{
	std::string szUser = CMain::getInstance()->getConnections()->getConClient()->getUser( conClientMessage.m_clientId );
	if( conClientMessage.message.find( "QS|" ) != -1 )
	{
		int nPos = conClientMessage.message.find( "QS|" );
		std::string msg = "<REJECT|";
		msg += conClientMessage.message.substr( nPos, -1 );
		if( !CheckStockEntitlement( szUser, conClientMessage.message ) )
			throw EntitlementException(msg.c_str());
	}
	else if( conClientMessage.message.find( "QS2|" ) != -1 )
	{
		int nPos = conClientMessage.message.find( "QS2|" );
		std::string msg = "<REJECT|";
		msg += conClientMessage.message.substr( nPos, -1 );
		if( !CheckStock2Entitlement( szUser, conClientMessage.message ) )
			throw EntitlementException(msg.c_str());
	}
	else if( conClientMessage.message.find( "QO|" ) != -1 )
	{
		int nPos = conClientMessage.message.find( "QO|" );
		std::string msg = "<REJECT|";
		msg += conClientMessage.message.substr( nPos, -1 );
		if( !CheckOptionEntitlement( szUser, conClientMessage.message ) )
			throw EntitlementException(msg.c_str());
	}
	else if( conClientMessage.message.find( "QOB|" ) != -1 )
	{
		int nPos = conClientMessage.message.find( "QOB|" );
		std::string msg = "<REJECT|";
		msg += conClientMessage.message.substr( nPos, -1 );
		if( !CheckOptionBestEntitlement( szUser, conClientMessage.message ) )
			throw EntitlementException(msg.c_str());
	}
	else if( conClientMessage.message.find( "QF|" ) != -1 )
	{
		int nPos = conClientMessage.message.find( "QF|" );
		std::string msg = "<REJECT|";
		msg += conClientMessage.message.substr( nPos, -1 );
		if( !CheckFutureEntitlement( szUser, conClientMessage.message ) )
			throw EntitlementException(msg.c_str());
	}
}

// including index
bool RequestManager::CheckStockEntitlement( const std::string & szUser, const std::string & szMessage )
{
	// <QS|MSFT>, <QS|TD.TO>, <QS|DJI>, <QS|MSFT.NaE>, <QS|MSFT.A>, <QS|MSFT.N>
	std::string szEx, szSymbol, szTemp;

	int nIndex = szMessage.find( "QS|" );
	assert( nIndex != -1 );
	
	szTemp = szMessage.substr( nIndex+3, -1 );

	nIndex = szTemp.find( ">" );
	if( nIndex == -1 )
		return false;

	szTemp = szTemp.substr( 0, nIndex );

	nIndex = szTemp.find(".");
	if( nIndex == -1 ) // no exchange specified, use default rule
	{
		szSymbol = szTemp;
		if( IsIndex( szSymbol ) ) // index use RSF or NaE
		{
			szEx = "NaE";
		}
		else
		{
			if( szSymbol.length() < 4 )
				szEx = "N";
			else
			{
				nIndex = szSymbol.find('^');
				if( nIndex != -1 && nIndex < 4 )
					szEx = "N";
				else
					szEx = "NaE"; // szSymbol += ".O"; 
					
			}
		}
	}
	else
	{
		szSymbol = szTemp.substr( 0, nIndex );
		szEx = szTemp.substr( nIndex+1, -1 );
		if( IsIndex( szSymbol ) ) // index use RSF ???
		{
			szEx = "NaE";
		}
	}


	if( !CMain::getInstance()->m_Entitlement.IsEntitled( szUser, szEx, szSymbol ) )
		return false;

	return true;
}

bool RequestManager::CheckStock2Entitlement( const std::string & szUser, const std::string & szMessage )
{
	std::string szEx, szSymbol, szTemp;

	int nIndex = szMessage.find( "QS2|" );
	assert( nIndex != -1 );
	
	szTemp = szMessage.substr( nIndex+4, -1 );

	nIndex = szTemp.find( ">" );
	if( nIndex == -1 )
		return false;

	szTemp = szTemp.substr( 0, nIndex );

	nIndex = szTemp.find(".");
	if( nIndex == -1 ) // no exchange specified, use default rule
	{
		szSymbol = szTemp;
		if( IsIndex( szSymbol ) ) // index use RSF or NaE
		{
			szEx = "RSF";
		}
		else
		{
			if( szSymbol.length() < 4 )
				szEx = "N";
			else
			{
				nIndex = szSymbol.find('^');
				if( nIndex != -1 && nIndex < 4 )
					szEx = "N";
				else
					szEx = "NaE"; // szSymbol += ".O"; 
					
			}
		}
	}
	else
	{
		szSymbol = szTemp.substr( 0, nIndex );
		szEx = szTemp.substr( nIndex+1, -1 );
		if( IsIndex( szSymbol ) ) // index use RSF ???
		{
			szEx = "RSF";
		}
	}


	if( !CMain::getInstance()->m_Entitlement.IsEntitled( szUser, szEx, szSymbol ) )
		return false;
	
	return true;
}

bool RequestManager::CheckOptionEntitlement( const std::string & szUser, const std::string & szMessage )
{
	// <QO|MSFT|MSQ|A> <QO|INTC|INQEY| > <QO|INTC|INQEY|W>
	std::string szEx, szSymbol, szTemp;
	szTemp = szMessage;
	int nIndex = szTemp.find( "QO" );
	szTemp = szTemp.substr( nIndex+3, -1 );
	nIndex = szTemp.find( "|" );
	if( nIndex == -1 )
		return false;
	szSymbol = szTemp.substr( 0, nIndex );
	nIndex = szTemp.find(">");
	if( nIndex == -1 )
		return false;
	szTemp = szTemp.substr( 0, nIndex );
	nIndex = szTemp.rfind("|");
	if( nIndex == -1 )
		return false;
	szEx = szTemp.substr( nIndex+1, -1 );
	if( !szEx.length() || szEx == " " )
	{
		std::vector<std::string> vExchgs = m_pDataManager->getOptionExchanges( szSymbol );
		if( vExchgs.size() != 0 )
		{
			for( int i = 0; i < vExchgs.size(); i++ )
			{
				szEx = vExchgs[i];
				if( !CMain::getInstance()->m_Entitlement.IsEntitled( szUser, szEx, szSymbol ) )
					return false;
			}
		}
		else
		{

		}
	}
	else
	{
		if( !CMain::getInstance()->m_Entitlement.IsEntitled( szUser, szEx, szSymbol ) )
			return false;
	}
	return true;
}

bool RequestManager::CheckOptionBestEntitlement( const std::string & szUser, const std::string & szMessage )
{
	// <QOB|INTC|INQQY>
	std::string szSymbol, szTemp;
	szTemp = szMessage;
	int nIndex = szTemp.find( "QOB" );
	szTemp = szTemp.substr( nIndex+4, -1 );
	nIndex = szTemp.find( "|" );
	if( nIndex == -1 )
		return false;
	szSymbol = szTemp.substr( 0, nIndex );
	std::string szEx[6] = { "A", "B", "P", "W", "X", "Y" };
	for( int i = 0; i < 6; i++ )
	{
		if( !CMain::getInstance()->m_Entitlement.IsEntitled( szUser, szEx[i], szSymbol ) )
			return false;
	}
	return true;
}

bool RequestManager::CheckFutureEntitlement( const std::string & szUser, const std::string & szMessage )
{
	// <QF|MSFT|MSFT1K4:OX> <QF|NQ|NQ4U>
	std::string szEx, szSymbol, szTemp;
	szTemp = szMessage;
	int nIndex = szTemp.find( "QF" );
	szTemp = szTemp.substr( nIndex+3, -1 );
	nIndex = szTemp.find( "|" );
	if( nIndex == -1 )
		return false;
	szSymbol = szTemp.substr( 0, nIndex );
	szEx = "RSF";
	if( !CMain::getInstance()->m_Entitlement.IsEntitled( szUser, szEx, szSymbol ) )
		return false;

	return true;
}
