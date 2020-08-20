// ISEFixSession.cpp: implementation of the CISEFixSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISE.h"
#include "ISEFixSession.h"
#include "exchangehandler.h"
#include "fixadminmsgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CISEFixSession::CISEFixSession()
{
	m_bIgnoreAwayMarketForTNTO	= GetPrivateProfileInt("Fix Information", "IgnoreAwayMarketForTNTO", 0, ((CMessageApp*) AfxGetApp())->DatFile()) == 0 ? false : true;
	m_bIgnoreNewsMsg			= GetPrivateProfileInt("Fix Information", "IgnoreNewsMsg", 0, ((CMessageApp*) AfxGetApp())->DatFile()) == 0 ? false : true;
}

CISEFixSession::~CISEFixSession()
{

}

long CISEFixSession::ProcessEntry(COrder& a_Order)
{
	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER , ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing orders until fix session is corrected.");
		CString Msg("IFS:Fix session out of sequence:Can't send order.");
		throw Msg;
	}

	long l_SeqNum = OutgoingMsgs().NextMsgSeqNum();
	CFixOrder l_Order(
		l_SeqNum, 
		GetCurrentTimeStr(), 
		a_Order.OrderTag(), 
		FIX_AutoPublic,
		a_Order.Symbol(), 
		(EFixSide) Translator().Lookup(Translator().m_ToExchangeSide, a_Order.Side()), 
		(EFixOrdType) Translator().Lookup(Translator().m_ToExchangeType, a_Order.Type())
	);

	if (a_Order.Type() == COrder::Stop)
	{
		l_Order.StopPx(a_Order.StopPrice());
	}
	else if (a_Order.Type() == COrder::StopLimit)
	{
		l_Order.StopPx(a_Order.StopPrice());
		l_Order.Price(a_Order.dPrice());
	}
	else if (a_Order.Type() != COrder::Market && a_Order.Type() != COrder::MarketOnClose && a_Order.Type() != COrder::MarketAtOpen)
	{
		l_Order.Price(a_Order.dPrice());
	}
	
	if (strcmp(TargetSubId().c_str(), "") != 0)
	{
		l_Order.TargetSubID(TargetSubId().c_str());
	}

	l_Order.SenderSubID(a_Order.SourceId());
	l_Order.OrderQty(a_Order.Shares());
	
	l_Order.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Order.TIF()]);
	
	if (a_Order.ExecInst() != COrder::LastExecInst)
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Order.ExecInst());
		l_Order.ExecInst(ExecInst);
	}

	if ((a_Order.ExpTime()).IsEmpty() != TRUE)
	{
		l_Order.ExpireTime((LPCTSTR)a_Order.ExpTime());
	}

	if(a_Order.SecurityType() != COrder::Option)
		l_Order.Rule80A(a_Order.Capacity());

	if (a_Order.Floor() != 0)
	{
		l_Order.MaxFloor(a_Order.Floor());
	}

	if(a_Order.ExDestination() != "")
		l_Order.ExDestination(a_Order.ExDestination());

	if(OrderEntryOverride(a_Order, l_Order) )
		if(!SendMsg(l_Order))
		{
			CString Msg("IFS:PE:Error in SendMsg.");
			throw Msg;
		}
	
	return TRUE;
}


BOOL CISEFixSession::OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder)
{
	COptFixSession::OrderEntryOverride(a_Order, a_FixOrder);
	a_FixOrder.SetText(a_Order.ClearingId() + "-" + a_Order.ClearingAccount());
	
	
	if(a_Order.OpenClose() == 1)
		a_FixOrder.OpenClose("O");
	else if(a_Order.OpenClose() == 0)
		a_FixOrder.OpenClose("C");
	else
	{
		CString Msg("IFS:OEO:OpenClose parameter missing.");
		throw Msg;
	}

	if(m_bIgnoreAwayMarketForTNTO && a_Order.Trader().Compare("FIXTNTO") == 0)
	{
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9205)->Value(1);
	}
	
	//CMTA or GIVEUP Setting
	CString GiveupAccount = a_Order.GiveUpAccount();
	CString CMTAAccount = a_Order.CMTAAccount();

	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixOrder, a_Order);

	switch(a_Order.Capacity())
	{
		case COrder::Customer:
		case COrder::Agency://depricated
			a_FixOrder.CustomerOrFirm(0);
			break;
		case COrder::BrokerDealerFirm:
		case COrder::Principal://depricated
			a_FixOrder.CustomerOrFirm(1);
			break;
		case COrder::MarketMakerFirm:
		case COrder::MarketMaker:
		case COrder::PrimaryMM:			
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Order.MMAccount());
			a_FixOrder.CustomerOrFirm(5);
			break;
		case COrder::BrokerDealer:
			a_FixOrder.CustomerOrFirm(3);
			break;
		default:
			//unknown type
			CString Msg;
			Msg.Format("IFS:OEO:Unknown Capacity code %c in order %s", a_Order.Capacity(),a_Order.OrderTag());
			((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":IFS:OEO:",Msg);
			throw Msg;
	}//	switch(a_Order.Capacity())
	

	return TRUE;
}

BOOL CISEFixSession::ProcessCancel(CCancel& a_Cancel)
{

	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing cancel until fix session is corrected.");
		CString Msg("IFS:Fix session out of sequence:Can't send cancel.");
		throw Msg;
	}

	CFixCancel l_Cancel(
		m_OutgoingMsgs.NextMsgSeqNum(),
		GetCurrentTimeStr(),
		a_Cancel.OrderTag(),
		a_Cancel.CancelTag(),
		a_Cancel.Symbol(),
		(EFixSide) m_Translator.m_ToExchangeSide[a_Cancel.Side()]
	);

	if (strcmp(TargetSubId().c_str(), "") != 0)
	{
		l_Cancel.TargetSubID(m_TargetSubId.c_str());
	}

	l_Cancel.SenderSubID(a_Cancel.SourceId());
	l_Cancel.OrderID(a_Cancel.ExchangeTag());
	l_Cancel.OrderQty(a_Cancel.Shares());

	if(CFixMsg::GetVersionNum() < 4.099)
		l_Cancel.CxlType(FIX_FullCxl);

	if(CancelOverride(a_Cancel, l_Cancel))
		if(!SendMsg(l_Cancel))
		{
			CString Msg("IFS:PC:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CISEFixSession::CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl)
{
	COptFixSession::CancelOverride(a_Cxl, a_FixCxl);

	switch(a_Cxl.Capacity())
	{
		case COrder::Customer:
		case COrder::Agency://depricated
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("0");
			break;
		case COrder::BrokerDealerFirm:
		case COrder::Principal://depricated
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("1");
			break;
		case COrder::MarketMakerFirm:
		case COrder::MarketMaker:
		case COrder::PrimaryMM:			
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Cxl.MMAccount());
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("5");
			break;
		case COrder::BrokerDealer:
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("3");
			break;
		default:
			//unknown type
			CString Msg;
			Msg.Format("IFS:CO:Unknown Capacity code %c in order %s", a_Cxl.Capacity(),a_Cxl.OrderTag());
			((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":IFS:CO:",Msg);
			throw Msg;
	}//	switch(a_Cxl.Capacity())

	return TRUE;
}

BOOL CISEFixSession::ProcessReplace(CReplace& a_Replace)
{
	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing replace until fix session is corrected.");
		CString Msg("IFS:Fix session out of sequence:Can't replace order.");
		throw Msg;
	}

	CFixReplace l_Replace(
		m_OutgoingMsgs.NextMsgSeqNum(),
		GetCurrentTimeStr(),
		a_Replace.OrderTag(),
		a_Replace.CancelTag(),
		FIX_AutoPublic,
		a_Replace.Symbol(),
		(EFixSide) m_Translator.m_ToExchangeSide[a_Replace.Side()],
		(EFixOrdType) m_Translator.m_ToExchangeType[a_Replace.Type()]
	);

	if (strcmp(TargetSubId().c_str(), "") != 0)
	{
		l_Replace.TargetSubID(m_TargetSubId.c_str());
	}
	
	if (strcmp(a_Replace.SourceId(), "") != 0)
	{
		l_Replace.SenderSubID(a_Replace.SourceId());
	}		

	if (a_Replace.ExecInst() != COrder::LastExecInst)
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Replace.ExecInst());
		l_Replace.ExecInst(ExecInst);
	}

	l_Replace.OrderQty(a_Replace.Shares());
	
	if (a_Replace.Type() == COrder::Stop)
	{
		l_Replace.StopPx(a_Replace.StopPrice());
	}
	else if (a_Replace.Type() == COrder::StopLimit)
	{
		l_Replace.StopPx(a_Replace.StopPrice());
		l_Replace.Price(a_Replace.Price());
	}
	else if (a_Replace.Type() != COrder::Market && a_Replace.Type() != COrder::MarketOnClose && a_Replace.Type() != COrder::MarketAtOpen)
	{
		l_Replace.Price(a_Replace.Price());
	}

	if (a_Replace.Floor() != 0)
	{
		l_Replace.MaxFloor(a_Replace.Floor());
	}

	if(ReplaceOverride(a_Replace, l_Replace))
		if(!SendMsg(l_Replace))
		{
			CString Msg("IFS:PR:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CISEFixSession::ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace)
{
	COptFixSession::ReplaceOverride(a_Replace, a_FixReplace);
		
	if(a_Replace.OpenClose() == 1)
		a_FixReplace.OpenClose("O");
	else if(a_Replace.OpenClose() == 0)
		a_FixReplace.OpenClose("C");
	else
	{
		CString Msg("IFS:RO:OpenClose parameter missing.");
		throw Msg;
	}

	if(m_bIgnoreAwayMarketForTNTO && a_Replace.Trader().Compare("FIXTNTO") == 0)
	{
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)9205)->Value(1);
	}
	
	CString GiveupAccount = a_Replace.GiveUpAccount();
	CString CMTAAccount = a_Replace.CMTAAccount();

	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixReplace, a_Replace);

	switch(a_Replace.Capacity())
	{
		case COrder::Customer:
		case COrder::Agency://depricated
			a_FixReplace.GuaranteedGetCustomField((EFixFieldID)204)->Value("0");
			break;
		case COrder::BrokerDealerFirm:
		case COrder::Principal://depricated
			a_FixReplace.GuaranteedGetCustomField((EFixFieldID)204)->Value("1");
			break;
		case COrder::MarketMakerFirm:
		case COrder::MarketMaker:
		case COrder::PrimaryMM:			
			a_FixReplace.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Replace.MMAccount());
			a_FixReplace.GuaranteedGetCustomField((EFixFieldID)204)->Value("5");
			break;
		case COrder::BrokerDealer:
			a_FixReplace.GuaranteedGetCustomField((EFixFieldID)204)->Value("3");
			break;
		default:
			//unknown type
			CString Msg;
			Msg.Format("IFS:RO:Unknown Capacity code %c in order %s", a_Replace.Capacity(),a_Replace.OrderTag());
			((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":IFS:RO:",Msg);
			throw Msg;
	}//	switch(a_Replace.Capacity())

	return TRUE;
}

BOOL CISEFixSession::ProcessSpread(OrderVector& orderVector)
{
	return TRUE;
}

void CISEFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order)
{
	if (GiveupAccount != "")
	{
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)76)->Value(GiveupAccount);
	}
	
	if (CMTAAccount != "")
	{
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}
}


void CISEFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace)
{
	if (GiveupAccount != "")
	{
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)76)->Value(GiveupAccount);
	}
	
	if (CMTAAccount != "")
	{
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}
}

void CISEFixSession::Process(const CFixExecution& a_Exec)
{
	if(!AddIncomingMsgToSequence(a_Exec))
		return;
	
	EFixExecTransType l_TransType = a_Exec.ExecTransType();
	
	if(l_TransType == FIX_TransType_Cancel)
	{
		CString l_strOrdType;
		MapOrderTypeStrings::iterator theIterator;
		theIterator = m_mapOrderTypeStrings.find(a_Exec.OrdType());
		if(theIterator != m_mapOrderTypeStrings.end())
			l_strOrdType = ((*theIterator).second).c_str();

		CString l_strTIF;
		MapTIFStrings::iterator theTIFIterator;
		theTIFIterator = m_mapTIFStrings.find(a_Exec.TimeInForce());
		if(theTIFIterator != m_mapTIFStrings.end())
			l_strTIF = ((*theTIFIterator).second).c_str();

		//busted trade
		CString l_MsgStr;
		l_MsgStr.Format("Exchange order ID: %s\nAffected Execution Ref ID: %s\nSide: %s\nOriginal Shares: %d\nSymbol: %s\nYearMonth: %s\nStrike Price: %f\nPutOrCall: %s\nOrder Type: %s\nOriginal Price: %f\nTime-in-Force: %s\nLast Shares(busted): %d\nLast Price: %f\nBust Time (GMT): %s\nCustomer: %s",
						a_Exec.ClOrdID().c_str(),
						a_Exec.ExecRefID().c_str(),
						a_Exec.Side() == 1?"BUY":"SELL",
						a_Exec.OrderQty(),
						a_Exec.Symbol().c_str(),
						a_Exec.MaturityMonthYear().c_str(),
						a_Exec.StrikePrice(),
						a_Exec.PutOrCall() == 0?"PUT":"CALL",
						l_strOrdType,
						a_Exec.Price(),
						l_strTIF,
						a_Exec.LastShares(),//busted trade
						a_Exec.LastPx(),
						a_Exec.SendingTime().c_str(),
						a_Exec.TargetSubID().c_str());
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"ISE:Busted Option Trade", l_MsgStr);
		return;
	}
	else if(l_TransType == FIX_TransType_Correct)
	{
		CString l_strOrdType;
		MapOrderTypeStrings::iterator theIterator;
		theIterator = m_mapOrderTypeStrings.find(a_Exec.OrdType());
		if(theIterator != m_mapOrderTypeStrings.end())
			l_strOrdType = ((*theIterator).second).c_str();

		CString l_strTIF;
		MapTIFStrings::iterator theTIFIterator;
		theTIFIterator = m_mapTIFStrings.find(a_Exec.TimeInForce());
		if(theTIFIterator != m_mapTIFStrings.end())
			l_strTIF = ((*theTIFIterator).second).c_str();

		//trade correction
		CString l_MsgStr;
		l_MsgStr.Format("Exchange order ID: %s\nAffected Execution Ref ID: %s\nSide: %s\nOriginal Shares: %d\nSymbol: %s\nYearMonth: %s\nStrike Price: %f\nPutOrCall: %s\nOrder Type: %s\nOriginal Price: %f\nTime-in-Force: %s\nLast Shares: %d\nLast Price: %f\nCorrection Time (GMT): %s\nCustomer: %s",
						a_Exec.ClOrdID().c_str(),
						a_Exec.ExecRefID().c_str(),
						a_Exec.Side() == 1?"BUY":"SELL",
						a_Exec.OrderQty(),
						a_Exec.Symbol().c_str(),
						a_Exec.MaturityMonthYear().c_str(),
						a_Exec.StrikePrice(),
						a_Exec.PutOrCall() == 0?"PUT":"CALL",
						l_strOrdType,
						a_Exec.Price(),
						l_strTIF,
						a_Exec.LastShares(),
						a_Exec.LastPx(),
						a_Exec.SendingTime().c_str(),
						a_Exec.TargetSubID().c_str());
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"ISE:Option Trade Correction", l_MsgStr);
		return;
	}

	EFixOrdStatus l_OrdStatus = a_Exec.OrdStatus();

	CExecution l_Exec;
	l_Exec.SetSourceId(a_Exec.TargetSubID().c_str());
	l_Exec.SetReport((char) a_Exec.ReportToExch());
	l_Exec.SetOrderTag(a_Exec.ClOrdID().c_str());
	l_Exec.SetExchangeTag(a_Exec.OrderID().c_str());
	l_Exec.SetOriginalTag(a_Exec.OrigClOrdID().c_str());
	l_Exec.SetExecBroker(a_Exec.ExecBroker().c_str());
	l_Exec.SetConfirm(a_Exec.ExecID().c_str());
	l_Exec.SetTransactionType(a_Exec.ExecTransType());
	l_Exec.SetReferenceConfirm(a_Exec.ExecRefID().c_str());
	l_Exec.SetAccount(a_Exec.Account().c_str());
	l_Exec.SetSymbol(a_Exec.Symbol().c_str());
	l_Exec.SetInstructions(a_Exec.ExecInst().c_str());
	l_Exec.SetText(a_Exec.Text().c_str());
	l_Exec.SetStatus(m_Translator.m_ToStaffordStatus[l_OrdStatus]);
	l_Exec.SetSide(m_Translator.m_ToStaffordSide[a_Exec.Side()]);
	l_Exec.SetShares(a_Exec.OrderQty());
	l_Exec.SetOrderType(m_Translator.m_ToStaffordType[a_Exec.OrdType()]);
	l_Exec.SetTimeInForce(m_Translator.m_ToStaffordTIF[a_Exec.TimeInForce()]);
	l_Exec.SetLastShares(a_Exec.LastShares());
	l_Exec.SetLeaves(a_Exec.LeavesQty());
	l_Exec.SetCumulative(a_Exec.CumQty());
	l_Exec.SetAverage(a_Exec.AvgPx());
	l_Exec.SetPrice(a_Exec.Price());
	l_Exec.SetStopPrice(a_Exec.StopPx());
	l_Exec.SetLastPrice(a_Exec.LastPx());
	l_Exec.SetExecType(m_Translator.m_ToStaffordStatus[a_Exec.ExecType()]);
	l_Exec.SetTransactionTime((a_Exec.TransactTime()).c_str());
	l_Exec.SetTradeTag((a_Exec.ExecID()).c_str());

	if (TRUE == ExecutionOverride(l_Exec, a_Exec))
	{
		((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Exec);
	}
}

void CISEFixSession::Process(const CFixCancelReject& a_Reject)
{
	//This message used to reject both cancels and cancel\replace

	if(!AddIncomingMsgToSequence(a_Reject))
		return;

	if(1 == atoi(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(434).c_str()))
	{
		CCancelReject l_Reject;
		l_Reject.SetOrderTag(a_Reject.OrigClOrdID().c_str());
		l_Reject.SetExchangeTag(a_Reject.OrderID().c_str());
		l_Reject.SetCancelTag(a_Reject.ClOrdID().c_str());
		l_Reject.SetStatus(m_Translator.m_ToStaffordStatus[a_Reject.OrdStatus()]);
		l_Reject.SetReason(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(58).c_str());
		((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
	}
	else if(2 == atoi(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(434).c_str()))
	{
		CReplaceReject l_Reject;
		l_Reject.SetOrderTag(a_Reject.OrigClOrdID().c_str());
		l_Reject.SetExchangeTag(a_Reject.OrderID().c_str());
		l_Reject.SetCancelTag(a_Reject.ClOrdID().c_str());
		//l_Reject.SetStatus(m_Translator.m_ToStaffordStatus[a_Reject.OrdStatus()]);
		l_Reject.SetReason(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(58).c_str());
		((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
	}
}

void CISEFixSession::Process(const CFixNews & a_News)
{
	if(!AddIncomingMsgToSequence(a_News))
		return;

	CString l_Text;
	l_Text = a_News.Text().c_str();
	//Take the subject out of the fix messages, propagate it through CFixNews and then put it into the subject!
	if(!m_bIgnoreNewsMsg)
		ProcessEmail(CString(""),l_Text);
}