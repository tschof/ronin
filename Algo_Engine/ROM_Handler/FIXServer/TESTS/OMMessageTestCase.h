#ifndef OMMESSAGE_TEST_CASE_H
#define OMMESSAGE_TEST_CASE_H

#include "CPPTest\TestCase.h"
#include "../OMMessage.h"

class OMMessageTest : public CPPTest::Test<OM::Message>
{
	public:
		OMMessageTest()
		: m_fields(m_message.m_fields) {}

		bool onSetup( OM::Message*& pObject );
		void onTeardown( OM::Message* pObject );
	
	protected:
		OM::Message m_message;
		OM::Message::Fields& m_fields;	
};

class OMMessageTestCase : public CPPTest::TestCase<OM::Message>
{
	public:
		OMMessageTestCase();
		
	private:
		class TestRead : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testRead;
		
		class TestWrite : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testWrite;

		class TestGetField : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testGetField;

		class TestSetField : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testSetField;

		class TestConvertTimeToString : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testConvertTimeToString;

		class TestConvertStringToTime : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testConvertStringToTime;

		class TestConvertIntToString : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testConvertIntToString;

		class TestConvertStringToDouble : public OMMessageTest
		{
			public:
				void onRun( OM::Message& message );

		} m_testConvertStringToDouble;
};

#endif
