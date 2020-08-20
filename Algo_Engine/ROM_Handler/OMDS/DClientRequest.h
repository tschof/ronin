// DClientRequest.h: interface for the CDClientRequest class.
//
//////////////////////////////////////////////////////////////////////
//
//	Client Request Data object
//

#if !defined(AFX_DCLIENTREQUEST_H__D86EE61B_1CF6_4BD5_91DA_A12386566C64__INCLUDED_)
#define AFX_DCLIENTREQUEST_H__D86EE61B_1CF6_4BD5_91DA_A12386566C64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConClientMessage.h"
#include "OMDSCommon.h"

#define DATACLIENTREQUEST_REREQUEST			0x00000001
#define DATACLIENTREQUEST_DELETEALL			0x00000002
#define DATACLIENTREQUEST_OPTIONSERIES		0x00000004
#define DATACLIENTREQUEST_STOCKQUOTE		0x00000008
#define DATACLIENTREQUEST_L2STOCKQUOTE		0x00000010
#define DATACLIENTREQUEST_OPTIONQUOTE		0x00000020
#define DATACLIENTREQUEST_OPTIONQUOTEBEST	0x00000040
#define DATACLIENTREQUEST_GREEKDELTA		0x00000080
#define DATACLIENTREQUEST_FUTURESERIES		0x00000100
#define DATACLIENTREQUEST_FUTUREQUOTE		0x00000200
// ====================================================================
// virtual Data Client Request
// ====================================================================
class CDCRStockQuote;
class CDCRL2StockQuote;
class CDCROptionQuote;
class CDCROptionQuoteBest;
class CDCROptionSeries;

// request key is composed of QO|MSFT, QS|MSFT, QS|MSFT2, OS|MSFT, etc

class CDClientRequest  : public CConClientMessage
{
public:
	CDClientRequest();
	virtual ~CDClientRequest();

	void refIncrease()
	{
		InterlockedIncrement(&m_count);
	}

	long refDecrease()
	{
		InterlockedDecrement(&m_count);
		if (m_count == 0)
		{
			delete this;
			return 0L;
		}
		else
			return 1L;
	}

	virtual bool	addClient	( int nClientID );
	virtual int		delClient	( int nClientID );

	void			clearClients();
	IntIntMap		getClients	();

	int				getType()			{return m_type;};
	std::string		getUnderlying()		{return m_underlying;};

protected:

	int				m_type;
	long			m_count;
	std::string		m_underlying;

	IntIntMap		m_Clients;
	Lock			m_ClientLock; 
};

class CDCRStockQuote : public CDClientRequest
{
public:
	CDCRStockQuote( const std::string & szUnder );

	virtual ~CDCRStockQuote();

};

class CDCRReregister : public CDClientRequest
{
public:
	CDCRReregister()			{m_type = DATACLIENTREQUEST_REREQUEST;};
	virtual ~CDCRReregister()	{};
};

class CDCRDeleteAll : public CDClientRequest
{
public:
	CDCRDeleteAll()				{m_type = DATACLIENTREQUEST_DELETEALL;};
	virtual ~CDCRDeleteAll()	{};
};


class CDCRL2StockQuote : public CDCRStockQuote
{
public:
	CDCRL2StockQuote( const std::string & szUnder );
	virtual ~CDCRL2StockQuote();
};

class CDCRGreekDelta : public CDClientRequest
{
public:
	CDCRGreekDelta( const std::string & szUnder );
	virtual ~CDCRGreekDelta();
};

class CDCROptionSeries : public CDClientRequest
{
public:
	CDCROptionSeries( const std::string & szUnder );
	virtual ~CDCROptionSeries()	{};

};

class CDCRFutureSeries : public CDClientRequest
{
public:
	CDCRFutureSeries( const std::string & szUnder );
	virtual ~CDCRFutureSeries()	{};

};

class CDCROptionQuote : public CDClientRequest
{
public:
	CDCROptionQuote( const std::string & szUnder, const std::string & szClass, const std::string & szExch );
	virtual ~CDCROptionQuote();
	std::string		getRic() { return m_ric; }
	std::string		getExchange() { return m_exchange; }
private:
	std::string			m_ric;
	std::string			m_exchange;
};

class CDCROptionQuoteBest : public CDClientRequest
{
public:
	CDCROptionQuoteBest( const std::string & szUnder, const std::string & szClass );
	virtual ~CDCROptionQuoteBest();
	std::string		getRic() { return m_ric; }
private:
	std::string			m_ric;
};

class CDCRFutureQuote : public CDClientRequest
{
public:
	CDCRFutureQuote( const std::string & szUnder, const std::string & szSymbol );
	virtual ~CDCRFutureQuote();
	std::string		getRic() { return m_ric; }
private:
	std::string		m_ric;
};

#endif // !defined(AFX_DCLIENTREQUEST_H__D86EE61B_1CF6_4BD5_91DA_A12386566C64__INCLUDED_)
