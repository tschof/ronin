#include "..\stdafx.h"
#include "OMMessageTestCase.h"
#include <sstream>
#include <time.h>

bool OMMessageTest::onSetup( OM::Message*& pMessage )
{
	pMessage = &m_message;
	return true;
}

void OMMessageTest::onTeardown( OM::Message* pMessage )
{
	
}

OMMessageTestCase::OMMessageTestCase()
{
	add( &m_testRead );
	add( &m_testWrite );
	add( &m_testGetField );
	add( &m_testSetField );
	add( &m_testConvertTimeToString );
	add( &m_testConvertStringToTime );
	add( &m_testConvertIntToString );
	add( &m_testConvertStringToDouble );
}

void OMMessageTestCase::TestRead::onRun( OM::Message& message )
{
	std::stringstream stream("L,125959,TRDR\n");
	assert( !message.Read(stream).fail() );
	assert( m_fields.size() == 3 );

	OM::Message::Fields::const_iterator iter = m_fields.begin();
	assert( *iter == "L" );
	++iter;
	assert( *iter == "125959" );
	++iter;
	assert( *iter == "TRDR" );
}

void OMMessageTestCase::TestWrite::onRun( OM::Message& message )
{
	m_fields.push_back("L");
	m_fields.push_back("125959");
	m_fields.push_back("TRDR");

	std::stringstream stream;
	assert( !message.Write(stream).fail() );
	assert( stream.str() == "L,125959,TRDR\n" );
}

void OMMessageTestCase::TestGetField::onRun( OM::Message& message )
{
	m_fields.push_back("L");
	m_fields.push_back("125959");
	m_fields.push_back("TRDR");

	assert( message.GetField(1) == "125959" );

	try
	{
		message.GetField(0);
		assert(false);
	}
	catch( std::out_of_range& ) {}

	try
	{
		message.GetField(3);
		assert(false);
	}
	catch( std::out_of_range& ) {}
}

void OMMessageTestCase::TestSetField::onRun( OM::Message& message )
{
	message.SetField( 1, "125959" );
	assert( m_fields[1] == "125959" );

	try
	{
		message.SetField( 0, "x" );
		assert( false );
	}
	catch( std::out_of_range& ){}
}

void OMMessageTestCase::TestConvertTimeToString::onRun( OM::Message& message )
{
	time_t timeVal;
	time(&timeVal);

	tm* pTimeStruct = localtime(&timeVal);

	pTimeStruct->tm_hour = 1;
	pTimeStruct->tm_min = 59;
	pTimeStruct->tm_sec = 0;

	timeVal = mktime(pTimeStruct);

	assert( OM::Message::ConvertTimeToString(timeVal) == "015900" );
}

void OMMessageTestCase::TestConvertStringToTime::onRun( OM::Message& message )
{
	time_t timeVal = OM::Message::ConvertStringToTime( "015900" );

	tm* pTimeStruct = localtime(&timeVal);

	assert( pTimeStruct->tm_hour == 1 );
	assert( pTimeStruct->tm_min == 59 );
	assert( pTimeStruct->tm_sec == 0 );
}

void OMMessageTestCase::TestConvertIntToString::onRun( OM::Message& message )
{
	assert( OM::Message::ConvertIntToString(1) == "1" );
	assert( OM::Message::ConvertIntToString(200) == "200" );
}

void OMMessageTestCase::TestConvertStringToDouble::onRun( OM::Message& message )
{
	assert( OM::Message::ConvertStringToDouble("1.23") == 1.23 );
	assert( OM::Message::ConvertStringToDouble("1.23000000000") == 1.23 );
}
