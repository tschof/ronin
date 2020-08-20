//**************************************************************************
// 
//	File:
//	
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "Message.h"
#include "Order.h"
#include "iLinkGlobex.h"
#include "GlobexStatusHandler.h"

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
CGlobexStatusHandler::CGlobexStatusHandler()
{
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
CGlobexStatusHandler::~CGlobexStatusHandler()
{
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CGlobexStatusHandler::SetStatus(CCancel& cancel)
{
    CILinkGlobexApp *pApp = (CILinkGlobexApp *)AfxGetApp();
    
    CCsv status((char)CMessage::Status);
    
    status.SetAt(CSV_TRADER,		cancel.Trader());
    status.SetAt(CSV_SIDE,			cancel.Side());
    status.SetAt(CSV_SYMBOL,		cancel.Symbol());
    status.SetAt(CSV_SHARES,		cancel.Shares());
    status.SetAt(CSV_PRICE,			cancel.Price());
    status.SetAt(CSV_STATUS,		COrder::PendingCancel);
    status.SetAt(CSV_ORDER_TAG,		cancel.OrderTag());
    status.SetAt(CSV_EXCHANGE_TAG,	cancel.ExchangeTag());
    status.SetAt(CSV_STAFFORD_TAG,	cancel.StaffordTag());
    status.SetAt(CSV_CANCEL_TAG,	cancel.CancelTag());
    
    pApp->Audit(status, "SERVER GLOBEX", "FROM CME",
        FALSE, cancel.CancelTag());
    
    if (!(pApp->SendToClient(cancel.lpszOwner(), status.Message())))
    {
        if (!(pApp->SendToClient("OM", status.Message()))) {
            pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
            return FALSE;
        }
    }
    pApp->Audit(status, "SERVER GLOBEX", "TO SERVER OM",
        TRUE, cancel.CancelTag());	
    return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CGlobexStatusHandler::SetStatus(CCancelReject& reject)
{
    CILinkGlobexApp *pApp = (CILinkGlobexApp *)AfxGetApp();
    CString		trader;
    CString		owner;
    int			index;
    
    CCsv status((char)CMessage::Status);
    
    CString id = reject.SourceId();
    
    if ((index = id.Find(',')) > 0) {
        owner = id.Left(index);
        trader = id.Mid(index+1);
    } else {
        trader	= reject.SourceId();
        owner	= reject.SourceId();
    }
    
    status.SetAt(CSV_TRADER,		trader);
    status.SetAt(CSV_SYMBOL,		reject.Symbol());
    status.SetAt(CSV_STATUS,		COrder::CancelRejected);
    status.SetAt(CSV_ORDER_TAG,		reject.OrderTag());
    status.SetAt(CSV_EXCHANGE_TAG,	reject.ExchangeTag());
    status.SetAt(CSV_STAFFORD_TAG,	reject.StaffordTag());
    status.SetAt(CSV_CANCEL_TAG,	reject.CancelTag());
    status.SetAt(CSV_TEXT,			reject.Reason());
    
    pApp->Audit(status, "SERVER GLOBEX", "FROM CME",
        FALSE, reject.CancelTag());
    
    if (!(pApp->SendToClient(owner, status.Message())))
    {
       // if (!(pApp->SendToClient("OM", status.Message()))) {
            pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
            return FALSE;
       // }
    }
    
    pApp->Audit(status, "SERVER GLOBEX", "TO SERVER OM",
        TRUE, reject.CancelTag());
    
    return TRUE;
}

BOOL CGlobexStatusHandler::SetStatus(CReplaceReject& reject)
{
    CILinkGlobexApp *pApp = (CILinkGlobexApp *)AfxGetApp();
    CString		trader;
    CString		owner;
    int			index;
    
    CCsv status((char)CMessage::Status);
    
    CString id = reject.SourceId();
    
    if ((index = id.Find(',')) > 0) {
        owner = id.Left(index);
        trader = id.Mid(index+1);
    } else {
        trader	= reject.SourceId();
        owner	= reject.SourceId();
    }
    
    status.SetAt(CSV_TRADER, trader);
    status.SetAt(CSV_SYMBOL, reject.Symbol());
    status.SetAt(CSV_STATUS, COrder::ReplaceRejected);
    status.SetAt(CSV_ORDER_TAG, reject.CancelTag());
    status.SetAt(CSV_EXCHANGE_TAG, reject.ExchangeTag());
    status.SetAt(CSV_STAFFORD_TAG, reject.StaffordTag());
    status.SetAt(CSV_CANCEL_TAG, reject.CancelTag());
    status.SetAt(CSV_TEXT, reject.Reason());
    
    pApp->Audit(status, "SERVER GLOBEX", "FROM CME",
        FALSE, reject.CancelTag());
    
    if (!(pApp->SendToClient(owner, status.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
        return FALSE;
    }	
    pApp->Audit(status, "SERVER GLOBEX", "TO SERVER OM",
        TRUE, reject.CancelTag());			
    return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CGlobexStatusHandler::SetStatus(CExecution& execution)
{
    int		index;
    
    CString	trader			= execution.SourceId();
    CString	owner 			= execution.SourceId();	
    CString sourceId		= execution.SourceId();
    double price;
    
    CCsv csvExecution((char)CMessage::Status);
    
    
    if (execution.TransactionType() != "0")
    {
        return FALSE;
    }
    
    CILinkGlobexApp *pApp = (CILinkGlobexApp *)AfxGetApp();
    
    if (NULL == pApp)
    {
        return FALSE;
    }
    
    if ((index = sourceId.Find(',')) > 0)
    {
        owner	= sourceId.Left(index);
        trader	= sourceId.Mid(index+1);
    }
    
    // Populate the common fields
    csvExecution.SetAt(CSV_TRADER,			trader);
    csvExecution.SetAt(CSV_SIDE,			execution.Side());
    csvExecution.SetAt(CSV_SYMBOL,			execution.Symbol());
    csvExecution.SetAt(CSV_TIF,				execution.TimeInForce());
    csvExecution.SetAt(CSV_ORDER_TAG,		execution.OrderTag());
    csvExecution.SetAt(CSV_EXCHANGE_TAG,	execution.ExchangeTag());
    csvExecution.SetAt(CSV_STAFFORD_TAG,	execution.StaffordTag());
    csvExecution.SetAt(CSV_CANCEL_TAG,		execution.CancelTag());
    csvExecution.SetAt(CSV_EXEC_TAG,		execution.TradeTag());
    csvExecution.SetAt(CSV_EXEC_CONFIRM,	execution.Confirm());
    csvExecution.SetAt(CSV_REFERENCE,		execution.ReferenceConfirm());
    csvExecution.SetAt(CSV_CONTRA,			execution.ExecBroker());
    csvExecution.SetAt(CSV_TEXT,			execution.Reason() + "(" + execution.Text() + ")");
    csvExecution.SetAt(CSV_REPORT,			"N");
    csvExecution.SetAt(CSV_EXEC_TIME,		execution.TransactionTime());
    
    switch (execution.Status())
    {	
    case COrder::PendingCancel:
    case COrder::Canceled:
    case COrder::Rejected:
        
        if (execution.LastShares())	
        {
            csvExecution.SetAt(CSV_SHARES,			execution.LastShares());
            csvExecution.SetAt(CSV_PRICE,			execution.LastPrice());
            csvExecution.SetAt(CSV_STATUS,			COrder::PartiallyFilled);
            csvExecution.SetAt(CSV_CLEARINGID,		execution.Account());
            csvExecution.SetAt(CSV_CLEARINGACCOUNT,	"");
            
            pApp->Audit(csvExecution, "SERVER GLOBEX", "FROM CME",
                FALSE, execution.OrderTag());
            
            if (!(pApp->SendToClient(owner, csvExecution.Message())))
            {	
                pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
                return FALSE;
            }
            
            pApp->Audit(csvExecution, "SERVER GLOBEX", "TO SERVER OM",
                TRUE, execution.OrderTag());
        }
        
        break;
    }
    
    csvExecution.SetAt(CSV_STATUS,	execution.Status());
    
    if ((execution.Status() == COrder::PartiallyFilled) || (execution.Status() == COrder::Filled))
    {		
        price = execution.LastPrice();
        
        csvExecution.SetAt(CSV_SHARES,	execution.LastShares());
        csvExecution.SetAt(CSV_PRICE,	price);
    }
    else
    {
        price = execution.Price();
        
        csvExecution.SetAt(CSV_SHARES,	execution.Shares());
        csvExecution.SetAt(CSV_PRICE,	price);
    }
    
    pApp->Audit(csvExecution, "SERVER GLOBEX", "FROM CME",
        FALSE, execution.OrderTag());
    
    if (!(pApp->SendToClient(owner, csvExecution.Message())))
    {
       // if(!(pApp->SendToClient("OM", csvExecution.Message()))) {
            pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
            return FALSE;
      //  }
    }
    
    pApp->Audit(csvExecution, "SERVER GLOBEX", "TO SERVER OM",
        TRUE, execution.OrderTag());
    
    return TRUE;
}



BOOL CGlobexStatusHandler::SetStatus(CReplace& Replace)
{
    CILinkGlobexApp *pApp = (CILinkGlobexApp *)AfxGetApp();
    CCsv Status((char)CMessage::Status);
    
    Status.SetAt(CSV_TRADER, Replace.Trader());
    Status.SetAt(CSV_SIDE, Replace.Side());
    Status.SetAt(CSV_SYMBOL, Replace.Symbol());
    Status.SetAt(CSV_SHARES, Replace.Shares());
    Status.SetAt(CSV_PRICE, Replace.Price());
    Status.SetAt(CSV_STATUS, COrder::ReplacePending);
    Status.SetAt(CSV_ORDER_TAG, Replace.OrderTag());
    Status.SetAt(CSV_EXCHANGE_TAG, Replace.ExchangeTag());
    Status.SetAt(CSV_STAFFORD_TAG, Replace.StaffordTag());
    Status.SetAt(CSV_CANCEL_TAG, Replace.CancelTag());
    Status.SetAt(CSV_TIF, Replace.TIF());
    Status.SetAt(CSV_EXEC_INSTRUCTION, Replace.Instructions());
    
    if(Replace.StopPrice() > 0.0 ) {
        Status.SetAt(CSV_STOPPRICE, Replace.StopPrice());
    }
    pApp->Audit(Status, "SERVER GLOBEX", "FROM CME",
        FALSE, Replace.CancelTag());
    if (!(pApp->Clients().Send(Replace.lpszOwner(),
        Status.Message()))) {
        if(!(pApp->SendToClient("OM", Status.Message()))) {
            pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
            return FALSE;
        }
    }
    pApp->Audit(Status, "SERVER GLOBEX", "TO SERVER OM",
        TRUE, Replace.CancelTag());
    return(TRUE);
}


//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CGlobexStatusHandler::SetStatus(COrder& order)
{
    CILinkGlobexApp *pApp = (CILinkGlobexApp *)AfxGetApp();
    
    order.SetStatus(COrder::PendingNew);
    
    CCsv status((char)CMessage::Status);
    
    status.SetAt(CSV_TRADER,			order.Trader());
    status.SetAt(CSV_TAG,				order.Tag());
    status.SetAt(CSV_SIDE,				order.Side());
    status.SetAt(CSV_SYMBOL,			order.Symbol());
    status.SetAt(CSV_SHARES,			order.Shares());
    
    double price = order.dPrice();
    
    if (order.SecurityType() == COrder::Future)
    {
        price = price / 100.00;
    }
    
    status.SetAt(CSV_PRICE,				price);
    
    status.SetAt(CSV_TYPE,				order.Type());
    status.SetAt(CSV_TIF,				order.TIF());
    status.SetAt(CSV_CAPACITY,			order.Capacity());
    status.SetAt(CSV_CLEARINGID,		order.ClearingId());
    status.SetAt(CSV_CLEARINGACCOUNT,	order.ClearingAccount());
    status.SetAt(CSV_EXCHANGE,			order.Exchange());
    status.SetAt(CSV_STATUS,			order.Status());
    status.SetAt(CSV_ORDER_TAG,			order.OrderTag());
    status.SetAt(CSV_EXCHANGE_TAG,		order.ExchangeTag());
    status.SetAt(CSV_STAFFORD_TAG,		order.StaffordTag());
    status.SetAt(CSV_CANCEL_TAG,		order.CancelTag());
    
    pApp->Audit(status, "SERVER GLOBEX", "FROM CME",
        TRUE, order.OrderTag());
    
    if (!(pApp->SendToClient(order.lpszOwner(), status.Message())))
    {
        if(!(pApp->SendToClient("OM", status.Message()))) {
            pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
            return FALSE;
        }
    }
    pApp->Audit(status, "SERVER GLOBEX", "TO SERVER OM",
        TRUE, order.OrderTag());		
    return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CGlobexStatusHandler::SetStatus(CReject& reject)
{
    CILinkGlobexApp *pApp = (CILinkGlobexApp *)AfxGetApp();
    
    CString		trader;
    CString		owner;
    int			index;
    
    CCsv status((char)CMessage::Status);
    
    CString sourceId = reject.SourceId();
    
    if ((index = sourceId.Find(',')) > 0)
    {
        owner	= sourceId.Left(index);
        trader	= sourceId.Mid(index+1);
    }
    else
    {
        trader	= reject.SourceId();
        owner	= reject.SourceId();
    }
    
    status.SetAt(CSV_TRADER,		trader);
    status.SetAt(CSV_SYMBOL,		reject.Symbol());
    status.SetAt(CSV_STATUS,		COrder::Rejected);
    status.SetAt(CSV_ORDER_TAG,		reject.OrderTag());
    status.SetAt(CSV_EXCHANGE_TAG,	reject.ExchangeTag());
    status.SetAt(CSV_STAFFORD_TAG,	reject.StaffordTag());
    status.SetAt(CSV_CANCEL_TAG,	reject.CancelTag());
    status.SetAt(CSV_TEXT,			reject.Reason() + "(" + reject.Text() + ")");
    
    pApp->Audit(status, "SERVER GLOBEX", "FROM CME",
        FALSE, reject.OrderTag());
    
    if (!(pApp->SendToClient(owner, status.Message())))
    {
        if(!(pApp->SendToClient("OM", status.Message()))) {
            pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
            return FALSE;
        }
    }
    
    pApp->Audit(status, "SERVER GLOBEX", "TO SERVER OM",
        TRUE, reject.OrderTag());
    
    return TRUE;
}