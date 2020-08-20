#ifndef _ZLOG_H__
#define _ZLOG_H__



#include <stdio.h>
#include <string>

#if defined(_WIN32)
	using namespace std;
#endif

class ZLog
{

public:

	static string DateString();
	static string TimeString();
	static void CreateCurrentDayDirectories();
	static string GetSecTypeStr(char type);

	enum SECTYPE
	{
		EQUITY = 'e',
		OPTION = 'o',
		FUTURE = 'f',
		CURRENCY = 'c',
		COMPLEX = 's'
	};

	ZLog(char secType, std::string romID);
	ZLog(char secType, std::string romID, std::string date); //for recover
	virtual ~ZLog();

	void Log(const std::string& msg);
	string FileName() { return m_Name;}
	void SetFileName(string name) { m_Name = name; }
	void UpdateFileName();
	//bool IsFirstRun();
	string GetSecTypeStr();

private:

	string m_Name;
	string m_datFile;
	string m_logfileEntry;	 //dat file entry for current log file full path
	string m_logfileDir;     //the directory for log file
	char m_secType;
	
	FILE *m_File;

	std::string RecoverDateString(std::string& date);
	

	//unsigned long m_MaxFileSize;
};

#endif //_ZLOG_H__

