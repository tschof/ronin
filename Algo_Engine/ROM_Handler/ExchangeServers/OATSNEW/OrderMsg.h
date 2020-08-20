// OrderMsg.h: interface for the COrderMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDERMSG_H__74CE4A5D_D960_4EA5_9936_5EE312A061F9__INCLUDED_)
#define AFX_ORDERMSG_H__74CE4A5D_D960_4EA5_9936_5EE312A061F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QueueRequest.h"
#include "StatusMsg.h"
#include <string>

class CCsv;

class CDBConnection;
class COrderMsg : public CStatusMsg  
{
public:
	friend class CDBConnection;
	COrderMsg();
	COrderMsg( CCsv * pCsv );
	virtual ~COrderMsg();

	bool Process();
	std::string TimeStamp();

protected:
	CString	m_szOrderNum;		// db rec_num
    CString	m_szCacacity;		// CSV_CAPACITY
    CString	m_szClgID;			// CSV_CLEARINGID
	CString m_szInstruction;	// CSV_INSTRUCTIONS
    CString m_szPriceCheck;		// CSV_PRICECHECK 
    CString	m_szMinQty;			// CSV_MINQTY 
    CString	m_szDiscretion;		// CSV_DISCRETIONARY 
    CString m_szSolicited;		// CSV_SOLICITED
    CString	m_szDBSymbol;		// CSV_DBSYMBOL 
    double	m_fMultipler;		// CSV_MULTIPLIER 
    CString	m_szExpDate;		// CSV_EXPDATE 

    CString	m_szCallPut;		// CSV_CALLPUT  
    double	m_fStrikePrice;		// CSV_STRIKEPRICE 
    CString m_szUnderlying;		// CSV_UNDERLYING 
    CString m_szOrderTag;		// CSV_ORDER_TAG
    CString m_szExchgTag;		// CSV_EXCHANGE_TAG
    CString m_szCancelTag;		// CSV_CANCEL_TAG 
    CString m_szExecTag;		// CSV_EXEC_TAG
	CString m_szExecConfirm;	// CSV_EXEC_CONFIRM 
    CString m_szReference;		// CSV_REFERENCE
    CString m_szReport;			// CSV_REPORT

    CString m_szContra;			// CSV_CONTRA 
    CString m_szText;			// CSV_TEXT
    CString m_szOwner;			// CSV_OWNER
    CString m_szOMExecTag;		// CSV_OMEXECUTION_TAG 
    int		m_nExecShares;		// CSV_EXEC_SHARES 
    int		m_nOrgShares;		// CSV_ORIGINAL_SHARES 
    CString m_szExecPrice;		// CSV_EXEC_PRICE 
    CString m_szOwnerCancel;	// CSV_OWNERCANCEL  
	CString m_szClientTime;		// CSV_CLIENT_TIME
	CString m_szCMTA;			// CSV_CMTAACCOUNT

	CString m_szExecTime;		// CSV_EXEC_TIME
	CString m_szGiveUp;			// CSV_GIVEUPACCOUNT
	CString m_szMMAccount;		// CSV_MM_ACCOUNT
	CString m_szMMExchange;		// CSV_MM_EXCHANGE
	CString m_szOMTime;			// CSV_OM_TIME
	int		m_nOpenClose;		// CSV_OPENCLOSE
	CString m_szOrderExpDate;	// CSV_GTD_DATE
	CString m_szSecType;		// CSV_SECURITY_TYPE
	int		m_nMaxFloor;		// CSV_MAX_FLOOR
	double	m_dAvgPrice;		// CSV_AVG_PRICE

	int		m_nOrderDest;		// CSV_ORDER_DEST
	CString	m_szCurrency;		// CSV_CURRENCY
	CString	m_szShortLender;	// CSV_SHORT_LENDER
	int		m_nExecInst;		// CSV_EXEC_INST
	CString m_szEffectiveTime;   // CSV_EFFECTIVE_TIME
	CString m_szBillingCode;		// CSV_BILLING_CODE
	CString m_szParentOmTag;		// CSV_PARENT_STAFFORD_TAG
	CString m_szSecID;			// CSV_SECID
	int		m_nCplxType;		// CSV_CPLXORDER_TYPE
};

#endif // !defined(AFX_ORDERMSG_H__74CE4A5D_D960_4EA5_9936_5EE312A061F9__INCLUDED_)
