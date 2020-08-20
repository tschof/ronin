#ifndef _UTILITY_H__
#define _UTILITY_H__

#pragma once

#include <string>

class CUtility
{
public:
	CUtility(void);
	~CUtility(void);

	static std::string GetGmtTimeStr();
	static std::string GetLocalTimeStr();
	static std::string CUtility::GetGmtTimeStrMM();
	static std::string GetOatsRecoverTimeStr();
    //static void SendEmail(std::string& server, std::string& recipients, std::string& subject, std::string& msg);
    static char GetCmdFromMessage(const std::string& msg);
    static void RemoveMidNewlines(std::string& msg);
	static std::string GetTimeHHMMSS();
	static std::string GetTimehhmmss();
	static void StrCpy(char* pDest, short len, const char* pSource);
	static int GetRomIDFromRomTag(const std::string& romTag);
	static bool LaterThanToday(const char* date);
	static std::string GetStrPrice(double price, std::string delimiter = "*");
	static std::string GetStrShares(long shares, std::string delimiter = "*");
	static bool GetField(char* pSource, int sourceSize, int csvTag, char* pTarget, int targetSize);
};

#endif //_UTILITY_H__