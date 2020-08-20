// BOXFixSession.cpp: implementation of the CBOXFixSession class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "BOX.h"
#include "BOXFixSession.h"
#include "fixmsg42\FixAdminMsgs.h"
#include "exchangehandler.h"
#include "fixmsg42\Fix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBOXFixSession::CBOXFixSession()
{
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("ISE","15"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("CBOE","W"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("AMEX","1"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("PHLX","X"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("PCX","8"));

	ReloadFirmCombinedOrderType();
}

CBOXFixSession::~CBOXFixSession()
{

}

void CBOXFixSession::ReloadFirmCombinedOrderType()
{
	m_nCombinedOrderType = GetPrivateProfileInt("Fix Information", "CombinedOrdType", 0, ((CMessageApp *)AfxGetApp())->DatFile());

	if(m_nCombinedOrderType != 2 && m_nCombinedOrderType != 3)
	{
		AfxMessageBox("CombinedOrdType not defined in BOX.dat");
		m_nCombinedOrderType = 3;
	}
}

BOOL CBOXFixSession::Logon()
{
	if(!m_LogonInitiated && !m_LogoutInitiated)
	{
		ProcessLogEntry("Sending logon.");
		CFixLogon l_Logon(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr(), m_Encrypt,
			m_SendHeartbtInt);

		LogonOverride(l_Logon);

		if(SendMsg(l_Logon))
		{
			m_LogonInitiated = true;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CBOXFixSession::Logout()
{
	ProcessLogEntry("Sending logout.");
	CFixLogout l_Logout(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr());
	
	//l_Logout.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);

	LogoutOverride(l_Logout);

	if(SendMsg(l_Logout))
	{
		m_LogoutInitiated = true;
		return TRUE;
	}
	
	Shutdown();
	return FALSE;
}

void CBOXFixSession::Process(const CFixLogout& a_Logout)
{
	if(!AddIncomingMsgToSequence(a_Logout))
	{
		return;
	}

	TOStrStream l_LogMsg;
	l_LogMsg << "Logout received -- Text: " << a_Logout.Text();
	ProcessLogEntry(l_LogMsg.str().c_str());

	if(!m_LogoutInitiated)
	{
		ProcessLogEntry("Sending logout acknowledgement.");
		CFixLogout l_Logout(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr());
		SendMsg(l_Logout);
		m_LogoutInitiated = true;
	}
	else
	{
		m_LogoutInitiated = false;
		Shutdown();
	}
}

long CBOXFixSession::ProcessEntry(COrder& a_Order)
{
	
	long l_SeqNum = m_OutgoingMsgs.NextMsgSeqNum();

	CString clientOrderId = a_Order.OrderTag();

	CFixOrder l_Order(
		l_SeqNum, 
		GetCurrentTimeStr(), 
		a_Order.OrderTag(), 
		FIX_AutoPublic,
		a_Order.Symbol(), 
		(EFixSide) Translator().Lookup(Translator().m_ToExchangeSide, a_Order.Side()), 
		(EFixOrdType) Translator().Lookup(Translator().m_ToExchangeType, a_Order.Type())
	);

	if (a_Order.Type() == COrder::Limit)
	{
		l_Order.Price(a_Order.dPrice());
	}

	l_Order.OrderQty(a_Order.Shares());

	l_Order.TimeInForce((EFixTimeInForce) Translator().Lookup(Translator().m_ToExchangeTIF, a_Order.TIF())); 

	if(a_Order.SecurityType() != COrder::Option)
	{
		l_Order.Rule80A(a_Order.Capacity());
	}

	if(OrderEntryOverride(a_Order, l_Order) )
		if(!SendMsg(l_Order))
		{
			CString Msg("BFS:PE:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CBOXFixSession::OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder)
{
	
	CString strInstructions = a_Order.Instructions();
	strInstructions.MakeUpper();
	
	if( (COrder::PrimaryMM !=  a_Order.Capacity() && COrder::MarketMaker !=  a_Order.Capacity() && COrder::MarketMakerFirm !=  a_Order.Capacity()) && ( -1 != strInstructions.Find("P:",0) ) )
	{
		CString Msg;
		Msg.Format("BFS:OEO:Only Market Maker can send Linkage Order.");
		((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:OEO:",Msg);
		throw Msg;
	}

	if(-1 != strInstructions.Find("INCROWD",0) )
	{
		CString Msg;
		Msg.Format("BFS:OEO:BOX doesn't accept InCrowd orders");
		((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:OEO:",Msg);
		throw Msg;
	}

	//option specific stuff
	a_FixOrder.SecurityType("OPT");

	a_FixOrder.Account(a_Order.ClearingAccount());
	
	a_FixOrder.SetText(a_Order.ClearingId() + "-" + a_Order.ClearingAccount());
	
	a_FixOrder.MaturityMonthYear((LPCTSTR) a_Order.ExpDate());

	if(a_Order.CallOrPut() == 'P')
		a_FixOrder.PutOrCall(0);
	else if(a_Order.CallOrPut() == 'C')
		a_FixOrder.PutOrCall(1);
	else
	{
		CString Msg("BFS:OEO:PutOrCall parameter missing.");
		throw Msg;
	}

	if(1 == a_Order.OpenClose())
		a_FixOrder.OpenClose("O");
	else if(0 == a_Order.OpenClose())
		a_FixOrder.OpenClose("C");
	else
	{
		CString Msg("BFS:OEO:OpenClose parameter missing.");
		throw Msg;
	}

	a_FixOrder.StrikePrice(a_Order.StrikePrice());


	//CMTA or GIVEUP Setting
	CString GiveupAccount = "";//a_Order.GiveUpAccount(); //since BOX deosn't take GiveUp Account
	CString CMTAAccount = a_Order.CMTAAccount();

	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixOrder, a_Order);

	a_FixOrder.TransactTime((LPCTSTR)GetCurrentTimeStr());

	switch(a_Order.Capacity())
	{
	case COrder::Agency://Customer
	case COrder::Customer:
	case COrder::BrokerDealer:
		a_FixOrder.Rule80A('C');
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)7906)->Value(1); //default value as well
		break;

	case COrder::Principal://Firm
	case COrder::BrokerDealerFirm: //Broker Dealer Firm
		a_FixOrder.Rule80A('F');
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)7906)->Value(m_nCombinedOrderType); //default value as well
		break;

	//Market Maker
	case COrder::MarketMaker:
	case COrder::PrimaryMM:
	case COrder::MarketMakerFirm://Market Maker Firm		
		if(-1 != strInstructions.Find("P:",0)) //Linkage Order
		{
			if((a_Order.Type() != COrder::Limit))
			{
				CString Msg;
				Msg.Format("BFS:OEO:Linkage order must be a Limit order.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:OEO:",Msg);
				throw Msg;
			}
			
			MapDestToLinkDest::iterator theIterator;
			theIterator = m_mapDestToLinkDest.find((LPCTSTR)strInstructions.Mid(2));
			if(theIterator != m_mapDestToLinkDest.end())
				a_FixOrder.ExDestination(((*theIterator).second).c_str() );
			else
			{
				CString Msg;
				Msg.Format("BFS:OEO:Not a valid Linkage Destination.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:OEO:",Msg);
				throw Msg;
			}

			a_FixOrder.Rule80A('M');
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)6528)->Value("P");
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)7906)->Value(4);
		}
		else if(-1 != strInstructions.Find("S:",0)) //Satisfaction Order
		{

			CExchangeHandler* l_pHandler =(CExchangeHandler*) &( (CMessageApp *)AfxGetApp() )->Handler();
			COptionMemTagSet*& l_MemTag = l_pHandler->GetMemTag();

			int index1 = strInstructions.Find("S:",0);
			index1 += 2;
			int index2 = strInstructions.Find("#",index1);
						
			if (l_MemTag->GetTags( MAP_STAFFORD_TAG_QUERY, strInstructions.Mid(index1,index2-index1) ) )
			{
				a_FixOrder.GuaranteedGetCustomField((EFixFieldID)41)->Value(l_MemTag->OrderTag());//Original Order ID
				l_MemTag->GetTags( MAP_STAFFORD_TAG_QUERY, a_Order.StaffordTag() );

				index1 = strInstructions.Find("#",++index2);
				a_FixOrder.GuaranteedGetCustomField((EFixFieldID)5202)->Value(strInstructions.Mid(index2,index1-index2));//UTC

				index2 = strInstructions.Find("#",++index1);
				a_FixOrder.GuaranteedGetCustomField((EFixFieldID)5203)->Value(strInstructions.Mid(index1,index2-index1));//Order Size

				index1 = strInstructions.Find("#",++index2);
				a_FixOrder.GuaranteedGetCustomField((EFixFieldID)5204)->Value(strInstructions.Mid(index2,index1-index2));//Order Price

				index2 = strInstructions.Find("#",++index1);
				MapDestToLinkDest::iterator theIterator;
				theIterator = m_mapDestToLinkDest.find((LPCTSTR)strInstructions.Mid(index1,index2-index1));
				if(theIterator != m_mapDestToLinkDest.end())
					a_FixOrder.ExDestination(((*theIterator).second).c_str() );//ExDestination
				else
				{
					CString Msg;
					Msg.Format("BFS:OEO:Not a valid Linkage Destination.");
					((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:OEO:",Msg);
					throw Msg;
				}

				a_FixOrder.Rule80A('C');
				a_FixOrder.GuaranteedGetCustomField((EFixFieldID)6528)->Value("R");
				a_FixOrder.GuaranteedGetCustomField((EFixFieldID)7906)->Value(2);
			}
			else
			{
				CString Msg;
				Msg.Format("BFS:OEO:Original Order ID does not exist.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:OEO:",Msg);
				throw Msg;
			}
		}
		else //Market Maker Order
		{
			a_FixOrder.Rule80A('M');
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)7906)->Value(2);
		}

		a_FixOrder.Account(a_Order.MMAccount());

		break;
	default:
		//unknown type
		CString Msg;
		Msg.Format("Unknown Capacity code %c in order %s with OrderID %s", a_Order.Capacity(),a_Order.Tag(),a_Order.OrderTag());
		((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":BFS:OEO:",Msg);
		throw LPCTSTR(Msg);
	}//	switch(a_Order.Capacity())

	return TRUE;
}

BOOL CBOXFixSession::ProcessCancel(CCancel& a_Cancel)
{
	CFixCancel l_Cancel(
		m_OutgoingMsgs.NextMsgSeqNum(),
		GetCurrentTimeStr(),
		a_Cancel.OrderTag(),
		a_Cancel.CancelTag(),
		a_Cancel.Symbol(),
		(EFixSide) m_Translator.m_ToExchangeSide[a_Cancel.Side()]
		);

	if(CancelOverride(a_Cancel, l_Cancel))
		if(!SendMsg(l_Cancel))
		{
			CString Msg("BFS:PC:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CBOXFixSession::CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl)
{
	BOOL ret = COptFixSession::CancelOverride(a_Cxl, a_FixCxl);

	a_FixCxl.TransactTime(GetCurrentTimeStr());

	return ret;
}

BOOL CBOXFixSession::ProcessReplace(CReplace& a_Replace)
{
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
	
	if (strcmp(a_Replace.SourceId(), "") != 0)
	{
		l_Replace.SenderSubID(a_Replace.SourceId());
	}		

	l_Replace.OrderQty(a_Replace.Shares());
	l_Replace.Price(a_Replace.Price());
	
	if (a_Replace.Floor() != 0)
	{
		l_Replace.MaxFloor(a_Replace.Floor());
	}
	
	if(ReplaceOverride(a_Replace, l_Replace))
		if(!SendMsg(l_Replace))
		{
			CString Msg("BFS:PR:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CBOXFixSession::ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace)
{
	a_FixReplace.SecurityType("OPT");
	a_FixReplace.MaturityMonthYear((LPCTSTR) a_Replace.ExpDate());
	//not all exchanges may except TIF in replaces!!!!
	a_FixReplace.TimeInForce((EFixTimeInForce) Translator().Lookup(Translator().m_ToExchangeTIF,a_Replace.TIF()));
	a_FixReplace.StrikePrice(a_Replace.StrikePrice());
	
	CString strInstructions = a_Replace.Instructions();
	strInstructions.MakeUpper();
	
	if( -1 != strInstructions.Find("P:",0) ) 
	{
		CString Msg;
		Msg.Format("BFS:RO:Linkage orders cant be replaced.");
		((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:RO:",Msg);
		throw Msg;
	}

	if(-1 != strInstructions.Find("INCROWD",0) )
	{
		CString Msg;
		Msg.Format("BFS:RO:BOX doesn't accept InCrowd orders");
		((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":BFS:RO:",Msg);
		throw Msg;
	}

	
	if(a_Replace.CallOrPut() == 'P')
		a_FixReplace.PutOrCall(0);
	else if(a_Replace.CallOrPut() == 'C')
		a_FixReplace.PutOrCall(1);
	else
	{
		CString Msg("BFS:RO:PutOrCall parameter missing.");
		throw Msg;
	}

	if(1 == a_Replace.OpenClose())
		a_FixReplace.OpenClose("O");
	else if(0 == a_Replace.OpenClose())
		a_FixReplace.OpenClose("C");
	else
	{
		CString Msg("BFS:RO:OpenClose parameter missing.");
		throw Msg;
	}

	//a_FixReplace.Rule80A(a_Replace.Capacity());

	switch(a_Replace.Capacity())
	{
	case COrder::Agency://Customer
	case COrder::Customer:
	case COrder::BrokerDealer:
		a_FixReplace.Rule80A('C');
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)7906)->Value(1); //default value as well
		break;

	case COrder::Principal://Firm
	case COrder::BrokerDealerFirm: //Broker Dealer Firm
		a_FixReplace.Rule80A('F');
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)7906)->Value(m_nCombinedOrderType); //default value as well
		break;

	//Market Maker
	case COrder::MarketMaker:
	case COrder::PrimaryMM:
	case COrder::MarketMakerFirm://Market Maker Firm
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)7906)->Value(2);	
		a_FixReplace.Rule80A('M');
		a_FixReplace.Account(a_Replace.MMAccount());
		break;
	default:
		//unknown type
		CString Msg;
		Msg.Format("Unknown Capacity code %c in order %s", a_Replace.Capacity(),a_Replace.OrderTag());
		((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":BFS:RO:",Msg);
		throw LPCTSTR(Msg);
	}//	switch(a_Replace.Capacity())

	
	CString GiveupAccount = "";//a_Replace.GiveUpAccount();
	CString CMTAAccount = a_Replace.CMTAAccount();

	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixReplace, a_Replace);

	a_FixReplace.TransactTime((LPCTSTR)GetCurrentTimeStr());

	return TRUE;
}

void CBOXFixSession::Process(const CFixExecution& a_Exec)
{
	if(!AddIncomingMsgToSequence(a_Exec))
		return;
	
	EFixExecTransType l_TransType = a_Exec.ExecTransType();
	
	if(l_TransType == FIX_TransType_Cancel)
	{
		//busted trade
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
		l_MsgStr.Format("Exchange order ID: %s\nAffected Execution Ref ID: %s\nSide: %s\nOriginal Shares: %d\nSymbol: %s\nYearMonth: %s\nStrike Price: %f\nPutOrCall: %s\nOrder Type: %s\nOriginal Price: %f\nTime-in-Force: %s\nLast Shares(busted): %d\nLast Price: %f\nBust Time (GMT): %s",
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
						a_Exec.SendingTime().c_str());
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"BOX:Busted Option Trade", l_MsgStr);
		return;

	}
	else if(l_TransType == FIX_TransType_Correct)
	{
		//trade correction
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
		l_MsgStr.Format("Exchange order ID: %s\nAffected Execution Ref ID: %s\nSide: %s\nOriginal Shares: %d\nSymbol: %s\nYearMonth: %s\nStrike Price: %f\nPutOrCall: %s\nOrder Type: %s\nOriginal Price: %f\nTime-in-Force: %s\nLast Shares: %d\nLast Price: %f\nCorrection Time (GMT): %s",
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
						a_Exec.SendingTime().c_str());
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"BOX:Option Trade Correction", l_MsgStr);
		return;
	}
	
	EFixOrdStatus l_OrdStatus = a_Exec.OrdStatus();
	
	CExecution l_Exec;
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
	l_Exec.SetTransactionTime(a_Exec.TransactTime().c_str());

	if (TRUE == ExecutionOverride(l_Exec, a_Exec))
	{
		((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Exec);
	}
}

BOOL CBOXFixSession::ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec)
{
	if(!COptFixSession::ExecutionOverride(Exec, a_Exec))
		return FALSE;

	//extract the cancel Qty from the Custom Cancel Qty field
	if(Exec.ExecType() == COrder::Canceled || Exec.ExecType() == COrder::DoneForDay )
		Exec.SetShares(Exec.LastShares());

	if(Exec.ExecType() == COrder::Rejected )
	{
		CString reason;
		reason.Format("Reject Reason:%d", a_Exec.OrdRejReason());
		Exec.SetReason(reason);
	}

	Exec.SetTradeTag(a_Exec.ExecID().c_str());

	//convert a market order to a limit order after the first partial fill
	if(Exec.OrderType() == COrder::Market)
		if( (Exec.ExecType() == COrder::PartiallyFilled) || (Exec.ExecType() == COrder::Filled && Exec.LastShares() != Exec.Shares()) )
		{
			Exec.SetOrderType(COrder::Limit);
			Exec.SetPrice(Exec.LastPrice());
		}

	return TRUE;
}

void CBOXFixSession::Process(const CFixCancelReject& a_Reject)
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

void CBOXFixSession::Process(const CFixResendRequest& a_Request)
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
	
	for(CFixMsgSequence::TFixMsgList::iterator i=l_MsgListPtr->begin(); i!=l_MsgListPtr->end(); ++i)
	{
		l_CurrSeqNo = i->MsgSeqNum();
		
		switch(i->MsgType()) 
		{
			case FIX_Heartbeat:
			case FIX_Logon:
			case FIX_TestRequest:
			case FIX_ResendRequest:
			case FIX_Reject:
			case FIX_SequenceReset:
			case FIX_Logout:
				continue;

			case FIX_OrderSingle:
			{
				CFixOrder l_Order;
				l_Order.Swap(*i);
				CReject l_Reject;
				l_Reject.SetOrderTag(l_Order.ClOrdID().c_str());
				l_Reject.SetSymbol(l_Order.Symbol().c_str());
				l_Reject.SetReason("Sequence Reset Error");
				((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
				continue;
			}

		default:
			{
				if(l_CurrSeqNo > l_ExpectedSeqNo)
				{
					TString l_TimeStr = GetCurrentTimeStr();
					CFixSequenceReset l_Reset(l_ExpectedSeqNo, l_TimeStr.c_str(), l_CurrSeqNo);
					//l_Reset.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
				
					l_Reset.GapFillFlag(FIX_Yes);
					l_Reset.PossDupFlag(FIX_Yes);
					l_Reset.PossResend(FIX_Yes);
					l_Reset.OrigSendingTime(l_TimeStr.c_str());
					l_Stream << l_Reset;
				}
			
				i->PossDupFlag(FIX_Yes);
				i->PossResend(FIX_Yes);
				i->OrigSendingTime(i->SendingTime().c_str());
				i->SendingTime(GetCurrentTimeStr());
				l_Stream << *i;
				l_ExpectedSeqNo = l_CurrSeqNo + 1;

				break;
			}
		}
	}

	if(l_EndSeqNo == FIX_MSG_SEQ_RESEND_ALL)
	{
		if(l_ExpectedSeqNo < m_OutgoingMsgs.NextMsgSeqNum())
		{
			TString l_TimeStr = GetCurrentTimeStr();
			CFixSequenceReset l_Reset(l_ExpectedSeqNo, l_TimeStr.c_str(), m_OutgoingMsgs.NextMsgSeqNum());
			//l_Reset.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
			l_Reset.GapFillFlag(FIX_Yes);
			l_Reset.PossDupFlag(FIX_Yes);
			l_Reset.PossResend(FIX_Yes);
			l_Reset.OrigSendingTime(l_TimeStr.c_str());

			SequenceResetOverride(l_Reset);

			l_Stream << l_Reset;
		}
	}
	else if(l_ExpectedSeqNo <= l_EndSeqNo)
	{
		TString l_TimeStr = GetCurrentTimeStr();
		CFixSequenceReset l_Reset(l_ExpectedSeqNo, l_TimeStr.c_str(), l_EndSeqNo + 1);
		//l_Reset.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
		l_Reset.GapFillFlag(FIX_Yes);
		l_Reset.PossDupFlag(FIX_Yes);
		l_Reset.PossResend(FIX_Yes);
		l_Reset.OrigSendingTime(l_TimeStr.c_str());

		SequenceResetOverride(l_Reset);

		l_Stream << l_Reset;
	}

	((CMessageApp*) AfxGetApp())->Host().SendMessage(l_Stream.str().c_str());
	AddIncomingMsgToSequence(a_Request);
}

bool CBOXFixSession::AddIncomingMsgToSequence(const CFixMsg& a_Msg)
{
	CFixMsgSequence::EAddMsgReturnCode l_RetCode;

	if((l_RetCode = m_IncomingMsgs.AddMsg(a_Msg)) != CFixMsgSequence::ADD_MSG_NO_ERROR)
	{
		switch(l_RetCode)
		{
		
		case CFixMsgSequence::ADD_MSG_SEQ_NUM_GAP:
			
			if(!m_AwaitingResend)
			{
				CFixResendRequest l_ResendReq(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr(), 
					m_IncomingMsgs.NextMsgSeqNum(), FIX_MSG_SEQ_RESEND_ALL);

				ResendOverride(l_ResendReq);

				SendMsg(l_ResendReq);

				m_AwaitingResend = true;
			}
			
			break;

		case CFixMsgSequence::ADD_MSG_SEQ_NUM_SMALLER_THAN_EXPECTED:

			{
				TOStrStream l_ErrMsg;
				l_ErrMsg << "Received: " << a_Msg.MsgSeqNum() << " Expecting: " << m_IncomingMsgs.NextMsgSeqNum();
				ProcessError(("SERIOUS ERROR: Incoming sequence number smaller than expected -- " + l_ErrMsg.str()).c_str());
				SendRejectMsg(a_Msg.MsgSeqNum(), ("MsgSeqNum value smaller than expected -- " + l_ErrMsg.str()).c_str());

				break;
			}
			
		default:
			ProcessError("Unknown error occurred while adding incoming msg to sequence.");
		}

		return false;
	}

	if(m_AwaitingResend)
	{
		m_AwaitingResend = false;
	}

	return true;
}

void CBOXFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order)
{
	if (CMTAAccount != "")
	{
		a_FixOrder.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}
}


void CBOXFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace)
{
	if (CMTAAccount != "")
	{
		a_FixReplace.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}
}
