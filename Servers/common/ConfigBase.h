#ifndef _CONFIGBASE_H__
#define _CONFIGBASE_H__

#include <string>
#include "config_loader.h"
//#include "generic_db.h"
//#include "psql_db.h"

class CDBExecutor;

typedef struct TDGLOGIN
{
    std::string user;
    std::string password;
    std::string dbserver;
    std::string dbserver_port;
    std::string database;
    int dbtype;  //0:sqlserver, 1:postgresql
} tDBLogin;

typedef struct TLOGONUPATE
{
    //gendb_h* dbCon;
    CDBExecutor* executor;
    char login[128];
    char server[128];
    //char logontime[128];
    time_t logontime_t;
} tLogonUpdate;

class CConfigBase
{
public:
    CConfigBase(const std::string& configFile);
    ~CConfigBase();

    void Reload();

    bool GetConfig(const std::string& section, const std::string& tag, std::string& value);
    bool GetConfig(const std::string& section, const std::string& tag, int& value);
    bool GetConfig(const std::string& section, const std::string& tag, unsigned int& value);
    bool GetConfig(const std::string& section, const std::string& tag, long& value);
    bool GetConfig(const std::string& section, const std::string& tag, double& value);

    void UpdateConfig(const std::string& section, const std::string& tag, const std::string& value);
    void UpdateConfig(const std::string& section, const std::string& tag, int value);
    void UpdateConfig(const std::string& section, const std::string& tag, long value);

    void SetEndTime(int value)
    {
        m_EndTime = value;
    }
    void SetStartTime(int value)
    {
        m_StartTime = value;
    }
    /*
    const std::string& dbServer()const
    {
        return m_dbServer;
    }
    const std::string& dbLogin()const
    {
        return m_dbLogin;
    }
    const std::string& dbPassword()const
    {
        return m_dbPassword;
    }
    const std::string& Database()const
    {
        return m_Database;
    }*/
    const std::string& MailServer()const
    {
        return m_MailServer;
    }
    const std::string& MailBizGroup()const
    {
        return m_MailBizGroup;
    }
    const std::string& MailTechGroup()const
    {
        return m_MailTechGroup;
    }
    bool IsFirstRun()
    {
        return m_firstRun;
    }
    const int EndTime()const
    {
        return m_EndTime;
    }
    const int StartTime()const
    {
        return m_StartTime;
    }
    int EndHour();
    int EndMin();
    unsigned int CsvStoreStep()
    {
        return m_CsvStoreStep;
    }
    unsigned int CsvStoreBase()
    {
        return m_CsvStoreBase;

    }
    const int Instance()const
    {
        return m_instance;
    }
    const std::string& sInstance()const
    {
        return m_sInstance;
    }

    virtual const std::string& InstanceName()const =0;

    void SetReady(bool value)
    {
        pthread_mutex_lock(&m_readymutex);
        m_ready = value;
        pthread_mutex_unlock(&m_readymutex);
    }

    bool Ready()
    {
        bool rc = false;
        pthread_mutex_lock(&m_readymutex);
        rc = m_ready;
        pthread_mutex_unlock(&m_readymutex);
        return rc;
    }

    bool Closing(int now = -1);     //now is the time in local time
    void UpdateRunDate();

    std::string SessionDateStr() { return m_curSessionDate; }
    std::string SessionDateStrGMT() { return m_curSessionDateGMT; }

    unsigned int SendTryCnt()
    {
        return m_sendTryCnt;
    }

    unsigned int CheckDbPeriod() { return m_checkdbPeriod;}

    int numThreads() { return m_numThreads;}

    time_t SecondsToClose();

    //gendb_h* dbCon() { return m_dbCon;}
    int* Cpus()     { return m_Cpus; }
    const tDBLogin& db() { return m_db;}
    bool ResetType() { return m_ResetType; }
    int ResetDay()  { return m_ResetDay; }
    bool LastRunSucceeded() { return m_lastRunSucceeded; }
    void UpdateSessionDate();
    const std::string& ProcLoadRomUsers() { return m_procLoadRomUsers; }
    //unsigned long HeartBeatLimit() { return m_hbLimit; }
    
private:

    void SetFirstRun();
    void LoadCpus();
    void LoadLastExitCode();

    std::string m_fileName;
    //std::string m_dbServer;
    //std::string m_dbLogin;
    //std::string m_dbPassword;
    //std::string m_Database;
    std::string m_MailServer;
    std::string m_MailBizGroup;
    std::string m_MailTechGroup;
    std::string m_sInstance;
    std::string m_curSessionDate;       //in central time
    std::string m_curSessionDateGMT;
    std::string m_procLoadRomUsers;

    bool m_firstRun;
    bool m_ready;
    bool m_lastRunSucceeded;
    int m_EndTime;              //in local time
    int m_StartTime;            //in local time
    int m_ResetDay;     //0 to 6
    int m_EndDay;       //0 to 6
    int m_ResetType;  //0=daily and 1=weekly

    int	m_instance;
    unsigned int m_CsvStoreStep;
    unsigned int m_CsvStoreBase;
    unsigned int m_checkdbPeriod;
    //unsigned long m_hbLimit;
    unsigned int m_numThreads;
    dart_config* m_pConfig;
    pthread_mutex_t m_readymutex;
    unsigned int m_sendTryCnt;
    int* m_Cpus;

    tDBLogin m_db;
};

#endif //_CONFIGBASE_H__
