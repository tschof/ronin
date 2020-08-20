#ifndef _ORDERMSG_H__
#define _ORDERMSG_H__

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "QueueRequest.h"
#include "StatusMsg.h"
#include <string>

class CRomCsv;
class CDBConnection;

class COrderMsg : public CStatusMsg  
{
public:
	static bool IsOptionType(char secType);
	static bool IsFutureType(char secType);
	static bool IsCurrencyType(char secType);
	static bool IsEquityType(char secType);
	static bool IsComplexType(char secType);

	friend class CDBConnection;
	COrderMsg();
	COrderMsg( CRomCsv * pCsv );
	virtual ~COrderMsg();

	//bool Process();
	std::string TimeStamp();

	const char*	OrderNum() { return m_szOrderNum.c_str(); }
	const char*	Cacacity() { return m_szCacacity.c_str(); }
    const char*	ClgID() { return m_szClgID.c_str(); }
	const char* Instruction() { return m_szInstruction.c_str(); }
    const char* PriceCheck() { return m_szPriceCheck.c_str(); }
    const char*	MinQty() { return m_szMinQty.c_str(); }
    const char*	Discretion() { return m_szDiscretion.c_str(); }
    const char* Solicited() { return m_szSolicited.c_str(); }
    const char*	DBSymbol() { return m_szDBSymbol.c_str(); }
	double		Multipler() { return m_fMultipler; }
    const char*	ExpDate() { return m_szExpDate.c_str(); }

    const char*	CallPut() { return m_szCallPut.c_str(); }
	double		StrikePrice() { return m_fStrikePrice; }
    const char* Underlying() { return m_szUnderlying.c_str(); }
    const char* OrderTag() { return m_szOrderTag.c_str(); }
    const char* ExchgTag() { return m_szExchgTag.c_str(); }
    const char* CancelTag() { return m_szCancelTag.c_str(); }
    const char* ExecTag() { return m_szExecTag.c_str(); }
	const char* ExecConfirm() { return m_szExecConfirm.c_str(); }
    const char* Reference() { return m_szReference.c_str(); }
    const char* Report() { return m_szReport.c_str(); }

    const char* Contra() { return m_szContra.c_str(); }
    const char* Text() { return m_szText.c_str(); }
    const char* Owner() { return m_szOwner.c_str(); }
    const char* OMExecTag() { return m_szOMExecTag.c_str(); }
	int			ExecShares() { return m_nExecShares; }
	int			OrgShares() { return m_nOrgShares; }
    const char* ExecPrice() { return m_szExecPrice.c_str(); }
    const char* OwnerCancel() { return m_szOwnerCancel.c_str(); }
	const char* ClientTime() { return m_szClientTime.c_str(); }
	const char* CMTA() { return m_szCMTA.c_str(); }

	const char* ExecTime() { return m_szExecTime.c_str(); }
	const char* GiveUp() { return m_szGiveUp.c_str(); }
	const char* MMAccount() { return m_szMMAccount.c_str(); }
	const char* MMExchange() { return m_szMMExchange.c_str(); }
	const char* OMTime() { return m_szOMTime.c_str(); }
	int			OpenClose() { return m_nOpenClose; }
	const char* OrderExpDate() { return m_szOrderExpDate.c_str(); }
	const char*	SecType() { return m_szSecType.c_str(); }
	int			MaxFloor() { return m_nMaxFloor; }
	double		AvgPrice() { return m_dAvgPrice; }

	int			OrderDest() { return m_nOrderDest; }
	const char*	Currency() { return m_szCurrency.c_str(); }
	const char*	ShortLender() { return m_szShortLender.c_str(); }
	int			ExecInst() { return m_nExecInst; }
	const char* EffectiveTime() { return m_szEffectiveTime.c_str(); }
	const char* BillingCode() { return m_szBillingCode.c_str(); }
	const char* ParentOmTag() { return m_szParentOmTag.c_str(); }
	const char* SecID() { return m_szSecID.c_str(); }
	const char* SecDef() { return m_szSecDef.c_str(); }
	const char* ExpireDate() { return m_szExpireDate.c_str(); }

	int			CplxType() { return m_nCplxType; }
	int			AlgoType() { return m_nAlgoType; }

protected:
	std::string	m_szOrderNum;		// db rec_num
    std::string	m_szCacacity;		// CSV_CAPACITY
    std::string	m_szClgID;			// CSV_CLEARINGID
	std::string m_szInstruction;	// CSV_INSTRUCTIONS
    std::string m_szPriceCheck;		// CSV_PRICECHECK 
    std::string	m_szMinQty;			// CSV_MINQTY 
    std::string	m_szDiscretion;		// CSV_DISCRETIONARY 
    std::string m_szSolicited;		// CSV_SOLICITED
    std::string	m_szDBSymbol;		// CSV_DBSYMBOL 
    double		m_fMultipler;		// CSV_MULTIPLIER 
    std::string	m_szExpDate;		// CSV_EXPDATE 

    std::string	m_szCallPut;		// CSV_CALLPUT  
    double		m_fStrikePrice;		// CSV_STRIKEPRICE 
    std::string m_szUnderlying;		// CSV_UNDERLYING 
    std::string m_szOrderTag;		// CSV_ORDER_TAG
    std::string m_szExchgTag;		// CSV_EXCHANGE_TAG
    std::string m_szCancelTag;		// CSV_CANCEL_TAG 
    std::string m_szExecTag;		// CSV_EXEC_TAG
	std::string m_szExecConfirm;	// CSV_EXEC_CONFIRM 
    std::string m_szReference;		// CSV_REFERENCE
    std::string m_szReport;			// CSV_REPORT

    std::string m_szContra;			// CSV_CONTRA 
    std::string m_szText;			// CSV_TEXT
    std::string m_szOwner;			// CSV_OWNER
    std::string m_szOMExecTag;		// CSV_OMEXECUTION_TAG 
    int			m_nExecShares;		// CSV_EXEC_SHARES 
    int			m_nOrgShares;		// CSV_ORIGINAL_SHARES 
    std::string m_szExecPrice;		// CSV_EXEC_PRICE 
    std::string m_szOwnerCancel;	// CSV_OWNERCANCEL  
	std::string m_szClientTime;		// CSV_CLIENT_TIME
	std::string m_szCMTA;			// CSV_CMTAACCOUNT

	std::string m_szExecTime;		// CSV_EXEC_TIME
	std::string m_szGiveUp;			// CSV_GIVEUPACCOUNT
	std::string m_szMMAccount;		// CSV_MM_ACCOUNT
	std::string m_szMMExchange;		// CSV_MM_EXCHANGE
	std::string m_szOMTime;			// CSV_OM_TIME
	int			m_nOpenClose;		// CSV_OPENCLOSE
	std::string m_szOrderExpDate;	// CSV_GTD_DATE
	std::string	m_szSecType;		// CSV_SECURITY_TYPE
	int			m_nMaxFloor;		// CSV_MAX_FLOOR
	double		m_dAvgPrice;		// CSV_AVG_PRICE

	int			m_nOrderDest;		// CSV_ORDER_DEST
	std::string	m_szCurrency;		// CSV_CURRENCY
	std::string	m_szShortLender;	// CSV_SHORT_LENDER
	int			m_nExecInst;		// CSV_EXEC_INST
	std::string m_szEffectiveTime;   // CSV_EFFECTIVE_TIME
	std::string m_szBillingCode;		// CSV_BILLING_CODE
	std::string m_szParentOmTag;		// CSV_PARENT_STAFFORD_TAG
	std::string m_szSecID;			// CSV_SECID
	std::string m_szSecDef;			// CSV_SEC_DEF
	int			m_nCplxType;		// CSV_CPLXORDER_TYPE
	std::string m_szExpireDate;	// CSV_EXPDATE + CSV_MATURITYDAY
	int			m_nAlgoType;		// CSV_ALGOTYPE
};

#endif // _ORDERMSG_H__
