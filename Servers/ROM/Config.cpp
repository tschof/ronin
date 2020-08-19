#include "Config.h"
#include <string>
#include <vector>
#include "config_loader.h"
#include "Utility.h"

CConfig::CConfig(const std::string& filename) : CConfigBase(filename),
m_dbexecutor(db())
{
    m_instanceName = "ROM";
    m_instanceName.append(sInstance());
    //m_Cpus = NULL;

    if (GetConfig("Server", "IP", m_ServerIP) == false)
    {
        m_ServerIP = "";
    }

    if (GetConfig("Server", "Map File", m_MapFile) == false)
    {
        std::string sConfig = CUtility::ConfigFile();
        int ps = sConfig.find_last_of('/');
        m_MapFile = sConfig.substr(0, ps + 1) + std::string("RomMap.file");
    }

    if (GetConfig("Server", "Port", m_ServerPort) == false)
    {
        m_ServerPort = 8008;
    }

    if (GetConfig("Server", "Host Check Period", m_HostCheckPeriod) == false)
    {
        m_HostCheckPeriod = 10;
    }

    if (GetConfig("Oats Connection", "Enabled", m_OatsEnabled) == false)
    {
        m_OatsEnabled = 1;
    }

    if (GetConfig("Oats Connection", "Warning Queue Size", m_OatsWarningQueueSize) == false)
    {
        m_OatsWarningQueueSize = 5000;
    }

    if (GetConfig("Oats Connection", "Oats InstanceName", m_oatsInstanceName) == false)
    {
        m_oatsInstanceName = "OATS";
    }

    if (GetConfig("Oats Connection", "Last Oats InstanceName", m_LastOatsInstanceName) == false)
    {
        m_LastOatsInstanceName = m_oatsInstanceName;
    }

    if (GetConfig("Option Information", "Load Orders From DB", m_LoadOrdersFromDB) == false)
    {
        m_LoadOrdersFromDB = 0;
    }

    if (GetConfig("Option Information", "Enable Gap Fill", m_GapFillEnabled) == false)
    {
        m_GapFillEnabled = 1;
    }

    if (GetConfig("Option Information", "Cancel Order Minutes", m_avCancel) == false)
    {
        m_avCancel = 3;
    }

    if (GetConfig("Option Information", "Orders Allocation Step", m_ordersAllocStep) == false)
    {
        m_ordersAllocStep = 3000;
    }

    if (GetConfig("Option Information", "Using Async Framework", m_bAsync) == false)
    {
        m_bAsync = 0;
    }

    if (GetConfig("Option Information", "Number of Framework Threads", m_numThreads) == false)
    {
        m_numThreads = 3;
    }

    SetAdjustedEndTime();

    if (GetConfig("Option Information", "Time Out", m_timeout) == false)
    {
        m_timeout = 20;
    }

    if (GetConfig("Option Information", "Load Live Orders from Oats", m_QueryLiveOrderFromOats) == false)
    {
        m_QueryLiveOrderFromOats = 0;
    }

    if (GetConfig("Option Information", "No Drop", m_noDrop) == false)
    {
        m_noDrop = 0;
    }

    if (GetConfig("Option Information", "No TPOS", m_noTPOS) == false)
    {
        m_noTPOS = 0;
    }

    if (GetConfig("Option Information", "Make Oats Msg", m_mkOats) == false)
    {
        m_mkOats = 1;
    }

    m_SessionDateInstanceStr = SessionDateStr();
    m_SessionDateInstanceStr = m_SessionDateInstanceStr.substr(1);
    m_SessionDateInstanceStr = sInstance() + m_SessionDateInstanceStr;

    LoadSpecialOptionDests();
    LoadNonSpecialOptonDests();
      

    //if (GetConfig("Option Information", "Do Future Product Limit", m_bFutureProductLimit) == false)
    //{
    //    m_bFutureProductLimit = 1;
    //}

    GetConfig("MDS Connection", "IP", m_mdsIP);

    if (GetConfig("MDS Connection", "Port", m_mdsPort) == false)
    {
        m_mdsPort = 0;
    }

    if (GetConfig("Option Information", "Option Price Factor", m_optPriceFactor) == false)
    {
        m_optPriceFactor = 0.8;
    }

    //if (GetConfig("Option Information", "Reject Missing NBBO", m_rejectMissingNBBO) == false)
    //{
    //    m_rejectMissingNBBO = 1;
    //}
    
    if (GetConfig("Option Information", "Initial Cum File", m_initCumFile) == false)
    {
        m_initCumFile.assign("InitCum.txt");
    }
    
    if (GetConfig("RDS Connection", "IP", m_rdsIP) == false)
    {
        m_rdsIP.assign("10.105.96.42");
    }
    
    if (GetConfig("RDS Connection", "Port", m_rdsPort) == false)
    {
        m_rdsPort = 7777;
    }
    
    if (GetConfig("Option Information", "TIME TO LOAD EASYBORROWLIST", m_hhmmLoadEasyBorrowList) == false)
    {
        m_hhmmLoadEasyBorrowList.assign("0730");
    }
    
    int minutes = 0;
    if (GetConfig("Option Information", "Easy Symbol Loading Warning Period", m_eswPeriod) == false)
    {
        minutes = 10;
    }
    m_eswPeriod = 60 * minutes;
    
    if (GetConfig("Option Information", "EASY BORROW LIST PATH FORMAT", m_BorrowListFullPathFmt) == false)
    {
        m_BorrowListFullPathFmt.assign("%s_FORTIS_EZ2Borrow.txt");
    }
    
    if (GetConfig("Option Information", "EASY BORROW LIST PATH FORMAT PAST", m_BorrowListFullPathFmt) == false)
    {
        m_BorrowListFullPathFmtPast.assign("./ez2b_old/%s_FORTIS_EZ2Borrow.txt");
    }

    if (GetConfig("Option Information", "THRESHOLD LIST PATH FORMAT", m_ThresholdListFullPathFmt) == false)
    {
        m_ThresholdListFullPathFmt.assign("OCCth%s.txt");
    }
        
    if (GetConfig("Option Information", "POS_MGR Enabled", m_PosMgrEnabled) == false)
    {
        m_PosMgrEnabled = 0;
    }
    
    //if (GetConfig("Option Information", "Ronin Trading Lender", m_RoninTradingLender) == false)
    //{
    //    m_RoninTradingLender = "ABA";
    //}
        
    if (GetConfig("Database Information", "Datasource_rc", m_rcDb.dbserver) == false)
    {
        m_rcDb.dbserver = "DBI-ORL-SQL1";
    }

    if (GetConfig("Database Information", "User_rc", m_rcDb.user) == false)
    {
        m_rcDb.user = "dart_user";
    }

    if (GetConfig("Database Information", "Password_rc", m_rcDb.password) == false)
    {
        m_rcDb.password = "resu_trad";
    }

    if (GetConfig("Database Information", "Database_rc", m_rcDb.database) == false)
    {
        m_rcDb.database = "RoninClearing";
    }
    
    m_rcDb.dbtype = 0;
        
    if (GetConfig("EMAIL", "Mail Special Biz Group", m_specialMailGrp) == false)
    {
        m_specialMailGrp = "ROMBusAlerts@roncap.com";
    }
    
    if (GetConfig("Option Information", "Load Lending Limits Time", m_hhmmLoadLendingLimits) == false)
    {
        m_hhmmLoadLendingLimits.assign("0801");
    }
    
    if (GetConfig("Option Information", "Load Easy Threshold Time", m_hhmmLoadEasyThreshold) == false)
    {
        m_hhmmLoadEasyThreshold.assign("0831");
    }
    
    if (GetConfig("Database Information", "BI Proc Locate", m_biProcLocate) == false)
    {
        m_biProcLocate.assign("getLocateReceived");
    }
    
    if (GetConfig("Database Information", "BI Proc Dept", m_biProcDept) == false)
    {
        m_biProcDept.assign("getDepartmentMembers");
    }
    
    std::string buffer;
    if (GetConfig("Option Information", "PriceCheckExemptedDests", buffer))
    {
        CUtility::spliter(buffer, m_ExemptedDests, ',');
    }
}

bool CConfig::IsPriceCheckExemptedDest(const std::string& dest)
{
    bool rc = false;
    
    for (unsigned int i = 0; i < m_ExemptedDests.size(); i++)
    {
        if (dest.compare(m_ExemptedDests[i]) == 0)
        {
            rc = true;
        }
    }
    
    return rc;
}

CConfig::~CConfig(void)
{
    //if (m_Cpus)
    //{
    //    free(m_Cpus);
    //}
}

std::string CConfig::Login()const
{
    char value[10];

    sprintf(value, "ROM%d", Instance());

    std::string rc = value;
    return rc;
}

void CConfig::LoadNonSpecialOptonDests()
{
    char sNum[5];
    std::string sBuf;
    int i = 0;

    while (1)
    {
        sprintf(sNum, "%d", i++);

        if (GetConfig("Special NonOption Dests", sNum, sBuf) == false)
        {
            sBuf = "";
        }

        if (!sBuf.empty())
        {
            m_SpecialNonOptionDests.insert(sBuf);
        }
        else
        {
            break;
        }
    }
}

void CConfig::LoadSpecialOptionDests()
{
    char sNum[5];
    std::string sBuf;
    int i = 0;

    while (1)
    {
        sprintf(sNum, "%d", i++);

        if (GetConfig("Special Option Dests", sNum, sBuf) == false)
        {
            sBuf = "";
        }

        if (!sBuf.empty())
        {
            m_SpecialOptionDests.insert(sBuf);
        }
        else
        {
            break;
        }
    }

    if (m_SpecialOptionDests.empty() == true)
    {
        m_SpecialOptionDests.insert("47"); //GLOBEX_O
        m_SpecialOptionDests.insert("79"); //ICE_OPT
    }
}


bool CConfig::IsSpecialNonOptionDest(std::string destID)
{
    bool rc = false;
    std::set<std::string>::iterator it = m_SpecialNonOptionDests.find(destID);
    if (it != m_SpecialNonOptionDests.end())
    {
        rc = true;
    }
    return rc;
}

bool CConfig::IsSpecialOptionDest(std::string destID)
{
    bool rc = false;
    std::set<std::string>::iterator it = m_SpecialOptionDests.find(destID);
    if (it != m_SpecialOptionDests.end())
    {
        rc = true;
    }
    return rc;
}

void CConfig::LoadReloadables()
{
    Reload();
}

//void CConfig::GetDestList(tDestRequestList& list)
//{
//
//}


bool CConfig::GetDestIpPort(const std::string& destName, std::string& ip, int& port)
{
    std::string secName = destName + " Connection";
    bool rc = false;
    if (GetConfig(secName, "Port", port) &&
        GetConfig(secName, "Host", ip))
    {
        rc = true;
    }
    return rc;
}

void CConfig::GetDestDeathDelay(const std::string& destName, unsigned int& delay)
{
    std::string secName = destName + " Connection";

    if (GetConfig(secName, "Death Delay", delay) == false)
    {
        delay = 2;;
    }
}

/*void CConfig::LoadCpus()
{
    std::string cpus;

    if (GetConfig("Option Information", "CPUs", cpus))
    {
        m_Cpus = (int*)calloc(m_numThreads, sizeof(int));
        std::vector<std::string> cpuV;
        CUtility::spliter(cpus, cpuV, ';');
        for (int i = 0; i < cpuV.size() && i < m_numThreads; i++)
        {
            m_Cpus[i] = atoi(cpuV[i].c_str());
        }
    }
}*/

void CConfig::SetAdjustedEndTime()
{
    int endminutes = EndTime() % 100;
    int endhour = EndTime() / 100;

    if (endminutes + m_avCancel < 60)
    {
        endminutes += m_avCancel;
    }
    else
    {
        endhour += 1;
        if (endhour >= 24)
        {
            endhour -= 24;
        }
        endminutes = endminutes + m_avCancel - 60;
    }

    m_AdjustedEndTime = endhour * 100 + endminutes;
}

int CConfig::AdjustedEndHour()
{
    return m_AdjustedEndTime / 100;
}

int CConfig::AdjustedEndMin()
{
    return m_AdjustedEndTime % 100;
}

//bool CConfig::DBExecute(const std::string& sqlstr, gendb_parm ** parms, int parm_len, queue_t result, std::string& errStr)
//{
//    return m_dbexecutor.Execute(sqlstr, parms, parm_len, result, errStr);
//}

bool CConfig::DbUp()
{
    return m_dbexecutor.DbUp();
}

void CConfig::UpdateLastOatsInstanceName(const std::string& name)
{
    UpdateConfig("Oats Connection", "Last Oats InstanceName", name);
}
