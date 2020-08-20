/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *  : Defines the class behaviors for the application.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *     Parnell     
 *
 *******************************************************************************/
#include "stdafx.h"
#include "ArcaDirect.h"
#include "ArcaDirectStatus.h"


ArcaDirectStatus::ArcaDirectStatus() :
    CStatusHandler()
{
}

ArcaDirectStatus::~ArcaDirectStatus()
{
}

BOOL ArcaDirectStatus::SetStatus(COrder&	Order)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();

    Order.SetStatus(COrder::PendingNew);

    CCsv Status((char)CMessage::Status);

    Status.SetAt(CSV_TRADER, Order.Trader());
    Status.SetAt(CSV_TAG, Order.Tag());
    Status.SetAt(CSV_SIDE, Order.Side());
    Status.SetAt(CSV_SYMBOL, Order.Symbol());
    Status.SetAt(CSV_SHARES, Order.Shares());
    Status.SetAt(CSV_PRICE, Order.dPrice());
    Status.SetAt(CSV_TYPE, Order.Type());
    Status.SetAt(CSV_TIF, Order.TIF());
    Status.SetAt(CSV_CAPACITY, Order.Capacity());
    Status.SetAt(CSV_CLEARINGID, Order.ClearingId());
    Status.SetAt(CSV_CLEARINGACCOUNT, Order.ClearingAccount());
    Status.SetAt(CSV_EXCHANGE, Order.Exchange());
    Status.SetAt(CSV_STATUS, Order.Status());
    Status.SetAt(CSV_ORDER_TAG,	Order.OrderTag());
    Status.SetAt(CSV_EXCHANGE_TAG, Order.ExchangeTag());
    Status.SetAt(CSV_STAFFORD_TAG, Order.StaffordTag());
    Status.SetAt(CSV_CANCEL_TAG, Order.CancelTag());

    if (!(pApp->SendToClient(Order.lpszOwner(), Status.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler",
                             "Error Sending Message To Client:" + Status.Message());
        return(FALSE);
    }
			
    return(TRUE);
}

BOOL ArcaDirectStatus::SetStatus(CCancel& Cancel)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();

    CCsv Status((char)CMessage::Status);

    Status.SetAt(CSV_TRADER, Cancel.Trader());
    Status.SetAt(CSV_SIDE, Cancel.Side());
    Status.SetAt(CSV_SYMBOL, Cancel.Symbol());
    Status.SetAt(CSV_SHARES, Cancel.Shares());
    Status.SetAt(CSV_PRICE, Cancel.Price());
    Status.SetAt(CSV_STATUS, COrder::PendingCancel);
    Status.SetAt(CSV_ORDER_TAG,	Cancel.OrderTag());
    Status.SetAt(CSV_EXCHANGE_TAG, Cancel.ExchangeTag());
    Status.SetAt(CSV_STAFFORD_TAG, Cancel.StaffordTag());
    Status.SetAt(CSV_CANCEL_TAG, Cancel.CancelTag());

    if (!(pApp->SendToClient(Cancel.lpszOwner(), Status.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler",
                             "Error Sending Message To Client:" + Status.Message());
        return(FALSE);
    }
			
    return(TRUE);
}

BOOL ArcaDirectStatus::SetStatus(CCancelReject& Reject)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CString Trader;
    CString Owner;

    CCsv Status((char)CMessage::Status);

    Owner = Reject.Owner();
    if(Owner.GetLength() <= 0) {
        Owner += "OM";
    }

    Status.SetAt(CSV_TRADER, Reject.SourceId());
    Status.SetAt(CSV_SYMBOL, Reject.Symbol());
    Status.SetAt(CSV_STATUS, COrder::CancelRejected);
    Status.SetAt(CSV_ORDER_TAG,	Reject.OrderTag());
    Status.SetAt(CSV_EXCHANGE_TAG, Reject.ExchangeTag());
    Status.SetAt(CSV_STAFFORD_TAG, Reject.StaffordTag());
    Status.SetAt(CSV_CANCEL_TAG, Reject.CancelTag());
    Status.SetAt(CSV_TEXT, Reject.Reason());

    if (!(pApp->SendToClient(Owner, Status.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler",
                             "Error Sending Message To Client");
        return(FALSE);
    }
			
    return(TRUE);
}

BOOL ArcaDirectStatus::SetStatus(CReplaceReject& Reject)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CString Trader;
    CString Owner;

    CCsv Status((char)CMessage::Status);

    Owner = Reject.Owner();
    if(Owner.GetLength() <= 0) {
        Owner += "OM";
    }

    Status.SetAt(CSV_TRADER, Reject.SourceId());
    Status.SetAt(CSV_SYMBOL, Reject.Symbol());
    Status.SetAt(CSV_STATUS, COrder::ReplaceRejected);
    Status.SetAt(CSV_ORDER_TAG, Reject.OrderTag());
    Status.SetAt(CSV_EXCHANGE_TAG, Reject.ExchangeTag());
    Status.SetAt(CSV_STAFFORD_TAG, Reject.StaffordTag());
    Status.SetAt(CSV_CANCEL_TAG, Reject.CancelTag());
    Status.SetAt(CSV_TEXT, Reject.Reason());

    if (!(pApp->SendToClient(Owner, Status.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler",
                             "Error Sending Message To Client");
        return(FALSE);
    }
			
    return(TRUE);
}



BOOL ArcaDirectStatus::SetStatus(CReject& Reject)
{
    BOOL Result = FALSE;

    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CString Trader;
    CString Owner;

    CCsv Status((char)CMessage::Status);

    
    Owner = Reject.Owner();
    if(Owner.GetLength() <= 0) {
        Owner += "OM";
    }

    Status.SetAt(CSV_TRADER, Reject.SourceId());
    Status.SetAt(CSV_SYMBOL, Reject.Symbol());
    Status.SetAt(CSV_STATUS, COrder::Rejected);
    Status.SetAt(CSV_ORDER_TAG, Reject.OrderTag());
    Status.SetAt(CSV_EXCHANGE_TAG, Reject.ExchangeTag());
    Status.SetAt(CSV_STAFFORD_TAG, Reject.StaffordTag());
    Status.SetAt(CSV_CANCEL_TAG, Reject.CancelTag());
    Status.SetAt(CSV_TEXT, Reject.Reason());

    if (!(pApp->SendToClient(Owner, Status.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler",
                             "Error Sending Message To Client");
        return(FALSE);
    }
	
    return(Result);
}


BOOL ArcaDirectStatus::SetStatus(CExecution& Exec)
{
    int Status = Exec.Status();
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CCsv Message((char)CMessage::Status);
    //Exec.SetOwner("OM");
    
    if ((Status == COrder::PartiallyFilled) ||
        (Status == COrder::Filled)) {
        Message.SetAt(CSV_PRICE, Exec.Price());
        Message.SetAt(CSV_LAST_PRICE, Exec.LastPrice());
    } else {
        Message.SetAt(CSV_PRICE, Exec.Price());
    }

    CString Owner;
    Owner = Exec.Owner();//Reject.Owner();
    if(Owner.GetLength() <= 0) {
        Owner += "OM";
    }
    
    Message.SetAt(CSV_SHARES, Exec.Shares());
    Message.SetAt(CSV_CUM_SHARES, Exec.Cumulative());
    Message.SetAt(CSV_LAST_SHARES, Exec.LastShares());
    Message.SetAt(CSV_LEAVE_SHARES, Exec.Leaves());
    
    Message.SetAt(CSV_TRADER, Exec.SourceId());//Trader);
    Message.SetAt(CSV_SIDE, Exec.Side());
    Message.SetAt(CSV_SYMBOL, Exec.Symbol());
    Message.SetAt(CSV_TIF, Exec.TimeInForce());
    Message.SetAt(CSV_STATUS, Exec.Status());
    Message.SetAt(CSV_ORDER_TAG, Exec.OrderTag());
    Message.SetAt(CSV_EXCHANGE_TAG, Exec.ExchangeTag());
    Message.SetAt(CSV_STAFFORD_TAG, Exec.StaffordTag());
    Message.SetAt(CSV_CANCEL_TAG, Exec.CancelTag());
    Message.SetAt(CSV_EXEC_TAG, Exec.TradeTag());
    Message.SetAt(CSV_EXEC_CONFIRM, Exec.Confirm());
    Message.SetAt(CSV_REFERENCE, Exec.ReferenceConfirm());
    Message.SetAt(CSV_REPORT, Exec.Report());
    Message.SetAt(CSV_CONTRA, Exec.ExecBroker());
    Message.SetAt(CSV_EXEC_TIME, Exec.TransactionTime());
    Message.SetAt(CSV_BILLING_CODE, Exec.BillingCode());
    //Reason = Exec.Reason() + Exec.Text();
    Message.SetAt(CSV_TEXT, Exec.Reason() + Exec.Text());//Reason);
    
    if (!(pApp->SendToClient(Owner, Message.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Message Handler",
            "Error Sending Message To Client");
        return(FALSE);
    }
    return(FALSE);
}

BOOL ArcaDirectStatus::SetStatus(CReplace& Replace)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
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
    if (!(pApp->Clients().Send(Replace.lpszOwner(),
                               Status.Message()))) {
        pApp->ProcessErrorEx(MODE_ERR_NOTIFY,
                             "Status Handler", "Error Sending Message To Client");
        return(FALSE);
    }
    
    return(TRUE);
}


BOOL ArcaDirectStatus::ProcessTrade(CExecution& Exec)
{
    return(TRUE);
}
