// ABNAMROFixSession.cpp: implementation of the CABNAMROFixSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ABNAMRO.h"
#include "ABNAMROFixSession.h"
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

CABNAMROFixSession::CABNAMROFixSession()
{
	m_mapExDestToABNAMRODest.insert(MapExDestToABNAMRODest::value_type("ISE",	"I"));
	m_mapExDestToABNAMRODest.insert(MapExDestToABNAMRODest::value_type("CBOE",	"W"));
	m_mapExDestToABNAMRODest.insert(MapExDestToABNAMRODest::value_type("AMEX",	"A"));
	m_mapExDestToABNAMRODest.insert(MapExDestToABNAMRODest::value_type("PHLX",	"X"));
	m_mapExDestToABNAMRODest.insert(MapExDestToABNAMRODest::value_type("PCX",	"P"));
	m_mapExDestToABNAMRODest.insert(MapExDestToABNAMRODest::value_type("BOX",	"B"));

	GetPrivateProfileString("Clearing Information", "DefaultCMTA", "", m_strDefaultCMTA.GetBuffer(5), 5, ((CExchangeApp *)AfxGetApp())->DatFile()); 
	m_strDefaultCMTA.ReleaseBuffer();
	((CExchangeApp *)AfxGetApp())->ProcessDebug("ABNAMRO DefaultCMTA: ",m_strDefaultCMTA);
}

CABNAMROFixSession::~CABNAMROFixSession()
{

}

long CABNAMROFixSession::ProcessEntry(COrder& a_Order)
{

	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER , ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing orders until fix session is corrected.");
		CString Msg("AFS:Fix session out of sequence:Can't send order.");
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

	if (a_Order.Type() == COrder::Limit || a_Order.Type() == COrder::LimitOrBetter)
	{
		l_Order.Price(a_Order.dPrice());
	}
	else if(a_Order.Type() != COrder::Market )
	{
		CString Msg("IFS:PE:Invalid Order Type.");
		throw Msg;
	}
	
	if (strcmp(TargetSubId().c_str(), "") != 0)
	{
		l_Order.TargetSubID(TargetSubId().c_str());
	}

	l_Order.SenderSubID(a_Order.SourceId());
	l_Order.OrderQty(a_Order.Shares());
	
	l_Order.Account(a_Order.ClearingId());
	
	if(COrder::Day == a_Order.TIF() || COrder::OPG == a_Order.TIF() || COrder::IC == a_Order.TIF() || COrder::GTC == a_Order.TIF())
		l_Order.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Order.TIF()]);
	else
	{
		CString Msg("IFS:PE:Invalid Time-In-Force.");
		throw Msg;
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


BOOL CABNAMROFixSession::OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder)
{
	COptFixSession::OrderEntryOverride(a_Order, a_FixOrder);

	if(a_Order.OpenClose() == 1)
		a_FixOrder.OpenClose("O");
	else if(a_Order.OpenClose() == 0)
		a_FixOrder.OpenClose("C");
	else
	{
		CString Msg("AFS:OEO:OpenClose parameter missing.");
		throw Msg;
	}

	MapExDestToABNAMRODest::iterator theIterator;
	theIterator = m_mapExDestToABNAMRODest.find((LPCSTR)a_Order.ExDestination());
	if(theIterator != m_mapExDestToABNAMRODest.end())
		a_FixOrder.ExDestination(((*theIterator).second).c_str() );
	else
	{
		CString Msg;
		Msg.Format("AFS:OEO:Not a valid ABN Destination.");
		((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":AFS:OEO:",Msg);
		throw Msg;
	}

	//CMTA or GIVEUP Setting
	CString GiveupAccount = a_Order.GiveUpAccount();
	CString CMTAAccount = a_Order.CMTAAccount();

	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixOrder, a_Order);

	switch(a_Order.Capacity())
	{
		case COrder::Customer://CUST
		case COrder::Agency:
			a_FixOrder.CustomerOrFirm(0);
			break;
		case COrder::BrokerDealerFirm://FIRM
		case COrder::Principal:
			a_FixOrder.CustomerOrFirm(1);
			break;
		case COrder::MarketMakerFirm://MM CUST
		case COrder::MarketMaker:
		case COrder::PrimaryMM:
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Order.MMAccount());
			a_FixOrder.CustomerOrFirm(0);
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9201)->Value("MM");
			break;
		case COrder::BrokerDealer://BD CUST
			a_FixOrder.CustomerOrFirm(0);
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9201)->Value("BD");
			break;
		default:
			//unknown type
			CString Msg;
			Msg.Format("IFS:OEO:Unknown Capacity code %c in order %s", a_Order.Capacity(),a_Order.OrderTag());
			((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":IFS:OEO:",Msg);
			throw Msg;
	}//	switch(a_Order.Capacity())
	
	if(a_Order.Type() == COrder::Market )
	{
		a_FixOrder.Price(0.0);
	}

	return TRUE;
}

BOOL CABNAMROFixSession::ProcessCancel(CCancel& a_Cancel)
{
	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing cancel until fix session is corrected.");
		CString Msg("AFS:Fix session out of sequence:Can't send cancel.");
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
	//l_Cancel.TargetSubID(m_TargetSubId.c_str());
	l_Cancel.SenderSubID(a_Cancel.SourceId());
	l_Cancel.OrderID(a_Cancel.ExchangeTag());
	l_Cancel.OrderQty(a_Cancel.Shares());

	//equity only
	//if(a_Cancel.ExpDate().ABNAMROmpty())
	//	l_Cancel.TransactTime((LPCTSTR)GetCurrentTimeStr());
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

BOOL CABNAMROFixSession::CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl)
{
	COptFixSession::CancelOverride(a_Cxl, a_FixCxl);

	switch(a_Cxl.Capacity())
	{
		case COrder::Customer://CUST
		case COrder::Agency:
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("0");
			break;
		case COrder::BrokerDealerFirm://FIRM
		case COrder::Principal:
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("1");
			break;
		case COrder::MarketMakerFirm://MM CUST
		case COrder::MarketMaker:
		case COrder::PrimaryMM:
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Cxl.MMAccount());
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("0");
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)9201)->Value("MM");
			break;
		case COrder::BrokerDealer:
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)204)->Value("0");
			a_FixCxl.GuaranteedGetCustomField((EFixFieldID)9201)->Value("BD");
			break;
		default:
			//unknown type
			CString Msg;
			Msg.Format("IFS:CO:Unknown Capacity code %c in order %s", a_Cxl.Capacity(),a_Cxl.OrderTag());
			((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":IFS:CO:",Msg);
			throw Msg;
	}//	switch(a_Cxl.Capacity())

	a_FixCxl.GuaranteedGetCustomField((EFixFieldID)59)->Value((EFixTimeInForce) Translator().Lookup(Translator().m_ToExchangeTIF,a_Cxl.TIF()));
	a_FixCxl.GuaranteedGetCustomField((EFixFieldID)40)->Value((EFixOrdType) Translator().Lookup(Translator().m_ToExchangeType, a_Cxl.Type()));
	a_FixCxl.GuaranteedGetCustomField((EFixFieldID)44)->Value(a_Cxl.Price());
		
	if(a_Cxl.OpenClose() == 1)
		a_FixCxl.GuaranteedGetCustomField((EFixFieldID)77)->Value("O");
	else if(a_Cxl.OpenClose() == 0)
		a_FixCxl.GuaranteedGetCustomField((EFixFieldID)77)->Value("C");
	else
	{
		CString Msg("AFS:CO:OpenClose parameter missing.");
		throw Msg;
	}

	return TRUE;
}

void CABNAMROFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order)
{
	if (CMTAAccount != "")
	{
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}
	else
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)439)->Value(m_strDefaultCMTA);
}


void CABNAMROFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace)
{
	if(CMTAAccount != "")
	{
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}
	else
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)439)->Value(m_strDefaultCMTA);

}

void CABNAMROFixSession::Process(const CFixExecution& a_Exec)
{
	if(!AddIncomingMsgToSequence(a_Exec))
		return;
	EFixExecTransType l_TransType = a_Exec.ExecTransType();
	if(l_TransType != FIX_TransType_Status && (FIX_Filled == a_Exec.ExecType() || FIX_PartiallyFilled == a_Exec.ExecType() || FIX_Canceled == a_Exec.ExecType())) {
		const EFixFieldID COND_REQ_FIELDS[] = {
			FIX_LastPx,
			FIX_LastShares
		};		
		if(!a_Exec.Includes(COND_REQ_FIELDS, COND_REQ_FIELDS + 2)) {
			SendRejectMsg(a_Exec.MsgSeqNum(), "Missing conditionally required fields.");
			return;
		}
	}
	EFixOrdStatus l_OrdStatus = a_Exec.OrdStatus();
	if(CFixMsg::GetVersionNum() > 4.099) {
		switch(l_OrdStatus) {
			case FIX_PendingCancel:
			case FIX_Replaced:{
				const EFixFieldID COND_REQ_FIELDS[] = {
					FIX_OrigClOrdID
				};
				if(!a_Exec.Includes(COND_REQ_FIELDS, COND_REQ_FIELDS + 1)) {
					SendRejectMsg(a_Exec.MsgSeqNum(), "Missing conditionally required fields.");
					return;
				}
				break;
			}
			default:
				break;
		}
	}
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
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"ABNAMRO:Busted Option Trade", l_MsgStr);
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
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"ABNAMRO:Option Trade Correction", l_MsgStr);
		return;
	}

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

void CABNAMROFixSession::Process(const CFixNews & a_News)
{
	if(!AddIncomingMsgToSequence(a_News))
		return;

	CString l_Text;
	l_Text = a_News.Text().c_str();
	//Take the subject out of the fix messages, propagate it through CFixNews and then put it into the subject!
	ProcessEmail(CString(""),l_Text);
}


void CABNAMROFixSession::Process(const CFixResendRequest& a_Request)
{
	int l_BeginSeqNo = a_Request.BeginSeqNo();
	int l_EndSeqNo = a_Request.EndSeqNo();
	TOStrStream l_LogMsg;
	l_LogMsg << "Resend request received -- BeginSeqNo: " << l_BeginSeqNo << " EndSeqNo: " << l_EndSeqNo;
	ProcessLogEntry(l_LogMsg.str().c_str());
	int l_ExpectedSeqNo = l_BeginSeqNo;
	int l_CurrSeqNo;
	TOStrStream l_Stream;
	const CFixMsgSequence::TFixMsgListAutoPtr l_MsgListPtr = m_OutgoingMsgs.FindRange(l_BeginSeqNo, l_EndSeqNo);
	for(CFixMsgSequence::TFixMsgList::iterator i=l_MsgListPtr->begin(); i!=l_MsgListPtr->end(); ++i) {
		l_CurrSeqNo = i->MsgSeqNum();
		switch(i->MsgType()) {
			case FIX_Heartbeat:
			case FIX_Logon:
			case FIX_TestRequest:
			case FIX_ResendRequest:
			case FIX_Reject:
			case FIX_SequenceReset:
			case FIX_Logout:
				continue;
			case FIX_OrderSingle: {
				CFixOrder l_Order;
				l_Order.Swap(*i);
				CReject l_Reject;
				l_Reject.SetOrderTag(l_Order.ClOrdID().c_str());
				l_Reject.SetSymbol(l_Order.Symbol().c_str());
				l_Reject.SetReason("Sequence Reset Error");
				((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
				continue;
			}
			default: {
				if(l_CurrSeqNo > l_ExpectedSeqNo) {
					TString l_TimeStr = GetCurrentTimeStr();
					CFixSequenceReset l_Reset(l_ExpectedSeqNo, l_TimeStr.c_str(), l_CurrSeqNo);
					l_Reset.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);

					l_Reset.GapFillFlag(FIX_Yes);
					l_Reset.PossDupFlag(FIX_Yes);
					l_Reset.PossResend(FIX_No);//SPECIAL FIX for ABN
					l_Reset.OrigSendingTime(l_TimeStr.c_str());
					l_Stream << l_Reset;
				}
				i->PossDupFlag(FIX_Yes);
				i->PossResend(FIX_No);//SPECIAL FIX for ABN
				i->OrigSendingTime(i->SendingTime().c_str());
				i->SendingTime(GetCurrentTimeStr());
				l_Stream << *i;
				l_ExpectedSeqNo = l_CurrSeqNo + 1;
				break;
			}
		}
	}
	if(l_EndSeqNo == FIX_MSG_SEQ_NUM_MAX) {
		if(l_ExpectedSeqNo < m_OutgoingMsgs.NextMsgSeqNum()) {
			TString l_TimeStr = GetCurrentTimeStr();
			CFixSequenceReset l_Reset(l_ExpectedSeqNo, l_TimeStr.c_str(), m_OutgoingMsgs.NextMsgSeqNum());
			l_Reset.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
			l_Reset.GapFillFlag(FIX_Yes);
			l_Reset.PossDupFlag(FIX_Yes);
			l_Reset.PossResend(FIX_No);//SPECIAL FIX for ABN
			l_Reset.OrigSendingTime(l_TimeStr.c_str());
			l_Stream << l_Reset;
		}
	}
	else if(l_ExpectedSeqNo <= l_EndSeqNo) {
		TString l_TimeStr = GetCurrentTimeStr();
		CFixSequenceReset l_Reset(l_ExpectedSeqNo, l_TimeStr.c_str(), m_OutgoingMsgs.NextMsgSeqNum());
		l_Reset.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
		l_Reset.GapFillFlag(FIX_Yes);
		l_Reset.PossDupFlag(FIX_Yes);
		l_Reset.PossResend(FIX_No);//SPECIAL FIX for ABN
		l_Reset.OrigSendingTime(l_TimeStr.c_str());
		l_Stream << l_Reset;
	}
	((CMessageApp*) AfxGetApp())->Host().SendMessage(l_Stream.str().c_str());
	AddIncomingMsgToSequence(a_Request);
}
