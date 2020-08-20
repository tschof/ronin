// StatusMsg.h: interface for the CStatusMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSMSG_H__D7F89787_88A7_42F2_A2C0_2D7AAD28FC2F__INCLUDED_)
#define AFX_STATUSMSG_H__D7F89787_88A7_42F2_A2C0_2D7AAD28FC2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QueueRequest.h"
#include "Lock.h"

class CCsv;
class CDBConnection;

class CStatusMsg : public CQueueRequest
{
public:
	friend class CDBConnection;
	CStatusMsg();
	CStatusMsg( CCsv * pCsv );
	virtual ~CStatusMsg();

	virtual bool Process();
	virtual long GetRequestID();
protected:
	CString m_szCommand;		// CSV_COMMAND
	CString m_szTrader;			// CSV_TRADER
    CString	m_szTradeFor;		// CSV_TRADE_FOR
    CString m_szTag;			// CSV_TAG
    int		m_nSide;			// CSV_SIDE               
    CString m_szSymbol;			// CSV_SYMBOL
    int		m_nShares;			// CSV_LEAVESQTY :It is the leaves qty 
    double	m_fPrice;			// CSV_PRICE
    int		m_nType;			// CSV_TYPE 
    int		m_nTIF;				// CSV_TIF 

    CString m_szLocalAcct;		// CSV_LOCALACCOUNT
	CString m_szFirm;			// CSV_FIRM
    CString	m_szClgAcct;		// CSV_CLEARINGACCOUNT
    int		m_nStatus;			// CSV_STATUS
    CString	m_szStaffordTag;	// CSV_STAFFORD_TAG
    int		m_nExchange;		// CSV_EXCHANGE 
    CString	m_szExDest;			// CSV_EXDEST
    double	m_fStopPrice;		// CSV_STOPPRICE 
    int		m_nCumQty;			// CSV_CUMQTY : calculated value
    int		m_nLeaveQty;		// CSV_SHARES 

	static Lock	m_Lock;			// for debug purpose
};

#endif // !defined(AFX_STATUSMSG_H__D7F89787_88A7_42F2_A2C0_2D7AAD28FC2F__INCLUDED_)
