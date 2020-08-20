// Logger.h: interface for the Logger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_H__DFA6341B_38E6_4D34_B48F_853FC6829EC4__INCLUDED_)
#define AFX_LOGGER_H__DFA6341B_38E6_4D34_B48F_853FC6829EC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <fstream>
#include <string>
#include "Lock.h"

class Logger  
{
	Logger();
public:
	virtual ~Logger();

	static Logger & getInstance()
	{
		static Logger logger;
		return logger;
	}


	void LogError( const std::string & szError );
	void LogMessage( const std::string & szMsg );
	void LogDebug( const std::string & szMsg );

private:
	std::string getDateTime();
	std::string getTimeStamp();

	std::ofstream	m_fsError;
	std::ofstream	m_fsMsg;
	std::ofstream	m_fsDebug;
	Lock			m_Lock;
};

#endif // !defined(AFX_LOGGER_H__DFA6341B_38E6_4D34_B48F_853FC6829EC4__INCLUDED_)
