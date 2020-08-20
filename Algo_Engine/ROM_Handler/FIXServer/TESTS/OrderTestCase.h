#ifndef ORDER_TEST_CASE_H
#define ORDER_TEST_CASE_H

#include "CPPTest\TestCase.h"
#include "..\Order.h"

class OrderTestCase : public CPPTest::TestCase<Order>
{
	public:
		OrderTestCase();

		class OrderTest : public CPPTest::Test<Order>
		{
			bool onSetup( Order*& pObject );
			void onTeardown( Order* pObject );
		};

		class TestUpdateWithStatusOpen : public OrderTest
		{
			void onRun( Order& order );

		} m_testUpdateWithStatusOpen;

		class TestUpdateWithStatusPendingCancelThenCancelReject : public OrderTest
		{
			void onRun( Order& order );
		} m_testUpdateWithStatusPendingCancelThenCancelReject;

		class TestUpdateWithStatusPendingCancelThenFilledThenCancelReject : public OrderTest
		{
			void onRun( Order& order );
		} m_testUpdateWithStatusPendingCancelThenFilledThenCancelReject;
};
#endif
