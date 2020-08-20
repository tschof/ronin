#if !defined(CLASS_LOG)
#define CLASS_LOG

// ZLog
// ----
//
// Creates a time stamped log file

#include <stdio.h>
#include <string>

#if defined(_WIN32)
	using namespace std;
#endif

class ZLog
{

public:

	enum SECTYPE
	{
		EQUITY = 'e',
		OPTION = 'o',
		FUTURE = 'f',
		CURRENCY = 'c'
	};

	//ZLog(string name, unsigned long maxSize, bool firstRun, bool RecordTime = true, bool AppendEOL = true);
	ZLog(string datFile, char secType);
	virtual ~ZLog();

	void Log(char *, int length = 0);
	static string DateString();
	static string TimeString();
	string FileName() { return m_Name;}
	void SetFileName(string name) { m_Name = name; }
	void UpdateFileName();
	bool IsFirstRun();
	string GetSecTypStr();
	

private:

	string m_Name;
	string m_datFile;
	string m_logfileEntry;	 //dat file entry for current log file full path
	string m_logfileDir;     //the directory for log file
	char m_secType;
	
	FILE *m_File;
	
//	bool m_RecordTime;
//	bool m_AppendEOL;
	unsigned long m_MaxFileSize;
};

#endif

