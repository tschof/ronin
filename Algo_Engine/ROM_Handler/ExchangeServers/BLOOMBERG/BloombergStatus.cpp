// BloombergStatus.cpp: implementation of the CBloombergStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bloomberg.h"
#include "BloombergStatus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBloombergStatus::~CBloombergStatus()
{
	delete m_pExecMap;
}

CBloombergStatus::CBloombergStatus() :
	CStatusHandler()
{
	CString Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	try
	{ 
		m_pExecMap	= new CExecutionMapSet(&pApp->Database());	
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Opening Status Tables (%s)", e->m_strError);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Bloomberg Status", Error);
		e->Delete();
	}
}

BOOL CBloombergStatus::SetStatus(COrder&	Order)
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
	Status.SetAt(CSV_CLEARINGACCOUNT,	Order.ClearingAccount());
	Status.SetAt(CSV_EXCHANGE,			Order.Exchange());
	Status.SetAt(CSV_STATUS,			Order.Status());
	Status.SetAt(CSV_ORDER_TAG,			Order.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,		Order.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,		Order.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,		Order.CancelTag());

	if (!(pApp->SendToClient(Order.lpszOwner(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + Status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBloombergStatus::SetStatus(CCancel& Cancel)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CCsv Status((char)CMessage::Status);

	Status.SetAt(CSV_TRADER,			Cancel.Trader());
	Status.SetAt(CSV_SIDE,				Cancel.Side());
	Status.SetAt(CSV_SYMBOL,			Cancel.Symbol());
	Status.SetAt(CSV_SHARES,			Cancel.Shares());
	Status.SetAt(CSV_PRICE,				Cancel.Price());
	Status.SetAt(CSV_STATUS,			COrder::PendingCancel);
	Status.SetAt(CSV_ORDER_TAG,			Cancel.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,		Cancel.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,		Cancel.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,		Cancel.CancelTag());

	if (!(pApp->SendToClient(Cancel.lpszOwner(), Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client:" + Status.Message());
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBloombergStatus::SetStatus(CCancelReject& Reject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	int			Index;

	CCsv	Status((char)CMessage::Status);

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

	Status.SetAt(CSV_TRADER,		Trader);
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_STATUS,		COrder::CancelRejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	try
	{
		Status.SetAt(CSV_TEXT,		Reject.Reason());
	}
	catch(...)
	{
		Status.SetAt(CSV_TEXT, "Unknown");
	}

	if (!(pApp->SendToClient(Owner, Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
			
	return(TRUE);
}

BOOL CBloombergStatus::SetStatus(CReject& Reject)
{
	BOOL		Result = FALSE;

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	int			Index;

	CCsv	Status((char)CMessage::Status);

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

	Status.SetAt(CSV_TRADER,		Trader);
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_STATUS,		COrder::Rejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	try
	{
		Status.SetAt(CSV_TEXT,		Reject.Reason());
	}
	catch(...)
	{
		Status.SetAt(CSV_TEXT, "Unknown: Invalid Account?");
	}

	if (!(pApp->SendToClient(Owner, Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(Result);
}


BOOL CBloombergStatus::SetStatus(CReplaceReject& Reject)
{
	BOOL		Result = FALSE;

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Trader;
	CString		Owner;
	int			Index;

	CCsv	Status((char)CMessage::Status);

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

	Status.SetAt(CSV_TRADER,		Trader);
	Status.SetAt(CSV_SYMBOL,		Reject.Symbol());
	Status.SetAt(CSV_STATUS,		COrder::ReplaceRejected);
	Status.SetAt(CSV_ORDER_TAG,		Reject.OrderTag());
	Status.SetAt(CSV_EXCHANGE_TAG,	Reject.ExchangeTag());
	Status.SetAt(CSV_STAFFORD_TAG,	Reject.StaffordTag());
	Status.SetAt(CSV_CANCEL_TAG,	Reject.CancelTag());
	try
	{
		Status.SetAt(CSV_TEXT,		Reject.Reason());
	}
	catch(...)
	{
		Status.SetAt(CSV_TEXT, "Unknown: Invalid Account?");
	}

	if (!(pApp->SendToClient(Owner, Status.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Status Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	
	return(Result);
}


BOOL CBloombergStatus::SetStatus(CExecution& Exec)
{
	int		ExecType	= Exec.ExecType();
	int		Status		= Exec.Status();
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
	
	if ((Status == COrder::PartiallyFilled) || (Status == COrder::Filled))
	{
		//if (!(ProcessTrade(Exec)))
		//	return(FALSE);

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
	Message.SetAt(CSV_EXEC_TIME,	Exec.TransactionTime());

	if (!(pApp->SendToClient(Owner, Message.Message())))
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,"Message Handler", "Error Sending Message To Client");
		return(FALSE);
	}
	return(FALSE);
}


BOOL CBloombergStatus::ProcessTrade(CExecution& Exec)
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
			pApp->ProcessErrorEx(MODE_ERR_BASIC,"Bloomberg Status", "Dublicate Execution Received (" + Exec.Confirm() + "): " + Exec.Debug());
			return(FALSE);
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Trade TradeId(%d) OrderId(%s): (%s)", TradeId, Exec.StaffordTag(), e->m_strError, Exec.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Bloomberg Status", Error);
		e->Delete();
		return(TRUE);	// return true even if db error so that trade gets processed
	}

	return(FALSE);
}
