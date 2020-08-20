// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////
//
//	Log object
//

#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "Log.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include "Main.h"
#include "Locker.h"

static std::string		directory		= "C:\\Ragnarok\\logfile\\Debug";
static std::string		fileName		= "Log";
static std::string		currentFileName = "";
static std::ofstream	logStream;
static int				size			= 1000000;
static int				currentSize		= 0;
static int				level			= LOG_WARNING;
static int				levelEvent		= LOG_WARNING;
static int				maxFiles		= 5;
int						gCount			= 0;

//CRITICAL_SECTION		csLog;
Lock					gLock;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	if (0 == gCount)
	{
//		InitializeCriticalSectionAndSpinCount(&csLog, 0x80000400);
	}

	gCount++;
}

CLog::~CLog()
{
	gCount--;
	if (0 == gCount)
	{
//		DeleteCriticalSection(&csLog);
	}
}

void CLog::setDirectory(const std::string & newDirectory)
{
	Locker locker(&gLock);
//	EnterCriticalSection(&csLog);
	directory.assign(newDirectory);
//	LeaveCriticalSection(&csLog);
}

std::string CLog::getDirectory()
{
	return (directory) ;
}

void CLog::setFile( const std::string & newFileName)
{
	Locker locker(&gLock);

//	EnterCriticalSection(&csLog);
	fileName = newFileName;
//	LeaveCriticalSection(&csLog);
}

std::string CLog::getFile()
{
	return (fileName);
}

void CLog::setLogSize(int newSize)
{
	Locker locker(&gLock);

//	EnterCriticalSection(&csLog);
	size = newSize;
//	LeaveCriticalSection(&csLog);
}

void CLog::open()
{
	char		newFileName_1[500];
	char		newFileName_2[500];
	time_t		now = time(NULL);
	tm *		tmNow = localtime(&now);
	std::string		findFile;

//	EnterCriticalSection(&csLog);
	Locker locker(&gLock);

	deleteLogs();

	try
	{
		_chdir(directory.c_str());

		sprintf(newFileName_1,"%s/%s_%04d%02d%02d%02d%02d%02d.log",
							directory.c_str(),
							fileName.c_str(),
							tmNow->tm_year+1900,
							tmNow->tm_mon+1,
							tmNow->tm_mday,
							tmNow->tm_hour,
							tmNow->tm_min,
							tmNow->tm_sec);


		sprintf(newFileName_2,"./%s_%04d%02d%02d%02d%02d%02d%02d.log",
							directory.c_str(),
							fileName.c_str(),
							tmNow->tm_year+1900,
							tmNow->tm_mon+1,
							tmNow->tm_mday,
							tmNow->tm_hour,
							tmNow->tm_min,
							tmNow->tm_sec);

		if (logStream.is_open())
		{
			logStream.close();
		}

		currentSize = 0;

		logStream.open(newFileName_1);

		if (!logStream.is_open())
		{
			logStream.open(newFileName_2);
		}
	}
	catch(...)
	{
		//LeaveCriticalSection(&csLog);
		throw;
	}
	//LeaveCriticalSection(&csLog);
}

void CLog::setLevel(int logLevel)
{
	Locker locker(&gLock);

//	EnterCriticalSection(&csLog);
	level = logLevel;
//	LeaveCriticalSection(&csLog);
}

int CLog::getLevel()
{
	return (level);
}

void CLog::write( const std::string & message)
{
	Locker locker(&gLock);

//	EnterCriticalSection(&csLog);
	write(LOG_MESSAGE, message);
//	LeaveCriticalSection(&csLog);
}

void CLog::write(int logLevel, const std::string & message)
{
	Locker locker(&gLock);
	writeEventLog(logLevel, message);
	time_t	now;
	time(&now);
	struct tm * pTmNow = localtime(&now);
	char tStamp[20];

	sprintf(tStamp, "%04d%02d%02d_%02d:%02d:%02d",
		pTmNow->tm_year+1900,
		pTmNow->tm_mon+1,
		pTmNow->tm_mday,
		pTmNow->tm_hour,
		pTmNow->tm_min,
		pTmNow->tm_sec);

//	EnterCriticalSection(&csLog);

	if (logStream.is_open() && logLevel <= level)
	{
		logStream << tStamp << " " ;

		switch(logLevel)
		{
		case LOG_ERROR:
			CMain::getInstance()->AddError( message );
			logStream << "ERROR   " ;
			break;
		case LOG_WARNING:
			CMain::getInstance()->AddWarning( message );
			logStream << "WARNING ";
			break;
		case LOG_MESSAGE:
			logStream << "MESSAGE ";
			CMain::getInstance()->AddWarning( message );
			break;
		case LOG_DEBUG:
			logStream << "DEBUG   ";
			break;
		case LOG_BUNCH_O_CRAP:
			logStream << "BUNCH_O_CRAP   ";
			break;
		}
		currentSize += 8;

		try
		{
			if(m_objectName.size())
			{
				logStream << "(" << m_objectName << ") " ;
			}
		}
		catch(...)
		{
			// don't worry about the object name
		}


		int i = message.find("\n");
		if (i >= 0)
		{
			logStream << message;
		}
		else
		{
			logStream << message << std::endl;
		}
		currentSize += message.size();

		logStream.flush();

		if (currentSize > size)
		{
			open();
			deleteLogs();
		}
	}
//	LeaveCriticalSection(&csLog);
}

void CLog::write(const std::string & object, int line, int logLevel, const std::string & message)
{
	Locker locker(&gLock);
//	EnterCriticalSection(&csLog);
	char	buff[256];
	sprintf(buff,"%s[%d] ", object.c_str(), line);

	std::string temp = buff + message;

	write(logLevel, temp);
//	LeaveCriticalSection(&csLog);
}

void CLog::setObjectName( const std::string & name)
{
	Locker locker(&gLock);

//	EnterCriticalSection(&csLog);
	m_objectName = name;
//	LeaveCriticalSection(&csLog);
}

std::string CLog::getObjectName()
{
	return(m_objectName);
}

void CLog::deleteLogs()
{
//	EnterCriticalSection(&csLog);
	Locker locker(&gLock);

    struct _finddata_t	c_file;
    long				hFile;
	std::string				searchString;
	int					i = 1;

	try
	{
		searchString = directory + "/" + fileName + "*.log";

		if ((hFile = _findfirst(searchString.c_str(), &c_file)) == -1L)
		{
			searchString = "./" + fileName + "*.log";
			hFile = _findfirst(searchString.c_str(), &c_file);
		}

		if(hFile != -1L)
		{
			while( _findnext( hFile, &c_file ) == 0 )
			{
				i++;
			}
		}

		_findclose(hFile);

		hFile = _findfirst(searchString.c_str(), &c_file);
		if(hFile != -1L && i>maxFiles)
		{
			do
			{
				searchString = directory + "/" + c_file.name ;
				_unlink(searchString.c_str());
			} while (--i>maxFiles && (_findnext( hFile, &c_file ) == 0));
		}

		_findclose(hFile);
	}
	catch(...)
	{
	}
//	LeaveCriticalSection(&csLog);
}

void CLog::setMaxLogs(int i)
{
	Locker locker(&gLock);

//	EnterCriticalSection(&csLog);
	maxFiles = i;
//	LeaveCriticalSection(&csLog);
}

int CLog::getMaxLogFiles()
{
	return(maxFiles);
}

void CLog::setEventLogLevel(int level)
{
	levelEvent = level;
}

void CLog::writeEventLog(int level, const std::string & message)
{
	std::string szBuff = "";
	if(level <= levelEvent)
	{
		HANDLE h = RegisterEventSource(NULL, "OMDS");
		WORD type;
		//char * data = (char*)malloc(message.size() + 20);

		switch(level)
		{
		case LOG_ERROR:
			type=EVENTLOG_ERROR_TYPE;
			szBuff = (std::string) "ERROR   " + message;
			break;
		case LOG_WARNING:
			type=EVENTLOG_WARNING_TYPE;
			szBuff = (std::string) "WARNING " + message;
			break;
		case LOG_MESSAGE:
			type=EVENTLOG_INFORMATION_TYPE;
			szBuff = (std::string) "MESSAGE " + message;
			break;
		case LOG_DEBUG:
			type=EVENTLOG_INFORMATION_TYPE;
			szBuff = (std::string) "DEBUG   " + message;
			break;
		case LOG_BUNCH_O_CRAP:
			type=EVENTLOG_INFORMATION_TYPE;
			szBuff = (std::string) "MISC    " + message;
			break;
		}

		//strcpy(data,message.c_str());
		LPCTSTR pMsg = szBuff.c_str();
		ReportEvent(h, type, NULL,0x00,NULL,1,0,(const char **)&pMsg, NULL);
		if (!DeregisterEventSource(h))
		{
			write(LOG_ERROR, "Unable to DeregisterEventSource.");
		}
	}
}
