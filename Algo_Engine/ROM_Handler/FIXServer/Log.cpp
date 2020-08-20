#pragma warning ( disable : 4786 )	// identifier was truncated to '255' characters in the browser information
#include "StdAfx.h"
#include "Log.h"
#include <map>			// required for map container (see STL documentation)
#include <list>			// required for list container (see STL documentation) 
#include <time.h>		// required for date/time operations
#include <bitset>		// required for bitwise operations (used by GetLevel)

#include <fstream>
#include <iostream>
#include <ostream.h>
#include <iosfwd>
#include <string>

namespace {
    static std::ofstream s_log;
    static CRITICAL_SECTION s_CS;
    
}

Log Log::s_pLog;
//CRITICAL_SECTION g_CS;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Log::Log() : s_bInitialized(false) 
{
    std::cout<<"CREATING A LOG FILE!!!"<<std::endl;
}

Log::~Log()
{
}

//////////////////////////////////////////////////////////////////////
// Initialize
//////////////////////////////////////////////////////////////////////
void Log::Initialize(const std::string & szPath, const std::string & szApp )
{
    // lock method
    if( Log::getInstance().s_bInitialized )
        return;
    InitializeCriticalSection(&s_CS);
    std::string fileName;
    fileName.append(szPath);
    fileName.append("\\");
    fileName.append(szApp);
    
    char	tBuffer[64];
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    memset(tBuffer, '\0', 64);
    wsprintf(tBuffer, "%04u%02u%02u%",                
        systime.wYear, systime.wMonth, systime.wDay);
    fileName.append(tBuffer);
    fileName.append(".txt");
    s_log.open(fileName.c_str(),std::ios::out|std::ios::app);   
    Log::getInstance().s_bInitialized = true;
}

void Log::Shutdown()
{
    getInstance().s_bInitialized = false;
    DeleteCriticalSection(&s_CS);
    s_log.close();
}


//////////////////////////////////////////////////////////////////////
// Write 
//////////////////////////////////////////////////////////////////////
DWORD Log::Write(DWORD dwMask, const std::string & sMessage)
{	
    //EnterCriticalSection(&s_CS);
    DWORD dwReturn = ERROR_SUCCESS;
    char	tBuffer[64];
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    memset(tBuffer, '\0', 64);
    wsprintf(tBuffer, "%04u%02u%02u-%02u:%02u:%02u.%03u",                
        systime.wYear, systime.wMonth, systime.wDay,
        systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
    int len = strlen(tBuffer);
    s_log<<tBuffer<<" -- " <<sMessage.c_str()<<std::endl;
    s_log.flush();   
    ::SetLastError(dwReturn);
    //LeaveCriticalSection(&s_CS);
    return dwReturn;
}


//////////////////////////////////////////////////////////////////////
// Level -- function for setting masks 
//////////////////////////////////////////////////////////////////////
DWORD Log::Level (DWORD dwLevel)
{	
	DWORD dwReturn = 1;
	if(dwLevel > 0 && dwLevel < 32)
	{	// shift the 1 to left dwLevel times
		dwReturn = dwReturn << dwLevel;
		return dwReturn;
	}
	else
	{
		return 0;
	}
}


