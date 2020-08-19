#ifndef _CONFIG_H__
#define _CONFIG_H__

#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include "ConfigBase.h"
#include "ROMTypedef.h"
#include "Utility.h"
#include "RomMapN.h"
#include "DestRequest.h"
#include "DBExecutor.h"

class CConfig : public CConfigBase
{
public:
    ~CConfig(void);

    static CConfig & getInstance()
    {
        static CConfig theConfig(CUtility::ConfigFile());
        return theConfig;
    }

    virtual const std::string& InstanceName()const
    {
        return m_instanceName;
    }

    const std::string& datFile()const
    {
        return m_datFile;
    }
    std::string Login()const;
    const std::string ServerIP()const
    {
        return m_ServerIP;
    }
    const std::string& MapFile()const
    {
        return m_MapFile;
    }
    const std::string& homePath() const
    {
        return m_homePath;
    }
    const std::string& SessionDateInstanceStr() const
    {
        return m_SessionDateInstanceStr;
    }

    const std::string& OatsInstancename() const
    {
        return m_oatsInstanceName;
    }

    const std::string& LastOatsInstancename() const
    {
        return m_LastOatsInstanceName;
    }

    const long HostCheckPeriod()const
    {
        return m_HostCheckPeriod;
    }
    const long OatsWarningQueueSize()const
    {
        return m_OatsWarningQueueSize;
    }
    const int ServerPort()const
    {
        return m_ServerPort;
    }
    const int OatsEnabled()const
    {
        return m_OatsEnabled;
    }
    const short ClientLogonMaxTry() const
    {
        return m_clientlogonmaxtry;
    }
    const int OrdersAllocStep() const
    {
        return m_ordersAllocStep;
    }

    bool IsSpecialOptionDest(std::string destID);
    bool IsSpecialNonOptionDest(std::string destID);
    bool LoadOrdersFromDB()
    {
        return m_LoadOrdersFromDB;
    }
    int GapFillEnabled()
    {
        return m_GapFillEnabled;
    }

    int Async()
    {
        return m_bAsync;
    }
    int numThreads()
    {
        return m_numThreads;
    }
    int timeout()
    {
        return m_timeout;
    }

    const int QueryLiveOrderFromOats()
    {
        return m_QueryLiveOrderFromOats;
    }

    void SetGapFillEnabled(int value)
    {
        m_GapFillEnabled = value;
    }
    void LoadReloadables();
    bool GetDestIpPort(const std::string& destName, std::string& ip, int& port);
    void GetDestDeathDelay(const std::string& destName, unsigned int& delay);
    int adCancel() { return m_avCancel; }
    int AdjustedEndHour();
    int AdjustedEndMin();
    int noTPOS() { return m_noTPOS; }
    int noDrop() { return m_noDrop; }
    int mkOats() { return m_mkOats; }
    //int FutureProductLimitEnabled() { return m_bFutureProductLimit; }

    CDBExecutor* dbexecutor() { return &m_dbexecutor; }
    bool DbUp();
    void UpdateLastOatsInstanceName(const std::string& name);

    int mds_port() { return m_mdsPort; }
    const std::string& mds_ip() { return m_mdsIP; }
    double OptPriceFactor() { return m_optPriceFactor; }
    
    int rdsPort() 
    {
        return m_rdsPort;
    }
    const std::string& rdsIP()
    {
        return m_rdsIP;
    }
    
    const std::string& initCumFile() const { return m_initCumFile;}
    
    const std::string& hhmmLoadEasyBorrowList() const { return m_hhmmLoadEasyBorrowList; }
    
    const std::string& BorrowListFullPathFmt() const { return m_BorrowListFullPathFmt; }
    const std::string& BorrowListFullPathFmtPast() const { return m_BorrowListFullPathFmtPast; }
    const std::string& ThresholdListFullPathFmt() const { return m_ThresholdListFullPathFmt; }
    
    time_t eswPeriod() { return m_eswPeriod; }
    
    int PosMgrEnabled() { return m_PosMgrEnabled; }
    const tDBLogin& rcDb() { return m_rcDb; }
    const std::string& SpecialMailGrp() { return m_specialMailGrp; }
    const std::string& hhmmLoadLendingLimits() { return m_hhmmLoadLendingLimits;}
    const std::string& hhmmLoadEasyThreshold() { return m_hhmmLoadEasyThreshold;}
    
    const std::string& BiProcLocate() { return m_biProcLocate; }
    const std::string& BiProcDept() { return m_biProcDept; }
    
    bool IsPriceCheckExemptedDest(const std::string& dest);
    
private:

    CConfig(const std::string& filename);
    void LoadSpecialOptionDests();
    void LoadNonSpecialOptonDests();
    void SetAdjustedEndTime();

    std::set<std::string> m_SpecialOptionDests;
    std::set<std::string> m_SpecialNonOptionDests;

    std::vector<int> m_cupIDs;

    std::string m_datFile;
    std::string m_ServerIP;
    std::string m_mdsIP;
    std::string m_homePath;
    std::string m_MapFile;
    std::string m_SessionDateInstanceStr;
    std::string m_instanceName;
    std::string m_oatsInstanceName;
    std::string m_LastOatsInstanceName;
    //std::string m_clientmsgLimitError;
    long	m_HostCheckPeriod;
    long	m_OatsWarningQueueSize;
    int     m_ordersAllocStep;
    short	m_clientlogonmaxtry;
    int		m_ServerPort;
    int     m_mdsPort;
    int		m_OatsEnabled;
    int		m_LoadOrdersFromDB;
    int		m_GapFillEnabled;
    int     m_QueryLiveOrderFromOats;
    int     m_noDrop;
    int     m_noTPOS;
    int     m_mkOats;

    int m_bAsync;
    //int m_bFutureProductLimit;
    int m_numThreads;  //total threads framework uses to serve dest and client connections
    int m_timeout;     //in microseconds
    int m_avCancel;   //cancel all orders m_avCancel minutes before ending the program.
    int m_AdjustedEndTime;
    //int m_rejectMissingNBBO;
    CDBExecutor m_dbexecutor;
    double m_optPriceFactor;
    
    std::string m_rdsIP;
    int m_rdsPort;
    std::string m_initCumFile;
    std::string m_hhmmLoadEasyBorrowList;
    std::string m_hhmmLoadLendingLimits;
    std::string m_hhmmLoadEasyThreshold;
    
    std::string m_biProcLocate;
    std::string m_biProcDept;
    
    time_t      m_eswPeriod;
    std::string m_BorrowListFullPathFmt;
    std::string m_BorrowListFullPathFmtPast;
    std::string m_ThresholdListFullPathFmt;
    int m_PosMgrEnabled;
    //std::string m_RoninTradingLender;
    tDBLogin m_rcDb;
    std::string m_specialMailGrp;
    std::vector<std::string> m_ExemptedDests;
};
#endif //_CONFIG_H__
