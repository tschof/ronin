// ExchangeStatus.cpp: implementation of the CExchangeStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExchangeApp.h"
#include "ExchangeStatus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExchangeStatus::CExchangeStatus():
	CStatusHandler()
{
	CString Error;
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

	m_nExchangeID	= GetPrivateProfileInt(pApp->Name() + " Server", "Exchange Destination Id", -1, pApp->DatFile());
}

CExchangeStatus::~CExchangeStatus()
{
}


BOOL CExchangeStatus::SetStatus(COrder&	Order)
{
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

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
	Status.SetAt(CSV_CLEARINGACCOUNT,	Order.ClearingAccount());
	Status.SetAt(CSV_EXCHANGE,			Order.Exchange());
	Status.SetAt(CSV_STATUS,			Order.Status());
	Status.SetAt(CSV_ORDER_TAG,			Order.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,		Order.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,		Order.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,		Order.CancelTag());
	
	if (!(pApp->SendToClient(pApp->GetPCName(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + Status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CExchangeStatus::SetStatus(CCancel& Cancel)
{
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

	CCsv Status((char)CMessage::Status);

	Status.SetAt(CSV_TRADER,		Cancel.Trader());
	Status.SetAt(CSV_SIDE,			Cancel.Side());
	Status.SetAt(CSV_SYMBOL,		Cancel.Symbol());
	Status.SetAt(CSV_SHARES,		Cancel.Shares());
	Status.SetAt(CSV_PRICE,			Cancel.Price());
	Status.SetAt(CSV_EXCHANGE,		m_nExchangeID);
	Status.SetAt(CSV_STATUS,		COrder::PendingCancel);
	Status.SetAt(CSV_ORDER_TAG,		Cancel.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Cancel.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Cancel.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Cancel.CancelTag());

	if (!(pApp->SendToClient(pApp->GetPCName(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + Status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CExchangeStatus::SetStatus(CReplace& Replace)
{
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

	CCsv Status((char)CMessage::Status);

	Status.SetAt(CSV_TRADER,			Replace.Trader());
	Status.SetAt(CSV_SIDE,				Replace.Side());
	Status.SetAt(CSV_SYMBOL,			Replace.Symbol());
	Status.SetAt(CSV_SHARES,			Replace.Shares());
	Status.SetAt(CSV_PRICE,				Replace.Price());
	Status.SetAt(CSV_EXCHANGE,			m_nExchangeID);
	Status.SetAt(CSV_STATUS,			COrder::ReplacePending);
	Status.SetAt(CSV_ORDER_TAG,			Replace.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,		Replace.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,		Replace.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,		Replace.CancelTag());
	Status.SetAt(CSV_TIF,				Replace.TIF());
	Status.SetAt(CSV_EXEC_INSTRUCTION,	Replace.Instructions());
	
	if(Replace.StopPrice() > 0.0 )
			Status.SetAt(CSV_STOPPRICE,		Replace.StopPrice());

	if (!(pApp->SendToClient(pApp->GetPCName(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + Status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CExchangeStatus::SetStatus(CExecution& Exec)
{
	int		ExecType	= Exec.ExecType();
	int		Status		= Exec.Status();
	CString TransType	= Exec.TransactionType();
	double	LastPrice	= Exec.LastPrice();
	double	LastShares	= Exec.LastShares();

	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

	CCsv Message((char)CMessage::Status);
	CCsv Execution((char)CMessage::Status);

	
	if (TransType == "0")		// new
	{
		// check for executions embedded in a pending, ur out, or reject message
		switch (Status)
		{	
			case COrder::PendingCancel:	// pending cancel or replace
			case COrder::Canceled:		// ur out
			case COrder::Rejected:		// order reject
				if (LastShares)	
				{
						Execution.SetAt(CSV_TAG,				Exec.SourceId());
						Execution.SetAt(CSV_SIDE,				Exec.Side());
						Execution.SetAt(CSV_SYMBOL,				Exec.Symbol());
						Execution.SetAt(CSV_LAST_SHARES,		Exec.LastShares());
						Execution.SetAt(CSV_PRICE,				Exec.LastPrice());
						Execution.SetAt(CSV_TIF,				Exec.TimeInForce());
						Execution.SetAt(CSV_EXCHANGE,			m_nExchangeID);
						Execution.SetAt(CSV_STATUS,				COrder::PartiallyFilled);
						Execution.SetAt(CSV_CLEARINGID,			Exec.Account());
						Execution.SetAt(CSV_CLEARINGACCOUNT,	"");
						Execution.SetAt(CSV_ORDER_TAG,			Exec.OrderTag());
						Execution.SetAt(CSV_EXCHANGE_TAG,		Exec.ExchangeTag());
						Execution.SetAt(CSV_STAFFORD_TAG,		Exec.StaffordTag());
						Execution.SetAt(CSV_CANCEL_TAG,			Exec.CancelTag());
						Execution.SetAt(CSV_EXEC_TAG,			Exec.TradeTag());
						Execution.SetAt(CSV_EXEC_TIME,			Exec.TransactionTime());
						Execution.SetAt(CSV_EXEC_CONFIRM,		Exec.Confirm());
						Execution.SetAt(CSV_REFERENCE,			Exec.ReferenceConfirm());
						Execution.SetAt(CSV_REPORT,				"N"); //Exec.Report());
						Execution.SetAt(CSV_CONTRA,				Exec.ExecBroker());
						Execution.SetAt(CSV_TEXT,				Exec.Reason() + "(" + Exec.Text() + ")");
						Execution.SetAt(CSV_SHARES,				Exec.Shares());
						Execution.SetAt(CSV_TYPE,				Exec.OrderType());
						Execution.SetAt(CSV_EXEC_INSTRUCTION,	Exec.Instructions());

						if(Exec.Price() > 0.0 )
							Execution.SetAt(CSV_PRICE,		Exec.Price());

						if(Exec.StopPrice() > 0.0 )
							Execution.SetAt(CSV_STOPPRICE,		Exec.StopPrice());

						if(Exec.Leaves() > 0 )
							Execution.SetAt(CSV_LEAVE_SHARES,		Exec.Leaves());

						if (!(pApp->SendToClient(pApp->GetPCName(), Execution.Message())))
						{
							pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
							return(FALSE);
						}	
				}
				break;
			default:
				break;		
		};
		
		if ((Status == COrder::PartiallyFilled) || (Status == COrder::Filled))
		{
			Message.SetAt(CSV_LAST_SHARES,	Exec.LastShares());
			Message.SetAt(CSV_LAST_PRICE,	Exec.LastPrice());
		}

		Message.SetAt(CSV_TAG,				Exec.SourceId());
		Message.SetAt(CSV_SIDE,				Exec.Side());
		Message.SetAt(CSV_SYMBOL,			Exec.Symbol());
		Message.SetAt(CSV_TIF,				Exec.TimeInForce());
		Message.SetAt(CSV_EXCHANGE,			m_nExchangeID);
		Message.SetAt(CSV_STATUS,			Exec.Status());
		Message.SetAt(CSV_ORDER_TAG,		Exec.OrderTag());
		Message.SetAt(CSV_EXCHANGE_TAG,		Exec.ExchangeTag());
		Message.SetAt(CSV_STAFFORD_TAG,		Exec.StaffordTag());
		Message.SetAt(CSV_CANCEL_TAG,		Exec.CancelTag());
		Message.SetAt(CSV_EXEC_TAG,			Exec.TradeTag());
		Message.SetAt(CSV_EXEC_TIME,		Exec.TransactionTime());
		Message.SetAt(CSV_EXEC_CONFIRM,		Exec.Confirm());
		Message.SetAt(CSV_REFERENCE,		Exec.ReferenceConfirm());
		Message.SetAt(CSV_REPORT,			"N");	//Exec.Report());
		Message.SetAt(CSV_CONTRA,			Exec.ExecBroker());
		Message.SetAt(CSV_TEXT,				Exec.Reason() + "(" + Exec.Text() + ")");
		Message.SetAt(CSV_SHARES,			Exec.Shares());
		Message.SetAt(CSV_TYPE,				Exec.OrderType());
		Message.SetAt(CSV_EXEC_INSTRUCTION,	Exec.Instructions());
		
		if(Exec.Price() > 0.0 )
			Message.SetAt(CSV_PRICE,		Exec.Price());

		if(Exec.StopPrice() > 0.0 )
			Message.SetAt(CSV_STOPPRICE,		Exec.StopPrice());

		if((Status == COrder::Replaced) && Exec.Leaves() > 0 )
			Message.SetAt(CSV_LEAVE_SHARES,		Exec.Leaves());

		if (!(pApp->SendToClient(pApp->GetPCName(), Message.Message())))
		{
			pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Message Handler", "Error Sending Message To Client");
			return(FALSE);
		}
	}

	return(FALSE);
}

BOOL CExchangeStatus::SetStatus(CReject& Reject)
{
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

	CCsv	Status((char)CMessage::Status);

	Status.SetAt(CSV_TAG,			Reject.SourceId());
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_EXCHANGE,		m_nExchangeID);
	Status.SetAt(CSV_STATUS,		COrder::Rejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	Status.SetAt(CSV_TEXT,			Reject.Reason() + "(" + Reject.Text() + ")");

	if (!(pApp->SendToClient(pApp->GetPCName(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(TRUE);
}

BOOL CExchangeStatus::SetStatus(CCancelReject& Reject)
{
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

	CCsv Status((char)CMessage::Status);

	Status.SetAt(CSV_TAG,			Reject.SourceId());
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_EXCHANGE,		m_nExchangeID);
	Status.SetAt(CSV_STATUS,		COrder::CancelRejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	Status.SetAt(CSV_TEXT,			Reject.Reason() + "(" + Reject.Text() + ")");
	
	if (!(pApp->SendToClient(pApp->GetPCName(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CExchangeStatus::SetStatus(CReplaceReject& Reject)
{
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();

	CCsv	Status((char)CMessage::Status);

	Status.SetAt(CSV_TAG,			Reject.SourceId());
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_EXCHANGE,		m_nExchangeID);
	Status.SetAt(CSV_STATUS,		COrder::ReplaceRejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	Status.SetAt(CSV_TEXT,			Reject.Reason() + "(" + Reject.Text() + ")");
	
	if (!(pApp->SendToClient(pApp->GetPCName(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(TRUE);
}

