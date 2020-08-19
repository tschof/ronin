#ifndef _UTILITY_H__
#define _UTILITY_H__

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <vector>
#include <string>
#include <set>

#include "Csv.h"
#include "message_queue.h"
//#include "generic_db.h"
//#include "psql_db.h"
#include "ThreadSet.h"
#include "IovecStore.h"
#include "RomMapN.h"
#include "ConfigBase.h"


template<typename T>
struct theLess
{
    bool operator()(const T lhs, const T rhs)
    {
        return lhs < rhs;
    }
};

typedef struct
{
    unsigned short openTime;
    unsigned short closeTime;
    unsigned short resetWDay;
} tSC;



typedef struct TSERVERCONINFO
{
    time_t lastConTime;
    unsigned short resetDay;
} tServerConInfo;

typedef struct RUNKEY
{
    int fd;
    std::string fname;
} tRunKey;

class CUtility
{
public:
    CUtility(void);
    ~CUtility(void);

    static bool GetGmtTimeStr(char* pBuffer, unsigned int size);
    static time_t GetLocalTimeStr(char* pBuffer, unsigned int size, bool db = false);
    static time_t GetCentryTimeStr(char* pBuffer, unsigned int size, time_t t, bool db = false);
    static std::string GetLocalDayStr();
    static std::string GetGmtDayStr();
    static void GetLocalDateStr(std::string& nowStr);
    static void GetCentralDateStr(std::string& nowStr);
    static void GetCentralDateStrYYYYMMDD(std::string& nowStr);
    static void GetLocalYYYYMMDD(std::string& dStr, int days_from_today);
    //static std::string GetOatsRecoverTimeStr();
    static void SendTechEMail(std::string& subject, std::string& msg);
    static void SendBizEMail(std::string& subject, std::string& msg);
    static void SendTechEMail(std::string& subject, const char* pMsg, int len);
    static void SendBizEMail(std::string& subject, const char* pMsg, int len);
    static char GetCmdFromMessage(const std::string& msg);
    static void RemoveMidNewlines(std::string& msg);
    static std::string GetLocalTimeHHMMSS();
    static std::string GetLocalTimehhmmss();
    static void StrCpy(char* pDest, unsigned int len, const char* pSource);
    static int GetRomIDFromRomTag(const std::string& romTag);
    static void GetRomLoginFromRomTag(const std::string& romTag, std::string& romLogin);
    static void GetOwnerFromRomTag(const std::string& romTag, std::string& owner);
    static bool LaterThanToday(const char* date);
    static std::string GetStrPrice(double price, std::string delimiter = "*");
    static std::string GetStrShares(long shares, std::string delimiter = "*");

    static bool deletefile(const std::string& name);
    static bool fileExist(const std::string& name);
    static bool fileExist(const char* pName);
    static bool renamefile(const std::string& name, const std::string& newname);
    static unsigned short GetLocalTimeHHMM();
    static unsigned short GetCentralTimeHHMM();
    static unsigned short GetGmTimeHHMM();
    static unsigned short GetLocalTimeHH();
    static unsigned short GetTimeMM();
    static const std::string& ConfigFile()
    {
        return m_ConfigFile;
    }
    static void SetConfigFile(std::string modulName);
    static void spliter(const std::string& source, std::vector<std::string>& result, char delimeter);
    //static bool DbExec(const char* sqlStr, const char* dbserver, const char* dblogin, const char* dbpassword, queue_t* pQ);

    static bool IsFutureType(char secType);
    static bool IsOptionType(char secType);
    static bool IsCurrencyType(char secType);
    static bool IsEquityType(char secType);
    static bool IsComplexOrder(const CCsv& csv);
    static void Sleep(unsigned int s);
    static std::string DirSeparator();
    //static bool LoadNoneFirstClientLogons(const tDBLogin& dblogin, CThreadSet<std::string>& clients, const std::string& server, CDBExecutor& executor);
    //static bool LoadNoneFirstServerLogons(const tDBLogin& dblogin, CThreadSet<std::string>& servers, const std::string& client, CDBExecutor& executor);
    static bool LoadServerConInfo(CRomMap<tServerConInfo>& conInfo, const std::string& client, CDBExecutor& executor);
    //static bool LoadClientConInfo(const tDBLogin& dblogin, CRomMap<time_t>& logonInfo, const std::string& server, CDBExecutor& executor);

    //static void* UpdateClientLastLogon(void* pArg);
    //static bool DeleteClientLastLogons(const tDBLogin& dblogin, const std::string& server, CDBExecutor& executor);
    //static bool DeleteServerLastLogons(const tDBLogin& dblogin, const std::string& client, CDBExecutor& executor);
    static bool ftoA(double value, int precision, char* buffer, int len);
    static bool itoA(long value, char* buffer, int len);
    static unsigned short GetLocalTimeFromCentryTime(unsigned short centryTime);
    static unsigned short GetLocalWdayFromCentralWday(unsigned short centralDay);
    static unsigned short GetCentryTimeFromLocalTime(unsigned short localtime);
    static bool GetSecTypeStr(unsigned short secIndex, std::string& secStr);
    static bool GetDataBufFromIovec(tIovec* source, databuf_t* dest);

    static unsigned short GetDaysDiffFromLastWDay(unsigned short wToday, unsigned short lastWDay);
    static unsigned short GetDaysDiffFromNextWDay(unsigned short wToday, unsigned short nextWDay);
    static void AdjustToCentralTime(struct tm& tmlocalTime, struct tm& tmCentryTime);
    static void ConvertHourFromCentralToLocal(unsigned short cHour, unsigned short& lHour);
    //since what stored in db is in central time serverResetDay and serverOpenTime are expected in central time
    static bool IsRelogonToWeeklyServer(unsigned short serverResetDay, unsigned short serverOpenTime, time_t lastLogonTime);
    //static unsigned long SecondsToNextOpen(time_t now, unsigned short serverResetDay, unsigned short serverOpenTime);
    static time_t GetLastLogonTime(const std::string& client, const std::string& server, CDBExecutor& executor);
    static bool TrimExtraPennyDecimal(char* price);
    static bool TrimExtraPennyDecimal(char* price, double& outPrice);
    static bool IsEquityPriceDecimalValid(char* price, std::string& err);

    static bool GetCsvField(const char* source, int nField, std::string& value);
    static bool GetCsvField(const char* source, int nField, int& value);
    //static bool GetCsvField(const char* pSource, int csvTag, char* pTarget, int targetSize);

    static tRunKey* GetRunKey(const char* keypath, std::string& errstr);
    static void DestroyRunKey(tRunKey* pKey);
    static bool IsTheSessionOrder(unsigned short destOpenTime, unsigned short destCloseTime, unsigned short destResetDay, const char* orderCreateTime);
    static time_t GetTimeTFromCentryTime(const char* centryTime);  //yyyymmdd-hh:mm:ss for centryTime is assumed

    //sessionStartDateTime is local time and omTime is in GMT
    static bool IsCurrentSessionActivity(unsigned short sessionStartTime, const char* pOmTime);
    static bool GetPasswordFromDB(CDBExecutor& executor, const std::string login, std::string& password);

    //Format YYYY-MM-DAY-HH:MM:SS is expected for datetimeStr and local time is assumed
    static time_t GetSecondsFromDateTimeStr(const std::string& datetimeStr);
    static bool DayTurn_Central();
    static time_t BeginCentralDaySeconds();
    static void GetLocalDateTimeStr(std::string& dtStr);
    static void GetLocalYYYYMMDD(std::string& dStr);
    static bool CreateDirectories(const char* pFullPath);
    //static bool LoadDestIdToDestNameMap(CRomMap<std::string> & destMap, const tDBLogin& dblogin, CDBExecutor& executor);
    static long GetIntPrice(const std::string& sPrice);
    //Backward calculation
    static time_t GetTimeTB(unsigned short hour,
                           unsigned short min, unsigned short day); //hour/min/day is central time
    static bool IsNickelTickPrice(const std::string& price);
    static time_t GetSecondsToNow(const std::string& hh_mm);
    static time_t GetTimeT(const std::string& hh_mm);
    
    static bool IsValidDartSDouble(const std::string& sd);
    static void GetGmtTimeFromTimeval(std::string& buf, const struct timeval& tv);
    static void SendRollingLogMessage(const std::string& sender, const std::string& msg);

private:
    static std::string m_ConfigFile;
};

#endif //_UTILITY_H__
