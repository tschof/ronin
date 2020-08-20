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

	ZLog(char *);
	virtual ~ZLog();

	void Log(char *);
	static string DateString();
	static string TimeString();
	string FileName();
private:

	string m_Name;
	string m_Date;
	string m_Ext;
	FILE *m_File;
};

#endif

