#include "Utility.h"
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include "romvaluedefs.h"
#include "ConfigBase.h"
#include "mailer.h"
#include "tm_to_str.h"
#include "Utility.h"
#include "DBParm.h"
#include "DBExecutor.h"
#include "debug_logger.h"
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include "client_manager.h"

static std::string s_SecStr[4] = {"Equity", "Option", "OptionOnFuture", "Future"};

std::string CUtility::m_ConfigFile;

static unsigned short LastWeekday_dif[7][7] = {
    { 7, 6, 5, 4, 3, 2, 1 },
    { 1, 7, 6, 5, 4, 3, 2 },
    { 2, 1, 7, 6, 5, 4, 3 },
    { 3, 2, 1, 7, 6, 5, 4 },
    { 4, 3, 2, 1, 7, 6, 5 },
    { 5, 4, 3, 2, 1, 7, 1 },
    { 6, 5, 4, 3, 2, 1, 7 }
};

static unsigned short NextWeekday_dif[7][7] = {
    { 7, 1, 2, 3, 4, 5, 6 },
    { 6, 7, 1, 2, 3, 4, 5 },
    { 5, 6, 7, 1, 2, 3, 4 },
    { 4, 5, 6, 7, 1, 2, 3 },
    { 3, 4, 5, 6, 7, 1, 2 },
    { 2, 3, 4, 5, 6, 7, 1 },
    { 1, 2, 3, 4, 5, 6, 7 }
};

unsigned short CUtility::GetDaysDiffFromLastWDay(unsigned short wToday, unsigned short lastWDay)
{
    return LastWeekday_dif[wToday][lastWDay];
}

unsigned short CUtility::GetDaysDiffFromNextWDay(unsigned short wToday, unsigned short nextWDay)
{
    return NextWeekday_dif[wToday][nextWDay];
}

CUtility::CUtility(void)
{

}

CUtility::~CUtility(void)
{
}

void CUtility::SetConfigFile(std::string modulName)
{
#ifdef _WIN32
    m_ConfigFile = modulName.substr(0, modulName.length() - 3) + std::string("dat");
#else
    m_ConfigFile = modulName + std::string(".dat");
#endif
}

void CUtility::StrCpy(char* pDest, unsigned int len, const char* pSource)
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

std::string CUtility::GetLocalDayStr()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    std::stringstream stm;
    stm << std::setw(4) << 1900 + Tm.tm_year
        << std::setw(2) << std::setfill('0') << Tm.tm_mon+1
        << std::setw(2) << std::setfill('0') << Tm.tm_mday ;

    return stm.str();
}

std::string CUtility::GetGmtDayStr()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    gmtime_r(&ltime, &Tm);

    std::stringstream stm;
    stm << std::setw(4) << 1900 + Tm.tm_year
        << std::setw(2) << std::setfill('0') << Tm.tm_mon+1
        << std::setw(2) << std::setfill('0') << Tm.tm_mday ;

    return stm.str();
}

void CUtility::GetLocalDateStr(std::string& nowStr)
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    char now[14];
    sprintf(now, "%04u%02u%02u-%02u%02u", Tm.tm_year + 1900, Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min);
    nowStr = now;
}

void CUtility::GetCentralDateStr(std::string& nowStr)
{
    struct tm TmLocal, TmCentry;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &TmLocal);
    AdjustToCentralTime(TmLocal, TmCentry);
    char now[14];
    sprintf(now, "%04u%02u%02u-%02u%02u", TmCentry.tm_year + 1900, TmCentry.tm_mon + 1, TmCentry.tm_mday, TmCentry.tm_hour, TmCentry.tm_min);
    nowStr = now;
}

void CUtility::GetCentralDateStrYYYYMMDD(std::string& nowStr)
{
    std::string daystr;
    GetCentralDateStr(daystr);
    int index = daystr.find('-');
    nowStr = daystr.substr(0, index);
    nowStr.append(daystr.substr(index));
}

time_t CUtility::GetLocalTimeStr(char* pBuffer, unsigned int size, bool db)
{
    time_t rc = true;

    if (size < 18)
    {
        rc = 0;
    }
    else
    {
        *(pBuffer + 17) = '\0';

        struct tm Tm;
        time_t ltime;

        time(&rc);
        localtime_r(&ltime, &Tm);
        tm_to_str(&Tm, pBuffer, size);

        if (db)
        {
            *(pBuffer + 8) = ' ';
        }
    }

    return rc;
}

/*bool CUtility::IsRelogonToWeeklyServer(const tServerConInfo& conInfo, time_t now, const struct tm& tmNow, const std::string openTime)
{
    bool rc = false;
    int opent = atoi(openTime.c_str());

    int timeDiff = (tmNow.tm_hour * 60 + tmNow.tm_min) * 60 + tmNow.tm_sec - ((opent/100) * 60 + opent % 100) * 60;
    unsigned short wdayDiff = (conInfo.resetDay == 7)? 0 : GetWeedDayDiff(conInfo.resetDay, tmNow.tm_wday);
    time_t lastOpenTime = now - (wdayDiff * 86400 + timeDiff);

    if (conInfo.lastConTime >= lastOpenTime)
    {
        rc = true;
    }

    return rc;
}*/

bool CUtility::IsRelogonToWeeklyServer(unsigned short serverResetDay, unsigned short serverOpenTime, time_t lastLogonTime)
{
    bool rc = false;
    time_t now = 0;
    struct tm tmNow, tmCentralTime;
    time(&now);
    localtime_r(&now, &tmNow);

    //remember that both serverResetDay and serverOpenTime are central time!!!
    AdjustToCentralTime(tmNow, tmCentralTime);

    int timeDiff = (tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60 + tmCentralTime.tm_sec - ((serverOpenTime/100) * 60 + serverOpenTime % 100) * 60;
    unsigned short wdayDiff = (serverResetDay == 7)? 0 : GetDaysDiffFromLastWDay(tmCentralTime.tm_wday, serverResetDay);

    if (timeDiff < 0 &&
        wdayDiff == 0)
    {
        timeDiff += 86400; //cross day session compensation for nonweekly reset
    }

    time_t lastOpenTime = now - (wdayDiff * 86400 + timeDiff);

    if (lastLogonTime >= lastOpenTime)
    {
        rc = true;
    }

    return rc;
}

/*
unsigned long CUtility::SecondsToNextOpen(time_t now, unsigned short serverResetDay, unsigned short serverOpenTime)
{
    unsigned long rc = 0;

    struct tm tmNow, tmCentralTime;
    localtime_r(&now, &tmNow);
    //remember that both serverResetDay and serverOpenTime are central time!!!
    AdjustToCentralTime(tmNow, tmCentralTime);

    int timeDiff = ((serverOpenTime/100) * 60 + serverOpenTime % 100) * 60 - tmCentralTime.tm_sec - (tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60;

    unsigned short daysDiff = (serverResetDay == 7)? 0 : GetDaysDiffFromNextWDay(tmCentralTime.tm_wday, serverResetDay);

    if (daysDiff == 0)
    {
        rc = timeDiff;
        if (timeDiff < 0)
        {
            rc += 86400;
        }
    }
    else
    {
        if (daysDiff == 7)
        {
            rc = timeDiff;

            if (timeDiff < 0)
            {
                rc += daysDiff * 86400;
            }
        }
        else
        {
            rc = daysDiff * 86400 + timeDiff;
        }
    }

    return rc;
}
*/
void CUtility::AdjustToCentralTime(struct tm& tmlocalTime, struct tm& tmCentryTime)
{
    tmlocalTime.tm_isdst = -1;
    tmCentryTime = tmlocalTime;
    char buffer[10] = {0};
    int nwritten = strftime(buffer, sizeof(buffer), "%Z", &tmCentryTime);
    if (nwritten == 3 &&
        (buffer[1] == 'S' || buffer[1] == 'D') &&
         buffer[2] == 'T')
    {
        switch (buffer[0])
        {
        case 'E':
            tmCentryTime.tm_hour -= 1;
            break;
        case 'M':
            tmCentryTime.tm_hour += 1;
            break;
        case 'P':
            tmCentryTime.tm_hour += 2;
            break;
        }
    }

    mktime(&tmCentryTime);
}

void CUtility::ConvertHourFromCentralToLocal(unsigned short cHour, unsigned short& lHour)
{
    short hour = cHour;
    char buffer[10] = {0};
    time_t dummyNow = 0;
    struct tm tmDummyNow;
    time(&dummyNow);
    localtime_r(&dummyNow, &tmDummyNow);
    int nwritten = strftime(buffer, sizeof buffer, "%Z", &tmDummyNow);
    if (nwritten == 3 &&
        (buffer[1] == 'S' || buffer[1] == 'D') &&
         buffer[2] == 'T')
    {
        switch (buffer[0])
        {
        case 'E':
            hour = (23 == hour)? 0 : hour + 1;
            break;
        case 'M':
            hour = (0 == hour)? 23 : hour - 1;
            break;
        case 'P':
            hour = (hour < 2)? hour + 22 : hour - 2;
            break;
        }
    }

    lHour = hour;
}

time_t CUtility::GetTimeTFromCentryTime(const char* centryTime)
{
    struct tm tmLocal, tmTemp;

    char buf[5];
    memset(buf, '\0', sizeof(buf));
    strncpy(buf, centryTime + 4, 2);
    tmLocal.tm_mon = atoi(buf) - 1;
    strncpy(buf, centryTime + 6, 2);
    tmLocal.tm_mday = atoi(buf);
    strncpy(buf, centryTime + 9, 2);
    tmLocal.tm_hour = atoi(buf);
    strncpy(buf, centryTime + 12, 2);
    tmLocal.tm_min = atoi(buf);
    strncpy(buf, centryTime + 15, 2);
    tmLocal.tm_sec = atoi(buf);
    strncpy(buf, centryTime, 4);
    tmLocal.tm_year = atoi(buf) - 1900;
    tmLocal.tm_isdst = -1;

    time_t ltime;
    time(&ltime);
    localtime_r(&ltime, &tmTemp);

    char buffer[10] = {0};
    int nwritten = strftime(buffer, sizeof(buffer), "%Z", &tmTemp);
    if (nwritten == 3 &&
        (buffer[1] == 'S' || buffer[1] == 'D') &&
         buffer[2] == 'T')
    {
        switch (buffer[0])
        {
        case 'E':
            tmLocal.tm_hour += 1;
            break;
        case 'M':
            tmLocal.tm_hour -= 1;
            break;
        case 'P':
            tmLocal.tm_hour -= 2;
            break;
        }
    }

    return mktime(&tmLocal);
}

time_t CUtility::GetCentryTimeStr(char* pBuffer, unsigned int size, time_t t, bool db)
{
    time_t rc = 0;

    if (size < 18)
    {
        rc = 0;
    }
    else
    {
        *(pBuffer + 17) = '\0';

        struct tm Tm;

        if (t != 0)
        {
            rc = t;
        }
        else
        {
            time(&rc);
        }

        localtime_r(&rc, &Tm);

        char buffer[10] = {0};
        int nwritten = strftime(buffer, sizeof buffer, "%Z", &Tm);
        if (nwritten == 3 &&
            (buffer[1] == 'S' || buffer[1] == 'D') &&
             buffer[2] == 'T')
        {
            switch (buffer[0])
            {
            case 'E':
                Tm.tm_hour -= 1;
                break;
            case 'M':
                Tm.tm_hour += 1;
                break;
            case 'P':
                Tm.tm_hour += 2;
                break;
            }
        }

        //for adjustment purpose
        mktime(&Tm);
        tm_to_str(&Tm, pBuffer, size);

        if (db)
        {
            *(pBuffer + 8) = ' ';
        }
    }

    return rc;
}

bool CUtility::GetGmtTimeStr(char* pBuffer, unsigned int size)
{
    bool rc = true;

    if (size < 22)
    {
        rc = false;
    }
    else
    {
        *(pBuffer + 21) = '\0';

        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm Tm;
        gmtime_r(&(tv.tv_sec), &Tm);

        tm_to_str_millis(&Tm, tv.tv_usec / 1000, pBuffer, 21);
        //tm_to_str_micros(&Tm, tv.tv_usec, pBuffer, size);
    }

    return rc;
}

void CUtility::GetGmtTimeFromTimeval(std::string& buf, const struct timeval& tv)
{
    struct tm Tm;
    gmtime_r(&(tv.tv_sec), &Tm);
    
    char l_buf[22];
    tm_to_str_millis(&Tm, tv.tv_usec / 1000, l_buf, 21);
    buf.assign(l_buf);
}

std::string CUtility::GetLocalTimehhmmss()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    std::stringstream stm;
    stm << std::setw(2) << std::setfill('0') << Tm.tm_hour
        << std::setw(2) << std::setfill('0') << Tm.tm_min
        << std::setw(2) << std::setfill('0') << Tm.tm_sec;

    return stm.str();
}

std::string CUtility::GetLocalTimeHHMMSS()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    std::stringstream stm;
    stm << std::setw(2) << std::setfill('0') << Tm.tm_hour
        << ":"
        << std::setw(2) << std::setfill('0') << Tm.tm_min
        << ":"
        << std::setw(2) << std::setfill('0') << Tm.tm_sec;

    return stm.str();
}

void CUtility::SendTechEMail(std::string& subject, std::string& msg)
{
    send_tech_message(msg.c_str(), msg.size(), subject.c_str(), subject.size());
}

void CUtility::SendBizEMail(std::string& subject, std::string& msg)
{
    send_biz_message(msg.c_str(), msg.size(), subject.c_str(), subject.size());
}

void CUtility::SendTechEMail(std::string& subject, const char* pMsg, int len)
{
    send_tech_message(pMsg, len, subject.c_str(), subject.size());
}

void CUtility::SendBizEMail(std::string& subject, const char* pMsg, int len)
{
    send_biz_message(pMsg, len, subject.c_str(), subject.size());
}


char CUtility::GetCmdFromMessage(const std::string& msg)
{
    int index = msg.find_first_of(ROMFIELDS::DELIMITER);
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

void CUtility::GetRomLoginFromRomTag(const std::string& romTag, std::string& romLogin)
{
    int index1 = romTag.find_last_of('-') - 8;
    int index2 = romTag.find_last_of('-', index1);
    int len = index1 - index2;
    romLogin = "ROM";
    romLogin.append(romTag.substr(index2 + 1, len));
}

void CUtility::GetOwnerFromRomTag(const std::string& romTag, std::string& owner)
{
    int index = romTag.find_first_of('-');
    owner = romTag.substr(0, index);
}

bool CUtility::LaterThanToday(const char* date)
{
    bool rc = false;
    std::string sDate = date;

    if (sDate.length() < 8)
    {
        return rc;
    }

    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    long today = (Tm.tm_year + 1900) * 1000 + (Tm.tm_mon + 1) * 100 + Tm.tm_mday;
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

/*bool CUtility::GetCsvField(const char* pSource, int csvTag, char* pTarget, int targetSize)
{
    char* pCur = strchr(const_cast<char*>(pSource), ',');
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
            if (strlen(pCur) <= targetSize - 1)
            {
                strcpy(pTarget, pCur);
                rc = true;
            }
        }
        else
        {
            if (p - pCur < targetSize - 1)
            {
                strncpy(pTarget, pCur, p - pCur);
                char* pTr = pTarget;
                pTr += p - pCur;
                *pTr = '\0';
                rc = true;
            }
        }
    }

    return rc;
}


bool CUtility::GetCsvField(const char* pSource, int csvTag, std::string& target)
{
    char field[256];

    bool rc = GetCsvField(pSource, csvTag, field, sizeof(field));
    if (rc)
    {
        target = field;
    }

    return rc;
}
*/

bool CUtility::fileExist(const std::string& name)
{
    return  fileExist(name.c_str());
}

bool CUtility::fileExist(const char* pName)
{
    bool rc = true;
    struct stat Stat;

    if (stat(pName, &Stat) != 0)
    {
        char* pErrStr = strerror(errno);
        printf(pErrStr);
        rc = false;
    }
    
    return rc;
}

bool CUtility::deletefile(const std::string& name)
{
    bool rc = true;

    if (fileExist(name))
    {
        if (unlink(name.c_str()) == -1)
        {
            char* pErrStr = strerror(errno);
            printf(pErrStr);
            rc = false;
        }
    }
    else
    {
        rc = false;
    }

    return rc;
}

bool CUtility::renamefile(const std::string& name, const std::string& newname)
{
    bool rc = true;
#ifdef _WIN32
    rc = MoveFile(name.c_str(), newname.c_str());
#else
    if (rename(name.c_str(), newname.c_str()) == -1)
    {
        rc = false;
    }
#endif
    return rc;
}

std::string CUtility::DirSeparator()
{
    std::string ret;

#ifdef _WIN32
    ret = "\\";
#else
    ret = "/";
#endif

    return ret;
}



unsigned short CUtility::GetLocalTimeHHMM()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    return Tm.tm_hour * 100 + Tm.tm_min;
}

unsigned short CUtility::GetCentralTimeHHMM()
{
    unsigned short localHHMM = GetLocalTimeHHMM();
    return GetCentryTimeFromLocalTime(localHHMM);
}

unsigned short CUtility::GetGmTimeHHMM()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    gmtime_r(&ltime, &Tm);

    return Tm.tm_hour * 100 + Tm.tm_min;
}

unsigned short CUtility::GetLocalTimeHH()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    return Tm.tm_hour;
}

unsigned short CUtility::GetTimeMM()
{
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    return Tm.tm_min;
}

void CUtility::spliter(const std::string& source, std::vector<std::string>& result, char delimeter)
{
    std::string substring;
    std::size_t index = source.find(delimeter);
    unsigned int curIndex = 0, len = 0;

    while (index != std::string::npos)
    {
        len = index - curIndex;
        substring = source.substr(curIndex, len);
        if (substring.empty() == false)
        {
            result.push_back(substring);
        }
        curIndex = index + 1;
        if (curIndex >= source.size())
        {
            break;
        }
        index = source.find(delimeter, curIndex);
    }

    if (curIndex < source.size())
    {
        substring = source.substr(curIndex);
        result.push_back(substring);
    }
}

/*
bool CUtility::DbExec(const char* sqlStr, const char* dbserver, const char* dblogin, const char* dbpassword, queue_t* pQ)
{
    bool rc = false;

    int rt = gendb_get_results( dblogin,
                                strlen(dblogin),
                                dbpassword,
                                strlen(dbpassword),
                                dbserver,
                                strlen(dbserver),
                                *pQ,
                                sqlStr);

    if (!rt)
    {
        rc = true;
    }

    return rc;
}*/

bool CUtility::IsOptionType(char secType)
{
    if (secType == ROMVALUE::SECTYPE_OPTION ||
        secType == ROMVALUE::SECTYPE_OPTIONINDEX ||
        secType == ROMVALUE::SECTYPE_OPTIONFUTURE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CUtility::IsFutureType(char secType)
{
    if (secType == ROMVALUE::SECTYPE_FUTURE ||
            secType == ROMVALUE::SECTYPE_FUTUREEQUITY ||
            secType == ROMVALUE::SECTYPE_FUTURECURRENCY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CUtility::IsCurrencyType(char secType)
{
    if (secType == ROMVALUE::SECTYPE_CURRENCY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CUtility::IsEquityType(char secType)
{
    if (secType == ROMVALUE::SECTYPE_EQUITY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CUtility::IsComplexOrder(const CCsv& csv)
{
    bool rc = false;
    int cplxtype = 0;
    csv.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);
    if (csv.IsFieldEmpty(ROMFIELDS::CSV_PARENT_STAFFORD_TAG) == false ||
            cplxtype != ROMVALUE::CPLXTYPE_SINGLE)
    {
        rc = true;
    }

    return rc;
}

void CUtility::Sleep(unsigned int s)
{
    sleep(s);
}

/*bool CUtility::DeleteClientLastLogons(const tDBLogin& dblogin, const std::string& server, CDBExecutor& executor)
{
    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr, "delete from %s.dbo.ClientLastLogon where server = '%s'", dblogin.database.c_str(), server.c_str());
    return executor.Execute(sqlStr, NULL, 0, results, errStr);
}

bool CUtility::DeleteServerLastLogons(const tDBLogin& dblogin, const std::string& rom, CDBExecutor& executor)
{
    bool rc = false;

    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;

    sprintf(sqlStr, "delete from %s.dbo.ClientLastLogon where login = '%s' and server not like '%s'", dblogin.database.c_str(), rom.c_str(), "OATS%");

    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {
        rc = true;
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);

    return rc;
}*/

/*void* CUtility::UpdateClientLastLogon(void* pArg)
{
    queue_t results = create_queue();
    std::string procName = "UpdateClientLastLogon";
    std::string errStr;

    //CUtility::GetLocalTimeStr(curDateTime, 18, true);
    tLogonUpdate* pLogonUpdate = (tLogonUpdate*)pArg;
    CDBParm parms[3];
    gendb_parm* parms_ptr[3];

    parms[0].Set("@login", pLogonUpdate->login);
    parms_ptr[0] = parms[0].parm();

    parms[1].Set("@server", pLogonUpdate->server);
    parms_ptr[1] = parms[1].parm();

    parms[2].Set("@logon_time_t", (int)pLogonUpdate->logontime_t);
    parms_ptr[2] = parms[2].parm();

    CDBExecutor* executor = pLogonUpdate->executor;
    executor->Execute(procName.c_str(),
                     parms_ptr,
                     3,
                     results,
                     errStr);

    //gendb_exec( pLogonUpdate->dbCon,
    //            procName.c_str(),
    //            parms_ptr,
    //            3,
    //            results,
    //            errStr,
    //            sizeof(errStr));

    destroy_queue(results);

    free(pLogonUpdate);
    return NULL;
}

bool CUtility::LoadNoneFirstClientLogons(const tDBLogin& dblogin, CThreadSet<std::string>& clients, const std::string& server, CDBExecutor& executor)
{
    bool rc = false;

    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr, "select login from %s.dbo.ClientLastLogon where server = '%s'", dblogin.database.c_str(), server.c_str());

    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {
    //if (CUtility::DbExec(sqlStr, dblogin.dbserver.c_str(), dblogin.user.c_str(), dblogin.password.c_str(), &results))
    //{
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);
                if (pVal->arr_len)
                {
                    std::string client((pVal->g_un).un_array, pVal->arr_len);
                    clients.Add(client);
                }
                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            free(rowE);
            destroy_queue(row);
        }

        rc = true;
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);

    return rc;
}

bool CUtility::LoadNoneFirstServerLogons(const tDBLogin& dblogin, CThreadSet<std::string>& servers, const std::string& client, CDBExecutor& executor)
{
    bool rc = false;

    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr, "select server from %s.dbo.ClientLastLogon where login = '%s' and server not like '%s'", dblogin.database.c_str(), client.c_str(), "OATS%");

    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {
    //if (CUtility::DbExec(sqlStr, dblogin.dbserver.c_str(), dblogin.user.c_str(), dblogin.password.c_str(), &results))
    //{
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);
                if (pVal->arr_len)
                {
                    std::string server;
                    server.assign((pVal->g_un).un_array, pVal->arr_len);
                    servers.Add(server);
                }
                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            free(rowE);
            destroy_queue(row);
        }

        rc = true;
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);

    return rc;
}*/

bool CUtility::LoadServerConInfo(CRomMap<tServerConInfo>& conInfo, const std::string& client, CDBExecutor& executor)
{
    bool rc = false;

    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr,
            "select A.server, A.logon_time_t, B.reset_wday from ClientLastLogon A, OMDestination B where A.server = (B.DestID)::VARCHAR(30) and A.login = '%s'",
             client.c_str());
        
    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {       
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
                
        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            std::string server;
            int cnt = 0;
            tServerConInfo info = {0, 7};

            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                switch(cnt)
                {
                case 0:
                    if (pVal->arr_len)
                    {
                        server.assign((pVal->g_un).un_array, pVal->arr_len);
                    }
                    break;
                case 1:
                    if (server.length())
                    {
                        info.lastConTime = (pVal->g_un).un_int;
                    }
                    break;
                case 2:
                    if (server.length())
                    {
                        info.resetDay = (pVal->g_un).un_int;
                    }
                    break;
                }

                cnt++;
                //servers.Add((pVal->g_un).un_array);
                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            if (server.length())
            {
                conInfo.Add(server, info);
            }
            free(rowE);
            destroy_queue(row);
        }

        rc = true;
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);

    return rc;
}

//bool CUtility::LoadClientConInfo(const tDBLogin& dblogin, CRomMap<time_t>& logonInfo, const std::string& server)
/*bool CUtility::LoadClientConInfo(const tDBLogin& dblogin, CRomMap<time_t>& logonInfo, const std::string& server, CDBExecutor& executor)
{
    bool rc = false;

    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr,
            "select login, logon_time_t from %s.dbo.ClientLastLogon where server = '%s'",
            dblogin.database.c_str(),
            server.c_str());

    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {
    //if (CUtility::DbExec(sqlStr, dblogin.dbserver.c_str(), dblogin.user.c_str(), dblogin.password.c_str(), &results))
    //{
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            std::string client;
            time_t t = 0;
            int cnt = 0;

            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                switch(cnt)
                {
                case 0:
                    if (pVal->arr_len)
                    {
                        client.assign((pVal->g_un).un_array, pVal->arr_len);
                    }
                    break;
                case 1:
                    t = (pVal->g_un).un_int;
                    break;
                }

                cnt++;
                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            logonInfo.Add(client, t);
            free(rowE);
            destroy_queue(row);
        }

        rc = true;
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);

    return rc;
}*/

time_t CUtility::GetLastLogonTime(const std::string& login, const std::string& server, CDBExecutor& executor)
{
    time_t rc = 0;

    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr,
            "select logon_time_t from ClientLastLogon where login = '%s' and server = '%s'",
            login.c_str(),
            server.c_str());

    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {
    //if (CUtility::DbExec(sqlStr, dblogin.dbserver.c_str(), dblogin.user.c_str(), dblogin.password.c_str(), &results))
    //{
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            if (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);
                rc = (pVal->g_un).un_int;

                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
                free(rowE);
                destroy_queue(row);

                break;
            }
            else
            {
                free(rowE);
                destroy_queue(row);
            }
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);

    return rc;
}


bool CUtility::ftoA(double value, int precision, char* buffer, int len)
{
    if (precision > 8)
    {
        precision = 8;
    }

    char l_buffer[100];
    int i = 0;
    char zero = '0';
    bool negative = false;

    char* p = buffer;

    if (value < 0)
    {
        negative = true;
        value *= -1;
    }

    unsigned long integer = value / 1;
    unsigned long multiplier = pow(10, precision + 1);
    unsigned long decimal = multiplier * (value - integer);
    decimal += 1;
    decimal = decimal / 10;

    for (i = 0; i < precision; i++)
    {
        l_buffer[i] = zero + decimal % 10;
        decimal /= 10;
    }

    l_buffer[i] = '.';
    i++;

    if (integer == 0)
    {
        l_buffer[i] = '0';
        i++;
    }
    else
    {
        for (; integer > 0; i++)
        {
            l_buffer[i] = zero + integer % 10;
            integer /= 10;
        }
    }

    if (negative)
    {
        l_buffer[i]  = '-';
    }
    else
    {
        i--;
    }

    if (i >= len)
    {
        return false;
    }

    for (int k = 0; k <= i; k++)
    {
        *p = l_buffer[i - k];
        p++;
    }

    //if (precision < 8)
    //{
    //    p = &(buffer[i - 7 + precision]);
    //}

    *p = '\0';

    return true;
}

bool CUtility::itoA(long value, char* buffer, int len)
{
    int i = 0;
    char zero = '0';
    char l_buffer[100];
    bool negative = false;

    if (0 == value)
    {
        buffer[0] = zero;
        buffer[1] = '\0';
        return true;
    }
    if (value < 0)
    {
        negative = true;
        value = labs(value);
    }

    for (; value > 0; i++)
    {
        l_buffer[i] = zero + value % 10;
        value /= 10;
    }

    if (negative)
    {
        l_buffer[i] = '-';
    }
    else
    {
        i--;
    }

    if (len <= i)
    {
        return false;
    }

    int k = 0;
    for (; k <= i; k++)
    {
        buffer[k] = l_buffer[i - k];
    }

    buffer[k] = '\0';

    return true;
}

unsigned short CUtility::GetLocalTimeFromCentryTime(unsigned short centryTime)
{
    short localTime = centryTime;

    time_t t = time(0);
    struct tm Tm;
    if (localtime_r(&t, &Tm))
    {
        char buffer[10] = {0};
        int nwritten = strftime(buffer, sizeof buffer, "%Z", &Tm);
        if (nwritten == 3 && (buffer[1] == 'S' || buffer[1] == 'D')
                && buffer[2] == 'T')
        {
            switch (buffer[0])
            {
            case 'E':
                localTime += 100;
                break;
            case 'C':
                //offset = 0;
                break;
            case 'M':
                localTime -= 100;
                break;
            case 'P':
                localTime -= 200;
                break;
            }
        }
    }

    //handling crossing day adjustment
    if (localTime == 2400)
    {
        localTime = 0;
    }
    else if (localTime > 2400)
    {
        localTime -= 2400;
    }
    else if (localTime < 0)
    {
        localTime += 2400;
    }

    return localTime;
}

unsigned short CUtility::GetCentryTimeFromLocalTime(unsigned short localtime)
{
    short centrytime = localtime;

    time_t t = time(0);
    struct tm Tm;
    if (localtime_r(&t, &Tm))
    {
        char buffer[10] = {0};
        int nwritten = strftime(buffer, sizeof buffer, "%Z", &Tm);
        if (nwritten == 3 && (buffer[1] == 'S' || buffer[1] == 'D')
                && buffer[2] == 'T')
        {
            switch (buffer[0])
            {
            case 'E':
                centrytime -= 100;
                break;
            case 'M':
                centrytime += 100;
                break;
            case 'P':
                centrytime += 200;
                break;
            }
        }
    }

    //handling crossing day adjustment
    if (centrytime == 2400)
    {
        centrytime = 0;
    }
    else if (centrytime > 2400)
    {
        centrytime -= 2400;
    }
    else if (centrytime < 0)
    {
        centrytime += 2400;
    }

    return centrytime;
}


bool CUtility::GetSecTypeStr(unsigned short secIndex, std::string& secStr)
{
    bool rc = false;

    if (secIndex < 4)
    {
        secStr = s_SecStr[secIndex];
        rc = true;
    }
    /*
    if (CUtility::IsEquityType(secType))
    {
        secStr = "Equity";
    }
    else if (CUtility::IsFutureType(secType))
    {
        secStr = "Future";
    }
    else if (CUtility::IsOptionType(secType))
    {
        if (ROMVALUE::SECTYPE_OPTIONFUTURE == secType)
        {
            secStr = "OptionOnFuture";
        }
        else
        {
            secStr = "Option";
        }
    }
    else if (CUtility::IsCurrencyType(secType))
    {
        secStr = "Currency";
    }
*/
    return rc;
}

bool CUtility::GetDataBufFromIovec(tIovec* source, databuf_t* dest)
{
    for (size_t i = 0; i < source->size; i++)
    {
        databuf_memcpy(dest, (source->vec)[i].iov_base, (source->vec)[i].iov_len);
    }

    return true;
}

bool CUtility::TrimExtraPennyDecimal(char* price)
{
    bool rc = false;

    if (price)
    {
        rc = true;
        char *pDeci = strchr(price, '.');
        if (pDeci != NULL)
        {
            char* pP = price;
            bool underDollar = true;

            while (pP != pDeci)
            {
                if ( *pP >= 49 && *pP <= 57)
                {
                    underDollar = false;
                    break;
                }
                pP++;
            }
            unsigned long cutoff_l = 0;
            if (underDollar)
            {
                cutoff_l = pDeci - price + 5;
            }
            else
            {
                cutoff_l = pDeci - price + 3;
            }
            if (strlen(price) > cutoff_l)
            {
                price[cutoff_l] = '\0';
            }
        }
    }

    return rc;
}

bool CUtility::TrimExtraPennyDecimal(char* price, double& outPrice)
{
    bool rc = TrimExtraPennyDecimal(price);

    if (rc)
    {
        outPrice = atof(price);
    }

    return rc;
}

bool CUtility::IsEquityPriceDecimalValid(char* price, std::string& err)
{
    bool rc = true;

    if (price)
    {
        rc = true;
        size_t price_len = strlen(price);

        char *pDeci = strchr(price, '.');
        if (pDeci != NULL)
        {
            char* pP = price;
            bool underDollar = true;

            while (pP != pDeci)
            {
                if ( *pP >= 49 && *pP <= 57)
                {
                    underDollar = false;
                    break;
                }
                pP++;
            }

            unsigned long cutoff_l = 0;
            if (underDollar)
            {
                cutoff_l = pDeci - price + 5;
            }
            else
            {
                cutoff_l = pDeci - price + 3;
            }

            if (price_len > cutoff_l)
            {
                for (size_t i = cutoff_l; i < price_len; i++)
                {
                    if (price[i] != 48)
                    {
                        if (underDollar)
                        {
                            err = "under dollar stock price can not have more than four decimals";
                        }
                        else
                        {
                            err = "above dollar stock price can not have more than two decimals";
                        }

                        rc = false;
                        break;
                    }
                }

                if (rc)
                {
                    price[cutoff_l] = '\0';
                }
            }
        }
    }

    return rc;
}

bool CUtility::GetCsvField(const char* source, int nField, std::string& value)
{
    char* pTmp = NULL, *pStart = const_cast<char*>(source);
    pTmp = pStart;
    int cnt = 0;
    int len = strlen(pStart);
    bool hit = false;

    if (nField == 0)
    {
        if (len > 0)
        {
            if ((pTmp = strchr(pTmp, ',')) != NULL)
            {
                value.assign(pStart, pTmp - pStart);
                pTmp = NULL;
            }
            else
            {
                value = pStart;
            }

            hit = true;
        }
    }
    else
    {
        while ((pTmp = strchr(pTmp, ',')) != NULL)
        {
            cnt++;

            if (cnt == nField)
            {
                hit = true;
                break;
            }
            else
            {
                if ((pTmp - pStart + 1) < len)
                {
                    pTmp++;
                }
            }
        }
    }

    if (pTmp != NULL &&
        true == hit)
    {
        if ((pTmp - pStart + 1) < len)
        {
            pTmp++;
            char* pStr = pTmp;
            if ((pTmp = strchr(pTmp, ',')) != NULL)
            {
                value.assign(pStr, pTmp - pStr);
            }
            else
            {
                value = pStr;
            }
        }
    }

    if (hit && value[value.length() - 1] == ROMVALUE::MESSAGE_END)
    {
        value = value.substr(0, value.length() - 1);
    }
    return hit;
}

bool CUtility::GetCsvField(const char* source, int nField, int& value)
{
    std::string sValue;
    bool rc = GetCsvField(source, nField, sValue);
    if (true == rc)
    {
        value = atoi(sValue.c_str());
    }
    return rc;
}

tRunKey* CUtility::GetRunKey(const char* keypath, std::string& errstr)
{
    tRunKey* pKey = NULL;

    std::string tempfile = keypath;
    tempfile.append("_tmp");
    int fd = open(tempfile.c_str(), O_RDWR | O_CREAT, S_IRWXU);

    if (fd == -1)
    {
        char* pErrStr = strerror(errno);
        errstr = "open() in GetRunKey() failed:";
        errstr.append(pErrStr);
    }
    else
    {
        if (lockf(fd, F_TLOCK, 0) == -1)
        {
            char errStr[128];
            memset(errStr, 0, sizeof(errStr));

            if (errno == EACCES ||
                errno == EAGAIN) //"man lockf" tells why
            {
                strcpy(errStr, "Another instance is already running\n");
            }
            else
            {
                char* pErrStr = strerror(errno);
                strncpy(errStr, pErrStr, sizeof(errStr));
            }

            errstr = "flock() in GetRunKey() failed:";
            errstr.append(errStr);
            close(fd);
        }
        else
        {
            pKey = new tRunKey;
            pKey->fd = fd;
            pKey->fname = tempfile;
        }
    }

    return pKey;
}

void CUtility::DestroyRunKey(tRunKey* pKey)
{
    //lockf(pKey->fd, LOCK_UN, 0);
    if (lockf(pKey->fd, F_ULOCK, 0) != 0)
    {
        char* pErrStr = strerror(errno);
        printf(pErrStr);
    }
   
    if (close(pKey->fd) != 0)
    {
        char* pErrStr = strerror(errno);
        printf(pErrStr);
    }
    
    deletefile(pKey->fname);
}

bool CUtility::IsTheSessionOrder(unsigned short destOpenTime, unsigned short destCloseTime, unsigned short destResetDay, const char* orderCreateTime)
{
    //yyyymmdd-hh:mm:ss for centryTime is assumed and
    //destOpenTime/destClose are centryTime in hhmm format
    bool rc = false;
    time_t orderTime = GetTimeTFromCentryTime(orderCreateTime);
    time_t destLastOpenTime = 0, destLastCloseTime = 0;

    time_t now = 0;
    struct tm tmNow, tmCentralTime;
    time(&now);
    localtime_r(&now, &tmNow);

    //remember that both destResetDay and destOpenTime are central time!!!
    AdjustToCentralTime(tmNow, tmCentralTime);

    int openDiff = (tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60 + tmCentralTime.tm_sec - ((destOpenTime/100) * 60 + destOpenTime % 100) * 60;
    int closeDiff = ((destCloseTime/100) * 60 + destCloseTime % 100) * 60 - (tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60 + tmCentralTime.tm_sec;

    unsigned short wdayDiff = (destResetDay == 7)? 0 : GetDaysDiffFromLastWDay(tmCentralTime.tm_wday, destResetDay);

    if (openDiff < 0 &&
        wdayDiff == 0)
    {
        openDiff += 86400; //cross day session compensation for nonweekly reset
    }

    if (closeDiff < 0 &&
        wdayDiff == 0)
    {
        closeDiff += 86400; //cross day session compensation for nonweekly reset
    }



    destLastOpenTime = now - (wdayDiff * 86400 + openDiff);
    destLastCloseTime = now + (wdayDiff * 86400 + closeDiff);

    if (orderTime >= destLastOpenTime &&
        orderTime < destLastCloseTime)
    {
        rc = true;
    }


    return rc;
}


//sessionStartDateTime is in local time and omTime is in GMT
bool CUtility::IsCurrentSessionActivity(unsigned short sessionStartTime, const char* pOmTime)
{
    bool rc = false;

    struct tm tmgmt, tmSessionStart;

    char buf[5];
    memset(buf, '\0', sizeof(buf));
    strncpy(buf, pOmTime + 4, 2);
    tmgmt.tm_mon = atoi(buf) - 1;
    strncpy(buf, pOmTime + 6, 2);
    tmgmt.tm_mday = atoi(buf);
    strncpy(buf, pOmTime + 9, 2);
    tmgmt.tm_hour = atoi(buf);
    strncpy(buf, pOmTime + 12, 2);
    tmgmt.tm_min = atoi(buf);
    strncpy(buf, pOmTime + 15, 2);
    tmgmt.tm_sec = atoi(buf);
    strncpy(buf, pOmTime, 4);
    tmgmt.tm_year = atoi(buf) - 1900;
    tmgmt.tm_isdst = -1;

    time_t omTime_t = timegm(&tmgmt);

    time_t ltime;
    time(&ltime);
    localtime_r(&ltime, &tmSessionStart);
    tmSessionStart.tm_hour = sessionStartTime / 100;
    tmSessionStart.tm_min = sessionStartTime % 100;
    tmSessionStart.tm_sec = 0;

    time_t sessionStart_t = mktime(&tmSessionStart);

    if (omTime_t > sessionStart_t)
    {
        rc = true;
    }

    return rc;
}


bool CUtility::GetPasswordFromDB(CDBExecutor& executor, const std::string login, std::string& password)
{
    bool rc = false;
    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr,
            "select password from OMuser where username = '%s'",
            login.c_str());

    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            if (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);
                if (pVal->arr_len)
                {
                    password.append((pVal->g_un).un_array, pVal->arr_len);
                    rc = true;
                }

                free(colE);
                gendb_destroy_v(pVal);
                free(rowE);
                destroy_queue(row);
                rc = true;
                break;
            }
            else
            {
                free(rowE);
                destroy_queue(row);
            }
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);

    return rc;
}

time_t CUtility::GetSecondsFromDateTimeStr(const std::string& datetimeStr)
{
    time_t rc = 0;

    struct tm tM;
    tM.tm_isdst = -1;
    std::string year = datetimeStr.substr(0, 4);
    std::string month = datetimeStr.substr(5, 2);
    std::string day = datetimeStr.substr(8, 2);
    std::string hour = datetimeStr.substr(11, 2);
    std::string minutes = datetimeStr.substr(14, 2);
    std::string seconds = datetimeStr.substr(17, 2);

    tM.tm_year = atoi(year.c_str()) - 1900;
    tM.tm_mon = atoi(month.c_str()) - 1;
    tM.tm_mday = atoi(day.c_str());
    tM.tm_hour = atoi(hour.c_str());
    tM.tm_min = atoi(minutes.c_str());
    tM.tm_sec = atoi(seconds.c_str());

    rc = mktime(&tM);
    return rc;
}

time_t CUtility::GetTimeT(const std::string& hh_mm)
{
    time_t now = 0;
    time(&now);
    
    struct tm tM;
    localtime_r(&now, &tM);
    std::string shour = hh_mm.substr(0, 2);
    std::string smin = hh_mm.substr(2, 2);
    tM.tm_hour = atoi(shour.c_str());
    tM.tm_min = atoi(smin.c_str());
    tM.tm_sec = 0;
    
    time_t tt = mktime(&tM);
    return tt;
}

time_t CUtility::GetSecondsToNow(const std::string& hh_mm)
{
    time_t now = 0;
    time(&now);
    
    struct tm tM;
    localtime_r(&now, &tM);
    std::string shour = hh_mm.substr(0, 2);
    std::string smin = hh_mm.substr(2, 2);
    tM.tm_hour = atoi(shour.c_str());
    tM.tm_min = atoi(smin.c_str());
    tM.tm_sec = 0;
    
    time_t tt = mktime(&tM);
    
    return (tt > now)? tt - now : 0;
}

void CUtility::GetLocalDateTimeStr(std::string& dtStr)
{
    time_t now = 0;
    struct tm tmNow;

    time(&now);
    localtime_r(&now, &tmNow);

    std::stringstream stm;
    stm << std::setw(4) << 1900 + tmNow.tm_year
        << "-"
        << std::setw(2) << std::setfill('0') << tmNow.tm_mon+1
        << "-"
        << std::setw(2) << std::setfill('0') << tmNow.tm_mday
        << " "
        << std::setw(2) << std::setfill('0') << tmNow.tm_hour
        << ":"
        << std::setw(2) << std::setfill('0') << tmNow.tm_min
        << ":"
        << std::setw(2) << std::setfill('0') << tmNow.tm_sec;

    dtStr = stm.str();

}

void CUtility::GetLocalYYYYMMDD(std::string& dStr, int days_from_today) //0:today, negative:past and positive:future
{
    time_t now = 0;
    struct tm tmNow;

    time(&now);
    now += days_from_today * 86400;
    localtime_r(&now, &tmNow);

    std::stringstream stm;
    stm << std::setw(4) << 1900 + tmNow.tm_year
        << std::setw(2) << std::setfill('0') << tmNow.tm_mon+1
        << std::setw(2) << std::setfill('0') << tmNow.tm_mday;
    dStr = stm.str();
}

bool CUtility::DayTurn_Central()
{
    static time_t tIni = BeginCentralDaySeconds();

    bool rc = false;
    time_t now = 0;
    time(&now);
    time_t dif = now - tIni;

    if (dif > 86400)
    {
        unsigned int days = dif / 86400;
        tIni += days * 86400;
        rc = true;
    }

    return rc;
}

time_t CUtility::BeginCentralDaySeconds()
{
    time_t now = 0;
    struct tm tmNow, tmCentralTime;
    time(&now);
    localtime_r(&now, &tmNow);

    //remember that both serverResetDay and serverOpenTime are central time!!!
    AdjustToCentralTime(tmNow, tmCentralTime);

    now -= (tmCentralTime.tm_hour * 3600 + tmCentralTime.tm_min * 60 + tmCentralTime.tm_sec);
    return now;
}

bool CUtility::CreateDirectories(const char* pFullPath)
{
    bool ret = true;

    char* p = const_cast<char*>(pFullPath);

    char* sp = NULL;

    while ((sp = strchr(p, '/')) != NULL)
    {
        if (sp != p)
        {
            struct stat Stat;
            *sp = '\0';
            if (stat(pFullPath, &Stat) != 0)
            {
                if (mkdir(pFullPath, S_IRWXU) != 0)
                {
                    ret = false;
                    break;
                }
            }
            else
            {
                if (S_ISDIR(Stat.st_mode) == 0)
                {
                    break;
                    ret = false;
                }
            }
            *sp = '/';
        }
        p = sp + 1;
    }

    if (true == ret && *p != '\0')
    {
        struct stat Stat;
        if (stat(pFullPath, &Stat) != 0)
        {
            if (mkdir(pFullPath, S_IRWXU) != 0)
            {
                ret = false;
            }
        }
        else
        {
            if (S_ISDIR(Stat.st_mode) == 0)
            {
                ret = false;
            }
        }
    }

    return ret;
}


/*bool CUtility::LoadDestIdToDestNameMap(CRomMap<std::string> & destMap, const tDBLogin& dblogin, CDBExecutor& executor)
{
    bool rc = false;

    queue_t results = create_queue();
    char sqlStr[256];
    std::string errStr;
    sprintf(sqlStr,
            "select DestID, dest_short_name from %s.dbo.OMDestination where DestID = DestRouteID and active_flag = 1",
            dblogin.database.c_str());

    if (executor.Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            std::string destname;
            long destid = 0;
            char sdestid[10];

            int cnt = 0;

            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                switch(cnt)
                {
                case 0:
                    destid = (pVal->g_un).un_int;
                    itoA(destid, sdestid, sizeof(sdestid));
                    break;
                case 1:
                    if (pVal->arr_len)
                    {
                        destname.assign((pVal->g_un).un_array, pVal->arr_len);
                    }
                    break;
                }

                cnt++;
                free(colE);
                gendb_destroy_v(pVal);
            }

            destMap.Add(sdestid, destname);

            free(rowE);
            destroy_queue(row);
        }

        rc = true;
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);

    return rc;
}*/

long CUtility::GetIntPrice(const std::string& sPrice)
{
    long rc = 0;
    char* p = strchr(const_cast<char*>(sPrice.c_str()), '.');
    size_t int_len = 0;

    if (p)
    {
        char buffer[128];
        int_len = p - sPrice.c_str();
        if (int_len > 0)
        {
            memcpy(buffer, sPrice.c_str(), int_len);
            buffer[int_len] = '\0';
            rc = atol(buffer);
            rc *= 10000;
        }

        size_t i = 0;

        for (; i < 4; i++)
        {
            p++;
            if (sPrice.size() > int_len + 1 + i)
            {
                buffer[i] = *p;
            }
            else
            {
                buffer[i] = '0';
            }
        }
        buffer[i] = '\0';
        rc += atol(buffer);
    }
    else
    {
        rc = atol(sPrice.c_str()) * 10000;
    }

    return rc;
}


time_t CUtility::GetTimeTB(unsigned short hour, unsigned short min, unsigned short day)
{
    time_t now = 0;
    time(&now);
    struct tm tm_tmp, tm_tmpc;
    localtime_r(&now, &tm_tmp);
    //values got from database are in central time!
    CUtility::AdjustToCentralTime(tm_tmp, tm_tmpc);
    long hr_diff = hour - tm_tmpc.tm_hour;
    long min_diff = min - tm_tmpc.tm_min;

    time_t rc = now +
                hr_diff * 3600 +
                min_diff * 60;

    if (7 != day)  //weekly session
    {
        if (day != tm_tmpc.tm_wday)
        {
            rc -= 86400 * CUtility::GetDaysDiffFromLastWDay(tm_tmpc.tm_wday, day);
        }
    }

    return rc;
}

bool CUtility::IsNickelTickPrice(const std::string& price)
{
    bool rc = true;
    
    size_t index = price.find_last_of('.');
    if (index != std::string::npos)
    {
        if (price.size() - index > 2)
        {
            if ((price[index + 2] != '0' && 
                 price[index + 2] != '5') ||
                (price.size() - index > 3 && price.substr(index + 3).find_first_not_of('0') != std::string::npos))
            {
                rc = false;
            }
        }
    }
    return rc;
}

bool CUtility::IsValidDartSDouble(const std::string& sd)
{
    int len = sd.size();
    bool rc = true;
    int m_cnt = 0;
    int dot_cnt = 0;
    
    for (int i = 0; i < len; i++)
    {
        if (sd[i] == '-')
        {
            m_cnt++;
            if (m_cnt > 1)
            {
                rc = false;
                break;
            }
            else if (i != 0)
            {
                rc = false;
                break;
            }
        }
        else if (sd[i] == '.')
        {
            dot_cnt++;
            if (dot_cnt > 1)
            {
                rc = false;
                break;
            }
        }
        else if (sd[i] < 48 || sd[i] > 57)
        {
            rc = false;
            break;
        }
    }
    
    return rc;
}

void CUtility::SendRollingLogMessage(const std::string& sender, const std::string& msg)
{
    struct message Msg;
    Msg.name = const_cast<char*>(sender.c_str());
    Msg.name_len = sender.length();
    Msg.data = const_cast<char*>(msg.c_str());
    Msg.len = msg.length();
    dart_rolling_log_coming(&Msg, Msg.name, Msg.name_len);
}