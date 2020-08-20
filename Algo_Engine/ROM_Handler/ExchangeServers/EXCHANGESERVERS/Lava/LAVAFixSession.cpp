/*******************************************************************************
 *
 * Copyright (c) 2006 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * LAVAFixSession.cpp: implementation of the CLAVAFixSession class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *
 *******************************************************************************/

#include "stdafx.h"
#include "LAVA.h"
#include "LAVAFixSession.h"
#include "exchangehandler.h"
#include "LavaMemTagSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef std::vector<std::string> vectorString;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLAVAFixSession::CLAVAFixSession()
{
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type('0'					,"Unknown"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_Market				,"Market"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_Limit				,"Limit"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_Stop				,"Stop"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_StopLimit			,"Stop Limit"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_MarketOnClose		,"Market on Close"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_WithOrWithout		,"With or Without"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_LimitOrBetter		,"Limit or Better"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_LimitWithOrWithout	,"Limit with or Without"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_OnBasis			,"On Basis"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_OnClose			,"On Close"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_LimitOnClose		,"Limit on Close"));
	m_mapOrderTypeStrings.insert(MapOrderTypeStrings::value_type(FIX_MarketAtOpen		,"Market At Open"));

	m_mapTIFStrings.insert(MapTIFStrings::value_type(FIX_Day,"DAY"));
	m_mapTIFStrings.insert(MapTIFStrings::value_type(FIX_GTC,"GTC"));
	m_mapTIFStrings.insert(MapTIFStrings::value_type(FIX_OPG,"OPG"));
	m_mapTIFStrings.insert(MapTIFStrings::value_type(FIX_IC ,"OC"));
	m_mapTIFStrings.insert(MapTIFStrings::value_type(FIX_FOK,"FOK"));
	m_mapTIFStrings.insert(MapTIFStrings::value_type(FIX_GTX,"GTX"));
	m_mapTIFStrings.insert(MapTIFStrings::value_type(FIX_GTD,"GTD"));

	InitializeFixCustomTags();
}

CLAVAFixSession::~CLAVAFixSession()
{

}


void CLAVAFixSession::InitializeFixCustomTags()
{
	CMessageApp* l_pApp = (CMessageApp*) AfxGetApp();
	TString l_FileName = l_pApp->DatFile();
	TString l_Buf;
	l_Buf.resize(129);

	
	GetPrivateProfileString("Fix Information", "ExecBroker", "", (char*) l_Buf.c_str(), 8,l_FileName.c_str()); 
	m_execBroker = l_Buf.c_str();

	
	GetPrivateProfileString("Fix Information", "UserName", "", (char*) l_Buf.c_str(), 8,l_FileName.c_str()); 
	m_user = l_Buf.c_str();

	GetPrivateProfileString("Fix Information", "Password", "", (char*) l_Buf.c_str(), 8,l_FileName.c_str()); 
	m_pass = l_Buf.c_str();
}


BOOL CLAVAFixSession::Logon()
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

void CLAVAFixSession::LogonOverride(CFixLogon& l_FixLogon)
{
	l_FixLogon.GuaranteedGetCustomField((EFixFieldID)553)->Value(m_user.c_str());
	l_FixLogon.GuaranteedGetCustomField((EFixFieldID)554)->Value(m_pass.c_str());
}

void CLAVAFixSession::Process(const CFixLogout& a_Logout)
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
		l_Logout.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
		SendMsg(l_Logout);
		m_LogoutInitiated = true;
	}
	else
	{
		m_LogoutInitiated = false;
		Shutdown();
	}
}

long CLAVAFixSession::ProcessEntry(COrder& a_Order)
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

    // { WXU 
    //- l_Order.SenderSubID(a_Order.SourceId());
    TString senderSubId = SenderSubId();
    if ( senderSubId.length() > 0 )
        l_Order.SenderSubID((const char*)senderSubId.c_str());
    else
        l_Order.SenderSubID(a_Order.SourceId());

    const char* shortLender = a_Order.ShortLender();
    if (shortLender && *shortLender) {
        l_Order.GuaranteedGetCustomField((EFixFieldID)FIX_ShortLender)->Value(shortLender);
    } else if(a_Order.Side() == COrder::Short) {
        l_Order.GuaranteedGetCustomField((EFixFieldID)FIX_ShortLender)->Value("xyz");
    }
    // }
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

    //if(a_Order.ExDestination() != "")
    //	l_Order.ExDestination(a_Order.ExDestination());

    if (a_Order.Exchange() == COrder::DIRECTPLUS || a_Order.ExDestination() == "Direct+")
    {
        l_Order.ExDestination("+");
    }
    else 
    {
        if(a_Order.ExDestination() != "")
            l_Order.ExDestination(a_Order.ExDestination());

    }

    if(OrderEntryOverride(a_Order, l_Order) )
        if(!SendMsg(l_Order))
        {
            CString Msg("AFS:PE:Error in SendMsg.");
            throw Msg;
        }
	
    return TRUE;
}

BOOL CLAVAFixSession::OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder)
{

	//a_FixOrder.SecurityType("CS");

	if(a_FixOrder.Side() != FIX_Buy && a_FixOrder.Side() != FIX_Sell && a_FixOrder.Side() != FIX_SellShort && a_FixOrder.Side() != FIX_SellShortExempt)
	{
		CString Msg("AFS:OEO:LAVA supports Buy/Sell/Short/ShortExempt sides only.");
		throw Msg;
	}

	//a_FixOrder.Account(a_Order.ClearingId() + "-" + a_Order.ClearingAccount());
	a_FixOrder.Account(a_Order.ClearingAccount());

	// a_FixOrder.ExecBroker(m_execBroker.c_str());

	a_FixOrder.TransactTime((LPCTSTR)GetCurrentTimeStr());

	switch(a_Order.Capacity())
	{
	case COrder::Agency://Customer
		a_FixOrder.Rule80A('A');
		break;
	case COrder::Principal://Firm
		a_FixOrder.Rule80A('P');
		break;
	default:
		//unknown type
		CString Msg;
		Msg.Format("Unknown Capacity code %c in order %s with OrderID %s", a_Order.Capacity(),a_Order.Tag(),a_Order.OrderTag());
		((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":AFS:OEO:",Msg);
		throw LPCTSTR(Msg);
	}//	switch(a_Order.Capacity())

	if (a_Order.TIF() == COrder::GoodTillDate)
	{
		int year, month, day;

		year	= atoi(a_Order.ExpTime().Mid(0, 4));
		month	= atoi(a_Order.ExpTime().Mid(4,2));
		day		= atoi(a_Order.ExpTime().Mid(6,2));

		CTime t(year, month, day, 0 , 0, 0);

		CString ExpTime;
		ExpTime.Format("%s-04:59:00", t.Format("%Y%m%d"));
		a_FixOrder.ExpireTime((LPCTSTR)ExpTime);
	}

	a_FixOrder.FuttSettDate("SPOT");
	a_FixOrder.Symbol(a_Order.Symbol());

	
	return TRUE;
}

BOOL CLAVAFixSession::ProcessCancel(CCancel& a_Cancel)
{

	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing cancel until fix session is corrected.");
		a_Cancel.SetInstructions("Fix session out of sequence:Can't send cancel.");
		return FALSE;
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

	// { WXU 
	//- l_Cancel.SenderSubID(a_Cancel.SourceId());
	TString senderSubId = SenderSubId();
	if ( senderSubId.length() > 0 )
		l_Cancel.SenderSubID((const char*)senderSubId.c_str());
	else
		l_Cancel.SenderSubID(a_Cancel.SourceId());
	// }
	l_Cancel.OrderID(a_Cancel.ExchangeTag());
	l_Cancel.OrderQty(a_Cancel.Shares());

	if(CFixMsg::GetVersionNum() < 4.099)
	{
		l_Cancel.CxlType(FIX_FullCxl);
	}

	if(CancelOverride(a_Cancel, l_Cancel))
		if(!SendMsg(l_Cancel))
		{
			CString Msg("AFS:PC:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CLAVAFixSession::CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl)
{
	a_FixCxl.TransactTime(GetCurrentTimeStr());
	//a_FixCxl.ExecBroker(m_execBroker.c_str());

	//char buffer[20];
	//MapOnBehalfOfCompID::iterator theIterator;

	//theIterator = m_mapOnBehalfOfCompID.find(_itoa(a_Cxl.Exchange(),buffer,10));
	//if(theIterator != m_mapOnBehalfOfCompID.end())
	//	a_FixCxl.OnBehalfOfCompID(((*theIterator).second).c_str());
	//else
	//	a_FixCxl.OnBehalfOfCompID(m_defaultOnBehalfOfCompID.c_str());
	
	a_FixCxl.TargetSubID( COrder::TIFString(a_Cxl.TIF()) );

	return TRUE;
}

BOOL CLAVAFixSession::ProcessReplace(CReplace& a_Replace)
{
	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing replace until fix session is corrected.");
		a_Replace.SetInstructions("Fix session out of sequence:Can't replace order.");
		return FALSE;
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

	TString senderSubId = SenderSubId();
	if ( senderSubId.length() > 0 )
		l_Replace.SenderSubID((const char*)senderSubId.c_str());
	else if (strcmp(a_Replace.SourceId(), "") != 0)
		l_Replace.SenderSubID(a_Replace.SourceId());

	const char* shortLender = a_Replace.ShortLender();
	if (shortLender && *shortLender)
		l_Replace.GuaranteedGetCustomField((EFixFieldID)FIX_ShortLender)->Value(shortLender);
	// }

	if (a_Replace.ExecInst() != COrder::LastExecInst)
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Replace.ExecInst());
		l_Replace.ExecInst(ExecInst);
	}

	l_Replace.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Replace.TIF()]);

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
			CString Msg("AFS:PR:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CLAVAFixSession::ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace)
{
	//a_FixReplace.SecurityType("CS");

	if(a_FixReplace.Side() != FIX_Buy && a_FixReplace.Side() != FIX_Sell && a_FixReplace.Side() != FIX_SellShort)
	{
		CString Msg("AFS:RO:LAVA supports Buy/Sell/Short sides only.");
		throw Msg;
	}

	switch(a_Replace.Capacity())
	{
	case COrder::Agency://Customer
		a_FixReplace.Rule80A('A');
		break;
	case COrder::Principal://Firm
		a_FixReplace.Rule80A('P');
		break;
	default:
		//unknown type
		CString Msg;
		Msg.Format("Unknown Capacity code %c in order %s", a_Replace.Capacity(),a_Replace.OrderTag());
		((CExchangeApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":AFS:RO:",Msg);
		throw LPCTSTR(Msg);
	}//	switch(a_Replace.Capacity())

	//a_FixReplace.Account(a_Replace.ClearingId() + "-" + a_Replace.ClearingAccount());
	//a_FixReplace.Account(a_Replace.ClearingAccount());
	a_FixReplace.TimeInForce((EFixTimeInForce) Translator().Lookup(Translator().m_ToExchangeTIF,a_Replace.TIF()));
	a_FixReplace.TransactTime((LPCTSTR)GetCurrentTimeStr());
	//a_FixReplace.ExecBroker(m_execBroker.c_str());

	if (a_Replace.TIF() == COrder::GoodTillDate)
	{
		int year, month, day;

		year	= atoi(a_Replace.ExpTime().Mid(0, 4));
		month	= atoi(a_Replace.ExpTime().Mid(4,2));
		day		= atoi(a_Replace.ExpTime().Mid(6,2));

		CTime t(year, month, day, 0 , 0, 0);

		CString ExpTime;
		ExpTime.Format("%s-04:59:00", t.Format("%Y%m%d"));
		a_FixReplace.ExpireTime((LPCTSTR)ExpTime);
	}

	//char buffer[20];
	//MapOnBehalfOfCompID::iterator theIterator;

	//theIterator = m_mapOnBehalfOfCompID.find(_itoa(a_Replace.Exchange(),buffer,10));
	//if(theIterator != m_mapOnBehalfOfCompID.end())
	//	a_FixReplace.OnBehalfOfCompID(((*theIterator).second).c_str());
	//else
	//	a_FixReplace.OnBehalfOfCompID(m_defaultOnBehalfOfCompID.c_str());

	return TRUE;
}

void CLAVAFixSession::Process(const CFixExecution& a_Exec)
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
		l_MsgStr.Format("Exchange order ID: %s\nAffected Execution Ref ID: %s\nSide: %s\nOriginal Shares: %d\nSymbol: %s\nOrder Type: %s\nOriginal Price: %f\nTime-in-Force: %s\nLast Shares(busted): %d\nLast Price: %f\nBust Time (GMT): %s",
						a_Exec.ClOrdID().c_str(),
						a_Exec.ExecRefID().c_str(),
						a_Exec.Side() == 1?"BUY":"SELL",
						a_Exec.OrderQty(),
						a_Exec.Symbol().c_str(),
						l_strOrdType,
						a_Exec.Price(),
						l_strTIF,
						a_Exec.LastShares(),//busted trade
						a_Exec.LastPx(),
						a_Exec.SendingTime().c_str());
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"LAVA:Busted Stock Trade", l_MsgStr);
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
		l_MsgStr.Format("Exchange order ID: %s\nAffected Execution Ref ID: %s\nSide: %s\nOriginal Shares: %d\nSymbol: %s\nOrder Type: %s\nOriginal Price: %f\nTime-in-Force: %s\nLast Shares: %d\nLast Price: %f\nCorrection Time (GMT): %s",
						a_Exec.ClOrdID().c_str(),
						a_Exec.ExecRefID().c_str(),
						a_Exec.Side() == 1?"BUY":"SELL",
						a_Exec.OrderQty(),
						a_Exec.Symbol().c_str(),
						l_strOrdType,
						a_Exec.Price(),
						l_strTIF,
						a_Exec.LastShares(),
						a_Exec.LastPx(),
						a_Exec.SendingTime().c_str());
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"LAVA:Stock Trade Correction", l_MsgStr);
		return;
	}

	EFixOrdStatus l_OrdStatus = a_Exec.OrdStatus();

	CExecution l_Exec;
	l_Exec.SetReport((char) a_Exec.ReportToExch());
	l_Exec.SetOrderTag(a_Exec.ClOrdID().c_str());
	l_Exec.SetExchangeTag(a_Exec.OrderID().c_str());
	l_Exec.SetOriginalTag(a_Exec.OrigClOrdID().c_str());
	//l_Exec.SetExecBroker(a_Exec.ExecBroker().c_str());
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

BOOL CLAVAFixSession::ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec)
{
	if(Exec.Status() == COrder::Rejected )
	{
		CString reason;
		reason.Format("Reject Reason:%d", a_Exec.OrdRejReason());
		Exec.SetReason(reason);
	}

	Exec.SetTradeTag(a_Exec.ExecID().c_str());

	if(Exec.ExecType() == COrder::ReplacePending )
	{
			CExchangeHandler* l_pHandler =(CExchangeHandler*) &( (CMessageApp *)AfxGetApp() )->Handler();
			CLavaMemTagSet* l_MemTag = (CLavaMemTagSet*)l_pHandler->GetMemTag();
			
			//To pre-fill in the rest of the (partial cancel) replace info as so that succeeding blocks of code have data to place correctfully.
			l_MemTag->GetTags(MAP_ORDER_TAG_QUERY, Exec.OriginalTag());
			l_MemTag->SetReplaceShares(Exec.Leaves());
			l_MemTag->SetReplacePrice(Exec.Price());
			l_MemTag->SetReplaceTag(Exec.OrderTag());
			l_MemTag->SetLeavesShares(Exec.Leaves());
			l_MemTag->SetTimeInForce(Exec.TimeInForce());
			l_MemTag->SetReplaceStopPrice(Exec.StopPrice());
			l_MemTag->SetReplaceOrderType(Exec.OrderType());
			if(!Exec.Instructions().IsEmpty())
				l_MemTag->SetReplaceExecInst(Exec.Instructions().GetAt(0));
	}
	return TRUE;
}


void CLAVAFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order)
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


void CLAVAFixSession::ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace)
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
