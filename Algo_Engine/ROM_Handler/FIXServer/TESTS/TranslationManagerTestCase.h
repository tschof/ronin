#ifndef TRANSLATIONMANAGER_TEST_CASE_H
#define TRANSLATIONMANAGER_TEST_CASE_H

#include "CPPTest\TestCase.h"
#include "..\TranslationManager.h"

class TranslationManagerTest : public CPPTest::Test<TranslationManager>
{
	public:
		bool onSetup( TranslationManager*& pObject );
		void onTeardown( TranslationManager* pObject );
	
	protected:
		TranslationManager::OMIDToOrderMap* m_omOrders;
		TranslationManager::FIXIDToOrderMap* m_fixOrders;
};

class TranslationManagerTestCase : public CPPTest::TestCase<TranslationManager>
{
	public:
		TranslationManagerTestCase();

		class TestInsert : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );
		
		} m_testInsert;

		class TestFindByTag : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testFindByTag;

		class TestErase : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testErase;

		class TestTranslateNewOrderSingleToEnterOrder : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateNewOrderSingleToEnterOrder;

		class TestTranslateStatusRejectedToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusRejectedToExecutionReport;

		class TestTranslateStatusOpenToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusOpenToExecutionReport;

		class TestTranslateStatusFilledToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusFilledToExecutionReport;

		class TestTranslateStatusPartiallyFilledToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusPartiallyFilledToExecutionReport;

		class TestTranslateStatusFilledAfterPartialToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusFilledAfterPartialToExecutionReport;

		class TestTranslateOrderCancelRequestToCancelOrder : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateOrderCancelRequestToCancelOrder;

		class TestTranslateStatusToOrderCancelReject : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusToOrderCancelReject;

		class TestTranslateStatusPendingCancelToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusPendingCancelToExecutionReport;

		class TestTranslateStatusFilledAfterPendingCancelToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusFilledAfterPendingCancelToExecutionReport;


		class TestTranslateStatusPartiallyFilledAfterPendingCancelToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusPartiallyFilledAfterPendingCancelToExecutionReport;

		class TestTranslateStatusCancelledToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusCancelledToExecutionReport;

		class TestTranslateOrderCancelReplacePriceToReplaceOrder : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateOrderCancelReplacePriceToReplaceOrder;

		class TestTranslateOrderCancelReplaceQuantityToReplaceOrder : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateOrderCancelReplaceQuantityToReplaceOrder;

		class TestTranslateStatusFilledAfterReplaceToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusFilledAfterReplaceToExecutionReport;

		class TestTranslateStatusReplacedToExecutionReport : public TranslationManagerTest
		{
			public:
				void onRun( TranslationManager& tm );

		} m_testTranslateStatusReplacedToExecutionReport;

};

#endif
