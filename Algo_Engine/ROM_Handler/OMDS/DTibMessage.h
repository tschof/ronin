// DTibMessage.h: interface for the CDTibMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DTIBMESSAGE_H__7C0ABC13_23E7_40C5_8A0E_4C5A7B5E8393__INCLUDED_)
#define AFX_DTIBMESSAGE_H__7C0ABC13_23E7_40C5_8A0E_4C5A7B5E8393__INCLUDED_

#include "OMDScommon.h"
#include "OptionSeries.h"
#include "FutureSeries.h"
#include "Main.h"
using namespace WinRVTool;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DATATIBMESSAGE_BADMESSAGE		0x10000000
#define DATATIBMESSAGE_LINK				0x10000001
#define DATATIBMESSAGE_CONTRACTMASK		0x00000002
#define DATATIBMESSAGE_GREEKS			0x00000004
#define DATATIBMESSAGE_STOCKQUOTE		0x00000008
#define DATATIBMESSAGE_OPTIONQUOTE		0x00000010
#define DATATIBMESSAGE_OPTIONQUOTEBEST	0x00000020
#define DATATIBMESSAGE_STOCK2QUOTE		0x00000040
#define DATATIBMESSAGE_OPTIONSERIES		0x00000080
#define DATATIBMESSAGE_FUTURESERIES		0x00000100
#define DATATIBMESSAGE_FUTUREQUOTE		0x00000200
// ====================================================================
// virtual Data Tib Message
// ====================================================================
class CDateConverter
{
public:
	std::string	convertDate( const std::string &exp )
	{
//			std::string response = m_ssMap[exp.substr(2,2)] + " 20" + exp.substr(0,2) ;
		std::string response = "20" + exp.substr(0,2) + "|" + exp.substr(2,2);
		return(response);
	}
	CDateConverter()
	{
		m_ssMap.insert(StringStringMap::value_type("01", "Jan"));
		m_ssMap.insert(StringStringMap::value_type("02", "Feb"));
		m_ssMap.insert(StringStringMap::value_type("03", "Mar"));
		m_ssMap.insert(StringStringMap::value_type("04", "Apr"));
		m_ssMap.insert(StringStringMap::value_type("05", "May"));
		m_ssMap.insert(StringStringMap::value_type("06", "Jun"));
		m_ssMap.insert(StringStringMap::value_type("07", "Jul"));
		m_ssMap.insert(StringStringMap::value_type("08", "Aug"));
		m_ssMap.insert(StringStringMap::value_type("09", "Sep"));
		m_ssMap.insert(StringStringMap::value_type("10", "Oct"));
		m_ssMap.insert(StringStringMap::value_type("11", "Nov"));
		m_ssMap.insert(StringStringMap::value_type("12", "Dec"));
	};
	virtual ~CDateConverter() {};
	static std::string f2s(float f, int nPrecision = 2);
	static std::string l2s(long l);

private:
	StringStringMap		m_ssMap;
};

class RequestManager;
class CDClientRequest;
class CDataManager;

class CDTibMessage
{
public:
	CDTibMessage();
	virtual ~CDTibMessage();

	CDTibMessage( const CDTibMessage & msg );
	CDTibMessage & operator=( const CDTibMessage & msg );

	virtual void		Process			();
	virtual std::string GetClientRequest() { return ""; }
	virtual std::string CreateResponse	( const std::string & szUnder ) { return ""; }
	virtual void		updateCache		() {}

	int					getRetry		() { return m_nRetry; }
	void				setRetry		( int nRetry )	{ m_nRetry = nRetry; }

	int					getType			() { return m_type; };

	std::string			getSubject		() { return m_subject; }
	std::string 		getBase			() { return m_base; }
	std::string			getSymbol		() { return m_symbol; }
	std::string			getRic			() { return m_ric; }
	std::string			getSecType		() { return m_secType; }

	void				setSubject		( const std::string & subject );
	void				setBase			( const std::string & szBase );
	void				setSymbol		( const std::string & szSymbol );
	void				setRic			( const std::string & szRic );
	void				setSecType		( const std::string & szSecType );

protected:
	
	RequestManager *	m_pRequestManager;
	CDataManager *		m_pDataManager;

	int					m_type;
	std::string			m_subject;
	std::string			m_base;		// MSFT		DJI		MSFT	
	std::string			m_ric;		// MSFT		^DJI	ZMFAP
	std::string			m_symbol;	// MSFT		.DJI	ZMFAP
	std::string			m_secType;	// E, I		
	int					m_nRetry;
};

class CDTMGreeks : public CDTibMessage
{
public:
	CDTMGreeks() : m_expiration("") {m_type = DATATIBMESSAGE_GREEKS;};
	virtual ~CDTMGreeks() {};

	CDTMGreeks( const CDTMGreeks & msg );
	CDTMGreeks & operator=( const CDTMGreeks & msg );
	
	virtual std::string	CreateResponse	(const std::string & szUnder);
	virtual std::string GetClientRequest()
	{ 
		if( m_base.empty() )
			return "";
		return "GD|"+m_base; 
	}
	virtual void		updateCache		();
	std::string			getExpiration() { return m_expiration; }
	void				setExpiration( const std::string & exp );
public:
	std::string			m_expiration;
	tFloatStrikeMap		m_greekMap;
};

#define FIELD_QUOTE_BID			0x0001
#define FIELD_QUOTE_BIDSIZE		0x0002
#define FIELD_QUOTE_ASK			0x0004
#define FIELD_QUOTE_ASKSIZE		0x0008
#define FIELD_QUOTE_LAST		0x0010
#define FIELD_QUOTE_LASTSIZE	0x0020
#define FIELD_QUOTE_OPEN		0x0040
#define FIELD_QUOTE_HIGH		0x0080
#define FIELD_QUOTE_LOW			0x0100
#define FIELD_QUOTE_CLOSE		0x0200
#define FIELD_QUOTE_NET			0x0400
#define FIELD_QUOTE_VOLUME		0x0800
#define FIELD_QUOTE_PRCTICK		0x1000
#define FIELD_QUOTE_DISPLAY		0x2000

class CDTMOptionQuoteBest : public CDTibMessage
{
public:
	CDTMOptionQuoteBest();
	virtual ~CDTMOptionQuoteBest();
	CDTMOptionQuoteBest( const CDTMOptionQuoteBest & msg );
	CDTMOptionQuoteBest & operator=( const CDTMOptionQuoteBest & msg );

	virtual std::string	CreateResponse	(const std::string & szUnder);
	virtual std::string GetClientRequest()
	{ 
		if( m_symbol.empty() )
			return "";
		return "QOB|"+m_symbol; 
	}
	virtual void		updateCache		();

	std::string	getAskEx	() { return m_askEx; }
	std::string	getBidEx	() { return m_bidEx; }
	std::string	getLastEx	() { return m_lastEx; }

	void		setAskEx	( const std::string & szAskEx );
	void		setBidEx	( const std::string & szBidEx );
	void		setLastEx	( const std::string & szLastEx );

	int			m_bidSize;
	int			m_askSize;
	int			m_lastSize;
	int			m_changedFields;

	long		m_volume;
	float		m_last;
	float		m_bid;
	float		m_ask;
	float		m_delta;
	float		m_change;
private:
	std::string	m_askEx;
	std::string	m_bidEx;
	std::string	m_lastEx;
};

typedef std::map<std::string, CDTMOptionQuoteBest *> StringOptionQuoteBestMap;

class CDTMOptionQuote : public CDTibMessage
{
public:
	CDTMOptionQuote();
	virtual ~CDTMOptionQuote();
	CDTMOptionQuote( const CDTMOptionQuote & msg );
	CDTMOptionQuote & operator=( const CDTMOptionQuote & msg );

	virtual std::string	CreateResponse	(const std::string & szUnder);
	virtual std::string GetClientRequest()
	{ 
		if( m_symbol.empty() || m_exchange.empty() )
			return "";
		return "QO|"+m_symbol+"|"+m_exchange; 
	}
	virtual void		updateCache		();

	std::string	getExStatus	() { return m_exchangeStatus; }
	std::string	getExchange	() { return m_exchange; }

	void		setExStatus	( const std::string & szStatus );
	void		setExchange	( const std::string & szExchg );

	
	int			m_changedFields;
	long		m_volume;
	float		m_last;
	float		m_bid;
	float		m_ask;
	float		m_delta;
	float		m_change;
	int			m_bidSize;
	int			m_askSize;
	int			m_lastSize;
protected:
	std::string	m_exchangeStatus;
	std::string m_exchange;
};

typedef std::map<std::string, CDTMOptionQuote *> StringOptionQuoteMap;

class CDTMStockQuote : public CDTMOptionQuote
{
public:
	CDTMStockQuote();
	virtual ~CDTMStockQuote();
	CDTMStockQuote( const CDTMStockQuote & msg );
	CDTMStockQuote & operator=( const CDTMStockQuote & msg );

	std::string GetRequestKey()
	{
		int nPos = m_subject.rfind(".");
		if( nPos == -1 )
			return m_base+"."+m_exchange;
		std::string szEx = m_subject.substr( nPos, -1 );
		
		nPos = m_ric.find("/");
		if( nPos != -1 )
		{
			std::string temp = m_base;
			temp[nPos] = '^';
			return temp+"."+m_exchange;
		}

		return m_base+szEx;
		
	}
	
	virtual std::string GetClientRequest()
	{ 
		return "QS|"+GetRequestKey();
		/*
		if( m_ric.empty() )
			return "";
		if( m_ric[0] == '^')
			return "QS|"+m_base+".NaE";
		if( m_ric.find(".TO") != -1 )
			return "QS|"+m_ric;
		int nPos = m_ric.find("/");
		if( nPos != -1 )
		{
			std::string temp = m_ric;
			temp[nPos] = '^';
			return "QS|"+temp;
		}
		
		if( m_exchange == "O" )
			return "QS|"+m_base+".NaE";
		return "QS|"+m_base+"."+m_exchange;
		*/
	}
	virtual std::string	CreateResponse	(const std::string & szUnder);
	virtual void		updateCache		();

	std::string			CreateResponse	();

	float	m_open;
	float	m_high;
	float	m_low;
	float	m_close;
	std::string			m_szPrcTick;
	std::string			m_szDisplay;
};

class CDTMStock2Quote : public CDTibMessage
{
public:
	CDTMStock2Quote();
	virtual ~CDTMStock2Quote();
	CDTMStock2Quote( const CDTMStock2Quote & msg );
	CDTMStock2Quote & operator=( const CDTMStock2Quote & msg );

	virtual std::string GetClientRequest()
	{
		if( m_base.empty() )
			return "";
		return "QS2|"+m_base; 
	}
	virtual std::string	CreateResponse	(const std::string & szUnder);
	virtual void		updateCache		();

	std::string			CreateResponse	();

	std::string	getMM	() { return m_mmname; }
	void		setMM	( const std::string & szMMName );

	int		m_changedFields;
	float	m_bid;
	int		m_bidSize;
	float	m_ask;
	float	m_askSize;
private:
	std::string m_mmname;
};

typedef std::map<std::string, CDTMStockQuote *> StringStockQuoteMap;

class CDTOptionSeries : public CDTibMessage
{
public:
	CDTOptionSeries( const std::string & szUnder );
	virtual ~CDTOptionSeries() {}
	CDTOptionSeries( const CDTOptionSeries & msg );
	CDTOptionSeries & operator=( const CDTOptionSeries & msg );

	virtual void		Process			();
	virtual std::string GetClientRequest()
	{ 
		if( m_base.empty() )
			return "";
		return "OS|"+m_base; 
	}
	virtual std::string CreateResponse	( const std::string & szUnder );

	virtual void		updateCache		();
	std::string			CreateResponse	();

	void setSeries( const COptionSeries & series ) { m_OptionSeries = series; }
public:
	COptionSeries	m_OptionSeries;
};


class CDTFutureSeries : public CDTibMessage
{
public:
	CDTFutureSeries( const std::string & szUnder );
	virtual ~CDTFutureSeries() {}
	CDTFutureSeries( const CDTFutureSeries & msg );
	CDTFutureSeries & operator=( const CDTFutureSeries & msg );

	virtual void		Process			();
	virtual std::string GetClientRequest()
	{ 
		if( m_base.empty() )
			return "";
		return "FS|"+m_base; 
	}
	virtual std::string CreateResponse	( const std::string & szUnder );
	virtual void		updateCache		();
	std::string			CreateResponse	();

public:
	CFutureSeries	m_FutureSeries;
};

class CDTMFutureQuote : public CDTibMessage
{
public:
	CDTMFutureQuote();
	virtual ~CDTMFutureQuote() {}
	CDTMFutureQuote( const CDTMFutureQuote & msg );
	CDTMFutureQuote & operator=( const CDTMFutureQuote & msg );

	virtual std::string	CreateResponse	(const std::string & szUnder);
	virtual std::string GetClientRequest()
	{ 
		if( m_symbol.empty() )
			return "";
		return "QF|"+m_symbol; 
	}
	virtual void		updateCache		();

	int			m_changedFields;
	long		m_volume;
	float		m_open;
	float		m_high;
	float		m_low;
	float		m_close;
	float		m_last;
	float		m_bid;
	float		m_ask;
	float		m_change;
	int			m_bidSize;
	int			m_askSize;
	int			m_lastSize;
};


class CDTCMask : public CDTibMessage
{
public:
	CDTCMask( const std::string & szUnder );
	virtual ~CDTCMask() {}

	virtual void		Process			();
};

typedef std::map<std::string, CDTMFutureQuote *> StringFutureQuoteMap;

typedef std::list<CDTibMessage *> DataTibMsgList;

#endif // !defined(AFX_DTIBMESSAGE_H__7C0ABC13_23E7_40C5_8A0E_4C5A7B5E8393__INCLUDED_)
