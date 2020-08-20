// TerraStatus.cpp: implementation of the CArcaStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TerraNova.h"
#include "TerraStatus.h"
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

CArcaStatus::~CArcaStatus()
{
}

CArcaStatus::CArcaStatus() : CStatusHandler()
{
	CString Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
}

BOOL CArcaStatus::SetStatus(CCancel& cancel)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv status((char)CMessage::Status);

	status.SetAt(CSV_SIDE,			cancel.Side());
	status.SetAt(CSV_SYMBOL,		cancel.Symbol());
	status.SetAt(CSV_SHARES,		cancel.Shares());
	status.SetAt(CSV_PRICE,			cancel.Price());
	status.SetAt(CSV_STATUS,		COrder::PendingCancel);
	status.SetAt(CSV_ORDER_TAG,		cancel.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	cancel.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	cancel.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	cancel.CancelTag());

	if (!(pApp->SendToClient(cancel.SourceId(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CArcaStatus::SetStatus(CCancelReject& cancelReject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv status((char)CMessage::Status);

	status.SetAt(CSV_SYMBOL,		cancelReject.Symbol());
	status.SetAt(CSV_STATUS,		COrder::CancelRejected);
	status.SetAt(CSV_ORDER_TAG,		cancelReject.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	cancelReject.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	cancelReject.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	cancelReject.CancelTag());
	status.SetAt(CSV_TEXT,			cancelReject.Reason());

	if (!(pApp->SendToClient(cancelReject.SourceId(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CArcaStatus::SetStatus(CExecution& execution)
{
	int		ExecType	= execution.ExecType();
	CString TransType	= execution.TransactionType();
	double	LastPrice	= execution.LastPrice();

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv status((char)CMessage::Status);
	CCsv Execution((char)CMessage::Status);

	if (TransType != "0")
	{
		return FALSE;
	}

	// check for executions embedded in a pending, ur out, or reject message
	switch (execution.Status())
	{	
	case COrder::PendingCancel:		// pending cancel or replace
	case COrder::ReplacePending:
	case COrder::Canceled:		// ur out
	case COrder::Rejected:		// order reject
		
		if (execution.LastShares())	
		{
			Execution.SetAt(CSV_SIDE,			execution.Side());
			Execution.SetAt(CSV_SYMBOL,			execution.Symbol());
			Execution.SetAt(CSV_SHARES,			execution.LastShares());
			Execution.SetAt(CSV_PRICE,			execution.LastPrice());
			Execution.SetAt(CSV_TIF,			execution.TimeInForce());
			Execution.SetAt(CSV_STATUS,			COrder::PartiallyFilled);
			Execution.SetAt(CSV_CLEARINGID,		execution.Account());
			Execution.SetAt(CSV_CLEARINGACCOUNT,"");
			Execution.SetAt(CSV_ORDER_TAG,		execution.OrderTag());
			Execution.SetAt(CSV_EXCHANGE_TAG,	execution.ExchangeTag());
			Execution.SetAt(CSV_STAFFORD_TAG,	execution.StaffordTag());
			Execution.SetAt(CSV_CANCEL_TAG,		execution.CancelTag());
			Execution.SetAt(CSV_EXEC_TAG,		execution.TradeTag());
			Execution.SetAt(CSV_EXEC_CONFIRM,	execution.Confirm());
			Execution.SetAt(CSV_REFERENCE,		execution.ReferenceConfirm());
			Execution.SetAt(CSV_REPORT,			"N"); //Exec.Report());
			Execution.SetAt(CSV_CONTRA,			execution.ExecBroker());
			Execution.SetAt(CSV_TEXT,			execution.Reason() + "(" + execution.Text() + ")");
            Execution.SetAt(CSV_BILLING_CODE,  execution.BillingCode());
			
			if (!(pApp->SendToClient(execution.SourceId(), Execution.Message())))
			{
				pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
				return(FALSE);
			}	
		}
		break;

	default:
		break;		
	};
	
	if ((execution.Status() == COrder::PartiallyFilled) || (execution.Status() == COrder::Filled))
	{
		CString s = (execution.Instructions()).Right(1);
		if (s == "-")
		{
			status.SetAt(CSV_QUERY_TAG, "DOT");
			int l = (execution.Instructions()).GetLength();
			s = (execution.Instructions()).Left(l - 1);
			execution.SetInstructions(s);
			
		}
		
		status.SetAt(CSV_SHARES,	execution.LastShares());
		status.SetAt(CSV_PRICE,	execution.LastPrice());
		
		if (execution.Text() == "P")
		{
			status.SetAt(CSV_TRADING_SESSION, "P");
		}
	}
	else if (execution.Status() == COrder::Replaced)
	{
		status.SetAt(CSV_SHARES, execution.Shares() - execution.Cumulative());
		status.SetAt(CSV_PRICE, execution.Price());
	}
	else
	{
		status.SetAt(CSV_SHARES,	execution.Shares());
		status.SetAt(CSV_PRICE,	execution.Price());
	}
	
	status.SetAt(CSV_SIDE,			execution.Side());
	status.SetAt(CSV_SYMBOL,		execution.Symbol());
	status.SetAt(CSV_TIF,			execution.TimeInForce());
	status.SetAt(CSV_STATUS,		execution.Status());
	status.SetAt(CSV_ORDER_TAG,		execution.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	execution.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	execution.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	execution.CancelTag());
	status.SetAt(CSV_EXEC_TAG,		execution.TradeTag());
	status.SetAt(CSV_EXEC_CONFIRM,	execution.Confirm());
	status.SetAt(CSV_REFERENCE,		execution.ReferenceConfirm());
	status.SetAt(CSV_REPORT,		"N");
	status.SetAt(CSV_CONTRA,		execution.ExecBroker());
	status.SetAt(CSV_TEXT,			execution.Reason() + "(" + execution.Text() + ")");
	status.SetAt(CSV_EXEC_TIME,		execution.TransactionTime());
	status.SetAt(CSV_BILLING_CODE,  execution.BillingCode());

	if (!(pApp->SendToClient(execution.SourceId(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Message Handler", "Error Sending Message To Client");
		return(FALSE);
	}

	return(FALSE);
}

BOOL CArcaStatus::SetStatus(COrder&	order)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	order.SetStatus(COrder::PendingNew);

	CCsv status((char)CMessage::Status);

	status.SetAt(CSV_TAG,				order.Tag());
	status.SetAt(CSV_SIDE,				order.Side());
	status.SetAt(CSV_SYMBOL,			order.Symbol());
	status.SetAt(CSV_SHARES,			order.Shares());
	status.SetAt(CSV_PRICE,				order.dPrice());
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

	if (!(pApp->SendToClient(order.SourceId(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CArcaStatus::SetStatus(CReject& reject)
{
	BOOL Result = FALSE;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv status((char)CMessage::Status);

	status.SetAt(CSV_SYMBOL,		reject.Symbol());
	status.SetAt(CSV_STATUS,		COrder::Rejected);
	status.SetAt(CSV_ORDER_TAG,		reject.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	reject.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	reject.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	reject.CancelTag());
	status.SetAt(CSV_TEXT,			reject.Reason() + "(" + reject.Text() + ")");

	if (!(pApp->SendToClient(reject.SourceId(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(Result);
}

BOOL CArcaStatus::SetStatus(CReplace& replace)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv status((char)CMessage::Status);

	status.SetAt(CSV_SIDE,			replace.Side());
	status.SetAt(CSV_SYMBOL,		replace.Symbol());
	status.SetAt(CSV_SHARES,		replace.Shares());
	status.SetAt(CSV_PRICE,			replace.Price());
	status.SetAt(CSV_STATUS,		COrder::PendingCancel);
	status.SetAt(CSV_ORDER_TAG,		replace.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	replace.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	replace.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	replace.CancelTag());

	if (!(pApp->SendToClient(replace.SourceId(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CArcaStatus::SetStatus(CReplaceReject& replaceReject)
{
	BOOL		result = FALSE;

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv status((char)CMessage::Status);

	status.SetAt(CSV_SYMBOL,		replaceReject.Symbol());
	status.SetAt(CSV_STATUS,		COrder::ReplaceRejected);
	status.SetAt(CSV_ORDER_TAG,		replaceReject.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	replaceReject.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	replaceReject.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	replaceReject.CancelTag());
	status.SetAt(CSV_TEXT,			replaceReject.Reason() + "(" + replaceReject.Text() + ")");

	if (!(pApp->SendToClient(replaceReject.SourceId(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return result;
}