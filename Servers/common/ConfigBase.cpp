#include "ConfigBase.h"
#include "Utility.h"

#ifndef _WIN32
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

CConfigBase::CConfigBase(const std::string& configFile) : m_fileName(configFile)
{
    m_ready = false;
    m_Cpus = NULL;
    m_lastRunSucceeded = true;

    pthread_mutex_init(&m_readymutex, NULL);

    m_pConfig = initialize_config(m_fileName.c_str(), m_fileName.length());

    if (GetConfig("Server", "Instance ID", m_instance) == false)
    {
        m_instance = 2;
    }

    char value[5];
    sprintf(value, "%d", m_instance);
    m_sInstance = value;

    if (GetConfig("Database Information", "Datasource", m_db.dbserver) == false)
    {
        m_db.dbserver = "";
    }

    if (GetConfig("Database Information", "DB port", m_db.dbserver_port) == false)
    {
        m_db.dbserver_port = "5432";
    }

    if (GetConfig("Database Information", "User", m_db.user) == false)
    {
        m_db.user = "";
    }

    if (GetConfig("Database Information", "Password", m_db.password) == false)
    {
        m_db.password = "";
    }

    if (GetConfig("Database Information", "Database", m_db.database) == false)
    {
        m_db.database = "rom";
    }

    m_db.dbtype = 1;

    if (GetConfig("Database Information", "RomClientsProc", m_procLoadRomUsers) == false)
    {
        m_procLoadRomUsers = "ROMGetROMusersNew";
    }

    if (GetConfig("EMAIL", "Mail Server", m_MailServer) == false)
    {
        m_MailServer = "relay.ronin.roncap.com";
    }

    if (GetConfig("EMAIL", "Mail Biz Group", m_MailBizGroup) == false)
    {
        m_MailBizGroup = "ROMBusAlerts@ronin-capital.com";
    }

    if (GetConfig("EMAIL", "Mail Tech Group", m_MailTechGroup) == false)
    {
        m_MailTechGroup = "ROMTechAlerts@ronin-capital.com";
    }

    if (GetConfig("Server", "End Time", m_EndTime) == false)
    {
        m_EndTime = 1700;
    }

    if (GetConfig("Server", "Start Time", m_StartTime) == false)
    {
        m_StartTime = 600;
    }

    if (GetConfig("Server", "Reset Type", m_ResetType) == false)
    {
        m_ResetType = 0;
    }

    if (GetConfig("Server", "Reset Day", m_ResetDay) == false)
    {
        m_ResetDay = 7;   //daily
    }

    if (GetConfig("Server", "End Day", m_EndDay) == false)
    {
        m_EndDay = 7;  //daily
    }

    if (GetConfig("Option Information", "CsvStore Step", m_CsvStoreStep) == false)
    {
        m_CsvStoreStep = 500;
    }

    if (GetConfig("Option Information", "CsvStore Base", m_CsvStoreBase) == false)
    {
        m_CsvStoreBase = 2000;
    }

    if (GetConfig("Option Information", "Number of Framework Threads", m_numThreads) == false)
    {
        m_numThreads = 3;
    }

    if (GetConfig("Option Information", "Send Try Count", m_sendTryCnt) == false)
    {
        m_sendTryCnt = 100000;
    }

    if (GetConfig("Option Information", "Check DB", m_checkdbPeriod) == false)
    {
        m_checkdbPeriod = 30;
    }

    //if (GetConfig("Option Information", "Client HeartBeat Limit", (unsigned int&)m_hbLimit) == false)
    //{
    //    m_hbLimit = 3;
    //}
    
    struct tm TmLocal, TmGMT, TmCentral;
    time_t t;
    time(&t);
    localtime_r(&t, &TmLocal);
    CUtility::AdjustToCentralTime(TmLocal, TmCentral);
    gmtime_r(&t, &TmGMT);

    char localDate[9];
    sprintf(localDate, "%04u%02u%02u", TmLocal.tm_year + 1900, TmLocal.tm_mon + 1, TmLocal.tm_mday);
    m_firstRun = false;

    SetFirstRun();

    LoadCpus();
    LoadLastExitCode();
}

void CConfigBase::UpdateRunDate()
{
    std::string currentDayStr;
    CUtility::GetLocalDateTimeStr(currentDayStr);
    UpdateConfig("Server", "Last Run Date Time", currentDayStr);
}

CConfigBase::~CConfigBase()
{
    pthread_mutex_destroy(&m_readymutex);

    if (m_Cpus)
    {
        free(m_Cpus);
    }
}

void CConfigBase::Reload()
{
    dart_reset_maps(m_pConfig);
}

bool CConfigBase::GetConfig(const std::string& section, const std::string& tag, std::string& value)
{
    bool rc = false;
    int len = 0;
    char* pV = get_val_for_tag(m_pConfig, const_cast<char*>(section.c_str()), section.size(), const_cast<char*>(tag.c_str()), tag.size(), &len);
    if (pV)
    {
        value.assign(pV, len);
        free(pV);
        rc = true;
    }
    return rc;
}

bool CConfigBase::GetConfig(const std::string& section, const std::string& tag, int& value)
{
    bool rc = false;
    int len = 0;
    char* pV = get_val_for_tag(m_pConfig, const_cast<char*>(section.c_str()), section.size(), const_cast<char*>(tag.c_str()), tag.size(), &len);
    if (pV)
    {
        std::string tmp;
        tmp.assign(pV, len);
        value = atoi(tmp.c_str());
        free(pV);
        rc = true;
    }
    return rc;
}

bool CConfigBase::GetConfig(const std::string& section, const std::string& tag, unsigned int& value)
{
    bool rc = false;
    int len = 0;
    char* pV = get_val_for_tag(m_pConfig, const_cast<char*>(section.c_str()), section.size(), const_cast<char*>(tag.c_str()), tag.size(), &len);
    if (pV)
    {
        std::string tmp;
        tmp.assign(pV, len);
        value = atoi(tmp.c_str());
        free(pV);
        rc = true;
    }
    return rc;
}

bool CConfigBase::GetConfig(const std::string& section, const std::string& tag, long& value)
{
    bool rc = false;
    int len = 0;
    char* pV = get_val_for_tag(m_pConfig, const_cast<char*>(section.c_str()), section.size(), const_cast<char*>(tag.c_str()), tag.size(), &len);
    if (pV)
    {
        std::string tmp;
        tmp.assign(pV, len);
        value = atol(tmp.c_str());
        free(pV);
        rc = true;
    }
    return rc;
}

bool CConfigBase::GetConfig(const std::string& section, const std::string& tag, double& value)
{
    bool rc = false;
    int len = 0;
    char* pV = get_val_for_tag(m_pConfig, const_cast<char*>(section.c_str()), section.size(), const_cast<char*>(tag.c_str()), tag.size(), &len);
    if (pV)
    {
        std::string tmp;
        tmp.assign(pV, len);
        value = atof(tmp.c_str());
        free(pV);
        rc = true;
    }
    return rc;
}


void CConfigBase::UpdateConfig(const std::string& section, const std::string& tag, const std::string& value)
{
    set_val_for_tag(m_pConfig,
                    const_cast<char*>(section.c_str()),
                    section.size(),
                    const_cast<char*>(tag.c_str()),
                    tag.size(),
                    const_cast<char*>(value.c_str()),
                    value.size());
}

void CConfigBase::UpdateConfig(const std::string& section, const std::string& tag, int value)
{
    char buffer[200];
    sprintf(buffer, "%d", value);
    std::string val = buffer;
    UpdateConfig(section, tag, val);
}

void CConfigBase::UpdateConfig(const std::string& section, const std::string& tag, long value)
{
    char buffer[200];
    sprintf(buffer, "%ld", value);
    std::string val = buffer;
    UpdateConfig(section, tag, val);
}

int CConfigBase::EndHour()
{
    return m_EndTime / 100;
}

int CConfigBase::EndMin()
{
    return  m_EndTime - EndHour() * 100;
}

bool CConfigBase::Closing(int now)
{
    bool rc = false;

    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    if (now == -1)
    {
        now = Tm.tm_hour * 100 + Tm.tm_min;
    }

    if (m_ResetType)  //weekly
    {
        if (m_ResetDay < m_EndDay)
        {
            if (Tm.tm_wday < m_ResetDay ||
                Tm.tm_wday > m_EndDay)
            {
                return rc;
            }
        }
        else if (m_EndDay < m_ResetDay)
        {
            if (Tm.tm_wday > m_EndDay &&
                Tm.tm_wday < m_ResetDay)
            {
                return rc;
            }
        }
    }

    /*
    if (now == -1)
    {
        struct tm Tm;
        time_t ltime;

        time(&ltime);
        localtime_r(&ltime, &Tm);
        now = Tm.tm_hour * 100 + Tm.tm_min;
    }
    */

    if (m_EndTime > m_StartTime)
    {
        if (now >= m_EndTime || now < m_StartTime)
        {
            rc = true;
        }
    }
    else
    {
        if (now >= m_EndTime && now < m_StartTime)
        {
            rc = true;
        }
    }

    return rc;
}

time_t CConfigBase::SecondsToClose()
{
    int days = 0;
    struct tm Tm;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);
    int isdst_begin = Tm.tm_isdst;

    int now = Tm.tm_hour * 100 + Tm.tm_min;

    if (1 == m_ResetType)  //weekly
    {
        if (Tm.tm_wday == m_EndDay)
        {
            if (now > ((m_EndTime / 100 + m_EndTime % 100) * 60))
            {
                days = CUtility::GetDaysDiffFromNextWDay(Tm.tm_wday, m_EndDay);
            }
        }
        else
        {
            days = CUtility::GetDaysDiffFromNextWDay(Tm.tm_wday, m_EndDay);
        }
    }

    time_t rc = 0;

    //if (m_EndTime > m_StartTime)
    if (m_EndTime >= now)
    {
        //if (now < m_EndTime && now >= m_StartTime)
        //{
            rc = ((m_EndTime / 100 - now / 100) * 60 + m_EndTime % 100 - now % 100) * 60;
        //}
    }
    else
    {
        rc = 86400 - ((now / 100 - m_EndTime / 100) * 60 + now % 100 - m_EndTime % 100) * 60;
        //if (now < m_EndTime)
        //{
        //    rc = ((m_EndTime / 100 - now / 100) * 60 + m_EndTime % 100 - now % 100) * 60;
        //}
        //else if (now >= m_StartTime)
        //{
        //    rc = 86400 - ((now / 100 - m_EndTime / 100) * 60 + now % 100 - m_EndTime % 100) * 60;
        //}
    }

    rc = rc + days * 86400;
    ltime += rc;

    struct tm Tm1;
    localtime_r(&ltime, &Tm1);
    int isdst_end = Tm1.tm_isdst;

    if (isdst_begin == 0 && isdst_end == 1)
    {
        rc -= 3600;
    }
    else if (isdst_begin == 1 && isdst_end == 0)
    {
        rc += 3600;
    }
    return rc;
}

void CConfigBase::LoadCpus()
{
    std::string cpus;

    if (GetConfig("Option Information", "CPUs", cpus))
    {
        m_Cpus = (int*)calloc(m_numThreads, sizeof(int));
        std::vector<std::string> cpuV;
        CUtility::spliter(cpus, cpuV, ';');
        for (size_t i = 0; i < cpuV.size() && i < m_numThreads; i++)
        {
            m_Cpus[i] = atoi(cpuV[i].c_str());
        }
    }
}

void CConfigBase::UpdateSessionDate()
{
    struct tm Tm, TmCentral;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    char today[128];
    CUtility::AdjustToCentralTime(Tm, TmCentral);
    sprintf(today, "%04u%02u%02u", TmCentral.tm_year + 1900, TmCentral.tm_mon + 1, TmCentral.tm_mday);
    m_curSessionDate = today;
}

void CConfigBase::SetFirstRun()
{
    m_firstRun = true;

    int days = 0;
    struct tm Tm, TmCentral;
    time_t ltime;

    time(&ltime);
    localtime_r(&ltime, &Tm);

    char today[128];
    CUtility::AdjustToCentralTime(Tm, TmCentral);
    sprintf(today, "%04u%02u%02u", TmCentral.tm_year + 1900, TmCentral.tm_mon + 1, TmCentral.tm_mday);
    m_curSessionDate = today;

    if (1 == m_ResetType)
    {
        int now = Tm.tm_hour * 3600 + Tm.tm_min * 60 + Tm.tm_sec;

        if (Tm.tm_wday == m_EndDay)
        {
            if (now < ((m_EndTime / 100) * 3600 + (m_EndTime % 100)* 60))
            {
                days = CUtility::GetDaysDiffFromLastWDay(Tm.tm_wday, m_ResetDay);
            }
        }
        else
        {
            days = CUtility::GetDaysDiffFromLastWDay(Tm.tm_wday, m_ResetDay);
        }
    }
    else
    {
        int start_seconds = (m_StartTime / 100) * 3600 + (m_StartTime % 100) * 60;
        int end_seconds = (m_EndTime / 100) * 3600 + (m_EndTime % 100) * 60;
        if (end_seconds < start_seconds) //cross mid night session!
        {
            days = 1;
        }
    }
    
    ltime -= Tm.tm_hour * 3600 + Tm.tm_min * 60 + Tm.tm_sec;//back to the beginning of today
    ltime -= days * 86400; //back to the beginning of the last start day
    time_t lastSessionStartSeconds = ltime  + ((m_StartTime / 100) * 3600 + (m_StartTime % 100) * 60);

    std::string lastRunDate;
    if (GetConfig("Server", "Last Run Date Time", lastRunDate) == false)
    {
        lastRunDate = "";
    }
    else
    {
        time_t lastRunSeconds = CUtility::GetSecondsFromDateTimeStr(lastRunDate);
        if (lastRunSeconds > lastSessionStartSeconds)
        {
            m_firstRun = false;
        }
    }
}


void CConfigBase::LoadLastExitCode()
{
    m_lastRunSucceeded = true;
    FILE* fp = fopen("ExitCode", "r+");

    if (fp)
    {
        setbuf(fp, NULL);

        if (false == m_firstRun)
        {
            char code = 'S';
            if ((code = fgetc(fp)) != EOF)
            {
                if ('F' == code)
                {
                    m_lastRunSucceeded = false;
                }
            }
            rewind(fp);
        }
        fputc('F', fp);
        fclose(fp);
    }
}