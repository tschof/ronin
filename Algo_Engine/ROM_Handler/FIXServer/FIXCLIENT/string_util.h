#pragma once
#include <string>
#include <vector>
class string_util
{
public:
	static std::string trim( const std::string & str )
	{
		LPCTSTR lpsz = str.c_str();
		while (_istspace(*lpsz))
			lpsz = _tcsinc(lpsz);
		return lpsz;
	}

	static void parse( const std::string & str, std::vector<std::string> & fields, char token, char end = '\n' )
	{
		std::stringstream stm( str );
		stm.unsetf( std::ios_base::skipws );
		char c;
		std::string s;
		while( !(stm >> c).fail() )
		{
			if( c == token )
			{
				if( s.empty() && !fields.size() )
					continue;
				s = string_util::trim(s);
				fields.push_back(s);
				s.erase();
			}
			else if( c == end )
			{
				break;
			}
			else
			{
				s += c;
			}
		}
		fields.push_back(s);
	}

};

