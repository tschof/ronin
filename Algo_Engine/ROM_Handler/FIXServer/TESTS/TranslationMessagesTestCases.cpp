#include "..\stdafx.h"
#include "TranslationMessagesTestCases.h"
#include "FIX42_Messages.h"
#include "Values.h"
#include "..\Order.h"

EnterOrderFromFIXTestCase::EnterOrderFromFIXTestCase()
{
	add( &m_testBuy100MSFTAtMarket );
	add( &m_testSell1000MSFTAt50 );
}

bool EnterOrderFromFIXTestCase::TestBuy100MSFTAtMarket::onSetup( EnterOrderFromFIX*& pObject )
{
	FIX42::NewOrderSingle single;
	single.getHeader().set(FIX::BeginString("FIX.4.2"));
	single.getHeader().set(FIX::SenderCompID("CL"));
	single.getHeader().set(FIX::TargetCompID("TD"));
	single.set(FIX::ClOrdID("1"));
	single.set(FIX::Side(FIX::Side_BUY));
	single.set(FIX::Symbol("MSFT"));
	single.set(FIX::OrderQty(100));
	single.set(FIX::OrdType(FIX::OrdType_MARKET));
	single.set(FIX::TimeInForce(FIX::TimeInForce_DAY));
	single.set(FIX::Rule80A(FIX::Rule80A_PRINCIPAL));
	single.set(FIX::Account("0551 12345 TD ABCD"));
	single.setField(10001, "0");
	pObject = new EnterOrderFromFIX(single, "TRDR", "FIX.4.2 CL TD 1");

	return true;
}

void EnterOrderFromFIXTestCase::TestBuy100MSFTAtMarket::onTeardown( EnterOrderFromFIX* pObject )
{
	delete pObject;
}

void EnterOrderFromFIXTestCase::TestBuy100MSFTAtMarket::onRun( EnterOrderFromFIX& message )
{
	assert( message.GetTrader() == "TRDR" );
	assert( message.GetTag() == "FIX.4.2 CL TD 1" );
	assert( message.GetSide() == OM::Side_Buy );
	assert( message.GetSymbol() == "MSFT" );
	assert( message.GetShares() == 100 );
	assert( message.GetType() == OM::Type_Market );
	assert( message.GetTIF() == 0 );
	assert( message.GetCapacity() == OM::Capacity_Principal );
	assert( message.GetClearingID() == "0551" );
	assert( message.GetClearingAccount() == "12345" );
	assert( message.GetFirm() == "TD" );
	assert( message.GetLocalAccount() == "ABCD" );
}

bool EnterOrderFromFIXTestCase::TestSell1000MSFTAt50::onSetup( EnterOrderFromFIX*& pObject )
{
	FIX42::NewOrderSingle single;
	single.getHeader().set(FIX::BeginString("FIX.4.2"));
	single.getHeader().set(FIX::SenderCompID("CL"));
	single.getHeader().set(FIX::TargetCompID("TD"));
	single.set(FIX::ClOrdID("1"));
	single.set(FIX::Side(FIX::Side_SELL));
	single.set(FIX::Symbol("MSFT"));
	single.set(FIX::OrderQty(1000));
	single.set(FIX::OrdType(FIX::OrdType_LIMIT));
	single.set(FIX::Price(50.0));
	single.set(FIX::TimeInForce(FIX::TimeInForce_DAY));
	single.set(FIX::Rule80A(FIX::Rule80A_PRINCIPAL));
	single.set(FIX::Account("0551 12345 TD ABCD"));
	single.setField(10001, "0");
	pObject = new EnterOrderFromFIX(single, "TRDR", "FIX.4.2 CL TD 1");

	return true;
}

void EnterOrderFromFIXTestCase::TestSell1000MSFTAt50::onTeardown( EnterOrderFromFIX* pObject )
{
	delete pObject;
}

void EnterOrderFromFIXTestCase::TestSell1000MSFTAt50::onRun( EnterOrderFromFIX& message )
{
	assert( message.GetTrader() == "TRDR" );
	assert( message.GetTag() == "FIX.4.2 CL TD 1" );
	assert( message.GetSide() == OM::Side_Sell );
	assert( message.GetSymbol() == "MSFT" );
	assert( message.GetShares() == 1000 );
	assert( message.GetType() == OM::Type_Limit );
	assert( message.GetPrice() == 50.0 );
	assert( message.GetTIF() == OM::TIF_Day );
	assert( message.GetCapacity() == OM::Capacity_Principal );
	assert( message.GetFirm() == "TD" );
	assert( message.GetLocalAccount() == "ABCD" );
}

CancelOrderFromFIXTestCase::CancelOrderFromFIXTestCase()
{
	add(&m_testCancelSellMSFT);
}

bool CancelOrderFromFIXTestCase::TestCancelSellMSFT::onSetup( CancelOrderFromFIX*& pObject )
{
	FIX42::OrderCancelRequest cancel;

	cancel.getHeader().set(FIX::BeginString("FIX.4.2"));
	cancel.getHeader().set(FIX::SenderCompID("CL"));
	cancel.getHeader().set(FIX::TargetCompID("TD"));
	
	cancel.set( FIX::ClOrdID("2") );
	cancel.set( FIX::OrigClOrdID("1") );
	cancel.set( FIX::Side(FIX::Side_SELL) );
	cancel.set( FIX::Symbol("MSFT") );
	
	time_t t;
	time(&t);
	cancel.set( FIX::TransactTime(t) );

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_NEW, 0.0, 0, 0, "1");

	pObject = new CancelOrderFromFIX(cancel, order);
	return true;
}

void CancelOrderFromFIXTestCase::TestCancelSellMSFT::onTeardown( CancelOrderFromFIX* pObject )
{
	delete pObject;
}

void CancelOrderFromFIXTestCase::TestCancelSellMSFT::onRun( CancelOrderFromFIX& message )
{
	assert( message.GetTrader() == "CL" );
	assert( message.GetTag() == "FIX.4.2 CL TD 1" );
	assert( message.GetSide() == OM::Side_Sell );
	assert( message.GetSymbol() == "MSFT" );
}

ReplaceOrderFromFIXTestCase::ReplaceOrderFromFIXTestCase()
{
	add(&m_testReplaceBuy100MSFTAt50WithAt51);
}

bool ReplaceOrderFromFIXTestCase::TestReplaceBuy100MSFTAt50WithAt51::onSetup( ReplaceOrderFromFIX*& pObject )
{
	FIX42::OrderCancelReplaceRequest replace;
	
	replace.getHeader().set(FIX::BeginString("FIX.4.2"));
	replace.getHeader().set(FIX::SenderCompID("CL"));
	replace.getHeader().set(FIX::TargetCompID("TD"));

	replace.set( FIX::ClOrdID("2") );
	replace.set( FIX::OrigClOrdID("1") );
	replace.set( FIX::Side(FIX::Side_BUY) );
	replace.set( FIX::Symbol("MSFT") );
	replace.set( FIX::OrdType(FIX::OrdType_LIMIT) );
	replace.set( FIX::Price(51) );
	replace.set( FIX::OrderQty(100) );

	time_t t;
	time(&t);
	replace.set( FIX::TransactTime(t) );

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_NEW, 0.0, 0, 100, "1");

	pObject = new ReplaceOrderFromFIX(replace, order);
	
	return true;
}

void ReplaceOrderFromFIXTestCase::TestReplaceBuy100MSFTAt50WithAt51::onTeardown( ReplaceOrderFromFIX* pObject )
{
	delete pObject;
}

void ReplaceOrderFromFIXTestCase::TestReplaceBuy100MSFTAt50WithAt51::onRun( ReplaceOrderFromFIX& message )
{
	assert( message.GetTrader() == "CL" );
	assert( message.GetTag() == "FIX.4.2 CL TD 1" );
	assert( message.GetPrice() == 51.0 );
	assert( message.GetShares() == 100 );
}

ExecutionReportFromOMTestCase::ExecutionReportFromOMTestCase()
{
	add(&m_testOrderRejected);
	add(&m_testOrderAccepted);
	add(&m_testOrderFilled);
	add(&m_testOrderPartiallyFilled);
	add(&m_testOrderPendingCancel);
	add(&m_testOrderCancelled);
	add(&m_testOrderReplaced);
}

bool ExecutionReportFromOMTestCase::OrderRejected::onSetup( ExecutionReportFromOM*& pObject )
{
	OM::Status status;

	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(100);
	status.SetPrice(0.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_Rejected);
	status.SetFirmTag("MM5-20020603-2");
	
	status.SetTradeFor("CL");
	status.SetText("Unable To Send To Exchange");
	status.SetOwner("CL");

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100, FIX::OrdStatus_REJECTED, 0.0, 0, 0);
	
	pObject = new ExecutionReportFromOM(status, &order);
	return true;
}

void ExecutionReportFromOMTestCase::OrderRejected::onTeardown( ExecutionReportFromOM* pObject )
{
	delete pObject;
}

void ExecutionReportFromOMTestCase::OrderRejected::onRun( ExecutionReportFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "1" );
	assert( message.get(FIX::ExecID()) == "0" );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
	assert( message.get(FIX::Side()) == FIX::Side_BUY );
	assert( message.get(FIX::Symbol()) == "MSFT" );
	assert( message.get(FIX::AvgPx()) == 0.0 );
	assert( message.get(FIX::CumQty()) == 0 );
	assert( message.get(FIX::LeavesQty()) == 0 );
	assert( message.get(FIX::ExecTransType()) == FIX::ExecTransType_NEW );
	assert( message.get(FIX::ExecType()) == FIX::ExecType_REJECTED );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_REJECTED );
	assert( message.get(FIX::Text()) == "Unable To Send To Exchange" );		
}

bool ExecutionReportFromOMTestCase::OrderAccepted::onSetup( ExecutionReportFromOM*& pObject )
{
	OM::Status status;

	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(100);
	status.SetPrice(0.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_Open);
	status.SetFirmTag("MM5-20020603-2");
	
	status.SetTradeFor("CL");
	status.SetOwner("CL");

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100, FIX::OrdStatus_NEW);

	pObject = new ExecutionReportFromOM(status, &order);
	return true;
}

void ExecutionReportFromOMTestCase::OrderAccepted::onTeardown( ExecutionReportFromOM* pObject )
{
	delete pObject;
}

void ExecutionReportFromOMTestCase::OrderAccepted::onRun( ExecutionReportFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "1" );
	assert( message.get(FIX::ExecID()) == "0" );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
	assert( message.get(FIX::Side()) == FIX::Side_BUY );
	assert( message.get(FIX::Symbol()) == "MSFT" );
	assert( message.get(FIX::AvgPx()) == 0.0 );
	assert( message.get(FIX::CumQty()) == 0 );
	assert( message.get(FIX::LeavesQty()) == 100 );
	assert( message.get(FIX::ExecTransType()) == FIX::ExecTransType_NEW );
	assert( message.get(FIX::ExecType()) == FIX::ExecType_NEW );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_NEW );
		
}

bool ExecutionReportFromOMTestCase::OrderFilled::onSetup( ExecutionReportFromOM*& pObject )
{
	OM::Status status;

	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(100);
	status.SetPrice(50.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_Filled);
	status.SetFirmTag("MM5-20020603-2");
	status.SetTradeFor("CL");
	status.SetOwner("CL");
	status.SetOMExecTag("11");

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100, FIX::OrdStatus_FILLED, 50.0, 100, 0);

	pObject = new ExecutionReportFromOM(status, &order);
	return true;
}

void ExecutionReportFromOMTestCase::OrderFilled::onTeardown( ExecutionReportFromOM* pObject )
{
	delete pObject;
}

void ExecutionReportFromOMTestCase::OrderFilled::onRun( ExecutionReportFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "1" );
	assert( message.get(FIX::ExecID()) == "11" );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
	assert( message.get(FIX::Side()) == FIX::Side_BUY );
	assert( message.get(FIX::Symbol()) == "MSFT" );
	assert( message.get(FIX::LastPx()) == 50.0 );
	assert( message.get(FIX::LastShares()) == 100 );
	assert( message.get(FIX::AvgPx()) == 50.0 );
	assert( message.get(FIX::CumQty()) == 100 );
	assert( message.get(FIX::LeavesQty()) == 0 );
	assert( message.get(FIX::ExecTransType()) == FIX::ExecTransType_NEW );
	assert( message.get(FIX::ExecType()) == FIX::ExecType_FILL );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_FILLED );

}

bool ExecutionReportFromOMTestCase::OrderPartiallyFilled::onSetup( ExecutionReportFromOM*& pObject )
{
	OM::Status status;

	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(75);
	status.SetPrice(50.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_PartiallyFilled);
	status.SetFirmTag("MM5-20020603-2");
	status.SetTradeFor("CL");
	status.SetOwner("CL");
	status.SetOMExecTag("12");

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100, FIX::OrdStatus_PARTIALLY_FILLED, 50.0, 75, 25);

	pObject = new ExecutionReportFromOM(status, &order);
	return true;
}

void ExecutionReportFromOMTestCase::OrderPartiallyFilled::onTeardown( ExecutionReportFromOM* pObject )
{
	delete pObject;
}

void ExecutionReportFromOMTestCase::OrderPartiallyFilled::onRun( ExecutionReportFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "1" );
	assert( message.get(FIX::ExecID()) == "12" );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
	assert( message.get(FIX::Side()) == FIX::Side_BUY );
	assert( message.get(FIX::Symbol()) == "MSFT" );
	assert( message.get(FIX::LastPx()) == 50.0 );
	assert( message.get(FIX::LastShares()) == 75 );
	assert( message.get(FIX::AvgPx()) == 50.0 );
	assert( message.get(FIX::CumQty()) == 75 );
	assert( message.get(FIX::LeavesQty()) == 25 );
	assert( message.get(FIX::ExecTransType()) == FIX::ExecTransType_NEW );
	assert( message.get(FIX::ExecType()) == FIX::ExecType_PARTIAL_FILL );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_PARTIALLY_FILLED );
		
}

bool ExecutionReportFromOMTestCase::OrderPendingCancel::onSetup( ExecutionReportFromOM*& pObject )
{
	OM::Status status;

	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(100);
	status.SetPrice(0.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_PendingCancel);
	status.SetFirmTag("MM5-20020603-2");
	
	status.SetTradeFor("CL");
	status.SetOwner("CL");

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_PENDING_CANCEL, 0.0, 0, 100, "1");

	pObject = new ExecutionReportFromOM(status, &order);
	return true;
}

void ExecutionReportFromOMTestCase::OrderPendingCancel::onTeardown( ExecutionReportFromOM* pObject )
{
	delete pObject;
}

void ExecutionReportFromOMTestCase::OrderPendingCancel::onRun( ExecutionReportFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "2" );
	assert( message.get(FIX::OrigClOrdID()) == "1" );
	assert( message.get(FIX::ExecID()) == "0" );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
	assert( message.get(FIX::Side()) == FIX::Side_BUY );
	assert( message.get(FIX::Symbol()) == "MSFT" );
	assert( message.get(FIX::AvgPx()) == 0.0 );
	assert( message.get(FIX::CumQty()) == 0 );
	assert( message.get(FIX::LeavesQty()) == 100 );
	assert( message.get(FIX::ExecTransType()) == FIX::ExecTransType_NEW );
	assert( message.get(FIX::ExecType()) == FIX::ExecType_PENDING_CANCEL );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_PENDING_CANCEL );
		
}

bool ExecutionReportFromOMTestCase::OrderCancelled::onSetup( ExecutionReportFromOM*& pObject )
{
	OM::Status status;

	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(100);
	status.SetPrice(0.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_Cancelled);
	status.SetFirmTag("MM5-20020603-2");
	
	status.SetTradeFor("CL");
	status.SetOwner("CL");

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_CANCELED, 0.0, 0, 0, "1");

	pObject = new ExecutionReportFromOM(status, &order);
	return true;
}

void ExecutionReportFromOMTestCase::OrderCancelled::onTeardown( ExecutionReportFromOM* pObject )
{
	delete pObject;
}

void ExecutionReportFromOMTestCase::OrderCancelled::onRun( ExecutionReportFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "2" );
	assert( message.get(FIX::OrigClOrdID()) == "1" );
	assert( message.get(FIX::ExecID()) == "0" );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
	assert( message.get(FIX::Side()) == FIX::Side_BUY );
	assert( message.get(FIX::Symbol()) == "MSFT" );
	assert( message.get(FIX::AvgPx()) == 0.0 );
	assert( message.get(FIX::CumQty()) == 0 );
	assert( message.get(FIX::LeavesQty()) == 0 );
	assert( message.get(FIX::ExecTransType()) == FIX::ExecTransType_NEW );
	assert( message.get(FIX::ExecType()) == FIX::ExecType_CANCELED );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_CANCELED );
		
}

bool ExecutionReportFromOMTestCase::OrderReplaced::onSetup( ExecutionReportFromOM*& pObject )
{
	OM::Status status;

	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(50);
	status.SetPrice(0.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_Replaced);
	status.SetFirmTag("MM5-20020603-2");
	
	status.SetTradeFor("CL");
	status.SetOwner("CL");
	
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_REPLACED, 0.0, 0, 0, "1");

	pObject = new ExecutionReportFromOM(status, &order);
	return true;
}

void ExecutionReportFromOMTestCase::OrderReplaced::onTeardown( ExecutionReportFromOM* pObject )
{
	delete pObject;
}

void ExecutionReportFromOMTestCase::OrderReplaced::onRun( ExecutionReportFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "2" );
	assert( message.get(FIX::OrigClOrdID()) == "1" );
	assert( message.get(FIX::ExecID()) == "0" );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
	assert( message.get(FIX::Side()) == FIX::Side_BUY );
	assert( message.get(FIX::Symbol()) == "MSFT" );
	assert( message.get(FIX::OrderQty()) == 50 );
	assert( message.get(FIX::AvgPx()) == 0.0 );
	assert( message.get(FIX::CumQty()) == 0 );
	assert( message.get(FIX::LeavesQty()) == 0 );
	assert( message.get(FIX::ExecTransType()) == FIX::ExecTransType_NEW );
	assert( message.get(FIX::ExecType()) == FIX::ExecType_REPLACE );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_REPLACED );
}

OrderCancelRejectFromOMTestCase::OrderCancelRejectFromOMTestCase()
{
	add(&m_testOrderCancelRejected);
}

bool OrderCancelRejectFromOMTestCase::OrderCancelRejected::onSetup( OrderCancelRejectFromOM*& pObject )
{
	OM::Status status;
	
	status.SetTrader("CL");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(100);
	status.SetPrice(0.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_CancelRejected);
	status.SetFirmTag("MM5-20020603-2");
	
	status.SetTradeFor("CL");
	status.SetOwner("CL");

	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));

	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_NEW, 0.0, 0, 100, "1");

	pObject = new OrderCancelRejectFromOM(status, order);
	return true;
}

void OrderCancelRejectFromOMTestCase::OrderCancelRejected::onTeardown( OrderCancelRejectFromOM* pObject )
{
	delete pObject;
}

void OrderCancelRejectFromOMTestCase::OrderCancelRejected::onRun( OrderCancelRejectFromOM& message )
{
	assert( message.getHeader().get(FIX::BeginString()) == "FIX.4.2" );
	assert( message.getHeader().get(FIX::SenderCompID()) == "TD" );
	assert( message.getHeader().get(FIX::TargetCompID()) == "CL" );
	assert( message.get(FIX::ClOrdID()) == "2" );
	assert( message.get(FIX::OrigClOrdID()) == "1" );
	assert( message.get(FIX::CxlRejResponseTo()) == '1' );
	assert( message.get(FIX::OrdStatus()) == FIX::OrdStatus_NEW );
	assert( message.get(FIX::OrderID()) == "MM5-20020603-2" );
}
