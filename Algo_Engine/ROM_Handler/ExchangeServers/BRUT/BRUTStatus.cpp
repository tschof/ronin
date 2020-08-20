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
 * 11-11-2005   Parnell     Added the overloaded SetStatus(CReplace).
 *******************************************************************************/


// BRUTStatus.cpp: implementation of the CBRUTStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BRUT.h"
#include "BRUTStatus.h"
#include "Message.h"
#include "Order.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBRUTStatus::~CBRUTStatus()
{
	delete m_pExecMap;
}

CBRUTStatus::CBRUTStatus() :
	CStatusHandler()
{
	CString Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	try { m_pExecMap = new CExecutionMapSet(&pApp->Database()); }

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Opening Status Tables (%s)", e->m_strError);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Status", Error);
		e->Delete();
	}
}

BOOL CBRUTStatus::SetStatus(COrder&	Order)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	Order.SetStatus(COrder::PendingNew);

	CCsv	Status((char)CMessage::Status);

	Status.SetAt(CSV_TRADER,			Order.Trader());
	Status.SetAt(CSV_TAG,				Order.Tag());
	Status.SetAt(CSV_SIDE,				Order.Side());
	Status.SetAt(CSV_SYMBOL,			Order.Symbol());
	Status.SetAt(CSV_SHARES,			Order.Shares());
	Status.SetAt(CSV_PRICE,				Order.dPrice());
	Status.SetAt(CSV_TYPE,				Order.Type());
	Status.SetAt(CSV_TIF,				Order.TIF());
	Status.SetAt(CSV_CAPACITY,			Order.Capacity());
	Status.SetAt(CSV_CLEARINGID,		Order.ClearingId());
	Status.SetAt(CSV_CLEARINGACCOUNT,Order.ClearingAccount());
	Status.SetAt(CSV_EXCHANGE,			Order.Exchange());
	Status.SetAt(CSV_STATUS,			Order.Status());
	Status.SetAt(CSV_ORDER_TAG,		Order.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Order.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Order.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,		Order.CancelTag());

	if (!(pApp->Clients().Send(Order.lpszOwner(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + Status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBRUTStatus::SetStatus(CCancel& Cancel)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv Status((char)CMessage::Status);

	Status.SetAt(CSV_TRADER,		Cancel.Trader());
	Status.SetAt(CSV_SIDE,			Cancel.Side());
	Status.SetAt(CSV_SYMBOL,		Cancel.Symbol());
	Status.SetAt(CSV_SHARES,		Cancel.Shares());
	Status.SetAt(CSV_PRICE,			Cancel.Price());
	Status.SetAt(CSV_STATUS,		COrder::PendingCancel);
	Status.SetAt(CSV_ORDER_TAG,		Cancel.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Cancel.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Cancel.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Cancel.CancelTag());

	if (!(pApp->Clients().Send(Cancel.lpszOwner(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + Status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBRUTStatus::SetStatus(CCancelReject& Reject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	//int			Index;

	CCsv Status((char)CMessage::Status);

	CString Id = Reject.SourceId();
	// Brut doesn't allow comma in sender field
	Trader = Reject.SourceId();
	Owner = "OM";

	Status.SetAt(CSV_TRADER,		Trader);
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_STATUS,		COrder::CancelRejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	Status.SetAt(CSV_TEXT,			Reject.Reason());

	if (!(pApp->Clients().Send(Owner, Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBRUTStatus::SetStatus(CReject& Reject)
{
	BOOL Result = FALSE;

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;

	CCsv Status((char)CMessage::Status);

	CString Id = Reject.SourceId();
	Trader = Reject.SourceId();
	Owner = "OM";

	Status.SetAt(CSV_TRADER,		Trader);
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_STATUS,		COrder::Rejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	Status.SetAt(CSV_TEXT,			Reject.Reason() + "(" + Reject.Text() + ")");

	if (!(pApp->Clients().Send(Owner, Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(Result);
}
BOOL CBRUTStatus::SetStatus(CReplace& Replace)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CCsv Status((char)CMessage::Status);
    
    Status.SetAt(CSV_TRADER,			Replace.Trader());
    Status.SetAt(CSV_SIDE,				Replace.Side());
    Status.SetAt(CSV_SYMBOL,			Replace.Symbol());
    Status.SetAt(CSV_SHARES,			Replace.Shares());
    Status.SetAt(CSV_PRICE,				Replace.Price());
    Status.SetAt(CSV_STATUS,			COrder::ReplacePending);
    Status.SetAt(CSV_ORDER_TAG,			Replace.OrderTag());
    Status.SetAt(CSV_EXCHANGE_TAG,		Replace.ExchangeTag());
    Status.SetAt(CSV_STAFFORD_TAG,		Replace.StaffordTag());
    Status.SetAt(CSV_CANCEL_TAG,		Replace.CancelTag());
    Status.SetAt(CSV_TIF,				Replace.TIF());
    Status.SetAt(CSV_EXEC_INSTRUCTION,	Replace.Instructions());
    
    if(Replace.StopPrice() > 0.0 )
        Status.SetAt(CSV_STOPPRICE,		Replace.StopPrice());
    
    if (!(pApp->Clients().Send(Replace.lpszOwner(), Status.Message())))
    {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
        return(FALSE);
    }
    
    return(TRUE);
}

BOOL CBRUTStatus::SetStatus(CExecution& Exec)
{
	CString	TransactionType	= Exec.TransactionType();
	int		Status		= Exec.Status();
	double	LastPrice	= Exec.LastPrice();
	double	LastShares	= Exec.LastShares();

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	
	CCsv	Message((char)CMessage::Status);
	CCsv	Execution((char)CMessage::Status);

	CString Id = Exec.SourceId();
	Trader = Exec.SourceId();
	Owner = "OM";
	
	if (TransactionType == "0")		// new
	{
		// check for executions embedded in a pending, ur out, or reject message
		switch (Status)
		{	
			case COrder::PendingCancel:	// pending cancel or replace
			case COrder::Canceled:		// ur out
			case COrder::Rejected:		// order reject
				break;
			default:
				break;		
		};

		
		if ((Status == COrder::PartiallyFilled) || (Status == COrder::Filled))
		{
			Message.SetAt(CSV_SHARES,	Exec.LastShares());
			Message.SetAt(CSV_PRICE,	Exec.LastPrice());
		}
		else
		{
			Message.SetAt(CSV_SHARES,	Exec.Shares());
			Message.SetAt(CSV_PRICE,	Exec.Price());
		}

		Message.SetAt(CSV_TRADER,		Trader);
		Message.SetAt(CSV_SIDE,			Exec.Side());
		Message.SetAt(CSV_SYMBOL,		Exec.Symbol());
		Message.SetAt(CSV_TIF,			Exec.TimeInForce());
		Message.SetAt(CSV_STATUS,		Exec.Status());
		Message.SetAt(CSV_ORDER_TAG,	Exec.OrderTag());
		Message.SetAt(CSV_EXCHANGE_TAG,	Exec.ExchangeTag());
		Message.SetAt(CSV_STAFFORD_TAG,	Exec.StaffordTag());
		Message.SetAt(CSV_CANCEL_TAG,	Exec.CancelTag());
		Message.SetAt(CSV_EXEC_TAG,		Exec.TradeTag());
		Message.SetAt(CSV_EXEC_CONFIRM,	Exec.Confirm());
		Message.SetAt(CSV_REFERENCE,	Exec.ReferenceConfirm());
		Message.SetAt(CSV_REPORT,		"N");	//Exec.Report());
		Message.SetAt(CSV_CONTRA,		Exec.ExecBroker());
		Message.SetAt(CSV_TEXT,			Exec.Reason() + "(" + Exec.Text() + ")");
		Message.SetAt(CSV_EXEC_TIME,		Exec.TransactionTime());
        Message.SetAt(CSV_BILLING_CODE,  Exec.BillingCode());

		pApp->SendToClient(Owner, Message.Message());
	}

	return(FALSE);
}

BOOL CBRUTStatus::ProcessTrade(CExecution& Exec)
{
	long		TradeId = -1;
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	try
	{	
		if (!(m_pExecMap->Lookup(Exec.Confirm(), Exec.ExchangeTag())))
		{
			m_pExecMap->AddMap(Exec);
			return(TRUE);
		}
		else
		{
			pApp->ProcessErrorEx(MODE_ERR_BASIC,"BRUT Status",
                "Duplicate Execution Received (" + Exec.Confirm() + "): " + Exec.Debug());
			return(FALSE);
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Trade TradeId(%d) OrderId(%s): (%s)",
            TradeId, Exec.StaffordTag(), e->m_strError, Exec.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Status", Error);
		e->Delete();
		return(TRUE);	// return true even if db error so that trade gets processed
	}

	return(FALSE);
}

