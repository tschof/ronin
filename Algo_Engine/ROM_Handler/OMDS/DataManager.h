// DataManager.h: interface for the CDataManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAMANAGER_H__51E7A3BD_12E5_42C0_ACB8_7F755A3FC6F3__INCLUDED_)
#define AFX_DATAMANAGER_H__51E7A3BD_12E5_42C0_ACB8_7F755A3FC6F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
#include "DTibMessage.h"
#include "OptionSeries.h"

using namespace WinRVTool;

typedef std::map<std::string,CDTOptionSeries*> StringOptionSeriesMap;
typedef std::map<std::string,CDTMStock2Quote*> Stock2QuoteMap;

typedef std::map<std::string,CDTFutureSeries*> StringFutureSeriesMap;

class Stock2Quotes
{
public:
	Stock2Quotes	() {}
	~Stock2Quotes	() {}
	Stock2QuoteMap & getQuotes() { return m_Stock2Quotes; }
private:
	Stock2QuoteMap	m_Stock2Quotes;
};

typedef std::map<std::string, Stock2Quotes *> StringStock2QuoteMap;

typedef std::map<std::string,CDTMGreeks*> GreekMap;

class Greeks
{
public:
	Greeks(){}
	virtual ~Greeks(){}
	GreekMap & getGreeks() { return m_Greeks; }
private:
	GreekMap	m_Greeks;
};

typedef std::map<std::string, Greeks *> StringGreekMap;

class CDataManager
{
public:
	CDataManager();
	virtual ~CDataManager();

	void		addSymbolMap(const std::string & key, const std::string & value);
	double		getStockLast(const std::string& symbol);
	std::vector<std::string> getOptionExchanges( const std::string & szBase );

	void		shutDown();

	std::string	getExchangeAcr				( const std::string & exch );
	std::string	getStockQuoteSnapShot		( const std::string & szBase );		// Level 1
	std::string	getStock2QuoteSnapShot		( const std::string & szBase );		// Level 2
	std::string	getOptionQuoteSnapShot		( const std::string & szBase, const std::string & szSymbol, const std::string & szExch );	// Option Best
	std::string	getOptionQuoteBestSnapShot	( const std::string & szBase, const std::string & szSymbol );	// Option Best
	std::string	getOptionSeriesSnapShot		( const std::string & szBase );
	std::string getGreekDeltaSnapShot		( const std::string & szBase );
	std::string getFutureSeriesSnapShot		( const std::string & szBase );
	std::string getFutureQuoteSnapShot		( const std::string & szBase, const std::string & szSymbol );

	bool	hasOptionSeries			( const std::string & szBase );
	void	updateOptionSeries		( CDTOptionSeries * pOS );
	void	updateGreeks			( CDTMGreeks * pG );
	void	updateStockQuote		( CDTMStockQuote * pQS );
	void	updateStock2Quote		( CDTMStock2Quote * PQS2 );
	void	updateOptionQuote		( CDTMOptionQuote * pQO );
	void	updateOptionQuoteBest	( CDTMOptionQuoteBest * pQOB );
	void	updateFutureSeries		( CDTFutureSeries * pFS );
	void	updateFutureQuote		( CDTMFutureQuote * pQF );

	COptionSeries getOptionSeries	( const std::string & szBase );
	CFutureSeries getFutureSeries	( const std::string & szBase );

private:

	void	clearStockQuoteMap		();
	void	clearStock2QuoteMap		();
	void	clearOptionQuoteMap		();
	void	clearOptionSeriesMap	();
	void	optionBestQuoteClear	( std::string ric );

	CLog						m_log;
	
	StringOptionSeriesMap		m_optionSeriesMap;
	Lock						m_optionSeriesLock;


	StringStringMap				m_exchAcrMap;
	Lock						m_exAcrLock;

	StringStockQuoteMap			m_stockQuoteMap;
	Lock						m_stockQuoteLock;

	StringStock2QuoteMap		m_stock2QuoteMap;
	Lock						m_stock2QuoteLock;

	StringOptionQuoteMap		m_optionQuoteMap;
	Lock						m_optionQuoteLock;

	StringStringMap				m_symbolMap;
	Lock						m_symbolLock;

	StringOptionQuoteBestMap	m_optionQuoteBestMap;
	Lock						m_optionQuoteBestLock;

	StringGreekMap				m_greekMap;
	Lock						m_greekLock;

	StringFutureSeriesMap		m_futureSeriesMap;
	Lock						m_futureSeriesLock;

	StringFutureQuoteMap		m_futureQuoteMap;
	Lock						m_futureQuoteLock;

};

#endif // !defined(AFX_DATAMANAGER_H__51E7A3BD_12E5_42C0_ACB8_7F755A3FC6F3__INCLUDED_)
