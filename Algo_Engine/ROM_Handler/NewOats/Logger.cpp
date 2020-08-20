// Logger.cpp: implementation of the Logger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Logger.h"
#include "Config.h"
#include <exception>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "Locker.h"
#include <time.h>
#include <sys/timeb.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
std::string GetLogFileName()
{
	std::stringstream stm;
	time_t t = time(NULL);
	tm * pTM = localtime( &t );
	stm << "_OATS"
		<< std::setw(4) << pTM->tm_year+1900
		<< std::setw(2) << std::setfill('0') << pTM->tm_mon+1
		<< std::setw(2) << std::setfill('0') << pTM->tm_mday 
		<< ".log";
	return stm.str();
}

Logger::Logger()
{
	std::string szTemp = "ERROR";
	szTemp	+= GetLogFileName();
	m_fsError.open( szTemp.c_str(), std::ios_base::out|std::ios_base::app );
	
	szTemp = "MSG";
	szTemp	+= GetLogFileName();
	m_fsMsg.open( szTemp.c_str(), std::ios_base::out|std::ios_base::app );
	
	szTemp = "DEBUG";
	szTemp	+= GetLogFileName();
	m_fsDebug.open( szTemp.c_str(), std::ios_base::out|std::ios_base::app );

	if ((CConfig::getInstance()).DbPfmLogOn() == true)
	{
		szTemp = "PERFORMENCE";
		szTemp	+= GetLogFileName();
		m_fsPerformence.open( szTemp.c_str(), std::ios_base::out|std::ios_base::app );
	}
}

Logger::~Logger()
{
	m_fsError.close();
	m_fsDebug.close();
	m_fsMsg.close();

	if ((CConfig::getInstance()).DbPfmLogOn() == true)
	{
		m_fsPerformence.close();
	}
}

std::string Logger::getDateTime()
{
	time_t t = time(NULL);
	tm * pTM = localtime( &t );
	std::stringstream stm;
	stm << std::setw(2) << std::setfill('0') << pTM->tm_mon+1		<< '/' 
		<< std::setw(2) << std::setfill('0') << pTM->tm_mday		<< '/'
		<< std::setw(2) << std::setfill('0') << pTM->tm_year-100	<< ' '
		<< std::setw(2) << std::setfill('0') << pTM->tm_hour		<< ':'
		<< std::setw(2) << std::setfill('0') << pTM->tm_min			<< ':'
		<< std::setw(2) << std::setfill('0') << pTM->tm_sec			<< ' ';
	return stm.str();
}

std::string Logger::getTimeStamp()
{
	struct _timeb tm;
	_ftime( &tm );

	std::stringstream stm;
	stm << getDateTime() << tm.millitm << " ";

	return stm.str();
}

void Logger::LogError( const std::string & szError )
{
	Locker locker( &m_Lock );
	m_fsError << getDateTime() << szError.c_str() << std::endl;
}

void Logger::LogMessage( const std::string & szMsg )
{
	Locker locker( &m_Lock );
	m_fsMsg << getDateTime() << szMsg.c_str() << std::endl;
}

void Logger::LogDebug( const std::string & szMsg )
{
	Locker locker( &m_Lock );
	m_fsDebug << getTimeStamp() << szMsg.c_str() << std::endl;
}

void Logger::LogPerformence( const std::string & szMsg )
{
	if ((CConfig::getInstance()).DbPfmLogOn() == true)
	{
		Locker locker( &m_Lock );
		m_fsPerformence << szMsg.c_str() << std::endl;
	}
}
