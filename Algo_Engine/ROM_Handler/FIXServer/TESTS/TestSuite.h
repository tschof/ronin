#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <CPPTest/TestSuite.h>
#include "FIXServerTestCase.h"
#include "OMMessageTestCase.h"
#include "TranslationMessagesTestCases.h"
#include "TranslationManagerTestCase.h"
#include "OrderTestCase.h"

class TestSuite : public CPPTest::TestSuite
{
	public:
		TestSuite( CPPTest::TestDisplay& display )
			: CPPTest::TestSuite( display )
		{
			add( &m_FIXServer );
			add( &m_OMMessage );
			add( &m_enterOrderFromFIX );
			add( &m_cancelOrderFromFIX );
			add( &m_replaceOrderFromFIX );
			add( &m_executionReportFromOM );
			add( &m_orderCancelRejectFromOM );
			add( &m_translationManager );
			add( &m_order );
		}

	private:
		FIXServerTestCase m_FIXServer;
		OMMessageTestCase m_OMMessage;
		EnterOrderFromFIXTestCase m_enterOrderFromFIX;
		CancelOrderFromFIXTestCase m_cancelOrderFromFIX;
		ReplaceOrderFromFIXTestCase m_replaceOrderFromFIX;
		ExecutionReportFromOMTestCase m_executionReportFromOM;
		OrderCancelRejectFromOMTestCase m_orderCancelRejectFromOM;
		TranslationManagerTestCase m_translationManager;
		OrderTestCase m_order;
};

#endif
