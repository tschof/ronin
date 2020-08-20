#include "stdafx.h"

#include "ZLog.h"
#include <time.h>
#include <sstream>
#include <iomanip>

/*
ZLog::ZLog(string name, unsigned long maxSize, bool firstRun, bool RecordTime, bool AppendEOL)
{
	if (firstRun)
	{
		string fileName = name + string("-") + DateString() + string ("_0") + string(".")	+ string("csv");
		SetFileName(fileName);
	}
	else
	{
		SetFileName(name);
	}

	m_File = fopen(FileName().c_str(), "a+");

	if( !m_File )
	{
		std::stringstream stm;
		stm << "Fail to open log file: " << FileName().c_str();
		throw exception(stm.str().c_str());
	}
	m_RecordTime = RecordTime;
	m_AppendEOL = AppendEOL;

	m_MaxFileSize = maxSize;
}
*/

ZLog::ZLog(string datFile, char secType)
:m_datFile(datFile)
,m_secType(secType)
{
	m_MaxFileSize = GetPrivateProfileInt("Settings", "Max CSVFile Size", 50000000, m_datFile.c_str() );
	string secTypeStr = GetSecTypStr();
	m_logfileEntry = string("Current ") + GetSecTypStr() + string(" CSV File");
	
	char l_drive[160];
	GetPrivateProfileString("Settings", "WorkPath", "C:\\", l_drive, 159, "\\OrderManager\\Oats.dat");
	string currentDate = (CTime::GetCurrentTime()).Format("%Y%m%d").GetBuffer(0);

	std::stringstream stm;
	stm << l_drive 
		<< "\\"
		<< "OATS"
		<< "\\"
		<< secTypeStr
		<< "\\"
		<< currentDate;

	m_logfileDir = stm.str();

	if (IsFirstRun())
	{
		CreateDirectory(m_logfileDir.c_str(), NULL);
		
		std::stringstream stm;
		stm << m_logfileDir
			<< "\\"
			<< m_secType
			<< "Oats"
			<< '-'
			<< DateString()
			//<< '_'
			//<< TimeString()
			<< "_0.csv";

		SetFileName(stm.str());
		WritePrivateProfileString("Settings", m_logfileEntry.c_str(), FileName().c_str(), m_datFile.c_str());
	}
	else
	{
		char l_filename[200];
		GetPrivateProfileString("Settings", m_logfileEntry.c_str(), "", l_filename, 199, m_datFile.c_str());
		SetFileName(l_filename);
	}

	m_File = fopen(FileName().c_str(), "a+");
	fseek(m_File, 0, SEEK_END);
}

ZLog::~ZLog()
{
	if( m_File )
		fclose(m_File);
}


void ZLog::Log(char * buffer, int length)
{
	if(!length)
		length = strlen(buffer);

	//if(DateString() != m_Date)
	//{
	//	fclose(m_File);
	//	m_Date = DateString();
	//	m_File = fopen(FileName().c_str(), "a+");
	//}

	//if(m_RecordTime)
	//{
	//	fwrite(TimeString().c_str(), sizeof(char), 6, m_File);
	//	fputc(' ', m_File);
	//}

	unsigned long currentSize = ftell(m_File);
	if (currentSize >= m_MaxFileSize - length)
	{
		fclose(m_File);
		UpdateFileName();
		m_File = fopen(FileName().c_str(), "a+");
		WritePrivateProfileString("Settings", m_logfileEntry.c_str(), FileName().c_str(), m_datFile.c_str());
	}

	fwrite(buffer, sizeof(char), length, m_File);
	
	//if(m_AppendEOL)
	fwrite("\r\n", sizeof(char), 2, m_File);

	fflush(m_File);
}

string ZLog::DateString()
{
	string r_value;
	/*char convert[3];

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
	r_value += string(convert);*/

	r_value = CTime::GetCurrentTime().Format("%Y-%m-%d").GetBuffer(0);

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

void ZLog::UpdateFileName()
{
	int index1 = m_Name.find_last_of('.');
	int index2 = m_Name.find_last_of('_') + 1;
	string num = m_Name.substr(index2, index1 - index2);
	int iNum = atoi(num.c_str()) + 1;

	std::stringstream stm;

	stm << m_logfileDir
		<< "\\"
		<< m_secType
		<< "Oats"
		<< '-'
		<< DateString()
		<< '_'
		//<< TimeString()
		//<< '_'
		<< iNum
		<< ".csv";

	m_Name = stm.str();
}


bool ZLog::IsFirstRun()
{
	char	LastRunDate[128];
	bool	rc = true;
	CString CurrentDate = CTime::GetCurrentTime().Format("%Y%m%d");
	
	GetPrivateProfileString("Runtime Information", "Last Run Date", "ABC", LastRunDate, m_datFile.length(), m_datFile.c_str());

	if(strcmp((LPCTSTR)CurrentDate, LastRunDate) == 0)
	{
		rc = false;
	}

	return rc;
}


string ZLog::GetSecTypStr()
{
	string rc = "";

	switch(m_secType)
	{
	case EQUITY:
		rc = "Equity";
		break;
	case OPTION:
		rc = "Option";
		break;
	case FUTURE:
		rc = "Future";
		break;
	case CURRENCY:
		rc = "Currency";
		break;
	}

	return rc;
}
