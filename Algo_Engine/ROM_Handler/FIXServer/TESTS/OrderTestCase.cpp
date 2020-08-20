#include "..\stdafx.h"
#include "OrderTestCase.h"
#include "..\OMMessages.h"

OrderTestCase::OrderTestCase()
{
	add( &m_testUpdateWithStatusOpen );
	add( &m_testUpdateWithStatusPendingCancelThenCancelReject );
	add( &m_testUpdateWithStatusPendingCancelThenFilledThenCancelReject );
}

bool OrderTestCase::OrderTest::onSetup( Order*& pObject )
{
	FIX::SessionID sessionId(std::string("FIX.4.2"), std::string("TD"), std::string("CL"));
	pObject = new Order("CL", "FIX.4.2 CL TD 1", sessionId, "1",100, FIX::OrdStatus_NEW);
	
	return true;
}

void OrderTestCase::OrderTest::onTeardown( Order* pObject )
{
	delete pObject;
}

void OrderTestCase::TestUpdateWithStatusOpen::onRun( Order& order )
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

	order.Update(status);

	assert( order.GetOrdStatus() == FIX::OrdStatus_NEW );
}

void OrderTestCase::TestUpdateWithStatusPendingCancelThenCancelReject::onRun( Order& order )
{
	OM::Status status1;

	status1.SetTrader("CL");
	status1.SetTag("FIX.4.2 CL TD 1");
	status1.SetSide(OM::Side_Buy);
	status1.SetSymbol("MSFT");
	status1.SetShares(100);
	status1.SetPrice(0.00);
	status1.SetType(OM::Type_Market);
	status1.SetTIF(OM::TIF_Day);
	status1.SetCapacity(OM::Capacity_Agent);
	status1.SetClearingID("0551");
	status1.SetClearingAccount("123456");
//	status1.SetExchange(OM::Exchange_Archipelago);
	status1.SetStatus(OM::Status_PendingCancel);
	status1.SetFirmTag("MM5-20020603-2");
	
	status1.SetTradeFor("CL");
	status1.SetOwner("CL");

	order.Update(status1);

	assert( order.GetOrdStatus() == FIX::OrdStatus_PENDING_CANCEL );

	OM::Status status2;
	
	status2.SetTrader("CL");
	status2.SetTag("FIX.4.2 CL TD 1");
	status2.SetSide(OM::Side_Buy);
	status2.SetSymbol("MSFT");
	status2.SetShares(100);
	status2.SetPrice(0.00);
	status2.SetType(OM::Type_Market);
	status2.SetTIF(OM::TIF_Day);
	status2.SetCapacity(OM::Capacity_Agent);
	status2.SetClearingID("0551");
	status2.SetClearingAccount("123456");
//	status2.SetExchange(OM::Exchange_Archipelago);
	status2.SetStatus(OM::Status_CancelRejected);
	status2.SetFirmTag("MM5-20020603-2");
	
	status2.SetTradeFor("CL");
	status2.SetOwner("CL");

	order.Update(status2);

	assert( order.GetOrdStatus() == FIX::OrdStatus_NEW );
}

void OrderTestCase::TestUpdateWithStatusPendingCancelThenFilledThenCancelReject::onRun( Order& order )
{
	OM::Status status1;

	status1.SetTrader("CL");
	status1.SetTag("FIX.4.2 CL TD 1");
	status1.SetSide(OM::Side_Buy);
	status1.SetSymbol("MSFT");
	status1.SetShares(100);
	status1.SetPrice(0.00);
	status1.SetType(OM::Type_Market);
	status1.SetTIF(OM::TIF_Day);
	status1.SetCapacity(OM::Capacity_Agent);
	status1.SetClearingID("0551");
	status1.SetClearingAccount("123456");
//	status1.SetExchange(OM::Exchange_Archipelago);
	status1.SetStatus(OM::Status_PendingCancel);
	status1.SetFirmTag("MM5-20020603-2");
	
	status1.SetTradeFor("CL");
	status1.SetOwner("CL");

	order.Update(status1);

	assert( order.GetOrdStatus() == FIX::OrdStatus_PENDING_CANCEL );

	OM::Status status2;

	status2.SetTrader("CL");
	status2.SetTag("FIX.4.2 CL TD 1");
	status2.SetSide(OM::Side_Buy);
	status2.SetSymbol("MSFT");
	status2.SetShares(100);
	status2.SetPrice(50.00);
	status2.SetType(OM::Type_Market);
	status2.SetTIF(OM::TIF_Day);
	status2.SetCapacity(OM::Capacity_Agent);
	status2.SetClearingID("0551");
	status2.SetClearingAccount("123456");
//	status2.SetExchange(OM::Exchange_Archipelago);
	status2.SetStatus(OM::Status_Filled);
	status2.SetFirmTag("MM5-20020603-2");
	status2.SetTradeFor("CL");
	status2.SetOwner("CL");
	status2.SetOMExecTag("11");

	order.Update(status2);

	assert( order.GetOrdStatus() == FIX::OrdStatus_PENDING_CANCEL );

	OM::Status status3;
	
	status3.SetTrader("CL");
	status3.SetTag("FIX.4.2 CL TD 1");
	status3.SetSide(OM::Side_Buy);
	status3.SetSymbol("MSFT");
	status3.SetShares(100);
	status3.SetPrice(0.00);
	status3.SetType(OM::Type_Market);
	status3.SetTIF(OM::TIF_Day);
	status3.SetCapacity(OM::Capacity_Agent);
	status3.SetClearingID("0551");
	status3.SetClearingAccount("123456");
//	status3.SetExchange(OM::Exchange_Archipelago);
	status3.SetStatus(OM::Status_CancelRejected);
	status3.SetFirmTag("MM5-20020603-2");
	
	status3.SetTradeFor("CL");
	status3.SetOwner("CL");

	order.Update(status3);

	assert( order.GetOrdStatus() == FIX::OrdStatus_FILLED );
}
