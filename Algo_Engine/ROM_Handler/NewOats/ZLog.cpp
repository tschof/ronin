#include "stdafx.h"
#include "ZLog.h"
#include <time.h>
#include <sstream>
#include <iomanip>
#include "Config.h"
#include "Windows.h"
#include "shlobj.h"

ZLog::ZLog(char secType, std::string romID, std::string date)
: m_secType(secType)
{
	string secTypeStr = GetSecTypeStr();

	std::stringstream stm;
	stm << (CConfig::getInstance()).Drive() 
	    << "OATS"
		<< "\\"
		<< secTypeStr
		<< "\\"
		<< date;

	//CreateDirectory(stm.str().c_str(), NULL);
	SHCreateDirectoryEx(NULL, stm.str().c_str(), NULL);

	std::stringstream stm1;
	stm1 << stm.str()
		 << "\\"
		 << romID
		 << m_secType
		 << "Oats"
		 << '-'
		 << RecoverDateString(date)
		 << "_0.csv";

	m_Name = stm1.str();

	m_File = fopen(m_Name.c_str(), "a+");
}

std::string ZLog::RecoverDateString(std::string& date)
{
	std::stringstream stm;
	stm << date.substr(0, 4)
		<< "-"
		<< date.substr(4, 2)
		<< "-"
		<< date.substr(6, 2);
	return stm.str();
}

ZLog::ZLog(char secType, std::string romID)
: m_secType(secType)
{
	string secTypeStr = GetSecTypeStr();
	m_logfileEntry = string("Current ") + romID + string(" ") + GetSecTypeStr() + string(" CSV File");

	char	Date[128];

	SYSTEMTIME systime;
	GetSystemTime(&systime);
	sprintf(Date, "%04u%02u%02u", systime.wYear, systime.wMonth, systime.wDay);

	std::stringstream stm;
	stm << (CConfig::getInstance()).Drive() 
		<< "OATS"
		<< "\\"
		<< secTypeStr
		<< "\\"
		<< Date;

	m_logfileDir = stm.str();

	(CConfig::getInstance()).GetConfig("Settings", m_logfileEntry, m_Name);

	if (strncmp(m_logfileDir.c_str(), m_Name.c_str(), m_logfileDir.length()) != 0)
	//if (IsFirstRun())
	{
		//CreateDirectory(m_logfileDir.c_str(), NULL);
		SHCreateDirectoryEx(NULL, m_logfileDir.c_str(), NULL);
		
		std::stringstream stm;
		stm << m_logfileDir
			<< "\\"
			<< romID
			<< m_secType
			<< "Oats"
			<< '-'
			<< DateString()
			<< "_0.csv";

		m_Name = stm.str();
		(CConfig::getInstance()).SaveConfig("Settings", m_logfileEntry, m_Name);
	}

	m_File = fopen(FileName().c_str(), "a+");
	fseek(m_File, 0, SEEK_END);
}

ZLog::~ZLog()
{
	if( m_File )
		fclose(m_File);
}


void ZLog::Log(const std::string& msg)
{
	int length = msg.length();

	unsigned long currentSize = ftell(m_File);
	long maxSize = (CConfig::getInstance()).MaxOatsFileSize();
	if (currentSize >= maxSize - length)
	{
		fclose(m_File);
		UpdateFileName();
		m_File = fopen(FileName().c_str(), "a+");
		fwrite(msg.c_str(), sizeof(char), length, m_File);
		(CConfig::getInstance()).SaveConfig("Settings", m_logfileEntry, m_Name);
		//WritePrivateProfileString("Settings", m_logfileEntry.c_str(), FileName().c_str(), m_datFile.c_str());
	}
	else
	{
		fwrite(msg.c_str(), sizeof(char), length, m_File);
	}

	fflush(m_File);
}

string ZLog::DateString()
{
	string r_value;

	char	CurrentDate[128];
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	sprintf(CurrentDate, "%04u-%02u-%02u", systime.wYear, systime.wMonth, systime.wDay);
	r_value = CurrentDate;

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

	m_Name = m_Name.substr(0, index2);

	std::stringstream stm;

	stm << m_Name
		<< iNum
		<< ".csv";

	m_Name = stm.str();
}

/*
bool ZLog::IsFirstRun()
{
	char	CurrentDate[128];
	bool	rc = true;
	SYSTEMTIME systime;

    GetSystemTime(&systime);
	sprintf(CurrentDate, "%04u%02u%02u", systime.wYear, systime.wMonth, systime.wDay);

	if(strcmp(CurrentDate, (CConfig::getInstance()).LastRunDate()) == 0)
	{
		rc = false;
	}

	return rc;
}*/


string ZLog::GetSecTypeStr()
{
	return ZLog::GetSecTypeStr(m_secType);
}

void ZLog::CreateCurrentDayDirectories()
{
	char	CurrentDate[128];
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	sprintf(CurrentDate, "%04u%02u%02u", systime.wYear, systime.wMonth, systime.wDay);

	char secTypes[6] = {EQUITY, OPTION, FUTURE, CURRENCY, COMPLEX, '\0'};

	for (int i = 0; i < 5; i++)
	{
		string secTypeStr = GetSecTypeStr(secTypes[i]);

		std::stringstream stm1;
		stm1 << (CConfig::getInstance()).Drive() 
			 << "OATS";

		//CreateDirectory(stm1.str().c_str(), NULL);

		std::stringstream stm2;
		stm2 << stm1.str()
			 << "\\"
			 << secTypeStr;

		//CreateDirectory(stm2.str().c_str(), NULL);
		
		std::stringstream stm3;
		stm3 << stm2.str()
			 << "\\"
			 << CurrentDate;

		//CreateDirectory(stm3.str().c_str(), NULL);

		SHCreateDirectoryEx(NULL, stm3.str().c_str(), NULL);
		//std::stringstream stm;
		//stm << (CConfig::getInstance()).Drive() 
		//	<< "OATS"
		//	<< "\\"
		//	<< secTypeStr
		//	<< "\\"
		//	<< CurrentDate;

		//CreateDirectory(stm.str().c_str(), NULL);
	}
}

string ZLog::GetSecTypeStr(char type)
{
	string rc = "";

	switch(type)
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
	case COMPLEX:
		rc = "Complex";
		break;
	}

	return rc;
}
