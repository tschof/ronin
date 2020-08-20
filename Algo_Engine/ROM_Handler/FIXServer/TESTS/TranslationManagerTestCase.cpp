#include "..\stdafx.h"
#include "TranslationManagerTestCase.h"
#include "..\Order.h"
#include "Values.h"
#include "FIX42_Messages.h"
#include "..\OMMessages.h"

TranslationManagerTestCase::TranslationManagerTestCase()
{
	add(&m_testInsert);
	add(&m_testFindByTag);
	add(&m_testErase);
	add(&m_testTranslateNewOrderSingleToEnterOrder);
	add(&m_testTranslateStatusRejectedToExecutionReport);
	add(&m_testTranslateStatusOpenToExecutionReport);
	add(&m_testTranslateStatusFilledToExecutionReport);
	add(&m_testTranslateStatusPartiallyFilledToExecutionReport);
	add(&m_testTranslateStatusFilledAfterPartialToExecutionReport);
	add(&m_testTranslateOrderCancelRequestToCancelOrder);
	add(&m_testTranslateOrderCancelReplacePriceToReplaceOrder);
	add(&m_testTranslateOrderCancelReplaceQuantityToReplaceOrder);
	add(&m_testTranslateStatusFilledAfterReplaceToExecutionReport);
	add(&m_testTranslateStatusToOrderCancelReject);
	add(&m_testTranslateStatusPendingCancelToExecutionReport);
	add(&m_testTranslateStatusFilledAfterPendingCancelToExecutionReport);
	add(&m_testTranslateStatusPartiallyFilledAfterPendingCancelToExecutionReport);
	add(&m_testTranslateStatusCancelledToExecutionReport);
	add(&m_testTranslateStatusReplacedToExecutionReport);
}

bool TranslationManagerTest::onSetup( TranslationManager*& pObject )
{
	pObject = new TranslationManager();
	m_omOrders = &(pObject->m_omOrders);
	m_fixOrders = &(pObject->m_fixOrders);
	return true;
}

void TranslationManagerTest::onTeardown( TranslationManager* pObject )
{
	delete pObject;
}

void TranslationManagerTestCase::TestInsert::onRun( TranslationManager& tm )
{
	assert( m_omOrders->empty() );
	assert( m_fixOrders->empty() );
	
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100) );
	
	assert( m_omOrders->size() == 1 );
	assert( m_fixOrders->size() == 1 );
	assert( m_omOrders->find("FIX.4.2 CL TD 1") != m_omOrders->end() );
	assert( 
		m_fixOrders->find( 
			std::make_pair( sessionId, std::string("1") ) 
		) != m_fixOrders->end() 
	);
}

void TranslationManagerTestCase::TestFindByTag::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100) );
	
	assert( tm.FindByTag("FIX.4.2 CL TD 1") != NULL );
	assert( tm.FindByTag("FIX.4.2 CL TD 2") == NULL );
}

void TranslationManagerTestCase::TestErase::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100) );
	tm.Insert( Order("CL", "FIX.4.2 CL TD 2", sessionId, "2", 125) );


	assert( tm.Erase("FIX.4.2 CL TD 1") );

	assert( m_omOrders->size() == 1 );
	assert( m_omOrders->find("FIX.4.2 CL TD 1") == m_omOrders->end() );
}

void TranslationManagerTestCase::TestTranslateNewOrderSingleToEnterOrder::onRun( TranslationManager& tm )
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

	OM::Message message = tm.Translate(single);

	assert( message.GetCommand() == 'E' );
	
	OM::EnterOrder enterOrder(message);
	assert( enterOrder.GetTrader() == "CL" );
	assert( enterOrder.GetTag() == "FIX.4.2 CL TD 1" );

	Order* pOrder = tm.FindByTag("FIX.4.2 CL TD 1");
	assert( pOrder != NULL );
	assert( pOrder->GetTrader() == "CL" );
	assert( pOrder->GetTag() == "FIX.4.2 CL TD 1" );
	assert( pOrder->GetSessionID().getBeginString() == "FIX.4.2" );
	assert( pOrder->GetSessionID().getSenderCompID() == "TD" );
	assert( pOrder->GetSessionID().getTargetCompID() == "CL" );
	assert( pOrder->GetClOrdID() == "1" );
}

void TranslationManagerTestCase::TestTranslateStatusRejectedToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1", 100) );
	
	OM::Status status;

	status.SetTrader("MM5");
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
	
	status.SetTradeFor("MM5");
	status.SetText("Unable To Send To Exchange");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);

	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );
}

void TranslationManagerTestCase::TestTranslateStatusOpenToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100) );
	
	OM::Status status;

	status.SetTrader("MM5");
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
	
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);
	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );
	
	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::AvgPx()) == 0.0 );
	assert( execRpt.get(FIX::CumQty()) == 0 );
	assert( execRpt.get(FIX::LeavesQty()) == 100 );

	const Order* pOrder = tm.FindByTag("FIX.4.2 CL TD 1");
}

void TranslationManagerTestCase::TestTranslateStatusFilledToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100) );
	
	OM::Status status;

	status.SetTrader("MM5");
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
	status.SetExecTag("11");
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate( status );
	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::AvgPx()) == 50.0 );
	assert( execRpt.get(FIX::CumQty()) == 100 );
	assert( execRpt.get(FIX::LeavesQty()) == 0 );

	const Order* pOrder = tm.FindByTag("FIX.4.2 CL TD 1");
}

void TranslationManagerTestCase::TestTranslateStatusPartiallyFilledToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100) );

	OM::Status status;

	status.SetTrader("MM5");
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
	status.SetExecTag("12");
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);
	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::AvgPx()) == 50.0 );
	assert( execRpt.get(FIX::CumQty()) == 75 );
	assert( execRpt.get(FIX::LeavesQty()) == 25 );
}

void TranslationManagerTestCase::TestTranslateStatusFilledAfterPartialToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100, FIX::OrdStatus_PENDING_NEW, 50.0, 75, 25) );

	OM::Status status;

	status.SetTrader("MM5");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(25);
	status.SetPrice(51.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_Filled);
	status.SetFirmTag("MM5-20020603-2");
	status.SetExecTag("12");
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);
	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::AvgPx()) == 50.25 );
	assert( execRpt.get(FIX::CumQty()) == 100 );
	assert( execRpt.get(FIX::LeavesQty()) == 0 );
}

void TranslationManagerTestCase::TestTranslateOrderCancelRequestToCancelOrder::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100) );

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
	
	OM::Message message = tm.Translate(cancel);

	assert( message.GetCommand() == 'C' );
	
	OM::CancelOrder cancelOrder(message);
	
	assert( cancelOrder.GetTrader() == "CL" );
	assert( cancelOrder.GetTag() == "FIX.4.2 CL TD 1" );

	Order* pOrder = tm.FindBySessionIDAndClOrdID(sessionId, "1");
	assert( pOrder != NULL);
	assert( pOrder->GetClOrdID() == "2" );
	assert( pOrder->GetOrigClOrdID() == "1" );
}

void TranslationManagerTestCase::TestTranslateStatusToOrderCancelReject::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_PENDING_CANCEL, 0.0, 0, 100, "1") );
	
	OM::Status status;
	
	status.SetTrader("MM5");
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
	
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);

	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_OrderCancelReject );
	
	FIX42::OrderCancelReject cancelRej(message);
	assert( cancelRej.get(FIX::ClOrdID()) == "2" );
	assert( cancelRej.get(FIX::OrigClOrdID()) == "1" );
}

void TranslationManagerTestCase::TestTranslateStatusPendingCancelToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_PENDING_NEW, 0.0, 0, 100, "1") );

	OM::Status status;

	status.SetTrader("MM5");
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
	
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);

	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::ClOrdID()) == "2" );
	assert( execRpt.get(FIX::OrigClOrdID()) == "1" );
	assert( execRpt.get(FIX::OrdStatus()) == FIX::OrdStatus_PENDING_CANCEL );
}

void TranslationManagerTestCase::TestTranslateStatusFilledAfterPendingCancelToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_PENDING_CANCEL, 0.0, 0, 100, "1");
	tm.Insert( order );

	OM::Status status;

	status.SetTrader("MM5");
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
	status.SetExecTag("11");
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate( status );
	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::ExecType()) == FIX::ExecType_FILL );
	assert( execRpt.get(FIX::OrdStatus()) == FIX::OrdStatus_PENDING_CANCEL );
	assert( execRpt.get(FIX::ClOrdID()) == "1" );
	assert( !execRpt.isSetField(FIX::OrigClOrdID()) );
	assert( execRpt.get(FIX::AvgPx()) == 50.0 );
	assert( execRpt.get(FIX::CumQty()) == 100 );
	assert( execRpt.get(FIX::LeavesQty()) == 0 );
	
}

void TranslationManagerTestCase::TestTranslateStatusPartiallyFilledAfterPendingCancelToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_PENDING_CANCEL, 0.0, 0, 100, "1");
	tm.Insert( order );

	OM::Status status;

	status.SetTrader("MM5");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(50);
	status.SetPrice(50.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_PartiallyFilled);
	status.SetFirmTag("MM5-20020603-2");
	status.SetExecTag("11");
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate( status );
	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::ExecType()) == FIX::ExecType_PARTIAL_FILL );
	assert( execRpt.get(FIX::OrdStatus()) == FIX::OrdStatus_PENDING_CANCEL );
	assert( execRpt.get(FIX::ClOrdID()) == "1" );
	assert( !execRpt.isSetField(FIX::OrigClOrdID()) );
	assert( execRpt.get(FIX::AvgPx()) == 50.0 );
	assert( execRpt.get(FIX::CumQty()) == 50 );
	assert( execRpt.get(FIX::LeavesQty()) == 50 );
	
}

void TranslationManagerTestCase::TestTranslateStatusCancelledToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_PENDING_NEW, 0.0, 0, 100, "1") );

	OM::Status status;

	status.SetTrader("MM5");
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
	
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);

	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::ClOrdID()) == "2" );
	assert( execRpt.get(FIX::OrigClOrdID()) == "1" );	
	assert( tm.FindBySessionIDAndClOrdID(sessionId, "2") != NULL );
}

void TranslationManagerTestCase::TestTranslateOrderCancelReplacePriceToReplaceOrder::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100) );
	
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

	OM::Message message = tm.Translate(replace);

	assert( message.GetCommand() == 'R' );
	
	OM::ReplaceOrder replaceOrder(message);
	assert( replaceOrder.GetTrader() == "CL" );
	assert( replaceOrder.GetTag() == "FIX.4.2 CL TD 1" );
	
	Order* pOrder = tm.FindByTag("FIX.4.2 CL TD 1");
	assert( pOrder != NULL );
	assert( pOrder->GetClOrdID() == "2" );
	assert( pOrder->GetOrigClOrdID() == "1" );

}

void TranslationManagerTestCase::TestTranslateOrderCancelReplaceQuantityToReplaceOrder::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100, FIX::OrdStatus_PARTIALLY_FILLED, 50.0, 20, 80) );
	
	FIX42::OrderCancelReplaceRequest replace;

	replace.getHeader().set(FIX::BeginString("FIX.4.2"));
	replace.getHeader().set(FIX::SenderCompID("CL"));
	replace.getHeader().set(FIX::TargetCompID("TD"));

	replace.set( FIX::ClOrdID("2") );
	replace.set( FIX::OrigClOrdID("1") );
	replace.set( FIX::Side(FIX::Side_BUY) );
	replace.set( FIX::Symbol("MSFT") );
	replace.set( FIX::OrdType(FIX::OrdType_LIMIT) );
	replace.set( FIX::Price(50.0) );
	replace.set( FIX::OrderQty(80) );

	time_t t;
	time(&t);
	replace.set( FIX::TransactTime(t) );

	OM::Message message = tm.Translate(replace);

	assert( message.GetCommand() == 'R' );
	
	OM::ReplaceOrder replaceOrder(message);
	assert( replaceOrder.GetTrader() == "CL" );
	assert( replaceOrder.GetTag() == "FIX.4.2 CL TD 1" );
	assert( replaceOrder.GetShares() == 60 );


	Order* pOrder = tm.FindByTag("FIX.4.2 CL TD 1");
	assert( pOrder != NULL );
	assert( pOrder->GetClOrdID() == "2" );
	assert( pOrder->GetOrigClOrdID() == "1" );
}

void TranslationManagerTestCase::TestTranslateStatusFilledAfterReplaceToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	
	Order order("CL", "FIX.4.2 CL TD 1", sessionId, "2", 100, FIX::OrdStatus_PENDING_REPLACE, 0.0, 0, 100, "1");
	tm.Insert( order );

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
	status.SetExecTag("11");
	status.SetTradeFor("CL");
	status.SetOwner("CL");

	FIX42::Message message = tm.Translate( status );
	assert( message.getHeader().get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::ExecType()) == FIX::ExecType_FILL );
	assert( execRpt.get(FIX::OrdStatus()) == FIX::OrdStatus_PENDING_REPLACE );
	assert( execRpt.get(FIX::ClOrdID()) == "1" );
	assert( !execRpt.isSetField(FIX::OrigClOrdID()) );
	assert( execRpt.get(FIX::AvgPx()) == 50.0 );
	assert( execRpt.get(FIX::CumQty()) == 100 );
	assert( execRpt.get(FIX::LeavesQty()) == 0 );
}

void TranslationManagerTestCase::TestTranslateStatusReplacedToExecutionReport::onRun( TranslationManager& tm )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	tm.Insert( Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100) );
	
	OM::Status status;
	
	status.SetTrader("MM5");
	status.SetTag("FIX.4.2 CL TD 1");
	status.SetSide(OM::Side_Buy);
	status.SetSymbol("MSFT");
	status.SetShares(80);
	status.SetPrice(0.00);
	status.SetType(OM::Type_Market);
	status.SetTIF(OM::TIF_Day);
	status.SetCapacity(OM::Capacity_Agent);
	status.SetClearingID("0551");
	status.SetClearingAccount("123456");
//	status.SetExchange(OM::Exchange_Archipelago);
	status.SetStatus(OM::Status_Replaced);
	status.SetFirmTag("MM5-20020603-2");
	
	status.SetTradeFor("MM5");
	status.SetOwner("MM5");

	FIX42::Message message = tm.Translate(status);

	assert( message.get(FIX::MsgType()) == FIX::MsgType_ExecutionReport );

	FIX42::ExecutionReport execRpt(message);
	assert( execRpt.get(FIX::OrderQty()) == 80 );
}
