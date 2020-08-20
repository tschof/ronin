#ifndef _STATUSMSG_H__
#define _STATUSMSG_H__

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "Windows.h"
#include "QueueRequest.h"
#include "Lock.h"
#include <string>

class CRomCsv;
class CDBConnection;

class CStatusMsg : public CQueueRequest
{
public:
	friend class CDBConnection;
	CStatusMsg();
	CStatusMsg( CRomCsv * pCsv );
	virtual ~CStatusMsg();

	virtual bool Process();
	virtual long GetRequestID();

	const char*	Command()	{ return 	m_szCommand.c_str(); }
	const char* Trader()	{ return m_szTrader.c_str();}
	const char*	TradeFor()	{ return m_szTradeFor.c_str(); }
	const char* Tag()		{ return m_szTag.c_str(); }
	int			Side()		{ return m_nSide; }              
	const char* Symbol()	{ return m_szSymbol.c_str(); }
	int			Shares()	{ return m_nShares;	}
	double		Price()		{ return m_fPrice;	}
	double		PegOffset()	{ return m_fPegOffset;	}
	int			Type()		{ return m_nType;	}
	int			TIF()		{ return m_nTIF; }

	const char* LocalAcct() { return m_szLocalAcct.c_str(); }
	const char* Firm()		{ return m_szFirm.c_str(); }
	const char*	ClgAcct()	{ return m_szClgAcct.c_str(); }
	int			Status()	{ return m_nStatus; }
	//const char*	pStaffordTag() { return m_szStaffordTag.c_str(); }
	const std::string&	StaffordTag() { return m_szStaffordTag; }

	int			Exchange()		{ return m_nExchange; }
	const char*	ExDest()		{ return m_szExDest.c_str(); }
	double		StopPrice()		{ return m_fStopPrice;	}
	int			CumQty()		{ return m_nCumQty; }
	int			LeaveQty()		{ return m_nLeaveQty; }

protected:
	std::string	m_szCommand;		// CSV_COMMAND
	std::string m_szTrader;			// CSV_TRADER
    std::string	m_szTradeFor;		// CSV_TRADE_FOR
    std::string m_szTag;			// CSV_TAG
    int			m_nSide;			// CSV_SIDE               
    std::string m_szSymbol;			// CSV_SYMBOL
    int			m_nShares;			// CSV_LEAVESQTY :It is the leaves qty 
    double		m_fPrice;			// CSV_PRICE
	double		m_fPegOffset;		// CSV_PEGPRICE_OFFSET
    int			m_nType;			// CSV_TYPE 
    int			m_nTIF;				// CSV_TIF 

    std::string m_szLocalAcct;		// CSV_LOCALACCOUNT
	std::string m_szFirm;			// CSV_FIRM
    std::string	m_szClgAcct;		// CSV_CLEARINGACCOUNT
    int			m_nStatus;			// CSV_STATUS
    std::string	m_szStaffordTag;	// CSV_STAFFORD_TAG
    int			m_nExchange;		// CSV_EXCHANGE 
    std::string	m_szExDest;			// CSV_EXDEST
    double		m_fStopPrice;		// CSV_STOPPRICE 
    int			m_nCumQty;			// CSV_CUMQTY : calculated value
    int			m_nLeaveQty;		// CSV_SHARES 

	static Lock	m_Lock;			// for debug purpose
};

#endif // _STATUSMSG_H__
