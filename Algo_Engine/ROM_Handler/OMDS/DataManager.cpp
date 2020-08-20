// DataManager.cpp: implementation of the CDataManager class.
//
//////////////////////////////////////////////////////////////////////
//
//	The CDataManager stores a copy of everythings
//	so a client request can get it from here rather than going to the tib
//	Quote subjects are derived from data received from previouse calls
//

#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "DataManager.h"
#include "Main.h"
#include "Locker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataManager::CDataManager()
{
	// Can't help it
	// these have to be hard coded
	m_exchAcrMap.insert(StringStringMap::value_type("AMEX","A"));
	m_exchAcrMap.insert(StringStringMap::value_type("A","AMEX"));
	m_exchAcrMap.insert(StringStringMap::value_type("BOSX","B"));
	m_exchAcrMap.insert(StringStringMap::value_type("B","BOSX"));
	m_exchAcrMap.insert(StringStringMap::value_type("CHIX","MW"));
	m_exchAcrMap.insert(StringStringMap::value_type("MW","CHIX"));
	m_exchAcrMap.insert(StringStringMap::value_type("NYSE","N"));
	m_exchAcrMap.insert(StringStringMap::value_type("N","NYSE"));
	m_exchAcrMap.insert(StringStringMap::value_type("NASDAQ","O"));
	m_exchAcrMap.insert(StringStringMap::value_type("O","NASDAQ"));
	m_exchAcrMap.insert(StringStringMap::value_type("PCST","P"));
	m_exchAcrMap.insert(StringStringMap::value_type("P","PCST"));
	m_exchAcrMap.insert(StringStringMap::value_type("CBOE","W"));
	m_exchAcrMap.insert(StringStringMap::value_type("W","CBOE"));
	m_exchAcrMap.insert(StringStringMap::value_type("PHLX","X"));
	m_exchAcrMap.insert(StringStringMap::value_type("X","PHLX"));
	m_exchAcrMap.insert(StringStringMap::value_type("ISE","Y"));
	m_exchAcrMap.insert(StringStringMap::value_type("Y","ISE"));
	m_exchAcrMap.insert(StringStringMap::value_type("MOE","M"));
	m_exchAcrMap.insert(StringStringMap::value_type("M","MOE"));

	m_log.setObjectName("CDataManager");
}

CDataManager::~CDataManager()
{

}

std::string CDataManager::getExchangeAcr(const std::string & exch)
{
	StringStringMap::iterator	it;

	it = m_exchAcrMap.find(exch);
	if (it == m_exchAcrMap.end())
	{
		return exch;
	}
	else
	{
		return (it->second);
	}
}

void CDataManager::shutDown()
{
	m_log.write(LOG_MESSAGE, "DataManager Shutdown...");

	clearStockQuoteMap();
	clearOptionQuoteMap();
	clearOptionSeriesMap();
	clearStock2QuoteMap();
	{
		Locker locker(&m_symbolLock);
		m_symbolMap.clear();
	}

	{
		Locker locker(&m_exAcrLock);
		m_exchAcrMap.clear();
	}
}

void CDataManager::updateStockQuote(CDTMStockQuote * pSQ)
{
	if( pSQ->getBase().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: StockQuote.");
		return;
	}
	Locker locker(&m_stockQuoteLock);
	StringStockQuoteMap::iterator it;
	CDTMStockQuote * pMySq = NULL;

	try
	{
		try
		{
			std::string szKey = pSQ->GetRequestKey();
			/*
			if( szRic[0] == '^' )
			{
				//szRic = szRic.substr(1,-1);
				szRic = pSQ->getBase()+".NaE";
			}
			*/
			it = m_stockQuoteMap.find(szKey);
			if (it == m_stockQuoteMap.end())
			{
				pMySq = new CDTMStockQuote();
				if( !pMySq )
				{
					m_log.write(LOG_ERROR,"updateStockQuote new CDTMStockQuote()"+pSQ->getRic());
					throw exception("Unknown Error");
				}
				*pMySq = *pSQ;
				pMySq->m_changedFields = 0xFFFFFFFF;

				try
				{
					m_stockQuoteMap.insert( std::make_pair<std::string,CDTMStockQuote*>(szKey,pMySq) );
				}
				catch(...)
				{
					m_log.write(LOG_ERROR,"m_stockQuoteMap.insert exception");
					throw exception("Unkown Error");
				}
			}
			else
			{
				pMySq = it->second;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR,"updateStockQuote memory error 1"+pSQ->getSymbol());
			throw exception("Unkown Error");
		}

		try
		{
			if (pSQ->m_changedFields & FIELD_QUOTE_LAST && 
				pSQ->m_changedFields & FIELD_QUOTE_LASTSIZE) 
			{
				pMySq->m_last = pSQ->m_last;
				pMySq->m_lastSize = pSQ->m_lastSize;
			}
			else if (pSQ->m_changedFields & FIELD_QUOTE_LAST && 
				!(pSQ->m_changedFields & FIELD_QUOTE_LASTSIZE)) 
			{
				pMySq->m_last = pSQ->m_last;
				pSQ->m_lastSize = pMySq->m_lastSize;
			}
			else if (!(pSQ->m_changedFields & FIELD_QUOTE_LAST) && 
				pSQ->m_changedFields & FIELD_QUOTE_LASTSIZE) 
			{
				pSQ->m_last = pMySq->m_last;
				pMySq->m_lastSize = pSQ->m_lastSize;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_LAST");
			throw exception("updateStockQuote memory error FIELD_QUOTE_LAST");
		}

		try
		{
			if(pSQ->m_changedFields & FIELD_QUOTE_VOLUME)
			{
				pMySq->m_volume = pSQ->m_volume;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_VOLUME");
			throw exception("updateStockQuote memory error FIELD_QUOTE_VOLUME");
		}

		try
		{
			if(pSQ->m_changedFields & FIELD_QUOTE_NET)
			{
				pMySq->m_change = pSQ->m_change;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_NET");
			throw exception("updateStockQuote memory error FIELD_QUOTE_NET");
		}

		try
		{
			if ( (pSQ->m_changedFields & FIELD_QUOTE_BID) &&
				 (pSQ->m_changedFields & FIELD_QUOTE_BIDSIZE))
			{
				pMySq->m_bid = pSQ->m_bid;
				pMySq->m_bidSize = pSQ->m_bidSize;
			}
			else if ( (pSQ->m_changedFields & FIELD_QUOTE_BID) &&
				     !(pSQ->m_changedFields & FIELD_QUOTE_BIDSIZE) )
			{
				pMySq->m_bid = pSQ->m_bid;
				// get the old bidsize from cache in case it is not updated
				pSQ->m_bidSize = pMySq->m_bidSize;
			}
			else if ( !(pSQ->m_changedFields & FIELD_QUOTE_BID) &&
				      (pSQ->m_changedFields & FIELD_QUOTE_BIDSIZE) )
			{
				// get old bid price from cache in case it is not updated.
				pSQ->m_bid = pMySq->m_bid;
				pMySq->m_bidSize = pSQ->m_bidSize;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_BID");
			throw exception("updateStockQuote memory error FIELD_QUOTE_BID");
		}

		try
		{
			if ( (pSQ->m_changedFields & FIELD_QUOTE_ASK) &&
				 (pSQ->m_changedFields & FIELD_QUOTE_ASKSIZE) )
			{
				pMySq->m_ask = pSQ->m_ask;
				pMySq->m_askSize = pSQ->m_askSize;
			}
			else if ( (pSQ->m_changedFields & FIELD_QUOTE_ASK) &&
					 !(pSQ->m_changedFields & FIELD_QUOTE_ASKSIZE) )
			{
				pMySq->m_ask = pSQ->m_ask;
				// get the old asksize from cache in case it is not updated
				pSQ->m_askSize = pMySq->m_askSize;
			}
			else if (!(pSQ->m_changedFields & FIELD_QUOTE_ASK) &&
					  (pSQ->m_changedFields & FIELD_QUOTE_ASKSIZE) )
			{
				// get the old ask price from cache in case it is not updated.
				pSQ->m_ask = pMySq->m_ask;
				pMySq->m_askSize = pSQ->m_askSize;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_ASK");
			throw exception("updateStockQuote memory error FIELD_QUOTE_ASK");
		}

		try
		{
			if(pSQ->m_changedFields & FIELD_QUOTE_OPEN)
			{
				pMySq->m_open = pSQ->m_open;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_ASK");
			throw exception("updateStockQuote memory error FIELD_QUOTE_ASK");
		}

		try
		{
			if(pSQ->m_changedFields & FIELD_QUOTE_HIGH)
			{
				pMySq->m_high = pSQ->m_high;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_HIGH");
			throw exception("updateStockQuote memory error FIELD_QUOTE_HIGH");
		}

		try
		{
			if(pSQ->m_changedFields & FIELD_QUOTE_LOW)
			{
				pMySq->m_low = pSQ->m_low;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_LOW");
			throw exception("updateStockQuote memory error FIELD_QUOTE_LOW");
		}

		try
		{
			if(pSQ->m_changedFields & FIELD_QUOTE_CLOSE)
			{
				pMySq->m_close = pSQ->m_close;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_CLOSE");
			throw exception("updateStockQuote memory error FIELD_QUOTE_CLOSE");
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "updateStockQuote failed.  Memory error.");
	}
}

double CDataManager::getStockLast(const std::string& symbol)
{
	StringStockQuoteMap::iterator it;
	CDTMStockQuote * pMySq = NULL;

	double last = 0.0;
//	int i = symbol.find('.');
//	std::string ric = symbol.substr(0,i);
	try
	{
		Locker locker(&m_stockQuoteLock);

//		it = m_stockQuoteMap.find(ric);
		it = m_stockQuoteMap.find(symbol);
		if(it != m_stockQuoteMap.end())
		{
			pMySq = it->second;
			last = pMySq->m_last;
		}
	}
	catch(...)
	{
		last = 0.0;
	}

	return last;
}

std::string CDataManager::getStockQuoteSnapShot(const std::string &symbol)
{
	std::string reply = "";
	{
		Locker locker(&m_stockQuoteLock);
		CDTMStockQuote * pQS = NULL;
		StringStockQuoteMap::iterator it = m_stockQuoteMap.find(symbol);
		if( it == m_stockQuoteMap.end() )
			return "";
		reply = (*it).second->CreateResponse();
	}
	return reply;
}

std::string CDataManager::getStock2QuoteSnapShot(const std::string & base)
{
	std::string		reply = "";
	try
	{
		Locker locker(&m_stock2QuoteLock);
		StringStock2QuoteMap::iterator it;
		Stock2QuoteMap::iterator its;
		Stock2Quotes * pQuotes;
		CDTMStock2Quote * pQS2 = NULL;
		it = m_stock2QuoteMap.find(base);
		if(it != m_stock2QuoteMap.end())
		{
			pQuotes = (*it).second;
			if( !pQuotes )
				return "";
			for( its = pQuotes->getQuotes().begin(); its != pQuotes->getQuotes().end(); its++ )
			{
				pQS2 = its->second;
				reply += pQS2->CreateResponse();
				reply += "\n";
			}

		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "getStockQuoteSnapShot failed with a memory error.  Symbol = " + base);
		reply = "";
	}
	return(reply);
}


std::string CDataManager::getOptionQuoteBestSnapShot(const std::string& underlying, const std::string& ric )
{
	std::string							reply = "";
	{
		Locker locker(&m_optionQuoteBestLock);

		StringOptionQuoteBestMap::iterator	mmIt;
		CDTMOptionQuoteBest *				pQOB = NULL;
		try
		{
			mmIt = m_optionQuoteBestMap.find(ric);
			if(mmIt != m_optionQuoteBestMap.end())
			{
				pQOB = mmIt->second;
				reply = pQOB->CreateResponse(underlying);
				m_log.write(LOG_DEBUG, "getOptionQuoteBestSnapShot " + reply);
			}

		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "getOptionQuoteBest failed with a memory error.");
			reply = "";
		}
	}
	return(reply);
}

std::string CDataManager::getOptionQuoteSnapShot(const std::string& underlying, const std::string& ric, const std::string& szExch )
{
	std::string							reply = "";
	{
		Locker locker(&m_optionQuoteLock);
		StringOptionQuoteMap::iterator		mmIt;
		CDTMOptionQuote *					pQO = NULL;
		try
		{
			mmIt = m_optionQuoteMap.find(ric+"|"+szExch);
			if(mmIt != m_optionQuoteMap.end())
			{
				pQO = mmIt->second;
				reply  = pQO->CreateResponse(underlying);
				m_log.write(LOG_DEBUG, "getOptionQuoteSnapShot " + reply);
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "getOptionQuote failed with a memory error.");
			reply = "";
		}
	}
	return(reply);
}

void CDataManager::clearStock2QuoteMap()
{
	Locker locker(&m_stock2QuoteLock);
	StringStock2QuoteMap::iterator it;
	for( it = m_stock2QuoteMap.begin(); it != m_stock2QuoteMap.end(); it++ )
	{
		Stock2Quotes * pQuotes = (*it).second;
		if( !pQuotes )
			continue;
		Stock2QuoteMap::iterator its;
		for( its = pQuotes->getQuotes().begin(); its != pQuotes->getQuotes().end(); its++ )
			delete (*its).second;
		delete pQuotes;
	}
	m_stock2QuoteMap.clear();
}

void CDataManager::clearStockQuoteMap()
{
	Locker locker(&m_stockQuoteLock);

	// Clean the m_stockQuoteMap
	StringStockQuoteMap::iterator ssqmIt;
	for(ssqmIt=m_stockQuoteMap.begin(); ssqmIt!=m_stockQuoteMap.end(); ssqmIt++)
	{
		try
		{
			delete (*ssqmIt).second;
			(*ssqmIt).second = NULL; // set to null
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "shutDown m_stockQuoteMap delete error.");
		}
	}

	try
	{
		m_stockQuoteMap.clear();
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "shutDown m_stockQuoteMap clear error.");
	}
}

void CDataManager::clearOptionQuoteMap()
{
	Locker locker(&m_optionQuoteLock);

	StringOptionQuoteMap::iterator			it;
	// Clean the m_optionQuoteMap
	try
	{
		for(it=m_optionQuoteMap.begin(); it!=m_optionQuoteMap.end(); it++)
		{
			delete (*it).second;
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "shutDown m_optionQuoteMap delete error.");
	}

}

void CDataManager::clearOptionSeriesMap()
{
	Locker locker(&m_optionSeriesLock);

	StringOptionSeriesMap::iterator	scmmIt;
	std::string subject;

	for(scmmIt=m_optionSeriesMap.begin(); scmmIt!=m_optionSeriesMap.end(); scmmIt++)
	{
		try
		{
			subject = (*scmmIt).first;
			delete (*scmmIt).second;
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "shutDown m_optionSeriesMap delete (" + subject + ") error.");
		}
	}
	try
	{
		m_optionSeriesMap.clear();
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "shutDown m_optionSeriesMap clear error.");
	}
}

void CDataManager::updateOptionQuote(CDTMOptionQuote *pOQ)
{
	StringOptionQuoteMap::iterator			it;
	CDTMOptionQuote *						pMyOq = NULL;
	if( pOQ->getSymbol().empty() || pOQ->getExchange().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: OptionQuote.");
		return;
	}
	try
	{
		Locker locker(&m_optionQuoteLock);

		it = m_optionQuoteMap.find(pOQ->getSymbol()+"|"+pOQ->getExchange());
		if (it != m_optionQuoteMap.end())
		{
			pMyOq = it->second;
		}
		else
		{
			pMyOq = new CDTMOptionQuote();
			if( !pMyOq )
			{
				m_log.write(LOG_ERROR, "updateOptionQuote new CDTMOptionQuote failed");
				return;
			}
			*pMyOq = *pOQ;
			pMyOq->m_changedFields = 0xFFFFFFFF;
			std::string key = pOQ->getSymbol()+"|"+pOQ->getExchange();
			m_optionQuoteMap.insert( std::make_pair<std::string,CDTMOptionQuote*>(key,pMyOq) );
		}

		if(pMyOq != NULL)
		{
			//Locker lockerQuote((long) pMyOq);
			pMyOq->setSymbol( pOQ->getSymbol() );

			pMyOq->setExStatus( pOQ->getExStatus() );

			if(pOQ->m_changedFields & FIELD_QUOTE_LAST)
			{
				pMyOq->m_last = pOQ->m_last;
				pMyOq->m_lastSize = pOQ->m_lastSize;
				pMyOq->m_change = pOQ->m_change;
				pMyOq->m_volume = pOQ->m_volume;

			}

			if(pOQ->m_changedFields & FIELD_QUOTE_BID)
			{
				pMyOq->m_bid = pOQ->m_bid;
			}

			if(pOQ->m_changedFields & FIELD_QUOTE_ASK)
			{
				pMyOq->m_ask = pOQ->m_ask;
			}

			if(pOQ->m_changedFields & FIELD_QUOTE_BIDSIZE)
			{
				pMyOq->m_bidSize = pOQ->m_bidSize;
			}

			if(pOQ->m_changedFields & FIELD_QUOTE_ASKSIZE)
			{
				pMyOq->m_askSize = pOQ->m_askSize;
			}
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"updateOptionQuote failed with a memory error.  Shutting down.");
	}
}

void CDataManager::updateOptionQuoteBest(CDTMOptionQuoteBest *pQOB)
{
	StringOptionQuoteBestMap::iterator	mmIt;
	CDTMOptionQuoteBest *				pMyQOB = NULL;
	if( pQOB->getSymbol().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: OptionQuoteBest.");
		return;
	}
	try
	{
		Locker locker(&m_optionQuoteBestLock);
		mmIt = m_optionQuoteBestMap.find(pQOB->getSymbol());
		if(mmIt != m_optionQuoteBestMap.end())
		{
			pMyQOB = mmIt->second;
		}
		else
		{
			pMyQOB = new CDTMOptionQuoteBest();
			if( !pMyQOB )
			{
				m_log.write(LOG_ERROR, "updateOptionQuoteBest new CDTMOptionQuoteBest,"+pQOB->getSymbol());
				return;
			}
			*pMyQOB = *pQOB;
			pMyQOB->m_changedFields = 0xFFFFFFFF;
			m_optionQuoteBestMap.insert( std::make_pair<std::string,CDTMOptionQuoteBest*>(pQOB->getSymbol(),pMyQOB) );
		}

		if(pMyQOB != NULL)
		{
			pMyQOB->setSymbol( pQOB->getSymbol() );

			if(pQOB->m_changedFields & FIELD_QUOTE_LAST)
			{
				pMyQOB->m_last = pQOB->m_last;
			}
			else
			{
				pQOB->m_last = pMyQOB->m_last;
			}

			if(pQOB->m_changedFields & FIELD_QUOTE_LASTSIZE)
			{
				pMyQOB->m_lastSize = pQOB->m_lastSize;
			}
			else
			{
				pQOB->m_lastSize = pMyQOB->m_lastSize;
			}

			if(pQOB->m_changedFields & FIELD_QUOTE_NET)
			{
				pMyQOB->m_change = pQOB->m_change;
			}
			else
			{
				pQOB->m_change = pMyQOB->m_change;
			}

			if(pQOB->m_changedFields & FIELD_QUOTE_VOLUME)
			{
				pMyQOB->m_volume = pQOB->m_volume;
			}
			else
			{
				pQOB->m_volume = pMyQOB->m_volume;
			}

			if(pQOB->m_changedFields & FIELD_QUOTE_BID)
			{
				pMyQOB->m_bid = pQOB->m_bid;
			}
			else
			{
				pQOB->m_bid = pMyQOB->m_bid;
			}

			if(pQOB->m_changedFields & FIELD_QUOTE_ASK)
			{
				pMyQOB->m_ask = pQOB->m_ask;
			}
			else
			{
				pQOB->m_ask = pMyQOB->m_ask;
			}

			if(pQOB->m_changedFields & FIELD_QUOTE_BIDSIZE)
			{
				pMyQOB->m_bidSize = pQOB->m_bidSize;
			}
			else
			{
				pQOB->m_bidSize = pMyQOB->m_bidSize;
			}

			if(pQOB->m_changedFields & FIELD_QUOTE_ASKSIZE)
			{
				pMyQOB->m_askSize = pQOB->m_askSize;
			}
			else
			{
				pQOB->m_askSize = pMyQOB->m_askSize;
			}

			if (pQOB->getAskEx().length() > 0)
			{
				pMyQOB->setAskEx( pQOB->getAskEx() );
			}
			else
			{
				pQOB->setAskEx( pMyQOB->getAskEx() );
			}

			if (pQOB->getBidEx().length() > 0)
			{
				pMyQOB->setBidEx( pQOB->getBidEx() );
			}
			else
			{
				pQOB->setBidEx( pMyQOB->getBidEx() );
			}

			if (pQOB->getLastEx().length() > 0)
			{
				pMyQOB->setLastEx( pQOB->getLastEx() );
			}
			else
			{
				pQOB->setLastEx( pMyQOB->getLastEx() );
			}
		}
		else
		{
			m_log.write(LOG_ERROR, "updateOptionQuoteBest failed to store quote for " + pQOB->getSymbol());
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "updateOptionQuoteBest failed with a memory error.");
	}
}

void CDataManager::addSymbolMap(const std::string & key, const std::string & value)
{
	Locker locker(&m_symbolLock);

	if( key.length() < 1 )
		return;

	StringStringMap::iterator it;

	it = m_symbolMap.find(key);
	if (it == m_symbolMap.end())
	{
		m_symbolMap.insert(StringStringMap::value_type(key, value.length() < 1 ? key : value));
	}
}

std::string CDataManager::getOptionSeriesSnapShot(const std::string & szBase )
{
	return "";
	std::string reply = "";
	{
		std::string szKey = szBase;
		std::string szEx = "";
		int nIndex = szBase.find( "." );
		if( nIndex != -1 )
		{
			szKey = szBase.substr(0,nIndex);
			szEx = szBase.substr(nIndex+1,-1);
		}
		Locker locker(&m_optionSeriesLock);
		StringOptionSeriesMap::iterator it = m_optionSeriesMap.find( szKey );
		CDTOptionSeries * pSeries = NULL;
		if( it == m_optionSeriesMap.end() )
			return "";
		pSeries = (*it).second;
		reply = pSeries->CreateResponse(szEx);
	}
	return reply;
}

void CDataManager::updateStock2Quote( CDTMStock2Quote * pQS2 )
{
	if( pQS2->getBase().empty() || pQS2->getSymbol().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: Stock2Quote.");
		return;
	}


	Locker locker(&m_stock2QuoteLock);

	StringStock2QuoteMap::iterator it;
	Stock2QuoteMap::iterator its;
	CDTMStock2Quote * pMySq	= NULL;
	Stock2Quotes * pQuotes = NULL;
	try
	{
		try
		{
			it = m_stock2QuoteMap.find(pQS2->getBase());
			if( it == m_stock2QuoteMap.end())
			{
				pQuotes = new Stock2Quotes();
				m_stock2QuoteMap.insert( std::make_pair<std::string,Stock2Quotes*>(pQS2->getBase(),pQuotes) );
			}
			else
			{
				pQuotes = (*it).second;
			}

			its = pQuotes->getQuotes().find(pQS2->getSymbol());
			if( its == pQuotes->getQuotes().end() )
			{
				pMySq = new CDTMStock2Quote();
				if( !pMySq )
				{
					m_log.write(LOG_ERROR,"updateStockQuote new CDTMStockQuote()"+pQS2->getBase());
					throw exception("Unknown Error");
				}
				*pMySq = *pQS2;
				pMySq->m_changedFields = 0xFFFFFFFF;
				pQuotes->getQuotes().insert( std::make_pair<std::string,CDTMStock2Quote*>(pQS2->getSymbol(),pMySq) );
			}
			else
			{
				pMySq = (*its).second;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR,"updateStock2Quote memory error 1"+pQS2->getSymbol());
			throw exception("Unkown Error");
		}

		try
		{
			if ( (pQS2->m_changedFields & FIELD_QUOTE_BID) &&
				 (pQS2->m_changedFields & FIELD_QUOTE_BIDSIZE))
			{
				pMySq->m_bid = pQS2->m_bid;
				pMySq->m_bidSize = pQS2->m_bidSize;
			}
			else if ( (pQS2->m_changedFields & FIELD_QUOTE_BID) &&
				     !(pQS2->m_changedFields & FIELD_QUOTE_BIDSIZE) )
			{
				pMySq->m_bid = pQS2->m_bid;
				// get the old bidsize from cache in case it is not updated
				pQS2->m_bidSize = pMySq->m_bidSize;
			}
			else if ( !(pQS2->m_changedFields & FIELD_QUOTE_BID) &&
				      (pQS2->m_changedFields & FIELD_QUOTE_BIDSIZE) )
			{
				// get old bid price from cache in case it is not updated.
				pQS2->m_bid = pMySq->m_bid;
				pMySq->m_bidSize = pQS2->m_bidSize;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_BID");
			throw exception("updateStockQuote memory error FIELD_QUOTE_BID");
		}

		try
		{
			if ( (pQS2->m_changedFields & FIELD_QUOTE_ASK) &&
				 (pQS2->m_changedFields & FIELD_QUOTE_ASKSIZE) )
			{
				pMySq->m_ask = pQS2->m_ask;
				pMySq->m_askSize = pQS2->m_askSize;
			}
			else if ( (pQS2->m_changedFields & FIELD_QUOTE_ASK) &&
					 !(pQS2->m_changedFields & FIELD_QUOTE_ASKSIZE) )
			{
				pMySq->m_ask = pQS2->m_ask;
				// get the old asksize from cache in case it is not updated
				pQS2->m_askSize = pMySq->m_askSize;
			}
			else if (!(pQS2->m_changedFields & FIELD_QUOTE_ASK) &&
					  (pQS2->m_changedFields & FIELD_QUOTE_ASKSIZE) )
			{
				// get the old ask price from cache in case it is not updated.
				pQS2->m_ask = pMySq->m_ask;
				pMySq->m_askSize = pQS2->m_askSize;
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "updateStockQuote memory error FIELD_QUOTE_ASK");
			throw exception("updateStockQuote memory error FIELD_QUOTE_ASK");
		}

	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "updateStock2Quote failed.  Memory error.");
	}
}

bool CDataManager::hasOptionSeries( const std::string & szBase )
{
	Locker locker(&m_optionSeriesLock);
	StringOptionSeriesMap::iterator it = m_optionSeriesMap.find( szBase );
	if( it != m_optionSeriesMap.end() )
	{
		return true;
	}
	return false;
}

void CDataManager::updateOptionSeries( CDTOptionSeries * pOS )
{
	if( pOS->getBase().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: OptionSeries.");
		return;
	}
	Locker locker(&m_optionSeriesLock);
	StringOptionSeriesMap::iterator it = m_optionSeriesMap.find( pOS->getBase() );
	if( it != m_optionSeriesMap.end() )
	{
		*( (*it).second ) = *pOS;
		return;
	}
	CDTOptionSeries * pSeries = new CDTOptionSeries(pOS->getBase());
	try
	{
		*pSeries = *pOS;
		m_optionSeriesMap.insert(std::make_pair<std::string,CDTOptionSeries*>(pOS->getBase(),pSeries) );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "updateOptionSeries failed.  Memory error.");
	}
}

std::vector<std::string> CDataManager::getOptionExchanges( const std::string & szBase )
{
	Locker locker(&m_optionSeriesLock);
	std::vector<std::string> vExchanges;
	StringOptionSeriesMap::iterator it = m_optionSeriesMap.find( szBase );
	if( it == m_optionSeriesMap.end() )
		return vExchanges;
	try
	{
		std::string ex = (*it).second->m_OptionSeries.m_optionExchange;
		ex += (*it).second->m_OptionSeries.m_optionAditionalExchanges;
		int index = -1; 
		while( index = ex.find('.') != -1 )
		{
			vExchanges.push_back( ex.substr(0,index) );
			ex = ex.substr( index+1, -1 );
		}
		vExchanges.push_back( ex );
		int nSize = vExchanges.size();
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "getOptionExchanges failed.  Memory error.");
	}

	return vExchanges;
}

COptionSeries CDataManager::getOptionSeries	( const std::string & szBase )
{
	Locker locker(&m_optionSeriesLock);
	StringOptionSeriesMap::iterator it = m_optionSeriesMap.find( szBase );
	if( it == m_optionSeriesMap.end() )
		throw exception("no option series");
	return (*it).second->m_OptionSeries;
}

std::string CDataManager::getGreekDeltaSnapShot( const std::string & szBase )
{
	std::string		reply = "";
	try
	{
		Locker locker(&m_greekLock);
		StringGreekMap::iterator it;
		GreekMap::iterator itg;
		Greeks * pGreeks = NULL;
		CDTMGreeks * pTM = NULL;
		it = m_greekMap.find(szBase);
		if(it != m_greekMap.end())
		{
			pGreeks = (*it).second;
			if( !pGreeks )
				return "";
			for( itg = pGreeks->getGreeks().begin(); itg != pGreeks->getGreeks().end(); itg++ )
			{
				pTM = itg->second;
				reply += pTM->CreateResponse(szBase);
			}
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "getGreekDeltaSnapShot failed with a memory error.  Symbol = " + szBase);
		reply = "";
	}
	return(reply);
}

void CDataManager::updateGreeks(CDTMGreeks *pG)
{
	Locker locker(&m_greekLock);
	if( pG->getBase().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: Greeks.");
		return;
	}

	StringGreekMap::iterator it;
	GreekMap::iterator its;
	CDTMGreeks * pMyG	= NULL;
	Greeks * pGreeks = NULL;
	try
	{
		it = m_greekMap.find(pG->getBase());
		if( it == m_greekMap.end())
		{
			pGreeks = new Greeks();
			m_greekMap.insert( std::make_pair<std::string,Greeks*>(pG->getBase(),pGreeks) );
		}
		else
		{
			pGreeks = (*it).second;
		}

		its = pGreeks->getGreeks().find(pG->getExpiration());
		if( its == pGreeks->getGreeks().end() )
		{
			pMyG = new CDTMGreeks();
			if( !pMyG )
			{
				m_log.write(LOG_ERROR,"updateGreeks new CDTMGreek()"+pG->getBase());
				throw exception("Unknown Error");
			}
			*pMyG = *pG;
			pGreeks->getGreeks().insert( std::make_pair<std::string,CDTMGreeks*>(pG->getExpiration(),pMyG) );
		}
		else
		{
			pMyG = (*its).second;
			*pMyG = *pG;
		}

	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "updateGreeks failed.  Memory error.");
	}
}

void CDataManager::updateFutureSeries( CDTFutureSeries * pFS )
{
	if( pFS->getBase().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: FutureSeries.");
		return;
	}
	Locker locker(&m_futureSeriesLock);
	StringFutureSeriesMap::iterator it = m_futureSeriesMap.find( pFS->getBase() );
	if( it != m_futureSeriesMap.end() )
		return;
	CDTFutureSeries * pSeries = new CDTFutureSeries(pFS->getBase());
	try
	{
		*pSeries = *pFS;
		m_futureSeriesMap.insert(std::make_pair<std::string,CDTFutureSeries*>(pFS->getBase(),pSeries) );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR, "updateFutureSeries failed.  Memory error.");
	}
}

void CDataManager::updateFutureQuote( CDTMFutureQuote * pQF )
{
	StringFutureQuoteMap::iterator			it;
	CDTMFutureQuote *						pMyOq = NULL;
	if( pQF->getSymbol().empty() )
	{
		m_log.write(LOG_DEBUG, "Bad TibMsg: FutureQuote.");
		return;
	}
	try
	{
		Locker locker(&m_futureQuoteLock);

		it = m_futureQuoteMap.find(pQF->getSymbol());
		if (it != m_futureQuoteMap.end())
		{
			pMyOq = it->second;
		}
		else
		{
			pMyOq = new CDTMFutureQuote();
			if( !pMyOq )
			{
				m_log.write(LOG_ERROR, "updateFutureQuote new CDTMFutureQuote failed");
				return;
			}
			*pMyOq = *pQF;
			pMyOq->m_changedFields = 0xFFFFFFFF;
			std::string key = pQF->getSymbol();
			m_futureQuoteMap.insert( std::make_pair<std::string,CDTMFutureQuote*>(key,pMyOq) );
		}

		if(pMyOq != NULL)
		{
			pMyOq->setSymbol( pQF->getSymbol() );

			if(pQF->m_changedFields & FIELD_QUOTE_LAST)
			{
				pMyOq->m_last = pQF->m_last;
				pMyOq->m_lastSize = pQF->m_lastSize;
				pMyOq->m_change = pQF->m_change;
				pMyOq->m_volume = pQF->m_volume;

			}

			if(pQF->m_changedFields & FIELD_QUOTE_BID)
			{
				pMyOq->m_bid = pQF->m_bid;
			}

			if(pQF->m_changedFields & FIELD_QUOTE_ASK)
			{
				pMyOq->m_ask = pQF->m_ask;
			}

			if(pQF->m_changedFields & FIELD_QUOTE_BIDSIZE)
			{
				pMyOq->m_bidSize = pQF->m_bidSize;
			}

			if(pQF->m_changedFields & FIELD_QUOTE_ASKSIZE)
			{
				pMyOq->m_askSize = pQF->m_askSize;
			}

			if(pQF->m_changedFields & FIELD_QUOTE_OPEN)
			{
				pMyOq->m_open = pQF->m_open;
			}

			if(pQF->m_changedFields & FIELD_QUOTE_HIGH)
			{
				pMyOq->m_high = pQF->m_high;
			}

			if(pQF->m_changedFields & FIELD_QUOTE_LOW)
			{
				pMyOq->m_low = pQF->m_low;
			}

			if(pQF->m_changedFields & FIELD_QUOTE_CLOSE)
			{
				pMyOq->m_close = pQF->m_close;
			}
		}
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"updateFutureQuote failed with a memory error.  Shutting down.");
	}
}

std::string CDataManager::getFutureSeriesSnapShot( const std::string & szBase )
{
	std::string reply = "";
	{
		Locker locker(&m_futureSeriesLock);
		StringFutureSeriesMap::iterator it = m_futureSeriesMap.find( szBase );
		CDTFutureSeries * pSeries = NULL;
		if( it == m_futureSeriesMap.end() )
			return "";
		pSeries = (*it).second;
		reply = pSeries->CreateResponse();
	}
	return reply;
}

std::string CDataManager::getFutureQuoteSnapShot( const std::string & szBase, const std::string & szSymbol )
{
	std::string							reply = "";
	{
		Locker locker(&m_futureQuoteLock);
		StringFutureQuoteMap::iterator		mmIt;
		CDTMFutureQuote *					pQF = NULL;
		try
		{
			mmIt = m_futureQuoteMap.find(szSymbol);
			if(mmIt != m_futureQuoteMap.end())
			{
				pQF = mmIt->second;
				reply  = pQF->CreateResponse(szBase);
				m_log.write(LOG_DEBUG, "getFutureQuoteSnapShot " + reply);
			}
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "getFutureQuote failed with a memory error.");
			reply = "";
		}
	}
	return(reply);
}