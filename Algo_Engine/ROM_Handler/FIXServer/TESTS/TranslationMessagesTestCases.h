#ifndef TRANSLATION_MESSAGES_TEST_CASES_H
#define TRANSLATION_MESSAGES_TEST_CASES_H

#include "CPPTest\TestCase.h"
#include "..\TranslationMessages.h"

class EnterOrderFromFIXTestCase : public CPPTest::TestCase<EnterOrderFromFIX>
{
	public:
		EnterOrderFromFIXTestCase();
		
		class TestBuy100MSFTAtMarket : public CPPTest::Test<EnterOrderFromFIX>
		{
			public:
				bool onSetup( EnterOrderFromFIX*& pObject );
				void onTeardown( EnterOrderFromFIX* pObject );
				void onRun( EnterOrderFromFIX& message );
		
		} m_testBuy100MSFTAtMarket;

		class TestSell1000MSFTAt50 : public CPPTest::Test<EnterOrderFromFIX>
		{
			public:
				bool onSetup( EnterOrderFromFIX*& pObject );
				void onTeardown( EnterOrderFromFIX* pObject );
				void onRun( EnterOrderFromFIX& message );
		
		} m_testSell1000MSFTAt50;

};

class CancelOrderFromFIXTestCase : public CPPTest::TestCase<CancelOrderFromFIX>
{
	public:
		CancelOrderFromFIXTestCase();

		class TestCancelSellMSFT : public CPPTest::Test<CancelOrderFromFIX>
		{
			public:
				bool onSetup( CancelOrderFromFIX*& pObject );
				void onTeardown( CancelOrderFromFIX* pObject );
				void onRun( CancelOrderFromFIX& message );

		} m_testCancelSellMSFT;
};

class ReplaceOrderFromFIXTestCase : public CPPTest::TestCase<ReplaceOrderFromFIX>
{
	public:
		ReplaceOrderFromFIXTestCase();

		class TestReplaceBuy100MSFTAt50WithAt51 : public CPPTest::Test<ReplaceOrderFromFIX>
		{
			public:
				bool onSetup( ReplaceOrderFromFIX*& pObject );
				void onTeardown( ReplaceOrderFromFIX* pObject );
				void onRun( ReplaceOrderFromFIX& message );

		} m_testReplaceBuy100MSFTAt50WithAt51;
};

class ExecutionReportFromOMTestCase : public CPPTest::TestCase<ExecutionReportFromOM>
{
	public:
		ExecutionReportFromOMTestCase();

		class OrderRejected : public CPPTest::Test<ExecutionReportFromOM>
		{
			public:
				bool onSetup( ExecutionReportFromOM*& pObject );
				void onTeardown( ExecutionReportFromOM* pObject );
				void onRun( ExecutionReportFromOM& message );

		} m_testOrderRejected;
		
		class OrderAccepted : public CPPTest::Test<ExecutionReportFromOM>
		{
			public:
				bool onSetup( ExecutionReportFromOM*& pObject );
				void onTeardown( ExecutionReportFromOM* pObject );
				void onRun( ExecutionReportFromOM& message );

		} m_testOrderAccepted;

		class OrderFilled : public CPPTest::Test<ExecutionReportFromOM>
		{
			public:
				bool onSetup( ExecutionReportFromOM*& pObject );
				void onTeardown( ExecutionReportFromOM* pObject );
				void onRun( ExecutionReportFromOM& message );

		} m_testOrderFilled;

		class OrderPartiallyFilled : public CPPTest::Test<ExecutionReportFromOM>
		{
			public:
				bool onSetup( ExecutionReportFromOM*& pObject );
				void onTeardown( ExecutionReportFromOM* pObject );
				void onRun( ExecutionReportFromOM& message );

		} m_testOrderPartiallyFilled;

		class OrderPendingCancel : public CPPTest::Test<ExecutionReportFromOM>
		{
			public:
				bool onSetup( ExecutionReportFromOM*& pObject );
				void onTeardown( ExecutionReportFromOM* pObject );
				void onRun( ExecutionReportFromOM& message );

		} m_testOrderPendingCancel;

		class OrderCancelled : public CPPTest::Test<ExecutionReportFromOM>
		{
			public:
				bool onSetup( ExecutionReportFromOM*& pObject );
				void onTeardown( ExecutionReportFromOM* pObject );
				void onRun( ExecutionReportFromOM& message );

		} m_testOrderCancelled;

		class OrderReplaced : public CPPTest::Test<ExecutionReportFromOM>
		{
			public:
				bool onSetup( ExecutionReportFromOM*& pObject );
				void onTeardown( ExecutionReportFromOM* pObject );
				void onRun( ExecutionReportFromOM& message );

		} m_testOrderReplaced;
};

class OrderCancelRejectFromOMTestCase : public CPPTest::TestCase<OrderCancelRejectFromOM>
{
	public:
		OrderCancelRejectFromOMTestCase();

		class OrderCancelRejected : public CPPTest::Test<OrderCancelRejectFromOM>
		{
			public:
				bool onSetup( OrderCancelRejectFromOM*& pObject );
				void onTeardown( OrderCancelRejectFromOM* pObject );
				void onRun( OrderCancelRejectFromOM& message );

		} m_testOrderCancelRejected;
};

#endif
