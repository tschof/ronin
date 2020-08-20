// BrassStatus.cpp: implementation of the CBrassStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Brass.h"
#include "BrassStatus.h"
#include "Message.h"
#include "order.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBrassStatus::~CBrassStatus()
{
	delete m_pExecMap;
}

CBrassStatus::CBrassStatus() :
	CStatusHandler()
{
	CString Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	try 
	{ 
		m_pExecMap = new CExecutionMapSet(&pApp->Database());
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Opening status Tables (%s)", e->m_strError);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Brass status", Error);
		e->Delete();
	}
}

BOOL CBrassStatus::SetStatus(CCancel& cancel)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

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

	if (!(pApp->SendToClient(cancel.lpszOwner(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"status Handler", "Error Sending Message To Client:" + status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBrassStatus::SetStatus(CCancelReject& cancelReject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	int			Index;

	CCsv status((char)CMessage::Status);

	CString Id = cancelReject.SourceId();
	if ((Index = Id.Find(',')) > 0)
	{
		Owner = Id.Left(Index);
		Trader = Id.Mid(Index+1);
	}
	else
	{
		Trader = cancelReject.SourceId();
		Owner = cancelReject.SourceId();
	}

	status.SetAt(CSV_TRADER,		Trader);
	status.SetAt(CSV_SYMBOL,		cancelReject.Symbol());
	status.SetAt(CSV_STATUS,		COrder::CancelRejected);
	status.SetAt(CSV_ORDER_TAG,		cancelReject.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	cancelReject.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	cancelReject.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	cancelReject.CancelTag());
	status.SetAt(CSV_TEXT,			cancelReject.Reason());

	if (!(pApp->SendToClient(Owner, status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBrassStatus::SetStatus(CExecution& Exec)
{
	int		ExecType	= Exec.ExecType();
	int		status		= Exec.Status();
	CString TransType	= Exec.TransactionType();
	double	LastPrice	= Exec.LastPrice();
	double	LastShares	= Exec.LastShares();

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	int			Index;

	CCsv Message((char)CMessage::Status);
	CCsv Execution((char)CMessage::Status);

	CString Id = Exec.SourceId();
	if ((Index = Id.Find(',')) > 0)
	{
		Owner = Id.Left(Index);
		Trader = Id.Mid(Index+1);
	}
	else
	{
		Trader = Exec.SourceId();
		Owner = Exec.SourceId();
	}
	
	if (TransType == "0")		// new
	{
		// check for executions embedded in a pending, ur out, or reject message
		switch (status)
		{	
			case COrder::PendingCancel:		// pending cancel or replace
			case COrder::Canceled:		// ur out
			case COrder::Rejected:		// order reject
				if (LastShares)	
				{
					//if (ProcessTrade(Exec))
					//{
						Execution.SetAt(CSV_TRADER,			Trader);
						Execution.SetAt(CSV_SIDE,			Exec.Side());
						Execution.SetAt(CSV_SYMBOL,			Exec.Symbol());
						Execution.SetAt(CSV_SHARES,			Exec.LastShares());
						Execution.SetAt(CSV_PRICE,			Exec.LastPrice());
						Execution.SetAt(CSV_TIF,			Exec.TimeInForce());
						Execution.SetAt(CSV_STATUS,			COrder::PartiallyFilled);
						Execution.SetAt(CSV_CLEARINGID,		Exec.Account());
						Execution.SetAt(CSV_CLEARINGACCOUNT,"");
						Execution.SetAt(CSV_ORDER_TAG,		Exec.OrderTag());
						Execution.SetAt(CSV_EXCHANGE_TAG,	Exec.ExchangeTag());
						Execution.SetAt(CSV_STAFFORD_TAG,	Exec.StaffordTag());
						Execution.SetAt(CSV_CANCEL_TAG,		Exec.CancelTag());
						Execution.SetAt(CSV_EXEC_TAG,		Exec.TradeTag());
						Execution.SetAt(CSV_EXEC_CONFIRM,	Exec.Confirm());
						Execution.SetAt(CSV_REFERENCE,		Exec.ReferenceConfirm());
						Execution.SetAt(CSV_REPORT,			"N"); //Exec.Report());
						Execution.SetAt(CSV_CONTRA,			Exec.ExecBroker());
						Execution.SetAt(CSV_TEXT,			Exec.Reason() + "(" + Exec.Text() + ")");

						if (!(pApp->SendToClient(Owner, Execution.Message())))
						{
							pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"status Handler", "Error Sending Message To Client");
							return(FALSE);
						}	
					//}
				}
				break;
			default:
				break;		
		};
		
		if ((status == COrder::PartiallyFilled) || (status == COrder::Filled))
		{
			//if (!(ProcessTrade(Exec)))
			//	return(FALSE);

			Message.SetAt(CSV_LAST_SHARES,	Exec.LastShares());
			Message.SetAt(CSV_LAST_PRICE,	Exec.LastPrice());

			//For DBRecorder
			Message.SetAt(CSV_SHARES,	Exec.LastShares());
			Message.SetAt(CSV_PRICE,	Exec.LastPrice());
		}
		else if (status == COrder::Replaced)
		{
			Message.SetAt(CSV_LEAVE_SHARES,	Exec.Shares());
			Message.SetAt(CSV_PRICE,	Exec.Price());
			Message.SetAt(CSV_SHARES, Exec.Shares());
			Message.SetAt(CSV_TIF, Exec.TimeInForce());
			Message.SetAt(CSV_EXEC_INSTRUCTION, Exec.Instructions());
			Message.SetAt(CSV_STOPPRICE, Exec.StopPrice());
			Message.SetAt(CSV_TYPE, Exec.OrderType());
		}
		else
		{
			Message.SetAt(CSV_SHARES,	Exec.Shares());
			Message.SetAt(CSV_PRICE,	Exec.Price());
		}

		if(Trader != "")
		{
			Message.SetAt(CSV_TRADER,	Trader);
			Message.SetAt(CSV_CONTRA,	Exec.ExecBroker());
			Message.SetAt(CSV_REPORT,	"N");	//Exec.Report());
		}
		else
		{
			Message.SetAt(CSV_TRADER,			Exec.Trader());
			Message.SetAt(CSV_LOCALACCOUNT,		Exec.LocalAccount());
			Message.SetAt(CSV_CLEARINGACCOUNT,	Exec.ClearingAccount());
			Message.SetAt(CSV_FIRM,				Exec.Firm());
		}
		
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
		Message.SetAt(CSV_TEXT,			Exec.Reason() + "(" + Exec.Text() + ")");
		Message.SetAt(CSV_EXEC_TIME,	Exec.TransactionTime());

		// ExecBroker can't tell if the order was sent by TM
		// The fix tag 11 should be used
		if ((Exec.OrderTag()).Right(6) != _T("MANUAL"))
		//if (!Exec.ExecBroker().GetLength())
		{
			if (!(pApp->SendToClient(Owner, Message.Message())))
			{
				pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Message Handler", "Error Sending Message To Client");
				return(FALSE);
			}
		}
		else
		{
			Message.SetAt(CSV_TRADER,			Exec.Trader());
			Message.SetAt(CSV_LOCALACCOUNT,		Exec.LocalAccount());
			Message.SetAt(CSV_CLEARINGACCOUNT,	Exec.ClearingAccount());
			Message.SetAt(CSV_FIRM,				Exec.Firm());

			Message.SetAt(CSV_COMMAND,		(char)CMessage::TradeReport);
			Message.SetAt(CSV_EXCHANGE,		COrder::BRASS);

			if (!(pApp->SendToRecorder(Message.Message())))
			{
				pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Message Handler", "Error Sending Message To DBRecorder");
				return(FALSE);
			}
		}
	}
	
	return(FALSE);
}

BOOL CBrassStatus::SetStatus(COrder& order)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	order.SetStatus(COrder::PendingNew);

	CCsv	status((char)CMessage::Status);

	status.SetAt(CSV_TRADER,			order.Trader());
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

	if (!(pApp->SendToClient(order.lpszOwner(), status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"status Handler", "Error Sending Message To Client:" + status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBrassStatus::SetStatus(CReject& Reject)
{
	BOOL		Result = FALSE;

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	int			Index;

	CCsv	status((char)CMessage::Status);

	CString Id = Reject.SourceId();
	if ((Index = Id.Find(',')) > 0)
	{
		Owner = Id.Left(Index);
		Trader = Id.Mid(Index+1);
	}
	else
	{
		Trader = Reject.SourceId();
		Owner = Reject.SourceId();
	}

	status.SetAt(CSV_TRADER,		Trader);
	status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	status.SetAt(CSV_STATUS,		COrder::Rejected);
	status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	status.SetAt(CSV_TEXT,			Reject.Reason() + "(" + Reject.Text() + ")");

	if (!(pApp->SendToClient(Owner, status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(Result);
}


BOOL CBrassStatus::SetStatus(CReplaceReject& Reject)
{
	BOOL		Result = FALSE;

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	int			Index;

	CCsv	status((char)CMessage::Status);

	CString Id = Reject.SourceId();
	if ((Index = Id.Find(',')) > 0)
	{
		Owner = Id.Left(Index);
		Trader = Id.Mid(Index+1);
	}
	else
	{
		Trader = Reject.SourceId();
		Owner = Reject.SourceId();
	}

	status.SetAt(CSV_TRADER,		Trader);
	status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	status.SetAt(CSV_STATUS,		COrder::ReplaceRejected);
	status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	status.SetAt(CSV_TEXT,			Reject.Reason() + "(" + Reject.Text() + ")");

	if (!(pApp->SendToClient(Owner, status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(Result);
}

BOOL CBrassStatus::ProcessTrade(CExecution& Exec)
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
			pApp->ProcessErrorEx(MODE_ERR_BASIC,"Brass status", "Duplicate Execution Received (" + Exec.Confirm() + "): " + Exec.Debug());
			return(FALSE);
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Trade TradeId(%d) OrderId(%s): (%s)", TradeId, Exec.StaffordTag(), e->m_strError, Exec.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Brass status", Error);
		e->Delete();
		return(TRUE);	// return true even if db error so that trade gets processed
	}

	return(FALSE);
}