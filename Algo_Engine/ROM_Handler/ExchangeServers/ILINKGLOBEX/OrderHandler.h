/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*   
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*
*******************************************************************************/
#if !defined(_ILINKGLOBEXFIXORDERHANDLER_H_)
#define _ILINKGLOBEXFIXORDERHANDLER_H_

#include <map>
#include "Cancel.h"
#include "Order.h"
#include "Execution.h"
#include "Replace.h"
#include "Request.h"
#include "CancelReject.h"
#include "ReplaceReject.h"
#include "Reject.h"
#include "OrderHandler.h"
#include "OrderTypeValidator.h"
#include "GlobexSequenceGenerator.h"

#include <atlbase.h>

class CGlobexMemTagSet;

class CILinkGlobexOrderHandler : public COrderHandler
{

public:

	void ProcessMessage	(COrder&		order);
	void ProcessMessage	(CReplace&		replace);
	void ProcessMessage	(CCancel&		cancel);	
	void ProcessMessage	(CExecution&	report);
	void ProcessMessage	(CCancelReject& cancelReject);
	void ProcessMessage	(CReplaceReject& Reject);
	void ProcessMessage	(CReject&		rejeect);
	void ProcessMessage	(const char *	status);

    void LoadTradeFor(CString& tradeFor, const char* staffordTag);
		
private:
	CGlobexMemTagSet*	m_pTags;
	CString				m_DefaultAccount;
	//CRegKey				m_OrderIdRegKey;
    CGlobexOrderIdGenerator m_orderIdGenerator;

	COrderTypeValidator*	m_orderTypeValidator;
        CString m_AllowedIDs;
    std::map<CString, int> m_AllowedFirmIDs;

	// methods
	//long SetLastKnownOrderId(DWORD lastKnownOrderId);
	//long GetLastKnownOrderId(DWORD& lastKnownOrderId);

	//void CreateCancelTag(CString& orderTag, int sequenceNumber); 
	void GetOrigOrderId(CString& origOrderId, const CString& orderTag);

public:
	CILinkGlobexOrderHandler();
	virtual ~CILinkGlobexOrderHandler();
};

#endif