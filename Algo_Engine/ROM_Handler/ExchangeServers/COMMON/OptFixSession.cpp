#include "stdafx.h"
#include "optFixSession.h"
#include "exchangehandler.h"

COptFixSession::COptFixSession()
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
}


COptFixSession::~COptFixSession()
{

}

void COptFixSession::ProcessEmail(CString& a_Subject, CString& a_Text)
{
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();		
	CString l_Subject;
	l_Subject.Format("Fix Email from %s: %s", pApp->Name(), a_Subject);
	pApp->ProcessErrorEx(MODE_ERR_BUSINESS,l_Subject, a_Text);
}

BOOL COptFixSession::OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder)
{
	try
	{
		a_FixOrder.SecurityType("OPT");
		a_FixOrder.MaturityMonthYear((LPCTSTR) a_Order.ExpDate());

		a_FixOrder.Account(a_Order.ClearingId() + "-" + a_Order.ClearingAccount());

		if(a_Order.CallOrPut() == 'P')
			a_FixOrder.PutOrCall(0);
		else if(a_Order.CallOrPut() == 'C')
			a_FixOrder.PutOrCall(1);
		else
		{
			CString Msg("OFS:OEO:PutOrCall parameter missing.");
			throw Msg;
		}

		a_FixOrder.StrikePrice(a_Order.StrikePrice());

	}
	catch(...)
	{
		CString Msg("OFS:OEO:Exception in processing.");
		throw Msg;
	}

	return TRUE;
}

BOOL COptFixSession::CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl)
{
	try
	{
		a_FixCxl.SecurityType("OPT");
		a_FixCxl.MaturityMonthYear((LPCTSTR) a_Cxl.ExpDate());

		if(a_Cxl.CallOrPut() == 'P')
			a_FixCxl.PutOrCall(0);
		else if(a_Cxl.CallOrPut() == 'C')
			a_FixCxl.PutOrCall(1);
		else
		{
			CString Msg("OFS:CO:PutOrCall parameter missing.");
			throw Msg;
		}

		a_FixCxl.StrikePrice(a_Cxl.StrikePrice());
	}
	catch(...)
	{
		CString Msg("OFS:CO:Exception in cancellation.");
		throw Msg;
	}

	return TRUE;
}

BOOL COptFixSession::ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace)
{
	try
	{
		a_FixReplace.SecurityType("OPT");
		a_FixReplace.MaturityMonthYear((LPCTSTR) a_Replace.ExpDate());

		//not all exchanges may except TIF in replaces!!!!
		a_FixReplace.TimeInForce((EFixTimeInForce) Translator().Lookup(Translator().m_ToExchangeTIF,a_Replace.TIF()));

		if(a_Replace.CallOrPut() == 'P')
			a_FixReplace.PutOrCall(0);
		else if(a_Replace.CallOrPut() == 'C')
			a_FixReplace.PutOrCall(1);
		else
		{
			CString Msg("OFS:RO:PutOrCall parameter missing.");
			throw Msg;
		}

		a_FixReplace.StrikePrice(a_Replace.StrikePrice());
	}
	catch(...)
	{
		CString Msg("OFS:RO:Exception in replacing.");
		throw Msg;
	}

	return TRUE;
}

BOOL COptFixSession::ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec)
{
	//IGNORE DONEFORDAY ON GTC ORDERS
	if((COrder::DoneForDay == Exec.ExecType()) && (COrder::GTC == Exec.TimeInForce()))
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
		l_MsgStr.Format("Exchange order ID: %s\nAffected Execution Ref ID: %s\nSide: %s\nOriginal Shares: %d\nSymbol: %s\nYearMonth: %s\nStrike Price: %f\nPutOrCall: %s\nOrder Type: %s\nOriginal Price: %f\nTime-in-Force: %s\nSending Time (GMT): %s\nCustomer: %s",
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
						a_Exec.SendingTime().c_str(),
						a_Exec.TargetSubID().c_str());

		l_MsgStr += "\nGTC Option Order on AMEX thru ABNAMRO received DoneForDay status, system ignored message according to rule, please confirm with exchange about status of order.";
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER | MODE_ERR_BUSINESS,"ABNAMRO:AMEX:DoneForDay for GTC Option order, please check!!", l_MsgStr);
		
		return FALSE;
	}

	return TRUE;
}

const char const * COptFixSession::GetLinkString(int a_HomeEx)
{
	switch(a_HomeEx)
	{
	case COrder::AMEX:
	case COrder::AMEXOPTION:
		return FromAMEX;
		break;
	case COrder::CBOE:
		return FromCBOE;
		break;
	case COrder::ISE:
		return FromISE;
		break;
	case COrder::PCX:
		return FromPCX;
		break;
	case COrder::PHLX:
		return FromPHLX;
		break;
	}
	CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();
	CString Msg;
	Msg.Format("Unknown MM Home exchange %d ", a_HomeEx);
	pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,((CExchangeApp *)AfxGetApp())->Name() +":OptFixSession:GetLinkString:", Msg);

	return NULL;
}
