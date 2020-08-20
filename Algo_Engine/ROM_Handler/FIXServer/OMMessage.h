#ifndef OM_MESSAGE_H
#define OM_MESSAGE_H

#pragma warning( disable : 4786 )

#include <iosfwd>
#include <vector>
#include <string>

class OMMessageTest;

namespace OM
{
	class Message
	{
		public:
			Message() {}
			explicit Message( char command, int length );
			Message( const std::string& str );

			std::istream& Read( std::istream& stream );
			std::ostream& Write( std::ostream& stream ) const;

			char GetCommand() const;
			
			const std::string& GetField(int pos ) const throw( std::out_of_range );
			void SetField( int pos, const std::string& value );

			std::string getString() const; 

		protected:
			static std::string ConvertTimeToString( const time_t& value );
			static time_t ConvertStringToTime( const std::string& value );
			static std::string ConvertIntToString( int value );
			static int ConvertStringToInt( const std::string& value );
			static std::string ConvertDoubleToString( double value );
			static double ConvertStringToDouble( const std::string& value );
			static std::string ConvertCharToString( char value );
			static char ConvertStringToChar( const std::string& value );
		private:
			typedef std::vector<std::string> Fields;
			Fields m_fields;
			std::string m_szMsg;
			friend OMMessageTest;
	};
}

inline std::istream& operator>>( std::istream& stream, OM::Message& message )
{
	return message.Read(stream);
}

inline std::ostream& operator<<( std::ostream& stream, const OM::Message& message )
{
	return message.Write(stream);
}

#endif
