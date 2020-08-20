#include "stdafx.h"

#include "ZLog.h"
#include <time.h>

ZLog::ZLog(char * name)
{
	m_Name = string(name);
	m_Date = DateString();
	m_Ext = "log";

	m_File = fopen(FileName().c_str(), "a+");
}

ZLog::~ZLog()
{
	fclose(m_File);
}

string ZLog::FileName()
{
	string r_value = m_Name + string(".") + m_Ext;
	return r_value;
}

void ZLog::Log(char * buffer)
{
	if(DateString() != m_Date)
	{
		fclose(m_File);
		m_Date = DateString();
		m_File = fopen(FileName().c_str(), "a+");
	}

	fwrite(buffer, sizeof(char), strlen(buffer), m_File);
	fflush(m_File);
}

string ZLog::DateString()
{
	string r_value;
	char convert[3];

	time_t systime;
	tm *ltime;

	time(&systime);
	ltime = localtime(&systime);

	#ifdef _WIN32
	ltime->tm_mon++;
	#endif

	sprintf(convert, "%02d", ltime->tm_mday);
	r_value = convert;

	sprintf(convert, "%02d", ltime->tm_mon);
	r_value += string(convert);

	sprintf(convert, "%02d", ltime->tm_year);
	r_value += string(convert);

	return r_value;
}

string ZLog::TimeString()
{
	string r_value;
	char convert[3];

	time_t systime;
	tm *ltime;

	time(&systime);
	ltime = localtime(&systime);

	sprintf(convert, "%02d", ltime->tm_hour);
	r_value = convert;

	sprintf(convert, "%02d", ltime->tm_min);
	r_value += string(convert);

	sprintf(convert, "%02d", ltime->tm_sec);
	r_value += string(convert);

	return r_value;
}
