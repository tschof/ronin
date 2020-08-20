#include "stdafx.h"
#include "OMMessage.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <iomanip>

namespace OM
{
	Message::Message( char command, int length ) : m_szMsg("")
	{
		m_fields.resize(length+1);
		
		std::string str;
		str += command;
		m_fields[0] = str;
	}

	Message::Message( const std::string& str ) : m_szMsg(str)
	{
		std::stringstream stream(str);
		Read(stream);
	}

	std::string Message::getString() const
	{
		if( !m_szMsg.empty() )
			return m_szMsg;
		std::stringstream stm;
		for( int i = 0; i < m_fields.size(); i++ )
			stm << m_fields[i] << ',';
		return stm.str();
	}

	std::istream& Message::Read( std::istream& stream )
	{
		m_fields.clear();

		char c;
		std::string s;
		
		stream.unsetf( std::ios_base::skipws );
		while( !(stream >> c).fail() )
		{
			switch( c )
			{
				case ',':
				{
					m_fields.push_back(s);
					s.erase();
					break;
				}
				case '\n':
				{
					m_fields.push_back(s);
					return stream;
				}
				default:
					s += c;
					break;
			}
		}

		return stream;
	}

	std::ostream& Message::Write( std::ostream& stream ) const
	{
		Fields::const_iterator iter = m_fields.begin();

		if( iter == m_fields.end() )
			return stream;

		stream << *iter;

		for( ++iter; iter!=m_fields.end(); ++iter )
		{
			stream << ',' << *iter;
			if( stream.fail() )
				return stream;
		}

		stream << std::endl;

		return stream;
	}

	char Message::GetCommand() const
	{
		return m_fields[0][0];
	}

	const std::string& Message::GetField(int pos ) const
	{
		if( pos <= 0 || pos >= m_fields.size() )
			throw std::out_of_range("Out of range.");
		
		return m_fields[pos];
	}

	void Message::SetField( int pos, const std::string& value )
	{
		if( pos <= 0 )
			throw std::out_of_range("Out of range.");

		if( m_fields.size() <= pos )
			m_fields.resize(pos+1);

		m_fields[pos] = value;
	}

	std::string Message::ConvertTimeToString( const time_t& value )
	{
		std::stringstream stream;

		tm* pTimeStruct = localtime(&value);

		stream 
			<< std::setw(2) << std::setfill('0') << pTimeStruct->tm_hour 
			<< std::setw(2) << std::setfill('0') << pTimeStruct->tm_min 
			<< std::setw(2) << std::setfill('0') << pTimeStruct->tm_sec;
		
		return stream.str();
	}
	
	time_t Message::ConvertStringToTime( const std::string& value )
	{
		time_t timeVal;
		time(&timeVal);

		tm* pTimeStruct = localtime(&timeVal);
		
		pTimeStruct->tm_hour = atoi( value.substr(0,2).c_str() );
		pTimeStruct->tm_min = atoi( value.substr(2,2).c_str() );
		pTimeStruct->tm_sec = atoi( value.substr(4,2).c_str() );

		return mktime(pTimeStruct);
	}

	std::string Message::ConvertIntToString( int value )
	{
		std::stringstream stream;
		stream << value;
		return stream.str();
	}

	int Message::ConvertStringToInt( const std::string& value )
	{
		return atoi( value.c_str() );
	}

	std::string Message::ConvertDoubleToString( double value )
	{
		std::stringstream stream;
		stream << value;
		return stream.str();
	}
	
	double Message::ConvertStringToDouble( const std::string& value )
	{
		return strtod( value.c_str(), NULL );
	}

	std::string Message::ConvertCharToString( char value )
	{
		std::string str;
		str += value;
		return str;
	}

	char Message::ConvertStringToChar( const std::string& value )
	{
		return value[0];
	}
}
