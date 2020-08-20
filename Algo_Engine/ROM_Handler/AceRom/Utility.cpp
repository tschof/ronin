#include "StdAfx.h"
#include "Utility.h"
#include <sstream>
#include <iomanip>
#include <time.h>
//#include "Mailer.h"
#include "ROMMessage.h"
#include "RomValueDefs.h"
#include "Config.h"

CUtility::CUtility(void)
{
}

CUtility::~CUtility(void)
{
}


void CUtility::StrCpy(char* pDest, short len, const char* pSource)
{
	if (pDest && pSource)
	{
		if (len > strlen(pSource))
		{
			strcpy(pDest, pSource);
		}
		else
		{
			strncpy(pDest, pSource, len - 1);
			pDest[len - 1] = '\0';
		}
	}
}

std::string CUtility::GetGmtTimeStr()
{
	struct tm gmttime;
	_int64 ltime;

	_time64(&ltime);
	_gmtime64_s(&gmttime, &ltime);

	std::stringstream stm;
	stm << std::setw(4) << 1900 + gmttime.tm_year
		<< std::setw(2) << std::setfill('0') << gmttime.tm_mon+1
		<< std::setw(2) << std::setfill('0') << gmttime.tm_mday 
		<< "-" 
		<< std::setw(2) << std::setfill('0') << gmttime.tm_hour
		<< ":"
		<< std::setw(2) << std::setfill('0') << gmttime.tm_min
		<< ":"
		<< std::setw(2) << std::setfill('0') << gmttime.tm_sec;

	return stm.str();
}


std::string CUtility::GetLocalTimeStr()
{
	struct tm localtime;
	_int64 ltime;

	_time64(&ltime);
	_localtime64_s(&localtime, &ltime);

	std::stringstream stm;
	stm << std::setw(4) << 1900 + localtime.tm_year
		<< std::setw(2) << std::setfill('0') << localtime.tm_mon+1
		<< std::setw(2) << std::setfill('0') << localtime.tm_mday 
		<< "-" 
		<< std::setw(2) << std::setfill('0') << localtime.tm_hour
		<< ":"
		<< std::setw(2) << std::setfill('0') << localtime.tm_min
		<< ":"
		<< std::setw(2) << std::setfill('0') << localtime.tm_sec;

	return stm.str();
}

std::string CUtility::GetGmtTimeStrMM()
{
	SYSTEMTIME systime;
	GetSystemTime(&systime);

	char GmtTime[50];
	memset(GmtTime, '\0', 50);

	wsprintf(GmtTime, 
			 "%04u%02u%02u-%02u:%02u:%02u.%03u",                
			 systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond,
			 systime.wMilliseconds);

	return std::string(GmtTime);
}

std::string CUtility::GetOatsRecoverTimeStr()
{
	struct tm localtime;
	_int64 ltime;

	_time64(&ltime);
	_localtime64_s(&localtime, &ltime);

	std::stringstream stm;
	stm << std::setw(4) << 1900 + localtime.tm_year
		<< "/"
		<< std::setw(2) << std::setfill('0') << localtime.tm_mon+1
		<< "/"
		<< std::setw(2) << std::setfill('0') << localtime.tm_mday 
		<< " " 
		<< std::setw(2) << std::setfill('0') << localtime.tm_hour
		<< ":"
		<< std::setw(2) << std::setfill('0') << localtime.tm_min
		<< ":"
		<< std::setw(2) << std::setfill('0') << localtime.tm_sec
		<< " ";

	return stm.str();
}


std::string CUtility::GetTimehhmmss()
{
	struct tm tmstr;
	__time32_t ltime;

	_time32(&ltime);
	_localtime32_s(&tmstr, &ltime);

	std::stringstream stm;
	stm << std::setw(2) << std::setfill('0') << tmstr.tm_hour
		<< std::setw(2) << std::setfill('0') << tmstr.tm_min
		<< std::setw(2) << std::setfill('0') << tmstr.tm_sec;

	return stm.str();
}

std::string CUtility::GetTimeHHMMSS()
{
	struct tm tmstr;
	__time32_t ltime;

	_time32(&ltime);
	_localtime32_s(&tmstr, &ltime);

	std::stringstream stm;
	stm << std::setw(2) << std::setfill('0') << tmstr.tm_hour
		<< ":"
		<< std::setw(2) << std::setfill('0') << tmstr.tm_min
		<< ":"
		<< std::setw(2) << std::setfill('0') << tmstr.tm_sec;

	return stm.str();
}

/*
void CUtility::SendEmail(std::string& server, std::string& recipients, std::string& subject, std::string& msg)
{
    static SMTP::CMailer mailer(server, recipients, recipients);
	std::string sbj = subject + std::string("<") + CConfig::getInstance().InstanceName() + std::string(">");
    mailer.MailMessage(sbj, msg);
}*/


char CUtility::GetCmdFromMessage(const std::string& msg)
{
    int index = msg.find_first_of(ROMVALUE::DELIMITER);
	if (index > 0)
	{
		return msg[index - 1];
	}
	else
	{
		return ROMVALUE::CMD_EMPTY;
	}
}

void CUtility::RemoveMidNewlines(std::string& msg)
{
    std::string::iterator it = msg.begin();
    while (it != msg.end())
    {
        if (*it == '\n')
        {
            it = msg.erase(it);
        }
        else
        {
            it++;
        }
    }

    msg += "\n";
}

int CUtility::GetRomIDFromRomTag(const std::string& romTag)
{
	int index1 = romTag.find_last_of('-') - 8;
	int index2 = romTag.find_last_of('-', index1);
	int len = index1 - index2;
	std::string sub = romTag.substr(index2 + 1, len);
	return atoi(sub.c_str());
}


bool CUtility::LaterThanToday(const char* date)
{
	bool rc = false;
	std::string sDate = date;

	if (sDate.length() < 8)
	{
		return rc;
	}

	SYSTEMTIME systime;
    GetSystemTime(&systime);
	long today = systime.wYear *  10000 + systime.wMonth * 100 + systime.wDay;
	rc = today < atol(sDate.substr(0, 8).c_str());

	return rc;
}

std::string CUtility::GetStrPrice(double price, std::string delimiter)
{
	char buffer[50];
	sprintf(buffer, "%.2f", price);
	std::string sTemp = buffer;
	std::string sPrice = sTemp;
	std::string sub;
	int index = sPrice.size() - 6;

	if (index <= 0 ||
		(index == 1 && sPrice[0] == '-'))
	{
		return sPrice;
	}
	else
	{
		sPrice = "";
	}

	while (index > 0 && 
		   !(sTemp.size() == 4 && sTemp[0] == '-'))
	{
		sub = sTemp.substr(index);
		sPrice = delimiter + sub + sPrice;
		sTemp = sTemp.substr(0, index);
		index = sTemp.size() - 3;
	}

	return  sTemp + sPrice;
}

std::string CUtility::GetStrShares(long shares, std::string delimiter)
{
	char buffer[50];
	sprintf(buffer, "%ld", shares);
	std::string sTemp = buffer;
	std::string sShares = sTemp;
	std::string sub;
	int index = sShares.size() - 3;

	if (index <= 0 ||
		(index == 1 && sShares[0] == '-'))
	{
		return sShares;
	}
	else
	{
		sShares = "";
	}

	while (index > 0 && 
		   !(sTemp.size() == 4 && sTemp[0] == '-'))
	{
		sub = sTemp.substr(index);
		sShares = delimiter + sub + sShares;
		sTemp = sTemp.substr(0, index);
		index = sTemp.size() - 3;
	}

	return sTemp + sShares;
}

bool CUtility::GetField(char* pSource, int sourceSize, int csvTag, char* pTarget, int targetSize)
{
	char* pCur = strchr(pSource, ',');
	int tag = 0;
	bool rc = false;

	while (pCur != NULL && tag < csvTag)
	{
		pCur += 1;
		tag++;
		pCur = strchr(pCur, ',');
	}

	if (pCur != NULL)
	{
		pCur += 1;
		char* p = strchr(pCur, ',');
		if (NULL == p)
		{
			if (strlen(pCur) <= targetSize)
			{
				strcpy(pTarget, pCur);
				rc = false;
			}
		}
		else
		{
			if (p - pCur < targetSize)
			{
				strncpy(pTarget, pCur, p - pCur);
				rc = false;
			}
		}
	}
	
	return rc;
}