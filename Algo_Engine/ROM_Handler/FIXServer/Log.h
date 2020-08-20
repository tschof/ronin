#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef DWORD
#include <windows.h>
#endif // DWORD

#include <fstream>
#include <iostream>
#include <string>	// required for string operations
#include <set>		// required for set containers (see STL documentation)
#include <map>

// define default masks
#define LOG_ERROR		Log::Level(1)		// 0000 0000 0001
#define LOG_WARNING		Log::Level(4)		// 0000 0000 1000
#define	LOG_MESSAGE		Log::Level(7)		// 0000 0100 0000
#define	LOG_DEBUG		Log::Level(10)	// 0010 0000 0000



class Log  
{
private:
    Log();
    
    //DWORD			Open			(const std::string &sFileName, DWORD dwMask);
    //DWORD			Close			(const std::string &sFileName);
    //DWORD			SetFileMaxSize	(DWORD dwMaxFileSizeInMegaBytes);
    //DWORD			GetFileMaxSize	(); 
    //DWORD			SetFileMaxCount	(DWORD dwFileMaxSize);
    //DWORD			GetFileMaxCount	(); 
    //DWORD			SetMask			(const std::string &sFileName, DWORD dwMask);
    //DWORD			GetMask			(const std::string &sFileName); // returns 0 if error
    //DWORD			SetLevel		(const std::string &sFileName, DWORD dwMask);
    //DWORD			SetLevel		(DWORD dwLevel);
    //DWORD			GetLevel		(const std::string &sFileName); // returns 0 if error
    //DWORD			SetMessageTag	(DWORD dwMask, const std::string & sMessageTag);
    //DWORD			GetMessageTag	(DWORD dwMask, std::string & sMessageTag);
    //std::string		GetMessageTag	(DWORD dwMask);
   // DWORD			SetDirectory	(const std::string & sDirectory);
   // std::string		GetDirectory ();
   // DWORD			GetDirectory	(std::string & sDirectory);
   // std::set<std::string> GetFileNames();
public:
    ~Log();
    static void		Initialize		(const std::string & szPath,const std::string & szApp);
    static DWORD	Write			(DWORD dwMask, const std::string & sMessage);
    static void		Shutdown		();
    static DWORD	Level			(DWORD dwLevel);
    
private:
    static Log s_pLog;
    static Log& getInstance()
    {      
        return s_pLog;
    }
    //std::ofstream m_file;
    DWORD	_Write			(DWORD dwMask, const std::string & sMessage);
    
    DWORD		s_dwFileMaxSize;	// logfile max size; set at 1 mb at init;
    DWORD		s_dwFileMaxCount;		// logfile max count; set at 10 at init;
    std::string	s_sDirectory;				// directory for logfiles (current by default)
    std::string m_szApp;
    // structure for logfile information to be stored in the map container
    struct FILEINFO
    {
        DWORD				dwMask;		// file mask
        std::ofstream *		pStream;	// output Stream
    } _FILEINFO;
    
    std::map<std::string, FILEINFO> s_mapFilenameInfo;		// map FileName(key), FILEINFO(value)
    std::map<std::string, FILEINFO> s_mapFilenameInfoTemp;	// temp map FileName(key), FILEINFO(value)
    std::map<DWORD, std::string> s_mapMaskMessagetag;		// map Mask(key), Messagetag(value)
    bool			s_bInitialized;		// initialization flag (set to true on call to any method)
};

#define DEBUG_THREAD \
    DWORD dwThreadId = GetCurrentThreadId(); \
    std::stringstream stm; stm << L"Current Thread: " << (long)dwThreadId << std::endl; \
    std::string szLog = stm.str(); \
Log::Write( LOG_DEBUG, szLog );

