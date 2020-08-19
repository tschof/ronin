#ifndef __CLIENTPROFILEMANAGER_H__
#define __CLIENTPROFILEMANAGER_H__


#include "RomOrderNew.h"
#include "RomMapN.h"
#include "ROMTypedef.h"

/*
typedef struct TBALANCEUPDATE
{
    long orderChange;
    long shareChange;
    double dollarChange;
    char secType;
} tBalanceUpdate;

typedef struct TCLEARINGBALANCEUPDATE
{
    tBalanceUpdate update;
    std::string clearing_sec;
    std::string trader;
} tKeyedBalanceUpdate;
*/
typedef struct LIMIT
{
    double BuyingPower;
    double MaxDollars;
    double Balance;
    long MaxShares;
    long MaxTotalShares;
    long NumShares;
    int MaxOrders;
    int NumOrders;
} tLimits;


typedef struct DROPINFO
{
    std::string ReportingFirm;
    int ReportingType;
    bool RoninMember;
} tDropInfo;


typedef std::set<std::string> tStringSet;

class CThreadLimits
{
public:
    CThreadLimits(const std::string& ClearingSecKey, const tLimits& limits);
    ~CThreadLimits();

    bool ExceedLimit(const tBalanceUpdate& BUpdate, std::string& error);
    void UpdateBalance(const tBalanceUpdate& BUpdate);
    void GetLimits(tLimits& limits);
    void UpdateLimits(const tLimits& limits);
    const std::string& ClearingSeckey() { return  m_ClearingSecKey; }

private:
    tLimits m_limits;
    std::string m_ClearingSecKey;
    pthread_rwlock_t m_lock;
};

class CClearingActLimits
{
public:
    CClearingActLimits();
    ~CClearingActLimits();

    bool ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, std::string& error);
    bool UpdateBalance(const std::string& key, const tBalanceUpdate& BUpdate);
    bool GetLimits(const std::string& ClearingSecKey, tLimits& limits);
    bool AddLimits(const std::string& ClearingSecKey, const tLimits& limits);
    bool UpdateLimits(const std::string& ClearingSecKey, const tLimits& limits);
    void ReloadFromDB(const std::string* key = NULL);

private:
    void ReloadAllLimits();
    void ReloadLimits(const std::string* key);

    CRomMap<CThreadLimits*> m_ClearingSecToLimits;
    pthread_rwlock_t m_lock;
};

class CTraderData
{
public:

    CTraderData()
    {
        m_Trader = "";
        m_Password = "";
        m_Group = "";
        m_ReportingFirm = "";
        m_OtherTraders = "";
        m_NumAccounts = 0;
        m_PrincipalAbility = false;
        m_DiscretionaryAbility = false;
        m_Exempt = false;
        m_RoninMember = false;
        m_ReportingType = 1;
        memset(&m_EquityLimits, 0, sizeof(tLimits));
        memset(&m_OptionLimits, 0, sizeof(tLimits));
        memset(&m_FutureLimits, 0, sizeof(tLimits));
        memset(&m_CurrencyLimits, 0, sizeof(tLimits));
    }
    ~CTraderData()
    {

    }

    void Get(CTraderData* pData);
    void SetTrader(const std::string& value)
    {
        m_Trader = value;
    }
    void SetGroup(const std::string& value)
    {
        m_Group = value;
    }
    void SetTrader(const char* value)
    {
        m_Trader = value;
    }
    void SetGroup(const char* value)
    {
        m_Group = value;
    }
    void SetOtherTraders(const std::string& value)
    {
        m_OtherTraders = value;
    }
    void SetOtherTraders(const char* value)
    {
        m_OtherTraders = value;
    }
    void SetNumAccounts(int value)
    {
        m_NumAccounts = value;
    }
    void SetPrincipalAbility(bool value)
    {
        m_PrincipalAbility = value;
    }
    void SetDiscretionaryAbility(bool value)
    {
        m_DiscretionaryAbility = value;
    }
    void SetExempt(bool value)
    {
        m_Exempt = value;
    }
    void SetPassword(const std::string& value)
    {
        m_Password = value;
    }
    void SetPassword(const char* value)
    {
        m_Password = value;
    }
    void SetReportingFirm(const std::string& value)
    {
        m_ReportingFirm = value;
    }
    void SetReportingFirm(const char* value)
    {
        m_ReportingFirm = value;
    }
    void SetReportingType(int value)
    {
        m_ReportingType = value;
    }
    void SetRoninMember(bool value)
    {
        m_RoninMember = value;
    }
    //bool IsSubTrader(const char* trader);

    std::string Trader()
    {
        return m_Trader;
    }
    std::string Password()
    {
        return m_Password;
    }
    std::string Group()
    {
        return m_Group;
    }

    std::string OtherTraders()
    {
        return m_OtherTraders;
    }
    int  NumAccounts()
    {
        return m_NumAccounts;
    }
    bool PrincipalAbility()
    {
        return m_PrincipalAbility;
    }
    bool DiscretionaryAbility()
    {
        return m_DiscretionaryAbility;
    }
    bool Exempt()
    {
        return m_Exempt;
    }
    std::string ReportingFirm()
    {
        return m_ReportingFirm;
    }
    int  ReportingType()
    {
        return m_ReportingType;
    }
    bool RoninMember()
    {
        return m_RoninMember;
    }

    bool IsSubTrader(const char* trader);
    void LoadSubTraders(std::string traders);
    void LoadSubTraders(tStringSet& traderSet);
    bool ExceedLimit(const tBalanceUpdate& theChange, std::string& error);
    void UpdateBalance(const tBalanceUpdate& update);
    void SetBalance(const tBalanceUpdate& update);
    void GetBalance(tBalanceUpdate& update);
    bool CanTradeFor(const std::string& tradeFor);

    void GetDropInfo(tDropInfo& dropInfo);

    inline tLimits* GetLimits(char secType)
    {
        switch(secType)
        {
        case ROMVALUE::SECTYPE_EQUITY:
            return &m_EquityLimits;
        case ROMVALUE::SECTYPE_OPTION:
        case ROMVALUE::SECTYPE_OPTIONINDEX:
        case ROMVALUE::SECTYPE_OPTIONFUTURE:
            return &m_OptionLimits;
        case ROMVALUE::SECTYPE_FUTURE:
        case ROMVALUE::SECTYPE_FUTUREEQUITY:
        case ROMVALUE::SECTYPE_FUTUREINDEX:
        case ROMVALUE::SECTYPE_FUTURECURRENCY:
            return &m_FutureLimits;
        case ROMVALUE::SECTYPE_CURRENCY:
            return &m_CurrencyLimits;
        default:
            return &m_EquityLimits;
        }
    }

    inline double BuyingPower(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->BuyingPower;
    }

    inline double MaxDollars(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->MaxDollars;
    }

    inline int  MaxOrders(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->MaxOrders;
    }

    inline int  MaxShares(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->MaxShares;
    }

    inline long MaxTotalShares(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->MaxTotalShares;
    }

    inline double Balance(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->Balance;
    }

    inline int NumOrders(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->NumOrders;
    }

    inline long NumShares(char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        return pLimits->NumShares;
    }

    inline void SetBuyingPower(double value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        pLimits->BuyingPower = value;
    }

    inline void SetMaxDollars(double value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        pLimits->MaxDollars = value;
    }

    inline void SetMaxOrders(int value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        pLimits->MaxOrders = value;
    }

    inline void SetMaxShares(long value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        pLimits->MaxShares = value;
    }

    inline void SetMaxTotalShares(long value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        pLimits->MaxTotalShares = value;
    }

    inline void UpdateBalance(double value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);

        // if (pLimits == &m_OptionLimits)
        // {
        //   value *= 100;
        // }

        pLimits->Balance += value;
    }

    inline void UpdateNumOrders(int value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        pLimits->NumOrders += value;
    }

    inline void UpdateNumShares(long value, char secType)
    {
        tLimits* pLimits = GetLimits(secType);
        pLimits->NumShares += value;
    }

    inline void SetEquityLimits(const tLimits& limits)
    {
        m_EquityLimits = limits;
    }

    inline void SetOptionLimits(const tLimits& limits)
    {
        m_OptionLimits = limits;
    }

    inline void SetFutureLimits(const tLimits& limits)
    {
        m_FutureLimits = limits;
    }

    inline void SetCurrencyLimits(const tLimits& limits)
    {
        m_CurrencyLimits = limits;
    }

private:
    std::string m_Trader;
    std::string m_Password;
    std::string m_Group;
    std::string m_ReportingFirm;
    std::string m_OtherTraders;
    int m_NumAccounts;
    bool m_PrincipalAbility;
    bool m_DiscretionaryAbility;
    bool m_Exempt;
    int	 m_ReportingType;
    bool m_RoninMember;

    tStringSet m_SubTraderSet;

    tLimits m_OptionLimits;
    tLimits m_FutureLimits;
    tLimits m_EquityLimits;
    tLimits m_CurrencyLimits;
};

class CThreadTraderData
{
public:
    CThreadTraderData(CTraderData* pData);
    ~CThreadTraderData();

    void UpdateBalance(const tBalanceUpdate& update);
    void SetBalance(const tBalanceUpdate& update);
    void GetBalance(tBalanceUpdate& update);
    void GetTraderData(CTraderData* pData);
    void ExemptTrader(bool exempt);
    void GetPassword(std::string& pwd);
    void UpdatePassword(const std::string& pwd);
    bool CanTradeFor(const std::string& tradeFor);
    bool ExceedLimit(const CRomOrderNew& order, tBalanceUpdate& BUpdate, std::string& error);
    bool ExceedLimit_Cplx(CRomOrderNew& order, const vector<CCsv*>* pLegs, tBalanceUpdate& BUpdate, std::string& error);
    bool ExceedLimit(const tBalanceUpdate& change, std::string& error);
    void GetDropInfo(tDropInfo& dropInfo);
    bool Exempted();
    const std::string Trader()
    {
        return m_data.Trader();
    }

private:
    CTraderData m_data;
    pthread_rwlock_t m_lock;
};

class CClientProfileManager
{
public:

    CClientProfileManager();
    ~CClientProfileManager();

    static CClientProfileManager & getInstance()
    {
        static CClientProfileManager theClientProfileManager;
        return theClientProfileManager;
    }

    static void* ReloadProfileFromDB(void* pArg = NULL);
    static void* ReloadClearingActSecLimits(void* pArg = NULL);

    bool GetPassword(const std::string& login, std::string& password);
    bool UpdatePassword(const std::string& login, std::string& password);
    bool ValidateLogin(const std::string& login, const std::string& password, std::string& err);
    void GetLoginList(ROMTYPEDEF::tLoginList& rLoginList);
    bool GetTraderData(const std::string& trader, CTraderData* pData);
    bool ExemptTrader(std::string trader, bool action);
    bool UpdateBalance(const std::string& trader, const tBalanceUpdate& update);
    bool UpdateClearingActSecBalance(const std::string& key, const tBalanceUpdate& update);

    bool LoadProfileFromDB(const std::string* pTrader);
    bool LoadProfilesFromDB(bool reload = false);

    bool AllowedToPlaceOrder(const CRomOrderNew& order, tBalanceUpdate& pBUpdate, std::string& error);
    bool AllowedToPlaceOrder_UserDefinedSpread(const CRomOrderNew& order, const vector<CCsv*>* pLegs, tBalanceUpdate& pBUpdate, std::string& error);
    //bool AllowedToReplaceOrder(const CRomOrder& order, tBalanceUpdate* pBUpdate, std::string& error);
    bool AllowedToReplaceOrder(tKeyedBalanceUpdate& update, std::string& error);
    void BackOutReplace(const CRomOrderNew& order);
    bool GetDropInfo(const std::string& trader, tDropInfo& dropInfo);
    bool GetClearingActSecLimits(const std::string& ClearingActSecKey, tLimits& limits);
    void ReloadClearingActSecLimits(const std::string* key = NULL);

private:

    CRomMap<CThreadTraderData*> m_TraderToTraderData;
    CRomMap<std::string> m_LoginToPassword;
    CClearingActLimits m_ClearingActSecLimits;
    pthread_rwlock_t m_lock;
};


#endif //__CLIENTPROFILEMANAGER_H__
